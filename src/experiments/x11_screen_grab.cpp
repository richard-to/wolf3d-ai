#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace cv;

const char kWindowDOSBox[] = "DOSBox";
const char kWindowTitle[] = "Wolf3d Screenshot";
const char kActiveWindows[] = "_NET_CLIENT_LIST";

// Find the DOSBox Window so we can do cool stuff with it!
bool findDOSBoxWindow(Display *display, Window &window)
{
    Atom actualType;
    int format;
    unsigned long numItems;
    unsigned long bytesAfter;

    bool found = false;
    Window rootWindow = RootWindow(display, DefaultScreen(display));
    Atom atom = XInternAtom(display, kActiveWindows, true);
    
    unsigned char *data = '\0';
    Window *list;    
    char *windowName;

    int status = XGetWindowProperty(display, rootWindow, atom, 0L, (~0L), false,
        AnyPropertyType, &actualType, &format, &numItems, &bytesAfter, &data);
    list = (Window *)data;
    
    if (status >= Success && numItems) {
        for (unsigned long i = 0; i < numItems; ++i) {
            status = XFetchName(display, list[i], &windowName);
            if (status >= Success) {
                string windowNameStr(windowName);
                if (windowNameStr.find(kWindowDOSBox) == 0) {
                    window = list[i];
                    found = true;
                    break;
                }
            }
        }
    }

    XFree(windowName);
    XFree(data);

    return found;
}

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay(NULL); 
    Window DOSBoxWindow;

    XWindowAttributes DOSBoxWindowAttributes;
    if (findDOSBoxWindow(display, DOSBoxWindow) == false) {
        printf("Error: Cannot find DOSBox window. Exiting program.");
        return 0;
    }

    XGetWindowAttributes(display, DOSBoxWindow, &DOSBoxWindowAttributes);
  
    int width = DOSBoxWindowAttributes.width;
    int height = DOSBoxWindowAttributes.height;

    namedWindow(kWindowTitle, WINDOW_AUTOSIZE);    
    
    Mat frame = Mat::zeros(height, width, CV_8UC3);
    Vec3b frameRGB;

    XColor colors;
    XImage *image;
    
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    
    while (true) {
        image = XGetImage(
            display, DOSBoxWindow, 0, 0, width, height, AllPlanes, ZPixmap);

        red_mask = image->red_mask;
        green_mask = image->green_mask;
        blue_mask = image->blue_mask;

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                colors.pixel = XGetPixel(image, j, i);
                frameRGB = frame.at<Vec3b>(i, j);            
                frameRGB.val[0] = colors.pixel & blue_mask;
                frameRGB.val[1] = (colors.pixel & green_mask) >> 8;
                frameRGB.val[2] = (colors.pixel & red_mask) >> 16;       
                frame.at<Vec3b>(i, j) = frameRGB;
            }
        }
        
        XFree(image);

        imshow(kWindowTitle, frame);
        
        if (waitKey(10) >= 0) {
            break;
        }
    }
}

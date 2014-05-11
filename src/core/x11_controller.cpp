#include "core/x11_controller.h"

const char kActiveWindows[] = "_NET_CLIENT_LIST";
const char kWindowDosBox[] = "DOSBox";

bool wolf3d::findDOSBoxWindow(Display *display, Window &window, const char windowNamePrefix[])
{
    Window rootWindow = RootWindow(display, DefaultScreen(display));
    Atom atom = XInternAtom(display, kActiveWindows, true);    
    Atom actualType;    
    int format;
    unsigned long numItems;
    unsigned long bytesAfter;
    unsigned char *data = '\0';
       
    int status = XGetWindowProperty(display, rootWindow, atom, 0L, (~0L), false,
        AnyPropertyType, &actualType, &format, &numItems, &bytesAfter, &data);
    Window *list = (Window *)data;
 
    char *windowName;
    bool found = false;

    if (status >= Success && numItems) {
        for (unsigned long i = 0; i < numItems; ++i) {
            status = XFetchName(display, list[i], &windowName);
            if (status >= Success && windowName != NULL) {
                string windowNameStr(windowName);
                if (windowNameStr.find(windowNamePrefix) == 0) {
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

bool wolf3d::findDOSBoxWindow(Display *display, Window &window)
{
    return wolf3d::findDOSBoxWindow(display, window, kWindowDosBox);
}

void wolf3d::grab_screen(Display *display, Window window, Mat &frame)
{
    int height = frame.rows;
    int width = frame.cols;

    Vec3b frameRGB;

    XColor colors;
    XImage *image;

    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    
    image = XGetImage(
        display, window, 0, 0, width, height, AllPlanes, ZPixmap);

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
    XDestroyImage(image);
}

void wolf3d::send_move(Display *display, Window rootWindow, Window window, KeySym keysym, int duration) {
    XKeyEvent event;
    event.type = KeyPress;      
    event.display = display;
    event.send_event = False;
    event.window = window;
    event.root = rootWindow;
    event.time = CurrentTime;
    event.same_screen = True;
    event.keycode = XKeysymToKeycode(display, keysym);
    XSendEvent(display, window, True, KeyPressMask, (XEvent *)&event);
    XFlush(display);

    millisleep(duration);

    event.type = KeyRelease;      
    XSendEvent(display, window, True, KeyReleaseMask, (XEvent *)&event);
    XFlush(display);
}
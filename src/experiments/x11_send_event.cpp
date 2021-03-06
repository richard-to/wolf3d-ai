#include <unistd.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

using namespace cv;

const char kWindowDOSBox[] = "DOSBox";
const char kWindowTitle[] = "Wolf3d Screenshot";
const char kActiveWindows[] = "_NET_CLIENT_LIST";

// Find the DOSBox Window so we can do cool stuff with it!
bool findDOSBoxWindow(Display *display, Window &window)
{
    bool found = false;
    Window rootWindow = RootWindow(display, DefaultScreen(display));
    Atom atom = XInternAtom(display, kActiveWindows, true);
    Atom actualType;
    int format;
    unsigned long numItems;
    unsigned long bytesAfter;
    
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

void millisleep(int ms)
{
    usleep(ms * 1000);
}

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay(NULL);
    Window rootWindow = RootWindow(display, DefaultScreen(display));    
    Window DOSBoxWindow;

    if (findDOSBoxWindow(display, DOSBoxWindow) == false) {
        printf("Error: Cannot find DOSBox window. Exiting program.");
        return 0;
    }

    XKeyEvent event;
    event.type = KeyPress;      
    event.display = display;
    event.send_event = False;
    event.window = DOSBoxWindow;
    event.root = rootWindow;
    event.time = CurrentTime;
    event.same_screen = True;
    event.keycode = XKeysymToKeycode(display, XK_Up);
    XSendEvent(display, DOSBoxWindow, True, KeyPressMask, (XEvent *)&event);
    XFlush(display);

    millisleep(100);

    event.type = KeyRelease;      
    XSendEvent(display, DOSBoxWindow, True, KeyReleaseMask, (XEvent *)&event);
    XFlush(display);
}

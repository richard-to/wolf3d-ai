#ifndef WOLF3D_CORE_X11_CONTROLLER_H
#define WOLF3D_CORE_X11_CONTROLLER_H

#include "opencv2/imgproc/imgproc.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "core/util.h"

using namespace cv;

namespace wolf3d
{
    bool findDOSBoxWindow(Display *display, Window &window, const char windowNamePrefix[]);
    bool findDOSBoxWindow(Display *display, Window &window);
    void send_move(Display *display, Window rootWindow, Window window, KeySym keysym, int duration);
    void grab_screen(Display *display, Window window, Mat &frame);
}

#endif
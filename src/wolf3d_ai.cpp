#include <unistd.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "core/util.h"
#include "core/x11_controller.h"
#include "core/localize.h"
#include "core/locate_doors.h"
#include "core/locate_enemies.h"

using namespace cv;
using namespace std;
using namespace wolf3d;

const char kWindowTitle[] = "Wolf3d Screenshot";
const int kNumMeasurements = 16;
const double kDoorRealHeight = 450.0;
const int kDoorViewport = 301;
const int kMeasureDelay = 185;
const int kCoexistTableDim = 500;

enum AI_MODE { LOCALIZE, FIND_DOOR, GOTO_DOOR, STOP, FIND_ENEMY, ATTACK_ENEMY };

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay(NULL);
    Window rootWindow = RootWindow(display, DefaultScreen(display));    
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
    Mat frame_prev; 
    Mat frame_doors = Mat::zeros(frame.rows, frame.cols, CV_8U);    
    Mat frame_enemy = Mat::zeros(frame.rows, frame.cols, CV_8U);

    COM_Meta com_meta;
    AI_MODE mode = LOCALIZE;
    
    double measurement = 0;
    double measurements[kNumMeasurements];
    double distance[kNumMeasurements/2];
    int measurement_count = 0;

    printf("Localizing...\n");

    grab_screen(display, DOSBoxWindow, frame);
    frame_prev = frame.clone();

    while (true) {

        grab_screen(display, DOSBoxWindow, frame);

        if (mode != FIND_ENEMY && mode != ATTACK_ENEMY) {
            for (int i = 329; i < 390; ++i) {
                for (int j = 333; j < 406; ++j) {
                    if (frame.at<uchar>(i, j) != frame_prev.at<uchar>(i, j)) {
                        mode = FIND_ENEMY;
                        printf("An enemy is attacking...\n");                        
                        break;
                    }
                }
                if (mode == FIND_ENEMY) {
                    break;
                }
            }
        }

        if (mode == LOCALIZE) {
            Mat frame_out;
            measurement = measure_wall(frame, frame_out);            
            send_move(display, rootWindow, DOSBoxWindow, XK_Right, kMeasureDelay);
            if (measurement == kDoorViewport) {
                measurement = kDoorRealHeight;
            } else if (measurement == 0) {
                measurement = 1;
            }
            measurements[measurement_count] = kDoorRealHeight / measurement;
            measurement_count++;
            if (measurement_count == kNumMeasurements) {
                measurement_count = 0;
                mode = FIND_DOOR;

                printf("Distance Measurements: ");
                for (int i = 0; i < kNumMeasurements / 2; ++i) {
                    distance[i] = (measurements[i] + measurements[i + kNumMeasurements / 2]) - 1;
                }

                for (int i = 0; i < kNumMeasurements / 2; ++i) {
                    printf("%.1f ", distance[i]);
                }
                printf("\n");

                int delay = rand() % 100 + 1;
                send_move(display, rootWindow, DOSBoxWindow, XK_Left, delay);
                send_move(display, rootWindow, DOSBoxWindow, XK_Up, 100);

                printf("Searching for doors...\n");
            }

            imshow(kWindowTitle, frame_out);
        } else if (mode == FIND_DOOR || mode == GOTO_DOOR) {
            hsv_threshold_doors(frame, frame_doors);
        
            vector<int> labels(kCoexistTableDim, 0); 
            Mat frame_label = Mat::zeros(height, width, CV_8UC3);
            Mat frame_com = frame.clone();
            label_objects(frame_doors, frame_label, labels);

            bool found_door = false;
            for (int i = 0; i < kCoexistTableDim; ++i) {
                if (labels[i] > 200) {
                    if (com_doors(frame_label, frame_com, i, &com_meta)) {
                        found_door = true;
                        break;
                    }
                }    
            }

            if (mode == FIND_DOOR) {
                if (found_door == false) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Left, 100);
                } else {
                    int delay = rand() % 100 + 1;
                    if (delay > 70) {
                        printf("Found a door. Heading to door...\n");                        
                        mode = GOTO_DOOR;
                    } else {
                        printf("Found a door, but trying another door...\n");                        
                    }
                }
            } else if (mode == GOTO_DOOR) {
                if ((com_meta.bottom - com_meta.top) > 290) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_space, 50);
                    millisleep(2000);
                    send_move(display, rootWindow, DOSBoxWindow, XK_Up, 600);                
                    millisleep(4000);
                    mode = LOCALIZE;
                    printf("Localizing...\n");
                } else if (com_meta.xbar > (frame.cols / 2) - 30 && com_meta.xbar < (frame.cols / 2) + 30) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Up, 50);
                } else if (com_meta.xbar > frame.cols / 2) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Right, 50);    
                } else if (com_meta.xbar < frame.cols / 2) {
                   send_move(display, rootWindow, DOSBoxWindow, XK_Left, 50);    
                }
            }
            imshow(kWindowTitle, frame_com);
        } else if (mode == FIND_ENEMY || mode == ATTACK_ENEMY) {
            hsv_threshold_enemies(frame, frame_enemy);

            Mat dilatedTemp;
            Mat dilated;
            Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));
            dilate(frame_enemy, dilatedTemp, element);
            dilate(dilatedTemp, dilated, element);

            vector<int> labels(kCoexistTableDim, 0); 
            Mat frame_label = Mat::zeros(height, width, CV_8UC3);
            Mat frame_com = frame.clone();
            label_objects(dilated, frame_label, labels);

            bool found_enemy = false;
            for (int i = 0; i < kCoexistTableDim; ++i) {
                if (labels[i] > 200) {
                    if (com_enemies(frame_label, frame_com, i, &com_meta)) {
                        found_enemy = true;
                        break;
                    }
                }    
            }

            if (mode == FIND_ENEMY) {
                if (found_enemy == false) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Left, 150);
                } else {
                    printf("Found an enemy. Attacking enemy...\n");
                    mode = ATTACK_ENEMY;
                }
            } else if (mode == ATTACK_ENEMY) {
                if (com_meta.xbar > (frame.cols / 2) - 30 && com_meta.xbar < (frame.cols / 2) + 30) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Control_L, 50);
                } else if (com_meta.xbar > frame.cols / 2) {
                    send_move(display, rootWindow, DOSBoxWindow, XK_Right, 50);    
                } else if (com_meta.xbar < frame.cols / 2) {
                   send_move(display, rootWindow, DOSBoxWindow, XK_Left, 50);    
                } else {
                    mode = LOCALIZE;
                }
            }
            imshow(kWindowTitle, frame_com);
        }
        
        frame_prev = frame.clone();

        if (waitKey(10) >= 0) {
            break;
        }
    }
}

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "wolf3d_ai_util.h"

#define WINDOW_NAME "Hough Door"
#define DOOR_IMAGE "images/scene1.png"

#define CEILING 56
#define FLOOR 113

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);

    Mat src = imread(DOOR_IMAGE);
    Mat dst;

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);    
    Mat out = Mat::zeros(src.rows, src.cols, CV_8U);    

    int h = gray.rows - GAME_H_END;
    int w = gray.cols - GAME_W_END;
    cout << h << " " << w << endl;
    for (int i = GAME_H_BEGIN; i < h; ++i) {
        for (int j = GAME_W_BEGIN; j < w; ++j) {
            if (gray.at<uchar>(i, j) == CEILING) {
                out.at<uchar>(i, j) = 0;           
            } else if (gray.at<uchar>(i, j) == FLOOR)  {
                out.at<uchar>(i, j) = 0;
            } else {
                out.at<uchar>(i, j) = 255;
            }
        }
    }

    Canny(out, dst, 50, 200, 3);
    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/2, 75, 50, 5 );

    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        line(src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    imshow(WINDOW_NAME, src);
    waitKey(0);

    imshow(WINDOW_NAME, dst);
    waitKey(0);
}
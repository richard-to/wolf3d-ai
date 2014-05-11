#include <iostream>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "wolf3d_ai_util.h"

#define WINDOW_NAME "Segmented Door"
#define DOOR_IMAGE "images/scene12.png"

#define CEILING 56
#define FLOOR 113

using namespace cv;
using namespace std;

#include "wolf3d_ai_util.h"

int main(int argc, char *argv[])
{
    namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);

    Mat src = imread(DOOR_IMAGE);
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);    
    Mat out = Mat::zeros(src.rows, src.cols, CV_8U);    

    int h = gray.rows - GAME_H_END;
    int w = gray.cols - GAME_W_END;

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
    Mat dilated;
    Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));
    dilate(out, dilated, element);
    
    int xbar = gray.cols/2;
    cout << gray.rows/2 << endl;
    bool found_top = false;
    bool found_bottom = false;
    int ytop = (h - GAME_H_BEGIN)/2 + GAME_H_BEGIN + 1;
    int ybottom = (h - GAME_H_BEGIN)/2 + GAME_H_BEGIN  - 1;
    while (found_top == false || found_bottom == false) {
        if (ytop == GAME_H_BEGIN || dilated.at<uchar>(ytop, xbar) == 0) {
            found_top = true;
            cout << "TOP: " << ytop << endl;
        } else if (found_top == false) {
            --ytop;
        }
        if (ybottom == h || dilated.at<uchar>(ybottom, xbar) == 0) {
            found_bottom = true;
            cout << "BOT: " << ybottom << endl;
        } else if (found_bottom == false) {
            ++ybottom;
        }
    }
    cout << ybottom - ytop << endl;


    line(src,
        Point(xbar, ytop),
        Point(xbar, ybottom),
        COM_LINE_COLOR,
        1);


    imshow(WINDOW_NAME, src);
    waitKey(0);

    imshow(WINDOW_NAME, gray);
    waitKey(0); 

    imshow(WINDOW_NAME, out);
    waitKey(0);

    imshow(WINDOW_NAME, dilated);
    waitKey(0);

}
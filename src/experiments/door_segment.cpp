#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const char kWindowName[] = "Segmented Door";
const char kDoorImage[] = "images/scene4.png";
const int kThresholdLow = 100;
const int kThresholdHigh = 110;
const int kDoorRThresh = 135;
const int kDoorGThresh = 135;
const int kDoorBThresh = 20;
const int kCeilThresh = 56;
const int kFloorThresh = 113;
const int kVarThresh = 40;

int main(int argc, char *argv[])
{
    namedWindow(kWindowName, WINDOW_AUTOSIZE);

    Mat src = imread(kDoorImage);
    Mat gray;
    Mat thresh = Mat::zeros(src.rows, src.cols, CV_8U);  
    Mat thresh_rgb = src.clone();
    Vec3b rgb;

    cvtColor(src, gray, CV_RGB2GRAY);

    for (int i = 0; i < gray.rows; ++i) {
        for (int j = 0; j < gray.cols; ++j) {
            if (gray.at<uchar>(i, j) > kThresholdLow && gray.at<uchar>(i, j) < kThresholdHigh) {
               thresh.at<uchar>(i, j) = 255;           
            } else {
                thresh.at<uchar>(i, j) = 0;
            }
        }
    }

    for (int i = 0; i < thresh_rgb.rows; ++i) {
        for (int j = 0; j < thresh_rgb.cols; ++j) {
            rgb = src.at<Vec3b>(i, j);
            if (rgb[0] == kCeilThresh) {
                rgb[0] = 0;
                rgb[1] = 0;
                rgb[2] = 0;
                thresh_rgb.at<Vec3b>(i, j) = rgb;
            } else if (rgb[0] == kFloorThresh) {
                rgb[0] = 0;
                rgb[1] = 0;
                rgb[2] = 0;
                thresh_rgb.at<Vec3b>(i, j) = rgb;
            } else if (rgb[0] > kDoorRThresh - kVarThresh && 
                rgb[0] < kDoorRThresh + kVarThresh &&
                rgb[1] > kDoorGThresh - kVarThresh && 
                rgb[1] < kDoorGThresh + kVarThresh &&
                rgb[2] > kDoorBThresh - kVarThresh && 
                rgb[2] < kDoorBThresh + kVarThresh) {              
                rgb[0] = 255;
                rgb[1] = 255;
                rgb[2] = 255;                
                thresh_rgb.at<Vec3b>(i, j) = rgb;
            } else {
                rgb[0] = 0;
                rgb[1] = 0;
                rgb[2] = 0;                
                thresh_rgb.at<Vec3b>(i, j) = rgb;
            }
        }
    }

    imshow(kWindowName, src);
    waitKey(0);

    imshow(kWindowName, gray);
    waitKey(0);

    imshow(kWindowName, thresh);
    waitKey(0); 

    imshow(kWindowName, thresh_rgb);
    waitKey(0);     
}
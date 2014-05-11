#include <iostream>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "core/localize.h"

using namespace cv;
using namespace std;
using namespace wolf3d;

const char kWindowName[] = "Segmented Door";
const char kDoorImage[] = "images/scene12.png";

const int kCeil = 56;
const int kFloor = 113;

const int kGameHBegin = 10;
const int kGameHEnd = 89;
const int kGameWBegin = 15;
const int kGameWEnd = 18;

const Scalar kComLineColor = Scalar(0, 255, 255);

int main(int argc, char *argv[])
{
    namedWindow(kWindowName, WINDOW_AUTOSIZE);

    Mat src = imread(kDoorImage[] =);
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);    
    Mat out = Mat::zeros(src.rows, src.cols, CV_8U);    

    int h = gray.rows - kGameHEnd;
    int w = gray.cols - kGameWEnd;

    for (int i = kGameHBegin; i < h; ++i) {
        for (int j = kGameWBegin; j < w; ++j) {
            if (gray.at<uchar>(i, j) == kCeil) {
                out.at<uchar>(i, j) = 0;           
            } else if (gray.at<uchar>(i, j) == kFloor)  {
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
    int ytop = (h - kGameHBegin) / 2 + kGameHBegin + 1;
    int ybottom = (h - kGameHBegin) / 2 + kGameHBegin  - 1;
    while (found_top == false || found_bottom == false) {
        if (ytop == kGameHBegin || dilated.at<uchar>(ytop, xbar) == 0) {
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
        kComLineColor,
        1);


    imshow(kWindowName, src);
    waitKey(0);

    imshow(kWindowName, gray);
    waitKey(0); 

    imshow(kWindowName, out);
    waitKey(0);

    imshow(kWindowName, dilated);
    waitKey(0);

}
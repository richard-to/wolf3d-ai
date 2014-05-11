#include "core/localize.h"

const int kDoorHueMin = 82;
const int kDoorHueMax = 92;

const int kGameHBegin = 10;
const int kGameHEnd = 89;
const int kGameWBegin = 15;
const int kGameWEnd = 18;

const int kWhite8Bit = 255;
const int kBlack8Bit = 0;

const double kDoorHistPercentMin = 0.05;
const double kDoorHistPercentMax = 0.18;
const double kDoorDimDiffRatioMin = 0.50;

const int kCeil = 56;
const int kFloor = 113;

const Scalar kComLineColor = Scalar(0, 255, 255);

int wolf3d::measure_wall(Mat frame, Mat &frame_out)
{
    int h = frame.rows - kGameHEnd;
    int w = frame.cols - kGameWEnd;

    frame_out = frame.clone();

    Mat dilated;
    Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));
    Mat frame_bin = Mat::zeros(frame.rows, frame.cols, CV_8U);
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

    for (int i = kGameHBegin; i < h; ++i) {
        for (int j = kGameWBegin; j < w; ++j) {
            if (frame_gray.at<uchar>(i, j) == kCeil) {
                frame_bin.at<uchar>(i, j) = kBlack8Bit;           
            } else if (frame_gray.at<uchar>(i, j) == kFloor)  {
                frame_bin.at<uchar>(i, j) = kBlack8Bit;
            } else {
                frame_bin.at<uchar>(i, j) = kWhite8Bit;
            }
        }
    }

    dilate(frame_bin, dilated, element);
    
    int xbar = frame_gray.cols / 2 + 50;
    bool found_top = false;
    bool found_bottom = false;

    int ytop = (h - kGameHBegin) / 2 + kGameHBegin + 1;
    int ybottom = (h - kGameHBegin) / 2 + kGameHBegin  - 1;
    
    while (found_top == false || found_bottom == false) {
        if (ytop == kGameHBegin || dilated.at<uchar>(ytop, xbar) == 0) {
            found_top = true;
        } else if (found_top == false) {
            --ytop;
        }
        if (ybottom == h || dilated.at<uchar>(ybottom, xbar) == 0) {
            found_bottom = true;
        } else if (found_bottom == false) {
            ++ybottom;
        }
    }

    line(frame_out,
        Point(xbar, ytop),
        Point(xbar, ybottom),
        kComLineColor,
        2);

    return ybottom - ytop;
}

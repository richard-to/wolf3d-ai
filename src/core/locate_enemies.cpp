#include "core/locate_enemies.h"

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

const Scalar kComBoxColor = Scalar(0, 255, 255);
const Scalar kComLineColor = Scalar(0, 255, 255);

void hsv_threshold_enemies(Mat frame, Mat &out)
{
    Vec3b hsv;    
    Mat img = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    Mat img_hsv;
    cvtColor(frame, img_hsv, COLOR_BGR2HSV);

    int h = img.rows - kGameHEnd;
    int w = img.cols - kGameWEnd;

    for (int i = kGameHBegin; i < h; ++i) {
        for (int j = kGameWBegin; j < w; ++j) {
            hsv = img_hsv.at<Vec3b>(i, j);
            if (hsv[0] > 10 && hsv[0] < 20) {
                out.at<uchar>(i, j) = kWhite8Bit;
            } else {
                out.at<uchar>(i, j) = kBlack8Bit;
            }
        }
    }
}

bool com_enemy(Mat labelled, Mat &out, int label, wolf3d::COM_Meta *meta)
{
    int h = labelled.rows;
    int w = labelled.cols;

    int top_i = 0;
    int bottom_i = 0;
    int left_j = 0;
    int right_j = 0;

    int i_bar = 0;
    int j_bar = 0;

    int i = 0;
    int j = 0;

    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            if (labelled.at<uchar>(i, j) == label) {
                top_i = i;
                break;
            }
        }
        if (top_i > 0) {
            break;
        }
    }

    for (i = h - 1; i >= 0; --i) {
        for (j = w - 1; j >= 0; --j) {
            if (labelled.at<uchar>(i, j) == label) {
                bottom_i = i;
                break;
            }
        }
        if (bottom_i > 0) {
            break;
        }
    }

    for (j = 0; j < w; ++j) {
        for (i = 0; i < h; ++i) {
            if (labelled.at<uchar>(i, j) == label) {
                left_j = j;
                break;
            }
        }
        if (left_j > 0) {
            break;
        }
    }

    for (j = w - 1; j >= 0; --j) {
        for (i = h - 1; i >= 0; --i) {
            if (labelled.at<uchar>(i, j) == label) {
                right_j = j;
                break;
            }
        }

        if (right_j > 0) {
            break;
        }
    }

    i_bar = (int)(((double)bottom_i - (double)top_i) / 2.0 + (double)top_i);
    j_bar = (int)(((double)right_j - (double)left_j) / 2.0 + (double)left_j);

    int black_count = 0;
    int white_count = 0;
    for (i = top_i; i <= bottom_i; ++i) {
        for (j = left_j; j <= right_j; ++j) {
            if (labelled.at<uchar>(i, j) == 0) {
                ++black_count;
            } else {
                ++white_count;
            }    
        }
    }

    double black_percent = (double)black_count / (double)white_count;
    double box_height = bottom_i -top_i;
    double box_width = right_j - left_j;
    double dim_ratio = 0;
    if (box_width > box_height) {
        dim_ratio = box_height / box_width;
    } else {
        dim_ratio = box_width / box_height;
    }

    if (black_percent > 0.37 && black_percent < 0.63 && 
            dim_ratio > 0.62 && dim_ratio < 0.85 && 
            box_height > 30  && box_width > 30 &&
            box_height > box_width) {

        rectangle(out,
            Point(left_j, top_i),
            Point(right_j, bottom_i),
            kComBoxColor,
            2);

        line(out,
            Point(j_bar, 0),
            Point(j_bar, out.rows),
            kComLineColor,
            1);

        line(out,
            Point(0, i_bar),
            Point(out.cols, i_bar),
            kComLineColor,
            1);

        meta->top = top_i;
        meta->bottom = bottom_i;
        meta->left = left_j;
        meta->right = right_j;
        meta->xbar = j_bar;
        meta->ybar = i_bar;
        return true;
    } else {
        return false;
    }
}
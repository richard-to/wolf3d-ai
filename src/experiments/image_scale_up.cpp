#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const char kWindowTitle[] = "2x Scale Up";
const char kImage[] = "images/chugach-mtns.jpg";

int main(int argc, char *argv[])
{
    namedWindow(kWindowTitle, WINDOW_AUTOSIZE);    
    
    Mat src = imread(kImage);
    Mat scaled = Mat::zeros(src.rows * 2, src.cols * 2, CV_8UC3);
    Vec3b p0;
    Vec3b p1;
    Vec3b pInterpolated;

    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            scaled.at<Vec3b>(i * 2, j * 2) = src.at<Vec3b>(i, j);

            if (i + 1 == src.rows) {
                scaled.at<Vec3b>(i * 2 + 1, j * 2) = src.at<Vec3b>(i, j);
            } else {
                p0 = src.at<Vec3b>(i, j); 
                p1 = src.at<Vec3b>(i + 1, j);
                pInterpolated.val[0] = p0.val[0] + (p1.val[0] - p0.val[0]) * 0.5;
                pInterpolated.val[1] = p0.val[1] + (p1.val[1] - p0.val[1]) * 0.5;
                pInterpolated.val[2] = p0.val[2] + (p1.val[2] - p0.val[2]) * 0.5;
                scaled.at<Vec3b>(i * 2 + 1, j * 2)  = pInterpolated;
            }

            if (j + 1 == src.cols) {
                scaled.at<Vec3b>(i * 2, j * 2 + 1) = src.at<Vec3b>(i, j);
            } else {
                p0 = src.at<Vec3b>(i, j); 
                p1 = src.at<Vec3b>(i, j + 1);
                pInterpolated.val[0] = p0.val[0] + (p1.val[0] - p0.val[0]) * 0.5;
                pInterpolated.val[1] = p0.val[1] + (p1.val[1] - p0.val[1]) * 0.5;
                pInterpolated.val[2] = p0.val[2] + (p1.val[2] - p0.val[2]) * 0.5;
                scaled.at<Vec3b>(i * 2, j * 2 + 1)  = pInterpolated;
            }

            if (i + 1 == src.rows || j + 1 == src.cols) {
                scaled.at<Vec3b>(i * 2 + 1, j * 2 + 1) = src.at<Vec3b>(i, j);
            } else {
                p0 = src.at<Vec3b>(i, j);
                p1 = src.at<Vec3b>(i + 1, j + 1);
                pInterpolated.val[0] = p0.val[0] + (p1.val[0] - p0.val[0]) * 0.5;
                pInterpolated.val[1] = p0.val[1] + (p1.val[1] - p0.val[1]) * 0.5;
                pInterpolated.val[2] = p0.val[2] + (p1.val[2] - p0.val[2]) * 0.5;
                scaled.at<Vec3b>(i * 2 + 1, j * 2 + 1)  = pInterpolated;  
            }
        }
    }
    imshow(kWindowTitle, scaled);
    waitKey(0);
}

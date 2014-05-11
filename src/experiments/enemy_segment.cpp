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

#define WINDOW_NAME "Segmented Enemy"
#define DOOR_IMAGE "images/scene17.png"

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);

    Mat src = imread(DOOR_IMAGE);
    Mat gray = Mat::zeros(src.rows, src.cols, CV_8U);
    Mat labelled = Mat::zeros(src.rows, src.cols, CV_8U);
    Mat com = src.clone();
    Mat hsv_img;
    cvtColor(src, hsv_img, COLOR_BGR2HSV);    
    Vec3b hsv;
    vector<int> labels(COEXIST_TABLE_DIM, 0); 

    locate_enemy(src, gray);
    

    Mat dilated;
    Mat dilated2;
    Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));
    dilate(gray, dilated2, element);
    dilate(dilated2, dilated, element);    

    label_objects(dilated, labelled, labels);  

    COM_Meta com_meta;

    for (int i = 0; i < COEXIST_TABLE_DIM; ++i) {
        if (labels[i] > 200) {
            printf("COM for %d with %d points\n", i, labels[i]);
            if (draw_com_enemy(labelled, com, i, &com_meta)) {
                cout << com_meta.xbar;
            }
        }    
    }

    imshow(WINDOW_NAME, src);
    waitKey(0);

    imshow(WINDOW_NAME, gray);
    waitKey(0); 

    imshow(WINDOW_NAME, labelled);
    waitKey(0);

    imshow(WINDOW_NAME, com);
    waitKey(0);
}
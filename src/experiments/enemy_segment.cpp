#include <iostream>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "core/util.h"
#include "core/locate_enemies.h"

using namespace cv;
using namespace std;
using namespace wolf3d;

const char kWindowName[] = "Segmented Enemy";
const char kDoorImage[] = "images/scene17.png";
const int kCoexistTableDim = 500;

int main(int argc, char *argv[])
{
    namedWindow(kWindowName, WINDOW_AUTOSIZE);

    Mat src = imread(kDoorImage);
    Mat gray = Mat::zeros(src.rows, src.cols, CV_8U);
    Mat labelled = Mat::zeros(src.rows, src.cols, CV_8U);
    Mat com = src.clone();
    Mat hsv_img;
    cvtColor(src, hsv_img, COLOR_BGR2HSV);    
    Vec3b hsv;
    vector<int> labels(kCoexistTableDim, 0); 

    hsv_threshold_enemies(src, gray);
    
    Mat dilated;
    Mat dilated2;
    Mat element = getStructuringElement(MORPH_CROSS, Size(5, 5));
    dilate(gray, dilated2, element);
    dilate(dilated2, dilated, element);    

    label_objects(dilated, labelled, labels);  

    COM_Meta com_meta;

    for (int i = 0; i < kCoexistTableDim; ++i) {
        if (labels[i] > 200) {
            printf("COM for %d with %d points\n", i, labels[i]);
            if (com_enemies(labelled, com, i, &com_meta)) {
                cout << com_meta.xbar;
            }
        }    
    }

    imshow(kWindowName, src);
    waitKey(0);

    imshow(kWindowName, gray);
    waitKey(0); 

    imshow(kWindowName, labelled);
    waitKey(0);

    imshow(kWindowName, com);
    waitKey(0);
}
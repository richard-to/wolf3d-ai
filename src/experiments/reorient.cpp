#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

const char kWindowName[] = "Hough Door";
const char kDoorImage[] = "images/scene1.png";

const int kGameHBegin = 10;
const int kGameHEnd = 89;
const int kGameWBegin = 15;
const int kGameWEnd = 18;

const int kCeil = 56;
const int kFloor = 113;

int main(int argc, char *argv[])
{
    namedWindow(kWindowName, WINDOW_AUTOSIZE);

    Mat src = imread(kDoorImage);
    Mat dst;

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);    
    Mat out = Mat::zeros(src.rows, src.cols, CV_8U);    

    int h = gray.rows - kGameHEnd;
    int w = gray.cols - kGameWEnd;
    cout << h << " " << w << endl;
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

    Canny(out, dst, 50, 200, 3);
    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/2, 75, 50, 5 );

    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        line(src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    imshow(kWindowName, src);
    waitKey(0);

    imshow(kWindowName, dst);
    waitKey(0);
}
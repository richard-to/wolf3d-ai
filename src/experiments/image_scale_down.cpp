#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

const char kWindowTitle[] = "2x Scale Down";
const char kImage[] = "images/chugach-mtns.jpg";

int main(int argc, char *argv[])
{
    namedWindow(kWindowTitle, WINDOW_AUTOSIZE);    
    
    Mat src = imread(kImage);
    Mat scaled = Mat::zeros(src.rows / 2, src.cols / 2, CV_8UC3);

    for (int i = 0; i < scaled.rows; ++i) {
        for (int j = 0; j < scaled.cols; ++j) {
            scaled.at<Vec3b>(i, j) = src.at<Vec3b>(i * 2, j * 2);
        }
    }
    imshow(kWindowTitle, scaled);
    waitKey(0);
}
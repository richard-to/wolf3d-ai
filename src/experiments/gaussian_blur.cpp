#include <math.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

const char kWindowTitle[] = "Gaussian Blur";
const char kImage[] = "images/chugach-mtns.jpg";
const double kSigma = 1.0;

int main(int argc, char *argv[])
{
    namedWindow(kWindowTitle, WINDOW_AUTOSIZE);    
    
    Mat src = imread(kImage);
    Mat blur = Mat::zeros(src.rows, src.cols, CV_8UC3);

    double kernel[3][3];
    double sum = 0;
    double sigFactor = 1.0 / (2.0 * M_PI * kSigma * kSigma);

    // Calculate 3x3 kernel given sigma
    for (int x = 0; x < 3; ++x) {
        int xdist = abs(x - 1);
        int xdistsq = xdist * xdist;
        for (int y = 0; y < 3; ++y) {
            int ydist = abs(y - 1);
            int ydistsq = ydist * ydist;
            kernel[x][y] = sigFactor * exp(-((xdistsq + ydistsq) / (2.0 * kSigma * kSigma)));
            sum += kernel[x][y];
        }
    }
    
    // Normalize Sigma
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {   
            kernel[x][y] = kernel[x][y] / sum; 
        }
    }

    // Log Kernel for Sanity Check
    for (int x = 0; x < 3; ++x) {       
        printf("%f %f %f\n", kernel[x][0], kernel[x][1], kernel[x][2]);
    }

    // Apply kernel
    for (int i = 1; i < blur.rows - 1; ++i) {
        for (int j = 1; j < blur.cols - 1; ++j) {
            Vec3b p0 = src.at<Vec3b>(i - 1, j - 1);
            Vec3b p1 = src.at<Vec3b>(i - 1, j);
            Vec3b p2 = src.at<uchar>(i - 1, j + 1);

            Vec3b p3 = src.at<Vec3b>(i, j - 1);
            Vec3b p4 = src.at<Vec3b>(i, j);
            Vec3b p5 = src.at<Vec3b>(i, j + 1);

            Vec3b p6 = src.at<Vec3b>(i + 1, j - 1);
            Vec3b p7 = src.at<Vec3b>(i + 1, j);
            Vec3b p8 = src.at<Vec3b>(i + 1, j + 1);

            double r = 
                p0[0] * kernel[0][0] + p1[0] * kernel[0][1] + p2[0] * kernel[0][2] +
                p3[0] * kernel[1][0] + p4[0] * kernel[1][1] + p5[0] * kernel[1][2] +
                p6[0] * kernel[2][0] + p7[0] * kernel[2][1] + p8[0] * kernel[2][2];

            double g =
                p0[1] * kernel[0][0] + p1[1] * kernel[0][1] + p2[1] * kernel[0][2] +
                p3[1] * kernel[1][0] + p4[1] * kernel[1][1] + p5[1] * kernel[1][2] +
                p6[1] * kernel[2][0] + p7[1] * kernel[2][1] + p8[1] * kernel[2][2];

            double b =
                p0[2] * kernel[0][0] + p1[2] * kernel[0][1] + p2[2] * kernel[0][2] +
                p3[2] * kernel[1][0] + p4[2] * kernel[1][1] + p5[2] * kernel[1][2] +
                p6[2] * kernel[2][0] + p7[2] * kernel[2][1] + p8[2] * kernel[2][2];              

            Vec3b q0;
            q0[0] = r;
            q0[1] = g;
            q0[2] = b;

            blur.at<Vec3b>(i, j) = q0;
        }
    }
    imshow(kWindowTitle, blur);
    waitKey(0);
}
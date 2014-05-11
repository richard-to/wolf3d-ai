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

const char kWindowName[] = "Results";
const char kImage[] = "images/chugach-mtns.jpg";
const bool kShowGaussians = true;
const bool kShowDogs = true;
const bool kShowExtrema = true;
const double kRWeight = 0.21;
const double kGWeight = 0.71;
const double kBWeight = 0.07;
const int kKernalSize = 3;
const int kNumOctaves = 3;
const int kNumScales = 5;
const double kPreSigmaBlur = 0.5;
const double kInitialSigma = 0.7;
const double kContrastThreshold = 0.03;
const int kMax8Bit = 255;

void grayscale(Mat src, Mat &gray)
{
    gray = Mat::zeros(src.rows, src.cols, CV_64F);
    Vec3b rgb;
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            rgb = src.at<Vec3b>(i, j);
            gray.at<double>(i, j) = kRWeight * rgb[0] + kGWeight * rgb[1] + kBWeight * rgb[2];
        }
    }
}

void normalize(Mat src, Mat &norm)
{
    norm = Mat::zeros(src.rows, src.cols, CV_64F);
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            norm.at<double>(i, j) = src.at<double>(i, j) / kMax8Bit;
        }
    }
}

void scale_down_2x(Mat src, Mat &scaled)
{
    scaled = Mat::zeros(src.rows / 2, src.cols / 2, CV_64F);
    for (int i = 0; i < scaled.rows; ++i) {
        for (int j = 0; j < scaled.cols; ++j) {
            scaled.at<double>(i, j) = src.at<double>(i * 2, j * 2);
        }
    }
}

void scale_up_2x(Mat src, Mat &scaled)
{
    scaled = Mat::zeros(src.rows * 2, src.cols * 2, CV_64F);
    double p0;
    double p1;
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            scaled.at<double>(i * 2, j * 2) = src.at<double>(i, j);

            if (i + 1 == src.rows) {
                scaled.at<double>(i * 2 + 1, j * 2) = src.at<double>(i, j);
            } else {
                p0 = src.at<double>(i, j); 
                p1 = src.at<double>(i + 1, j);
                scaled.at<double>(i * 2 + 1, j * 2) = p0 + (p1 - p0) * 0.5;
            }

            if (j + 1 == src.cols) {
                scaled.at<double>(i * 2, j * 2 + 1) = src.at<double>(i, j);
            } else {
                p0 = src.at<double>(i, j); 
                p1 = src.at<double>(i, j + 1);
                scaled.at<double>(i * 2, j * 2 + 1) = p0 + (p1 - p0) * 0.5;
            }

            if (i + 1 == src.rows || j + 1 == src.cols) {
                scaled.at<double>(i * 2 + 1, j * 2 + 1) = src.at<double>(i, j);
            } else {
                p0 = src.at<double>(i, j);
                p1 = src.at<double>(i + 1, j + 1);
                scaled.at<double>(i * 2 + 1, j * 2 + 1) = p0 + (p1 - p0) * 0.5;
            }
        }
    }
}

void gaussian_blur(Mat src, Mat &blur, double sigma)
{
    blur = src.clone();

    double xdist[kKernalSize][kKernalSize] = 
    {
        {1, 1, 1},
        {0, 0, 0},
        {1, 1, 1}
    };
    
    double ydist[kKernalSize][kKernalSize] = 
    {
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1}
    };

    double kernel[kKernalSize][kKernalSize];
    double sum = 0;
    double sigFactor = 1.0 / (2.0 * M_PI * sigma * sigma);

    int x;
    int y;
    int i;
    int j;

    double p0;
    double p1;
    double p2;
    double p3;
    double p4;
    double p5;
    double p6;
    double p7;
    double p8;

    for (x = 0; x < kKernalSize; ++x) {
        for (y = 0; y < kKernalSize; ++y) {
            kernel[x][y] = sigFactor * exp(-((xdist[x][y] + ydist[x][y]) / (2.0 * sigma * sigma)));
            sum += kernel[x][y];
        }
    }
    
    for (x = 0; x < kKernalSize; ++x) {
        for (y = 0; y < kKernalSize; ++y) {   
            kernel[x][y] = kernel[x][y] / sum; 
        }
    }

    for (i = 1; i < blur.rows - 1; ++i) {
        for (j = 1; j < blur.cols - 1; ++j) {
            p0 = src.at<double>(i - 1, j - 1);
            p1 = src.at<double>(i - 1, j);
            p2 = src.at<double>(i - 1, j + 1);

            p3 = src.at<double>(i, j - 1);
            p4 = src.at<double>(i, j);
            p5 = src.at<double>(i, j + 1);

            p6 = src.at<double>(i + 1, j - 1);
            p7 = src.at<double>(i + 1, j);
            p8 = src.at<double>(i + 1, j + 1);

            blur.at<double>(i, j) = 
                p0 * kernel[0][0] + p1 * kernel[0][1] + p2 * kernel[0][2] +
                p3 * kernel[1][0] + p4 * kernel[1][1] + p5 * kernel[1][2] +
                p6 * kernel[2][0] + p7 * kernel[2][1] + p8 * kernel[2][2];
        }
    }
}

void diff_of_gaussians(Mat gauss1, Mat gauss2, Mat &dog)
{
    dog = Mat::zeros(gauss1.rows, gauss1.cols, CV_64F);
    int i;
    int j;
    for (i = 0; i < dog.rows; ++i) {
        for (j = 0; j < dog.cols; ++j) {
            dog.at<double>(i, j) = gauss2.at<double>(i, j) - gauss1.at<double>(i, j);
        }
    }
}

void scale_space_pyramid(Mat src, Mat gauss[], Mat dogs[], double sigma, int num_octaves, int num_scales)
{
    int num_images = num_octaves * num_scales;
    int dog_count = 0;
    double scale[num_scales];
    double sig_prev = 0;
    double sig_total = 0;
    int i;

    double k = pow(2.0, 1.0 / (num_scales - 3));    
    scale[0] = sigma;
    for (i = 1; i < num_scales; ++i) {
        sig_prev = pow(k, i - 1) * sigma;
        sig_total = sig_prev * k;
        scale[i] = sqrt(sig_total * sig_total - sig_prev * sig_prev);
    }        

    gaussian_blur(src, gauss[0], scale[0]);
    for (i = 1; i < num_images; ++i) {
        if (i % num_scales == 0) {    
            scale_down_2x(gauss[i - num_scales + (num_scales - 3)], gauss[i]);
        } else {
            gaussian_blur(gauss[i - 1], gauss[i], scale[i % num_scales]); 
            diff_of_gaussians(gauss[i - 1], gauss[i], dogs[dog_count++]);
        }
    }
}

void detect_extrema(Mat dogs[], Mat extremas[], int num_octaves, int num_dogs)
{  
    int dogs_per_octave = num_dogs / num_octaves;    
    Mat image_above;
    Mat image;
    Mat image_below;
    int rows;
    int cols;
    int i;
    int h;
    int w;
    double val;
    Mat extrema;
    int extrema_count = 0;
    int found = 0;
    for (i = 0; i < num_dogs; ++i) {
        if (i % dogs_per_octave == 0) {
            extrema = Mat::zeros(dogs[i].rows, dogs[i].cols, CV_64F);;
        } else if ((i + 1) % dogs_per_octave > 0) {
            rows = dogs[i].rows - 1;
            cols = dogs[i].cols - 1;
            image_above = dogs[i + 1];
            image = dogs[i];
            image_below = dogs[i - 1];
            for (h = 1; h < rows; ++h) {
                for (w = 1; w < cols; ++w) {
                    val = image.at<double>(h, w);
                    if (
                        (   
                            abs(val) < kContrastThreshold &&
                            val > image.at<double>(h - 1, w - 1) &&
                            val > image.at<double>(h - 1, w) &&
                            val > image.at<double>(h - 1, w + 1) &&
                            val > image.at<double>(h, w - 1) &&
                            val > image.at<double>(h, w + 1) &&  
                            val > image.at<double>(h + 1, w - 1) &&
                            val > image.at<double>(h + 1, w) &&
                            val > image.at<double>(h + 1, w + 1) &&

                            val > image_above.at<double>(h - 1, w - 1) &&
                            val > image_above.at<double>(h - 1, w) &&
                            val > image_above.at<double>(h - 1, w + 1) &&
                            val > image_above.at<double>(h, w - 1) &&
                            val > image_above.at<double>(h, w) &&
                            val > image_above.at<double>(h, w + 1) &&  
                            val > image_above.at<double>(h + 1, w - 1) &&
                            val > image_above.at<double>(h + 1, w) &&
                            val > image_above.at<double>(h + 1, w + 1) &&                          

                            val > image_below.at<double>(h - 1, w - 1) &&
                            val > image_below.at<double>(h - 1, w) &&
                            val > image_below.at<double>(h - 1, w + 1) &&
                            val > image_below.at<double>(h, w - 1) &&
                            val > image_below.at<double>(h, w) &&
                            val > image_below.at<double>(h, w + 1) &&  
                            val > image_below.at<double>(h + 1, w - 1) &&
                            val > image_below.at<double>(h + 1, w) &&
                            val > image_below.at<double>(h + 1, w + 1)                         
                        ) 
                            ||
                        (        
                            abs(val) < kContrastThreshold &&
                            val < image.at<double>(h - 1, w - 1) &&
                            val < image.at<double>(h - 1, w) &&
                            val < image.at<double>(h - 1, w + 1) &&
                            val < image.at<double>(h, w - 1) &&
                            val < image.at<double>(h, w + 1) &&  
                            val < image.at<double>(h + 1, w - 1) &&
                            val < image.at<double>(h + 1, w) &&
                            val < image.at<double>(h + 1, w + 1) &&

                            val < image_above.at<double>(h - 1, w - 1) &&
                            val < image_above.at<double>(h - 1, w) &&
                            val < image_above.at<double>(h - 1, w + 1) &&
                            val < image_above.at<double>(h, w - 1) &&
                            val < image_above.at<double>(h, w) &&
                            val < image_above.at<double>(h, w + 1) &&  
                            val < image_above.at<double>(h + 1, w - 1) &&
                            val < image_above.at<double>(h + 1, w) &&
                            val < image_above.at<double>(h + 1, w + 1) &&                          

                            val < image_below.at<double>(h - 1, w - 1) &&
                            val < image_below.at<double>(h - 1, w) &&
                            val < image_below.at<double>(h - 1, w + 1) &&
                            val < image_below.at<double>(h, w - 1) &&
                            val < image_below.at<double>(h, w) &&
                            val < image_below.at<double>(h, w + 1) &&  
                            val < image_below.at<double>(h + 1, w - 1) &&
                            val < image_below.at<double>(h + 1, w) &&
                            val < image_below.at<double>(h + 1, w + 1)
                        ) 
                    ) {
                        extrema.at<double>(h, w) = 1.0;

                        double dxx = 
                            (image.at<double>(h - 1, w) + image.at<double>(h - 1, w)) - 2.0 * val;

                        double dyy = 
                            (image.at<double>(h, w - 1) + image.at<double>(h, w + 1)) - 2.0 * val;

                        double dxy =
                            (image.at<double>(h - 1, w - 1) +
                                 image.at<double>(h + 1, w + 1) -
                                 image.at<double>(h + 1, w - 1) -
                                 image.at<double>(h - 1, w + 1)) / 4.0;

                        double trH = dxx + dyy;
                        double detH = dxx * dyy - dxy * dxy;
                        double R = ((10.0 + 1) * (10.0 + 1)) / 10.0;
                        double curvature_ratio = trH * trH / detH;

                        if (detH < 0 || curvature_ratio > R) {
                            extrema.at<double>(h, w) = 0;
                        } else {
                            found++;
                        }                       
                    }
                }
            }
            extremas[extrema_count++] = extrema;
        }
    }
    cout << found;
}

int main(int argc, char *argv[])
{
    namedWindow(kWindowName, WINDOW_AUTOSIZE);

    int num_images = kNumOctaves * kNumScales;
    int dog_count = kNumOctaves * (kNumScales - 1);
    int extrema_count = dog_count - (kNumOctaves * 2);

    Mat src = imread(kImage);
    Mat gray;
    Mat norm;
    Mat preblur;    
    Mat scaledUp;
    Mat gauss[num_images];    
    Mat dogs[dog_count];
    Mat extremas[dog_count - (kNumOctaves * 2)];

    grayscale(src, gray);
    normalize(gray, norm);
    gaussian_blur(norm, preblur, kPreSigmaBlur);
    scale_up_2x(preblur, scaledUp); 
    scale_space_pyramid(scaledUp, gauss, dogs, kInitialSigma, kNumOctaves, kNumScales);
    detect_extrema(dogs, extremas, kNumOctaves, dog_count);

    int i;
    if (kShowGaussians) {
        for (i = 0; i < num_images; ++i) {                
            imshow(kWindowName, gauss[i]);
            waitKey(0);          
        }
    }

    if (kShowDogs) {    
        for (i = 0; i < dog_count; ++i) {                
            imshow(kWindowName, dogs[i]);
            waitKey(0);          
        }
    }

    if (kShowGaussians) {      
        for (i = 0; i < extrema_count; ++i) {                
            imshow(kWindowName, extremas[i]);
            waitKey(0);          
        }
    }
}
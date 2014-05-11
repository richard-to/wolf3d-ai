#include "core/util.h"

const int kCoexistTableDim = 500;

const int kWhite8Bit = 255;

void wolf3d::millisleep(int ms)
{
    usleep(ms * 1000);
}

void wolf3d::label_objects(Mat in, Mat &out, vector<int> &labels) 
{
    int i;
    int j;
    int k;

    int h = in.rows - 1;
    int w = in.cols - 1;    
        
    int p5, p4, p3, p2, p0;

    int N = 0;
    int min = INT_MAX;

    int list[4] = {0, 0, 0, 0};
    int coexist[kCoexistTableDim][kCoexistTableDim];

    for (i = 0; i < kCoexistTableDim; ++i) {
        for (j = 0; j < kCoexistTableDim; ++j) {
            if (i == j) {
                coexist[i][j] = i;
            } else {
                coexist[i][j] = 0;
            }
        }
    }

    for (i = 1; i < h; ++i) {
        for (j = 1; j < w; ++j) {
            p4 = (int)out.at<uchar>(i - 1, j - 1);
            p3 = (int)out.at<uchar>(i - 1, j);
            p2 = (int)out.at<uchar>(i - 1, j + 1);

            p5 = (int)out.at<uchar>(i, j - 1);
            p0 = (int)out.at<uchar>(i, j);

            if (in.at<uchar>(i,j) == kWhite8Bit) {
                if (p2 == 0 && p3 == 0 && p4 == 0 && p5 == 0) {
                    ++N;
                    out.at<uchar>(i, j) = N;
                } else {

                    min = INT_MAX;
                    
                    list[0] = p2;
                    list[1] = p3;
                    list[2] = p4;
                    list[3] = p5;

                    for (k = 0; k < 4; ++k) {
                        if (list[k] > 0 && list[k] < min) {
                            min = list[k];
                        }
                    }

                    p0 = out.at<uchar>(i, j) = min;
                    
                    if (p0 < p2) {
                        coexist[p0][p2] = p0;
                    } else {
                        coexist[p0][p2] = p2;
                    }

                    if (p0 < p3) {
                        coexist[p0][p3] = p0;
                    } else {
                        coexist[p0][p3] = p3;
                    }

                    if (p0 < p4) {
                        coexist[p0][p4] = p0;
                    } else {
                        coexist[p0][p4] = p4;
                    }

                    if (p0 < p5) {
                        coexist[p0][p5] = p0;
                    } else {
                        coexist[p0][p5] = p5;
                    }                    
                }
            }   
        }
    }

    for (i = 0; i < kCoexistTableDim; ++i) {
        min = INT_MAX;
        for (j = 0; j < kCoexistTableDim; ++j) {
            if (coexist[i][j] != 0 && coexist[i][j] < min) {
                min = coexist[i][j];
            }
        }
        for (j = 0; j < kCoexistTableDim; ++j) {
            if (coexist[i][j] != 0) {
                coexist[i][j] = min;
            }
        }        
    }

    for (j = 0; j < kCoexistTableDim; ++j) {
        min = INT_MAX;
        for (i = 0; i < kCoexistTableDim; ++i) {
            if (coexist[i][j] != 0 && coexist[i][j] < min) {
                min = coexist[i][j];
            }
        }
        for (i = 0; i < kCoexistTableDim; ++i) {
            if (coexist[i][j] != 0) {
                coexist[i][j] = min;
            }
        }        
    } 

    for (i = 0; i < kCoexistTableDim; ++i) {
        min = INT_MAX;
        for (j = 0; j < kCoexistTableDim; ++j) {
            if (coexist[i][j] != 0 && coexist[i][j] < min) {
                min = coexist[i][j];
            }
        }
        for (j = 0; j < kCoexistTableDim; ++j) {
            if (coexist[i][j] != 0) {
                coexist[i][j] = min;
            }
        }        
    }

    for (i = 1; i < h; ++i) {
        for (j = 1; j < w; ++j) {             
            if (out.at<uchar>(i, j) > 0) {
                min = out.at<uchar>(i, j);
                for (k = 0; k < kCoexistTableDim; ++k) {
                    if (coexist[min][k] > 0) {
                        out.at<uchar>(i, j) = coexist[min][k];
                        labels[coexist[min][k]] += 1;
                        break;
                    }
                }
            }
        }
    }
}
#ifndef WOLF3D_CORE_UTIL_H
#define WOLF3D_CORE_UTIL_H

#include <unistd.h>
#include <vector>

#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

namespace wolf3d
{
    typedef struct _COM_Meta {
        int top;
        int bottom;
        int left;
        int right;
        int xbar;
        int ybar;
    } COM_Meta;

    void millisleep(int ms);
    void label_objects(Mat in, Mat &out, vector<int> &labels);
}

#endif
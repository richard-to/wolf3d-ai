#ifndef WOLF3D_CORE_LOCATE_DOORS_H
#define WOLF3D_CORE_LOCATE_DOORS_H

#include "opencv2/imgproc/imgproc.hpp"

#include "core/util.h"

using namespace cv;

namespace wolf3d
{
    void hsv_threshold_doors(Mat frame, Mat &out);
    bool com_doors(Mat labelled, Mat &out, int label, COM_Meta *meta);
}

#endif
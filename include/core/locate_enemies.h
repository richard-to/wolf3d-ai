#ifndef WOLF3D_CORE_LOCATE_ENEMIES_H
#define WOLF3D_CORE_LOCATE_ENEMIES_H

#include "opencv2/imgproc/imgproc.hpp"

#include "core/util.h"

using namespace cv;

namespace wolf3d
{
    void hsv_threshold_enemies(Mat frame, Mat &out);
    bool com_enemies(Mat labelled, Mat &out, int label, COM_Meta *meta);
}

#endif

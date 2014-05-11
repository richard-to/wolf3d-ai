#ifndef WOLF3D_CORE_LOCALIZE_H
#define WOLF3D_CORE_LOCALIZE_H

#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

namespace wolf3d
{
    int measure_wall(Mat frame, Mat &frame_out);
}

#endif
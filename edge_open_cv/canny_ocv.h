#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <cstddef>

using namespace cv;

Mat canny(Mat input,int low_threshold, int high_threshold,int kernel_size);
//can return NULL if the file is not found.
Mat canny(char* filename,int low_threshold, int high_threshold,int kernel_size);
void show_img(Mat m);

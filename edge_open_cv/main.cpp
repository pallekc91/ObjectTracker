#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;

Mat src, src_gray,gauss_blurred,dst;

//low_threshold to start filtering edges
int low_threshold = 0;
//maximum of low_threshold to run canny with
int max_lowThreshold = 100;
//kernel size to use for sobel operator
int kernel_size = 3;
//a name given to the window that runs over the given input image
char* window_name = "Edge Map";

void CannyThreshold(int, void*)
{
    blur( src_gray, gauss_blurred, Size(3,3) ); //adding gaussian blur
    Canny( gauss_blurred, gauss_blurred, low_threshold, 3 * low_threshold, kernel_size );//running canny
    dst = Scalar::all(0); //initialising the dst to store the output
    src.copyTo( dst, gauss_blurred); //copying the output to dst
    imshow( window_name, dst ); //displaying the image
}



int main(){

    //following code to read the image, please change the input file image location.
    char * imgae_locaton = "/Users/pallekc/Downloads/pallekc.jpg";
    src = imread( samples::findFile(imgae_locaton), IMREAD_COLOR );
    if(src.empty())
    {
        std::cout << "Cound not find/open image" << std::endl;
        return -1;
    }

    dst.create(src.size(), src.type()); //creating the destination 
    cvtColor(src, src_gray, COLOR_BGR2GRAY); //convering colour image to grayscale

    namedWindow( window_name, WINDOW_AUTOSIZE );    //creating a window to display result

    //running the "CannyThreshold" function and displaying the result in the window we just created with a low_threshold to max_lowthreshold limits
    createTrackbar( "Min Threshold:", window_name, &low_threshold, max_lowThreshold, CannyThreshold ); 
    CannyThreshold(0, 0);
    waitKey(0);
    return 0;
}


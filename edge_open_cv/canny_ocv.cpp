#include "canny_ocv.h"

Mat canny(Mat input,int low_threshold, int high_threshold,int kernel_size){
    Mat dst,input_grey;
    dst.create(input.size(), input.type()); //creating the destination 
    cvtColor(input, input_grey, COLOR_BGR2GRAY); //convering colour image to grayscale
    blur(input_grey, dst, Size(kernel_size,kernel_size) ); //adding gaussian blur
    Canny(dst, dst, low_threshold, high_threshold, kernel_size );//running canny
    return dst;
}

//can return NULL if the file is not found.
Mat canny(char* filename,int low_threshold, int high_threshold,int kernel_size){

    Mat input = imread( samples::findFile(filename), IMREAD_COLOR );
    if(input.empty())
    {
        std::cout << "Cound not find/open image at location " << filename << std::endl;
        Mat * ret = NULL;
        return *ret;
    }
    return canny(input,low_threshold,high_threshold,kernel_size);
}

void show_img(Mat m){
    namedWindow( "Display window", WINDOW_AUTOSIZE );  // Create a window for display.
    imshow( "Display window", m );                   // Show our image inside it.
    waitKey(0);                                          // Wait for a keystroke in the window
}

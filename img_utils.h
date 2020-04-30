#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <limits>

using namespace cv;
using namespace std;

Mat canny(Mat input,int low_threshold, int high_threshold,int kernel_size)
{
    Mat dst,input_grey;
    dst.create(input.size(), input.type()); //creating the destination 
    cvtColor(input, input_grey, COLOR_BGR2GRAY); //convering colour image to grayscale
    blur(input_grey, dst, Size(kernel_size,kernel_size) ); //adding gaussian blur
    Canny(dst, dst, low_threshold, high_threshold, kernel_size );//running canny
    return dst;
}

Rect2d lowest_loss(Mat ori, Mat target)
{
    Rect2d  res;
    double min_loss = numeric_limits<double>::infinity();
    Mat loss;
    #pragma acc parallel loop reduction(min:min_loss)
    for(int i=0;i<ori.cols-target.cols;i++){
        for(int j=0;j<ori.rows-target.rows;j++){
            Rect2d inner_mat_rect(i,j,target.cols,target.rows);
            Mat temp = ori(inner_mat_rect);
            Mat loss_mat;
            absdiff(target,temp,loss_mat);
            double loss = loss_mat.dot(loss_mat);
            if(min_loss > loss){
                min_loss = loss;
            }
        }
    }
    int i_min = 0;
    int j_min = 0;
    #pragma acc parallel loop copy(i_min,j_min)
    for(int i=0;i<ori.cols-target.cols;i++){
        for(int j=0;j<ori.rows-target.rows;j++){
            Rect2d inner_mat_rect(i,j,target.cols,target.rows);
            Mat temp = ori(inner_mat_rect);
            Mat loss_mat;
            absdiff(target,temp,loss_mat);
            double loss = loss_mat.dot(loss_mat);
            if(loss == min_loss){
                #pragma acc atomic write
                i_min = i;
                #pragma acc atomic write
                j_min = j;
            }
        }
    }
    res.x = i_min;
    res.y = j_min;
    res.height = target.rows;
    res.width = target.cols;
    cout << "returning res rect " << res << endl;
    return res;
}

Rect2d lowest_loss_old(Mat ori, Mat target)
{
   
    Rect2d  res;
    double min_loss = numeric_limits<double>::infinity();
    Mat loss;
    #pragma acc parallel loop
    for(int i=0;i<ori.cols-target.cols;i++){
        for(int j=0;j<ori.rows-target.rows;j++){
            Rect2d inner_mat_rect(i,j,target.cols,target.rows);
            Mat temp = ori(inner_mat_rect);
            Mat loss_mat;
            absdiff(target,temp,loss_mat);
            double loss = loss_mat.dot(loss_mat);
            if(min_loss > loss){
                res = inner_mat_rect;
                min_loss = loss;
            }
        }
    }
    cout << "returning res rect " << res << endl;
    return res;
}


#include "utils.h"
#include <stddef.h>
#include <iostream>
#include <math.h>


//i,j += product(image,kernel)
double ** applyKernel_add(double ** image, double** kernel,int image_size,int kernel_size){
    if(image == NULL || kernel == NULL){
        //improve logging, maybe use spdlog
        std::cout << "applyKernel has received NULL for either image or kernel" << std::endl; 
        return NULL;
    }

    double ** output;
    for(int i=0;i<image_size;i++) output = new double*[image_size];
    //can be parallelized
    for(int i=0;i<image_size;i++){
        for(int j=0;j<image_size;j++){
            output[i][j] = 0;
        }
    }

    //can be parallelized
    for(int i=0;i<image_size;i++){
        for(int j=0;j<image_size;j++){
            output[i][j] += image[i][j]*kernel[i%kernel_size][j%kernel_size];
        }
    }

    return output;
}

//has to be a square kernel with odd size
//i,j = sum(product(image,kernel))
double ** applyKernel_mul(double ** image, double** kernel,int image_size,int kernel_size){
    if(image == NULL || kernel == NULL){
        //improve logging, maybe use spdlog
        std::cout << "applyKernel has received NULL for either image or kernel" << std::endl; 
        return NULL;
    }

    double ** output;
    for(int i=0;i<image_size;i++) output = new double*[image_size];
    //can be parallelized
    for(int i=0;i<image_size;i++){
        for(int j=0;j<image_size;j++){
            output[i][j] = 0;
        }
    }

    double sum = 0;
    int mid = kernel_size/2;
    //can be parallelized
    for(int i=kernel_size/2;i<image_size-kernel_size+1;i++){
        for(int j=kernel_size/2;j<image_size-kernel_size+1;j++){
            sum = 0;
            for(int k = 0;k<kernel_size;k++){
                for(int l = 0;l<kernel_size;l++){
                    sum += image[i+k-mid][j+l-mid] * kernel[k][l];
                }
            }
        }
    }

    return output;
}

double ** sobel_kernel_3(){
    double ** output;
    int i,j;

    for(i=0;i<3;i++) output = new double*[3];
    //can be parallelized
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            output[i][j] = 0;
        }
    }

    output[0][0] = -1;
    output[0][1] = -2;
    output[0][2] = -1;
    output[1][0] = 0;
    output[1][1] = 0;
    output[1][2] = 0;
    output[2][0] = 1;
    output[2][1] = 1;
    output[2][2] = 1;

    return output;
}

double** gaussian_kernel(int size, double sigma)
{
    double ** output;
    int i,j;

    for(i=0;i<size;i++) output = new double*[size];
    //can be parallelized
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            output[i][j] = 0;
        }
    }
    double sum=0.0;

    //can be parallelized, although not necessary though
    for (i=0 ; i<size ; i++) {
        for (j=0 ; j<size ; j++) {
            output[i][j] = exp(-(i*i+j*j)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += output[i][j];
        }
    }

    for (i=0 ; i<size ; i++) {
        for (j=0 ; j<size ; j++) {
            output[i][j] /= sum;
        }
    }

    return output;
}
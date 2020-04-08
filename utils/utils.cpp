#include "utils.h"
#include <stddef.h>
#include <iostream>
#include <math.h>


void initialise_matrix(double ** output,int image_size){
    for(int i=0;i<image_size;i++) output = new double*[image_size];
    //can be parallelized
        for(int i=0;i<image_size;i++){
            for(int j=0;j<image_size;j++){
            output[i][j] = 0;
            }
        }
}

double ** get_angle_matrix(double ** x_output, double ** y_output,int image_size){
    double ** output;
    initialise_matrix(output,image_size);
     for(int i=0;i<image_size;i++){
        for(int j=0;j<image_size;j++){
            output[i][j] = atan(x_output[i][j]/y_output[i][j]);
        }
    }
    return output;
}

double ** get_intensity_matrix(double ** x_output, double ** y_output,int image_size){
    double ** output;
    double max = 0;
    initialise_matrix(output,image_size);
     for(int i=0;i<image_size;i++){
        for(int j=0;j<image_size;j++){
            double value = sqrt(pow(x_output[i][j],2) + pow(y_output[i][j],2));
            if(max < value){
                max = value;
            }
            output[i][j] = value;
        }
    }
    return output;
}


//i,j += product(image,kernel)
double ** apply_kernel_add(double ** image, double** kernel,int image_size,int kernel_size){
    if(image == NULL || kernel == NULL){
        //improve logging, maybe use spdlog
        std::cout << "apply_kernel_add has received NULL for either image or kernel" << std::endl; 
        return NULL;
    }

    double ** output;
    initialise_matrix(output,image_size);

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
double ** apply_kernel_mul(double ** image, double** kernel,int image_size,int kernel_size){
    if(image == NULL || kernel == NULL){
        //improve logging, maybe use spdlog
        std::cout << "apply_kernel_mul has received NULL for either image or kernel" << std::endl; 
        return NULL;
    }

    double ** output;
    initialise_matrix(output,image_size);

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

double ** y_sobel_kernel_3(){
    double ** output;
    int i,j;

    initialise_matrix(output,3);

    output[0][0] = -1;
    output[0][1] = 0;
    output[0][2] = 1;
    output[1][0] = -2;
    output[1][1] = 0;
    output[1][2] = 2;
    output[2][0] = -1;
    output[2][1] = 0;
    output[2][2] = 1;

    return output;
}

double ** x_sobel_kernel_3(){
    double ** output;
    int i,j;

    initialise_matrix(output,3);

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

    initialise_matrix(output,size);
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
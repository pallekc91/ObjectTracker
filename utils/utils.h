

double ** applyKernel_add(double ** image, double** kernel,int image_size,int kernel_size);
double ** applyKernel_mul(double ** image, double** kernel,int image_size,int kernel_size);
double** gaussian_kernel(int size, double sigma);
double ** sobel_kernel_3();
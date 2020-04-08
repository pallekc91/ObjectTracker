
double ** get_intensity_matrix(double ** x_output, double ** y_output,int image_size);
double ** get_angle_matrix(double ** x_output, double ** y_output,int image_size);
double ** apply_kernel_add(double ** image, double** kernel,int image_size,int kernel_size);
double ** apply_kernel_mul(double ** image, double** kernel,int image_size,int kernel_size);
double** gaussian_kernel(int size, double sigma);
double ** x_sobel_kernel_3();
double ** y_sobel_kernel_3();
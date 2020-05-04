#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "img_utils.h"

#include <chrono>
#include <iostream>
#include <mpi.h>
#include <limits>

using namespace cv;
using namespace std;

int world_size; // number of processes
int my_rank;    // my process number


int main(int argc, char **argv){

    if(argc != 4){
        std::cout << "Improper command line arguments." << std::endl;
        std::cout << "./main <location_to_input_video> <out_file_location> <min_threshold_for_canny>" << std::endl;
        return 1;
    }

    // Initialize MPI
   MPI_Init(&argc, &argv);

   // Get the number of processes
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);

   // Get the rank
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    const int nitems = 5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype mpi_result_type;
    MPI_Aint     offsets[5];

    offsets[0] = offsetof(result, loss);
    offsets[1] = offsetof(result, x);
    offsets[2] = offsetof(result, y);
    offsets[3] = offsetof(result, width);
    offsets[4] = offsetof(result, height);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_result_type);
    MPI_Type_commit(&mpi_result_type);

    char* video_location = argv[1];
    VideoCapture cap(video_location);

    int frame_height = 512;
    int frame_width = 512;
    Size frame_size(frame_width, frame_height);
    int frames_per_second = 30;

    Mat target_canny, frame, canny_out;
    int frame_count = 1;

    char* out_file = argv[2];
    VideoWriter oVideoWriter(out_file, VideoWriter::fourcc('M', 'J', 'P', 'G'), frames_per_second, frame_size, true);
    if (oVideoWriter.isOpened() == false) 
    {
        cout << "Could not open the out_file to save the video" << endl;
        return -1;
    }

    int upper_threshold = 200;
    int lower_threshold = std::stoi (argv[3],nullptr);
    if(lower_threshold > 199){
        std::cout << "improper threshold value" << std::endl;
        std::cout << "please enter value less than 200" << std::endl;
        return 1;
    }

    MPI_Request request[world_size];
    int requests;
    MPI_Status status[world_size];

    std::chrono::time_point<std::chrono::steady_clock> begin_time = std::chrono::steady_clock::now();

    cap >> frame;
    resize(frame, frame, Size(frame_height, frame_width), 0, 0, INTER_CUBIC);

    if(my_rank == 0){

        //take the input from the user of the object that he wants to track
        bool fromCenter = false;
        Rect2d main_box = selectROI(frame,fromCenter);
        cout << "x=" << main_box.x << " ,y=" << main_box.y << endl;
        cout << "height=" << main_box.height << " ,width=" << main_box.width << endl;

        //send it to all slaves
        result res;
        res.x = main_box.x;
        res.y = main_box.y;
        res.height = main_box.height;
        res.width = main_box.width;

        for(int i=1;i<world_size;i++){
            MPI_Isend(&res, 1 , mpi_result_type, i, 0, MPI_COMM_WORLD, request + requests++);
        }
        MPI_Waitall(requests, request, status);

    } else {

        //receive data from master and prepare canny_out
        result res;
        MPI_Irecv(&res, 1, mpi_result_type, 0, 0, MPI_COMM_WORLD, request + requests++);
        MPI_Waitall(requests, request, status);
        canny_out = canny(frame,lower_threshold,upper_threshold,3);
        target_canny = canny_out(Rect2d(res.x,res.y,res.width,res.height));

    }

    while(1){
        //get the next frame and resize
        MPI_Request request[2*world_size];
        requests = 0;
        MPI_Status status[2*world_size];
        cap >> frame;
       
        if (frame.empty())
            break;
        resize(frame, frame, Size(frame_height, frame_width), 0, 0, INTER_CUBIC);
        
        if(my_rank != 0){

            //run canny on the current frame
            canny_out = canny(frame,lower_threshold,upper_threshold,3);
            
            //find the position of the target_output in the current frame
            result res = lowest_loss(canny_out,target_canny,my_rank,world_size);

            //send local minimum
            MPI_Isend(&res, 1, mpi_result_type, 0, 0, MPI_COMM_WORLD, request + requests++);

            //get global minimum
            MPI_Irecv(&res, 1, mpi_result_type, 0, 0, MPI_COMM_WORLD, request + requests++);

            MPI_Waitall(requests, request, status);

            //prepare target for next frame
            target_canny = canny_out(Rect2d(res.x,res.y,res.width,res.height));

            
        } else {

            result res[world_size-1];

            for(int i=1;i<world_size;i++){
                MPI_Irecv(&res[i-1], 1, mpi_result_type, i, 0, MPI_COMM_WORLD, request + requests++);
            }

            MPI_Waitall(requests, request, status);

            double loss = numeric_limits<double>::infinity();
            result global;

            for(int i=0;i<world_size-1;i++){
                if(loss > res[i].loss){
                    global.x = res[i].x;
                    global.y = res[i].y;
                    global.width = res[i].width;
                    global.height = res[i].height;
                    loss = res[i].loss;
                }
            }

            for(int i=1;i<world_size;i++){
                MPI_Isend(&global, 1, mpi_result_type, i, 0, MPI_COMM_WORLD, request + requests++);
            }

            Rect2d pos = Rect2d(global.x,global.y,global.width,global.height);
            rectangle( frame, pos.tl(), pos.br(), Scalar(0,0,0), 10 );
            oVideoWriter.write(frame); 
            imshow("frame",frame);
            waitKey(1);
            frame_count++;
        }
    }
    //close  oVideoWriter
    oVideoWriter.release();

    std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();

    //computing the difference in seconds
    std::chrono::duration<double> difference_in_time = end_time - begin_time;
    double difference_in_seconds = difference_in_time.count();
    if(my_rank == 0)
        std::cout << "Time taken to process " << frame_count << " frame(s)  is " << difference_in_seconds << " seconds" << std::endl;

    MPI_Finalize();
    return 0;
}
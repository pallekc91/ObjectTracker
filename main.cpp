#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "img_utils.h"

#include <chrono>
#include <iostream>
#include <string>

using namespace cv;

int main(int argc, char **argv){

    if(argc != 4){
        std::cout << "Improper command line arguments." << std::endl;
        std::cout << "./main <location_to_input_video> <out_file_location> <min_threshold_for_canny>" << std::endl;
        return 1;
    }

    //open the video file
    char* video_location = argv[1];
    VideoCapture cap(video_location);

    //emitting the outfile 
    char* out_file = argv[2];

    int upper_threshold = 200;
    int lower_threshold = std::stoi (argv[3],nullptr);
    if(lower_threshold > 199){
        std::cout << "improper threshold value" << std::endl;
        std::cout << "please enter value less than 200" << std::endl;
        return 1;
    }

    //parameters for the outfile and resizing the input
    int frame_height = 1024;
    int frame_width = 1024;
    Size frame_size(frame_width, frame_height);
    int frames_per_second = 30;

    //initialising oVideoWriter and ending program if it fails
    VideoWriter oVideoWriter(out_file, VideoWriter::fourcc('M', 'J', 'P', 'G'), frames_per_second, frame_size, true);
    if (oVideoWriter.isOpened() == false) 
    {
        cout << "Could not open the out_file to save the video" << endl;
        return -1;
    }

    //recording the time before running the program 
    std::chrono::time_point<std::chrono::steady_clock> begin_time = std::chrono::steady_clock::now();

    //get the first frame a resize it
    Mat frame;
    cap >> frame;
    resize(frame, frame, Size(frame_height, frame_width), 0, 0, INTER_CUBIC);

    //take the input from the user of the object that he wants to track
    bool fromCenter = false;
    Rect2d main_box = selectROI(frame,fromCenter);
    cout << "x=" << main_box.x << " ,y=" << main_box.y << endl;
    cout << "height=" << main_box.height << " ,width=" << main_box.width << endl;

    //run canny on frame 1 and collect the target_output for the next frame
    Mat canny_out = canny(frame,lower_threshold,upper_threshold,3);
    Mat target_canny = canny_out(main_box);

    //variable to count the frames, initialised to 1 cause we start processing from frame 2
    int frame_count = 1;

    //looping for frame by frame
    while(1){
        //get the next frame and resize
        cap >> frame;
        if (frame.empty())
            break;
        resize(frame, frame, Size(frame_height, frame_width), 0, 0, INTER_CUBIC);

        //run canny on the current frame
        canny_out = canny(frame,lower_threshold,upper_threshold,3);

        //find the position of the target_output in the current frame
        Rect2d pos = lowest_loss(canny_out,target_canny);

        //display the frame with a bounding box on the identified position
        rectangle( frame, pos.tl(), pos.br(), Scalar(0,0,0), 10 );
        imshow( "frame", frame );
        waitKey(1);
        oVideoWriter.write(frame); 

        //update the target_optput for the next frame
        target_canny = canny_out(pos);
        frame_count++;
    }

    //close  oVideoWriter
    oVideoWriter.release();

    //record the time after running the program
    std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();

    //computing the difference in seconds
    std::chrono::duration<double> difference_in_time = end_time - begin_time;
    double difference_in_seconds = difference_in_time.count();
    std::cout << "Time taken to process " << frame_count << " frame(s)  is " << difference_in_seconds << " seconds" << std::endl;
}


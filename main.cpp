#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "img_utils.h"

#include <iostream>

using namespace cv;
RNG rng(12345);

int main(){

    //open the video file
    char* video_location = "./video0.mp4";
    VideoCapture cap(video_location);

    //get the first frame a resize it
    Mat frame;
    cap >> frame;
    resize(frame, frame, Size(512, 512), 0, 0, INTER_CUBIC);

    //take the input from the user of the object that he wants to track
    bool fromCenter = false;
    Rect2d main_box = selectROI(frame,fromCenter);
    cout << "x=" << main_box.x << " ,y=" << main_box.y << endl;
    cout << "height=" << main_box.height << " ,width=" << main_box.width << endl;

    //run canny on frame 1 and collect the target_output for the next frame
    Mat canny_out = canny(frame,150,200,3);
    Mat target_canny = canny_out(main_box);

    //looping for frame by frame
    while(1){
        //get the next frame and resize
        cap >> frame;
        if (frame.empty())
            break;
        resize(frame, frame, Size(512, 512), 0, 0, INTER_CUBIC);

        //run canny on the current frame
        canny_out = canny(frame,150,200,3);

        //find the position of the target_output in the current frame
        Rect2d pos = lowest_loss(canny_out,target_canny);

        //display the frame with a bounding box on the identified position
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        rectangle( frame, pos.tl(), pos.br(), color, 10 );
        imshow( "frame", frame );

        //update the target_optput for the next frame
        target_canny = canny_out(pos);
    }
}


#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2\tracking.hpp>
#include <Windows.h>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	
	Mat frame;
	int key = 0;
	/* initialize camera */
	VideoCapture cap;
	cap.open(0);

	/* always check */
	if (!cap.isOpened()) {
		printf("Cannot open initialize webcam!\n");
		exit(0);
	}

	/* create a window for the video */
	cvNamedWindow("result", CV_WINDOW_AUTOSIZE);
	
		/* get a frame */
		cap.read(frame);
		/* always check */
		if (frame.empty()) {
			cerr << "error blank frame";
			return 0;
		} 

		/* sets the Region of Interest*/
		Rect2d roi;

		//create tracker
		Ptr<Tracker> tracker = Tracker::create("KCF");

		cap >> frame;
		roi = selectROI("tracker", frame);
		
		tracker->init(frame, roi);

		while (true) {
			//get frame from the camera
			cap >> frame;

			//if the frame is empty quit
			if (frame.empty()) {
				break;
			}
			//update the tracker
			tracker->update(frame, roi);
			//note left and right are flipped
			SetCursorPos(roi.x * 3, roi.y *3);
			
			//draw the tracking square
			rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);

			//show the frame
			imshow("Live", frame);

			if (waitKey(1) == 27) {
				break;
			}
		}
		/* display current frame */
	return 0;
}


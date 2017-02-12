// Author Mark A. Gradecki


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

//to use select one ROI then press space, then select the second object and press space, 
//then press esc to start tracking, the first object that was tracked moves the mouse.
//if the distance between the two objects is less than 100 pixels then it will click the mouse.



//function to call for mouse click
void mouseLeftClick(){
	INPUT Input = { 0 };

	// left mouse button down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// left mouse button up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}


int main(int argc, char *argv[])
{
	
	Mat frame;
	Mat dst;
	int key = 0;
	String trackingAlg = "KFC";
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
		//Rect2d roi;
		vector<Rect2d> objects;

		//create tracker
		//Ptr<Tracker> tracker = Tracker::create("KCF");
		MultiTracker trackers("KCF");

		cap >> frame;

		//flip image about y-axis           
		flip(frame, dst, 1);

		//roi = selectROI("tracker", dst);
		selectROI("tracker", dst, objects);

		//tracker->init(dst, roi);
		trackers.add(dst, objects);

		while (true) {
			//get frame from the camera
			cap >> frame;

			//flip image about y-axis           
			flip(frame, dst, 1);

			//if the frame is empty quit
			if (frame.empty()) {
				break;
			}
			//update the tracker
			//tracker->update(dst, roi);
			trackers.update(dst,objects);


			SetCursorPos(objects[0].x * 3, objects[0].y * 3);
			
			//draw the tracking square
			//rectangle(dst, roi, Scalar(255, 0, 0), 2, 1);

			for (int i = 0; i < trackers.objects.size(); i++) {
				rectangle(dst, trackers.objects[i], Scalar(255, 0, 0), 2, 1);
			}
			if (objects.size() > 1) {
				//calculates the distance formula fot the two points
				int xSqs = pow((objects[1].x - objects[0].x), 2);
				int ySqs = pow((objects[1].y - objects[0].y), 2);
				int distance = sqrt((xSqs + ySqs));
				if (distance < 100) {
					//if the two tracked points is close then click
					mouseLeftClick();
				}
			}
			//show the frame
			imshow("tracker", dst);

			if (waitKey(1) == 27) {
				break;
			}
		}
		/* display current frame */
	return 0;
}


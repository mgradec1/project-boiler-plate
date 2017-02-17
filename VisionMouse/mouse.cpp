// Author Mark A. Gradecki
// feature complete 2/16/17

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
//optical tracker with hover click functionality
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

void SetCursorPosC(int x, int y, int screenX, int screenY) {
	SetCursorPos(((x -100)*4 + (screenX/2)), ((y + 60)*5 + (screenY/2)));
}


int main(int argc, char *argv[])
{
	
	Mat frame;
	Mat dst;
	int key = 0;
	String trackingAlg = "KFC";

	//get variables for screen res from the system
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	//variable used to determine if there is focus for a click
	int lastX = 0;
	int lastY = 0;
	int focusTime = 0;

	/* initialize camera */
	VideoCapture cap;
	cap.open(0);

	/* always check */
	if (!cap.isOpened()) {
		printf("Cannot open initialize webcam!\n");
		exit(0);
	}
		
		/* get a frame */
		cap.read(frame);
		//set camera res to max or the screen resolution
		cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, 600);

		

		/* always check */
		if (frame.empty()) {
			cerr << "error blank frame";
			return 0;
		} 

		/* sets the Region of Interest*/
		
		vector<Rect2d> objects;

		//create tracker
		
		MultiTracker trackers("KCF");

		cap >> frame;

		//flip image about y-axis           
		flip(frame, dst, 1);

		
		selectROI("tracker", dst, objects);

		
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
			
			trackers.update(dst,objects);

			//custom origin mouse function
			SetCursorPosC(((objects[0].x * 4) - (screenX/2)), ((objects[0].y * 2) - (screenY/2)), screenX, screenY);
			//hover click functionality
			if ((lastX - objects[0].x < 3) && (lastY - objects[0].y) < 3) {
				focusTime++;
				if (focusTime >= 60) {
					mouseLeftClick();
					focusTime = 0;
				}
				else {
					focusTime++;
				}
			}
			else {
				focusTime = 0;
			}

			//update the last coordinate
			lastX = objects[0].x;
			lastY = objects[0].y;

			//debug output
			//cout << "(" <<(objects[0].x)<<", " << (objects[0].y) << ")" << endl;

			//draw the tracking square
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

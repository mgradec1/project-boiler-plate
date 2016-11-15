#include <iostream>
#include <Windows.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect.hpp"

using namespace cv;
using namespace std;

CascadeClassifier face_cascade;
int iLastX = -1;
int iLastY = -1;
int totalY = GetSystemMetrics(SM_CXSCREEN); //get total size in pixels of screen's height
int totalX = GetSystemMetrics(SM_CYSCREEN); //get total size in pixels of screen's width

void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 2, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	
	for (size_t i = 0; i < faces.size(); i++) {
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		Mat faceROI = frame_gray(faces[i]);

		if (center.x > iLastX + 7) {
		SetCursorPos(center.x + (totalX/4), center.y);
		}
		if (center.x < iLastX - 7) {
			SetCursorPos(center.x - (totalX / 4), center.y);
		}
		if (center.y > iLastY + 7) {
			SetCursorPos(center.x, center.y + (totalY / 4));
		}
		if (center.y > iLastX - 7) {
			SetCursorPos(center.x, center.y - (totalY / 4));
		}


		iLastX = center.x;
		iLastY = center.y;
	}
}


int main(int argc, char** argv){
	VideoCapture cap(0); //capture the video from webcam
	String face_cascadeName = "C:\\OpenCV-3.1.0\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	
	bool start = true; //flag for seting the cursor to middle of screen on start

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	if (!face_cascade.load(face_cascadeName)) {
		printf("--(!)Error loading face cascade\n"); 
		return -1;
	}

	

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	
	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		
		if (start == true) {
			SetCursorPos(totalX, totalY);
			start = false;
		}

		detectAndDisplay(imgOriginal);

		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}



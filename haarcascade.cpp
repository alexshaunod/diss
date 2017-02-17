#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "haarcascade.h"

using namespace std;
using namespace cv;

/** Function Headers */
//void detectAndDisplay(Mat frame);

/** Global variables */
String body_cascade_name = "haarcascade_fullbody_alt.xml";
String upper_cascade_name = "haarcascade_upperbody.xml";

CascadeClassifier body_cascade, upper_cascade;
string window_name = "Capture - People Detection";
RNG rng(12345);

/** @function main */
int HaarCascade::run()
{

	VideoCapture capCam = VideoCapture();
	Mat frame;

	//-- 1. Load the cascades
	if (!body_cascade.load(body_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!upper_cascade.load(body_cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	//-- 2. Read the video stream
	capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
	if (capCam.isOpened())
	{
		while (true)
		{
			capCam.read(frame);

			//-- 3. Apply the classifier to the frame
			if (!frame.empty())
			{
				detectAndDisplay(frame);
			}
			else
			{
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	return 0;
}

/** @function detectAndDisplay */
void HaarCascade::detectAndDisplay(Mat frame)
{
	std::vector<Rect> people_far, people_near;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect
	body_cascade.detectMultiScale(frame, people_far, 1.15, 1, 0 | CV_HAAR_SCALE_IMAGE, Size(20,40), Size(40,70)); //far away
	upper_cascade.detectMultiScale(frame, people_near, 1.05, 1, 0 | CV_HAAR_SCALE_IMAGE, Size(60, 60), Size(200, 200)); //nearby

	for (size_t i = 0; i < people_far.size(); i++)
	{
		rectangle(frame,
			Point (people_far[i].x, people_far[i].y),
			Point (people_far[i].x + people_far[i].width, people_far[i].y + people_far[i].height),
			Scalar(0, 255, 255),
			1,
			LINE_8
		);

	}
	
	for (size_t i = 0; i < people_near.size(); i++)
	{
		rectangle(frame,
			Point(people_near[i].x, people_near[i].y),
			Point(people_near[i].x + people_near[i].width, people_near[i].y + people_near[i].height),
			Scalar(0, 0, 255),
			1,
			LINE_8
		);

	}
	// Display
	imshow(window_name, frame);
}
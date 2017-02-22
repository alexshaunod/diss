#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include "haarcascade.h"

using namespace std;
using namespace cv;

/** Function Headers */
//void detectAndDisplay(Mat frame);

/** Global variables */
String body_cascade_name = "haarcascade_fullbody_alt.xml";
String upper_cascade_name = "haarcascade_upperbody.xml";
Size farMinSize = Size(20, 40);
Size farMaxSize = Size(40, 70);
Size nearMinSize = Size(60, 60);
Size nearMaxSize = Size(200, 200);
CascadeClassifier body_cascade, upper_cascade;
string window_name = "Capture - People Detection";
RNG rng(12345);
Scalar bgrYellow = Scalar(0, 255, 255);
Scalar bgrRed = Scalar(0, 0, 255);
int i, currentfps;
VideoCapture capCam = VideoCapture();

/** @function main */
int HaarCascade::run()
{
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
	Mat frame_grey;

	//cvtColor(frame, frame_grey, CV_BGR2GRAY);
	//equalizeHist(frame_grey, frame_grey);

	// Detect
	body_cascade.detectMultiScale(frame, people_far, 1.15, 1, 0 | 1, farMinSize, farMaxSize); //far away
	upper_cascade.detectMultiScale(frame, people_near, 1.05, 1, 0 | 1, nearMinSize, nearMaxSize); //nearby

	for (i = 0; i < people_far.size(); i++)
	{
		rectangle(frame,
			Point (people_far[i].x, people_far[i].y),
			Point (people_far[i].x + people_far[i].width, people_far[i].y + people_far[i].height),
			bgrYellow,
			1,
			LINE_8
		);

	}
	
	for (i = 0; i < people_near.size(); i++)
	{
		rectangle(frame,
			Point(people_near[i].x, people_near[i].y),
			Point(people_near[i].x + people_near[i].width, people_near[i].y + people_near[i].height),
			bgrRed,
			1,
			LINE_8
		);

	}
	// Display
	imshow(window_name, frame);
}
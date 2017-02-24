#include "blobdetector.h"

int BlobDetector::run()
{
	return 0;
}

void BlobDetector::examine_frame(Mat *frame, Mat *fgmask)
{
	int i, j, minX, maxX, minY, maxY;

	minX = 108;
	maxX = 116;
	minY = 228;
	maxY = 236;
	
	for (i = minX; i < maxX; i++)
	{
		for (j = minY; j<maxY; j++)
		{
			if ((int)fgmask->at<uchar>(i, j) >= 127) {
				cout << "[" << (int)fgmask->at<uchar>(i, j) << "]";
			}
		}
		//cout << endl;
	}
	//cout << endl;

	rectangle(*fgmask,
		Point(minX, minY),
		Point(maxX, maxY),
		Scalar(255, 255, 255),
		1,
		LINE_8
	);
	//imshow("test", fgmask);
}

Mat BlobDetector::highlight_contours(Mat *frame, Mat *fgmask)
{
	Mat drawn_contours;
	RNG rng(12345);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(*fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> hull(contours.size());
	vector<vector<Vec4i>> defects(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);
	}

	drawn_contours = Mat::zeros(frame->size() , CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		drawContours(*frame,
			contours,
			i,
			Scalar(0 , 0, 255),
			1,
			8,
			hierarchy,
			0,
			Point());
		drawContours(drawn_contours,
			contours,
			i,
			Scalar(0, 0, 255),
			1,
			8,
			hierarchy,
			0,
			Point());
		drawContours(drawn_contours,
			hull,
			i,
			Scalar(255, 0, 255),
			1,
			8,
			hierarchy,
			0,
			Point());
	}
	return drawn_contours;
}


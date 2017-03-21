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

Mat BlobDetector::highlight_contours(Mat *frame, Mat *fgmask, Mat *contoursonly)
{
	int k = 0;
	Mat drawn_contours;
	RNG rng(12345);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(*fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> hull(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		if (contourArea(contours[i]) > 300) //threshold for the number of contours to be drawn, excludes smaller shapes
		{
			convexHull(Mat(contours[i]), hull[k], false);
			k++;
			//cout << contourArea(contours[i]) << endl;
		}
	}
	//cout << endl;
	drawn_contours = Mat::zeros(frame->size() , CV_8UC3);
	*contoursonly = Mat::zeros(frame->size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		draw_annotations(frame, &drawn_contours, contoursonly, contours, hierarchy, hull, i);
	}

	get_large_shapes(contoursonly, hull, k);

	return drawn_contours;
}

void BlobDetector::draw_annotations(Mat *frame, Mat *drawn_contours, Mat *contoursonly, vector<vector<Point>> contours, vector<Vec4i> hierarchy, vector<vector<Point>> hull, int i)
{
	drawContours(*frame,
		contours,
		i,
		Scalar(0, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
	drawContours(*drawn_contours,
		contours,
		i,
		Scalar(0, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
	drawContours(*contoursonly,
		contours,
		i,
		Scalar(0, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
	drawContours(*drawn_contours,
		hull,
		i,
		Scalar(255, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
}

vector<Mat> BlobDetector::get_large_shapes(Mat *contoursonly, vector<vector<Point>> hull, int hullsize)
{
	vector<Mat> shapes(20);
	Point topleft = Point(contoursonly->rows,contoursonly->cols);
	Point botright = Point(0,0);
	Rect roi;
	Mat localsrc = *contoursonly;

	for (int i = 0; i < hullsize; i++)
	{
		for (int k = 0; k < hull[i].size(); k++) 
		{
			if (hull[i][k].x < topleft.x)
			{
				topleft.x = hull[i][k].x;
			}

			if (hull[i][k].x > botright.x)
			{
				botright.x = hull[i][k].x;
			}

			if (hull[i][k].y < topleft.y)
			{
				topleft.y = hull[i][k].y;
			}

			if (hull[i][k].y > botright.y)
			{
				botright.y = hull[i][k].y;
			}
		}
		//rectangle(*contoursonly, topleft, botright, Vec3b(255, 255, 255));

		if (botright.x != 0 && topleft.x != 0)
		{
			roi = Rect(topleft.x, topleft.y, botright.x - topleft.x, botright.y - topleft.y);
			shapes[i] = localsrc(roi);

			imshow("extract", shapes[i]);
			waitKey(0);
			destroyWindow("extract");
		}

		topleft = Point(contoursonly->rows, contoursonly->cols);
		botright = Point(0, 0);
	}

	return shapes;
}


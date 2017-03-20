#ifndef BLOBDETECTOR_H
#define BLOBDETECTOR_H

#include <iostream>
#include <stdio.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

class BlobDetector
{
	public:
		int run();
		void examine_frame(Mat *frame,Mat *fgmask);
		Mat highlight_contours(Mat *frame, Mat *fgmask, Mat *contoursonly);
		void draw_annotations(Mat * frame, Mat * drawn_contours, Mat *contoursonly, vector<vector<Point>> contours, vector<Vec4i> hierarchy, vector<vector<Point>> hull, int i);
		vector<Mat> get_large_shapes(Mat * contoursonly, vector<vector<Point>> hull, int hullsize);
};

#endif

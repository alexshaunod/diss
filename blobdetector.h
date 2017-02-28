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
		Mat highlight_contours(Mat *frame, Mat *fgmask);
		void draw_annotations(Mat * frame, Mat * drawn_contours, vector<vector<Point>> contours, vector<Vec4i> hierarchy, vector<vector<Point>> hull, int i);
};

#endif

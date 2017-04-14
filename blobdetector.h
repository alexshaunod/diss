#ifndef BLOBDETECTOR_H
#define BLOBDETECTOR_H

#include <iostream>
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

class BlobDetector
{
	private:
		vector<vector<Point>> hull_list;
		int hull_size;
		vector<Mat> src_shapes;

	public:
		BlobDetector(vector<Mat> src_shapes);
		Mat highlight_contours(Mat *frame, Mat *fgmask, Mat *contoursonly);
		void draw_annotations(Mat * frame, Mat * drawn_contours, Mat *contoursonly, vector<vector<Point>> contours, vector<Vec4i> hierarchy, vector<vector<Point>> hull, int i);
		vector<Mat> get_large_shapes(Mat *src_image, Mat * filtered_mask, vector<vector<Point>> hull, int hullsize, int edge_space);
		vector<Mat> get_src_shapes();
		bool is_within_bound(Point node, int x_bound, int y_bound);
		vector<vector<Point>> get_hull_list();
		int get_hull_size();
};

#endif

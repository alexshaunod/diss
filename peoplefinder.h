#ifndef PEOPLEFINDER_H
#define PEOPLEFINDER_H
#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "branch.h"
#include "blobdetector.h"

using namespace cv;
using namespace std;

class PeopleFinder
{
	private :

	public :
		void run();
		void train();
		void demo();
		vector<Point> create_skeleton(Mat * contoursonly, int imagenum);
		Point find_head_feature(vector<Point> shape_pixels, int threshold);
		Point find_torso_feature(vector<Point> shape_pixels, int threshold, Point head_feature);
		Point find_waist_feature(vector<Point> shape_pixels, int threshold, Point torso_feature);
		Point find_foot_feature(vector<Point> shape_pixels, int threshold, Point waist_feature, Point corner);
		void set_shoulder_positions(vector<Point> shape_pixels, int threshold, Point torso_feature, Point * left_shoulder, Point * right_shoulder, int *arm_width);
		void calc_halfway_torso_dist(Point torso_feature, Point waist_feature, Point * halfway_node, double * halfway_dist);
		Point find_elbow_feature(vector<Point> shape_pixels, Point torso_feature, Point waist_feature, Point shoulder_feature, int *arm_width, double halfway_dist, Point halfway_node);
		Point find_hand_feature(vector<Point> shape_pixels, vector<Point> outline_pixels, Point waist_feature, Point elbow_feature, int *arm_width, double halfway_dist, Point halfway_node, Mat *contours);
		Point find_closest_pixel(vector<Point> shape_pixels, Point goal_node, int x_bound, int n);
		vector<string> search_dataset_files(const string directory);
		vector<Mat> load_dataset_files(vector<string> filenames, const string directory);
};

#endif

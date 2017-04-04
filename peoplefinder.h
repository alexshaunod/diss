#ifndef PEOPLEFINDER_H
#define PEOPLEFINDER_H
#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <ctime>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blobdetector.h"

using namespace cv;
using namespace std;

class PeopleFinder
{
	private :
		vector<Point> min_range;
		vector<Point> max_range;
		bool bad_skel_flag;

	public :
		PeopleFinder(vector<Point> min, vector<Point> max, bool bad);
		void init();
		void run();
		void train();
		void train_compare_ranges(vector<Point> feature_nodes);
		void demo();
		void test(vector<Mat> shapes, int hull_size);

		vector<Point> create_skeleton(Mat * contoursonly, int imagenum);

		//void highlight_pixels(Mat * contoursonly, vector<Point>* shape_pixels, vector<Point>* outline_pixels);

		Point find_head_feature(vector<Point> shape_pixels, int threshold, int *index_head);
		Point find_torso_feature(vector<Point> shape_pixels, int threshold, Point head_feature, int index_head, int *index_torso);
		Point find_waist_feature(vector<Point> shape_pixels, int threshold, Point torso_feature, int index_torso, int *index_waist);
		Point find_foot_feature(vector<Point> shape_pixels, int threshold, Point waist_feature, Point corner, int index_waist);

		void set_shoulder_positions(vector<Point> shape_pixels, int threshold, Point torso_feature, Point * left_shoulder, Point * right_shoulder, int *arm_width, int index_torso, int *index_shoulders);
		void calc_halfway_torso_dist(Point torso_feature, Point waist_feature, Point * halfway_node, double * halfway_dist);

		Point find_elbow_feature(vector<Point> shape_pixels, Point torso_feature, Point waist_feature, Point shoulder_feature, int *arm_width, double halfway_dist, Point halfway_node, int index_shoulders);
		Point find_hand_feature(vector<Point> shape_pixels, vector<Point> outline_pixels, Point waist_feature, Point elbow_feature, int *arm_width, double halfway_dist, Point halfway_node, Mat *contours, int index_shoulders);
		Point find_closest_pixel(vector<Point> shape_pixels, Point goal_node, int x_bound, int n);

		void draw_skeleton(Mat *image, vector<Point> nodes);

		bool is_within_bound(Point node, int x_bound, int y_bound);

		vector<string> search_dataset_files(const string directory);
		vector<Mat> load_dataset_files(vector<string> filenames, const string directory);
		void save_image(Mat image, string folder, int i);
};

#endif

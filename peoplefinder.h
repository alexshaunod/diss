#ifndef PEOPLEFINDER_H
#define PEOPLEFINDER_H

#include <iostream>
#include <stdio.h>
#include <windows.h>
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
		vector<Point> create_skeleton(Mat * contoursonly, int imagenum);
		Point find_head_feature(vector<Point> shapepixels, int threshold);
		Point find_torso_feature(vector<Point> shapepixels, int threshold, Point head_feature);
		vector<string> search_dataset_files(const string directory);
		vector<Mat> load_dataset_files(vector<string> filenames, const string directory);
		void blob_detect_training();
		void analyse_frame();
};

#endif

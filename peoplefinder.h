#ifndef PEOPLEFINDER_H
#define PEOPLEFINDER_H

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

class PeopleFinder
{
	public :
		void train();
		void read_dataset_files(const string directory);
		void analyse_frame();
};

#endif

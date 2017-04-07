#ifndef RECORDLOG_H
#define RECORDLOG_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <ctime>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

class RecordLog
{
	private:
		ofstream file;
		string current_date;
		int total_records;

	public:
		void init_log(string videoPath);

		void new_record(int mill_seconds, Mat src_image, Mat contour_image, string verdict);

		string get_date();

		string save_image(string type_name, Mat * image);

		void convert_milliseconds(int mill_seconds, int * hh, int * mm, int * ss);

		void close_log();
	
};

#endif

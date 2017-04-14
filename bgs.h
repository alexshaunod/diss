#ifndef BGS_H
#define BGS_H

#include <iostream>
#include <stdio.h>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/photo.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "peoplefinder.h"
#include "blobdetector.h"
#include "recordlog.h"

class BGS
{
	private:
		RecordLog rlog;
		string training_path;
		string video_path;
		int bgs_history;
		double bgs_threshold;

	public :
		BGS(string t_path, string v_path, int history, double thresh);
		int run();
		void run_frame_analysis(int milliseconds, vector<Mat> src_shapes, vector<Mat> large_shapes, vector<string> verdicts);
		Mat filter_noise(Mat *fgmask);
		Mat erode_first(Mat *srcimg, Mat *element);
		Mat dilate_first(Mat *srcimg, Mat *element);
};

#endif

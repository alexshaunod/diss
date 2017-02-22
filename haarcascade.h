#ifndef HAARCASCADE_H
#define HAARCASCADE_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class HaarCascade
{
	public :
		int run();
		void detectAndDisplay(Mat frame);
};

#endif


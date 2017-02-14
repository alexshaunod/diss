#ifndef HAARCASCADE_H
#define HAARCASCADE_H

using namespace cv;

class HaarCascade
{
	public :
		int run();
		void detectAndDisplay(Mat frame);
};

#endif


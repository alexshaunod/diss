#ifndef HOGCASCADE_H
#define HOGCASCADE_H

using namespace cv;

class HogCascade
{
	public:
		int run();
		void testMatrix(Mat &mag, Mat &angle);
};

#endif
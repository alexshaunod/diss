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
#include "blobdetector.h"

using namespace cv;
using namespace std;

class BGS
{
	public :
		int run();
		Mat filter_noise(Mat *fgmask);
};

#endif

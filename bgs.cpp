#include <iostream>
#include <stdio.h>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "bgs.h"

using namespace std;
using namespace cv;

int BGS::run()
{
	VideoCapture capCam = VideoCapture();
	Mat frame, fgMask, fgMask2;
	Ptr<BackgroundSubtractor> pMOG, pMOG2;
	stringstream ss;
	string frameNumberString;

	capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
	if (capCam.isOpened())
	{
		while (true)
		{
			capCam.read(frame);

			if (!frame.empty())
			{
				pMOG->apply(frame, fgMask);
				pMOG2->apply(frame, fgMask2);
				imshow("test", frame);

				//display fps
				ss << capCam.get(CAP_PROP_POS_FRAMES);
				frameNumberString = ss.str();
				putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
					FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
			}
			else
			{
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}

	return 0;
}
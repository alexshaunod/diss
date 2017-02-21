#include <iostream>
#include <stdio.h>

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "bgs.h"

using namespace std;
using namespace cv;

int BGS::run()
{
	VideoCapture capCam = VideoCapture();
	Mat frame, fgMaskKNN, fgMaskMOG2;
	Ptr<BackgroundSubtractor> pKNN, pMOG2;
	int currentfps;
	char fps[50];

	pKNN = createBackgroundSubtractorKNN();
	pMOG2 = createBackgroundSubtractorMOG2();

	capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
	
	if (capCam.isOpened())
	{
		while (true)
		{
			capCam.read(frame);

			if (!frame.empty())
			{
				pKNN->apply(frame, fgMaskKNN);
				pMOG2->apply(frame, fgMaskMOG2);

				//display fps
				currentfps = capCam.get(CV_CAP_PROP_FPS);
				sprintf_s(fps, "%i" ,currentfps);
				putText(frame, fps, Point(frame.rows, 15), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 1);

				imshow("Video", frame);
				imshow("KNN", fgMaskKNN);
				imshow("MOG2", fgMaskMOG2);
			}
			else
			{
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	capCam.release();
	return 0;
}
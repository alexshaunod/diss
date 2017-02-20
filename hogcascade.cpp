#include <iostream>
#include <stdio.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "hogcascade.h"

using namespace std;
using namespace cv;

int HogCascade::run()
{
	VideoCapture capCam = VideoCapture();
	Mat frame, gx, gy, mag, angle;

	capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
	if (capCam.isOpened())
	{
		while (true)
		{
			capCam.read(frame);
			frame.convertTo(frame, CV_32F, 1 / 255.0);

			Sobel(frame, gx, CV_32F, 1, 0, 1); //horizontal kernel
			Sobel(frame, gy, CV_32F, 0, 1, 1); //vertical kernel
			
			if (!frame.empty())
			{
				cartToPolar(gx, gy, mag, angle, 1);
				testMatrix(mag, angle);
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

void HogCascade::testMatrix(Mat &mag, Mat &angle)
{
	int i, j, minX, maxX, minY, maxY;
	
	minX = 128;
	maxX = 136;
	minY = 72;
	maxY = 80;
	for (i = minX; i < maxX; i++)
	{
		for (j = minY; j<maxY; j++)
		{
			cout << "[" << mag.at<float>(i,j) << "," << angle.at<float>(i, j) << "]";
		}
		cout << endl;
	}
	cout << endl;
	
	rectangle(mag,
		Point(minX, minY),
		Point(maxX, maxY),
		Scalar(0, 0, 255) ,
		1,
		LINE_8
	);
	imshow("test", mag);
}
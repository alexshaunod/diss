#include "bgs.h"

int BGS::run()
{
	BlobDetector bd = BlobDetector();
	VideoCapture capCam = VideoCapture();
	Mat frame, fgMaskKNN, fgMaskMOG2, filteredMask, contourimg, contoursonly;
	Ptr<BackgroundSubtractor> pKNN, pMOG2;
	//int currentfps;
	//char fps[50];

	pKNN = createBackgroundSubtractorKNN(750, 500, false);
	//pMOG2 = createBackgroundSubtractorMOG2(500, 32, false);

	capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
	
	if (capCam.isOpened())
	{
		while (true)
		{
			capCam.read(frame);

			if (!frame.empty())
			{
				pKNN->apply(frame, fgMaskKNN);
				filteredMask = filter_noise(&fgMaskKNN);
				contourimg = bd.highlight_contours(&frame, &filteredMask, &contoursonly);
				//pMOG2->apply(frame, fgMaskMOG2); // type CV_8U

				//display fps
				//currentfps = capCam.get(CV_CAP_PROP_FPS);
				//sprintf_s(fps, "%i" ,currentfps);
				//putText(frame, fps, Point(frame.rows, 15), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 1);

				//bd.examine_frame(&frame, &fgMaskMOG2);
				imshow("Contours", contourimg);
				imshow("KNN", filteredMask);
				imshow("Video", frame);
				//imshow("MOG2", fgMaskMOG2);
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

Mat BGS::filter_noise(Mat *fgmask)
{
	Mat elem1, elem2, filteredMask, filteredMask2;
	elem1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	elem2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	filteredMask = dilate_first(fgmask, &elem1);
	filteredMask2 = erode_first(&filteredMask, &elem2);
	return filteredMask2;
}

Mat BGS::erode_first(Mat *srcimg, Mat *element)
{
	Mat erodedMask, dilatedMask;
	erode(*srcimg, erodedMask, *element);
	dilate(erodedMask, dilatedMask, *element);
	return dilatedMask;
}

Mat BGS::dilate_first(Mat *srcimg, Mat *element)
{
	Mat dilatedMask, erodedMask;
	dilate(*srcimg, dilatedMask, *element);
	erode(dilatedMask, erodedMask, *element);
	return erodedMask;
}

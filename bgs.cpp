#include "bgs.h"

int BGS::run()
{
	BlobDetector bd = BlobDetector();
	VideoCapture capCam = VideoCapture();
	Mat frame, fgMaskKNN, filteredMask, contourimg, contoursonly;
	Ptr<BackgroundSubtractor> pKNN, pMOG2;

	pKNN = createBackgroundSubtractorKNN(750, 500, false);

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

				imshow("Contours", contourimg);
				imshow("KNN", filteredMask);
				imshow("Video", frame);
				moveWindow("Video", 128, 128);
				moveWindow("KNN", 512, 128);
				moveWindow("Contours", 896, 128);
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

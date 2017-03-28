#include "bgs.h"

int BGS::run()
{
	BlobDetector bd = BlobDetector();
	PeopleFinder pf = PeopleFinder();
	VideoCapture capCam = VideoCapture();
	Mat frame, frame2, fgMaskKNN, filteredMask, contourimg, contoursonly;
	vector<Mat> large_shapes;
	Ptr<BackgroundSubtractor> pKNN, pMOG2;
	int fps, frame_number = 0, iteration = 0;

	pKNN = createBackgroundSubtractorKNN(750, 400, false);

	//capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg"); //MOST DIFFICULT VIDEO
	capCam.open("DataSets/CAVIAR/OneStopEnter2cor.mpg"); //REFLECTIONS SUPPRESSED, BEST VIDEO FOR HIGHLIGHTING INDIVIDUAL MOVEMENT
	//capCam.open("DataSets/CAVIAR/OneShopOneWait2front.mpg"); //GRAYSCALE INTERRUPTS SLIGHTLY, BUT REFLECTIONS MOSTLY SUPPRESSED
	//capCam.open("DataSets/CAVIAR/OneStopNoEnter1cor.mpg");
	
	if (capCam.isOpened())
	{
		fps = capCam.get(CV_CAP_PROP_FPS);
		while (true)
		{
			capCam.read(frame);

			if (!frame.empty())
			{
				frame_number++;
				cvtColor(frame, frame2, CV_BGRA2GRAY);
				pKNN->apply(frame2, fgMaskKNN);
				filteredMask = filter_noise(&fgMaskKNN);

				contourimg = bd.highlight_contours(&frame, &filteredMask, &contoursonly);

				imshow("KNN", filteredMask);
				imshow("Video", frame);
				imshow("Contours", contourimg);
				moveWindow("Video", 128, 128);
				moveWindow("KNN", 512, 128);
				moveWindow("Contours", 896, 128);

				if (frame_number - (fps * iteration) == fps)	//This equation ensures that it runs the pedestrian detector every second of the video
				{
					iteration++;
					large_shapes = bd.get_large_shapes(&filteredMask, bd.get_hull_list(), bd.get_hull_size());
					pf.test(large_shapes, bd.get_hull_size());
				}
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

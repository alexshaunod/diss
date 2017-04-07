#include "bgs.h"

BGS::BGS(string t_path, string v_path)
	: training_path(t_path), video_path(v_path)
{}

int BGS::run()
{
	BlobDetector bd = BlobDetector(vector<Mat>(20));
	PeopleFinder pf = PeopleFinder(vector<Point>(11), vector<Point>(11), training_path, false);
	rlog = RecordLog();
	VideoCapture capCam = VideoCapture();
	Mat frame, frame2, fgMaskKNN, filteredMask, contourimg, contoursonly;
	vector<Mat> large_shapes;
	Ptr<BackgroundSubtractor> pKNN, pMOG2;
	int fps, frame_number = 0, iteration = 0;

	rlog.init_log(video_path);
	pf.train();

	pKNN = createBackgroundSubtractorKNN(750, 400, false);

	//capCam.open("DataSets/CAVIAR/WalkByShop1cor.mpg"); //MOST DIFFICULT VIDEO
	capCam.open(video_path); //REFLECTIONS SUPPRESSED, BEST VIDEO FOR HIGHLIGHTING INDIVIDUAL MOVEMENT
	//capCam.open("DataSets/CAVIAR/OneShopOneWait2front.mpg"); //GRAYSCALE INTERRUPTS SLIGHTLY, BUT REFLECTIONS MOSTLY SUPPRESSED
	//capCam.open("DataSets/CAVIAR/OneStopNoEnter1cor.mpg");	//Pedestrian shapes appear fairly well, slight reflections

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
				moveWindow("KNN", 128 + frame.cols, 128);
				moveWindow("Contours", 128 + (frame.cols*2), 128);

				if (frame_number - (fps * iteration) == fps)	//This equation ensures that it runs the pedestrian detector every second of the video
				{
					iteration++;
					large_shapes = bd.get_large_shapes(&frame, &filteredMask, bd.get_hull_list(), bd.get_hull_size(), 10);
					pf.test(&large_shapes, bd.get_hull_size());
					run_frame_analysis(capCam.get(CV_CAP_PROP_POS_MSEC), bd.get_src_shapes(), large_shapes, pf.get_verdicts());
				}
			}
			else
			{
				printf(" --(!) Video has finished playing -- Break!\n"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	capCam.release();
	rlog.close_log();

	return 0;
}

void BGS::run_frame_analysis(int milliseconds, vector<Mat> src_shapes, vector<Mat> large_shapes, vector<string> verdicts)
{
	int i = 0;

	while (large_shapes[i].rows != 0)
	{
		rlog.new_record(milliseconds, src_shapes[i], large_shapes[i], verdicts[i]);
		i++;
	}
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

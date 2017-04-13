#include "bgs.h"

/**
 *	@file bgs.cpp
 *  @desc Plays the video footage while displaying the source, BGS and blob detection
 *  contour frames to the user. Runs the PeopleFinder on the video each second and saves
 *  information to the record log.
 *
 *  @param string training_path - path for training images folder
 *  @param string video_path - path for video file 
 *
 *	@author Alex O'Donnell
 *	@version 1.00
 */

BGS::BGS(string t_path, string v_path)
	: training_path(t_path), video_path(v_path)
{}

/**
 *  @desc Begins playing the video footage from the video path, converting it to greyscale
 *  and running OpenCV's BGS KNN function on the footage. After filtering noise, the contours 
 *  are highlighted using the BlobDetector.
 *  
 *  Pedestrian detection is applied every second, where the larger shapes within the contour 
 *  image are sent to the PeopleFinder. The results of the PeopleFinder's analysis are sent
 *  to the recordlog.
 *
 *  @returns 0
 */
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

	capCam.open(video_path); //REFLECTIONS SUPPRESSED, BEST VIDEO FOR HIGHLIGHTING INDIVIDUAL MOVEMENT

	if (capCam.isOpened())
	{
		capCam.read(frame);
		fps = capCam.get(CV_CAP_PROP_FPS);
		imshow("KNN", frame);
		imshow("Video", frame);
		imshow("Contours", frame);
		moveWindow("Video", 128, 128);
		moveWindow("KNN", 136, 136);
		moveWindow("Contours", 144, 144);

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
	

				if (frame_number - (fps * iteration) == fps)	//This equation ensures that it runs the pedestrian detector every second of the video
				{
					iteration++;
					large_shapes = bd.get_large_shapes(&frame, &filteredMask, bd.get_hull_list(), bd.get_hull_size(), 10);
					pf.test(&large_shapes);
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

/**
 *  @desc Enters a new record into the record log
 *
 *  @param int milliseconds - the timestamp of the video 
 *  @param vector<Mat> src_shapes - the source of the contour shape
 *  @param vector<Mat> large_shapes - the PeopleFinder interpretation of the shape
 *  @param vector<string> verdicts - what the PeopleFinder classifies the shape as
 *
 *  @returns 0
 */
void BGS::run_frame_analysis(int milliseconds, vector<Mat> src_shapes, vector<Mat> large_shapes, vector<string> verdicts)
{
	int i = 0;

	while (large_shapes[i].rows != 0)
	{
		rlog.new_record(milliseconds, src_shapes[i], large_shapes[i], verdicts[i]);
		i++;
	}
}

/**
 *  @desc Filters speckle noise from the BGS frame. Using a closing operation
 *  followed by an opening operation.
 *
 *  @param Mat *fgmask - current frame in BGS
 *
 *  @returns Mat filteredMask2 - the BGS frame with noise reduced
 */
Mat BGS::filter_noise(Mat *fgmask)
{
	Mat elem1, elem2, filteredMask, filteredMask2;
	elem1 = getStructuringElement(MORPH_RECT, Size(5, 5));
	elem2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	filteredMask = dilate_first(fgmask, &elem1);
	filteredMask2 = erode_first(&filteredMask, &elem2);
	return filteredMask2;
}

/**
 *  @desc erodes speckles first then dilates them after.
 *
 *  @param Mat *srcimg - current frame in BGS
 *  @param Mat *element - structuring kernel
 *
 *  @returns the dilated mask
 */
Mat BGS::erode_first(Mat *srcimg, Mat *element)
{
	Mat erodedMask, dilatedMask;
	erode(*srcimg, erodedMask, *element);
	dilate(erodedMask, dilatedMask, *element);
	return dilatedMask;
}

/**
 *  @desc dilates speckles first then erodes them after.
 *
 *  @param Mat *srcimg - current frame in BGS
 *  @param Mat *element - structuring kernel
 *
 *  @returns the eroded mask
 */
Mat BGS::dilate_first(Mat *srcimg, Mat *element)
{
	Mat dilatedMask, erodedMask;
	dilate(*srcimg, dilatedMask, *element);
	erode(dilatedMask, erodedMask, *element);
	return erodedMask;
}

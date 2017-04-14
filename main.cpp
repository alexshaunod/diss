#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "bgs.h"
#include "peoplefinder.h"

using namespace cv;
using namespace std;

/**
 *	@file main.cpp
 *	@desc Provides a menu for running the feature detection demo or the
 *  video analysis, where the user can enter the file paths for the training folder
 *  and the video file.
 *
 *  The demo runs the	PeopleFinder's create_skeleton() function on the images within the 
 *  training path directory and displays the feature skeletons.
 *
 *  The video surveillance trains the PeopleFinder classifier using the images in the training
 *  path directory, and runs the video file to search for pedestrians via BGS.
 *
 *	@author Alex O'Donnell
 *	@version 1.00
 */

int main()
{	
	string response;
	string training_path = "DataSets/PedCut2013/data/completeData/left_groundtruth/*.*";
	string video_path = "DataSets/CVLAB/campus4-c1.avi";
	int bgs_history = 750;
	double bgs_threshold = 500;
	cout << "---Welcome to AutoSurvCV. Please type an option from the menu---" << endl;
	cout << "1. Training Demo" << endl;
	cout << "2. Run Video Surveillance" << endl;
	cout << "Q. Quit" << endl;

	while (true)
	{
		cin >> response;
		if (response.compare("1") == 0)
		{
			cout << "Enter the path of the training directory you wish to use." << endl;
			cout << "Input 'default' to use the default training directory: " << training_path << endl;
			cin >> response;
			if (response.compare("default") != 0)
			{
				training_path = response;
			}
			PeopleFinder pf = PeopleFinder(vector<Point>(11), vector<Point>(11), training_path, false);
			pf.demo();
			break;
		}

		if (response.compare("2") == 0)
		{
			cout << "Enter the path of the training directory you wish to use." << endl;
			cout << "Input 'default' to use the default training directory: " << training_path << endl;
			cin >> response;
			if (response.compare("default") != 0)
			{
				training_path = response;
			}

			cout << "Enter the path of the video you wish to use." << endl;
			cout << "Input 'default' to use the default video: " << video_path << endl;
			cin >> response;
			if (response.compare("default") != 0)
			{
				video_path = response;
			}

			cout << "Enter the BGS history length. Recommended: " << bgs_history << endl;
			cin >> bgs_history;
			if (cin.fail())
			{
				cout << "Please enter a number" << endl;
				break;
			}

			cout << "Enter the threshold on squared distance pixels and the sample. Recommended: " << bgs_threshold << endl;
			cin >> bgs_threshold;
			if (cin.fail())
			{
				cout << "Please enter a number" << endl;
				break;
			}
			BGS bgs = BGS(training_path, video_path, bgs_history, bgs_threshold);
			bgs.run();
			break;
		}
		if (response.compare("Q") == 0 || response.compare("q") == 0)
		{
			break;
		}
		cout << "Invalid Input!" << endl;
		cin.clear();
	}
	std::cin.ignore();

	return 0;
	
}
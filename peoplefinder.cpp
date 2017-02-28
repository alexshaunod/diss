#include "peoplefinder.h"


void PeopleFinder::train()
{
	Mat img;
	read_dataset_files("DataSets/PedCut2013/data/completeData/left_groundTruth/*.*");
	//img = imread("DataSets\PedCut2013\data\completeData\left_groundTruth");

	/*
	img.open("DataSets/CAVIAR/WalkByShop1cor.mpg");
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
	*/
}

void PeopleFinder::read_dataset_files(const string directory)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;
	string workdirectory, tempstr;

	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	workdirectory = string(buffer);

	tempstr = workdirectory.substr(0, workdirectory.find("x64", 0));
	replace(tempstr.begin(), tempstr.end(), '\\', '/');

	tempstr.append(directory);
	cout << tempstr << endl;
	
	hFind = FindFirstFile(tempstr.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			printf("%s\n", data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}	
}

void PeopleFinder::analyse_frame()
{

}
#include "peoplefinder.h"


void PeopleFinder::train()
{
	vector<string> filenames(2000);
	vector<Mat> images(2000);
	Mat tempimg;
	int i = 0;
	const string directory = "DataSets/PedCut2013/data/completeData/left_groundTruth/*.*";
	BlobDetector bd = BlobDetector();


	filenames = search_dataset_files(directory); //FORMAT: place folder in AutoSurvCV, forward slashes and end in "*.*"
	images = load_dataset_files(filenames, directory);
	while (!images.empty())
	{
		imshow("Contours", bd.highlight_contours(&images[i], &images[i]));
		imshow("Ground Truth Data", images[i]);
		waitKey(0);
		destroyWindow("Contours");
		destroyWindow("Ground Truth Data");
		i++;
	}
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

vector<string> PeopleFinder::search_dataset_files(const string directory)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;
	string workdirectory, tempstr;
	vector<string> filenames(2000);
	int i = 0;

	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	workdirectory = string(buffer);

	tempstr = workdirectory.substr(0, workdirectory.find("x64", 0));
	replace(tempstr.begin(), tempstr.end(), '\\', '/');

	tempstr.append(directory);
	cout << tempstr << endl;
	
	hFind = FindFirstFile(tempstr.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		cout << "Saving file paths..." << endl;
		do 
		{
			filenames[i] = data.cFileName;
			//cout << filenames[i] << endl;
			i++;
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
		cout << "File paths saved." << endl;
	}	
	else
	{
		cout << "Unable to open directory. Please check formatting. (Use / and *.*)" << endl;
	}
	return filenames;
}

vector<Mat> PeopleFinder::load_dataset_files(vector<string> filenames, const string directory)
{
	vector<Mat> imgs(2000);
	Mat tempimg;
	string directory_edit, fullpath;
	int i = 0, j = 0;

	directory_edit = directory.substr(0, directory.find("*.*", 0));
	cout << "Loading images from the file paths..." << endl;
	while (!filenames[i].empty())
	{
		fullpath = directory_edit;
		fullpath.append(filenames[i]);
		tempimg = imread(fullpath);
		if (!tempimg.empty())
		{
			cvtColor(tempimg, imgs[j], CV_BGRA2GRAY);
			j++;
		}
		i++;
	}
	cout << "Images loaded." << endl;
	return imgs;
}

void PeopleFinder::blob_detect_training()
{

}

void PeopleFinder::analyse_frame()
{

}
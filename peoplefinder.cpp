#include "peoplefinder.h"


void PeopleFinder::run()
{
	train();
}

void PeopleFinder::train()
{
	vector<string> filenames(2000);
	vector<Mat> images(2000);
	vector<Point> nodes(6);
	Mat tempimg, contourimg, contoursonly, distimg;
	int i = 0;
	const string directory = "DataSets/PedCut2013/data/completeData/left_groundTruth/*.*";
	BlobDetector bd = BlobDetector();

	filenames = search_dataset_files(directory); //FORMAT: place folder in AutoSurvCV, forward slashes and end in "*.*"
	images = load_dataset_files(filenames, directory);
	while (!images.empty())
	{
		contourimg = bd.highlight_contours(&images[i], &images[i], &contoursonly);
		nodes = create_skeleton(&contoursonly);
		//imshow("Hull", contourimg);
		imshow("Contours Only", contoursonly);
		imshow("Ground Truth Data", images[i]);
		waitKey(0);
		//destroyWindow("Hull");
		destroyWindow("Contours Only");
		destroyWindow("Ground Truth Data");
		i++;
	}


	//Point testnode1 = Point(10, 10);
	//Point testnode2 = Point(contourimg.rows, contourimg.cols);
	//Branch testbranch = Branch(testnode1.x, testnode1.y, testnode2.x, testnode2.y);
	//line(contourimg, testnode1, testnode2, Scalar(0, 127, 255));
	//distanceTransform(images[i], distimg, CV_DIST_C, 3);
}

vector<Point> PeopleFinder::create_skeleton(Mat *contoursonly)
{
	vector<Point> nodes(6);
	bool insideshape = false;
	int i, j;
	for (i = 0; i < contoursonly->rows; i++)
	{
		for (j = 0; j < contoursonly->cols; j++)
		{
			cout << "[" << (int)contoursonly->at<uchar>(i, j) << "]";
			//if ((int)contoursonly->at<uchar>(i, j) == 0)
			//{
				//(int)contoursonly->at<uchar>(i, j) == 255;
			//}
			/*
			if (insideshape)
			{
				if ((int)contoursonly->at<uchar>(j, i) == 255)
				{
					insideshape = false;
				}
				else
				{
					contoursonly->at<uchar>(j, i) = 255;
				}
			}
			if ((int)contoursonly->at<uchar>(j, i) == 255)
			{
				insideshape = true;
			}
			*/
		}
	}

	return nodes;

}

vector<string> PeopleFinder::search_dataset_files(const string directory)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;				//cite Microsoft
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
	Mat tempimg, resizeimg;
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
			cvtColor(tempimg, resizeimg, CV_BGRA2GRAY);
			resize(resizeimg, imgs[j], Size(128, 256));
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
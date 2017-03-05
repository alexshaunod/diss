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
		imshow("Ground Truth Data", images[i]);
		imshow("Contours Only", contoursonly);
		waitKey(0);
		//destroyWindow("Hull");
		destroyWindow("Ground Truth Data");
		destroyWindow("Contours Only");
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
	vector<Point> innerpixels(8192);
	Point highest_px;
	bool insideshape = false;
	int i, j, k;

	floodFill(*contoursonly, Point(32, 64), Scalar(64, 0, 0));
	k = 0;
	
	for (i = 0; i < contoursonly->rows; i++)
	{
		for (j = 0; j < contoursonly->cols; j++)
		{
			if (contoursonly->at<Vec3b>(i, j) == Vec3b(64, 0, 0))
			{
				innerpixels[k] = Point(i, j);
				k++;
			}
		}
	}
	k = 0;
	highest_px = Point(1000,1000);

	while (innerpixels[k] != Point(0,0))
	{
		if (innerpixels[k].x < highest_px.x)
		{
			highest_px.x = innerpixels[k].x;
			highest_px.y = innerpixels[k].y;
		}
		k++;
	}

	highest_px.x += 5;
	contoursonly->at<Vec3b>(highest_px.x, highest_px.y) = Vec3b(0, 128, 128);
	circle(*contoursonly, Point(highest_px.y, highest_px.x), 2, Scalar(0, 128, 128));
	
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
			resize(resizeimg, imgs[j], Size(64, 128));
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
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
	const string directory = "DataSets/PedCut2013/data/completeData/left_groundtruth/*.*";
	BlobDetector bd = BlobDetector();

	filenames = search_dataset_files(directory); //FORMAT: place folder in AutoSurvCV, forward slashes and end in "*.*"
	images = load_dataset_files(filenames, directory);
	while (!images[i].rows == 0)
	{
		cout << "Image :" << i << endl;
		contourimg = bd.highlight_contours(&images[i], &images[i], &contoursonly);
		nodes = create_skeleton(&contoursonly, i);
		//imshow("Hull", contourimg);
		imshow("Ground Truth Data", images[i]);
		imshow("Contours Only", contoursonly);
		waitKey(0);
		//destroyWindow("Hull");
		destroyWindow("Ground Truth Data");
		destroyWindow("Contours Only");
		i++;
	}
}

vector<Point> PeopleFinder::create_skeleton(Mat *contoursonly, int imagenum)
{
	vector<Point> nodes(6);
	vector<Point> shapepixels(8192);
	bool insideshape = false;
	int i, j, k;

	if (contoursonly->at<Vec3b>(64, 32) != Vec3b(0, 0, 255)) // checks to see if the middle pixel overlaps with a contour
	{
		floodFill(*contoursonly, Point(32, 64), Scalar(64, 0, 0)); //assumes the middle pixel always falls inside the shape
	}

	if (contoursonly->at<Vec3b>(0, 0) != Vec3b(64, 0, 0) && contoursonly->at<Vec3b>(64, 32) != Vec3b(0, 0, 255)) // if the fill is outside the center, skip the image(poor quality image)
	{
		k = 0;

		for (i = 0; i < contoursonly->rows; i++)
		{
			for (j = 0; j < contoursonly->cols; j++)
			{
				if (contoursonly->at<Vec3b>(i, j) == Vec3b(64, 0, 0))
				{
					shapepixels[k] = Point(i, j);
					k++;
				}
			}
		}

		nodes[0] = find_head_feature(shapepixels, 5);
		nodes[1] = find_torso_feature(shapepixels, 5, nodes[0]);

		for (i = 0; i < 6; i++)
		{
			if (nodes[i] != Point(0, 0))
			{
				contoursonly->at<Vec3b>(nodes[i].x, nodes[i].y) = Vec3b(0, 255, 0);
				circle(*contoursonly, Point(nodes[i].y, nodes[i].x), 2, Scalar(0, 255, 0));
				//rectangle(*contoursonly, Point(0,0), Point(63,63), Scalar(0, 255, 255), 1);
			}
		}
	}
	else
	{
		cout << "Unable to find skeleton in image " << imagenum << "(Reason: Off center)" << endl;
	}
	
	return nodes;

}

Point PeopleFinder::find_head_feature(vector<Point> shapepixels, int threshold)
{
	int i = 0;
	Point headnode = Point(1000, 1000);

	while (shapepixels[i] != Point(0, 0))
	{
		if (shapepixels[i].x < headnode.x)
		{
			headnode.x = shapepixels[i].x;
			headnode.y = shapepixels[i].y;
		}
		i++;
	}
	headnode.x += threshold;
	return headnode;
}

Point PeopleFinder::find_torso_feature(vector<Point> shapepixels, int threshold, Point head_feature)
{
	int i = 0;
	int lower_bound_x = 64; //half way down the image
	Point torsonode = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point current_row = shapepixels[0];
	int shortest_dist = 1000;
	int current_dist = 0; //distance between two sides of the shape

	if (lower_bound_x < head_feature.x) //prevent errors
	{
		lower_bound_x = head_feature.x + 1;
	}

	while (shapepixels[i].x < head_feature.x + threshold) //skip the pixels above the head feature
	{
		i++;
	}

	current_row = shapepixels[i];

	while (shapepixels[i] != Point(0, 0) && shapepixels[i].x < lower_bound_x)
	{
		i++;
		if (shapepixels[i].x == current_row.x)
		{
			current_dist += 1;
		}
		else
		{
			if (current_dist < shortest_dist)
			{
				shortest_dist = current_dist;
				best_fit_node = shapepixels[i - 1];
			}
			current_row = shapepixels[i];
			current_dist = 0;
		}
	}

	torsonode.x = best_fit_node.x + threshold;
	torsonode.y = best_fit_node.y - (shortest_dist / 2);

	return torsonode;
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
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
		//cout << "Image :" << i << endl;
		contourimg = bd.highlight_contours(&images[i], &images[i], &contoursonly);
		nodes = create_skeleton(&contoursonly, i);
		//imshow("Hull", contourimg);
		imshow("Ground Truth Data", images[i]);
		moveWindow("Ground Truth Data", 128, 128);
		imshow("Contours Only", contoursonly);
		moveWindow("Contours Only", 192, 128);
		waitKey(0);
		//destroyWindow("Hull");
		destroyWindow("Ground Truth Data");
		destroyWindow("Contours Only");
		i++;
	}
}

vector<Point> PeopleFinder::create_skeleton(Mat *contoursonly, int imagenum)
{
	vector<Point> nodes(10);
	vector<Point> shape_pixels(8192);
	vector<Point> outline_pixels(8192);
	Point halfway_node = Point(1000, 1000);
	bool insideshape = false;
	int i, j, k, m, arm_width;
	double halfway_dist;

	if (contoursonly->at<Vec3b>(64, 32) != Vec3b(0, 0, 255)) // checks to see if the middle pixel overlaps with a contour
	{
		floodFill(*contoursonly, Point(32, 64), Scalar(64, 0, 0)); //assumes the middle pixel always falls inside the shape
	}

	if (contoursonly->at<Vec3b>(0, 0) != Vec3b(64, 0, 0) && contoursonly->at<Vec3b>(64, 32) != Vec3b(0, 0, 255)) // if the fill is outside the center, skip the image(poor quality image)
	{
		k = 0;
		m = 0;

		for (i = 0; i < contoursonly->rows; i++)
		{
			for (j = 0; j < contoursonly->cols; j++)
			{
				if (contoursonly->at<Vec3b>(i, j) == Vec3b(64, 0, 0))
				{
					shape_pixels[k] = Point(i, j);
					k++;
				}
				if (contoursonly->at<Vec3b>(i, j) == Vec3b(0, 0, 255))
				{
					outline_pixels[m] = Point(i, j);
					m++;
				}
			}
		}

		nodes[0] = find_head_feature(shape_pixels, 5);
		nodes[1] = find_torso_feature(shape_pixels, 5, nodes[0]);
		nodes[2] = find_waist_feature(shape_pixels, 5, nodes[1]);
		calc_halfway_torso_dist(nodes[1], nodes[2], &halfway_node, &halfway_dist);

		nodes[3] = find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 1));
		nodes[4] = find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 63));

		set_shoulder_positions(shape_pixels, 5, nodes[1], &nodes[5], &nodes[6], &arm_width);
		nodes[7] = find_elbow_feature(shape_pixels, nodes[1], nodes[2], nodes[5], &arm_width, halfway_dist, halfway_node);
		nodes[8] = find_left_hand_feature(shape_pixels, outline_pixels, nodes[7], &arm_width, halfway_dist, halfway_node);

		line(*contoursonly, Point(nodes[0].y, nodes[0].x), Point(nodes[1].y, nodes[1].x), Scalar(255, 0, 255));
		line(*contoursonly, Point(nodes[1].y, nodes[1].x), Point(nodes[2].y, nodes[2].x), Scalar(255, 0, 255));
		line(*contoursonly, Point(nodes[2].y, nodes[2].x), Point(nodes[3].y, nodes[3].x), Scalar(255, 0, 255));
		line(*contoursonly, Point(nodes[2].y, nodes[2].x), Point(nodes[4].y, nodes[4].x), Scalar(255, 0, 255));

		for (i = 0; i < 10; i++)
		{
			if (nodes[i] != Point(0, 0))
			{
				contoursonly->at<Vec3b>(nodes[i].x, nodes[i].y) = Vec3b(0, 255, 0);
				circle(*contoursonly, Point(nodes[i].y, nodes[i].x), 2, Scalar(0, 255, 0));
			}
		}
		//rectangle(*contoursonly, Point(63,127), Point(62,126), Scalar(0, 255, 255), 1);
	}
	else
	{
		cout << "Unable to find skeleton in image " << imagenum << "(Reason: Off center)" << endl;
	}
	
	return nodes;

}

Point PeopleFinder::find_head_feature(vector<Point> shape_pixels, int threshold)
{
	int i = 0;
	Point headnode = Point(1000, 1000);

	while (shape_pixels[i] != Point(0, 0))
	{
		if (shape_pixels[i].x < headnode.x)
		{
			headnode.x = shape_pixels[i].x;
			headnode.y = shape_pixels[i].y;
		}
		i++;
	}
	headnode.x += threshold;
	return headnode;
}

Point PeopleFinder::find_torso_feature(vector<Point> shape_pixels, int threshold, Point head_feature)
{
	int i = 0;
	int lower_bound_x = 48; //half way down the image
	Point torsonode = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point current_row = shape_pixels[0];
	int shortest_dist = 1000;
	int current_dist = 0; //distance between two sides of the shape
	int head_range_y = 10;

	if (lower_bound_x < head_feature.x) //prevent out of bound errors, lower boundary for the torso must at least be lower than the head
	{
		lower_bound_x = head_feature.x + 1;
	}

	while (shape_pixels[i].x < head_feature.x + threshold) //skip the pixels above the head feature
	{
		i++;
	}

	current_row = shape_pixels[i];
	//shapepixels[i].y > head_feature.y - head_range_y && shapepixels[i].y < head_feature.y + head_range_y

	while (shape_pixels[i] != Point(0, 0) && shape_pixels[i].x < lower_bound_x)
	{
		i++;
		if (shape_pixels[i].x == current_row.x)
		{
			current_dist += 1;
		}
		else
		{
			if (current_dist < shortest_dist)
			{
				shortest_dist = current_dist;
				best_fit_node = shape_pixels[i - 1];
			}
			current_dist = 0;
			current_row = shape_pixels[i];
		}
	}

	torsonode.x = best_fit_node.x + threshold;
	torsonode.y = best_fit_node.y - (shortest_dist / 2);

	return torsonode;
}

Point PeopleFinder::find_waist_feature(vector<Point> shape_pixels, int threshold, Point torso_feature)
{
	int i = 0;
	int upper_bound_x = 64; //half way down the image
	int lower_bound_x = 80;
	Point waistnode = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point current_row = shape_pixels[0]; //uses the column (y) axis to check for discontinuities, e.g if it jumps from 1,1 -> 1,3 then 1,2 is a discontinuity
	int largest_dist = 0;
	int current_dist = 0; //distance between two sides of the shape

	if (upper_bound_x < torso_feature.x) //prevent out of bound errors, lower boundary for the torso must at least be lower than the head
	{
		upper_bound_x = torso_feature.x + 1;
	}

	while (shape_pixels[i].x < upper_bound_x + threshold) //skip the pixels above the upper boundary, only searching lower half of the body
	{
		i++;
	}

	current_row = shape_pixels[i];

	while (shape_pixels[i] != Point(0, 0) && shape_pixels[i].x < lower_bound_x)
	{
		i++;
		current_row.y += 1; //by increasing the current rows' y position, we can ignore discontinuities cause by arms/hands
		if (shape_pixels[i] == current_row)
		{
			current_dist += 1;
		}
		else
		{
			if (current_dist > largest_dist)
			{
				largest_dist = current_dist;
				best_fit_node = shape_pixels[i - 1];
			}
			current_row = shape_pixels[i];
			current_dist = 0;
		}
	}

	waistnode.x = best_fit_node.x - threshold;
	waistnode.y = best_fit_node.y - (largest_dist / 2);

	return waistnode;
}

Point PeopleFinder::find_foot_feature(vector<Point> shape_pixels, int threshold, Point waist_feature, Point corner)
{
	int i = 0;
	int upper_bound_x = 70;
	Point footnode = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	double distx, disty;
	double current_dist;
	double shortest_corner_dist = 10000;

	if (upper_bound_x < waist_feature.x) //prevent out of bound errors, lower boundary for the torso must at least be lower than the head
	{
		upper_bound_x = waist_feature.x + 1;
	}

	while (shape_pixels[i].x < upper_bound_x + threshold) //skip the pixels above the upper boundary, only searching lower half of the body
	{
		i++;
	}

	while (shape_pixels[i] != Point(0, 0))
	{
		i++;
		distx = (corner.x - shape_pixels[i].x) * (corner.x - shape_pixels[i].x);
		disty = (corner.y - shape_pixels[i].y) * (corner.y - shape_pixels[i].y);
		current_dist = sqrt(distx + disty);
		if (current_dist < shortest_corner_dist)
		{
			shortest_corner_dist = current_dist;
			best_fit_node = shape_pixels[i];
		}
	}

	footnode.x = best_fit_node.x;
	footnode.y = best_fit_node.y;

	return footnode;
}

void PeopleFinder::set_shoulder_positions(vector<Point> shape_pixels, int threshold, Point torso_feature, Point *left_shoulder, Point *right_shoulder, int *arm_width)
{
	int i = 0;
	int upper_bound_x = torso_feature.x;
	int lower_bound_x = torso_feature.x + threshold;
	Point best_fit_node = Point(1000, 1000);
	Point current_row = shape_pixels[0]; //uses the column (y) axis to check for discontinuities, e.g if it jumps from 1,1 -> 1,3 then 1,2 is a discontinuity
	int largest_dist = 0;
	int current_dist = 0; //distance between two sides of the shape

	while (shape_pixels[i].x < upper_bound_x)
	{
		i++;
	}

	current_row = shape_pixels[i];

	while (shape_pixels[i] != Point(0, 0) && shape_pixels[i].x < lower_bound_x)
	{
		i++;
		current_row.y += 1; //by increasing the current rows' y position, we can ignore discontinuities cause by arms/hands
		if (shape_pixels[i] == current_row)
		{
			current_dist += 1;
		}
		else
		{
			if (current_dist > largest_dist)
			{
				largest_dist = current_dist;
				best_fit_node = shape_pixels[i - 1];
			}
			current_row = shape_pixels[i];
			current_dist = 0;
		}
	}

	*arm_width = largest_dist / 10;

	*left_shoulder = Point( best_fit_node.x, best_fit_node.y - largest_dist + *arm_width);
	*right_shoulder = Point( best_fit_node.x, best_fit_node.y - *arm_width);

}

void PeopleFinder::calc_halfway_torso_dist(Point torso_feature, Point waist_feature, Point *halfway_node, double *halfway_dist)
{
	int halfway_dist_x, halfway_dist_y;
	double distx, disty;

	halfway_dist_x = (waist_feature.x - torso_feature.x) / 2;
	halfway_node->x = torso_feature.x + halfway_dist_x;
	halfway_dist_y = (waist_feature.y - torso_feature.y) / 2;
	halfway_node->y = torso_feature.y + halfway_dist_y;

	distx = (halfway_node->x - torso_feature.x) * (halfway_node->x - torso_feature.x);
	disty = (halfway_node->y - torso_feature.y) * (halfway_node->y - torso_feature.y);
	*halfway_dist = sqrt(distx + disty);
}

Point PeopleFinder::find_elbow_feature(vector<Point> shape_pixels, Point torso_feature, Point waist_feature, Point shoulder_feature, int *arm_width, double halfway_dist, Point halfway_node)
{
	int i = 0;
	Point elbow_node = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point valid_pixel = Point(1000, 1000);
	double distx, disty;
	double current_dist;
	double closest_dist = 100000;

	while (shape_pixels[i].x < shoulder_feature.x) //assume the elbows are not above the shoulder
	{
		i++;
	}

	valid_pixel = Point(shape_pixels[i].x, shape_pixels[i].y + *arm_width);
	i++;

	while (shape_pixels[i] != Point(0,0) && shape_pixels[i].x <= halfway_node.x)
	{
		i++;
		if (shape_pixels[i] == valid_pixel)
		{
			distx = (shape_pixels[i].x - shoulder_feature.x) * (shape_pixels[i].x - shoulder_feature.x);
			disty = (shape_pixels[i].y - shoulder_feature.y) * (shape_pixels[i].y - shoulder_feature.y);
			current_dist = sqrt(distx + disty);

			if ((halfway_dist - current_dist) <= closest_dist)
			{
				closest_dist = halfway_dist - current_dist;
				best_fit_node = shape_pixels[i];
			}
		}
		if (shape_pixels[i].x != valid_pixel.x)
		{
			valid_pixel = Point(shape_pixels[i].x, shape_pixels[i].y + *arm_width);
		}
	}

	elbow_node.x = best_fit_node.x;
	elbow_node.y = best_fit_node.y;

	return elbow_node;
}

Point PeopleFinder::find_left_hand_feature(vector<Point> shape_pixels, vector<Point> outline_pixels, Point elbow_feature, int *arm_width, double halfway_dist, Point halfway_node)
{
	int i = 0;
	Point hand_point = Point(1000, 1000);
	double dist_iteration = 0;
	double average_angle = 0;
	double angle_rad;

	while (outline_pixels[i].x < elbow_feature.x - *arm_width) 
	{
		i++;
	}

	while (outline_pixels[i] != Point(0, 0) && dist_iteration <= halfway_dist)
	{
		i++;
		if (outline_pixels[i].y <= halfway_node.y + *arm_width)
		{
			dist_iteration++;
			angle_rad = atan2(elbow_feature.x - outline_pixels[i].x, elbow_feature.y - outline_pixels[i].y) * M_PI / 180;
			average_angle += angle_rad;
			cout << "Angle between " << elbow_feature << " and " << outline_pixels[i] << " is " << angle_rad << endl;
		}
	}
	average_angle = average_angle/halfway_dist;
	//average_angle = average_angle * M_PI / 180;

	hand_point.x = (int)elbow_feature.x + halfway_dist * acos(average_angle);
	hand_point.y = (int)elbow_feature.y + halfway_dist * acos(average_angle);

	return hand_point;
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
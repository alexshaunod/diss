#include "peoplefinder.h"

void PeopleFinder::run()
{
	//train();
	demo();
}

void PeopleFinder::train()
{
	vector<string> filenames(2000);
	vector<Mat> images(2000);
	Mat tempimg, contourimg, contoursonly, distimg;
	int i = 0;
	const string directory = "DataSets/PedCut2013/data/completeData/left_groundtruth/*.*";
	BlobDetector bd = BlobDetector();

	filenames = search_dataset_files(directory); //FORMAT: place folder in AutoSurvCV, forward slashes and end in "*.*"
	images = load_dataset_files(filenames, directory);
	while (!images[i].rows == 0)
	{
		contourimg = bd.highlight_contours(&images[i], &images[i], &contoursonly);
		create_skeleton(&contoursonly, i);
		i++;
	}
	cout << "Classifier has been trained" << endl;
}

void PeopleFinder::demo()
{
	vector<string> filenames(2000);
	vector<Mat> images(2000);
	Mat tempimg, contourimg, contoursonly, distimg;
	int i = 0;
	const string directory = "DataSets/PedCut2013/data/completeData/left_groundtruth/*.*";
	BlobDetector bd = BlobDetector();

	filenames = search_dataset_files(directory); //FORMAT: place folder in AutoSurvCV, forward slashes and end in "*.*"
	images = load_dataset_files(filenames, directory);
	while (!images[i].rows == 0)
	{
		contourimg = bd.highlight_contours(&images[i], &images[i], &contoursonly);
		create_skeleton(&contoursonly, i);
		imshow("Ground Truth Data", images[i]);
		moveWindow("Ground Truth Data", 128, 128);
		imshow("Contours Only", contoursonly);
		moveWindow("Contours Only", 192, 128);
		waitKey(0);
		destroyWindow("Ground Truth Data");
		destroyWindow("Contours Only");
		create_skeleton(&contoursonly, i);
		i++;
	}
}
void PeopleFinder::test(vector<Mat> shapes, int hull_size)
{
	Mat current_shape;
	int i = 0;

	while (shapes[i].rows != 0)
	{
		resize(shapes[i], current_shape, Size(64, 128));
		create_skeleton(&current_shape, i);
		//save_image(current_shape, "faulty_imgs/", i);
		//imshow("Contours Only", current_shape);
		//moveWindow("Contours Only", 192, 128);

		//waitKey(0);
		//destroyWindow("Contours Only");
		i++;
	}
	
}

vector<Point> PeopleFinder::create_skeleton(Mat *contoursonly, int imagenum)
{
	vector<Point> nodes(11);
	vector<Point> shape_pixels(8192);
	vector<Point> outline_pixels(4096);
	Point halfway_node = Point(1000, 1000);
	bool insideshape = false;
	int i, j, k, m, arm_width;
	int index_head, index_torso, index_waist, index_shoulders;
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

		nodes[0] = find_head_feature(shape_pixels, 5, &index_head);
		nodes[1] = find_torso_feature(shape_pixels, 5, nodes[0], index_head, &index_torso);
		if (is_within_bound(nodes[1], contoursonly->rows, contoursonly->cols))	//Torso gives indication on whether the shape is valid or not
		{
			nodes[2] = find_waist_feature(shape_pixels, 5, nodes[1], index_torso, &index_waist);
			calc_halfway_torso_dist(nodes[1], nodes[2], &halfway_node, &halfway_dist);

			nodes[3] = find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 1), index_waist);
			nodes[4] = find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 63), index_waist);

			set_shoulder_positions(shape_pixels, 5, nodes[1], &nodes[5], &nodes[6], &arm_width, index_torso, &index_shoulders);
			nodes[7] = find_elbow_feature(shape_pixels, nodes[1], nodes[2], nodes[5], &arm_width, halfway_dist, halfway_node, index_shoulders);
			nodes[8] = find_hand_feature(shape_pixels, outline_pixels, nodes[2], nodes[7], &arm_width, halfway_dist, halfway_node, contoursonly, index_shoulders);
			nodes[9] = find_elbow_feature(shape_pixels, nodes[1], nodes[2], nodes[6], &arm_width, halfway_dist, halfway_node, index_shoulders);
			nodes[10] = find_hand_feature(shape_pixels, outline_pixels, nodes[2], nodes[9], &arm_width, halfway_dist, halfway_node, contoursonly, index_shoulders);

			draw_skeleton(contoursonly, nodes);
		}
		else
		{
			cout << "Bad shape, no skeleton found" << endl;
		}
	}
	else
	{
		cout << "Unable to find skeleton in image " << "(Reason: Off center)" << endl;
	}
	
	return nodes;

}

/*
void PeopleFinder::highlight_pixels(Mat *contoursonly, vector<Point> *shape_pixels, vector<Point> *outline_pixels)
{
	int i, j, k, m;
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
}
*/

Point PeopleFinder::find_head_feature(vector<Point> shape_pixels, int threshold, int *index_head)
{
	int i = 0;
	Point headnode = Point(1000,1000);

	while (shape_pixels[i] != Point(0, 0) && shape_pixels[i].x < headnode.x + threshold)	//Assume the head has been found after the first few rows have been searched
	{
		if (shape_pixels[i].x < headnode.x)
		{
			headnode.x = shape_pixels[i].x;
			headnode.y = shape_pixels[i].y;
			*index_head = i;
		}
		i++;
	}
	headnode.x += threshold;
	return headnode;
}

Point PeopleFinder::find_torso_feature(vector<Point> shape_pixels, int threshold, Point head_feature, int index_head, int *index_torso)
{
	int i = index_head;	//Start the initial iterations from the head pixel
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

	while (shape_pixels[i].x < head_feature.x + threshold && shape_pixels[i] != Point(0, 0)) //skip the pixels above the head feature
	{
		i++;
	}

	current_row = shape_pixels[i];
	best_fit_node = shape_pixels[i];

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
				*index_torso = i - 1;
			}
			current_dist = 0;
			current_row = shape_pixels[i];
		}
	}

	torsonode.x = best_fit_node.x + threshold;
	torsonode.y = best_fit_node.y - (shortest_dist / 2);

	return torsonode;
}

Point PeopleFinder::find_waist_feature(vector<Point> shape_pixels, int threshold, Point torso_feature, int index_torso, int *index_waist)
{
	int i = index_torso; //Start the search from the torso pixel
	int upper_bound_x = 64; //half way down the image
	int lower_bound_x = 80;
	Point waistnode = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point current_row = shape_pixels[0]; //uses the column (y) axis to check for discontinuities, e.g if it jumps from 1,1 -> 1,3 then 1,2 is a discontinuity
	int largest_dist = 0;
	int current_dist = 0; //distance between two sides of the shape

	try
	{
		if (upper_bound_x < torso_feature.x) //prevent out of bound errors, lower boundary for the torso must at least be lower than the head
		{
			upper_bound_x = torso_feature.x + 1;
		}

		while (shape_pixels[i].x < upper_bound_x + threshold) //skip the pixels above the upper boundary, only searching lower half of the body
		{
			if ((upper_bound_x + threshold - shape_pixels[i].x) >= 8)	//If the shape_pixels search space is 8 rows away from the ideal waist, speed up
			{
				i += 100;
			}
			i++;
		}

		current_row = shape_pixels[i];
		best_fit_node = current_row;

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
					*index_waist = i - 1;
				}
				current_row = shape_pixels[i];
				current_dist = 0;
			}
		}
	}
	catch (Exception e)
	{
		cout << "Unable to find waist feature." << endl;
	}

	waistnode.x = best_fit_node.x - threshold;
	waistnode.y = best_fit_node.y - (largest_dist / 2);

	return waistnode;
}

Point PeopleFinder::find_foot_feature(vector<Point> shape_pixels, int threshold, Point waist_feature, Point corner, int index_waist)
{
	int i = index_waist;
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

	while (shape_pixels[i].x < upper_bound_x + threshold && shape_pixels[i] != Point(0,0)) //skip the pixels above the upper boundary, only searching lower half of the body
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

	footnode = best_fit_node;

	return footnode;
}

void PeopleFinder::set_shoulder_positions(vector<Point> shape_pixels, int threshold, Point torso_feature, Point *left_shoulder, Point *right_shoulder, int *arm_width, int index_torso, int *index_shoulders)
{
	int i = index_torso;
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
				*index_shoulders = i - 1;
			}
			current_row = shape_pixels[i];
			current_dist = 0;
		}
	}

	*arm_width = largest_dist / 10;
	if (*arm_width == 0)
	{
		*arm_width = 1;
	}

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

Point PeopleFinder::find_elbow_feature(vector<Point> shape_pixels, Point torso_feature, Point waist_feature, Point shoulder_feature, int *arm_width, double halfway_dist, Point halfway_node, int index_shoulders)
{
	int i = index_shoulders;
	Point elbow_node = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point valid_pixel = Point(1000, 1000);
	double distx, disty;
	double current_dist;
	double closest_dist = 100000;

	try
	{
		while (shape_pixels[i].x < shoulder_feature.x) //assume the elbows are not above the shoulder
		{
			i++;
		}

		valid_pixel = Point(shape_pixels[i].x, shape_pixels[i].y + *arm_width);
		best_fit_node = valid_pixel;
		if (shoulder_feature.y >= torso_feature.y) //right shoudler, assume were looking for the right elbow
		{
			valid_pixel = Point(shape_pixels[i].x, shape_pixels[i - 1].y - *arm_width);
		}
		i++;

		while (shape_pixels[i] != Point(0, 0) && shape_pixels[i].x <= halfway_node.x)
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
				if (shoulder_feature.y >= torso_feature.y) //right shoudler, assume were looking for the right elbow
				{
					valid_pixel = Point(shape_pixels[i].x, shape_pixels[i - 1].y - *arm_width);
				}
			}
		}

	}
	catch (Exception e)
	{
		cout << "Unable to find elbow feature" << endl;
	}

	elbow_node = best_fit_node;

	return elbow_node;
}

Point PeopleFinder::find_hand_feature(vector<Point> shape_pixels, vector<Point> outline_pixels, Point waist_feature, Point elbow_feature, int *arm_width, double halfway_dist, Point halfway_node, Mat *contours, int index_shoulders)
{
	int i = elbow_feature.x, j = index_shoulders; //skip ahead using these idnex values to increase performance
	Point hand_node = Point(1000, 1000);
	Point best_fit_node = Point(1000, 1000);
	Point prev_valid_pixel = Point(1000, 1000);
	Point curr_pixel;
	vector<Point> neighbours(8);
	int dist_iteration = 0;
	double angle, average_angle = 0;

	try
	{
		while (outline_pixels[i].x < elbow_feature.x - *arm_width && i < outline_pixels.size()-1 && outline_pixels[i] != Point(0,0))
		{
			if ((elbow_feature.x - *arm_width - outline_pixels[i].x) > 5)	//iterate faster if the goal pixel is far away
			{
				i += 10;
			}
			i++;
		}

	
		while (shape_pixels[j].x < elbow_feature.x - *arm_width && shape_pixels[j] != Point(0,0))
		{
			if ((elbow_feature.x - *arm_width - shape_pixels[j].x) > 5)	//iterate faster if the goal pixel is far away
			{
				j += 100;
			}
			j++;
		}

		curr_pixel = outline_pixels[i]; //get the first valid node
		if (elbow_feature.y >= waist_feature.y) //if looking for the right arm, assumes the right elbow is right of the waist
		{
			curr_pixel = outline_pixels[i - 1];
		}

		while (curr_pixel != Point(0, 0) && dist_iteration <= halfway_dist / 2) //examine neighbours, and follow the outline path for halfway_dist times
		{
			neighbours[0] = Point(curr_pixel.x + 1, curr_pixel.y + 1); //Lower right
			neighbours[1] = Point(curr_pixel.x + 1, curr_pixel.y); //Lower mid
			neighbours[2] = Point(curr_pixel.x + 1, curr_pixel.y - 1); //Lower left
			neighbours[3] = Point(curr_pixel.x, curr_pixel.y + 1); //right
			neighbours[4] = Point(curr_pixel.x, curr_pixel.y - 1); //left
			neighbours[5] = Point(curr_pixel.x - 1, curr_pixel.y + 1); //Upper right
			neighbours[6] = Point(curr_pixel.x - 1, curr_pixel.y); //Upper mid
			neighbours[7] = Point(curr_pixel.x - 1, curr_pixel.y - 1); //Upper left

			for (i = 0; i < 8; i++)
			{
				if (is_within_bound(neighbours[i], contours->rows, contours->cols)) //don't check out of bounds neighbours
				{
					if (contours->at<Vec3b>(neighbours[i].x, neighbours[i].y) == Vec3b(0, 0, 255) && neighbours[i] != prev_valid_pixel)
					{
						prev_valid_pixel = curr_pixel;
						curr_pixel = neighbours[i];
						//dist_iteration++;

						angle = atan2(curr_pixel.y - prev_valid_pixel.y, curr_pixel.x - prev_valid_pixel.x);
						average_angle += angle;
						break;
					}
				}
			}
			dist_iteration++;

		}
	}
	catch (Exception e)
	{
		cout << "Unable to find hand feature." << endl;
	}

	average_angle = average_angle / dist_iteration;
	best_fit_node = Point(elbow_feature.x + halfway_dist * cos(average_angle), elbow_feature.y + halfway_dist * sin(average_angle));
	hand_node = find_closest_pixel(shape_pixels, best_fit_node, elbow_feature.x + halfway_dist, j);

	return hand_node;
}

Point PeopleFinder::find_closest_pixel(vector<Point> shape_pixels, Point goal_node, int x_bound, int n) //finds the closest pixel IF THE GOAL NODE IS KNOWN
{
	Point best_fit_node;
	double distx, disty;
	double current_dist;
	double best_dist = 1000;
	
	n += 200; //assume you won't find an ideal pixel in the first 200 iterations (increase speed)

	while (shape_pixels[n] != Point(0, 0) && shape_pixels[n].x <= x_bound)
	{
		n++;
		if (shape_pixels[n] == goal_node)
		{
			best_fit_node = shape_pixels[n];
			break;
		}
		distx = (goal_node.x - shape_pixels[n].x) * (goal_node.x - shape_pixels[n].x);
		disty = (goal_node.y - shape_pixels[n].y) * (goal_node.y - shape_pixels[n].y);
		current_dist = sqrt(distx + disty);
		if (current_dist <= best_dist)
		{
			best_dist = current_dist;
			best_fit_node = shape_pixels[n];
		}
	}

	return best_fit_node;
}

void PeopleFinder::draw_skeleton(Mat * image, vector<Point> nodes)
{
	int i;

	try
	{
		line(*image, Point(nodes[0].y, nodes[0].x), Point(nodes[1].y, nodes[1].x), Scalar(255, 0, 255));	//head to feet
		line(*image, Point(nodes[1].y, nodes[1].x), Point(nodes[2].y, nodes[2].x), Scalar(255, 0, 255));
		line(*image, Point(nodes[2].y, nodes[2].x), Point(nodes[3].y, nodes[3].x), Scalar(255, 0, 255));
		line(*image, Point(nodes[2].y, nodes[2].x), Point(nodes[4].y, nodes[4].x), Scalar(255, 0, 255));

		line(*image, Point(nodes[1].y, nodes[1].x), Point(nodes[5].y, nodes[5].x), Scalar(255, 0, 255)); //left side
		line(*image, Point(nodes[5].y, nodes[5].x), Point(nodes[7].y, nodes[7].x), Scalar(255, 0, 255));
		line(*image, Point(nodes[7].y, nodes[7].x), Point(nodes[8].y, nodes[8].x), Scalar(255, 0, 255));

		line(*image, Point(nodes[1].y, nodes[1].x), Point(nodes[6].y, nodes[6].x), Scalar(255, 0, 255));	//right side
		line(*image, Point(nodes[6].y, nodes[6].x), Point(nodes[9].y, nodes[9].x), Scalar(255, 0, 255));
		line(*image, Point(nodes[9].y, nodes[9].x), Point(nodes[10].y, nodes[10].x), Scalar(255, 0, 255));

		for (i = 0; i < 11; i++)
		{
			if (nodes[i] != Point(0, 0))
			{
				image->at<Vec3b>(nodes[i].x, nodes[i].y) = Vec3b(0, 255, 0);
				circle(*image, Point(nodes[i].y, nodes[i].x), 2, Scalar(0, 255, 0));
			}
		}
	}
	catch (Exception e)
	{
		cout << "Bad Skeleton, feature out of bounds." << endl;
		//save_image(*image , "faulty_imgs/");
	}
}

bool PeopleFinder::is_within_bound(Point node, int x_bound, int y_bound)
{
	return (node.x >= 0 && node.x < x_bound && node.y >= 0 && node.y < y_bound);
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

void PeopleFinder::save_image(Mat image, string folder, int i)
{
	string str = folder;	//TEMPORARY SAVING CODE
	stringstream ss;
	time_t seconds;

	time(&seconds);
	ss << seconds;
	str.append(ss.str());
	ss << i;
	str.append(ss.str());

	str.append(".png");
	imwrite(str, image);
}

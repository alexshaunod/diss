#include "blobdetector.h"

/**
 *	@file blobdetector.cpp
 *  @desc Receives BGS frames or ground truth images and highlights contours within the 
 *  images. Applies convex hulls to the contours to distinguish larger shapes which are saved
 *  and sent to the PeopleFinder.
 *
 *  @param vector<Mat> src_shapes - the source images of the larger shapes
 *
 *	@author Alex O'Donnell
 *	@version 1.00
 */

BlobDetector::BlobDetector(vector<Mat> src_shapes)
	: src_shapes(src_shapes)
{}

/**
 *  @desc Highlights contours and applies OpenCV's convexHull function. Using the area
 *  of the hulls the larger shapes are extracted.
 *
 *  @param Mat *frame - the source frame
 *  @param Mat *fgmask - the BGS frame
 *  @param Mat *contoursonly - the contour frame
 *
 *  @returns Mat drawn_contours - contour data for drawing process
 */
Mat BlobDetector::highlight_contours(Mat *frame, Mat *fgmask, Mat *contoursonly)
{
	hull_size = 0;
	Mat drawn_contours;
	RNG rng(12345);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(*fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> local_hull_list(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		if (contourArea(contours[i]) > 300) //threshold for the number of contours to be drawn, excludes smaller shapes
		{
			convexHull(Mat(contours[i]), local_hull_list[hull_size], false);
			hull_size++;
		}
	}

	drawn_contours = Mat::zeros(frame->size() , CV_8UC3);
	*contoursonly = Mat::zeros(frame->size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		draw_annotations(frame, &drawn_contours, contoursonly, contours, hierarchy, local_hull_list, i);
	}

	hull_list = local_hull_list;

	return drawn_contours;
}


/**
 *  @desc Draws the contour information on the different frames 
 *
 *  @param Mat *frame - the source frame
 *  @param Mat *drawn_contours - the contour data
 *  @param Mat *contoursonly - visualisation of the contours
 *  @param vector<vector<Point>> contours - x/y positions of each end of the contour lines
 *  @param vector<Vec4i> hierachy - information on image topology
 *  @param vector<vector<Point>> - x/y positions of each end of the hull
 *  @param int i - current contour
 */
void BlobDetector::draw_annotations(Mat *frame, Mat *drawn_contours, Mat *contoursonly, vector<vector<Point>> contours, vector<Vec4i> hierarchy, vector<vector<Point>> hull, int i)
{
	drawContours(*drawn_contours,
		contours,
		i,
		Scalar(0, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
	drawContours(*contoursonly,
		contours,
		i,
		Scalar(0, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
	drawContours(*drawn_contours,
		hull,
		i,
		Scalar(255, 0, 255),
		1,
		8,
		hierarchy,
		0,
		Point());
}


/**
 *  @desc Examines the shapes within the convex hulls, saves them as their own images and
 *  resizes them to make them compatible with PeopleFinder. Reapplies the contours after resizing 
 *  to prevent distortion
 *
 *  @param Mat *src_image - the source frame
 *  @param Mat *filtered_mask - the BGS frame
 *  @param vector<vector<Point>> hull - x/y positions of each end of the hull
 *  @param int hullsize - number of hulls in the current image
 *  @param int edge_space - space between the shape and the edge of the image
 *
 *  @returns vector<Mat> shapes - larger shapes
 */
vector<Mat> BlobDetector::get_large_shapes(Mat *src_image, Mat *filtered_mask, vector<vector<Point>> hull, int hullsize, int edge_space)
{
	vector<Mat> shapes(30), bg_shapes(30);
	Point topleft = Point(filtered_mask->rows,filtered_mask->cols);
	Point botright = Point(0,0);
	Rect roi;
	Mat localsrc = *src_image;
	Mat localfilter = *filtered_mask;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	for (int i = 0; i < hullsize; i++)
	{
		for (int k = 0; k < hull[i].size(); k++) 
		{
			if (hull[i][k].x < topleft.x)
			{
				topleft.x = hull[i][k].x;
			}

			if (hull[i][k].x > botright.x)
			{
				botright.x = hull[i][k].x;
			}

			if (hull[i][k].y < topleft.y)
			{
				topleft.y = hull[i][k].y;
			}

			if (hull[i][k].y > botright.y)
			{
				botright.y = hull[i][k].y;
			}
		}
		//rectangle(*filtered_mask, topleft, botright, Vec3b(255, 255, 255));

		if (botright.x != 0 && topleft.x != 0)	
		{
			if (is_within_bound(Point(topleft.x - edge_space, topleft.y - edge_space), filtered_mask->cols, filtered_mask->rows) && 
				is_within_bound(Point(botright.x + edge_space, botright.y + edge_space), filtered_mask->cols, filtered_mask->rows))
			{
				topleft.x -= edge_space;
				topleft.y -= edge_space;
				botright.x += edge_space;
				botright.y += edge_space;
			}
			roi = Rect(topleft.x, topleft.y, botright.x - topleft.x, botright.y - topleft.y);
			src_shapes[i] = localsrc(roi);	//GET THE SOURCE OF THE SHAPE FOR THE RECORD
			resize(localfilter(roi), bg_shapes[i], Size(64, 128));

			findContours(bg_shapes[i], contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));	//draw contours around the image again for clearer outlines.
			shapes[i] = Mat::zeros(bg_shapes[i].size(), CV_8UC3);
			for (int n = 0; n < contours.size(); n++)
			{
				drawContours(shapes[i], contours, n, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
			}
		}

		topleft = Point(filtered_mask->rows, filtered_mask->cols);
		botright = Point(0, 0);
	}

	return shapes;
}

vector<Mat> BlobDetector::get_src_shapes()
{
	return src_shapes;
}

bool BlobDetector::is_within_bound(Point node, int x_bound, int y_bound)
{
	return (node.x >= 0 && node.x < x_bound && node.y >= 0 && node.y < y_bound);
}

vector<vector<Point>> BlobDetector::get_hull_list()
{
	return hull_list;
}

int BlobDetector::get_hull_size()
{
	return hull_size;
}


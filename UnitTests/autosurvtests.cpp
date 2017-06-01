#include "stdafx.h"
#include "CppUnitTest.h"
#include "\AutoSurvCV\peoplefinder.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace cv;
using namespace std;

/**
 *	@file autosurvtests.cpp	
 *	@desc Contains a series of unit tests based around the PeopleFinder classifier. 
 * 
 *	@author Alex O'Donnell
 *	@version 1.00
 *	@subpackage AutoSurvCV.lib
 */

namespace UnitTests
{		
	/**
	 * @desc Test class loads a "good" image that is pre processed with contours and filled pixels
	 * which is then used to test each body part finder function to draw the skeleton. 
	 *
	 * The class also tests whether the PeopleFinder can handle a "bad" image that could 
	 * raise exceptions.
	 */
	TEST_CLASS(AutoSurvTests)
	{
	public:
		PeopleFinder pf = PeopleFinder(vector<Point>(11), vector<Point>(11), "", false);
		Mat img;
		vector<Point> nodes;
		vector<Point> shape_pixels;
		vector<Point> outline_pixels;
		Point halfway_node = Point(1000, 1000);
		int arm_width;
		int index_head = 0, index_torso = 0, index_waist = 0, index_shoulders = 0;
		double halfway_dist;

		/**
		 * @desc Retrieves the path for the "good" image within the AutoSurvCV directory
		 * and attempts to load it.
		 *
		 * @returns Will pass if the image loaded, will fail if the image is not present.
		 */
		TEST_METHOD(LoadGoodImageForTestingTest)
		{
			char full[_MAX_PATH];
			string workdirectory, tempstr;
			_fullpath(full, ".\\", _MAX_PATH);

			workdirectory = full;
			tempstr = workdirectory.substr(0, workdirectory.find("x64", 0));
			replace(tempstr.begin(), tempstr.end(), '\\', '/');
			tempstr.append("test_good.png");

			img = imread(tempstr);

			Assert::IsFalse(img.empty(), 
				L"Couldn't find the image file within the AutoSurvCV\ folder. Make sure the image has the contours ready and already filled. (AutoSurvCV/test_good.png)");
		}

		/**
		 * @desc Retrieves the path for the "bad" image within the AutoSurvCV directory
		 * and attempts to load it. Then runs the entire create_skeleton() function.
		 *
	 	 * @returns Will pass if the bad skeleton flag is raised, will fail if the image is not present
		 * or the create_skeleton() function fails.
		 */
		TEST_METHOD(LoadAndHandleBadImageTest)
		{
			char full[_MAX_PATH];
			string workdirectory, tempstr;
			_fullpath(full, ".\\", _MAX_PATH);

			workdirectory = full;
			tempstr = workdirectory.substr(0, workdirectory.find("x64", 0));
			replace(tempstr.begin(), tempstr.end(), '\\', '/');
			tempstr.append("test_bad.png");

			img = imread(tempstr);
			pf.create_skeleton(&img);

			Assert::IsTrue(pf.get_bad_flag(), L"Program couldn't handle the bad image, check the image and try again. (AutoSurvCV/test_bad.png)");
		}

		/**
		 * @desc Stores the pixels that appear inside/on the outline of the shape as Points
		 * to use for the feature detection functions.
		 */
		TEST_METHOD(HighlightingAndStoringPixelDataTest)
		{
			LoadGoodImageForTestingTest();
			shape_pixels = vector<Point>(8192);
			outline_pixels = vector<Point>(4096);
			nodes = vector<Point>(11);

			pf.highlight_pixels(&img, &shape_pixels, &outline_pixels);
		}

		/**
		 * @desc Runs the head detection function on the "good" image
		 *
		 * @returns Will pass if the head node falls within the boundaries of the image
	 	 */
		TEST_METHOD(PlotHeadFeatureTest)
		{
			HighlightingAndStoringPixelDataTest();
			nodes[0] = pf.find_head_feature(shape_pixels, 5, &index_head);

			Assert::IsTrue(pf.is_within_bound(nodes[0], 0, 0, img.rows, img.cols), 
				L"Can't find head feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the torso detection function on the "good" image
		 *
		 * @returns Will pass if the torso node falls within the boundaries of the image
		 */
		TEST_METHOD(PlotTorsoFeatureTest)
		{
			PlotHeadFeatureTest();
			nodes[1] = pf.find_torso_feature(shape_pixels, 5, nodes[0], index_head, &index_torso);

			Assert::IsTrue(pf.is_within_bound(nodes[1], 0, 0, img.rows, img.cols),
				L"Can't find torso feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the waist detection function on the "good" image
		 *
		 * @returns Will pass if the waist node falls within the boundaries of the image
	 	 */
		TEST_METHOD(PlotWaistFeatureTest)
		{
			PlotTorsoFeatureTest();
			nodes[2] = pf.find_waist_feature(shape_pixels, 5, nodes[1], index_torso, &index_waist);

			Assert::IsTrue(pf.is_within_bound(nodes[2], 0, 0, img.rows, img.cols),
				L"Can't find waist feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the foot detection function on the "good" image for both feet
		 *
		 * @returns Will pass if both nodes fall within the boundaries of the image
	 	 */
		TEST_METHOD(PlotFeetFeatureTest)
		{
			PlotWaistFeatureTest();

			nodes[3] = pf.find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 1), index_waist);
			nodes[4] = pf.find_foot_feature(shape_pixels, 5, nodes[2], Point(127, 63), index_waist);

			Assert::IsTrue(pf.is_within_bound(nodes[3], 0, 0, img.rows, img.cols) && (pf.is_within_bound(nodes[4], 0, 0, img.rows, img.cols)),
				L"Can't find feet features/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Calculates the distance between the torso and the waist and halves it for 
		 * arm detection, sets the shoulder positions which are also necessary for the 
		 * remaining features.
		 *
		 * @returns Will pass if both nodes fall within the boundaries of the image
		 */
		TEST_METHOD(PlotShouldersFeatureTest)
		{
			PlotFeetFeatureTest();
			pf.calc_halfway_torso_dist(nodes[1], nodes[2], &halfway_node, &halfway_dist);
			pf.set_shoulder_positions(shape_pixels, 5, nodes[1], &nodes[5], &nodes[6], &arm_width, index_torso, &index_shoulders);

			Assert::IsTrue(pf.is_within_bound(nodes[5], 0, 0, img.rows, img.cols) && (pf.is_within_bound(nodes[6], 0, 0, img.rows, img.cols)),
				L"Can't find shoulder features/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the elbow detection function on the "good" image for the left elbow
		 *
		 * @returns Will pass if the node falls within the boundaries of the image
		 */
		TEST_METHOD(PlotLeftElbowFeatureTest)
		{
			PlotShouldersFeatureTest();
			nodes[7] = pf.find_elbow_feature(shape_pixels, nodes[1], nodes[2], nodes[5], &arm_width, halfway_dist, halfway_node, index_shoulders);

			Assert::IsTrue(pf.is_within_bound(nodes[7], 0, 0, img.rows, img.cols),
				L"Can't find left elbow feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the hand detection function on the "good" image for the left hand
		 *
		 * @returns Will pass if the node falls within the boundaries of the image
		 */
		TEST_METHOD(PlotLeftHandFeatureTest) 
		{
			PlotLeftElbowFeatureTest();
			nodes[8] = pf.find_hand_feature(shape_pixels, outline_pixels, nodes[2], nodes[7], &arm_width, halfway_dist, halfway_node, &img, index_shoulders);

			Assert::IsTrue(pf.is_within_bound(nodes[8], 0, 0, img.rows, img.cols),
				L"Can't find left hand feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the elbow detection function on the "good" image for the right elbow
		 *
	  	 * @returns Will pass if the node falls within the boundaries of the image
		 */
		TEST_METHOD(PlotRightElbowFeatureTest)
		{
			PlotLeftHandFeatureTest();
			nodes[9] = pf.find_elbow_feature(shape_pixels, nodes[1], nodes[2], nodes[6], &arm_width, halfway_dist, halfway_node, index_shoulders);

			Assert::IsTrue(pf.is_within_bound(nodes[9], 0, 0, img.rows, img.cols),
				L"Can't find right elbow feature/out of bounds, check to see if the image is valid.");
		}

		/**
		 * @desc Runs the hand detection function on the "good" image for the right hand
		 *
		 * @returns Will pass if the node falls within the boundaries of the image
		 */
		TEST_METHOD(PlotRightHandFeatureTest)
		{
			PlotRightElbowFeatureTest();
			nodes[10] = pf.find_hand_feature(shape_pixels, outline_pixels, nodes[2], nodes[9], &arm_width, halfway_dist, halfway_node, &img, index_shoulders);

			Assert::IsTrue(pf.is_within_bound(nodes[10], 0, 0, img.rows, img.cols),
				L"Can't find right hand feature/out of bounds, check to see if the image is valid.");
		}

		/**
	 	 * @desc Draws the nodes onto the image to create the final skeleton
		 */
		TEST_METHOD(AnnotateFeatureSkeletonTest)
		{
			PlotRightHandFeatureTest();
			pf.draw_skeleton(&img, nodes);
		}
	};
}
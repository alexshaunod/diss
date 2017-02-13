#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "screenobserver.h"

using namespace cv;
using namespace std;


int main()
{
	int i, j;
	ScreenObserver scrOb = ScreenObserver();
	scrOb.printSomething();
	
	/*
	Mat img = imread("tiny.png", CV_LOAD_IMAGE_UNCHANGED);
	for (i=0;i<img.rows;i++)
	{
		for (j=0;j<img.cols;j++)
		{
			cout << img.at<Vec3b>(i, j) << " ";
		}
		cout << endl;
	}
	*/
	/*
	Mat img = imread("test.JPG", CV_LOAD_IMAGE_UNCHANGED); //read the image data in the file "MyPic.JPG" and store it in 'img'
	cout << img.type();
	if (img.empty()) //check whether the image is loaded or not
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		//system("pause"); //wait for a key press
		return -1;
	}
	
	namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
	imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

	waitKey(0); //wait infinite time for a keypress
	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
	*/
	std::cin.ignore();
	

	return 0;
	
}
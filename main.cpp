#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "screenobserver.h"
#include "visualoverlay.h"
#include "haarcascade.h"
#include "hogcascade.h"
#include "bgs.h"
#include "peoplefinder.h"

using namespace cv;
using namespace std;


int main()
{

	/* ENHANCE DEMO
	Mat gx, gy, mag, angle, img;
	Mat oldimg = imread("DataSets/enhance_blob_test.png", CV_LOAD_IMAGE_UNCHANGED);
	resize(oldimg, img, Size(oldimg.rows*2, oldimg.cols*2));
	img.convertTo(img, CV_32F, 1 / 255.0);

	Sobel(img, gx, CV_32F, 1, 0, 1); //horizontal kernel
	Sobel(img, gy, CV_32F, 0, 1, 1); //vertical kernel

	cartToPolar(gx, gy, mag, angle, 1);

	imshow("MyWindow", img);
	imshow("mag", mag);

	waitKey(0); //wait infinite time for a keypress
	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
	*/

	//HogCascade hc = HogCascade();
	//HaarCascade hc = HaarCascade();
	//hc.run();

	PeopleFinder pf = PeopleFinder();
	pf.run();
	//BGS bgs = BGS();
	//bgs.run();

	/*
	int i, j;
	Mat img;
	ScreenObserver scrOb = ScreenObserver();
	VisualOverlay vo = VisualOverlay();
	HWND hwndDesktop = GetDesktopWindow();

	scrOb.tempTest();
	img = scrOb.screenToMatrix(hwndDesktop);

	vo.drawRectangle(img,
		scrOb.getPositionX(),
		scrOb.getPositionY(),
		scrOb.getPositionX() + scrOb.getWidth(),
		scrOb.getPositionY() + scrOb.getHeight()
	);

	namedWindow("Placeholder Name", CV_WINDOW_AUTOSIZE);

	for (i = 0; i < 100; i++)
	{
		img = scrOb.screenToMatrix(hwndDesktop);
		vo.highlightViewArea(hwndDesktop,
			scrOb.getPositionX(),
			scrOb.getPositionY(),
			scrOb.getWidth(),
			scrOb.getHeight()
			);
		scrOb.displayImage(img, "Placeholder Name");
		waitKey(0);
	}

	destroyWindow("Placeholder Name");

	std::cout << "app running\n";
	*/
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
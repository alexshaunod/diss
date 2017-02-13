#include <iostream>
#include <windows.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "screenobserver.h"
#include "visualoverlay.h"

using namespace cv;
using namespace std;

ScreenObserver::ScreenObserver(int w, int h, int px, int py)
: width(w), height(h), positionX(px), positionY(py)
{}

void ScreenObserver::tempTest() 
{
	width = 640;
	height = 355;
	positionX = 310;
	positionY = 355;
	
}

Mat ScreenObserver::screenToMatrix(HWND hwnd) //Modified version of John Ktejik's solution found at: http://stackoverflow.com/a/14167433
{
	Mat screenShot;
	HDC hwindowDC, hwindowCompatibleDC;

	int srcheight, srcwidth; //width and height of the captured image, will be the same as user input to prevent resizing the image
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd); //gets the device context of the desktop window
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	srcheight = height;
	srcwidth = width;

	screenShot.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //determines which way round the image is drawn
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC,
		0,
		0,
		width,
		height,
		hwindowDC,
		positionX,
		positionY,
		srcwidth,
		srcheight,
		SRCCOPY
	); //CAN SET WINDOW POSITION HERE
	GetDIBits(hwindowCompatibleDC,
		hbwindow,
		0,
		height,
		screenShot.data,
		(BITMAPINFO *)&bi,
		DIB_RGB_COLORS
	);  //copy from hwindowCompatibleDC to hbwindow
																									  
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC); // avoid memory leak

	return screenShot;	
}

void ScreenObserver::displayImage(Mat img, const String wname) 
{
	imshow(wname, img); 
}

int ScreenObserver::getWidth()
{
	return width;
}

int ScreenObserver::getHeight()
{
	return height;
}

int ScreenObserver::getPositionX()
{
	return positionX;
}

int ScreenObserver::getPositionY()
{
	return positionY;
}

void ScreenObserver::setWidth(int w)
{
	width = w;
}

void ScreenObserver::setHeight(int h)
{
	height = h;
}

void ScreenObserver::setPositionX(int px)
{
	positionX = px;
}

void ScreenObserver::setPositionY(int py)
{
	positionY = py;
}

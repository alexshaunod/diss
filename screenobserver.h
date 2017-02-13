#ifndef SCREENOBSERVER_H
#define SCREENOBSERVER_H

#include <windows.h>

using namespace cv;

class ScreenObserver
{
	private :
		int width;
		int height;
		int positionX;
		int positionY;
	public :
		ScreenObserver(int w = 0, int h = 0, int px = 0, int py = 0);
		void tempTest();
		Mat screenToMatrix(HWND hwnd);
		void displayImage(Mat img, const String wname);
		int getWidth();
		int getHeight();
		int getPositionX();
		int getPositionY();
		void setWidth(int w);
		void setHeight(int h);
		void setPositionX(int px);
		void setPositionY(int py);
};

#endif

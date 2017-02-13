#ifndef SCREENOBSERVER_H
#define SCREENOBSERVER_H

#include <Windows.h>

class ScreenObserver
{
	private :
		int width;
		int height;
		int positionX;
		int positionY;
	public :
		ScreenObserver(int w = 0, int h = 0, int px = 0, int py = 0);
		void printSomething();
		cv::Mat screenToMatrix(HWND hwnd);
		void displayImage(cv::Mat img);
};

#endif

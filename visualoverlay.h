#ifndef VISUALOVERLAY_H
#define VISUALOVERLAY_H

#include <windows.h>

using namespace cv;

class VisualOverlay
{
	public:
		VisualOverlay();
		void highlightViewArea(HWND desktop, int width, int height, int positionX, int positionY);
		void drawRectangle(Mat img, int upperLeftX, int upperLeftY, int lowerRightX, int lowerRightY);
};

#endif


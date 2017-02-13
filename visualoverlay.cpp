#include <windows.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "visualoverlay.h"

using namespace cv;

VisualOverlay::VisualOverlay()
{
}

void VisualOverlay::highlightViewArea(HWND desktop, int width, int height, int positionX, int positionY)
{
	HDC currentWindow = GetDC(desktop);
	RECT rect = { positionX, positionY, width, height };
	HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
	FillRect(currentWindow, &rect, blueBrush);
}

void VisualOverlay::drawRectangle(Mat img, int upperLeftX, int upperLeftY, int lowerRightX, int lowerRightY)
{
	img.create(700, 450, CV_8UC4);
	rectangle(img,
		Point(upperLeftX, upperLeftY),
		Point(lowerRightX, lowerRightY),
		Scalar(0, 255, 255),
		3,
		LINE_8
	);
}

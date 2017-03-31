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
	//HogCascade hc = HogCascade();
	//HaarCascade hc = HaarCascade();
	//hc.run();

	PeopleFinder pf = PeopleFinder(vector<Point>(11), vector<Point>(11), false);
	pf.run();
	//BGS bgs = BGS();
	//bgs.run();
	std::cin.ignore();

	return 0;
	
}
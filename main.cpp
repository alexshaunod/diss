#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "bgs.h"
#include "peoplefinder.h"

using namespace cv;
using namespace std;

int main()
{
	BGS bgs = BGS();
	bgs.run();
	std::cin.ignore();

	return 0;
	
}
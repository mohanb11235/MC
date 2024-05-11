#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//#define work 1
#ifdef work



int main()
{
	string path = "D://BLending_code_python//JAN_2024_IMAGES_CODE//blended_screen_1.jpg";
	Mat img = imread(path);
	imshow("Image", img);
	waitKey(0);
	destroyAllWindows();
	return 0;
}

#endif
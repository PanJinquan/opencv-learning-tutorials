#include<opencv2/opencv.hpp>
#include<string>
#include "Debug.h"
#include "myhello.h"

using namespace std;
void main() {
	string path = "E:/SSGF/dataset/images/geda.png";
	cv::Mat image = cv::imread(path);
	PrintHelloWorld();
	DEBUG_IMSHOW("image", image);
	cv::waitKey(0);
}
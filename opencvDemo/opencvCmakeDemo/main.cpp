#include<opencv2/opencv.hpp>
#include<string>
#include "Debug.h"
#include "myhello.h"

using namespace std;
int main() {
	string path = "../1.jpg";
	cv::Mat image = cv::imread(path);
	PrintHelloWorld();
	DEBUG_IMSHOW("image", image);
	cv::waitKey(0);
	return 0;
}

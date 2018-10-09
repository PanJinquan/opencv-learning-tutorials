#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

const size_t width = 300;
const size_t height = 300;
String labelFile = "D:/opencv-3.3/opencv/sources/samples/data/dnn/pascal-classes.txt";
String modelFile = "D:/opencv-3.3/opencv/sources/samples/data/dnn/fcn8s-heavy-pascal.caffemodel";
String model_text_file = "D:/opencv-3.3/opencv/sources/samples/data/dnn/fcn8s-heavy-pascal.prototxt";

vector<Vec3b> readColors();
int main(int argc, char** argv) {
	Mat frame = imread("D:/opencv-3.3/opencv/sources/samples/data/dnn/rgb.jpg");
	if (frame.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", frame);
	resize(frame, frame, Size(500, 500));
	vector<Vec3b> colors = readColors();

	// init net
	Net net = readNetFromCaffe(model_text_file, modelFile);
	Mat blobImage = blobFromImage(frame);

	// use net
	float time = getTickCount();
	net.setInput(blobImage, "data");
	Mat score = net.forward("score");
	float tt = getTickCount() - time;
	printf("time consume: %.2f \n", (tt / getTickFrequency()) * 1000);
	
	// segmentation and display
	const int rows = score.size[2];
	const int cols = score.size[3];
	const int chns = score.size[1];
	Mat maxCl(rows, cols, CV_8UC1);
	Mat maxVal(rows, cols, CV_32FC1);

	// setup LUT
	for (int c = 0; c < chns; c++) {
		for (int row = 0; row < rows; row++) {
			const float *ptrScore = score.ptr<float>(0, c, row);
			uchar *ptrMaxCl = maxCl.ptr<uchar>(row);
			float *ptrMaxVal = maxVal.ptr<float>(row);
			for (int col = 0; col < cols; col++) {
				if(ptrScore[col] > ptrMaxVal[col]) {
					ptrMaxVal[col] = ptrScore[col];
					ptrMaxCl[col] = (uchar)c;
				}
			}
		}
	}

	// look up colors
	Mat result = Mat::zeros(rows, cols, CV_8UC3);
	for (int row = 0; row < rows; row++) {
		const uchar *ptrMaxCl = maxCl.ptr<uchar>(row);
		Vec3b *ptrColor = result.ptr<Vec3b>(row);
		for (int col = 0; col < cols; col++) {
			ptrColor[col] = colors[ptrMaxCl[col]];
		}
	}
	Mat dst;
	addWeighted(frame, 0.3, result, 0.7, 0, dst);
	imshow("FCN-demo", dst);

	waitKey(0);
	return 0;
}

vector<Vec3b> readColors() {
	vector<Vec3b> colors;
	ifstream fp(labelFile);
	if (!fp.is_open()) {
		printf("could not open the file...\n");
		exit(-1);
	}
	string line;
	while (!fp.eof()) {
		getline(fp, line);
		if (line.length()) {
			stringstream ss(line);
			string name;
			ss >> name;
			int temp;
			Vec3b color;
			ss >> temp;
			color[0] = (uchar)temp;
			ss >> temp;
			color[1] = (uchar)temp;
			ss >> temp;
			color[2] = (uchar)temp;
			colors.push_back(color);
		}
	}
	return colors;
}
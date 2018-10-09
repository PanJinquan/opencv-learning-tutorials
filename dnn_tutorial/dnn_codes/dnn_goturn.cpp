#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

String  goturn_model = "D:/vcprojects/images/dnn/goturn_files/goturn.caffemodel";
String goturn_prototxt = "D:/vcprojects/images/dnn/goturn_files/goturn.prototxt.txt";

Net net;
void initGoturn();
Rect trackObjects(Mat &frame, Mat &prevFrame);
Mat frame, prevFrame;
Rect prevBB;
int main(int argc, char** argv) {
	initGoturn();
	VideoCapture capture;
	capture.open("D:/vcprojects/images/visit.mp4");
	capture.read(frame);
	frame.copyTo(prevFrame);
	prevBB = selectROI(frame, true, true);
	namedWindow("frame", CV_WINDOW_AUTOSIZE);
	while (capture.read(frame)) {
		Rect currentBB = trackObjects(frame, prevFrame);
		rectangle(frame, currentBB, Scalar(0, 0, 255), 2, 8, 0);

		// ready for next frame
		frame.copyTo(prevFrame);
		prevBB.x = currentBB.x;
		prevBB.y = currentBB.y;
		prevBB.width = currentBB.width;
		prevBB.height = currentBB.height;

		imshow("frame", frame);
		char c = waitKey(50);
		if (c == 27) {
			break;
		}
	}
}

void initGoturn() {
	Ptr<Importer> importer;
	importer = createCaffeImporter(goturn_prototxt, goturn_model);
	importer->populateNet(net);
	importer.release();
}

Rect trackObjects(Mat& frame, Mat &prevFrame) {
	Rect rect;
	int INPUT_SIZE = 227;
	//Using prevFrame & prevBB from model and curFrame GOTURN calculating curBB
	Mat curFrame = frame.clone();
	Rect2d curBB;

	float padTargetPatch = 2.0;
	Rect2f searchPatchRect, targetPatchRect;
	Point2f currCenter, prevCenter;
	Mat prevFramePadded, curFramePadded;
	Mat searchPatch, targetPatch;

	prevCenter.x = (float)(prevBB.x + prevBB.width / 2);
	prevCenter.y = (float)(prevBB.y + prevBB.height / 2);

	targetPatchRect.width = (float)(prevBB.width*padTargetPatch);
	targetPatchRect.height = (float)(prevBB.height*padTargetPatch);
	targetPatchRect.x = (float)(prevCenter.x - prevBB.width*padTargetPatch / 2.0 + targetPatchRect.width);
	targetPatchRect.y = (float)(prevCenter.y - prevBB.height*padTargetPatch / 2.0 + targetPatchRect.height);

	copyMakeBorder(prevFrame, prevFramePadded, (int)targetPatchRect.height, (int)targetPatchRect.height, (int)targetPatchRect.width, (int)targetPatchRect.width, BORDER_REPLICATE);
	targetPatch = prevFramePadded(targetPatchRect).clone();

	copyMakeBorder(curFrame, curFramePadded, (int)targetPatchRect.height, (int)targetPatchRect.height, (int)targetPatchRect.width, (int)targetPatchRect.width, BORDER_REPLICATE);
	searchPatch = curFramePadded(targetPatchRect).clone();

	//Preprocess
	//Resize
	resize(targetPatch, targetPatch, Size(INPUT_SIZE, INPUT_SIZE));
	resize(searchPatch, searchPatch, Size(INPUT_SIZE, INPUT_SIZE));

	//Mean Subtract
	targetPatch = targetPatch - 128;
	searchPatch = searchPatch - 128;

	//Convert to Float type
	targetPatch.convertTo(targetPatch, CV_32F);
	searchPatch.convertTo(searchPatch, CV_32F);

	Mat targetBlob = blobFromImage(targetPatch);
	Mat searchBlob = blobFromImage(searchPatch);

	net.setInput(targetBlob, ".data1");
	net.setInput(searchBlob, ".data2");

	Mat res = net.forward("scale");
	Mat resMat = res.reshape(1, 1);
	//printf("width : %d, height : %d\n", (resMat.at<float>(2) - resMat.at<float>(0)), (resMat.at<float>(3) - resMat.at<float>(1)));

	curBB.x = targetPatchRect.x + (resMat.at<float>(0) * targetPatchRect.width / INPUT_SIZE) - targetPatchRect.width;
	curBB.y = targetPatchRect.y + (resMat.at<float>(1) * targetPatchRect.height / INPUT_SIZE) - targetPatchRect.height;
	curBB.width = (resMat.at<float>(2) - resMat.at<float>(0)) * targetPatchRect.width / INPUT_SIZE;
	curBB.height = (resMat.at<float>(3) - resMat.at<float>(1)) * targetPatchRect.height / INPUT_SIZE;

	//Predicted BB
	Rect boundingBox = curBB;
	return boundingBox;
}
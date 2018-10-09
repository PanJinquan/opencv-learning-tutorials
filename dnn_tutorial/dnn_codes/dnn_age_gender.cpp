#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;
String haar_file = "D:/opencv-3.3/opencv/build/etc/haarcascades/haarcascade_frontalface_alt_tree.xml";
String age_model = "D:/vcprojects/images/dnn/cnn_age_gender_models.2/age_net.caffemodel";
String age_text = "D:/vcprojects/images/dnn/cnn_age_gender_models.2/deploy_age.prototxt";

String gender_model = "D:/vcprojects/images/dnn/cnn_age_gender_models.2/gender_net.caffemodel";
String gender_text = "D:/vcprojects/images/dnn/cnn_age_gender_models.2/deploy_gender.prototxt";

void predict_age(Net &net, Mat &image);
void predict_gender(Net &net, Mat &image);
int main(int argc, char** argv) {
	Mat src = imread("D:/vcprojects/images/three.png");
	if (src.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);
	CascadeClassifier detector;
	detector.load(haar_file);
	vector<Rect> faces;
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	detector.detectMultiScale(gray, faces, 1.02, 1, 0, Size(40, 40), Size(200, 200));

	Net age_net = readNetFromCaffe(age_text, age_model);
	Net gender_net = readNetFromCaffe(gender_text, gender_model);

	for (size_t t= 0; t < faces.size(); t++) {
		rectangle(src, faces[t], Scalar(30, 255, 30), 2, 8, 0);
		predict_age(age_net, src(faces[t]));
		predict_gender(age_net, src(faces[t]));
	}
	imshow("age-gender-prediction-demo", src);

	waitKey(0);
	return 0;
}

vector<String> ageLabels() {
	vector<String> ages;
	ages.push_back("0-2");
	ages.push_back("4 - 6");
	ages.push_back("8 - 13");
	ages.push_back("15 - 20");
	ages.push_back("25 - 32");
	ages.push_back("38 - 43");
	ages.push_back("48 - 53");
	ages.push_back("60-");
	return ages;
}

void predict_age(Net &net, Mat &image) {
	// 输入
	Mat blob = blobFromImage(image, 1.0, Size(227, 227));
	net.setInput(blob, "data");
	// 预测分类
	Mat prob = net.forward("prob");
	Mat probMat = prob.reshape(1, 1);
	Point classNum;
	double classProb;

	vector<String> ages = ageLabels();
	minMaxLoc(probMat, NULL, &classProb, NULL, &classNum);
	int classidx = classNum.x;
	putText(image, format("age:%s", ages.at(classidx).c_str()), Point(2, 10), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255), 1);
}

void predict_gender(Net &net, Mat &image) {
	// 输入
	Mat blob = blobFromImage(image, 1.0, Size(227, 227));
	net.setInput(blob, "data");
	// 预测分类
	Mat prob = net.forward("prob");
	Mat probMat = prob.reshape(1, 1);
	putText(image, format("gender:%s", (probMat.at<float>(0, 0) > probMat.at<float>(0, 1) ? "M" : "F")),
		Point(2, 20), FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255), 1);
}

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>

using namespace cv;
using namespace cv::dnn;
using namespace std;

const size_t width = 300;
const size_t height = 300;
String labelFile = "D:/vcprojects/VGGNet/ILSVRC2016/SSD_300x300/labelmap_det.txt";
String modelFile = "D:/vcprojects/VGGNet/ILSVRC2016/SSD_300x300/VGG_ILSVRC2016_SSD_300x300_iter_440000.caffemodel";
String model_text_file = "D:/vcprojects/VGGNet/ILSVRC2016/SSD_300x300/deploy.prototxt";

vector<String> readLabels();
const int meanValues[3] = { 104, 117, 123 };
static Mat getMean(const size_t &w, const size_t &h) {
	Mat mean;
	vector<Mat> channels;
	for (int i = 0; i < 3; i++) {
		Mat channel(h, w, CV_32F, Scalar(meanValues[i]));
		channels.push_back(channel);
	}
	merge(channels, mean);
	return mean;
}

static Mat preprocess(const Mat &frame) {
	Mat preprocessed;
	frame.convertTo(preprocessed, CV_32F);
	resize(preprocessed, preprocessed, Size(width, height)); // 300x300 image
	Mat mean = getMean(width, height);
	subtract(preprocessed, mean, preprocessed);
	return preprocessed;
}

int main(int argc, char** argv) {
	Mat frame = imread("D:/vcprojects/images/persons.png");
	if (frame.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", frame);

	vector<String> objNames = readLabels();
	// import Caffe SSD model
	Ptr<dnn::Importer> importer;
	try {
		importer = createCaffeImporter(model_text_file, modelFile);
	}
	catch (const cv::Exception &err) {
		cerr << err.msg << endl;
	}
	Net net;
	importer->populateNet(net);
	importer.release();

	Mat input_image = preprocess(frame);
	Mat blobImage = blobFromImage(input_image);

	net.setInput(blobImage, "data");
	Mat detection = net.forward("detection_out");
	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	float confidence_threshold = 0.2;
	for (int i = 0; i < detectionMat.rows; i++) {
		float confidence = detectionMat.at<float>(i, 2);
		if (confidence > confidence_threshold) {
			size_t objIndex = (size_t)(detectionMat.at<float>(i, 1));
			float tl_x = detectionMat.at<float>(i, 3) * frame.cols;
			float tl_y = detectionMat.at<float>(i, 4) * frame.rows;
			float br_x = detectionMat.at<float>(i, 5) * frame.cols;
			float br_y = detectionMat.at<float>(i, 6) * frame.rows;

			Rect object_box((int)tl_x, (int)tl_y, (int)(br_x - tl_x), (int)(br_y - tl_y));
			rectangle(frame, object_box, Scalar(0, 0, 255), 2, 8, 0);
			putText(frame, format("%s", objNames[objIndex].c_str()), Point(tl_x, tl_y), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0), 2);
		}
	}
	imshow("ssd-demo", frame);

	waitKey(0);
	return 0;
}

vector<String> readLabels() {
	vector<String> objNames;
	ifstream fp(labelFile);
	if (!fp.is_open()) {
		printf("could not open the file...\n");
		exit(-1);
	}
	string name;
	while (!fp.eof()) {
		getline(fp, name);
		if (name.length() && (name.find("display_name:") == 0)) {
			string temp = name.substr(15);
			temp.replace(temp.end() - 1, temp.end(), "");
			objNames.push_back(temp);
		}
	}
	return objNames;
}
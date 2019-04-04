#pragma once
#include <opencv2/opencv.hpp>
using namespace std;

class MySVM
{
public:
	MySVM();
	~MySVM();
	void load_model(string model_path);
	void save_model(string model_path);
	float svm_predict(cv::Mat &FeatureDatah);
	void svm_train(cv::Mat &trainData, cv::Mat &trainLabels);

	cv::Ptr<cv::ml::SVM> get_model();
private:

public:
	cv::Ptr<cv::ml::SVM> model;

};





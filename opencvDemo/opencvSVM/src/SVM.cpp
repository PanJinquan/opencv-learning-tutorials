#include "SVM.h"
#include <fstream>
#include <string>
#include "vector"
#include <iostream>
#include "file_processing.h"
#include <opencv2/opencv.hpp>
#include "imageFeature.h"
#include "config.h"
using namespace cv::ml;

MySVM::MySVM()
{
	model = SVM::create();
}

MySVM::~MySVM()
{
	model.release();
}
void MySVM::load_model(string model_path) {
	model = cv::ml::SVM::load(loadSVMPath);//从XML文件读取训练好的SVM模型
}
void MySVM::save_model(string model_path) {
	model->save(model_path);
}

cv::Ptr<cv::ml::SVM> MySVM::get_model() {
	return model;
}

void MySVM::svm_train(cv::Mat &trainData, cv::Mat &trainLabels) {
	/************************************************************************/
	//训练SVM分类器
	//迭代终止条件，当迭代满1000次或误差小于FLT_EPSILON时停止迭代
	cv::TermCriteria  criteria = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
	model->setType(SVM::C_SVC);
	//核函数 SIGMOID(40.7%)、LINEAR(93.3%)、RBF(95.3)、CHI2(54.5%),POLY
	//svm->setKernel(SVM::LINEAR);  
	model->setKernel(SVM::RBF);
	model->setTermCriteria(criteria);//设置算法的终止条件
	//设置训练数据   
	trainData.convertTo(trainData, CV_32FC1);
	trainLabels.convertTo(trainLabels, CV_32SC1);
	cv::Ptr<TrainData> tData = TrainData::create(trainData, ROW_SAMPLE, trainLabels);
	// 训练分类器  
	model->train(tData);
	//保存训练结果XML文件
	cout << "***************************SVM训练完毕***************************" << endl;
}

float MySVM::svm_predict(cv::Mat &FeatureData) {
	 //用训练好的SVM分类器对测试图片的特征向量进行分类
	float result = model->predict(FeatureData,cv::noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);//返回类标
	return result;
}
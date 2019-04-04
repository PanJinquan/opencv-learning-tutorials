// imageCheck.cpp : 定义控制台应用程序的入口点。

#include <fstream>
#include <string>
#include "vector"
#include <iostream>
#include "file_processing.h"
#include <opencv2/opencv.hpp>
#include "imageFeature.h"
#include "config.h"
#include "SVM.h"
using namespace std;

//训练
int train(string trian_filename, string image_dir,string saveModel)
{
	MySVM *svm = new MySVM();
	FileData fileData = loadFileData((char *)trian_filename.c_str());
	cv::Mat labelsMat=array2DToMat((int*)fileData.data, txtRows, txtCols-1);
	int sampleNums = fileData.name.size();//样本个数
	int featureDim;//特征维度
	cv::Mat sampleFeatureMat;//所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数	
	cv::Mat sampleLabelMat;//训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，-1表示无人

	for (size_t num = 0; num < sampleNums; num++)
	{
		string image_name = fileData.name.at(num);
		string image_path = image_dir+(string)"/"+ image_name;
		//图像预处理
		cv::Mat image = cv::imread(image_path);//读取图片
		if (image.empty()) {
			std::cout << "no path:"<<image_path << endl;
			continue;
		}
		std::cout << image_path << endl;
		//图像预处理
		image = image_processing(image);
		//提取特征
		vector<float> per_fea = getFeature(image, 0);
		//处理第一个样本时初始化特征向量矩阵和类别矩阵
		if (0 == num)
		{
			std::cout << "特征维数:" << per_fea.size() << endl;
			featureDim = per_fea.size();//HOG描述子的维数
			 //初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数sampleFeatureMat
			sampleFeatureMat = cv::Mat::zeros(sampleNums, featureDim, CV_32FC1);
			//初始化训练样本的类别向量，行数等于所有样本的个数，列数等于1；1表示有人，0表示无人
			sampleLabelMat = cv::Mat::zeros(sampleNums, 1, CV_32FC1);
		}

		//样本特征矩阵sampleFeatureMat
		for (int i = 0; i < featureDim; i++)
			sampleFeatureMat.at<float>(num, i) = per_fea[i];//第num个样本的特征向量中的第i个元素
		sampleLabelMat.at<float>(num, 0) = labelsMat.at<uchar>(num,0);//正样本类别为1，有人
	}

	//设置训练数据和标签
	cv::Mat trainData = sampleFeatureMat;
	cv::Mat trainLable = sampleLabelMat;
	if (doPCA)
	{
		cv::Mat SCORE;
		pcaData data = getPcaFeature(sampleFeatureMat, dimNum, SCORE);
		savePcaPara(pcaPath, data);
		std::cout << "PCA降维:" << dimNum << endl;
		trainData = SCORE;
	}

	svm->svm_train(trainData, trainLable);
	svm->save_model(saveModel);
	return 0;
}

//测试
int test(string model_path, string image_path) {

	MySVM *svm = new MySVM();
	svm->load_model(model_path);
	pcaData pca_data = loadPcaPara(pcaPath);


	std::cout << image_path << endl;
	cv::Mat image = cv::imread(image_path);//读取图片
	 //图像预处理
	image = image_processing(image);
	//提取特征
	vector<float> per_fea = getFeature(image, 0);
	int featureDim = per_fea.size();//HOG描述子的维数
	//样本特征矩阵sampleFeatureMat
	cv::Mat sampleFeatureMat = cv::Mat::zeros(1, featureDim, CV_32FC1);
	for (int i = 0; i < featureDim; i++)
		sampleFeatureMat.at<float>(0, i) = per_fea[i];//第num个样本的特征向量中的第i个元素

	cv::Mat testData = sampleFeatureMat;
	if (doPCA)
	{
		testData = projectionPCA(testData, pca_data);
	}
	float predict=svm->svm_predict(testData);
	std::cout << "识别结果:" << predict << endl;
	cout << "***************************SVM测试完毕***************************" << endl;
	return 0;
}

//批量测试
int batch_test(string test_filename, string image_dir,string model_path) {
	MySVM *svm = new MySVM();
	svm->load_model(model_path);
	pcaData pca_data = loadPcaPara(pcaPath);

	FileData fileData = loadFileData((char *)test_filename.c_str());
	cv::Mat labelsMat = array2DToMat((int*)fileData.data, txtRows, txtCols - 1);
	int sampleNums = fileData.name.size();//样本个数
	int featureDim;//特征维度
	for (size_t num = 0; num < sampleNums; num++)
	{
		string image_name = fileData.name.at(num);
		string image_path = image_dir + (string)"/" + image_name;
		cv::Mat image = cv::imread(image_path);//读取图片
											   //图像预处理
		image = image_processing(image);
		//提取特征
		vector<float> per_fea = getFeature(image, 0);
		int featureDim = per_fea.size();//HOG描述子的维数
		//样本特征矩阵sampleFeatureMat
		cv::Mat sampleFeatureMat = cv::Mat::zeros(1, featureDim, CV_32FC1);
		for (int i = 0; i < featureDim; i++)
			sampleFeatureMat.at<float>(0, i) = per_fea[i];//第num个样本的特征向量中的第i个元素

		cv::Mat testData = sampleFeatureMat;
		if (doPCA)
		{
			testData = projectionPCA(testData, pca_data);
		}
		float predict = svm->svm_predict(testData);
		std::cout << "识别结果:" << predict << endl;
	}
	cout << "***************************SVM测试完毕***************************" << endl;
	return 0;

}
int main() {
	train(train_filename, image_dir, saveSVMPath);
	test(saveSVMPath, test_image);
	batch_test(train_filename, image_dir, saveSVMPath);

}
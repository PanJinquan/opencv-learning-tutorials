#include "config.h"
#include "imageFeature.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <lbpfeatures.h>

cv::Mat image_processing(cv::Mat image) {
	if (image.channels() == 3 && toGray)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	IM_RESIZE(image);
	image = getEdgeImage(image);
	return image;
}
cv::Mat getEdgeImage(cv::Mat image) {
	cv::Mat edgeImage ;
	if (image.channels() == 3 && toGray)
		cv::cvtColor(image, image, CV_RGB2GRAY);
	cv::Mat grad_x, grad_y;
	double scale = 1;//默认1。
	double delta = 0;//默认0。
	//求X方向梯度 
	cv::Sobel(image, grad_x, image.depth(), 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	//求Y方向梯度 
	cv::Sobel(image, grad_y, image.depth(), 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, grad_x);
	cv::convertScaleAbs(grad_y, grad_y);
	cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edgeImage); //一种近似的估计
	return edgeImage;
}

pcaData getPcaFeature(cv::Mat imageData,int num_components, cv::Mat &SCORE) {
	// PCA算法保持5主成分分量
	//int num_components = 5;
	//执行pca算法
	cv::PCA pca(imageData, cv::Mat(), CV_PCA_DATA_AS_ROW, num_components);
	//copy  pca算法结果
	pcaData data;
	data.mean = pca.mean.clone();;//样本均值
	data.eigenvalues = pca.eigenvalues.clone();;//特征值  
	data.eigenvectors = pca.eigenvectors.clone();;//特征向量，每行一个特征向量 
	cv::Mat imageData2,V,tempMean;
	cv::repeat(pca.mean, imageData.rows, 1, tempMean);
	subtract(imageData, tempMean, imageData2);//去中心化
    transpose(pca.eigenvectors, V);//特征向量矩阵转置
	SCORE = imageData2*V;//主成份
	//cv::gemm(data.eigenvectors, dest, 1, cv::Mat(), 0, SCORE,(CV_PCA_DATA_AS_COL) ? CV_GEMM_B_T : 0);
	return data;
}

pcaData loadPcaPara(string path) {
	pcaData data;
	cv::Mat mean, eigenvectors;
	cv::FileStorage fs(path, cv::FileStorage::READ);
	if (fs.isOpened()) {
		fs["mean"] >> data.mean;
		fs["eigenvectors"] >> data.eigenvectors;
		fs["eigenvalues"] >> data.eigenvalues;
		fs.release();
	}
	return data;
}

void savePcaPara(string path, pcaData data) {
	cv::FileStorage fs(path, cv::FileStorage::WRITE);
	fs << "mean" << data.mean;
	fs << "eigenvalues" << data.eigenvalues;
	fs << "eigenvectors" << data.eigenvectors;
	// 6.close the file opened  
	fs.release();
}


cv::Mat  projectionPCA(cv::Mat inputData, pcaData pca_data ) {
	cv::Mat test, V;
	subtract(inputData, pca_data.mean, test);//去中心化
	transpose(pca_data.eigenvectors, V);//特征向量矩阵转置
	cv::Mat SCORE= test*V;//主成份
	return SCORE;
}
void cv::PCA::project(InputArray _data, OutputArray result) const
{
	Mat data = _data.getMat();//输入数据   
	CV_Assert(mean.data && eigenvectors.data &&
		((mean.rows == 1 && mean.cols == data.cols) || (mean.cols == 1 && mean.rows == data.rows)));
	Mat tmp_data, tmp_mean = repeat(mean, data.rows / mean.rows, data.cols / mean.cols);//期望repeat   
	int ctype = mean.type();
	if (data.type() != ctype || tmp_mean.data == mean.data)
	{
		data.convertTo(tmp_data, ctype);
		subtract(tmp_data, tmp_mean, tmp_data);//样本中心化，tmp_data是输出   
	}
	else
	{
		subtract(data, tmp_mean, tmp_mean);//样本中心化，tmp_mean是输出   
		tmp_data = tmp_mean;
	}
	//函数gemm，实现矩阵相乘，这里result = eigenvectors'*tmp_data  
	if (mean.rows == 1)
		gemm(tmp_data, eigenvectors, 1, Mat(), 0, result, GEMM_2_T);
	else
		gemm(eigenvectors, tmp_data, 1, Mat(), 0, result, 0);

}

//void getLBPFeature(cv::Mat image) {
//	cv::cvtColor(image, image, CV_BGR2GRAY);
//	cv::resize(image, image, cv::Size(50, 50), 0.0, 0.0);
//	cv::xobjdetect::CvFeatureParams fp;
//	fp.LBP;
//	cv::xobjdetect::CvLBPEvaluator lbp;
//	cv::xobjdetect::CvFeatureParams _featureParams;
//	//_featureParams.init(fp);
//	_featureParams.LBP;
//	_featureParams.maxCatCount = 256;
//	_featureParams.name = LBPF_NAME;
//	int _maxSampleCount =256;
//	cv::Size _winSize(16,16);
//	lbp.init(&_featureParams, _maxSampleCount, _winSize);
//	std::vector<int> feature_ind;
//	lbp.setImage(image, 1,1, feature_ind);
//	lbp.getNumFeatures();
//	float res0 = lbp(0);
//	float res1=lbp(1);
//	float res2 = lbp(2);
//	float res3 = lbp(3);
//
//	cout << "res" << res0 << endl;
//}

std::vector<float> getHOGFeature(cv::Mat image) {
	//检测窗口(64,32),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9
	cv::HOGDescriptor hog(cv::Size(32, 32), cv::Size(32, 32), cv::Size(16, 16), cv::Size(16, 16), 9);
	std::vector<float> descriptors;//HOG描述子向量
	hog.compute(image, descriptors, cv::Size(8, 8));//计算HOG描述子，检测窗口移动步长(8,8)
	return descriptors;
}


std::vector<float> getFeature(cv::Mat image,int featureType) {
	std::vector<float> feaVec;
	if (featureType == 0) {
		feaVec = getHOGFeature(image);
		//feaMat = convertVector2Mat<float>(descriptors,1, 1);
	}
	else if(featureType==1)
	{

	}
	return feaVec;
}


cv::Mat array2DToMat(int *data, int rows, int cols) {
	//二维数组本质是一维数组
	cv::Mat dataMat(rows, cols, CV_8UC1);
	for (size_t i = 0;i < rows;i++)
	{
		uchar *ptr = dataMat.ptr(i);
		for (size_t j = 0; j < cols; j++)
		{
			ptr[j] = data[i*cols + j];
		}
	}
	return dataMat;
}

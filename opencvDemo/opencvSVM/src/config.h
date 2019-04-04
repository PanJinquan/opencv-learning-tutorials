#ifndef CONFIG_H
#define CONFIG_H
#include <opencv2/opencv.hpp>
using namespace std;
//******************************************设置训练数据******************************************/
static string  project_root   = "E:/git/opencv-learning-tutorials";      //项目根目录
static string  train_filename = project_root + "/data/train_labels.txt"; //训练标签文件
static string  image_dir      = project_root + "/data/trainData";        //训练图像目录
static string  test_image     = project_root + "/data/trainData/A1.jpg"; //测试图片
static string  modelPath      = project_root + "/opencvDemo/opencvSVM/model";//模型保存目录
static string  saveSVMPath    = modelPath + "/SVM_HOG.xml";//保存SVM训练好的模型的路径
static string  loadSVMPath	  = modelPath + "/SVM_HOG.xml"; //载入SVM训练好的模型

//******************************************设置训练数据******************************************/
#define toGray                   true//是否将图像转为灰度图
#define IM_RESIZE(src)           cv::resize(src, src, cv::Size(64, 64))
#define doPCA                    true// 是否进行PCA降维处理
#define dimNum                   95//降维到100；
static string pcaPath		  =  modelPath + "/pca.xml";

#define predictTH                0.0
#endif 
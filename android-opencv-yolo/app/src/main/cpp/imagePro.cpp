//
// Created by panjq1 on 2017/10/22.
//
#include <string>
#include <android/log.h>
#include "opencv2/opencv.hpp"
#include "AndroidDebug.h"
#include "object_detection_yolov3.h"
#include "imagePro.h"
#include "com_panjq_opencv_alg_ImagePro.h"
#define LOG_TAG    "---JNILOG---" // 这个是自定义的LOG的标识
using namespace cv;
using namespace std;


extern "C"
JNIEXPORT void JNICALL Java_com_panjq_opencv_alg_ImagePro_jniImagePro3
        (JNIEnv *, jobject, jlong matAddrSrcImage, jlong matAddrDestImage){
    DEBUG__TIME0;
    Mat& srcImage  = *(Mat*)matAddrSrcImage;
    Mat& destImage = *(Mat*)matAddrDestImage;
    //blur(imgData,imgData,Size(20,20));//图像模糊
    cv::cvtColor(srcImage,srcImage,CV_BGRA2BGR);

    string pro_dir = "/storage/emulated/0/yolov3/"; //项目根目录
    string model_path=pro_dir+"yolov3.weights";
    string modelConfiguration = pro_dir + "yolov3.cfg";
    string modelWeights = pro_dir + "yolov3.weights";
    string classesFile = pro_dir + "coco.names";// "coco.names";
    destImage=detect_image(srcImage, modelWeights, modelConfiguration, classesFile);
    cv::cvtColor(destImage,destImage,CV_BGR2BGRA);
    DEBUG__TIME1;
    LOGE("Run time:jniImagePro3=%dms\n",(TIME1-TIME0)/1000);
    LOGE("jniImagePro3: ouput image size=[%d,%d],channels=%d\n",destImage.rows,destImage.cols,destImage.channels());
}
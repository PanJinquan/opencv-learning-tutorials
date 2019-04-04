#pragma once
#include <fstream>
#include <string>
#include "vector"
#include <iostream>
using namespace std;

//txt文本的路径
#define txtRows			9//txt文本行数
#define txtCols			2//txt文本列数
struct FileData
{
	vector<string> name;//第1列
	int data[txtRows][txtCols - 1];//保存txt文本的数据（第2列开始）
};

FileData loadFileData(char* path);

#include"LBP.h"

//获取i中0,1的跳变次数
int LBP::GetHopCount(int i)
{
	// 转换为二进制
	int a[8] = { 0 };
	int k = 7;
	while (i)
	{
		// 除2取余
		a[k] = i % 2;
		i /= 2;
		--k;
	}

	// 计算跳变次数
	int count = 0;
	for (int k = 0; k<8; ++k)
	{
		// 注意，是循环二进制,所以需要判断是否为8
		if (a[k] != a[k + 1 == 8 ? 0 : k + 1])
		{
			++count;
		}
	}
	return count;

}

// 建立等价模式表
// 这里为了便于建立LBP特征图，58种等价模式序号从1开始:1~58,第59类混合模式映射为0
void LBP::BuildUniformPatternTable(int *table)
{
	memset(table, 0, 256 * sizeof(int));
	uchar temp = 1;
	for (int i = 0; i<256; ++i)
	{
		if (GetHopCount(i) <= 2)
		{
			table[i] = temp;
			temp++;
		}
	}

}

void LBP::ComputeLBPFeatureVector_256(const Mat &srcImage, Size cellSize, Mat &featureVector)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);

	Mat LBPImage;
	ComputeLBPImage_256(srcImage, LBPImage);

	// 计算cell个数
	int widthOfCell = cellSize.width;
	int heightOfCell = cellSize.height;
	int numberOfCell_X = srcImage.cols / widthOfCell;// X方向cell的个数
	int numberOfCell_Y = srcImage.rows / heightOfCell;

	// 特征向量的个数
	int numberOfDimension = 256 * numberOfCell_X*numberOfCell_Y;
	featureVector.create(1, numberOfDimension, CV_32FC1);
	featureVector.setTo(Scalar(0));

	// 计算LBP特征向量
	int stepOfCell = srcImage.cols;
	int pixelCount = cellSize.width*cellSize.height;
	float *dataOfFeatureVector = (float *)featureVector.data;

	// cell的特征向量在最终特征向量中的起始位置
	int index = -256;
	for (int y = 0; y <= numberOfCell_Y - 1; ++y)
	{
		for (int x = 0; x <= numberOfCell_X - 1; ++x)
		{
			index += 256;

			// 计算每个cell的LBP直方图
			Mat cell = LBPImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));
			uchar *rowOfCell = cell.data;
			for (int y_Cell = 0; y_Cell <= cell.rows - 1; ++y_Cell, rowOfCell += stepOfCell)
			{
				uchar *colOfCell = rowOfCell;
				for (int x_Cell = 0; x_Cell <= cell.cols - 1; ++x_Cell, ++colOfCell)
				{
					++dataOfFeatureVector[index + colOfCell[0]];
				}
			}

			// 一定要归一化！否则分类器计算误差很大
			for (int i = 0; i <= 255; ++i)
				dataOfFeatureVector[index + i] /= pixelCount;

		}
	}

}

//srcImage:灰度图
//LBPImage:LBP图
void LBP::ComputeLBPImage_256(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
	LBPImage.create(srcImage.size(), srcImage.type());

	// 扩充原图像边界，便于边界处理
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 计算LBP特征图
	int heightOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	int widthOfLBP = LBPImage.cols;
	uchar *rowOfExtendedImage = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowOfLBPImage = LBPImage.data;
	for (int y = 1; y <= heightOfExtendedImage - 2; ++y, rowOfExtendedImage += widthOfExtendedImage, rowOfLBPImage += widthOfLBP)
	{
		// 列
		uchar *colOfExtendedImage = rowOfExtendedImage;
		uchar *colOfLBPImage = rowOfLBPImage;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x, ++colOfExtendedImage, ++colOfLBPImage)
		{
			// 计算LBP值
			int LBPValue = 0;
			if (colOfExtendedImage[0 - widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 128;
			if (colOfExtendedImage[0 - widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 64;
			if (colOfExtendedImage[0 - widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 32;
			if (colOfExtendedImage[0 + 1] >= colOfExtendedImage[0])
				LBPValue += 16;
			if (colOfExtendedImage[0 + widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 8;
			if (colOfExtendedImage[0 + widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 4;
			if (colOfExtendedImage[0 + widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 2;
			if (colOfExtendedImage[0 - 1] >= colOfExtendedImage[0])
				LBPValue += 1;

			colOfLBPImage[0] = LBPValue;

		}  // x

	}// y


}

// cellSize:每个cell的大小,如16*16
void LBP::ComputeLBPFeatureVector_Uniform(const Mat &srcImage, Size cellSize, Mat &featureVector)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);

	Mat LBPImage;
	ComputeLBPImage_Uniform(srcImage, LBPImage);

	// 计算cell个数
	int widthOfCell = cellSize.width;
	int heightOfCell = cellSize.height;
	int numberOfCell_X = srcImage.cols / widthOfCell;// X方向cell的个数
	int numberOfCell_Y = srcImage.rows / heightOfCell;

	// 特征向量的个数
	int numberOfDimension = 58 * numberOfCell_X*numberOfCell_Y;
	featureVector.create(1, numberOfDimension, CV_32FC1);
	featureVector.setTo(Scalar(0));

	// 计算LBP特征向量
	int stepOfCell = srcImage.cols;
	int index = -58;// cell的特征向量在最终特征向量中的起始位置
	float *dataOfFeatureVector = (float *)featureVector.data;
	for (int y = 0; y <= numberOfCell_Y - 1; ++y)
	{
		for (int x = 0; x <= numberOfCell_X - 1; ++x)
		{
			index += 58;

			// 计算每个cell的LBP直方图
			Mat cell = LBPImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));
			uchar *rowOfCell = cell.data;
			int sum = 0; // 每个cell的等价模式总数
			for (int y_Cell = 0; y_Cell <= cell.rows - 1; ++y_Cell, rowOfCell += stepOfCell)
			{
				uchar *colOfCell = rowOfCell;
				for (int x_Cell = 0; x_Cell <= cell.cols - 1; ++x_Cell, ++colOfCell)
				{
					if (colOfCell[0] != 0)
					{
						// 在直方图中转化为0~57，所以是colOfCell[0] - 1
						++dataOfFeatureVector[index + colOfCell[0] - 1];
						++sum;
					}
				}
			}

			// 一定要归一化！否则分类器计算误差很大
			for (int i = 0; i <= 57; ++i)
				dataOfFeatureVector[index + i] /= sum;

		}
	}
}

// 计算等价模式LBP特征图，为了方便表示特征图，58种等价模式表示为1~58,第59种混合模式表示为0
// 注：你可以将第59类混合模式映射为任意数值，因为要突出等价模式特征，所以非等价模式设置为0比较好
void LBP::ComputeLBPImage_Uniform(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
	LBPImage.create(srcImage.size(), srcImage.type());

	// 计算LBP图
	// 扩充原图像边界，便于边界处理
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 构建LBP 等价模式查找表
	//int table[256];
	//BuildUniformPatternTable(table);

	// LUT(256种每一种模式对应的等价模式)
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

	// 计算LBP
	int heightOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	int widthOfLBP = LBPImage.cols;
	uchar *rowOfExtendedImage = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowOfLBPImage = LBPImage.data;
	for (int y = 1; y <= heightOfExtendedImage - 2; ++y, rowOfExtendedImage += widthOfExtendedImage, rowOfLBPImage += widthOfLBP)
	{
		// 列
		uchar *colOfExtendedImage = rowOfExtendedImage;
		uchar *colOfLBPImage = rowOfLBPImage;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x, ++colOfExtendedImage, ++colOfLBPImage)
		{
			// 计算LBP值
			int LBPValue = 0;
			if (colOfExtendedImage[0 - widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 128;
			if (colOfExtendedImage[0 - widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 64;
			if (colOfExtendedImage[0 - widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 32;
			if (colOfExtendedImage[0 + 1] >= colOfExtendedImage[0])
				LBPValue += 16;
			if (colOfExtendedImage[0 + widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 8;
			if (colOfExtendedImage[0 + widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 4;
			if (colOfExtendedImage[0 + widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 2;
			if (colOfExtendedImage[0 - 1] >= colOfExtendedImage[0])
				LBPValue += 1;

			colOfLBPImage[0] = table[LBPValue];

		} // x

	}// y
}

// 计算9种等价模式，等价模式编号也是从1开始：1~9
int LBP::ComputeValue9(int value58)
{
	int value9 = 0;
	switch (value58)
	{
	case 1:
		value9 = 1;
		break;
	case 2:
		value9 = 2;
		break;
	case 4:
		value9 = 3;
		break;
	case 7:
		value9 = 4;
		break;
	case 11:
		value9 = 5;
		break;
	case 16:
		value9 = 6;
		break;
	case 22:
		value9 = 7;
		break;
	case 29:
		value9 = 8;
		break;
	case 58:
		value9 = 9;
		break;
	}

	return value9;

}

int LBP::GetMinBinary(int binary)
{
	static const int miniBinaryLUT[256] = { 0, 1, 1, 3, 1, 5, 3, 7, 1, 9, 5, 11, 3, 13, 7, 15, 1, 17, 9, 19, 5,
		21, 11, 23, 3, 25, 13, 27, 7, 29, 15, 31, 1, 9, 17, 25, 9, 37, 19, 39, 5, 37, 21, 43, 11, 45,
		23, 47, 3, 19, 25, 51, 13, 53, 27, 55, 7, 39, 29, 59, 15, 61, 31, 63, 1, 5, 9, 13, 17, 21, 25,
		29, 9, 37, 37, 45, 19, 53, 39, 61, 5, 21, 37, 53, 21, 85, 43, 87, 11, 43, 45, 91, 23, 87, 47, 95,
		3, 11, 19, 27, 25, 43, 51, 59, 13, 45, 53, 91, 27, 91, 55, 111, 7, 23, 39, 55, 29, 87, 59, 119, 15,
		47, 61, 111, 31, 95, 63, 127, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 9, 25, 37,
		39, 37, 43, 45, 47, 19, 51, 53, 55, 39, 59, 61, 63, 5, 13, 21, 29, 37, 45, 53, 61, 21, 53, 85,
		87, 43, 91, 87, 95, 11, 27, 43, 59, 45, 91, 91, 111, 23, 55, 87, 119, 47, 111, 95, 127, 3,
		7, 11, 15, 19, 23, 27, 31, 25, 39, 43, 47, 51, 55, 59, 63, 13, 29, 45, 61, 53, 87, 91, 95, 27, 59,
		91, 111, 55, 119, 111, 127, 7, 15, 23, 31, 39, 47, 55, 63, 29, 61, 87, 95, 59, 111, 119, 127, 15, 31, 47, 63,
		61, 95, 111, 127, 31, 63, 95, 127, 63, 127, 127, 255 };

	return miniBinaryLUT[binary];
}

// 获取循环二进制的最小二进制模式
uchar LBP::GetMinBinary(uchar *binary)
{
	// 计算8个二进制
	uchar LBPValue[8] = { 0 };
	for (int i = 0; i <= 7; ++i)
	{
		LBPValue[0] += binary[i] << (7 - i);
		LBPValue[1] += binary[(i + 7) % 8] << (7 - i);
		LBPValue[2] += binary[(i + 6) % 8] << (7 - i);
		LBPValue[3] += binary[(i + 5) % 8] << (7 - i);
		LBPValue[4] += binary[(i + 4) % 8] << (7 - i);
		LBPValue[5] += binary[(i + 3) % 8] << (7 - i);
		LBPValue[6] += binary[(i + 2) % 8] << (7 - i);
		LBPValue[7] += binary[(i + 1) % 8] << (7 - i);
	}

	// 选择最小的
	uchar minValue = LBPValue[0];
	for (int i = 1; i <= 7; ++i)
	{
		if (LBPValue[i] < minValue)
		{
			minValue = LBPValue[i];
		}
	}

	return minValue;

}
// cellSize:每个cell的大小,如16*16
void LBP::ComputeLBPFeatureVector_Rotation_Uniform(const Mat &srcImage, Size cellSize, Mat &featureVector)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);

	Mat LBPImage;
	ComputeLBPImage_Rotation_Uniform(srcImage, LBPImage);

	// 计算cell个数
	int widthOfCell = cellSize.width;
	int heightOfCell = cellSize.height;
	int numberOfCell_X = srcImage.cols / widthOfCell;// X方向cell的个数
	int numberOfCell_Y = srcImage.rows / heightOfCell;

	// 特征向量的个数
	int numberOfDimension = 9 * numberOfCell_X*numberOfCell_Y;
	featureVector.create(1, numberOfDimension, CV_32FC1);
	featureVector.setTo(Scalar(0));

	// 计算LBP特征向量
	int stepOfCell = srcImage.cols;
	int index = -9;// cell的特征向量在最终特征向量中的起始位置
	float *dataOfFeatureVector = (float *)featureVector.data;
	for (int y = 0; y <= numberOfCell_Y - 1; ++y)
	{
		for (int x = 0; x <= numberOfCell_X - 1; ++x)
		{
			index += 9;

			// 计算每个cell的LBP直方图
			Mat cell = LBPImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));
			uchar *rowOfCell = cell.data;
			int sum = 0; // 每个cell的等价模式总数
			for (int y_Cell = 0; y_Cell <= cell.rows - 1; ++y_Cell, rowOfCell += stepOfCell)
			{
				uchar *colOfCell = rowOfCell;
				for (int x_Cell = 0; x_Cell <= cell.cols - 1; ++x_Cell, ++colOfCell)
				{
					if (colOfCell[0] != 0)
					{
						// 在直方图中转化为0~8，所以是colOfCell[0] - 1
						++dataOfFeatureVector[index + colOfCell[0] - 1];
						++sum;
					}
				}
			}

			// 直方图归一化
			for (int i = 0; i <= 8; ++i)
				dataOfFeatureVector[index + i] /= sum;

		}
	}
}

void LBP::ComputeLBPImage_Rotation_Uniform(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
	LBPImage.create(srcImage.size(), srcImage.type());

	// 扩充图像，处理边界情况
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 构建LBP 等价模式查找表
	//int table[256];
	//BuildUniformPatternTable(table);

	// 查找表
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

	int heigthOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	int widthOfLBPImage = LBPImage.cols;

	uchar *rowOfExtendedImage = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowOfLBPImage = LBPImage.data;
	for (int y = 1; y <= heigthOfExtendedImage - 2; ++y, rowOfExtendedImage += widthOfExtendedImage, rowOfLBPImage += widthOfLBPImage)
	{
		// 列
		uchar *colOfExtendedImage = rowOfExtendedImage;
		uchar *colOfLBPImage = rowOfLBPImage;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x, ++colOfExtendedImage, ++colOfLBPImage)
		{
			// 计算LBP值
			int LBPValue = 0;
			if (colOfExtendedImage[0 - widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 128;
			if (colOfExtendedImage[0 - widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 64;
			if (colOfExtendedImage[0 - widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 32;
			if (colOfExtendedImage[0 + 1] >= colOfExtendedImage[0])
				LBPValue += 16;
			if (colOfExtendedImage[0 + widthOfExtendedImage + 1] >= colOfExtendedImage[0])
				LBPValue += 8;
			if (colOfExtendedImage[0 + widthOfExtendedImage] >= colOfExtendedImage[0])
				LBPValue += 4;
			if (colOfExtendedImage[0 + widthOfExtendedImage - 1] >= colOfExtendedImage[0])
				LBPValue += 2;
			if (colOfExtendedImage[0 - 1] >= colOfExtendedImage[0])
				LBPValue += 1;

			int minValue = GetMinBinary(LBPValue);

			// 计算58种等价模式LBP
			int value58 = table[minValue];

			// 计算9种等价模式
			colOfLBPImage[0] = ComputeValue9(value58);
		}

	}

}

void LBP::ComputeLBPImage_Rotation_Uniform_2(const Mat &srcImage, Mat &LBPImage)
{
	// 参数检查，内存分配
	CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
	LBPImage.create(srcImage.size(), srcImage.type());

	// 扩充图像，处理边界情况
	Mat extendedImage;
	copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

	// 构建LBP 等价模式查找表
	//int table[256];
	//BuildUniformPatternTable(table);

	// 通过查找表
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

	uchar binary[8] = { 0 };// 记录每个像素的LBP值
	int heigthOfExtendedImage = extendedImage.rows;
	int widthOfExtendedImage = extendedImage.cols;
	int widthOfLBPImage = LBPImage.cols;

	uchar *rowOfExtendedImage = extendedImage.data + widthOfExtendedImage + 1;
	uchar *rowOfLBPImage = LBPImage.data;
	for (int y = 1; y <= heigthOfExtendedImage - 2; ++y, rowOfExtendedImage += widthOfExtendedImage, rowOfLBPImage += widthOfLBPImage)
	{
		// 列
		uchar *colOfExtendedImage = rowOfExtendedImage;
		uchar *colOfLBPImage = rowOfLBPImage;
		for (int x = 1; x <= widthOfExtendedImage - 2; ++x, ++colOfExtendedImage, ++colOfLBPImage)
		{
			// 计算旋转不变LBP(最小的二进制模式)
			binary[0] = colOfExtendedImage[0 - widthOfExtendedImage - 1] >= colOfExtendedImage[0] ? 1 : 0;
			binary[1] = colOfExtendedImage[0 - widthOfExtendedImage] >= colOfExtendedImage[0] ? 1 : 0;
			binary[2] = colOfExtendedImage[0 - widthOfExtendedImage + 1] >= colOfExtendedImage[0] ? 1 : 0;
			binary[3] = colOfExtendedImage[0 + 1] >= colOfExtendedImage[0] ? 1 : 0;
			binary[4] = colOfExtendedImage[0 + widthOfExtendedImage + 1] >= colOfExtendedImage[0] ? 1 : 0;
			binary[5] = colOfExtendedImage[0 + widthOfExtendedImage] >= colOfExtendedImage[0] ? 1 : 0;
			binary[6] = colOfExtendedImage[0 + widthOfExtendedImage - 1] >= colOfExtendedImage[0] ? 1 : 0;
			binary[7] = colOfExtendedImage[0 - 1] >= colOfExtendedImage[0] ? 1 : 0;

			int minValue = GetMinBinary(binary);

			// 计算58种等价模式LBP
			int value58 = table[minValue];

			// 计算9种等价模式
			colOfLBPImage[0] = ComputeValue9(value58);
		}

	}
}


// 验证灰度不变+旋转不变+等价模式种类
void LBP::Test()
{
	uchar LBPValue[8] = { 0 };
	int k = 7, j;
	int temp;
	LBP lbp;
	int number[256] = { 0 };
	int numberOfMinBinary = 0;

	// 旋转不变
	for (int i = 0; i < 256; ++i)
	{
		k = 7;
		temp = i;
		while (k >= 0)
		{
			LBPValue[k] = temp & 1;
			temp = temp >> 1;
			--k;
		}
		int minBinary = lbp.GetMinBinary(LBPValue);

		// 查找有无重复的
		for (j = 0; j <= numberOfMinBinary - 1; ++j)
		{
			if (number[j] == minBinary)
				break;
		}
		if (j == numberOfMinBinary)
		{
			number[numberOfMinBinary++] = minBinary;
		}
	}
	cout << "旋转不变一共有：" << numberOfMinBinary << "种" << endl;

	// LUT
	static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
		0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
		, 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

	for (int i = 0; i <= numberOfMinBinary - 1; ++i)
	{
		cout << "旋转不变的LBP：" << number[i] << " " << "对应的等价模式：" << table[number[i]] << endl;
	}

}

void LBP::TestGetMinBinaryLUT()
{
	for (int i = 0; i <= 255; ++i)
	{
		uchar a[8] = { 0 };
		int k = 7;
		int j = i;
		while (j)
		{
			// 除2取余
			a[k] = j % 2;
			j /= 2;
			--k;
		}
		uchar minBinary = GetMinBinary(a);
		printf("%d,", minBinary);

	}
}
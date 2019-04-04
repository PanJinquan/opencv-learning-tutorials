#include "file_processing.h"

FileData loadFileData(char* path) {
	FileData m_fileData;
	char name[81];
	std::vector<string> v_name;
	int txtData[txtRows][txtCols - 1];
	int i, j;
	FILE* fp = fopen(path, "r"); //打开文件    
	if (fp == NULL)
	{
		printf("文件读取错误...");
		return m_fileData;
	}
	for (i = 0; i < txtRows; i++)
	{
		for (j = 0; j < txtCols; j++)
		{
			if (j == 0)
			{
				fscanf(fp, "%s", name);
				v_name.push_back(name);
			}
			else {
				fscanf(fp, "%d", &txtData[i][j - 1]);/*每次读取一个数，fscanf函数遇到空格或者换行结束*/
			}
		}
		fscanf(fp, "\n");
	}
	fclose(fp);
	memcpy(m_fileData.data, txtData, sizeof(m_fileData.data));
	m_fileData.name = v_name;
	return m_fileData;
}



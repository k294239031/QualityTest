#include <stdlib.h>
#include <iostream>
#include <vector>
#include<io.h>
#include "QualityTest.h"
using namespace std;

#include <fstream>
#include <string>
#include <sstream>


void GetAllFiles(string path, vector<string>& files, string file_type)
{
	// 文件句柄
	intptr_t handle_file = 0;
	// 文件信息
	struct _finddata_t file_info;
	string p;
	if ((handle_file = _findfirst(p.assign(path).append("/*" + file_type).c_str(), &file_info)) != -1) {
		do {
			// 保存文件的全路径
			files.push_back(p.assign(path).append("/").append(file_info.name));

		} while (_findnext(handle_file, &file_info) == 0);  //寻找下一个，成功返回0，否则-1

		_findclose(handle_file);
	}
}

int main()
{
	vector<string> file_name;
	string path="C:/Users/weikang1/Desktop/异常图收集/RGB_错位";
	GetAllFiles(path, file_name, ".jpg");
	cv::Mat img;
	QualityTest a;
	int b;
	string str;
	clock_t start, end;
	for (int i = 0; i < file_name.size(); i++)
	{
		img = cv::imread(file_name[i]);
		//cv::imshow("IMG", img);
		start = clock();
		if (i == 131)
			cv::waitKey(1000);
		b = a.main(img);
		if (b)
		{
			str = to_string(i);
			cv::imshow(str, img);
		}
		cout << "第" << i+1 << "张图片,bool=" << b << endl;
		
		end = clock();

		cout << (double)(end - start) / CLOCKS_PER_SEC << endl;;

		//cv::waitKey(200);
	}
	cv::waitKey(0);
	return 0;
}

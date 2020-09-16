#include <stdlib.h>
#include <iostream>
#include <vector>
#include<io.h>
#include<conio.h>
#include <direct.h>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>
#include "QualityTest.h"
using namespace std;

bool GetAllFiles(string path, vector<string>& files, string file_type);
void getMoveFilesName(int& moveFilesType);
bool MoveFiles(int moveFilesType, vector<string> initial_path, vector<string> target_path);
string createFloder(const char* path);
int copyFile(std::string path1, std::string path2);
int moveFile(std::string path1, std::string path2);

string path;
string file_type, isJPEG = ".jpg";
vector<string> file_path, file_name, errfile_name, errfile_path, save_filename;
cv::Mat img;
QualityTest a;
int getImageDetectInfo = 0, moveFilesType = 0;
string str;
char isLightLoop;
string temp_path;
int main()
{
	char loop='y';
	while (loop=='y'|| loop == 'Y')
	{
		std::cout << "输入图片所在文件夹路径（如C:/Users/Desktop）：" << std::endl;
		//path = "C:/Users/weikang1/Desktop/异常图收集/depth_黑";
		std::cin >> path;
		std::cout << "输入图片格式（如.jpg）：" << std::endl;
		//file_type = ".jpg";
		std::cin >> file_type;
		if (!GetAllFiles(path, file_path, file_type))
		{
			return 0;
		}
		std::cout << "是否用于检测上下电的情况（即亮暗变化明显）：" << std::endl;
		std::cout << "1		——是" << std::endl;
		std::cout << "0		——否" << std::endl;
		std::cin >> isLightLoop;
		if (isLightLoop == '1') a.SetUserParameter(0.65, 0.7, 4, 3, 0.7, 1);

		for (int i = 0; i < file_path.size(); i++)
		{
			cout << "第" << i + 1 << "张图片	" ;
			if (file_type == isJPEG)
			{
				getImageDetectInfo = a.IsJPEGDamage(file_path[i].c_str());
			}

			if (getImageDetectInfo)
			{
				errfile_name.push_back(file_path[i]);
				cout << "bool=" << getImageDetectInfo << endl;
			}
			else
			{
				img = cv::imread(file_path[i]);
				getImageDetectInfo = a.ImageDetect(img);
				if (getImageDetectInfo)
				{
					errfile_name.push_back(file_path[i]);
					
					//str = to_string(i + 1);
					//cv::imshow(str, img);
				}
				cout << "bool=" << getImageDetectInfo << endl;
			}
		}
		cout << "错误图片如下：" << endl;
		for (int i=0;i< errfile_name.size();++i)
		{
			cout << endl << errfile_name[i] << endl;
		}
		cout << "共检测出" << errfile_name.size() << "张错误图片" << endl << endl;
		
		getMoveFilesName(moveFilesType);
		MoveFiles(moveFilesType, errfile_name, errfile_path);

		cout << "是否继续检测?" << endl;
		std::cout << "y		——继续" << std::endl;
		std::cout << "n		——退出" << std::endl;
		cin >> loop;
		//清空缓存
		vector<string>().swap(file_name);
	}
	return 0;
}

bool GetAllFiles(string path, vector<string>& files, string file_type)
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
		std::cout << endl
			<< "图片获取成功，共获取到"
			<< file_type
			<< "格式，图片共"
			<< files.size()
			<< "张"
			<< std::endl;
		std::cout << "输入回车继续..." << endl << endl;
		_getch();
		return true;
	}
	else
	{
		std::cout << endl << "图片获取失败" << endl;
		std::cout << "输入回车退出..." << endl;
		_getch();
		return false;
	}
}

string createFloder(const char* path)
{	
	stringstream target_path;
	target_path << path << "/ErrorImage";
	if (0 != _access(target_path.str().c_str(), 0))
	{	// if this folder not exist, create a new one.
		if (0 != _mkdir(target_path.str().c_str()))   // 返回 0 表示创建成功，-1 表示失败
			cout << endl << target_path.str() << "文件夹创建失败" << endl;
	}
	return target_path.str();
}

void getMoveFilesName(int &moveFilesType)
{
	std::cout << "是否复制或剪切检测到的错图至其他文件夹？" << std::endl;
	std::cout << "0		——取消" << std::endl;
	std::cout << "1		——复制" << std::endl;
	std::cout << "2		——剪切" << std::endl;
	char num;
	cin >> num;
	switch (num)
	{
	case '0':
		moveFilesType = 0;
		cout << endl << "Unmove image files" << endl;
		break;
	case '1':
		moveFilesType = 1;
		cout << endl << "Copy image files..." << endl;
		break;
	case '2':
		moveFilesType = 2;
		cout << endl << "Cut and paste image files..." << endl;
		break;
	default:
		moveFilesType = 0;
		cout << endl << "Error num ,and unmove image files" << endl;
		break;
	}
	
	std::cout << "输入图片移动的目标文件夹路径（如C:/Users/Desktop/错图）：" << std::endl;
	cin >> temp_path;
	save_filename = errfile_name;
	for (int i = 0; i < save_filename.capacity(); ++i)
	{
		save_filename[i].erase(0, path.size()+1);
		errfile_path.push_back(temp_path);
		errfile_path[i].append("/").append(save_filename[i]);
	}
}
bool MoveFiles(int moveFilesType, vector<string> initial_path, vector<string> target_path)
{
	int success_num=0, failure_num=0;
	//createFloder(temp_path.c_str());
	switch (moveFilesType)
	{
	case 1:
		for (int i=0;i<initial_path.size();++i)
		{
			if (!copyFile(initial_path[i].c_str(), target_path[i].c_str()))//成功返回0
			{
				success_num++;
			}
			else
			{
				failure_num++;
			}
		}
		break;
	case 2:
		for (int i = 0; i < initial_path.size(); ++i)
		{
			if (!moveFile(initial_path[i].c_str(), target_path[i].c_str()))
			{
				success_num++;
			}
			else
			{
				failure_num++;
			}
		}
		break;
	default:
		return false;
	}
	cout << endl 
		<< "共有"
		<< initial_path.size() 
		<<"张图片需要保存，成功"
		<< success_num 
		<<"张，失败"
		<< failure_num
		<<"张" << endl;
	return true;
}

//拷贝文件，成功返回0，失败返回错误码
int copyFile(std::string path1, std::string path2)
{	//false 覆盖； 成功返回非0，失败返回0
	if (!CopyFileA(path1.c_str(), path2.c_str(), false))
	{
		printf_s("拷贝失败");
		return GetLastError();
	}
	//printf_s("拷贝成功\n");
	return 0;
}

//剪切文件，成功返回0，失败返回错误码
int moveFile(std::string path1, std::string path2)
{
	if (!MoveFileA(path1.c_str(), path2.c_str()))
	{
		printf_s("剪切失败");
		return GetLastError();
	}
	//printf_s("剪切成功\n");
	return 0;
}

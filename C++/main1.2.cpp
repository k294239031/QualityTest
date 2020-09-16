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
string CreateFloder(const char* path);
int copyFile(std::string path1, std::string path2);
int moveFile(std::string path1, std::string path2);

struct
{
	string source;
	vector<string> files, errfiles, errfiles_name, errfiles2;

}Path;
string file_type, isJPEG = "jpg";
cv::Mat img;
QualityTest demo;
int getImageDetectInfo = 0;

int main()
{
	char loop = 'y';
	while (loop == 'y' || loop == 'Y')
	{
		//1.获取所有图片文件路径
		//
		std::cout << "输入图片所在文件夹路径（如C:/Users/Desktop）：" << std::endl;
		//path = "C:/Users/weikang1/Desktop/异常图收集/depth_黑";
		std::cin >> Path.source;
		if (file_type.size() == 0)
		{
			std::cout << "输入图片格式（如jpg）：" << std::endl;
			//file_type = ".jpg";
			std::cin >> file_type;
		}
		if (!GetAllFiles(Path.source, Path.files, file_type))
		{
			return 0;
		}


		//2.对图片进行检测
		//
		for (int i = 0; i < Path.files.size(); ++i)
		{
			if (file_type == isJPEG)
			{
				getImageDetectInfo = demo.IsJPEGDamage(Path.files[i].c_str());
				if (getImageDetectInfo)
				{
					Path.errfiles.push_back(Path.files[i]);
					cout << "第" << i + 1 << "张图片," << "bool=" << getImageDetectInfo << endl;
					continue;
				}
				img = cv::imread(Path.files[i]);
				getImageDetectInfo = demo.ImageDetect(img);
			}
			else
			{
				img = cv::imread(Path.files[i]);
				getImageDetectInfo = demo.ImageDetect(img);
			}
			
			if (getImageDetectInfo)
			{
				Path.errfiles.push_back(Path.files[i]);
				cout << "第" << i + 1 << "张图片," << "bool=" << getImageDetectInfo << endl;
			}
		}
		cout << "共检测出" << Path.errfiles.size() << "张错误图片" << endl << endl;

		//3.复制错误图片至指定文件夹
		CreateFloder(Path.source.c_str());
		Path.errfiles_name = Path.errfiles;
		int success_num = 0;
		for (int i = 0; i < Path.errfiles.size(); ++i)
		{
			Path.errfiles_name[i].erase(0, Path.source.size() + 1);
			Path.errfiles2.push_back(Path.source);
			Path.errfiles2[i].append("/ErrorImage/").append(Path.errfiles_name[i]);
			if (!copyFile(Path.errfiles[i].c_str(), Path.errfiles2[i].c_str()))//成功返回0
			{
				++success_num;
			}
		}
		cout << endl
			<< "共有"
			<< Path.errfiles.size()
			<< "张图片需要保存，成功"
			<< success_num
			<< "张，失败"
			<< (Path.errfiles.size()-success_num)
			<< "张" << endl;


		//4.清空缓存
		//
		vector<string>().swap(Path.errfiles);
		vector<string>().swap(Path.errfiles2);
		vector<string>().swap(Path.errfiles_name);
		vector<string>().swap(Path.files);

		std::cout << "是否继续检测?" << "（默认检测" << file_type << "格式图片)" << std::endl;
		std::cout << "y		——继续" << std::endl;
		std::cout << "n		——退出" << std::endl;
		cin >> loop;
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
	if ((handle_file = _findfirst(p.assign(path).append("/*." + file_type).c_str(), &file_info)) != -1) {
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
			<< std::endl
			<< std::endl
			<< std::endl;
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

string CreateFloder(const char* path)
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


bool MoveFiles(int moveFilesType, vector<string> initial_path, vector<string> target_path)
{
	int success_num = 0, failure_num = 0;
	//createFloder(temp_path.c_str());
	switch (moveFilesType)
	{
	case 1:
		for (int i = 0; i < initial_path.size(); ++i)
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
		<< "张图片需要保存，成功"
		<< success_num
		<< "张，失败"
		<< failure_num
		<< "张" << endl;
	return true;
}

//拷贝文件，成功返回0，失败返回错误码
int copyFile(std::string path1, std::string path2)
{	//false 覆盖； 成功返回非0，失败返回0
	if (!CopyFileA(path1.c_str(), path2.c_str(), false))
	{
		//printf_s("拷贝失败");
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

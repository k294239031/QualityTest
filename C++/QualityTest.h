#ifndef QUALITYTEST_H
#define QUALITYTEST_H

#include <vector>
#include <opencv2/opencv.hpp>
class QualityTest
{
public:
	cv::Mat img;
	int point[5][4] = { 0 },i;
	float point_R[5] = { 0 };
	struct UserParameter
	{
		//全局阈值不应小于0.6
		double globalThreshold = 0.65;	//全局检测阈值，超过视为错图
		//局部阈值不应小于0.7
		double localThreshold = 0.9;		//局部检测阈值，超过视为错图
		//设置n张图为正确参考图存入imgBuffer
		int numEvaluat = 4;				//imgBuffer缓存图片数量
		//设置判定连续n张图错误时，认为场景变化（该值需小于numEvaluat）
		int numSceneChange = 3;			//用来判定环境改变（如开、关灯）的数量
		//设置局部探测窗口的大小，范围为0-1
		double detetWindowSize = 0.7;	//局部探测窗口的尺寸
		//0，表示不更新normalImgInfo；n，表示每隔n张图更新normalImgInfo（该值需大于numEvaluat）
		int fixedTimeRefresh = 20;	//每检测n张图，通过imgBuffer强制更新normalImgmgInfo
	}UserPara;	//需自定义设置的参数
	struct NormalImgInfo
	{
		float RGB[3] = {0,0,0};
		float point_R[5] = { 0,0,0,0,0 };
		int normalImgNum = 0;
		int totalImgNum = 0;

	}NormalInf;

	struct ImgBuffer
	{
		std::vector <cv::Mat> imgBuffer;
		std::vector <cv::Mat>::iterator bufferCount;
		int bufferTrigger = 1;

	}Buf;
	struct ErrorInfo
	{
		std::vector <cv::Mat> imgBuffer;
		std::vector <cv::Mat>::iterator bufferCount;
		int isError = 0;
		int totalNum = 0;
		int continuesNum = 0;

	}Err;

	QualityTest();
	~QualityTest();

	void setUserParameter(double& global, double& local, int& numE, int& numS, double& detectW);
	int main(cv::Mat);
	void getImgBuffer();

	void getErrBuffer();
	void returnErrBuffer();
	void restartErrBuffer();

	void getNormalImageInfo();
	
	bool isScenceChange();
	bool isGlobalError();
	bool isLocalError();
	

};

#endif // !QUALITYTEST_H#pragma once

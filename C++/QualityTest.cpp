#include <iostream>
#include "QualityTest.h"
#include <opencv2/opencv.hpp>
#include <iterator>

QualityTest::QualityTest()
{
	Err.imgBuffer.clear();
	for (int num = 0; num < UserPara.numSceneChange; ++num) {
		Err.imgBuffer.push_back(cv::Mat::zeros(1, 1, CV_8UC1));
	}
	Err.bufferCount = Err.imgBuffer.begin();
}
QualityTest::~QualityTest()
{

}

void QualityTest::setUserParameter(double& global, double& local, int& numE, int& numS, double& detectW, bool& isLightLoop)
{
	UserPara.globalThreshold = global;		//全局检测阈值
	if ((UserPara.globalThreshold > 0) && (UserPara.globalThreshold < 1))
	{
		UserPara.localThreshold = local;		//局部检测阈值
		if ((UserPara.localThreshold > 0) && (UserPara.localThreshold < 1))
		{
			UserPara.numEvaluat = numE;				//imgBuffer缓存图片数量
			if (UserPara.numEvaluat > 1)
			{
				UserPara.numSceneChange = numS;			//用来判定环境改变（如开、关灯）的数量
				if ((UserPara.numSceneChange > 1) && (UserPara.localThreshold < UserPara.numEvaluat))
				{
					UserPara.detetWindowSize = detectW;
					UserPara.isLightLoop = isLightLoop;
					if ((UserPara.detetWindowSize <= 0) || (UserPara.detetWindowSize >= 1))
					{
						std::cout << "detetWindowSize range is 0-1(example: 0.7)" << std::endl;
						isSetError = 1;
						return;
					}
				}
				else
				{
					std::cout << "numSceneChange range is 2-numEvaluat(example: 3)" << std::endl;
					isSetError = 1;
					return;
				}
			}
			else
			{
				std::cout << "numEvaluat should more than 1(example: 4)" << std::endl;
				isSetError = 1;
				return;
			}
		}
		else
		{
			std::cout << "localThreshold range is 0-1(example: 0.9)" << std::endl;
			isSetError = 1;
			return;
		}
	}
	else
	{
		std::cout << "globalThreshold range is 0-1(example: 0.65)" << std::endl;
		isSetError = 1;
		return;
	}
	isSetError = 0;
}
//返回值 0表示图片无误
//1表示出现全局错误（灰、黑图）
//2表示出现局部错误（错位、缺失）
int QualityTest::main(cv::Mat pic)
{
	if (isSetError)
	{
		std::cout << "Parameters setting error!" << std::endl;
		return 1;
	}
	img = pic;
	if (UserPara.isLightLoop)
	{
		cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
	}
	
	NormalInf.totalImgNum++;

	getImgBuffer();

	if (i ==UserPara.numEvaluat)
	{
		//图片全局信息判断
		Err.isError = isGlobalError();
		if (Err.isError)
		{
			Err.totalNum++;
			Err.continuesNum++;
			std::cout << "全局" << std::endl;
			//对错误图进行处理&删除当前存入imgBuffer的图
			getErrBuffer();

			if (Buf.bufferCount == Buf.imgBuffer.begin())
				Buf.bufferCount == Buf.imgBuffer.end();
			else
				Buf.bufferCount--;
			
			//std::cout << "检测到黑、灰图" << std::endl;
			return 1;
		}
		//图片局部信息判断
		Err.isError = isLocalError();
		if (Err.isError)
		{
			Err.totalNum++;
			Err.continuesNum++;
			std::cout << "局部" << std::endl;
			getErrBuffer();
			if(Buf.bufferCount == Buf.imgBuffer.begin())
				Buf.bufferCount == Buf.imgBuffer.end();
			else
				Buf.bufferCount--;
			return Err.isError;
		}
		Err.continuesNum = 0;
		restartErrBuffer();
	}
	NormalInf.normalImgNum++;
	return 0;
}
//将得到的n张图片存入imgBuffer缓存
void QualityTest::getImgBuffer()
{
	if (NormalInf.totalImgNum==1)
	{
		for (; i < UserPara.numEvaluat; ++i) {
			Buf.imgBuffer.push_back(cv::Mat::zeros(1, 1, CV_8UC1));
		}
		i = -1;
		Buf.bufferCount = Buf.imgBuffer.begin();
	}
	if(i< UserPara.numEvaluat)++i;
	
	//每隔fixedTimeRefresh张图重新获取imgBuffer参考信息
	if (UserPara.fixedTimeRefresh==0)
		Buf.bufferTrigger = 0;
	else
	{
		if ((NormalInf.normalImgNum != 0) && (NormalInf.normalImgNum % UserPara.fixedTimeRefresh == 0))
			Buf.bufferTrigger = 1;
	}
	
	//当缓存图片足够时，检测场景渐变
	if (NormalInf.normalImgNum > UserPara.numEvaluat)
		Buf.bufferTrigger = isScenceChange();


	//当缓存图片足够 且 缓存开关打开时，再获取图片信息
	if ((i == UserPara.numEvaluat) && (Buf.bufferTrigger == 1))
	{
		getNormalImageInfo();
		Buf.bufferTrigger = 0;
	}

	*Buf.bufferCount = img;
	Buf.bufferCount++;
	//如果缓存图片超过预设值，则覆盖第一张缓存
	if (Buf.bufferCount == Buf.imgBuffer.end())
		Buf.bufferCount = Buf.imgBuffer.begin();
}

//初始化/重置errBuffer缓存
void QualityTest::restartErrBuffer()
{
	Err.imgBuffer.clear();
	for (int num = 0; num < UserPara.numSceneChange; ++num) {
		Err.imgBuffer.push_back(cv::Mat::zeros(1, 1, CV_8UC1));
	}
	Err.bufferCount = Err.imgBuffer.begin();
}

//将得到的n张图片存入errBuffer缓存
void QualityTest::getErrBuffer()
{
	//循环存储
	*Err.bufferCount = img;
	Err.bufferCount++;
	if (Err.bufferCount == Err.imgBuffer.end())
		Err.bufferCount = Err.imgBuffer.begin();
}

//将errBuffer缓存还给imgBuffer并更新normalImageInfo
void QualityTest::returnErrBuffer()
{
	//传递
	for (int i = 1; i <= UserPara.numSceneChange; i++)
	{
		*Buf.bufferCount = *Err.bufferCount;
		Err.bufferCount++;
		Buf.bufferCount++;
		if (Buf.bufferCount == Buf.imgBuffer.end())
			Buf.bufferCount = Buf.imgBuffer.begin();
	}
	Err.bufferCount = Err.imgBuffer.begin();
	//更新normalImageInfo
	if (i == UserPara.numEvaluat)
	{
		getNormalImageInfo();
		Buf.bufferTrigger = 0;
	}
}

//对imgBuffer中的图片进行数据统计，得到normalImgInfo
void QualityTest::getNormalImageInfo()
{

	if (Buf.bufferTrigger == 1)
	{
		NormalInf.RGB[0] = 0;
		NormalInf.RGB[1] = 0;
		NormalInf.RGB[2] = 0;
		for (int i = 0; i < 5; i++)
			NormalInf.point_R[i] = 0;

		cv::Mat img_channels[3], imgROI;
		cv::Rect ROI;
		cv::Scalar tempLocal, tempGlobal;
		int a = (int)img.cols / 4;
		int b = (int)img.rows / 4;
		//detectWindowWidth
		int dWW = (int)(UserPara.detetWindowSize * img.cols / 4);
		//detectWindowHeight
		int dWH = (int)(UserPara.detetWindowSize * img.rows / 4);
		((std::array<int, 20>&) point[0][0]) = { a - dWW / 2,b - dWH / 2,dWW,dWH,
							3 * a - dWW / 2,b - dWH / 2,dWW,dWH,
							2 * a - dWW / 2,2 * b - dWH / 2,dWW,dWH,
							a - dWW / 2,3 * b - dWH / 2,dWW,dWH,
							3 * a - dWW / 2,3 * b - dWH / 2,dWW,dWH };


		std::vector <cv::Mat>::iterator buf = Buf.imgBuffer.begin();

		for (int i = 0; i < UserPara.numEvaluat; i++, buf++)
		{
			cv::split(*buf, img_channels);
			//计算5个局部区域统计数据
			for (int j = 0; j < 5; j++)
			{
				ROI = cv::Rect(point[j][0], point[j][1], dWW, dWH);

				imgROI = img_channels[0](ROI);
				tempLocal = cv::mean(imgROI);
				NormalInf.point_R[j] = NormalInf.point_R[j] + tempLocal[0];
			}
			//计算全局统计数据
			tempGlobal = cv::mean(*buf);
			NormalInf.RGB[0] = NormalInf.RGB[0] + tempGlobal[0];
			NormalInf.RGB[1] = NormalInf.RGB[1] + tempGlobal[1];
			NormalInf.RGB[2] = NormalInf.RGB[2] + tempGlobal[2];
		}
		//获取全图统计数据
		NormalInf.RGB[0] = NormalInf.RGB[0] / UserPara.numEvaluat;
		NormalInf.RGB[1] = NormalInf.RGB[1] / UserPara.numEvaluat;
		NormalInf.RGB[2] = NormalInf.RGB[2] / UserPara.numEvaluat;
		for (int j = 0; j < 5; j++)
		{
			NormalInf.point_R[j] = NormalInf.point_R[j] / UserPara.numEvaluat;
		}

	}
}

//用于判断场景是否出现如开关灯、视野改变等亮度变化的情况
//若是，则返回bufferTrigger为1，获取新的imgInfo
//若否，则返回bufferTrigger为0
bool QualityTest::isScenceChange()
{

	if (Err.continuesNum >= UserPara.numSceneChange)
	{
		//对检测图片进行处理
		std::cout << "检测到连续图片错误" << std::endl;
		/*std::vector <cv::Mat>::iterator first;
		int temp;
		for (int i=1; i <= Err.continuesNum; i++)
		{
			temp = std::distance(first,Buf.bufferCount) - i + 2;
			if (temp <= 0)
				temp = temp + UserPara.numEvaluat;
		}*/
		//
		returnErrBuffer();
		Err.continuesNum = 0;
		return true;
	}
	else
		return false;
}

//用于检测图片是否有出现黑图、灰图等全局错误
bool QualityTest::isGlobalError()
{
	cv::Scalar RGB_mean = cv::mean(img);
	for (int i = 0; i <1 ; i++) 
	{
		if ((RGB_mean[i]>NormalInf.RGB[i]*(1+UserPara.globalThreshold)) 
			|| (RGB_mean[i] < NormalInf.RGB[i] * (1 - UserPara.globalThreshold)))
		{
			return true;
		}
	}
	return false;
}
//用于检测图片是否有出现错图、缺失等局部错误
bool QualityTest::isLocalError()
{
	double img_point_R[5];
	cv::Mat img_channels[3];
	cv::split(img, img_channels);
	for (int i = 0; i < 5; ++i)
	{
		cv::Rect ROI = cv::Rect(point[i][0], point[i][1], point[i][2], point[i][3]);
		cv::Mat imgROI = img_channels[0](ROI);
		cv::Scalar temp = cv::mean(imgROI);
		img_point_R[i] = temp[0];
		if (img_point_R[i] > NormalInf.point_R[i] * (1 + UserPara.localThreshold)
			|| img_point_R[i] < NormalInf.point_R[i] * (1 - UserPara.localThreshold))
		{
			return true;
		}
	}

	return false;
}




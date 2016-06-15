#include "CarDistance.h"

using namespace std;
using namespace DemoCarDistance;


int CCarDistance::Init(char* strCascadeFie)
{
	if(NULL != strCascadeFie)
	{
		m_strCascadeFile = strCascadeFie;
		m_cascadeCar = new  cv::CascadeClassifier( m_strCascadeFile);
	}
	else
	{
		CCascadeClassifier cascadeGenerator;
		m_cascadeCar = new  cv::CascadeClassifier();
		return -1;
	}

	if(m_cascadeCar->empty())
		return -1;
	return 0;
}

int CCarDistance::SetInputVideo(char* strInputVideo)
{
	if(NULL == strInputVideo)
		return -1;
	m_strInputVideo = strInputVideo;
	return 0;
}

int CCarDistance::SetImage(cv::Mat imageInput,ImageIndex index)
{
	if(imageInput.empty() )
		return -1;
	switch (index)
	{
		case CurImage: 
			m_imageCurrent = imageInput.clone();
			break;
		case PreImage1: 
			m_imagePre1 = imageInput.clone();
			break;
		case PreImage2: 
			m_imagePre2 = imageInput.clone();
			break;
		default:
			m_imageCurrent = imageInput.clone();
	}
	return 0;
}
int CCarDistance::DetectCar(cv::Mat imageInput,cv::Rect rectArea, std::vector<cv::Rect> &vecRectCar)
{
	if(imageInput.empty())
		return -1;
	if(m_cascadeCar->empty())
		return -1;

	if(rectArea.width == imageInput.cols && rectArea.height == imageInput.rows)
		m_cascadeCar->detectMultiScale(imageInput,vecRectCar,m_detectScaleFactor,m_minNeighbors,
									    0 ,cv::Size(m_iCarSize,m_iCarSize), cv::Size(6.5*m_iCarSize,6.5*m_iCarSize));
	else
	{
		cv::Mat imageDetect = imageInput(rectArea);
		m_cascadeCar->detectMultiScale(imageDetect,vecRectCar,m_detectScaleFactor,m_minNeighbors,
									    0 ,cv::Size(m_iCarSize,m_iCarSize), cv::Size(6.5*m_iCarSize,6.5*m_iCarSize));
		for(int i=0;i<vecRectCar.size();i++)
		{
			vecRectCar.at(i).x = vecRectCar.at(i).x + rectArea.x;
			vecRectCar.at(i).y = vecRectCar.at(i).y + rectArea.y;
		}
	}
	return 0;
}

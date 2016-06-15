#pragma once
#include "param.h"
#include "CascadeClassifier.h"

namespace DemoCarDistance {

	using namespace std;
	class CCarDistance
	{
	public:
		CCarDistance();
		~CCarDistance();
	private:
		char *m_strInputVideo;
		char *m_strCascadeFile;

		cv::CascadeClassifier *m_cascadeCar;

		cv::Mat m_imageCurrent;
		cv::Mat m_imagePre2;
		cv::Mat m_imagePre1;

		int m_iCarSize;		
		int m_minNeighbors;
		double m_scaleRatio;
		double m_detectScaleFactor;

		int DetectCar(cv::Mat imageInput, cv::Rect rectArea, std::vector<cv::Rect> &vecRectCar);
		int SelectFrontCar(cv::Mat imageInput, std::vector<cv::Rect> vecRectCar,cv::Rect &frontCar);
		int CompareNearestImage();
		int RefineCarImage(cv::Mat imageInput, std::vector<cv::Rect> &vecRectCar);
		double CalculateDistance(const CarType carType, const int carWidth);
		void FindBlobs(cv::Mat imageGray, cv::Mat &imageBinary, std::vector < std::vector<cv::Point2i> > &blobs);

	public:
		int Init(char *strCascadeFie);
		int SetInputVideo(char* strInputVideo);		
		int RunSingle(cv::Mat imageInput,double scaleRatio,cv::Mat &imageOutput);
		int SetImage(cv::Mat imageInput,ImageIndex index);
	};
}

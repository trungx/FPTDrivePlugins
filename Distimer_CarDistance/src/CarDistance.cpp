#include "CarDistance.h"
using namespace std;
using namespace DemoCarDistance;


bool IsFloatEqual(float lhs, float rhs)
{
    return (fabs(lhs - rhs) <= FLT_EPSILON * fabs(lhs));
}

bool IsDoubleEqual(double lhs, double rhs)
{
    return (fabs(lhs - rhs) <= DBL_EPSILON * fabs(lhs));
}

int Round(double x)
{
	int ix = (int)x;
	double r = x - ix;
	return ((r >= 0.5) ? (ix + 1) : ((r <= -0.5) ? (ix - 1) : ix));
}

double Round(double x, int numOfDigits)
{
	if (numOfDigits != 0)
	{
		double coef = pow(10.0, numOfDigits);
		x *= coef;
		x = Round(x);
		x /= coef;
	}
	else 
	{
		x = Round(x);
	}
	
	return x;
}


CCarDistance::CCarDistance()
{
	m_strInputVideo = NULL;
	m_strCascadeFile= "..\\G21_haar_v6.0_basic_24.xml";

	m_scaleRatio = 1.0;
	m_iCarSize = 24;
	m_detectScaleFactor = 1.1;
	m_minNeighbors = 3;
	
}

CCarDistance::~CCarDistance()
{

}

//// Select the car that is in the same lane
int CCarDistance::SelectFrontCar(cv::Mat imageInput, std::vector<cv::Rect> vecRectCar, cv::Rect &frontCar)
{
	if(vecRectCar.size() < 1)
		return -1;
	int iCenterImageX = imageInput.cols/2;
	int iCenterImageY = imageInput.rows/2;

	int index = 0;
	double minDis = 10000;
	for(int i=0;i<vecRectCar.size();i++)
	{
		cv::Rect curCar = vecRectCar.at(i);
		int iCenterCarX = curCar.x + curCar.width/2;
		int iCenterCarY = curCar.y + curCar.height/2;
		double curDis = sqrt( (double)( (iCenterImageX - iCenterCarX)*(iCenterImageX - iCenterCarX) + (iCenterImageY-iCenterCarY)*(iCenterImageY-iCenterCarY) )); 
		if(curDis < minDis)
		{
			minDis = curDis;
			index = i;
		}
	}
	frontCar = vecRectCar.at(index);
	return 0;
}

int CCarDistance::RefineCarImage(cv::Mat imageInput, std::vector<cv::Rect> &vecRectCar)
{
	return 0;
}

double CCarDistance::CalculateDistance(const CarType carType, const int carWidth)
{
	double distance = 0.0;

	double carTypeDependantConstant = 0.0;
	switch (carType)
	{
		case UnknownType: 
		case CompactCar:
		case Automobile:
		case AutomobileSedan:
			carTypeDependantConstant = 881.0;
			break;
		case TruckSmall:
		case Truck2T:
		case Truck4T:
		case DumpSmall:
		case DumpMedium:
		case DumpLarge:
		case Microbus:
		case BusinessBusSmall:
		case BusinessBusLarge:
			carTypeDependantConstant = 1095.0;
			break;
		default:
			break;
	}

	if (!IsDoubleEqual(carTypeDependantConstant, 0.0))
	{
		distance = 0.700 / tan(carWidth / carTypeDependantConstant) - 0.687;
		distance = std::max<double>(0.0, distance);
	}

	//return distace in cm
	return Round(distance, 1) * 100;
}

//// Process each image
int CCarDistance::RunSingle(cv::Mat imageInput, double scaleRatio, cv::Mat &imageOutput)
{	
	if(imageInput.empty())
		return -1;
	//// Resize image
	cv::Mat imageResize;
	if(IsDoubleEqual(1.0,scaleRatio))
		imageResize = imageInput.clone();
	else
	{
		cv::resize(imageInput,imageResize,cv::Size(imageInput.cols*scaleRatio,imageInput.rows*scaleRatio));
	}
	//// Create output image
	imageOutput = imageResize.clone();

	//// Detect car
	std::vector<cv::Rect> vecRectCar;
	cv::Rect rectArea = cv::Rect(200,0,300,imageResize.rows);
	int rsDetect = DetectCar(imageResize,rectArea,vecRectCar);
	if(0 != rsDetect)
		return -1;

	//// Select nearest front car
	cv::Rect frontCar;
	int rsChoose = SelectFrontCar(imageResize,vecRectCar,frontCar);
	if(rsChoose !=0)
		return -1;
	
	//// Estimage distance
	double dDistance = CalculateDistance(CarType::AutomobileSedan,frontCar.width);

	//// Draw result
	cv::rectangle(imageOutput,frontCar,cv::Scalar(0,0,255,0),2);
	int iStartX = frontCar.x + 5;
	int iStartY = frontCar.y + frontCar.height + 20;
	char text[50];
	
	int iStop = clock();	
	sprintf(text,"Distance : %d cm",(int)dDistance);	
	cv::putText(imageOutput,text,cv::Point(iStartX,iStartY),cv::FONT_HERSHEY_COMPLEX,0.7,cv::Scalar(0,0,255,0),2,8);
	
	return dDistance;
}

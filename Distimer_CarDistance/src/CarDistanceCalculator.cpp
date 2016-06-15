#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <CarDistance.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

using namespace cv;
using namespace std;
using namespace DemoCarDistance;

int testFromVideo() {
	//// Init detect module
	CCarDistance *carDistanceObj = new CCarDistance();
	int rsInit = carDistanceObj->Init(const_cast<char*>("G21_haar_v6.0_basic_24.xml"));
	if(0 != rsInit)
		return 0;

	//// Read video from video
	//cv::VideoCapture video(const_cast<char*>("FILE0026_new.mp4"));
	cv::VideoCapture video(0);
	cv::Mat img_src;
	video.read(img_src);
	if(img_src.empty())
		return 0;

	//// Select resize ratio
	bool m_isRunning = true;
	int iWidth = 640;
	double dRatio = (double)640/img_src.cols;

	//// Output to video file
				int codec = CV_FOURCC('D', 'I', 'V', 'X');
				cv::VideoWriter videoWriter(const_cast<char*>("FILE0026_new_ooouuuttt.mp4"),codec,24,cv::Size(640,img_src.rows*dRatio));

				//// Run current video
				while(m_isRunning)
				{
					cv::Mat img_src;
					cv::Mat img_dst;
					video.read(img_src);
					if(img_src.empty())
						m_isRunning = false;

					//// Run calculate distance
					// AnhNT62: ---- Main function .
					carDistanceObj->RunSingle(img_src,dRatio,img_dst);
					cv::imshow("Result",img_dst);

					//// Write to video output
					videoWriter<<(img_dst);
					int key = cv::waitKey(10);
					//if(27 == key)
					//	m_isRunning = false;

				}//// End of run current video

	return 0;
}

int runFromCamera() {
	//// Init detect module
	CCarDistance *carDistanceObj = new CCarDistance();
	int rsInit = carDistanceObj->Init(const_cast<char*>("G21_haar_v6.0_basic_24.xml"));
	if(0 != rsInit)
		return 0;

	//// Read video from video
	//cv::VideoCapture video(const_cast<char*>("FILE0026_new.mp4"));
	cv::VideoCapture video(0);
	cv::Mat img_src;
	video.read(img_src);
	if(img_src.empty())
		return 0;

	//// Select resize ratio
	bool m_isRunning = true;
	int iWidth = 640;
	double dRatio = (double)640/img_src.cols;

				//// Run current video
				while(m_isRunning)
				{
					cv::Mat img_src;
					cv::Mat img_dst;
					video.read(img_src);
					if(img_src.empty())
						m_isRunning = false;

					//// Run calculate distance
					// AnhNT62: ---- Main function .
					int iDistance = carDistanceObj->RunSingle(img_src,dRatio,img_dst);
					cout <<  iDistance << std::endl ;
					
					int key = cv::waitKey(10);
				}//// End of run current video

	return 0;
}

int testFromImage(char * imgPath) {
	CCarDistance *carDistanceObj = new CCarDistance();
		int rsInit = carDistanceObj->Init(const_cast<char*>("G21_haar_v6.0_basic_24.xml"));
		if(0 != rsInit)
			return 0;

	cv::Mat img_src = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);
	cv::Mat img_dst;

	double dRatio = (double)640/img_src.cols;
	int iDistance = carDistanceObj->RunSingle(img_src,dRatio,img_dst);

	cout <<  iDistance << std::endl ;

	//cv::namedWindow( "Display window", WINDOW_AUTOSIZE );
	//cv::imshow("Result",img_dst);
	//waitKey(0);

	return 0;
}

int main( int argc, char** argv )
{
	bool isFromCamera = true;
	
	if (isFromCamera) {
		runFromCamera();
		/*
		int pid = fork();
		if (pid == 0) {
			execl("/usr/bin/curl", "curl", "http://localhost:1880/eyeblinked/",(char *)0);
		}*/
	} else {
		bool hardImage = false;

		if (hardImage) {
				testFromImage(const_cast<char*>("/Volumes/Work/Worksapce/java-workspace/samplecpp/imgs/sample4.jpg"));
		} else {
			//testFromVideo();
				 if( argc != 2) {
					 cout <<"-1" << endl;
					 return -1;
				 }

				 //"/Volumes/Work/Worksapce/java-workspace/samplecpp/imgs/sample1.jpg"
				testFromImage(argv[1]);
		}	
	}
	
}

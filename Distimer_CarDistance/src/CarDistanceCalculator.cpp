#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <csignal>
#include <chrono>
#include <thread>
#include <iostream>
#include <CarDistance.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace DemoCarDistance;

const std::string currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

int testFromVideo() {
	//// Init detect module
	CCarDistance *carDistanceObj = new CCarDistance();
	int rsInit = carDistanceObj->Init(
			const_cast<char*>("G21_haar_v6.0_basic_24.xml"));
	if (0 != rsInit)
		return 0;

	//// Read video from video
	//cv::VideoCapture video(const_cast<char*>("FILE0026_new.mp4"));
	cv::VideoCapture video(0);
	cv::Mat img_src;
	video.read(img_src);
	if (img_src.empty())
		return 0;

	//// Select resize ratio
	bool m_isRunning = true;
	int iWidth = 640;
	double dRatio = (double) 640 / img_src.cols;

	//// Output to video file
	int codec = CV_FOURCC('D', 'I', 'V', 'X');
	cv::VideoWriter videoWriter(const_cast<char*>("FILE0026_new_ooouuuttt.mp4"),
			codec, 24, cv::Size(640, img_src.rows * dRatio));

	//// Run current video
	while (m_isRunning) {
		cv::Mat img_src;
		cv::Mat img_dst;
		video.read(img_src);
		if (img_src.empty())
			m_isRunning = false;

		//// Run calculate distance
		// AnhNT62: ---- Main function .
		carDistanceObj->RunSingle(img_src, dRatio, img_dst);
		cv::imshow("Result", img_dst);

		//// Write to video output
		videoWriter << (img_dst);
		int key = cv::waitKey(10);
		//if(27 == key)
		//	m_isRunning = false;

	}					//// End of run current video

	return 0;
}

int runFromCamera(char * haar, char * noderedApi, bool isShowVideo) {
	//// Init detect module
	CCarDistance *carDistanceObj = new CCarDistance();
	int rsInit = carDistanceObj->Init(const_cast<char*>(haar));
	if (0 != rsInit)
		return 0;

	//// Read video from video
	//cv::VideoCapture video(const_cast<char*>("FILE0026_new.mp4"));
	cv::VideoCapture video(0);
	cv::Mat img_src;
	video.read(img_src);
	if (img_src.empty())
		return 0;

	//// Select resize ratio
	bool m_isRunning = true;
	int iWidth = 640;
	double dRatio = (double) 640 / img_src.cols;

	int waitInterval = 10;

	//// Run current video
	//int pidId = fork();
	char apiUrl[255];

	while (m_isRunning) {
		cv::Mat img_src;
		cv::Mat img_dst;
		video.read(img_src);
		if (img_src.empty())
			m_isRunning = false;

		//// Run calculate distance
		// AnhNT62: ---- Main function .
		int iDistance = carDistanceObj->RunSingle(img_src, dRatio, img_dst);
		if (isShowVideo) {
			cv::imshow("Result", img_dst);
		}

		if (iDistance > 0) {
			int pidId = fork();
			if (pidId == 0) {
				try {
					int iStop = clock();
					sprintf(apiUrl, "%s%s", noderedApi, iDistance);

					cout << apiUrl << endl;
					execl("/usr/bin/curl", "curl", apiUrl, (char *) 0);
				} catch (Exception e) {
				}
			}
		}

		cout << iDistance << std::endl;

		//int key = cv::waitKey(10);
		this_thread::sleep_for(chrono::milliseconds(waitInterval));
	}					//// End of run current video

	return 0;
}

int testFromImage(char * imgPath) {
	CCarDistance *carDistanceObj = new CCarDistance();
	int rsInit = carDistanceObj->Init(
			const_cast<char*>("G21_haar_v6.0_basic_24.xml"));
	if (0 != rsInit)
		return 0;

	cv::Mat img_src = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);
	cv::Mat img_dst;

	double dRatio = (double) 640 / img_src.cols;
	int iDistance = carDistanceObj->RunSingle(img_src, dRatio, img_dst);

	cout << iDistance << std::endl;

	//cv::namedWindow( "Display window", WINDOW_AUTOSIZE );
	//cv::imshow("Result",img_dst);
	//waitKey(0);

	return 0;
}

int main(int argc, char** argv) {
	bool isFromCamera = true;

	if (isFromCamera) {
		if (argc < 3) {
			cout << "App XML API isShowVideo" << endl;
			cout
					<< "Ex: Distimer_CarDistance G21_haar_v6.0_basic_24.xml http://localhost:1880/api/cardistance/?distance= true"
					<< endl;
			cout << "-1" << endl;
			return -1;
		} else {
			bool isShowVideo = false;
			if (argc == 4) {
				isShowVideo = true;
			}
			runFromCamera(argv[1], argv[2], isShowVideo);
		}
	} else {
		bool hardImage = false;

		if (hardImage) {
			testFromImage(
					const_cast<char*>("/Volumes/Work/Worksapce/java-workspace/samplecpp/imgs/sample4.jpg"));
		} else {
			//testFromVideo();
			if (argc != 2) {
				cout << "-1" << endl;
				return -1;
			}

			//"/Volumes/Work/Worksapce/java-workspace/samplecpp/imgs/sample1.jpg"
			testFromImage(argv[1]);
		}
	}

}

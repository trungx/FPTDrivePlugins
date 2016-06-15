/*
Author: DuongVB@fsoft.com.vn
*/

#pragma once
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include "eye_blink_detector.h"
#include "Param.h"

using namespace std;
using namespace cv;

class frm {
	private:
          CascadeClassifier* m_face_cascade;	

	public:
	  frm();
	  ~frm();
          void startWork(char * haarcascade_frontalface_alt);
          void test(int& a);
	private: 
	  int checkEyesBlinkStatus(cv::Mat frame, bool& isBlink);
	  void calculateCropROI(cv::Mat  imageCaptureFull, int& winX, int& winY, int& winW, int& winH);
       
};

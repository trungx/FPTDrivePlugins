/*
 Author: DuongVB@fsoft.com.vn
 */

#include <algorithm>
#include <csignal>
#include "frm.h"
#include <iostream>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <time.h>

bool isRunning;

void sigIntHandler(int signal) {
	isRunning = true;
}

//Init class frm
frm::frm() {
	isRunning = false;
}

// Destroy class frm
frm::~frm() {

}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

// Capture video and create data for report.
void frm::startWork(char * haarcascade_frontalface_alt, int cameraId, char * noderedApi, bool isShowVideo) {
	try {
		m_face_cascade = new cv::CascadeClassifier(haarcascade_frontalface_alt);
		if (m_face_cascade->empty()) {
			printf("Error on loading face cascade");
		}

		VideoCapture cameraRecord(cameraId); //0 is the id of video device.0 if you have only one camera.
		if (!cameraRecord.isOpened()) { //check if video device has been initialised
			cout << "cannot open camera cameraId=" << cameraId << endl;
			exit(1);
		}

		// set up parameter :
		cameraRecord.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH); //set camera frame width and height
		cameraRecord.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
		int waitInterval = 1000 / CAMERA_FPS;

		while (!isRunning) {
			// Create image from cropped area.
			Mat imageCaptureFull;
			cameraRecord.read(imageCaptureFull);
			if (imageCaptureFull.empty())
				break;
			int winX = 0;
			int winY = 0;
			int winW = imageCaptureFull.cols;
			int winH = imageCaptureFull.rows;
			calculateCropROI(imageCaptureFull, winX, winY, winW, winH);
			Mat imageCrop = imageCaptureFull(cv::Rect(winX, winY, winW, winH));

			if (isShowVideo) {
				imshow("camera", imageCrop); //show camera for verification
				waitKey(waitInterval);
			}

			//Create report data
			bool status = false;
			checkEyesBlinkStatus(imageCrop, status);
			if (status) {
				cout << "1" << endl;
				int pidId = fork();
				if (pidId == 0) {
					try {
						execl("/usr/bin/curl", "curl", noderedApi, (char *) 0);
					} catch (Exception e) {
					}
				}
			} else {
				cout << currentDateTime() << "\t0" << endl;
			}

			this_thread::sleep_for(chrono::milliseconds(waitInterval));
		}
	} catch (int e) {
		printf("read file error\n");
	}

}

// eyes blink checking
int frm::checkEyesBlinkStatus(Mat frame,
		bool& isBlink/*, CascadeClassifier *face_cascade*/) {
	Mat gray_frame;

	eye_blink_detector eye_det;
	Mat imgClone;

	frame.copyTo(imgClone);
	if (frame.empty())
		return -1;
	cvtColor(frame, gray_frame, CV_BGR2GRAY);
	vector < Rect > faces;
	if (gray_frame.empty())
		return -1;
	m_face_cascade->detectMultiScale(gray_frame, faces, 1.2, 2, 0 /*| CV_HAAR_SCALE_IMAGE |
	 CV_HAAR_FIND_BIGGEST_OBJECT*/, cv::Size(150, 150));
	if (faces.size() > 0) {
		rectangle(frame, faces[0], Scalar(0, 0, 255), 1, 8, 0);
		isBlink = eye_det.get_blink(imgClone, gray_frame, faces[0]);
		faces.clear();
		return 1;
	} else {
		//frame.release();
		isBlink = false;
		return 0;
	}

}

void frm::calculateCropROI(cv::Mat imageCaptureFull, int& winX, int& winY,
		int& winW, int& winH) {
	if (imageCaptureFull.empty())
		return;

	int w = imageCaptureFull.cols;
	int h = imageCaptureFull.rows;
	int wintop = 0;
	int winleft = 0;
	int winw = w;
	int winh = h;

	//chuan bi vung cua so sao chep
	float ImageScalingFactor_X = (float) w / PBXCAMIMG_WIDTH;
	float ImageScalingFactor_Y = (float) h / PBXCAMIMG_HEIGHT;
	float ScalingFactor = std::max(ImageScalingFactor_X, ImageScalingFactor_Y);
	ImageScalingFactor_X = ScalingFactor;
	ImageScalingFactor_Y = ScalingFactor;
	float ImageOffsetInClient_X = (PBXCAMIMG_WIDTH - w / ImageScalingFactor_X)
			/ 2;
	float ImageOffsetInClient_Y = (PBXCAMIMG_HEIGHT - h / ImageScalingFactor_Y)
			/ 2;
	int wz = w / ScalingFactor;
	int hz = h / ScalingFactor;

	// duongvb hardcode for pbCamImg->Width = 192 and pbCamImg->Height = 144				
	int w_ins_tlx = MIN(PBXCAMIMG_WIDTH / 2 + wz / 2,
			MAX((PBXCAMIMG_WIDTH - wz) / 2, WIN_LEFT)) + 1;
	int w_ins_tly = MIN(PBXCAMIMG_HEIGHT / 2 + hz / 2,
			MAX((PBXCAMIMG_HEIGHT - hz) / 2, WIN_TOP)) + 1;
	int w_ins_brx = MIN(PBXCAMIMG_WIDTH / 2 + wz / 2,
			MAX((PBXCAMIMG_WIDTH - wz) / 2, WIN_LEFT + WIN_WIDTH)) - 1;
	int w_ins_bry = MIN(PBXCAMIMG_HEIGHT / 2 + hz / 2,
			MAX((PBXCAMIMG_HEIGHT - hz) / 2, WIN_TOP + WIN_HEIGHT)) - 1;

	winleft =
			(int) ((w_ins_tlx - ImageOffsetInClient_X) * ImageScalingFactor_X);
	wintop = (int) ((w_ins_tly - ImageOffsetInClient_Y) * ImageScalingFactor_Y);

	winw = (int) ((w_ins_brx - w_ins_tlx) * ImageScalingFactor_X / 4);
	winh = (int) ((w_ins_bry - w_ins_tly) * ImageScalingFactor_Y / 4);
	winw = 4 * winw;
	winh = 4 * winh;
	//chuan bi xong				

	//// Output
	winX = winleft;
	winY = wintop;
	winW = winw;
	winH = winh;
	return;
}


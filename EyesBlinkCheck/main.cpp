/*
 Author: DuongVB@fsoft.com.vn
 */
#pragma once
#include <stdio.h>
#include "frm.h"

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "App <haar xml file> <nodered api> " << endl;
		cout
				<< "Ex: EyesBlinkCheck haarcascade_frontalface_alt.xml http://localhost:1880/api/eyesblinked isShowVideo"
				<< endl;
		cout << "-1" << endl;
		return -1;
	}

	bool isShowVideo = false;
	if (argc == 4) {
		isShowVideo = true;
	}

	frm eyesDetect;
	eyesDetect.startWork(argv[1], argv[2], isShowVideo);
	waitKey(0);
	return 0;
}


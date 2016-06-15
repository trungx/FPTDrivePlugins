/*
 Author: DuongVB@fsoft.com.vn
 */
#pragma once
#include <stdio.h>
#include "frm.h"

int main(int argc, char** argv) {
	if (argc < 4) {
		cout << "EyesBlinkDetect <haar xml file> <cam_id> <nodered api> <isShowVideo>" << endl;
		cout
				<< "Ex: EyesBlinkDetect haarcascade_frontalface_alt.xml 0 http://localhost:1880/api/eyesblinked isShowVideo"
				<< endl;
		cout << "-1" << endl;
		return -1;
	}

	bool isShowVideo = false;
	if (argc == 5) {
		isShowVideo = true;
	}

	frm eyesDetect;
	int camId = argv[2][0] - '0';
	eyesDetect.startWork(argv[1], camId, argv[3], isShowVideo);
	waitKey(0);
	return 0;
}


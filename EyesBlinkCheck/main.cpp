/*
 Author: DuongVB@fsoft.com.vn
 */
#pragma once
#include <stdio.h>
#include "frm.h"

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "App <haar xmm> <nodered api>" << endl;
		cout << "Ex: EyesBlinkCheck haarcascade_frontalface_alt.xml http://localhost:1880/eyesblinked" << endl;
		cout << "-1" << endl;
		return -1;
	}

	frm eyesDetect;
	eyesDetect.startWork(argv[1], argv[2]);
	waitKey(0);
	return 0;
}


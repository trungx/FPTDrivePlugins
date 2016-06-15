#include <iostream>
#include <vector>
#include "CarDistance.h"

using namespace std;
using namespace DemoCarDistance;

void CCarDistance::FindBlobs(cv::Mat imageGray, cv::Mat &imageBinary, std::vector < std::vector<cv::Point2i> > &blobs)
{
    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 255  - unlabelled foreground
    // 2+ - labelled foreground
	
	cv::Mat imageSobel = cv::Mat::zeros(imageGray.size(), CV_8UC1);
	cv::Mat imageSobelVertical = cv::Mat::zeros(imageGray.size(), CV_8UC1);
	cv::Mat imageSobelHorizontal = cv::Mat::zeros(imageGray.size(), CV_8UC1);

	if(0)
	{
		cv::Sobel(imageGray,imageSobelVertical,CV_8U,0,1);
		cv::Sobel(imageGray,imageSobelHorizontal,CV_8U,1,0);
		cv::bitwise_or(imageSobelVertical,imageSobelHorizontal,imageSobel);
	}
	else
		cv::Sobel(imageGray,imageSobel,CV_8U,1,0);
	cv::imshow("Image Sobel",imageSobel);

	//// Threshold Sobel
	cv::threshold(imageSobel,imageBinary,125,255,CV_THRESH_BINARY);

	// find threshold to binary image
	double total = cv::sum(imageGray)[0];
	double avegr = total/(imageGray.total());
	cv::Mat imageCanny = cv::Mat::zeros(imageGray.size(), CV_8UC1);
	cv::Canny(imageGray,imageCanny,avegr*0.85,avegr*1.25);
	cv::imshow("Image Canny",imageCanny);

	//// Find connected component
    cv::Mat label_image;
    imageBinary.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++) {
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++) {
			if(255 == label_count)
				label_count++;

            if(row[x] != 255)
                continue;

            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x,y), label_count, &rect, 0, 0, 4);
            std::vector <cv::Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(row2[j] != label_count)
                        continue;
                    blob.push_back(cv::Point2i(j,i));
                }
			}

            blobs.push_back(blob);
            label_count++;
        }
    }
	
	//// Show Image
	cv::Mat imageComponent = cv::Mat::zeros(imageBinary.size(), CV_8UC3);
	//// Randomy color the blobs
    for(size_t i=0; i < blobs.size(); i++) {
        unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
        unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
        unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));

		if(blobs[i].size() < 20)
			continue;

        for(size_t j=0; j < blobs[i].size(); j++) {
            int x = blobs[i][j].x;
            int y = blobs[i][j].y;

            imageComponent.at<cv::Vec3b>(y,x)[0] = b;
            imageComponent.at<cv::Vec3b>(y,x)[1] = g;
            imageComponent.at<cv::Vec3b>(y,x)[2] = r;
        }
    }
	cv::imshow("Extend Image",imageGray);
	cv::imshow("Binary Image",imageBinary);
	cv::imshow("Component Image",imageComponent);
	cv::waitKey(0);
	cv::destroyAllWindows();
}
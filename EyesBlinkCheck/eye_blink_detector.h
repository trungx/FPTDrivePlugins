#pragma once
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//#include <iostream>
#include <queue>

//using namespace cv;
//using namespace std;
//int checkStatus(cv::Mat frame, bool &isBlink, bool &isYawning, cv::CascadeClassifier *face_cascade);

class eye_blink_detector
{
public:

    // Size constants
    int m_eyePercentTop ;
    int m_eyePercentSide ;
    int m_eyePercentHeight;
    int m_eyePercentWidth;

    int m_kFastEyeWidth;

    eye_blink_detector();

    cv::Point
    unscalePoint(cv::Point p, cv::Rect origSize) ;

    void
    scaleToFastSize(const cv::Mat &src, cv::Mat &dst);

    bool
    get_blink(cv::Mat frame,
              cv::Mat gray_frame,
              cv::Rect face);

    cv::Mat
    matrixMagnitude(const cv::Mat &matX, const cv::Mat &matY);

    double
    computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor);

    cv::Mat
    computeMatXGradient(const cv::Mat &mat);

    void
    testPossibleCentersFormula(int x,
                               int y,
                               const cv::Mat &weight,
                               double gx,
                               double gy,
                               cv::Mat &out);
    cv::Point
    findEyeCenter(cv::Mat face,
                  cv::Rect eye);

    cv::Mat
    floodKillEdges(cv::Mat &mat);

    bool
    floodShouldPushPoint(const cv::Point &np,
                         const cv::Mat &mat);

    bool
    matching( cv::Mat gray_frame,
              cv::Point Pupil);

    bool
    isInBox( cv::Point Pupil, cv::Rect box );

};


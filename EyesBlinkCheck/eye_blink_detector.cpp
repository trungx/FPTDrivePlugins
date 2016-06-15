#include "eye_blink_detector.h"
#include "stdio.h"

using namespace cv;
using namespace std;

eye_blink_detector::eye_blink_detector()
{
	// Size constants
	m_eyePercentTop = 30;
	m_eyePercentSide = 15;
	m_eyePercentHeight = 20;
	m_eyePercentWidth = 30;

	m_kFastEyeWidth = 50;
}

Point
eye_blink_detector::
unscalePoint(Point p, Rect origSize)
{
	float ratio = (((float)m_kFastEyeWidth) / origSize.width);
	int x = (int)ceil(p.x / ratio);
	int y = (int)ceil(p.y / ratio);
	return Point(x, y);
}

void
eye_blink_detector::
scaleToFastSize(const Mat &src, Mat &dst)
{
	resize(src, dst,
		Size(m_kFastEyeWidth,
		(int)((((float)m_kFastEyeWidth) / src.cols) * src.rows)));
}

Mat
eye_blink_detector::
matrixMagnitude(const Mat &matX, const Mat &matY)
{
	Mat mags(matX.rows, matX.cols, CV_64F);
	for (int y = 0; y < matX.rows; ++y)
	{
		const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
		double *Mr = mags.ptr<double>(y);
		for (int x = 0; x < matX.cols; ++x)
		{
			double gX = Xr[x], gY = Yr[x];
			double magnitude = sqrt((gX * gX) + (gY * gY));
			Mr[x] = magnitude;
		}
	}
	return mags;
}

double
eye_blink_detector::
computeDynamicThreshold(const Mat &mat, double stdDevFactor)
{
	Scalar stdMagnGrad, meanMagnGrad;
	meanStdDev(mat, meanMagnGrad, stdMagnGrad);
	double stdDev = (double)stdMagnGrad[0] / std::sqrt((double)mat.rows*mat.cols);
	return stdDevFactor * stdDev + meanMagnGrad[0];
}


Mat
eye_blink_detector::
computeMatXGradient(const Mat &mat)
{
	Mat out(mat.rows, mat.cols, CV_64F);

	for (int y = 0; y < mat.rows; ++y)
	{
		const uchar *Mr = mat.ptr<uchar>(y);
		double *Or = out.ptr<double>(y);

		Or[0] = Mr[1] - Mr[0];
		for (int x = 1; x < mat.cols - 1; ++x)
		{
			Or[x] = (Mr[x + 1] - Mr[x - 1]) / 2.0;
		}
		Or[mat.cols - 1] = Mr[mat.cols - 1] - Mr[mat.cols - 2];
	}

	return out;
}


void
eye_blink_detector::
testPossibleCentersFormula(int x,
int y,
const Mat &weight,
double gx,
double gy,
Mat &out)
{
	// for all possible centers
	for (int cy = 0; cy < out.rows; ++cy)
	{
		double *Or = out.ptr<double>(cy);
		const uchar *Wr = weight.ptr<uchar>(cy);

		for (int cx = 0; cx < out.cols; ++cx)
		{
			if (x == cx && y == cy)
			{
				continue;
			}

			// create a vector from the possible center to the gradient origin
			double dx = x - cx;
			double dy = y - cy;

			// normalized
			double magnitude = sqrt((dx * dx) + (dy * dy));
			dx = dx / magnitude;
			dy = dy / magnitude;

			double dotProduct = dx*gx + dy*gy;
			dotProduct = max(0.0, dotProduct);

			// square and multiply by the weight
			Or[cx] += dotProduct * dotProduct * (Wr[cx]);

		}
	}
}

Point
eye_blink_detector::
findEyeCenter(Mat face,
Rect eye)
{
	Mat eyeROIUnscaled = face(eye);
	Mat eyeROI;
	scaleToFastSize(eyeROIUnscaled, eyeROI);

	//  namedWindow( "s", CV_WINDOW_NORMAL );
	//  resizeWindow( "s", eyeROI.size().width * 4, eyeROI.size().height * 4);
	//  imshow( "s", eyeROI);

	// draw eye region
	//rectangle(face,eye,1234);

	//-- Find the gradient
	Mat gradientX = computeMatXGradient(eyeROI);
	Mat gradientY = computeMatXGradient(eyeROI.t()).t();

	//-- Normalize and threshold the gradient
	// compute all the magnitudes
	Mat mags = matrixMagnitude(gradientX, gradientY);

	//compute the threshold
	double gradientThresh = computeDynamicThreshold(mags, 1);

	//normalize
	for (int y = 0; y < eyeROI.rows; ++y)
	{
		double *Xr = gradientX.ptr<double>(y);
		double *Yr = gradientY.ptr<double>(y);
		const double *Mr = mags.ptr<double>(y);

		for (int x = 0; x < eyeROI.cols; ++x)
		{
			double gX = Xr[x], gY = Yr[x];
			double magnitude = Mr[x];
			if (magnitude > gradientThresh)
			{
				Xr[x] = gX / magnitude;
				Yr[x] = gY / magnitude;
			}
			else
			{
				Xr[x] = 0.0;
				Yr[x] = 0.0;
			}
		}
	}

	// Create a blurred and inverted image for weighting
	Mat weight;
	GaussianBlur(eyeROI, weight, Size(5, 5), 0, 0);

	for (int y = 0; y < weight.rows; ++y)
	{
		unsigned char *row = weight.ptr<unsigned char>(y);
		for (int x = 0; x < weight.cols; ++x)
		{
			row[x] = (255 - row[x]);
		}
	}


	// Run the algorithm!

	Mat outSum = Mat::zeros(eyeROI.rows, eyeROI.cols, CV_64F);

	// for each possible gradient location
	// Note: these loops are reversed from the way the paper does them
	// it evaluates every possible center for each gradient location instead of
	// every possible gradient location for every center.

	for (int y = 0; y < weight.rows; ++y)
	{
		const double *Xr = gradientX.ptr<double>(y);
		const double *Yr = gradientY.ptr<double>(y);

		for (int x = 0; x < weight.cols; ++x)
		{
			double gX = Xr[x], gY = Yr[x];
			if (gX == 0.0 && gY == 0.0)
			{
				continue;
			}
			testPossibleCentersFormula(x, y, weight, gX, gY, outSum);
		}
	}

	// scale all the values down, basically averaging them
	double numGradients = (weight.rows*weight.cols);
	Mat out;
	outSum.convertTo(out, CV_32F, 1.0 / numGradients);

	// Find the maximum point
	Point maxP;
	double maxVal;

	minMaxLoc(out, NULL, &maxVal, NULL, &maxP);

	// Postprocessing

	Mat floodClone;
	double floodThresh = maxVal * 0.97;
	threshold(out, floodClone, floodThresh, 0.0f, THRESH_TOZERO);

	Mat mask = floodKillEdges(floodClone);

	// redo max
	minMaxLoc(out, NULL, &maxVal, NULL, &maxP, mask);

	return unscalePoint(maxP, eye);
}

// returns a mask
Mat
eye_blink_detector::
floodKillEdges(Mat &mat)
{
	rectangle(mat, Rect(0, 0, mat.cols, mat.rows), 255);

	Mat mask(mat.rows, mat.cols, CV_8U, 255);
	queue<Point> toDo;
	toDo.push(Point(0, 0));

	while (!toDo.empty())
	{
		Point p = toDo.front();
		toDo.pop();
		if (mat.at<float>(p) == 0.0f)
		{
			continue;
		}
		// add in every direction
		Point np(p.x + 1, p.y); // right
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x - 1; np.y = p.y; // left
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y + 1; // down
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		np.x = p.x; np.y = p.y - 1; // up
		if (floodShouldPushPoint(np, mat)) toDo.push(np);
		// kill it
		mat.at<float>(p) = 0.0f;
		mask.at<uchar>(p) = 0;
	}
	return mask;
}

bool
eye_blink_detector::
floodShouldPushPoint(const Point &np,
const Mat &mat)
{
	return np.x >= 0 && np.x < mat.cols && np.y >= 0 && np.y < mat.rows;
}


bool
eye_blink_detector::
get_blink(Mat frame,
Mat gray_frame,
Rect face)
{
	bool is_blink = false;

	Mat faceROI = gray_frame(face).clone();

	equalizeHist(faceROI, faceROI);

	double sigma = 0.005 * face.width;
	GaussianBlur(faceROI, faceROI, Size(0, 0), sigma);

	//-- Find eye regions and draw them
	int eye_region_width = (int)(face.width  * (m_eyePercentWidth / 100.0));
	int eye_region_height = (int)(face.height * (m_eyePercentHeight / 100.0));
	int eye_region_top = (int)(face.height * (m_eyePercentTop / 100.0));
	int eye_region_side = (int)(face.width  * (m_eyePercentSide / 100.0));

	Rect leftEyeRegion(eye_region_side,
		eye_region_top,
		eye_region_width,
		eye_region_height);

	Rect rightEyeRegion(face.width - eye_region_width - eye_region_side,
		eye_region_top,
		eye_region_width,
		eye_region_height);
 
	// Find Eye Centers
	Point leftPupil = findEyeCenter(faceROI, leftEyeRegion);

	Point rightPupil = findEyeCenter(faceROI, rightEyeRegion);
	

	Rect ll(leftEyeRegion);
	Rect lr(rightEyeRegion);

	ll.x += face.x;
	ll.y += face.y;
	lr.x += face.x;
	lr.y += face.y;
	//rectangle(frame, ll, 200);
	//rectangle(frame, lr, 200);


	// change eye centers to face coordinates
	rightPupil.x += rightEyeRegion.x + face.x;
	rightPupil.y += rightEyeRegion.y + face.y;
	leftPupil.x += leftEyeRegion.x + face.x;
	leftPupil.y += leftEyeRegion.y + face.y;

	//circle(frame, leftPupil, 3, 1234);
	//circle(frame, rightPupil, 3, 1234);

	bool left_eye_closed, right_eye_closed;
	

	if (isInBox(leftPupil, ll))
		left_eye_closed = matching(gray_frame, leftPupil);
	else
		left_eye_closed = true;

	if (isInBox(rightPupil, lr))
		right_eye_closed = matching(gray_frame, rightPupil);
	else
		right_eye_closed = true;

	if (right_eye_closed && left_eye_closed)
		is_blink = true;
	else
		is_blink = false;
	return is_blink;
}

bool
eye_blink_detector::
isInBox(Point Pupil, Rect box)
{
	//    cout<< endl<< Pupil<< endl<< box<< endl;

	bool out_of_box = false;
	int offset = 5;

	if ((Pupil.x > box.x + offset && Pupil.x < box.x + box.width - offset) &&
		(Pupil.y > box.y + offset && Pupil.y < box.y + box.height - offset))
		out_of_box = false;
	else
		out_of_box = true;

	return !out_of_box;
}

bool
eye_blink_detector::
matching(Mat gray_frame,
Point Pupil)
{

	int k1 = 10, k2 = 20;
	int rx = Pupil.x - k2;
	int ry = Pupil.y - k1;

	Rect eyeROI(rx, ry, 2 * k2, 2 * k1);

	Mat eye = gray_frame(eyeROI).clone();

	Mat result1, result2;

	Mat eye_open = imread("eye_open.jpg", 0); //edited by duongvb
	Mat eye_closed = imread("eye_closed.jpg", 0); //edited by duongvb

	result1.create(eye_open.cols, eye_open.rows, CV_32FC1);

	result2.create(eye_closed.cols, eye_closed.rows, CV_32FC1);

	matchTemplate(eye_open, eye, result1, CV_TM_SQDIFF_NORMED);

	matchTemplate(eye_closed, eye, result2, CV_TM_SQDIFF_NORMED);

	Point  minloc1, maxloc1;
	double minval1, maxval1;

	minMaxLoc(result1, &minval1, &maxval1, &minloc1, &maxloc1);

	Point  minloc2, maxloc2;
	double minval2, maxval2;
	minMaxLoc(result2, &minval2, &maxval2, &minloc2, &maxloc2);

	bool is_closed = false;

	if (minval1 < minval2)
	{
		//cout << "Eye is Open" << endl;
		is_closed = false;
	}
	else
	{
		//cout << "Eye is Closed" << endl;
		is_closed = true;
	}

	return is_closed;
}

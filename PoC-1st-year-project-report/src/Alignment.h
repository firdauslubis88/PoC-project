#pragma once

#include <vector>
#include "ofImage.h"
#include "ofxOpenCv.h"
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/optflow.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

#define USE_PTZ_ADJUSTMENT

class Alignment
{
public:
	Alignment();
	Alignment(int minHessian);
	static void ptzAlign(const Mat refImage, const Mat inputImage, const int x, const int y, const int mask_width, const int mask_height);
	static void align(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	static Mat align_direct(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	static bool ptzAlreadyChanged;
	static bool alreadyChanged;
	static int xReturn, yReturn;
	~Alignment();

private:
	static Ptr<SURF> detector;
	static float comparisonThreshold;
	vector<Point2f> pts, pts2;
	static Mat hBig;
	static bool alreadyCreated;
	static int minHessian;
};


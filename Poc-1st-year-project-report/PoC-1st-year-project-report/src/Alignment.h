#pragma once

#include <vector>
#include "ofImage.h"
#include "ofxOpenCv.h"
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/optflow.hpp>

using namespace cv;
using namespace cv::xfeatures2d;

class Alignment
{
public:
	Alignment();
	Alignment(int minHessian);
	Mat align(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	~Alignment();

private:
	Ptr<SURF> detector;
	double comparisonThreshold;
	vector<Point2f> pts, pts2;
	Mat hBig;
};


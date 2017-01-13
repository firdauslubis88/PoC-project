#pragma once

#include <vector>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/reg/mapprojec.hpp>
#include <opencv2/reg/mapaffine.hpp>
#include <opencv2/reg/mappergradproj.hpp>
#include <opencv2/reg/mappergradaffine.hpp>
#include <opencv2/reg/mapperpyramid.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace cv::reg;

//#define USE_PTZ_ADJUSTMENT
//#define ALIGNMENT_CHECK

namespace Alignment
{
	/*
	Ptr<SURF> detector;
	float comparisonThreshold;
	bool alreadyCreated;
	int counter;
	bool ptzAlreadyChanged;
	bool alreadyChanged;
	int xReturn, yReturn;
	*/

	Mat hBig;
	Ptr<Map> mapPtr;

	void ptzAlign(const Mat refImage, const Mat inputImage, const int x, const int y, const int mask_width, const int mask_height);
	bool align(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	Mat align_direct(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	bool align_reglib(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
	Mat align_direct_reglib(Mat refImage, Mat inputImage, int x, int y, int mask_width, int mask_height);
};


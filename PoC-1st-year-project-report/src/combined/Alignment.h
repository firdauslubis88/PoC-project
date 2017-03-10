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


//using namespace std;
//using namespace cv;
//using namespace cv::xfeatures2d;
//using namespace cv::reg;

//#define USE_PTZ_ADJUSTMENT
//#define ALIGNMENT_CHECK

class Alignment
{
public:
	Alignment();
	Alignment(int minHessian);
	static void ptzAlign(const cv::Mat refImage, const cv::Mat inputImage, const int x, const int y, const int mask_width, const int mask_height);
	static void align(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	static cv::Mat align_direct(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	static void align_reglib(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	static cv::Mat align_direct_reglib(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	static bool ptzAlreadyChanged;
	static bool alreadyChanged;
	static int xReturn, yReturn;
	static cv::Ptr<cv::reg::Map> mapPtr;
	~Alignment();

private:
	static cv::Ptr<cv::xfeatures2d::SURF> detector;
	static float comparisonThreshold;
	static cv::Mat hBig;
	static bool alreadyCreated;
	static int minHessian;
	static int counter;
};


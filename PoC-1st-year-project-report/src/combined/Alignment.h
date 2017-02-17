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
	void ptzAlign(const cv::Mat refImage, const cv::Mat inputImage, const int x, const int y, const int mask_width, const int mask_height);
	std::pair<cv::Point2f, cv::Point2f> track(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	void align(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	cv::Mat align_direct(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	void align_reglib(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	cv::Mat align_direct_reglib(cv::Mat refImage, cv::Mat inputImage, int x, int y, int mask_width, int mask_height);
	bool ptzAlreadyChanged;
	bool alreadyChanged;
	bool bPair;
	int xReturn, yReturn;
	cv::Ptr<cv::reg::Map> mapPtr;
	~Alignment();

private:
	cv::Ptr<cv::xfeatures2d::SURF> detector;
	float comparisonThreshold;
	cv::Mat hBig;
	bool alreadyCreated;
	int minHessian;
	int counter;
};


#pragma once
#include "ofImage.h"
#include "ofxOpenCv.h"
#include "Alignment.h"
#include "Cloning.h"

using namespace cv;

class CombinedCamera
{
public:
	CombinedCamera(int image_width, int image_height);
	~CombinedCamera();

//	void seamlessClone(InputArray _src, InputArray _dst, InputArray _mask, Point p, OutputArray _blend, int flags);

	ofPixels combine(ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);
	void setSkipCloning(bool value);

	void setSkipAligning(bool value);

	void MVCSeamlessClone(Mat source, Mat target, Mat mask, Point center, Mat & clone);

private:
	std::shared_ptr<ofxCvImage> combinedImage;
	ofxCvColorImage ldCvImage, hdCvImage, combinedCvImage, sourceCvImage;
	bool skipCloning = false, skipAligning = false;
};

#pragma once
#include "ofImage.h"
#include "ofxOpenCv.h"
#include "opencv2/core/core.hpp"

class CombinedCamera
{
public:
	CombinedCamera();
	~CombinedCamera();

	ofPixels combine(ofImage ldPixels, ofImage hdPixels, int x, int y, int width, int height);

private:
	std::shared_ptr<ofxCvImage> combinedImage;
};

#pragma once
#include "ofImage.h"
#include "ofxOpenCv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/photo/photo.hpp"

using namespace cv;

class CombinedCamera
{
public:
	CombinedCamera(int image_width, int image_height);
	~CombinedCamera();

//	void seamlessClone(InputArray _src, InputArray _dst, InputArray _mask, Point p, OutputArray _blend, int flags);

	ofPixels combine(ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);

private:
	std::shared_ptr<ofxCvImage> combinedImage;
	ofxCvColorImage ldCvImage, hdCvImage, combinedCvImage;

};


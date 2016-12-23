#pragma once
#include "ofImage.h"
#include "ofxOpenCv.h"
#include "Alignment.h"
#include "Cloning.h"

using namespace cv;

class CombinedCamera
{
public:
	CombinedCamera() = default;
	CombinedCamera(int image_width, int image_height);
	~CombinedCamera();

//	void seamlessClone(InputArray _src, InputArray _dst, InputArray _mask, Point p, OutputArray _blend, int flags);

	ofPixels combine(ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);
	static void combine(ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height, ofPixels* outputPixels);
	void setSkipCloning(bool value);
	void setSkipAligning(bool value);

private:
	std::shared_ptr<ofxCvImage> combinedImage;
	static ofxCvColorImage ldCvImage, hdCvImage, combinedCvImage;
	static bool skipCloning, skipAligning, alreadyInitialized;
};

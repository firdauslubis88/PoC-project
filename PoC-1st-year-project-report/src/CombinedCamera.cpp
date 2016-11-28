#include "CombinedCamera.h"



CombinedCamera::CombinedCamera()
{
}


CombinedCamera::~CombinedCamera()
{
}

ofPixels CombinedCamera::combine(ofImage ldPixels, ofImage hdPixels, int x, int y, int width, int height)
{
	ofxCvColorImage combinedColorImage = ofxCvColorImage();
	combinedColorImage.setFromPixels(hdPixels);
//	this->combinedImage = make_shared<ofxCvColorImage>(combinedColorImage);
	
	return ldPixels;
}

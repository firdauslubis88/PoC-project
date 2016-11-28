#include "CombinedCamera.h"



CombinedCamera::CombinedCamera()
{
}


CombinedCamera::~CombinedCamera()
{
}

ofPixels CombinedCamera::combine(ofPixels ldPixel, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height)
{
	ofImage ldImage;
	ofxCvColorImage ldCvImage, hdCvImage, combinedCvImage;

	ldImage.setFromPixels(ldPixel);
	ldImage.setImageType(OF_IMAGE_COLOR);

	ldCvImage.allocate(image_width, image_height);
	ldCvImage.setFromPixels(ldImage.getPixels());
	hdCvImage.allocate(image_width, image_height);
	hdCvImage.setFromPixels(hdImage.getPixels());
	combinedCvImage.allocate(image_width, image_height);

	combinedCvImage = ldCvImage;
	combinedCvImage.setROI(x, y, width, height);
	hdCvImage.setROI(x, y, width, height);
	ofPixels hdROIPixels = hdCvImage.getRoiPixels();
	combinedCvImage.setRoiFromPixels(hdROIPixels.getData(), hdROIPixels.getWidth(), hdROIPixels.getHeight());

	return combinedCvImage.getPixels();
}

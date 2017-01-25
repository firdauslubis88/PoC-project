#include "CombinedCameraTask.h"



CombinedCameraTask::CombinedCameraTask(const std::string& name, ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height): Poco::Task(name), privateLdPixels(ldPixels), privateHdImage(hdImage), privateImage_width(image_width), privateImage_height(image_height), privateX(x), privateY(y), privateWidth(width), privateHeight(height)
{
}


CombinedCameraTask::~CombinedCameraTask()
{
}

void CombinedCameraTask::runTask()
{
//	Alignment::ptzAlreadyChanged = false;
//	Alignment::alreadyChanged = false;
	combinedCamera.combine_align(privateLdPixels, privateHdImage, privateImage_width, privateImage_height, privateX, privateY, privateWidth, privateHeight);
}

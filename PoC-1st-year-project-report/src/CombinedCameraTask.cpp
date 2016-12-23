#include "CombinedCameraTask.h"



CombinedCameraTask::CombinedCameraTask(const std::string& name, ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height, ofPixels& outputPixels): Poco::Task(name), privateLdPixels(ldPixels), privateHdImage(hdImage), privateImage_width(image_width), privateImage_height(image_height), privateX(x), privateY(y), privateWidth(width), privateHeight(height)
{
	privateOutputPixels = &outputPixels;
}


CombinedCameraTask::~CombinedCameraTask()
{
}

void CombinedCameraTask::runTask()
{
	CombinedCamera::combine(privateLdPixels, privateHdImage, privateImage_width, privateImage_height, privateX, privateY, privateWidth, privateHeight, privateOutputPixels);
}

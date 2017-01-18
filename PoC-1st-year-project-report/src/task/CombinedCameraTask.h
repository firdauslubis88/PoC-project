#pragma once
#include "CombinedCamera.h"
#include "Poco/Task.h"

class CombinedCameraTask : public Poco::Task
{
public:
	CombinedCameraTask(const std::string& name, ofPixels ldPixels, ofImage hdImage, int image_width, int image_height, int x, int y, int width, int height);
	~CombinedCameraTask();
	void runTask();

private:
	ofPixels privateLdPixels;
	ofImage privateHdImage;
	int privateImage_width;
	int privateImage_height;
	int privateX;
	int privateY;
	int privateWidth;
	int privateHeight;
};


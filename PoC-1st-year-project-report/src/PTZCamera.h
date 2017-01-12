#pragma once
#include "BasePTZCamera.h"
//#ifdef OF_VIDEO_CAPTURE_DIRECTSHOW
#include "LogitechPTZCamera.h"
#define PTZ_CAMERA_TYPE LogitechPTZCamera
//#endif

class PTZCamera: public BasePTZCamera
{
public:
	PTZCamera();
	~PTZCamera();

	bool setup(int w, int h);

	int SetPanning(int input);
	int GetPanning();
	int SetTilting(int input);
	int GetTilting();
	int SetZooming(int zoom);
	long GetZooming();

	void update();
	void draw(int x, int y, int width, int height);
	ofPixels& getPixels();
	void setDeviceID(int deviceId);
	void videoSettings();
	shared_ptr<BasePTZCamera> getPTZ();

private:
	shared_ptr<BasePTZCamera> ptz;
};


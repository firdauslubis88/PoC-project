#pragma once
#include "BasePTZCamera.h"
//#ifdef OF_VIDEO_CAPTURE_DIRECTSHOW
#include "LogitechPTZCamera.h"
// #include "ViewrunPTZCamera.h"
#define PTZ_CAMERA_TYPE LogitechPTZCamera
//#endif

class PTZCamera: public BasePTZCamera
{
public:
	PTZCamera();
	~PTZCamera();

	bool setup(int w, int h);
	void update();

	int SetPanning();
	int GetPanning();
	int SetTilting();
	int GetTilting();
	int SetZooming();
	long GetZooming();
	int getPanAngle();
	void setPanAngle(int privatePanAngle);
	int getTiltAngle();
	void setTiltAngle(int privateTiltAngle);
	int getZoom();
	void setZoom(int privateZoom);
	int getPtzPanOffset();
	void setPtzPanOffset(int privatePtzTiltOffset);
	int getPtzTiltOffset();
	void setPtzTiltOffset(int privatePtzTiltOffset);
	int getPtzPanScale();
	void setPtzPanScale(int privatePtzTiltOffset);
	int getPtzTiltScale();
	void setPtzTiltScale(int privatePtzTiltOffset);
	int getCameraPanDragThres();
	void setCameraPanDragThres();
	int getCameraTiltDragThres();
	void setCameraTiltDragThres();

	void draw(int x, int y, int width, int height);
	ofPixels& getPixels();
	void setDeviceID(int deviceId);
	void videoSettings();
	shared_ptr<BasePTZCamera> getPTZ();

private:
	shared_ptr<BasePTZCamera> ptz;
};


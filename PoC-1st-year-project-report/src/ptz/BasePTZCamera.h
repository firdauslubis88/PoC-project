#pragma once
#include "ofMain.h"

class BasePTZCamera : public ofVideoGrabber
{
public:
	virtual int SetPanning() = 0;
	virtual int GetPanning() = 0;
	virtual int SetTilting() = 0;
	virtual int GetTilting() = 0;
	virtual int SetZooming() = 0;
	virtual long GetZooming() = 0;

	virtual int getPanAngle();
	virtual void setPanAngle(int privatePanAngle);
	virtual int getTiltAngle();
	virtual void setTiltAngle(int privateTiltAngle);
	virtual int getZoom();
	virtual void setZoom(int privateZoom);
	virtual int getPtzPanOffset();
	virtual void setPtzPanOffset(int privatePtzTiltOffset);
	virtual int getPtzTiltOffset();
	virtual void setPtzTiltOffset(int privatePtzTiltOffset);
	virtual int getPtzPanScale();
	virtual void setPtzPanScale(int privatePtzTiltOffset);
	virtual int getPtzTiltScale();
	virtual void setPtzTiltScale(int privatePtzTiltOffset);

	int cameraPanDragThres, cameraTiltDragThres;

protected:
	enum CameraControlFeature
	{
		KSPROPERTY_CAMERACONTROL_PAN,
		KSPROPERTY_CAMERACONTROL_TILT,
		KSPROPERTY_CAMERACONTROL_ROLL,
		KSPROPERTY_CAMERACONTROL_ZOOM,
		KSPROPERTY_CAMERACONTROL_EXPOSURE,
		KSPROPERTY_CAMERACONTROL_IRIS,
		KSPROPERTY_CAMERACONTROL_FOCUS,
		KSPROPERTY_CAMERACONTROL_SCANMODE,
		KSPROPERTY_CAMERACONTROL_PRIVACY,
		KSPROPERTY_CAMERACONTROL_PANTILT,
		KSPROPERTY_CAMERACONTROL_PAN_RELATIVE,
		KSPROPERTY_CAMERACONTROL_TILT_RELATIVE,
		KSPROPERTY_CAMERACONTROL_ROLL_RELATIVE,
		KSPROPERTY_CAMERACONTROL_ZOOM_RELATIVE,
		KSPROPERTY_CAMERACONTROL_EXPOSURE_RELATIVE,
		KSPROPERTY_CAMERACONTROL_IRIS_RELATIVE,
		KSPROPERTY_CAMERACONTROL_FOCUS_RELATIVE,
		KSPROPERTY_CAMERACONTROL_PANTILT_RELATIVE,
		KSPROPERTY_CAMERACONTROL_FOCAL_LENGTH,
		KSPROPERTY_CAMERACONTROL_AUTO_EXPOSURE_PRIORITY
	};

	enum PTZPanDirection
	{
		PANSTOP = 0,
		PANRIGHT = 1,
		PANLEFT = -1
	};

	enum PTZTiltDirection
	{
		TILTSTOP = 0,
		TILTUP = 1,
		TILTDOWN = -1
	};

	int panAngle, tiltAngle, zoom, ptzPanOffset, ptzTiltOffset, ptzPanScale, ptzTiltScale, panSend, tiltSend;
};


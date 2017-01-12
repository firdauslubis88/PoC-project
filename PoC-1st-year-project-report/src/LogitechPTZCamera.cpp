#include "LogitechPTZCamera.h"



LogitechPTZCamera::LogitechPTZCamera()
{
}


LogitechPTZCamera::~LogitechPTZCamera()
{
}

/*This implementation PTZ camera class (logitech) only support relative panning.
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
int LogitechPTZCamera::SetPanning(int input)
{
	if (input > 0)
	{
		this->panDirection = PANRIGHT;
	}
	else if (input < 0)
	{
		this->panDirection = PANLEFT;
	}
	else
	{
		this->panDirection = PANSTOP;
	}
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_PAN_RELATIVE, this->panDirection, 0);
	ofSleepMillis(50);
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_PAN_RELATIVE, PANSTOP, 0);
	return 1;
}

//This implementation PTZ camera class (logitech) doesn't support this method
int LogitechPTZCamera::GetPanning()
{
	return -1;
}

/*This implementation PTZ camera class (logitech) only support relative tilting. 
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
int LogitechPTZCamera::SetTilting(int input)
{
	if (input > 0)
	{
		this->tiltDirection = TILTUP;
	}
	else if(input < 0)
	{
		this->tiltDirection = TILTDOWN;
	}
	else
	{
		this->tiltDirection = TILTSTOP;
	}
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_TILT_RELATIVE, this->tiltDirection, 0);
	ofSleepMillis(50);
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_TILT_RELATIVE, TILTSTOP, 0);
	return 1;
}

//This implementation PTZ camera class (logitech) doesn't support this method
int LogitechPTZCamera::GetTilting()
{
	return -1;
}

/*Set destination zoom value*/
int LogitechPTZCamera::SetZooming(int zoom)
{
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, zoom, 0);
	return 1;
}

/*Get current zoom value*/
long LogitechPTZCamera::GetZooming()
{
	long min, max, SteppingDelta, currentValue, flags, defaultValue;
	getVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, min, max, SteppingDelta, currentValue, flags, defaultValue);
	return currentValue;
}
#include "LogitechPTZCamera.h"



/*This implementation PTZ camera class (logitech) only support relative panning.
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
int LogitechPTZCamera::SetPanning()
{
	if (this->panSend > 0)
	{
		this->panDirection = PANRIGHT;
	}
	else if (this->panSend < 0)
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
	this->panAngle = 0;
	return 0;
}

//This implementation PTZ camera class (logitech) doesn't support this method. It will just send 0
int LogitechPTZCamera::GetPanning()
{
	return this->panDirection;
}

/*This implementation PTZ camera class (logitech) only support relative tilting. 
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
int LogitechPTZCamera::SetTilting()
{
	if (this->tiltSend > 0)
	{
		this->tiltDirection = TILTUP;
	}
	else if(this->tiltSend < 0)
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
	this->tiltAngle = 0;
	return 0;
}

//This implementation PTZ camera class (logitech) doesn't support this method. It will just send 0
int LogitechPTZCamera::GetTilting()
{
	return this->tiltDirection;
}

/*Set destination zoom value*/
int LogitechPTZCamera::SetZooming()
{
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, this->zoom, 0);
	return zoom;
}

/*Get current zoom value*/
long LogitechPTZCamera::GetZooming()
{
	long min, max, SteppingDelta, currentValue, flags, defaultValue;
	getVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, min, max, SteppingDelta, currentValue, flags, defaultValue);
	return currentValue;
}


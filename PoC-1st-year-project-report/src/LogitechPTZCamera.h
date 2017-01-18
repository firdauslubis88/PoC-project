#pragma once
#include "PTZCamera.h"

class LogitechPTZCamera:  public BasePTZCamera
{
public:
	LogitechPTZCamera();

	void update();
	int SetPanning();
	int GetPanning();
	int SetTilting();
	int GetTilting();
	int SetZooming();
	long GetZooming();
	int getZoom();


private:
	PTZPanDirection panDirection;
	PTZTiltDirection tiltDirection;
};


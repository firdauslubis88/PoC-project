#pragma once
#include "PTZCamera.h"

class LogitechPTZCamera:  public BasePTZCamera
{
public:
	int SetPanning();
	int GetPanning();
	int SetTilting();
	int GetTilting();
	int SetZooming();
	long GetZooming();

private:
	PTZPanDirection panDirection;
	PTZTiltDirection tiltDirection;
};


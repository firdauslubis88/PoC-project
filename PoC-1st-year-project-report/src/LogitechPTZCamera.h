#pragma once
#include "BasePTZCamera.h"

class LogitechPTZCamera:  public BasePTZCamera
{
public:
	LogitechPTZCamera();
	~LogitechPTZCamera();

	int SetPanning(int input);
	int GetPanning();
	int SetTilting(int input);
	int GetTilting();
	int SetZooming(int zoom);
	long GetZooming();

private:
	PTZPanDirection panDirection = PANSTOP;
	PTZTiltDirection tiltDirection = TILTSTOP;
};


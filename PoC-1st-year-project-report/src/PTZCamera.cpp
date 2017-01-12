#include "PTZCamera.h"


PTZCamera::PTZCamera()
{
	this->ptz = make_shared<PTZ_CAMERA_TYPE>();
}


PTZCamera::~PTZCamera()
{
}

shared_ptr<BasePTZCamera> PTZCamera::getPTZ()
{
//	if (!this->ptz) {
//		this->ptz = make_shared<PTZ_CAMERA_TYPE>();
//	}
	return this->ptz;

}

void PTZCamera::setDeviceID(int deviceId)
{
	this->ptz->setDeviceID(deviceId);
}

bool PTZCamera::setup(int w, int h)
{
	bool retVal = this->ptz->setup(w, h);
	return retVal;
}

int PTZCamera::SetPanning(int input)
{
	int retVal = this->ptz->SetPanning(input);
	return retVal;
}

int PTZCamera::GetPanning()
{
	int retVal = this->ptz->GetPanning();
	if (retVal == -1)
	{
		ofLog(OF_LOG_VERBOSE, "PTZ Camera doesn't support global pan");
	}
	return retVal;
}

int PTZCamera::SetTilting(int input)
{
	int retVal = this->ptz->SetTilting(input);
	return retVal;
}

int PTZCamera::GetTilting()
{
	int retVal = this->ptz->GetTilting();
	if (retVal == -1)
	{
		ofLog(OF_LOG_VERBOSE, "PTZ Camera doesn't support global tilt");
	}
	return retVal;
}

int PTZCamera::SetZooming(int zoom)
{
	int retVal = this->ptz->SetZooming(zoom);
	return retVal;
}

long PTZCamera::GetZooming()
{
	int retVal = this->ptz->GetZooming();
	if (retVal == -1)
	{
		ofLog(OF_LOG_VERBOSE, "PTZ Camera doesn't support zoom");
	}
	return retVal;
}

void PTZCamera::update()
{
	this->ptz->update();
}

void PTZCamera::draw(int x, int y, int width, int height)
{
	this->ptz->draw(x, y, width, height);
}

ofPixels& PTZCamera::getPixels()
{
	return this->ptz->getPixels();
}

void PTZCamera::videoSettings()
{
	this->ptz->videoSettings();
}
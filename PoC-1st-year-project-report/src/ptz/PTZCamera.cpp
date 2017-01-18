#include "PTZCamera.h"


PTZCamera::PTZCamera()
{
	this->ptz = make_shared<PTZ_CAMERA_TYPE>(PTZ_CAMERA_TYPE());
}


PTZCamera::~PTZCamera()
{
}

shared_ptr<BasePTZCamera> PTZCamera::getPTZ()
{
	if (!this->ptz) {
		this->ptz = make_shared<PTZ_CAMERA_TYPE>(PTZ_CAMERA_TYPE());
	}
	return this->ptz;

}

void PTZCamera::setDeviceID(int deviceId)
{
	this->ptz->setDeviceID(deviceId);
}

bool PTZCamera::setup(int w, int h)
{
	return this->ptz->setup(w, h);
}

void PTZCamera::update()
{
	this->ptz->update();
}

int PTZCamera::SetPanning()
{
	return this->ptz->SetPanning();
}

int PTZCamera::GetPanning()
{
	return this->ptz->GetPanning();
}

int PTZCamera::SetTilting()
{
	return this->ptz->SetTilting();
}

int PTZCamera::GetTilting()
{
	return this->ptz->GetTilting();
}

int PTZCamera::SetZooming()
{
	return this->ptz->SetZooming();
}

long PTZCamera::GetZooming()
{
	return this->ptz->GetZooming();
}

int PTZCamera::getPanAngle()
{
	return this->ptz->getPanAngle();
}

void PTZCamera::setPanAngle(int privatePanAngle)
{
	this->ptz->setPanAngle(privatePanAngle);
}

int PTZCamera::getTiltAngle()
{
	return this->ptz->getTiltAngle();
}

void PTZCamera::setTiltAngle(int privateTiltAngle)
{
	this->ptz->setTiltAngle(privateTiltAngle);
}

int PTZCamera::getZoom()
{
	return this->ptz->getZoom();
}

void PTZCamera::setZoom(int privateZoom)
{
	this->ptz->setZoom(privateZoom);
}

int PTZCamera::getPtzPanOffset()
{
	return this->ptz->getPtzPanOffset();
}

void PTZCamera::setPtzPanOffset(int privatePtzPanOffset)
{
	this->ptz->setPtzPanOffset(privatePtzPanOffset);
}

int PTZCamera::getPtzTiltOffset()
{
	return this->ptz->getPtzTiltOffset();
}

void PTZCamera::setPtzTiltOffset(int privatePtzTiltOffset)
{
	this->ptz->setPtzTiltOffset(privatePtzTiltOffset);
}

int PTZCamera::getPtzPanScale()
{
	return this->ptz->getPtzPanScale();
}

void PTZCamera::setPtzPanScale(int privatePtzPanScale)
{
	this->ptz->setPtzPanScale(privatePtzPanScale);
}

int PTZCamera::getPtzTiltScale()
{
	return this->ptz->getPtzTiltScale();
}

void PTZCamera::setPtzTiltScale(int privatePtzTiltScale)
{
	this->ptz->setPtzTiltScale(privatePtzTiltScale);
}

int PTZCamera::getCameraPanDragThres()
{
	return this->ptz->cameraPanDragThres;
}

void PTZCamera::setCameraPanDragThres()
{
}

int PTZCamera::getCameraTiltDragThres()
{
	return this->ptz->cameraTiltDragThres;
}

void PTZCamera::setCameraTiltDragThres()
{
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
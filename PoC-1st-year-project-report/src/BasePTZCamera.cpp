#include "BasePTZCamera.h"

BasePTZCamera::BasePTZCamera()
{
	this->panAngle = 0;
	this->tiltAngle = 0;
	this->ptzPanOffset = 0;
	this->ptzTiltOffset = 0;
	this->ptzPanScale = 1;
	this->ptzTiltScale = 1;
}

void BasePTZCamera::update()
{
	ofVideoGrabber::update();
	this->panSend = this->ptzPanScale*(this->panAngle + this->ptzPanOffset);
	this->tiltSend = this->ptzTiltScale*(this->tiltAngle + this->ptzTiltOffset);
	if (this->panSend > 180)
	{
		this->panSend -= 360;
	}
	else if (this->panSend < -180)
	{
		this->panSend += 360;
	}
	if (this->tiltSend > 180)
	{
		this->tiltSend -= 360;
	}
	else if (this->tiltSend < -180)
	{
		this->tiltSend += 360;
	}
}


int BasePTZCamera::getPanAngle()
{
	return this->panAngle;
}

void BasePTZCamera::setPanAngle(int privatePanAngle)
{
	this->panAngle = privatePanAngle;
}

int BasePTZCamera::getTiltAngle()
{
	return this->tiltAngle;
}

void BasePTZCamera::setTiltAngle(int privateTiltAngle)
{
	this->tiltAngle = privateTiltAngle;
}

int BasePTZCamera::getZoom()
{
	return this->zoom;
}

void BasePTZCamera::setZoom(int privateZoom)
{
	this->zoom = privateZoom;
}

int BasePTZCamera::getPtzPanOffset()
{
	return this->ptzPanOffset;
}

void BasePTZCamera::setPtzPanOffset(int privatePtzPanOffset)
{
	this->ptzPanOffset = privatePtzPanOffset;
}

int BasePTZCamera::getPtzTiltOffset()
{
	return this->ptzTiltOffset;
}

void BasePTZCamera::setPtzTiltOffset(int privatePtzTiltOffset)
{
	this->ptzTiltOffset = privatePtzTiltOffset;
}

int BasePTZCamera::getPtzPanScale()
{
	return this->ptzPanScale;
}

void BasePTZCamera::setPtzPanScale(int privatePtzPanScale)
{
	this->ptzPanScale = privatePtzPanScale;
}

int BasePTZCamera::getPtzTiltScale()
{
	return this->ptzTiltOffset;
}

void BasePTZCamera::setPtzTiltScale(int privatePtzTiltScale)
{
	this->ptzTiltScale = privatePtzTiltScale;
}
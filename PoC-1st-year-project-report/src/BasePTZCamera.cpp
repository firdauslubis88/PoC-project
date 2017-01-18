#include "BasePTZCamera.h"


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
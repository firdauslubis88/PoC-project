#pragma once
#include "ofApp.h"

void ofApp::printScreen()
{
	if (isHdCameraConnected)
	{
		stringstream hdImageName;
		ofImage hdImage;
		hdImage.setFromPixels(hdPixels);
		hdImage.setImageType(OF_IMAGE_COLOR);
		hdImageName << "PTZ_" << printScreenCount << ".jpg";
		hdImage.save(hdImageName.str());
	}
	if (isldCameraConnected)
	{
		stringstream ldImageName;
		ofImage ldImage;
		ldImage.setFromPixels(ldPixels);
		ldImage.setImageType(OF_IMAGE_COLOR);
		ldImageName << "360_" << printScreenCount << ".jpg";
		ldImage.save(ldImageName.str());
	}
	printScreenCount++;
}

void ofApp::restart()
{
	_easyCam.reset();
	_easyCam.rotate(-90, 0, 0, 1);

	hdVideoGrabber.setPanAngle(0);
	hdVideoGrabber.setTiltAngle(0);

	queue.start(new PTZCameraTask("UPDATE PTZ", hdVideoGrabber.getPTZ()));
}

void ofApp::onToggle(const void * sender)
{
	ofxButton * p = (ofxButton *)sender;
	cameraSelected = p->getName();
	if (cameraSelected == "PTZ Camera")
	{
		ptzCameraCommand("UPDATE PTZ");
	}
	if (cameraSelected == "Combined Camera")
	{
#ifdef USE_PTZ_ADJUSTMENT
		Alignment::ptzAlreadyChanged = false;
#endif
		ptzCameraCommand("UPDATE PTZ THEN COMBINE");
	}
}
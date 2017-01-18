#pragma once
#include "ofApp.h"

void ofApp::printScreen()
{
	hdImage.save("PTZ.jpg");
	ofImage ldImage;
	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);
	ldImage.save("360.jpg");
}

void ofApp::restart()
{
	_easyCam.reset();
	_easyCam.rotate(-90, 0, 0, 1);

	hdVideoGrabber.setPanAngle(0);
	hdVideoGrabber.setTiltAngle(0);

	queue.start(new PTZCameraTask("UPDATE PT", hdVideoGrabber.getPTZ()));
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
		ptzCameraCommand("UPDATE PT THEN COMBINE");
	}
}
#pragma once
#include "ofApp.h"

void ofApp::ptzCameraCommand(PTZSIGN sign, int value, string taskName)
{
	if (allowUpdatePTZ)
	{
		if (sign == PAN)
		{
			hdVideoGrabber.setPanAngle(hdVideoGrabber.getPanAngle() + value);
		}
		else if (sign == TILT)
		{
			hdVideoGrabber.setTiltAngle(hdVideoGrabber.getTiltAngle() + value);
		}
		else
		{
			long currentValue = hdVideoGrabber.getZoom();
			long nextZoom = value * 10 + currentValue;
			hdVideoGrabber.setZoom(nextZoom);
		}
		ptzCameraCommand(taskName);
	}
}

void ofApp::ptzCameraCommand(string taskName)
{
	if (allowUpdatePTZ)
	{
		allowUpdatePTZ = false;
		queue.start(new PTZCameraTask(taskName, hdVideoGrabber.getPTZ()));
	}
}


void ofApp::onProperty()
{
	if (cameraSelected == "PTZ Camera")
	{
		hdVideoGrabber.videoSettings();
	}
}

// ref at http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
ofVec3f ofApp::getPTZEuler() const {
	ofQuaternion myQuaternion = _easyCam.getOrientationQuat();
	double theta3, theta2, theta1;
	double e = -1;
	double p0 = myQuaternion.w(), p1 = myQuaternion.z(), p2 = myQuaternion.x(), p3 = myQuaternion.y();
	double test = e*p3*p1 + p2*p0;
	//	cout << test << endl;
	if (test > 0.499999) { // singularity at north pole
		theta1 = 2 * atan2(p1, p0);
		theta2 = PI / 2;
		theta3 = 0;
	}
	else if (test < -0.499999) { // singularity at south pole
		theta1 = -2 * atan2(p1, p0);
		theta2 = -PI / 2;
		theta3 = 0;
	}
	else {
		double sqp3 = p3 * p3;
		double sqp1 = p1 * p1;
		double sqp2 = p2 * p2;
		theta1 = atan2(2.0f * p1 * p0 - 2.0f * p3 * p2 * e, 1.0f - 2.0f*sqp1 - 2.0f*sqp2);
		theta2 = asin(2 * test);
		theta3 = atan2(2.0f*p3 * p0 - 2.0f * p1 * p2 * e, 1.0f - 2.0f*sqp3 - 2.0f*sqp2);
	}
	float attitude = theta1;
	float heading = theta3;
	float bank = theta2;
	return ofVec3f(ofRadToDeg(bank), ofRadToDeg(heading), ofRadToDeg(attitude));
}
#include "ofApp_calibration.h"

void ofApp_calibration::setup()
{
	ofDisableArbTex();
	VIDEO_WIDTH = 640;
	VIDEO_HEIGHT = 480;
	bLdCameraShow = false;
	bHdCameraShow = false;
	
	listVideoDevice = ldVideoGrabber.listDevices();
	for (size_t i = 0; i < listVideoDevice.size(); i++)
	{
		ofVideoDevice device = listVideoDevice[i];
		if (device.deviceName == "RICOH THETA S")
		{
			isldCameraConnected = true;
		}
		if (device.deviceName == "THETA UVC Blender")
		{
			ldVideoGrabber.setDeviceID(device.id);
			ldVideoGrabber.setup(VIDEO_WIDTH, VIDEO_HEIGHT);
			ldFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			ldPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
			sphereVboMesh = ofSpherePrimitive(2000, 24).getMesh();
			for (int i = 0; i<sphereVboMesh.getNumTexCoords(); i++) {
				sphereVboMesh.setTexCoord(i, ofVec2f(1.0) - sphereVboMesh.getTexCoord(i));
			}
			for (int i = 0; i<sphereVboMesh.getNumNormals(); i++) {
				sphereVboMesh.setNormal(i, sphereVboMesh.getNormal(i) * ofVec3f(-1));
			}
			_easyCam.setAutoDistance(false);
			_easyCam.setDistance(0);
			_easyCam.rotate(-90, 0, 0, 1);
		}
		if (device.deviceName == "PTZ Pro Camera")
		{
			isHdCameraConnected = true;
			hdVideoGrabber.setDeviceID(device.id);
			hdVideoGrabber.setup(VIDEO_WIDTH, VIDEO_HEIGHT);
			hdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			hdPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		}
	}
	if (!isldCameraConnected && !isHdCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO CAMERA FOUND!!");
		ofExit();
	}
	if(!isldCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO 360 CAMERA FOUND!!");
	}
	if (!isHdCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO PTZ CAMERA FOUND!!");
	}

	/*********************************************************CALIBRATION PART BEGIN****************************************************************/
	this->cameraNum = 1;
	this->additionalViewNum = 0;
	imagePixels = new ofPixels[this->cameraNum];
	calibration.init(VIDEO_WIDTH, VIDEO_HEIGHT, 9, 6, 0.0262, bLiveVideo, this->cameraNum, this->additionalViewNum);
	/*********************************************************CALIBRATION PART END*****************************************************************/

	_panel.setup();
	calibrationToggle.setup("Single Calibration", false);
	calibrationToggle.addListener(this, &ofApp_calibration::onToggle);
	_panel.add(&calibrationToggle);
	stereoCalibrationToggle.setup("Stereo Calibration", false);
	stereoCalibrationToggle.addListener(this, &ofApp_calibration::onToggle);
	_panel.add(&stereoCalibrationToggle);
}

void ofApp_calibration::exit()
{
	delete[] imagePixels;
}

void ofApp_calibration::update()
{
	if (isldCameraConnected)
	{
		ldVideoGrabber.update();
	}
	if (isHdCameraConnected)
	{
		hdVideoGrabber.update();
	}
}

void ofApp_calibration::draw()
{
	if (isldCameraConnected)
	{
		ldFbo.begin();
		_easyCam.begin();
		ofClear(0);
		ldVideoGrabber.getTextureReference().bind();
		sphereVboMesh.draw();
		ldVideoGrabber.getTextureReference().unbind();
		_easyCam.end();
		ldFbo.end();
		ldFbo.readToPixels(ldPixels);
		if (bLdCameraShow)
		{
			ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		}
	}
	if (isHdCameraConnected)
	{
		hdFbo.begin();
		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
		hdFbo.end();
		hdFbo.readToPixels(hdPixels);
		hdImage.setFromPixels(hdPixels);
		hdImage.setImageType(OF_IMAGE_COLOR);

		if (bHdCameraShow)
		{
			hdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		}
	}
	/*********************************************************CALIBRATION PART BEGIN****************************************************************/
//	imagePixels[0] = ldPixels;
	imagePixels[0] = hdPixels;
	calibration.main(imagePixels);

	for (size_t i = 0; i < this->cameraNum; i++)
	{
		calibration.calibrationView[i].draw(0, i*VIDEO_HEIGHT);
	}

	if (calibration.additionalViewNum > 0)
	{
		if (calibration.mode == CALIBRATED)
		{
			calibration.calibrationView[2].draw(640, 0);
		}
	}
	/*********************************************************CALIBRATION PART END*****************************************************************/

	_panel.draw();
}

void ofApp_calibration::keyPressed(int key)
{
	switch (key)
	{
	case 'g':
		calibration.bStartCapture = true;
		break;
	case 'u':
		calibration.undistortImage = !calibration.undistortImage;
		break;
	/*
	case 't':
		calibration.trackImage = !calibration.trackImage;
		break;
	*/
	default:
		break;
	}
}

void ofApp_calibration::keyReleased(int key)
{
}

void ofApp_calibration::mouseMoved(int x, int y)
{
}

void ofApp_calibration::mouseDragged(int x, int y, int button)
{
}

void ofApp_calibration::mousePressed(int x, int y, int button)
{
}

void ofApp_calibration::mouseReleased(int x, int y, int button)
{
}

void ofApp_calibration::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
}

void ofApp_calibration::mouseEntered(int x, int y)
{
}

void ofApp_calibration::mouseExited(int x, int y)
{
}

void ofApp_calibration::windowResized(int w, int h)
{
}

void ofApp_calibration::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp_calibration::gotMessage(ofMessage msg)
{
}

void ofApp_calibration::onToggle(const void * sender)
{
	ofxButton * p = (ofxButton *)sender;
	cameraSelected = p->getName();
	int WINDOW_WIDTH, WINDOW_HEIGHT;
	if (cameraSelected == "Single Calibration")
	{
		WINDOW_WIDTH = 640;
		WINDOW_HEIGHT = 480;
	}
	if (cameraSelected == "Stereo Calibration")
	{
		WINDOW_WIDTH = 1280;
		WINDOW_HEIGHT = 960;
	}
	ofSetWindowShape(WINDOW_WIDTH, WINDOW_HEIGHT);
}

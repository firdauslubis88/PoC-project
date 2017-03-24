#include "ofApp_test.h"

void ofApp_test::setup()
{
	ofDisableArbTex();
	VIDEO_WIDTH = ofGetWidth();
	VIDEO_HEIGHT = ofGetHeight();
	LD_VIDEO_WIDTH = 1280;
	LD_VIDEO_HEIGHT = 720;
	HD_VIDEO_WIDTH = 640;
	HD_VIDEO_HEIGHT = 480;

	bLdCameraShow = false;
	bHdCameraShow = false;

	_panel.setup();
	listVideoDevice = ldVideoGrabber.listDevices();
	for (size_t i = 0; i < listVideoDevice.size(); i++)
	{
		ofVideoDevice device = listVideoDevice[i];
		cout << device.deviceName << endl;
		if (device.deviceName == "RICOH THETA S")
		{
			isldCameraConnected = true;
		}
		if (device.deviceName == "RICOH THETA S")
		{
			ldVideoGrabber.setDeviceID(device.id);
			ldVideoGrabber.setup(LD_VIDEO_WIDTH, LD_VIDEO_HEIGHT);
			_shader.load("shaders/equirectanguler");
			tempLdImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
			ldFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			tempLdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
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
			offsetParameter.set("uvOffset", ofVec4f(0, 0.0, 0, 0.0), ofVec4f(-0.1), ofVec4f(0.1));
			radiusParameter.set("radius", 0.445, 0.0, 1.0);
			ldParameterGroup.add(offsetParameter);
			ldParameterGroup.add(radiusParameter);
			ldToggle.setup("360 Camera", false);
			ldToggle.addListener(this, &ofApp_test::onToggle);
			_panel.add(&ldToggle);
		}
		if (device.deviceName == "PTZ Pro Camera")
		{
			isHdCameraConnected = true;
			hdVideoGrabber.setDeviceID(device.id);
//			hdVideoGrabber.setPixelFormat(OF_PIXELS_YUY2);
//			hdVideoGrabber.setDesiredFrameRate(5);
			hdVideoGrabber.setup(HD_VIDEO_WIDTH, HD_VIDEO_HEIGHT);
			hdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			hdPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
			hdToggle.setup("PTZ Camera", false);
			hdToggle.addListener(this, &ofApp_test::onToggle);
			_panel.add(&hdToggle);
		}

	}
//	ofSleepMillis(10000);
	if (!isldCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO 360 CAMERA FOUND!!");
		ofExit();
	}
	if (!isHdCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO PTZ CAMERA FOUND!!");
		ofExit();
	}
	/*
	soundStream.printDeviceList();
	int bufferSize = 256;
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	soundStream.setup(this, 2, 2, 44100, bufferSize, 4);
	*/
}

void ofApp_test::exit()
{
}

void ofApp_test::update()
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

void ofApp_test::draw()
{
	if (isldCameraConnected)
	{
		tempLdFbo.begin();
//		ldVideoGrabber.getTexture().drawSubsection(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, 0, 0, VIDEO_WIDTH , VIDEO_HEIGHT);
		ldVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
//		ofSetColor(0, 0, 0);
//		ofDrawRectangle(VIDEO_WIDTH / 2, 0, VIDEO_WIDTH / 2, VIDEO_HEIGHT);
		tempLdFbo.end();
//		tempLdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		/*
		_easyCam.begin();
		ofClear(0);
		ldVideoGrabber.getTextureReference().bind();
		sphereVboMesh.draw();
		ldVideoGrabber.getTextureReference().unbind();
		_easyCam.end();
		*/
//		ldVideoGrabber.draw(0, 0);
		/*
		ldFbo.begin();
		_easyCam.begin();
		ofClear(0);
		_shader.begin();
//		tempLdImage.setFromPixels(ldVideoGrabber.getPixels());
		_shader.setUniformTexture("mainTex", tempLdFbo.getTexture(), 0);
		_shader.setUniforms(ldParameterGroup);
		sphereVboMesh.draw();
		_shader.end();
		_easyCam.end();
		ldFbo.end();
		ldFbo.readToPixels(ldPixels);
		if (bLdCameraShow)
		{
			ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		}
		*/
	}
	if (isHdCameraConnected)
	{
//		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
		hdFbo.begin();
//		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
		hdVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.end();
		hdFbo.readToPixels(hdPixels);
	}
	if (cameraSelected == "360 Camera")
	{
		tempLdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else if (cameraSelected == "PTZ Camera")
	{
		hdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	_panel.draw();

}

void ofApp_test::keyPressed(int key)
{
}

void ofApp_test::keyReleased(int key)
{
}

void ofApp_test::mouseMoved(int x, int y)
{
}

void ofApp_test::mouseDragged(int x, int y, int button)
{
}

void ofApp_test::mousePressed(int x, int y, int button)
{
}

void ofApp_test::mouseReleased(int x, int y, int button)
{
}

void ofApp_test::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
}

void ofApp_test::mouseEntered(int x, int y)
{
}

void ofApp_test::mouseExited(int x, int y)
{
}

void ofApp_test::windowResized(int w, int h)
{
}

void ofApp_test::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp_test::gotMessage(ofMessage msg)
{
}

void ofApp_test::onToggle(const void * sender)
{
	ofxButton * p = (ofxButton *)sender;
	cameraSelected = p->getName();
}

/*
void ofApp_test::audioIn(float * input, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) {
		left[i] = input[i * 2] * 0.5;
		right[i] = input[i * 2 + 1] * 0.5;
	}
}
*/

/*
//--------------------------------------------------------------
void ofApp_test::audioOut(float * output, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) {
		output[i*nChannels] = left[i];
		output[i*nChannels + 1] = right[i];
	}
}
*/

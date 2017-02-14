#include "ofApp_test.h"

void ofApp_test::setup()
{
	ofDisableArbTex();
	VIDEO_WIDTH = ofGetWidth();
	VIDEO_HEIGHT = ofGetHeight();

	bLdCameraShow = true;
	bHdCameraShow = false;

	listVideoDevice = ldVideoGrabber.listDevices();
	for (size_t i = 0; i < listVideoDevice.size(); i++)
	{
		ofVideoDevice device = listVideoDevice[i];
		if (device.deviceName == "RICOH THETA S")
		{
			isldCameraConnected = true;
		}
		if (device.deviceName == "RICOH THETA S")
		{
			ldVideoGrabber.setDeviceID(device.id);
			ldVideoGrabber.setup(VIDEO_WIDTH, VIDEO_HEIGHT);
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
		}
	}
	if (!isldCameraConnected)
	{
		ofLog(OF_LOG_ERROR, "NO CAMERA FOUND!!");
		ofExit();
	}
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
}

void ofApp_test::draw()
{
	if (isldCameraConnected)
	{
		tempLdFbo.begin();
		ldVideoGrabber.getTexture().drawSubsection(0, 0, VIDEO_WIDTH/2, VIDEO_HEIGHT, 0, 0, VIDEO_WIDTH / 2, VIDEO_HEIGHT);
		ofSetColor(0, 0, 0);
		ofDrawRectangle(VIDEO_WIDTH / 2, 0, VIDEO_WIDTH / 2, VIDEO_HEIGHT);
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
	}
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
}

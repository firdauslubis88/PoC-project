#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenVR.h"

#include "CombinedCamera.h"

#include "PTZControl.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void exit();

	void update();
	void draw();

	void render(vr::Hmd_Eye nEye);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int x, int y, float scrollX, float scrollY);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void onToggle(const void* sender);
	void onProperty();
	void restart();

	ofVec3f getPTZEuler() const;

	ofxOpenVR openVR;

	bool bShowHelp;
	std::ostringstream _strHelp;

	ofFbo ldFbo;
	ofFbo hdFbo;
	ofVboMesh sphereVboMesh;
	ofEasyCam _easyCam;
	ofImage ldImage, hdImage, combinedImage, ldPassImage;
	vector<ofVideoDevice> listVideoDevice;
	ofShader _shader;
	ofVideoGrabber ldVideoGrabber;
	PTZControl hdVideoGrabber;
	bool isldCameraConnected;
	bool isHdCameraConnected;
	bool isCombined;

	ofShader shader;
	ofSpherePrimitive sphere;

	ofxPanel _panel;
	ofParameter<ofVec4f> offsetParameter;
	ofParameter<float> radiusParameter;
	ofParameterGroup ldParameterGroup;
	ofxButton ldToggle;
	ofxButton hdToggle;
	ofxButton combinedToggle;

	int VIDEO_WIDTH = 1280, VIDEO_HEIGHT = 640;
	string cameraSelected;

	CombinedCamera combinedCamera = CombinedCamera(VIDEO_WIDTH,VIDEO_HEIGHT);
	ofPixels ldPixels;

//	PTZControl ptzControl;

	int panAngle = 0, tiltAngle = 0, prevPanAngle = 0, prevTiltAngle = 0;
	int prevXDrag;
	int prevYDrag;
	int ptzPanOffset;
	bool followingMode = false;
	bool singularMode = false;
};
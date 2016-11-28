#pragma once

#include "ofMain.h"
#include "ofxOpenVR.h"

#include "CombinedCamera.h"

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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxOpenVR openVR;

	bool bShowHelp;
	std::ostringstream _strHelp;

	ofFbo ldFbo;
	ofFbo hdFbo;
	CombinedCamera combinedCamera;
	ofVboMesh sphereVboMesh;
	ofEasyCam _easyCam;
	vector<ofVideoDevice> listVideoDevice;
	ofVideoGrabber ldVideoGrabber;
	bool isldCameraConnected;

	ofShader shader;
	ofSpherePrimitive sphere;

	ofxPanel _panel;
	ofParameter<ofVec4f> offsetParameter;
	ofParameter<float> radiusParameter;
	ofParameterGroup ldParameterGroup;
	ofxButton ldToggle;
	ofxButton hdToggle;
	ofxButton combinedToggle;

	int VIDEO_WIDTH, VIDEO_HEIGHT;
	string cameraSelected;

	ofPixels ldPixels, hdPixels, combinedPixels;
};

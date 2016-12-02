#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenVR.h"

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

	void onToggle(const void* sender);

	ofxOpenVR openVR;

	bool bShowHelp;
	std::ostringstream _strHelp;

	ofFbo ldFbo;
	ofFbo hdFbo;
	ofVboMesh sphereVboMesh;
	ofEasyCam _easyCam;
	ofImage image;
	vector<ofVideoDevice> listVideoDevice;
	ofShader _shader;
	ofVideoGrabber ldVideoGrabber;
	ofVideoGrabber hdVideoGrabber;
	bool isldCameraConnected;
	bool isHdCameraConnected;

	ofShader shader;
	ofSpherePrimitive sphere;

	ofxPanel _panel;
	ofParameter<ofVec4f> offsetParameter;
	ofParameter<float> radiusParameter;
	ofParameterGroup ldParameterGroup;
	ofxButton ldToggle;
	ofxButton hdToggle;

	int VIDEO_WIDTH, VIDEO_HEIGHT;
	string cameraSelected;
};

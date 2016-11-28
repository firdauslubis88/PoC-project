#pragma once

#include "ofMain.h"
#include "ofxOpenVR.h"
#include "ofApp.h"
#include "CombinedCamera.h"


class ofAppVR : public ofBaseApp {

public:
	void setup();
	void exit();

	void update();
	void draw();

	void render(vr::Hmd_Eye nEye);

	void prerender(vr::Hmd_Eye nEye);

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

	shared_ptr<ofApp> mainApp;

	bool bShowHelp;
	std::ostringstream _strHelp;

	ofImage ldImage, hdImage;
	vector<ofVideoDevice> listVideoDevice;
	ofVideoGrabber ldVideoGrabber;
	bool isldCameraConnected;

	ofShader shader;
	ofSpherePrimitive sphere;

	ofFbo hmdFbo;
	ofImage rightImage, leftImage;
};

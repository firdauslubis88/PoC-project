#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "PTZCamera.h"
#include "Calibration.h"

class ofApp_calibration: public ofBaseApp
{
public:
	void setup();
	void exit();
	void update();
	void draw();

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

	//CALIBRATION variables and functions
	Calibration calibration = Calibration();

	bool bShowHelp, bHdCameraShow, bLdCameraShow;
	std::ostringstream _strHelp;
	int VIDEO_WIDTH, VIDEO_HEIGHT;
	ofFbo ldFbo;
	ofFbo hdFbo;
	ofVboMesh sphereVboMesh;
	ofEasyCam _easyCam;
	ofImage ldImage, hdImage, combinedImage;
	vector<ofVideoDevice> listVideoDevice;
	ofShader _shader;
	ofVideoGrabber ldVideoGrabber;
	PTZCamera hdVideoGrabber;
	bool isldCameraConnected;
	bool isHdCameraConnected;
	bool isCombined;
	ofShader shader;
	ofSpherePrimitive sphere;
	ofxPanel _panel;
	ofxButton ldToggle;
	ofxButton hdToggle;
	ofxButton combinedToggle;
	string cameraSelected;
	ofPixels ldPixels;
	ofPixels hdPixels;
	int prevXDrag;
	int prevYDrag;
	bool combinedCameraFinished;
	bool showROI;
	bool combinedMode;
	bool allowUpdatePTZ;
};


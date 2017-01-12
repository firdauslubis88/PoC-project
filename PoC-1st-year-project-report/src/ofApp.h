#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenVR.h"
#include "ofxTaskQueue.h"
#include "ofxVideoRecorder.h"

#include "PTZControl.h"
#include "CombinedCamera.h"
#include "Cloning.h"

#include "PTZMotorControl.h"
#include "CombinedCameraTask.h"
#include "SimpleTaskProgress.h"

#include "PTZCamera.h"
#include "PTZCameraTask.h"

//#define USE_VIDEO_RECORDER

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
	void restart();
	void onProperty();
	ofVec3f getPTZEuler() const;
	void start_record();
	void stop_record();
	void printScreen();

	void onTaskQueued(const ofx::TaskQueueEventArgs & args);
	void onTaskStarted(const ofx::TaskQueueEventArgs & args);
	void onTaskCancelled(const ofx::TaskQueueEventArgs & args);
	void onTaskFinished(const ofx::TaskQueueEventArgs & args);
	void onTaskFailed(const ofx::TaskFailedEventArgs & args);
	void onTaskProgress(const ofx::TaskProgressEventArgs & args);
#ifdef USE_VIDEO_RECORDER
	void audioIn(float * input, int bufferSize, int nChannels);
#endif // USE_VIDEO_RECORDER


	ofxOpenVR openVR;

	bool bShowHelp;
	std::ostringstream _strHelp;

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
//	ofParameter<ofVec4f> offsetParameter;
//	ofParameter<float> radiusParameter;
//	ofParameterGroup ldParameterGroup;
	ofxButton ldToggle;
	ofxButton hdToggle;
	ofxButton combinedToggle;

	int VIDEO_WIDTH = ofGetWidth(), VIDEO_HEIGHT = ofGetHeight();
	string cameraSelected;

	CombinedCamera combinedCamera;
	ofPixels ldPixels;

	int panAngle, tiltAngle;
	int prevXDrag;
	int prevYDrag;
	int ptzPanOffset = 0, ptzTiltOffset = 0;

	ofx::TaskQueue queue, combinedCameraQueue;
	typedef std::map<std::string, SimpleTaskProgress> TaskProgress;
	// We keep a simple task progress queue.
	TaskProgress taskProgress;
#ifdef USE_VIDEO_RECORDER
	ofxVideoRecorder    vidRecorder;
	bool bRecording;
	string fileName;
	string fileExt;
	int sampleRate;
	int channels;
	ofSoundStream       soundStream;
#endif // USE_VIDEO_RECORDER

	volatile int panSend, tiltSend;
	bool combinedCameraFinished;
	bool showROI;
	bool combinedMode;
	float maskXStart;
	float maskYStart;
	int maskWidth;
	int maskHeight;
	bool allowUpdatePT;
};

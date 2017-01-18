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

	enum PTZSIGN {
		PAN,
		TILT,
		ZOOM
	};

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

	//UTIL functions
	void onToggle(const void* sender);
	void restart();
	void printScreen();

	//PTZ functions
	void onProperty();
	ofVec3f getPTZEuler() const;
	void ptzCameraCommand(PTZSIGN sign, int value, string taskName);
	void ptzCameraCommand(string taskName);

	//THREAD TASK functions
	ofx::TaskQueue queue, combinedCameraQueue;
	typedef std::map<std::string, SimpleTaskProgress> TaskProgress;
	// We keep a simple task progress queue.
	TaskProgress taskProgress;
	void onTaskQueued(const ofx::TaskQueueEventArgs & args);
	void onTaskStarted(const ofx::TaskQueueEventArgs & args);
	void onTaskCancelled(const ofx::TaskQueueEventArgs & args);
	void onTaskFinished(const ofx::TaskQueueEventArgs & args);
	void onTaskFailed(const ofx::TaskFailedEventArgs & args);
	void onTaskProgress(const ofx::TaskProgressEventArgs & args);

	//VIDEO RECORDER functions
	ofxVideoRecorder    vidRecorder;
	bool bRecording;
	string fileName;
	string fileExt;
	int sampleRate;
	int channels;
	ofSoundStream       soundStream;
	bool videoRecorderSetup();
	bool videoRecorderUpdate();
	bool videoRecorderExit();
	bool videoRecorderStartRecord();
	bool videoRecorderStopRecord();
	void audioIn(float * input, int bufferSize, int nChannels);

	//HMD functions
	ofxOpenVR openVR;
	void render(vr::Hmd_Eye nEye);

	bool bShowHelp;
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

	int prevXDrag;
	int prevYDrag;

	bool combinedCameraFinished;
	bool showROI;
	bool combinedMode;
	bool allowUpdatePTZ;

	CombinedCamera combinedCamera;
	float maskXStart;
	float maskYStart;
	int maskWidth;
	int maskHeight;
};

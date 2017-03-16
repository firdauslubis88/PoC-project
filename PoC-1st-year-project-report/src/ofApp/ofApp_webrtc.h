#pragma once
#include "ofMain.h"
#include "Init.h"
#include "ofxOpenCv.h"
#include "opencv2/imgproc.hpp"

class ofApp_webrtc : public ofBaseApp
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
	void onToggle(const void* sender);


	MSG msg;
	BOOL gm;

private:
	int VIDEO_WIDTH, VIDEO_HEIGHT;
	shared_ptr<WebRTC> webRTC;
	ofxCvColorImage tempCvImage;
	ofImage tempImage;
	cv::Mat tempMatDst;

};


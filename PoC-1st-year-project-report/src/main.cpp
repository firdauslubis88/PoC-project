#include "ofMain.h"
#include "ofApp.h"
#include "ofAppVR.h"
#include "ofApp_calibration.h"
#include "ofApp_test.h"
#include "ofApp_webrtc.h"
#include "ofAppGLFWWindow.h"
#include "Init.h"

//========================================================================
int main( ){
	shared_ptr<WebRTC> webRTC(new WebRTC);

	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 1);
	settings.width = 2560;
	settings.height = 1280;
	settings.setPosition(ofVec2f(0, 0));
	shared_ptr<ofAppBaseWindow> dualCameraWindow = ofCreateWindow(settings);
//	settings.setGLVersion(4, 1);
//	settings.width = 1280;
//	settings.height = 720;
//	settings.setPosition(ofVec2f(50, 450));
//	settings.shareContextWith = dualCameraWindow;
//	shared_ptr<ofAppBaseWindow> vrWindow = ofCreateWindow(settings);

//	shared_ptr<ofAppVR> vrApp(new ofAppVR);
//	shared_ptr<ofApp> mainApp(new ofApp);
	shared_ptr<ofApp_webrtc> app_webrtc(new ofApp_webrtc);
	HWND hwnd = dualCameraWindow->getWin32Window();
	//This will disable windows resizing and maximizing:
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX&~WS_SIZEBOX);

//	vrApp->mainApp = mainApp;

	ofRunApp(dualCameraWindow, app_webrtc);
//	ofRunApp(vrWindow, vrApp);
	ofRunMainLoop();
}

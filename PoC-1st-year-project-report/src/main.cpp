#include "ofMain.h"
#include "ofApp.h"
#include "ofAppVR.h"
#include "ofApp_calibration.h"
#include "ofApp_test.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 1);
	settings.width = 1280;
	settings.height = 960;
	settings.setPosition(ofVec2f(0, 0));
	shared_ptr<ofAppBaseWindow> dualCameraWindow = ofCreateWindow(settings);
	HWND hWnd = dualCameraWindow->getWin32Window();
	//This will disable windows resizing and maximizing:
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX&~WS_SIZEBOX);
//	settings.setGLVersion(4, 1);
//	settings.width = 1280;
//	settings.height = 720;
//	settings.setPosition(ofVec2f(50, 450));
//	settings.shareContextWith = dualCameraWindow;
//	shared_ptr<ofAppBaseWindow> vrWindow = ofCreateWindow(settings);

//	shared_ptr<ofAppVR> vrApp(new ofAppVR);
//	shared_ptr<ofApp> mainApp(new ofApp);
	shared_ptr<ofApp_calibration> calibrationApp(new ofApp_calibration);

//	vrApp->mainApp = mainApp;

	ofRunApp(dualCameraWindow, calibrationApp);
//	ofRunApp(vrWindow, vrApp);
	ofRunMainLoop();
}

#include "ofMain.h"
#include "ofApp.h"
#include "ofAppVR.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 1);
	settings.width = 2560;
	settings.height = 1280;
	settings.setPosition(ofVec2f(50, 50));
	shared_ptr<ofAppBaseWindow> dualCameraWindow = ofCreateWindow(settings);

//	settings.setGLVersion(4, 1);
//	settings.width = 1280;
//	settings.height = 720;
//	settings.setPosition(ofVec2f(50, 450));
//	settings.shareContextWith = dualCameraWindow;
//	shared_ptr<ofAppBaseWindow> vrWindow = ofCreateWindow(settings);

//	shared_ptr<ofAppVR> vrApp(new ofAppVR);
	shared_ptr<ofApp> mainApp(new ofApp);
//	vrApp->mainApp = mainApp;

	ofRunApp(dualCameraWindow, mainApp);
//	ofRunApp(vrWindow, vrApp);
	ofRunMainLoop();
}

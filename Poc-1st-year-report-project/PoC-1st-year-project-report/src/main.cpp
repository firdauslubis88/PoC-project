#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(4, 1);
	settings.width = 1280;
	settings.height = 640;
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}

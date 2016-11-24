// Thanks to @num3ric for sharing this:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6

#include "ofAppVR.h"

//--------------------------------------------------------------
void ofAppVR::setup() {
	ofSetVerticalSync(false);
	ofDisableArbTex();

	// We need to pass the method we want ofxOpenVR to call when rending the scene
	openVR.setup(std::bind(&ofAppVR::render, this, std::placeholders::_1));

	image.allocate(1280, 720, OF_IMAGE_COLOR);
	/*
	listVideoDevice = ldVideoGrabber.listDevices();
	isldCameraConnected = false;
	for (int i = 0; i < listVideoDevice.size(); i++) {
		ofLog(OF_LOG_VERBOSE, listVideoDevice[i].deviceName);
		if (listVideoDevice[i].deviceName == "THETA UVC Blender") {
			ldVideoGrabber.setDeviceID(listVideoDevice[i].id);
			isldCameraConnected = true;
		}
	}
	if (isldCameraConnected) {
		ldVideoGrabber.initGrabber(1280, 720);
	}
	else
	{
		ofLog(OF_LOG_ERROR, "RICOH THETA S is not found.");
	}
	*/
	shader.load("sphericalProjection");

	sphere.set(10, 10);
	sphere.setPosition(ofVec3f(.0f, .0f, .0f));

	bShowHelp = true;
}

//--------------------------------------------------------------
void ofAppVR::exit() {
	openVR.exit();
}

//--------------------------------------------------------------
void ofAppVR::update() {
	if (mainApp->isldCameraConnected)
	{
		image.setFromPixels(mainApp->ldVideoGrabber.getPixels());
	}
	openVR.update();
}

//--------------------------------------------------------------
void ofAppVR::draw() {
	openVR.render();
	openVR.renderDistortion();

	openVR.drawDebugInfo(10.0f, 500.0f);

	// Help
	if (bShowHelp) {
		_strHelp.str("");
		_strHelp.clear();
		_strHelp << "HELP (press h to toggle): " << endl;
		_strHelp << "Drag and drop a 360 spherical (equirectangular) image to load it in the player. " << endl;
		_strHelp << "Toggle OpenVR mirror window (press: m)." << endl;
		ofDrawBitmapStringHighlight(_strHelp.str(), ofPoint(10.0f, 20.0f), ofColor(ofColor::black, 100.0f));
	}
}

//--------------------------------------------------------------
void  ofAppVR::render(vr::Hmd_Eye nEye) {

	ofPushView();
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(openVR.getCurrentProjectionMatrix(nEye));
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofMatrix4x4 currentViewMatrixInvertY = openVR.getCurrentViewMatrix(nEye);
	currentViewMatrixInvertY.scale(1.0f, -1.0f, 1.0f);
	ofLoadMatrix(currentViewMatrixInvertY);

	ofSetColor(ofColor::white);

	shader.begin();
	shader.setUniformTexture("tex0", image, 1);
	sphere.draw();
	shader.end();

	ofPopView();

}

//--------------------------------------------------------------
void ofAppVR::keyPressed(int key) {
	switch (key) {
	case 'h':
		bShowHelp = !bShowHelp;
		break;

	case 'm':
		openVR.toggleMirrorWindow();
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------
void ofAppVR::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofAppVR::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofAppVR::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofAppVR::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofAppVR::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofAppVR::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofAppVR::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofAppVR::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofAppVR::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofAppVR::dragEvent(ofDragInfo dragInfo) {
	//TODO: Why do we need to parse the path to replace the \ by / in order to work?
	std::string path = dragInfo.files[0];
	std::replace(path.begin(), path.end(), '\\', '/');

	image.load(path);
	image.update();
}

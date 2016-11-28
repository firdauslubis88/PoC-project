// Thanks to @num3ric for sharing this:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(false);
	ofDisableArbTex();

	// We need to pass the method we want ofxOpenVR to call when rending the scene
	openVR.setup(std::bind(&ofApp::render, this, std::placeholders::_1));

	image.allocate(1280, 720, OF_IMAGE_COLOR);
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
	if (isldCameraConnected && isHdCameraConnected)
	{
		ldPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		hdPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		combinedToggle.setup("Combined Camera", false);
		combinedToggle.addListener(this, &ofApp::onToggle);
		_panel.add(&combinedToggle);
	}

	shader.load("sphericalProjection");

	sphere.set(10, 10);
	sphere.setPosition(ofVec3f(.0f, .0f, .0f));

	bShowHelp = true;
}

//--------------------------------------------------------------
void ofApp::exit() {
	openVR.exit();
}

//--------------------------------------------------------------
void ofApp::update() {
	ldVideoGrabber.update();
	if (ldVideoGrabber.isFrameNew())
	{
		image.setFromPixels(ldVideoGrabber.getPixelsRef());
	}
	openVR.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
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
	if (isHdCameraConnected)
	{
		hdFbo.begin();
//		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
		hdVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.end();
	}
	/*
	if (cameraSelected == "Combined Camera")
	{
		ofImage combinedImage;
		combinedImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		this->ldFbo.readToPixels(ldPixels);
		this->hdFbo.readToPixels(hdPixels);
		//		ldImage.setFromPixels(ldPixels);
		//		hdImage.setFromPixels(hdPixels);
		//		ldImage.setImageType(OF_IMAGE_COLOR);
		//		hdImage.setImageType(OF_IMAGE_COLOR);
		//		combinedImage.setFromPixels(this->combinedCamera.combine(ldPixels, hdPixels, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT));
		//		combinedImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		combinedImage.setFromPixels(ldPixels);
		//		this->combinedCamera.combine(ldPixels, hdPixels, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		combinedImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else if (cameraSelected == "PTZ Camera")
	{
//		hdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else if (cameraSelected == "Combined Camera")
	{
	//		ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	*/
	ofDisableDepthTest();
	_panel.draw();
}

//--------------------------------------------------------------
void  ofApp::render(vr::Hmd_Eye nEye) {

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
void ofApp::keyPressed(int key) {
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
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
	//TODO: Why do we need to parse the path to replace the \ by / in order to work?
	std::string path = dragInfo.files[0];
	std::replace(path.begin(), path.end(), '\\', '/');

	image.load(path);
	image.update();
}

// Thanks to @num3ric for sharing this:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
//	ofSetVerticalSync(false);
	ofDisableArbTex();

	listVideoDevice = ldVideoGrabber.listDevices();
	isldCameraConnected = false;
	isHdCameraConnected = false;
	_panel.setup();
	for (int i = 0; i < listVideoDevice.size(); i++) {
		ofLog(OF_LOG_VERBOSE, listVideoDevice[i].deviceName);
		if (listVideoDevice[i].deviceName == "THETA UVC Blender") {
			ldVideoGrabber.setDeviceID(listVideoDevice[i].id);
			isldCameraConnected = true;
		}
		if (listVideoDevice[i].deviceName == "PTZ Pro Camera") {
			hdVideoGrabber.setDeviceID(listVideoDevice[i].id);
			isHdCameraConnected = true;
		}
	}
	if (isldCameraConnected) {
		ldVideoGrabber.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT);
		ldFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
//		_shader.load("shaders/equirectanguler");
		sphereVboMesh = ofSpherePrimitive(2000, 24).getMesh();
		for (int i = 0; i<sphereVboMesh.getNumTexCoords(); i++) {
			sphereVboMesh.setTexCoord(i, ofVec2f(1.0) - sphereVboMesh.getTexCoord(i));
		}
		for (int i = 0; i<sphereVboMesh.getNumNormals(); i++) {
			sphereVboMesh.setNormal(i, sphereVboMesh.getNormal(i) * ofVec3f(-1));
		}
		ldToggle.setup("360 Camera", false);
		offsetParameter.set("uvOffset", ofVec4f(0, 0.0, 0, 0.0), ofVec4f(-0.1), ofVec4f(0.1));
		radiusParameter.set("radius", 0.445, 0.0, 1.0);
		ldParameterGroup.add(offsetParameter);
		ldParameterGroup.add(radiusParameter);
		ldToggle.setup("360 Camera", false);
		ldToggle.addListener(this, &ofApp::onToggle);
		_panel.setup(ldParameterGroup);
		_panel.add(&ldToggle);
	}
	else
	{
		ofLog(OF_LOG_ERROR, "RICOH THETA S is not found.");
	}
	if (isHdCameraConnected) {
		hdVideoGrabber.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
		hdToggle.setup("PTZ Camera", false);
		hdToggle.addListener(this, &ofApp::onToggle);
		_panel.add(&hdToggle);
		hdImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
	}
	else
	{
		ofLog(OF_LOG_ERROR, "PTZ Pro Camera is not found.");
	}
	if (isldCameraConnected && isHdCameraConnected) {
		isCombined = true;
		combinedImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		combinedToggle.setup("Combined Camera", false);
		combinedToggle.addListener(this, &ofApp::onToggle);
		ldPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		ldPassImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		_panel.add(&combinedToggle);
	}

	_easyCam.setAutoDistance(false);
	_easyCam.setDistance(0);
}

//--------------------------------------------------------------
 void ofApp::exit() {

}

//--------------------------------------------------------------
void ofApp::update() {
	if (isldCameraConnected)
	{
		ldVideoGrabber.update();
	}
	if (isHdCameraConnected)
	{
		hdVideoGrabber.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (isldCameraConnected)
	{
		ldFbo.begin();
		_easyCam.begin();
		ofClear(0);
		ldVideoGrabber.getTextureReference().bind();
		sphereVboMesh.draw();
		ldVideoGrabber.getTextureReference().unbind();
		_easyCam.end();
		ldFbo.end();
	}
	if (isHdCameraConnected)
	{
		hdFbo.begin();
		hdVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.end();
	}
	if (cameraSelected == "360 Camera")
	{
		ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else if (cameraSelected == "PTZ Camera")
	{
		hdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else if (cameraSelected == "Combined Camera")
	{
		ldFbo.readToPixels(ldPixels);
		ldPassImage.setFromPixels(ldPixels);
		hdImage.setFromPixels(hdVideoGrabber.getPixels());
		combinedCamera.setSkipCloning(false);
		combinedImage.setFromPixels(combinedCamera.combine(ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 2*VIDEO_WIDTH / 5, 2*VIDEO_HEIGHT / 5, VIDEO_WIDTH / 5, VIDEO_HEIGHT / 5));
		combinedImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	ofDisableDepthTest();
	_panel.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

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

}

void ofApp::onToggle(const void * sender)
{
	ofxButton * p = (ofxButton *)sender;
	cameraSelected = p->getName();
}

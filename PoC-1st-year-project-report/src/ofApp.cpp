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
		if (listVideoDevice[i].deviceName == "DFK Z12G445") {
			hdVideoGrabber.setDeviceID(listVideoDevice[i].id);
			isHdCameraConnected = true;
		}
	}
	if (isldCameraConnected) {
		ldVideoGrabber.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT);
		ldFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
		_shader.load("shaders/equirectanguler");
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
	else
	{
		return;
	}

	_easyCam.setAutoDistance(false);
	_easyCam.setDistance(0);
	_easyCam.rotate(-90, 0, 0, 1);
	combinedCamera.setSkipAligning(true);
	combinedCamera.setSkipCloning(false);

	ptzPanOffset = -90;
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
	if ((panAngle + ptzPanOffset) > 180)
	{
		panAngle = 180 - ptzPanOffset;
	}
	else if((panAngle+ptzPanOffset) < -180)
	{
		panAngle = -180 - ptzPanOffset;
	}
	if (tiltAngle > 180)
	{
		tiltAngle = 180;
	}
	else if (tiltAngle < -180)
	{
		tiltAngle = -180;
	}
	hdVideoGrabber.SetPanning(panAngle+ptzPanOffset);
	hdVideoGrabber.SetTilting(tiltAngle);
	prevPanAngle = (int)getPTZEuler().x;
//	_easyCam.setGlobalOrientation(ofQuaternion(0, 0, -sin(PI / 4), cos(PI / 4)));
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (isldCameraConnected)
	{
		ldFbo.begin();
		_easyCam.begin();
		ofClear(0);
		ldVideoGrabber.getTextureReference().bind();
//		_shader.begin();
//		_shader.setUniformTexture("mainTex", ldVideoGrabber.getTexture(), 0);
//		_shader.setUniforms(ldParameterGroup);
		sphereVboMesh.draw();
		ldVideoGrabber.getTextureReference().unbind();
//		_shader.end();
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
		combinedImage.setFromPixels(combinedCamera.combine(ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1*VIDEO_WIDTH / 3, 1*VIDEO_HEIGHT / 3, VIDEO_WIDTH / 3, VIDEO_HEIGHT / 3));
		combinedImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	ofDisableDepthTest();
	_panel.draw();
	if (isHdCameraConnected)
	{
		ofDrawBitmapStringHighlight("PAN ANGLE: " + ofToString(hdVideoGrabber.GetPanning()-ptzPanOffset), 10, 230);
		ofDrawBitmapStringHighlight("TILT ANGLE: " + ofToString(hdVideoGrabber.GetTilting()), 10, 250);
		ofDrawBitmapStringHighlight("Click 'p' to open PTZ camera control settings", 10, 270);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key)
	{
		case 'w':
			tiltAngle++;
			followingMode = false;
			break;
		case 's':
			tiltAngle--;
			followingMode = false;
			break;
		case 'a':
			panAngle--;
			followingMode = false;
			break;
		case 'd':
			panAngle++;
			followingMode = false;
			break;
		case 'p':
			onProperty();
			break;
		case 'c':
			cout << "origin\t" << _easyCam.getGlobalOrientation().getEuler() << endl;
			cout << "change\t" << getPTZEuler() << endl;
			break;
		case 'r':
			restart();
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
	if (prevXDrag == -1 && prevYDrag == -1)
	{
		prevXDrag = x;
		prevYDrag = y;
	}
	if (cameraSelected == "PTZ Camera")
	{
		if (y > (prevYDrag + 50))
		{
			tiltAngle--;
		}
		else if (y < (prevYDrag - 50))
		{
			tiltAngle++;
		}
		if (x >(prevXDrag + 50))
		{
			panAngle++;
		}
		else if (x < (prevXDrag - 50))
		{
			panAngle--;
		}
	}
	else if (cameraSelected == "360 Camera")
	{
		if (followingMode)
		{
			int currentPan = (int)getPTZEuler().x;
			ofVec3f ldCameraEulerOrientation = getPTZEuler();
			if ((x < prevXDrag) && (prevPanAngle < currentPan))
			{
				panAngle = -(ldCameraEulerOrientation.x);
			}
			else if ((x < prevXDrag) && (prevPanAngle > currentPan))
			{
				panAngle = 180+(ldCameraEulerOrientation.x);
			}
			else if ((x > prevXDrag) && (prevPanAngle > currentPan))
			{
				panAngle = -(ldCameraEulerOrientation.x);
			}
			else if ((x > prevXDrag) && (prevPanAngle < currentPan))
			{
				panAngle = 180 + (ldCameraEulerOrientation.x);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (cameraSelected == "PTZ Camera")
	{
		_easyCam.disableMouseMiddleButton();
		_easyCam.disableMouseInput();
	}
	if (cameraSelected == "360 Camera")
	{
		followingMode = true;
		_easyCam.enableMouseMiddleButton();
		_easyCam.enableMouseInput();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (prevXDrag != -1 && prevYDrag != -1)
	{
		prevXDrag = -1;
		prevYDrag = -1;
	}
}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	if (cameraSelected == "PTZ Camera")
	{
		long min, max, SteppingDelta, currentValue, flags, defaultValue;
		currentValue = hdVideoGrabber.GetZooming();
		long nextZoom = scrollY * 10 + currentValue;
		hdVideoGrabber.SetZooming(nextZoom);
	}
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

void ofApp::onProperty()
{
	if (cameraSelected == "PTZ Camera")
	{
		hdVideoGrabber.videoSettings();
	}
}

void ofApp::restart()
{
	_easyCam.reset();
	_easyCam.rotate(-90, 0, 0, 1);

	tiltAngle = 0;
	panAngle = 0;
}

// ref at http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
ofVec3f ofApp::getPTZEuler() const {
	ofQuaternion myQuaternion = _easyCam.getOrientationQuat();
	double theta3, theta2, theta1;
	double e = -1;
	double p0 = myQuaternion.w(), p1 = myQuaternion.z(), p2 = myQuaternion.x(), p3 = myQuaternion.y();
	double test = e*p3*p1 + p2*p0;
//	cout << test << endl;
	if (test > 0.499) { // singularity at north pole
		theta1 = 2 * atan2(p1, p0);
		theta2 = PI / 2;
		theta3 = 0;
	}
	else if (test < -0.499) { // singularity at south pole
		theta1 = -2 * atan2(p1, p0);
		theta2 = -PI / 2;
		theta3 = 0;
	}
	else {
		double sqp3 = p3 * p3;
		double sqp1 = p1 * p1;
		double sqp2 = p2 * p2;
		theta1 = atan2(2.0f * p1 * p0 - 2.0f * p3 * p2 * e, 1.0f - 2.0f*sqp1 - 2.0f*sqp2);
		theta2 = asin(2 * test);
		theta3 = atan2(2.0f*p3 * p0 - 2.0f * p1 * p2 * e, 1.0f - 2.0f*sqp3 - 2.0f*sqp2);
	}
	float attitude = theta1;
	float heading = theta3;
	float bank = theta2;
	return ofVec3f(ofRadToDeg(bank), ofRadToDeg(heading), ofRadToDeg(attitude));
}

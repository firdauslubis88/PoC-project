// Thanks to @num3ric for sharing Athis:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6

#include "ofApp_All.h"

//--------------------------------------------------------------
void ofApp::setup() {
	VIDEO_WIDTH = ofGetWidth();
	VIDEO_HEIGHT = ofGetHeight();
	allowUpdatePTZ = true;
	ofSetVerticalSync(false);
	ofDisableArbTex();
	printScreenCount = 0;
	listVideoDevice = ldVideoGrabber.listDevices();
	isldCameraConnected = false;
	isHdCameraConnected = false;
	_panel.setup();
	for (int i = 0; i < listVideoDevice.size(); i++) {
		ofLog(OF_LOG_VERBOSE, listVideoDevice[i].deviceName);
		if (listVideoDevice[i].deviceName == "RICOH THETA S") {
			isldCameraConnected = true;
			ldPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
			ldToggle.setup("360 Camera", false);
			ldToggle.addListener(this, &ofApp::onToggle);
			_panel.add(&ldToggle);
		}
		if (listVideoDevice[i].deviceName == "THETA UVC Blender") {
			ldVideoGrabber.setDeviceID(listVideoDevice[i].id);
			ldVideoGrabber.initGrabber(VIDEO_WIDTH, VIDEO_HEIGHT);
			ldFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			//	_shader.load("shaders/equirectanguler");
			sphereVboMesh = ofSpherePrimitive(2000, 24).getMesh();
			for (int i = 0; i<sphereVboMesh.getNumTexCoords(); i++) {
				sphereVboMesh.setTexCoord(i, ofVec2f(1.0) - sphereVboMesh.getTexCoord(i));
			}
			for (int i = 0; i<sphereVboMesh.getNumNormals(); i++) {
				sphereVboMesh.setNormal(i, sphereVboMesh.getNormal(i) * ofVec3f(-1));
			}
		}

		if (listVideoDevice[i].deviceName == "PTZ Pro Camera") {
			hdVideoGrabber.setDeviceID(listVideoDevice[i].id);
			hdVideoGrabber.setup(VIDEO_WIDTH, VIDEO_HEIGHT);
			hdVideoGrabber.setPtzPanOffset(0);
			hdVideoGrabber.setPtzTiltOffset(0);
			hdVideoGrabber.setPtzPanScale(1);
			hdVideoGrabber.setPtzTiltScale(1);
			//	ptzPanOffset = 0;// 90;
			//	ptzTiltOffset = 0;
			hdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
			hdToggle.setup("PTZ Camera", false);
			hdToggle.addListener(this, &ofApp::onToggle);
			_panel.add(&hdToggle);
			isHdCameraConnected = true;
			hdPixels.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		}
	}

	if (!isldCameraConnected) {
		ofLog(OF_LOG_ERROR, "RICOH THETA S is not found.");
	}

	if (!isHdCameraConnected){
		ofLog(OF_LOG_ERROR, "PTZ Pro Camera is not found.");
	}

	if (!isldCameraConnected && !isHdCameraConnected)
	{
		ofExit();
	}

	if (isldCameraConnected && isHdCameraConnected) {
		isCombined = true;
		combinedImage.allocate(VIDEO_WIDTH, VIDEO_HEIGHT, OF_IMAGE_COLOR);
		combinedToggle.setup("Combined Camera", false);
		combinedToggle.addListener(this, &ofApp::onToggle);
		_panel.add(&combinedToggle);
		combinedCameraQueue.setMaximumTasks(1);
		combinedCameraQueue.registerTaskProgressEvents(this);
		combinedCamera = CombinedCamera(VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	_easyCam.setAutoDistance(false);
	_easyCam.setDistance(0);
	_easyCam.rotate(-90, 0, 0, 1);
//	_easyCam.setFov(40.0);

//	combinedCamera.setSkipAligning(false);
//	combinedCamera.setSkipCloning(false);

	combinedCameraFinished = true;
	showROI = true;
	combinedMode = true;
	maskXStart = (float)VIDEO_WIDTH / 3;
	maskYStart = (float)VIDEO_HEIGHT / 3;
	maskWidth = 432;
	maskHeight = 224;

	combinedCamera.setSkipCloning(true);

	// Limit the maximum number of tasks for shared thread pools.
	queue.setMaximumTasks(1);
	// Register to receive task queue events.
	queue.registerTaskProgressEvents(this);

	videoRecorderSetup();
#ifdef USE_PTZ_ADJUSTMENT
	Alignment::ptzAlreadyChanged = false;
#endif
}

//--------------------------------------------------------------
 void ofApp::exit() {
	 videoRecorderExit();
	 queue.cancelAll();
	 combinedCameraQueue.cancelAll();
}

//--------------------------------------------------------------
 void ofApp::update() {
	 if (isldCameraConnected)
	 {
		 ldVideoGrabber.update();
		 videoRecorderUpdate();
	 }
	 if (isHdCameraConnected)
	 {
		 hdVideoGrabber.update();
#ifdef USE_PTZ_ADJUSTMENT
		 panAngle += Alignment::xReturn;
		 tiltAngle += Alignment::yReturn;
#endif // USE_PTZ_ADJUSTMENT
#ifdef USE_PTZ_ADJUSTMENT
		 if (!combinedCameraFinished && !Alignment::ptzAlreadyChanged)
		 {
			 queue.start(new PTZMotorControl("UPDATE PT", panSend, tiltSend));
		 }
#endif
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
		ldFbo.readToPixels(ldPixels);
	}
	if (isHdCameraConnected)
	{
		hdFbo.begin();
		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
//		hdVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.end();
		hdFbo.readToPixels(hdPixels);
		hdImage.setFromPixels(hdPixels);
	}
	if (cameraSelected == "360 Camera")
	{
		ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		if (showROI)
		{
			ofSetColor(0, 255, 0);
			ofNoFill();
			ofRect(maskXStart, maskYStart, maskWidth, maskHeight);
			ofSetColor(255, 255, 255);
		}
		ofDrawBitmapStringHighlight("Click 'h' to toggle the ROI green area", 10, 100);
	}
	else if (cameraSelected == "PTZ Camera")
	{
		hdFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		ofDrawBitmapStringHighlight("Click 'p' to open PTZ camera control settings", 10, 170);
	}
	else if (cameraSelected == "Combined Camera")
	{
		if (combinedMode)
		{
			combinedImage.setFromPixels(combinedCamera.combine_direct(ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
			combinedImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		}
		else
		{
			ldFbo.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
			ofSetColor(0, 255, 0);
			ofNoFill();
			ofRect(maskXStart, maskYStart, maskWidth, maskHeight);
			ofSetColor(255, 255, 255);
		}

		ofDrawBitmapStringHighlight("Click 'j' to realign the camera", 10, 190);
		if (combinedCameraFinished)ofDrawBitmapStringHighlight("DONE ALIGNING", 10, 210);
		else ofDrawBitmapStringHighlight("ALIGNING...", VIDEO_WIDTH/2 - 40, VIDEO_HEIGHT / 2 - 40);
	}

	ofDisableDepthTest();
	_panel.draw();

//	ofDrawBitmapStringHighlight("PAN ANGLE: " + ofToString(PTZControl::GetPanning()), 10, 130);
//	ofDrawBitmapStringHighlight("TILT ANGLE: " + ofToString(PTZControl::GetTilting()), 10, 150);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key)
	{
	case 'w':
		if (cameraSelected == "PTZ Camera")
		{
			ptzCameraCommand(TILT, -1, "UPDATE PTZ");
		}
		break;
	case 's':
		if (cameraSelected == "PTZ Camera")
		{
			ptzCameraCommand(TILT, 1, "UPDATE PTZ");
		}
		break;
	case 'a':
		if (cameraSelected == "PTZ Camera")
		{
			ptzCameraCommand(PAN, 1, "UPDATE PTZ");
		}
		break;
	case 'd':
		if (cameraSelected == "PTZ Camera")
		{
			ptzCameraCommand(PAN, -1, "UPDATE PTZ");
		}
		break;
	case 'p':
		onProperty();
		break;
	case 'r':
		restart();
		break;
	case 'l':
		videoRecorderStartRecord();
		break;
	case 'k':
		videoRecorderStopRecord();
		break;
	case 'j':
		combinedCamera.alignment.alreadyChanged = false;
		combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
		break;
	case 'n':
		combinedCamera.alignment.alreadyChanged = true;
		break;
	case 'h':
		showROI = !showROI;
		break;
	case 'g':
		printScreen();
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
		if (y > (prevYDrag + hdVideoGrabber.getCameraTiltDragThres()))
		{
			ptzCameraCommand(TILT, 1, "UPDATE PTZ");
		}
		else if (y < (prevYDrag - hdVideoGrabber.getCameraTiltDragThres()))
		{
			ptzCameraCommand(TILT, -1, "UPDATE PTZ");
		}
		if (x >(prevXDrag + hdVideoGrabber.getCameraPanDragThres()))
		{
			ptzCameraCommand(PAN, -1, "UPDATE PTZ");
		}
		else if (x < (prevXDrag - hdVideoGrabber.getCameraPanDragThres()))
		{
			ptzCameraCommand(PAN, 1, "UPDATE PTZ");
		}
	}
	else if (cameraSelected == "360 Camera" || cameraSelected == "Combined Camera")
	{
		float ricohY = getPTZEuler().x;
		float ricohX = getPTZEuler().y;
		float ricohZ = getPTZEuler().z;

		int localPanAngle = (int)ricohY;
		if (abs(ricohX) >= 90)
		{
			localPanAngle = -(180 + ricohY);
		}
		hdVideoGrabber.setPanAngle(localPanAngle);
		int localTiltAngle = (int)ricohX;
		if (abs(ricohX) >= 90)
		{
			localTiltAngle = -(ricohX) + 180;
		}
		hdVideoGrabber.setTiltAngle(localTiltAngle);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (cameraSelected == "PTZ Camera")
	{
		_easyCam.disableMouseMiddleButton();
		_easyCam.disableMouseInput();
	}
	else if (cameraSelected == "360 Camera")
	{
		_easyCam.enableMouseMiddleButton();
		_easyCam.enableMouseInput();
	}
	else if (cameraSelected == "Combined Camera")
	{
		_easyCam.enableMouseMiddleButton();
		_easyCam.enableMouseInput();
		combinedMode = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (prevXDrag != -1 && prevYDrag != -1)
	{
		prevXDrag = -1;
		prevYDrag = -1;
	}
	if (cameraSelected == "Combined Camera")
	{
		combinedMode = true;
		ptzCameraCommand("UPDATE PTZ THEN COMBINE");
	}
}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	if (cameraSelected == "PTZ Camera")
	{
		ptzCameraCommand(ZOOM, scrollY, "UPDATE PTZ");
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
// Thanks to @num3ric for sharing Athis:
// http://discourse.libcinder.org/t/360-vr-video-player-for-ios-in-cinder/294/6

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	panAngle = 0;
	tiltAngle = 0;
	allowUpdatePT = true;
	ofSetVerticalSync(false);
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
	//	_shader.load("shaders/equirectanguler");
		sphereVboMesh = ofSpherePrimitive(2000, 24).getMesh();
		for (int i = 0; i<sphereVboMesh.getNumTexCoords(); i++) {
			sphereVboMesh.setTexCoord(i, ofVec2f(1.0) - sphereVboMesh.getTexCoord(i));
		}
		for (int i = 0; i<sphereVboMesh.getNumNormals(); i++) {
			sphereVboMesh.setNormal(i, sphereVboMesh.getNormal(i) * ofVec3f(-1));
		}
		ldToggle.setup("360 Camera", false);
		ldToggle.addListener(this, &ofApp::onToggle);
		_panel.add(&ldToggle);
	}
	else
	{
		ofLog(OF_LOG_ERROR, "RICOH THETA S is not found.");
	}
	if (isHdCameraConnected) {
		hdVideoGrabber.setup(VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.allocate(VIDEO_WIDTH, VIDEO_HEIGHT);
		hdToggle.setup("PTZ Camera", false);
		hdToggle.addListener(this, &ofApp::onToggle);
		_panel.add(&hdToggle);
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
		_panel.add(&combinedToggle);
		combinedCameraQueue.setMaximumTasks(1);
		combinedCameraQueue.registerTaskProgressEvents(this);
		combinedCamera = CombinedCamera(VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	if (!isldCameraConnected && !isHdCameraConnected)
	{
		exit();
	}
	_easyCam.setAutoDistance(false);
	_easyCam.setDistance(0);
	_easyCam.rotate(-90, 0, 0, 1);
//	_easyCam.setFov(40.0);

//	combinedCamera.setSkipAligning(false);
//	combinedCamera.setSkipCloning(false);

	ptzPanOffset = 0;// 90;
	ptzTiltOffset = 0;

	combinedCameraFinished = true;
	showROI = true;
	combinedMode = true;
	maskXStart = (float)VIDEO_WIDTH / 3;
	maskYStart = (float)VIDEO_HEIGHT / 3;
	maskWidth = 432 * 2;
	maskHeight = 224 * 2;

	CombinedCamera::setSkipCloning(true);

	// Limit the maximum number of tasks for shared thread pools.
	queue.setMaximumTasks(1);
	// Register to receive task queue events.
	queue.registerTaskProgressEvents(this);


#ifdef USE_VIDEO_RECORDER
#ifdef TARGET_WIN32
	string appPath = ofFilePath::getAbsolutePath(ofFilePath::getCurrentExePath());
	vidRecorder.setFfmpegLocation(appPath + "\\ffmpeg.exe"); // use this is you have ffmpeg installed in your data folder
#endif
	fileName = "testMovie";
	fileExt = ".mp4"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats

					  // override the default codecs if you like
					  // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
	vidRecorder.setVideoCodec("mpeg4");
	vidRecorder.setVideoBitrate("800k");
	vidRecorder.setAudioCodec("mp3");
	vidRecorder.setAudioBitrate("192k");
	bRecording = false;
	sampleRate = 44100;
	channels = 2;

	soundStream.setup(this, 0, channels, sampleRate, 256, 4);

	ofEnableAlphaBlending();
#endif // USE_VIDEO_RECORDER
#ifdef USE_PTZ_ADJUSTMENT
	Alignment::ptzAlreadyChanged = false;
#endif
}

//--------------------------------------------------------------
 void ofApp::exit() {
#ifdef USE_VIDEO_RECORDER
	 vidRecorder.close();
#endif // USE_VIDEO_RECORDER
	 queue.cancelAll();
	 combinedCameraQueue.cancelAll();
}

//--------------------------------------------------------------
 void ofApp::update() {
	 if (isldCameraConnected)
	 {
		 ldVideoGrabber.update();
#ifdef USE_VIDEO_RECORDER
		 if (ldVideoGrabber.isFrameNew() && bRecording)
		 {
			 vidRecorder.addFrame(ldVideoGrabber.getPixelsRef());
			 //			 ofLogWarning("This frame was not added!");
		 }
		 // Check if the video recorder encountered any error while writing video frame or audio smaples.
		 if (vidRecorder.hasVideoError()) {
			 ofLogWarning("The video recorder failed to write some frames!");
		 }

		 if (vidRecorder.hasAudioError()) {
			 ofLogWarning("The video recorder failed to write some audio samples!");
		 }
#endif
	 }
	 if (isHdCameraConnected)
	 {
		 hdVideoGrabber.update();
#ifdef USE_PTZ_ADJUSTMENT
		 panAngle += Alignment::xReturn;
		 tiltAngle += Alignment::yReturn;
#endif // USE_PTZ_ADJUSTMENT
		 panSend = -(panAngle + ptzPanOffset);
		 tiltSend = -(tiltAngle + ptzTiltOffset);
		 if (panSend > 180)
		 {
			 panSend -= 360;
		 }
		 else if (panSend < -180)
		 {
			 panSend += 360;
		 }
		 if (tiltSend > 180)
		 {
			 tiltSend -= 360;
		 }
		 else if (tiltSend < -180)
		 {
			 tiltSend += 360;
		 }
		 if (cameraSelected == "PTZ Camera" && allowUpdatePT)
		 {
			 allowUpdatePT = false;
			 queue.start(new PTZCameraTask("UPDATE PT", hdVideoGrabber.getPTZ(), panSend, tiltSend));
		 }

#ifdef USE_PTZ_ADJUSTMENT
		 if (!combinedCameraFinished && !Alignment::ptzAlreadyChanged)
		 {
			 queue.start(new PTZMotorControl("UPDATE PT", panSend, tiltSend));
		 }
#endif
		 //	 cout << "Tilt sent:\t" << tiltSend << endl;
		 //	 tiltSend = 0;
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
//		hdVideoGrabber.draw(VIDEO_WIDTH, VIDEO_HEIGHT, -VIDEO_WIDTH, -VIDEO_HEIGHT);
		hdVideoGrabber.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		hdFbo.end();
	}
	if (isldCameraConnected && isHdCameraConnected)
	{
		ldFbo.readToPixels(ldPixels);
		hdImage.setFromPixels(hdVideoGrabber.getPixels());
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
			combinedImage.setFromPixels(CombinedCamera::combine_direct(ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
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
		tiltAngle--;
		break;
	case 's':
		tiltAngle++;
		break;
	case 'a':
		panAngle++;
		break;
	case 'd':
		panAngle--;
		break;
	case 'p':
		onProperty();
		break;
	case 'r':
		restart();
		break;
#ifdef USE_VIDEO_RECORDER
	case 'l':
		start_record();
		break;
	case 'k':
		stop_record();
		break;
#endif // USE_VIDEO_RECORDER
	case 'j':
		Alignment::alreadyChanged = false;
///		combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
		break;
	case 'n':
		Alignment::alreadyChanged = true;
		break;
	case 'h':
		showROI = !showROI;
		break;
	case 'g':
		printScreen();
		break;
	default:
		if (cameraSelected == "PTZ Camera")
		{
			queue.start(new PTZCameraTask("UPDATE PT", hdVideoGrabber.getPTZ(), 0, 0));
		}
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
			tiltAngle++;
		}
		else if (y < (prevYDrag - 50))
		{
			tiltAngle--;
		}
		if (x >(prevXDrag + 50))
		{
			panAngle--;
		}
		else if (x < (prevXDrag - 50))
		{
			panAngle++;
		}
	}
	else if (cameraSelected == "360 Camera" || cameraSelected == "Combined Camera")
	{
		float ricohY = getPTZEuler().x;
		float ricohX = getPTZEuler().y;
		float ricohZ = getPTZEuler().z;

		panAngle = (int)ricohY;
		if (abs(ricohX) >= 90)
		{
			panAngle = -(180 + ricohY);
		}
		tiltAngle = (int)ricohX;
		if (abs(ricohX) >= 90)
		{
			tiltAngle = -(ricohX) + 180;
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
	else if (cameraSelected == "360 Camera")
	{
		_easyCam.enableMouseMiddleButton();
		_easyCam.enableMouseInput();
	}
	else if (cameraSelected == "Combined Camera")
	{
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
///		queue.start(new PTZMotorControl("UPDATE PT THEN COMBINE", panSend, tiltSend));
	}
}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	if (cameraSelected == "PTZ Camera")
	{
		long currentValue = hdVideoGrabber.GetZooming();
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
	if (cameraSelected == "PTZ Camera")
	{
		queue.start(new PTZCameraTask("UPDATE PT", hdVideoGrabber.getPTZ(), panSend, tiltSend));
	}
	if (cameraSelected == "Combined Camera")
	{
#ifdef USE_PTZ_ADJUSTMENT
		Alignment::ptzAlreadyChanged = false;
#endif
///		queue.start(new PTZMotorControl("UPDATE PT THEN COMBINE", panSend, tiltSend));
	}
}

// ref at http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
ofVec3f ofApp::getPTZEuler() const {
	ofQuaternion myQuaternion = _easyCam.getOrientationQuat();
	double theta3, theta2, theta1;
	double e = -1;
	double p0 = myQuaternion.w(), p1 = myQuaternion.z(), p2 = myQuaternion.x(), p3 = myQuaternion.y();
	double test = e*p3*p1 + p2*p0;
	//	cout << test << endl;
	if (test > 0.499999) { // singularity at north pole
		theta1 = 2 * atan2(p1, p0);
		theta2 = PI / 2;
		theta3 = 0;
	}
	else if (test < -0.499999) { // singularity at south pole
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

#ifdef USE_VIDEO_RECORDER
void ofApp::start_record()
{
	bRecording = true;
	if (bRecording && !vidRecorder.isInitialized()) {
		vidRecorder.setup(fileName + ofGetTimestampString() + fileExt, ldVideoGrabber.getWidth(), ldVideoGrabber.getHeight(), 30, sampleRate, channels);
		//          vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30); // no audio
		//            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 0,0,0, sampleRate, channels); // no video
		//          vidRecorder.setupCustomOutput(vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels, "-vcodec mpeg4 -b 1600k -acodec mp2 -ab 128k -f mpegts udp://localhost:1234"); // for custom ffmpeg output string (streaming, etc)

		// Start recording
		vidRecorder.start();
	}
	else if (!bRecording && vidRecorder.isInitialized()) {
		vidRecorder.setPaused(true);
	}
	else if (bRecording && vidRecorder.isInitialized()) {
		vidRecorder.setPaused(false);
	}
}

void ofApp::stop_record()
{
	bRecording = false;
	vidRecorder.close();
}

#endif // USE_VIDEO_RECORDER
void ofApp::printScreen()
{
	hdImage.save("PTZ.jpg");
	ofImage ldImage;
	ldImage.setFromPixels(ldPixels);
	ldImage.setImageType(OF_IMAGE_COLOR);
	ldImage.save("360.jpg");
}


void ofApp::onTaskQueued(const ofx::TaskQueueEventArgs & args)
{
}

void ofApp::onTaskStarted(const ofx::TaskQueueEventArgs & args)
{
	taskProgress[args.getTaskId()].update(args);
	if (args.getTaskName() == "Combined Camera")
	{
		combinedCameraFinished = false;
	}
}

void ofApp::onTaskCancelled(const ofx::TaskQueueEventArgs & args)
{
}

void ofApp::onTaskFinished(const ofx::TaskQueueEventArgs & args)
{
	taskProgress[args.getTaskId()].update(args);
	if (args.getTaskName() == "Combined Camera")
	{
		if (Alignment::alreadyChanged)
		{
			combinedCameraFinished = true;
		}
		else
		{
///			combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
		}
	}
	else if (args.getTaskName() == "UPDATE PT THEN COMBINE")
	{
		allowUpdatePT = true;
		ofSleepMillis(1000);
		Alignment::alreadyChanged = false;
///		combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
	}
	else
	{
		allowUpdatePT = true;
	}
}

void ofApp::onTaskFailed(const ofx::TaskFailedEventArgs & args)
{
}

void ofApp::onTaskProgress(const ofx::TaskProgressEventArgs & args)
{
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

	panSend = -(panAngle + ptzPanOffset);
	if (panSend > 180)
	{
		panSend -= 360;
	}
	else if (panSend < -180)
	{
		panSend += 360;
	}
	tiltSend = -(tiltAngle + ptzTiltOffset);
	if (tiltSend > 180)
	{
		tiltSend -= 360;
	}
	else if (tiltSend < -180)
	{
		tiltSend += 360;
	}
	queue.start(new PTZCameraTask("UPDATE PT", hdVideoGrabber.getPTZ(), panSend, tiltSend));
}

#ifdef USE_VIDEO_RECORDER
void ofApp::audioIn(float *input, int bufferSize, int nChannels) {
	if (bRecording)
		vidRecorder.addAudioSamples(input, bufferSize, nChannels);
}
#endif // USE_VIDEO_RECORDER



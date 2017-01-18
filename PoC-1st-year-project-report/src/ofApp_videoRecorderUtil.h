#pragma once
#include "ofApp.h"
bool ofApp::videoRecorderSetup()
{
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
	return true;
#endif
	return false;
}

bool ofApp::videoRecorderUpdate()
{
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
	return true;
#endif
	return false;
}

bool ofApp::videoRecorderExit()
{
#ifdef USE_VIDEO_RECORDER
	vidRecorder.close();
#endif
	return false;
}

bool ofApp::videoRecorderStartRecord()
{
#ifdef USE_VIDEO_RECORDER
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
	return true;
#endif
	return false;
}

bool ofApp::videoRecorderStopRecord()
{
#ifdef USE_VIDEO_RECORDER
	bRecording = false;
	vidRecorder.close();
	return true;
#endif
	return false;
}

void ofApp::audioIn(float *input, int bufferSize, int nChannels) {
#ifdef USE_VIDEO_RECORDER
	if (bRecording)
		vidRecorder.addAudioSamples(input, bufferSize, nChannels);
#endif
}

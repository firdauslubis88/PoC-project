#pragma once
#include "ofApp.h"

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
		if (combinedCamera.alignment.alreadyChanged)
		{
			combinedCameraFinished = true;
		}
		else
		{
			combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
		}
	}
	else if (args.getTaskName() == "UPDATE PTZ THEN COMBINE")
	{
		allowUpdatePTZ = true;
		ofSleepMillis(1000);
		combinedCamera.alignment.alreadyChanged = false;
		combinedCameraQueue.start(new CombinedCameraTask("Combined Camera", ldPixels, hdImage, VIDEO_WIDTH, VIDEO_HEIGHT, 1 * VIDEO_WIDTH / 3, 1 * VIDEO_HEIGHT / 3, maskWidth, maskHeight));
	}
	else
	{
		allowUpdatePTZ = true;
	}
}

void ofApp::onTaskFailed(const ofx::TaskFailedEventArgs & args)
{
}

void ofApp::onTaskProgress(const ofx::TaskProgressEventArgs & args)
{
}
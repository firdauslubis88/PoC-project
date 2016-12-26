#include "SimpleTaskProgress.h"
#include "ofGraphics.h"


SimpleTaskProgress::SimpleTaskProgress() :
	taskId(""),
	name(""),
	state(Poco::Task::TASK_IDLE),
	progress(0),
	errorMessage(""),
	data(""),
	fader(1)
{
}


SimpleTaskProgress::SimpleTaskProgress(const ofx::TaskQueueEventArgs& args) :
	taskId(args.getTaskId()),
	name(args.getTaskName()),
	state(args.getState()),
	progress(0),
	errorMessage(""),
	data(""),
	fader(1)
{
}


SimpleTaskProgress::~SimpleTaskProgress()
{
}


void SimpleTaskProgress::update(const ofx::TaskQueueEventArgs& args)
{
	taskId = args.getTaskId();
	name = args.getTaskName();
	state = args.getState();
}


void SimpleTaskProgress::update(const ofx::TaskProgressEventArgs& args)
{
	taskId = args.getTaskId();
	name = args.getTaskName();
	state = args.getState();
	progress = args.getProgress();
}


void SimpleTaskProgress::update(const ofx::TaskFailedEventArgs& args)
{
	taskId = args.getTaskId();
	name = args.getTaskName();
	state = args.getState();
	errorMessage = args.getException().displayText();
}


void SimpleTaskProgress::draw(float x, float y, float width, float height)
{

}

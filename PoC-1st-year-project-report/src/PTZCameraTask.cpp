#include "PTZCameraTask.h"
#include "Poco/TaskNotification.h"

PTZCameraTask::PTZCameraTask(const std::string & name, shared_ptr<BasePTZCamera> ptz) : Poco::Task(name), privatePtz(ptz)
{
}

PTZCameraTask::~PTZCameraTask()
{
}

void PTZCameraTask::runTask()
{
	privatePtz->SetPanning();
	privatePtz->SetTilting();
}

#include "PTZCameraTask.h"
#include "Poco/TaskNotification.h"

PTZCameraTask::PTZCameraTask(const std::string & name, shared_ptr<BasePTZCamera> ptz, int panSent, int tiltSent) : Poco::Task(name), privatePtz(ptz), privatePanSent(panSent), privateTiltSent(tiltSent)
{
}

PTZCameraTask::~PTZCameraTask()
{
}

void PTZCameraTask::runTask()
{
	privatePtz->SetPanning(privatePanSent);
	privatePtz->SetTilting(privateTiltSent);
}

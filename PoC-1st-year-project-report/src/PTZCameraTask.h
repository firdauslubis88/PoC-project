#pragma once
#include "PTZCamera.h"
#include "Poco/Task.h"

class PTZCameraTask : public Poco::Task
{
public:
	PTZCameraTask(const std::string& name, shared_ptr<BasePTZCamera> ptz, int panSent, int tiltSent);
	~PTZCameraTask();
	void runTask();

private:
	shared_ptr<BasePTZCamera> privatePtz;
	volatile int privatePanSent;
	volatile int privateTiltSent;
};


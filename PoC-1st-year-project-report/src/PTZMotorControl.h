#pragma once
#include "PTZControl.h"
#include "Poco/Task.h"

class PTZMotorControl: public Poco::Task
{
public:
	PTZMotorControl(const std::string& name, int panSent, int tiltSent);
	~PTZMotorControl();
	void runTask();

private:
	int privatePanSent;
	int privateTiltSent;
};


#include "PTZMotorControl.h"
#include "Poco/TaskNotification.h"

PTZMotorControl::PTZMotorControl(const std::string & name, int panSent, int tiltSent): Poco::Task(name),privatePanSent(panSent), privateTiltSent(tiltSent)
{
}

PTZMotorControl::~PTZMotorControl()
{
}

void PTZMotorControl::runTask()
{
	PTZControl::SetPanning(privatePanSent);
	PTZControl::SetTilting(privateTiltSent);
}


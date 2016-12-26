#pragma once

#include <conio.h>

#include <stdlib.h>
#include <stdio.h>
#include "ofMain.h"
#include "dynamixel_sdk.h"	// Uses Dynamixel SDK library

//#include "Listener.h"
//using namespace DShowLib;

// Control table address
#define ADDR_PRO_TORQUE_ENABLE          64                 // Control table address is different in Dynamixel model
#define ADDR_PRO_GOAL_POSITION          116
#define ADDR_PRO_PRESENT_POSITION       132

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID1                          1                   // Dynamixel ID: 1 - pan
#define DXL_ID2                          2                   // Dynamixel ID: 2 - tilt
#define BAUDRATE                        57600
#define DEVICENAME                      "COM2"				// Check which port is being used on your controller

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      0             // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4095              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     5                  // Dynamixel moving status threshold

class PTZControl: public ofVideoGrabber
{
public:
	PTZControl();
	~PTZControl();

	static dynamixel::PortHandler *portHandler;
	static dynamixel::PacketHandler *packetHandler;
	static bool alreadyUsed;

	int index;
	int dxl_comm_result;
	int dxl_goal_position_pan;
	int dxl_goal_position_tilt;
	int newZoom;

	long min, max, SteppingDelta, currentValue, flags, defaultValue;
	//int dxl_goal_position[2];

	uint8_t dxl_error;
	int32_t dxl_present_position_pan;
	int32_t dxl_present_position_tilt;

	static int PTZControl::SetPanning(int input);
	static int PTZControl::GetPanning();
	static int PTZControl::SetTilting(int input);
	static int PTZControl::GetTilting();
	int PTZControl::SetZooming(int zoom);
	long PTZControl::GetZooming();

	//DShowLib::Grabber		m_cGrabber; // The instance of the Grabber class.
	int PTZControl::getch();
	int PTZControl::kbhit(void);
};


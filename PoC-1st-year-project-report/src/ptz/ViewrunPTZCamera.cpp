#include "ViewrunPTZCamera.h"

/*This implementation PTZ camera class (logitech) only support relative panning.
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
ViewrunPTZCamera::ViewrunPTZCamera()
{
	this->panAngle = 0;
	this->tiltAngle = 0;
	this->ptzPanOffset = 0;
	this->ptzTiltOffset = 0;
	this->ptzPanScale = 1;
	this->ptzTiltScale = 1;

	// Initialize PortHandler instance
	// Set the port path
	// Get methods and members of PortHandlerWindows
	portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

	// Initialize PacketHandler instance
	// Set the protocol version
	// Get methods and members of Protocol2PacketHandler
	packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

	index = 0;
	dxl_comm_result = COMM_TX_FAIL;             // Communication result
												//dxl_goal_position[2] = { DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE };         // Goal position

	dxl_error = 0;                          // Dynamixel error
	dxl_present_position_pan = 0;               // Present position
	dxl_present_position_tilt = 0;
	// Open port
	if (portHandler->openPort())
	{
		printf("Succeeded to open the port!\n");
	}
	else
	{
		printf("Failed to open the port!\n");
		printf("Press any key to terminate...\n");
		getch();
	}

	// Set port baudrate
	if (portHandler->setBaudRate(BAUDRATE))
	{
		printf("Succeeded to change the baudrate!\n");
	}
	else
	{
		printf("Failed to change the baudrate!\n");
		printf("Press any key to terminate...\n");
		getch();
	}

	// Enable Dynamixel Torque
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID1, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}
	else
	{
		printf("Dynamixel has been successfully connected \n");
	}

}

int ViewrunPTZCamera::SetPanning()
{
	uint8_t dxl_error;
	int dxl_goal_position_pan = (this->panSend + 180) * 4096 / 360;
	int dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID1, ADDR_PRO_GOAL_POSITION, dxl_goal_position_pan, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}

	return 0;
}

//This implementation PTZ camera class (logitech) doesn't support this method. It will just send 0
int ViewrunPTZCamera::GetPanning()
{
	uint8_t dxl_error;
	int dxl_present_position_pan;
	int dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID1, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position_pan, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}

	dxl_present_position_pan = (dxl_present_position_pan * 360 / 4096) - 180;
	return dxl_present_position_pan;
}

/*This implementation PTZ camera class (logitech) only support relative tilting.
**Only the sign (+, -, and 0) is important. It will be used as moving direction*/
int ViewrunPTZCamera::SetTilting()
{
	uint8_t dxl_error;
	int dxl_goal_position_tilt = (this->tiltSend + 180) * 4096 / 360;
	int dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_GOAL_POSITION, dxl_goal_position_tilt, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}

	return 0;
}

//This implementation PTZ camera class (logitech) doesn't support this method. It will just send 0
int ViewrunPTZCamera::GetTilting()
{
	// Read present position
	uint8_t dxl_error;
	int dxl_present_position_tilt;
	int dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position_tilt, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		packetHandler->printRxPacketError(dxl_error);
	}

	dxl_present_position_tilt = (dxl_present_position_tilt * 360 / 4096) - 180;
	return dxl_present_position_tilt;
}

/*Set destination zoom value*/
int ViewrunPTZCamera::SetZooming()
{
	setVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, this->zoom, 0);
	return zoom;
}

/*Get current zoom value*/
long ViewrunPTZCamera::GetZooming()
{
	long min, max, SteppingDelta, currentValue, flags, defaultValue;
	getVideoSettingCamera(KSPROPERTY_CAMERACONTROL_ZOOM, min, max, SteppingDelta, currentValue, flags, defaultValue);
	return currentValue;
}

int ViewrunPTZCamera::getch() {
	return _getch();
}

int ViewrunPTZCamera::kbhit(void) {
	return _kbhit();
}

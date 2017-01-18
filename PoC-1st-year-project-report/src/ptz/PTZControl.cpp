#include "PTZControl.h"
#include "Poco/TaskNotification.h"

dynamixel::PortHandler * PTZControl::portHandler = nullptr;
dynamixel::PacketHandler * PTZControl::packetHandler = nullptr;
bool PTZControl::alreadyUsed = false;

PTZControl::PTZControl()
{
	if (!alreadyUsed)
	{
		// Initialize PortHandler instance
		// Set the port path
		// Get methods and members of PortHandlerWindows
		PTZControl::portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

		// Initialize PacketHandler instance
		// Set the protocol version
		// Get methods and members of Protocol2PacketHandler
		PTZControl::packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

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
		dxl_comm_result = PTZControl::packetHandler->write1ByteTxRx(PTZControl::portHandler, DXL_ID1, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
		dxl_comm_result = PTZControl::packetHandler->write1ByteTxRx(PTZControl::portHandler, DXL_ID2, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);

		if (dxl_comm_result != COMM_SUCCESS)
		{
			PTZControl::packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{
			PTZControl::packetHandler->printRxPacketError(dxl_error);
		}
		else
		{
			printf("Dynamixel has been successfully connected \n");
		}

		PTZControl::alreadyUsed = true;
	}
}


PTZControl::~PTZControl()
{
}


int PTZControl::SetPanning(int input) {
	uint8_t dxl_error;
	int dxl_goal_position_pan = (input + 180) * 4096 / 360;
	int dxl_comm_result = PTZControl::packetHandler->write4ByteTxRx(PTZControl::portHandler, DXL_ID1, ADDR_PRO_GOAL_POSITION, dxl_goal_position_pan, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		PTZControl::packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		PTZControl::packetHandler->printRxPacketError(dxl_error);
	}

	return 0;
}

int PTZControl::GetPanning() {
	uint8_t dxl_error;
	int dxl_present_position_pan;
	int dxl_comm_result = PTZControl::packetHandler->read4ByteTxRx(PTZControl::portHandler, DXL_ID1, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position_pan, &dxl_error);
	
	if (dxl_comm_result != COMM_SUCCESS)
	{
		PTZControl::packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		PTZControl::packetHandler->printRxPacketError(dxl_error);
	}
	/*
	do
	{
		// Read present position
		dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID1, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_goal_position_pan, &dxl_error);

		if (dxl_comm_result != COMM_SUCCESS)
		{
			packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{
			packetHandler->printRxPacketError(dxl_error);
		}

		printf("[ID1:%03d] GoalPos:%03d  PresPos:%03d\n", DXL_ID1, dxl_goal_position_pan, dxl_present_position_pan);

	} while ((abs(dxl_goal_position_pan - dxl_present_position_pan) > DXL_MOVING_STATUS_THRESHOLD));
	*/

	dxl_present_position_pan = (dxl_present_position_pan * 360 / 4096) - 180;
	return dxl_present_position_pan;
}

int PTZControl::SetTilting(int input) {
	uint8_t dxl_error;
	int dxl_goal_position_tilt = (input + 180) * 4096 / 360;
	int dxl_comm_result = PTZControl::packetHandler->write4ByteTxRx(PTZControl::portHandler, DXL_ID2, ADDR_PRO_GOAL_POSITION, dxl_goal_position_tilt, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		PTZControl::packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		PTZControl::packetHandler->printRxPacketError(dxl_error);
	}

	return 0;
}


int PTZControl::GetTilting() {
	// Read present position
	uint8_t dxl_error;
	int dxl_present_position_tilt;
	int dxl_comm_result = PTZControl::packetHandler->read4ByteTxRx(PTZControl::portHandler, DXL_ID2, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position_tilt, &dxl_error);

	if (dxl_comm_result != COMM_SUCCESS)
	{
		PTZControl::packetHandler->printTxRxResult(dxl_comm_result);
	}
	else if (dxl_error != 0)
	{
		PTZControl::packetHandler->printRxPacketError(dxl_error);
	}
	/*
	do
	{
		// Read present position
		dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position_tilt, &dxl_error);

		if (dxl_comm_result != COMM_SUCCESS)
		{
			packetHandler->printTxRxResult(dxl_comm_result);
		}
		else if (dxl_error != 0)
		{
			packetHandler->printRxPacketError(dxl_error);
		}

		printf("[ID2:%03d] GoalPos:%03d  PresPos:%03d\n", DXL_ID2, dxl_goal_position_tilt, dxl_present_position_tilt);

	} while ((abs(dxl_goal_position_tilt - dxl_present_position_tilt) > DXL_MOVING_STATUS_THRESHOLD));
	*/

	dxl_present_position_tilt = (dxl_present_position_tilt * 360 / 4096) - 180;
	return dxl_present_position_tilt;
}

int PTZControl::SetZooming(int zoom) {
	newZoom = zoom;
	setVideoSettingCamera(3, newZoom, 0);
	return 0;
}

long PTZControl::GetZooming() {
	getVideoSettingCamera(3, min, max, SteppingDelta, currentValue, flags, defaultValue);
	return currentValue;
}

int PTZControl::getch(){
	return _getch();
}

int PTZControl::kbhit(void){
	return _kbhit();
}

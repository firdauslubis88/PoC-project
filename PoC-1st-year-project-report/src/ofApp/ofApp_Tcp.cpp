#include "ofApp_Tcp.h"

void ofApp_Tcp::setup()
{
	/*
	bool connected = tcpClient.setup("localhost", 8888);
	if (connected)
	{
		const char* message = "GET /sign_in?lubis HTTP/1.0\r\n\r\n";
		tcpClient.send(message);
	}
	*/
}

void ofApp_Tcp::exit()
{
}

void ofApp_Tcp::update()
{
//	std::string message = tcpClient.receiveRaw();
}

void ofApp_Tcp::draw()
{
}

void ofApp_Tcp::keyPressed(int key)
{
	switch (key)
	{
	case 's': {
//		std::cout << "Press S" << std::endl;
	}
		break;
	default:
		break;
	}
}

void ofApp_Tcp::keyReleased(int key)
{
}

void ofApp_Tcp::mouseMoved(int x, int y)
{
}

void ofApp_Tcp::mouseDragged(int x, int y, int button)
{
}

void ofApp_Tcp::mousePressed(int x, int y, int button)
{
}

void ofApp_Tcp::mouseReleased(int x, int y, int button)
{
}

void ofApp_Tcp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
}

void ofApp_Tcp::mouseEntered(int x, int y)
{
}

void ofApp_Tcp::mouseExited(int x, int y)
{
}

void ofApp_Tcp::windowResized(int w, int h)
{
}

void ofApp_Tcp::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp_Tcp::gotMessage(ofMessage msg)
{
}

void ofApp_Tcp::onToggle(const void * sender)
{
}
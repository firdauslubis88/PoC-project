#include "ofApp_webrtc.h"
#include "opencv2/highgui.hpp"

/*
#include "flagdefs.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
*/

void ofApp_webrtc::setup()
{
	shared_ptr<WebRTC> tempWebRTC(new WebRTC);
	webRTC = tempWebRTC;
	webRTC->InitWebRTC();
	/*
	VIDEO_WIDTH = ofGetWidth();
	VIDEO_HEIGHT = ofGetHeight();

	shared_ptr<WebRTC> tempWebRTC(new WebRTC);
	webRTC = tempWebRTC;
	webRTC->InitWebRTC();
	tempCvImage.allocate(640, 480);
	tempImage.allocate(640, 480, OF_IMAGE_COLOR);
	*/
	panel.setup();
	loginButton.setup("LOGIN");
	logoutButton.setup("LOGOUT");
	loginButton.addListener(this, &ofApp_webrtc::Login);
	logoutButton.addListener(this, &ofApp_webrtc::Logout);
	panel.add(&loginButton);

	prevConnectedIndicator = false;
}

void ofApp_webrtc::exit()
{
	webRTC->CloseWebRTC();
}

void ofApp_webrtc::update()
{
	webRTC->UpdateWebRTC();
	if (webRTC->connectedToServer != prevConnectedIndicator)
	{
		if (webRTC->connectedToServer)
		{
			//		webRTC->StartLogout();
			panel.clear();
			ofPeers = webRTC->GetPeersList();
			peersNameToggle = new ofxButton[ofPeers.size()];
//			std::cout << "UPDATE" << std::endl;
			int i = 0;
			for (Peers::iterator it = ofPeers.begin(); it != ofPeers.end(); ++it)
			{
//				std::cout << it->first << " " << it->second << std::endl;
				peersNameToggle[i].setup(it->second);
				peersNameToggle[i].addListener(this, &ofApp_webrtc::ConnectToPeerSelected);
				ofPeersInt[&peersNameToggle[i]] = it->first;
				panel.add(&peersNameToggle[i]);
			}

			panel.add(&logoutButton);
			prevConnectedIndicator = true;
		}
		else
		{
			//		webRTC->StartLogin();
			panel.clear();
			panel.add(&loginButton);
			ofPeersInt.clear();
			delete[] peersNameToggle;
		}
		prevConnectedIndicator = webRTC->connectedToServer;
	}
}

void ofApp_webrtc::draw()
{
	/*
	if (webRTC->public_image_recording_indicator)
	{
		cv::Mat tempMat = cv::Mat(webRTC->public_image_height, webRTC->public_image_width, CV_8UC4, (void*)webRTC->public_image);
		cv::cvtColor(tempMat, tempMatDst, cv::COLOR_RGBA2BGR);
		IplImage temp = tempMatDst;
		IplImage* pTemp = &temp;
		if (pTemp != NULL)
		{
			tempCvImage = pTemp;
		}
		tempImage.setFromPixels(tempCvImage.getPixels());
		tempImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else
	{
	}
	*/
	panel.draw();
}

void ofApp_webrtc::keyPressed(int key)
{
	webRTC->ofMessage = key;

}

void ofApp_webrtc::keyReleased(int key)
{
}

void ofApp_webrtc::mouseMoved(int x, int y)
{
}

void ofApp_webrtc::mouseDragged(int x, int y, int button)
{
}

void ofApp_webrtc::mousePressed(int x, int y, int button)
{
}

void ofApp_webrtc::mouseReleased(int x, int y, int button)
{
}

void ofApp_webrtc::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
}

void ofApp_webrtc::mouseEntered(int x, int y)
{
}

void ofApp_webrtc::mouseExited(int x, int y)
{
}

void ofApp_webrtc::windowResized(int w, int h)
{
}

void ofApp_webrtc::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp_webrtc::gotMessage(ofMessage msg)
{
}

void ofApp_webrtc::onToggle(const void * sender)
{
}

void ofApp_webrtc::Login()
{
	if (!webRTC->Is_Connected())
	{
		webRTC->StartLogin();
	}
}

void ofApp_webrtc::Logout()
{
	if (webRTC->Is_Connected())
	{
		webRTC->StartLogout();
	}
}
void ofApp_webrtc::ConnectToPeerSelected(const void * sender)
{
	ofxButton * p = (ofxButton *)sender;
	PeersInt::iterator ofPeerIdIt = ofPeersInt.find(p);
	int ofPeerId = ofPeerIdIt->second;
	webRTC->ConnectToPeer(ofPeerId);

//	std::cout << "You click peer number:\t" << ofPeerId << std::endl;
}
/*
void ofApp_webrtc::RegisterObserver(MainWndCallback * callback)
{
	callback_ = callback;
	if (auto_connect_) {
		customAutoConnectToServer();
	}

}

void ofApp_webrtc::customAutoConnectToServer() {
	if (!callback_)
		return;
	int port = this->port_.length() ? atoi(this->port_.c_str()) : 0;
	callback_->StartLogin(this->server_, port);
}

bool ofApp_webrtc::IsWindow() {
	return wnd_ && ::IsWindow(wnd_) != FALSE;
}

bool ofApp_webrtc::PreTranslateMessage(MSG* msg) {
	bool ret = false;
	if (msg->hwnd == NULL && msg->message == UI_THREAD_CALLBACK) {
		callback_->UIThreadCallback(static_cast<int>(msg->wParam),
			reinterpret_cast<void*>(msg->lParam));
		ret = true;
	}
	return ret;
}

void ofApp_webrtc::SwitchToPeerList(const Peers& peers) {
	if (peers.begin() != peers.end()) {
		// Get the number of items in the list
		//Do something here...
	}
}

void ofApp_webrtc::QueueUIThreadCallback(int msg_id, void* data) {
	::PostThreadMessage(ui_thread_id_, UI_THREAD_CALLBACK,
		static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(data));
}
*/
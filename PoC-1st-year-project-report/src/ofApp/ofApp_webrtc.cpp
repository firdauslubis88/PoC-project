#include "ofApp_webrtc.h"

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
}

void ofApp_webrtc::exit()
{
	webRTC->CloseWebRTC();
}

void ofApp_webrtc::update()
{
	webRTC->UpdateWebRTC();
}

void ofApp_webrtc::draw()
{
	ofSetColor(0, 0, 255);    //set te color to blue
	ofDrawRectangle(10, 10, 100, 100);
}

void ofApp_webrtc::keyPressed(int key)
{
//	std::cout << "This is the key in int:\t" << key << endl;
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
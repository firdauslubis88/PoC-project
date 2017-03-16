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
	VIDEO_WIDTH = ofGetWidth();
	VIDEO_HEIGHT = ofGetHeight();

	shared_ptr<WebRTC> tempWebRTC(new WebRTC);
	webRTC = tempWebRTC;
	webRTC->InitWebRTC();
	tempCvImage.allocate(640, 480);
	tempImage.allocate(640, 480, OF_IMAGE_COLOR);
//	cv::namedWindow("test");
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
	if (webRTC->public_image_recording_indicator)
	{
		/*
		std::cout << "width:\t" << webRTC->public_image_width << endl;
		std::cout << "height:\t" << webRTC->public_image_height << endl;
		std::cout << "size:\t" << webRTC->public_image_size << endl;
		std::cout << "bitcount:\t" << webRTC->public_bit_count << endl;
		*/
		cv::Mat tempMat = cv::Mat(webRTC->public_image_height, webRTC->public_image_width, CV_8UC4, (void*)webRTC->public_image);
		cv::cvtColor(tempMat, tempMatDst, cv::COLOR_RGBA2BGR);
//		cv::imshow("test", tempMatDst);
		IplImage temp = tempMatDst;
		IplImage* pTemp = &temp;
		if (pTemp != NULL)
		{
			tempCvImage = pTemp;
		}
//		delete[] webRTC->public_image;
//		tempCvImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
		tempImage.setFromPixels(tempCvImage.getPixels());
		tempImage.draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	else
	{
		/*
		ofSetColor(0, 0, 255);    //set te color to blue
		ofDrawRectangle(10, 10, 100, 100);
		ofSetColor(255, 255, 255);
		*/
	}
	/*
	ofSetColor(0, 0, 255);    //set te color to blue
	ofDrawRectangle(10, 10, 100, 100);
	*/

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
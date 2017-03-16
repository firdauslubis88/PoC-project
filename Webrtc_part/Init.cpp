#include <iostream>
#include "Init.h"

WebRTC::WebRTC():wnd("localhost", 8888, true, false)
{

}

WebRTC::~WebRTC()
{

}

int WebRTC::InitWebRTC() {
	 rtc::AsyncResolver* localResolver = new rtc::AsyncResolver();

	rtc::EnsureWinsockInit();
	rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

	if (!wnd.Create()) {
		RTC_NOTREACHED();
		return -1;
	}
//	std::cout << "wnd 1st thread id:\t" << wnd.ui_thread_id_ << std::endl;

	rtc::InitializeSSL();

	rtc::scoped_refptr<Conductor> tempConductor(
		new rtc::RefCountedObject<Conductor>(&client, &wnd));
	conductor = tempConductor;
	/*
	// Main loop.
	while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (!wnd.PreTranslateMessage(&msg)) {
//			std::cout << msg.message << std::endl;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	rtc::CleanupSSL();
	*/
	return 0;
}

INIT_API int WebRTC::CloseWebRTC()
{
	rtc::CleanupSSL();
	return 0;
}

INIT_API int WebRTC::UpdateWebRTC()
{
	if ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (!wnd.PreTranslateMessage(&msg)) {
//			std::cout << msg.message << std::endl;
//			std::cout << check << std::endl;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			check++;
		}
//		std::cout << "of thread id:\t" << ::GetCurrentThreadId() << std::endl;
//		std::cout << "wnd thread id:\t" << wnd.ui_thread_id_ << std::endl;
	}
	this->public_image = wnd.public_image;
	this->public_image_width = wnd.public_image_width;
	this->public_image_height = wnd.public_image_height;
	this->public_image_size = wnd.public_image_size;
	this->public_bit_count = wnd.public_bit_count;
	this->public_image_recording_indicator = wnd.public_image_recording_indicator;
//	std::cout << "This is the key in int:\t" << ofMessage << std::endl;
	/*
	if (!wnd.PreTranslateMessage(ofMessage)) {
		ofMessage = 0;
		check++;
	}
	*/
	return 0;
}

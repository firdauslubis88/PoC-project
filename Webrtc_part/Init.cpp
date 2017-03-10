#include "Init.h"

WebRTC::WebRTC()
{

}

int WebRTC::Init() {
	rtc::EnsureWinsockInit();
	rtc::Win32Thread w32_thread;
	rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

	//  MainWnd wnd(FLAG_server, FLAG_port, FLAG_autoconnect, FLAG_autocall);
	MainWnd wnd("localhost", 8888, true, false);
	if (!wnd.Create()) {
		RTC_NOTREACHED();
		return -1;
	}

	rtc::InitializeSSL();
	PeerConnectionClient client;

	rtc::scoped_refptr<Conductor> conductor(
		new rtc::RefCountedObject<Conductor>(&client, &wnd));

	// Main loop.
	MSG msg;
	BOOL gm;

	while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (!wnd.PreTranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	rtc::CleanupSSL();
	return 0;
}

#pragma once

#ifdef INIT_EXPORTS  
#define INIT_API __declspec(dllexport)   
#else  
#define INIT_API __declspec(dllimport)   
#endif  
#include "conductor.h"
/*
#include "flagdefs.h"
*/
#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
class WebRTC {
public:
	INIT_API WebRTC();
	INIT_API ~WebRTC();
	INIT_API int InitWebRTC();
	INIT_API int CloseWebRTC();
	INIT_API int UpdateWebRTC();
	int ofMessage;

private:
	rtc::Win32Thread w32_thread;
	MainWnd wnd;
	PeerConnectionClient client;
	rtc::scoped_refptr<Conductor> conductor;
	MSG msg;
	BOOL gm;
	int check;
};

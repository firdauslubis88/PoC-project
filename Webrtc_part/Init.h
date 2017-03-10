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
#include "main_wnd.h"
#include "peer_connection_client.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"
class WebRTC {
public:
	INIT_API WebRTC();
	INIT_API int Init();
};

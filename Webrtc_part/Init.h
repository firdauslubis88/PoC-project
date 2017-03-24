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

#include "custom_peer_connection_client.h"

enum UI {
	CONNECT_TO_SERVER,
	LIST_PEERS,
	STREAMING,
};

/*
virtual void SwitchToConnectUI() = 0;
virtual void SwitchToPeerList(const Peers& peers) = 0;
virtual void SwitchToStreamingUI() = 0;
*/
/*
virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video) = 0;
virtual void StopLocalRenderer() = 0;
virtual void StartRemoteRenderer(
	webrtc::VideoTrackInterface* remote_video) = 0;
virtual void StopRemoteRenderer() = 0;
*/

enum ofWindowSwitchTo {
	SWITCH_TO_CONNECT_UI,
	SWITCH_TO_PEER_LIST,
	SWITCH_TO_STREAMING_UI
};


class WebRTC: 
	public CustomPeerConnectionClientObserver, 
	public webrtc::PeerConnectionObserver,
	public webrtc::CreateSessionDescriptionObserver	{
public:
	INIT_API WebRTC();
	INIT_API ~WebRTC();
	INIT_API int InitWebRTC();
	INIT_API int CloseWebRTC();
	INIT_API int UpdateWebRTC();
	INIT_API void StartLogin();
	INIT_API void DisconnectFromServer();
	INIT_API void ConnectToPeer(int peer_id);
	INIT_API void DisconnectFromCurrentPeer();

	INIT_API void StartLogout();
	INIT_API bool Is_Connected();
	INIT_API Peers GetPeersList();
	int ofMessage;
	uint8_t* public_image;
	int public_image_width, public_image_height;
	DWORD public_image_size;
	WORD public_bit_count;
	bool public_image_recording_indicator;
	void Close();

	Peers peers;

	//CustomPeerConnectionClient callback implementation
	void OnSignedIn();
	void OnSignedOut();
	void OnDisconnected();
	void OnPeerConnected(int id, const std::string& name);
	void OnPeerDisconnected(int id);
	void OnMessageFromPeer(int peer_id, const std::string& message);
	void OnMessageSent(int err);
	void OnServerConnectionFailure();
	bool connectedToServer;

	//
	// PeerConnectionObserver implementation.
	void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
	void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
	void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {};
	void OnRenegotiationNeeded() {};
	void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {};
	void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {};
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
	void OnIceConnectionReceivingChange(bool receiving) {};
	void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) {};
	void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) {};

	// CreateSessionDescriptionObserver implementation.
	void OnSuccess(webrtc::SessionDescriptionInterface* desc);
	void OnFailure(const std::string& error);
	void OnSetSuccess() {};
	void OnSetFailure(const std::string& error) {};
	int AddRef() const { return 0; }
	int Release() const { return 0; }

protected:
	bool InitializePeerConnection();
	bool ReinitializePeerConnectionForLoopback();
	bool CreatePeerConnection(bool dtls);
	void DeletePeerConnection();
	void EnsureStreamingUI();
	void AddStreams();
	// Send a message to the remote peer.
	void SendMessage(const std::string& json_object);
	std::unique_ptr<cricket::VideoCapturer> OpenVideoCaptureDevice();


private:
	class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
	public:
		VideoRenderer(int width, int height,
			webrtc::VideoTrackInterface* track_to_render);
		virtual ~VideoRenderer();

		void Lock() {
			::EnterCriticalSection(&buffer_lock_);
		}

		void Unlock() {
			::LeaveCriticalSection(&buffer_lock_);
		}

		// VideoSinkInterface implementation
		void OnFrame(const webrtc::VideoFrame& frame) override;

		const BITMAPINFO& bmi() const { return bmi_; }
		const uint8_t* image() const { return image_.get(); }

	protected:
		void SetSize(int width, int height);

		enum {
			SET_SIZE,
			RENDER_FRAME,
		};

//		HWND wnd_;
		BITMAPINFO bmi_;
		std::unique_ptr<uint8_t[]> image_;
		CRITICAL_SECTION buffer_lock_;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
	};
	// A little helper class to make sure we always to proper locking and
	// unlocking when working with VideoRenderer buffers.
	template <typename T>
	class AutoLock {
	public:
		explicit AutoLock(T* obj) : obj_(obj) { obj_->Lock(); }
		~AutoLock() { obj_->Unlock(); }
	protected:
		T* obj_;
	};

	void StartLocalRenderer(webrtc::VideoTrackInterface* local_video);
	void StopLocalRenderer();
	void StartRemoteRenderer(webrtc::VideoTrackInterface* remote_video);
	void StopRemoteRenderer();

	std::unique_ptr<VideoRenderer> local_renderer_;
	std::unique_ptr<VideoRenderer> remote_renderer_;
	rtc::Win32Thread w32_thread;
	MainWnd wnd;
//	PeerConnectionClient client;
//	rtc::scoped_refptr<Conductor> conductor;
	MSG msg;
	BOOL gm;
	int check;

	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
		peer_connection_factory_;
	CustomPeerConnectionClient* custom_client;
	int peer_id_;
	std::deque<std::string*> pending_messages_;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >
		active_streams_;
	bool loopback_;
	ofWindowSwitchTo CURRENT_UI;
	UI ui_;
};

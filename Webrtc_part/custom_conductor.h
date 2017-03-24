#pragma once

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "peer_connection_client.h"

class DummySetSessionDescriptionObserver
	: public webrtc::SetSessionDescriptionObserver {
public:
	static DummySetSessionDescriptionObserver* Create() {
		return
			new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
	}
	virtual void OnSuccess() {
		LOG(INFO) << __FUNCTION__;
	}
	virtual void OnFailure(const std::string& error) {
		LOG(INFO) << __FUNCTION__ << " " << error;
	}

protected:
	DummySetSessionDescriptionObserver() {}
	~DummySetSessionDescriptionObserver() {}
};
/*
class OfWindowCallback {
public:
	virtual void StartLogin(const std::string& server, int port) = 0;
	virtual void DisconnectFromServer() = 0;
	virtual void ConnectToPeer(int peer_id) = 0;
	virtual void DisconnectFromCurrentPeer() = 0;
	virtual void UIThreadCallback(int msg_id, void* data) = 0;
	virtual void Close() = 0;
	virtual bool test() = 0;

protected:
	virtual ~OfWindowCallback() {}
};
*/
class custom_conductor
{
public:
	static custom_conductor* GetInstance();

	bool connection_active() const;
	virtual void Close();

private:
	custom_conductor();
	~custom_conductor();
	// PeerConnection Observer
	class PCObserver : public webrtc::PeerConnectionObserver
	{
		void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
		void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);
		void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
		void OnRenegotiationNeeded();
		void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state);
		void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state);
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
		void OnIceConnectionReceivingChange(bool receiving);
		void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates);
		void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state);
	};

	// SdpObserver
	class SdpObserver : public webrtc::CreateSessionDescriptionObserver
	{
		// Create Observer
		void OnSuccess(webrtc::SessionDescriptionInterface* desc);
		void OnFailure(const std::string& error);
		void OnSetSuccess();
		void OnSetFailure(const std::string& error);
		int AddRef() const { return 0; }
		int Release() const { return 0; }
	};

	// PCCObserver
	class PCCObserver : public PeerConnectionClientObserver
	{
		// Create Observer
		void OnSignedIn();
		void OnDisconnected();
		void OnPeerConnected(int id, const std::string& name);
		void OnPeerDisconnected(int id);
		void OnMessageFromPeer(int peer_id, const std::string& message);
		void OnMessageSent(int err);
		void OnServerConnectionFailure();
	};
	/*
	// OfWindowObserver
	class OfWindowObserver : public OfWindowCallback
	{
		// Create Observer
		void StartLogin(const std::string& server, int port);
		void DisconnectFromServer();
		void ConnectToPeer(int peer_id);
		void DisconnectFromCurrentPeer();
		void Close();
	};
	*/
	class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
	public:
		VideoRenderer(int width, int height, webrtc::VideoTrackInterface* track_to_render);
		~VideoRenderer();

		void Lock() {
			::EnterCriticalSection(&buffer_lock_);
		}

		void Unlock() {
			::LeaveCriticalSection(&buffer_lock_);
		}

		// VideoSinkInterface implementation
		void OnFrame(const webrtc::VideoFrame& frame);

		const BITMAPINFO& bmi() const { return bmi_; }
		const uint8_t* image() const { return image_.get(); }

	protected:
		void SetSize(int width, int height);

		enum {
			SET_SIZE,
			RENDER_FRAME,
		};

		BITMAPINFO bmi_;
		std::unique_ptr<uint8_t[]> image_;
		CRITICAL_SECTION buffer_lock_;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
	};

	static custom_conductor* instance;

	PCObserver* peer_connection_observer_;
	SdpObserver* sdp_observer_;
	PCCObserver* peer_connection_client_observer;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
		peer_connection_factory_;
	PeerConnectionClient* client_;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >
		active_streams_;
	std::string server_;

	std::unique_ptr<VideoRenderer> local_renderer_;
	std::unique_ptr<VideoRenderer> remote_renderer_;

	int peer_id_;
};

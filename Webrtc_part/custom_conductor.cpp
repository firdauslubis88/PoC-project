#include "custom_conductor.h"

custom_conductor* custom_conductor::instance = nullptr;

custom_conductor::custom_conductor()
{
	peer_id_ = -1;
	peer_connection_observer_ = new PCObserver();
	sdp_observer_ = new SdpObserver();
	client_ = new PeerConnectionClient();
	peer_connection_client_observer = new PCCObserver();
	client_->RegisterObserver(peer_connection_client_observer);
}


custom_conductor::~custom_conductor()
{
}

custom_conductor * custom_conductor::GetInstance()
{
	if (instance == NULL) instance = new custom_conductor();
	return instance;
}

bool custom_conductor::connection_active() const
{
	return peer_connection_.get() != NULL;
}

void custom_conductor::Close()
{
	client_->SignOut();
}

void custom_conductor::PCObserver::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
	webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
	if (!tracks.empty()) {
		webrtc::VideoTrackInterface* track = tracks[0];
		instance->remote_renderer_.reset(new VideoRenderer(1, 1, track));
	}
	stream->Release();
}

void custom_conductor::PCObserver::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
	stream->Release();
}

void custom_conductor::PCObserver::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
}

void custom_conductor::PCObserver::OnRenegotiationNeeded()
{
}

void custom_conductor::PCObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
}

void custom_conductor::PCObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
}

void custom_conductor::PCObserver::OnIceCandidate(const webrtc::IceCandidateInterface * candidate)
{
	if (!instance->peer_connection_->AddIceCandidate(candidate)) {
		LOG(WARNING) << "Failed to apply the received candidate";
	}

}

void custom_conductor::PCObserver::OnIceConnectionReceivingChange(bool receiving)
{
}

void custom_conductor::PCObserver::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)
{
}

void custom_conductor::PCObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
}

void custom_conductor::SdpObserver::OnSuccess(webrtc::SessionDescriptionInterface * desc)
{
	instance->peer_connection_->SetLocalDescription(
		DummySetSessionDescriptionObserver::Create(), desc);
}

void custom_conductor::SdpObserver::OnFailure(const std::string & error)
{
	LOG(LERROR) << error;
}

void custom_conductor::SdpObserver::OnSetSuccess()
{
}

void custom_conductor::SdpObserver::OnSetFailure(const std::string & error)
{
}

void custom_conductor::PCCObserver::OnSignedIn()
{

}

void custom_conductor::PCCObserver::OnDisconnected()
{
}

void custom_conductor::PCCObserver::OnPeerConnected(int id, const std::string & name)
{
}

void custom_conductor::PCCObserver::OnPeerDisconnected(int id)
{
}

void custom_conductor::PCCObserver::OnMessageFromPeer(int peer_id, const std::string & message)
{
}

void custom_conductor::PCCObserver::OnMessageSent(int err)
{
}

void custom_conductor::PCCObserver::OnServerConnectionFailure()
{
}

custom_conductor::VideoRenderer::~VideoRenderer()
{

}

void custom_conductor::VideoRenderer::OnFrame(const webrtc::VideoFrame& frame)
{

}

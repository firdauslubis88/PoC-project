#include <iostream>
#include "Init.h"
#include "defaults.h"

#include "webrtc/api/test/fakeconstraints.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/json.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"

#include "libyuv/convert_argb.h"
#include "webrtc/api/video/i420_buffer.h"

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

#define DTLS_ON  true
#define DTLS_OFF false

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

WebRTC::WebRTC():wnd("localhost", 8888, true, false)
{

}

WebRTC::~WebRTC()
{
	RTC_DCHECK(peer_connection_.get() == NULL);
}

int WebRTC::InitWebRTC() {
	 rtc::AsyncResolver* localResolver = new rtc::AsyncResolver();

	rtc::EnsureWinsockInit();
	rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

	rtc::InitializeSSL();
	custom_client = new CustomPeerConnectionClient();
	custom_client->RegisterObserver(this);
	peer_id_ = -1;
	loopback_ = false;
	connectedToServer = false;
//	StartLogin();
/*
	rtc::scoped_refptr<Conductor> tempConductor(
		new rtc::RefCountedObject<Conductor>(&client, &wnd));
	conductor = tempConductor;
*/

	return 0;
}

INIT_API int WebRTC::CloseWebRTC()
{
	Close();
	rtc::CleanupSSL();
	return 0;
}

INIT_API int WebRTC::UpdateWebRTC()
{
	/*
	if ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (!wnd.PreTranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			check++;
		}
	}
	this->public_image = wnd.public_image;
	this->public_image_width = wnd.public_image_width;
	this->public_image_height = wnd.public_image_height;
	this->public_image_size = wnd.public_image_size;
	this->public_bit_count = wnd.public_bit_count;
	this->public_image_recording_indicator = wnd.public_image_recording_indicator;
	*/

	return 0;
}

INIT_API void WebRTC::StartLogin()
{
	if (custom_client->is_connected())
		return;
	std::string server("161.122.38.93");
	int port(8888);
	custom_client->Connect(server, port, "lubis");
}

INIT_API void WebRTC::DisconnectFromServer()
{
	if (custom_client->is_connected())
	{
		std::cout << "DISCONNECT FROM SERVER" << std::endl; //this must be written?? weird bugs....
		custom_client->SignOut();
	}
}

INIT_API void WebRTC::ConnectToPeer(int peer_id)
{
	RTC_DCHECK(peer_id_ == -1);
	RTC_DCHECK(peer_id != -1);

	if (peer_connection_.get()) {
		std::cout << "We only support connecting to one peer at a time" << std::endl;
	return;
	}

	if (InitializePeerConnection()) {
		peer_id_ = peer_id;
		peer_connection_->CreateOffer(this, NULL);
	}
	else {
		std::cout << "Failed to initialize PeerConnection" << std::endl;
	}
}

INIT_API void WebRTC::DisconnectFromCurrentPeer()
{
	if (peer_connection_.get()) {
		custom_client->SendHangUp(peer_id_);
		DeletePeerConnection();
	}
	/*
	if (main_wnd_->IsWindow())
		main_wnd_->SwitchToPeerList(client_->peers());
		*/
	ui_ = LIST_PEERS;
}

INIT_API void WebRTC::StartLogout()
{
	Close();
}

INIT_API bool WebRTC::Is_Connected()
{
	return custom_client->is_connected();
}

void WebRTC::Close()
{
	/*
	if (custom_client->is_connected())
	{
		custom_client->SignOut();
		std::cout << "CLOSED" << std::endl; //this must be written?? weird bugs....
	}
	*/
	if (ui_ == STREAMING) {
		DisconnectFromCurrentPeer();
	}
	else {
		DisconnectFromServer();
	}
}

void WebRTC::OnSignedIn()
{
	connectedToServer = true;
}

void WebRTC::OnSignedOut()
{
	connectedToServer = false;
}

void WebRTC::OnDisconnected()
{
	DeletePeerConnection();

	/*
	if (main_wnd_->IsWindow())
		main_wnd_->SwitchToConnectUI();
		*/
	ui_ = CONNECT_TO_SERVER;
}

void WebRTC::OnPeerConnected(int id, const std::string & name)
{
	/*
	if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
		main_wnd_->SwitchToPeerList(client_->peers());
		*/
	if (ui_ == LIST_PEERS)
	{
		ui_ = LIST_PEERS;
	}
}

void WebRTC::OnPeerDisconnected(int id)
{
	if (id == peer_id_) {
		LOG(INFO) << "Our peer disconnected";
//		main_wnd_->QueueUIThreadCallback(PEER_CONNECTION_CLOSED, NULL);
		DeletePeerConnection();

		RTC_DCHECK(active_streams_.empty());
		DisconnectFromServer();

		/*
		if (main_wnd_->IsWindow()) {
			if (client_->is_connected()) {
				main_wnd_->SwitchToPeerList(client_->peers());
			}
			else {
				main_wnd_->SwitchToConnectUI();
			}
		}
		else {
			DisconnectFromServer();
		}
		*/
		if (custom_client->is_connected())
		{
			ui_ = LIST_PEERS;
		}
		else
		{
			ui_ = CONNECT_TO_SERVER;
		}
	}
	else {
		// Refresh the list if we're showing it.
		/*
		if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
			main_wnd_->SwitchToPeerList(client_->peers());
			*/
		if (ui_ == LIST_PEERS)
		{
			ui_ = LIST_PEERS;
		}
	}
}

void WebRTC::OnMessageFromPeer(int peer_id, const std::string & message)
{
	RTC_DCHECK(peer_id_ == peer_id || peer_id_ == -1);
	RTC_DCHECK(!message.empty());

	if (!peer_connection_.get()) {
		RTC_DCHECK(peer_id_ == -1);
		peer_id_ = peer_id;

		if (!InitializePeerConnection()) {
			LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
			custom_client->SignOut();
			return;
		}
	}
	else if (peer_id != peer_id_) {
		RTC_DCHECK(peer_id_ != -1);
		LOG(WARNING) << "Received a message from unknown peer while already in a "
			"conversation with a different peer.";
		return;
	}

	Json::Reader reader;
	Json::Value jmessage;
	if (!reader.parse(message, jmessage)) {
		LOG(WARNING) << "Received unknown message. " << message;
		return;
	}
	std::string type;
	std::string json_object;

	rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);
	if (!type.empty()) {
		if (type == "offer-loopback") {
			// This is a loopback call.
			// Recreate the peerconnection with DTLS disabled.
			if (!ReinitializePeerConnectionForLoopback()) {
				LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
				DeletePeerConnection();
				custom_client->SignOut();
			}
			return;
		}

		std::string sdp;
		if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
			&sdp)) {
			LOG(WARNING) << "Can't parse received session description message.";
			return;
		}
		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* session_description(
			webrtc::CreateSessionDescription(type, sdp, &error));
		if (!session_description) {
			LOG(WARNING) << "Can't parse received session description message. "
				<< "SdpParseError was: " << error.description;
			return;
		}
		LOG(INFO) << " Received session description :" << message;
		peer_connection_->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(), session_description);
		if (session_description->type() ==
			webrtc::SessionDescriptionInterface::kOffer) {
			peer_connection_->CreateAnswer(this, NULL);
		}
		return;
	}
	else {
		std::string sdp_mid;
		int sdp_mlineindex = 0;
		std::string sdp;
		if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName,
			&sdp_mid) ||
			!rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName,
				&sdp_mlineindex) ||
			!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) {
			LOG(WARNING) << "Can't parse received message.";
			return;
		}
		webrtc::SdpParseError error;
		std::unique_ptr<webrtc::IceCandidateInterface> candidate(
			webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
		if (!candidate.get()) {
			LOG(WARNING) << "Can't parse received candidate message. "
				<< "SdpParseError was: " << error.description;
			return;
		}
		if (!peer_connection_->AddIceCandidate(candidate.get())) {
			LOG(WARNING) << "Failed to apply the received candidate";
			return;
		}
		LOG(INFO) << " Received candidate :" << message;
		return;
	}
}

void WebRTC::OnMessageSent(int err)
{
//	main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, NULL);

}

void WebRTC::OnServerConnectionFailure()
{
	connectedToServer = false;
	std::cout << "Failed To Connect to Server" << std::endl;
}

void WebRTC::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
	LOG(INFO) << __FUNCTION__ << " " << stream->label();
//	stream.release();
	//  main_wnd_->MessageBox("Succeed", "Succeed to OnAddStream", true);
//	main_wnd_->QueueUIThreadCallback(NEW_STREAM_ADDED, stream.release());
	webrtc::MediaStreamInterface* tempStream  = reinterpret_cast<webrtc::MediaStreamInterface*>(stream.release());
	webrtc::VideoTrackVector tracks = tempStream->GetVideoTracks();
	if (!tracks.empty()) {
		webrtc::VideoTrackInterface* track = tracks[0];
		StartRemoteRenderer(track);
	}
	tempStream->Release();
}

void WebRTC::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
	webrtc::MediaStreamInterface* tempStream = reinterpret_cast<webrtc::MediaStreamInterface*>(stream.release());
	tempStream->Release();
}

void WebRTC::OnIceCandidate(const webrtc::IceCandidateInterface * candidate)
{
	if (loopback_) {
		if (!peer_connection_->AddIceCandidate(candidate)) {
			LOG(WARNING) << "Failed to apply the received candidate";
		}
		return;
	}

	Json::StyledWriter writer;
	Json::Value jmessage;

	jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
	jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
	std::string sdp;
	if (!candidate->ToString(&sdp)) {
		LOG(LS_ERROR) << "Failed to serialize candidate";
		return;
	}
	jmessage[kCandidateSdpName] = sdp;
	SendMessage(writer.write(jmessage));
}

void WebRTC::OnSuccess(webrtc::SessionDescriptionInterface * desc)
{
	peer_connection_->SetLocalDescription(
		DummySetSessionDescriptionObserver::Create(), desc);

	std::string sdp;
	desc->ToString(&sdp);

	// For loopback test. To save some connecting delay.
	if (loopback_) {
		// Replace message type from "offer" to "answer"
		webrtc::SessionDescriptionInterface* session_description(
			webrtc::CreateSessionDescription("answer", sdp, nullptr));
		peer_connection_->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(), session_description);
		return;
	}

	Json::StyledWriter writer;
	Json::Value jmessage;
	jmessage[kSessionDescriptionTypeName] = desc->type();
	jmessage[kSessionDescriptionSdpName] = sdp;
	SendMessage(writer.write(jmessage));
}

void WebRTC::OnFailure(const std::string & error)
{
}

bool WebRTC::InitializePeerConnection()
{
	RTC_DCHECK(peer_connection_factory_.get() == NULL);
	RTC_DCHECK(peer_connection_.get() == NULL);

	peer_connection_factory_ = webrtc::CreatePeerConnectionFactory();

	if (!peer_connection_factory_.get()) {
		std::cout << "Failed to initialize PeerConnectionFactory" << std::endl;
		DeletePeerConnection();
		return false;
	}

	if (!CreatePeerConnection(DTLS_ON)) {
		std::cout << "CreatePeerConnection failed" << std::endl;
		DeletePeerConnection();
	}
	AddStreams();
	return peer_connection_.get() != NULL;
}

bool WebRTC::ReinitializePeerConnectionForLoopback()
{
	loopback_ = true;
	rtc::scoped_refptr<webrtc::StreamCollectionInterface> streams(
		peer_connection_->local_streams());
	peer_connection_ = NULL;
	if (CreatePeerConnection(DTLS_OFF)) {
		for (size_t i = 0; i < streams->count(); ++i)
			peer_connection_->AddStream(streams->at(i));
		peer_connection_->CreateOffer(this, NULL);
	}
	return peer_connection_.get() != NULL;
}

bool WebRTC::CreatePeerConnection(bool dtls)
{
	RTC_DCHECK(peer_connection_factory_.get() != NULL);
	RTC_DCHECK(peer_connection_.get() == NULL);

	webrtc::PeerConnectionInterface::RTCConfiguration config;
	webrtc::PeerConnectionInterface::IceServer server;
	server.uri = GetPeerConnectionString();
	config.servers.push_back(server);

	webrtc::FakeConstraints constraints;
	if (dtls) {
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
			"true");
	}
	else {
		constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
			"false");
	}

	peer_connection_ = peer_connection_factory_->CreatePeerConnection(
		config, &constraints, NULL, NULL, this);
	return peer_connection_.get() != NULL;
}

void WebRTC::DeletePeerConnection()
{
	peer_connection_ = NULL;
	active_streams_.clear();
	StopLocalRenderer();
	StopRemoteRenderer();
	peer_connection_factory_ = NULL;
	peer_id_ = -1;
	loopback_ = false;
}

void WebRTC::EnsureStreamingUI()
{
	RTC_DCHECK(peer_connection_.get() != NULL);
	/*
	if (main_wnd_->IsWindow()) {
		if (main_wnd_->current_ui() != MainWindow::STREAMING)
			main_wnd_->SwitchToStreamingUI();
	}
	*/
	if (ui_ != STREAMING)
	{
		ui_ = STREAMING;
	}
}

void WebRTC::AddStreams()
{
	if (active_streams_.find(kStreamLabel) != active_streams_.end())
		return;  // Already added.

	rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
		peer_connection_factory_->CreateAudioTrack(
			kAudioLabel, peer_connection_factory_->CreateAudioSource(NULL)));

	rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
		peer_connection_factory_->CreateVideoTrack(
			kVideoLabel,
			peer_connection_factory_->CreateVideoSource(OpenVideoCaptureDevice(),
				NULL)));
//	main_wnd_->StartLocalRenderer(video_track);
	StartLocalRenderer(video_track);
	

	rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
		peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);

	stream->AddTrack(audio_track);
	stream->AddTrack(video_track);
	if (!peer_connection_->AddStream(stream)) {
		LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
	}
	typedef std::pair<std::string,
		rtc::scoped_refptr<webrtc::MediaStreamInterface> >
		MediaStreamPair;
	active_streams_.insert(MediaStreamPair(stream->label(), stream));
//	main_wnd_->SwitchToStreamingUI();
	ui_ = STREAMING;
}

std::unique_ptr<cricket::VideoCapturer> WebRTC::OpenVideoCaptureDevice()
{
	std::vector<std::string> device_names;
	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
			webrtc::VideoCaptureFactory::CreateDeviceInfo());
		if (!info) {
			return nullptr;
		}
		int num_devices = info->NumberOfDevices();
		for (int i = 0; i < num_devices; ++i) {
			const uint32_t kSize = 256;
			char name[kSize] = { 0 };
			char id[kSize] = { 0 };
			if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
				device_names.push_back(name);
			}
		}
	}

	cricket::WebRtcVideoDeviceCapturerFactory factory;
	std::unique_ptr<cricket::VideoCapturer> capturer;
	for (const auto& name : device_names) {
		capturer = factory.Create(cricket::Device(name, 0));
		if (capturer) {
			//		main_wnd_->MessageBox("Success", "Succeed to create Capturer", true);
			break;
		}
		else
		{
			//		main_wnd_->MessageBox("Error", "Failed to create Capturer", true);
		}
	}
	return capturer;
}

Peers WebRTC::GetPeersList()
{
	return custom_client->peers();
}

void WebRTC::SendMessage(const std::string& json_object) {
	std::string* msg = new std::string(json_object);
//	main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
	if (msg) {
		// For convenience, we always run the message through the queue.
		// This way we can be sure that messages are sent to the server
		// in the same order they were signaled without much hassle.
		pending_messages_.push_back(msg);
	}

	if (!pending_messages_.empty() && !custom_client->IsSendingMessage()) {
		msg = pending_messages_.front();
		pending_messages_.pop_front();

		if (!custom_client->SendToPeer(peer_id_, *msg) && peer_id_ != -1) {
			LOG(LS_ERROR) << "SendToPeer failed";
			DisconnectFromServer();
		}
		delete msg;
	}

	if (!peer_connection_.get())
		peer_id_ = -1;
}

WebRTC::VideoRenderer::VideoRenderer(
	int width, int height,
	webrtc::VideoTrackInterface* track_to_render)
	: rendered_track_(track_to_render) {
	::InitializeCriticalSection(&buffer_lock_);
	ZeroMemory(&bmi_, sizeof(bmi_));
	bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi_.bmiHeader.biPlanes = 1;
	bmi_.bmiHeader.biBitCount = 32;
	bmi_.bmiHeader.biCompression = BI_RGB;
	bmi_.bmiHeader.biWidth = width;
	bmi_.bmiHeader.biHeight = -height;
	bmi_.bmiHeader.biSizeImage = width * height *
		(bmi_.bmiHeader.biBitCount >> 3);
	rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

WebRTC::VideoRenderer::~VideoRenderer() {
	rendered_track_->RemoveSink(this);
	::DeleteCriticalSection(&buffer_lock_);
}

void WebRTC::VideoRenderer::SetSize(int width, int height) {
	AutoLock<VideoRenderer> lock(this);

	if (width == bmi_.bmiHeader.biWidth && height == bmi_.bmiHeader.biHeight) {
		return;
	}

	bmi_.bmiHeader.biWidth = width;
	bmi_.bmiHeader.biHeight = -height;
	bmi_.bmiHeader.biSizeImage = width * height *
		(bmi_.bmiHeader.biBitCount >> 3);
	image_.reset(new uint8_t[bmi_.bmiHeader.biSizeImage]);
}

void WebRTC::VideoRenderer::OnFrame(
	const webrtc::VideoFrame& video_frame) {

		{
			AutoLock<VideoRenderer> lock(this);

			rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
				video_frame.video_frame_buffer());
			if (video_frame.rotation() != webrtc::kVideoRotation_0) {
				buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
			}

			SetSize(buffer->width(), buffer->height());

			RTC_DCHECK(image_.get() != NULL);
			libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(),
				buffer->DataU(), buffer->StrideU(),
				buffer->DataV(), buffer->StrideV(),
				image_.get(),
				bmi_.bmiHeader.biWidth *
				bmi_.bmiHeader.biBitCount / 8,
				buffer->width(), buffer->height());
		}
//		InvalidateRect(wnd_, NULL, TRUE);
}

void WebRTC::StartLocalRenderer(webrtc::VideoTrackInterface* local_video) {
	local_renderer_.reset(new VideoRenderer(1, 1, local_video));
}

void WebRTC::StopLocalRenderer() {
	local_renderer_.reset();
}

void WebRTC::StartRemoteRenderer(webrtc::VideoTrackInterface* remote_video) {
	remote_renderer_.reset(new VideoRenderer(1, 1, remote_video));
}

void WebRTC::StopRemoteRenderer() {
	remote_renderer_.reset();
}

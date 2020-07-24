
#ifndef PORTSIP_PORTSIPUC_hxx
#define PORTSIP_PORTSIPUC_hxx

#include "AbstractCallbackDispatcher.hxx"
#include "PortSIPTypes.hxx"


namespace PortSIP
{


int  PortSIP_getNICNums();
int  PortSIP_getLocalIpAddress(int index, char* ip, int ipSize);

void PortSIP_delCallbackParameters(void* parameters);

void* PortSIP_initialize(AbstractCallbackDispatcher* callbackDispatcher,
	TRANSPORT_TYPE transport,
	const char* localIp,
	int localSIPPort,
	PORTSIP_LOG_LEVEL logLevel,
	const char* logFilePath,
	int maxCallSessions,
	const char* sipAgentString,
	int audioDeviceLayer,
	int videoDeviceLayer,
	const char* TLSCertificatesRootPath,
	const char* TLSCipherList,
	bool verifyTLSCertificate,
	int* errorCode);


int PortSIP_setInstanceId(void* libSDK, const char* instanceId);

void PortSIP_unInitialize(void* libSDK);

int PortSIP_getVersion(void* libSDK, int* majorVersion, int* minorVersion);

int PortSIP_setUser(void* libSDK,
	const char* userName,
	const char* displayName,
	const char* authName,
	const char* password,
	const char* sipDomain,
	const char* sipServerAddr,
	int sipServerPort,
	const char* stunServerAddr,
	int stunServerPort,
	const char* outboundServerAddr,
	int outboundServerPort);

void PortSIP_removeUser(void* libSDK);
int PortSIP_setDisplayName(void* libSDK, const char* displayName);

int PortSIP_setInstanceId(void* libSDK, const char* uuid);

int PortSIP_registerServer(void* libSDK, int regExpires, int retryTimes);
int PortSIP_refreshRegistration(void* libSDK, int regExpires);
int PortSIP_unRegisterServer(void* libSDK);

int PortSIP_enableRport(void* libSDK, bool enable);
int PortSIP_enableEarlyMedia(void* libSDK, bool enable);
int PortSIP_enableReliableProvisional(void* libSDK, bool enable);
int PortSIP_enable3GppTags(void* libSDK, bool enable);
void PortSIP_enableCallbackSignaling(void* libSDK, bool enableSending, bool enableReceived);
int PortSIP_setRtpCallback(void* libSDK, void* obj, fnReceivedRTPPacket receivedCallback, fnSendingRTPPacket sendingCallback);

int PortSIP_addAudioCodec(void* libSDK, AUDIOCODEC_TYPE codecType);
int PortSIP_addVideoCodec(void* libSDK, VIDEOCODEC_TYPE codecType);
bool PortSIP_isAudioCodecEmpty(void* libSDK);
bool PortSIP_isVideoCodecEmpty(void* libSDK);
int PortSIP_setAudioCodecPayloadType(void* libSDK, AUDIOCODEC_TYPE codecType, int payloadType);
int PortSIP_setVideoCodecPayloadType(void* libSDK, VIDEOCODEC_TYPE codecType, int payloadType);
void PortSIP_clearAudioCodec(void* libSDK);
void PortSIP_clearVideoCodec(void* libSDK);
int PortSIP_setAudioCodecParameter(void* libSDK, AUDIOCODEC_TYPE codecType, const char* parameter);
int PortSIP_setVideoCodecParameter(void* libSDK, VIDEOCODEC_TYPE codecType, const char* parameter);

int PortSIP_setSrtpPolicy(void* libSDK, SRTP_POLICY policy, bool allowSrtpOverUnsecureTransport);
int PortSIP_setRtpPortRange(void* libSDK, int minimumRtpAudioPort, int maximumRtpAudioPort, int minimumRtpVideoPort, int maximumRtpVideoPort);
int PortSIP_setRtcpPortRange(void* libSDK, int minimumRtcpAudioPort, int maximumRtcpAudioPort, int minimumRtcpVideoPort, int maximumRtcpVideoPort);

int PortSIP_enableCallForward(void* libSDK, bool forBusyOnly, const char* forwardTo);
int PortSIP_disableCallForward(void* libSDK);
int PortSIP_enableSessionTimer(void* libSDK, int timerSeconds, SESSION_REFRESH_MODE refreshMode);
int PortSIP_disableSessionTimer(void* libSDK);
void PortSIP_setDoNotDisturb(void* libSDK, bool state);
int PortSIP_enableAutoCheckMwi(void* libSDK, bool state);
int PortSIP_setRtpKeepAlive(void* libSDK, bool state, int keepAlivePayloadType, int deltaTransmitTimeMS);
int PortSIP_setKeepAliveTime(void* libSDK, int keepAliveTime);

int PortSIP_getSipMessageHeaderValue(void* libSDK, const char* sipMessage, const char* headerName, char* headerValue, int headerValueLength);

// msgType - 1: request; msgType - 2: response; msgType - 3: both
int PortSIP_addSipMessageHeader(void* libSDK, long sessionId, const char* methodName, int msgType, const char* headerName, const char* headerValue);
int PortSIP_removeAddedSipMessageHeader(void* libSDK, long sipMessageHeaderId);
int PortSIP_modifySipMessageHeader(void* libSDK, long sessionId, const char* methodName, int msgType, const char* headerName, const char* headerValue);
int PortSIP_removeModifiedSipMessageHeader(void* libSDK, long sipMessageHeaderId);
void PortSIP_clearAddedSipMessageHeaders(void* libSDK);
void PortSIP_clearModifiedSipMessageHeaders(void* libSDK);

int PortSIP_addSupportedMimeType(void* libSDK, const char* methodName, const char* mimeType, const char* subMimeType);

int PortSIP_setAudioSamples(void* libSDK, int ptime, int maxPtime);
int PortSIP_setAudioDeviceId(void* libSDK, int inputDeviceId, int outputDeviceId);
int PortSIP_setVideoDeviceId(void* libSDK, int deviceId);
int PortSIP_setVideoResolution(void* libSDK, int width, int height);
int PortSIP_setAudioBitrate(void* libSDK, long sessionId, AUDIOCODEC_TYPE codecType, int bitrateKbps);
int PortSIP_setVideoBitrate(void* libSDK, long sessionId, int bitrateKbps);
int PortSIP_setVideoFrameRate(void* libSDK, long sessionId, int frameRate);
int PortSIP_sendVideo(void* libSDK, long sessionId, bool sendState);

void PortSIP_muteMicrophone(void* libSDK, bool mute);
void PortSIP_muteSpeaker(void* libSDK, bool mute);
void PortSIP_setChannelOutputVolumeScaling(void* libSDK, long sessionId, int scaling);
void PortSIP_setChannelInputVolumeScaling(void* libSDK, long sessionId, int scaling);
void PortSIP_setLocalVideoWindow(void* libSDK, void* localVideoWindow);
int PortSIP_setRemoteVideoWindow(void* libSDK, long sessionId, void* remoteVideoWindow);
int PortSIP_displayLocalVideo(void* libSDK, bool state, bool mirror);
int PortSIP_setVideoNackStatus(void* libSDK, bool state);

long PortSIP_call(void* libSDK, const char* callee, bool sendSdp, bool videoCall);
int PortSIP_rejectCall(void* libSDK, long sessionId, int code);
int PortSIP_hangUp(void* libSDK, long sessionId);
int PortSIP_answerCall(void* libSDK, long sessionId, bool videoCall);
int PortSIP_updateCall(void* libSDK, long sessionId, bool enableAudio, bool enableVideo);
int PortSIP_hold(void* libSDK, long sessionId);
int PortSIP_unHold(void* libSDK, long sessionId);
int PortSIP_refer(void* libSDK, long sessionId, const char* referTo);
int PortSIP_attendedRefer(void* libSDK, long sessionId, long replaceSessionId, const char* referTo);
int PortSIP_attendedRefer2(void* libSDK, long sessionId, long replaceSessionId, const char* replaceMethod, const char* target, const char* referTo);
int PortSIP_outOfDialogRefer(void* libSDK, long replaceSessionId, const char* replaceMethod, const char* target, const char* referTo);
long PortSIP_acceptRefer(void* libSDK, long referId, const char* referSignaling);
int PortSIP_rejectRefer(void* libSDK, long referId);
int PortSIP_muteSession(void* libSDK,
	long sessionId,
	bool muteIncomingAudio,
	bool muteOutgoingAudio,
	bool muteIncomingVideo,
	bool muteOutgoingVideo);

int PortSIP_forwardCall(void* libSDK, long sessionId, const char* forwardTo);

long PortSIP_pickupBLFCall(void* libSDK, const char* replaceDialogId, bool videoCall);
int PortSIP_sendDtmf(void* libSDK, long sessionId, DTMF_METHOD dtmfMethod, int code, int dtmfDuration, bool playDtmfTone);

int PortSIP_enableSendPcmStreamToRemote(void* libSDK, long sessionId, bool state, int streamSamplesPerSec);
int PortSIP_sendPcmStreamToRemote(void* libSDK, long sessionId, const unsigned char* data, int dataLength);

int PortSIP_enableSendVideoStreamToRemote(void* libSDK, long sessionId, bool state);
int PortSIP_sendVideoStreamToRemote(void* libSDK, long sessionId, const unsigned char* data, int dataLength, int width, int height);


int PortSIP_enableAudioStreamCallback(void* libSDK, int sessionId, bool enable, AUDIOSTREAM_CALLBACK_MODE callbackMode, void* obj, fnAudioRawCallback callbackFunc);
int PortSIP_enableVideoStreamCallback(void* libSDK, int sessionId, VIDEOSTREAM_CALLBACK_MODE callbackMode, void* obj, fnVideoRawCallback callbackFunc);

int PortSIP_startRecord(void* libSDK,
	long sessionId,
	const char* recordFilePath,
	const char* recordFileName,
	bool appendTimeStamp,
	AUDIO_FILE_FORMAT audioFileFormat,
	RECORD_MODE audioRecordMode,
	VIDEOCODEC_TYPE videoFileCodecType,
	RECORD_MODE videoRecordMode);

int PortSIP_stopRecord(void* libSDK, long sessionId);

int  PortSIP_playVideoFileToRemote(void* libSDK,
	long sessionId,
	const char* aviFile,
	bool loop,
	bool playAudio);

int PortSIP_stopPlayVideoFileToRemote(void* libSDK, long sessionId);

int PortSIP_playAudioFileToRemote(void* libSDK,
	long sessionId,
	const char* filename, // Support .wav, .amr and .pcm file formats
	int fileSamplesPerSec, // Must set fileSamplesPerSec for play .pcm file 
	bool loop);

int PortSIP_stopPlayAudioFileToRemote(void* libSDK, long sessionId);

int PortSIP_playAudioFileToRemoteAsBackground(void* libSDK, long sessionId, const char* filename, int fileSamplesPerSec);
int PortSIP_stopPlayAudioFileToRemoteAsBackground(void* libSDK, long sessionId);

int PortSIP_createAudioConference(void* libSDK);
int PortSIP_createVideoConference(void* libSDK, void* conferenceVideoWindow, int width, int height, bool displayLocalVideoInConference);
void PortSIP_destroyConference(void* libSDK);
int PortSIP_setConferenceVideoWindow(void* libSDK, void* conferenceVideoWindow);
int PortSIP_joinToConference(void* libSDK, long sessionId);
int PortSIP_removeFromConference(void* libSDK, long sessionId);

int PortSIP_setAudioRtcpBandwidth(void* libSDK, long sessionId, int BitsRR, int BitsRS, int KBitsAS);
int PortSIP_setVideoRtcpBandwidth(void* libSDK, long sessionId, int BitsRR, int BitsRS, int KBitsAS);

int PortSIP_getAudioStatistics(void* libSDK, long sessionId,
	int* sendBytes, int* sendPackets,
	int* sendPacketsLost, int* sendFractionLost,
	int* sendRttMS, int* sendCodecType,
	int* sendJitterMS, int* sendAudioLevel,
	int* recvBytes, int* recvPackets,
	int* recvPacketsLost, int* recvFractionLost,
	int* recvCodecType, int* recvJitterMS,
	int* recvAudioLevel);

int PortSIP_getVideoStatistics(void* libSDK, long sessionId,
	int* sendBytes,
	int* sendPackets,
	int* sendPacketsLost,
	int* sendFractionLost,
	int* sendRttMS,
	int* sendCodecType,
	int* sendFrameWidth,
	int* sendFrameHeight,
	int* sendBitrateBPS,
	int* sendFramerate,
	//Receiver
	int* recvBytes,
	int* recvPackets,
	int* recvPacketsLost,
	int* recvFractionLost,
	int* recvCodecType,
	int* recvFrameWidth,
	int* recvFrameHeight,
	int* recvBitrateBPS,
	int* recvFramerate);

void PortSIP_enableVAD(void* libSDK, bool state);
void PortSIP_enableAEC(void* libSDK, bool state);
void PortSIP_enableCNG(void* libSDK, bool state);
void PortSIP_enableAGC(void* libSDK, bool state);
void PortSIP_enableANS(void* libSDK, bool state);
int PortSIP_enableAudioQos(void* libSDK, bool state);
int PortSIP_enableVideoQos(void* libSDK, bool state);
int PortSIP_setVideoMTU(void* libSDK, int mtu);


int PortSIP_sendOptions(void* libSDK, const char* to, const char* sdp);
int PortSIP_sendInfo(void* libSDK, long sessionId, const char* mimeType, const char* subMimeType, const char* infoContents);

long PortSIP_sendSubscription(void* libSDK, const char* to, const char* eventName);
void PortSIP_terminateSubscription(void* libSDK, long subscribeId);

long PortSIP_sendMessage(void* libSDK,
	long sessionId,
	const char* mimeType,
	const char* subMimeType,
	const unsigned char* message,
	int messageLength);

long PortSIP_sendOutOfDialogMessage(void* libSDK,
	const char* to,
	const char* mimeType,
	const char* subMimeType,
	bool isSMS,
	const unsigned char* message,
	int messageLength);

int PortSIP_setDefaultSubscriptionTime(void* libSDK, unsigned int secs);
int PortSIP_setDefaultPublicationTime(void* libSDK, unsigned int secs);

int PortSIP_setPresenceMode(void* libSDK, int mode);
long PortSIP_presenceSubscribe(void* libSDK, const char* subscribeTo, const char* subject);
int PortSIP_presenceTerminateSubscribe(void* libSDK, long subscribeId);

int PortSIP_presenceAcceptSubscribe(void* libSDK, long subscribeId);


int PortSIP_presenceRejectSubscribe(void* libSDK, long subscribeId);
int PortSIP_setPresenceStatus(void* libSDK, long subscribeId, const char* statusText);



//////////////////////////////////////////////////////////////////////////
//
// Device Manage functions.
//
//////////////////////////////////////////////////////////////////////////

int PortSIP_getNumOfRecordingDevices(void* libSDK);

int PortSIP_getNumOfPlayoutDevices(void* libSDK);

int PortSIP_getRecordingDeviceName(void* libSDK,
	int index,
	char* nameUTF8,
	int nameUTF8Length);

int PortSIP_getPlayoutDeviceName(void* libSDK,
	int index,
	char* nameUTF8,
	int nameUTF8Length);

int PortSIP_setSpeakerVolume(void* libSDK, unsigned int volume);
int PortSIP_getSpeakerVolume(void* libSDK);

int PortSIP_setMicVolume(void* libSDK, unsigned int volume);
int PortSIP_getMicVolume(void* libSDK);
void PortSIP_audioPlayLoopbackTest(void* libSDK, bool enable);

// Video device
int PortSIP_getNumOfVideoCaptureDevices(void* libSDK);
int PortSIP_getVideoCaptureDeviceName(void* libSDK,
	unsigned int index,
	char* uniqueIdUTF8,
	const unsigned int uniqueIdUTF8Length,
	char* deviceNameUTF8,
	const unsigned int deviceNameUTF8Length);

int PortSIP_showVideoCaptureSettingsDialogBox(void* libSDK,
	const char* uniqueIdUTF8,
	const unsigned int uniqueIdUTF8Length,
	const char* dialogTitle,
	void* parentWindow,
	const unsigned int x,
	const unsigned int y);



}

#endif
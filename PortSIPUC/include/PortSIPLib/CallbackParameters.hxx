#ifndef PORTSIP_ICALLBACKPARAMETERS_hxx
#define PORTSIP_ICALLBACKPARAMETERS_hxx


namespace PortSIP
{

#ifndef ET_BUFFER_SIZE
#define ET_BUFFER_SIZE		512
#endif

#ifndef MAX_MATCHED_AUDIO_CODEC_COUNT
#define  MAX_MATCHED_AUDIO_CODEC_COUNT 15
#endif

#ifndef MAX_MATCHED_VIDEO_CODEC_COUNT
#define  MAX_MATCHED_VIDEO_CODEC_COUNT 5
#endif



#ifndef SIP_MESSAGE_MAXSIZE
#define SIP_MESSAGE_MAXSIZE	1024*64
#endif


typedef enum
{
	SIP_UNKNOWN = 0,
	SIP_REGISTER_SUCCESS = 1,		// Register to SIP server succeeded
	SIP_REGISTER_FAILURE,			// Register to SIP server failed

	SIP_INVITE_INCOMING,			//	The call is incoming
	SIP_INVITE_TRYING,				//	The call is trying
	SIP_INVITE_SESSION_PROGRESS,	//	The 183 MESSAGE, early media	
	SIP_INVITE_RINGING,				//	The call is ringing
	SIP_INVITE_ANSWERED,			//	The callee has answered this call
	SIP_INVITE_FAILURE,				//	The call is failed
	SIP_INVITE_UPDATED,				//	The remote party updated this call
	SIP_INVITE_CONNECTED,			//	The call(dialog) is connected - usually for received or sent the ACK
	SIP_INVITE_BEGINING_FORWARD,	//  When the SDK begin forwarding the call
	SIP_INVITE_CLOSED,				//	The call is closed

	SIP_DIALOG_STATE_UPDATED,		//  When a subscribed user in a call and the 
									//  call state is changed, this event will be triggered

	SIP_REMOTE_HOLD,				//	The remote party has hold this call
	SIP_REMOTE_UNHOLD,				//	The remote party has take off the hold

	SIP_RECEIVED_REFER,				//	Received a REFER message
	SIP_REFER_ACCEPTED,				//	The REFER request was accepted
	SIP_REFER_REJECTED,				//	The REFER request was rejected
	SIP_TRANSFER_TRYING,			//	The call transfer is trying	
	SIP_TRANSFER_RINGING,			//	The transfer call is ringing
	SIP_ACTV_TRANSFER_SUCCESS,		//	The active transfer call succeeds
	SIP_ACTV_TRANSFER_FAILURE,		//	Active transfer call is failure

	SIP_RECEIVED_SIGNALING,			//	This event will be fired when the SDK received every SIP message
	SIP_SENDING_SIGNALING,			//	This event will be fired when the SDK send every SIP message

	SIP_WAITING_VOICEMESSAGE,		//	If have waiting voice message, this event will be fired(MWI)
	SIP_WAITING_FAXMESSAGE,			//	If have waiting fax message, this event will be fired(MWI)

	SIP_RECV_DTMFTONE,				//	This event will be fired when received a DTMF tone

	SIP_RECV_MESSAGE,				//	Received a MESSAGE message in dialog
	SIP_RECV_OUTOFDIALOG_MESSAGE,	//	Received a MESSAGE out of dialog

	SIP_SEND_MESSAGE_SUCCESS,		//	Succeeded to send the message 
	SIP_SEND_MESSAGE_FAILURE,		//	Failed to send the message 

	SIP_SEND_OUTOFDIALOG_MESSAGE_SUCCESS,	//	Succeeded to send the out of dialog message 
	SIP_SEND_OUTOFDIALOG_MESSAGE_FAILURE,	//	Failed to send the out of dialog message 

	SIP_PRESENCE_RECV_SUBSCRIBE,	//	The remote side request subscribe presence state
	SIP_PRESENCE_ONLINE,			//	The contact is go online
	SIP_PRESENCE_OFFLINE,			//	The contact is go offline

	SIP_RECV_OPTIONS,				//	Received an OPTIONS message out of dialog
	SIP_RECV_INFO,					//	Received a INFO message in dialog

	SIP_RECV_NOTIFY_OF_SUBSCRIPTION, // Received a NOTIFY of the SUBSCRIPTION
	SIP_SUBSCRIPTION_FAILURE,        // Failed to SUBSCRIBE an event
	SIP_SUBSCRIPTION_TERMINATED,     // The SUBSCRIPTION is terminated

	SIP_PLAY_AUDIO_FILE_FINISHED,
	SIP_PLAY_VIDEO_FILE_FINISHED,

}SIP_EVENT;


class ICallbackParameters
{

public:
    virtual ~ICallbackParameters(){};
	virtual SIP_EVENT getEventType() = 0;

	virtual long getSessionId() = 0;

	virtual const char* getCallerDisplayName() = 0;
	virtual const char* getCalleeDisplayName() = 0;


	virtual const char*  getCaller() = 0;
	virtual const char*  getCallee() = 0;

	virtual bool getExistsEarlyMedia() = 0;

	virtual int getStatusCode() = 0;
	virtual const char* getStatusText() = 0;

	virtual long getReferId() = 0;
	virtual const char* getReferFrom() = 0;
	
	virtual const char* getReferTo() = 0;

	virtual const char* getForwardTo() = 0;

	virtual const unsigned char * getMessageData() = 0;
	virtual	int getMessageDataLength() = 0;

	virtual const char * getSignaling() = 0;

	virtual const char* getAudioCodecs() = 0;
	virtual const char* getVideoCodecs() = 0;

	virtual bool getExistsVideo() = 0;
	virtual bool getExistsAudio() = 0;

	virtual const char* getWaitingMessageAccount() = 0;

	virtual const char* getPresenceSubject() = 0;

	virtual int getUrgentNewWaitingMessageCount() = 0;

	virtual int getNewWaitingMessageCount() = 0;

	virtual int getUrgentOldWaitingMessageCount() = 0;

	virtual int getOldWaitingMessageCount() = 0;

	virtual int getDTMFTone() = 0;

	virtual long getSubscribeId() = 0;
	virtual const char* getPlayedAudioFileName() = 0;

	virtual const char* getMimeType() = 0;
	virtual const char* getSubMimeType() = 0;
	virtual long getMessageId() = 0;

	virtual int getUserId() = 0;
	

	virtual const char * getBLFMonitoredUri() = 0;
	virtual const char * getBLFDialogState() = 0;
	virtual const char * getBLFDialogDirection() = 0;
	virtual const char * getBLFDialogId() = 0;
};

}
#endif//PORTSIP_ICALLBACKPARAMETERS_hxx

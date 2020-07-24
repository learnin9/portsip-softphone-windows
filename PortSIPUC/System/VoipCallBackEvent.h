#ifndef PORTSIP_CALLBACKEVENT_H
#define PORTSIP_CALLBACKEVENT_H
#include "stdafx.h"
const char * const SIP_HEADER_MSG_ID	 = 	"portsip-push-id";
const char * const SIP_HEADER_MSG_ID_2	 =  "x-push-id";
const char * const SIP_HEADER_SENDER_MSG_ID = "X-Sender-Msg-Id";
const char * const SIP_HEADER_CONFENRENCE = "X-Host-Conference";


class CSystemEx;
class CVoipCallBackEvent :public AbstractCallbackDispatcher
{
public:
	CVoipCallBackEvent();
	~CVoipCallBackEvent();

	void onMessage(void * command);

private:

};
#endif
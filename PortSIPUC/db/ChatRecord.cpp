#include "stdafx.h"
#include "ChatRecord.h"
CChatRecord::CChatRecord()
{
	m_nMsgID =0;
	m_nMSGType			= ENUM_MSG_TEXT;
	m_strMSGTime		="";
	m_nIsMyself			=false;
	m_strMSG			= "";
	m_strTalkerURL = "";
	//m_strRecverURL = "";
	//m_nTalkType			= 0;
	m_strReserved		= "";
	m_nMSGStatus		= ENUM_MSG_STATUS_SENDING;
	m_strSignMsgID		= "";
	m_strLocalSignMsgID = "";
	m_enumFileStatus	= ENUM_FAILED_PRE;
	m_strSendLocalFilePath = "";
	m_strSendLocalFileName = "";
	m_strPlayTime = "00:00";
	m_nUserID = -1;
}
CChatRecord::~CChatRecord()
{

}
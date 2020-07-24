#include "stdafx.h"
#include "VoiceMail.h"
CVoiceMail::CVoiceMail()
{
	m_ID		= 0;
	m_strCaller = "";
	m_strCalled = "";
	m_strTime = "";
	m_enumFileStatus =ENUM_MSG_STATUS_UNREAD;
	m_enumDownLoadStatus = ENUM_UNPROCESS;
	m_strReserved = "";
	m_strFileName = "";
	m_strPlayTime = "";
	m_nUserID = -1;
	m_bRead = false;
}
CVoiceMail::~CVoiceMail()
{

}
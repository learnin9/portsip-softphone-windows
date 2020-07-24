#include "stdafx.h"
#include "VARecord.h"
CVARecord::CVARecord()
{
	m_nCallHistoryID		= 0;
	m_enumVARecordType		=ENUM_VA_RECORD_AUDIO;
	m_strFileName			="";
	m_strBeginTime			="";
	m_strEndTime			="";
	m_strSIPNum				="";
	m_strReserved			="";
	m_strPlayTime			= "";
	m_nUserID				= -1;
	m_bRead = false;
}
CVARecord::~CVARecord()
{

}
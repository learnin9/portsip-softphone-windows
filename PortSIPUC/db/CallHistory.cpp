#include "stdafx.h"
#include "CallHistory.h"
CCallHistory::CCallHistory()
{
	m_nCallHistoryID = 0;
	m_enumContentType = CALL_PHONE;
	m_enumCallType = ENUM_CALL_IN;
	m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	m_strBeginTime   = "";
	m_strEndTime	 = "";
	m_strSIPNum		 = "";
	m_strReserved	 = "";
	m_nUserID		 = -1;
	m_bRead = false;
}
CCallHistory::~CCallHistory()
{

}
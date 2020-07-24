#ifndef CALL_HISTORY_H
#define CALL_HISTORY_H
#include "DBParam.h"
class CCallHistory
{
public:
	CCallHistory();
	~CCallHistory();
	int					m_nCallHistoryID;
	ENUM_CONTENT_TYPE	m_enumContentType;
	ENUM_CALL_TYPE		m_enumCallType;
	ENUM_CALL_STATUS    m_enumCallStatus;
	std::string			m_strBeginTime;
	std::string			m_strEndTime;
	std::string			m_strSIPNum;
	std::string			m_strReserved;
	int					m_nUserID;
	bool                m_bRead;
};
#endif
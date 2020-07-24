#ifndef VA_RECORD_H
#define VA_RECORD_H
class CVARecord
{
public:
	CVARecord();
	~CVARecord();

	int					m_nCallHistoryID;
	int                 m_nUserID;
	ENUM_VA_RECORD		m_enumVARecordType;
	std::string		    m_strFileName;
	std::string			m_strBeginTime;
	std::string			m_strEndTime;
	//SIPnumber
	std::string			m_strSIPNum;
	std::string         m_strPlayTime;
	std::string			m_strReserved;
	bool                m_bRead;
};
#endif
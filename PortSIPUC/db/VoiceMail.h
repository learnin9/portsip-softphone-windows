#ifndef VOICE_MAIL_H
#define VOICE_MAIL_H
class CVoiceMail
{
public:
	CVoiceMail();
	~CVoiceMail();

	long     long          m_ID;
	int                 m_nUserID;
	std::string 		m_strCaller;
	std::string		    m_strCalled;
	std::string			m_strTime;
	std::string			m_strFileName;
	ENUM_MSG_STATUS		m_enumFileStatus;
	ENUM_FILE_STATUS			m_enumDownLoadStatus;
	std::string			m_strReserved;
	std::string         m_strPlayTime;
	bool                m_bRead;
};

#endif
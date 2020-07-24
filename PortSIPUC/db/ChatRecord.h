#ifndef CHART_RECORD_H
#define CHART_RECORD_H
class CChatRecord
{
public:
	CChatRecord();
	~CChatRecord();
	int			m_nMsgID;
	ENUM_MSG_TYPE			m_nMSGType;
	std::string m_strMSGTime;
	bool			m_nIsMyself;
	std::string m_strMSG;
	std::string m_strTalkerURL;

	//int		m_nTalkType;
	ENUM_MSG_STATUS         m_nMSGStatus;
	ENUM_FILE_STATUS		m_enumFileStatus;
	std::string				m_strSendLocalFilePath;
	std::string				m_strSendLocalFileName;
	std::string		  m_strReserved;
	std::string       m_strSignMsgID;
	std::string       m_strLocalSignMsgID;
	std::string       m_strPlayTime;
	int               m_nUserID;
};
#endif
#ifndef TALK_GROUP_NUMBER_H
#define TALK_GROUP_NUMBER_H
using namespace std;
class CTalkGroupNumber
{
public:
	CTalkGroupNumber();
	~CTalkGroupNumber();
	int					m_nTalkGroupID;
	std::string			m_strSIPNumber;
	std::string			m_strSelfDisplayName;
	bool				m_bIsManager;
	bool				m_bIsCreator;
	bool				m_bFavorite;
	bool				m_bIsDisable;

	void SetContactDB(CContactDB* pContact) { m_pContact = pContact; };
	CContactDB* GetContactDB() {
		return m_pContact;
	}
	void SetActiveLine(int nActiveLine) { m_nActiveLine = nActiveLine; };
	int  ReturnActiveLine() { return m_nActiveLine; }
	void SetCallHistoryID(int nCallHistoryID) {
		m_nCallHistoryID = nCallHistoryID;
	}
	int  GetCallHistoryID() { return m_nCallHistoryID; }
	void ResetCallHistortID() { m_nCallHistoryID = -1; };
	void ResetActiveLine() { m_nActiveLine = -1; };
	//void SetCallType(ENUM_CALL_STATUS enumStatus) {return };
	void ClearAddConfenceInfo()
	{
		if (m_pContact)
		{
			m_pContact->ClearAddConfenceInfo();
		}
	};
public:
	bool GetIsConnected() { return m_bIsConnected; }
	bool          m_bIsConnected;
	int			  m_nActiveLine;
	long		 m_nCallHistoryID;
private:
	CContactDB*        m_pContact;
	
};
#endif
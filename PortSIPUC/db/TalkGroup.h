#ifndef TALK_GROUP_H
#define TALK_GROUP_H
#include <map>
#include "db/TalkGroupNumber.h"
#include "../System/CriticalSection.h"
using namespace std;

struct TAG_RECENT;
class CTalkGroup
{
public:
	CTalkGroup();
	~CTalkGroup();
	int					m_nChatGroupID;
	std::string			m_strChatGroupName;
	std::string			m_strSelfDisplayName;
	std::string			m_strRecentDateTime;
	int					m_nUserID;
	std::string			m_strReserved;
	vector<CTalkGroupNumber *> m_vecContact;
	void AddContact2Group(CTalkGroupNumber* pContact);
	void InitContact2Group(vector<CTalkGroupNumber*> vecContact);
	vector<CTalkGroupNumber*> GetContact();
	int GetMemberCount() { return m_vecContact.size(); }
	CTalkGroupNumber* GetMember(int nIndex);
	time_t		m_tRecentChat;
	void SetUserData(DWORD_PTR dwData) { m_dwUserData = dwData; };
	unsigned long  GetUserData() const {	return m_dwUserData;};
	unsigned long m_dwUserData;


	void SetCallType(ENUM_CALL_TYPE enumCallType) { m_enumCallType = enumCallType; };
	ENUM_CALL_TYPE GetCallType() { return m_enumCallType; }

	ENUM_CALL_TYPE m_enumCallType;

	void CopySrcData2GroupNumber(int nActiveLine,int nCallHistoryID,void *pContact);

	bool ContactIsInGroup(TAG_RECENT *pContactInfo);
	void UpdateGroupNameByContact(TAG_RECENT *pContactInfo);
	CCriticalSection		 m_oMemberLock;
};
#endif
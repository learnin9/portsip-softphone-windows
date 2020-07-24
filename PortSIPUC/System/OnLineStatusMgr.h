#pragma  once
#include"system/CriticalSection.h"
#include "BaseThread.h"
class COnLineStatusMgr
{
public:
	COnLineStatusMgr();
	~COnLineStatusMgr();
	void InitLocalContact(std::vector<TAG_RECENT*> vecLocalContact)
	{
		vecLocalContact.swap(m_vecLocalContact);
	};
	void UpdateLocalContact(std::vector<TAG_RECENT*> vecLocalContact);
	void InitRemoteContact(std::vector<CContactDB*> vecRemoteContact)
	{
		vecRemoteContact.swap(m_vecRemoteContact);
	};
	void InitExtersionContact(std::vector<CContactDB*> vecExtersionContact)
	{
		vecExtersionContact.swap(m_vecExtersionContact);
	};
	void StartSub();
	//
	void GetOnLineStatus();
	bool GetStatusOfLocalContact();
	bool GetStatusOfRemoteContact();
	bool GetStatusOfExtersions();
protected:
	std::vector<TAG_RECENT*> m_vecLocalContact;
	std::vector<CContactDB*> m_vecRemoteContact;
	std::vector<CContactDB*> m_vecExtersionContact;
	CCriticalSection			m_oLocalContact;
private:
};
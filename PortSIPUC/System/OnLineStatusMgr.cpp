#include "stdafx.h"
#include "OnLineStatusMgr.h"
#include "System/SystemEx.h"
COnLineStatusMgr::COnLineStatusMgr()
{
}

COnLineStatusMgr::~COnLineStatusMgr()
{
}

void COnLineStatusMgr::StartSub()
{
	bool bRetLocal = GetStatusOfLocalContact();
	bool bRetExtersions = GetStatusOfExtersions();
}

bool COnLineStatusMgr::GetStatusOfLocalContact()
{
	CCriticalAutoLock loContact(m_oLocalContact);
	CSystemEx::GetInstance()->SubContactStatus(m_vecLocalContact);
	return true;
}
bool COnLineStatusMgr::GetStatusOfRemoteContact()
{
	return true;
}
bool COnLineStatusMgr::GetStatusOfExtersions()
{
	return true;
}


void COnLineStatusMgr::UpdateLocalContact(std::vector<TAG_RECENT *> vecLocalContact)
{
	CCriticalAutoLock loContact(m_oLocalContact);
	m_vecLocalContact.clear();
	m_vecLocalContact.swap(vecLocalContact);
}
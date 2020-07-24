#include "stdafx.h"
#include "ConferenceStatusSubMgr.h"
#include "System/SystemEx.h"
CConferenceStatusMgr::CConferenceStatusMgr()
{
}

CConferenceStatusMgr::~CConferenceStatusMgr()
{
}

void CConferenceStatusMgr::AddConferenceNumber(std::string strConferenceNumber)
{
	auto iterLocalConference = m_mapConferenceID2SubStatus.find(strConferenceNumber);
	if (iterLocalConference!= m_mapConferenceID2SubStatus.end())
	{
		return;
	}
	long nSubID;
	bool bRet= CSystemEx::GetInstance()->GetConferenceStatus(strConferenceNumber, nSubID);
	if (bRet)
	{
		m_mapConferenceID2SubStatus.insert(make_pair(strConferenceNumber, nSubID));
	}
}

void CConferenceStatusMgr::UnSubConferenceNumber(std::string strConferenceNumber)
{
	auto iterLocalConference = m_mapConferenceID2SubStatus.find(strConferenceNumber);
	if (iterLocalConference != m_mapConferenceID2SubStatus.end())
	{
	
		CSystemEx::GetInstance()->UnGetConferenceStatus(iterLocalConference->second);
		 m_mapConferenceID2SubStatus.erase(iterLocalConference);
	}
}

bool CConferenceStatusMgr::UnSubAllConference()
{
	auto iterLocalConference = m_mapConferenceID2SubStatus.begin();
	for (iterLocalConference;iterLocalConference!=m_mapConferenceID2SubStatus.end();)
	{
		CSystemEx::GetInstance()->UnGetConferenceStatus(iterLocalConference->second);
		iterLocalConference = m_mapConferenceID2SubStatus.erase(iterLocalConference);
	}
	return true;
}

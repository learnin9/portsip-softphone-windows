#include "stdafx.h"
#include "TalkGroup.h"
#include "../Utility/utility.hxx"
#include "../System/SystemEx.h"
CTalkGroup::CTalkGroup()
{
	m_nChatGroupID				 = -1;
	m_strChatGroupName			 = "";
	m_strSelfDisplayName	 = "";
	m_strReserved			 = "";
	m_strRecentDateTime		 = "";
	m_vecContact.clear();
}
CTalkGroup::~CTalkGroup()
{

}

void CTalkGroup::AddContact2Group(CTalkGroupNumber* pContact)
{
	CCriticalAutoLock olock(m_oMemberLock);
	m_vecContact.push_back(pContact);
}
void CTalkGroup::InitContact2Group(vector<CTalkGroupNumber*> vecContact)
{
	CCriticalAutoLock olock(m_oMemberLock);
	m_vecContact = vecContact;
}
vector<CTalkGroupNumber*> CTalkGroup::GetContact()
{
	CCriticalAutoLock olock(m_oMemberLock);
	return m_vecContact;
}

CTalkGroupNumber* CTalkGroup::GetMember(int nIndex)
{
	CTalkGroupNumber *pRet = NULL;
	CCriticalAutoLock olock(m_oMemberLock);
	size_t nCount = m_vecContact.size();
	if (nIndex >= 0 && (size_t)nIndex < nCount)
	{
		pRet = m_vecContact.at(nIndex);
	}

	return pRet;
}
bool CTalkGroup::ContactIsInGroup(TAG_RECENT *pContactInfo)
{
	bool bFind = false;
	CCriticalAutoLock olock(m_oMemberLock);
	auto iterGroupNumber = m_vecContact.begin();
	for (iterGroupNumber; iterGroupNumber != m_vecContact.end(); iterGroupNumber++)
	{
		CTalkGroupNumber *pTalkGroupNumber = *iterGroupNumber;
		if (pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
		{
			if (pTalkGroupNumber->GetContactDB() == pContactInfo->m_unionContactInfo.m_pContact)
			{
				bFind = true;
				break;
			}
			if (PortUtility::compareStringNoCase(pTalkGroupNumber->GetContactDB()->m_strSIPNum, pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
			{
				bFind = true;
				break;
			}
		}
	}
	return bFind;
}
void CTalkGroup::CopySrcData2GroupNumber(int nActiveLine, int nCallHistoryID, void *pContact)
{
	TAG_RECENT* pContactInfo = (TAG_RECENT*)pContact;
	if (pContactInfo == NULL)
		return;
	if (pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
	{
		return;
	}
	CCriticalAutoLock olock(m_oMemberLock);
	auto iterGroupNumber = m_vecContact.begin();
	for (iterGroupNumber;iterGroupNumber!=m_vecContact.end();iterGroupNumber++)
	{
		CTalkGroupNumber *pTalkGroupNumber = *iterGroupNumber;
		if (pTalkGroupNumber->GetContactDB()== pContactInfo->m_unionContactInfo.m_pContact)
		{
			pTalkGroupNumber->SetActiveLine(nActiveLine);
			pTalkGroupNumber->SetCallHistoryID(nCallHistoryID);
		}
	}
}

void CTalkGroup::UpdateGroupNameByContact(TAG_RECENT *pContactInfo)
{
	if (pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
	{
		return;
	}
	CCriticalAutoLock olock(m_oMemberLock);
	auto iterVecContact = m_vecContact.begin();
	int nIndex = 0;
	std::string strUTF8ChatGroupName, strMemberList, strUTF8DispalyName;
	for (iterVecContact; iterVecContact != m_vecContact.end(); iterVecContact++)
	{
		CTalkGroupNumber* pTalkGroupNumber = *iterVecContact;
		strMemberList += pTalkGroupNumber->m_strSIPNumber + ";";
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		if (PortUtility::compareStringNoCase(pTalkGroupNumber->m_strSIPNumber, pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
		{
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactInfo->m_unionContactInfo.m_pContact);
		}
		else
		{
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pTalkGroupNumber->GetContactDB());
		}

		pTalkGroupNumber->m_strSelfDisplayName = PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
		if (nIndex < m_vecContact.size() - 1)
		{
			strUTF8ChatGroupName += PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength())) + ",";
		}
		else if (nIndex == m_vecContact.size() - 1)
		{
			strUTF8ChatGroupName += PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
		}
		nIndex++;
	}
	if (m_vecContact.size() > 2)
	{
		strUTF8ChatGroupName += "... ";
	}

	this->m_strChatGroupName = strUTF8ChatGroupName;

	CSystemEx::GetInstance()->GetDBOprate()->UpdateGroupName(m_nChatGroupID,strUTF8ChatGroupName);
}
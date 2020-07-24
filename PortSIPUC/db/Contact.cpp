#include "stdafx.h"
#include "Contact.h"
#include "../Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/CallingPageDialog/CallingPageTransfer/CallingTransferDlg.h"
CContactDB::CContactDB()
{
	m_nUserID = 0;
	m_strSIPNum			= "";
	m_strWork			= "";
	m_strShareNote		= "";
	m_strHeadImage		= "";
	m_strWorkPhone		= "";
	m_strMobilePhone	= "";
	m_strFirstName		= "";
	m_strLastName		= "";
	m_strGroupName		= "";
	m_bFavorites = false;
	m_strReserved		= "";
	m_eOnlineState = ONLINESTATE_OFFLINE;
	m_dwUserData = 0;
	m_strEmailAdress = "";
	m_strHomeNumber = "";
	SetCallType(ENUM_CALL_IN);
	m_bTrueContact = false;
	m_nActiveLine = -1;
	m_eLastOnlineState = ONLINESTATE_OFFLINE;
	time(&m_tRecentChat);
	m_nOffLineUnReadMsgNum = 0;
	m_bTransfer = false;
	m_strDisPlayName = L"";
	m_nCallHistoryID = -1;
	m_bStartContact = false;
	m_pTransferedContact = NULL;
	m_pAddContacter = NULL;
	m_pAddDlg = NULL;
	m_bConfence = false;
	m_bSetUserFaceImage = false;
}
CContactDB::~CContactDB()
{

}
void CContactDB::CopyOnlineStatus(CContactDB* pContactDB)
{
	this->m_eOnlineState = pContactDB->m_eOnlineState;
	this->m_eLastOnlineState = pContactDB->m_eLastOnlineState;
}
void CContactDB::CopyProfileMemData(CContactDB* pContactDB)
{
	this->m_strFirstName = pContactDB->m_strFirstName;
	this->m_strLastName = pContactDB->m_strLastName;
	this->m_strEmailAdress = pContactDB->m_strEmailAdress;
	this->m_strMobilePhone = pContactDB->m_strMobilePhone;
	this->m_strHomeNumber = pContactDB->m_strHomeNumber;
	this->m_strWorkPhone = pContactDB->m_strWorkPhone;
	this->m_bFavorites = pContactDB->m_bFavorites;
	
}
unsigned long  CContactDB::GetUserData() const
{
	return m_dwUserData;
}
void CContactDB::SetIsTransfer(Window* pCallingTransferDlg, void* pTransferedConatct,bool bStartContact, bool bTransfer )
{ 
	m_bStartContact = bStartContact;
	m_pCallingTransferDlg = pCallingTransferDlg; 
	m_bTransfer = bTransfer; 
	m_pTransferedContact = pTransferedConatct;
};

void CContactDB::SetIsAddedConfence(Window* pCallingAddDlg, void* pAddContact, bool bStartContact,bool bIsAdded )
{
	m_bStartContact = bStartContact;
	m_pAddDlg = pCallingAddDlg;
	m_pAddContacter = pAddContact;
	m_bConfence = bIsAdded;
};
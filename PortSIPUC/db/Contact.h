#ifndef CONTACT_H
#define CONTACT_H
#include "DBParam.h"
//
/*
Reserved             VARCHAR(1024),
*/
enum OLE_TYPE
{
	OT_IMAGE,
	OT_VIDEO,
	OT_AUDIO,
	OT_FILE
};

class CContactDB
{
public:
	CContactDB();
	~CContactDB();

	void CopyProfileMemData(CContactDB* pContactDB);
	void CopyOnlineStatus(CContactDB* pContactDB);
	void SetUserData(DWORD_PTR dwData) { m_dwUserData = dwData; };
	unsigned long  GetUserData() const;
	void SetCallType(ENUM_CALL_TYPE enumCallType) { m_enumCallType = enumCallType; };
	ENUM_CALL_TYPE GetCallType() { return m_enumCallType; }
	void SetActiveLine(int nActiveLine) { m_nActiveLine = nActiveLine; };
	int  ReturnActiveLine() { return m_nActiveLine; }
	void SetIsTransfer(Window* pCallingTransferDlg, void* pTransferedConatct,bool bStartContact, bool bTransfer = true);
	void SetIsAddedConfence(Window* pCallingAddDlg, void* pAddContact, bool bStartContact,bool bIsAdded = true);
	bool GetIsTransfer() { return m_bTransfer; };
	bool GetIsStartedContact() { return m_bStartContact; }
	bool GetIsAdded() { return m_bConfence; };
	void SetUserFaceImage(bool bSet) 
	{
		m_bSetUserFaceImage = true;
	}
	bool GetSetUserFaceImage() {
		return m_bSetUserFaceImage;
	}
	Window* GetTransferDlg() { return m_pCallingTransferDlg; };
	Window* GetAddConfenceDlg() { return m_pAddDlg; };

	void* GetTransferContact() {
		return m_pTransferedContact;
	}
	void* GetAddContact()
	{
		return m_pAddContacter;
	}
	
	void ClearTransferInfo() { m_pCallingTransferDlg = NULL; m_bTransfer = false; m_pTransferedContact = NULL; };
	void ClearAddConfenceInfo() { m_pAddDlg = NULL, m_bConfence = false; }
	int SetOnlineState(EOnlineState eOnlineState)
	{
		m_eOnlineState = eOnlineState;
		return 0;
	}
	void SaveLastOnlineStateByNowState()
	{
		m_eLastOnlineState = m_eOnlineState;
	}
	EOnlineState GetOnlineState() const
	{
		return m_eOnlineState;
	}
	EOnlineState GetLastOnlineState() const
	{
		return m_eLastOnlineState;
	}
	void ClearnCallData()
	{
		m_nActiveLine = -1;
	}
	void SetContactDisplayName(CIUIString strDisplayname) { m_strDisPlayName = strDisplayname; };
	CIUIString GetContactDisplayName() { return m_strDisPlayName; }

	void SetHistoryID(long lHistoryID) {
		m_nCallHistoryID = lHistoryID;
	};
	int GetHistoryID()
	{
		return m_nCallHistoryID;
	}

	ENUM_CALL_TYPE m_enumCallType;
	unsigned long m_dwUserData;
	int			  m_nActiveLine;
	bool          m_bIsConnected;
	bool          m_bTrueContact;
	int		m_nUserID;
	std::string m_strSIPNum;

	std::string m_strWork;
	std::string m_strShareNote;
	std::string m_strHeadImage;
	std::string m_strWorkPhone;
	std::string m_strMobilePhone;
	std::string m_strFirstName;
	std::string m_strLastName;
	std::string m_strGroupName;
	bool			m_bFavorites;
	EOnlineState        m_eLastOnlineState;
	EOnlineState		 m_eOnlineState;
	std::string m_strReserved;
	std::string m_strEmailAdress;
	std::string m_strHomeNumber;
	time_t		m_tRecentChat;
	int         m_nOffLineUnReadMsgNum;
	bool        m_bTransfer;
	void* m_pTransferedContact;
	void* m_pAddContacter;
	Window* m_pCallingTransferDlg;
	bool  m_bStartContact;
	Window* m_pAddDlg;
	bool    m_bConfence;
	long		 m_nCallHistoryID;
	CIUIString m_strDisPlayName;
	
	bool   m_bSetUserFaceImage;
};
#endif
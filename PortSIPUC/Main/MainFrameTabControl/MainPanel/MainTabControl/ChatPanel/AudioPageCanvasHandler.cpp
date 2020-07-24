#include "stdafx.h"
#include "AudioPageCanvasHandler.h"


#include "System/SystemEx.h"
#include "../../MainCanvasHandler.h"
std::map<int, CAudioPageCanvasHandler*> CAudioPageCanvasHandler::m_mapHwnd2AudioPage;
CAudioPageCanvasHandler::CAudioPageCanvasHandler()
	: m_pCanvasCenter(NULL)
	, m_pStaFace(NULL)
	, m_pStaName(NULL)
	, m_pStaSign(NULL)
	, m_pChkRec(NULL)
	, m_pChkMute(NULL)
	, m_pChkPause(NULL)
	, m_pBtnTransfer(NULL)
	, m_pBtnAdd(NULL)
	, m_pBtnKeyboard(NULL)
	, m_pBtnShare(NULL)
	, m_pBtnMsg(NULL)
	, m_pBChkNoVideo(NULL)
	, m_pBtnHangup(NULL)
	, m_pContactInfo(NULL)
	, m_pStaState(NULL)
{	
	m_hSessionTimer = 0;
	m_pCallingTransferDlg = NULL;
	m_pAddCallerDlg = NULL;
	m_pDTMFKeyboardWnd = NULL;
	m_bHoldBySelf = false;
	m_bIsConference = false;
}

CAudioPageCanvasHandler::~CAudioPageCanvasHandler()
{
}

void CAudioPageCanvasHandler::SetIsConference(bool bIsConference)
{
	m_bIsConference = bIsConference;
}
void  CAudioPageCanvasHandler::TimerProcSession(HWND hlWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CAudioPageCanvasHandler* pAudioPageCanvasHandler = NULL;
	auto iterFind= m_mapHwnd2AudioPage.find(idEvent);
	if (iterFind!=m_mapHwnd2AudioPage.end())
	{
		pAudioPageCanvasHandler = iterFind->second;
	}
	if (pAudioPageCanvasHandler!=NULL)
	{
		if (pAudioPageCanvasHandler->m_hSessionTimer == idEvent)
		{
			OutputDebugString(L"");
			pAudioPageCanvasHandler->ProcessTimer();
		}
		else
		{

		}
		
		return;
	}
	

}
int CAudioPageCanvasHandler::InitControls()
{
	m_pCanvasCenter = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_CENTER")));
	_ASSERT(m_pCanvasCenter != NULL);
	m_pStaFace = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_FACE")));
	_ASSERT(m_pStaFace != NULL);
	m_pStaName = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_NAME")));
	_ASSERT(m_pStaName != NULL);
	m_pStaSign = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SIGN")));
	_ASSERT(m_pStaSign != NULL);
	m_pChkRec = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_REC")));
	_ASSERT(m_pChkRec != NULL);
	m_pChkMute = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_MUTE")));
	_ASSERT(m_pChkMute != NULL);
	m_pChkPause = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_PAUSE")));
	_ASSERT(m_pChkPause != NULL);
	m_pBtnTransfer = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_TRANSFER")));
	_ASSERT(m_pBtnTransfer != NULL);
	m_pBtnAdd = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_ADD")));
	_ASSERT(m_pBtnAdd != NULL);
	m_pBtnKeyboard = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_KEYBOARD")));
	_ASSERT(m_pBtnKeyboard != NULL);
	m_pBtnShare = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SHARE")));
	_ASSERT(m_pBtnShare != NULL);
	m_pBtnMsg = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MSG")));
	_ASSERT(m_pBtnMsg != NULL);
	m_pBChkNoVideo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NO_VIDEO")));
	_ASSERT(m_pBChkNoVideo != NULL);
	m_pBtnHangup = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_HANGUP")));
	_ASSERT(m_pBtnHangup != NULL);
	m_pStaState = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_STATE")));
	_ASSERT(m_pStaState != NULL);
	EnableWindow(m_pBChkNoVideo->GetSafeHwnd(), FALSE);
	EnableWindow(m_pBtnShare->GetSafeHwnd(), FALSE);
	
	if (m_pContactInfo!=NULL)
	{
		if (m_pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
		{
			EnableWindow(m_pBtnMsg->GetSafeHwnd(), TRUE);
		}
		else
		{
			EnableWindow(m_pBtnMsg->GetSafeHwnd(), TRUE);
		}
	}
	
	SetFunBtnStat(FALSE);
	EnableWindow(m_pBtnAdd->GetSafeHwnd(), FALSE);
	return 0;
	
}
void CAudioPageCanvasHandler::BeginTimer()
{
	if (m_hSessionTimer!=0)
	{
		::KillTimer(NULL, m_hSessionTimer);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			if (pContactDB->ReturnActiveLine() > 0)
			{
				CSystemEx::GetInstance()->ReSetCallTime(pContactDB->ReturnActiveLine());
			}
		}
		
	}
	m_hSessionTimer = ::SetTimer(NULL, IDT_SESSION_TIMER, 1000, TimerProcSession);
	m_mapHwnd2AudioPage.insert(make_pair(m_hSessionTimer, this));
}
void CAudioPageCanvasHandler::ProcessTimer()
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		if (pContactDB->ReturnActiveLine() < 0)
		{
			return;
		}

		CString str = CSystemEx::GetInstance()->RefreshCallTime(pContactDB->ReturnActiveLine());
		if (m_pStaSign->IsWindowVisible() == FALSE)
		{
			m_pStaSign->ShowWindow(SW_SHOW);
		}
		m_pStaSign->SetWindowText(str);
	}
}

int CAudioPageCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		_ASSERT(pContactDB != NULL);
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

		m_pStaName->SetWindowText(strSrcUserName);
		if (pContactDB->GetCallType() == ENUM_CALL_OUT)
		{
			CallTheContact();
		}
	}
	else
	{
		CTalkGroup *pGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		CIUIString strSrcUserName;
		strSrcUserName = PortUtility::Utf82Unicode_ND(m_pContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str();
		

		m_pStaName->SetWindowText(strSrcUserName);
		if (pGroup->GetCallType() == ENUM_CALL_OUT)
		{
			CallTheContact();
		}
	}
	return 0;
}
int    CAudioPageCanvasHandler::GetActiveLine()
{
	if (m_pContactInfo==NULL)
	{
		return -1;
	}
	if (m_pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
	{
		return m_pContactInfo->m_unionContactInfo.m_pContact->ReturnActiveLine();
	}
	return -1;
}
void CAudioPageCanvasHandler::SetConnected(bool bConnected,int nActiveLine)
{
	bool bIsTalkGroup = false;
	if (m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT)
	{
		bIsTalkGroup = true;
		auto vecGroupNumber = m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetContact();
		auto iterVecGroupNumber = vecGroupNumber.begin();
		for (iterVecGroupNumber;iterVecGroupNumber!=vecGroupNumber.end();iterVecGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterVecGroupNumber;
			if (pTalkGroupNumber->ReturnActiveLine()==nActiveLine)
			{
				pTalkGroupNumber->m_bIsConnected = bConnected;
			}
		}
	}
	else
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		pContactDB->m_bIsConnected = bConnected;
	}
	SetFunBtnStat(true, bIsTalkGroup);
}

bool CAudioPageCanvasHandler::GetIsConnected(int nActiveLine) {
	bool bRet = false;
	if (m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT)
	{
		auto vecGroupNumber = m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetContact();
		auto iterVecGroupNumber = vecGroupNumber.begin();
		for (iterVecGroupNumber; iterVecGroupNumber != vecGroupNumber.end(); iterVecGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterVecGroupNumber;
			if (pTalkGroupNumber->ReturnActiveLine() == nActiveLine)
			{
				bRet =  pTalkGroupNumber->m_bIsConnected ;
				break;
			}
		}
	}
	else
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		bRet = pContactDB->m_bIsConnected;
	}
	return bRet;
};
void CAudioPageCanvasHandler::SetFunBtnStat(BOOL bEnable, bool isTalkGroup)
{
	EnableWindow(m_pChkPause->GetSafeHwnd(), bEnable);
	EnableWindow(m_pChkMute->GetSafeHwnd(), bEnable);
	EnableWindow(m_pChkPause->GetSafeHwnd(), bEnable);
	EnableWindow(m_pBtnAdd->GetSafeHwnd(), bEnable);
	
	
	if (isTalkGroup&&m_pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
	{
		EnableWindow(m_pBtnTransfer->GetSafeHwnd(), FALSE);
		EnableWindow(m_pBtnKeyboard->GetSafeHwnd(), FALSE);
		EnableWindow(m_pChkRec->GetSafeHwnd(), FALSE);
	}
	else
	{
		EnableWindow(m_pChkRec->GetSafeHwnd(), bEnable);
		EnableWindow(m_pBtnTransfer->GetSafeHwnd(), bEnable);
		EnableWindow(m_pBtnKeyboard->GetSafeHwnd(), bEnable);
	}
	m_pStaSign->SetWindowText(L"");
	Invalidate((GetBindWLWnd()));
}
bool CAudioPageCanvasHandler::GetCallStatus()
{
	bool bRet = false;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		if (pContactDB->ReturnActiveLine() < 0)
		{
			bRet = false;
		}
		else
		{
			bRet = true;
		}
		
	}
	else
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		
		bool bTotalRet = false;
		auto vecGroupNumber = pTalkGroup->GetContact();
		auto iterGroupNumber = vecGroupNumber.begin();
		int  nNoCall = 0;
		for (iterGroupNumber;iterGroupNumber!=vecGroupNumber.end();iterGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterGroupNumber;
			if (pTalkGroupNumber->ReturnActiveLine()<0)
			{
				nNoCall++;
			}
			else
			{
				bRet =  true;
				break;
			}
		}
		if (nNoCall==vecGroupNumber.size())
		{
			bRet = false;
		}
	}
	return bRet;
}
bool CAudioPageCanvasHandler::CallTheGroup()
{
	CSystemEx::GetInstance()->CreateConfence(ENUM_CONFENCE_AUDIO, NULL);
	CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
	auto vecTalkGroup = pTalkGroup->GetContact();
	auto iterTalkMember = vecTalkGroup.begin();
	bool bTotalFailed = false;
	for (; iterTalkMember != vecTalkGroup.end(); iterTalkMember++)
	{
		CTalkGroupNumber *pGroupGroupNumber = *iterTalkMember;
		string strSipNumber = pGroupGroupNumber->m_strSIPNumber;
		string strErrorInfo = "";
		bool bRet = CSystemEx::GetInstance()->CallOneNumberAudio(strSipNumber, true, pGroupGroupNumber->m_nActiveLine, pGroupGroupNumber->m_nCallHistoryID, strErrorInfo);

		if (pGroupGroupNumber->m_nActiveLine < 0)
		{
			LOG4_ERROR("here cause the toolbar un enable ,CallOneNumberAudio");
		}
		if (bRet == false)
		{
			std::wstring strTip = PortUtility::string2WString(strErrorInfo);
			std::wstring strTemplate = _T("call error:") + strTip;
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"), strTemplate.c_str(), GetHostWnd(GetBindWLWnd()));
		}
		else
		{
			bTotalFailed = true;
			CSystemEx::GetInstance()->EraseSession2CanvasHandler(pGroupGroupNumber->m_nActiveLine);
			CSystemEx::GetInstance()->SaveSession2AudioCanvasHandler(pGroupGroupNumber->m_nActiveLine, this);
		}
	}
	if (bTotalFailed == false)
	{
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	}
	else
	{
		pTalkGroup->SetUserData(CT_AUDIO);

	}
	return bTotalFailed;
}
bool CAudioPageCanvasHandler::CallTheContact()
{
	bool bRet = false;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		std::string strSipNumber = pContactDB->m_strSIPNum;
		std::string strErrorInfo = "";
		bool bRet = CSystemEx::GetInstance()->CallOneNumberAudio(strSipNumber, true, pContactDB->m_nActiveLine, pContactDB->m_nCallHistoryID, strErrorInfo);
		if (pContactDB->m_nActiveLine < 0)
		{
			LOG4_ERROR("here cause the toolbar un enable ,CallOneNumberAudio CAudioPageCanvasHandler");
		}
		if (bRet == false)
		{
			std::wstring strTip = PortUtility::string2WString(strErrorInfo);
			std::wstring strTemplate = _T("call error:") + strTip;
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"), strTemplate.c_str(), GetHostWnd(GetBindWLWnd()));
			SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
			return false;
		}
		pContactDB->SetUserData(CT_AUDIO);
		CAudioPageCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pContactInfo);
		CSystemEx::GetInstance()->SaveSession2AudioCanvasHandler(pContactDB->m_nActiveLine, pAudioPageCanvasHandler);
		bRet = true;
	}
	else
	{
		bRet  = CallTheGroup();
	}
	
	
	return bRet;
}

LRESULT CAudioPageCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}

	return 0;
}

LRESULT CAudioPageCanvasHandler::OnChkRec(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		std::string strFilePath = CSystemEx::GetInstance()->GetGlobalOptions()->m_strRecordFolder;
		if (strFilePath.empty())
		{
			m_pChkRec->SetCheck(BST_UNCHECKED);
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("ERROR"), CSystemEx::GetInstance()->GetTipsNoRecordPathText(), GetHostWnd(GetBindWLWnd()), 3000);
			return 0;
		}
		SYSTEMTIME st;
		CIUIString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(L"%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		std::string strFileName = PortUtility::wstring2String(strDate.GetBuffer(strDate.GetLength()));
		if (m_pChkRec->GetCheck() == BST_CHECKED)
		{
			bool bRet = CSystemEx::GetInstance()->StartRecord(pContactDB->m_nActiveLine, strFilePath, strFileName);
			if (bRet==false)
			{
				m_pChkRec->SetCheck(BST_CHECKED);
			}
		}
		else
		{
			CSystemEx::GetInstance()->EndRecord(pContactDB->m_nActiveLine, pContactDB->m_strSIPNum, ENUM_VA_RECORD_AUDIO);
		}
	}
	return 0;
}
void CAudioPageCanvasHandler::MuteMicroPhineSession()
{
	bool bCheck = m_pChkMute->GetCheck() == BST_UNCHECKED ? true : false;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{

		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		long nSessionID = CSystemEx::GetInstance()->GetSessionID(pContactDB->ReturnActiveLine());
		if (bCheck)
		{
			CSystemEx::GetInstance()->MuteMicrophineSession(false, nSessionID);
		}
		else
		{
			CSystemEx::GetInstance()->MuteMicrophineSession(true, nSessionID);

		}
	}
	else
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
		auto iterTalkGroupNumber = vecTalkNumber.begin();
		for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
			if (pTalkGroupNumber != NULL)
			{
				int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
				int nSessionID = CSystemEx::GetInstance()->GetSessionID(nActiveLine);
				if (bCheck)
				{
					CSystemEx::GetInstance()->MuteMicrophineSession(false, nSessionID);
				}
				else
				{
					CSystemEx::GetInstance()->MuteMicrophineSession(true, nSessionID);
				}
			}
		}
	}
	return ;
}
LRESULT CAudioPageCanvasHandler::OnChkMute(RoutedEventArgs *pe)
{
	MuteMicroPhineSession();
	return 0;
}

LRESULT CAudioPageCanvasHandler::OnChkPause(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		bool bCheck = m_pChkPause->GetCheck() == BST_UNCHECKED ? true : false;
		if (bCheck)
		{
			m_bHoldBySelf = false;
			CSystemEx::GetInstance()->UnHold(pContactDB->ReturnActiveLine());
			m_pStaState->SetWindowText(L"");
		}
		else
		{
			m_bHoldBySelf = true;
			CSystemEx::GetInstance()->Hold(pContactDB->ReturnActiveLine());
			m_pStaState->SetWindowText(L"Hold");
		}
	}
	else
	{
		//where in group  ,ignore this  status
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
		auto iterTalkGroupNumber = vecTalkNumber.begin();
		for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
			if (pTalkGroupNumber != NULL)
			{
				int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
				bool bCheck = m_pChkMute->GetCheck() == BST_UNCHECKED ? true : false;
				if (bCheck)
				{
					CSystemEx::GetInstance()->UnHold(nActiveLine);
					CIUIString strText = m_pStaState->GetWindowText();
					if (strText.IsEmpty())
					{
						m_pStaState->SetWindowText(L"");
					}
				}
				else
				{
					CSystemEx::GetInstance()->Hold(nActiveLine);
					CIUIString strText = m_pStaState->GetWindowText();
					if (strText.IsEmpty())
					{
						m_pStaState->SetWindowText(L"Hold");
					}
				}
			}
		}
	}
	return 0;
}

LRESULT CAudioPageCanvasHandler::OnBtnTransfer(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{

		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (m_pCallingTransferDlg!=NULL)
		{
			CloseTransferDlg();
		}
		if (m_pAddCallerDlg != NULL)
		{
			CloseAddCallerDlg();
		}
		m_pCallingTransferDlg = new CCallingTransferDlg(this);

		//CCallingTransferDlg dlg;
		m_pCallingTransferDlg->SetProject(GetProject(pe->hOriginalSender));
		m_pCallingTransferDlg->SetTransferedContact(m_pContactInfo);
		m_pCallingTransferDlg->SetXml(_T("IDD_CALLING_TRANSFER.xml"));
		m_pCallingTransferDlg->DoModal();
		TransferClosed();
	}
	return 0;
}

void CAudioPageCanvasHandler::ProcessTransferFailed()
{
	CloseAddCallerDlg();
}

void CAudioPageCanvasHandler::ProcessAddContactFailed()
{
	CloseAddCallerDlg(); 
}
LRESULT CAudioPageCanvasHandler::OnBtnAdd(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_MULTI_CONTACT)
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		int nSize = pTalkGroup->GetMemberCount();
		if (nSize >= MAX_MERBER_CONFRENCE)
		{
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", CSystemEx::GetInstance()->GetConfrenceNumberMaxText(), NULL, 5000);
			return 0;
		}
		
	}
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		if (m_pCallingTransferDlg != NULL)
		{
			CloseTransferDlg();
		}
		if (m_pAddCallerDlg != NULL)
		{
			CloseAddCallerDlg();
		}
		m_pAddCallerDlg = new CAddCallerDlg(this,ENUM_CONFENCE_AUDIO);
		m_pAddCallerDlg->SetCurrentContact(m_pContactInfo);
		m_pAddCallerDlg->SetProject(GetProject(pe->hOriginalSender));
		m_pAddCallerDlg->SetXml(_T("IDD_CALLING_TRANSFER.xml"));
		m_pAddCallerDlg->DoModal();
	}
	return 0;

}
LRESULT CAudioPageCanvasHandler::OnBtnKeyboard(RoutedEventArgs *pe)
{
	if (m_pContactInfo==NULL)
	{
		return 0;
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB==NULL)
		{
			return 0;
		}
		if (m_pDTMFKeyboardWnd != NULL)
		{
			if (::IsWindow(m_pDTMFKeyboardWnd->GetSafeHwnd())==TRUE)
			{
				EndDialog(m_pDTMFKeyboardWnd->GetSafeHwnd(), IDOK);
			}
		};
		m_pDTMFKeyboardWnd = new CMiniKeyboardWnd();
		m_pDTMFKeyboardWnd->SetProject(GetProject(GetBindWLWnd()));
		m_pDTMFKeyboardWnd->CreatePopupWindow(_T("IDW_MINI_KEYBOARD.xml"), GetHostWnd(GetBindWLWnd()));
		m_pDTMFKeyboardWnd->SetActiveLine(pContactDB->ReturnActiveLine());
		CIUIRect rcButton;
		IUI::GetWindowRect(m_pBtnKeyboard->GetSafeHwnd(), rcButton);

		CIUIRect rcDlg;
		::GetWindowRect(m_pDTMFKeyboardWnd->m_hWnd, rcDlg);

		rcDlg.OffsetRect(rcButton.left - rcDlg.left - (rcDlg.Width() - rcButton.Width()) / 2,
			rcButton.top - rcDlg.top - rcDlg.Height());
		::MoveWindow(m_pDTMFKeyboardWnd->m_hWnd, rcDlg.left, rcDlg.top, rcDlg.Width(), rcDlg.Height(), FALSE);

		m_pDTMFKeyboardWnd->Invalidate();
	}
	return 0;
}

LRESULT CAudioPageCanvasHandler::OnBtnShare(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->TipsFunNoRun();
	return 0;
}

LRESULT CAudioPageCanvasHandler::OnBtnMsg(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAudioPageCanvasHandler::OnBtnNoVideo(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->TipsFunNoRun();
	return 0;
}
void CAudioPageCanvasHandler::ResetBtnStatus()
{
	SetFunBtnStat(FALSE);
}
void CAudioPageCanvasHandler::CloseDTMF()
{
	if (m_pDTMFKeyboardWnd != NULL)
	{
		if (::IsWindow(m_pDTMFKeyboardWnd->GetSafeHwnd()) == TRUE)
		{
			m_pDTMFKeyboardWnd->EndDialog(IDOK);
			//::EndDialog(m_pDTMFKeyboardWnd->GetSafeHwnd(), IDOK);
			OutputDebugString(_T("guanbi \n"));
		}
	};
	m_pDTMFKeyboardWnd = NULL;
}
void CAudioPageCanvasHandler::CleanChkStatus()
{
	//REC
	bool bChechEnable = m_pChkRec->IsWindowEnabled();
	int nStatus = m_pChkRec->GetCheck();
	if (m_pChkRec->GetCheck()== BST_CHECKED)
	{
		m_pChkRec->SetCheck(BST_UNCHECKED);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			CSystemEx::GetInstance()->EndRecord(pContactDB->m_nActiveLine, pContactDB->m_strSIPNum, ENUM_VA_RECORD_AUDIO);
		}
	}
	//MUTE
	if (m_pChkMute->GetCheck()== BST_CHECKED)
	{
		m_pChkMute->SetCheck(BST_UNCHECKED);
		MuteMicroPhineSession();
		//CSystemEx::GetInstance()->MuteMicrophine(false);
	}
	//Hold
	if (m_pChkPause->GetCheck()== BST_CHECKED)
	{
		m_pChkPause->SetCheck(BST_UNCHECKED);
	}
	//Lebel Text
	m_pStaState->SetWindowText(L"");
}
void CAudioPageCanvasHandler::ProcessSwtichLine(bool bHold)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	vector<int > vecActiveLine;
	if (m_pChkRec==NULL)
	{
		return;
	}
	if (m_pChkPause->IsWindowVisible()!=TRUE)
	{
		return;
	}
	bool bCheck = m_pChkPause->GetCheck() == BST_UNCHECKED ? false : true;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		int nActiveLine = m_pContactInfo->m_unionContactInfo.m_pContact->ReturnActiveLine();
		if (nActiveLine>0)
		{
			vecActiveLine.push_back(nActiveLine);
			if (bHold)
			{
				if (bCheck==false&&m_bHoldBySelf==false)
				{
					m_pChkPause->SetCheck(BST_CHECKED);
					m_pStaState->SetWindowText(L"Hold");
					CSystemEx::GetInstance()->SwtichLineUpdateHold(vecActiveLine);
				}
			}
			else
			{
				if (bCheck&&m_bHoldBySelf == false)
				{
					m_pStaState->SetWindowText(L"");
					CSystemEx::GetInstance()->SwtichLineUpdateUnHold(vecActiveLine);
					m_pChkPause->SetCheck(BST_UNCHECKED);
				}
			}
		}
	}
	else
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
		auto iterTalkGroupNumber = vecTalkNumber.begin();

		bool bAllFail = false;
		int nFailCount = 0;
		for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
			if (pTalkGroupNumber != NULL)
			{
				int nHistoryID = pTalkGroupNumber->GetCallHistoryID();
				int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
				vecActiveLine.push_back(nActiveLine);
			}
		}

		if (bHold&&vecActiveLine.size() > 0)
		{
			if (bCheck == false && m_bHoldBySelf == false)
			{
				m_pChkPause->SetCheck(BST_CHECKED);
				CSystemEx::GetInstance()->SwtichLineUpdateHold(vecActiveLine);
			}
		}
		else if (vecActiveLine.size() > 0 && bHold == false)
		{
			if (bCheck&&m_bHoldBySelf == false)
			{
				CSystemEx::GetInstance()->SwtichLineUpdateUnHold(vecActiveLine);
				m_pChkPause->SetCheck(BST_UNCHECKED);
			}
		}
	}
}
void CAudioPageCanvasHandler::DoSingleContactHangup(bool bBtnSelf , bool bJumpPage )
{
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

	if (pContactDB->ReturnActiveLine() < 0)
	{
		LOG4_INFO("the active line is error where hangup by self");
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
		pContactDB->SetActiveLine(-1);
		return;
	}
	

	ENUM_CALL_STATUS enumCallStatus;
	if (GetIsConnected(pContactDB->ReturnActiveLine()) == true)
	{
		enumCallStatus = ENUM_CALL_STATUS_SUCC;
	}
	else
	{
		enumCallStatus = ENUM_CALL_STATUS_FAILD;
	}
	ResetBtnStatus();

	CleanChkStatus();

	
	bool bTransferShouldChangeContactPage = false,bAddShouldChangeContactPage = false;
	bool bIsStartTransferContact = true, bIsAddContact = true;
	TAG_RECENT *pTransferContact = NULL;
	TAG_RECENT *pAddContact = NULL;
	if (m_pContactInfo->m_enumRecentContact == ENUM_SINGLE_CONTACT)
	{
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

		if (m_pContactInfo->m_unionContactInfo.m_pContact->GetIsTransfer())
		{
			CCallingTransferDlg* pCalingTransferDlg = (CCallingTransferDlg*)m_pContactInfo->m_unionContactInfo.m_pContact->GetTransferDlg();
			if (pCalingTransferDlg!=NULL)
			{
				if (::IsWindow(pCalingTransferDlg->m_hWnd))
				{
					pCalingTransferDlg->EndDialog(IDOK);
				}
				
			}
			pTransferContact = (TAG_RECENT*)m_pContactInfo->m_unionContactInfo.m_pContact->GetTransferContact();
			bIsStartTransferContact = m_pContactInfo->m_unionContactInfo.m_pContact->GetIsStartedContact();
			if (pTransferContact != NULL)
			{
				CHAT_TAB enumCallType = pCalingTransferDlg->GetTransferType();
				CAudioPageCanvasHandler* pAudioPage = (CAudioPageCanvasHandler*)pCalingTransferDlg->GetSrcCalingPage();
				if (pAudioPage&&enumCallType == CT_AUDIO)
				{
					pAudioPage->ProcessTransferFailed();
				}

				

				if (bBtnSelf == false)
				{

					CIUIString strErrorTip;
					strErrorTip.Format(L"call closed by %s  ", strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
					

					CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", strErrorTip, NULL);
				}
				bTransferShouldChangeContactPage = true;
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsTransfer(NULL, NULL, FALSE,FALSE);
			}
		}
		else if (pContactDB->GetIsAdded() == true)
		{
			
			CAddCallerDlg* pAddConfenceDlg = (CAddCallerDlg*)m_pContactInfo->m_unionContactInfo.m_pContact->GetAddConfenceDlg();
			if (pAddConfenceDlg != NULL)
			{
				if (::IsWindow(pAddConfenceDlg->m_hWnd))
				{
					pAddConfenceDlg->EndDialog(IDOK);
				}

			}
			pAddContact = (TAG_RECENT*)m_pContactInfo->m_unionContactInfo.m_pContact->GetAddContact();
			bIsAddContact = m_pContactInfo->m_unionContactInfo.m_pContact->GetIsStartedContact();
			if (pAddContact != NULL)
			{
				CHAT_TAB enumCallType = pAddConfenceDlg->GetAddType();
				CAudioPageCanvasHandler* pAudioPage = (CAudioPageCanvasHandler*)pAddConfenceDlg->GetSrcCalingPage();
				if (pAudioPage&&enumCallType == CT_AUDIO)
				{
					pAudioPage->ProcessTransferFailed();
				}

				if (bBtnSelf == false)
				{

					CIUIString strErrorTip;
					strErrorTip.Format(L"call closed by %s ", strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
					

					CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", strErrorTip, NULL);
				}
				
				bAddShouldChangeContactPage = true;
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsAddedConfence(NULL, NULL, FALSE, FALSE);
			}
		}
		
	}
	CSystemEx::GetInstance()->Hangup(pContactDB->ReturnActiveLine());
	//CAudioPageCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pContact);
	pContactDB->SetUserData(CT_TEXT_CHAT);
	pContactDB->SetCallType(ENUM_CALL_IN);
	SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	
	if (bTransferShouldChangeContactPage == true&&pTransferContact!= m_pContactInfo&& bIsStartTransferContact !=true&& bJumpPage==false)
	{
		bool bIsConnected = pTransferContact->m_unionContactInfo.m_pContact->m_bIsConnected;
		if (bIsConnected)
		{
			SwitchToContact(pTransferContact, CT_AUDIO);
		}
	}
	if (bAddShouldChangeContactPage == true && pAddContact != m_pContactInfo && bIsAddContact != true && bJumpPage == false)
	{
		bool bIsConnected = pAddContact->m_unionContactInfo.m_pContact->m_bIsConnected;
		if (bIsConnected)
		{
			SwitchToContact(pAddContact, CT_AUDIO);
		}
		
	}
	if (pContactDB->m_nCallHistoryID != -1)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgEndTimeByID(pContactDB->m_nCallHistoryID);
		CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(pContactDB->m_nCallHistoryID, enumCallStatus);
		bool bFindRecord = false;
		CChatRecord theHistoryRecord = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByCallHistoryID(pContactDB->m_nCallHistoryID, bFindRecord);
		if (bFindRecord==true)
		{
			CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(pContactDB->m_strSIPNum,theHistoryRecord, INSERT_FORWARD);
		}
		
	}
	CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
	if (pInboxCanvasHandler != NULL)
	{
		CCallHistory oLocalCallHistory;
		bool bRet = CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(pContactDB->m_nCallHistoryID, oLocalCallHistory);
		if (bRet)
		{
			if (enumCallStatus == ENUM_CALL_STATUS_FAILD)
			{
				pInboxCanvasHandler->Insert2Miss(oLocalCallHistory);
			}
			pInboxCanvasHandler->Insert2Call(oLocalCallHistory);
		}
	}
	pContactDB->m_nCallHistoryID = -1;
	if (m_hSessionTimer != 0)
	{
		::KillTimer(NULL, m_hSessionTimer);
		m_hSessionTimer = 0;
		m_pStaSign->ShowWindow(SW_HIDE);
		CSystemEx::GetInstance()->ReSetCallTime(pContactDB->ReturnActiveLine());
	}
	pContactDB->m_bIsConnected = false;
	CSystemEx::GetInstance()->EraseSession2CanvasHandler(pContactDB->m_nActiveLine);
	pContactDB->SetActiveLine(-1);

	CloseDTMF();
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);

	CloseAddCallerDlg();
	CloseTransferDlg();
	
}
bool CAudioPageCanvasHandler::GetIsConnectedNoLine()
{
	bool bRet = false;
	if (m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT)
	{
		auto vecGroupNumber = m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetContact();
		auto iterVecGroupNumber = vecGroupNumber.begin();
		for (iterVecGroupNumber; iterVecGroupNumber != vecGroupNumber.end(); iterVecGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterVecGroupNumber;
			bRet = pTalkGroupNumber->m_bIsConnected;
			if (bRet == true)
			{
				break;
			}
		}
	}
	else
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		bRet = pContactDB->m_bIsConnected;
	}
	return bRet;
}
void CAudioPageCanvasHandler::ExistOneLine(int nExitActiveLine,bool bConnected)
{
	CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
	vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
	auto iterTalkGroupNumber = vecTalkNumber.begin();

	bool bAllFail = false;
	int nFailCount = 0;
	for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
	{
		CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
		if (pTalkGroupNumber != NULL)
		{
			int nHistoryID = pTalkGroupNumber->GetCallHistoryID();
			int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
		
			//CContactDB *pContactDB = pTalkGroupNumber->GetContactDB();
			//pContactDB->
			if (nExitActiveLine == nActiveLine)
			{
				ENUM_CALL_STATUS enumCallStatus;
				if (GetIsConnected(nActiveLine) == true)
				{
					enumCallStatus = ENUM_CALL_STATUS_SUCC;
				}
				else
				{
					enumCallStatus = ENUM_CALL_STATUS_FAILD;
				}
				CSystemEx::GetInstance()->Hangup(nActiveLine);
				if (nHistoryID != -1)
				{
					CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgEndTimeByID(nHistoryID);
					CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(nHistoryID, enumCallStatus);
					bool bFindRecord = false;
					CChatRecord theHistoryRecord = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByCallHistoryID(nHistoryID, bFindRecord);
					if (bFindRecord == true)
					{
						CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(pTalkGroupNumber->m_strSIPNumber, theHistoryRecord, INSERT_FORWARD);
					}
				}

				CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
				if (pInboxCanvasHandler != NULL)
				{
					CCallHistory oLocalCallHistory;
					bool bRet = CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(pTalkGroupNumber->GetCallHistoryID(), oLocalCallHistory);
					if (bRet)
					{
						if (enumCallStatus == ENUM_CALL_STATUS_FAILD)
						{
							pInboxCanvasHandler->Insert2Miss(oLocalCallHistory);
						}
						pInboxCanvasHandler->Insert2Call(oLocalCallHistory);
						

					}
				}

				CSystemEx::GetInstance()->EraseSession2CanvasHandler(pTalkGroupNumber->m_nActiveLine);

				pTalkGroupNumber->ResetCallHistortID();
				pTalkGroupNumber->ResetActiveLine();
				pTalkGroupNumber->ClearAddConfenceInfo();
				pTalkGroupNumber->m_bIsConnected = false;
				
				
			}
			
			if (pTalkGroupNumber->GetIsConnected() == false)
			{
				nFailCount++;
			}
			
		}
	}

	if (nFailCount==vecTalkNumber.size())
	{
		pTalkGroup->SetUserData(CT_TEXT_CHAT);
		SetFunBtnStat(FALSE, TRUE);
		CleanChkStatus();
		if (m_hSessionTimer != 0)
		{
			::KillTimer(NULL, m_hSessionTimer);
			m_hSessionTimer = 0;
			m_pStaSign->ShowWindow(SW_HIDE);
			CSystemEx::GetInstance()->ReSetCallTime(GROUP_LINE);
		}
		CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
		CSystemEx::GetInstance()->DestoryConfence();
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	}
}
void CAudioPageCanvasHandler::DoTalkGroupHangup()
{
	CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
	vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
	auto iterTalkGroupNumber = vecTalkNumber.begin();
	for (iterTalkGroupNumber; iterTalkGroupNumber!=vecTalkNumber.end();iterTalkGroupNumber++)
	{
		CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
		if (pTalkGroupNumber!=NULL)
		{
			int nHistoryID = pTalkGroupNumber->GetCallHistoryID();
			int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
			
			ENUM_CALL_STATUS enumCallStatus;
			if (GetIsConnected(nActiveLine) == true)
			{
				enumCallStatus = ENUM_CALL_STATUS_SUCC;
			}
			else
			{
				enumCallStatus = ENUM_CALL_STATUS_FAILD;
			}
			CSystemEx::GetInstance()->Hangup(nActiveLine);
			if (nHistoryID != -1)
			{
				CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgEndTimeByID(nHistoryID);
				CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(nHistoryID, enumCallStatus);
				bool bFindRecord = false;
				CChatRecord theHistoryRecord = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByCallHistoryID(nHistoryID, bFindRecord);
				if (bFindRecord == true)
				{
					CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(pTalkGroupNumber->m_strSIPNumber, theHistoryRecord, INSERT_FORWARD);
				}
			}

			CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
			if (pInboxCanvasHandler != NULL)
			{
				CCallHistory oLocalCallHistory;
				bool bRet = CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(pTalkGroupNumber->m_nCallHistoryID, oLocalCallHistory);
				if (bRet)
				{
					if (enumCallStatus == ENUM_CALL_STATUS_FAILD)
					{
						pInboxCanvasHandler->Insert2Miss(oLocalCallHistory);
					}
					pInboxCanvasHandler->Insert2Call(oLocalCallHistory);
				}
			}
			CSystemEx::GetInstance()->EraseSession2CanvasHandler(pTalkGroupNumber->m_nActiveLine);
			pTalkGroupNumber->ResetCallHistortID();
			pTalkGroupNumber->ResetActiveLine();
			pTalkGroupNumber->ClearAddConfenceInfo();
			
		}
	}
	pTalkGroup->SetUserData(CT_TEXT_CHAT);
	SetFunBtnStat(FALSE, TRUE);
	CleanChkStatus();
	if (m_hSessionTimer != 0)
	{
		::KillTimer(NULL, m_hSessionTimer);
		m_hSessionTimer = 0;
		m_pStaSign->ShowWindow(SW_HIDE);
		CSystemEx::GetInstance()->ReSetCallTime(GROUP_LINE);
	}
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
	//
	CloseAddCallerDlg();
	CloseTransferDlg();
	CSystemEx::GetInstance()->DestoryConfence();
	SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);

}
void CAudioPageCanvasHandler::CloseTransferDlg()
{
	if (m_pCallingTransferDlg!=NULL)
	{
		if (::IsWindow(m_pCallingTransferDlg->m_hWnd))
		{
			m_pCallingTransferDlg->EndDialog(IDOK);
			//delete m_pCallingTransferDlg;
			m_pCallingTransferDlg = NULL;
		}
	}
}
void CAudioPageCanvasHandler::CloseAddCallerDlg()
{
	if (m_pAddCallerDlg != NULL)
	{
		if (::IsWindow(m_pAddCallerDlg->m_hWnd))
		{
			m_pAddCallerDlg->EndDialog(IDOK);
			//delete m_pAddCallerDlg;
			m_pAddCallerDlg = NULL;
		}
	}
}
void CAudioPageCanvasHandler::DoHangup(bool bBtnSelf,bool bJumpPage)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		DoSingleContactHangup(bBtnSelf,bJumpPage);
	}
	else
	{
		DoTalkGroupHangup();
	}

	
}
LRESULT CAudioPageCanvasHandler::OnBtnHangup(RoutedEventArgs *pe)
{
	DoHangup(true);
	return 0;
}
void   CAudioPageCanvasHandler::AnswerByRemote(int nActiveLine,bool bConferenceRoom)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		if (pContactDB->m_nCallHistoryID != -1)
		{
			CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(pContactDB->m_nCallHistoryID, ENUM_CALL_STATUS_SUCC);
		}

		
		pContactDB->m_bIsConnected = true;
		if (pContactDB->GetIsTransfer()==true)
		{
			CCallingTransferDlg* pTransferDlg = (CCallingTransferDlg*)pContactDB->GetTransferDlg();
			if (pTransferDlg != NULL)
			{
				if (IsWindow(pTransferDlg->m_hWnd))
				{
					pTransferDlg->SetDesPagePointer(this);
					pTransferDlg->ProcessTranseferSuccess(true);
				}
				else
				{
					SwitchToContact(m_pContactInfo, CT_AUDIO);
				}
				
				//pTransferDlg->DoAttendTransferOk();
			}

			//DoHangup();
		}
		else if (pContactDB->GetIsAdded() == true)
		{
			CAddCallerDlg* pAddContactDlg = (CAddCallerDlg*)pContactDB->GetAddConfenceDlg();

			ENUM_RECENT_CONTACT_TYPE enumRecentContactType = pAddContactDlg->GetSrcRecentContactType();
			if (enumRecentContactType == ENUM_SINGLE_CONTACT)
			{
				if (IsWindow(pAddContactDlg->m_hWnd))
				{
					pAddContactDlg->SetDesPagePointer(this);
					pAddContactDlg->DoAddedContact();
				}
				else
				{
					SwitchToContact(m_pContactInfo, CT_AUDIO);
				}
			}
			else
			{
				if (IsWindow(pAddContactDlg->m_hWnd))
				{
					CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
					if (pTalkGroup)
					{
						CSystemEx::GetInstance()->joinConference(nActiveLine);
					}
					pAddContactDlg->DoAddedContact();
				}
				else
				{
					SwitchToContact(m_pContactInfo, CT_AUDIO);
				}
			}
		}
		SetConnected(true, nActiveLine);
		BeginTimer();
	}
	else
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		if (pTalkGroup)
		{
			CSystemEx::GetInstance()->joinConference(nActiveLine);
			SetConnected(true, nActiveLine);
		}
	}
	SetIsConference(bConferenceRoom);

	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
}
void    CAudioPageCanvasHandler::UnHoldByRemote()
{
	/*
	//because in moh mode £¬server provide wrong status 
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB->ReturnActiveLine() < 0)
		{
			LOG4_INFO("the active line is error where hangup by remote");
			return;
		}
		CIUIString strText = m_pStaState->GetWindowText();
		if (strText.Compare(L"Hold By Remote/Hold")==0)
		{
			m_pStaState->SetWindowText(L"Hold");
		}
		else
		{
			m_pStaState->SetWindowText(L"");
		}
	}*/
}
void    CAudioPageCanvasHandler::HoldByRemote()
{
	return;
}
void   CAudioPageCanvasHandler::ProcessTranseferSuccess(bool bSuccess, int nErrorCode)
{
	if (m_pCallingTransferDlg != NULL)
	{
		m_pCallingTransferDlg->ProcessTranseferSuccess(bSuccess,  nErrorCode);
	}
	if (bSuccess==false)
	{
		if (m_pContactInfo->m_enumRecentContact == ENUM_SINGLE_CONTACT)
		{
			if (m_pContactInfo->m_unionContactInfo.m_pContact->GetIsTransfer())
			{
				CCallingTransferDlg* pCalingTransferDlg = (CCallingTransferDlg*)m_pContactInfo->m_unionContactInfo.m_pContact->GetTransferDlg();
				if (pCalingTransferDlg != NULL)
				{
					if (::IsWindow(pCalingTransferDlg->m_hWnd))
					{
						pCalingTransferDlg->EndDialog(IDOK);
					}

				}
				TAG_RECENT *pTransferContact = (TAG_RECENT*)m_pContactInfo->m_unionContactInfo.m_pContact->GetTransferContact();
				if (pTransferContact != NULL)
				{

				}
				CHAT_TAB enumCallType = pCalingTransferDlg->GetTransferType();
				CAudioPageCanvasHandler* pAudioPage = (CAudioPageCanvasHandler*)pCalingTransferDlg->GetSrcCalingPage();

				if (pAudioPage&&enumCallType == CT_AUDIO)
				{
					pAudioPage->ProcessTransferFailed();
				}
				SwitchToContact(pTransferContact, CT_AUDIO);
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsTransfer(NULL, NULL, FALSE,FALSE);
			}
		}
	}
}
void CAudioPageCanvasHandler::TransferClosed()
{
	if (m_pCallingTransferDlg!=NULL)
	{
		m_pCallingTransferDlg = NULL;
	}
}
void CAudioPageCanvasHandler::HangupByRemote(bool bConnected,int nActiveLine, bool bJumpPage)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB* pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB->GetIsTransfer())
		{
			CCallingTransferDlg* pTransferDlg = (CCallingTransferDlg*)pContactDB->GetTransferDlg();
			if (pTransferDlg != NULL)
			{
				pTransferDlg->ProcessTranseferSuccess(false);
			}
		}
		else if (pContactDB->GetIsAdded())
		{
			//CAddCallerDlg* pAddContactDlg = (CAddCallerDlg*)pContactDB->GetAddConfenceDlg();
			//if (pAddContactDlg != NULL)
			//{
				//make here 
				//pAddContactDlg->DoAddedContact();
			//}
		}
	}
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		DoSingleContactHangup(false, bJumpPage);
	}
	else
	{
		ExistOneLine(nActiveLine, bConnected);
	}
	return;
}
void CAudioPageCanvasHandler::CloseWnd()
{
	CleanChkStatus();
	ResetBtnStatus();
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactInfo);
		//CSystemEx::GetInstance()->EraseSession2CanvasHandler(pContactDB->m_nActiveLine);
		pContactDB->m_bIsConnected = false;
		pContactDB->m_nCallHistoryID = -1;

		CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
		CSystemEx::GetInstance()->EraseSession2CanvasHandler(pContactDB->m_nActiveLine);
		pContactDB->SetActiveLine(-1);
		
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	}

}

void CAudioPageCanvasHandler::AddContact2Group(TAG_RECENT * pConctactInfo)
{
	if (m_pContactInfo->m_enumRecentContact!=ENUM_MULTI_CONTACT)
	{
		return;
	}
	if (pConctactInfo->m_enumRecentContact!=ENUM_SINGLE_CONTACT)
	{
		return;
	}
	CContactDB *pContact = pConctactInfo->m_unionContactInfo.m_pContact;
}
void CAudioPageCanvasHandler::ChangeDeviceSwtich(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{

		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		long nSessionID = CSystemEx::GetInstance()->GetSessionID(pContactDB->ReturnActiveLine());
	
		CSystemEx::GetInstance()->MuteSession(!bEnableAudioOut,! bEnableAudioIn, !bEnableCamera, nSessionID);
		
	}
	else
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
		auto iterTalkGroupNumber = vecTalkNumber.begin();
		for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
			if (pTalkGroupNumber != NULL)
			{
				int nActiveLine = pTalkGroupNumber->ReturnActiveLine();
				int nSessionID = CSystemEx::GetInstance()->GetSessionID(nActiveLine);
				CSystemEx::GetInstance()->MuteSession(!bEnableAudioOut, !bEnableAudioIn, !bEnableCamera, nSessionID);
			}
		}
	}
}
LRESULT CAudioPageCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkRec->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkRec(pe);
	}
	else if (m_pChkMute->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMute(pe);
	}
	else if (m_pChkPause->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkPause(pe);
	}
	else if (m_pBtnTransfer->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnTransfer(pe);
	}
	else if (m_pBtnAdd->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnAdd(pe);
	}
	else if (m_pBtnKeyboard->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnKeyboard(pe);
	}
	else if (m_pBtnShare->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnShare(pe);
	}
	else if (m_pBtnMsg->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMsg(pe);
	}
	else if (m_pBChkNoVideo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNoVideo(pe);
	}
	else if (m_pBtnHangup->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnHangup(pe);
	}

	return 0;
}

LRESULT CAudioPageCanvasHandler::OnAfterPaint(RoutedEventArgs *pe)
{
	if (NULL != m_pContactInfo && m_pStaFace->GetSafeHwnd() == pe->hOriginalSender)
	{
		CIUIRect rc;
		GetClientRect(pe->hOriginalSender, rc);
		if (m_pContactInfo == NULL)
		{
			return 0;
		}
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

			if (m_pStaFace->GetSafeHwnd() == pe->hOriginalSender)
			{
				DrawFace((HDC)pe->lParam, rc, (PortUtility::Utf82Unicode_ND(pContactDB->m_strHeadImage).c_str())); 
				HFONT hDefFont = FontManager::CreateIUIFont(GetProject(GetBindWLWnd()), _T("IDF_MAX_NAME"));
				_ASSERT(hDefFont != NULL);

				CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
				FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
				if (pContactDB->GetSetUserFaceImage() == false)
				{
					CIUIString strLeft = strSrcUserName.Left(1);
					DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
						CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont, RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
				}
			}
		}
		else
		{
			DrawGroupFace((HDC)pe->lParam, rc, m_pContactInfo);
		}
	}
	return 0;
}

int CAudioPageCanvasHandler::SetContactInfo(TAG_RECENT *pContactInfo)
{
	m_pContactInfo = pContactInfo;
	return 0;
}

TAG_RECENT *CAudioPageCanvasHandler::GetContactInfo()
{
	return m_pContactInfo;
}

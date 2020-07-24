#include "stdafx.h"
#include "CallerBarWnd.h"
#include "System/SystemEx.h"
#include "./MainFrameTabControl/MainPanel/MainCanvasHandler.h"
CCallerBarWnd::CCallerBarWnd()
	: m_pStaFace(NULL)
	, m_pStaCaller(NULL)
	, m_pBtnAudio(NULL)
	, m_pBtnVideo(NULL)
	, m_pBtnHangup(NULL)
	, m_pCallBackParameters(NULL)
	, m_pContactDBInfo (NULL)
	, m_nSessionID(-1)
	, m_pStaPhoneNumber(NULL)
	, m_pStaSign(NULL)
	, m_pStaCall(NULL)
	, m_pStaSplitter(NULL)
	, m_pStaCall2(NULL)
	, m_pStaCaller2(NULL)
	, m_pStaPhoneNumber2(NULL)
	, m_pStaSign2(NULL)
{
	m_bIsConfrence = false;
	m_nActiveLine = -1;
	m_enumCallType = CALL_PHONE;
}

CCallerBarWnd::~CCallerBarWnd()
{
}

int CCallerBarWnd::InitControls()
{
	
	m_pStaFace = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_FACE")));
	_ASSERT(m_pStaFace != NULL);
	m_pStaCaller = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_CALLER")));
	_ASSERT(m_pStaCaller != NULL);
	m_pBtnAudio = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_AUDIO")));
	_ASSERT(m_pBtnAudio != NULL);
	m_pBtnVideo = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_VIDEO")));
	_ASSERT(m_pBtnVideo != NULL);
	m_pBtnHangup = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_HANGUP")));
	_ASSERT(m_pBtnHangup != NULL);
	m_pStaPhoneNumber = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_PHONE_NUMBER")));
	_ASSERT(m_pStaPhoneNumber != NULL);
	m_pStaSign = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SIGN")));
	_ASSERT(m_pStaSign != NULL);
	m_pStaCall = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_CALL")));
	_ASSERT(m_pStaCall != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pStaCall2 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_CALL2")));
	_ASSERT(m_pStaCall2 != NULL);
	m_pStaCaller2 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_CALLER2")));
	_ASSERT(m_pStaCaller2 != NULL);
	m_pStaPhoneNumber2 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_PHONE_NUMBER2")));
	_ASSERT(m_pStaPhoneNumber2 != NULL);
	m_pStaSign2 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SIGN2")));
	_ASSERT(m_pStaSign2 != NULL);
	InitControlData();
	return 0;
}

void CCallerBarWnd::InitControlData()
{
	if (m_pCallBackParameters != NULL)
	{
		
		if (m_oProcessComing.m_strSIPNum.empty())
		{
			std::string strErrorInfo = "Can't find Sip Num In recv call Coming";
			LOG4_INFO(strErrorInfo.c_str());
			return ;
		}

		if (m_pContactDBInfo == NULL)
		{
			m_pContactDBInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(m_oProcessComing.m_strSIPNum);
		}
		if (m_pContactDBInfo==NULL)
		{
			std::string strErrorInfo = "Can't find Sip Num In recv call Coming FindContactDBBySipNumber";
			LOG4_INFO(strErrorInfo.c_str());
			m_pContactDBInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(m_oProcessComing.m_strSIPNum);
		}
	
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pContactDBInfo->m_unionContactInfo.m_pContact);

		m_pStaPhoneNumber->SetWindowText(strSrcSipNumber);
	
		
		m_pStaCaller->SetWindowText(strSrcUserName);
		m_nSessionID = m_pCallBackParameters->getSessionId();

		if (m_pCallBackParameters->getExistsVideo() == true)
		{
			m_pStaCall->SetWindowText(L"Video Call");
			IUI::ShowWindow(m_pBtnVideo->GetSafeHwnd(), SW_SHOW);
			m_enumCallType = CALL_VIDEO;
		}
		else
		{
			m_pStaCall->SetWindowText(L"Audio Call");
			IUI::ShowWindow(m_pBtnVideo->GetSafeHwnd(), SW_HIDE);
			m_enumCallType = CALL_PHONE;
		}
		m_pStaSign->SetWindowText(L"PortGO Client");
		m_pStaCall2->SetWindowText(L"Business Info");
		IUI::ShowWindow(m_pBtnAudio->GetSafeHwnd(), SW_SHOW);


		CSystemEx::GetInstance()->RefreshRecentTime(m_pContactDBInfo);
	}
}
LRESULT CCallerBarWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	CenterWindow(); 
	return 0;
}

LRESULT CCallerBarWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_NCDESTROY == uMsg)
	{
		delete this;
		return 0;
	}
	if (WM_VIDEO_SHOW_OK==uMsg)
	{
		int nIndex = (int)wParam;
		HWND hVideoWnd = (HWND)lParam;
		CSystemEx::GetInstance()->Answer(true, nIndex, hVideoWnd);
	}
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CCallerBarWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CCallerBarWnd::OnBtnAudio(RoutedEventArgs *pe)
{
	bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	HWND hMainDlg = CSystemEx::GetInstance()->GetMainDlg()->m_hWnd;
	if (bMinSize)
	{
		SetForegroundWindow(hMainDlg);
		::SendMessage(hMainDlg,WM_SYSCOMMAND, SC_RESTORE,0);
	}
	else
	{
		if (bVisible == FALSE)
		{
			::ShowWindow(hMainDlg, SW_SHOW);
		}
	}

	if (m_pContactDBInfo == NULL)
	{
		LOG4_INFO("somthing error in caller wnd");
		return 0;
	}
	int nIndex = CSystemEx::GetInstance()->FindSessionBySessionID(m_nSessionID);
	
	CSystemEx::GetInstance()->Answer(false, nIndex);
	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_IN);
	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetActiveLine(nIndex);

	

	SwitchToContact(m_pContactDBInfo, CT_AUDIO);
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pContactDBInfo);
	pAudioPageCanvasHandler->SetIsConference(m_bIsConfrence);
	pAudioPageCanvasHandler->SetConnected(true, nIndex);
	pAudioPageCanvasHandler->BeginTimer();
	CSystemEx::GetInstance()->SaveSession2AudioCanvasHandler(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_nActiveLine, pAudioPageCanvasHandler);
	
	
	bool bEnableAudioInputDev = CSystemEx::GetInstance()->GetAudioInEnable();
	bool bEnableAudioOutputDev = CSystemEx::GetInstance()->GetAudioOutEnable();
	bool bEnableVideoDev = CSystemEx::GetInstance()->GetCameraEnable();
	CSystemEx::GetInstance()->MuteSession(!bEnableAudioOutputDev, !bEnableAudioInputDev, !bEnableVideoDev, m_nSessionID);
	CCallHistory theRecvCallHistory;
	theRecvCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_SUCC;
	theRecvCallHistory.m_enumCallType = ENUM_CALL_IN;
	theRecvCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theRecvCallHistory.m_strSIPNum = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum;
	theRecvCallHistory.m_enumContentType = CALL_PHONE;
	theRecvCallHistory.m_bRead = true;
	int nRecordPrimaryKeyID = -1;
	CSystemEx::GetInstance()->GetDBOprate()->saveCallMsg(theRecvCallHistory, nRecordPrimaryKeyID);
	if (pAudioPageCanvasHandler!=NULL)
	{
		m_pContactDBInfo->m_unionContactInfo.m_pContact->SetHistoryID(nRecordPrimaryKeyID);
	}
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;
	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = ENUM_MSG_AUDIO;
	theLocalChatRecord.m_strTalkerURL = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_READ;
	CSystemEx::GetInstance()->GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	CSystemEx::GetInstance()->GetCallerBarWndMGR().CloseCallerBarWnd(this);

	DestroyWindow(m_hWnd);
	pe->bHandled = true;

	return 0;
}
ICallbackParameters * CCallerBarWnd::GetParameters()
{ 
	return m_pCallBackParameters; 
};

void CCallerBarWnd::CloseByRemote()
{
	CCallHistory theRecvCallHistory;
	theRecvCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	theRecvCallHistory.m_enumCallType = ENUM_CALL_IN;
	theRecvCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theRecvCallHistory.m_strSIPNum = m_oProcessComing.m_strSIPNum;
	theRecvCallHistory.m_enumContentType = CALL_PHONE;
	theRecvCallHistory.m_bRead = false;
	int nRecordPrimaryKeyID = -1;
	CSystemEx::GetInstance()->GetDBOprate()->saveCallMsg(theRecvCallHistory, nRecordPrimaryKeyID);
	theRecvCallHistory.m_nCallHistoryID = nRecordPrimaryKeyID;


	CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(nRecordPrimaryKeyID,theRecvCallHistory);
	CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
	if (pInboxCanvasHandler != NULL)
	{
		pInboxCanvasHandler->Insert2Miss(theRecvCallHistory);
		pInboxCanvasHandler->Insert2Call(theRecvCallHistory);
	}
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;
	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = m_enumCallType== CALL_PHONE? ENUM_MSG_AUDIO: ENUM_MSG_VIDEO;
	theLocalChatRecord.m_strTalkerURL = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_UNREAD;
	CSystemEx::GetInstance()->GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	//m_pContactDBInfo->m_unionContactInfo.m_pContact->m_nOffLineUnReadMsgNum = 0;
	CSystemEx::GetInstance()->GetMainCanvasHandler()->InsertRecentContact(m_pContactDBInfo);
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactDBInfo);
	CSystemEx::GetInstance()->GetCallerBarWndMGR().CloseCallerBarWnd(this);
	theLocalChatRecord.m_nMsgID = nRecordPrimaryKeyID;
	CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum, theLocalChatRecord, INSERT_FORWARD);
	CSystemEx::GetInstance()->GetMainDlg()->RefreshLocalContactUnReadNum(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum, theLocalChatRecord);
	DestroyWindow(m_hWnd);
	
}
LRESULT CCallerBarWnd::OnBtnVideo(RoutedEventArgs *pe)
{
	if (m_pContactDBInfo->m_unionContactInfo.m_pContact == NULL)
	{
		LOG4_INFO("somthing error in caller wnd");
		return 0;
	}

	bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	HWND hMainDlg = CSystemEx::GetInstance()->GetMainDlg()->m_hWnd;
	if (bMinSize)
	{
		SetForegroundWindow(hMainDlg);
		::SendMessage(hMainDlg,WM_SYSCOMMAND, SC_RESTORE,0);
	}
	else
	{
		if (bVisible == FALSE)
		{
			::ShowWindow(hMainDlg, SW_SHOW);
		}
	}

	
	HWND hVideoWnd = NULL;
	SwitchToContact(m_pContactDBInfo, CT_VIDEO);
	Sleep(1);
	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_IN);
	int nIndex = CSystemEx::GetInstance()->FindSessionBySessionID(m_nSessionID);

	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetActiveLine(nIndex);
	CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactDBInfo);
	CSystemEx::GetInstance()->SaveSession2VideoCanvasHandler(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_nActiveLine, pAudioPageCanvasHandler);
	hVideoWnd = pAudioPageCanvasHandler->GetPlayHwnd();

	//PostMessage(WM_VIDEO_SHOW_OK, nIndex,(LPARAM) hVideoWnd);
	CSystemEx::GetInstance()->Answer(true, nIndex, hVideoWnd);


	HWND hLocalVideoWnd = pAudioPageCanvasHandler->GetLocalHwnd();
	if (hLocalVideoWnd!=NULL)
	{
		if (::IsWindow(hLocalVideoWnd))
		{
			std::string strErrorInfo;
			bool bSelfVideo = true;
			if (m_bIsConfrence)
			{
				bSelfVideo = CSystemEx::GetInstance()->GetGlobalOptions()->m_bLocalVideoInRoom;
			}
			if (bSelfVideo)
			{
				CSystemEx::GetInstance()->SetLocalVideoHwnd(hLocalVideoWnd, false, strErrorInfo);
			}		
		}
	}
	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_IN);
	m_pContactDBInfo->m_unionContactInfo.m_pContact->SetActiveLine(nIndex);
	pAudioPageCanvasHandler->SetConnected(true, nIndex);
	
	bool bEnableAudioInputDev = CSystemEx::GetInstance()->GetAudioInEnable();
	bool bEnableAudioOutputDev = CSystemEx::GetInstance()->GetAudioOutEnable();
	bool bEnableVideoDev = CSystemEx::GetInstance()->GetCameraEnable();
	if (bEnableVideoDev==true)
	{
		CSystemEx::GetInstance()->SendVideo(nIndex);
	}
	CSystemEx::GetInstance()->MuteSession(!bEnableAudioOutputDev, !bEnableAudioInputDev, !bEnableVideoDev, m_nSessionID);
	CCallHistory theRecvCallHistory;
	theRecvCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_SUCC;
	theRecvCallHistory.m_enumCallType = ENUM_CALL_IN;
	theRecvCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theRecvCallHistory.m_strSIPNum = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum;
	theRecvCallHistory.m_enumContentType = CALL_PHONE;
	theRecvCallHistory.m_bRead = true;
	int nRecordPrimaryKeyID = -1;
	CSystemEx::GetInstance()->GetDBOprate()->saveCallMsg(theRecvCallHistory, nRecordPrimaryKeyID);
	if (pAudioPageCanvasHandler != NULL)
	{
		m_pContactDBInfo->m_unionContactInfo.m_pContact->SetHistoryID(nRecordPrimaryKeyID);
	}
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;
	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = ENUM_MSG_AUDIO;
	theLocalChatRecord.m_strTalkerURL = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_READ;
	CSystemEx::GetInstance()->GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	CSystemEx::GetInstance()->GetCallerBarWndMGR().CloseCallerBarWnd(this);

	DestroyWindow(m_hWnd);
	pe->bHandled = true;
	return 0;
}

LRESULT CCallerBarWnd::OnBtnHangup(RoutedEventArgs *pe)
{
	//LONG lTestSessionID = m_pCallBackParameters->getSessionId();
	int nIndex  = (int)CSystemEx::GetInstance()->FindSessionBySessionID(m_nSessionID);
	CSystemEx::GetInstance()->Hangup(nIndex);
	CSystemEx::GetInstance()->GetCallerBarWndMGR().CloseCallerBarWnd(this);
	CCallHistory theRecvCallHistory;
	theRecvCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	theRecvCallHistory.m_enumCallType = ENUM_CALL_IN;
	theRecvCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theRecvCallHistory.m_strSIPNum = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum;
	theRecvCallHistory.m_enumContentType = m_enumCallType;
	theRecvCallHistory.m_bRead = true;
	int nRecordPrimaryKeyID = -1;
	CSystemEx::GetInstance()->GetDBOprate()->saveCallMsg(theRecvCallHistory, nRecordPrimaryKeyID);
	CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(nRecordPrimaryKeyID, theRecvCallHistory);

	pe->bHandled = true;
	//REFRESH INBOX 
	CSystemEx::GetInstance()->GetInboCanvasHandler()->Insert2Call(theRecvCallHistory);
	CSystemEx::GetInstance()->GetInboCanvasHandler()->Insert2Miss(theRecvCallHistory);
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;

	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = m_enumCallType == CALL_PHONE ? ENUM_MSG_AUDIO : ENUM_MSG_VIDEO;
	theLocalChatRecord.m_strTalkerURL = m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_READ;
	CSystemEx::GetInstance()->GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	theLocalChatRecord.m_nMsgID = nRecordPrimaryKeyID;
	CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum, theLocalChatRecord, INSERT_FORWARD);
	CSystemEx::GetInstance()->GetMainDlg()->RefreshLocalContactUnReadNum(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strSIPNum, theLocalChatRecord);
	
	DestroyWindow(m_hWnd);
	return 0;
}

LRESULT CCallerBarWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnAudio->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnAudio(pe);
	}
	else if (m_pBtnVideo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnVideo(pe);
	}
	else if (m_pBtnHangup->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnHangup(pe);
	}

	return 0;
}

void CCallerBarWnd::SetParameters(ICallbackParameters * pCallParameters,int nActiveLine, bool bIsConference)
{
	m_pCallBackParameters = pCallParameters;
	InitProcessData();
	m_nActiveLine = nActiveLine;
	m_bIsConfrence = bIsConference;
}

void CCallerBarWnd::InitProcessData()
{
	const char* sender = m_pCallBackParameters->getCaller();
	// Get display name
	const char* displayName = m_pCallBackParameters->getCallerDisplayName();
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormat(sender);

	int nActiveLine = m_pCallBackParameters->getSessionId();
	m_oProcessComing.m_enumCallType = ENUM_CALL_OUT;
	m_oProcessComing.m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	m_oProcessComing.m_strSIPNum = strSipNumber;
	m_oProcessComing.m_enumContentType = CALL_PHONE;
}

LRESULT CCallerBarWnd::OnAfterPaint(RoutedEventArgs *pe)
{
	if (m_pStaFace->GetSafeHwnd() == pe->hOriginalSender)
	{
		CIUIRect rc;
		IUI::GetClientRect(pe->hOriginalSender, rc);

		DrawFace((HDC)pe->lParam, rc, PortUtility::Utf82Unicode_ND(m_pContactDBInfo->m_unionContactInfo.m_pContact->m_strHeadImage).c_str());
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pContactDBInfo->m_unionContactInfo.m_pContact);

		if (m_pContactDBInfo->m_unionContactInfo.m_pContact->GetSetUserFaceImage() == false)
		{
			HIUIFONT hDefFont = GetProject()->GetFontMgr()->GetDefaultFont();
			_ASSERT(hDefFont != NULL);

			CIUIString strLeft = strSrcUserName.Left(1);
			DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
				CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
		}
	}
	return 0;
}
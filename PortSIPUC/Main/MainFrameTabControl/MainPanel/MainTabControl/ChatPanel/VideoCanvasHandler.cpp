#include "stdafx.h"
#include "VideoCanvasHandler.h"
#include "CallingPageDialog/CallingPageTransfer/CallingTransferDlg.h"
#include "System/SystemEx.h"
#include "MiniKeyboardWnd.h"
#include "../../MainCanvasHandler.h"



std::map<int, CVideoCanvasHandler*> CVideoCanvasHandler::m_mapHwnd2VideoPage;
/*LRESULT CALLBACK FullScreenVideoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CHAR:
		if (VK_ESCAPE == wParam)
		{
			HWLWND hwlVideoHost = (HWLWND)::GetProp(hWnd, GETPROP_VIDEO_HOST);
			SendMessage(hwlVideoHost, WM_TOGGLE_VIDEO, 0, 0);
		}
		break;

	case WM_LBUTTONDBLCLK:
		if (::IsWindow(hWnd))
		{
			HWLWND hwlVideoHost = (HWLWND)::GetProp(hWnd, GETPROP_VIDEO_HOST);
			SendMessage(hwlVideoHost, WM_TOGGLE_VIDEO, 0, 0);
		}
		else
		{
			_ASSERT(FALSE);
		}
		break;

	case WM_DESTROY:
		::SendMessage(g_pMainFrame->GetSafeHwnd(), WM_EXIT_FULL_SCREEN_VIDEO, 0, 0);
		break;
	}

	break;


	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
*/LRESULT CALLBACK FullScreenVideoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CHAR:
		if (VK_ESCAPE == wParam)
		{
			HWLWND hwlVideoHost = (HWLWND)::GetProp(hWnd, GETPROP_VIDEO_HOST);
			SendMessage(hwlVideoHost, WM_TOGGLE_VIDEO, 0, 0);
		}
		break;

	case WM_LBUTTONDBLCLK:
		if (::IsWindow(hWnd))
		{
			HWLWND hwlVideoHost = (HWLWND)::GetProp(hWnd, GETPROP_VIDEO_HOST);
			SendMessage(hwlVideoHost, WM_TOGGLE_VIDEO, 0, 0);
		}
		else
		{
			_ASSERT(FALSE);
		}
		break;

	case WM_DESTROY:
		::SendMessage(g_pMainFrame->GetSafeHwnd(), WM_EXIT_FULL_SCREEN_VIDEO, 0, 0);
		break;
	case WM_WINDOWPOSCHANGED:
	{
		HWND hMediaToolbar = (HWND)GetProp(hWnd, GETPROP_MEDIA_TOOLBAR);
		CIUIRect rcVideo;
		::GetWindowRect(hWnd, rcVideo);

		if (::IsWindow(hMediaToolbar))
		{
			CIUIRect rcToolBar;
			::GetWindowRect(hMediaToolbar, rcToolBar);

			CRect rcNew;
			CanvasLayout(rcToolBar, CLH_CENTER, CLV_BOTTOM, NULL, NULL,
				rcVideo.Width(), rcVideo.Height(), rcNew);

			IUIClientToScreen(hWnd, rcNew);

			::MoveWindow(hMediaToolbar, rcNew.left, rcNew.top,
				rcNew.Width(), rcNew.Height(), TRUE);

			ShowWindow(hMediaToolbar, IsWindowVisible(hWnd) ? SW_SHOW : SW_HIDE);
		}
		HWND hMySelfVideo = (HWND)GetProp(hWnd, GETPROP_MY_VIDEO);
		if (::IsWindow(hMySelfVideo))
		{
			CRect rcMySlefVideo;
			::GetWindowRect(hMySelfVideo, rcMySlefVideo);

			CRect rcNew;
			CanvasLayout(rcMySlefVideo, CLH_RIGHT, CLV_TOP, NULL, NULL,
				rcMySlefVideo.Width(), rcMySlefVideo.Height(), rcNew);

			//::MoveWindow(hMySelfVideo, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), FALSE);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hDC = ::BeginPaint(hWnd, &ps);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		HDC hMemDC = ::CreateCompatibleDC(hDC);

		HBITMAP hMemBmp = ::CreateCompatibleBitmap(hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

		HBITMAP hbmpMemOld = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

		TCHAR szName[MAX_PATH] = { 0 };
		GetWindowText(hWnd, szName, MAX_PATH);
		if (StrCmp(szName, _T("SmallVideo")) == 0)
		{
			// Draw content to dcMem...
			//IUI::FillSolidRect(hMemDC, &rcClient, RGB(255, 0, 0), FALSE);
		}
		else if (StrCmp(szName, _T("BigVideo")) == 0)
		{
			// Draw content to dcMem...
			//IUI::FillSolidRect(hMemDC, &rcClient, RGB(255, 0, 255), FALSE);
		}

		::BitBlt(hDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hMemDC, 0, 0, SRCCOPY);

		::SelectObject(hMemDC, hbmpMemOld);

		::EndPaint(hWnd, &ps);

	}
	break;

	case WM_LBUTTONDOWN:
	{
		TCHAR szName[MAX_PATH] = { 0 };
		GetWindowText(hWnd, szName, MAX_PATH);
		if (StrCmp(szName, _T("SmallVideo")) == 0)
		{
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		}
	}
	break;

	default:
		break;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL RegisterVideoWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = FullScreenVideoWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = VIDEO_WND_CLASS;
	ATOM ret = ::RegisterClass(&wc);
	_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND CreateVideoWnd(HWND hParent)
{
	BOOL bRet = RegisterVideoWindowClass();
	if (!bRet)
	{
		return NULL;
	}

	HWND hWnd = ::CreateWindowEx(0, VIDEO_WND_CLASS,
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0,
		hParent, 0, IUIGetInstanceHandle(), 0);

	return hWnd;
}


//////////////////////////////////////////////////////////////////////////

CVideoCanvasHandler::CVideoCanvasHandler(TAG_RECENT* pContactInfo)
	:
	/*
	m_pCanvasMediaToolbar(NULL)
	, m_pBtnRec(NULL)
	, m_pBtnMute(NULL)
	, m_pBtnPause(NULL)
	, m_pBtnTransfer(NULL)
	, m_pBtnAdd(NULL)
	, m_pBtnKeyboard(NULL)
	, m_pBtnShare(NULL)
	, m_pBtnMsg(NULL)
	, m_pBtnNoVideo(NULL)
	, m_pBtnHangup(NULL)

	,*/
	m_pContactInfo(NULL)
	,m_hVideoHost(NULL)
	, m_hVideoWnd(NULL)
	, m_hSmallVideoWnd(NULL)
	, m_bOldVideoVisibleState(TRUE)
{
	m_pWndVideoToolbar = new CMediaToolbarWnd(CT_VIDEO, pContactInfo->m_enumRecentContact);
	m_pCallingTransferDlg = NULL;
	m_pAddCallerDlg = NULL;
	m_pSrcContact = NULL;
	m_pAddContact = NULL;
	m_hDecodeTimer = 0;
	m_bSaveNormalPT = false;
	m_bIsConference = false;
	m_pwndVideoHost = NULL;
	m_hVideoHost = NULL;
	m_hVideoWnd = NULL;
	m_pDTMFKeyboardWnd = NULL;
}

void CVideoCanvasHandler::SetIsConference(bool bIsConference)
{
	m_bIsConference = bIsConference;
	
	bool bShow = ShowLocalVideoInConference();
	std::string strErrofInfo;
	if (bShow==false)
	{
		
		CSystemEx::GetInstance()->StopLocalVideoHwnd(m_hSmallVideoWnd, false, strErrofInfo);
	}
	else
	{
		CSystemEx::GetInstance()->SetLocalVideoHwnd(m_hSmallVideoWnd, false, strErrofInfo);
	}
}
CVideoCanvasHandler::~CVideoCanvasHandler()
{
}
void CVideoCanvasHandler::BeginTimer()
{
	if (m_hDecodeTimer != 0)
	{
		::KillTimer(NULL, m_hDecodeTimer);
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
	m_hDecodeTimer = ::SetTimer(NULL, IDT_SESSION_TIMER, 1000, TimerProcSession);
	m_mapHwnd2VideoPage.insert(make_pair(m_hDecodeTimer, this));
}
void CVideoCanvasHandler::CleanChkStatus()
{
	bool bChechEnable = m_pWndVideoToolbar->GetRecEnable();
	if (m_pWndVideoToolbar->GetRecCheck() == BST_CHECKED )
	{
		m_pWndVideoToolbar->SetRecCheck(BST_UNCHECKED);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			CSystemEx::GetInstance()->EndRecord(pContactDB->m_nActiveLine, pContactDB->m_strSIPNum, ENUM_VA_RECORD_VIDEO);
		}
	}
	//MUTE
	if (m_pWndVideoToolbar->GetMuteCheck() == BST_CHECKED)
	{
		m_pWndVideoToolbar->SetMuteCheck(BST_UNCHECKED);
		MuteMicroPhineSession();
	}
	//Hold
	if (m_pWndVideoToolbar->GetVideoCheck() == BST_CHECKED)
	{
		m_pWndVideoToolbar->SetVideoCheck(BST_UNCHECKED);
	}
	//NOVIDEO
	if (m_pWndVideoToolbar->GetHoldCheck() == BST_CHECKED)
	{
		m_pWndVideoToolbar->SetHoldCheck(BST_UNCHECKED);
	}
}
void  CVideoCanvasHandler::TimerProcSession(HWND hlWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CVideoCanvasHandler* pVideoCanvasHandler = NULL;
	auto iterFind = m_mapHwnd2VideoPage.find(idEvent);
	if (iterFind != m_mapHwnd2VideoPage.end())
	{
		pVideoCanvasHandler = iterFind->second;
	}
	if (pVideoCanvasHandler != NULL)
	{
		if (pVideoCanvasHandler->m_hDecodeTimer == idEvent)
		{
			OutputDebugString(L"");
			pVideoCanvasHandler->ProcessTimer();
		}
		else
		{

		}

		return;
	}
}
void CVideoCanvasHandler::ProcessTimer()
{
	int  sendBytes, sendPackets, sendPacketsLost, sendFractionLost, sendRttMS, sendCodecType, sendFrameWidth, sendFrameHeight, sendBitrateBPS, sendFramerate;
	//Receiver
	int  recvBytes, recvPackets,recvPacketsLost,recvFractionLost,recvCodecType,recvFrameWidth, recvFrameHeight, recvBitrateBPS,recvFramerate;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	switch (localRecentContactType)
	{
	case ENUM_SINGLE_CONTACT:
	{
		int  nSessionID = CSystemEx::GetInstance()->GetSessionID(m_pContactInfo->m_unionContactInfo.m_pContact->ReturnActiveLine());
		int nErrorCode =PortSIP_getVideoStatistics(CSystemEx::GetInstance()->GetSipLib(), nSessionID,\
			&sendBytes,
			&sendPackets,
			&sendPacketsLost,
			&sendFractionLost,
			&sendRttMS,
			&sendCodecType,
			&sendFrameWidth,
			&sendFrameHeight,
			&sendBitrateBPS,
			&sendFramerate,
			&recvBytes,
			&recvPackets,
			&recvPacketsLost,
			&recvFractionLost,
			&recvCodecType,
			&recvFrameWidth,
			&recvFrameHeight,
			&recvBitrateBPS,
			&recvFramerate);
		std::string strInfo;
		CIUIString strIUIInfo;
		if (nErrorCode==0)
		{
			strIUIInfo.Format(_T("[SendBytes:%d] [sendPackets:%d][sendPacketsLost:%d] [sendFractionLost:%d] [sendRttMS:%d] [sendCodecType:%d]  [sendFrameWidth:%d] [sendFrameHeight:%d]\
			 [sendBitrateBPS:%d] [sendFramerate:%d] [recvBytes:%d] [recvPackets:%d] [recvPacketsLost:%d] [recvFractionLost:%d] [recvCodecType:%d] [recvFrameWidth:%d] [recvFrameHeight:%d]\
			[recvBitrateBPS:%d] [recvFramerate:%d]"),sendBytes,sendPackets,sendPacketsLost,sendFractionLost,sendRttMS,sendCodecType,sendFrameWidth,sendFrameHeight,sendBitrateBPS,sendFramerate,\
			recvBytes, recvPackets, recvPacketsLost, recvFractionLost, recvCodecType, recvFrameWidth, recvFrameHeight, recvBitrateBPS, recvFramerate);

			strInfo = PortUtility::Unicode2Utf8_ND(strIUIInfo.GetBuffer(strIUIInfo.GetLength()));
			//LOG4_DEBUG(strInfo.c_str());
			//OutputDebugString(strIUIInfo);
			//OutputDebugString(L"\n");
		}
	}
	break;
	default:
		break;
	}
}
int CVideoCanvasHandler::InitControls()
{
	/*
	m_pCanvasMediaToolbar = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_MEDIA_TOOLBAR")));
	_ASSERT(m_pCanvasMediaToolbar != NULL);
	

	m_pBtnRec = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_REC")));
	_ASSERT(m_pBtnRec != NULL);
	m_pBtnMute = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MUTE")));
	_ASSERT(m_pBtnMute != NULL);
	m_pBtnPause = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PAUSE")));
	_ASSERT(m_pBtnPause != NULL);
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
	m_pBtnNoVideo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NO_VIDEO")));
	_ASSERT(m_pBtnNoVideo != NULL);
	m_pBtnHangup = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_HANGUP")));
	_ASSERT(m_pBtnHangup != NULL);
	*/
	return 0;
}

int CVideoCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
#if _DEBUG
	TIMER_START(OnInitPanel);
#endif
	InitControls();

	if (!::IsWindow(m_hVideoHost))
	{
		m_hVideoHost = CreateWaitVideoHost(g_pMainFrame->GetSafeHwnd());
		SetWindowText(m_hVideoHost, _T("VideoHost"));
		::SetProp(m_hVideoHost, GETPROP_VIDEO_HOST, GetBindWLWnd());
		m_bOldVideoVisibleState = TRUE;

		//::InvalidateRect(m_hVideoHost, NULL, TRUE);
		//::UpdateWindow(m_hVideoHost);
		CRect rcVideoHost;
		::GetWindowRect(m_hVideoHost, rcVideoHost);
		BOOL bRet = RegisterVideoWindowClass();
		if (!bRet)
		{
			return NULL;
		}
		int nLocalWidth1 = rcVideoHost.Width();
		int nLocalHeight1 = rcVideoHost.Height();
		m_hVideoWnd = ::CreateWindowEx(0, VIDEO_WND_CLASS,
			_T("BigVideo"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, rcVideoHost.Width(), rcVideoHost.Height(),
			m_hVideoHost, 0, IUIGetInstanceHandle(), 0);
		
		::SetProp(m_hVideoWnd, GETPROP_VIDEO_HOST, GetBindWLWnd());

		m_stuLocalMyVideo = CSystemEx::GetInstance()->GetLocalMetrics(); 
		int nLocalWidth = MAX_LOCAL_VIDEO_WIDTH_NORMAL;
		int nLocalHeight = MAX_LOCAL_VIDEO_HEIGHT_NORMAL;

		m_hSmallVideoWnd = ::CreateWindowEx(0, VIDEO_WND_CLASS,
			_T("SmallVideo"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, nLocalWidth, nLocalHeight,
			m_hVideoHost, 0, IUIGetInstanceHandle(), 0);

		::SetProp(m_hVideoHost, GETPROP_MY_VIDEO, m_hSmallVideoWnd);

		ShowWaitGif();
		m_pWndVideoToolbar->SetProject(GetProject(GetBindWLWnd()));
		m_pWndVideoToolbar->CreatePopupWindow(_T("IDW_MEDIA_TOOLBAR.xml"), m_hVideoHost);
		m_pWndVideoToolbar->SetBindVideoPanel(GetBindWLWnd());

		::SetProp(m_hVideoHost, GETPROP_MEDIA_TOOLBAR, m_pWndVideoToolbar->GetSafeHwnd());
		m_pWndVideoToolbar->Invalidate();
	}
	_ASSERT(m_pContactInfo != NULL);
	if (m_bSaveNormalPT == false)
	{
		CRect rectScreen;
		GetClientRect(m_hVideoHost, &rectScreen);
		m_ptNormal.x = rectScreen.right - rectScreen.left;
		m_ptNormal.y = rectScreen.bottom - rectScreen.top;
		m_bSaveNormalPT = true;
	}
#if _DEBUG
	TIMER_STOP(OnInitPanel);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" CVideoCanvasHandler OnInitPanel Cost %d ms.\n", TIMER_MSEC(OnInitPanel));
	OutputDebugString(strFormatInfo);
#endif
	
	return 0;
}
void CVideoCanvasHandler::OnProcessConfenceAction(ENUM_RECENT_CONTACT_TYPE  SrcContactType)
{
	switch (  SrcContactType)
	{
	case ENUM_SINGLE_CONTACT:
	{
		if (m_pSrcContact == NULL || m_pAddContact == NULL)
		{
			return;
		}
		CVideoCanvasHandler *pSrcVideoPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pSrcContact);
		CVideoCanvasHandler * pAddVideoPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pAddContact);

		CSystemEx::GetInstance()->CreateConfence(ENUM_CONFENCE_VIDEO, GetPlayHwnd());

		int nHistroyIDSrc, nHistroyIDAdd, nActiveLineSrc, nActiveLineAdd;
		CTalkGroup* pGroupTalk = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		if (pGroupTalk)
		{
			nHistroyIDSrc = m_pSrcContact->m_unionContactInfo.m_pContact->GetHistoryID();
			nActiveLineSrc = m_pSrcContact->m_unionContactInfo.m_pContact->ReturnActiveLine();
			pGroupTalk->CopySrcData2GroupNumber(nActiveLineSrc, nHistroyIDSrc, (void*)m_pSrcContact);
			if (pSrcVideoPageCanvasHandler != NULL)
			{
				pSrcVideoPageCanvasHandler->CloseWnd();
			}
			CSystemEx::GetInstance()->SaveSession2VideoCanvasHandler(nActiveLineSrc, this);
		

			CSystemEx::GetInstance()->joinConference(nActiveLineSrc);

			nHistroyIDAdd = m_pAddContact->m_unionContactInfo.m_pContact->GetHistoryID();
			nActiveLineAdd = m_pAddContact->m_unionContactInfo.m_pContact->ReturnActiveLine();
			pGroupTalk->CopySrcData2GroupNumber(nActiveLineAdd, nHistroyIDAdd, (void*)m_pAddContact);
	
			if (pAddVideoPageCanvasHandler != NULL)
			{
				pAddVideoPageCanvasHandler->CloseWnd();
			}
			CSystemEx::GetInstance()->SaveSession2VideoCanvasHandler(nActiveLineAdd, this);

			CSystemEx::GetInstance()->joinConference(nActiveLineAdd);

		}
		
		CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactInfo);
	

		SwitchToContact(m_pContactInfo, CT_VIDEO);
		SetConnected(true, nActiveLineSrc);
		SetConnected(true, nActiveLineAdd);
	}
	break;
	case ENUM_MULTI_CONTACT:
	{
		CTalkGroup *pGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		if (pGroup->GetCallType() == ENUM_CALL_OUT)
		{
			CallTheContact();
		}
	}
	break;
	default:
		break;
	}
}
int CVideoCanvasHandler::OnWindowPosChanged(RoutedEventArgs *pe)
{
	if (::IsWindow(m_hVideoHost))
	{
		if (!::IsZoomed(m_hVideoHost))
		{
			LayoutVideoWnd(pe->wParam, pe->lParam);
		}
	}

	return 0;
}
bool CVideoCanvasHandler::CallTheContact()
{
#if _DEBUG
	TIMER_START(CallTheContact);
#endif

	if (!::IsWindow(m_hVideoWnd))
	{
		m_hVideoWnd = CreateVideoWnd(/*GetHostWnd(GetBindWLWnd())*/CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	}

	ShowWaitGif();
	::PostMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_CALL_ACTION_SINGLE, (WPARAM)m_pContactInfo, (LPARAM)this);
	
#if _DEBUG
	TIMER_STOP(CallTheContact);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" CallTheContact Cost %d ms.\n", TIMER_MSEC(CallTheContact));
	OutputDebugString(strFormatInfo);
#endif
	return true;
}
bool    CVideoCanvasHandler::OnCallTheContact()
{
	bool bRet = false;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		std::string strSipNumber = pContactDB->m_strSIPNum;
		std::string strErrorInfo = "";

		bRet = CSystemEx::GetInstance()->CallOneNumberVideo(strSipNumber, true, pContactDB->m_nActiveLine, pContactDB->m_nCallHistoryID, strErrorInfo, m_hVideoWnd);
		if (pContactDB->m_nActiveLine < 0)
		{
			LOG4_ERROR("here cause the toolbar un enable ,CallOneNumberAudio CAudioPageCanvasHandler 2");
		}
		if (bRet == false)
		{
			std::wstring strTip = PortUtility::string2WString(strErrorInfo);
			std::wstring strTemplate = _T("call error:") + strTip;
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"), strTemplate.c_str(), GetHostWnd(GetBindWLWnd()));
			SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
			return false;
		}
		CSystemEx::GetInstance()->SetLocalVideoHwnd(m_hSmallVideoWnd, false, strErrorInfo);
		
		pContactDB->SetUserData(CT_VIDEO);
		CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactInfo);
		CSystemEx::GetInstance()->SaveSession2VideoCanvasHandler(pContactDB->m_nActiveLine, pAudioPageCanvasHandler);
		pContactDB->m_bIsConnected = false;
	}

	return bRet;
}
bool    CVideoCanvasHandler::OnCallTheGroup()
{
	if (!::IsWindow(m_hVideoWnd))
	{
		m_hVideoWnd = CreateVideoWnd(/*GetHostWnd(GetBindWLWnd())*/CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	}

	CSystemEx::GetInstance()->CreateConfence(ENUM_CONFENCE_VIDEO, m_hVideoWnd);

	CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
	auto vecTalkGroup = pTalkGroup->GetContact();
	auto iterTalkMember = vecTalkGroup.begin();
	bool bTotalFailed = false;
	//int  nLastLine = -1;
	for (; iterTalkMember != vecTalkGroup.end(); iterTalkMember++)
	{
		CTalkGroupNumber *pGroupGroupNumber = *iterTalkMember;
		string strSipNumber = pGroupGroupNumber->m_strSIPNumber;
		string strErrorInfo = "";
		bool bRet = CSystemEx::GetInstance()->CallOneNumberVideo(strSipNumber, true, pGroupGroupNumber->m_nActiveLine, pGroupGroupNumber->m_nCallHistoryID, strErrorInfo, m_hVideoWnd, NULL, true);

		if (pGroupGroupNumber->m_nActiveLine < 0)
		{
			LOG4_ERROR("here cause the toolbar un enable ,CallOneNumberAudio CAudioPageCanvasHandler 3");
		}
		if (bRet == false)
		{
			std::wstring strTip = PortUtility::string2WString(strErrorInfo);
			std::wstring strTemplate = _T("call error:") + strTip;
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"), strTemplate.c_str(), GetHostWnd(GetBindWLWnd()));
			continue;
		}
		else
		{
			bool bSelfVideo = ShowLocalVideoInConference();
			if (bSelfVideo)
			{
				CSystemEx::GetInstance()->SetLocalVideoHwnd(m_hSmallVideoWnd, false, strErrorInfo);
			}
			else
			{
				CSystemEx::GetInstance()->StopLocalVideoHwnd(m_hSmallVideoWnd, false, strErrorInfo);
			}
			bTotalFailed = true;
			CSystemEx::GetInstance()->EraseSession2CanvasHandler(pGroupGroupNumber->m_nActiveLine);
			CSystemEx::GetInstance()->SaveSession2VideoCanvasHandler(pGroupGroupNumber->m_nActiveLine, this);
		}
	}
	if (bTotalFailed == false)
	{
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	}
	else
	{
		pTalkGroup->SetUserData(CT_VIDEO);

	}
	return bTotalFailed;
}
bool CVideoCanvasHandler::OnCallAction(TAG_RECENT *pContactInfo)
{
	if (pContactInfo!=m_pContactInfo)
	{
		LOG4_ERROR("pContactInfo ERROR");
		return false;
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		return OnCallTheContact();
	}
	else
	{
		return OnCallTheGroup();
	}
}

LRESULT CVideoCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);
	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			if (GetName(pe->hOriginalSender) == _T("IDC_CHK_REC"))
			{
				OnChkRec(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_CHK_MUTE"))
			{
				OnChkMute(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_CHK_PAUSE"))
			{
				OnChkPause(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_TRANSFER"))
			{
				OnBtnTransfer(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_ADD"))
			{
				OnBtnAdd(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_KEYBOARD"))
			{
				OnBtnKeyboard(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_SHARE"))
			{
				OnBtnShare(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_MSG"))
			{
				OnBtnMsg(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_CHK_NO_VIDEO"))
			{
				OnBtnNoVideo(pe);
			}
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_HANGUP"))
			{
				OnBtnHangup(pe);
			}
		}
	}

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
			else if (WM_WINDOWPOSCHANGED == pe->RoutedEvent)
			{
				return OnWindowPosChanged(pe);
			}
		}
		else if (RS_DIRECT == pe->eRoutingStrategy)
		{
			if (WM_TOGGLE_VIDEO == pe->RoutedEvent)
			{
				return ToggleVideoWnd();
			}
			else if (UE_PROPERTYSHEET_SHOW == pe->RoutedEvent)
			{
				SetProp(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), GETPROP_VEDIO_HWND, GetBindWLWnd());
				ShowWindow(m_hVideoHost, SW_SHOW);
				m_bOldVideoVisibleState = TRUE;
			}
			else if (UE_PROPERTYSHEET_HIDE == pe->RoutedEvent)
			{
				::ShowWindow(m_hVideoHost, SW_HIDE);
				m_bOldVideoVisibleState = FALSE;
			}
			else if (WM_HOST_WINDOWPOSCHANGED == pe->RoutedEvent)
			{
				LayoutVideoWnd(pe->wParam, pe->lParam);
			}
			else if (WLM_PARENTSHOWWINDOW == pe->RoutedEvent)
			{
				BOOL bShow = pe->lParam;

				if (bShow)
				{
					::ShowWindow(m_hVideoHost, m_bOldVideoVisibleState ? SW_SHOW : SW_HIDE);
				}
				else
				{
					::ShowWindow(m_hVideoHost, SW_HIDE);
				}
			}
			else if (WLM_PARENTENABLE == pe->RoutedEvent)
			{
				BOOL bEnable = pe->wParam;
				if (bEnable)
				{
					SetParent(m_hVideoHost, NULL);
					LONG lStyle = GetWindowLong(m_hVideoHost, GWL_STYLE);
					lStyle &= ~WS_CHILD;
					lStyle |= WS_POPUP;
					SetWindowLong(m_hVideoHost, GWL_STYLE, lStyle);

					HWND hVideoHostParent = GetParent(m_hVideoHost);

					LayoutVideoWnd(0, 0);
				}
				else
				{
					LONG lStyle = GetWindowLong(m_hVideoHost, GWL_STYLE);
					lStyle &= ~WS_POPUP;
					lStyle |= WS_CHILD;
					SetWindowLong(m_hVideoHost, GWL_STYLE, lStyle);

					SetParent(m_hVideoHost, GetHostWnd(GetBindWLWnd()));
					CIUIRect rcVideo;
					GetWindowRect(GetBindWLWnd(), rcVideo);
					IUIScreenToClient(GetHostWnd(GetBindWLWnd()), rcVideo);
					SetWindowPos(m_hVideoHost, NULL, rcVideo.left, rcVideo.top, 0, 0, SWP_NOSIZE);
				}
				::EnableWindow(m_pWndVideoToolbar->GetSafeHwnd(), bEnable);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
	}

	return 0;
}
void CVideoCanvasHandler::UpdateVideoHwndInfo()
{
	SetProp(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), GETPROP_VEDIO_HWND, GetBindWLWnd());
}

LRESULT CVideoCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CVideoCanvasHandler::OnChkRec(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		std::string strFilePath = CSystemEx::GetInstance()->GetGlobalOptions()->m_strRecordFolder;
		if (strFilePath.empty())
		{
			m_pWndVideoToolbar->SetRecCheck(BST_UNCHECKED);
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("ERROR"),CSystemEx::GetInstance()->GetTipsNoRecordPathText(), GetHostWnd(GetBindWLWnd()),3000);
			return 0;
		}
		SYSTEMTIME st;
		CIUIString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(L"%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		std::string strFileName = PortUtility::wstring2String(strDate.GetBuffer(strDate.GetLength()));
		if (m_pWndVideoToolbar->GetRecCheck() == BST_CHECKED)
		{
			BST_CHECKED;
			bool bRet = CSystemEx::GetInstance()->StartRecord(pContactDB->m_nActiveLine, strFilePath, strFileName);
			if (bRet == false)
			{
				m_pWndVideoToolbar->SetRecCheck(BST_CHECKED);
			}
		}
		else
		{
			m_pWndVideoToolbar->SetRecCheck(BST_UNCHECKED);
			CSystemEx::GetInstance()->EndRecord(pContactDB->m_nActiveLine, pContactDB->m_strSIPNum, ENUM_VA_RECORD_VIDEO);
		}
	}
	return 0;
}
void CVideoCanvasHandler::MuteMicroPhineSession()
{
	bool bCheck = m_pWndVideoToolbar->GetMuteCheck() == BST_UNCHECKED ? true : false;
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
	return;
}
LRESULT CVideoCanvasHandler::OnChkMute(RoutedEventArgs *pe)
{
	MuteMicroPhineSession();
	return 0;
}

LRESULT CVideoCanvasHandler::OnChkPause(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	bool bCheck = m_pWndVideoToolbar->GetHoldCheck() == BST_UNCHECKED ? true : false;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (bCheck)
		{
			if (pContactDB)
			{
				CSystemEx::GetInstance()->UnHold(pContactDB->m_nActiveLine);
				m_pWndVideoToolbar->SetHoldBySelf(false);
			}

		}
		else
		{
			if (pContactDB)
			{
				CSystemEx::GetInstance()->Hold(pContactDB->m_nActiveLine);
				m_pWndVideoToolbar->SetHoldBySelf(true);
			}

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
				if (bCheck)
				{
					CSystemEx::GetInstance()->UnHold(nActiveLine);
					m_pWndVideoToolbar->SetHoldBySelf(false);
				}
				else
				{
					CSystemEx::GetInstance()->Hold(nActiveLine);
					m_pWndVideoToolbar->SetHoldBySelf(true);
				}
			}
		}
	}
	return 0;
}

LRESULT CVideoCanvasHandler::OnBtnTransfer(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
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

		m_pCallingTransferDlg = new CCallingTransferDlg(this,CT_VIDEO);
		if (m_hVideoHost != NULL)
		{
			if (IsWindow(m_hVideoHost))
			{
				m_pCallingTransferDlg->SetParentHwnd(m_hVideoHost);
			}
		}
		//CCallingTransferDlg dlg;
		m_pCallingTransferDlg->SetProject(GetProject(pe->hOriginalSender));
		m_pCallingTransferDlg->SetTransferedContact(m_pContactInfo);
		m_pCallingTransferDlg->SetXml(_T("IDD_CALLING_TRANSFER.xml"));
		m_pCallingTransferDlg->DoModal();
		TransferClosed();
	}
	return 0;
}

LRESULT CVideoCanvasHandler::OnBtnAdd(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType==ENUM_MULTI_CONTACT)
	{
		CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		int nSize = pTalkGroup->GetMemberCount();
		if (nSize>= MAX_MERBER_CONFRENCE)
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
		m_pAddCallerDlg = new CAddCallerDlg(this,ENUM_CONFENCE_VIDEO);
		m_pAddCallerDlg->SetCurrentContact(m_pContactInfo);
		m_pAddCallerDlg->SetProject(GetProject(pe->hOriginalSender));
		m_pAddCallerDlg->SetXml(_T("IDD_CALLING_TRANSFER.xml"));
		m_pAddCallerDlg->DoModal();
	}

	return 0;

}

LRESULT CVideoCanvasHandler::OnBtnKeyboard(RoutedEventArgs *pe)
{
	if (m_pContactInfo==NULL)
	{
		LOG4_ERROR("m_pContactInfo==NULL");
		return 0;
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		LOG4_ERROR("ocalRecentContactType == ENUM_SINGLE_CONTACT==NULL");
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB==NULL)
		{
			LOG4_ERROR("pContactDB==NULL");
			return 0;
		}
		if (m_pDTMFKeyboardWnd != NULL)
		{
			if (::IsWindow(m_pDTMFKeyboardWnd->GetSafeHwnd()) == true)
			{
				EndDialog(m_pDTMFKeyboardWnd->GetSafeHwnd(), IDOK);
			}
		};
		LOG4_ERROR("ew CMiniKeyboardW");
		m_pDTMFKeyboardWnd = new CMiniKeyboardWnd();
		m_pDTMFKeyboardWnd->SetProject(GetProject(GetBindWLWnd()));
		m_pDTMFKeyboardWnd->CreatePopupWindow(_T("IDW_MINI_KEYBOARD.xml"), GetHostWnd(GetBindWLWnd()));
		LOG4_ERROR("ew CreatePopupWindow");
		m_pDTMFKeyboardWnd->SetActiveLine(pContactDB->ReturnActiveLine());
		CIUIRect rcButton;
		IUI::GetWindowRect(m_pWndVideoToolbar->m_pBtnKeyboard->GetSafeHwnd(), rcButton);

		CIUIRect rcDlg;
		::GetWindowRect(m_pDTMFKeyboardWnd->m_hWnd, rcDlg);

		rcDlg.OffsetRect(rcButton.left - rcDlg.left - (rcDlg.Width() - rcButton.Width()) / 2,
			rcButton.top - rcDlg.top - rcDlg.Height());
		::MoveWindow(m_pDTMFKeyboardWnd->m_hWnd, rcDlg.left, rcDlg.top, rcDlg.Width(), rcDlg.Height(), FALSE);
		LOG4_ERROR("ew MoveWindow");
		m_pDTMFKeyboardWnd->Invalidate();
		LOG4_ERROR("ew MoveInvalidateWindow");
	}
	return 0;
}

LRESULT CVideoCanvasHandler::OnBtnShare(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->TipsFunNoRun();
	return 0;
	
}

LRESULT CVideoCanvasHandler::OnBtnMsg(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CVideoCanvasHandler::OnBtnNoVideo(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	bool bCheck = m_pWndVideoToolbar->GetVideoCheck() == BST_UNCHECKED ? true : false;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (!bCheck)
		{
			CSystemEx::GetInstance()->StopVideo(pContactDB->m_nActiveLine);
		
		}
		else
		{
			CSystemEx::GetInstance()->SendVideo(pContactDB->m_nActiveLine);
		}
	}
	return 0;
}
void CVideoCanvasHandler::ProcessTransferFailed()
{
	CloseTransferDlg();
}

void CVideoCanvasHandler::ProcessAddContactFailed()
{
	CloseAddCallerDlg();
}
LRESULT CVideoCanvasHandler::OnBtnHangup(RoutedEventArgs *pe)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		DoSingleContactHangup(true);
	}
	else
	{
		DoTalkGroupHangup();
	}
	
	return 0;
}

void CVideoCanvasHandler::CloseWnd()
{
	CleanChkStatus();
	if (m_pWndVideoToolbar->GetSafeHwnd() != NULL)
	{
		m_pWndVideoToolbar->ResetBtnStatus();
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactInfo);
		pContactDB->m_bIsConnected = false;
		pContactDB->m_nCallHistoryID = -1;

		CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
		CSystemEx::GetInstance()->EraseSession2CanvasHandler(pContactDB->m_nActiveLine);
		pContactDB->SetActiveLine(-1);
		
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);

		ShowVideoWindow(FALSE);
	}

}

void CVideoCanvasHandler::AddContact2Group(TAG_RECENT * pConctactInfo)
{
	if (m_pContactInfo->m_enumRecentContact != ENUM_MULTI_CONTACT)
	{
		return;
	}
	if (pConctactInfo->m_enumRecentContact != ENUM_SINGLE_CONTACT)
	{
		return;
	}
	CContactDB *pContact = pConctactInfo->m_unionContactInfo.m_pContact;
}

void    CVideoCanvasHandler::UnHoldByRemote()
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB->ReturnActiveLine() < 0)
		{
			LOG4_INFO("the active line is error where unhold by remote");
			return;
		}
	}
	
}
void CVideoCanvasHandler::ProcessSwtichLineAboutMyLocalVideo()
{
	std::string strErrorInfo = "";
	bool bSelfVideo = true;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	bool bShow = ShowLocalVideoInConference();

	if (m_hSmallVideoWnd==NULL||!IsWindow(m_hSmallVideoWnd)||!IsWindowVisible(m_hSmallVideoWnd))
	{
		return;
	}
	if (bSelfVideo)
	{
		CSystemEx::GetInstance()->SetLocalVideoHwnd(m_hSmallVideoWnd,false, strErrorInfo);
	}
	else
	{
		CSystemEx::GetInstance()->StopLocalVideoHwnd(m_hSmallVideoWnd,false, strErrorInfo);
	}
}
void CVideoCanvasHandler::ProcessSwtichLine(bool bHold)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	vector<int > vecActiveLine;
	if (m_pWndVideoToolbar == NULL)
	{
		return;
	}
	if (m_pWndVideoToolbar->m_pChkPause==NULL)
	{
		return;
	}
	bool bCheck = m_pWndVideoToolbar->GetHoldCheck() == BST_UNCHECKED ? false : true;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		int nActiveLine = m_pContactInfo->m_unionContactInfo.m_pContact->ReturnActiveLine();
		if (nActiveLine > 0)
		{
			vecActiveLine.push_back(nActiveLine);
			if (bHold)
			{
				if (bCheck == false && m_pWndVideoToolbar->GetHoldBySelf() == false)
				{
					m_pWndVideoToolbar->SetHoldCheck(BST_CHECKED);
					CSystemEx::GetInstance()->SwtichLineUpdateHold(vecActiveLine);
				}
			}
			else
			{
				if (bCheck&&m_pWndVideoToolbar->GetHoldBySelf() == false)
				{
					CSystemEx::GetInstance()->SwtichLineUpdateUnHold(vecActiveLine);
					m_pWndVideoToolbar->SetHoldCheck(BST_UNCHECKED);
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
			if (bCheck == false && m_pWndVideoToolbar->GetHoldBySelf() == false)
			{
				m_pWndVideoToolbar->SetHoldCheck(BST_CHECKED);
				CSystemEx::GetInstance()->SwtichLineUpdateHold(vecActiveLine);
			}
		}
		else if (vecActiveLine.size() > 0 && bHold == false)
		{
			if (bCheck&& m_pWndVideoToolbar->GetHoldBySelf() == false)
			{
				CSystemEx::GetInstance()->SwtichLineUpdateUnHold(vecActiveLine);
				m_pWndVideoToolbar->SetHoldCheck(BST_UNCHECKED);
			}
		}
	}
	ProcessSwtichLineAboutMyLocalVideo();
}
void   CVideoCanvasHandler::SetConnected(bool bConnected, int nActiveLine)
{
	bool bIsTalkGroup = m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT ? true : false;
	if (m_pWndVideoToolbar->GetSafeHwnd()!=NULL)
	{
		m_pWndVideoToolbar->SetFunBtnStat(TRUE, bIsTalkGroup);
		m_pWndVideoToolbar->Invalidate();


		HideWaitGif();
		LOG4_INFO("Set Video Tool Bar btn Enable");
		BeginTimer();
	}
	if (bIsTalkGroup)
	{
		auto vecGroupNumber = m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetContact();
		auto iterVecGroupNumber = vecGroupNumber.begin();
		for (iterVecGroupNumber; iterVecGroupNumber != vecGroupNumber.end(); iterVecGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterVecGroupNumber;
			if (pTalkGroupNumber->ReturnActiveLine() == nActiveLine)
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
}


bool CVideoCanvasHandler::GetIsConnected(int nActiveLine) {
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
				bRet = pTalkGroupNumber->m_bIsConnected;
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
bool CVideoCanvasHandler::GetIsConnectedNoLine( )
{
	bool bRet = false;
	CHAT_TAB eChatTab = CT_TEXT_CHAT;
	if (m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT)
	{
		eChatTab = (CHAT_TAB)m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetUserData();
		auto vecGroupNumber = m_pContactInfo->m_unionContactInfo.m_pChartGroup->GetContact();
		auto iterVecGroupNumber = vecGroupNumber.begin();
		for (iterVecGroupNumber; iterVecGroupNumber != vecGroupNumber.end(); iterVecGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterVecGroupNumber;
			if (eChatTab==CT_VIDEO)
			{
				bRet = pTalkGroupNumber->m_bIsConnected;
			}
			if (bRet==true)
			{
				break;
			}
		}
	}
	else
	{
		eChatTab = (CHAT_TAB)m_pContactInfo->m_unionContactInfo.m_pContact->GetUserData();
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (eChatTab == CT_VIDEO)
		{
			bRet = pContactDB->m_bIsConnected;
		}
	}
	return bRet;
}
bool CVideoCanvasHandler::GetCallStatus()
{
	bool bRet = false;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		if (pContactDB->ReturnActiveLine() < 0)
		{
			bRet =  false;
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
		for (iterGroupNumber; iterGroupNumber != vecGroupNumber.end(); iterGroupNumber++)
		{
			CTalkGroupNumber* pTalkGroupNumber = *iterGroupNumber;
			if (pTalkGroupNumber->ReturnActiveLine() < 0)
			{
				nNoCall++;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (nNoCall == vecGroupNumber.size())
		{
			bRet = false;
		}
	}
	return bRet;
}
void    CVideoCanvasHandler::HoldByRemote()
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB->ReturnActiveLine() < 0)
		{
			LOG4_INFO("the active line is error where hold by remote");
			return;
		}
	}
}
void   CVideoCanvasHandler::AnswerByRemote( int nActiveLine, bool bConferenceRoom)
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

#if _DEBUG
		TIMER_START(UpdateCallMsgStatus);
#endif
	

		if (pContactDB->m_nCallHistoryID != -1)
		{
			CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(pContactDB->m_nCallHistoryID, ENUM_CALL_STATUS_SUCC);
		}
#if _DEBUG
		TIMER_STOP(UpdateCallMsgStatus);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" UpdateCallMsgStatus %d ms.\n", TIMER_MSEC(UpdateCallMsgStatus));
		OutputDebugString(strFormatInfo);
#endif
		pContactDB->m_bIsConnected = true;
		if (pContactDB->GetIsTransfer())
		{
#if _DEBUG
			TIMER_START(TransferDlg);
#endif
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
					SwitchToContact(m_pContactInfo, CT_VIDEO);
				}
			}
#if _DEBUG
			TIMER_STOP(TransferDlg);
			CIUIString strFormatInfo;
			strFormatInfo.Format(L" TransferDlg %d ms.\n", TIMER_MSEC(TransferDlg));
			OutputDebugString(strFormatInfo);
#endif
			//DoHangup();
		}
		else if (pContactDB->GetIsAdded())
		{
#if _DEBUG
			TIMER_START(AddConfence);
#endif
			CAddCallerDlg* pAddContactDlg = (CAddCallerDlg*)pContactDB->GetAddConfenceDlg();
			if (pAddContactDlg != NULL)
			{
				ENUM_RECENT_CONTACT_TYPE enumRecentContactType = pAddContactDlg->GetSrcRecentContactType();
				if (enumRecentContactType==ENUM_SINGLE_CONTACT)
				{
					if (IsWindow(pAddContactDlg->m_hWnd))
					{
						pAddContactDlg->SetDesPagePointer(this);
						pAddContactDlg->DoAddedContact();
					}
					else
					{
						SwitchToContact(m_pContactInfo, CT_VIDEO);
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
						SwitchToContact(m_pContactInfo, CT_VIDEO);
					}
				}
			}
#if _DEBUG
			TIMER_STOP(AddConfence);
			CIUIString strFormatInfo;
			strFormatInfo.Format(L" TransferDlg %d ms.\n", TIMER_MSEC(AddConfence));
			OutputDebugString(strFormatInfo);
#endif
		}
		SetConnected(true, nActiveLine);
		
	
		//if (m_pWndVideoToolbar->GetSafeHwnd() != NULL)
		//{
		//	m_pWndVideoToolbar->SetFunBtnStat(TRUE);
			//test
		//	BeginTimer();
		//}
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
#if _DEBUG
	TIMER_START(SetIsConference);
#endif
	SetIsConference(bConferenceRoom);
#if _DEBUG
	TIMER_STOP(SetIsConference);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" SetIsConference %d ms.\n", TIMER_MSEC(SetIsConference));
	OutputDebugString(strFormatInfo);
#endif
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
}
void CVideoCanvasHandler::DoSingleContactHangup(bool bBtnSelf, bool bJumpPage)
{
#if _DEBUG
	TIMER_START(x);
#endif
	CleanChkStatus();
#if _DEBUG
	TIMER_STOP(x);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" Level 1 Cost %d ms.\n", TIMER_MSEC(x));
	OutputDebugString(strFormatInfo);
#endif
	if (m_pWndVideoToolbar->GetSafeHwnd() != NULL)
	{
		m_pWndVideoToolbar->ResetBtnStatus();
	}
#if _DEBUG
	TIMER_START(x2);
#endif
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	if (pContactDB->ReturnActiveLine() < 0)
	{
		LOG4_INFO("the active line is error where hangup by remote");
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
		pContactDB->SetActiveLine(-1);
		return;
	}
#if _DEBUG
	TIMER_STOP(x2);
	strFormatInfo;
	strFormatInfo.Format(L" Level 2 Cost %d ms.\n", TIMER_MSEC(x2));
	OutputDebugString(strFormatInfo);
#endif
	ENUM_CALL_STATUS enumCallStatus;
	if (GetIsConnected(pContactDB->ReturnActiveLine()) == true)
	{
		enumCallStatus = ENUM_CALL_STATUS_SUCC;
	}
	else
	{
		enumCallStatus = ENUM_CALL_STATUS_FAILD;
	}
	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
#if _DEBUG
	TIMER_START(x3);
#endif
	bool bTransferShouldChangeContactPage = false, bAddShouldChangeContactPage = false;
	bool bIsStartTransferContact = true, bIsAddContact = true;
	TAG_RECENT *pTransferContact = NULL;
	TAG_RECENT *pAddContact = NULL;
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
			pTransferContact = (TAG_RECENT*)m_pContactInfo->m_unionContactInfo.m_pContact->GetTransferContact();
			bIsStartTransferContact = m_pContactInfo->m_unionContactInfo.m_pContact->GetIsStartedContact();
			if (pTransferContact != NULL)
			{
				CHAT_TAB enumCallType = pCalingTransferDlg->GetTransferType();
				CVideoCanvasHandler* pVideoPage = (CVideoCanvasHandler*)pCalingTransferDlg->GetSrcCalingPage();
				if (pVideoPage&&enumCallType == CT_VIDEO)
				{
					pVideoPage->ProcessTransferFailed();
				}
				if (bBtnSelf == false)
				{

					CIUIString strErrorTip;
					strErrorTip.Format(L"call closed by %s  ", strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
				
					CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", strErrorTip, NULL);
				}
				bTransferShouldChangeContactPage = true;
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsTransfer(NULL, NULL, FALSE, FALSE);
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
				CVideoCanvasHandler* pVideoPage = (CVideoCanvasHandler*)pAddConfenceDlg->GetSrcCalingPage();
				if (pVideoPage&&enumCallType == CT_VIDEO)
				{
					pVideoPage->ProcessTransferFailed();
				}
				if (bBtnSelf == false)
				{

					CIUIString strErrorTip;
					strErrorTip.Format(L"call closed by %s  ", strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
				
					CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", strErrorTip, NULL);
				}
				bAddShouldChangeContactPage = true;
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsAddedConfence(NULL, NULL, FALSE, FALSE);
			}
		}

	}
#if _DEBUG
	TIMER_STOP(x3);
	strFormatInfo;
	strFormatInfo.Format(L" Level 3 Cost %d ms.\n", TIMER_MSEC(x3));
	OutputDebugString(strFormatInfo);
	TIMER_START(x4);
#endif
	CSystemEx::GetInstance()->Hangup(pContactDB->ReturnActiveLine());
#if _DEBUG
	TIMER_STOP(x4);
	strFormatInfo;
	strFormatInfo.Format(L" Level 4 Cost %d ms.\n", TIMER_MSEC(x4));
	OutputDebugString(strFormatInfo);
#endif
	CVideoCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pContactInfo);
	pContactDB->SetUserData(CT_TEXT_CHAT);
	pContactDB->SetCallType(ENUM_CALL_IN);
#if _DEBUG
	TIMER_START(x5);
#endif
	SwitchToContactNoBlock(m_pContactInfo, CT_TEXT_CHAT);
#if _DEBUG
	TIMER_STOP(x5);
	strFormatInfo;
	strFormatInfo.Format(L" Level 5 Cost %d ms.\n", TIMER_MSEC(x5));
#endif
	if (bTransferShouldChangeContactPage == true && pTransferContact != m_pContactInfo && bIsStartTransferContact != true && bJumpPage == false)
	{
		bool bIsConnected = pTransferContact->m_unionContactInfo.m_pContact->m_bIsConnected;
		if (bIsConnected)
		{
			SwitchToContact(pTransferContact, CT_VIDEO);
		}
	}
	if (bAddShouldChangeContactPage == true && pAddContact != m_pContactInfo && bIsAddContact != true && bJumpPage == false)
	{
		bool bIsConnected = pAddContact->m_unionContactInfo.m_pContact->m_bIsConnected;
		if (bIsConnected)
		{
			SwitchToContact(pAddContact, CT_VIDEO);
		}

	}
#if _DEBUG
	OutputDebugString(strFormatInfo);
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
	TIMER_START(x6);
#endif
	if (pContactDB->m_nCallHistoryID != -1)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgEndTimeByID(pContactDB->m_nCallHistoryID);
		CSystemEx::GetInstance()->GetDBOprate()->UpdateCallMsgStatus(pContactDB->m_nCallHistoryID, enumCallStatus);
		bool bFindRecord = false;
		CChatRecord theHistoryRecord = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByCallHistoryID(pContactDB->m_nCallHistoryID, bFindRecord);
		if (bFindRecord == true)
		{
			CSystemEx::GetInstance()->GetMainDlg()->InsertCallEventMsg(pContactDB->m_strSIPNum, theHistoryRecord, INSERT_FORWARD);
		}
	}
#if _DEBUG
	TIMER_STOP(x6);
	strFormatInfo;
	strFormatInfo.Format(L" Level 6 Cost %d ms.\n", TIMER_MSEC(x6));
	OutputDebugString(strFormatInfo);
	TIMER_START(x7);
#endif
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


	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
	CSystemEx::GetInstance()->EraseSession2CanvasHandler(pContactDB->m_nActiveLine);
	pContactDB->m_bIsConnected = false;
	pContactDB->SetActiveLine(-1);
	if (m_hDecodeTimer != 0)
	{
		::KillTimer(NULL, m_hDecodeTimer);
		m_hDecodeTimer = 0;
	}
#if _DEBUG
	TIMER_STOP(x7);
	strFormatInfo;
	strFormatInfo.Format(L" Level 7 Cost %d ms.\n", TIMER_MSEC(x7));
	OutputDebugString(strFormatInfo);
#endif
	//

	CloseAddCallerDlg();
	CloseTransferDlg();
	ShowVideoWindow(FALSE);
	CloseDTMF();
#if _DEBUG
	TIMER_START(x8);
#endif
	CloseLocalVideoShow();
#if _DEBUG
	TIMER_STOP(x8);
	strFormatInfo;
	strFormatInfo.Format(L" Level 8 Cost %d ms.\n", TIMER_MSEC(x8));
	OutputDebugString(strFormatInfo);
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
}

void CVideoCanvasHandler::CloseDTMF()
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
void CVideoCanvasHandler::CloseLocalVideoShow()
{
	HWND hWnd = CSystemEx::GetInstance()->GetMainDlg()->GetHWND();
	if (hWnd!=NULL)
	{
		PostMessage(hWnd, WM_CLOSE_LOCAL_VIDEO_WND, (WPARAM)m_hSmallVideoWnd, 0);
	}
	LOG4_INFO(L"realse dev\n");
	
}
void CVideoCanvasHandler::DoTalkGroupHangup()
{
	CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
	vector<CTalkGroupNumber*> vecTalkNumber = pTalkGroup->GetContact();
	auto iterTalkGroupNumber = vecTalkNumber.begin();
	for (iterTalkGroupNumber; iterTalkGroupNumber != vecTalkNumber.end(); iterTalkGroupNumber++)
	{
		CTalkGroupNumber* pTalkGroupNumber = *iterTalkGroupNumber;
		if (pTalkGroupNumber != NULL)
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
	if (m_pWndVideoToolbar->GetSafeHwnd() != NULL)
	{
		m_pWndVideoToolbar->SetFunBtnStat(FALSE,TRUE);
		m_pWndVideoToolbar->Invalidate();
	}
	CleanChkStatus();
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);

	CloseAddCallerDlg();
	CloseTransferDlg();

	CSystemEx::GetInstance()->DestoryConfence();
	SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	ShowVideoWindow(FALSE);
}
void CVideoCanvasHandler::HangupByRemote(bool bConnected, int nActiveSession, bool bJumpPage)
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
		DoSingleContactHangup(false,false);
	}
	else
	{
		ExistOneLine(nActiveSession,bConnected);
	}
}

void CVideoCanvasHandler::CloseTransferDlg()
{
	if (m_pCallingTransferDlg != NULL)
	{
		if (::IsWindow(m_pCallingTransferDlg->m_hWnd))
		{
			m_pCallingTransferDlg->EndDialog(IDOK);
			//delete m_pCallingTransferDlg;
			m_pCallingTransferDlg = NULL;
		}
	}
}
void CVideoCanvasHandler::CloseAddCallerDlg()
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

HWND CVideoCanvasHandler::GetPlayHwnd()
{
	return m_hVideoWnd;	
}
HWND CVideoCanvasHandler::GetLocalHwnd()
{
	return m_hSmallVideoWnd;
}
int CVideoCanvasHandler::SetContactInfo(TAG_RECENT *pContactInfo)
{
	m_pContactInfo = pContactInfo;
	return 0;
}

TAG_RECENT *CVideoCanvasHandler::GetContactInfo()
{
	return m_pContactInfo;
}
void CVideoCanvasHandler::ChangeDeviceSwtich(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera)
{
	if (m_pWndVideoToolbar==NULL)
	{
		return;
	}
	bool bMuteAudioIn = m_pWndVideoToolbar->GetMuteCheck() == BST_UNCHECKED ? true : false;
	bool bSetAudioIn = bMuteAudioIn && (bEnableAudioIn);

	bool bCamera = m_pWndVideoToolbar->GetVideoCheck() == BST_UNCHECKED ? true : false;
	bool bSetCamera = bCamera && (bEnableCamera);

	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{

		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		long nSessionID = CSystemEx::GetInstance()->GetSessionID(pContactDB->ReturnActiveLine());

	
		CSystemEx::GetInstance()->MuteSession(!bEnableAudioOut, !bSetAudioIn, !bSetCamera, nSessionID);

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
				CSystemEx::GetInstance()->MuteSession(!bEnableAudioOut, !bEnableAudioIn, !bSetCamera, nSessionID);
			}
		}
	}
}

int CVideoCanvasHandler::ToggleVideoWnd()
{
	if (::IsWindow(m_hVideoHost))
	{
		if (::IsZoomed(m_hVideoHost))
		{
			ShowWindow(m_hVideoHost, SW_RESTORE);
			LayoutVideoWnd(0, 0);
			::SendMessage(g_pMainFrame->GetSafeHwnd(), WM_EXIT_FULL_SCREEN_VIDEO, 0, 0);
		}
		else
		{
			ShowWindow(m_hVideoHost, SW_MAXIMIZE);
			LayoutVideoWnd(0, 0);
			ChangeMySelfWndSize();
		}
	}

	return 0;
}

void CVideoCanvasHandler::ChangeOtherPersonWndSize( bool bIsFullScreen)
{
	CIUIRect rcVideoCanvas;
	GetWindowRect(m_hVideoHost, rcVideoCanvas);
	if (!bIsFullScreen)
	{
		
	}
	else
	{
		::MoveWindow(m_hVideoWnd, 0, 0,
			rcVideoCanvas.Width(), rcVideoCanvas.Height(), FALSE);
	}
}
HWND CVideoCanvasHandler::CreateWaitVideoHost(HWND hParent)
{
	if (m_pwndVideoHost!=NULL)
	{
		if (m_pwndVideoHost->GetSafeHwnd() != NULL)
		{
			m_pwndVideoHost->Close();
			delete m_pwndVideoHost;
			m_pwndVideoHost = NULL;
		}
	}
	m_pwndVideoHost = new CVideoHostWnd();
	m_pwndVideoHost->CreatePopupWindow(_T("IDW_VIDEO_HOST.xml"), hParent);
	return m_pwndVideoHost->GetSafeHwnd();
}

void CVideoCanvasHandler::HideWaitGif()
{
	if (m_pwndVideoHost->GetSafeHwnd() != NULL)
	{
		m_pwndVideoHost->HideLabelWait();
	}
	if (::IsWindow(m_hVideoWnd))
	{
		ShowWindow(m_hVideoWnd,SW_SHOW);
	}
	
}

bool CVideoCanvasHandler::ShowLocalVideoInConference()
{
	bool bSelfVideo = CSystemEx::GetInstance()->GetGlobalOptions()->m_bLocalVideoInRoom;
	bool bConfrence = m_pContactInfo->m_enumRecentContact == ENUM_MULTI_CONTACT;
	bConfrence |= m_bIsConference;
	if (::IsWindow(m_hSmallVideoWnd))
	{
		if (bConfrence == false)
		{
			if (IsWindowVisible(m_hSmallVideoWnd)==FALSE)
			{
				ShowWindow(m_hSmallVideoWnd, SW_SHOW);
				BringWindowToTop(m_hSmallVideoWnd);
			}
			return true;
		}
		else if (bSelfVideo == true && bConfrence == true)
		{
			if (IsWindowVisible(m_hSmallVideoWnd) == TRUE)
			{
				ShowWindow(m_hSmallVideoWnd, SW_HIDE);
			}
			return false;
		}
		else if (bSelfVideo==false&&bConfrence==true)
		{
			if (IsWindowVisible(m_hSmallVideoWnd) == FALSE)
			{
				ShowWindow(m_hSmallVideoWnd, SW_SHOW);
			}
			return true;
		}
	}
	return false;
}

void CVideoCanvasHandler::ShowWaitGif()
{
	TIMER_START(x);
	if (::IsWindow(m_hVideoWnd))
	{
		ShowWindow(m_hVideoWnd, SW_HIDE);

		TIMER_STOP(x);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" Level 111 Cost %d ms.\n", TIMER_MSEC(x));
		OutputDebugString(strFormatInfo);
	}
	TIMER_START(x1);
	if (::IsWindow(m_hSmallVideoWnd))
	{
		ShowWindow(m_hSmallVideoWnd, SW_HIDE);
		TIMER_STOP(x1);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" Level 2222 Cost %d ms.\n", TIMER_MSEC(x1));
		OutputDebugString(strFormatInfo);
	}
	TIMER_START(x2);
	if (m_pwndVideoHost->GetSafeHwnd() != NULL)
	{
		m_pwndVideoHost->ShowLabelWait();
		TIMER_STOP(x2);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" Level 3333 Cost %d ms.\n", TIMER_MSEC(x2));
		OutputDebugString(strFormatInfo);
	}

}
void CVideoCanvasHandler::ChangeMySelfWndSize()
{
	if (m_bSaveNormalPT == false)
	{
		return;
	}
	CRect rectHostVideo;
	GetClientRect(m_hVideoHost, &rectHostVideo);
	int cy = rectHostVideo.Height();
	int cx = rectHostVideo.Width();

	float fsp[2];
	fsp[0] = (float)cx / m_ptNormal.x;
	fsp[1] = (float)cy / m_ptNormal.y;
	CPoint TLPoint; 
	CPoint BRPoint; 
	if (m_hSmallVideoWnd != NULL)
	{
		if (::IsWindow(m_hSmallVideoWnd))
		{
			CRect rcMySelf;
			::GetWindowRect(m_hSmallVideoWnd, &rcMySelf);
			int nMySelfVideoWidth = rcMySelf.Width();
			int nMySelfVideoHeight = rcMySelf.Height();

			int nNewMySelfVideoWidth = nMySelfVideoWidth * fsp[0]* SELF_VIDEO_SIZE_RATIO;
			int nNewMySelfVideoHeight = nMySelfVideoHeight * fsp[1]* SELF_VIDEO_SIZE_RATIO;
			//move
		
			bool bShow = ShowLocalVideoInConference();	
			if (bShow)
			{
				::MoveWindow(m_hSmallVideoWnd, rectHostVideo.right - nNewMySelfVideoWidth, rcMySelf.top, nNewMySelfVideoWidth, nNewMySelfVideoHeight, true);
			}
		}
	}
}

int CVideoCanvasHandler::LayoutVideoWnd(WPARAM wParam, LPARAM lParam)
{
	WINDOWPOS *pPos = (WINDOWPOS *)lParam;
	if (NULL != pPos)
	{
		if (pPos->flags & SWP_HIDEWINDOW)
		{
			::ShowWindow(m_hVideoHost, SW_HIDE);
			m_bOldVideoVisibleState = FALSE;
		}
		else if (pPos->flags & SWP_SHOWWINDOW)
		{
			if (GetIsConnectedNoLine() == true)
			{
				::ShowWindow(m_hVideoHost, SW_SHOW);
			}
			HWLWND hParent = IUI::GetParent(GetBindWLWnd());
			if (NULL != hParent && IUI::GetClassName(hParent) == IUI_TABCONTROL)
			{
				TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(hParent);
				_ASSERT(pTbChat != NULL);
				if (pTbChat->GetCurPropertySheet() == CT_VIDEO)
				{
					ShowVideoWindow(TRUE);
				}

			}
		}
	}

	bool bIsFullScreen = ::IsZoomed(m_hVideoHost);
	if (::IsWindow(m_hVideoHost))
	{
		if (!::IsMinimized(g_pMainFrame->GetSafeHwnd())&& bIsFullScreen==false)
		{
			CIUIRect rcVideoCanvas;
			LONG lStyle = GetWindowLong(m_hVideoHost, GWL_STYLE);
			if (IsIncludeFlag(lStyle, WS_CHILD))
			{
				GetWindowRectToHWND(GetBindWLWnd(), rcVideoCanvas);
			}
			else
			{
				GetWindowRect(GetBindWLWnd(), rcVideoCanvas);
			}
			::MoveWindow(m_hVideoHost, rcVideoCanvas.left, rcVideoCanvas.top,
				rcVideoCanvas.Width(), rcVideoCanvas.Height(), FALSE);
	
			::MoveWindow(m_hVideoWnd, 0, 0,
				rcVideoCanvas.Width(), rcVideoCanvas.Height(), FALSE);
			int nLocalWidth = MAX_LOCAL_VIDEO_WIDTH_NORMAL;
			int nLocalHeight = MAX_LOCAL_VIDEO_HEIGHT_NORMAL;
			CRect rectHostVideo;
			GetClientRect(m_hVideoHost, &rectHostVideo);
			bool bShow = ShowLocalVideoInConference();
			if (bShow)
			{
				::MoveWindow(m_hSmallVideoWnd, rectHostVideo.right - nLocalWidth, rectHostVideo.top, nLocalWidth, nLocalHeight, true);
			}
			
		}
		if (!::IsMinimized(g_pMainFrame->GetSafeHwnd()))
		{
			ChangeOtherPersonWndSize(IsZoomed(m_hVideoHost));
		}
	
	}

	return 0;
}

void CVideoCanvasHandler::ProcessTranseferSuccess(bool bSuccess, int nErrorCode)
{
	if (m_pCallingTransferDlg != NULL)
	{
		m_pCallingTransferDlg->ProcessTranseferSuccess(bSuccess,  nErrorCode);
	}
	if (bSuccess == false)
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

				if (pAudioPage&&enumCallType == CT_VIDEO)
				{
					pAudioPage->ProcessTransferFailed();
				}
				SwitchToContact(pTransferContact, CT_VIDEO);
				m_pContactInfo->m_unionContactInfo.m_pContact->SetIsTransfer(NULL, NULL, FALSE, FALSE);
			}
		}
	}
}

void CVideoCanvasHandler::ExistOneLine(int nExitActiveLine,bool bConnected)
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

	if (nFailCount == vecTalkNumber.size())
	{
		pTalkGroup->SetUserData(CT_TEXT_CHAT);
		if (m_pWndVideoToolbar->GetSafeHwnd() != NULL)
		{
			this;
			CIUIString strInfo;
			strInfo.Format(L"%x this\n", this);
			OutputDebugString(strInfo);

			strInfo.Format(L"%x m_pWndVideoToolbar\n", m_pWndVideoToolbar);
			OutputDebugString(strInfo);
			CleanChkStatus();
			m_pWndVideoToolbar->ResetBtnStatus();
		}
		CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
		m_pWndVideoToolbar->Invalidate();
		CSystemEx::GetInstance()->DestoryConfence();
		SwitchToContact(m_pContactInfo, CT_TEXT_CHAT);
	}
}
void CVideoCanvasHandler::ClearnCallData()
{
	TAG_RECENT *pContactInfo = m_pContactInfo;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		m_pContactInfo->m_unionContactInfo.m_pContact->ClearnCallData();
		m_pContactInfo->m_unionContactInfo.m_pContact->m_bIsConnected = false;
	}
}


void CVideoCanvasHandler::TransferClosed()
{
	if (m_pCallingTransferDlg != NULL)
	{
		m_pCallingTransferDlg = NULL;
	}
}

int CVideoCanvasHandler::ShowVideoWindow(BOOL bShow)
{
	::ShowWindow(m_hVideoHost, bShow ==TRUE? SW_SHOW : SW_HIDE);
	m_bOldVideoVisibleState = bShow;
	return 0;
}
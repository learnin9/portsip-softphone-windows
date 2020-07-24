#include "stdafx.h"
#include "MainWnd.h"
#include "MainFrameTabControl/MainPanel/MainCanvasHandler.h"
#include "MainFrameTabControl/CallPanel/CallCanvasHandler.h"
#include "MainFrameTabControl/SearchPanel/SearchCanvasHandler.h"
#include "MainFrameTabControl/MainPanel/MainTabControl/InboxPanel/InboxCanvasHandler.h"
#include "MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/ChatTabControlHandler.h"
#include "MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/VideoCanvasHandler.h"
#include <shellapi.h>
#include "System/SystemEx.h"

CMainWnd::CMainWnd()
	: m_pBtnIcon(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnMin(NULL)
	, m_pChkMax(NULL)
	, m_pBtnClose(NULL)
	, m_pTabMainFrame(NULL)
	, m_pCanvasModalMask(NULL)
	, m_pBtnCall(NULL)
	, m_pBtnCloseCallPage(NULL)
	, m_pBtnSearch(NULL)
	, m_pBtnCloseSearchPage(NULL)
	, m_bTrayLButtonDown(FALSE)
{
	m_pMainCanvasHandler = NULL;
}

CMainWnd::~CMainWnd()
{
}

int CMainWnd::InitControls()
{
	m_pBtnIcon = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_ICON")));
	_ASSERT(m_pBtnIcon != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pTabMainFrame = (TabControl *)CWLWnd::FromHandle(FindControl(_T("IDC_PNL_TAB_MAIN_FRAME")));
	_ASSERT(m_pTabMainFrame != NULL);
	m_pCanvasModalMask = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_MODAL_MASK")));
	_ASSERT(m_pCanvasModalMask != NULL);
	CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
	m_pStaTitle->SetWindowTextW(strAppName);
#ifdef BZXX
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_NORMAL, L"Logo_Small_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_HOVER, L"Logo_Small_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_PRESSED, L"Logo_Small_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_DISABLED, L"Logo_Small_BZXX.png");
#endif

#ifdef KCXX
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_NORMAL, L"Logo_Small_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_HOVER, L"Logo_Small_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_PRESSED, L"Logo_Small_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_BTN_ICON")), FALSE, CONTROL_STATE_DISABLED, L"Logo_Small_KCXX.png");
#endif

	m_vecContact = CSystemEx::GetInstance()->GetDBOprate()->getAllContacts(MAX_CONTACT_NUM,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());

	m_vecDBContact = CSystemEx::GetInstance()->GetDBOprate()->getAllContacts(MAX_CONTACT_NUM, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	InitFavorite();
	m_vecExtersionContact = CSystemEx::GetInstance()->GetRestAPI()->GetExtensions();
	m_vecRemoteContact = CSystemEx::GetInstance()->GetRestAPI()->GetRemoteContacts();
	//CSystemEx::GetInstance()->GetRestAPI()->GetAllConferenceRoomNumber();
	//ExtersionExistContactInLocal();
	//RemoteExistContactInLocal();
	m_OnLineStatusMgr.InitLocalContact(m_vecExtersionContact);
	m_OnLineStatusMgr.StartSub();


	
	return 0;
}
/*
void CMainWnd::UpdateContacts()
{
	CContactDB *pContactDB = NULL;
	
	vector<CContactDB*> vecReslutFromDB;
	vecReslutFromDB = CSystemEx::GetInstance()->GetDBOprate()->getAllContacts(MAX_CONTACT_NUM);
	auto iterNewContactDB = vecReslutFromDB.begin();
	
	CContactDB *pNewContact = NULL;
	CContactDB *pCurrentContact = NULL;

	for(iterNewContactDB; iterNewContactDB != vecReslutFromDB.end(); iterNewContactDB++)
	{
		pNewContact = *iterNewContactDB;
		auto iterCurrentContactDB = m_vecContact.begin();
		bool bFind = false;
		for (iterCurrentContactDB; iterCurrentContactDB!=m_vecContact.end();iterCurrentContactDB++)
		{
			
			pCurrentContact = *iterCurrentContactDB;
			if (PortUtility::compareStringNoCase(pCurrentContact->m_strSIPNum, pNewContact->m_strSIPNum)==0)
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			m_vecContact.push_back(pNewContact);
		}
	} 
	
}
*/
void CMainWnd::ExtersionExistContactInLocal()
{
	return;
	auto iterLocalDBContact = m_vecDBContact.begin();
	for (iterLocalDBContact;iterLocalDBContact!=m_vecDBContact.end();iterLocalDBContact++)
	{
		TAG_RECENT *pLocalContact = *iterLocalDBContact;
		auto iterExtersion = m_vecExtersionContact.begin();
		for (iterExtersion;iterExtersion!=m_vecExtersionContact.end();iterExtersion++)
		{
			TAG_RECENT *pExtersionContact = *iterExtersion;
			std::string strLocalSipNumber = pLocalContact->m_unionContactInfo.m_pContact->m_strSIPNum;
			std::string strExtersionNumber = pExtersionContact->m_unionContactInfo.m_pContact->m_strSIPNum;
			if (!strLocalSipNumber.empty()&&!strExtersionNumber.empty())
			{
				if (PortUtility::compareStringNoCase(strLocalSipNumber, strExtersionNumber) == 0)
				{
					pExtersionContact->m_unionContactInfo.m_pContact->m_strFirstName = pLocalContact->m_unionContactInfo.m_pContact->m_strFirstName;
					pExtersionContact->m_unionContactInfo.m_pContact->m_strLastName = pLocalContact->m_unionContactInfo.m_pContact->m_strLastName;

				}
			}
			
		}
	}
}
void CMainWnd::RemoteExistContactInLocal()
{
	return;
	auto iterLocalDBContact = m_vecDBContact.begin();
	for (iterLocalDBContact; iterLocalDBContact != m_vecDBContact.end(); iterLocalDBContact++)
	{
		TAG_RECENT *pLocalContact = *iterLocalDBContact;
		auto iterExtersion = m_vecRemoteContact.begin();
		for (iterExtersion; iterExtersion != m_vecRemoteContact.end(); iterExtersion++)
		{
			TAG_RECENT *pExtersionContact = *iterExtersion;
			std::string strLocalSipNumber = pLocalContact->m_unionContactInfo.m_pContact->m_strSIPNum;
			std::string strExtersionNumber = pExtersionContact->m_unionContactInfo.m_pContact->m_strSIPNum;
			if (!strLocalSipNumber.empty() && !strExtersionNumber.empty())
			{
				if (PortUtility::compareStringNoCase(strLocalSipNumber, strExtersionNumber) == 0)
				{
					pExtersionContact->m_unionContactInfo.m_pContact->m_strFirstName = pLocalContact->m_unionContactInfo.m_pContact->m_strFirstName;
					pExtersionContact->m_unionContactInfo.m_pContact->m_strLastName = pLocalContact->m_unionContactInfo.m_pContact->m_strLastName;

				}
			}

		}
	}
}
void CMainWnd::InitControlsData()
{
	CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
	if (pInboxCanvasHandler!=NULL)
	{
		pInboxCanvasHandler->InitHistory();
		pInboxCanvasHandler->InitMiss();
		pInboxCanvasHandler->InitVoiceMail();
		pInboxCanvasHandler->InitRecord();
		
	}
	
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler!=NULL)
	{
	}
	CSystemEx::GetInstance()->BeginVoiceMailPollThread();
}

int CMainWnd::SwitchToPropertySheet(MAINFRAME_TAB_KEY ePage)
{
	return m_pTabMainFrame->SwitchPropertySheet(ePage);
}
CMainCanvasHandler* CMainWnd::TestGetMainCanvasHandler()
{
	// Message handler
	HWLWND m_hPropertySheet = m_pTabMainFrame->GetPropertySheet(MTK_MAIN)->m_hPropertySheet;
	CMainCanvasHandler *pMainCanvasHandler = (CMainCanvasHandler*)CWLWnd::FromHandle(m_hPropertySheet);
	return pMainCanvasHandler;
}

LRESULT CMainWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	CSystemEx::GetInstance()->SetMainDlg(this);
	g_hMainFrame = m_hWnd;

	InitControls();

	//SetTrueBorderThickness(CIUIRect(1, 1, 1, 1));
	 
	CMainCanvasHandler*  PTHEIS = ALLOC_HANDLER(CMainCanvasHandler); 
	m_pMainCanvasHandler = PTHEIS;
	
	m_pTabMainFrame->AddPropertySheet(MTK_MAIN,
		_T("IDW_MAIN.xml"), PTHEIS);
	m_pTabMainFrame->AddPropertySheet(MTK_CALL,
		_T("IDW_CALL_PAGE.xml"), ALLOC_HANDLER(CCallCanvasHandler));
	m_pTabMainFrame->AddPropertySheet(MTK_SEARCH,
		_T("IDW_SEARCH_PAGE.xml"), ALLOC_HANDLER(CSearchCanvasHandler));
	SwitchToPropertySheet(MTK_MAIN);

	MoveWindow(m_hWnd, 0, 0, 1200, 700, FALSE);


	CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
	NOTIFYICONDATA tnd;
	tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
	tnd.hWnd = m_hWnd;
#ifdef BZXX
	tnd.uID = IDI_BZXX_PG;
	tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif
	
#ifdef KCXX
	tnd.uID = IDI_KCXX_PG;
	tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYNOTIFY;

	Shell_NotifyIcon(NIM_ADD, &tnd);

	if (NULL == m_pTaskBarlist)
	{
		CoCreateInstance(
			CLSID_TaskbarList, NULL, CLSCTX_ALL,
			IID_ITaskbarList3, (void**)&m_pTaskBarlist);
	}
	m_pTaskBarlist->SetProgressState(m_hWnd, TBPF_INDETERMINATE);

	InitControlsData();
	//TestFunDownLoad(); OK
	//TestFunUpLoad();  OK
	return 0;
}

LRESULT CMainWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_GETMINMAXINFO == uMsg)
	{
		MINMAXINFO *pMinMaxInfo = (MINMAXINFO *)lParam;

		if (NULL != pMinMaxInfo)
		{
			pMinMaxInfo->ptMinTrackSize.x = 1000;
			pMinMaxInfo->ptMinTrackSize.y = 700;
			CIUIRect rect;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
			pMinMaxInfo->ptMaxSize.x = rect.Width();
			pMinMaxInfo->ptMaxSize.y = rect.Height();
			return 0;
		}
	}
	else if (WM_ENABLE == uMsg)
	{
		BOOL bEnabled = wParam;
		if (bEnabled)
		{
			m_pCanvasModalMask->ShowWindow(SW_HIDE);
		}
		else
		{
			m_pCanvasModalMask->ShowWindow(SW_SHOW);
		}
		IUIUpdateWindow(m_hWnd);
	}
	else if (WM_SWITCH_TO_CONTACT == uMsg)
	{
		OnSwitchToContact(wParam, lParam);
	}
	else if (WM_EXIT_FULL_SCREEN_VIDEO == uMsg)
	{
	}
	else if (WM_TRAYNOTIFY == uMsg)
	{
		OnTrayNotify(wParam, lParam);
	}
	else  if (WM_SIPEVENT==uMsg)
	{
		OnSIPCallbackEvent(wParam, lParam);
	}
	else if (WM_ANSWER_BY_REMOTE == uMsg)
	{
		OnAnswerByRemote(wParam, lParam);
	}
	else if (WM_DOWNLOAD_FILE==uMsg)
	{
#if _DEBUG
		TIMER_START(OnProcessDownLoadFile);
#endif
		OnProcessDownLoadFile(wParam, lParam);
#if _DEBUG
		TIMER_STOP(OnProcessDownLoadFile);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" OnProcessDownLoadFile %d ms.\n", TIMER_MSEC(OnProcessDownLoadFile));
		OutputDebugString(strFormatInfo);
#endif
	}
	else if (WM_UPLOAD_FILE ==uMsg)
	{
		OnProcessUpLoadFile(wParam, lParam);
	}
	else if (WM_UPLOAD_FILE_GET_OK == uMsg)
	{
#if _DEBUG
		TIMER_START(OnUploadFileGetOk);
#endif
		OnUploadFileGetOk(wParam, lParam);
#if _DEBUG
		TIMER_STOP(OnUploadFileGetOk);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" OnUploadFileGetOk %d ms.\n", TIMER_MSEC(OnUploadFileGetOk));
		OutputDebugString(strFormatInfo);
#endif
	}
	else if (WM_DOWNLOAD_VOICE_MAIL == uMsg)
	{
		OnDownLoadVoiceMailOK(wParam, lParam);
	}
	else if (WM_DOWNLOAD_FILE_OK == uMsg)
	{
#if _DEBUG
		TIMER_START(ProcessDownLoadFileOK);
#endif
		ProcessDownLoadFileOK(wParam, lParam);
#if _DEBUG
		TIMER_STOP(ProcessDownLoadFileOK);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" ProcessDownLoadFileOK %d ms.\n", TIMER_MSEC(ProcessDownLoadFileOK));
		OutputDebugString(strFormatInfo);
#endif
	}
	else if (WM_CONTACT_INFO_UPDATE == uMsg)
	{
		UpdateContactInfo(wParam, lParam);
	}
	else if (WM_CALL_ACTION_SINGLE == uMsg)
	{
		OnCallAction(wParam, lParam);
	}
	else if (WM_WINDOWPOSCHANGED == uMsg)
	{
		HWLWND hVideoPage = (HWLWND)GetProp(m_hWnd, GETPROP_VEDIO_HWND);
		IUI::SendMessage(hVideoPage, WM_HOST_WINDOWPOSCHANGED, wParam, lParam);
	}
	else if (WM_MOVE == uMsg)
	{
		LayoutMsgHistoryWnd();
	}
	else if (WM_CREATE_INFO_WND==uMsg)
	{
		OnCreateInfoWnd(wParam, lParam);
	}
	else if (WM_CONFENCE_ACTION ==uMsg)
	{
		OnVideoConfenceAction(wParam, lParam);
	}
	else if (WM_SET_IS_CONFERENCE==uMsg)
	{
		OnGetIsConference(wParam, lParam);
	}
	else if (WM_EXIT_CONFERENCE==uMsg)
	{
		OnUnGetConferenceStatus(wParam, lParam);
	}
	else if (WM_RESTORE_MAIN_DLG ==uMsg)
	{
		OnRestoreMainDlg(wParam, lParam);
	}
	else if (WM_CLOSE_LOCAL_VIDEO_WND==uMsg)
	{
		OnCloseLocalVideoWnd(wParam, lParam);
	}
	else if (WM_MOUSEWHEEL == uMsg)
	{
		CIUIPoint pt(lParam);
		CIUIPoint ptCtrl = pt;
		::ScreenToClient(m_hWnd, &ptCtrl);
		HWLWND hHittest = IUI::WindowFromPoint(m_hWnd, ptCtrl);
		if (NULL != hHittest)
		{
			if (GetClassName(hHittest) == IUI_TREEVIEW
				|| GetClassName(hHittest) == IUI_IM
				|| GetClassName(hHittest) == IUI_LISTVIEW
				|| GetClassName(hHittest) == IUI_RICHTEXTBOX)
			{
				RoutedEventArgs arg;
				arg.hOriginalSender = hHittest;
				arg.RoutedEvent = uMsg;
				arg.eRoutingStrategy = RS_TUNNEL;
				arg.wParam = wParam;
				arg.lParam = MAKELPARAM(pt.x, pt.y);
				LRESULT lr = RaiseEvent(hHittest, &arg);
				return lr;
			}
		}
	}
	if (uMsg == WM_CHAR)
	{
		LPMSG lpMsg = (LPMSG)lParam;
		if (lpMsg != NULL)
		{
			char ascii_code = wParam;
			stringstream strstrTestOut;
			strstrTestOut << "WM_CHAR: Character =" << ascii_code<<endl;
			string strTestOut = strstrTestOut.str();
			OutputDebugString(PortUtility::Utf82Unicode_ND(strTestOut).c_str());
		 } 
	}
	
	return Window::WindowProc(uMsg, wParam, lParam);
}
LRESULT CMainWnd::OnCreateInfoWnd(WPARAM wParam, LPARAM lParam)
{
	InfoWndParam *pParam = (InfoWndParam*)wParam;
	 m_oInfoMgr.CreatInfoWnd(pParam->strTitle, pParam->strMsg, GetSafeHwnd(), pParam->nAutoTimer);
	 delete pParam;
	 pParam = NULL;
	 return 0;
}
LRESULT CMainWnd::OnAnswerByRemote(WPARAM wParam, LPARAM lParam)
{

	CIUIString strFormatInfo;
	strFormatInfo.Format(L" OnAnswerByRemote.\n");
	OutputDebugString(strFormatInfo);

	int nIndex = (int)wParam;
	bool bIsConference = (bool)lParam;
	CSystemEx::GetInstance()->AnswerByRemote(nIndex, bIsConference);

// 	if (bIsConference)
// 	{
//		TAG_RECENT *pContactInfo = NULL;
//		pContactInfo = FindContactDBBySipNumber(strSipNumber);
// 		if (pContactInfo)
// 		{
// 			CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
// 			if (pChatTabControlHandler)
// 			{
// 				pChatTabControlHandler->SetIsConference(bIsConference);
// 			}
// 		}
// 	}

	return 0;
}
LRESULT CMainWnd::OnRestoreMainDlg(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	return 0;
}
LRESULT CMainWnd::OnCloseLocalVideoWnd(WPARAM wParam, LPARAM lParam)
{
	HWND hSmallVideoWnd = (HWND)wParam;
	std::string strErrofInfo;
	{
		CSystemEx::GetInstance()->StopLocalVideoHwnd(hSmallVideoWnd, false, strErrofInfo);
	}

	return 0;
}
LRESULT CMainWnd::OnVideoConfenceAction(WPARAM wParam, LPARAM lParam)
{
	CVideoCanvasHandler* pVideoCanvasHandler = (CVideoCanvasHandler*)wParam;
	ENUM_RECENT_CONTACT_TYPE enumSrcConfenceType = (ENUM_RECENT_CONTACT_TYPE)lParam;
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->OnProcessConfenceAction(enumSrcConfenceType);
	}
	return 0;
}
LRESULT CMainWnd::OnGetIsConference(WPARAM wParam, LPARAM lParam)
{
	TAG_ASYNC_SIP_CONFERENCE_PARAM *pConferenceParam = (TAG_ASYNC_SIP_CONFERENCE_PARAM*)wParam;
	bool bIsConference = (bool) wParam;
	if (bIsConference)
	{
		m_oConferenceMgr.AddConferenceNumber(pConferenceParam->m_strSipNumber);
	}
	else
	{
		//do nothing
	}
	delete pConferenceParam;
	pConferenceParam = NULL;

	return 0;
}
LRESULT CMainWnd::OnUnGetConferenceStatus(WPARAM wParam,LPARAM lParam)
{
	std::string strConferenceNumber = "";
	strConferenceNumber.append((const char *)wParam);

	m_oConferenceMgr.UnSubConferenceNumber(strConferenceNumber);
	return 0;
}

LRESULT CMainWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == WM_CREATE)
		{
			OnEventCreate(pe);
		}
		else if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			OnButtonClicked(pe);
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

HRESULT CMainWnd::FlashInTaskBar()
{
		if (!(GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE))
		{
			ShowWindow(SW_RESTORE);
		}
		if (HINSTANCE hUser = LoadLibrary(_T("User32")))
		{
			BOOL(WINAPI *pfnFlashWindowEx)(PFLASHWINFO pfwi);
			(FARPROC&)pfnFlashWindowEx = GetProcAddress(hUser, "FlashWindowEx");
			if (pfnFlashWindowEx)
			{
				FLASHWINFO pFWX;
				pFWX.cbSize = sizeof(pFWX);
				pFWX.dwFlags = 0x00000003 | 0x0000000C;
				pFWX.uCount = 3;
				pFWX.dwTimeout = 0;
				pFWX.hwnd = m_hWnd;
				(*pfnFlashWindowEx)(&pFWX);
			}
			FreeLibrary(hUser);
		}
		return 0;
}
LRESULT CMainWnd::OnBtnIcon(RoutedEventArgs *pe)
{
#ifdef BZXX
	string command = "https://www.portsip.com/portsip-pbx/";
#endif

#ifdef KCXX
	string command = "http://chinacreator.com/cn/";
#endif
	ShellExecuteA(m_hWnd, "OPEN", command.c_str(), NULL, NULL, SW_NORMAL);

// 	if (nTest == 0)
// 	{
// 		nPos = 0;
// 		m_pTaskBarlist->SetProgressState(m_hWnd, TBPF_NOPROGRESS);
// 		FlashWindow(m_hWnd, true);
// 
// 	}
// 	else
// 	{
// 		FlashWindowEx(,FLASHW_TRAY | FLASHW_TIMERNOFG, 2, 0);
// 	}
// 	
// 	nPos ++;
	FlashInTaskBar();
	return 0;
}

LRESULT CMainWnd::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainWnd::OnChkMax(RoutedEventArgs *pe)
{
	return 0;}

LRESULT CMainWnd::OnBtnClose(RoutedEventArgs *pe)
{
	::ShowWindow(m_hWnd, SW_HIDE);
	
	return 0;
}

LRESULT CMainWnd::OnEventCreate(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_BTN_CALL"))
	{
		_ASSERT(m_pBtnCall == NULL);

		m_pBtnCall = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_CLOSE_CALL_PAGE"))
	{
		_ASSERT(m_pBtnCloseCallPage == NULL);
		m_pBtnCloseCallPage = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_SEARCH"))
	{
		_ASSERT(m_pBtnSearch == NULL);

		m_pBtnSearch = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_CLOSE_SEARCH_PAGE"))
	{
		_ASSERT(m_pBtnCloseSearchPage == NULL);
		m_pBtnCloseSearchPage = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	}

	return 0;
}

LRESULT CMainWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnIcon->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnIcon(pe);
	}
	else if (m_pBtnMin->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMin(pe);
	}
	else if (m_pChkMax->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMax(pe);
	}
	else if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}
	else if (m_pBtnCall->GetSafeHwnd() == pe->hOriginalSender)
	{
		SwitchToPropertySheet(MTK_CALL);
	}
	else if (m_pBtnCloseCallPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		SwitchToPropertySheet(MTK_MAIN);
	}
	else if (m_pBtnSearch->GetSafeHwnd() == pe->hOriginalSender)
	{
		SwitchToPropertySheet(MTK_SEARCH);
	}
	else if (m_pBtnCloseSearchPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		SwitchToPropertySheet(MTK_MAIN);
	} 

	return 0;
}
void CMainWnd::RefreshMsgStatus(TAG_RECENT*pContact, long lSignMsgID,bool bSucc)
{
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			bool bFind = false;
			CChatRecord theLocalRecord = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByLocalSignMsgID(lSignMsgID, bFind);
			if (bFind)
			{
				pChatTabControlHandler->RefreshMsgStatus(theLocalRecord.m_nMsgID, bSucc);
			}
		}
		else
		{
			return;
		}

	}
}

void CMainWnd::InsertMsg2Chat(TAG_RECENT* pContact, std::string strMsg, std::string strMsgTime,int lMsgID,CIUIString strDisplayName)
{
	if (m_pMainCanvasHandler!=NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsg(strMsg, strMsgTime,lMsgID, strDisplayName);
		}
		else
		{
			SwitchToContact(pContact, CT_TEXT_CHAT);
			CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
			if (pChatTabControlHandler!=NULL)
			{
				pChatTabControlHandler->InsertMsg(strMsg, strMsgTime, lMsgID, strDisplayName);
			}
			
		}
		
	}
}
LRESULT CMainWnd::OnCallAction(WPARAM wParam, LPARAM lParam)
{
	TAG_RECENT *pContactInfo = (TAG_RECENT*)wParam;
	CVideoCanvasHandler *pVideoCanvasHandler = (CVideoCanvasHandler*)lParam;

	if (pVideoCanvasHandler != NULL&& pContactInfo!=NULL)
	{
		pVideoCanvasHandler->OnCallAction(pContactInfo);
	}
	return 0;
}

LRESULT CMainWnd::OnSwitchToContact(WPARAM wParam, LPARAM lParam)
{
#if _DEBUG
	TIMER_START(SwitchToPropertySheet);
#endif
	SwitchToPropertySheet(MTK_MAIN);
#if _DEBUG
	TIMER_STOP(SwitchToPropertySheet);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" SwitchToPropertySheet MTK_MAIN Cost %d ms.\n", TIMER_MSEC(SwitchToPropertySheet));
	OutputDebugString(strFormatInfo);
#endif
#if _DEBUG
	TIMER_START(GetPropertySheet);
#endif
	UINT uKey = m_pTabMainFrame->GetCurPropertySheet();
	CPropertySheetInfo *ppsi = m_pTabMainFrame->GetPropertySheet(uKey);
	if (NULL != ppsi)
	{
		if (NULL != ppsi->m_hPropertySheet)
		{
			_ASSERT(GetName(ppsi->m_hPropertySheet) == _T("IDC_CANVAS_MAIN"));
#if _DEBUG
			TIMER_STOP(GetPropertySheet);
			CIUIString strFormatInfo;
			strFormatInfo.Format(L" GetPropertySheet  IDC_CANVAS_MAIN Cost %d ms.\n", TIMER_MSEC(GetPropertySheet));
			OutputDebugString(strFormatInfo);
#endif
			IUI::SendMessage(ppsi->m_hPropertySheet, WM_SWITCH_TO_CONTACT, wParam, lParam);
		}
		else
		{
			_ASSERT(FALSE);
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}


LRESULT CMainWnd::OnSIPCallbackEvent(WPARAM WParam, LPARAM LParam)
{
	ICallbackParameters * command = (ICallbackParameters *)WParam;
	if (!command)
	{
		return S_FALSE;
	}

	SIP_EVENT et = command->getEventType();

	switch (command->getEventType())
	{
	case SIP_PRESENCE_RECV_SUBSCRIBE:
	case SIP_PRESENCE_ONLINE:
	case SIP_PRESENCE_OFFLINE:
	case SIP_RECV_OUTOFDIALOG_MESSAGE:
	case SIP_SEND_OUTOFDIALOG_MESSAGE_FAILURE:
	case SIP_SEND_OUTOFDIALOG_MESSAGE_SUCCESS:
		OnSIPIMCallbackEvent(command);
		break;
	case SIP_REGISTER_SUCCESS:
		LOG4_INFO("Sip Register Success");
		//TODO LOAD INFO
		//LoadContacts();
		//OnRadCallPhones();

		//GetDlgItem(IDC_BTN_STATUSLIST)->SetWindowText(g_languageText.MenuStatus.M16000);
		//m_wndContacts->onSIPCallbackEvent(command);
		ProcessLoginReslut(true, "");
		break;
	case SIP_REGISTER_FAILURE:
		CSystemEx::GetInstance()->SipUnRegister();
		CSystemEx::GetInstance()->SetRegister(false);
		LOG4_INFO("Sip Register Failure");
	
		ResetOnLineStatus(FALSE);
		ReConnect2Server();
		break;
	case SIP_INVITE_INCOMING:
		onSIPCallIncoming(command);
		break;
	case SIP_INVITE_TRYING:
		onSIPCallTrying(command);
		break;

	case SIP_INVITE_SESSION_PROGRESS:
		onSIPCallSessionProgress(command);
		break;
	case SIP_INVITE_RINGING:
		onSIPCallRinging(command);
		break;

	case SIP_INVITE_ANSWERED:
		onSIPCallAnswered(command);
		break; 
	case SIP_INVITE_FAILURE:
		onSIPCallFailure(command); 
		break;
	case SIP_INVITE_CLOSED:
		onSIPCallClosed(command);
		break;;
	case  SIP_REMOTE_HOLD:
		onSIPCallRemoteHold(command);
		break;
	case  SIP_REMOTE_UNHOLD:
		onSIPCallRemoteUnhold(command);
		break;
	case SIP_ACTV_TRANSFER_SUCCESS:
		onSIPACTVTransferSuccess(command);
		break;

	case SIP_ACTV_TRANSFER_FAILURE:
		onSIPACTVTrasferFailure(command);
		break;
	}

	PortSIP_delCallbackParameters(command);
	command = NULL;

	return S_OK;

}
void CMainWnd::onSIPCallIncoming(ICallbackParameters * parameters)
{
	const char* caller = parameters->getCaller();
	stringstream log;
	log << "Incoming call from " << caller ;
	LOG4_INFO(log.str().c_str());
	int nSessionID = parameters->getSessionId();
	const char* sender = parameters->getCaller();
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormat(sender);
	int nActiveLine = CSystemEx::GetInstance()->ProcessComingCall(parameters, strSipNumber);
	if (nActiveLine==-1)
	{
		return;
	}

	bool bIsConference = false;
	const char * templateMsg = parameters->getSignaling();
	std::string strHeaderValue = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_CONFENRENCE, templateMsg);
	if (!strHeaderValue.empty())
	{
		int nPos = strHeaderValue.find("true");
		if (nPos>0&&nPos<strHeaderValue.length())
		{
			bIsConference = true;
		}
		else
		{
			bIsConference = false;
		}
	}


	
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
	

	const char *szDisPlayerNameCaller = parameters->getCallerDisplayName();
	CIUIString strDisPlayerName = PortUtility::Utf82Unicode_ND(szDisPlayerNameCaller).c_str();
	CContactDB *pContact = NULL;
	if (pContactInfo == NULL)
	{
		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSipNumber);
	}
	pContact = pContactInfo->m_unionContactInfo.m_pContact;
	//bool bRet = CSystemEx::GetInstance()->GetDBOprate()->addOneContact(pContact);

	//if (bRet)
	{//refresh 
		//Add2Contact(pContactInfo);
	}
	pContactInfo->m_unionContactInfo.m_pContact;
	if (!strDisPlayerName.IsEmpty())
	{
		pContact->SetContactDisplayName(strDisPlayerName);
	}

	CSystemEx::GetInstance()->GetCallerBarWndMGR().CreateCallerBarWnd(parameters, m_hWnd, nActiveLine,bIsConference);

	
}

void CMainWnd::onSIPCallTrying(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		return;
	}

	stringstream text;
	text << "Call is trying on line " << index << "";
	LOG4_INFO(text.str().c_str());
}

void CMainWnd::onSIPCallSessionProgress(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		return;
	}

	if (parameters->getExistsEarlyMedia())
	{
		// Checking does this call has video
		if (parameters->getExistsVideo())
		{
			// This incoming call has video
			// If more than one codecs using, then they are separated with "#",
			// for example: "g.729#GSM#AMR", "H264#H263", you have to parse them by yourself.
			const char* videoCodecs = parameters->getVideoCodecs();
		}

		if (parameters->getExistsAudio())
		{
			// If more than one codecs using, then they are separated with "#",
			// for example: "g.729#GSM#AMR", "H264#H263", you have to parse them by yourself.
			const char* audioCodecs = parameters->getAudioCodecs();
		}
	}

	stringstream log;
	log << "Call session progress on line  " << index << "";
	LOG4_INFO(log.str().c_str());
}

void CMainWnd::onSIPCallRinging(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		return;
	}

	/*if (!m_SessionArray[index].getExistEarlyMedia())
	{
		// No early media, you must play the local WAVE file for ringing tone
	}*/

	stringstream log;
	log << "Call ringing on line " << index << "";
	LOG4_INFO(log.str().c_str());
}

void CMainWnd::onSIPCallAnswered(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		return;
	}
	const char* caller = parameters->getCallee();
	string FromContact;
	if (!strstr(caller, "sip:"))
	{
		FromContact = "sip:";
	}
	FromContact += caller;

	// Get the display name
	const char* displayName = parameters->getCallerDisplayName();

	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormat(caller);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	// If more than one codecs using, then they are separated with "#",
	// for example: "g.729#GSM#AMR", "H264#H263", you have to parse them by yourself.
	// Checking does this call has video
	if (parameters->getExistsVideo())
	{
		const char* videoCodecs = parameters->getVideoCodecs();
	}

	if (parameters->getExistsAudio())
	{
		// Audio codecs name
		const char* audioCodecs = parameters->getAudioCodecs();
	}

	bool bIsConference = false;
	const char * templateMsg = parameters->getSignaling();
	std::string strHeaderValue = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_CONFENRENCE, templateMsg);
	if (!strHeaderValue.empty())
	{
		int nPos = strHeaderValue.find("true");
		if (nPos > 0 && nPos < strHeaderValue.length())
		{
			bIsConference = true;
		}
		else
		{
			bIsConference = false;
		}
	}
	//PostMessage(WM_ANSWER_BY_REMOTE, (WPARAM)index, (LPARAM)bIsConference);
	CSystemEx::GetInstance()->AnswerByRemote(index, bIsConference);
	//
	TAG_ASYNC_SIP_CONFERENCE_PARAM *pSipConferenceParam = new TAG_ASYNC_SIP_CONFERENCE_PARAM;
	pSipConferenceParam->m_strSipNumber = strSipNumber;
	pSipConferenceParam->m_nStartTime = CBaseThread::GetSystemTime();
	PostMessage(WM_SET_IS_CONFERENCE,(WPARAM) pSipConferenceParam, NULL);
	
	if (bIsConference)
	{
		TAG_RECENT *pContactInfo = NULL;
		pContactInfo = FindContactDBBySipNumber(strSipNumber);
		if (pContactInfo)
		{
			CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
			if (pChatTabControlHandler)
			{
				pChatTabControlHandler->SetIsConference(bIsConference);
			}
		}
	}
}

void CMainWnd::onSIPCallFailure(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}
	int statusCode = parameters->getStatusCode();
	const char* statusText = parameters->getStatusText();

	stringstream log;
	log << "Failed to call on line " << index << ": " << statusText << " " << statusCode << "";
	LOG4_INFO(log.str().c_str());
	CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"), PortUtility::string2WString(log.str()).c_str(), GetSafeHwnd());
 	CSystemEx::GetInstance()->HangupByRemote(index,false,false);
}

void CMainWnd::onSIPInviteUpdated(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	// Checking does this call has video
	if (parameters->getExistsVideo())
	{
		const char* videoCodecs = parameters->getVideoCodecs();
	}
	if (parameters->getExistsAudio())
	{
		const char* audioCodecs = parameters->getAudioCodecs();
	}

	stringstream log;
	log << "The call has been updated on line " << index << "";
	LOG4_INFO(log.str().c_str());
}
void CMainWnd::onSIPInviteConnected(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}
	stringstream log;
	log << "The call is connected on line " << index << "";
	LOG4_INFO(log.str().c_str());
}
void CMainWnd::onSIPCallForwarding(ICallbackParameters * parameters)
{
	const char* value = parameters->getForwardTo();

	stringstream text;
	text << "Call has been forward to: " << value;
	LOG4_INFO(text.str().c_str());
}

void CMainWnd::onSIPCallClosed(ICallbackParameters * parameters)
{
	bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	HWND hMainDlg = CSystemEx::GetInstance()->GetMainDlg()->m_hWnd;
	if (bMinSize)
	{
		SetForegroundWindow(hMainDlg);
		::SendMessage(hMainDlg, WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else
	{
		if (bVisible == FALSE)
		{
			::ShowWindow(hMainDlg, SW_SHOW);
		}
	}
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}
	stringstream log;
	log << "Call closed by remote on line " << index << "";
	CSystemEx::GetInstance()->HangupByRemote(index);
	
	LOG4_INFO(log.str().c_str());
}

void CMainWnd::onSIPDialogStateUpdated(ICallbackParameters * parameters)
{
	stringstream log;

	const char * uri = parameters->getBLFMonitoredUri();
	const char * state = parameters->getBLFDialogState();
	const char * dialogId = parameters->getBLFDialogId();
	const char * direction = parameters->getBLFDialogDirection();

	log << "The user: " << uri << " dialog state is updated: " << state << ", dialog id: " << dialogId << ", direction: " << direction;
	LOG4_INFO(log.str().c_str());
}
void CMainWnd::onSIPCallRemoteHold(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}
	
	stringstream log;
	log << "Placed on hold by remote on line " << index << "";
	LOG4_INFO(log.str().c_str());

	CSystemEx::GetInstance()->HoldByRemote(index);
}
void CMainWnd::onSIPCallRemoteUnhold(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance() ->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "Take off hold by remote on line " << index << "";
	LOG4_INFO(log.str().c_str());

	CSystemEx::GetInstance()->UnHoldByRemote(index);
}
void CMainWnd::onSIPReferAccepted(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "Line " << index << ", the REFER was accepted.";
	LOG4_INFO(log.str().c_str());
}


void CMainWnd::onSIPReferRejected(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "LINE " << index << ", the REFER was rejected.";
	LOG4_INFO(log.str().c_str());
}



void CMainWnd::onSIPTransferTrying(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	const char* referTo = parameters->getReferTo();

	stringstream log;
	log << "Transfer trying on line " << index << "";
	LOG4_INFO(log.str().c_str());
}


void CMainWnd::onSIPTransferRinging(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "Transfer ringing on line " << index << "";
	LOG4_INFO(log.str().c_str());
}


void CMainWnd::onSIPACTVTransferSuccess(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "Transfer succeeded on line " << index << ", close the call";
	LOG4_INFO(log.str().c_str());
	CSystemEx::GetInstance()->ProcessTransfer(index,true);
	CSystemEx::GetInstance()->HangupByRemote(index);
}

void CMainWnd::onSIPACTVTrasferFailure(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	log << "Failed to transfer on line " << index << "";
	LOG4_INFO(log.str().c_str());

	const char* reason = parameters->getStatusText();
	int code = parameters->getStatusCode(); // error code
	CSystemEx::GetInstance()->ProcessTransfer(index,false, code);
}



void CMainWnd::onSIPReceivedDtmfTone(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	stringstream log;
	int tone = parameters->getDTMFTone();

	log << "Received DTMF tone: ";
	if (tone == 10)
	{
		log << "*";
	}
	else if (tone == 11)
	{
		log << "#";
	}
	else if (tone == 12)
	{
		log << "A";
	}
	else if (tone == 13)
	{
		log << "B";
	}
	else if (tone == 14)
	{
		log << "C";
	}
	else if (tone == 15)
	{
		log << "D";
	}
	else if (tone == 16)
	{
		log << "FLASH";
	}
	else
	{
		log << tone;
	}

	log << " on line " << index;
	LOG4_INFO(log.str().c_str());
	
}



void CMainWnd::onSIPRecvOptions(ICallbackParameters * parameters)
{
	const char* data = parameters->getSignaling();

	stringstream Text;
	Text << "Received an OPTIONS message: ";
	Text << data;
}

void CMainWnd::onSIPRecvInfo(ICallbackParameters * parameters)
{
	int index = CSystemEx::GetInstance()->FindSessionBySessionID(parameters->getSessionId());
	if (index == -1)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "can not find seesion " << parameters->getSessionId();
		LOG4_INFO(strErrorInfo.str().c_str());
		return;
	}

	const char* data = parameters->getSignaling();

	stringstream Text;
	Text << "Received a INFO message on line " << index << ": ";
	Text << data;

	CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("Info"),PortUtility::string2WString(Text.str()).c_str(), GetSafeHwnd());
}


void CMainWnd::onSIPRecvNotifyOfSubscription(ICallbackParameters * parameter)
{
	const char * sipMsg = parameter->getSignaling();
	const unsigned char * contentData = parameter->getMessageData();
	int contentDataSize = parameter->getMessageDataLength();
}


void CMainWnd::onSIPSubscriptionFailure(ICallbackParameters * parameter)
{
	stringstream Text;
	Text << "Failed to send the SUBSCRIBE id = " << parameter->getSubscribeId();
}

void CMainWnd::onSIPSubscriptionTerminated(ICallbackParameters * parameter)
{
	stringstream Text;
	Text << "The subscription has been terminated: id = " << parameter->getSubscribeId();

}


void CMainWnd::OnSIPIMCallbackEvent(ICallbackParameters * parameters)
{
	if (!parameters)
	{
		return ;
	}
	switch (parameters->getEventType())
	{
	case SIP_PRESENCE_OFFLINE:
		OnPresenceOffline(parameters);
		break;

	case SIP_PRESENCE_ONLINE:
		OnPresenceOnline(parameters);
		break;


	case SIP_PRESENCE_RECV_SUBSCRIBE:
		OnPresenceRecvSubscribe(parameters);
		break;


	case SIP_RECV_OUTOFDIALOG_MESSAGE:
		OnRecvOutOfDialogMessage(parameters);
		break;


	case SIP_SEND_OUTOFDIALOG_MESSAGE_FAILURE:
		OnSendOutOfDialogMessageFailure(parameters);
		break;

	case SIP_SEND_OUTOFDIALOG_MESSAGE_SUCCESS:
		OnSendOutOfDialogMessageSuccess(parameters);
		break;
	}
}

void CMainWnd::OnPresenceOffline(ICallbackParameters * parameters)
{
	const char* caller = parameters->getCaller();
	string FromContact;
	if (!strstr(caller, "sip:"))
	{
		FromContact = "sip:";
	}
	FromContact += caller;
	const char* statusText = parameters->getStatusText();
	// Get the display name
	const char* displayName = parameters->getCallerDisplayName();
	
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormat(caller);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
	CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
	if (pContactInfo == NULL)
	{
	}
	else
	{
		pContact->m_eOnlineState = ONLINESTATE_OFFLINE;
	}
	if (pContact->GetLastOnlineState()!=pContact->GetOnlineState())
	{
		bool bUpdate = false;
		RefreshRecentContantOnlineStatus(pContact, bUpdate);
		if (!bUpdate)
		{
			RefreshLocalContactOnlineStatus(pContact);
		}
		pContact->SaveLastOnlineStateByNowState();
	}

}
void CMainWnd::OnPresenceOnline(ICallbackParameters * parameters)
{
	const char* caller = parameters->getCaller();

	string FromContact;
	if (!strstr(caller, "sip:"))
	{
		FromContact = "sip:";
	}
	FromContact += caller;


	const char* statusText = parameters->getStatusText();
	const unsigned char* strMsg = parameters->getMessageData();
	const char* strMessBody = parameters->getSignaling();

	
	//LOG4_FATAL(strStatusText.c_str());
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormat(caller);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	
	// Get the display name
	const char *szDisPlayerNameCaller = parameters->getCallerDisplayName();
	CIUIString strDisPlayerName = PortUtility::Utf82Unicode_ND(szDisPlayerNameCaller).c_str();
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
	CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
	if (pContact == NULL)
	{
	}
	else
	{
		pContact->m_eOnlineState = ONLINESTATE_ONLINE;
		pContact->SetOnlineState(ONLINESTATE_ONLINE);
		if (!strDisPlayerName.IsEmpty())
		{
			pContact->SetContactDisplayName(strDisPlayerName);
		}
	}
	//判断是否要刷新左边的树以及联系人列表
	//发生状态变化
	if (pContact->GetLastOnlineState() != pContact->GetOnlineState())
	{
		bool bUpdate = false;
		 RefreshRecentContantOnlineStatus(pContact,bUpdate);
		 if (!bUpdate)
		 {
			 RefreshLocalContactOnlineStatus(pContact);
		 }
		
		 pContact->SaveLastOnlineStateByNowState();
	}

	std::string strStatusText;
	strStatusText.append(statusText);
	if (strStatusText.empty())
	{
		return;
	}
	ProcessIsConferenceConfStatus(pContactInfo, strStatusText);
}

void CMainWnd::ProcessIsConferenceConfStatus(TAG_RECENT *pContactInfo, std::string strStatusText)
{
	CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);

	if (pChatTabControlHandler)
	{
		pChatTabControlHandler->PaticipantsUpdate(strStatusText);
	}
	else
	{
		return;
		SwitchToContact(pContactInfo, CT_TEXT_CHAT);
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
		if (pChatTabControlHandler != NULL)
		{
			pChatTabControlHandler->PaticipantsUpdate(strStatusText);
		}

	}
}
void CMainWnd::OnPresenceRecvSubscribe(ICallbackParameters * parameters)
{
	const char* caller = parameters->getCaller();

	string FromContact;
	if (!strstr(caller, "sip:"))
	{
		FromContact = "sip:";
	}
	FromContact += caller;
	// Get display name
	const char* displayName = parameters->getCallerDisplayName();
	string from = displayName;
	from += " <";
	from += FromContact;
	from += ">";

	if (CSystemEx::GetInstance()->GetGlobalUser()->m_nPresencMode != 0)
	{
		
	}
	else
	{
		PortSIP_presenceAcceptSubscribe(CSystemEx::GetInstance()->GetSipLib(), parameters->getSubscribeId());
		PortSIP_setPresenceStatus(CSystemEx::GetInstance()->GetSipLib(), 0, "Available");

		//long id = ContactsProfile.GetSubscribeId(FromContact);
		//ContactsProfile.SetSubscribeId(FromContact, parameters->getSubscribeId());
		//if (ContactsProfile.GetSubscribed(FromContact) == true && id <= 0)
		{
		// The contact subscribed me, accept it and also subscribe the contact's presence
			PortSIP_presenceSubscribe(CSystemEx::GetInstance()->GetSipLib(), FromContact.c_str(), "Hello");
		}
		return;
	}
}
void CMainWnd::OnRecvOutOfDialogMessage(ICallbackParameters * parameters)
{
	const char* mimeType = parameters->getMimeType();
	const char* subMimeType = parameters->getSubMimeType();

	const char* sender = parameters->getCaller();

	// Get display name
	const char* displayName = parameters->getCallerDisplayName();
	CIUIString strDisPlayerName = PortUtility::Utf82Unicode_ND(displayName).c_str();
	const unsigned char * data = parameters->getMessageData();
	int len = parameters->getMessageDataLength();
	
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormatNoSip(sender);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	char szHeaderValue[MAX_PATH] = { 0 };

	const char * templateMsg = parameters->getSignaling();
	std::string strHeaderValue = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_MSG_ID, templateMsg);
	if (strHeaderValue.empty())
	{
		strHeaderValue = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_MSG_ID_2, templateMsg);
	}


	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
	if (pContactInfo == NULL)
	{
		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSipNumber);
		//bool bRet = CSystemEx::GetInstance()->GetDBOprate()->addOneContact(pContact);
		//if (bRet)
		{// refresh 
			//Add2Contact(pContactInfo);
		}

	}
	
	if (!strDisPlayerName.IsEmpty()&& pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
	{
		pContactInfo->m_unionContactInfo.m_pContact->SetContactDisplayName(strDisPlayerName);
	}

	time_t timep;
	time(&timep);
	pContactInfo->m_unionContactInfo.m_pContact->m_tRecentChat = timep;
	
	std::string strMsg;
	strMsg.append(data, data + len);
	std::string strTime;

	//TODO FORMAT TIME
	strTime = "2019-05-29 08:04:05";
	bool bFind = false;
	
	CChatRecord theUnReadMsg;
	if (strstr(mimeType, "text") && strstr(subMimeType, "plain"))
	{
		stringstream Text;
		Text << "Received a MESSAGE message out of dialog: ";
		Text << strMsg;
		ProcessPureMsg(strTime, strMsg, strSipNumber, theUnReadMsg, strHeaderValue, strDisPlayerName);
	}
	else if (strstr(mimeType, "application") && (strstr(subMimeType, "vnd.3gpp.sms") || strstr(subMimeType, "vnd.3gpp2.sms")))
	{
		stringstream Text;
		Text << "Received a binary MESSAGE message out of dialog.";
		ProcessPureMsg(strTime, strMsg, strSipNumber, theUnReadMsg, strHeaderValue, strDisPlayerName);
	}
	else if (strstr(mimeType, "application") && strstr(subMimeType, "json"))
	{
		Json::Reader json_reader(Json::Features::strictMode());
		Json::Value json_root;
		if (!json_reader.parse(strMsg, json_root))
		{
			LOG4_ERROR("application json parse error \n");
			return;
		}
		ProcessJsonMsg(strTime,strMsg,strSipNumber,json_root, theUnReadMsg, strHeaderValue, strDisPlayerName);
	}
	
	if (theUnReadMsg.m_nMSGType != ENUM_MSG_SHAKE)
	{
		RefreshLocalContactUnReadNum(strSipNumber, theUnReadMsg);
	}
#if _DEBUG
	CIUIString strUnicode = PortUtility::Utf82Unicode_ND(strMsg).c_str();
	OutputDebugString(L"\n");
	OutputDebugString(strUnicode);
	OutputDebugString(L"\n");
#endif
	CSystemEx::GetInstance()->GetMainCanvasHandler()->ReListItem();
	FlashInTaskBar();
}
void CMainWnd::OnSendOutOfDialogMessageFailure(ICallbackParameters * parameters)
{
	const char* mimeType = parameters->getMimeType();
	const char* subMimeType = parameters->getSubMimeType();

	const char* recver = parameters->getCallee();
	long lLocalSignMsgID = parameters->getMessageId();
	int nTest = parameters->getStatusCode();
	std::string strErrorInfo = CSystemEx::GetInstance()->GetSipErrorInfo(parameters->getSessionId());
	if (lLocalSignMsgID >0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateStatusBySignMsgID(ENUM_MSG_STATUS_SENDSUCC, lLocalSignMsgID);
	}
	else
	{
		LOG4_INFO(_T("SIGN MSG ID IS LESS ZERO\n"));
	}
	

	const char * templateMsg = parameters->getSignaling();
	std::string strSignMsgID = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_SENDER_MSG_ID, templateMsg);
	if (!strSignMsgID.empty()&& lLocalSignMsgID > 0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateServerMSGIDBySignMsgID(lLocalSignMsgID, strSignMsgID);
	}
	else
	{
		LOG4_INFO(_T("SERVER SIGN MSG ID IS Empty\n"));
	}

	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormatNoSip(recver);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}

	TAG_RECENT *pContactInfo = NULL;
	pContactInfo = FindContactDBBySipNumber(strSipNumber);

	if (pContactInfo != NULL)
	{
		RefreshMsgStatus(pContactInfo, lLocalSignMsgID,false);
	}
}
void CMainWnd::OnSendOutOfDialogMessageSuccess(ICallbackParameters * parameters)
{
	const char* mimeType = parameters->getMimeType();
	const char* subMimeType = parameters->getSubMimeType();
	const char* recver = parameters->getCallee();
	long lSignMsgID = parameters->getMessageId();
	if (lSignMsgID > 0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateStatusBySignMsgID(ENUM_MSG_STATUS_SENDSUCC, lSignMsgID);
	}
	else
	{
		LOG4_INFO(_T("SIGN MSG ID IS LESS ZERO\n"));
	}
	std::string strSipNumber = PortUtility::GetSipNumberBySenderFormatNoSip(recver);
	if (strSipNumber.empty())
	{
		std::string strErrorInfo = "Can't find Sip Num In recv MSG";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	const char * templateMsg = parameters->getSignaling();
	std::string strSignMsgID = CSystemEx::GetInstance()->GetSipHeaderValue(SIP_HEADER_SENDER_MSG_ID, templateMsg);
	if (!strSignMsgID.empty() && lSignMsgID > 0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateServerMSGIDBySignMsgID(lSignMsgID, strSignMsgID);
	}
	else
	{
		LOG4_INFO(_T("SERVER SIGN MSG ID IS Empty\n"));
	}
	TAG_RECENT *pContactInfo = NULL;
	pContactInfo = FindContactDBBySipNumber(strSipNumber);

	if (pContactInfo != NULL)
	{
		RefreshMsgStatus(pContactInfo, lSignMsgID, true);
	}
}


LRESULT CMainWnd::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT)wParam;
	UINT message = (UINT)lParam;
	if (WM_RBUTTONUP == message)
	{
		switch (uID)
		{
		case IDI_BZXX_PG:
		case IDI_KCXX_PG:
		{
			::SetForegroundWindow(m_hWnd);
			POINT pt;
			GetCursorPos(&pt);

			HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TRAY));
			HMENU hMoreMenu = GetSubMenu(hMenu, 0);
			_ASSERT(NULL != hMoreMenu);

			ModifyMenu(hMoreMenu,0 ,MF_BYPOSITION | MF_STRING, ID_TRAYMENU_SHOWMAINWINDOW, CSystemEx::GetInstance()->GetShowMainText());
			ModifyMenu(hMoreMenu, 1, MF_BYPOSITION | MF_STRING, ID_TRAYMENU_ABOUT, CSystemEx::GetInstance()->GetAboutText());
			ModifyMenu(hMoreMenu, 2, MF_BYPOSITION | MF_STRING, ID_TRAYMENU_EXIT, CSystemEx::GetInstance()->GetExitText());
			UINT uRet = ::TrackPopupMenu(hMoreMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
					pt.x, pt.y, 0, m_hWnd, NULL);

			if (ID_TRAYMENU_SHOWMAINWINDOW == uRet)
			{
				CSystemEx::GetInstance()->GetMainCanvasHandler()->AutoCleanMsgOfTreeItem();
				::ShowWindow(m_hWnd, SW_SHOW);
			}
			else if (ID_TRAYMENU_ABOUT == uRet)
			{
			}
			else if (ID_TRAYMENU_EXIT == uRet)
			{

				NOTIFYICONDATA tnd;
				tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
				tnd.hWnd = m_hWnd;
				CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
#ifdef BZXX
				tnd.uID = IDI_BZXX_PG;
				tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
				StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif

#ifdef KCXX
				tnd.uID = IDI_KCXX_PG;
				tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
				StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif
				tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
				tnd.uCallbackMessage = WM_TRAYNOTIFY;
#ifdef BZXX
				tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
#endif

#ifdef KCXX
				tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
#endif

				Shell_NotifyIcon(NIM_DELETE, &tnd);
				//::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
				//CSystemEx::GetInstance()->TerminateAllThread();
				TerminateProcess(GetCurrentProcess(), 0);
			
				return 1;
			}

			::PostMessage(m_hWnd, WM_NULL, 0, 0);
		}
		break;
		default:
			break;
		}
	}
	else if (WM_LBUTTONDOWN == message)
	{
		m_bTrayLButtonDown = TRUE;	
	}
	else if (WM_LBUTTONUP==message)
	{
		bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		if (bVisible)
		{
			//::SendMessage(m_hWnd, WM_RESTORE_MAIN_DLG, 0, 0);
			//::ShowWindow(m_hWnd, SW_SHOW);
			SetForegroundWindow(m_hWnd);
			//::ShowWindow(m_hWnd, SW_SHOW);
			SendMessage(WM_SYSCOMMAND, SC_RESTORE);
		}
		else
		{
			if (bVisible == FALSE)
			{
				::ShowWindow(m_hWnd, SW_SHOW);
			}
		}
		CSystemEx::GetInstance()->GetMainCanvasHandler()->AutoCleanMsgOfTreeItem();
	}
	else if (WM_LBUTTONUP == message)
	{
		if (m_bTrayLButtonDown)
		{
			BOOL bVisible = ::IsWindowVisible(m_hWnd);
			switch (uID)
			{
			case IDI_BZXX_PG:
			case IDI_KCXX_PG:
				if (bVisible)
				{
					::ShowWindow(m_hWnd, SW_HIDE);
				}
				else
				{
					::ShowWindow(m_hWnd, SW_SHOWNORMAL);
				}
				break;

			default:
				break;
			}
		}

		m_bTrayLButtonDown = FALSE;
	}

	return 0;
}

/*
int CMainWnd::FindIDBySipNumber(std::string strSipNumber, bool& bFind)
{
	bFind = false;
	int nContactID = 0;
	m_oLockContact.Lock();
	std::vector<CContactDB*>::iterator iterContact = m_vecContact.begin();
	for (iterContact; iterContact < m_vecContact.end(); iterContact++)
	{
		CContactDB* pContactDB = *iterContact;
		if (pContactDB)
		{
			if (PortUtility::compareStringNoCase(strSipNumber,pContactDB->m_strSIPNum)==0)
			{
				bFind = true;
				nContactID= pContactDB->m_nContactID;
				break;
			}
		}
	}
	m_oLockContact.UnLock();
	return nContactID;
}
*/
void CMainWnd::InitFavorite()
{
	vector<TAG_RECENT *> vecContactDB;
	auto iterContact = m_vecContact.begin();
	for (iterContact; iterContact != m_vecContact.end(); iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			if (pFindContactInfo->m_unionContactInfo.m_pContact->m_bFavorites)
			{
				m_vecFavoriteContact.push_back(pFindContactInfo);
			}
		}
	}
}
vector<TAG_RECENT*> CMainWnd::GetPureContact()
{
	vector<TAG_RECENT *> vecContactDB;
	auto iterContact = m_vecContact.begin();
	for (iterContact; iterContact!=m_vecContact.end();iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			vecContactDB.push_back(pFindContactInfo);
		}
	}
	return vecContactDB;
}
vector<TAG_RECENT*> CMainWnd::GetDBContact()
{
	vector<TAG_RECENT *> vecContactDB;
	auto iterContact = m_vecDBContact.begin();
	for (iterContact; iterContact != m_vecDBContact.end(); iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			vecContactDB.push_back(pFindContactInfo);
		}
	}
	return vecContactDB;
}
TAG_RECENT * CMainWnd::FindTagRecentByContact(CContactDB *pContactDB)
{
	return FindContactDBBySipNumber(pContactDB->m_strSIPNum);
}
TAG_RECENT * CMainWnd::FindContactDBBySipNumber(std::string strSipNumber)
{
	bool bFind = false;
	TAG_RECENT * pFindContactInfo = NULL;
	m_oLockContact.Lock();
	std::vector<TAG_RECENT*>::iterator iterContact = m_vecContact.begin();
	for (iterContact; iterContact < m_vecContact.end(); iterContact++)
	{
		pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = pFindContactInfo->m_unionContactInfo.m_pContact;
			if (PortUtility::compareStringNoCase(strSipNumber, pFindContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
			{
				bFind = true;
				break;
			}
		}
		if (bFind)
		{
			break;
		}
		else
		{
			pFindContactInfo = NULL;
		}
	}
	if (bFind == false)
	{

		iterContact = m_vecExtersionContact.begin();
		for (iterContact; iterContact < m_vecExtersionContact.end(); iterContact++)
		{
			pFindContactInfo = *iterContact;
			ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
			if (localRecentContactType == ENUM_SINGLE_CONTACT)
			{
				CContactDB *pContactDB = pFindContactInfo->m_unionContactInfo.m_pContact;
				if (PortUtility::compareStringNoCase(strSipNumber, pFindContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
				{
					bFind = true;
					break;
				}
			}
			if (bFind)
			{
				break;
			}
			else
			{
				pFindContactInfo = NULL;
			}
		}
	}
	m_oLockContact.UnLock();
	return pFindContactInfo;
}
TAG_RECENT * CMainWnd::FindContactDBByVecSipNumber(vector<CContactDB *> vecSipNumber)
{
	bool bFind = false;
	TAG_RECENT * pFindContactInfo = NULL;
	m_oLockContact.Lock();
	std::vector<TAG_RECENT*>::iterator iterContact = m_vecContact.begin();
	for (iterContact; iterContact < m_vecContact.end(); iterContact++)
	{
		pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_MULTI_CONTACT)
		{
		}
		if (bFind)
		{
			break;
		}
		else
		{
			pFindContactInfo = NULL;
		}
	}
	m_oLockContact.UnLock();
	return pFindContactInfo;
}
std::vector<TAG_RECENT*> CMainWnd::FindMemContactByVecSipInfo(vector<TagRecentContactInfo > vecContactInfo)
{
	std::vector<TAG_RECENT*> vecContactDB;
	if (vecContactInfo.size()==0)
	{
		return vecContactDB;
	}
	CContactDB *pFindContactDB = NULL;

	std::vector<TagRecentContactInfo>::iterator iterContactInfo = vecContactInfo.begin();
	TagRecentContactInfo stuRecentContactInfo;
	for (iterContactInfo; iterContactInfo < vecContactInfo.end(); iterContactInfo++)
	{
		stuRecentContactInfo = *iterContactInfo;
		bool  bFind = false;
		std::vector<TAG_RECENT*>::iterator iterContact = m_vecContact.begin();
		for (iterContact; iterContact < m_vecContact.end(); iterContact++)
		{
			TAG_RECENT* pContactInfo = *iterContact;
			if (pContactInfo)
			{
				CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
				std::string strTalker = stuRecentContactInfo.m_strTalkerURL;
				//std::string strDBRecverURL = stuRecentContactInfo.m_strContatctRecverURL;
				std::string strMemSipURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
			
				//TODO：today
				if (PortUtility::compareStringNoCase(strTalker, strMemSipURL) == 0\
					|| PortUtility::compareStringNoCase(strTalker, pContactDB->m_strSIPNum) == 0)
				{
					pFindContactDB = pContactDB;
					pContactDB->m_tRecentChat = PortUtility::StringToDatetime(stuRecentContactInfo.m_strDateTime.c_str());
					bFind = true;
					vecContactDB.push_back(pContactInfo);
					break;
				}
			}
		}
		if (bFind == false)
		{

			iterContact = m_vecExtersionContact.begin();
			for (iterContact; iterContact < m_vecExtersionContact.end(); iterContact++)
			{
				TAG_RECENT*  pFindContactInfo = *iterContact;
				if (pFindContactInfo)
				{
					CContactDB *pContactDB = pFindContactInfo->m_unionContactInfo.m_pContact;
					ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
					std::string strTalker = stuRecentContactInfo.m_strTalkerURL;
					std::string strMemSipURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
					if (localRecentContactType == ENUM_SINGLE_CONTACT)
					{
						if (PortUtility::compareStringNoCase(strTalker, pFindContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0\
							|| PortUtility::compareStringNoCase(strTalker, strMemSipURL) == 0)
						{
							bFind = true;
							pContactDB->m_tRecentChat = PortUtility::StringToDatetime(stuRecentContactInfo.m_strDateTime.c_str());
							vecContactDB.push_back(pFindContactInfo);
							break;
						}
					}
				}
			}
		}
		if (bFind == false)
		{
			TAG_RECENT* pContactInfo = new TAG_RECENT;
			pContactInfo->m_enumRecentContact = ENUM_SINGLE_CONTACT;
			CContactDB* pContactDB = new CContactDB;
			pContactInfo->m_unionContactInfo.m_pContact = pContactDB;
			
			int  nRand = rand() % 6;
			CIUIString strImage;
			//strImage.Format(_T("%sSkins\\PortGo\\%d.png"), GetRunFolder().GetBuffer(0), nRand);
			strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));
			std::wstring strImagePath = strImage;
			pContactDB->m_strHeadImage = PortUtility::Unicode2Utf8_ND(strImagePath);
			if (stuRecentContactInfo.m_bIsMyself)
			{
				pContactDB->m_strSIPNum =PortUtility::GetSipNumberBySenderFormatNoSip(stuRecentContactInfo.m_strTalkerURL);
				pContactDB->m_strLastName = PortUtility::GetSipNumberBySenderFormatNoSip(stuRecentContactInfo.m_strTalkerURL);
			}
			else
			{
				pContactDB->m_strSIPNum = PortUtility::GetSipNumberBySenderFormatNoSip(stuRecentContactInfo.m_strTalkerURL);
				pContactDB->m_strLastName = PortUtility::GetSipNumberBySenderFormatNoSip(stuRecentContactInfo.m_strTalkerURL);
			}
			pContactDB->m_tRecentChat = PortUtility::StringToDatetime(stuRecentContactInfo.m_strDateTime.c_str());
			pContactDB->m_nUserID = -1;
			vecContactDB.push_back(pContactInfo);
			m_vecContact.push_back(pContactInfo);
		}
	}
	return vecContactDB;
}
/*
CContactDB * CMainWnd::FindMemContactBySipNumber(std::string strSipNumber)
{
	CContactDB *pFindContactDB = NULL;
	std::vector<TAG_RECENT*>::iterator iterContact = m_vecContact.begin();
	for (iterContact; iterContact < m_vecContact.end(); iterContact++)
	{
		CContactDB* pContactDB = *iterContact;
		if (pContactDB)
		{
			if (PortUtility::compareStringNoCase(strSipNumber, pContactDB->m_strSIPNum) == 0)
			{
				pFindContactDB = pContactDB;
				break;
			}
		}
	}
	return pFindContactDB;
}*/

TAG_RECENT* CMainWnd::CreateNewContactBySipNumber(std::string strSipNumber)
{
	TAG_RECENT * pCreateNewContact = new TAG_RECENT();
	pCreateNewContact->m_enumRecentContact = ENUM_SINGLE_CONTACT;
	pCreateNewContact->m_unionContactInfo.m_pContact = new CContactDB;
	pCreateNewContact->m_unionContactInfo.m_pContact->m_strSIPNum = strSipNumber;
	int  nRand = rand() % 7;
	CIUIString strImage;
	//strImage.Format(_T("%sSkins\\PortGo\\%d.png"), GetRunFolder().GetBuffer(0), nRand);
	strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));
	std::wstring strImagePath = strImage;
	pCreateNewContact->m_unionContactInfo.m_pContact->m_strHeadImage = PortUtility::Unicode2Utf8_ND(strImagePath);
	
	pCreateNewContact->m_unionContactInfo.m_pContact->m_nUserID = -1;
	m_vecContact.push_back(pCreateNewContact);
	return pCreateNewContact;
}
TAG_RECENT * CMainWnd::CreateNewTalkGroupByVecSContatct(vector<CContactDB*> vecSipNumber)
{
	TAG_RECENT * pCreateNewContact = new TAG_RECENT();
	pCreateNewContact->m_enumRecentContact = ENUM_MULTI_CONTACT;
	pCreateNewContact->m_unionContactInfo.m_pChartGroup = new CTalkGroup;

	std::string strUTF8ChatGroupName, strMemberList, strUTF8DispalyName;
	int nIndex = 0;
	auto iterVecContact = vecSipNumber.begin();
	for (iterVecContact; iterVecContact != vecSipNumber.end(); iterVecContact++)
	{
		CContactDB* pContactDB = *iterVecContact;
		CTalkGroupNumber *pTalkGroupNumber = new CTalkGroupNumber;
		strMemberList += pContactDB->m_strSIPNum + ";";
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

		pTalkGroupNumber->m_strSIPNumber = pContactDB->m_strSIPNum ;
		pTalkGroupNumber->m_bFavorite = false;
		pTalkGroupNumber->m_bIsCreator = false;
		pTalkGroupNumber->m_bIsDisable = false;
		pTalkGroupNumber->m_bIsManager = false;
		pTalkGroupNumber->SetContactDB(pContactDB);
		pCreateNewContact->m_unionContactInfo.m_pChartGroup->AddContact2Group(pTalkGroupNumber);
		pTalkGroupNumber->m_strSelfDisplayName = PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
		if (nIndex < vecSipNumber.size()-1)
		{
			strUTF8ChatGroupName += PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength())) + ",";
		}
		else if (nIndex == vecSipNumber.size() - 1)
		{
			strUTF8ChatGroupName += PortUtility::Unicode2Utf8_ND(strSrcUserName.GetBuffer(strSrcUserName.GetLength()));
		}
		nIndex++;
	}
	if (vecSipNumber.size() > 2)
	{
		strUTF8ChatGroupName += "... ";
	}
	
	pCreateNewContact->m_unionContactInfo.m_pChartGroup->m_strChatGroupName = strUTF8ChatGroupName;
	pCreateNewContact->m_unionContactInfo.m_pChartGroup->m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	return pCreateNewContact;
}

HWND CMainWnd::GetHwndBySipNumber(std::string strSipNumber)
{
	bool nNeedCreate = false; HWND hWndFind = NULL;
	if (nNeedCreate ==true)
	{
		
	}
	return hWndFind;
}
HWND CMainWnd::FindHwndByContact(TAG_RECENT* pContactDB)
{
	HWND hWnd = NULL;
	
	if (m_pMainCanvasHandler!=NULL)
	{
		CChatTabControlHandler *pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactDB);
		if (pChatTabControlHandler==NULL)
		{
			return NULL;
		}
		else
		{
			 CVideoCanvasHandler  *pVideoCanvasHandler = pChatTabControlHandler->GetVideoCanvasHandler();
			 if (pVideoCanvasHandler)
			 {
				 hWnd =(HWND)pVideoCanvasHandler->GetBindWLWnd();
				 return hWnd;
			 }
			 else
			 {
				 return NULL;
			 }
		}
	}
}

LRESULT CMainWnd::OnProcessDownLoadFile(WPARAM wParam, LPARAM lParam)
{
	UpLoadDownLoadParam *pFileParam = (UpLoadDownLoadParam*)wParam;
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pFileParam->m_pContact);
		if (pChatTabControlHandler)
		{
			switch (pFileParam->m_enumFileType)
			{	
			case ENUM_MSG_FILE_PIC:
			{
				std::string strTime = "";
				pChatTabControlHandler->InsertMsgPic(pFileParam->m_strFileName, strTime, pFileParam->m_fProcessData,pFileParam->m_lMsgID);
			}
				break;
			case ENUM_MSG_FILE:
			{
				std::string strTime = "";
				pChatTabControlHandler->InsertMsgFile(pFileParam->m_strFileName, strTime, pFileParam->m_fProcessData, pFileParam->m_lMsgID);
			}
				break;
			case ENUM_MSG_EMOJI:
				break;
			case  ENUM_MSG_FILE_AUDIO:
			{
			
				std::string strTime = "";
				pChatTabControlHandler->InsertMsgAudio(pFileParam->m_strFileName, strTime, pFileParam->m_fProcessData, pFileParam->m_lMsgID);
			}
				break;
			case ENUM_MSG_FILE_VIDEO:
			{
				std::string strTime = "";
				pChatTabControlHandler->InsertMsgVideo(pFileParam->m_strFileName, strTime, pFileParam->m_fProcessData, pFileParam->m_lMsgID);
			}
				break;
			default:
				break;
			}
		}
		
	}
	//TODO pChatTabControlHandler->UpdateDownLoadFileProgress(&pFileParam);
	return 0;
}
LRESULT CMainWnd::OnProcessUpLoadFile(WPARAM wParam, LPARAM lParam)
{
	UpLoadDownLoadParam *pFileParam = (UpLoadDownLoadParam*)wParam;

	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pFileParam->m_pContact);
		if (pChatTabControlHandler)
		{
			
			std::string strTime = "";
			switch (pFileParam->m_enumFileType)
			{
			case ENUM_MSG_FILE_PIC:
			case ENUM_MSG_SCREEN_PIC:
				pChatTabControlHandler->UpdateUpLoadPicFileProgress(pFileParam);
				break;
			case ENUM_MSG_FILE:
				pChatTabControlHandler->UpdateUpLoadFileProgress(pFileParam);
				break;
			case ENUM_MSG_FILE_AUDIO:
				pChatTabControlHandler->UpdateUpLoadAudioFileProgress(pFileParam);
				break;
			case ENUM_MSG_FILE_VIDEO:
				pChatTabControlHandler->UpdateUpLoadVideoFileProgress(pFileParam);
				break;
			default:
				break;
			}
		}
	}
	return 0;
}
LRESULT CMainWnd::OnUploadFileGetOk(WPARAM wParam, LPARAM lParam)
{
	UpLoadDownLoadParam *pFileParam = (UpLoadDownLoadParam*)wParam;

	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pFileParam->m_pContact);
		if (pChatTabControlHandler)
		{
			
			std::string strTime = "";
			pChatTabControlHandler->OnUpdateFileOK(pFileParam);
		}
	}
	
	return 0;
}

LRESULT CMainWnd::OnDownLoadVoiceMailOK(WPARAM wParam, LPARAM lParam)
{
	VoiceMailDownLoadParam *pVoiceMailParam = (VoiceMailDownLoadParam*)wParam;
	CSystemEx::GetInstance()->GetDBOprate()->UpdateOneVoiceMailDownLoadSuccess(pVoiceMailParam->m_lID);
	if (m_pMainCanvasHandler)
	{
		CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
		if (pInboxCanvasHandler != NULL)
		{
			//CVoiceMail *pVoiceMail = CSystemEx::GetInstance()->GetDBOprate()->GetVoiceMailByID(pVoiceMailParam->m_lID);
			pInboxCanvasHandler->InitVoiceMail();
			//if (pVoiceMail)
			//{
				//pInboxCanvasHandler->Insert2VoiceMail(pVoiceMail);
			//}
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"Recv new voice mail", NULL);
		}
	}
	return 0;
}

void CMainWnd::ProcessJsonMsg(std::string strTime, std::string strMsg, std::string strSipNumber, Json::Value & json_data, CChatRecord &oChatRecord, std::string strMsgID,  CIUIString strDisPlayName)
{
	CContactDB *pContactDB = NULL;
	TAG_RECENT *pContactInfo = NULL;
	pContactInfo = FindContactDBBySipNumber(strSipNumber);
	pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
	const Json::Value *val;
	std::string strMsgType = "", strFileName = "",strMimeType = "",strHttpURL = "";
	val = json_data.find(key_G_MESSAGE_TYPE, key_G_MESSAGE_TYPE + strlen(key_G_MESSAGE_TYPE));
	INT64 nFileSize = 0;
	
	if (val && val->isString())
	{
		strMsgType = val->asString();
	}
	
	val = json_data.find(key_G_FILE_NAME, key_G_FILE_NAME + strlen(key_G_FILE_NAME));
	if (val && val->isString())
	{
		strFileName = val->asString();
	}
	
	val = json_data.find(key_G_FILE_MIME_TYPE, key_G_FILE_MIME_TYPE + strlen(key_G_FILE_MIME_TYPE));
	if (val && val->isString())
	{
		strMimeType = val->asString();
	}
	
	val = json_data.find(key_G_FILE_URL, key_G_FILE_URL + strlen(key_G_FILE_URL));
	if (val&&val->isString())
	{
		strHttpURL = val->asString();
	}
	val = json_data.find(key_G_FILE_SIZE, key_G_FILE_SIZE + strlen(key_G_FILE_SIZE));
	if (val&&val->isString())
	{
		nFileSize = val->asInt64();
	}
	 
	std::string strTrueFileName = "";
	CChatRecord theLocalRecord;
	ENUM_MSG_TYPE enumMsgType = PortUtility::GetMsgTypeByString(strMsgType);
	switch (enumMsgType)
	{
	case ENUM_MSG_TEXT:
		theLocalRecord.m_nMSGType = ENUM_MSG_TEXT;
		//ProcessMsgText(strTime, strMsg, pContactInfo);
		break;

	case ENUM_MSG_EMOJI:
		theLocalRecord.m_nMSGType = ENUM_MSG_EMOJI;
		ProcessMsgEmoji(strTime, strFileName, pContactInfo);
		break;
	case ENUM_MSG_OTHER:
		theLocalRecord.m_nMSGType = ENUM_MSG_OTHER;
		ProcessMsgOther(strTime, strFileName, pContactInfo);
		break;
	case ENUM_MSG_SHAKE:
	{
		theLocalRecord.m_nMSGType = ENUM_MSG_SHAKE;
		oChatRecord.m_nMSGType = ENUM_MSG_SHAKE;

		bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		if (bMinSize)
		{
			SetForegroundWindow(m_hWnd);
			SendMessage(WM_SYSCOMMAND, SC_RESTORE);
		}
		else
		{
			if (bVisible == FALSE)
			{
				::ShowWindow(m_hWnd, SW_SHOW);
			}
		}
		
		Sleep(1);
		ProcessShake();
		OutputDebugString(L"shake\n");
	}
		return;
	case ENUM_MSG_FILE_AUDIO:
		theLocalRecord.m_nMSGType = ENUM_MSG_FILE_AUDIO;
		break;
	case ENUM_MSG_FILE_VIDEO:
		theLocalRecord.m_nMSGType = ENUM_MSG_FILE_VIDEO;
		break;
	case ENUM_MSG_FILE_PIC:
		theLocalRecord.m_nMSGType = ENUM_MSG_FILE_PIC;
		break;
	case ENUM_MSG_FILE:
		theLocalRecord.m_nMSGType = ENUM_MSG_FILE;
		break;
	default:
		PrintJsonKeyValueError(strMsg);
		return;
	}
	if (theLocalRecord.m_nMSGType == ENUM_MSG_SHAKE)
	{
		return;
	}
	if ( pContactDB != NULL)
	{
		//InsertJsonFileMsg2Chat(pContactDB, strFileName, strMimeType);
		//InsertJsonOffLineMsg2Chat(pContactDB);
		//InsertMsg2Chat(pContactDB, strMsg, strTime);
	}// TODO:
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
		if (pChatTabControlHandler==NULL)
		{
			SwitchToContact(pContactInfo, CT_TEXT_CHAT);
		}
	}
	theLocalRecord.m_nIsMyself = false;
	theLocalRecord.m_nMSGStatus = ENUM_MSG_STATUS_UNREAD;
	theLocalRecord.m_strMSG = strMsg;
	theLocalRecord.m_strTalkerURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalRecord.m_strSignMsgID = strMsgID;
	theLocalRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->m_nUserID;
	oChatRecord = theLocalRecord;
	int nMsgID = CSystemEx::GetInstance()->GetDBOprate()->saveOneChatRecord(theLocalRecord);
	oChatRecord.m_nMsgID = nMsgID;

	switch (enumMsgType)
	{
	case ENUM_MSG_FILE_PIC:
		strTrueFileName = ProcessMsgPic(nMsgID, strTime, strFileName, strHttpURL, pContactInfo, nFileSize);
		break;
	case ENUM_MSG_FILE:
		strTrueFileName = ProcessMsgFile(nMsgID,strTime, strFileName, strHttpURL, pContactInfo, nFileSize);
		break;
	case ENUM_MSG_FILE_AUDIO:
		ProcessMsgAudio(nMsgID,strTime, strFileName, strHttpURL, pContactInfo, nFileSize);
		break;
	case ENUM_MSG_FILE_VIDEO:
		ProcessMsgVideo(nMsgID, strTime, strFileName, strHttpURL, pContactInfo, nFileSize);
		break;
	default:
		break;
	}
	return;
}
void CMainWnd::ShakeSelf()
{
	return ProcessShake();
}

void CMainWnd::ProcessPureMsg(std::string strTime, std::string strMsg, std::string strSipNumber, CChatRecord &oChatRecord,  std::string strMsgID, CIUIString strDisPlayName)
{
	TAG_RECENT *pContactInfo = NULL;
	pContactInfo = FindContactDBBySipNumber(strSipNumber);
	CChatRecord theLocalRecord;
	theLocalRecord.m_nIsMyself = false;
	theLocalRecord.m_nMSGStatus = ENUM_MSG_STATUS_UNREAD;
	theLocalRecord.m_strMSG = strMsg;
	theLocalRecord.m_strTalkerURL = strSipNumber + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalRecord.m_nMSGType = ENUM_MSG_TEXT;
	theLocalRecord.m_strSignMsgID = strMsgID;
	theLocalRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	oChatRecord = theLocalRecord;
	int nLocalMsgID = CSystemEx::GetInstance()->GetDBOprate()->saveOneChatRecord(theLocalRecord);
	oChatRecord.m_nMsgID = nLocalMsgID;

	if (pContactInfo != NULL)
	{
		InsertMsg2Chat(pContactInfo, strMsg, strTime, nLocalMsgID, strDisPlayName);
	}// TODO:

}

void CMainWnd::InsertCallEventMsg(std::string strSipNumber,CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode)
{
	TAG_RECENT *pContactInfo = NULL;
	pContactInfo = FindContactDBBySipNumber(strSipNumber);
	if (pContactInfo==NULL)
	{
		return;
	}
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertCallEventMsg(oChatRecord, enumInsertHistoryMode);
		}
	}
}
void  CMainWnd::ProcessMsgText(std::string strTime, std::string strMsg,  TAG_RECENT *pContact,int lMsgID)
{
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsg(strMsg, strTime, lMsgID);
		}
	}
}
std::string  CMainWnd::ProcessMsgAudio(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize)
{
	/*if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsgAudio(srFileName, strTime);
		}
	}*/
	UpLoadDownLoadParam *pUpLoadParam = new UpLoadDownLoadParam();
	std::string strTrueFileName = CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, srFileName, strHttpURL, ENUM_MSG_FILE_AUDIO, pContact, nFileSize, msgID);
	CSystemEx::GetInstance()->AddFole2DownLoadList(pUpLoadParam);

	return strTrueFileName;
}
string  CMainWnd::ProcessMsgVideo(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize)
{
	/*if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsgVideo(srFileName, strTime);
		}
	}*/
	UpLoadDownLoadParam *pUpLoadParam = new UpLoadDownLoadParam();
	std::string strTrueFileName = CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, srFileName, strHttpURL, ENUM_MSG_FILE_VIDEO, pContact, nFileSize, msgID);
	CSystemEx::GetInstance()->AddFole2DownLoadList(pUpLoadParam);
	return strTrueFileName;
}
string  CMainWnd::ProcessMsgPic(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize)
{
	UpLoadDownLoadParam *pUpLoadParam = new UpLoadDownLoadParam();
	std::string strTrueFileName = CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, srFileName, strHttpURL, ENUM_MSG_FILE_PIC, pContact, nFileSize, msgID);
	CSystemEx::GetInstance()->AddFole2DownLoadList(pUpLoadParam);
	return strTrueFileName;
}
void CMainWnd::UpdateContactInfo(WPARAM wParam, LPARAM lParam)
{
	TAG_RECENT *pContactInfo = (TAG_RECENT*)wParam;
	if (pContactInfo==NULL)
	{
		return;
	}
	CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContactInfo);
	if (pChatTabControlHandler != NULL)
	{
		pChatTabControlHandler->UpdateContactInfo();
	}
}
void CMainWnd::ProcessDownLoadFileOK(WPARAM wParam, LPARAM lParam)
{
	UpLoadDownLoadParam *pFileParam = (UpLoadDownLoadParam*)wParam;
	if (pFileParam==NULL)
	{
		return;
	}

	std::string  strFilePath = pFileParam->m_strFilePath;
	std::string  strFileName = pFileParam->m_strFileName;
	std::string strTemplatePath = strFilePath + "//" + strFileName;
	std::wstring wstrTemlatePath = PortUtility::string2WString(strTemplatePath);
	ULARGE_INTEGER uliFileSize;
	uliFileSize.LowPart = GetCompressedFileSize(wstrTemlatePath.c_str(),
		&uliFileSize.HighPart);
	bool bComplete = false;
	if (pFileParam->m_lFileSize==0&& uliFileSize.LowPart != 0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateRecordFileStatus(pFileParam->m_lMsgID, ENUM_OK);
		bComplete = true;
	}
	else if (uliFileSize.LowPart ==pFileParam->m_lFileSize&&pFileParam->m_lFileSize !=0)
	{
		CSystemEx::GetInstance()->GetDBOprate()->UpdateRecordFileStatus(pFileParam->m_lMsgID, ENUM_OK);
		bComplete = true;
	}
	else
	{
		PortUtility::removeFile(strTemplatePath);
		bComplete = false;
		CSystemEx::GetInstance()->GetDBOprate()->UpdateRecordFileStatus(pFileParam->m_lMsgID, ENUM_FAILED_END);
	}
	
	CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pFileParam->m_pContact);
	if (pChatTabControlHandler != NULL)
	{
		//if (pFileParam->m_lFileSize == uliFileSize.QuadPart)
		{
			pChatTabControlHandler->ProcessDownLoadFileOK(pFileParam->m_lMsgID, bComplete, pFileParam->m_strFileName,pFileParam->m_enumFileType,pFileParam->m_strDisPlayName,pFileParam->m_hWnd);
		}
		//else
		{
			//	pChatTabControlHandler->ProcessDownLoadFileFailed(pFileParam->m_lMsgID);
		}
	}
}
string  CMainWnd::ProcessMsgFile(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize)
{
	UpLoadDownLoadParam *pUpLoadParam = new UpLoadDownLoadParam();
	std::string strTrueFileName= CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, srFileName, strHttpURL,ENUM_MSG_FILE, pContact, nFileSize, msgID);
	CSystemEx::GetInstance()->AddFole2DownLoadList(pUpLoadParam);
	return strTrueFileName;
}

void  CMainWnd::ProcessMsgEmoji(std::string strTime, std::string srFileName, TAG_RECENT *pContact)
{
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsgEmoji(srFileName, strTime);
		}
	}
}
void  CMainWnd::ProcessMsgOther(std::string strTime, std::string srFileName, TAG_RECENT *pContact)
{
	if (m_pMainCanvasHandler != NULL)
	{
		CChatTabControlHandler* pChatTabControlHandler = m_pMainCanvasHandler->FindChatTabControlHandlerByContactDBPointer(pContact);
		if (pChatTabControlHandler)
		{
			pChatTabControlHandler->InsertMsgOther(srFileName, strTime);
		}
	}
}

void CMainWnd::PrintJsonKeyValueError(std::string strJsonValue)
{
	std::string strMsgError = "there is something error where process  json msg:";
	strMsgError += strJsonValue;
	LOG4_INFO(strMsgError.c_str());
}

void CMainWnd::TestFunDownLoad()
{
	//CContactDB *pContactDB = NULL;
	//pContactDB = FindContactDBBySipNumber("102");
	//ProcessMsgFile("", "wjbjq.jpg", "192.168.1.233:8080/4,0d5aa8087c", pContactDB);
	//ProcessMsgFile("","11.wav",)
}

void CMainWnd::TestFunUpLoad()
{
// 	CContactDB *pContactDB = NULL;
// 	pContactDB = FindContactDBBySipNumber("102");
// 	UpLoadDownLoadParam* pUpLoadParam = new UpLoadDownLoadParam;
// 	pUpLoadParam->m_strFilePath = "D:\\Program Files\\";
// 	PortUtility::CheckFilePath(pUpLoadParam->m_strFilePath);
// 	pUpLoadParam->m_pContact = pContactDB;
// 	pUpLoadParam->m_strFileName = "alienware.jpg";
// 	pUpLoadParam->m_strMediaType = MESSAGE_TYPE_FILE;
// 	CSystemEx::GetInstance()->AddFile2UploadList(pUpLoadParam
}

void CMainWnd::RefreshRecentContantOnlineStatus(CContactDB *pContact,bool &bUpdate)
{
	if (m_pMainCanvasHandler==NULL|| m_pMainCanvasHandler->m_pTreeRecentContacts == NULL)
	{
		return;
	}
	m_pMainCanvasHandler->RefreshRecentContantOnlineStatus(pContact, bUpdate);
}
void CMainWnd::RefreshLocalContactOnlineStatus(CContactDB *pContact)
{
	if (m_pMainCanvasHandler == NULL || m_pMainCanvasHandler->m_pTreeRecentContacts == NULL)
	{
		return;
	}
	m_pMainCanvasHandler->RefreshLocalContactOnlineStatus(pContact);
}

void CMainWnd::RefreshLocalContactUnReadNum(std::string  strSipNumber, CChatRecord oChatRecord)
{
	if (m_pMainCanvasHandler == NULL || m_pMainCanvasHandler->m_pTreeRecentContacts == NULL)
	{
		return;
	}
	CContactDB *pContactDB = NULL;
	TAG_RECENT *pContactInfo = FindContactDBBySipNumber(strSipNumber);
	pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
	if (pContactInfo != NULL)
	{
		m_pMainCanvasHandler->RefreshLocalContactUnReadNum(pContactInfo, oChatRecord);
	}
}
void CMainWnd::RefreshLocalContactInfo(TAG_RECENT *pContact)
{
	if (m_pMainCanvasHandler == NULL || m_pMainCanvasHandler->m_pTreeRecentContacts == NULL)
	{
		OutputDebugString(L"504\n");
		return;
	}
	m_pMainCanvasHandler->RefreshLocalContactInfo(pContact);
	
}

void CMainWnd::CreatInfoWnd(LPCTSTR lpTitle, LPCTSTR lpInfo, HWND hParent, int nAutoCloseTime)
{
	InfoWndParam *pParam = new InfoWndParam;
	pParam->strMsg = lpInfo;
	pParam->strTitle = lpTitle;
	pParam->nAutoTimer = nAutoCloseTime;
	SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, NULL);
	
}
void CMainWnd::ClosedInfoWnd(CInfoWnd *pInfoWnd)
{
	m_oInfoMgr.ClosedInfoWnd(pInfoWnd);
	return;
}

void CMainWnd::TipsFunNoRun()
{
	m_oInfoMgr.CreatInfoWnd(_T("tips"), L"enable this fun", GetSafeHwnd(), 3000);
}

void CMainWnd::Add2Favorite(TAG_RECENT* pContact)
{
	m_vecFavoriteContact.push_back(pContact);
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->Insert2Favorite(pContact);
	}
}
void CMainWnd::Add2Contact(TAG_RECENT* pContact)
{
	bool bExist = false;
	auto iterFind = m_vecContact.begin();
	for (iterFind;iterFind!=m_vecContact.end();iterFind++)
	{
		TAG_RECENT *pLocalContact = *iterFind;
		if (pContact->m_enumRecentContact==pLocalContact->m_enumRecentContact&&pLocalContact->m_enumRecentContact==ENUM_SINGLE_CONTACT)
		{
			if (PortUtility::compareStringNoCase(pContact->m_unionContactInfo.m_pContact->m_strSIPNum, pLocalContact->m_unionContactInfo.m_pContact->m_strSIPNum)==0)
			{
				bExist = true;
				break;
			}
		}
	}
	RefreshLocalContactInfo(pContact);
	if (bExist)
	{
		return;
	}
	m_vecContact.push_back(pContact);
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->Insert2Contact(pContact);
	}
	
	
}
void CMainWnd::Add2DBContact(TAG_RECENT* pContact)
{
	bool bExist = false;
	auto iterFind = m_vecDBContact.begin();
	for (iterFind; iterFind != m_vecDBContact.end(); iterFind++)
	{
		TAG_RECENT *pLocalContact = *iterFind;
		if (pContact->m_enumRecentContact == pLocalContact->m_enumRecentContact&&pLocalContact->m_enumRecentContact == ENUM_SINGLE_CONTACT)
		{
			if (PortUtility::compareStringNoCase(pContact->m_unionContactInfo.m_pContact->m_strSIPNum, pLocalContact->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
			{
				bExist = true;
				break;
			}
		}
	}
	RefreshLocalContactInfo(pContact);
	if (bExist)
	{
		return;
	}
	m_vecDBContact.push_back(pContact);
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->Insert2DBContact(pContact);
	}
}
void CMainWnd::Add2RemoteContact(TAG_RECENT* pContact)
{
	m_vecRemoteContact.push_back(pContact);
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->Insert2RemoteContact(pContact);
	}
}
void CMainWnd::Add2Extension(TAG_RECENT* pContact)
{
	m_vecExtersionContact.push_back(pContact);
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->Insert2RemoteContact(pContact);
	}
}
void CMainWnd::UpdateFavorite(TAG_RECENT* pContact)
{
}
void CMainWnd::UpdateContact(TAG_RECENT* pContact)
{
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{

		pContactCanvasHandler->UpdateContactByContact(pContact->m_unionContactInfo.m_pContact);
		pContactCanvasHandler->UpdateFavoriteByContact(pContact->m_unionContactInfo.m_pContact);
		pContactCanvasHandler->UpdateFavorites();
		pContactCanvasHandler->UpdateContacts();
		pContactCanvasHandler->UpdateExtersion();
		pContactCanvasHandler->UpdateRemote();
	}
	if (pContact->m_enumRecentContact==ENUM_SINGLE_CONTACT)
	{

		bool bUpdate;
		m_pMainCanvasHandler->RefreshRecentContantOnlineStatus(pContact->m_unionContactInfo.m_pContact, bUpdate);
		RefreshLocalContactInfo(pContact);

		OutputDebugString(L"\n222RefreshLocalContactInfo\n");
	}
	
}
void CMainWnd::UpdateRemoteContact(TAG_RECENT* pContact)
{
}
void CMainWnd::UpdateExtension(TAG_RECENT* pContact)
{
}
void CMainWnd::EraseContactFavorite(std::string strSipNumber, int nUserID)
{
	CContactDB* pFavoriteContact = NULL;
	TAG_RECENT* pContactInfo = NULL;
	auto iterFindFavoriteContact = m_vecFavoriteContact.begin();
	for (; iterFindFavoriteContact != m_vecFavoriteContact.end();)
	{
		pContactInfo = *iterFindFavoriteContact;
		pFavoriteContact = pContactInfo->m_unionContactInfo.m_pContact;
		if (pFavoriteContact->m_nUserID == nUserID&&PortUtility::compareStringNoCase(strSipNumber,pFavoriteContact->m_strSIPNum)==0)
		{
			iterFindFavoriteContact = m_vecFavoriteContact.erase(iterFindFavoriteContact); 
			//TODO
			break;
		}
		else
		{
			iterFindFavoriteContact++;
		}
	}

	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->EraseItemFavorite(strSipNumber,nUserID);
	}

	
}
void CMainWnd::EraseContactLocal(std::string strSipNumber, int nUserID)
{
	CContactDB* pLocalContact = NULL;
	TAG_RECENT* pContactInfo = NULL;

	auto iterFindLocalContact = m_vecDBContact.begin();
	for (; iterFindLocalContact != m_vecDBContact.end();)
	{
		pContactInfo = *iterFindLocalContact;
		
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pLocalContact->m_nUserID == nUserID&&PortUtility::compareStringNoCase(pLocalContact->m_strSIPNum,strSipNumber)==0)
			{
				iterFindLocalContact = m_vecDBContact.erase(iterFindLocalContact);
				break;
			}
			else
			{
				iterFindLocalContact++;
			}
		}
		else
		{
			iterFindLocalContact++;
		}
		
	}
	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->EraseItemContact(strSipNumber, nUserID);
		pContactCanvasHandler->EraseItemFavorite(strSipNumber, nUserID);
	}
	DeleteContactInfo(pContactInfo);
	RefreshLocalContactInfo(pContactInfo);
}
TAG_RECENT *  CMainWnd::SearchFromExtensionContact(std::string strKeySipNumber)
{

	TAG_RECENT *pExtersionContact = NULL;
	auto iterExtersion = m_vecExtersionContact.begin();
	for (iterExtersion; iterExtersion != m_vecExtersionContact.end(); iterExtersion++)
	{
		pExtersionContact = *iterExtersion;
		std::string strLocalSipNumber = strKeySipNumber;
		std::string strExtersionNumber = pExtersionContact->m_unionContactInfo.m_pContact->m_strSIPNum;
		if (!strLocalSipNumber.empty() && !strExtersionNumber.empty())
		{
			if (PortUtility::compareStringNoCase(strLocalSipNumber, strExtersionNumber) == 0)
			{
				return pExtersionContact;
			}
		}
	}
	return NULL;
}
void CMainWnd::DeleteContactInfo(TAG_RECENT* pContactInfo)
{
	ENUM_RECENT_CONTACT_TYPE localRecentContactType = pContactInfo->m_enumRecentContact;
	CContactDB *pContactDB = NULL;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB==NULL)
		{
			return;
		}
		TAG_RECENT *pContactInfo = SearchFromExtensionContact(pContactDB->m_strSIPNum);
		if (pContactInfo)
		{
			pContactDB->m_strFirstName = pContactInfo->m_unionContactInfo.m_pContact->m_strFirstName;
			pContactDB->m_strLastName = pContactInfo->m_unionContactInfo.m_pContact->m_strLastName;
			return;
		}

		if (pContactDB)
		{
			pContactDB->m_strFirstName = "";
			pContactDB->m_strLastName = "";
			pContactDB->m_strEmailAdress = "";
		}
	}
}
void CMainWnd::EraseContactRemoteContact(long long nDataID)
{
	CContactDB* pLocalContact = NULL;
	TAG_RECENT* pContactInfo = NULL;


	CContactDB* pContact = NULL;
	auto iterFindRemoteContact = m_vecRemoteContact.begin();
	for (; iterFindRemoteContact != m_vecRemoteContact.end();)
	{
		pContactInfo = *iterFindRemoteContact;

		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;

			if (pContact->m_nUserID == nDataID)
			{
				iterFindRemoteContact = m_vecRemoteContact.erase(iterFindRemoteContact);
				break;
			}
			else
			{
				iterFindRemoteContact++;
			}
		}
		else
		{
			iterFindRemoteContact++;
		}
	}
}
void CMainWnd::EraseContactExtension(long long nDataID)
{
	/*
	TAG_RECENT* pContactInfo = NULL;
	CContactDB* pLocalContact = NULL;
	auto iterFindExternsion = m_vecExtersionContact.begin();
	for (; iterFindExternsion != m_vecExtersionContact.end();)
	{
		pContactInfo = *iterFindExternsion;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pLocalContact->m_nContactID == nDataID)
			{
				iterFindExternsion = m_vecExtersionContact.erase(iterFindExternsion);
				break;
			}
			else
			{
				iterFindExternsion++;
			}
		}
		else
		{
			iterFindExternsion++;
		}
	}
	*/
}

int CMainWnd::SearchFromContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString, bool bGroupSwtich)
{
	SearchContact(vecContact,strKeyString, bGroupSwtich);
	//SearchFromRemoteContact(vecContact, strKeyString);
	SearchFromExtensionContact(vecContact, strKeyString);
	return vecContact.size();
}
void CMainWnd::SearchContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString,bool bGroupSwtich)
{
	if (strKeyString.empty())
	{
		vector<TAG_RECENT* >::iterator iterRemoteContact = m_vecContact.begin();
		for (iterRemoteContact; iterRemoteContact != m_vecContact.end(); iterRemoteContact++)
		{
			TAG_RECENT *pLocalContactData = *iterRemoteContact;
			vecContact.push_back(pLocalContactData);
		}
		return;
	}

	vector<TAG_RECENT* >::iterator iterRemoteContact = m_vecContact.begin();
	for (iterRemoteContact; iterRemoteContact != m_vecContact.end(); iterRemoteContact++)
	{
		TAG_RECENT *pLocalContactData = *iterRemoteContact;
		CContactDB *pContactDB = pLocalContactData->m_unionContactInfo.m_pContact;
		std::string strUserName = pContactDB->m_strFirstName + pContactDB->m_strLastName;
		if (PortUtility::findStringLike(strUserName, strKeyString) >= 0 || PortUtility::findStringLike(pContactDB->m_strSIPNum, strKeyString) >= 0)
		{
			vecContact.push_back(pLocalContactData);
		}
	}
}
void CMainWnd::SearchFromRemoteContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString)
{
	if (strKeyString.empty())
	{
		vector<TAG_RECENT* >::iterator iterRemoteContact = m_vecRemoteContact.begin();
		for (iterRemoteContact; iterRemoteContact != m_vecRemoteContact.end(); iterRemoteContact++)
		{
			
			TAG_RECENT *pLocalContactInfo = *iterRemoteContact;
			CContactDB *pContactDB = pLocalContactInfo->m_unionContactInfo.m_pContact;
			vecContact.push_back(pLocalContactInfo);
		}
		return;
	}
	
	vector<TAG_RECENT* >::iterator iterRemoteContact = m_vecRemoteContact.begin();
	for (iterRemoteContact; iterRemoteContact != m_vecRemoteContact.end(); iterRemoteContact++)
	{
		TAG_RECENT *pLocalContactInfo = *iterRemoteContact;
		CContactDB *pLocalContactData = pLocalContactInfo->m_unionContactInfo.m_pContact;

		std::string strUserName = pLocalContactData->m_strFirstName + pLocalContactData->m_strLastName;
		if (PortUtility::findStringLike(strUserName, strKeyString) >= 0 || PortUtility::findStringLike(pLocalContactData->m_strSIPNum, strKeyString) >= 0)
		{
			vecContact.push_back(pLocalContactInfo);
		}
	}
}
void CMainWnd::SearchFromExtensionContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString)
{
	if (strKeyString.empty())
	{
		//return all Data
		vector<TAG_RECENT* >::iterator iterExtensionsContact = m_vecExtersionContact.begin();
		for (iterExtensionsContact; iterExtensionsContact != m_vecExtersionContact.end(); iterExtensionsContact++)
		{
			TAG_RECENT *pLocalContactInfo = *iterExtensionsContact;
			CContactDB *pLocalContactData = pLocalContactInfo->m_unionContactInfo.m_pContact;

			vecContact.push_back(pLocalContactInfo);
		}
		return;
	}

	vector<TAG_RECENT* >::iterator iterExtensionsContact = m_vecExtersionContact.begin();
	for (iterExtensionsContact; iterExtensionsContact != m_vecExtersionContact.end(); iterExtensionsContact++)
	{
		TAG_RECENT *pLocalContactInfo = *iterExtensionsContact;
		CContactDB *pLocalContactData = pLocalContactInfo->m_unionContactInfo.m_pContact;
		std::string strUserName = pLocalContactData->m_strFirstName + pLocalContactData->m_strLastName;
		if (PortUtility::findStringLike(strUserName, strKeyString) >= 0 || PortUtility::findStringLike(pLocalContactData->m_strSIPNum, strKeyString) >= 0)
		{
			vecContact.push_back(pLocalContactInfo);
		}
	}
}

void CMainWnd::ResetOnLineStatus(bool bOnLine)
{
	CSystemEx::GetInstance()->SetRegister(false);
	InfoWndParam *pParam = new InfoWndParam;
	pParam->strMsg = L"lost connect to server";
	pParam->strTitle = _T("Error");
	pParam->nAutoTimer = 3000;
	SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);

	resetContactOnlineStatusOffLine();
}
void CMainWnd::resetContactOnlineStatusOffLine()
{
	auto iterContact = m_vecContact.begin();
	for (iterContact; iterContact != m_vecContact.end(); iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			if (pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState==ONLINESTATE_ONLINE)
			{
				pFindContactInfo->m_unionContactInfo.m_pContact->SaveLastOnlineStateByNowState();
				pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState = ONLINESTATE_OFFLINE;
				bool bUpdate = false;
				RefreshRecentContantOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact, bUpdate);
				if (!bUpdate)
				{
					RefreshLocalContactOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact);
				}
			}
		}
	}

	iterContact = m_vecRemoteContact.begin();
	for (iterContact; iterContact != m_vecRemoteContact.end(); iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			if (pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState == ONLINESTATE_ONLINE)
			{
				pFindContactInfo->m_unionContactInfo.m_pContact->SaveLastOnlineStateByNowState();
				pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState = ONLINESTATE_OFFLINE;
				bool bUpdate = false;
				RefreshRecentContantOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact, bUpdate);
				if (!bUpdate)
				{
					RefreshLocalContactOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact);
				}
			}
		}
	}

	iterContact = m_vecExtersionContact.begin();
	for (iterContact; iterContact != m_vecExtersionContact.end(); iterContact++)
	{
		TAG_RECENT * pFindContactInfo = *iterContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pFindContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			if (pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState == ONLINESTATE_ONLINE)
			{
				pFindContactInfo->m_unionContactInfo.m_pContact->SaveLastOnlineStateByNowState();
				pFindContactInfo->m_unionContactInfo.m_pContact->m_eOnlineState = ONLINESTATE_OFFLINE;
				bool bUpdate = false;
				RefreshRecentContantOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact, bUpdate);
				if (!bUpdate)
				{
					RefreshLocalContactOnlineStatus(pFindContactInfo->m_unionContactInfo.m_pContact);
				}
			}
		}
	}
	

}
void CMainWnd::ReConnect2Server()
{
	CSystemEx::GetInstance()->ReConnectThread();
}
void CMainWnd::EraseItemRemoteContact()
{
	CSystemEx::GetInstance()->GetContactCanvasHandler()->EraseItemRemoteContact();
}
void CMainWnd::EraseItemExtension()
{
	CSystemEx::GetInstance()->GetContactCanvasHandler()->EraseItemExtension();
}
void CMainWnd::reSubContactStatus()
{
	m_vecExtersionContact = CSystemEx::GetInstance()->GetRestAPI()->GetExtensions();
	m_vecRemoteContact = CSystemEx::GetInstance()->GetRestAPI()->GetRemoteContacts();

	CSystemEx::GetInstance()->GetContactCanvasHandler()->InitExtersions();
	CSystemEx::GetInstance()->GetContactCanvasHandler()->InitRemoteContacts();
	m_OnLineStatusMgr.InitLocalContact(m_vecExtersionContact);
	m_OnLineStatusMgr.StartSub();
}
LRESULT CMainWnd::ProcessLoginReslut(bool bLoginSuccess, std::string strErrorInfo)
{
	if (bLoginSuccess)
	{
		CSystemEx::GetInstance()->InitUserOptions();
		CSystemEx::GetInstance()->SetRegister(true);
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = L"register success";
		pParam->strTitle = _T("Info");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);

		
		EraseItemExtension();
		EraseItemRemoteContact();

		reSubContactStatus();
	}
	else
	{
		CSystemEx::GetInstance()->SetRegister(false);
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = PortUtility::Utf82Unicode_ND(strErrorInfo).c_str();
		pParam->strTitle = _T("Error");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
	}
	return 0;

}
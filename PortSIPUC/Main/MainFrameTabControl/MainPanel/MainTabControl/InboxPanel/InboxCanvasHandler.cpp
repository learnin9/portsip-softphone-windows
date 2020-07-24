#include "stdafx.h"
#include "InboxCanvasHandler.h"
#include "system/SystemEx.h"
#include "../../AddContactWnd.h"
CInboxCanvasHandler::CInboxCanvasHandler()
	: m_pRadCalls(NULL)
	, m_pStaUnreadCalls(NULL)
	, m_pRadMissed(NULL)
	, m_pStaUnreadMissed(NULL)
	, m_pRadVoiceMails(NULL)
	, m_pStaUnreadVoicemail(NULL)
	, m_pRadRecordings(NULL)
	, m_pStaUnreadRecord(NULL)
	, m_pRadMessages(NULL)
	, m_pStaUnreadMessage(NULL)
	, m_pTabInbox(NULL)
	, m_pStaSplitter(NULL)

	, m_pTvCalls(NULL)
	, m_pTvMissed(NULL)
	, m_pTvMessages(NULL)
	, m_pBtnFirstPage(NULL)
	, m_pBtnPrePage(NULL)
	, m_pBtnNextPage(NULL)
	, m_pBtnLastPage(NULL)
{
	m_hOriginalSenderCallHistory = NULL;
	m_hOriginalSenderMissed = NULL;
	m_hOriginalSenderVoicemails = NULL;
	m_hOriginalSenderRecords = NULL;
	m_hOriginalSenderMsgs = NULL;
	m_bIsPlaying = false;
	m_nVoiceMailIndex = 1;
	m_nRecordIndex =	1;
	m_nMissCallIndex =	1;
	m_nLocalCallIndex = 1;

	m_nLocalCallIndexMax = 0;
	m_nRecordIndexMax	 = 0;
	m_nMissCallIndexMax	 = 0;
	m_nLocalCallIndexMax = 0;

	m_nTabPage = ENUM_CALL_PAGE;
	m_nLastTabPage = ENUM_CALL_PAGE;

	m_pAudioItemProp = NULL;
	m_pBindVideoProp = NULL;
	m_pBtnPlaying = NULL;
	m_hCurrentPlayingButton = NULL;
	m_hLastPlayButton = NULL;
}

CInboxCanvasHandler::~CInboxCanvasHandler()
{
}

int CInboxCanvasHandler::InitControls()
{
	m_pRadCalls = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_CALLS")));
	_ASSERT(m_pRadCalls != NULL);
	m_pStaUnreadCalls = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UNREAD_CALLS")));
	_ASSERT(m_pStaUnreadCalls != NULL);
	m_pRadMissed = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_MISSED")));
	_ASSERT(m_pRadMissed != NULL);
	m_pStaUnreadMissed = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UNREAD_MISSED")));
	_ASSERT(m_pStaUnreadMissed != NULL);
	m_pRadVoiceMails = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_VOICE_MAILS")));
	_ASSERT(m_pRadVoiceMails != NULL);
	m_pStaUnreadVoicemail = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UNREAD_VOICEMAIL")));
	_ASSERT(m_pStaUnreadVoicemail != NULL);
	m_pRadRecordings = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_RECORDINGS")));
	_ASSERT(m_pRadRecordings != NULL);
	m_pStaUnreadRecord = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UNREAD_RECORD")));
	_ASSERT(m_pStaUnreadRecord != NULL);
	m_pRadMessages = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_MESSAGES")));
	_ASSERT(m_pRadMessages != NULL);
	m_pStaUnreadMessage = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UNREAD_MESSAGE")));
	_ASSERT(m_pStaUnreadMessage != NULL);
	m_pTabInbox = (TabControl *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PNL_TAB_INBOX")));
	_ASSERT(m_pTabInbox != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pBtnFirstPage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_FIRST_PAGE")));
	_ASSERT(m_pBtnFirstPage != NULL);
	m_pBtnPrePage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PRE_PAGE")));
	_ASSERT(m_pBtnPrePage != NULL);
	m_pBtnNextPage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NEXT_PAGE")));
	_ASSERT(m_pBtnNextPage != NULL);
	m_pBtnLastPage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_LAST_PAGE")));
	_ASSERT(m_pBtnLastPage != NULL);
	
	
	m_pStaUnreadCalls->ShowWindow(SW_HIDE); 
	m_pStaUnreadMessage->ShowWindow(SW_HIDE);
	

	return 0;
}

int CInboxCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	m_pTabInbox->AddPropertySheet(m_pRadCalls->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pTabInbox->AddPropertySheet(m_pRadMissed->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pTabInbox->AddPropertySheet(m_pRadVoiceMails->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pTabInbox->AddPropertySheet(m_pRadRecordings->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pTabInbox->AddPropertySheet(m_pRadMessages->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);

	m_pTabInbox->SwitchPropertySheet(m_pRadMissed->GetSafeHwnd());
	m_pTabInbox->SwitchPropertySheet(m_pRadVoiceMails->GetSafeHwnd());
	m_pTabInbox->SwitchPropertySheet(m_pRadRecordings->GetSafeHwnd());
	m_pTabInbox->SwitchPropertySheet(m_pRadMessages->GetSafeHwnd());
	m_pTabInbox->SwitchPropertySheet(m_pRadCalls->GetSafeHwnd());

	CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init( ENUM_AUDIO);
	return 0;
}
void CInboxCanvasHandler::InitHistory()
{
	m_vecCallHistory = CSystemEx::GetInstance()->GetDBOprate()->GetAllCallMsg(MAX_PAGE_SHOW_CONTACT,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	m_nLocalCallIndexMax = ceil((float)m_vecCallHistory.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_nLocalCallIndexMax==0)
	{
		m_nLocalCallIndexMax = 1;
	}
	Swtich2CallPage(false);
	m_pBtnLastPage->EnableWindow(false);
}
void CInboxCanvasHandler::InitVoiceMail()
{
	m_vecVoiceMail = CSystemEx::GetInstance()->GetDBOprate()->GetVoiceMailDownLoadSuccess(CSystemEx::GetInstance()->GetGlobalUser()->GetUID(),MAX_PAGE_SHOW_CONTACT);
	m_nVoiceMailIndexMax = ceil((float)m_vecVoiceMail.size() / (float)MAX_CALL_VIEW_NUM);
	RefreshVoicemail();
}
void CInboxCanvasHandler::InitMiss()
{
	int nCallerID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	m_vecMissedHistory = CSystemEx::GetInstance()->GetDBOprate()->GetMissedCallMsg(MAX_PAGE_SHOW_CONTACT, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	m_nMissCallIndexMax = ceil((float)m_vecMissedHistory.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_nMissCallIndexMax == 0)
	{
		m_nMissCallIndexMax = 1;
	}
	RefreshMiss();
}
void CInboxCanvasHandler::InitRecord()
{
	m_vecVARecord = CSystemEx::GetInstance()->GetDBOprate()->GetPageAVRecord(CSystemEx::GetInstance()->GetGlobalUser()->GetUID(),MAX_PAGE_SHOW_CONTACT);
	m_nRecordIndexMax = ceil((float)m_vecVARecord.size() / (float)MAX_CALL_VIEW_NUM);
	RefreshRecord();
}
CIUIString CInboxCanvasHandler::FormatTimeType(CCallHistory oCallHistory, ENUM_CALL_STATUS enumCallStatus)
{
	CIUIString strFormat = "";
	CIUIString strType;
	if (oCallHistory.m_enumCallType == ENUM_CALL_OUT)
	{
		strType = CSystemEx::GetInstance()->GetCallOutText();
	}
	else
	{
		strType = CSystemEx::GetInstance()->GetCallInText();
	}
	if (oCallHistory.m_strEndTime.empty()|| oCallHistory.m_strBeginTime.empty()|| enumCallStatus==ENUM_CALL_STATUS_FAILD)
	{
		return strType;
	}
	std::string strTimeFormat;
	time_t stuCallTime = PortUtility::StringToDatetime(oCallHistory.m_strEndTime.c_str())- PortUtility::StringToDatetime(oCallHistory.m_strBeginTime.c_str());
	tm *pTmp = localtime(&stuCallTime);
	//int nTimeHour = pTmp->tm_hour;
	int nMin = pTmp->tm_min;
	int nSec = pTmp->tm_sec;
	char szTime[MAX_PATH] = { 0 };
	_snprintf(szTime, MAX_PATH - 1, "%02d:%02d", nMin, nSec);
	strTimeFormat.append(szTime);
	strFormat = strType +"/"+PortUtility::Utf82Unicode_ND(strTimeFormat).c_str();
	return strFormat;
}



int CInboxCanvasHandler::InitInboxCalls(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->SetInboCanvasHandler(this);
	return 0;
}

int CInboxCanvasHandler::InitInboxMissed(RoutedEventArgs *pe)
{
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

	CIUIProject *pProject = GetProject(pe->hOriginalSender);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_VOICEMAILS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);
	return 0;
}

int CInboxCanvasHandler::InitInboxVoiceMails(RoutedEventArgs *pe)
{
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

	CIUIProject *pProject = GetProject(pe->hOriginalSender);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_VOICEMAILS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);
	return 0;
}

int CInboxCanvasHandler::InitInboxRecordings(RoutedEventArgs *pe)
{
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

	CIUIProject *pProject = GetProject(pe->hOriginalSender);

	CControlProp *pAudioItemProp = g_pProject->FindProjectItem(_T("IDW_INBOX_VOICEMAILS_ITEM.xml"));
	_ASSERT(pAudioItemProp != NULL);
	CControlProp *pBindAudioProp = (CControlProp *)pAudioItemProp->GetChild(0);
	_ASSERT(pBindAudioProp != NULL);

	CControlProp *pVideoItemProp = g_pProject->FindProjectItem(_T("IDW_INBOX_RECORD_VIDEO_ITEM.xml"));
	_ASSERT(pVideoItemProp != NULL);
	CControlProp *pBindVideoProp = (CControlProp *)pVideoItemProp->GetChild(0);
	_ASSERT(pBindVideoProp != NULL);

	m_pAudioItemProp = pAudioItemProp;
	m_pBindVideoProp = pVideoItemProp;
	//TODO INIT CONTACT MSG
	DoFlushRecord(m_nRecordIndex);
	return 0;
}

int CInboxCanvasHandler::InitInboxMessages(RoutedEventArgs *pe)
{
	m_pTvMessages = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

	CIUIProject *pProject = GetProject(pe->hOriginalSender);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_MESSAGES_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);
	return 0;
}

int CInboxCanvasHandler::OnCreate(RoutedEventArgs *pe)
{
	if (m_pTabInbox != NULL)
	{
		CREATESTRUCT *pcs = (CREATESTRUCT *)pe->lParam;
		CIUIProject *pProject = GetProject(pe->hOriginalSender);
		_ASSERT(pProject != NULL);

		CPropertySheetInfo *pSheetCalls = m_pTabInbox->GetPropertySheet(m_pRadCalls->GetSafeHwnd());
		if (pSheetCalls != NULL && pcs->lpCreateParams == pSheetCalls)
		{
			m_hOriginalSenderCallHistory = pe->hOriginalSender;
			InitInboxCalls(pe);
			
		}

		CPropertySheetInfo *pSheetMissed = m_pTabInbox->GetPropertySheet(m_pRadMissed->GetSafeHwnd());
		if (pSheetMissed != NULL && pcs->lpCreateParams == pSheetMissed)
		{
			m_hOriginalSenderMissed = pe->hOriginalSender;
			InitInboxMissed(pe);
		}


		CPropertySheetInfo *pSheetVoiceMails = m_pTabInbox->GetPropertySheet(m_pRadVoiceMails->GetSafeHwnd());
		if (pSheetVoiceMails != NULL && pcs->lpCreateParams == pSheetVoiceMails)
		{
			m_hOriginalSenderVoicemails = pe->hOriginalSender;
			InitInboxVoiceMails(pe);
		}


		CPropertySheetInfo *pSheetRecordings = m_pTabInbox->GetPropertySheet(m_pRadRecordings->GetSafeHwnd());
		if (pSheetRecordings != NULL && pcs->lpCreateParams == pSheetRecordings)
		{
			m_hOriginalSenderRecords= pe->hOriginalSender; 
			InitInboxRecordings(pe);
		}

		CPropertySheetInfo *pSheetMessages = m_pTabInbox->GetPropertySheet(m_pRadMessages->GetSafeHwnd());
		if (pSheetMessages != NULL && pcs->lpCreateParams == pSheetMessages)
		{
			m_hOriginalSenderMsgs = pe->hOriginalSender;
			InitInboxMessages(pe);
		}
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnTreeViewHotChanging(RoutedEventArgs *pe)
{
	if (m_pTvCalls->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvMissed->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvMessages->GetSafeHwnd() == pe->hOriginalSender)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		HTREEITEM hNewHot = (HTREEITEM)pe->lParam;

		if (hOldHot==NULL&& hNewHot==NULL)
		{
			return 0;
		}

		CIUIPoint pt;
		GetCursorPos(&pt);
		HWND hHost = GetHostWnd(GetBindWLWnd());
		ScreenToClient(hHost, &pt);
		HWLWND hHit = WindowFromPoint(hHost, pt);
		if (NULL != hHit && NULL != hOldHot)
		{
			HWLWND hBindWnd = pTreeView->GetItemBindWindowless(hOldHot);

			BOOL bDescendant = IsDescendant(hBindWnd, hHit);
			if (bDescendant)
			{
				pe->lResult = TRUE;
				return 1;
			}
		}
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnTreeViewHotChanged(RoutedEventArgs *pe)
{
	if (m_pTvCalls->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvMissed->GetSafeHwnd() == pe->hOriginalSender)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		if (NULL != hOldHot)
		{
			HWLWND hBind = pTreeView->GetItemBindWindowless(hOldHot);

			HWLWND hPhone = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
			_ASSERT(NULL != hPhone);
			ShowWindow(hPhone, SW_HIDE);

			HWLWND hBtnVideo = IUI::FindControl(hBind, _T("IDC_BTN_VIDEO"));
			_ASSERT(NULL != hBtnVideo);
			ShowWindow(hBtnVideo, SW_HIDE);

			HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
			_ASSERT(NULL != hBtnMore);
			ShowWindow(hBtnMore, SW_HIDE);

			Invalidate(pTreeView->GetSafeHwnd());
		}

		HTREEITEM hHot = (HTREEITEM)pe->lParam;
		if (NULL != hHot)
		{
			HWLWND hBind = pTreeView->GetItemBindWindowless(hHot);

			HWLWND hPhone = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
			_ASSERT(NULL != hPhone);
			ShowWindow(hPhone, SW_SHOW);

			HWLWND hBtnVideo = IUI::FindControl(hBind, _T("IDC_BTN_VIDEO"));
			_ASSERT(NULL != hBtnVideo);
			ShowWindow(hBtnVideo, SW_SHOW);

			HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
			_ASSERT(NULL != hBtnMore);
			ShowWindow(hBtnMore, SW_SHOW);
		}
	}
	else if (m_pTvMessages->GetSafeHwnd() == pe->hOriginalSender)
	{
		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		if (NULL != hOldHot)
		{
			HWLWND hBind = m_pTvMessages->GetItemBindWindowless(hOldHot);

			HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
			_ASSERT(NULL != hBtnMore);
			ShowWindow(hBtnMore, SW_HIDE);

			Invalidate(m_pTvMessages->GetSafeHwnd());
		}

		HTREEITEM hHot = (HTREEITEM)pe->lParam;
		if (NULL != hHot)
		{
			HWLWND hBind = m_pTvMessages->GetItemBindWindowless(hHot);

			HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
			_ASSERT(NULL != hBtnMore);
			ShowWindow(hBtnMore, SW_SHOW);
		}
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnLButtonDblClk(RoutedEventArgs *pe)
{
	//if (GetName(pe->hOriginalSender) == _T("IDC_TREECTRL_INBOX"))
	if (m_pTvCalls->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvMissed->GetSafeHwnd() == pe->hOriginalSender
		)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		CIUIPoint pt(pe->lParam);
		HTREEITEM hItem = pTreeView->HitTest(pt);
		if (NULL != hItem)
		{
			ItemUserData *pItemUserData = (ItemUserData *)pTreeView->GetItemData(hItem);
			TAG_RECENT* pContatctInfo = pItemUserData->m_pContactInfo;
			ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContatctInfo->m_enumRecentContact;
			if (localRecentContactType == ENUM_SINGLE_CONTACT)
			{
				CContactDB* pContact = pContatctInfo->m_unionContactInfo.m_pContact;
				if (NULL != pContact)
				{
					pContact->SetCallType(ENUM_CALL_OUT);
					SwitchToContact(pContatctInfo, CT_TEXT_CHAT);
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
		}
	}
	return 0;
}

LRESULT CInboxCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(hWndThis == this->GetBindWLWnd());

	if (hWndThis == pe->hOriginalSender)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pe->RoutedEvent == WM_INITDIALOG)
			{
				OnInitPanel(pe);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (WM_CREATE == pe->RoutedEvent)
		{
			OnCreate(pe);
		}
		else if (TVN_HOTCHANGING == pe->RoutedEvent)
		{
			OnTreeViewHotChanging(pe);
		}
		else if (TVN_HOTCHANGED == pe->RoutedEvent)
		{
			OnTreeViewHotChanged(pe);
		}
		else if (WM_HSCROLL == pe->RoutedEvent)
		{
			return OnHScroll(pe);
		}
		
		
	}
	else if (RS_TUNNEL == pe->eRoutingStrategy)
	{
		if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			OnLButtonDblClk(pe);
		}
		
	}
	else if (RS_DIRECT == pe->eRoutingStrategy)
	{
		if (WM_PLAY_BEGIN == pe->RoutedEvent)
		{
			OnPlay(pe);
		}
		else  if (WM_PLAY_END == pe->RoutedEvent)
		{
			OnPlayEnd(pe);
		}
		else if (WM_PLAY_POS_CHANGED == pe->RoutedEvent)
		{
			OnPlayPosChanged(pe);
		}
		else if (WM_PLAY_DRAG_POS == pe->RoutedEvent)
		{
			OnDragPos(pe);
		}
		else if (WM_PLAY_TIME_CHANGED == pe->RoutedEvent)
		{
			OnPlayTimeChanged(pe);
		}
	}

	return 0;
}
LRESULT CInboxCanvasHandler::OnPlay(RoutedEventArgs *pe)
{
	if (m_pBtnPlaying)
	{
		if (IUI::IsWindow(m_pBtnPlaying->GetSafeHwnd()))
		{
			if (m_pBtnPlaying->GetCheck() == BST_UNCHECKED)
			{
				m_pBtnPlaying->SetCheck(BST_CHECKED);
			}
		}
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnPlayEnd(RoutedEventArgs *pe)
{
	int nPos = (int)pe->wParam;
	CIUIString strText = (LPWSTR)pe->lParam;
	if (nPos < 100)
	{
		return 0;
	}
	if (strText.IsEmpty()==true)
	{
		return 0; 
	}
	if (m_pBtnPlaying)
	{
		if (IUI::IsWindow(m_pBtnPlaying->GetSafeHwnd()))
		{
			if (m_pBtnPlaying->GetCheck() == BST_CHECKED)
			{
				m_pBtnPlaying->SetCheck(BST_UNCHECKED);
			}
		}
	}
	m_pSldDuration->SetPos(nPos);

	if (m_hStaDuration)
	{
		SetWindowText(m_hStaDuration, strText);
	}

	Invalidate(GetBindWLWnd());
	return 0;
}

LRESULT CInboxCanvasHandler::OnPlayPosChanged(RoutedEventArgs *pe)
{
	int nPos = (int)pe->wParam;
	if (nPos < 0)
	{
		return 0;
	}
	if (nPos > 100)
	{
		nPos = 100;
	}
	if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState()== STATE_PAUSE)
	{
		return 0;
	}
	if (m_pSldDuration)
	{
		m_pSldDuration->SetPos(nPos);
	}

	return 0;
}
LRESULT CInboxCanvasHandler::OnDragPos(RoutedEventArgs *pe)
{

	return 0;
}
LRESULT CInboxCanvasHandler::OnPlayTimeChanged(RoutedEventArgs *pe)
{
	CIUIString strText = (LPWSTR)pe->wParam;
	if (m_hStaDuration)
	{
		SetWindowText(m_hStaDuration,strText);
	}
	return 0;
}

LRESULT CInboxCanvasHandler::OnRadCalls(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_CALL_PAGE;
	if (m_nLastTabPage==m_nTabPage)
	{
	}
	else
	{
		Swtich2CallPage(FALSE);
	}
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CInboxCanvasHandler::OnRadMissed(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_MISS_PAGE;
	if (m_nLastTabPage == m_nTabPage)
	{
	}
	else
	{
		Swtich2MissPage(FALSE);
	}
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CInboxCanvasHandler::OnRadVoiceMails(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_VOICE_MAIL_PAGE;
	if (m_nLastTabPage == m_nTabPage)
	{
	}
	else
	{
		Swtich2VoiceMailPage(FALSE);
	}
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CInboxCanvasHandler::OnRadRecordings(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_RECORD_PAGE;
	if (m_nLastTabPage == m_nTabPage)
	{
	}
	else
	{
		Swtich2RecordPage(FALSE);
	}
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CInboxCanvasHandler::OnRadMessages(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CInboxCanvasHandler::OnItemBtnAudio(RoutedEventArgs *pe)
{
	HWLWND hParent = pe->hOriginalSender;
	while (NULL != hParent)
	{
		ULONG_PTR dwData = GetBindItemControlData(hParent);
		if (NULL != dwData)
		{
			HTREEITEM hItem = (HTREEITEM)dwData;

			HWLWND hTreeView = GetParent(hParent);
			if (NULL != hTreeView)
			{
				TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(hTreeView);
				ItemUserData *pItemUserData = (ItemUserData *)pTreeView->GetItemData(hItem);
				TAG_RECENT* pContactInfo = pItemUserData->m_pContactInfo;
				//TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
				if (NULL != pContactInfo)
				{
					ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
					if (localRecentContactType == ENUM_SINGLE_CONTACT)
					{
						CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
						pContactDB->SetCallType(ENUM_CALL_OUT);
					}
					SwitchToContact(pContactInfo, CT_AUDIO);
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

			break;
		}

		hParent = GetParent(hParent);
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnItemBtnVideo(RoutedEventArgs *pe)
{
	HWLWND hParent = pe->hOriginalSender;
	while (NULL != hParent)
	{
		ULONG_PTR dwData = GetBindItemControlData(hParent);
		if (NULL != dwData)
		{
			HTREEITEM hItem = (HTREEITEM)dwData;

			HWLWND hTreeView = GetParent(hParent);
			if (NULL != hTreeView)
			{
				TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(hTreeView);
				ItemUserData *pItemUserData = (ItemUserData *)pTreeView->GetItemData(hItem);
				TAG_RECENT* pContactInfo = pItemUserData->m_pContactInfo;
				if (NULL != pContactInfo)
				{
					ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
					if (localRecentContactType == ENUM_SINGLE_CONTACT)
					{
						CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
						pContactDB->SetCallType(ENUM_CALL_OUT);
					}
					SwitchToContact(pContactInfo, CT_VIDEO);
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

			break;
		}

		hParent = GetParent(hParent);
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnItemBtnMore(RoutedEventArgs *pe)
{
	HWLWND hParent = pe->hOriginalSender;
	while (NULL != hParent)
	{
		ULONG_PTR dwData = GetBindItemControlData(hParent);
		if (NULL != dwData)
		{
			HTREEITEM hItem = (HTREEITEM)dwData;

			HWLWND hTreeView = GetParent(hParent);
			if (NULL != hTreeView)
			{
				TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(hTreeView);
				ItemUserData *pItemUserData = (ItemUserData *)pTreeView->GetItemData(hItem);
				TAG_RECENT* pContactInfo = pItemUserData->m_pContactInfo;
				//TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
				if (NULL != pContactInfo)
				{
					CIUIRect rcMore;
					IUI::GetWindowRect(pe->hOriginalSender, rcMore);
					if (m_nTabPage == ENUM_MISS_PAGE || m_nTabPage == ENUM_CALL_PAGE)
					{
						HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU_CALLRECORD));
						HMENU hMoreMenu = GetSubMenu(hMenu, 0);
						_ASSERT(NULL != hMoreMenu);
						ModifyMenu(hMoreMenu, 0, MF_BYPOSITION | MF_STRING, ID_CALLRECORD_ADDTOLOCALCONTACTS, CSystemEx::GetInstance()->GetAddContactText());
						ModifyMenu(hMoreMenu, 1, MF_BYPOSITION | MF_STRING, ID_CALLRECORD_DELETECALLRECORD, CSystemEx::GetInstance()->GetDeleteThisRecordText());
					
						CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
						bool bExitContact = CSystemEx::GetInstance()->GetDBOprate()->searchContact(pContactDB->m_strSIPNum);
						if (bExitContact)
						{
							EnableMenuItem(hMoreMenu,
								ID_CALLRECORD_ADDTOLOCALCONTACTS,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
						}
						UINT uRet = ::TrackPopupMenu(hMoreMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
							rcMore.left, rcMore.bottom, 0, GetHostWnd(GetBindWLWnd()), rcMore);
				
						
						if (ID_CALLRECORD_ADDTOLOCALCONTACTS == uRet)
						{
							CAddContactWnd *pAddContactWnd = new CAddContactWnd(ENUM_ADD_CONTACT, pContactInfo);
							if (NULL != pAddContactWnd)
							{
								pAddContactWnd->SetProject(g_pProject);
								pAddContactWnd->SetXml(_T("IDW_ADD_CONTACT.xml"));
								//
								pAddContactWnd->DoModal();
							}
						}
						else if (ID_CALLRECORD_DELETECALLRECORD == uRet&& m_nTabPage == ENUM_CALL_PAGE)
						{
							long lChatRecord = pItemUserData->m_nHistoryDataID;
							EraseItemInCall(lChatRecord);
							//pTreeView->DeleteItem(hItem);
						}
						else if (ID_CALLRECORD_DELETECALLRECORD == uRet && m_nTabPage == ENUM_MISS_PAGE)
						{
							long lChatRecord = pItemUserData->m_nHistoryDataID;
							EraseItemInMiss(lChatRecord);
							delete pItemUserData;
							pItemUserData = NULL;
						}
					}
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

			break;
		}

		hParent = GetParent(hParent);
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pRadCalls->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadCalls(pe);
	}
	else if (m_pRadMissed->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadMissed(pe);
		UpdateReadStatusMiss();
	}
	else if (m_pRadVoiceMails->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadVoiceMails(pe);
		UpdateReadStatusVoiceMail();
	}
	else if (m_pRadRecordings->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadRecordings(pe);
		UpdateReadStatusRecord();
	}
	else if (m_pRadMessages->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadMessages(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_AUDIO"))
	{
		OnItemBtnAudio(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_VIDEO"))
	{
		OnItemBtnVideo(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_MORE"))
	{
		OnItemBtnMore(pe);
	}
	else if (m_pBtnFirstPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnFirstPage(pe);
	}
	else if (m_pBtnPrePage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnPrePage(pe);
	}
	else if (m_pBtnNextPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNextPage(pe);
	}
	else if (m_pBtnLastPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnLastPage(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_CHK_START_PAUSE"))
	{
		OnBtnPlayPause(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_PLAY"))
	{
		OnBtnPlayVideo(pe);
	}
	return 0;
}
void CInboxCanvasHandler::UpdateReadStatusMiss()
{
	CSystemEx::GetInstance()->GetDBOprate()->updateMissReadStatus();
	m_pStaUnreadMissed->ShowWindow(SW_HIDE);
}
void CInboxCanvasHandler::UpdateReadStatusVoiceMail()
{
	CSystemEx::GetInstance()->GetDBOprate()->updateVoiceMailStatus();
	m_pStaUnreadVoicemail->ShowWindow(SW_HIDE);
}
void CInboxCanvasHandler::UpdateReadStatusRecord()
{
	CSystemEx::GetInstance()->GetDBOprate()->updateRecordStatus();
	m_pStaUnreadRecord->ShowWindow(SW_HIDE);
}
bool CInboxCanvasHandler::HaveUnReadMiss()
{
	bool bHave = false;
	auto iterMiss = m_vecMissedHistory.begin();
	for (iterMiss; iterMiss<m_vecMissedHistory.end();iterMiss++)
	{
		CCallHistory pCallHistory = *iterMiss;
		if (pCallHistory.m_bRead==false)
		{
			bHave = true;
			break;
		}
	}
	return bHave;
}
bool CInboxCanvasHandler::HaveUnReadVoiceMail()
{
	bool bHave = false;
	auto iterMiss = m_vecVoiceMail.begin();
	for (iterMiss; iterMiss < m_vecVoiceMail.end(); iterMiss++)
	{
		CVoiceMail *pCallHistory = *iterMiss;
		if (pCallHistory != NULL)
		{
			if (pCallHistory->m_bRead == false)
			{
				bHave = true;
				break;
			}
		}
	}
	return bHave;
}
bool CInboxCanvasHandler::HaveUnReadRecord()
{
	return false;
	bool bHave = false;
	auto iterMiss = m_vecVARecord.begin();
	for (iterMiss; iterMiss < m_vecVARecord.end(); iterMiss++)
	{
		CVARecord *pCallHistory = *iterMiss;
		if (pCallHistory != NULL)
		{
			if (pCallHistory->m_bRead == false)
			{
				bHave = true;
				break;
			}
		}
	}
	return bHave;
}
LRESULT CInboxCanvasHandler::OnBtnFirstPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_CALL_PAGE:
		m_nLocalCallIndex = 1;
		Swtich2CallPage();
		break;
	case ENUM_MISS_PAGE:
		m_nMissCallIndex = 1;
		Swtich2MissPage();
		break;
	case ENUM_VOICE_MAIL_PAGE:
		m_nVoiceMailIndex = 1;
		Swtich2VoiceMailPage();
		break;
	case ENUM_RECORD_PAGE:
		m_nRecordIndex = 1;
		Swtich2RecordPage();
		break;
	default:
		break;
	}

	return 0;
}

LRESULT CInboxCanvasHandler::OnBtnPrePage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_CALL_PAGE:
		m_nLocalCallIndex--;
		Swtich2CallPage();
		break;
	case ENUM_MISS_PAGE:
		m_nMissCallIndex--;
		Swtich2MissPage();
		break;
	case ENUM_VOICE_MAIL_PAGE:
		m_nVoiceMailIndex--;
		Swtich2VoiceMailPage();
		break;
	case ENUM_RECORD_PAGE:
		m_nRecordIndex--;
		Swtich2RecordPage();
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CInboxCanvasHandler::OnBtnNextPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_CALL_PAGE:
		m_nLocalCallIndex++;
		Swtich2CallPage();
		break;
	case ENUM_MISS_PAGE:
		m_nMissCallIndex++;
		Swtich2MissPage();
		break;
	case ENUM_VOICE_MAIL_PAGE:
		m_nVoiceMailIndex++;
		Swtich2VoiceMailPage();
		break;
	case ENUM_RECORD_PAGE:
		m_nRecordIndex++;
		Swtich2RecordPage();
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CInboxCanvasHandler::OnBtnLastPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_CALL_PAGE:
		m_nLocalCallIndex = m_nLocalCallIndexMax;
		Swtich2CallPage();
		break;
	case ENUM_MISS_PAGE:
		m_nMissCallIndex = m_nMissCallIndexMax;
		Swtich2MissPage();
		break;
	case ENUM_VOICE_MAIL_PAGE:
		m_nVoiceMailIndex = m_nVoiceMailIndexMax;
		Swtich2VoiceMailPage();
		break;
	case ENUM_RECORD_PAGE:
		m_nRecordIndex = m_nVoiceMailIndexMax;
		Swtich2RecordPage();
		break;
	default:
		break;
	}
	return 0;
}

void CInboxCanvasHandler::Swtich2CallPage(bool bTipsCheckSwitch)
{
	m_nLocalCallIndexMax = ceil((float)m_vecCallHistory.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_nLocalCallIndexMax == 0)
	{
		m_nLocalCallIndexMax = 1;
	}
	BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nLocalCallIndex,m_nLocalCallIndexMax, bTipsCheckSwitch);
	if (bCanSwitch==FALSE)
	{
		return;
	}
	m_pTvCalls = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderCallHistory);
	if (m_pTvCalls)
	{
		m_pTvCalls->DeleteAllItems();
	}
	CIUIProject *pProject = GetProject(m_hOriginalSenderCallHistory);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_CALLS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nLocalCallIndex-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecCallHistory.size())
		{
			break;
		}
		CCallHistory theLocalCallHistory = m_vecCallHistory.at(nDataPos);
		TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(theLocalCallHistory.m_strSIPNum);
		if (pContactInfo == NULL)
		{
			pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(theLocalCallHistory.m_strSIPNum);
		}
		CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
		ItemUserData *pItemUserData = new ItemUserData;
		pItemUserData->m_pContactInfo = pContactInfo;
		pItemUserData->m_nHistoryDataID = theLocalCallHistory.m_nCallHistoryID;
		HTREEITEM hItem = m_pTvCalls->InsertItem(_T(""));
		m_pTvCalls->SetItemData(hItem, (DWORD_PTR)pItemUserData);
		//IUI::SetProp(hItem, GETPROP_RECORD_ID, pLocalVoiceMail);
		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			m_pTvCalls->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderCallHistory), 0);
		m_pTvCalls->SetItemBindWindowless(hItem, hBind);
		HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
		_ASSERT(hIcon != NULL);
		if (theLocalCallHistory.m_enumCallType == ENUM_CALL_IN && theLocalCallHistory.m_enumCallStatus == ENUM_CALL_STATUS_SUCC)
		{
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callin_s.png"));
		}
		else if (theLocalCallHistory.m_enumCallType == ENUM_CALL_OUT && theLocalCallHistory.m_enumCallStatus == ENUM_CALL_STATUS_SUCC)
		{
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callout_s.png"));
		}
		else if (theLocalCallHistory.m_enumCallType == ENUM_CALL_OUT/* && theLocalCallHistory.m_enumCallStatus == ENUM_CALL_STATUS_FAILD*/)
		{
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callout_f.png"));
		}
		else if (theLocalCallHistory.m_enumCallType == ENUM_CALL_IN/* && theLocalCallHistory.m_enumCallStatus == ENUM_CALL_STATUS_FAILD*/)
		{
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callin_f.png"));
		}
		else
		{
			OutputDebugString(L"not normal");
		}
		HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
		_ASSERT(hFrom != NULL);
	
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContact);

		SetWindowText(hFrom, strSrcUserName);

		HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
		_ASSERT(hState != NULL);

		CIUIString strFormatTimeType;
		strFormatTimeType = FormatTimeType(theLocalCallHistory, theLocalCallHistory.m_enumCallStatus);
		SetWindowText(hState, strFormatTimeType);

		HWLWND hTime = IUI::FindControl(hBind, _T("IDC_STA_TIME"));
		_ASSERT(hTime != NULL);
		std::string strFormatTime;
		PortUtility::FormatDateTime(theLocalCallHistory.m_strBeginTime, strFormatTime,(ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLastLanguage);
		SetWindowText(hTime, PortUtility::string2WString(strFormatTime).c_str());
	}
}
void CInboxCanvasHandler::Swtich2MissPage(bool bTipsCheckSwitch)
{
	m_nMissCallIndexMax = ceil((float)m_vecMissedHistory.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_nMissCallIndexMax == 0)
	{
		m_nMissCallIndexMax = 1;
	}
	BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nMissCallIndex, m_nMissCallIndexMax, bTipsCheckSwitch);
	if (bCanSwitch == FALSE)
	{
		return;
	}
	m_pTvMissed = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderMissed);
	if (m_pTvMissed == NULL)
	{
		return;
	}
	m_pTvMissed->DeleteAllItems();
	CIUIProject *pProject = GetProject(m_hOriginalSenderMissed);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_CALLS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);
	std::vector<CCallHistory>::iterator it = m_vecMissedHistory.begin();
	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nMissCallIndex-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecMissedHistory.size())
		{
			break;
		}
		CCallHistory theLocalCallHistory = m_vecMissedHistory.at(nDataPos);
		if (theLocalCallHistory.m_enumCallStatus == ENUM_CALL_STATUS_FAILD)
		{
			TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(theLocalCallHistory.m_strSIPNum);
			if (pContactInfo == NULL)
			{
				pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(theLocalCallHistory.m_strSIPNum);
			}
			ItemUserData *pItemUserData = new ItemUserData;
			pItemUserData->m_pContactInfo = pContactInfo;
			pItemUserData->m_nHistoryDataID = theLocalCallHistory.m_nCallHistoryID;
			CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
			HTREEITEM hItem = m_pTvMissed->InsertItem(_T(""));
			m_pTvMissed->SetItemData(hItem, (DWORD_PTR)pItemUserData);

			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTvMissed->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderMissed), 0);
			m_pTvMissed->SetItemBindWindowless(hItem, hBind);

			HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hIcon != NULL);
			switch (theLocalCallHistory.m_enumCallType)
			{
			case ENUM_CALL_IN:
				SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callin_f.png"));
				break;
			default:
				SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_inbox_callout_f.png"));
				break;
			}
			
			HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
			_ASSERT(hFrom != NULL);
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContact);

			SetWindowText(hFrom, strSrcUserName);

			HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
			_ASSERT(hState != NULL);
			if (theLocalCallHistory.m_enumCallType == ENUM_CALL_OUT)
			{
				SetWindowText(hState,CSystemEx::GetInstance()->GetCallOutText());
			}
			else
			{
				SetWindowText(hState, CSystemEx::GetInstance()->GetCallInText());
			}

			HWLWND hTime = IUI::FindControl(hBind, _T("IDC_STA_TIME"));
			_ASSERT(hTime != NULL);
			std::string strFormatTime;
			PortUtility::FormatDateTime(theLocalCallHistory.m_strBeginTime, strFormatTime, (ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLastLanguage);
			SetWindowText(hTime, PortUtility::string2WString(strFormatTime).c_str());
		}
		else
		{
		}
	}
}
void CInboxCanvasHandler::Swtich2VoiceMailPage(bool bTipsCheckSwitch)
{
	m_nVoiceMailIndexMax = ceil((float)m_vecVoiceMail.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_vecVoiceMail.size() == 0)
	{
		return;
	}
	BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nVoiceMailIndex, m_nVoiceMailIndexMax, bTipsCheckSwitch);
	if (bCanSwitch == FALSE)
	{
		return;
	}
	if (m_vecVoiceMail.size() <= 0)
	{
		return;
	}
	
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderVoicemails);
	if (pTvSheet == NULL)
	{
		return;
	}
	pTvSheet->DeleteAllItems();
	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nVoiceMailIndex-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecVoiceMail.size())
		{
			break;
		}
		CVoiceMail *pLocalVoiceMail = m_vecVoiceMail.at(nDataPos);

		CIUIProject *pProject = GetProject(m_hOriginalSenderVoicemails);

		CControlProp *pProp = pProject->FindProjectItem(_T("IDW_INBOX_VOICEMAILS_ITEM.xml"));
		_ASSERT(pProp != NULL);
		CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);

		HTREEITEM hItem = pTvSheet->InsertItem(_T(""));
		pTvSheet->SetItemData(hItem, (DWORD_PTR)pLocalVoiceMail);

		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			pTvSheet->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderVoicemails), 0);
		pTvSheet->SetItemBindWindowless(hItem, hBind);

		HWLWND hStaIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
		_ASSERT(hStaIcon != NULL);
		if (pLocalVoiceMail->m_enumFileStatus == ENUM_MSG_STATUS_READ)
		{
			SetBkImage(hStaIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_voicemail_read.png"));
		}
		else
		{
			SetBkImage(hStaIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_voicemail_unread.png"));
		}
		HWLWND hPlayPause = IUI::FindControl(hBind, _T("IDC_CHK_START_PAUSE"));
		_ASSERT(hPlayPause != NULL);
		IUI::SetProp(hPlayPause, GETPROP_VOICE_MAIL, pLocalVoiceMail);
		IUI::SetProp(hPlayPause, GETPROP_PAGE, m_pRadVoiceMails->GetSafeHwnd());

		HWLWND hSldVolumn = IUI::FindControl(hBind, _T("IDC_SLD_VOLUMN"));
		_ASSERT(hSldVolumn != NULL);
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(hSldVolumn);
		pSlider->SetRange(0, 100);
		IUI::SetProp(hSldVolumn, GETPROP_VOICE_MAIL, pLocalVoiceMail);
		IUI::SetProp(hSldVolumn, GETPROP_PAGE, m_pRadVoiceMails->GetSafeHwnd());
		
		HWLWND hStaName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
		_ASSERT(hStaName != NULL);
		SetWindowText(hStaName, PortUtility::string2WString(pLocalVoiceMail->m_strCaller).c_str());

		HWLWND hStaDuration = IUI::FindControl(hBind, _T("IDC_STA_DURATION"));
		_ASSERT(hStaDuration != NULL);
		CIUIString strDuration = PortUtility::FormatPlayTime(pLocalVoiceMail->m_strPlayTime);
		SetWindowText(hStaDuration, strDuration);
		
		HWLWND hStaReceiveTime = IUI::FindControl(hBind, _T("IDC_STA_RECEIVE_TIME"));
		_ASSERT(hStaReceiveTime != NULL);
	
		
		std::string strFormatTime;
		PortUtility::FormatDateTime(pLocalVoiceMail->m_strTime, strFormatTime, (ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLastLanguage);
		
		CIUIString strInfo = PortUtility::string2WString(strFormatTime).c_str();
		CIUIString strTemplateInfo = strInfo;
		SetWindowText(hStaReceiveTime, strTemplateInfo);
	}

}
void CInboxCanvasHandler::Swtich2RecordPage(bool bTipsCheckSwitch)
{
	m_nRecordIndexMax = ceil((float)m_vecVARecord.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_vecVARecord.size() == 0)
	{
		return;
	}
	BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nRecordIndex, m_nRecordIndexMax, bTipsCheckSwitch);
	if (bCanSwitch == FALSE)
	{
		return;
	}
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderRecords);
	if (pTvSheet == NULL)
	{
		return;
	}
	pTvSheet->DeleteAllItems();

	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nRecordIndex-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecVARecord.size())
		{
			break;
		}
		CVARecord* pLocalRecord = m_vecVARecord.at(nDataPos);

		CControlProp *pAudioItemProp = g_pProject->FindProjectItem(_T("IDW_INBOX_VOICEMAILS_ITEM.xml"));
		_ASSERT(pAudioItemProp != NULL);
		CControlProp *pBindAudioProp = (CControlProp *)pAudioItemProp->GetChild(0);
		_ASSERT(pBindAudioProp != NULL);

		CControlProp *pVideoItemProp = g_pProject->FindProjectItem(_T("IDW_INBOX_RECORD_VIDEO_ITEM.xml"));
		_ASSERT(pVideoItemProp != NULL);
		CControlProp *pBindVideoProp = (CControlProp *)pVideoItemProp->GetChild(0);
		_ASSERT(pBindVideoProp != NULL);

		HTREEITEM hItem = pTvSheet->InsertItem(_T(""));
		pTvSheet->SetItemData(hItem, (DWORD_PTR)pLocalRecord);
		HWLWND hBind = NULL;
		if (pLocalRecord->m_enumVARecordType == ENUM_VA_RECORD_AUDIO)
		{
			hBind = CreatePreDefineChildControl(NULL, NULL, pBindAudioProp,
				pTvSheet->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderRecords), 0);

			HWLWND hStaIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hStaIcon != NULL);
			SetBkImage(hStaIcon, FALSE, CONTROL_STATE_NORMAL, _T("ic_recordings_voice.png"));

			HWLWND hPlayPause = IUI::FindControl(hBind, _T("IDC_CHK_START_PAUSE"));
			_ASSERT(hPlayPause != NULL);
			IUI::SetProp(hPlayPause, GETPROP_CONTACT, pLocalRecord);
			IUI::SetProp(hPlayPause, GETPROP_PAGE, m_pRadRecordings->GetSafeHwnd());

	
			HWLWND hSldVolumn = IUI::FindControl(hBind, _T("IDC_SLD_VOLUMN"));
			_ASSERT(hSldVolumn != NULL);

			Slider *pSlider = (Slider *)CWLWnd::FromHandle(hSldVolumn);
			pSlider->SetRange(0, 100);

			IUI::SetProp(hSldVolumn, GETPROP_CONTACT, pLocalRecord);
			IUI::SetProp(hSldVolumn, GETPROP_PAGE, m_pRadRecordings->GetSafeHwnd());
			IUI::SetProp(hSldVolumn, GETPROP_RECORD_FILE, pLocalRecord);
			HWLWND hStaDuration = IUI::FindControl(hBind, _T("IDC_STA_DURATION"));
			IUI::SetProp(hPlayPause, GETPROP_RECORD_FILE, pLocalRecord);
			
	
			_ASSERT(hStaDuration != NULL);

			CIUIString strDuration = PortUtility::FormatPlayTime(pLocalRecord->m_strPlayTime);
			SetWindowText(hStaDuration, strDuration);
		}
		else
		{
			hBind = CreatePreDefineChildControl(NULL, NULL, pBindVideoProp,
				pTvSheet->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderRecords), 0);

			HWLWND hPlay = IUI::FindControl(hBind, _T("IDC_BTN_PLAY"));
			_ASSERT(hPlay != NULL);
			IUI::SetProp(hPlay, GETPROP_CONTACT, pLocalRecord);
			IUI::SetProp(hPlay, GETPROP_PAGE, m_pRadRecordings->GetSafeHwnd());
			IUI::SetProp(hPlay, GETPROP_RECORD_FILE, pLocalRecord);

			HWLWND hStaDuration = IUI::FindControl(hBind, _T("IDC_STA_DURATION"));
			_ASSERT(hStaDuration != NULL);
		
			CIUIString strTotal = PortUtility::FormatPlayTime(pLocalRecord->m_strPlayTime);
			SetWindowText(hStaDuration, strTotal);
		}
		pTvSheet->SetItemBindWindowless(hItem, hBind);
		
		HWLWND hStaName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
		_ASSERT(hStaName != NULL);
		std::string strFilePath = pLocalRecord->m_strFileName;
		size_t nPos = strFilePath.rfind("//");
		std::string strFileName;
		if (nPos)
		{
			strFileName = strFilePath.substr(nPos + 2, strFilePath.length());
		}
		if (strFileName.empty())
		{
			LOG4_INFO("Data ERROR When Flush Record");
		}
		CIUIRect rcStaName;
		GetClientRect(hStaName, rcStaName);
		SetWindowText(hStaName, PortUtility::string2WString(strFileName).c_str());

		HWLWND hStaReceiveTime = IUI::FindControl(hBind, _T("IDC_STA_RECEIVE_TIME"));
		_ASSERT(hStaReceiveTime != NULL);
		std::string strFormatTime;
		PortUtility::FormatDateTime(pLocalRecord->m_strEndTime, strFormatTime,(ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLastLanguage);
		SetWindowText(hStaReceiveTime, PortUtility::string2WString(strFormatTime).c_str());
	}
}
void CInboxCanvasHandler::Insert2Call(CCallHistory oAddCallHistory)
{
	m_vecCallHistory.insert(m_vecCallHistory.begin(), oAddCallHistory);
	Swtich2CallPage(false);
}
void CInboxCanvasHandler::Insert2Miss(CCallHistory oAddMissCallHistory)
{
	m_vecMissedHistory.insert(m_vecMissedHistory.begin(), oAddMissCallHistory);
	Swtich2MissPage(false);
	RefreshMiss();
}
void CInboxCanvasHandler::Insert2VoiceMail(CVoiceMail *pVoiceMail)
{
	m_vecVoiceMail.insert(m_vecVoiceMail.begin(), pVoiceMail);
	Swtich2VoiceMailPage(false);
	RefreshVoicemail();
}
void CInboxCanvasHandler::Insert2Record(CVARecord *pRecord)
{
	m_vecVARecord.insert(m_vecVARecord.begin(), pRecord);
	Swtich2RecordPage(false);
	RefreshRecord();
}
void CInboxCanvasHandler::RefreshMiss()
{
	bool bHaveUnreadRecord = HaveUnReadMiss();
	if (bHaveUnreadRecord == true)
	{
		m_pStaUnreadMissed->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pStaUnreadMissed->ShowWindow(SW_HIDE);
	}
}
void CInboxCanvasHandler::RefreshVoicemail()
{
	bool bHaveUnreadRecord = HaveUnReadVoiceMail();
	if (bHaveUnreadRecord == true)
	{
		m_pStaUnreadVoicemail->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pStaUnreadVoicemail->ShowWindow(SW_HIDE);
	}
}
void CInboxCanvasHandler::RefreshRecord()
{
	bool bHaveUnreadRecord = HaveUnReadRecord();
	if (bHaveUnreadRecord == true)
	{
		m_pStaUnreadRecord->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pStaUnreadRecord->ShowWindow(SW_HIDE);
	}
}
void CInboxCanvasHandler::RefreshMsg()
{
}
void CInboxCanvasHandler::EraseItemInCallNoDB(long long nDataID)
{
	bool bFind = false;
	auto iterFindCall = m_vecCallHistory.begin();
	for (; iterFindCall != m_vecCallHistory.end();)
	{
		CCallHistory theLocalHistory = *iterFindCall;
		if (theLocalHistory.m_nCallHistoryID == nDataID)
		{
			iterFindCall = m_vecCallHistory.erase(iterFindCall);
			bFind = true;
			break;
		}
		else
		{
			iterFindCall++;
		}
	}
	if (bFind == false)
	{
		return;
	}
	Swtich2CallPage(m_nLocalCallIndex);
}
void  CInboxCanvasHandler::EraseItemInCall(long long nDataID)
{
	bool bFind = false;
	auto iterFindCall = m_vecCallHistory.begin();
	for (;iterFindCall!=m_vecCallHistory.end();)
	{
		CCallHistory theLocalHistory = *iterFindCall;
		if (theLocalHistory.m_nCallHistoryID==nDataID)
		{
			iterFindCall = m_vecCallHistory.erase(iterFindCall);
			bFind = true;
			break;
		}
		else
		{
			iterFindCall++;
		}
	}
	if (bFind==false)
	{
		return;
	}
	EraseItemInMissNoDB(nDataID);
	Swtich2CallPage(false);
	CSystemEx::GetInstance()->GetDBOprate()->DeleteCallMsg(nDataID);
	CSystemEx::GetInstance()->GetDBOprate()->deleteOneCallRecord(nDataID);
}
void CInboxCanvasHandler::RefreshCallInfo()
{
	Swtich2CallPage(false);
	Swtich2MissPage(false);
}

void CInboxCanvasHandler::EraseItemInMissNoDB(long long nDataID)
{
	bool bFind = false;
	auto iterFindMiss = m_vecMissedHistory.begin();
	for (; iterFindMiss != m_vecMissedHistory.end();)
	{
		CCallHistory theLocalHistory = *iterFindMiss;
		if (theLocalHistory.m_nCallHistoryID == nDataID)
		{
			iterFindMiss = m_vecMissedHistory.erase(iterFindMiss);
			bFind = true;
			break;
		}
		else
		{
			iterFindMiss++;
		}
	}
	if (bFind ==false)
	{
		return;
	}
	Swtich2CallPage(m_nMissCallIndex);
}
void  CInboxCanvasHandler::EraseItemInMiss(long long nDataID)
{
	bool bFind = false;
	auto iterFindMiss = m_vecMissedHistory.begin();
	for (; iterFindMiss != m_vecMissedHistory.end();)
	{
		CCallHistory theLocalHistory = *iterFindMiss;
		if (theLocalHistory.m_nCallHistoryID == nDataID)
		{
			iterFindMiss = m_vecMissedHistory.erase(iterFindMiss);
			bFind = true;
			break;
		}
		else
		{
			iterFindMiss++;
		}
	}
	if (bFind==false)
	{
		return;
	}
	EraseItemInCallNoDB(nDataID);
	Swtich2MissPage(false);
	CSystemEx::GetInstance()->GetDBOprate()->DeleteCallMsg(nDataID);
	CSystemEx::GetInstance()->GetDBOprate()->deleteOneCallRecord(nDataID);
}
void  CInboxCanvasHandler::EraseItemVoiceMail(long long nDataID)
{
	auto iterFindVoiceMail = m_vecVoiceMail.begin();
	for (; iterFindVoiceMail != m_vecVoiceMail.end();)
	{
		CVoiceMail *pLocalHistory = *iterFindVoiceMail;
		if (pLocalHistory->m_ID == nDataID)
		{
			iterFindVoiceMail = m_vecVoiceMail.erase(iterFindVoiceMail);
			delete pLocalHistory;
		}
		else
		{
			iterFindVoiceMail++;
		}
	}
	Swtich2CallPage(m_nVoiceMailIndex);
	CSystemEx::GetInstance()->GetDBOprate()->DeleteVoiceMail(nDataID);
}
void  CInboxCanvasHandler::EraseItemRecord(long long nDataID)
{
	auto iterFindRecord = m_vecVARecord.begin();
	for (; iterFindRecord != m_vecVARecord.end();)
	{
		CVARecord *pLocalRecord = *iterFindRecord;
		if (pLocalRecord->m_nCallHistoryID == nDataID)
		{
			iterFindRecord = m_vecVARecord.erase(iterFindRecord);
			delete pLocalRecord;
		}
		else
		{
			iterFindRecord++;
		}
	}
	Swtich2CallPage(m_nRecordIndex);
	CSystemEx::GetInstance()->GetDBOprate()->DeleteVoiceMail(nDataID);
}

void CInboxCanvasHandler::RefreshVARecord()
{
	if (m_hOriginalSenderRecords==NULL)
	{
	}
	else
	{
		DoFlushRecord(m_nRecordIndex);
	}
}
void CInboxCanvasHandler::DoFlushRecord(int nIndex)
{
	InitRecord();
	Swtich2RecordPage(false);
}
LRESULT CInboxCanvasHandler::OnBtnPlayPause(RoutedEventArgs *pe)
{
	if (NULL == pe || NULL == pe->hOriginalSender)
	{
		return -1;
	}

	Button *pBtnPlayPause = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	CVoiceMail *pNowVoiceMail = (CVoiceMail *)IUI::GetPropW(pe->hOriginalSender, GETPROP_VOICE_MAIL);
	CVARecord *pVARecord = (CVARecord*)IUI::GetProp(pe->hOriginalSender, GETPROP_RECORD_FILE);
	HWLWND hPage = (HWLWND)IUI::GetProp(pe->hOriginalSender, GETPROP_PAGE);
	bool bIsPalyerPlay = CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay();
	BOOL bChecked = pBtnPlayPause->GetCheck()==BST_CHECKED?true:false;
	m_hCurrentPlayingButton = pe->hOriginalSender;
	if (bChecked)
	{
		if (NULL != m_hCurrentPlayingButton)
		{
			Button *pBtnPlaying = (Button *)CWLWnd::FromHandle(m_hCurrentPlayingButton);
			CVoiceMail *pLastVoiceMail = (CVoiceMail *)IUI::GetPropW(m_hCurrentPlayingButton , GETPROP_VOICE_MAIL);
			HWLWND hPage = (HWLWND)IUI::GetProp(m_hCurrentPlayingButton, GETPROP_PAGE);

			if (m_hCurrentPlayingButton == m_hLastPlayButton&& bIsPalyerPlay == true)
			{
				if (STATE_PAUSE== CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState())
				{
					pBtnPlaying->SetCheck(BST_CHECKED);
				}
				else if(STATE_PLAY == CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState())
				{
					pBtnPlaying->SetCheck(BST_UNCHECKED);
				}
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Pause();
				return 0;
			}
			else
			{
				Button *pLastBtnPlaying = (Button *)CWLWnd::FromHandle(m_hLastPlayButton);
				if (pLastBtnPlaying!=NULL)
				{
					pLastBtnPlaying->SetCheck(BST_UNCHECKED);
				}
				
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Stop();
			}
		}	
	}
	
	if (hPage == m_pRadVoiceMails->GetSafeHwnd())
	{
		HWLWND hDuration = FindControl(GetParent(pe->hOriginalSender), _T("IDC_STA_DURATION"));
		m_hStaDuration = hDuration;
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(FindControl(GetParent(pe->hOriginalSender), _T("IDC_SLD_VOLUMN")));
		m_pSldDuration = pSlider;
		_ASSERT(pSlider != NULL);
		Button *pBtnPlaying = (Button *)CWLWnd::FromHandle(m_hCurrentPlayingButton);
		m_pBtnPlaying = pBtnPlaying;

		if (bChecked)
		{
			std::string strFilePath = CSystemEx::GetInstance()->GetVoiceMailDownLoadPath();
			std::string templateFilePath = strFilePath + pNowVoiceMail->m_strFileName;
			if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetIsInit()==false)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init(ENUM_AUDIO);
			}
			
			bool bRet = CSystemEx::GetInstance()->GetGlobalAVPlayer()->PlayByWLWnd(templateFilePath, GetBindWLWnd(), NORMAL_VOLUME,0);
			if (bRet==false)
			{
				return 0;
			}
			int nPos = 0;
			if (pSlider != NULL)
			{
				nPos = pSlider->GetPos();
				if (nPos>=100)
				{
					nPos = 0;
				}
			}
			if (nPos != 0)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
			}

			pBtnPlaying->SetCheck(BST_CHECKED);
			m_hLastPlayButton = m_hCurrentPlayingButton;
		}
		else
		{
			if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay() == true)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Pause();
				pBtnPlaying->SetCheck(BST_UNCHECKED);
			}
		}
	}
	
	else if (hPage == m_pRadRecordings->GetSafeHwnd())
	{
		HWLWND hDuration = FindControl(GetParent(pe->hOriginalSender), _T("IDC_STA_DURATION"));
		m_hStaDuration = hDuration;
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(FindControl(GetParent(pe->hOriginalSender), _T("IDC_SLD_VOLUMN")));
		m_pSldDuration = pSlider;
		_ASSERT(pSlider != NULL);
		Button *pBtnPlaying = (Button *)CWLWnd::FromHandle(m_hCurrentPlayingButton);
		m_pBtnPlaying = pBtnPlaying;

		if (bChecked)
		{
			if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetIsInit() == false)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init(ENUM_AUDIO);
			}
			if (pVARecord != NULL)
			{
				int nPos = 0;
				//CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(100);
				bool bRet =CSystemEx::GetInstance()->GetGlobalAVPlayer()->PlayByWLWnd(pVARecord->m_strFileName, (GetBindWLWnd()), NORMAL_VOLUME, 0);
				if (bRet==false)
				{
					return 0;
				}
				if (pSlider != NULL)
				{
					nPos = pSlider->GetPos();
					if (nPos >= 100)
					{
						nPos = 0;
					}
				}
				if (nPos!=0)
				{
					CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
				}
				pBtnPlaying->SetCheck(BST_CHECKED);
				m_hLastPlayButton = m_hCurrentPlayingButton;
			}
		}
		else
		{
			if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay() == true)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->Pause();
				pBtnPlaying->SetCheck(BST_UNCHECKED);
			}
		}
	}

	return 0;
}
LRESULT CInboxCanvasHandler::OnHScroll(RoutedEventArgs *pe)
{
	if (NULL == pe)
	{
		return -1;
	}

	if (GetClassName(pe->hOriginalSender) == IUI_SLIDER)
	{
		if (LOWORD(pe->wParam) == SB_ENDSCROLL)
		{
			Slider *pSlider = (Slider *)CWLWnd::FromHandle(pe->hOriginalSender);
			CVoiceMail *pVoiceMail = (CVoiceMail *)IUI::GetProp(pe->hOriginalSender, GETPROP_VOICE_MAIL);
			CVARecord  *pRecord = (CVARecord *)IUI::GetProp(pe->hOriginalSender, GETPROP_RECORD_FILE);
			HWLWND hPage = (HWLWND)IUI::GetProp(pe->hOriginalSender, GETPROP_PAGE);
			if ((NULL != pVoiceMail && NULL != hPage) || pRecord != NULL)
			{
				int nPos = pSlider->GetPos();
				if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
				{
					std::string strPlayerFileName = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayFilePath();
					std::string strUIPlayerFileName = pVoiceMail == NULL ?  pRecord->m_strFileName:pVoiceMail->m_strFileName  ;
					if (PortUtility::IsContainsStr(strPlayerFileName, strUIPlayerFileName) == true)
					{
						CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
					}
					if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PAUSE)
					{
						CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnContinue();
					}
				}
			}
		}
		else if (LOWORD(pe->wParam) == SB_THUMBTRACK)
		{
			if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState()== STATE_PLAY)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnlyPause();
			}
		}
	}
	return 0;
}

LRESULT CInboxCanvasHandler::OnBtnPlayVideo(RoutedEventArgs *pe)
{
	if (NULL == pe || NULL == pe->hOriginalSender)
	{
		return -1;
	}

	Button *pBtnPlay = (Button *)CWLWnd::FromHandle(pe->hOriginalSender);
	CContact *pContact = (CContact *)IUI::GetProp(pe->hOriginalSender, GETPROP_CONTACT);
	HWLWND hPage = (HWLWND)IUI::GetProp(pe->hOriginalSender, GETPROP_PAGE);

	if (hPage == m_pRadRecordings->GetSafeHwnd())
	{
		CVARecord *pVARecord = (CVARecord*)IUI::GetProp(pe->hOriginalSender, GETPROP_RECORD_FILE);
		if (pVARecord!=NULL)
		{
			CIUIString strFilePath, strFileName,strTotalPlayTime;
			auto nSize = pVARecord->m_strFileName.rfind("//");
			if (nSize< pVARecord->m_strFileName.length())
			{
				string strPath = pVARecord->m_strFileName.substr(0, nSize+2);
				string strName = pVARecord->m_strFileName.substr(nSize+2, pVARecord->m_strFileName.length());
				strFilePath = PortUtility::Utf82Unicode_ND(strPath).c_str();
				strFileName = PortUtility::Utf82Unicode_ND(strName).c_str();
				strTotalPlayTime = PortUtility::Utf82Unicode_ND(pVARecord->m_strPlayTime).c_str();

				CIUIString strSrcFileName = strFilePath + strFileName;


				HBITMAP hBmp;
				PortUtility::GetShellThumbnailImage(strSrcFileName, &hBmp);

				if (hBmp == NULL)
				{
					hBmp = IUIImage::LoadHBITMAP(GetRunFolder() + _T("Skins\\PortGo\\VideoThumbnial.bmp"));
				}
				PopupVideoWnd(hBmp,GetHostWnd(GetBindWLWnd()), strFilePath, strTotalPlayTime, strFileName);
			}
			else
			{

			}
			
		}
	
	}

	return 0;
}

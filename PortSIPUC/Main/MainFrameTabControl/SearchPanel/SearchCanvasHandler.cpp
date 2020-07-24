#include "stdafx.h"
#include "SearchCanvasHandler.h"
#include "System/SystemEx.h"

CSearchCanvasHandler::CSearchCanvasHandler()
	: m_pStaSearchTitle(NULL)
	, m_pBtnCloseSearchPage(NULL)
	, m_pReSearch(NULL)
	, m_pTreeSerachResult(NULL)
{
}

CSearchCanvasHandler::~CSearchCanvasHandler()
{
}

int CSearchCanvasHandler::InitControls()
{
	m_pStaSearchTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SEARCH_TITLE")));
	_ASSERT(m_pStaSearchTitle != NULL);
	m_pBtnCloseSearchPage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_CLOSE_SEARCH_PAGE")));
	_ASSERT(m_pBtnCloseSearchPage != NULL);
	m_pReSearch = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_SEARCH")));
	_ASSERT(m_pReSearch != NULL);
	m_pTreeSerachResult = (TreeView *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_TREECTRL_SERACH_RESULT")));
	_ASSERT(m_pTreeSerachResult != NULL);

	return 0;
}

int CSearchCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	m_pReSearch->SetEventMask(m_pReSearch->GetEventMask() | ENM_CHANGE);

	UpdateSearchResult(NULL);

	return 0;
}

int CSearchCanvasHandler::UpdateSearchResult(LPCTSTR lpszSearchText)
{
	SetULWNoRedraw(GetHostWnd(GetBindWLWnd()), TRUE);

	m_pTreeSerachResult->DeleteAllItems();

	CIUIProject *pProject = GetProject(GetBindWLWnd());
	CControlProp *pContactProp = pProject->FindProjectItem(_T("IDW_SEARCH_CONTACTS_ITEM.xml"));
	_ASSERT(pContactProp != NULL);
	CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
	_ASSERT(pBindContactProp != NULL);

	CControlProp *pMessageProp = pProject->FindProjectItem(_T("IDW_SEARCH_MESSAGE_ITEM.xml"));
	_ASSERT(pMessageProp != NULL);
	CControlProp *pBindMessageProp = (CControlProp *)pMessageProp->GetChild(0);
	_ASSERT(pBindMessageProp != NULL);

	//
	CIUIString strSearchText = lpszSearchText;
	if (strSearchText.IsEmpty())
	{
		HTREEITEM hFrequentContactsRootItem = m_pTreeSerachResult->InsertItem(
				_T("FREQUENT CONTACTS"));
		vector<TAG_RECENT*> vecContact; 
		CSystemEx::GetInstance()->GetMainDlg()->SearchFromContact(vecContact, "");
		auto iterContact = vecContact.begin();
		for (iterContact;iterContact!=vecContact.end();iterContact++)
		{
			TAG_RECENT *pContactInfo = *iterContact;
			InsertContactItem(hFrequentContactsRootItem, pBindContactProp, pContactInfo);	
		}
		m_pTreeSerachResult->Expand(hFrequentContactsRootItem, TVE_EXPAND);
	}
	else
	{
		SetULWNoRedraw(GetHostWnd(GetBindWLWnd()), TRUE);

		HTREEITEM hPersonalContactsRootItem = m_pTreeSerachResult->InsertItem(
				_T("PERSONAL CONTACTS"));
		if (hPersonalContactsRootItem != NULL)
		{
			vector<TAG_RECENT*> vecContact;
			string tempKeyString = PortUtility::Unicode2Utf8_ND(lpszSearchText);
			CSystemEx::GetInstance()->GetMainDlg()->SearchFromContact(vecContact, tempKeyString);
			auto iterContact = vecContact.begin();
			for (iterContact; iterContact != vecContact.end(); iterContact++)
			{
				TAG_RECENT *pContactInfo = *iterContact;
				
				InsertContactItem(hPersonalContactsRootItem, pBindContactProp, pContactInfo);
				
			}
			m_pTreeSerachResult->Expand(hPersonalContactsRootItem, TVE_EXPAND);
			
		}
		HTREEITEM hMessagesRootItem = m_pTreeSerachResult->InsertItem(_T("MESSAGES"));
		if (hMessagesRootItem != NULL)
		{
			InsertMessageItem(hMessagesRootItem, pBindMessageProp, _T("ic_inbox_callin.png"),
				_T("JIM'S UBERCONFERENCE"),	_T("THU AUG 9"),
				_T("Welcome! You can now start an UberConference directly from PortGo to meet and collaborate with anyone."));

			m_pTreeSerachResult->Expand(hMessagesRootItem, TVE_EXPAND);
		}
	}

	SetULWNoRedraw(GetHostWnd(GetBindWLWnd()), FALSE);
	Invalidate(GetBindWLWnd());

	return 0;
}

int CSearchCanvasHandler::InsertContactItem(
	HTREEITEM hParent,
	CControlProp *pBindProp,
	TAG_RECENT * pContactInfo)
{
	if (NULL == hParent || NULL == pBindProp || NULL == pContactInfo)
	{
		return -1;
	}
	CIUIString strHeadImage, strUserName, strSipNumber;
	ENUM_RECENT_CONTACT_TYPE enumRecentContactType = pContactInfo->m_enumRecentContact;
	if (enumRecentContactType==ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
		FormatItemContact(strUserName, strHeadImage, strSipNumber, pContact);
	}
	

	HTREEITEM hItem = m_pTreeSerachResult->InsertItem(_T(""), hParent);
	m_pTreeSerachResult->SetItemData(hItem, (DWORD_PTR)pContactInfo);

	HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			m_pTreeSerachResult->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
	m_pTreeSerachResult->SetItemBindWindowless(hItem, hBind);

	HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
	_ASSERT(hIcon != NULL);
	SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, strHeadImage);

	HWLWND hStaName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
	_ASSERT(hStaName != NULL);
	SetWindowText(hStaName, strUserName);
	
	HWLWND hStaPhone = IUI::FindControl(hBind, _T("IDC_STA_PHONE"));
	_ASSERT(hStaPhone != NULL);
	SetWindowText(hStaPhone, strSipNumber);

	HWLWND hStaState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
	_ASSERT(hStaState != NULL);

	return 0;
}

int CSearchCanvasHandler::InsertMessageItem(
	HTREEITEM hParent,
	CControlProp *pBindProp,
	LPCTSTR lpszIconFile,
	LPCTSTR lpszMsgTitle,
	LPCTSTR lpszMsgTime,
	LPCTSTR lpszMsgContent)
{
	HTREEITEM hItem = m_pTreeSerachResult->InsertItem(_T(""), hParent);
	m_pTreeSerachResult->SetItemHeightEx(hItem, 145);

	HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			m_pTreeSerachResult->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
	m_pTreeSerachResult->SetItemBindWindowless(hItem, hBind);

	HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
	_ASSERT(hIcon != NULL);
	SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, lpszIconFile);

	HWLWND hStaMsgTitle = IUI::FindControl(hBind, _T("IDC_STA_MSG_TITLE"));
	_ASSERT(hStaMsgTitle != NULL);
	SetWindowText(hStaMsgTitle, lpszMsgTitle);

	HWLWND hStaMsgTime = IUI::FindControl(hBind, _T("IDC_STA_MSG_TIME"));
	_ASSERT(hStaMsgTime != NULL);
	SetWindowText(hStaMsgTime, lpszMsgTime);

	HWLWND hStaMsgContent = IUI::FindControl(hBind, _T("IDC_STA_MSG_CONTENT"));
	_ASSERT(hStaMsgContent != NULL);
	SetWindowText(hStaMsgContent, lpszMsgContent);

	return 0;
}

LRESULT CSearchCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (WM_COMMAND == pe->RoutedEvent)
		{
			OnCommand(pe);
		}
		else if (UE_AFTERPAINTBKIMAGE == pe->RoutedEvent)
		{
			OnAfterPaintBkImage(pe);
		}
		else if (TVN_HOTCHANGING == pe->RoutedEvent)
		{
			OnTreeViewHotChanging(pe);
		}
		else if (TVN_HOTCHANGED == pe->RoutedEvent)
		{
			OnTreeViewHotChanged(pe);
		}
	}
	else if (RS_TUNNEL == pe->eRoutingStrategy)
	{
		if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			OnLButtonDblClk(pe);
		}
	}

	return 0;
}

LRESULT CSearchCanvasHandler::OnBtnCloseSearchPage(RoutedEventArgs *pe)
{
	return 0;
}
LRESULT CSearchCanvasHandler::OnTreeViewHotChanging(RoutedEventArgs *pe)
{
	if (m_pTreeSerachResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		HTREEITEM hNewHot = (HTREEITEM)pe->lParam;
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
LRESULT CSearchCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnCloseSearchPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnCloseSearchPage(pe);
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

	return 0;
}

LRESULT CSearchCanvasHandler::OnCommand(RoutedEventArgs *pe)
{
	if (m_pReSearch->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (EN_CHANGE == HIWORD(pe->wParam))
		{
			CIUIString strNewText = m_pReSearch->GetWindowText();
			UpdateSearchResult(strNewText);
		}
	}

	return 0;
}

LRESULT CSearchCanvasHandler::OnAfterPaintBkImage(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_RE_SEARCH")
		&& GetClassName(pe->hOriginalSender) == IUI_RICHTEXTBOX)
	{
		CIUIString strText = GetWindowText(pe->hOriginalSender);
		if (strText.IsEmpty())
		{
			HDC hDC = (HDC)pe->lParam;
			CIUIRect rcClient;
			IUI::GetClientRect(pe->hOriginalSender, rcClient);

			RECT rcTextPadding = GetTextPadding(pe->hOriginalSender);

			CIUIProject *pProject = GetProject(pe->hOriginalSender);
			HFONT hFont = FontManager::CreateIUIFont(pProject, _T("IDF_YAHEI_START_CONNECT"));
			COLORREF crText = pProject->GetColor(_T("IDR_COLOR_ADVANCE_N"));
			_ASSERT(hFont != NULL);
			CIUIString strPrompt = _T("Search");
			DrawFormatText(hDC, strPrompt, strPrompt.GetLength(),
				rcClient, &rcTextPadding,
				TAH_CENTER, TAV_TOP, DT_SINGLELINE, hFont,
				crText, FALSE);
		}
	}

	return 0;
}

LRESULT CSearchCanvasHandler::OnTreeViewHotChanged(RoutedEventArgs *pe)
{
	if (m_pTreeSerachResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		if (NULL != hOldHot)
		{
			HWLWND hBind = m_pTreeSerachResult->GetItemBindWindowless(hOldHot);

			if (NULL != hBind)
			{
				DWORD_PTR dwItemData = m_pTreeSerachResult->GetItemData(hOldHot);

				if (0 != dwItemData)
				{
					HWLWND hAudio = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
					_ASSERT(NULL != hAudio);
					ShowWindow(hAudio, SW_HIDE);

					HWLWND hBtnVideo = IUI::FindControl(hBind, _T("IDC_BTN_VIDEO"));
					_ASSERT(NULL != hBtnVideo);
					ShowWindow(hBtnVideo, SW_HIDE);

					HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
					_ASSERT(NULL != hBtnMore);
					ShowWindow(hBtnMore, SW_HIDE);
				}

				Invalidate(m_pTreeSerachResult->GetSafeHwnd());
			}
		}

		HTREEITEM hHot = (HTREEITEM)pe->lParam;
		if (NULL != hHot)
		{
			HWLWND hBind = m_pTreeSerachResult->GetItemBindWindowless(hHot);

			if (NULL != hBind)
			{
				DWORD_PTR dwItemData = m_pTreeSerachResult->GetItemData(hHot);

				if (0 != dwItemData)
				{
					HWLWND hAudio = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
					_ASSERT(NULL != hAudio);
					ShowWindow(hAudio, SW_SHOW);

					HWLWND hBtnVideo = IUI::FindControl(hBind, _T("IDC_BTN_VIDEO"));
					_ASSERT(NULL != hBtnVideo);
					ShowWindow(hBtnVideo, SW_SHOW);

					HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
					_ASSERT(NULL != hBtnMore);
					ShowWindow(hBtnMore, SW_HIDE);
				}
			}
		}
	}

	return 0;
}

LRESULT CSearchCanvasHandler::OnLButtonDblClk(RoutedEventArgs *pe)
{
	if (m_pTreeSerachResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		CIUIPoint pt(pe->lParam);
		HTREEITEM hItem = m_pTreeSerachResult->HitTest(pt);
		if (NULL != hItem)
		{
			TAG_RECENT *pContactInfo = (TAG_RECENT *)m_pTreeSerachResult->GetItemData(hItem);
			if (pContactInfo == NULL)
			{
				return 0;
			}
			CContactDB *pContact = NULL;
			ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
			if (localRecentContactType == ENUM_SINGLE_CONTACT)
			{
				if (pContactInfo)
					pContact = pContactInfo->m_unionContactInfo.m_pContact;

				if (NULL != pContact)
				{
					pContact->SetCallType(ENUM_CALL_OUT);
				}
				if (pContactInfo)
				{
					SwitchToContact(pContactInfo, CT_TEXT_CHAT);
				}
			}
			
		}
	}

	return 0;
}

LRESULT CSearchCanvasHandler::OnItemBtnAudio(RoutedEventArgs *pe)
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

				TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
			
				if (NULL != pContactInfo)
				{
					CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;

					pContact->SetCallType(ENUM_CALL_OUT);
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

LRESULT CSearchCanvasHandler::OnItemBtnVideo(RoutedEventArgs *pe)
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

				TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
				if (NULL != pContactInfo)
				{
					CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
					pContact->SetCallType(ENUM_CALL_OUT);
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

LRESULT CSearchCanvasHandler::OnItemBtnMore(RoutedEventArgs *pe)
{
	return 0;
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

				CContact *pContact = (CContact *)pTreeView->GetItemData(hItem);
				if (NULL != pContact)
				{

					CIUIRect rcMore;
					IUI::GetWindowRect(pe->hOriginalSender, rcMore);

					HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MORE));
					HMENU hMoreMenu = GetSubMenu(hMenu, 0);
					_ASSERT(NULL != hMoreMenu);
					UINT uRet = ::TrackPopupMenu(hMoreMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
						rcMore.left, rcMore.bottom, 0, GetHostWnd(GetBindWLWnd()), rcMore);

					if (ID_MORE_FORWARD == uRet)
					{
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

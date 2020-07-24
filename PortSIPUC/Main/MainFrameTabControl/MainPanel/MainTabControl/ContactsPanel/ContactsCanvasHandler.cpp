#include "stdafx.h"
#include "ContactsCanvasHandler.h"
#include "system/SystemEx.h"
#include "../../AddContactWnd.h"
CContactsCanvasHandler::CContactsCanvasHandler()
	: m_pRadFavorites(NULL)
	, m_pRadLocalContacts(NULL)
	, m_pRadRemoteContacts(NULL)
	, m_pRadExtensions(NULL)
	, m_pContacts(NULL)
	, m_pStaSplitter(NULL)

	, m_pTvFavorites(NULL)
	, m_pTvLocalContacts(NULL)
	, m_pTvRemoteContacts(NULL)
	, m_pTvExtensions(NULL)
	, m_pBtnFirstPage(NULL)
	, m_pBtnPrePage(NULL)
	, m_pBtnNextPage(NULL)
	, m_pBtnLastPage(NULL)
{
	 m_hOriginalSenderFavorites = NULL;
	 m_hOriginalSenderLocal = NULL;
	 m_hOriginalSenderRemote = NULL;
	 m_hOriginalSenderExtensions = NULL;
	
	 m_nFavoritePage		= 1;
	 m_nLocalContactPage	= 1; 
	 m_nRemoteContactPage	= 1;
	 m_nRemoteExtensionPage	= 1;

	 m_nFavoritePageMax			= 0;
	 m_nLocalContactPageMax		= 0;
	 m_nRemoteContactPageMax	= 0;
	 m_nRemoteExtensionPageMax	= 0;

	 m_nTabPage = ENUM_FAVORITE_PAGE;
	 m_nLastTabPage = ENUM_FAVORITE_PAGE;
}

CContactsCanvasHandler::~CContactsCanvasHandler()
{
}

int CContactsCanvasHandler::InitControls()
{
	m_pRadFavorites = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_FAVORITES")));
	_ASSERT(m_pRadFavorites != NULL);
	m_pRadLocalContacts = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_LOCAL_CONTACTS")));
	_ASSERT(m_pRadLocalContacts != NULL);
	m_pRadRemoteContacts = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_REMOTE_CONTACTS")));
	_ASSERT(m_pRadRemoteContacts != NULL);
	m_pRadExtensions = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_EXTENSIONS")));
	_ASSERT(m_pRadExtensions != NULL);
	m_pContacts = (TabControl *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PNL_TAB_CONTACTS")));
	_ASSERT(m_pContacts != NULL);
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
	return 0;
}

int CContactsCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	m_pContacts->AddPropertySheet(m_pRadFavorites->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pContacts->AddPropertySheet(m_pRadLocalContacts->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pContacts->AddPropertySheet(m_pRadRemoteContacts->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pContacts->AddPropertySheet(m_pRadExtensions->GetSafeHwnd(), _T("IDW_INBOX_PAGE.xml"), NULL);
	m_pContacts->SwitchPropertySheet(m_pRadFavorites->GetSafeHwnd());

	InitContacts();
	InitFavorites();
	InitExtersions();
	InitRemoteContacts(); 
	return 0;
}

/*
void CContactsCanvasHandler::UpdateContacts()
{
	if (m_pTvLocalContacts!=NULL&& m_hOriginalSenderLocal!=NULL)
	{
		m_pTvLocalContacts->DeleteAllItems();
		CSystemEx::GetInstance()->GetMainDlg()->UpdateContacts();
		std::vector<CContactDB*> vecContact = CSystemEx::GetInstance()->GetMainDlg()->GetContacts();
		if (vecContact.size() > 0)
		{
			InitContacts();
		}
	}
}*/
void CContactsCanvasHandler::UpdateContactByContact(CContactDB* pConactDB)
{
	auto iterFindLocalContact = m_vecDBLocalContact.begin();
	for (; iterFindLocalContact != m_vecDBLocalContact.end();iterFindLocalContact++)
	{
		TAG_RECENT *pContactInfo = *iterFindLocalContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (PortUtility::compareStringNoCase(pConactDB->m_strSIPNum, pLocalContact->m_strSIPNum) == 0)
			{
				pLocalContact->CopyProfileMemData(pConactDB);
			}
		}	
	}
}
void CContactsCanvasHandler::UpdateFavoriteByContact(CContactDB* pConactDB)
{
	auto iterFindLocalContact = m_vecFavoriteContact.begin();
	for (; iterFindLocalContact != m_vecFavoriteContact.end(); iterFindLocalContact++)
	{
		TAG_RECENT *pContactInfo = *iterFindLocalContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (PortUtility::compareStringNoCase(pConactDB->m_strSIPNum, pLocalContact->m_strSIPNum) == 0)
			{
				pLocalContact->CopyProfileMemData(pConactDB);
			}
		}
	}
}
void CContactsCanvasHandler::UpdateFavorites()
{
	Swtich2Favorite(FALSE, FALSE);
}
void CContactsCanvasHandler::UpdateContacts()
{
	Swtich2Contact(FALSE, FALSE);
}
void CContactsCanvasHandler::UpdateExtersion()
{
	Swtich2Extension(FALSE, FALSE);
}
void CContactsCanvasHandler::UpdateRemote()
{
	Swtich2RemoteContact(FALSE, FALSE);
}
void CContactsCanvasHandler::RefreshLocalContactOnlineStatus(CContactDB* pContactDB)
{
	if (m_pTvLocalContacts==NULL||pContactDB==NULL)
	{
		return;
	}
	if (m_pTvLocalContacts == NULL)
		return;
	HTREEITEM hVisiable = m_pTvLocalContacts->GetFirstVisibleItem();
	if (hVisiable)
	{
		TAG_RECENT *pContactInfo = (TAG_RECENT *)m_pTvLocalContacts->GetItemData(hVisiable);
		if (pContactInfo != NULL)
		{
			if (!pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT&&pContactInfo->m_unionContactInfo.m_pContact!=NULL)
			{
				if (PortUtility::compareStringNoCase(pContactDB->m_strSIPNum, pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
				{
					::Invalidate(GetBindWLWnd());
					return;
				}
			}
			
		}
		hVisiable = m_pTvLocalContacts->GetNextVisibleItem(hVisiable);
		while (hVisiable != NULL)
		{
			TAG_RECENT * pTreeContact = (TAG_RECENT *)m_pTvLocalContacts->GetItemData(hVisiable);
			if (pTreeContact != NULL)
			{
				if (!pContactInfo->m_enumRecentContact == ENUM_SINGLE_CONTACT && pTreeContact->m_unionContactInfo.m_pContact != NULL)
				{
					if (PortUtility::compareStringNoCase(pContactDB->m_strSIPNum, pTreeContact->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
					{
						::Invalidate(GetBindWLWnd());
						return;
					}
				}
			}
			hVisiable = m_pTvLocalContacts->GetNextVisibleItem(hVisiable);
		}
		OutputDebugString(_T("end find\n"));
	}
	else
	{
		return;
	}
}
void CContactsCanvasHandler::InitContacts()
{
	m_vecDBLocalContact = CSystemEx::GetInstance()->GetMainDlg()->GetDBContact();
	m_nLocalContactPageMax = ceil((float)m_vecDBLocalContact.size() / (float)MAX_CALL_VIEW_NUM);
}

void CContactsCanvasHandler::InitFavorites()
{
	m_vecFavoriteContact = CSystemEx::GetInstance()->GetMainDlg()->GetFavorite();
	m_nFavoritePageMax = ceil((float)m_vecFavoriteContact.size() / (float)MAX_CALL_VIEW_NUM);
	Swtich2Favorite(FALSE,FALSE);
}
void CContactsCanvasHandler::InitExtersions()
{
	m_vecExtensionsContact = CSystemEx::GetInstance()->GetMainDlg()->GetExtension();
	m_nRemoteExtensionPageMax = ceil((float)m_vecExtensionsContact.size() / (float)MAX_CALL_VIEW_NUM);
}

void CContactsCanvasHandler::InitRemoteContacts()
{
	m_vecRemoteContact = CSystemEx::GetInstance()->GetMainDlg()->GetRemoteContact();
	m_nRemoteContactPageMax = ceil((float)m_vecRemoteContact.size() / (float)MAX_CALL_VIEW_NUM);

}
void  CContactsCanvasHandler::GetAllRemoteContactsChildNodes()
{
	HTREEITEM hRoot = m_pTvRemoteContacts->GetRootItem();
	if (hRoot == NULL)
	{
		return;
	}
	vector<CContactDB*> vecRemoteContact;
	if (m_pTvRemoteContacts->ItemHasChildren(hRoot))
	{
		SearchRemoteContactsFromTree(hRoot, vecRemoteContact);
	}
	vector<CContactDB*>::iterator iterRemoteContact = vecRemoteContact.begin();
	for (iterRemoteContact;iterRemoteContact!=vecRemoteContact.end();iterRemoteContact++)
	{
		
	}

}
void CContactsCanvasHandler::SearchRemoteContactsFromTree(HTREEITEM hParent, vector<CContactDB *> &vecContact)
{
	CString strText = _T("");

	HTREEITEM hChild = NULL;
	hChild = m_pTvRemoteContacts->GetChildItem(hParent);
	CContactDB *pContact = NULL;
	while (hChild != NULL)
	{
		pContact =(CContactDB*) m_pTvRemoteContacts->GetItemData(hChild);
		vecContact.push_back(pContact);
		if (m_pTvRemoteContacts->ItemHasChildren(hChild))
		{
			SearchRemoteContactsFromTree(hChild, vecContact);
		}

		hChild = m_pTvRemoteContacts->GetNextItem(hChild, TVGN_NEXT);
	}
}
int CContactsCanvasHandler::InitContactsFavorites(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->SetContactCanvasHandler(this);
	return 0;
}
int CContactsCanvasHandler::InitContactsLocalContacts(RoutedEventArgs *pe)
{
	return 0;
}
int CContactsCanvasHandler::InitContactsRemoteContacts(RoutedEventArgs *pe)
{
	return 0;
}

int CContactsCanvasHandler::InitContactsExtensions(RoutedEventArgs *pe)
{
	return 0;
}

int CContactsCanvasHandler::OnCreate(RoutedEventArgs *pe)
{
	if (m_pContacts != NULL)
	{
		CREATESTRUCT *pcs = (CREATESTRUCT *)pe->lParam;
		CIUIProject *pProject = GetProject(pe->hOriginalSender);
		_ASSERT(pProject != NULL);

		CPropertySheetInfo *pSheetFavorites = m_pContacts->GetPropertySheet(m_pRadFavorites->GetSafeHwnd());
		if (pSheetFavorites != NULL && pcs->lpCreateParams == pSheetFavorites)
		{
			m_hOriginalSenderFavorites = pe->hOriginalSender;
			InitContactsFavorites(pe);
		}

		CPropertySheetInfo *pSheetLocalContacts = m_pContacts->GetPropertySheet(m_pRadLocalContacts->GetSafeHwnd());
		if (pSheetLocalContacts != NULL && pcs->lpCreateParams == pSheetLocalContacts)
		{
			m_hOriginalSenderLocal= pe->hOriginalSender;
			InitContactsLocalContacts(pe);
			Swtich2Contact(false, false);
		}


		CPropertySheetInfo *pSheetRemoteContacts = m_pContacts->GetPropertySheet(m_pRadRemoteContacts->GetSafeHwnd());
		if (pSheetRemoteContacts != NULL && pcs->lpCreateParams == pSheetRemoteContacts)
		{
			m_hOriginalSenderRemote = pe->hOriginalSender;
			InitContactsRemoteContacts(pe);
			Swtich2RemoteContact(false, false);
		}

		CPropertySheetInfo *pSheetExtensions = m_pContacts->GetPropertySheet(m_pRadExtensions->GetSafeHwnd());
		if (pSheetExtensions != NULL && pcs->lpCreateParams == pSheetExtensions)
		{
			m_hOriginalSenderExtensions = pe->hOriginalSender;
			InitContactsExtensions(pe);
			Swtich2Extension(false, false);
		}
	}

	return 0;
}

LRESULT CContactsCanvasHandler::OnTreeViewHotChanged(RoutedEventArgs *pe)
{
	if (m_pTvFavorites->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvLocalContacts->GetSafeHwnd() == pe->hOriginalSender
		/*|| m_pTvRemoteContacts->GetSafeHwnd() == pe->hOriginalSender*/
		|| m_pTvExtensions->GetSafeHwnd() == pe->hOriginalSender)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		if (NULL != hOldHot)
		{
			HWLWND hBind = pTreeView->GetItemBindWindowless(hOldHot);

			HWLWND hAudio = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
			_ASSERT(NULL != hAudio);
			ShowWindow(hAudio, SW_HIDE);

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

			HWLWND hAudio = IUI::FindControl(hBind, _T("IDC_BTN_AUDIO"));
			_ASSERT(NULL != hAudio);
			ShowWindow(hAudio, SW_SHOW);

			HWLWND hBtnVideo = IUI::FindControl(hBind, _T("IDC_BTN_VIDEO"));
			_ASSERT(NULL != hBtnVideo);
			ShowWindow(hBtnVideo, SW_SHOW);

			HWLWND hBtnMore = IUI::FindControl(hBind, _T("IDC_BTN_MORE"));
			_ASSERT(NULL != hBtnMore);
			ShowWindow(hBtnMore, SW_SHOW);
		}
	}

	return 0;
}

LRESULT CContactsCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
		if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			OnButtonClicked(pe);
		}
		else if (pe->RoutedEvent == WM_CREATE)
		{
			OnCreate(pe);
		}
		else if (TVN_HOTCHANGED == pe->RoutedEvent)
		{
			OnTreeViewHotChanged(pe);
		}
		else if (TVN_HOTCHANGING == pe->RoutedEvent)
		{
			OnTreeViewHotChanging(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
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
LRESULT CContactsCanvasHandler::OnTreeViewHotChanging(RoutedEventArgs *pe)
{
	if (m_pTvFavorites->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvLocalContacts->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvRemoteContacts->GetSafeHwnd() == pe->hOriginalSender
		|| m_pTvExtensions->GetSafeHwnd() == pe->hOriginalSender)
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		HTREEITEM hOldHot = (HTREEITEM)pe->wParam;
		HTREEITEM hNewHot = (HTREEITEM)pe->lParam;

		if (hOldHot == NULL && hNewHot == NULL)
		{
			return 0;
		}

		CIUIPoint pt;
		GetCursorPos(&pt);
		HWND hHost = GetHostWnd(GetBindWLWnd());
		ScreenToClient(hHost, &pt);
		HWLWND hHit = WindowFromPoint(hHost, pt);
		if (NULL != hHit)
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

LRESULT CContactsCanvasHandler::OnRadFavorites(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_FAVORITE_PAGE;
	
	if (m_nLastTabPage != m_nTabPage)
	{
	Swtich2Favorite(FALSE);
	}
	
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CContactsCanvasHandler::OnRadLocalContacts(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_LOCAL_CONTACT_PAGE;
	if (m_nLastTabPage != m_nTabPage)
	{
		Swtich2Contact(FALSE);
	}
	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CContactsCanvasHandler::OnRadRemoteContacts(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_REMOTE_CONTACT_PAGE;

	if (m_nLastTabPage != m_nTabPage)
	{
		Swtich2RemoteContact(FALSE);
	}

	m_nLastTabPage = m_nTabPage;
	return 0;
}

LRESULT CContactsCanvasHandler::OnRadExtensions(RoutedEventArgs *pe)
{
	m_nTabPage = ENUM_EXTERSION_PAGE;

	if (m_nLastTabPage != m_nTabPage)
	{
		Swtich2Extension(FALSE);
	}

	m_nLastTabPage = m_nTabPage;
	return 0;
}
LRESULT CContactsCanvasHandler::OnBtnFirstPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_FAVORITE_PAGE:
		m_nFavoritePage = 1;
		Swtich2Favorite(true, false);
		break;
	case ENUM_LOCAL_CONTACT_PAGE:
		m_nLocalContactPage = 1;
		Swtich2Contact(true, false);
		break;
	case ENUM_REMOTE_CONTACT_PAGE:
		m_nRemoteContactPage = 1;
		Swtich2RemoteContact(true, false);
		break;
	case ENUM_EXTERSION_PAGE:
		m_nRemoteExtensionPage = 1;
		Swtich2Extension(true, false);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CContactsCanvasHandler::OnBtnPrePage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_FAVORITE_PAGE:
		m_nFavoritePage--;
		Swtich2Favorite(true, false);
		break;
	case ENUM_LOCAL_CONTACT_PAGE:
		m_nLocalContactPage--;
		Swtich2Contact(true, false);
		break;
	case ENUM_REMOTE_CONTACT_PAGE:
		m_nRemoteContactPage--;
		Swtich2RemoteContact(true, false);
		break;
	case ENUM_EXTERSION_PAGE:
		m_nRemoteExtensionPage--;
		Swtich2Extension(true,false);
		break;
	default:
		break;
	}

	return 0;
}

LRESULT CContactsCanvasHandler::OnBtnNextPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_FAVORITE_PAGE:
		m_nFavoritePage++;
		Swtich2Favorite(true, false);
		break;
	case ENUM_LOCAL_CONTACT_PAGE:
		m_nLocalContactPage++;
		Swtich2Contact(true, false);
		break;
	case ENUM_REMOTE_CONTACT_PAGE:
		m_nRemoteContactPage++;
		Swtich2RemoteContact(true, false);
		break;
	case ENUM_EXTERSION_PAGE:
		m_nRemoteExtensionPage++;
		Swtich2Extension(true, false);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CContactsCanvasHandler::OnBtnLastPage(RoutedEventArgs *pe)
{
	switch (m_nTabPage)
	{
	case ENUM_FAVORITE_PAGE:
		m_nFavoritePage = m_nFavoritePageMax;
		Swtich2Favorite(true, false);
		break;
	case ENUM_LOCAL_CONTACT_PAGE:
		m_nLocalContactPage = m_nLocalContactPageMax;
		Swtich2Contact(true, false);
		break;
	case ENUM_REMOTE_CONTACT_PAGE:
		m_nRemoteContactPage = m_nRemoteContactPageMax;
		Swtich2RemoteContact(true, false);
		break;
	case ENUM_EXTERSION_PAGE:
		m_nRemoteExtensionPage = m_nRemoteExtensionPageMax;
		Swtich2Extension(true, false);
		break;
	default:
		break;
	}

	return 0;
}

LRESULT CContactsCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pRadFavorites->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadFavorites(pe);
	}
	else if (m_pRadLocalContacts->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadLocalContacts(pe);
	}
	else if (m_pRadRemoteContacts->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadRemoteContacts(pe);
	}
	else if (m_pRadExtensions->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadExtensions(pe);
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
	return 0;
}

LRESULT CContactsCanvasHandler::OnItemBtnAudio(RoutedEventArgs *pe)
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
					ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
					CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
					if (localRecentContactType == ENUM_SINGLE_CONTACT)
					{
						pContactDB->SetCallType(ENUM_CALL_OUT);
						pContactDB->SetUserData(CT_AUDIO);
						SwitchToContact(pContactInfo, CT_AUDIO);
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

LRESULT CContactsCanvasHandler::OnItemBtnVideo(RoutedEventArgs *pe)
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
				CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
				if (NULL != pContactInfo)
				{
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

LRESULT CContactsCanvasHandler::OnItemBtnMore(RoutedEventArgs *pe)
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
				CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
				if (NULL != pContactDB)
				{

					CIUIRect rcMore;
					IUI::GetWindowRect(pe->hOriginalSender, rcMore);

					
					if (m_nTabPage == ENUM_FAVORITE_PAGE )
					{
						HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_FAVORITE));
						HMENU hMoreMenu = GetSubMenu(hMenu, 0);
						ModifyMenu(hMoreMenu, 0, MF_BYPOSITION | MF_STRING, ID_FAVORITE_EDITTHISCONTACT, CSystemEx::GetInstance()->GetEditLocalContactText());
						ModifyMenu(hMoreMenu, 1, MF_BYPOSITION | MF_STRING, ID_FAVORITE_CANCELFAVORITE, CSystemEx::GetInstance()->GetCancelFavoriteText());
						UINT uRet = ::TrackPopupMenu(hMoreMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
							rcMore.left, rcMore.bottom, 0, GetHostWnd(GetBindWLWnd()), rcMore);
						ProcessMenuItemMsg(uRet, pTreeView, hItem);
					}
					else
					{
						HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MORE));
						HMENU hMoreMenu = GetSubMenu(hMenu, 0);

						ModifyMenu(hMoreMenu, 0, MF_BYPOSITION | MF_STRING, ID_MORE_FORWARD, CSystemEx::GetInstance()->GetForwardText());
						ModifyMenu(hMoreMenu, 1, MF_BYPOSITION | MF_STRING, ID_MORE_DELETE, CSystemEx::GetInstance()->GetDeleteThisContactText());
						ModifyMenu(hMoreMenu, 2, MF_BYPOSITION | MF_STRING, ID_MORE_ADD_TO_CONTACT, CSystemEx::GetInstance()->GetAddContactText());
						ModifyMenu(hMoreMenu, 3, MF_BYPOSITION | MF_STRING, ID_MORE_EDIT_AN_EXISTING_CONTACT, CSystemEx::GetInstance()->GetEditLocalContactText());
						ModifyMenu(hMoreMenu, 4, MF_BYPOSITION | MF_STRING, ID_MORE_ADD_TO_FAVORITE_CONTACT, CSystemEx::GetInstance()->GetAddToFavoriteContactText());
						if (m_nTabPage == ENUM_LOCAL_CONTACT_PAGE)
						{

							EnableMenuItem(hMoreMenu,
								ID_MORE_ADD_TO_CONTACT,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							if (pContactDB->m_bFavorites == true)
							{
								EnableMenuItem(hMoreMenu,
									ID_MORE_ADD_TO_FAVORITE_CONTACT,
									MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							}
						}
						else if (m_nTabPage == ENUM_EXTERSION_PAGE)
						{
							bool bExitContact = CSystemEx::GetInstance()->GetDBOprate()->searchContact(pContactDB->m_strSIPNum);
							if (bExitContact)
							{
								EnableMenuItem(hMoreMenu,
									ID_MORE_ADD_TO_CONTACT,
									MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							}
							EnableMenuItem(hMoreMenu,
								ID_MORE_EDIT_AN_EXISTING_CONTACT,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							EnableMenuItem(hMoreMenu,
								ID_MORE_DELETE,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							EnableMenuItem(hMoreMenu, ID_MORE_ADD_TO_FAVORITE_CONTACT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

						}
						else
						{
							EnableMenuItem(hMoreMenu,
								ID_MORE_EDIT_AN_EXISTING_CONTACT,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							EnableMenuItem(hMoreMenu,
								ID_MORE_DELETE,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							EnableMenuItem(hMoreMenu, ID_MORE_ADD_TO_FAVORITE_CONTACT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

							/*TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(pContactDB->m_strSIPNum);
							if (pContactInfo!=NULL)
							{
								EnableMenuItem(hMoreMenu, ID_MORE_ADD_TO_CONTACT, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
							}*/
							EnableMenuItem(hMoreMenu,
								ID_MORE_ADD_TO_CONTACT,
								MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
						}
						EnableMenuItem(hMoreMenu, ID_MORE_DELETE_THREAD, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
						UINT uRet = ::TrackPopupMenu(hMoreMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
							rcMore.left, rcMore.bottom, 0, GetHostWnd(GetBindWLWnd()), rcMore);
						ProcessMenuItemMsg(uRet, pTreeView, hItem);
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

void CContactsCanvasHandler::ProcessMenuItemMsg(int nItemID, TreeView *pTreeView, HTREEITEM hItem)
{
	TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
	CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;

	if (ID_MORE_FORWARD == nItemID)
	{
		if (NULL != pContactInfo)
		{
			SwitchToContact(pContactInfo, CT_TEXT_CHAT);
		}
	}
	else if (ID_MORE_DELETE == nItemID)
	{
		if (m_nTabPage==ENUM_LOCAL_CONTACT_PAGE)
		{
			CSystemEx::GetInstance()->GetMainDlg()->EraseContactLocal(pContact->m_strSIPNum, pContact->m_nUserID);
			pTreeView->DeleteItem(hItem);
			CSystemEx::GetInstance()->GetDBOprate()->deleteContact(pContact->m_strSIPNum, pContact->m_nUserID);
			Swtich2Contact(false, false);
		}
		else if (m_nTabPage==ENUM_FAVORITE_PAGE)
		{
			CSystemEx::GetInstance()->GetMainDlg()->EraseContactFavorite(pContact->m_strSIPNum, pContact->m_nUserID);
			pTreeView->DeleteItem(hItem);
			CSystemEx::GetInstance()->GetDBOprate()->updateContactFavorite(pContact);
			Swtich2Favorite(false, false);
		}
		else
		{
			return;
		}
	
	}
	else  if (ID_MORE_EDIT_AN_EXISTING_CONTACT== nItemID)
	{
		CAddContactWnd *pAddContactWnd = new CAddContactWnd(ENUM_UPDATE_CONTACT,pContactInfo);
		if (NULL != pAddContactWnd)
		{
			pAddContactWnd->SetProject(g_pProject);
			pAddContactWnd->SetXml(_T("IDW_ADD_CONTACT.xml"));
			//
			pAddContactWnd->DoModal();
		}
	}
	else if (ID_MORE_ADD_TO_FAVORITE_CONTACT == nItemID)
	{
		pContact->m_bFavorites = true;
		CSystemEx::GetInstance()->GetDBOprate()->updateContactFavorite(pContact);
		CSystemEx::GetInstance()->GetMainDlg()->Add2Favorite(pContactInfo);
		m_pContacts->SwitchPropertySheet(m_pRadFavorites->GetSafeHwnd());
		Swtich2Favorite(false);
		m_nTabPage = ENUM_FAVORITE_PAGE;
	}
	else if (ID_FAVORITE_CANCELFAVORITE==nItemID)
	{
		pContact->m_bFavorites = false;
		CSystemEx::GetInstance()->GetDBOprate()->updateContactFavorite(pContact);
		m_pContacts->SwitchPropertySheet(m_pRadFavorites->GetSafeHwnd());
		
		CSystemEx::GetInstance()->GetMainDlg()->EraseContactFavorite(pContact->m_strSIPNum, pContact->m_nUserID);
		Swtich2Favorite(false);
	}
	else if (ID_MORE_ADD_TO_CONTACT == nItemID)
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
}
LRESULT CContactsCanvasHandler::OnLButtonDblClk(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_TREECTRL_INBOX"))
	{
		TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(pe->hOriginalSender);

		CIUIPoint pt(pe->lParam);
		HTREEITEM hItem = pTreeView->HitTest(pt);
		if (NULL != hItem)
		{
			TAG_RECENT *pContactInfo = (TAG_RECENT *)pTreeView->GetItemData(hItem);
			if (NULL != pContactInfo)
			{
				SwitchToContact(pContactInfo, CT_TEXT_CHAT);
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
	}
	return 0;
}


LRESULT CContactsCanvasHandler::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_ICON"))
	{
		TAG_RECENT *pContactInfo =( TAG_RECENT * )GetProp(pe->hOriginalSender, GETPROP_CONTACT);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContact = (CContactDB *)pContactInfo->m_unionContactInfo.m_pContact;
			if (NULL != pContact)
			{
				if (PortUtility::compareStringNoCase("1001",pContact->m_strSIPNum)==0)
				{
					printf("");
				}
				if (ONLINESTATE_OFFLINE == pContact->GetOnlineState())
				{
					HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, _T("GrayFaceMask.png"));
					CIUIRect rcMask;
					GetClientRect(pe->hOriginalSender, rcMask);
					IUIDrawImage((HDC)pe->lParam, rcMask, hFace, IRM_CENTER, NULL);
					ImageManager::ReleaseIUIImage(hFace);
					hFace = NULL;
				}
			}
		}
		
	}
	return 0;
}


void CContactsCanvasHandler::Insert2Favorite(TAG_RECENT* pConatct)
{
	m_vecFavoriteContact.insert(m_vecFavoriteContact.begin(), pConatct);
	Swtich2Favorite(false);
}
void CContactsCanvasHandler::Insert2Contact(TAG_RECENT* pConatct)
{
	m_vecDBLocalContact.insert(m_vecLocalContact.begin(), pConatct);
	//Swtich2Contact(false);
}
void CContactsCanvasHandler::Insert2DBContact(TAG_RECENT* pConatct)
{
	m_vecDBLocalContact.insert(m_vecDBLocalContact.begin(), pConatct);
	Swtich2Contact(false);
}
void CContactsCanvasHandler::Insert2RemoteContact(TAG_RECENT* pConatct)
{
	m_vecRemoteContact.insert(m_vecRemoteContact.begin(), pConatct);
	Swtich2RemoteContact(false);
}
void CContactsCanvasHandler::Insert2Extension(TAG_RECENT *pExtension)
{
	m_vecExtensionsContact.insert(m_vecExtensionsContact.begin(), pExtension);
	Swtich2Extension(false);
}
void  CContactsCanvasHandler::EraseItemFavorite(std::string strSipNumber, int nUserID)
{
	auto iterFindFavoriteContact = m_vecFavoriteContact.begin();
	for (; iterFindFavoriteContact != m_vecFavoriteContact.end();)
	{
		TAG_RECENT *pContactInfo = *iterFindFavoriteContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pFavoriteContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pFavoriteContact->m_nUserID == nUserID&&PortUtility::compareStringNoCase(strSipNumber,pFavoriteContact->m_strSIPNum)==0)
			{
				iterFindFavoriteContact = m_vecFavoriteContact.erase(iterFindFavoriteContact);
				pContactInfo->m_unionContactInfo.m_pContact->m_bFavorites = false;
				CSystemEx::GetInstance()->GetDBOprate()->updateContactFavorite(pContactInfo->m_unionContactInfo.m_pContact);
			}
			else
			{
				iterFindFavoriteContact++;
			}
		}
		else
		{
			iterFindFavoriteContact++;
		}
		
	}
	Swtich2Favorite(m_nFavoritePage,false);
	
}
void  CContactsCanvasHandler::EraseItemContact(std::string strSipNumber, int nUserID)
{
	auto iterFindLocalContact = m_vecDBLocalContact.begin();
	for (; iterFindLocalContact != m_vecDBLocalContact.end();)
	{
		TAG_RECENT *pContactInfo = *iterFindLocalContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pLocalContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pLocalContact->m_nUserID == nUserID&&(PortUtility::compareStringNoCase(strSipNumber,pLocalContact->m_strSIPNum)==0))
			{
				iterFindLocalContact = m_vecDBLocalContact.erase(iterFindLocalContact);
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
	Swtich2Contact(m_nLocalContactPage);
	//CSystemEx::GetInstance()->GetDBOprate()->deleteContact(nDataID);
}

void CContactsCanvasHandler::EraseItemRemoteContact()
{
	m_vecRemoteContact.clear();
	Swtich2RemoteContact(m_nRemoteContactPage);
}

void  CContactsCanvasHandler::EraseItemRemoteContact(long long nDataID)
{
	auto iterFindRemoteContact = m_vecRemoteContact.begin();
	for (; iterFindRemoteContact != m_vecRemoteContact.end();)
	{
		TAG_RECENT *pContactInfo = *iterFindRemoteContact;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pContact->m_nUserID == nDataID)
			{
				iterFindRemoteContact = m_vecRemoteContact.erase(iterFindRemoteContact);
				//delete pLocalHistory;
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
	Swtich2RemoteContact(m_nRemoteContactPage);
}
void CContactsCanvasHandler::EraseItemExtension()
{
	m_vecExtensionsContact.clear();
	Swtich2Extension(m_nRemoteExtensionPage);
}
void  CContactsCanvasHandler::EraseItemExtension(long long nDataID)
{
	auto iterFindExternsion = m_vecExtensionsContact.begin();
	for (; iterFindExternsion != m_vecExtensionsContact.end();)
	{
		TAG_RECENT *pContactInfo = *iterFindExternsion;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pExternsionContact = pContactInfo->m_unionContactInfo.m_pContact;
			if (pExternsionContact->m_nUserID == nDataID)
			{
				iterFindExternsion = m_vecExtensionsContact.erase(iterFindExternsion);
				//delete pLocalRecord;
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
	Swtich2Extension(m_nRemoteExtensionPage);
}
void CContactsCanvasHandler::Swtich2Favorite(bool bCheck,bool bTipsCheckSwitch)
{
	m_nFavoritePageMax = ceil((float)m_vecFavoriteContact.size() / (float)MAX_CALL_VIEW_NUM);
	if (bCheck)
	{
		BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nFavoritePage, m_nFavoritePageMax, bTipsCheckSwitch);
		if (bCanSwitch == FALSE)
		{
			return;
		}
	}
	m_pTvFavorites = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderFavorites);
	if (m_pTvFavorites)
	{
		m_pTvFavorites->DeleteAllItems();
	}
	CIUIProject *pProject = GetProject(m_hOriginalSenderFavorites);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_CONTACTS_LOCAL_CONTACTS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nFavoritePage-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecFavoriteContact.size())
		{
			break;
		}
		TAG_RECENT *pContactInfo = m_vecFavoriteContact.at(nDataPos);
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB != NULL)
		{
			HTREEITEM hItem = m_pTvFavorites->InsertItem(_T(""));
			m_pTvFavorites->SetItemData(hItem, (DWORD_PTR)pContactInfo);
			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTvFavorites->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderFavorites), 0);
			m_pTvFavorites->SetItemBindWindowless(hItem, hBind);

			HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hIcon != NULL);
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("bg_user.png"));
			SetProp(hIcon, GETPROP_CONTACT, pContactInfo);
			HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
			_ASSERT(hFrom != NULL);

			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
			SetWindowText(hFrom, strSrcUserName);

			SetWindowText(hIcon, strSrcUserName.Left(1));

			HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
			_ASSERT(hState != NULL);
		
			SetWindowText(hState, strSrcSipNumber);

		}
	}
}
void CContactsCanvasHandler::Swtich2Contact(bool bCheck, bool bTipsCheckSwitch)
{
	m_nLocalContactPageMax = ceil((float)m_vecDBLocalContact.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_vecDBLocalContact.size() == 0)
	{
		return;
	}

	if (bCheck)
	{
		BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nLocalContactPage, m_nLocalContactPageMax, bTipsCheckSwitch);
		if (bCanSwitch == FALSE)
		{
			return;
		}
	}
	m_pTvLocalContacts = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderLocal);
	if (m_pTvLocalContacts == NULL)
	{
		return;
	}
	m_pTvLocalContacts->DeleteAllItems();
	m_pTvLocalContacts = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderLocal);
	CIUIProject *pProject = GetProject(m_hOriginalSenderLocal);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_CONTACTS_LOCAL_CONTACTS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nLocalContactPage-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecDBLocalContact.size())
		{
			break;
		}
		TAG_RECENT *pContactInfo = m_vecDBLocalContact.at(nDataPos);
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB != NULL)
		{
			//m_pTvLocalContacts = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderLocal);
			HTREEITEM hItem = m_pTvLocalContacts->InsertItem(_T(""));
			m_pTvLocalContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);
			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTvLocalContacts->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderLocal), 0);
			m_pTvLocalContacts->SetItemBindWindowless(hItem, hBind);

			HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hIcon != NULL);
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("bg_user.png"));
			SetProp(hIcon, GETPROP_CONTACT, pContactInfo);
			HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
			_ASSERT(hFrom != NULL);
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
			

			SetWindowText(hFrom, strSrcUserName);

			CIUIString strName = GetWindowText(hFrom);
			SetWindowText(hIcon, strName.Left(1));

			HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
			_ASSERT(hState != NULL);
			SetWindowText(hState, strSrcSipNumber);

		}
	}
	Invalidate(GetBindWLWnd());
}
void CContactsCanvasHandler::Swtich2RemoteContact(bool bCheck, bool bTipsCheckSwitch)
{
	m_nRemoteContactPageMax = ceil((float)m_vecRemoteContact.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_vecRemoteContact.size() == 0)
	{
		TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderRemote);
		if (pTvSheet == NULL)
		{
			return;
		}
		pTvSheet->DeleteAllItems();
		return;
	}
	if (bCheck)
	{
		BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nRemoteContactPage, m_nRemoteContactPageMax, bTipsCheckSwitch);
		if (bCanSwitch == FALSE)
		{
			return;
		}
	}
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderRemote);
	if (pTvSheet == NULL)
	{
		return;
	}
	pTvSheet->DeleteAllItems();
	m_pTvRemoteContacts = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderRemote);
	CIUIProject *pProject = GetProject(m_hOriginalSenderRemote);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_CONTACTS_LOCAL_CONTACTS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	std::vector<TAG_RECENT*>::iterator iterVecContact = m_vecRemoteContact.begin();
	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nRemoteContactPage-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecRemoteContact.size())
		{
			break;
		}
		TAG_RECENT *pContactInfo = m_vecRemoteContact.at(nDataPos);
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB != NULL)
		{
			HTREEITEM hItem = m_pTvRemoteContacts->InsertItem(_T(""));
			m_pTvRemoteContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);
			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTvRemoteContacts->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderRemote), 0);
			m_pTvRemoteContacts->SetItemBindWindowless(hItem, hBind);

			HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hIcon != NULL);
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("bg_user.png"));
			SetProp(hIcon, GETPROP_CONTACT, pContactInfo);
			HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
			_ASSERT(hFrom != NULL);
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
			SetWindowText(hFrom, strSrcUserName);

			
			SetWindowText(hFrom, strSrcUserName);
			SetWindowText(hIcon, strSrcUserName.Left(1));

			HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
			_ASSERT(hState != NULL);
			SetWindowText(hState, strSrcSipNumber);

		}
	}
}
void CContactsCanvasHandler::Swtich2Extension(bool bCheck, bool bTipsCheckSwitch)
{
	m_nRemoteExtensionPageMax = ceil((float)m_vecExtensionsContact.size() / (float)MAX_CALL_VIEW_NUM);
	if (m_vecExtensionsContact.size() == 0)
	{
		TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderExtensions);
		if (pTvSheet == NULL)
		{
			return;
		}
		pTvSheet->DeleteAllItems();
		return;
	}
	if (bCheck)
	{
		BOOL bCanSwitch = CSystemEx::GetInstance()->SwtichEnable(m_nRemoteExtensionPage, m_nRemoteExtensionPageMax, bTipsCheckSwitch);
		if (bCanSwitch == FALSE)
		{
			return;
		}
	}
	
	TreeView *pTvSheet = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderExtensions);
	if (pTvSheet == NULL)
	{
		return;
	}
	pTvSheet->DeleteAllItems();

	m_pTvExtensions = (TreeView *)CWLWnd::FromHandle(m_hOriginalSenderExtensions);
	CIUIProject *pProject = GetProject(m_hOriginalSenderExtensions);

	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_CONTACTS_LOCAL_CONTACTS_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	auto iterVecContact = m_vecExtensionsContact.begin();
	int nDataPos = 0;
	for (int nIndex = 0; nIndex < MAX_CALL_VIEW_NUM; nIndex++)
	{

		nDataPos = ((m_nRemoteExtensionPage-1)* MAX_CALL_VIEW_NUM) + nIndex;
		if (nDataPos >= m_vecExtensionsContact.size())
		{
			break;
		}
		TAG_RECENT *pContactInfo = m_vecExtensionsContact.at(nDataPos);
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (pContactDB != NULL)
		{
			HTREEITEM hItem = m_pTvExtensions->InsertItem(_T(""));
			m_pTvExtensions->SetItemData(hItem, (DWORD_PTR)pContactInfo);
			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTvExtensions->GetSafeHwnd(), GetHostWnd(m_hOriginalSenderExtensions), 0);
			m_pTvExtensions->SetItemBindWindowless(hItem, hBind);

			HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_ICON"));
			_ASSERT(hIcon != NULL);
			SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, _T("bg_user.png"));
			SetProp(hIcon, GETPROP_CONTACT, pContactInfo);
			HWLWND hFrom = IUI::FindControl(hBind, _T("IDC_STA_FROM"));
			_ASSERT(hFrom != NULL);
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

			SetWindowText(hFrom, strSrcUserName);
			SetWindowText(hIcon, strSrcUserName.Left(1));

			HWLWND hState = IUI::FindControl(hBind, _T("IDC_STA_STATE"));
			_ASSERT(hState != NULL);
			SetWindowText(hState, strSrcSipNumber);

		}
	}
}
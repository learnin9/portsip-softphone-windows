#include "stdafx.h"
#include "AddCallerDlg.h"
#include "System/SystemEx.h"
#include "../../../../MainCanvasHandler.h"
CAddCallerDlg::CAddCallerDlg(CWLWndHandler *pSrcCallingPage, ENUM_CONFENCE_TYPE enumConfenceType, CHAT_TAB enumChatTab)
	: m_pCanvasMain(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnClose(NULL)
	, m_pStaSplitter1(NULL)
	, m_pTabTransfer(NULL)

	, m_pReSearch(NULL)
	, m_pBtnGo(NULL)
	, m_pTvResult(NULL)
	, m_pBtnTransferAsk(NULL)
	, m_pBtnTransferNow(NULL)
	, m_pAddContactInfo(NULL)
	, m_enumConfenceType(enumConfenceType)
	, m_pAddCallerCanvasHandler(NULL)
{
	m_enumPage = AP_ADD_CALLER;
	m_typeTab = enumChatTab;
	m_pSrcCalingPage = pSrcCallingPage;
}

CAddCallerDlg::~CAddCallerDlg()
{
	if (NULL != m_pAddCallerCanvasHandler)
	{
		delete m_pAddCallerCanvasHandler;
		m_pAddCallerCanvasHandler = NULL;
	}
}

int CAddCallerDlg::InitControls()
{
	m_pCanvasMain = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_MAIN")));
	_ASSERT(m_pCanvasMain != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pStaSplitter1 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SPLITTER1")));
	_ASSERT(m_pStaSplitter1 != NULL);
	m_pTabTransfer = (TabControl *)CWLWnd::FromHandle(FindControl(_T("IDC_PNL_TAB_TRANSFER")));
	_ASSERT(m_pTabTransfer != NULL);
	return 0;
}

ENUM_RECENT_CONTACT_TYPE CAddCallerDlg::GetSrcRecentContactType()
{
	if (m_pSrcContact!=NULL)
	{
		return m_pSrcContact->m_enumRecentContact;
	}
	return ENUM_SINGLE_CONTACT;
}
LRESULT CAddCallerDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	
	m_pStaTitle->SetWindowText(_T("ADD CALLER"));

	m_pTabTransfer->AddPropertySheet(AP_SEARCH, _T("IDD_CALLING_TRANSFER_SEARCH.xml"), NULL);
	m_pTabTransfer->AddPropertySheet(AP_ASK, _T("IDD_CALLING_TRANSFER_ASK.xml"), NULL);
	m_pAddCallerCanvasHandler = new CAddCallerCanvasHandler();
	m_pTabTransfer->AddPropertySheet(AP_ADD_CALLER,
		_T("IDD_CALLER_ADD.xml"), m_pAddCallerCanvasHandler);
	m_pTabTransfer->SwitchPropertySheet(AP_SEARCH);
	InitControlData();
	CenterWindow();

	return 0;
}
int CAddCallerDlg::InitControlData()
{
	if (m_pTvResult != NULL)
	{
		m_pTvResult->DeleteAllItems();
	}

	CSystemEx::GetInstance()->GetMainDlg()->SearchFromContact(m_vecContact, "",true);
	auto iterContact = m_vecContact.begin();
	CContactDB *pContact;

	CIUIProject *pProject = GetProject();
	CControlProp *pContactProp = pProject->FindProjectItem(_T("IDD_CALLING_TRANSFER_RESULT_ITEM.xml"));
	_ASSERT(pContactProp != NULL);
	CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
	_ASSERT(pBindContactProp != NULL);

	for (iterContact; iterContact != m_vecContact.end(); iterContact++)
	{
		TAG_RECENT *pContactInfo = *iterContact;
		pContact = pContactInfo->m_unionContactInfo.m_pContact;
		{
			CIUIString strHeadImage, strUserName, strSipNumber;

			FormatItemContact(strUserName, strHeadImage, strSipNumber, pContact);
			HTREEITEM hItem = InsertContactItem(pBindContactProp, strHeadImage,
				strUserName, strSipNumber, pContact);

			m_pTvResult->SetItemData(hItem, (DWORD_PTR)pContactInfo);

		}
	}
	return 0;
}
LRESULT CAddCallerDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CAddCallerDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			OnButtonClicked(pe);
		}
		else if (UE_TAB_SWITCHED == pe->RoutedEvent)
		{
			OnTabSwitched(pe);
		}
		else if (WM_COMMAND == pe->RoutedEvent)
		{
			OnCommand(pe);
		}
		else if (WM_NOTIFY == pe->RoutedEvent)
		{
			OnNotify(pe);
		}
		else if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			OnItemDBClick(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			OnAfterPaint(pe);
		}

		
	}

	if (uMsg == WM_KEYDOWN)
	{
		LPMSG lpMsg = (LPMSG)lParam;
		if (lpMsg != NULL)
		{
			if (VK_RETURN == pe->wParam)
			{
				switch (m_enumPage)
				{
				case AP_SEARCH:
					DoSelectContactOk();
					break;
				case AP_ASK:
					break;
				case AP_ADD_CALLER:
					break;
				default:
					break;
				}
			}
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CAddCallerDlg::OnBtnClose(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddCallerDlg::OnBtnGo(RoutedEventArgs *pe)
{
	DoSelectContactOk();
	return 0;
}

LRESULT CAddCallerDlg::OnBtnOk(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddCallerDlg::OnBtnCancelAddCaller(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddCallerDlg::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
	{
		Label *pStaFace = (Label *)CWLWnd::FromHandle(pe->hOriginalSender);
		CContactDB *pContact = (CContactDB *)IUI::GetProp(pe->hOriginalSender, GETPROP_CONTACT);
		if (NULL != pContact)
		{
			Gdiplus::Graphics g((HDC)pe->lParam);
			g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

			CIUIRect rc;
			IUI::GetClientRect(pe->hOriginalSender, rc);
			DrawFace((HDC)pe->lParam, rc,PortUtility::Utf82Unicode_ND(pContact->m_strHeadImage).c_str() );
			

			HIUIFONT hDefFont = GetProject()->GetFontMgr()->GetDefaultFont();
			_ASSERT(hDefFont != NULL);

			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContact);
			if (pContact->GetSetUserFaceImage() == false)
			{
				CIUIString strLeft = strSrcUserName.Left(1);
				DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
					CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
			}	
		}
	}

	return 0;
}

LRESULT CAddCallerDlg::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}
	else if (m_pBtnTransferNow->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (m_pAddContactInfo!=NULL)
		{
			DoAdd(m_pAddContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum);
			EnableWindow(m_pBtnTransferNow->GetSafeHwnd(), FALSE);
		}
		
		//m_pTabTransfer->SwitchPropertySheet(AP_ADD_CALLER);

	}
	else if (m_pBtnTransferAsk->GetSafeHwnd()==pe->hOriginalSender)
	{
		m_pTabTransfer->SwitchPropertySheet(AP_SEARCH);
		//m_pTvResult->Invalidate();
		//m_pTabTransfer->Invalidate();
	}
	else if (m_pBtnGo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnGo(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_OK"))
	{
		OnBtnOk(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_ADD_CALLER_CANCEL"))
	{
		OnBtnCancelAddCaller(pe);
	}

	return 0;
}

int CAddCallerDlg::UpdateSearchResult(LPCTSTR lpszSearchText)
{
	SetULWNoRedraw(m_hWnd, TRUE);

	m_pTvResult->DeleteAllItems();

	CIUIProject *pProject = GetProject();

	CControlProp *pContactProp = pProject->FindProjectItem(_T("IDD_CALLING_TRANSFER_RESULT_ITEM.xml"));
	_ASSERT(pContactProp != NULL);
	CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
	_ASSERT(pBindContactProp != NULL);

	string tempKeyString = PortUtility::Unicode2Utf8_ND(lpszSearchText);
	std::vector<TAG_RECENT *> vFoundContacts;
	CSystemEx::GetInstance()->GetMainDlg()->SearchFromContact(vFoundContacts, tempKeyString,true);

	std::vector<TAG_RECENT *>::iterator it = vFoundContacts.begin();
	for (; it != vFoundContacts.end(); ++it)
	{
		TAG_RECENT *pContactInfo = *it;
		CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
		CIUIString strHeadImage, strUserName, strSipNumber;
		FormatItemContact(strUserName, strHeadImage, strSipNumber, pContact);
		HTREEITEM hItem = InsertContactItem(pBindContactProp, strHeadImage,
			strUserName, strSipNumber, pContact);

		m_pTvResult->SetItemData(hItem, (DWORD_PTR)pContactInfo);
	}

	SetULWNoRedraw(m_hWnd, FALSE);
	IUIUpdateWindow(m_hWnd);

	return 0;
}

HTREEITEM CAddCallerDlg::InsertContactItem(
	CControlProp *pBindProp,
	LPCTSTR lpszIconFile,
	LPCTSTR lpszName,
	LPCTSTR lpszPhone, const CContactDB *pContact)
{
	HTREEITEM hItem = m_pTvResult->InsertItem(_T(""));

	HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
		m_pTvResult->GetSafeHwnd(), m_hWnd, 0);
	m_pTvResult->SetItemBindWindowless(hItem, hBind);

	HWLWND hIcon = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
	_ASSERT(hIcon != NULL);
	IUI::SetBkImage(hIcon, FALSE, CONTROL_STATE_NORMAL, lpszIconFile);
	IUI::SetProp(hIcon, GETPROP_CONTACT, (HANDLE)pContact);

	HWLWND hStaName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
	_ASSERT(hStaName != NULL);
	SetWindowText(hStaName, lpszName);

	HWLWND hStaPhone = IUI::FindControl(hBind, _T("IDC_STA_PHONE"));
	_ASSERT(hStaPhone != NULL);
	SetWindowText(hStaPhone, lpszPhone);

	return hItem;
}
LRESULT CAddCallerDlg::OnTabSwitched(RoutedEventArgs *pe)
{
	if (m_pTabTransfer->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (AP_SEARCH == pe->lParam)
		{
			m_pReSearch = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_SEARCH")));
			_ASSERT(m_pReSearch != NULL);
			m_pReSearch->SetEventMask(m_pReSearch->GetEventMask() | ENM_CHANGE);

			m_pBtnGo = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_GO")));
			_ASSERT(m_pBtnGo != NULL);

			m_pTvResult = (TreeView *)CWLWnd::FromHandle(FindControl(_T("IDC_TREECTRL_RESULT")));
			_ASSERT(m_pTvResult != NULL);
			m_enumPage = AP_SEARCH;
		}
		else if (AP_ASK == pe->lParam)
		{
			_ASSERT(m_pAddContactInfo != NULL);

			HWLWND hCvsAskRoot = FindControl(_T("IDC_CANVAS_TRANSFER_ASK"));
			_ASSERT(hCvsAskRoot != NULL);

			HWLWND hStaTitle = IUI::FindControl(hCvsAskRoot, _T("IDC_STA_ASK_TITLE"));
			_ASSERT(hStaTitle != NULL);
			CIUIString strTitle;
			CIUIString strHeadImage, strUserName, strSipNumber;
			FormatItemContact(strUserName, strHeadImage, strSipNumber, m_pAddContactInfo->m_unionContactInfo.m_pContact);


			CIUIString strJoinPre, strJoinEnd;
			strJoinPre = CSystemEx::GetInstance()->GetJoinGroupPreText();
			strJoinEnd = CSystemEx::GetInstance()->GetJoinGroupEndText();
			strTitle = strJoinPre + strUserName + strJoinEnd;
			SetWindowText(hStaTitle, strTitle);

			HWLWND hStaFace = IUI::FindControl(hCvsAskRoot, _T("IDC_STA_FACE"));
			SetWindowText(hStaFace, strUserName.Left(1));

			m_pBtnTransferAsk = (Button *)CWLWnd::FromHandle(
				IUI::FindControl(hCvsAskRoot, _T("IDC_BTN_ASK_FIRST")));
			_ASSERT(m_pBtnTransferAsk != NULL);

			m_pBtnTransferNow = (Button *)CWLWnd::FromHandle(
					IUI::FindControl(hCvsAskRoot, _T("IDC_BTN_TRANSFER_NOW")));
			_ASSERT(m_pBtnTransferNow != NULL);

			m_pBtnTransferAsk->SetWindowText(_T("BACK"));
			m_pBtnTransferNow->SetWindowText(_T("CONFIRM"));
			m_enumPage = AP_ASK;
		}
	}

	return 0;
}

LRESULT CAddCallerDlg::OnCommand(RoutedEventArgs *pe)
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
LRESULT CAddCallerDlg::OnItemDBClick(RoutedEventArgs * pe)
{
	if (m_pTvResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		HTREEITEM hSelItem = m_pTvResult->GetSelectedItem();
		m_pAddContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
		if (m_pAddCallerCanvasHandler==NULL)
		{
			return 0;
		}
		m_pAddCallerCanvasHandler->SetContact(m_pAddContactInfo->m_unionContactInfo.m_pContact);
		if (m_pAddContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}
	return 0;
}
LRESULT CAddCallerDlg::OnNotify(RoutedEventArgs *pe)
{
	if (m_pTvResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		NMHDR *pnmhdr = (NMHDR *)pe->lParam;

		if (TVN_SELCHANGED == pnmhdr->code)
		{
			HTREEITEM hSelItem = m_pTvResult->GetSelectedItem();
			if (NULL != hSelItem)
			{
				m_pAddContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
				_ASSERT(m_pAddContactInfo != NULL);	
			}
		}
	}

	return 0;
}
bool CAddCallerDlg::DoSelectContactOk()
{
	HTREEITEM hSelItem = m_pTvResult->GetSelectedItem();
	if (hSelItem == NULL)
	{
		CIUIString strSearch = m_pReSearch->GetWindowText();
		std::string strSearchUTF8 = PortUtility::Unicode2Utf8_ND(strSearch.GetBuffer(strSearch.GetLength()));
		bool bIsNum = PortUtility::CheckExtersionNumber(strSearch);
		if (bIsNum == false)
		{
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"please provide  an available number", NULL);
			return false;
		}
		TAG_RECENT *pContactInfo = NULL;

		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSearchUTF8);
		if (pContactInfo == NULL)
		{
			pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSearchUTF8);
		}
		m_pAddContactInfo = pContactInfo;
		if (m_pAddContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}
	else
	{
		m_pAddContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
		if (m_pAddContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}

	return true;
}

void CAddCallerDlg::DoAdd(std::string strNum)
{
	
	DoAskFirst();
}
void CAddCallerDlg::DoAddedContact()
{
	std::string strError;
	if (m_pSrcContact != NULL && m_pAddContactInfo != NULL)
	{
		//todo
		m_pTabTransfer->SwitchPropertySheet(AP_ADD_CALLER);
	}
	vector<CContactDB*> vecContactInfo;
	if (m_pAddContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT&&m_pAddContactInfo->m_unionContactInfo.m_pContact!=NULL)
	{
		vecContactInfo.push_back(m_pAddContactInfo->m_unionContactInfo.m_pContact);
	}
	
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pSrcContact->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		if (m_pSrcContact->m_unionContactInfo.m_pContact!=NULL)
		{
			vecContactInfo.push_back(m_pSrcContact->m_unionContactInfo.m_pContact);
		}
		TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewTalkGroupByVecSContatct(vecContactInfo);
		CSystemEx::GetInstance()->GetDBOprate()->CreateGroup(pContactInfo);
		CSystemEx::GetInstance()->GetMainCanvasHandler()->CreateGroupContact(pContactInfo);
		
		switch (m_enumConfenceType)
		{
		case ENUM_CONFENCE_AUDIO:
			{
				CSystemEx::GetInstance()->CreateConfence(ENUM_CONFENCE_AUDIO, NULL);
				CAudioPageCanvasHandler *pSrcAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pSrcContact);
				CAudioPageCanvasHandler * pAddAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pAddContactInfo);
				CAudioPageCanvasHandler * pGroupPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(pContactInfo);
				int nHistroyIDSrc, nHistroyIDAdd, nActiveLineSrc, nActiveLineAdd;
				//CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler();
				CTalkGroup* pGroupTalk = pContactInfo->m_unionContactInfo.m_pChartGroup;
				if (pGroupTalk)
				{
					nHistroyIDSrc = m_pSrcContact->m_unionContactInfo.m_pContact->GetHistoryID();
					nActiveLineSrc = m_pSrcContact->m_unionContactInfo.m_pContact->ReturnActiveLine();
					pGroupTalk->CopySrcData2GroupNumber(nActiveLineSrc, nHistroyIDSrc, (void*)m_pSrcContact);
					CSystemEx::GetInstance()->joinConference(nActiveLineSrc);

					nHistroyIDAdd = m_pAddContactInfo -> m_unionContactInfo.m_pContact->GetHistoryID();
					nActiveLineAdd = pAddAudioPageCanvasHandler->GetActiveLine();
					pGroupTalk->CopySrcData2GroupNumber(nActiveLineAdd, nHistroyIDAdd, (void*)m_pAddContactInfo);
					CSystemEx::GetInstance()->joinConference(nActiveLineAdd);
					
				}
				
				if (pSrcAudioPageCanvasHandler != NULL)
				{
					pSrcAudioPageCanvasHandler->CloseWnd();
				}

				if (pAddAudioPageCanvasHandler != NULL)
				{
					pAddAudioPageCanvasHandler->CloseWnd();
				}
				if (pGroupPageCanvasHandler!=NULL)
				{
					CSystemEx::GetInstance()->SaveSession2AudioCanvasHandler(nActiveLineSrc, pGroupPageCanvasHandler);
					CSystemEx::GetInstance()->SaveSession2AudioCanvasHandler(nActiveLineAdd, pGroupPageCanvasHandler);
				}
				SwitchToContact(pContactInfo, CT_AUDIO);
				
				if (pGroupPageCanvasHandler != NULL)
				{
					pGroupPageCanvasHandler->SetConnected(true, nActiveLineSrc);
					pGroupPageCanvasHandler->SetConnected(true, nActiveLineAdd);
				}
			}
			break;
		case ENUM_CONFENCE_VIDEO:
			{
				CVideoCanvasHandler * pGroupPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(pContactInfo);
				SwitchToContact(pContactInfo, CT_VIDEO);
				pGroupPageCanvasHandler->m_pSrcContact = m_pSrcContact;
				pGroupPageCanvasHandler->m_pAddContact = m_pAddContactInfo;
				::PostMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_CONFENCE_ACTION, (WPARAM)pGroupPageCanvasHandler, ENUM_SINGLE_CONTACT);
			}
			break;
		}

	}
	else
	{
		CContactDB *pContact = m_pAddContactInfo->m_unionContactInfo.m_pContact;
		if (pContact==NULL)
		{
			LOG4_ERROR(L"error type");
			return;
		}
		CIUIString strHeadImage, strUserName, strSipNumber;
		FormatItemContact(strUserName, strHeadImage, strSipNumber, pContact);
		CTalkGroupNumber *pTalkGroupNumber = new CTalkGroupNumber;
		pTalkGroupNumber->m_strSIPNumber = pContact->m_strSIPNum ;
		pTalkGroupNumber->m_bFavorite = false;
		pTalkGroupNumber->m_strSelfDisplayName = PortUtility::Unicode2Utf8_ND(strUserName.GetBuffer(strUserName.GetLength())).c_str();
		pTalkGroupNumber->m_bIsDisable = false;
		pTalkGroupNumber->m_bIsCreator = false;
		pTalkGroupNumber->m_bIsManager = false;
		pTalkGroupNumber->SetContactDB(pContact);
		CSystemEx::GetInstance()->GetDBOprate()->AddContact2Group(m_pSrcContact, pTalkGroupNumber);

		CTalkGroup *pTalkGroup = m_pSrcContact->m_unionContactInfo.m_pChartGroup;
		pTalkGroup->AddContact2Group(pTalkGroupNumber);
		pTalkGroup->UpdateGroupNameByContact(m_pAddContactInfo);

		CSystemEx::GetInstance()->GetMainCanvasHandler()->UpdateGroupInfo(m_pSrcContact);
		switch (m_enumConfenceType)
		{
			case ENUM_CONFENCE_AUDIO:
		{
			CAudioPageCanvasHandler *pAudioPageCanvasHandler = CSystemEx::GetInstance()->GetAudioPageCanvasHandlerByContact(m_pAddContactInfo);
			if (pAudioPageCanvasHandler != NULL)
			{
				pAudioPageCanvasHandler->CloseWnd();
			}

			SwitchToContact(m_pSrcContact, CT_AUDIO);
			break;
		}
		case ENUM_CONFENCE_VIDEO:
		{
			CVideoCanvasHandler *pVideoPageCanvasHandler = CSystemEx::GetInstance()->GetVideoCanvasHandlerByContact(m_pAddContactInfo);
			if (pVideoPageCanvasHandler != NULL)
			{
				pVideoPageCanvasHandler->CloseWnd();
			}
			SwitchToContact(m_pSrcContact, CT_VIDEO);
			break;
		}
		}
	}
	EndDialog(IDOK);
}
bool CAddCallerDlg::DoAskFirst()
{

	if (m_pAddContactInfo)
	{
		CContactDB *pContactDB = m_pAddContactInfo->m_unionContactInfo.m_pContact;
		CContactDB *pContactAdd = m_pSrcContact->m_unionContactInfo.m_pContact;
		pContactDB->SetIsAddedConfence(this, m_pSrcContact,false);
		pContactAdd->SetIsAddedConfence(this, m_pAddContactInfo,true);
		int nReplaceLine = -1;
		switch (m_enumConfenceType)
		{
		case ENUM_CONFENCE_AUDIO:
			pContactDB->SetCallType(ENUM_CALL_OUT);
			SwitchToContact(m_pAddContactInfo, CT_AUDIO);
			break;
		case ENUM_CONFENCE_VIDEO:
			pContactDB->SetCallType(ENUM_CALL_OUT);
			SwitchToContact(m_pAddContactInfo, CT_VIDEO);
			break;
		default:
			break;
		}

		HWLWND hCvsAskRoot = FindControl(_T("IDC_CANVAS_TRANSFER_ASK"));
		_ASSERT(hCvsAskRoot != NULL);

		HWLWND hStaTitle = IUI::FindControl(hCvsAskRoot, _T("IDC_STA_ASK_TITLE"));
		_ASSERT(hStaTitle != NULL);
		CIUIString strTitle;
		CIUIString strHeadImage, strUserName, strSipNumber;
		FormatItemContact(strUserName, strHeadImage, strSipNumber, m_pAddContactInfo->m_unionContactInfo.m_pContact);
		CIUIString strJoinPre, strJoinEnd;
		strJoinPre = CSystemEx::GetInstance()->GetJoinGroupPreText();
		strJoinEnd = CSystemEx::GetInstance()->GetJoinGroupEndText();
		strTitle = strJoinPre + strUserName + strJoinEnd;
		SetWindowText(hStaTitle, strTitle);
	}
	return true;
}


void CAddCallerDlg::CleanCalingPageData()
{
	switch (m_typeTab)
	{
	case CT_TEXT_CHAT:
		break;
	case CT_AUDIO:
	{
		if (m_pSrcCalingPage != NULL)
		{
			CAudioPageCanvasHandler *pAudioPageCanvasHandler = (CAudioPageCanvasHandler*)m_pSrcCalingPage;
			pAudioPageCanvasHandler->DoHangup(true);
		}
		if (m_pDesCalingPage != NULL)
		{
			CAudioPageCanvasHandler *pAudioPageCanvasHandler = (CAudioPageCanvasHandler *)m_pDesCalingPage;
			pAudioPageCanvasHandler->DoHangup(true);
		}
	}
	break;
	case CT_VIDEO:
	{
		if (m_pSrcCalingPage != NULL)
		{
			CVideoCanvasHandler *pAudioPageCanvasHandler = (CVideoCanvasHandler*)m_pSrcCalingPage;
			//pAudioPageCanvasHandler->DoHangup();
		}
		if (m_pDesCalingPage != NULL)
		{
			CVideoCanvasHandler *pAudioPageCanvasHandler = (CVideoCanvasHandler *)m_pDesCalingPage;
			//pAudioPageCanvasHandler->DoHangup();
		}
	}
	break;
	default:
		break;
	}

}
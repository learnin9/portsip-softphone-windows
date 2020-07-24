#include "stdafx.h"
#include "CallingTransferDlg.h"
#include "System/SystemEx.h"

CCallingTransferDlg::CCallingTransferDlg(CWLWndHandler *pSrcCallingPage,CHAT_TAB enumChatTab)
	: m_pCanvasMain(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnClose(NULL)
	, m_pStaSplitter1(NULL)
	, m_pTabTransfer(NULL)

	, m_pReSearch(NULL)
	, m_pTvResult(NULL)
	, m_pBtnTransferNow(NULL)
	, m_pBtnAskFirst(NULL)
	, m_pTransferContactInfo(NULL)
	, m_pBtnGo(NULL)
{
	m_typeTab = enumChatTab;
	m_enumTransferType = NO_CONDITION;
	m_bClickTree = false;
	m_pSrcCalingPage = pSrcCallingPage;
	m_enumPage = TP_SEARCH;
	m_hSetParentHwnd = NULL;
	m_enumAskTransferStep = STEP_ASK;
}

CCallingTransferDlg::~CCallingTransferDlg()
{
}
BOOL CCallingTransferDlg::CreateEx(
	DWORD dwExStyle,
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU nIDorHMenu,
	LPVOID lpParam)
{
	HWND hParent = hWndParent;
	if (NULL != m_hSetParentHwnd)
	{
		hParent = m_hSetParentHwnd;
	}

	return Window::CreateEx(dwExStyle, lpszClassName, lpszWindowName,
		dwStyle, x, y, nWidth, nHeight, hParent, nIDorHMenu, lpParam);
}

int CCallingTransferDlg::InitControls()
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

	TAG_RECENT *pContactInfo = NULL;

	pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber("105");
	m_pTransferContactInfo = pContactInfo;
	return 0;
}

LRESULT CCallingTransferDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	m_pTabTransfer->AddPropertySheet(TP_SEARCH, _T("IDD_CALLING_TRANSFER_SEARCH.xml"), NULL);
	m_pTabTransfer->AddPropertySheet(TP_ASK, _T("IDD_CALLING_TRANSFER_ASK.xml"), NULL);
	m_pTabTransfer->AddPropertySheet(TP_TIPS_PAGE, _T("IDD_CALLING_TRANSFER_OK.xml"), NULL);

	m_pTabTransfer->SwitchPropertySheet(TP_SEARCH);
	InitControlData();
	CenterWindow();
	return 0;
}

int CCallingTransferDlg::InitControlData()
{
	if (m_pTvResult != NULL)
	{
		m_pTvResult->DeleteAllItems();
	}
	std::vector<TAG_RECENT*> vecContact;
	CSystemEx::GetInstance()->GetMainDlg()->SearchFromContact(vecContact,"",true);
	auto iterContact = vecContact.begin();
	CContactDB *pContact;

	CIUIProject *pProject = GetProject();
	CControlProp *pContactProp = pProject->FindProjectItem(_T("IDD_CALLING_TRANSFER_RESULT_ITEM.xml"));
	_ASSERT(pContactProp != NULL);
	CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
	_ASSERT(pBindContactProp != NULL);

	for (iterContact;iterContact!= vecContact.end();iterContact++)
	{
		TAG_RECENT *pContactInfo = *iterContact;
		pContact = pContactInfo->m_unionContactInfo.m_pContact;
		{
			CIUIString strHeadImage=L"",strUserName = L"",strSipNumber = L"";
			
			FormatItemContact(strUserName, strHeadImage, strSipNumber, pContact);
			HTREEITEM hItem = InsertContactItem(pBindContactProp, strHeadImage,
				strUserName, strSipNumber,pContact);

			m_pTvResult->SetItemData(hItem, (DWORD_PTR)pContactInfo);

		}
	}
	return 0;
}

LRESULT CCallingTransferDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CHAR)
	{
		LPMSG lpMsg = (LPMSG)lParam;
		if (lpMsg != NULL)
		{
			char ascii_code = wParam;
			stringstream strstrTestOut;
			strstrTestOut << "WM_CHAR: Character =" << ascii_code << endl;
			string strTestOut = strstrTestOut.str();
			OutputDebugString(PortUtility::Utf82Unicode_ND(strTestOut).c_str());
			LOG4_INFO(strTestOut.c_str());
		}
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CCallingTransferDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
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
		else if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			OnItemDBClick(pe);
		}
		else if (WM_NOTIFY == pe->RoutedEvent)
		{
			OnNotify(pe);
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
				case TP_SEARCH:
					DoSelectContactOk();
					break;
				case TP_ASK:
					break;
				case TP_TIPS_PAGE:
					break;
				default:
					break;
				}
			}
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}
bool CCallingTransferDlg::DoSelectContactOk()
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
		m_pTransferContactInfo = pContactInfo;
		if (m_pTransferContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}
	else
	{
		m_pTransferContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
		if (m_pTransferContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}

	return true;
}
LRESULT CCallingTransferDlg::OnBtnGo(RoutedEventArgs *pe)
{
	DoSelectContactOk();
	return 0;
}
LRESULT CCallingTransferDlg::OnAfterPaint(RoutedEventArgs *pe)
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
			DrawFace((HDC)pe->lParam, rc, PortUtility::Utf82Unicode_ND(pContact->m_strHeadImage).c_str());


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
LRESULT CCallingTransferDlg::OnBtnClose(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CCallingTransferDlg::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
		m_bClickTree = false;
	}
	else if (m_pBtnTransferNow->GetSafeHwnd() == pe->hOriginalSender)
	{
		DoTransfer(m_pTransferContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum);
		m_bClickTree = false;
	}
	else if (m_pBtnAskFirst->GetSafeHwnd()==pe->hOriginalSender)
	{
		DoAskFirst();
		m_bClickTree = false;
	}
	else if (m_pBtnGo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnGo(pe);
		m_bClickTree = false;
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_OK"))
	{
	}
	else if (GetName(pe->hOriginalSender)==_T("IDC_BTN_TRANSFER"))
	{
		DoAttendTransferOk();
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_CANCLE"))
	{
		OnBtnCancle();
	}
	return 0;
}
void CCallingTransferDlg::OnBtnCancle()
{
	EndDialog(IDOK);
}

int CCallingTransferDlg::UpdateSearchResult(LPCTSTR lpszSearchText)
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
		CIUIString strHeadImage, strUserName, strSipNumberFormat;
		FormatItemContact(strUserName, strHeadImage, strSipNumberFormat, pContact);
		HTREEITEM hItem = InsertContactItem(pBindContactProp, strHeadImage,
			strUserName, strSipNumberFormat,pContact);

		m_pTvResult->SetItemData(hItem, (DWORD_PTR)pContactInfo);
	}
	
	SetULWNoRedraw(m_hWnd, FALSE);
	IUIUpdateWindow(m_hWnd);

	return 0;
}

HTREEITEM CCallingTransferDlg::InsertContactItem(
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

LRESULT CCallingTransferDlg::OnTabSwitched(RoutedEventArgs *pe)
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
			m_enumPage = (TRANSFER_PAGE)TP_SEARCH;
		}
		else if (AP_ASK == pe->lParam)
		{
			_ASSERT(m_pTransferContactInfo != NULL);

			HWLWND hCvsAskRoot = FindControl(_T("IDC_CANVAS_TRANSFER_ASK"));
			_ASSERT(hCvsAskRoot != NULL);

			HWLWND hStaTitle = IUI::FindControl(hCvsAskRoot, _T("IDC_STA_ASK_TITLE"));
			_ASSERT(hStaTitle != NULL);
			CIUIString strTitle;
			/*Image *pImage = Image::FromFile(PortUtility::string2WString(pContact->m_strHeadImage).c_str());
			TextureBrush *ptBrush = new TextureBrush(pImage);
			IUI::SetUserData(hFace, (LPARAM)ptBrush);

			HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
			_ASSERT(hName != NULL);
			*/
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pTransferedContactInfo->m_unionContactInfo.m_pContact);
			CIUIString strDesHeadImage, strDesUserName, strDesSipNumber;
			FormatItemContact(strDesUserName, strDesHeadImage, strDesSipNumber, m_pTransferContactInfo->m_unionContactInfo.m_pContact);
			
			CIUIString strOkPre, strOkEnd;
			strOkPre = CSystemEx::GetInstance()->GetStartTransferPreText();
			strOkEnd = CSystemEx::GetInstance()->GetStartTransferEndText();
			strTitle = strOkPre + strSrcUserName + strOkEnd;

			SetWindowText(hStaTitle, strTitle);

			HWLWND hStaFace = IUI::FindControl(hCvsAskRoot, _T("IDC_STA_FACE"));
			SetWindowText(hStaFace, strDesUserName);
		
			m_pBtnTransferNow = (Button *)CWLWnd::FromHandle(
					IUI::FindControl(hCvsAskRoot, _T("IDC_BTN_TRANSFER_NOW")));
			_ASSERT(m_pBtnTransferNow != NULL);

			m_pBtnAskFirst = (Button *)CWLWnd::FromHandle(
				IUI::FindControl(hCvsAskRoot, _T("IDC_BTN_ASK_FIRST")));

			_ASSERT(m_pBtnAskFirst != NULL);
			m_enumPage = (TRANSFER_PAGE)TP_ASK;
		}
		else if (TP_TIPS_PAGE == pe->lParam)
		 {
			RefreshTipsPage();
			m_enumPage = (TRANSFER_PAGE)TP_TIPS_PAGE;
		 }
	}

	return 0;
}

void CCallingTransferDlg::RefreshTipsPage()
{
	HWLWND hStaTransferOk = FindControl(_T("IDC_STA_TITLE_OK"));
	_ASSERT(hStaTransferOk != NULL);
	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pTransferedContactInfo->m_unionContactInfo.m_pContact);
	CIUIString strDesHeadImage, strDesUserName, strDesSipNumber;
	FormatItemContact(strDesUserName, strDesHeadImage, strDesSipNumber, m_pTransferContactInfo->m_unionContactInfo.m_pContact);
	CIUIString strOk;

	m_pInfoTansfer = (Label *)CWLWnd::FromHandle(
		FindControl(_T("IDC_STA_TITLE_OK")));
	_ASSERT(m_pInfoTansfer != NULL);

	m_pBtnCancle = (Button *)CWLWnd::FromHandle(
		FindControl(_T("IDC_BTN_CANCLE")));
	_ASSERT(m_pBtnCancle != NULL);

	m_pBtnOK = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_OK")));
	_ASSERT(m_pBtnOK != NULL);

	m_pBtnTransferOK = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_TRANSFER")));
	_ASSERT(m_pBtnTransferOK != NULL);

	m_pBtnOK->ShowWindow(SW_HIDE);

	if (m_enumTransferType == NO_CONDITION)
	{
		CIUIString strOkPre, strOkEnd;
		strOkPre = CSystemEx::GetInstance()->GetWaitingAnswerPreText();
		strOkEnd = CSystemEx::GetInstance()->GetWaitingAnswerEndText();
		strOk = strOkPre + strDesUserName + strOkEnd;
		SetWindowText(hStaTransferOk, strOk);
		EnableWindow(m_pBtnTransferOK->GetSafeHwnd(), FALSE);
	}
	else
	{
		CIUIString strOkPre, strOkEnd;
		if (m_enumAskTransferStep == STEP_ASK)
		{
			strOkPre = CSystemEx::GetInstance()->GetWaitingAnswerPreText();
			strOkEnd = CSystemEx::GetInstance()->GetWaitingAnswerEndText();
			strOk = strOkPre + strDesUserName + strOkEnd;
		}
		else
		{
			strOkPre = CSystemEx::GetInstance()->GetTransferPreText();
			strOk = strOkPre + strDesUserName;
		}

		SetWindowText(hStaTransferOk, strOk);
		EnableWindow(m_pBtnTransferOK->GetSafeHwnd(), TRUE);
	}
}
void CCallingTransferDlg::ProcessTransferByRemote(bool bSuccess)
{
	if (bSuccess)
	{
		if (m_pBtnOK)
		{
			m_pBtnOK->ShowWindow(SW_SHOW);
		}
		EndDialog(IDOK);
	}
	else
	{
		CIUIString strOk; 

		strOk.Format(_T("Transfer failed "));
		if (m_pInfoTansfer)
		{
			m_pInfoTansfer->SetWindowText(strOk);
		}
		EndDialog(IDOK);
	}
}
LRESULT CCallingTransferDlg::OnCommand(RoutedEventArgs *pe)
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
LRESULT CCallingTransferDlg::OnItemDBClick(RoutedEventArgs * pe)
{
	if (m_pTvResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		HTREEITEM hSelItem = m_pTvResult->GetSelectedItem();
		m_pTransferContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
		if (m_pTransferContactInfo != NULL)
		{
			m_pTabTransfer->SwitchPropertySheet(AP_ASK);
		}
	}
	return 0;
}
LRESULT CCallingTransferDlg::OnNotify(RoutedEventArgs *pe)
{
	if (m_pTvResult->GetSafeHwnd() == pe->hOriginalSender)
	{
		NMHDR *pnmhdr = (NMHDR *)pe->lParam;

		if (TVN_SELCHANGED == pnmhdr->code)
		{
			HTREEITEM hSelItem = m_pTvResult->GetSelectedItem();
			if (NULL != hSelItem)
			{
				m_pTransferContactInfo = (TAG_RECENT *)m_pTvResult->GetItemData(hSelItem);
				_ASSERT(m_pTransferContactInfo != NULL);

				m_bClickTree = true;
				//m_pTabTransfer->SwitchPropertySheet(TP_ASK);
			}
		}
	}

	return 0;
}


void CCallingTransferDlg::DoNext(bool bClick)
{
	CIUIString strSearch = m_pReSearch->GetWindowText();
	if (!strSearch.IsEmpty()|| bClick==true)
	{
		std::string strSearchUTF8 = PortUtility::Unicode2Utf8_ND(strSearch.GetBuffer(strSearch.GetLength()));
		bool bIsNum = PortUtility::CheckExtersionNumber(strSearch);
		TAG_RECENT *pContactInfo = NULL;
		if (bClick == false) {
			pContactInfo =  CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSearchUTF8);
			if (pContactInfo == NULL)
			{
				pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSearchUTF8);
			}
			m_pTransferContactInfo = pContactInfo;
		}
		else
		{
			m_pTransferContactInfo;
		}
		if (bClick)
		{
			strSearchUTF8 = m_pTransferContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum;
		}
		if (bIsNum|| bClick == true)
		{
			DoTransfer(strSearchUTF8);
		}
		else
		{
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"please choise one contact ", NULL);
		}
	}
	else
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"please choise one contact or type one Nimber",NULL);
	}
}

void CCallingTransferDlg::CleanCalingPageData()
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
		if (m_pDesCalingPage!=NULL)
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
void CCallingTransferDlg::DoAttendTransferOk()
{
	if (m_enumTransferType == ASK_FIRST)
	{
		std::string strError;
		CContactDB *pDesContactDB = m_pTransferContactInfo->m_unionContactInfo.m_pContact;
		CContactDB *pSrcContactDB = m_pTransferedContactInfo->m_unionContactInfo.m_pContact;
		bool bRet = CSystemEx::GetInstance()->AttendTransfer(pDesContactDB->ReturnActiveLine(), \
			pSrcContactDB->ReturnActiveLine(), pDesContactDB->m_strSIPNum, strError);
		if (bRet)
		{
			CleanCalingPageData();
		}
		else
		{
			CIUIString strErrorTip;
			strErrorTip.Format(L"Transfer failed, Error info:%s", PortUtility::string2WString(strError).c_str());
			string strInfo = PortUtility::wstring2String(strErrorTip.GetBuffer(strErrorTip.GetLength()));
			
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", PortUtility::Utf82Unicode_ND(strInfo).c_str(), NULL);
		}

	}
	else
	{
		CContactDB *pDesContactDB = m_pTransferContactInfo->m_unionContactInfo.m_pContact;
		DoTransfer(pDesContactDB->m_strSIPNum);
	}
	
}
void CCallingTransferDlg::ProcessTranseferSuccess(bool bSuccess, int nErrorCode)
{
	switch (m_enumTransferType)
	{
	case NO_CONDITION:
		if (bSuccess)
		{
		}
		else
		{
			EndDialog(IDOK);
			CIUIString strErrorTip;
			std::string strErrorInfo = CSystemEx::GetInstance()->GetSipErrorInfo(nErrorCode);
			if (!strErrorInfo.empty() && nErrorCode != 0)
			{
				strErrorTip.Format(L"Transfer failed:%s", PortUtility::Utf82Unicode_ND(strErrorInfo).c_str());
			}
			else
			{
				strErrorTip.Format(L"Transfer failed");
			}
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", strErrorTip, NULL);
		}
		break;
	case ASK_FIRST:
		if (bSuccess)
		{
			m_enumAskTransferStep = STEP_SUCC;
			RefreshTipsPage();
		}
		break;
	default:
		break;
	}
	
	
}
void CCallingTransferDlg::DoTransfer(std::string strNum)
{
	m_enumTransferType = NO_CONDITION;
	std::string strError;
	bool bRet  = CSystemEx::GetInstance()->Transfer(strNum, m_pTransferedContactInfo->m_unionContactInfo.m_pContact->ReturnActiveLine(), strError);
	if (bRet)
	{
		m_pTabTransfer->SwitchPropertySheet(TP_TIPS_PAGE);
	}
	else
	{
		std::wstring wstrUnicode = PortUtility::string2WString(strError);
		CIUIString strErrorTip;
		strErrorTip.Format(L"Transfer failed, Error info:%s", wstrUnicode.c_str());
		string strInfo = PortUtility::wstring2String(strErrorTip.GetBuffer(strErrorTip.GetLength()));

		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", PortUtility::Utf82Unicode_ND(strInfo).c_str(), NULL);
	}

}

bool CCallingTransferDlg::DoAskFirst()
{
	m_enumTransferType = ASK_FIRST;
	if (m_pTransferContactInfo)
	{
		CContactDB *pSrcContact = m_pTransferedContactInfo->m_unionContactInfo.m_pContact;
		CContactDB *pDesContact = m_pTransferContactInfo->m_unionContactInfo.m_pContact;
		pSrcContact->SetIsTransfer(this, m_pTransferContactInfo,true);
		pDesContact->SetIsTransfer(this, m_pTransferedContactInfo,false);
		int nReplaceLine = -1;
		switch (m_typeTab)
		{
		case CT_AUDIO:
			pDesContact->SetCallType(ENUM_CALL_OUT);
			
			SwitchToContact(m_pTransferContactInfo, CT_AUDIO);
			break;
		case CT_VIDEO:
			pDesContact->SetCallType(ENUM_CALL_OUT);
			SwitchToContact(m_pTransferContactInfo, CT_VIDEO);
			break;
		case CT_TEXT_CHAT:
			break;
		default:
			break;
		}
	}

	m_pTabTransfer->SwitchPropertySheet(TP_TIPS_PAGE);
	m_enumAskTransferStep = STEP_ASK;
	return true;
}
#include "StdAfx.h"
#include "LoginDlg.h"
#include "LoginSettingsWnd.h"



CLoginDlg::CLoginDlg()
{
	m_bHaveLocalUserData = false;
	m_pCanvasLogging = NULL;
	m_pStaLogging = NULL;
}


CLoginDlg::~CLoginDlg(void)
{
}

int CLoginDlg::InitControls()
{
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pCanvasMain = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_MAIN")));
	_ASSERT(m_pCanvasMain != NULL);
	m_pLogo = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_LOGO")));
	_ASSERT(m_pLogo != NULL);
	m_pStartConnectTips = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_START")));
	_ASSERT(m_pStartConnectTips != NULL);
	m_pCmbAccount = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_ACCOUNT")));
	_ASSERT(m_pCmbAccount != NULL);
	m_pRePassword = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_PASSWORD")));
	_ASSERT(m_pRePassword != NULL);
	m_pReSipdomain = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_DOMAIN")));
	_ASSERT(m_pRePassword != NULL);
	m_pRemberMe = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_REMEMBER_ACCOUNT")));
	_ASSERT(m_pRemberMe != NULL);
	m_pAutoLogin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_AUTO_LOGIN")));
	_ASSERT(m_pAutoLogin != NULL);
	m_pAdvanced = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_ADVANCED")));
	_ASSERT(m_pAdvanced != NULL);
	m_pLogin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_LOGIN")));
	_ASSERT(m_pLogin != NULL);
	m_pCanvasLogging = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_LOGGING")));
	_ASSERT(m_pCanvasLogging != NULL);
	m_pStaLogging = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_LOGGING")));
	_ASSERT(m_pStaLogging != NULL);
#ifdef BZXX
	tagSIZE tagSize;
	tagSize.cx = 140;
	tagSize.cy = 52;
	
	CRect rcClientLogo;
	m_pLogo->GetClientRect(&rcClientLogo);
	m_pLogo->MoveWindow(rcClientLogo.left,rcClientLogo.top,tagSize.cx,tagSize.cy);
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_NORMAL, L"Logo_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_HOVER, L"Logo_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_PRESSED, L"Logo_BZXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_DISABLED, L"Logo_BZXX.png");
#endif

#ifdef KCXX
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_NORMAL, L"Logo_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_HOVER, L"Logo_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_PRESSED, L"Logo_KCXX.png");
	IUI::SetBkImage(FindControl(_T("IDC_STA_LOGO")), FALSE, CONTROL_STATE_DISABLED, L"Logo_KCXX.png");
#endif
	m_lstUser = CSystemEx::GetInstance()->GetDBOprate()->getAllAccount();
	if (m_lstUser.size()!=0)
	{
		m_bHaveLocalUserData = true;
		InitControlsData();
	}
	else
	{
		m_pUser = new CUser;
	}
	
	return 0;
}
void CLoginDlg::InitControlsData()
{
	int nIndex = 0;
	CControlProp *pProp = g_pProject->FindProjectItem(_T("IDW_LOGIG_ACCOUNT_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);
	HWND hListBox = m_pCmbAccount->GetDroppedWnd();
	std::list< CUser*>::iterator itUser;
	for (itUser = m_lstUser.begin(); itUser != m_lstUser.end(); itUser++)
	{
		CUser* pLocalUser = *itUser;
		CIUIString strListName, strSip, strSipDomian;
		strSip = PortUtility::Utf82Unicode_ND(pLocalUser->m_strSipNumber).c_str();
		strSipDomian = PortUtility::Utf82Unicode_ND(pLocalUser->m_strSipDomain).c_str();
		strListName = strSip + L"@" + strSipDomian;
		int nItemIndex = m_pCmbAccount->InsertString(nIndex, strSip);

		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			NULL, hListBox, 0);
		_ASSERT(NULL != hBind);
		HWLWND hStaItem = IUI::FindControl(hBind, _T("IDC_STA_ACCOUNT_ITEM"));
		_ASSERT(NULL != hStaItem);
		IUI::SetWindowText(hStaItem, strListName);
		m_pCmbAccount->SetItemBindWindowless(nItemIndex, hBind);

		nIndex++;
	}
	itUser = m_lstUser.begin();
	m_pUser = *itUser;
	m_pCmbAccount->SetCurSel(0);
	if (m_pUser->m_bRemberMe==true)
	{
		m_pRemberMe->SetCheck(true);
		m_pRePassword->SetWindowText(PortUtility::string2WString(m_pUser->m_strPassword).c_str());
	}
	else
	{
		m_pRemberMe->SetCheck(false);
		m_pRePassword->SetWindowText(L"");
	}
	if (m_pUser->m_bAutoLogin)
	{
		m_pAutoLogin->SetCheck(true);
	}
	
	m_pReSipdomain->SetWindowText(PortUtility::string2WString(m_pUser->m_strSipDomain).c_str());
	
	if (m_pUser->m_bAutoLogin)
	{
		EnableWindowBtn();
		CBaseThread loThread;
		loThread.BeginThread(_AutoLoginThread, this);
	}
}
void CLoginDlg::EnableWindowBtn()
{
	EnableWindow(m_pCmbAccount->GetSafeHwnd(), FALSE);
	EnableWindow(m_pRemberMe->GetSafeHwnd(), FALSE);
	EnableWindow(m_pAdvanced->GetSafeHwnd(), FALSE);
	EnableWindow(m_pLogin->GetSafeHwnd(), FALSE);

}
void CLoginDlg::UnEnableWindowBtn()
{
	EnableWindow(m_pCmbAccount->GetSafeHwnd(), TRUE);
	EnableWindow(m_pRemberMe->GetSafeHwnd(), TRUE);
	EnableWindow(m_pAdvanced->GetSafeHwnd(), TRUE);
	EnableWindow(m_pLogin->GetSafeHwnd(), TRUE);
}
LRESULT CLoginDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	CenterWindow();
	Invalidate();
	return 0;
}

LRESULT CLoginDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	 if (uMsg == WM_COMMAND)
		{
			if (HIBYTE(wParam) == CBN_SELCHANGE)
			{
				int nIndex = m_pCmbAccount->GetCurSel();
			}
		}
	 if (uMsg ==WM_CREATE_INFO_WND)
	 {
		 InfoWndParam *pInfoWndParam = NULL;
		 pInfoWndParam = (InfoWndParam*)wParam;
		 if (pInfoWndParam==NULL)
		 {
			 PopupInfoWnd(L"Error", L"something wrong when login ", GetSafeHwnd(), INFO_WND, 5000);
		 }
		 else
		 {
			 PopupInfoWnd(pInfoWndParam->strTitle, pInfoWndParam->strMsg, GetSafeHwnd(), INFO_WND, pInfoWndParam->nAutoTimer);
			 delete pInfoWndParam;
			 pInfoWndParam = NULL;
		 }
		
	 }
	 
	if (uMsg == WM_KEYDOWN)
	 {
		 LPMSG lpMsg = (LPMSG)lParam;
		 if (lpMsg != NULL)
		 {
			if (VK_RETURN == wParam)
			 {
				if (IsWindowEnabled(m_pLogin->GetSafeHwnd())==true)
				{
					DoLogin();
				}
			 }
		}		
	}
	 
	return Window::WindowProc(uMsg, wParam, lParam);
}
void CLoginDlg::ShowLoginWaiting()
{
	m_pCanvasMain->ShowWindow(SW_HIDE);
	m_pCanvasLogging->ShowWindow(SW_SHOW);
}
void CLoginDlg::EndLoginWaiting()
{
	if (m_pCanvasMain)
	m_pCanvasMain->ShowWindow(SW_SHOW);
	
	if(m_pCanvasLogging)
	m_pCanvasLogging->ShowWindow(SW_HIDE);
	
}
void CLoginDlg::ProcessLoginReslut(bool bLoginSuccess, std::string strErrorInfo)
{

	if (bLoginSuccess)
	{
		m_pUser->m_nUserID = CSystemEx::GetInstance()->GetDBOprate()->saveActiveAccount(m_pUser);
		CSystemEx::GetInstance()->SetGobalUser(m_pUser);
		CSystemEx::GetInstance()->InitUserOptions();
		//if (bRet == false)
		{

		}
		CSystemEx::GetInstance()->SetRegister(true);
		CSystemEx::GetInstance()->InitFileServer();
		EndLoginWaiting();
		
		//CheckTime(1);
		EndDialog(IDOK);
	}
	else
	{

		EndLoginWaiting();
		//::MessageBox(GetSafeHwnd(), _T("please check the network and login param,or login in at later,there is some thing error about login."), NULL, MB_OK);
		UnEnableWindowBtn();
		CSystemEx::GetInstance()->SetRegister(false);
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = PortUtility::Utf82Unicode_ND(strErrorInfo).c_str();
		pParam->strTitle = _T("Error");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
	}
	
}
bool CLoginDlg::CheckTime(int nCode)
{
	CTime LocalStart = CTime::GetCurrentTime();
	CTime LocalEnd;
	if (nCode==0)
	{
		LocalEnd = CTime(2020, 9, 30, 0, 0, 0);
	}
	else
	{
		LocalEnd = CTime(2020, 10, 1, 0, 0,0);
	}
	
	CTimeSpan tmSpan = LocalEnd - LocalStart;
	if (tmSpan.GetTimeSpan()<0)
	{
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = CSystemEx::GetInstance()->GetTipsDateOutText();
		pParam->strTitle = _T("Error");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
		Sleep(10000);
		TerminateProcess(GetCurrentProcess(), 0);
	}
	
	return true;
}
void CLoginDlg::DoLogin()
{
	//bool bEndTime = CheckTime(0);
	
	bool bSetOK = false;
	std::string strErrorInfo;
	bSetOK = VerifyLocalParam(strErrorInfo);
	if (bSetOK)
	{
		CIUIString strTemplateURL = m_pCmbAccount->GetWindowText();
		size_t nPos = strTemplateURL.Find(L"@");
		CIUIString strSipNumber;
		if (nPos > 0 && nPos < strTemplateURL.GetLength())
		{
			strSipNumber = strTemplateURL.Left(nPos);
		}
		else
		{
			strSipNumber = strTemplateURL;
		}
		
		
		string tempNumber = PortUtility::Unicode2Utf8_ND(strSipNumber.GetBuffer(strSipNumber.GetLength()));
		m_pUser->m_strSipNumber = tempNumber;
		CIUIString strPassword = m_pRePassword->GetWindowText();
		//strBuffer = strPassword.GetBuffer(strPassword.GetLength());
		string tempPassword = PortUtility::Unicode2Utf8_ND(strPassword.GetBuffer(strPassword.GetLength()));
		m_pUser->m_strPassword = tempPassword;
		//SIP domain
		CIUIString strSipDomain = m_pReSipdomain->GetWindowText();
		//strBuffer = strSipDomain.GetBuffer(strSipDomain.GetLength());
		string tempSipDomain = PortUtility::Unicode2Utf8_ND(strSipDomain.GetBuffer(strSipDomain.GetLength()));
		m_pUser->m_strSipDomain = tempSipDomain;
		
		CSystemEx::GetInstance()->SetGobalUser(m_pUser);
		
		EnableWindow(m_pLogin->GetSafeHwnd(), FALSE);
		ShowLoginWaiting();

		CBaseThread loThread;
		loThread.BeginThread(_DoLoginThread, this);
		
		
	}
	else
	{
		LOG4_INFO(strErrorInfo.c_str());
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = PortUtility::string2WString(strErrorInfo).c_str();
		pParam->strTitle = _T("Info");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
	}
}
void CLoginDlg::DoAutoLogin()
{
	Sleep(2000);
	if (m_pAutoLogin->GetCheck()==BST_CHECKED)
	{
		DoLogin();
	}
	else
	{
		UnEnableWindowBtn();
	}
}
void CLoginDlg::DoLoginNext()
{
	std::string strErrorInfo;
	bool bRet = CSystemEx::GetInstance()->InitRestAPIByUser();
	if (bRet==false)
	{
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = L"Login RestAPI error£¬Some functions cannot be used ";
		pParam->strTitle = _T("Info");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
		//return;
	}
	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);
	CIUIString strTempDataFolder = CSystemEx::GetInstance()->GetDataFolder();
	if (strTempDataFolder.IsEmpty())
	{
		LOG4_ERROR("save file path is error");
		strTempDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
	}
	strFullPath = PortUtility::wstring2String(strTempDataFolder.GetBuffer(strTempDataFolder.GetLength())) + "\\clientlog";
	PortUtility::CheckFilePath(strFullPath);
	bRet = CSystemEx::GetInstance()->InitSIPByUser((TRANSPORT_TYPE)CSystemEx::GetInstance()->GetGlobalUser()->GetProtocolMode(), PORTSIP_LOG_NONE, strFullPath.c_str(), 0, 0, false, strErrorInfo, m_pUser);
	if (bRet == false)
	{
		EndLoginWaiting();
		EnableWindow(m_pLogin->GetSafeHwnd(), TRUE);
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = PortUtility::string2WString(strErrorInfo).c_str();
		pParam->strTitle = _T("Info");
		pParam->nAutoTimer = 3000;
		SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
	}
	else
	{

	}
}
 unsigned int  CLoginDlg::_AutoLoginThread(void* pParam)
{
	if (pParam)
	{
		CLoginDlg* pThisDlg = (CLoginDlg*)pParam;
		pThisDlg->DoAutoLogin();
	}
	return 0;
}
unsigned int CLoginDlg::_DoLoginThread(void* pParam)
{
	if (pParam)
	{
		CLoginDlg* pThisDlg = (CLoginDlg*)pParam;
		pThisDlg->DoLoginNext();
	}
	return 0;
}
LRESULT CLoginDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == WM_NCCREATE)
		{
		}
		else if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			if (GetName(pe->hOriginalSender) == _T("IDC_BTN_LOGIN"))
			{
				DoLogin();
			}
			else if (GetName(pe->hOriginalSender)==_T("IDC_CHK_REMEMBER_ACCOUNT"))
			{
				m_pUser->m_bRemberMe = m_pRemberMe->GetCheck();
			}
			else if (GetName(pe->hOriginalSender)==_T("IDC_CHK_AUTO_LOGIN)")||pe->hOriginalSender== m_pAutoLogin->GetSafeHwnd())
			{
				m_pUser->m_bAutoLogin = m_pAutoLogin->GetCheck();
			}
			else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_ADVANCED"))
			{
				//VerifyLocalParam()==true;
				if (m_pCmbAccount->GetWindowText().IsEmpty())
				{
					InfoWndParam *pParam = new InfoWndParam;
					pParam->strMsg = L"Please Input Account";
					pParam->strTitle = _T("Info");
					pParam->nAutoTimer = 3000;
					SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
					return Window::OnControlEvent(wParam, lParam);
				}
				CLoginSettingsWnd dlg;
				dlg.SetCurrentUser(m_pUser);
				CSystemEx::GetInstance()->SetGobalUser(m_pUser);
				dlg.SetProject(GetProject());
				dlg.SetXml(_T("IDW_LOGIN_SETTINGS.xml"));
				dlg.DoModal();
			}
		}
		else if (pe->RoutedEvent == UE_AFTERPAINTBKIMAGE)
		{
			if (GetName(pe->hOriginalSender) == XNAME_COMBOBOX_EDIT
				|| GetName(pe->hOriginalSender) == _T("IDC_RE_PASSWORD")
				|| GetName(pe->hOriginalSender) == _T("IDC_RE_DOMAIN"))
			{
				CIUIString strText = GetWindowText(pe->hOriginalSender);
				if (strText.IsEmpty())
				{
					HDC hDC = (HDC)pe->lParam;
					CIUIRect rcClient;
					IUI::GetClientRect(pe->hOriginalSender, rcClient);

					RECT rcTextPadding = GetTextPadding(pe->hOriginalSender);
					HIUIFONT hDefFont = GetProject()->GetFontMgr()->GetDefaultFont();
					_ASSERT(hDefFont != NULL);
					CIUIString strPrompt;
					if (GetName(pe->hOriginalSender) == XNAME_COMBOBOX_EDIT/*&& m_bHaveLocalUserData==false*/)
					{
						strPrompt = CSystemEx::GetInstance()->GetAccountText();
					}
					else if (GetName(pe->hOriginalSender) == _T("IDC_RE_PASSWORD"))
					{
						strPrompt = CSystemEx::GetInstance()->GetPasswordText();
					}
					else if (GetName(pe->hOriginalSender) == _T("IDC_RE_DOMAIN"))
					{
						strPrompt = CSystemEx::GetInstance()->GetSipDomainText();
					}
					DrawFormatText(hDC, strPrompt, strPrompt.GetLength(),
						rcClient, &rcTextPadding,
						TAH_LEFT, TAV_TOP, DT_SINGLELINE, hDefFont->GetSafeHFont(),
						RGB(128, 128, 128), FALSE);
				}
			}
		}
		else if (uMsg == WM_COMMAND)
		{
			if (HIWORD(pe->wParam) == CBN_SELCHANGE)
			{
				int nIndex = m_pCmbAccount->GetCurSel();
				ResetUserData(nIndex);
			}
		}
		
		
	}

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
			{
				OnBtnClose(pe);
			}
		}
	}


	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CLoginDlg::OnBtnClose(RoutedEventArgs *pe)
{
	EndDialog(IDCANCEL);
	return 0;
}
void CLoginDlg::ResetUserData(int nColNum)
{
	if (nColNum>m_lstUser.size())
	{
		OutputDebugStringA("ERROR");
	}
	auto iterFind = m_lstUser.begin();
	for (size_t i = 0; i < nColNum; i++)
	{
		iterFind++;
	}
	m_pUser = *iterFind;
	if (m_pUser->m_bRemberMe == true)
	{
		m_pRemberMe->SetCheck(true);
		m_pRePassword->SetWindowText(PortUtility::string2WString(m_pUser->m_strPassword).c_str());
	}
	else
	{
		m_pRemberMe->SetCheck(false);
		m_pRePassword->SetWindowText(L"");
	}

	m_pReSipdomain->SetWindowText(PortUtility::string2WString(m_pUser->m_strSipDomain).c_str());

}
bool CLoginDlg::VerifyLocalParam(std::string &strError)
{
	//Have account
	if (m_pCmbAccount->GetWindowText().IsEmpty())
	{
		strError = "Plz Input a Account";
		return false;
	}
	//Have Pwd
	if (m_pRePassword->GetWindowText().IsEmpty())
	{
		strError = "Plz Input Password";
		return false;
	}
	//Have sipdmomain
	if (m_pReSipdomain->GetWindowText().IsEmpty())
	{
		strError = "Plz Input Sip domain";
		return false;
	}
	return true;
}
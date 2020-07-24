#include "stdafx.h"
#include "LoginSettingsWnd.h"
#include "System/SystemEx.h"
wchar_t *G_TCHAR_TRANSPORT[] = { L"UDP",L"TLS",L"TCP" };
wchar_t *G_TCHAR_SRTP[] = { L"None",L"Force",L"Prefer" };
wchar_t *G_TCHAR_PRECENCE_MODE[] = { L"Peer To Peer", L"Agent" };
CLoginSettingsWnd::CLoginSettingsWnd()
	: m_pBtnMin(NULL)
	, m_pChkMax(NULL)
	, m_pBtnClose(NULL)
	, m_pCanvasMain(NULL)
	, m_pStaSettings(NULL)
	, m_pStaAccount(NULL)
	, m_pStaDisplayName(NULL)
	, m_pReDisplayName(NULL)
	, m_pStaAuthroizationName(NULL)
	, m_pReAuthroizationName(NULL)
	, m_pStaOutboundServer(NULL)
	, m_pReOutboundServer(NULL)
	, m_pStaTransport(NULL)
	, m_pStaSignalingTransport(NULL)
	, m_pCmbSignalingTransport(NULL)
	, m_pStaSrtp(NULL)
	, m_pCmbSrtp(NULL)
	, m_pChkVerifyTls(NULL)
	//, m_pStaPresnece(NULL)
	//, m_pStaPresenceMode(NULL)
	//, m_pCmbPresenceMode(NULL)
	, m_pStaPollTime(NULL)
	, m_pRePollTime(NULL)
	, m_pStaRefreshInterval(NULL)
	, m_pReRefreshInterval(NULL)
	, m_pBtnReturn(NULL)
	, m_pBtnSave(NULL)
{
}

CLoginSettingsWnd::~CLoginSettingsWnd()
{
}

int CLoginSettingsWnd::InitControls()
{
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pCanvasMain = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_MAIN")));
	_ASSERT(m_pCanvasMain != NULL);
	m_pStaSettings = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SETTINGS")));
	_ASSERT(m_pStaSettings != NULL);
	m_pStaAccount = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_ACCOUNT")));
	_ASSERT(m_pStaAccount != NULL);
	m_pStaDisplayName = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_DISPLAY_NAME")));
	_ASSERT(m_pStaDisplayName != NULL);
	m_pReDisplayName = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_DISPLAY_NAME")));
	_ASSERT(m_pReDisplayName != NULL);
	m_pStaAuthroizationName = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_AUTHROIZATION_NAME")));
	_ASSERT(m_pStaAuthroizationName != NULL);
	m_pReAuthroizationName = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_AUTHROIZATION_NAME")));
	_ASSERT(m_pReAuthroizationName != NULL);
	m_pStaOutboundServer = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_OUTBOUND_SERVER")));
	_ASSERT(m_pStaOutboundServer != NULL);
	m_pReOutboundServer = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_OUTBOUND_SERVER")));
	_ASSERT(m_pReOutboundServer != NULL);
	m_pStaTransport = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TRANSPORT")));
	_ASSERT(m_pStaTransport != NULL);
	m_pStaSignalingTransport = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SIGNALING_TRANSPORT")));
	_ASSERT(m_pStaSignalingTransport != NULL);
	m_pCmbSignalingTransport = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_SIGNALING_TRANSPORT")));
	_ASSERT(m_pCmbSignalingTransport != NULL);
	m_pStaSrtp = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SRTP")));
	_ASSERT(m_pStaSrtp != NULL);
	m_pCmbSrtp = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_SRTP")));
	_ASSERT(m_pCmbSrtp != NULL);
	m_pChkVerifyTls = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_VERIFY_TLS")));
	_ASSERT(m_pChkVerifyTls != NULL);
	//m_pStaPresnece = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_PRESNECE")));
	//_ASSERT(m_pStaPresnece != NULL);
	//m_pStaPresenceMode = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_PRESENCE_MODE")));
	//_ASSERT(m_pStaPresenceMode != NULL);
	//m_pCmbPresenceMode = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_PRESENCE_MODE")));
	//_ASSERT(m_pCmbPresenceMode != NULL);
	m_pStaPollTime = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_POLL_TIME")));
	_ASSERT(m_pStaPollTime != NULL);
	m_pRePollTime = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_POLL_TIME")));
	_ASSERT(m_pRePollTime != NULL);
	m_pStaRefreshInterval = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_REFRESH_INTERVAL")));
	_ASSERT(m_pStaRefreshInterval != NULL);
	m_pReRefreshInterval = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_REFRESH_INTERVAL")));
	_ASSERT(m_pReRefreshInterval != NULL);
	m_pBtnReturn = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_RETURN")));
	_ASSERT(m_pBtnReturn != NULL);
	m_pBtnSave = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_SAVE")));
	_ASSERT(m_pBtnSave != NULL);

	//m_pStaPresnece->ShowWindow(SW_HIDE);
	//m_pStaPresenceMode->ShowWindow(SW_HIDE);
	//m_pCmbPresenceMode->ShowWindow(SW_HIDE);

	for (int nIndex = 0;nIndex<= TRANSPORT_TCP;nIndex++)
	{
		m_pCmbSignalingTransport->InsertString(nIndex, G_TCHAR_TRANSPORT[nIndex]);
	}
	for (int nIndex = 0;nIndex<=SRTP_POLICY_PREFER;nIndex++)
	{
		m_pCmbSrtp->InsertString(nIndex, G_TCHAR_SRTP[nIndex]);
	}
	for (int nIndex = 0; nIndex <= ENUM_AGENT;nIndex++)
	{
		//m_pCmbPresenceMode->InsertString(nIndex, G_TCHAR_PRECENCE_MODE[nIndex]);
		//G_TCHAR_PRECENCE_MODE
	}
	
	CUser *pCurrentUser = m_pUser;
	std::stringstream ssStrTemp;
	ssStrTemp << pCurrentUser->m_nSubscribeRefreshTime;
	m_pRePollTime->SetWindowText(PortUtility::Utf82Unicode_ND(ssStrTemp.str()).c_str());
	ssStrTemp.clear();
	ssStrTemp.str("");
	ssStrTemp << pCurrentUser->m_nPublishRefreshTime;
	m_pReRefreshInterval->SetWindowText(PortUtility::Utf82Unicode_ND(ssStrTemp.str()).c_str());
	ssStrTemp.clear();
	ssStrTemp.str("");
	ssStrTemp << pCurrentUser->m_strDisplayName;
	m_pReDisplayName->SetWindowText(PortUtility::Utf82Unicode_ND(ssStrTemp.str()).c_str());
	ssStrTemp.clear();
	ssStrTemp.str("");
	ssStrTemp << pCurrentUser->m_strAuthName;
	m_pReAuthroizationName->SetWindowText(PortUtility::Utf82Unicode_ND(ssStrTemp.str()).c_str());

	ssStrTemp.clear();
	ssStrTemp.str("");
	if (pCurrentUser->m_strOutboundServer.empty()==true)
	{
	}
	else
	{

		if (pCurrentUser->m_nOutboundPort!=0)
		{
			ssStrTemp << pCurrentUser->m_strOutboundServer << ":" << pCurrentUser->m_nOutboundPort;
		}
		else
		{
			ssStrTemp << pCurrentUser->m_strOutboundServer;
		}
	}
	m_pReOutboundServer->SetWindowText(PortUtility::string2WString(ssStrTemp.str()).c_str());


	m_pCmbSignalingTransport->SetCurSel(pCurrentUser->m_nTransport);
	m_pCmbSrtp->SetCurSel(pCurrentUser->m_nSrtpMode);
	m_pChkVerifyTls->SetCheck((pCurrentUser->m_bTLSVerify));
	//m_pCmbPresenceMode->SetCurSel(pCurrentUser->m_nPresencMode);
	return 0;
}

LRESULT CLoginSettingsWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	CenterWindow();
	return 0;
}

LRESULT CLoginSettingsWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CLoginSettingsWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
			OnButtonClicked(pe);
		}
	}
	if (uMsg == WM_COMMAND)
	{
		if (HIWORD(pe->wParam) == CBN_SELCHANGE)
		{
			//if(m_pCmbPresenceMode->GetSafeHwnd() == pe->hOriginalSender)
			//{
			//	
			//	CUser* pCurrentUser = CSystemEx::GetInstance()->GetGlobalUser();
			//}
			
		}
	}
	else if (pe->RoutedEvent == UE_AFTERPAINTBKIMAGE)
	{
		if ( GetName(pe->hOriginalSender) == _T("IDC_RE_OUTBOUND_SERVER"))
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
				
				strPrompt = _T("example :192.1.168.2:20");
				
				DrawFormatText(hDC, strPrompt, strPrompt.GetLength(),
					rcClient, &rcTextPadding,
					TAH_LEFT, TAV_TOP, DT_SINGLELINE, hDefFont->GetSafeHFont(),
					RGB(128, 128, 128), FALSE);
			}
		}
	}
	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CLoginSettingsWnd::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CLoginSettingsWnd::OnChkMax(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CLoginSettingsWnd::OnBtnClose(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CLoginSettingsWnd::OnChkVerifyTls(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CLoginSettingsWnd::OnBtnReturn(RoutedEventArgs *pe)
{
	PostMessage(WM_CLOSE, 0, 0);
	
	return 0;
}

LRESULT CLoginSettingsWnd::OnBtnSave(RoutedEventArgs *pe)
{
	std::string strErrorInfo = "";
	if (VerifyParam(strErrorInfo) == true)
	{
		Save2Memery();
	}
	else
	{
		InfoWndParam *pParam = new InfoWndParam;
		pParam->strMsg = PortUtility::string2WString(strErrorInfo).c_str();
		pParam->strTitle = _T("Info");
		pParam->nAutoTimer = 3000;
		CSystemEx::GetInstance()->GetCruuentLoginDlg()->SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
	}
	PostMessage(WM_CLOSE, 0, 0);
	return 0;
}
void CLoginSettingsWnd::Save2Memery()
{
	//presence mode
	//int nIndex = m_pCmbPresenceMode->GetCurSel();
	//CSystemEx::GetInstance()->GetGlobalUser()->SetPresenceMode(nIndex);
	//displayname 
	CIUIString strDisplayName = m_pReDisplayName->GetWindowText();
	std::string strUTF8DisplayName = PortUtility::Unicode2Utf8_ND(strDisplayName.GetBuffer(strDisplayName.GetLength()));
	CSystemEx::GetInstance()->GetGlobalUser()->m_strDisplayName = strUTF8DisplayName;
	//auth name
	CIUIString strAuthName = m_pReAuthroizationName->GetWindowText();
	std::string strUTF8AuthName = PortUtility::Unicode2Utf8_ND(strAuthName.GetBuffer(strAuthName.GetLength()));
	CSystemEx::GetInstance()->GetGlobalUser()->m_strAuthName = strUTF8AuthName;
	//Outband server
	CIUIString strOutbandServer = m_pReOutboundServer->GetWindowText();
	CIUIString strIP, strPort;

	int nIndex = m_pCmbSignalingTransport->GetCurSel();

	if (!strOutbandServer.IsEmpty())
	{
		int nPos = strOutbandServer.Find(L":");
		if (nPos < 0)
		{
			if (nIndex != TRANSPORT_UDP)
			{
				InfoWndParam *pParam = new InfoWndParam;
				pParam->strMsg = L"outband server format maybe is error,the format should be ip:port,if did't have port client will use port 5060";
				pParam->strTitle = _T("Info");
				pParam->nAutoTimer = 3000;
				CSystemEx::GetInstance()->GetCruuentLoginDlg()->SendMessage(WM_CREATE_INFO_WND, (WPARAM)pParam, 0);
			}
			strIP = strOutbandServer;
			std::string strUTF8OutbandServer, strUTF8OutbandPort;
			strUTF8OutbandServer = PortUtility::Unicode2Utf8_ND(strIP.GetBuffer(strIP.GetLength()));
			CSystemEx::GetInstance()->GetGlobalUser()->m_strOutboundServer = strUTF8OutbandServer;
			CSystemEx::GetInstance()->GetGlobalUser()->m_nOutboundPort = 0;
		}
		else
		{
			strIP = strOutbandServer.Left(nPos);
			strPort = strOutbandServer.Right(strOutbandServer.GetLength() - nPos - 1);
			std::string strUTF8OutbandServer, strUTF8OutbandPort;
			strUTF8OutbandServer = PortUtility::Unicode2Utf8_ND(strIP.GetBuffer(strIP.GetLength()));
			CSystemEx::GetInstance()->GetGlobalUser()->m_strOutboundServer = strUTF8OutbandServer;
			strUTF8OutbandPort = PortUtility::Unicode2Utf8_ND(strPort.GetBuffer(strPort.GetLength()));
			CSystemEx::GetInstance()->GetGlobalUser()->m_nOutboundPort = atoi(strUTF8OutbandPort.c_str());
		}	
	}
	else
	{
		CSystemEx::GetInstance()->GetGlobalUser()->m_strOutboundServer = "";
	}
	//m_pCmbSignalingTransport
	
	CSystemEx::GetInstance()->GetGlobalUser()->m_nTransport = nIndex;
	//m_pCmbSrtp
	nIndex = m_pCmbSrtp->GetCurSel();
	CSystemEx::GetInstance()->GetGlobalUser()->m_nSrtpMode = nIndex;
	//TLS
	BOOL bCheck = m_pChkVerifyTls->GetCheck();
	CSystemEx::GetInstance()->GetGlobalUser()->m_bTLSVerify = bCheck;
	//
	CIUIString strPollTime = m_pRePollTime->GetWindowText();
	CSystemEx::GetInstance()->GetGlobalUser()->m_nSubscribeRefreshTime = atoi(PortUtility::Unicode2Utf8_ND(strPollTime.GetBuffer(strPollTime.GetLength())).c_str());
	//
	CIUIString strRefreshTime = m_pReRefreshInterval->GetWindowText();
	CSystemEx::GetInstance()->GetGlobalUser()->m_nPublishRefreshTime = atoi(PortUtility::Unicode2Utf8_ND(strRefreshTime.GetBuffer(strRefreshTime.GetLength())).c_str());
}
LRESULT CLoginSettingsWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnMin->GetSafeHwnd() == pe->hOriginalSender)
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
	else if (m_pChkVerifyTls->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkVerifyTls(pe);
	}
	else if (m_pBtnReturn->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnReturn(pe);
	}
	else if (m_pBtnSave->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSave(pe);
	}
	
	return 0;
}

bool CLoginSettingsWnd::VerifyParam(std::string& strErrorMsg)
{
	/*
	PROXY_SERVER_STRING_LENGTH 	
	 */   
	int nStringLength = 0;
	CIUIString strText = m_pReOutboundServer->GetWindowText();
	nStringLength = strText.GetLength();
	if (nStringLength>OUT_BOUND_STRING_LENGTH)
	{
		strErrorMsg = "Out Bound String Too Long";
		LOG4_INFO(strErrorMsg.c_str());
		return false;
	}
	strText = m_pReAuthroizationName->GetWindowText();
	nStringLength = strText.GetLength();
	if (nStringLength > AUTH_NAME_STRING_LENGTH)
	{
		strErrorMsg = "Auth Name String Too Long";
		LOG4_INFO(strErrorMsg.c_str());
		return false;
	}

	strText = m_pReDisplayName->GetWindowText();
	nStringLength = strText.GetLength();
	if (nStringLength > DISPLAY_NAME_STRING_LENGTH)
	{
		strErrorMsg = "Display Name String Too Long";
		LOG4_INFO(strErrorMsg.c_str());
		return false;
	}

	strText = m_pRePollTime->GetWindowText();
	if (strText.SpanIncluding(_T("0123456789"))== strText)
	{

	}
	else
	{
		strErrorMsg = "PollTime should be digit";
		LOG4_INFO(strErrorMsg.c_str());
		return false;
	}
	
	return true;
}
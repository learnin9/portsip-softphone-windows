#include "stdafx.h"
#include "SettingsMiscCanvasHandler.h"
#include "System/SystemEx.h"

CSettingsMiscCanvasHandler::CSettingsMiscCanvasHandler()
	: m_pStaRtpPorts(NULL)
	, m_pStaVideoRange(NULL)
	, m_pReVideoFrom(NULL)
	, m_pStaTo(NULL)
	, m_pReVideoTo(NULL)
	, m_pStaAudioRange(NULL)
	, m_pReAudioFrom(NULL)
	, m_pStaAudioTo(NULL)
	, m_pReAudioTo(NULL)
	, m_pStaEnableRtp(NULL)
	, m_pChkEnableRtp(NULL)
	, m_pStaSplitter3(NULL)
	, m_pStaEnableMwi(NULL)
	, m_pChkEnableMwi(NULL)
	, m_pStaSplitter4(NULL)
	, m_pStaEnableSessionTimer(NULL)
	, m_pChkEnableSessionTimer(NULL)
	, m_pReSessionTimer(NULL)
	, m_pStaSessionTimer(NULL)
	, m_pStaSplitter5(NULL)
	, m_pStaAlwaysForwardTo(NULL)
	, m_pReAlwaysForwardTo(NULL)
	, m_pStaSplitter6(NULL)
	, m_pStaOnPhone(NULL)
	, m_pReOnPhone(NULL)
	, m_pStaSplitter7(NULL)
	, m_pChkAlwaysForwardTo(NULL)
	, m_pChkOnPhone(NULL)
{
	m_bIsOpen = false;
}

CSettingsMiscCanvasHandler::~CSettingsMiscCanvasHandler()
{
}

int CSettingsMiscCanvasHandler::InitControls()
{
	m_pStaRtpPorts = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_RTP_PORTS")));
	_ASSERT(m_pStaRtpPorts != NULL);
	m_pStaVideoRange = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_VIDEO_RANGE")));
	_ASSERT(m_pStaVideoRange != NULL);
	m_pReVideoFrom = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_VIDEO_FROM")));
	_ASSERT(m_pReVideoFrom != NULL);
	m_pStaTo = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_TO")));
	_ASSERT(m_pStaTo != NULL);
	m_pReVideoTo = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_VIDEO_TO")));
	_ASSERT(m_pReVideoTo != NULL);
	m_pStaAudioRange = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_AUDIO_RANGE")));
	_ASSERT(m_pStaAudioRange != NULL);
	m_pReAudioFrom = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_AUDIO_FROM")));
	_ASSERT(m_pReAudioFrom != NULL);
	m_pStaAudioTo = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_AUDIO_TO")));
	_ASSERT(m_pStaAudioTo != NULL);
	m_pReAudioTo = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_AUDIO_TO")));
	_ASSERT(m_pReAudioTo != NULL);
	m_pStaEnableRtp = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ENABLE_RTP")));
	_ASSERT(m_pStaEnableRtp != NULL);
	m_pChkEnableRtp = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ENABLE_RTP")));
	_ASSERT(m_pChkEnableRtp != NULL);
	m_pStaSplitter3 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER3")));
	_ASSERT(m_pStaSplitter3 != NULL);
	m_pStaEnableMwi = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ENABLE_MWI")));
	_ASSERT(m_pStaEnableMwi != NULL);
	m_pChkEnableMwi = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ENABLE_MWI")));
	_ASSERT(m_pChkEnableMwi != NULL);
	m_pStaSplitter4 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER4")));
	_ASSERT(m_pStaSplitter4 != NULL);
	m_pStaEnableSessionTimer = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ENABLE_SESSION_TIMER")));
	_ASSERT(m_pStaEnableSessionTimer != NULL);
	m_pChkEnableSessionTimer = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ENABLE_SESSION_TIMER")));
	_ASSERT(m_pChkEnableSessionTimer != NULL);
	m_pReSessionTimer = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_SESSION_TIMER")));
	_ASSERT(m_pReSessionTimer != NULL);
	m_pStaSessionTimer = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SESSION_TIMER")));
	_ASSERT(m_pStaSessionTimer != NULL);
	m_pStaSplitter5 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER5")));
	_ASSERT(m_pStaSplitter5 != NULL);
	m_pStaAlwaysForwardTo = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ALWAYS_FORWARD_TO")));
	_ASSERT(m_pStaAlwaysForwardTo != NULL);
	m_pReAlwaysForwardTo = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_ALWAYS_FORWARD_TO")));
	_ASSERT(m_pReAlwaysForwardTo != NULL);
	m_pStaSplitter6 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER6")));
	_ASSERT(m_pStaSplitter6 != NULL);
	m_pStaOnPhone = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ON_PHONE")));
	_ASSERT(m_pStaOnPhone != NULL);
	m_pReOnPhone = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_ON_PHONE")));
	_ASSERT(m_pReOnPhone != NULL);
	m_pStaSplitter7 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER7")));
	_ASSERT(m_pStaSplitter7 != NULL);
	m_pChkAlwaysForwardTo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ALWAYS_FORWARD_TO")));
	_ASSERT(m_pChkAlwaysForwardTo != NULL);
	m_pChkOnPhone = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ON_PHONE")));
	_ASSERT(m_pChkOnPhone != NULL);
	return 0;
}

int CSettingsMiscCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	m_bIsOpen = true;
	LoadOptions();
	return 0;
}

LRESULT CSettingsMiscCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
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

LRESULT CSettingsMiscCanvasHandler::OnChkEnableRtp(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMiscCanvasHandler::OnChkEnableMwi(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMiscCanvasHandler::OnChkEnableSessionTimer(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMiscCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkEnableRtp->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEnableRtp(pe);
	}
	else if (m_pChkEnableMwi->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEnableMwi(pe);
	}
	else if (m_pChkEnableSessionTimer->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEnableSessionTimer(pe);
	}
	else if (m_pChkAlwaysForwardTo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkAlwaysForwardTo(pe);
	}
	else if (m_pChkOnPhone->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkOnPhone(pe);
	}

	return 0;
}
void CSettingsMiscCanvasHandler::LoadOptions()
{
	COptions* pOptions = CSystemEx::GetInstance()->GetGlobalOptions();

	CString strAudioRtpMinPort, strAudioRtpMaxPort, strVideoRtpMinPort,strVideoRtpMaxPort;
	strAudioRtpMinPort.Format(L"%d", pOptions->m_nAudioRtpMinPort);
	strAudioRtpMaxPort.Format(L"%d", pOptions->m_nAudioRtpMaxPort);
	strVideoRtpMinPort.Format(L"%d", pOptions->m_nVideoRtpMinPort);
	strVideoRtpMaxPort.Format(L"%d", pOptions->m_nVideoRtpMaxPort);

	m_pReAudioFrom->SetWindowText(strAudioRtpMinPort);
	m_pReAudioTo->SetWindowText(strAudioRtpMaxPort);
	m_pReVideoFrom->SetWindowText(strVideoRtpMinPort);
	m_pReVideoTo->SetWindowText(strVideoRtpMaxPort);
	
	//mark g_options.RtpKeepLive == true)
	m_pChkEnableRtp->SetCheck(pOptions->m_bRTPKeepLive);
	m_pChkEnableMwi->SetCheck(pOptions->m_bEnableMWI);
	m_pChkEnableSessionTimer->SetCheck(pOptions->m_bEnableSessionTimer);

	CString strSessionTimerTime;
	strSessionTimerTime.Format(L"%d", pOptions->m_nSessionTimerTime);
	m_pReSessionTimer->SetWindowText(strSessionTimerTime);

	
	m_pChkAlwaysForwardTo->SetCheck(pOptions->m_bEnableForword);
	m_pChkOnPhone->SetCheck(pOptions->m_bEnableOnPhoneForword);
	
	if (pOptions->m_bEnableForword)
	{
		//m_pReAlwaysForwardTo->SetReadOnly(TRUE);
		EnableWindow(m_pReAlwaysForwardTo->GetSafeHwnd(), TRUE);
	}
	else
	{
		EnableWindow(m_pReAlwaysForwardTo->GetSafeHwnd(), FALSE);
	}
	if (pOptions->m_bEnableOnPhoneForword )
	{
		EnableWindow(m_pReOnPhone->GetSafeHwnd(), TRUE);
	}
	else
	{
		EnableWindow(m_pReOnPhone->GetSafeHwnd(), FALSE);
	}
	m_pReAlwaysForwardTo->SetWindowText(PortUtility::Utf82Unicode_ND(pOptions->m_strForwardTo).c_str());
	m_pReOnPhone->SetWindowText(PortUtility::Utf82Unicode_ND(pOptions->m_strBusyForwardTo).c_str());
}
void CSettingsMiscCanvasHandler::Save()
{
	if (m_bIsOpen == false)
	{
		return;
	}
	COptions *pOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pOptions == NULL)
	{
		return;
	}
	std::wstring  strTempMin;
	strTempMin = m_pReAudioFrom->GetWindowText();
	std::wistringstream ssTempMin(strTempMin);
	ssTempMin >> pOptions->m_nAudioRtpMinPort;

	std::wstring  strTempMax;
	strTempMax = m_pReAudioTo->GetWindowText();
	std::wistringstream ssTempMax(strTempMax);
	ssTempMax >> pOptions->m_nAudioRtpMinPort;

	strTempMin = m_pReAudioFrom->GetWindowText();
	std::wistringstream ssTempMin2(strTempMin);
	ssTempMin2 >> pOptions->m_nVideoRtpMinPort;

	strTempMax = m_pReVideoTo->GetWindowText();
	std::wistringstream ssTempMax2(strTempMax);
	ssTempMax2 >> pOptions->m_nVideoRtpMaxPort;

	pOptions->m_bRTPKeepLive = (bool)m_pChkEnableRtp->GetCheck();
	pOptions->m_bEnableMWI = (bool)m_pChkEnableMwi->GetCheck();
	pOptions->m_bEnableSessionTimer = (bool)m_pChkEnableSessionTimer->GetCheck();


	strTempMax = m_pReSessionTimer->GetWindowText();
	std::wistringstream ssTempSessionTimer(strTempMax);
	ssTempMax2 >> pOptions->m_nVideoRtpMaxPort;
	
	//pOptions->m_bawalys = (bool)m_pChkEnableSessionTimer->GetCheck();
	//pOptions->m_bEnableSessionTimer = (bool)m_pChkEnableSessionTimer->GetCheck();
	pOptions->m_bEnableForword = m_pChkAlwaysForwardTo->GetCheck();
	pOptions->m_bEnableOnPhoneForword = m_pChkOnPhone->GetCheck();
	std::wstring strForward = m_pReAlwaysForwardTo->GetWindowText();
	pOptions->m_strForwardTo = PortUtility::Unicode2Utf8_ND(strForward);
	
	
	strForward = m_pReOnPhone->GetWindowText();
	pOptions->m_strBusyForwardTo = PortUtility::Unicode2Utf8_ND(strForward);

	CSystemEx::GetInstance()->InitCallForwardingSettings();
	if (pOptions->m_nAudioRtpMinPort >= 0 && pOptions->m_nAudioRtpMaxPort >= 0 && pOptions->m_nVideoRtpMinPort >= 0 && pOptions->m_nVideoRtpMaxPort >= 0)
	{

		PortSIP_setRtpPortRange(CSystemEx::GetInstance()->GetSipLib(),
			pOptions->m_nAudioRtpMinPort,
			pOptions->m_nAudioRtpMaxPort,
			pOptions->m_nVideoRtpMinPort,
			pOptions->m_nVideoRtpMaxPort);
	}
	CSystemEx::GetInstance()->InitCallForwardingSettings();

}

LRESULT CSettingsMiscCanvasHandler::OnChkAlwaysForwardTo(RoutedEventArgs *pe)
{
	
	if (m_pChkAlwaysForwardTo->GetCheck())
	{
		EnableWindow(m_pReAlwaysForwardTo->GetSafeHwnd(), TRUE);
	}
	else
	{
		EnableWindow(m_pReAlwaysForwardTo->GetSafeHwnd(), FALSE);
	}
	return 0;
}

LRESULT CSettingsMiscCanvasHandler::OnChkOnPhone(RoutedEventArgs *pe)
{
	
	if (m_pChkOnPhone->GetCheck())
	{
		EnableWindow(m_pReOnPhone->GetSafeHwnd(), TRUE);
	}
	else
	{
		EnableWindow(m_pReOnPhone->GetSafeHwnd(), FALSE);
	}
	
	return 0;
}
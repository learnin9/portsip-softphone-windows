#include "stdafx.h"
#include "SettingsMediaQualityHandler.h"
#include "System/SystemEx.h"

CSettingsMediaQualityHandler::CSettingsMediaQualityHandler()
	: m_pStaAes(NULL)
	, m_pChkAes(NULL)
	, m_pStaSplitter1(NULL)
	, m_pStaReduceNoise(NULL)
	, m_pChkReduceNoise(NULL)
	, m_pStaSplitter2(NULL)
	, m_pStaAutomaticControl(NULL)
	, m_pChkAutomaticControl(NULL)
	, m_pStaSplitter3(NULL)
	, m_pStaVoiceDetection(NULL)
	, m_pChkVoiceDetection(NULL)
	, m_pStaSplitter4(NULL)
	, m_pStaComfortNoiseGenerator(NULL)
	, m_pChkComfortNoiseGenerator(NULL)
	, m_pStaSplitter5(NULL)
	, m_pStaEnableAudioQos(NULL)
	, m_pChkEnableAudioQos(NULL)
	, m_pStaSplitter6(NULL)
	, m_pStaEnableVideoQos(NULL)
	, m_pChkEnableVideoQos(NULL)
	, m_pStaSplitter7(NULL)
	, m_pStaVideoQuality(NULL)
	, m_pStaDes(NULL)
	, m_pStaResolution(NULL)
	, m_pCmbResolution(NULL)
	, m_pStaBitrate(NULL)
	, m_pCmbBitrate(NULL)
	, m_pStaKbps(NULL)
{
	m_bIsOpen = false;
}

CSettingsMediaQualityHandler::~CSettingsMediaQualityHandler()
{
}

int CSettingsMediaQualityHandler::InitControls()
{
	m_pStaAes = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_AES")));
	_ASSERT(m_pStaAes != NULL);
	m_pChkAes = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_AES")));
	_ASSERT(m_pChkAes != NULL);
	m_pStaSplitter1 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER1")));
	_ASSERT(m_pStaSplitter1 != NULL);
	m_pStaReduceNoise = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_REDUCE_NOISE")));
	_ASSERT(m_pStaReduceNoise != NULL);
	m_pChkReduceNoise = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_REDUCE_NOISE")));
	_ASSERT(m_pChkReduceNoise != NULL);
	m_pStaSplitter2 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER2")));
	_ASSERT(m_pStaSplitter2 != NULL);
	m_pStaAutomaticControl = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_AUTOMATIC_CONTROL")));
	_ASSERT(m_pStaAutomaticControl != NULL);
	m_pChkAutomaticControl = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_AUTOMATIC_CONTROL")));
	_ASSERT(m_pChkAutomaticControl != NULL);
	m_pStaSplitter3 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER3")));
	_ASSERT(m_pStaSplitter3 != NULL);
	m_pStaVoiceDetection = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_VOICE_DETECTION")));
	_ASSERT(m_pStaVoiceDetection != NULL);
	m_pChkVoiceDetection = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_VOICE_DETECTION")));
	_ASSERT(m_pChkVoiceDetection != NULL);
	m_pStaSplitter4 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER4")));
	_ASSERT(m_pStaSplitter4 != NULL);
	m_pStaComfortNoiseGenerator = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_COMFORT_NOISE_GENERATOR")));
	_ASSERT(m_pStaComfortNoiseGenerator != NULL);
	m_pChkComfortNoiseGenerator = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_COMFORT_NOISE_GENERATOR")));
	_ASSERT(m_pChkComfortNoiseGenerator != NULL);
	m_pStaSplitter5 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER5")));
	_ASSERT(m_pStaSplitter5 != NULL);
	m_pStaEnableAudioQos = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ENABLE_AUDIO_QOS")));
	_ASSERT(m_pStaEnableAudioQos != NULL);
	m_pChkEnableAudioQos = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ENABLE_AUDIO_QOS")));
	_ASSERT(m_pChkEnableAudioQos != NULL);
	m_pStaSplitter6 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER6")));
	_ASSERT(m_pStaSplitter6 != NULL);
	m_pStaEnableVideoQos = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ENABLE_VIDEO_QOS")));
	_ASSERT(m_pStaEnableVideoQos != NULL);
	m_pChkEnableVideoQos = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_ENABLE_VIDEO_QOS")));
	_ASSERT(m_pChkEnableVideoQos != NULL);
	m_pStaSplitter7 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER7")));
	_ASSERT(m_pStaSplitter7 != NULL);
	m_pStaVideoQuality = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_VIDEO_QUALITY")));
	_ASSERT(m_pStaVideoQuality != NULL);
	m_pStaDes = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_DES")));
	_ASSERT(m_pStaDes != NULL);
	m_pStaResolution = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_RESOLUTION")));
	_ASSERT(m_pStaResolution != NULL);
	m_pCmbResolution = (ComboBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CMB_RESOLUTION")));
	_ASSERT(m_pCmbResolution != NULL);
	m_pStaBitrate = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_BITRATE")));
	_ASSERT(m_pStaBitrate != NULL);
	m_pCmbBitrate = (ComboBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CMB_BITRATE")));
	_ASSERT(m_pCmbBitrate != NULL);
	m_pStaKbps = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_KBPS")));
	_ASSERT(m_pStaKbps != NULL);

	return 0;
}

int CSettingsMediaQualityHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	LoadOptions();
	m_bIsOpen = true;
	return 0;
}

LRESULT CSettingsMediaQualityHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

LRESULT CSettingsMediaQualityHandler::OnChkAes(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkReduceNoise(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkAutomaticControl(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkVoiceDetection(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkComfortNoiseGenerator(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkEnableAudioQos(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnChkEnableVideoQos(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsMediaQualityHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkAes->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkAes(pe);
	}
	else if (m_pChkReduceNoise->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkReduceNoise(pe);
	}
	else if (m_pChkAutomaticControl->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkAutomaticControl(pe);
	}
	else if (m_pChkVoiceDetection->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkVoiceDetection(pe);
	}
	else if (m_pChkComfortNoiseGenerator->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkComfortNoiseGenerator(pe);
	}
	else if (m_pChkEnableAudioQos->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEnableAudioQos(pe);
	}
	else if (m_pChkEnableVideoQos->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEnableVideoQos(pe);
	}

	return 0;
}

void CSettingsMediaQualityHandler::LoadOptions()
{
	m_pCmbResolution->AddString(L"QCIF");
	m_pCmbResolution->AddString(L"CIF");
	m_pCmbResolution->AddString(L"VGA");
	m_pCmbResolution->AddString(L"SVGA");
	m_pCmbResolution->AddString(L"XVGA");
	m_pCmbResolution->AddString(L"720P");
	m_pCmbResolution->SetCurSel(0);

	m_pCmbBitrate->AddString(L"128");
	m_pCmbBitrate->AddString(L"256");
	m_pCmbBitrate->AddString(L"512");
	m_pCmbBitrate->AddString(L"640");
	m_pCmbBitrate->AddString(L"1024");
	m_pCmbBitrate->AddString(L"1500");
	m_pCmbBitrate->AddString(L"2000");
	m_pCmbBitrate->SetCurSel(0);

	COptions* pOptions = CSystemEx::GetInstance()->GetGlobalOptions();

	m_pChkAes->SetCheck(pOptions->m_bAEC);
	m_pChkReduceNoise->SetCheck(pOptions->m_bAGC);
	m_pChkAutomaticControl->SetCheck(pOptions->m_bANS);
	m_pChkVoiceDetection->SetCheck(pOptions->m_bVAD);
	m_pChkComfortNoiseGenerator->SetCheck(pOptions->m_bCNG);
	m_pChkComfortNoiseGenerator->SetCheck(pOptions->m_bCNG);
	m_pChkEnableAudioQos->SetCheck(pOptions->m_bEAQ);
	m_pChkEnableVideoQos->SetCheck(pOptions->m_bVAD);
	m_pCmbResolution->SetCurSel(pOptions->m_nVideoResolution);
	m_pCmbBitrate->SetCurSel(pOptions->m_nVideoImageQualityLevel);
}

void CSettingsMediaQualityHandler::Save()
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
	if (pOptions->m_nVideoResolution != m_pCmbResolution->GetCurSel())
	{
	}
	else
	{
	}

	pOptions->m_nVideoResolution = m_pCmbResolution->GetCurSel();
	pOptions->m_nVideoImageQualityLevel = m_pCmbBitrate->GetCurSel();
	pOptions->m_bAEC = (bool)m_pChkAes->GetCheck();
	pOptions->m_bAGC = (bool)m_pChkReduceNoise->GetCheck();
	pOptions->m_bANS = (bool)m_pChkAutomaticControl->GetCheck();
	pOptions->m_bVAD = (bool)m_pChkVoiceDetection->GetCheck();
	pOptions->m_bCNG = (bool)m_pChkComfortNoiseGenerator->GetCheck();
	pOptions->m_bEAQ = (bool)m_pChkEnableAudioQos->GetCheck();
	pOptions->m_bEVQ = (bool)m_pChkEnableVideoQos->GetCheck();

	if (pOptions->m_bAEC)
	{
		PortSIP_enableAEC(CSystemEx::GetInstance()->GetSipLib(), EC_DEFAULT);
	}
	else
	{
		PortSIP_enableAEC(CSystemEx::GetInstance()->GetSipLib(), EC_NONE);
	}
	if (pOptions->m_bAGC)
	{
		PortSIP_enableAGC(CSystemEx::GetInstance()->GetSipLib(), AGC_DEFAULT);
	}
	else
	{
		PortSIP_enableAGC(CSystemEx::GetInstance()->GetSipLib(), AGC_NONE);
	}
	if (pOptions->m_bANS)
	{
		PortSIP_enableANS(CSystemEx::GetInstance()->GetSipLib(), NS_DEFAULT);
	}
	else
	{
		PortSIP_enableANS(CSystemEx::GetInstance()->GetSipLib(), NS_NONE);
	}

	PortSIP_enableVAD(CSystemEx::GetInstance()->GetSipLib(), pOptions->m_bVAD);
	PortSIP_enableCNG(CSystemEx::GetInstance()->GetSipLib(), pOptions->m_bCNG);

	TagScreenMetrics tagLocalVideoMetrics = PortUtility::transVideoResolution(pOptions->m_nVideoResolution);
	PortSIP_setVideoResolution(CSystemEx::GetInstance()->GetSipLib(), tagLocalVideoMetrics.nFrameWidth, tagLocalVideoMetrics.nFrameHeight);
	//int nBitrate = PortUtility::transVideoBitrate(pOptions->m_nVideoImageQualityLevel);
	//PortSIP_setVideoBitrate(CSystemEx::GetInstance()->GetSipLib(),,)
	/*
	stringstream sql;
	sql << "Update ";
	sql << TABLE_OPTIONS;
	sql << " set ";
	sql << "AGC = '";
	sql << agc;
	sql << "', VAD = '";
	sql << vad;
	sql << "', CNG = '";
	sql << cng;
	sql << "', AEC = '";
	sql << aec;
	sql << "', ANS = '";
	sql << ans;
	sql << "', VideoImageQualityLevel = '";
	sql << g_options.VideoImageQualityLevel;
	sql << "', VideoResolution = '";
	sql << g_options.VideoResolution;
	sql << "'";



	string errnoMessage;
	g_optionsDB.exec(sql.str().c_str(), NULL, NULL, errnoMessage);
	*/
}
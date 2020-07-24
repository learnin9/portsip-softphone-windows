#include "stdafx.h"
#include "SpeakerDlg.h"
#include "System/SystemEx.h"
#define LOCAL_VIDEO_WND_CLASS _T("LocalVideoWndClass")

LRESULT CALLBACK LocalVideoWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_DESTROY:
		break;

	default:
		break;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL RegisterLocalVideoWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = LocalVideoWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = LOCAL_VIDEO_WND_CLASS;
	ATOM ret = ::RegisterClass(&wc);
	_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND CreateLocalVideoWnd(HWND hParent)
{
	BOOL bRet = RegisterLocalVideoWindowClass();
	if (!bRet)
	{
		return NULL;
	}

	HWND hWnd = ::CreateWindowEx(0, LOCAL_VIDEO_WND_CLASS,
		NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
		hParent, 0, IUIGetInstanceHandle(), 0);

	return hWnd;
}
CSpeakerDlg::CSpeakerDlg()
	: m_pStaTitle(NULL)
	, m_pBtnClose(NULL)
	, m_pStaPic(NULL)
	, m_pStaCameraOn(NULL)
	, m_pChkCameraOn(NULL)
	, m_pStaSpeaker(NULL)
	, m_pChkSpeaker(NULL)
	, m_pCmbSpeaker(NULL)
	, m_pBtnTestSound(NULL)
	, m_pStaMicrophone(NULL)
	, m_pChkMicrophone(NULL)
	, m_pCmbMicrophone(NULL)
	, m_pBtnOk(NULL)
	, m_hLocalVideoDlg(NULL)
	, m_pCmbSelectDevice(NULL)
	, m_pSldSpeaker(NULL)
	, m_pSldMicrophone(NULL)
{
	m_AudioLoopbackTest = false;
}

CSpeakerDlg::~CSpeakerDlg()
{
	
}

int CSpeakerDlg::InitControls()
{
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pStaPic = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_PIC")));
	_ASSERT(m_pStaPic != NULL);
	m_pStaCameraOn = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_CAMERA_ON")));
	_ASSERT(m_pStaCameraOn != NULL);
	m_pChkCameraOn = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_CAMERA_ON")));
	_ASSERT(m_pChkCameraOn != NULL);
	m_pStaSpeaker = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SPEAKER")));
	_ASSERT(m_pStaSpeaker != NULL);
	m_pChkSpeaker = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_SPEAKER")));
	_ASSERT(m_pChkSpeaker != NULL);
	m_pCmbSpeaker = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_SPEAKER")));
	_ASSERT(m_pCmbSpeaker != NULL);
	m_pBtnTestSound = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_TEST_SOUND")));
	_ASSERT(m_pBtnTestSound != NULL);
	m_pStaMicrophone = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_MICROPHONE")));
	_ASSERT(m_pStaMicrophone != NULL);
	m_pChkMicrophone = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MICROPHONE")));
	_ASSERT(m_pChkMicrophone != NULL);
	m_pCmbMicrophone = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_MICROPHONE")));
	_ASSERT(m_pCmbMicrophone != NULL);
	m_pBtnOk = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_OK")));
	_ASSERT(m_pBtnOk != NULL);
	m_pCmbSelectDevice = (ComboBox *)CWLWnd::FromHandle(FindControl(_T("IDC_CMB_SELECT_DEVICE")));
	_ASSERT(m_pCmbSelectDevice != NULL);
	m_pSldSpeaker = (Slider *)CWLWnd::FromHandle(FindControl(_T("IDC_SLD_SPEAKER")));
	_ASSERT(m_pSldSpeaker != NULL);
	m_pSldMicrophone = (Slider *)CWLWnd::FromHandle(FindControl(_T("IDC_SLD_MICROPHONE")));
	_ASSERT(m_pSldMicrophone != NULL);

	loadOptions();
	return 0;
}

LRESULT CSpeakerDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	if (!::IsWindow(m_hLocalVideoDlg))
	{
		m_hLocalVideoDlg = CreateLocalVideoWnd(GetHWND());
		CIUIRect rcVideoCanvas;
		m_pStaPic->GetWindowRectToHWND(rcVideoCanvas);
		::MoveWindow(m_hLocalVideoDlg, rcVideoCanvas.left, rcVideoCanvas.top,
			rcVideoCanvas.Width(), rcVideoCanvas.Height(), TRUE);
	}

	CenterWindow();
	UpdateVideoData();
	Invalidate();
	//PostMessage(WM_DOWNLOAD_FILE_OK);
	return 0;
}



LRESULT CSpeakerDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CSpeakerDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (WM_HSCROLL == pe->RoutedEvent)
		{
			return OnHScroll(pe);
		}
		if (uMsg == WM_COMMAND)
		{
			if (HIWORD(pe->wParam) == CBN_SELCHANGE)
			{
				COptions *pGlobalOptions = CSystemEx::GetInstance()->GetGlobalOptions();
				//camare
				int nVideoDeviceID = pGlobalOptions->m_nVideoDeviceID;
				if (pGlobalOptions->m_bEnableVideoDev == false)
				{
					nVideoDeviceID = -1;
				}
				else
				{
					if (m_pCmbSelectDevice->GetSafeHwnd() == pe->hOriginalSender)
					{
						int nIndex = m_pCmbSelectDevice->GetCurSel();

						if (pGlobalOptions->m_nVideoDeviceID != nIndex)
						{
							if (pGlobalOptions->m_nVideoDeviceID != -1)
							{
								pGlobalOptions->m_nVideoDeviceID = nIndex;
								int nErrorCode = PortSIP_setVideoDeviceId(CSystemEx::GetInstance()->GetSipLib(), nIndex);
								if (nErrorCode < 0)
								{
									std::string strError = CSystemEx::GetInstance()->GetSipErrorInfo(nErrorCode);
									LOG4_INFO(strError.c_str());
								}
								else
								{
									UpdateVideoData();
								}
							}
						}
					}
				}
				
				int nLocalAudioInputDeviceID = pGlobalOptions->m_nAudioInputDeviceID;
				int nLocalAudioOutputDeviceID = pGlobalOptions->m_nAudioOutputDeviceID;
				bool bHasChange = false;
				//audio speaker 
				if (m_pCmbSpeaker->GetSafeHwnd() == pe->hOriginalSender)
				{
					int nIndex = m_pCmbSpeaker->GetCurSel();
					
					if (pGlobalOptions->m_nAudioOutputDeviceID != nIndex)
					{
						if (pGlobalOptions->m_nAudioOutputDeviceID != -1)
						{
							pGlobalOptions->m_nAudioOutputDeviceID = nIndex;
							nLocalAudioOutputDeviceID = nIndex;
							bHasChange = true;
						}
					}
				}

				//audio mic 
				if (m_pCmbMicrophone->GetSafeHwnd() == pe->hOriginalSender)
				{
					int nIndex = m_pCmbMicrophone->GetCurSel();
					if (pGlobalOptions->m_nAudioInputDeviceID != nIndex)
					{
						if (pGlobalOptions->m_nAudioInputDeviceID != -1)
						{
							pGlobalOptions->m_nAudioInputDeviceID = nIndex;
							nLocalAudioInputDeviceID = nIndex;
							bHasChange = true;
						}
					}
				}

				if (m_pChkMicrophone->GetCheck() == false)
				{
					nLocalAudioInputDeviceID = -1;
				}

				if (m_pChkSpeaker->GetCheck() == false)
				{
					nLocalAudioOutputDeviceID = -1;
				}
				if (bHasChange==true)
				{
					int nReslut = PortSIP_setAudioDeviceId(CSystemEx::GetInstance()->GetSipLib(), pGlobalOptions->m_nAudioInputDeviceID, pGlobalOptions->m_nAudioOutputDeviceID);
					if (nReslut < 0)
					{
						std::string strError = CSystemEx::GetInstance()->GetSipErrorInfo(nReslut);
						LOG4_INFO(strError.c_str());
					}
				}
				
			}
		}
	}
	
	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CSpeakerDlg::OnBtnClose(RoutedEventArgs *pe)
{
	std::string strErrorInfo;
	CSystemEx::GetInstance()->SetLocalVideoHwnd(NULL,true, strErrorInfo);
	PortSIP_displayLocalVideo(CSystemEx::GetInstance()->GetSipLib(), false,true);
	if (m_AudioLoopbackTest)
	{
		PortSIP_audioPlayLoopbackTest(CSystemEx::GetInstance()->GetSipLib(), false);
	}

	saveOptions();
	EndDialog(IDCANCEL);
	pe->bHandled = TRUE;
	return 0;
}

LRESULT CSpeakerDlg::OnChkCameraOn(RoutedEventArgs *pe)
{
	UpdateVideoData();
	bool bEnableAudioInputDev = m_pChkMicrophone->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableAudioOutputDev = m_pChkSpeaker->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableVideoDev = m_pChkCameraOn->GetCheck() == BST_CHECKED ? true : false;
	CSystemEx::GetInstance()->ChangeDeviceSwtich(bEnableAudioOutputDev, bEnableAudioInputDev, bEnableVideoDev);
	saveOptions();

	CSystemEx::GetInstance()->InitCameraSwtich(bEnableVideoDev);
	return 0;
}

LRESULT CSpeakerDlg::OnChkSpeaker(RoutedEventArgs *pe)
{
	bool bEnableAudioInputDev = m_pChkMicrophone->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableAudioOutputDev = m_pChkSpeaker->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableVideoDev = m_pChkCameraOn->GetCheck() == BST_CHECKED ? true : false;
	CSystemEx::GetInstance()->ChangeDeviceSwtich(bEnableAudioOutputDev, bEnableAudioInputDev, bEnableVideoDev);

	CSystemEx::GetInstance()->InitAudioOutSwtich(bEnableAudioOutputDev);
	saveOptions();
	return 0;
}

LRESULT CSpeakerDlg::OnBtnTestSound(RoutedEventArgs *pe)
{
	if (!CSystemEx::GetInstance()->GetSipLib())
	{
		return 0;
	}
	if (!m_AudioLoopbackTest)
	{
		PortSIP_audioPlayLoopbackTest(CSystemEx::GetInstance()->GetSipLib(), true);

		m_AudioLoopbackTest = TRUE;

		m_pBtnTestSound->SetWindowText(L"Stop Test");
	}
	else
	{
		PortSIP_audioPlayLoopbackTest(CSystemEx::GetInstance()->GetSipLib(), false);

		m_AudioLoopbackTest = FALSE;

		m_pBtnTestSound->SetWindowText(L"Test Sound");
	}
	return 0;
}

LRESULT CSpeakerDlg::OnChkMicrophone(RoutedEventArgs *pe)
{
	bool bEnableAudioInputDev = m_pChkMicrophone->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableAudioOutputDev = m_pChkSpeaker->GetCheck() == BST_CHECKED ? true : false;
	bool bEnableVideoDev = m_pChkCameraOn->GetCheck() == BST_CHECKED ? true : false;
	CSystemEx::GetInstance()->ChangeDeviceSwtich(bEnableAudioOutputDev, bEnableAudioInputDev, bEnableVideoDev);


	CSystemEx::GetInstance()->InitAudioInSwtich(bEnableAudioInputDev);
	saveOptions();
	return 0;
}

LRESULT CSpeakerDlg::OnBtnOk(RoutedEventArgs *pe)
{
	EndDialog(IDOK);
	pe->bHandled = TRUE;


	std::string strErrorInfo;

	CSystemEx::GetInstance()->StopLocalVideoHwnd(NULL,true, strErrorInfo);

	if (m_AudioLoopbackTest)
	{
		PortSIP_audioPlayLoopbackTest(CSystemEx::GetInstance()->GetSipLib(), false);
	}

	saveOptions();

	PortSIP_audioPlayLoopbackTest(CSystemEx::GetInstance()->GetSipLib(), false);
	m_AudioLoopbackTest = FALSE;
	return 0;
}

LRESULT CSpeakerDlg::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}
	else if (m_pChkCameraOn->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkCameraOn(pe);
	}
	else if (m_pChkSpeaker->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkSpeaker(pe);
	}
	else if (m_pBtnTestSound->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnTestSound(pe);
	}
	else if (m_pChkMicrophone->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMicrophone(pe);
	}
	else if (m_pBtnOk->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnOk(pe);
	}

	return 0;
}
LRESULT CSpeakerDlg::SelectCamera(RoutedEventArgs *pe)
{
	/*
	if ()
	{
	}
	*/
	return 0;
}
void CSpeakerDlg::LoadMicphoneVolume(int deviceId)
{
	int volume = PortSIP_getMicVolume(CSystemEx::GetInstance()->GetSipLib());
	m_pSldMicrophone->SetRange(0, 255);
	m_pSldMicrophone->SetPos(volume);
}
void CSpeakerDlg::loadSpeakerVolume(int deviceId)
{
	int volume = PortSIP_getSpeakerVolume(CSystemEx::GetInstance()->GetSipLib());

	m_pSldSpeaker->SetRange(0, 255);
	m_pSldSpeaker->SetPos(volume);
}

void CSpeakerDlg::loadOptions()
{
	//Speaker
	int nNumSpeakerDevice = PortSIP_getNumOfPlayoutDevices(CSystemEx::GetInstance()->GetSipLib());
	for (int i = 0; i < nNumSpeakerDevice; ++i)
	{
		char name[512] = { 0 };
		PortSIP_getPlayoutDeviceName(CSystemEx::GetInstance()->GetSipLib(), i, name, 512);

		std::stringstream ssDeviceName;
		ssDeviceName << name;
		std::wstring strName = PortUtility::Utf82Unicode_ND(ssDeviceName.str());;
		m_pCmbSpeaker->AddString(strName.c_str());
	}
	

	COptions *pGlobalOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pGlobalOptions->m_bEnableAudioOutputDev)
	{
		m_pChkSpeaker->SetCheck(pGlobalOptions->m_bEnableAudioOutputDev);
	}

	if ((pGlobalOptions->m_nAudioInputDeviceID+ 1) >nNumSpeakerDevice&&nNumSpeakerDevice>0)
	{
		m_pCmbSpeaker->SetCurSel(0);
	}
	else if ( pGlobalOptions->m_nAudioInputDeviceID + 1 <= nNumSpeakerDevice&&nNumSpeakerDevice > 0)
	{
		m_pCmbSpeaker->SetCurSel(pGlobalOptions->m_nAudioOutputDeviceID);
	}
	else
	{
		m_pChkSpeaker->SetCheck(false);
	}
	
	//MIC
	nNumSpeakerDevice = PortSIP_getNumOfRecordingDevices(CSystemEx::GetInstance()->GetSipLib());
	for (int i = 0; i < nNumSpeakerDevice; ++i)
	{
		char name[512] = { 0 };
		PortSIP_getRecordingDeviceName(CSystemEx::GetInstance()->GetSipLib(), i, name, 512);

		std::stringstream ssDeviceName;
		ssDeviceName << name;
		std::wstring strName = PortUtility::Utf82Unicode_ND(ssDeviceName.str());;
		m_pCmbMicrophone->AddString(strName.c_str());
	}
	if (pGlobalOptions->m_bEnableAudioInputDev)
	{
		m_pChkMicrophone->SetCheck(pGlobalOptions->m_bEnableAudioInputDev);
	}
	if (pGlobalOptions->m_nAudioInputDeviceID + 1>nNumSpeakerDevice&&nNumSpeakerDevice>0)
	{
		m_pCmbMicrophone->SetCurSel(0);
	}
	else if((pGlobalOptions->m_nAudioInputDeviceID + 1) <= nNumSpeakerDevice&&nNumSpeakerDevice > 0)
	{
		m_pCmbMicrophone->SetCurSel(pGlobalOptions->m_nAudioInputDeviceID);
	}
	else
	{
		m_pChkMicrophone->SetCheck(false);
	}
	//carema
	nNumSpeakerDevice = PortSIP_getNumOfVideoCaptureDevices(CSystemEx::GetInstance()->GetSipLib());
	for (int i = 0; i < nNumSpeakerDevice; ++i)
	{
		char name[512] = { 0 };
		char uniqueId[512] = { 0 };
		PortSIP_getVideoCaptureDeviceName(CSystemEx::GetInstance()->GetSipLib(), i, uniqueId, 512, name, 512);
		
		std::stringstream ssDeviceName;
		ssDeviceName << name;
	
		std::wstring strDevice = PortUtility::Utf82Unicode_ND(ssDeviceName.str());
		m_pCmbSelectDevice->AddString(strDevice.c_str());
	}
	if (pGlobalOptions->m_bEnableVideoDev)
	{
		m_pChkCameraOn->SetCheck(pGlobalOptions->m_bEnableVideoDev);
	}
	if (pGlobalOptions->m_nVideoDeviceID + 1 > nNumSpeakerDevice&&nNumSpeakerDevice > 0)
	{
		m_pCmbSelectDevice->SetCurSel(pGlobalOptions->m_nVideoDeviceID);	
	}
	else if (pGlobalOptions->m_nVideoDeviceID + 1 <= nNumSpeakerDevice && nNumSpeakerDevice > 0)
	{
		m_pCmbSelectDevice->SetCurSel(pGlobalOptions->m_nVideoDeviceID);
	}
	else
	{
		m_pChkCameraOn->SetCheck(false);
	}

	int volume = PortSIP_getSpeakerVolume(CSystemEx::GetInstance()->GetSipLib());
	m_pSldSpeaker->SetRange(0, 255);
	m_pSldSpeaker->SetPos(volume);
	volume = PortSIP_getMicVolume(CSystemEx::GetInstance()->GetSipLib());
	m_pSldMicrophone->SetRange(0, 255);
	m_pSldMicrophone->SetPos(volume);
	
}
void CSpeakerDlg::UpdateVideoData()
{
	COptions *pGlobalOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	int nVideoDeviceID = pGlobalOptions->m_nVideoDeviceID;
	int nCheckStatus = m_pChkCameraOn->GetCheck();
	int nErrorCode = PortSIP_setVideoDeviceId(CSystemEx::GetInstance()->GetSipLib(), nVideoDeviceID);
	if (nErrorCode < 0)
	{
		std::string strError = CSystemEx::GetInstance()->GetSipErrorInfo(nErrorCode);
		LOG4_INFO(strError.c_str());
	}
	std::string strErrorInfo;

	if (nCheckStatus == BST_CHECKED)
	{
		CSystemEx::GetInstance()->SetLocalVideoHwnd(m_hLocalVideoDlg,true, strErrorInfo);
	}
	else
	{
		CSystemEx::GetInstance()->StopLocalVideoHwnd(NULL,true, strErrorInfo);
	}
 	
	CIUIRect rcVideoCanvas;
	m_pStaPic->GetWindowRectToHWND(rcVideoCanvas);
	::InvalidateRect(m_hLocalVideoDlg, NULL, TRUE);
	::UpdateWindow(m_hLocalVideoDlg);

}
LRESULT CSpeakerDlg::OnHScroll(RoutedEventArgs *pe)
{
	if (NULL == pe)
	{
		return -1;
	}
	if (GetClassName(pe->hOriginalSender) == IUI_SLIDER
		&& LOWORD(pe->wParam) == SB_ENDSCROLL)
	{
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(pe->hOriginalSender);
		int nPos = pSlider->GetPos();
		if (pSlider->GetName() == _T("IDC_SLD_SPEAKER"))
		{
			int nPos = pSlider->GetPos();
			PortSIP_setSpeakerVolume(CSystemEx::GetInstance()->GetSipLib(), nPos);
		}
		else if (pSlider->GetName() == _T("IDC_SLD_MICROPHONE"))
		{
			int nPos = pSlider->GetPos();
			PortSIP_setMicVolume(CSystemEx::GetInstance()->GetSipLib(), nPos);
		}
	}

	return 0;
}
void CSpeakerDlg::saveOptions()
{
	COptions* pTheLocalOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	pTheLocalOptions->m_nAudioOutputDeviceID = m_pCmbSpeaker->GetCurSel();
	pTheLocalOptions->m_nAudioInputDeviceID= m_pCmbMicrophone->GetCurSel();
	pTheLocalOptions->m_nVideoDeviceID = m_pCmbSelectDevice->GetCurSel();
	pTheLocalOptions->m_bEnableAudioInputDev = m_pChkMicrophone->GetCheck()== BST_CHECKED?true:false;
	pTheLocalOptions->m_bEnableAudioOutputDev = m_pChkSpeaker->GetCheck() == BST_CHECKED ? true : false;
	pTheLocalOptions->m_bEnableVideoDev = m_pChkCameraOn->GetCheck() == BST_CHECKED ? true : false;
	stringstream sql;
	sql << "update ";
	sql << T_OPTIONS;

	sql << " set AudioInputDeviceId = ";
	sql << pTheLocalOptions->m_nAudioInputDeviceID;
	sql << ",  AudioOutputDeviceId = ";
	sql << pTheLocalOptions->m_nAudioOutputDeviceID;
	sql << ",  VideoDeviceId = ";
	sql << pTheLocalOptions->m_nVideoDeviceID;
	sql << ",  EnableAudioInputDev = ";
	int nTempValue = pTheLocalOptions->m_bEnableAudioInputDev;
	sql << nTempValue;
	sql << ",  EnableAudioOutputDev = ";
	nTempValue = pTheLocalOptions->m_bEnableAudioOutputDev;
	sql << nTempValue;
	sql << ",  EnableVideoDev = ";
	nTempValue = pTheLocalOptions->m_bEnableVideoDev ;
	sql << pTheLocalOptions->m_bEnableVideoDev;
	string errnoMessage;
	CSystemEx::GetInstance()->GetDBOprate()->exec(sql.str(), NULL, NULL, errnoMessage);
}
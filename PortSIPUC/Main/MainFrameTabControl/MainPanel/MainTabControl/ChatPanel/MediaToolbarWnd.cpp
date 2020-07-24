#include "stdafx.h"
#include "MediaToolbarWnd.h"


CMediaToolbarWnd::CMediaToolbarWnd(CHAT_TAB eChatTab, ENUM_RECENT_CONTACT_TYPE enumContactType)
	: m_eChatTab(eChatTab)
	, m_pCanvasMediaToolbar(NULL)
	, m_pChkRec(NULL)
	, m_pChkMute(NULL)
	, m_pChkPause(NULL)
	, m_pBtnTransfer(NULL)
	, m_pBtnAdd(NULL)
	, m_pBtnKeyboard(NULL)
	, m_pBtnShare(NULL)
	, m_pBtnMsg(NULL)
	, m_pChkNoVideo(NULL)
	, m_pBtnHangup(NULL)
{
	m_enumContactTpye = enumContactType;
	m_nActiveLine = -1;
	m_bHoldBySelf = false;
}

CMediaToolbarWnd::~CMediaToolbarWnd()
{
}

int CMediaToolbarWnd::SetBindVideoPanel(HWLWND hBindVideo)
{
	m_hBindVideo = hBindVideo;

	return 0;
}

HWLWND CMediaToolbarWnd::GetBindVideoPanel() const
{
	return m_hBindVideo;
}

int CMediaToolbarWnd::InitControls()
{
	m_pCanvasMediaToolbar = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_MEDIA_TOOLBAR")));
	_ASSERT(m_pCanvasMediaToolbar != NULL);
	m_pChkRec = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_REC")));
	_ASSERT(m_pChkRec != NULL);
	m_pChkMute = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MUTE")));
	_ASSERT(m_pChkMute != NULL);
	m_pChkPause = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_PAUSE")));
	_ASSERT(m_pChkPause != NULL);
	m_pBtnTransfer = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_TRANSFER")));
	_ASSERT(m_pBtnTransfer != NULL);
	m_pBtnAdd = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_ADD")));
	_ASSERT(m_pBtnAdd != NULL);
	m_pBtnKeyboard = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_KEYBOARD")));
	_ASSERT(m_pBtnKeyboard != NULL);
	m_pBtnShare = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_SHARE")));
	_ASSERT(m_pBtnShare != NULL);
	m_pBtnMsg = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MSG")));
	_ASSERT(m_pBtnMsg != NULL);
	m_pChkNoVideo = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_NO_VIDEO")));
	_ASSERT(m_pChkNoVideo != NULL);
	m_pBtnHangup = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_HANGUP")));
	_ASSERT(m_pBtnHangup != NULL);
	EnableWindow(m_pBtnShare->GetSafeHwnd(), FALSE);
	EnableWindow(m_pBtnMsg->GetSafeHwnd(), FALSE);
	EnableWindow(m_pBtnAdd->GetSafeHwnd(), FALSE);
	EnableWindow(m_pBtnTransfer->GetSafeHwnd(), FALSE);
	EnableWindow(m_pBtnKeyboard->GetSafeHwnd(), FALSE);
	EnableWindow(m_pChkNoVideo->GetSafeHwnd(), FALSE);
	SetFunBtnStat(FALSE);
	return 0;
}
void CMediaToolbarWnd::SetFunBtnStat(BOOL bEnable, bool isTalkGroup)
{
	if (isTalkGroup&&m_enumContactTpye == ENUM_MULTI_CONTACT)
	{
		EnableWindow(m_pBtnTransfer->GetSafeHwnd(), FALSE);
		EnableWindow(m_pBtnKeyboard->GetSafeHwnd(), FALSE);
		EnableWindow(m_pChkRec->GetSafeHwnd(), FALSE);
	}
	else
	{
		EnableWindow(m_pChkRec->GetSafeHwnd(), bEnable);
		EnableWindow(m_pBtnTransfer->GetSafeHwnd(), bEnable);
		EnableWindow(m_pBtnKeyboard->GetSafeHwnd(), bEnable);
		EnableWindow(m_pBtnMsg->GetSafeHwnd(), bEnable);
	}
	
	EnableWindow(m_pChkMute->GetSafeHwnd(), bEnable);
	EnableWindow(m_pChkPause->GetSafeHwnd(), bEnable);
	EnableWindow(m_pChkNoVideo->GetSafeHwnd(), bEnable);
	EnableWindow(m_pBtnAdd->GetSafeHwnd(), bEnable);
}
void CMediaToolbarWnd::ResetBtnStatus(bool isTalkGroup)
{
	SetFunBtnStat(FALSE);
}
LRESULT CMediaToolbarWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	return 0;
}

LRESULT CMediaToolbarWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CMediaToolbarWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CMediaToolbarWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	RoutedEventArgs arg;
	arg.hSender = pe->hOriginalSender;
	arg.hOriginalSender = pe->hOriginalSender;
	arg.RoutedEvent = UE_BUTTON_CLICK;
	arg.eRoutingStrategy = RS_BUBBLE;
	RaiseEvent(m_hBindVideo, &arg);

	return 0;
}
int CMediaToolbarWnd::GetRecCheck() 
{ 
	return m_pChkRec->GetCheck(); 
}
void CMediaToolbarWnd::SetRecCheck(int nMode) 
{ 
	if (m_pChkRec!=NULL)
	{
		m_pChkRec->SetCheck(nMode);
	}
	return ;
}

int CMediaToolbarWnd::GetMuteCheck()
{
	if (m_pChkMute!=NULL)
	{
		return m_pChkMute->GetCheck();
	}
	return -1;
}
void CMediaToolbarWnd::SetMuteCheck(int nMode)
{
	m_pChkMute->SetCheck(nMode);
}


int CMediaToolbarWnd::GetHoldCheck()
{	
	if (m_pChkMute!=NULL)
	{
		return m_pChkPause->GetCheck();
	}
	return -1;
}
void CMediaToolbarWnd::SetHoldCheck(int nMode)
{
	m_pChkPause->SetCheck(nMode);
}
BOOL CMediaToolbarWnd::GetRecEnable() 
{ 
	return m_pChkRec->IsWindowEnabled();
}


int CMediaToolbarWnd::GetVideoCheck()
{
	if (m_pChkNoVideo!=NULL)
	{
		return m_pChkNoVideo->GetCheck();
	}
	return -1;
}
void CMediaToolbarWnd::SetVideoCheck(int nMode)
{
	m_pChkNoVideo->SetCheck(nMode);
}
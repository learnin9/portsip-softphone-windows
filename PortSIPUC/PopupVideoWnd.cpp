#include "stdafx.h"
#include "PopupVideoWnd.h"
#include "System/SystemEx.h"

#define VIDEO_FILE_WND_CLASS _T("PortGoVideoFileWndClass")

BOOL RegisterVideoFileWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = VIDEO_FILE_WND_CLASS;
	ATOM ret = ::RegisterClass(&wc);
	_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND CreateVideoFileWnd(HWND hParent)
{
	BOOL bRet = RegisterVideoFileWindowClass();
	if (!bRet)
	{
		return NULL;
	}

	HWND hWnd = ::CreateWindowEx(0, VIDEO_FILE_WND_CLASS,
		NULL, WS_CHILD | WS_VISIBLE, 0, 0, 200, 200,
		hParent, 0, IUIGetInstanceHandle(), 0);

	return hWnd;
}

CPopupVideoWnd::CPopupVideoWnd()
	: m_pBtnMin(NULL)
	, m_pChkMax(NULL)
	, m_pBtnClose(NULL)
	, m_pCanvasVideo(NULL)
	, m_pChkPlayPause(NULL)
	, m_pStaDuration(NULL)
	, m_pSldDuration(NULL)
	, m_pChkMute(NULL)
	, m_pSldVolume(NULL)
	, m_pBtnFullScreen(NULL)
{
	m_pBitMap = NULL;
	m_pBitmapHelper = NULL;
}

CPopupVideoWnd::~CPopupVideoWnd()
{
	if (m_pBitmapHelper)
	{
		delete m_pBitmapHelper;
		m_pBitmapHelper = NULL;
	}
}

int CPopupVideoWnd::InitControls()
{
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pCanvasVideo = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_VIDEO")));
	_ASSERT(m_pCanvasVideo != NULL);
	m_pChkPlayPause = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_PLAY_PAUSE")));
	_ASSERT(m_pChkPlayPause != NULL);
	m_pStaDuration = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_DURATION")));
	_ASSERT(m_pStaDuration != NULL);
	m_pSldDuration = (Slider *)CWLWnd::FromHandle(FindControl(_T("IDC_SLD_DURATION")));
	_ASSERT(m_pSldDuration != NULL);
	m_pChkMute = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MUTE")));
	_ASSERT(m_pChkMute != NULL);
	m_pSldVolume = (Slider *)CWLWnd::FromHandle(FindControl(_T("IDC_SLD_VOLUME")));
	_ASSERT(m_pSldVolume != NULL);
	m_pBtnFullScreen = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_FULL_SCREEN")));
	_ASSERT(m_pBtnFullScreen != NULL);

	m_pSldVolume->SetRange(0, 100);
	m_pSldVolume->SetRange(0,100);

	return 0;
}
void CPopupVideoWnd::AutoPlay()
{
	return;
	CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init(ENUM_VIDEO);
	if (m_hVideoWnd != NULL)
	{
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->SetHwnd(m_hVideoWnd, GetHWND());
	}

	if (!m_strFilePath.IsEmpty() && !m_strFileName.IsEmpty())
	{
		CIUIString strTemplateFileName = m_strFilePath + m_strFileName;
		bool bFalse = CSystemEx::GetInstance()->GetGlobalAVPlayer()->PlayByHwnd(PortUtility::Unicode2Utf8_ND(strTemplateFileName.GetBuffer(strTemplateFileName.GetLength())), \
			GetHWND());

		if (bFalse==false)
		{
			m_pChkPlayPause->SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_pChkPlayPause->SetCheck(BST_CHECKED);
		}
	}
	
}
LRESULT CPopupVideoWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	m_hVideoWnd = CreateVideoFileWnd(m_hWnd);
	m_pSldVolume->SetRange(0,100);
	m_pSldVolume->SetPos(100);
	AutoPlay();
	if (m_pBitmapHelper != NULL)
	{
		m_pBitmapHelper->ShowOnWindow(this->m_hVideoWnd);
	}

	if (!m_strTotalPlayTime.IsEmpty())
	{
		IUI::SetWindowText(m_pStaDuration->GetSafeHwnd(),m_strTotalPlayTime);
	}
	return 0;
}

LRESULT CPopupVideoWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_PLAY_BEGIN == uMsg)
	{
		OnPlay(wParam, lParam);
	}
	else  if (WM_PLAY_END == uMsg)
	{
		OnPlayEnd(wParam, lParam);
	}
	else if (WM_PLAY_POS_CHANGED == uMsg)
	{
		OnPlayPosChanged(wParam, lParam);
	}
	else if (WM_PLAY_DRAG_POS ==uMsg)
	{
		OnDragPos(wParam, lParam);
	}
	else if (WM_PLAY_TIME_CHANGED==uMsg)
	{
		OnPlayTimeChanged(wParam, lParam);
	}
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CPopupVideoWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (WM_MOVE == pe->RoutedEvent)
		{
			if (m_pCanvasVideo->GetSafeHwnd() == pe->hOriginalSender)
			{
				CIUIRect rcVideo;
				m_pCanvasVideo->GetWindowRectToHWND(rcVideo);
				::MoveWindow(m_hVideoWnd, rcVideo.left, rcVideo.top,
					rcVideo.Width(), rcVideo.Height(), FALSE);
			}
		}
		else if (WM_HSCROLL == pe->RoutedEvent)
		{
			return OnHScroll(pe);
		}
		else if (WM_LBUTTONDOWN==pe->RoutedEvent)
		{
			return OnMouseLBDown(pe);
		}
	}
	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (UE_DRAW_BK_IMAGE == pe->RoutedEvent)
		{
			OnAfterPaint(pe);
		}
	}
	
	return Window::OnControlEvent(wParam, lParam);
}
LRESULT CPopupVideoWnd::OnAfterPaint(RoutedEventArgs *pe)
{
	if (m_pBitmapHelper !=NULL)
	{
		m_pBitmapHelper ->ShowOnWindow(m_hVideoWnd);
	}
	return 0;
}
void CPopupVideoWnd::SetPlayParam(HBITMAP pHBitmap,/*Button *pSrcChkPlayPause, */CIUIString strFilePath, CIUIString strTotalPlayTime,  CIUIString strFileName, int nCrruentVolumn)
{
	//m_pSrcChkPlayPause = pSrcChkPlayPause;	
	m_strFilePath = strFilePath;	
	m_strTotalPlayTime = strTotalPlayTime;
	m_strFileName = strFileName;
	m_nCrruentVolumn = nCrruentVolumn;	
	m_pBitMap = pHBitmap;
	if (m_pBitmapHelper==NULL)
	{
		m_pBitmapHelper = new BitmapHelper(m_pBitMap);
	}
}
LRESULT CPopupVideoWnd::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CPopupVideoWnd::OnChkMax(RoutedEventArgs *pe)
{
	
	return 0;
}

LRESULT CPopupVideoWnd::OnBtnClose(RoutedEventArgs *pe)
{
	/*if (m_pSrcChkPlayPause->GetCheck()==TRUE)
	{
		m_pSrcChkPlayPause->SetCheck(TRUE);
	}*/
	if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
	{
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->Stop();
	}
	if (m_pBitmapHelper)
	{
		delete m_pBitmapHelper;
		m_pBitmapHelper = NULL;
	}

	return 0;
}
void CPopupVideoWnd::SetVol()
{
	bool bMuteVol = m_pChkMute->GetCheck() == BST_CHECKED ? true : false;
	if (bMuteVol)
	{
		//CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(0);
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->MuteVol(bMuteVol);
	}
	else
	{
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->MuteVol(bMuteVol);
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(m_nCrruentVolumn);
	}
}
LRESULT CPopupVideoWnd::OnChkPlayPause(RoutedEventArgs *pe)
{
	if (m_pChkPlayPause->GetCheck() == BST_CHECKED)
	{
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(FindControl(_T("IDC_SLD_DURATION")));

		if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay() == true)
		{
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->Stop();
		}
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init(ENUM_VIDEO);
		if (m_hVideoWnd != NULL)
		{
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->SetHwnd(m_hVideoWnd, GetHWND());
		}

		if (!m_strFilePath.IsEmpty() && !m_strFileName.IsEmpty())
		{
			CIUIString strTemplateFileName = m_strFilePath + m_strFileName;
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->PlayByHwnd(PortUtility::Unicode2Utf8_ND(strTemplateFileName.GetBuffer(strTemplateFileName.GetLength())), \
				GetHWND());
			SetVol();
			int nPos = 0;
			if (pSlider != NULL)
			{
				nPos = pSlider->GetPos();
				if (nPos >= 100)
				{
					nPos = 0;
				}
			}
			if (nPos != 0)
			{
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
			}
		}
	}
	else
	{
		if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
		{
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->Pause();
		}
	}

	

	return 0;
}

LRESULT CPopupVideoWnd::OnChkMute(RoutedEventArgs *pe)
{
	if (m_pChkMute->GetCheck() == BST_UNCHECKED)
	{
		if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
		{
			SetVol();
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(m_nCrruentVolumn);
		}
	}
	else
	{
		if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
		{
			SetVol();
			CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(0);
		}
	}
	
	return 0;
}

LRESULT CPopupVideoWnd::OnBtnFullScreen(RoutedEventArgs *pe)
{
	::ShowWindow(m_hWnd, SW_MAXIMIZE);
	if (m_pBitmapHelper != NULL)
	{
		m_pBitmapHelper->ShowOnWindow(this->m_hVideoWnd);
	}
	Invalidate();
	return 0;
}

LRESULT CPopupVideoWnd::OnButtonClicked(RoutedEventArgs *pe)
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
	else if (m_pChkPlayPause->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkPlayPause(pe);
	}
	else if (m_pChkMute->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMute(pe);
	}
	else if (m_pBtnFullScreen->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnFullScreen(pe);
	}

	return 0;
}
LRESULT CPopupVideoWnd::OnMouseLBDown(RoutedEventArgs *pe)
{
	if (NULL == pe)
	{
		return -1;
	}

	return 0;
}
LRESULT CPopupVideoWnd::OnMouseLBUp(RoutedEventArgs *pe)
{
	if (NULL == pe)
	{
		return -1;
	}
	return 0;
}
LRESULT CPopupVideoWnd::OnHScroll(RoutedEventArgs *pe)
{
	if (NULL == pe)
	{
		return -1;
	}
	
	if (GetClassName(pe->hOriginalSender) == IUI_SLIDER)
	{
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(pe->hOriginalSender);
		if (LOWORD(pe->wParam) == SB_ENDSCROLL)
		{
			if (m_pSldDuration->GetSafeHwnd() == pe->hOriginalSender)
			{
				int nPos = m_pSldDuration->GetPos();
				if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
				{
					CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
					if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PAUSE)
					{
						CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnContinue();
					}
				}
			}
			else if (m_pSldVolume->GetSafeHwnd() == pe->hOriginalSender)
			{
				m_pChkMute->SetCheck(BST_UNCHECKED);
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->MuteVol(false);
				int nPos = m_pSldVolume->GetPos();
				m_nCrruentVolumn = nPos;
				CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(nPos);
			}
		}
		else if (LOWORD(pe->wParam) == SB_THUMBTRACK)
		{
			if (m_pSldDuration->GetSafeHwnd() == pe->hOriginalSender)
			{
				if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PLAY)
				{
					CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnlyPause();
				}
			}
			
		}
	}

	return 0;
}

LRESULT CPopupVideoWnd::OnPlay(WPARAM wParam, LPARAM lParam)
{
	if (m_pChkPlayPause)
	{
		m_pChkPlayPause->SetCheck(true);
	}
	
	return 0;
}
LRESULT CPopupVideoWnd::OnPlayEnd(WPARAM wParam, LPARAM lParam)
{
	int nPos = (int)wParam;
	CIUIString strText = (LPWSTR)lParam;
	if (nPos < 100)
	{
		return 0;
	}
	if (strText.IsEmpty() == true)
	{
		return 0;
	}
	if (m_pChkPlayPause)
	{
		m_pChkPlayPause->SetCheck(false);
	}
	if (m_pBitmapHelper != NULL)
	{
		m_pBitmapHelper->ShowOnWindow(this->m_hVideoWnd);
	}
	if (m_pSldDuration)
	{
		m_pSldDuration->SetPos(nPos);
		m_pSldDuration->Invalidate();
	}
	if (m_pStaDuration)
	{
		m_pStaDuration->SetWindowText(strText);
	}
	Invalidate();
	return 0;
}
LRESULT CPopupVideoWnd::OnPlayPosChanged(WPARAM wParam, LPARAM lParam)
{
	int nPos = (int)wParam;
	if (nPos<0)
	{
		return 0;
	}
	if (nPos>100)
	{
		nPos = 100;
	}
	if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PAUSE)
	{
		return 0;
	}
	if (m_pSldDuration)
	{
		m_pSldDuration->SetPos(nPos);
	}
	
	return 0;
}
LRESULT CPopupVideoWnd::OnDragPos(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}
LRESULT CPopupVideoWnd::OnPlayTimeChanged(WPARAM wParam, LPARAM lParam)
{
	CIUIString strText = (LPWSTR)wParam;
	if (m_pStaDuration)
	{
		m_pStaDuration->SetWindowText(strText);
	}
	return 0;
}
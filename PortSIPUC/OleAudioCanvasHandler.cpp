#include "stdafx.h"
#include "OleAudioCanvasHandler.h"
#include "System/SystemEx.h"

COleAudioCanvasHandler::COleAudioCanvasHandler()
	: m_pCanvasToolbar(NULL)
	, m_pChkPlayPause(NULL)
	, m_pStaDuration(NULL)
	, m_pSldProgress(NULL)
	, m_pChkMute(NULL)
	, m_pSldVolumn(NULL)
	, m_pStaFile(NULL)
	, m_pPrgAudioUpload(NULL)
	, m_pStaInfo(NULL)
{
	m_strFilePath = L"";
	m_strTotalPlayTime = L"";
	m_nCrruentVolumn = 100;
	m_strFileName = L"";
}
void COleAudioCanvasHandler::SetAudioParam(LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime,  LPCTSTR lpszStrFileName)
{
	m_strFilePath = lpszFilePath;
	m_strTotalPlayTime = lpszTotalPlayTime;
	m_strFileName = lpszStrFileName;
}
COleAudioCanvasHandler::~COleAudioCanvasHandler()
{
}

int COleAudioCanvasHandler::InitControls()
{
	m_pCanvasToolbar = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_TOOLBAR")));
	_ASSERT(m_pCanvasToolbar != NULL);
	m_pChkPlayPause = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_PLAY_PAUSE")));
	_ASSERT(m_pChkPlayPause != NULL);
	m_pStaDuration = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_DURATION")));
	_ASSERT(m_pStaDuration != NULL);
	m_pSldProgress = (Slider *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_SLD_PROGRESS")));
	_ASSERT(m_pSldProgress != NULL);
	m_pChkMute = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_MUTE")));
	_ASSERT(m_pChkMute != NULL);
	m_pSldVolumn = (Slider *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_SLD_VOLUMN")));
	_ASSERT(m_pSldVolumn != NULL);
	m_pStaFile = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_FILE")));
	_ASSERT(m_pStaFile != NULL);
	m_pPrgAudioUpload = (ProgressBar *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PRG_AUDIO_UPLOAD")));
	_ASSERT(m_pPrgAudioUpload != NULL);
	m_pStaInfo = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_INFO")));
	_ASSERT(m_pStaInfo != NULL);
	m_pStaDuration = m_pStaDuration;
	m_pSldProgress->SetRange(0, 100);
	m_pSldProgress->ShowWindow(SW_SHOW);
	return 0;
}

int COleAudioCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	m_pSldVolumn->SetRange(0, 100);
	m_pSldVolumn->SetPos(100);
	return 0;
}

LRESULT COleAudioCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
		else if (WM_HSCROLL == pe->RoutedEvent)
		{
			int nCode = LOWORD(pe->wParam);
			if (TB_ENDTRACK == nCode)
			{
				if (m_pSldProgress->GetSafeHwnd() == pe->hOriginalSender)
				{
					int nPos = m_pSldProgress->GetPos();
					if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->IsPlay())
					{
						CSystemEx::GetInstance()->GetGlobalAVPlayer()->StreamSeek(nPos);
						if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PAUSE)
						{
							CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnContinue();
						}
					}
				}
				else if (m_pSldVolumn->GetSafeHwnd() == pe->hOriginalSender)
				{
					CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(m_nCrruentVolumn);
				}
			}
			else if (nCode == SB_THUMBTRACK)
			{
				if (CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetPlayState() == STATE_PLAY)
				{
					CSystemEx::GetInstance()->GetGlobalAVPlayer()->OnlyPause();
				}
			}
		}
	}

	else if (RS_DIRECT == pe->eRoutingStrategy)
	{
		if (WM_PLAY_BEGIN == pe->RoutedEvent)
		{
			OnPlay(pe);
		}
		else  if (WM_PLAY_END == pe->RoutedEvent)
		{
			OnPlayEnd(pe);
		}
		else if (WM_PLAY_POS_CHANGED == pe->RoutedEvent)
		{
			OnPlayPosChanged(pe);
		}
		else if (WM_PLAY_DRAG_POS == pe->RoutedEvent)
		{
			OnDragPos(pe);
		}
		else if (WM_PLAY_TIME_CHANGED == pe->RoutedEvent)
		{
			OnPlayTimeChanged(pe);
		}
	}


	return 0;
}

LRESULT COleAudioCanvasHandler::OnChkPlayPause(RoutedEventArgs *pe)
{
	CSystemEx::GetInstance()->GetGlobalAVPlayer()->Init(ENUM_AUDIO);
	if (!m_strFilePath.IsEmpty()&&!m_strFileName.IsEmpty())
	{
		Slider *pSlider = (Slider *)CWLWnd::FromHandle(FindControl(GetParent(pe->hOriginalSender), _T("IDC_SLD_PROGRESS")));
		
		CIUIString strTemplateFileName = m_strFilePath+ m_strFileName;
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->PlayByWLWnd(PortUtility::Unicode2Utf8_ND(strTemplateFileName.GetBuffer(strTemplateFileName.GetLength())),\
			 (GetBindWLWnd()),NORMAL_VOLUME, 0);

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
	
	return 0;
}

LRESULT COleAudioCanvasHandler::OnChkMute(RoutedEventArgs *pe)
{
	bool bCheck = m_pChkMute->GetCheck();
	if (bCheck==false)
	{
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(0);
	}
	else
	{
		CSystemEx::GetInstance()->GetGlobalAVPlayer()->VolumeSet(m_nCrruentVolumn);
	}
	return 0;
}

LRESULT COleAudioCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkPlayPause->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkPlayPause(pe);
	}
	else if (m_pChkMute->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMute(pe);
	}

	return 0;
}

LRESULT COleAudioCanvasHandler::OnPlay(RoutedEventArgs *pe)
{
	if (m_pChkPlayPause)
	{
		if (IUI::IsWindow(m_pChkPlayPause->GetSafeHwnd()))
		{
			if (m_pChkPlayPause->GetCheck() == BST_UNCHECKED)
			{
				m_pChkPlayPause->SetCheck(BST_CHECKED);
			}
		}
	}

	return 0;
}

LRESULT COleAudioCanvasHandler::OnPlayEnd(RoutedEventArgs *pe)
{
	int nPos = (int)pe->wParam;
	CIUIString strText = (LPWSTR)pe->lParam;
	if (nPos < 100)
	{
		return 0;
	}
	if (strText.IsEmpty() == TRUE)
	{
		return 0;
	}
	if (m_pChkPlayPause)
	{
		if (IUI::IsWindow(m_pChkPlayPause->GetSafeHwnd()))
		{
			if (m_pChkPlayPause->GetCheck() == BST_CHECKED)
			{
				m_pChkPlayPause->SetCheck(BST_UNCHECKED);
			}
		}
	}
	if (m_pSldProgress)
	{
		m_pSldProgress->SetPos(nPos);
	}

	if (m_pStaDuration)
	{
		m_pStaDuration->SetWindowText(strText);
	}
	Invalidate(GetBindWLWnd());
	return 0;
}

LRESULT COleAudioCanvasHandler::OnPlayPosChanged(RoutedEventArgs *pe)
{
	int nPos = (int)pe->wParam;
	if (nPos < 0)
	{
		nPos= 0;
	}
	if (nPos > 100)
	{
		nPos = 100;
	}

	if (m_pSldProgress)
	{
		m_pSldProgress->SetPos(nPos);
	}

	return 0;
}
LRESULT COleAudioCanvasHandler::OnDragPos(RoutedEventArgs *pe)
{

	return 0;
}
LRESULT COleAudioCanvasHandler::OnPlayTimeChanged(RoutedEventArgs *pe)
{
	CIUIString strText = (LPWSTR)pe->wParam;
	if (m_pStaDuration)
	{
		m_pStaDuration->SetWindowText( strText);
	}
	return 0;
}

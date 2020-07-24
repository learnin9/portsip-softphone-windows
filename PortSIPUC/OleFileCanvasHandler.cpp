#include "stdafx.h"
#include "OleFileCanvasHandler.h"
#include "System/SystemEx.h"

COleFileCanvasHandler::COleFileCanvasHandler()
	: m_pStaIcon(NULL)
	, m_pStaFileName(NULL)
	, m_pBtnMore(NULL)
	, m_pPrgUpload(NULL)
{
	m_strFilePath = L"";
}

COleFileCanvasHandler::~COleFileCanvasHandler()
{
}
void COleFileCanvasHandler::SetFilePath(LPCTSTR lpszFilePath)
{
	m_strFilePath = lpszFilePath;
}
int COleFileCanvasHandler::InitControls()
{
	m_pStaIcon = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ICON")));
	_ASSERT(m_pStaIcon != NULL);
	m_pStaFileName = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_FILE_NAME")));
	_ASSERT(m_pStaFileName != NULL);
	m_pBtnMore = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MORE")));
	_ASSERT(m_pBtnMore != NULL);
	m_pPrgUpload = (ProgressBar *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PRG_UPLOAD")));
	_ASSERT(m_pPrgUpload != NULL);
	m_pPrgUpload->ShowWindow(SW_HIDE);
	return 0;
}

int COleFileCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	
	return 0;
}

LRESULT COleFileCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

LRESULT COleFileCanvasHandler::OnBtnMore(RoutedEventArgs *pe)
{
	CIUIString strFileName = m_pStaFileName->GetWindowText();
	CIUIString strFilePath;
	if (m_strFilePath.IsEmpty())
	{
		strFilePath = CSystemEx::GetInstance()->GetFileSavePathByMsgType(ENUM_MSG_FILE).c_str() + strFileName;
	}
	else
	{
		strFilePath = m_strFilePath;
	} 
	ShellExecute(NULL, _T("open"), strFilePath, NULL, NULL, SW_SHOWNORMAL);
	return 0;
}

LRESULT COleFileCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnMore->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMore(pe);
	}
	return 0;
}

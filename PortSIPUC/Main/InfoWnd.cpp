#include "stdafx.h"
#include "InfoWnd.h"
#include "System/SystemEx.h"
#define IDT_AUTO_CLOSE		1

CInfoWnd::CInfoWnd(WND_INFO_LEVEL enumInfoWndLevel, bool bInMain)
	: m_pStaTitle(NULL)
	, m_pStaInfo(NULL)
	, m_pBtnClose(NULL)
	, m_nAutoCloseTime(10000)
	, m_enumWndInfoLevel(INFO_WND)
	, m_bInMainDlg (bInMain)
{

}

CInfoWnd::~CInfoWnd()
{
}

int CInfoWnd::SetAutoCloseTime(int nSecond)
{
	m_nAutoCloseTime = nSecond;
	return 0;
}

int CInfoWnd::SetTitle(LPCTSTR lpszTitle)
{
	if (NULL != m_pStaTitle)
	{
		m_pStaTitle->SetWindowText(lpszTitle);
	}
	return 0;
}

int CInfoWnd::SetInfo(LPCTSTR lpszInfo)
{
	if (NULL != m_pStaInfo)
	{
		m_pStaInfo->SetWindowText(lpszInfo);
	}
	return 0;
}

void CInfoWnd::CloseWndByOutSide()
{
	if (m_hWnd==NULL)
	{
		return;
	}
	if (IsWindow(m_hWnd))
	{
		DestroyWindow(m_hWnd);
		delete this;
	}
}

int CInfoWnd::InitControls()
{
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pStaInfo = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_INFO")));
	_ASSERT(m_pStaInfo != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);

	return 0;
}

LRESULT CInfoWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	::SetTimer(m_hWnd, IDT_AUTO_CLOSE, m_nAutoCloseTime, NULL);

	return 0;
}

LRESULT CInfoWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_TIMER == uMsg)
	{
		if (IDT_AUTO_CLOSE == wParam)
		{
			if (m_bInMainDlg)
			{
				CSystemEx::GetInstance()->GetMainDlg()->ClosedInfoWnd(this);
			}
			
			DestroyWindow(m_hWnd);
			delete this;
			return 0;
		}
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CInfoWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CInfoWnd::OnBtnClose(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CInfoWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}

	return 0;
}

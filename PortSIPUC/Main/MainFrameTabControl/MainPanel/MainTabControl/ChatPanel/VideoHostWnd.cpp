#include "stdafx.h"
#include "VideoHostWnd.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


CVideoHostWnd::CVideoHostWnd()
	: m_pStaWait(NULL)
{
}

CVideoHostWnd::~CVideoHostWnd()
{
}

int CVideoHostWnd::InitControls()
{
	m_pStaWait = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_WAIT")));
	_ASSERT(m_pStaWait != NULL);

	return 0;
}

LRESULT CVideoHostWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	return 0;
}
void CVideoHostWnd::HideLabelWait()
{
	if (m_pStaWait!=NULL)
	{
		m_pStaWait->ShowWindow(SW_HIDE);
	}
}
void CVideoHostWnd::ShowLabelWait()
{
	if (m_pStaWait != NULL)
	{
		m_pStaWait->ShowWindow(SW_SHOW);
	}
}
LRESULT CVideoHostWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_WINDOWPOSCHANGED:
	{
		HWND hMediaToolbar = (HWND)GetProp(m_hWnd, GETPROP_MEDIA_TOOLBAR);
		if (::IsWindow(hMediaToolbar))
		{
			CIUIRect rcVideo;
			::GetWindowRect(m_hWnd, rcVideo);

			CIUIRect rcToolBar;
			::GetWindowRect(hMediaToolbar, rcToolBar);

			CRect rcNew;
			CanvasLayout(rcToolBar, CLH_CENTER, CLV_BOTTOM, NULL, NULL,
				rcVideo.Width(), rcVideo.Height(), rcNew);

			IUIClientToScreen(m_hWnd, rcNew);

			::MoveWindow(hMediaToolbar, rcNew.left, rcNew.top,
				rcNew.Width(), rcNew.Height(), TRUE);

			::ShowWindow(hMediaToolbar, IsWindowVisible(m_hWnd) ? SW_SHOW : SW_HIDE);
		}
	}
	break;
	case WM_CHAR:
		if (VK_ESCAPE == wParam)
		{
			HWLWND hwlVideoHost = (HWLWND)::GetProp(m_hWnd, GETPROP_VIDEO_HOST);
			IUI::SendMessage(hwlVideoHost, WM_TOGGLE_VIDEO, 0, 0);
		}
		break;
	default:
		break;
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CVideoHostWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CVideoHostWnd::OnButtonClicked(RoutedEventArgs *pe)
{

	return 0;
}

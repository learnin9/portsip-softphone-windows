#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


HostWnd::HostWnd()
{
	m_hHost = NULL;
}


HostWnd::~HostWnd()
{

}

HWND HostWnd::GetHWND() const
{
	return m_hHost;
}

int HostWnd::SetHWND(HWND hWnd)
{
	if (m_hHost != NULL)
	{
		return -1;
	}

	m_hHost = hWnd;

	return 0;
}

LRESULT HostWnd::EventHandler(RoutedEventArgs *pe)
{
	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->hOriginalSender == m_hWnd)
		{
			if (pe->RoutedEvent == WM_SHOWWINDOW)
			{
				BOOL bShow = pe->lParam;
				::ShowWindow(m_hHost, bShow ? SW_SHOW : SW_HIDE);
			}
			else if (pe->RoutedEvent == WM_WINDOWPOSCHANGED)
			{
				CIUIRect rect;
				GetWindowRectToHWND(rect);
				::MoveWindow(m_hHost, rect.left, rect.top,
					rect.Width(), rect.Height(), TRUE);
			}
			else if (pe->RoutedEvent == WM_ENABLE)
			{
				BOOL bEnable = pe->lParam;
				::EnableWindow(m_hHost, bEnable);
			}
		}
	}

	return CWLWnd::EventHandler(pe);
}

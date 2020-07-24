#include "stdafx.h"
#include "TrayMsgWnd.h"


CTrayMsgWnd::CTrayMsgWnd()
	: m_pStaName(NULL)
	, m_pBtnIgnore(NULL)
	, m_pTreeMsg(NULL)
{
}

CTrayMsgWnd::~CTrayMsgWnd()
{
}

int CTrayMsgWnd::InitControls()
{
	m_pStaName = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_NAME")));
	_ASSERT(m_pStaName != NULL);
	m_pBtnIgnore = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_IGNORE")));
	_ASSERT(m_pBtnIgnore != NULL);
	m_pTreeMsg = (TreeView *)CWLWnd::FromHandle(FindControl(_T("IDC_TREECTRL_MSG")));
	_ASSERT(m_pTreeMsg != NULL);

	return 0;
}

LRESULT CTrayMsgWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	CControlProp *pProp = g_pProject->FindProjectItem(_T("IDW_TRAY_MSG_ITEM.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	SetRedraw(m_pTreeMsg->GetSafeHwnd(), TRUE);

	return 0;
}

LRESULT CTrayMsgWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CTrayMsgWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CTrayMsgWnd::OnBtnMenu(RoutedEventArgs *pe)
{
	MessageBox(m_hWnd, _T("IDC_BTN_MENU"), NULL, MB_OK);
	return 0;
}

LRESULT CTrayMsgWnd::OnBtnIgnore(RoutedEventArgs *pe)
{
	MessageBox(m_hWnd, _T("IDC_BTN_IGNORE"), NULL, MB_OK);
	return 0;
}

LRESULT CTrayMsgWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnIgnore->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnIgnore(pe);
	}

	return 0;
}

LRESULT CTrayMsgWnd::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
	{
		CIUIRect rc;
		GetWindowRectToParent(pe->hOriginalSender, rc);
		int nDeflate = 4;
		rc.DeflateRect(nDeflate, nDeflate, nDeflate, nDeflate);

		//CContact *pContact = (CContact *)IUI::GetUserData(pe->hOriginalSender);
		//if (pContact->IsGroup())
		//{
		//	DrawGroupFace((HDC)pe->lParam, rc, (CGroup *)pContact);
		//}
		//else
		//{
		//	DrawFace((HDC)pe->lParam, rc, pContact->GetFace());
		//}
	}

	return 0;
}

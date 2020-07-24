#include "stdafx.h"
#include "MsgHistoryWnd.h"


CMsgHistoryWnd::CMsgHistoryWnd()
	: m_pBtnIcon(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnMin(NULL)
	, m_pChkMax(NULL)
	, m_pBtnClose(NULL)
	, m_pRadAllMsg(NULL)
	, m_pRadPicMsg(NULL)
	, m_pRadFileMsg(NULL)
	, m_pBtnFirstPage(NULL)
	, m_pBtnPrePage(NULL)
	, m_pBtnNextPage(NULL)
	, m_pBtnLastPage(NULL)
	, m_pHistory(NULL)
{
	
}

CMsgHistoryWnd::~CMsgHistoryWnd()
{
}

int CMsgHistoryWnd::InitControls()
{
	m_pBtnIcon = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_ICON")));
	_ASSERT(m_pBtnIcon != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pRadAllMsg = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_RAD_ALL_MSG")));
	_ASSERT(m_pRadAllMsg != NULL);
	m_pRadPicMsg = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_RAD_PIC_MSG")));
	_ASSERT(m_pRadPicMsg != NULL);
	m_pRadFileMsg = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_RAD_FILE_MSG")));
	_ASSERT(m_pRadFileMsg != NULL);
	m_pBtnFirstPage = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_FIRST_PAGE")));
	_ASSERT(m_pBtnFirstPage != NULL);
	m_pBtnPrePage = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_PRE_PAGE")));
	_ASSERT(m_pBtnPrePage != NULL);
	m_pBtnNextPage = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_NEXT_PAGE")));
	_ASSERT(m_pBtnNextPage != NULL);
	m_pBtnLastPage = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_LAST_PAGE")));
	_ASSERT(m_pBtnLastPage != NULL);
	m_pHistory = (IMRichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_IM_HISTORY")));
	_ASSERT(m_pHistory != NULL);

	return 0;
}

LRESULT CMsgHistoryWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	m_pHistory->SetSenderIndent(40);
	m_pHistory->SetMsgLeftIndent(52);
	m_pHistory->SetMsgRightIndent(50);
	m_pHistory->SetMaxRelayoutWidth(1000);
	m_pHistory->SetMsgSenderTitleAlias(0, _T(""));
	m_pHistory->SetMsgSenderTitleAlias(1, _T(""));
	m_pHistory->ShowMsgSenderTitle(TRUE);
	m_pHistory->SetCombineMsg(TRUE);
	m_pHistory->SetSenderTopQiPao(_T("pop_left_up.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSenderMidQiPao(_T("pop_left_mid.png"), CIUIRect(1, 15, 15, 15));
	m_pHistory->SetSenderBottomQiPao(_T("pop_left_end.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSelfTopQiPao(_T("pop_right_up.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSelfMidQiPao(_T("pop_right_mid.png"), CIUIRect(15, 15, 1, 15));
	m_pHistory->SetSelfBottomQiPao(_T("pop_right_end.png"), CIUIRect(15, 15, 15, 15));

	m_pHistory->LineScroll(MAXINT, 0);

	return 0;
}

LRESULT CMsgHistoryWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CMsgHistoryWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CMsgHistoryWnd::OnBtnIcon(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnChkMax(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnClose(RoutedEventArgs *pe)
{
	::DestroyWindow(m_hWnd);
	delete this;
	pe->bHandled = TRUE;
	pe->lResult = TRUE;
	g_pMsgHistoryWnd = NULL;
	return 1;
}

LRESULT CMsgHistoryWnd::OnRadAllMsg(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnRadPicMsg(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnRadFileMsg(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnFirstPage(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnPrePage(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnNextPage(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnBtnLastPage(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMsgHistoryWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnIcon->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnIcon(pe);
	}
	else if (m_pBtnMin->GetSafeHwnd() == pe->hOriginalSender)
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
	else if (m_pRadAllMsg->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadAllMsg(pe);
	}
	else if (m_pRadPicMsg->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadPicMsg(pe);
	}
	else if (m_pRadFileMsg->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadFileMsg(pe);
	}
	else if (m_pBtnFirstPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnFirstPage(pe);
	}
	else if (m_pBtnPrePage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnPrePage(pe);
	}
	else if (m_pBtnNextPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNextPage(pe);
	}
	else if (m_pBtnLastPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnLastPage(pe);
	}

	return 0;
}

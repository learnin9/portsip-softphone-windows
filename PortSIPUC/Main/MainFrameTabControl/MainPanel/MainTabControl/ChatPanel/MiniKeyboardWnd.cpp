#include "stdafx.h"
#include "MiniKeyboardWnd.h"
#include "System/SystemEx.h"

CMiniKeyboardWnd::CMiniKeyboardWnd()
	: m_pBtn1(NULL)
	, m_pBtn2(NULL)
	, m_pBtn3(NULL)
	, m_pBtn4(NULL)
	, m_pBtn5(NULL)
	, m_pBtn6(NULL)
	, m_pBtn7(NULL)
	, m_pBtn8(NULL)
	, m_pBtn9(NULL)
	, m_pBtnXing(NULL)
	, m_pBtn0(NULL)
	, m_pBtnSharp(NULL)
{
	m_nActiveSession = 0;
}

CMiniKeyboardWnd::~CMiniKeyboardWnd()
{
}

int CMiniKeyboardWnd::InitControls()
{
	m_pBtn1 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_1")));
	_ASSERT(m_pBtn1 != NULL);
	m_pBtn2 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_2")));
	_ASSERT(m_pBtn2 != NULL);
	m_pBtn3 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_3")));
	_ASSERT(m_pBtn3 != NULL);
	m_pBtn4 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_4")));
	_ASSERT(m_pBtn4 != NULL);
	m_pBtn5 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_5")));
	_ASSERT(m_pBtn5 != NULL);
	m_pBtn6 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_6")));
	_ASSERT(m_pBtn6 != NULL);
	m_pBtn7 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_7")));
	_ASSERT(m_pBtn7 != NULL);
	m_pBtn8 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_8")));
	_ASSERT(m_pBtn8 != NULL);
	m_pBtn9 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_9")));
	_ASSERT(m_pBtn9 != NULL);
	m_pBtnXing = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_XING")));
	_ASSERT(m_pBtnXing != NULL);
	m_pBtn0 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_0")));
	_ASSERT(m_pBtn0 != NULL);
	m_pBtnSharp = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_SHARP")));
	_ASSERT(m_pBtnSharp != NULL);

	return 0;
}

LRESULT CMiniKeyboardWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	SetFocus();

	return 0;
}

LRESULT CMiniKeyboardWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_KILLFOCUS == uMsg)
	{
		if (IsWindowVisible(m_hWnd))
		{
			DestroyWindow(m_hWnd);
			return 0;
		}
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CMiniKeyboardWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CMiniKeyboardWnd::OnBtn1(RoutedEventArgs *pe)
{
	if (m_nActiveSession>0)
	{
		CSystemEx::GetInstance()->BtnAudio('1');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 1);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn2(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('2');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 2);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn3(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('3');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 3);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn4(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('4');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 4);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn5(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('5');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 5);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn6(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('6');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 6);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn7(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('7');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 7);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn8(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('8');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 8);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn9(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('9');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 9);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtnXing(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('*');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 10);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtn0(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('0');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 0);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnBtnSharp(RoutedEventArgs *pe)
{
	if (m_nActiveSession > 0)
	{
		CSystemEx::GetInstance()->BtnAudio('#');
		CSystemEx::GetInstance()->SendDTMF(m_nActiveSession, 11);
	}
	return 0;
}

LRESULT CMiniKeyboardWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtn1->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn1(pe);
	}
	else if (m_pBtn2->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn2(pe);
	}
	else if (m_pBtn3->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn3(pe);
	}
	else if (m_pBtn4->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn4(pe);
	}
	else if (m_pBtn5->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn5(pe);
	}
	else if (m_pBtn6->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn6(pe);
	}
	else if (m_pBtn7->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn7(pe);
	}
	else if (m_pBtn8->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn8(pe);
	}
	else if (m_pBtn9->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn9(pe);
	}
	else if (m_pBtnXing->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnXing(pe);
	}
	else if (m_pBtn0->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtn0(pe);
	}
	else if (m_pBtnSharp->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSharp(pe);
	}

	return 0;
}

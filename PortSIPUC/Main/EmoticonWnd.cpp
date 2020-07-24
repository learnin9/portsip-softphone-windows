#include "stdafx.h"
#include "EmoticonWnd.h"
#include "EmoticonPageCanvasHandler.h"


CEmoticonWnd::CEmoticonWnd(HWLWND hNotify)
	: m_pRadClassicalEmoticon(NULL)
	, m_pRadSymbolEmoticon(NULL)
	, m_pTabEmoticon(NULL)
	, m_hNotify(hNotify)
{
}

CEmoticonWnd::~CEmoticonWnd()
{
}

int CEmoticonWnd::InitControls()
{
	m_pRadClassicalEmoticon = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_RAD_CLASSICAL_EMOTICON")));
	_ASSERT(m_pRadClassicalEmoticon != NULL);
	m_pRadSymbolEmoticon = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_RAD_SYMBOL_EMOTICON")));
	_ASSERT(m_pRadSymbolEmoticon != NULL);
	m_pTabEmoticon = (TabControl *)CWLWnd::FromHandle(FindControl(_T("IDC_PNL_TAB_EMOTICON")));
	_ASSERT(m_pTabEmoticon != NULL);

	return 0;
}

LRESULT CEmoticonWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();


	CEmoticonPageCanvasHandler *pClassicalEmoticonPageCanvasHandler = ALLOC_HANDLER(CEmoticonPageCanvasHandler);
	pClassicalEmoticonPageCanvasHandler->SetUserData(EP_CLASSICAL);
	m_pTabEmoticon->AddPropertySheet(m_pRadClassicalEmoticon->GetSafeHwnd(), _T("IDW_EMOTICON_PAGE.xml"),
		pClassicalEmoticonPageCanvasHandler);

	CEmoticonPageCanvasHandler *pSymbolEmoticonPageCanvasHandler = ALLOC_HANDLER(CEmoticonPageCanvasHandler);
	pSymbolEmoticonPageCanvasHandler->SetUserData(EP_SYMBOL);
	m_pTabEmoticon->AddPropertySheet(m_pRadSymbolEmoticon->GetSafeHwnd(), _T("IDW_EMOTICON_PAGE.xml"),
		pSymbolEmoticonPageCanvasHandler);
	m_pTabEmoticon->SwitchPropertySheet(m_pRadClassicalEmoticon->GetSafeHwnd());

	SetFocus();

	return 0;
}

LRESULT CEmoticonWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_KILLFOCUS == uMsg)
	{
		if (IsWindowVisible(m_hWnd))
		{
			::ShowWindow(m_hWnd, SW_HIDE);
			return 0;
		}
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CEmoticonWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CEmoticonWnd::OnRadClassicalEmoticon(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CEmoticonWnd::OnRadSymbolEmoticon(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CEmoticonWnd::OnStaEmotion(RoutedEventArgs *pe)
{
	Label *pStaEmotion = (Label *)CWLWnd::FromHandle(pe->hOriginalSender);
	BOOL bCombineImage = TRUE;
	CIUIString strGif;
	pStaEmotion->GetBkImage(CONTROL_STATE_NORMAL, &bCombineImage, NULL, &strGif, NULL);

	if (strGif.IsEmpty())
	{
		_ASSERT(FALSE);
	}
	else
	{
		::ShowWindow(m_hWnd, SW_HIDE);

		if (NULL != m_hNotify)
		{
			RoutedEventArgs args;
			args.RoutedEvent = WM_SELECTED_EMOTICON;
			args.eRoutingStrategy = RS_DIRECT;
			args.hSender = args.hOriginalSender = pe->hOriginalSender;
			args.lParam = (LPARAM)(LPCTSTR)strGif;
			RaiseEvent(m_hNotify, &args);
		}

		return 0;
	}

	return 0;
}

LRESULT CEmoticonWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pRadClassicalEmoticon->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadClassicalEmoticon(pe);
	}
	else if (m_pRadSymbolEmoticon->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadSymbolEmoticon(pe);
	}

	//
	// 如果是点击了表情Item，则把表情插入到输入框。
	//
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_EMOTICON"))
	{
		OnStaEmotion(pe);
	}

	return 0;
}

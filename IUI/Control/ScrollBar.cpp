#include "stdafx.h"
#include "ControlCore/Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


ScrollBar::ScrollBar()
{
}

ScrollBar::~ScrollBar()
{

}

BOOL ScrollBar::Create(
	CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParentWnd,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_SCROLLBAR, NULL, dwStyle, rect, hParentWnd, pParent, lpszName);
}

int ScrollBar::GetScrollPos() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(SBM_GETPOS, 0, 0L);
}

int ScrollBar::SetScrollPos(int nPos, BOOL bRedraw/* = TRUE*/)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(SBM_SETPOS, nPos, bRedraw);
}

int ScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const
{
	_ASSERT(IsWindow());
	SendMessage(SBM_GETRANGE, (WPARAM)lpMinPos, (LPARAM)lpMaxPos);

	return 0;
}

int ScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw/* = TRUE*/)
{
	_ASSERT(IsWindow());
	SendMessage(SBM_SETRANGE, nMinPos, nMaxPos);

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

void ScrollBar::ShowScrollBar(BOOL bShow/* = TRUE*/)
{
	ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

BOOL ScrollBar::EnableScrollBar(UINT nArrowFlags/* = ESB_ENABLE_BOTH*/)
{
	return FALSE;
}

BOOL ScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw/* = TRUE*/)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(SBM_SETSCROLLINFO, bRedraw, (LPARAM)lpScrollInfo);
}

BOOL ScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask/* = SIF_ALL*/)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(SBM_GETSCROLLINFO, 0, (LPARAM)lpScrollInfo);
}

int ScrollBar::GetScrollLimit()
{
	int nMin, nMax;
	GetScrollRange(&nMin, &nMax);
	SCROLLINFO info;
	if (GetScrollInfo(&info, SIF_PAGE))
	{
		nMax -= __max(info.nPage - 1, 0);
	}
	return nMax;
}

BOOL ScrollBar::GetScrollBarInfo(PSCROLLBARINFO pScrollInfo) const
{
#define SBM_GETSCROLLBARINFO        0x00EB

	_ASSERT(IsWindow());
	return (BOOL)SendMessage(SBM_GETSCROLLBARINFO, NULL, (LPARAM)pScrollInfo);
}

///
// ScrollBar API:

int IUI::ScrollBar_BindStyle(HWLWND hWnd, const CScrollBarProp *pProp)
{
	if (pProp == NULL)
	{
		return -1;
	}

	// 轨道
	ScrollBar_SetCombineGrooveImage(hWnd, pProp->m_strBKCombine);
	ScrollBar_SetGrooveImage9GridResizeParam(hWnd, &(RECT)pProp->m_rcBkImage9GridResizeArg);

	// Button1
	ScrollBar_SetButton1Length(hWnd, (LONG)pProp->m_nButton1Width);
	ScrollBar_SetCombineButton1BkImage(hWnd, pProp->m_strButton1Combine);
	ScrollBar_SetButton1BkImage(hWnd, CONTROL_STATE_ALL,
		pProp->m_strButton1ImageN, pProp->m_strButton1ImageH,
		pProp->m_strButton1ImageP, pProp->m_strButton1ImageD, FALSE);

	// Button2
	ScrollBar_SetButton2Length(hWnd, (LONG)pProp->m_nButton2Width);
	ScrollBar_SetCombineButton2BkImage(hWnd, pProp->m_strButton2Combine);
	ScrollBar_SetButton2BkImage(hWnd, CONTROL_STATE_ALL,
		pProp->m_strButton2ImageN, pProp->m_strButton2ImageH,
		pProp->m_strButton2ImageP, pProp->m_strButton2ImageD, FALSE);

	// Thumb
	ScrollBar_SetCombineThumbBkImage(hWnd, pProp->m_strThumbCombine);
	ScrollBar_SetThumbBkImage(hWnd, CONTROL_STATE_ALL,
		pProp->m_strThumbN, pProp->m_strThumbH,
		pProp->m_strThumbP, pProp->m_strThumbD, FALSE);
	ScrollBar_SetThumb9GridResizeParam(hWnd, &(RECT)pProp->m_rcThumbResize);

	return 0;
}

// 设置轨道组合背景
int IUI::ScrollBar_SetCombineGrooveImage(HWLWND hSb, LPCTSTR lpszImage)
{
	return SetCombineBkImage(hSb, lpszImage);
}

// 设置轨道非组合背景
int IUI::ScrollBar_SetGrooveImage(
	HWLWND hSb,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw)
{
	IUISetControlBkImage4(hSb, FALSE, uMask,
		lpszImageNameN, lpszImageNameH, lpszImageNameP, lpszImageNameD);

	if (bRedraw)
	{
		Invalidate(hSb);
	}

	return 0;
}

int IUI::ScrollBar_SetGrooveImage9GridResizeParam(HWLWND hSb, LPCRECT lpszResize)
{
	return SetBkImage9GridResizeParam(hSb, lpszResize);
}

int IUIAPI ScrollBar_GetGrooveImage9GridResizeParam(HWLWND hSb, LPRECT lpszResize)
{
	return GetBkImage9GridResizeParam(hSb, lpszResize);
}

// 设置垂直滚动条上按钮或水平滚动条左按钮背景图
int IUI::ScrollBar_SetButton1Length(HWLWND hSb, int nWidth)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_SET_BUTTON_LENGTH, MAKEWPARAM(0, 0), nWidth);
}

int IUI::ScrollBar_GetButton1Length(HWLWND hSb)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_GET_BUTTON_LENGTH, MAKEWPARAM(0, 0), 0);
}

int IUI::ScrollBar_SetCombineButton1BkImage(HWLWND hSb, LPCTSTR lpszImage)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
			MAKEWPARAM(0, CONTROL_STATE_COMBINE), (LPARAM)lpszImage);
}

int IUI::ScrollBar_SetButton1BkImage(
	HWLWND hSb,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw)
{
	_ASSERT(IsWindow(hSb));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_NORMAL), LPARAM(lpszImageNameN));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_HOVER), LPARAM(lpszImageNameH));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_PRESSED), LPARAM(lpszImageNameP));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_DISABLED), LPARAM(lpszImageNameD));
	}

	if (bRedraw)
	{
		Invalidate(hSb);
	}

	return 0;
}

int IUI::ScrollBar_SetButton2Length(HWLWND hSb, int nWidth)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_SET_BUTTON_LENGTH, MAKEWPARAM(1, 0), nWidth);
}

int IUI::ScrollBar_GetButton2Length(HWLWND hSb)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_GET_BUTTON_LENGTH, MAKEWPARAM(1, 0), 0);
}

int IUI::ScrollBar_SetCombineButton2BkImage(HWLWND hSb, LPCTSTR lpszImage)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
			MAKEWPARAM(1, CONTROL_STATE_COMBINE), (LPARAM)lpszImage);
}

int IUI::ScrollBar_SetButton2BkImage(
	HWLWND hSb,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw)
{
	_ASSERT(IsWindow(hSb));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(1, CONTROL_STATE_NORMAL), LPARAM(lpszImageNameN));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(1, CONTROL_STATE_HOVER), LPARAM(lpszImageNameH));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(1, CONTROL_STATE_PRESSED), LPARAM(lpszImageNameP));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		return SendMessage(hSb, SBM_SET_BUTTON_IMAGE,
				MAKEWPARAM(1, CONTROL_STATE_DISABLED), LPARAM(lpszImageNameD));
	}

	if (bRedraw)
	{
		Invalidate(hSb);
	}

	return 0;
}

int IUI::ScrollBar_SetCombineThumbBkImage(HWLWND hSb, LPCTSTR lpszImage)
{
	_ASSERT(IsWindow(hSb));
	return SendMessage(hSb, SBM_SET_THUMB_IMAGE,
			MAKEWPARAM(0, CONTROL_STATE_COMBINE), (LPARAM)lpszImage);
}

int IUI::ScrollBar_SetThumbBkImage(
	HWLWND hSb,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw)
{
	_ASSERT(IsWindow(hSb));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		return SendMessage(hSb, SBM_SET_THUMB_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_NORMAL), LPARAM(lpszImageNameN));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		return SendMessage(hSb, SBM_SET_THUMB_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_HOVER), LPARAM(lpszImageNameH));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		return SendMessage(hSb, SBM_SET_THUMB_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_PRESSED), LPARAM(lpszImageNameP));
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		return SendMessage(hSb, SBM_SET_THUMB_IMAGE,
				MAKEWPARAM(0, CONTROL_STATE_DISABLED), LPARAM(lpszImageNameD));
	}

	if (bRedraw)
	{
		Invalidate(hSb);
	}

	return 0;
}

int IUI::ScrollBar_SetThumb9GridResizeParam(HWLWND hSb, LPCRECT lprcParam)
{
	if (!IsWindow(hSb))
	{
		return -1;
	}

	if (lprcParam == NULL)
	{
		return -2;
	}

	SBWND *pSb = (SBWND *)GetMember(hSb);
	pSb->m_rcThumbImageResize = lprcParam;

	return 0;
}

int IUI::ScrollBar_GetThumb9GridResizeParam(HWLWND hSb, LPRECT lprcParam)
{
	if (!IsWindow(hSb))
	{
		return -1;
	}

	if (lprcParam == NULL)
	{
		return -2;
	}

	SBWND *pSb = (SBWND *)GetMember(hSb);
	*lprcParam = pSb->m_rcThumbImageResize;

	return 0;
}

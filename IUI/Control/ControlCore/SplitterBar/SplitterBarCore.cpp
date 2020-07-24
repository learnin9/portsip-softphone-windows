#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


// SplitterBar是一个Windowless控件，在拖动SplitterBar的时候，临时创建一个Windowed、半透明
// 窗口用来显示SplitterBar，等松开鼠标后，再销毁这个临时创建的窗口。 VS2010就是用的这种模式。
// CTrackingSplitterBar就是那个临时创建的窗口
class CTrackingSplitterBar : public Window
{
public:
	CTrackingSplitterBar();
	virtual ~CTrackingSplitterBar();

	virtual BOOL LoadFrame(
		const RECT &rect,
		SPLITTER_ORIENTATION eOrientation,
		HWLWND hBindSplitterBar,
		HWLWND hPanel1,
		HWLWND hPanel2
	);
	int ReplacePanel12(HWLWND hNewPanel1, HWLWND hNewPanel2);

public:
	int SetBkImages(LPCTSTR lpszImageNameN, BOOL bRedraw = TRUE);
	int GetBkImages(CIUIString *pstrImageNameN);

	virtual int StartTracking();
	virtual int StopTracking(BOOL bAccept);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrackingSplitterBar)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	HIUIIMAGE m_himgBk;
	CIUIRect m_rc9GridArg;
	CIUIRect m_rcOld;
	SPLITTER_ORIENTATION m_eOrientation;
	HWLWND m_hBindSplitterBar;
	HWLWND m_hPanel1;
	HWLWND m_hPanel2;
};


CTrackingSplitterBar::CTrackingSplitterBar()
{
	m_eOrientation = SO_WE;
	m_hBindSplitterBar = NULL;
	m_hPanel1 = NULL;
	m_hPanel2 = NULL;
	m_himgBk = NULL;
}

CTrackingSplitterBar::~CTrackingSplitterBar()
{
	if (m_himgBk != NULL)
	{
		ReleaseIUIImage(m_himgBk);
		m_himgBk = NULL;
	}
}

BOOL CTrackingSplitterBar::LoadFrame(
	const RECT &rect,
	SPLITTER_ORIENTATION eOrientation,
	HWLWND hBindSplitterBar,
	HWLWND hPanel1,
	HWLWND hPanel2)
{
	// attempt to create the window
	CIUIString strMyClass;
	strMyClass = _T("IUITrackSplitter");
	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = IUIGetInstanceHandle();
	wndcls.hIcon = NULL;
	wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = (LPCTSTR) strMyClass;

	// Register the new class and exit if it fails
	if (!RegisterClass(&wndcls))
	{
		IUITRACE(_T("[LibUIDK]===Class Registration Failed\n"));
		return FALSE;
	}

	m_rcOld = rect;
	m_eOrientation = eOrientation;
	m_hBindSplitterBar = hBindSplitterBar;
	m_hPanel1 = hPanel1;
	m_hPanel2 = hPanel2;

	if (!CreateEx(WS_EX_TOOLWINDOW, strMyClass, NULL, WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN, rect, NULL, 0))
	{
		return FALSE;
	}

	return TRUE;
}

int CTrackingSplitterBar::ReplacePanel12(
	HWLWND hNewPanel1,
	HWLWND hNewPanel2)
{
	m_hPanel1 = hNewPanel1;
	m_hPanel2 = hNewPanel2;

	return 0;
}

int CTrackingSplitterBar::SetBkImages(LPCTSTR lpszImageName, BOOL bRedraw/* = TRUE*/)
{
	_ASSERT(m_hWnd != NULL);

	IUISetControlImage(GetProject(), &m_himgBk, lpszImageName);

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int CTrackingSplitterBar::GetBkImages(CIUIString *pstrImageName)
{
	IUIGetControlImage(m_himgBk, pstrImageName);

	return 0;
}

int CTrackingSplitterBar::StartTracking()
{
	_ASSERT(m_hBindSplitterBar != NULL);

	return IUI::SendMessage(m_hBindSplitterBar, SPB_START_TRACKING, 0, 0);
}

int CTrackingSplitterBar::StopTracking(BOOL bAccept)
{
	_ASSERT(m_hBindSplitterBar != NULL);

	return IUI::SendMessage(m_hBindSplitterBar, SPB_STOP_TRACKING, 0, bAccept);
}

LRESULT CTrackingSplitterBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
		HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
		if (hInst != NULL)
		{
			typedef BOOL (WINAPI * MYFUNC)(HWND, COLORREF, BYTE, DWORD);
			MYFUNC SetLayeredWindowAttributes = NULL;
			SetLayeredWindowAttributes = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
			if (SetLayeredWindowAttributes)
			{
				SetLayeredWindowAttributes(this->GetSafeHwnd(), 0, 150, LWA_ALPHA);
			}

			FreeLibrary(hInst);
		}
	}
	else if (message == WM_ERASEBKGND)
	{
		return TRUE;
	}
	else if (message == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m_hWnd, &ps);

		CIUIRect rcClient;
		GetClientRect(rcClient);

		HDC hdcMem = CreateCompatibleDC(hDC);

		HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rcClient.Width(), rcClient.Height());

		HBITMAP hbmpMemOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

		IUINineGridBlt(hdcMem, rcClient, m_himgBk, m_rc9GridArg);

		BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(), hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpMemOld);
		EndPaint(m_hWnd, &ps);
	}
	else if (message == WM_CANCELMODE)
	{
		StopTracking(FALSE);
	}
	else if (message == WM_KEYDOWN)
	{
		CIUIPoint pt;
		GetCursorPos(&pt);

		int cz = GetKeyState(VK_CONTROL) < 0 ? 1 : 16;
		int dx = 0;
		int dy = 0;

		int nChar = (int)wParam;
		switch (nChar)
		{
		case VK_ESCAPE:
			StopTracking(FALSE);
			return Window::WindowProc(message, wParam, lParam);
		case VK_RETURN:
			StopTracking(TRUE);
			return Window::WindowProc(message, wParam, lParam);

		case VK_LEFT:
			dx = -1;
			break;
		case VK_RIGHT:
			dx = +1;
			break;
		case VK_UP:
			dy = -1;
			break;
		case VK_DOWN:
			dy = +1;
			break;

		default:
			//Default();  // pass other keys through
			return Window::WindowProc(message, wParam, lParam);
		}
	}
	else if (message == WM_WINDOWPOSCHANGING)	// Limit splitter bar's position
	{
		WINDOWPOS *pwp = (WINDOWPOS *)lParam;

		// Creating
		if (pwp->cx == 0 || pwp->cy == 0)
		{
			return Window::WindowProc(message, wParam, lParam);
		}

		// Get max limit rect
		CIUIRect rcPanel1;
		IUI::GetWindowRect(m_hPanel1, rcPanel1);
		CIUIRect rcPanel2;
		IUI::GetWindowRect(m_hPanel2, rcPanel2);
		CIUIRect rcLimit;
		rcLimit.left = rcPanel1.left;
		rcLimit.top = rcPanel1.top;
		rcLimit.right = rcPanel2.right;
		rcLimit.bottom = rcPanel2.bottom;

		// Get the minimize size of the panel.
		CIUIRect rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

		CIUISize size1(0, 0);
		MINMAXINFO MMI1;
		MMI1.ptMinTrackSize.x = size1.cx;
		MMI1.ptMinTrackSize.y = size1.cy;
		MMI1.ptMaxSize.x = rect.Width();
		MMI1.ptMaxSize.y = rect.Height();
		IUI::SendMessage(m_hPanel1, WM_GETMINMAXINFO, 0, (LPARAM)(&MMI1));

		CIUISize size2(0, 0);
		MINMAXINFO MMI2;
		MMI2.ptMinTrackSize.x = size2.cx;
		MMI2.ptMinTrackSize.y = size2.cy;
		MMI2.ptMaxSize.x = rect.Width();
		MMI2.ptMaxSize.y = rect.Height();
		IUI::SendMessage(m_hPanel1, WM_GETMINMAXINFO, 0, (LPARAM)(&MMI2));

		// Limit
		if (m_eOrientation == SO_WE)
		{
			pwp->y = m_rcOld.top;

			if (pwp->x < rcLimit.left + MMI1.ptMinTrackSize.x)
			{
				pwp->x = rcLimit.left + MMI1.ptMinTrackSize.x;
			}
			if (pwp->x + pwp->cx > rcLimit.right - MMI2.ptMinTrackSize.x)
			{
				pwp->x = rcLimit.right - MMI2.ptMinTrackSize.x - pwp->cx;
			}

			IUI::SendMessage(m_hBindSplitterBar, SPB_SET_OFFSET, 0, pwp->x - m_rcOld.left);
		}
		else if (m_eOrientation == SO_NS)
		{
			pwp->x = m_rcOld.left;

			if (pwp->y < rcLimit.top + MMI1.ptMinTrackSize.y)
			{
				pwp->y = rcLimit.top + MMI1.ptMinTrackSize.y;
			}
			if (pwp->y + pwp->cy > rcLimit.bottom - MMI2.ptMinTrackSize.y)
			{
				pwp->y = rcLimit.bottom - MMI2.ptMinTrackSize.y - pwp->cy;
			}

			IUI::SendMessage(m_hBindSplitterBar, SPB_SET_OFFSET, 0, pwp->y - m_rcOld.top);
		}
	}
	else if (message == WM_EXITSIZEMOVE)
	{
		// If press esc key while tracking the bar, in WM_WINDOWPOSCHANGING message, the WINDOWPOS::x and WINDOWPOS::y will be restored.
		// and SplitterBar::m_ptTrackOffset will be 0. so, at this step, can use TRUE to call StopTracking.
		StopTracking(TRUE);
	}

	return Window::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

struct SPLITTERBAR
{
	SPLITTERBAR()
	{
		m_hwnd = NULL;
		m_hPanel1 = NULL;
		m_hPanel2 = NULL;
		for (int i = 0; i < 4; ++i)
		{
			m_himgBk[i] = NULL;
		}

		m_hHCursor = NULL;
		m_hVCursor = NULL;
		m_eOrientation = SO_WE;
		m_bIsMouseOver = FALSE;
		m_bTracking = FALSE;
		memset(&m_rectTracker, 0, sizeof(m_rectTracker));
		memset(&m_rectLimit, 0, sizeof(m_rectLimit));
		memset(&m_ptTrackOffset, 0, sizeof(m_ptTrackOffset));
		memset(&m_ptLButtonDown, 0, sizeof(m_ptLButtonDown));
		m_dwHoverTime = HOVER_DEFAULT;
		m_bHoverRepeat = FALSE;
	}

	int Release()
	{
		m_hPanel1 = NULL;
		m_hPanel2 = NULL;
		for (int i = 0; i < 4; ++i)
		{
			ReleaseIUIImage(m_himgBk[i]);
			m_himgBk[i] = NULL;
		}

		m_hHCursor = NULL;
		m_hVCursor = NULL;
		m_eOrientation = SO_WE;
		m_bIsMouseOver = FALSE;
		m_bTracking = FALSE;
		memset(&m_rectTracker, 0, sizeof(m_rectTracker));
		memset(&m_rectLimit, 0, sizeof(m_rectLimit));
		memset(&m_ptTrackOffset, 0, sizeof(m_ptTrackOffset));
		memset(&m_ptLButtonDown, 0, sizeof(m_ptLButtonDown));
		m_dwHoverTime = HOVER_DEFAULT;
		m_bHoverRepeat = FALSE;

		return 0;
	}

	HWLWND m_hwnd;
	HWLWND m_hPanel1;	// Left or top panel
	HWLWND m_hPanel2;	// Right or bottom panel

	HIUIIMAGE m_himgBk[4];

	HCURSOR m_hHCursor;
	HCURSOR m_hVCursor;

	SPLITTER_ORIENTATION m_eOrientation;

	BOOL m_bIsMouseOver;

	// Tracking info - only valid when 'm_bTracking' is set
	BOOL m_bTracking;
	CIUIRect m_rectTracker;
	CIUIRect m_rectLimit;
	CIUIPoint m_ptTrackOffset;
	CIUIPoint m_ptLButtonDown;
	CTrackingSplitterBar m_wndTracking;

	// for mouse hover
	DWORD m_dwHoverTime;
	BOOL m_bHoverRepeat;
};

// Copy from CDC::GetHalftoneBrush()
HBRUSH GetHalftoneBrush()
{
	WORD grayPattern[8];
	for (int i = 0; i < 8; i++)
	{
		grayPattern[i] = (WORD)(0x5555 << (i & 1));
	}
	HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);

	HBRUSH hBrush = NULL;
	if (grayBitmap != NULL)
	{
		hBrush = ::CreatePatternBrush(grayBitmap);
		DeleteObject(grayBitmap);
	}

	return hBrush;
}

// Copy from CSplitterWnd::OnInvertTracker
int OnInvertTracker(SPLITTERBAR *pSPBar)
{
	_ASSERT(!pSPBar->m_rectTracker.IsRectEmpty());

	// pat-blt without clip children on
	HDC hDC = GetDC(GetHostWnd(pSPBar->m_hwnd));
	// invert the brush pattern (looks just like frame window sizing)
	HBRUSH hBrush = GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if (hBrush != NULL)
	{
		hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	}
	PatBlt(hDC, pSPBar->m_rectTracker.left, pSPBar->m_rectTracker.top,
		pSPBar->m_rectTracker.Width(), pSPBar->m_rectTracker.Height(), PATINVERT);
	if (hOldBrush != NULL)
	{
		SelectObject(hDC, hOldBrush);
	}
	ReleaseDC(GetHostWnd(pSPBar->m_hwnd), hDC);
	DeleteObject(hBrush);

	return 0;
}

int StartTracking(SPLITTERBAR *pSPBar)
{
	// GetHitRect will restrict 'pSPBar->m_rectLimit' as appropriate

	// set tracking state and appropriate cursor
	pSPBar->m_bTracking = TRUE;

	if (pSPBar->m_himgBk[0] == NULL)
	{
		CIUIPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(pSPBar->m_hwnd, &pt);
		pSPBar->m_ptLButtonDown = pt;

		OnInvertTracker(pSPBar);
	}
	else
	{
		if (pSPBar->m_wndTracking.GetSafeHwnd() == NULL)
		{
			CIUIRect rcWnd;
			GetWindowRect(pSPBar->m_hwnd, rcWnd);

			if (!pSPBar->m_wndTracking.LoadFrame(rcWnd,
					pSPBar->m_eOrientation, pSPBar->m_hwnd, pSPBar->m_hPanel1, pSPBar->m_hPanel2))
			{
				return -2;
			}
			pSPBar->m_wndTracking.SetBkImages(pSPBar->m_himgBk[0]->GetSafeImageName());

			pSPBar->m_wndTracking.SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(0, 0));
		}
		else
		{
			_ASSERT(FALSE);
			pSPBar->m_wndTracking.ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

int StopTracking(SPLITTERBAR *pSPBar, BOOL bAccept)
{
	if (!pSPBar->m_bTracking)
	{
		return 1;
	}

	if (pSPBar->m_wndTracking.GetSafeHwnd() != NULL)
	{
		::DestroyWindow(pSPBar->m_wndTracking.GetSafeHwnd());
	}

	// erase tracker rectangle
	if (pSPBar->m_himgBk[0] == NULL)
	{
		OnInvertTracker(pSPBar);
	}

	pSPBar->m_bTracking = FALSE;

	if (bAccept)
	{
		if (pSPBar->m_eOrientation == SO_WE)
		{
			if (pSPBar->m_hPanel1 != NULL)
			{
				CIUIRect rcOld1;
				GetWindowRect(pSPBar->m_hPanel1, rcOld1);
				SetWindowPos(pSPBar->m_hPanel1, NULL,
					0, 0, rcOld1.Width() + pSPBar->m_ptTrackOffset.x, rcOld1.Height(), SWP_NOMOVE);
			}

			if (pSPBar->m_hPanel2 != NULL)
			{
				CIUIRect rcOld2;
				GetWindowRect(pSPBar->m_hPanel2, rcOld2);
				ScreenToClient(GetParent(pSPBar->m_hwnd), rcOld2);
				MoveWindow(pSPBar->m_hPanel2,
					rcOld2.left + pSPBar->m_ptTrackOffset.x,
					rcOld2.top,
					rcOld2.Width() - pSPBar->m_ptTrackOffset.x, rcOld2.Height(), TRUE);
			}
		}
		else if (pSPBar->m_eOrientation == SO_NS)
		{
			if (pSPBar->m_hPanel1 != NULL)
			{
				CIUIRect rcOld1;
				GetWindowRect(pSPBar->m_hPanel1, rcOld1);
				SetWindowPos(pSPBar->m_hPanel1, NULL,
					0, 0, rcOld1.Width(), rcOld1.Height() + pSPBar->m_ptTrackOffset.y, SWP_NOMOVE);
			}

			if (pSPBar->m_hPanel2 != NULL)
			{
				CIUIRect rcOld2;
				GetWindowRect(pSPBar->m_hPanel2, rcOld2);
				ScreenToClient(GetParent(pSPBar->m_hwnd), rcOld2);
				MoveWindow(pSPBar->m_hPanel2,
					rcOld2.left, rcOld2.top + pSPBar->m_ptTrackOffset.y,
					rcOld2.Width(), rcOld2.Height() - pSPBar->m_ptTrackOffset.y, TRUE);
			}
		}
		else
		{
			_ASSERT(FALSE);
		}

		SendMessage(GetParent(pSPBar->m_hwnd), SPB_STOP_TRACKING, WPARAM(pSPBar->m_hwnd), 0);
	}

	// May be restore active panel.

	return 0;
}

int SplitterBar_DrawBkColor(SPLITTERBAR *pSPBar, HDC hDC, LPCRECT lpRect)
{
	_ASSERT(IsWindow(pSPBar->m_hwnd));

	if (!IsDrawBkColor(pSPBar->m_hwnd))
	{
		return 1;
	}

	// Prepare state index
	CONTROL_STATE eButtonState = GetControlState(pSPBar->m_hwnd);
	if (eButtonState < CONTROL_STATE_NORMAL || eButtonState > CONTROL_STATE_DISABLED)
	{
		_ASSERT(FALSE);
		return -1;
	}
	int nStateIndex = 0;
	if (IsWindowEnabled(pSPBar->m_hwnd))
	{
		nStateIndex = GetButtonStateIndex(eButtonState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pSPBar->m_hwnd, rcClient);

	//FillSolidRect(hDC, rcClient, pSPBar->m_crBk[nStateIndex],
	//	IsUpdateLayeredWindow(GetHostWnd(pSPBar->m_hwnd)));

	return 0;
}

int SplitterBar_DrawBkImage(SPLITTERBAR *pSPBar, HDC hDC, LPCRECT lprcDest)
{
	_ASSERT(IsWindow(pSPBar->m_hwnd));

	BOOL bCheckBox = IsIncludeFlag(GetStyle(pSPBar->m_hwnd), BS_AUTOCHECKBOX);
	BOOL bRadioButton = IsIncludeFlag(GetStyle(pSPBar->m_hwnd), BS_AUTORADIOBUTTON);
	BOOL bToggleButton = bCheckBox | bRadioButton;

	// Prepare state index
	int nStateIndex = 0;
	if (IsWindowEnabled(pSPBar->m_hwnd))
	{
		CONTROL_STATE eButtonState = GetControlState(pSPBar->m_hwnd);
		nStateIndex = GetButtonStateIndex(eButtonState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pSPBar->m_hwnd, rcClient);

	if (IsCombineBkImage(pSPBar->m_hwnd))
	{
		int nImagePart = nStateIndex;

		IUIPartDrawImage(hDC, rcClient,
			pSPBar->m_hwnd->ci.m_himgCombineBk, pSPBar->m_hwnd->ci.m_eBkImageResizeMode,
			pSPBar->m_hwnd->ci.m_rcBkImage9GridResizeArg,
			bToggleButton ? COMBINEIMAGESIZE8 : COMBINEIMAGESIZE4, nImagePart);
	}
	else
	{
		HIUIIMAGE hImgBk = pSPBar->m_himgBk[nStateIndex];

		IUIDrawImage(hDC, rcClient,
			hImgBk, pSPBar->m_hwnd->ci.m_eBkImageResizeMode, pSPBar->m_hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;


	//CRect rcClient;
	//GetClientRect(pSPBar->m_hwnd, rcClient);

	//// Prepare state index
	//int nStateIndex = 0;
	//BOOL bSelected = GetKeyState(VK_LBUTTON) < 0;
	//if (!IsWindowEnabled(pSPBar->m_hwnd))
	//{
	//	// 1.disable
	//	nStateIndex = 3;
	//}
	//else
	//{
	//	if (bSelected)
	//	{
	//		if (pSPBar->m_bIsMouseOver)
	//		{
	//			// 2.selected(push down)
	//			nStateIndex = 2;
	//		}
	//		else
	//		{
	//			// 3.highlight
	//			nStateIndex = 1;
	//		}
	//	}
	//	else
	//	{
	//		if (pSPBar->m_bIsMouseOver)
	//		{
	//			// 3.highlight
	//			nStateIndex = 1;
	//		}
	//		else
	//		{
	//			// 4.normal
	//			nStateIndex = 0;
	//		}
	//	}
	//}
}

int SplitterBar_DrawFgImage(SPLITTERBAR *pSPBar, HDC hDC)
{
	_ASSERT(IsWindow(pSPBar->m_hwnd));

	//BOOL bCheckBox = IsIncludeFlag(GetStyle(pSPBar->m_hwnd), BS_AUTOCHECKBOX);
	//BOOL bRadioButton = IsIncludeFlag(GetStyle(pSPBar->m_hwnd), BS_AUTORADIOBUTTON);
	//BOOL bToggleButton = bCheckBox | bRadioButton;

	//// Prepare state index
	//CONTROL_STATE eButtonState = GetControlState(pSPBar->m_hwnd);
	//int nStateIndex = GetButtonStateIndex(eButtonState);

	//CIUIRect rcClient;
	//GetClientRect(pSPBar->m_hwnd, rcClient);

	//CIUIRect rcFgImageMargin = pSPBar->m_rcForegroundImageMargin;

	//FOREGROUND_ALIGN_HOR eFgAlignHor = pSPBar->m_eForegroundImageAlignHor;
	//FOREGROUND_ALIGN_VER eFgAlignVer = pSPBar->m_eForegroundImageAlignVer;

	//if (pSPBar->m_bCombineFgImage)
	//{
	//	if (pSPBar->m_himgCombineFg->GetSafeHBITMAP() == NULL)
	//	{
	//		pSPBar->m_himgCombineFg->SafeLoadSavedImage();
	//	}

	//	if (pSPBar->m_himgCombineFg->GetSafeHBITMAP() != NULL)
	//	{
	//		int nPartWidth = 0;
	//		nPartWidth = pSPBar->m_himgCombineFg->GetWidth() / (bToggleButton ? COMBINEIMAGESIZE8 : COMBINEIMAGESIZE4);

	//		CIUIRect rcFgPart;

	//		rcFgPart.left = nStateIndex * nPartWidth;

	//		rcFgPart.right = rcFgPart.left + nPartWidth;
	//		rcFgPart.top = 0;
	//		rcFgPart.bottom = pSPBar->m_himgCombineFg->GetHeight();

	//		IUIPartDrawForeground(hDC, rcClient, rcFgImageMargin,
	//			pSPBar->m_himgCombineFg, rcFgPart, eFgAlignHor, eFgAlignVer, -1, 255);
	//	}
	//}
	//else
	//{
	//	HIUIIMAGE hImgFg = pSPBar->m_himgFg[nStateIndex];

	//	IUIDrawForeground(hDC,
	//		rcClient,
	//		pSPBar->m_rcForegroundImageMargin,
	//		hImgFg,
	//		pSPBar->m_eForegroundImageAlignHor,
	//		pSPBar->m_eForegroundImageAlignVer,
	//		RGB(255, 0, 255), 255);
	//}

	return 0;
}

int SplitterBar_DrawBorder(SPLITTERBAR *pSPBar, HDC hDC)
{
	_ASSERT(IsWindow(pSPBar->m_hwnd));

	if (!IsDrawBorderColor(pSPBar->m_hwnd))
	{
		return 1;
	}

	// Prepare state index
	CONTROL_STATE eButtonState = GetControlState(pSPBar->m_hwnd);
	if (!IsWindowEnabled(pSPBar->m_hwnd))
	{
		eButtonState = CONTROL_STATE_DISABLED;
	}

	if (eButtonState < CONTROL_STATE_NORMAL || eButtonState > CONTROL_STATE_DISABLED)
	{
		_ASSERT(FALSE);
		return -1;
	}

	int nStateIndex = GetButtonStateIndex(eButtonState);

	CIUIRect rcClient;
	GetClientRect(pSPBar->m_hwnd, rcClient);

	//COLORREF crBorder = pSPBar->m_crBorder[nStateIndex];

	//HBRUSH hbrBorder = CreateSolidBrush(crBorder);
	//FrameRect(hDC, rcClient, hbrBorder);
	//DeleteObject(hbrBorder);

	return 0;
}

int SplitterBar_OnPaint(SPLITTERBAR *pSPBar, HDC hDC, LPCRECT lprcPaint)
{
	if (!IsRedraw(pSPBar->m_hwnd))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pSPBar->m_hwnd, rcClient);
	CIUIRect rcPaint;
	if (!::IntersectRect(&rcPaint, lprcPaint, rcClient))
	{
		return 2;
	}

	HDC hDCTarget = NULL;
	HDC hMemDC = NULL;
	HBITMAP hOldBmp = NULL;

	// 由于GDI不支持浮点数坐标，所以当使用RectF坐标时，采用GDI+绘制
	// RectF坐标常用于制作平滑移动的动画。 这时，先把控件内容画到一个32位alpha位图上
	// 最后再把位图绘制到指定坐标。
	HBITMAP hBmp = GetCachedAlphaBitmap(pSPBar->m_hwnd);
	hDCTarget = hDC;

	//
	// 1. 先绘制自己
	//
	// TODO: BeforPaint和AfterPaint应该由框架调用
	BeforePaint(pSPBar->m_hwnd, hDCTarget);
	SplitterBar_DrawBkColor(pSPBar, hDCTarget, lprcPaint);
	SplitterBar_DrawBkImage(pSPBar, hDCTarget, NULL);
	SplitterBar_DrawFgImage(pSPBar, hDCTarget);
	//OnDrawCustom(hDCTarget);
	SplitterBar_DrawBorder(pSPBar, hDCTarget);
	AfterPaint(pSPBar->m_hwnd, hDCTarget);

	if (IsCachedMode(pSPBar->m_hwnd) && hMemDC != NULL && hOldBmp != NULL)
	{
		BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
			hDCTarget, 0, 0, SRCCOPY);
		::SelectObject(hMemDC, hOldBmp);
		DeleteDC(hMemDC);
	}

	return 0;
}

LRESULT WINAPI IUI::SplitterBar_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	SPLITTERBAR *pSPBar = (SPLITTERBAR *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (hWnd == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (NULL == pSPBar)
			{
				if (WM_NCCREATE == uMsg)
				{
					CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;

					pSPBar = new SPLITTERBAR;
					pSPBar->m_hwnd = hWnd;
					pSPBar->m_hwnd->ci.hwndParent = GetParent(hWnd);

					SetMember(hWnd, pSPBar);
				}

				return 0;
			}

			switch (uMsg)
			{
			case WM_DESTROY:
				pSPBar->Release();
				delete pSPBar;
				SetMember(hWnd, NULL);
				return 0;

			case WM_KILLFOCUS:
			{
				BOOL bVisible = IsWindowVisible(hWnd);
				if (bVisible)
				{
					SetRedraw(hWnd, FALSE);
				}
				LRESULT lr = DefEventHandler(hWnd, pe);
				if (bVisible)
				{
					SetRedraw(hWnd, TRUE);
					Invalidate(hWnd);
				}

				return lr;
			}

			default:
				break;
			}
		}
		else if (RS_TUNNEL == pe->eRoutingStrategy)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
			{
				Invalidate(hWnd);

				// when the button as a menu bar, click the button more then once,
				// can not accept WM_MOUSELEAVE message after the pop-up menu has been destroy
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = GetHostWnd(hWnd);
				tme.dwHoverTime = HOVER_DEFAULT;
				::TrackMouseEvent(&tme);
				Invalidate(hWnd);

				//
				// following code for tracking...
				//
				if (!pSPBar->m_bTracking)
				{
					SendMessage(hWnd, SPB_START_TRACKING, 0, 0);
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				// UI
				if (!pSPBar->m_bIsMouseOver)
				{
					pSPBar->m_bIsMouseOver = TRUE;

					// when set RGN for highlight stats, show the tool tip when mouse over the button(the tool tip and highlight stats at the same time work).
					// don't call Invalidate() after this "if" section
					Invalidate(hWnd);
				}

				// when the button as a menu bar, can not accept WM_MOUSELEAVE message after the pop-up menu has been destroy
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.hwndTrack = GetHostWnd(hWnd);
				tme.dwHoverTime = pSPBar->m_dwHoverTime;
				::TrackMouseEvent(&tme);

				//
				// the following code for tracking
				//
				StopTracking(pSPBar, TRUE);
			}
			break;

			case WM_MOUSEHOVER:
			{
				if (pSPBar->m_bHoverRepeat)
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER;
					tme.hwndTrack = GetHostWnd(hWnd);
					tme.dwHoverTime = pSPBar->m_dwHoverTime;
					::TrackMouseEvent(&tme);
				}
			}
			break;

			case WM_MOUSELEAVE:
			{
				pSPBar->m_bIsMouseOver = FALSE;
				SetControlState(hWnd, CONTROL_STATE_NORMAL);
				Invalidate(hWnd);
			}
			break;

			default:
				break;
			}
		}
		else if (RS_DIRECT == pe->eRoutingStrategy)
		{
			switch (uMsg)
			{
			case WM_SETCURSOR:
			{
				HWND hFocus = HWND(wParam);
				if (!IsIconic(GetHostWnd(hWnd)))
				{
					if (pSPBar->m_eOrientation == SO_WE)
					{
						if (pSPBar->m_hHCursor != NULL)
						{
							SetCursor(pSPBar->m_hHCursor);
						}
						else
						{
							// Copy from CSplitterWnd::SetSplitCursor
							//HINSTANCE hInst = FindResourceHandle(MAKEINTRESOURCE(AFX_IDC_HSPLITBAR), RT_GROUP_CURSOR);
							//HCURSOR hCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_HSPLITBAR));
							//if (hCursor != NULL)
							//{
							//	SetCursor(hCursor);
							//}
							//else
							{
								SetCursor(LoadCursor(NULL, IDC_SIZEWE));
							}
						}
					}
					else if (pSPBar->m_eOrientation == SO_NS)
					{
						if (pSPBar->m_hVCursor != NULL)
						{
							SetCursor(pSPBar->m_hVCursor);
						}
						else
						{
							//HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(AFX_IDC_VSPLITBAR), RT_GROUP_CURSOR);
							//HCURSOR hCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_VSPLITBAR));
							//if (hCursor != NULL)
							//{
							//	SetCursor(hCursor);
							//}
							//else
							{
								SetCursor(LoadCursor(NULL, IDC_SIZENS));
							}
						}
					}
					else
					{
						_ASSERT(FALSE);
					}

					return TRUE;
				}

				return 0;
			}

			case SPB_SET_CURSOR:
				if (wParam)
				{
					pSPBar->m_hHCursor = (HCURSOR)lParam;
				}
				else
				{
					pSPBar->m_hVCursor = (HCURSOR)lParam;
				}
				return 0;

			case SPB_GET_CURSOR:
				if (wParam)
				{
					return (LRESULT)pSPBar->m_hHCursor;
				}
				else
				{
					return (LRESULT)pSPBar->m_hVCursor;
				}
				return NULL;

			case SPB_SET_ORIENTATION:
				pSPBar->m_eOrientation = (SPLITTER_ORIENTATION)lParam;
				return 0;

			case SPB_GET_ORIENTATION:
				return pSPBar->m_eOrientation;

			case SPB_ADD_PANEL:
				pSPBar->m_hPanel1 = (HWLWND)wParam;
				pSPBar->m_hPanel2 = (HWLWND)lParam;
				return 0;

			case SPB_GET_PANEL:
				if (wParam)
				{
					return (LRESULT)pSPBar->m_hPanel1;
				}
				else
				{
					return (LRESULT)pSPBar->m_hPanel2;
				}
				return NULL;

			case SPB_REPLACE_PANEL:
				if (wParam)
				{
					HWLWND hOld = pSPBar->m_hPanel1;
					pSPBar->m_hPanel1 = (HWLWND)lParam;
					return (LRESULT)hOld;
				}
				else
				{
					HWLWND hOld = pSPBar->m_hPanel2;
					pSPBar->m_hPanel2 = (HWLWND)lParam;
					return (LRESULT)hOld;
				}
				return NULL;

			case SPB_START_TRACKING:
				return StartTracking(pSPBar);

			case SPB_STOP_TRACKING:
				return StopTracking(pSPBar, lParam);

			case SPB_SET_OFFSET:
			{
				if (pSPBar->m_eOrientation == SO_NS)
				{
					pSPBar->m_ptTrackOffset.y = (LONG)lParam;
				}
				else if (pSPBar->m_eOrientation == SO_WE)
				{
					pSPBar->m_ptTrackOffset.x = (LONG)lParam;
				}
			}
			return 0;

			default:
				break;
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

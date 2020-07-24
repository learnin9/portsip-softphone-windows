#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


BOOL _AfxIsComboBoxControl(HWND hWnd, UINT nStyle)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}
	// do cheap style compare first
	if ((UINT)(::GetWindowLong(hWnd, GWL_STYLE) & 0x0F) != nStyle)
	{
		return FALSE;
	}

	// do expensive classname compare next
	TCHAR szCompare[_countof("combobox") + 1];
	::GetClassName(hWnd, szCompare, _countof(szCompare));
	return lstrcmpi(szCompare, _T("combobox")) == 0;
}

HWND _AfxChildWindowFromPoint(HWND hWnd, POINT pt)
{
	_ASSERT(hWnd != NULL);

	// check child windows
	::ClientToScreen(hWnd, &pt);
	HWND hWndChild = ::GetWindow(hWnd, GW_CHILD);
	for (; hWndChild != NULL; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
	{
		if (GetDlgCtrlID(hWndChild) != (WORD)(-1)
			&& (::GetWindowLong(hWndChild, GWL_STYLE) & WS_VISIBLE))
		{
			// see if point hits the child window
			CIUIRect rect;
			::GetWindowRect(hWndChild, rect);
			if (rect.PtInRect(pt))
			{
				return hWndChild;
			}
		}
	}

	return NULL;    // not found
}

//HINSTANCE AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType)
//{
//	_ASSERT(lpszName != NULL);
//	_ASSERT(lpszType != NULL);
//
//	HINSTANCE hInst;
//
//	// first check the main module state
//	AFX_MODULE_STATE *pModuleState = AfxGetModuleState();
//	if (!pModuleState->m_bSystem)
//	{
//		hInst = AfxGetResourceHandle();
//		if (::FindResource(hInst, lpszName, lpszType) != NULL)
//		{
//			return hInst;
//		}
//	}
//
//	// check for non-system DLLs in proper order
//	AfxLockGlobals(CRIT_DYNLINKLIST);
//	for (CDynLinkLibrary *pDLL = pModuleState->m_libraryList; pDLL != NULL;
//		pDLL = pDLL->m_pNextDLL)
//	{
//		if (!pDLL->m_bSystem && pDLL->m_hResource != NULL &&
//			::FindResource(pDLL->m_hResource, lpszName, lpszType) != NULL)
//		{
//			// found it in a DLL
//			AfxUnlockGlobals(CRIT_DYNLINKLIST);
//			return pDLL->m_hResource;
//		}
//	}
//	AfxUnlockGlobals(CRIT_DYNLINKLIST);
//
//	// check language specific resource next
//	hInst = pModuleState->m_appLangDLL;
//	if (hInst != NULL && ::FindResource(hInst, lpszName, lpszType) != NULL)
//	{
//		return hInst;
//	}
//
//	// check the main system module state
//	if (pModuleState->m_bSystem)
//	{
//		hInst = AfxGetResourceHandle();
//		if (::FindResource(hInst, lpszName, lpszType) != NULL)
//		{
//			return hInst;
//		}
//	}
//
//	// check for system DLLs in proper order
//	AfxLockGlobals(CRIT_DYNLINKLIST);
//	for (pDLL = pModuleState->m_libraryList; pDLL != NULL; pDLL = pDLL->m_pNextDLL)
//	{
//		if (pDLL->m_bSystem && pDLL->m_hResource != NULL &&
//			::FindResource(pDLL->m_hResource, lpszName, lpszType) != NULL)
//		{
//			// found it in a DLL
//			AfxUnlockGlobals(CRIT_DYNLINKLIST);
//			return pDLL->m_hResource;
//		}
//	}
//	AfxUnlockGlobals(CRIT_DYNLINKLIST);
//
//	// if failed to find resource, return application resource
//	return AfxGetResourceHandle();
//}

ToolTipCtrl::ToolTipCtrl()
{
	m_hWnd = NULL;
}

BOOL ToolTipCtrl::Create(HWND hParentWnd, const CIUIProject *pProject, DWORD dwStyle)
{
	if (hParentWnd == NULL)
	{
		return FALSE;
	}

	// 调用InitCommonControlsEx，以便MASM能够程序和comctl32.dll连接
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	// initialize common controls
	InitToolTipsClass(NULL);

	// allow modification of several common create parameters
	CREATESTRUCT cs;
	cs.dwExStyle = 0;
	cs.lpszClass = IUI_TOOLTIPS;
	cs.lpszName = NULL;
	cs.style = dwStyle;
	cs.x = CW_USEDEFAULT;
	cs.y = CW_USEDEFAULT;
	cs.cx = CW_USEDEFAULT;
	cs.cy = CW_USEDEFAULT;
	cs.hwndParent = hParentWnd;
	cs.hMenu = 0;
	cs.hInstance = IUIGetInstanceHandle();
	cs.lpCreateParams = (LPVOID)pProject;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		return FALSE;
	}

	// 创建虚拟根控件
	m_hVirtualRoot = CreateRoot();

	HookWindowCreate(this);

	m_hWnd = ::CreateWindowEx(NULL, IUI_TOOLTIPS, NULL,
			WS_POPUP /*| TTS_NOPREFIX | TTS_ALWAYSTIP*/ | dwStyle, // force WS_POPUP
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hParentWnd, NULL, NULL, (LPVOID)pProject);

	if (!UnhookWindowCreate())
	{
		PostNcDestroy();    // cleanup if CreateWindowEx fails too soon
	}

	if (m_hWnd == NULL)
	{
		return FALSE;
	}

	m_hWndOwner = hParentWnd;

	return TRUE;
}

ToolTipCtrl::~ToolTipCtrl()
{
	::DestroyWindow(m_hWnd);
}

BOOL ToolTipCtrl::DestroyToolTipCtrl()
{
#ifdef _AFXDLL
	BOOL bDestroy = (AfxGetModuleState() == m_pModuleState);
#else
	BOOL bDestroy = TRUE;
#endif

	if (bDestroy)
	{
		::DestroyWindow(m_hWnd);
		delete this;
	}
	return bDestroy;
}

BOOL ToolTipCtrl::AddTool(HWND hWnd, LPCTSTR lpszText, LPCRECT lpRectTool,
	UINT nIDTool)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);
	_ASSERT(lpszText != NULL);
	// the toolrect and toolid must both be zero or both valid
	_ASSERT((lpRectTool != NULL && nIDTool != 0) ||
		(lpRectTool == NULL) && (nIDTool == 0));

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	if (lpRectTool != NULL)
	{
		memcpy(&ti.rect, lpRectTool, sizeof(RECT));
	}
	else
	{
		RECT rect;
		::GetClientRect(hWnd, &rect);
		ti.rect = rect;
	}
	ti.lpszText = (LPTSTR)lpszText;
	return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL ToolTipCtrl::AddTool(HWND hWnd, UINT nIDText, LPCRECT lpRectTool, UINT nIDTool)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(nIDText != 0);
	_ASSERT(hWnd != NULL);
	// the toolrect and toolid must both be zero or both valid
	_ASSERT((lpRectTool != NULL && nIDTool != 0) ||
		(lpRectTool == NULL) && (nIDTool == 0));

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	if (lpRectTool != NULL)
	{
		memcpy(&ti.rect, lpRectTool, sizeof(RECT));
	}
	ti.hinst = NULL;// TODO:AfxFindResourceHandle(MAKEINTRESOURCE((nIDText >> 4) + 1), RT_STRING);
	_ASSERT(ti.hinst != NULL);
	ti.lpszText = (LPTSTR)MAKEINTRESOURCE(nIDText);
	return (BOOL) ::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL ToolTipCtrl::AddTool(
	__in HWND hWnd,
	__in const CToolTipCtrlProp *pToolTipCtrlProp,
	__in LPCTSTR lpszText/* = LPSTR_TEXTCALLBACK*/,
	__in LPCRECT lpRectTool,
	__in UINT_PTR nIDTool
)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);
	_ASSERT(pToolTipCtrlProp != NULL);
	_ASSERT(lpszText != NULL);
	// the toolrect and toolid must both be zero or both valid
	_ASSERT((lpRectTool != NULL && nIDTool != 0) ||
		(lpRectTool == NULL) && (nIDTool == 0));

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	if (lpRectTool != NULL)
	{
		memcpy(&ti.rect, lpRectTool, sizeof(RECT));
	}
	else
	{
		RECT rect;
		::GetClientRect(hWnd, &rect);
		ti.rect = rect;
	}
	ti.lpszText = (LPTSTR)lpszText;
	ti.lParam = (LPARAM)pToolTipCtrlProp;
	return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

BOOL ToolTipCtrl::AddTool(
	__in HWND hWnd,
	__in const CToolTipCtrlProp *pToolTipCtrlProp,
	__in LPCRECT lpRectTool/* = NULL*/,
	__in UINT_PTR nIDTool/* = 0*/)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);
	_ASSERT(pToolTipCtrlProp != NULL);
	// the tool rect and tool id must both be zero or both valid
	_ASSERT((lpRectTool != NULL && nIDTool != 0) ||
		(lpRectTool == NULL) && (nIDTool == 0));

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	if (lpRectTool != NULL)
	{
		memcpy(&ti.rect, lpRectTool, sizeof(RECT));
	}
	else
	{
		RECT rect;
		::GetClientRect(hWnd, &rect);
		ti.rect = rect;
	}
	ti.lParam = (LPARAM)pToolTipCtrlProp;
	return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, TRUE, (LPARAM)&ti);
}

void ToolTipCtrl::DelTool(HWND hWnd, UINT nIDTool)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)&ti);
}

void ToolTipCtrl::SetDelayTime(UINT nDelay)
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_SETDELAYTIME, 0, nDelay);
}
int ToolTipCtrl::GetDelayTime(DWORD dwDuration) const
{
	_ASSERT(::IsWindow(m_hWnd));
	return (int) ::SendMessage(m_hWnd, TTM_GETDELAYTIME, dwDuration, 0L);
}
void ToolTipCtrl::GetMargin(LPRECT lprc) const
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_GETMARGIN, 0, (LPARAM)lprc);
}
int ToolTipCtrl::GetMaxTipWidth() const
{
	_ASSERT(::IsWindow(m_hWnd));
	return (int) ::SendMessage(m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0L);
}
COLORREF ToolTipCtrl::GetTipBkColor() const
{
	_ASSERT(::IsWindow(m_hWnd));
	return (COLORREF) ::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0L);
}
COLORREF ToolTipCtrl::GetTipTextColor() const
{
	_ASSERT(::IsWindow(m_hWnd));
	return (COLORREF) ::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0L);
}
void ToolTipCtrl::Pop()
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_POP, 0, 0L);
}
void ToolTipCtrl::SetDelayTime(DWORD dwDuration, int iTime)
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_SETDELAYTIME, dwDuration, MAKELPARAM(iTime, 0));
}
void ToolTipCtrl::SetMargin(LPRECT lprc)
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_SETMARGIN, 0, (LPARAM)lprc);
}
int ToolTipCtrl::SetMaxTipWidth(int iWidth)
{
	_ASSERT(::IsWindow(m_hWnd));
	return (int) ::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, iWidth);
}
void ToolTipCtrl::SetTipBkColor(COLORREF clr)
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_SETTIPBKCOLOR, clr, 0L);
}
void ToolTipCtrl::SetTipTextColor(COLORREF clr)
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_SETTIPTEXTCOLOR, clr, 0L);
}
void ToolTipCtrl::Update()
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_UPDATE, 0, 0L);
}
BOOL ToolTipCtrl::GetCurrentTool(LPTOOLINFO lpToolInfo) const
{
	_ASSERT(::IsWindow(m_hWnd));
	return (BOOL) ::SendMessage(m_hWnd, TTM_GETCURRENTTOOL, 0, (LPARAM)lpToolInfo);
}

#if _WIN32_IE >= 0x0500
CIUISize ToolTipCtrl::GetBubbleSize(LPTOOLINFO lpToolInfo) const
{
	_ASSERT(::IsWindow(m_hWnd));
	return CIUISize((DWORD)::SendMessage(m_hWnd, TTM_GETBUBBLESIZE, 0, (LPARAM)lpToolInfo));
}
BOOL ToolTipCtrl::AdjustRect(LPRECT lprc, BOOL bLarger /*= TRUE*/)
{
	_ASSERT(::IsWindow(m_hWnd));
	return (BOOL)::SendMessage(m_hWnd, TTM_ADJUSTRECT, bLarger, (LPARAM)lprc);
}
BOOL ToolTipCtrl::SetTitle(UINT uIcon, LPCTSTR lpstrTitle)
{
	_ASSERT(::IsWindow(m_hWnd));
	return (BOOL)::SendMessage(m_hWnd, TTM_SETTITLE, uIcon, (LPARAM)lpstrTitle);
}
#endif

#if (_WIN32_WINNT >= 0x0501) && defined(UNICODE)
void ToolTipCtrl::Popup()
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_POPUP, 0, 0L);
}
void ToolTipCtrl::GetTitle(PTTGETTITLE pTTGetTitle) const
{
	_ASSERT(::IsWindow(m_hWnd));
	::SendMessage(m_hWnd, TTM_GETTITLE, 0, (LPARAM)pTTGetTitle);
}
#endif

void ToolTipCtrl::GetText(CIUIString &str, HWND hWnd, UINT nIDTool) const
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	ti.lpszText = str.GetBuffer(256);
	::SendMessage(m_hWnd, TTM_GETTEXT, 0, (LPARAM)&ti);
	str.ReleaseBuffer();
}

BOOL ToolTipCtrl::GetToolInfo(CToolInfo &ToolInfo, HWND hWnd,
	UINT nIDTool) const
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);

	FillInToolInfo(ToolInfo, hWnd, nIDTool);
	ToolInfo.lpszText = ToolInfo.szText;
	return (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)&ToolInfo);
}

BOOL ToolTipCtrl::HitTest(HWND hWnd, CIUIPoint pt, LPTOOLINFO lpToolInfo) const
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);
	_ASSERT(lpToolInfo != NULL);

	TTHITTESTINFO hti;
	memset(&hti, 0, sizeof(hti));
	hti.ti.cbSize = sizeof(TTTOOLINFO); // TODO: Modified!
	hti.hwnd = hWnd;
	hti.pt.x = pt.x;
	hti.pt.y = pt.y;
	if ((BOOL)::SendMessage(m_hWnd, TTM_HITTEST, 0, (LPARAM)&hti))
	{
		memcpy(lpToolInfo, &hti.ti, sizeof(TTTOOLINFO));
		return TRUE;
	}

	return FALSE;
}

void ToolTipCtrl::SetToolRect(HWND hWnd, UINT nIDTool, LPCRECT lpRect)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);
	_ASSERT(nIDTool != 0);

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	memcpy(&ti.rect, lpRect, sizeof(RECT));
	::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
}

void ToolTipCtrl::UpdateTipText(LPCTSTR lpszText, HWND hWnd, UINT nIDTool)
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT(hWnd != NULL);

	TOOLINFO ti;
	FillInToolInfo(ti, hWnd, nIDTool);
	ti.lpszText = (LPTSTR)lpszText;
	::SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
}

void ToolTipCtrl::UpdateTipText(UINT nIDText, HWND hWnd, UINT nIDTool)
{
	_ASSERT(nIDText != 0);

	CIUIString str;
	str.LoadString(nIDText);
	UpdateTipText(str, hWnd, nIDTool);
}

/////////////////////////////////////////////////////////////////////////////
// ToolTipCtrl Implementation

// 与CToolTipCtrl::FillInToolInfo实现不同
void ToolTipCtrl::FillInToolInfo(TOOLINFO &ti, HWND hWnd, UINT nIDTool) const
{
	memset(&ti, 0, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);

	// 如果nIDTool为0，表示“工具”为“全局工具”
	if (nIDTool == 0)
	{
		// ti.hwnd表示通知窗口
		ti.hwnd = hWnd;
		// TTF_SUBCLASS标志很重要，它可以让Tooltips控件子类化目标窗口
		// 然后自动处理目标窗口的鼠标消息，并转发给Tooltips处理，否则
		// 用户需要自己在目标窗口的PreTranslateMessage中处理鼠标消息，并且
		// 向Tooltips控件发送TTM_RELAYEVENT消息。
		ti.uFlags = TTF_SUBCLASS;
		ti.uId = 0;
	}
	else
	{
		// ti.hwnd表示“工具栏”窗口
		ti.hwnd = hWnd;
		ti.uFlags = TTF_SUBCLASS;
		ti.uId = nIDTool;
	}
}

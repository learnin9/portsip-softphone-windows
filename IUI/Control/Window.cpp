#include "StdAfx.h"

#ifdef _DEBUG
#pragma comment(lib, "shlwapi.lib")
#endif

#if(_MSC_VER <= 1200) // VC6.0
#include <ATLBASE.H>
#endif

#include <list>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


BOOL IUIInternalPreTranslateMessage(MSG *pMsg)
{
	//	ASSERT_VALID(this);

	// 	CIUIWinThread *pThread = IUIGetThread();
	// 	if (pThread)
	// 	{
	// 		// if this is a thread-message, short-circuit this function
	// 		if (pMsg->hwnd == NULL && pThread->DispatchThreadMessageEx(pMsg))
	// 			return TRUE;
	// 	}
	//
	// walk from target to main window
	Window *pMainWnd = IUIGetMainWnd();
	if (Window::WalkPreTranslateTree(pMainWnd->GetSafeHwnd(), pMsg))
	{
		return TRUE;
	}

	return FALSE;   // no special processing
}

BOOL IUI::IUIPreTranslateMessage(MSG *pMsg)
{
	CIUIWinThread *pThread = IUIGetThread();
	if (pThread)
	{
		return pThread->PreTranslateMessage(pMsg);
	}
	else
	{
		return IUIInternalPreTranslateMessage(pMsg);
	}
}


CWndHandler::CWndHandler()
	: m_pBindWindow(NULL)
	, m_nMsgHandlerCount(0)
	, m_lParam(0)
{
}

CWndHandler::~CWndHandler()
{
}

int CWndHandler::SetBindWindow(Window *pBindWindow)
{
	m_pBindWindow = pBindWindow;

	return 0;
}

Window *CWndHandler::GetBindWindow()
{
	return m_pBindWindow;
}

const Window *CWndHandler::GetBindWindow() const
{
	return m_pBindWindow;
}

LPARAM CWndHandler::SetUserData(LPARAM lParam)
{
	LPARAM lOldParam = m_lParam;
	m_lParam = lParam;
	return lOldParam;
}

LPARAM CWndHandler::GetUserData() const
{
	return m_lParam;
}

LRESULT CWndHandler::WindowProc(Window *pThis, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//const IUI_MSGHANDLER_MAP *pMsgMap = GetMessageMap();
	//if (pMsgMap == NULL)
	//{
	//	// Must return 0 to continue message
	//	return 0;
	//}


	//// Use the last setting handler in handler map,
	//for (int i = m_nMsgHandlerCount - 1;
	//	i >= 0 && i < m_nMsgHandlerCount;
	//	--i)
	//{
	//	const IUI_MSGHANDLER_MAP *pMsg = pMsgMap + i;
	//	if (pMsg->uMsg != uMsg)
	//	{
	//		continue;
	//	}

	//	IUI_PMSGHANDLER handler = pMsg->pfn;
	//	if (handler == NULL)
	//	{
	//		continue;
	//	}

	//	if (pMsg->uMsg == 0)
	//	{
	//		continue;
	//	}

	//	// General message
	//	if (handler != NULL)
	//	{
	//		return (int)(this->*handler)(pThis, wParam, lParam);
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}

	return  0;
}

//////////////////////////////////////////////////////////////////////////
///
Window::Window()
	: m_hWnd(NULL)
	, m_hWndOwner(NULL)
	, m_pfnSuper(NULL)
	, m_hResInstance(NULL)
	, m_strClassName(IUI_WINDOW)
	, m_uIDD(0)
	, m_pBindWindowProp(NULL)
	, m_hVirtualRoot(NULL)
	, m_hFocus(NULL)
	, m_eHAlign(CLH_LEFT)
	, m_eVAlign(CLV_TOP)
	, m_bUseMaskColor(FALSE)
	, m_bAlpha(FALSE)
	, m_crMask(RGB(255, 0, 255))
	, m_btAlphaValue(255)
	, m_bDrawBkColor(false)
	, m_crBk(RGB(255, 255, 255))
	, m_hImageBk(NULL)
	, m_eImageResizeMode(IRM_9GRID)
	, m_bDrawBorderColor(false)
	, m_crBorderColor(RGB(0, 0, 0))
	, m_nTitleBarHeight(24)
	, m_bCreateAsDialog(false)
	, m_pWndHandler(NULL)
	, m_bDragResize(FALSE)
	, m_bDragResizing(FALSE)
	, m_nHitTestRet(HTCLIENT)
	, m_nCXBorder(4)
	, m_nCYBorder(4)
	, m_hShadowWnd(NULL)
{
	int cxEdge = GetSystemMetrics(SM_CXEDGE);
	int cyEdge = GetSystemMetrics(SM_CXEDGE);
	int cxDlgFrame = GetSystemMetrics(SM_CXDLGFRAME);
	int cyDlgFrame = GetSystemMetrics(SM_CYDLGFRAME);
	int cx = 2 * cxEdge + cxDlgFrame;
	int cy = 2 * cyEdge + cyDlgFrame;
	m_rcTrueBorder.SetRect(cx, cy, cx, cy);

	m_WindowHelper.m_pThis = this;
	m_sizeScroll = CIUISize(16, 16);
}

Window::~Window()
{
	ReleaseIUIImage(m_hImageBk);
	m_hImageBk = NULL;
}

HWND Window::GetHWND() const
{
	if (this == NULL)
	{
		return NULL;
	}

	return m_hWnd;
}

HWND Window::GetSafeHwnd() const
{
	if (this == NULL)
	{
		return NULL;
	}

	return m_hWnd;
}

int Window::SetWindowClassName(LPCTSTR lpszClassName)
{
	m_strClassName = lpszClassName;

	return 0;
}

CIUIString Window::GetWindowClassName() const
{
	return m_strClassName;
}

UINT Window::GetClassStyle() const
{
	return CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
}

int Window::SetProject(CIUIProject *pProject)
{
	// 对于普通IUI，在创建之前，就需要“工程”，但对于ToolTips窗口，如果
	// “工具”不绑定CToolTipCtrlProp，创建之前是不需要“工程”的
	IUIGetModuleState()->m_pProject = pProject;

	return 0;
}

CIUIProject *Window::GetProject()
{
	return (CIUIProject *)::SendMessage(m_hWnd, WM_GET_PROJECT, 0, 0);
}

const CIUIProject *Window::GetProject() const
{
	return (const CIUIProject *)((Window *)this)->SendMessage(WM_GET_PROJECT, 0, 0);
}

Window::operator HWND() const
{
	return m_hWnd;
}

int Window::SetXml(LPCTSTR lpszXml)
{
	m_strXml = lpszXml;

	return 0;
}

CIUIString Window::GetXml() const
{
	return m_strXml;
}

BOOL Window::Create(
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT &rect,
	HWND hwndParent,
	UINT nID,
	LPVOID pContext/* = NULL*/)
{
	// can't use for desktop or pop-up windows (use CreateEx instead)
	_ASSERT(hwndParent != NULL);
	_ASSERT((dwStyle & WS_POPUP) == 0);

	return CreateEx(0, lpszClassName, lpszWindowName,
			dwStyle | WS_CHILD,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top,
			hwndParent, (HMENU)nID, (LPVOID)pContext);
}

BOOL Window::CreateEx(
	DWORD dwExStyle,
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	const RECT &rect,
	Window *pParentWnd,
	UINT nID,
	LPVOID lpParam/* = NULL */)
{
	return CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle,
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			pParentWnd->GetSafeHwnd(), (HMENU)nID, lpParam);
}

HWLWND IUI::CreateRoot()
{
	HWLWND hRootWLWnd = new WLWND;
	CWLWnd *pRoot = new CWLWnd;

	hRootWLWnd->ci.pThis = pRoot;
	hRootWLWnd->m_uID = UINT(-1); // 把m_uID设置成-1，表示它是虚拟根控件

	pRoot->m_hWnd = hRootWLWnd;

	return hRootWLWnd;
}

int IUI::ReleaseRoot(HWLWND hRoot)
{
	if (hRoot == NULL)
	{
		return -1;
	}

	// 释放里面的子HWLWND
	HWLWND hWalk = hRoot->spwndChild;
	while (NULL != hWalk)
	{
		// 记录下一个节点
		HWLWND hNext = hWalk->spwndNext;

		IUI::DestroyWindow(hWalk);

		hWalk = hNext;
	}

	delete hRoot;

	return 0;
}

BOOL Window::CreateEx(
	DWORD dwExStyle,
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU nIDorHMenu,
	LPVOID lpParam)
{
	if (IUIGetModuleState()->m_pProject == NULL)
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	m_strClassName = lpszClassName;

	if (!RegisterWindowClass())
	{
		return NULL;
	}

	// allow modification of several common create parameters
	CREATESTRUCT cs;
	cs.dwExStyle = dwExStyle;
	cs.lpszClass = GetWindowClassName();
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle;
	cs.x = x;
	cs.y = y;
	cs.cx = nWidth;
	cs.cy = nHeight;
	cs.hwndParent = hWndParent;
	cs.hMenu = nIDorHMenu;
	cs.hInstance = IUIGetInstanceHandle();
	cs.lpCreateParams = lpParam;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		return FALSE;
	}

	// 创建虚拟根控件
	m_hVirtualRoot = CreateRoot();

	HookWindowCreate(this);
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, GetWindowClassName(),
			cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
			cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

#ifdef _DEBUG
	if (hWnd == NULL)
	{
		IUITRACE(_T("Warning: Window creation failed: GetLastError returns 0x%8.8X\n"),
			GetLastError());
	}
	else
	{
		IUITRACE(_T("[IUI]=== Window %08x creation successed\r\n"), hWnd);
	}
#endif

	if (!UnhookWindowCreate())
	{
		PostNcDestroy();    // cleanup if CreateWindowEx fails too soon
	}

	if (hWnd == NULL)
	{
		return FALSE;
	}

	_ASSERT(hWnd == m_hWnd); // should have been set in send msg hook

	// 有可能Hook函数没执行，导致虽然hWnd被创建出来，但未赋值给m_hWnd
	if (m_hWnd == NULL)
	{
		::DestroyWindow(hWnd);
	}

	return (m_hWnd != NULL);
}

BOOL Window::Create(LPCTSTR lpszXMLTemplateName, HWND hParentWnd)
{
	CUIParse uiParse;
	uiParse.SetProject(IUIGetModuleState()->m_pProject);
	uiParse.SetUIPath(lpszXMLTemplateName);
	CWindowProp *pUIWndPorp = uiParse.GetWindowProp();
	if (pUIWndPorp == NULL)
	{
		return FALSE;
	}

	pUIWndPorp->SetFileName(lpszXMLTemplateName);

	return CreateChildIndirect(pUIWndPorp, hParentWnd);
}

// Create child window
BOOL Window::CreateChildIndirect(CWindowProp *pWindowPorp, HWND hParentWnd)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (pWindowPorp->m_bClipSiblings)
	{
		dwStyle |= WS_CLIPSIBLINGS;
	}

	BOOL bRet = Create(GetWindowClassName(), NULL,
			dwStyle, CIUIRect(0, 0, 1, 1), hParentWnd, 1);

	if (!bRet)
	{
		return FALSE;
	}

	m_WindowHelper.InitWindow(m_hWnd, pWindowPorp);

	return TRUE;
}

// Create Popup window
BOOL Window::CreatePopupWindow(LPCTSTR lpszXMLTemplateName, HWND hParentWnd)
{
	CUIParse uiParse;
	uiParse.SetProject(IUIGetModuleState()->m_pProject);
	int nRet = uiParse.SetUIPath(lpszXMLTemplateName);
	if (nRet != 0)
	{
		return FALSE;
	}

	CWindowProp *pUIWndPorp = uiParse.GetWindowProp();
	if (pUIWndPorp == NULL)
	{
		return FALSE;
	}

	pUIWndPorp->SetFileName(lpszXMLTemplateName);

	return CreatePopupIndirect(pUIWndPorp, hParentWnd);
}

BOOL Window::CreatePopupIndirect(CWindowProp *pWindowProp, HWND hParentWnd)
{
	if (pWindowProp == NULL)
	{
		return FALSE;
	}

	if (::IsWindow(m_hWnd))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	// 临时赋值
	m_pBindWindowProp = pWindowProp;

	CIUIRect rect = pWindowProp->m_rcControl;

	DWORD dwStyle = WS_POPUP | WS_MINIMIZEBOX | GetControlBaseStyle(pWindowProp);
	if ((bool)m_pBindWindowProp->m_bResizingBorder)
	{
		if ((bool)m_pBindWindowProp->m_bTrueResizingBorder)
		{
			dwStyle |= WS_THICKFRAME;
		}
	}

	DWORD dwExStyle = 0;
	if ((bool)pWindowProp->m_bLayeredWindow)
	{
		dwExStyle |= WS_EX_LAYERED;
	}
	if ((bool)pWindowProp->m_bAppWindow)
	{
		dwExStyle |= WS_EX_APPWINDOW;
	}
	if ((bool)pWindowProp->m_bToolWindow)
	{
		dwExStyle |= WS_EX_TOOLWINDOW;
	}
	if ((bool)pWindowProp->m_bNoActivate)
	{
		dwExStyle |= WS_EX_NOACTIVATE;
	}
	if ((bool)pWindowProp->m_bTopmost)
	{
		dwExStyle |= WS_EX_TOPMOST;
	}
	if ((bool)pWindowProp->m_bAcceptFiles)
	{
		dwExStyle |= WS_EX_ACCEPTFILES;
	}

	BOOL bVisible = IsIncludeFlag(dwStyle, WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	BOOL bRet = CreateEx(dwExStyle, GetWindowClassName(), NULL, dwStyle,
			rect.left, rect.top, rect.Width(), rect.Height(), hParentWnd, 0);
	if (!bRet)
	{
		return FALSE;
	}

	m_WindowHelper.InitWindow(m_hWnd, pWindowProp);

	::MoveWindow(m_hWnd, rect.left, rect.top, rect.Width(), rect.Height(), FALSE);

	// 发送WM_INITDIALOG消息
	::SendMessage(m_hWnd, WM_INITDIALOG, 0, 0);

	// 创建阴影窗口
	CreateShadowWindow(pWindowProp, this);

	// Win32中，对话框在收到WM_INITDIALOG时，窗口也是不可见的。
	// 注意：::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0)会使窗口可见，
	// 如果在执行到这里之前，已经发送过带TRUE参数的WM_SETREDRAW消息，则窗口可能已经显示出来了。
	if (bVisible)
	{
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	return TRUE;
}

BOOL Window::PreCreateWindow(CREATESTRUCT &cs)
{
	if ((cs.hMenu == NULL) && (cs.style & WS_CHILD))
	{
		cs.hMenu = (HMENU)(UINT_PTR)this;
	}

	if (m_pBindWindowProp != NULL)
	{
		if (m_pBindWindowProp->m_bClipChildren)
		{
			cs.style |= WS_CLIPCHILDREN;
		}
		if (m_pBindWindowProp->m_bClipSiblings)
		{
			cs.style |= WS_CLIPSIBLINGS;
		}
		if (m_pBindWindowProp->m_bTabStop)
		{
			cs.style |= WS_TABSTOP;
		}

#if(_WIN32_WINNT >= 0x0500)
		if (m_pBindWindowProp->m_bNoActivate)
		{
			cs.dwExStyle |= WS_EX_NOACTIVATE;
		}
#endif /* _WIN32_WINNT >= 0x0500 */
		if (m_pBindWindowProp->m_bAppWindow)
		{
			cs.dwExStyle |= WS_EX_APPWINDOW;
		}
		if (m_pBindWindowProp->m_bTopmost)
		{
			cs.dwExStyle |= WS_EX_TOPMOST;
		}
		if (m_pBindWindowProp->m_bAcceptFiles)
		{
			cs.dwExStyle |= WS_EX_ACCEPTFILES;
		}
		if (m_pBindWindowProp->m_bToolWindow)
		{
			cs.dwExStyle |= WS_EX_TOOLWINDOW;
		}
	}

	return TRUE;
}

void Window::PostNcDestroy()
{
	// default to nothing
	ReleaseRoot(m_hVirtualRoot);
	m_hVirtualRoot = NULL;
}

WNDPROC *Window::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	return &m_pfnSuper;
}

HWND Window::PreModal()
{
	// cannot call DoModal on a dialog already constructed as modeless
	_ASSERT(m_hWnd == NULL);

	// find parent HWND
	HWND hParent = ::GetParent(m_hWnd);

	if (hParent == NULL)
	{
		// 如果父窗口为NULL，使用主窗口作为父窗口
		hParent = IUIGetMainWnd()->GetSafeHwnd();
	}
	else
	{
		// 如果父窗口不为NULL，向上查找父窗口的顶级窗口。

		// a popup window cannot be owned by a child window
		while (true)
		{
			LONG lStyle = ::GetWindowLong(hParent, GWL_STYLE);

			if ((lStyle & WS_CHILD) == WS_CHILD)
			{
				hParent = ::GetParent(hParent);
			}
			else
			{
				break;
			}
		}

		_ASSERT(hParent != NULL);
	}

	// hook for creation of dialog
	//HookWindowCreate(this);

	// return window to use as parent for dialog
	return hParent;
}

void Window::PostModal()
{
	//AfxUnhookWindowCreate();   // just in case
	Detach();               // just in case

	//// re-enable windows
	//if (::IsWindow(m_hWndTop))
	//{
	//	::EnableWindow(m_hWndTop, TRUE);
	//}
	//m_hWndTop = NULL;
}

INT_PTR Window::DoModal()
{
	HWND hWndParent = PreModal();

	// 如果要使用鼠标缩放窗口或移动窗口，必须加上WS_THICKFRAME风格。
	BOOL bRet = CreatePopupWindow(m_strXml, hWndParent);
	if (!bRet)
	{
		return 0;
	}

	m_bCreateAsDialog = true;
	UINT uRet = ShowModal();

	return uRet;
}

void Window::EndDialog(int nRetCode)
{
	_ASSERT(::IsWindow(m_hWnd));

	if (!::IsWindow(m_hWnd))
	{
		return;
	}

	// 这里换成SendMessage后，不稳定。用Esc键退出对话框，再打开对话框，再用Esc键退出，反复几次后，界面就显示不出来了。
	::PostMessage(m_hWnd, WM_CLOSE, (WPARAM)nRetCode, 0);
}

int Window::BindStyle(const CWindowProp *pWindowProp)
{
	return Window_BindStyle(m_hWnd, pWindowProp);
}

BOOL Window::ShowWindow(int nCmdShow)
{
	if (!::IsWindow(m_hWnd))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return ::ShowWindow(m_hWnd, nCmdShow);
}

UINT Window::ShowModal()
{
	_ASSERT(::IsWindow(m_hWnd));
	UINT nRet = 0;
	HWND hWndParent = ::GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	if (IsUpdateLayeredWindow(m_hWnd))
	{
		Invalidate();
	}

	MSG msg = { 0 };

	while (::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message != WM_KICKIDLE && !IUI::IUIPreTranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd)
			{
				nRet = msg.wParam;
				::EnableWindow(hWndParent, TRUE);
				::SetFocus(hWndParent);
			}
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
	}

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);

	if (msg.message == WM_QUIT)
	{
		::PostQuitMessage(msg.wParam);
	}

	return nRet;
}

void Window::Close(UINT nRet)
{
	_ASSERT(::IsWindow(m_hWnd));

	if (!::IsWindow(m_hWnd))
	{
		return;
	}

	PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
}

void Window::CenterWindow()
{
	_ASSERT(::IsWindow(m_hWnd));
	_ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD) == 0);
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd = *this;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);

	if (hWndCenter != NULL)
	{
		hWnd = hWndCenter;
	}

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = { 0 };
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if (hWndCenter == NULL)
	{
		rcCenter = rcArea;
	}
	else
	{
		::GetWindowRect(hWndCenter, &rcCenter);
	}

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if (xLeft < rcArea.left)
	{
		xLeft = rcArea.left;
	}
	else if (xLeft + DlgWidth > rcArea.right)
	{
		xLeft = rcArea.right - DlgWidth;
	}

	if (yTop < rcArea.top)
	{
		yTop = rcArea.top;
	}
	else if (yTop + DlgHeight > rcArea.bottom)
	{
		yTop = rcArea.bottom - DlgHeight;
	}

	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void Window::SetResInstance(HINSTANCE hInst)
{
	m_hResInstance = hInst;
}

void Window::SetIcon(UINT nRes)
{
	HICON hIcon = (HICON)::LoadImage(m_hResInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
			::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	_ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadImage(m_hResInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	_ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool Window::RegisterWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = GetClassStyle();
	wc.lpfnWndProc = IUIWndProcBase;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClass(&wc);
	_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

// Window窗口的窗口过程
LRESULT CALLBACK IUI::IUIWndProcBase(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!::IsWindow(hWnd))
	{
		return 0;
	}

	// MFC中，也是通过CWnd::FromHandlePermanent来得到HWND对应的CWnd *
	Window *pWnd = Window::FromHandlePermanent(hWnd);
	_ASSERT(pWnd != NULL);
	_ASSERT(pWnd->m_hWnd == hWnd);
	if (pWnd == NULL || pWnd->m_hWnd != hWnd)
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return pWnd->WindowProc(uMsg, wParam, lParam);
}

LRESULT Window::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;
	if (NULL == pe || pe->bHandled || !IsWindow(m_hWnd))
	{
		return pe->lResult;
	}

	if (wParam == UE_BUTTON_CLICK)
	{
		// 当普通按钮设置成最小化按钮或关闭按钮后，点击按钮后最小化和关闭窗口。
		if (GetClassName(pe->hOriginalSender) == IUI_BUTTON)
		{
			HWLWND hButton = pe->hOriginalSender;

			//
			// 如果按钮绑定了弹出式窗口，在这里弹出。
			//

			//
			BOOL bCheckBox = IsIncludeFlag(GetStyle(hButton), BS_AUTOCHECKBOX);
			BOOL bRadioButton = IsIncludeFlag(GetStyle(hButton), BS_AUTORADIOBUTTON);

			if (bCheckBox)
			{
				// 如果CheckBox是最大化按钮
				CHK_TYPE eCheckBoxType = (CHK_TYPE)IUI::SendMessage(hButton, BM_GET_BUTTON_TYPE, 0, 0);
				if (CT_MAXIMIZEBOX == eCheckBoxType)
				{
					if (IsZoomed(m_hWnd))
					{
						SendMessage(WM_SYSCOMMAND, SC_RESTORE);
					}
					else
					{
						if (!IsIconic(m_hWnd))
						{
							SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
						}
					}
				}
			}
			else if (bRadioButton)
			{
				// 如果Radio Button是Tab Control的标签。 则切换属性页
				LPCTSTR lpszBindTabControl = RadioButton_GetBindTabControl(hButton);

				if (!CIUIString(lpszBindTabControl).IsEmpty())
				{
					// call SwitchToTaskWindow for each CTaskWndMgr
					HWLWND hBindTabControl = FindControl(lpszBindTabControl);
					if (IsWindow(hBindTabControl))
					{
						if (GetClassName(hBindTabControl) == IUI_TABCONTROL)
						{
							// If the bind window not add to task window list, add it.
							CPropertySheetInfo *pInfo = TabControl_FindPropertySheet(hBindTabControl, hButton);
							if (pInfo == NULL)
							{
								LPCTSTR lpszBindPropertySheet = RadioButton_GetBindPropertySheet(hButton);
								CWindowProp *pBindUIWndProp = (CWindowProp *)IUIGetModuleState()->m_pProject->FindProjectItem(lpszBindPropertySheet);
								TabControl_AddPropertySheet(hBindTabControl, hButton,
									lpszBindPropertySheet, pInfo->m_pUserHandler);
							}

							// Switch to task window
							if (IsWindowVisible(hBindTabControl))
							{
								if (TabControl_FindPropertySheet(hBindTabControl, hButton) != NULL)
								{
									TabControl_SwitchPropertySheet(hBindTabControl, hButton);
								}
							}
						}
					}
				}
			}
			else
			{
				// 如果Push Button是最小化或关闭按钮
				BTN_TYPE eType = (BTN_TYPE)IUI::SendMessage(hButton, BM_GET_BUTTON_TYPE, 0, 0);
				int nID = GetDlgCtrlID(hButton);

				if (eType == BT_MINIMIZEBOX)
				{
					::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				}
				else if (eType == BT_CLOSEBOX)
				{
					if (m_bCreateAsDialog)
					{
						EndDialog(nID);
					}
					else
					{
						LRESULT lr = ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
						return lr;
					}
				}
			}
		}
	}
	else if (wParam == WM_DESTROY)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			// 当焦点控件被删除
			if (pe->hOriginalSender == m_hFocus)
			{
				m_hFocus = NULL;
				Invalidate();
			}
		}
	}

	return 0;
}

// ----------------------------------------------------------------------------
//
//  返回指定HWLWND下方与指定HWLWND最靠近的、带Tabstop风格的HWLWND(控制树的List顺序).
//
// ----------------------------------------------------------------------------

HWLWND HWLWND_GetNextCtrl(HWLWND hCtrl)
{
	if (NULL == hCtrl)
	{
		return NULL;
	}

	// 如果控件有孩子，则返回第一个孩子
	if (hCtrl->spwndChild != NULL)
	{
		return hCtrl->spwndChild;
	}

checkNext:
	// 如果控件有下一个兄弟控件，则返回下一个兄弟控件
	if (hCtrl->spwndNext != NULL)
	{
		return hCtrl->spwndNext;
	}

	// 向上沿线查找
	hCtrl = hCtrl->spwndParent;
	if (hCtrl != NULL)
	{
		goto checkNext;
	}

	return NULL;
}

BOOL IsTabStopCtrl(HWLWND hCtrl)
{
	BOOL bTabStop = FALSE;

	if (IsWindowVisible(hCtrl))
	{
		if (IsWindowEnabled(hCtrl))
		{
			if ((GetStyle(hCtrl) & WS_TABSTOP) == WS_TABSTOP)
			{
				bTabStop = TRUE;
			}
		}
	}

	return bTabStop;
}

HWLWND GetNextTabstopCtrl(HWLWND hCurCtrl)
{
	if (NULL == hCurCtrl)
	{
		return NULL;
	}

	HWLWND hNextTabstop = NULL;

	// 从当前控件向下循环
	HWLWND hWalk = hCurCtrl;
	while ((hWalk = HWLWND_GetNextCtrl(hWalk)) != NULL)
	{
		if (IsTabStopCtrl(hWalk))
		{
			hNextTabstop = hWalk;
			break;
		}
	}

	// 如果下面没有，从控件树开始继续遍历，直到自己
	if (NULL == hNextTabstop)
	{
		HWLWND hTopLevel = GetTopLevelWindow(hCurCtrl);

		HWLWND hWalk = hTopLevel;
		do
		{
			if (IsTabStopCtrl(hWalk))
			{
				hNextTabstop = hWalk;
				break;
			}

			if (hWalk == hCurCtrl)
			{
				break;
			}
		}
		while ((hWalk = HWLWND_GetNextCtrl(hWalk)) != NULL);
	}

	return hNextTabstop;
}

// 响应Tab按键
int Window::HandleTabKey(MSG *pMsg)
{
	// 	UIWNDMEMBER *pMember = (UIWNDMEMBER *)m_pMember;
	//
	HWLWND hCtrlFocus = GetWLFocus();
	if (hCtrlFocus == NULL)
	{
		return 1;    //IsDialogMessage(pMsg);
	}

	BOOL bCtrl = GetKeyState(VK_CONTROL) < 0;
	BOOL bShift = GetKeyState(VK_SHIFT) < 0;

	// TODO: 如果控件是Tab Control的标签，则处理Ctrl和Shift按键
	// Code


	HWLWND hNextTabstop = GetNextTabstopCtrl(hCtrlFocus);
	if (NULL != hNextTabstop)
	{
		IUI::SetFocus(m_hWnd, hNextTabstop);
		IUI::SendMessage(hNextTabstop, WM_GETDLGCODE, pMsg->wParam, LPARAM(pMsg));
	}

	//
	// 	//
	// 	// 1. If current focus control is a tab control in a CTaskWndMgr.
	// 	// press Ctrl+Tab switch next tab, press Ctrl+Shift+tab switch to prev tab, Press Tab switch to the first child of task window.
	// 	//
	//
	// 	// Find in all CTaskWndMgr
	// 	CTaskWndInfo TaskInfo;
	// 	CTaskWndMgr *pTaskWndMgr = FindTaskWindow(nFocusID, &TaskInfo);
	//
	// 	if (pTaskWndMgr != NULL)
	// 	{
	// 		// Loop between tabs
	// 		if (bCtrl)
	// 		{
	// 			if (bShift)
	// 			{
	// 				pTaskWndMgr->SwitchToPrevTaskWindow();
	// 			}
	// 			else
	// 			{
	// 				pTaskWndMgr->SwitchToNextTaskWindow();
	// 			}
	//
	// 			return 2;//CWnd::PreTranslateMessage(pMsg);
	// 		}
	// 		// Loop between controls
	// 		else
	// 		{
	// 			if (!bShift)
	// 			{
	// 				if (TaskInfo.m_pTaskWnd->GetSafeHwnd() == NULL)
	// 				{
	// 					return 1;    //IsDialogMessage(pMsg);
	// 				}
	//
	// 				CWnd *pFirstChild = TaskInfo.m_pTaskWnd->GetWindow(GW_CHILD);
	// 				if (pFirstChild->GetSafeHwnd() == NULL)
	// 				{
	// 					return 1;    //IsDialogMessage(pMsg);
	// 				}
	//
	// 				pFirstChild->SetFocus();
	// 				return 3;//TRUE;
	// 			}
	//
	// 			return 1;//IsDialogMessage(pMsg);
	// 		}
	// 	}
	//
	// 	//
	// 	// 2. If the focus control is last control of task window. Press tab key switch to next tab-stop control in parent of task window.
	// 	//
	// 	CWnd *pWndLastTabstop = GetLastTabstopCtrl(NULL);
	// 	if (pWndFocus->GetSafeHwnd() == pWndLastTabstop->GetSafeHwnd())
	// 	{
	// 		CTaskWndMgr *pTaskWndMgr = NULL;
	// 		BOOL bHasTaskWndParent = IsControlInRadioTaskWnd(pWndFocus, &pTaskWndMgr);
	// 		if (bHasTaskWndParent)
	// 		{
	// 			//pTaskWndMgr->GetParent()->GetDlgItem(pTaskWndMgr->GetCurrentTaskWindow())->SetFocus();
	// 			CWnd *pRadio = pTaskWndMgr->GetParent()->GetDlgItem(pTaskWndMgr->GetCurrentTaskWindow());
	// 			CWnd *pNextTabstop = pTaskWndMgr->GetParent()->GetNextDlgTabItem(pRadio, FALSE);
	// 			if (pNextTabstop != NULL)
	// 			{
	// 				pNextTabstop->SetFocus();
	// 				return 3;//TRUE;
	// 			}
	// 		}
	// 	}

	return 1;
}

LRESULT Window::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	_ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT Window::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	_ASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void Window::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
	_ASSERT(::IsWindow(m_hWnd));
	RECT rc = { 0 };

	if (!::GetClientRect(m_hWnd, &rc))
	{
		return;
	}

	if (cx != -1)
	{
		rc.right = cx;
	}

	if (cy != -1)
	{
		rc.bottom = cy;
	}

	if (!::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD)
				&& (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)))
	{
		return;
	}

	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

int Window::UseLayeredWindow(BOOL bUsed)
{
	_ASSERT(m_hWnd != NULL);

	// Add or Remove the window WS_EX_LAYERED style, and Set the window transparent
	SetLayeredWindow(m_hWnd, bUsed);

	if (bUsed)
	{

		// 对于分层窗口，必须调用UpdateLayeredWindow或SetLayeredWindowAttributes之一，
		// 否则窗口不会显示

		// Call UpdateLayeredWindow show the layered window.
		if (IsUpdateLayeredWindow(m_hWnd))
		{
			Invalidate();
		}
		// Call SetLayeredWindowAttributes to set the opacity
		// and transparency color key of a layered window.
		else
		{
			HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
			if (hInst)
			{
				typedef BOOL (WINAPI * MYFUNC)(HWND, COLORREF, BYTE, DWORD);
				MYFUNC SetLayeredWindowAttributes = NULL;
				SetLayeredWindowAttributes = (MYFUNC)GetProcAddress(hInst,
						"SetLayeredWindowAttributes");
				if (SetLayeredWindowAttributes)
				{
					DWORD dwFlags = 0;
					if (m_bUseMaskColor)
					{
						dwFlags |= LWA_COLORKEY;
					}
					if (m_bAlpha)
					{
						dwFlags |= LWA_ALPHA;
					}

					BOOL bRet = SetLayeredWindowAttributes(m_hWnd,
							m_crMask, m_btAlphaValue, dwFlags);
				}
				FreeLibrary(hInst);
			}
		}
	}
	else
	{
		if (m_bUseMaskColor)
		{
		}
	}

	return 0;
}

BOOL Window::IsLayeredWindow() const
{
	_ASSERT(m_hWnd != NULL);

	LONG lStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);


	return IsIncludeFlag(lStyle, WS_EX_LAYERED);
}

int Window::NeedRelayoutChildren()
{
	m_WindowHelper.m_bNeedRelayoutChildren = TRUE;
	return 0;
}

void Window::Invalidate(BOOL bErase/* = TRUE*/)
{
	if (!::IsWindowVisible(m_hWnd))
	{
		return;
	}

	if (IsUpdateLayeredWindow(m_hWnd))
	{
		if (!IsULWNoDraw(m_hWnd))
		{
			DWORD dwStart = GetTickCount();
			CIUIRect rcWin;
			::GetWindowRect(m_hWnd, rcWin);

			int nWidth = rcWin.Width();
			int nHeight = rcWin.Height();

			// draw image to memory DC
			HDC hdcTemp = ::GetDC(m_hWnd);
			HDC hdcMem = CreateCompatibleDC(hdcTemp);
			HBITMAP hbmpMem = CreateCompatibleBitmap(hdcTemp, nWidth, nHeight);
			//void *pBits = NULL;
			//HBITMAP hbmpMem = Create32BitsHBITMAP(nWidth, nHeight, 32, BI_RGB, &pBits);
			HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMem, hbmpMem);

			CIUIRect rcDest = rcWin;
			rcDest.OffsetRect(-rcWin.TopLeft());

			// m_bNeedRelayout为true表示需要重新布局。 有可能是窗口尺寸发生了变化,
			// 或者插入了新的子控件等。
			// 之所以在WM_PAINT中调整布局，是因为在WM_SIZE中调整的话，每次收到WM_SIZE都要调整布局。
			// 虽然每次WM_SIZE都会InvalidateRect，但WM_PAINT优先级比较低。
			// 所以收到WM_SIZE 100次，过程中可能只收到WM_PAINT 20次，这样提高性能。
			// 为了不是每次WM_PAINT都重调布局，增加一个标志m_bNeedRelayout。
			// m_bNeedRelayout在WM_SIZE中会设置为true，在例如插入控件等操作时，
			// 也会设置为true。 原则就是需要重新布局时，设置为true。
			int nRet = RelayoutChildren();
			nRet = OnDraw(hdcMem, rcDest);

#if 0
			// 把hTempBmp各像素的Alpha值设置为0xFF
			// 		for (int y = 0; y < nHeight; y++)
			// 		{
			// 			for (int x = 0; x < nWidth; x++)
			// 			{
			// 				byte *pByte = (byte *)(pBits) + (y * nWidth * 4 + x * 4);
			// 				pByte[0] = 0x00; // B
			// 				pByte[1] = 0x00; // G
			// 				pByte[2] = 0xFF; // R
			// 				pByte[3] = 0xFF;
			// 			}
			// 		}
#endif
			// Update Layered Window
			SIZE sizeWindow = {nWidth, nHeight};
			POINT ptSrc = {0, 0};

			BLENDFUNCTION blend;
			blend.BlendOp = AC_SRC_OVER;
			blend.BlendFlags = 0;
			blend.SourceConstantAlpha = 255;
			blend.AlphaFormat = AC_SRC_ALPHA;

			if (nRet == 0)
			{
				BOOL bRet = ::UpdateLayeredWindow(m_hWnd, hdcTemp,
						NULL, &sizeWindow, hdcMem, &ptSrc, 0, &blend, 2);
			}

			::SelectObject(hdcMem, hbmpOld);
			::DeleteObject(hbmpMem);
			hbmpMem = NULL;
			::DeleteDC(hdcMem);
			hdcMem = NULL;
			::ReleaseDC(m_hWnd, hdcTemp);
			hdcTemp = NULL;

#ifdef _DEBUG
			DWORD dw = GetTickCount() - dwStart;
			int n = 0;
			static int i = 0;
			IUITRACE(_T("%d: UpdateLayeredWindow speed %d ms\n"), i++, dw);
#endif
		}
	}
	else
	{
		::InvalidateRect(m_hWnd, NULL, bErase);
	}
}

CONTROL_LAYOUT_HOR Window::SetHorzLayoutMode(CONTROL_LAYOUT_HOR eLayout)
{
	return Window_SetHorzLayoutMode(m_hWnd, eLayout);
}

CONTROL_LAYOUT_HOR Window::GetHorzLayoutMode() const
{
	return m_eHAlign;
}

CONTROL_LAYOUT_VER Window::SetVertLayoutMode(CONTROL_LAYOUT_VER eLayout)
{
	CONTROL_LAYOUT_VER eOld = m_eVAlign;
	m_eVAlign = eLayout;
	return eOld;
}

CONTROL_LAYOUT_VER Window::GetVertLayoutMode() const
{
	return m_eVAlign;
}

int Window::SetLayoutMargin(LPCRECT lprcMargin)
{
	return ::SendMessage(m_hWnd, WM_SET_LAYOUT_MARGIN, TRUE, (LPARAM)lprcMargin);
}

int Window::GetLayoutMargin(LPRECT lprcMargin) const
{
	if (lprcMargin == NULL)
	{
		return -1;
	}

	*lprcMargin = m_rcMargin;

	return 0;
}

int Window::SetLayoutInset(LPCRECT lprcInset)
{
	return ::SendMessage(m_hWnd, WM_SET_LAYOUT_INSET, TRUE, (LPARAM)lprcInset);
}

int Window::GetLayoutInset(LPRECT lprcInset) const
{
	if (lprcInset == NULL)
	{
		return -1;
	}

	*lprcInset = m_rcInset;

	return 0;
}

HWLWND Window::GetChild()
{
	return (HWLWND)SendMessage(WM_GETCHILD, 0, 0);
}

HWND Window::SetFocus()
{
	return ::SetFocus(m_hWnd);
}

// Window类不应该有SetWLFocus，而是应该调用CWLWnd::SetFocus代替
HWLWND Window::GetWLFocus()
{
	return (HWLWND)::SendMessage(m_hWnd, WM_GETWLFOCUS, 0, LPARAM(m_hWnd));
}

HWLWND Window::GetWLFocus(HWND hWnd)
{
	return (HWLWND)::SendMessage(hWnd, WM_GETWLFOCUS, 0, LPARAM(hWnd));
}

WindowCore *Window::GetPaintManager()
{
	return &m_WindowHelper;
}

// This can be set to TRUE to override all AfxEnableMemoryTracking calls,
// allowing all allocations, even MFC internal allocations to be tracked.
BOOL _iuiMemoryLeakOverride = FALSE;

BOOL IUIEnableMemoryTracking(BOOL bTrack)
{
	if (_iuiMemoryLeakOverride)
	{
		return TRUE;
	}

	int nOldState = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	if (bTrack)
	{
		_CrtSetDbgFlag(nOldState | _CRTDBG_ALLOC_MEM_DF);
	}
	else
	{
		_CrtSetDbgFlag(nOldState & ~_CRTDBG_ALLOC_MEM_DF);
	}
	return nOldState & _CRTDBG_ALLOC_MEM_DF;
}

#ifndef AFX_CDECL
#define AFX_CDECL __cdecl
#endif

int AFX_CDECL IUINewHandler(size_t /* nSize */)
{
	// AfxThrowMemoryException();
	return 0;
}

#pragma warning(disable: 4273)

// for global data that should be in COMDATs (packaged data)
#ifndef AFX_COMDAT
#define AFX_COMDAT __declspec(selectany)
#endif

std::map<HWND, Window *> *IUI::iuiMapHWND(BOOL bCreate)
{
	IUI_MODULE_THREAD_STATE *pState = IUIGetModuleThreadState();
	if (pState->m_pmapHWND == NULL && bCreate)
	{
		BOOL bEnable = IUIEnableMemoryTracking(FALSE);
		//#ifndef _AFX_PORTABLE
		//		_PNH pnhOldHandler = IUISetNewHandler(&IUICriticalNewHandler);
		//#endif

		pState->m_pmapHWND = new std::map<HWND, Window *>;
		//pState->m_pmapHWND = new CHandleMap(RUNTIME_CLASS(CWnd),
		//	ConstructDestruct<CWnd>::Construct, ConstructDestruct<CWnd>::Destruct,
		//	offsetof(CWnd, m_hWnd));

		//#ifndef _AFX_PORTABLE
		//		AfxSetNewHandler(pnhOldHandler);
		//#endif
		IUIEnableMemoryTracking(bEnable);
	}

	return pState->m_pmapHWND;
}

std::map<HWND, std::set<void *> > *IUI::iuiMapSubclass(BOOL bCreate)
{
	IUI_MODULE_THREAD_STATE *pState = IUIGetModuleThreadState();
	if (pState->m_pmapSubclassInfo == NULL && bCreate)
	{
		BOOL bEnable = IUIEnableMemoryTracking(FALSE);
		pState->m_pmapSubclassInfo = new std::map<HWND, std::set<void *> >;
		IUIEnableMemoryTracking(bEnable);
	}

	return pState->m_pmapSubclassInfo;
}

std::map<UINT_PTR, HTIMER> *IUI::iuiMapTimer(BOOL bCreate/* = FALSE*/)
{
	IUI_MODULE_THREAD_STATE *pState = IUIGetModuleThreadState();
	if (bCreate)
	{
		BOOL bEnable = IUIEnableMemoryTracking(FALSE);
		//#ifndef _AFX_PORTABLE
		//		_PNH pnhOldHandler = IUISetNewHandler(&IUICriticalNewHandler);
		//#endif

		//pState->m_pmapHWND = new CHandleMap(RUNTIME_CLASS(CWnd),
		//	ConstructDestruct<CWnd>::Construct, ConstructDestruct<CWnd>::Destruct,
		//	offsetof(CWnd, m_hWnd));

		//#ifndef _AFX_PORTABLE
		//		AfxSetNewHandler(pnhOldHandler);
		//#endif
		IUIEnableMemoryTracking(bEnable);
	}

	return &pState->m_mapTimer;
}


Window *Window::FromHandlePermanent(HWND hWnd)
{
	std::map<HWND, Window *> *pMap = iuiMapHWND(TRUE);
	Window *pWnd = NULL;
	if (pMap != NULL)
	{
		// only look in the permanent map - does no allocations
		std::map<HWND, Window *>::iterator it = pMap->find(hWnd);
		if (it != pMap->end())
		{
			pWnd = it->second;
		}
		_ASSERT(pWnd == NULL || pWnd->m_hWnd == hWnd);
	}

	return pWnd;
}

BOOL Window::Attach(HWND hWndNew)
{
	_ASSERT(m_hWnd == NULL);     // only attach once, detach on destroy
	_ASSERT(FromHandlePermanent(hWndNew) == NULL);
	// must not already be in permanent map

	if (hWndNew == NULL)
	{
		return FALSE;
	}

	std::map<HWND, Window *> *pMap = iuiMapHWND(TRUE);
	_ASSERT(pMap != NULL);

	(*pMap)[m_hWnd = hWndNew] = this;

	return TRUE;
}

HWND Window::Detach()
{
	HWND hWnd = m_hWnd;

	if (hWnd != NULL)
	{
		std::map<HWND, Window *> *pMap = iuiMapHWND();
		if (pMap != NULL)
		{
			std::map<HWND, Window *>::iterator it = pMap->find(m_hWnd);
			if (it != pMap->end())
			{
				pMap->erase(it);
			}
		}

		m_hWnd = NULL;
	}

	return hWnd;
}

void Window::PreSubclassWindow()
{
	// no default processing
}

int Window::AddHandler(CWndHandler *pHandler)
{
	m_pWndHandler = pHandler;
	pHandler->SetBindWindow(this);

	// Return index.
	return 1;
}

int Window::RemoveHandler(CWndHandler *pHandler)
{
	if (m_pWndHandler == pHandler)
	{
		m_pWndHandler = NULL;
	}

	return 0;
}

int Window::RemoveAllHanlders()
{
	m_pWndHandler = NULL;
	return 0;
}

int Window::GetHandlerCount() const
{
	return m_pWndHandler == NULL ? 0 : 1;
}

HWLWND Window::FindControl(LPCTSTR lpszControlName)
{
	HWLWND hRet = NULL;

	HWLWND hRoot = GetChild();
	for (; hRoot != NULL; hRoot = GetWindow(hRoot, GW_HWNDNEXT))
	{
		if (GetName(hRoot) == lpszControlName)
		{
			hRet = hRoot;
			break;
		}

		hRet = IUI::FindControl(hRoot, lpszControlName);
		if (hRet != NULL)
		{
			break;
		}
	}

	return hRet;
}

BOOL PASCAL Window::WalkPreTranslateTree(HWND hWndStop, MSG *pMsg)
{
	_ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
	_ASSERT(pMsg != NULL);

	// walk from the target window up to the hWndStop window checking
	//  if any window wants to translate this message

	for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
	{
		Window *pWnd = Window::FromHandlePermanent(hWnd);
		if (pWnd != NULL)
		{
			// target window is a C++ window
			if (pWnd->PreTranslateMessage(pMsg))
			{
				return TRUE;    // trapped by target window (eg: accelerators)
			}
		}

		// got to hWndStop window without interest
		if (hWnd == hWndStop)
		{
			break;
		}
	}
	return FALSE;       // no special processing
}

BOOL _IUICompareClassName(HWND hWnd, LPCTSTR lpszClassName)
{
	_ASSERT(::IsWindow(hWnd));
	TCHAR szTemp[32];
	::GetClassName(hWnd, szTemp, _countof(szTemp));
	return ::StrCmp(szTemp, lpszClassName) == 0;
}

BOOL Window::PreTranslateMessage(MSG *pMsg)
{
	// for modeless processing (or modal)
	_ASSERT(m_hWnd != NULL);

	// allow tooltip messages to be filtered
	IUI_MODULE_STATE *pModuleState = IUIGetModuleState();
	if (pModuleState->m_pfnFilterToolTipMessage != NULL)
	{
		(*pModuleState->m_pfnFilterToolTipMessage)(pMsg, this);
	}

	// fix around for VK_ESCAPE in a multiline Edit that is on a Dialog
	// that doesn't have a cancel or the cancel is disabled.
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL) &&
			(::GetWindowLong(pMsg->hwnd, GWL_STYLE) & ES_MULTILINE) &&
			_IUICompareClassName(pMsg->hwnd, _T("Edit")))
		{
			HWND hItem = ::GetDlgItem(m_hWnd, IDCANCEL);
			if (hItem == NULL || ::IsWindowEnabled(hItem))
			{
				SendMessage(WM_COMMAND, IDCANCEL, 0);
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_TAB)
		{
			int nRet = HandleTabKey(pMsg);
			if (nRet == 1)
			{
				return IsDialogMessage(m_hWnd, pMsg);
			}
			else if (nRet == 2)
			{
				return FALSE;
			}
			else if (nRet == 3)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL Window::SetDrawBkColor(BOOL bDraw)
{
	BOOL bOld = m_bDrawBkColor;
	m_bDrawBkColor = bDraw ? true : false;
	return m_bDrawBkColor;
}

BOOL Window::IsDrawBkColor() const
{
	return m_bDrawBkColor ? TRUE : FALSE;
}

COLORREF Window::SetBkColor(COLORREF dwColor)
{
	COLORREF dwOldColor = m_crBk;
	m_crBk = dwColor;
	return dwOldColor;
}

COLORREF Window::GetBkColor() const
{
	return m_crBk;
}

int Window::SetBkImage(LPCTSTR lpszImage)
{
	IUISetControlImage(IUIGetModuleState()->m_pProject, &m_hImageBk, lpszImage);
	Invalidate();

	return 0;
}

LPCTSTR Window::GetBkImage() const
{
	return m_hImageBk->GetSafeImageName();
}

int Window::SetBkImageResizeMode(IMAGE_RESIZE_MODE eImageResizeMode)
{
	m_eImageResizeMode = eImageResizeMode;
	return 0;
}

IMAGE_RESIZE_MODE Window::GetBkImageResizeMode() const
{
	return m_eImageResizeMode;
}

int Window::Set9GridResizeParam(__in LPCRECT lpResize)
{
	if (lpResize == NULL)
	{
		return -1;
	}

	m_rc9GridResizeParam = *lpResize;

	return 0;
}

int Window::Get9GridResizeParam(__out LPRECT lpResize)
{
	if (lpResize == NULL)
	{
		return -1;
	}

	*lpResize = m_rc9GridResizeParam;

	return 0;
}

bool Window::SetDrawFalseBorderColor(bool bDraw)
{
	bool bOld = m_bDrawBorderColor;
	m_bDrawBorderColor = bDraw;
	return m_bDrawBorderColor;
}

bool Window::IsDrawFalseBorderColor() const
{
	return m_bDrawBorderColor;
}

DWORD Window::SetFalseBorderColor(DWORD dwColor)
{
	DWORD dwOldColor = m_crBorderColor;
	m_crBorderColor = dwColor;
	return dwOldColor;
}

DWORD Window::GetFalseBorderColor() const
{
	return m_crBorderColor;
}

int Window::SetTrueBorderThickness(LPCRECT lprcBorder)
{
	m_rcTrueBorder = lprcBorder;
	return 0;
}

int Window::GetTrueBorderThickness(LPRECT lprcBorder) const
{
	if (lprcBorder == NULL)
	{
		return -1;
	}

	*lprcBorder = m_rcTrueBorder;

	return 0;
}

int Window::SetTitleBarHeight(int nHeight)
{
	int nOld = m_nTitleBarHeight;
	m_nTitleBarHeight = nHeight;
	return nOld;
}

int Window::GetTitleBarHeight() const
{
	return m_nTitleBarHeight;
}

int Window::GetClientRect(__out LPRECT lpRect) const
{
	BOOL bRet = ::GetClientRect(m_hWnd, lpRect);
	return bRet ? 0 : -1;
}

int Window::GetWindowRect(__out LPRECT lpRect) const
{
	BOOL bRet = ::GetWindowRect(m_hWnd, lpRect);
	return bRet ? 0 : -1;
}

int Window::CenterWindow(HWND hAlternateOwner = NULL)
{
	_ASSERT(::IsWindow(m_hWnd));

	// determine owner window to center against
	DWORD dwStyle = (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
	HWND hWndCenter = hAlternateOwner;
	if (hAlternateOwner == NULL)
	{
		if (dwStyle & WS_CHILD)
		{
			hWndCenter = ::GetParent(m_hWnd);
		}
		else
		{
			hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
		}
		if (hWndCenter != NULL)
		{
			// let parent determine alternate center window
			HWND hWndTemp = (HWND)::SendMessage(hWndCenter,
					WM_QUERYCENTERWND, 0, 0);
			if (hWndTemp != NULL)
			{
				hWndCenter = hWndTemp;
			}
		}
	}

	// get coordinates of the window relative to its parent
	CIUIRect rcDlg;
	GetWindowRect(&rcDlg);
	CIUIRect rcArea;
	CIUIRect rcCenter;
	HWND hWndParent;
	if (!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if (hWndCenter != NULL)
		{
			DWORD dwAlternateStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if (!(dwAlternateStyle & WS_VISIBLE) || (dwAlternateStyle & WS_MINIMIZE))
			{
				hWndCenter = NULL;
			}
		}

		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		// center within appropriate monitor coordinates
		if (hWndCenter == NULL)
		{
			HWND hwDefault = IUIGetMainWnd()->GetSafeHwnd();

			GetMonitorInfo(
				MonitorFromWindow(hwDefault, MONITOR_DEFAULTTOPRIMARY), &mi);
			rcCenter = mi.rcWork;
			rcArea = mi.rcWork;
		}
		else
		{
			::GetWindowRect(hWndCenter, &rcCenter);
			GetMonitorInfo(
				MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;
		}
	}
	else
	{
		// center within parent client coordinates
		hWndParent = ::GetParent(m_hWnd);
		_ASSERT(::IsWindow(hWndParent));

		::GetClientRect(hWndParent, &rcArea);
		_ASSERT(::IsWindow(hWndCenter));
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT *)&rcCenter, 2);
	}

	// find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - rcDlg.Width() / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - rcDlg.Height() / 2;

	// if the dialog is outside the screen, move it inside
	if (xLeft + rcDlg.Width() > rcArea.right)
	{
		xLeft = rcArea.right - rcDlg.Width();
	}
	if (xLeft < rcArea.left)
	{
		xLeft = rcArea.left;
	}

	if (yTop + rcDlg.Height() > rcArea.bottom)
	{
		yTop = rcArea.bottom - rcDlg.Height();
	}
	if (yTop < rcArea.top)
	{
		yTop = rcArea.top;
	}

	// map screen coordinates to child coordinates
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

int Window::SetDragResizeFlag(BOOL bDragResize)
{
	m_bDragResize = bDragResize;
	return 0;
}

BOOL Window::GetDragResizeFlag() const
{
	return m_bDragResize;
}

LRESULT Window::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT Window::OnDestroy()
{
	m_WindowHelper.DestroyAllRootControls();

	// 销毁背景
	ReleaseIUIImage(m_hImageBk);
	m_hImageBk = NULL;

	return 0;
}

LRESULT Window::OnNcDestroy()
{
	// 断掉最后一个事件。 因为子控件内存将被删除。
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

	if (pThreadState != NULL)
	{
		// cleanup main and active windows
		if (pThreadState->m_pMainWnd == this)
		{
			pThreadState->m_pMainWnd = NULL;
		}

		if (pThreadState->m_pActiveWnd == this)
		{
			pThreadState->m_pActiveWnd = NULL;
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	::RevokeDragDrop(m_hWnd);

	Detach();
	_ASSERT(m_hWnd == NULL);

	// 释放内存和资源，允许本对象重新Create.
	m_hResInstance = NULL;
	m_uIDD = 0;
	m_hFocus = NULL;
	m_strXml = _T("");
	m_rcXML.SetRectEmpty();
	m_bCreateAsDialog = false;

	ReleaseRoot(m_hVirtualRoot);
	m_hVirtualRoot = NULL;

	if (IsWindow(m_hShadowWnd))
	{
		::DestroyWindow(m_hShadowWnd);
	}
	m_hShadowWnd = NULL;

	return 0;
}

int ReturnHTCAPTION(HWND hWnd, int nTitleBarHeight, POINT pt)
{
	if (nTitleBarHeight == -1)
	{
		return HTCAPTION;
	}
	else if (nTitleBarHeight > 0)
	{
		CIUIRect rcClient;
		::GetClientRect(hWnd, rcClient);
		rcClient.bottom = nTitleBarHeight;

		if (rcClient.PtInRect(pt))
		{
			return HTCAPTION;
		}
	}

	return 0;
}

// 转成非客户区鼠标消息（如果命中了非客户区的话）
UINT IUI::ConvertToNonClientMouseMessage(int uHitTest, UINT uMouseMsg)
{
	UINT uNewMsg = uMouseMsg;
	if (uHitTest >= HTCAPTION)
	{
		switch (uMouseMsg)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
#if (_WIN32_WINNT >= 0x0500)
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
#endif
			uNewMsg = uMouseMsg + (WM_NCMOUSEMOVE - WM_MOUSEMOVE);
			break;
		}

		if (uMouseMsg == WM_MOUSELEAVE)
		{
			uNewMsg = WM_NCMOUSELEAVE;
		}
	}

	return uNewMsg;
}

// 采用二分法查找指定cp后面第一个Ole对象在Ole管理中的索引（非在RichEdit中的cp）
// 参数微软泄漏代码：CObjectMgr::FindIndexForCp
int IUI::FindObjectNearCp(RichTextBox *pRe, LONG cp, __out REOBJECT *pReObj)
{
	if (NULL == pRe || NULL == pReObj)
	{
		return -1;
	}

	CComPtr<IRichEditOle> pRichEditOle = pRe->GetIRichEditOle();
	LONG nOleCount = pRichEditOle->GetObjectCount();
	if (nOleCount <= 0)
	{
		return -1;
	}

	LONG lLeft, lRight;
	IOleObject *pobj = NULL;
	int iMid = 0;

	lLeft = 0;
	lRight = nOleCount - 1;

	REOBJECT ro;
	ro.cbStruct = sizeof(REOBJECT);

	// 采用二分法查找指定cp附近的Ole对象, 找到的对象可能在指定cp的前面，也可能在后面
	// 或者指定的cp就是Ole对象
	while (lRight >= lLeft)
	{
		iMid = (lLeft + lRight) / 2;

		HRESULT hr = pRichEditOle->GetObject(iMid, &ro, REO_GETOBJ_POLEOBJ);
		pobj = ro.poleobj;

		_ASSERT(NULL != ro.poleobj);

		if (ro.cp == cp)
		{
			// 如果指定的cp就是Ole对象，直接返回
			*pReObj = ro;
			return iMid;
		}
		else if (ro.cp < cp)
		{
			lLeft = iMid + 1;
		}
		else
		{
			lRight = iMid - 1;
		}
	}

	// 如果RichEdit中包含Ole，则pojb一定不为NULL,
	// 如果RichEdit中不包含Ole，则pobj一定为NULL，且iMid一定为0
	_ASSERT(NULL != pobj || (NULL == pobj && iMid == 0));

	// 执行到这儿，说明指定的cp不是Ole对象，那么查找到的cp附近的Ole，可能在cp前面，也可能在后面
	// 如果在前面，则索引加1，返回cp后面那个Ole对象
	if (NULL != pobj)
	{
		// 如果RichEdit中包含Ole，则最后一个测试的Ole的cp一定与传入的cp不相等，因为如果
		// 相等，在上面的while中就已经提交返回了。
		_ASSERT(ro.cp != cp);

		// 如果最后一个测试的Ole的cp小于传入的cp，则索引加1 ，返回传入cp之后第一个Ole的索引
		if (ro.cp < cp)
		{
			iMid++;

		}
	}

	HRESULT hr = pRichEditOle->GetObject(iMid, &ro, REO_GETOBJ_POLEOBJ);
	*pReObj = ro;

	return iMid;
}

/*
*	DuplicateHGlobal
*
*	Purpose:
*		duplicates the passed in hglobal
*/

HGLOBAL IUI::DuplicateHGlobal(HGLOBAL hglobal)
{
	//TRACEBEGIN(TRCSUBSYSUTIL, TRCSCOPEINTERN, "DuplicateHGlobal");

	UINT	flags;
	DWORD	size;
	HGLOBAL hNew;
	BYTE 	*pSrc;
	BYTE 	*pDest;

	if (hglobal == NULL)
	{
		return NULL;
	}

	flags = GlobalFlags(hglobal);
#ifdef MACPORT
	// note: GlobalFlags does not return GMEM_SHARE or GMEM_MOVEABLE
	// and these are required for Mac OLE
	flags |= (GMEM_SHARE | GMEM_MOVEABLE);
#endif

	size = GlobalSize(hglobal);

	hNew = GlobalAlloc(flags, size);

	if (hNew)
	{
		pDest = (BYTE *)GlobalLock(hNew);
		pSrc = (BYTE *)GlobalLock(hglobal);

		if (pDest == NULL || pSrc == NULL)
		{
			GlobalUnlock(hNew);
			GlobalUnlock(hglobal);
			GlobalFree(hNew);

			return NULL;
		}

		memcpy(pDest, pSrc, size);

		GlobalUnlock(hNew);
		GlobalUnlock(hglobal);
	}

	return hNew;
}

/*
*	TextHGlobalWtoA
*
*	Purpose:
*		converts a unicode text hglobal into a newly allocated
*		allocated hglobal with ANSI data
*
*	Notes:
*		does *NOT* free the incoming hglobal
*/

HGLOBAL IUI::TextHGlobalWtoA(HGLOBAL hglobalW)
{
	//TRACEBEGIN(TRCSUBSYSUTIL, TRCSCOPEINTERN, "TextHGlobalWtoA");

	LPWSTR 	pwstr;
	HGLOBAL hnew = NULL;
	DWORD	cbSize;

	if (!hglobalW)
	{
		return NULL;
	}

	pwstr = (LPWSTR)GlobalLock(hglobalW);

	CStrIn  strin(pwstr);

	cbSize = (strin.strlen() + 1) * sizeof(CHAR);
	hnew = GlobalAlloc(GMEM_MOVEABLE, cbSize);

	if (hnew)
	{
		LPSTR pstr = (LPSTR)GlobalLock(hnew);

		if (pstr)
		{
			memcpy(pstr, (CHAR *)strin, cbSize);

			GlobalUnlock(hnew);
		}
	}

	GlobalUnlock(hglobalW);

	return hnew;
}

/*
* AllocObjectDescriptor
*
* Purpose:
*  Allocated and fills an OBJECTDESCRIPTOR structure.
*
* Parameters:
*  clsID           CLSID to store.
*  dwAspect        DWORD with the display aspect
*  pszl            LPSIZEL (optional) if the object is being scaled in
*                  its container, then the container should pass the
*                  extents that it is using to display the object.
*  ptl             POINTL from upper-left corner of object where
*                  mouse went down for use with Drag & Drop.
*  dwMisc          DWORD containing MiscStatus flags
*  pszName         LPTSTR naming the object to copy
*  pszSrc          LPTSTR identifying the source of the object.
*
* Return Value:
*  HBGLOBAL         Handle to OBJECTDESCRIPTOR structure.
*/
static HGLOBAL AllocObjectDescriptor(
	CLSID clsID,
	DWORD dwAspect,
	SIZEL szl,
	POINTL ptl,
	DWORD dwMisc,
	LPTSTR pszName,
	LPTSTR pszSrc)
{
	HGLOBAL              hMem = NULL;
	LPOBJECTDESCRIPTOR   pOD;
	DWORD                cb, cbStruct;
	DWORD                cchName, cchSrc;

	cchName = _tcslen(pszName) + 1;

	if (NULL != pszSrc)
	{
		cchSrc = _tcslen(pszSrc) + 1;
	}
	else
	{
		cchSrc = cchName;
		pszSrc = pszName;
	}

	/*
	* Note:  CFSTR_OBJECTDESCRIPTOR is an ANSI structure.
	* That means strings in it must be ANSI.  OLE will do
	* internal conversions back to Unicode as necessary,
	* but we have to put ANSI strings in it ourselves.
	*/
	cbStruct = sizeof(OBJECTDESCRIPTOR);
	cb = cbStruct + (sizeof(WCHAR) * (cchName + cchSrc)); //HACK

	hMem = GlobalAlloc(GHND, cb);

	if (NULL == hMem)
	{
		return NULL;
	}

	pOD = (LPOBJECTDESCRIPTOR)GlobalLock(hMem);

	pOD->cbSize = cb;
	pOD->clsid = clsID;
	pOD->dwDrawAspect = dwAspect;
	pOD->sizel = szl;
	pOD->pointl = ptl;
	pOD->dwStatus = dwMisc;

	if (pszName)
	{
		pOD->dwFullUserTypeName = cbStruct;
		StringCchCopy((LPTSTR)((LPBYTE)pOD + pOD->dwFullUserTypeName), cb, pszName);
	}
	else
	{
		pOD->dwFullUserTypeName = 0;    //No string
	}

	if (pszSrc)
	{
		pOD->dwSrcOfCopy = cbStruct + (cchName * sizeof(WCHAR));

		StringCchCopy((LPTSTR)((LPBYTE)pOD + pOD->dwSrcOfCopy), cb, pszSrc);
	}
	else
	{
		pOD->dwSrcOfCopy = 0;    //No string
	}

	GlobalUnlock(hMem);
	return hMem;
}

HGLOBAL IUI::OleGetObjectDescriptorDataFromOleObject(
	LPOLEOBJECT pObj,
	DWORD dwAspect,
	POINTL ptl,
	LPSIZEL pszl)
{
	CLSID           clsID;
	LPOLESTR          pszName = NULL;
	LPOLESTR          pszSrc = NULL;
	BOOL            fLink = FALSE;
	IOleLink       *pLink;
	TCHAR           szName[512];
	DWORD           dwMisc = 0;
	SIZEL           szl;
	_ASSERT(FALSE);
	szl.cx = 0;
	szl.cy = 0;
	HGLOBAL         hMem;
	HRESULT         hr;

	if (SUCCEEDED(pObj->QueryInterface(IID_IOleLink
				, (void **)&pLink)))
	{
		fLink = TRUE;
	}

	if (FAILED(pObj->GetUserClassID(&clsID)))
	{
		ZeroMemory(&clsID, sizeof(CLSID));
	}

	//Get user string, expand to "Linked %s" if this is link
	pObj->GetUserType(USERCLASSTYPE_FULL, &pszName);
	if (fLink && NULL != pszName)
	{
		// NB!! we do these two lines of code below instead
		// wcscat because we don't use wcscat anywhere else
		// in the product at the moment.  The string "Linked "
		// should never change either.
		StringCchCopy(szName, 512, TEXT("Linked "));
		USES_CONVERSION;
		StringCchCopy(&(szName[7]), 512 - 7, W2T(pszName));
	}
	else
	{
		USES_CONVERSION;
		StringCchCopy(szName, 512, W2T(pszName));
	}

	CoTaskMemFree(pszName);

	/*
	* Get the source name of this object using either the
	* link display name (for link) or a moniker display
	* name.
	*/

	if (fLink)
	{
		hr = pLink->GetSourceDisplayName(&pszSrc);
	}
	else
	{
		IMoniker   *pmk;

		hr = pObj->GetMoniker(OLEGETMONIKER_TEMPFORUSER
				, OLEWHICHMK_OBJFULL, &pmk);

		if (SUCCEEDED(hr))
		{
			IBindCtx  *pbc;
			CreateBindCtx(0, &pbc);

			pmk->GetDisplayName(pbc, NULL, &pszSrc);
			pbc->Release();
			pmk->Release();
		}
	}

	if (fLink)
	{
		pLink->Release();
	}

	//Get MiscStatus bits
	hr = pObj->GetMiscStatus(dwAspect, &dwMisc);

	//Get OBJECTDESCRIPTOR
	USES_CONVERSION;
	hMem = AllocObjectDescriptor(clsID, dwAspect, szl, ptl, dwMisc, szName, W2T(pszSrc));

	CoTaskMemFree(pszSrc);

	return hMem;
}

int Window::HitTest(CIUIPoint pt, BOOL bSetCursor)
{
	CIUIRect rc, rcClient;

	if (IsZoomed(m_hWnd))
	{
		return HTCLIENT;
	}

	GetClientRect(&rcClient);
	rc.left = rc.top = 0;
	rc.right = rc.bottom = m_nCXBorder;

	int nRet = HTCLIENT;

	// left top
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		}
		nRet = HTTOPLEFT;
		return nRet;
	}

	// top
	rc.left = m_nCXBorder + 1;
	rc.right = rcClient.right - m_nCXBorder - 1;
	rc.bottom = m_nCYBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}
		nRet = HTTOP;
		return nRet;
	}

	// right top
	rc.left = rcClient.right - m_nCXBorder;
	rc.right = rcClient.right;
	rc.bottom = m_nCXBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		}
		nRet = HTTOPRIGHT;
		return nRet;
	}

	// right
	rc.top = m_nCXBorder + 1;
	rc.bottom = rcClient.bottom - m_sizeScroll.cy - 1;
	rc.left = rcClient.right - m_nCYBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		nRet = HTRIGHT;
		return nRet;
	}

	// right bottom
	rc.top = rcClient.bottom - m_sizeScroll.cy;
	rc.bottom = rcClient.bottom;
	rc.left = rcClient.right - m_sizeScroll.cx;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		}
		nRet = HTBOTTOMRIGHT;
		return nRet;
	}

	// bottom
	rc.left = m_nCXBorder + 1;
	rc.right = rcClient.right - m_sizeScroll.cx - 1;
	rc.top = rcClient.bottom - m_nCYBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}
		nRet = HTBOTTOM;
		return nRet;
	}

	// left bottom
	rc.left = 0;
	rc.right = m_nCXBorder;
	rc.top = rcClient.bottom - m_nCXBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		}
		nRet = HTBOTTOMLEFT;
		return nRet;
	}

	// left
	rc.top = m_nCXBorder + 1;
	rc.bottom = rcClient.bottom - m_nCXBorder - 1;
	rc.right = m_nCYBorder;
	if (PtInRect(&rc, pt))
	{
		if (bSetCursor)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		nRet = HTLEFT;
		return nRet;
	}

	if (bSetCursor)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	return nRet;
}

int IUI::ResizeWindowByMouse(HWND hWnd, POINT point, POINT ptLButtonDown, UINT uHitTest, LPCRECT lprcLButtonDown)
{
	if (lprcLButtonDown == NULL)
	{
		return -1;
	}

	CIUIPoint pt = point;
	::ClientToScreen(hWnd, &pt);
	int nDx, nDy;
	nDx = pt.x - ptLButtonDown.x;
	nDy = pt.y - ptLButtonDown.y;

	int nNewW = 0;
	int nNewH = 0;

	switch (uHitTest)
	{
	case HTTOPLEFT:
		nNewW = RECTWIDTH(*lprcLButtonDown) - nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown) - nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->right - nNewW, lprcLButtonDown->bottom - nNewH, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTTOP:
		nNewW = RECTWIDTH(*lprcLButtonDown);
		nNewH = RECTHEIGHT(*lprcLButtonDown) - nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->left, lprcLButtonDown->bottom - nNewH, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTTOPRIGHT:
		nNewW = RECTWIDTH(*lprcLButtonDown) + nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown) - nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->left, lprcLButtonDown->bottom - nNewH, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTRIGHT:
		nNewW = RECTWIDTH(*lprcLButtonDown) + nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown);
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->left, lprcLButtonDown->top, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTBOTTOMRIGHT:
		nNewW = RECTWIDTH(*lprcLButtonDown) + nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown) + nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->left, lprcLButtonDown->top, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTBOTTOM:
		nNewW = RECTWIDTH(*lprcLButtonDown);
		nNewH = RECTHEIGHT(*lprcLButtonDown) + nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->left, lprcLButtonDown->top, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTBOTTOMLEFT:
		nNewW = RECTWIDTH(*lprcLButtonDown) - nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown) + nDy;
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->right - nNewW, lprcLButtonDown->top, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;
	case HTLEFT:
		nNewW = RECTWIDTH(*lprcLButtonDown) - nDx;
		nNewH = RECTHEIGHT(*lprcLButtonDown);
		::SetWindowPos(hWnd, NULL, lprcLButtonDown->right - nNewW, lprcLButtonDown->top, nNewW, nNewH, 0);
		IUIUpdateWindow(hWnd);
		break;

	case HTCLIENT:
	{
		Window *pWnd = Window::FromHandlePermanent(hWnd);
		pWnd->HitTest(point, TRUE);
	}
	break;
	}

	return 0;
}

// WM_NC* 鼠标消息，不在这个范围内
BOOL Window::IsMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST || !IsWindow(m_hWnd))
	{
		return FALSE;
	}

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;
	BOOL bRet = TrackMouseEvent(&tme);

	HWND hCapture = ::GetCapture();

#ifdef _DEBUG
	// 调试用代码
	if (uMsg == WM_LBUTTONDOWN)
	{
		int n = 0;
	}
#endif

	CIUIPoint pt(lParam);
	HWLWND hDest = NULL;

	// 缩放边框的过程中，不进行命中测试
	if (!m_bDragResizing)
	{
		// WindowFromPoint不能命中绑定到Item上的控件
		hDest = WindowFromPoint(m_hWnd, pt);
		// IUITRACE(_T("命中控件 %s[%s]\r\n"), GetClassName(hDest), GetWindowText(hDest));
	}

	// resize the window by dragging its border
	if (m_bDragResize && hDest == NULL)
	{
		if (uMsg == WM_LBUTTONDOWN)
		{
			CIUIPoint point((DWORD)lParam);

			m_nHitTestRet = this->HitTest(point, TRUE);
			if (m_nHitTestRet != HTCLIENT)
			{
				::SetCapture(m_hWnd);
				m_ptLButtonDown = point;
				::ClientToScreen(m_hWnd, &m_ptLButtonDown);
				m_bDragResizing = TRUE;
				GetWindowRect(&m_rcLButtonDown);
			}
		}

		else if (uMsg == WM_LBUTTONUP)
		{
			m_nHitTestRet = HTCLIENT;
			m_rcLButtonDown.SetRectEmpty();
			m_ptLButtonDown.x = m_ptLButtonDown.y = 0;
			m_bDragResizing = FALSE;

			// 如果按下了ComboBox的下拉按钮后放开，不应该释放鼠标捕获。
			HWND hCapture = ::GetCapture();
			if (hCapture == m_hWnd)
			{
				HWLWND hCaptureCtrl = IUI::GetCapture(NULL);
				if (GetClassName(hCaptureCtrl) != IUI_COMBOBOX)
				{
					::ReleaseCapture();
				}
			}
		}

		else if (uMsg == WM_CAPTURECHANGED)
		{
			HWND hWndNew = HWND(lParam);
			if (hWndNew != m_hWnd)
			{
				m_nHitTestRet = HTCLIENT;
				m_rcLButtonDown.SetRectEmpty();
				m_ptLButtonDown.x = m_ptLButtonDown.y = 0;
				m_bDragResizing = FALSE;
			}
		}

		// 缩放边框
		else if (uMsg == WM_MOUSEMOVE)
		{
			LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);

			// 当窗口包含WS_THICKFRAME风格时，使用窗口自己的缩放边框
			if (!IsIncludeFlag(lStyle, WS_THICKFRAME))
			{
				ResizeWindowByMouse(m_hWnd, CIUIPoint(lParam),
					m_ptLButtonDown, m_nHitTestRet, m_rcLButtonDown);
			}
		}
	}

	// 命中测试
	LRESULT lrHittest = HTCLIENT;
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

	// MSDN: WM_MOUSEWHEEL消息是发送给焦点窗口（控件）的
	if (uMsg == WM_MOUSEWHEEL)
	{
		HWLWND hFocus = GetWLFocus(m_hWnd);
		if (hFocus != NULL)
		{
			hDest = hFocus;
		}
	}
	// 其它消息发送给普通窗口或捕获鼠标的窗口（控件）
	else
	{
		if (pThreadState->m_pWLCapture != NULL)
		{
			// 经验证，Win32中，Disabled窗口或不可见窗口也可以捕获鼠标，捕获后，
			// 这两种窗口也可收到鼠标消息
			if (GetHostWnd(pThreadState->m_pWLCapture) == m_hWnd)
			{
				hDest = pThreadState->m_pWLCapture;

				// 如果已捕获了鼠标，就使用捕获前的命中测试值。
				lrHittest = pThreadState->m_pWLCapture->m_uLButtonDownHitTestRet;
			}
		}
		else if (hCapture != NULL)
		{
			int n = 0;
			// If there has a window captured mouse input and no windowless control captured mouse input,
			// don't pass mouse message to windowless control.
		}
		else
		{
			CIUIPoint ptScreen = lParam;
			::ClientToScreen(m_hWnd, &ptScreen);
			lrHittest = IUI::HitTest(hDest, &ptScreen);

			if (pThreadState->m_pWLMouseIn != hDest)
			{
				HWLWND hOldMouseIn = pThreadState->m_pWLMouseIn;
				pThreadState->m_pWLMouseIn = hDest;

				if (hOldMouseIn != NULL)
				{
					// 发送鼠标离开消息
					RoutedEventArgs arg;
					arg.hOriginalSender = hOldMouseIn;
					arg.RoutedEvent = ConvertToNonClientMouseMessage(lrHittest, WM_MOUSELEAVE);
					arg.eRoutingStrategy = RS_TUNNEL;
					arg.wParam = wParam;
					arg.lParam = lParam;

					RaiseEvent(hOldMouseIn, &arg);
				}
			}
		}
	}

	if (hDest != NULL)
	{
		// 如果命中了非客户区，要转成非客户区鼠标消息
		UINT uNewMsg = ConvertToNonClientMouseMessage(lrHittest, uMsg);

		// 转成相对目标控件的坐标
		// 对于Windowless RichEdit控件，由于没有窗口句柄，
		// RichEdit控件接收响应的坐标，都应该是相对于Host窗口的坐标。
		// 所以，RichEdit内部，都需要把传入的坐标，转成相对于Host，
		// 这点在写程序时要格外注意。
		CIUIPoint ptDest(lParam);
		::ClientToScreen(m_hWnd, &ptDest);
		ScreenToClient(hDest, &ptDest);

		RoutedEventArgs arg;
		arg.hOriginalSender = hDest;
		arg.RoutedEvent = uNewMsg;
		arg.eRoutingStrategy = RS_TUNNEL;
		arg.wParam = wParam;
		arg.lParam = MAKELPARAM(ptDest.x, ptDest.y);

		LRESULT lr = RaiseEvent(hDest, &arg);
		if (0 != lr || arg.bHandled || 0 != arg.lResult)
		{
			return lr;
		}
	}

	if (pThreadState->m_pWLCapture == NULL && hCapture == NULL)
	{
		CIUIPoint pt(lParam);

		if (0 != m_nTitleBarHeight)
		{
			// 移动Host窗口
			if (uMsg == WM_LBUTTONDOWN)
			{
				if (hDest != NULL)
				{
					if (IsMouseTransparent(hDest))
					{
						// 鼠标透明的窗口就不应该被命中
						_ASSERT(FALSE);
						int nRet = ReturnHTCAPTION(m_hWnd, m_nTitleBarHeight, pt);
						if (nRet == HTCAPTION)
						{
							::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
						}
					}
				}
				else
				{
					int nRet = ReturnHTCAPTION(m_hWnd, m_nTitleBarHeight, pt);
					if (nRet == HTCAPTION)
					{
						::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
					}
				}
			}
			else if (WM_LBUTTONDBLCLK == uMsg)
			{
				::ReleaseCapture();

				// Maximize or restore the window.
				LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
				if ((WS_MAXIMIZEBOX & lStyle) == WS_MAXIMIZEBOX)
				{
					int nRet = ReturnHTCAPTION(m_hWnd, m_nTitleBarHeight, pt);
					if (nRet == HTCAPTION)
					{
						if ((m_nTitleBarHeight > 0 && pt.y < m_nTitleBarHeight)
							|| (m_nTitleBarHeight < 0))
						{
							if (::IsZoomed(m_hWnd))
							{
								::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
							}
							else
							{
								::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
							}
						}
					}
				}
			}
		}

		// If not enter new control, send mouse leave message to the old control
	}

	return TRUE;
}

LRESULT Window::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsWindow(m_hWnd))
	{
		return 0;
	}

	if (m_pfnSuper != NULL)
	{
		return ::CallWindowProc(m_pfnSuper, m_hWnd, nMsg, wParam, lParam);
	}

	WNDPROC pfnWndProc;
	if ((pfnWndProc = *GetSuperWndProcAddr()) == NULL)
	{
		return ::DefWindowProc(m_hWnd, nMsg, wParam, lParam);
	}
	else
	{
		return ::CallWindowProc(pfnWndProc, m_hWnd, nMsg, wParam, lParam);
	}
}

int SendValidateMinMaxInfo(HWND hWnd, WINDOWPOS *pPos)
{
	CIUIRect rcClient;
	::GetClientRect(hWnd, rcClient);

	// Show window from iconic mode, the rcClient is empty, But rcWnd has error value.
	if (rcClient.IsRectEmpty())
	{
		return 0;
	}

	LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
	LONG lExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	// 经使用mfc对话框程序测试，如果对话框没有Resizing（WS_THICKFRAME风格）边框，即使有Thin或
	// Dialog Frame边框，对话框创建过程及使用SetWindowPos修改对话框尺寸时，
	// 都不会收到WM_GETMINMAXINFO消息。
	// 如果对话框有Resizing边框，则创建过程或调用SetWindowPos都会收到
	// WM_GETMINMAXINFO消息，并且受WM_GETMINMAXINFO消息中设置的最小ptMinTrackSize
	// 影响（即使是使用代码SetWindowPos来设置窗口尺寸，也受ptMinTrackSize影响）
	//
	// 对于IUI中无WS_THICKFRAME风格的窗口需要缩放时，我们可以模拟有WS_THICKFRAME风格时的缩放,
	// 在WM_WINDOWPOSCHANGING中，主动发出WM_GETMINMAXINFO消息。

	MINMAXINFO MMI;

	// Copy from X:\win2k\private\ntos\w32\ntuser\kernel\minmax.c
	// 中的xxxInitSendValidateMinMaxInfo
	RECT rcParent;
	// 微软的代码中，如果窗口有父窗口（Parent或Owner），则最大化时，尺寸不能超过父窗口，
	// 但经过写代码测试，有Owner的子窗口最大化的时候，是可以超过父窗口尺寸的。
	/* What monitor is the window really going to maximize to? */
	HMONITOR hMonitorReal = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);

	/* Send dimensions based on the primary only. */
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitorReal, &mi);
	rcParent = mi.rcMonitor;

	int cBorders = GetWindowBorders(lStyle, lExStyle, TRUE, FALSE);

	InflateRect(&rcParent,
		cBorders * GetSystemMetrics(SM_CXBORDER),
		cBorders * GetSystemMetrics(SM_CYBORDER));

	rcParent.right -= rcParent.left;
	rcParent.bottom -= rcParent.top;

	MMI.ptReserved.x = GetSystemMetrics(SM_CXMINIMIZED);
	MMI.ptReserved.y = GetSystemMetrics(SM_CYMINIMIZED);
	MMI.ptMaxSize.x = rcParent.right;
	MMI.ptMaxSize.y = rcParent.bottom;
	MMI.ptMaxPosition = *((LPPOINT)&rcParent.left);
	if (IsIncludeFlag(lStyle, WS_CAPTION)
		&& !IsIncludeFlag(lExStyle, WS_EX_TOOLWINDOW))
	{
		MMI.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
		MMI.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
	}
	else
	{
		int xMin = cBorders * GetSystemMetrics(SM_CXEDGE);
		int yMin = cBorders * GetSystemMetrics(SM_CYEDGE);

		MMI.ptMinTrackSize.x = max(GetSystemMetrics(SM_CXEDGE), xMin);
		MMI.ptMinTrackSize.y = max(GetSystemMetrics(SM_CYEDGE), yMin);
	}
	MMI.ptMaxTrackSize.x = GetSystemMetrics(SM_CXMAXTRACK);
	MMI.ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK);

	::SendMessage(hWnd, WM_GETMINMAXINFO, 0, (LPARAM)(&MMI));

	if (pPos->cx < MMI.ptMinTrackSize.x)
	{
		pPos->cx = MMI.ptMinTrackSize.x;
	}
	if (pPos->cy < MMI.ptMinTrackSize.y)
	{
		pPos->cy = MMI.ptMinTrackSize.y;
	}

	if (pPos->cx > MMI.ptMaxSize.x)
	{
		pPos->cx = MMI.ptMaxSize.x;
	}
	if (pPos->cy > MMI.ptMaxSize.y)
	{
		pPos->cy = MMI.ptMaxSize.y;
	}

	return 0;
}

LRESULT OnNcCalcSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	/*
	* wParam = fCalcValidRects
	* lParam = LPRECT rgrc[3]:
	*        lprc[0] = rcWindowNew = New window rectangle
	*    if fCalcValidRects:
	*        lprc[1] = rcWindowOld = Old window rectangle
	*        lprc[2] = rcClientOld = Old client rectangle
	*
	* On return:
	*        rgrc[0] = rcClientNew = New client rect
	*    if fCalcValidRects:
	*        rgrc[1] = rcValidDst  = Destination valid rectangle
	*        rgrc[2] = rcValidSrc  = Source valid rectangle
	*/

	LPRECT lprcNewWindow = (LPRECT)lParam;

	LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (IsIncludeFlag(lStyle, WS_THICKFRAME))
	{
		CIUIRect rcThickFrame;
		::SendMessage(hWnd, WM_GET_THICKFRAME, 0, (LPARAM)(LPRECT)rcThickFrame);

		lprcNewWindow->left += rcThickFrame.left;
		lprcNewWindow->top += rcThickFrame.top;
		lprcNewWindow->right -= rcThickFrame.right;
		lprcNewWindow->bottom -= rcThickFrame.bottom;
	}

	return 0;
}

BOOL CALLBACK WalkEnableWindowHWLWnd(HWLWND hWndEnableWindow, void *pCallbackParam)
{
	SendMessage(hWndEnableWindow, WLM_PARENTENABLE, WPARAM(pCallbackParam), 0);

	return FALSE;
}

struct SControlTreeItem
{
	HWLWND hWnd;
	HWLWND hParent;
	HWND hDest;
};

struct SControlTreeData
{
	WCHAR szType[32];
	WCHAR szName[100];
	void *pAddress;
	HWLWND hTree;
	HTREEITEM hTreeItem;
};

struct STreeItemContent
{
	HWLWND hTree;
	HTREEITEM hTreeItem;
	HWLWND hWnd;
};

std::list<SControlTreeItem> g_lstControlTree;
BOOL CALLBACK WalkControlTree(HWLWND hWnd, void *pCallbackParam)
{
	SControlTreeItem cti;
	cti.hWnd = hWnd;
	cti.hParent = GetParent(hWnd);

	g_lstControlTree.push_back(cti);

	return FALSE;
}

LRESULT Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!::IsWindow(m_hWnd))
	{
		return 0;
	}

	// WM_NCDESTROY不受CallIUIHandlerMap限制
	if (uMsg == WM_NCDESTROY)
	{
		OnNcDestroy();
	}
	else if (uMsg == WM_UPDATEWINDOW)
	{
		if (IsUpdateLayeredWindow(m_hWnd))
		{
			Invalidate();
		}
		else
		{
			::InvalidateRect(m_hWnd, NULL, TRUE);
			::UpdateWindow(m_hWnd);
		}
	}

	LRESULT lr = CallIUIHandlerMap(uMsg, wParam, lParam);
	if (lr != 0)
	{
		return lr;
	}

	// 鼠标消息
	if (IsMouseMessage(uMsg, wParam, lParam))
	{
		if (!IsWindow(m_hWnd))
		{
			return 0;
		}

		return DefWindowProc(uMsg, wParam, lParam);
	}

	// 键盘消息和输入法消息
	BOOL bKeyboardMsg = (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST);
	BOOL bIMEMsg = ((uMsg >= WM_IME_STARTCOMPOSITION && uMsg <= WM_IME_KEYLAST)
			|| (uMsg >= WM_IME_SETCONTEXT && uMsg <= WM_IME_CHAR)
			|| WM_IME_REQUEST == uMsg
			|| WM_IME_KEYDOWN == uMsg
			|| WM_IME_KEYUP == uMsg);
	if (bKeyboardMsg || bIMEMsg)
	{
		// 直接发送给焦点控件
		if (m_hFocus != NULL)
		{
			RoutedEventArgs arg;
			arg.hSender = m_hFocus;
			arg.hOriginalSender = m_hFocus;
			arg.RoutedEvent = uMsg;
			arg.eRoutingStrategy = RS_TUNNEL;
			arg.wParam = wParam;
			arg.lParam = lParam;

			return RaiseEvent(m_hFocus, &arg);
		}
	}

	// 逐步删除WindowCore
	switch (uMsg)
	{
	case WM_CREATE:
	{
		if (m_pBindWindowProp != NULL
			&& (bool)m_pBindWindowProp->m_bLayeredWindow)
		{
			if ((bool)m_pBindWindowProp->m_bUpdateLayeredWindow)
			{
				::SetProp(m_hWnd, _T("UpdateLayeredWindow"), (HANDLE)1);
			}
			UseLayeredWindow(TRUE);
		}

		// Resizing border
		if (m_pBindWindowProp != NULL)
		{
			SetDragResizeFlag((bool)m_pBindWindowProp->m_bResizingBorder);
		}
	}
	break;

	case WM_INITDIALOG:
	{
		LRESULT lr = OnInitDialog(wParam, lParam);
		if (lr == -1)
		{
			return lr;
		}
	}
	break;

	case WM_DESTROY:
	{
		OnDestroy();

		// 只有主界面销毁时，才退出消息循环
		if (m_hWnd == IUIGetMainWnd()->GetSafeHwnd())
		{
			PostQuitMessage(0);
		}
	}
	break;

	case WM_PRINT:
	case WM_PAINT:
	{
		// static int i = 0;
		// IUITRACE(_T("%d ++++++++++++++++Paint\r\n"), ++i);
		HDC hDC = (HDC)wParam;
		if (hDC == NULL)
		{
			PAINTSTRUCT ps = { 0 };
			::BeginPaint(GetSafeHwnd(), &ps);

			// 只绘制Window类型的窗口
			TCHAR szClass[MAX_PATH] = {0};
			::GetClassName(m_hWnd, szClass, MAX_PATH);
			if (CIUIString(szClass) == IUI_WINDOW)
			{
				m_WindowHelper.OnPaint(ps.hdc, &ps.rcPaint);
			}

			::EndPaint(GetSafeHwnd(), &ps);
		}
		else
		{
			CIUIRect rcClient;
			GetClientRect(rcClient);
			m_WindowHelper.OnPaint(hDC, rcClient);
		}
	}
	return 0;

	case WM_ERASEBKGND:
		return TRUE;

	case WM_SIZE:
	{
		m_WindowHelper.m_bNeedRelayoutChildren = TRUE;
		Invalidate();
	}
	break;

	case WM_COMMAND:
	{
		WORD nNotify = HIWORD(wParam);
		WORD nID = LOWORD(wParam);
		HWND hControl = (HWND)lParam;
		if (nNotify == BN_CLICKED)
		{
			// If the command send by a control, the hControl isn't NULL
			if (!::IsWindow(hControl))
			{
				// 如果窗口作为对话框，处理Esc按钮后，关闭对话框。
				if (m_bCreateAsDialog && nID == IDCANCEL)
				{
					EndDialog(IDCANCEL);
				}
			}
		}
	}
	break;

	case WM_SETWLFOCUS:
	{
		HWLWND hOldFocus = m_hFocus;
		m_hFocus = (HWLWND)lParam;
		// IUITRACE(_T("New Focus %s\r\n"), m_hFocus == NULL ? _T("NULL") : (LPCTSTR)GetName(m_hFocus));
		return (LRESULT)hOldFocus;
	}
	break;

	case WM_GETWLFOCUS:
		return (LRESULT)m_hFocus;

	case WM_KILLFOCUS:
	{
		HWLWND hCapture = IUIGetThreadState()->m_pWLCapture;
		if (NULL != hCapture)
		{
			ReleaseCapture(hCapture);
		}
		else
		{
			::ReleaseCapture();
		}

		if (m_hFocus != NULL)
		{
			RoutedEventArgs arg;
			arg.hSender = m_hFocus;
			arg.hOriginalSender = m_hFocus;
			arg.RoutedEvent = WM_KILLFOCUS;
			arg.eRoutingStrategy = RS_TUNNEL;
			arg.wParam = wParam;
			arg.lParam = lParam;
			HWLWND hTop = GetTopLevelWindow(m_hFocus);
			m_hFocus = NULL;
			LRESULT lr = RaiseEvent(hTop, &arg);
		}

		_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
		if (pThreadState != NULL)
		{
			pThreadState->m_pWLCapture = NULL;
		}
	}
	break;

	case WM_SETCURSOR:
	{
		LRESULT lr = m_WindowHelper.OnSetCursor(wParam, lParam);
		if (lr != 0)
		{
			return lr;
		}
	}
	break;

	case WM_CAPTURECHANGED:
	{
		_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
		if (pThreadState->m_pWLCapture != NULL)
		{
			ReleaseCapture(pThreadState->m_pWLCapture);
			_ASSERT(pThreadState->m_pWLCapture == NULL);
		}
	}
	break;

	case WM_CONTROLEVENT:
	{
		LRESULT lr = OnControlEvent(wParam, lParam);
		RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

		if (lr != 0 || NULL == pe || pe->bHandled)
		{
			return lr;
		}
	}
	break;

	case WM_SET_PROJECT:
		IUIGetModuleState()->m_pProject = (CIUIProject *)lParam;
		return 0;

	case WM_GET_PROJECT:
		return (LRESULT)(IUIGetModuleState()->m_pProject);

	case WM_GET_INTERNAL_ROOT:
		return (LRESULT)m_hVirtualRoot;

	case WM_GETCHILD:
		return (LRESULT)IUI::GetWindow(m_hVirtualRoot, GW_CHILD);

	case WM_GETORIGRECT:
	{
		LPRECT lpRect = (LPRECT)lParam;
		if (lpRect == NULL)
		{
			return -1;
		}

		*lpRect = m_rcXML;
	}
	return 0;

	case WM_SET_LAYOUT_MARGIN:
		if (lParam == NULL)
		{
			return -1;
		}

		m_rcMargin = *(LPCRECT)lParam;

		if (wParam)
		{
			Invalidate();
		}
		return 0;

	case WM_GET_LAYOUT_MARGIN:
		if (lParam == NULL)
		{
			return -1;
		}

		*(LPRECT)lParam = m_rcMargin;
		return 0;

	case WM_SET_LAYOUT_INSET:
		if (lParam == NULL)
		{
			return -1;
		}

		m_rcInset = *(LPCRECT)lParam;

		if (wParam)
		{
			Invalidate();
		}
		return 0;

	case WM_GET_LAYOUT_INSET:
		if (lParam == NULL)
		{
			return -1;
		}

		*(LPRECT)lParam = m_rcInset;
		return 0;

	case WM_SET_XML_RECT:
		if (lParam == NULL)
		{
			return -1;
		}

		m_rcXML = *(LPCRECT)lParam;
		return 0;

	case WM_SET_HORZ_LAYOUT_MODE:
	{
		CONTROL_LAYOUT_HOR eOld = m_eHAlign;
		m_eHAlign = (CONTROL_LAYOUT_HOR)lParam;
		if (wParam)
		{
			Invalidate();
		}
		return eOld;
	}

	case WM_GET_HORZ_LAYOUT_MODE:
		return m_eHAlign;

	case WM_SET_VERT_LAYOUT_MODE:
	{
		CONTROL_LAYOUT_VER eOld = m_eVAlign;
		m_eVAlign = (CONTROL_LAYOUT_VER)lParam;
		if (wParam)
		{
			Invalidate();
		}
		return eOld;
	}

	case WM_GET_VERT_LAYOUT_MODE:
		return m_eVAlign;

	case WM_SET_THICKFRAME:
		m_rcTrueBorder = (LPCRECT)lParam;
		break;

	case WM_GET_THICKFRAME:
		if (lParam == 0)
		{
			return -1;
		}
		else
		{
			*(LPRECT)lParam = m_rcTrueBorder;
		}
		break;

	case WM_MOUSELEAVE:
	{
		_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
		if (pThreadState->m_pWLMouseIn != NULL)
		{
			RoutedEventArgs arg;
			arg.hSender = pThreadState->m_pWLMouseIn;
			arg.hOriginalSender = pThreadState->m_pWLMouseIn;
			arg.RoutedEvent = WM_MOUSELEAVE;
			arg.eRoutingStrategy = RS_TUNNEL;
			arg.wParam = wParam;
			arg.lParam = lParam;
			RaiseEvent(pThreadState->m_pWLMouseIn, &arg);

			pThreadState->m_pWLMouseIn = NULL;
		}
	}
	break;

	// 配合WM_GETMINMAXINFO消息，限制最小Track尺寸
	case WM_WINDOWPOSCHANGING:
	{
		LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);

		// 当窗口包含WS_THICKFRAME风格时，使用窗口自己的缩放边框
		if (!IsIncludeFlag(lStyle, WS_THICKFRAME))
		{
			WINDOWPOS *pPos = (WINDOWPOS *)lParam;

			BOOL bNoSize = ((pPos->flags & SWP_NOSIZE) == SWP_NOSIZE);
			if (!IsIconic(m_hWnd) && !bNoSize)
			{
				SendValidateMinMaxInfo(m_hWnd, pPos);
			}
		}
	}
	break;

	case WM_SET_SHADOW_WINDOW:
		m_hShadowWnd = (HWND)lParam;
		return 0;

	case WM_GET_SHADOW_WINDOW:
		return (LRESULT)m_hShadowWnd;

	case WM_WINDOWPOSCHANGED:
		// 阴影窗口同步跟随
	{
		if (::IsWindow(m_hShadowWnd))
		{
			::SendMessage(m_hShadowWnd, WM_SYNC_HOST_POS, 0, 0);
		}
	}
	break;

	// 如果有阴影窗口，让阴影窗口也跟着显示或隐藏
	case WM_SHOWWINDOW:
	{
		::ShowWindow(m_hShadowWnd, wParam ? SW_SHOW : SW_HIDE);
		Window *pShadowWnd = (Window *)Window::FromHandlePermanent(m_hShadowWnd);
		if (NULL != pShadowWnd)
		{
			pShadowWnd->Invalidate();
		}
	}
	break;

	case WM_NCCALCSIZE:
		return OnNcCalcSize(m_hWnd, wParam, lParam);

	case WM_SYSCOMMAND:
	{
		// Update the check status of Maximize and Restore box.
		if (SC_MAXIMIZE == wParam || SC_RESTORE == wParam)
		{
			HWLWND hButton = NULL;
			HWLWND hParent = NULL;
			HWLWND hChildAfter = NULL;
			while (hButton = FindControlEx(m_hWnd, hParent, hChildAfter, IUI_BUTTON, NULL))
			{
				if (IsIncludeFlag(GetStyle(hButton), BS_AUTOCHECKBOX))
				{
					// 如果CheckBox是最大化按钮
					CHK_TYPE eCheckBoxType = (CHK_TYPE)IUI::SendMessage(hButton, BM_GET_BUTTON_TYPE, 0, 0);
					if (CT_MAXIMIZEBOX == eCheckBoxType)
					{
						if (SC_MAXIMIZE == wParam)
						{
							IUI::SendMessage(hButton, BM_SETCHECK, BST_CHECKED, 0);
						}
						else if (SC_RESTORE == wParam)
						{
							IUI::SendMessage(hButton, BM_SETCHECK, BST_UNCHECKED, 0);
						}
						else
						{
							_ASSERT(FALSE);
						}
					}
				}

				hParent = GetParent(hButton);
				hChildAfter = hButton;
			}
		}
	}
	break;

	case WM_ENABLE:
		// 向它所有后代所送消息。因为它的后代可能需要感知祖先被显示或隐藏了。
		WalkHWLWNDChild(m_hVirtualRoot, WalkEnableWindowHWLWnd, (void *)wParam);
		break;

	case WM_COPYDATA:
	{
		COPYDATASTRUCT *pcds = (COPYDATASTRUCT *)lParam;
		if (NULL != pcds)
		{
			if (7777 == pcds->cbData)
			{
				// 外部请求获得控件列表。
				g_lstControlTree.clear();
				WalkHWLWNDChild(m_hVirtualRoot, WalkControlTree, 0);

				if (!g_lstControlTree.empty())
				{
					SControlTreeItem *p = new SControlTreeItem[g_lstControlTree.size()];
					std::list<SControlTreeItem>::iterator it = g_lstControlTree.begin();
					for (int i = 0; it != g_lstControlTree.end(); ++it, ++i)
					{
						memcpy(p + i, &(*it), sizeof(SControlTreeItem));
						(p + i)->hDest = m_hWnd;
					}
					COPYDATASTRUCT cds;
					cds.cbData = 7778;
					cds.dwData = sizeof(SControlTreeItem) * g_lstControlTree.size();
					cds.lpData = p;
					::SendMessage((HWND)wParam, WM_COPYDATA, WPARAM(m_hWnd), LPARAM(&cds));

					delete []p;
					p = NULL;
				}
			}
			else if (7779 == pcds->cbData)
			{
				// 由于一次性不能把整个数据传回去（WM_COPYDATA有数据长度限制）
				// 所以第一次先传列表，接下来每项一次次传
				STreeItemContent *ptic = (STreeItemContent *)pcds->lpData;

				SControlTreeData cti;
				USES_CONVERSION;
				StringCchCopyW(cti.szType, 32,  GetClassName(ptic->hWnd));
				StringCchCopyW(cti.szName, 100, GetName(ptic->hWnd));
				cti.pAddress = ptic->hWnd;
				cti.hTree = ptic->hTree;
				cti.hTreeItem = ptic->hTreeItem;

				COPYDATASTRUCT cds;
				cds.cbData = 7780;
				cds.dwData = sizeof(SControlTreeData);
				cds.lpData = &cti;
				::SendMessage((HWND)wParam, WM_COPYDATA, WPARAM(m_hWnd), LPARAM(&cds));
			}
		}
	}
	break;

	default:
		break;
	}

	return DefWindowProc(uMsg, wParam, lParam);
}

int Window::OnDraw(HDC hMemDC, LPCRECT lprcPaint)
{
	OnDeforeDraw(hMemDC);

	OnDrawBackground(hMemDC, NULL);

	int nRet = OnDrawControls(hMemDC, lprcPaint);

	OnDrawBorder(hMemDC);

	OnAfterDraw(hMemDC);

	return nRet;
}

int Window::OnDeforeDraw(HDC hMemDC)
{
	return 0;
}

int Window::OnAfterDraw(HDC hMemDC)
{
	return 0;
}

int Window::OnDrawBackground(HDC hMemDC, LPCRECT lprcDest)
{
	CIUIRect rcClient;
	if (lprcDest == NULL)
	{
		GetClientRect(rcClient);
	}
	else
	{
		rcClient = *lprcDest;
	}

	if (IsDrawBkColor())
	{
		FillSolidRect(hMemDC, rcClient, GetBkColor(), IsUpdateLayeredWindow(m_hWnd));
	}

	IUIDrawImage(hMemDC, rcClient, m_hImageBk, m_eImageResizeMode, m_rc9GridResizeParam);

	return 0;
}

int Window::OnDrawBorder(HDC hMemDC)
{
	if (IsDrawFalseBorderColor())
	{
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, (HBRUSH)GetStockObject(NULL_BRUSH));

		HPEN hPen = CreatePen(PS_SOLID, 1, GetFalseBorderColor());
		HPEN hOldPen = (HPEN)SelectObject(hMemDC, hPen);

		CIUIRect rcClient;
		GetClientRect(rcClient);
		Rectangle(hMemDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

		SelectObject(hMemDC, hOldPen);
		DeleteObject(hPen);

		SelectObject(hMemDC, hOldBrush);
	}

	return 0;
}

int Window::OnDrawControls(HDC hMemDC, LPCRECT lprcPaint)
{
	// IUI支持空窗口。

	HWLWND hRoot = GetChild();
	if (hRoot == NULL)
	{
		return 0;
	}

	//
	CIUIRect rcClient;
	GetClientRect(&rcClient);

	BOOL bDrawOK = TRUE;

	for (; hRoot != NULL; hRoot = GetWindow(hRoot, GW_HWNDNEXT))
	{
		if (!IsWindowVisible(hRoot))
		{
			continue;
		}

		int nRet = DrawControlAndChildren(hRoot, hMemDC, lprcPaint);
		if (nRet != 0)
		{
			bDrawOK = FALSE;
		}
	}

	return bDrawOK ? 0 : -1;
}

int Window::RelayoutChildren()
{
	if (!m_WindowHelper.m_bNeedRelayoutChildren)
	{
		return 1;
	}

	HWLWND hRoot = GetChild();

	if (hRoot == NULL)
	{
		return -2;
	}

	CIUIRect rcClient;
	GetClientRect(&rcClient);
	if (::IsRectEmpty(&rcClient))
	{
		return -3;
	}

	m_WindowHelper.m_bNeedRelayoutChildren = FALSE;

	for (; hRoot != NULL; hRoot = GetWindow(hRoot, GW_HWNDNEXT))
	{
		CIUIRect rcNew;
		int nRet = CanvasLayoutElement(hRoot, rcClient.Width(), rcClient.Height(), rcNew);
		if (0 == nRet)
		{
			IUI::MoveWindow(hRoot, rcNew, TRUE);
		}
	}

	return 0;
}

LRESULT Window::CallIUIHandlerMap(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pWndHandler == NULL)
	{
		// Must return 0 to continue message
		return 0;
	}

	DWORD dw = GetTickCount();

	LRESULT lr = m_pWndHandler->WindowProc(this, message, wParam, lParam);

	DWORD d2 = GetTickCount() - dw;

	return lr;
}

/////////////////////////////////////////////////////////////////////////////
// Window tooltip support

//BOOL Window::_EnableToolTips(BOOL bEnable, UINT nFlag)
//{
//	_ASSERT(nFlag == WF_TOOLTIPS || nFlag == WF_TRACKINGTOOLTIPS);
//
//	_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
//	ToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
//
//	if (!bEnable)
//	{
//		// nothing to do if tooltips not enabled
//		if (!(m_nFlags & nFlag))
//		{
//			return TRUE;
//		}
//
//		// cancel tooltip if this window is active
//		if (pThreadState->m_pLastHit == this)
//		{
//			CancelToolTips(TRUE);
//		}
//
//		// remove "dead-area" toolbar
//		if (pToolTip->GetSafeHwnd() != NULL)
//		{
//			TOOLINFO ti;
//			memset(&ti, 0, sizeof(TOOLINFO));
//			ti.cbSize = sizeof(AFX_OLDTOOLINFO);
//			ti.uFlags = TTF_IDISHWND;
//			ti.hwnd = m_hWnd;
//			ti.uId = (UINT)m_hWnd;
//			pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&ti);
//		}
//
//		// success
//		m_nFlags &= ~nFlag;
//		return TRUE;
//	}
//
//	// if already enabled for tooltips, nothing to do
//	if (!(m_nFlags & nFlag))
//	{
//		// success
//		AFX_MODULE_STATE *pModuleState = _AFX_CMDTARGET_GETSTATE();
//		pModuleState->m_pfnFilterToolTipMessage = &Window::_FilterToolTipMessage;
//		m_nFlags |= nFlag;
//	}
//	return TRUE;
//}
//
//BOOL Window::EnableToolTips(BOOL bEnable)
//{
//	return _EnableToolTips(bEnable, WF_TOOLTIPS);
//}
//
//BOOL Window::EnableTrackingToolTips(BOOL bEnable)
//{
//	return _EnableToolTips(bEnable, WF_TRACKINGTOOLTIPS);
//}
//
//void _AfxRelayToolTipMessage(ToolTipCtrl *pToolTip, MSG *pMsg)
//{
//	// transate the message based on TTM_WINDOWFROMPOINT
//	MSG msg = *pMsg;
//	msg.hwnd = (HWND)pToolTip->SendMessage(TTM_WINDOWFROMPOINT, 0, (LPARAM)&msg.pt);
//	CPoint pt = pMsg->pt;
//	if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
//	{
//		::ScreenToClient(msg.hwnd, &pt);
//	}
//	msg.lParam = MAKELONG(pt.x, pt.y);
//
//	// relay mouse event before deleting old tool
//	pToolTip->SendMessage(TTM_RELAYEVENT, 0, (LPARAM)&msg);
//}
//
//void PASCAL Window::_FilterToolTipMessage(MSG *pMsg, HWND hWnd)
//{
//	pWnd->FilterToolTipMessage(pMsg);
//}
//
//void Window::FilterToolTipMessage(MSG *pMsg)
//{
//	// this Window has tooltips enabled
//	UINT message = pMsg->message;
//	if ((message == WM_MOUSEMOVE || message == WM_NCMOUSEMOVE ||
//			message == WM_LBUTTONUP || message == WM_RBUTTONUP ||
//			message == WM_MBUTTONUP) &&
//		(GetKeyState(VK_LBUTTON) >= 0 && GetKeyState(VK_RBUTTON) >= 0 &&
//			GetKeyState(VK_MBUTTON) >= 0))
//	{
//		// make sure that tooltips are not already being handled
//		HWND hWnd = Window::FromHandle(pMsg->hwnd);
//		while (pWnd != NULL && !(pWnd->m_nFlags & (WF_TOOLTIPS | WF_TRACKINGTOOLTIPS)))
//		{
//			pWnd = pWnd->GetParent();
//		}
//		if (pWnd != this)
//		{
//			if (pWnd == NULL)
//			{
//				// tooltips not enabled on this Window, clear last state data
//				_AFX_THREAD_STATE *pThreadState = _afxThreadState.GetData();
//				pThreadState->m_pLastHit = NULL;
//				pThreadState->m_nLastHit = -1;
//			}
//			return;
//		}
//
//		_AFX_THREAD_STATE *pThreadState = _afxThreadState.GetData();
//		ToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
//		HWND hOwner = GetParentOwner();
//		if (pToolTip != NULL && pToolTip->GetOwner() != pOwner)
//		{
//			pToolTip->DestroyWindow();
//			delete pToolTip;
//			pThreadState->m_pToolTip = NULL;
//			pToolTip = NULL;
//		}
//		if (pToolTip == NULL)
//		{
//			pToolTip = new ToolTipCtrl;
//			if (!pToolTip->Create(pOwner, TTS_ALWAYSTIP))
//			{
//				delete pToolTip;
//				return;
//			}
//			pToolTip->SendMessage(TTM_ACTIVATE, FALSE);
//			pThreadState->m_pToolTip = pToolTip;
//		}
//
//		ASSERT_VALID(pToolTip);
//		_ASSERT(::IsWindow(pToolTip->m_hWnd));
//
//		// add a "dead-area" tool for areas between toolbar buttons
//		TOOLINFO ti;
//		memset(&ti, 0, sizeof(TOOLINFO));
//		ti.cbSize = sizeof(AFX_OLDTOOLINFO);
//		ti.uFlags = TTF_IDISHWND;
//		ti.hwnd = m_hWnd;
//		ti.uId = (UINT)m_hWnd;
//		if (!pToolTip->SendMessage(TTM_GETTOOLINFO, 0, (LPARAM)&ti))
//		{
//			_ASSERT(ti.uFlags == TTF_IDISHWND);
//			_ASSERT(ti.hwnd == m_hWnd);
//			_ASSERT(ti.uId == (UINT)m_hWnd);
//			VERIFY(pToolTip->SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti));
//		}
//
//		// determine which tool was hit
//		CPoint point = pMsg->pt;
//		::ScreenToClient(m_hWnd, &point);
//		TOOLINFO tiHit;
//		memset(&tiHit, 0, sizeof(TOOLINFO));
//		tiHit.cbSize = sizeof(AFX_OLDTOOLINFO);
//		int nHit = OnToolHitTest(point, &tiHit);
//
//		// build new toolinfo and if different than current, register it
//		HWND hHitWnd = nHit == -1 ? NULL : this;
//		if (pThreadState->m_nLastHit != nHit || pThreadState->m_pLastHit != pHitWnd)
//		{
//			if (nHit != -1)
//			{
//				// add new tool and activate the tip
//				ti = tiHit;
//				ti.uFlags &= ~(TTF_NOTBUTTON | TTF_ALWAYSTIP);
//				if (m_nFlags & WF_TRACKINGTOOLTIPS)
//				{
//					ti.uFlags |= TTF_TRACK;
//				}
//				VERIFY(pToolTip->SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti));
//				if ((tiHit.uFlags & TTF_ALWAYSTIP) || IsTopParentActive())
//				{
//					// allow the tooltip to popup when it should
//					pToolTip->SendMessage(TTM_ACTIVATE, TRUE);
//					if (m_nFlags & WF_TRACKINGTOOLTIPS)
//					{
//						pToolTip->SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
//					}
//
//					// bring the tooltip window above other popup windows
//					::SetWindowPos(pToolTip->m_hWnd, HWND_TOP, 0, 0, 0, 0,
//						SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
//				}
//			}
//			else
//			{
//				pToolTip->SendMessage(TTM_ACTIVATE, FALSE);
//			}
//
//			// relay mouse event before deleting old tool
//			_AfxRelayToolTipMessage(pToolTip, pMsg);
//
//			// now safe to delete the old tool
//			if (pThreadState->m_lastInfo.cbSize >= sizeof(AFX_OLDTOOLINFO))
//			{
//				pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&pThreadState->m_lastInfo);
//			}
//
//			pThreadState->m_pLastHit = pHitWnd;
//			pThreadState->m_nLastHit = nHit;
//			pThreadState->m_lastInfo = tiHit;
//		}
//		else
//		{
//			if (m_nFlags & WF_TRACKINGTOOLTIPS)
//			{
//				POINT pt;
//
//				::GetCursorPos(&pt);
//				pToolTip->SendMessage(TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y));
//			}
//			else
//			{
//				// relay mouse events through the tooltip
//				if (nHit != -1)
//				{
//					_AfxRelayToolTipMessage(pToolTip, pMsg);
//				}
//			}
//		}
//
//		if ((tiHit.lpszText != LPSTR_TEXTCALLBACK) && (tiHit.hinst == 0))
//		{
//			free(tiHit.lpszText);
//		}
//	}
//	else if (m_nFlags & (WF_TOOLTIPS | WF_TRACKINGTOOLTIPS))
//	{
//		// make sure that tooltips are not already being handled
//		HWND hWnd = Window::FromHandle(pMsg->hwnd);
//		while (pWnd != NULL && pWnd != this && !(pWnd->m_nFlags & (WF_TOOLTIPS | WF_TRACKINGTOOLTIPS)))
//		{
//			pWnd = pWnd->GetParent();
//		}
//		if (pWnd != this)
//		{
//			return;
//		}
//
//		BOOL bKeys = (message >= WM_KEYFIRST && message <= WM_KEYLAST) ||
//			(message >= WM_SYSKEYFIRST && message <= WM_SYSKEYLAST);
//		if ((m_nFlags & WF_TRACKINGTOOLTIPS) == 0 &&
//			(bKeys ||
//				(message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK) ||
//				(message == WM_RBUTTONDOWN || message == WM_RBUTTONDBLCLK) ||
//				(message == WM_MBUTTONDOWN || message == WM_MBUTTONDBLCLK) ||
//				(message == WM_NCLBUTTONDOWN || message == WM_NCLBUTTONDBLCLK) ||
//				(message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONDBLCLK) ||
//				(message == WM_NCMBUTTONDOWN || message == WM_NCMBUTTONDBLCLK)))
//		{
//			Window::CancelToolTips(bKeys);
//		}
//	}
//}

/////////////////////
// Window API

int IUI::Window_BindStyle(HWND hWnd, const CWindowProp *pWindowProp)
{
	// 布局
	Window_SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pWindowProp->m_eHorLayoutMode);
	Window_SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pWindowProp->m_eVerLayoutMode);

	::SendMessage(hWnd, WM_SET_LAYOUT_MARGIN,
		FALSE, (LPARAM)(&(RECT)pWindowProp->m_rcLayoutMargin));
	::SendMessage(hWnd, WM_SET_LAYOUT_INSET,
		FALSE, (LPARAM)(&(RECT)pWindowProp->m_rcInset));

	::SendMessage(hWnd, WM_SET_XML_RECT, FALSE, (LPARAM)(&(RECT)pWindowProp->m_rcControl));

	Window *pWindow = Window::FromHandlePermanent(hWnd);

	pWindow->SetDrawBkColor((bool)pWindowProp->m_bDrawBkColor ? TRUE : FALSE);
	CColorProp *pColorProp = pWindow->GetProject()->GetColorProp(pWindowProp->m_strBkColorResIDN);
	if (pColorProp != NULL)
	{
		pWindow->SetBkColor((COLORREF)pColorProp->m_crColor);
	}
	else
	{
		CColorProp *pDefColorProp = pWindow->GetProject()->GetDefaultColorProp();
		if (pDefColorProp != NULL)
		{
			pWindow->SetBkColor(pDefColorProp->m_crColor);
		}
	}

	pWindow->SetBkImage(pWindowProp->m_strBKImageN);
	pWindow->SetBkImageResizeMode(pWindowProp->m_eBkImageResizeMode);
	pWindow->Set9GridResizeParam(&(RECT(pWindowProp->m_rcBkImage9GridResizeArg)));

	pWindow->SetDrawFalseBorderColor(pWindowProp->m_bDrawBorderColor);

	COLORREF crBorder;
	pWindow->GetProject()->GetControlBorderColor(pWindowProp, &crBorder);
	pWindow->SetFalseBorderColor(crBorder);

	pWindow->SetTitleBarHeight((LONG)pWindowProp->m_lTitlebarHeight);

	return 0;
}

CONTROL_LAYOUT_HOR IUI::Window_SetHorzLayoutMode(HWND hWnd, CONTROL_LAYOUT_HOR eLayout)
{
	return (CONTROL_LAYOUT_HOR)::SendMessage(hWnd, WM_SET_HORZ_LAYOUT_MODE, FALSE, eLayout);
}

CONTROL_LAYOUT_VER IUI::Window_SetVertLayoutMode(HWND hWnd, CONTROL_LAYOUT_VER eLayout)
{
	return (CONTROL_LAYOUT_VER)::SendMessage(hWnd, WM_SET_VERT_LAYOUT_MODE, FALSE, eLayout);
}

#define IUI_SetULWNoRedraw	_T("IUI_SetULWNoRedraw")
void IUI::SetULWNoRedraw(HWND hWnd, BOOL bNoRedraw)
{
	if (!IsUpdateLayeredWindow(hWnd))
	{
		return;
	}

	::SetProp(hWnd, IUI_SetULWNoRedraw, (HANDLE)bNoRedraw);
}

BOOL IUI::IsULWNoDraw(HWND hWnd)
{
	if (!IsUpdateLayeredWindow(hWnd))
	{
		return FALSE;
	}

	return (BOOL)::GetProp(hWnd, IUI_SetULWNoRedraw);
}

int IUI::IUIUpdateWindow(HWND hWnd)
{
	::SendMessage(hWnd, WM_UPDATEWINDOW, 0, 0);

	return 0;
}

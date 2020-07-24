#include "StdAfx.h"
#include "../Control/ControlCore/Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{

	CWLWndHandler::CWLWndHandler()
		: m_hBindWLWnd(NULL)
		, m_nMsgHandlerCount(0)
		, m_lParam(0)
	{
	}

	CWLWndHandler::~CWLWndHandler()
	{
		if (NULL != m_hBindWLWnd)
		{
			m_hBindWLWnd->m_pUserBindWLWndHandler = NULL;
		}
	}

	LRESULT CWLWndHandler::EventHandler(HWLWND hWnd, RoutedEventArgs *pe)
	{
		return 0;
	}

	HWLWND CWLWndHandler::GetBindWLWnd()
	{
		return m_hBindWLWnd;
	}

	LPARAM CWLWndHandler::SetUserData(LPARAM lParam)
	{
		LPARAM lOldParam = m_lParam;
		m_lParam = lParam;
		return lOldParam;
	}

	LPARAM CWLWndHandler::GetUserData() const
	{
		return m_lParam;
	}

	CWLWnd::CWLWnd()
		: m_hWnd(NULL)
		, m_pfnSuper(NULL)
	{
	}

	CWLWnd::~CWLWnd()
	{
	}

	int CWLWnd::SetXml(LPCTSTR lpszXml)
	{
		m_hWnd->m_strXml = lpszXml;

		return 0;
	}

	LPCTSTR CWLWnd::GetXml() const
	{
		return m_hWnd->m_strXml;
	}

	BOOL CWLWnd::Create(
		CWLWndHandler *pUserHandler,
		LPCTSTR lpszClassName,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName,
		LPVOID lpParam/* = NULL*/)
	{
		return CreateEx(pUserHandler, 0, lpszClassName, lpszCaption, dwStyle, rect, hParent, pParent, lpszName, lpParam);
	}

	BOOL CWLWnd::CreateEx(
		CWLWndHandler *pUserHandler,
		DWORD dwExStyle,
		LPCTSTR lpszClassName,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName,
		LPVOID lpParam/* = NULL*/)
	{
		RECT rc = {x, y, x + nWidth, y + nHeight};
		return CreateEx(pUserHandler, dwExStyle, lpszClassName, lpszCaption, dwStyle, rc, hParent, pParent, lpszName, lpParam);
	}

	LRESULT WINAPI CWLWnd_EventHandler(HWLWND hWnd, RoutedEventArgs *pe)
	{
		if (hWnd != NULL && hWnd->ci.pThis != NULL)
		{
			return hWnd->ci.pThis->EventHandler(pe);
		}
		else
		{
			_ASSERT(FALSE);
		}

		return 0;
	}

	LRESULT WINAPI IUIHookWLCreate(HWLWND hWnd, RoutedEventArgs *)
	{
		_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();
		if (pThreadState->m_pWLWndInit != NULL)
		{
			if (pThreadState->m_pWLWndInit->m_hWnd == hWnd)
			{
				// 子类化windowless控件
				pThreadState->m_pWLWndInit->m_pfnSuper = SubclassWindowless(hWnd, CWLWnd_EventHandler);
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		else
		{
			_ASSERT(FALSE);
		}

		return 0;
	}

	// Hook windowless控件的创建，在windowless控件创建时，替换控件的EventHandler
	// 在我们的EventHandler中，调用CWLWnd::EventHandler虚函数
	void HookWindowlessCreate(CWLWnd *pWnd)
	{
		_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();
		if (pThreadState->m_pWLWndInit == pWnd)
		{
			return;
		}

		if (pThreadState->m_hHookWLCreate == NULL)
		{
			pThreadState->m_hHookWLCreate = IUIHookWLCreate;
		}

		_ASSERT(pWnd != NULL);
		_ASSERT(pWnd->m_hWnd == NULL);   // only do once

		_ASSERT(pThreadState->m_pWLWndInit == NULL);   // hook not already in progress
		pThreadState->m_pWLWndInit = pWnd;
	}

	BOOL CWLWnd::CreateEx(
		CWLWndHandler *pUserHandler,
		DWORD dwExStyle,
		LPCTSTR lpszClassName,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName,
		LPVOID lpParam/* = NULL*/)
	{
		// Hook，主要目的是把HWLWND与CWLWnd *对应起来
		HookWindowlessCreate(this);

		m_hWnd = CreateWindowEx(NULL, dwExStyle, lpszClassName, lpszCaption, dwStyle,
				rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
				pParent ==  NULL ? NULL : pParent->m_hWnd,
				lpszName, NULL, lpParam);

		if (m_hWnd == NULL)
		{
			return FALSE;
		}

		_ASSERT(m_hWnd->ci.pThis == this);

		return TRUE;
	}

	// 不带RECT参数
	BOOL CWLWnd::Create(
		CWLWndHandler *pUserHandler,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName,
		LPVOID lpParam/* = NULL*/)
	{
		RECT rc = {0, 0, 0, 0};
		return Create(pUserHandler, lpszCaption, NULL, dwStyle, rc, hParent, pParent, lpszName, lpParam);
	}

	// 由XML直接创建成控件，XML中包含Create重载中的其它参数。
	BOOL CWLWnd::Create(
		CWLWndHandler *pUserHandler,
		LPCTSTR lpszXmlName,
		HWND hParent,
		CWLWnd *pParent)
	{
		if (lpszXmlName == NULL)
		{
			_ASSERT(FALSE);
			return FALSE;
		}

		CUIParse uiParse;
		uiParse.SetUIPath(lpszXmlName);
		CWindowProp *pUIWndPorp = uiParse.GetWindowProp();
		if (pUIWndPorp == NULL)
		{
			return FALSE;
		}

		_ASSERT(!(bool)pUIWndPorp->m_bWindow);
		uiParse.GetWindowProp()->SetFileName(lpszXmlName);

		int nCount = pUIWndPorp->GetChildCount();
		_ASSERT(nCount == 0 || nCount == 1);
		if (nCount != 1)
		{
			return FALSE;
		}

		CControlBaseProp *pCtrlProp = pUIWndPorp->GetChild(0);
		if (!pCtrlProp->IsControl())
		{
			return FALSE;
		}

		return CreateIndirect(pUserHandler, (CControlProp *)pCtrlProp, hParent, pParent);
	}

	// 由控件属性直接创建成控件，控件属性中包含Create重载中的其它参数。
	BOOL CWLWnd::CreateIndirect(
		CWLWndHandler *pUserHandler,
		CControlProp *pCtrlProp,
		HWND hParent,
		CWLWnd *pParent)
	{
		if (pCtrlProp == NULL || !::IsWindow(hParent))
		{
			_ASSERT(FALSE);
			return FALSE;
		}

		m_hWnd = CreatePreDefineChildControl(pUserHandler, NULL,
				pCtrlProp, pParent->m_hWnd, hParent, 0);

		if (m_hWnd == NULL)
		{
			return FALSE;
		}

		return TRUE;
	}

	// static
	CWLWnd *CWLWnd::FromHandle(HWLWND hWnd)
	{
		if (hWnd == NULL)
		{
			return NULL;
		}

		if (hWnd->ci.pThis == NULL)
		{
			hWnd->ci.pThis = new CWLWnd;
			hWnd->ci.pThis->m_hWnd = hWnd;
		}

		return hWnd->ci.pThis;
	}

	BOOL CWLWnd::Attach(HWLWND hWndNew)
	{
		_ASSERT(m_hWnd == NULL);     // only attach once, detach on destroy
		_ASSERT(hWndNew->ci.pThis == NULL);

		if (hWndNew == NULL)
		{
			return FALSE;
		}

		m_hWnd = hWndNew;
		m_hWnd->ci.pThis = this;

		return TRUE;
	}

	HWLWND CWLWnd::Detach()
	{
		HWLWND hWnd = m_hWnd;
		if (hWnd != NULL)
		{
			hWnd->ci.pThis = NULL;
			m_hWnd = NULL;
		}

		return hWnd;
	}

	HWLWND CWLWnd::GetSafeHwnd()
	{
		if (this == NULL)
		{
			return NULL;
		}

		return m_hWnd;
	}

	BOOL CWLWnd::DestroyWindow()
	{
		if (m_hWnd == NULL)
		{
			return FALSE;
		}

		BOOL bResult = IUI::DestroyWindow(m_hWnd);
		m_hWnd = NULL;

		return bResult;
	}

	int CWLWnd::BindStyle(const CControlProp *pCtrlProp)
	{
		if (pCtrlProp == NULL)
		{
			return -1;
		}

		MoveWindow(&RECT(pCtrlProp->m_rcControl));

		RECT rcControl = pCtrlProp->m_rcControl;
		MakeControlMargin(GetHostWnd(), GetParent()->GetSafeHwnd(),
			&rcControl, m_hWnd->m_rcControlMargin);

		SetMouseTransparent((bool)pCtrlProp->m_bMouseTransparent);

		// 解析\r\n
		CIUIString strCaption = (LPCTSTR)pCtrlProp->m_strCaption;
		strCaption.Replace(_T("\\r"), _T("\r"));
		strCaption.Replace(_T("\\n"), _T("\n"));
		SetWindowText(strCaption);
		SetTextPadding(pCtrlProp->m_rcPadding4Text);

		// 背景颜色
		SetDrawBkColor((bool)pCtrlProp->m_bDrawBkColor);

		COLORREF cr[COMBINEIMAGESIZE2] = {0};
		GetProject()->GetControlBkColor2(pCtrlProp, cr);
		SetBkColor(cr[0], cr[1]);

		// 边框颜色
		SetDrawBorderColor((bool)pCtrlProp->m_bDrawBorderColor);
		GetProject()->GetControlBorderColor2(pCtrlProp, cr);
		SetBorderColor(cr[0], cr[1]);

		// 文本颜色
		GetProject()->GetControlTextColor2(pCtrlProp, cr);
		SetTextColor(cr[0], cr[1]);

		// 布局
		SetHorzLayoutMode((CONTROL_LAYOUT_HOR)pCtrlProp->m_eHorLayoutMode);
		SetVertLayoutMode((CONTROL_LAYOUT_VER)pCtrlProp->m_eVerLayoutMode);
		SetDockProperty((DOCK)pCtrlProp->m_eDockProperty);

		SetLayoutMargin(&(RECT)pCtrlProp->m_rcLayoutMargin);
		SetLayoutInset(&(RECT)pCtrlProp->m_rcInset);

		return 0;
	}

	BOOL CWLWnd::IsWindow() const
	{
		if (this == NULL)
		{
			return FALSE;
		}

		return IUI::IsWindow(m_hWnd);
	}

	CWLWnd *CWLWnd::SetParent(CWLWnd *pNewParent)
	{
		HWLWND hOld = IUI::SetParent(m_hWnd, pNewParent->m_hWnd);
		if (hOld == NULL)
		{
			return NULL;
		}

		return hOld->ci.pThis;
	}

	CWLWnd *CWLWnd::GetParent() const
	{
		HWLWND hParent = IUI::GetParent(m_hWnd);
		if (hParent == NULL)
		{
			return NULL;
		}

		return hParent->ci.pThis;
	}

	int CWLWnd::SetZOrder(CWLWnd *pAfter)
	{
		return IUI::SetZOrder(m_hWnd, pAfter->m_hWnd);
	}

	CIUIString CWLWnd::SetName(LPCTSTR pstrName)
	{
		return IUI::SetName(m_hWnd, pstrName);
	}

	CIUIString CWLWnd::GetName() const
	{
		return IUI::GetName(m_hWnd);
	}

	int CWLWnd::SetNotify(BOOL bNotify)
	{
		return IUI::SetNotify(m_hWnd, bNotify);
	}

	BOOL CWLWnd::IsNotify() const
	{
		return IUI::IsNotify(m_hWnd);
	}

	LONG CWLWnd::SetStyle(LONG lStyle)
	{
		return IUI::SetStyle(m_hWnd, lStyle);
	}

	LONG CWLWnd::GetStyle() const
	{
		return IUI::GetStyle(m_hWnd);
	}

	LONG CWLWnd::AddStyle(LONG lAdd)
	{
		return IUI::AddStyle(m_hWnd, lAdd);
	}

	LONG CWLWnd::RemoveStyle(LONG lRemove)
	{
		return IUI::RemoveStyle(m_hWnd, lRemove);
	}

	LONG CWLWnd::SetExStyle(LONG lExStyle)
	{
		return IUI::SetExStyle(m_hWnd, lExStyle);
	}

	LONG CWLWnd::GetExStyle() const
	{
		return IUI::GetExStyle(m_hWnd);
	}

	LONG CWLWnd::AddExStyle(LONG lAdd)
	{
		return IUI::AddExStyle(m_hWnd, lAdd);
	}

	LONG CWLWnd::RemoveExStyle(LONG lRemove)
	{
		return IUI::RemoveExStyle(m_hWnd, lRemove);
	}

	HWND CWLWnd::GetHostWnd() const
	{
		return IUI::GetHostWnd(m_hWnd);
	}

	CIUIString CWLWnd::GetClassName() const
	{
		return IUI::GetClassName(m_hWnd);
	}

	int CWLWnd::SetDlgCtrlID(int nID)
	{
		return IUI::SetDlgCtrlID(m_hWnd, nID);
	}

	int CWLWnd::GetDlgCtrlID() const
	{
		return IUI::GetDlgCtrlID(m_hWnd);
	}

	HWND CWLWnd::GetNativeWindow() const
	{
		return NULL;
	}

	CIUIProject *CWLWnd::GetProject()
	{
		return IUI::GetProject(m_hWnd);
	}

	const CIUIProject *CWLWnd::GetProject() const
	{
		return IUI::GetProject(m_hWnd);
	}

	HWLWND CWLWnd::SetCapture()
	{
		return IUI::SetCapture(m_hWnd);
	}

	CWLWnd *CWLWnd::GetTopLevelWindow()
	{
		HWLWND hTop = IUI::GetTopLevelWindow(m_hWnd);
		if (hTop == NULL)
		{
			return NULL;
		}

		return hTop->ci.pThis;
	}

	CIUIString CWLWnd::GetWindowText() const
	{
		return IUI::GetWindowText(m_hWnd);
	}

	void CWLWnd::SetWindowText(LPCTSTR pstrText)
	{
		IUI::SetWindowText(m_hWnd, pstrText);
	}

	void CWLWnd::SetResText(HINSTANCE hRes, UINT uID)
	{
		IUI::SetResText(m_hWnd, hRes, uID);
	}

#if (_MSC_VER > 1200)	// VC6.0
	void CWLWnd::SetXResText(HXSRESOURCE hXRes, LPCTSTR lpszSectionID, LPCTSTR lpszStringID)
	{
		IUI::SetXResText(m_hWnd, hXRes, lpszSectionID, lpszStringID);
	}
#endif // #if (_MSC_VER > 1200)	// VC6.0

	RECT CWLWnd::GetTextPadding() const
	{
		return IUI::GetTextPadding(m_hWnd);
	}

	int CWLWnd::SetTextPadding(RECT rc)
	{
		return IUI::SetTextPadding(m_hWnd, rc);
	}

	int CWLWnd::SetTextColor(COLORREF crN, COLORREF crD)
	{
		return IUI::SetTextColor(m_hWnd, crN, crD);
	}

	int CWLWnd::GetTextColor(COLORREF *pcrN, COLORREF *pcrD) const
	{
		return IUI::GetTextColor(m_hWnd, pcrN, pcrD);
	}

	BOOL CWLWnd::SetDrawBkColor(BOOL bDraw)
	{
		return IUI::SetDrawBkColor(m_hWnd, bDraw);
	}

	BOOL CWLWnd::IsDrawBkColor() const
	{
		return IUI::IsDrawBkColor(m_hWnd);
	}

	int CWLWnd::SetBkColor(COLORREF crBkN, COLORREF crBkD)
	{
		return IUI::SetBkColor(m_hWnd, crBkN, crBkD);
	}

	int CWLWnd::GetBkColor(COLORREF *pcrBkN, COLORREF *pcrBkD) const
	{
		return IUI::GetBkColor(m_hWnd, pcrBkN, pcrBkD);
	}

	BOOL CWLWnd::CombineBkImage(BOOL bCombine)
	{
		return IUI::CombineBkImage(m_hWnd, bCombine);
	}

	BOOL CWLWnd::IsCombineBkImage() const
	{
		return IUI::IsCombineBkImage(m_hWnd);
	}

	int CWLWnd::SetCombineBkImage(LPCTSTR lpszImage)
	{
		return IUI::SetCombineBkImage(m_hWnd, lpszImage);
	}

	int CWLWnd::GetCombineBkImage(CIUIString *pstrCombineImage) const
	{
		return IUI::GetCombineBkImage(m_hWnd, pstrCombineImage);
	}

	int CWLWnd::SetBkImageResizeMode(IMAGE_RESIZE_MODE eImageResizeMode)
	{
		return IUI::SetBkImageResizeMode(m_hWnd, eImageResizeMode);
	}

	IMAGE_RESIZE_MODE CWLWnd::GetBkImageResizeMode() const
	{
		return IUI::GetBkImageResizeMode(m_hWnd);
	}

	int CWLWnd::SetBkImage9GridResizeParam(LPCRECT lprcParam)
	{
		return IUI::SetBkImage9GridResizeParam(m_hWnd, lprcParam);
	}

	int CWLWnd::GetBkImage9GirdResizeParam(LPRECT lprcParam) const
	{
		return IUI::GetBkImage9GridResizeParam(m_hWnd, lprcParam);
	}

	BOOL CWLWnd::SetDrawBorderColor(BOOL bDraw)
	{
		return IUI::SetDrawBorderColor(m_hWnd, bDraw);
	}

	BOOL CWLWnd::IsDrawBorderColor() const
	{
		return IUI::IsDrawBorderColor(m_hWnd);
	}

	int CWLWnd::SetBorderColor(COLORREF crN, COLORREF crD)
	{
		return IUI::SetBorderColor(m_hWnd, crN, crD);
	}

	int CWLWnd::GetBorderColor(COLORREF *pcrN, COLORREF *pcrD) const
	{
		return IUI::GetBorderColor(m_hWnd, pcrN, pcrD);
	}

	DWORD CWLWnd::GetFocusBorderColor() const
	{
		return IUI::GetFocusBorderColor(m_hWnd);
	}

	void CWLWnd::SetFocusBorderColor(DWORD dwBorderColor)
	{
		IUI::SetFocusBorderColor(m_hWnd, dwBorderColor);
	}

	int CWLWnd::SetBorderRect(const RECT &rcBorder)
	{
		return IUI::SetBorderRect(m_hWnd, rcBorder);
	}

	CIUIRect CWLWnd::GetBorderRect() const
	{
		return IUI::GetBorderRect(m_hWnd);
	}

	SIZE CWLWnd::GetBorderRound() const
	{
		return IUI::GetBorderRound(m_hWnd);
	}

	void CWLWnd::SetBorderRound(SIZE cxyRound)
	{
		IUI::SetBorderRound(m_hWnd, cxyRound);
	}

	int CWLWnd::SetWindowRgn(HRGN hRgn, BOOL bRedraw)
	{
		return IUI::SetWindowRgn(m_hWnd, hRgn, bRedraw);
	}

	int CWLWnd::GetWindowRgn(HRGN hRgn) const
	{
		return IUI::GetWindowRgn(m_hWnd, hRgn);
	}

	int CWLWnd::RelayoutChild()
	{
		return IUI::RelayoutChild(m_hWnd);
	}

	void CWLWnd::EnableDPI(BOOL bEnable)
	{
		IUI::EnableDPI(m_hWnd, bEnable);
	}

	BOOL CWLWnd::IsDPIEnabled() const
	{
		return IUI::IsDPIEnabled(m_hWnd);
	}

	const CIUIRect &CWLWnd::GetWindowRectToParent() const
	{
		return IUI::GetWindowRectToParent(m_hWnd);
	}

	int CWLWnd::GetWindowRectToParent(LPRECT lprc) const
	{
		return IUI::GetWindowRectToParent(m_hWnd, lprc);
	}

	int CWLWnd::GetWindowRectFToParent(Gdiplus::RectF *pRect) const
	{
		return SendMessage(WM_WL_GETRECTF, 0, (LPARAM)pRect);
	}

	int CWLWnd::ClientToScreen(LPRECT lpRect)
	{
		return IUI::ClientToScreen(m_hWnd, lpRect);
	}

	int CWLWnd::ClientToScreen(LPPOINT lpPt)
	{
		return IUI::ClientToScreen(m_hWnd, lpPt);
	}

	int CWLWnd::ScreenToClient(LPRECT lpRect)
	{
		return IUI::ScreenToClient(m_hWnd, lpRect);
	}

	int CWLWnd::ScreenToClient(LPPOINT lpPt)
	{
		return IUI::ScreenToClient(m_hWnd, lpPt);
	}

	void CWLWnd::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint/* = TRUE*/)
	{
		IUI::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
	}

	void CWLWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint/* = TRUE*/)
	{
		IUI::MoveWindow(m_hWnd, lpRect, bRepaint);
	}

	BOOL CWLWnd::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
	{
		return IUI::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	}

	int CWLWnd::GetClientRect(LPRECT lpRect) const
	{
		return IUI::GetClientRect(m_hWnd, lpRect);
	}

	// 相对于屏幕
	int CWLWnd::GetWindowRect(LPRECT lpRect) const
	{
		return IUI::GetWindowRect(m_hWnd, lpRect);
	}

	// 相对于宿主窗口
	int CWLWnd::GetWindowRectToHWND(LPRECT lpRect) const
	{
		return IUI::GetWindowRectToHWND(m_hWnd, lpRect);
	}

	void CWLWnd::SetFixedWidth(int cx)
	{
		IUI::SetFixedWidth(m_hWnd, cx);
	}

	int CWLWnd::GetFixedWidth() const
	{
		return IUI::GetFixedWidth(m_hWnd);
	}

	void CWLWnd::SetFixedHeight(int cy)
	{
		IUI::SetFixedHeight(m_hWnd, cy);
	}

	int CWLWnd::GetFixedHeight() const
	{
		return IUI::GetFixedHeight(m_hWnd);
	}

	BOOL CWLWnd::IsFixedWidth() const
	{
		return IUI::IsFixedWidth(m_hWnd);
	}

	BOOL CWLWnd::IsFixedHeight() const
	{
		return IUI::IsFixedHeight(m_hWnd);
	}

	void CWLWnd::ClearFixedWidth()
	{
		IUI::ClearFixedWidth(m_hWnd);
	}

	void CWLWnd::ClearFixedHeight()
	{
		IUI::ClearFixedHeight(m_hWnd);
	}

	int CWLWnd::GetMinWidth() const
	{
		return IUI::GetMinWidth(m_hWnd);
	}

	void CWLWnd::SetMinWidth(int cx)
	{
		IUI::SetMinWidth(m_hWnd, cx);
	}

	int CWLWnd::GetMaxWidth() const
	{
		return IUI::GetMaxWidth(m_hWnd);
	}

	void CWLWnd::SetMaxWidth(int cx)
	{
		IUI::SetMaxWidth(m_hWnd, cx);
	}

	int CWLWnd::GetMinHeight() const
	{
		return IUI::GetMinHeight(m_hWnd);
	}

	void CWLWnd::SetMinHeight(int cy)
	{
		IUI::SetMinHeight(m_hWnd, cy);
	}

	int CWLWnd::GetMaxHeight() const
	{
		return IUI::GetMaxHeight(m_hWnd);
	}

	void CWLWnd::SetMaxHeight(int cy)
	{
		IUI::SetMaxHeight(m_hWnd, cy);
	}

	int CWLWnd::SetTitleBarHeight(int nHeight)
	{
		return IUI::SetTitleBarHeight(m_hWnd, nHeight);
	}

	int CWLWnd::GetTitleBarHeight() const
	{
		return IUI::GetTitleBarHeight(m_hWnd);
	}

	int CWLWnd::SetBorderThickness(LPCRECT lprcBorder)
	{
		return IUI::SetBorderThickness(m_hWnd, lprcBorder);
	}

	int CWLWnd::GetBorderThickness(LPRECT lprcBorder) const
	{
		return IUI::GetBorderThickness(m_hWnd, lprcBorder);
	}

	int CWLWnd::SetControlMargin(LPCRECT lprc)
	{
		return IUI::SetControlMargin(m_hWnd, lprc);
	}

	int CWLWnd::GetControlMargin(LPRECT lprc) const
	{
		return IUI::GetControlMargin(m_hWnd, lprc);
	}

	UINT CWLWnd::SetResizeFlag(UINT uFlag)
	{
		return IUI::SetResizeFlag(m_hWnd, uFlag);
	}

	UINT CWLWnd::GetResizeFlag() const
	{
		return IUI::GetResizeFlag(m_hWnd);
	}

	int CWLWnd::HitTest(LPPOINT lpPt)
	{
		return IUI::HitTest(m_hWnd, lpPt);
	}

	CWLWnd *CWLWnd::HitTestChild(LPPOINT lpPt)
	{
		HWLWND hRet = IUI::HitTestChild(m_hWnd, lpPt);
		if (hRet == NULL)
		{
			return NULL;
		}

		return hRet->ci.pThis;
	}

	BOOL CWLWnd::SetMouseTransparent(BOOL bMouseTransparent)
	{
		return IUI::SetMouseTransparent(m_hWnd, bMouseTransparent);
	}

	BOOL CWLWnd::IsMouseTransparent() const
	{
		return IUI::IsMouseTransparent(m_hWnd);
	}

	CONTROL_LAYOUT_HOR CWLWnd::SetHorzLayoutMode(CONTROL_LAYOUT_HOR eLayout)
	{
		return IUI::SetHorzLayoutMode(m_hWnd, eLayout);
	}

	CONTROL_LAYOUT_HOR CWLWnd::GetHorzLayoutMode() const
	{
		return IUI::GetHorzLayoutMode(m_hWnd);
	}

	CONTROL_LAYOUT_VER CWLWnd::SetVertLayoutMode(CONTROL_LAYOUT_VER eLayout)
	{
		return IUI::SetVertLayoutMode(m_hWnd, eLayout);
	}

	CONTROL_LAYOUT_VER CWLWnd::GetVertLayoutMode() const
	{
		return IUI::GetVertLayoutMode(m_hWnd);
	}

	DOCK CWLWnd::SetDockProperty(DOCK eDock)
	{
		return IUI::SetDockProperty(m_hWnd, eDock);
	}

	DOCK CWLWnd::GetDockProperty() const
	{
		return IUI::GetDockProperty(m_hWnd);
	}

	int CWLWnd::SetLayoutMargin(LPCRECT lprcMargin)
	{
		return IUI::SetLayoutMargin(m_hWnd, lprcMargin);
	}

	int CWLWnd::GetLayoutMargin(LPRECT lprcMargin) const
	{
		return IUI::GetLayoutMargin(m_hWnd, lprcMargin);
	}

	int CWLWnd::SetLayoutInset(LPCRECT lprcInset)
	{
		return IUI::SetLayoutInset(m_hWnd, lprcInset);
	}

	int CWLWnd::GetLayoutInset(LPRECT lprcInset) const
	{
		return IUI::GetLayoutInset(m_hWnd, lprcInset);
	}

	CIUIString CWLWnd::GetToolTip() const
	{
		return IUI::GetToolTip(m_hWnd);
	}

	void CWLWnd::SetToolTip(LPCTSTR pstrText)
	{
		IUI::SetToolTip(m_hWnd, pstrText);
	}

	void CWLWnd::SetResToolTip(HINSTANCE hRes, UINT uID)
	{
		IUI::SetResToolTip(m_hWnd, hRes, uID);
	}

#if (_MSC_VER > 1200)	// VC6.0
	void CWLWnd::SetXResToolTip(HXSRESOURCE hXRes, LPCTSTR lpszSectionID, LPCTSTR lpszStringID)
	{
		IUI::SetXResToolTip(m_hWnd, hXRes, lpszSectionID, lpszStringID);
	}
#endif // #if (_MSC_VER > 1200)	// VC6.0

	void CWLWnd::SetToolTipWidth(int nWidth)
	{
		IUI::SetToolTipWidth(m_hWnd, nWidth);
	}

	int CWLWnd::GetToolTipWidth(void)
	{
		return IUI::GetToolTipWidth(m_hWnd);
	}

	LPARAM CWLWnd::GetUserData()
	{
		return IUI::GetUserData(m_hWnd);
	}

	void CWLWnd::SetUserData(LPARAM lParam)
	{
		IUI::SetUserData(m_hWnd, lParam);
	}

	void CWLWnd::SetTag(LPCTSTR lpszTag)
	{
		IUI::SetTag(m_hWnd, lpszTag);
	}

	LPCTSTR CWLWnd::GetTag() const
	{
		return IUI::GetTag(m_hWnd);
	}

	UINT CWLWnd::SetState(UINT uState)
	{
		return IUI::SetState(m_hWnd, uState);
	}

	UINT CWLWnd::GetState() const
	{
		return IUI::GetState(m_hWnd);
	}

	UINT CWLWnd::AddState(UINT uState)
	{
		return IUI::AddState(m_hWnd, uState);
	}

	UINT CWLWnd::RemoveState(UINT uState)
	{
		return IUI::RemoveState(m_hWnd, uState);
	}

	BOOL CWLWnd::IsWindowVisible() const
	{
		return IsIncludeFlag(GetStyle(), WS_VISIBLE);
	}

	BOOL CWLWnd::ShowWindow(int nCmdShow)
	{
		return IUI::ShowWindow(m_hWnd, nCmdShow);
	}

	BOOL CWLWnd::IsWindowEnabled() const
	{
		return !IsIncludeFlag(GetStyle(), WS_DISABLED);
	}

	BOOL CWLWnd::EnableWindow(BOOL bEnabled)
	{
		return IUI::EnableWindow(m_hWnd, bEnabled);
	}

	BOOL CWLWnd::IsFocused() const
	{
		return IUI::IsFocused(m_hWnd);
	}

	void CWLWnd::SetFocus()
	{
		IUI::SetFocus(IUI::GetHostWnd(m_hWnd), m_hWnd);
	}

	int CWLWnd::SetControlState(CONTROL_STATE eNewState)
	{
		return IUI::SetControlState(m_hWnd, eNewState);
	}

	CONTROL_STATE CWLWnd::GetControlState() const
	{
		return IUI::GetControlState(m_hWnd);
	}

	CWLWnd *CWLWnd::FindControl(LPCTSTR lpszName)
	{
		HWLWND hFound = IUI::FindControl(m_hWnd, lpszName);
		if (hFound == NULL)
		{
			return NULL;
		}

		return hFound->ci.pThis;
	}

	BOOL CWLWnd::Invalidate()
	{
		return IUI::Invalidate(m_hWnd);
	}

	BOOL CWLWnd::InvalidateRect(LPCRECT lpRect)
	{
		return IUI::InvalidateRect(m_hWnd, lpRect);
	}

	UINT_PTR CWLWnd::SetTimer(UINT_PTR nIDEvent, UINT uElapse, WLTIMERPROC lpTimerFunc)
	{
		return IUI::SetTimer(m_hWnd, nIDEvent, uElapse, lpTimerFunc);
	}

	BOOL CWLWnd::KillTimer(UINT_PTR nIDEvent)
	{
		return IUI::KillTimer(m_hWnd, nIDEvent);
	}

	BOOL CWLWnd::IsTimerExist(UINT_PTR nIDEvent)
	{
		return IUI::IsTimerExist(m_hWnd, nIDEvent);
	}

	SIZE CWLWnd::EstimateSize(SIZE szAvailable)
	{
		return IUI::EstimateSize(m_hWnd);
	}

	int CWLWnd::Paint(HDC hDC, const RECT &rcPaint)
	{
		return SendMessage(WM_PRINT, (WPARAM)hDC, (LPARAM)(&rcPaint));
	}

	int CWLWnd::BeginAnimation(ANIMATION_TARGET_PROPERTY eTarget, Animation *pAnimation)
	{
		return IUI::BeginAnimation(m_hWnd, eTarget, pAnimation);
	}

	void CWLWnd::SetRedraw(BOOL bRedraw/* = true*/)
	{
		if (m_hWnd == NULL)
		{
			return;
		}

		IUI::SetRedraw(m_hWnd, bRedraw);
	}

	BOOL CWLWnd::IsRedraw()
	{
		return IUI::IsRedraw(m_hWnd);
	}

	int CWLWnd::SetClip(BOOL bClip)
	{
		return IUI::SetClip(m_hWnd, bClip);
	}

	BOOL CWLWnd::IsClip() const
	{
		return IUI::IsClip(m_hWnd);
	}

	LRESULT CWLWnd::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		_ASSERT(IsWindow());

		LRESULT lr = IUI::SendMessage(m_hWnd, uMsg, wParam, lParam);

		return lr;
	}

	LRESULT CWLWnd::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) const
	{
		_ASSERT(IsWindow());

		LRESULT lr = IUI::SendMessage(m_hWnd, uMsg, wParam, lParam);

		return lr;
	}

	LRESULT CWLWnd::OnCreate(RoutedEventArgs *pe)
	{
		return 0;
	}

	LRESULT CWLWnd::OnSize(RoutedEventArgs *pe)
	{
		return 0;
	}

	LRESULT CWLWnd::OnWindowPosChanged(RoutedEventArgs *pe)
	{
		return 0;
	}

	LRESULT CWLWnd::DefEventHandler(RoutedEventArgs *pe)
	{
		if (m_pfnSuper != NULL)
		{
			return IUI::CallEventHandler(m_pfnSuper, m_hWnd, pe);
		}

		_ASSERT(FALSE);
		return 0;
	}

	LRESULT CWLWnd::EventHandler(RoutedEventArgs *pe)
	{
		return DefEventHandler(pe);
	}

	LRESULT CWLWnd::CallIUIHandlerMap(RoutedEventArgs *pe)
	{
		return IUI::CallIUIHandlerMap(m_hWnd, pe);
	}

	LRESULT CWLWnd::RaiseEvent(RoutedEventArgs *pe)
	{
		return IUI::RaiseEvent(m_hWnd, pe);
	}

	void CWLWnd::EnableScrollBar(BOOL bEnableVertical, BOOL bEnableHorizontal)
	{
		IUI::EnableScrollBar(m_hWnd, bEnableVertical, bEnableHorizontal);
	}

	BOOL CWLWnd::IsFloatVBar() const
	{
		return IUI::IsFloatVBar(m_hWnd);
	}

	BOOL CWLWnd::IsFloatHBar() const
	{
		return IUI::IsFloatHBar(m_hWnd);
	}

	int CWLWnd::SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw/* = TRUE*/)
	{
		return IUI::SetScrollRange(m_hWnd, nBar, nMinPos, nMaxPos, bRedraw);
	}

	int CWLWnd::GetScrollRange(int nBar, int *pnMinPos, int *pnMaxPos)
	{
		return IUI::GetScrollRange(m_hWnd, nBar, pnMinPos, pnMaxPos);
	}

	int CWLWnd::SetScrollPos(int nBar, int nPos, BOOL bRedraw/* = TRUE*/)
	{
		return IUI::SetScrollPos(m_hWnd, nBar, nPos, bRedraw);
	}

	int CWLWnd::GetScrollPos(int nBar) const
	{
		return IUI::GetScrollPos(m_hWnd, nBar);
	}

	BOOL CWLWnd::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
	{
		return IUI::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
	}

	BOOL CWLWnd::GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask)
	{
		return IUI::GetScrollInfo(m_hWnd, nBar, lpScrollInfo);
	}

	void CWLWnd::ShowScrollBar(UINT nBar, BOOL bShow/* = TRUE*/)
	{
		IUI::ShowScrollBar(m_hWnd, nBar, bShow);
	}

	int CWLWnd::SetHScrollBarHeight(int nHeight)
	{
		return IUI::SetHScrollBarHeight(m_hWnd, nHeight);
	}

	int CWLWnd::GetHScrollBarHeight() const
	{
		return IUI::GetHScrollBarHeight(m_hWnd);
	}

	int CWLWnd::SetVScrollBarWidth(int nWidth)
	{
		return IUI::SetVScrollBarWidth(m_hWnd, nWidth);
	}

	int CWLWnd::GetVScrollBarWidth() const
	{
		return IUI::GetVScrollBarWidth(m_hWnd);
	}

	void CWLWnd::SetMember(void *pMember)
	{
		IUI::SetMember(m_hWnd, pMember);
	}

	void *CWLWnd::GetMember() const
	{
		return IUI::GetMember(m_hWnd);
	}

	void CWLWnd::EnableFloatScrollBar(BOOL bFloatVBar, BOOL bFloatHBar)
	{
		IUI::EnableFloatScrollBar(m_hWnd, bFloatVBar, bFloatHBar);
	}

	HWLWND CWLWnd::GetVerticalScrollBar() const
	{
		return IUI::GetVerticalScrollBar(m_hWnd);
	}

	HWLWND CWLWnd::GetHorizontalScrollBar() const
	{
		return IUI::GetHorizontalScrollBar(m_hWnd);
	}

	int IUI::HWLWND_BindStyle(HWLWND hWnd, const CControlProp *pCtrlProp)
	{
		int nRet = 0;
		if (pCtrlProp == NULL)
		{
			nRet = -1;
		}
		else
		{
			RECT rcControl = pCtrlProp->m_rcControl;
			MakeControlMargin(GetHostWnd(hWnd), GetParent(hWnd),
				&rcControl, hWnd->m_rcControlMargin);

			SetMouseTransparent(hWnd, (bool)pCtrlProp->m_bMouseTransparent);

			SetTextPadding(hWnd, pCtrlProp->m_rcPadding4Text);

			// 背景颜色
			SetDrawBkColor(hWnd, (bool)pCtrlProp->m_bDrawBkColor);

			COLORREF cr[COMBINEIMAGESIZE2] = { 0 };
			GetProject(hWnd)->GetControlBkColor2(pCtrlProp, cr);
			SetBkColor(hWnd, cr[0], cr[1]);

			// 边框颜色
			SetDrawBorderColor(hWnd, (bool)pCtrlProp->m_bDrawBorderColor);
			GetProject(hWnd)->GetControlBorderColor2(pCtrlProp, cr);
			SetBorderColor(hWnd, cr[0], cr[1]);

			// 文本颜色
			GetProject(hWnd)->GetControlTextColor2(pCtrlProp, cr);
			SetTextColor(hWnd, cr[0], cr[1]);

			// 布局
			SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pCtrlProp->m_eHorLayoutMode);
			SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pCtrlProp->m_eVerLayoutMode);
			SetDockProperty(hWnd, (DOCK)pCtrlProp->m_eDockProperty);

			SetLayoutMargin(hWnd, &(RECT)pCtrlProp->m_rcLayoutMargin);
			SetLayoutInset(hWnd, &(RECT)pCtrlProp->m_rcInset);

			// 创建绑定的子控件
			CIUIString strBindXml = (LPCTSTR)pCtrlProp->m_strBindXml;
			if (!strBindXml.IsEmpty())
			{
				CControlProp *pProp = GetProject(hWnd)->FindProjectItem(strBindXml);
				if (NULL != pProp)
				{
					CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
					if (NULL != pBindProp)
					{
						HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
								hWnd, GetHostWnd(hWnd), 0);
					}
					else
					{
						IUITRACE(_T("===[IUI] 控件绑定的XML'%s'中未解析出子控件"),
							(LPCTSTR)strBindXml);
					}
				}
				else
				{
					IUITRACE(_T("===[IUI] 绑定的控件无效！"));
				}
			}
		}

		return nRet;
	}

} // namespace IUI

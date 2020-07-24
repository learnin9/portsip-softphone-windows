#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


int IUI::FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr, BOOL bUseGdiplus)
{
	_ASSERT(hDC != NULL);
	_ASSERT(lpRect != NULL);

	if (bUseGdiplus)
	{
		Gdiplus::Graphics g(hDC);
		// TODO: 发现如果Alpha值填0xFF的话，会导致目标区域全透明
		Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetRValue(clr), GetGValue(clr), GetBValue(clr)));
		g.FillRectangle(&brush, lpRect->left, lpRect->top,
			RECTWIDTH(*lpRect), RECTHEIGHT(*lpRect));
	}
	else
	{
		HBRUSH hBrush = CreateSolidBrush(clr);
		FillRect(hDC, lpRect, hBrush);
		DeleteObject(hBrush);
	}

	return 0;
}

int IUI::FillSolidRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr, BOOL bUseGdiplus)
{
	CIUIRect rect(x, y, x + cx, y + cy);
	return FillSolidRect(hDC, rect, clr, bUseGdiplus);
}

FILE *IUI::TFOPEN(LPCTSTR filename, LPCTSTR mode)
{
#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
	return _tfopen(filename, mode);
#else
	FILE *p = NULL;
	errno_t t = _tfopen_s(&p, filename, mode);

	if (t == 0)
	{
		return p;
	}
	else
	{
		return NULL;
	}

#endif // (_MSC_VER <= 1310)
}

// 得到在XML中的控件类型
// bInCode: TRUE: 返回在代码中的类名；FALSE: 返回在XML中的类名
// 对于CheckBox，在代码中的类名为Button，而是XML中，为CheckBox
CIUIString IUI::GetControlTypeString(CONTROL_TYPE eControlType, BOOL bInCode/* = FALSE*/)
{
	CIUIString strType;

	switch (eControlType)
	{
	case CT_ARROW:
	{
		strType = _T("ARROW");
	}
	break;

	case CT_PUSHBUTTON:
	{
		strType = IUI_BUTTON;
	}
	break;

	case CT_CHECK:
	{
		if (bInCode)
		{
			strType = IUI_BUTTON;
		}
		else
		{
			strType = _T("CheckBox");
		}
	}
	break;

	case CT_RADIO:
	{
		if (bInCode)
		{
			strType = IUI_BUTTON;
		}
		else
		{
			strType = _T("RadioButton");
		}
	}
	break;

	case CT_COMBOBOX:
	{
		strType = IUI_COMBOBOX;
	}
	break;

	case CT_STATIC:
	{
		strType = IUI_LABEL;
	}
	break;

	case CT_SLIDER:
	{
		strType = _T("Slider");		// same to wpf
	}
	break;

	case CT_PROGRESS:
	{
		strType = _T("ProgressBar");	// same to wpf
	}
	break;

	case CT_SPIN:
	{
		strType = _T("SPIN");
	}
	break;

	case CT_HOTKEY:
	{
		strType = _T("HOTKEY");
	}
	break;

	case CT_IPADDRESS:
	{
		strType = _T("IPADDRESS");
	}
	break;

	case CT_LISTVIEW:
	{
		strType = IUI_LISTVIEW;
	}
	break;

	case CT_TREEVIEW:
	{
		strType = _T("TreeView");	// same to wpf
	}
	break;

	case CT_SCROLLBAR:
	{
		strType = _T("SCROLLBAR");
	}
	break;

	case CT_TREELIST:
	{
		strType = _T("TREELIST");
	}
	break;

	case CT_RICHEDIT:
	{
		strType = IUI_RICHTEXTBOX;	// same to wpf
	}
	break;

	case CT_IM:
	{
		strType = IUI_IM;
	}
	break;

	case CT_HTMLVIEW:
	{
		strType = IUI_HTMLVIEW;
	}
	break;

	case CT_SPLITTER:
	{
		strType = _T("SPLITTER");
	}
	break;

	case CT_DATETIMEPICKER:
	{
		strType = _T("DatePicker");	// same to wpf
	}
	break;

	case CT_MENU:
	{
		strType = _T("MENU");
	}
	break;

	case CT_MENUBAR:
	{
		strType = _T("MENUBAR");
	}
	break;

	case CT_HEADERCTRL:
	{
		strType = IUI_COLUMNHEADER;
	}
	break;

	case CT_PNL_TABCONTROL:
	{
		strType = _T("TabControl");	// same to wpf
	}
	break;

	case CT_PNL_CANVAS:
	{
		strType = IUI_CANVAS;
	}
		// Panel
	break;

	case CT_PNL_DOCK:
	{
		strType = _T("DockPanel");	// same to wpf
	}
	break;

	case CT_UNIFORM_GRID:
	{
		strType = _T("UniformGrid");	// same to wpf
	}
	break;

	case CT_PNL_STACK:
	{
		strType = _T("StackPanel");	// same to wpf
	}
	break;

	case CT_PNL_TILE:
	{
		strType = _T("TilePanel");
	}

	break;

	case CT_UIWND:
	{
		strType = IUI_WINDOW;
	}
		// Resource
	break;

	case CT_RESOURCE_DEFAULT_FONT:
	{
		strType = _T("DEFAULT_FONT");
	}
	break;

	case CT_RESOURCE_FONT:
	{
		strType = _T("FONT");
	}
	break;

	case CT_RESOURCE_DEFAULT_COLOR:
	{
		strType = _T("DEFAULT_COLOR");
	}
	break;

	case CT_RESOURCE_COLOR:
	{
		strType = _T("COLOR");
	}
	break;

	case CT_RESOURCE_STRING:
		strType = _T("STRING");
		break;

	case CT_STYLE_PUSHBUTTON:
	{
		strType = _T("STYLE_PUSHBUTTON");
	}
	break;

	case CT_STYLE_CHECK:
	{
		strType = _T("STYLE_CHECK");
	}
	break;

	case CT_STYLE_RADIO:
	{
		strType = _T("STYLE_RADIO");
	}
	break;

	case CT_STYLE_COMBOBOX:
	{
		strType = _T("STYLE_COMBOBOX");
	}
	break;

	case CT_STYLE_STATIC:
	{
		strType = _T("STYLE_STATIC");
	}
	break;

	case CT_STYLE_RICHEDIT:
	{
		strType = _T("STYLE_RICHEDIT");
	}
	break;

	case CT_STYLE_SLIDER:
	{
		strType = _T("STYLE_SLIDER");
	}
	break;

	case CT_STYLE_PROGRESS:
	{
		strType = _T("STYLE_PROGRESS");
	}
	break;

	case CT_STYLE_SPIN:
	{
		strType = _T("STYLE_SPIN");
	}
	break;

	case CT_STYLE_HOTKEY:
	{
		strType = _T("STYLE_HOTKEY");
	}
	break;

	case CT_STYLE_IPADDRESS:
	{
		strType = _T("STYLE_IPADDRESS");
	}
	break;

	case CT_STYLE_LISTCTRL:
	{
		strType = _T("STYLE_LISTCTRL");
	}
	break;

	case CT_STYLE_TREECTRL:
	{
		strType = _T("STYLE_TREECTRL");
	}
	break;

	case CT_STYLE_TREELIST:
	{
		strType = _T("STYLE_TREELIST");
	}
	break;

	case CT_STYLE_HTMLCTRL:
	{
		strType = _T("STYLE_HTMLCTRL");
	}
	break;

	case CT_STYLE_DATETIMECTRL:
	{
		strType = _T("STYLE_DATETIMECTRL");
	}
	break;

	case CT_STYLE_MENU:
	{
		strType = _T("STYLE_MENU");
	}
	break;

	case CT_STYLE_MENUBAR:
	{
		strType = _T("STYLE_MENUBAR");
	}
	break;

	case CT_STYLE_HEADERCTRL:
	{
		strType = _T("STYLE_HEADERCTRL");
	}
	break;

	case CT_STYLE_SCROLLBAR:
	{
		strType = _T("STYLE_SCROLLBAR");
	}
	break;

	case CT_STYLE_TOOLTIPS:
	{
		strType = _T("STYLE_TOOLTIPS");
	}
	break;

	case CT_STYLE_SPLITTER:
	{
		strType = _T("STYLE_SPLITTER");
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return strType;
}

BOOL IUI::ExtractSubString(CIUIString &rString, LPCTSTR lpszFullString, int iSubString, TCHAR chSep)
{
	if (lpszFullString == NULL)
	{
		return FALSE;
	}

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
	_ASSERT(nLen >= 0);
	IUICopyMemory(rString.GetBufferSetLength(nLen), nLen * sizeof(TCHAR),
		lpszFullString, nLen * sizeof(TCHAR));
	rString.ReleaseBuffer();	// Need to call ReleaseBuffer
	// after calling GetBufferSetLength
	return TRUE;
}

int IUI::LoadCustomResourceA(LPCSTR lpszResName, LPCSTR lpType, LPVOID *lpData, DWORD *pdwSize)
{
	HRSRC hRsrc = FindResourceA(NULL, lpszResName, lpType);
	if (NULL == hRsrc)
	{
		return -1;
	}

	DWORD dwSize = SizeofResource(NULL, hRsrc);
	if (0 == dwSize)
	{
		return -2;
	}

	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		return -3;
	}

	LPVOID pBuffer = LockResource(hGlobal);
	if (NULL == pBuffer)
	{
		return -4;
	}

	*lpData = pBuffer;
	*pdwSize = dwSize;

	return 0;
}

BOOL IUI::IsIncludeFlag(UINT uFlags, UINT uFlag)
{
	if (uFlag == 0)
	{
		// ==优先级比&高，所以要加上括号
		if ((uFlags & 1) == 1)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	BOOL bInclude = ((uFlags & uFlag) == uFlag);

	return bInclude;
}

int IUI::IUIClientToScreen(HWND hWnd, LPRECT lpRect)
{
	if (!::IsWindow(hWnd) || lpRect == NULL)
	{
		return -1;
	}

	BOOL bRet = ::ClientToScreen(hWnd, (LPPOINT)lpRect);
	bRet = ::ClientToScreen(hWnd, ((LPPOINT)lpRect) + 1);

	return 0;
}

int IUI::IUIScreenToClient(HWND hWnd, LPRECT lpRect)
{
	if (!::IsWindow(hWnd) || lpRect == NULL)
	{
		return -1;
	}

	::ScreenToClient(hWnd, (LPPOINT)lpRect);
	::ScreenToClient(hWnd, ((LPPOINT)lpRect) + 1);

	return 0;
}

CIUIString IUI::StringFromVariant(const _variant_t &rVariant)
{
	CIUIString strRet;

	if (rVariant.vt == VT_NULL)
	{
		return strRet;
	}

	strRet = (TCHAR *)_bstr_t(rVariant);

	return strRet;
}

// UTF16 means Unicode or wide char.
std::wstring IUI::MultiByteToUTF16(LPCSTR lpMultiByteStr, int cbMultiByte, UINT uCodePage)
{
	std::wstring wstrResult;

	// First call to get the required wide char length. not include the terminating null character.
	int cchWideChar = ::MultiByteToWideChar(uCodePage, 0, lpMultiByteStr, cbMultiByte, NULL, 0);

	if (cchWideChar > 0)
	{
		// must +1, MSDN is wrong, the return value of the required buffer size, in WCHAR values not include the terminating null character.
		wchar_t *lpWideCharStr = new wchar_t[cchWideChar + 1];

		// Second call to transform the string to utf-16 format.
		cchWideChar = ::MultiByteToWideChar(uCodePage, 0, lpMultiByteStr, cbMultiByte, lpWideCharStr,
				cchWideChar);

		// Make the last wide char to L'\0'.
		if (cchWideChar > 0)
		{
			lpWideCharStr[cchWideChar] = L'\0';
			wstrResult = lpWideCharStr;
		}

		// Clear memory.
		delete []lpWideCharStr;
	}

	return wstrResult;
}

BOOL IUI::IsStringExist(const std::vector<CIUIString> *pvStrings, const CIUIString &rString, BOOL bNoCase)
{
	BOOL bFind = FALSE;

	size_t nCount = pvStrings->size();

	for (size_t i = 0; i < nCount; ++i)
	{
		CIUIString strString = (*pvStrings)[i];

		if (bNoCase)
		{
			if (strString.CompareNoCase(rString) == 0)
			{
				bFind = TRUE;
				break;
			}
		}
		else
		{
			if (strString.Compare(rString) == 0)
			{
				bFind = TRUE;
				break;
			}
		}
	}

	return bFind;
}

#ifndef DI_GETDRAGIMAGE
#define DI_GETDRAGIMAGE TEXT("ShellGetDragImage")
#endif // DI_GETDRAGIMAGE

GLOBALVAR::GLOBALVAR()
{
	g_fSmoothScroll = FALSE;

	c_szNULL = TEXT("");
	c_szEllipses = TEXT("...");
	c_szHeaderClass = WC_HEADER;
	c_szSelect = TEXT("CCSelect");
	c_szSpace = TEXT(" ");

	g_clrBtnFace = GetSysColor(COLOR_BTNFACE);
	g_clrWindow = GetSysColor(COLOR_WINDOW);
	g_clrHighlight = GetSysColor(COLOR_HIGHLIGHT);
	g_clrHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	g_clrBtnText = GetSysColor(COLOR_BTNTEXT);
	g_clrWindowText = GetSysColor(COLOR_WINDOWTEXT);
	g_clrGrayText = GetSysColor(COLOR_GRAYTEXT);
	g_hbrHighlight = GetSysColorBrush(COLOR_HIGHLIGHT);
	g_hbrBtnFace = GetSysColorBrush(COLOR_BTNFACE);
	g_hbrWindow = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	g_cxBorder = GetSystemMetrics(SM_CXBORDER);
	g_cyBorder = GetSystemMetrics(SM_CYBORDER);
	g_cxScrollbar = GetSystemMetrics(SM_CXVSCROLL);
	g_cyScrollbar = GetSystemMetrics(SM_CYVSCROLL);
	g_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	g_cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
	g_cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
	g_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	g_cxIcon = GetSystemMetrics(SM_CXICON);
	g_cyIcon = GetSystemMetrics(SM_CYICON);
	g_cxEdge = GetSystemMetrics(SM_CXEDGE);
	g_cyEdge = GetSystemMetrics(SM_CYEDGE);
	g_cxIconSpacing = GetSystemMetrics(SM_CXICONSPACING);
	g_cyIconSpacing = GetSystemMetrics(SM_CYICONSPACING);
	g_cxFrame = GetSystemMetrics(SM_CXFRAME);
	g_cyFrame = GetSystemMetrics(SM_CYFRAME);

	g_cxLabelMargin = 8;
	g_uDragImages = RegisterWindowMessage(DI_GETDRAGIMAGE);
	g_msgMSWheel = RegisterWindowMessage(TEXT("MSWHEEL_ROLLMSG"));
	g_fDBCSInputEnabled = GetSystemMetrics(SM_DBCSENABLED);
	g_fDBCSEnabled = g_fDBCSInputEnabled;
	gcWheelDelta = 0;
	g_ucScrollLines = 3;                        /* default */
	g_cxIconMargin = 0;
	g_cyIconMargin = 0;

	dwExStyleRTLMirrorWnd = WS_EX_LAYOUTRTL;
	g_uiACP = CP_ACP;
	g_cyLabelSpace = g_cyIconMargin + 8;
	g_hSharedHeap = NULL;

	g_clrInfoText = GetSysColor(COLOR_INFOTEXT);
	g_clrInfoBk = GetSysColor(COLOR_INFOBK);

	g_cxDoubleClk = GetSystemMetrics(SM_CXDOUBLECLK);
	g_hbrWindowText = CreateSolidBrush(RGB(0, 0, 0));
	g_hbrGrayText = CreateSolidBrush(RGB(150, 150, 150));
	g_hfontSystem = NULL;
}

GLOBALVAR::~GLOBALVAR()
{
	::DeleteObject(g_hbrWindowText);
	::DeleteObject(g_hbrGrayText);
	::DeleteObject(g_hbrWindow);
}

GLOBALVAR IUI_g_global_var;
HGLOBALVAR IUI::IUIGetGloabelVar()
{
	return &IUI_g_global_var;
}

CIUIString IUI::GetRunFolder()
{
	CIUIString strPath;
	TCHAR *lpszPath = strPath.GetBufferSetLength(MAX_PATH * 4);
	GetModuleFileName(NULL, lpszPath, MAX_PATH * 4);

	PathRemoveFileSpec(lpszPath);

	PathAddBackslash(lpszPath);
	strPath.ReleaseBuffer();

	return strPath;
}

CIUIString IUI::GetFilePath(LPCTSTR lpszFullPath)
{
	TCHAR szBuf[MAX_PATH] = { 0 };
	StringCchCopy(szBuf, MAX_PATH, lpszFullPath);
	PathRemoveFileSpec(szBuf);
	PathAddBackslash(szBuf);
	CIUIString strRet;
	strRet = szBuf;

	return strRet;
}

// return title and ext.
CIUIString IUI::GetFileName(LPCTSTR lpszFullPath)
{
	CIUIString strRet;

	TCHAR szTitle[MAX_PATH] = {0};
	TCHAR szExt[MAX_PATH] = {0};
	TSPLITPATH(lpszFullPath, NULL, 0, NULL, 0, szTitle, MAX_PATH, szExt, MAX_PATH);
	strRet = szTitle;
	strRet += szExt;

	return strRet;
}

// return only title, not include ext.
CIUIString IUI::IUIGetFileTitle(LPCTSTR lpszFullPath)
{
	CIUIString strRet;
	TCHAR szName[MAX_PATH] = {0};
	TSPLITPATH(lpszFullPath, NULL, 0, NULL, 0, szName, MAX_PATH, NULL, 0);
	strRet = szName;
	return strRet;
}

std::wstring IUI::GetFileTitleExW(LPCWSTR lpszFile)
{
	std::wstring wstrRet;
	if (lpszFile == NULL)
	{
		return wstrRet;
	}

	WCHAR szName[MAX_PATH] = { 0 };
#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
	_wsplitpath(lpszFile, NULL, NULL, szName, NULL);
#else
	_wsplitpath_s(lpszFile, NULL, 0, NULL, 0, szName, MAX_PATH, NULL, 0);
#endif // (_MSC_VER <= 1310)

	wstrRet = szName;
	return wstrRet;
}

std::wstring IUI::GetFileExtW(LPCWSTR lpszFile, BOOL bHasDot)
{
	std::wstring wstrRet;
	if (lpszFile == NULL)
	{
		return wstrRet;
	}

	WCHAR szExt[MAX_PATH] = { 0 };
#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
	_wsplitpath(lpszFile, NULL, NULL, NULL, szExt);
#else
	_wsplitpath_s(lpszFile, NULL, 0, NULL, 0, NULL, 0, szExt, MAX_PATH);
#endif // (_MSC_VER <= 1310)

	wstrRet = szExt;

	// Remove dot
	if (!bHasDot && !wstrRet.empty() && wstrRet[0] == '.')
	{
		wstrRet.erase(wstrRet.begin());
	}

	return wstrRet;
}

int IUI::CanvasLayout(
	LPCRECT lprcCtrlOld,
	CONTROL_LAYOUT_HOR eControlLayoutHor,
	CONTROL_LAYOUT_VER eControlLayoutVer,
	LPCRECT lprcCtrlMargin,
	LPCRECT lprcLayoutMargin,
	int nParentWidth,
	int nParentHeight,
	OUT LPRECT lprcNew)
{
	if (NULL == lprcCtrlOld || NULL == lprcNew)
	{
		return -1;
	}

	// 只有Fill对齐模式才会改变控件的尺寸，而Fill对齐时，也不需要控件的原始宽高
	// 所以，我们不需要保存控件的原始坐标
	CIUIRect rcOld = lprcCtrlOld;
	*lprcNew = rcOld;

	CIUIRect rcControlMargin;
	if (NULL != lprcCtrlMargin)
	{
		rcControlMargin = lprcCtrlMargin;
	}
	CIUIRect rcLayoutMargin;
	if (NULL != lprcLayoutMargin)
	{
		rcLayoutMargin = lprcLayoutMargin;
	}

	// Horizontal
	switch (eControlLayoutHor)
	{
	case CLH_LEFT:
	{
		lprcNew->left = rcLayoutMargin.left + rcControlMargin.left;
		lprcNew->right = lprcNew->left + rcOld.Width();
	}
	break;

	case CLH_FREE:
		// Keep
		break;

	case CLH_RIGHT:
	{
		lprcNew->right = nParentWidth - rcLayoutMargin.right - rcControlMargin.right;
		lprcNew->left = lprcNew->right - rcOld.Width();
	}
	break;

	case CLH_CENTER:
	{
		lprcNew->left = (rcLayoutMargin.left + nParentWidth - rcOld.Width()) / 2;
		lprcNew->right = lprcNew->left + rcOld.Width();
	}
	break;

	case CLH_FILL:
	{
		lprcNew->left = rcLayoutMargin.left + rcControlMargin.left;
		lprcNew->right = nParentWidth - rcLayoutMargin.right - rcControlMargin.right;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	// Vertical
	switch (eControlLayoutVer)
	{
	case CLV_TOP:
	{
		lprcNew->top = rcLayoutMargin.top + rcControlMargin.top;
		lprcNew->bottom = lprcNew->top + rcOld.Height();
	}
	break;

	case CLV_FREE:
		// Keep
		break;

	case CLV_BOTTOM:
	{
		lprcNew->bottom = nParentHeight - rcLayoutMargin.bottom - rcControlMargin.bottom;
		lprcNew->top = lprcNew->bottom - rcOld.Height();
	}
	break;

	case CLV_CENTER:
	{
		lprcNew->top = (rcLayoutMargin.top + nParentHeight - rcOld.Height()) / 2;
		lprcNew->bottom = lprcNew->top + rcOld.Height();
	}
	break;

	case CLV_FILL:
	{
		lprcNew->top = rcLayoutMargin.top + rcControlMargin.top;
		lprcNew->bottom = nParentHeight - rcLayoutMargin.bottom - rcControlMargin.bottom;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return 0;
}

int IUI::CanvasLayoutElement(
	HWLWND hCtrl,
	int nParentWidth,
	int nParentHeight,
	OUT CIUIRect &rcNew)
{
	if (!IsWindow(hCtrl))
	{
		return -1;
	}

	if (!IsWindowVisible(hCtrl))
	{
		return 1;
	}

	// 只有Fill对齐模式才会改变控件的尺寸，而Fill对齐时，也不需要控件的原始宽高
	// 所以，我们不需要保存控件的原始坐标
	CIUIRect rcOld;
	GetWindowRectToParent(hCtrl, rcOld);
	rcNew = rcOld;

	CIUIRect rcControlMargin;
	GetControlMargin(hCtrl, rcControlMargin);

	CIUIRect rcLayoutMargin;
	GetLayoutMargin(hCtrl, rcLayoutMargin);

	// Horizontal
	CONTROL_LAYOUT_HOR eHorLayoutMode = GetHorzLayoutMode(hCtrl);
	switch (eHorLayoutMode)
	{
	case CLH_LEFT:
	{
		rcNew.left = rcLayoutMargin.left + rcControlMargin.left;
		rcNew.right = rcNew.left + rcOld.Width();
	}
	break;

	case CLH_FREE:
		// Keep
		break;

	case CLH_RIGHT:
	{
		rcNew.right = nParentWidth - rcLayoutMargin.right - rcControlMargin.right;
		rcNew.left = rcNew.right - rcOld.Width();
	}
	break;

	case CLH_CENTER:
	{
		rcNew.left = (rcLayoutMargin.left + nParentWidth - rcOld.Width()) / 2;
		rcNew.right = rcNew.left + rcOld.Width();
	}
	break;

	case CLH_FILL:
	{
		rcNew.left = rcLayoutMargin.left + rcControlMargin.left;
		rcNew.right = nParentWidth - rcLayoutMargin.right - rcControlMargin.right;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	// Vertical
	CONTROL_LAYOUT_VER eVerLayoutMode = GetVertLayoutMode(hCtrl);
	switch (eVerLayoutMode)
	{
	case CLV_TOP:
	{
		rcNew.top = rcLayoutMargin.top + rcControlMargin.top;
		rcNew.bottom = rcNew.top + rcOld.Height();
	}
	break;

	case CLV_FREE:
		// Keep
		break;

	case CLV_BOTTOM:
	{
		rcNew.bottom = nParentHeight - rcLayoutMargin.bottom - rcControlMargin.bottom;
		rcNew.top = rcNew.bottom - rcOld.Height();
	}
	break;

	case CLV_CENTER:
	{
		rcNew.top = (rcLayoutMargin.top + nParentHeight - rcOld.Height()) / 2;
		rcNew.bottom = rcNew.top + rcOld.Height();
	}
	break;

	case CLV_FILL:
	{
		rcNew.top = rcLayoutMargin.top + rcControlMargin.top;
		rcNew.bottom = nParentHeight - rcLayoutMargin.bottom - rcControlMargin.bottom;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return 0;
}

int IUI::CanvasLayoutCtrl(HWLWND hWnd)
{
	if (!IsWindowVisible(hWnd))
	{
		return 1;
	}

	HWLWND hParent = GetParent(hWnd);
	int nCurParentWidth = 0;
	int nCurParentHeight = 0;
	if (hParent == NULL)
	{
		CIUIRect rcParent;
		::GetClientRect(GetHostWnd(hWnd), rcParent);
		nCurParentWidth = rcParent.Width();
		nCurParentHeight = rcParent.Height();
	}
	else
	{
		CIUIRect rcParent;
		GetWindowRectToParent(hParent, rcParent);
		nCurParentWidth = rcParent.Width();
		nCurParentHeight = rcParent.Height();
	}

	CIUIRect rcNew;
	CanvasLayoutElement(hWnd, nCurParentWidth, nCurParentHeight, rcNew);
	MoveWindow(hWnd, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), FALSE);

	return 0;
}

int IUI::CanvasLayoutWindow(HWND hWnd, BOOL bExcludeTaskBar, __out LPRECT lprcNew)
{
	if (lprcNew == NULL)
	{
		return -1;
	}

	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	if (hMonitor == NULL)
	{
		return -2;
	}

	CONTROL_LAYOUT_HOR eHorLayoutMode = (CONTROL_LAYOUT_HOR)::SendMessage(hWnd,
			WM_GET_HORZ_LAYOUT_MODE, 0, 0);
	CONTROL_LAYOUT_VER eVerLayoutMode = (CONTROL_LAYOUT_VER)::SendMessage(hWnd,
			WM_GET_VERT_LAYOUT_MODE, 0, 0);

	CIUIRect rcMargin;
	::SendMessage(hWnd, WM_GET_LAYOUT_MARGIN, 0, (LPARAM)(LPRECT)rcMargin);

	CIUIRect rcView;
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &mi);

	if (bExcludeTaskBar)
	{
		rcView = mi.rcWork;
	}
	else
	{
		rcView = mi.rcMonitor;
	}

	CIUIRect rcWnd;
	::GetWindowRect(hWnd, rcWnd);

	// Horizontal
	CIUIRect rcNew = rcView;
	if (eHorLayoutMode == CLH_LEFT)
	{
		rcNew.left += rcMargin.left;
		rcNew.right = rcNew.left + rcWnd.Width();
	}
	else if (eHorLayoutMode == CLH_FREE)
	{
		// Keep
		rcNew.left = rcWnd.left;
		rcNew.right = rcWnd.right;
	}
	else if (eHorLayoutMode == CLH_RIGHT)
	{
		rcNew.right -= rcMargin.right;
		rcNew.left = rcNew.right - rcWnd.Width();
	}
	else if (eHorLayoutMode == CLH_CENTER)
	{
		rcNew.left += ((rcMargin.left + rcView.Width() - rcWnd.Width()) / 2);
		rcNew.right = rcNew.left + rcWnd.Width();
	}
	else if (eHorLayoutMode == CLH_FILL)
	{
		rcNew.left += rcMargin.left;
		rcNew.right -= rcMargin.right;
	}
	else
	{
		_ASSERT(FALSE);
	}

	// Vertical
	if (eVerLayoutMode == CLV_TOP)
	{
		rcNew.top += rcMargin.top;
		rcNew.bottom = rcNew.top + rcWnd.Height();
	}
	else if (eVerLayoutMode == CLV_FREE)
	{
		// Keep
		rcNew.top = rcWnd.top;
		rcNew.bottom = rcWnd.bottom;
	}
	else if (eVerLayoutMode == CLV_BOTTOM)
	{
		rcNew.bottom -= rcMargin.bottom;
		rcNew.top = rcNew.bottom - rcWnd.Height();
	}
	else if (eVerLayoutMode == CLV_CENTER)
	{
		rcNew.top = ((rcMargin.top + rcView.Height() - rcWnd.Height()) / 2);
		rcNew.bottom = rcNew.top + rcWnd.Height();
	}
	else if (eVerLayoutMode == CLV_FILL)
	{
		rcNew.top += rcMargin.top;
		rcNew.bottom -= rcMargin.bottom;
	}
	else
	{
		_ASSERT(FALSE);
	}

	*lprcNew = rcNew;

	return 0;
}

int StackToHorizontal(
	HWLWND hPrevCtrl,
	HWLWND hCurCtrl,
	int nParentHeight,
	BOOL bLeft,
	OUT CIUIRect &rcNew)
{
	// 得到上一个可视控件的坐标
	CIUIRect rcPrev;
	GetWindowRectToParent(hPrevCtrl, rcPrev);

	// 忽略控件Margin，只使用Layout Margin
	CIUIRect rcPrevCtrlLayoutMargin;
	GetLayoutMargin(hPrevCtrl, rcPrevCtrlLayoutMargin);

	CIUIRect rcLayoutMargin;
	GetLayoutMargin(hCurCtrl, rcLayoutMargin);

	CIUIRect rcOld;
	GetWindowRectToParent(hCurCtrl, rcOld);

	rcNew = rcOld;
	if (bLeft)
	{
		rcNew.left = rcPrev.right + rcPrevCtrlLayoutMargin.right + rcLayoutMargin.left;
		rcNew.right = rcNew.left + rcOld.Width();
	}
	else
	{
		rcNew.right = rcPrev.left - rcPrevCtrlLayoutMargin.left - rcLayoutMargin.right;
		rcNew.left = rcNew.right - rcOld.Width();
	}

	// Vertical
	CONTROL_LAYOUT_VER eVerLayoutMode = GetVertLayoutMode(hCurCtrl);
	switch (eVerLayoutMode)
	{
	case CLV_TOP:
	{
		rcNew.top = rcLayoutMargin.top;
		rcNew.bottom = rcNew.top + rcOld.Height();
	}
	break;

	case CLV_FREE:
		// Keep
		break;

	case CLV_BOTTOM:
	{
		rcNew.bottom = nParentHeight - rcLayoutMargin.bottom;
		rcNew.top = rcNew.bottom - rcOld.Height();
	}
	break;

	case CLV_CENTER:
	{
		rcNew.top = (rcLayoutMargin.top + nParentHeight - rcOld.Height()) / 2;
		rcNew.bottom = rcNew.top + rcOld.Height();
	}
	break;

	case CLV_FILL:
	{
		rcNew.top = rcLayoutMargin.top;
		rcNew.bottom = nParentHeight - rcLayoutMargin.bottom ;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return 0;
}

int StackToVertical(
	HWLWND hPrevCtrl,
	HWLWND hCurCtrl,
	int nParentWidth,
	BOOL bTop,
	OUT CIUIRect &rcNew)
{
	// 得到上一个控件的坐标
	CIUIRect rcPrev;
	GetWindowRectToParent(hPrevCtrl, rcPrev);

	// 忽略控件Margin，只使用Layout Margin
	CIUIRect rcPrevCtrlLayoutMargin;
	GetLayoutMargin(hPrevCtrl, rcPrevCtrlLayoutMargin);

	CIUIRect rcLayoutMargin;
	GetLayoutMargin(hCurCtrl, rcLayoutMargin);

	CIUIRect rcOld;
	GetWindowRectToParent(hCurCtrl, rcOld);

	rcNew = rcOld;
	if (bTop)
	{
		rcNew.top = rcPrev.bottom + rcPrevCtrlLayoutMargin.bottom + rcLayoutMargin.top;
		rcNew.bottom = rcNew.left + rcOld.Height();
	}
	else
	{
		rcNew.bottom = rcPrev.top - rcPrevCtrlLayoutMargin.top - rcLayoutMargin.bottom;
		rcNew.top = rcNew.bottom - rcOld.Height();
	}

	// Horizontal
	CONTROL_LAYOUT_HOR eHorLayoutMode = GetHorzLayoutMode(hCurCtrl);
	switch (eHorLayoutMode)
	{
	case CLH_LEFT:
	{
		rcNew.left = rcLayoutMargin.left;
		rcNew.right = rcNew.left + rcOld.Width();
	}
	break;

	case CLH_FREE:
		// Keep
		break;

	case CLH_RIGHT:
	{
		rcNew.right = nParentWidth - rcLayoutMargin.right;
		rcNew.left = rcNew.right - rcOld.Width();
	}
	break;

	case CLH_CENTER:
	{
		rcNew.left = (rcLayoutMargin.left + nParentWidth - rcOld.Width()) / 2;
		rcNew.right = rcNew.left + rcOld.Width();
	}
	break;

	case CLH_FILL:
	{
		rcNew.left = rcLayoutMargin.left;
		rcNew.right = nParentWidth - rcLayoutMargin.right;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return 0;
}

int IUI::StackPanelLayoutElement(
	HWLWND hPrevVisibleCtrl,
	HWLWND hCtrl,
	int nParentWidth,
	int nParentHeight,
	OUT CIUIRect &rcNew)
{
	if (!IsWindowVisible(hCtrl))
	{
		return -1;
	}

	HWLWND hParent = GetParent(hCtrl);
	if (GetClassName(hParent) != IUI_STACKPANEL)
	{
		return -2;
	}

	// 得到StackPanel的方向
	Direction eDirection = (Direction)IUI::SendMessage(hParent, SP_GET_DIRECTION, 0, 0);

	switch (eDirection)
	{
	case D_LEFT:
		StackToHorizontal(hPrevVisibleCtrl, hCtrl, nParentHeight, TRUE, rcNew);
		break;
	case D_TOP:
		StackToVertical(hPrevVisibleCtrl, hCtrl, nParentWidth, TRUE, rcNew);
		break;
	case D_RIGHT:
		StackToHorizontal(hPrevVisibleCtrl, hCtrl, nParentHeight, FALSE, rcNew);
		break;
	case D_BOTTOM:
		StackToVertical(hPrevVisibleCtrl, hCtrl, nParentWidth, FALSE, rcNew);
		break;
	default:
		break;
	}

	return 0;
}

int IUI::StackPanelLayoutCtrl(HWLWND hPrevVisiableCtrl, HWLWND hWnd)
{
	if (!IsWindowVisible(hWnd))
	{
		return 1;
	}

	HWLWND hParent = GetParent(hWnd);
	int nCurParentWidth = 0;
	int nCurParentHeight = 0;
	if (hParent == NULL)
	{
		CIUIRect rcParent;
		::GetClientRect(GetHostWnd(hWnd), rcParent);
		nCurParentWidth = rcParent.Width();
		nCurParentHeight = rcParent.Height();
	}
	else
	{
		CIUIRect rcParent;
		GetWindowRectToParent(hParent, rcParent);
		nCurParentWidth = rcParent.Width();
		nCurParentHeight = rcParent.Height();
	}

	CIUIRect rcNew;
	StackPanelLayoutElement(hPrevVisiableCtrl, hWnd, nCurParentWidth, nCurParentHeight, rcNew);
	MoveWindow(hWnd, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), FALSE);

	return 0;
}

int IUI::GetChildren(const CControlProp *pParent,
	std::vector<const CControlProp *> *pvpChilren)
{
	if (pvpChilren == NULL)
	{
		return -1;
	}

	int nChildCount = pParent->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		const CControlBaseProp *pChild = pParent->GetChild(i);

		if (pChild->IsControl())
		{
			pvpChilren->push_back((CControlProp *)pChild);

			GetChildren((CControlProp *)pChild, pvpChilren);
		}
	}

	return 0;
}

HWLWND IUI::GetWindow(HWLWND hWnd, UINT uCmd)
{
	HWLWND pwndT = NULL;

	switch (uCmd)
	{
	case GW_HWNDNEXT:
		pwndT = hWnd->spwndNext;
		break;

	case GW_HWNDFIRST:
		if (hWnd->spwndParent != NULL)
		{
			if (hWnd->m_bInBindChild)
			{
				pwndT = hWnd->spwndParent->spwndChild;
			}
			else
			{
				pwndT = hWnd->spwndParent->spwndBindItemChild;
			}
		}
		break;

	case GW_HWNDLAST:
		pwndT = hWnd;
		while (NULL != pwndT && NULL != pwndT->spwndNext)
		{
			pwndT = pwndT->spwndNext;
		}
		break;

	case GW_HWNDPREV:
		pwndT = hWnd->spwndPrev;
		break;

	case GW_OWNER:
		pwndT = GetParent(hWnd);
		break;

	case GW_CHILD:
		pwndT = hWnd->spwndChild;
		break;

	case GW_CHILDBINDITEM:
		pwndT = hWnd->spwndBindItemChild;
		break;

	case GW_ENABLEDPOPUP:
		pwndT = NULL;
		break;

	default:
		return NULL;
	}

	return pwndT;
}

CWLWnd *IUI::GetWindow(CWLWnd *pWnd, UINT uCmd)
{
	HWLWND hRet = GetWindow(pWnd->m_hWnd, uCmd);
	if (hRet == NULL)
	{
		return NULL;
	}

	return hRet->ci.pThis;
}



// ----------------------------------------------------------------------------
//
//  返回指定hWnd窗口下方与指定hWnd最靠近的窗口(按List顺序，类似于树控件的List顺序)
//
// ----------------------------------------------------------------------------
HWLWND GetNextHWLWND(HWLWND hWnd)
{
	// 如果Item有孩子，则返回第一个孩子
	if (hWnd->spwndChild != NULL)
	{
		return hWnd->spwndChild;
	}
	if (NULL != hWnd->spwndBindItemChild)
	{
		return hWnd->spwndBindItemChild;
	}
checkNext:
	// 如果hWnd有下一个兄弟窗口，则返回下一个兄弟窗口
	if (hWnd->spwndNext != NULL)
	{
		return hWnd->spwndNext;
	}

	// 向上沿线查找
	hWnd = hWnd->spwndParent;
	if (hWnd != NULL && hWnd->m_uID != (UINT)(-1))
	{
		goto checkNext;
	}

	return NULL;
}

BOOL IUI::IsDescendant(HWLWND hParent, HWLWND hWnd)
{
	if (NULL == hWnd)
	{
		if (NULL != hParent)
		{
			return TRUE;
		}
	}

	if (NULL == hParent)
	{
		return TRUE;
	}

	BOOL bOk = FALSE;
	hWnd = hWnd->spwndParent;
	do
	{
		if (hWnd == hParent)
		{
			bOk = TRUE;
			break;
		}
	}
	while (hWnd = hWnd->spwndParent);

	return bOk;
}

// 得到hParent分支下、指定hWnd窗口下方与指定
// hWnd最靠近的窗口(按List顺序，类似于树控件的List顺序)
// hWnd必须是hParent的后裔。
// 与HWLWND GetNextHWLWND(HWLWND hWnd)的区别是：
// GetNextHWLWND(HWLWND hParent, HWLWND hWnd)仅在hParent分支内查找
HWLWND GetNextHWLWND(HWLWND hParent, HWLWND hWnd)
{
	if (NULL == hParent)
	{
		return NULL;
	}

	if (NULL == hWnd)
	{
		return hParent->spwndChild != NULL ? hParent->spwndChild : hParent->spwndBindItemChild;
	}

	// 检测hWnd是否是hParent后裔
	if (!IsDescendant(hParent, hWnd))
	{
		return NULL;
	}

	// 如果Item有孩子，则返回第一个孩子
	if (hWnd->spwndChild != NULL)
	{
		return hWnd->spwndChild;
	}
	if (NULL != hWnd->spwndBindItemChild)
	{
		return hWnd->spwndBindItemChild;
	}

checkNext:
	// 如果hWnd有下一个兄弟窗口，则返回下一个兄弟窗口
	if (hWnd->spwndNext != NULL)
	{
		return hWnd->spwndNext;
	}

	// 向上沿线查找
	hWnd = hWnd->spwndParent;
	if (hWnd != NULL && hWnd->m_uID != (UINT)(-1) && hWnd != hParent)
	{
		goto checkNext;
	}

	return NULL;
}

// hwndChildAfter必须是hwndParent的后裔
HWLWND IUI::FindControlEx(
	HWND hHost,
	HWLWND hwndParent,
	HWLWND hwndChildAfter,
	LPCTSTR lpszClass,
	LPCTSTR lpszWindow)
{
	if (hHost == NULL)
	{
		return NULL;
	}

	if (lpszClass == NULL && lpszWindow == NULL)
	{
		return NULL;
	}

	// 如果指定的父控件为NULL，则使用虚拟根控件作为父控件
	if (NULL == hwndParent)
	{
		hwndParent = (HWLWND)::SendMessage(hHost, WM_GET_INTERNAL_ROOT, 0, 0);
	}
	if (NULL == hwndParent)
	{
		return NULL;
	}

	if (!IsDescendant(hwndParent, hwndChildAfter))
	{
		return NULL;
	}

	HWLWND hRet = NULL;
	HWLWND hWalk = hwndChildAfter;
	while (hWalk = GetNextHWLWND(hwndParent, hWalk))
	{
		BOOL bMatchClass = TRUE;
		if (lpszClass != NULL)
		{
			if (GetClassName(hWalk) != lpszClass)
			{
				bMatchClass = FALSE;
			}
		}

		BOOL bMatchCaption = TRUE;
		if (lpszWindow != NULL)
		{
			if (GetWindowText(hWalk) != lpszWindow)
			{
				bMatchCaption = FALSE;
			}
		}

		if (bMatchClass && bMatchCaption)
		{
			hRet = hWalk;
			break;
		}
	}

	return hRet;
}

int IUI::GetAllChildren(IN HWLWND hWnd, BOOL bIncludeSelf, OUT std::vector<HWLWND> *pvChildren)
{
	if (hWnd == NULL || pvChildren == NULL)
	{
		return -1;
	}

	if (bIncludeSelf)
	{
		pvChildren->push_back(hWnd);
	}

	HWLWND hChild = GetWindow(hWnd, GW_CHILD);
	while (hChild != NULL)
	{
		pvChildren->push_back(hChild);

		GetAllChildren(hChild, FALSE, pvChildren);

		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}

	return 0;
}

int IUI::BindTextAlignStyle(HWLWND hWnd, const CLabelProp *pLabelProp)
{
	if (NULL == hWnd || NULL == pLabelProp)
	{
		return -1;
	}

	UINT uOld = hWnd->ci.m_uTextStyle;

	if ((bool)pLabelProp->m_bMultiline)
	{
		hWnd->ci.m_uTextStyle |= DT_WORDBREAK;
		hWnd->ci.m_uTextStyle &= ~DT_SINGLELINE;
	}
	else
	{
		hWnd->ci.m_uTextStyle |= DT_SINGLELINE;
		hWnd->ci.m_uTextStyle &= ~DT_WORDBREAK;
	}

	if ((bool)pLabelProp->m_bEndEllipsis)
	{
		hWnd->ci.m_uTextStyle |= DT_END_ELLIPSIS;
	}
	else
	{
		hWnd->ci.m_uTextStyle &= ~DT_END_ELLIPSIS;
	}

	if ((bool)pLabelProp->m_bPathEllipsis)
	{
		hWnd->ci.m_uTextStyle |= DT_PATH_ELLIPSIS;
	}
	else
	{
		hWnd->ci.m_uTextStyle &= ~DT_PATH_ELLIPSIS;
	}

	return 0;
}

int IUI::IUISetControlBkImage2(
	HWLWND hWnd,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameD)
{
	_ASSERT(IsWindow(hWnd));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUI::SetBkImage(hWnd, FALSE, CONTROL_STATE_NORMAL, lpszImageNameN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUI::SetBkImage(hWnd, FALSE, CONTROL_STATE_DISABLED, lpszImageNameD);
	}

	return 0;
}

int IUI::IUIGetControlBkImage2(
	HWLWND hWnd,
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD)
{
	if (pbCombineImage == NULL)
	{
		return -1;
	}

	*pbCombineImage = IUI::IsCombineBkImage(hWnd);

	if (*pbCombineImage)
	{
		return IUI::GetBkImage(hWnd, FALSE, CONTROL_STATE_COMBINE, pstrCombineImageName);
	}
	else
	{
		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			return IUI::GetBkImage(hWnd, FALSE, CONTROL_STATE_NORMAL, pstrImageNameN);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			return IUI::GetBkImage(hWnd, FALSE, CONTROL_STATE_DISABLED, pstrImageNameD);
		}
	}

	return 0;
}

int IUI::IUISetControlBkImage4(
	HWLWND hWnd,
	BOOL bChecked,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD)
{
	_ASSERT(IsWindow(hWnd));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUI::SetBkImage(hWnd, bChecked, CONTROL_STATE_NORMAL, lpszImageNameN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		IUI::SetBkImage(hWnd, bChecked, CONTROL_STATE_HOVER, lpszImageNameH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		IUI::SetBkImage(hWnd, bChecked, CONTROL_STATE_PRESSED, lpszImageNameP);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUI::SetBkImage(hWnd, bChecked, CONTROL_STATE_DISABLED, lpszImageNameD);
	}

	return 0;
}

int IUI::IUIGetControlBkImage4(
	HWLWND hWnd,
	BOOL bChecked,
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameP,
	CIUIString *pstrImageNameD)
{
	if (pbCombineImage == NULL)
	{
		return -1;
	}

	*pbCombineImage = IUI::IsCombineBkImage(hWnd);

	if (*pbCombineImage)
	{
		return IUI::GetBkImage(hWnd, FALSE, CONTROL_STATE_COMBINE, pstrCombineImageName);
	}
	else
	{
		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			return IUI::GetBkImage(hWnd, bChecked, CONTROL_STATE_NORMAL, pstrImageNameN);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
		{
			return IUI::GetBkImage(hWnd, bChecked, CONTROL_STATE_HOVER, pstrImageNameH);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
		{
			return IUI::GetBkImage(hWnd, bChecked, CONTROL_STATE_PRESSED, pstrImageNameP);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			return IUI::GetBkImage(hWnd, bChecked, CONTROL_STATE_DISABLED, pstrImageNameD);
		}
	}

	return 0;
}

int IUI::IUISetControlFgImage4(
	HWLWND hWnd,
	BOOL bChecked,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD)
{
	_ASSERT(IsWindow(hWnd));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUI::SetFgImage(hWnd, bChecked, CONTROL_STATE_NORMAL, lpszImageNameN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		IUI::SetFgImage(hWnd, bChecked, CONTROL_STATE_HOVER, lpszImageNameH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		IUI::SetFgImage(hWnd, bChecked, CONTROL_STATE_PRESSED, lpszImageNameP);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUI::SetFgImage(hWnd, bChecked, CONTROL_STATE_DISABLED, lpszImageNameD);
	}

	return 0;
}

int IUI::IUIGetControlFgImage4(
	HWLWND hWnd,
	BOOL bChecked,
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameP,
	CIUIString *pstrImageNameD)
{
	if (pbCombineImage == NULL)
	{
		return -1;
	}

	*pbCombineImage = IUI::IsCombineFgImage(hWnd);

	if (*pbCombineImage)
	{
		return IUI::GetFgImage(hWnd, FALSE, CONTROL_STATE_COMBINE, pstrCombineImageName);
	}
	else
	{
		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			return IUI::GetFgImage(hWnd, bChecked, CONTROL_STATE_NORMAL, pstrImageNameN);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
		{
			return IUI::GetFgImage(hWnd, bChecked, CONTROL_STATE_HOVER, pstrImageNameH);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
		{
			return IUI::GetFgImage(hWnd, bChecked, CONTROL_STATE_PRESSED, pstrImageNameP);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			return IUI::GetFgImage(hWnd, bChecked, CONTROL_STATE_DISABLED, pstrImageNameD);
		}
	}

	return 0;
}

CIUIString IUI::HexStringFromInt(int n, BOOL bHasPrefix, BOOL b0Fill, int nBits/* = 8*/)
{
	CIUIString strRet;
	if (bHasPrefix)
	{
		if (b0Fill)
		{
			CIUIString strFormat;
			strFormat.Format(_T("0x%%0%dX"), nBits);
			strRet.Format(strFormat, n);
		}
		else
		{
			strRet.Format(_T("0x%X"), n);
		}
	}
	else
	{
		if (b0Fill)
		{
			CIUIString strFormat;
			strFormat.Format(_T("%%0%dX"), nBits);
			strRet.Format(strFormat, n);
		}
		else
		{
			strRet.Format(_T("%X"), n);
		}
	}

	return strRet;
}

std::string IUI::GetHexCodeW(LPCWSTR lpszSource)
{
	size_t nLen = wcslen(lpszSource);

	size_t nSize = nLen * sizeof(WCHAR);
	BYTE *pData = new BYTE[nSize];

	// Ignore 0.
	size_t nIndex = 0;
	BYTE *pSource = (BYTE *)lpszSource;
	for (size_t i = 0; i < nSize; ++i)
	{
		if (pSource[i] != 0)
		{
			pData[nIndex++] = pSource[i];
		}
	}

	CIUIString strRet;
	for (size_t j = 0; j < nIndex; ++j)
	{
		BYTE bt = pData[j];

		BYTE nHi = bt >> 4;
		BYTE nLo = bt << 4;
		nLo = nLo >> 4;

		strRet += HexStringFromInt(nHi, FALSE, FALSE, 1);
		strRet += HexStringFromInt(nLo, FALSE, FALSE, 1);
	}

	delete[] pData;

	return (LPCSTR)_bstr_t(strRet);
}

CIUIString IUI::GetFileNameHexCode(LPCTSTR lpszImageName, BOOL bEncodeExt)
{
	// Must utf-16.
	std::wstring wstrFileName = (LPCWSTR)_bstr_t(lpszImageName);
	// ABC.bmp and abc.bmp must be the same one hex code.
	std::transform(wstrFileName.begin(), wstrFileName.end(), wstrFileName.begin(), towlower);

	if (bEncodeExt)
	{
		return (LPCTSTR)_bstr_t(GetHexCodeW(wstrFileName.c_str()).c_str());
	}

	// Prefix, the hex code string may a number, So need a prefix.
	std::wstring wstrFileHexCode = L"I";

	std::wstring wstrTitle = GetFileTitleExW(wstrFileName.c_str());
	std::wstring wstrExt = GetFileExtW(wstrFileName.c_str(), TRUE);
	wstrFileHexCode += (LPCWSTR)_bstr_t(GetHexCodeW(wstrTitle.c_str()).c_str());
	wstrFileHexCode += wstrExt;

	return (LPCTSTR)_bstr_t(wstrFileHexCode.c_str());
}

// 1 twips equal to 1/1440 inch.
int IUI::GetXTwipsFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nXDPI == 0)
	{
		nXDPI = 96;
	}

	int nRet = MulDiv(nPixel, 1440, nXDPI);

	return nRet;
}

int IUI::GetYTwipsFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nYDPI == 0)
	{
		nYDPI = 96;
	}

	int nRet = MulDiv(nPixel, 1440, nYDPI);

	return nRet;
}

int IUI::GetXPixelFromTwips(int nTwips)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nYDPI == 0)
	{
		nYDPI = 96;
	}

	int nRet = MulDiv(nTwips, nYDPI, 1440);

	return nRet;
}

int IUI::GetYPixelFromTwips(int nTwips)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nYDPI == 0)
	{
		nYDPI = 96;
	}

	int nRet = MulDiv(nTwips, nYDPI, 1440);

	return nRet;
}

// 1 FPPTS equal to 1/20 twips
float IUI::GetXFPPTSFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nXDPI == 0)
	{
		nXDPI = 96;
	}

	float nRet = (float)MulDiv(nPixel, 72, nXDPI); // 72 = 1440 * 0.05

	return nRet;
}

float IUI::GetYFPPTSFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nYDPI == 0)
	{
		nYDPI = 96;
	}

	float nRet = (float)MulDiv(nPixel, 72, nYDPI); // 72 = 1440 * 0.05

	return nRet;
}

// 1 himetric equal to 1/100 mm.
int IUI::GetXHimetricFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);
	int nRet = MulDiv(nPixel, 2540, nXDPI);

	return nRet;
}

int IUI::GetYHimetricFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);
	int nRet = MulDiv(nPixel, 2540, nYDPI);

	return nRet;
}

int IUI::GetXPixelFromHimetric(int nHimetric)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	int nRet = MulDiv(nHimetric, nXDPI, 2540);

	return nRet;
}

int IUI::GetYPixelFromHimetric(int nHimetric)
{
	// TODO: 这些DPI值，应该得到一次后保存下来
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	int nRet = MulDiv(nHimetric, nYDPI, 2540);

	return nRet;
}

LRESULT CALLBACK IUI::IUICbtFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
	_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();

	if (code != HCBT_CREATEWND)
	{
		// wait for HCBT_CREATEWND just pass others on...
		return CallNextHookEx(pThreadState->m_hHookOldCbtFilter, code,
				wParam, lParam);
	}

	_ASSERT(lParam != NULL);
	LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
	_ASSERT(lpcs != NULL);

	Window *pWndInit = pThreadState->m_pWndInit;
	if (pWndInit != NULL || (!(lpcs->style & WS_CHILD)))
	{
		_ASSERT(wParam != NULL); // should be non-NULL HWND
		HWND hWnd = (HWND)wParam;

		if (pWndInit != NULL)
		{
			// the window should not be in the permanent map at this time
			_ASSERT(Window::FromHandlePermanent(hWnd) == NULL);

			// connect the HWND to pWndInit...
			pWndInit->Attach(hWnd);
			pWndInit->m_hVirtualRoot->m_hHostWnd = hWnd;

			// allow other subclassing to occur first
			pWndInit->PreSubclassWindow();

			// 子类化目标窗口
			WNDPROC oldWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
			if (oldWndProc != IUI::IUIWndProcBase)
			{
				oldWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC,
						(DWORD_PTR)IUI::IUIWndProcBase);
				_ASSERT(oldWndProc != NULL);

				WNDPROC *pOldWndProc = pWndInit->GetSuperWndProcAddr();
				_ASSERT(pOldWndProc != NULL);
				*pOldWndProc = oldWndProc;
			}

			pThreadState->m_pWndInit = NULL;
		}
	}

	LRESULT lResult = CallNextHookEx(pThreadState->m_hHookOldCbtFilter, code,
			wParam, lParam);

	return lResult;
}

void IUI::HookWindowCreate(Window *pWnd)
{
	_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();
	if (pThreadState->m_pWndInit == pWnd)
	{
		return;
	}

	if (pThreadState->m_hHookOldCbtFilter == NULL)
	{
		pThreadState->m_hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT,
				IUICbtFilterHook, NULL, ::GetCurrentThreadId());

		if (pThreadState->m_hHookOldCbtFilter == NULL)
		{
			_ASSERT(FALSE);
			return;
		}
	}

	_ASSERT(pThreadState->m_hHookOldCbtFilter != NULL);
	_ASSERT(pWnd != NULL);
	_ASSERT(pWnd->m_hWnd == NULL);   // only do once

	_ASSERT(pThreadState->m_pWndInit == NULL);   // hook not already in progress
	pThreadState->m_pWndInit = pWnd;
}

BOOL IUI::UnhookWindowCreate()
{
	_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();

#ifndef _AFXDLL
	if (iuiContextIsDLL && pThreadState->m_hHookOldCbtFilter != NULL)
	{
		::UnhookWindowsHookEx(pThreadState->m_hHookOldCbtFilter);
		pThreadState->m_hHookOldCbtFilter = NULL;
	}
#endif
	if (pThreadState->m_pWndInit != NULL)
	{
		pThreadState->m_pWndInit = NULL;
		return FALSE;   // was not successfully hooked
	}

	return TRUE;
}

BOOL IUI::IsUpdateLayeredWindow(HWND hWnd)
{
	return (::GetProp(hWnd, _T("UpdateLayeredWindow")) != 0);
}

int IUI::SetLayeredWindow(HWND hWnd, BOOL bSet)
{
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if (bSet && (dwExStyle & WS_EX_LAYERED) != WS_EX_LAYERED)
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED);
	}
	else if (!bSet && (dwExStyle & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle & ~ WS_EX_LAYERED);
	}

	return 0;
}

CIUIString IUI::GetDefaultFontID()
{
	return _T("DefaultFontID_{20C69D19-3DFD-4e9f-8C0D-B1CE0A89A3D6}");
}

CIUIString IUI::GetDefaultColorID()
{
	return _T("DefaultColorID_{50155D59-045F-4d37-B042-5E4A4AE94CBD}");;
}

void IUI::IUITRACEA(LPCSTR lpszFormat, ...)
{
#ifdef _DEBUG
	_ASSERT(lpszFormat != NULL);

	va_list argList;
	va_start(argList, lpszFormat);

	CIUIString s;
	USES_CONVERSION;
	s.FormatV(A2T((LPSTR)lpszFormat), argList);
	va_end(argList);

	OutputDebugStringA(T2A(s.GetBuffer(0)));
#endif // _DEBUG
}

void IUI::IUITRACE(LPCTSTR lpszFormat, ...)
{
#ifdef _DEBUG
	// 如果c盘根下有文件IUI.NOLOG文件，则不打印调试信息
	if (PathFileExists(_T("C:\\IUI.NOLOG")))
	{
		return;
	}

	_ASSERT(lpszFormat != NULL);

	va_list argList;
	va_start(argList, lpszFormat);

	CIUIString s;
	s.FormatV(lpszFormat, argList);
	va_end(argList);

	OutputDebugString(s);
#endif // _DEBUG
}

BOOL IUI::IUIIsValidAddress(const void *p, size_t nBytes, BOOL bReadWrite/* = TRUE*/)
{
	(bReadWrite);
	(nBytes);
	return (p != NULL);
}

BOOL IUI::IUIIsValidStringA(LPCSTR psz, size_t nMaxLength/* = UINT_MAX*/)
{
	(nMaxLength);
	return (psz != NULL);
}

BOOL IUI::IUIIsValidStringW(LPCWSTR psz, size_t nMaxLength/* = UINT_MAX*/)
{
	(nMaxLength);
	return (psz != NULL);
}

errno_t IUI::IUICopyMemory(
	void *dest,
	size_t numberOfElements,
	const void *src,
	size_t count)
{
#if(_MSC_VER > 1200) // VC6.0
	Checked::memcpy_s(dest, numberOfElements, src, count);
	return 0;
#else
	void *p = memcpy(dest, src, count);
	return p != NULL ? 0 : ERANGE;
#endif
}

errno_t IUIAPI IUI::IUIStringCopy(
	TCHAR *_Dest,
	size_t _SizeInChars,
	const TCHAR *_Source,
	size_t _Count)
{
#if(_MSC_VER > 1200) // VC6.0
	return Checked::tcsncpy_s(_Dest, _SizeInChars, _Source, _Count);
#else
	TCHAR *p = _tcsncpy(_Dest, _Source, _Count);
	return p != NULL ? 0 : ERANGE;
#endif
}

bool DPI::IsEnableDPI()
{
	return true;
}

int DPI::GetDPIRect(__inout LPRECT lprc)
{
	return 0;
}

int DPI::GetDPIPadding(__inout LPRECT lprc)
{
	return 0;
}

int DPI::GetDPIWidth(__in int nWidth)
{
	return nWidth;
}

int DPI::GetDPIHeight(__in int nHeight)
{
	return nHeight;
}

COLORREF Metrics::g_crButtonBkN = RGB(225, 225, 225);
COLORREF Metrics::g_crButtonBkH = RGB(229, 241, 251);
COLORREF Metrics::g_crButtonBkS = RGB(225, 225, 225);
COLORREF Metrics::g_crButtonBkD = RGB(204, 204, 204);
COLORREF Metrics::g_crButtonBkCN = RGB(225, 225, 225);
COLORREF Metrics::g_crButtonBkCH = RGB(229, 241, 251);
COLORREF Metrics::g_crButtonBkCS = RGB(225, 225, 225);
COLORREF Metrics::g_crButtonBkCD = RGB(204, 204, 204);
COLORREF Metrics::g_crButtonBorderN = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderH = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderS = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderD = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderCN = RGB(255, 120, 205);
COLORREF Metrics::g_crButtonBorderCH = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderCS = RGB(0, 120, 205);
COLORREF Metrics::g_crButtonBorderCD = RGB(0, 120, 205);

// 滚动条按钮背景颜色
COLORREF Metrics::g_crScrollButtonBkN = RGB(240, 240, 240);
COLORREF Metrics::g_crScrollButtonBkH = RGB(218, 218, 218);
COLORREF Metrics::g_crScrollButtonBkP = RGB(96, 96, 96);
COLORREF Metrics::g_crScrollButtonBkD;
// 滚动条轨道颜色
COLORREF Metrics::g_crScrollGroove = RGB(240, 240, 240);
// 滚动条滑块颜色
COLORREF Metrics::g_crScrollThumbBkN = RGB(205, 205, 205);
COLORREF Metrics::g_crScrollThumbBkH = RGB(166, 166, 166);
COLORREF Metrics::g_crScrollThumbBkP = RGB(96, 96, 96);
COLORREF Metrics::g_crScrollThumbBkD;
const CScrollBarProp *Metrics::g_pHorScrollBarBindProp = NULL;
const CScrollBarProp *Metrics::g_pVerScrollBarBindProp = NULL;
int Metrics::g_nScrollBarWidth = 17;


BOOL Metrics::g_bFloatScrollBar = FALSE;

void Metrics::InitWin10Style()
{
	// 	g_crButtonBkN;
	// 	g_crButtonBkH;
	// 	g_crButtonBkS;
	// 	g_crButtonBkD;
	//
	// 	g_crButtonBorderN;
	// 	g_crButtonBorderH;
	// 	g_crButtonBorderS;
	// 	g_crButtonBorderD;
}

// 当未指定控件背景图时，提供默认外观
COLORREF Metrics::GetButtonBkColorN()
{
	return g_crButtonBkN;
}

COLORREF Metrics::GetButtonBkColorH()
{
	return g_crButtonBkH;
}

COLORREF Metrics::GetButtonBkColorS()
{
	return g_crButtonBkS;
}

COLORREF Metrics::GetButtonBkColorD()
{
	return g_crButtonBkD;
}

COLORREF Metrics::GetButtonBkColorCN()
{
	return g_crButtonBkCN;
}

COLORREF Metrics::GetButtonBkColorCH()
{
	return g_crButtonBkCH;
}

COLORREF Metrics::GetButtonBkColorCS()
{
	return g_crButtonBkCS;
}

COLORREF Metrics::GetButtonBkColorCD()
{
	return g_crButtonBkCD;
}

COLORREF Metrics::GetButtonBorderColorN()
{
	return g_crButtonBorderN;
}

COLORREF Metrics::GetButtonBorderColorH()
{
	return g_crButtonBorderH;
}

COLORREF Metrics::GetButtonBorderColorS()
{
	return g_crButtonBorderS;
}

COLORREF Metrics::GetButtonBorderColorD()
{
	return g_crButtonBorderD;
}

COLORREF Metrics::GetButtonBorderColorCN()
{
	return g_crButtonBorderCN;
}

COLORREF Metrics::GetButtonBorderColorCH()
{
	return g_crButtonBorderCH;
}

COLORREF Metrics::GetButtonBorderColorCS()
{
	return g_crButtonBorderCS;
}

COLORREF Metrics::GetButtonBorderColorCD()
{
	return g_crButtonBorderCD;
}

COLORREF Metrics::GetScrollButtonBkColorN()
{
	return g_crScrollButtonBkN;
}

COLORREF Metrics::GetScrollButtonBkColorH()
{
	return g_crScrollButtonBkH;
}

COLORREF Metrics::GetScrollButtonBkColorP()
{
	return g_crScrollButtonBkP;
}

COLORREF Metrics::GetScrollButtonBkColorD()
{
	return g_crScrollButtonBkD;
}

COLORREF Metrics::GetScrollGrooveColor()
{
	return g_crScrollGroove;
}

COLORREF Metrics::GetScrollThumbBkColorN()
{
	return g_crScrollThumbBkN;
}

COLORREF Metrics::GetScrollThumbBkColorH()
{
	return g_crScrollThumbBkH;
}

COLORREF Metrics::GetScrollThumbBkColorP()
{
	return g_crScrollThumbBkP;
}

COLORREF Metrics::GetScrollThumbBkColorD()
{
	return g_crScrollThumbBkD;
}

void Metrics::SetFloatScrollBar(BOOL bSet)
{
	g_bFloatScrollBar = bSet;
}

BOOL Metrics::IsFloatScrollBar()
{
	return g_bFloatScrollBar;
}

void Metrics::SetHorScrollBarBindStyle(const CScrollBarProp *pBindProp)
{
	g_pHorScrollBarBindProp = pBindProp;
}
const CScrollBarProp *Metrics::GetHorScrollBarBindStyle()
{
	return g_pHorScrollBarBindProp;
}
void Metrics::SetVerScrollBarBindStyle(const CScrollBarProp *pBindProp)
{
	g_pVerScrollBarBindProp = pBindProp;
}
const CScrollBarProp *Metrics::GetVerScrollBarBindStyle()
{
	return g_pVerScrollBarBindProp;
}
void Metrics::SetScrollBarWidth(int nWidth)
{
	g_nScrollBarWidth = nWidth;
}
int Metrics::GetScrollBarWidth()
{
	return g_nScrollBarWidth;
}

int Global::IUISetControlFont(IN CIUIProject *pOwner,
	__inout HIUIFONT *phIUIFont, LPCTSTR lpszFontResID)
{
	if (pOwner == NULL || phIUIFont == NULL)
	{
		return -1;
	}

	CIUIString strNewFontResID = lpszFontResID;

	// 不论新旧字体是否一样，都释放旧字体
	// 这是由于可能在界面编辑器的Resource中，修改了某个字体，但名字没改。
	if (*phIUIFont != NULL)
	{
		ReleaseIUIFontInternal(*phIUIFont);
	}

	// 输入ID有效
	if (!strNewFontResID.IsEmpty())
	{
		*phIUIFont = CreateEmptyIUIFontInternal(pOwner, lpszFontResID);
	}
	else
	{
		// 使用默认字体
		*phIUIFont = CreateEmptyIUIFontInternal(pOwner, GetDefaultFontID());
	}

	return 0;
}

int Global::IUISetControlFont2(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIFONT *phIUIFont,
	LPCTSTR lpszFontResIDN,
	LPCTSTR lpszFontResIDD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDD);
	}

	return 0;
}

int Global::IUISetControlFont4(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIFONT *phIUIFont,
	LPCTSTR lpszFontResIDN,
	LPCTSTR lpszFontResIDH,
	LPCTSTR lpszFontResIDS,
	LPCTSTR lpszFontResIDD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDH);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDS);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDD);
	}

	return 0;
}

int Global::IUISetControlCheckedFont4(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIFONT *phIUIFont,
	LPCTSTR lpszFontResIDCN,
	LPCTSTR lpszFontResIDCH,
	LPCTSTR lpszFontResIDCS,
	LPCTSTR lpszFontResIDCD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDCN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDCH);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDCS);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlFont(pOwner, phIUIFont + nIndex, lpszFontResIDCD);
	}

	return 0;
}

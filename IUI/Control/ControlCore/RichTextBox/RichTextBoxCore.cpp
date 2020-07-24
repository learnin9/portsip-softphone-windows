// 对于Windowless RichEdit控件，由于没有窗口句柄，
// 所以传给RichEdit控件的坐标，都应该是相对于Host窗口的坐标。
// CTextHost基于微软的CTxtWinHost

#include "stdafx.h"
#if(_MSC_VER <= 1200) // VC6.0
#include <ObjIdl.h>
#endif
#include <RichOle.h>
#include <atlbase.h>
#pragma comment (lib, "Imm32.lib")
#ifdef DEBUG_RICHEDIT
#include "RichEditSrc/_common.h"
#include "RichEditSrc/_edit.h"
#include "RichEditSrc/_host.h"
#include "RichEditSrc/_nlsprcs.h"
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


// 调试RichEdit的时候，使用微软源代码
EXTERN_C const IID IID_ITextServices =   // 8d33f740-cf58-11ce-a89d-00aa006cadc5
{
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost =   /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
{
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextDocument =
{
	0x8CC497C0L,
	0xA1DF,
	0x11CE,
	{0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D}
};

#ifdef DEBUG_RICHEDIT
#define cpMax	cpMost
#endif // DEBUG_RICHEDIT

HRESULT IUI::GenerateCharFormat(CHARFORMAT2 *pcf, LOGFONT &lf)
{
	memset(pcf, 0, sizeof(CHARFORMAT2));

	pcf->cbSize = sizeof(CHARFORMAT2);

	// dwMask的值由两个集合中的宏组合
	// 一个集合中的宏指示要设置或读取CHARFORMAT2中的哪个成员
	// 另一个集合中的宏指示要设置或读取CHARFORMAT2::dwEffects中的哪一位。
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_CHARSET | CFM_FACE
		| CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	// CFM_SIZE，单位缇
	pcf->yHeight = GetXTwipsFromPixel(-lf.lfHeight);

	// CFM_OFFSET，单位缇
	pcf->yOffset = 0;

	// CFM_CHARSET
	pcf->bCharSet = lf.lfCharSet;

	pcf->bPitchAndFamily = lf.lfPitchAndFamily;

	// CFM_FACE
	StringCchCopy(pcf->szFaceName, LF_FACESIZE, lf.lfFaceName);

	// dwEffects集中的CFM_BOLD
	pcf->dwEffects = 0;
	if (lf.lfWeight >= FW_BOLD)
	{
		pcf->dwEffects |= CFE_BOLD;
	}

	// dwEffects集中的CFM_ITALIC
	if (lf.lfItalic)
	{
		pcf->dwEffects |= CFE_ITALIC;
	}

	// dwEffects集中的CFM_UNDERLINE
	if (lf.lfUnderline)
	{
		pcf->dwEffects |= CFE_UNDERLINE;
	}

	return S_OK;
}

HRESULT IUI::GenerateCharFormat(CHARFORMAT2 *pcf, COLORREF crText)
{
	memset(pcf, 0, sizeof(CHARFORMATW));

	pcf->cbSize = sizeof(CHARFORMATW);
	// dwMask的值由两个集合中的宏组合
	// 一个集合中的宏指示要设置或读取CHARFORMAT2中的哪个成员
	// 另一个集合中的宏指示要设置或读取CHARFORMAT2::dwEffects中的哪一位。
	pcf->dwMask = CFM_COLOR;

	// CFM_COLOR
	pcf->crTextColor = crText;

	return S_OK;
}

HRESULT GenerateParaFormat(RichTextBox *pRichTextBox, PARAFORMAT2 *ppf)
{
	memset(ppf, 0, sizeof(PARAFORMAT2));
	ppf->cbSize = sizeof(PARAFORMAT2);
	ppf->dwMask = PFM_ALL;
	ppf->wAlignment = PFA_LEFT;
	ppf->cTabCount = 1;
	ppf->rgxTabs[0] = lDefaultTab;

	return S_OK;
}

int ConvertCHARFORMAT2(__in const CHARFORMAT2A *pSrc, __out CHARFORMAT2W *pDest)
{
	if (pSrc == NULL || pDest == NULL)
	{
		return -1;
	}

	// copy base class _charformat
	pDest->cbSize = sizeof(CHARFORMAT2W);
	pDest->dwMask = pDest->dwMask;
	pDest->dwEffects = pDest->dwEffects;
	pDest->yHeight = pDest->yHeight;
	pDest->yOffset = pDest->yOffset;
	pDest->crTextColor = pDest->crTextColor;
	pDest->bCharSet = pDest->bCharSet;
	pDest->bPitchAndFamily = pDest->bPitchAndFamily;

	StringCchCopyW(pDest->szFaceName, LF_FACESIZE, (LPCWSTR)(_bstr_t)pSrc->szFaceName);

	// copy CHARFORMAT2
	pDest->wWeight = pSrc->wWeight;			// Font weight (LOGFONT value)
	pDest->sSpacing = pSrc->sSpacing;			// Amount to space between letters
	pDest->crBackColor = pSrc->crBackColor;		// Background color
	pDest->lcid = pSrc->lcid;				// Locale ID
#if (_RICHEDIT_VER >= 0x0500)
	pDest->dwReserved = pSrc->dwReserved;			// Name up to 5.0
	pDest->dwCookie = pSrc->dwCookie;	 		// Client cookie opaque to RichEdit
#else
	pDest->dwReserved = pSrc->dwReserved;			// Name up to 5.0
#endif
	pDest->sStyle = pSrc->sStyle; 			// Style handle
	pDest->wKerning = pSrc->wKerning;			// Twip size above which to kern char pair
	pDest->bUnderlineType = pSrc->bUnderlineType; 	// Underline type
	pDest->bAnimation = pSrc->bAnimation; 		// Animated text like marching ants
	pDest->bRevAuthor = pSrc->bRevAuthor; 		// Revision author index
#if (_RICHEDIT_VER >= 0x0800)
	pDest->bUnderlineColor = pSrc->bUnderlineColor;	// Underline color
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////


LONG CTxtWinHost::_xWidthSys = 0;			// Average char width of system font
LONG CTxtWinHost::_yHeightSys = 0;			// Height of system font

const INT 	MAX_PROCS = 27;
FARPROC nlsProcTable[MAX_PROCS];					// Final proc table.

typedef HIMC(WINAPI *IGC_CAST)(HWND);
#define	pImmGetContext(a) ((IGC_CAST)nlsProcTable[iImmGetContext])(a)
typedef HIMC(WINAPI *IAC_CAST)(HWND, HIMC);
#define	pImmAssociateContext(a,b) ((IAC_CAST)nlsProcTable[iImmAssociateContext])(a,b)
// ImmReleaseContext
typedef BOOL(WINAPI *IRC_CAST)(HWND, HIMC);
#define	pImmReleaseContext(a,b) ((IRC_CAST)nlsProcTable[iImmReleaseContext])(a,b)

#define TRACEWARNHR(hr)     TraceMsg(MAKEFLAGS(TRCSUBSYSNONE, TRCSEVWARN,\
		TRCSCOPENONE, TRCDATAHRESULT, TRCDATANONE),\
	(DWORD)(hr), (DWORD)0, __FILE__, __LINE__)

#define TESTANDTRACEHR(hr)	if( hr < 0 ) { TRACEWARNHR(hr); }

#define SYS_ALTERNATE		0x20000000
#ifdef DBCS
#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
	ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
	ECO_VERTICAL | \
	ECO_SELECTIONBAR)
#else
#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
	ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
	ECO_SELECTIONBAR)
#endif

#define AssertSz(a, b)	if (!(a)) {\
		OutputDebugStringA((b)); \
	}

#define TRCSUBSYSHOST   0x9
#define ibPed 0
#define VERS4 4

#ifdef DEBUG_RICHEDIT
extern DWORD dwMajorVersion;
extern LONG xPerInchScreenDC; 					// Pixels per inch used for conversions ...
extern LONG yPerInchScreenDC;					// ... and determining whether screen or ...
extern COLORREF crAuto;
extern INT cxBorder;
extern INT cyBorder;
extern INT cxVScroll;
extern INT cyHScroll;
extern LONG dxSelBar;
#else // DEBUG_RICHEDIT
DWORD dwMajorVersion = 0;
LONG xPerInchScreenDC = 0; 					// Pixels per inch used for conversions ...
LONG yPerInchScreenDC = 0;					// ... and determining whether screen or ...
COLORREF crAuto = 0;
INT cxBorder;
INT cyBorder;
INT cxVScroll;
INT cyHScroll;
LONG dxSelBar;
#endif // DEBUG_RICHEDIT

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH   2540
#endif

// Convert Himetric along the X axis to X pixels
inline LONG	HimetricXtoDX2(LONG xHimetric, LONG xPerInch)
{
	// This formula is rearranged to get rid of the need for floating point
	// arithmetic. The real way to understand the formula is to use
	// (xHimetric / HIMETRIC_PER_INCH) to get the inches and then multiply
	// the inches by the number of x pixels per inch to get the pixels.
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
inline LONG HimetricYtoDY2(LONG yHimetric, LONG yPerInch)
{
	// This formula is rearranged to get rid of the need for floating point
	// arithmetic. The real way to understand the formula is to use
	// (xHimetric / HIMETRIC_PER_INCH) to get the inches and then multiply
	// the inches by the number of y pixels per inch to get the pixels.
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
inline LONG DXtoHimetricX2(LONG dx, LONG xPerInch)
{
	// This formula is rearranged to get rid of the need for floating point
	// arithmetic. The real way to understand the formula is to use
	// (dx / x pixels per inch) to get the inches and then multiply
	// the inches by the number of himetric units per inch to get the
	// count of himetric units.
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
inline LONG DYtoHimetricY2(LONG dy, LONG yPerInch)
{
	// This formula is rearranged to get rid of the need for floating point
	// arithmetic. The real way to understand the formula is to use
	// (dy / y pixels per inch) to get the inches and then multiply
	// the inches by the number of himetric units per inch to get the
	// count of himetric units.
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

BOOL GetIconic2(HWND hwnd)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "GetIconic2");

	while (hwnd)
	{
		if (::IsIconic(hwnd))
		{
			return TRUE;
		}
		hwnd = GetParent(hwnd);
	}
	return FALSE;
}

void DrawRectFn2(
	HDC hdc,
	RECT *prc,
	INT icrTL,
	INT icrBR,
	BOOL fBot,
	BOOL fRght)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "DrawRectFn2");

	COLORREF cr;
	COLORREF crSave;
	RECT rc;

	cr = GetSysColor(icrTL);
	crSave = SetBkColor(hdc, cr);

	// top
	rc = *prc;
	rc.bottom = rc.top + 1;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

	// left
	rc.bottom = prc->bottom;
	rc.right = rc.left + 1;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

	if (icrTL != icrBR)
	{
		cr = GetSysColor(icrBR);
		SetBkColor(hdc, cr);
	}

	// right
	rc.right = prc->right;
	rc.left = rc.right - 1;
	if (!fBot)
	{
		rc.bottom -= cyHScroll;
	}
	if (fRght)
	{
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}

	// bottom
	if (fBot)
	{
		rc.left = prc->left;
		rc.top = rc.bottom - 1;
		if (!fRght)
		{
			rc.right -= cxVScroll;
		}
		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	SetBkColor(hdc, crSave);
}

/*
 *	GetECDefaultHeightAndWidth2
 *
 *	@mfunc	Helper for host to get ave char width and height for default character set
 *			for the control.
 *
 *	@rdesc	Height of default character set
 *
 *	@devnote:
 *			This really only s/b called by the window's host.
 *
 */
LONG GetECDefaultHeightAndWidth2(
	ITextServices *pts,			//@parm ITextServices to conver to CTxtEdit.
	HDC hdc,					//@parm DC to use for retrieving the font.
	LONG lZoomNumerator,		//@parm Zoom numerator
	LONG lZoomDenominator,		//@parm Zoom denominator
	LONG yPixelsPerInch,		//@parm Pixels per inch for hdc
	LONG *pxAveWidth,			//@parm Optional ave width of character
	LONG *pxOverhang,			//@parm Optional overhang
	LONG *pxUnderhang)			//@parm Optional underhang
{
	//L // Convert the text
	//CTxtEdit *ped = (CTxtEdit *) pts;

	//// Get the CCcs that has all the information we need
	//CCcs *pccs = fc().GetCcs(hdc, ped->GetCharFormat(-1), lZoomNumerator,
	//		lZoomDenominator, yPixelsPerInch);

	//if (pxAveWidth != NULL)
	//{
	//	// Return the information
	//	*pxAveWidth = pccs->_xAveCharWidth;
	//}

	//if (pxOverhang != NULL)
	//{
	//	// Return the overhang
	//	*pxOverhang = pccs->_xOverhang;
	//}

	//if (pxUnderhang != NULL)
	//{
	//	// Return the overhang
	//	*pxUnderhang = pccs->_xUnderhang;
	//}

	//LONG yHeight = pccs->_yHeight;

	//// Release the CCcs
	//pccs->Release();

	//return yHeight;

	return 20;
}


//////////////// CTxtWinHost Creation/Initialization/Destruction ///////////////////////
// 微软ITextHost代码
/*
 *	CTxtWinHost::OnCreate (hwnd, pcs)
 *
 *	@mfunc
 *		Static global method to handle WM_CREATE message
 *
 *	@devnote
 *		phost ptr is stored in window data (GetWindowLong())
 */
void CTxtWinHost::OnNCDestroy(
	CTxtWinHost *phost)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnNCDestroy");

	phost->Shutdown();
	phost->Release();
}

CTxtWinHost::CTxtWinHost()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::CTxtWinHost");

	m_pserv = NULL;
	m_rcViewInset.left = m_rcViewInset.top = m_rcViewInset.right = m_rcViewInset.bottom = 0;
	m_fRegisteredForDrop = FALSE;
	m_crefs = 1;
	if (!m_fNotSysBkgnd)
	{
		m_crBackground = GetSysColor(COLOR_WINDOW);
	}
	m_hRichEdit20Mod = NULL;
	m_hRichTextBox = NULL;
	_hwndParent = NULL;
	m_hwndListBox = NULL;
}

CTxtWinHost::~CTxtWinHost()
{
	if (m_pserv)
	{
		Shutdown();
	}
}

// 关闭对象, 但不释放内存
void CTxtWinHost::Shutdown()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::Shutdown");

	RevokeDragDrop();						// Revoke our drop target

	if (m_pserv)
	{
		m_pserv->OnTxInPlaceDeactivate();
		m_pserv->Release();
		m_pserv = NULL;
	}

#ifdef MACPORT
	if (fHaveIMMProcs)
	{
		ImmTerminateForMac();					// terminate IMM for the mac
	}
#endif //MACPORT

	if (NULL != m_hRichEdit20Mod)
	{
		FreeLibrary(m_hRichEdit20Mod);
		m_hRichEdit20Mod = NULL;
	}

	m_pserv = NULL;
}

/*
 *	CTxtWinHost::Init (hwnd, pcs)
 *
 *	@mfunc
 *		Initialize this CTxtWinHost
 */
BOOL CTxtWinHost::Init(HWLWND hRichTextBox, const CREATESTRUCT *pcs)
{
	if (hRichTextBox == NULL || pcs == NULL)
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	HDC hDCN = ::GetDC(::GetDesktopWindow());
	xPerInchScreenDC = GetDeviceCaps(hDCN, LOGPIXELSX);
	yPerInchScreenDC = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	m_hRichTextBox = hRichTextBox;

	// Edit controls created without a window are multiline by default
	// so that paragraph formats can be
	m_dwStyle = ES_MULTILINE;
	m_fHidden = TRUE;

	if (pcs)
	{
		_hwndParent = pcs->hwndParent;
		m_dwExStyle	= pcs->dwExStyle;
		m_dwStyle	= pcs->style;

		if (pcs->style & ES_COMBOBOX)
		{
			HWLWND hParent = GetParent(hRichTextBox);
			if (hParent != NULL)
			{
				// 如果RichTextBox的父控件是ComboBox，则保存ComboBox的下拉框
				m_hwndListBox = (HWND)SendMessage(hParent, CBM_GETDROPPEDWND, 0, 0);
			}
		}

		// According to the edit control documentation WS_HSCROLL implies that
		// ES_AUTOSCROLL is set and WS_VSCROLL implies that ES_AUTOVSCROLL is
		// set. Here, we make this so.
		if ((m_dwStyle & WS_HSCROLL) != 0)
		{
			m_dwStyle |= ES_AUTOHSCROLL;
		}

		if ((m_dwStyle & WS_VSCROLL) != 0)
		{
			m_dwStyle |= ES_AUTOVSCROLL;
		}

		m_fBorder = !!(m_dwStyle & WS_BORDER);

		if ((m_dwStyle & ES_SUNKEN) || (m_dwExStyle & WS_EX_CLIENTEDGE))
		{
			m_fBorder = TRUE;
		}

		// handle default passwords
		if ((m_dwStyle & ES_PASSWORD))
		{
			m_chPassword = TEXT('*');
		}

		// handle default disabled
		if ((m_dwStyle & WS_DISABLED))
		{
			m_fDisabled = TRUE;
		}
	}

	if (0 == _xWidthSys)
	{
		// Init system metrics
		HDC hdc = GetDC(GetHostWnd(m_hRichTextBox));

		if (!hdc)
		{
			return FALSE;
		}

		HFONT hfontOld = (HFONT)SelectObject(hdc, GetStockObject(SYSTEM_FONT));
		if (hfontOld == NULL)
		{
			return FALSE;
		}

		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		SelectObject(hdc, hfontOld);

		_xWidthSys = (INT) tm.tmAveCharWidth;
		_yHeightSys = (INT) tm.tmHeight;

		ReleaseDC(GetHostWnd(m_hRichTextBox), hdc);
	}

	m_xInset = _xWidthSys / 2;
	m_yInset = _yHeightSys / 4;

	// At this point the border flag is set and so is the pixels per inch
	// so we can initalize the inset.
	SetDefaultInset();

	IUnknown *pUnk = NULL;
#ifndef DEBUG_RICHEDIT
	PCreateTextServices CreateTextServices = NULL;
	//msftedit.dll是最新版本的richedit支持多国语言输入（如泰语等），而Riched20.dll是不支持的（比如泰语等）这个可以测试一下。
	//这里使用老的Riched20是因为在win8下面插入链接不显示
	//这个问题暂时先这样处理，以后看看能不能找到win8下为什么msftedit.dll不能插入链接的根本原因。
	//HMODULE hmod = LoadLibrary(_T("msftedit.dll"));
	m_hRichEdit20Mod = LoadLibrary(_T("Riched20.dll"));
	if (NULL != m_hRichEdit20Mod)
	{
		CreateTextServices = (PCreateTextServices)GetProcAddress(m_hRichEdit20Mod, "CreateTextServices");
	}

	// Create Text Services component
	// CreateTextServices内部，会按顺序调用
	// ITextHost.TxGetCharFormat;
	// ITextHost.TxGetParaFormat;
	// ITextHost.TxGetPropertyBits, 是否包含下面这些标记(TXTBITS | TXTBIT_MULTILINE | TXTBIT_SHOWACCELERATOR)
	// ITextHost.TxGetBackStyle，结果是否TXTBACK_TRANSPARENT;
	// ITextHost.TxGetMaxLength
	// 来请求默认值，而CreateTextServices内部的ITextHost，就是本类（CTextHost）的实例
	// 所以，本类应该提供这些默认值
	if (FAILED(CreateTextServices(NULL, this, &pUnk)))
	{
		return FALSE;
	}
#else
	if (FAILED(CreateTextServices(NULL, this, &pUnk)))
	{
		return FALSE;
	}
#endif
	// Get text services interface
	HRESULT hr = pUnk->QueryInterface(IID_ITextServices, (void **)&m_pserv);

	// Regardless of whether the previous call succeeded or failed, we are
	// done with the private interface.
	pUnk->Release();

	if (FAILED(hr))
	{
		return FALSE;
	}

	PARAFORMAT PF;							// If center or right alignment,
	if (m_dwStyle & (ES_CENTER | ES_RIGHT))	//  tell text services
	{
		PF.cbSize = sizeof(PARAFORMAT);
		PF.dwMask = PFM_ALIGNMENT;
		PF.wAlignment = (m_dwStyle & ES_RIGHT) ? PFA_RIGHT : PFA_CENTER;
		m_pserv->TxSendMessage(EM_SETPARAFORMAT, SPF_SETDEFAULT, (LPARAM)&PF, NULL);
	}

#ifdef MACPORT
	if (fHaveIMMProcs)
	{

		if (!ImmInitializeForMac(hinstRE))
		{
#ifdef DEBUG
			OutputDebugStringA("Could not register Imm ImmInitializeForMac.\r\n");
#endif	// DEBUG
		}
	}
#endif //MACPORT


	return TRUE;
}

/*
 *	CTxtWinHost::OnCreate (hwnd, pcs)
 *
 *	@mfunc
 *		Handle WM_CREATE message
 *
 *	@rdesc
 *		LRESULT = -1 if failed to in-place activate; else 0
 */
LRESULT CTxtWinHost::OnCreate(const CREATESTRUCT *pcs)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnCreate");

	RECT rcClient;

	//L // sometimes, these values are -1 (from windows itself); just treat them
	// rcClient的坐标，不论是基于HWND还是Windowless的RichEdit，
	// 都是相对RichEdit父窗口的
	GetWindowRectToHWND(m_hRichTextBox, &rcClient);

	// Notify Text Services that we are in place active
	// 执行这句，才能看到光标，才能通过鼠标选中文本
	// 在OnTxInPlaceActivate内部，会调用TxGetDC，如果TxGetDC返回NULL，
	// OnTxInPlaceActivate就会失败。 所以，先提供一个临时的HDC
	if (FAILED(m_pserv->OnTxInPlaceActivate(&rcClient)))
	{
		return -1;
	}

	//L DWORD dwStyle = GetWindowLong(_hwnd, GWL_STYLE);

	//// Hide all scrollbars to start
	//if (_hwnd && !(m_dwStyle & ES_DISABLENOSCROLL))
	//{
	//	SetScrollRange(_hwnd, SB_VERT, 0, 0, TRUE);
	//	SetScrollRange(_hwnd, SB_HORZ, 0, 0, TRUE);

	//	dwStyle &= ~(WS_VSCROLL | WS_HSCROLL);
	//	SetWindowLong(_hwnd, GWL_STYLE, dwStyle);
	//}

	if (!(pcs->style & (ES_READONLY | ES_NOOLEDRAGDROP)))
	{
		// This isn't a read only window or a no drop window,
		// so we need a drop target.
		RegisterDragDrop();
	}

	m_usIMEMode = 0;
	if (pcs->style & ES_NOIME)
	{
		m_usIMEMode = ES_NOIME;
	}
	else if (pcs->style & ES_SELFIME)
	{
		m_usIMEMode = ES_SELFIME;
	}

	// turn off IME mode if necessary
	if ((pcs->style & ES_READONLY) || (m_usIMEMode == ES_NOIME))
	{
		ImmAssociateNULLContext(TRUE);
	}
#ifdef MACPORT
	else if (fHaveIMMProcs)
	{
		// will have to switch to proc pointers when we dynamically link.
		HIMC hIMC;
		hIMC	= ImmCreateContext();
		/* hOldIMC = */ ::ImmAssociateContext(_hwnd, hIMC);
	}
#endif

	return 0;
}


/////////////////////////////////  IUnknown ////////////////////////////////

HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppv)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::QueryInterface");

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = (IUnknown *)this;
	}
	else if (IsEqualIID(riid, IID_ITextHost))
	{
		*ppv = (ITextHost *)this;
	}
	//else if (IsEqualIID(riid, IID_ITextHost2))
	//{
	//	*ppv = (ITextHost2 *)this;
	//}
	else
	{
		*ppv = NULL;
	}

	if (*ppv)
	{
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}

ULONG CTxtWinHost::AddRef(void)
{
	//TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::AddRef");

	return ++m_crefs;
}

ULONG CTxtWinHost::Release(void)
{
	//TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::Release");

	--m_crefs;

	if (m_crefs == 0)
	{
		delete this;
		return 0;
	}

	return m_crefs;
}


//////////////////////////////// Activation ////////////////////////////////

//////////////////////////////// Properties ////////////////////////////////


IUITXTEFFECT CTxtWinHost::TxGetEffects() const
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetEffects");

	if ((m_dwStyle & ES_SUNKEN) || (m_dwExStyle & WS_EX_CLIENTEDGE))
	{
		return IUITXTEFFECT_SUNKEN;
	}
	else
	{
		return IUITXTEFFECT_NONE;
	}
}



//////////////////////////// System API wrapper ////////////////////////////



///////////////////////  Windows message dispatch methods  ///////////////////////////////


/*
 *	CTxtWinHost::TxWindowProc (hwnd, msg, wparam, lparam)
 *
 *	@mfunc
 *		Handle window messages pertinent to the host and pass others on to
 *		text services. Called by RichEditWndProc() in remain.c.
 *
 *	#rdesc
 *		LRESULT = (code processed) ? 0 : 1
 */
LRESULT CTxtWinHost::TxWindowProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg != WM_TIMER
		&& msg != WM_MOUSEMOVE
		&& msg != WM_NCHITTEST)
	{
		// 		char szBuf[MAX_PATH] = {0};
		// 		sprintf(szBuf, "CTxtWinHost::TxWindowProc message is 0x%04x", msg);
		// 		TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, szBuf);
	}

	LRESULT	lres = 0;
	HRESULT hr;

	// stabilize ourselves
	AddRef();

	// Handle mouse/keyboard message filter notifications
	if (m_fKeyMaskSet || m_fMouseMaskSet || m_fScrollMaskSet)
	{
		// We may need to fire a MSGFILTER notification.  In
		// the tests below, we check to see if either mouse
		// or key events are hit.  If so, then we fire the msgfilter
		// notification.  The list of events was generated from
		// RichEdit 1.0 sources.

		MSGFILTER msgfltr;

		/* RichEdit Compatibility issue: The following code gets all
		 * keyboard and mouse actions, but the RichEdit 1.0 code only got
		 * WM_KEYDOWN, WM_KEYUP, WM_CHAR, WM_SYSKEYDOWN, WM_SYSKEYUP,
		 * WM_MOUSEACTIVATE, WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MOUSEMOVE,
		 * WM_RBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP
		 */

		if ((m_fKeyMaskSet && (msg >= WM_KEYFIRST && msg <= WM_KEYLAST)) ||
			(m_fMouseMaskSet && ((msg >= WM_MOUSEFIRST &&
						msg <= WM_MOUSELAST) || msg == WM_MOUSEACTIVATE)) ||
			(m_fScrollMaskSet && ((msg == WM_HSCROLL) || (msg == WM_VSCROLL))))
		{
			ZeroMemory(&msgfltr, sizeof(MSGFILTER));
			msgfltr.msg = msg;
			msgfltr.wParam = wparam;
			msgfltr.lParam = lparam;

			//
			// The MSDN document on MSGFILTER is wrong, if the
			// send message returns 0 (NOERROR via TxNotify in this
			// case), it means process the event.  Otherwise, return.
			//
			// The documentation states the reverse.
			//
			if (TxNotify(EN_MSGFILTER, &msgfltr) == NOERROR)
			{
				// Since client is allowed to modify the contents of
				// msgfltr, we must use the returned values.
				msg	   = msgfltr.msg;
				wparam = msgfltr.wParam;
				lparam = msgfltr.lParam;
			}
			else
			{
				goto Exit;
			}
		}
	}

	switch (msg)
	{
	case EM_SETEVENTMASK:
		m_fKeyMaskSet = !!(lparam & ENM_KEYEVENTS);
		m_fMouseMaskSet = !!(lparam & ENM_MOUSEEVENTS);
		m_fScrollMaskSet = !!(lparam & ENM_SCROLLEVENTS);
		goto serv;

		break;

	case EM_SETSEL:

		// When we are in a dialog box that is empty, EM_SETSEL will not select
		// the final always existing EOP if the control is rich.
		//L if (m_fUseSpecialSetSel
		//	&& (((CTxtEdit *)m_pserv)->GetAdjustedTextLength() == 0)
		//	&& (wparam != (WPARAM) - 1))
		//{
		//	lparam = 0;

		//	if (wparam != 0)
		//	{
		//		wparam = 0;
		//	}
		//}

		goto serv;

	case WM_CREATE:
		lres = OnCreate((CREATESTRUCT *)lparam);
		break;

	case WM_KEYDOWN:
		if ((WORD) wparam == VK_PROCESSKEY)
		{
			// 准备通过两个WM_CHAR消息累积DBC
			SetAccumulateDBCMode(TRUE);
			m_chLeadByte = 0;
		}
		else
		{
			lres = OnKeyDown((WORD) wparam, (DWORD) lparam);
		}

		if (lres)							// Didn't process code:
		{
			goto serv;    //  give it to text services
		}

		return 1;

	case WM_CHAR:

		// check if we are waiting for 2 consecutive WM_CHAR messages
		if (IsAccumulateDBCMode())
		{
			if (!m_chLeadByte)
			{
				// This is the first WM_CHAR message,

				if (!IsDBCSLeadByte((BYTE)(WORD)wparam))
					// this is not a Lead byte, terminate this special mode for any other message
				{
					SetAccumulateDBCMode(FALSE);
				}
				else
				{
					// save the Lead Byte and don't process this message
					m_chLeadByte = (WORD)wparam << 8 ;
					goto Exit;
				}
			}
			else
			{
				// This is the second WM_CHAR message,
				// combine the current byte with previous byte.
				// This DBC will be handled as WM_IME_CHAR.
				wparam |= m_chLeadByte;
				msg = WM_IME_CHAR;

				// terminate this special mode
				SetAccumulateDBCMode(FALSE);
				goto serv;						//  give it to text services
			}
		}

		lres = OnChar((WORD) wparam, (DWORD) lparam);
		if (lres)							// Didn't process code:
		{
			goto serv;    //  give it to text services
		}
		break;

	case WM_ENABLE:

		if ((wparam == 0) != (m_fDisabled != 0))
		{
			// Stated of window changed so invalidate it so it will
			// get redrawn.
			InvalidateRect(m_hRichTextBox, NULL);
			SetScrollBarsForWmEnable(wparam);
		}

		// Set the disabled flag
		m_fDisabled = (0 == wparam);

		// Return value for message
		lres = 0;

	case WM_SYSCOLORCHANGE:
		OnSysColorChange();
		goto serv;							// Notify text services that
	//  system colors have changed
	case WM_GETDLGCODE:
		lres = OnGetDlgCode(wparam, lparam);
		break;

	case EM_GETOPTIONS:
		lres = OnGetOptions();
		break;

	case EM_GETPASSWORDCHAR:
		lres = m_chPassword;
		break;

	case EM_GETRECT:
		OnGetRect((LPRECT)lparam);
		break;

	case EM_HIDESELECTION:
		if ((BOOL)lparam)
		{
			DWORD dwPropertyBits = 0;

			if ((BOOL)wparam)
			{
				m_dwStyle &= ~(DWORD) ES_NOHIDESEL;
				dwPropertyBits = TXTBIT_HIDESELECTION;
			}
			else
			{
				m_dwStyle |= ES_NOHIDESEL;
			}

			// Notify text services of change in status.
			m_pserv->OnTxPropertyBitsChange(TXTBIT_HIDESELECTION,
				dwPropertyBits);
		}
		goto serv;

	case EM_SETBKGNDCOLOR:
		lres = (LRESULT) m_crBackground;
		m_fNotSysBkgnd = !wparam;
		m_crBackground = (COLORREF) lparam;

		if (wparam)
		{
			m_crBackground = GetSysColor(COLOR_WINDOW);
		}

		if (lres != (LRESULT) m_crBackground)
		{
			// Notify text services that color has changed
			LRESULT	lres1 = 0;
			m_pserv->TxSendMessage(WM_SYSCOLORCHANGE, 0, 0, &lres1);
			TxInvalidateRect(NULL, TRUE);
		}

		break;

	case WM_STYLECHANGING:
		// Just pass this one to the default window proc
		break;

	case WM_STYLECHANGED:
		//
		// For now, we only interested in GWL_EXSTYLE Transparent mode changed.
		// This is to fix Bug 753 since Window95 is not passing us
		// the WS_EX_TRANSPARENT.
		//
		lres = 1;
		if (GWL_EXSTYLE == wparam)
		{
			LPSTYLESTRUCT	lpss = (LPSTYLESTRUCT) lparam;
			if (IsTransparentMode() != (BOOL)(lpss->styleNew & WS_EX_TRANSPARENT))
			{
				m_dwExStyle = lpss->styleNew;

				TxInvalidateRect(NULL, FALSE);

				// return 0 to indicate we have handled this
				// message
				lres = 0;
			}
		}
		break;

	case EM_SETOPTIONS:
		OnSetOptions((WORD) wparam, (DWORD) lparam);
		lres = (m_dwStyle & ECO_STYLES);
		if (m_fEnableAutoWordSel)
		{
			lres |= ECO_AUTOWORDSELECTION;
		}
		break;

	case EM_SETPASSWORDCHAR:
		if (m_chPassword != (TCHAR)wparam)
		{
			m_chPassword = (TCHAR)wparam;
			m_pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD,
				m_chPassword ? TXTBIT_USEPASSWORD : 0);
		}
		break;

	case EM_SETREADONLY:
		ImmAssociateNULLContext(BOOL(wparam) || (m_usIMEMode == ES_NOIME));
		OnSetReadOnly(BOOL(wparam));
		lres = 1;
		break;

	case EM_SETRECTNP:						// Can implement: identical to
		break;								//  EM_SETRECT, except that edit
	//  control window isn't redrawn
	case EM_SETRECT:
		// TODO: RichEdit版本不同，参数的意义也不同
		OnSetRect((LPRECT)lparam, wparam == 1);
		break;

	case WM_SIZE:
		m_pserv->TxSendMessage(msg, wparam, lparam, &lres);
		lres = OnSize(NULL, wparam, LOWORD(lparam), HIWORD(lparam));
		break;

	//L	case WM_WINDOWPOSCHANGING:
	//#ifdef DBCS
	//		dwStyleOrg = OnRemoveFeStyles();
	//		lres = ::DefWindowProc(hwnd, msg, wparam, lparam);
	//		if (dwStyleOrg != 0xffffffff)
	//		{
	//			SetWindowLong(_hwnd, GWL_STYLE, dwStyleOrg);
	//		}
	//#else
	//		lres = ::DefWindowProc(GetHostWnd(m_hRichTextBox), msg, wparam, lparam);
	//#endif						// not DBCS
	//		if (TxGetEffects() == TXTEFFECT_SUNKEN)
	//		{
	//			OnSunkenWindowPosChanging(hwnd, (WINDOWPOS *) lparam);
	//		}
	//		break;

	case WM_SETCURSOR:
		//			Only set cursor when over us rather than a child; this
		//			helps prevent us from fighting it out with an inplace child
		if ((HWND)wparam == GetHostWnd(m_hRichTextBox))
		{
			// 不要调用DefWindowProc，否则光标在窗口类光标与脱字符光标间来回切换
			POINT pt;
			GetCursorPos(&pt);
			::ScreenToClient(GetHostWnd(m_hRichTextBox), &pt);
			m_pserv->OnTxSetCursor(
				DVASPECT_CONTENT,
				-1,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,			// Client rect - no redraw
				pt.x,
				pt.y);
			lres = TRUE;
		}
		break;

	case WM_SHOWWINDOW:
		hr = OnTxVisibleChange((BOOL)wparam);
		break;

	//L	case WM_NCPAINT:
	//
	//#ifdef DBCS
	//		dwStyleOrg = RemoveFeStyle();
	//		lres = ::DefWindowProc(hwnd, msg, wparam, lparam);
	//		if (dwStyleOrg != 0xffffffff)
	//		{
	//			SetWindowLong(hwnd, GWL_STYLE, dwStyleOrg);
	//		}
	//#else
	//		lres = ::DefWindowProc(hwnd, msg, wparam, lparam);
	//#endif					// NOT DBCS
	//		if ((TxGetEffects() == TXTEFFECT_SUNKEN)
	//			&& (dwMajorVersion < VERS4))
	//		{
	//			HDC hdc = GetDC(hwnd);
	//
	//			if (hdc)
	//			{
	//				DrawSunkenBorder(hwnd, hdc);
	//				ReleaseDC(hwnd, hdc);
	//			}
	//		}
	//		break;

	//L由RichTextBox::OnDraw绘制	case WM_PAINT:
	//	{
	//		PAINTSTRUCT ps;
	//		HPALETTE hpalOld = NULL;
	//		HDC hdc = BeginPaint(hwnd, &ps);
	//		RECT rcClient;
	//
	//#ifdef DEBUG
	//		// We only want to paint if we are not asserting because
	//		// the assert might be in the paint code itself.
	//		if (!fInAssert)
	//		{
	//#endif // DEBUG
	//
	//			// Set up the palette for drawing our data
	//			if (m_hpal != NULL)
	//			{
	//				hpalOld = SelectPalette(hdc, m_hpal, FALSE);
	//				RealizePalette(hdc);
	//			}
	//
	//			// Since we are using the CS_PARENTDC style, make sure
	//			// the clip region is limited to our client window.
	//			GetClientRect(hwnd, &rcClient);
	//			SaveDC(hdc);
	//			IntersectClipRect(hdc, rcClient.left, rcClient.top, rcClient.right,
	//				rcClient.bottom);
	//
	//
	//
	//			m_pserv->TxDraw(
	//				DVASPECT_CONTENT,  		// Draw Aspect
	//				-1,						// Lindex
	//				NULL,					// Info for drawing optimazation
	//				NULL,					// target device information
	//				hdc,					// Draw device HDC
	//				NULL, 				   	// Target device HDC
	//				(const RECTL *) &rcClient,// Bounding client rectangle
	//				NULL, 					// Clipping rectangle for metafiles
	//				&ps.rcPaint,			// Update rectangle
	//				NULL, 	   				// Call back function
	//				NULL,					// Call back parameter
	//				TXTVIEW_ACTIVE);		// What view - the active one!
	//
	//			// Restore the palette if there is one.
	//			if (hpalOld != NULL)
	//			{
	//				SelectPalette(hdc, hpalOld, TRUE);
	//			}
	//
	//			if ((TxGetEffects() == TXTEFFECT_SUNKEN)
	//				&& (dwMajorVersion < VERS4))
	//			{
	//				DrawSunkenBorder(hwnd, hdc);
	//			}
	//
	//#ifdef DEBUG
	//			// See above comment
	//		}
	//#endif // DEBUG
	//
	//		RestoreDC(hdc, -1);
	//
	//		EndPaint(hwnd, &ps);
	//	}
	//	break;

	case EM_SETMARGINS:

		OnSetMargins(wparam, LOWORD(lparam), HIWORD(lparam));
		break;

	case EM_SETPALETTE:

		// Application is setting a palette for us to use.
		m_hpal = (HPALETTE) wparam;

		// Invalidate the window & repaint to reflect the new palette.
		InvalidateRect(m_hRichTextBox, NULL);

		break;

	case WLM_SET_BK_IMAGE:
	{
		BOOL bChecked = (LOWORD(wparam) == BST_CHECKED);
		CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wparam);
		LPCTSTR lpszImage = LPCTSTR(lparam);

		if (eControlState >= CONTROL_STATE_NORMAL
			&& eControlState <= CONTROL_STATE_DISABLED)
		{
			int nStateIndex = GetButtonStateIndex(eControlState);

			if (!bChecked)
			{
				REBox *pREBox = (REBox *)GetMember(m_hRichTextBox);

				return IUISetControlImage(GetProject(m_hRichTextBox),
						&pREBox->m_himgBk[nStateIndex],
						lpszImage);
			}
		}
		else if (eControlState == CONTROL_STATE_COMBINE)
		{
			REBox *pREBox = (REBox *)GetMember(m_hRichTextBox);

			return IUISetControlImage(GetProject(m_hRichTextBox),
					&pREBox->hwnd->ci.m_himgCombineBk,
					lpszImage);
		}
	}
	return 0;

	case WLM_GET_BK_IMAGE:
	{
		BOOL bChecked = (LOWORD(wparam) == BST_CHECKED);
		CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wparam);

		if (eControlState >= CONTROL_STATE_NORMAL
			&& eControlState <= CONTROL_STATE_DISABLED)
		{
			int nStateIndex = GetButtonStateIndex(eControlState);

			if (!bChecked)
			{
				REBox *pREBox = (REBox *)GetMember(m_hRichTextBox);
				return (LRESULT)(LPCTSTR)pREBox->m_himgBk[nStateIndex]->GetSafeImageName();
			}
		}
		else if (eControlState == CONTROL_STATE_COMBINE)
		{
			REBox *pREBox = (REBox *)GetMember(m_hRichTextBox);
			return (LRESULT)(LPCTSTR)pREBox->hwnd->ci.m_himgCombineBk->GetSafeImageName();
		}
	}
	return 0;

	default:
serv:
		{
			if (m_usIMEMode == ES_SELFIME)
			{
				// host is doing IME, ignore IME related messages.
				switch (msg)
				{
				case WM_IME_STARTCOMPOSITION:	// IME has begun interpreting user input
				case WM_IME_COMPOSITION:		// State of user's input has changed
				case WM_IME_ENDCOMPOSITION:		// User has OK'd IME conversions
				case WM_IME_NOTIFY:				// Candidate window state change info, etc.
				case WM_IME_COMPOSITIONFULL:	// Level 2 comp string about to overflow.
					lres = ::DefWindowProc(GetHostWnd(m_hRichTextBox), msg, wparam, lparam);
					goto Exit;
				}
			}

			if (m_pserv != NULL)
			{
				hr = m_pserv->TxSendMessage(msg, wparam, lparam, &lres);
			}

			if (hr == S_FALSE)
			{
				// Message was not processed by text services so send it
				// to the default window proc.
				// lres = ::DefWindowProc(GetHostWnd(m_hRichTextBox), msg, wparam, lparam);
				// TODO: 在RichEdit处理完WM_WINDOWPOSCHANGED后，如果调用::DefWindowProc，
				// Window类会刷新窗口，刷新窗口时，会重新布局，然后又会布局RichEdit，导致反复刷新。这块还得仔细研究
				lres = 0;
			}
		}
	}

	// Special border processing. The inset changes based on the size of the
	// defautl character set. So if we got a message that changes the default
	// character set, we need to update the inset.
	if (m_fBorder)
	{
		if ((msg == WM_SETFONT)
			|| ((msg == EM_SETCHARFORMAT) && (wparam == 0)))
		{
			ResizeInset();
		}
	}

Exit:
	Release();

	return lres;
}


///////////////////////////////  Keyboard Messages  //////////////////////////////////

/*
 *	CTxtWinHost::OnKeyDown (vkey, dwFlags)
 *
 *	@mfunc
 *		Handle WM_KEYDOWN messages that need to send a message to the parent
 *		window (may happen when control is in a dialog box)
 *
 *	#rdesc
 *		LRESULT = (code processed) ? 0 : 1
 */
LRESULT CTxtWinHost::OnKeyDown(
	WORD	vkey,			//@parm WM_KEYDOWN wparam (virtual key code)
	DWORD	dwFlags)		//@parm WM_KEYDOWN flags
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnKeyDown");

	if (!m_fInDialogBox) 					// Not in a dialog box
	{
		return 1;    // Signal key-down msg not processed
	}

	BOOL fIsListVisible = FALSE;
	BOOL fIsExtendedUI = FALSE;

	HWLWND hParent = GetParent(m_hRichTextBox);

	switch (vkey)
	{
	//case VK_ESCAPE:
	//	PostMessage(_hwndParent, WM_CLOSE, 0, 0);
	//	return 0;

	case VK_RETURN:
		if (!(GetKeyState(VK_CONTROL) & 0x8000) && !(m_dwStyle & ES_WANTRETURN))
		{
			// Send to default button
			HWND	hwndT;
			LRESULT id = ::SendMessage(_hwndParent, DM_GETDEFID, 0, 0);

			if (LOWORD(id) &&
				(hwndT = GetDlgItem(_hwndParent, LOWORD(id))))
			{
				::SendMessage(_hwndParent, WM_NEXTDLGCTL, (WPARAM) hwndT, (LPARAM) 1);
				if (Window::GetWLFocus(GetHostWnd(m_hRichTextBox)) != m_hRichTextBox)
				{
					PostMessage(hwndT, WM_KEYDOWN, (WPARAM) VK_RETURN, 0);
				}
			}
			return 0;
		}
		break;

	case VK_TAB:
		if (!(GetKeyState(VK_CONTROL) & 0x8000))
		{
			::SendMessage(_hwndParent, WM_NEXTDLGCTL,
				!!(GetKeyState(VK_SHIFT) & 0x8000), 0);
			return 0;
		}
		break;

	// For ComboBox 下拉列表
	case VK_UP:
		if (m_hwndListBox != NULL)
		{

			/*
			 * Handle Combobox support
			 */
			if (hParent != NULL)
			{
				fIsExtendedUI = (BOOL)SendMessage(hParent, CB_GETEXTENDEDUI, 0, 0);
				fIsListVisible = (BOOL)SendMessage(hParent, CB_GETDROPPEDSTATE, 0, 0);

				if (!fIsListVisible && fIsExtendedUI)
				{

					/*
					 * For TandyT
					 */
DropExtendedUIListBox:

					/*
					 * Since an extendedui combo box doesn't do anything on f4, we
					 * turn off the extended ui, send the f4 to drop, and turn it
					 * back on again.
					 */
					SendMessage(hParent, CB_SETEXTENDEDUI, 0, 0);
					::SendMessage(m_hwndListBox, WM_KEYDOWN, VK_F4, 0);
					SendMessage(hParent, CB_SETEXTENDEDUI, 1, 0);
					return 0;
				}
				else
				{
					goto SendKeyToListBox;
				}
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		break;

	case VK_DOWN:
		if (m_hwndListBox != NULL)
		{

			/*
			 * Handle Combobox support
			 */
			if (hParent != NULL)
			{
				fIsExtendedUI = (BOOL)SendMessage(hParent, CB_GETEXTENDEDUI, 0, 0);
				fIsListVisible = (BOOL)SendMessage(hParent, CB_GETDROPPEDSTATE, 0, 0);

				if (!fIsListVisible && fIsExtendedUI)
				{

					/*
					 * For TandyT
					 */
					goto DropExtendedUIListBox;
				}
				else
				{
					goto SendKeyToListBox;
				}
			}
		}
		break;

	case VK_F4:
	case VK_PRIOR:
	case VK_NEXT:
		/*
		 * Send keys to the listbox if we are a part of a combo box. This
		 * assumes the listbox ignores keyup messages which is correct right
		 * now.
		 */
SendKeyToListBox:
		if (m_hwndListBox != NULL)
		{

			/*
			 * Handle Combobox support
			 */
			::SendMessage(m_hwndListBox, WM_KEYDOWN, vkey, 0L);
			return 0;
		}
		break;
	}

	return 1;
}

/*
 *	CTxtWinHost::OnChar (vkey, dwFlags)
 *
 *	@mfunc
 *		Eat some WM_CHAR messages for a control in a dialog box
 *
 *	#rdesc
 *		LRESULT = (code processed) ? 0 : 1
 */
LRESULT CTxtWinHost::OnChar(
	WORD	vkey,			//@parm WM_CHAR wparam (translated key code)
	DWORD	dwFlags)		//@parm WM_CHAR flags
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnChar");

	if (!m_fInDialogBox || (GetKeyState(VK_CONTROL) & 0x8000))
	{
		return 1;
	}

	switch (vkey)
	{
	case 'J' - 0x40:					// Ctrl-Return generates Ctrl-J (LF):
	case VK_RETURN:						//  treat it as an ordinary return
	case VK_ESCAPE:
		// 如果RichTextBox作为ComboBox的孩子，并且ComboBox的下拉框可见
		// 则把消息转发给ComboBox
		if (vkey == VK_RETURN || vkey == VK_ESCAPE)
		{
			HWLWND hParent = GetParent(m_hRichTextBox);
			if (m_hwndListBox != NULL && hParent != NULL)
			{
				if (SendMessage(hParent, CB_GETDROPPEDSTATE, 0, 0L))
				{
					SendMessage(hParent, WM_KEYDOWN, (WPARAM)vkey, 0L);
					break;
				}
			}
		}
		if (vkey != VK_ESCAPE)
		{
			if (!(m_dwStyle & ES_WANTRETURN))
			{
				return 0;    // Signal char processed (eaten)
			}
		}

		break;

	case VK_TAB:
		return 0;
	}

	return 1;							// Signal char not processed
}


/////////////////////////////////  View rectangle //////////////////////////////////////

void CTxtWinHost::OnGetRect(
	LPRECT prc)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnGetRect");

	RECT rcInset;
	LONG lSelBarWidth = 0;

	if (m_fEmSetRectCalled)
	{
		// Get the selection bar width and add it back to the view inset so
		// we return the rectangle that the application set.
		TxGetSelectionBarWidth(&lSelBarWidth);
	}

	// Get view inset (in HIMETRIC)
	TxGetViewInset(&rcInset);

	// Get client rect in pixels
	TxGetClientRect(prc);

	// Modify the client rect by the inset converted to pixels
	prc->left	+= HimetricXtoDX2(rcInset.left + lSelBarWidth, xPerInchScreenDC);
	prc->top	+= HimetricYtoDY2(rcInset.top,	 yPerInchScreenDC);
	prc->right	-= HimetricXtoDX2(rcInset.right,	 xPerInchScreenDC);
	prc->bottom -= HimetricYtoDY2(rcInset.bottom, yPerInchScreenDC);
}

void CTxtWinHost::OnSetRect(
	LPRECT prc,
	BOOL fNewBehavior)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSetRect");

	RECT rcClient;
	LONG lSelBarWidth;

	// Assuming this is not set to the default, turn on special EM_SETRECT
	// processing. The important part of this is that we subtract the selection
	// bar from the view inset because the EM_SETRECT rectangle does not
	// include the selection bar.
	m_fEmSetRectCalled = TRUE;

	if (!prc)
	{
		// We are back to the default so turn off special EM_SETRECT procesing.
		m_fEmSetRectCalled = FALSE;
		SetDefaultInset();
	}
	else
	{
		// For screen display, the following intersects new view RECT
		// with adjusted client area RECT
		TxGetClientRect(&rcClient);

		// Adjust client rect
		// Factors in space for borders
		if (m_fBorder)
		{
			rcClient.top		+= m_yInset;
			rcClient.bottom 	-= m_yInset - 1;
			rcClient.left		+= m_xInset;
			rcClient.right		-= m_xInset;
		}

		// prc是相对RichEdit的坐标，把它转成相对Host
		CIUIRect rc = prc;
		ClientToScreen(m_hRichTextBox, rc);
		IUIScreenToClient(GetHostWnd(m_hRichTextBox), rc);

		if (!fNewBehavior)
		{

			// Intersect the new view rectangle with the
			// adjusted client area rectangle
			if (!IntersectRect(&m_rcViewInset, &rcClient, rc))
			{
				m_rcViewInset = rcClient;
			}
		}
		else
		{
			m_rcViewInset = rc;
		}

		// Get the selection bar width
		TxGetSelectionBarWidth(&lSelBarWidth);

		// compute inset in pixels and convert to HIMETRIC.
		m_rcViewInset.left = DXtoHimetricX2(m_rcViewInset.left - rcClient.left, xPerInchScreenDC)
			- lSelBarWidth;
		m_rcViewInset.top = DYtoHimetricY2(m_rcViewInset.top - rcClient.top, yPerInchScreenDC);
		m_rcViewInset.right = DXtoHimetricX2(rcClient.right
				- m_rcViewInset.right, xPerInchScreenDC);
		m_rcViewInset.bottom = DYtoHimetricY2(rcClient.bottom
				- m_rcViewInset.bottom, yPerInchScreenDC);
	}

	m_pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE,
		TXTBIT_VIEWINSETCHANGE);
}


///////////////////////////////  System notifications  //////////////////////////////////

void CTxtWinHost::OnSysColorChange()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSysColorChange");

	crAuto = GetSysColor(COLOR_WINDOWTEXT);

	if (!m_fNotSysBkgnd)
	{
		m_crBackground = GetSysColor(COLOR_WINDOW);
	}
	TxInvalidateRect(NULL, TRUE);
}

/*
 *	CTxtWinHost::OnGetDlgCode (wparam, lparam)
 *
 *	@mfunc
 *		Handle some WM_GETDLGCODE messages
 *
 *	#rdesc
 *		LRESULT = dialog code
 */
LRESULT CTxtWinHost::OnGetDlgCode(
	WPARAM wparam,
	LPARAM lparam)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnGetDlgCode");

	LRESULT lres = DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_WANTTAB;

	if (m_dwStyle & ES_MULTILINE)
	{
		lres |= DLGC_WANTALLKEYS;
	}

	if (!(m_dwStyle & ES_SAVESEL))
	{
		lres |= DLGC_HASSETSEL;
	}

	// HACK: If we get one of these messages then we turn on the special
	// EM_SETSEL behavior. The problem is that m_fInDialogBox gets turned
	// on after the EM_SETSEL has occurred.
	m_fUseSpecialSetSel = TRUE;

	/*
	** -------------------------------------------- JEFFBOG HACK ----
	** Only set Dialog Box Flag if GETDLGCODE message is generated by
	** IsDialogMessage -- if so, the lParam will be a pointer to the
	** message structure passed to IsDialogMessage; otherwise, lParam
	** will be NULL.  Reason for the HACK alert:  the wParam & lParam
	** for GETDLGCODE is still not clearly defined and may end up
	** changing in a way that would throw this off
	** -------------------------------------------- JEFFBOG HACK ----
	 */
	if (lparam)
	{
		m_fInDialogBox = TRUE;
	}

	/*
	** If this is a WM_SYSCHAR message generated by the UNDO keystroke
	** we want this message so we can EAT IT in remain.c, case WM_SYSCHAR:
	 */
	if (lparam &&
		(((LPMSG) lparam)->message == WM_SYSCHAR) &&
		((DWORD)((LPMSG) lparam)->lParam & SYS_ALTERNATE) &&
		((WORD) wparam == VK_BACK))
	{
		lres |= DLGC_WANTMESSAGE;
	}

	return lres;
}


/////////////////////////////////  Other messages  //////////////////////////////////////

LRESULT CTxtWinHost::OnGetOptions() const
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnGetOptions");

	LRESULT lres = (m_dwStyle & ECO_STYLES);

	if (m_fEnableAutoWordSel)
	{
		lres |= ECO_AUTOWORDSELECTION;
	}

	return lres;
}

void CTxtWinHost::OnSetOptions(
	WORD  wOp,
	DWORD eco)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSetOptions");

	DWORD		dwChangeMask = 0;
	DWORD		dwProp = 0;
	DWORD		dwStyle;
	DWORD		dwStyleNew = m_dwStyle;
	const BOOL	fAutoWordSel = !!(eco & ECO_AUTOWORDSELECTION);

	// We keep track of the bits changed and then if any have changed we
	// query for all of our property bits and then send them. This simplifies
	// the code because we don't have to set all the bits specially. If the
	// code is changed to make the properties more in line with the new
	// model, we want to look at this code again.

	// Single line controls can't have a selection bar or do vertical writing
	if (!(m_dwStyle & ES_MULTILINE))
	{
#ifdef DBCS
		eco &= ~(ECO_SELECTIONBAR | ECO_VERTICAL);
#else
		eco &= ~ECO_SELECTIONBAR;
#endif
	}
	_ASSERT((DWORD)fAutoWordSel <= 1);			// Make sure that BOOL is 1/0
	dwStyle = (eco & ECO_STYLES);

	switch (wOp)
	{
	case ECOOP_SET:
		dwStyleNew			= (dwStyleNew & ~ECO_STYLES) | dwStyle;
		m_fEnableAutoWordSel = fAutoWordSel;
		break;

	case ECOOP_OR:
		dwStyleNew |= dwStyle;					// Setting a :1 flag = TRUE
		if (fAutoWordSel)						//  or FALSE is 1 instruction
		{
			m_fEnableAutoWordSel = TRUE;    // Setting it to a BOOL
		}
		break;									//  averages 9 instructions!

	case ECOOP_AND:
		dwStyleNew &= (dwStyle | ~ECO_STYLES);
		if (!fAutoWordSel)
		{
			m_fEnableAutoWordSel = FALSE;
		}
		break;

	case ECOOP_XOR:
		dwStyleNew ^= dwStyle;
		if (fAutoWordSel)
		{
			m_fEnableAutoWordSel ^= 1;
		}
		break;
	}

	if (m_fEnableAutoWordSel != (unsigned)fAutoWordSel)
	{
		dwChangeMask |= TXTBIT_AUTOWORDSEL;
	}

	if (dwStyleNew != m_dwStyle)
	{
		DWORD dwChange = dwStyleNew ^ m_dwStyle;

		AssertSz(!(dwChange & ~ECO_STYLES), "non-eco style changed");
		m_dwStyle = dwStyleNew;
		SetStyle(m_hRichTextBox, dwStyleNew);

		if (dwChange & ES_NOHIDESEL)
		{
			dwChangeMask |= TXTBIT_HIDESELECTION;
		}

		// these two local variables to use to keep track of
		// previous setting of ES_READONLY
		BOOL bNeedToSetIME = FALSE;
		BOOL bReadOnly = (m_dwStyle & ES_READONLY);

		if (dwChange & ES_READONLY)
		{
			dwChangeMask |= TXTBIT_READONLY;

			// Change drop target state as appropriate.
			if (dwStyleNew & ES_READONLY)
			{
				RevokeDragDrop();
			}
			else
			{
				RegisterDragDrop();
			}

			// setup bReadOnly so we will enable/disable IME
			// input mode according to the new ES_READONLY style
			bNeedToSetIME = TRUE;
			bReadOnly = (dwStyleNew & ES_READONLY);
		}

		if (dwChange & ES_VERTICAL)
		{
			dwChangeMask |= TXTBIT_VERTICAL;
		}

		if (dwChange & ES_NOIME)
		{
			m_usIMEMode = (dwStyleNew & ES_NOIME) ? 0 : ES_NOIME;
			bNeedToSetIME = TRUE;
		}
		else if (dwChange & ES_SELFIME)
		{
			m_usIMEMode = (dwStyleNew & ES_SELFIME) ? 0 : ES_SELFIME;
		}

		// we want to turn off IME if the new style is ES_READONLY or
		// ES_NOIME
		if (bNeedToSetIME)
		{
			ImmAssociateNULLContext(bReadOnly || (m_usIMEMode == ES_NOIME));
		}


		// no action require for ES_WANTRETURN
		// no action require for ES_SAVESEL
		// do this last
		if (dwChange & ES_SELECTIONBAR)
		{
			dwChangeMask |= TXTBIT_SELBARCHANGE;
		}
	}

	if (dwChangeMask)
	{
		TxGetPropertyBits(dwChangeMask, &dwProp);
		m_pserv->OnTxPropertyBitsChange(dwChangeMask, dwProp);
	}
}

void CTxtWinHost::OnSetReadOnly(
	BOOL fReadOnly)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSetReadOnly");

	DWORD dwT = GetStyle(m_hRichTextBox);
	DWORD dwUpdatedBits = 0;

	if (fReadOnly)
	{
		dwT |= ES_READONLY;
		m_dwStyle |= ES_READONLY;

		// Turn off Drag Drop
		RevokeDragDrop();
		dwUpdatedBits |= TXTBIT_READONLY;
	}
	else
	{
		dwT &= ~(DWORD) ES_READONLY;
		m_dwStyle &= ~(DWORD) ES_READONLY;

		// Turn drag drop back on
		RegisterDragDrop();
	}

	m_pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, dwUpdatedBits);

	SetStyle(m_hRichTextBox, dwT);
}


////////////////////////////////////  Helpers  /////////////////////////////////////////

void CTxtWinHost::SetDefaultInset()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::SetDefaultInset");

	// Generate default view rect from client rect
	if (m_fBorder)
	{
		// Factors in space for borders
		m_rcViewInset.top	= DYtoHimetricY2(m_yInset, yPerInchScreenDC);
		m_rcViewInset.bottom	= DYtoHimetricY2(m_yInset - 1, yPerInchScreenDC);
		m_rcViewInset.left	= DXtoHimetricX2(m_xInset, xPerInchScreenDC);
		m_rcViewInset.right	= DXtoHimetricX2(m_xInset, xPerInchScreenDC);
	}
	else
	{
		// Default the top and bottom inset to 0 and the left and right
		// to the size of the border.
		m_rcViewInset.top = 0;
		m_rcViewInset.bottom = 0;
		m_rcViewInset.left = DXtoHimetricX2(cxBorder, xPerInchScreenDC);
		m_rcViewInset.right = DXtoHimetricX2(cxBorder, xPerInchScreenDC);
	}
}


/////////////////////////////////  Far East Support  //////////////////////////////////////

//#ifdef WIN95_IME
HIMC CTxtWinHost::TxImmGetContext()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxImmGetContext");

	HIMC himc;

	_ASSERT(fHaveIMMProcs);

	_ASSERT(IsWindow(m_hRichTextBox));
	himc = pImmGetContext(GetHostWnd(m_hRichTextBox));
	_ASSERT(himc != NULL);
	return himc;
}

void CTxtWinHost::TxImmReleaseContext(
	HIMC himc)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxImmReleaseContext");

	_ASSERT(fHaveIMMProcs);
	_ASSERT(IsWindow(m_hRichTextBox));
	pImmReleaseContext(GetHostWnd(m_hRichTextBox), himc);
}

//#endif

void CTxtWinHost::RevokeDragDrop()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::RevokeDragDrop");

	if (m_fRegisteredForDrop)
	{
		// Note that if the revoke fails we want to know about this in debug
		// builds so we can fix any problems. In retail, we can't really do
		// so we just ignore it.
#ifdef DEBUG
		HRESULT hr =
#endif // DEBUG

			IUI::RevokeDragDrop(m_hRichTextBox);

		//L #ifdef DEBUG
		//		TESTANDTRACEHR(hr);
		//#endif // DEBUG

		m_fRegisteredForDrop = FALSE;
	}
}

void CTxtWinHost::RegisterDragDrop()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::RegisterDragDrop");

	IDropTarget *pdt = NULL;

	if (!m_fRegisteredForDrop && m_pserv->TxGetDropTarget(&pdt) == NOERROR)
	{
		// The most likely reason for RegisterDragDrop to fail is some kind of
		// bug in our program.

		HRESULT hr = IUI::RegisterDragDrop(m_hRichTextBox, pdt);

		if (hr == NOERROR)
		{
			m_fRegisteredForDrop = TRUE;
		}
		pdt->Release();
	}
}

/*
 *	void IMMAssociateNULLContext ( BOOL fReadOnly )
 *
 *	@func
 *		Enable/disable the IME Composition
 *
 *	@comm
 *		Called from the message loop to handle EM_SETREADONLY
 *		or when the style ES_NOIME is changed.
 *
 */
void CTxtWinHost::ImmAssociateNULLContext(BOOL fReadOnly)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::ImmAssociateNULLContext");

	if (!fHaveIMMProcs)
	{
		return;
	}

	// Notify text services that read only attribute has changed
	m_pserv->TxSendMessage(EM_SETREADONLY, fReadOnly, 0, NULL);

	_ASSERT(IsWindow(m_hRichTextBox));
	if (fReadOnly && !m_oldhimc)
	{
		// terminate IME Composition and save old himc
		m_oldhimc = pImmAssociateContext(GetHostWnd(m_hRichTextBox), (HIMC) NULL);
	}
	else if (fReadOnly == FALSE && m_oldhimc)
	{
		HIMC	prevhime;
		// restore previous himc and enable IME Composition
		prevhime = pImmAssociateContext(GetHostWnd(m_hRichTextBox), m_oldhimc);
		_ASSERT(!prevhime);

		m_oldhimc = (HIMC) NULL;
	}

	return;
}

#define cmultBorder 1

void CTxtWinHost::OnSunkenWindowPosChanging(
	HWND hwnd,
	WINDOWPOS *pwndpos)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSunkenWindowPosChanging");

	if (m_fVisible)
	{
		RECT rc;
		HWND hwndParent;

		::GetWindowRect(hwnd, &rc);
		InflateRect(&rc, cxBorder * cmultBorder, cyBorder * cmultBorder);
		hwndParent = ::GetParent(hwnd);
		MapWindowPoints(HWND_DESKTOP, hwndParent, (POINT *) &rc, 2);
		::InvalidateRect(hwndParent, &rc, FALSE);
	}
}

void CTxtWinHost::DrawSunkenBorder(
	HWND hwnd,
	HDC hdc)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::DrawSunkenBorder");
	BOOL fVScroll = (m_dwStyle & WS_VSCROLL);
	BOOL fHScroll = (m_dwStyle & WS_HSCROLL);

	RECT rc;
	RECT rcParent;
	HWND hwndParent;

	::GetWindowRect(hwnd, &rc);
	hwndParent = ::GetParent(hwnd);
	rcParent = rc;
	MapWindowPoints(HWND_DESKTOP, hwndParent, (POINT *)&rcParent, 2);
	InflateRect(&rcParent, cxBorder, cyBorder);
	OffsetRect(&rc, -rc.left, -rc.top);

	if (m_pserv)
	{
		// If we have a text control then get whether it thinks there are
		// scroll bars.
		m_pserv->TxGetHScroll(NULL, NULL, NULL, NULL, &fHScroll);
		m_pserv->TxGetVScroll(NULL, NULL, NULL, NULL, &fVScroll);
	}


	// draw inner rect
	//L DrawRectFn(hdc, &rc, icr3DDarkShadow, COLOR_BTNFACE,
	//	!fHScroll, !fVScroll);

	// draw outer rect
	hwndParent = ::GetParent(hwnd);
	hdc = GetDC(hwndParent);
	DrawRectFn2(hdc, &rcParent, COLOR_BTNSHADOW, COLOR_BTNHIGHLIGHT,
		TRUE, TRUE);
	ReleaseDC(hwndParent, hdc);
}

LRESULT CTxtWinHost::OnSize(
	HWND hwnd,
	WORD fwSizeType,
	int  nWidth,
	int  nHeight)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSize");

	RECT rc;

	TxGetClientRect(&rc);

	if (!m_fVisible)
	{
		m_fIconic = GetIconic2(hwnd);
		if (!m_fIconic)
		{
			m_fResized = TRUE;
		}
	}
	else
	{
		if (GetIconic2(hwnd))
		{
			//L TRACEINFOSZ("Iconicizing");
			m_fIconic = TRUE;
		}
		else
		{
			// We use this property because this will force a recalc.
			// We don't actually recalc on a client rect change because
			// most of the time it is pointless. We force one here because
			// some applications use size changes to calculate the optimal
			// size of the window.
			m_pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE,
				TXTBIT_EXTENTCHANGE);

			if (m_fIconic)
			{
				//L TRACEINFOSZ("Restoring from iconic");
				::InvalidateRect(hwnd, NULL, FALSE);
				m_fIconic = FALSE;
			}

			if ((TxGetEffects() == IUITXTEFFECT_SUNKEN)	// Draw borders
				&& (dwMajorVersion < VERS4))
			{
				DrawSunkenBorder(hwnd, NULL);
			}
		}
	}
	return 0;
}

HRESULT CTxtWinHost::OnTxVisibleChange(
	BOOL fVisible)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnTxVisibleChange");

	m_fVisible = fVisible;

	if (!m_fVisible && m_fResized)
	{
		RECT rc;
		// Control was resized while hidden,
		// need to really resize now
		TxGetClientRect(&rc);
		m_fResized = FALSE;
		m_pserv->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE,
			TXTBIT_CLIENTRECTCHANGE);
	}
	return S_OK;
}


//////////////////////////// ITextHost Interface  ////////////////////////////

// @doc EXTERNAL
/*
 *	CTxtWinHost::TxGetDC()
 *
 *	@mfunc
 *		Abstracts GetDC so Text Services does not need a window handle.
 *
 *	@rdesc
 *		A DC or NULL in the event of an error.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
HDC CTxtWinHost::TxGetDC()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetDC");

	return (HDC)SendMessage(m_hRichTextBox, REM_GET_DC, 0, 0);
}

/*
 *	CTxtWinHost::TxReleaseDC (hdc)
 *
 *	@mfunc
 *		Release DC gotten by TxGetDC.
 *
 *	@rdesc
 *		1 - HDC was released. <nl>
 *		0 - HDC was not released. <nl>
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
int CTxtWinHost::TxReleaseDC(HDC hdc)				//@parm	DC to release
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxReleaseDC");

	return 1;
}

/*
 *	CTxtWinHost::TxShowScrollBar (fnBar, fShow)
 *
 *	@mfunc
 *		Shows or Hides scroll bar in Text Host window
 *
 *	@rdesc
 *		TRUE on success, FALSE otherwise
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxShowScrollBar(
	INT  fnBar, 		//@parm	Specifies scroll bar(s) to be shown or hidden
	BOOL fShow)		//@parm	Specifies whether scroll bar is shown or hidden
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxShowScrollBar");

	_ASSERT(m_hRichTextBox);
	return IUI::ShowScrollBar(m_hRichTextBox, fnBar, fShow);
}

/*
 *	CTxtWinHost::TxEnableScrollBar (fuSBFlags, fuArrowflags)
 *
 *	@mfunc
 *		Enables or disables one or both scroll bar arrows
 *		in Text Host window.
 *
 *	@rdesc
 *		If the arrows are enabled or disabled as specified, the return
 *		value is TRUE. If the arrows are already in the requested state or an
 *		error occurs, the return value is FALSE.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxEnableScrollBar(
	INT fuSBFlags, 		//@parm Specifies scroll bar type
	INT fuArrowflags)	//@parm	Specifies whether and which scroll bar arrows
//		are enabled or disabled
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxEnableScrollBar");

	_ASSERT(NULL != m_hRichTextBox);
	return IUI::EnableScrollBar(m_hRichTextBox, fuSBFlags, fuArrowflags);
}

/*
 *	CTxtWinHost::TxSetScrollRange (fnBar, nMinPos, nMaxPos, fRedraw)
 *
 *	@mfunc
 *		Sets the minimum and maximum position values for the specified
 *		scroll bar in the text host window.
 *
 *	@rdesc
 *		If the arrows are enabled or disabled as specified, the return value
 *		is TRUE. If the arrows are already in the requested state or an error
 *		occurs, the return value is FALSE.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxSetScrollRange(
	INT	 fnBar, 		//@parm	Scroll bar flag
	LONG nMinPos, 		//@parm	Minimum scrolling position
	INT  nMaxPos, 		//@parm	Maximum scrolling position
	BOOL fRedraw)		//@parm	Specifies whether scroll bar should be redrawn
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetScrollRange");

	_ASSERT(NULL != m_hRichTextBox);

	if (NULL == m_pserv)
	{
		// We are initializing so do this instead of callback
		return IUI::SetScrollRange(m_hRichTextBox, fnBar, nMinPos, nMaxPos, fRedraw);
	}

	return TRUE;
}

/*
 *	CTxtWinHost::TxSetScrollPos (fnBar, nPos, fRedraw)
 *
 *	@mfunc
 *		Tells Text host to set the position of the scroll box (thumb) in the
 *		specified scroll bar and, if requested, redraws the scroll bar to
 *		reflect the new position of the scroll box.
 *
 *	@rdesc
 *		TRUE on success; FALSE otherwise.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxSetScrollPos(
	INT		fnBar, 		//@parm	Scroll bar flag
	INT		nPos, 		//@parm	New position in scroll box
	BOOL	fRedraw)	//@parm	Redraw flag
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetScrollPos");

	_ASSERT(NULL != m_hRichTextBox);

	if (NULL == m_pserv)
	{
		// We are initializing so do this instead of callback
		return IUI::SetScrollPos(m_hRichTextBox, fnBar, nPos, fRedraw);
	}

	SetScrollInfo(fnBar, fRedraw);

	return TRUE;
}

/*
 *	CTxtWinHost::TxInvalidateRect (prc, fMode)
 *
 *	@mfunc
 *		Adds a rectangle to the Text Host window's update region
 *
 *	@comm
 *		This function may be called while inactive; however the host
 *		implementation is free to invalidate an area greater than
 *		the requested rect.
 */
void CTxtWinHost::TxInvalidateRect(
	LPCRECT	prc, 		//@parm	Address of rectangle coordinates
	BOOL	fMode)		//@parm	Erase background flag
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxInvalidateRect");

	CIUIRect rc = *prc;
	InvalidateRect(m_hRichTextBox, NULL);
}

/*
 *	CTxtWinHost::TxViewChange (fUpdate)
 *
 *	@mfunc
 *		Notify Text Host that update region should be repainted.
 *
 *	@comm
 *		It is the responsibility of the text services to call
 *		TxViewChanged every time it decides that it's visual representation
 *		has changed, regardless of whether the control is active or
 *		not. If the control is active, the text services has the additional
 *		responsibility of making sure the controls window is updated.
 *		It can do this in a number of ways: 1) get a DC for the control's
 *		window and start blasting pixels (TxGetDC and TxReleaseDC), 2)
 *		invalidate the control's window (TxInvalidate), or 3) scroll
 *		the control's window (TxScrollWindowEx).
 *
 *		Text services can choose to call TxViewChange after it has
 *		performed any operations to update the active view and pass a
 *		true along with the call.  By passing true, the text host
 *		calls UpdateWindow to make sure any unpainted areas of the
 *		active control are repainted.
 */
void CTxtWinHost::TxViewChange(
	BOOL fUpdate)		//@parm TRUE = call update window
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxViewChange");

	if (fUpdate)
	{
		InvalidateRect(m_hRichTextBox, NULL);
	}
}

/*
 *	CTxtWinHost::TxCreateCaret (hbmp, xWidth, yHeight)
 *
 *	@mfunc
 *		Create new shape for Text Host's caret
 *
 *	@rdesc
 *		TRUE on success, FALSE otherwise.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxCreateCaret(
	HBITMAP hbmp, 		//@parm Handle of bitmap for caret shape
	INT xWidth, 		//@parm	Caret width
	INT yHeight)		//@parm	Caret height
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxCreateCaret");

	_ASSERT(IsWindow(m_hRichTextBox));
	return ::CreateCaret(GetHostWnd(m_hRichTextBox), hbmp, xWidth, yHeight);
}

/*
 *	CTxtWinHost::TxShowCaret (fShow)
 *
 *	@mfunc
 *		Make caret visible/invisible at caret position in Text Host window.
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxShowCaret(
	BOOL fShow)			//@parm Flag whether caret is visible
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxShowCaret");

	return fShow ? ::ShowCaret(GetHostWnd(m_hRichTextBox)) : ::HideCaret(GetHostWnd(m_hRichTextBox));
}

/*
 *	CTxtWinHost::TxSetCaretPos (x, y)
 *
 *	@mfunc
 *		Move caret position to specified coordinates in Text Host window.
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
BOOL CTxtWinHost::TxSetCaretPos(
	INT x, 				//@parm	Horizontal position (in client coordinates)
	INT y)				//@parm	Vertical position (in client coordinates)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetCaretPos");

	// 对于Windowless RichEdit控件，由于没有窗口句柄，
	// 所以这里的坐标，应该是相对于Host窗口的坐标。
	//IUITRACEA("CTxtWinHost::TxSetCaretPos\r\n");

	return ::SetCaretPos(x, y);
}

/*
 *	CTxtWinHost::TxSetTimer (idTimer, uTimeout)
 *
 *	@mfunc
 *		Request Text Host to creates a timer with specified time out.
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 */
BOOL CTxtWinHost::TxSetTimer(
	UINT idTimer, 		//@parm Timer identifier
	UINT uTimeout)		//@parm	Timeout in msec
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetTimer");

	_ASSERT(IsWindow(m_hRichTextBox));
	UINT_PTR uRet = SetTimer(m_hRichTextBox, idTimer, uTimeout, NULL);
	return uRet > 0;
}

/*
 *	CTxtWinHost::TxKillTimer (idTimer)
 *
 *	@mfunc
 *		Destroy specified timer
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 *
 *	@comm
 *		This method may be called at any time irrespective of active versus
 *		inactive state.
 */
void CTxtWinHost::TxKillTimer(
	UINT idTimer)		//@parm	id of timer
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxKillTimer");

	_ASSERT(IsWindow(m_hRichTextBox));
	::KillTimer(GetHostWnd(m_hRichTextBox), idTimer);
}

/*
 *	CTxtWinHost::TxScrollWindowEx (dx, dy, lprcScroll, lprcClip, hrgnUpdate,
 *									lprcUpdate, fuScroll)
 *	@mfunc
 *		Request Text Host to scroll the content of the specified client area
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
void CTxtWinHost::TxScrollWindowEx(
	INT		dx, 			//@parm	Amount of horizontal scrolling
	INT		dy, 			//@parm	Amount of vertical scrolling
	LPCRECT lprcScroll, 	//@parm	Scroll rectangle
	LPCRECT lprcClip,		//@parm	Clip rectangle
	HRGN	hrgnUpdate, 	//@parm	Handle of update region
	LPRECT	lprcUpdate,		//@parm	Update rectangle
	UINT	fuScroll)		//@parm	Scrolling flags
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxScrollWindowEx");
}

/*
 *	CTxtWinHost::TxSetCapture (fCapture)
 *
 *	@mfunc
 *		Set mouse capture in Text Host's window.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may do nothing.
 */
void CTxtWinHost::TxSetCapture(
	BOOL fCapture)		//@parm	Whether to get or release capture
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetCapture");

	_ASSERT(IsWindow(m_hRichTextBox));
	if (fCapture)
	{
		SetCapture(m_hRichTextBox);
	}
	else
	{
		::ReleaseCapture();
	}
}

/*
 *	CTxtWinHost::TxSetFocus ()
 *
 *	@mfunc
 *		Set focus in text host window.
 *
 *	@comm
 *		This method is only valid when the control is in-place active;
 *		calls while inactive may fail.
 */
void CTxtWinHost::TxSetFocus()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetFocus");

	// RichEdit内部维护着控件是否是focus的状态，但无法直接获取。
	// 不过在得到焦点和失去焦点时，都会向外通知。失去焦点是通过CTextHost::TxNotify
	// 发送的EN_KILLFOCUS通知。
	_ASSERT(IsWindow(m_hRichTextBox));
	SetFocus(GetHostWnd(m_hRichTextBox), m_hRichTextBox);
}

/*
 *	CTxtWinHost::TxSetCursor (hcur, fText)
 *
 *	@mfunc
 *		Establish a new cursor shape in the Text Host's window.
 *
 *	@comm
 *		This method may be called at any time, irrespective of
 *		active vs. inactive state.
 *
 *		ITextHost::TxSetCursor should be called back by the Text Services
 *		to actually set the mouse cursor. If the fText parameter is TRUE,
 *		Text Services is trying to set the "text" cursor (cursor over text
 *		that is not selected, currently an IBEAM). In that case, the host
 *		can set it to whatever the control MousePointer property is. This is
 *		required by VB compatibility since, via the MousePointer property,
 *		the VB programmer has control over the shape of the mouse cursor,
 *		whenever it would normally be set to an IBEAM.
 */
void CTxtWinHost::TxSetCursor(
	HCURSOR hcur,		//@parm	Handle to cursor
	BOOL	fText)		//@parm Indicates caller wants to set text cursor
//		(IBeam) if true.
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxSetCursor");

	::SetCursor(hcur);
}

/*
 *	CTxtWinHost::TxScreenToClient (lppt)
 *
 *	@mfunc
 *		Convert screen coordinates to Text Host window coordinates.
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 */
BOOL CTxtWinHost::TxScreenToClient(
	LPPOINT lppt)		//@parm	Coordinates for point
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxScreenToClient");

	_ASSERT(IsWindow(m_hRichTextBox));

	return ScreenToClient(m_hRichTextBox, lppt);
}

/*
 *	CTxtWinHost::TxClientToScreen (lppt)
 *
 *	@mfunc
 *		Convert Text Host coordinates to screen coordinates
 *
 *	@rdesc
 *		TRUE - call succeeded <nl>
 *		FALSE - call failed <nl>
 *
 *	@comm
 *		This call is valid at any time, although it is allowed to
 *		fail.  In general, if text services has coordinates it needs
 *		to translate from client coordinates (e.g. for TOM's
 *		PointFromRange method) the text services will actually be
 *		visible.
 *
 *		However, if no conversion is possible, then the method will fail.
 */
BOOL CTxtWinHost::TxClientToScreen(
	LPPOINT lppt)		//@parm	Client coordinates to convert.
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxClientToScreen");

	_ASSERT(IsWindow(m_hRichTextBox));
	return ClientToScreen(m_hRichTextBox, lppt);
}

/*
 *	CTxtWinHost::TxActivate (plOldState)
 *
 *	@mfunc
 *		Notify Text Host that control is active
 *
 *	@rdesc
 *		S_OK 	- call succeeded. <nl>
 *		E_FAIL	- activation is not possible at this time
 *
 *	@comm
 *		It is legal for the host to refuse an activation request;
 *		the control may be minimized and thus invisible, for instance.
 *
 *		The caller should be able to gracefully handle failure to activate.
 *
 *		Calling this method more than once does not cumulate; only
 *		once TxDeactivate call is necessary to deactive.
 *
 *		The callee will will in <p plOldState> with an arbitrary
 *		value.  The caller (Text Services) should hang onto this
 *		handle and return it in a subsequent call to TxDeactivate.
 */
HRESULT CTxtWinHost::TxActivate(
	LONG *plOldState)	//@parm Where to put previous activation state
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxActivate");

	return S_OK;
}

/*
 *	CTxtWinHost::TxDeactivate (lNewState)
 *
 *	@mfunc
 *		Notify Text Host that control is now inactive
 *
 *	@rdesc
 *		S_OK - call succeeded. <nl>
 *		E_FAIL				   <nl>
 *
 *	@comm
 *		Calling this method more than once does not cumulate
 */
HRESULT CTxtWinHost::TxDeactivate(
	LONG lNewState)	//@parm	New state (typically value returned by
//		TxActivate
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxDeactivate");

	return S_OK;
}

/*
 *	CTxtWinHost::TxGetClientRect (prc)
 *
 *	@mfunc
 *		Retrive client coordinates of Text Host's client area.
 *
 *	@rdesc
 *		HRESULT = (success) ? S_OK : E_FAIL
 */

// TODO: 对于带句柄的RichEdit，得到相对RichEdit句柄的坐标
// 对于Windowless RichEdit，得到RichEdit相对它宿主窗口的坐标
// RichEdit内部在计算鼠标光标坐标的时候，也会用到它。
HRESULT CTxtWinHost::TxGetClientRect(
	LPRECT prc)		//@parm	Where to put client coordinates
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetClientRect");

	_ASSERT(IsWindow(m_hRichTextBox) && prc);

	GetClientRect(m_hRichTextBox, prc);
	ClientToScreen(m_hRichTextBox, prc);
	IUIScreenToClient(GetHostWnd(m_hRichTextBox), prc);

	return S_OK;
}

/*
 *	CTxtWinHost::TxGetViewInset	(prc)
 *
 *	@mfunc
 *		Get inset for Text Host window.  Inset is the "white space"
 *		around text.
 *
 *	@rdesc
 *		HRESULT = NOERROR
 *
 *	@comm
 *		The Inset rectangle is not strictly a rectangle.  The top, bottom,
 *		left, and right fields of the rect structure indicate how far in
 *		each direction drawing should be inset. Inset sizes are in client
 *		coordinates.
 */
HRESULT CTxtWinHost::TxGetViewInset(
	LPRECT prc)			//@parm Where to put inset rectangle
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetViewInset");

	_ASSERT(prc);

	*prc = m_rcViewInset;

	return NOERROR;
}

/*
 *	CTxtWinHost::TxGetCharFormat (ppCF)
 *
 *	@mfunc
 *		Get Text Host's default character format
 *
 *	@rdesc
 *		HRESULT = E_NOTIMPL (not needed in simple Windows host, since text
 *		services provides desired default)
 *
 *	@comm
 *		The callee retains ownwership of the charformat returned.  However,
 *		the pointer returned must remain valid until the callee notifies
 *		Text Services via OnTxPropertyBitsChange that the default character
 *		format has changed.
 */
HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetCharFormat");

	// 不需要提供默认char fomat，当需要改变RichTextBox的char format时，可随时调用SetDefaultCharFormat。
	return E_NOTIMPL;
}

/*
 *	CTxtWinHost::TxGetParaFormat (ppPF)
 *
 *	@mfunc
 *		Get Text Host default paragraph format
 *
 *	@rdesc
 *		HRESULT = E_NOTIMPL (not needed in simple Windows host, since text
 *		services provides desired default)
 *
 *	@comm
 *		The callee retains ownwership of the charformat returned.  However,
 *		the pointer returned must remain valid until the callee notifies
 *		Text Services via OnTxPropertyBitsChange that the default paragraph
 *		format has changed.
 */
HRESULT CTxtWinHost::TxGetParaFormat(
	const PARAFORMAT **ppPF) 	//@parm Where to put ptr to default
//		paragraph format
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetParaFormat");

	return E_NOTIMPL;
}

/*
 *	CTxtWinHost::TxGetSysColor (nIndex)
 *
 *	@mfunc
 *		Get specified color identifer from Text Host.
 *
 *	@rdesc
 *		Color identifier
 *
 *	@comm
 *		Note that the color returned may be *different* than the
 *		color that would be returned from a call to GetSysColor.
 *		This allows hosts to override default system behavior.
 *
 *		Needless to say, hosts should be very careful about overriding
 *		normal system behavior as it could result in inconsistent UI
 *		(particularly with respect to Accessibility	options).
 */
COLORREF CTxtWinHost::TxGetSysColor(
	int nIndex)			//@parm Color to get, same parameter as
//		GetSysColor Win32 API
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetSysColor");

	if (!m_fDisabled
		|| ((nIndex != COLOR_WINDOW) && (nIndex != COLOR_WINDOWTEXT)))
	{
		// This window is not disabled or the color is not interesting
		// in the disabled case.
		return (nIndex == COLOR_WINDOW && m_fNotSysBkgnd)
			? m_crBackground : GetSysColor(nIndex);
	}

	// Disabled case
	if (COLOR_WINDOWTEXT == nIndex)
	{
		// Color of text for disabled window
		return GetSysColor(COLOR_GRAYTEXT);
	}

	// Background color for disabled window
	return GetSysColor(COLOR_3DFACE);
}

/*
 *	CTxtWinHost::TxGetBackStyle	(pstyle)
 *
 *	@mfunc
 *		Get Text Host background style.
 *
 *	@rdesc
 *		HRESULT = S_OK
 *
 *	@xref	<e TXTBACKSTYLE>
 */
HRESULT CTxtWinHost::TxGetBackStyle(
	TXTBACKSTYLE *pstyle)  //@parm Where to put background style
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetBackStyle");

	*pstyle = (m_dwExStyle & WS_EX_TRANSPARENT)
		? TXTBACK_TRANSPARENT : TXTBACK_OPAQUE;
	return NOERROR;
}

/*
 *	CTxtWinHost::TxGetMaxLength	(pLength)
 *
 *	@mfunc
 *		Get Text Host's maximum allowed length.
 *
 *	@rdesc
 *		HRESULT = S_OK
 *
 *	@comm
 *		This method parallels the EM_LIMITTEXT message.
 *		If INFINITE (0xFFFFFFFF) is returned, then text services
 *		will use as much memory as needed to store any given text.
 *
 *		If the limit returned is less than the number of characters
 *		currently in the text engine, no data is lost.  Instead,
 *		no edits will be allowed to the text *other* than deletion
 *		until the text is reduced to below the limit.
 */
HRESULT CTxtWinHost::TxGetMaxLength(
	DWORD *pLength) 	//@parm Maximum allowed length, in number of
//		characters
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetMaxLength");

	//IUITRACEA("CTxtWinHost::TxGetMaxLength why is this being called?\r\n");
	return NOERROR;
}

/*
 *	CTxtWinHost::TxGetScrollBars (pdwScrollBar)
 *
 *	@mfunc
 *		Get Text Host's scroll bars supported.
 *
 *	@rdesc
 *		HRESULT = S_OK
 *
 *	@comm
 *		<p pdwScrollBar> is filled with a boolean combination of the
 *		window styles related to scroll bars.  Specifically, these are:
 *
 *			WS_VSCROLL	<nl>
 *			WS_HSCROLL	<nl>
 *			ES_AUTOVSCROLL	<nl>
 *			ES_AUTOHSCROLL	<nl>
 *			ES_DISABLENOSCROLL	<nl>
 */
HRESULT CTxtWinHost::TxGetScrollBars(
	DWORD *pdwScrollBar) 	//@parm Where to put scrollbar information
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetScrollBars");

	*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
			ES_AUTOHSCROLL | ES_DISABLENOSCROLL);
	return NOERROR;
}

/*
 *	CTxtWinHost::TxGetPasswordChar (pch)
 *
 *	@mfunc
 *		Get Text Host's password character.
 *
 *	@rdesc
 *		HRESULT = (password character not enabled) ? S_FALSE : S_OK
 *
 *	@comm
 *		The password char will only be shown if the TXTBIT_USEPASSWORD bit
 *		is enabled in TextServices.  If the password character changes,
 *		re-enable the TXTBIT_USEPASSWORD bit via
 *		ITextServices::OnTxPropertyBitsChange.
 */
HRESULT CTxtWinHost::TxGetPasswordChar(
	TCHAR *pch)		//@parm Where to put password character
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetPasswordChar");

	*pch = m_chPassword;
	return NOERROR;
}

/*
 *	CTxtWinHost::TxGetAcceleratorPos (pcp)
 *
 *	@mfunc
 *		Get special character to use for underlining accelerator character.
 *
 *	@rdesc
 *		Via <p pcp>, returns character position at which underlining
 *		should occur.  -1 indicates that no character should be underlined.
 *		Return value is an HRESULT (usually S_OK).
 *
 *	@comm
 *		Accelerators allow keyboard shortcuts to various UI elements (like
 *		buttons.  Typically, the shortcut character is underlined.
 *
 *		This function tells Text Services which character is the accelerator
 *		and thus should be underlined.  Note that Text Services will *not*
 *		process accelerators; that is the responsiblity of the host.
 *
 *		This method will typically only be called if the TXTBIT_SHOWACCELERATOR
 *		bit is set in text services.
 *
 *		Note that *any* change to the text in text services will result in the
 *		invalidation of the accelerator underlining.  In this case, it is the
 *		host's responsibility to recompute the appropriate character position
 *		and inform text services that a new accelerator is available.
 */
HRESULT CTxtWinHost::TxGetAcceleratorPos(
	LONG *pcp) 		//@parm Out parm to receive cp of character to underline
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetAcceleratorPos");

	*pcp = -1;
	return S_OK;
}

/*
 *	CTxtWinHost::OnTxCharFormatChange
 *
 *	@mfunc
 *		Set default character format for the Text Host.
 *
 *	@rdesc
 *		S_OK - call succeeded.	<nl>
 *		E_INVALIDARG			<nl>
 *		E_FAIL					<nl>
 */
HRESULT CTxtWinHost::OnTxCharFormatChange(
	const CHARFORMATW *pcf) //@parm New default character format
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnTxCharFormatChange");

	return S_OK;
}

/*
 *	CTxtWinHost::OnTxParaFormatChange
 *
 *	@mfunc
 *		Set default paragraph format for the Text Host.
 *
 *	@rdesc
 *		S_OK - call succeeded.	<nl>
 *		E_INVALIDARG			<nl>
 *		E_FAIL					<nl>
 */
HRESULT CTxtWinHost::OnTxParaFormatChange(
	const PARAFORMAT *ppf) //@parm New default paragraph format
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnTxParaFormatChange");

	return S_OK;
}

/*
 *	CTxtWinHost::TxGetPropertyBits (dwMask, dwBits)
 *
 *	@mfunc
 *		Get the bit property settings for Text Host.
 *
 *	@rdesc
 *		S_OK
 *
 *	@comm
 *		This call is valid at any time, for any combination of
 *		requested property bits.  <p dwMask> may be used by the
 *		caller to request specific properties.
 */
HRESULT CTxtWinHost::TxGetPropertyBits(
	DWORD dwMask,		//@parm	Mask of bit properties to get
	DWORD *pdwBits)	//@parm Where to put bit values
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetPropertyBits");

	// FUTURE: Obvious optimization is to save bits in host the same way that
	// they are returned and just return them instead of this mess.

	// Note: this RichEdit host never sets TXTBIT_SHOWACCELERATOR or
	// TXTBIT_SAVESELECTION. They are currently only used by Forms^3 host.
	// This host is always rich text.

	DWORD dwProperties = TXTBIT_RICHTEXT | TXTBIT_ALLOWBEEP;

#ifdef DEBUG
	// make sure that TS doesn't think it's plain text mode when
	// we return TXTBIT_RICHTEXT
	if ((dwMask & TXTBIT_RICHTEXT) && m_pserv)
	{
		DWORD mode;
		mode = m_pserv->TxSendMessage(EM_GETTEXTMODE, 0, 0, NULL);
		_ASSERT(mode == TM_RICHTEXT);
	}
#endif // DEBUG

	if (m_dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (m_dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}

	if (m_dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(m_dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (m_fEnableAutoWordSel)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

	if (!(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)))
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

	if (m_dwStyle & ES_NOOLEDRAGDROP)
	{
		dwProperties |= TXTBIT_DISABLEDRAG;
	}

	*pdwBits = dwProperties & dwMask;
	return NOERROR;
}

/*
 * RichEdit的实现与HWND是无关的，在内部，当输入的内容发生改变后，调用
 *  ITextHost::TxNotify(EN_CHANGE, ...);
 *  告诉ITextHost文本内部发生了变化。 所以，我们需要提供ITextHost的派生类给RichEdit,
 *  来承接这些事件。 对于HWND的RichEdit的ITextHost派生类，在收到这些事件后，转义成
 *  普通消息，要么发给RichEdit自己的HWND，要么发给父窗口。 同理，对于windowless的RichEdit,
 *  也需要根据情况，把这些事件路由出去。 CTxtWinHost::TxNotify是RichEdit内部通知外部事件的接口
 *	CTxtWinHost::TxNotify (iNotify,	pv)
 *
 *	@mfunc
 *		Notify Text Host of various events.  Note that there are
 *		two basic categories of events, "direct" events and
 *		"delayed" events.  Direct events are sent immediately as
 *		they need some processing: EN_PROTECTED is a canonical
 *		example.  Delayed events are sent after all processing
 *		has occurred; the control is thus in a "stable" state.
 *		EN_CHANGE, EN_ERRSPACE, EN_SELCHANGED are examples
 *		of delayed notifications.
 *
 *
 *	@rdesc
 *		S_OK - call succeeded <nl>
 *		S_FALSE	-- success, but do some different action
 *		depending on the event type (see below).
 *
 *	@comm
 *		The notification events are the same as the notification
 *		messages sent to the parent window of a richedit window.
 *		The firing of events may be controlled with a mask set via
 *		the EM_SETEVENTMASK message.
 *
 *		In general, is legal to make any calls to text services while
 *		processing this method; however, implementors are cautioned
 *		to avoid excessive recursion.
 *
 *		Here is the complete list of notifications that may be
 *		sent and a brief description of each:
 *
 *		<p EN_CHANGE>		Sent when some data in the edit control
 *		changes (such as text or formatting).  Controlled by the
 *		ENM_CHANGE event mask.  This notification is sent _after_
 *		any screen updates have been requested.
 *
 *		<p EN_CORRECTTEXT>	PenWin only; currently unused.
 *
 *		<p EN_DROPFILES>	If the client registered the edit
 *		control via DragAcceptFiles, this event will be sent when
 *		a WM_DROPFILES or DragEnter( CF_HDROP ) message is received.
 *		If S_FALSE is returned, the drop will be ignored, otherwise,
 *		the drop will be processed.  The ENM_DROPFILES mask
 *		controls this event notification.
 *
 *		<p EN_ERRSPACE>		Sent when the edit control cannot
 *		allocate enough memory.  No additional data is sent and
 *		there is no mask for this event.
 *
 *		<p EN_HSCROLL>		Sent when the user clicks on an edit
 *		control's horizontal scroll bar, but before the screen
 *		is updated.  No additional data is sent.  The ENM_SCROLL
 *		mask controls this event.
 *
 *		<p EN_IMECHANGE>	unused
 *
 *		<p EN_KILLFOCUS>	Sent when the edit control looses focus.
 *		No additional data is sent and there is no mask.
 *
 *		<p EN_MAXTEXT>	Sent when the current text insertion
 *		has exceeded the specified number of characters for the
 *		edit control.  The text insertion has been truncated.
 *		There is no mask for this event.
 *
 *		<p EN_MSGFILTER>	NB!!! THIS MESSAGE IS NOT SENT TO
 *		TxNotify, but is included here for completeness.  With
 *		ITextServices::TxSendMessage, client have complete
 *		flexibility in filtering all window messages.
 *
 *		Sent on a keyboard or mouse event
 *		in the control.  A MSGFILTER data structure is sent,
 *		containing the msg, wparam and lparam.  If S_FALSE is
 *		returned from this notification, the msg is processed by
 *		TextServices, otherwise, the message is ignored.  Note
 *		that in this case, the callee has the opportunity to modify
 *		the msg, wparam, and lparam before TextServices continues
 *		processing.  The ENM_KEYEVENTS and ENM_MOUSEEVENTS masks
 *		control this event for the respective event types.
 *
 *		<p EN_OLEOPFAILED> 	Sent when an OLE call fails.  The
 *		ENOLEOPFAILED struct is passed with the index of the object
 *		and the error code.  Mask value is nothing.
 *
 *		<p EN_PROTECTED>	Sent when the user is taking an
 *		action that would change a protected range of text.  An
 *		ENPROTECTED data structure is sent, indicating the range
 *		of text affected and the window message (if any) affecting
 *		the change.  If S_FALSE is returned, the edit will fail.
 *		The ENM_PROTECTED mask controls this event.
 *
 *		<p EN_REQUESTRESIZE>	Sent when a control's contents are
 *		either smaller or larger than the control's window size.
 *		The client is responsible for resizing the control.  A
 *		REQRESIZE structure is sent, indicating the new size of
 *		the control.  NB!  Only the size is indicated in this
 *		structure; it is the host's responsibility to do any
 *		translation necessary to generate a new client rectangle.
 *		The ENM_REQUESTRESIZE mask controls this event.
 *
 *		<p EN_SAVECLIPBOARD> Sent when an edit control is being
 *		destroyed, the callee should indicate whether or not
 *		OleFlushClipboard should be called.  Data indicating the
 *		number of characters and objects to be flushed is sent
 *		in the ENSAVECLIPBOARD data structure.
 *		Mask value is nothing.
 *
 *		<p EN_SELCHANGE>	Sent when the current selection has
 *		changed.  A SELCHANGE data structure is also sent, which
 *		indicates the new selection range at the type of data
 *		the selection is currently over.  Controlled via the
 *		ENM_SELCHANGE mask.
 *
 *		<p EN_SETFOCUS>	Sent when the edit control receives the
 *		keyboard focus.  No extra data is sent; there is no mask.
 *
 *		<p EN_STOPNOUNDO>	Sent when an action occurs for which
 *		the control cannot allocate enough memory to maintain the
 *		undo state.  If S_FALSE is returned, the action will be
 *		stopped; otherwise, the action will continue.
 *
 *		<p EN_UPDATE>	Sent before an edit control requests a
 *		redraw of altered data or text.  No additional data is
 *		sent.  This event is controlled via the ENM_UPDATE mask.
 *
 *		<p EN_VSCROLL>	Sent when the user clicks an edit control's
 *		vertical scrollbar bar before the screen is updated.
 *		Controlled via the ENM_SCROLL mask; no extra data is sent.
 *
 *		<p EN_LINK>		Sent when a mouse event (or WM_SETCURSOR) happens
 *		over a range of text that has the EN_LINK mask bit set.
 *		An ENLINK data structure will be sent with relevant info.
 */
HRESULT CTxtWinHost::TxNotify(
	//@parm	Event to notify host of.  One of the
	//		EN_XXX values from Win32, e.g., EN_CHANGE
	DWORD iNotify,
	//@parm In-only parameter with extra data.  Type
	//		dependent on <p iNotify>
	void *pv)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxNotify");

	HRESULT		hr = NOERROR;
	LONG		nId;
	NMHDR 		*phdr;
	REQRESIZE 	*preq;
	RECT		rcOld;

	// We assume here that TextServices has already masked out notifications,
	// so if we get one here, it should be sent
	// 当RichTextBox作为普通HWND窗口的孩子时，兼容Win32模式。
	if (_hwndParent)
	{
		nId = 0;// TODO:GetWindowLong(_hwnd, GWL_ID);
		// First, handle WM_NOTIFY style notifications
		switch (iNotify)
		{
		case EN_REQUESTRESIZE:
			// Need to map new size into correct rectangle
			_ASSERT(pv);
			GetWindowRect(m_hRichTextBox, &rcOld);
			MapWindowPoints(HWND_DESKTOP, _hwndParent, (POINT *) &rcOld, 2);

			preq = (REQRESIZE *)pv;
			preq->rc.top	= rcOld.top;
			preq->rc.left	= rcOld.left;
			preq->rc.right	+= rcOld.left;
			preq->rc.bottom += rcOld.top;

		// FALL-THROUGH!!

		case EN_DROPFILES:
		case EN_MSGFILTER:
		case EN_OLEOPFAILED:
		case EN_PROTECTED:
		case EN_SAVECLIPBOARD:
		case EN_SELCHANGE:
		case EN_STOPNOUNDO:
		case EN_LINK:
		case EN_OBJECTPOSITIONS:
		case EN_DRAGDROPDONE:
		{
			if (pv)						// Fill out NMHDR portion of pv
			{
				phdr = (NMHDR *)pv;
				phdr->hwndFrom = (HWND)m_hRichTextBox;
				phdr->idFrom = nId;
				phdr->code = iNotify;
			}

			RoutedEventArgs arg;
			arg.hSender = m_hRichTextBox;
			arg.hOriginalSender = m_hRichTextBox;
			arg.RoutedEvent = WM_NOTIFY;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = nId;
			arg.lParam = (LPARAM) pv;
			LRESULT lr = RaiseEvent(m_hRichTextBox, &arg);
			if (lr != 0 || arg.lResult != 0)
			{
				hr = S_FALSE;
			}
		}
		break;

		default:
		{
			RoutedEventArgs arg;
			arg.hSender = m_hRichTextBox;
			arg.hOriginalSender = m_hRichTextBox;
			arg.RoutedEvent = WM_COMMAND;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = GET_WM_COMMAND_MPS(nId, m_hRichTextBox, iNotify);
			arg.lParam = (LPARAM)m_hRichTextBox;
			LRESULT lr = RaiseEvent(m_hRichTextBox, &arg);
		}
		}
	}

	// IUI模式
	switch (iNotify)
	{
	case EN_REQUESTRESIZE:
		// Need to map new size into correct rectangle
		_ASSERT(pv);
		GetWindowRect(m_hRichTextBox, &rcOld);
		MapWindowPoints(HWND_DESKTOP, _hwndParent, (POINT *) &rcOld, 2);

		preq = (REQRESIZE *)pv;
		preq->rc.top	= rcOld.top;
		preq->rc.left	= rcOld.left;
		preq->rc.right	+= rcOld.left;
		preq->rc.bottom += rcOld.top;

	// FALL-THROUGH!!

	case EN_DROPFILES:
	case EN_MSGFILTER:
	case EN_OLEOPFAILED:
	case EN_PROTECTED:
	case EN_SAVECLIPBOARD:
	case EN_SELCHANGE:
	case EN_STOPNOUNDO:
	case EN_LINK:
	case EN_OBJECTPOSITIONS:
	case EN_DRAGDROPDONE:

		if (pv)						// Fill out NMHDR portion of pv
		{
			phdr = (NMHDR *)pv;
			phdr->hwndFrom = (HWND)m_hRichTextBox;
			phdr->idFrom = nId;
			phdr->code = iNotify;
		}

		{
			RoutedEventArgs arg;
			arg.hSender = m_hRichTextBox;
			arg.hOriginalSender = m_hRichTextBox;
			arg.RoutedEvent = WM_NOTIFY;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = (WPARAM)nId;
			arg.lParam = (LPARAM)pv;
			LRESULT lr = RaiseEvent(m_hRichTextBox, &arg);
			if (lr != 0)
			{
				hr = S_FALSE;
			}
		}
		break;

	default:
		RoutedEventArgs arg;
		arg.hSender = m_hRichTextBox;
		arg.hOriginalSender = m_hRichTextBox;
		arg.RoutedEvent = WM_COMMAND;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = GET_WM_COMMAND_MPS(nId, m_hRichTextBox, iNotify);
		arg.lParam = (LPARAM)m_hRichTextBox;
		LRESULT lr = RaiseEvent(m_hRichTextBox, &arg);
	}

	return hr;
}

/*
 *	CTxtWinHost::TxGetUndoManager (ppUndoMgr)
 *
 *	@mfunc
 *		Notify Host of undo manager
 *
 *	@rdesc
 *		E_NOTIMPL	<nl>
 *		S_OK		<nl>
 *
 *	@comm
 *		If an IUndoActionManager object is provided by the host, text
 *		services will add undoable actions to the returned object as
 *		appropriate.  The undo manager object should be the undo manager
 *		for the *default* user.
 *
 *		However, if no IUndoActionManager object is returned, text
 *		services will still maintain its own undo state.  To turn off
 *		undo, set the undo limit to zero via EM_SETUNDOLIMIT.
 *
 *		It is valid to call this method at any time.  Mutliple calls
 *		should return a pointer to the same object; failure to do so
 *		may result in lost undo information or memory leaks.
 */
//L HRESULT	CTxtWinHost::TxGetUndoManager(
//	IUndoActionManager **ppUndoMgr) //@parm Forms3 undo manager
//{
//	IUITRACEA("CTxtWinHost::TxGetUndoManager\r\n");
//
//	return E_NOTIMPL;
//}

/*
 *	CTxtWinHost::TxGetExtent (lpExtent)
 *
 *	@mfunc
 *		Return native size of the control in HIMETRIC
 *
 *	@rdesc
 *		S_OK	<nl>
 *		some failure code <nl>
 *
 *	@comm
 *		This method is used by Text Services to implement zooming.
 *		Text Services would derive the zoom factor from the ratio between
 *		the himetric and device pixel extent of the client rectangle.
 *
 *		[vertical zoom factor] = [pixel height of the client rect] * 2540
 *		/ [himetric vertical extent] * [pixel per vertical inch (from DC)]
 *
 *		If the vertical and horizontal zoom factors are not the same, Text
 *		Services could ignore the horizontal zoom factor and assume it is
 *		the same as the vertical one.
 */
HRESULT CTxtWinHost::TxGetExtent(
	LPSIZEL lpExtent) 	//@parm  Extent in himetric
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetExtent");

	AssertSz(lpExtent, "CTxtWinHost::TxGetExtent Invalid lpExtent");

	// We could implement the TxGetExtent in the following way. However, the
	// call to this in ITextServices is implemented in such a way that it
	// does something sensible in the face of an error in this call. That
	// something sensible is that it sets the extent equal to the current
	// client rectangle which is what the following does in a rather convoluted
	// manner. Therefore, we dump the following and just return an error.


#if 0
	// The window's host extent is always the same as the client
	// rectangle.
	RECT rc;
	HRESULT hr = TxGetClientRect(&rc);

	// Get our client rectangle
	if (SUCCEEDED(hr))
	{
		// Calculate the length & convert to himetric
		lpExtent->cx = DXtoHimetricX(rc.right - rc.left, xPerInchScreenDC);
		lpExtent->cy = DYtoHimetricY(rc.bottom - rc.top, yPerInchScreenDC);
	}

	return hr;
#endif // 0

	return E_NOTIMPL;
}

/*
 *	CTxtWinHost::TxGetSelectionBarWidth (lSelBarWidth)
 *
 *	@mfunc
 *		Returns size of selection bar in HIMETRIC
 *
 *	@rdesc
 *		S_OK	<nl>
 */
HRESULT	CTxtWinHost::TxGetSelectionBarWidth(
	LONG *lSelBarWidth)		//@parm  Where to return selection bar width
// in HIMETRIC
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetSelectionBarWidth");

	*lSelBarWidth = (m_dwStyle & ES_SELECTIONBAR) ? dxSelBar : 0;
	return S_OK;
}

//
//	ITextHost2 methods
//

/*
 *	CTxtWinHost::TxIsDoubleClickPending
 *
 *	@mfunc	Look into the message queue for this hwnd and see if a
 *			double click is pending.  This enables TextServices to
 *			effeciently transition between two inplace active objects.
 *
 *	@rdesc	BOOL
 */
BOOL CTxtWinHost::TxIsDoubleClickPending()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxIsDoubleClickPending");

	MSG msg;

	// 	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN,
	// 		"CTxtWinHost::TxIsDoubleClickPending");

	if (PeekMessageA(&msg, GetHostWnd(m_hRichTextBox), WM_LBUTTONDBLCLK, WM_LBUTTONDBLCLK,
			PM_NOREMOVE | PM_NOYIELD))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 *	CTxtWinHost::TxGetWindow
 *
 *	@mfunc	Fetches the window associated with this control (or
 *			set of controls potentially).  Useful for answering
 *			IOleWindow::GetWindow.
 *
 *	@rdesc	HRESULT
 */
HRESULT CTxtWinHost::TxGetWindow(HWND *phwnd)
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::TxGetWindow");

	*phwnd = GetHostWnd(m_hRichTextBox);
	return NOERROR;
}


/*
 *	CTxtWinHost::SetForegroundWindow
 *
 *	@mfunc	Sets window to foreground window & gives the focus
 *
 *	@rdesc	NOERROR - succeeded
 *			E_FAIL - failed.
 */
HRESULT CTxtWinHost::TxSetForegroundWindow()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN,
		"CTxtWinHost::SetForegroundWindow");

	if (!SetForegroundWindow(GetHostWnd(m_hRichTextBox)))
	{
		IUI::SetFocus(GetHostWnd(m_hRichTextBox), m_hRichTextBox);
	}

	return NOERROR;
}


/*
 *	CTxtWinHost::TxGetPalette
 *
 *	@mfunc	Returns application specific palette if there is one
 *
 *	@rdesc	~NULL - there was one
 *			NULL - use default palette
 */
HPALETTE CTxtWinHost::TxGetPalette()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN,
		"CTxtWinHost::TxGetPalette");

	return m_hpal;
}


/*
 *	CTxtWinHost::ResizeInset
 *
 *	@mfunc	Recalculates the view inset for a change in the
 *			default character set.
 *
 *	@rdesc	None.
 */
void CTxtWinHost::ResizeInset()
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::ResizeInset");

	// Create a DC
	HDC hdc = GetDC(GetHostWnd(m_hRichTextBox));
	LONG dxRemoveFromSunkenBorder = 0;
	LONG dyRemoveFromSunkenBorder = 0;

	// Get the inset information
	LONG xAveCharWidth;
	LONG yCharHeight = GetECDefaultHeightAndWidth2(m_pserv, hdc, 1, 1,
			yPerInchScreenDC, &xAveCharWidth, NULL, NULL);

	ReleaseDC(GetHostWnd(m_hRichTextBox), hdc);

	if (dwMajorVersion >= VERS4)
	{
		// This takes into account the size of the sunken borders in Win95. This
		// was lifted directly from richedit 1.0 sources.
		dxRemoveFromSunkenBorder = cxBorder * 2;
		dyRemoveFromSunkenBorder = cyBorder * 2;
	}

	// Update the inset
	m_xInset = min(xAveCharWidth, _xWidthSys) / 2 - dxRemoveFromSunkenBorder;

	m_yInset = min(yCharHeight, _yHeightSys) / 4 - dyRemoveFromSunkenBorder;

	SetDefaultInset();

	m_pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE,
		TXTBIT_VIEWINSETCHANGE);
}



/*
 *	CTxtWinHost::OnSetMargins
 *
 *	@mfunc	Handle EM_SETMARGINS message
 *
 *	@rdesc	None.
 */
void CTxtWinHost::OnSetMargins(
	DWORD fwMargin,		//@parm Type of requested operation
	DWORD xLeft,		//@parm Where to put left margin
	DWORD xRight)		//@parm Where to put right margin
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::OnSetMargins");

	LONG xLeftMargin = -1;
	LONG xRightMargin = -1;
	HDC hdc;

	if (EC_USEFONTINFO == fwMargin)
	{
		// Get the DC since it is needed for the call
		hdc = GetDC(GetHostWnd(m_hRichTextBox));

		// Multiline behaves differently than single line
		if (m_dwStyle & ES_MULTILINE)
		{
			// Multiline - over/underhange controls margin
			GetECDefaultHeightAndWidth2(m_pserv, hdc, 1, 1,
				yPerInchScreenDC, NULL, &xLeftMargin, &xRightMargin);
		}
		else
		{
			// Single line edit controls set the margins to
			// the average character width on both left and
			// right.
			GetECDefaultHeightAndWidth2(m_pserv, hdc, 1, 1,
				yPerInchScreenDC, &xLeftMargin, NULL, NULL);

			xRightMargin = xRightMargin;
		}

		ReleaseDC(GetHostWnd(m_hRichTextBox), hdc);
	}
	else
	{
		// The request is for setting exact pixels.
		if (EC_LEFTMARGIN & fwMargin)
		{
			xLeftMargin = xLeft;
		}

		if (EC_RIGHTMARGIN & fwMargin)
		{
			xRightMargin = xRight;
		}
	}

	// Set left margin if so requested
	if (xLeftMargin != -1)
	{
		m_rcViewInset.left =	DXtoHimetricX2(xLeftMargin, xPerInchScreenDC);
	}

	// Set right margin if so requested
	if (xRightMargin != -1)
	{
		m_rcViewInset.right = DXtoHimetricX2(xRightMargin, xPerInchScreenDC);
	}
}



/*
 *	CTxtWinHost::SetScrollInfo
 *
 *	@mfunc	Set scrolling information for the scroll bar.
 *
 *	@rdesc	None.
 */
void CTxtWinHost::SetScrollInfo(
	INT fnBar,			//@parm	Specifies scroll bar to be updated
	BOOL fRedraw)		//@parm whether redraw is necessary
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::SetScrollInfo");

	// Set up the basic structure for the call
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = (SIF_RANGE | SIF_PAGE | SIF_POS);// SetScrollInfo不支持SIF_TRACKPOS

	AssertSz(m_pserv != NULL,
		"CTxtWinHost::SetScrollInfo called with NULL m_pserv");

	// Call back to the control to get the parameters
	if (fnBar == SB_HORZ)
	{
		m_pserv->TxGetHScroll((LONG *) &si.nMin, (LONG *) &si.nMax,
			(LONG *) &si.nPos, (LONG *) &si.nPage, NULL);
	}
	else
	{
		m_pserv->TxGetVScroll((LONG *) &si.nMin,
			(LONG *) &si.nMax, (LONG *) &si.nPos, (LONG *) &si.nPage, NULL);
	}

	// Do the call
	IUI::SetScrollInfo(m_hRichTextBox, fnBar, &si, fRedraw);
}

/*
 *	CTxtWinHost::SetScrollBarsForWmEnable
 *
 *	@mfunc	Enable/Disable scroll bars
 *
 *	@rdesc	None.
 */
void CTxtWinHost::SetScrollBarsForWmEnable(
	BOOL fEnable)		//@parm Whether scrollbars s/b enabled or disabled.
{
	TRACEBEGIN(TRCSUBSYSHOST, TRCSCOPEINTERN, "CTxtWinHost::SetScrollBarsForWmEnable");

	if (NULL == m_pserv)
	{
		// If we don't have an edit object there can't be any scroll bars.
		return;
	}

	BOOL fHoriz = FALSE;
	BOOL fVert = FALSE;
	UINT wArrows = fEnable ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH;

	m_pserv->TxGetHScroll(NULL, NULL, NULL, NULL, &fHoriz);

	m_pserv->TxGetVScroll(NULL, NULL, NULL, NULL, &fVert);

	if (fHoriz)
	{
		// There is a vertical scroll bar
		EnableScrollBar(m_hRichTextBox, SB_HORZ, wArrows);
	}

	if (fVert)
	{
		// There is a horizontal scroll bar
		EnableScrollBar(m_hRichTextBox, SB_VERT, wArrows);
	}
}

// 当RichTextBox是UpdateLayeredWindow窗口的子控件时，由于RichEditBox使用GDI绘制
// 这将导致文字透明。 所以，我们需要TxDraw后，修复文字的Alpha值。
// 算法基于一个事实：
// GDI函数（除AlphaBlend）会把绘制覆盖的目录像素的alpha通道全改成0.
// 算法如下：
// 0. 提前备份目标区域的alpha值；
// 1. 创建一个32位BIB位图选入hTempDC；
// 2. 遍历位图，将位图所有字节全改成0xFF（白色不透明）；
// 3. 调用TxDraw往hTempDC上绘制；
// 4. 再次遍历位图，把alpha为0（表示该像素被绘制过）的像素，改成需要的alpha值
//		；把alpha不为0的像素的alpha值改成0；
// 5. 通过AlphaBlend将生成好的hTempDC，绘制到目标DC上。
// 如果希望绘制的文字完全不透明，则可忽略第0步，并且第4步中，把alpha为0的像素，改为0xFF。
// 上述算法，TxDraw是绘制到一个白色背景上。 如果文字本身有alpha值，到修复后的文字
// 混合到五彩背景上后，文字可能出现白边（仅推测，未测试），
// 所以，第1步中，我们可以直接截取目标DC上相应区域作为hTempDC背景位图。
LRESULT IUI::RichTextBox_DrawText(REBox *pREBox, HDC hDC, const RECT &rcClip)
{
	ITextServices *pTextServices = pREBox->m_pTextHost->GetTextServices();

	LRESULT lr = S_OK;
	if (pTextServices != NULL)
	{
		CIUIRect rc;
		GetClientRect(pREBox->hwnd, rc);

#ifdef _DEBUG
		CIUIPoint pt;
		GetViewportOrgEx(hDC, &pt);
#endif // _DEBUG
		// 注：下面的rc很重要。rc一定要与RichEdit原生显示面积完全一样，
		// 即RichEdit的客户区（特别注意，一定不要包含非客户区）。也就是说，
		// 自己调用TxDraw显示的内容，要与WM_PAINT完全对上，否则会引起严重
		// 的闪烁问题。 在这个问题上，花了很久解决。 这是因为，TxDraw绘制的时候，
		// 会计算垂直滚动信息，WM_PAINT绘制时，也会计算垂直滚动信息，
		// 但如果TxDraw使用的rc的宽度与WM_PAINT使用的rc的宽度不一样，则计算出
		// 来的垂直滚动范围就不一样。 这样，在RichEdit尺寸变化时，
		// 两者来回设置滚动条，导致来回刷新，就引起闪烁了。
		// TODO: 在RichTextBox获得焦点时，Window类在处理WM_SETWLFOCUS时触发
		// 的那次绘制，TxDraw会调用失败，导致没有内容绘制到界面上，这会引起一次闪烁。
		lr = pTextServices->TxDraw(
				DVASPECT_CONTENT,          // Draw Aspect
				/*-1*/0,                // Lindex
				NULL,                    // Info for drawing optimazation
				NULL,                    // target device information
				hDC,                    // Draw device HDC
				NULL,                        // Target device HDC
				(RECTL *)&rc,            // Bounding client rectangle
				NULL,                     // Clipping rectangle for metafiles
				(RECT *)&rc,        // Update rectangle
				NULL,                        // Call back function
				NULL,                    // Call back parameter
				0);                        // What view of the object

		if (FAILED(lr))
		{
			int n = 0;
		}
#if 0
		CHARRANGE cr;
		cr.cpMin = 0;
		cr.cpMax = -1;
		SendMessage(pREBox->hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);

		cr.cpMin = cr.cpMax = 0;
		SendMessage(pREBox->hwnd, EM_EXGETSEL, 0, (LPARAM)&cr);
		CIUIString strText;
		LPTSTR lpsz = strText.GetBufferSetLength((cr.cpMax - cr.cpMin + 1) * 2);
		lpsz[0] = NULL;
		SendMessage(pREBox->hwnd, EM_GETSELTEXT, 0, (LPARAM)lpsz);
		strText.ReleaseBuffer();

		rc.OffsetRect(-rc.TopLeft());
		HDC hDCN = GetWindowDC(NULL);
		SetBkMode(hDCN, TRANSPARENT);
		LRESULT lr2 = pTextServices->TxDraw(
				DVASPECT_CONTENT,          // Draw Aspect
				-1,                // Lindex
				NULL,                    // Info for drawing optimazation
				NULL,                    // target device information
				hDCN,                    // Draw device HDC
				NULL,                        // Target device HDC
				(RECTL *)&rc,            // Bounding client rectangle
				NULL,                     // Clipping rectangle for metafiles
				(RECT *)&rc,        // Update rectangle
				NULL,                        // Call back function
				NULL,                    // Call back parameter
				TXTVIEW_ACTIVE);                        // What view of the object

		int n = 0;
#endif
	}

	return lr;
}

int IUI::RichTextBox_DrawBkColor(REBox *pREBox, HDC hDC, const RECT &rcClip)
{
	DrawBkColor(pREBox->hwnd, hDC, &rcClip);

	return 0;
}

int IUI::RichTextBox_DrawBkImage(REBox *pREBox, HDC hDC, const RECT &rcClip)
{
	CIUIRect rc;
	GetClientRect(pREBox->hwnd, rc);

	CONTROL_STATE eButtonState = GetControlState(pREBox->hwnd);
	if (eButtonState < CONTROL_STATE_NORMAL || eButtonState > CONTROL_STATE_DISABLED)
	{
		_ASSERT(FALSE);
		return -1;
	}
	int nStateIndex = 0;
	if (IsWindowEnabled(pREBox->hwnd))
	{
		if (IUI::GetStyle(pREBox->hwnd) & ES_READONLY)
		{
			nStateIndex = 3;
		}
		else
		{
			nStateIndex = GetButtonStateIndex(eButtonState);
		}
	}
	else
	{
		nStateIndex = 3;
	}

	if (IsCombineBkImage(pREBox->hwnd))
	{
		if (pREBox->hwnd->ci.m_eBkImageResizeMode == IRM_9GRID)
		{
			IUIPartNineGridBlt(hDC, rc,
				pREBox->hwnd->ci.m_himgCombineBk, pREBox->hwnd->ci.m_rcBkImage9GridResizeArg,
				COMBINEIMAGESIZE4, nStateIndex);
		}
	}
	else
	{
		IUIDrawImage(hDC,
			rc,
			pREBox->m_himgBk[nStateIndex],
			pREBox->hwnd->ci.m_eBkImageResizeMode,
			pREBox->hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;
}

int RichTextBox_OnPaint(REBox *pREBox, HDC hDC, const RECT &rcClip)
{
	pREBox->m_hDC = hDC;

	ITextServices *pTextServices = pREBox->m_pTextHost->GetTextServices();

	LRESULT lr = S_OK;
	if (pTextServices != NULL)
	{
		CIUIRect rc;
		GetClientRect(pREBox->hwnd, rc);

		if (!rc.IsRectEmpty())
		{
			int nWidth = rc.Width();
			int nHeight = rc.Height();

			int nStateIndex = 0;

			HDC hDrawDC = NULL;
			HBITMAP hTempBmp = NULL;
			HBITMAP hOldBmp = NULL;
			void *pBits = NULL;
			BOOL bUpdateLayeredWindow = IsUpdateLayeredWindow(GetHostWnd(pREBox->hwnd));
			//bUpdateLayeredWindow = FALSE;
			if (bUpdateLayeredWindow)
			{
				HDC hTempDC = CreateCompatibleDC(hDC);
				if (hTempDC == NULL)
				{
					_ASSERT(FALSE);
					return -1;
				}

				hTempBmp = Create32BitsHBITMAP(nWidth, nHeight, 32, BI_RGB, &pBits);
				//HBITMAP hTempBmp = CreateCompatibleBitmap(hDC, nWidth, nHeight);
				if (hTempBmp == NULL)
				{
					_ASSERT(FALSE);
					return -2;
				}

				hOldBmp = (HBITMAP)::SelectObject(hTempDC, hTempBmp);

				// 拷贝RichEdit覆盖区域
				BitBlt(hTempDC, 0, 0, nWidth, nHeight, hDC, 0, 0, SRCCOPY);

				// 把hTempBmp各像素的Alpha值设置为0xFF
				// 图像像素是从左到右、从下向上的。
				for (int y = 0; y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						byte *pByte = (byte *)(pBits) + (y * nWidth * 4 + x * 4);
						//pByte[0] = 0x00; // B
						//pByte[1] = 0x00; // G
						//pByte[2] = 0xFF; // R
						pByte[3] = 0xFF; // alpha
					}
				}

				hDrawDC = hTempDC;
			}
			else
			{
				hDrawDC = hDC;
			}

			BeforePaint(pREBox->hwnd, hDrawDC);
			RichTextBox_DrawBkColor(pREBox, hDrawDC, rcClip);
			AfterPaintBkColor(pREBox->hwnd, hDrawDC);
			RichTextBox_DrawBkImage(pREBox, hDrawDC, rcClip);
			AfterPaintBkImage(pREBox->hwnd, hDrawDC);
			lr = RichTextBox_DrawText(pREBox, hDrawDC, rcClip);

			if (bUpdateLayeredWindow)
			{
				// 把hTempBmp中alpha为0的像素的Alpha值设置为0xFF
				for (int y = 0; SUCCEEDED(lr) && y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						byte *pByte = (byte *)(pBits) + (y * nWidth * 4 + x * 4);
						if (pByte[3] == 0x00)
						{
							pByte[3] = 0xFF; // Alpha
						}
					}
				}

				// 把修复好后的位图，贴到目标DC上。
				if (SUCCEEDED(lr))
				{
					BitBlt(hDC, 0, 0, nWidth, nHeight, hDrawDC, 0, 0, SRCCOPY);
				}

				// 释放
				::SelectObject(hDrawDC, hOldBmp);
				hOldBmp = NULL;
				::DeleteObject(hTempBmp);
				hTempBmp = NULL;
				DeleteDC(hDrawDC);
				hDrawDC = NULL;
			}
		}
	}

	AfterPaint(pREBox->hwnd, hDC);

	return SUCCEEDED(lr) ? 0 : -1;
}

// RichEdit收到鼠标消息后，如果Ole对象绑定了控件，根据具体情况，转发给这些控件
BOOL RE_IsMouseMessage(REBox *pRe, RoutedEventArgs *pe)
{
	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
	{
		return FALSE;
	}

#ifdef _DEBUG
	// 调试用代码
	if (uMsg == WM_LBUTTONDOWN)
	{
		int n = 0;
	}
#endif

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pRe->hwnd);
	CIUIPoint pt(lParam);

	// 命中Ole对象
	IOleObject *pOleObject = pRichTextBox->HitTestOleObject(&pt);
	if (NULL == pOleObject)
	{
		return FALSE;
	}

	OleControl *pOleControl = NULL;
	pOleObject->QueryInterface(IID_IUIOleControl, (void **)&pOleControl);
	if (NULL == pOleControl)
	{
		// 命中的Ole对象不是IUI Ole
		return FALSE;
	}

	//
	// 命中绑定到Ole的控件。
	//
	HWLWND hDest = HitTestChild(pOleControl->GetOleBindWindowless(), &pt);
	if (NULL == hDest)
	{
		return FALSE;
	}

	static int i = 0;
	//IUITRACE(_T("%d hDest is %s, caption is %s\n"), i++, GetName(hDest), GetWindowText(hDest));

	// 命中测试
	LRESULT lrHittest = HTCLIENT;
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

	CIUIPoint ptScreen = lParam;
	ClientToScreen(pRe->hwnd, &ptScreen);
	lrHittest = IUI::HitTest(hDest, &ptScreen);

	if (pThreadState->m_pWLMouseIn != hDest)
	{
		pThreadState->m_pWLMouseIn = hDest;
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
		ClientToScreen(pRe->hwnd, &ptDest);
		ScreenToClient(hDest, &ptDest);

		RoutedEventArgs arg;
		arg.hOriginalSender = hDest;
		arg.RoutedEvent = uNewMsg;
		arg.eRoutingStrategy = RS_TUNNEL;
		arg.wParam = wParam;
		arg.lParam = MAKELPARAM(ptDest.x, ptDest.y);

		RaiseEvent(hDest, &arg);
	}

	// TODO: If there has a capture windowless control, pass the message to it directly.

	return (hDest != NULL);
}

LRESULT WINAPI IUI::RichTextBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	REBox *pREBox = (REBox *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	// 要保证所有Window消息，都转发给m_pTextHost->TxWindowProc
	// 对于宿主窗口收到的鼠标消息，判断当前命中的是否是RichEdit，
	// 如果是，直接转发。 对于宿主窗口收到的非鼠标消息，如果当前焦点窗口是RichEdit，
	// 转发给RichEdit.
	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pe->RoutedEvent == WM_NCCREATE)
			{
				if (pREBox == NULL)
				{
					pREBox = new REBox;
					pREBox->hwnd = hWnd;
					SetMember(hWnd, pREBox);
					return 0;
				}
			}
			else if (pe->RoutedEvent == WM_CREATE)
			{
				CREATESTRUCT *pcs = (CREATESTRUCT *)pe->lParam;

				// See CTxtWinHost::OnNCCreate
				pREBox->m_pTextHost = new CTxtWinHost;
				if (pREBox->m_pTextHost == NULL)
				{
					return FALSE;
				}

				// 初始化
				CREATESTRUCT cs;
				cs = *pcs;
				cs.dwExStyle |= WS_EX_TRANSPARENT;

				if (!pREBox->m_pTextHost->Init(hWnd, &cs))
				{
					pREBox->m_pTextHost->Shutdown();
					delete pREBox->m_pTextHost;
					return FALSE;
				}

				// 在OnCreate中，会调用OnTxInPlaceActivate，而OnTxInPlaceActivate内部，
				// 会调用TxGetDC，如果TxGetDC返回NULL，OnTxInPlaceActivate就会失败。
				// 所以，先提供一个临时的HDC
				pREBox->m_hDC = ::GetDC(GetHostWnd(hWnd));

				pREBox->m_pTextHost->OnCreate(&cs);

				ReleaseDC(GetHostWnd(hWnd), pREBox->m_hDC);
				pREBox->m_hDC = NULL;

				return 0;
			}
			else if (pe->RoutedEvent == WM_DESTROY)
			{
				pREBox->Release();
				delete pREBox;
				pREBox = NULL;
				SetMember(hWnd, NULL);
			}
			else if (pe->RoutedEvent == WM_WINDOWPOSCHANGED)
			{
				if (pREBox->m_pTextHost != NULL)
				{
					CIUIRect rc;
					GetClientRect(hWnd, rc);
					//					m_pTextHost->SetClientRect(rc);
				}
			}

			// 其它消息，在执行WM_DESTROY后，pREBox为NULL
			if (pREBox != NULL && pREBox->m_pTextHost != NULL)
			{
				LRESULT lr = pREBox->m_pTextHost->TxWindowProc(pe->RoutedEvent, pe->wParam, pe->lParam);
				if (0 != lr)
				{
					pe->bHandled = TRUE;
					return 0;
				}
				return lr;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			BOOL bChildHandled = RE_IsMouseMessage(pREBox, pe);

			if (pe->RoutedEvent >= WM_MOUSEFIRST
				&& pe->RoutedEvent <= WM_MOUSELAST)
			{
				CIUIPoint pt(pe->lParam);
				ClientToScreen(hWnd, &pt);
				if (pe->RoutedEvent == WM_MOUSEWHEEL)
				{
					// 如果RichTextBox作为ComboBox的孩子, 则把消息转发给ComboBox
					HWLWND hParent = GetParent(hWnd);
					if (IUI::GetClassName(hParent) == IUI_COMBOBOX)
					{
						SendMessage(hParent, WM_MOUSEWHEEL, wParam, MAKELPARAM(pt.x, pt.y));
						return 0;
					}
				}
				else
				{
					// 转成相对Host的坐标
					::ScreenToClient(GetHostWnd(hWnd), &pt);
					pe->lParam = MAKELPARAM(pt.x, pt.y);
				}
			}

			// 命中OLE绑定的子控件后，鼠标是否继续交给默认窗口过程处理
			if (bChildHandled && !pREBox->m_bDefaultProcHandleIfHitOleCtrl)
			{
				pe->bHandled = TRUE;
				return 0;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			// 把窗口向RichTextBox发送的非鼠标消息，全部转发给RichEdit内部。
			if (pREBox != NULL)
			{
				if (pREBox->m_pTextHost != NULL && pREBox->m_pTextHost->m_pserv != NULL)
				{
					switch (uMsg)
					{
					case WM_PRINTCLIENT:
					case WM_PAINT:
					case WM_PRINT:
						return RichTextBox_OnPaint(pREBox, (HDC)wParam, *LPCRECT(lParam));

					case WLM_BIND_STYLE:
						RichTextBox_BindStyle(hWnd, (CRichTextBoxProp *)lParam);
						return 0;

					case REM_GET_TEXT_SERVICES:
						return (LRESULT)pREBox->m_pTextHost->m_pserv;

					case REM_GET_TEXT_DOCUMENT:
					{
						if (pREBox->m_pTextDocument == NULL)
						{
							IRichEditOle *pRichEditOle = NULL;
							SendMessage(hWnd, EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);

							if (pRichEditOle == NULL)
							{
								return NULL;
							}

							HRESULT hr = pRichEditOle->QueryInterface(::IID_ITextDocument,
									(void **)&pREBox->m_pTextDocument);
							pREBox->m_pTextDocument->AddRef();
						}

						return (LRESULT)pREBox->m_pTextDocument;
					}

					case REM_GET_DC:
						return (LRESULT)pREBox->m_hDC;

					case REM_REVOKEDRAGDROP:
						// 不接受拖放操作“放”的数据。注：不限制发起拖放
						pREBox->m_pTextHost->RevokeDragDrop();
						return 0;

					case REM_SET_DEFAULTPROC_HANDLE:
					{
						BOOL bOld = pREBox->m_bDefaultProcHandleIfHitOleCtrl;
						pREBox->m_bDefaultProcHandleIfHitOleCtrl = lParam;
						return bOld;
					}

					case REM_GET_DEFAULTPROC_HANDLE:
						return pREBox->m_bDefaultProcHandleIfHitOleCtrl;

					default:
					{
						LRESULT lr = pREBox->m_pTextHost->TxWindowProc(uMsg, wParam, lParam);
						if (lr != 0)
						{
							pe->bHandled = TRUE;
							return lr;
						}
						break;
					}
					}
				}
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
	}

	// RichEdit在接受到滚动条拖动滑块发出的WM_XSCROLL消息后，并没有向滚动条调用SetScrollPos
	// 这将导致滚动条的滑块不能刷新，通过向滚动条发送SBM_REFRSHSELF消息，让滚动条可以自己刷新。
	if (pe->RoutedEvent == WM_CREATE)
	{
		if (GetClassName(pe->hOriginalSender) == IUI_SCROLLBAR)
		{
			if (GetName(pe->hOriginalSender) == XNAME_HORZSCROLLBAR
				|| GetName(pe->hOriginalSender) == XNAME_VERTSCROLLBAR)
			{
				SendMessage(pe->hOriginalSender, SBM_REFRSHSELF, 0, TRUE);
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

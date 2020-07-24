#include "stdafx.h"
#include "RichTextBox.h"
#include "_common.h"
#include "_nlsprcs.h"
#include "_clasfyc.h"

RichTextBox::RichTextBox()
{
}

RichTextBox::~RichTextBox()
{
}

extern UINT gWM_MouseRoller;
BOOL RichTextBox::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, LPCTSTR lpszName)
{
	// Copy from DllMain in reinit.cpp
	InitializeCriticalSection(&g_CriticalSection);
	InitUnicodeWrappers();			 	// Init UNICODE wrappers for Chicago
	GetSysParms();						// Define screen/window parms
	RegisterFETCs();					// Register new clipboard formats
	CreateFormatCaches();				// Create global format caches
	InitNLSProcTable();					// Get National Language Support Functions.
	if (!InitKinsokuClassify())		// Init tables for classifying Unicode chars.
	{
		return FALSE;
	}
	// Magellan Mouse.
	gWM_MouseRoller = RegisterWindowMessageW(MSH_MOUSEWHEEL);

	//hinstRE = hmod;

	// 	osversioninfo.dwOSVersionInfoSize = sizeof(_OSVERSIONINFOA);
	//
	// 	GetVersionExA(&osversioninfo);
	// 	dwPlatformId = osversioninfo.dwPlatformId;
	// 	dwMajorVersion = osversioninfo.dwMajorVersion;
	//
	// 	if (osversioninfo.dwMajorVersion >= VERS4)
	// 	{
	// 		icr3DDarkShadow = COLOR_3DDKSHADOW;
	// 	}


	RichFRegisterClass();

	CWnd *pWnd = this;
	return pWnd->Create(RICHEDIT_CLASSW, NULL, dwStyle, rect, pParentWnd, 1);
}

BOOL RichTextBox::CanUndo() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_CANUNDO, 0, 0);
}
BOOL RichTextBox::CanRedo() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_CANREDO, 0, 0);
}
UNDONAMEID RichTextBox::GetUndoName() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UNDONAMEID)SendMessage(EM_GETUNDONAME, 0, 0);
}
UNDONAMEID RichTextBox::GetRedoName() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UNDONAMEID)SendMessage(EM_GETREDONAME, 0, 0);
}
int RichTextBox::GetLineCount() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (int)SendMessage(EM_GETLINECOUNT, 0, 0);
}
BOOL RichTextBox::GetModify() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_GETMODIFY, 0, 0);
}
void RichTextBox::SetModify(BOOL bModified /* = TRUE */)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_SETMODIFY, bModified, 0);
}
BOOL RichTextBox::SetTextMode(UINT fMode)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_SETTEXTMODE, (WPARAM) fMode, 0);
}
UINT RichTextBox::GetTextMode() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT)SendMessage(EM_GETTEXTMODE, 0, 0);
}
void RichTextBox::GetRect(LPRECT lpRect) const
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_GETRECT, 0, (LPARAM)lpRect);
}
CPoint RichTextBox::GetCharPos(long lChar) const
{
	ASSERT(::IsWindow(m_hWnd));
	CPoint pt;
	SendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar);
	return pt;
}
UINT RichTextBox::GetOptions() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT)SendMessage(EM_GETOPTIONS, 0, 0);
}
void RichTextBox::SetOptions(WORD wOp, DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_SETOPTIONS, (WPARAM)wOp, (LPARAM)dwFlags);
}
BOOL RichTextBox::SetAutoURLDetect(BOOL bEnable /* = TRUE */)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_AUTOURLDETECT, (WPARAM) bEnable, 0);
}
void RichTextBox::EmptyUndoBuffer()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_EMPTYUNDOBUFFER, 0, 0);
}
UINT RichTextBox::SetUndoLimit(UINT nLimit)
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT)SendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0);
}
void RichTextBox::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText);
}
void RichTextBox::SetRect(LPCRECT lpRect)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_SETRECT, 0, (LPARAM)lpRect);
}
void RichTextBox::StopGroupTyping()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_STOPGROUPTYPING, 0, 0);
}
BOOL RichTextBox::Redo()
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_REDO, 0, 0);
}
BOOL RichTextBox::Undo()
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_UNDO, 0, 0);
}
void RichTextBox::Clear()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(WM_CLEAR, 0, 0);
}
void RichTextBox::Copy()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(WM_COPY, 0, 0);
}
void RichTextBox::Cut()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(WM_CUT, 0, 0);
}
void RichTextBox::Paste()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(WM_PASTE, 0, 0);
}
BOOL RichTextBox::SetReadOnly(BOOL bReadOnly /* = TRUE */)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_SETREADONLY, bReadOnly, 0L);
}
int RichTextBox::GetFirstVisibleLine() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (int)SendMessage(EM_GETFIRSTVISIBLELINE, 0, 0L);
}
BOOL RichTextBox::DisplayBand(LPRECT pDisplayRect)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_DISPLAYBAND, 0, (LPARAM)pDisplayRect);
}
void RichTextBox::GetSel(CHARRANGE &cr) const
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
}
BOOL RichTextBox::GetPunctuation(UINT fType, PUNCTUATION *lpPunc) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_GETPUNCTUATION, (WPARAM) fType, (LPARAM) lpPunc);
}
BOOL RichTextBox::SetPunctuation(UINT fType, PUNCTUATION *lpPunc)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_SETPUNCTUATION, (WPARAM) fType, (LPARAM) lpPunc);
}
void RichTextBox::LimitText(long nChars)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_EXLIMITTEXT, 0, nChars);
}
long RichTextBox::LineFromChar(long nIndex) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_EXLINEFROMCHAR, 0, nIndex);
}
CPoint RichTextBox::PosFromChar(UINT nChar) const
{
	ASSERT(::IsWindow(m_hWnd));
	// RichEdit版本不同，这个消息的参数和返回值的意义也不同。
	// 这份RichEdit源代码的WPARAM表示cp，返回值表示坐标
	//POINTL pt;
	//SendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar);
	LRESULT pt = SendMessage(EM_POSFROMCHAR, nChar, 0);
	return CPoint(pt);
}
int RichTextBox::CharFromPos(CPoint pt) const
{
	ASSERT(::IsWindow(m_hWnd));
	POINTL ptl = {pt.x, pt.y};
	return (int)SendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}
void RichTextBox::SetSel(CHARRANGE &cr)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}
DWORD RichTextBox::FindWordBreak(UINT nCode, DWORD nStart) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (DWORD)SendMessage(EM_FINDWORDBREAK, (WPARAM) nCode, (LPARAM) nStart);
}

long RichTextBox::FindText(DWORD dwFlags, FINDTEXTEX *pFindText) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_FINDTEXTEX, dwFlags, (LPARAM)pFindText);
}

long RichTextBox::FormatRange(FORMATRANGE *pfr, BOOL bDisplay)
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_FORMATRANGE, (WPARAM)bDisplay, (LPARAM)pfr);
}

long RichTextBox::GetEventMask() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_GETEVENTMASK, 0, 0L);
}

long RichTextBox::GetLimitText() const
{
	return (long)SendMessage(EM_GETLIMITTEXT, 0, 0L);
}

//long RichTextBox::GetSelText(__out LPSTR lpBuf) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	return (long)SendMessage(EM_GETSELTEXT, 0, (LPARAM)lpBuf);
//}

void RichTextBox::HideSelection(BOOL bHide, BOOL bPerm)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_HIDESELECTION, bHide, bPerm);
}

void RichTextBox::RequestResize()
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_REQUESTRESIZE, 0, 0L);
}

WORD RichTextBox::GetSelectionType() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (WORD)SendMessage(EM_SELECTIONTYPE, 0, 0L);
}

UINT RichTextBox::GetWordWrapMode() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT) SendMessage(EM_GETWORDWRAPMODE, 0, 0);
}

UINT RichTextBox::SetWordWrapMode(UINT uFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT)SendMessage(EM_SETWORDWRAPMODE, (WPARAM) uFlags, 0);
}

COLORREF RichTextBox::SetBackgroundColor(BOOL bSysColor, COLORREF cr)
{
	ASSERT(::IsWindow(m_hWnd));
	return (COLORREF)SendMessage(EM_SETBKGNDCOLOR, bSysColor, cr);
}

DWORD RichTextBox::SetEventMask(DWORD dwEventMask)
{
	ASSERT(::IsWindow(m_hWnd));
	return (DWORD)SendMessage(EM_SETEVENTMASK, 0, dwEventMask);
}

BOOL RichTextBox::SetOLECallback(IRichEditOleCallback *pCallback)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback);
}

BOOL RichTextBox::SetTargetDevice(HDC hDC, long lLineWidth)
{
	ASSERT(::IsWindow(m_hWnd));
	return (BOOL)SendMessage(EM_SETTARGETDEVICE, (WPARAM)hDC, lLineWidth);
}

long RichTextBox::StreamIn(int nFormat, EDITSTREAM &es)
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_STREAMIN, nFormat, (LPARAM)&es);
}

long RichTextBox::StreamOut(int nFormat, EDITSTREAM &es)
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es);
}

long RichTextBox::GetTextLength() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (long)SendMessage(WM_GETTEXTLENGTH, NULL, NULL);
}

int RichTextBox::GetLine(__in int nIndex, LPTSTR lpszBuffer) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (int)SendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
}

int RichTextBox::LineIndex(int nLine /* = -1 */) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (int)SendMessage(EM_LINEINDEX, nLine, 0);
}

int RichTextBox::LineLength(int nLine /* = -1 */) const
{
	ASSERT(::IsWindow(m_hWnd));
	return (int)SendMessage(EM_LINELENGTH, nLine, 0);
}

void RichTextBox::LineScroll(int nLines, int nChars /* = 0 */)
{
	ASSERT(::IsWindow(m_hWnd));
	SendMessage(EM_LINESCROLL, nChars, nLines);
}

void RichTextBox::SetSel(long nStartChar, long nEndChar)
{
	ASSERT(::IsWindow(m_hWnd));
	CHARRANGE cr;
	cr.cpMin = nStartChar;
	cr.cpMost = nEndChar;
	SendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}

BOOL RichTextBox::CanPaste(UINT nFormat) const
{
	ASSERT(::IsWindow(m_hWnd));
	//COleMessageFilter *pFilter = AfxOleGetMessageFilter();
	//if (pFilter != NULL)
	//{
	//	pFilter->BeginBusyState();
	//}
	BOOL b = (BOOL)SendMessage(EM_CANPASTE, nFormat, 0L);
	//if (pFilter != NULL)
	//{
	//	pFilter->EndBusyState();
	//}
	return b;
}

void RichTextBox::PasteSpecial(UINT nClipFormat, DWORD dvAspect, HMETAFILE hMF)
{
	ASSERT(::IsWindow(m_hWnd));
	REPASTESPECIAL reps;
	reps.dwAspect = dvAspect;
	reps.dwParam = (DWORD_PTR)hMF;
	SendMessage(EM_PASTESPECIAL, nClipFormat, (LPARAM)&reps);
}

int RichTextBox::GetLine(__in int nIndex, LPTSTR lpszBuffer, __in int nMaxLength) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(sizeof(nMaxLength) <= nMaxLength * sizeof(TCHAR) && nMaxLength > 0);
	*(LPINT)lpszBuffer = nMaxLength;
	return (int)SendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
}

void RichTextBox::GetSel(long &nStartChar, long &nEndChar) const
{
	ASSERT(::IsWindow(m_hWnd));
	CHARRANGE cr;
	SendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	nStartChar = cr.cpMin;
	nEndChar = cr.cpMost;
}

CString RichTextBox::GetSelText() const
{
	ASSERT(::IsWindow(m_hWnd));
	CHARRANGE cr;
	cr.cpMin = cr.cpMost = 0;
	SendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	CString strText;
	LPTSTR lpsz = strText.GetBufferSetLength((cr.cpMost - cr.cpMin + 1) * 2);
	lpsz[0] = NULL;
	SendMessage(EM_GETSELTEXT, 0, (LPARAM)lpsz);
	strText.ReleaseBuffer();
	return CString(strText);
}

IRichEditOle *RichTextBox::GetIRichEditOle() const
{
	ASSERT(::IsWindow(m_hWnd));

	//if (m_pTwh == NULL)
	//{
	//	return NULL;
	//}

	//ITextServices *pTextServices = m_pTwh->GetTextServices();

	//if (!pTextServices)
	//{
	//	ASSERT(0);
	//	return NULL;
	//}

	IRichEditOle *pRichEditOle = NULL;
	SendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);

	if (NULL == pRichEditOle)
	{
		ASSERT(0);
		return NULL;
	}

	return pRichEditOle;
}

long RichTextBox::GetTextLengthEx(DWORD dwFlags, UINT nCodePage/* = -1*/) const
{
	ASSERT(::IsWindow(m_hWnd));
	GETTEXTLENGTHEX textLenEx;
	textLenEx.flags = dwFlags;

	if (nCodePage == -1)
	{
#ifdef _UNICODE
		// UNICODE code page
		textLenEx.codepage = 1200;
#else
		// default code page
		textLenEx.codepage = CP_ACP;
#endif
	}
	else
		// otherwise, use the code page specified
	{
		textLenEx.codepage = nCodePage;
	}

	return (long)SendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0);
}

int RichTextBox::GetTextRange(int nFirst, int nLast, CString &refString) const
{
	ASSERT(::IsWindow(m_hWnd));

#ifdef _UNICODE
	TEXTRANGEW textRange;
#else
	TEXTRANGE textRange;
#endif
	textRange.chrg.cpMin = nFirst;
	textRange.chrg.cpMost = nLast;

	// can't be backwards
	int nLength = int(nLast - nFirst + 1);
	ASSERT(nLength > 0);

	textRange.lpstrText = refString.GetBuffer(nLength);
	nLength = (int)SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange);
	refString.ReleaseBuffer(nLength);

	return nLength;
}

BOOL RichTextBox::SetDefaultCharFormat(CHARFORMAT &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);
}

BOOL RichTextBox::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);
}

BOOL RichTextBox::SetSelectionCharFormat(CHARFORMAT &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

BOOL RichTextBox::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

BOOL RichTextBox::SetWordCharFormat(CHARFORMAT &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
}

BOOL RichTextBox::SetWordCharFormat(CHARFORMAT2 &cf)
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (BOOL)SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
}

DWORD RichTextBox::GetDefaultCharFormat(CHARFORMAT &cf) const
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT);
	return (DWORD)SendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf);
}

DWORD RichTextBox::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (DWORD)SendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf);
}

DWORD RichTextBox::GetSelectionCharFormat(CHARFORMAT &cf) const
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT);
	return (DWORD)SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

DWORD RichTextBox::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
	ASSERT(::IsWindow(m_hWnd));
	cf.cbSize = sizeof(CHARFORMAT2);
	return (DWORD)SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

DWORD RichTextBox::GetParaFormat(PARAFORMAT &pf) const
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT);
	return (DWORD)SendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf);
}

DWORD RichTextBox::GetParaFormat(PARAFORMAT2 &pf) const
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT2);
	return (DWORD)SendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf);
}

BOOL RichTextBox::SetParaFormat(PARAFORMAT &pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT);
	return (BOOL)SendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
}

BOOL RichTextBox::SetParaFormat(PARAFORMAT2 &pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT2);
	return (BOOL)SendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
}

int GetXHimetricFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);
	int nRet = MulDiv(nPixel, 2540, nXDPI);

	return nRet;
}

int GetYHimetricFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);
	int nRet = MulDiv(nPixel, 2540, nYDPI);

	return nRet;
}

int GetXPixelFromHimetric(int nHimetric)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	int nRet = MulDiv(nHimetric, nXDPI, 2540);

	return nRet;
}

int GetYPixelFromHimetric(int nHimetric)
{
	// TODO: 这些DPI值，应该得到一次后保存下来
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	int nRet = MulDiv(nHimetric, nYDPI, 2540);

	return nRet;
}

int RichTextBox::InsertOle(IOleObject *pOleObject, LONG cp, int nWidth, int nHeight)
{
	try
	{
		//
		// 1. Get IRichEditOle and IOleClientSite interface
		//
		CComPtr<IRichEditOle> pRichEditOle = GetIRichEditOle();

		CComPtr<IOleClientSite> pClientSite = NULL;
		HRESULT hr = pRichEditOle->GetClientSite(&pClientSite);
		if (FAILED(hr))
		{
			return -1;
		}

		//
		// 2. Create ILockBytes interface
		//
		CComPtr<ILockBytes> pLockBytes = NULL;
		hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
		if (FAILED(hr))
		{
			return -2;
		}

		//
		// 3. Create IStorage interface
		//
		CComPtr<IStorage> pStorage = NULL;
		hr = ::StgCreateDocfileOnILockBytes(pLockBytes,
				STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);
		if (FAILED(hr))
		{
			return -3;
		}

		//
		// 4. Add OLE to Richedit
		//
		CLSID clsid;// = CLSID_NULL;
		hr = pOleObject->GetUserClassID(&clsid);

		REOBJECT reobject;
		ZeroMemory(&reobject, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		reobject.clsid = clsid;
		reobject.cp = cp;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.dwFlags = REO_BELOWBASELINE;
		reobject.dwUser = 7878; // 签名，当dwUser为7878时，即表示是我们插入的ole
		reobject.poleobj = pOleObject;
		reobject.polesite = pClientSite;
		reobject.pstg = pStorage;

		// REOBJECT::sizel的单位是0.01毫米
		SIZEL sizel = { GetXHimetricFromPixel(nWidth), GetYHimetricFromPixel(nHeight) };
		reobject.sizel = sizel;
		hr = pRichEditOle->InsertObject(&reobject);
		if (FAILED(hr))
		{
			return -4;
		}

		CComPtr<IOleClientSite> pOldClientSite = NULL;
		hr = pOleObject->GetClientSite(&pOldClientSite);
		if (pOldClientSite == NULL)
		{
			pOleObject->SetClientSite(pClientSite);
		}
	}
	catch (...)
	{
	}

	return 0;
}

// 采用二分法查找指定cp后面第一个Ole对象在Ole管理中的索引（非在RichEdit中的cp）
// 参数微软泄漏代码：CObjectMgr::FindIndexForCp
int FindObjectNearCp(RichTextBox *pRe, LONG cp, __out REOBJECT *pReObj)
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

int RichTextBox::GetFirstVisibleOleOjbect(__out REOBJECT *pObj)
{
	if (NULL == pObj)
	{
		return -1;
	}

	// 先得到第一个可见的字符位置
	int nFirstVisibleLine = GetFirstVisibleLine();
	int nFirstVisibleCp = LineIndex(nFirstVisibleLine);

	CRect rcRe;
	GetClientRect(rcRe);

	// 采用二分查找法，可定位第一个可见的Ole object
	REOBJECT reObj;
	int nFirstVisibleOle = FindObjectNearCp(this, nFirstVisibleCp, &reObj);
	if (nFirstVisibleOle < 0)
	{
		return -1;
	}

	_ASSERT(NULL != reObj.poleobj);

	// 得到Ole的坐标
	CPoint pt = PosFromChar(reObj.cp);
	int nOleWidth = GetXPixelFromHimetric(reObj.sizel.cx);
	int nOleHeight = GetYPixelFromHimetric(reObj.sizel.cy);
	CRect rcOle;
	rcOle.left = pt.x;
	rcOle.top = pt.y;
	rcOle.right = rcOle.left + nOleWidth;
	rcOle.bottom = rcOle.top + nOleHeight;

	// 看Ole是否在可视范围内
	rcRe.IntersectRect(rcRe, rcOle);
	if (rcRe.IsRectEmpty() || rcRe.IsRectNull())
	{
		return -1;
	}

	*pObj = reObj;

	return nFirstVisibleOle;
}

IOleObject *RichTextBox::HitTest(LPPOINT lpPt)
{
	REOBJECT reObj;
	int nOleIndex = GetFirstVisibleOleOjbect(&reObj);
	if (nOleIndex < 0)
	{
		return NULL;
	}

	CRect rcRe;
	GetClientRect(rcRe);

	IOleObject *pRet = NULL;

	CComPtr<IRichEditOle> pRichEditOle = GetIRichEditOle();
	LONG nOleCount = pRichEditOle->GetObjectCount();

	for (int i = nOleIndex; i < nOleCount; ++i)
	{
		REOBJECT ro;
		ro.cbStruct = sizeof(REOBJECT);
		HRESULT hr = pRichEditOle->GetObject(i, &ro, REO_GETOBJ_POLEOBJ);

		// 得到Ole的坐标
		CPoint pt = PosFromChar(ro.cp);
		CRect rcOle;
		rcOle.left = pt.x;
		rcOle.top = pt.y;
		rcOle.right = rcOle.left + GetXPixelFromHimetric(ro.sizel.cx);
		rcOle.bottom = rcOle.top + GetYPixelFromHimetric(ro.sizel.cy);

		// 看Ole是否在可视范围内
		CRect rcIntersect;
		rcIntersect.IntersectRect(rcRe, rcOle);
		if (rcIntersect.IsRectEmpty() || rcIntersect.IsRectNull())
		{
			continue;
		}

		if (rcOle.PtInRect(*lpPt))
		{
			pRet = ro.poleobj;
			break;
		}
	}

	return pRet;
}

LRESULT RichTextBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_LBUTTONDOWN == message)
	{
		IOleObject *pHitTestOle = HitTest(&CPoint(lParam));
		if (NULL != pHitTestOle)
		{
			return 0;
		}

		//LONG nOldSelMinCp = -1;
		//LONG nOldSelMostCp = -1;
		//GetSel(nOldSelMinCp, nOldSelMostCp);

		//LRESULT lr = CWnd::WindowProc(message, wParam, lParam);

		//LONG nSelMinCp = -1;
		//LONG nSelMostCp = -1;
		//GetSel(nSelMinCp, nSelMostCp);
		//if (nSelMinCp >= 0 && nSelMinCp + 1 == nSelMostCp) // 单选
		//{
		//	REOBJECT reo;
		//	reo.cbStruct = sizeof(REOBJECT);
		//	int nOleIndex = FindObjectNearCp(this, nSelMinCp, &reo);
		//	// 自己被单选
		//	if (reo.cp == nSelMinCp
		//		&& (reo.dwFlags & REO_SELECTED))
		//	{
		//		SetSel(nOldSelMinCp, nOldSelMostCp);
		//	}
		//}

		//return lr;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
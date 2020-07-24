// 内部头文件，不提供给用户
#pragma once


#define WHEEL_DELTA     120

extern const TCHAR c_szTreeViewClass[];

#ifdef USE_MIRRORING
#define RTL_MIRRORED_WINDOW             dwExStyleRTLMirrorWnd
#define IS_WINDOW_RTL_MIRRORED(hwnd)    (g_bMirroredOS && Mirror_IsWindowMirroredRTL(hwnd))
#else
#define RTL_MIRRORED_WINDOW             0L
#define IS_WINDOW_RTL_MIRRORED(hwnd)    FALSE
#endif

#define RECTWIDTH(rc)   ((rc).right-(rc).left)
#define RECTHEIGHT(rc)  ((rc).bottom-(rc).top)



#ifdef WIN32
#define CODESEG_INIT        ".text"
#else
#define CODESEG_INIT 	    "_INIT"
#endif

#define TVE_ACTIONMASK          0x0007      //  (TVE_COLLAPSE | TVE_EXPAND | TVE_TOGGLE)

#ifdef WIN32
typedef MSG MSG32;
typedef MSG32 *LPMSG32;
#define GetMessage32(lpmsg, hwnd, min, max, f32)        GetMessage(lpmsg, hwnd, min, max)
#define PeekMessage32(lpmsg, hwnd, min, max, flags, f32)       PeekMessage(lpmsg, hwnd, min, max, flags)
#define TranslateMessage32(lpmsg, f32)  TranslateMessage(lpmsg)
#define DispatchMessage32(lpmsg, f32)   DispatchMessage(lpmsg)
#define CallMsgFilter32(lpmsg, u, f32)  CallMsgFilter(lpmsg, u)
#define IsDialogMessage32(hwnd, lpmsg, f32)   IsDialogMessage(hwnd, lpmsg)
#endif

#define SSW_EX_NOTIMELIMIT      0x00010000
#define SSW_EX_IMMEDIATE        0x00020000
#define SSW_EX_IGNORESETTINGS   0x00040000  // ignore system settings to turn on/off smooth scroll

#define SSI_DEFAULT ((UINT)-1)

#define SSIF_SCROLLPROC    0x0001
#define SSIF_MAXSCROLLTIME 0x0002
#define SSIF_MINSCROLL     0x0004


//
// Bitfields don't get along too well with bools,
// so here's an easy way to convert them:
//
#define BOOLIFY(expr)           (!!(expr))

// COMPILETIME_ASSERT(f)
//
//  Generates a build break at compile time if the constant expression
//  is not true.  Unlike the "#if" compile-time directive, the expression
//  in COMPILETIME_ASSERT() is allowed to use "sizeof".
//
//  Compiler magic!  If the expression "f" is FALSE, then you get the
//  compiler error "Duplicate case expression in switch statement".
//
#define COMPILETIME_ASSERT(f) switch (0) case 0: case f:

#ifdef _WIN64
#define HIWORD64(p)     ((ULONG_PTR)(p) >> 16)
#define BOOLFROMPTR(p)  ((p) != 0)
#define SPRINTF_PTR		"%016I64x"
#else
#define HIWORD64        HIWORD
#define BOOLFROMPTR(p)  ((BOOL)(p))
#define SPRINTF_PTR		"%08x"
#endif

/*
* Converts an ANSI string to UNICODE
*/
#define ConvertAToWN( uiCodePage, pszWBuf, cchW, pszA, cchA )         \
	MultiByteToWideChar( uiCodePage, MB_PRECOMPOSED, pszA, cchA, pszWBuf, cchW )

/*
 * FreeProducedString
 *
 * Takes a pointer returned from Produce?From?() and frees it.  No
 * validity checking is needed before calling this function.  (ie, any
 * value returned by Produce?From?() can be safely sent to this function)
 */
#define FreeProducedString( psz )   \
	if((psz) != NULL && ((LPSTR)psz) != LPSTR_TEXTCALLBACKA) {LocalFree(psz);} else

#define HDFT_ISMASK         0x000f
#define TTF_MEMALLOCED          0x0200
#define TTF_UNICODE             0x0040       // Unicode Notify's       ;Internal
#define TTF_RIGHT               0x0800       // right-aligned tooltips text (multi-line tooltips) ;Internal
#define TTF_USEHITTEST          0x0400
#define CH_PREFIX TEXT('&')

/*
* IsFlagPtr
*  Returns TRUE if the pointer == NULL or -1
*/
#define IsFlagPtr( p )  ((p) == NULL || (LPSTR)(p) == LPSTR_TEXTCALLBACKA)

#define LVN_GETEMPTYTEXTA          (LVN_FIRST-60)
#define LVN_GETEMPTYTEXTW          (LVN_FIRST-61)

#ifdef UNICODE
#define LVN_GETEMPTYTEXT           LVN_GETEMPTYTEXTW
#else
#define LVN_GETEMPTYTEXT           LVN_GETEMPTYTEXTA
#endif

#define ConvertWToAN( uiCodePage, pszABuf, cchA, pszW, cchW )         \
	WideCharToMultiByte(uiCodePage, 0, pszW, cchW, pszABuf, cchA, NULL, NULL)

#define CDRF_VALIDFLAGS         0xFFFF00F6
typedef void (CALLBACK *NOTIFYWINEVENTPROC)(UINT, HWND, LONG, LONG_PTR);
#define DONOTHING_NOTIFYWINEVENT ((NOTIFYWINEVENTPROC)1)
#define DT_SEARCHTIMEOUT    1000L       // 1 seconds
#define g_dwPrototype   0
#define PTF_NOISEARCHTO 0x00000002  // No incremental search timeout
#define IsFlagSet(obj, f)           (BOOL)(((obj) & (f)) == (f))


extern BOOL fHaveNLSProcs;
extern BOOL fHaveIMMProcs;
extern BOOL fHaveIMEShareProcs;

//----------------------------------------------------------------------------
// Define a Global Shared Heap that we use allocate memory out of that we
// Need to share between multiple instances.
#ifndef WINNT
#define GROWABLE        0
#define MAXHEAPSIZE     GROWABLE
#define HEAP_SHARED	0x04000000		/* put heap in shared memory */
#endif

#define MAKELRESULTFROMUINT(i)  ((LRESULT)i)
#define STATEIMAGEMASKTOINDEX(i) ((i & LVIS_STATEIMAGEMASK) >> 12)

#define IDT_NAMEEDIT    42
#define CCResetInfoTipWidth(hwndOwner, hwndToolTips) \
	SendMessage(hwndToolTips, TTM_SETMAXTIPWIDTH, 0, -1)
#define SetWindowID(hwnd,id)    SetWindowLongPtr(hwnd, GWLP_ID, id)
#define FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, fn) \
	(void)(fn)((hwnd), WM_COMMAND, MAKEWPARAM((UINT)(id),(UINT)(codeNotify)), (LPARAM)(HWND)(hwndCtl))

#define TVM_SETBORDER         (TV_FIRST + 35)
#define TVM_GETBORDER         (TV_FIRST + 36)
#define TVSBF_YBORDER   0x00000002
#define TVSBF_XBORDER   0x00000001
#define CCM_TRANSLATEACCELERATOR (CCM_FIRST + 0xa)  // lParam == lpMsg
#define TVM_TRANSLATEACCELERATOR    CCM_TRANSLATEACCELERATOR
#define HANDLE_WM_CHAR(hwnd, wParam, lParam, fn) \
	((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define IDC_HAND_INTERNAL       108
#define GET_WM_HSCROLL_CODE(wp, lp)                 LOWORD(wp)
#define GET_WM_HSCROLL_POS(wp, lp)                  HIWORD(wp)
#define GET_WM_HSCROLL_HWND(wp, lp)                 (HWND)(lp)
#define GET_WM_HSCROLL_MPS(code, pos, hwnd)    \
	(WPARAM)MAKELONG(code, pos), (LPARAM)(hwnd)

#define GET_WM_VSCROLL_CODE(wp, lp)                 LOWORD(wp)
#define GET_WM_VSCROLL_POS(wp, lp)                  HIWORD(wp)
#define GET_WM_VSCROLL_HWND(wp, lp)                 (HWND)(lp)
#define GET_WM_VSCROLL_MPS(code, pos, hwnd)    \
	(WPARAM)MAKELONG(code, pos), (LPARAM)(hwnd)

#define GET_WM_COMMAND_ID(wp, lp)               LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp)             (HWND)(lp)
#define GET_WM_COMMAND_CMD(wp, lp)              HIWORD(wp)
#define GET_WM_COMMAND_MPS(id, hwnd, cmd)    \
	(WPARAM)MAKELONG(id, cmd), (LPARAM)(hwnd)

#define MSAA_CLASSNAMEIDX_BASE 65536L
#define MSAA_CLASSNAMEIDX_TREEVIEW   (MSAA_CLASSNAMEIDX_BASE+25)
#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))

#define GetWindowInt        GetWindowLongPtr
#define SetWindowInt        SetWindowLongPtr
#define SetWindowID(hwnd,id)    SetWindowLongPtr(hwnd, GWLP_ID, id)
#define GetClassCursor(hwnd)    ((HCURSOR)GetClassLongPtr(hwnd, GCLP_HCURSOR))
#define GetClassIcon(hwnd)      ((HICON)GetClassLongPtr(hwnd, GCLP_HICON))
#define BOOL_PTR                INT_PTR

#define DT_LV       (DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_EDITCONTROL)
#define DT_LVWRAP   (DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL)
#define CCHLABELMAX MAX_PATH  // BUGBUG dangerous???
#define ISERACALENDAR(pct)            ((pct)->hdpaEras)
#define DP_InRange(id, idFirst, idLast)      ((UINT)((id)-(idFirst)) <= (UINT)((idLast)-(idFirst)))
#define SE_YEARLIKE(s)      ((s) == SE_YEAR || (s) == SE_YEARALT)
#define SE_DATELIKE(s)      DP_InRange(s, SE_ERA, SE_DATELAST)
#define GregorianToOther(pct, yr)     ((yr) + (pct)->dyrOffset)
#define OtherToGregorian(pct, yr)     ((yr) - (pct)->dyrOffset)
#define SECYBORDER 2
#define SECXBORDER 2

#ifndef SPF_SETDEFAULT
#define SPF_SETDEFAULT		4
#endif

#define UDS_UNSIGNED            0x0200          // ;internal

#define SelectFont(hdc, hfont)  ((HFONT)SelectObject((hdc), (HGDIOBJ)(HFONT)(hfont)))
#define Edit_LimitText(hwndCtl, cchMax)         ((void)SNDMSG((hwndCtl), EM_LIMITTEXT, (WPARAM)(cchMax), 0L))
#define Edit_SetSel(hwndCtl, ichStart, ichEnd)  ((void)SNDMSG((hwndCtl), EM_SETSEL, (ichStart), (ichEnd)))
#define FORWARD_WM_SETFONT(hwnd, hfont, fRedraw, fn) \
	(void)(fn)((hwnd), WM_SETFONT, (WPARAM)(HFONT)(hfont), (LPARAM)(BOOL)(fRedraw))
#define     SubclassWindow(hwnd, lpfn)       \
	((WNDPROC)SetWindowLongPtr((hwnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(lpfn)))

#define     GetWindowID(hwnd)            GetDlgCtrlID(hwnd)
#define SSW_EX_IMMEDIATE        0x00020000

#ifdef NONAMELESSUNION
#define DUMMYUNION_MEMBER(member)   DUMMYUNIONNAME.member
#define DUMMYUNION2_MEMBER(member)  DUMMYUNIONNAME2.member
#define DUMMYUNION3_MEMBER(member)  DUMMYUNIONNAME3.member
#define DUMMYUNION4_MEMBER(member)  DUMMYUNIONNAME4.member
#define DUMMYUNION5_MEMBER(member)  DUMMYUNIONNAME5.member
#else
#define DUMMYUNION_MEMBER(member)    member
#define DUMMYUNION2_MEMBER(member)   member
#define DUMMYUNION3_MEMBER(member)   member
#define DUMMYUNION4_MEMBER(member)   member
#define DUMMYUNION5_MEMBER(member)   member
#endif

#define TVIF_WIN95              0x007F
#define TVIF_ALL                0x00FF

#define NearAlloc(cb)       ((void *)LocalAlloc(LPTR, (cb)))
#define NearReAlloc(pb, cb) ((void *)LocalReAlloc((HLOCAL)(pb), (cb), LMEM_MOVEABLE | LMEM_ZEROINIT))
#define NearFree(pb)        (LocalFree((HLOCAL)(pb)) ? FALSE : TRUE)
#define NearSize(pb)        LocalSize(pb)

enum
{
	iImmGetCompositionStringA = 0,
	iImmGetContext = 1,
	iImmSetCompositionFontA = 2,
	iImmSetCompositionWindow = 3,
	iImmReleaseContext = 4,
	iImmGetProperty = 5,
	iImmGetCandidateWindow = 6,
	iImmSetCandidateWindow = 7,
	iImmNotifyIME = 8,
	iImmAssociateContext = 9,
	iImmGetVirtualKey = 10,
	iImmSetOpenStatus = 11,
	iImmGetConversionStatus = 12,
	iImmEscape = 13,
	iGetKeyboardLayout = 14,
	iGetKeyboardLayoutList = 15,
	iTranslateCharsetInfo = 16,
	iFSupportSty = 17,
	iPIMEStyleFromAttr = 18,
	iPColorStyleTextFromIMEStyle = 19,
	iPColorStyleBackFromIMEStyle = 20,
	iFBoldIMEStyle = 21,
	iFItalicIMEStyle = 22,
	iFUlIMEStyle = 23,
	iIdUlIMEStyle = 24,
	iRGBFromIMEColorStyle = 25
};

extern const INT MAX_PROCS;
FARPROC nlsProcTable[];
typedef struct  									// loop support.
{
	char	*libName;
	char	**procNameList;
	BOOL	*fLoaded;								// Flag all clients check!!
	SHORT	iProcStart;
} LibsAndProcs;

//////////////////////////////////////////////////////////////////////////
// mem.h

#define ControlAlloc(hheap, cb)       HeapAlloc((hheap), HEAP_ZERO_MEMORY, (cb))
#define ControlReAlloc(hheap, pb, cb) HeapReAlloc((hheap), HEAP_ZERO_MEMORY, (pb),(cb))
#define ControlFree(hheap, pb)        HeapFree((hheap), 0, (pb))
#define ControlSize(hheap, pb)        HeapSize((hheap), 0, (LPCVOID)(pb))


//////////////////////////////////////////////////////////////////////////

#ifndef WINNT
HANDLE InitSharedHeap(void);
#endif
void *WINAPI Alloc(long cb);
void *WINAPI ReAlloc(void *pb, long cb);
BOOL SameChars(LPTSTR lpsz, TCHAR c);
//
//
//  WindowLong accessor macros and other Win64 niceness
//

LRESULT PASCAL CIHandleNotifyFormat(LibUIDK_LPCONTROLINFO lpci, LPARAM lParam);
void RescrollEditWindow(HWND hwndEdit);
HIMAGELIST CreateCheckBoxImagelist(HIMAGELIST himl, BOOL fTree, BOOL fUseColorKey, BOOL fMirror);
void PASCAL DrawInsertMark(HDC hdc, LPRECT prc, BOOL fHorizMode, COLORREF clr);

//////////////////////////////////////////////////////////////////////////
// ctlspriv.h



// Macro
/*
 * Window styles, from 0x0E00 to 0x0F80.
 */
#define WFMAXBOX                0x0E01  // WS_MAXIMIZEBOX
#define WFTABSTOP               0x0E01  // WS_TABSTOP
#define WFMINBOX                0x0E02  // WS_MAXIMIZEBOX
#define WFGROUP                 0x0E02  // WS_GROUP
#define WFSIZEBOX               0x0E04  // WS_THICKFRAME, WS_SIZEBOX
#define WFSYSMENU               0x0E08  // WS_SYSMENU
#define WFHSCROLL               0x0E10  // WS_HSCROLL
#define WFVSCROLL               0x0E20  // WS_VSCROLL
#define WFDLGFRAME              0x0E40  // WS_DLGFRAME
#define WFTOPLEVEL              0x0E40  // ???
#define WFBORDER                0x0E80  // WS_BORDER
#define WFBORDERMASK            0x0EC0  // WS_BORDER | WS_DLGFRAME
#define WFCAPTION               0x0EC0  // WS_CAPTION

#define OFFSETOF(x)     ((UINT)(x))
#define BOOL_PTR                INT_PTR
#define RECOMPUTE  (DWORD)MAXINT
#define SRECOMPUTE ((short)0x7FFF)

#define LVN_GETEMPTYTEXTA          (LVN_FIRST-60)
#define LVN_GETEMPTYTEXTW          (LVN_FIRST-61)

#ifdef UNICODE
#define LVN_GETEMPTYTEXT           LVN_GETEMPTYTEXTW
#else
#define LVN_GETEMPTYTEXT           LVN_GETEMPTYTEXTA
#endif

#define LVM_GETHOTLIGHTCOLOR    (LVM_FIRST + 79)
#define LVM_SETHOTLIGHTCOLOR    (LVM_FIRST + 80)
#define LVM_SETLVRANGEOBJECT    (LVM_FIRST + 82)
#define LVM_RESETEMPTYTEXT      (LVM_FIRST + 84)

#define MSAA_CLASSNAMEIDX_BASE 65536L
#define MSAA_CLASSNAMEIDX_LISTVIEW   (MSAA_CLASSNAMEIDX_BASE+19)

#define hmemcpy     MoveMemory
#define LVIS_ALL            0xFFFF  /* ;Internal */

#ifdef USE_MIRRORING
#define RTL_MIRRORED_WINDOW             dwExStyleRTLMirrorWnd
#define IS_WINDOW_RTL_MIRRORED(hwnd)    (g_bMirroredOS && Mirror_IsWindowMirroredRTL(hwnd))
#else
#define RTL_MIRRORED_WINDOW             0L
#define IS_WINDOW_RTL_MIRRORED(hwnd)    FALSE
#endif

#define GetWindowID(hwnd) GetDlgCtrlID(hwnd)
#define SetWindowID(hwnd,id)    SetWindowLongPtr(hwnd, GWLP_ID, id)

#define CCResetInfoTipWidth(hwndOwner, hwndToolTips) \
	SendMessage(hwndToolTips, TTM_SETMAXTIPWIDTH, 0, -1)

#define NearAlloc(cb)       ((void *)LocalAlloc(LPTR, (cb)))

#define IncrementSearchFree(pis) ((pis)->pszCharBuf ? HeapFree(GetSharedHeapHandle(), 0, (pis)->pszCharBuf) : 0)
#define NearFree(pb)        (LocalFree((HLOCAL)(pb)) ? FALSE : TRUE)

#define GCS_RESULTSTR                   0x0800

#define IDSYS_SCROLL 0x0000FFFEL

/*
 * FreeProducedString
 *
 * Takes a pointer returned from Produce?From?() and frees it.  No
 * validity checking is needed before calling this function.  (ie, any
 * value returned by Produce?From?() can be safely sent to this function)
 */
#define FreeProducedString( psz )   \
	if((psz) != NULL && ((LPSTR)psz) != LPSTR_TEXTCALLBACKA) {LocalFree(psz);} else

#ifdef WIN32
typedef MSG MSG32;
typedef MSG32 *LPMSG32;
#define GetMessage32(lpmsg, hwnd, min, max, f32)        GetMessage(lpmsg, hwnd, min, max)
#define PeekMessage32(lpmsg, hwnd, min, max, flags, f32)       PeekMessage(lpmsg, hwnd, min, max, flags)
#define TranslateMessage32(lpmsg, f32)  TranslateMessage(lpmsg)
#define DispatchMessage32(lpmsg, f32)   DispatchMessage(lpmsg)
#define CallMsgFilter32(lpmsg, u, f32)  CallMsgFilter(lpmsg, u)
#define IsDialogMessage32(hwnd, lpmsg, f32)   IsDialogMessage(hwnd, lpmsg)
#endif

#define STATEIMAGEMASKTOINDEX(i) ((i & LVIS_STATEIMAGEMASK) >> 12)
#define CCHELLIPSES 3

#define TF_BKIMAGE 0x00000200


#define ConvertWToAN( uiCodePage, pszABuf, cchA, pszW, cchW )         \
	WideCharToMultiByte(uiCodePage, 0, pszW, cchW, pszABuf, cchA, NULL, NULL)

#define EVAL(exp) \
	((exp) != 0)

BOOL CcshellRipA(LPCSTR pszFile, int line, LPCSTR pszEval, BOOL bBreakInside);
BOOL CcshellRipW(LPCWSTR pszFile, int line, LPCWSTR pwszEval, BOOL bBreakInside);
BOOL CDECL CcshellRipMsgA(BOOL bRip, LPCSTR pszMsg, ...);
void IUIAPI CDECL CcshellDebugMsgW(DWORD mask, LPCSTR pszMsg, ...);
void CDECL CcshellDebugMsgA(DWORD mask, LPCSTR pszMsg, ...);

#ifdef UNICODE
#define CcshellRip      CcshellRipW
#define CcshellRipMsg   CcshellRipMsgW
#else
#define CcshellRip      CcshellRipA
#define CcshellRipMsg   CcshellRipMsgA
#endif

#ifdef UNICODE
#define CcshellDebugMsg         CcshellDebugMsgW
#define _DebugMsg
#else
#define CcshellDebugMsg         CcshellDebugMsgA
#define _DebugMsg
#endif

#ifdef _DEBUG
#define RIPMSG

#define TraceMsgW           CcshellDebugMsgW
#define TraceMsgA           CcshellDebugMsgA
#define TraceMsg

#else  // DEBUG
#define RIPMSG          1 ? (void)0 : (void)
#define TraceMsg
#endif

#define ControlAlloc(hheap, cb)       HeapAlloc((hheap), HEAP_ZERO_MEMORY, (cb))
#define ControlReAlloc(hheap, pb, cb) HeapReAlloc((hheap), HEAP_ZERO_MEMORY, (pb),(cb))
#define ControlFree(hheap, pb)        HeapFree((hheap), 0, (pb))
#define ControlSize(hheap, pb)        HeapSize((hheap), 0, (LPCVOID)(pb))

#define TF_ERROR 0x00000002
#define DM_ERROR            TF_ERROR        // OBSOLETE Error

#define SSI_DEFAULT ((UINT)-1)

#define SSIF_SCROLLPROC    0x0001
#define SSIF_MAXSCROLLTIME 0x0002
#define SSIF_MINSCROLL     0x0004

#define TF_LISTVIEW 0x00002000

#define DebugMsg

#define LVIR_MAX 4
#define SHDT_EXTRAMARGIN 0x0040

#define SHDT_DRAWTEXT       0x0001
#define SHDT_ELLIPSES       0x0002
#define SHDT_CLIPPED        0x0004
#define SHDT_SELECTED       0x0008
#define SHDT_DESELECTED     0x0010
#define SHDT_DEPRESSED      0x0020
#define SHDT_EXTRAMARGIN    0x0040
#define SHDT_TRANSPARENT    0x0080
#define SHDT_SELECTNOFOCUS  0x0100
#define SHDT_HOTSELECTED    0x0200
#define SHDT_DTELLIPSIS     0x0400
#ifdef WINDOWS_ME
#define SHDT_RTLREADING     0x0800
#endif
#define SHDT_NODBCSBREAK    0x1000

#define ILD_MIRROR              0x0080
#define SELECTOROF(x)   ((WPARAM)(x))

// Trace flags for g_dwTraceFlags
#define TF_ALWAYS           0xFFFFFFFF
#define TF_NEVER            0x00000000
#define TF_WARNING          0x00000001
#define TF_ERROR            0x00000002
#define TF_GENERAL          0x00000004      // Standard messages
#define TF_FUNC             0x00000008      // Trace function calls
#define TF_ATL              0x00000008      // Since TF_FUNC is so-little used, I'm overloading this bit

// COMPILETIME_ASSERT(f)
//
//  Generates a build break at compile time if the constant expression
//  is not true.  Unlike the "#if" compile-time directive, the expression
//  in COMPILETIME_ASSERT() is allowed to use "sizeof".
//
//  Compiler magic!  If the expression "f" is FALSE, then you get the
//  compiler error "Duplicate case expression in switch statement".
//
#define COMPILETIME_ASSERT(f) switch (0) case 0: case f:

/*
* IsFlagPtr
*  Returns TRUE if the pointer == NULL or -1
*/
#define IsFlagPtr( p )  ((p) == NULL || (LPSTR)(p) == LPSTR_TEXTCALLBACKA)


__inline void *GetWindowPtr(HWND hWnd, int nIndex)
{
	return (void *)GetWindowLongPtr(hWnd, nIndex);
}
__inline void *SetWindowPtr(HWND hWnd, int nIndex, void *p)
{
	return (void *)SetWindowLongPtr(hWnd, nIndex, (LONG_PTR)p);
}

// Large icon.
#define LVS_ALIGNTOP        0x0000
#define LVS_ALIGNBOTTOM     0x0400     /* ;Internal */
#define LVS_ALIGNLEFT       0x0800
#define LVS_ALIGNRIGHT      0x0c00     /* ;Internal */
#define LVS_ALIGNMASK       0x0c00

#define LVA_SORTASCENDING   0x0100  // ;Internal
#define LVA_SORTDESCENDING  0x0200  // ;Internal

#define SSW_EX_NOTIMELIMIT      0x00010000
#define SSW_EX_IMMEDIATE        0x00020000
#define SSW_EX_IGNORESETTINGS   0x00040000  // ignore system settings to turn on/off smooth scroll

// parameter of ImmGetCompositionString
#define GCS_COMPREADSTR                 0x0001
#define GCS_COMPREADATTR                0x0002
#define GCS_COMPREADCLAUSE              0x0004
#define GCS_COMPSTR                     0x0008
#define GCS_COMPATTR                    0x0010
#define GCS_COMPCLAUSE                  0x0020
#define GCS_CURSORPOS                   0x0080
#define GCS_DELTASTART                  0x0100
#define GCS_RESULTREADSTR               0x0200
#define GCS_RESULTREADCLAUSE            0x0400
#define GCS_RESULTSTR                   0x0800
#define GCS_RESULTCLAUSE                0x1000

// lParam for WM_IME_SETCONTEXT
#define ISC_SHOWUICANDIDATEWINDOW       0x00000001
#define ISC_SHOWUICOMPOSITIONWINDOW     0x80000000
#define ISC_SHOWUIGUIDELINE             0x40000000
#define ISC_SHOWUIALLCANDIDATEWINDOW    0x0000000F
#define ISC_SHOWUIALL                   0xC000000F

// lvicon.c in-place editing
#define SEIPS_WRAP          0x0001
#ifdef DEBUG
#define SEIPS_NOSCROLL      0x0002      // Flag is used only in DEBUG
#endif


#define Edit_GetSel(hwndCtl)                    ((DWORD)SNDMSG((hwndCtl), EM_GETSEL, 0L, 0L))
#define Edit_SetSel(hwndCtl, ichStart, ichEnd)  ((void)SNDMSG((hwndCtl), EM_SETSEL, (ichStart), (ichEnd)))

#if (_MSC_VER == 1200)	// VC6.0
#ifdef Edit_GetText
#undef Edit_GetText
#define Edit_GetText(hwndCtl, lpch, cchMax)     ::GetWindowText((hwndCtl), (lpch), (cchMax))
#endif // Edit_GetText
#endif // #endif // #if (_MSC_VER == 1200)	// VC6.0

#define Edit_GetTextLength(hwndCtl)             GetWindowTextLength(hwndCtl)
#define Edit_SetText(hwndCtl, lpsz)             SetWindowText((hwndCtl), (lpsz))

#define Edit_LimitText(hwndCtl, cchMax)         ((void)SNDMSG((hwndCtl), EM_LIMITTEXT, (WPARAM)(cchMax), 0L))

#define FORWARD_WM_SETFONT(hwnd, hfont, fRedraw, fn) \
	(void)(fn)((hwnd), WM_SETFONT, (WPARAM)(HFONT)(hfont), (LPARAM)(BOOL)(fRedraw))


#define     SubclassWindow(hwnd, lpfn)       \
	((WNDPROC)SetWindowLongPtr((hwnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(lpfn)))

#define     SelectFont(hdc, hfont)  ((HFONT)SelectObject((hdc), (HGDIOBJ)(HFONT)(hfont)))
#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))
#define Edit_ReplaceSel(hwndCtl, lpszReplace)   ((void)SNDMSG((hwndCtl), EM_REPLACESEL, 0L, (LPARAM)(LPCTSTR)(lpszReplace)))

#define Edit_LineFromChar(hwndCtl, ich)         ((int)(DWORD)SNDMSG((hwndCtl), EM_LINEFROMCHAR, (WPARAM)(int)(ich), 0L))
#define Edit_LineIndex(hwndCtl, line)           ((int)(DWORD)SNDMSG((hwndCtl), EM_LINEINDEX, (WPARAM)(int)(line), 0L))
#define Edit_LineLength(hwndCtl, line)          ((int)(DWORD)SNDMSG((hwndCtl), EM_LINELENGTH, (WPARAM)(int)(line), 0L))

typedef void (CALLBACK *NOTIFYWINEVENTPROC)(UINT, HWND, LONG, LONG_PTR);
#define DONOTHING_NOTIFYWINEVENT ((NOTIFYWINEVENTPROC)1)

#define CbFromCchW(cch)             ((cch)*sizeof(WCHAR))
#define CbFromCchA(cch)             ((cch)*sizeof(CHAR))

#define SIZECHARS(sz)               (sizeof(sz)/sizeof(sz[0]))
#define IDC_HAND_INTERNAL       108

//----------------------------------------------------------------------------
// Define a Global Shared Heap that we use allocate memory out of that we
// Need to share between multiple instances.
#ifndef WINNT
#define GROWABLE        0
#define MAXHEAPSIZE     GROWABLE
#define HEAP_SHARED	0x04000000		/* put heap in shared memory */
#endif

#define CDRF_VALIDFLAGS         0xFFFF00F6

/*
 * The following masks can be used along with the wDisableFlags field of SB
 * to find if the Up/Left or Down/Right arrow or Both are disabled;
 * Now it is possible to selectively Enable/Disable just one or both the
 * arrows in a scroll bar control;
 */
#define LTUPFLAG    0x0001  // Left/Up arrow disable flag.
#define RTDNFLAG    0x0002  // Right/Down arrow disable flag.

#define GRECT_CLIENT        0x0001
#define GRECT_WINDOW        0x0002
#define GRECT_RECTMASK      0x0003

#define GRECT_CLIENTCOORDS  0x0010
#define GRECT_WINDOWCOORDS  0x0020
#define GRECT_PARENTCOORDS  0x0040
#define GRECT_COORDMASK     0x0070

#define SIF_RANGE           0x0001                          // ;Internal 4.0
#define SIF_PAGE            0x0002                          // ;Internal 4.0
#define SIF_POS             0x0004                          // ;Internal 4.0
#define SIF_DISABLENOSCROLL 0x0008                          // ;Internal 4.0
#define SIF_TRACKPOS        0x0010                          // ;Internal 4.0
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)// ;Internal
#define SIF_RETURNOLDPOS    0x1000                          // ;Internal
#define SIF_NOSCROLL        0x2000                          // ;Internal
#define SIF_MASK            0x701F                          // ;Internal

/*
 * Scrollbar initialization types
 */
#define SCROLL_NORMAL   0
#define SCROLL_DIRECT   1
#define SCROLL_MENU     2


#define DT_SEARCHTIMEOUT    1000L       // 1 seconds
#define g_dwPrototype   0
#define PTF_NOISEARCHTO 0x00000002  // No incremental search timeout
#define IsFlagSet(obj, f)           (BOOL)(((obj) & (f)) == (f))
__inline BOOL IsISearchTimedOut(PISEARCHINFO pis)
{
	return GetMessageTime() - pis->timeLast > DT_SEARCHTIMEOUT &&
		!IsFlagSet(g_dwPrototype, PTF_NOISEARCHTO);

}

typedef struct tagNMTTSHOWINFO
{
	NMHDR hdr;
	DWORD dwStyle;
} NMTTSHOWINFO, *LPNMTTSHOWINFO;

typedef int (CALLBACK *PFNSMOOTHSCROLLPROC)(SListViewData *plv,
	int dx,
	int dy,
	CONST RECT *prcScroll,
	CONST RECT *prcClip,
	HRGN hrgnUpdate,
	LPRECT prcUpdate,
	UINT flags);

typedef struct tagSSWInfo
{
	UINT cbSize;
	DWORD fMask;
	HWLWND hWLWnd;
	int dx;
	int dy;
	LPCRECT lprcSrc;
	LPCRECT lprcClip;
	HRGN hrgnUpdate;
	LPRECT lprcUpdate;
	UINT fuScroll;

	UINT uMaxScrollTime;
	UINT cxMinScroll;
	UINT cyMinScroll;

	PFNSMOOTHSCROLLPROC pfnScrollProc;  // we'll call this back instead
} SMOOTHSCROLLINFO, *PSMOOTHSCROLLINFO;


typedef struct tagTHUNKSTATE
{
	LPVOID ts_pvThunk1;
	LPVOID ts_pvThunk2;
	DWORD ts_dwThunkSize;
} THUNKSTATE;

// 滚动条私有变量
typedef struct tagSBWND
{
	tagSBWND()
		: hwnd(NULL)
		, m_nMinThumbLength(8)
		, m_himgCombineButton1Bk(NULL)
		, m_himgCombineButton2Bk(NULL)
		, m_himgCombineThumbBk(NULL)
		, wDisableFlags(0)
		, m_uButton1Length(GetSystemMetrics(SM_CXHSCROLL))
		, m_uButton2Length(GetSystemMetrics(SM_CXHSCROLL))
		, m_crScrollButtonBkN(GetScrollButtonBkColorN())
		, m_crScrollButtonBkH(GetScrollButtonBkColorH())
		, m_crScrollButtonBkP(GetScrollButtonBkColorP())
		, m_crScrollButtonBkD(GetScrollButtonBkColorD())
		, m_crScrollGroove(GetScrollGrooveColor())
		, m_crScrollThumbBkN(GetScrollThumbBkColorN())
		, m_crScrollThumbBkH(GetScrollThumbBkColorH())
		, m_crScrollThumbBkP(GetScrollThumbBkColorP())
		, m_crScrollThumbBkD(GetScrollThumbBkColorD())
		, m_bRecordLButtonDown(FALSE)
		, m_bScrollBarRefreshSelf(FALSE)
	{
		SBCalc.nTrackPos = 0;

		for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			m_himgGroove[i] = NULL;
			m_himgButton1Bk[i] = NULL;
			m_himgButton2Bk[i] = NULL;
			m_himgThumbBk[i] = NULL;
		}
	}

	int Release();

	HWLWND hwnd;

	UINT fVert;
	UINT wDisableFlags;	// 滚动条的按钮、滑块等元素是否是Disable状态
	SBCALC SBCalc;

	UINT m_nMinThumbLength;

	// 滚动条轨道
	COLORREF m_crScrollGroove;
	HIUIIMAGE m_himgGroove[COMBINEIMAGESIZE4];
	// 滚动条按钮
	UINT m_uButton1Length;
	HIUIIMAGE m_himgCombineButton1Bk;
	HIUIIMAGE m_himgButton1Bk[COMBINEIMAGESIZE4];
	UINT m_uButton2Length;
	HIUIIMAGE m_himgCombineButton2Bk;
	HIUIIMAGE m_himgButton2Bk[COMBINEIMAGESIZE4];
	COLORREF m_crScrollButtonBkN;
	COLORREF m_crScrollButtonBkH;
	COLORREF m_crScrollButtonBkP;
	COLORREF m_crScrollButtonBkD;
	// 滚动条滑块
	COLORREF m_crScrollThumbBkN;
	COLORREF m_crScrollThumbBkH;
	COLORREF m_crScrollThumbBkP;
	COLORREF m_crScrollThumbBkD;
	HIUIIMAGE m_himgCombineThumbBk;
	HIUIIMAGE m_himgThumbBk[COMBINEIMAGESIZE4];
	CIUIRect m_rcThumbImageResize;

	// Helper
	BOOL m_bRecordLButtonDown; // 记录鼠标是否在本控件内按下过
	BOOL m_bScrollBarRefreshSelf;	// 滚动条在发送WM_XSCROLL消息后，自己刷新自己的滑块位置
} SBWND, *PSBWND, *LPSBWND;

struct ITEM_BACKGROUND
{
	ITEM_BACKGROUND()
	{
		bItemBkEnabled = FALSE;
		eItemBackgroundType = BKT_COLOR;
		eItemBkImageResizeMode = IRM_9GRID;
		for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			crItemBk[i] = RGB(0, 0, 0);
			himgItemBk[i] = NULL;
			crText[i] = RGB(0, 0, 0);
		}
	}

	BOOL bItemBkEnabled;
	BACKGROUND_TYPE eItemBackgroundType;
	COLORREF crItemBk[COMBINEIMAGESIZE4];
	HIUIIMAGE himgItemBk[COMBINEIMAGESIZE4];
	IMAGE_RESIZE_MODE eItemBkImageResizeMode;
	CIUIRect rcItemBkImage9GridArg;
	COLORREF crText[COMBINEIMAGESIZE4];
};

namespace IUI
{
#define CFETC	15						// Dimension of g_rgFETC[]

#if 1
#define TRACEBEGIN(A, B, C)
#else
#define TRCSUBSYSDTE	0xa
#define TRCSCOPEINTERN  0x2
	inline void TRACEBEGIN(int, int, const char *pszInfo)
	{
		// 如果c盘根下有文件IUI.NOLOG文件，则不打印调试信息
		if (PathFileExists(_T("C:\\IUI.NOLOG")))
		{
			return;
		}

		static int i = 0;
		i++;
		if (i > MAXINT)
		{
			i = 0;
		}

		char szBuf[MAX_PATH] = {0};
		sprintf(szBuf, "%d %s\r\n", i, pszInfo);
		OutputDebugStringA(szBuf);
	}
#endif

	enum FETCINDEX							// Keep in sync with g_rgFETC[]
	{
		iRtfFETC,							// RTF
		iEmbObj,							// Embedded Object
		iEmbSrc,							// Embed Source
		iObtDesc,							// Object Descriptor
		iLnkSrc,							// Link Source
		iMfPict,							// Metafile
		iDIB,								// DIB
		iBitmap,							// Bitmap
		iRtfNoObjs,							// RTF with no objects
		iUnicodeFETC,						// Unicode plain text
		iAnsiFETC,							// ANSI plain text
		iFilename,							// Filename
		iRtfAsTextFETC,						// Pastes RTF as text
		iTxtObj,						// Richedit Text
		iRichEdit							// RichEdit Text w/formatting
	};

	enum
	{
		SE_ERA = 1,
		SE_YEAR,
		SE_YEARALT,         // see SEGetTimeDateFormat
		SE_MONTH,
		SE_MONTHALT,        // see SEGetTimeDateFormat
		SE_DAY,
		SE_DATELAST = SE_DAY,
		SE_MARK,            // "AM" or "PM" indicator
		SE_HOUR,
		SE_MINUTE,
		SE_SECOND,
		SE_STATIC,
		SE_APP,
		SE_MAX
	};


	extern FORMATETC g_rgFETC[];

#define cf_RICHEDIT			g_rgFETC[iRichEdit].cfFormat
#define cf_EMBEDDEDOBJECT	g_rgFETC[iEmbObj].cfFormat
#define cf_EMBEDSOURCE		g_rgFETC[iEmbSrc].cfFormat
#define cf_OBJECTDESCRIPTOR	g_rgFETC[iObtDesc].cfFormat
#define cf_LINKSOURCE		g_rgFETC[iLnkSrc].cfFormat
#define cf_RTF				g_rgFETC[iRtfFETC].cfFormat
#define cf_RTFNOOBJS		g_rgFETC[iRtfNoObjs].cfFormat
#define cf_TEXTOBJECT		g_rgFETC[iTxtObj].cfFormat
#define cf_RTFASTEXT		g_rgFETC[iRtfAsTextFETC].cfFormat
#define cf_FILENAME			g_rgFETC[iFilename].cfFormat

#define CF_EMBEDSOURCE			"Embed Source"
#define CF_EMBEDDEDOBJECT		"Embedded Object"
#define CF_LINKSOURCE			"Link Source"
#define CF_CUSTOMLINKSOURCE		"Custom Link Source"
#define CF_OBJECTDESCRIPTOR		"Object Descriptor"
#define CF_LINKSRCDESCRIPTOR	"Link Source Descriptor"
#define CF_OWNERLINK			"OwnerLink"
#define CF_FILENAME				"FileName"

	// 用于IM控件。
	// IM控件自己发出DoDragDrop
	// 以保证任何情况，拖动的内容都不会被删除。
	class CProtectedDataObject : public IDataObject
	{
	public:

		// IUnknown methods
		STDMETHOD(QueryInterface)(REFIID riid, void **ppvObj);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IDataObject methods
		STDMETHOD(DAdvise)(FORMATETC *pFormatetc, DWORD advf,
			IAdviseSink *pAdvSink, DWORD *pdwConnection);
		STDMETHOD(DUnadvise)(DWORD dwConnection);
		STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise);
		STDMETHOD(EnumFormatEtc)(DWORD dwDirection,
			IEnumFORMATETC **ppenumFormatEtc);
		STDMETHOD(GetCanonicalFormatEtc)(FORMATETC *pformatetc,
			FORMATETC *pformatetcOut);
		STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
		STDMETHOD(GetDataHere)(FORMATETC *pformatetc, STGMEDIUM *pmedium);
		STDMETHOD(QueryGetData)(FORMATETC *pformatetc);
		STDMETHOD(SetData)(FORMATETC *pformatetc, STGMEDIUM *pmedium,
			BOOL fRelease);

		static	CProtectedDataObject *Create(HWLWND hRe, LONG lStreamFormat);

	private:
		// NOTE: private cons/destructor, may not be allocated on the stack as
		// this would break OLE's current object liveness rules
		CProtectedDataObject();
		~CProtectedDataObject();

		ULONG		_crefs;
		ULONG		_cTotal;		// total number of formats supported
		FORMATETC 	*_prgFormats;	// the array of supported formats
		LONG		_lStreamFormat; // Stream format to use in Rtf cobversion

	private:
		enum TEXTKIND
		{
			tPlain,
			tRtf
		};

		HGLOBAL     TextToHglobal(HGLOBAL &hText, TEXTKIND tKind);
		LPSTORAGE	GetDataForEmbeddedObject(LPOLEOBJECT pOleObj, LPSTORAGE lpstgdest);
		HGLOBAL		GetDataForObjectDescriptor(LPOLEOBJECT pOleObj, DWORD dwAspect);

		HGLOBAL UnicodePlainTextFromRange();

	public:
		HGLOBAL			_hPlainText;	// handle to the plain UNICODE text
		HGLOBAL			_hRtfText;		// handle to the RTF UNICODE text
		IOleObject 	*_pOleObj;		// Embedded Object
		LPSTORAGE		_pObjStg;		// Embedded object data
		HGLOBAL			_hObjDesc;		// Embedded object descriptor
		HMETAFILE		_hMFPict;		// Embedded object metafile
		LONG			_cch;			// number of "characters" in the this
		// dxfer object
		LONG			_cpMin;			// Starting cp for this dxfer object
		DWORD			_cObjs;			// number of objects in this dxfer object.
		HWLWND m_hRe;
	};

	class CProtectedDropSource : public IDropSource
	{
	public:
		// IUnknown methods
		STDMETHOD(QueryInterface)(REFIID riid, void **ppv);
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IDropSource methods
		STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
		STDMETHOD(GiveFeedback)(DWORD dwEffect);

		CProtectedDropSource();

	private:
		// NOTE: private destructor, may not be allocated on the stack as
		// this would break OLE's current object liveness rules
		~CProtectedDropSource();

		ULONG		_crefs;
	};

	class CConvertStr
	{
	public:
		operator char *();

	protected:
		CConvertStr();
		~CConvertStr();
		void Free();

		LPSTR   _pstr;
		char    _ach[MAX_PATH * 2];
	};

	//+---------------------------------------------------------------------------
	//
	//  Member:     CConvertStr::operator char *
	//
	//  Synopsis:   Returns the string.
	//
	//----------------------------------------------------------------------------

	inline
	CConvertStr::operator char *()
	{
		return _pstr;
	}

	//+---------------------------------------------------------------------------
	//
	//  Member:     CConvertStr::CConvertStr
	//
	//  Synopsis:   ctor.
	//
	//----------------------------------------------------------------------------

	inline
	CConvertStr::CConvertStr()
	{
		_pstr = NULL;
	}


	//+---------------------------------------------------------------------------
	//
	//  Member:     CConvertStr::~CConvertStr
	//
	//  Synopsis:   dtor.
	//
	//----------------------------------------------------------------------------

	inline
	CConvertStr::~CConvertStr()
	{
		Free();
	}

	class CStrIn : public CConvertStr
	{
	public:
		CStrIn(LPCWSTR pwstr);
		CStrIn(LPCWSTR pwstr, int cwch);
		int strlen();

	protected:
		CStrIn();
		void Init(LPCWSTR pwstr, int cwch);

		int _cchLen;
	};

	//+---------------------------------------------------------------------------
	//
	//  Member:     CStrIn::CStrIn
	//
	//  Synopsis:   Initialization for derived classes which call Init.
	//
	//----------------------------------------------------------------------------

	inline
	CStrIn::CStrIn()
	{
	}


	//+---------------------------------------------------------------------------
	//
	//  Member:     CStrIn::strlen
	//
	//  Synopsis:   Returns the length of the string in characters, excluding
	//              the terminating NULL.
	//
	//----------------------------------------------------------------------------

	inline int
	CStrIn::strlen()
	{
		return _cchLen;
	}

	// Default size for stack buffer
#define MAX_STACK_BUF 512

	/*
	*	CTempBuf
	*
	* 	@class	A simple temporary buffer allocator class that will allocate
	*			buffers on the stack up to MAX_STACK_BUF and then use the
	*			heap thereafter.
	*/
	class CTempBuf
	{
		//@access Public Data
	public:
		//@cmember Constructor
		CTempBuf();

		//@cmember Destructor
		~CTempBuf();

		//@cmember Get buffer of size cb
		void 					*GetBuf(LONG cb);

		//@access Private Data
	private:

		//@cmember Sets up initial state of object
		void					Init();

		//@cmember Frees any buffers allocated from heap
		void					FreeBuf();

		//@cmember Buffer on stack to use
		char					_chBuf[MAX_STACK_BUF];

		//@cmember Pointer to buffer to use
		void 					*_pv;

		//@cmember Size of currently allocated buffer
		LONG					_cb;
	};

	/*
	*	CTempBuf::CTempBuf
	*
	*	@mfunc	Initialize object
	*
	*/
	inline CTempBuf::CTempBuf()
	{
		Init();
	}

	/*
	*	CTempBuf::~CTempBuf
	*
	*	@mfunc	Free any resources attached to this object
	*
	*/
	inline CTempBuf::~CTempBuf()
	{
		FreeBuf();
	}

	/*
	*	CTempCharBuf
	*
	* 	@class	A wrapper for the temporary buffer allocater that returns a buffer of
	*			char's.
	*
	*	@base	private | CTempBuf
	*/
	class CTempWcharBuf : private CTempBuf
	{
		//@access Public Data
	public:

		//@cmember Get buffer of size cch wide characters
		WCHAR 					*GetBuf(LONG cch);
	};

	/*
	*	CTempBuf::GetBuf
	*
	*	@mfunc	Get a buffer of the requested size
	*
	*	@rdesc	Pointer to buffer or NULL if one could not be allocated
	*
	*/
	inline WCHAR *CTempWcharBuf::GetBuf(
		LONG cch)				//@parm size of buffer needed in *characters*
	{
		return (WCHAR *)CTempBuf::GetBuf(cch * sizeof(WCHAR));
	}

}

#ifndef NOHEADER

#define MSAA_CLASSNAMEIDX_HEADER     (MSAA_CLASSNAMEIDX_BASE+17)
#define DM_TRACE    0x0001
// begin_r_commctrl

#define HDS_HORZ                0x0000
#define HDS_VERT                0x0001  // BUGBUG: not implemente ;Internal
#define HDS_BUTTONS             0x0002
#if (_WIN32_IE >= 0x0300)
#define HDS_HOTTRACK            0x0004
#endif
#define HDS_HIDDEN              0x0008

#define HDS_SHAREDIMAGELISTS    0x0000
#define HDS_PRIVATEIMAGELISTS   0x0010

#define HDS_OWNERDATA           0x0020
#if (_WIN32_IE >= 0x0300)
#define HDS_DRAGDROP            0x0040
#define HDS_FULLDRAG            0x0080
#endif
#if (_WIN32_IE >= 0x0500)
#define HDS_FILTERBAR           0x0100
#endif

// end_r_commctrl

#if (_WIN32_IE >= 0x0500)

#define HDFT_ISSTRING       0x0000      // HD_ITEM.pvFilter points to a HD_TEXTFILTER
#define HDFT_ISNUMBER       0x0001      // HD_ITEM.pvFilter points to a INT
#define HDFT_ISMASK         0x000f

#define HDFT_HASNOVALUE     0x8000      // clear the filter, by setting this bit

#ifdef UNICODE
#define HD_TEXTFILTER HD_TEXTFILTERW
#define HDTEXTFILTER HD_TEXTFILTERW
#define LPHD_TEXTFILTER LPHD_TEXTFILTERW
#define LPHDTEXTFILTER LPHD_TEXTFILTERW
#else
#define HD_TEXTFILTER HD_TEXTFILTERA
#define HDTEXTFILTER HD_TEXTFILTERA
#define LPHD_TEXTFILTER LPHD_TEXTFILTERA
#define LPHDTEXTFILTER LPHD_TEXTFILTERA
#endif

#endif  // _WIN32_IE >= 0x0500

#if (_WIN32_IE >= 0x0300)
#define HD_ITEMA HDITEMA
#define HD_ITEMW HDITEMW
#else
#define HDITEMW  HD_ITEMW
#define HDITEMA  HD_ITEMA
#endif
#define HD_ITEM HDITEM

#define HDITEMA_V1_SIZE CCSIZEOF_STRUCT(HDITEMA, lParam)
#define HDITEMW_V1_SIZE CCSIZEOF_STRUCT(HDITEMW, lParam)


#ifdef UNICODE
#define HDITEM HDITEMW
#define LPHDITEM LPHDITEMW
#define HDITEM_V1_SIZE HDITEMW_V1_SIZE
#else
#define HDITEM HDITEMA
#define LPHDITEM LPHDITEMA
#define HDITEM_V1_SIZE HDITEMA_V1_SIZE
#endif


#define HDI_WIDTH               0x0001
#define HDI_HEIGHT              HDI_WIDTH
#define HDI_TEXT                0x0002
#define HDI_FORMAT              0x0004
#define HDI_LPARAM              0x0008
#define HDI_BITMAP              0x0010
#if (_WIN32_IE >= 0x0300)
#define HDI_IMAGE               0x0020
#define HDI_DI_SETITEM          0x0040
#define HDI_ORDER               0x0080
#endif
#if (_WIN32_IE >= 0x0500)
#define HDI_FILTER              0x0100
#endif
#define HDI_ALL                 0x01ff

#define HDM_GETITEMCOUNT        (HDM_FIRST + 0)
#define Header_GetItemCount(hwndHD) \
	(int)SNDMSG((hwndHD), HDM_GETITEMCOUNT, 0, 0L)


#define HDM_INSERTITEMA         (HDM_FIRST + 1)
#define HDM_INSERTITEMW         (HDM_FIRST + 10)

#ifdef UNICODE
#define HDM_INSERTITEM          HDM_INSERTITEMW
#else
#define HDM_INSERTITEM          HDM_INSERTITEMA
#endif


#define HDM_DELETEITEM          (HDM_FIRST + 2)
#define Header_DeleteItem(hwndHD, i) \
	(BOOL)SNDMSG((hwndHD), HDM_DELETEITEM, (WPARAM)(int)(i), 0L)


#define HDM_GETITEMA            (HDM_FIRST + 3)
#define HDM_GETITEMW            (HDM_FIRST + 11)

#ifdef UNICODE
#define HDM_GETITEM             HDM_GETITEMW
#else
#define HDM_GETITEM             HDM_GETITEMA
#endif

#define HDM_SETITEMA            (HDM_FIRST + 4)
#define HDM_SETITEMW            (HDM_FIRST + 12)

#ifdef UNICODE
#define HDM_SETITEM             HDM_SETITEMW
#else
#define HDM_SETITEM             HDM_SETITEMA
#endif

#if (_WIN32_IE >= 0x0300)
#define HD_LAYOUT  HDLAYOUT
#else
#define HDLAYOUT   HD_LAYOUT
#endif


#define HDM_LAYOUT              (HDM_FIRST + 5)


#define HHT_NOWHERE             0x0001
#define HHT_ONHEADER            0x0002
#define HHT_ONDIVIDER           0x0004
#define HHT_ONDIVOPEN           0x0008
#if (_WIN32_IE >= 0x0500)
#define HHT_ONFILTER            0x0010
#define HHT_ONFILTERBUTTON      0x0020
#endif
#define HHT_ABOVE               0x0100
#define HHT_BELOW               0x0200
#define HHT_TORIGHT             0x0400
#define HHT_TOLEFT              0x0800

#if (_WIN32_IE >= 0x0300)
#define HD_HITTESTINFO HDHITTESTINFO
#else
#define HDHITTESTINFO  HD_HITTESTINFO
#endif


#define HDM_HITTEST             (HDM_FIRST + 6)

#if (_WIN32_IE >= 0x0300)

// lparam = int array of size HDM_GETITEMCOUNT
// the array specifies the order that all items should be displayed.
// e.g.  { 2, 0, 1}
// says the index 2 item should be shown in the 0ths position
//      index 0 should be shown in the 1st position
//      index 1 should be shown in the 2nd position


#define HDM_SETHOTDIVIDER          (HDM_FIRST + 19)
#define Header_SetHotDivider(hwnd, fPos, dw) \
	(int)SNDMSG((hwnd), HDM_SETHOTDIVIDER, (WPARAM)(fPos), (LPARAM)(dw))
// convenience message for external dragdrop
// wParam = BOOL  specifying whether the lParam is a dwPos of the cursor
//              position or the index of which divider to hotlight
// lParam = depends on wParam  (-1 and wParm = FALSE turns off hotlight)
#endif      // _WIN32_IE >= 0x0300

#if (_WIN32_IE >= 0x0500)

#define HDM_SETBITMAPMARGIN          (HDM_FIRST + 20)
#define Header_SetBitmapMargin(hwnd, iWidth) \
	(int)SNDMSG((hwnd), HDM_SETBITMAPMARGIN, (WPARAM)(iWidth), 0)

#define HDM_GETBITMAPMARGIN          (HDM_FIRST + 21)
#define Header_GetBitmapMargin(hwnd) \
	(int)SNDMSG((hwnd), HDM_GETBITMAPMARGIN, 0, 0)
#endif


#if (_WIN32_IE >= 0x0400)
#define HDM_SETUNICODEFORMAT   CCM_SETUNICODEFORMAT
#define Header_SetUnicodeFormat(hwnd, fUnicode)  \
	(BOOL)SNDMSG((hwnd), HDM_SETUNICODEFORMAT, (WPARAM)(fUnicode), 0)

#define HDM_GETUNICODEFORMAT   CCM_GETUNICODEFORMAT
#define Header_GetUnicodeFormat(hwnd)  \
	(BOOL)SNDMSG((hwnd), HDM_GETUNICODEFORMAT, 0, 0)
#endif

#if (_WIN32_IE >= 0x0500)
#define HDM_SETFILTERCHANGETIMEOUT  (HDM_FIRST+22)
#define Header_SetFilterChangeTimeout(hwnd, i) \
	(int)SNDMSG((hwnd), HDM_SETFILTERCHANGETIMEOUT, 0, (LPARAM)(i))

#define HDM_EDITFILTER          (HDM_FIRST+23)
#define Header_EditFilter(hwnd, i, fDiscardChanges) \
	(int)SNDMSG((hwnd), HDM_EDITFILTER, (WPARAM)(i), MAKELPARAM(fDiscardChanges, 0))

// Clear filter takes -1 as a column value to indicate that all
// the filter should be cleared.  When this happens you will
// only receive a single filter changed notification.

#define HDM_CLEARFILTER         (HDM_FIRST+24)
#define Header_ClearFilter(hwnd, i) \
	(int)SNDMSG((hwnd), HDM_CLEARFILTER, (WPARAM)(i), 0)
#define Header_ClearAllFilters(hwnd) \
	(int)SNDMSG((hwnd), HDM_CLEARFILTER, (WPARAM)-1, 0)
#endif

#define HDN_ITEMCHANGINGA           (HDN_FIRST-0)
#define HDN_ITEMCHANGINGW       (HDN_FIRST-20)
#define HDN_ITEMCHANGEDA        (HDN_FIRST-1)
#define HDN_ITEMCHANGEDW        (HDN_FIRST-21)
#define HDN_ITEMCLICKA          (HDN_FIRST-2)
#define HDN_ITEMCLICKW          (HDN_FIRST-22)
#define HDN_ITEMDBLCLICKA       (HDN_FIRST-3)
#define HDN_ITEMDBLCLICKW       (HDN_FIRST-23)
#define HDN_DIVIDERDBLCLICKA    (HDN_FIRST-5)
#define HDN_DIVIDERDBLCLICKW    (HDN_FIRST-25)
#define HDN_BEGINTRACKA         (HDN_FIRST-6)
#define HDN_BEGINTRACKW         (HDN_FIRST-26)
#define HDN_ENDTRACKA           (HDN_FIRST-7)
#define HDN_ENDTRACKW           (HDN_FIRST-27)
#define HDN_TRACKA              (HDN_FIRST-8)
#define HDN_TRACKW              (HDN_FIRST-28)
#if (_WIN32_IE >= 0x0300)
#define HDN_GETDISPINFOA        (HDN_FIRST-9)
#define HDN_GETDISPINFOW        (HDN_FIRST-29)
#define HDN_BEGINDRAG           (HDN_FIRST-10)
#define HDN_ENDDRAG             (HDN_FIRST-11)
#endif
#if (_WIN32_IE >= 0x0500)
#define HDN_FILTERCHANGE        (HDN_FIRST-12)
#define HDN_FILTERBTNCLICK      (HDN_FIRST-13)
#endif

#ifdef UNICODE
#define HDN_ITEMCHANGING         HDN_ITEMCHANGINGW
#define HDN_ITEMCHANGED          HDN_ITEMCHANGEDW
#define HDN_ITEMCLICK            HDN_ITEMCLICKW
#define HDN_ITEMDBLCLICK         HDN_ITEMDBLCLICKW
#define HDN_DIVIDERDBLCLICK      HDN_DIVIDERDBLCLICKW
#define HDN_BEGINTRACK           HDN_BEGINTRACKW
#define HDN_ENDTRACK             HDN_ENDTRACKW
#define HDN_TRACK                HDN_TRACKW
#if (_WIN32_IE >= 0x0300)
#define HDN_GETDISPINFO          HDN_GETDISPINFOW
#endif
#else
#define HDN_ITEMCHANGING         HDN_ITEMCHANGINGA
#define HDN_ITEMCHANGED          HDN_ITEMCHANGEDA
#define HDN_ITEMCLICK            HDN_ITEMCLICKA
#define HDN_ITEMDBLCLICK         HDN_ITEMDBLCLICKA
#define HDN_DIVIDERDBLCLICK      HDN_DIVIDERDBLCLICKA
#define HDN_BEGINTRACK           HDN_BEGINTRACKA
#define HDN_ENDTRACK             HDN_ENDTRACKA
#define HDN_TRACK                HDN_TRACKA
#if (_WIN32_IE >= 0x0300)
#define HDN_GETDISPINFO          HDN_GETDISPINFOA
#endif
#endif



#if (_WIN32_IE >= 0x0300)
#define HD_NOTIFYA              NMHEADERA
#define HD_NOTIFYW              NMHEADERW
#else
#define tagNMHEADERA            _HD_NOTIFY
#define NMHEADERA               HD_NOTIFYA
#define tagHMHEADERW            _HD_NOTIFYW
#define NMHEADERW               HD_NOTIFYW
#endif
#define HD_NOTIFY               NMHEADER


#ifdef UNICODE
#define NMHEADER                NMHEADERW
#define LPNMHEADER              LPNMHEADERW
#else
#define NMHEADER                NMHEADERA
#define LPNMHEADER              LPNMHEADERA
#endif


#ifdef UNICODE
#define NMHDDISPINFO            NMHDDISPINFOW
#define LPNMHDDISPINFO          LPNMHDDISPINFOW
#else
#define NMHDDISPINFO            NMHDDISPINFOA
#define LPNMHDDISPINFO          LPNMHDDISPINFOA
#endif

#endif      // NOHEADER

#define IDS_ENTERTEXTHERE 0x1050

#define V_HIMAGELISTVOID(himl)  \
	if (!IsImageList(himl)) {   \
		RIPMSG(0, "Invalid imagelist handle passed to API"); \
		return;                 \
	}

// WM_SYSKEYDOWN masks for lKeyData
#define SYS_ALTERNATE		0x20000000
#define SYS_PREVKEYSTATE	0x40000000

#ifdef UNICODE
#define FastCharNext(pch) ((pch)+1)
#define FastCharPrev(pchStart, pch) ((pch)-1)
#else
#define FastCharNext        CharNext
#define FastCharPrev        CharPrev
#endif

#define MSAA_CLASSNAMEIDX_BASE 65536L
#define MSAA_CLASSNAMEIDX_STATUS     (MSAA_CLASSNAMEIDX_BASE+11)
#define MSAA_CLASSNAMEIDX_TOOLBAR    (MSAA_CLASSNAMEIDX_BASE+12)
#define MSAA_CLASSNAMEIDX_PROGRESS   (MSAA_CLASSNAMEIDX_BASE+13)
#define MSAA_CLASSNAMEIDX_ANIMATE    (MSAA_CLASSNAMEIDX_BASE+14)
#define MSAA_CLASSNAMEIDX_TAB        (MSAA_CLASSNAMEIDX_BASE+15)
#define MSAA_CLASSNAMEIDX_HOTKEY     (MSAA_CLASSNAMEIDX_BASE+16)
#define MSAA_CLASSNAMEIDX_HEADER     (MSAA_CLASSNAMEIDX_BASE+17)
#define MSAA_CLASSNAMEIDX_TRACKBAR   (MSAA_CLASSNAMEIDX_BASE+18)
#define MSAA_CLASSNAMEIDX_LISTVIEW   (MSAA_CLASSNAMEIDX_BASE+19)
#define MSAA_CLASSNAMEIDX_UPDOWN     (MSAA_CLASSNAMEIDX_BASE+22)
#define MSAA_CLASSNAMEIDX_TOOLTIPS   (MSAA_CLASSNAMEIDX_BASE+24)
#define MSAA_CLASSNAMEIDX_TREEVIEW   (MSAA_CLASSNAMEIDX_BASE+25)

#define ABS(x)                      ((x) > 0 ? (x) : -(x))
#define CMS_TOOLTIP             135
#define SWAP(x,y, _type)  { _type i; i = x; x = y; y = i; }
#define BOUND(x,low,high)   max(min(x, high),low)

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))

// stuff for the moth/datetime pickers
#define IDS_TODAY        0x1043
#define IDS_GOTOTODAY    0x1044
#define IDS_DELIMETERS   0x1045
#define IDS_MONTHFMT     0x1046
#define IDS_MONTHYEARFMT 0x1047

#define DatePick_ShowCheck(pdp)     ((pdp)->hwnd->ci.style & DTS_SHOWNONE)
#define DatePick_AppCanParse(pdp)   ((pdp)->hwnd->ci.style & DTS_APPCANPARSE)
#define DatePick_RightAlign(pdp)    ((pdp)->hwnd->ci.style & DTS_RIGHTALIGN)

#define DatePick_GetPtr(hwnd)      (DATEPICK*)GetWindowPtr(hwnd, 0)
#define DatePick_SetPtr(hwnd, p)   (DATEPICK*)SetWindowPtr(hwnd, 0, p)

#define MonthCal_GetPtr(hwnd)      (MONTHCAL*)GetWindowPtr(hwnd, 0)
#define MonthCal_SetPtr(hwnd, p)   (MONTHCAL*)SetWindowPtr(hwnd, 0, p)

#define MonthCal_IsMultiSelect(pmc)     ((pmc)->hwnd->ci.style & MCS_MULTISELECT)
#define MonthCal_IsDayState(pmc)        ((pmc)->hwnd->ci.style & MCS_DAYSTATE)
#define MonthCal_ShowWeekNumbers(pmc)   ((pmc)->hwnd->ci.style & MCS_WEEKNUMBERS)
#define MonthCal_ShowTodayCircle(pmc)   (!((pmc)->hwnd->ci.style & MCS_NOTODAYCIRCLE))
#define MonthCal_ShowToday(pmc)         (!((pmc)->hwnd->ci.style & MCS_NOTODAY))

#define CopyDate(stS, stD)  ((stD).wYear = (stS).wYear,(stD).wMonth = (stS).wMonth,(stD).wDay = (stS).wDay)
#define CopyTime(stS, stD)  ((stD).wHour = (stS).wHour,(stD).wMinute = (stS).wMinute,(stD).wSecond = (stS).wSecond)

// BUGBUG raymondc - these metrics do not scale with user settings
#define CALBORDER       6

#define INCRSYS_YEAR    0x0001
#define INCRSYS_MONTH   0x0002
#define INCRSYS_WEEK    0x0004
#define INCRSYS_DAY     0x0008
#define INCRSYS_HOUR    0x0010
#define INCRSYS_MINUTE  0x0020
#define INCRSYS_SECOND  0x0040

//
// DATEPICK stuff
//

#define DPYBORDER       2
#define DPXBUFFER       2
#define DP_DXBUTTON     15
#define DP_DYBUTTON     15
#define DP_IDAUTOSPIN   1
#define DP_MSECAUTOSPIN 200
#define DATEPICK_UPDOWN 1000
#define DTP_FORMATLENGTH 128

#define DTS_FORMATMASK      0x000C // ;internal
#define DTS_VALIDBITS       0x003F // ;Internal
#define DTS_INVALIDBITS     ((~DTS_VALIDBITS) & 0x0000FFFF) // ;Internal

#define BUDDHIST_BIAS   543
#define KOREAN_BIAS     2333

//
// Note: SECIncrFocus assumes that SUBEDIT_NONE is numerical value -1
//
#define SUBEDIT_NONE -1 // no field is being edited
#define SUBEDIT_ALL  -2 // all fields are being edited (DTS_APPCANPARSE)

/*
 * First check for a 0, 0 filter which means we want all input.
 * If inverted message range, filter is exclusive.
 */
#define CheckMsgFilter(wMsg, wMsgFilterMin, wMsgFilterMax)                 \
	(   ((wMsgFilterMin) == 0 && (wMsgFilterMax) == 0xFFFFFFFF)            \
		|| (  ((wMsgFilterMin) > (wMsgFilterMax))                             \
			? (((wMsg) <  (wMsgFilterMax)) || ((wMsg) >  (wMsgFilterMin)))    \
			: (((wMsg) >= (wMsgFilterMin)) && ((wMsg) <= (wMsgFilterMax)))))

namespace IUI
{
	HWLWND CreateRoot();
	int ReleaseRoot(HWLWND hRoot);

	LRESULT WINAPI Label_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI SB_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI TV_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI Canvas_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI TabControl_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI DockPanel_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI StackPanel_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI Button_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI RichTextBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI IMRichTextBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI ComboBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI ProgressBar_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI Slider_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI Header_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI ListView_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI HtmlView_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI SplitterBar_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI DateTimePicker_WndProc(HWLWND hWnd, RoutedEventArgs *pe);
	LRESULT WINAPI SpinButton_WndProc(HWLWND hwnd, RoutedEventArgs *pe);

	LRESULT CCSendNotify(IUI_CONTROL_INFO *pci, int code, LPNMHDR pnm);
	void IUIAPI MyNotifyWinEvent(UINT event, HWND hwnd, LONG idContainer, LONG_PTR idChild);
	void *CCLocalReAlloc(void *p, UINT uBytes);
	BOOL Str_Set(LPTSTR *ppsz, LPCTSTR psz);
	HBITMAP CreateColorBitmap(int cx, int cy);
	void FillRectClr(HDC hdc, LPRECT prc, COLORREF clr);
	BOOL MirrorIcon(HICON *phiconSmall, HICON *phiconLarge);
	STDAPI_(HCURSOR) LoadHandCursor(DWORD dwRes);
	BOOL PASCAL ChildOfActiveWindow(HWND hwnd);
	void CCPlaySound(LPCTSTR lpszName);
	LONG GetMessagePosClient(HWLWND hWnd, LPPOINT ppt);
	void CCSetInfoTipWidth(HWND hwndOwner, HWND hwndToolTips);
	void RelayToToolTips(HWND hwndToolTips, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
	int GetIncrementSearchStringA(PISEARCHINFO pis, UINT uiCodePage, LPSTR lpsz);
	int GetIncrementSearchString(PISEARCHINFO pis, LPTSTR lpsz);
	LPWSTR ProduceWFromA(UINT uiCodePage, LPCSTR psz);
	BOOL CCWndProc(IUI_CONTROL_INFO *pci, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plres);
	BOOL IUI_CCWndProc(IUICONTROLINFO *pci, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plres);
	void InitGlobalMetrics(WPARAM wParam);

	BOOL IncrementSearchString(PISEARCHINFO pis, UINT ch, LPTSTR *lplpstr);
	UINT RTLSwapLeftRightArrows(IUI_CONTROL_INFO *pci, WPARAM wParam);
	void IncrementSearchBeep(PISEARCHINFO pis);
	DWORD SetWindowBits(HWND hWnd, int iWhich, DWORD dwBits, DWORD dwValue);
	BOOL CCReleaseCapture(IUI_CONTROL_INFO *pci);
	BOOL PASCAL CheckForDragBegin(HWLWND hWnd, int x, int y);
	UINT GetCodePageForFont(HFONT hFont);
	HFONT CCGetHotFont(HFONT hFont, HFONT *phFontHot);
	LPTSTR CCReturnDispInfoText(LPTSTR pszSrc, LPTSTR pszDest, UINT cchDest);
	int SmoothScrollWindow(PSMOOTHSCROLLINFO psi);
	DWORD CICustomDrawNotify(LibUIDK_LPCONTROLINFO lpci, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd);
	void SHDrawText(HDC hdc, LPCTSTR pszText, RECT *prc, int fmt,
		UINT flags, int cyChar, int cxEllipses, COLORREF clrText, COLORREF clrTextBk);
	HBITMAP CreateMonoBitmap(int cx, int cy);
	HIMAGELIST ImageList_Clone(HIMAGELIST himl, int cx, int cy, UINT flags, int cInitial, int cGrow);
	void ImageList_CopyDitherImage(HIMAGELIST pimlDest, WORD iDst,
		int xDst, int yDst, HIMAGELIST pimlSrc, int iSrc, UINT fStyle);
	DWORD CIFakeCustomDrawNotify(IUI_CONTROL_INFO *lpci, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd);
	LRESULT SendNotifyEx(HWLWND hwndTo, HWLWND hwndFrom, int code, NMHDR *pnmhdr, BOOL bUnicode);
	LPSTR ProduceAFromW(UINT uiCodePage, LPCWSTR psz);
	BOOL InOutWtoA(IUI_CONTROL_INFO *pci, THUNKSTATE *pts, LPWSTR *ppsz, DWORD cchTextMax);
	BOOL IUI_InOutWtoA(IUICONTROLINFO *pci, THUNKSTATE *pts, LPWSTR *ppsz, DWORD cchTextMax);
	void StringBufferAtoW(UINT uiCodePage, LPVOID pvOrgPtr, DWORD dwOrgSize, CHAR **ppszText);
	void InOutAtoW(IUI_CONTROL_INFO *pci, THUNKSTATE *pts, LPSTR *ppsz);
	void IUI_InOutAtoW(IUICONTROLINFO *pci, THUNKSTATE *pts, LPSTR *ppsz);
	BOOL ThunkToolTipTextAtoW(LPTOOLTIPTEXTA lpTttA, LPTOOLTIPTEXTW lpTttW, UINT uiCodePage);
	void SetEditInPlaceSize(HWND hwndEdit, RECT *prc, HFONT hFont, UINT seips);
	HWND CreateEditInPlaceWindow(HWND hwnd, LPCTSTR lpText, int cbText, LONG style, HFONT hFont);
	BOOL CDECL CcshellRipMsgW(BOOL f, LPCSTR pszMsg, ...);
	__inline HANDLE GetSharedHeapHandle(void);
	void *Alloc(long cb);
	void *ReAlloc(void *pb, long cb);
#ifndef WINNT
	HANDLE InitSharedHeap(void);
#endif
	UINT CCSwapKeys(WPARAM wParam, UINT vk1, UINT vk2);
	int IUIStr_GetPtr(LPCTSTR pszCurrent, LPTSTR pszBuf, int cchBuf);
	UINT SystoChar(UINT message, LPARAM lParam);
	HFONT CCCreateStatusFont(void);
	LRESULT WINAPI IUI_CCSendNotify(IUICONTROLINFO *pci, int code, LPNMHDR pnmhdr);
	LRESULT IUI_SendNotifyEx(HWND hwndTo, HWND hwndFrom, int code, NMHDR *pnmhdr, BOOL bUnicode);
	void FAR PASCAL CIInitialize(LPIUICONTROLINFO lpci, HWND hwnd, LPCREATESTRUCT lpcs);
	void PASCAL StripAccelerators(LPTSTR lpszFrom, LPTSTR lpszTo, BOOL fAmpOnly);
	BOOL FAR PASCAL MGetTextExtent(HDC hdc, LPCTSTR lpstr, int cnt, int FAR *pcx, int FAR *pcy);
	void SlideAnimate(HWND hwnd, LPCRECT prc);
	STDAPI_(void) CoolTooltipBubble(IN HWND hwnd, IN LPCRECT prc, BOOL fAllowFade, BOOL fAllowAnimate);
	LRESULT FAR PASCAL IUI_CIHandleNotifyFormat(LPIUICONTROLINFO lpci, LPARAM lParam);
	void FlipPoint(LPPOINT lppt);
	void FAR PASCAL FlipRect(LPRECT prc);
	int GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient);
	int CCLoadStringEx(UINT uID, LPWSTR lpBuffer, int nBufferMax, WORD wLang);

	int GetWeekNumber(const SYSTEMTIME *pst, int dowFirst, int woyFirst);
	int CmpDate(const SYSTEMTIME *pst1, const SYSTEMTIME *pst2);
	int CmpSystemtime(const SYSTEMTIME *pst1, const SYSTEMTIME *pst2);
	void IncrSystemTime(SYSTEMTIME *pstSrc, SYSTEMTIME *pstDest, LONG delta, LONG flags);
	int GetDaysForMonth(int year, int month);
	int GetStartDowForMonth(int year, int month);
	DWORD DaysBetweenDates(const SYSTEMTIME *pstStart, const SYSTEMTIME *pstEnd);
	int DowFromDate(const SYSTEMTIME *pst);

	BOOL IsValidDate(const SYSTEMTIME *pst);
	BOOL IsValidTime(const SYSTEMTIME *pst);
	BOOL IsValidSystemtime(const SYSTEMTIME *pst);

	// 	typedef LRESULT(CALLBACK *SUBCLASSPROC)(HWND hWnd, UINT uMsg, WPARAM wParam,
	// 		LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	// 	BOOL GetWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass,
	// 		DWORD_PTR *pdwRefData);

	int GetLabelStateIndex(CONTROL_STATE eControlState);
	int GetButtonStateIndex(CONTROL_STATE eControlState);
	BOOL RemoveHWLWND(HWLWND hWnd);

	// lprcParentClip相对于hCtrl父控件的裁剪区域
	int DrawControlAndChildren(HWLWND hCtrl, HDC hMemDC, LPCRECT prcPSPaint);
	int Label_DrawBkImage(_LABEL *pLb, HDC hDC, LPCRECT lprcDest);

	int RichTextBox_DrawBkColor(REBox *pREBox, HDC hDC, const RECT &rcClip);
	int RichTextBox_DrawBkImage(REBox *pREBox, HDC hDC, const RECT &rcClip);
	LRESULT RichTextBox_DrawText(REBox *pREBox, HDC hDC, const RECT &rcClip);

	int CreateShadowWindow(CWindowProp *pWindowProp, Window *pHostWnd);
	UINT ConvertToNonClientMouseMessage(int uHitTest, UINT uMouseMsg);

	int FindObjectNearCp(RichTextBox *pRe, LONG cp, __out REOBJECT *pReObj);
	HGLOBAL DuplicateHGlobal(HGLOBAL hglobal);
	HGLOBAL TextHGlobalWtoA(HGLOBAL hglobal);
	HGLOBAL OleGetObjectDescriptorDataFromOleObject(
		LPOLEOBJECT pObj,
		DWORD dwAspect,
		POINTL ptl,
		LPSIZEL pszl
	);
	void RegisterFETCs();
	void InitNLSProcTable(); // 通过dll加载函数地址


} // namespace IUI

namespace ControlResMgr
{
	int IUISetControlImage2(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIIMAGE *phIUIImage,
		LPCTSTR lpszImageNameN,
		LPCTSTR lpszImageNameD);

	int IUISetControlCheckedImage4(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIIMAGE *phIUIImage,
		LPCTSTR lpszImageNameCN,
		LPCTSTR lpszImageNameCH,
		LPCTSTR lpszImageNameCS,
		LPCTSTR lpszImageNameCD);

	int IUISetControlForeground2(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIIMAGE *phIUIImage,
		LPCTSTR lpszImageNameN,
		LPCTSTR lpszImageNameD);

	// Get control image
	int IUIGetControlImage(HIUIIMAGE hIUIImage, CIUIString *pstrImageName);

	int IUIGetControlImage2(
		UINT uMask,
		HIUIIMAGE hIUICombineImage,
		HIUIIMAGE *phIUIImage,
		BOOL bCombineImage,
		CIUIString *pstrImageNameCombine,
		CIUIString *pstrImageNameN,
		CIUIString *pstrImageNameD);

} // namespace ControlResMgr

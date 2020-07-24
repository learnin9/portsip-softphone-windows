#include "StdAfx.h"
#include <WinUser.h>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


/*
* List Box Instance Variables
*/
typedef struct _SCROLLPOS
{
	INT cItems;
	UINT iPage;
	INT iPos;
	UINT fMask;
	INT iReturn;
} SCROLLPOS, *PSCROLLPOS;

// ListBox私有成员，用于ComboBox下拉列表
typedef struct tagLBIV
{
public:
	tagLBIV()
		: spwndParent(NULL)
		, spwnd(NULL)
		, iTop(0)
		, iSel(0)
		, iSelBase(0)
		, cItemFullMax(0)
		, cMac(0)
		, cMax(0)
		, rgpch(NULL)
		, hStrings(NULL)
		, cchStrings(0)
		, ichAlloc(0)
		, cxChar(0)
		, cyChar(0)
		, cxColumn(0)
		, itemsPerColumn(0)
		, numberOfColumns(0)
		, OwnerDraw(0)
		, fRedraw(0)
		, fDeferUpdate(0)
		, wMultiple(0)
		, fSort(0)
		, fNotify(0)
		, fMouseDown(0)
		, fCaptured(0)
		, fCaret(0)
		, fDoubleClick(0)
		, fCaretOn(0)
		, fAddSelMode(0)
		, fHasStrings(0)
		, fHasData(0)
		, fNewItemState(0)
		, fUseTabStops(0)
		, fMultiColumn(0)
		, fNoIntegralHeight(0)
		, fWantKeyboardInput(0)
		, fDisableNoScroll(0)
		, fHorzBar(0)
		, fVertBar(0)
		, fFromInsert(0)
		, fNoSel(0)
		, fHorzInitialized(0)
		, fVertInitialized(0)
		, fSized(0)
		, fIgnoreSizeMsg(0)
		, fInitialized(0)
		, fRightAlign(0)
		, fRtoLReading(0)
		, fSmoothScroll(0)
		, xRightOrigin(0)
		, iLastSelection(0)
		, iMouseDown(0)
		, iLastMouseMove(0)
		, iTabPixelPositions(NULL)
		, hFont(NULL)
		, xOrigin(0)
		, maxWidth(0)
		, pcbox(NULL)
		, hdc(NULL)
		, dwLocaleId(0)
		, iTypeSearch(0)
		, pszTypeSearch(NULL)
		, m_hVerticalScrollBar(NULL)
		, m_hHorizontalScrollBar(NULL)
		, m_bFloatVBar(FALSE)
		, m_bFloatHBar(FALSE)
		, m_nHScrollBarHeight(0)
		, m_nVScrollBarWidth(0)
		, m_pBindHorzScrollBarStyle(NULL)
		, m_pBindVertScrollBarStyle(NULL)
		, m_crItemBkH(IUIGetGloabelVar()->g_clrHighlight)
		, m_hVirtualRoot(NULL)

	{
		ptPrev.x = ptPrev.y = 0;
		ZeroMemory(&HPos, sizeof(SCROLLPOS));
		ZeroMemory(&VPos, sizeof(SCROLLPOS));
	}

	virtual ~tagLBIV()
	{
		Release();
	}

	int Release()
	{
		DestroyWindow(m_hVerticalScrollBar);
		m_hVerticalScrollBar = NULL;

		DestroyWindow(m_hHorizontalScrollBar);
		m_hHorizontalScrollBar = NULL;

		ReleaseRoot(m_hVirtualRoot);
		m_hVirtualRoot = NULL;

		return 0;
	}

	HWND    spwndParent;    /* parent window */
	HWND    spwnd;          /* lbox ctl window */
	INT     iTop;           /* index of top item displayed          */
	INT     iSel;           /* index of current item selected       */
	INT     iSelBase;       /* base sel for multiple selections     */

							// 给定的ListBox高度能容纳完整Item的个数
							// 如果是固定高度的Item，它的值为CItemInWindow(plb, FALSE)，否则为1.
	INT cItemFullMax;

	// ListBox中Item总数
	INT cMac;

	// ListBox内容数据结构能容纳的Item的数量，注意，它并不是当前放入ListBox中的Item的数量
	INT cMax;

	PBYTE   rgpch;          /* pointer to array of string offsets    */

							// ListBox所有Item的字符串都在hStrings中保存。
							// LBItem::offsz记录了每个Item字符串在hString中的偏移。
	LPWSTR hStrings;

	INT     cchStrings;     /* Size in bytes of hStrings            */
	INT     ichAlloc;       /* Pointer to end of hStrings (end of last valid
							string) */
	INT     cxChar;         /* Width of a character                 */
	INT     cyChar;         /* height of line                       */
	INT     cxColumn;       /* width of a column in multicolumn listboxes */
	INT     itemsPerColumn; /* for multicolumn listboxes */
	INT     numberOfColumns; /* for multicolumn listboxes */
	POINT   ptPrev;         /* coord of last tracked mouse pt. used for auto
							scrolling the listbox during timer's */

	UINT    OwnerDraw : 2;     /* Owner draw styles. Non-zero if ownerdraw. */
	UINT     fRedraw : 1;     /* if TRUE then do repaints             */
	UINT     fDeferUpdate : 1; /* */
	UINT    wMultiple : 2;      /* SINGLESEL allows a single item to be selected.
								* MULTIPLESEL allows simple toggle multi-selection
								* EXTENDEDSEL allows extended multi selection;
								*/

	UINT     fSort : 1;       /* if TRUE the sort list                */
	UINT     fNotify : 1;     /* if TRUE then Notify parent           */
	UINT     fMouseDown : 1;  /* if TRUE then process mouse moves/mouseup */
	UINT     fCaptured : 1;   // if TRUE then process mouse messages
	UINT     fCaret : 1;      /* flashing caret allowed               */
	UINT     fDoubleClick : 1; /* mouse down in double click           */
	UINT     fCaretOn : 1;    /* if TRUE then caret is on             */
	UINT     fAddSelMode : 1; /* if TRUE, then it is in ADD selection mode */
	UINT     fHasStrings : 1;  /* True if the listbox has a string associated
							   * with each item else it has an app suppled LONG
							   * value and is ownerdraw
							   */
	UINT     fHasData : 1;    /* if FALSE, then lb doesn't keep any line data
							  * beyond selection state, but instead calls back
							  * to the client for each line's definition.
							  * Forces OwnerDraw==OWNERDRAWFIXED, !fSort,
							  * and !fHasStrings.
							  */
	UINT     fNewItemState : 1; /* select/deselect mode? for multiselection lb
								*/
	UINT     fUseTabStops : 1; /* True if the non-ownerdraw listbox should handle
							   * tabstops
							   */
	UINT     fMultiColumn : 1; /* True if this is a multicolumn listbox */
	UINT     fNoIntegralHeight : 1; /* True if we don't want to size the listbox
									* an integral lineheight
									*/
	UINT     fWantKeyboardInput : 1; /* True if we should pass on WM_KEY & CHAR
									 * so that the app can go to special items
									 * with them.
									 */
	UINT     fDisableNoScroll : 1;   /* True if the listbox should
									 * automatically Enable/disable
									 * it's scroll bars. If false, the scroll
									 * bars will be hidden/Shown automatically
									 * if they are present.
									 */
	UINT    fHorzBar : 1; // TRUE if WS_HSCROLL specified at create time

	UINT    fVertBar : 1; // TRUE if WS_VSCROLL specified at create time
	UINT    fFromInsert : 1; // TRUE if client drawing should be deferred during delete/insert ops
	UINT    fNoSel : 1;

	UINT    fHorzInitialized : 1;   // Horz scroll cache initialized
	UINT    fVertInitialized : 1;   // Vert scroll cache initialized

	UINT    fSized : 1;             // Listbox was resized.
	UINT    fIgnoreSizeMsg : 1;     // If TRUE, ignore WM_SIZE message

	UINT    fInitialized : 1;

	UINT    fRightAlign : 1;    // used primarily for MidEast right align
	UINT    fRtoLReading : 1;   // used only for MidEast, text rtol reading order
	UINT    fSmoothScroll : 1;  // allow just one smooth-scroll per scroll cycle

	int     xRightOrigin;      // For horizontal scrolling. The current x origin

	INT     iLastSelection; /* Used for cancelable selection. Last selection
							* in listbox for combo box support
							*/
	INT     iMouseDown;     /* For multiselection mouse click & drag extended
							* selection. It is the ANCHOR point for range
							* selections
							*/
	INT     iLastMouseMove; /* selection of listbox items */
							/*
							* IanJa/Win32: Tab positions remain int for 32-bit API ??
							*/
	LPINT   iTabPixelPositions; /* List of positions for tabs */
	HANDLE  hFont;          /* User settable font for listboxes */
	int     xOrigin;        /* For horizontal scrolling. The current x origin */
	int     maxWidth;       /* Maximum width of listbox in pixels for
							horizontal scrolling purposes */
	PCBOX   pcbox;          /* Combo box pointer */
	HDC     hdc;            /* hdc currently in use */
	DWORD   dwLocaleId;     /* Locale used for sorting strings in list box */
	int     iTypeSearch;
	LPWSTR  pszTypeSearch;
	SCROLLPOS HPos;
	SCROLLPOS VPos;

	// IUI Add
	HWLWND m_hVerticalScrollBar;
	HWLWND m_hHorizontalScrollBar;
	UINT m_bFloatVBar; // 浮动ScrollBar不占用客户区位置，即不会挤压客户区内容
	UINT m_bFloatHBar;
	int m_nHScrollBarHeight;
	int m_nVScrollBarWidth;
	CScrollBarProp *m_pBindHorzScrollBarStyle;
	CScrollBarProp *m_pBindVertScrollBarStyle;
	COLORREF m_crItemBkH; // Hover item background color

	// 虚拟根控件。 模仿TreeView控件的虚拟根节点。 本虚拟根控件永远不会显示，
	// 仅作为根控件的窗口，不参与布局、消息处理等业务
	HWLWND m_hVirtualRoot;
} LBIV, *PLBIV;

/*
* The following is the header of all objects managed in the handle list.
* (allocated as part of the object for easy access).  All object
* headers must start with the members of a HEAD structure.
*/
typedef struct _HEAD
{
	HANDLE h;
	DWORD cLockObj;
} HEAD, *PHEAD;


#ifndef _USERSRV_
/*
* Monitor information structure.
*
*     This structure defines the attributes of a single monitor
*     in a virtual display.
*/
typedef struct tagMONITOR
{
	HEAD                    head;            // object handle stuff

	struct tagMONITOR      *pMonitorNext;    // next monitor in free or used list
	DWORD                   dwMONFlags;      // flags
	RECT                    rcMonitor;       // location of monitor in virtual screen coordinates
	RECT                    rcWork;          // work area of monitor in virtual screen coordinates
	HRGN                    hrgnMonitor;     // monitor region in virtual screen coordinates
	short                   cFullScreen;     // number of fullscreen apps on this monitor
	short                   cWndStack;       // number of tiled top-level windows
	HANDLE                  hDev;            // hdev associated with this monitor
} MONITOR, *PMONITOR;
#endif

typedef struct _ZONE_HEADER
{
	SINGLE_LIST_ENTRY FreeList;
	SINGLE_LIST_ENTRY SegmentList;
	ULONG BlockSize;
	ULONG TotalSegmentSize;
} ZONE_HEADER, *PZONE_HEADER;


/*
* Lookaside definitions
*/
typedef struct _LOOKASIDE
{
	PVOID LookasideBase;
	PVOID LookasideBounds;
	ZONE_HEADER LookasideZone;
	DWORD EntrySize;
#if DBG
	ULONG AllocHiWater;
	ULONG AllocCalls;
	ULONG AllocSlowCalls;
	ULONG DelCalls;
	ULONG DelSlowCalls;
#endif // DBG
} LOOKASIDE, *PLOOKASIDE;

typedef struct tagLBItem
{
	// 记录了本Item字符串在tagLBIV::hString中的偏移。
	LONG offsz;
	ULONG_PTR itemData;
	HWLWND hBindWindowless;
} LBItem, *lpLBItem;

typedef struct tagLBODItem
{
	ULONG_PTR itemData;
	HWLWND hBindWindowless;
} LBODItem, *lpLBODItem;


LRESULT xxxCBCreateHandler(
	PCBOX pcbox,
	HWLWND pwnd);

void xxxCBSetFontHandler(
	PCBOX pcbox,
	HANDLE hFont,
	BOOL fRedraw);
void xxxCBShowListBoxWindow(
	PCBOX pcbox, BOOL fTrack);
BOOL xxxCBHideListBoxWindow(
	PCBOX pcbox,
	BOOL fNotifyParent,
	BOOL fSelEndOK);

void xxxCBUpdateListBoxWindow(
	PCBOX pcbox,
	BOOL fSelectionAlso);
void xxxCBCompleteEditWindow(
	PCBOX pcbox);
void xxxCBNcDestroyHandler(
	HWLWND pwnd,
	PCBOX pcbox);
void xxxCBGetFocusHelper(
	PCBOX pcbox);
void xxxCBKillFocusHelper(
	PCBOX pcbox);
void xxxCBSizeHandler(
	PCBOX pcbox);
void xxxCBPosition(PCBOX pcbox);
int xxxCBDir(
	PCBOX pcbox,
	UINT attrib,
	LPWSTR pFileName);
void xxxCBInternalUpdateEditWindow(
	PCBOX pcbox,
	HDC hdcPaint);
LONG xxxCBSetEditItemHeight(
	PCBOX pcbox,
	int dyEdit);
LRESULT xxxCBMessageItemHandler(
	PCBOX pcbox,
	UINT message,
	LPVOID lpfoo);  /* Actually can be any of the structs below */
LONG CBNcCreateHandler(
	PCBOX pcbox,
	HWLWND pwnd);
void xxxCBNotifyParent(
	PCBOX pcbox,
	short notificationCode);
void xxxCBCalcControlRects(PCBOX pcbox, LPRECT lprcList);
int LayoutScrollBar(PLBIV plb);

#ifndef USEREXTS
typedef struct _TL
{
	int iBogus;
} TL;
#endif

typedef struct _TL *PTL;
#define CheckLock(pobj)
#define Lock(ppobj, pobj) (*ppobj = pobj)

typedef struct tagSTAT
{
	HWND spwnd;
	union
	{
		HANDLE hFont;
		BOOL   fDeleteIt;
	};
	HANDLE hImage;
	UINT cicur;
	UINT iicur;
	UINT fPaintKbdCuesOnly : 1;
} STAT, *PSTAT;

#define ThreadLock(PVOID, PTL)

//L #ifdef _DEBUG
// VOID ThreadLock(PVOID pobj, PTL ptl);
// #else // _DEBUG
// #define ThreadLock(_pobj_, _ptl_)          \
// {                                          \
// 	\
// 	PTHREADINFO _pti_;                     \
// 	PVOID __pobj_ = (_pobj_);              \
// 	\
// 	_pti_ = PtiCurrent();                  \
// 	(_ptl_)->next = _pti_->ptl;            \
// 	_pti_->ptl = (_ptl_);                  \
// 	(_ptl_)->pobj = __pobj_;              \
// 	if (__pobj_ != NULL) {                \
// 	HMLockObject(__pobj_);            \
// 	}                                      \
// }
// #endif // _DEBUG

#ifndef MAXUSHORT
#define MAXUSHORT 0xffff
#endif

#define ThreadUnlock(ptl) (ptl)
#define PTR_TO_ID(p)    ((USHORT)(((ULONG_PTR)(p)) & MAXUSHORT))

#define WM_FINALDESTROY          0x0070
#define WM_UPDATEUISTATE 0x0128


#define SDROPPABLE      2
#define SEDITABLE       1

// for pcbox->CBoxStyle
#define SSIMPLE         SEDITABLE
#define SDROPDOWNLIST   SDROPPABLE
#define SDROPDOWN		(SDROPPABLE | SEDITABLE)

#define IsComboVisible(pcbox) (!pcbox->fNoRedraw && IsWindowVisible(pcbox->hwnd))

#define WHEEL_DELTA                     120
/*
* CBOX.OwnerDraw & LBIV.OwnerDraw types
*/
#define OWNERDRAWFIXED 1
#define OWNERDRAWVAR   2

#define UPPERCASE 1
#define LOWERCASE 2
#define LB_ADDSTRINGUPPER        0x01AC
#define LB_ADDSTRINGLOWER        0x01AD
#define LB_INSERTSTRINGUPPER     0x01AA
#define LB_INSERTSTRINGLOWER     0x01AB
#define LBCB_STARTTRACK          0x01AE
#define LBCB_ENDTRACK            0x01AF
#define LBCB_CARETON             0x01A3
#define LBCB_CARETOFF            0x01A4

#define CBEDITID 1001
/*
* Macro to mask off uniqueness bits for WOW handles
*/
#define SAMEWOWHANDLE(h1, h2)  ((BOOL)!(((UINT)(h1) ^ (UINT)(h2)) & 0xffff))
#define DIFFWOWHANDLE(h1, h2)  (!SAMEWOWHANDLE(h1, h2))

#define CBEC_SETCOMBOFOCUS           (WM_USER+CB_MSGMAX+1)
#define CBEC_KILLCOMBOFOCUS          (WM_USER+CB_MSGMAX+2)

#define FNID_LISTBOX                0x000002A4      // No server side proc
#define FNID_STATIC                 0x000002A6      // No server side proc

#define HWq(X)	(X)
#define HW(X)	(X)
#define PWND HWND
#define IDSYS_CARET         0x0000FFFFL
#define IDSYS_SCROLL        0x0000FFFEL
/* Listbox notification messages */
#define WM_VKEYTOITEM       0x002E
#define WM_CHARTOITEM       0x002F
#define WM_LBTRACKPOINT     0x0131              /* ;Internal */
#define HILITE     1
#define WEFDRAGOBJECT           0x0802  // ???
#define WM_BEGINDRAG        0x022C              /* ;Internal */

#define LBUP_RELEASECAPTURE 0x0001
#define LBUP_RESETSELECTION 0x0002
#define LBUP_NOTIFY         0x0004
#define LBUP_SUCCESS        0x0008
#define LBUP_SELCHANGE      0x0010

#define VERKEY_SLASH     0xBF   /* Vertual key for '/' character */
#define VERKEY_BACKSLASH 0xDC   /* Vertual key for '\' character */

/***************************************************************************\
*
* ListBox
*
\***************************************************************************/

#define IsLBoxVisible(plb)  (plb->fRedraw && IsWindowVisible(plb->spwnd))
#define MultDiv(x, y, z) (((INT)(x) * (INT)(y) + (INT)(z) / 2) / (INT)(z))

#define HIGHLIGHT           17

#define HILITEONLY      0x0001
#define SELONLY         0x0002
#define HILITEANDSEL    (HILITEONLY + SELONLY)

#define         SINGLESEL       0
#define         MULTIPLESEL     1
#define         EXTENDEDSEL     2
#define SW_SCROLLWINDOW     0x8000

//
// System timer IDs
//
#define IDSYS_WNDTRACKING   0x0000FFF7L
#define IDSYS_FLASHWND      0x0000FFF8L
#define IDSYS_MNAUTODISMISS 0x0000FFF9L
#define IDSYS_MOUSEHOVER    0x0000FFFAL
#define IDSYS_MNANIMATE     0x0000FFFBL
#define IDSYS_MNDOWN        MFMWFP_DOWNARROW /* 0xFFFFFFFC */
#define IDSYS_LBSEARCH      0x0000FFFCL
#define IDSYS_MNUP          MFMWFP_UPARROW   /* 0xFFFFFFFD */
#define IDSYS_STANIMATE     0x0000FFFDL
#define IDSYS_MNSHOW        0x0000FFFEL
#define IDSYS_SCROLL        0x0000FFFEL
#define IDSYS_MNHIDE        0x0000FFFFL
#define IDSYS_CARET         0x0000FFFFL

#define EQ        0
#define PREFIX    1
#define LT        2
#define GT        3

#define LBI_ADD     0x0004
#define CITEMSALLOC 32

#define MSGFLAG_MASK                0xFFFE0000
#define MSGFLAG_WOW_RESERVED        0x00010000      // Used by WOW
#define MSGFLAG_DDE_MID_THUNK       0x80000000      // DDE tracking thunk
#define MSGFLAG_DDE_SPECIAL_SEND    0x40000000      // WOW bad DDE app hack
#define MSGFLAG_SPECIAL_THUNK       0x10000000      // server->client thunk needs special handling

extern PVOID pUserHeap;
#define UserLocalAlloc(uFlag,uBytes) HeapAlloc(pUserHeap, uFlag, (uBytes))
#define UserLocalReAlloc(p, uBytes, uFlags) HeapReAlloc(pUserHeap, uFlags, (LPSTR)(p), (uBytes))
#define UserLocalFree(X) HeapFree(pUserHeap, 0, (X));

#define CaretCreate(plb)    ((plb)->fCaret = TRUE)

#define ucWheelScrollLines 40
#define cxSysFontChar 16
#define cySysFontChar 16

#define FNID_DDE_BIT                0x00002000    // Used by RegisterClassExWOW
#define FNID_CLEANEDUP_BIT          0x00004000
#define FNID_DELETED_BIT            0x00008000
#define FNID_STATUS_BITS            (FNID_CLEANEDUP_BIT | FNID_DELETED_BIT)

#define FNID_START                  0x0000029A
#define FNID_WNDPROCSTART           0x0000029A

#define FNID_SCROLLBAR              0x0000029A      // xxxSBWndProc;
#define FNID_ICONTITLE              0x0000029B      // xxxDefWindowProc;
#define FNID_MENU                   0x0000029C      // xxxMenuWindowProc;
#define FNID_DESKTOP                0x0000029D      // xxxDesktopWndProc;
#define FNID_DEFWINDOWPROC          0x0000029E      // xxxDefWindowProc;

#define FNID_WNDPROCEND             0x0000029E      // see PatchThreadWindows
#define FNID_CONTROLSTART           0x0000029F

#define FNID_BUTTON                 0x0000029F      // No server side proc
#define FNID_COMBOBOX               0x000002A0      // No server side proc
#define FNID_COMBOLISTBOX           0x000002A1      // No server side proc
#define FNID_DIALOG                 0x000002A2      // No server side proc
#define FNID_EDIT                   0x000002A3      // No server side proc
#define FNID_LISTBOX                0x000002A4      // No server side proc
#define FNID_MDICLIENT              0x000002A5      // No server side proc
#define FNID_STATIC                 0x000002A6      // No server side proc

#define FNID_IME                    0x000002A7      // No server side proc
#define FNID_CONTROLEND             0x000002A7

#define FNID_HKINLPCWPEXSTRUCT      0x000002A8
#define FNID_HKINLPCWPRETEXSTRUCT   0x000002A9
#define FNID_DEFFRAMEPROC           0x000002AA      // No server side proc
#define FNID_DEFMDICHILDPROC        0x000002AB      // No server side proc
#define FNID_MB_DLGPROC             0x000002AC      // No server side proc
#define FNID_MDIACTIVATEDLGPROC     0x000002AD      // No server side proc
#define FNID_SENDMESSAGE            0x000002AE

#define FNID_SENDMESSAGEFF          0x000002AF
#define FNID_SENDMESSAGEEX          0x000002B0
#define FNID_CALLWINDOWPROC         0x000002B1
#define FNID_SENDMESSAGEBSM         0x000002B2
#define FNID_SWITCH                 0x000002B3      // Just used by GetTopMostInserAfter
#define FNID_TOOLTIP                0x000002B4
#define FNID_END                    0x000002B4

#define WM_DROPOBJECT       0x022A              /* ;Internal */
#define WM_QUERYDROPOBJECT  0x022B              /* ;Internal */
#define WM_BEGINDRAG        0x022C              /* ;Internal */
#define WM_DRAGLOOP         0x022D              /* ;Internal */
#define WM_DRAGSELECT       0x022E              /* ;Internal */
#define WM_DRAGMOVE         0x022F              /* ;Internal */
#define WM_QUERYDRAGICON    0x0037
#define WM_DROPFILES        0x0233

BOOL LBGetItemRect(
	PLBIV plb,
	INT sItem,
	LPRECT lprc);

INT LBGetVariableHeightItemHeight(
	PLBIV plb,
	INT itemNumber);


INT CItemInWindow(
	PLBIV plb,
	BOOL fPartial);

LONG TabTextOut(
	HDC hdc,
	int x,
	int y,
	LPCWSTR lpstring,
	int nCount,
	int nTabPositions,
	CONST INT *lpTabPositions,
	int iTabOrigin,
	BOOL fDrawTheText,
	int iCharset);


LPWSTR GetLpszItem(
	PLBIV pLBIV,
	INT sItem);

BOOL IsSelected(
	PLBIV plb,
	INT sItem,
	UINT wOpFlags);

void xxxLBoxDrawItem(
	PLBIV plb,
	INT item,
	UINT itemAction,
	UINT itemState,
	LPRECT lprect);

INT CItemInWindowVarOwnerDraw(
	PLBIV plb,
	BOOL fPartial);

INT LBPage(
	PLBIV plb,
	INT startItem,
	BOOL fPageForwardDirection);

void xxxLBShowHideScrollBars(
	PLBIV plb);

void xxxNewITopEx(
	PLBIV plb,
	INT iTopNew,
	DWORD dwTime);

int xxxSetLBScrollParms(PLBIV plb, int nCtl);

void xxxAlterHilite(
	PLBIV plb,
	INT i,
	INT j,
	BOOL fHilite,
	INT OpFlags,
	BOOL fSelStatus);

void xxxLBBlockHilite(
	PLBIV plb,
	INT iSelFromPt,
	BOOL fKeyBoard);

void xxxLBButtonUp(PLBIV plb, UINT uFlags);

void xxxInsureVisible(
	PLBIV plb,
	INT iSel,
	BOOL fPartial);  /* It is ok for the item to be partially visible */

INT LBCalcVarITopScrollAmt(
	PLBIV plb,
	INT iTopOld,
	INT iTopNew);

LONG_PTR LBGetItemData(
	PLBIV plb,
	INT sItem);

HWLWND LBGetItemBind(PLBIV plb, INT nItem);

int LBSetHighlightItemBkColor(PLBIV plb, COLORREF crItemBk);
COLORREF LBGetHighlightItemBkColor(PLBIV plb);

void xxxNewITop(PLBIV plb, INT iTopNew);

void xxxLBoxCtlHScroll(
	PLBIV plb,
	INT cmd,
	int xAmt);

void xxxNotifyOwner(
	PLBIV plb,
	INT sEvt);

void xxxCheckRedraw(
	PLBIV plb,
	BOOL fConditional,
	INT sItem);

BOOL xxxLBInvalidateRect(PLBIV plb, LPRECT lprc, BOOL fErase);

void xxxLBoxDeleteItem(
	PLBIV plb,
	INT sItem);

void LBSetCItemFullMax(
	PLBIV plb);

void xxxLBSetRedraw(
	PLBIV plb,
	BOOL fRedraw);

UINT LBCalcAllocNeeded(
	PLBIV plb,
	INT cItems);

BOOL xxxLBResetContent(
	PLBIV plb);

void xxxInvertLBItem(
	PLBIV plb,
	INT i,
	BOOL fHilite);  /* The new selection state of the item */

void xxxTrackMouse(
	PLBIV plb,
	UINT wMsg,
	POINT pt);

void xxxLBPaint(
	PLBIV plb,
	HDC hdc,
	LPRECT lprcBounds);

int xxxLBNcPaint(PLBIV plb, HDC hDC);

void xxxLBSetCaret(PLBIV plb, BOOL fSetCaret);

void xxxCaretDestroy(
	PLBIV plb);

void xxxLBoxCtlScroll(
	PLBIV plb,
	INT cmd,
	int yAmt);

BOOL LBSetTabStops(
	PLBIV plb,
	INT count,
	LPINT lptabstops);

void InitHStrings(
	PLBIV plb);

BOOL NtUserSetWindowFNID(
	IN HWND hwnd,
	IN WORD fnid);

LRESULT APIENTRY ComboBoxWndProcWorker(ComboBox *pwnd, RoutedEventArgs *pe);

BOOL APIENTRY CopyInflateRect(
	LPRECT prcDst,
	CONST RECT *prcSrc,
	int cx, int cy);

BOOL ISelFromPt(
	PLBIV plb,
	POINT pt,
	LPDWORD piItem);

void LBSetVariableHeightItemHeight(
	PLBIV plb,
	INT itemNumber,
	INT itemHeight);

LONG xxxLBCreate(
	PLBIV plb, PWND pwnd, LPCREATESTRUCT lpcs);

void xxxLBSetFont(
	PLBIV plb,
	HANDLE hFont,
	BOOL fRedraw);

void LBDropObjectHandler(
	PLBIV plb,
	PDROPSTRUCT pds);

INT xxxLBInsertItem(
	PLBIV plb,

	/*
	* For owner draw listboxes without LBS_HASSTRINGS style, this is not a
	* string but rather a 4 byte value we will store for the app.
	*/
	LPWSTR lpsz,
	INT index,
	UINT wFlags);

int LBSetItemData(
	PLBIV plb,
	int index,
	LONG_PTR data);

int LBSetItemBind(PLBIV plb, int index, HWLWND hBind);

LONG xxxLBInitStorage(PLBIV plb, BOOL fAnsi, INT cItems, INT cb);
INT xxxLBoxCtlDelete(
	PLBIV plb,
	INT sItem);  /* Item number to delete */

LONG xxxLBSetSel(
	PLBIV plb,
	BOOL fSelect,  /* New state to set selection to */
	INT iSel);

int xxxLBSetCurSel(
	PLBIV plb,
	int iSel);

void xxxLBSelRange(
	PLBIV plb,
	int iStart,
	int iEnd,
	BOOL fnewstate);

INT LBGetText(
	PLBIV plb,
	BOOL fLengthOnly,
	BOOL fAnsi,
	INT index,
	LPWSTR lpbuffer);

INT xxxLBSetCount(
	PLBIV plb,
	INT cItems);

INT xxxFindString(
	PLBIV plb,
	LPWSTR lpstr,
	INT sStart,
	INT code,
	BOOL fWrap);

void xxxLBoxCtlKeyInput(
	PLBIV plb,
	UINT msg,
	UINT vKey);

void xxxLBoxCtlCharInput(
	PLBIV plb,
	UINT  inputChar,
	BOOL  fAnsi);

int LBoxGetSelItems(
	PLBIV plb,
	BOOL fCountOnly,
	int wParam,
	LPINT lParam);

void xxxSetISelBase(
	PLBIV plb,
	INT sItem);

int LBGetSetItemHeightHandler(
	PLBIV plb,
	UINT message,
	int item,
	UINT height);

LRESULT APIENTRY ListBoxWndProcWorker(
	HWND pwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	DWORD fAnsi);

LRESULT WINAPI ListBoxWndProcW(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

void xxxLBSize(
	PLBIV plb,
	INT cx,
	INT cy);

void xxxDestroyLBox(
	PLBIV pLBIV,
	PWND pwnd);


#pragma hdrstop
#define WM_MOUSEWHEEL                   0x020A
#define CheckLock(pobj)


typedef LRESULT(APIENTRY *WNDPROC_PWNDEX)(HWND, UINT, WPARAM, LPARAM, ULONG_PTR);
typedef LRESULT(APIENTRY *WNDPROC_PWND)(HWND, UINT, WPARAM, LPARAM);
#define FNID_ARRAY_SIZE 32
#define FNID_WNDPROCEND 0x0000029E
#define FNID_START 0x0000029A
#define FNID_END 0x000002B4
#define CrackCombinedDbcsTB(c) ((c) >> 8)
#define STATEOFFSET (FIELD_OFFSET(WND, state))
#define TestWF(hwnd, flag)   (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) & LOBYTE(flag))

// typedef struct tagSERVERINFO {      // si
//     WORD    wRIPFlags;              // RIPF_ flags
//     WORD    wSRVIFlags;             // SRVIF_ flags
//     WORD    wRIPPID;                // PID of process to apply RIP flags to (zero means all)
//     WORD    wRIPError;              // Error to break on (zero means all errors are treated equal)
//
//     DWORD   cHandleEntries;         // count of handle entries in array
//
//     /*
//      * Array of server-side function pointers.
//      * Client passes servers function ID so they can be easily validated;
//      * this array maps function ID into server-side function address.
//      * The order of these are enforced by the FNID_ constants, and must match
//      * the client-side mpFnidClientPfn[] order as well.
//      */
//     WNDPROC_PWNDEX mpFnidPfn[FNID_ARRAY_SIZE]; // function mapping table
//     WNDPROC_PWND aStoCidPfn[(FNID_WNDPROCEND - FNID_START) + 1];
//
//     // mapping of fnid to min bytes need by public windproc user
//     WORD mpFnid_serverCBWndProc[(FNID_END - FNID_START) + 1];
//
//     /*
//      * Client side functions pointer structure.
//      */
//     struct _PFNCLIENT apfnClientA;
//     struct _PFNCLIENT apfnClientW;
//     struct _PFNCLIENTWORKER apfnClientWorker;
//
//     DWORD cbHandleTable;
//
//     /*
//      * Class atoms to allow fast checks on the client.
//      */
//     ATOM atomSysClass[ICLS_MAX];   // Atoms for control classes
//
//     DWORD dwDefaultHeapBase;            // so WOW can do handle validation
//     DWORD dwDefaultHeapSize;
//
//     UINT uiShellMsg;         // message for shell hooks
//
//     UINT  wMaxBtnSize;   /* Size of the longest button string in any MessageBox */
//     DWORD mpAllMBbtnStringsToSTR[MAX_MB_STRINGS];  /* MB button String IDs */
//     DWORD AllMBbtnStrings[MAX_MB_STRINGS]; /* MB button String buffer offsets*/
//     WCHAR szOK[10];
//     WCHAR szCANCEL[15];
//     WCHAR szYES[10];
//     WCHAR szNO[10];
//     WCHAR szABORT[15];
//     WCHAR szRETRY[15];
//     WCHAR szIGNORE[15];
//     WCHAR szCLOSE[15];
//     WCHAR szHELP[15];
//
//     /*
//      * values to allow HasCaptionIcon to be in user32
//      */
//     ATOM atomIconSmProp;
//     ATOM atomIconProp;
//
//     ATOM atomContextHelpIdProp;
//
//     char acOemToAnsi[NCHARS];
//     char acAnsiToOem[NCHARS];
//
//     /*
//      * Per user settings. We use _HYDRA_'s PERUSERSERVERINO struct
//      * to avoid defining fields in two places.
//      */
//     PERUSERSERVERINFO;
//
// #if DEBUGTAGS
//     DWORD adwDBGTAGFlags[DBGTAG_Max + 1];
// #endif // DEBUGTAGS
//
// } SERVERINFO, *PSERVERINFO;

// PSERVERINFO gpsi;

/*
 * Quick test for any Window Event Hooks.
 */
#define TEST_FLAG(field, flag)                  ((field) & (flag))
#define TEST_SRVIF(f)                   TEST_FLAG(gpsi->wSRVIFlags, f)

#ifdef _USERK_
#define FWINABLE() gpWinEventHooks
#else
#define FWINABLE() TEST_SRVIF(SRVIF_WINEVENTHOOKS)
#endif
/************************************************************************\
* CopyInflateRect (API)
*
* This function copies the rect from prcSrc to prcDst, and inflates it.
*
* History:
* 12-16-93  FritzS
\************************************************************************/

BOOL APIENTRY CopyInflateRect(
	LPRECT prcDst,
	CONST RECT *prcSrc,
	int cx, int cy)
{
	prcDst->left   = prcSrc->left   - cx;
	prcDst->right  = prcSrc->right  + cx;
	prcDst->top    = prcSrc->top    - cy;
	prcDst->bottom = prcSrc->bottom + cy;
	return TRUE;
}


WORD DbcsCombine(HWND hwnd, WORD ch)
{
	MSG msg;
	int i = 10; /* loop counter to avoid the infinite loop */

	while (!PeekMessageA(&msg, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE))
	{
		if (--i == 0)
		{
			return 0;
		}
		Sleep(1);
	}

	return (WORD)ch | ((WORD)(msg.wParam) << 8);
}

long xxxCBCommandHandler(PCBOX pcbox, DWORD wParam, HWND hwndControl);


///////////////////////////////////////////////////////////////

LOOKASIDE ComboboxLookaside;

BOOL NtUserTrackMouseEvent(TRACKMOUSEEVENT *ptme);
LONG xxxCBGetTextLengthHelper(PCBOX pcbox, BOOL fAnsi);
LONG xxxCBGetTextHelper(PCBOX pcbox, int len, LPWSTR lpstr, BOOL fAnsi);

/***************************************************************************\
* HotTrack
*
* If we're not already hot-tracking and the mouse is over the combobox,
* turn on hot-tracking and invalidate the drop-down button.
*
\***************************************************************************/

#ifdef COLOR_HOTTRACKING

void HotTrack(PCBOX pcbox)
{
	if (!pcbox->fButtonHotTracked && !pcbox->fMouseDown)
	{
		HWND hwnd = HWq(pcbox->spwnd);
		TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
		if (NtUserTrackMouseEvent(&tme))
		{
			pcbox->fButtonHotTracked = TRUE;
			InvalidateRect(hwnd, &pcbox->buttonrc, TRUE);
		}
	}
}

#endif // COLOR_HOTTRACKING

/***************************************************************************\
* xxxComboBoxDBCharHandler
*
* Double Byte character handler for ANSI ComboBox
*
* History:
\***************************************************************************/

LRESULT ComboBoxDBCharHandler(
	PCBOX pcbox,
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	//WORD w;
	//HWND pwndSend;

	//w = DbcsCombine(hwnd, (BYTE)wParam);
	//if (w == 0)
	//{
	//	return CB_ERR;  // Failed to assemble DBCS
	//}

	//_ASSERT(pcbox->spwndList);
	//if (pcbox->fNoEdit)
	//{
	//	pwndSend = pcbox->spwndList;
	//}
	//else if (pcbox->spwndEdit)
	//{
	//	//L         RIPMSG1(RIP_WARNING, "ComboBoxWndProcWorker: WM_CHAR is posted to Combobox itself(%08x).",
	//	//                 hwnd);
	//	pwndSend = pcbox->spwndEdit;
	//}
	//else
	//{
	//	return CB_ERR;
	//}

	////L    RIPMSG1(RIP_VERBOSE, "ComboBoxWndProcWorker: sending WM_CHAR %04x", w);

	////L     if (!TestWF(pwndSend, WFANSIPROC))
	//// 	{
	////         //
	////         // If receiver is not ANSI WndProc (may be subclassed?),
	////         // send a UNICODE message.
	////         //
	////         WCHAR wChar;
	////         LPWSTR lpwstr = &wChar;
	////
	////         if (MBToWCSEx(THREAD_CODEPAGE(), (LPCSTR)&w, 2, &lpwstr, 1, FALSE) == 0) {
	////             RIPMSG1(RIP_WARNING, "ComboBoxWndProcWorker: cannot convert 0x%04x to UNICODE.", w);
	////             return CB_ERR;
	////         }
	////         return SendMessage(pwndSend, message, wChar, lParam, FALSE);
	////     }

	///*
	// * Post the Trailing byte to the target
	// * so that they can peek the second WM_CHAR
	// * message later.
	// * Note: it's safe since sender is A and receiver is A,
	// * translation layer does not perform any DBCS combining and cracking.
	// */
	////L PostMessageA(pwndSend, message, CrackCombinedDbcsTB(w), lParam);
	//return SendMessage(pwndSend, message, wParam, lParam); //L
	return 0;
}

BOOL ComboBoxMsgOKInInit(UINT message, LRESULT *plRet)
{
	switch (message)
	{
	default:
		break;
	case WM_SIZE:
		*plRet = 0;
		return FALSE;
	case WM_STYLECHANGED:
	case WM_GETTEXT:
	case WM_GETTEXTLENGTH:
	case WM_PRINT:
	case WM_COMMAND:
	case CBEC_KILLCOMBOFOCUS:
	case WM_PRINTCLIENT:
	case WM_SETFONT:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_MOUSEWHEEL:
	case WM_CAPTURECHANGED:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
	case WM_SETREDRAW:
	case WM_ENABLE:
	case CB_SETDROPPEDWIDTH:
	case CB_DIR:
	case CB_ADDSTRING:
		/*
		 * Cannot handle those messages yet. Bail out.
		 */
		*plRet = CB_ERR;
		return FALSE;
	}
	return TRUE;
}

int ComboBox_DrawBkColor(CBOX *pCb, HDC hDC, LPCRECT lpRect)
{
	if (!IsDrawBkColor(pCb->hwnd))
	{
		return 1;
	}

	// Prepare state index
	COLORREF crBk = pCb->hwnd->ci.m_crBkN;
	if (IsWindowEnabled(pCb->hwnd))
	{
		CONTROL_STATE eControlState = GetControlState(pCb->hwnd);
		int nStateIndex = GetButtonStateIndex(eControlState);
		crBk = pCb->m_crBk[nStateIndex];
	}
	else
	{
		crBk = pCb->hwnd->ci.m_crBkD;
	}

	CIUIRect rcClient;
	GetClientRect(pCb->hwnd, rcClient);

	FillSolidRect(hDC, rcClient, crBk, IsUpdateLayeredWindow(GetHostWnd(pCb->hwnd)));

	return 0;
}

int ComboBox_DrawBkImage(CBOX *pCb, HDC hDC, LPCRECT lprcDest)
{
	// Prepare state index
	int nStateIndex = 0;
	if (IsWindowEnabled(pCb->hwnd))
	{
		CONTROL_STATE eControlState = GetControlState(pCb->hwnd);
		nStateIndex = GetButtonStateIndex(eControlState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pCb->hwnd, rcClient);

	if (IsCombineBkImage(pCb->hwnd))
	{
		IUIPartNineGridBlt(hDC, rcClient,
			pCb->hwnd->ci.m_himgCombineBk, pCb->hwnd->ci.m_rcBkImage9GridResizeArg, 4, nStateIndex);
	}
	else
	{
		IUIDrawImage(hDC, rcClient,
			pCb->m_himgBk[nStateIndex],
			pCb->hwnd->ci.m_eBkImageResizeMode, pCb->hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;
}

// 下拉按钮共5态，多了一个选中状态
int ComboBox_DrawButton(CBOX *pcbox, HDC hDC, LPCRECT lprcDest)
{
	CONTROL_STATE eButtonIndex = pcbox->m_eButtonState;
	int nButtonStateIndex = GetButtonStateIndex(eButtonIndex);

	if (pcbox->m_bDrawButtonBkColor)
	{
		COLORREF cr = pcbox->m_crButtonBk[nButtonStateIndex];

		HBRUSH hBrush = ::CreateSolidBrush(cr);
		::FillRect(hDC, &pcbox->buttonrc, hBrush);
		::DeleteObject(hBrush);
	}

	// 优化使用组合按钮背景位图，当设置了组合按钮背景后，忽略非组合按钮背景。
	if (pcbox->m_himgCombineButtonBk != NULL)
	{
		IUIPartDrawImage(hDC, &pcbox->buttonrc, pcbox->m_himgCombineButtonBk,
			IRM_STRETCH, NULL, COMBINEIMAGESIZE4 + 1, nButtonStateIndex);
	}
	else
	{
		IUIDrawImage(hDC, &pcbox->buttonrc, pcbox->m_himgButtonBk[nButtonStateIndex],
			IRM_STRETCH, NULL);
	}

	// 绘制按钮边框颜色
	if (pcbox->m_bDrawButtonBorderColor)
	{
		COLORREF cr = pcbox->m_crButtonBorder[nButtonStateIndex];

		HBRUSH hBrush = ::CreateSolidBrush(cr);
		::FrameRect(hDC, &pcbox->buttonrc, hBrush);
		::DeleteObject(hBrush);

#ifdef _DEBUG
		CIUIString s;
		s.Format(_T("%d"), nButtonStateIndex);
		DrawText(hDC, s, s.GetLength(), &pcbox->buttonrc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
#endif
	}

	return 0;
}

int ComboBox_DrawBorder(CBOX *pCb, HDC hDC)
{
	if (!IsDrawBorderColor(pCb->hwnd))
	{
		return 1;
	}

	// Prepare state index
	int nStateIndex = 0;
	if (IsWindowEnabled(pCb->hwnd))
	{
		CONTROL_STATE eControlState = GetControlState(pCb->hwnd);
		nStateIndex = GetButtonStateIndex(eControlState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pCb->hwnd, rcClient);

	HBRUSH hbrBorder = CreateSolidBrush(pCb->m_crBorder[nStateIndex]);
	FrameRect(hDC, rcClient, hbrBorder);
	DeleteObject(hbrBorder);

	return 0;
}

int ComboBox_OnPaint(CBOX *pCb, HDC hDC, const RECT &rcClip)
{
	if (!IsRedraw(pCb->hwnd))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pCb->hwnd, rcClient);
	CIUIRect rcPaint;
	if (!::IntersectRect(&rcPaint, &rcClip, rcClient))
	{
		return 2;
	}

	HDC hDCTarget = NULL;
	HDC hMemDC = NULL;
	HBITMAP hOldBmp = NULL;

	hDCTarget = hDC;

	BeforePaint(pCb->hwnd, hDCTarget);
	ComboBox_DrawBkColor(pCb, hDCTarget, &rcClip);
	ComboBox_DrawBkImage(pCb, hDCTarget, NULL);
	ComboBox_DrawButton(pCb, hDCTarget, NULL);
	ComboBox_DrawBorder(pCb, hDCTarget);
	AfterPaint(pCb->hwnd, hDCTarget);

	if (IsCachedMode(pCb->hwnd) && hMemDC != NULL && hOldBmp != NULL)
	{
		BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
			hDCTarget, 0, 0, SRCCOPY);
		::SelectObject(hMemDC, hOldBmp);
		DeleteDC(hMemDC);
	}

	return 0;
}

/***************************************************************************\
* xxxComboBoxCtlWndProc/ComboBox_EventHandler
*
* Class procedure for all combo boxes
*
* History:
\***************************************************************************/
LRESULT WINAPI IUI::ComboBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	CBOX *pcbox = (CBOX *)GetMember(hWnd);

	UINT message = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	BOOL fAnsi = FALSE;
	POINT pt;
	TL tlpwndEdit;
	TL tlpwndList;
	//PAINTSTRUCT ps;
	LPWSTR lpwsz = NULL;
	LRESULT lReturn;
	static BOOL fInit = TRUE;
	int  i;

	if (pe->hOriginalSender == hWnd)
	{
		CheckLock(pwnd);

		//L     VALIDATECLASSANDSIZE(pwnd, FNID_COMBOBOX);
		//     INITCONTROLLOOKASIDE(&ComboboxLookaside, CBOX, spwnd, 8);

		/*
		 * Get the pcbox for the given window now since we will use it a lot in
		 * various handlers.  This is stored by NtUserSetWindowLongPtr() in the
		 * INITCONTROLLOOKASIDE macro above.
		 */

		//L    pcbox = ((PCOMBOWND)pwnd)->pcbox;

		/*
		 * Protect the combobox during the initialization.
		 */
		if (pcbox != NULL && pcbox->spwndList == NULL)
		{
			LRESULT lRet;

			if (!ComboBoxMsgOKInInit(message, &lRet))
			{
				//             RIPMSG2(RIP_WARNING, "ComboBoxWndProcWorker: msg=%04x is sent to hwnd=%08x in the middle of initialization.",
				//                     message, hwnd);
				return lRet;
			}
		}

		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			switch (message)
			{
			case WM_NCCREATE:
				if (pcbox == NULL)
				{
					CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;

					pcbox = (PCBOX)new CBOX;
					pcbox->hwnd = hWnd;
					pcbox->hwnd->ci.hwndParent = GetParent(hWnd);
					pcbox->CBoxStyle = pcs->style;
					SetMember(hWnd, pcbox);
				}

				/*
				 * wParam - Contains a handle to the window being created
				 * lParam - Points to the CREATESTRUCT data structure for the window.
				 */
				return CBNcCreateHandler(pcbox, hWnd);

			case WM_DESTROY:
			{
				pcbox->Release();
				delete pcbox;
				pcbox = NULL;
				SetMember(hWnd, NULL);
			}
			return 0;

			case WM_CREATE:
				/*
				* wParam - not used
				* lParam - Points to the CREATESTRUCT data structure for the window.
				*/
			{
				LRESULT lr = xxxCBCreateHandler(pcbox, hWnd);
				return lr;
			}

			case WM_CAPTURECHANGED:
				//L         if (!(TestWF(pwnd, WFWIN40COMPAT)))
				//             return 0;

				if ((pcbox->fMouseDown))
				{
					pcbox->fMouseDown = FALSE;

					//
					// Pop combo listbox back up, canceling.
					//
					if (pcbox->fLBoxVisible)
					{
						xxxCBHideListBoxWindow(pcbox, TRUE, FALSE);
					}

					InvalidateRect(hWnd, NULL);
				}
				break;

			case WM_STYLECHANGED:
				_ASSERT(pcbox->spwndList != NULL);
				{
					LONG OldStyle;
					LONG NewStyle = 0;

					LONG lExStyle = GetExStyle(hWnd);

					pcbox->fRtoLReading = (lExStyle & WS_EX_RTLREADING);
					pcbox->fRightAlign  = (lExStyle & WS_EX_RIGHT);
					if (pcbox->fRtoLReading)
					{
						NewStyle |= (WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR);
					}
					if (pcbox->fRightAlign)
					{
						NewStyle |= WS_EX_RIGHT;
					}

					ThreadLock(pcbox->spwndList, &tlpwndList);
					OldStyle = GetWindowLong(pcbox->spwndList, GWL_EXSTYLE) & ~(WS_EX_RIGHT | WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR);
					SetWindowLong(pcbox->spwndList, GWL_EXSTYLE, OldStyle | NewStyle);
					ThreadUnlock(&tlpwndList);

					if (!pcbox->fNoEdit && pcbox->spwndEdit)
					{
						ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
						OldStyle = GetExStyle(pcbox->spwndEdit) & ~(WS_EX_RIGHT | WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR);
						SetExStyle(pcbox->spwndEdit, OldStyle | NewStyle);
						ThreadUnlock(&tlpwndEdit);
					}
					xxxCBPosition(pcbox);
					InvalidateRect(hWnd, NULL);
				}
				break;

			case WM_NCDESTROY:
			case WM_FINALDESTROY:
				xxxCBNcDestroyHandler(hWnd, pcbox);
				break;

			case WM_SETFOCUS:
				if (pcbox->fNoEdit)
				{

					/*
					 * There is no editcontrol so set the focus to the combo box itself.
					 */
					xxxCBGetFocusHelper(pcbox);
				}
				else if (pcbox->spwndEdit)
				{
					/*
					 * Set the focus to the edit control window if there is one
					 */
					//L            ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
					// 输入框在得到Focus后，会向父控件发送参数为EN_SETFOCUS的WM_COMMAND命令
					SetFocus(GetHostWnd(pcbox->spwndEdit), pcbox->spwndEdit);
					//            ThreadUnlock(&tlpwndEdit);
				}
				break;

			case WM_KILLFOCUS:

				/*
				 * wParam has the new focus hwnd
				 */
				//L         if (wParam != 0)
				//             wParam = (WPARAM)ValidateHwnd((HWND)wParam);
				//if ((wParam == 0) || !IsChild(pwnd, (HWND)wParam))
				//{

				//	/*
				//	 * We only give up the focus if the new window getting the focus
				//	 * doesn't belong to the combo box.
				//	 */
				//	xxxCBKillFocusHelper(pcbox);
				//}

				_ASSERT(pcbox->spwndList);
				//L         {
				//             PLBIV plb = ((PLBWND)pcbox->spwndList)->pLBIV;
				//
				//             if ((plb != NULL) && (plb != (PLBIV)-1)) {
				//                 plb->iTypeSearch = 0;
				//                 if (plb->pszTypeSearch) {
				//                     UserLocalFree(plb->pszTypeSearch);
				//                     plb->pszTypeSearch = NULL;
				//                 }
				//             }
				//         }
				break;

			case WM_ENABLE:

				/*
				 * Invalidate the rect to cause it to be drawn in grey for its
				 * disabled view or ungreyed for non-disabled view.
				 */
				InvalidateRect(hWnd, NULL);
				if ((pcbox->CBoxStyle & SEDITABLE) && pcbox->spwndEdit)
				{

					/*
					 * Enable/disable the edit control window
					 */
					//L            ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
					EnableWindow(pcbox->spwndEdit, IsWindowEnabled(hWnd));
					//            ThreadUnlock(&tlpwndEdit);
				}

				/*
				 * Enable/disable the listbox window
				 */
				_ASSERT(pcbox->spwndList);
				//L        ThreadLock(pcbox->spwndList, &tlpwndList);
				::EnableWindow(pcbox->spwndList, IsWindowEnabled(hWnd));
				//        ThreadUnlock(&tlpwndList);
				break;

			case WM_WINDOWPOSCHANGED:

				/*
				 * wParam - defines the type of resizing fullscreen, sizeiconic,
				 *          sizenormal etc.
				 * lParam - new width in LOWORD, new height in HIGHUINT of client area
				 */
				_ASSERT(pcbox->spwndList);
				if (LOWORD(lParam) == 0 || HIWORD(lParam) == 0)
				{

					/*
					 * If being sized to a zero width or to a zero height or we aren't
					 * fully initialized, just return.
					 */
					return 0;
				}

				// OPTIMIZATIONS -- first check if old and new widths are the same
				RECT rcWindow;
				GetWindowRect(hWnd, &rcWindow);
				if (pcbox->cxCombo == rcWindow.right - rcWindow.left)
				{
					int iNewHeight = rcWindow.bottom - rcWindow.top;

					// now check if new height is the dropped down height
					if (pcbox->fLBoxVisible)
					{
						// Check if new height is the full size height
						if (pcbox->cyDrop + pcbox->cyCombo == iNewHeight)
						{
							return (0L);
						}
					}
					else
					{
						// Check if new height is the closed up height
						if (pcbox->cyCombo == iNewHeight)
						{
							return (0L);
						}
					}
				}

				xxxCBSizeHandler(pcbox);
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			switch (message)
			{
			case WM_SYSKEYDOWN:
				if (lParam & 0x20000000L)  /* Check if the alt key is down */
				{

					/*
					 * Handle Combobox support.  We want alt up or down arrow to behave
					 * like F4 key which completes the combo box selection
					 */
					if (lParam & 0x1000000)
					{

						/*
						 * This is an extended key such as the arrow keys not on the
						 * numeric keypad so just drop the combobox.
						 */
						if (wParam == VK_DOWN || wParam == VK_UP)
						{
							goto DropCombo;
						}

						goto CallDWP;
					}

					if (GetKeyState(VK_NUMLOCK) & 0x1)
					{
						/*
						 * If numlock down, just send all system keys to dwp
						 */
						goto CallDWP;
					}
					else
					{

						/*
						 * We just want to ignore keys on the number pad...
						 */
						if (!(wParam == VK_DOWN || wParam == VK_UP))
						{
							goto CallDWP;
						}
					}
DropCombo:
					if (!pcbox->fLBoxVisible)
					{

						/*
						 * If the listbox isn't visible, just show it
						 */
						xxxCBShowListBoxWindow(pcbox, TRUE);
					}
					else
					{

						/*
						 * Ok, the listbox is visible.  So hide the listbox window.
						 */
						if (!xxxCBHideListBoxWindow(pcbox, TRUE, TRUE))
						{
							return (0L);
						}
					}
				}
				goto CallDWP;
				break;

			// 直接事件中，也有一份相同的拷贝。 因为来自ComboBox的子RichTextBox
			// 也会把VK_ESCAPE等按键，通过WM_KEYDOWN，直接转发给ComboBox
			case WM_KEYDOWN:
				/*
				 * If the listbox is dropped and the ENTER key is pressed,
				 * close up the listbox successfully.  If ESCAPE is pressed,
				 * close it up like cancel.
				 */
				if (pcbox->fLBoxVisible)
				{
					if ((wParam == VK_RETURN) || (wParam == VK_ESCAPE))
					{
						xxxCBHideListBoxWindow(pcbox, TRUE, (wParam != VK_ESCAPE));
						InvalidateRect(hWnd, NULL);
						break;
					}
				}
			// FALL THROUGH

			case WM_CHAR:
				//L         if (fAnsi && IS_DBCS_ENABLED() && IsDBCSLeadByteEx(THREAD_CODEPAGE(), (BYTE)wParam)) {
				//             return ComboBoxDBCharHandler(pcbox, hWnd, message, wParam, lParam);
				//         }

				if (pcbox->fNoEdit)
				{
					goto CallListSendMessage;
				}
				else
				{
					goto CallEditSendMessage;
				}
				break;

			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:

#ifdef COLOR_HOTTRACKING
				pcbox->fButtonHotTracked = FALSE;
#endif // COLOR_HOTTRACKING

				/*
				 * Set the focus to the combo box if we get a mouse click on it.
				 */
				if (!pcbox->fFocus)
				{
					SetFocus(GetHostWnd(hWnd), hWnd);
					if (!pcbox->fFocus)
					{

						/*
						 * Don't do anything if we still don't have the focus.
						 */
						break;
					}
				}

				/*
				 * If user clicked in button rect and we are a combobox with edit, then
				 * drop the listbox.  (The button rect is 0 if there is no button so the
				 * ptinrect will return false.) If a drop down list (no edit), clicking
				 * anywhere on the face causes the list to drop.
				 */

				POINTSTOPOINT(pt, lParam);
				if ((pcbox->CBoxStyle == SDROPDOWN && PtInRect(&pcbox->buttonrc, pt))
					|| pcbox->CBoxStyle == SDROPDOWNLIST)
				{

					/*
					 * Set the fMouseDown flag so that we can handle clicking on
					 * the popdown button and dragging into the listbox (when it just
					 * dropped down) to make a selection.
					 */
					if (pcbox->fLBoxVisible)
					{
						if (pcbox->fMouseDown)
						{
							pcbox->fMouseDown = FALSE;
							ReleaseCapture(hWnd);
						}

						InvalidateRect(hWnd, &pcbox->buttonrc);

						if (!xxxCBHideListBoxWindow(pcbox, TRUE, TRUE))
						{
							return (0L);
						}
					}
					else
					{
						xxxCBShowListBoxWindow(pcbox, FALSE);

						// Setting and resetting this flag must always be followed
						// imediately by SetCapture or ReleaseCapture
						//
						pcbox->fMouseDown = TRUE;
						pcbox->m_eButtonState = CONTROL_STATE_PRESSED;
						InvalidateRect(hWnd, &pcbox->buttonrc);

						SetCapture(hWnd);
						//L                 if (FWINABLE()) {
						//                     NotifyWinEvent(EVENT_OBJECT_STATECHANGE, hWnd, OBJID_CLIENT, INDEX_COMBOBOX_BUTTON);
						//                 }
					}
				}
				return 0;

			case WM_MOUSEWHEEL:
				/*
				 * Handle only scrolling.
				 */
				if (wParam & (MK_CONTROL | MK_SHIFT))
				{
					goto CallDWP;
				}

				/*
				 * If the listbox is visible, send it the message to scroll.
				 */
				if (pcbox->fLBoxVisible)
				{
					goto CallListSendMessage;
				}

				/*
				 * If we're in extended UI mode or the edit control isn't yet created,
				 * bail.
				 */
				if (pcbox->fExtendedUI || pcbox->spwndEdit == NULL)
				{
					return TRUE;
				}

				/*
				 * Emulate arrow up/down messages to the edit control.
				 */
				i = abs(((short)HIWORD(wParam)) / WHEEL_DELTA);
				wParam = ((short)HIWORD(wParam) > 0) ? VK_UP : VK_DOWN;

				//L        ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
				while (i-- > 0)
				{
					SendMessage(pcbox->spwndEdit, WM_KEYDOWN, wParam, 0);
				}
				//L        ThreadUnlock(&tlpwndEdit);
				return TRUE;

			case WM_LBUTTONUP:
				pe->bHandled = true;
				POINTSTOPOINT(pt, lParam);

				/*
				 * Clear this flag so that mouse moves aren't sent to the listbox
				 */
				if (pcbox->fMouseDown)
				{
					pcbox->fMouseDown = FALSE;

					if (pcbox->CBoxStyle == SDROPDOWN)
					{
						// If an item in the listbox matches the text in the edit
						// control, scroll it to the top of the listbox. Select the
						// item only if the mouse button isn't down otherwise we
						// will select the item when the mouse button goes up.
						xxxCBUpdateListBoxWindow(pcbox, TRUE);
						xxxCBCompleteEditWindow(pcbox);
					}
					ReleaseCapture(hWnd);

					// Now, we want listbox to track mouse moves while mouse up
					// until mouse down, and select items as though they were
					// clicked on.
					//if (TestWF(hWnd, WFWIN40COMPAT))
					{
						ThreadLock(pcbox->spwndList, &tlpwndList);
						::SendMessage(pcbox->spwndList, LBCB_STARTTRACK, FALSE, 0);
						ThreadUnlock(&tlpwndList);
					}

					pcbox->m_eButtonState = CONTROL_STATE_SELECTED;
					InvalidateRect(hWnd, NULL);
				}
				return 0;

			case WM_MOUSELEAVE:
				pcbox->m_eButtonState = CONTROL_STATE_NORMAL;
				InvalidateRect(hWnd, NULL);
				break;

			case WM_MOUSEMOVE:
				POINTSTOPOINT(pt, lParam);

				if (pcbox->fMouseDown)
				{
					if (PtInRect(&pcbox->buttonrc, pt))
					{
						if (pcbox->m_eButtonState != CONTROL_STATE_PRESSED)
						{
							pcbox->m_eButtonState = CONTROL_STATE_PRESSED;
							InvalidateRect(hWnd, &pcbox->buttonrc);
						}
					}
					else
					{
						if (pcbox->m_eButtonState != CONTROL_STATE_HOVER)
						{
							pcbox->m_eButtonState = CONTROL_STATE_HOVER;
							InvalidateRect(hWnd, &pcbox->buttonrc);
						}
					}

					ClientToScreen(hWnd, &pt);

					RECT rcList;
					::GetClientRect(pcbox->spwndList, &rcList);
					BOOL bRet = ::ClientToScreen(pcbox->spwndList, (LPPOINT)&rcList);
					bRet = ::ClientToScreen(pcbox->spwndList, ((LPPOINT)&rcList) + 1);

					if (PtInRect(&rcList, pt))
					{

						/*
						 * This handles dropdown comboboxes/listboxes so that clicking
						 * on the dropdown button and dragging into the listbox window
						 * will let the user make a listbox selection.
						 */
						pcbox->fMouseDown = FALSE;
						ReleaseCapture(hWnd);

						if (pcbox->CBoxStyle & SEDITABLE)
						{

							/*
							 * If an item in the listbox matches the text in the edit
							 * control, scroll it to the top of the listbox.  Select the
							 * item only if the mouse button isn't down otherwise we
							 * will select the item when the mouse button goes up.
							 */

							/*
							 * We need to select the item which matches the editcontrol
							 * so that if the user drags out of the listbox, we don't
							 * cancel back to his origonal selection
							 */
							xxxCBUpdateListBoxWindow(pcbox, TRUE);
						}

						/*
						 * Convert point to listbox coordinates and send a buttondown
						 * message to the listbox window.
						 */
						::ScreenToClient(pcbox->spwndList, &pt);
						lParam = POINTTOPOINTS(pt);
						message = WM_LBUTTONDOWN;
						goto CallListSendMessage;
					}
				}
				else
				{
					if (pcbox->m_eButtonState == CONTROL_STATE_NORMAL)
					{
						if (PtInRect(&pcbox->buttonrc, pt))
						{
							pcbox->m_eButtonState = CONTROL_STATE_HOVER;
							InvalidateRect(hWnd, NULL);
						}
					}
					else if (pcbox->m_eButtonState == CONTROL_STATE_HOVER)
					{
						if (!PtInRect(&pcbox->buttonrc, pt))
						{
							pcbox->m_eButtonState = CONTROL_STATE_NORMAL;
							InvalidateRect(hWnd, NULL);
						}
					}
					//if (pcbox->fButtonPressed)
					//{
					//	pcbox->fButtonPressed = TRUE;
					//	InvalidateRect(hWnd, NULL);
					//}
				}
#ifdef COLOR_HOTTRACKING
				HotTrack(pcbox);
#endif // COLOR_HOTTRACKING
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (message)
			{
			// 下沉事件里也有一份同样的代码
			case WM_MOUSEWHEEL:
				/*
				 * Handle only scrolling.
				 */
				if (wParam & (MK_CONTROL | MK_SHIFT))
				{
					goto CallDWP;
				}

				/*
				 * If the listbox is visible, send it the message to scroll.
				 */
				if (pcbox->fLBoxVisible)
				{
					goto CallListSendMessage;
				}

				/*
				 * If we're in extended UI mode or the edit control isn't yet created,
				 * bail.
				 */
				if (pcbox->fExtendedUI || pcbox->spwndEdit == NULL)
				{
					return TRUE;
				}

				/*
				 * Emulate arrow up/down messages to the edit control.
				 */
				i = abs(((short)HIWORD(wParam)) / WHEEL_DELTA);
				wParam = ((short)HIWORD(wParam) > 0) ? VK_UP : VK_DOWN;

				//L        ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
				while (i-- > 0)
				{
					SendMessage(pcbox->spwndEdit, WM_KEYDOWN, wParam, 0);
				}
				//L        ThreadUnlock(&tlpwndEdit);
				return TRUE;

			case WM_PRINTCLIENT:
			case WM_PAINT:
			case WM_PRINT:
				ComboBox_OnPaint(pcbox, (HDC)wParam, *LPCRECT(lParam));
				return (0);

			case WLM_BIND_STYLE:
				ComboBox_BindStyle(hWnd, (CComboBoxProp *)lParam);
				return 0;

			case WLM_SET_BK_IMAGE:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = LPCTSTR(lParam);

				if (!bChecked)
				{
					if (eControlState >= CONTROL_STATE_NORMAL
						&& eControlState <= CONTROL_STATE_DISABLED)
					{
						int nStateIndex = GetButtonStateIndex(eControlState);

						return IUISetControlImage(GetProject(hWnd),
								&pcbox->m_himgBk[nStateIndex], lpszImage);
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						return IUISetControlImage(GetProject(hWnd),
								&pcbox->hwnd->ci.m_himgCombineBk, lpszImage);
					}
				}
			}
			return 0;

			case WLM_GET_BK_IMAGE:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (!bChecked)
				{
					if (eControlState >= CONTROL_STATE_NORMAL
						&& eControlState <= CONTROL_STATE_DISABLED)
					{
						int nStateIndex = GetButtonStateIndex(eControlState);

						return (LRESULT)(LPCTSTR)pcbox->m_himgBk[nStateIndex]->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						return (LRESULT)(LPCTSTR)pcbox->hwnd->ci.m_himgCombineBk->GetSafeImageName();
					}
				}
			}
			return 0;

			// 下沉事件处理中，也有WM_KEYDOWN和WM_CHAR消息的相同代码
			case WM_KEYDOWN:
				/*
				 * If the listbox is dropped and the ENTER key is pressed,
				 * close up the listbox successfully.  If ESCAPE is pressed,
				 * close it up like cancel.
				 */
				if (pcbox->fLBoxVisible)
				{
					if ((wParam == VK_RETURN) || (wParam == VK_ESCAPE))
					{
						xxxCBHideListBoxWindow(pcbox, TRUE, (wParam != VK_ESCAPE));
						InvalidateRect(hWnd, NULL);
						break;
					}
				}
			// FALL THROUGH

			case WM_CHAR:
				//L         if (fAnsi && IS_DBCS_ENABLED() && IsDBCSLeadByteEx(THREAD_CODEPAGE(), (BYTE)wParam)) {
				//             return ComboBoxDBCharHandler(pcbox, hWnd, message, wParam, lParam);
				//         }

				if (pcbox->fNoEdit)
				{
					goto CallListSendMessage;
				}
				else
				{
					goto CallEditSendMessage;
				}
				break;

			case CBM_SET_BUTTON_WIDTH:
				pe->bHandled = true;
				pcbox->m_nButtonWidth = wParam;
				xxxCBPosition(pcbox);
				return 0;

			case CBM_GET_BUTTON_WIDTH:
				pe->bHandled = true;
				return pcbox->m_nButtonWidth;

			case CBM_SET_BUTTON_MARGIN:
				pe->bHandled = true;
				pcbox->m_rcButtonMargin = (LPCRECT)lParam;
				xxxCBPosition(pcbox);
				return 0;

			case CBM_GET_BUTTON_MARGIN:
				pe->bHandled = true;
				if (lParam != NULL)
				{
					*((LPRECT)lParam) = pcbox->m_rcButtonMargin;
				}
				return 0;

			case CBM_SET_EDIT_MARGIN:
				pe->bHandled = true;
				pcbox->m_rcEditMargin = (LPCRECT)lParam;
				xxxCBPosition(pcbox);
				return 0;

			case CBM_GET_EDIT_MARGIN:
				pe->bHandled = true;
				if (lParam != NULL)
				{
					*((LPRECT)lParam) = pcbox->m_rcEditMargin;
				}
				return 0;

			case WLM_SET_BK_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					pcbox->m_crBk[nStateIndex] = cr;
					return 0;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_BK_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					return pcbox->m_crBk[nStateIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_BORDER_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					pcbox->m_crBorder[nStateIndex] = cr;
					return 0;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_BORDER_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					return pcbox->m_crBorder[nStateIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_TEXT_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					pcbox->m_crText[nStateIndex] = cr;
					return 0;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_TEXT_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					return pcbox->m_crText[nStateIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_TEXT_FONT:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				return IUISetControlFont4(GetProject(pcbox->hwnd),
						eControlState, pcbox->m_hIUIFont,
						(LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam);
			}
			break;

			case WLM_GET_TEXT_FONT:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					return pcbox->m_hIUIFont[0] == NULL ? NULL : (LRESULT)(LPCTSTR)pcbox->m_hIUIFont[0]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_HOVER)
				{
					return pcbox->m_hIUIFont[1] == NULL ? NULL : (LRESULT)(LPCTSTR)pcbox->m_hIUIFont[1]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_PRESSED)
				{
					return pcbox->m_hIUIFont[2] == NULL ? NULL : (LRESULT)(LPCTSTR)pcbox->m_hIUIFont[2]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					return pcbox->m_hIUIFont[3] == NULL ? NULL : (LRESULT)(LPCTSTR)pcbox->m_hIUIFont[3]->m_strFontResID;
				}
			}
			return 0;

			case CBM_SET_DRAW_DROP_BUTTON_BK_COLOR:
				pe->bHandled = true;
				pcbox->m_bDrawButtonBkColor = (BOOL)wParam;
				return 0;

			case CBM_GET_DRAW_DROP_BUTTON_BK_COLOR:
				pe->bHandled = true;
				return (LRESULT)pcbox->m_bDrawButtonBkColor;

			case CBM_SET_DROP_BUTTON_BK_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				if (eControlState == CONTROL_STATE_NORMAL)
				{
					pcbox->m_crButtonBk[0] = (COLORREF)lParam;
				}
				else if (eControlState == CONTROL_STATE_HOVER)
				{
					pcbox->m_crButtonBk[1] = (COLORREF)lParam;
				}
				else if (eControlState == CONTROL_STATE_PRESSED)
				{
					pcbox->m_crButtonBk[2] = (COLORREF)lParam;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					pcbox->m_crButtonBk[3] = (COLORREF)lParam;
				}
				else if (eControlState == CONTROL_STATE_SELECTED)
				{
					pcbox->m_crButtonBk[4] = (COLORREF)lParam;
				}
			}
			return 0;

			case CBM_GET_DROP_BUTTON_BK_COLOR:
				pe->bHandled = true;
				break;

			case CBM_SET_DROP_BUTTON_BK_IMAGE:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				if (eControlState == CONTROL_STATE_COMBINE)
				{
					IUISetControlImage(GetProject(pcbox->hwnd),
						&pcbox->m_himgCombineButtonBk, (LPCTSTR)lParam);
				}
				else
				{
					int nButtonStateIndex = GetButtonStateIndex(eControlState);
					_ASSERT(nButtonStateIndex >= 0 && nButtonStateIndex <= 5);
					IUISetControlImage(GetProject(pcbox->hwnd),
						&pcbox->m_himgButtonBk[nButtonStateIndex], (LPCTSTR)lParam);
				}
			}
			return 0;

			case CBM_GET_DROP_BUTTON_BK_IMAGE:
				pe->bHandled = true;
				break;

			case CBM_SET_DRAW_DROP_BUTTON_BORDER_COLOR:
			{
				pe->bHandled = true;
				BOOL bOld = pcbox->m_bDrawButtonBorderColor;
				pcbox->m_bDrawButtonBorderColor = wParam;
				return bOld;
			}

			case CBM_GET_DRAW_DROP_BUTTON_BORDER_COLOR:
				pe->bHandled = true;
				return pcbox->m_bDrawButtonBorderColor;

			case CBM_SET_DROP_BUTTON_BORDER_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				int nButtonStateIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nButtonStateIndex >= 0 && nButtonStateIndex <= 5);
				pcbox->m_crButtonBorder[nButtonStateIndex] = cr;

				return 0;
			}

			case CBM_GET_DROP_BUTTON_BORDER_COLOR:
			{
				pe->bHandled = true;
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nButtonStateIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nButtonStateIndex >= 0 && nButtonStateIndex <= 5);
				return pcbox->m_crButtonBorder[nButtonStateIndex];
			}

			case CBM_SET_DROP_HEIGHT:
				pcbox->cyDrop = lParam;
				{
					CIUIRect rcList;
					::GetWindowRect(pcbox->spwndList, rcList);
					rcList.bottom = rcList.top + pcbox->cyDrop;
					::SetWindowPos(pcbox->spwndList, NULL, 0, 0, rcList.Width(), rcList.Height(),
						SWP_NOMOVE | SWP_NOREDRAW);
				}

				// 通知ListBox，高度发生了变化
				::SendMessage(pcbox->spwndList, LBCB_DROPHEIGHT_CHANGED, 0, 0);
				return 0;

			case CBM_GET_DROP_HEIGHT:
				return pcbox->cyDrop;

			case CBM_GETDROPPEDWND:
				return (LRESULT)pcbox->spwndList;

			case CBM_SET_ITEM_BIND:
				message = LB_SET_ITEM_BIND;
				goto CallListSendMessage;
				break;

			case CBM_GET_ITEM_BIND:
				message = LB_GET_ITEM_BIND;
				goto CallListSendMessage;
				break;

			case WLM_SET_ITEM_BK_COLOR:
				if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						message = LB_SET_ITEM_BK_COLOR;
						goto CallListSendMessage;
					}
				}
				break;

			case WLM_GET_ITEM_BK_COLOR:
				if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						message = LB_GET_ITEM_BK_COLOR;
						goto CallListSendMessage;
					}
				}
				break;

			case CBEC_KILLCOMBOFOCUS:
			case 0x0167:	// get from SPY++

				/*
				 * Private message coming from editcontrol informing us that the combo
				 * box is losing the focus to a window which isn't in this combo box.
				 */
				xxxCBKillFocusHelper(pcbox);
				break;

			case WM_COMMAND:

				/*
				 * So that we can handle notification messages from the listbox and
				 * edit control.
				 */
				return xxxCBCommandHandler(pcbox, (DWORD)wParam, (HWND)lParam);

			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
			case WM_CTLCOLOR:
				//
				// Causes compatibility problems for 3.X apps.  Forward only
				// for 4.0
				//
				//L         if (TestWF(hWnd, WFWIN40COMPAT))
				// 		{
				//             TL tlpwndParent;
				//             LRESULT ret;
				//             HWND pwndParent;
				//
				//             pwndParent = REBASEPWND(hWnd, spwndParent);
				//             ThreadLock(pwndParent, &tlpwndParent);
				//             ret = SendMessage(HW(pwndParent), message, wParam, lParam);
				//             ThreadUnlock(tlpwndParent);
				//             return ret;
				//         }
				// 		else
			{
				return 0;
			}
			break;

			case WM_GETTEXT:
				if (pcbox->fNoEdit)
				{
					return xxxCBGetTextHelper(pcbox, (int)wParam, (LPWSTR)lParam, fAnsi);
				}
				goto CallEditSendMessage;
				break;

			case WM_GETTEXTLENGTH:

				/*
				 * If the is not edit control, CBS_DROPDOWNLIST, then we have to
				 * ask the list box for the size
				 */

				if (pcbox->fNoEdit)
				{
					return xxxCBGetTextLengthHelper(pcbox, fAnsi);
				}

			// FALL THROUGH

			case WM_CLEAR:
			case WM_CUT:
			case WM_PASTE:
			case WM_COPY:
			case WM_SETTEXT:
				goto CallEditSendMessage;
				break;

			case WM_ERASEBKGND:

				/*
				 * Just return 1L so that the background isn't erased
				 */
				return 1L;

			case WM_GETFONT:
				return (LRESULT)pcbox->hFont;

			case WM_GETDLGCODE:

				/*
				 * wParam - not used
				 * lParam - not used
				 */
			{
				LRESULT code = DLGC_WANTCHARS | DLGC_WANTARROWS;

				// If the listbox is dropped and the ENTER key is pressed,
				// we want this message so we can close up the listbox
				if (lParam != 0)
				{
					LPMSG lpMsg = (LPMSG)lParam;
					if (lpMsg->message == WM_KEYDOWN)
					{
						if (pcbox->fLBoxVisible)
						{
							if ((wParam == VK_RETURN) || (wParam == VK_ESCAPE))
							{
								code |= DLGC_WANTMESSAGE;
							}
						}
					}
				}
				return code;
			}
			/*
			 * No fall through
			 */

			case WM_SETFONT:
				xxxCBSetFontHandler(pcbox, (HANDLE)wParam, LOWORD(lParam));
				break;

			case WM_SETREDRAW:

				/*
				 * wParam - specifies state of the redraw flag.  nonzero = redraw
				 * lParam - not used
				 */

				/*
				 * effects: Sets the state of the redraw flag for this combo box
				 * and its children.
				 */
				pcbox->fNoRedraw = (UINT)!((BOOL)wParam);

				/*
				 * Must check pcbox->spwnEdit in case we get this message before
				 * WM_CREATE - PCBOX won't be initialized yet. (Eudora does this)
				 */
				if (!pcbox->fNoEdit && pcbox->spwndEdit)
				{
					//L            ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
					SendMessage(pcbox->spwndEdit, message, wParam, lParam);
					//            ThreadUnlock(&tlpwndEdit);
				}
				goto CallListSendMessage;
				break;

			case CB_GETDROPPEDSTATE:

				/*
				 * returns 1 if combo is dropped down else 0
				 * wParam - not used
				 * lParam - not used
				 */
				return pcbox->fLBoxVisible;

			case CB_GETDROPPEDCONTROLRECT:

				/*
				 * wParam - not used
				 * lParam - lpRect which will get the dropped down window rect in
				 *          screen coordinates.
				 */
			{
				RECT rcWindow;
				GetWindowRect(hWnd, &rcWindow);

				((LPRECT)lParam)->left      = rcWindow.left;
				((LPRECT)lParam)->top       = rcWindow.top;
				((LPRECT)lParam)->right     = rcWindow.left + max(pcbox->cxDrop, pcbox->cxCombo);
				((LPRECT)lParam)->bottom    = rcWindow.top + pcbox->cyCombo + pcbox->cyDrop;
			}
			break;

			case CB_SETDROPPEDWIDTH:
				if (pcbox->CBoxStyle & SDROPPABLE)
				{
					if (wParam)
					{
						wParam = max(wParam, (UINT)pcbox->cxCombo);

						if (wParam != (UINT) pcbox->cxDrop)
						{
							pcbox->cxDrop = (int)wParam;
							xxxCBPosition(pcbox);
						}
					}
				}
			// fall thru

			case CB_GETDROPPEDWIDTH:
				if (pcbox->CBoxStyle & SDROPPABLE)
				{
					return ((LRESULT) max(pcbox->cxDrop, pcbox->cxCombo));
				}
				else
				{
					return (CB_ERR);
				}
				break;

			case CB_DIR:
				/*
				 * wParam - Dos attribute value.
				 * lParam - Points to a file specification string
				 */
				//L         if (fAnsi && lParam != 0) {
				//             if (MBToWCS((LPSTR)lParam, -1, &lpwsz, -1, TRUE) == 0)
				//                 return CB_ERR;
				//             lParam = (LPARAM)lpwsz;
				//         }
				lReturn = xxxCBDir(pcbox, LOWORD(wParam), (LPWSTR)lParam);
				if (fAnsi && lParam != 0)
				{
					UserLocalFree(lpwsz);
				}
				return lReturn;

			case CB_SETEXTENDEDUI:

				/*
				 * wParam - specifies state to set extendui flag to.
				 * Currently only 1 is allowed.  Return CB_ERR (-1) if
				 * failure else 0 if success.
				 */
				if (pcbox->CBoxStyle & SDROPPABLE)
				{
					if (!wParam)
					{
						pcbox->fExtendedUI = 0;
						return 0;
					}

					if (wParam == 1)
					{
						pcbox->fExtendedUI = 1;
						return 0;
					}

					//             RIPERR1(ERROR_INVALID_PARAMETER,
					//                     RIP_WARNING,
					//                     "Invalid parameter \"wParam\" (%ld) to ComboBoxWndProcWorker",
					//                     wParam);

				}
				else
				{
					//             RIPERR1(ERROR_INVALID_MESSAGE,
					//                     RIP_WARNING,
					//                     "Invalid message (%ld) sent to ComboBoxWndProcWorker",
					//                     message);
				}

				return CB_ERR;

			case CB_GETEXTENDEDUI:
				if (pcbox->CBoxStyle & SDROPPABLE)
				{
					if (pcbox->fExtendedUI)
					{
						return TRUE;
					}
				}
				return FALSE;

			case CB_GETEDITSEL:

				/*
				 * wParam - not used
				 * lParam - not used
				 * effects: Gets the selection range for the given edit control.  The
				 * starting BYTE-position is in the low order word.  It contains the
				 * the BYTE-position of the first nonselected character after the end
				 * of the selection in the high order word.  Returns CB_ERR if no
				 * editcontrol.
				 */
				message = EM_GETSEL;
				goto CallEditSendMessage;
				break;

			case CB_LIMITTEXT:

				/*
				 * wParam - max number of bytes that can be entered
				 * lParam - not used
				 * effects: Specifies the maximum number of bytes of text the user may
				 * enter.  If maxLength is 0, we may enter MAXINT number of BYTES.
				 */
				message = EM_LIMITTEXT;
				goto CallEditSendMessage;
				break;

			case CB_SETEDITSEL:

				/*
				 * wParam - ichStart
				 * lParam - ichEnd
				 *
				 */
				message = EM_SETSEL;

				wParam = (int)(SHORT)LOWORD(lParam);
				lParam = (int)(SHORT)HIWORD(lParam);
				goto CallEditSendMessage;
				break;

			case CB_ADDSTRING:

				/*
				 * wParam - not used
				 * lParam - Points to null terminated string to be added to listbox
				 */
				if (!pcbox->fCase)
				{
					message = LB_ADDSTRING;
				}
				else
				{
					message = (pcbox->fCase & UPPERCASE) ? LB_ADDSTRINGUPPER : LB_ADDSTRINGLOWER;
				}
				goto CallListSendMessage;
				break;

			case CB_DELETESTRING:

				/*
				 * wParam - index to string to be deleted
				 * lParam - not used
				 */
				message = LB_DELETESTRING;
				goto CallListSendMessage;
				break;

			case CB_INITSTORAGE:
				// wParamLo - number of items
				// lParam - number of bytes of string space
				message = LB_INITSTORAGE;
				goto CallListSendMessage;

			case CB_SETTOPINDEX:
				// wParamLo - index to make top
				// lParam - not used
				message = LB_SETTOPINDEX;
				goto CallListSendMessage;

			case CB_GETTOPINDEX:
				// wParamLo / lParam - not used
				message = LB_GETTOPINDEX;
				goto CallListSendMessage;

			case CB_GETCOUNT:

				/*
				 * wParam - not used
				 * lParam - not used
				 */
				message = LB_GETCOUNT;
				goto CallListSendMessage;
				break;

			case CB_GETCURSEL:

				/*
				 * wParam - not used
				 * lParam - not used
				 */
				message = LB_GETCURSEL;
				goto CallListSendMessage;
				break;

			case CB_GETLBTEXT:

				/*
				 * wParam - index of string to be copied
				 * lParam - buffer that is to receive the string
				 */
				message = LB_GETTEXT;
				goto CallListSendMessage;
				break;

			case CB_GETLBTEXTLEN:

				/*
				 * wParam - index to string
				 * lParam - now used for cbANSI
				 */
				message = LB_GETTEXTLEN;
				goto CallListSendMessage;
				break;

			case CB_INSERTSTRING:

				/*
				 * wParam - position to receive the string
				 * lParam - points to the string
				 */
				if (!pcbox->fCase)
				{
					message = LB_INSERTSTRING;
				}
				else
				{
					message = (pcbox->fCase & UPPERCASE) ? LB_INSERTSTRINGUPPER : LB_INSERTSTRINGLOWER;
				}
				goto CallListSendMessage;
				break;

			case CB_RESETCONTENT:

				/*
				 * wParam - not used
				 * lParam - not used
				 * If we come here before WM_CREATE has been processed,
				 * pcbox->spwndList will be NULL.
				 */
				_ASSERT(pcbox->spwndList);
				//L        ThreadLock(pcbox->spwndList, &tlpwndList);
				::SendMessage(pcbox->spwndList, LB_RESETCONTENT, 0, 0);
				//ThreadUnlock(&tlpwndList);
				xxxCBInternalUpdateEditWindow(pcbox, NULL);
				break;

			case CB_GETHORIZONTALEXTENT:
				message = LB_GETHORIZONTALEXTENT;
				goto CallListSendMessage;

			case CB_SETHORIZONTALEXTENT:
				message = LB_SETHORIZONTALEXTENT;
				goto CallListSendMessage;

			case CB_FINDSTRING:

				/*
				 * wParam - index of starting point for search
				 * lParam - points to prefix string
				 */
				message = LB_FINDSTRING;
				goto CallListSendMessage;
				break;

			case CB_FINDSTRINGEXACT:

				/*
				 * wParam - index of starting point for search
				 * lParam - points to a exact string
				 */
				message = LB_FINDSTRINGEXACT;
				goto CallListSendMessage;
				break;

			case CB_SELECTSTRING:

				/*
				 * wParam - index of starting point for search
				 * lParam - points to prefix string
				 */
				_ASSERT(pcbox->spwndList);
				//L        ThreadLock(pcbox->spwndList, &tlpwndList);
				lParam = ::SendMessage(pcbox->spwndList, LB_SELECTSTRING,
						wParam, lParam);
				//        ThreadUnlock(&tlpwndList);
				xxxCBInternalUpdateEditWindow(pcbox, NULL);
				return lParam;

			case CB_SETCURSEL:

				/*
				 * wParam - Contains index to be selected
				 * lParam - not used
				 * If we come here before WM_CREATE has been processed,
				 * pcbox->spwndList will be NULL.
				 */

				_ASSERT(pcbox->spwndList);

				ThreadLock(pcbox->spwndList, &tlpwndList);
				lParam = ::SendMessage(pcbox->spwndList, LB_SETCURSEL, wParam, lParam);
				if (lParam != -1)
				{
					::SendMessage(pcbox->spwndList, LB_SETTOPINDEX, wParam, 0);
				}
				//        ThreadUnlock(&tlpwndList);
				xxxCBInternalUpdateEditWindow(pcbox, NULL);
				return lParam;

			case CB_GETITEMDATA:
				message = LB_GETITEMDATA;
				goto CallListSendMessage;
				break;

			case CB_SETITEMDATA:
				message = LB_SETITEMDATA;
				goto CallListSendMessage;
				break;

			case CB_SETITEMHEIGHT:
				if (wParam == -1)
				{
					if (HIWORD(lParam) != 0)
					{
						return CB_ERR;
					}
					return xxxCBSetEditItemHeight(pcbox, LOWORD(lParam));
				}

				message = LB_SETITEMHEIGHT;
				goto CallListSendMessage;
				break;

			case CB_GETITEMHEIGHT:
				if (wParam == -1)
				{
					return pcbox->editrc.bottom - pcbox->editrc.top;
				}

				message = LB_GETITEMHEIGHT;
				goto CallListSendMessage;
				break;

			case CB_SHOWDROPDOWN:

				/*
				 * wParam - True then drop down the listbox if possible else hide it
				 * lParam - not used
				 */
				if (wParam && !pcbox->fLBoxVisible)
				{
					xxxCBShowListBoxWindow(pcbox, TRUE);
				}
				else
				{
					if (!wParam && pcbox->fLBoxVisible)
					{
						xxxCBHideListBoxWindow(pcbox, TRUE, FALSE);
					}
				}
				break;

			case CB_SETLOCALE:

				/*
				 * wParam - locale id
				 * lParam - not used
				 */
				message = LB_SETLOCALE;
				goto CallListSendMessage;
				break;

			case CB_GETLOCALE:

				/*
				 * wParam - not used
				 * lParam - not used
				 */
				message = LB_GETLOCALE;
				goto CallListSendMessage;
				break;

			case WM_MEASUREITEM:
			case WM_DELETEITEM:
			case WM_DRAWITEM:
			case WM_COMPAREITEM:
				return xxxCBMessageItemHandler(pcbox, message, (LPVOID)lParam);


			case WM_PARENTNOTIFY:
				if (LOWORD(wParam) == WM_DESTROY)
				{
					if ((HWLWND)lParam == pcbox->spwndEdit)
					{
						pcbox->CBoxStyle &= ~SEDITABLE;
						pcbox->fNoEdit = TRUE;
						pcbox->spwndEdit = NULL;
					}
					else if ((HWND)lParam == pcbox->spwndList)
					{
						pcbox->CBoxStyle &= ~SDROPPABLE;
						pcbox->spwndList = NULL;
					}
				}
				break;

			case WM_UPDATEUISTATE:
				/*
				 * Propagate the change to the list control, if any
				 */
				_ASSERT(pcbox->spwndList);
				ThreadLock(pcbox->spwndList, &tlpwndList);
				::SendMessage(pcbox->spwndList, WM_UPDATEUISTATE,
					wParam, lParam);
				ThreadUnlock(&tlpwndList);
				goto CallDWP;

			//L     case WM_HELP:
			//         {
			//             LPHELPINFO lpHelpInfo;
			//
			//             /* Check if this message is form a child of this combo
			//              */
			//             if ((lpHelpInfo = (LPHELPINFO)lParam) != NULL &&
			//                 ((pcbox->spwndEdit && lpHelpInfo->iCtrlId == (SHORT)(PTR_TO_ID(pcbox->spwndEdit->spmenu))) ||
			//                  lpHelpInfo->iCtrlId == (SHORT)(PTR_TO_ID(pcbox->spwndList->spmenu)) )) {
			//
			//                 // BUGBUG - What to do here?
			//                 lpHelpInfo->iCtrlId = (SHORT)(PTR_TO_ID(hWnd->spmenu));
			//                 lpHelpInfo->hItemHandle = hWnd;
			//                 lpHelpInfo->dwContextId = GetContextHelpId(hWnd);
			//             }
			//         }
			/*
			 * Fall through to DefWindowProc
			 */

			default:

				if (GetSystemMetrics(SM_PENWINDOWS) &&
					(message >= WM_PENWINFIRST && message <= WM_PENWINLAST))
				{
					goto CallEditSendMessage;
				}

CallDWP:
				return DefEventHandler(hWnd, pe);
			}
		} // pe->eRoutingStrategy == RS_DIRECT

		return DefEventHandler(hWnd, pe);

		/*
		* The following forward messages off to the child controls.
		*/
CallEditSendMessage:
		if (pcbox != NULL && !pcbox->fNoEdit && pcbox->spwndEdit != NULL)
		{
			/*
			 * pcbox->spwndEdit will be NULL if we haven't done WM_CREATE yet!
			 */
			ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
			lReturn = SendMessage(pcbox->spwndEdit, message, wParam, lParam);
			ThreadUnlock(&tlpwndEdit);
		}
		else
		{
			//L         RIPERR0(ERROR_INVALID_COMBOBOX_MESSAGE, RIP_VERBOSE, "");
			lReturn = CB_ERR;
		}
		return lReturn;

CallListSendMessage:
		if (pcbox != NULL)
		{
			/*
			 * pcbox->spwndList will be NULL if we haven't done WM_CREATE yet!
			 */
			_ASSERT(pcbox->spwndList);
			ThreadLock(pcbox->spwndList, &tlpwndList);
			lReturn = ::SendMessage(pcbox->spwndList, message, wParam, lParam);
			ThreadUnlock(&tlpwndList);
		}

		return lReturn;
	}

	if (pcbox != NULL && pe->hOriginalSender == pcbox->spwndEdit)
	{
		// pe->hOriginalSender == hWnd已在上面的分支return了。
		_ASSERT(pe->hOriginalSender != hWnd);

		switch (message)
		{
		case WM_COMMAND:

			/*
			 * So that we can handle notification messages from the listbox and
			 * edit control.
			 */
			return xxxCBCommandHandler(pcbox, (DWORD)wParam, (HWND)lParam);

		}
	}

	return DefEventHandler(hWnd, pe);

}  /* ComboBoxWndProcWorker */


/***************************************************************************\
* xxxCBMessageItemHandler
*
* Handles WM_DRAWITEM,WM_MEASUREITEM,WM_DELETEITEM,WM_COMPAREITEM
* messages from the listbox.
*
* History:
\***************************************************************************/

LRESULT xxxCBMessageItemHandler(
	PCBOX pcbox,
	UINT message,
	LPVOID lpfoo)  /* Actually can be any of the structs below */
{
	LRESULT lRet;
	TL tlpwndParent;

	CheckLock(pcbox->hwnd);

	/*
	 * Send the <foo>item message back to the application after changing some
	 * parameters to their combo box specific versions.
	 */
	((LPMEASUREITEMSTRUCT)lpfoo)->CtlType = ODT_COMBOBOX;
	//HMENU hMenu = GetMenu(pcbox->spwnd);
	//((LPMEASUREITEMSTRUCT)lpfoo)->CtlID = PtrToUlong(hMenu);
	if (message == WM_DRAWITEM)
	{
		((LPDRAWITEMSTRUCT)lpfoo)->hwndItem = (HWND)pcbox->hwnd;
	}
	else if (message == WM_DELETEITEM)
	{
		((LPDELETEITEMSTRUCT)lpfoo)->hwndItem = (HWND)pcbox->hwnd;
	}
	else if (message == WM_COMPAREITEM)
	{
		((LPCOMPAREITEMSTRUCT)lpfoo)->hwndItem = (HWND)pcbox->hwnd;
	}

	ThreadLock(pcbox->pwndParent, &tlpwndParent);
	lRet = SendMessage(pcbox->hwnd->ci.hwndParent, message, (WPARAM)/*hMenu*/0, (LPARAM)lpfoo);
	ThreadUnlock(&tlpwndParent);

	return lRet;
}

/***************************************************************************\
* xxxCBCommandHandler
*
* Check the various notification codes from the controls and do the
* proper thing.
* always returns 0L
*
* History:
\***************************************************************************/

long xxxCBCommandHandler(
	PCBOX pcbox,
	DWORD wParam,
	HWND hwndControl)
{

	CheckLock(pcbox->hwnd);

	/*
	 * Check the edit control notification codes.  Note that currently, edit
	 * controls don't send EN_KILLFOCUS messages to the parent.
	 */
	if (!pcbox->fNoEdit &&
		SAMEWOWHANDLE(hwndControl, pcbox->spwndEdit))
	{

		/*
		 * Edit control notification codes
		 */
		switch (HIWORD(wParam))
		{
		case EN_SETFOCUS:
			if (!pcbox->fFocus)
			{

				/*
				 * The edit control has the focus for the first time which means
				 * this is the first time the combo box has received the focus
				 * and the parent must be notified that we have the focus.
				 */
				xxxCBGetFocusHelper(pcbox);
			}
			break;

		case EN_CHANGE:
			xxxCBNotifyParent(pcbox, CBN_EDITCHANGE);
			xxxCBUpdateListBoxWindow(pcbox, FALSE);
			break;

		case EN_UPDATE:
			xxxCBNotifyParent(pcbox, CBN_EDITUPDATE);
			break;

		case EN_ERRSPACE:
			xxxCBNotifyParent(pcbox, CBN_ERRSPACE);
			break;
		}
	}

	/*
	 * Check listbox control notification codes
	 */
	if (SAMEWOWHANDLE(hwndControl, pcbox->spwndList))
	{

		/*
		 * Listbox control notification codes
		 */
		int nEvent = (int)HIWORD(wParam);
		switch (nEvent)
		{
		case LBN_DBLCLK:
			xxxCBNotifyParent(pcbox, CBN_DBLCLK);
			break;

		case LBN_ERRSPACE:
			xxxCBNotifyParent(pcbox, CBN_ERRSPACE);
			break;

		case LBN_SELCHANGE:
		case LBN_SELCANCEL:
			if (!pcbox->fKeyboardSelInListBox)
			{

				/*
				 * If the selchange is caused by the user keyboarding through,
				 * we don't want to hide the listbox.
				 */
				if (!xxxCBHideListBoxWindow(pcbox, TRUE, TRUE))
				{
					return (0L);
				}
			}
			else
			{
				pcbox->fKeyboardSelInListBox = FALSE;
			}

			xxxCBNotifyParent(pcbox, CBN_SELCHANGE);
			xxxCBInternalUpdateEditWindow(pcbox, NULL);
			break;
		}
	}

	return 0L;
}


/***************************************************************************\
* xxxCBNotifyParent
*
* Sends the notification code to the parent of the combo box control
*
* History:
\***************************************************************************/

void xxxCBNotifyParent(
	PCBOX pcbox,
	short notificationCode)
{
	CheckLock(pcbox->hwnd);

	/*
	 * wParam contains Control ID and notification code.
	 * lParam contains Handle to window
	 */
	//HMENU hMenu = GetMenu(pcbox->spwnd);

	RoutedEventArgs args;
	args.RoutedEvent = WM_COMMAND;
	args.eRoutingStrategy = RS_BUBBLE;
	args.hSender = args.hOriginalSender = pcbox->hwnd;
	args.wParam = MAKELONG(PTR_TO_ID(/*hMenu*/0), notificationCode);
	args.lParam = (LPARAM)pcbox->hwnd;
	RaiseEvent(pcbox->hwnd, &args);
}

/***************************************************************************\
*
*
* Completes the text in the edit box with the closest match from the
* listbox.  If a prefix match can't be found, the edit control text isn't
* updated. Assume a DROPDOWN style combo box.
*
*
* History:
\***************************************************************************/
void xxxCBCompleteEditWindow(
	PCBOX pcbox)
{
	int cchText;
	int cchItemText;
	int itemNumber;
	LPWSTR pText;
	TL tlpwndEdit;
	TL tlpwndList;

	CheckLock(pcbox->hwnd);

	/*
	 * Firstly check the edit control.
	 */
	if (!IsWindow(pcbox->spwndEdit))
	{
		return;
	}

	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
	ThreadLock(pcbox->spwndList, &tlpwndList);

	/*
	 * +1 for null terminator
	 */
	cchText = (int)SendMessage(pcbox->spwndEdit, WM_GETTEXTLENGTH, 0, 0);

	if (cchText)
	{
		cchText++;
		if (!(pText = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, cchText * sizeof(WCHAR))))
		{
			goto Unlock;
		}

		/*
		 * We want to be sure to free the above allocated memory even if
		 * the client dies during callback (xxx) or some of the following
		 * window revalidation fails.
		 */
		__try
		{
			SendMessage(pcbox->spwndEdit, WM_GETTEXT, cchText, (LPARAM)pText);
			itemNumber = (int)SendMessage(pcbox->spwndList,
					LB_FINDSTRINGEXACT, (WPARAM)(-1), (LPARAM)pText);
			if (itemNumber == -1)
			{
				itemNumber = (int)SendMessage(pcbox->spwndList,
						LB_FINDSTRING, (WPARAM)(-1), (LPARAM)pText);
			}
		}
		__finally
		{
			UserLocalFree((HANDLE)pText);
		}

		if (itemNumber == -1)
		{

			/*
			 * No close match.  Blow off.
			 */
			goto Unlock;
		}

		cchItemText = (int)SendMessage(pcbox->spwndList, LB_GETTEXTLEN, itemNumber, 0);
		if (cchItemText)
		{
			cchItemText++;
			if (!(pText = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, cchItemText * sizeof(WCHAR))))
			{
				goto Unlock;
			}

			/*
			 * We want to be sure to free the above allocated memory even if
			 * the client dies during callback (xxx) or some of the following
			 * window revalidation fails.
			 */
			__try
			{
				SendMessage(pcbox->spwndList, LB_GETTEXT,
					itemNumber, (LPARAM)pText);
				SendMessage(pcbox->spwndEdit, WM_SETTEXT, 0, (LPARAM)pText);
			}
			__finally
			{
				UserLocalFree((HANDLE)pText);
			}

			SendMessage(pcbox->spwndEdit, EM_SETSEL, 0, MAXLONG);
		}
	}

Unlock:
	ThreadUnlock(&tlpwndList);
	ThreadUnlock(&tlpwndEdit);
}


/***************************************************************************\
* xxxCBHideListBoxWindow
*
* Hides the dropdown listbox window if it is a dropdown style.
*
* History:
\***************************************************************************/

BOOL xxxCBHideListBoxWindow(
	PCBOX pcbox,
	BOOL fNotifyParent,
	BOOL fSelEndOK)
{
	HWLWND hwnd = pcbox->hwnd;
	HWND hwndList = pcbox->spwndList;
	TL tlpwndList;


	CheckLock(pcbox->hwnd);

	//L     // For 3.1+ apps, send CBN_SELENDOK to all types of comboboxes but only
	//     // allow CBN_SELENDCANCEL to be sent for droppable comboboxes
	//     if (fNotifyParent && TestWF(pcbox->spwnd, WFWIN31COMPAT) &&
	//         ((pcbox->CBoxStyle & SDROPPABLE) || fSelEndOK)) {
	//         if (fSelEndOK)
	//         {
	//             xxxCBNotifyParent(pcbox, CBN_SELENDOK);
	//         }
	//         else
	//         {
	//             xxxCBNotifyParent(pcbox, CBN_SELENDCANCEL);
	//         }
	//         if (!IsWindow(hwnd))
	//             return(FALSE);
	//     }

	/*
	 * return, we don't hide simple combo boxes.
	 */
	if (!(pcbox->CBoxStyle & SDROPPABLE))
	{
		return TRUE;
	}

	/*
	 * Send a faked buttonup message to the listbox so that it can release
	 * the capture and all.
	 */
	ThreadLock(pcbox->spwndList, &tlpwndList);

	SendMessage(pcbox->spwndList, LBCB_ENDTRACK, fSelEndOK, 0);

	if (pcbox->fLBoxVisible)
	{
		WORD swpFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

		//L         if (!TestWF(pcbox->spwnd, WFWIN31COMPAT))
		swpFlags |= SWP_FRAMECHANGED;

		pcbox->fLBoxVisible = FALSE;

		/*
		 * Hide the listbox window
		 */
		ShowWindow(hwndList, SW_HIDE);

		//
		// Invalidate the item area now since SWP() might update stuff.
		// Since the combo is CS_VREDRAW/CS_HREDRAW, a size change will
		// redraw the whole thing, including the item rect.  But if it
		// isn't changing size, we still want to redraw the item anyway
		// to show focus/selection.
		//
		if (!(pcbox->CBoxStyle & SEDITABLE))
		{
			InvalidateRect(hwnd, &pcbox->editrc);
		}

		SetWindowPos(hwnd, HWND_TOP, 0, 0,
			pcbox->cxCombo, pcbox->cyCombo, swpFlags);

		// In case size didn't change
		UpdateWindow(GetHostWnd(hwnd));

		if (pcbox->CBoxStyle & SEDITABLE)
		{
			xxxCBCompleteEditWindow(pcbox);
		}

		if (fNotifyParent)
		{

			/*
			 * Notify parent we will be popping up the combo box.
			 */
			xxxCBNotifyParent(pcbox, CBN_CLOSEUP);
			if (!IsWindow(hwnd))
			{
				return (FALSE);
			}
		}
	}

	ThreadUnlock(&tlpwndList);

	return (TRUE);
}

/***************************************************************************\
* xxxCBShowListBoxWindow
*
* Lowers the dropdown listbox window.
* 显示ComboBox下拉列表，下拉列表根据Item个数，高度会适当缩小
* History:
\***************************************************************************/

void xxxCBShowListBoxWindow(
	PCBOX pcbox, BOOL fTrack)
{
	int         itemNumber;
	int         yTop;
	DWORD       dwMult;
	int         cyItem;
	HWLWND hwnd = pcbox->hwnd;
	HWND        hwndList = pcbox->spwndList;
	BOOL        fAnimPos;
	TL          tlpwndList;
	HMONITOR    hMonitor;

	//
	// THIS FUNCTION IS ONLY CALLED FOR DROPPABLE LIST COMBOBOXES
	//
	_ASSERT(pcbox->CBoxStyle & SDROPPABLE);

	CheckLock(pcbox->hwnd);

	ThreadLock(pcbox->spwndList, &tlpwndList);

	/*
	 * Notify parent we will be dropping down the combo box.
	 */
	xxxCBNotifyParent(pcbox, CBN_DROPDOWN);

	/*
	 * Invalidate the button rect so that the depressed arrow is drawn.
	 */
	InvalidateRect(hwnd, &pcbox->buttonrc);

	pcbox->fLBoxVisible = TRUE;

	if (pcbox->CBoxStyle == SDROPDOWN)
	{
		// 如果ListBox中的一个Item文本内容与ComboBox输入框中的文本匹配，
		// 把ListBox中的这个Item滚动到最上。 仅在鼠标未按下时选中Item,
		// 否则在鼠标抬起时，选中Item.
		xxxCBUpdateListBoxWindow(pcbox, !pcbox->fMouseDown);
		if (!pcbox->fMouseDown)
		{
			xxxCBCompleteEditWindow(pcbox);
		}
	}
	else
	{

		/*
		 * Scroll the currently selected item to the top of the listbox.
		 */
		itemNumber = (int)SendMessage(pcbox->spwndList, LB_GETCURSEL,
				0, 0);
		if (itemNumber == -1)
		{
			itemNumber = 0;
		}
		SendMessage(pcbox->spwndList, LB_SETTOPINDEX, itemNumber, 0);
		SendMessage(pcbox->spwndList, LBCB_CARETON, 0, 0);

		/*
		 * We need to invalidate the edit rect so that the focus frame/invert
		 * will be turned off when the listbox is visible.  Tandy wants this for
		 * his typical reasons...
		 */
		InvalidateRect(hwnd, &pcbox->editrc);
	}

	//
	// Figure out where to position the dropdown listbox.  We want it just
	// touching the edge around the edit rectangle.  Note that since the
	// listbox is a popup, we need the position in screen coordinates.
	//

	// We want the dropdown to pop below or above the combo

	// 得到ComboBox屏幕坐标
	RECT rcWindow;
	GetWindowRect(pcbox->hwnd, &rcWindow);

	// List area
	int cyEdge = GetSystemMetrics(SM_CYBORDER);
	cyItem = (int)SendMessage(pcbox->spwndList, LB_GETITEMHEIGHT, 0, 0);

	if (cyItem == 0)
	{
		// Make sure that it's not 0
		//         RIPMSG0( RIP_WARNING, "LB_GETITEMHEIGHT is returning 0\n" );

		cyItem = 24;//L gpsi->cySysFontChar;
	}

	//  we shoulda' just been able to use cyDrop here, but thanks to VB's need
	//  to do things their OWN SPECIAL WAY, we have to keep monitoring the size
	//  of the listbox 'cause VB changes it directly (jeffbog 03/21/94)

	// 重新计算ListBox的高度
	int iHeight = max(pcbox->cyDrop, rcWindow.bottom - rcWindow.top);

	if (dwMult = (DWORD)SendMessage(pcbox->spwndList, LB_GETCOUNT, 0, 0))
	{
		dwMult = (DWORD)(LOWORD(dwMult) * cyItem);
		dwMult += (2 * cyEdge);

		if (dwMult < 0x7FFF)
		{
			iHeight = min(LOWORD(dwMult), iHeight);
		}
	}

	// 把ListBox高度归整到Item高度整数倍。
	_ASSERT(cyItem);
	iHeight = ((iHeight - 2 * cyEdge) / cyItem) * cyItem + 2 * cyEdge;

	//
	// Other 1/2 of old app combo fix.  Make dropdown overlap combo window
	// a little.  That way we can have a chance of invalidating the overlap
	// and causing a repaint to help out Publisher 2.0's toolbar combos.
	// See comments for PressButton() above.
	// 如果下拉列表无法完整显示到ComboBox下方，则把它弹到ComboBox上方。
	//
	CIUIRect rcBorder = GetBorderRect(pcbox->hwnd);
	hMonitor = MonitorFromWindow(GetHostWnd(pcbox->hwnd), MONITOR_DEFAULTTOPRIMARY);
	if (hMonitor == NULL)
	{
		return;
	}

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &mi);

	if (rcWindow.bottom + iHeight <= mi.rcMonitor.bottom)
	{
		yTop = rcWindow.bottom;
		yTop -= rcBorder.bottom;

		fAnimPos = TRUE;
	}
	else
	{
		yTop = max(rcWindow.top - iHeight, mi.rcMonitor.top);
		yTop += rcBorder.top;

		fAnimPos = FALSE;
	}

	//L    if ( ! TestWF( pcbox->spwnd, WFWIN40COMPAT) )
	{
		// fix for Winword B#7504, Combo-ListBox text gets
		// truncated by a small width, this is do to us
		// now setting size here in SetWindowPos, rather than
		// earlier where we did this in Win3.1

		if ((rcWindow.right - rcWindow.left) > pcbox->cxDrop)
		{
			pcbox->cxDrop = rcWindow.right - rcWindow.left;
		}
	}

	// 重新设置下拉列表的坐标
	SetWindowPos(hwndList, HWND_TOPMOST, rcWindow.left,
		yTop, max(pcbox->cxDrop, pcbox->cxCombo), iHeight, SWP_NOACTIVATE);

	/*
	 * Get any drawing in the combo box window out of the way so it doesn't
	 * invalidate any of the SPB underneath the list window.
	 */
	UpdateWindow(GetHostWnd(hwnd));

	//L     if (!(TEST_EffectPUSIF(PUSIF_COMBOBOXANIMATION))
	//         || (GetAppCompatFlags2(VER40) & GACF2_ANIMATIONOFF)) {
	ShowWindow(hwndList, SW_SHOWNA);
	//     } else {
	//         AnimateWindow(hwndList, CMS_QANIMATION, (fAnimPos ? AW_VER_POSITIVE :
	//                 AW_VER_NEGATIVE) | AW_SLIDE);
	//     }

#ifdef LATER
	//
	// we don't have sys modal windows.
	//
	if (pwndSysModal)
	{

		/*
		 * If this combo is in a system modal dialog box, we need to explicitly
		 * call update window otherwise we won't automatically send paint
		 * messages to the toplevel listbox window.  This is especially
		 * noticeable in the File Open/Save sys modal dlgs which are put up at
		 * ExitWindows time.
		 */
		UpdateWindow(hwndList);
	}
#endif

	/*
	 * Restart search buffer from first char
	 */
	{
		// 		PLBIV plb = (PLBIV)GetWindowLong(pcbox->spwndList, 0);
		//
		// 		if ((plb != NULL) && (plb != (PLBIV)(-1)))
		// 		{
		// 			plb->iTypeSearch = 0;
		// 		}
	}

	if (fTrack/* && TestWF(pcbox->spwnd, WFWIN40COMPAT)*/)
	{
		SendMessage(pcbox->spwndList, LBCB_STARTTRACK, FALSE, 0);
	}

	ThreadUnlock(&tlpwndList);
}

/***************************************************************************\
* xxxCBInternalUpdateEditWindow
*
* Updates the editcontrol/statictext window so that it contains the text
* given by the current selection in the listbox.  If the listbox has no
* selection (ie. -1), then we erase all the text in the editcontrol.
*
* hdcPaint is from WM_PAINT messages Begin/End Paint hdc. If null, we should
* get our own dc.
*
* History:
\***************************************************************************/

void xxxCBInternalUpdateEditWindow(
	PCBOX pcbox,
	HDC hdcPaint)
{
	int cchText = 0;
	LPWSTR pText = NULL;
	int sItem;
	HDC hdc = NULL;
	UINT msg;
	HBRUSH hbrSave;
	HBRUSH hbrControl;
	HANDLE hOldFont = NULL;
	DRAWITEMSTRUCT dis;
	RECT rc;
	HWLWND hwnd = pcbox->hwnd;
	TL tlpwndList;
	TL tlpwndEdit;
	TL tlpwndParent;

	CheckLock(pcbox->hwnd);

	/* This check is also commented out in Win3.1 and Win95 */
	// if (!TestWF(pcbox->spwnd, WFVISIBLE)) {
	//    return;
	// }

	ThreadLock(pcbox->pwndParent, &tlpwndParent);
	ThreadLock(pcbox->spwndList, &tlpwndList);
	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);

	sItem = (int)SendMessage(pcbox->spwndList, LB_GETCURSEL, 0, 0);

	/*
	 * This 'try-finally' block ensures that the allocated 'pText' will
	 * be freed no matter how this routine is exited.
	 */
	__try
	{
		if (sItem != -1)
		{
			cchText = (int)SendMessage(pcbox->spwndList, LB_GETTEXTLEN,
					(DWORD)sItem, 0);
			if ((pText = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, (cchText + 1) * sizeof(WCHAR))))
			{
				cchText = (int)SendMessage(pcbox->spwndList, LB_GETTEXT,
						(DWORD)sItem, (LPARAM)pText);
			}
		}

		if (!pcbox->fNoEdit)
		{

			if (pcbox->spwndEdit)
			{
				LONG lStyle = GetStyle(pcbox->hwnd);
				if (lStyle & CBS_HASSTRINGS)
				{
					// 代替pcbox->spwndEdit->SetSel(0, MAXLONG);
					CHARRANGE cr;
					cr.cpMin = 0;
					cr.cpMax = MAXLONG;
					SendMessage(pcbox->spwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);

					// 代替pcbox->spwndEdit->ReplaceSel(pText ? pText : L"");
					SendMessage(pcbox->spwndEdit, EM_REPLACESEL,
						(WPARAM)FALSE, (LPARAM)(pText ? pText : L""));
				}

				if (pcbox->fFocus)
				{
					/*
					 * Only hilite the text if we have the focus.
					 */
					SendMessage(pcbox->spwndEdit, EM_SETSEL, 0, MAXLONG);
				}
			}
		}
		else if (IsComboVisible(pcbox))
		{
			if (hdcPaint)
			{
				hdc = hdcPaint;
			}
			else
			{
				_ASSERT(FALSE);
				//hdc = GetDC(hwnd);
			}

			SetBkMode(hdc, OPAQUE);
			//L             if (TestWF(pcbox->spwnd, WFWIN40COMPAT)) {
			//                 if (TestWF(pcbox->spwnd, WFDISABLED))
			//                     msg = WM_CTLCOLORSTATIC;
			//                 else
			//                     msg = WM_CTLCOLOREDIT;
			//             } else
			msg = WM_CTLCOLORLISTBOX;

			hbrControl = NULL;//L GetControlBrush(hwnd, hdc, msg);
			hbrSave = (HBRUSH)SelectObject(hdc, hbrControl);

			CopyInflateRect(&rc, &pcbox->editrc, GetSystemMetrics(SM_CXBORDER), 8);
			PatBlt(hdc, rc.left, rc.top, rc.right - rc.left,
				rc.bottom - rc.top, PATCOPY);
			InflateRect(&rc, -GetSystemMetrics(SM_CXBORDER), -8);

			if (pcbox->fFocus && !pcbox->fLBoxVisible)
			{
				//
				// Fill in the selected area
				//


				// only do the FillRect if we know its not
				// ownerdraw item, otherwise we mess up people up
				// BUT: for Compat's sake we still do this for Win 3.1 guys

				if (/*!TestWF( pcbox->spwnd, WFWIN40COMPAT) || */!pcbox->OwnerDraw)
				{
					FillRect(hdc, &rc, NULL);
				}

				SetBkColor(hdc, RGB(255, 0, 0));
				SetTextColor(hdc, RGB(255, 255, 0));
			}
			else if (!IsWindowEnabled(pcbox->hwnd) && !pcbox->OwnerDraw)
			{
				if (RGB(128, 128, 128) != GetBkColor(hdc))
				{
					SetTextColor(hdc, RGB(128, 128, 128));
				}
			}

			if (pcbox->hFont != NULL)
			{
				hOldFont = SelectObject(hdc, pcbox->hFont);
			}

			if (pcbox->OwnerDraw)
			{

				/*
				 * Let the app draw the stuff in the static text box.
				 */
				dis.CtlType = ODT_COMBOBOX;
				dis.CtlID = 0;//PtrToUlong(GetMenu(pcbox->hwnd));
				dis.itemID = sItem;
				dis.itemAction = ODA_DRAWENTIRE;
				dis.itemState = (UINT)
					((pcbox->fFocus && !pcbox->fLBoxVisible ? ODS_SELECTED : 0) |
						(!IsWindowEnabled(pcbox->hwnd) ? ODS_DISABLED : 0) |
						(pcbox->fFocus && !pcbox->fLBoxVisible ? ODS_FOCUS : 0) |
						0/*L |
                    (TestWF(pcbox->spwnd, WEFPUIFOCUSHIDDEN) ? ODS_NOFOCUSRECT : 0) |
                    (TestWF(pcbox->spwnd, WEFPUIACCELHIDDEN) ? ODS_NOACCEL : 0)*/);

				dis.hwndItem = (HWND)hwnd;
				dis.hDC = hdc;
				CopyRect(&dis.rcItem, &rc);

				// Don't let ownerdraw dudes draw outside of the combo client
				// bounds.
				IntersectClipRect(hdc, rc.left, rc.top, rc.right, rc.bottom);

				dis.itemData = (ULONG_PTR)SendMessage(pcbox->spwndList,
						LB_GETITEMDATA, (UINT)sItem, 0);

				SendMessage(pcbox->hwnd->ci.hwndParent, WM_DRAWITEM, dis.CtlID,
					(LPARAM)&dis);
			}
			else
			{

				/*
				 * Start the text one pixel within the rect so that we leave a
				 * nice hilite border around the text.
				 */

				int x ;
				UINT align ;

				//                if (pcbox->fRightAlign )
				{
					align = TA_RIGHT;
					x = rc.right - GetSystemMetrics(SM_CXBORDER);
				}
				// 				else {
				//                     x = rc.left + SYSMET(CXBORDER);
				//                     align = 0;
				//                 }

				//                 if (pcbox->fRtoLReading )
				//                     align |= TA_RTLREADING;

				if (align)
				{
					SetTextAlign(hdc, GetTextAlign(hdc) | align);
				}

				// Draw the text, leaving a gap on the left & top for selection.
				ExtTextOutW(hdc, x, rc.top + 8, ETO_CLIPPED | ETO_OPAQUE,
					&rc, pText ? pText : L"", cchText, NULL);
				if (pcbox->fFocus && !pcbox->fLBoxVisible)
				{
					//L                     if (!TestWF(pcbox->spwnd, WEFPUIFOCUSHIDDEN)) {
					//                         DrawFocusRect(hdc, &rc);
					//                     }
				}
			}

			if (pcbox->hFont && hOldFont)
			{
				SelectObject(hdc, hOldFont);
			}

			if (hbrSave)
			{
				SelectObject(hdc, hbrSave);
			}

			//if (!hdcPaint)
			//{
			//	ReleaseDC(hwnd, hdc);
			//}
		}

	}
	__finally
	{
		if (pText != NULL)
		{
			UserLocalFree((HANDLE)pText);
		}
	}

	ThreadUnlock(&tlpwndEdit);
	ThreadUnlock(&tlpwndList);
	ThreadUnlock(&tlpwndParent);
}

/***************************************************************************\
* xxxCBInvertStaticWindow
*
* Inverts the static text/picture window associated with the combo
* box.  Gets its own hdc, if the one given is null.
*
* History:
\***************************************************************************/

void xxxCBInvertStaticWindow(
	PCBOX pcbox,
	BOOL fNewSelectionState,  /* True if inverted else false */
	HDC hdc)
{
	BOOL focusSave = pcbox->fFocus;

	CheckLock(pcbox->hwnd);

	pcbox->fFocus = (UINT)fNewSelectionState;
	xxxCBInternalUpdateEditWindow(pcbox, hdc);

	pcbox->fFocus = (UINT)focusSave;
}

/***************************************************************************\
* xxxCBUpdateListBoxWindow
*
* 匹配ComboBox文本控件中的文本。 如果fSelectionAlso为FALSE, 将不会选中ListBox
* 中的Item，但会把最接近文本控件中的文本的那项，设置为Caret项（焦点项）。
*
* History:
\***************************************************************************/

void xxxCBUpdateListBoxWindow(
	PCBOX pcbox,
	BOOL fSelectionAlso)
{
	TL tlpwndEdit;
	TL tlpwndList;

	if (pcbox->spwndEdit == NULL)
	{
		return;
	}

	CheckLock(pcbox->hwnd);

	ThreadLock(pcbox->spwndList, &tlpwndList);
	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);

	int sItem = -1;
	int sSel = -1;
	int cchText = (int)SendMessage(pcbox->spwndEdit, WM_GETTEXTLENGTH, 0, 0);
	if (cchText > 0)
	{
		cchText++;

		LPWSTR pText = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, cchText * sizeof(WCHAR));
		if (pText != NULL)
		{
			__try
			{
				SendMessage(pcbox->spwndEdit, WM_GETTEXT, cchText, (LPARAM)pText);
				sItem = (int)SendMessage(pcbox->spwndList, LB_FINDSTRING,
						(WPARAM)(-1L), (LPARAM)pText);
			}
			__finally
			{
				UserLocalFree((HANDLE)pText);
			}
		}
	}
	else
	{
		sItem = -1;
	}

	if (fSelectionAlso)
	{
		sSel = sItem;
	}
	else
	{
		sSel = -1;
	}

	if (sItem == -1)
	{
		sItem = 0;

		//
		// Old apps:  w/ editable combos, selected 1st item in list even if
		// it didn't match text in edit field.  This is not desirable
		// behavior for 4.0 dudes esp. with cancel allowed.  Reason:
		//      (1) User types in text that doesn't match list choices
		//      (2) User drops combo
		//      (3) User pops combo back up
		//      (4) User presses OK in dialog that does stuff w/ combo
		//          contents.
		// In 3.1, when the combo dropped, we'd select the 1st item anyway.
		// So the last CBN_SELCHANGE the owner got would be 0--which is
		// bogus because it really should be -1.  In fact if you type anything
		// into the combo afterwards it will reset itself to -1.
		//
		// 4.0 dudes won't get this bogus 0 selection.
		//
		if (fSelectionAlso/* && !TestWF(pcbox->spwnd, WFWIN40COMPAT)*/)
		{
			sSel = 0;
		}
	}


	SendMessage(pcbox->spwndList, LB_SETCURSEL, (DWORD)sSel, 0);
	SendMessage(pcbox->spwndList, LB_SETCARETINDEX, (DWORD)sItem, 0);
	SendMessage(pcbox->spwndList, LB_SETTOPINDEX, (DWORD)sItem, 0);

	ThreadUnlock(&tlpwndEdit);
	ThreadUnlock(&tlpwndList);
}

/***************************************************************************\
* xxxCBGetFocusHelper
*
* Handles getting the focus for the combo box
*
* History:
\***************************************************************************/

void xxxCBGetFocusHelper(
	PCBOX pcbox)
{
	TL tlpwndList;
	TL tlpwndEdit;

	CheckLock(pcbox->hwnd);

	if (pcbox->fFocus)
	{
		return;
	}

	ThreadLock(pcbox->spwndList, &tlpwndList);
	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);

	/*
	 * The combo box has gotten the focus for the first time.
	 */

	/*
	 * First turn on the listbox caret
	 */

	if (pcbox->CBoxStyle == SDROPDOWNLIST)
	{
		SendMessage(pcbox->spwndList, LBCB_CARETON, 0, 0);
	}

	/*
	 * and select all the text in the editcontrol or static text rectangle.
	 */

	if (pcbox->fNoEdit)
	{

		/*
		 * Invert the static text rectangle
		 */
		xxxCBInvertStaticWindow(pcbox, TRUE, (HDC)NULL);
	}
	else if (pcbox->spwndEdit)
	{
		_ASSERT(pcbox->hwnd);
		SendMessage(pcbox->spwndEdit, EM_SETSEL, 0, MAXLONG);
	}

	pcbox->fFocus = TRUE;

	/*
	 * Notify the parent we have the focus
	 */
	xxxCBNotifyParent(pcbox, CBN_SETFOCUS);

	ThreadUnlock(&tlpwndEdit);
	ThreadUnlock(&tlpwndList);
}

/***************************************************************************\
* xxxCBKillFocusHelper
*
* Handles losing the focus for the combo box.
*
* History:
\***************************************************************************/

void xxxCBKillFocusHelper(
	PCBOX pcbox)
{
	TL tlpwndList;
	TL tlpwndEdit;

	CheckLock(pcbox->hwnd);

	if (!pcbox->fFocus || pcbox->spwndList == NULL)
	{
		return;
	}

	ThreadLock(pcbox->spwndList, &tlpwndList);
	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);

	/*
	 * The combo box is losing the focus.  Send buttonup clicks so that
	 * things release the mouse capture if they have it...  If the
	 * pwndListBox is null, don't do anything.  This occurs if the combo box
	 * is destroyed while it has the focus.
	 */
	SendMessage(pcbox->hwnd, WM_LBUTTONUP, 0L, 0xFFFFFFFFL);
	if (!xxxCBHideListBoxWindow(pcbox, TRUE, FALSE))
	{
		return;
	}

	/*
	 * Turn off the listbox caret
	 */

	if (pcbox->CBoxStyle == SDROPDOWNLIST)
	{
		SendMessage(pcbox->spwndList, LBCB_CARETOFF, 0, 0);
	}

	if (pcbox->fNoEdit)
	{

		/*
		 * Invert the static text rectangle
		 */
		xxxCBInvertStaticWindow(pcbox, FALSE, (HDC)NULL);
	}
	else if (pcbox->spwndEdit)
	{
		SendMessage(pcbox->spwndEdit, EM_SETSEL, 0, 0);
	}

	pcbox->fFocus = FALSE;
	xxxCBNotifyParent(pcbox, CBN_KILLFOCUS);

	ThreadUnlock(&tlpwndEdit);
	ThreadUnlock(&tlpwndList);
}


/***************************************************************************\
* xxxCBGetTextLengthHelper
*
* For the combo box without an edit control, returns size of current selected
* item
*
* History:
\***************************************************************************/

LONG xxxCBGetTextLengthHelper(
	PCBOX pcbox,
	BOOL fAnsi)
{
	int item;
	int cchText;
	TL tlpwndList;

	ThreadLock(pcbox->spwndList, &tlpwndList);
	item = (int)SendMessage(pcbox->spwndList, LB_GETCURSEL, 0, 0);

	if (item == LB_ERR)
	{

		/*
		 * No selection so no text.
		 */
		cchText = 0;
	}
	else
	{
		cchText = (int)SendMessage(pcbox->spwndList, LB_GETTEXTLEN,
				item, 0);
	}

	ThreadUnlock(&tlpwndList);

	return cchText;
}

/***************************************************************************\
* xxxCBGetTextHelper
*
* For the combo box without an edit control, copies cbString bytes of the
* string in the static text box to the buffer given by pString.
*
* History:
\***************************************************************************/

LONG xxxCBGetTextHelper(
	PCBOX pcbox,
	int cchString,
	LPWSTR pString,
	BOOL fAnsi)
{
	int item;
	int cchText;
	LPWSTR pText;
	DWORD dw;
	TL tlpwndList;

	CheckLock(pcbox->hwnd);

	if (!cchString || !pString)
	{
		return 0;
	}

	/*
	 * Null the buffer to be nice.
	 */
	if (fAnsi)
	{
		*((LPSTR)pString) = 0;
	}
	else
	{
		*((LPWSTR)pString) = 0;
	}

	ThreadLock(pcbox->spwndList, &tlpwndList);
	item = (int)SendMessage(pcbox->spwndList, LB_GETCURSEL, 0, 0);

	if (item == LB_ERR)
	{

		/*
		 * No selection so no text.
		 */
		ThreadUnlock(&tlpwndList);
		return 0;
	}

	cchText = (int)SendMessage(pcbox->spwndList, LB_GETTEXTLEN, item, 0);

	cchText++;
	if ((cchText <= cchString) ||
		(/*!TestWF(pcbox->spwnd, WFWIN31COMPAT) && */cchString == 2))
	{
		/*
		 * Just do the copy if the given buffer size is large enough to hold
		 * everything.  Or if old 3.0 app.  (Norton used to pass 2 & expect 3
		 * chars including the \0 in 3.0; Bug #7018 win31: vatsanp)
		 */
		dw = (int)SendMessage(pcbox->spwndList, LB_GETTEXT, item,
				(LPARAM)pString);
		ThreadUnlock(&tlpwndList);
		return dw;
	}

	if (!(pText = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, cchText * sizeof(WCHAR))))
	{

		/*
		 * Bail.  Not enough memory to chop up the text.
		 */
		ThreadUnlock(&tlpwndList);
		return 0;
	}

	__try
	{
		SendMessage(pcbox->spwndList, LB_GETTEXT, item, (LPARAM)pText);
		if (fAnsi)
		{
			RtlCopyMemory((PBYTE)pString, (PBYTE)pText, cchString);
			((LPSTR)pString)[cchString - 1] = 0;
		}
		else
		{
			RtlCopyMemory((PBYTE)pString, (PBYTE)pText, cchString * sizeof(WCHAR));
			((LPWSTR)pString)[cchString - 1] = 0;
		}
	}
	__finally
	{
		UserLocalFree((HANDLE)pText);
	}

	ThreadUnlock(&tlpwndList);
	return cchString;
}

/***************************************************************************\
* ClientTabTheTextOutForWimps
*
* effects: Outputs the tabbed text if fDrawTheText is TRUE and returns the
* textextent of the tabbed text.
*
* nCount                    Count of bytes in string
* nTabPositions             Count of tabstops in tabstop array
* lpintTabStopPositions     Tab stop positions in pixels
* iTabOrigin                Tab stops are with respect to this
*
* History:
* 19-Jan-1993 mikeke   Client side
* 13-Sep-1996 GregoryW This routine now calls the LPK(s) to handle text out.
*                      If no LPKs are installed, this defaults to calling
*                      UserLpkTabbedTextOut (identical behavior to what we
*                      had before supporting LPKs).
\***************************************************************************/

LONG TabTextOut(
	HDC hdc,
	int x,
	int y,
	LPCWSTR lpstring,
	int nCount,
	int nTabPositions,
	CONST INT *lpTabPositions,
	int iTabOrigin,
	BOOL fDrawTheText,
	int iCharset)
{
	int     cxCharWidth;
	int     cyCharHeight = 0;

	if (nCount == -1 && lpstring)
	{
		nCount = wcslen(lpstring);
	}
	if (!lpstring || nCount < 0 || nTabPositions < 0)
	{
		return 0;
	}


	// Check if it is SysFont AND the mapping mode is MM_TEXT;
	// Fix made in connection with Bug #8717 --02-01-90  --SANKAR--
	//if (IsSysFontAndDefaultMode(hdc))
	{
		cxCharWidth  = cxSysFontChar;
		cyCharHeight = cySysFontChar;
	}
	//else
	//{
	//	cxCharWidth  = GdiGetCharDimensions(hdc, NULL, &cyCharHeight);
	//	if (cxCharWidth == 0)
	//	{
	//		RIPMSG0(RIP_WARNING, "TabTextOut: GdiGetCharDimensions failed");
	//		return 0;
	//	}
	//}

	//return (*fpLpkTabbedTextOut)(hdc, x, y, lpstring, nCount, nTabPositions,
	//		lpTabPositions, iTabOrigin, fDrawTheText,
	//		cxCharWidth, cyCharHeight, iCharset);

	return 0;
}

////////////////////////////////////////////////
void *pUserHeap = GetProcessHeap();

//////////////////////////////////////////////////////////////////////////
// All the (one time) initialization/destruction code used for combo boxes
//////////////////////////////////////////////////////////////////////////

#pragma hdrstop

extern LOOKASIDE ComboboxLookaside;

#define RECALC_CYDROP   -1

void xxxCBSetDroppedSize(PCBOX pcbox, LPRECT lprc);

/***************************************************************************\
* CBNcCreateHandler
*
* Allocates space for the CBOX structure and sets the window to point to it.
*
* History:
\***************************************************************************/

LONG CBNcCreateHandler(
	PCBOX pcbox,
	HWLWND pwnd)
{
	/*
	* Save the style bits so that we have them when we create the client area
	* of the combo box window.
	*/
	LONG lStyle = GetStyle(pwnd);
	pcbox->styleSave = lStyle & (WS_VSCROLL | WS_HSCROLL);

	if (!(lStyle & CBS_OWNERDRAWFIXED))
	{
		// Add in CBS_HASSTRINGS if the style is implied...
		//L        SetWindowState(pwnd, CBFHASSTRINGS);
		lStyle |= CBS_HASSTRINGS;
	}

	//L     _ASSERT(HIBYTE(WFVSCROLL) == HIBYTE(WFHSCROLL));
	//     _ASSERT(HIBYTE(WFHSCROLL) == HIBYTE(WFBORDER));
	//    ClearWindowState(pwnd, WFVSCROLL | WFHSCROLL | WFBORDER);
	lStyle &= ~WS_VSCROLL;
	lStyle &= ~WS_HSCROLL;
	lStyle &= ~WS_BORDER;
	SetStyle(pwnd, lStyle);

	//
	// If the window is 4.0 compatible or has a CLIENTEDGE, draw the combo
	// in 3D.  Otherwise, use a flat border.
	//
	//L     if (TestWF(pwnd, WFWIN40COMPAT) || TestWF(pwnd, WEFCLIENTEDGE))
	//         pcbox->f3DCombo = TRUE;
	//
	//     ClearWindowState(pwnd, WEFEDGEMASK);

	return (LONG)TRUE;
}

/***************************************************************************\
* xxxCBCreateHandler
*
* Creates all the child controls within the combo box
* Returns -1 if error
*
* History:
\***************************************************************************/

LRESULT xxxCBCreateHandler(
	PCBOX pcbox,
	HWLWND pwnd)
{
	LONG lStyleT;
	HWND hwndList = NULL;
	//HWND hwndEdit;
	DWORD lExStyle;

	CheckLock(pwnd);

	/*
	* Don't lock the combobox window: this would prevent WM_FINALDESTROY
	* being sent to it, so pwnd and pcbox wouldn't get freed (zombies)
	* until thread cleanup. (IanJa)  LATER: change name from spwnd to pwnd.
	* Lock(&pcbox->spwnd, pwnd); - caused a 'catch-22'
	*/
	//L    Lock(&(pcbox->spwndParent), REBASEPWND(pwnd, spwndParent));

	/*
	* Break out the style bits so that we will be able to create the listbox
	* and editcontrol windows.
	*/

	LONG lStyle = GetStyle(pwnd);
	LONG lStyleEx = GetExStyle(pwnd);
	if (LOBYTE(lStyle & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
	{
		pcbox->CBoxStyle = SDROPDOWNLIST;
		pcbox->fNoEdit = TRUE;
	}
	else if (LOBYTE(lStyle & CBS_DROPDOWN) == CBS_DROPDOWN)
	{
		pcbox->CBoxStyle = SDROPDOWN;
	}
	else
	{
		pcbox->CBoxStyle = SSIMPLE;
	}

	pcbox->fRtoLReading = ((lStyleEx & WS_EX_RTLREADING) != 0);
	pcbox->fRightAlign = ((lStyleEx & WS_EX_RIGHT) != 0);

	if (lStyle & CBS_UPPERCASE)
	{
		pcbox->fCase = UPPERCASE;
	}
	else if (lStyle & CBS_LOWERCASE)
	{
		pcbox->fCase = LOWERCASE;
	}
	else
	{
		pcbox->fCase = 0;
	}

	// Listbox item flags.
	if (lStyle & CBS_OWNERDRAWVARIABLE)
	{
		pcbox->OwnerDraw = OWNERDRAWVAR;
	}
	if (lStyle & CBS_OWNERDRAWFIXED)
	{
		pcbox->OwnerDraw = OWNERDRAWFIXED;
	}

	/*
	* Get the size of the combo box rectangle.
	*/
	// Get control sizes.
	RECT rcWindow;
	GetWindowRect(pwnd, &rcWindow);
	pcbox->cxCombo = rcWindow.right - rcWindow.left;
	pcbox->cyDrop = RECALC_CYDROP;
	pcbox->cxDrop = 0;
	RECT rcList;
	xxxCBCalcControlRects(pcbox, &rcList);

	/*
	* Note that we have to create the listbox before the editcontrol since the
	* editcontrol code looks for and saves away the listbox pwnd and the
	* listbox pwnd will be NULL if we don't create it first.  Also, hack in
	* some special +/- values for the listbox size due to the way we create
	* listboxes with borders.
	*/
	lStyleT = pcbox->styleSave;

	lStyleT |= WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_COMBOBOX | WS_CLIPSIBLINGS;

	if (!IsWindowEnabled(pwnd))
	{
		lStyleT |= WS_DISABLED;
	}
	if (lStyle & CBS_NOINTEGRALHEIGHT)
	{
		lStyleT |= LBS_NOINTEGRALHEIGHT;
	}
	if (lStyle & CBS_SORT)
	{
		lStyleT |= LBS_SORT;
	}
	if (lStyle & CBS_HASSTRINGS)
	{
		lStyleT |= LBS_HASSTRINGS;
	}
	if (lStyle & CBS_DISABLENOSCROLL)
	{
		lStyleT |= LBS_DISABLENOSCROLL;
	}

	if (pcbox->OwnerDraw == OWNERDRAWVAR)
	{
		lStyleT |= LBS_OWNERDRAWVARIABLE;
	}
	else if (pcbox->OwnerDraw == OWNERDRAWFIXED)
	{
		lStyleT |= LBS_OWNERDRAWFIXED;
	}

	if (pcbox->CBoxStyle & SDROPPABLE)
	{
		lStyleT |= WS_BORDER;
	}

	HINSTANCE hIns = IUIGetInstanceHandle();

	// Simple风格的ComboBox不创建弹出式下拉框
	if (pcbox->CBoxStyle != SSIMPLE)
	{
		// 创建下拉列表
		WNDCLASS wc;

		wc.lpfnWndProc = ListBoxWndProcW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.lpszMenuName = NULL;
		wc.hInstance = hIns;
		wc.lpszClassName = IUI_COMBOLBOX;
		wc.hbrBackground = NULL;
		wc.style = CS_DBLCLKS | CS_GLOBALCLASS;
		wc.cbWndExtra = sizeof(PCBOX);// 这样，SetWindowLong(HWND, 0, void *)才能设置正确，0一般用来存放控件变量
		wc.cbClsExtra = 0;

		RegisterClass(&wc);

		//lExStyle = lExStyle & (WS_EX_RIGHT | WS_EX_RTLREADING | WS_EX_LEFTSCROLLBAR);
		lExStyle = 0;
		hwndList = CreateWindowEx(lExStyle |
			((pcbox->CBoxStyle & SDROPPABLE) ? WS_EX_TOOLWINDOW : WS_EX_CLIENTEDGE),
			IUI_COMBOLBOX, NULL, lStyleT,
			rcList.left, rcList.top, rcList.right - rcList.left,
			rcList.bottom - rcList.top,
			GetHostWnd(pwnd), (HMENU)CBLISTBOXID, hIns, pcbox);
		pcbox->spwndList = hwndList;

		if (!pcbox->spwndList)
		{
			return -1;
		}
	}

	/*
	* Create either the edit control or the static text rectangle.
	*/
	if (pcbox->fNoEdit)
	{

		/*
		* No editcontrol so we will draw text directly into the combo box
		* window.
		*/
		/*
		* Don't lock the combobox window: this would prevent WM_FINALDESTROY
		* being sent to it, so pwnd and pcbox wouldn't get freed (zombies)
		* until thread cleanup. (IanJa)  LATER: change name from spwnd to pwnd.
		* Lock(&(pcbox->spwndEdit), pcbox->spwnd); - caused a 'catch-22'
		*/
		pcbox->spwndEdit = NULL;//pcbox->spwnd;
	}
	else
	{
		DWORD dwCsFlags;

		lStyleT = WS_CHILD | WS_VISIBLE | ES_COMBOBOX | ES_NOHIDESEL;
		if (!IsWindowEnabled(pwnd))
		{
			lStyleT |= WS_DISABLED;
		}
		if (lStyle & CBS_AUTOHSCROLL)
		{
			lStyleT |= ES_AUTOHSCROLL;
		}
		if (lStyle & CBS_OEMCONVERT)
		{
			lStyleT |= ES_OEMCONVERT;
		}
		if (pcbox->fCase)
		{
			lStyleT |= (pcbox->fCase & UPPERCASE) ? ES_UPPERCASE : ES_LOWERCASE;
		}

		/*
		* Edit control need to know whether original CreateWindow*() call
		* was ANSI or Unicode.
		*/
		dwCsFlags = 0;//L TestWF(pcbox->spwnd, WFANSICREATOR) ? CW_FLAGS_ANSI : 0L;
		if (lExStyle & WS_EX_RIGHT)
		{
			lStyleT |= ES_RIGHT;
		}

		pcbox->spwndEdit = CreateWindowEx(
			NULL,
			lExStyle,
			IUI_RICHTEXTBOX,
			NULL, lStyleT | WS_VISIBLE,
			pcbox->editrc.left,
			pcbox->editrc.top,
			RECTWIDTH(pcbox->editrc),
			RECTHEIGHT(pcbox->editrc),
			GetHostWnd(pcbox->hwnd),
			pcbox->hwnd,
			XNAME_COMBOBOX_EDIT,
			NULL,
			NULL);
	}
	if (!IsWindow(pcbox->spwndEdit))
	{
		return -1L;
	}

	if (pcbox->CBoxStyle & SDROPPABLE)
	{

		ShowWindow(hwndList, SW_HIDE);
		SetParent(hwndList, NULL);

		// We need to do this so dropped size works right
		//L        if (!TestWF(pwnd, WFWIN40COMPAT))
		InflateRect(&rcList, 8, 8);

		xxxCBSetDroppedSize(pcbox, &rcList);
	}

	/*
	* return anything as long as it's not -1L (-1L == error)
	*/
	return (LRESULT)pwnd;
}

/***************************************************************************\
* xxxCBCalcControlRects
*
* 计算ComboBox各部件坐标
\***************************************************************************/
TCHAR szOneChar[] = TEXT("0");
void xxxCBCalcControlRects(PCBOX pcbox, LPRECT lprcList)
{
	HWLWND hwnd = pcbox->hwnd;

	CheckLock(pcbox->spwnd);

	/*
	* Determine height of the edit control.  We can use this info to center
	* the button with recpect to the edit/static text window.  For example
	* this will be useful if owner draw and this window is tall.
	*/

	/*
	* Set the initial width to be the combo box rect.  Later we will shorten it
	* if there is a dropdown button.
	*/
	CIUIRect rcWindow;
	GetWindowRect(pcbox->hwnd, &rcWindow);

	pcbox->cyCombo = rcWindow.Height();

	if (pcbox->cyDrop == RECALC_CYDROP)
	{
		pcbox->cyDrop = GetDPIHeight(180);
	}

	/*
	* Determine the rectangles for each of the windows...  1.  Pop down button 2.
	* Edit control or generic window for static text or ownerdraw...  3.  List
	* box
	*/

	// Is there a button?
	if (pcbox->CBoxStyle & SDROPPABLE)
	{
		CIUIRect rcButton;
		GetClientRect(pcbox->hwnd, rcButton);
		rcButton.DeflateRect(pcbox->m_rcButtonMargin);

		// Determine button's rectangle.
		if (pcbox->fRightAlign)
		{
			rcButton.right = rcButton.left + pcbox->m_nButtonWidth;
		}
		else
		{
			rcButton.left = rcButton.right - pcbox->m_nButtonWidth;
		}

		pcbox->buttonrc = rcButton;
	}
	else
	{
		/*
		* No button so make the rectangle 0 so that a point in rect will always
		* return false.
		*/
		SetRectEmpty(&pcbox->buttonrc);
	}

	/*
	* So now, the edit rect is really the item area.
	*/
	CIUIRect rcEdit;
	GetClientRect(pcbox->hwnd, rcEdit);
	rcEdit.DeflateRect(pcbox->m_rcEditMargin);

	// Is there a right-aligned button?
	if ((pcbox->CBoxStyle & SDROPPABLE))
	{
		if (pcbox->fRightAlign)
		{
			rcEdit.left +=
				(pcbox->m_rcButtonMargin.left
					+ pcbox->m_rcButtonMargin.right
					+ pcbox->m_nButtonWidth);
		}
		else
		{
			rcEdit.right -=
				(pcbox->m_rcButtonMargin.left
					+ pcbox->m_rcButtonMargin.right
					+ pcbox->m_nButtonWidth);
		}
	}
	pcbox->editrc = rcEdit;

	lprcList->left = 0;
	lprcList->top = pcbox->cyCombo;
	lprcList->right = max(pcbox->cxDrop, pcbox->cxCombo);
	lprcList->bottom = pcbox->cyCombo + pcbox->cyDrop;
}

/***************************************************************************\
* xxxCBNcDestroyHandler
*
* Destroys the combobox and frees up all memory used by it
*
* History:
\***************************************************************************/

void xxxCBNcDestroyHandler(
	HWLWND pwnd,
	PCBOX pcbox)
{
	CheckLock(pwnd);

	/*
	* If there is no pcbox, there is nothing to clean up.
	*/
	if (pcbox != NULL)
	{

		/*
		* Destroy the list box here so that it'll send WM_DELETEITEM messages
		* before the combo box turns into a zombie.
		*/
		if (pcbox->spwndList != NULL)
		{
			DestroyWindow(pcbox->spwndList);
			//L Unlock(&pcbox->spwndList);
		}

		pcbox->hwnd = NULL;
		//L Unlock(&pcbox->spwndParent);

		/*
		* If there is no editcontrol, spwndEdit is the combobox window which
		* isn't locked (that would have caused a 'catch-22').
		*/
		//if (pwnd != pcbox->spwndEdit)
		//{
		//	//L Unlock(&pcbox->spwndEdit);
		//}

		/*
		* Since a pointer and a handle to a fixed local object are the same.
		*/
		//L FreeLookasideEntry(&ComboboxLookaside, pcbox);
	}

	/*
	* Set the window's fnid status so that we can ignore rogue messages
	*/
	//L NtUserSetWindowFNID(HWq(pwnd), FNID_CLEANEDUP_BIT);
}

/***************************************************************************\
* xxxCBSetFontHandler
*
* History:
\***************************************************************************/

void xxxCBSetFontHandler(
	PCBOX pcbox,
	HANDLE hFont,
	BOOL fRedraw)
{
	TL tlpwndEdit;
	TL tlpwndList;

	CheckLock(pcbox->spwnd);

	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
	ThreadLock(pcbox->spwndList, &tlpwndList);

	pcbox->hFont = hFont;

	if (!pcbox->fNoEdit && pcbox->spwndEdit)
	{
		SendMessage(pcbox->spwndEdit, WM_SETFONT, (WPARAM)hFont, FALSE);
	}

	SendMessage(pcbox->spwndList, WM_SETFONT, (WPARAM)hFont, FALSE);

	// Recalculate the layout of controls.  This will hide the listbox also.
	xxxCBPosition(pcbox);

	if (fRedraw)
	{
		InvalidateRect(pcbox->hwnd, NULL);
		// LATER UpdateWindow(HW(pcbox->spwnd));
	}

	ThreadUnlock(&tlpwndList);
	ThreadUnlock(&tlpwndEdit);
}

/***************************************************************************\
* xxxCBSetEditItemHeight
*
* Sets the height of the edit/static item of a combo box.
*
* History:
* 06-27-91 DarrinM      Ported from Win 3.1.
\***************************************************************************/

LONG xxxCBSetEditItemHeight(
	PCBOX pcbox,
	int dyEdit)
{
	TL tlpwndEdit;
	TL tlpwndList;

	CheckLock(pcbox->spwnd);

	if (dyEdit > 255)
	{
		//L        RIPERR0(ERROR_INVALID_EDIT_HEIGHT, RIP_VERBOSE, "");
		return CB_ERR;
	}

	pcbox->editrc.bottom = pcbox->editrc.top + dyEdit;
	pcbox->cyCombo = pcbox->editrc.bottom + 8;

	if (pcbox->CBoxStyle & SDROPPABLE)
	{
		pcbox->buttonrc.bottom = pcbox->cyCombo - 8;
	}

	ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
	ThreadLock(pcbox->spwndList, &tlpwndList);


	/*
	* Reposition the editfield.
	* Don't let spwndEdit or List of NULL go through; if someone adjusts
	* the height on a NCCREATE; same as not having
	* HW instead of HWq but we don't go to the kernel.
	*/
	if (!pcbox->fNoEdit && pcbox->spwndEdit)
	{
		MoveWindow(pcbox->spwndEdit, pcbox->editrc.left, pcbox->editrc.top,
			pcbox->editrc.right - pcbox->editrc.left, dyEdit, TRUE);
	}

	/*
	* Reposition the list and combobox windows.
	*/
	if (pcbox->CBoxStyle == SSIMPLE)
	{
		if (pcbox->spwndList != 0)
		{
			RECT rcWindow;
			GetWindowRect(pcbox->spwndList, &rcWindow);
			MoveWindow(pcbox->spwndList, 0, pcbox->cyCombo, pcbox->cxCombo,
				pcbox->cyDrop, FALSE);

			MoveWindow(pcbox->hwnd, rcWindow.left, rcWindow.top,
				pcbox->cxCombo, pcbox->cyCombo + rcWindow.bottom - rcWindow.top, TRUE);
		}
	}
	else
	{
		if (pcbox->spwndList != NULL)
		{
			RECT rcWindow;
			GetWindowRect(pcbox->hwnd, &rcWindow);
			MoveWindow(pcbox->spwndList, rcWindow.left,
				rcWindow.top + pcbox->cyCombo,
				max(pcbox->cxDrop, pcbox->cxCombo), pcbox->cyDrop, FALSE);
		}

		MoveWindow(pcbox->hwnd, 0, 0, pcbox->cxCombo, pcbox->cyCombo, TRUE);
	}

	ThreadUnlock(&tlpwndList);
	ThreadUnlock(&tlpwndEdit);

	return CB_OKAY;
}


/***************************************************************************\
* xxxCBSizeHandler
*
* Recalculates the sizes of the internal controls in response to a
* resizing of the combo box window.  The app must size the combo box to its
* maximum open/dropped down size.
*
* History:
\***************************************************************************/

void xxxCBSizeHandler(
	PCBOX pcbox)
{
	CheckLock(pcbox->spwnd);

	/*
	* Assume listbox is visible since the app should size it to its maximum
	* visible size.
	*/
	RECT rcWindow;
	GetWindowRect(pcbox->hwnd, &rcWindow);

	pcbox->cxCombo = rcWindow.right - rcWindow.left;
	// only recalc cyDrop if the drop down is not TOTALLY nuked by the sizing
	// -- Visio 1.0 -- B#13112
	if (((rcWindow.bottom - rcWindow.top) - pcbox->cyCombo) > 0)
	{
		pcbox->cyDrop = RECALC_CYDROP;
	}

	// Reposition everything.
	xxxCBPosition(pcbox);
}

/***************************************************************************\
*
*  CBPosition()
*
*  Repositions components of edit control.
*
\***************************************************************************/
void xxxCBPosition(PCBOX pcbox)
{
	RECT rcList;

	// Calculate placement of components--button, item, list
	xxxCBCalcControlRects(pcbox, &rcList);

	if (!pcbox->fNoEdit && IsWindow(pcbox->spwndEdit))
	{
		TL tlpwndEdit;

		ThreadLock(pcbox->spwndEdit, &tlpwndEdit);
		MoveWindow(pcbox->spwndEdit, pcbox->editrc.left, pcbox->editrc.top,
			pcbox->editrc.right - pcbox->editrc.left,
			pcbox->editrc.bottom - pcbox->editrc.top, TRUE);
		ThreadUnlock(&tlpwndEdit);
	}

	// Recalculate drop height & width
	xxxCBSetDroppedSize(pcbox, &rcList);
}

/***************************************************************************\
*
*  CBSetDroppedSize()
*
*  Compute the drop down window's width and max height
*
\***************************************************************************/
void xxxCBSetDroppedSize(PCBOX pcbox, LPRECT lprc)
{
	TL tlpwndList;

	pcbox->fLBoxVisible = TRUE;
	xxxCBHideListBoxWindow(pcbox, FALSE, FALSE);

	ThreadLock(pcbox->spwndList, &tlpwndList);
	MoveWindow(pcbox->spwndList, lprc->left, lprc->top,
		lprc->right - lprc->left, lprc->bottom - lprc->top, FALSE);
	ThreadUnlock(&tlpwndList);

}


//////////////////////////////////////////////////////////////////////////
// Directory Combo Box Routines
//////////////////////////////////////////////////////////////////////////

#pragma hdrstop

/***************************************************************************\
* xxxCBDir
*
* Supports the CB_DIR message which adds a list of files from the
* current directory to the combo box.
*
* History:
\***************************************************************************/

int xxxCBDir(
	PCBOX pcbox,
	UINT attrib,
	LPWSTR pFileName)
{
	PLBIV plb;
	//    int errorValue;
	//TL tlpwnd;

	CheckLock(pcbox->spwnd);
	_ASSERT(pcbox->spwndList);

	plb = (PLBIV)GetWindowLong(pcbox->spwndList, 0);

	ThreadLock(plb->spwnd, &tlpwnd);
	//L     ThreadUnlock(&tlpwnd);
	//
	//     switch (errorValue) {
	//     case LB_ERR:
	//         return CB_ERR;
	//         break;
	//     case LB_ERRSPACE:
	//         return CB_ERRSPACE;
	//         break;
	//     default:
	//         return errorValue;
	//         break;
	//     }

	return 0;
}

/***************************************************************************\
* DlgDirSelectComboBoxEx
*
* Retrieves the current selection from the listbox of a combobox.
* It assumes that the combo box was filled by xxxDlgDirListComboBox()
* and that the selection is a drive letter, a file, or a directory name.
*
* History:
* 12-05-90 IanJa    converted to internal version
\***************************************************************************/

//L int DlgDirSelectComboBoxExA(
//     HWND hwndDlg,
//     LPSTR pszOut,
//     int cchOut,
//     int idComboBox)
// {
//     LPWSTR lpwsz;
//     BOOL fRet;
//
//     lpwsz = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, cchOut * sizeof(WCHAR));
//     if (!lpwsz) {
//         return FALSE;
//     }
//
//     fRet = DlgDirSelectComboBoxExW(hwndDlg, lpwsz, cchOut, idComboBox);
//
//     WCSToMB(lpwsz, -1, &pszOut, cchOut, FALSE);
//
//     UserLocalFree(lpwsz);
//
//     return fRet;
// }
//
// int DlgDirSelectComboBoxExW(
//     HWND hwndDlg,
//     LPWSTR pwszOut,
//     int cchOut,
//     int idComboBox)
// {
//     PWND pwndDlg;
//     PWND pwndComboBox;
//     PCBOX pcbox;
//
//     pwndDlg = ValidateHwnd(hwndDlg);
//
//     if (pwndDlg == NULL)
//         return FALSE;
//
//     pwndComboBox = _GetDlgItem(pwndDlg, idComboBox);
//     if (pwndComboBox == NULL) {
//         RIPERR0(ERROR_CONTROL_ID_NOT_FOUND, RIP_VERBOSE, "");
//         return 0;
//     }
//     pcbox = ((PCOMBOWND)pwndComboBox)->pcbox;
//     if (pcbox == NULL) {
//         RIPERR0(ERROR_WINDOW_NOT_COMBOBOX, RIP_VERBOSE, "");
//         return 0;
//     }
//
//     return DlgDirSelectHelper(pwszOut, cchOut, HWq(pcbox->spwndList));
// }
//
//
// /***************************************************************************\
// * xxxDlgDirListComboBox
// *
// * History:
// * 12-05-90 IanJa    converted to internal version
// \***************************************************************************/
//
// int DlgDirListComboBoxA(
//     HWND hwndDlg,
//     LPSTR lpszPathSpecClient,
//     int idComboBox,
//     int idStaticPath,
//     UINT attrib)
// {
//     LPWSTR lpszPathSpec;
//     TL tlpwndDlg;
//     PWND pwndDlg;
//     BOOL fRet;
//
//     pwndDlg = ValidateHwnd(hwndDlg);
//
//     if (pwndDlg == NULL)
//         return FALSE;
//
//     lpszPathSpec = NULL;
//     if (lpszPathSpecClient) {
//         if (!MBToWCS(lpszPathSpecClient, -1, &lpszPathSpec, -1, TRUE))
//             return FALSE;
//     }
//
//     ThreadLock(pwndDlg, &tlpwndDlg);
//     fRet = xxxDlgDirListHelper(pwndDlg, lpszPathSpec, lpszPathSpecClient,
//             idComboBox, idStaticPath, attrib, FALSE);
//     ThreadUnlock(&tlpwndDlg);
//
//     if (lpszPathSpec) {
//         if (fRet) {
//             /*
//              * Non-zero retval means some text to copy out.  Copy out up to
//              * the nul terminator (buffer will be big enough).
//              */
//             WCSToMB(lpszPathSpec, -1, &lpszPathSpecClient, MAXLONG, FALSE);
//         }
//         UserLocalFree(lpszPathSpec);
//     }
//
//     return fRet;
// }
//
// int DlgDirListComboBoxW(
//     HWND hwndDlg,
//     LPWSTR lpszPathSpecClient,
//     int idComboBox,
//     int idStaticPath,
//     UINT attrib)
// {
//     LPWSTR lpszPathSpec;
//     PWND pwndDlg;
//     TL tlpwndDlg;
//     BOOL fRet;
//
//     pwndDlg = ValidateHwnd(hwndDlg);
//
//     if (pwndDlg == NULL)
//         return FALSE;
//
//     lpszPathSpec = lpszPathSpecClient;
//
//     ThreadLock(pwndDlg, &tlpwndDlg);
//     fRet = xxxDlgDirListHelper(pwndDlg, lpszPathSpec, (/LPBYTE)lpszPathSpecClient,
//             idComboBox, idStaticPath, attrib, FALSE);
//     ThreadUnlock(&tlpwndDlg);
//
//     return fRet;
// }

//////////////////////////////////////////////////////////////////////////
// ListBox routines
//////////////////////////////////////////////////////////////////////////


#pragma hdrstop

LOOKASIDE ListboxLookaside;


/***************************************************************************\
* xxxLBoxCtlWndProc
*
* Window Procedure for ListBox AND ComboLBox control.
* NOTE: All window procedures are APIENTRY
* WARNING: This listbox code contains some internal messages and styles which
* are defined in combcom.h and in combcom.inc.  They may be redefined
* (or renumbered) as needed to extend the windows API.
*
* History:
* 16-Apr-1992 beng      Added LB_SETCOUNT
\***************************************************************************/

LRESULT APIENTRY ListBoxWndProcWorker(
	HWND pwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	DWORD fAnsi)
{
	HWND hwnd = pwnd;
	PAINTSTRUCT ps;
	HDC         hdc;
	LPRECT      lprc;
	PLBIV plb;    /* List Box Instance Variable */
	INT iSel;     /* Index of selected item */
	DWORD dw;
	TL tlpwndParent;
	UINT wFlags;
	LPWSTR lpwsz = NULL;
	LRESULT lReturn = 0;
	static BOOL fInit = TRUE;

	CheckLock(pwnd);

	//VALIDATECLASSANDSIZE(pwnd, FNID_LISTBOX);
	//INITCONTROLLOOKASIDE(&ListboxLookaside, LBIV, spwnd, 4);

	/*
	* Get the plb for the given window now since we will use it a lot in
	* various handlers. This was stored using SetWindowLong(hwnd,0,plb)
	* when the listbox was first created (by INITCONTROLLOOKASIDE above)
	*/
	plb = (PLBIV)GetWindowLong(hwnd, 0);//((PLBWND)pwnd)->pLBIV;

										/*
										* Handle ANSI translations of input parameters
										*/
	if (fAnsi)
	{
		switch (message)
		{
		case LB_ADDSTRING:
		case LB_ADDSTRINGUPPER:
		case LB_ADDSTRINGLOWER:
		case LB_FINDSTRING:
		case LB_FINDSTRINGEXACT:
		case LB_INSERTSTRING:
		case LB_INSERTSTRINGUPPER:
		case LB_INSERTSTRINGLOWER:
		case LB_SELECTSTRING:
			if (!plb->fHasStrings)
			{
				break;
			}
			// Fall through...
		case LB_ADDFILE:
		case LB_DIR:
			if (lParam)
			{
				//L if (!MBToWCS((LPSTR)lParam, -1, &lpwsz, -1, TRUE))
				//{
				//	return LB_ERR;
				//}
			}
			break;
		default:
			break;
		}
		if (lpwsz)
		{
			lParam = (LPARAM)lpwsz;
		}
	}

	switch (message)
	{
	case WM_NCCREATE:
		if (plb == NULL)
		{
			CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;
			plb = new LBIV;
			plb->spwnd = hwnd;
			plb->spwndParent = pcs->hwndParent;
			plb->cyChar = 24;
			plb->m_nHScrollBarHeight = Metrics::g_nScrollBarWidth;
			plb->m_nVScrollBarWidth = Metrics::g_nScrollBarWidth;

			// 创建虚拟根控件
			plb->m_hVirtualRoot = CreateRoot();
			plb->m_hVirtualRoot->m_hHostWnd = hwnd;

			SetWindowLong(hwnd, 0, (LONG)plb);
		}
		//if ((lStyle & LBS_MULTICOLUMN) && (lStyle & WS_VSCROLL))
		//{
		//	DWORD mask = WS_VSCROLL;
		//	DWORD flags = 0;
		//	if (!TestWF(pwnd, WFWIN40COMPAT))
		//	{
		//		mask |= WS_HSCROLL;
		//		flags = WS_HSCROLL;
		//	}
		//	NtUserAlterWindowStyle(hwnd, mask, flags);
		//}
		goto CallDWP;

	case WM_NCDESTROY:
	case WM_FINALDESTROY:
		xxxDestroyLBox(plb, pwnd);
		delete plb;
		plb = NULL;
		break;

	case LB_GETTOPINDEX:        // Return index of top item displayed.
		return plb->iTop;

	case LB_SETTOPINDEX:
		if (wParam && ((INT)wParam < 0 || (INT)wParam >= plb->cMac))
		{
			//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
			return LB_ERR;
		}
		if (plb->cMac)
		{
			xxxNewITop(plb, (INT)wParam);
		}
		break;

	case WM_STYLECHANGED:
		plb->fRtoLReading = FALSE;//(TestWF(pwnd, WEFRTLREADING) != 0);
		plb->fRightAlign = FALSE;//(TestWF(pwnd, WEFRIGHT) != 0);
		xxxCheckRedraw(plb, FALSE, 0);
		break;

	case WM_WINDOWPOSCHANGED:

		/*
		* If we are in the middle of creation, ignore this
		* message because it will generate a WM_SIZE message.
		* See xxxLBCreate().
		*/
		if (!plb->fIgnoreSizeMsg)
		{
			goto CallDWP;
		}
		break;

	case WM_SIZE:

		/*
		* If we are in the middle of creation, ignore size
		* messages.  See xxxLBCreate().
		*/
		if (!plb->fIgnoreSizeMsg)
		{
			//xxxLBSize(plb, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;

	case WM_ERASEBKGND:
		//ThreadLock(plb->spwndParent, &tlpwndParent);
		//FillWindow(HW(plb->spwndParent), hwnd, (HDC)wParam,
		//	(HBRUSH)CTLCOLOR_LISTBOX);
		//ThreadUnlock(&tlpwndParent);
		return TRUE;

	case LB_RESETCONTENT:
		xxxLBResetContent(plb);
		break;

	case WM_TIMER:
		if (wParam == IDSYS_LBSEARCH)
		{
			plb->iTypeSearch = 0;
			KillTimer(hwnd, IDSYS_LBSEARCH);
			xxxInvertLBItem(plb, plb->iSel, TRUE);
			break;
		}

		message = WM_MOUSEMOVE;
		xxxTrackMouse(plb, message, plb->ptPrev);
		break;

		/*
		* Fall through
		*/
		// ComboBox下拉列表并不是在下拉列表弹出时捕获鼠标，而是在下面两两个时机：
		// 1. 在ComboBox中，按下下拉按钮，然后释放鼠标后，下拉列表会捕获鼠标；
		// 2. 在ComboBox中，按下下拉按钮，不释放鼠标，移动鼠标到下拉按钮中。
		// 当在ComboBox中的下拉按钮中按下鼠标左键，并移动鼠标到下拉列表中时，会把
		// ComboBox的WM_MOUSEMOVE消息转义成下拉列表的WM_LBUTTONDOWN消息。
		// 当下拉列表捕获了鼠标后，就可以持续调用xxxTrackMouse跟踪鼠标了。
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		POINT pt;

		POINTSTOPOINT(pt, lParam);
		xxxTrackMouse(plb, message, pt);
	}
	break;

	case WM_MBUTTONDOWN:
		//EnterReaderModeHelper(hwnd);
		break;

	case WM_CAPTURECHANGED:
		//
		// Note that this message should be handled only on unexpected
		// capture changes currently.
		//
		//UserAssert(TestWF(pwnd, WFWIN40COMPAT));
		if (plb->fCaptured)
		{
			xxxLBButtonUp(plb, LBUP_NOTIFY);
		}
		break;

	case LBCB_STARTTRACK:
		//
		// 通过设置capture来跟踪鼠标在listbox中的移动。
		//
		if (!plb->pcbox)
		{
			break;
		}

		plb->fCaptured = FALSE;
		if (wParam)
		{
			POINT pt;

			POINTSTOPOINT(pt, lParam);

			ScreenToClient(pwnd, &pt);
			xxxTrackMouse(plb, WM_LBUTTONDOWN, pt);
		}
		else
		{
			// 当一个窗口得到Capture后，它的滚动条是无法拖动的。
			// 所以，下面的SetCapture将导致下拉列表窗口的滚动条无法拖动。
			SetCapture(hwnd);
			plb->fCaptured = TRUE;
			plb->iLastSelection = plb->iSel;
		}
		break;

	case LBCB_ENDTRACK:
		// Kill capture, tracking, etc.
		if (plb->fCaptured)
		{
			xxxLBButtonUp(plb, LBUP_RELEASECAPTURE | (wParam ? LBUP_SELCHANGE :
				LBUP_RESETSELECTION));
		}
		break;

	case WM_PRINTCLIENT:
		xxxLBPaint(plb, (HDC)wParam, NULL);
		break;

	case WM_PAINT:
		if (wParam)
		{
			hdc = (HDC)wParam;
			lprc = NULL;
		}
		else
		{
			hdc = BeginPaint(hwnd, &ps);
			lprc = &(ps.rcPaint);
		}

		if (IsWindowVisible(plb->spwnd))
		{
			xxxLBPaint(plb, hdc, lprc);
		}

		if (!wParam)
		{
			EndPaint(hwnd, &ps);
		}
		break;

		// 作为ComboBox的下拉框，没有焦点
	case WM_SETFOCUS:
		// DISABLED in Win 3.1        xxxUpdateWindow(pwnd);
		CaretCreate(plb);
		xxxLBSetCaret(plb, TRUE);
		xxxNotifyOwner(plb, LBN_SETFOCUS);
		//if (FWINABLE())
		//{
		//	if (_IsWindowVisible(pwnd))
		//	{
		//		LBEvent(plb, EVENT_OBJECT_FOCUS, plb->iSelBase);
		//	}
		//}
		break;

	case WM_KILLFOCUS:
		/*
		* Reset the wheel delta count.
		*/
		IUIGetGloabelVar()->gcWheelDelta = 0;

		xxxLBSetCaret(plb, FALSE);
		xxxCaretDestroy(plb);
		xxxNotifyOwner(plb, LBN_KILLFOCUS);
		if (plb->iTypeSearch)
		{
			plb->iTypeSearch = 0;
			KillTimer(hwnd, IDSYS_LBSEARCH);
		}
		if (plb->pszTypeSearch)
		{
			//UserLocalFree(plb->pszTypeSearch);
			plb->pszTypeSearch = NULL;
		}
		break;

	case WM_MOUSEWHEEL:
	{
		int     cDetants;
		int     cPage;
		int     cLines;
		RECT    rc;
		int     windowWidth;
		int     cPos;

		/*
		* Don't handle zoom and datazoom.
		*/
		if (wParam & (MK_SHIFT | MK_CONTROL))
		{
			goto CallDWP;
		}

		lReturn = 1;
		IUIGetGloabelVar()->gcWheelDelta -= (short)HIWORD(wParam);
		cDetants = IUIGetGloabelVar()->gcWheelDelta / WHEEL_DELTA;
		LONG lStyle = GetWindowLong(pwnd, GWL_STYLE);
		if (cDetants != 0
			&& 40 > 0
			&& (lStyle & (WS_VSCROLL | WS_HSCROLL)))
		{

			IUIGetGloabelVar()->gcWheelDelta = IUIGetGloabelVar()->gcWheelDelta % WHEEL_DELTA;

			if (lStyle & WS_VSCROLL)
			{
				cPage = max(1, (plb->cItemFullMax - 1));
				cLines = cDetants *
					(int)min((UINT)cPage, 40);

				cPos = max(0, min(plb->iTop + cLines, plb->cMac - 1));
				if (cPos != plb->iTop)
				{
					xxxLBoxCtlScroll(plb, SB_THUMBPOSITION, cPos);
					xxxLBoxCtlScroll(plb, SB_ENDSCROLL, 0);
				}
			}
			else if (plb->fMultiColumn)
			{
				cPage = max(1, plb->numberOfColumns);
				cLines = cDetants * (int)min((UINT)cPage, ucWheelScrollLines);
				cPos = max(
					0,
					min((plb->iTop / plb->itemsPerColumn) + cLines,
						plb->cMac - 1 - ((plb->cMac - 1) % plb->itemsPerColumn)));

				//if (cPos != plb->iTop)
				//{
				//	xxxLBoxCtlHScrollMultiColumn(plb, SB_THUMBPOSITION, cPos);
				//	xxxLBoxCtlHScrollMultiColumn(plb, SB_ENDSCROLL, 0);
				//}
			}
			else
			{
				GetClientRect(plb->spwnd, &rc);
				windowWidth = rc.right;
				cPage = max(plb->cxChar, (windowWidth / 3) * 2) /
					plb->cxChar;

				cLines = cDetants *
					(int)min((UINT)cPage, ucWheelScrollLines);

				cPos = max(
					0,
					min(plb->xOrigin + (cLines * plb->cxChar),
						plb->maxWidth));

				if (cPos != plb->xOrigin)
				{
					xxxLBoxCtlHScroll(plb, SB_THUMBPOSITION, cPos);
					xxxLBoxCtlHScroll(plb, SB_ENDSCROLL, 0);
				}
			}
		}
	}
	break;

	case WM_VSCROLL:
		xxxLBoxCtlScroll(plb, LOWORD(wParam), HIWORD(wParam));
		break;

	case WM_HSCROLL:
		xxxLBoxCtlHScroll(plb, LOWORD(wParam), HIWORD(wParam));
		break;

	case WM_GETDLGCODE:
		return DLGC_WANTARROWS | DLGC_WANTCHARS;

	case WM_CREATE:
		return xxxLBCreate(plb, pwnd, (LPCREATESTRUCT)lParam);

	case WM_SETREDRAW:

		/*
		* If wParam is nonzero, the redraw flag is set
		* If wParam is zero, the flag is cleared
		*/
		xxxLBSetRedraw(plb, (wParam != 0));
		break;

	case WM_ENABLE:
		xxxLBInvalidateRect(plb, NULL, !plb->OwnerDraw);
		break;

	case WM_SETFONT:
		xxxLBSetFont(plb, (HANDLE)wParam, LOWORD(lParam));
		break;

	case WM_GETFONT:
		return (LRESULT)plb->hFont;

	case WM_DRAGSELECT:
	case WM_DRAGLOOP:
	case WM_DRAGMOVE:
	case WM_DROPFILES:
		ThreadLock(plb->spwndParent, &tlpwndParent);
		lReturn = SendMessage(HW(plb->spwndParent), message, wParam, lParam);
		ThreadUnlock(&tlpwndParent);
		return lReturn;


	case WM_QUERYDROPOBJECT:
	case WM_DROPOBJECT:

		/*
		* fix up control data, then pass message to parent
		*/
		LBDropObjectHandler(plb, (PDROPSTRUCT)lParam);
		ThreadLock(plb->spwndParent, &tlpwndParent);
		lReturn = SendMessage(HW(plb->spwndParent), message, wParam, lParam);
		ThreadUnlock(&tlpwndParent);
		return lReturn;

	case LB_GETITEMRECT:
		return LBGetItemRect(plb, (INT)wParam, (LPRECT)lParam);

	case LB_GETITEMDATA:
		return LBGetItemData(plb, (INT)wParam);  // wParam = item index

	case LB_SETITEMDATA:
		/*
		* wParam is item index
		*/
		return LBSetItemData(plb, (INT)wParam, lParam);

	case LB_GET_ITEM_BIND:
		return (LRESULT)LBGetItemBind(plb, (INT)wParam);

	case LB_SET_ITEM_BIND:
		return LBSetItemBind(plb, (INT)wParam, (HWLWND)lParam);

	case LB_SET_ITEM_BK_COLOR:
		return LBSetHighlightItemBkColor(plb, lParam);

	case LB_GET_ITEM_BK_COLOR:
		return (LRESULT)LBGetHighlightItemBkColor(plb);

	case LB_ADDSTRINGUPPER:
		wFlags = UPPERCASE | LBI_ADD;
		goto CallInsertItem;

	case LB_ADDSTRINGLOWER:
		wFlags = LOWERCASE | LBI_ADD;
		goto CallInsertItem;

	case LB_ADDSTRING:
		wFlags = LBI_ADD;
		goto CallInsertItem;

	case LB_INSERTSTRINGUPPER:
		wFlags = UPPERCASE;
		goto CallInsertItem;

	case LB_INSERTSTRINGLOWER:
		wFlags = LOWERCASE;
		goto CallInsertItem;

	case LB_INSERTSTRING:
		wFlags = 0;
	CallInsertItem:
		lReturn = ((LRESULT)xxxLBInsertItem(plb, (LPWSTR)lParam, (int)wParam, wFlags));
		break;

	case LB_INITSTORAGE:
		return xxxLBInitStorage(plb, fAnsi, (INT)wParam, (INT)lParam);

	case LB_DELETESTRING:
		return xxxLBoxCtlDelete(plb, (INT)wParam);

	case LB_DIR:
		/*
		* wParam - Dos attribute value.
		* lParam - Points to a file specification string
		*/
		//lReturn = xxxLbDir(plb, (INT)wParam, (LPWSTR)lParam);
		break;

	case LB_ADDFILE:
		//lReturn = xxxLbInsertFile(plb, (LPWSTR)lParam);
		break;

	case LB_SETSEL:
		return xxxLBSetSel(plb, (wParam != 0), (INT)lParam);

	case LB_SETCURSEL:
		/*
		* If window obscured, update so invert will work correctly
		*/
		// DISABLED in Win 3.1        xxxUpdateWindow(pwnd);
		return xxxLBSetCurSel(plb, (INT)wParam);

	case LB_GETSEL:
		if (wParam >= (UINT)plb->cMac)
		{
			return ((LRESULT)LB_ERR);
		}

		return IsSelected(plb, (INT)wParam, SELONLY);

	case LB_GETCURSEL:
		if (plb->wMultiple == SINGLESEL)
		{
			return plb->iSel;
		}
		return plb->iSelBase;

	case LB_SELITEMRANGE:
		if (plb->wMultiple == SINGLESEL)
		{
			/*
			* Can't select a range if only single selections are enabled
			*/
			//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "Invalid index passed to LB_SELITEMRANGE");
			return LB_ERR;
		}

		xxxLBSelRange(plb, LOWORD(lParam), HIWORD(lParam), (wParam != 0));
		break;

	case LB_SELITEMRANGEEX:
		if (plb->wMultiple == SINGLESEL)
		{
			/*
			* Can't select a range if only single selections are enabled
			*/
			//RIPERR0(ERROR_INVALID_LB_MESSAGE, RIP_VERBOSE, "LB_SELITEMRANGEEX:Can't select a range if only single selections are enabled");
			return LB_ERR;
		}
		else
		{
			BOOL fHighlight = ((DWORD)lParam > (DWORD)wParam);
			if (fHighlight == FALSE)
			{
				ULONG_PTR temp = lParam;
				lParam = wParam;
				wParam = temp;
			}
			xxxLBSelRange(plb, (INT)wParam, (INT)lParam, fHighlight);
		}
		break;

	case LB_GETTEXTLEN:
		if (lParam != 0)
		{
			//RIPMSG1(RIP_WARNING, "LB_GETTEXTLEN with lParam = %lx\n", lParam);
		}
		lReturn = LBGetText(plb, TRUE, fAnsi, (INT)wParam, NULL);
		break;

	case LB_GETTEXT:
		lReturn = LBGetText(plb, FALSE, fAnsi, (INT)wParam, (LPWSTR)lParam);
		break;

	case LB_GETCOUNT:
		// Lotus Approach calls CallWndProc(ListWndProc, LB_GETCOUNT,...)
		// on a window that doesn't have a plb yet. So, we need to make
		// this check. Bug #6675 - 11/7/94 --
		if (plb)
		{
			return ((LRESULT)plb->cMac);
		}
		else
		{
			return (0);
		}

	case LB_SETCOUNT:
		return xxxLBSetCount(plb, (INT)wParam);

	case LB_SELECTSTRING:
	case LB_FINDSTRING:
		iSel = xxxFindString(plb, (LPWSTR)lParam, (INT)wParam, PREFIX, TRUE);
		if (message == LB_FINDSTRING || iSel == LB_ERR)
		{
			lReturn = iSel;
		}
		else
		{
			lReturn = xxxLBSetCurSel(plb, iSel);
		}
		break;

	case LB_GETLOCALE:
		return plb->dwLocaleId;

	case LB_SETLOCALE:

		/*
		* Validate locale
		*/
		wParam = ConvertDefaultLocale((LCID)wParam);
		if (!IsValidLocale((LCID)wParam, LCID_INSTALLED))
		{
			return LB_ERR;
		}

		dw = plb->dwLocaleId;
		plb->dwLocaleId = (DWORD)wParam;
		return dw;

	case WM_KEYDOWN:

		/*
		* IanJa: Use LOWORD() to get low 16-bits of wParam - this should
		* work for Win16 & Win32.  The value obtained is the virtual key
		*/
		xxxLBoxCtlKeyInput(plb, message, LOWORD(wParam));
		break;

	case WM_CHAR:
		xxxLBoxCtlCharInput(plb, LOWORD(wParam), fAnsi);
		break;

	case LB_GETSELITEMS:
	case LB_GETSELCOUNT:

		/*
		* IanJa/Win32 should this be LPWORD now?
		*/
		return LBoxGetSelItems(plb, (message == LB_GETSELCOUNT), (INT)wParam,
			(LPINT)lParam);

	case LB_SETTABSTOPS:

		/*
		* IanJa/Win32: Tabs given by array of INT for backwards compatability
		*/
		return LBSetTabStops(plb, (INT)wParam, (LPINT)lParam);

	case LB_GETHORIZONTALEXTENT:

		/*
		* Return the max width of the listbox used for horizontal scrolling
		*/
		return plb->maxWidth;

	case LB_SETHORIZONTALEXTENT:

		/*
		* Set the max width of the listbox used for horizontal scrolling
		*/
		if (plb->maxWidth != (INT)wParam)
		{
			plb->maxWidth = (INT)wParam;

			/*
			* When horizontal extent is set, Show/hide the scroll bars.
			* NOTE: LBShowHideScrollBars() takes care if Redraw is OFF.
			* Fix for Bug #2477 -- 01/14/91 -- SANKAR --
			*/
			xxxLBShowHideScrollBars(plb); //Try to show or hide scroll bars
			if (plb->fHorzBar && plb->fRightAlign && !(plb->fMultiColumn || plb->OwnerDraw))
			{
				/*
				* origin to right
				*/
				xxxLBoxCtlHScroll(plb, SB_BOTTOM, 0);
			}
		}
		break;    /* originally returned register ax (message) ! */

	case LB_SETCOLUMNWIDTH:

		/*
		* Set the width of a column in a multicolumn listbox
		*/
		plb->cxColumn = (INT)wParam;
		//LBCalcItemRowsAndColumns(plb);
		if (IsLBoxVisible(plb))
		{
			InvalidateRect(hwnd, NULL, TRUE);
		}
		xxxLBShowHideScrollBars(plb);
		break;

	case LB_SETANCHORINDEX:
		if ((INT)wParam >= plb->cMac)
		{
			//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "Invalid index passed to LB_SETANCHORINDEX");
			return LB_ERR;
		}
		plb->iMouseDown = (INT)wParam;
		plb->iLastMouseMove = (INT)wParam;
		xxxInsureVisible(plb, (int)wParam, (BOOL)(lParam != 0));
		break;

	case LB_GETANCHORINDEX:
		return plb->iMouseDown;

	case LB_SETCARETINDEX:
		if ((plb->iSel == -1) || ((plb->wMultiple != SINGLESEL) &&
			(plb->cMac > (INT)wParam)))
		{

			/*
			* Set's the iSelBase to the wParam
			* if lParam, then don't scroll if partially visible
			* else scroll into view if not fully visible
			*/
			xxxInsureVisible(plb, (INT)wParam, (BOOL)LOWORD(lParam));
			xxxSetISelBase(plb, (INT)wParam);
			break;
		}
		else
		{
			if ((INT)wParam >= plb->cMac)
			{
				//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "Invalid index passed to LB_SETCARETINDEX");
			}
			return LB_ERR;
		}
		break;

	case LB_GETCARETINDEX:
		return plb->iSelBase;

	case LB_SETITEMHEIGHT:
	case LB_GETITEMHEIGHT:
		return LBGetSetItemHeightHandler(plb, message, (INT)wParam, LOWORD(lParam));
		break;

	case LB_FINDSTRINGEXACT:
		lReturn = xxxFindString(plb, (LPWSTR)lParam, (INT)wParam, EQ, TRUE);
		break;

	case LB_ITEMFROMPOINT:
	{
		POINT pt;
		BOOL bOutside;
		DWORD dwItem;

		POINTSTOPOINT(pt, lParam);
		bOutside = ISelFromPt(plb, pt, &dwItem);
		//UserAssert(bOutside == 1 || bOutside == 0);
		return (LRESULT)MAKELONG(dwItem, bOutside);
	}

	case LBCB_CARETON:

		/*
		* Internal message for combo box support
		*/
		CaretCreate(plb);
		// Set up the caret in the proper location for drop downs.
		plb->iSelBase = plb->iSel;
		xxxLBSetCaret(plb, TRUE);
		//if (FWINABLE())
		//{
		//	if (_IsWindowVisible(pwnd))
		//	{
		//		LBEvent(plb, EVENT_OBJECT_FOCUS, plb->iSelBase);
		//	}
		//}
		return (plb->iSel);

	case LBCB_CARETOFF:

		/*
		* Internal message for combo box support
		*/
		xxxLBSetCaret(plb, FALSE);
		xxxCaretDestroy(plb);
		break;

	case LBCB_DROPHEIGHT_CHANGED:
		LBSetCItemFullMax(plb);
		break;

		// 作为IUI Host窗口
	case WM_GET_INTERNAL_ROOT:
		return (LRESULT)plb->m_hVirtualRoot;

	case WM_CONTROLEVENT:
	{
		// 控件的自定义滚动条在滚动结束后，会释放滚动条的Capture，进而释放
		// 滚动条Host窗口（本窗口）的Capture，所以，这里处理一下，不要释放。
		RoutedEventArgs *pe = (RoutedEventArgs *)lParam;
		if (pe->RoutedEvent == WM_CAPTURECHANGING)
		{
			if (pe->hOriginalSender == plb->m_hVerticalScrollBar
				|| pe->hOriginalSender == plb->m_hHorizontalScrollBar)
			{
				pe->bHandled = TRUE;
				return 1;
			}
		}
	}
	break;

	case WM_NCCALCSIZE:
	{
		BOOL bCalcValidRects = wParam;
		CIUIRect m_rcMargin(1, 1, 1, 1);
		if (bCalcValidRects)
		{
			NCCALCSIZE_PARAMS *pncp = (NCCALCSIZE_PARAMS *)lParam;
			pncp->rgrc[0].left += m_rcMargin.left;
			pncp->rgrc[0].top += m_rcMargin.top;
			pncp->rgrc[0].right -= m_rcMargin.right;
			pncp->rgrc[0].bottom -= m_rcMargin.bottom;

			return 0;
		}
		else
		{
			LPRECT lpRect = LPRECT(lParam);

			lpRect->left += m_rcMargin.left;
			lpRect->top += m_rcMargin.top;
			lpRect->right -= m_rcMargin.right;
			lpRect->bottom -= m_rcMargin.bottom;

			return 0;
		}
	}
	goto CallDWP;
	break;

	case WM_GET_PROJECT:
		return (LRESULT)(IUIGetModuleState()->m_pProject);

	default:
	CallDWP:
		return DefWindowProc(pwnd, message, wParam, lParam);
	}

	/*
	* Handle translation of ANSI output data and free buffer
	*/
	if (lpwsz)
	{
		UserLocalFree(lpwsz);
	}

	return lReturn;
}


/***************************************************************************\
\***************************************************************************/

LRESULT WINAPI ListBoxWndProcW(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	PWND pwnd = hwnd;

	//if ((pwnd = ValidateHwnd(hwnd)) == NULL)
	//{
	//	return (0L);
	//}

	/*
	* If the control is not interested in this message,
	* pass it to DefWindowProc.
	*/
	//if (!FWINDOWMSG(message, FNID_LISTBOX))
	//{
	//	return DefWindowProc(pwnd, message, wParam, lParam);
	//}

	return ListBoxWndProcWorker(pwnd, message, wParam, lParam, FALSE);
}

/***************************************************************************\
* GetLpszItem
*
* Returns a far pointer to the string belonging to item sItem
* ONLY for Listboxes maintaining their own strings (pLBIV->fHasStrings == TRUE)
*
* History:
\***************************************************************************/

LPWSTR GetLpszItem(
	PLBIV pLBIV,
	INT sItem)
{
	LONG offsz;
	lpLBItem plbi;

	if (sItem < 0 || sItem >= pLBIV->cMac)
	{
		//RIPERR1(ERROR_INVALID_PARAMETER,
		//	RIP_WARNING,
		//	"Invalid parameter \"sItem\" (%ld) to GetLpszItem",
		//	sItem);

		return NULL;
	}

	/*
	* get pointer to item index array
	* NOTE: NOT OWNERDRAW
	*/
	plbi = (lpLBItem)(pLBIV->rgpch);
	offsz = plbi[sItem].offsz;
	return (LPWSTR)((PBYTE)(pLBIV->hStrings) + offsz);
}


///////////////////////////////////////////////////////////////////////

BOOL NtUserSetWindowFNID(
	IN HWND hwnd,
	IN WORD fnid)
{
	//BEGINRECV_HWND(BOOL, FALSE, hwnd);

	///*
	// * Don't let apps mess with windows on other processes.
	// */
	//if (GETPTI(pwnd)->ppi != PpiCurrent())
	//{
	//	MSGERROR(0);
	//}

	///*
	// * Make sure the fnid is in the correct range.
	// */
	//if (fnid != FNID_CLEANEDUP_BIT)
	//{
	//	if ((fnid < FNID_CONTROLSTART) || (fnid > FNID_CONTROLEND) || (GETFNID(pwnd) != 0))
	//	{
	//		MSGERROR(0);
	//	}
	//}

	///*
	// * Remember what window class this window belongs to.  Can't use
	// * the real class because any app can call CallWindowProc()
	// * directly no matter what the class is!
	// */
	//pwnd->fnid |= fnid;
	//retval = TRUE;

	//TRACE("NtUserSetWindowFNID");
	//ENDRECV_HWND();

	return FALSE;
}

int IUI::GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient)
{
	int cBorders = 0;

	if (fWindow)
	{
		//
		// Is there a 3D border around the window?
		//
		if (dwExStyle & WS_EX_WINDOWEDGE)
		{
			cBorders += 2;
		}
		else if (dwExStyle & WS_EX_STATICEDGE)
		{
			++cBorders;
		}

		//
		// Is there a single flat border around the window?  This is true for
		// WS_BORDER, WS_DLGFRAME, and WS_EX_DLGMODALFRAME windows.
		//
		if ((lStyle & WS_CAPTION) || (dwExStyle & WS_EX_DLGMODALFRAME))
		{
			++cBorders;
		}

		//
		// Is there a sizing flat border around the window?
		//
		//if (lStyle & WS_SIZEBOX)
		//{
		//	cBorders += gpsi->gclBorder;
		//}
	}

	if (fClient)
	{
		//
		// Is there a 3D border around the client?
		//
		if (dwExStyle & WS_EX_CLIENTEDGE)
		{
			cBorders += 2;
		}
	}

	return (cBorders);
}

//////////////////////////////////////////////////////////////////////////
// List Box Handling Routines
//////////////////////////////////////////////////////////////////////////

#pragma hdrstop

INT xxxLBBinarySearchString(PLBIV plb, LPWSTR lpstr);

/***************************************************************************\
*
*  SetLBScrollParms()
*
*  Sets the scroll range, page, and position
*
\***************************************************************************/

int xxxSetLBScrollParms(PLBIV plb, int nCtl)
{
	int         iPos;
	int         cItems;
	UINT        iPage;
	SCROLLINFO  si;
	BOOL        fNoScroll = FALSE;
	PSCROLLPOS  psp;
	BOOL        fCacheInitialized;
	int         iReturn;

	if (nCtl == SB_VERT)
	{
		iPos = plb->iTop;
		cItems = plb->cMac;
		iPage = plb->cItemFullMax;
		if (!plb->fVertBar)
		{
			fNoScroll = TRUE;
		}
		psp = &plb->VPos;
		fCacheInitialized = plb->fVertInitialized;
	}
	else
	{
		CIUIRect rcClient;
		GetClientRect(plb->spwnd, rcClient);
		if (plb->fMultiColumn)
		{
			iPos = plb->iTop / plb->itemsPerColumn;
			cItems = plb->cMac ? ((plb->cMac - 1) / plb->itemsPerColumn) + 1 : 0;
			iPage = plb->numberOfColumns;
			if (plb->fRightAlign && cItems)
			{
				iPos = cItems - iPos - 1;
			}
		}
		else
		{
			iPos = plb->xOrigin;
			cItems = plb->maxWidth;
			iPage = rcClient.right - rcClient.left;
		}

		if (!plb->fHorzBar)
		{
			fNoScroll = TRUE;
		}
		psp = &plb->HPos;
		fCacheInitialized = plb->fHorzInitialized;
	}

	if (cItems)
	{
		cItems--;
	}

	if (fNoScroll)
	{
		// Limit page to 0, posMax + 1
		iPage = max(min((int)iPage, cItems + 1), 0);

		// Limit pos to 0, posMax - (page - 1).
		return (max(min(iPos, cItems - ((iPage) ? (int)(iPage - 1) : 0)), 0));
	}
	else
	{
		si.fMask = SIF_ALL;
		if (plb->fDisableNoScroll)
		{
			si.fMask |= SIF_DISABLENOSCROLL;
		}

		/*
		* If the scrollbar is already where we want it, do nothing.
		*/
		if (fCacheInitialized)
		{
			if (psp->fMask == si.fMask &&
				psp->cItems == cItems && psp->iPage == iPage &&
				psp->iPos == iPos)
			{
				return psp->iReturn;
			}
		}
		else if (nCtl == SB_VERT)
		{
			plb->fVertInitialized = TRUE;
		}
		else
		{
			plb->fHorzInitialized = TRUE;
		}

		si.cbSize = sizeof(SCROLLINFO);
		si.nMin = 0;
		si.nMax = cItems;
		si.nPage = iPage;

		if (plb->fMultiColumn && plb->fRightAlign)
		{
			si.nPos = (iPos + 1) > (int)iPage ? iPos - iPage + 1 : 0;
		}
		else
		{
			si.nPos = iPos;
		}


		// 设置原生滚动条信息
		iReturn = SetScrollInfo(plb->spwnd, nCtl, &si, plb->fRedraw);

		// 设置自定义滚动条信息：
		// 先设置滚动条位置，因为下面为滚动条设置SetScrollInfo时，会
		// 利用滚动条坐标，计算上下按钮及滑块坐标
		LayoutScrollBar(plb);

		// MSDN中的SetScrollInfo不允许设置Track pos
		si.fMask &= ~SIF_TRACKPOS;

		// 把滚动信息设置到滚动条控件上
		HWLWND hScrollBar = (nCtl == SB_HORZ ? plb->m_hHorizontalScrollBar : plb->m_hVerticalScrollBar);
		if (hScrollBar != NULL)
		{
			SCROLLINFO si2;
			si2 = si;

			// 因为SBM_SETSCROLLINFO内部对SCROLLINFO的值进行合法性检查时，可能会修改它的值
			// 而外面调用SetScrollInfo后，可能还要使用lpsi，所以，这里传一个lpsi的拷贝给
			// SBM_SETSCROLLINFO消息。 (解决调整Header 最右侧Item的右分隔线时，ListView水平滚动
			// 错乱的问题)
			SendMessage(hScrollBar, SBM_SETSCROLLINFO, TRUE, (LPARAM)&si2);
		}

		if (plb->fMultiColumn && plb->fRightAlign)
		{
			iReturn = cItems - (iReturn + iPage - 1);
		}

		/*
		* Update the position cache
		*/
		psp->fMask = si.fMask;
		psp->cItems = cItems;
		psp->iPage = iPage;
		psp->iPos = iPos;
		psp->iReturn = iReturn;

		return iReturn;
	}
}

/***************************************************************************\
* xxxLBShowHideScrollBars
*
* History:
\***************************************************************************/

void xxxLBShowHideScrollBars(
	PLBIV plb)
{
	BOOL fVertDone = FALSE;
	BOOL fHorzDone = FALSE;

	// Don't do anything if there are no scrollbars or if parents
	// are invisible.
	if ((!plb->fHorzBar && !plb->fVertBar) || !plb->fRedraw)
	{
		return;
	}

	//
	// Adjust iTop if necessary but DO NOT REDRAW PERIOD.  We never did
	// in 3.1.  There's a potential bug:
	//      If someone doesn't have redraw off and inserts an item in the
	// same position as the caret, we'll tell them to draw before they may
	// have called LB_SETITEMDATA for their item.  This is because we turn
	// the caret off & on inside of NewITop(), even if the item isn't
	// changing.
	//      So we just want to reflect the position/scroll changes.
	// CheckRedraw() will _really_ redraw the visual changes later if
	// redraw isn't off.
	//

	if (!plb->fFromInsert)
	{
		xxxNewITop(plb, plb->iTop);
		fVertDone = TRUE;
	}

	if (!plb->fMultiColumn)
	{
		if (!plb->fFromInsert)
		{
			fHorzDone = TRUE;
			xxxLBoxCtlHScroll(plb, SB_THUMBPOSITION, plb->xOrigin);
		}

		if (!fVertDone)
		{
			xxxSetLBScrollParms(plb, SB_VERT);
		}
	}
	if (!fHorzDone)
	{
		xxxSetLBScrollParms(plb, SB_HORZ);
	}
}

/***************************************************************************\
* LBGetItemData
*
* returns the long value associated with listbox items. -1 if error
*
* History:
* 16-Apr-1992 beng      The NODATA listbox case
\***************************************************************************/

LONG_PTR LBGetItemData(
	PLBIV plb,
	INT sItem)
{
	LONG_PTR buffer;
	LPBYTE lpItem;

	if (sItem < 0 || sItem >= plb->cMac)
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return LB_ERR;
	}

	// No-data listboxes always return 0L
	//
	if (!plb->fHasData)
	{
		return 0L;
	}

	lpItem = (plb->rgpch +
		(sItem * (plb->fHasStrings ? sizeof(LBItem) : sizeof(LBODItem))));
	buffer = (plb->fHasStrings ? ((lpLBItem)lpItem)->itemData : ((lpLBODItem)lpItem)->itemData);
	return buffer;
}

/***************************************************************************\
* LBGetText
*
* Copies the text associated with index to lpbuffer and returns its length.
* If fLengthOnly, just return the length of the text without doing a copy.
*
* Waring: for size only querries lpbuffer is the count of ANSI characters
*
* Returns count of chars
*
* History:
\***************************************************************************/

INT LBGetText(
	PLBIV plb,
	BOOL fLengthOnly,
	BOOL fAnsi,
	INT index,
	LPWSTR lpbuffer)
{
	LPWSTR lpItemText;
	INT cchText;

	if (index < 0 || index >= plb->cMac)
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return LB_ERR;
	}

	if (!plb->fHasStrings && plb->OwnerDraw)
	{

		/*
		* Owner draw without strings so we must copy the app supplied DWORD
		* value.
		*/
		cchText = sizeof(ULONG_PTR);

		if (!fLengthOnly)
		{
			LONG_PTR UNALIGNED *p = (LONG_PTR UNALIGNED *)lpbuffer;
			*p = LBGetItemData(plb, index);
		}
	}
	else
	{
		lpItemText = GetLpszItem(plb, index);
		if (!lpItemText)
		{
			return LB_ERR;
		}

		/*
		* These are strings so we are copying the text and we must include
		* the terminating 0 when doing the RtlMoveMemory.
		*/
		cchText = wcslen(lpItemText);

		if (fLengthOnly)
		{
			if (fAnsi)
			{
				//RtlUnicodeToMultiByteSize(&cchText, lpItemText, cchText * sizeof(WCHAR));
			}
		}
		else
		{
			if (fAnsi)
			{
#ifdef FE_SB // LBGetText()
				cchText = WCSToMB(lpItemText, cchText + 1, &((LPSTR)lpbuffer), (cchText + 1) * sizeof(WORD), FALSE);
				/*
				* Here.. cchText contains null-terminate char, subtract it... Because, we pass cchText+1 to
				* above Unicode->Ansi convertsion to make sure the string is terminated with null.
				*/
				cchText--;
#else
				//WCSToMB(lpItemText, cchText + 1, &((LPSTR)lpbuffer), cchText + 1, FALSE);
#endif // FE_SB
			}
			else
			{
				RtlCopyMemory(lpbuffer, lpItemText, (cchText + 1) * sizeof(WCHAR));
			}
		}

	}

	return cchText;
}

/***************************************************************************\
* GrowMem
*
* History:
* 16-Apr-1992 beng      NODATA listboxes
* 23-Jul-1996 jparsons  Added numItems parameter for LB_INITSTORAGE support
\***************************************************************************/

BOOL GrowMem(
	PLBIV plb,
	INT   numItems)

{
	LONG cb;
	HANDLE hMem;

	/*
	* Allocate memory for pointers to the strings.
	*/
	cb = (plb->cMax + numItems) *
		(plb->fHasStrings ? sizeof(LBItem)
			: (plb->fHasData ? sizeof(LBODItem)
				: 0));

	/*
	* If multiple selection list box (MULTIPLESEL or EXTENDEDSEL), then
	* allocate an extra byte per item to keep track of it's selection state.
	*/
	if (plb->wMultiple != SINGLESEL)
	{
		cb += (plb->cMax + numItems);
	}

	/*
	* Extra bytes for each item so that we can store its height.
	*/
	if (plb->OwnerDraw == OWNERDRAWVAR)
	{
		cb += (plb->cMax + numItems);
	}

	/*
	* Don't allocate more than 2G of memory
	*/
	if (cb > MAXLONG)
	{
		return FALSE;
	}

	if (plb->rgpch == NULL)
	{
		if ((plb->rgpch = (PBYTE)UserLocalAlloc(HEAP_ZERO_MEMORY, (LONG)cb)) == NULL)
		{
			return FALSE;
		}
	}
	else
	{
		if ((hMem = UserLocalReAlloc(plb->rgpch, (LONG)cb, HEAP_ZERO_MEMORY)) == NULL)
		{
			return FALSE;
		}
		plb->rgpch = (PBYTE)hMem;
	}

	plb->cMax += numItems;

	return TRUE;
}

/***************************************************************************\
* xxxLBInitStorage
*
* History:
* 23-Jul-1996 jparsons  Added support for pre-allocation
\***************************************************************************/
LONG xxxLBInitStorage(PLBIV plb, BOOL fAnsi, INT cItems, INT cb)
{
	HANDLE hMem;
	INT    cbChunk;

	/*
	* if the app is talking ANSI, then adjust for the worst case in unicode
	* where each single ansi byte translates to one 16 bit unicode value
	*/
	if (fAnsi)
	{
		cb *= sizeof(WCHAR);
	} /* if */

	  /*
	  * Fail if either of the parameters look bad.
	  */
	if ((cItems < 0) || (cb < 0))
	{
		xxxNotifyOwner(plb, LBN_ERRSPACE);
		return LB_ERRSPACE;
	} /* if */

	  /*
	  * try to grow the pointer array (if necessary) accounting for the free space
	  * already available.
	  */
	cItems -= plb->cMax - plb->cMac;
	if ((cItems > 0) && !GrowMem(plb, cItems))
	{
		xxxNotifyOwner(plb, LBN_ERRSPACE);
		return LB_ERRSPACE;
	} /* if */

	  /*
	  * now grow the string space if necessary
	  */
	if (plb->fHasStrings)
	{
		if ((cbChunk = (plb->ichAlloc + cb)) > plb->cchStrings)
		{

			/*
			* Round up to the nearest 256 byte chunk.
			*/
			cbChunk = (cbChunk & ~0xff) + 0x100;
			if (!(hMem = UserLocalReAlloc(plb->hStrings, (LONG)cbChunk, 0)))
			{
				xxxNotifyOwner(plb, LBN_ERRSPACE);
				return LB_ERRSPACE;
			}
			plb->hStrings = (LPWSTR)hMem;
			plb->cchStrings = cbChunk;
		} /* if */
	} /* if */

	  /*
	  * return the number of items that can be stored
	  */
	return plb->cMax;
}

/***************************************************************************\
* xxxInsertString
*
* Insert an item at a specified position.
*
* History:
* 16-Apr-1992 beng      NODATA listboxes
\***************************************************************************/

INT xxxLBInsertItem(
	PLBIV plb,

	/*
	* For owner draw listboxes without LBS_HASSTRINGS style, this is not a
	* string but rather a 4 byte value we will store for the app.
	*/
	LPWSTR lpsz,
	INT index,
	UINT wFlags)
{
	MEASUREITEMSTRUCT measureItemStruct;
	INT cbString;
	INT cbChunk;
	PBYTE lp;
	PBYTE lpT;
	PBYTE lpHeightStart;
	LONG cbItem;     /* sizeof the Item in rgpch */
	HANDLE hMem;
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	if (wFlags & LBI_ADD)
	{
		index = (plb->fSort) ? xxxLBBinarySearchString(plb, lpsz) : -1;
	}

	if (plb->rgpch == NULL)
	{
		if (index != 0 && index != -1)
		{
			//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
			return LB_ERR;
		}

		plb->iSel = -1;
		plb->iSelBase = 0;
		plb->cMax = 0;
		plb->cMac = 0;
		plb->iTop = 0;
		plb->rgpch = (PBYTE)UserLocalAlloc(HEAP_ZERO_MEMORY, 0L);
		if (plb->rgpch == NULL)
		{
			return LB_ERR;
		}
	}

	if (index == -1)
	{
		index = plb->cMac;
	}

	if (index > plb->cMac || plb->cMac >= MAXLONG)
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return LB_ERR;
	}

	if (plb->fHasStrings)
	{

		/*
		* we must store the string in the hStrings memory block.
		*/
		cbString = (wcslen(lpsz) + 1) * sizeof(WCHAR); /* include 0 terminator */

		if ((cbChunk = (plb->ichAlloc + cbString)) > plb->cchStrings)
		{

			/*
			* Round up to the nearest 256 byte chunk.
			*/
			cbChunk = (cbChunk & ~0xff) + 0x100;
			if (!(hMem = UserLocalReAlloc(plb->hStrings, (LONG)cbChunk,
				0)))
			{
				xxxNotifyOwner(plb, LBN_ERRSPACE);
				return LB_ERRSPACE;
			}
			plb->hStrings = (LPWSTR)hMem;

			plb->cchStrings = cbChunk;
		}

		/*
		* Note difference between Win 95 code with placement of new string
		*/
		if (wFlags & UPPERCASE)
		{
			CharUpperBuffW((LPWSTR)lpsz, cbString / sizeof(WCHAR));
		}
		else if (wFlags & LOWERCASE)
		{
			CharLowerBuffW((LPWSTR)lpsz, cbString / sizeof(WCHAR));
		}

		lp = (PBYTE)(plb->hStrings);
		RtlMoveMemory(lp + plb->ichAlloc, lpsz, cbString);
	}

	/*
	* Now expand the pointer array.
	*/
	if (plb->cMac >= plb->cMax)
	{
		if (!GrowMem(plb, CITEMSALLOC))
		{
			xxxNotifyOwner(plb, LBN_ERRSPACE);
			return LB_ERRSPACE;
		}
	}

	lpHeightStart = lpT = lp = plb->rgpch;

	/*
	* Now calculate how much room we must make for the string pointer (lpsz).
	* If we are ownerdraw without LBS_HASSTRINGS, then a single DWORD
	* (LBODItem.itemData) stored for each item, but if we have strings with
	* each item then a LONG string offset (LBItem.offsz) is also stored.
	*/
	cbItem = (plb->fHasStrings ? sizeof(LBItem)
		: (plb->fHasData ? sizeof(LBODItem) : 0));
	cbChunk = (plb->cMac - index) * cbItem;

	if (plb->wMultiple != SINGLESEL)
	{

		/*
		* Extra bytes were allocated for selection flag for each item
		*/
		cbChunk += plb->cMac;
	}

	if (plb->OwnerDraw == OWNERDRAWVAR)
	{

		/*
		* Extra bytes were allocated for each item's height
		*/
		cbChunk += plb->cMac;
	}

	/*
	* First, make room for the 2 byte pointer to the string or the 4 byte app
	* supplied value.
	*/
	lpT += (index * cbItem);
	RtlMoveMemory(lpT + cbItem, lpT, cbChunk);
	if (!plb->fHasStrings && plb->OwnerDraw)
	{
		if (plb->fHasData)
		{
			/*
			* Ownerdraw so just save the DWORD value
			*/
			lpLBODItem p = (lpLBODItem)lpT;
			p->itemData = (ULONG_PTR)lpsz;
		}
	}
	else
	{
		lpLBItem p = ((lpLBItem)lpT);

		/*
		* Save the start of the string.  Let the item data field be 0
		*/
		p->offsz = (LONG)(plb->ichAlloc);
		p->itemData = 0;
		plb->ichAlloc += cbString;
	}

	/*
	* Now if Multiple Selection lbox, we have to insert a selection status
	* byte.  If var height ownerdraw, then we also have to move up the height
	* bytes.
	*/
	if (plb->wMultiple != SINGLESEL)
	{
		lpT = lp + ((plb->cMac + 1) * cbItem) + index;
		RtlMoveMemory(lpT + 1, lpT, plb->cMac - index +
			(plb->OwnerDraw == OWNERDRAWVAR ? plb->cMac : 0));
		*lpT = 0;  /* fSelected = FALSE */
	}

	/*
	* Increment count of items in the listbox now before we send a message to
	* the app.
	*/
	plb->cMac++;

	/*
	* If varheight ownerdraw, we much insert an extra byte for the item's
	* height.
	*/
	if (plb->OwnerDraw == OWNERDRAWVAR)
	{

		/*
		* Variable height owner draw so we need to get the height of each item.
		*/
		lpHeightStart += (plb->cMac * cbItem) + index +
			(plb->wMultiple ? plb->cMac : 0);

		RtlMoveMemory(lpHeightStart + 1, lpHeightStart, plb->cMac - 1 - index);

		/*
		* Query for item height only if we are var height owner draw.
		*/
		measureItemStruct.CtlType = ODT_LISTBOX;
		measureItemStruct.CtlID = 0;//L PtrToUlong(plb->spwnd->spmenu);
		measureItemStruct.itemID = index;

		/*
		* System font height is default height
		*/
		measureItemStruct.itemHeight = 40;//(UINT)gpsi->cySysFontChar;
		measureItemStruct.itemData = (ULONG_PTR)lpsz;

		/*
		* If "has strings" then add the special thunk bit so the client data
		* will be thunked to a client side address.  LB_DIR sends a string
		* even if the listbox is not HASSTRINGS so we need to special
		* thunk this case.  HP Dashboard for windows send LB_DIR to a non
		* HASSTRINGS listbox needs the server string converted to client.
		* WOW needs to know about this situation as well so we mark the
		* previously uninitialized itemWidth as FLAT.
		*/
		if (plb->fHasStrings || (wFlags & MSGFLAG_SPECIAL_THUNK))
		{
			measureItemStruct.itemWidth = 100;//L MIFLAG_FLAT;
		}

		ThreadLock(plb->spwndParent, &tlpwndParent);
		SendMessage(HW(plb->spwndParent),
			WM_MEASUREITEM,
			measureItemStruct.CtlID,
			(LPARAM)&measureItemStruct);
		ThreadUnlock(&tlpwndParent);
		*lpHeightStart = (BYTE)measureItemStruct.itemHeight;
	}


	/*
	* If the item was inserted above the current selection then move
	* the selection down one as well.
	*/
	if ((plb->wMultiple == SINGLESEL) && (plb->iSel >= index))
	{
		plb->iSel++;
	}

	//if (plb->OwnerDraw == OWNERDRAWVAR)
	//{
	//	LBSetCItemFullMax(plb);
	//}

	/*
	* Check if scroll bars need to be shown/hidden
	*/
	plb->fFromInsert = TRUE;
	xxxLBShowHideScrollBars(plb);
	if (plb->fHorzBar && plb->fRightAlign && !(plb->fMultiColumn || plb->OwnerDraw))
	{
		/*
		* origin to right
		*/
		xxxLBoxCtlHScroll(plb, SB_BOTTOM, 0);
	}
	plb->fFromInsert = FALSE;

	xxxCheckRedraw(plb, TRUE, index);

	//if (FWINABLE())
	//{
	//	LBEvent(plb, EVENT_OBJECT_CREATE, index);
	//}

	return index;
}


/***************************************************************************\
* LBlstrcmpi
*
* This is a version of lstrcmpi() specifically used for listboxes
* This gives more weight to '[' characters than alpha-numerics;
* The US version of lstrcmpi() and lstrcmp() are similar as far as
* non-alphanumerals are concerned; All non-alphanumerals get sorted
* before alphanumerals; This means that subdirectory strings that start
* with '[' will get sorted before; But we don't want that; So, this
* function takes care of it;
*
* History:
\***************************************************************************/

INT LBlstrcmpi(
	LPWSTR lpStr1,
	LPWSTR lpStr2,
	DWORD dwLocaleId)
{

	/*
	* NOTE: This function is written so as to reduce the number of calls
	* made to the costly IsCharAlphaNumeric() function because that might
	* load a language module; It 'traps' the most frequently occurring cases
	* like both strings starting with '[' or both strings NOT starting with '['
	* first and only in abosolutely necessary cases calls IsCharAlphaNumeric();
	*/
	if (*lpStr1 == TEXT('['))
	{
		if (*lpStr2 == TEXT('['))
		{
			goto LBL_End;
		}
		if (IsCharAlphaNumericW(*lpStr2))
		{
			return 1;
		}
	}

	if ((*lpStr2 == TEXT('[')) && IsCharAlphaNumericW(*lpStr1))
	{
		return -1;
	}

LBL_End:
	//if ((GetClientInfo()->dwTIFlags & TIF_16BIT) &&
	//	dwLocaleId == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US))
	//{
	//	/*
	//	 * This is how Windows95 does, bug #4199
	//	 */
	//	return (*pfnWowIlstrcmp)(lpStr1, lpStr2);
	//}
	return (INT)CompareStringW((LCID)dwLocaleId, NORM_IGNORECASE,
		lpStr1, -1, lpStr2, -1) - 2;
}


/***************************************************************************\
* xxxLBBinarySearchString
*
* Does a binary search of the items in the SORTED listbox to find
* out where this item should be inserted.  Handles both HasStrings and item
* long WM_COMPAREITEM cases.
*
* History:
*    27 April 1992  GregoryW
*          Modified to support sorting based on current list box locale.
\***************************************************************************/

INT xxxLBBinarySearchString(
	PLBIV plb,
	LPWSTR lpstr)
{
	BYTE **lprgpch;
	INT sortResult;
	COMPAREITEMSTRUCT cis;
	LPWSTR pszLBBase = NULL;
	LPWSTR pszLB;
	INT itemhigh;
	INT itemnew = 0;
	INT itemlow = 0;
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	if (!plb->cMac)
	{
		return 0;
	}

	lprgpch = (BYTE **)(plb->rgpch);
	if (plb->fHasStrings)
	{
		pszLBBase = plb->hStrings;
	}

	itemhigh = plb->cMac - 1;
	while (itemlow <= itemhigh)
	{
		itemnew = (itemhigh + itemlow) / 2;

		if (plb->fHasStrings)
		{

			/*
			* Searching for string matches.
			*/
			pszLB = (LPWSTR)((LPSTR)pszLBBase + ((lpLBItem)lprgpch)[itemnew].offsz);
			sortResult = LBlstrcmpi(pszLB, lpstr, plb->dwLocaleId);
		}
		else
		{

			/*
			* Send compare item messages to the parent for sorting
			*/
			cis.CtlType = ODT_LISTBOX;
			cis.CtlID = 0;//L PtrToUlong(plb->spwnd->spmenu);
			cis.hwndItem = HWq(plb->spwnd);
			cis.itemID1 = itemnew;
			cis.itemData1 = ((lpLBODItem)lprgpch)[itemnew].itemData;
			cis.itemID2 = (UINT)(-1);
			cis.itemData2 = (ULONG_PTR)lpstr;
			cis.dwLocaleId = plb->dwLocaleId;
			ThreadLock(plb->spwndParent, &tlpwndParent);
			sortResult = (INT)SendMessage(HW(plb->spwndParent), WM_COMPAREITEM,
				cis.CtlID, (LPARAM)&cis);
			ThreadUnlock(&tlpwndParent);
		}

		if (sortResult < 0)
		{
			itemlow = itemnew + 1;
		}
		else if (sortResult > 0)
		{
			itemhigh = itemnew - 1;
		}
		else
		{
			itemlow = itemnew;
			goto FoundIt;
		}
	}

FoundIt:

	return max(0, itemlow);
}

/***************************************************************************\
* xxxLBResetContent
*
* History:
\***************************************************************************/

BOOL xxxLBResetContent(
	PLBIV plb)
{
	if (!plb->cMac)
	{
		return TRUE;
	}

	//L xxxLBoxDoDeleteItems(plb);

	if (plb->rgpch != NULL)
	{
		//UserLocalFree(plb->rgpch);
		plb->rgpch = NULL;
	}

	if (plb->hStrings != NULL)
	{
		//UserLocalFree(plb->hStrings);
		plb->hStrings = NULL;
	}

	//InitHStrings(plb);

	//if (TestWF(plb->spwnd, WFWIN31COMPAT))
	//{
	//	xxxCheckRedraw(plb, FALSE, 0);
	//}
	//else if (IsVisible(plb->spwnd))
	{
		InvalidateRect(HWq(plb->spwnd), NULL, TRUE);
	}

	plb->iSelBase = 0;
	plb->iTop = 0;
	plb->cMac = 0;
	plb->cMax = 0;
	plb->xOrigin = 0;
	plb->iLastSelection = 0;
	plb->iSel = -1;

	xxxLBShowHideScrollBars(plb);
	return TRUE;
}


/***************************************************************************\
* xxxLBoxCtlDelete
*
* History:
* 16-Apr-1992 beng      NODATA listboxes
\***************************************************************************/

INT xxxLBoxCtlDelete(
	PLBIV plb,
	INT sItem)  /* Item number to delete */
{
	LONG cb;
	LPBYTE lp;
	LPBYTE lpT;
	RECT rc;
	int cbItem;    /* size of Item in rgpch */
	LPWSTR lpString;
	PBYTE pbStrings;
	INT cbStringLen;
	LPBYTE itemNumbers;
	INT sTmp;
	TL tlpwnd;

	CheckLock(plb->spwnd);

	if (sItem < 0 || sItem >= plb->cMac)
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return LB_ERR;
	}

	//if (FWINABLE())
	//{
	//	LBEvent(plb, EVENT_OBJECT_DESTROY, sItem);
	//}

	if (plb->cMac == 1)
	{

		/*
		* When the item count is 0, we send a resetcontent message so that we
		* can reclaim our string space this way.
		*/
		SendMessage(plb->spwnd, LB_RESETCONTENT, 0, 0);
		goto FinishUpDelete;
	}

	/*
	* Get the rectangle associated with the last item in the listbox.  If it is
	* visible, we need to invalidate it.  When we delete an item, everything
	* scrolls up to replace the item deleted so we must make sure we erase the
	* old image of the last item in the listbox.
	*/
	if (LBGetItemRect(plb, (INT)(plb->cMac - 1), &rc))
	{
		xxxLBInvalidateRect(plb, &rc, TRUE);
	}

	// 3.1 and earlier used to only send WM_DELETEITEMs if it was an ownerdraw
	// listbox.  4.0 and above will send WM_DELETEITEMs for every item that has
	// nonzero item data.
	if (/*TestWF(plb->spwnd, WFWIN40COMPAT) || */(plb->OwnerDraw && plb->fHasData))
	{
		xxxLBoxDeleteItem(plb, sItem);
	}

	plb->cMac--;

	cbItem = (plb->fHasStrings ? sizeof(LBItem)
		: (plb->fHasData ? sizeof(LBODItem) : 0));
	cb = ((plb->cMac - sItem) * cbItem);

	/*
	* Byte for the selection status of the item.
	*/
	if (plb->wMultiple != SINGLESEL)
	{
		cb += (plb->cMac + 1);
	}

	if (plb->OwnerDraw == OWNERDRAWVAR)
	{

		/*
		* One byte for the height of the item.
		*/
		cb += (plb->cMac + 1);
	}

	/*
	* Might be nodata and singlesel, for instance.
	* but what out for the case where cItem == cMac (and cb == 0).
	*/
	if ((cb != 0) || plb->fHasStrings)
	{
		lp = plb->rgpch;

		lpT = (lp + (sItem * cbItem));

		if (plb->fHasStrings)
		{
			/*
			* If we has strings with each item, then we want to compact the string
			* heap so that we can recover the space occupied by the string of the
			* deleted item.
			*/
			/*
			* Get the string which we will be deleting
			*/
			pbStrings = (PBYTE)(plb->hStrings);
			lpString = (LPWSTR)(pbStrings + ((lpLBItem)lpT)->offsz);
			cbStringLen = (wcslen(lpString) + 1) * sizeof(WCHAR);  /* include null terminator */

																   /*
																   * Now compact the string array
																   */
			plb->ichAlloc = plb->ichAlloc - cbStringLen;

			RtlMoveMemory(lpString, (PBYTE)lpString + cbStringLen,
				plb->ichAlloc + (pbStrings - (LPBYTE)lpString));

			/*
			* We have to update the string pointers in plb->rgpch since all the
			* string after the deleted string have been moved down stringLength
			* bytes.  Note that we have to explicitly check all items in the list
			* box if the string was allocated after the deleted item since the
			* LB_SORT style allows a lower item number to have a string allocated
			* at the end of the string heap for example.
			*/
			itemNumbers = lp;
			for (sTmp = 0; sTmp <= plb->cMac; sTmp++)
			{
				lpLBItem p = (lpLBItem)itemNumbers;
				if ((LPWSTR)(p->offsz + pbStrings) > lpString)
				{
					p->offsz -= cbStringLen;
				}
				p++;
				itemNumbers = (LPBYTE)p;
			}
		}

		/*
		* Now compact the pointers to the strings (or the long app supplied values
		* if ownerdraw without strings).
		*/
		RtlMoveMemory(lpT, lpT + cbItem, cb);

		/*
		* Compress the multiselection bytes
		*/
		if (plb->wMultiple != SINGLESEL)
		{
			lpT = (lp + (plb->cMac * cbItem) + sItem);
			RtlMoveMemory(lpT, lpT + 1, plb->cMac - sItem +
				(plb->OwnerDraw == OWNERDRAWVAR ? plb->cMac + 1 : 0));
		}

		if (plb->OwnerDraw == OWNERDRAWVAR)
		{
			/*
			* Compress the height bytes
			*/
			lpT = (lp + (plb->cMac * cbItem) + (plb->wMultiple ? plb->cMac : 0)
				+ sItem);
			RtlMoveMemory(lpT, lpT + 1, plb->cMac - sItem);
		}

	}

	if (plb->wMultiple == SINGLESEL)
	{
		if (plb->iSel == sItem)
		{
			plb->iSel = -1;

			if (plb->pcbox != NULL)
			{
				ThreadLock(plb->pcbox->spwnd, &tlpwnd);
				xxxCBInternalUpdateEditWindow(plb->pcbox, NULL);
				ThreadUnlock(&tlpwnd);
			}
		}
		else if (plb->iSel > sItem)
		{
			plb->iSel--;
		}
	}

	if ((plb->iMouseDown != -1) && (sItem <= plb->iMouseDown))
	{
		plb->iMouseDown = -1;
	}

	if (plb->iSelBase && sItem == plb->iSelBase)
	{
		plb->iSelBase--;
	}

	if (plb->cMac)
	{
		plb->iSelBase = min(plb->iSelBase, plb->cMac - 1);
	}
	else
	{
		plb->iSelBase = 0;
	}

	if ((plb->wMultiple == EXTENDEDSEL) && (plb->iSel == -1))
	{
		plb->iSel = plb->iSelBase;
	}

	if (plb->OwnerDraw == OWNERDRAWVAR)
	{
		LBSetCItemFullMax(plb);
	}

	/*
	* We always set a new iTop.  The iTop won't change if it doesn't need to
	* but it will change if:  1.  The iTop was deleted or 2.  We need to change
	* the iTop so that we fill the listbox.
	*/
	xxxInsureVisible(plb, plb->iTop, FALSE);

FinishUpDelete:

	/*
	* Check if scroll bars need to be shown/hidden
	*/
	plb->fFromInsert = TRUE;
	xxxLBShowHideScrollBars(plb);
	plb->fFromInsert = FALSE;

	xxxCheckRedraw(plb, TRUE, sItem);
	xxxInsureVisible(plb, plb->iSelBase, FALSE);

	return plb->cMac;
}

/***************************************************************************\
* xxxLBoxDeleteItem
*
* Sends a WM_DELETEITEM message to the owner of an ownerdraw listbox
*
* History:
\***************************************************************************/

void xxxLBoxDeleteItem(
	PLBIV plb,
	INT sItem)
{
	DELETEITEMSTRUCT dis;
	TL tlpwndParent;

	CheckLock(plb->spwnd);
	if (plb->spwnd == NULL)
	{
		return;
	}

	/*
	* Bug 262122 - joejo
	* No need to send message if no data!
	*/
	if (!plb->fHasData)
	{
		return;
	}

	/*
	* Fill the DELETEITEMSTRUCT
	*/
	dis.CtlType = ODT_LISTBOX;
	dis.CtlID = 0;//L PtrToUlong(plb->spwnd->spmenu);
	dis.itemID = sItem;
	dis.hwndItem = HWq(plb->spwnd);

	/*
	* Bug 262122 - joejo
	* Fixed in 93 so that ItemData was passed. For some reason, not
	* merged in.
	*/
	dis.itemData = LBGetItemData(plb, sItem);

	if (plb->spwndParent != NULL)
	{
		ThreadLock(plb->spwndParent, &tlpwndParent);
		SendMessage(HWq(plb->spwndParent), WM_DELETEITEM, dis.CtlID,
			(LPARAM)&dis);
		ThreadUnlock(&tlpwndParent);
	}
}

/**************************************************************************\
* xxxLBSetCount
*
* Sets the number of items in a lazy-eval (fNoData) listbox.
*
* Calling SetCount scorches any existing selection state.  To preserve
* selection state, call Insert/DeleteItem instead.
*
* History
* 16-Apr-1992 beng      Created
\**************************************************************************/

INT xxxLBSetCount(
	PLBIV plb,
	INT cItems)
{
	UINT  cbRequired;
	BOOL    fRedraw;

	CheckLock(plb->spwnd);

	/*
	* SetCount is only valid on lazy-eval ("nodata") listboxes.
	* All other lboxen must add their items one at a time, although
	* they may SetCount(0) via RESETCONTENT.
	*/
	if (plb->fHasStrings || plb->fHasData)
	{
		//RIPERR0(ERROR_SETCOUNT_ON_BAD_LB, RIP_VERBOSE, "");
		return LB_ERR;
	}

	if (cItems == 0)
	{
		SendMessage(plb->spwnd, LB_RESETCONTENT, 0, 0);
		return 0;
	}

	// If redraw isn't turned off, turn it off now
	if (fRedraw = plb->fRedraw)
	{
		xxxLBSetRedraw(plb, FALSE);
	}

	cbRequired = LBCalcAllocNeeded(plb, cItems);

	/*
	* Reset selection and position
	*/
	plb->iSelBase = 0;
	plb->iTop = 0;
	plb->cMax = 0;
	plb->xOrigin = 0;
	plb->iLastSelection = 0;
	plb->iSel = -1;

	if (cbRequired != 0)   // Only if record instance data required
	{

		/*
		* If listbox was previously empty, prepare for the
		* realloc-based alloc strategy ahead.
		*/
		if (plb->rgpch == NULL)
		{
			plb->rgpch = (PBYTE)UserLocalAlloc(HEAP_ZERO_MEMORY, 0L);
			plb->cMax = 0;

			if (plb->rgpch == NULL)
			{
				xxxNotifyOwner(plb, LBN_ERRSPACE);
				return LB_ERRSPACE;
			}
		}

		/*
		* rgpch might not have enough room for the new record instance
		* data, so check and realloc as necessary.
		*/
		if (cItems >= plb->cMax)
		{
			INT    cMaxNew;
			UINT   cbNew;
			HANDLE hmemNew;

			/*
			* Since GrowMem presumes a one-item-at-a-time add schema,
			* SetCount can't use it.  Too bad.
			*/
			cMaxNew = cItems + CITEMSALLOC;
			cbNew = LBCalcAllocNeeded(plb, cMaxNew);
			hmemNew = UserLocalReAlloc(plb->rgpch, cbNew, HEAP_ZERO_MEMORY);

			if (hmemNew == NULL)
			{
				xxxNotifyOwner(plb, LBN_ERRSPACE);
				return LB_ERRSPACE;
			}

			plb->rgpch = (PBYTE)hmemNew;
			plb->cMax = cMaxNew;
		}

		/*
		* Reset the item instance data (multisel annotations)
		*/
		RtlZeroMemory(plb->rgpch, cbRequired);
	}

	plb->cMac = cItems;

	// Turn redraw back on
	if (fRedraw)
	{
		xxxLBSetRedraw(plb, TRUE);
	}

	xxxLBInvalidateRect(plb, NULL, TRUE);
	// Not In Chicago -- FritzS
	//    NtUserSetScrollPos(plb->spwnd, SB_HORZ, 0, plb->fRedraw);
	//    NtUserSetScrollPos(plb->spwnd, SB_VERT, 0, plb->fRedraw);
	xxxLBShowHideScrollBars(plb); // takes care of fRedraw

	return 0;
}

/**************************************************************************\
* LBCalcAllocNeeded
*
* Calculate the number of bytes needed in rgpch to accommodate a given
* number of items.
*
* History
* 16-Apr-1992 beng      Created
\**************************************************************************/

UINT LBCalcAllocNeeded(
	PLBIV plb,
	INT cItems)
{
	UINT cb;

	/*
	* Allocate memory for pointers to the strings.
	*/
	cb = cItems * (plb->fHasStrings ? sizeof(LBItem)
		: (plb->fHasData ? sizeof(LBODItem)
			: 0));

	/*
	* If multiple selection list box (MULTIPLESEL or EXTENDEDSEL), then
	* allocate an extra byte per item to keep track of it's selection state.
	*/
	if (plb->wMultiple != SINGLESEL)
	{
		cb += cItems;
	}

	/*
	* Extra bytes for each item so that we can store its height.
	*/
	if (plb->OwnerDraw == OWNERDRAWVAR)
	{
		cb += cItems;
	}

	return cb;
}

//////////////////////////////////////////////////////////////////////////


#pragma hdrstop

#define LB_KEYDOWN WM_USER+1
#define NOMODIFIER  0  /* No modifier is down */
#define SHIFTDOWN   1  /* Shift alone */
#define CTLDOWN     2  /* Ctl alone */
#define SHCTLDOWN   (SHIFTDOWN + CTLDOWN)  /* Ctrl + Shift */

/*
* Variables for incremental type search support
*/
#define MAX_TYPESEARCH  256

BOOL LBGetDC(PLBIV plb);
void LBReleaseDC(PLBIV plb);

/***************************************************************************\
*
*  LBInvalidateRect()
*
*  If the listbox is visible, invalidates a rectangle in the listbox.
*  If the listbox is not visible, sets the defer update flag for the listbox
*
\***************************************************************************/
BOOL xxxLBInvalidateRect(PLBIV plb, LPRECT lprc, BOOL fErase)
{
	CheckLock(plb->spwnd);

	if (IsWindowVisible(plb->spwnd))
	{
		InvalidateRect(plb->spwnd, lprc, fErase);
		return (TRUE);
	}

	if (!plb->fRedraw)
	{
		plb->fDeferUpdate = TRUE;
	}

	return (FALSE);
}

/***************************************************************************\
*
*  LBGetBrush()
*
*  Gets background brush & colors for listbox.
*
\***************************************************************************/
HBRUSH xxxLBGetBrush(PLBIV plb, HBRUSH *phbrOld)
{
	return CreateSolidBrush(RGB(255, 255, 255));
	//HBRUSH  hbr;
	//HBRUSH  hbrOld;
	//TL tlpwndParent;

	//CheckLock(plb->spwnd);

	//SetBkMode(plb->hdc, OPAQUE);

	////
	//// Get brush & colors
	////
	//if ((GetParent(plb->spwnd) == NULL) ||
	//	(REBASEPWND(plb->spwnd, spwndParent) == _GetDesktopWindow()))
	//{
	//	ThreadLock(plb->spwndParent, &tlpwndParent);
	//	hbr = GetControlColor(HW(plb->spwndParent), HWq(plb->spwnd),
	//			plb->hdc, WM_CTLCOLORLISTBOX);
	//	ThreadUnlock(&tlpwndParent);
	//}
	//else
	//{
	//	hbr = GetControlBrush(HWq(plb->spwnd), plb->hdc, WM_CTLCOLORLISTBOX);
	//}

	////
	//// Select brush into dc
	////
	//if (hbr != NULL)
	//{
	//	hbrOld = SelectObject(plb->hdc, hbr);
	//	if (phbrOld)
	//	{
	//		*phbrOld = hbrOld;
	//	}
	//}

	//return (hbr);
}


/***************************************************************************\
*
*  LBInitDC()
*
*  Initializes dc for listbox
*
\***************************************************************************/
void LBInitDC(PLBIV plb)
{
	RECT    rc;

	// Set font
	if (plb->hFont)
	{
		SelectObject(plb->hdc, plb->hFont);
	}

	// Set clipping area
	GetClientRect(plb->spwnd, &rc);
	IntersectClipRect(plb->hdc, rc.left, rc.top, rc.right, rc.bottom);

	OffsetWindowOrgEx(plb->hdc, plb->xOrigin, 0, NULL);
}


/***************************************************************************\
* LBGetDC
*
* Returns a DC which can be used by a list box even if parentDC is in effect
*
* History:
\***************************************************************************/

BOOL LBGetDC(
	PLBIV plb)
{
	if (plb->hdc)
	{
		return (FALSE);
	}

	plb->hdc = GetDC(plb->spwnd);

	LBInitDC(plb);

	return TRUE;
}

/***************************************************************************\
*
*  LBTermDC()
*
*  Cleans up when done with listbox dc.
*
\***************************************************************************/
void LBTermDC(PLBIV plb)
{
	if (plb->hFont)
	{
		//L SelectObject(plb->hdc, ghFontSys);
	}
}



/***************************************************************************\
* LBReleaseDC
*
* History:
\***************************************************************************/

void LBReleaseDC(
	PLBIV plb)
{
	LBTermDC(plb);
	ReleaseDC(plb->spwnd, plb->hdc);
	plb->hdc = NULL;
}


/***************************************************************************\
* LBGetItemRect
*
* Return the rectangle that the item will be drawn in with respect to the
* listbox window.  Returns TRUE if any portion of the item's rectangle
* is visible (ie. in the listbox client rect) else returns FALSE.
*
* History:
\***************************************************************************/

BOOL LBGetItemRect(
	PLBIV plb,
	INT sItem,
	LPRECT lprc)
{
	INT sTmp;
	int clientbottom;

	/*
	* Always allow an item number of 0 so that we can draw the caret which
	* indicates the listbox has the focus even though it is empty.

	* FreeHand 3.1 passes in -1 as the itemNumber and expects
	* a non-null rectangle. So we check for -1 specifically.
	* BUGTAG: Fix for Bug #540 --Win95B-- SANKAR -- 2/20/95 --
	*/

	if (sItem && (sItem != -1) && ((UINT)sItem >= (UINT)plb->cMac))
	{
		SetRectEmpty(lprc);
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return (LB_ERR);
	}

	GetClientRect(plb->spwnd, lprc);

	if (plb->fMultiColumn)
	{

		/*
		* itemHeight * sItem mod number ItemsPerColumn (itemsPerColumn)
		*/
		lprc->top = plb->cyChar * (sItem % plb->itemsPerColumn);
		lprc->bottom = lprc->top + plb->cyChar  /*+(plb->OwnerDraw ? 0 : 1)*/;

		//UserAssert(plb->itemsPerColumn);

		//if (plb->fRightAlign)
		//{
		//	lprc->right = lprc->right - plb->cxColumn *
		//		((sItem / plb->itemsPerColumn) - (plb->iTop / plb->itemsPerColumn));

		//	lprc->left = lprc->right - plb->cxColumn;
		//}
		//else
		{
			/*
			* Remember, this is integer division here...
			*/
			lprc->left += plb->cxColumn *
				((sItem / plb->itemsPerColumn) - (plb->iTop / plb->itemsPerColumn));

			lprc->right = lprc->left + plb->cxColumn;
		}
	}
	else if (plb->OwnerDraw == OWNERDRAWVAR)
	{

		/*
		* Var height owner draw
		*/
		lprc->right += plb->xOrigin;
		clientbottom = lprc->bottom;

		if (sItem >= plb->iTop)
		{
			for (sTmp = plb->iTop; sTmp < sItem; sTmp++)
			{
				lprc->top = lprc->top + LBGetVariableHeightItemHeight(plb, sTmp);
			}

			/*
			* If item number is 0, it may be we are asking for the rect
			* associated with a nonexistant item so that we can draw a caret
			* indicating focus on an empty listbox.
			*/
			lprc->bottom = lprc->top + (sItem < plb->cMac ? LBGetVariableHeightItemHeight(plb, sItem) : plb->cyChar);
			return (lprc->top < clientbottom);
		}
		else
		{

			/*
			* Item we want the rect of is before plb->iTop.  Thus, negative
			* offsets for the rect and it is never visible.
			*/
			for (sTmp = sItem; sTmp < plb->iTop; sTmp++)
			{
				lprc->top = lprc->top - LBGetVariableHeightItemHeight(plb, sTmp);
			}
			lprc->bottom = lprc->top + LBGetVariableHeightItemHeight(plb, sItem);
			return FALSE;
		}
	}
	else
	{

		/*
		* For fixed height listboxes
		*/
		if (plb->fRightAlign && !(plb->fMultiColumn || plb->OwnerDraw) && plb->fHorzBar)
		{
			lprc->right += plb->xOrigin + (plb->xRightOrigin - plb->xOrigin);
		}
		else
		{
			lprc->right += plb->xOrigin;
		}
		lprc->top = (sItem - plb->iTop) * plb->cyChar;
		lprc->bottom = lprc->top + plb->cyChar;
	}

	return (sItem >= plb->iTop) &&
		(sItem < (plb->iTop + CItemInWindow(plb, TRUE)));
}


/***************************************************************************\
*
*  LBPrintCallback
*
*  Called back from DrawState()
*
\***************************************************************************/
BOOL CALLBACK LBPrintCallback(HDC hdc, LPWSTR lpstr, PLBIV plb, int cx, int cy)
{
	int     xStart;
	UINT    cLen;
	RECT    rc;
	UINT    oldAlign;

	if (!lpstr)
	{
		return FALSE;
	}

	if (plb->fMultiColumn)
	{
		xStart = 0;
	}
	else
	{
		xStart = 2;
	}

	if (plb->fRightAlign)
	{
		oldAlign = SetTextAlign(hdc, TA_RIGHT | GetTextAlign(hdc));
		xStart = cx - xStart;
	}

	cLen = wcslen(lpstr);

	if (plb->fUseTabStops)
	{
		TabTextOut(hdc, xStart, 0, lpstr, cLen,
			(plb->iTabPixelPositions ? plb->iTabPixelPositions[0] : 0),
			(plb->iTabPixelPositions ? (LPINT)&plb->iTabPixelPositions[1] : NULL),
			plb->fRightAlign ? cx : 0, TRUE, GetTextCharset(plb->hdc));
	}
	else
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = cx;
		rc.bottom = cy;

		if (plb->wMultiple)
		{
			ExtTextOutW(hdc, xStart, 0, ETO_OPAQUE, &rc, lpstr, cLen, NULL);
		}
		else if (plb->fMultiColumn)
		{
			ExtTextOutW(hdc, xStart, 0, ETO_CLIPPED, &rc, lpstr, cLen, NULL);
		}
		else
		{
			ExtTextOutW(hdc, xStart, 0, 0, NULL, lpstr, cLen, NULL);

			/*
			* When the listbox is in the incremental search mode and the item
			* is highlighted (so we only draw in the current item), draw the
			* caret for search indication.
			*/
			if ((plb->iTypeSearch != 0) && (plb->OwnerDraw == 0) &&
				(GetBkColor(hdc) == RGB(255, 0, 0)))
			{
				SIZE size;
				GetTextExtentPointW(hdc, lpstr, plb->iTypeSearch, &size);
				PatBlt(hdc, xStart + size.cx - 1, 1, 1, cy - 2, DSTINVERT);
			}
		}
	}

	if (plb->fRightAlign)
	{
		SetTextAlign(hdc, oldAlign);
	}

	return (TRUE);
}


/***************************************************************************\
* xxxLBDrawLBItem
*
* History:
\***************************************************************************/

void xxxLBDrawLBItem(
	PLBIV plb,
	INT sItem,
	LPRECT lprect,
	BOOL fHilite,
	HBRUSH hbr)
{
	DWORD rgbSave;
	HDC hdc = plb->hdc;

	CheckLock(plb->spwnd);
	SetBkMode(plb->hdc, TRANSPARENT);

	// 选中后填充Highlight背景色
	if (fHilite)
	{
		HBRUSH hHoverBrush = CreateSolidBrush(plb->m_crItemBkH);
		FillRect(hdc, lprect, hHoverBrush);
		DeleteObject(hHoverBrush);
		hHoverBrush = NULL;

		rgbSave = SetTextColor(hdc, RGB(0, 0, 0));
	}
	else
	{

		/*
		* If fUseTabStops, we must fill the background, because later we use
		* LBTabTheTextOutForWimps(), which fills the background only partially
		* Fix for Bug #1509 -- 01/25/91 -- SANKAR --
		*/
		if ((hbr != NULL) && ((sItem == plb->iSelBase) || (plb->fUseTabStops)))
		{
			FillRect(hdc, lprect, hbr);
		}
	}

	// 如果Item绑定了控件，则只绘制控件
	HWLWND hBind = LBGetItemBind(plb, sItem);
	if (hBind != NULL)
	{
		MoveWindow(hBind, lprect, FALSE);

		// 备份视口、裁剪区域
		POINT ptOrg;
		GetViewportOrgEx(plb->hdc, &ptOrg);

		HRGN hOldRgn = CreateRectRgn(0, 0, 0, 0);
		GetClipRgn(plb->hdc, hOldRgn);

		// 绘制绑定的控件
		DrawControlAndChildren(hBind, plb->hdc, lprect);

		// 恢复视口：
		// 注意：IUI默认绘制机制是先绘制父控件，等父控件整个绘制完后，再绘制子控件。
		// 虽然子控件绘制时，会修改HDC的视口，但这时父控件已彻底绘制完了，所以子控件
		// 修改HDC视口，也不会影响父控件的绘制。
		// 但由于ListView的特殊性，ListView是在绘制自己的时候，同时把Item绑定的控件
		// 进行了绘制，而在绘制完Item绑定的控件后，HDC的视口已不是ListView的视口了，
		// 而这时，ListView还未绘制完，这将导致ListView之后的绘制位置全部错误。
		SetViewportOrgEx(plb->hdc, ptOrg.x, ptOrg.y, NULL);
		SelectClipRgn(plb->hdc, hOldRgn);
		DeleteObject(hOldRgn);
		hOldRgn = NULL;
	}
	else
	{
		UINT uFlags = DST_COMPLEX;
		LPWSTR lpstr = GetLpszItem(plb, sItem);

		if (!IsWindowEnabled(plb->spwnd))
		{
			uFlags |= DSS_UNION;
		}

		if (plb->fRightAlign)
		{
			uFlags |= DSS_RIGHT;
		}

		UINT  oldAlign;
		if (plb->fRtoLReading)
		{
			oldAlign = SetTextAlign(hdc, TA_RTLREADING | GetTextAlign(hdc));
		}

		DrawTextW(hdc, lpstr, wcslen(lpstr), lprect, uFlags);

		if (plb->fRtoLReading)
		{
			SetTextAlign(hdc, oldAlign);
		}
	}

	if (fHilite)
	{
		SetTextColor(hdc, rgbSave);
	}
}


/***************************************************************************\
*
* LBSetCaret()
*
\***************************************************************************/
void xxxLBSetCaret(PLBIV plb, BOOL fSetCaret)
{
	RECT    rc;
	BOOL    fNewDC;

	if (plb->fCaret && ((BOOL)plb->fCaretOn != !!fSetCaret))
	{
		if (IsWindowVisible(plb->spwnd))
		{
			/* Turn the caret (located at plb->iSelBase) on */
			fNewDC = LBGetDC(plb);

			LBGetItemRect(plb, plb->iSelBase, &rc);

			if (fNewDC)
			{
				SetBkColor(plb->hdc, RGB(0, 0, 0));
				SetTextColor(plb->hdc, RGB(0, 0, 0));
			}

			if (plb->OwnerDraw)
			{
				/* Fill in the drawitem struct */
				UINT itemState = (fSetCaret) ? ODS_FOCUS : 0;

				if (IsSelected(plb, plb->iSelBase, HILITEONLY))
				{
					itemState |= ODS_SELECTED;
				}

				xxxLBoxDrawItem(plb, plb->iSelBase, ODA_FOCUS, itemState, &rc);
			}
			//else if (!TestWF(plb->spwnd, WEFPUIFOCUSHIDDEN))
			//{
			//	COLORREF crBk = SetBkColor(plb->hdc, CreateSolidBrush(RGB(0, 0, 0)));
			//	COLORREF crText = SetTextColor(plb->hdc, CreateSolidBrush(RGB(0, 0, 0)));

			//	DrawFocusRect(plb->hdc, &rc);

			//	SetBkColor(plb->hdc, crBk);
			//	SetTextColor(plb->hdc, crText);
			//}

			if (fNewDC)
			{
				LBReleaseDC(plb);
			}
		}
		plb->fCaretOn = !!fSetCaret;
	}
}


/***************************************************************************\
* IsSelected
*
* History:
* 16-Apr-1992 beng      The NODATA listbox case
\***************************************************************************/

BOOL IsSelected(
	PLBIV plb,
	INT sItem,
	UINT wOpFlags)
{
	LPBYTE lp;

	if ((sItem >= plb->cMac) || (sItem < 0))
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		//        return LB_ERR;
		return (FALSE);
	}

	if (plb->wMultiple == SINGLESEL)
	{
		return (sItem == plb->iSel);
	}

	int nItemSize = 0;
	if (plb->fHasStrings)
	{
		nItemSize = sizeof(LBItem);
	}
	else
	{
		if (plb->fHasData)
		{
			nItemSize = sizeof(LBODItem);
		}
		else
		{
			nItemSize = 0;
		}
	}
	lp = plb->rgpch + sItem + plb->cMac * nItemSize;
	sItem = *lp;

	if (wOpFlags == HILITEONLY)
	{
		sItem >>= 4;
	}
	else
	{
		sItem &= 0x0F;  /* SELONLY */
	}

	return sItem;
}


/***************************************************************************\
* CItemInWindow
*
* Returns the number of items which can fit in a list box.  It
* includes the partially visible one at the bottom if fPartial is TRUE. For
* var height ownerdraw, return the number of items visible starting at iTop
* and going to the bottom of the client rect.
*
* History:
\***************************************************************************/

INT CItemInWindow(
	PLBIV plb,
	BOOL fPartial)
{
	RECT rect;

	if (plb->OwnerDraw == OWNERDRAWVAR)
	{
		return CItemInWindowVarOwnerDraw(plb, fPartial);
	}

	if (plb->fMultiColumn)
	{
		return plb->itemsPerColumn * (plb->numberOfColumns + (fPartial ? 1 : 0));
	}

	GetClientRect(plb->spwnd, &rect);

	/*
	* fPartial must be considered only if the listbox height is not an
	* integral multiple of character height.
	* A part of the fix for Bug #3727 -- 01/14/91 -- SANKAR --
	*/
	//UserAssert(plb->cyChar);
	return (INT)((rect.bottom / plb->cyChar) +
		((rect.bottom % plb->cyChar) ? (fPartial ? 1 : 0) : 0));
}


/***************************************************************************\
* xxxLBoxCtlScroll
*
* Handles vertical scrolling of the listbox
*
* History:
\***************************************************************************/

void xxxLBoxCtlScroll(
	PLBIV plb,
	INT cmd,
	int yAmt)
{
	INT iTopNew;
	INT cItemPageScroll;
	DWORD dwTime = 0;

	CheckLock(plb->spwnd);

	if (plb->fMultiColumn)
	{

		/*
		* Don't allow vertical scrolling on a multicolumn list box.  Needed
		* in case app sends WM_VSCROLL messages to the listbox.
		*/
		return;
	}

	cItemPageScroll = plb->cItemFullMax;

	if (cItemPageScroll > 1)
	{
		cItemPageScroll--;
	}

	if (plb->cMac)
	{
		iTopNew = plb->iTop;
		switch (cmd)
		{
		case SB_LINEUP:
			dwTime = yAmt;
			iTopNew--;
			break;

		case SB_LINEDOWN:
			dwTime = yAmt;
			iTopNew++;
			break;

		case SB_PAGEUP:
			if (plb->OwnerDraw == OWNERDRAWVAR)
			{
				iTopNew = LBPage(plb, plb->iTop, FALSE);
			}
			else
			{
				iTopNew -= cItemPageScroll;
			}
			break;

		case SB_PAGEDOWN:
			if (plb->OwnerDraw == OWNERDRAWVAR)
			{
				iTopNew = LBPage(plb, plb->iTop, TRUE);
			}
			else
			{
				iTopNew += cItemPageScroll;
			}
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{

			/*
			* If the listbox contains more than 0xFFFF items
			* it means that the scrolbar can return a position
			* that cannot fit in a WORD (16 bits), so use
			* GetScrollInfo (which is slower) in this case.
			*/
			if (plb->cMac < 0xFFFF)
			{
				iTopNew = yAmt;
			}
			else
			{
				SCROLLINFO si;

				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_TRACKPOS;

				GetScrollInfo(plb->spwnd, SB_VERT, &si);

				iTopNew = si.nTrackPos;
			}
			break;
		}
		case SB_TOP:
			iTopNew = 0;
			break;

		case SB_BOTTOM:
			iTopNew = plb->cMac - 1;
			break;

		case SB_ENDSCROLL:
			plb->fSmoothScroll = TRUE;
			xxxLBSetCaret(plb, FALSE);
			xxxLBShowHideScrollBars(plb);
			xxxLBSetCaret(plb, TRUE);
			return;
		}

		xxxNewITopEx(plb, iTopNew, dwTime);
	}
}

/***************************************************************************\
* LBGetScrollFlags
*
\***************************************************************************/

DWORD LBGetScrollFlags(PLBIV plb, DWORD dwTime)
{
	DWORD dwFlags;

	//if (GetAppCompatFlags(NULL) & GACF_NOSMOOTHSCROLLING)
	//{
	//	goto NoSmoothScrolling;
	//}

	if (dwTime != 0)
	{
		dwFlags = MAKELONG(SW_SCROLLWINDOW | SW_SMOOTHSCROLL | SW_SCROLLCHILDREN, dwTime);
	}
	//else if (TEST_EffectPUSIF(PUSIF_LISTBOXSMOOTHSCROLLING) && plb->fSmoothScroll)
	//{
	//	dwFlags = SW_SCROLLWINDOW | SW_SMOOTHSCROLL | SW_SCROLLCHILDREN;
	//	plb->fSmoothScroll = FALSE;
	//}
	else
	{
		//NoSmoothScrolling:
		dwFlags = SW_SCROLLWINDOW | SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN;
	}

	return dwFlags;
}

/***************************************************************************\
* xxxLBoxCtlHScroll
*
* Supports horizontal scrolling of listboxes
*
* History:
\***************************************************************************/

void xxxLBoxCtlHScroll(
	PLBIV plb,
	INT cmd,
	int xAmt)
{
	int newOrigin = plb->xOrigin;
	int oldOrigin = plb->xOrigin;
	int windowWidth;
	RECT rc;
	DWORD dwTime = 0;

	CheckLock(plb->spwnd);

	/*
	* Update the window so that we don't run into problems with invalid
	* regions during the horizontal scroll.
	*/
	if (plb->fMultiColumn)
	{

		/*
		* Handle multicolumn scrolling in a separate segment
		*/
		//xxxLBoxCtlHScrollMultiColumn(plb, cmd, xAmt);
		return;
	}

	GetClientRect(plb->spwnd, &rc);
	windowWidth = rc.right;

	if (plb->cMac)
	{

		switch (cmd)
		{
		case SB_LINEUP:
			dwTime = xAmt;
			newOrigin -= plb->cxChar;
			break;

		case SB_LINEDOWN:
			dwTime = xAmt;
			newOrigin += plb->cxChar;
			break;

		case SB_PAGEUP:
			newOrigin -= (windowWidth / 3) * 2;
			break;

		case SB_PAGEDOWN:
			newOrigin += (windowWidth / 3) * 2;
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			newOrigin = xAmt;
			break;

		case SB_TOP:
			newOrigin = 0;
			break;

		case SB_BOTTOM:
			newOrigin = plb->maxWidth;
			break;

		case SB_ENDSCROLL:
			plb->fSmoothScroll = TRUE;
			xxxLBSetCaret(plb, FALSE);
			xxxLBShowHideScrollBars(plb);
			xxxLBSetCaret(plb, TRUE);
			return;
		}

		xxxLBSetCaret(plb, FALSE);
		plb->xOrigin = newOrigin;
		plb->xOrigin = xxxSetLBScrollParms(plb, SB_HORZ);

		if ((cmd == SB_BOTTOM) && plb->fRightAlign)
		{
			/*
			* so we know where to draw from.
			*/
			plb->xRightOrigin = plb->xOrigin;
		}

		if (oldOrigin != plb->xOrigin)
		{
			HWND hwnd = HWq(plb->spwnd);
			DWORD dwFlags;

			dwFlags = LBGetScrollFlags(plb, dwTime);
			ScrollWindowEx(hwnd, oldOrigin - plb->xOrigin,
				0, NULL, &rc, NULL, NULL, dwFlags);
			UpdateWindow(hwnd);
		}

		xxxLBSetCaret(plb, TRUE);
	}
	else
	{
		// this is a less-than-ideal fix for ImageMind ScreenSaver (Win95
		// B#8252) but it works and it doesn't hurt anybody -- JEFFBOG 10/28/94
		xxxSetLBScrollParms(plb, SB_HORZ);
	}
}

int LayoutScrollBar(PLBIV plb)
{
	CIUIRect rcClient;
	GetClientRect(plb->spwnd, rcClient);

	DWORD dwStyle = GetWindowLong(plb->spwnd, GWL_STYLE);
	BOOL bShowHScrollBar = IsIncludeFlag(dwStyle, WS_HSCROLL);
	BOOL bShowVScrollBar = IsIncludeFlag(dwStyle, WS_VSCROLL);

	DWORD dwExStyle = GetWindowLong(plb->spwnd, GWL_EXSTYLE);
	BOOL bLeftVScrollBar = IsIncludeFlag(dwExStyle, WS_EX_LEFTSCROLLBAR);

	// 如果水平或垂直滚动条为NULL，则创建
	if (bShowHScrollBar && plb->m_hHorizontalScrollBar == NULL)
	{
		HWLWND hHorzScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_HORZ,
			0, 0, 0, 0, plb->spwnd, NULL, XNAME_HORZSCROLLBAR, NULL, NULL);
		if (hHorzScrollBar == NULL)
		{
			return -1;
		}

		plb->m_hHorizontalScrollBar = hHorzScrollBar;
		// Bind Style
		CScrollBarProp *pBindStyle = (CScrollBarProp *)SendMessage(plb->spwnd,
			WLM_GET_BIND_SCROLLBAR_STYLE, TRUE, 0);
		ScrollBar_BindStyle(plb->m_hHorizontalScrollBar, pBindStyle);
	}

	if (bShowVScrollBar && plb->m_hVerticalScrollBar == NULL)
	{
		HWLWND hVertScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_VERT,
			0, 0, 0, 0, plb->spwnd, NULL, XNAME_VERTSCROLLBAR, NULL, NULL);
		if (hVertScrollBar == NULL)
		{
			return -2;
		}

		plb->m_hVerticalScrollBar = hVertScrollBar;
		// Bind Style
		CScrollBarProp *pBindStyle = (CScrollBarProp *)SendMessage(plb->spwnd,
			WLM_GET_BIND_SCROLLBAR_STYLE, FALSE, 0);
		ScrollBar_BindStyle(plb->m_hVerticalScrollBar, pBindStyle);
	}

	if (bShowHScrollBar)
	{
		CIUIRect rcHScrollBar(rcClient);
		rcHScrollBar.top = rcHScrollBar.bottom - plb->m_nHScrollBarHeight;

		if (bShowVScrollBar)
		{
			if (bLeftVScrollBar)
			{
				rcHScrollBar.left += plb->m_nVScrollBarWidth;
			}
			else
			{
				rcHScrollBar.right -= plb->m_nVScrollBarWidth;
			}
		}

		IUI::MoveWindow(plb->m_hHorizontalScrollBar, rcHScrollBar, TRUE);

		if (!IsWindowVisible(plb->m_hHorizontalScrollBar))
		{
			ShowWindow(plb->m_hHorizontalScrollBar, SW_SHOW);
		}
	}
	else
	{
		ShowWindow(plb->m_hHorizontalScrollBar, SW_HIDE);
	}

	if (bShowVScrollBar)
	{
		CIUIRect rcVScrollBar(rcClient);

		if (bLeftVScrollBar)
		{
			rcVScrollBar.right = rcVScrollBar.left + plb->m_nVScrollBarWidth;
		}
		else
		{
			rcVScrollBar.left = rcVScrollBar.right - plb->m_nVScrollBarWidth;
		}

		if (bShowHScrollBar)
		{
			rcVScrollBar.bottom -= plb->m_nHScrollBarHeight;
		}

		CIUIRect rcOld;
		GetWindowRectToParent(plb->m_hVerticalScrollBar, rcOld);
		if (rcOld != rcVScrollBar)
		{
			MoveWindow(plb->m_hVerticalScrollBar, rcVScrollBar, TRUE);
		}

		// 在ULW模式，ShowWindow会引起UpdateLayeredWindow调用
		// 判断一下，减小调用次数
		if (!IsWindowVisible(plb->m_hVerticalScrollBar))
		{
			ShowWindow(plb->m_hVerticalScrollBar, SW_SHOW);
		}
	}
	else
	{
		ShowWindow(plb->m_hVerticalScrollBar, SW_HIDE);
	}

	return 0;
}

/***************************************************************************\
* xxxLBoxCtlPaint
*
* History:
\***************************************************************************/

void xxxLBPaint(
	PLBIV plb,
	HDC hdc,
	LPRECT lprcBounds)
{
	INT i;
	RECT rect;
	RECT    scratchRect;
	BOOL fCaretOn;
	RECT    rcBounds;

	CheckLock(plb->spwnd);

	if (lprcBounds == NULL)
	{
		lprcBounds = &rcBounds;
		GetClientRect(plb->spwnd, lprcBounds);
	}

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hdc,
		RECTWIDTH(*lprcBounds), RECTHEIGHT(*lprcBounds));
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);
	FillSolidRect(hdcMem, 0, 0, RECTWIDTH(*lprcBounds), RECTHEIGHT(*lprcBounds),
		RGB(255, 255, 255), FALSE);

	HDC hdcSave = plb->hdc;
	plb->hdc = hdcMem;

	// Initialize dc.
	LBInitDC(plb);

	// Turn caret off
	if (fCaretOn = plb->fCaretOn)
	{
		xxxLBSetCaret(plb, FALSE);
	}

	HBRUSH hbrSave = NULL;
	HBRUSH hbrControl = xxxLBGetBrush(plb, &hbrSave);

	// Get listbox's client
	GetClientRect(plb->spwnd, &rect);

	// Adjust width of client rect for scrolled amount
	// fix for #140, t-arthb
	if (plb->fRightAlign && !(plb->fMultiColumn || plb->OwnerDraw) && plb->fHorzBar)
	{
		rect.right += plb->xOrigin + (plb->xRightOrigin - plb->xOrigin);
	}
	else
	{
		rect.right += plb->xOrigin;
	}

	// Get the index of the last item visible on the screen. This is also
	// valid for var height ownerdraw.
	int iLastItem = plb->iTop + CItemInWindow(plb, TRUE);
	iLastItem = min(iLastItem, plb->cMac - 1);

	// Fill in the background of the listbox if it's an empty listbox
	// or if we're doing a control print
	if (iLastItem == -1)
	{
		FillRect(plb->hdc, &rect, hbrControl);
	}


	// Allow AnimateWindow() catch the apps that do not use our DC when
	// drawing the list box
	//SetBoundsRect(plb->hdc, NULL, DCB_RESET | DCB_ENABLE);

	for (i = plb->iTop; i <= iLastItem; i++)
	{

		/*
		* Note that rect contains the clientrect from when we did the
		* GetClientRect so the width is correct.  We just need to adjust
		* the top and bottom of the rectangle to the item of interest.
		*/
		rect.bottom = rect.top + plb->cyChar;

		if ((UINT)i < (UINT)plb->cMac)
		{

			/*
			* If var height, get the rectangle for the item.
			*/
			if (plb->OwnerDraw == OWNERDRAWVAR || plb->fMultiColumn)
			{
				LBGetItemRect(plb, i, &rect);
			}

			if (IntersectRect(&scratchRect, lprcBounds, &rect))
			{
				// 是否高亮显示
				BOOL fHilite = !plb->fNoSel && IsSelected(plb, i, HILITEONLY);

				if (plb->OwnerDraw)
				{

					/*
					* Fill in the drawitem struct
					*/
					xxxLBoxDrawItem(plb, i, ODA_DRAWENTIRE,
						(UINT)(fHilite ? ODS_SELECTED : 0), &rect);
				}
				else
				{
					xxxLBDrawLBItem(plb, i, &rect, fHilite, hbrControl);
				}
			}
		}
		rect.top = rect.bottom;
	}

	if (hbrSave != NULL)
	{
		SelectObject(plb->hdc, hbrSave);
	}

	if (fCaretOn)
	{
		xxxLBSetCaret(plb, TRUE);
	}

	// 隐藏、显示并绘制滚动条。 由于以后可能制作带alpha的ComboBox下拉列表，所以把
	// 滚动条创建成客户区，以方便控制。
	LayoutScrollBar(plb);
	DrawControlAndChildren(plb->m_hHorizontalScrollBar, plb->hdc, lprcBounds);
	DrawControlAndChildren(plb->m_hVerticalScrollBar, plb->hdc, lprcBounds);

	BitBlt(hdc, 0, 0, RECTWIDTH(*lprcBounds), RECTHEIGHT(*lprcBounds), hdcMem, 0, 0, SRCCOPY);
	LBTermDC(plb);

	plb->hdc = hdcSave;

	SelectObject(hdcMem, hbmpOld);
	DeleteObject(hbmpMem);
	hbmpMem = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;
}


/***************************************************************************\
* ISelFromPt
*
* In the loword, returns the closest item number the pt is on. The high
* word is 0 if the point is within bounds of the listbox client rect and is
* 1 if it is outside the bounds.  This will allow us to make the invertrect
* disappear if the mouse is outside the listbox yet we can still show the
* outline around the item that would be selected if the mouse is brought back
* in bounds...
*
* History:
\***************************************************************************/
// 如果鼠标位于Item内，则返回FALSE，否则返回TRUE。
// piItem: 返回鼠标最接近的Item索引
BOOL ISelFromPt(
	PLBIV plb,
	POINT pt,
	LPDWORD piItem)
{
	RECT rect;
	int y;
	UINT mouseHighWord = 0;
	INT sItem;
	INT sTmp;

	GetClientRect(plb->spwnd, &rect);

	if (pt.y < 0)
	{

		/*
		* Mouse is out of bounds above listbox
		*/
		*piItem = plb->iTop;
		return TRUE;
	}
	else if ((y = pt.y) > rect.bottom)
	{
		y = rect.bottom;
		mouseHighWord = 1;
	}

	if (pt.x < 0 || pt.x > rect.right)
	{
		mouseHighWord = 1;
	}

	/*
	* Now just need to check if y mouse coordinate intersects item's rectangle
	*/
	if (plb->OwnerDraw != OWNERDRAWVAR)
	{
		if (plb->fMultiColumn)
		{
			if (y < plb->itemsPerColumn * plb->cyChar)
			{
				if (plb->fRightAlign)
					sItem = plb->iTop + (INT)((y / plb->cyChar) +
					((rect.right - pt.x) / plb->cxColumn) * plb->itemsPerColumn);
				else
					sItem = plb->iTop + (INT)((y / plb->cyChar) +
					(pt.x / plb->cxColumn) * plb->itemsPerColumn);

			}
			else
			{

				/*
				* User clicked in blank space at the bottom of a column.
				* Just select the last item in the column.
				*/
				mouseHighWord = 1;
				sItem = plb->iTop + (plb->itemsPerColumn - 1) +
					(INT)((pt.x / plb->cxColumn) * plb->itemsPerColumn);
			}
		}
		else
		{
			sItem = plb->iTop + (INT)(y / plb->cyChar);
		}
	}
	else
	{

		/*
		* VarHeightOwnerdraw so we gotta do this the hardway...   Set the x
		* coordinate of the mouse down point to be inside the listbox client
		* rectangle since we no longer care about it.  This lets us use the
		* point in rect calls.
		*/
		pt.x = 8;
		pt.y = y;
		for (sTmp = plb->iTop; sTmp < plb->cMac; sTmp++)
		{
			(void)LBGetItemRect(plb, sTmp, &rect);
			if (PtInRect(&rect, pt))
			{
				*piItem = sTmp;
				return mouseHighWord;
			}
		}

		/*
		* Point was at the empty area at the bottom of a not full listbox
		*/
		*piItem = plb->cMac - 1;
		return mouseHighWord;
	}

	/*
	* Check if user clicked on the blank area at the bottom of a not full list.
	* Assumes > 0 items in the listbox.
	*/
	if (sItem > plb->cMac - 1)
	{
		mouseHighWord = 1;
		sItem = plb->cMac - 1;
	}

	*piItem = sItem;
	return mouseHighWord;
}


/***************************************************************************\
* SetSelected
*
* This is used for button initiated changes of selection state.
*
*  fSelected : TRUE  if the item is to be set as selected, FALSE otherwise
*
*  wOpFlags : HILITEONLY = Modify only the Display state (hi-nibble)
*             SELONLY    = Modify only the Selection state (lo-nibble)
*             HILITEANDSEL = Modify both of them;
*
* History:
* 16-Apr-1992 beng      The NODATA listbox case
\***************************************************************************/

void SetSelected(
	PLBIV plb,
	INT iSel,
	BOOL fSelected,
	UINT wOpFlags)
{
	LPSTR lp;
	BYTE cMask;
	BYTE cSelStatus;

	if (iSel < 0 || iSel >= plb->cMac)
	{
		return;
	}

	if (plb->wMultiple == SINGLESEL)
	{
		if (fSelected)
		{
			plb->iSel = iSel;
		}
	}
	else
	{
		cSelStatus = (BYTE)fSelected;
		switch (wOpFlags)
		{
		case HILITEONLY:

			/*
			* Mask out lo-nibble
			*/
			cSelStatus = (BYTE)(cSelStatus << 4);
			cMask = 0x0F;
			break;
		case SELONLY:

			/*
			* Mask out hi-nibble
			*/
			cMask = 0xF0;
			break;
		case HILITEANDSEL:

			/*
			* Mask the byte fully
			*/
			cSelStatus |= (cSelStatus << 4);
			cMask = 0;
			break;
		}
		lp = (LPSTR)(plb->rgpch) + iSel +
			(plb->cMac * (plb->fHasStrings
				? sizeof(LBItem)
				: (plb->fHasData ? sizeof(LBODItem) : 0)));

		*lp = (*lp & cMask) | cSelStatus;
	}
}


/***************************************************************************\
* LastFullVisible
*
* Returns the last fully visible item in the listbox. This is valid
* for ownerdraw var height and fixed height listboxes.
*
* History:
\***************************************************************************/

INT LastFullVisible(
	PLBIV plb)
{
	INT iLastItem;

	if (plb->OwnerDraw == OWNERDRAWVAR || plb->fMultiColumn)
	{
		iLastItem = plb->iTop + CItemInWindow(plb, FALSE) - 1;
		iLastItem = max(iLastItem, plb->iTop);
	}
	else
	{
		iLastItem = min(plb->iTop + plb->cItemFullMax - 1, plb->cMac - 1);
	}
	return iLastItem;
}


/***************************************************************************\
* xxxInvertLBItem
*
* History:
\***************************************************************************/

void xxxInvertLBItem(
	PLBIV plb,
	INT i,
	BOOL fHilite)  /* The new selection state of the item */
{
	RECT rect;
	BOOL fCaretOn;

	CheckLock(plb->spwnd);

	// Skip if item isn't showing.
	if (plb->fNoSel || (i < plb->iTop) || (i >= (plb->iTop + CItemInWindow(plb, TRUE))))
	{
		return;
	}

	if (IsWindowVisible(plb->spwnd))
	{
		LBGetItemRect(plb, i, &rect);

		/*
		* Only turn off the caret if it is on.  This avoids annoying caret
		* flicker when nesting xxxCaretOns and xxxCaretOffs.
		*/
		if (fCaretOn = plb->fCaretOn)
		{
			xxxLBSetCaret(plb, FALSE);
		}

		/*
		* Turn the caret back on only if it was originally on.
		*/
		if (fCaretOn)
		{
			xxxLBSetCaret(plb, TRUE);
		}
	}
}


/***************************************************************************\
* xxxResetWorld
*
* Resets everyone's selection and hilite state except items in the
* range sStItem to sEndItem (Both inclusive).
*
* History:
\***************************************************************************/

void xxxResetWorld(
	PLBIV plb,
	INT iStart,
	INT iEnd,
	BOOL fSelect)
{
	INT i;
	INT iLastInWindow;
	BOOL fCaretOn;

	CheckLock(plb->spwnd);

	/*
	* If iStart and iEnd are not in correct order we swap them
	*/

	if (iStart > iEnd)
	{
		i = iStart;
		iStart = iEnd;
		iEnd = i;
	}

	if (plb->wMultiple == SINGLESEL)
	{
		if (plb->iSel != -1 && ((plb->iSel < iStart) || (plb->iSel > iEnd)))
		{
			xxxInvertLBItem(plb, plb->iSel, fSelect);
			plb->iSel = -1;
		}
		return;
	}

	iLastInWindow = plb->iTop + CItemInWindow(plb, TRUE);

	if (fCaretOn = plb->fCaretOn)
	{
		xxxLBSetCaret(plb, FALSE);
	}

	for (i = 0; i < plb->cMac; i++)
	{
		if (i == iStart)
			// skip range to be preserved
		{
			i = iEnd;
		}
		else
		{
			if ((plb->iTop <= i) && (i <= iLastInWindow) &&
				(fSelect != IsSelected(plb, i, HILITEONLY)))
				// Only invert the item if it is visible and present Selection
				// state is different from what is required.
			{
				xxxInvertLBItem(plb, i, fSelect);
			}

			// Set all items outside of preserved range to unselected
			SetSelected(plb, i, fSelect, HILITEANDSEL);
		}
	}

	if (fCaretOn)
	{
		xxxLBSetCaret(plb, TRUE);
	}

}


/***************************************************************************\
* xxxNotifyOwner
*
* History:
\***************************************************************************/

void xxxNotifyOwner(
	PLBIV plb,
	INT sEvt)
{
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	ThreadLock(plb->spwndParent, &tlpwndParent);
	SendMessage(plb->pcbox->hwnd, WM_COMMAND,
		MAKELONG(/*PTR_TO_ID(plb->spwnd->spmenu)*/0, sEvt), (LPARAM)HWq(plb->spwnd));
	ThreadUnlock(&tlpwndParent);
}


/***************************************************************************\
* xxxSetISelBase
*
* History:
\***************************************************************************/

void xxxSetISelBase(
	PLBIV plb,
	INT sItem)
{
	CheckLock(plb->spwnd);

	xxxLBSetCaret(plb, FALSE);
	plb->iSelBase = sItem;
	xxxLBSetCaret(plb, TRUE);

	//if (FWINABLE())
	//{
	//	xxxInsureVisible(plb, plb->iSelBase, FALSE);
	//	if (_IsWindowVisible(plb->spwnd))
	//	{
	//		LBEvent(plb, EVENT_OBJECT_FOCUS, sItem);
	//	}
	//}
}


/***************************************************************************\
* xxxTrackMouse
*
* History:
\***************************************************************************/

void xxxTrackMouse(
	PLBIV plb,
	UINT wMsg,
	POINT pt)
{
	INT iSelFromPt;
	INT iSelTemp;
	BOOL fMouseInRect;
	RECT rcClient;
	UINT wModifiers = 0;
	BOOL fSelected;
	UINT uEvent = 0;
	INT trackPtRetn;
	HWND hwnd = HWq(plb->spwnd);
	TL tlpwndEdit;
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	/*
	* Optimization:  do nothing if mouse not captured
	*/
	if ((wMsg != WM_LBUTTONDOWN) && (wMsg != WM_LBUTTONDBLCLK))
	{
		if (!plb->fCaptured)
		{
			return;
		}
		/*
		* If we are processing a WM_MOUSEMOVE but the mouse has not moved from
		* the previous point, then we may be dealing with a mouse "jiggle" sent
		* from the kernel (see zzzInvalidateDCCache).  If we process this, we will
		* snap the listbox selection back to where the mouse cursor is pointing,
		* even if the user has not touched the mouse.  FritzS: NT5 bug 220722.
		*  Some apps (like MSMoney98) rely on this, so added the bLastRITWasKeyboard
		* check.  MCostea #244450
		*/
		if ((wMsg == WM_MOUSEMOVE) && RtlEqualMemory(&pt, &(plb->ptPrev), sizeof(POINT))
			/*&& gpsi->bLastRITWasKeyboard*/)
		{
			//RIPMSG0(RIP_WARNING, "xxxTrackMouse ignoring WM_MOUSEMOVE with no mouse movement");
			return;
		}
	}

	// ISelFromPt: 如果鼠标位于Item内，则返回FALSE，否则返回TRUE。
	BOOL mousetemp = ISelFromPt(plb, pt, (LPDWORD)&iSelFromPt);

	/*
	* If we allow the user to cancel his selection then fMouseInRect is true if
	* the mouse is in the listbox client area otherwise it is false.  If we
	* don't allow the user to cancel his selection, then fMouseInRect will
	* always be true.  This allows us to implement cancelable selection
	* listboxes ie.  The selection reverts to the origional one if the user
	* releases the mouse outside of the listbox.
	*/
	fMouseInRect = !mousetemp || !plb->pcbox;

	GetClientRect(plb->spwnd, &rcClient);

	//
	// 是否命中了自定义滚动条
	//
	HWLWND hScrollBar = NULL;
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (IsIncludeFlag(dwStyle, WS_VSCROLL))
	{
		CIUIRect rcVScrollBar;
		GetWindowRectToParent(plb->m_hVerticalScrollBar, rcVScrollBar);

		if (rcVScrollBar.PtInRect(pt))
		{
			hScrollBar = plb->m_hVerticalScrollBar;
		}
	}

	if (IsIncludeFlag(dwStyle, WS_HSCROLL))
	{
		CIUIRect rcHScrollBar;
		GetWindowRectToParent(plb->m_hHorizontalScrollBar, rcHScrollBar);

		if (rcHScrollBar.PtInRect(pt))
		{
			hScrollBar = plb->m_hHorizontalScrollBar;
		}
	}

	// 如果命中了自定义滚动条，则把消息都转发给滚动条。
	if (hScrollBar != NULL)
	{
		POINT ptDest = pt;
		ClientToScreen(hwnd, &ptDest);
		ScreenToClient(hScrollBar, &ptDest);

		RoutedEventArgs arg;
		arg.hOriginalSender = hScrollBar;
		arg.RoutedEvent = wMsg;
		arg.eRoutingStrategy = RS_TUNNEL;
		arg.wParam = 0;
		arg.lParam = MAKELPARAM(ptDest.x, ptDest.y);

		RaiseEvent(hScrollBar, &arg);

		return;
	}

	switch (wMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	{
		// 转义鼠标点击消息。 如果用户点击了下拉列表框外面，则隐藏下拉列表框
		if (plb->fCaptured)
		{
			/*
			* If plb->pcbox is NULL, this is a listbox that
			* received a WM_LBUTTONDOWN again w/o receiving
			* a WM_LBUTTONUP for the previous WM_LBUTTONDOWN
			* bug
			*/
			if (plb->pcbox && mousetemp)
			{
				ClientToScreen(plb->spwnd, &pt);

				CIUIRect rcWindow;
				GetWindowRect(plb->spwnd, rcWindow);
				CIUIRect rcClient;
				GetClientRect(plb->spwnd, rcClient);
				if (!PtInRect(&rcWindow, pt))
				{
					// 点击到下拉列表框外面，则隐藏列表框
					xxxCBHideListBoxWindow(plb->pcbox, TRUE, FALSE);
				}
				else if (!PtInRect(&rcClient, pt))
				{
					// 如果点击到下拉列表框的滚动条上，则需要保存、恢复capture.
					// 并且把客户区鼠标消息转义成非客户区消息。
					plb->fCaptured = FALSE;
					ReleaseCapture();

					// wParam参数很重要。如果设置不正确，则可能无法拖动滚动条。
					UINT uHitTest = SendMessage(plb->spwnd, WM_NCHITTEST,
						0, MAKELPARAM(pt.x, pt.y));
					SendMessage(plb->spwnd, WM_NCLBUTTONDOWN, uHitTest, MAKELONG(pt.x, pt.y));

					SetCapture(hwnd);
					plb->fCaptured = TRUE;
				}

				break;
			}

			plb->fCaptured = FALSE;
			ReleaseCapture();
		}

		if (plb->pcbox)
		{

			/*
			* If this listbox is in a combo box, set the focus to the combo
			* box window so that the edit control/static text is also
			* activated
			*/
			ThreadLock(plb->pcbox->spwndEdit, &tlpwndEdit);
			SetFocus(GetHostWnd(plb->pcbox->spwndEdit), plb->pcbox->spwndEdit);
			ThreadUnlock(&tlpwndEdit);
		}
		else
		{

			/*
			* Get the focus if the listbox is clicked in and we don't
			* already have the focus.  If we don't have the focus after
			* this, run away...
			*/
			SetFocus(hwnd);
			if (!plb->fCaret)
			{
				return;
			}
		}

		if (plb->fAddSelMode)
		{

			/*
			* If it is in "Add" mode, quit it using shift f8 key...
			* However, since we can't send shift key state, we have to turn
			* this off directly...
			*/

			/*
			*SendMessage(HW(plb->spwnd),WM_KEYDOWN, (UINT)VK_F8, 0L);
			*/

			/*
			* Switch off the Caret blinking
			*/
			KillTimer(hwnd, IDSYS_CARET);

			/*
			* Make sure the caret does not vanish
			*/
			xxxLBSetCaret(plb, TRUE);
			plb->fAddSelMode = FALSE;
		}

		if (!plb->cMac)
		{

			/*
			* Don't even bother handling the mouse if no items in the
			* listbox since the code below assumes >0 items in the
			* listbox.  We will just get the focus (the statement above) if
			* we don't already have it.
			*/
			break;
		}

		if (mousetemp)
		{

			/*
			* Mouse down occurred in a empty spot.  Just ignore it.
			*/
			break;
		}

		plb->fDoubleClick = (wMsg == WM_LBUTTONDBLCLK);

		if (!plb->fDoubleClick)
		{

			/*
			* This hack put in for the shell.  Tell the shell where in the
			* listbox the user clicked and at what item number.  The shell
			* can return 0 to continue normal mouse tracking or TRUE to
			* abort mouse tracking.
			*/
			// 向ListBox父窗口发送WM_LBTRACKPOINT消息，父窗口在处理WM_LBTRACKPOINT消息时，
			// 可以返回非0值，使用ListBox结束mouse track
			ThreadLock(plb->spwndParent, &tlpwndParent);
			trackPtRetn = (INT)SendMessage(HW(plb->spwndParent), WM_LBTRACKPOINT,
				(DWORD)iSelFromPt, MAKELONG(pt.x + plb->xOrigin, pt.y));
			ThreadUnlock(&tlpwndParent);
			if (trackPtRetn)
			{
				if (trackPtRetn == 2)
				{

					/*
					* Ignore double clicks
					*/
					//L NtUserCallNoParam(SFI__RESETDBLCLK);
				}
				return;
			}
		}

		if (plb->pcbox)
		{

			/*
			* Save the last selection if this is a combo box.  So that it
			* can be restored if user decides to cancel the selection by up
			* clicking outside the listbox.
			*/
			plb->iLastSelection = plb->iSel;
		}

		/*
		* Save for timer
		*/
		plb->ptPrev = pt;

		plb->fMouseDown = TRUE;
		SetCapture(hwnd);
		plb->fCaptured = TRUE;

		if (plb->fDoubleClick)
		{

			/*
			* Double click.  Fake a button up and exit
			*/
			xxxTrackMouse(plb, WM_LBUTTONUP, pt);
			return;
		}

		// 启定时器，使得鼠标移出ListBox外面后，ListBox可自动滚动
		SetTimer(hwnd, IDSYS_SCROLL, 400, NULL);

		/*
		* If extended multiselection listbox, are any modifier key pressed?
		*/
		if (plb->wMultiple == EXTENDEDSEL)
		{
			if (GetKeyState(VK_SHIFT) < 0)
			{
				wModifiers = SHIFTDOWN;
			}
			if (GetKeyState(VK_CONTROL) < 0)
			{
				wModifiers += CTLDOWN;
			}

			/*
			* Please Note that (SHIFTDOWN + CTLDOWN) == (SHCTLDOWN)
			*/
		}


		switch (wModifiers)
		{
		case NOMODIFIER:
		MouseMoveHandler:
			if (plb->iSelBase != iSelFromPt)
			{
				xxxLBSetCaret(plb, FALSE);
			}

			/*
			* We only look at the mouse if the point it is pointing to is
			* not selected.  Since we are not in ExtendedSelMode, anywhere
			* the mouse points, we have to set the selection to that item.
			* Hence, if the item isn't selected, it means the mouse never
			* pointed to it before so we can select it.  We ignore already
			* selected items so that we avoid flashing the inverted
			* selection rectangle.  Also, we could get WM_SYSTIMER simulated
			* mouse moves which would cause flashing otherwise...
			*/

			iSelTemp = (fMouseInRect ? iSelFromPt : -1);

			/*
			* If the LB is either SingleSel or Extended multisel, clear all
			* old selections except the new one being made.
			*/
			if (plb->wMultiple != MULTIPLESEL)
			{
				xxxResetWorld(plb, iSelTemp, iSelTemp, FALSE);
				/*
				* This will be TRUE if iSelTemp isn't -1 (like below)
				* and also if it is but there is a current selection.
				*/
				if ((iSelTemp == -1) && (plb->iSel != -1))
				{
					uEvent = EVENT_OBJECT_SELECTIONREMOVE;
				}
			}

			fSelected = IsSelected(plb, iSelTemp, HILITEONLY);
			if (iSelTemp != -1)
			{

				/*
				* If it is MULTIPLESEL, then toggle; For others, only if
				* not selected already, select it.
				*/
				if (((plb->wMultiple == MULTIPLESEL) && (wMsg != WM_LBUTTONDBLCLK)) || !fSelected)
				{
					SetSelected(plb, iSelTemp, !fSelected, HILITEANDSEL);

					xxxInvertLBItem(plb, iSelTemp, !fSelected);
					fSelected = !fSelected;  /* Set the new state */
					if (plb->wMultiple == MULTIPLESEL)
					{
						uEvent = (fSelected ? EVENT_OBJECT_SELECTIONADD :
							EVENT_OBJECT_SELECTIONREMOVE);
					}
					else
					{
						uEvent = EVENT_OBJECT_SELECTION;
					}
				}
			}

			// 设置选中项
			plb->iSel = iSelTemp;
			InvalidateRect(plb->spwnd, NULL, FALSE);

			/*
			* Set the new anchor point
			*/
			plb->iMouseDown = iSelFromPt;
			plb->iLastMouseMove = iSelFromPt;
			plb->fNewItemState = fSelected;

			break;
		case SHIFTDOWN:

			/*
			* This is so that we can handle click and drag for multisel
			* listboxes using Shift modifier key .
			*/
			plb->iLastMouseMove = plb->iSel = iSelFromPt;



			/*
			* Check if an anchor point already exists
			*/
			if (plb->iMouseDown == -1)
			{
				plb->iMouseDown = iSelFromPt;

				/*
				* Reset all the previous selections
				*/
				xxxResetWorld(plb, plb->iMouseDown, plb->iMouseDown, FALSE);

				/*
				* Select the current position
				*/
				SetSelected(plb, plb->iMouseDown, TRUE, HILITEANDSEL);
				xxxInvertLBItem(plb, plb->iMouseDown, TRUE);
				/*
				* We are changing the selction to this item only
				*/
				uEvent = EVENT_OBJECT_SELECTION;
			}
			else
			{

				/*
				* Reset all the previous selections
				*/
				xxxResetWorld(plb, plb->iMouseDown, plb->iMouseDown, FALSE);

				/*
				* Select all items from anchor point upto current click pt
				*/
				xxxAlterHilite(plb, plb->iMouseDown, iSelFromPt, HILITE, HILITEONLY, FALSE);
				uEvent = EVENT_OBJECT_SELECTIONWITHIN;
			}
			plb->fNewItemState = (UINT)TRUE;
			break;

		case CTLDOWN:

			/*
			* This is so that we can handle click and drag for multisel
			* listboxes using Control modifier key.
			*/

			/*
			* Reset the anchor point to the current point
			*/
			plb->iMouseDown = plb->iLastMouseMove = plb->iSel = iSelFromPt;

			/*
			* The state we will be setting items to
			*/
			plb->fNewItemState = (UINT)!IsSelected(plb, iSelFromPt, (UINT)HILITEONLY);

			/*
			* Toggle the current point
			*/
			SetSelected(plb, iSelFromPt, plb->fNewItemState, HILITEANDSEL);
			xxxInvertLBItem(plb, iSelFromPt, plb->fNewItemState);
			uEvent = (plb->fNewItemState ? EVENT_OBJECT_SELECTIONADD :
				EVENT_OBJECT_SELECTIONREMOVE);
			break;

		case SHCTLDOWN:

			/*
			* This is so that we can handle click and drag for multisel
			* listboxes using Shift and Control modifier keys.
			*/

			/*
			* Preserve all the previous selections
			*/

			/*
			* Deselect only the selection connected with the last
			* anchor point; If the last anchor point is associated with a
			* de-selection, then do not do it
			*/
			if (plb->fNewItemState)
			{
				xxxAlterHilite(plb, plb->iMouseDown, plb->iLastMouseMove, FALSE, HILITEANDSEL, FALSE);
			}
			plb->iLastMouseMove = plb->iSel = iSelFromPt;

			/*
			* Check if an anchor point already exists
			*/
			if (plb->iMouseDown == -1)
			{

				/*
				* No existing anchor point; Make the current pt as anchor
				*/
				plb->iMouseDown = iSelFromPt;
			}

			/*
			* If one exists preserve the most recent anchor point
			*/

			/*
			* The state we will be setting items to
			*/
			plb->fNewItemState = (UINT)IsSelected(plb, plb->iMouseDown, HILITEONLY);

			/*
			* Select all items from anchor point upto current click pt
			*/
			xxxAlterHilite(plb, plb->iMouseDown, iSelFromPt, plb->fNewItemState, HILITEONLY, FALSE);
			uEvent = EVENT_OBJECT_SELECTIONWITHIN;
			break;
		}

		/*
		* Set the new base point (the outline frame caret).  We do the check
		* first to avoid flashing the caret unnecessarly.
		*/
		if (plb->iSelBase != iSelFromPt)
		{

			/*
			* Since xxxSetISelBase always turns on the caret, we don't need to
			* do it here...
			*/
			xxxSetISelBase(plb, iSelFromPt);
		}

		/*
		* SetISelBase will change the focus and send a focus event.
		* Then we send the selection event.
		*/
		//if (FWINABLE() && uEvent)
		//{
		//	LBEvent(plb, uEvent, iSelFromPt);
		//}
		//if (wMsg == WM_LBUTTONDOWN/* && TestWF(plb->spwnd, WEFDRAGOBJECT)*/)
		//{
		//	if (DragDetect(hwnd, pt))
		//	{

		//		/*
		//		 * User is trying to drag object...
		//		 */

		//		/*
		//		 *  Fake an up click so that the item is selected...
		//		 */
		//		xxxTrackMouse(plb, WM_LBUTTONUP, pt);

		//		/*
		//		 * Notify parent
		//		 * #ifndef WIN16 (32-bit Windows), plb->iSelBase gets
		//		 * zero-extended to LONG wParam automatically by the compiler.
		//		 */
		//		ThreadLock(plb->spwndParent, &tlpwndParent);
		//		SendMessage(HW(plb->spwndParent), WM_BEGINDRAG, plb->iSelBase,
		//			(LPARAM)hwnd);
		//		ThreadUnlock(&tlpwndParent);
		//	}
		//	else
		//	{
		//		xxxTrackMouse(plb, WM_LBUTTONUP, pt);
		//	}
		//	return;
		//}
	}
	break;

	case WM_MOUSEMOVE:
	{
		int dist;
		int iTimer;

		/*
		* Save for timer.
		*/
		plb->ptPrev = pt;
		/*
		* Autoscroll listbox if mouse button is held down and mouse is
		* moved outside of the listbox
		*/
		if (plb->fMouseDown)
		{
			//if (plb->fMultiColumn)
			//{
			//	if ((pt.x < 0) || (pt.x >= rcClient.right - 1))
			//	{
			//		/*
			//		 * Reset timer interval based on distance from listbox.
			//		 * use a longer default interval because each multicolumn
			//		 * scrolling increment is larger
			//		 */
			//		dist = pt.x < 0 ? -pt.x : (pt.x - rcClient.right + 1);
			//		iTimer = ((400 * 3) / 2) - ((WORD) dist << 4);

			//		if (plb->fRightAlign)
			//		{
			//			xxxLBoxCtlHScrollMultiColumn(plb, (pt.x < 0 ? SB_LINEDOWN : SB_LINEUP), 0);
			//		}
			//		else
			//		{
			//			xxxLBoxCtlHScrollMultiColumn(plb, (pt.x < 0 ? SB_LINEUP : SB_LINEDOWN), 0);
			//		}

			//		goto SetTimerAndSel;
			//	}
			//}
			/*else */if ((pt.y < 0) || (pt.y >= rcClient.bottom - 1))
			{
				/*
				* Reset timer interval based on distance from listbox.
				*/
				dist = pt.y < 0 ? -pt.y : (pt.y - rcClient.bottom + 1);
				iTimer = 400 - ((WORD)dist << 4);

				xxxLBoxCtlScroll(plb, (pt.y < 0 ? SB_LINEUP : SB_LINEDOWN), 0);
				//SetTimerAndSel:
				SetTimer(hwnd, IDSYS_SCROLL, max(iTimer, 1), NULL);
				ISelFromPt(plb, pt, (LPDWORD)&iSelFromPt);
			}
		}
		else
		{
			/*
			* Ignore if not in client since we don't autoscroll
			*/
			if (!PtInRect(&rcClient, pt))
			{
				break;
			}
		}

		switch (plb->wMultiple)
		{
		case SINGLESEL:
			/*
			* If it is a single selection or plain multisel list box
			*/
			goto MouseMoveHandler;
			break;

		case MULTIPLESEL:
		case EXTENDEDSEL:

			/*
			* Handle mouse movement with extended selection of items
			*/
			if (plb->iSelBase != iSelFromPt)
			{
				xxxSetISelBase(plb, iSelFromPt);

				/*
				* If this is an extended Multi sel list box, then
				* adjust the display of the range due to the mouse move
				*/
				if (plb->wMultiple == EXTENDEDSEL)
				{
					xxxLBBlockHilite(plb, iSelFromPt, FALSE);
					//if (FWINABLE())
					//{
					//	LBEvent(plb, EVENT_OBJECT_SELECTIONWITHIN, iSelFromPt);
					//}
				}
				plb->iLastMouseMove = iSelFromPt;
			}
			break;
		}
		break;
	}
	case WM_LBUTTONUP:
		if (plb->fMouseDown)
		{
			xxxLBButtonUp(plb, LBUP_RELEASECAPTURE | LBUP_NOTIFY |
				(mousetemp ? LBUP_RESETSELECTION : 0) |
				(fMouseInRect ? LBUP_SUCCESS : 0));
		}
	}
}

/***************************************************************************\
*
*  LBButtonUp()
*
*  Called in response to both WM_CAPTURECHANGED and WM_LBUTTONUP.
*
\***************************************************************************/
void xxxLBButtonUp(PLBIV plb, UINT uFlags)
{

	CheckLock(plb->spwnd);

	/*
	* If the list box is an Extended listbox, then change the select status
	* of all items between the anchor and the last mouse position to the
	* newItemState
	*/
	if (plb->wMultiple == EXTENDEDSEL)
	{
		xxxAlterHilite(plb, plb->iMouseDown, plb->iLastMouseMove,
			plb->fNewItemState, SELONLY, FALSE);
	}

	/*
	* This is a combo box and user upclicked outside the listbox
	* so we want to restore the original selection.
	*/
	if (plb->pcbox && (uFlags & LBUP_RESETSELECTION))
	{
		int iSelOld;

		iSelOld = plb->iSel;

		if (iSelOld >= 0)
		{
			xxxInvertLBItem(plb, plb->iSel, FALSE);
		}

		plb->iSel = plb->iLastSelection;
		xxxInvertLBItem(plb, plb->iSel, TRUE);

		/*
		* Note that we always send selection events before we tell the
		* app.  This is on purpose--the app may turn around and select
		* something else when notified.  In which case our event would
		* be out of order.
		*/
		//if (FWINABLE())
		//{
		//	LBEvent(plb, EVENT_OBJECT_SELECTION, plb->iSel);
		//}

		/*
		* On win-95 and NT4 the check used to be !(uFlags & LBUP_NOTIFY) which
		* is a bug because we would notify even when the lb is not LBUP_NOTIFY
		*/
		if ((uFlags & LBUP_NOTIFY) && plb->fNotify && (iSelOld != plb->iSel))
		{
			xxxNotifyOwner(plb, LBN_SELCHANGE);
		}
	}

	KillTimer(HWq(plb->spwnd), IDSYS_SCROLL);
	plb->fMouseDown = FALSE;
	if (plb->fCaptured)
	{
		plb->fCaptured = FALSE;
		if (uFlags & LBUP_RELEASECAPTURE)
		{
			ReleaseCapture();
		}
	}
	/*
	* Don't scroll item as long as any part of it is visible
	*/
	if (plb->iSelBase < plb->iTop ||
		plb->iSelBase > plb->iTop + CItemInWindow(plb, TRUE))
	{
		xxxInsureVisible(plb, plb->iSelBase, FALSE);
	}

	if (plb->fNotify)
	{
		if (uFlags & LBUP_NOTIFY)
		{
			if (uFlags & LBUP_SUCCESS)
			{
				/*
				* ArtMaster needs this SELCHANGE notification now!
				*/
				if ((plb->fDoubleClick)/* && !TestWF(plb->spwnd, WFWIN31COMPAT)*/)
				{
					xxxNotifyOwner(plb, LBN_SELCHANGE);
				}

				/*
				* Notify owner of click or double click on selection
				*/
				xxxNotifyOwner(plb, (plb->fDoubleClick) ? LBN_DBLCLK : LBN_SELCHANGE);
			}
			else
			{
				/*
				* Notify owner that the attempted selection was cancelled.
				*/
				xxxNotifyOwner(plb, LBN_SELCANCEL);
			}
		}
		else if (uFlags & LBUP_SELCHANGE)
		{
			/*
			* Did we do some semi-selecting with mouse moves, then hit Enter?
			* If so, we need to make sure the app knows that something was
			* really truly selected.
			*/
			//UserAssert(TestWF(plb->spwnd, WFWIN40COMPAT));
			if (plb->iLastSelection != plb->iSel)
			{
				xxxNotifyOwner(plb, LBN_SELCHANGE);
			}

		}
	}

}


/***************************************************************************\
* IncrementISel
*
* History:
\***************************************************************************/

INT IncrementISel(
	PLBIV plb,
	INT iSel,
	INT sInc)
{

	/*
	* Assumes cMac > 0, return iSel+sInc in range [0..cmac).
	*/
	iSel += sInc;
	if (iSel < 0)
	{
		return 0;
	}
	else if (iSel >= plb->cMac)
	{
		return plb->cMac - 1;
	}
	return iSel;
}


/***************************************************************************\
* NewITop
*
\***************************************************************************/

void xxxNewITop(PLBIV plb, INT iTopNew)
{
	xxxNewITopEx(plb, iTopNew, 0);
}


/***************************************************************************\
* xxxNewITopEx
*
* History:
\***************************************************************************/

void xxxNewITopEx(
	PLBIV plb,
	INT iTopNew,
	DWORD dwTime)
{
	int     iTopOld;
	BOOL fCaretOn;
	BOOL fMulti = plb->fMultiColumn;

	CheckLock(plb->spwnd);

	// Always try to turn off caret whether or not redraw is on
	if (fCaretOn = plb->fCaretOn)
	{
		xxxLBSetCaret(plb, FALSE);
	}

	iTopOld = (fMulti) ? (plb->iTop / plb->itemsPerColumn) : plb->iTop;
	plb->iTop = iTopNew;
	iTopNew = xxxSetLBScrollParms(plb, (fMulti) ? SB_HORZ : SB_VERT);
	plb->iTop = (fMulti) ? (iTopNew * plb->itemsPerColumn) : iTopNew;

	if (!IsWindowVisible(plb->spwnd))
	{
		return;
	}

	if (iTopNew != iTopOld)
	{
		int     xAmt, yAmt;
		RECT    rc;
		DWORD   dwFlags;

		GetClientRect(plb->spwnd, &rc);

		if (fMulti)
		{
			yAmt = 0;
			if (abs(iTopNew - iTopOld) > plb->numberOfColumns)
				// Handle scrolling a large number of columns properly so that
				// we don't overflow the size of a rect.
			{
				xAmt = 32000;
			}
			else
			{
				xAmt = (iTopOld - iTopNew) * plb->cxColumn;
				if (plb->fRightAlign)
				{
					xAmt = -xAmt;
				}
			}
		}
		else
		{
			xAmt = 0;
			if (plb->OwnerDraw == OWNERDRAWVAR)
			{
				//
				// Have to fake iTopOld for OWNERDRAWVAR listboxes so that
				// the scrolling amount calculations work properly.
				//
				plb->iTop = iTopOld;
				yAmt = LBCalcVarITopScrollAmt(plb, iTopOld, iTopNew);
				plb->iTop = iTopNew;
			}
			else if (abs(iTopNew - iTopOld) > plb->cItemFullMax)
			{
				yAmt = 32000;
			}
			else
			{
				yAmt = (iTopOld - iTopNew) * plb->cyChar;
			}
		}

		dwFlags = LBGetScrollFlags(plb, dwTime);
		//		ScrollWindowEx(HWq(plb->spwnd), xAmt, yAmt, NULL, &rc, NULL, NULL, dwFlags);
		::InvalidateRect(plb->spwnd, NULL, FALSE);
		UpdateWindow(HWq(plb->spwnd));
	}

	// Note that although we turn off the caret regardless of redraw, we
	// only turn it on if redraw is true. Slimy thing to fixup many
	// caret related bugs...
	if (fCaretOn)
		// Turn the caret back on only if we turned it off. This avoids
		// annoying caret flicker.
	{
		xxxLBSetCaret(plb, TRUE);
	}
}


/***************************************************************************\
* xxxInsureVisible
*
* History:
\***************************************************************************/

void xxxInsureVisible(
	PLBIV plb,
	INT iSel,
	BOOL fPartial)  /* It is ok for the item to be partially visible */
{
	INT sLastVisibleItem;

	CheckLock(plb->spwnd);

	if (iSel < plb->iTop)
	{
		xxxNewITop(plb, iSel);
	}
	else
	{
		if (fPartial)
		{

			/*
			* 1 must be subtracted to get the last visible item
			* A part of the fix for Bug #3727 -- 01/14/91 -- SANKAR
			*/
			sLastVisibleItem = plb->iTop + CItemInWindow(plb, TRUE) - (INT)1;
		}
		else
		{
			sLastVisibleItem = LastFullVisible(plb);
		}

		if (plb->OwnerDraw != OWNERDRAWVAR)
		{
			if (iSel > sLastVisibleItem)
			{
				if (plb->fMultiColumn)
				{
					xxxNewITop(plb,
						((iSel / plb->itemsPerColumn) -
							max(plb->numberOfColumns - 1, 0)) * plb->itemsPerColumn);
				}
				else
				{
					xxxNewITop(plb, (INT)max(0, iSel - sLastVisibleItem + plb->iTop));
				}
			}
		}
		else if (iSel > sLastVisibleItem)
		{
			xxxNewITop(plb, LBPage(plb, iSel, FALSE));
		}
	}
}

/***************************************************************************\
* xxxLBoxCaretBlinker
*
* Timer callback function toggles Caret
* Since it is a callback, it is APIENTRY
*
* History:
\***************************************************************************/

VOID xxxLBoxCaretBlinker(
	HWND hwnd,
	UINT wMsg,
	UINT_PTR nIDEvent,
	DWORD dwTime)
{
	//PWND pwnd;
	//PLBIV plb;

	///*
	// * Standard parameters for a timer callback function that aren't used.
	// * Mentioned here to avoid compiler warnings
	// */
	//UNREFERENCED_PARAMETER(wMsg);
	//UNREFERENCED_PARAMETER(nIDEvent);
	//UNREFERENCED_PARAMETER(dwTime);

	//pwnd = ValidateHwnd(hwnd);
	//plb = ((PLBWND)pwnd)->pLBIV;

	///*
	// * leave caret on, don't blink it off (prevents rapid blinks?)
	// */
	//if (ISREMOTESESSION() && plb->fCaretOn)
	//{
	//	return;
	//}

	///*
	// * Check if the Caret is ON, if so, switch it OFF
	// */
	//xxxLBSetCaret(plb, !plb->fCaretOn);
	return;
}


/***************************************************************************\
* xxxLBoxCtlKeyInput
*
* If msg == LB_KEYDOWN, vKey is the number of the item to go to,
* otherwise it is the virtual key.
*
* History:
\***************************************************************************/

void xxxLBoxCtlKeyInput(
	PLBIV plb,
	UINT msg,
	UINT vKey)
{
	INT i;
	INT iNewISel;
	INT cItemPageScroll;
	PCBOX pcbox;
	BOOL fDropDownComboBox;
	BOOL fExtendedUIComboBoxClosed;
	BOOL hScrollBar = FALSE;//TestWF(plb->spwnd, WFHSCROLL);
	UINT wModifiers = 0;
	BOOL fSelectKey = FALSE;  /* assume it is a navigation key */
	UINT uEvent = 0;
	HWND hwnd = HWq(plb->spwnd);
	TL tlpwndParent;
	TL tlpwnd;

	CheckLock(plb->spwnd);

	pcbox = plb->pcbox;

	/*
	* Is this a dropdown style combo box/listbox ?
	*/
	fDropDownComboBox = pcbox && (pcbox->CBoxStyle & SDROPPABLE);

	/*
	*Is this an extended ui combo box which is closed?
	*/
	fExtendedUIComboBoxClosed = fDropDownComboBox && pcbox->fExtendedUI &&
		!pcbox->fLBoxVisible;

	if (plb->fMouseDown || (!plb->cMac && vKey != VK_F4))
	{

		/*
		* Ignore keyboard input if we are in the middle of a mouse down deal or
		* if there are no items in the listbox. Note that we let F4's go
		* through for combo boxes so that the use can pop up and down empty
		* combo boxes.
		*/
		return;
	}

	/*
	* Modifiers are considered only in EXTENDED sel list boxes.
	*/
	if (plb->wMultiple == EXTENDEDSEL)
	{

		/*
		* If multiselection listbox, are any modifiers used ?
		*/
		if (GetKeyState(VK_SHIFT) < 0)
		{
			wModifiers = SHIFTDOWN;
		}
		if (GetKeyState(VK_CONTROL) < 0)
		{
			wModifiers += CTLDOWN;
		}

		/*
		* Please Note that (SHIFTDOWN + CTLDOWN) == (SHCTLDOWN)
		*/
	}

	if (msg == LB_KEYDOWN)
	{

		/*
		* This is a listbox "go to specified item" message which means we want
		* to go to a particular item number (given by vKey) directly.  ie.  the
		* user has typed a character and we want to go to the item which
		* starts with that character.
		*/
		iNewISel = (INT)vKey;
		goto TrackKeyDown;
	}

	cItemPageScroll = plb->cItemFullMax;

	if (cItemPageScroll > 1)
	{
		cItemPageScroll--;
	}

	if (plb->fWantKeyboardInput)
	{

		/*
		* Note: msg must not be LB_KEYDOWN here or we'll be in trouble...
		*/
		ThreadLock(plb->spwndParent, &tlpwndParent);
		iNewISel = (INT)SendMessage(HW(plb->spwndParent), WM_VKEYTOITEM,
			MAKELONG(vKey, plb->iSelBase), (LPARAM)hwnd);
		ThreadUnlock(&tlpwndParent);

		if (iNewISel == -2)
		{

			/*
			* Don't move the selection...
			*/
			return;
		}
		if (iNewISel != -1)
		{

			/*
			* Jump directly to the item provided by the app
			*/
			goto TrackKeyDown;
		}

		/*
		* else do default processing of the character.
		*/
	}

	switch (vKey)
	{
		// LATER IanJa: not language independent!!!
		// We could use VkKeyScan() to find out which is the '\' key
		// This is VK_OEM_5 '\|' for US English only.
		// Germans, Italians etc. have to type CTRL+^ (etc) for this.
		// This is documented as File Manager behaviour for 3.0, but apparently
		// not for 3.1., although functionality remains. We should still fix it,
		// although German (etc?) '\' is generated with AltGr (Ctrl-Alt) (???)
	case VERKEY_BACKSLASH:  /* '\' character for US English */

							/*
							* Check if this is CONTROL-\ ; If so Deselect all items
							*/
		if ((wModifiers & CTLDOWN) && (plb->wMultiple != SINGLESEL))
		{
			xxxLBSetCaret(plb, FALSE);
			xxxResetWorld(plb, plb->iSelBase, plb->iSelBase, FALSE);

			/*
			* And select the current item
			*/
			SetSelected(plb, plb->iSelBase, TRUE, HILITEANDSEL);
			xxxInvertLBItem(plb, plb->iSelBase, TRUE);
			uEvent = EVENT_OBJECT_SELECTION;
			goto CaretOnAndNotify;
		}
		return;
		break;

	case VK_DIVIDE:     /* NumPad '/' character on enhanced keyboard */
						// LATER IanJa: not language independent!!!
						// We could use VkKeyScan() to find out which is the '/' key
						// This is VK_OEM_2 '/?' for US English only.
						// Germans, Italians etc. have to type CTRL+# (etc) for this.
	case VERKEY_SLASH:  /* '/' character */

						/*
						* Check if this is CONTROL-/ ; If so select all items
						*/
		if ((wModifiers & CTLDOWN) && (plb->wMultiple != SINGLESEL))
		{
			xxxLBSetCaret(plb, FALSE);
			xxxResetWorld(plb, -1, -1, TRUE);

			uEvent = EVENT_OBJECT_SELECTIONWITHIN;

		CaretOnAndNotify:
			xxxLBSetCaret(plb, TRUE);
			//if (FWINABLE())
			//{
			//	LBEvent(plb, uEvent, plb->iSelBase);
			//}
			xxxNotifyOwner(plb, LBN_SELCHANGE);
		}
		return;
		break;

	case VK_F8:

		/*
		* The "Add" mode is possible only in Multiselection listboxes...  Get
		* into it via SHIFT-F8...  (Yes, sometimes these UI people are sillier
		* than your "typical dumb user"...)
		*/
		if (plb->wMultiple != SINGLESEL && wModifiers == SHIFTDOWN)
		{

			/*
			* We have to make the caret blink! Do something...
			*/
			if (plb->fAddSelMode)
			{

				/*
				* Switch off the Caret blinking
				*/
				KillTimer(hwnd, IDSYS_CARET);

				/*
				* Make sure the caret does not vanish
				*/
				xxxLBSetCaret(plb, TRUE);
			}
			else
			{

				/*
				* Create a timer to make the caret blink
				*/
				SetTimer(hwnd, IDSYS_CARET, 40,
					(TIMERPROC)xxxLBoxCaretBlinker);
			}

			/*
			* Toggle the Add mode flag
			*/
			plb->fAddSelMode = (UINT)!plb->fAddSelMode;
		}
		return;
	case VK_SPACE:  /* Selection key is space */
		i = 0;
		fSelectKey = TRUE;
		break;

	case VK_PRIOR:
		if (fExtendedUIComboBoxClosed)
		{

			/*
			* Disable movement keys for TandyT.
			*/
			return;
		}

		if (plb->OwnerDraw == OWNERDRAWVAR)
		{
			i = LBPage(plb, plb->iSelBase, FALSE) - plb->iSelBase;
		}
		else
		{
			i = -cItemPageScroll;
		}
		break;

	case VK_NEXT:
		if (fExtendedUIComboBoxClosed)
		{

			/*
			* Disable movement keys for TandyT.
			*/
			return;
		}

		if (plb->OwnerDraw == OWNERDRAWVAR)
		{
			i = LBPage(plb, plb->iSelBase, TRUE) - plb->iSelBase;
		}
		else
		{
			i = cItemPageScroll;
		}
		break;

	case VK_HOME:
		if (fExtendedUIComboBoxClosed)
		{

			/*
			* Disable movement keys for TandyT.
			*/
			return;
		}

		i = (INT_MIN / 2) + 1; /* A very big negative number */
		break;

	case VK_END:
		if (fExtendedUIComboBoxClosed)
		{

			/*
			* Disable movement keys for TandyT.
			*/
			return;
		}

		i = (INT_MAX / 2) - 1; /* A very big positive number */
		break;

	case VK_LEFT:
		if (plb->fMultiColumn)
		{
			if (plb->fRightAlign
#ifdef USE_MIRRORING
				^ (!!TestWF(plb->spwnd, WEFLAYOUTRTL))

#endif
				)
			{
				goto ReallyRight;
			}
		ReallyLeft:
			if (plb->iSelBase / plb->itemsPerColumn == 0)
			{
				i = 0;
			}
			else
			{
				i = -plb->itemsPerColumn;
			}
			break;
		}

		if (hScrollBar)
		{
			goto HandleHScrolling;
		}
		else
		{

			/*
			* Fall through and handle this as if the up arrow was pressed.
			*/

			vKey = VK_UP;
		}

		/*
		* Fall through
		*/

	case VK_UP:
		if (fExtendedUIComboBoxClosed)
			// Disable movement keys for TandyT.
		{
			return;
		}

		i = -1;
		break;

	case VK_RIGHT:
		if (plb->fMultiColumn)
		{
			if (plb->fRightAlign
#ifdef USE_MIRRORING
				^ (!!TestWF(plb->spwnd, WEFLAYOUTRTL))

#endif
				)
			{
				goto ReallyLeft;
			}
		ReallyRight:
			if (plb->iSelBase / plb->itemsPerColumn == plb->cMac / plb->itemsPerColumn)
			{
				i = 0;
			}
			else
			{
				i = plb->itemsPerColumn;
			}
			break;
		}
		if (hScrollBar)
		{
		HandleHScrolling:
			PostMessage(hwnd, WM_HSCROLL,
				(vKey == VK_RIGHT ? SB_LINEDOWN : SB_LINEUP), 0L);
			return;
		}
		else
		{

			/*
			* Fall through and handle this as if the down arrow was
			* pressed.
			*/
			vKey = VK_DOWN;
		}

		/*
		* Fall through
		*/

	case VK_DOWN:
		if (fExtendedUIComboBoxClosed)
		{

			/*
			* If the combo box is closed, down arrow should open it.
			*/
			if (!pcbox->fLBoxVisible)
			{

				/*
				* If the listbox isn't visible, just show it
				*/
				ThreadLock(pcbox->spwnd, &tlpwnd);
				xxxCBShowListBoxWindow(pcbox, TRUE);
				ThreadUnlock(&tlpwnd);
			}
			return;
		}
		i = 1;
		break;

	case VK_ESCAPE:
	case VK_RETURN:
		if (!fDropDownComboBox || !pcbox->fLBoxVisible)
		{
			return;
		}

		// |  If this is a dropped listbox for a combobox and the ENTER  |
		// |  key is pressed, close up the listbox, so FALLTHRU          |
		// V                                                             V

	case VK_F4:
		if (fDropDownComboBox && !pcbox->fExtendedUI)
		{

			/*
			* If we are a dropdown combo box/listbox we want to process
			* this key.  BUT for TandtT, we don't do anything on VK_F4 if we
			* are in extended ui mode.
			*/
			ThreadLock(pcbox->spwnd, &tlpwnd);
			if (!pcbox->fLBoxVisible)
			{

				/*
				* If the listbox isn't visible, just show it
				*/
				xxxCBShowListBoxWindow(pcbox, (vKey != VK_ESCAPE));
			}
			else
			{

				/*
				* Ok, the listbox is visible.  So hide the listbox window.
				*/
				xxxCBHideListBoxWindow(pcbox, TRUE, (vKey != VK_ESCAPE));
			}
			ThreadUnlock(&tlpwnd);
		}

		/*
		* Fall through to the return
		*/

	default:
		return;
	}

	/*
	* Find out what the new selection should be
	*/
	iNewISel = IncrementISel(plb, plb->iSelBase, i);


	if (plb->wMultiple == SINGLESEL)
	{
		if (plb->iSel == iNewISel)
		{

			/*
			* If we are single selection and the keystroke is moving us to an
			* item which is already selected, we don't have to do anything...
			*/
			return;
		}

		uEvent = EVENT_OBJECT_SELECTION;

		plb->iTypeSearch = 0;
		if ((vKey == VK_UP || vKey == VK_DOWN) &&
			!IsSelected(plb, plb->iSelBase, HILITEONLY))
		{

			/*
			* If the caret is on an unselected item and the user just hits the
			* up or down arrow key (ie. with no shift or ctrl modifications),
			* then we will just select the item the cursor is at. This is
			* needed for proper behavior in combo boxes but do we always want
			* to run this code??? Note that this is only used in single
			* selection list boxes since it doesn't make sense in the
			* multiselection case. Note that an LB_KEYDOWN message must not be
			* checked here because the vKey will be an item number not a
			* VK_and we will goof. Thus, trackkeydown label is below this to
			* fix a bug caused by it being above this...
			*/
			iNewISel = (plb->iSelBase == -1) ? 0 : plb->iSelBase;
		}
	}

TrackKeyDown:

	xxxSetISelBase(plb, iNewISel);

	xxxLBSetCaret(plb, FALSE);

	if (wModifiers & SHIFTDOWN)
	{
		// Check if iMouseDown is un-initialised
		if (plb->iMouseDown == -1)
		{
			plb->iMouseDown = iNewISel;
		}
		if (plb->iLastMouseMove == -1)
		{
			plb->iLastMouseMove = iNewISel;
		}

		// Check if we are in ADD mode
		if (plb->fAddSelMode)
		{
			/* Preserve all the pre-existing selections except the
			* ones connected with the last anchor point; If the last
			* Preserve all the previous selections
			*/
			/* Deselect only the selection connected with the last
			* anchor point; If the last anchor point is associated
			* with de-selection, then do not do it
			*/

			if (!plb->fNewItemState)
			{
				plb->iLastMouseMove = plb->iMouseDown;
			}

			/* We haven't done anything here because, LBBlockHilite()
			* will take care of wiping out the selection between
			* Anchor point and iLastMouseMove and select the block
			* between anchor point and current cursor location
			*/
		}
		else
		{
			/* We are not in ADD mode */
			/* Remove all selections except between the anchor point
			* and last mouse move because it will be taken care of in
			* LBBlockHilite
			*/
			xxxResetWorld(plb, plb->iMouseDown, plb->iLastMouseMove, FALSE);
		}

		uEvent = EVENT_OBJECT_SELECTIONWITHIN;

		/* LBBlockHilite takes care to deselect the block between
		* the anchor point and iLastMouseMove and select the block
		* between the anchor point and the current cursor location
		*/
		/* Toggle all items to the same selection state as the item
		* item at the anchor point) from the anchor point to the
		* current cursor location.
		*/
		plb->fNewItemState = IsSelected(plb, plb->iMouseDown, SELONLY);
		xxxLBBlockHilite(plb, iNewISel, TRUE);

		plb->iLastMouseMove = iNewISel;
		/* Preserve the existing anchor point */
	}
	else
	{
		/* Check if this is in ADD mode */
		if ((plb->fAddSelMode) || (plb->wMultiple == MULTIPLESEL))
		{
			/* Preserve all pre-exisiting selections */
			if (fSelectKey)
			{
				/* Toggle the selection state of the current item */
				plb->fNewItemState = !IsSelected(plb, iNewISel, SELONLY);
				SetSelected(plb, iNewISel, plb->fNewItemState, HILITEANDSEL);

				xxxInvertLBItem(plb, iNewISel, plb->fNewItemState);

				/* Set the anchor point at the current location */
				plb->iLastMouseMove = plb->iMouseDown = iNewISel;
				uEvent = (plb->fNewItemState ? EVENT_OBJECT_SELECTIONADD :
					EVENT_OBJECT_SELECTIONREMOVE);
			}
		}
		else
		{
			/* We are NOT in ADD mode */
			/* Remove all existing selections except iNewISel, to
			* avoid flickering.
			*/
			xxxResetWorld(plb, iNewISel, iNewISel, FALSE);

			/* Select the current item */
			SetSelected(plb, iNewISel, TRUE, HILITEANDSEL);
			xxxInvertLBItem(plb, iNewISel, TRUE);

			/* Set the anchor point at the current location */
			plb->iLastMouseMove = plb->iMouseDown = iNewISel;
			uEvent = EVENT_OBJECT_SELECTION;
		}
	}

	/*
	* Move the cursor to the new location
	*/
	xxxInsureVisible(plb, iNewISel, FALSE);
	xxxLBShowHideScrollBars(plb);

	xxxLBSetCaret(plb, TRUE);

	//if (FWINABLE() && uEvent)
	//{
	//	LBEvent(plb, uEvent, iNewISel);
	//}

	/*
	* Should we notify our parent?
	*/
	if (plb->fNotify)
	{
		if (fDropDownComboBox && pcbox->fLBoxVisible)
		{

			/*
			* If we are in a drop down combo box/listbox and the listbox is
			* visible, we need to set the fKeyboardSelInListBox bit so that the
			* combo box code knows not to hide the listbox since the selchange
			* message is caused by the user keyboarding through...
			*/
			pcbox->fKeyboardSelInListBox = TRUE;
			plb->iLastSelection = iNewISel;
		}
		xxxNotifyOwner(plb, LBN_SELCHANGE);
	}
}


/***************************************************************************\
* Compare
*
* Is lpstr1 equal/prefix/less-than/greater-than lsprst2 (case-insensitive) ?
*
* LATER IanJa: this assume a longer string is never a prefix of a longer one.
* Also assumes that removing 1 or more characters from the end of a string will
* give a string tahs sort before the original.  These assumptions are not valid
* for all languages.  We nedd better support from NLS. (Consider French
* accents, Spanish c/ch, ligatures, German sharp-s/SS, etc.)
*
* History:
\***************************************************************************/

INT Compare(
	LPCWSTR pwsz1,
	LPCWSTR pwsz2,
	DWORD dwLocaleId)
{
	UINT len1 = wcslen(pwsz1);
	UINT len2 = wcslen(pwsz2);
	INT result;

	/*
	* CompareStringW returns:
	*    1 = pwsz1  <  pwsz2
	*    2 = pwsz1  == pwsz2
	*    3 = pwsz1  >  pwsz2
	*/
	result = CompareStringW((LCID)dwLocaleId, NORM_IGNORECASE,
		pwsz1, min(len1, len2), pwsz2, min(len1, len2));

	if (result == CSTR_LESS_THAN)
	{
		return LT;
	}
	else if (result == CSTR_EQUAL)
	{
		if (len1 == len2)
		{
			return EQ;
		}
		else if (len1 < len2)
		{
			/*
			* LATER IanJa: should not assume shorter string is a prefix
			* Spanish "c" and "ch", ligatures, German sharp-s/SS etc.
			*/
			return PREFIX;
		}
	}
	return GT;
}

/***************************************************************************\
* xxxFindString
*
* Scans for a string in the listbox prefixed by or equal to lpstr.
* For OWNERDRAW listboxes without strings and without the sort style, we
* try to match the long app supplied values.
*
* History:
* 16-Apr-1992 beng      The NODATA case
\***************************************************************************/

INT xxxFindString(
	PLBIV plb,
	LPWSTR lpstr,
	INT sStart,
	INT code,
	BOOL fWrap)
{
	/*
	* Search for a prefix match (case-insensitive equal/prefix)
	* sStart == -1 means start from beginning, else start looking at sStart+1
	* assumes cMac > 0.
	*/
	INT sInd;  /* index of string */
	INT sStop;          /* index to stop searching at */
	lpLBItem pRg;
	TL tlpwndParent;
	INT sortResult;

	/*
	* Owner-Draw version of pRg
	*/
#define pODRg ((lpLBODItem)pRg)
	COMPAREITEMSTRUCT cis;
	LPWSTR listboxString;

	CheckLock(plb->spwnd);

	if (plb->fHasStrings && (!lpstr || !*lpstr))
	{
		return LB_ERR;
	}

	if (!plb->fHasData)
	{
		//RIPERR0(ERROR_INVALID_PARAMETER, RIP_WARNING, "FindString called on NODATA lb");
		return LB_ERR;
	}

	if ((sInd = sStart + 1) >= plb->cMac)
	{
		sInd = (fWrap ? 0 : plb->cMac - 1);
	}

	sStop = (fWrap ? sInd : 0);

	/*
	* If at end and no wrap, stop right away
	*/
	if (((sStart >= plb->cMac - 1) && !fWrap) || (plb->cMac < 1))
	{
		return LB_ERR;
	}

	/*
	* Apps could pass in an invalid sStart like -2 and we would blow up.
	* Win 3.1 would not so we need to fixup sInd to be zero
	*/
	if (sInd < 0)
	{
		sInd = 0;
	}

	pRg = (lpLBItem)(plb->rgpch);

	do
	{
		if (plb->fHasStrings)
		{

			/*
			* Searching for string matches.
			*/
			listboxString = (LPWSTR)((LPBYTE)plb->hStrings + pRg[sInd].offsz);

			if (code == PREFIX &&
				listboxString &&
				*lpstr != TEXT('[') &&
				*listboxString == TEXT('['))
			{

				/*
				* If we are looking for a prefix string and the first items
				* in this string are [- then we ignore them.  This is so
				* that in a directory listbox, the user can goto drives
				* by selecting the drive letter.
				*/
				listboxString++;
				if (*listboxString == TEXT('-'))
				{
					listboxString++;
				}
			}

			if (Compare(lpstr, listboxString, plb->dwLocaleId) <= code)
			{
				goto FoundIt;
			}

		}
		else
		{
			if (plb->fSort)
			{

				/*
				* Send compare item messages to the parent for sorting
				*/
				cis.CtlType = ODT_LISTBOX;
				cis.CtlID = 0;//L PtrToUlong(plb->spwnd->spmenu);
				cis.hwndItem = HWq(plb->spwnd);
				cis.itemID1 = (UINT)(-1);
				cis.itemData1 = (ULONG_PTR)lpstr;
				cis.itemID2 = (UINT)sInd;
				cis.itemData2 = pODRg[sInd].itemData;
				cis.dwLocaleId = plb->dwLocaleId;

				ThreadLock(plb->spwndParent, &tlpwndParent);
				sortResult = (INT)SendMessage(HW(plb->spwndParent), WM_COMPAREITEM,
					cis.CtlID, (LPARAM)&cis);
				ThreadUnlock(&tlpwndParent);


				if (sortResult == -1)
				{
					sortResult = LT;
				}
				else if (sortResult == 1)
				{
					sortResult = GT;
				}
				else
				{
					sortResult = EQ;
				}

				if (sortResult <= code)
				{
					goto FoundIt;
				}
			}
			else
			{

				/*
				* Searching for app supplied long data matches.
				*/
				if ((ULONG_PTR)lpstr == pODRg[sInd].itemData)
				{
					goto FoundIt;
				}
			}
		}

		/*
		* Wrap round to beginning of list
		*/
		if (++sInd == plb->cMac)
		{
			sInd = 0;
		}
	} while (sInd != sStop);

	sInd = -1;

FoundIt:
	return sInd;
}


/***************************************************************************\
* xxxLBoxCtlCharInput
*
* History:
\***************************************************************************/

void xxxLBoxCtlCharInput(
	PLBIV plb,
	UINT  inputChar,
	BOOL  fAnsi)
{
	INT iSel;
	BOOL fControl;
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	if (plb->cMac == 0 || plb->fMouseDown)
	{

		/*
		* Get out if we are in the middle of mouse routines or if we have no
		* items in the listbox, we just return without doing anything.
		*/
		return;
	}

	fControl = (GetKeyState(VK_CONTROL) < 0);

	switch (inputChar)
	{
	case VK_ESCAPE:
		plb->iTypeSearch = 0;
		if (plb->pszTypeSearch)
		{
			plb->pszTypeSearch[0] = 0;
		}
		break;

	case VK_BACK:
		if (plb->iTypeSearch)
		{
			plb->pszTypeSearch[plb->iTypeSearch--] = 0;
			if (plb->fSort)
			{
				iSel = -1;
				goto TypeSearch;
			}
		}
		break;

	case VK_SPACE:
		if (plb->fAddSelMode || plb->wMultiple == MULTIPLESEL)
		{
			break;
		}
		/* Otherwise, for single/extended selection listboxes not in add
		* selection mode, let the  space go thru as a type search character
		* FALL THRU
		*/

	default:

		/*
		* Move selection to first item beginning with the character the
		* user typed.  We don't want do this if we are using owner draw.
		*/

		//if (fAnsi && IS_DBCS_ENABLED() && IsDBCSLeadByteEx(THREAD_CODEPAGE(), (BYTE)inputChar))
		//{
		//	WCHAR wch;
		//	LPWSTR lpwstr = &wch;

		//	inputChar = DbcsCombine(HWq(plb->spwnd), (BYTE)inputChar);
		//	RIPMSG1(RIP_VERBOSE, "xxxLBoxCtlCharInput: combined DBCS. 0x%04x", inputChar);

		//	if (inputChar == 0)
		//	{
		//		RIPMSG1(RIP_WARNING, "xxxLBoxCtlCharInput: cannot combine two DBCS. LB=0x%02x",
		//			inputChar);
		//		break;
		//	}
		//	// If it is DBCS, let's ignore the ctrl status.
		//	fControl = FALSE;

		//	// Convert DBCS to UNICODE.
		//	// Note: Leading byte is in the low byte, trailing byte is in high byte.
		//	// Let's assume Little Endian CPUs only, so inputChar can directly be
		//	// input for MBSToWCSEx as an ANSI string.
		//	if (MBToWCSEx(THREAD_CODEPAGE(), (LPCSTR)&inputChar, 2, &lpwstr, 1, FALSE) == 0)
		//	{
		//		RIPMSG1(RIP_WARNING, "xxxLBoxCtlCharInput: cannot convert 0x%04x to UNICODE.",
		//			inputChar);
		//		break;
		//	}
		//	inputChar = wch;
		//}

		if (plb->fHasStrings)
		{
			// Incremental Type Search processing
			//
			// update szTypeSearch string and then move to the first item from
			// the current selection whose prefix matches szTypeSearch
			//
			// the szTypeSearch will continue to grow until a "long enough"
			// gap between key entries is encountered -- at which point any
			// more searching will start over

			/*
			* Undo CONTROL-char to char
			*/
			if (fControl && inputChar < 0x20)
			{
				inputChar += 0x40;
			}

			if (plb->iTypeSearch == MAX_TYPESEARCH)
			{
				//NtUserMessageBeep(0);
				break;
			}
			iSel = -1;

			if (plb->pszTypeSearch == NULL)
			{
				plb->pszTypeSearch = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY, sizeof(WCHAR) * (MAX_TYPESEARCH + 1));
			}

			if (plb->pszTypeSearch == NULL)
			{
				//NtUserMessageBeep(0);
				break;
			}

			plb->pszTypeSearch[plb->iTypeSearch++] = (WCHAR)inputChar;
			plb->pszTypeSearch[plb->iTypeSearch] = 0;

		TypeSearch:
			if (plb->fSort)
			{
				// Set timer to determine when to kill incremental searching
				SetTimer(HWq(plb->spwnd), IDSYS_LBSEARCH,
					50, NULL);
			}
			else
			{
				// If this is not a sorted listbox, no incremental search.
				plb->iTypeSearch = 0;
				iSel = plb->iSelBase;
			}


			/*
			* Search for the item beginning with the given character starting
			* at iSel+1.  We will wrap the search to the beginning of the
			* listbox if we don't find the item.   If SHIFT is down and we are
			* a multiselection lb, then the item's state will be set to
			* plb->fNewItemState according to the current mode.
			*/
			iSel = xxxFindString(plb, plb->pszTypeSearch, iSel, PREFIX, TRUE);
			if (iSel == -1)
			{
				// no match found -- check for prefix match
				// (i.e. "p" find FIRST item that starts with 'p',
				//       "pp" find NEXT item that starts with 'p')
				if (plb->iTypeSearch)
				{
					plb->iTypeSearch--;
					if ((plb->iTypeSearch == 1) && (plb->pszTypeSearch[0] == plb->pszTypeSearch[1]))
					{
						plb->pszTypeSearch[1] = 0;
						iSel = xxxFindString(plb, plb->pszTypeSearch, plb->iSelBase, PREFIX, TRUE);
					}
				}
			}
			// if match is found -- select it
			if (iSel != -1)
			{
			CtlKeyInput:
				xxxLBoxCtlKeyInput(plb, LB_KEYDOWN, iSel);

			}
		}
		else
		{
			if (plb->spwndParent != NULL)
			{
				ThreadLock(plb->spwndParent, &tlpwndParent);
				iSel = (INT)SendMessage(plb->spwndParent, WM_CHARTOITEM,
					MAKELONG(inputChar, plb->iSelBase), (LPARAM)HWq(plb->spwnd));
				ThreadUnlock(&tlpwndParent);
			}
			else
			{
				iSel = -1;
			}

			if (iSel != -1 && iSel != -2)
			{
				goto CtlKeyInput;
			}

		}
		break;
	}
}


/***************************************************************************\
* LBoxGetSelItems
*
* effects: For multiselection listboxes, this returns the total number of
* selection items in the listbox if fCountOnly is true.  or it fills an array
* (lParam) with the items numbers of the first wParam selected items.
*
* History:
\***************************************************************************/

int LBoxGetSelItems(
	PLBIV plb,
	BOOL fCountOnly,
	int wParam,
	LPINT lParam)
{
	int i;
	int itemsselected = 0;

	if (plb->wMultiple == SINGLESEL)
	{
		return LB_ERR;
	}

	for (i = 0; i < plb->cMac; i++)
	{
		if (IsSelected(plb, i, SELONLY))
		{
			if (!fCountOnly)
			{
				if (itemsselected < wParam)
				{
					*lParam++ = i;
				}
				else
				{

					/*
					* That's all the items we can fit in the array.
					*/
					return itemsselected;
				}
			}
			itemsselected++;
		}
	}

	return itemsselected;
}


/***************************************************************************\
* xxxLBSetRedraw
*
* Handle WM_SETREDRAW message
*
* History:
\***************************************************************************/

void xxxLBSetRedraw(
	PLBIV plb,
	BOOL fRedraw)
{
	CheckLock(plb->spwnd);

	if (fRedraw)
	{
		fRedraw = TRUE;
	}

	if (plb->fRedraw != (UINT)fRedraw)
	{
		plb->fRedraw = !!fRedraw;

		if (fRedraw)
		{
			xxxLBSetCaret(plb, TRUE);
			xxxLBShowHideScrollBars(plb);

			if (plb->fDeferUpdate)
			{
				plb->fDeferUpdate = FALSE;
				RedrawWindow(HWq(plb->spwnd), NULL, NULL,
					RDW_INVALIDATE | RDW_ERASE |
					RDW_FRAME | RDW_ALLCHILDREN);
			}
		}
	}
}

/***************************************************************************\
* xxxLBSelRange
*
* Selects the range of items between i and j, inclusive.
*
* History:
\***************************************************************************/

void xxxLBSelRange(
	PLBIV plb,
	int iStart,
	int iEnd,
	BOOL fnewstate)
{
	DWORD temp;
	RECT rc;

	CheckLock(plb->spwnd);

	if (iStart > iEnd)
	{
		temp = iEnd;
		iEnd = iStart;
		iStart = temp;
	}

	/*
	* We don't want to loop through items that don't exist.
	*/
	iEnd = min(plb->cMac, iEnd);
	iStart = max(iStart, 0);
	if (iStart > iEnd)
	{
		return;
	}


	/*
	* iEnd could be equal to MAXINT which is why we test temp and iEnd
	* as DWORDs.
	*/
	for (temp = iStart; temp <= (DWORD)iEnd; temp++)
	{

		if (IsSelected(plb, temp, SELONLY) != fnewstate)
		{
			SetSelected(plb, temp, fnewstate, HILITEANDSEL);
			LBGetItemRect(plb, temp, &rc);

			xxxLBInvalidateRect(plb, (LPRECT)&rc, FALSE);
		}

	}
	//UserAssert(plb->wMultiple);
	//if (FWINABLE())
	//{
	//	LBEvent(plb, EVENT_OBJECT_SELECTIONWITHIN, iStart);
	//}
}


/***************************************************************************\
* xxxLBSetCurSel
*
* History:
\***************************************************************************/

int xxxLBSetCurSel(
	PLBIV plb,
	int iSel)
{
	CheckLock(plb->spwnd);

	if (!(plb->wMultiple || iSel < -1 || iSel >= plb->cMac))
	{
		xxxLBSetCaret(plb, FALSE);
		if (plb->iSel != -1)
		{

			/*
			* This prevents scrolling when iSel == -1
			*/
			if (iSel != -1)
			{
				xxxInsureVisible(plb, iSel, FALSE);
			}

			/*
			* Turn off old selection
			*/
			xxxInvertLBItem(plb, plb->iSel, FALSE);
		}

		if (iSel != -1)
		{
			xxxInsureVisible(plb, iSel, FALSE);
			plb->iSelBase = plb->iSel = iSel;

			/*
			* Highlight new selection
			*/
			xxxInvertLBItem(plb, plb->iSel, TRUE);
		}
		else
		{
			plb->iSel = -1;
			if (plb->cMac)
			{
				plb->iSelBase = min(plb->iSelBase, plb->cMac - 1);
			}
			else
			{
				plb->iSelBase = 0;
			}
		}

		//if (FWINABLE())
		//{
		//	/*
		//	 * Send both focus and selection events
		//	 */
		//	if (_IsWindowVisible(plb->spwnd))
		//	{
		//		LBEvent(plb, EVENT_OBJECT_FOCUS, plb->iSelBase);
		//		LBEvent(plb, EVENT_OBJECT_SELECTION, plb->iSel);
		//	}
		//}

		xxxLBSetCaret(plb, TRUE);
		return plb->iSel;
	}

	return LB_ERR;
}


/***************************************************************************\
* LBSetItemData
*
* Makes the item at index contain the data given.
*
* History:
* 16-Apr-1992 beng      The NODATA listbox case
\***************************************************************************/

int LBSetItemData(
	PLBIV plb,
	int index,
	LONG_PTR data)
{
	LPSTR lpItemText;

	/*
	* v-ronaar: fix bug #25865, don't allow negative indices!
	*/
	if ((index != -1) && ((UINT)index >= (UINT)plb->cMac))
	{
		//RIPERR1(ERROR_INVALID_INDEX, RIP_WARNING, "LBSetItemData with invalid index %x", index);
		return LB_ERR;
	}

	/*
	* No-data listboxes just ignore all LB_SETITEMDATA calls
	*/
	if (!plb->fHasData)
	{
		return TRUE;
	}

	lpItemText = (LPSTR)plb->rgpch;

	if (index == -1)
	{

		/*
		* index == -1 means set the data to all the items
		*/
		if (plb->fHasStrings)
		{
			for (index = 0; index < plb->cMac; index++)
			{

				((lpLBItem)lpItemText)->itemData = data;
				lpItemText += sizeof(LBItem);
			}
		}
		else
		{
			for (index = 0; index < plb->cMac; index++)
			{

				((lpLBODItem)lpItemText)->itemData = data;
				lpItemText += sizeof(LBODItem);
			}
		}
		return TRUE;
	}

	if (plb->fHasStrings)
	{

		lpItemText = (LPSTR)(lpItemText + (index * sizeof(LBItem)));
		((lpLBItem)lpItemText)->itemData = data;
	}
	else
	{

		lpItemText = (LPSTR)(lpItemText + (index * sizeof(LBODItem)));
		((lpLBODItem)lpItemText)->itemData = data;
	}
	return TRUE;
}

int LBSetItemBind(PLBIV plb, int index, HWLWND hBind)
{
	LPSTR lpItemText;

	/*
	* v-ronaar: fix bug #25865, don't allow negative indices!
	*/
	if ((index != -1) && ((UINT)index >= (UINT)plb->cMac))
	{
		//RIPERR1(ERROR_INVALID_INDEX, RIP_WARNING, "LBSetItemData with invalid index %x", index);
		return LB_ERR;
	}

	/*
	* No-data listboxes just ignore all LB_SETITEMDATA calls
	*/
	if (!plb->fHasData)
	{
		return TRUE;
	}

	lpItemText = (LPSTR)plb->rgpch;

	if (index == -1)
	{
		/*
		* index == -1 means set the data to all the items
		*/
		if (plb->fHasStrings)
		{
			for (index = 0; index < plb->cMac; index++)
			{
				((lpLBItem)lpItemText)->hBindWindowless = hBind;
				lpItemText += sizeof(LBItem);
			}
		}
		else
		{
			for (index = 0; index < plb->cMac; index++)
			{

				((lpLBODItem)lpItemText)->hBindWindowless = hBind;
				lpItemText += sizeof(LBODItem);
			}
		}
		return TRUE;
	}

	if (plb->fHasStrings)
	{

		lpItemText = (LPSTR)(lpItemText + (index * sizeof(LBItem)));
		((lpLBItem)lpItemText)->hBindWindowless = hBind;
	}
	else
	{

		lpItemText = (LPSTR)(lpItemText + (index * sizeof(LBODItem)));
		((lpLBODItem)lpItemText)->hBindWindowless = hBind;
	}

	return TRUE;
}

HWLWND LBGetItemBind(PLBIV plb, INT nItem)
{
	if (nItem < 0 || nItem >= plb->cMac)
	{
		//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
		return NULL;
	}

	int nItemSize = 0;
	if (plb->fHasStrings)
	{
		nItemSize = sizeof(LBItem);
	}
	else
	{
		nItemSize = sizeof(LBODItem);
	}

	LPBYTE lpItem = (plb->rgpch + nItem * nItemSize);
	HWLWND hBind = (plb->fHasStrings ? ((lpLBItem)lpItem)->hBindWindowless : ((lpLBODItem)lpItem)->hBindWindowless);
	return hBind;
}

int LBSetHighlightItemBkColor(PLBIV plb, COLORREF crItemBk)
{
	plb->m_crItemBkH = crItemBk;

	return 0;
}

COLORREF LBGetHighlightItemBkColor(PLBIV plb)
{
	return plb->m_crItemBkH;
}

/***************************************************************************\
* xxxCheckRedraw
*
* History:
\***************************************************************************/

void xxxCheckRedraw(
	PLBIV plb,
	BOOL fConditional,
	INT sItem)
{
	CheckLock(plb->spwnd);

	if (fConditional && plb->cMac &&
		(sItem > (plb->iTop + CItemInWindow(plb, TRUE))))
	{
		return;
	}

	/*
	* Don't do anything if the parent is not visible.
	*/
	xxxLBInvalidateRect(plb, (LPRECT)NULL, TRUE);
}


/***************************************************************************\
* xxxCaretDestroy
*
* History:
\***************************************************************************/

void xxxCaretDestroy(
	PLBIV plb)
{
	CheckLock(plb->spwnd);

	/*
	* We're losing the focus.  Act like up clicks are happening so we release
	* capture, set the current selection, notify the parent, etc.
	*/
	if (plb->fCaptured)

		/*
		* If we have the capture and we lost the focus, that means we already
		* changed the selection and we have to notify also the parent about
		* this. So we need to add also the LBUP_SUCCESS flag in this case.
		*/

		xxxLBButtonUp(plb, LBUP_RELEASECAPTURE | LBUP_NOTIFY |
		(plb->fMouseDown ? LBUP_SUCCESS : 0));

	if (plb->fAddSelMode)
	{

		/*
		* Switch off the Caret blinking
		*/
		KillTimer(HWq(plb->spwnd), IDSYS_CARET);

		/*
		* Make sure the caret goes away
		*/
		xxxLBSetCaret(plb, FALSE);
		plb->fAddSelMode = FALSE;
	}

	plb->fCaret = FALSE;
}


/***************************************************************************\
* xxxLbSetSel
*
* History:
\***************************************************************************/

LONG xxxLBSetSel(
	PLBIV plb,
	BOOL fSelect,  /* New state to set selection to */
	INT iSel)
{
	INT sItem;
	RECT rc;
	UINT uEvent = 0;

	CheckLock(plb->spwnd);

	/*
	* Bug 17656. WinZip's accelerator key for 'DeSelect All' sends a LB_SETSEL
	* message with lparam = 0x0000ffff instead of 0xffffffff(-1). If iSel
	* is equal to  0x0000ffff and there are less than 0xffff elements in the
	* list we set iSel equal to 0xffffffff.
	*/
	if ((iSel == (UINT)0xffff) && (iSel >= plb->cMac))
	{
		iSel = -1;
		//RIPMSG0(RIP_WARNING, "Sign extending iSel=0xffff to 0xffffffff");
	}


	if ((plb->wMultiple == SINGLESEL) || (iSel != -1 && iSel >= plb->cMac))
	{
		//RIPERR0(ERROR_INVALID_PARAMETER, RIP_WARNING,
		//	"xxxLBSetSel:Invalid iSel or SINGLESEL listbox");
		return LB_ERR;
	}

	xxxLBSetCaret(plb, FALSE);

	if (iSel == -1/*(INT)0xffff*/)
	{

		/*
		* Set/clear selection from all items if -1
		*/
		for (sItem = 0; sItem < plb->cMac; sItem++)
		{
			if (IsSelected(plb, sItem, SELONLY) != fSelect)
			{
				SetSelected(plb, sItem, fSelect, HILITEANDSEL);
				if (LBGetItemRect(plb, sItem, &rc))
				{
					xxxLBInvalidateRect(plb, &rc, FALSE);
				}
			}
		}
		xxxLBSetCaret(plb, TRUE);
		uEvent = EVENT_OBJECT_SELECTIONWITHIN;
	}
	else
	{
		if (fSelect)
		{

			/*
			* Check if the item if fully hidden and scroll it into view if it
			* is.  Note that we don't want to scroll partially visible items
			* into full view because this breaks the shell...
			*/
			xxxInsureVisible(plb, iSel, TRUE);
			plb->iSelBase = plb->iSel = iSel;

			plb->iMouseDown = plb->iLastMouseMove = iSel;
			uEvent = EVENT_OBJECT_FOCUS;
		}
		else
		{
			uEvent = EVENT_OBJECT_SELECTIONREMOVE;
		}
		SetSelected(plb, iSel, fSelect, HILITEANDSEL);

		/*
		* Note that we set the caret on bit directly so that we avoid flicker
		* when drawing this item.  ie.  We turn on the caret, redraw the item and
		* turn it back on again.
		*/
		if (!fSelect && plb->iSelBase != iSel)
		{
			xxxLBSetCaret(plb, TRUE);
		}
		else if (plb->fCaret)
		{
			plb->fCaretOn = TRUE;
		}

		if (LBGetItemRect(plb, iSel, &rc))
		{
			xxxLBInvalidateRect(plb, &rc, FALSE);
		}
	}

	//if (FWINABLE() && _IsWindowVisible(plb->spwnd))
	//{
	//	if (uEvent == EVENT_OBJECT_FOCUS)
	//	{
	//		LBEvent(plb, uEvent, plb->iSelBase);
	//		uEvent = EVENT_OBJECT_SELECTION;
	//	}
	//	LBEvent(plb, uEvent, iSel);
	//}

	return 0;
}


/***************************************************************************\
* xxxLBoxDrawItem
*
* This fills the draw item struct with some constant data for the given
* item.  The caller will only have to modify a small part of this data
* for specific needs.
*
* History:
* 16-Apr-1992 beng      The NODATA case
\***************************************************************************/

void xxxLBoxDrawItem(
	PLBIV plb,
	INT item,
	UINT itemAction,
	UINT itemState,
	LPRECT lprect)
{
	DRAWITEMSTRUCT dis;
	TL tlpwndParent;

	CheckLock(plb->spwnd);

	/*
	* Fill the DRAWITEMSTRUCT with the unchanging constants
	*/

	dis.CtlType = ODT_LISTBOX;
	dis.CtlID = 0;//L PtrToUlong(plb->spwnd->spmenu);

				  /*
				  * Use -1 if an invalid item number is being used.  This is so that the app
				  * can detect if it should draw the caret (which indicates the lb has the
				  * focus) in an empty listbox
				  */
	dis.itemID = (UINT)(item < plb->cMac ? item : -1);
	dis.itemAction = itemAction;
	dis.hwndItem = HWq(plb->spwnd);
	dis.hDC = plb->hdc;
	dis.itemState = itemState |
		(UINT)(!IsWindowEnabled(plb->spwnd) ? ODS_DISABLED : 0);

	//if (TestWF(plb->spwnd, WEFPUIFOCUSHIDDEN))
	//{
	//	dis.itemState |= ODS_NOFOCUSRECT;
	//}
	//if (TestWF(plb->spwnd, WEFPUIACCELHIDDEN))
	//{
	//	dis.itemState |= ODS_NOACCEL;
	//}

	/*
	* Set the app supplied data
	*/
	if (!plb->cMac || !plb->fHasData)
	{

		/*
		* If no strings or no items, just use 0 for data.  This is so that we
		* can display a caret when there are no items in the listbox.
		*
		* Lazy-eval listboxes of course have no data to pass - only itemID.
		*/
		dis.itemData = 0L;
	}
	else
	{
		dis.itemData = LBGetItemData(plb, item);
	}

	CopyRect(&dis.rcItem, lprect);

	/*
	* Set the window origin to the horizontal scroll position.  This is so that
	* text can always be drawn at 0,0 and the view region will only start at
	* the horizontal scroll offset. We pass this as wParam
	*/
	/*
	* Note:  Only pass the itemID in wParam for 3.1 or newer apps.  We break
	* ccMail otherwise.
	*/

	ThreadLock(plb->spwndParent, &tlpwndParent);
	SendMessage(HW(plb->spwndParent), WM_DRAWITEM,
		0,
		(LPARAM)&dis);
	ThreadUnlock(&tlpwndParent);
}


/***************************************************************************\
* xxxLBBlockHilite
*
*       In Extended selection mode for multiselection listboxes, when
*   mouse is draged to a new position, the range being marked should be
*   properly sized(parts of which will be highlighted/dehighlighted).
*   NOTE: This routine assumes that iSelFromPt and LasMouseMove are not
*          equal because only in that case this needs to be called;
*   NOTE: This routine calculates the region whose display attribute is to
*          be changed in an optimised way. Instead of de-highlighting the
*          the old range completely and highlight the new range, it omits
*          the regions that overlap and repaints only the non-pverlapping
*          area.
*   fKeyBoard = TRUE if this is called for Keyboard interface
*                FALSE if called from Mouse interface routines
*
* History:
\***************************************************************************/

void xxxLBBlockHilite(
	PLBIV plb,
	INT iSelFromPt,
	BOOL fKeyBoard)
{
	INT sCurPosOffset;
	INT sLastPosOffset;
	INT sHiliteOrSel;
	BOOL fUseSelStatus;
	BOOL DeHiliteStatus;

	CheckLock(plb->spwnd);

	if (fKeyBoard)
	{

		/*
		* Set both Hilite and Selection states
		*/
		sHiliteOrSel = HILITEANDSEL;

		/*
		* Do not use the Selection state while de-hiliting
		*/
		fUseSelStatus = FALSE;
		DeHiliteStatus = FALSE;
	}
	else
	{

		/*
		* Set/Reset only the Hilite state
		*/
		sHiliteOrSel = HILITEONLY;

		/*
		* Use the selection state for de-hilighting
		*/
		fUseSelStatus = TRUE;
		DeHiliteStatus = plb->fNewItemState;
	}



	/*
	* The idea of the routine is to :
	*          1.  De-hilite the old range (iMouseDown to iLastMouseDown)  and
	*          2.  Hilite the new range (iMouseDwon to iSelFromPt)
	*/

	/*
	* Offset of current mouse position from the anchor point
	*/
	sCurPosOffset = plb->iMouseDown - iSelFromPt;

	/*
	* Offset of last mouse position from the anchor point
	*/
	sLastPosOffset = plb->iMouseDown - plb->iLastMouseMove;

	/*
	* Check if both current position and last position lie on the same
	* side of the anchor point.
	*/
	if ((sCurPosOffset * sLastPosOffset) >= 0)
	{

		/*
		* Yes they are on the same side; So, highlight/dehighlight only
		* the difference.
		*/
		if (abs(sCurPosOffset) > abs(sLastPosOffset))
		{
			xxxAlterHilite(plb, plb->iLastMouseMove, iSelFromPt,
				plb->fNewItemState, sHiliteOrSel, FALSE);
		}
		else
		{
			xxxAlterHilite(plb, iSelFromPt, plb->iLastMouseMove, DeHiliteStatus,
				sHiliteOrSel, fUseSelStatus);
		}
	}
	else
	{
		xxxAlterHilite(plb, plb->iMouseDown, plb->iLastMouseMove,
			DeHiliteStatus, sHiliteOrSel, fUseSelStatus);
		xxxAlterHilite(plb, plb->iMouseDown, iSelFromPt,
			plb->fNewItemState, sHiliteOrSel, FALSE);
	}
}


/***************************************************************************\
* xxxAlterHilite
*
* Changes the hilite state of (i..j] (ie. excludes i, includes j in case
* you've forgotten this silly notation) to fHilite. It inverts this changes
* the hilite state.
*
*  OpFlags:  HILITEONLY      Only change the display state of the items
*            SELONLY         Only Change the selection state of the items
*            HILITEANDSELECT Do both.
*  fHilite:
*            HILITE/TRUE
*            DEHILITE/FALSE
*  fSelStatus:
*            if TRUE, use the selection state of the item to hilite/dehilite
*            if FALSE, use the fHilite parameter to hilite/dehilite
*
* History:
\***************************************************************************/

void xxxAlterHilite(
	PLBIV plb,
	INT i,
	INT j,
	BOOL fHilite,
	INT OpFlags,
	BOOL fSelStatus)
{
	INT low;
	INT high;
	INT sLastInWindow;
	BOOL fCaretOn;
	BOOL fSelected;

	CheckLock(plb->spwnd);

	sLastInWindow = plb->iTop + CItemInWindow(plb, TRUE);
	sLastInWindow = min(sLastInWindow, plb->cMac - 1);
	high = max(i, j) + 1;

	if (fCaretOn = plb->fCaretOn)
	{
		xxxLBSetCaret(plb, FALSE);
	}

	for (low = min(i, j); low < high; low++)
	{
		if (low != i)
		{
			if (OpFlags & HILITEONLY)
			{
				if (fSelStatus)
				{
					fSelected = IsSelected(plb, low, SELONLY);
				}
				else
				{
					fSelected = fHilite;
				}
				if (IsSelected(plb, low, HILITEONLY) != fSelected)
				{
					if (plb->iTop <= low && low <= sLastInWindow)
					{

						/*
						* Invert the item only if it is visible
						*/
						xxxInvertLBItem(plb, low, fSelected);
					}
					SetSelected(plb, low, fSelected, HILITEONLY);
				}
			}

			if (OpFlags & SELONLY)
			{
				SetSelected(plb, low, fHilite, SELONLY);
			}
		}
	}

	if (fCaretOn)
	{
		xxxLBSetCaret(plb, TRUE);
	}
}


//////////////////////////////////////////////////////////////////////////
// Infrequently Used List Box Routines
//////////////////////////////////////////////////////////////////////////


#pragma hdrstop

extern LOOKASIDE ListboxLookaside;

/***************************************************************************\
* LBSetCItemFullMax
*
* History:
* 03-04-92 JimA             Ported from Win 3.1 sources.
\***************************************************************************/

void LBSetCItemFullMax(
	PLBIV plb)
{
	if (plb->OwnerDraw != OWNERDRAWVAR)
	{
		plb->cItemFullMax = CItemInWindow(plb, FALSE);
	}
	else if (plb->cMac < 2)
	{
		plb->cItemFullMax = 1;
	}
	else
	{
		int     height;
		RECT    rect;
		int     i;
		int     j = 0;

		GetClientRect(plb->spwnd, &rect);
		height = rect.bottom;

		plb->cItemFullMax = 0;
		for (i = plb->cMac - 1; i >= 0; i--, j++)
		{
			height -= LBGetVariableHeightItemHeight(plb, i);

			if (height < 0)
			{
				plb->cItemFullMax = j;
				break;
			}
		}
		if (!plb->cItemFullMax)
		{
			plb->cItemFullMax = j;
		}
	}
}

/***************************************************************************\
* xxxCreateLBox
*
* History:
* 16-Apr-1992 beng      Added LBS_NODATA
\***************************************************************************/

LONG xxxLBCreate(
	PLBIV plb, PWND pwnd, LPCREATESTRUCT lpcs)
{
	UINT style;
	MEASUREITEMSTRUCT measureItemStruct;
	TL tlpwndParent;

	/*
	* Once we make it here, nobody can change the ownerdraw style bits
	* by calling SetWindowLong. The window style must match the flags in plb
	*
	*/
	plb->fInitialized = TRUE;

	style = GetWindowLong(pwnd, GWL_STYLE);

	/*
	* Compatibility hack.
	*/
	if (GetParent(pwnd) == NULL)
	{
		SetParent(pwnd, GetDesktopWindow());
	}
	//else
	//{
	//	Lock(&(plb->spwndParent), REBASEPWND(pwnd, spwndParent));
	//}

	/*
	* Break out the style bits
	*/
	plb->fRedraw = ((style & LBS_NOREDRAW) == 0);
	plb->fDeferUpdate = FALSE;
	plb->fNotify = (UINT)((style & LBS_NOTIFY) != 0);
	plb->fVertBar = ((style & WS_VSCROLL) != 0);
	plb->fHorzBar = ((style & WS_HSCROLL) != 0);

	//if (!TestWF(pwnd, WFWIN40COMPAT))
	//{
	//	// for 3.x apps, if either scroll bar was specified, the app got BOTH
	//	if (plb->fVertBar || plb->fHorzBar)
	//	{
	//		plb->fVertBar = plb->fHorzBar = TRUE;
	//	}
	//}

	plb->fRtoLReading = FALSE;//(TestWF(pwnd, WEFRTLREADING) != 0);
	plb->fRightAlign = FALSE;//(TestWF(pwnd, WEFRIGHT) != 0);
	plb->fDisableNoScroll = ((style & LBS_DISABLENOSCROLL) != 0);

	plb->fSmoothScroll = TRUE;

	/*
	* LBS_NOSEL gets priority over any other selection style.  Next highest
	* priority goes to LBS_EXTENDEDSEL. Then LBS_MULTIPLESEL.
	*/
	if (/*TestWF(pwnd, WFWIN40COMPAT) && */(style & LBS_NOSEL))
	{
		plb->wMultiple = SINGLESEL;
		plb->fNoSel = TRUE;
	}
	else if (style & LBS_EXTENDEDSEL)
	{
		plb->wMultiple = EXTENDEDSEL;
	}
	else
	{
		plb->wMultiple = (UINT)((style & LBS_MULTIPLESEL) ? MULTIPLESEL : SINGLESEL);
	}

	plb->fNoIntegralHeight = ((style & LBS_NOINTEGRALHEIGHT) != 0);
	plb->fWantKeyboardInput = ((style & LBS_WANTKEYBOARDINPUT) != 0);
	plb->fUseTabStops = ((style & LBS_USETABSTOPS) != 0);
	if (plb->fUseTabStops)
	{

		/*
		* Set tab stops every <default> dialog units.
		*/
		LBSetTabStops(plb, 0, NULL);
	}
	plb->fMultiColumn = ((style & LBS_MULTICOLUMN) != 0);
	plb->fHasStrings = TRUE;
	plb->iLastSelection = -1;
	plb->iMouseDown = -1;  /* Anchor point for multi selection */
	plb->iLastMouseMove = -1;

	/*
	* Get ownerdraw style bits
	*/
	if ((style & LBS_OWNERDRAWFIXED))
	{
		plb->OwnerDraw = OWNERDRAWFIXED;
	}
	else if ((style & LBS_OWNERDRAWVARIABLE) && !plb->fMultiColumn)
	{
		plb->OwnerDraw = OWNERDRAWVAR;

		/*
		* Integral height makes no sense with var height owner draw
		*/
		plb->fNoIntegralHeight = TRUE;
	}

	if (plb->OwnerDraw && !(style & LBS_HASSTRINGS))
	{

		/*
		* If owner draw, do they want the listbox to maintain strings?
		*/
		plb->fHasStrings = FALSE;
	}

	/*
	* If user specifies sort and not hasstrings, then we will send
	* WM_COMPAREITEM messages to the parent.
	*/
	plb->fSort = ((style & LBS_SORT) != 0);

	/*
	* "No data" lazy-eval listbox mandates certain other style settings
	*/
	plb->fHasData = TRUE;

	if (style & LBS_NODATA)
	{
		if (plb->OwnerDraw != OWNERDRAWFIXED || plb->fSort || plb->fHasStrings)
		{
			//RIPERR0(ERROR_INVALID_FLAGS, RIP_WARNING,
			//	"NODATA listbox must be OWNERDRAWFIXED, w/o SORT or HASSTRINGS");
		}
		else
		{
			plb->fHasData = FALSE;
		}
	}

	plb->dwLocaleId = GetThreadLocale();

	/*
	* Check if this is part of a combo box
	*/
	if ((style & LBS_COMBOBOX) != 0)
	{

		/*
		* Get the pcbox structure contained in the parent window's extra data
		* pointer.  Check cbwndExtra to ensure compatibility with SQL windows.
		*/
		//if (plb->spwndParent->cbwndExtra != 0)
		{
			plb->pcbox = (PCBOX)lpcs->lpCreateParams;
		}
	}

	/*
	* No need to set these to 0 since that was done for us when we Alloced
	* the PLBIV.
	*/

	/*
	* plb->rgpch       = (PBYTE)0;
	*/

	/*
	* plb->iSelBase    = plb->iTop = 0;
	*/

	/*
	* plb->fMouseDown  = FALSE;
	*/

	/*
	* plb->fCaret      = FALSE;
	*/

	/*
	* plb->fCaretOn    = FALSE;
	*/

	/*
	* plb->maxWidth    = 0;
	*/

	plb->iSel = -1;

	plb->hdc = NULL;

	/*
	* Set the keyboard state so that when the user keyboard clicks he selects
	* an item.
	*/
	plb->fNewItemState = TRUE;

	InitHStrings(plb);

	if (plb->fHasStrings && plb->hStrings == NULL)
	{
		return -1L;
	}

	//hdc = GetDC(HWq(pwnd));
	plb->cxChar = 16;//GdiGetCharDimensions(hdc, NULL, &plb->cyChar);
					 //ReleaseDC(HWq(pwnd), hdc);

	if (plb->cxChar == 0)
	{
		//RIPMSG0(RIP_WARNING, "xxxLBCreate: GdiGetCharDimensions failed");
		plb->cxChar = cxSysFontChar;
		plb->cyChar = cySysFontChar;
	}

	if (plb->OwnerDraw == OWNERDRAWFIXED)
	{

		/*
		* Query for item height only if we are fixed height owner draw.  Note
		* that we don't care about an item's width for listboxes.
		*/
		measureItemStruct.CtlType = ODT_LISTBOX;
		measureItemStruct.CtlID = 0;//PtrToUlong(pwnd->spmenu);

									/*
									* System font height is default height
									*/
		measureItemStruct.itemHeight = plb->cyChar;
		measureItemStruct.itemWidth = 0;
		measureItemStruct.itemData = 0;

		/*
		* IanJa: #ifndef WIN16 (32-bit Windows), plb->id gets extended
		* to LONG wParam automatically by the compiler
		*/
		ThreadLock(plb->spwndParent, &tlpwndParent);
		SendMessage(HW(plb->spwndParent), WM_MEASUREITEM,
			measureItemStruct.CtlID,
			(LPARAM)&measureItemStruct);
		ThreadUnlock(&tlpwndParent);

		/*
		* Use default height if given 0.  This prevents any possible future
		* div-by-zero errors.
		*/
		if (measureItemStruct.itemHeight)
		{
			plb->cyChar = measureItemStruct.itemHeight;
		}


		if (plb->fMultiColumn)
		{

			/*
			* Get default column width from measure items struct if we are a
			* multicolumn listbox.
			*/
			plb->cxColumn = measureItemStruct.itemWidth;
		}
	}
	else if (plb->OwnerDraw == OWNERDRAWVAR)
	{
		plb->cyChar = 0;
	}


	if (plb->fMultiColumn)
	{

		/*
		* Set these default values till we get the WM_SIZE message and we
		* calculate them properly.  This is because some people create a
		* 0 width/height listbox and size it later.  We don't want to have
		* problems with invalid values in these fields
		*/
		if (plb->cxColumn <= 0)
		{
			plb->cxColumn = 15 * plb->cxChar;
		}
		plb->numberOfColumns = plb->itemsPerColumn = 1;
	}

	LBSetCItemFullMax(plb);

	// Don't do this for 4.0 apps.  It'll make everyone's lives easier and
	// fix the anomaly that a combo & list created the same width end up
	// different when all is done.
	// B#1520

	//if (!TestWF(pwnd, WFWIN40COMPAT))
	//{
	//	plb->fIgnoreSizeMsg = TRUE;
	//	NtUserMoveWindow(HWq(pwnd),
	//		lpcs->x - SYSMET(CXBORDER),
	//		lpcs->y - SYSMET(CYBORDER),
	//		lpcs->cx + SYSMET(CXEDGE),
	//		lpcs->cy + GetSystemMetrics(SM_CYEDGE),
	//		FALSE);
	//	plb->fIgnoreSizeMsg = FALSE;
	//}

	if (!plb->fNoIntegralHeight)
	{

		/*
		* Send a message to ourselves to resize the listbox to an integral
		* height.  We need to do it this way because at create time we are all
		* mucked up with window rects etc...
		* IanJa: #ifndef WIN16 (32-bit Windows), wParam 0 gets extended
		* to wParam 0L automatically by the compiler.
		*/
		PostMessage(HWq(pwnd), WM_SIZE, 0, 0L);
	}

	return 1L;
}

/***************************************************************************\
* xxxLBoxDoDeleteItems
*
* Send DELETEITEM message for all the items in the ownerdraw listbox.
*
* History:
* 16-Apr-1992 beng          Nodata case
\***************************************************************************/

void xxxLBoxDoDeleteItems(
	PLBIV plb)
{
	INT sItem;

	CheckLock(plb->spwnd);

	/*
	* Send WM_DELETEITEM message for ownerdraw listboxes which are
	* being deleted.  (NODATA listboxes don't send such, though.)
	*/
	if (plb->OwnerDraw && plb->cMac && plb->fHasData)
	{
		for (sItem = plb->cMac - 1; sItem >= 0; sItem--)
		{
			xxxLBoxDeleteItem(plb, sItem);
		}
	}
}


/***************************************************************************\
* xxxDestroyLBox
*
* History:
\***************************************************************************/

void xxxDestroyLBox(
	PLBIV pLBIV,
	PWND pwnd)
{
	CheckLock(pwnd);

	if (pLBIV != NULL)
	{
		CheckLock(pLBIV->spwnd);

		/*
		* If ownerdraw, send deleteitem messages to parent
		*/
		xxxLBoxDoDeleteItems(pLBIV);

		if (pLBIV->rgpch != NULL)
		{
			UserLocalFree(pLBIV->rgpch);
			pLBIV->rgpch = NULL;
		}

		if (pLBIV->hStrings != NULL)
		{
			UserLocalFree(pLBIV->hStrings);
			pLBIV->hStrings = NULL;
		}

		if (pLBIV->iTabPixelPositions != NULL)
		{
			UserLocalFree((HANDLE)pLBIV->iTabPixelPositions);
			pLBIV->iTabPixelPositions = NULL;
		}

		//Unlock(&pLBIV->spwnd);
		//Unlock(&pLBIV->spwndParent);

		if (pLBIV->pszTypeSearch)
		{
			UserLocalFree(pLBIV->pszTypeSearch);
		}

		//FreeLookasideEntry(&ListboxLookaside, pLBIV);
	}

	/*
	* Set the window's fnid status so that we can ignore rogue messages
	*/
	NtUserSetWindowFNID(HWq(pwnd), FNID_CLEANEDUP_BIT);

	/*
	* If we're part of a combo box, let it know we're gone
	*/
	//pwndParent = REBASEPWND(pwnd, spwndParent);
	//if (pwndParent && GETFNID(pwndParent) == FNID_COMBOBOX)
	if (IsWindow(pLBIV->spwndParent))
	{
		SendMessage(pLBIV->spwndParent, WM_PARENTNOTIFY, MAKELONG(WM_DESTROY, 0), (LPARAM)pwnd);
	}
}


/***************************************************************************\
* xxxLBSetFont
*
* History:
\***************************************************************************/

void xxxLBSetFont(
	PLBIV plb,
	HANDLE hFont,
	BOOL fRedraw)
{
	//HDC     hdc;
	//HANDLE  hOldFont = NULL;
	//int     iHeight;

	//CheckLock(plb->spwnd);

	//plb->hFont = hFont;

	//hdc = GetDC(HWq(plb->spwnd));

	//if (hFont)
	//{
	//	hOldFont = SelectObject(hdc, hFont);
	//	if (!hOldFont)
	//	{
	//		plb->hFont = NULL;
	//	}
	//}

	//plb->cxChar = 40;//GdiGetCharDimensions(hdc, NULL, &iHeight);
	//if (plb->cxChar == 0)
	//{
	//	//RIPMSG0(RIP_WARNING, "xxxLBSetFont: GdiGetCharDimensions failed");
	//	plb->cxChar = cxSysFontChar;
	//	iHeight = cySysFontChar;
	//}


	//if (!plb->OwnerDraw && (plb->cyChar != iHeight))
	//{

	//	/*
	//	 * We don't want to mess up the cyChar height for owner draw listboxes
	//	 * so don't do this.
	//	 */
	//	plb->cyChar = iHeight;

	//	/*
	//	 * Only resize the listbox for 4.0 dudes, or combo dropdowns.
	//	 * Macromedia Director 4.0 GP-faults otherwise.
	//	 */
	//	if (!plb->fNoIntegralHeight &&
	//		(plb->pcbox/* || TestWF(plb->spwnd, WFWIN40COMPAT)*/))
	//	{
	//		xxxLBSize(plb,
	//			plb->spwnd->rcClient.right  - plb->spwnd->rcClient.left,
	//			plb->spwnd->rcClient.bottom - plb->spwnd->rcClient.top);
	//	}
	//}

	//if (hOldFont)
	//{
	//	SelectObject(hdc, hOldFont);
	//}

	///*
	// * IanJa: was ReleaseDC(hwnd, hdc);
	// */
	//ReleaseDC(HWq(plb->spwnd), hdc);

	//if (plb->fMultiColumn)
	//{
	//	LBCalcItemRowsAndColumns(plb);
	//}

	//LBSetCItemFullMax(plb);

	//if (fRedraw)
	//{
	//	xxxCheckRedraw(plb, FALSE, 0);
	//}
}


/***************************************************************************\
* xxxLBSize
*
* History:
\***************************************************************************/

void xxxLBSize(
	PLBIV plb,
	INT cx,
	INT cy)
{
	RECT rc;
	int iTopOld;
	BOOL fSizedSave;

	CheckLock(plb->spwnd);

	LONG style = GetWindowLong(plb->spwnd, GWL_STYLE);
	LONG ExStyle = GetWindowLong(plb->spwnd, GWL_EXSTYLE);
	CIUIRect rcWindow;
	GetWindowRect(plb->spwnd, rcWindow);
	if (!plb->fNoIntegralHeight)
	{
		int cBdrs = GetWindowBorders(style, ExStyle, TRUE, TRUE);

		CopyInflateRect(&rc, &rcWindow, 0, -cBdrs * 8);

		// Size the listbox to fit an integral # of items in its client
		if ((rc.bottom - rc.top) % plb->cyChar)
		{
			int iItems = (rc.bottom - rc.top);

			// B#2285 - If its a 3.1 app its SetWindowPos needs
			// to be window based dimensions not Client !
			// this crunches Money into using a scroll bar

			//if (! TestWF(plb->spwnd, WFWIN40COMPAT))
			//{
			iItems += (cBdrs * 8);    // so add it back in
									  //}

			iItems /= plb->cyChar;

			SetWindowPos(HWq(plb->spwnd), HWND_TOP, 0, 0, rc.right - rc.left,
				iItems * plb->cyChar + (8 * cBdrs),
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

			/*
			* Changing the size causes us to recurse.  Upon return
			* the state is where it should be and nothing further
			* needs to be done.
			*/
			return;
		}
	}

	//if (plb->fMultiColumn)
	//{

	//	/*
	//	 * Compute the number of DISPLAYABLE rows and columns in the listbox
	//	 */
	//	LBCalcItemRowsAndColumns(plb);
	//}
	//else
	{

		/*
		* Adjust the current horizontal position to eliminate as much
		* empty space as possible from the right side of the items.
		*/
		GetClientRect(plb->spwnd, &rc);
		if ((plb->maxWidth - plb->xOrigin) < (rc.right - rc.left))
		{
			plb->xOrigin = max(0, plb->maxWidth - (rc.right - rc.left));
		}
	}

	LBSetCItemFullMax(plb);

	/*
	* Adjust the top item in the listbox to eliminate as much empty space
	* after the last item as possible
	* (fix for bugs #8490 & #3836)
	*/
	iTopOld = plb->iTop;
	fSizedSave = plb->fSized;
	plb->fSized = FALSE;
	xxxNewITop(plb, plb->iTop);

	/*
	* If changing the top item index caused a resize, there is no
	* more work to be done here.
	*/
	if (plb->fSized)
	{
		return;
	}
	plb->fSized = fSizedSave;

	if (IsLBoxVisible(plb))
	{
		/*
		* This code no longer blows because it's fixed right!!!  We could
		* optimize the fMultiColumn case with some more code to figure out
		* if we really need to invalidate the whole thing but note that some
		* 3.0 apps depend on this extra invalidation (AMIPRO 2.0, bug 14620)
		*
		* For 3.1 apps, we blow off the invalidaterect in the case where
		* cx and cy are 0 because this happens during the processing of
		* the posted WM_SIZE message when we are created which would otherwise
		* cause us to flash.
		*/
		if ((plb->fMultiColumn && !(cx == 0 && cy == 0)) ||
			plb->iTop != iTopOld)
		{
			InvalidateRect(HWq(plb->spwnd), NULL, TRUE);
		}
		else if (plb->iSelBase >= 0)
		{

			/*
			* Invalidate the item with the caret so that if the listbox
			* grows horizontally, we redraw it properly.
			*/
			LBGetItemRect(plb, plb->iSelBase, &rc);
			InvalidateRect(HWq(plb->spwnd), &rc, FALSE);
		}
	}
	else if (!plb->fRedraw)
	{
		plb->fDeferUpdate = TRUE;
	}

	/*
	* Send "fake" scroll bar messages to update the scroll positions since we
	* changed size.
	*/
	//if (TestWF(plb->spwnd, WFVSCROLL))
	//{
	//	xxxLBoxCtlScroll(plb, SB_ENDSCROLL, 0);
	//}

	/*
	* We count on this to call LBShowHideScrollBars except when plb->cMac == 0!
	*/
	xxxLBoxCtlHScroll(plb, SB_ENDSCROLL, 0);

	/*
	* Show/hide scroll bars depending on how much stuff is visible...
	*
	* Note:  Now we only call this guy when cMac == 0, because it is
	* called inside the LBoxCtlHScroll with SB_ENDSCROLL otherwise.
	*/
	if (plb->cMac == 0)
	{
		xxxLBShowHideScrollBars(plb);
	}
}


/***************************************************************************\
* LBSetTabStops
*
* Sets the tab stops for this listbox. Returns TRUE if successful else FALSE.
*
* History:
\***************************************************************************/

BOOL LBSetTabStops(
	PLBIV plb,
	INT count,
	LPINT lptabstops)
{
	PINT ptabs;

	if (!plb->fUseTabStops)
	{
		//RIPERR0(ERROR_LB_WITHOUT_TABSTOPS, RIP_VERBOSE, "");
		return FALSE;
	}

	if (count)
	{
		/*
		* Allocate memory for the tab stops.  The first byte in the
		* plb->iTabPixelPositions array will contain a count of the number
		* of tab stop positions we have.
		*/
		ptabs = (LPINT)UserLocalAlloc(HEAP_ZERO_MEMORY, (count + 1) * sizeof(int));
		if (ptabs == NULL)
		{
			return FALSE;
		}

		if (plb->iTabPixelPositions != NULL)
		{
			UserLocalFree(plb->iTabPixelPositions);
		}
		plb->iTabPixelPositions = ptabs;

		/*
		* Set the count of tab stops
		*/
		*ptabs++ = count;

		for (; count > 0; count--)
		{

			/*
			* Convert the dialog unit tabstops into pixel position tab stops.
			*/
			*ptabs++ = MultDiv(*lptabstops, plb->cxChar, 4);
			lptabstops++;
		}
	}
	else
	{

		/*
		* Set default 8 system font ave char width tabs.  So free the memory
		* associated with the tab stop list.
		*/
		if (plb->iTabPixelPositions != NULL)
		{
			UserLocalFree((HANDLE)plb->iTabPixelPositions);
			plb->iTabPixelPositions = NULL;
		}
	}

	return TRUE;
}


/***************************************************************************\
* InitHStrings
*
* History:
\***************************************************************************/

void InitHStrings(
	PLBIV plb)
{
	if (plb->fHasStrings)
	{
		plb->ichAlloc = 0;
		plb->cchStrings = 0;
		plb->hStrings = (LPWSTR)UserLocalAlloc(0, 0L);
	}
}


/***************************************************************************\
* LBDropObjectHandler
*
* Handles a WM_DROPITEM message on this listbox
*
* History:
\***************************************************************************/

void LBDropObjectHandler(
	PLBIV plb,
	PDROPSTRUCT pds)
{
	LONG mouseSel;

	if (ISelFromPt(plb, pds->ptDrop, (LPDWORD)&mouseSel))
	{

		/*
		* User dropped in empty space at bottom of listbox
		*/
		pds->dwControlData = (DWORD)-1L;
	}
	else
	{
		pds->dwControlData = mouseSel;
	}
}


/***************************************************************************\
* LBGetSetItemHeightHandler()
*
* Sets/Gets the height associated with each item.  For non ownerdraw
* and fixed height ownerdraw, the item number is ignored.
*
* History:
\***************************************************************************/

int LBGetSetItemHeightHandler(
	PLBIV plb,
	UINT message,
	int item,
	UINT height)
{
	if (message == LB_GETITEMHEIGHT)
	{
		/*
		* All items are same height for non ownerdraw and for fixed height
		* ownerdraw.
		*/
		if (plb->OwnerDraw != OWNERDRAWVAR)
		{
			return plb->cyChar;
		}

		if (plb->cMac && item >= plb->cMac)
		{
			//RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
			return LB_ERR;
		}

		return (int)LBGetVariableHeightItemHeight(plb, (INT)item);
	}

	if (!height || height > 255)
	{
		//RIPERR1(ERROR_INVALID_PARAMETER,
		//	RIP_WARNING,
		//	"Invalid parameter \"height\" (%ld) to LBGetSetItemHeightHandler",
		//	height);

		return LB_ERR;
	}

	if (plb->OwnerDraw != OWNERDRAWVAR)
	{
		plb->cyChar = height;
	}
	else
	{
		if (item < 0 || item >= plb->cMac)
		{
			//RIPERR1(ERROR_INVALID_PARAMETER,
			//	RIP_WARNING,
			//	"Invalid parameter \"item\" (%ld) to LBGetSetItemHeightHandler",
			//	item);

			return LB_ERR;
		}

		LBSetVariableHeightItemHeight(plb, (INT)item, (INT)height);
	}

	//if (plb->fMultiColumn)
	//{
	//	LBCalcItemRowsAndColumns(plb);
	//}

	LBSetCItemFullMax(plb);

	return (0);
}

/*****************************************************************************\
*
* LBEvent()
*
* This is for item focus & selection events in listboxes.
*
\*****************************************************************************/
void LBEvent(PLBIV plb, UINT uEvent, int iItem)
{
	//UserAssert(FWINABLE());

	switch (uEvent)
	{
	case EVENT_OBJECT_SELECTIONREMOVE:
		if (plb->wMultiple != SINGLESEL)
		{
			break;
		}
		iItem = -1;
		//
		// FALL THRU
		//

	case EVENT_OBJECT_SELECTIONADD:
		if (plb->wMultiple == MULTIPLESEL)
		{
			uEvent = EVENT_OBJECT_SELECTION;
		}
		break;

	case EVENT_OBJECT_SELECTIONWITHIN:
		iItem = -1;
		break;
	}

	NotifyWinEvent(uEvent, HW(plb->spwnd), OBJID_CLIENT, iItem + 1);
}


//////////////////////////////////////////////////////////////////////////
// List Box variable height owner draw routines
//////////////////////////////////////////////////////////////////////////


#pragma hdrstop

/***************************************************************************\
* LBGetVariableHeightItemHeight
*
* Returns the height of the given item number. Assumes variable
* height owner draw.
*
* History:
\***************************************************************************/

INT LBGetVariableHeightItemHeight(
	PLBIV plb,
	INT itemNumber)
{
	BYTE itemHeight;
	int offsetHeight;

	if (plb->cMac)
	{
		if (plb->fHasStrings)
		{
			offsetHeight = plb->cMac * sizeof(LBItem);
		}
		else
		{
			offsetHeight = plb->cMac * sizeof(LBODItem);
		}

		if (plb->wMultiple)
		{
			offsetHeight += plb->cMac;
		}

		offsetHeight += itemNumber;

		itemHeight = *(plb->rgpch + (UINT)offsetHeight);

		return (INT)itemHeight;

	}

	/*
	*Default, we return the height of the system font.  This is so we can draw
	* the focus rect even though there are no items in the listbox.
	*/
	return 40;//gpsi->cySysFontChar;
}


/***************************************************************************\
* LBSetVariableHeightItemHeight
*
* Sets the height of the given item number. Assumes variable height
* owner draw, a valid item number and valid height.
*
*
* History:
\***************************************************************************/

void LBSetVariableHeightItemHeight(
	PLBIV plb,
	INT itemNumber,
	INT itemHeight)
{
	int offsetHeight;

	if (plb->fHasStrings)
	{
		offsetHeight = plb->cMac * sizeof(LBItem);
	}
	else
	{
		offsetHeight = plb->cMac * sizeof(LBODItem);
	}

	if (plb->wMultiple)
	{
		offsetHeight += plb->cMac;
	}

	offsetHeight += itemNumber;

	*(plb->rgpch + (UINT)offsetHeight) = (BYTE)itemHeight;

}


/***************************************************************************\
* CItemInWindowVarOwnerDraw
*
* Returns the number of items which can fit in a variable height OWNERDRAW
* list box. If fDirection, then we return the number of items which
* fit starting at sTop and going forward (for page down), otherwise, we are
* going backwards (for page up). (Assumes var height ownerdraw) If fPartial,
* then include the partially visible item at the bottom of the listbox.
*
* History:
\***************************************************************************/

INT CItemInWindowVarOwnerDraw(
	PLBIV plb,
	BOOL fPartial)
{
	RECT rect;
	INT sItem;
	INT clientbottom;

	GetClientRect(plb->spwnd, (LPRECT)&rect);
	clientbottom = rect.bottom;

	/*
	* Find the number of var height ownerdraw items which are visible starting
	* from plb->iTop.
	*/
	for (sItem = plb->iTop; sItem < plb->cMac; sItem++)
	{

		/*
		* Find out if the item is visible or not
		*/
		if (!LBGetItemRect(plb, sItem, (LPRECT)&rect))
		{

			/*
			* This is the first item which is completely invisible, so return
			* how many items are visible.
			*/
			return (sItem - plb->iTop);
		}

		if (!fPartial && rect.bottom > clientbottom)
		{

			/*
			* If we only want fully visible items, then if this item is
			* visible, we check if the bottom of the item is below the client
			* rect, so we return how many are fully visible.
			*/
			return (sItem - plb->iTop - 1);
		}
	}

	/*
	* All the items are visible
	*/
	return (plb->cMac - plb->iTop);
}


/***************************************************************************\
* LBPage
*
* For variable height ownerdraw listboxes, calaculates the new iTop we must
* move to when paging (page up/down) through variable height listboxes.
*
* History:
\***************************************************************************/

INT LBPage(
	PLBIV plb,
	INT startItem,
	BOOL fPageForwardDirection)
{
	INT     i;
	INT height;
	RECT    rc;

	if (plb->cMac == 1)
	{
		return (0);
	}

	GetClientRect(plb->spwnd, &rc);
	height = rc.bottom;
	i = startItem;

	if (fPageForwardDirection)
	{
		while ((height >= 0) && (i < plb->cMac))
		{
			height -= LBGetVariableHeightItemHeight(plb, i++);
		}

		return ((height >= 0) ? plb->cMac - 1 : max(i - 2, startItem + 1));
	}
	else
	{
		while ((height >= 0) && (i >= 0))
		{
			height -= LBGetVariableHeightItemHeight(plb, i--);
		}

		return ((height >= 0) ? 0 : min(i + 2, startItem - 1));
	}

}


/***************************************************************************\
* LBCalcVarITopScrollAmt
*
* Changing the top most item in the listbox from iTopOld to iTopNew we
* want to calculate the number of pixels to scroll so that we minimize the
* number of items we will redraw.
*
* History:
\***************************************************************************/

INT LBCalcVarITopScrollAmt(
	PLBIV plb,
	INT iTopOld,
	INT iTopNew)
{
	RECT rc;
	RECT rcClient;

	GetClientRect(plb->spwnd, (LPRECT)&rcClient);

	/*
	* Just optimize redrawing when move +/- 1 item.  We will redraw all items
	* if moving more than 1 item ahead or back.  This is good enough for now.
	*/
	if (iTopOld + 1 == iTopNew)
	{

		/*
		* We are scrolling the current iTop up off the top off the listbox so
		* return a negative number.
		*/
		LBGetItemRect(plb, iTopOld, (LPRECT)&rc);
		return (rcClient.top - rc.bottom);
	}

	if (iTopOld - 1 == iTopNew)
	{

		/*
		* We are scrolling the current iTop down and the previous item is
		* becoming the new iTop so return a positive number.
		*/
		LBGetItemRect(plb, iTopNew, (LPRECT)&rc);
		return -rc.top;
	}

	return rcClient.bottom - rcClient.top;
}

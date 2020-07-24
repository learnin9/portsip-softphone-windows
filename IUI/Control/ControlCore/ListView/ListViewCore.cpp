#include "stdafx.h"

#include <mlang.h>
#include <inetreg.h>
#include <WinGDI.h>
#include "../Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


//
//  Apps steal our userdata space so make sure we don't use it.
//
#undef GWLP_USERDATA
#undef GWL_USERDATA

// define this to get single click activate to activate immediately.
// if a second click comes to the same window within a double-click-timeout
// period, we blow it off. we try to keep focus on the app that launched,
// but we can't figure out how to do that yet... with this not defined,
// the single-click-activate waits a double-click-timeout before activating.
//
//#define ONECLICKHAPPENED

// REVIEW: max items in a OWNERDATA listview
// due to currently unknown reasons the listview will not handle much more
// items than this.  Since this number is very high, no time has yet been
// spent on finding the reason(s).
//
#define MAX_LISTVIEWITEMS (100000000)

#define CLIP_HEIGHT                ( (plv->cyLabelChar * 2) + 8)
#define CLIP_HEIGHT_DI             ( (plvdi->plv->cyLabelChar * 2) + 8)

// Timer IDs
#define IDT_NAMEEDIT    42
#define IDT_SCROLLWAIT  43
#define IDT_MARQUEE     44
#define IDT_ONECLICKOK  45
#define IDT_ONECLICKHAPPENED 46

#define  g_cxIconOffset ((IUIGetGloabelVar()->g_cxIconSpacing - IUIGetGloabelVar()->g_cxIcon) / 2)
#define  g_cyIconOffset (IUIGetGloabelVar()->g_cyBorder * 2)    // NOTE: Must be >= cyIconMargin!

#define DT_LV       (DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_EDITCONTROL)
#define DT_LVWRAP   (DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL)
#define CCHLABELMAX MAX_PATH  // BUGBUG dangerous???

#define ListView_DefProc  DefWindowProc

typedef struct _IMAGE IMAGE;

typedef struct _LISTITEM    // li
{
	LPTSTR pszText;

	// Icon View item中，图标Icon左上角绝对坐标
	// 图标默认在Item所在单元格中水平居中。
	POINT pt;

	short iImage;
	short cxSingleLabel;
	short cxMultiLabel;
	short cyFoldedLabel;
	short cyUnfoldedLabel;
	short iWorkArea;        // Which workarea do I belong

	WORD state;     // LVIS_*
	short iIndent;
	LPARAM lParam;
	HWLWND hBindWindowless;

	// Region list view stuff
	HRGN hrgnIcon;      // Region which describes the icon for this item
	POINT ptRgn;        // Location that this item's hrgnIcon was calculated for
	RECT rcTextRgn;

	// IUI Add
	ITEM_BACKGROUND *pItemSpecBackground;
} LISTITEM;

// Report view sub-item structure

struct LISTSUBITEM
{
	LPTSTR pszText;
	short iImage;
	WORD state;
	HWLWND hBindWindowless;
	ULONG_PTR m_dwSubItemData;
};


#define COLUMN_VIEW

#define LV_HDPA_GROW   16  // Grow chunk size for DPAs
#define LV_HIML_GROW   8   // Grow chunk size for ImageLists


#define LV_StateImageValue(pitem) ((int)(((DWORD)((pitem)->state) >> 12) & 0xF))
#define LV_StateImageIndex(pitem) (LV_StateImageValue(pitem) - 1)

// listview flag values
#define LVF_FOCUSED       0x0001
#define LVF_VISIBLE       0x0002
#define LVF_ERASE         0x0004 /* is hrgnInval to be erased? */
#define LVF_NMEDITPEND    0x0008
#define LVF_REDRAW        0x0010 /* Value from WM_SETREDRAW message */
#define LVF_ICONPOSSML    0x0020 /* X, Y coords are in small icon view */
#define LVF_INRECOMPUTE   0x0040 /* Check to make sure we are not recursing */
#define LVF_UNFOLDED      0x0080
#define LVF_FONTCREATED   0x0100 /* we created the SListViewData font */
#define LVF_SCROLLWAIT    0x0200 /* we're waiting to scroll */
#define LVF_COLSIZESET    0x0400 /* Has the caller explictly set width for list view */
#define LVF_USERBKCLR     0x0800 /* user set the bk color (don't follow syscolorchange) */
#define LVF_ICONSPACESET  0x1000 /* the user has set the icon spacing */
#define LVF_CUSTOMFONT    0x2000 /* there is at least one item with a custom font */

#if defined(FE_IME) || !defined(WINNT)
#define LVF_DONTDRAWCOMP  0x4000 /* do not draw IME composition if true */
#define LVF_INSERTINGCOMP 0x8000 /* Avoid recursion */
#endif
#define LVF_INRECALCREGION  0x00010000 /* prevents recursion in RecalcRegion */

#define ENTIRE_REGION   1

// listview DrawItem flags
#define LVDI_NOIMAGE            0x0001  // don't draw image
#define LVDI_TRANSTEXT          0x0002  // draw text transparently in black
#define LVDI_NOWAYFOCUS         0x0004  // don't allow focus to drawing
#define LVDI_FOCUS              0x0008  // focus is set (for drawing)
#define LVDI_SELECTED           0x0010  // draw selected text
#define LVDI_SELECTNOFOCUS      0x0020
#define LVDI_HOTSELECTED        0x0040
#define LVDI_UNFOLDED           0x0080  // draw the item umfolded (forced)

typedef struct
{
	SListViewData *plv;
	LPPOINT lpptOrg;
	LPRECT prcClip;
	UINT flags;

	LISTITEM *pitem;

	DWORD dwCustom;
	NMLVCUSTOMDRAW nmcd;
} LVDRAWITEM, *PLVDRAWITEM;

// listview child control ids
#define LVID_HEADER             0

// Instance data pointer access functions

#define ListView_GetPtr(hwnd)      (SListViewData*)GetWindowPtr(hwnd, 0)
#define ListView_SetPtr(hwnd, p)   (SListViewData*)SetWindowPtr(hwnd, 0, p)

// view type check functions

#define ListView_IsIconView(plv)    (((plv)->hwnd->ci.style & (UINT)LVS_TYPEMASK) == (UINT)LVS_ICON)
#define ListView_IsSmallView(plv)   (((plv)->hwnd->ci.style & (UINT)LVS_TYPEMASK) == (UINT)LVS_SMALLICON)
#define ListView_IsListView(plv)    (((plv)->hwnd->ci.style & (UINT)LVS_TYPEMASK) == (UINT)LVS_LIST)
#define ListView_IsReportView(plv)  (((plv)->hwnd->ci.style & (UINT)LVS_TYPEMASK) == (UINT)LVS_REPORT)

#define ListView_IsOwnerData( plv )     (plv->hwnd->ci.style & (UINT)LVS_OWNERDATA)
#define ListView_CheckBoxes(plv)        (plv->exStyle & LVS_EX_CHECKBOXES)
#define ListView_FullRowSelect(plv)     (plv->exStyle & LVS_EX_FULLROWSELECT)
#define ListView_IsInfoTip(plv)         (plv->exStyle & LVS_EX_INFOTIP)
#define ListView_OwnerDraw(plv)         (plv->hwnd->ci.style & LVS_OWNERDRAWFIXED)
#define ListView_IsLabelTip(plv)        (plv->exStyle & LVS_EX_LABELTIP)

// Some helper macros for checking some of the flags...
#define ListView_RedrawEnabled(plv) ((plv->flags & (LVF_REDRAW | LVF_VISIBLE)) == (LVF_REDRAW|LVF_VISIBLE))

// The hdpaZorder is acutally an array of DWORDS which contains the
// indexes of the items and not actual pointers...
// NOTE: linear search! this can be slow
#define ListView_ZOrderIndex(plv, i) IUI_DPA_GetPtrIndex((plv)->hdpaZOrder, (void *)i)

// Message handler functions (listview.c):

BOOL ListView_NotifyCacheHint(SListViewData *plv, int iFrom, int iTo);
void ListView_NotifyRecreate(SListViewData *plv);
BOOL ListView_OnCreate(SListViewData *plv, CREATESTRUCT *lpCreateStruct);
void ListView_OnNCDestroy(SListViewData *plv);
void ListView_OnPaint(SListViewData *plv, HDC hdc);
void ListView_OnCommand(SListViewData *plv, int id, HWND hwndCtl, UINT codeNotify);
void ListView_OnEnable(SListViewData *plv, BOOL fEnable);
BOOL ListView_OnWindowPosChanging(SListViewData *plv, WINDOWPOS *lpwpos);
void ListView_OnWindowPosChanged(SListViewData *plv, const WINDOWPOS *lpwpos);
void ListView_OnSetFocus(SListViewData *plv, HWLWND hwndOldFocus);
void ListView_OnKillFocus(SListViewData *plv, HWND hwndNewFocus);
void ListView_OnKey(SListViewData *plv, UINT vk, BOOL fDown, int cRepeat, UINT flags);
BOOL ListView_OnImeComposition(SListViewData *plv, WPARAM wParam, LPARAM lParam);
#ifndef UNICODE
BOOL SameDBCSChars(LPTSTR lpsz, WORD w);
#endif
void ListView_OnChar(SListViewData *plv, UINT ch, int cRepeat);
void ListView_OnButtonDown(SListViewData *plv, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void ListView_OnLButtonUp(SListViewData *plv, int x, int y, UINT keyFlags);
void ListView_OnCancelMode(SListViewData *plv);
void ListView_OnTimer(SListViewData *plv, UINT id);
void ListView_SetupPendingNameEdit(SListViewData *plv);
#define ListView_CancelPendingEdit(plv) ListView_CancelPendingTimer(plv, LVF_NMEDITPEND, IDT_NAMEEDIT)
#define ListView_CancelScrollWait(plv) ListView_CancelPendingTimer(plv, LVF_SCROLLWAIT, IDT_SCROLLWAIT)
BOOL ListView_CancelPendingTimer(SListViewData *plv, UINT fFlag, int idTimer);
void ListView_OnHScroll(SListViewData *plv, HWND hwndCtl, UINT code, int pos);
void ListView_OnVScroll(SListViewData *plv, HWND hwndCtl, UINT code, int pos);
BOOL ListView_CommonArrange(SListViewData *plv, UINT style, HDPA hdpaSort);
BOOL ListView_CommonArrangeEx(SListViewData *plv, UINT style, HDPA hdpaSort, int iWorkArea);
BOOL ListView_OnSetCursor(SListViewData *plv, HWND hwndCursor, UINT codeHitTest, UINT msg);
UINT ListView_OnGetDlgCode(SListViewData *plv, MSG *lpmsg);
HBRUSH ListView_OnCtlColor(SListViewData *plv, HDC hdc, HWND hwndChild, int type);
void ListView_OnSetFont(SListViewData *plvCtl, HFONT hfont, BOOL fRedraw);
HFONT ListView_OnGetFont(SListViewData *plv);
void ListViews_OnTimer(SListViewData *plv, UINT id);
void ListView_OnWinIniChange(SListViewData *plv, WPARAM wParam, LPARAM lParam);
void PASCAL ListView_OnSysColorChange(SListViewData *plv);
void ListView_OnSetRedraw(SListViewData *plv, BOOL fRedraw);
HIMAGELIST ListView_OnCreateDragImage(SListViewData *plv, int iItem, LPPOINT lpptUpLeft);
BOOL ListView_ISetColumnWidth(SListViewData *plv, int iCol, int cx, BOOL fExplicit);

typedef void(*SCROLLPROC)(SListViewData *, int dx, int dy, UINT uSmooth);
void ListView_ComOnScroll(SListViewData *plv, UINT code, int posNew, int sb,
	int cLine, int cPage);

#ifdef UNICODE
BOOL ListView_OnGetItemA(SListViewData *plv, LV_ITEMA *plvi);
BOOL ListView_OnSetItemA(SListViewData *plv, LV_ITEMA *plvi);
int ListView_OnInsertItemA(SListViewData *plv, LV_ITEMA *plvi);
int  ListView_OnFindItemA(SListViewData *plv, int iStart, LV_FINDINFOA *plvfi);
int ListView_OnGetStringWidthA(SListViewData *plv, LPCSTR psz, HDC hdc);
BOOL ListView_OnGetColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol);
BOOL ListView_OnSetColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol);
int ListView_OnInsertColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol);
int PASCAL ListView_OnGetItemTextA(SListViewData *plv, int i, LV_ITEMA *lvitem);
BOOL ListView_OnSetItemTextA(SListViewData *plv, int i, int iSubItem, LPCSTR pszText);
BOOL ListView_OnGetBkImageA(SListViewData *plv, LPLVBKIMAGEA pbiA);
BOOL ListView_OnSetBkImageA(SListViewData *plv, LPLVBKIMAGEA pbiA);
#endif

BOOL ListView_IsItemUnfolded2(SListViewData *plv, int iItem, int iSubItem, LPTSTR pszText, int cchTextMax);
BOOL ListView_OnSetBkImage(SListViewData *plv, LPLVBKIMAGE pbi);
BOOL ListView_OnGetBkImage(SListViewData *plv, LPLVBKIMAGE pbi);
BOOL ListView_OnSetBkColor(SListViewData *plv, COLORREF clrBk);
HIMAGELIST ListView_OnSetImageList(SListViewData *plv, HIMAGELIST himl, BOOL fSmallImages);
BOOL ListView_OnDeleteAllItems(SListViewData *plv);
int  ListView_OnInsertItem(SListViewData *plv, const LVITEMEX *plvi);
BOOL ListView_OnDeleteItem(SListViewData *plv, int i);
BOOL ListView_OnReplaceItem(SListViewData *plv, const LVITEMEX *plvi);
int  ListView_OnFindItem(SListViewData *plv, int iStart, const LV_FINDINFO *plvfi);
BOOL ListView_OnSetItemPosition(SListViewData *plv, int i, int x, int y);
BOOL ListView_OnSetItem(SListViewData *plv, const LVITEMEX *plvi);
BOOL ListView_OnGetItem(SListViewData *plv, LVITEMEX *plvi);
BOOL ListView_OnGetItemPosition(SListViewData *plv, int i, POINT *ppt);
BOOL ListView_OnEnsureVisible(SListViewData *plv, int i, BOOL fPartialOK);
BOOL ListView_OnScroll(SListViewData *plv, int dx, int dy);
int ListView_OnHitTest(SListViewData *plv, LV_HITTESTINFO *pinfo);
int ListView_OnGetStringWidth(SListViewData *plv, LPCTSTR psz, HDC hdc);
BOOL ListView_OnGetItemRect(SListViewData *plv, int i, RECT *prc);
int ListView_OnInsertItem(SListViewData *plv, const LVITEMEX *plvi);
BOOL ListView_OnRedrawItems(SListViewData *plv, int iFirst, int iLast);
int ListView_OnGetNextItem(SListViewData *plv, int i, UINT flags);
BOOL ListView_OnSetColumnWidth(SListViewData *plv, int iCol, int cx);
int ListView_OnGetColumnWidth(SListViewData *plv, int iCol);
void ListView_OnStyleChanging(SListViewData *plv, UINT gwl, LPSTYLESTRUCT pinfo);
void ListView_OnStyleChanged(SListViewData *plv, UINT gwl, LPSTYLESTRUCT pinfo);
int ListView_OnGetTopIndex(SListViewData *plv);
int ListView_OnGetCountPerPage(SListViewData *plv);
BOOL ListView_OnGetOrigin(SListViewData *plv, POINT *ppt);
int PASCAL ListView_OnGetItemText(SListViewData *plv, int i, LVITEMEX *lvitem);
BOOL ListView_OnSetItemText(SListViewData *plv, int i, int iSubItem, LPCTSTR pszText);
HIMAGELIST ListView_OnGetImageList(SListViewData *plv, int iImageList);

UINT PASCAL ListView_OnGetItemState(SListViewData *plv, int i, UINT mask);
BOOL PASCAL ListView_OnSetItemState(SListViewData *plv, int i, UINT data, UINT mask);

// Private functions (listview.c):

BOOL ListView_Notify(SListViewData *plv, int i, int iSubItem, int code);
void ListView_GetRects(SListViewData *plv, int i,
	RECT *prcIcon, RECT *prcLabel,
	RECT *prcBounds, RECT *prcSelectBounds);
BOOL ListView_DrawItem(PLVDRAWITEM);

#define ListView_InvalidateItem(p,i,s,r) ListView_InvalidateItemEx(p,i,0,s,r,0)
void ListView_InvalidateItemEx(SListViewData *plv, int i, int iSubItem, BOOL fSelectionOnly,
	UINT fRedraw, UINT maskChanged);

BOOL ListView_StartDrag(SListViewData *plv, int iDrag, int x, int y);
void ListView_TypeChange(SListViewData *plv, DWORD styleOld);
void PASCAL ListView_DeleteHrgnInval(SListViewData *plv);

void ListView_Redraw(SListViewData *plv, HDC hdc, RECT *prc);
void ListView_RedrawSelection(SListViewData *plv);
BOOL ListView_FreeItem(SListViewData *plv, LISTITEM *pitem);
void ListView_FreeSubItem(LISTSUBITEM *plsi);
LISTITEM *ListView_CreateItem(SListViewData *plv, const LVITEMEX *plvi);
void ListView_UpdateScrollBars(SListViewData *plv);

int ListView_SetFocusSel(SListViewData *plv, int iNewFocus, BOOL fSelect, BOOL fDeselectAll, BOOL fToggleSel);

void ListView_GetRectsOwnerData(SListViewData *plv, int iItem,
	RECT *prcIcon, RECT *prcLabel, RECT *prcBounds,
	RECT *prcSelectBounds, LISTITEM *pitem);

void ListView_CalcMinMaxIndex(SListViewData *plv, PRECT prcBounding, int *iMin, int *iMax);
int ListView_LCalcViewItem(SListViewData *plv, int x, int y);
void LVSeeThruScroll(SListViewData *plv, LPRECT lprcUpdate);

BOOL ListView_UnfoldRects(SListViewData *plv, int iItem,
	RECT *prcIcon, RECT *prcLabel,
	RECT *prcBounds, RECT *prcSelectBounds);

__inline int ListView_Count(SListViewData *plv)
{
	_ASSERT(ListView_IsOwnerData(plv) || plv->cTotalItems == DPA_GetPtrCount(plv->hdpa));
	return plv->cTotalItems;
}

// Forcing (i) to UINT lets us catch bogus negative numbers, too.
#define ListView_IsValidItemNumber(plv, i) ((UINT)(i) < (UINT)ListView_Count(plv))


#define ListView_GetItemPtr(plv, i)         ((LISTITEM *)IUI_DPA_GetPtr((plv)->hdpa, (i)))

#ifdef DEBUG
#define ListView_FastGetItemPtr(plv, i)     ((LISTITEM *)IUI_DPA_GetPtr((plv)->hdpa, (i)))
#define ListView_FastGetZItemPtr(plv, i)    ((LISTITEM *)IUI_DPA_GetPtr((plv)->hdpa, \
		(int)OFFSETOF(IUI_DPA_GetPtr((plv)->hdpaZOrder, (i)))))

#else
#define ListView_FastGetItemPtr(plv, i)     ((LISTITEM *)DPA_FastGetPtr((plv)->hdpa, (i)))
#define ListView_FastGetZItemPtr(plv, i)    ((LISTITEM *)DPA_FastGetPtr((plv)->hdpa, \
		(int)OFFSETOF(DPA_FastGetPtr((plv)->hdpaZOrder, (i)))))

#endif

BOOL ListView_CalcMetrics();
void PASCAL ListView_ColorChange();
void ListView_DrawBackground(SListViewData *plv, HDC hdc, RECT *prcClip);

BOOL ListView_NeedsEllipses(HDC hdc, LPCTSTR pszText, RECT *prc, int *pcchDraw, int cxEllipses);
int ListView_CompareString(SListViewData *plv, int i, LPCTSTR pszFind, UINT flags, int iLen);
int ListView_GetLinkedTextWidth(HDC hdc, LPCTSTR psz, UINT cch, BOOL bLink);

int ListView_GetCxScrollbar(SListViewData *plv);
int ListView_GetCyScrollbar(SListViewData *plv);
DWORD ListView_GetWindowStyle(SListViewData *plv);
int ListView_GetScrollInfo(SListViewData *plv, int nBar, SCROLLINFO *psi);
int ListView_SetScrollInfo(SListViewData *plv, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw);
int ListView_SetScrollRange(SListViewData *plv, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw);

// lvicon.c functions

BOOL ListView_OnArrange(SListViewData *plv, UINT style);
HWND ListView_OnEditLabel(SListViewData *plv, int i, LPTSTR pszText);

int ListView_IItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem);
void ListView_IconViewGetRects(SListViewData *plv, LISTITEM *pitem, RECT *prcIcon,
	RECT *prcLabel, LPRECT prcBounds);
void ListView_ScaleIconPositions(SListViewData *plv, BOOL fSmallIconView);
void ListView_IGetRectsOwnerData(SListViewData *plv, int iItem, RECT *prcIcon,
	RECT *prcLabel, LISTITEM *pitem, BOOL fUsepitem);
void PASCAL _ListView_GetRectsFromItem(SListViewData *plv, BOOL bSmallIconView,
	LISTITEM *pitem,
	LPRECT prcIcon, LPRECT prcLabel, LPRECT prcBounds, LPRECT prcSelectBounds);

__inline void ListView_SetSRecompute(LISTITEM *pitem)
{
	pitem->cxSingleLabel = SRECOMPUTE;
	pitem->cxMultiLabel = SRECOMPUTE;
	pitem->cyFoldedLabel = SRECOMPUTE;
	pitem->cyUnfoldedLabel = SRECOMPUTE;
}

void ListView_Recompute(SListViewData *plv);

void ListView_RecomputeLabelSize(SListViewData *plv, LISTITEM *pitem, int i, HDC hdc, BOOL fUsepitem);

BOOL ListView_SetIconPos(SListViewData *plv, LISTITEM *pitem, int iSlot, int cSlot);
BOOL ListView_IsCleanRect(SListViewData *plv, RECT *prc, int iExcept, BOOL *pfUpdate, HDC hdc);
int ListView_FindFreeSlot(SListViewData *plv, int i, int iSlot, int cSlot, BOOL *pfUpdateSB, BOOL *pfAppend, HDC hdc);
int ListView_CalcHitSlot(SListViewData *plv, POINT pt, int cslot);

void ListView_GetViewRect2(SListViewData *plv, RECT *prcView, int cx, int cy);
int CALLBACK ArrangeIconCompare(LISTITEM *pitem1, LISTITEM *pitem2, LPARAM lParam);
int ListView_GetSlotCountEx(SListViewData *plv, BOOL fWithoutScroll, int iWorkArea);
int ListView_GetSlotCount(SListViewData *plv, BOOL fWithoutScroll);
void ListView_IUpdateScrollBars(SListViewData *plv);
DWORD ListView_GetClientRect(SListViewData *plv, RECT *prcClient, BOOL fSubScrolls, RECT *prcViewRect);

void ListView_SetEditSize(SListViewData *plv);
BOOL ListView_DismissEdit(SListViewData *plv, BOOL fCancel);
LRESULT CALLBACK ListView_EditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


UINT PASCAL ListView_DrawImageEx(SListViewData *plv, LVITEMEX *pitem, HDC hdc, int x, int y, UINT fDraw, int xMax);
#define ListView_DrawImage(plv, pitem, hdc, x, y, fDraw) \
	ListView_DrawImageEx(plv, pitem, hdc, x, y, fDraw, -1)

#if defined(FE_IME) || !defined(WINNT)
void PASCAL ListView_SizeIME(HWND hwnd);
void PASCAL ListView_InsertComposition(HWND hwnd, WPARAM wParam, LPARAM lParam, SListViewData *plv);
void PASCAL ListView_PaintComposition(HWND hwnd, SListViewData *plv);
#endif

// lvsmall.c functions:


void ListView_SGetRects(SListViewData *plv, LISTITEM *pitem, RECT *prcIcon,
	RECT *prcLabel, LPRECT prcBounds);
void ListView_SGetRectsOwnerData(SListViewData *plv, int iItem, RECT *prcIcon,
	RECT *prcLabel, LISTITEM *pitem, BOOL fUsepitem);
int ListView_SItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem);

int ListView_LookupString(SListViewData *plv, LPCTSTR lpszLookup, UINT flags, int iStart);

// lvlist.c functions:


void ListView_LGetRects(SListViewData *plv, int i, RECT *prcIcon,
	RECT *prcLabel, RECT *prcBounds, RECT *prcSelectBounds);
int ListView_LItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem);
void ListView_LUpdateScrollBars(SListViewData *plv);
BOOL ListView_MaybeResizeListColumns(SListViewData *plv, int iFirst, int iLast);

// lvrept.c functions:

int ListView_OnSubItemHitTest(SListViewData *plv, LPLVHITTESTINFO lParam);
void ListView_GetSubItem(SListViewData *plv, int i, int iSubItem, LISTSUBITEM *plsi);
BOOL LV_ShouldItemDrawGray(SListViewData *plv, UINT fText);
int ListView_OnInsertColumn(SListViewData *plv, int iCol, const LV_COLUMN *pcol);
BOOL ListView_OnDeleteColumn(SListViewData *plv, int iCol);
BOOL ListView_OnGetColumn(SListViewData *plv, int iCol, LV_COLUMN *pcol);
BOOL ListView_OnSetColumn(SListViewData *plv, int iCol, const LV_COLUMN *pcol);
BOOL ListView_ROnEnsureVisible(SListViewData *plv, int i, BOOL fPartialOK);
BOOL ListView_OnGetSubItemRect(SListViewData *plv, int i, LPRECT lprc);
#define ListView_RYHitTest(plv, cy)  ((int)(((cy) + plv->ptlRptOrigin.y - plv->yTop) / plv->cyItem))

BOOL ListView_SetSubItem(SListViewData *plv, const LVITEMEX *plvi);
void PASCAL ListView_RAfterRedraw(SListViewData *plv, HDC hdc);

int ListView_RGetColumnWidth(SListViewData *plv, int iCol);
BOOL ListView_RSetColumnWidth(SListViewData *plv, int iCol, int cx);
LPTSTR ListView_GetSubItemText(SListViewData *plv, int i, int iCol);

void ListView_RDestroy(SListViewData *plv);
LPTSTR ListView_RGetItemText(SListViewData *plv, int i, int iCol);
int ListView_RItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem);
void ListView_RUpdateScrollBars(SListViewData *plv);
void ListView_RGetRects(SListViewData *plv, int iItem, RECT *prcIcon,
	RECT *prcLabel, RECT *prcBounds, RECT *prcSelectBounds);

LRESULT ListView_HeaderNotify(SListViewData *plv, HD_NOTIFY *pnm);
int WINAPI ListView_FreeColumnData(LPVOID d, LPVOID p);

#define ListView_GetSubItemDPA(plv, idpa) \
	((HDPA)IUI_DPA_GetPtr((plv)->hdpaSubItems, (idpa)))

int  ListView_Arrow(SListViewData *plv, int iStart, UINT vk);

BOOL ListView_IsItemUnfolded(SListViewData *plv, int item);
BOOL ListView_IsItemUnfoldedPtr(SListViewData *plv, LISTITEM *pitem);

// Fake customdraw.  See comment block in lvrept.c

typedef struct LVFAKEDRAW
{
	NMLVCUSTOMDRAW nmcd;
	SListViewData *plv;
	DWORD dwCustomPrev;
	DWORD dwCustomItem;
	DWORD dwCustomSubItem;
	LVITEMEX *pitem;
	HFONT hfontPrev;
} LVFAKEDRAW, *PLVFAKEDRAW;

void ListView_BeginFakeCustomDraw(SListViewData *plv, PLVFAKEDRAW plvfd, LVITEMEX *pitem);
DWORD ListView_BeginFakeItemDraw(PLVFAKEDRAW plvfd);
void ListView_EndFakeItemDraw(PLVFAKEDRAW plvfd);
void ListView_EndFakeCustomDraw(PLVFAKEDRAW plvfd);

//============ External declarations =======================================

//extern HFONT g_hfontLabel;
extern HBRUSH g_hbrActiveLabel;
extern HBRUSH g_hbrInactiveLabel;
extern HBRUSH g_hbrBackground;


// function tables
#define LV_TYPEINDEX(plv) ((plv)->hwnd->ci.style & (UINT)LVS_TYPEMASK)

BOOL ListView_RDrawItem(PLVDRAWITEM);
BOOL ListView_IconViewDrawItem(PLVDRAWITEM);
BOOL ListView_LDrawItem(PLVDRAWITEM);

typedef BOOL(*PFNLISTVIEW_DRAWITEM)(PLVDRAWITEM);
extern const PFNLISTVIEW_DRAWITEM pfnListView_DrawItem[4];
#define _ListView_DrawItem(plvdi) \
	pfnListView_DrawItem[LV_TYPEINDEX(plvdi->plv)](plvdi)


void ListView_RUpdateScrollBars(SListViewData *plv);

typedef void(*PFNLISTVIEW_UPDATESCROLLBARS)(SListViewData *plv);
extern const PFNLISTVIEW_UPDATESCROLLBARS pfnListView_UpdateScrollBars[4];
#define _ListView_UpdateScrollBars(plv) \
	pfnListView_UpdateScrollBars[LV_TYPEINDEX(plv)](plv)


typedef DWORD(*PFNLISTVIEW_APPROXIMATEVIEWRECT)(SListViewData *plv, int, int, int);
extern const PFNLISTVIEW_APPROXIMATEVIEWRECT pfnListView_ApproximateViewRect[4];
#define _ListView_ApproximateViewRect(plv, iCount, iWidth, iHeight) \
	pfnListView_ApproximateViewRect[LV_TYPEINDEX(plv)](plv, iCount, iWidth, iHeight)


typedef int(*PFNLISTVIEW_ITEMHITTEST)(SListViewData *plv, int, int, UINT *, int *);
extern const PFNLISTVIEW_ITEMHITTEST pfnListView_ItemHitTest[4];
#define _ListView_ItemHitTest(plv, x, y, pflags, piSubItem) \
	pfnListView_ItemHitTest[LV_TYPEINDEX(plv)](plv, x, y, pflags, piSubItem)



void ListView_IOnScroll(SListViewData *plv, UINT code, int posNew, UINT fVert);
void ListView_LOnScroll(SListViewData *plv, UINT code, int posNew, UINT sb);
void ListView_ROnScroll(SListViewData *plv, UINT code, int posNew, UINT sb);

typedef void(*PFNLISTVIEW_ONSCROLL)(SListViewData *plv, UINT, int, UINT);
extern const PFNLISTVIEW_ONSCROLL pfnListView_OnScroll[4];
#define _ListView_OnScroll(plv, x, y, pflags) \
	pfnListView_OnScroll[LV_TYPEINDEX(plv)](plv, x, y, pflags)


void ListView_Scroll2(SListViewData *plv, int dx, int dy);
void ListView_IScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth);
void ListView_LScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth);
void ListView_RScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth);

typedef void(*PFNLISTVIEW_SCROLL2)(SListViewData *plv, int, int, UINT);
extern const PFNLISTVIEW_SCROLL2 pfnListView_Scroll2[4];
#define _ListView_Scroll2(plv, x, y, pflags) \
	pfnListView_Scroll2[LV_TYPEINDEX(plv)](plv, x, y, pflags)

int ListView_IGetScrollUnitsPerLine(SListViewData *plv, UINT sb);
int ListView_LGetScrollUnitsPerLine(SListViewData *plv, UINT sb);
int ListView_RGetScrollUnitsPerLine(SListViewData *plv, UINT sb);

typedef int(*PFNLISTVIEW_GETSCROLLUNITSPERLINE)(SListViewData *plv, UINT sb);
extern const PFNLISTVIEW_GETSCROLLUNITSPERLINE pfnListView_GetScrollUnitsPerLine[4];
#define _ListView_GetScrollUnitsPerLine(plv, sb) \
	pfnListView_GetScrollUnitsPerLine[LV_TYPEINDEX(plv)](plv, sb)


#define LVMI_PLACEITEMS (WM_USER)

#define IE_SETTINGS          TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced")
#define USE_DBL_CLICK_TIMER  TEXT("UseDoubleClickTimer")

int g_bUseDblClickTimer;

#define LVMP_WINDOWPOSCHANGED (WM_USER + 1)

#define COLORISLIGHT(clr) ((5*GetGValue((clr)) + 2*GetRValue((clr)) + GetBValue((clr))) > 8*128)

void ListView_HandleMouse(SListViewData *plv, BOOL fDoubleClick, int x, int y, UINT keyFlags, BOOL bMouseWheel);

/// function table setup
const PFNLISTVIEW_DRAWITEM pfnListView_DrawItem[4] =
{
	ListView_IconViewDrawItem,
	ListView_RDrawItem,
	ListView_IconViewDrawItem,
	ListView_LDrawItem,
};

void ListView_HandleStateIconClick(SListViewData *plv, int iItem);

DWORD ListView_IApproximateViewRect(SListViewData *, int, int, int);
DWORD ListView_RApproximateViewRect(SListViewData *, int, int, int);
DWORD ListView_LApproximateViewRect(SListViewData *, int, int, int);

const PFNLISTVIEW_APPROXIMATEVIEWRECT pfnListView_ApproximateViewRect[4] =
{
	ListView_IApproximateViewRect,
	ListView_RApproximateViewRect,
	ListView_IApproximateViewRect,
	ListView_LApproximateViewRect,
};

const PFNLISTVIEW_UPDATESCROLLBARS pfnListView_UpdateScrollBars[4] =
{
	ListView_IUpdateScrollBars,
	ListView_RUpdateScrollBars,
	ListView_IUpdateScrollBars,
	ListView_LUpdateScrollBars,
};

const PFNLISTVIEW_ITEMHITTEST pfnListView_ItemHitTest[4] =
{
	ListView_IItemHitTest,
	ListView_RItemHitTest,
	ListView_SItemHitTest,
	ListView_LItemHitTest,
};

const PFNLISTVIEW_ONSCROLL pfnListView_OnScroll[4] =
{
	ListView_IOnScroll,
	ListView_ROnScroll,
	ListView_IOnScroll,
	ListView_LOnScroll,
};

const PFNLISTVIEW_SCROLL2 pfnListView_Scroll2[4] =
{
	ListView_IScroll2,
	ListView_RScroll2,
	ListView_IScroll2,
	ListView_LScroll2,
};

const PFNLISTVIEW_GETSCROLLUNITSPERLINE pfnListView_GetScrollUnitsPerLine[4] =
{
	ListView_IGetScrollUnitsPerLine,
	ListView_RGetScrollUnitsPerLine,
	ListView_IGetScrollUnitsPerLine,
	ListView_LGetScrollUnitsPerLine,
};


// redefine to trace at most calls to ListView_SendChange
#define DM_LVSENDCHANGE 0


// BUGBUG -- penwin.h is screwy; define local stuff for now
#define HN_BEGINDIALOG        40    // Lens/EditText/garbage detection dialog is about
// to come up on this hedit/bedit
#define HN_ENDDIALOG          41    // Lens/EditText/garbage detection dialog has
// just been destroyed

//---------------------------------------------------------
// no way am I gonna make TWO function calls where I can do FOUR comparisons!
//
#define RECTS_IN_SIZE( sz, r2 ) (!RECTS_NOT_IN_SIZE( sz, r2 ))

#define RECTS_NOT_IN_SIZE( sz, r2 ) (\
	( (sz).cx <= (r2).left ) ||\
	( 0 >= (r2).right ) ||\
	( (sz).cy <= (r2).top ) ||\
	( 0 >= (r2).bottom ) )

//---------------------------------------------------------


void ListView_OnUpdate(SListViewData *plv, int i);
void ListView_OnDestroy(SListViewData *plv);
BOOL PASCAL ListView_ValidateScrollParams(SListViewData *plv, int *dx, int *dy);
UINT LV_IsItemOnViewEdge(SListViewData *plv, LISTITEM *pitem);
void PASCAL ListView_ButtonSelect(SListViewData *plv, int iItem, UINT keyFlags, BOOL bSelected);
void ListView_DeselectAll(SListViewData *plv, int iDontDeselect);
void ListView_LRInvalidateBelow(SListViewData *plv, int i, int fSmoothScroll);
void ListView_IInvalidateBelow(SListViewData *plv, int i);
void ListView_InvalidateFoldedItem(SListViewData *plv, int iItem, BOOL fSelectionOnly, UINT fRedraw);
void ListView_ReleaseBkImage(SListViewData *plv);
void ListView_RecalcRegion(SListViewData *plv, BOOL fForce, BOOL fRedraw);

BOOL g_fSlowMachine = -1;

BOOL ListView_GetRegIASetting(BOOL *pb)
{
	HKEY        hkey;
	BOOL        bRet = FALSE;
	BOOL        bValue = TRUE;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, IE_SETTINGS, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwValue;
		DWORD cbValue = sizeof(DWORD);

		if (RegQueryValueEx(hkey, (LPTSTR)USE_DBL_CLICK_TIMER, 0, &dwType, (LPBYTE)&dwValue, &cbValue) == ERROR_SUCCESS)
		{
			bValue = (BOOL)dwValue;
			bRet = TRUE;
		}
		RegCloseKey(hkey);
	}

	*pb = bValue;
	return bRet;
}


BOOL ListView_NotifyCacheHint(SListViewData *plv, int iFrom, int iTo)
{
	NM_CACHEHINT nm;

	_ASSERT(iFrom <= iTo);
	if (iFrom <= iTo)
	{
		nm.iFrom = iFrom;
		nm.iTo = iTo;
		return !(BOOL)CCSendNotify(&plv->hwnd->ci, LVN_ODCACHEHINT, &nm.hdr);
	}
	return FALSE;
}

void ListView_LazyCreateObjects(SListViewData *plv, int iMin, int iMax)
{
	//L for (; iMin < iMax; iMin++)
	//{
	//	MyNotifyWinEvent(EVENT_OBJECT_CREATE, GetHostWnd(plv->hwnd), OBJID_CLIENT, 1 + iMin);
	//}
}

//
//  Owner-data causes MSAA lots of grief, because there is no way to tell
//  MSAA "I just created 25 million items".  You have to tell it one at a
//  time.  Instead of sending out 25 million "add item" notifications, we
//  just send them out as they scroll into view.
//
//  plv->iMSAAMin and plv->iMSAAMax are the range of items we most
//  recently told MSAA about.  MSAAMax is *exclusive*, just like RECTs.
//  It makes the math easier.
//
//  We use iMSAAMin and iMSAAMax to avoid sending blatantly redundant
//  notifications, which would other happen very frequently.
//
void ListView_LazyCreateWinEvents(SListViewData *plv, int iFrom, int iTo)
{
	int iMin = iFrom;
	int iMax = iTo + 1;         // Convert from [From,To] to [Min,Max)

#ifdef LVDEBUG
	DebugMsg(TF_LISTVIEW, TEXT("lv.CWE old: [%d,%d), new=[%d,%d)"),
		plv->iMSAAMin, plv->iMSAAMax,
		iMin, iMax);
#endif

	//
	//  If the incoming range is entirely contained within the existing
	//  range, then there is nothing to do.  This happens a lot.
	//
	if (iMin >= plv->iMSAAMin && iMax <= plv->iMSAAMax)
	{
		return;
	}

	//
	//  If the incoming range is adjacent to or overlaps the low end
	//  of the existing range...  (This happens when scrolling backwards.)
	//
	if (iMin <= plv->iMSAAMin && iMax >= plv->iMSAAMin)
	{

		// Notify the low end.
		ListView_LazyCreateObjects(plv, iMin, plv->iMSAAMin);

		// Extend the list of things we've notified.
		plv->iMSAAMin = iMin;

		// Remove it from the things left to be notified.
		iMin = plv->iMSAAMax;
	}

	//
	//  Now do the same thing to the top end.
	//  (This happens when scrolling forwards.)
	//
	if (iMax >= plv->iMSAAMax && iMin <= plv->iMSAAMax)
	{

		// Notify the top end.
		ListView_LazyCreateObjects(plv, plv->iMSAAMax, iMax);

		// Extend the list of things we've notified.
		plv->iMSAAMax = iMax;

		// Remove it from the things left to be notified.
		iMax = plv->iMSAAMin;
	}

	//
	//  If there are still things to be notified, then it means that the
	//  incoming range isn't contiguous with the previous range, so throw
	//  away the old range and just set it to the current range.
	//  (This happens when you grab the scrollbar and jump to a completely
	//  unrelated part of the listview.)
	//
	if (iMin < iMax)
	{
		plv->iMSAAMin = iMin;
		plv->iMSAAMax = iMax;
		ListView_LazyCreateObjects(plv, iMin, iMax);
	}

#ifdef LVDEBUG
	DebugMsg(TF_LISTVIEW, TEXT("lv.CWE aft: [%d,%d)"), plv->iMSAAMin, plv->iMSAAMax);
#endif

}

LRESULT ListView_RequestFindItem(SListViewData *plv, CONST LV_FINDINFO *plvfi, int iStart)
{
	NM_FINDITEM nm;

	nm.lvfi = *plvfi;
	nm.iStart = iStart;
	return CCSendNotify(&plv->hwnd->ci, LVN_ODFINDITEM, &nm.hdr);
}

BOOL ListView_SendChange(SListViewData *plv, int i, int iSubItem, int code, UINT oldState, UINT newState,
	UINT changed, LPARAM lParam)
{
	NM_LISTVIEW nm;

	nm.iItem = i;
	nm.iSubItem = iSubItem;
	nm.uNewState = newState;
	nm.uOldState = oldState;
	nm.uChanged = changed;
	nm.ptAction.x = 0;
	nm.ptAction.y = 0;
	nm.lParam = lParam;

	return !CCSendNotify(&plv->hwnd->ci, code, &nm.hdr);
}

void ListView_SendODChangeAndInvalidate(SListViewData *plv, int iFrom, int iTo, UINT oldState,
	UINT newState)
{
	NM_ODSTATECHANGE nm;

	nm.iFrom = iFrom;
	nm.iTo = iTo;
	nm.uNewState = newState;
	nm.uOldState = oldState;

	CCSendNotify(&plv->hwnd->ci, LVN_ODSTATECHANGED, &nm.hdr);

	// Tell accessibility, "Selection changed in a complex way"
	MyNotifyWinEvent(EVENT_OBJECT_SELECTIONWITHIN, GetHostWnd(plv->hwnd), OBJID_CLIENT, CHILDID_SELF);

	// considerable speed increase less than 100 to do this method
	// while over 100, the other method works faster
	if ((iTo - iFrom) > 100)
	{
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, FALSE);
	}
	else
	{
		while (iFrom <= iTo)
		{
			ListView_InvalidateItem(plv, iFrom, TRUE, RDW_INVALIDATE);
			iFrom++;
		}
	}
}

BOOL ListView_Notify(SListViewData *plv, int i, int iSubItem, int code)
{
	NM_LISTVIEW nm;
	nm.iItem = i;
	nm.iSubItem = iSubItem;
	nm.uNewState = nm.uOldState = 0;
	nm.uChanged = 0;
	nm.lParam = 0;

	if (!ListView_IsOwnerData(plv))
	{
		if (code == LVN_DELETEITEM)
		{
			LISTITEM *pItem = ListView_GetItemPtr(plv, i);
			if (pItem)
			{
				nm.lParam = pItem->lParam;
			}
		}
	}

	return (BOOL)CCSendNotify(&plv->hwnd->ci, code, &nm.hdr);
}

BOOL _ListView_GetEmptyText(SListViewData *plv)
{
	NMLVDISPINFO nm;
	BOOL ret;
	TCHAR szText[80];

	if (plv->fNoEmptyText)
	{
		return FALSE;
	}

	if (plv->pszEmptyText)
	{
		return TRUE;
	}

	// For each listview control, we will only send this notify
	// once if necessary.

	memset(&nm, 0, sizeof(NMLVDISPINFO));
	nm.item.mask = LVIF_TEXT;
	nm.item.cchTextMax = ARRAYSIZE(szText);
	nm.item.pszText = szText;
	szText[0] = TEXT('\0');

	ret = (BOOL)CCSendNotify(&plv->hwnd->ci, LVN_GETEMPTYTEXT, &nm.hdr);

	if (ret)
		// save the text so we don't notify again.
	{
		Str_Set(&plv->pszEmptyText, szText);
	}
	else
		// set a flag so we don't notify again.
	{
		plv->fNoEmptyText = TRUE;
	}

	return ret;
}

void ListView_NotifyFocusEvent(SListViewData *plv)
{
	if (plv->iFocus != -1
		&& IsWindowVisible(GetHostWnd(plv->hwnd))
		&& GetFocus() == GetHostWnd(plv->hwnd))
	{
		MyNotifyWinEvent(EVENT_OBJECT_FOCUS, GetHostWnd(plv->hwnd), OBJID_CLIENT,
			plv->iFocus + 1);
	}
}

//
//  Call this function when the listview has changed in a radical manner.
//  It notifies MSAA that "Whoa, things are completely different now."
//
void ListView_NotifyRecreate(SListViewData *plv)
{
	MyNotifyWinEvent(EVENT_OBJECT_DESTROY, GetHostWnd(plv->hwnd), OBJID_CLIENT, CHILDID_SELF);
	MyNotifyWinEvent(EVENT_OBJECT_CREATE, GetHostWnd(plv->hwnd), OBJID_CLIENT, CHILDID_SELF);
	plv->iMSAAMin = plv->iMSAAMax = 0;
}

int ListView_OnSetItemCount(SListViewData *plv, int iItems, DWORD dwFlags)
{
	BOOL frt = TRUE;

	// For compatability we assume 0 for flags implies old (Athena) type of functionality and
	// does a Invalidate all otherwise if low bit is set we try to be a bit smarter.  First pass
	// If the first added item is visible invalidate all.  Yes we can do better...
	if (ListView_IsOwnerData(plv))
	{
		int iItem;
		int cTotalItemsOld = plv->cTotalItems;
		BOOL fInvalidateAll = ((dwFlags & LVSICF_NOINVALIDATEALL) == 0);

		if ((iItems >= 0) && (iItems <= MAX_LISTVIEWITEMS))
		{

			plv->cTotalItems = iItems;

			// check focus
			if (plv->iFocus >= iItems)
			{
				plv->iFocus = -1;
			}
			if (plv->iDropHilite >= iItems)
			{
				plv->iDropHilite = -1;
			}

			// check mark
			if (plv->iMark >= iItems)
			{
				plv->iMark = -1;
			}

			// make sure no selections above number of items
			//L plv->plvrangeCut->lpVtbl->ExcludeRange(plv->plvrangeCut, iItems, SELRANGE_MAXVALUE);
			//if (FAILED(plv->plvrangeSel->lpVtbl->ExcludeRange(plv->plvrangeSel, iItems, SELRANGE_MAXVALUE)))
			//{
			//	//BUGBUG:  Return low memory status
			//	//MemoryLowDlg( GetHostWnd(plv->hwnd) );
			//	return FALSE;
			//}


			plv->rcView.left = RECOMPUTE;  // recompute view rect

			if (ListView_IsSmallView(plv) || ListView_IsIconView(plv))
			{
				// Call off to the arrange function.
				ListView_OnArrange(plv, LVA_DEFAULT);

				if (!fInvalidateAll)
				{
					// Try to be smart and invalidate only what we need to.
					// Add a little logic to erase any message like no items found when
					// the view was previously empty...
					if (cTotalItemsOld < iItems)
					{
						iItem = cTotalItemsOld;
					}
					else
					{
						iItem = iItems - 1;    // Get the index
					}

					if ((iItem >= 0) && (cTotalItemsOld > 0))
					{
						ListView_IInvalidateBelow(plv, iItem);
					}
					else
					{
						fInvalidateAll = TRUE;
					}
				}

			}
			else
			{
				ListView_Recompute(plv);
				// if we have empty text and old count was zero... then we should redraw all
				if (plv->pszEmptyText && (cTotalItemsOld == 0) && (iItems > 0))
				{
					fInvalidateAll = TRUE;
				}

				// Try to do smart invalidates...
				if (!fInvalidateAll)
				{
					// Try to be smart and invalidate only what we need to.
					if (cTotalItemsOld < iItems)
					{
						iItem = cTotalItemsOld;
					}
					else
					{
						iItem = iItems - 1;    // Get the index
					}

					if (iItem >= 0)
					{
						ListView_LRInvalidateBelow(plv, iItem, FALSE);
					}
				}


				// We may try to resize the column
				ListView_MaybeResizeListColumns(plv, 0, ListView_Count(plv) - 1);

				// For compatability we assume 0 for flags implies old type
				// of functionality and scrolls the important item into view.
				// If second bit is set, we leave the scroll position alone.
				if ((dwFlags & LVSICF_NOSCROLL) == 0)
				{
					// what is the important item
					iItem = (plv->iFocus >= 0) ?
						plv->iFocus :
						ListView_OnGetNextItem(plv, -1, LVNI_SELECTED);

					iItem = max(0, iItem);

					// make important item visable
					ListView_OnEnsureVisible(plv, iItem, FALSE);
				}
			}


			if (fInvalidateAll)
			{
				InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
			}
			ListView_UpdateScrollBars(plv);

			ListView_NotifyRecreate(plv);
			ListView_NotifyFocusEvent(plv);

		}
		else
		{
			frt = FALSE;
		}

	}
	else
	{
		if (plv->hdpaSubItems)
		{
			int iCol;
			for (iCol = plv->cCol - 1; iCol >= 0; iCol--)
			{
				HDPA hdpa = ListView_GetSubItemDPA(plv, iCol);
				if (hdpa)   // this is optional, call backs don't have them
				{
					IUI_DPA_Grow(hdpa, iItems);
				}
			}
		}

		IUI_DPA_Grow(plv->hdpa, iItems);
		IUI_DPA_Grow(plv->hdpaZOrder, iItems);
	}

	return frt;
}

typedef struct _LVSortInfo
{
	SListViewData     *plv;
	BOOL fSortIndices;
	PFNLVCOMPARE        pfnCompare;
	LPARAM              lParam;
	BOOL                bPassLP;
} LVSortInfo;

int CALLBACK ListView_SortCallback(LPVOID dw1, LPVOID dw2, LPARAM lParam)
{
	LISTITEM *pitem1;
	LISTITEM *pitem2;
	LVSortInfo *pSortInfo = (LVSortInfo *)lParam;

	_ASSERT(!ListView_IsOwnerData(pSortInfo->plv));

	// determine whether  dw1 and dw2 are indices or the real items
	// and assign pitem? accordingly
	if (pSortInfo->fSortIndices)
	{
		pitem1 = ListView_GetItemPtr(pSortInfo->plv, PtrToUlong(dw1));
		pitem2 = ListView_GetItemPtr(pSortInfo->plv, PtrToUlong(dw2));
	}
	else
	{
		pitem1 = (LISTITEM *)dw1;
		pitem2 = (LISTITEM *)dw2;
	}

	if (!pSortInfo->pfnCompare)
	{
		// Treat NULL pszText like null string.
		LPCTSTR pszText1 = pitem1->pszText ? pitem1->pszText : TEXT("");
		LPCTSTR pszText2 = pitem2->pszText ? pitem2->pszText : TEXT("");

		// bugbug, should allow callbacks in text
		if (pszText1 != LPSTR_TEXTCALLBACK &&
			pszText2 != LPSTR_TEXTCALLBACK)
		{
			return lstrcmpi(pitem1->pszText, pitem2->pszText);
		}
		RIPMSG(0, "LVM_SORTITEM(EX): Cannot combine NULL callback with LPSTR_TEXTCALLBACK");
		return -1;
	}
	else
	{
		if (pSortInfo->bPassLP)
		{
			return (pSortInfo->pfnCompare(pitem1->lParam, pitem2->lParam, pSortInfo->lParam));
		}
		else
		{
			if (pSortInfo->fSortIndices)
			{
				return (pSortInfo->pfnCompare((LPARAM)dw1, (LPARAM)dw2, pSortInfo->lParam));
			}
			else
			{
				// we want to sort by the indices, but all we've got are pointers to the items
				// and there is no way to get back from that pointer to an index
				_ASSERT(0);
				return -1;
			}
		}

	}
	_ASSERT(0);
	return -1;
}

VOID ListView_InvalidateTTLastHit(SListViewData *plv, int iNewHit)
{
	if (plv->iTTLastHit == iNewHit)
	{
		plv->iTTLastHit = -1;
		if (plv->pszTip && plv->pszTip != LPSTR_TEXTCALLBACK)
		{
			plv->pszTip[0] = 0;
		}
	}
}

BOOL PASCAL ListView_SortAllColumns(SListViewData *plv, LVSortInfo *psi)
{
	_ASSERT(!ListView_IsOwnerData(plv));

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	// don't do this optimization if we will need the indices to sort by
	if (psi->bPassLP && ((!plv->hdpaSubItems) || !DPA_GetPtrCount(plv->hdpaSubItems)))
	{
		psi->fSortIndices = FALSE;
		return (IUI_DPA_Sort(plv->hdpa, ListView_SortCallback, (LPARAM)psi));
	}
	else
	{
		// if we need to sort several hdpa's, create one DPA of just indices
		// and sort that, then fix up all the dpa's
		BOOL fReturn = FALSE;
		HDPA hdpa;
		int i;
		int iMax;
		void **ph;
		void **pNewIndices;

		// initialize the hdpa with indices
		hdpa = IUI_DPA_Clone(plv->hdpa, NULL);
		if (hdpa)
		{
			_ASSERT(DPA_GetPtrCount(plv->hdpa) == DPA_GetPtrCount(hdpa));
			ph = pNewIndices = DPA_GetPtrPtr(hdpa);
			iMax = DPA_GetPtrCount(hdpa);
			for (i = 0; i < iMax; ph++, i++)
			{
				*ph = (LPVOID)(HANDLE)i;
			}

			psi->fSortIndices = TRUE;
			if (IUI_DPA_Sort(hdpa, ListView_SortCallback, (LPARAM)psi))
			{
#ifdef WIN32
				ph = (void **)LocalAlloc(LPTR, sizeof(LPVOID) * iMax);
#else
				ph = Alloc(sizeof(LPVOID) * iMax);
#endif

				if (ph)
				{
					int j;
					void **pSubItems;

					// we could get here because bPassLP is false, even if we don't have subitems
					if (plv->hdpaSubItems && DPA_GetPtrCount(plv->hdpaSubItems))
					{
						for (i = DPA_GetPtrCount(plv->hdpaSubItems) - 1; i >= 0; i--)
						{
							HDPA hdpaSubItem = ListView_GetSubItemDPA(plv, i);

							if (hdpaSubItem)
							{

								// make sure it's of the right size
								while (DPA_GetPtrCount(hdpaSubItem) < iMax)
								{
									if (IUI_DPA_InsertPtr(hdpaSubItem, iMax, NULL) == -1)
									{
										goto Bail;
									}
								}


								// actually copy across the dpa with the new indices
								pSubItems = DPA_GetPtrPtr(hdpaSubItem);
								for (j = 0; j < iMax; j++)
								{
									ph[j] = pSubItems[PtrToUlong(pNewIndices[j])];
								}

								// finally, copy it all back to the pSubItems;
								hmemcpy(pSubItems, ph, sizeof(LPVOID) * iMax);
							}
						}
					}

					// now do the main hdpa
					pSubItems = DPA_GetPtrPtr(plv->hdpa);
					for (j = 0; j < iMax; j++)
					{
						ph[j] = pSubItems[PtrToUlong(pNewIndices[j])];
					}

					// finally, copy it all back to the pSubItems;
					hmemcpy(pSubItems, ph, sizeof(LPVOID) * iMax);
					fReturn = TRUE;
Bail:
#ifdef WIN32
					LocalFree(ph);
#else
					Free(ph);
#endif

				}
			}
			IUI_DPA_Destroy(hdpa);
		}
		return fReturn;

	}
}

DWORD ListView_OnApproximateViewRect(SListViewData *plv, int iCount, int iWidth, int iHeight)
{
	if (iCount == -1)
	{
		iCount = ListView_Count(plv);
	}

	if (iWidth == -1)
	{
		iWidth = plv->sizeClient.cx;
	}

	if (iHeight == -1)
	{
		iHeight = plv->sizeClient.cy;
	}

	return _ListView_ApproximateViewRect(plv, iCount, iWidth, iHeight);
}

//L DWORD ListView_OnSetLVRangeObject(SListViewData *plv, int iWhich, ILVRange *plvrange)
// {
// 	ILVRange **pplvrange;
// 	switch (iWhich)
// 	{
// 	case LVSR_SELECTION:
// 		pplvrange = &plv->plvrangeSel;
// 		break;
// 	case LVSR_CUT:
// 		pplvrange = &plv->plvrangeCut;
// 		break;
// 	default:
// 		return FALSE;
// 	}
// 	if (*pplvrange)
// 	{
// 		// Release the old one
// 		(*pplvrange)->lpVtbl->Release(*pplvrange);
// 	}
// 	*pplvrange = plvrange;
//
// 	// Hold onto the pointer...
// 	if (plvrange)
// 	{
// 		plvrange->lpVtbl->AddRef(plvrange);
// 	}
//
// 	return TRUE;
// }


BOOL PASCAL ListView_OnSortItems(SListViewData *plv, LPARAM lParam, PFNLVCOMPARE pfnCompare, BOOL bPassLP)
{
	LVSortInfo SortInfo;
	LISTITEM *pitemFocused;
	SortInfo.pfnCompare = pfnCompare;
	SortInfo.lParam     = lParam;
	SortInfo.plv = plv;
	SortInfo.bPassLP = bPassLP;

	if (ListView_IsOwnerData(plv))
	{
		RIPMSG(0, "LVM_SORTITEMS: Invalid for owner-data listview");
		return FALSE;
	}

	ListView_DismissEdit(plv, TRUE);    // cancel edits

	// we're going to screw with the indices, so stash away the pointer to the
	// focused item.
	if (plv->iFocus != -1)
	{
		pitemFocused = ListView_GetItemPtr(plv, plv->iFocus);
	}
	else
	{
		pitemFocused = NULL;
	}

	if (ListView_SortAllColumns(plv, &SortInfo))
	{

		// restore the focused item.
		if (pitemFocused)
		{
			int i;
			for (i = ListView_Count(plv) - 1; i >= 0 ; i--)
			{
				if (ListView_GetItemPtr(plv, i) == pitemFocused)
				{
					plv->iFocus = i;
					plv->iMark = i;
				}
			}
		}

		if (ListView_IsSmallView(plv) || ListView_IsIconView(plv))
		{
			ListView_CommonArrange(plv, LVA_DEFAULT, plv->hdpa);
		}
		else if (ListView_IsReportView(plv) || ListView_IsListView(plv))
		{
			InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
		}

		// The items in the view have moved around; let apps know
		MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(plv->hwnd), OBJID_CLIENT, 0);
		return (TRUE);
	}
	return FALSE;
}


void PASCAL ListView_EnableWindow(SListViewData *plv, BOOL wParam)
{
	if (wParam)
	{
		if (plv->hwnd->ci.style & WS_DISABLED)
		{
			plv->hwnd->ci.style &= ~WS_DISABLED;      // enabled
			ListView_OnSetBkColor(plv, plv->clrBkSave);
		}
	}
	else
	{
		if (!(plv->hwnd->ci.style & WS_DISABLED))
		{
			plv->clrBkSave = plv->clrBk;
			plv->hwnd->ci.style |= WS_DISABLED;       // disabled
			ListView_OnSetBkColor(plv, GetSysColor(COLOR_BTNFACE));
		}
	}
	RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}


BOOL _ListView_IsItemVisible(SListViewData *plv, int i)
// Assumes parmss ok etc for speed. Called inside region calc code.
{
	RECT rcBounds;
	RECT rc, rcScratch;

	// get bounding rect of item
	ListView_GetRects(plv, i, NULL, NULL, &rcBounds, NULL);

	// setup rect for listview client. Should perf this up for
	// multimonitor case where there are dead zones in work area...
	rc.left  = 0;
	rc.top   = 0;
	rc.right = plv->sizeClient.cx;
	rc.bottom = plv->sizeClient.cy;

	return IntersectRect(&rcScratch, &rc, &rcBounds);
}



// Helper for ListView_RecalcRegion
#define BitOn(lpbits, x, y, cx) (*((BYTE *)(lpbits + ((y * cx) + (x / 8)))) & (0x80 >> (x % 8)))

void ListView_RecalcRegion(SListViewData *plv, BOOL fForce, BOOL fRedraw)
{
	HRGN hrgnUnion = NULL;
	HRGN hrgn = NULL;
	int i;
	HDC hdc = NULL;
	BYTE *lpBits = NULL;
	HBITMAP hbmp = NULL, hbmpOld = NULL;
	RECT rc, rcIcon;
	LISTITEM *pitem;
	BITMAP bm;

	// Bail out if we don't need to do any work
	if (!(plv->exStyle & LVS_EX_REGIONAL) || !ListView_RedrawEnabled(plv) ||
		(plv->flags & LVF_INRECALCREGION))
	{
		return;
	}

	// To prevent recursion
	plv->flags |= LVF_INRECALCREGION;

	if ((ListView_Count(plv) > 0))
	{
		int cxIcon, cyIcon;
		int dxOffset, dyOffset;

		// Run through first to see if anything changed - bail if not!
		if (!fForce)
		{
			for (i = 0; i < ListView_Count(plv); i++)
			{
				pitem = ListView_FastGetItemPtr(plv, i);

				if (!_ListView_IsItemVisible(plv, i))
				{
					if (pitem->hrgnIcon == (HANDLE)(-1) || !pitem->hrgnIcon)
						// Item was invisible and still is. Nothing changed.
					{
						continue;
					}

					if (pitem->hrgnIcon)
					{
						// Item was visible and now is invisible... Something
						// changed.
						pitem->ptRgn.x = RECOMPUTE;
						pitem->ptRgn.y = RECOMPUTE;
						DeleteObject(pitem->hrgnIcon);
						pitem->hrgnIcon = NULL;
					}
				}

				ListView_GetRects(plv, i, NULL, &rc, NULL, NULL);

				// If the location of the icon or the text rectangle have
				// changed, then we need to continue so that we can recalculate
				// the region.
				if ((pitem->pt.x != pitem->ptRgn.x) ||
					(pitem->pt.y != pitem->ptRgn.y) ||
					(!pitem->hrgnIcon) ||
					!EqualRect((CONST RECT *)&pitem->rcTextRgn, (CONST RECT *)&rc))
				{
					goto changed;
				}

			}
			// If we go through all the items and nothing changed, then
			// we can return without doing any work!
			_ASSERT(i == ListView_Count(plv));
			goto exit;
changed:
			;
		}

		// Figure out the dimensions of the Icon rectangle - assumes
		// each Icon rectangle is the same size.
		ListView_GetRects(plv, 0, &rcIcon, NULL, NULL, NULL);

		// Center the icon in the rectangle
		ImageList_GetIconSize(plv->himl, &cxIcon, &cyIcon);
		dxOffset = (rcIcon.right - rcIcon.left - cxIcon) / 2;
		dyOffset = (rcIcon.bottom - rcIcon.top - cyIcon) / 2;
		cxIcon = rcIcon.right - rcIcon.left;
		cyIcon = rcIcon.bottom - rcIcon.top;

		if (!(hdc = CreateCompatibleDC(NULL)) ||
			(!(hbmp = CreateBitmap(cxIcon, cyIcon, 1, 1, NULL))))
		{
			goto BailOut;
		}

		GetObject(hbmp, sizeof(bm), &bm);

		if (!(lpBits = (BYTE *)LocalAlloc(GPTR, bm.bmWidthBytes * bm.bmHeight)))
		{
			goto BailOut;
		}

		hbmpOld = (HBITMAP)SelectObject(hdc, hbmp);
		PatBlt(hdc, 0, 0, cxIcon, cyIcon, WHITENESS);

		if (hrgnUnion = CreateRectRgn(0, 0, 0, 0))
		{
			for (i = 0; i < ListView_Count(plv); i++)
			{
				int x, y, iResult;
				BOOL fStarted = FALSE;
				LPRECT lprc;

				pitem = ListView_FastGetItemPtr(plv, i);

				if (pitem->pt.y == RECOMPUTE)
				{
					continue;
				}

				if (!_ListView_IsItemVisible(plv, i))
				{
					// ignore invisible items
					if (pitem->hrgnIcon && pitem->hrgnIcon != (HANDLE)(-1))
					{
						pitem->ptRgn.x = RECOMPUTE;
						pitem->ptRgn.y = RECOMPUTE;
						DeleteObject(pitem->hrgnIcon);
						pitem->hrgnIcon = (HRGN)(-1);
					}
					continue;
				}

				// Add the region for the icon text first
				ListView_GetRects(plv, i, &rcIcon, &rc, NULL, NULL);

				// If we're in edit mode always use rcTextRgn
				if (i == plv->iEdit)
				{
					lprc = &pitem->rcTextRgn;
				}
				else
				{
					lprc = &rc;
				}

				if (!(hrgn = CreateRectRgnIndirect(lprc)))
				{
					goto Error;
				}

				iResult = CombineRgn(hrgnUnion, hrgn, hrgnUnion, RGN_OR);

				DeleteObject(hrgn);

				if (iResult == ERROR)
				{
					// Error case - out of memory.  Just select in a NULL region.
Error:
					DeleteObject(hrgnUnion);
					hrgnUnion = NULL;
					break;
				}

				// Succeeded, copy the rectangle to rcTextRgn so we
				// can test against it in the future.  Don't copy over
				// it if we are in edit mode, the rectangle is used to
				// store the edit window in that case.
				if (plv->iEdit != i)
				{
					CopyRect(&pitem->rcTextRgn, (CONST RECT *)&rc);
				}

				// Now create a region for the icon mask - or use the cached one
				if (!pitem->hrgnIcon || pitem->hrgnIcon == (HANDLE)(-1))
				{
					//                    (pitem->pt.x != pitem->ptRgn.x) ||
					//                    (pitem->pt.y != pitem->ptRgn.y)) {
					HRGN hrgnIcon = NULL;

					// On slow machines, we'll just wrap the icon with a rectangle.  But on
					// faster machines, we'll build a region that corresponds to the
					// mask for the icon so it looks sweet.
					if (g_fSlowMachine)
					{
						// Modify the rectangle slightly so it looks better

						// Glue the icon and text rectangles together
						rcIcon.bottom = rc.top;
						// Shrink the width of the rectangle so it's only as big as the icon itself
						InflateRect(&rcIcon, -dxOffset, 0);
						hrgnIcon = CreateRectRgnIndirect(&rcIcon);
					}
					else
					{
						// If the image isn't around, get it now.
						if (pitem->iImage == I_IMAGECALLBACK)
						{
							LVITEMEX item;

							item.iItem = i;
							item.iSubItem = 0;
							item.mask = LVIF_IMAGE;
							item.stateMask = LVIS_ALL;
							item.pszText = NULL;
							item.cchTextMax = 0;
							item.hBindWindowless = NULL;
							// BOGUS - do we need to worry about our state
							// getting messed up during the callback?
							ListView_OnGetItem(plv, &item);
						}

						ImageList_Draw(plv->himl, pitem->iImage, hdc, 0, 0, ILD_MASK | (pitem->state & LVIS_OVERLAYMASK));

						GetBitmapBits(hbmp, bm.bmWidthBytes * bm.bmHeight, (LPVOID)lpBits);

						for (y = 0; y < cyIcon; y++)
						{
							for (x = 0; x < cxIcon; x++)
							{
								if (!fStarted && !BitOn(lpBits, x, y, bm.bmWidthBytes))
								{
									rc.left = x;
									rc.top = y;
									rc.bottom = y + 1;
									fStarted = TRUE;
									if (x == (cxIcon - 1))
									{
										x++;
										goto AddIt;
									}
									else
									{
										continue;
									}
								}

								if (fStarted && BitOn(lpBits, x, y, bm.bmWidthBytes))
								{
AddIt:
									rc.right = x;
									//
									// Mirror the region so that the icons get displayed ok. [samera]
									//
									if (plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)
									{
										int iLeft = rc.left;
										rc.left = (cxIcon - (rc.right + 1));
										rc.right = (cxIcon - (iLeft + 1));
										OffsetRect(&rc, rcIcon.left - dxOffset, rcIcon.top + dyOffset);
									}
									else
									{
										OffsetRect(&rc, rcIcon.left + dxOffset, rcIcon.top + dyOffset);
									}


									if (hrgn = CreateRectRgnIndirect(&rc))
									{
										if (hrgnIcon || (hrgnIcon = CreateRectRgn(0, 0, 0, 0)))
										{
											iResult = CombineRgn(hrgnIcon, hrgn, hrgnIcon, RGN_OR);
										}
										else
										{
											iResult = ERROR;
										}

										DeleteObject(hrgn);
									}

									if (!hrgn || (iResult == ERROR))
									{
										if (hrgnIcon)
										{
											DeleteObject(hrgnIcon);
										}
										goto Error;
									}

									fStarted = FALSE;
								}
							}
						}
					}

					if (hrgnIcon)
					{
						// Cache it since it takes a long time to build it
						if (pitem->hrgnIcon && pitem->hrgnIcon != (HANDLE)(-1))
						{
							DeleteObject(pitem->hrgnIcon);
						}
						pitem->hrgnIcon = hrgnIcon;
						pitem->ptRgn = pitem->pt;

						// Add it to the accumulated window region
						if (ERROR == CombineRgn(hrgnUnion, hrgnIcon, hrgnUnion, RGN_OR))
						{
							goto Error;
						}
					}
				}
				else
				{
					OffsetRgn(pitem->hrgnIcon, pitem->pt.x - pitem->ptRgn.x, pitem->pt.y - pitem->ptRgn.y);
					pitem->ptRgn = pitem->pt;
					if (ERROR == CombineRgn(hrgnUnion, pitem->hrgnIcon, hrgnUnion, RGN_OR))
					{
						goto Error;
					}
				}
			}
		}
	}

BailOut:
	if (lpBits)
	{
		LocalFree((HGLOBAL)lpBits);
	}
	if (hbmp)
	{
		SelectObject(hdc, hbmpOld);
		DeleteObject(hbmp);
	}
	if (hdc)
	{
		DeleteDC(hdc);
	}

	// Windows takes ownership of the region when we select it in to the window
	SetWindowRgn(GetHostWnd(plv->hwnd), hrgnUnion, fRedraw);

exit:
	plv->flags &= ~LVF_INRECALCREGION;
}

void ListView_InitCheckBoxes(SListViewData *plv, BOOL fInitializeState)
{
	HIMAGELIST himlCopy = (plv->himlSmall ? plv->himlSmall : plv->himl);
	HIMAGELIST himl;
	BOOL fNoColorKey = FALSE;    // Backwards: If Cleartype is turned on, then we don't use colorkey.
	//L	if (g_bRunOnNT5)
	//	{
	//#ifdef CLEARTYPE    // Don't use SPI_CLEARTYPE because it's defined because of APIThk, but not in NT.
	//		SystemParametersInfo(SPI_GETCLEARTYPE, 0, &fNoColorKey, 0);
	//#endif
	//	}

	// [msadek], CheckBoxed need not to be mirrored.
	// mirroer it during imagelist creation time so that it displays correctly

	himl = CreateCheckBoxImagelist(himlCopy, FALSE, !fNoColorKey, IS_WINDOW_RTL_MIRRORED(GetHostWnd(plv->hwnd)));
	ImageList_SetBkColor(himl, fNoColorKey ? (CLR_NONE) : (plv->clrBk));
	ListView_OnSetImageList(plv, himl, LVSIL_STATE);

	if (fInitializeState)
	{
		ListView_OnSetItemState(plv, -1, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
	}
}

void ListView_PopBubble(SListViewData *plv)
{
	if (plv->hwndToolTips)
	{
		SendMessage(plv->hwndToolTips, TTM_POP, 0, 0);
	}
}

DWORD PASCAL ListView_ExtendedStyleChange(SListViewData *plv, DWORD dwNewStyle, DWORD dwExMask)
{
	DWORD dwOldStyle = plv->exStyle;

	// this messge didn't come till 3, so version is at least 3
	if (plv->hwnd->ci.iVersion < 3)
	{
		plv->hwnd->ci.iVersion = 3;
		// this will change the listview report size and painting algorithm
		// because of the leading edge, so need to re-update scroll bars
		// and repaint everything
		if (ListView_IsReportView(plv))
		{
			ListView_RUpdateScrollBars(plv);
			InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
		}
	}

	// Change of styles may also changes tooltip policy, so pop it
	ListView_PopBubble(plv);

	if (dwExMask)
	{
		dwNewStyle = (plv->exStyle & ~ dwExMask) | (dwNewStyle & dwExMask);
	}

	// Currently, the LVS_EX_REGIONAL style is only supported for large icon view
	if (!ListView_IsIconView(plv))
	{
		dwNewStyle &= ~(LVS_EX_REGIONAL | LVS_EX_MULTIWORKAREAS);
	}

	// LVS_EX_REGIONAL is not supported for ownerdata
	if (ListView_IsOwnerData(plv))
	{
		dwNewStyle &= ~LVS_EX_REGIONAL;
	}

	plv->exStyle = dwNewStyle;

	// do any invalidation or whatever is needed here.
	if ((dwOldStyle ^ dwNewStyle) & LVS_EX_GRIDLINES)
	{
		if (ListView_IsReportView(plv))
		{
			InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
		}
	}

	if ((dwOldStyle ^ dwNewStyle) & (LVS_EX_UNDERLINEHOT | LVS_EX_UNDERLINECOLD |
			LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE |
			LVS_EX_SUBITEMIMAGES))
	{
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
	}

	if ((dwOldStyle ^ dwNewStyle) & LVS_EX_CHECKBOXES)
	{
		if (dwNewStyle & LVS_EX_CHECKBOXES)
		{
			ListView_InitCheckBoxes(plv, TRUE);
		}
		else
		{
			// destroy the check boxes!
			HIMAGELIST himl = ListView_OnSetImageList(plv, NULL, LVSIL_STATE);
			if (himl)
			{
				ImageList_Destroy(himl);
			}
		}
	}

	if ((dwOldStyle ^ dwNewStyle) & LVS_EX_FLATSB)
	{
		if (dwNewStyle & LVS_EX_FLATSB)
		{
			InitializeFlatSB(GetHostWnd(plv->hwnd));
		}
		else
		{
			UninitializeFlatSB(GetHostWnd(plv->hwnd));
		}
	}

	if ((dwOldStyle ^ dwNewStyle) & LVS_EX_REGIONAL)
	{
		if (g_fSlowMachine == -1)
		{
#ifdef NEVER
			// Because some Alpha machines and faster pentiums were detected
			// as slow machines (bug #30972 in IE4 database), it was decided
			// to turn off this code.
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			g_fSlowMachine = (BOOL)((si.dwProcessorType == PROCESSOR_INTEL_386) ||
					(si.dwProcessorType == PROCESSOR_INTEL_486));
#else
			g_fSlowMachine = FALSE;
#endif
		}
		if (dwNewStyle & LVS_EX_REGIONAL)
		{
			ListView_RecalcRegion(plv, TRUE, TRUE);
		}
		else
		{
			int i;
			LISTITEM *pitem;

			// Delete all the cached regions, then NULL out our selected region.
			for (i = 0; i < ListView_Count(plv); i++)
			{
				pitem = ListView_FastGetItemPtr(plv, i);
				if (pitem->hrgnIcon && pitem->hrgnIcon != (HANDLE)(-1))
				{
					DeleteObject(pitem->hrgnIcon);
				}
				pitem->hrgnIcon = NULL;
			}
			SetWindowRgn(GetHostWnd(plv->hwnd), (HRGN)NULL, TRUE);
		}
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
	}

	return dwOldStyle;
}

// BUGBUG raymondc v6.0:  Doesn't detect WM_WINDOWPOSCHANGING as a way
// of being shown.  NT5 defview has to hack around it pretty grossly.
// Fix for v6.0.

void PASCAL LV_OnShowWindow(SListViewData *plv, BOOL fShow)
{
	if (fShow)
	{
		if (!(plv->flags & LVF_VISIBLE))
		{
			plv->flags |= LVF_VISIBLE;
			ListView_UpdateScrollBars(plv);
		}
	}
	else
	{
		plv->flags &= ~LVF_VISIBLE;
	}

}

LRESULT PASCAL ListView_OnHelp(SListViewData *plv, LPHELPINFO lpHelpInfo)
{

	//  If we're seeing WM_HELP because of our child header control, then
	//  munge the HELPINFO structure to use the ListView's control id.
	//  win\core\user\combo.c has similiar code to handle the child edit
	//  control of a combo box.
	if ((lpHelpInfo != NULL)
		&& ListView_IsReportView(plv)
		&& (lpHelpInfo->iCtrlId == LVID_HEADER))
	{

		lpHelpInfo->hItemHandle = GetHostWnd(plv->hwnd);
		lpHelpInfo->iCtrlId = GetWindowID(GetHostWnd(plv->hwnd));
		//  Shouldn't have to do this: USER would have filled in the appropriate
		//  context id by walking up the parent hwnd chain.
		//lpHelpInfo->dwContextId = GetContextHelpId(hwnd);

	}

	return DefWindowProc(GetHostWnd(plv->hwnd), WM_HELP, 0, (LPARAM)lpHelpInfo);

}

DWORD PASCAL ListView_OnSetIconSpacing(SListViewData *plv, LPARAM lParam)
{
	DWORD dwOld = MAKELONG(plv->cxIconSpacing, plv->cyIconSpacing);

	if (lParam == (LPARAM)(-1))
	{
		// go back to using defaults
		plv->flags &= ~LVF_ICONSPACESET;
		plv->cxIconSpacing = (plv->cxIcon + (GetSystemMetrics(SM_CXICONSPACING) - IUIGetGloabelVar()->g_cxIcon));
		plv->cyIconSpacing = (plv->cyIcon + (GetSystemMetrics(SM_CYICONSPACING) - IUIGetGloabelVar()->g_cyIcon));
	}
	else
	{
		if (LOWORD(lParam))
		{
			plv->cxIconSpacing = LOWORD(lParam);
		}
		if (HIWORD(lParam))
		{
			plv->cyIconSpacing = HIWORD(lParam);
		}

		plv->flags |= LVF_ICONSPACESET;
	}
	plv->iFreeSlot = -1;

	return dwOld;
}

BOOL ListView_OnSetCursorMsg(SListViewData *plv)
{
	if (plv->exStyle & (LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE))
	{
		if (plv->iHot != -1)
		{
			if (((plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickOK)) ||
				ListView_OnGetItemState(plv, plv->iHot, LVIS_SELECTED))
			{
				if (!plv->hCurHot)
				{
					plv->hCurHot = LoadHandCursor(0);
				}
				SetCursor(plv->hCurHot);

				return TRUE;
			}
		}
	}

	return FALSE;
}

void ListView_OnSetHotItem(SListViewData *plv, int iItem)
{
	UINT maskChanged;

	if (iItem != plv->iHot)
	{
		BOOL fSelectOnly;
		UINT fRedraw = RDW_INVALIDATE;
#ifndef DONT_UNDERLINE
		if (plv->clrTextBk == CLR_NONE)
		{
			fRedraw |= RDW_ERASE;
		}
#endif
		fSelectOnly = ListView_FullRowSelect(plv);
		maskChanged = (plv->exStyle & LVS_EX_BORDERSELECT) ? LVIF_TEXT | LVIF_IMAGE : LVIF_TEXT;
		ListView_InvalidateItemEx(plv, plv->iHot, 0, fSelectOnly, fRedraw, maskChanged);
		ListView_InvalidateItemEx(plv, iItem, 0, fSelectOnly, RDW_INVALIDATE, maskChanged);
		plv->iHot = iItem;
	}
}


/// Usability test prototype
// CHEEBUGBUG
BOOL fShouldFirstClickActivate()
{
	static BOOL fInited = FALSE;
	static BOOL fActivate = TRUE;
	if (!fInited)
	{
		long cb = 0;
		if (RegQueryValue(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\NoFirstClickActivate"),
				NULL, &cb) == ERROR_SUCCESS)
		{
			fActivate = FALSE;
		}
		fInited = TRUE;
	}
	return fActivate;
}

BOOL ChildOfDesktop(HWND hwnd)
{
	return IsChild(GetShellWindow(), hwnd);
}


void ListView_OnMouseMove(SListViewData *plv, int x, int y, UINT uFlags)
{
	if (plv->exStyle & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE))
	{
		// CHEEBUGBUG: for usability testing
		if (ChildOfActiveWindow(GetHostWnd(plv->hwnd))
			|| fShouldFirstClickActivate()
			|| ChildOfDesktop(GetHostWnd(plv->hwnd)))
		{
			int iItem;
			LV_HITTESTINFO ht;
			NMLISTVIEW nm;

			ht.pt.x = x;
			ht.pt.y = y;
			iItem = ListView_OnSubItemHitTest(plv, &ht);
			if (ht.iSubItem != 0)
			{
				// if we're not in full row select,
				// hitting on a subitem is like hitting on nowhere
				// also, in win95, ownerdraw fixed effectively had full row select
				if (!ListView_FullRowSelect(plv) &&
					!(plv->hwnd->ci.style & LVS_OWNERDRAWFIXED))
				{
					iItem = -1;
					ht.flags = LVHT_NOWHERE;
				}
			}

			if (ht.flags & LVHT_NOWHERE ||
				ht.flags & LVHT_ONITEMSTATEICON)
			{
				iItem = -1; // this is possible in the list mode (sucks)
			}

			nm.iItem = iItem;
			nm.iSubItem = ht.iSubItem;
			nm.uChanged = 0;
			nm.ptAction.x = x;
			nm.ptAction.y = y;

			if (!CCSendNotify(&plv->hwnd->ci, LVN_HOTTRACK, &nm.hdr))
			{
				ListView_OnSetHotItem(plv, nm.iItem);
				// Ensure our cursor is correct now since the WM_SETCURSOR
				// message was already generated for this mouse event.
				ListView_OnSetCursorMsg(plv);

				// this lets us know when we've left an item
				// and can then reselect/toggle it on hover events
				if (iItem != plv->iNoHover)
				{
					plv->iNoHover = -1;
				}
			}
		}
	}
}

BOOL EditBoxHasFocus()
{
	HWND hwndFocus = GetFocus();

	if (hwndFocus)
	{
		if (SendMessage(hwndFocus, WM_GETDLGCODE, 0, 0) & DLGC_HASSETSEL)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ListView_OnMouseHover(SListViewData *plv, int x, int y, UINT uFlags)
{
	int iItem;
	BOOL bSelected;
	LV_HITTESTINFO ht;
	BOOL fControl;
	BOOL fShift;
	BOOL fNotifyReturn = FALSE;

	if (GetCapture() || !ChildOfActiveWindow(GetHostWnd(plv->hwnd)) ||
		EditBoxHasFocus())
	{
		return;    // ignore hover while editing or any captured (d/d) operation
	}

	if (CCSendNotify(&plv->hwnd->ci, NM_HOVER, NULL))
	{
		return;
	}

	// REVIEW: right button implies no shift or control stuff
	// Single selection style also implies no modifiers
	//if (RIGHTBUTTON(keyFlags) || (plv->hwnd->ci.style & LVS_SINGLESEL))
	if ((plv->hwnd->ci.style & LVS_SINGLESEL))
	{
		fControl = FALSE;
		fShift = FALSE;
	}
	else
	{
		fControl = GetAsyncKeyState(VK_CONTROL) < 0;
		fShift = GetAsyncKeyState(VK_SHIFT) < 0;
	}

	ht.pt.x = x;
	ht.pt.y = y;
	iItem = ListView_OnHitTest(plv, &ht);

	if (iItem == -1 ||
		iItem == plv->iNoHover)
	{
		return;
	}

	//before we hover select we launch any pending item
	//this prevents clicking on one item and hover selecting other before
	//the timer goes off which result in wrong item being launched
	if (plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickHappened && plv->fOneClickOK)
	{
		HWND hwnd = GetHostWnd(plv->hwnd);

		IUI::KillTimer(plv->hwnd, IDT_ONECLICKHAPPENED);
		plv->fOneClickHappened = FALSE;
		CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &(plv->nmOneClickHappened.hdr));
		if (!IsWindow(hwnd))
		{
			return;
		}
	}

	plv->iNoHover = iItem;
	bSelected = ListView_OnGetItemState(plv, iItem, LVIS_SELECTED);

	if (ht.flags & (LVHT_ONITEMLABEL | LVHT_ONITEMICON))
	{
		UINT keyFlags = 0;

		if (fShift)
		{
			keyFlags |= MK_SHIFT;
		}
		if (fControl)
		{
			keyFlags |= MK_CONTROL;
		}

		if (!bSelected)
		{
			// if it wasn't selected, we're about to select it... play
			// a little ditty for us...
			CCPlaySound(IUIGetGloabelVar()->c_szSelect);
		}

		ListView_ButtonSelect(plv, iItem, keyFlags, bSelected);

		if (fControl)
		{
			ListView_SetFocusSel(plv, iItem, !fShift, FALSE, !fShift);
		}

		if (!fShift)
		{
			plv->iMark = iItem;
		}

		ListView_OnSetCursorMsg(plv);

		SetFocus(GetHostWnd(plv->hwnd), plv->hwnd);    // activate this window

	}
}

BOOL EqualRects(LPRECT prcNew, LPRECT prcOld, int nRects)
{
	int i;
	for (i = 0; i < nRects; i++)
		if (!EqualRect(&prcNew[i], &prcOld[i]))
		{
			return FALSE;
		}
	return TRUE;
}

BOOL ListView_FindWorkArea(SListViewData *plv, POINT pt, short *piWorkArea)
{
	int iWork;
	for (iWork = 0; iWork < plv->nWorkAreas; iWork++)
	{
		if (PtInRect(&plv->prcWorkAreas[iWork], pt))
		{
			*piWorkArea = (short)iWork;
			return TRUE;
		}
	}

	// (dli) default case is the primary work area
	*piWorkArea = 0;
	return FALSE;
}

void ListView_BullyIconsOnWorkarea(SListViewData *plv, HDPA hdpaLostItems)
{
	int ihdpa;
	int iFree = -1;  // the last free slot number
	LVFAKEDRAW lvfd;
	LVITEMEX item;

	// Caller should've filtered this case out
	_ASSERT(DPA_GetPtrCount(hdpaLostItems) > 0);

	// Set up in case caller is customdraw
	ListView_BeginFakeCustomDraw(plv, &lvfd, &item);
	item.mask = LVIF_PARAM;
	item.iSubItem = 0;
	item.hBindWindowless = NULL;

	// Go through my hdpa list of lost icons and try to place them within bound
	for (ihdpa = 0; ihdpa < DPA_GetPtrCount(hdpaLostItems); ihdpa++)
	{
		POINT ptNew, pt;
		RECT rcBound;
		int cxBound, cyBound;
		int iWidth, iHeight;
		int iItem;
		LISTITEM *pitem;

		iItem = PtrToUlong(IUI_DPA_GetPtr(hdpaLostItems, ihdpa));
		pitem = ListView_FastGetItemPtr(plv, iItem);
		pt = pitem->pt;

		iWidth  = RECTWIDTH(plv->prcWorkAreas[pitem->iWorkArea]);
		iHeight = RECTHEIGHT(plv->prcWorkAreas[pitem->iWorkArea]);

		ListView_GetRects(plv, iItem, NULL, NULL, &rcBound, NULL);
		cxBound = RECTWIDTH(rcBound);
		cyBound = RECTHEIGHT(rcBound);

		pt.x -= plv->prcWorkAreas[pitem->iWorkArea].left;
		pt.y -= plv->prcWorkAreas[pitem->iWorkArea].top;

		if (pt.x < (-cxBound / 2))
		{
			ptNew.x = 0;
		}
		else if (pt.x > (iWidth - (cxBound / 2)))
		{
			ptNew.x = iWidth - cxBound;
		}
		else
		{
			ptNew.x = pt.x;
		}

		if (pt.y < (-cyBound / 2))
		{
			ptNew.y = 0;
		}
		else if (pt.y > (iHeight - (cyBound / 2)))
		{
			ptNew.y = iHeight - cyBound;
		}
		else
		{
			ptNew.y = pt.y;
		}

		if ((ptNew.x != pt.x) || (ptNew.y != pt.y))
		{
			BOOL fUpdate;
			RECT rcTest;
			ptNew.x += plv->prcWorkAreas[pitem->iWorkArea].left;
			ptNew.y += plv->prcWorkAreas[pitem->iWorkArea].top;

			// See if the potential rectangle intersects other items.
			rcTest.left = ptNew.x - plv->ptOrigin.x;
			rcTest.top = ptNew.y - plv->ptOrigin.y;
			rcTest.right = rcTest.left + cxBound;
			rcTest.bottom = rcTest.top + cyBound;

			item.iItem = iItem;
			item.lParam = pitem->lParam;
			ListView_BeginFakeItemDraw(&lvfd);

			if (!ListView_IsCleanRect(plv, &rcTest, iItem, &fUpdate, lvfd.nmcd.nmcd.hdc))
			{
				// doh! We hit another item, let's try to find an available location
				// for this item
				BOOL fUpdateSB;
				BOOL fAppendAtEnd = FALSE;
				int cSlots = ListView_GetSlotCountEx(plv, FALSE, pitem->iWorkArea);
				iFree = ListView_FindFreeSlot(plv, iItem, iFree + 1, cSlots, &fUpdateSB, &fAppendAtEnd, lvfd.nmcd.nmcd.hdc);
				if (iFree == -1)
				{
					goto SetFirstGuess;
				}
				ListView_SetIconPos(plv, pitem, iFree, cSlots);
				ListView_EndFakeItemDraw(&lvfd);
				continue;
			}
SetFirstGuess:
			ListView_EndFakeItemDraw(&lvfd);
			ListView_OnSetItemPosition(plv, iItem, ptNew.x, ptNew.y);
		}
	}
	ListView_EndFakeCustomDraw(&lvfd);
}

#define DPA_LAST    0x7fffffff

//
// ListView_OnSetWorkAreas
//
// set the "work areas" for the list view.
// the "work areas" are a group of sub rectanges of the list view client rect
// where icons are aranged, and parked by default.
//
void ListView_OnSetWorkAreas(SListViewData *plv, int nWorkAreas, LPRECT prc)
{
	int nOldWorkAreas;
	int iWork;
	BOOL bAutoArrange;
	HDPA hdpaLostItems = NULL;
	RECT rcOldWorkAreas[LV_MAX_WORKAREAS];

	bAutoArrange = plv->hwnd->ci.style & LVS_AUTOARRANGE;

	nOldWorkAreas = plv->nWorkAreas;

	if (nOldWorkAreas > 0)
	{
		_ASSERT(plv->prcWorkAreas != NULL);
		hmemcpy(&rcOldWorkAreas[0], &plv->prcWorkAreas[0], sizeof(RECT) * nOldWorkAreas);
	}
	// for the mirrored case, the coordinates are reversed. IsRectEmpty() will always succeed
	if (nWorkAreas == 0 || prc == NULL || ((IsRectEmpty(prc)) && !(plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)))
	{
		plv->nWorkAreas = 0;
	}
	else
	{
		plv->nWorkAreas = min(nWorkAreas, LV_MAX_WORKAREAS);

		if (plv->prcWorkAreas == NULL)
		{
			plv->prcWorkAreas = (LPRECT)LocalAlloc(LPTR, sizeof(RECT) * LV_MAX_WORKAREAS);
		}

		if (plv->prcWorkAreas == NULL)
		{
			return;
		}

		//BUGBUG: Should we check if they intersect? This problem is sort of
		// solved (or made more confusing) by ListView_GetFreeSlot since it checks all of the icons for
		// intersection instead of just the ones in the workarea.
		for (iWork = 0; iWork < plv->nWorkAreas; iWork++)
		{
			CopyRect(&plv->prcWorkAreas[iWork], &prc[iWork]);
		}
	}

	// We don't support workareas for owner-data because our icon placement
	// algorithm (ListView_IGetRectsOwnerData) completely ignores workareas
	// and just dumps the icons in a rectangular array starting at (0,0).
	if (!ListView_IsOwnerData(plv) &&
		plv->nWorkAreas > 0 &&
		((plv->nWorkAreas  != nOldWorkAreas) ||
			(!EqualRects(&plv->prcWorkAreas[0], &rcOldWorkAreas[0], nOldWorkAreas))))
	{
		int iItem;
		LISTITEM *pitem;

		//
		//  Subtle - ListView_Recompute cleans up all the RECOMPUTE icons,
		//  but in order to do that, it needs to have valid work area
		//  rectangles. So the call must happen after the CopyRect but before
		//  the loop that checks the icon positions.
		//
		ListView_Recompute(plv);

		for (iItem = 0; iItem < ListView_Count(plv); iItem++)
		{
			pitem = ListView_FastGetItemPtr(plv, iItem);

			if (pitem->pt.x == RECOMPUTE || pitem->pt.y == RECOMPUTE)
			{
				// ListView_Recompute should've fixed these if we were in
				// an iconical view.
				_ASSERT(!(ListView_IsIconView(plv) || ListView_IsSmallView(plv)));
				continue;
			}

			// Try to move me to the same location relative to the same workarea.
			// This will give the cool shift effect when tools bars take the border areas.
			// And we only want to do this for the workareas that changed

			// Don't bully the icons on the workareas, Autoarrange will do the work for us

			if (nOldWorkAreas > 0)
			{
				int iOldWorkArea;
				iOldWorkArea = pitem->iWorkArea;
				if (iOldWorkArea >= plv->nWorkAreas)
				{
					// My workarea is gone, put me on the primary workarea i.e. #0
					pitem->iWorkArea = 0;
					if (!bAutoArrange)
					{
						// If this item point location is already in the new primary workarea,
						// move it out, and let ListView_BullyIconsOnWorkarea arrange it to the
						// right place. NOTE: this could happen in the case the old secondary monitor
						// is to the left of the old primary monitor, and user kills the secondary monitor
						if (PtInRect(&plv->prcWorkAreas[0], pitem->pt))
						{
							pitem->pt.x = plv->prcWorkAreas[0].right + 1;
							plv->iFreeSlot = -1; // an item moved -- old slot info is invalid
						}
						goto  InsertLostItemsArray;
					}
				}
				else if ((!bAutoArrange) && (!EqualRect(&plv->prcWorkAreas[iOldWorkArea], &rcOldWorkAreas[iOldWorkArea])))
				{
					RECT rcBound;
					POINT ptCenter;
					pitem->pt.x += plv->prcWorkAreas[iOldWorkArea].left - rcOldWorkAreas[iOldWorkArea].left;
					pitem->pt.y += plv->prcWorkAreas[iOldWorkArea].top - rcOldWorkAreas[iOldWorkArea].top;

					// Use the center of this icon to determine whether it's out of bound
					ListView_GetRects(plv, iItem, NULL, NULL, &rcBound, NULL);
					ptCenter.x = pitem->pt.x + RECTWIDTH(rcBound) / 2;
					ptCenter.y = pitem->pt.y + RECTHEIGHT(rcBound) / 2;

					// If this shifted me out of bounds, register to be bullied on the workarea
					if (!PtInRect(&plv->prcWorkAreas[iOldWorkArea], ptCenter))
					{
InsertLostItemsArray:
						if (!hdpaLostItems)
						{
							hdpaLostItems = IUI_DPA_Create(4);
							if (!hdpaLostItems)
								// we ran out of memory
							{
								_ASSERT(0);
							}
						}

						if (hdpaLostItems)
						{
							IUI_DPA_InsertPtr(hdpaLostItems, DPA_LAST, (LPVOID)iItem);
						}
					}
				}

			}
			else
			{
				// My first time in a multi-workarea system, so find out my workarea
				if (!ListView_FindWorkArea(plv, pitem->pt, &(pitem->iWorkArea)) && !bAutoArrange)
				{
					goto InsertLostItemsArray;
				}
			}

			if ((plv->exStyle & LVS_EX_REGIONAL) && (pitem->hrgnIcon))
			{
				if (pitem->hrgnIcon != (HANDLE)(-1))
				{
					DeleteObject(pitem->hrgnIcon);
				}
				pitem->hrgnIcon = NULL;
			}
		}

		if (hdpaLostItems)
		{
			_ASSERT(!bAutoArrange);
			if (DPA_GetPtrCount(hdpaLostItems) > 0)
			{
				ListView_BullyIconsOnWorkarea(plv, hdpaLostItems);
			}

			IUI_DPA_Destroy(hdpaLostItems);
		}

		if (plv->exStyle & LVS_EX_REGIONAL)
		{
			ListView_RecalcRegion(plv, TRUE, TRUE);
		}

		if ((plv->hwnd->ci.style & LVS_AUTOARRANGE) &&
			(ListView_IsSmallView(plv) || ListView_IsIconView(plv)))
		{
			ListView_OnArrange(plv, LVA_DEFAULT);
		}
	}

	RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}

void ListView_OnGetNumberOfWorkAreas(SListViewData *plv, int *pnWorkAreas)
{
	if (pnWorkAreas)
	{
		*pnWorkAreas = plv->nWorkAreas;
	}
}

void ListView_OnGetWorkAreas(SListViewData *plv, int nWorkAreas, LPRECT prc)
{
	int i;
	_ASSERT(prc);
	for (i = 0; i < min(plv->nWorkAreas, nWorkAreas); i++)
	{
		if (i < plv->nWorkAreas)
		{
			CopyRect(&prc[i], &plv->prcWorkAreas[i]);
		}
		else
			// Set the workareas to all zeros if we don't have it.
		{
			ZeroMemory(&prc[i], sizeof(RECT));
		}
	}
}

// test an item to see if it is unfolded (because it is focused)

BOOL ListView_IsItemUnfolded(SListViewData *plv, int item)
{
	return plv && (item >= 0) && ListView_IsIconView(plv) &&
		(plv->flags & LVF_UNFOLDED) && (plv->iFocus == item);
}

BOOL ListView_IsItemUnfoldedPtr(SListViewData *plv, LISTITEM *pitem)
{
	return plv && pitem && ListView_IsIconView(plv) &&
		(plv->flags & LVF_UNFOLDED) && (pitem->state & LVIS_FOCUSED);
}

// Returns TRUE if unfolding the item will be worthwhile
BOOL ListView_GetUnfoldedRect(SListViewData *plv, int iItem, RECT *prc)
{
	ListView_GetRects(plv, iItem, NULL, prc, NULL, NULL);
	return ListView_UnfoldRects(plv, iItem, NULL, prc, NULL, NULL);
}

LRESULT ListView_OnNotify(SListViewData *plv, WPARAM wParam, NMHDR *pnmh)
{
	// we can't switch on the control ID because the tooltip is a WS_POPUP window
	// and does not have a control ID. (header and tooltip both have 0 as ID)

	if (plv->hwndHdr != NULL && (plv->hwndHdr == (HWLWND)wParam))
	{
		// this is a notify for the header, deal with it as needed

		return ListView_HeaderNotify(plv, (HD_NOTIFY *)pnmh);
	}
	else if (plv->hwndToolTips && (plv->hwndToolTips == pnmh->hwndFrom))
	{
		// implement unfolding the text for items as well as info tip support

		switch (pnmh->code)
		{
		case TTN_NEEDTEXT:
		{
			POINT pt;
			UINT uFlags;
			int iNewHit;
			int iNewSubHit;
			NMTTDISPINFO *pttt = (NMTTDISPINFO *)pnmh;

			GetMessagePosClient(plv->hwnd, &pt);
			iNewHit = _ListView_ItemHitTest(plv, pt.x, pt.y, &uFlags, &iNewSubHit);

			if (iNewHit != plv->iTTLastHit || iNewSubHit != plv->iTTLastSubHit)
			{
				plv->fPlaceTooltip = FALSE;     // Set it to TRUE only if Unfolding tip is set
				Str_Set(&plv->pszTip, NULL);    // clear the old tip

				plv->iTTLastHit = iNewHit;
				plv->iTTLastSubHit = iNewSubHit;

				if ((iNewHit >= 0) && (plv->iEdit == -1))
				{
					TCHAR szBuf[INFOTIPSIZE], szBuf2[INFOTIPSIZE];
					BOOL bItemUnfolded;
					BOOL fInfoTip = FALSE;
					LPTSTR pszTip = szBuf;  // Use this one first

					szBuf[0] = 0;
					szBuf2[0] = 0;

					// preload the tip text for folder items. this
					// may be overridden by callback below
					bItemUnfolded = ListView_IsItemUnfolded2(plv, plv->iTTLastHit, plv->iTTLastSubHit, szBuf, ARRAYSIZE(szBuf));
					lstrcpyn(szBuf2, szBuf, ARRAYSIZE(szBuf2)); // Backup the unfolding text

					if (ListView_IsInfoTip(plv) && iNewSubHit == 0)
					{
						NMLVGETINFOTIP git;

						git.dwFlags = bItemUnfolded ? LVGIT_UNFOLDED : 0;
						git.pszText = szBuf;
						git.cchTextMax = ARRAYSIZE(szBuf);
						git.iItem = plv->iTTLastHit;
						git.iSubItem = 0;
						git.lParam = 0;

						// for folded items pszText is prepopulated with the
						// item text, clients should append to this

						CCSendNotify(&plv->hwnd->ci, LVN_GETINFOTIP, &git.hdr);

						// Sometimes defview gets confused and nulls out the
						// buffer instead of leaving it alone (sigh)

						if (szBuf[0] == TEXT('\0'))
						{
							pszTip = szBuf2;  // Use the original text
						}
						else if (lstrcmp(szBuf, szBuf2) != 0)
						{
							// App changed something - there is a real infotip
							fInfoTip = TRUE;
						}

					}

					//
					// Set the margins now before the TTN_SHOW because it will be too late then.
					//
					// We want fat margins if we're an infotip, thin margins if we're an
					// in-place tooltip.
					//
					if (fInfoTip)
					{
						static const RECT rcMargin = {4, 4, 4, 4};
						SendMessage(plv->hwndToolTips, TTM_SETMARGIN, 0, (LPARAM)&rcMargin);
						CCSetInfoTipWidth(GetHostWnd(plv->hwnd), plv->hwndToolTips);

					}
					else
					{
						static const RECT rcMargin = {0, 0, 0, 0};
						plv->fPlaceTooltip = TRUE;
						SendMessage(plv->hwndToolTips, TTM_SETMARGIN, 0, (LPARAM)&rcMargin);
						CCResetInfoTipWidth(GetHostWnd(plv->hwnd), plv->hwndToolTips);
					}

					Str_Set(&plv->pszTip, pszTip);
				}
			}

			pttt->lpszText = plv->pszTip;     // here it is...
		}
		break;

		// Handle custom draw as we want the tooltip painted as a multi-line that
		// matches the formatting used by the list view.

		case NM_CUSTOMDRAW:
		{
			LPNMTTCUSTOMDRAW pnm = (LPNMTTCUSTOMDRAW) pnmh;

			if (plv->fPlaceTooltip &&
				(pnm->nmcd.dwDrawStage == CDDS_PREPAINT ||
					pnm->nmcd.dwDrawStage == CDDS_ITEMPREPAINT))
			{
				DWORD dwCustom = 0;

				//
				//  Set up the customdraw DC to match the font of the SListViewData item.
				//
				if (plv->iTTLastHit != -1)
				{
					LVFAKEDRAW lvfd;
					LVITEMEX item;
					ListView_BeginFakeCustomDraw(plv, &lvfd, &item);

					item.iItem = plv->iTTLastHit;
					item.iSubItem = plv->iTTLastSubHit;
					item.mask = LVIF_PARAM;
					ListView_OnGetItem(plv, &item);
					dwCustom = ListView_BeginFakeItemDraw(&lvfd);

					// If client changed the font, then transfer the font
					// from our private hdc into the tooltip's HDC.  We use
					// a private HDC because we only want to let the app change
					// the font, not the colors or anything else.
					if (dwCustom & CDRF_NEWFONT)
					{
						SelectObject(pnm->nmcd.hdc, GetCurrentObject(lvfd.nmcd.nmcd.hdc, OBJ_FONT));
					}
					ListView_EndFakeItemDraw(&lvfd);
					ListView_EndFakeCustomDraw(&lvfd);

				}

				//
				//  The Large Icon tooltip needs to be drawn specially.
				//
				if (ListView_IsIconView(plv))
				{
					pnm->uDrawFlags &= ~(DT_SINGLELINE | DT_LEFT);
					pnm->uDrawFlags |= DT_CENTER | DT_LVWRAP;

					if (pnm->uDrawFlags & DT_CALCRECT)
					{
						pnm->nmcd.rc.right = pnm->nmcd.rc.left + (plv->cxIconSpacing - IUIGetGloabelVar()->g_cxLabelMargin * 2);
						pnm->nmcd.rc.bottom = pnm->nmcd.rc.top + 0x10000;           // big number, no limit!
					}
				}

				// Don't return other wacky flags to TT, since all we
				// did was change the font (if even that)
				return dwCustom & CDRF_NEWFONT;
			}
		}
		break;

		case TTN_SHOW:
			if (plv->iTTLastHit != -1)
			{
				if (plv->fPlaceTooltip)
				{
					LPNMTTSHOWINFO psi = (LPNMTTSHOWINFO)pnmh;
					RECT rcLabel;

					// In case we're doing subitem hit-testing
					rcLabel.top = plv->iTTLastSubHit;
					rcLabel.left = LVIR_LABEL;

					// reposition to allign with the text rect and
					// set it to topmost
					if (plv->iTTLastSubHit && ListView_OnGetSubItemRect(plv, plv->iTTLastHit, &rcLabel))
					{
						LVITEMEX item;

						// we got the subitem rect. When we draw subitems, we give
						// them SHDT_EXTRAMARGIN, so we have to also
						rcLabel.left += IUIGetGloabelVar()->g_cxLabelMargin * 3;
						rcLabel.right -= IUIGetGloabelVar()->g_cxLabelMargin * 3;

						// And take the image into account, too.
						// ListView_OnGetItem will worry about LVS_EX_SUBITEMIMAGES.
						item.mask = LVIF_IMAGE;
						item.iImage = -1;
						item.iItem = plv->iTTLastHit;
						item.iSubItem = plv->iTTLastSubHit;
						ListView_OnGetItem(plv, &item);
						if (item.iImage != -1)
						{
							rcLabel.left += plv->cxSmIcon;
						}
					}
					else                        // a tip from subitem zero
					{
						ListView_GetUnfoldedRect(plv, plv->iTTLastHit, &rcLabel);
						// SHDrawText actually leaves a g_cxLabelMargin margin
						rcLabel.left += IUIGetGloabelVar()->g_cxLabelMargin;
						rcLabel.right -= IUIGetGloabelVar()->g_cxLabelMargin;
					}

					// In report and list views, SHDrawText does vertical
					// centering (without consulting the custom-draw client,
					// even, so it just centers by a random amount).
					if (ListView_IsListView(plv) || ListView_IsReportView(plv))
					{
						rcLabel.top += (rcLabel.bottom - rcLabel.top - plv->cyLabelChar) / 2;
					}

					SendMessage(plv->hwndToolTips, TTM_ADJUSTRECT, TRUE, (LPARAM)&rcLabel);
					MapWindowRect(GetHostWnd(plv->hwnd), HWND_DESKTOP, &rcLabel);

					if (!ListView_IsIconView(plv))
					{
						// In non-large-icon view, the label size may be greater than the rect returned by ListView_GetUnfoldedRect.
						// So don't specify the size
						SetWindowPos(plv->hwndToolTips, HWND_TOP,
							rcLabel.left, rcLabel.top,
							0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_HIDEWINDOW);
					}
					else
					{
						SetWindowPos(plv->hwndToolTips, HWND_TOP,
							rcLabel.left, rcLabel.top,
							(rcLabel.right - rcLabel.left), (rcLabel.bottom - rcLabel.top),
							SWP_NOACTIVATE | SWP_HIDEWINDOW);
					}
					// This is an inplace tooltip, so disable animation.
					psi->dwStyle |= TTS_NOANIMATE;
					return TRUE;
				}
			}
			break;

		}
	}

	return 0;
}

// Pass the focus to the given window, and then check to see if it exists.
// Passing focus can cause the window to be destroyed (by the Explorer
// when renaming).

BOOL ListView_SetFocus(HWLWND hWnd)
{
	SetFocus(GetHostWnd(hWnd), hWnd);
	return IsWindow(hWnd);
}

void ListView_Realize(SListViewData *plv, HDC hdcParam, BOOL fBackground, BOOL fForceRepaint)
{
	if (plv->hpalHalftone)
	{
		HDC hdc = hdcParam ? hdcParam : GetDC(GetHostWnd(plv->hwnd));

		if (hdc)
		{
			BOOL fRepaint;

			SelectPalette(hdc, plv->hpalHalftone, fBackground);
			fRepaint = RealizePalette(hdc) || fForceRepaint;

			if (!hdcParam)
			{
				ReleaseDC(GetHostWnd(plv->hwnd), hdc);
			}

			if (fRepaint)
			{
				InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
			}
		}
	}
}

BOOL RectInRect(const RECT *prcOuter, const RECT *prcInner)
{
#ifdef COMPLETELYINRECT
	return (prcOuter->left   <= prcInner->left) &&
		(prcOuter->top    <= prcInner->top) &&
		(prcOuter->right  >= prcInner->right) &&
		(prcOuter->bottom >= prcInner->bottom);
#else
	RECT rcDummy;
	return IntersectRect(&rcDummy, prcOuter, prcInner);
#endif
}


LRESULT LVGenerateDragImage(SListViewData *plv, SHDRAGIMAGE *pshdi)
{
	LRESULT lRet = 0;
	int iNumSelected = plv->nSelected;
	int iIndex;
	int iSelectedItem;
	RECT rc = {0, 0, 0, 0};
	RECT rcVisRect;
	HBITMAP hbmpOld = NULL;
	HDC  hdcDragImage;

	// First loop through can get the selection rect
	if (ListView_IsOwnerData(plv))
	{
		//L plv->plvrangeSel->lpVtbl->CountIncluded(plv->plvrangeSel, &iNumSelected);
	}

	if (iNumSelected == 0)
	{
		return FALSE;
	}

	GetClientRect(plv->hwnd, &rcVisRect);


	// Loop Through and calculate the enclosing rect.
	for (iIndex = iNumSelected - 1, iSelectedItem = -1; iIndex >= 0; iIndex--)
	{
		iSelectedItem = ListView_OnGetNextItem(plv, iSelectedItem, LVNI_SELECTED);
		if (iSelectedItem != -1)
		{
			RECT rcItemBounds;

			// Make sure this is in the visible region
			if (ListView_GetItemRect(GetHostWnd(plv->hwnd), iSelectedItem, &rcItemBounds, LVIR_SELECTBOUNDS) &&
				RectInRect(&rcVisRect, &rcItemBounds))
			{
				UnionRect(&rc, &rc, &rcItemBounds);
			}
		}
	}

	hdcDragImage = CreateCompatibleDC(NULL);

	if (!hdcDragImage)
	{
		return 0;
	}

	// After this rc contains the bounds of all the items in Client Coordinates.
	//
	// Mirror the the DC, if the listview is mirrored.
	//
	//L if (plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)
	//{
	//	SET_DC_RTL_MIRRORED(hdcDragImage);
	//}

#define MAX_DRAG_RECT_WIDTH 300
#define MAX_DRAG_RECT_HEIGHT 300
	// If this rect is too big, fix it.
	if (RECTWIDTH(rc) > MAX_DRAG_RECT_WIDTH)
	{
		int iLeft = MAX_DRAG_RECT_WIDTH / 2;
		int iRight = MAX_DRAG_RECT_WIDTH / 2;

		int iRectOriginalLeft = rc.left;
		// Is the left boundry outside the visible rect?
		if (rc.left < plv->ptCapture.x - iLeft)
		{
			// Yes, then we have to clip it.
			rc.left = plv->ptCapture.x - iLeft;
		}
		else
		{
			// No? Well then shift the visible rect to the right, so that we have
			// more room.
			iRight += rc.left - (plv->ptCapture.x - iLeft);
		}

		// Is the right boundry outside the visible rect?
		if (rc.right > plv->ptCapture.x + iRight)
		{
			// Yes, then we have to clip it.
			rc.right = plv->ptCapture.x + iRight;
		}
		else
		{
			// No? Then try and add it to the left
			if (rc.left > iRectOriginalLeft)
			{
				rc.left -= iRight - (rc.right - plv->ptCapture.x);
				if (rc.left < iRectOriginalLeft)
				{
					rc.left = iRectOriginalLeft;
				}
			}
		}
	}

	if (RECTHEIGHT(rc) > MAX_DRAG_RECT_HEIGHT)
	{
		// same for top and bottom:
		// Is the top boundry outside the visible rect?
		int iTop = MAX_DRAG_RECT_HEIGHT / 2;
		int iBottom = MAX_DRAG_RECT_HEIGHT / 2;
		int iRectOriginalTop = rc.top;
		if (rc.top < plv->ptCapture.y - iTop)
		{
			// Yes, then we have to clip it.
			rc.top = plv->ptCapture.y - iTop;
		}
		else
		{
			// No? Well then shift the visible rect to the right, so that we have
			// more room.
			iBottom += rc.top - (plv->ptCapture.y - iTop);
		}

		// Is the right boundry outside the visible rect?
		if (rc.bottom > plv->ptCapture.y + iBottom)
		{
			// Yes, then we have to clip it.
			rc.bottom = plv->ptCapture.y + iBottom;
		}
		else
		{
			// No? Then try and add it to the top
			if (rc.top > iRectOriginalTop)
			{
				rc.top -= iBottom - (rc.bottom - plv->ptCapture.y);
				if (rc.top < iRectOriginalTop)
				{
					rc.top = iRectOriginalTop;
				}
			}
		}
	}

	pshdi->sizeDragImage.cx = RECTWIDTH(rc);
	pshdi->sizeDragImage.cy = RECTHEIGHT(rc);
	pshdi->hbmpDragImage = CreateBitmap(pshdi->sizeDragImage.cx, pshdi->sizeDragImage.cy,
			GetDeviceCaps(hdcDragImage, PLANES), GetDeviceCaps(hdcDragImage, BITSPIXEL),
			NULL);

	if (pshdi->hbmpDragImage)
	{
		LVDRAWITEM lvdi;
		DWORD dwType;
		int cItem;

		RECT rcImage = {0, 0, pshdi->sizeDragImage.cx, pshdi->sizeDragImage.cy};
		hbmpOld = (HBITMAP)SelectObject(hdcDragImage, pshdi->hbmpDragImage);

		pshdi->crColorKey = RGB(0xFF, 0x00, 0x55);
		FillRectClr(hdcDragImage, &rcImage, pshdi->crColorKey);
		pshdi->crColorKey = GetPixel(hdcDragImage, 0, 0);

		// Calculate the offset... The cursor should be in the bitmap rect.

		if (plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)
		{
			pshdi->ptOffset.x = rc.right - plv->ptCapture.x;
		}
		else
		{
			pshdi->ptOffset.x = plv->ptCapture.x - rc.left;
		}
		pshdi->ptOffset.y = plv->ptCapture.y - rc.top;

		lvdi.prcClip = NULL;
		lvdi.plv = plv;
		lvdi.nmcd.nmcd.hdc = hdcDragImage;
		lvdi.pitem = NULL;
		dwType = plv->hwnd->ci.style & LVS_TYPEMASK;
		cItem = ListView_Count(plv);

		// Now loop through again for the paint cycle
		for (iIndex = cItem - 1, iSelectedItem = -1; iIndex >= 0; iIndex--)
		{
			if (ListView_IsOwnerData(plv))
			{
				iSelectedItem++;
				//L plv->plvrangeSel->lpVtbl->NextSelected(plv->plvrangeSel, iSelectedItem, &iSelectedItem);
			}
			else
			{
				LISTITEM *pitem;
				iSelectedItem = (int)(UINT_PTR)DPA_FastGetPtr(plv->hdpaZOrder, iIndex);
				pitem = ListView_FastGetItemPtr(plv, iSelectedItem);
				if (!(pitem->state & LVIS_SELECTED))
				{
					iSelectedItem = -1;
				}
			}

			if (iSelectedItem != -1)
			{
				int     iOldItemDrawing;
				COLORREF crSave;
				POINT ptOrigin = {-rc.left, -rc.top};     //Offset the rects by...
				RECT  rcItemBounds;
				RECT rcTemp;

				iOldItemDrawing = plv->iItemDrawing;
				plv->iItemDrawing = iSelectedItem;
				lvdi.nmcd.nmcd.dwItemSpec = iSelectedItem;
				ListView_GetRects(plv, iSelectedItem, NULL, NULL, &rcItemBounds, NULL);

				// Make sure this is in the visible region
				if (IntersectRect(&rcTemp, &rcVisRect, &rcItemBounds))
				{
					ptOrigin.x += rcItemBounds.left;
					ptOrigin.y += rcItemBounds.top;
					// these may get changed
					lvdi.lpptOrg = &ptOrigin;
					lvdi.flags = 0;
					lvdi.nmcd.clrText = plv->clrText;
					lvdi.nmcd.clrTextBk = plv->clrTextBk;

					// Save the Background color!
					crSave = plv->clrBk;
					plv->clrBk = pshdi->crColorKey;

					ListView_DrawItem(&lvdi);

					plv->clrBk = crSave;
				}
				plv->iItemDrawing = iOldItemDrawing;
			}
		}

		SelectObject(hdcDragImage, hbmpOld);
		DeleteDC(hdcDragImage);

		// We're passing back the created HBMP.
		return 1;
	}


	return lRet;
}

HWLWND _ListView_GetHeader(HWLWND hList)
{
	SListViewData *plv = (SListViewData *)GetMember(hList);
	if (plv == NULL)
	{
		return NULL;
	}

	return plv->hwndHdr;
}

// ListView收到鼠标消息后，如果Item绑定了控件，根据具体情况，转发给这些控件
BOOL LV_IsMouseMessage(SListViewData *plv, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
	{
		return FALSE;
	}

#ifdef _DEBUG
	// 调试用代码
	if (uMsg == WM_LBUTTONUP)
	{
		int n = 0;
	}
#endif

	CIUIPoint pt(lParam);

	//
	// 命中绑定到Item的控件。
	//
	LVHITTESTINFO ht;
	ht.pt.x = pt.x;
	ht.pt.y = pt.y;
	int iItem = ListView_OnSubItemHitTest(plv, &ht);

	if (iItem < 0)
	{
		return FALSE;
	}

	LISTITEM *pitem = ListView_FastGetItemPtr(plv, iItem);

	if (NULL == pitem || NULL == pitem->hBindWindowless)
	{
		return FALSE;
	}

	HWLWND hDest = HitTestChild(pitem->hBindWindowless, &pt);
	if (NULL == hDest)
	{
		return FALSE;
	}

	static int i = 0;
	IUITRACE(_T("%d hDest is %s, caption is %s\n"), i++, GetName(hDest), GetWindowText(hDest));

	// 命中测试
	LRESULT lrHittest = HTCLIENT;
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

	CIUIPoint ptScreen = lParam;
	ClientToScreen(plv->hwnd, &ptScreen);
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
		ClientToScreen(plv->hwnd, &ptDest);
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

///////////////////////////////////////////////////////////
// ListView core

LRESULT WINAPI IUI::ListView_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	SListViewData *plv = (SListViewData *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			if ((pe->RoutedEvent >= WM_MOUSEFIRST) && (pe->RoutedEvent <= WM_MOUSELAST))
			{
				if (plv->exStyle & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE))
				{
					TRACKMOUSEEVENT tme;

					tme.cbSize = sizeof(tme);
					tme.hwndTrack = GetHostWnd(plv->hwnd);
					tme.dwHoverTime = plv->dwHoverTime;
					tme.dwFlags = TME_LEAVE | TME_HOVER | TME_QUERY;

					// see what's set
					TrackMouseEvent(&tme);
					tme.dwFlags &= TME_HOVER | TME_LEAVE;

					// set these bits if they aren't already set
					tme.dwFlags ^= TME_LEAVE;
					if (plv->exStyle & LVS_EX_TRACKSELECT)
					{
						tme.dwFlags ^= TME_HOVER;
					}

					tme.cbSize = sizeof(tme);
					tme.hwndTrack = GetHostWnd(plv->hwnd);
					tme.dwHoverTime = plv->dwHoverTime;
					// set it if there's anything to set
					if (tme.dwFlags & (TME_HOVER | TME_LEAVE))
					{
						TrackMouseEvent(&tme);
					}
				}
			}
		}

		if (pe->eRoutingStrategy == RS_DIRECT)
		{
			if (pe->RoutedEvent == IUIGetGloabelVar()->g_uDragImages)
			{
				return LVGenerateDragImage(plv, (SHDRAGIMAGE *)lParam);
			}
		}

		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (plv == NULL)
			{
				if (pe->RoutedEvent == WM_NCCREATE)
				{
					CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;

					// 分配并清空内存
					plv = (SListViewData *)NearAlloc(sizeof(SListViewData));
					if (!plv)
					{
						OutputDebugString(TEXT("ListView: Out of near memory"));
						return 0L;      // fail the window create
					}

					plv->hwnd = hWnd;
					plv->hwnd->ci.hwndParent = GetParent(hWnd);
					plv->hwnd->ci.style = pcs->style;
					plv->hwnd->ci.dwExStyle = pcs->dwExStyle;
					plv->flags = LVF_REDRAW;    // assume that redrawing enabled!
					plv->iFocus = -1;           // no focus
					plv->iMark = -1;
					plv->iSelCol = -1;
					plv->iDropHilite = -1;      // Assume no item has drop hilite...
					plv->cyItem = 18;//plv->cyItemSave = 1; // never let these be zero, not even for a moment
#ifdef WIN32
					plv->hheap = GetProcessHeap();
#else
					// plv->hheap = NULL;  // not used in 16 bits...
#endif
					SetMember(hWnd, plv);
				}
			}

			switch (pe->RoutedEvent)
			{
				HANDLE_MSG(plv, WM_CREATE, ListView_OnCreate);
				HANDLE_MSG(plv, WM_DESTROY, ListView_OnDestroy);
				HANDLE_MSG(plv, WM_KILLFOCUS, ListView_OnKillFocus);
				HANDLE_MSG(plv, WM_NCDESTROY, ListView_OnNCDestroy);

			case WM_SETFOCUS:
				ListView_OnSetFocus(plv, (HWLWND)wParam);
				return 0;

			case WM_SETCURSOR:
				if (ListView_OnSetCursorMsg(plv))
				{
					return TRUE;
				}
				break;

			case WM_PALETTECHANGED:
				if ((HWND)wParam == GetHostWnd(hWnd))
				{
					break;
				}

			case LVMP_WINDOWPOSCHANGED:
			case WM_WINDOWPOSCHANGED:
				HANDLE_WM_WINDOWPOSCHANGED(plv, wParam, lParam, ListView_OnWindowPosChanged);
				break;

			case WM_SHOWWINDOW:
				LV_OnShowWindow(plv, BOOLFROMPTR(wParam));
				break;

			case WM_WININICHANGE:
				ListView_OnWinIniChange(plv, wParam, lParam);
				break;

			case WM_ENABLE:
				// HACK: we don't get WM_STYLECHANGE on EnableWindow()
				ListView_EnableWindow(plv, BOOLFROMPTR(wParam));
				break;

			case WM_SYSCOLORCHANGE:
				//L InitGlobalColors();
				if (plv->hwnd->ci.style & WS_DISABLED)
				{
					if (!(plv->flags & LVF_USERBKCLR))
					{
						plv->clrBkSave = GetSysColor(COLOR_WINDOW);
					}
					ListView_OnSetBkColor(plv, GetSysColor(COLOR_BTNFACE));
				}
				else if (!(plv->flags & LVF_USERBKCLR))
				{
					ListView_OnSetBkColor(plv, GetSysColor(COLOR_WINDOW));
				}

				if (plv->exStyle & LVS_EX_CHECKBOXES)
				{
					ListView_InitCheckBoxes(plv, FALSE);
				}

				//  98/11/19 #249967 vtan: Always invalidate the list view
				//  rectangle so that the color change causes a refresh.

				::InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
				break;

			case WM_STYLECHANGING:
				ListView_OnStyleChanging(plv, (UINT)wParam, (LPSTYLESTRUCT)lParam);
				return 0;

			case WM_STYLECHANGED:
				ListView_OnStyleChanged(plv, (UINT) wParam, (LPSTYLESTRUCT)lParam);
				return 0L;

			case WM_SIZE:
				if (plv)
				{
					if (plv->hwndToolTips)
					{
						TOOLINFO ti;

						if (ListView_IsLabelTip(plv))
						{
							// A truncated label may have been exposed or vice versa.
							ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);
						}

						ti.cbSize = sizeof(ti);
						ti.hwnd = GetHostWnd(plv->hwnd);
						ti.uId = 0;

						// Resize the tooltip control so that it covers the entire
						// area of the window when its parent gets resized.

						GetClientRect(plv->hwnd, &ti.rect);
						::SendMessage(plv->hwndToolTips, TTM_NEWTOOLRECT, 0, (LPARAM) &ti);
					}
					// if we're supposed to center the image,
					// we need to do a full redraw on each size
					if ((plv->ulBkImageFlags & LVBKIF_SOURCE_MASK) &&
						(plv->xOffsetPercent || plv->yOffsetPercent))
					{
						::InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
					}

				}
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			BOOL bChildHandled = LV_IsMouseMessage(plv, uMsg, wParam, lParam);

			switch (pe->RoutedEvent)
			{
			case WM_MBUTTONDOWN:
				SetFocus(GetHostWnd(hWnd), hWnd);
				if (plv->hwndToolTips)
				{
					RelayToToolTips(plv->hwndToolTips, GetHostWnd(hWnd), uMsg, wParam, lParam);
				}
				break;

			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
				if (plv->hwndToolTips)
				{
					RelayToToolTips(plv->hwndToolTips, GetHostWnd(hWnd), uMsg, wParam, lParam);
				}

				if (!bChildHandled)
				{
					ListView_OnButtonDown(plv, TRUE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (UINT)wParam);
				}
				break;

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				if (!IsEditMode(hWnd))
				{
					if (plv->hwndToolTips)
					{
						RelayToToolTips(plv->hwndToolTips, GetHostWnd(hWnd), uMsg, wParam, lParam);
					}

					if (!bChildHandled)
					{
						ListView_OnButtonDown(plv, FALSE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (UINT)wParam);
					}
				}
				break;

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_NCMOUSEMOVE:
				if (plv->hwndToolTips)
				{
					RelayToToolTips(plv->hwndToolTips, GetHostWnd(hWnd), uMsg, wParam, lParam);
				}
				break;

			case WM_MOUSEHOVER:
				ListView_OnMouseHover(plv, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (UINT) wParam);
				break;

			case WM_MOUSEMOVE:
				if (plv->hwndToolTips)
				{
					UINT uFlags;
					int iHit, iSubHit;

					RelayToToolTips(plv->hwndToolTips, GetHostWnd(hWnd), uMsg, wParam, lParam);

					// check that we are still on the hit item, pop it!
					iHit = _ListView_ItemHitTest(plv, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &uFlags, &iSubHit);

					if (iHit != plv->iTTLastHit || iSubHit != plv->iTTLastSubHit)
					{
						ListView_PopBubble(plv);
					}
				}

				ListView_OnMouseMove(plv, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (UINT) wParam);
				break;

			case WM_MOUSELEAVE:
				ListView_OnSetHotItem(plv, -1);
				plv->iNoHover = -1;
				break;

			case WM_KEYUP:
				break;

			case WM_KEYDOWN:
				HANDLE_WM_KEYDOWN(plv, wParam, lParam, ListView_OnKey);
				break;

			case WM_CHAR:
				if (plv->iPuntChar)
				{
					plv->iPuntChar--;
					return TRUE;
				}
				else
				{
					return HANDLE_WM_CHAR(plv, wParam, lParam, ListView_OnChar);
				}

			// don't use HANDLE_MSG because this needs to go to the default handler
			case WM_SYSKEYDOWN:
				HANDLE_WM_SYSKEYDOWN(plv, wParam, lParam, ListView_OnKey);
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (pe->RoutedEvent)
			{
				HANDLE_MSG(plv, WM_HSCROLL, ListView_OnHScroll);
				HANDLE_MSG(plv, WM_VSCROLL, ListView_OnVScroll);
				HANDLE_MSG(plv, WM_GETDLGCODE, ListView_OnGetDlgCode);
				HANDLE_MSG(plv, WM_SETFONT, ListView_OnSetFont);
				HANDLE_MSG(plv, WM_GETFONT, ListView_OnGetFont);
				HANDLE_MSG(plv, WM_TIMER, ListView_OnTimer);
				HANDLE_MSG(plv, WM_SETREDRAW, ListView_OnSetRedraw);
			//HANDLE_MSG(plv, WM_COMMAND, ListView_OnCommand);

			case WM_PRINTCLIENT:
			case WM_PAINT:
			case WM_PRINT:
				ListView_OnPaint(plv, (HDC)wParam);
				return 1; // 禁止基类绘制

			case WLM_BIND_STYLE:
				ListView_BindStyle(hWnd, (CListViewProp *)lParam);
				return 0;

			case WM_NOTIFY:
				return ListView_OnNotify(plv, wParam, (LPNMHDR)lParam);

#ifdef UNICODE
			case LVM_GETITEMA:
				return (LRESULT)ListView_OnGetItemA(plv, (LV_ITEMA *)lParam);

			case LVM_SETITEMA:
				return (LRESULT)ListView_OnSetItemA(plv, (LV_ITEMA *)lParam);

			case LVM_INSERTITEMA:
				return (LRESULT)ListView_OnInsertItemA(plv, (LV_ITEMA *)lParam);

			case LVM_FINDITEMA:
				return (LRESULT)ListView_OnFindItemA(plv, (int)wParam, (LV_FINDINFOA *)lParam);

			case LVM_GETSTRINGWIDTHA:
				return (LRESULT)ListView_OnGetStringWidthA(plv, (LPCSTR)lParam, NULL);

			case LVM_GETCOLUMNA:
				return (LRESULT)ListView_OnGetColumnA(plv, (int)wParam, (LV_COLUMNA *)lParam);

			case LVM_SETCOLUMNA:
				return (LRESULT)ListView_OnSetColumnA(plv, (int)wParam, (LV_COLUMNA *)lParam);

			case LVM_INSERTCOLUMNA:
				return (LRESULT)ListView_OnInsertColumnA(plv, (int)wParam, (LV_COLUMNA *)lParam);

			case LVM_GETITEMTEXTA:
				return (LRESULT)ListView_OnGetItemTextA(plv, (int)wParam, (LV_ITEMA *)lParam);

			case LVM_SETITEMTEXTA:
				if (!lParam)
				{
					return FALSE;
				}

				return (LRESULT)ListView_OnSetItemTextA(plv, (int)wParam,
						((LV_ITEMA *)lParam)->iSubItem,
						(LPCSTR)((LV_ITEMA *)lParam)->pszText);

			case LVM_GETBKIMAGEA:
				return (LRESULT)ListView_OnGetBkImageA(plv, (LPLVBKIMAGEA)lParam);

			case LVM_SETBKIMAGEA:
				return (LRESULT)ListView_OnSetBkImageA(plv, (LPLVBKIMAGEA)lParam);

#else

#ifdef DEBUG

			case LVM_GETITEMW:
			case LVM_SETITEMW:
			case LVM_INSERTITEMW:
			case LVM_FINDITEMW:
			case LVM_GETSTRINGWIDTHW:
			case LVM_GETCOLUMNW:
			case LVM_SETCOLUMNW:
			case LVM_INSERTCOLUMNW:
			case LVM_GETITEMTEXTW:
			case LVM_SETITEMTEXTW:
			case LVM_GETBKIMAGEW:
			case LVM_SETBKIMAGEW:
			case LVM_GETISEARCHSTRINGW:
			case LVM_EDITLABELW:
				break;
#endif

#endif

			case WM_HELP:
				return ListView_OnHelp(plv, (LPHELPINFO)lParam);


			case LVM_GETIMAGELIST:
				return (LRESULT)(UINT_PTR)(ListView_OnGetImageList(plv, (int)wParam));

			case LVM_SETIMAGELIST:
				return (LRESULT)(UINT_PTR)ListView_OnSetImageList(plv, (HIMAGELIST)lParam, (int)wParam);

			case LVM_GETBKCOLOR:
				return (LRESULT)(plv->hwnd->ci.style & WS_DISABLED ? plv->clrBkSave : plv->clrBk);

			case LVM_SETBKCOLOR:
				plv->flags |= LVF_USERBKCLR;
				if (plv->hwnd->ci.style & WS_DISABLED)
				{
					plv->clrBkSave = (COLORREF)lParam;
					return TRUE;
				}
				else
				{
					return (LRESULT)ListView_OnSetBkColor(plv, (COLORREF)lParam);
				}

			case LVM_GETTEXTCOLOR:
				return (LRESULT)plv->clrText;
			case LVM_SETTEXTCOLOR:
				plv->clrText = (COLORREF)lParam;
				return TRUE;
			case LVM_GETTEXTBKCOLOR:
				return (LRESULT)plv->clrTextBk;
			case LVM_SETTEXTBKCOLOR:
				plv->clrTextBk = (COLORREF)lParam;
				return TRUE;
			case LVM_GETHOTLIGHTCOLOR:
				return (LRESULT)plv->clrHotlight;
			case LVM_SETHOTLIGHTCOLOR:
				plv->clrHotlight = (COLORREF)lParam;
				return (TRUE);

			case LVM_GETITEMCOUNT:
				if (ListView_IsOwnerData(plv))
				{
					return ((LRESULT)plv->cTotalItems);
				}
				else if (!plv->hdpa)
				{
					return (0);
				}
				else
				{
					return ((LRESULT)DPA_GetPtrCount(plv->hdpa));
				}
				break;

			case LVM_GETITEM:
				return (LRESULT)ListView_OnGetItem(plv, (LVITEMEX *)lParam);

			case LVM_GETITEMSTATE:
				return (LRESULT)ListView_OnGetItemState(plv, (int)wParam, (UINT)lParam);

			case LVM_SETITEMSTATE:
				if (!lParam)
				{
					return FALSE;
				}

				return (LRESULT)ListView_OnSetItemState(plv, (int)wParam,
						((LVITEMEX *)lParam)->state,
						((LVITEMEX *)lParam)->stateMask);

			case LVM_SETITEMTEXT:
				if (!lParam)
				{
					return FALSE;
				}

				return (LRESULT)ListView_OnSetItemText(plv, (int)wParam,
						((LVITEMEX *)lParam)->iSubItem,
						(LPCTSTR)((LVITEMEX *)lParam)->pszText);

			case LVM_GETITEMTEXT:
				return (LRESULT)ListView_OnGetItemText(plv, (int)wParam, (LVITEMEX *)lParam);

			case LVM_GETBKIMAGE:
				return (LRESULT)ListView_OnGetBkImage(plv, (LPLVBKIMAGE)lParam);

			case LVM_SETBKIMAGE:
				return (LRESULT)ListView_OnSetBkImage(plv, (LPLVBKIMAGE)lParam);

			case LVM_SETITEM:
				return (LRESULT)ListView_OnSetItem(plv, (const LVITEMEX *)lParam);

			case LVM_INSERTITEM:
				return (LRESULT)ListView_OnInsertItem(plv, (const LVITEMEX *)lParam);

			case LVM_DELETEITEM:
				return (LRESULT)ListView_OnDeleteItem(plv, (int)wParam);

			case LVM_UPDATE:
				ListView_OnUpdate(plv, (int)wParam);
				UpdateWindow(GetHostWnd(plv->hwnd));
				return TRUE;

			case LVM_DELETEALLITEMS:
				lParam = (LRESULT)ListView_OnDeleteAllItems(plv);
				// Optimization:  Instead of sending out a zillion EVENT_OBJECT_DESTROY's,
				// we send out a destroy of ourselves followed by a fresh create.
				// For compatibility with IE4, we still send out the REORDER notification.
				MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(hWnd), OBJID_CLIENT, 0);
				ListView_NotifyRecreate(plv);
				return (lParam);

			case LVM_GETITEMRECT:
				return (LRESULT)ListView_OnGetItemRect(plv, (int)wParam, (RECT *)lParam);

			case LVM_GETSUBITEMRECT:
				return (LRESULT)ListView_OnGetSubItemRect(plv, (int)wParam, (LPRECT)lParam);

			case LVM_SUBITEMHITTEST:
				return (LRESULT)ListView_OnSubItemHitTest(plv, (LPLVHITTESTINFO)lParam);

#ifdef UNICODE
			case LVM_GETISEARCHSTRINGA:
				if (GetFocus() == GetHostWnd(plv->hwnd))
				{
					return (LRESULT)GetIncrementSearchStringA(&plv->is, plv->hwnd->ci.uiCodePage, (LPSTR)lParam);
				}
				else
				{
					return 0;
				}

#endif

			case LVM_GETISEARCHSTRING:
				if (GetFocus() == GetHostWnd(plv->hwnd))
				{
					return (LRESULT)GetIncrementSearchString(&plv->is, (LPTSTR)lParam);
				}
				else
				{
					return 0;
				}

			case LVM_GETITEMSPACING:
				if (wParam)
				{
					return MAKELONG(plv->cxItem, plv->cyItem);
				}
				else
				{
					return MAKELONG(plv->cxIconSpacing, plv->cyIconSpacing);
				}

			case LVM_GETNEXTITEM:
				return (LRESULT)ListView_OnGetNextItem(plv, (int)wParam, (UINT)lParam);

			case LVM_FINDITEM:
				return (LRESULT)ListView_OnFindItem(plv, (int)wParam, (const LV_FINDINFO *)lParam);

			case LVM_SETSELECTIONMARK:
			{
				int iOldMark = plv->iMark;
				int iNewMark = (int)lParam;
				if (iNewMark == -1 || ListView_IsValidItemNumber(plv, iNewMark))
				{
					plv->iMark = iNewMark;
				}
				return iOldMark;
			}

			case LVM_GETSELECTIONMARK:
				return plv->iMark;

			case LVM_GETITEMPOSITION:
				return (LRESULT)ListView_OnGetItemPosition(plv, (int)wParam,
						(POINT *)lParam);

			case LVM_SETITEMPOSITION:
				return (LRESULT)ListView_OnSetItemPosition(plv, (int)wParam,
						GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			case LVM_SETITEMPOSITION32:
				if (!lParam)
				{
					return FALSE;
				}

				return (LRESULT)ListView_OnSetItemPosition(plv, (int)wParam,
						((LPPOINT)lParam)->x, ((LPPOINT)lParam)->y);

			case LVM_SCROLL:
			{
				int dx = (int)wParam;
				int dy = (int)lParam;
				return (LRESULT)
					(ListView_ValidateScrollParams(plv, &dx, &dy) &&
						ListView_OnScroll(plv, dx, dy));
			}

			case LVM_ENSUREVISIBLE:
				return (LRESULT)ListView_OnEnsureVisible(plv, (int)wParam, BOOLFROMPTR(lParam));

			case LVM_REDRAWITEMS:
				return (LRESULT)ListView_OnRedrawItems(plv, (int)wParam, (int)lParam);

			case LVM_ARRANGE:
				return (LRESULT)ListView_OnArrange(plv, (UINT)wParam);

			case LVM_GETEDITCONTROL:
				return (LRESULT)(UINT_PTR)plv->hwndEdit;

#ifdef UNICODE
			case LVM_EDITLABELA:
			{
				LPWSTR lpEditString = NULL;
				HWND   hRet;

				if (lParam)
				{
					lpEditString = ProduceWFromA(plv->hwnd->ci.uiCodePage, (LPSTR)lParam);
				}

				hRet = ListView_OnEditLabel(plv, (int)wParam, lpEditString);

				if (lpEditString)
				{
					FreeProducedString(lpEditString);
				}

				return (LRESULT)hRet;
			}
#endif

			case LVM_EDITLABEL:
				return (LRESULT)(UINT_PTR)ListView_OnEditLabel(plv, (int)wParam, (LPTSTR)lParam);

			case LVM_HITTEST:
				return (LRESULT)ListView_OnHitTest(plv, (LV_HITTESTINFO *)lParam);

			case LVM_GETSTRINGWIDTH:
				return (LRESULT)ListView_OnGetStringWidth(plv, (LPCTSTR)lParam, NULL);

			case LVM_GETCOLUMN:
				return (LRESULT)ListView_OnGetColumn(plv, (int)wParam, (LV_COLUMN *)lParam);

			case LVM_SETCOLUMN:
				return (LRESULT)ListView_OnSetColumn(plv, (int)wParam, (const LV_COLUMN *)lParam);

			case LVM_SETCOLUMNORDERARRAY:
				return SendMessage(plv->hwndHdr, HDM_SETORDERARRAY, wParam, lParam);

			case LVM_GETCOLUMNORDERARRAY:
				return SendMessage(plv->hwndHdr, HDM_GETORDERARRAY, wParam, lParam);

			case LVM_GETHEADER:
			{
				HWLWND hwndOld = plv->hwndHdr;
				if (lParam != NULL && IsWindow((HWLWND)lParam))
				{
					plv->hwndHdr = (HWLWND)lParam;
				}
				return (LRESULT)hwndOld;
			}

			case LVM_INSERTCOLUMN:
				return (LRESULT)ListView_OnInsertColumn(plv, (int)wParam, (const LV_COLUMN *)lParam);

			case LVM_DELETECOLUMN:
				return (LRESULT)ListView_OnDeleteColumn(plv, (int)wParam);

			case LVM_CREATEDRAGIMAGE:
				return (LRESULT)(UINT_PTR)ListView_OnCreateDragImage(plv, (int)wParam, (LPPOINT)lParam);


			case LVMI_PLACEITEMS:
				if (plv->uUnplaced)
				{
					ListView_Recompute(plv);
					ListView_UpdateScrollBars(plv);
				}
				return 0;

			case LVM_GETVIEWRECT:
				if (!lParam)
				{
					return FALSE;
				}

				ListView_GetViewRect2(plv, (RECT *)lParam, plv->sizeClient.cx, plv->sizeClient.cy);
				return (LPARAM)TRUE;

			case LVM_GETCOLUMNWIDTH:
				return (LPARAM)ListView_OnGetColumnWidth(plv, (int)wParam);

			case LVM_SETCOLUMNWIDTH:
				return (LPARAM)ListView_ISetColumnWidth(plv, (int)wParam,
						GET_X_LPARAM(lParam), TRUE);

			case LVM_SETCALLBACKMASK:
				plv->stateCallbackMask = (UINT)wParam;
				return (LPARAM)TRUE;

			case LVM_GETCALLBACKMASK:
				return (LPARAM)(UINT)plv->stateCallbackMask;

			case LVM_GETTOPINDEX:
				return (LPARAM)ListView_OnGetTopIndex(plv);

			case LVM_GETCOUNTPERPAGE:
				return (LPARAM)ListView_OnGetCountPerPage(plv);

			case LVM_GETORIGIN:
				return (LPARAM)ListView_OnGetOrigin(plv, (POINT *)lParam);

			case LVM_SETITEMCOUNT:
				return ListView_OnSetItemCount(plv, (int)wParam, (DWORD)lParam);

			case LVM_GETSELECTEDCOUNT:
				if (ListView_IsOwnerData(plv))
				{
					//L plv->plvrangeSel->lpVtbl->CountIncluded(plv->plvrangeSel, &plv->nSelected);
				}

				return plv->nSelected;

			case LVM_SORTITEMS:
				return ListView_OnSortItems(plv, (LPARAM)wParam, (PFNLVCOMPARE)lParam, TRUE);

			case LVM_SORTITEMSEX:
				return ListView_OnSortItems(plv, (LPARAM)wParam, (PFNLVCOMPARE)lParam, FALSE);

			case LVM_SETEXTENDEDLISTVIEWSTYLE:
				return ListView_ExtendedStyleChange(plv, (DWORD) lParam, (DWORD) wParam);

			case LVM_GETEXTENDEDLISTVIEWSTYLE:
				return plv->exStyle;

			case LVM_GETHOVERTIME:
				return plv->dwHoverTime;

			case LVM_SETHOVERTIME:
			{
				DWORD dwRet = plv->dwHoverTime;
				plv->dwHoverTime = (DWORD)lParam;
				return dwRet;
			}

			case LVM_GETTOOLTIPS:
				return (LRESULT)plv->hwndToolTips;

			case LVM_SETTOOLTIPS:
			{
				HWND hwndToolTips = plv->hwndToolTips;
				plv->hwndToolTips = (HWND)wParam;
				return (LRESULT)hwndToolTips;
			}

			case LVM_SETICONSPACING:
			{
				DWORD dwRet = ListView_OnSetIconSpacing(plv, lParam);

				// rearrange as necessary
				if (ListView_RedrawEnabled(plv) &&
					((plv->hwnd->ci.style & LVS_AUTOARRANGE) &&
						(ListView_IsSmallView(plv) || ListView_IsIconView(plv))))
				{
					// Call off to the arrange function.
					ListView_OnArrange(plv, LVA_DEFAULT);
				}
				return dwRet;
			}

			case LVM_SETHOTITEM:
			{
				int iOld = plv->iHot;
				int iNew = (int)wParam;
				if (iNew == -1 || ListView_IsValidItemNumber(plv, iNew))
				{
					ListView_OnSetHotItem(plv, (int)wParam);
				}
				return iOld;
			}

			case LVM_GETHOTITEM:
				return plv->iHot;

			// hCurHot is used iff LVS_EX_TRACKSELECT
			case LVM_SETHOTCURSOR:
			{
				HCURSOR hCurOld = plv->hCurHot;
				plv->hCurHot = (HCURSOR)lParam;
				return (LRESULT)hCurOld;
			}

			case LVM_GETHOTCURSOR:
				if (!plv->hCurHot)
				{
					plv->hCurHot = LoadHandCursor(0);
				}
				return (LRESULT)plv->hCurHot;

			case LVM_APPROXIMATEVIEWRECT:
				return ListView_OnApproximateViewRect(plv, (int)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

				//L case LVM_SETLVRANGEOBJECT:
				//	return ListView_OnSetLVRangeObject(plv, (int)wParam, (ILVRange *)lParam);

#ifdef LVM_SETWORKAREAS  // until the headers are in ssync.....
			case LVM_SETWORKAREAS:
				ListView_OnSetWorkAreas(plv, (int)wParam, (RECT *)lParam);
				return 0;

			case LVM_GETWORKAREAS:
				ListView_OnGetWorkAreas(plv, (int)wParam, (RECT *)lParam);
				return 0;

			case LVM_GETNUMBEROFWORKAREAS:
				ListView_OnGetNumberOfWorkAreas(plv, (int *)lParam);
				return 0;

			case LVM_RESETEMPTYTEXT:
				plv->fNoEmptyText = FALSE;
				Str_Set(&plv->pszEmptyText, NULL);
				if (ListView_Count(plv) == 0)
				{
					::InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
				}
				return 1;
#endif

			case WM_GETOBJECT:
				if (lParam == OBJID_QUERYCLASSNAMEIDX)
				{
					return MSAA_CLASSNAMEIDX_LISTVIEW;
				}
				break;

			case WM_QUERYNEWPALETTE:
				// Want to pass FALSE if WM_QUERYNEWPALETTE...
				ListView_Realize(plv, NULL, uMsg == WM_PALETTECHANGED, uMsg == WM_PALETTECHANGED);
				return TRUE;


				//L #if defined(FE_IME) || !defined(WINNT)
				// 	case WM_IME_COMPOSITION:
				// 		// Now only Korean version is interested in incremental search with composition string.
				// 		if (g_fDBCSInputEnabled)
				// 		{
				// 			if (((ULONG_PTR)GetKeyboardLayout(0L) & 0xF000FFFFL) == 0xE0000412L)
				// 			{
				// 				if (ListView_OnImeComposition(plv, wParam, lParam))
				// 				{
				// 					lParam &= ~GCS_RESULTSTR;
				// 					break;
				// 				}
				// 				else
				// 				{
				// 					return 0;
				// 				}
				// 			}
				// 		}
				// 		break;
				// #endif



#ifdef KEYBOARDCUES
			case WM_UPDATEUISTATE:
			{
				DWORD dwUIStateMask = MAKEWPARAM(0xFFFF, UISF_HIDEFOCUS);

				// we care only about focus not accel, and redraw only if changed
				if (CCOnUIState(&(plv->hwnd->ci), WM_UPDATEUISTATE, wParam & dwUIStateMask, lParam))
				{
					if (plv->iFocus >= 0)
					{
						// an item has the focus, invalidate it
						ListView_InvalidateItem(plv, plv->iFocus, FALSE, RDW_INVALIDATE | RDW_ERASE);
					}
				}

				goto DoDefault;
			}
#endif

			// IUI Add
			case LVM_SETITEMHEIGHT:
				plv->cyItem = wParam;
				return 0;

			case LVM_GETITEMHEIGHT:
				return plv->cyItem;

			case LVM_SETGRIDLINECOLOR:
				plv->crGridLine = wParam;
				return 0;

			case LVM_GETGRIDLINECOLOR:
				return plv->crGridLine;

			case LVM_SETGRIDLINEOFFSET:
				if (pe->lParam == 0)
				{
					return -1;
				}
				plv->ptGridLineOffset = *((POINT *)pe->lParam);
				return 0;

			case LVM_GETGRIDLINEOFFSET:
				if (pe->lParam == 0)
				{
					return -1;
				}
				*((POINT *)pe->lParam) = plv->ptGridLineOffset;
				return 0;

			case LVM_SETDRAWFIRSTROWLINE:
				plv->bDrawFirstRowLine = wParam;
				return 0;

			case LVM_ISDRAWFIRSTROWLINE:
				return plv->bDrawFirstRowLine;

			case LVM_SETITEMHEADERSPACE:
				plv->cyItemHeaderSpace = wParam;
				return 0;

			case LVM_GETITEMHEADERSPACE:
				return plv->cyItemHeaderSpace;

			case LVM_ENABLE_SELECTED_FIT_CLIENT:
				plv->m_bEnableSelectedFitClient = lParam;
				return 0;

			case LVM_IS_SELECTED_FIT_CLIENT_ENABLED:
				return plv->m_bEnableSelectedFitClient;

			case WLM_ENABLE_SPEC_ITEM_EFFECT:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);

				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				pitem->pItemSpecBackground->bItemBkEnabled = lParam;
			}
			return 0;

			case WLM_IS_SPEC_ITEM_EFFECT_ENABLED:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return FALSE;
				}

				return pitem->pItemSpecBackground->bItemBkEnabled;
			}
			return FALSE;

			case WLM_SET_SPEC_ITEM_BK_TYPE:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				pitem->pItemSpecBackground->eItemBackgroundType = (BACKGROUND_TYPE)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_TYPE:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return BKT_UNKNOWN;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return BKT_UNKNOWN;
				}

				return pitem->pItemSpecBackground->eItemBackgroundType;
			}
			return BKT_UNKNOWN;

			case WLM_SET_SPEC_ITEM_BK_COLOR:
			{
				int nItem = wParam;
				ITEM_COLOR *pic = (ITEM_COLOR *)lParam;

				if (ListView_IsValidItemNumber(plv, nItem) || pic == NULL)
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pic->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					pitem->pItemSpecBackground->crItemBk[nIndex] = pic->color;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_COLOR:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return 0;
				}
				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return pitem->pItemSpecBackground->crItemBk[nIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_SPEC_ITEM_BK_IMAGE:
			{
				int nItem = wParam;
				ITEM_IMAGE *pii = (ITEM_IMAGE *)lParam;
				if (pii == NULL || ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pii->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					IUISetControlImage(GetProject(hWnd),
						&pitem->pItemSpecBackground->himgItemBk[nIndex], pii->lpszImage);
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return NULL;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return NULL;
				}

				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return (LRESULT)(LPCTSTR)pitem->pItemSpecBackground->himgItemBk[nIndex]->GetSafeImageName();
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				pitem->pItemSpecBackground->eItemBkImageResizeMode = (IMAGE_RESIZE_MODE)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return IRM_UNKNOWN;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return IRM_UNKNOWN;
				}

				return pitem->pItemSpecBackground->eItemBkImageResizeMode;
			}
			return IRM_UNKNOWN;

			case WLM_SET_SPEC_ITEM_BK_IMAGE_9GRID_ARG:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				pitem->pItemSpecBackground->rcItemBkImage9GridArg = (LPCRECT)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE_9GRID_ARG:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return -2;
				}

				*(LPRECT)lParam = pitem->pItemSpecBackground->rcItemBkImage9GridArg;
			}
			return 0;

			case WLM_SET_SPEC_ITEM_TEXT_COLOR:
			{
				int nItem = wParam;
				ITEM_COLOR *pic = (ITEM_COLOR *)lParam;
				if (pic == NULL || ListView_IsValidItemNumber(plv, nItem))
				{
					return -1;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					pitem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pic->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					pitem->pItemSpecBackground->crText[nIndex] = pic->color;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_TEXT_COLOR:
			{
				int nItem = wParam;
				if (ListView_IsValidItemNumber(plv, nItem))
				{
					return 0;
				}

				LISTITEM *pitem = ListView_FastGetItemPtr(plv, nItem);
				if (pitem->pItemSpecBackground == NULL)
				{
					return 0;
				}
				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return pitem->pItemSpecBackground->crText[nIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_ITEM_BK_TYPE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemS[0].m_eBkType = (BACKGROUND_TYPE)lParam;
					}
					else if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						plv->m_lvpItemS[1].m_eBkType = (BACKGROUND_TYPE)lParam;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemH.m_eBkType = (BACKGROUND_TYPE)lParam;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemFocus.m_eBkType = (BACKGROUND_TYPE)lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_BK_TYPE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemS[0].m_eBkType;
					}
					else if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						return plv->m_lvpItemS[1].m_eBkType;
					}
				}
				if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemH.m_eBkType;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemFocus.m_eBkType;
					}
				}
				return -1;

			case WLM_SET_ITEM_BK_COLOR:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemS[0].m_crItemBk = lParam;
					}
					else if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						plv->m_lvpItemS[1].m_crItemBk = lParam;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemH.m_crItemBk = lParam;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemFocus.m_crItemBk = lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_BK_COLOR:
				if (ITEM_SELECTED == wParam)
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemS[0].m_crItemBk;
					}
					if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						return plv->m_lvpItemS[1].m_crItemBk;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemH.m_crItemBk;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemFocus.m_crItemBk;
					}
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = (LPCTSTR)lParam;

				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						IUISetControlImage(GetProject(hWnd),
							&plv->m_lvpItemS[0].m_hItemBkBmp, lpszImage);
					}
					else if (eControlState == CONTROL_STATE_DISABLED)
					{
						IUISetControlImage(GetProject(hWnd),
							&plv->m_lvpItemS[1].m_hItemBkBmp, lpszImage);
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						IUISetControlImage(GetProject(hWnd),
							&plv->m_himgSelectedItemCombineBk, lpszImage);
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						IUISetControlImage(GetProject(hWnd),
							&plv->m_lvpItemH.m_hItemBkBmp, lpszImage);
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						IUISetControlImage(GetProject(hWnd),
							&plv->m_lvpItemFocus.m_hItemBkBmp, lpszImage);
					}
				}
			}
			return 0;

			case WLM_GET_ITEM_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						return (LRESULT)(LPCTSTR)plv->m_lvpItemS[0].m_hItemBkBmp->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_DISABLED)
					{
						return (LRESULT)(LPCTSTR)plv->m_lvpItemS[1].m_hItemBkBmp->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						return (LRESULT)(LPCTSTR)plv->m_himgSelectedItemCombineBk->GetSafeImageName();
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						return (LRESULT)(LPCTSTR)plv->m_lvpItemH.m_hItemBkBmp->GetSafeImageName();
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return (LRESULT)(LPCTSTR)plv->m_lvpItemFocus.m_hItemBkBmp->GetSafeImageName();
					}
				}
			}
			return 0;

			case WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						// TODO: 默认9宫格
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						// TODO: 默认9宫格
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{

					}
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							// 默认9宫格
							return IRM_9GRID;
						}
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							// 默认9宫格
							return IRM_9GRID;
						}
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							return IRM_9GRID;
						}
					}
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					plv->m_lvpItemS[0].m_rcItemBkImage9GridArg = (LPCRECT)lParam;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					plv->m_lvpItemH.m_rcItemBkImage9GridArg = (LPCRECT)lParam;
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					plv->m_lvpItemFocus.m_rcItemBkImage9GridArg = (LPCRECT)lParam;
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (lParam != NULL)
					{
						*(LPRECT)lParam = plv->m_lvpItemS[0].m_rcItemBkImage9GridArg;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (lParam != NULL)
					{
						*(LPRECT)lParam = plv->m_lvpItemH.m_rcItemBkImage9GridArg;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (lParam != NULL)
					{
						*(LPRECT)lParam = plv->m_lvpItemFocus.m_rcItemBkImage9GridArg;
					}
				}

				return 0;

			case WLM_SET_ITEM_TEXT_COLOR:
				if (ITEM_SELECTED == wParam)
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemS[0].m_crItemText = lParam;
					}
					else if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						plv->m_lvpItemS[1].m_crItemText = lParam;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemH.m_crItemText = lParam;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						plv->m_lvpItemFocus.m_crItemText = lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_TEXT_COLOR:
				if (ITEM_SELECTED == wParam)
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemS[0].m_crItemText;
					}
					else if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						return plv->m_lvpItemS[1].m_crItemText;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemH.m_crItemText;
					}
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						return plv->m_lvpItemFocus.m_crItemText;
					}
				}
				return 0;

			case LVM_GET_HIGHLIGHT_ITEM:
				return plv->m_nCurHighlightItem;

			default:
			{
				LRESULT lres;
				if (CCWndProc(&plv->hwnd->ci, uMsg, wParam, lParam, &lres))
				{
					return lres;
				}
			}

				// Special handling of magellan mouse message
			if (uMsg == IUIGetGloabelVar()->g_msgMSWheel)
			{
				BOOL            fScroll;
				BOOL            fDataZoom;
				DWORD           dwStyle;
				int             sb;
				SCROLLINFO      si;
				int             cScrollUnitsPerLine;
				int             cPage;
				int             cLinesPerDetant;
				int             cDetants;
				int             dPos;
				int iWheelDelta;

				//L if (g_bRunOnNT || g_bRunOnMemphis)
				//{
				//	iWheelDelta = (int)(short)HIWORD(wParam);
				//	fScroll = !(wParam & (MK_SHIFT | MK_CONTROL));
				//	fDataZoom = (BOOL)(wParam & MK_SHIFT);
				//}
				//else
				{
					iWheelDelta = (int)wParam;
					fDataZoom = (GetKeyState(VK_SHIFT) < 0);
					fScroll = !fDataZoom && GetKeyState(VK_CONTROL) >= 0;
				}

				// Update count of scroll amount
				IUIGetGloabelVar()->gcWheelDelta -= iWheelDelta;
				cDetants = IUIGetGloabelVar()->gcWheelDelta / WHEEL_DELTA;
				if (cDetants != 0)
				{
					IUIGetGloabelVar()->gcWheelDelta %= WHEEL_DELTA;
				}

				if (fScroll)
				{
					if (IUIGetGloabelVar()->g_ucScrollLines > 0 &&
						cDetants != 0 &&
						((WS_VSCROLL | WS_HSCROLL) & (dwStyle = ListView_GetWindowStyle(plv))))
					{

						sb = (dwStyle & WS_VSCROLL) ? SB_VERT : SB_HORZ;

						// Get the scroll amount of one line
						cScrollUnitsPerLine = _ListView_GetScrollUnitsPerLine(plv, sb);
						_ASSERT(cScrollUnitsPerLine > 0);

						si.cbSize = sizeof(SCROLLINFO);
						si.fMask = SIF_PAGE | SIF_POS;
						if (!ListView_GetScrollInfo(plv, sb, &si))
						{
							return 1;
						}

						// The size of a page is at least one line, and
						// leaves one line of overlap
						cPage = (max(cScrollUnitsPerLine, (int)si.nPage - cScrollUnitsPerLine)) / cScrollUnitsPerLine;

						// Don't scroll more than one page per detant
						cLinesPerDetant = (int) min((ULONG) cPage, (ULONG) IUIGetGloabelVar()->g_ucScrollLines);

						dPos = cLinesPerDetant * cDetants * cScrollUnitsPerLine;

						ListView_DismissEdit(plv, FALSE);
						ListView_ComOnScroll(
							plv, SB_THUMBTRACK, si.nPos + dPos, sb, cScrollUnitsPerLine, - 1);
						ListView_UpdateScrollBars(plv);

						// After scrolling, the tooltip might need to change
						// so send the tooltip a fake mousemove message to force
						// a recompute.  We use WM_NCMOUSEMOVE since our lParam
						// is in screen coordinates, not client coordinates.
						ListView_PopBubble(plv);
						RelayToToolTips(plv->hwndToolTips, GetHostWnd(plv->hwnd),
							WM_NCMOUSEMOVE, HTCLIENT, lParam);
					}
					return 1;
				}
				else if (fDataZoom)
				{
					LV_HITTESTINFO ht;
					ht.pt.x = GET_X_LPARAM(lParam);
					ht.pt.y = GET_Y_LPARAM(lParam);
					ScreenToClient(hWnd, &(ht.pt));

					// If we are rolling forward and we hit an item then navigate
					// into that item (simulate dblclk which will open it).  Otherwise
					// just fall through so it isn't handled.  In that case if we
					// are being hosted in explorer it will do a backwards
					// history navigation.
					if ((iWheelDelta > 0) && (ListView_OnSubItemHitTest(plv, &ht) >= 0) &&
						(ht.flags & LVHT_ONITEM) && cDetants != 0)
					{
						BYTE aKeyState[256];
						// This is a bit yucky but when ListView_HandleMouse sends the
						// notification to the listview owner we need to make sure that
						// it doesn't think the shift key is down.  Otherwise it may
						// perform some "alternate" action but in this case we always
						// want it to perform the default open action.
						//
						// Strip the high bit of VK_SHIFT so that the shift key is
						// not down.
						GetKeyboardState(aKeyState);
						aKeyState[VK_SHIFT] &= 0x7f;
						SetKeyboardState(aKeyState);
						ListView_HandleMouse(plv, FALSE, ht.pt.x, ht.pt.y, 0, TRUE);
						ListView_HandleMouse(plv, TRUE, ht.pt.x, ht.pt.y, 0, TRUE);
						return 1;
					}
					// else fall through
				}
			}

			break;
			}
		}
	}
	// 处理Header发出的事件
	else if (pe->hOriginalSender == _ListView_GetHeader(hWnd))
	{
		if (pe->RoutedEvent == WM_NOTIFY)
		{
			return SendMessage(hWnd, WM_NOTIFY, (WPARAM)pe->hOriginalSender, pe->lParam);
		}
	}

	return DefEventHandler(hWnd, pe);
}

void ListView_OnWinIniChange(SListViewData *plv, WPARAM wParam, LPARAM lParam)
{
	// BUGBUG:  will this also catch sysparametersinfo?
	// we need a general way of handling this, not
	// just relying on the listview.
	InitGlobalMetrics(wParam);

	switch (wParam)
	{
	case 0:
	case SPI_SETNONCLIENTMETRICS:
	case SPI_SETICONTITLELOGFONT:
	case SPI_SETICONMETRICS:
		// If wParam is 0, only reload settings if lParam is 0 too.  This catches the wild-card scenario
		// (like the old plus tab which does WM_WININICHANGE, 0, 0) but allows us to ignore wParam = 0
		// and lParam = lpszSectionName.  Reduces unecessary flashing.
		if (wParam || !lParam)
		{
			if (!(plv->flags & LVF_ICONSPACESET))
			{
				ListView_OnSetIconSpacing(plv, (LPARAM)(-1));
			}

			if (plv->flags & LVF_FONTCREATED)
			{
				ListView_OnSetFont(plv, NULL, TRUE);
			}

			// Force a recalc of all the icon regions by stripping and
			// then adding back the LVS_EX_REGIONAL bit.
			if (plv->exStyle & LVS_EX_REGIONAL)
			{
				ListView_ExtendedStyleChange(plv, 0, LVS_EX_REGIONAL);
				ListView_ExtendedStyleChange(plv, LVS_EX_REGIONAL, LVS_EX_REGIONAL);
			}
		}
		break;

	default:
		break;
	}

	// If we are in an Iconic view and the user is in autoarrange mode,
	// then we need to arrange the items.
	//
	if ((ListView_IsOwnerData(plv) || (plv->hwnd->ci.style & LVS_AUTOARRANGE)) &&
		(ListView_IsSmallView(plv) || ListView_IsIconView(plv)))
	{
		// Call off to the arrange function.
		ListView_OnArrange(plv, LVA_DEFAULT);
	}
}

BOOL ListView_OnCreate(SListViewData *plv, CREATESTRUCT *lpCreateStruct)
{
#ifdef WINNT
	InitDitherBrush();
#endif

#ifdef DEBUG
	if (GetAsyncKeyState(VK_SHIFT) < 0 &&
		GetAsyncKeyState(VK_CONTROL) < 0)
	{
		//plv->exStyle |= LVS_EX_SUBITEMIMAGES;
		plv->exStyle |= LVS_EX_FULLROWSELECT;
		plv->hwnd->ci.style |= LVS_SHOWSELALWAYS;
		SetWindowLong(GetHostWnd(plv->hwnd), GWL_STYLE, plv->hwnd->ci.style);
	}
#endif

	HINSTANCE hInst = NULL;

	plv->dwExStyle = lpCreateStruct->dwExStyle;

	if (plv->hwnd->ci.style & WS_VISIBLE)
	{
		plv->flags |= LVF_VISIBLE;
	}

	ListView_GetRegIASetting(&g_bUseDblClickTimer);

	//L if (ListView_IsOwnerData(plv))
	//{
	//	// ownerdata initialization
	//	plv->plvrangeSel = LVRange_Create();
	//	if (NULL == plv->plvrangeSel)
	//	{
	//		goto error0;
	//	}

	//	plv->plvrangeCut = LVRange_Create();
	//	if (NULL == plv->plvrangeCut)
	//	{
	//		goto error0;
	//	}
	//}
	//else
	{
		//L _ASSERT(plv->plvrangeSel == NULL);
		//_ASSERT(plv->plvrangeCut == NULL);

		plv->hdpa = IUI_DPA_CreateEx(LV_HDPA_GROW, plv->hheap);
		if (!plv->hdpa)
		{
			goto error0;
		}

		plv->hdpaZOrder = IUI_DPA_CreateEx(LV_HDPA_GROW, plv->hheap);
		if (!plv->hdpaZOrder)
		{
			goto error1;
		}
	}

	_ASSERT(plv->nWorkAreas == 0);
	_ASSERT(plv->prcWorkAreas == NULL);
	plv->iNoHover = -1;
	plv->dwHoverTime = HOVER_DEFAULT;
	plv->iHot = -1;
	plv->iEdit = -1;
	plv->iFocus = -1;
	plv->iDrag = -1;
	plv->iTTLastHit = -1;
	plv->iFreeSlot = -1;
	plv->rcView.left = RECOMPUTE;
	_ASSERT(plv->iMSAAMin == plv->iMSAAMax);

	plv->sizeClient.cx = lpCreateStruct->cx;
	plv->sizeClient.cy = lpCreateStruct->cy;

	// Setup flag to say if positions are in small or large view
	if (ListView_IsSmallView(plv))
	{
		plv->flags |= LVF_ICONPOSSML;
	}

	// force calculation of listview metrics
	ListView_OnSetFont(plv, NULL, FALSE);

	plv->cxItem = 16 * plv->cxLabelChar + plv->cxSmIcon;

	// if we're in ownerdraw report mode, the size got saved to cyItemSave
	// at creation time, both need to have this
	if ((plv->hwnd->ci.style & LVS_OWNERDRAWFIXED) && ListView_IsReportView(plv))
	{
		plv->cyItem = plv->cyItemSave;
	}
	else
	{
		plv->cyItemSave = plv->cyItem;
	}

	ListView_OnSetIconSpacing(plv, (LPARAM)(-1));

	ListView_UpdateScrollBars(plv);     // sets plv->cItemCol

	plv->clrBk = CLR_NONE;
	plv->clrText = CLR_DEFAULT;
	plv->clrTextBk = CLR_DEFAULT;
	plv->clrHotlight = CLR_DEFAULT;

	// create the bk brush, and set the imagelists colors if needed
	ListView_OnSetBkColor(plv, GetSysColor(COLOR_WINDOW));

	// Initialize report view fields
	plv->xTotalColumnWidth = RECOMPUTE;

	if (plv->hwnd->ci.style & WS_DISABLED)
	{
		plv->hwnd->ci.style &= ~WS_DISABLED;
		ListView_EnableWindow(plv, FALSE);
	}

	// tooltip for unfolding name lables
	hInst = (HINSTANCE)GetWindowLong(GetHostWnd(plv->hwnd), GWL_HINSTANCE);
	plv->hwndToolTips = CreateWindow(TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_NOPREFIX, 0, 0, 0, 0,
			GetHostWnd(plv->hwnd), NULL, hInst, NULL);
	if (NULL != plv->hwndToolTips)
	{
		SetProp(plv->hwndToolTips, _T("GETPROP_TOOLTIPPARENT_LISTVIEW"), plv->hwnd);

		TOOLINFO ti;

		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_TRANSPARENT;
		ti.hwnd = GetHostWnd(plv->hwnd);
		ti.uId = 0;
		ti.hinst = NULL;
		ti.lpszText = LPSTR_TEXTCALLBACK;

		GetClientRect(plv->hwnd, &ti.rect);
		SendMessage(plv->hwndToolTips, TTM_ADDTOOL, 0, (LPARAM) &ti);

		/* Ensure that the tooltips use the same font as the view */
		FORWARD_WM_SETFONT(plv->hwndToolTips, plv->hfontLabel, FALSE, SendMessage);
	}
	_ASSERT(plv->hwndToolTips);

	_ASSERT(FALSE == plv->fOneClickOK);
	IUI::SetTimer(plv->hwnd, IDT_ONECLICKOK, GetDoubleClickTime(), NULL);

	return TRUE;

error1:
	IUI_DPA_Destroy(plv->hdpa);
error0:
	//L if (plv->plvrangeSel)
	// 	{
	// 		plv->plvrangeSel->lpVtbl->Release(plv->plvrangeSel);
	// 	}
	// 	if (plv->plvrangeCut)
	// 	{
	// 		plv->plvrangeCut->lpVtbl->Release(plv->plvrangeCut);
	// 	}

	return FALSE;
}

void PASCAL ListView_DeleteHrgnInval(SListViewData *plv)
{
	if (plv->hrgnInval && plv->hrgnInval != (HRGN)ENTIRE_REGION)
	{
		DeleteObject(plv->hrgnInval);
	}
	plv->hrgnInval = NULL;
}

void ListView_OnDestroy(SListViewData *plv)
{
	//
	// The tooltip window may or may not exist at this point.  It
	// depends if the owning window of the tips is also being destroy.
	// If so, then the tips are gone already.
	//

	if (IsWindow(plv->hwndToolTips))
	{
		DestroyWindow(plv->hwndToolTips);
	}

	if (plv->hCurHot)
	{
		DestroyCursor(plv->hCurHot);
	}

	plv->hwndToolTips = NULL;

	Str_Set(&plv->pszTip, NULL);
	Str_Set(&plv->pszEmptyText, NULL);

#ifdef WINNT
	TerminateDitherBrush();
#endif

	if (!ListView_IsOwnerData(plv))
	{
		// Make sure to notify the app
		ListView_OnDeleteAllItems(plv);
	}

	if ((plv->flags & LVF_FONTCREATED) && plv->hfontLabel)
	{
		DeleteObject(plv->hfontLabel);
		// plv->flags &= ~LVF_FONTCREATED;
		// plv->hwfontLabel = NULL;
	}
	if (plv->hFontHot)
	{
		DeleteObject(plv->hFontHot);
	}
	ListView_DeleteHrgnInval(plv);

	if (plv->prcWorkAreas)
	{
		// This assert is bogus: If the app created work areas then deleted
		// them, nWorkAreas will be 0 but prcWorkAreas will be non-NULL.
		// _ASSERT(plv->nWorkAreas > 0);
		LocalFree(plv->prcWorkAreas);
	}
}

void ListView_OnNCDestroy(SListViewData *plv)
{

	if ((!(plv->hwnd->ci.style & LVS_SHAREIMAGELISTS)) || ListView_CheckBoxes(plv))
	{

		if (plv->himlState &&
			(plv->himlState != plv->himl) &&
			(plv->himlState != plv->himlSmall))
		{
			ImageList_Destroy(plv->himlState);
		}
	}

	if (!(plv->hwnd->ci.style & LVS_SHAREIMAGELISTS))
	{
		if (plv->himl)
		{
			ImageList_Destroy(plv->himl);
		}
		if (plv->himlSmall)
		{
			ImageList_Destroy(plv->himlSmall);
		}
	}

	//L if (ListView_IsOwnerData(plv))
	//{
	//	plv->plvrangeSel->lpVtbl->Release(plv->plvrangeSel);
	//	plv->plvrangeCut->lpVtbl->Release(plv->plvrangeCut);
	//	plv->cTotalItems = 0;
	//}

	ListView_ReleaseBkImage(plv);

	if (plv->hbrBk)
	{
		DeleteBrush(plv->hbrBk);
	}

	if (plv->hdpa)
	{
		IUI_DPA_Destroy(plv->hdpa);
	}

	if (plv->hdpaZOrder)
	{
		IUI_DPA_Destroy(plv->hdpaZOrder);
	}

	ListView_RDestroy(plv);

	IncrementSearchFree(&plv->is);

	ListView_SetPtr(GetHostWnd(plv->hwnd), NULL);
	NearFree(plv);
}


// sets the background color for the listview
//
// this creats the brush for drawing the background as well
// as sets the imagelists background color if needed

BOOL ListView_OnSetBkColor(SListViewData *plv, COLORREF clrBk)
{
	if (plv->clrBk != clrBk)
	{
		if (plv->hbrBk)
		{
			DeleteBrush(plv->hbrBk);
			plv->hbrBk = NULL;
		}

		if (clrBk != CLR_NONE)
		{
			plv->hbrBk = CreateSolidBrush(clrBk);
			if (!plv->hbrBk)
			{
				return FALSE;
			}
		}

		// don't mess with the imagelist color if things are shared

		if (!(plv->hwnd->ci.style & LVS_SHAREIMAGELISTS))
		{

			if (plv->himl)
			{
				ImageList_SetBkColor(plv->himl, clrBk);
			}

			if (plv->himlSmall)
			{
				ImageList_SetBkColor(plv->himlSmall, clrBk);
			}

			if (plv->himlState)
			{
				ImageList_SetBkColor(plv->himlState, clrBk);
			}
		}

		plv->clrBk = clrBk;
	}
	return TRUE;
}

void PASCAL InitBrushOrg(SListViewData *plv, HDC hdc)
{
	int x;
	if (ListView_IsSmallView(plv) || ListView_IsIconView(plv))
	{
		x = plv->ptOrigin.x;
	}
	else if (ListView_IsListView(plv))
	{
		x = plv->xOrigin;
	}
	else
	{
		x = (int)plv->ptlRptOrigin.x;
	}
	SetBrushOrgEx(hdc, -x, 0, NULL);
}

void PASCAL ListView_InvalidateRegion(SListViewData *plv, HRGN hrgn)
{
	if (hrgn)
	{
		if (plv->hrgnInval == NULL)
		{
			plv->hrgnInval = hrgn;
		}
		else
		{

			// union it in if the entire region isn't marked for invalidate
			if (plv->hrgnInval != (HRGN)ENTIRE_REGION)
			{
				UnionRgn(plv->hrgnInval, plv->hrgnInval, hrgn);
			}
			DeleteObject(hrgn);
		}
	}
}


// ----------------------------------------------------------------------------
//
//  LVSeeThruScroll
//
//  Used when a watermark is the listview's background (detected via clrTextBk
//  being CLR_NONE) to perform a flicker-free scroll of the client area, using
//  an offscreen bitmap
//
//  potential perf issue -- caching DC and/or bitmap instead of create/destroy
//                          on each call
//
//  jeffbog 2/29/96
//
// ----------------------------------------------------------------------------

void LVSeeThruScroll(SListViewData *plv, LPRECT lprcUpdate)
{
	HDC     hdcOff;
	HBITMAP hbmpOff;
	int     x, y, cx, cy;
	HDC     hdc = GetDC(GetHostWnd(plv->hwnd));

	if (!lprcUpdate)
	{
		x = y = 0;
		cx = plv->sizeClient.cx;
		cy = plv->sizeClient.cy;
	}
	else
	{
		x  = lprcUpdate->left;
		y  = lprcUpdate->top;
		cx = lprcUpdate->right - x;
		cy = lprcUpdate->bottom - y;
	}

	hdcOff  = CreateCompatibleDC(hdc);
	hbmpOff = CreateCompatibleBitmap(hdc, plv->sizeClient.cx, plv->sizeClient.cy);
	SelectObject(hdcOff, hbmpOff);

	SendMessage(GetHostWnd(plv->hwnd), WM_PRINT, (WPARAM)hdcOff, PRF_CLIENT | PRF_ERASEBKGND);
	BitBlt(hdc, x, y, cx, cy, hdcOff, x, y, SRCCOPY);
	ReleaseDC(GetHostWnd(plv->hwnd), hdc);
	DeleteDC(hdcOff);
	DeleteObject(hbmpOff);
}

void ListView_OnPaint(SListViewData *plv, HDC hdc)
{
	if (plv == NULL || hdc == NULL)
	{
		_ASSERT(FALSE);
		return;
	}

	RECT rcPaint;
	RECT rcUpdate;

	BeforePaint(plv->hwnd, hdc);

	// Before handling WM_PAINT, go ensure everything's recomputed...
	//
	if (plv->rcView.left == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	// 如果是Report View，更新header，但由于我们是windowless控件
	// 调用InvalidateRect(plv->hwndHdr, NULL)会导致Host窗口的WM_PAINT，
	// 而Host窗口的WM_PAINT又会调用到本函数引起死循环，所以不再刷新Header
	// 	if (ListView_IsReportView(plv) && plv->hwndHdr)
	// 	{
	// 		InvalidateRect(plv->hwndHdr, NULL);
	// 	}

	// If nothing to do (i.e., we recieved a WM_PAINT because
	// of an RDW_INTERNALPAINT, and we didn't invalidate anything)
	// don't bother with the Begin/EndPaint.
	//
	if (hdc || GetUpdateRect(GetHostWnd(plv->hwnd), &rcUpdate, FALSE))
	{
		if (!(plv->flags & LVF_VISIBLE))
		{
			plv->flags |= LVF_VISIBLE;
			// We may try to resize the column
			ListView_MaybeResizeListColumns(plv, 0, ListView_Count(plv) - 1);
			ListView_UpdateScrollBars(plv);
		}

		// this needs to be done before the beginpaint because it clears
		// out the update region
		if (!(plv->flags & LVF_REDRAW))
		{
			// add this region to our local invalidate region
			HRGN hrgn = CreateRectRgn(0, 0, 0, 0);
			if (hrgn)
			{

				// ok if GetUpdateRgn fails... then hrgn will still be
				// and empty region..
				GetUpdateRgn(GetHostWnd(plv->hwnd), hrgn, FALSE);
				ListView_InvalidateRegion(plv, hrgn);
			}
		}

		InitBrushOrg(plv, hdc);

		SetRect(&rcPaint, 0, 0, plv->sizeClient.cx, plv->sizeClient.cy);
		if (ListView_IsReportView(plv))
		{
			if (!IsIncludeFlag(GetStyle(plv->hwnd), LVS_NOCOLUMNHEADER))
			{
				HWLWND hHeader = _ListView_GetHeader(plv->hwnd);
				CIUIRect rcHeader;
				GetWindowRectToParent(hHeader, rcHeader);
				rcPaint.top += rcHeader.Height();
			}
		}

		if (ListView_RedrawEnabled(plv))
		{
			ListView_Redraw(plv, hdc, &rcPaint);
		}
	}

	AfterPaint(plv->hwnd, hdc);

#ifdef _DEBUG
	DrawFocusRect(hdc, &rcPaint);
#endif
}

void ListView_DrawBackground(SListViewData *plv, HDC hdc, RECT *prcClip)
{
	HRGN hrgnClipSave;
	RECT rcClip;

	// Optimize the common/simple case
	if (!(plv->pImgCtx && plv->fImgCtxComplete))
	{
		return;
	}

	//
	// Save the old clipping region,
	// since we whack on it a lot.
	//
	hrgnClipSave = CreateRectRgnIndirect(prcClip);
	if (hrgnClipSave)
	{
		if (GetClipRgn(hdc, hrgnClipSave) <= 0)
		{
			DeleteObject(hrgnClipSave);
			hrgnClipSave = NULL;
		}
	}

	//
	// Clip the clipping region to the caller's rectangle,
	// and save the final clipping rectangle in rcClip.
	//
	if (prcClip != NULL)
	{
		IntersectClipRect(hdc, prcClip->left, prcClip->top,
			prcClip->right, prcClip->bottom);
	}
	GetClipBox(hdc, &rcClip);

	//
	// If we have an image to draw, go draw it and
	// exclue it from the clipping region.
	//
	if (plv->pImgCtx && plv->fImgCtxComplete)
	{
		RECT rcImage, rcClient;
		SIZE sizeImg;
		POINT ptBackOrg;

		//
		// Compute ptBackOrg (aka scrolling offset), based on view style.
		//
		switch (plv->hwnd->ci.style & LVS_TYPEMASK)
		{
		case LVS_LIST:
			ptBackOrg.x = -plv->xOrigin;
			ptBackOrg.y = 0;
			break;

		case LVS_REPORT:
			ptBackOrg.x = -plv->ptlRptOrigin.x;
			ptBackOrg.y = -plv->ptlRptOrigin.y + plv->yTop;
			break;

		default:
			ptBackOrg.x = -plv->ptOrigin.x;
			ptBackOrg.y = -plv->ptOrigin.y;
			break;
		}

		ListView_Realize(plv, hdc, TRUE, FALSE);

		switch (plv->ulBkImageFlags & LVBKIF_STYLE_MASK)
		{
		case LVBKIF_STYLE_TILE:
			//L IImgCtx_Tile(plv->pImgCtx, hdc, &ptBackOrg, prcClip, NULL);
			ExcludeClipRect(hdc, prcClip->left, prcClip->top,
				prcClip->right, prcClip->bottom);
			break;

#if 0
		case LVBKIF_STYLE_STRETCH:
			//
			// Stretch the image to the extents of the client & view areas.
			//
			GetClientRect(plv->hwnd, &rcImage);
			UnionRect(&rcImage, &rcImage, &plv->rcView);

			//
			// Adjust for ptBackOrg (scrolling offset).
			//
			rcImage.left += ptBackOrg.x;
			rcImage.top += ptBackOrg.y;
			rcImage.right += ptBackOrg.x;
			rcImage.bottom += ptBackOrg.y;

			//
			// Draw the image, if necessary.
			//
			if (RectVisible(hdc, &rcImage))
			{
				IImgCtx_Draw(plv->pImgCtx, hdc, &rcImage);
				ExcludeClipRect(hdc, prcClip->left, prcClip->top,
					prcClip->right, prcClip->bottom);
			}
			break;
#endif
		case LVBKIF_STYLE_NORMAL:
			//
			// Start with the base image.
			//
			//L IImgCtx_GetStateInfo(plv->pImgCtx, &ulState, &sizeImg, FALSE);
			sizeImg.cx = 16;
			sizeImg.cy = 16;
			rcImage.left = 0;
			rcImage.top = 0;
			rcImage.right = sizeImg.cx;
			rcImage.bottom = sizeImg.cy;

			//
			// Adjust for caller offsets.
			//
			GetClientRect(plv->hwnd, &rcClient);
			if (plv->xOffsetPercent)
			{
				LONG dx = plv->xOffsetPercent * (rcClient.right - sizeImg.cx) / 100;

				rcImage.left += dx;
				rcImage.right += dx;
			}
			if (plv->yOffsetPercent)
			{
				LONG dy = plv->yOffsetPercent * (rcClient.bottom - sizeImg.cy) / 100;

				rcImage.top += dy;
				rcImage.bottom += dy;
			}

			//
			// Adjust for ptBackOrg (scrolling offset).
			//
			rcImage.left += ptBackOrg.x;
			rcImage.top += ptBackOrg.y;
			rcImage.right += ptBackOrg.x;
			rcImage.bottom += ptBackOrg.y;

			//
			// Draw the image, if necessary.
			//
			if (RectVisible(hdc, &rcImage))
			{
				//L IImgCtx_Draw(plv->pImgCtx, hdc, &rcImage);
				ExcludeClipRect(hdc, rcImage.left, rcImage.top,
					rcImage.right, rcImage.bottom);
			}
			break;
		}
	}

	//
	// Restore old clipping region.
	//
	SelectClipRgn(hdc, hrgnClipSave);
	if (hrgnClipSave)
	{
		DeleteObject(hrgnClipSave);
	}
}

void ListView_OnCommand(SListViewData *plv, int id, HWND hwndCtl, UINT codeNotify)
{
	if (hwndCtl == plv->hwndEdit)
	{
		switch (codeNotify)
		{
		case EN_UPDATE:
#if defined(FE_IME) || !defined(WINNT)
			// We don't want flicker during replacing current selection
			// as we use selection for IME composition.
			//
			if ((IUIGetGloabelVar()->g_fDBCSInputEnabled) && (plv->flags & LVF_INSERTINGCOMP))
			{
				break;
			}
#endif
			// We will use the ID of the window as a Dirty flag...
			if (IsWindowVisible(plv->hwndEdit))
			{
				SetWindowID(plv->hwndEdit, 1);
				ListView_SetEditSize(plv);
			}
			break;

		case EN_KILLFOCUS:
			// We lost focus, so dismiss edit and save changes
			// (Note that the owner might reject the change and restart
			// edit mode, which traps the user.  Owners need to give the
			// user a way to get out.)
			//
#if 0       // BUGBUG raymondc v6.0
			//
			//  Fix horrible undocumented hanging problem:  LVN_ENDLABELEDIT
			//  is sent in response to EN_KILLFOCUS, which is send in response
			//  to WM_KILLFOCUS, and it is undocumented that you cannot display
			//  UI during WM_KILLFOCUS when a journal record hook is active,
			//  because the presence of a hook forces serialization of activation,
			//  and so when you put up UI, you generate activation changes, which
			//  get stuck because you haven't finished responding to the previous
			//  WM_KILLFOCUS message yet.
			//
			//  See NT bug 414634.
			//
			if (InSendMessage())
			{
				ReplyMessage(0);
			}
#endif
			if (!ListView_DismissEdit(plv, FALSE))
			{
				return;
			}
			break;

		case HN_BEGINDIALOG:  // pen windows is bringing up a dialog
			_ASSERT(GetSystemMetrics(SM_PENWINDOWS)); // only on a pen system
			plv->fNoDismissEdit = TRUE;
			break;

		case HN_ENDDIALOG: // pen windows has destroyed dialog
			_ASSERT(GetSystemMetrics(SM_PENWINDOWS)); // only on a pen system
			plv->fNoDismissEdit = FALSE;
			break;
		}

		// Forward edit control notifications up to parent
		//
		if (IsWindow(hwndCtl))
		{
			RoutedEventArgs arg;
			arg.hSender = plv->hwnd;
			arg.hOriginalSender = plv->hwnd;
			arg.RoutedEvent = codeNotify;
			arg.eRoutingStrategy = RS_BUBBLE;
			RaiseEvent(arg.hOriginalSender, &arg);

			//L FORWARD_WM_COMMAND(plv->hwnd->ci.hwndParent, id, hwndCtl, codeNotify, SendMessage);
		}
	}
}

void ListView_OnWindowPosChanged(SListViewData *plv, const WINDOWPOS *lpwpos)
{
	if (!lpwpos || !(lpwpos->flags & SWP_NOSIZE))
	{
		RECT rc;

		int iOldSlots;

		if (ListView_IsOwnerData(plv) &&
			(ListView_IsSmallView(plv) || ListView_IsIconView(plv)))
		{
			iOldSlots = ListView_GetSlotCount(plv, TRUE);
		}

		GetClientRect(plv->hwnd, &rc);
		plv->sizeClient.cx = rc.right;
		plv->sizeClient.cy = rc.bottom;

		if ((plv->hwnd->ci.style & LVS_AUTOARRANGE) &&
			(ListView_IsSmallView(plv) || ListView_IsIconView(plv)))
		{
			// Call off to the arrange function.
			ListView_OnArrange(plv, LVA_DEFAULT);
		}

		if (ListView_IsOwnerData(plv))
		{
			plv->rcView.left = RECOMPUTE;
			ListView_Recompute(plv);

			ListView_DismissEdit(plv, FALSE);
			if (ListView_IsSmallView(plv) || ListView_IsIconView(plv))
			{
				// Uses the
				int iNewSlots = ListView_GetSlotCount(plv, TRUE);
				if ((iNewSlots != iOldSlots) && (ListView_Count(plv) > min(iNewSlots, iOldSlots)))
				{
					RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
				}
			}
		}

		// Always make sure the scrollbars are updated to the new size
		ListView_UpdateScrollBars(plv);
	}
}


//L void ListView_InvalidateSelectedOrCutOwnerData(LV *plv, ILVRange *plvrangeSel)
// {
// 	UINT rdwFlags = RDW_INVALIDATE;
// 	int cItem = ListView_Count(plv);
// 	DWORD dwType = plv->hwnd->ci.style & LVS_TYPEMASK;
// 	int i;
// 	RECT rcView;
//
// 	_ASSERT(ListView_IsOwnerData(plv));
// 	_ASSERT(plv);
//
// 	GetClientRect(plv->hwnd, &rcView);
//
// 	if (plv->clrTextBk == CLR_NONE
// 		|| (plv->himl && (plv->clrBk != ImageList_GetBkColor(plv->himl))))
// 	{
// 		// always do an erase, otherwise the text background won't paint right
// 		rdwFlags |= RDW_ERASE;
// 	}
//
// 	// calculate start of items and end of items visible on the view
// 	//
// 	switch (dwType)
// 	{
// 	case LVS_REPORT:
// 		i = ListView_RYHitTest(plv, rcView.top);
// 		cItem = ListView_RYHitTest(plv, rcView.bottom) + 1;
// 		break;
//
// 	case LVS_LIST:
// 		i = ListView_LCalcViewItem(plv, rcView.left, rcView.top);
// 		cItem = ListView_LCalcViewItem(plv, rcView.right, rcView.bottom) + 1;
// 		break;
//
// 	default:
// 		ListView_CalcMinMaxIndex(plv, &rcView, &i, &cItem);
// 		break;
// 	}
//
// 	i = max(i, 0);
//
// 	cItem = min(ListView_Count(plv), cItem);
// 	if (cItem > i)
// 	{
// 		ListView_NotifyCacheHint(plv, i, cItem - 1);
// 	}
//
// 	for (; i < cItem; i++)
// 	{
// 		if (plvrangeSel->lpVtbl->IsSelected(plvrangeSel, i) == S_OK)
// 		{
// 			ListView_InvalidateItem(plv, i, FALSE, rdwFlags);
// 		}
// 	}
// }

void ListView_RedrawSelection(SListViewData *plv)
{

	if (ListView_IsOwnerData(plv))
	{
		//L ListView_InvalidateSelectedOrCutOwnerData(plv, plv->plvrangeSel);

	}
	else
	{

		int i = -1;

		while ((i = ListView_OnGetNextItem(plv, i, LVNI_SELECTED)) != -1)
		{
			ListView_InvalidateItemEx(plv, i, plv->iSubItemEditLabel, TRUE, RDW_INVALIDATE, 0);
		}

		if (ListView_IsReportView(plv))
		{
			int iEnd = ListView_RYHitTest(plv, plv->sizeClient.cy) + 1;

			iEnd = min(iEnd, ListView_Count(plv));

			// if we're in report mode, sub items may have selection focus
			for (i = ListView_RYHitTest(plv, 0); i < iEnd; i++)
			{
				int iCol;

				for (iCol = 1; iCol < plv->cCol; iCol++)
				{
					LISTSUBITEM lsi;
					ListView_GetSubItem(plv, i, iCol, &lsi);
					if (lsi.state & LVIS_SELECTED)
					{
						ListView_InvalidateItem(plv, i, FALSE, RDW_INVALIDATE);
					}
					break;
				}
			}
		}
	}

	UpdateWindow(GetHostWnd(plv->hwnd));
}

void ListView_OnSetFocus(SListViewData *plv, HWLWND hwndOldFocus)
{
	_ASSERT(IUIGetGloabelVar()->gcWheelDelta == 0);

	// due to the way listview call SetFocus on themselves on buttondown,
	// the window can get a strange sequence of focus messages: first
	// set, then kill, and then set again.  since these are not really
	// focus changes, ignore them and only handle "real" cases.
	//
	// But still send out the accessibility notification because USER
	// has already pushed focus back to the listview instead of to the
	// focus item.

	if (hwndOldFocus == plv->hwnd)
	{
		ListView_NotifyFocusEvent(plv);
		return;
	}

	plv->flags |= LVF_FOCUSED | LVF_UNFOLDED;
	if (IsWindowVisible(GetHostWnd(plv->hwnd)))
	{
		if (plv->iFocus != -1)
		{
			ListView_InvalidateItem(plv, plv->iFocus, TRUE, RDW_INVALIDATE);
			ListView_NotifyFocusEvent(plv);
		}

		ListView_RedrawSelection(plv);
	}

	// Let the parent window know that we are getting the focus.
	CCSendNotify(&plv->hwnd->ci, NM_SETFOCUS, NULL);
}

void ListView_OnKillFocus(SListViewData *plv, HWND hwndNewFocus)
{
	// Reset wheel scroll amount
	IUIGetGloabelVar()->gcWheelDelta = 0;

	// due to the way listview call SetFocus on themselves on buttondown,
	// the window can get a strange sequence of focus messages: first
	// set, then kill, and then set again.  since these are not really
	// focus changes, ignore them and only handle "real" cases.
	if (!plv || hwndNewFocus == GetHostWnd(plv->hwnd))
	{
		return;
	}

	plv->flags &= ~(LVF_FOCUSED | LVF_UNFOLDED);

	// Blow this off if we are not currently visible (being destroyed!)
	if (IsWindowVisible(plv->hwnd))
	{
		if (plv->iFocus != -1)
		{
			UINT fRedraw = RDW_INVALIDATE;
			if (plv->clrTextBk == CLR_NONE)
			{
				fRedraw |= RDW_ERASE;
			}
			//ListView_InvalidateFoldedItem(plv, plv->iFocus, TRUE, fRedraw);
		}
		ListView_RedrawSelection(plv);
	}

	// Let the parent window know that we are losing the focus.
	CCSendNotify(&plv->hwnd->ci, NM_KILLFOCUS, NULL);
	IncrementSearchString(&plv->is, 0, NULL);
}

void ListView_DeselectAll(SListViewData *plv, int iDontDeselect)
{
	int i = -1;
	int nSkipped = 0;
	BOOL fWasSelected = FALSE;

	if (iDontDeselect != -1)
	{
		if (ListView_OnGetItemState(plv, iDontDeselect, LVIS_SELECTED))
		{
			fWasSelected = TRUE;
		}
	}

	if (ListView_IsOwnerData(plv))
	{

		// if there's only one item selected, and that item is the iDontDeselect
		// then our work is done...
		//L plv->plvrangeSel->lpVtbl->CountIncluded(plv->plvrangeSel, &plv->nSelected);
		//if (plv->nSelected == 1 && fWasSelected)
		//{
		//	return;
		//}

		//ListView_InvalidateSelectedOrCutOwnerData(plv, plv->plvrangeSel);

		//ListView_OnSetItemState(plv, -1, 0, LVIS_SELECTED);
		//if (fWasSelected)
		//{
		//	ListView_OnSetItemState(plv, iDontDeselect, LVIS_SELECTED, LVIS_SELECTED);
		//	nSkipped = 1;
		//}

	}
	else
	{

		if (iDontDeselect != plv->iFocus)
		{
			ListView_OnSetItemState(plv, plv->iFocus, 0, LVIS_SELECTED);
		}

		while ((plv->nSelected - nSkipped) && (i = ListView_OnGetNextItem(plv, i, LVNI_SELECTED)) != -1)
		{
			if (i != iDontDeselect)
			{
				ListView_OnSetItemState(plv, i, 0, LVIS_SELECTED);
			}
			else
			{
				if (fWasSelected)
				{
					nSkipped++;
				}
			}
		}
	}

	_ASSERT((plv->nSelected - nSkipped) == 0);
	plv->nSelected = nSkipped;
}

// toggle the selection state of an item

void ListView_ToggleSelection(SListViewData *plv, int iItem)
{
	UINT cur_state;
	if (iItem != -1)
	{
		cur_state = ListView_OnGetItemState(plv, iItem, LVIS_SELECTED);
		ListView_OnSetItemState(plv, iItem, cur_state ^ LVIS_SELECTED, LVIS_SELECTED);
	}
}

// Selects (or toggles) a range of items in the list.
//      The curent iFocus is the starting location
//      iItem - is the ending item
//      fToggle - Well set all of the selection state of all of the items to
//          inverse the starting location
//
void ListView_SelectRangeTo(SListViewData *plv, int iItem, BOOL fResetRest)
{
	int iMin, iMax;
	int i = -1;
	UINT uSelVal = LVIS_SELECTED;


	if (plv->iMark == -1)
	{
		ListView_SetFocusSel(plv, iItem, TRUE, TRUE, FALSE);
		return;
	}

	if (!fResetRest)
	{
		uSelVal = ListView_OnGetItemState(plv, plv->iMark, LVIS_SELECTED);
	}

	// If we are in report view or list view we simply walk through the
	// indexes to see which items to select or deselect. otherwise it
	// is is based off of the location of the objects being within the
	// rectangle that is defined by
	if (ListView_IsListView(plv) || ListView_IsReportView(plv))
	{
		iMin = min(iItem, plv->iMark);
		iMax = max(iItem, plv->iMark);

		if (ListView_IsOwnerData(plv))
		{

			//L if (fResetRest)
			//{
			//	ListView_DeselectAll(plv, -1);
			//}

			//if (iMax > iMin)
			//{
			//	if (LVIS_SELECTED & uSelVal)
			//	{
			//		if (FAILED(plv->plvrangeSel->lpVtbl->IncludeRange(plv->plvrangeSel, iMin, iMax)))
			//		{
			//			return;
			//		}
			//	}
			//	else
			//	{
			//		if (FAILED(plv->plvrangeSel->lpVtbl->ExcludeRange(plv->plvrangeSel, iMin, iMax)))
			//		{
			//			return;
			//		}
			//	}
			//	ListView_SendODChangeAndInvalidate(plv, iMin, iMax, uSelVal ^ LVIS_SELECTED, uSelVal);
			//}
			//else
			//{
			//	ListView_OnSetItemState(plv, iMin, uSelVal, LVIS_SELECTED);
			//}

		}
		else
		{
			if (fResetRest)
			{
				while ((i = ListView_OnGetNextItem(plv, i, LVNI_SELECTED)) != -1)
				{
					if (i < iMin || i > iMax)
					{
						ListView_OnSetItemState(plv, i, 0, LVIS_SELECTED);
					}
				}
			}

			while (iMin <= iMax)
			{
				ListView_OnSetItemState(plv, iMin, uSelVal, LVIS_SELECTED);
				iMin++;
			}
		}
	}
	else
	{
		// Iconic views first calculate the bounding rectangle of the two
		// items.
		RECT    rcTemp;
		RECT    rcTemp2;
		RECT    rcBounding;
		POINT   pt;         //

		ListView_GetRects(plv, plv->iMark, NULL, NULL, NULL, &rcTemp);
		ListView_GetRects(plv, iItem, NULL, NULL, NULL, &rcTemp2);
		UnionRect(&rcBounding, &rcTemp, &rcTemp2);

		// since ownerdata icon views are always arranged, we can assume that
		// all items are in order and we can search only those between the
		// indexes found using the bouding rect
		//
		if (ListView_IsOwnerData(plv))
		{
			ListView_CalcMinMaxIndex(plv, &rcBounding, &iMin, &iMax);
			if (fResetRest)
			{
				ListView_DeselectAll(plv, -1);
			}

			iMax = min(iMax, ListView_Count(plv));
			iMin = max(iMin, 0);

		}
		else
		{
			iMax = ListView_Count(plv);
			iMin = 0;

		}

		if (ListView_IsOwnerData(plv)  && (iMax > iMin))
		{
			//L if (LVIS_SELECTED & uSelVal)
			//{
			//	if (FAILED(plv->plvrangeSel->lpVtbl->IncludeRange(plv->plvrangeSel, iMin, iMax - 1)))
			//	{
			//		return;
			//	}
			//}
			//else
			//{
			//	if (FAILED(plv->plvrangeSel->lpVtbl->ExcludeRange(plv->plvrangeSel, iMin, iMax - 1)))
			//	{
			//		return;
			//	}
			//}

			//ListView_SendODChangeAndInvalidate(plv, iMin, iMax, uSelVal ^ LVIS_SELECTED, uSelVal);

		}
		else
		{

			for (i = iMin; i < iMax; i++)
			{
				ListView_GetRects(plv, i, NULL, NULL, NULL, &rcTemp2);
				pt.x = (rcTemp2.right + rcTemp2.left) / 2;  // center of item
				pt.y = (rcTemp2.bottom + rcTemp2.top) / 2;

				if (PtInRect(&rcBounding, pt))
				{
					int iZ;

					if (!ListView_IsOwnerData(plv))
					{
						iZ = ListView_ZOrderIndex(plv, i);

						if (iZ > 0)
						{
							IUI_DPA_InsertPtr(plv->hdpaZOrder, 0, IUI_DPA_DeletePtr(plv->hdpaZOrder, iZ));
						}
					}

					ListView_OnSetItemState(plv, i, uSelVal, LVIS_SELECTED);
				}
				else if (fResetRest)
				{
					ListView_OnSetItemState(plv, i, 0, LVIS_SELECTED);
				}
			}
		}
	}
}

// makes an item the focused item and optionally selects it
//
// in:
//      iItem           item to get the focus
//      fSelectAlso     select this item as well as set it as the focus
//      fDeselectAll    deselect all items first
//      fToggleSel      toggle the selection state of the item
//
// returns:
//      index of focus item (if focus change was refused)

// Bugbug::this is getting to have a lot of parameters
int ListView_SetFocusSel(SListViewData *plv, int iItem, BOOL fSelectAlso,
	BOOL fDeselectAll, BOOL fToggleSel)
{
	int iFocus = plv->iFocus;

	// if we're single sel mode, don't bother with this because
	// the set item will do it for us
	if (!(plv->hwnd->ci.style & LVS_SINGLESEL) && (fDeselectAll))
	{
		ListView_DeselectAll(plv, -1);
	}

	if (iItem != plv->iFocus)
	{
		// remove the old focus
		if (plv->iFocus != -1)
		{
			// If he refuses to give up the focus, bail out.
			if (!ListView_OnSetItemState(plv, plv->iFocus, 0, LVIS_FOCUSED))
			{
				return plv->iFocus;
			}
		}
	}

	if (!ListView_IsOwnerData(plv))
	{

		if (fSelectAlso)
		{
			if (ListView_IsIconView(plv) || ListView_IsSmallView(plv))
			{
				int iZ = ListView_ZOrderIndex(plv, iItem);

				if (iZ > 0)
				{
					IUI_DPA_InsertPtr(plv->hdpaZOrder, 0, IUI_DPA_DeletePtr(plv->hdpaZOrder, iZ));
				}
			}
		}
	}

	/* Ensure that when moving focus that we refresh the previous focus
	 owner properly. */

	if (iFocus != -1 && iFocus != plv->iFocus && (plv->flags & LVF_UNFOLDED))
	{
		ListView_InvalidateFoldedItem(plv, iFocus, FALSE, RDW_INVALIDATE);
	}

	if (plv->iMark == -1)
	{
		plv->iMark = iItem;
	}

	IUI::SetTimer(plv->hwnd, IDT_SCROLLWAIT, GetDoubleClickTime(), NULL);
	plv->flags |= LVF_SCROLLWAIT;

	if (fToggleSel)
	{
		ListView_ToggleSelection(plv, iItem);
		ListView_OnSetItemState(plv, iItem, LVIS_FOCUSED, LVIS_FOCUSED);
	}
	else
	{
		UINT flags;

		flags = ((fSelectAlso || plv->hwnd->ci.style & LVS_SINGLESEL) ?
				(LVIS_SELECTED | LVIS_FOCUSED) : LVIS_FOCUSED);
		ListView_OnSetItemState(plv, iItem, flags, flags);
	}

	return iItem;
}

UINT GetLVKeyFlags()
{
	UINT uFlags = 0;

	if (GetKeyState(VK_MENU) < 0)
	{
		uFlags |= LVKF_ALT;
	}
	if (GetKeyState(VK_CONTROL) < 0)
	{
		uFlags |= LVKF_CONTROL;
	}
	if (GetKeyState(VK_SHIFT) < 0)
	{
		uFlags |= LVKF_SHIFT;
	}

	return uFlags;
}

void ListView_OnKey(SListViewData *plv, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	UINT lvni = 0;
	int iNewFocus;
	BOOL fCtlDown;
	BOOL fShiftDown;
	LV_KEYDOWN nm;
	HWND hwnd = GetHostWnd(plv->hwnd);

	if (!fDown)
	{
		return;
	}

	// Swap the left and right arrow key if the control is mirrored.
	vk = RTLSwapLeftRightArrows(&plv->hwnd->ci, vk);

	//prevent any change in selected items before the dbl click timer goes off
	//so that we don't launch wrong item(s)
	if (plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickHappened && plv->fOneClickOK)
	{
		//if a key is pressed with a mouse click with one click activate and double click
		//timer, we end up setting up a timer and then processing the keydown
		//this causes an item to be launched right away (from this code) and in case
		//of return being pressed it causes double activation
		//prevent these cases:
		if (vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU || vk == VK_RETURN)
		{
			return;
		}
		IUI::KillTimer(plv->hwnd, IDT_ONECLICKHAPPENED);
		plv->fOneClickHappened = FALSE;
		CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &(plv->nmOneClickHappened.hdr));
		if (!IsWindow(hwnd))
		{
			return;
		}
	}

	// Notify
	nm.wVKey = (WORD) vk;
	nm.flags = flags;
	if (CCSendNotify(&plv->hwnd->ci, LVN_KEYDOWN, &nm.hdr))
	{
		plv->iPuntChar++;
		return;
	}
	else if (plv->iPuntChar)
	{
		// this is tricky...  if we want to punt the char, just increment the
		// count.  if we do NOT, then we must clear the queue of WM_CHAR's
		// this is to preserve the iPuntChar to mean "punt the next n WM_CHAR messages
		MSG msg;
		while (plv->iPuntChar && PeekMessage(&msg, GetHostWnd(plv->hwnd), WM_CHAR, WM_CHAR, PM_REMOVE))
		{
			plv->iPuntChar--;
		}
		_ASSERT(!plv->iPuntChar);
	}

	if (ListView_Count(plv) == 0)   // don't blow up on empty list
	{
		return;
	}

	fCtlDown = GetKeyState(VK_CONTROL) < 0;
	fShiftDown = GetKeyState(VK_SHIFT) < 0;

	switch (vk)
	{
	case VK_SPACE:
#ifdef DEBUG
		if (fCtlDown && fShiftDown)
		{
			SendMessage(GetHostWnd(plv->hwnd), LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
				(SendMessage(GetHostWnd(plv->hwnd), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0) ^ LVS_EX_GRIDLINES) | LVS_EX_CHECKBOXES);
		}
#endif

		// If shift (extend) or control (disjoint) select,
		// then toggle selection state of focused item.
		if (fCtlDown)
		{
			plv->iMark = plv->iFocus;
			ListView_ToggleSelection(plv, plv->iFocus);
			plv->iPuntChar++;
		}

		// BUGBUG: Implement me
		if (fShiftDown)
		{
			ListView_SelectRangeTo(plv, plv->iFocus, TRUE);
		}

		if (ListView_CheckBoxes(plv))
		{
			if (plv->iFocus != -1)
			{
				ListView_HandleStateIconClick(plv, plv->iFocus);
			}
		}
#ifdef KEYBOARDCUES
		//notify of navigation key usage
		CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
		return;
	case VK_RETURN:
#ifdef DEBUG
		if (fCtlDown && fShiftDown)
		{
			if (plv->iFocus != -1)
			{
				LVITEMEX lvi;
				lvi.iSubItem = 1;
				lvi.iItem = plv->iFocus;
				lvi.iImage = 3;
				lvi.state = LVIS_SELECTED;
				lvi.stateMask = LVIS_SELECTED;
				lvi.mask = LVIF_STATE | LVIF_IMAGE;
				SendMessage(GetHostWnd(plv->hwnd), LVM_SETITEM, 0, (LPARAM)&lvi);
			}
			return;
		}
#endif
		CCSendNotify(&plv->hwnd->ci, NM_RETURN, NULL);

		/// some (comdlg32 for example) destroy on double click
		// we need to bail if that happens because plv is no longer valid
		if (!IsWindow(hwnd))
		{
			return;
		}

		{
			NMITEMACTIVATE nm;

			nm.iItem = plv->iFocus;
			nm.iSubItem = 0;
			nm.uChanged = 0;
			nm.ptAction.x = -1;
			nm.ptAction.y = -1;
			nm.uKeyFlags = GetLVKeyFlags();
			CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &nm.hdr);
			if (!IsWindow(hwnd))
			{
				return;
			}
		}
#ifdef KEYBOARDCUES
		//notify of navigation key usage
		CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
		return;

	case VK_ADD:
		if (ListView_IsReportView(plv) && (GetKeyState(VK_CONTROL) < 0))
		{
			HCURSOR hcurPrev;
			int i;

			hcurPrev = SetCursor(LoadCursor(NULL, IDC_WAIT));
			for (i = 0; i < plv->cCol; i++)
			{
				ListView_RSetColumnWidth(plv, i, -1);
			}

			SetCursor(hcurPrev);
#ifdef KEYBOARDCUES
			//notify of navigation key usage
			CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
			return;
		}
	}

	if (GetKeyState(VK_MENU) < 0)
	{
		return;
	}

	// For a single selection listview, disable extending the selection
	// by turning off the keyboard modifiers.
	if (plv->hwnd->ci.style & LVS_SINGLESEL)
	{
		fCtlDown = FALSE;
		fShiftDown = FALSE;
	}

	//
	// Let the Arrow function attempt to process the key.
	//
	iNewFocus = ListView_Arrow(plv, plv->iFocus, vk);

	// If control (disjoint) selection, don't change selection.
	// If shift (extend) or control selection, don't deselect all.
	//
	if (iNewFocus != -1)
	{
		if (fShiftDown)
		{
			ListView_SelectRangeTo(plv, iNewFocus, TRUE);
			ListView_SetFocusSel(plv, iNewFocus, FALSE, FALSE, FALSE);
		}
		else
		{
			if (!fCtlDown)
			{
				plv->iMark = iNewFocus;
			}
			ListView_SetFocusSel(plv, iNewFocus, !fCtlDown, !fShiftDown && !fCtlDown, FALSE);
		}
		IncrementSearchString(&plv->is, 0, NULL);
		CCPlaySound(IUIGetGloabelVar()->c_szSelect);
	}

	// on keyboard movement, scroll immediately.
	if (ListView_CancelScrollWait(plv))
	{
		ListView_OnEnsureVisible(plv, plv->iFocus, FALSE);
		UpdateWindow(GetHostWnd(plv->hwnd));
	}
#ifdef KEYBOARDCUES
	//notify of navigation key usage
	CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
}

//
//  LVN_INCREMENTALSEARCH gives the app the opportunity to customize
//  incremental search.  For example, if the items are numeric,
//  the app can do numerical search instead of string search.
//
//  App sets pnmfi->lvfi.lParam to the result of the incremental search,
//  or to -2 to fai the search and just beep.
//
//  App can return 2 to indicate that all processing should stop, if
//  app wants to take over incremental search completely.
//
BOOL ListView_IncrementalSearch(SListViewData *plv, int iStartFrom, LPNMLVFINDITEM pnmfi, int *pi)
{
	INT_PTR fRc;

	_ASSERT(!(pnmfi->lvfi.flags & LVFI_PARAM));
	pnmfi->lvfi.lParam = -1;

	fRc = CCSendNotify(&plv->hwnd->ci, LVN_INCREMENTALSEARCH, &pnmfi->hdr);
	*pi = (int)pnmfi->lvfi.lParam;

	// Cannot just return fRc because some apps return 1 to all WM_NOTIFY's
	return fRc == 2;
}

#if defined(FE_IME)
// Now only Korean version is interested in incremental search with composition string.
LPTSTR GET_COMP_STRING(HIMC hImc, DWORD dwFlags)
{
	LONG iNumComp;
	PTSTR pszCompStr;
	iNumComp = ImmGetCompositionString(hImc, dwFlags, NULL, 0);
	pszCompStr = (PTSTR)LocalAlloc(LPTR, sizeof(TCHAR) * (iNumComp + 1));
	if (pszCompStr)
	{
		if (iNumComp)
		{
			ImmGetCompositionString(hImc, dwFlags, pszCompStr, iNumComp + 1);
		}
		pszCompStr[iNumComp] = TEXT('\0');
	}
	return pszCompStr;
}

#define FREE_COMP_STRING(pszCompStr)    LocalFree((HLOCAL)(pszCompStr))

BOOL ListView_OnImeComposition(SListViewData *plv, WPARAM wParam, LPARAM lParam)
{
	LPTSTR lpsz;
	NMLVFINDITEM nmfi;
	int i;
	int iStartFrom = -1;
	int iLen;
	int iCount;
	HIMC hImc;
	TCHAR *pszCompStr;
	BOOL fRet = TRUE;

	iCount = ListView_Count(plv);

	if (!iCount || plv->iFocus == -1)
	{
		return fRet;
	}

	if (hImc = ImmGetContext(GetHostWnd(plv->hwnd)))
	{
		if (lParam & GCS_RESULTSTR)
		{
			fRet = FALSE;
			pszCompStr = GET_COMP_STRING(hImc, GCS_RESULTSTR);
			if (pszCompStr)
			{
				IncrementSearchImeCompStr(&plv->is, FALSE, pszCompStr, &lpsz);
				FREE_COMP_STRING(pszCompStr);
			}
		}
		if (lParam & GCS_COMPSTR)
		{
			fRet = TRUE;
			pszCompStr = GET_COMP_STRING(hImc, GCS_COMPSTR);
			if (pszCompStr)
			{
				if (IncrementSearchImeCompStr(&plv->is, TRUE, pszCompStr, &lpsz))
				{
					iStartFrom = plv->iFocus;
				}
				else
				{
					iStartFrom = ((plv->iFocus - 1) + iCount) % iCount;
				}

				nmfi.lvfi.flags = LVFI_SUBSTRING | LVFI_STRING | LVFI_WRAP;
				nmfi.lvfi.psz = lpsz;
				iLen = lstrlen(lpsz);

				// special case space as the first character
				if ((iLen == 1) && (*lpsz == TEXT(' ')))
				{
					if (plv->iFocus != -1)
					{
						ListView_OnSetItemState(plv, plv->iFocus, LVIS_SELECTED, LVIS_SELECTED);
						IncrementSearchString(&plv->is, 0, NULL);
					}
#ifdef KEYBOARDCUES
					//notify of navigation key usage
					CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
					return fRet;
				}

				// Give caller full string in case they want to do something custom
				if (ListView_IncrementalSearch(plv, iStartFrom, &nmfi, &i))
				{
					return fRet;
				}

#ifdef UNICODE
				if (iLen > 0 && SameChars(lpsz, lpsz[0]))
				{
#else
				if (iLen > 1 && SameDBCSChars(lpsz, (WORD)((BYTE)lpsz[0] << 8 | (BYTE)lpsz[1])))
				{
#endif

					//
					//  The user has been typing the same char over and over again.
					//  Switch from incremental search to Windows 3.1 style search.
					//
					iStartFrom = plv->iFocus;
					nmfi.lvfi.psz = lpsz + iLen - 1;
				}

				if (i == -1)
				{
					i = ListView_OnFindItem(plv, iStartFrom, &nmfi.lvfi);
				}

				if (!ListView_IsValidItemNumber(plv, i))
				{
					i = -1;
				}

#ifdef LVDEBUG
				DebugMsg(TF_LISTVIEW, TEXT("CIme listsearch %d %s %d"), (LPTSTR)lpsz, (LPTSTR)lpsz, i);
#endif

				if (i != -1)
				{
					ListView_SetFocusSel(plv, i, TRUE, TRUE, FALSE);
					plv->iMark = i;
					if (ListView_CancelScrollWait(plv))
					{
						ListView_OnEnsureVisible(plv, i, FALSE);
					}
				}
				else
				{
					// Don't beep on spaces, we use it for selection.
					IncrementSearchBeep(&plv->is);
				}

#ifdef KEYBOARDCUES
				//notify of navigation key usage
				CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
				FREE_COMP_STRING(pszCompStr);
			}
		}
		ImmReleaseContext(GetHostWnd(plv->hwnd), hImc);
	}
	return fRet;
}

#ifndef UNICODE
BOOL SameDBCSChars(LPSTR lpsz,  WORD w)
{
	while (*lpsz)
	{
		if (IsDBCSLeadByte(*lpsz) == FALSE)
		{
			return FALSE;
		}
		if ((WORD)((BYTE)*lpsz++ << 8 | (BYTE)*lpsz++) != w)
		{
			return FALSE;
		}
	}
	return TRUE;
}
#endif
#endif FE_IME

// REVIEW: We will want to reset ichCharBuf to 0 on certain conditions,
// such as: focus change, ENTER, arrow key, mouse click, etc.
//
void ListView_OnChar(SListViewData *plv, UINT ch, int cRepeat)
{
	LPTSTR lpsz = NULL;
	NMLVFINDITEM nmfi;
	int i;
	int iStartFrom = -1;
	int iLen;
	int iCount;

	iCount = ListView_Count(plv);

	if (!iCount || plv->iFocus == -1)
	{
		return;
	}

	// Don't search for chars that cannot be in a file name (like ENTER and TAB)
	// BUGBUG raymondc fix for v6: The Polish keyboard layout uses CTRL+ALT to
	// enter some normal letters, so don't punt if the CTRL key is down or
	// people in Poland are screwed!
	if (ch < TEXT(' ') || GetKeyState(VK_CONTROL) < 0)
	{
		IncrementSearchString(&plv->is, 0, NULL);
		return;
	}

	if (IncrementSearchString(&plv->is, ch, &lpsz))
	{
		iStartFrom = plv->iFocus;
	}
	else
	{
		iStartFrom = ((plv->iFocus - 1) + iCount) % iCount;
	}

	nmfi.lvfi.flags = LVFI_SUBSTRING | LVFI_STRING | LVFI_WRAP;
	nmfi.lvfi.psz = lpsz;
	iLen = lstrlen(lpsz);

	// special case space as the first character
	if ((iLen == 1) && (*lpsz == ' '))
	{
		if (plv->iFocus != -1)
		{
			ListView_OnSetItemState(plv, plv->iFocus, LVIS_SELECTED, LVIS_SELECTED);
			IncrementSearchString(&plv->is, 0, NULL);
		}
#ifdef KEYBOARDCUES
		//notify of navigation key usage
		CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
		return;
	}

	// Give caller full string in case they want to do something custom
	if (ListView_IncrementalSearch(plv, iStartFrom, &nmfi, &i))
	{
		return;
	}

	if (iLen > 0 && SameChars(lpsz, lpsz[0]))
	{
		//
		//  The user has been typing the same char over and over again.
		//  Switch from incremental search to Windows 3.1 style search.
		//
		iStartFrom = plv->iFocus;
		nmfi.lvfi.psz = lpsz + iLen - 1;
	}

	if (i == -1)
	{
		i = ListView_OnFindItem(plv, iStartFrom, &nmfi.lvfi);
	}

	if (!ListView_IsValidItemNumber(plv, i))
	{
		i = -1;
	}

#ifdef LVDEBUG
	DebugMsg(TF_LISTVIEW, TEXT("listsearch %d %s %d"), (LPTSTR)lpsz, (LPTSTR)lpsz, i);
#endif

	if (i != -1)
	{
		ListView_SetFocusSel(plv, i, TRUE, TRUE, FALSE);
		plv->iMark = i;
		if (ListView_CancelScrollWait(plv))
		{
			ListView_OnEnsureVisible(plv, i, FALSE);
		}
	}
	else
	{
		// Don't beep on spaces, we use it for selection.
		IncrementSearchBeep(&plv->is);
	}

#ifdef KEYBOARDCUES
	//notify of navigation key usage
	CCNotifyNavigationKeyUsage(&(plv->hwnd->ci), UISF_HIDEFOCUS);
#endif
}

UINT ListView_OnGetDlgCode(SListViewData *plv, MSG *lpmsg)
{
	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void ListView_InvalidateCachedLabelSizes(SListViewData *plv)
{
	int i;

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	// Label wrapping has changed, so we need to invalidate the
	// size of the items, such that they will be recomputed.
	//
	if (!ListView_IsOwnerData(plv))
	{
		for (i = ListView_Count(plv) - 1; i >= 0; i--)
		{
			LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);
			ListView_SetSRecompute(pitem);
		}
	}
	plv->rcView.left = RECOMPUTE;

	if ((plv->hwnd->ci.style & LVS_OWNERDRAWFIXED) && ListView_IsReportView(plv))
	{
		plv->cyItemSave = plv->cyItem;
	}
}


int LV_GetNewColWidth(SListViewData *plv, int iFirst, int iLast);

void ListView_OnStyleChanging(SListViewData *plv, UINT gwl, LPSTYLESTRUCT pinfo)
{
	if (gwl == GWL_STYLE)
	{
		// Don't allow LVS_OWNERDATA to change after creation
		DWORD stylePreserve = LVS_OWNERDATA;

		// Don't allow a LVS_EX_REGIONAL listview to change type, since
		// it must be LVS_ICON
		if (plv->exStyle & LVS_EX_REGIONAL)
		{
			stylePreserve |= LVS_TYPEMASK;
		}

		// Preserve the bits that must be preserved
		pinfo->styleNew ^= (pinfo->styleNew ^ pinfo->styleOld) & stylePreserve;
	}
}

void ListView_OnStyleChanged(SListViewData *plv, UINT gwl, LPSTYLESTRUCT pinfo)
{
	// Style changed: redraw everything...
	//
	// try to do this smartly, avoiding unnecessary redraws
	if (gwl == GWL_STYLE)
	{
		BOOL fRedraw = FALSE, fShouldScroll = FALSE;
		DWORD changeFlags, styleOld;

		ListView_DismissEdit(plv, FALSE);   // BUGBUG:  FALSE == apply edits.  Is this correct?

		// 按位异或，相异为1，相同为0
		changeFlags = pinfo->styleOld ^ pinfo->styleNew;
		styleOld = pinfo->styleOld;

		// (dli) Setting the small icon width here and only in the case when we go
		// from large icon view to some other view because of three reasons:
		// 1. According to chee, we want to set this before we change the style bit in
		// plv or after we scale.
		// 2. We don't want to do it after we scale because we want to set the width to
		// the maximum value so that the items in this listview do not cover each other
		// 3. we do it from large icon view because large icon view has fixed width for
		// each item, small icon view width can be scaled.
		//
		if ((changeFlags & LVS_TYPEMASK) && ((styleOld & LVS_TYPEMASK) == LVS_ICON))
			ListView_ISetColumnWidth(plv, 0,
				LV_GetNewColWidth(plv, 0, ListView_Count(plv) - 1), FALSE);

		plv->hwnd->ci.style = pinfo->styleNew;        // change our version

		if (changeFlags & (WS_BORDER | WS_CAPTION | WS_THICKFRAME))
		{
			// the changing of these bits affect the size of the window
			// but not until after this message is handled
			// so post ourself a message.
			PostMessage(GetHostWnd(plv->hwnd), LVMP_WINDOWPOSCHANGED, 0, 0);
		}

		if (changeFlags & LVS_NOCOLUMNHEADER)
		{
			if (plv->hwndHdr)
			{
				DWORD dwValue = (plv->hwnd->ci.style & LVS_NOCOLUMNHEADER) ? HDS_HIDDEN : 0;
				DWORD dwNewStyle = (GetStyle(plv->hwnd) & ~HDS_HIDDEN) | (dwValue & HDS_HIDDEN);
				SetStyle(plv->hwnd, dwNewStyle);

				fRedraw = TRUE;
				fShouldScroll = TRUE;
			}
		}


		if (changeFlags & LVS_NOLABELWRAP)
		{
			ListView_InvalidateCachedLabelSizes(plv);
			fShouldScroll = TRUE;
			fRedraw = TRUE;
		}

		if (changeFlags & LVS_TYPEMASK)
		{
			ListView_TypeChange(plv, styleOld);
			fShouldScroll = TRUE;
			fRedraw = TRUE;
		}

		if ((changeFlags & LVS_AUTOARRANGE) && (plv->hwnd->ci.style & LVS_AUTOARRANGE))
		{
			ListView_OnArrange(plv, LVA_DEFAULT);
			fRedraw = TRUE;
		}

		// bugbug, previously, this was the else to
		// (changeFlags & LVS_AUTOARRANGE && (plv->hwnd->ci.style & LVS_AUTOARRANGE))
		// I'm not sure that was really the right thing..
		if (fShouldScroll)
		{
			// Else we would like to make the most important item to still
			// be visible.  So first we will look for a cursorered item
			// if this fails, we will look for the first selected item,
			// else we will simply ask for the first item (assuming the
			// count > 0
			//
			int i;

			// And make sure the scrollbars are up to date Note this
			// also updates some variables that some views need
			ListView_UpdateScrollBars(plv);

			i = (plv->iFocus >= 0) ? plv->iFocus : ListView_OnGetNextItem(plv, -1, LVNI_SELECTED);
			if ((i == -1)  && (ListView_Count(plv) > 0))
			{
				i = 0;
			}

			if (i != -1)
			{
				ListView_OnEnsureVisible(plv, i, TRUE);
			}

		}

		if (fRedraw)
		{
			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
	}
	else if (gwl == GWL_EXSTYLE)
	{
		//
		// If the RTL_MIRROR extended style bit had changed, let's
		// repaint the control window.
		//
		if ((plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW) != (pinfo->styleNew & RTL_MIRRORED_WINDOW))
		{
			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}

		//
		// Save the new ex-style bits
		//
		plv->hwnd->ci.dwExStyle = pinfo->styleNew;
	}


	// Change of styles also changes tooltip policy, so pop it
	ListView_PopBubble(plv);

}

void ListView_TypeChange(SListViewData *plv, DWORD styleOld)
{
	RECT rc;

	//
	//  Invalidate all cached string metrics because customdraw clients
	//  may draw differently depending on the type.  This happens more
	//  often than you might think, not on purpose, but because apps are
	//  buggy.
	//
	//  APP COMPAT!  You'd think this was completely safe.  After all,
	//  all we're doing is invalidating our cache so we ask the parent
	//  afresh the next time we need the strings.  But noooooooo,
	//  Outlook98 will FAULT if you ask it for information that it thinks
	//  you by all rights already know.  Sigh.  So guard this with a v5.
	//
	if (plv->hwnd->ci.iVersion >= 5 && !ListView_IsOwnerData(plv))
	{
		int i;
		for (i = 0; i < ListView_Count(plv); i++)
		{
			LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);
			ListView_SetSRecompute(pitem);
		}
	}

	switch (styleOld & LVS_TYPEMASK)
	{
	case LVS_REPORT:
		ShowWindow(plv->hwndHdr, SW_HIDE);
		if (styleOld & LVS_OWNERDRAWFIXED)
		{
			// swap cyItem and cyFixed;
			int temp = plv->cyItem;
			plv->cyItem = plv->cyItemSave;
			plv->cyItemSave = temp;
		}
		break;

	default:
		break;
	}

	// Now handle any special setup needed for the new view
	switch (plv->hwnd->ci.style & LVS_TYPEMASK)
	{
	case (UINT)LVS_ICON:
		ListView_ScaleIconPositions(plv, FALSE);
		break;

	case (UINT)LVS_SMALLICON:
		ListView_ScaleIconPositions(plv, TRUE);
		break;

	case (UINT)LVS_LIST:
		// We may need to resize the columns
		ListView_MaybeResizeListColumns(plv, 0, ListView_Count(plv) - 1);
		break;

	case (UINT)LVS_REPORT:
		// if it's owner draw fixed, we may have to do funky stuff
		if ((styleOld & LVS_TYPEMASK) != LVS_REPORT)
		{
			plv->cyItemSave = plv->cyItem;
		}
		break;

	default:
		break;
	}

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	GetClientRect(plv->hwnd, &rc);
	plv->sizeClient.cx = rc.right;
	plv->sizeClient.cy = rc.bottom;
}

int ListView_OnHitTest(SListViewData *plv, LV_HITTESTINFO *pinfo)
{
	UINT flags;
	int x, y;

	if (!pinfo)
	{
		return -1;
	}

	x = pinfo->pt.x;
	y = pinfo->pt.y;

	pinfo->iItem = -1;
	flags = 0;
	if (x < 0)
	{
		flags |= LVHT_TOLEFT;
	}
	else if (x >= plv->sizeClient.cx)
	{
		flags |= LVHT_TORIGHT;
	}
	if (y < 0)
	{
		flags |= LVHT_ABOVE;
	}
	else if (y >= plv->sizeClient.cy)
	{
		flags |= LVHT_BELOW;
	}

	if (flags == 0)
	{
		pinfo->iItem = _ListView_ItemHitTest(plv, x, y, &flags, NULL);
	}

	pinfo->flags = flags;

	if (pinfo->iItem >= ListView_Count(plv))
	{
		pinfo->iItem = -1;
		pinfo->flags = LVHT_NOWHERE;
	}
	return pinfo->iItem;
}

int ScrollAmount(int large, int iSmall, int unit)
{

	return (((large - iSmall) + (unit - 1)) / unit) * unit;
}

// NOTE: this is duplicated in shell32.dll
//
// checks to see if we are at the end position of a scroll bar
// to avoid scrolling when not needed (avoid flashing)
//
// in:
//      code        SB_VERT or SB_HORZ
//      bDown       FALSE is up or left
//                  TRUE  is down or right
BOOL PASCAL CanScroll(SListViewData *plv, int code, BOOL bDown)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

	if (ListView_GetScrollInfo(plv, code, &si))
	{
		if (bDown)
		{
			if (si.nPage)
			{
				si.nMax -= si.nPage - 1;
			}
			return si.nPos < si.nMax;
		}
		else
		{
			return si.nPos > si.nMin;
		}
	}
	else
	{
		return FALSE;
	}
}

// detect if we should auto scroll the window
//
// in:
//      pt  cursor pos in hwnd's client coords
// out:
//      pdx, pdy ammount scrolled in x and y
//
// REVIEW, this should make sure a certain amount of time has passed
// before scrolling.

void ScrollDetect(SListViewData *plv, POINT pt, int *pdx, int *pdy)
{
	int dx, dy;

	*pdx = *pdy = 0;

	if (!(plv->hwnd->ci.style & (WS_HSCROLL | WS_VSCROLL)))
	{
		return;
	}

	dx = dy = plv->cyIcon / 16;
	if (ListView_IsReportView(plv))
	{
		dy = plv->cyItem;       // we scroll in units of items...
		if (!dx)
		{
			dx = plv->cxSmIcon;
		}
	}
	if (ListView_IsListView(plv))
	{
		dx = plv->cxItem;
	}

	if (!dx)
	{
		dx = 1;
	}

	if (!dy)
	{
		dy = 1;
	}

	// we need to check if we can scroll before acutally doing it
	// since the selection rect is adjusted based on how much
	// we scroll by

	if (plv->hwnd->ci.style & WS_VSCROLL)   // scroll vertically?
	{

		if (pt.y >= plv->sizeClient.cy)
		{
			if (CanScroll(plv, SB_VERT, TRUE))
			{
				*pdy = ScrollAmount(pt.y, plv->sizeClient.cy, dy);    // down
			}
		}
		else if (pt.y <= 0)
		{
			if (CanScroll(plv, SB_VERT, FALSE))
			{
				*pdy = -ScrollAmount(0, pt.y, dy);    // up
			}
		}
	}

	if (plv->hwnd->ci.style & WS_HSCROLL)   // horizontally
	{

		if (pt.x >= plv->sizeClient.cx)
		{
			if (CanScroll(plv, SB_HORZ, TRUE))
			{
				*pdx = ScrollAmount(pt.x, plv->sizeClient.cx, dx);    // right
			}
		}
		else if (pt.x <= 0)
		{
			if (CanScroll(plv, SB_HORZ, FALSE))
			{
				*pdx = -ScrollAmount(0, pt.x, dx);    // left
			}
		}
	}

	// BUGBUG: this will potentially scroll outside the bounds of the
	// listview.  we should bound the scroll amount in CanScroll()
	// or ScrollAmount().

	if (*pdx || *pdy)
	{
		ListView_ValidateScrollParams(plv, pdx, pdy);
	}
}

#define swap(pi1, pi2) {int i = *(pi1) ; *(pi1) = *(pi2) ; *(pi2) = i ;}

void OrderRect(RECT *prc)
{
	if (prc->left > prc->right)
	{
		swap(&prc->left, &prc->right);
	}

	if (prc->bottom < prc->top)
	{
		swap(&prc->bottom, &prc->top);
	}
}

// in:
//      x, y    starting point in client coords

#define SCROLL_FREQ     (GetDoubleClickTime()/2)     // 1/5 of a second between scrolls

//----------------------------------------------------------------------------
BOOL ShouldScroll(SListViewData *plv, LPPOINT ppt, LPRECT lprc)
{
	_ASSERT(ppt);

	if (plv->hwnd->ci.style & WS_VSCROLL)
	{
		if (ppt->y >= lprc->bottom)
		{
			if (CanScroll(plv, SB_VERT, TRUE))
			{
				return TRUE;
			}
		}
		else if (ppt->y <= lprc->top)
		{
			if (CanScroll(plv, SB_VERT, FALSE))
			{
				return TRUE;
			}
		}
	}

	if (plv->hwnd->ci.style & WS_HSCROLL)
	{
		if (ppt->x >= lprc->right)
		{
			if (CanScroll(plv, SB_HORZ, TRUE))
			{
				return TRUE;
			}
		}
		else if (ppt->x <= lprc->left)
		{
			if (CanScroll(plv, SB_HORZ, FALSE))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

#ifdef WIN95MARQUEE
#define DrawFocusRectClip(a, b, c) DrawFocusRect(a, b)
#else
BOOL DrawFocusRectClip(HDC hdc, CONST RECT *prc, CONST RECT *prcClip)
{
	RECT rc;

	IntersectRect(&rc, prc, prcClip);

	return DrawFocusRect(hdc, &rc);
}
#endif

//----------------------------------------------------------------------------
void ListView_DragSelect(SListViewData *plv, int x, int y)
{
	RECT rc, rcWindow, rcOld, rcUnion, rcTemp2, rcClip;
	POINT pt;
	MSG32 msg32;
	HDC hdc;
	HWND hwnd = GetHostWnd(plv->hwnd);
	int i, iEnd, dx, dy;
	BOOL bInOld, bInNew = FALSE, bLocked = FALSE;
	DWORD dwTime, dwNewTime;
	HRGN hrgnUpdate = NULL, hrgnLV = NULL;
	CIUIRect rcList;
	CIUIRect rcListClient;
	HRGN hDragFocusRgn = NULL;

	rc.left = rc.right = x;
	rc.top = rc.bottom = y;

	rcOld = rc;

	UpdateWindow(GetHostWnd(plv->hwnd));

	if (plv->exStyle & LVS_EX_REGIONAL)
	{
		if ((hrgnUpdate = CreateRectRgn(0, 0, 0, 0)) &&
			(hrgnLV = CreateRectRgn(0, 0, 0, 0)) &&
			(LockWindowUpdate(GetParent(hwnd))))
		{
			hdc = GetDCEx(hwnd, NULL, DCX_PARENTCLIP | DCX_LOCKWINDOWUPDATE);
			bLocked = TRUE;
		}
		else
		{
			goto BailOut;
		}
	}
	else
	{
		hdc = GetDC(hwnd);
	}

	SetCapture(plv->hwnd);

	// 框选时，所画的矩形焦点框，不能跑到ListView外面。
	// 因为框选焦点框是绘制到Host HDC上的，所以需要设置视口及裁剪
	GetClientRect(plv->hwnd, rcListClient);
	ClientToScreen(plv->hwnd, rcListClient);
	IUIScreenToClient(GetHostWnd(plv->hwnd), rcListClient);

	SetViewportOrgEx(hdc, rcListClient.left, rcListClient.top, NULL);

	// SelectClipRgn选入的HRGN，不受HDC视点影响
	hDragFocusRgn = CreateRectRgnIndirect(rcListClient);
	SelectClipRgn(hdc, hDragFocusRgn);
	DeleteObject(hDragFocusRgn);
	hDragFocusRgn = NULL;

	DrawFocusRect(hdc, &rc);

	GetClientRect(plv->hwnd, &rcClip);
	GetWindowRect(plv->hwnd, &rcWindow);

	dwTime = GetTickCount();

	for (;;)
	{
		// WM_CANCELMODE messages will unset the capture, in that
		// case I want to exit this loop
		if (GetCapture() != hwnd)
		{
			break;
		}

		if (!PeekMessage32(&msg32, NULL, 0, 0, PM_REMOVE, TRUE))
		{

			// if the cursor is outside of the window rect
			// we need to generate messages to make autoscrolling
			// keep going

			if (!PtInRect(&rcWindow, msg32.pt))
			{
				// If we may be able to scroll, generate a mouse move.
				if (ShouldScroll(plv, &msg32.pt, &rcWindow))
				{
					SetCursorPos(msg32.pt.x, msg32.pt.y);
				}
			}
			else
			{
				WaitMessage();
			}
			continue;
		}


		// See if the application wants to process the message...
		if (CallMsgFilter32(&msg32, MSGF_COMMCTRL_DRAGSELECT, TRUE) != 0)
		{
			continue;
		}

		switch (msg32.message)
		{

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
			CCReleaseCapture(&plv->hwnd->ci);
			goto EndOfLoop;


		case WM_TIMER:
			if (msg32.wParam != IDT_MARQUEE)
			{
				goto DoDefault;
			}
		// else fall through

		case WM_MOUSEMOVE:
		{
			int dMax = -1;
			pt = msg32.pt;
			ScreenToClient(plv->hwnd, &pt);

			dwNewTime = GetTickCount();
			//            if (1 || (dwNewTime - dwTime) > SCROLL_FREQ)
			//          {
			dwTime = dwNewTime;     // reset scroll timer
			ScrollDetect(plv, pt, &dx, &dy);
			//          }
			//          else
			//          {
			//              dx = dy = 0;
			//          }
			//SetTimer(GetHostWnd(plv->hwnd), IDT_MARQUEE, SCROLL_FREQ, NULL);

			y -= dy;    // scroll up/down
			x -= dx;    // scroll left/right

			rc.left = x;
			rc.top = y;
			rc.right = pt.x;
			rc.bottom = pt.y;

			// clip drag rect to the window
			//
			if (rc.right > rcClip.right)
			{
				rc.right = rcClip.right;
			}
			if (rc.right < rcClip.left)
			{
				rc.right = rcClip.left;
			}
			if (rc.bottom > rcClip.bottom)
			{
				rc.bottom = rcClip.bottom;
			}
			if (rc.bottom < rcClip.top)
			{
				rc.bottom = rcClip.top;
			}

			OrderRect(&rc);

			if (EqualRect(&rc, &rcOld))
			{
				break;
			}

			// move the old rect
			DrawFocusRect(hdc, &rcOld); // erase old
			if (dx || dy)
			{
				ListView_OnScroll(plv, dx, dy);
			}
			OffsetRect(&rcOld, -dx, -dy);

			//
			// For Report and List view, we can speed things up by
			// only searching through those items that are visible.  We
			// use the hittest to calculate the first item to paint.
			// BUGBUG:: We are using state specific info here...
			//
			UnionRect(&rcUnion, &rc, &rcOld);

			if (ListView_IsReportView(plv))
			{
				i = (int)((plv->ptlRptOrigin.y + rcUnion.top  - plv->yTop)
						/ plv->cyItem);
				iEnd = (int)((plv->ptlRptOrigin.y + rcUnion.bottom  - plv->yTop)
						/ plv->cyItem) + 1;
			}

			else if (ListView_IsListView(plv))
			{
				i = ((plv->xOrigin + rcUnion.left) / plv->cxItem)
					* plv->cItemCol + rcUnion.top / plv->cyItem;

				iEnd = ((plv->xOrigin + rcUnion.right) / plv->cxItem)
					* plv->cItemCol + rcUnion.bottom / plv->cyItem + 1;
			}

			else
			{
				if (ListView_IsOwnerData(plv))
				{
					ListView_CalcMinMaxIndex(plv, &rcUnion, &i, &iEnd);
				}
				else
				{
					i = 0;
					iEnd = ListView_Count(plv);
				}
			}

			// make sure our endpoint is in range.
			if (iEnd > ListView_Count(plv))
			{
				iEnd = ListView_Count(plv);
			}

			if (i < 0)
			{
				i = 0;
			}

			if (ListView_IsOwnerData(plv) && (i < iEnd))
			{
				ListView_NotifyCacheHint(plv, i, iEnd - 1);
			}

			if (bInNew && !(msg32.wParam & (MK_CONTROL | MK_SHIFT)))
			{
				plv->iMark = -1;
			}

			for (; i  < iEnd; i++)
			{
				RECT dummy;
				ListView_GetRects(plv, i, NULL, NULL, NULL, &rcTemp2);

				// don't do this infaltion if we're in report&full row mode
				// in that case, just touching is good enough
				if (!(ListView_IsReportView(plv) && ListView_FullRowSelect(plv)))
				{
					int cxInflate = (rcTemp2.right - rcTemp2.left) / 4;
					if (ListView_IsListView(plv))
					{
						cxInflate = min(cxInflate, plv->cxSmIcon);
					}
					InflateRect(&rcTemp2, -cxInflate, -(rcTemp2.bottom - rcTemp2.top) / 4);
				}

				bInOld = (IntersectRect(&dummy, &rcOld, &rcTemp2) != 0);
				bInNew = (IntersectRect(&dummy, &rc, &rcTemp2) != 0);

				if (msg32.wParam & MK_CONTROL)
				{
					if (bInOld != bInNew)
					{
						ListView_ToggleSelection(plv, i);
					}
				}
				else
				{
					// was there a change?
					if (bInOld != bInNew)
					{
						ListView_OnSetItemState(plv, i, bInOld ? 0 : LVIS_SELECTED, LVIS_SELECTED);
					}

					// if no alternate keys are down.. set the mark to
					// the item furthest from the cursor
					if (bInNew && !(msg32.wParam & (MK_CONTROL | MK_SHIFT)))
					{
						int dItem;
						dItem = (rcTemp2.left - pt.x) * (rcTemp2.left - pt.x) +
							(rcTemp2.top - pt.y) * (rcTemp2.top - pt.y);
						// if it's further away, set this as the mark
						//DebugMsg(TF_LISTVIEW, "dItem = %d, dMax = %d", dItem, dMax);
						if (dItem > dMax)
						{
							//DebugMsg(TF_LISTVIEW, "taking dItem .. iMark = %d", i);
							dMax = dItem;
							plv->iMark = i;
						}
					}
				}
			}

			//DebugMsg(TF_LISTVIEW, "Final iMark = %d", plv->iMark);
			if (bLocked)
			{
				if (GetUpdateRgn(GetHostWnd(plv->hwnd), hrgnUpdate, FALSE) > NULLREGION)
				{
					ValidateRect(GetHostWnd(plv->hwnd), NULL);
					GetWindowRgn(GetHostWnd(plv->hwnd), hrgnLV);
					CombineRgn(hrgnUpdate, hrgnUpdate, hrgnLV, RGN_AND);
					SelectClipRgn(hdc, hrgnUpdate);
					SendMessage(GetHostWnd(plv->hwnd), WM_PRINTCLIENT, (WPARAM)hdc, 0);
					SelectClipRgn(hdc, NULL);
				}
			}
			else
			{
				UpdateWindow(GetHostWnd(plv->hwnd));    // make selection draw
			}


			DrawFocusRect(hdc, &rc);

			rcOld = rc;
			break;
		}

		case WM_KEYDOWN:
			switch (msg32.wParam)
			{
			case VK_ESCAPE:
				ListView_DeselectAll(plv, -1);
				goto EndOfLoop;
			}
		case WM_CHAR:
		case WM_KEYUP:
			// don't process thay keyboard stuff during marquee
			break;


		default:

			// don't process mouse wheel stuff
			if (msg32.message == IUIGetGloabelVar()->g_msgMSWheel)
			{
				break;
			}

DoDefault:
			TranslateMessage32(&msg32, TRUE);
			DispatchMessage32(&msg32, TRUE);
		}
	}

EndOfLoop:
	DrawFocusRect(hdc, &rcOld); // erase old
	ReleaseDC(hwnd, hdc);

BailOut:
	if (hrgnUpdate)
	{
		DeleteObject(hrgnUpdate);
	}
	if (hrgnLV)
	{
		DeleteObject(hrgnLV);
	}
	if (bLocked)
	{
		LockWindowUpdate(NULL);
	}
}


#define SHIFT_DOWN(keyFlags)    (keyFlags & MK_SHIFT)
#define CONTROL_DOWN(keyFlags)  (keyFlags & MK_CONTROL)
#define RIGHTBUTTON(keyFlags)   (keyFlags & MK_RBUTTON)

void PASCAL ListView_ButtonSelect(SListViewData *plv, int iItem, UINT keyFlags, BOOL bSelected)
{
	if (SHIFT_DOWN(keyFlags))
	{
		ListView_SelectRangeTo(plv, iItem, !CONTROL_DOWN(keyFlags));
		ListView_SetFocusSel(plv, iItem, TRUE, FALSE, FALSE);
	}
	else if (!CONTROL_DOWN(keyFlags))
	{
		ListView_SetFocusSel(plv, iItem, TRUE, !bSelected, FALSE);
	}
}

void ListView_HandleStateIconClick(SListViewData *plv, int iItem)
{
	int iState =
		ListView_OnGetItemState(plv, iItem, LVIS_STATEIMAGEMASK);

	iState = STATEIMAGEMASKTOINDEX(iState) - 1;
	iState++;
	iState %= ImageList_GetImageCount(plv->himlState);
	iState++;
	ListView_OnSetItemState(plv, iItem, INDEXTOSTATEIMAGEMASK(iState), LVIS_STATEIMAGEMASK);
}

BOOL ListView_RBeginMarquee(SListViewData *plv, int x, int y, LPLVHITTESTINFO plvhti)
{
	if (ListView_FullRowSelect(plv) &&
		ListView_IsReportView(plv) &&
		!(plv->hwnd->ci.style & LVS_SINGLESEL) &&
		!ListView_OwnerDraw(plv) &&
		plvhti->iSubItem == 0)
	{
		// can only begin marquee in column 0.

		if (plvhti->flags == LVHT_ONITEM)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ListView_HandleMouse(SListViewData *plv, BOOL fDoubleClick, int x, int y, UINT keyFlags, BOOL bMouseWheel)
{
	LV_HITTESTINFO ht;
	NMITEMACTIVATE nm;
	int iItem, click, drag;
	BOOL bSelected, fHadFocus, fNotifyReturn = FALSE;
	BOOL fActive;
	HWND hwnd = GetHostWnd(plv->hwnd);

	if (plv->fButtonDown)
	{
		return;
	}
	plv->fButtonDown = TRUE;


	if (plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickHappened && plv->fOneClickOK)
	{
		IUI::KillTimer(plv->hwnd, IDT_ONECLICKHAPPENED);
		plv->fOneClickHappened = FALSE;
		CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &(plv->nmOneClickHappened.hdr));
		if (!IsWindow(hwnd))
		{
			return;
		}
	}

	fHadFocus = IsFocused(plv->hwnd);
	click = RIGHTBUTTON(keyFlags) ? NM_RCLICK : NM_CLICK;
	drag  = RIGHTBUTTON(keyFlags) ? LVN_BEGINRDRAG : LVN_BEGINDRAG;

	fActive = ChildOfActiveWindow(GetHostWnd(plv->hwnd)) || fShouldFirstClickActivate() ||
		ChildOfDesktop(GetHostWnd(plv->hwnd));

#ifdef LVDEBUG
	DebugMsg(TF_LISTVIEW, TEXT("ListView_OnButtonDown %d"), fDoubleClick);
#endif

	SetCapture(plv->hwnd);

	plv->ptCapture.x = x;
	plv->ptCapture.y = y;


	if (!ListView_DismissEdit(plv, FALSE) && GetCapture() != GetHostWnd(plv->hwnd))
	{
		goto EndButtonDown;
	}
	CCReleaseCapture(&plv->hwnd->ci);

	// REVIEW: right button implies no shift or control stuff
	// Single selection style also implies no modifiers
	//if (RIGHTBUTTON(keyFlags) || (plv->hwnd->ci.style & LVS_SINGLESEL))
	if ((plv->hwnd->ci.style & LVS_SINGLESEL))
	{
		keyFlags &= ~(MK_SHIFT | MK_CONTROL);
	}

	ht.pt.x = x;
	ht.pt.y = y;
	iItem = ListView_OnSubItemHitTest(plv, &ht);
	plv->iSubItemEditLabel = ht.iSubItem;
	if (ht.iSubItem != 0)
	{
		// if we're not in full row select,
		// hitting on a subitem is like hitting on nowhere
		// also, in win95, ownerdraw fixed effectively had full row select
		if (!ListView_FullRowSelect(plv) &&
			!(plv->hwnd->ci.style & LVS_OWNERDRAWFIXED))
		{
			iItem = -1;
			ht.flags = LVHT_NOWHERE;
		}
	}

	nm.iItem = iItem;
	nm.iSubItem = ht.iSubItem;
	nm.uChanged = 0;
	nm.ptAction.x = x;
	nm.ptAction.y = y;
	nm.uKeyFlags = GetLVKeyFlags();

	// FProt Profesional assumed that if the notification structure pointer + 14h bytes
	// had a value 2 that it was a displayinfo structure and they then used offset +2c as lparam...
	nm.uNewState = 0;

	plv->iNoHover = iItem;


	bSelected = (iItem >= 0) && ListView_OnGetItemState(plv, iItem, LVIS_SELECTED);

	if (fDoubleClick)
	{
		//
		// Cancel any name editing that might happen.
		//
		ListView_CancelPendingEdit(plv);
		IUI::KillTimer(plv->hwnd, IDT_SCROLLWAIT);

		if (ht.flags & LVHT_NOWHERE)
		{
			// this would have been done in the first click in win95 except
			// now we blow off the first click on focus change
			if (!SHIFT_DOWN(keyFlags) && !CONTROL_DOWN(keyFlags))
			{
				ListView_DeselectAll(plv, -1);
			}
		}

		click = RIGHTBUTTON(keyFlags) ? NM_RDBLCLK : NM_DBLCLK ;
		if (CCSendNotify(&plv->hwnd->ci, click, &nm.hdr))
		{
			goto EndButtonDown;
		}

		/// some (comdlg32 for example) destroy on double click
		// we need to bail if that happens because plv is no longer valid
		if (!IsWindow(hwnd))
		{
			return;
		}

		if (click == NM_DBLCLK)
		{
			// these shift control flags are to mirror when we don't send out the activate on the single click,
			// but are in the oneclick activate mode  (see below)
			if (ht.flags & (LVHT_ONITEMLABEL | LVHT_ONITEMICON))
			{
				// possible scenarios below:
				// 1) we're using classic windows style so double click => launch
				// 2) we're using single click activate
				//    a) shift is down and item is selected => launch
				//       this implies that the first click selected it
				//    b) control is down => launch
				//       the first click toggled the selection so if the item was
				//       the only item selected and we double clicked on it
				//       the first click deselects it and no item is selected
				//       so nothing will be launched - this is win95 behavior
				if (!(plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickOK) ||
					(plv->exStyle & LVS_EX_ONECLICKACTIVATE &&  plv->fOneClickOK &&
						(SHIFT_DOWN(keyFlags) || CONTROL_DOWN(keyFlags))))
				{
					CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &nm.hdr);
				}
			}
			// Double-click on checkbox state icon cycles it just like single click
			else if ((ht.flags & LVHT_ONITEMSTATEICON) && ListView_CheckBoxes(plv))
			{
				ListView_HandleStateIconClick(plv, iItem);
			}
		}

		if (!IsWindow(hwnd))
		{
			return;
		}
		goto EndButtonDown;
	}

	if (ht.flags & (LVHT_ONITEMLABEL | LVHT_ONITEMICON))
	{

		// if it wasn't selected, we're about to select it... play
		// a little ditty for us...
		CCPlaySound(IUIGetGloabelVar()->c_szSelect);

		if (!RIGHTBUTTON(keyFlags) || (!CONTROL_DOWN(keyFlags) && !SHIFT_DOWN(keyFlags)))
		{
			ListView_ButtonSelect(plv, iItem, keyFlags, bSelected);
		}

		// handle full row select
		// If single-select listview, disable marquee selection.
		//
		// Careful - CheckForDragBegin yields and the app may have
		// destroyed the item we were thinking about dragging!
		//
		if (!bMouseWheel && CheckForDragBegin(plv->hwnd, x, y))
		{
			// should we do a marquee?
			if (ListView_RBeginMarquee(plv, x, y, &ht) &&
				!CCSendNotify(&plv->hwnd->ci, LVN_MARQUEEBEGIN, &nm.hdr))
			{
				ListView_DragSelect(plv, x, y);
				fNotifyReturn = !CCSendNotify(&plv->hwnd->ci, click, &nm.hdr);
			}
			else
			{
				// Before we start dragging, make it sure that it is
				// selected and has the focus.
				ListView_SetFocusSel(plv, iItem, TRUE, FALSE, FALSE);

				if (!SHIFT_DOWN(keyFlags))
				{
					plv->iMark = iItem;
				}

				// Then, we need to update the window before start dragging
				// to show the selection chagne.
				UpdateWindow(GetHostWnd(plv->hwnd));

				CCSendNotify(&plv->hwnd->ci, drag, &nm.hdr);

				goto EndButtonDown;
			}
		}

		// CheckForDragBegin yields, so revalidate before continuing
		else if (IsWindow(hwnd))
		{
			// button came up and we are not dragging

			if (!RIGHTBUTTON(keyFlags))
			{
				if (CONTROL_DOWN(keyFlags))
				{
					// do this on the button up so that ctrl-dragging a range
					// won't toggle the select.

					if (SHIFT_DOWN(keyFlags))
					{
						ListView_SetFocusSel(plv, iItem, FALSE, FALSE, FALSE);
					}
					else
					{
						ListView_SetFocusSel(plv, iItem, TRUE, FALSE, TRUE);
					}
				}
			}
			if (!SHIFT_DOWN(keyFlags))
			{
				plv->iMark = iItem;
			}

			if (!ListView_SetFocus(plv->hwnd))    // activate this window
			{
				return;
			}

			// now do the deselect stuff
			if (!SHIFT_DOWN(keyFlags) && !CONTROL_DOWN(keyFlags) && !RIGHTBUTTON(keyFlags))
			{
				ListView_DeselectAll(plv, iItem);
				if ((ht.flags & LVHT_ONITEMLABEL) && bSelected &&
					!(plv->exStyle & (LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE)))
				{

					// doing this check for ownerdrawfixed is for compatability.
					// we don't want to go into edit mode if the user just happened to click
					// to this window when a different one had focus,
					// but ms hammer relied upon the notification being sent (and we
					// don't go into edit mode anyways for ownerdraw)
					if (fHadFocus ||
						(plv->hwnd->ci.style & LVS_OWNERDRAWFIXED))
					{
						// Click on item label.  It was selected and
						// no modifier keys were pressed and no drag operation
						// So setup for name edit mode.  Still need to wait
						// to make sure user is not doing double click.
						//
						ListView_SetupPendingNameEdit(plv);
					}
				}
			}

			fNotifyReturn = !CCSendNotify(&plv->hwnd->ci, click, &nm.hdr);
			if (!IsWindow(hwnd))
			{
				return;
			}

			if (plv->exStyle & (LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE))
			{
				if (!RIGHTBUTTON(keyFlags))
				{
					// We don't ItemActivate within one double-click time of creating
					// this listview. This is a common occurence for people used to
					// double-clicking. The first click pops up a new window which
					// receives the second click and ItemActivates the item...
					//
					if ((plv->exStyle & LVS_EX_ONECLICKACTIVATE && plv->fOneClickOK) || bSelected)
					{
						if (fActive)
						{
							// condition: if we're in a single click activate mode
							// don't launch if control or shift keys are pressed
							BOOL bCond = plv->exStyle & LVS_EX_ONECLICKACTIVATE && !CONTROL_DOWN(keyFlags) && !SHIFT_DOWN(keyFlags);

							if ((bSelected && plv->exStyle & LVS_EX_TWOCLICKACTIVATE) ||
								(bCond && !g_bUseDblClickTimer))
							{
								CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &nm.hdr);
								if (!IsWindow(hwnd))
								{
									return;
								}
							}
							else if (bCond && g_bUseDblClickTimer)
							{
								plv->fOneClickHappened = TRUE;
								plv->nmOneClickHappened = nm;
								IUI::SetTimer(plv->hwnd, IDT_ONECLICKHAPPENED, GetDoubleClickTime(), NULL);
							}
						}
					}
				}
			}
		}
		else
		{
			// IsWindow() failed.  Bail.
			return;
		}
	}
	else if (ht.flags & LVHT_ONITEMSTATEICON)
	{
		// Should activate window and send notificiation to parent...
		if (!ListView_SetFocus(plv->hwnd))   // activate this window
		{
			return;
		}
		fNotifyReturn = !CCSendNotify(&plv->hwnd->ci, click, &nm.hdr);
		if (fNotifyReturn && ListView_CheckBoxes(plv))
		{
			ListView_HandleStateIconClick(plv, iItem);
		}
	}
	else if (ht.flags & LVHT_NOWHERE)
	{
		if (!ListView_SetFocus(plv->hwnd))   // activate this window
		{
			return;
		}

		// If single-select listview, disable marquee selection.
		if (!(plv->hwnd->ci.style & LVS_SINGLESEL)
			&& CheckForDragBegin(plv->hwnd, x, y)
			&& !CCSendNotify(&plv->hwnd->ci, LVN_MARQUEEBEGIN, &nm.hdr))
		{
			if (!SHIFT_DOWN(keyFlags) && !CONTROL_DOWN(keyFlags))
			{
				ListView_DeselectAll(plv, -1);
			}
			ListView_DragSelect(plv, x, y);
			fNotifyReturn = !CCSendNotify(&plv->hwnd->ci, click, &nm.hdr);
		}
		else if (IsWindow(hwnd))
		{
			// if we didn't have focus and aren't showing selection always,
			// make the first click just set focus
			BOOL fDoFirstClickSelection = (fHadFocus || plv->hwnd->ci.style & LVS_SHOWSELALWAYS ||
					CONTROL_DOWN(keyFlags) || SHIFT_DOWN(keyFlags) ||
					RIGHTBUTTON(keyFlags));

			if (fDoFirstClickSelection && fActive)
			{

				if (!SHIFT_DOWN(keyFlags) && !CONTROL_DOWN(keyFlags))
				{
					ListView_DeselectAll(plv, -1);
				}

				fNotifyReturn = !CCSendNotify(&plv->hwnd->ci, click, &nm.hdr);
			}
		}
		else
		{
			// IsWindow() failed.  Bail.
			return;
		}
	}

	// re-check the key state so we don't get confused by multiple clicks

	// this needs to check the GetKeyState stuff only when we've gone into
	// a modal loop waiting for the rbutton up.
	if (fNotifyReturn && (click == NM_RCLICK)) // && (GetKeyState(VK_RBUTTON)>=0))
	{
		POINT pt = { x, y };
		ClientToScreen(GetHostWnd(plv->hwnd), &pt);
		FORWARD_WM_CONTEXTMENU(GetHostWnd(plv->hwnd), GetHostWnd(plv->hwnd), pt.x, pt.y, SendMessage);
	}

EndButtonDown:
	if (IsWindow(hwnd))
	{
		plv->fButtonDown = FALSE;
	}
}

void ListView_OnButtonDown(SListViewData *plv, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	ListView_HandleMouse(plv, fDoubleClick, x, y, keyFlags, FALSE);
}

BOOL ListView_CancelPendingTimer(SListViewData *plv, UINT fFlags, int idTimer)
{
	if (plv->flags & fFlags)
	{
		IUI::KillTimer(plv->hwnd, idTimer);
		plv->flags &= ~fFlags;
		return TRUE;
	}
	return FALSE;
}

//
// ListView_OnTimer:
//     process the WM_TIMER message.  If the timer id is thta
//     of the name editing, we should then start the name editing mode.
//
void ListView_OnTimer(SListViewData *plv, UINT id)
{
	IUI::KillTimer(plv->hwnd, id);

	if (id == IDT_NAMEEDIT)
	{
		// Kill the timer as we wont need any more messages from it.

		if (ListView_CancelPendingEdit(plv))
		{
			// And start name editing mode.
			if (!ListView_OnEditLabel(plv, plv->iFocus, NULL))
			{
				ListView_DismissEdit(plv, FALSE);
				ListView_SetFocusSel(plv, plv->iFocus, TRUE, TRUE, FALSE);
			}
		}
	}
	else if (id == IDT_SCROLLWAIT)
	{

		if (ListView_CancelScrollWait(plv))
		{
			ListView_OnEnsureVisible(plv, plv->iFocus, TRUE);
		}
	}
	else if (id == IDT_ONECLICKOK)
	{
		plv->fOneClickOK = TRUE;
	}
	else if (id == IDT_ONECLICKHAPPENED)
	{
		//if (!g_bUseDblClickTimer)
		//{
		////    EnableWindow(GetHostWnd(plv->hwnd), TRUE);
		//    SetWindowBits(GetHostWnd(plv->hwnd), GWL_STYLE, WS_DISABLED, 0);
		//    plv->fOneClickHappened = FALSE;
		//}
		// check the bit just in case they double-clicked
		//else
		if (plv->fOneClickHappened)
		{
			plv->fOneClickHappened = FALSE;
			CCSendNotify(&plv->hwnd->ci, LVN_ITEMACTIVATE, &(plv->nmOneClickHappened.hdr));
		}
	}
}

//
// ListView_SetupPendingNameEdit:
//      Sets up a timer to begin name editing at a delayed time.  This
//      will allow the user to double click on the already selected item
//      without going into name editing mode, which is especially important
//      in those views that only show a small icon.
//
void ListView_SetupPendingNameEdit(SListViewData *plv)
{
	IUI::SetTimer(plv->hwnd, IDT_NAMEEDIT, GetDoubleClickTime(), NULL);
	plv->flags |= LVF_NMEDITPEND;
}

// WM_*SCROLL的响应函数。 当滚动了ListView中的水平或垂直滚动条后，会执行到这里。
// code: LOWORD(wParam): of WM_XSCROLL: SB_*, 如SB_LINEDOWN
// pos: HIWORD(wParam) of WM_XSCROLL:当code为SB_THUMBPOSITION 或 SB_THUMBTRACK时，
//		表示当前位置，否则不使用此变量。
// sb: SB_VERT或SB_HORZ
void PASCAL ListView_OnHVScroll(SListViewData *plv, UINT code, int pos, int sb)
{
	int iScrollCount = 0;

	// 取pos值
	// 因为WM_XSCROLL中的pos最大范围为16位，所以，我们直接通过ListView_GetScrollInfo
	// 得到真正的32位pos值。
#ifdef SIF_TRACKPOS
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_TRACKPOS;

	if (ListView_GetScrollInfo(plv, sb, &si))
	{
		pos = (int)si.nTrackPos;
	}
#endif

	// 滚动时，隐藏Edit Label输入框
	ListView_DismissEdit(plv, FALSE);

	// 更新ptlRptOrigin。 通过更新ptlRptOrigin，来滚动视图
	_ListView_OnScroll(plv, code, pos, sb);

	switch (code)
	{
	case SB_PAGELEFT:
	case SB_PAGERIGHT:
		break;

	case SB_LINELEFT:
	case SB_LINERIGHT:
		break;

	case SB_ENDSCROLL:
		break;

	}

	InvalidateRect(plv->hwnd, NULL);
}

void ListView_OnVScroll(SListViewData *plv, HWND hwndCtl, UINT code, int pos)
{
	ListView_OnHVScroll(plv, code, pos, SB_VERT);
}

void ListView_OnHScroll(SListViewData *plv, HWND hwndCtl, UINT code, int pos)
{
	ListView_OnHVScroll(plv, code, pos, SB_HORZ);
}

int ListView_ValidateOneScrollParam(SListViewData *plv, int iDirection, int dx)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;



	if (!ListView_GetScrollInfo(plv, iDirection, &si))
	{
		return 0;
	}

	si.nMax -= (si.nPage - 1);
	si.nPos += dx;
	if (si.nPos < si.nMin)
	{
		dx += (int)(si.nMin - si.nPos);
	}
	else if (si.nPos > si.nMax)
	{
		dx -= (int)(si.nPos - si.nMax);
	}

	return dx;
}

BOOL PASCAL ListView_ValidateScrollParams(SListViewData *plv, int *pdx, int *pdy)
{
	int dx = *pdx;
	int dy = *pdy;

	if (plv->hwnd->ci.style & LVS_NOSCROLL)
	{
		return FALSE;
	}

	if (ListView_IsListView(plv))
	{
		ListView_MaybeResizeListColumns(plv, 0, ListView_Count(plv) - 1);
#ifdef COLUMN_VIEW
		if (dx < 0)
		{
			dx = (dx - plv->cxItem - 1) / plv->cxItem;
		}
		else
		{
			dx = (dx + plv->cxItem - 1) / plv->cxItem;
		}

		if (dy)
		{
			return FALSE;
		}
#else
		if (dy < 0)
		{
			dy = (dy - plv->cyItem - 1) / plv->cyItem;
		}
		else
		{
			dy = (dy + plv->cyItem - 1) / plv->cyItem;
		}

		if (dx)
		{
			return FALSE;
		}
#endif
	}
	else if (ListView_IsReportView(plv))
	{
		//
		// Note: This function expects that dy is in number of lines
		// and we are working with pixels so do a conversion use some
		// rounding up and down to make it right
		if (dy > 0)
		{
			dy = (dy + plv->cyItem / 2) / plv->cyItem;
		}
		else
		{
			dy = (dy - plv->cyItem / 2) / plv->cyItem;
		}
	}

	if (dy)
	{
		dy = ListView_ValidateOneScrollParam(plv, SB_VERT, dy);
		if (ListView_IsReportView(plv)
#ifndef COLUMN_VIEW
			|| ListView_IsListView(plv)
#endif
		)
		{
			// convert back to pixels
			dy *= plv->cyItem;
		}
		*pdy = dy;
	}

	if (dx)
	{

		dx = ListView_ValidateOneScrollParam(plv, SB_HORZ, dx);
#ifdef COLUMN_VIEW
		if (ListView_IsListView(plv))
		{
			dx *= plv->cxItem;
		}
#endif
		*pdx = dx;
	}

	return TRUE;
}


BOOL ListView_OnScroll(SListViewData *plv, int dx, int dy)
{

	if (plv->hwnd->ci.style & LVS_NOSCROLL)
	{
		return FALSE;
	}

	if (ListView_IsListView(plv))
	{
		// Scale pixel count to column count
		//
#ifdef COLUMN_VIEW
		if (dx < 0)
		{
			dx -= plv->cxItem - 1;
		}
		else
		{
			dx += plv->cxItem - 1;
		}

		dx = dx / plv->cxItem;

		if (dy)
		{
			return FALSE;
		}
#else
		if (dy < 0)
		{
			dy -= plv->cyItem - 1;
		}
		else
		{
			dy += plv->cyItem - 1;
		}

		dy = dy / plv->cyItem;

		if (dx)
		{
			return FALSE;
		}
#endif
	}
	else if (ListView_IsReportView(plv))
	{
		//
		// Note: This function expects that dy is in number of lines
		// and we are working with pixels so do a conversion use some
		// rounding up and down to make it right
		if (dy > 0)
		{
			dy = (dy + plv->cyItem / 2) / plv->cyItem;
		}
		else
		{
			dy = (dy - plv->cyItem / 2) / plv->cyItem;
		}
	}
	_ListView_Scroll2(plv, dx, dy, 0);
	ListView_UpdateScrollBars(plv);
	return TRUE;
}

BOOL ListView_OnEnsureVisible(SListViewData *plv, int i, BOOL fPartialOK)
{
	RECT rcBounds;
	RECT rc;
	int dx, dy;

	if (!ListView_IsValidItemNumber(plv, i) || plv->hwnd->ci.style & LVS_NOSCROLL)
	{
		return FALSE;
	}

	// we need to do this again inside because some callers don't do it.
	// other callers that do this need to do it outside so that
	// they can know not to call us if there's not wait pending
	ListView_CancelScrollWait(plv);

	if (ListView_IsReportView(plv))
	{
		return ListView_ROnEnsureVisible(plv, i, fPartialOK);
	}


	ListView_GetRects(plv, i, &rc, NULL, &rcBounds, NULL);

	if (!fPartialOK)
	{
		rc = rcBounds;
	}

	// If any part of rc is outside of rcClient, then
	// scroll so that all of rcBounds is visible.
	//
	dx = 0;
	if (rc.left < 0 || rc.right >= plv->sizeClient.cx)
	{
		dx = rcBounds.left - 0;
		if (dx >= 0)
		{
			dx = rcBounds.right - plv->sizeClient.cx;
			if (dx <= 0)
			{
				dx = 0;
			}
			else if ((rcBounds.left - dx) < 0)
			{
				dx = rcBounds.left - 0;    // Not all fits...
			}
		}
	}
	dy = 0;
	if (rc.top < 0 || rc.bottom >= plv->sizeClient.cy)
	{
		dy = rcBounds.top - 0;
		if (dy >= 0)
		{
			dy = rcBounds.bottom - plv->sizeClient.cy;
			if (dy < 0)
			{
				dy = 0;
			}
		}
	}

	if (dx | dy)
	{
		return ListView_OnScroll(plv, dx, dy);
	}

	return TRUE;
}

void ListView_UpdateScrollBars(SListViewData *plv)
{
	RECT rc;
	DWORD dwStyle;

	if ((plv->hwnd->ci.style & LVS_NOSCROLL) ||
		(!(ListView_RedrawEnabled(plv))))
	{
		return;
	}

	_ListView_UpdateScrollBars(plv);

	GetClientRect(plv->hwnd, &rc);
	plv->sizeClient.cx = rc.right;
	plv->sizeClient.cy = rc.bottom;

	dwStyle = ListView_GetWindowStyle(plv);
	//	plv->hwnd->ci.style = (plv->hwnd->ci.style & ~(WS_HSCROLL | WS_VSCROLL)) | (dwStyle & WS_HSCROLL | WS_VSCROLL);
}

#ifndef WINNT
#pragma optimize ("gle", off)
// Crappy hack for Sage, which passes unitialized memory to SetWindowPlacement.
// They used to get lucky and get zeros for the max position, but now they end
// up with non-zero stack trash that causes bad things to happen when sage is
// maximized.  Thus, zero a bunch of stack to save their tail...
void ZeroSomeStackForSage()
{
	BYTE aByte[1024];

	memset(aByte, 0, sizeof(aByte));

	aByte;
}
#pragma optimize ("", on)
#endif

void ListView_OnSetFont(SListViewData *plv, HFONT hfont, BOOL fRedraw)
{
	HDC hdc;
	SIZE siz;
	LOGFONT lf;
	HFONT hfontPrev;

	if ((plv->flags & LVF_FONTCREATED) && plv->hfontLabel)
	{
		DeleteObject(plv->hfontLabel);
		plv->flags &= ~LVF_FONTCREATED;
	}

	if (hfont == NULL)
	{
		SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, FALSE);
#ifdef WINNT
		// B#210235 - because NT4 initializes icontitle logfont with Ansi charset
		// no matter what font is selected, following A/W conversion would fail
		// on non US environment if we use this logfont to get codepage.
		// The ACP is guaranteed to work with any Ansi apps because these apps
		// assume ACP to be matching to their desired codepage.
		if (staticIsOS(OS_NT4) && !staticIsOS(OS_NT5))
		{
			CHARSETINFO csi;
			TranslateCharsetInfo((DWORD *)g_uiACP, &csi, TCI_SRCCODEPAGE);
			lf.lfCharSet = (BYTE)csi.ciCharset;
		}
#endif
		hfont = CreateFontIndirect(&lf);
		plv->flags |= LVF_FONTCREATED;
	}

	hdc = GetDC(HWND_DESKTOP);

	hfontPrev = SelectFont(hdc, hfont);

	GetTextExtentPoint(hdc, TEXT("0"), 1, &siz);

	plv->cyLabelChar = siz.cy;
	plv->cxLabelChar = siz.cx;

	GetTextExtentPoint(hdc, IUIGetGloabelVar()->c_szEllipses, CCHELLIPSES, &siz);
	plv->cxEllipses = siz.cx;

	SelectFont(hdc, hfontPrev);
	ReleaseDC(HWND_DESKTOP, hdc);

	plv->hfontLabel = hfont;

	plv->hwnd->ci.uiCodePage = GetCodePageForFont(hfont);

	ListView_InvalidateCachedLabelSizes(plv);

	/* Ensure that our tooltip control uses the same font as the list view is using, therefore
	/  avoiding any nasty formatting problems. */

	if (plv->hwndToolTips)
	{
		FORWARD_WM_SETFONT(plv->hwndToolTips, plv->hfontLabel, FALSE, SendMessage);
	}

	// If we have a header window, we need to forward this to it also
	// as we have destroyed the hfont that they are using...
	if (plv->hwndHdr)
	{
		//L FORWARD_WM_SETFONT(plv->hwndHdr, plv->hfontLabel, FALSE, SendMessage);
		ListView_UpdateScrollBars(plv);
	}

	if (plv->hFontHot)
	{
		DeleteObject(plv->hFontHot);
		plv->hFontHot = NULL;
	}

	CCGetHotFont(plv->hfontLabel, &plv->hFontHot);
	plv->iFreeSlot = -1;

	if (fRedraw)
	{
		RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
#ifndef WINNT
	ZeroSomeStackForSage();
#endif
}

HFONT ListView_OnGetFont(SListViewData *plv)
{
	return plv->hfontLabel;
}

// This function process the WM_SETREDRAW message by setting or clearing
// a bit in the listview structure, which several places in the code will
// check...
//
// REVIEW: Should probably forward to DefWindowProc()
//
void ListView_OnSetRedraw(SListViewData *plv, BOOL fRedraw)
{
	if (fRedraw)
	{
		BOOL fChanges = FALSE;
		// Only do work if we're turning redraw back on...
		//
		if (!(plv->flags & LVF_REDRAW))
		{
			plv->flags |= LVF_REDRAW;

			// deal with any accumulated invalid regions
			if (plv->hrgnInval)
			{
				UINT fRedraw = (plv->flags & LVF_ERASE) ? RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW : RDW_UPDATENOW | RDW_INVALIDATE;
				if (plv->hrgnInval == (HRGN)ENTIRE_REGION)
				{
					plv->hrgnInval = NULL;
				}

				RedrawWindow(GetHostWnd(plv->hwnd), NULL, plv->hrgnInval, fRedraw);
				ListView_DeleteHrgnInval(plv);
				fChanges = TRUE;
			}
			plv->flags &= ~LVF_ERASE;

			// now deal with the optimized stuff
			if (ListView_IsListView(plv) || ListView_IsReportView(plv))
			{
				if (plv->iFirstChangedNoRedraw != -1)
				{
					// We may try to resize the column
					if (!ListView_MaybeResizeListColumns(plv, plv->iFirstChangedNoRedraw,
							ListView_Count(plv) - 1))
					{
						ListView_OnUpdate(plv, plv->iFirstChangedNoRedraw);
					}
				}
				else
				{
					ListView_UpdateScrollBars(plv);
				}
			}
			else
			{
				int iCount;

				if (plv->iFirstChangedNoRedraw != -1)
				{
					for (iCount = ListView_Count(plv) ; plv->iFirstChangedNoRedraw < iCount; plv->iFirstChangedNoRedraw++)
					{
						ListView_InvalidateItem(plv, plv->iFirstChangedNoRedraw, FALSE, RDW_INVALIDATE);
					}
					fChanges = TRUE;
				}
				if (fChanges)
				{
					ListView_RecalcRegion(plv, TRUE, TRUE);
				}
				if ((plv->hwnd->ci.style & LVS_AUTOARRANGE) && fChanges)
				{
					ListView_OnUpdate(plv, plv->iFirstChangedNoRedraw);
				}
				else
				{
					ListView_UpdateScrollBars(plv);
				}
			}
		}
	}
	else
	{
		plv->iFirstChangedNoRedraw = -1;
		plv->flags &= ~LVF_REDRAW;
	}
}

HIMAGELIST ListView_OnGetImageList(SListViewData *plv, int iImageList)
{
	switch (iImageList)
	{
	case LVSIL_NORMAL:
		return plv->himl;

	case LVSIL_SMALL:
		return plv->himlSmall;

	case LVSIL_STATE:
		return plv->himlState;
	}
	_ASSERT(0);
	return NULL;
}


HIMAGELIST ListView_OnSetImageList(SListViewData *plv, HIMAGELIST himl, int iImageList)
{
	HIMAGELIST hImageOld = NULL;

	switch (iImageList)
	{
	case LVSIL_NORMAL:
		hImageOld = plv->himl;
		plv->himl = himl;
		if (himl)
		{
			ImageList_GetIconSize(himl, &plv->cxIcon, &plv->cyIcon);

			if (!(plv->flags & LVF_ICONSPACESET))
			{
				ListView_OnSetIconSpacing(plv, (LPARAM)(-1));
			}
		}
		break;

	case LVSIL_SMALL:
		hImageOld = plv->himlSmall;
		plv->himlSmall = himl;
		if (himl)
		{
			ImageList_GetIconSize(himl, &plv->cxSmIcon, &plv->cySmIcon);
		}
		plv->cxItem = 16 * plv->cxLabelChar + plv->cxSmIcon;
		if (plv->hwndHdr)
		{
			SendMessage(plv->hwndHdr, HDM_SETIMAGELIST, 0, (LPARAM)himl);
		}
		break;

	case LVSIL_STATE:
		if (himl)
		{
			ImageList_GetIconSize(himl, &plv->cxState, &plv->cyState);
		}
		else
		{
			plv->cxState = 0;
		}
		hImageOld = plv->himlState;
		plv->himlState = himl;
		break;

	default:
#ifdef LVDEBUG
		DebugMsg(TF_LISTVIEW, TEXT("sh TR - LVM_SETIMAGELIST: unrecognized iImageList"));
#endif
		break;
	}

	if (himl && !(plv->hwnd->ci.style & LVS_SHAREIMAGELISTS))
	{
		ImageList_SetBkColor(himl, plv->clrBk);
	}

	if (ListView_Count(plv) > 0)
	{
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
	}

	return hImageOld;
}

#ifdef UNICODE
BOOL ListView_OnGetItemA(SListViewData *plv, LV_ITEMA *plvi)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	BOOL fRet;

	//HACK ALERT -- this code assumes that LV_ITEMA is exactly the same
	// as LV_ITEMW except for the pointer to the string.
	//_ASSERT(sizeof(LV_ITEMA) == sizeof(LV_ITEMW))

	if (!plvi)
	{
		return FALSE;
	}

	if ((plvi->mask & LVIF_TEXT) && (plvi->pszText != NULL))
	{
		pszC = plvi->pszText;
		pszW = (LPWSTR)LocalAlloc(LMEM_FIXED, plvi->cchTextMax * sizeof(WCHAR));
		if (pszW == NULL)
		{
			return FALSE;
		}
		plvi->pszText = (LPSTR)pszW;
	}

	fRet = ListView_OnGetItem(plv, (LVITEMEX *) plvi);

	if (pszW)
	{
		if (plvi->pszText != LPSTR_TEXTCALLBACKA)
		{
			if (fRet && plvi->cchTextMax)
			{
				ConvertWToAN(plv->hwnd->ci.uiCodePage, pszC, plvi->cchTextMax, (LPWSTR)plvi->pszText, -1);
			}
			plvi->pszText = pszC;
		}

		LocalFree(pszW);
	}

	return fRet;

}
#endif

BOOL ListView_OnGetItem(SListViewData *plv, LVITEMEX *plvi)
{
	UINT mask;
	LISTITEM *pitem = NULL;
	NMLVDISPINFOEX nm;

	if (plvi == NULL)
	{
		RIPMSG(0, "LVM_GET(ITEM|ITEMTEXT): Invalid pitem = NULL");
		return FALSE;
	}

	if (!ListView_IsValidItemNumber(plv, plvi->iItem))
	{
#ifdef DEBUG
		// owner data views (e.g. docfind) may change the number of items in listview
		// while we are doing something, thus hitting this rip
		if (!ListView_IsOwnerData(plv))
		{
			RIPMSG(0, "LVM_GET(ITEM|ITEMTEXT|ITEMSTATE): item=%d does not exist", plvi->iItem);
		}
#endif
		return FALSE;
	}

	nm.item.mask = 0;
	mask = plvi->mask;

	if (!ListView_IsOwnerData(plv))
	{
		// Standard listviews
		pitem = ListView_FastGetItemPtr(plv, plvi->iItem);
		_ASSERT(pitem);

		// Handle sub-item cases for report view
		//
		if (plvi->iSubItem != 0)
		{
			LISTSUBITEM lsi;

			ListView_GetSubItem(plv, plvi->iItem, plvi->iSubItem, &lsi);
			if (mask & LVIF_TEXT)
			{
				if (lsi.pszText != LPSTR_TEXTCALLBACK)
				{
					IUIStr_GetPtr(lsi.pszText ? lsi.pszText : TEXT(""), plvi->pszText, plvi->cchTextMax);
				}
				else
				{
					// if this is LVIF_NORECOMPUTE we will update pszText later
					nm.item.mask |= LVIF_TEXT;
				}
			}

			if ((mask & LVIF_IMAGE) && (plv->exStyle & LVS_EX_SUBITEMIMAGES))
			{
				plvi->iImage = lsi.iImage;
				if (lsi.iImage == I_IMAGECALLBACK)
				{
					nm.item.mask |= LVIF_IMAGE;
				}
			}

			if (mask & LVIF_STATE)
			{

				if (ListView_FullRowSelect(plv))
				{
					// if we're in full row select,
					// the state bit for select and focus follows column 0.
					lsi.state |= pitem->state & (LVIS_SELECTED | LVIS_FOCUSED | LVIS_DROPHILITED);
				}

				plvi->state = lsi.state & plvi->stateMask;


				if (plv->stateCallbackMask)
				{
					nm.item.stateMask = (plvi->stateMask & plv->stateCallbackMask);
					if (nm.item.stateMask)
					{
						nm.item.mask |= LVIF_STATE;
						nm.item.state = 0;
					}
				}
			}

			if (mask & LVIF_BINDWL)
			{
				plvi->hBindWindowless = lsi.hBindWindowless;
			}
		}
		else
		{

			if (mask & LVIF_TEXT)
			{
				if (pitem->pszText != LPSTR_TEXTCALLBACK)
				{
					IUIStr_GetPtr(pitem->pszText ? pitem->pszText : TEXT(""), plvi->pszText, plvi->cchTextMax);
				}
				else
				{
					// if this is LVIF_NORECOMPUTE we will update pszText later
					nm.item.mask |= LVIF_TEXT;
				}
			}

			if (mask & LVIF_IMAGE)
			{
				plvi->iImage = pitem->iImage;
				if (pitem->iImage == I_IMAGECALLBACK)
				{
					nm.item.mask |= LVIF_IMAGE;
				}
			}

			if (mask & LVIF_INDENT)
			{
				plvi->iIndent = pitem->iIndent;
				if (pitem->iIndent == I_INDENTCALLBACK)
				{
					nm.item.mask |= LVIF_INDENT;
				}
			}

			if (mask & LVIF_STATE)
			{
				plvi->state = (pitem->state & plvi->stateMask);

				if (plv->stateCallbackMask)
				{
					nm.item.stateMask = (plvi->stateMask & plv->stateCallbackMask);
					if (nm.item.stateMask)
					{
						nm.item.mask |= LVIF_STATE;
						nm.item.state = 0;
					}
				}
			}

			if (mask & LVIF_BINDWL)
			{
				plvi->hBindWindowless = pitem->hBindWindowless;
			}
		}

		if (mask & LVIF_PARAM)
		{
			plvi->lParam = pitem->lParam;
		}
	}
	else
	{
		// Complete call back for info...

		// Handle sub-item cases for report view
		//
		if (plvi->iSubItem != 0)
		{
			// if there are no subitem images, don't query for them
			if (!(plv->exStyle & LVS_EX_SUBITEMIMAGES))
			{
				mask &= ~LVIF_IMAGE;
			}

			// don't allow indent on the non-0th column
			mask &= ~LVIF_INDENT;
		}

		if (mask & LVIF_PARAM)
		{
			plvi->lParam = 0L;    // Dont have any to return now...
		}

		if (mask & LVIF_STATE)
		{
			plvi->state = 0;

			if ((plvi->iSubItem == 0) || ListView_FullRowSelect(plv))
			{
				if (plvi->iItem == plv->iFocus)
				{
					plvi->state |= LVIS_FOCUSED;
				}

				//L if (plv->plvrangeSel->lpVtbl->IsSelected(plv->plvrangeSel, plvi->iItem) == S_OK)
				//{
				//	plvi->state |= LVIS_SELECTED;
				//}

				//if (plv->plvrangeCut->lpVtbl->IsSelected(plv->plvrangeCut, plvi->iItem) == S_OK)
				//{
				//	plvi->state |= LVIS_CUT;
				//}

				if (plvi->iItem == plv->iDropHilite)
				{
					plvi->state |= LVIS_DROPHILITED;
				}

				plvi->state &= plvi->stateMask;
			}

			if (plv->stateCallbackMask)
			{
				nm.item.stateMask = (plvi->stateMask & plv->stateCallbackMask);
				if (nm.item.stateMask)
				{
					nm.item.mask |= LVIF_STATE;
					nm.item.state = 0;
				}
			}
		}

		nm.item.mask |= (mask & (LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT));
	}

	if (mask & LVIF_NORECOMPUTE)
	{
		if (nm.item.mask & LVIF_TEXT)
		{
			plvi->pszText = LPSTR_TEXTCALLBACK;
		}
	}
	else if (nm.item.mask)
	{
		nm.item.iItem  = plvi->iItem;
		nm.item.iSubItem = plvi->iSubItem;
		if (ListView_IsOwnerData(plv))
		{
			nm.item.lParam = 0L;
		}
		else
		{
			nm.item.lParam = pitem->lParam;
		}

		// just in case LVIF_IMAGE is set and callback doesn't fill it in
		// ... we'd rather have a -1 than whatever garbage is on the stack
		nm.item.iImage = -1;
		nm.item.iIndent = 0;
		if (nm.item.mask & LVIF_TEXT)
		{
			RIPMSG(plvi->pszText != NULL, "LVM_GET(ITEM|ITEMTEXT) null string pointer");

			if (plvi->pszText)
			{
				nm.item.pszText = plvi->pszText;
				nm.item.cchTextMax = plvi->cchTextMax;

				// Make sure the buffer is zero terminated...
				if (nm.item.cchTextMax)
				{
					*nm.item.pszText = 0;
				}
			}
			else
			{
				// Don't make caller smash null pointer
				nm.item.mask &= ~LVIF_TEXT;
			}
		}

		CCSendNotify(&plv->hwnd->ci, LVN_GETDISPINFO, &nm.hdr);

		// use nm.item.mask to give the app a chance to change values
		if (nm.item.mask & LVIF_INDENT)
		{
			plvi->iIndent = nm.item.iIndent;
		}
		if (nm.item.mask & LVIF_STATE)
		{
			plvi->state ^= ((plvi->state ^ nm.item.state) & nm.item.stateMask);
		}
		if (nm.item.mask & LVIF_IMAGE)
		{
			plvi->iImage = nm.item.iImage;
		}
		if (nm.item.mask & LVIF_BINDWL)
		{
			plvi->hBindWindowless = nm.item.hBindWindowless;
		}
		if (nm.item.mask & LVIF_TEXT)
		{
			if (mask & LVIF_TEXT)
			{
				plvi->pszText = CCReturnDispInfoText(nm.item.pszText, plvi->pszText, plvi->cchTextMax);
			}
			else
			{
				plvi->pszText = nm.item.pszText;
			}
		}

		if (pitem && (nm.item.mask & LVIF_DI_SETITEM))
		{

			// BUGBUG HACKHACK
			//
			// The SendNotify above can set about a terrible series of events
			// whereby asking for DISPINFO causes the shell to look around
			// (call peekmessage) to see if its got a new async icon for the
			// listview.  This lets other messages be delivered, such as an
			// UPDATEIMAGE of Index == -1 (if the user is changing icon sizing
			// at the same time).  This causes a re-enumeration of the desktop
			// and hence this very listview is torn down and rebuilt while
			// we're sitting here for the DISPINFO to finish.  Thus, as a cheap
			// and dirty solution, I check to see if the item I think I have
			// is the same one I had when I made the notify, and if not, I
			// bail.  Don't blame me, I'm just cleaning up the mess.

			if (!EVAL(pitem == ListView_GetItemPtr(plv, plvi->iItem)))
			{
				return FALSE;
			}

			if (nm.item.iSubItem == 0)
			{
				//DebugMsg(TF_LISTVIEW, "SAVING ITEMS!");
				if (nm.item.mask & LVIF_IMAGE)
				{
					pitem->iImage = (short) nm.item.iImage;
				}

				if (nm.item.mask & LVIF_INDENT)
				{
					pitem->iIndent = (short) nm.item.iIndent;
				}

				if (nm.item.mask & LVIF_TEXT)
					if (nm.item.pszText)
					{
						Str_Set(&pitem->pszText, nm.item.pszText);
					}

				if (nm.item.mask & LVIF_STATE)
				{
					pitem->state ^= ((pitem->state ^ nm.item.state) & nm.item.stateMask);
				}
			}
			else
			{
				ListView_SetSubItem(plv, &nm.item);
			}
		}
	}

	return TRUE;
}

#ifdef UNICODE
BOOL ListView_OnSetItemA(SListViewData *plv, LV_ITEMA *plvi)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	BOOL fRet;

	// Let ListView_OnSetItem() handle owner-data validation

	//HACK ALERT -- this code assumes that LV_ITEMA is exactly the same
	// as LV_ITEMW except for the pointer to the string.
	_ASSERT(sizeof(LV_ITEMA) == sizeof(LV_ITEMW));

	if (!plvi)
	{
		return FALSE;
	}

	if ((plvi->mask & LVIF_TEXT) && (plvi->pszText != NULL))
	{
		pszC = plvi->pszText;
		pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszC);
		if (pszW == NULL)
		{
			return FALSE;
		}
		plvi->pszText = (LPSTR)pszW;
	}

	fRet = ListView_OnSetItem(plv, (const LVITEMEX *) plvi);

	if (pszW != NULL)
	{
		plvi->pszText = pszC;

		FreeProducedString(pszW);
	}

	return fRet;

}
#endif

BOOL ListView_OnSetItem(SListViewData *plv, const LVITEMEX *plvi)
{
	LISTITEM *pitem = NULL;
	UINT mask;
	UINT maskChanged;
	UINT rdwFlags = RDW_INVALIDATE;
	int i;
	UINT stateOld, stateNew;
	BOOL fFocused = FALSE;
	BOOL fSelected = FALSE;
	BOOL fStateImageChanged = FALSE;

	if (ListView_IsOwnerData(plv))
	{
		RIPMSG(0, "LVM_SETITEM: Invalid for owner-data listview");
		return FALSE;
	}

	if (!plvi)
	{
		return FALSE;
	}

	_ASSERT(plvi->iSubItem >= 0);

	if (plv->himl && (plv->clrBk != ImageList_GetBkColor(plv->himl)))
	{
		rdwFlags |= RDW_ERASE;
	}

	mask = plvi->mask;
	if (!mask)
	{
		return TRUE;
	}

	// If we're setting a subitem, handle it elsewhere...
	//
	if (plvi->iSubItem > 0)
	{
		return ListView_SetSubItem(plv, plvi);
	}

	i = plvi->iItem;

	ListView_InvalidateTTLastHit(plv, i);

	pitem = ListView_GetItemPtr(plv, i);
	if (!pitem)
	{
		return FALSE;
	}

	//REVIEW: This is a BOGUS HACK, and should be fixed.
	//This incorrectly calculates the old state (since we may
	// have to send LVN_GETDISPINFO to get it).
	//
	stateOld = stateNew = 0;
	if (mask & LVIF_STATE)
	{
		stateOld = pitem->state & plvi->stateMask;
		stateNew = plvi->state & plvi->stateMask;
	}

	// Prevent multiple selections in a single-select listview.
	if ((plv->hwnd->ci.style & LVS_SINGLESEL) && (mask & LVIF_STATE) && (stateNew & LVIS_SELECTED))
	{
		ListView_DeselectAll(plv, i);

		// Refresh the old state information
		stateOld = pitem->state & plvi->stateMask;
	}

	{
		NM_LISTVIEW nm;
		nm.hdr.code = LVN_ITEMCHANGING;
		nm.hdr.hwndFrom = HWND(plv->hwnd);
		nm.hdr.idFrom = 0;

		nm.iItem = i;
		nm.iSubItem = 0;
		nm.uNewState = stateNew;
		nm.uOldState = stateOld;
		nm.uChanged = mask;
		nm.ptAction.x = 0;
		nm.ptAction.y = 0;
		nm.lParam = pitem->lParam;

		RoutedEventArgs arg;
		arg.hSender = plv->hwnd;
		arg.hOriginalSender = plv->hwnd;
		arg.RoutedEvent = WM_NOTIFY;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = nm.hdr.idFrom;
		arg.lParam = (LPARAM)&nm;
		if (RaiseEvent(arg.hOriginalSender, &arg) != 0)
		{
			return FALSE;
		}
	}

	maskChanged = 0;
	if (mask & LVIF_STATE)
	{
		UINT change = (pitem->state ^ plvi->state) & plvi->stateMask;

		if (change)
		{
			pitem->state ^= change;

			maskChanged |= LVIF_STATE;

			// the selection state has changed.. update selected count
			if (change & LVIS_SELECTED)
			{
				fSelected = TRUE;

				if (pitem->state & LVIS_SELECTED)
				{
					plv->nSelected++;
				}
				else
				{
					if (plv->nSelected > 0)
					{
						plv->nSelected--;
					}
				}
			}

			// For some bits we can only invert the label area...
			// fSelectOnlyChange = ((change & ~(LVIS_SELECTED | LVIS_FOCUSED | LVIS_DROPHILITED)) == 0);
			// fEraseItem = ((change & ~(LVIS_SELECTED | LVIS_DROPHILITED)) != 0);

			// try to steal focus from the previous guy.
			if (change & LVIS_FOCUSED)
			{
				BOOL fUnfolded = ListView_IsItemUnfolded(plv, plv->iFocus);
				int iOldFocus = plv->iFocus;
				RECT rcLabel;

				fFocused = TRUE;

				if (plv->iFocus != i)
				{
					if ((plv->iFocus == -1) || ListView_OnSetItemState(plv, plv->iFocus, 0, LVIS_FOCUSED))
					{
						_ASSERT(pitem->state & LVIS_FOCUSED);
						plv->iFocus = i;
						if (plv->iMark == -1)
						{
							plv->iMark = i;
						}
					}
					else
					{
						fFocused = FALSE;
						pitem->state &= ~LVIS_FOCUSED;
					}
				}
				else
				{
					_ASSERT(!(pitem->state & LVIS_FOCUSED));
					plv->iFocus = -1;
				}

				// If we were previously unfolded and we move the focus we must
				// attempt to refresh the previous focus owner to referect this change.

				if (fUnfolded && !ListView_IsItemUnfolded(plv, iOldFocus) && (plv->iItemDrawing != iOldFocus))
				{
					ListView_GetUnfoldedRect(plv, iOldFocus, &rcLabel);
					RedrawWindow(GetHostWnd(plv->hwnd), &rcLabel, NULL, RDW_INVALIDATE | RDW_ERASE);
				}

				// Kill the tooltip if focus moves, it causes us headaches otherwise!
				ListView_PopBubble(plv);
			}

			if (change & LVIS_CUT ||
				plv->clrTextBk == CLR_NONE)
			{
				rdwFlags |= RDW_ERASE;
			}

			if (change & LVIS_OVERLAYMASK)
			{
				// Overlay changed, so need to blow away icon region cache
				if (pitem->hrgnIcon)
				{
					if (pitem->hrgnIcon != (HANDLE)(-1))
					{
						DeleteObject(pitem->hrgnIcon);
					}
					pitem->hrgnIcon = NULL;
				}
			}

			fStateImageChanged = (change & LVIS_STATEIMAGEMASK);

		}
	}

	if (mask & LVIF_TEXT)
	{
		// need to do this now because we're changing the text
		// so we need to get the rect of the thing before the text changes
		// but don't redraw the item we are currently painting
		if (plv->iItemDrawing != i)
		{
			ListView_InvalidateItemEx(plv, i, 0, FALSE,
				RDW_INVALIDATE | RDW_ERASE, LVIF_TEXT);
		}

		if (!Str_Set(&pitem->pszText, plvi->pszText))
		{
			return FALSE;
		}

		plv->rcView.left = RECOMPUTE;
		ListView_SetSRecompute(pitem);
		maskChanged |= LVIF_TEXT;
	}

	if (mask & LVIF_INDENT)
	{
		if (pitem->iIndent != plvi->iIndent)
		{
			pitem->iIndent = (short) plvi->iIndent;
			maskChanged |= LVIF_INDENT;

			if (ListView_IsReportView(plv))
			{
				rdwFlags |= RDW_ERASE;
			}
		}
	}

	if (mask & LVIF_IMAGE)
	{
		if (pitem->iImage != plvi->iImage)
		{
			pitem->iImage = (short) plvi->iImage;
			maskChanged |= LVIF_IMAGE;

			if (pitem->hrgnIcon)
			{
				if (pitem->hrgnIcon != (HANDLE)(-1))
				{
					DeleteObject(pitem->hrgnIcon);
				}
				pitem->hrgnIcon = NULL;
			}

			// erase if there was a set image
			if (pitem->iImage != I_IMAGECALLBACK)
			{
				rdwFlags |= RDW_ERASE;
			}
		}
	}

	if (mask & LVIF_PARAM)
	{
		if (pitem->lParam != plvi->lParam)
		{
			pitem->lParam = plvi->lParam;
			maskChanged |= LVIF_PARAM;
		}
	}

	if (mask & LVIF_BINDWL)
	{
		if (pitem->hBindWindowless != plvi->hBindWindowless)
		{
			pitem->hBindWindowless = plvi->hBindWindowless;
			SetBindItemControlParent(pitem->hBindWindowless, plv->hwnd, (ULONG_PTR)pitem);
			maskChanged |= LVIF_BINDWL;
		}
	}

	if (maskChanged)
	{
		// don't redraw the item we are currently painting
		if (plv->iItemDrawing != i)
		{
			ListView_InvalidateItemEx(plv, i, 0, FALSE, rdwFlags, maskChanged);
		}

		DebugMsg(DM_LVSENDCHANGE, TEXT("LV - SendChange %d %d %d %d"), i, stateOld, stateNew, maskChanged);

		{
			NM_LISTVIEW nm;
			nm.hdr.code = LVN_ITEMCHANGED;
			nm.hdr.hwndFrom = HWND(plv->hwnd);
			nm.hdr.idFrom = 0;

			nm.iItem = i;
			nm.iSubItem = 0;
			nm.uNewState = stateNew;
			nm.uOldState = stateOld;
			nm.uChanged = maskChanged;
			nm.ptAction.x = 0;
			nm.ptAction.y = 0;
			nm.lParam = pitem->lParam;

			RoutedEventArgs arg;
			arg.hSender = plv->hwnd;
			arg.hOriginalSender = plv->hwnd;
			arg.RoutedEvent = WM_NOTIFY;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = nm.hdr.idFrom;
			arg.lParam = (LPARAM)&nm;
			LRESULT lr = RaiseEvent(arg.hOriginalSender, &arg);
		}

		if (maskChanged & LVIF_TEXT)
		{
			MyNotifyWinEvent(EVENT_OBJECT_NAMECHANGE, GetHostWnd(plv->hwnd), OBJID_CLIENT, i + 1);
		}

		if (maskChanged & LVIF_STATE)
		{
			if (fFocused)
			{
				ListView_NotifyFocusEvent(plv);
			}

			if (fSelected)
			{
				if (stateNew & LVIS_SELECTED)
				{
					MyNotifyWinEvent((plv->nSelected == 1) ? EVENT_OBJECT_SELECTION :
						EVENT_OBJECT_SELECTIONADD, GetHostWnd(plv->hwnd), OBJID_CLIENT, i + 1);
				}
				else
				{
					MyNotifyWinEvent(EVENT_OBJECT_SELECTIONREMOVE, GetHostWnd(plv->hwnd), OBJID_CLIENT, i + 1);
				}
			}

			if (fStateImageChanged)
			{
				MyNotifyWinEvent(EVENT_OBJECT_STATECHANGE, GetHostWnd(plv->hwnd), OBJID_CLIENT, i + 1);
			}
		}
	}
	return TRUE;
}

UINT PASCAL ListView_OnGetItemState(SListViewData *plv, int i, UINT mask)
{
	LVITEMEX lvi;

	lvi.mask = LVIF_STATE;
	lvi.stateMask = mask;
	lvi.iItem = i;
	lvi.iSubItem = 0;
	if (!ListView_OnGetItem(plv, &lvi))
	{
		return 0;
	}

	return lvi.state;
}


BOOL PASCAL ListView_OnSetItemState(SListViewData *plv, int i, UINT data, UINT mask)
{
	UINT rdwFlags = RDW_INVALIDATE;
	LVITEMEX lvi;

	lvi.mask    = LVIF_STATE;
	lvi.state   = data;
	lvi.stateMask = mask;
	lvi.iItem   = i;
	lvi.iSubItem = 0;

	// if the item is -1, we will do it for all items.  We special case
	// a few cases here as to speed it up.  For example if the mask is
	// LVIS_SELECTED and data is zero it implies that we will deselect
	// all items...
	//
	if (ListView_IsOwnerData(plv))
	{
		UINT uOldData = 0;

		// these are the only two we handled
		mask &= (LVIS_SELECTED | LVIS_FOCUSED | LVIS_CUT | LVIS_DROPHILITED);
		if (!mask)
		{
			return TRUE;
		}

		if (plv->clrTextBk == CLR_NONE
			|| (plv->himl && (plv->clrBk != ImageList_GetBkColor(plv->himl))))
		{
			rdwFlags |= RDW_ERASE;
		}

		if (i == -1)
		{

			// request selection state change for all
			if (mask & LVIS_SELECTED)
			{
				if (data & LVIS_SELECTED)
				{
					// set selection
					if ((plv->hwnd->ci.style & LVS_SINGLESEL))
					{
						// cant make multiple selections in a single-select listview.
						return (FALSE);
					}

					if (plv->cTotalItems)
					{
						//L if (FAILED(plv->plvrangeSel->lpVtbl->IncludeRange(plv->plvrangeSel, 0, plv->cTotalItems - 1)))
						//{
						//	return (FALSE);
						//}
					}

					RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, rdwFlags);

				}
				else
				{
					// clear selection
					if (plv->nSelected > 0)
					{
						//L ListView_InvalidateSelectedOrCutOwnerData(plv, plv->plvrangeSel);
						//if (FAILED(plv->plvrangeSel->lpVtbl->Clear(plv->plvrangeSel)))
						//{
						//	return FALSE;
						//}
					}
					else
					{
						// if nothing was selected, then there's nothing to clear
						// no change.
						mask &= ~ LVIS_SELECTED;
					}
				}
				uOldData |= (LVIS_SELECTED & (mask ^ data));

				// Update our internal count to what the list thinks is the number selected...
				//L plv->plvrangeSel->lpVtbl->CountIncluded(plv->plvrangeSel, &plv->nSelected);

			}

			// can maybe combine with above code...
			if (mask & LVIS_CUT)
			{
				if (data & LVIS_CUT)
				{
					// set selection

					if (plv->cTotalItems)
					{
						//L if (FAILED(plv->plvrangeCut->lpVtbl->IncludeRange(plv->plvrangeCut, 0, plv->cTotalItems - 1)))
						//{
						//	return (FALSE);
						//}
					}

					RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, rdwFlags);

				}
				else
				{
					// clear selection
					//L if (plv->plvrangeCut->lpVtbl->IsEmpty(plv->plvrangeCut) != S_OK)
					//{
					//	ListView_InvalidateSelectedOrCutOwnerData(plv, plv->plvrangeCut);
					//	if (FAILED(plv->plvrangeCut->lpVtbl->Clear(plv->plvrangeCut)))
					//	{
					//		return (FALSE);
					//	}

					//}
					//else
					{
						// if nothing was selected, then there's nothing to clear
						// no change.
						mask &= ~ LVIS_CUT;
					}
				}
				uOldData |= (LVIS_CUT & (mask ^ data));

			}

			// request focus state change
			if (mask & LVIS_FOCUSED)
			{
				if (data & LVIS_FOCUSED)
				{
					// cant set focus to all
					return (FALSE);
				}
				else if (plv->iFocus != -1)
				{
					int iOldFocus = plv->iFocus;
					// clear focus
					uOldData |= (LVIS_FOCUSED & (mask ^ data));
					plv->iFocus = -1;
					// notify that the old focus is being lost
					DebugMsg(DM_LVSENDCHANGE, TEXT("VLV: LVN_ITEMCHANGED: %d %d %d"), iOldFocus, LVIS_FOCUSED, 0);
					ListView_SendChange(plv, iOldFocus, 0, LVN_ITEMCHANGED, LVIS_FOCUSED, 0, LVIF_STATE, 0);
					ListView_InvalidateFoldedItem(plv, iOldFocus, TRUE, RDW_INVALIDATE | RDW_ERASE);
				}
			}

			if (mask & LVIS_DROPHILITED)
			{
				if (data & LVIS_DROPHILITED)
				{
					// cant set focus to all
					return (FALSE);
				}
				else if (plv->iDropHilite != -1)
				{
					int iOldDropHilite = plv->iDropHilite;
					// clear focus
					uOldData |= (LVIS_FOCUSED & (mask ^ data));
					plv->iDropHilite = -1;
					// notify that the old focus is being lost
					ListView_SendChange(plv, iOldDropHilite, 0, LVN_ITEMCHANGED, LVIS_DROPHILITED, 0, LVIF_STATE, 0);
					ListView_InvalidateFoldedItem(plv, iOldDropHilite, TRUE, RDW_INVALIDATE | RDW_ERASE);
				}
			}

			// invalidate and notify if there was a change
			if (uOldData ^ (data & mask))
			{
				DebugMsg(DM_LVSENDCHANGE, TEXT("VLV: LVN_ITEMCHANGED: %d %d %d"), i, uOldData, data);
				ListView_SendChange(plv, i, 0, LVN_ITEMCHANGED, uOldData, data, LVIF_STATE, 0);

				if (mask & LVIS_SELECTED)
				{
					// Tell accessibility, "Selection changed in a complex way"
					// (There is no "select all" or "select none" notification)
					MyNotifyWinEvent(EVENT_OBJECT_SELECTIONWITHIN, GetHostWnd(plv->hwnd), OBJID_CLIENT, CHILDID_SELF);
				}

			}
		}
		else
		{
			if (!ListView_IsValidItemNumber(plv, i))
			{
				return (FALSE);
			}

			// request selection state change
			// and the selection state is new...
			if ((mask & LVIS_SELECTED))
			{


				//L if (((plv->plvrangeSel->lpVtbl->IsSelected(plv->plvrangeSel, i) == S_OK) ? LVIS_SELECTED : 0) ^ (data & LVIS_SELECTED))
				//{
				//	if (data & LVIS_SELECTED)
				//	{
				//		// set selection
				//		if ((plv->hwnd->ci.style & LVS_SINGLESEL))
				//		{
				//			// in single selection mode, we need to deselect everything else
				//			if (!ListView_OnSetItemState(plv, -1, 0, LVIS_SELECTED))
				//			{
				//				return FALSE;
				//			}
				//		}

				//		// now select the new item
				//		if (FAILED(plv->plvrangeSel->lpVtbl->IncludeRange(plv->plvrangeSel, i, i)))
				//		{
				//			return FALSE;
				//		}
				//	}
				//	else
				//	{
				//		// clear selection
				//		if (FAILED(plv->plvrangeSel->lpVtbl->ExcludeRange(plv->plvrangeSel, i, i)))
				//		{
				//			return (FALSE);
				//		}
				//	}

				//	// something actually changed (or else we wouldn't be in this
				//	// if block
				//	uOldData |= (LVIS_SELECTED & (mask ^ data));

				//}
				//else
				{

					// nothing changed... so make the uOldData be the same for this bit
					// else make this the same as
					uOldData |= (LVIS_SELECTED & (mask & data));
				}

				// Update our internal count to what the list thinks is the number selected...
				//L plv->plvrangeSel->lpVtbl->CountIncluded(plv->plvrangeSel, &plv->nSelected);
			}

			if ((mask & LVIS_CUT))
			{

				//L if (((plv->plvrangeCut->lpVtbl->IsSelected(plv->plvrangeCut, i) == S_OK) ? LVIS_CUT : 0) ^ (data & LVIS_CUT))
				//{
				//	if (data & LVIS_CUT)
				//	{
				//		// now select the new item
				//		if (FAILED(plv->plvrangeCut->lpVtbl->IncludeRange(plv->plvrangeCut, i, i)))
				//		{
				//			return FALSE;
				//		}
				//	}
				//	else
				//	{
				//		// clear selection
				//		if (FAILED(plv->plvrangeCut->lpVtbl->ExcludeRange(plv->plvrangeCut, i, i)))
				//		{
				//			return (FALSE);
				//		}
				//	}

				//	// something actually changed (or else we wouldn't be in this
				//	// if block
				//	uOldData |= (LVIS_CUT & (mask ^ data));
				//	rdwFlags |= RDW_ERASE;

				//}
				//else
				{

					// nothing changed... so make the uOldData be the same for this bit
					// else make this the same as
					uOldData |= (LVIS_CUT & (mask & data));
				}
			}

			// request focus state change
			if (mask & LVIS_FOCUSED)
			{
				int iOldFocus = plv->iFocus;

				if (data & LVIS_FOCUSED)
				{
					// set focus
					if (i != plv->iFocus)
					{
						// we didn't have the focus before
						plv->iFocus = i;
						if (plv->iMark == -1)
						{
							plv->iMark = i;
						}
						if (iOldFocus != -1)
						{

							// we're stealing it from someone
							// notify of the change
							DebugMsg(DM_LVSENDCHANGE, TEXT("VLV: LVN_ITEMCHANGED: %d %d %d"), iOldFocus, LVIS_FOCUSED, 0);
							ListView_SendChange(plv, iOldFocus, 0, LVN_ITEMCHANGED, LVIS_FOCUSED, 0, LVIF_STATE, 0);

						}
					}
					else
					{
						// we DID have the focus before
						uOldData |= LVIS_FOCUSED;
					}
				}
				else
				{
					// clear focus
					if (i == plv->iFocus)
					{
						plv->iFocus = -1;
						uOldData |= LVIS_FOCUSED;
					}
				}

			}

			// request focus state change
			if (mask & LVIS_DROPHILITED)
			{
				int iOldDropHilite = plv->iDropHilite;

				if (data & LVIS_DROPHILITED)
				{
					// set Drop Hilite
					if (i != plv->iDropHilite)
					{
						// we didn't have the Drop Hilite before
						plv->iDropHilite = i;
						if (iOldDropHilite != -1)
						{

							// we're stealing it from someone
							// notify of the change
							ListView_SendChange(plv, iOldDropHilite, 0, LVN_ITEMCHANGED, LVIS_DROPHILITED, 0, LVIF_STATE, 0);
							ListView_InvalidateFoldedItem(plv, iOldDropHilite, TRUE, RDW_INVALIDATE | RDW_ERASE);

						}
					}
					else
					{
						// we DID have the Drop Hilite before
						uOldData |= LVIS_DROPHILITED;
					}
				}
				else
				{
					// clear Drop Hilite
					if (i == plv->iDropHilite)
					{
						plv->iDropHilite = -1;
						uOldData |= LVIS_DROPHILITED;
					}
				}

			}

			// invalidate and notify if there was a change
			if (uOldData ^ (data & mask))
			{
				DebugMsg(DM_LVSENDCHANGE, TEXT("VLV: LVN_ITEMCHANGED: %d %d %d"), i, uOldData, data);
				ListView_SendChange(plv, i, 0, LVN_ITEMCHANGED, uOldData, data, LVIF_STATE, 0);
				ListView_InvalidateItem(plv, i, TRUE, rdwFlags);

				// Kill the tooltip if focus moves, it causes us headaches otherwise!
				if ((uOldData ^ (data & mask)) & LVIS_FOCUSED)
				{
					ListView_PopBubble(plv);
					ListView_NotifyFocusEvent(plv);
				}

				// Tell accessibility about the changes
				if (mask & LVIS_SELECTED)
				{
					UINT event;

					if (data & LVIS_SELECTED)
					{
						if (plv->nSelected == 1)
						{
							event = EVENT_OBJECT_SELECTION;    // this object is the entire selection
						}
						else
						{
							event = EVENT_OBJECT_SELECTIONADD;    // this object is selected
						}
					}
					else
					{
						event = EVENT_OBJECT_SELECTIONREMOVE;    // this object is unselected
					}
					MyNotifyWinEvent(event, GetHostWnd(plv->hwnd), OBJID_CLIENT, i + 1);
				}
			}
		}

	}
	else
	{

		if (i != -1)
		{
			return ListView_OnSetItem(plv, &lvi);
		}
		else
		{
			UINT flags = LVNI_ALL;


			if (data == 0)
			{
				switch (mask)
				{
				case LVIS_SELECTED:
					flags = LVNI_SELECTED;
					break;
				case LVIS_CUT:
					flags = LVNI_CUT;
					break;
				}
			}
			else if ((plv->hwnd->ci.style & LVS_SINGLESEL) && (mask == LVIS_SELECTED))
			{
				return FALSE;    /* can't select all in single-select listview */
			}
			else if ((mask & data) & LVIS_FOCUSED)
			{
				return FALSE; // can't set focus to everything
			}

			//
			// Now iterate over all of the items that match our criteria and
			// set their new value.
			//
			while ((lvi.iItem = ListView_OnGetNextItem(plv, lvi.iItem,
							flags)) != -1)
			{
				ListView_OnSetItem(plv, &lvi);
			}
		}
	}
	return (TRUE);
}

//
// Returns TRUE if the label of an item is not truncated (is unfolded) and FALSE
// otherwise. If FALSE is returned, it also fills the Unfolding text in pszText.
// If TRUE is returned, pszText is set to empty string.
//
BOOL ListView_IsItemUnfolded2(SListViewData *plv, int iItem, int iSubItem, LPTSTR pszText, int cchTextMax)
{
	BOOL bItemUnfolded = ListView_IsItemUnfolded(plv, iItem);

	if (pszText && cchTextMax > 0)    // Sanity checks on input params.
	{
		pszText[0] = 0;


		if (!bItemUnfolded)
		{
			RECT rcLabel;
			LVITEMEX item;

			item.iItem = iItem;
			item.iSubItem = iSubItem;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			if (!ListView_IsIconView(plv))
			{
				if (ListView_IsLabelTip(plv) || ListView_IsInfoTip(plv))
				{
					BOOL fSuccess;

					rcLabel.left = LVIR_LABEL;

					if (iSubItem)
					{
						rcLabel.top = iSubItem;
						fSuccess = ListView_OnGetSubItemRect(plv, iItem, &rcLabel);
					}
					else
					{
						fSuccess = ListView_OnGetItemRect(plv, iItem, &rcLabel);
					}

					if (fSuccess)
					{
						TCHAR szText[INFOTIPSIZE];

						item.pszText = szText;
						item.cchTextMax = min(ARRAYSIZE(szText), cchTextMax);
						if (ListView_OnGetItem(plv, &item) && item.pszText != LPSTR_TEXTCALLBACK)
						{
							SIZE siz;
							LVFAKEDRAW lvfd;
							int cx;

							ListView_BeginFakeCustomDraw(plv, &lvfd, &item);
							ListView_BeginFakeItemDraw(&lvfd);

							//        ---------Label width----------- ---Client width---
							cx = min(rcLabel.right - IUIGetGloabelVar()->g_cxLabelMargin, plv->sizeClient.cx);

							if (GetTextExtentPoint32(lvfd.nmcd.nmcd.hdc, item.pszText, lstrlen(item.pszText), &siz) &&
								(rcLabel.left + IUIGetGloabelVar()->g_cxLabelMargin + siz.cx) > cx)
							{
								lstrcpyn(pszText, item.pszText, item.cchTextMax);
							}
							else
							{
								// Not truncated after all
								bItemUnfolded = TRUE;
							}

							ListView_EndFakeItemDraw(&lvfd);
							ListView_EndFakeCustomDraw(&lvfd);
						}
					}
				}
			}
			else
			{
				// Large icon view is the only one that folds
				if (ListView_GetUnfoldedRect(plv, iItem, &rcLabel))
				{
					item.pszText = pszText;
					item.cchTextMax = cchTextMax;
					ListView_OnGetItem(plv, &item);
				}
				else
				{
					// Item was never folded
					bItemUnfolded = TRUE;
				}
			}
		}
	}
	return bItemUnfolded;
}

#ifdef UNICODE

// Rather than thunking to ListView_OnGetItemText, we let ListView_GetItemA
// do the work.

int PASCAL ListView_OnGetItemTextA(SListViewData *plv, int i, LV_ITEMA *plvi)
{
	if (!plvi)
	{
		return 0;
	}

	RIPMSG(plvi->pszText != NULL, "LVM_GETITEMTEXT null string pointer");

	plvi->mask = LVIF_TEXT;
	plvi->iItem = i;
	if (!ListView_OnGetItemA(plv, plvi))
	{
		return 0;
	}

	return lstrlenA(plvi->pszText);
}
#endif

int PASCAL ListView_OnGetItemText(SListViewData *plv, int i, LVITEMEX *plvi)
{
	if (!plvi)
	{
		return 0;
	}

	RIPMSG(plvi->pszText != NULL, "LVM_GETITEMTEXT null string pointer");

	plvi->mask = LVIF_TEXT;
	plvi->iItem = i;
	if (!ListView_OnGetItem(plv, plvi))
	{
		return 0;
	}

	return lstrlen(plvi->pszText);
}


#ifdef UNICODE
BOOL ListView_OnSetItemTextA(SListViewData *plv, int i, int iSubItem, LPCSTR pszText)
{
	LPWSTR pszW = NULL;
	BOOL fRet;

	// Let ListView_OnSetItemText() handle owner-data validation

	if (pszText != NULL)
	{
		pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszText);
		if (pszW == NULL)
		{
			return FALSE;
		}
	}

	fRet = ListView_OnSetItemText(plv, i, iSubItem, pszW);

	FreeProducedString(pszW);

	return fRet;
}
#endif

BOOL ListView_OnSetItemText(SListViewData *plv, int i, int iSubItem, LPCTSTR pszText)
{
	LVITEMEX lvi;

	if (ListView_IsOwnerData(plv))
	{
		RIPMSG(0, "LVM_SETITEMTEXT: Invalid for owner-data listview");
		return FALSE;
	}

	ListView_InvalidateTTLastHit(plv, i);

	lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPTSTR)pszText;
	lvi.iItem = i;
	lvi.iSubItem = iSubItem;

	return ListView_OnSetItem(plv, &lvi);
}

BOOL ListView_OnSetItemBindWL(SListViewData *plv, int i, int iSubItem, HWLWND hBind)
{
	LVITEMEX lvi;

	if (ListView_IsOwnerData(plv))
	{
		RIPMSG(0, "LVM_SETITEMTEXT: Invalid for owner-data listview");
		return FALSE;
	}

	ListView_InvalidateTTLastHit(plv, i);

	lvi.mask = LVIF_BINDWL;
	lvi.hBindWindowless = hBind;
	lvi.iItem = i;
	lvi.iSubItem = iSubItem;

	return ListView_OnSetItem(plv, &lvi);
}

HWLWND ListView_OnGetItemBindWL(SListViewData *plv, int i, int nSubItem)
{
	LVITEMEX lvi;

	lvi.mask = LVIF_BINDWL;
	lvi.iItem = i;
	lvi.iSubItem = nSubItem;
	if (!ListView_OnGetItem(plv, &lvi))
	{
		return 0;
	}

	return lvi.hBindWindowless;
}

VOID CALLBACK ImgCtxCallback(void *pvImgCtx, void *pvArg)
{
	SListViewData *plv = (SListViewData *)pvArg;
	ULONG ulState = 0;
	IUI::IImgCtx *pImgCtx = plv->pImgCtx;

	//L IImgCtx_GetStateInfo(pImgCtx, &ulState, &sizeImg, TRUE);

	if (ulState & (IMGLOAD_STOPPED | IMGLOAD_ERROR))
	{
		TraceMsg(TF_BKIMAGE, "Error!");
		plv->fImgCtxComplete = FALSE;
	}

	else if (ulState & IMGCHG_COMPLETE)
	{
		TraceMsg(TF_BKIMAGE, "Complete!");
		plv->fImgCtxComplete = TRUE;
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
	}
}

void ListView_ReleaseBkImage(SListViewData *plv)
{
	if (plv->pImgCtx)
	{
		//L IImgCtx_Release(plv->pImgCtx);
		plv->pImgCtx = NULL;

		if (plv->hpalHalftone)
		{
			// No need to delete the half tone palette since we really
			// share it with the image context and it will clean up.
			plv->hpalHalftone = NULL;
		}
	}

	if (plv->hbmBkImage)
	{
		DeleteObject(plv->hbmBkImage);
		plv->hbmBkImage = NULL;
	}

	if (plv->pszBkImage)
	{
		LocalFree(plv->pszBkImage);
		plv->pszBkImage = NULL;
	}
}

BOOL ListView_OnSetBkImage(SListViewData *plv, LPLVBKIMAGE pbi)
{
	LPCTSTR pszImage = pbi->pszImage;
	BOOL fRet = FALSE;

	switch (pbi->ulFlags & LVBKIF_SOURCE_MASK)
	{
	case LVBKIF_SOURCE_NONE:
		TraceMsg(TF_BKIMAGE, "LV SetBkImage to none");
		ListView_ReleaseBkImage(plv);
		break;

	case LVBKIF_SOURCE_HBITMAP:
		TraceMsg(TF_BKIMAGE, "LV SetBkImage to hBitmap %08lX", pbi->hbm);
		ListView_ReleaseBkImage(plv);
		if (pbi->hbm)
		{
			plv->hbmBkImage = pbi->hbm;
			_ASSERT(0); // KenSy hasn't implemented init from bitmap yet...
		}
		else
		{
			pbi->ulFlags &= ~LVBKIF_SOURCE_HBITMAP;
		}
		break;

	case LVBKIF_SOURCE_URL:
		//L		TraceMsg(TF_BKIMAGE, "LV SetBkImage to URL");
		//		ListView_ReleaseBkImage(plv);
		//		if (pszImage && pszImage[0])
		//		{
		//			HRESULT(*pfnCoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *);
		//			HRESULT hr;
		//			HMODULE hmodOLE;
		//
		//			plv->pszBkImage = (LPTSTR)LocalAlloc(LPTR, (lstrlen(pszImage) + 1) * sizeof(TCHAR));
		//			if (plv->pszBkImage == NULL)
		//			{
		//				TraceMsg(TF_BKIMAGE, "Wow, could not allocate memory for string!");
		//				return FALSE;
		//			}
		//			lstrcpy(plv->pszBkImage, pszImage);
		//
		//			if (((hmodOLE = GetModuleHandle(TEXT("OLE32"))) == NULL) ||
		//				((pfnCoCreateInstance = (HRESULT(*)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID *))GetProcAddress(hmodOLE, "CoCreateInstance")) == NULL))
		//			{
		//				TraceMsg(TF_BKIMAGE, "Could not find CoCreateInstance!");
		//				TraceMsg(TF_BKIMAGE, "Did the caller remember to call CoInitialize?");
		//				return FALSE;
		//			}
		//
		//			hr = pfnCoCreateInstance(&CLSID_IImgCtx, NULL, CLSCTX_INPROC_SERVER,
		//					&IID_IImgCtx, (LPVOID *)&plv->pImgCtx);
		//
		//			if (FAILED(hr))
		//			{
		//				TraceMsg(TF_BKIMAGE, "Could not create a pImgCtx!");
		//				TraceMsg(TF_BKIMAGE, "Did you remember to register IEIMAGE.DLL?");
		//				return FALSE;
		//			}
		//			//
		//			// Mirror the downloaded image if the listview window is RTL mirrored,
		//			// so that it would be displayed as is. [samera]
		//			//
		//			fl = ((IS_WINDOW_RTL_MIRRORED(GetHostWnd(plv->hwnd))) ? DWN_MIRRORIMAGE : 0);
		//
		//#ifdef UNICODE
		//			hr = IImgCtx_Load(plv->pImgCtx, pszImage, fl);
		//#else
		//			{
		//				//
		//				// Darn IImgCtx::Load only takes wide URLs, sigh
		//				//
		//				LPWSTR pwszImage;
		//
		//				pwszImage = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszImage);
		//				if (!pwszImage)
		//				{
		//					IImgCtx_Release(plv->pImgCtx);
		//					plv->pImgCtx = NULL;
		//					TraceMsg(TF_BKIMAGE, "Could not convert URL to wide!");
		//					return FALSE;
		//				}
		//				hr = IImgCtx_Load(plv->pImgCtx, pwszImage, fl);
		//				FreeProducedString(pwszImage);
		//			}
		//#endif
		//			if (FAILED(hr))
		//			{
		//				IImgCtx_Release(plv->pImgCtx);
		//				plv->pImgCtx = NULL;
		//				TraceMsg(TF_BKIMAGE, "Could not init a pImgCtx!");
		//				return FALSE;
		//			}
		//		}
		//		else
		//		{
		//			pbi->ulFlags &= ~LVBKIF_SOURCE_URL;
		//		}
		break;

	default:
		RIPMSG(0, "LVM_SETBKIMAGE: Unsupported image type %d", pbi->ulFlags & LVBKIF_SOURCE_MASK);
		return FALSE;
	}

	plv->ulBkImageFlags = pbi->ulFlags;
	plv->xOffsetPercent = pbi->xOffsetPercent;
	plv->yOffsetPercent = pbi->yOffsetPercent;

	//
	// If we actually created a pImgCtx, initialize it here.
	//
	//L if (plv->pImgCtx)
	//{
	//	if (plv->hpalHalftone == NULL)
	//	{
	//		IImgCtx_GetPalette(plv->pImgCtx, &plv->hpalHalftone);
	//	}

	//	plv->fImgCtxComplete = FALSE;
	//	IImgCtx_SetCallback(plv->pImgCtx, ImgCtxCallback, plv);
	//	IImgCtx_SelectChanges(plv->pImgCtx, IMGCHG_COMPLETE, 0, TRUE);

	//	TraceMsg(TF_BKIMAGE, "  SUCCESS!");
	//	fRet = TRUE;
	//}
	InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);

	return fRet;
}

#ifdef UNICODE
BOOL ListView_OnSetBkImageA(SListViewData *plv, LPLVBKIMAGEA pbiA)
{
	BOOL fProducedString = FALSE;
	BOOL fRet;
	LVBKIMAGEW biW;

	CopyMemory(&biW, pbiA, sizeof(LVBKIMAGE));

	switch (biW.ulFlags & LVBKIF_SOURCE_MASK)
	{
	case LVBKIF_SOURCE_NONE:
	case LVBKIF_SOURCE_HBITMAP:
		break;

	case LVBKIF_SOURCE_URL:
		if (biW.pszImage != NULL)
		{
			biW.pszImage = ProduceWFromA(plv->hwnd->ci.uiCodePage, (LPCSTR)biW.pszImage);
			if (biW.pszImage == (LPARAM)NULL)
			{
				return FALSE;
			}
			fProducedString = TRUE;
		}
		break;

	default:
		// Let ListView_OnSetBkImage() complain about the invalid parameter
		break;
	}

	fRet = ListView_OnSetBkImage(plv, &biW);

	if (fProducedString)
	{
		FreeProducedString((LPVOID)biW.pszImage);
	}

	return fRet;
}
#endif

BOOL ListView_OnGetBkImage(SListViewData *plv, LPLVBKIMAGE pbi)
{
	BOOL fRet = FALSE;

	if (!IsBadWritePtr(pbi, sizeof(*pbi)))
	{
		pbi->ulFlags = plv->ulBkImageFlags;

		switch (plv->ulBkImageFlags & LVBKIF_SOURCE_MASK)
		{
		case LVBKIF_SOURCE_NONE:
			fRet = TRUE;
			break;

		case LVBKIF_SOURCE_HBITMAP:
			pbi->hbm = plv->hbmBkImage;
			fRet = TRUE;
			break;

		case LVBKIF_SOURCE_URL:
			if (!IsBadWritePtr(pbi->pszImage, pbi->cchImageMax * sizeof(TCHAR)))
			{
				lstrcpyn(pbi->pszImage, plv->pszBkImage, pbi->cchImageMax);
				fRet = TRUE;
			}
			break;

		default:
			_ASSERT(0);
			break;
		}

		pbi->xOffsetPercent = plv->xOffsetPercent;
		pbi->yOffsetPercent = plv->yOffsetPercent;
	}

	return fRet;
}

#ifdef UNICODE
BOOL ListView_OnGetBkImageA(SListViewData *plv, LPLVBKIMAGEA pbiA)
{
	BOOL fRet = FALSE;

	if (!IsBadWritePtr(pbiA, sizeof(*pbiA)))
	{
		pbiA->ulFlags = plv->ulBkImageFlags;

		switch (plv->ulBkImageFlags & LVBKIF_SOURCE_MASK)
		{
		case LVBKIF_SOURCE_NONE:
			fRet = TRUE;
			break;

		case LVBKIF_SOURCE_HBITMAP:
			pbiA->hbm = plv->hbmBkImage;
			fRet = TRUE;
			break;

		case LVBKIF_SOURCE_URL:
			if (!IsBadWritePtr(pbiA->pszImage, pbiA->cchImageMax))
			{
				ConvertWToAN(plv->hwnd->ci.uiCodePage, pbiA->pszImage,
					pbiA->cchImageMax, plv->pszBkImage, -1);
				fRet = TRUE;
			}
			break;

		default:
			_ASSERT(0);
			break;
		}

		pbiA->xOffsetPercent = plv->xOffsetPercent;
		pbiA->yOffsetPercent = plv->yOffsetPercent;
	}

	return fRet;
}
#endif

void ListView_FreeSubItem(LISTSUBITEM *plsi)
{
	if (plsi)
	{
		Str_Set(&plsi->pszText, NULL);
		LocalFree(plsi);
	}
}

int ListView_GetCxScrollbar(SListViewData *plv)
{
	int cx;

	if (((plv->exStyle & LVS_EX_FLATSB) == 0) ||
		!FlatSB_GetScrollProp(GetHostWnd(plv->hwnd), WSB_PROP_CXVSCROLL, &cx))
	{
		cx = IUIGetGloabelVar()->g_cxScrollbar;
	}

	return cx;
}

int ListView_GetCyScrollbar(SListViewData *plv)
{
	int cy;

	if (((plv->exStyle & LVS_EX_FLATSB) == 0) ||
		!FlatSB_GetScrollProp(GetHostWnd(plv->hwnd), WSB_PROP_CYHSCROLL, &cy))
	{
		cy = IUIGetGloabelVar()->g_cyScrollbar;
	}

	return cy;
}

DWORD ListView_GetWindowStyle(SListViewData *plv)
{
	DWORD dwStyle;

	if (((plv->exStyle & LVS_EX_FLATSB) == 0) ||
		!FlatSB_GetScrollProp(GetHostWnd(plv->hwnd), WSB_PROP_WINSTYLE, (LPINT)&dwStyle))
	{
		dwStyle = GetStyle(plv->hwnd);
	}

	return dwStyle;
}

int ListView_SetScrollRange(SListViewData *plv, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw)
{
	if (plv->exStyle & LVS_EX_FLATSB)
	{
		// FlatSB_SetScrollRange(plv->hwnd, nBar, nMinPos, nMaxPos, bRedraw);
	}
	else
	{
		SetScrollRange(plv->hwnd, nBar, nMinPos, nMaxPos, bRedraw);
	}

	return 0;
}

int ListView_GetScrollInfo(SListViewData *plv, int nBar, SCROLLINFO *psi)
{
	return GetScrollInfo(plv->hwnd, nBar, psi);
}

int ListView_SetScrollInfo(SListViewData *plv, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw)
{
	int iRc;

	if (plv->exStyle & LVS_EX_FLATSB)
	{
		iRc = FlatSB_SetScrollInfo(GetHostWnd(plv->hwnd), fnBar, lpsi, fRedraw);
	}
	else
	{
		iRc = SetScrollInfo(plv->hwnd, fnBar, lpsi, fRedraw);
	}

	//
	//  You'd think we were finished, but in fact the game is only half over.
	//
	//  Some apps (e.g., Font Folder) will do
	//
	//      SetWindowLong(hwnd, GWL_STYLE, newStyle);
	//
	//  where newStyle toggles the WS_HSCROLL and/or WS_VSCROLL bits.
	//  This causes USER's internal bookkeeping to go completely out
	//  of whack:  The ScrollInfo says that there is a scrollbar, but
	//  the window style says there isn't, or vice versa.  The result
	//  is that we get a scrollbar when we shouldn't or vice versa.
	//
	//  So each time we tweak the scroll info in a manner that changes
	//  the range and page, we kick USER in the head to make sure USER's
	//  view of the world (via style bits) is the same as the scroll
	//  bar's view of the world (via SCROLLINFO).
	//

	//
	//  We should always change SIF_PAGE and SIF_RANGE at the same time.
	//
	_ASSERT((lpsi->fMask & (SIF_PAGE | SIF_RANGE)) == 0 ||
		(lpsi->fMask & (SIF_PAGE | SIF_RANGE)) == (SIF_PAGE | SIF_RANGE));

	if ((lpsi->fMask & (SIF_PAGE | SIF_RANGE)) == (SIF_PAGE | SIF_RANGE))
	{
		BOOL fShow;
		fShow = lpsi->nMax && (int)lpsi->nPage <= lpsi->nMax;

#ifdef DEBUG
		{
			DWORD dwStyle, dwScroll, dwWant;
			dwScroll = (fnBar == SB_VERT) ? WS_VSCROLL : WS_HSCROLL;
			//
			//  We can short-circuit some logic with secret knowledge about how
			//  ListView uses SetScrollInfo.
			//
			_ASSERT(lpsi->nMin == 0);

			dwWant = fShow ? dwScroll : 0;
			dwStyle = ListView_GetWindowStyle(plv);
			if ((dwStyle & dwScroll) != dwWant)
			{
				TraceMsg(TF_LISTVIEW, "ListView_SetScrollInfo: App twiddled WS_[VH]SCROLL");
			}
		}
#endif

		if (plv->exStyle & LVS_EX_FLATSB)
		{
			FlatSB_ShowScrollBar(GetHostWnd(plv->hwnd), fnBar, fShow);
		}
		else
		{
			ShowScrollBar(plv->hwnd, fnBar, fShow);
		}
	}

	return iRc;
}

// Add/remove/replace item

BOOL ListView_FreeItem(SListViewData *plv, LISTITEM *pitem)
{
	_ASSERT(!ListView_IsOwnerData(plv));

	if (pitem)
	{
		Str_Set(&pitem->pszText, NULL);
		if (pitem->hrgnIcon && pitem->hrgnIcon != (HANDLE)(-1))
		{
			DeleteObject(pitem->hrgnIcon);
		}
		// NOTE: We never remove items from the image list; that's
		// the app's responsibility.
		// REVIEW: Should we do this?  Or should we just provide
		// a message that will adjust image indices for the guy
		// when one is removed?
		//
		ControlFree(plv->hheap, pitem);
	}
	return FALSE;
}

LISTITEM *ListView_CreateItem(SListViewData *plv, const LVITEMEX *plvi)
{
	LISTITEM *pitem = (LISTITEM *)ControlAlloc(plv->hheap, sizeof(LISTITEM));

	_ASSERT(!ListView_IsOwnerData(plv));

	if (pitem != NULL)
	{
		if (plvi->mask & LVIF_STATE)
		{
			if (plvi->state & ~LVIS_ALL)
			{
				DebugMsg(DM_ERROR, TEXT("ListView: Invalid state: %04x"), plvi->state);
				return NULL;
			}

			// If adding a selected item to a single-select listview, deselect
			// any other items.
			if ((plv->hwnd->ci.style & LVS_SINGLESEL) && (plvi->state & LVIS_SELECTED))
			{
				ListView_DeselectAll(plv, -1);
			}

			pitem->state  = (plvi->state & ~(LVIS_FOCUSED | LVIS_SELECTED));
		}
		if (plvi->mask & LVIF_PARAM)
		{
			pitem->lParam = plvi->lParam;
		}

		if (plvi->mask & LVIF_IMAGE)
		{
			pitem->iImage = (short) plvi->iImage;
		}

		if (plvi->mask & LVIF_INDENT)
		{
			pitem->iIndent = (short) plvi->iIndent;
		}

		if (plvi->mask & LVIF_BINDWL)
		{
			pitem->hBindWindowless = plvi->hBindWindowless;
		}

		pitem->pt.x = pitem->pt.y = RECOMPUTE;
		ListView_SetSRecompute(pitem);

		pitem->pszText = NULL;
		if (plvi->mask & LVIF_TEXT)
		{
			if (!Str_Set(&pitem->pszText, plvi->pszText))
			{
				ListView_FreeItem(plv, pitem);
				return NULL;
			}
		}
	}
	return pitem;
}

// HACK ALERT!! -- fSmoothScroll is an added parameter!  It allows for smooth
// scrolling when deleting items.  ListView_LRInvalidateBelow is only currently
// called from ListView_OnUpdate and ListView_OnDeleteItem.  Both these calls
// have been modified to work correctly and be backwards compatible.
//
void ListView_LRInvalidateBelow(SListViewData *plv, int i, int fSmoothScroll)
{
	if (ListView_IsListView(plv) || ListView_IsReportView(plv))
	{
		RECT rcItem;

		if (!ListView_RedrawEnabled(plv) ||
			(ListView_IsReportView(plv) && (plv->pImgCtx != NULL)))
		{
			fSmoothScroll = FALSE;
		}

		if (i >= 0)
		{
			ListView_GetRects(plv, i, NULL, NULL, &rcItem, NULL);
		}
		else
		{
			rcItem.left = rcItem.top = 0;
			rcItem.right = plv->sizeClient.cx;
			rcItem.bottom = plv->sizeClient.cy;
		}

		// Don't try to scroll over the header part
		if (ListView_IsReportView(plv) && rcItem.top < plv->yTop)
		{
			rcItem.top = plv->yTop;
		}

		// For both List and report view need to erase the item and
		// below.  Note: do simple test to see if there is anything
		// to redraw

		// we can't check for bottom/right > 0 because if we nuked something
		// above or to the left of the view, it may affect us all
		if ((rcItem.top <= plv->sizeClient.cy) &&
			(rcItem.left <= plv->sizeClient.cx))
		{
			rcItem.bottom = plv->sizeClient.cy;

			if (ListView_RedrawEnabled(plv))
			{
				if ((plv->clrBk == CLR_NONE) && (plv->pImgCtx == NULL))
				{
					LVSeeThruScroll(plv, &rcItem);
				}
				else
				{
					RedrawWindow(GetHostWnd(plv->hwnd), &rcItem, NULL, RDW_INVALIDATE | RDW_ERASE);
				}
			}
			else
			{
				RedrawWindow(GetHostWnd(plv->hwnd), &rcItem, NULL, RDW_INVALIDATE | RDW_ERASE);
			}

			if (ListView_IsListView(plv))
			{
				RECT rcClient;
				// For Listview we need to erase the other columns...
				rcClient.left = rcItem.right;
				rcClient.top = 0;
				rcClient.bottom = plv->sizeClient.cy;
				rcClient.right = plv->sizeClient.cx;
				RedrawWindow(GetHostWnd(plv->hwnd), &rcClient, NULL, RDW_INVALIDATE | RDW_ERASE);
			}
		}
	}
}

// Used in Ownerdata Icon views to try to not invalidate the whole world...
void ListView_IInvalidateBelow(SListViewData *plv, int i)
{
	RECT rcItem;

	if (i >= 0)
	{
		ListView_GetRects(plv, i, NULL, NULL, &rcItem, NULL);
	}
	else
	{
		rcItem.left = rcItem.top = 0;
		rcItem.right = plv->sizeClient.cx;
		rcItem.bottom = plv->sizeClient.cy;
	}

	// For Iconviews we need to invalidate everything to the right of us in this
	// row and everything below the row...
	// below.  Note: do simple test to see if there is anything
	// to redraw

	if ((rcItem.top <= plv->sizeClient.cy) &&
		(rcItem.left <= plv->sizeClient.cx))
	{
		rcItem.right = plv->sizeClient.cx;
		RedrawWindow(GetHostWnd(plv->hwnd), &rcItem, NULL, RDW_INVALIDATE | RDW_ERASE);

		// Now erase everything below...
		rcItem.top = rcItem.bottom;
		rcItem.bottom = plv->sizeClient.cy;
		rcItem.left = 0;
		RedrawWindow(GetHostWnd(plv->hwnd), &rcItem, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
}


void ListView_OnUpdate(SListViewData *plv, int i)
{
	// If in icon/small view, don't call InvalidateItem, since that'll force
	// FindFreeSlot to get called, which is pig-like.  Instead, just
	// force a WM_PAINT message, which we'll catch and call Recompute with.
	//
	if (ListView_IsIconView(plv) || ListView_IsSmallView(plv))
	{
		if (plv->hwnd->ci.style & LVS_AUTOARRANGE)
		{
			ListView_OnArrange(plv, LVA_DEFAULT);
		}
		else
		{
			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INTERNALPAINT | RDW_NOCHILDREN);
		}
	}
	else
	{
		// HACK ALERT!! -- The third parameter is new.  It allows for
		// smooth scrolling when items are deleted in reportview.
		// Passing 0, tells it NOT to scroll.
		//
		ListView_LRInvalidateBelow(plv, i, 0);
	}
	ListView_UpdateScrollBars(plv);
}

#ifdef UNICODE
int ListView_OnInsertItemA(SListViewData *plv, LV_ITEMA *plvi)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	int iRet;

	//HACK ALERT -- this code assumes that LV_ITEMA is exactly the same
	// as LV_ITEMW except for the pointer to the string.
	_ASSERT(sizeof(LV_ITEMA) == sizeof(LV_ITEMW));

	if (!plvi)
	{
		return -1;
	}


	if ((plvi->mask & LVIF_TEXT) && (plvi->pszText != NULL))
	{
		pszC = plvi->pszText;
		pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszC);
		if (pszW == NULL)
		{
			return -1;
		}
		plvi->pszText = (LPSTR)pszW;
	}

	iRet = ListView_OnInsertItem(plv, (const LVITEMEX *) plvi);

	if (pszW != NULL)
	{
		plvi->pszText = pszC;

		FreeProducedString(pszW);
	}

	return iRet;

}
#endif

int ListView_OnInsertItem(SListViewData *plv, const LVITEMEX *plvi)
{
	int iItem;

	if (plvi == NULL || (plvi->iSubItem != 0))    // can only insert the 0th item
	{
		RIPMSG(0, "ListView_InsertItem: iSubItem must be 0 (app passed %d)", plvi->iSubItem);
		return -1;
	}

	// If sorted, then insert sorted.
	//
	if (plv->hwnd->ci.style & (LVS_SORTASCENDING | LVS_SORTDESCENDING)
		&& !ListView_IsOwnerData(plv))
	{
		if (plvi->pszText == LPSTR_TEXTCALLBACK)
		{
			DebugMsg(DM_ERROR, TEXT("Don't use LPSTR_TEXTCALLBACK with LVS_SORTASCENDING or LVS_SORTDESCENDING"));
			return -1;
		}
		iItem = ListView_LookupString(plv, plvi->pszText, LVFI_SUBSTRING | LVFI_NEARESTXY, 0);
	}
	else
	{
		iItem = plvi->iItem;
	}

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	if (!ListView_IsOwnerData(plv))
	{
		int iZ;
		LISTITEM *pitem = ListView_CreateItem(plv, plvi);
		UINT uSelMask = plvi->mask & LVIF_STATE ?
			(plvi->state & (LVIS_FOCUSED | LVIS_SELECTED))
			: 0;
		UINT uSel = uSelMask;

		if (pitem == NULL)
		{
			return -1;
		}

		iItem = IUI_DPA_InsertPtr(plv->hdpa, iItem, pitem);
		if (iItem == -1)
		{
			ListView_FreeItem(plv, pitem);
			return -1;
		}

		plv->cTotalItems++;

		if (plv->hdpaSubItems != NULL)
		{
			int iCol;
			// slide all the colum DPAs down to match the location of the
			// inserted item
			//
			for (iCol = plv->cCol - 1; iCol >= 0; iCol--)
			{
				HDPA hdpa = ListView_GetSubItemDPA(plv, iCol);
				if (hdpa != NULL)       // this is optional, call backs don't have them
				{
					// insert a blank item (REVIEW: should this be callback?)

					// since this can be a tail sparce array,
					// we need to make sure enough items are there.
					if (iItem >= DPA_GetPtrCount(hdpa))
					{
						IUI_DPA_SetPtr(hdpa, iItem, NULL);
					}
					else if (IUI_DPA_InsertPtr(hdpa, iItem, NULL) != iItem)
					{
						goto Failure;
					}
					// Bad assert since hdpa can be tail sparse
					// _ASSERT(ListView_Count(plv) == DPA_GetPtrCount(hdpa));
					_ASSERT(ListView_Count(plv) >= DPA_GetPtrCount(hdpa));
				}
			}
		}

		// Add item to end of z order
		//
		iZ = IUI_DPA_InsertPtr(plv->hdpaZOrder, ListView_Count(plv), (LPVOID)iItem);

		if (iZ == -1)
		{
Failure:
			DebugMsg(TF_LISTVIEW, TEXT("ListView_OnInsertItem() failed"));
			if (IUI_DPA_DeletePtr(plv->hdpa, iItem))
			{
				plv->cTotalItems--;
			}
			ListView_FreeItem(plv, pitem);
			return -1;
		}

		// if we inserted before the focus point, move the focus point up one
		if (iItem <= plv->iFocus)
		{
			plv->iFocus++;
		}
		// do the same thing for the mark
		if (iItem <= plv->iMark)
		{
			plv->iMark++;
		}

		// If the item was not added at the end of the list we need
		// to update the other indexes in the list
		if (iItem != ListView_Count(plv) - 1)
		{
			int i2;
			for (i2 = iZ - 1; i2 >= 0; i2--)
			{
				int iItemZ = (int)(UINT_PTR)DPA_FastGetPtr(plv->hdpaZOrder, i2);
				if (iItemZ >= iItem)
				{
					IUI_DPA_SetPtr(plv->hdpaZOrder, i2, (LPVOID)(UINT_PTR)(iItemZ + 1));
				}
			}
		}

		if (ListView_CheckBoxes(plv))
		{
			uSelMask |= LVIS_STATEIMAGEMASK;
			uSel |= INDEXTOSTATEIMAGEMASK(1);
		}

		if (uSelMask)
		{

			// we masked off these in the createitem above.
			// because turning these on means more than setting the bits.
			ListView_OnSetItemState(plv, iItem, uSel, uSelMask);
		}
	}
	else
	{
		//
		// simply adjust selection and count
		//
		if ((iItem >= 0) && (iItem <= MAX_LISTVIEWITEMS))
		{
			//L if (FAILED(plv->plvrangeSel->lpVtbl->InsertItem(plv->plvrangeSel, iItem)))
			//{
			//	return (-1);
			//}
			plv->cTotalItems++;
			plv->rcView.left = RECOMPUTE;
			ListView_Recompute(plv);
			if (!ListView_IsReportView(plv) && !ListView_IsListView(plv))
			{
				// We need to erase the background so that we don't leave
				// turds from wrapped labels in large icon mode.  This could
				// be optimized by only invalidating to the right of and
				// below the inserted item.
				InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
			}
			// if we inserted before the focus point, move the focus point up
			if (iItem <= plv->iFocus)
			{
				plv->iFocus++;
			}
			// do the same thing for the mark
			if (iItem <= plv->iMark)
			{
				plv->iMark++;
			}
		}

	}

	if (!ListView_IsOwnerData(plv))
	{
		_ASSERT(ListView_Count(plv) == DPA_GetPtrCount(plv->hdpaZOrder));
	}

	if (ListView_RedrawEnabled(plv))
	{
		// Update region
		ListView_RecalcRegion(plv, TRUE, TRUE);

		// The Maybe resize colmns may resize things in which case the next call
		// to Update is not needed.
		if (!ListView_MaybeResizeListColumns(plv, iItem, iItem))
		{
			ListView_OnUpdate(plv, iItem);
		}

		// this trick makes inserting lots of items cheap
		// even if redraw is enabled.... don't calc or position items
		// until this postmessage comes around
		if (!plv->uUnplaced)
		{
			PostMessage(GetHostWnd(plv->hwnd), LVMI_PLACEITEMS, 0, 0);
		}
		plv->uUnplaced++;
	}
	else
	{
		//
		// Special case code to make using SetRedraw work reasonably well
		// for adding items to a listview which is in a non layout mode...
		//
		if ((plv->iFirstChangedNoRedraw == -1) ||
			(iItem < plv->iFirstChangedNoRedraw))
		{
			plv->iFirstChangedNoRedraw = iItem;
		}

	}

	ListView_Notify(plv, iItem, 0, LVN_INSERTITEM);

	MyNotifyWinEvent(EVENT_OBJECT_CREATE, GetHostWnd(plv->hwnd), OBJID_CLIENT, iItem + 1);

	return iItem;
}

BOOL ListView_OnDeleteItem(SListViewData *plv, int iItem)
{
	int iCount = ListView_Count(plv);

	if (!ListView_IsValidItemNumber(plv, iItem))
	{
		return FALSE;    // out of range
	}

	MyNotifyWinEvent(EVENT_OBJECT_DESTROY, GetHostWnd(plv->hwnd), OBJID_CLIENT, iItem + 1);

	ListView_DismissEdit(plv, TRUE);  // cancel edits

	ListView_OnSetItemState(plv, iItem, 0, LVIS_SELECTED);

	if (plv->iFocus == iItem)
	{
		ListView_OnSetItemState(plv, (iItem == iCount - 1 ? iItem - 1 : iItem + 1), LVIS_FOCUSED, LVIS_FOCUSED);
	}

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	if (!ListView_IsOwnerData(plv))
	{
		LISTITEM *pitem;
		int iZ;

		if ((plv->rcView.left != RECOMPUTE) && (ListView_IsIconView(plv) || ListView_IsSmallView(plv)))
		{
			pitem = ListView_FastGetItemPtr(plv, iItem);
			if (LV_IsItemOnViewEdge(plv, pitem))
			{
				plv->rcView.left = RECOMPUTE;
			}

		}

		// We don't need to invalidate the item in report view because we
		// will be scrolling on top of it.
		//
		if (!ListView_IsReportView(plv))
		{
			ListView_InvalidateItem(plv, iItem, FALSE, RDW_INVALIDATE | RDW_ERASE);
		}

		// this notify must be done AFTER the Invalidate because some items need callbacks
		// to calculate the rect, but the notify might free it out
		ListView_Notify(plv, iItem, 0, LVN_DELETEITEM);

		// During the notify, the app might've done something really stupid
		// so revalidate the item number pointer so we don't fault
#ifdef DEBUG
		// Validate internally because DPA_DeletePtr will _ASSERT if you ask it
		// to delete something that doesn't exist.
		if (!ListView_IsValidItemNumber(plv, iItem))
		{
			pitem = NULL;
		}
		else
#endif
			pitem = (LISTITEM *)IUI_DPA_DeletePtr(plv->hdpa, iItem);

		if (!pitem)
		{
			RIPMSG(0, "Something strange happened during LVN_DELETEITEM; abandoning LVM_DELETEITEM");
			return FALSE;
		}

		plv->cTotalItems = DPA_GetPtrCount(plv->hdpa);

		// remove from the z-order, this is a linear search to find this!

		IUI_DPA_DeletePtr(plv->hdpaZOrder, ListView_ZOrderIndex(plv, iItem));

		//
		// As the Z-order hdpa is a set of indexes we also need to decrement
		// all indexes that exceed the one we are deleting.
		//
		for (iZ = ListView_Count(plv) - 1; iZ >= 0; iZ--)
		{
			int iItemZ = (int)(UINT_PTR)DPA_FastGetPtr(plv->hdpaZOrder, iZ);
			if (iItemZ > iItem)
			{
				IUI_DPA_SetPtr(plv->hdpaZOrder, iZ, (LPVOID)(DWORD)(iItemZ - 1));
			}
		}

		// remove from sub item DPAs if necessary

		if (plv->hdpaSubItems)
		{
			int iCol;
			for (iCol = plv->cCol - 1; iCol >= 0; iCol--)
			{
				HDPA hdpa = ListView_GetSubItemDPA(plv, iCol);
				if (hdpa)       // this is optional, call backs don't have them
				{
					LISTSUBITEM *plsi;

					// These DPAs are tail sparse, so don't get upset if we
					// try to delete something that's past the end of the list
#ifdef DEBUG
					plsi = (LISTSUBITEM *)(iItem < DPA_GetPtrCount(hdpa) ? IUI_DPA_DeletePtr(hdpa, iItem) : NULL);
#else
					plsi = (LISTSUBITEM *)IUI_DPA_DeletePtr(hdpa, iItem);
#endif
					ListView_FreeSubItem(plsi);
				}
			}
		}

		ListView_FreeItem(plv, pitem);  // ... finaly the item pointer

	}
	else
	{
		//
		// simply notify and then fixup selection state and count
		//
		if ((iItem >= 0) && (iItem <= MAX_LISTVIEWITEMS))
		{
			ListView_Notify(plv, iItem, 0, LVN_DELETEITEM);

			//L if (FAILED(plv->plvrangeSel->lpVtbl->RemoveItem(plv->plvrangeSel, iItem)))
			//{
			//	// BUGBUG: return out of memory status
			//	//MemoryLowDlg( GetHostWnd(plv->hwnd) );
			//	return FALSE;
			//}
			plv->cTotalItems--;
			plv->rcView.left = RECOMPUTE;
			ListView_Recompute(plv);

			if (!ListView_IsReportView(plv) && !ListView_IsListView(plv))
			{
				// We need to erase the background so that the last item gets
				// erased in both icon modes and so that we don't leave turds
				// from wrapped labels in large icon mode.  This could be
				// optimized by only invalidating to the right of and below
				// the deleted item.
				InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
			}
		}
		else
		{
			return FALSE;
		}
	}

	iCount = ListView_Count(plv);       // regrab count incase someone updated item...

	if (!ListView_IsOwnerData(plv))
	{
		_ASSERT(ListView_Count(plv) == DPA_GetPtrCount(plv->hdpaZOrder));
	}

	if (plv->iFocus == iItem)
	{
		if (plv->iFocus >= iCount)
		{
			plv->iFocus = iCount - 1;
		}
	}
	if (plv->iFocus > iItem)
	{
		plv->iFocus--;          // slide the focus index down
	}

	// same with the mark
	if (plv->iMark == iItem)    // deleted the mark item
	{

		if (plv->iMark >= iCount) // did we nuke the last item?
		{
			plv->iMark = iCount - 1;
		}

	}
	else if (plv->iMark > iItem)
	{
		plv->iMark--;    // slide the mark index down
	}

	// Deleting an icon invalidates the icon positioning cache
	plv->iFreeSlot = -1;

	// HACK ALERT!! -- This construct with ReportView steals code from
	// ListView_OnUpdate.  Currently, it will work exactly the same as before,
	// EXCEPT, that it won't call ListView_OnUpdate.  This is to allow us to
	// send a flag to ListView_LRUpdateBelow to tell it we're scrolling up.
	//
	if (ListView_IsReportView(plv))
	{

		// if the new count is zero and we will be showing empty text, simply invalidate the
		// rect and redraw, else go through the invalidate below code...

		// we don't know if we are going to show empty text if pszEmptyText is NULL, or not
		// because we may get one through notify, so if iCount is 0 invalidate everything
		if (iCount == 0)
		{
			InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
		}
		else
		{
			ListView_LRInvalidateBelow(plv, iItem, 1);
		}



		if (ListView_RedrawEnabled(plv))
		{
			ListView_UpdateScrollBars(plv);
		}
		else
		{
			//
			// Special case code to make using SetRedraw work reasonably well
			// for adding items to a listview which is in a non layout mode...
			//
			if ((plv->iFirstChangedNoRedraw != -1) && (iItem < plv->iFirstChangedNoRedraw))
			{
				plv->iFirstChangedNoRedraw--;
			}
		}
	}
	else
	{
		if (ListView_RedrawEnabled(plv))
		{
			ListView_OnUpdate(plv, iItem);
		}

		else
		{
			ListView_LRInvalidateBelow(plv, iItem, 0);
			//
			// Special case code to make using SetRedraw work reasonably well
			// for adding items to a listview which is in a non layout mode...
			//
			if ((plv->iFirstChangedNoRedraw != -1) && (iItem < plv->iFirstChangedNoRedraw))
			{
				plv->iFirstChangedNoRedraw--;
			}
		}
	}
	ListView_RecalcRegion(plv, TRUE, TRUE);

	return TRUE;
}

BOOL ListView_OnDeleteAllItems(SListViewData *plv)
{
	int i;
	BOOL bAlreadyNotified;
	BOOL fHasItemData;

	fHasItemData = !ListView_IsOwnerData(plv);

	ListView_DismissEdit(plv, TRUE);    // cancel edits

	// Must neutralize the focus because some apps will call
	// ListView_OnGetNextItem(LVNI_FOCUSED) during delete notifications,
	// so we need to make sure the focus is in a safe place.
	// May as well neutralize the mark, too.
	plv->iMark = plv->iFocus = -1;

	// Also nuke the icon positioning cache
	plv->iFreeSlot = -1;

	bAlreadyNotified = (BOOL)ListView_Notify(plv, -1, 0, LVN_DELETEALLITEMS);

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	//
	// 删除所有Items
	//
	if (fHasItemData || !bAlreadyNotified)
	{
		for (i = ListView_Count(plv) - 1; i >= 0; i--)
		{
			if (!bAlreadyNotified)
			{
				ListView_Notify(plv, i, 0, LVN_DELETEITEM);
			}

			if (fHasItemData)
			{
				ListView_FreeItem(plv, ListView_FastGetItemPtr(plv, i));
				//
				//  CAREFUL!  Applications such as NT Backup call back
				//  into ListView during the LVN_DELETEITEM notification,
				//  so we need to kill this item or we will fault at the
				//  next iteration because everybody relies on
				//  ListView_Count for validation.
				//
				DPA_FastDeleteLastPtr(plv->hdpa);
				plv->cTotalItems--;
			}
		}
	}

	if (ListView_IsOwnerData(plv))
	{
		//L if (FAILED(plv->plvrangeSel->lpVtbl->Clear(plv->plvrangeSel)))
		//{
		//	// BUGBUG: return low memory status
		//	//MemoryLowDlg( GetHostWnd(plv->hwnd) );
		//}
		plv->cTotalItems = 0;
	}
	else
	{
		// 移除所有Items，并减小PDA size.
		IUI_DPA_DeleteAllPtrs(plv->hdpa);
		IUI_DPA_DeleteAllPtrs(plv->hdpaZOrder);
		plv->cTotalItems = 0;

		//
		// 删除所有Sub Items
		//
		if (plv->hdpaSubItems)
		{
			int iCol;
			for (iCol = plv->cCol - 1; iCol >= 0; iCol--)
			{
				HDPA hdpa = ListView_GetSubItemDPA(plv, iCol);
				if (hdpa)
				{
					IUI_DPA_EnumCallback(hdpa, (PFNDAENUMCALLBACK)ListView_FreeColumnData, 0);
					IUI_DPA_DeleteAllPtrs(hdpa);
				}
			}
		}
	}

	plv->rcView.left = RECOMPUTE;
	plv->xOrigin = 0;
	plv->nSelected = 0;

	plv->ptlRptOrigin.x = 0;
	plv->ptlRptOrigin.y = 0;

	// reset the cxItem width
	if (!(plv->flags & LVF_COLSIZESET))
	{
		plv->cxItem = 16 * plv->cxLabelChar + plv->cxSmIcon;
	}

	RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	ListView_UpdateScrollBars(plv);

	return TRUE;
}

int PASCAL ListView_IFindNearestItem(SListViewData *plv, int left, int top, UINT vk)
{
	int iMin = -1;

	if (ListView_IsOwnerData(plv))
	{
		POINT pt;
		int cSlots;

		_ASSERT(!ListView_IsReportView(plv) && !ListView_IsListView(plv));

		pt.x = left + plv->ptOrigin.x;
		pt.y = top + plv->ptOrigin.y;

		cSlots = ListView_GetSlotCount(plv, TRUE);
		iMin = ListView_CalcHitSlot(plv, pt, cSlots);

		switch (vk)
		{
		case VK_HOME:
			iMin = 0;
			break;

		case VK_END:
			iMin = ListView_Count(plv) - 1;
			break;

		case VK_LEFT:
			if (iMin % cSlots)
			{
				iMin -= 1;
			}
			break;

		case VK_RIGHT:
			if ((iMin + 1) % cSlots)
			{
				iMin += 1;
			}
			break;

		case VK_UP:
			if (iMin >= cSlots)
			{
				iMin -= cSlots;
			}
			break;

		case VK_DOWN:
			if (iMin + cSlots < ListView_Count(plv))
			{
				iMin += cSlots;
			}
			break;

		default:
			;
		}

		iMin = max(0, iMin);
		iMin = min(ListView_Count(plv) - 1, iMin);

	}
	else
	{
		DWORD dMin = 0;
		int cyItem;
		int yEnd = 0, yLimit = 0, xEnd = 0;
		int iCount;
		int i;

		if (ListView_IsIconView(plv))
		{
			cyItem = plv->cyIcon;
		}
		else
		{
			cyItem = plv->cyItem;
		}

		iCount = ListView_Count(plv);
		if (iCount == 1)
		{
			return 0;
		}

		if (vk == VK_HOME)
		{
			yEnd = yLimit = plv->rcView.bottom;
			xEnd = plv->rcView.right;
		}
		else if (vk == VK_END)
		{

			yEnd = yLimit = plv->rcView.top;
			xEnd = plv->rcView.left;

		}
		for (i = 0; i < iCount; i++)
		{
			RECT rc;
			int dx;
			DWORD dxAbs, dyAbs;
			int dy;
			DWORD dOffset;

			ListView_GetRects(plv, i, &rc, NULL, NULL, NULL);

			dx = rc.left - left;
			dxAbs = (DWORD)(dx < 0 ? -dx : dx);
			dy = rc.top - top;
			dyAbs = (DWORD)(dy < 0 ? -dy : dy);

			if ((vk == VK_LEFT) && (dxAbs < dyAbs || dx >= 0))
			{
				continue;
			}
			else if ((vk == VK_RIGHT) && (dxAbs < dyAbs || dx <= 0))
			{
				continue;
			}
			else if ((vk == VK_UP) && (dxAbs > dyAbs || dy >= 0))
			{
				continue;
			}
			else if ((vk == VK_DOWN) && (dxAbs > dyAbs || dy <= 0))
			{
				continue;
			}

			if (vk == VK_HOME || vk == VK_END)
			{

				// home is not the nearest to the top corner, it's the leftmost of the top row.
				// ditto (reversed) for end.  thus we can't use the stuff below. bummer
				if (vk == VK_HOME)
				{
					if ((rc.top + cyItem < yEnd) ||  // if it's fully above the highest line so far, take it!
						((rc.top < yLimit) &&  // if it's on the same row as the top item to date
							(rc.left < xEnd)))
					{
						iMin = i;
						xEnd = rc.left;
						yEnd = rc.top;
						if (rc.top + cyItem < yLimit)
						{
							yLimit = rc.top + cyItem;
						}
					}
				}
				else
				{
					if ((rc.top > yEnd) || //if it's full below the lowest row
						((rc.top + cyItem > yLimit) && // if it's on the same row
							(rc.right > xEnd)))
					{

						iMin = i;
						xEnd = rc.right;
						yEnd = rc.top;
						if (rc.top > yLimit)
						{
							yLimit = rc.top;
						}
					}
				}

			}
			else
			{

				dOffset = ((dxAbs * dxAbs) + (dyAbs * dyAbs));
				if (iMin == -1 || (dMin > dOffset))
				{
					dMin = dOffset;
					iMin = i;
				}
			}
		}
	}
	return iMin;
}

int ListView_Arrow(SListViewData *plv, int iStart, UINT vk)
{
	RECT rcFocus;
	int i;
	int dx;
	int iCount;

	//
	// The algorithm to find which item depends if we are in a view
	// that is arrange(layout) oriented or a sorted (list) view.
	// For the sorted views we will use some optimizations to make
	// it faster
	//
	iCount = ListView_Count(plv);
	if (ListView_IsReportView(plv) || ListView_IsListView(plv))
	{
		//
		// For up and down arrows, simply increment or decrement the
		// index.  Note: in listview this will cause it to wrap columns
		// which is fine as it is compatible with the file manager
		//
		// Assumes only one of these flags is set...

		switch (vk)
		{
		case VK_LEFT:
			if (ListView_IsReportView(plv))
			{
				ListView_ROnScroll(plv, (GetAsyncKeyState(VK_CONTROL) < 0) ? SB_PAGELEFT : SB_LINELEFT, 0, SB_HORZ);
			}
			else
			{
				iStart -= plv->cItemCol;
			}
			break;

		case VK_RIGHT:
			if (ListView_IsReportView(plv))
			{
				// Make this horizontally scroll the report view
				ListView_ROnScroll(plv, (GetAsyncKeyState(VK_CONTROL) < 0) ? SB_PAGERIGHT : SB_LINERIGHT, 0, SB_HORZ);
			}
			else
			{
				iStart += plv->cItemCol;
			}
			break;

		case VK_UP:
			iStart--;
			break;

		case VK_DOWN:
			iStart++;
			break;

		case VK_HOME:
			iStart = 0;
			break;

		case VK_END:
			iStart = iCount - 1;
			break;

		case VK_NEXT:
			if (ListView_IsReportView(plv))
			{
				i = iStart; // save away to make sure we dont go wrong way!

				// First go to end of page...
				iStart = (int)(((LONG)(plv->sizeClient.cy - (plv->cyItem)
								- plv->yTop) + plv->ptlRptOrigin.y) / plv->cyItem);

				// If Same item, increment by page size.
				if (iStart <= i)
					iStart = i + max(
							(plv->sizeClient.cy - plv->yTop) / plv->cyItem - 1,
							1);

				if (iStart >= iCount)
				{
					iStart = iCount - 1;
				}

			}
			else
			{
				// multiply by 2/3 to give a good feel.. when the item is mostly shown
				// you want to go to the next column
				dx = (plv->sizeClient.cx + (plv->cxItem * 2) / 3) / plv->cxItem;
				if (!dx)
				{
					dx = 1;
				}

				iStart += plv->cItemCol *  dx;
				if (plv->cItemCol)
				{
					while (iStart >= iCount)
					{
						iStart -= plv->cItemCol;
					}
				}
			}
			break;

		case VK_PRIOR:

			if (ListView_IsReportView(plv))
			{
				i = iStart; // save away to make sure we dont go wrong way!

				// First go to end of page...
				iStart = (int)(plv->ptlRptOrigin.y / plv->cyItem);

				// If Same item, increment by page size.
				if (iStart >= i)
					iStart = i - max(
							(plv->sizeClient.cy - plv->yTop) / plv->cyItem - 1,
							1);

				if (iStart < 0)
				{
					iStart = 0;
				}

			}
			else
			{
				dx = (plv->sizeClient.cx + (plv->cxItem * 2) / 3) / plv->cxItem;
				if (!dx)
				{
					dx = 1;
				}
				iStart -= plv->cItemCol * dx;
				if (plv->cItemCol)
				{
					while (iStart < 0)
					{
						iStart += plv->cItemCol;
					}
				}

			}
			break;

		default:
			return -1;      // Out of range
		}

		// Make sure it is in range!.
		if ((iStart >= 0) && (iStart < iCount))
		{
			return iStart;
		}
		else if (iCount == 1)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	else
	{
		//
		// Layout type view. we need to use the position of the items
		// to figure out the next item
		//

		if (ListView_IsOwnerData(plv))
		{
			iStart = max(0, iStart);

			// if it does not matches any of the entries in the case statement below
			// this is done to skip the call back by the GetRects
			//
			if (vk != VK_LEFT  &&
				vk != VK_RIGHT &&
				vk != VK_UP &&
				vk != VK_DOWN &&
				vk != VK_HOME &&
				vk != VK_END &&
				vk != VK_NEXT &&
				vk != VK_PRIOR)
			{
				return -1;
			}
			ListView_GetRects(plv, iStart, &rcFocus, NULL, NULL, NULL);
		}
		else
		{
			if (iStart != -1)
			{
				ListView_GetRects(plv, iStart, &rcFocus, NULL, NULL, NULL);
			}
		}

		switch (vk)
		{
		// For standard arrow keys just fall out of here.
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			if (ListView_IsOwnerData(plv))
			{
				break;
			}
			else
			{
				if (iStart != -1)
				{
					// all keys map to VK_HOME except VK_END
					break;
				}

				// Fall through
				vk = VK_HOME;
			}

		case VK_HOME:
			rcFocus.left = - plv->ptOrigin.x;
			rcFocus.top = - plv->ptOrigin.y;
			break;

		case VK_END:
			rcFocus.left = plv->rcView.right;
			rcFocus.top = plv->rcView.bottom;
			break;

		case VK_NEXT:
			rcFocus.top += plv->sizeClient.cy;
			vk = VK_UP;
			break;

		case VK_PRIOR:
			vk = VK_DOWN;
			rcFocus.top -= plv->sizeClient.cy;
			break;
		default:
			return -1;      // Out of range
		}

		return ListView_IFindNearestItem(plv, rcFocus.left, rcFocus.top, vk);
	}
}

int ListView_OnGetNextItem(SListViewData *plv, int i, UINT flags)
{
	int iStart = i;
	int cItemMax = ListView_Count(plv);

	// Note that -1 is a valid starting point
	if (i < -1 || i >= cItemMax)
	{
		return -1;
	}

	if (ListView_IsOwnerData(plv))
	{
		if (flags & (LVNI_CUT | LVNI_DROPHILITED | LVNI_PREVIOUS))
		{
			return (-1);
		}
	}

	if (flags & LVNI_FOCUSED)
	{
		// we know which item is focused, jump right to it.
		// but we have to mimick the code below exactly for compat:
		//     if directional bits are set, they take precedence.
		if (!(flags & (LVNI_ABOVE | LVNI_BELOW | LVNI_TORIGHT | LVNI_TOLEFT)))
		{
			// there are no more focused items after iFocus
			if (i >= plv->iFocus)
			{
				return -1;
			}

			// subtract one here -- we increment it below
			i = plv->iFocus - 1;
		}
	}

	while (TRUE)
	{
		// BUGBUG: does anyone call this now???
		if (flags & (LVNI_ABOVE | LVNI_BELOW | LVNI_TORIGHT | LVNI_TOLEFT))
		{
			UINT vk;
			if (flags & LVNI_ABOVE)
			{
				vk = VK_UP;
			}
			else if (flags & LVNI_BELOW)
			{
				vk = VK_DOWN;
			}
			else if (flags & LVNI_TORIGHT)
			{
				vk = VK_RIGHT;
			}
			else
			{
				vk = VK_LEFT;
			}

			if (i != -1)
			{
				i = ListView_Arrow(plv, i, vk);
			}
			if (i == -1)
			{
				return i;
			}

		}
		else
		{
			i++;
			if (i == cItemMax)
			{
				return -1;
			}
		}

		// See if any other restrictions are set
		if (flags & ~(LVNI_ABOVE | LVNI_BELOW | LVNI_TORIGHT | LVNI_TOLEFT))
		{
			WORD wItemState;

			if (ListView_IsOwnerData(plv))
			{
				if (flags & LVNI_FOCUSED)
				{
					// we check LVNI_FOCUSED before the loop, so i == iFocus
					_ASSERT(i == plv->iFocus && i != -1);
					if (flags & LVNI_SELECTED)
					{
						//L if (plv->plvrangeSel->lpVtbl->IsSelected(plv->plvrangeSel, i) != S_OK)
						//{
						//	i = -1;
						//}
					}
				}
				else if (flags & LVNI_SELECTED)
				{
					i = max(i, 0);
					//L plv->plvrangeSel->lpVtbl->NextSelected(plv->plvrangeSel, i, &i);
				}
				else
				{
					i = -1;
				}
			}
			else
			{
				{
					LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);
					wItemState = pitem->state;
				}

				// for LVNI_FOCUSED, we start at the LVIS_FOCUSED element, if we're
				// not on that element, one of the below continues was hit, so
				// we'll never find the element. bail out early.
				if ((flags & LVNI_FOCUSED) && !(wItemState & LVIS_FOCUSED))
				{
					_ASSERT(i == plv->iFocus || i == plv->iFocus + 1);
					return (-1);
				}

				if (((flags & LVNI_SELECTED) && !(wItemState & LVIS_SELECTED)) ||
					((flags & LVNI_CUT) && !(wItemState & LVIS_CUT)) ||
					((flags & LVNI_DROPHILITED) && !(wItemState & LVIS_DROPHILITED)))
				{
					if (i != iStart)
					{
						continue;
					}
					else
					{
						// we've looped and we can't find anything to fit this criteria
						return -1;
					}
				}
			}
		}
		return i;
	}
}

int ListView_CompareString(SListViewData *plv, int i, LPCTSTR pszFind, UINT flags, int iLen)
{
	// BUGBUG: non protected globals
	int cb;
	TCHAR ach[CCHLABELMAX];
	LVITEMEX item;

	_ASSERT(!ListView_IsOwnerData(plv));
	_ASSERT(pszFind);

	item.iItem = i;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT;
	item.pszText = ach;
	item.cchTextMax = ARRAYSIZE(ach);
	ListView_OnGetItem(plv, &item);

	if (!(flags & (LVFI_PARTIAL | LVFI_SUBSTRING)))
	{
		return lstrcmpi(item.pszText, pszFind);
	}

	// REVIEW: LVFI_SUBSTRING is not really implemented yet.

	cb = lstrlen(pszFind);
	if (iLen && (cb > iLen))
	{
		cb = iLen;
	}

	//
	// If the sub strings not equal then return the ordering based
	// on the entire string.
	//
#ifndef UNIX
	return IntlStrEqNI(item.pszText, pszFind, cb) ? 0 : lstrcmp(item.pszText, pszFind);
#else
	return IntlStrEqNI(item.pszText, pszFind, cb) ? 0 : lstrcmpi(item.pszText, pszFind);
#endif

}

#ifdef UNICODE
int ListView_OnFindItemA(SListViewData *plv, int iStart, LV_FINDINFOA *plvfi)
{
	LPWSTR pszW = NULL;
	LPCSTR pszC = NULL;
	int iRet;

	//HACK ALERT -- this code assumes that LV_FINDINFOA is exactly the same
	// as LV_FINDINFOW except for the pointer to the string.
	_ASSERT(sizeof(LV_FINDINFOA) == sizeof(LV_FINDINFOW));

	if (!plvfi)
	{
		return -1;
	}

	if (!(plvfi->flags & LVFI_PARAM) && !(plvfi->flags & LVFI_NEARESTXY))
	{
		pszC = plvfi->psz;
		if ((pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszC)) == NULL)
		{
			return -1;
		}
		plvfi->psz = (LPSTR)pszW;
	}

	iRet = ListView_OnFindItem(plv, iStart, (const LV_FINDINFO *)plvfi);

	if (pszW != NULL)
	{
		plvfi->psz = pszC;

		FreeProducedString(pszW);
	}

	return iRet;
}
#endif

int ListView_OnFindItem(SListViewData *plv, int iStart, const LV_FINDINFO *plvfi)
{
	int i;
	int j;
	int cItem;
	UINT flags;

	if (!plvfi)
	{
		return -1;
	}

	if (plvfi->flags & LVFI_NEARESTXY)
	{
		if (ListView_IsIconView(plv) || ListView_IsSmallView(plv))
		{
			return ListView_IFindNearestItem(plv, plvfi->pt.x, plvfi->pt.y, plvfi->vkDirection);
		}
		else
		{
			return -1;
		}
	}

	// Note that -1 is a valid starting point
	if (iStart < -1 || iStart >= ListView_Count(plv))
	{
		return -1;
	}

	if (ListView_IsOwnerData(plv))
	{
		// call back to owner for search
		return ((int) ListView_RequestFindItem(plv, plvfi, iStart + 1));
	}
	else
	{
		flags  = plvfi->flags;
		i = iStart;
		cItem = ListView_Count(plv);
		if (flags & LVFI_PARAM)
		{
			LPARAM lParam = plvfi->lParam;

			// Linear search with wraparound...
			//
			for (j = cItem; j-- != 0;)
			{
				++i;
				if (i == cItem)
				{
					if (flags & LVFI_WRAP)
					{
						i = 0;
					}
					else
					{
						break;
					}
				}

				if (ListView_FastGetItemPtr(plv, i)->lParam == lParam)
				{
					return i;
				}
			}
		}
		else // if (flags & (LVFI_STRING | LVFI_SUBSTRING | LVFI_PARTIAL))
		{
			LPCTSTR pszFind = plvfi->psz;
			if (!pszFind)
			{
				return -1;
			}

			if (plv->hwnd->ci.style & (LVS_SORTASCENDING | LVS_SORTDESCENDING))
			{
				return ListView_LookupString(plv, pszFind, flags, i + 1);
			}

			for (j = cItem; j-- != 0;)
			{
				++i;
				if (i == cItem)
				{
					if (flags & LVFI_WRAP)
					{
						i = 0;
					}
					else
					{
						break;
					}
				}

				if (ListView_CompareString(plv,
						i,
						pszFind,
						(flags & (LVFI_PARTIAL | LVFI_SUBSTRING)), 0) == 0)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

BOOL ListView_OnGetItemRect(SListViewData *plv, int i, RECT *prc)
{
#ifndef UNIX
	LPRECT pRects[LVIR_MAX];
#else
	volatile LPRECT pRects[LVIR_MAX]; /* this is used by the irix compiler */
#endif

	// validate parameters
	if (!ListView_IsValidItemNumber(plv, i))
	{
		RIPMSG(0, "LVM_GETITEMRECT: invalid index %d", i);
		return FALSE;
	}

	if (!prc || prc->left >= LVIR_MAX || prc->left < 0)
	{
		RIPMSG(0, "LVM_GETITEMRECT: invalid rect pointer");
		return FALSE;
	}

	pRects[0] = NULL;
	pRects[1] = NULL;
	pRects[2] = NULL;
	pRects[3] = NULL;

	pRects[prc->left] = prc;
	ListView_GetRects(plv, i, pRects[LVIR_ICON], pRects[LVIR_LABEL],
		pRects[LVIR_BOUNDS], pRects[LVIR_SELECTBOUNDS]);
	return TRUE;
}

//
// in:
//      plv
//      iItem           MUST be a valid item index (in range)
// out:
//   prcIcon            icon bounding rect
//   prcLabel           label text bounding rect, for details this is the first column
//   prcBounds          entire item (all text and icon), including columns in details
//   prcSelectionBounds union of icon and label rects, does NOT include columns
//                      in details view

// BUGBUG raymondc - Need to pass an HDC parameter for measurement
// since sometimes we do this while painting

void ListView_GetRects(SListViewData *plv, int iItem,
	RECT *prcIcon, RECT *prcLabel, RECT *prcBounds,
	RECT *prcSelectBounds)
{
	_ASSERT(plv);

	if (ListView_IsReportView(plv))
	{
		ListView_RGetRects(plv, iItem, prcIcon, prcLabel, prcBounds, prcSelectBounds);
	}
	else if (ListView_IsListView(plv))
	{
		ListView_LGetRects(plv, iItem, prcIcon, prcLabel, prcBounds, prcSelectBounds);
	}
	else
	{
		if (ListView_IsOwnerData(plv))
		{
			RECT rcIcon;
			RECT rcTextBounds;
			LISTITEM item;

			if (ListView_IsIconView(plv))
			{
				ListView_IGetRectsOwnerData(plv, iItem, &rcIcon, &rcTextBounds, &item, FALSE);
			}
			else if (ListView_IsSmallView(plv))
			{
				ListView_SGetRectsOwnerData(plv, iItem, &rcIcon, &rcTextBounds, &item, FALSE);
			}
			if (prcIcon)
			{
				*prcIcon = rcIcon;
			}
			if (prcLabel)
			{
				*prcLabel = rcTextBounds;
			}

			if (prcBounds)
			{
				UnionRect(prcBounds, &rcIcon, &rcTextBounds);
			}

			if (prcSelectBounds)
			{
				UnionRect(prcSelectBounds, &rcIcon, &rcTextBounds);
			}
		}
		else
		{
			if (iItem >= ListView_Count(plv))
			{
				return;
			}
			else
			{
				LISTITEM *pitem = ListView_FastGetItemPtr(plv, iItem);

				if (pitem->cyFoldedLabel == SRECOMPUTE)
				{
					ListView_RecomputeLabelSize(plv, pitem, iItem, NULL, FALSE);
				}
				_ListView_GetRectsFromItem(plv, ListView_IsSmallView(plv), pitem,
					prcIcon, prcLabel, prcBounds, prcSelectBounds);
			}
		}
	}
}

void ListView_GetRectsOwnerData(SListViewData *plv, int iItem,
	RECT *prcIcon, RECT *prcLabel, RECT *prcBounds,
	RECT *prcSelectBounds, LISTITEM *pitem)
{
	_ASSERT(plv);
	_ASSERT(ListView_IsOwnerData(plv));

	if (ListView_IsReportView(plv))
	{
		ListView_RGetRects(plv, iItem, prcIcon, prcLabel, prcBounds,
			prcSelectBounds);
	}
	else if (ListView_IsListView(plv))
	{
		ListView_LGetRects(plv, iItem, prcIcon, prcLabel, prcBounds,
			prcSelectBounds);
	}
	else
	{
		RECT rcIcon;
		RECT rcTextBounds;

		if (ListView_IsIconView(plv))
		{
			ListView_IGetRectsOwnerData(plv, iItem, &rcIcon, &rcTextBounds, pitem, TRUE);
		}
		else if (ListView_IsSmallView(plv))
		{
			ListView_SGetRectsOwnerData(plv, iItem, &rcIcon, &rcTextBounds, pitem, TRUE);
		}

		// Don't need to check for folding here, as will have been handled in user data
		// rectangle fetching functions.

		if (prcIcon)
		{
			*prcIcon = rcIcon;
		}
		if (prcLabel)
		{
			*prcLabel = rcTextBounds;
		}

		if (prcBounds)
		{
			UnionRect(prcBounds, &rcIcon, &rcTextBounds);
		}

		if (prcSelectBounds)
		{
			UnionRect(prcSelectBounds, &rcIcon, &rcTextBounds);
		}
	}
}


BOOL ListView_OnRedrawItems(SListViewData *plv, int iFirst, int iLast)
{
	int iCount = ListView_Count(plv);

	if (iFirst < iCount)
	{

		if (iLast >= iCount)
		{
			iLast = iCount - 1;
		}

		while (iFirst <= iLast)
		{
			ListView_InvalidateItem(plv, iFirst++, FALSE, RDW_INVALIDATE | RDW_ERASE);
		}
	}
	return TRUE;
}

// fSelectionOnly       use the selection bounds only, ie. don't include
//                      columns in invalidation if in details view
//
// TODO: 如果是UpdateLayeredWindow，要在所有Item更新后整体一次刷新。
void ListView_InvalidateItemEx(SListViewData *plv, int iItem, int iSubItem, BOOL fSelectionOnly,
	UINT fRedraw, UINT maskChanged)
{
	RECT rc;
	LPRECT prcIcon;
	LPRECT prcLabel;
	LPRECT prcBounds;
	LPRECT prcSelectBounds;

	if (iItem == -1)
	{
		return;
	}

	prcIcon = prcLabel = prcBounds = prcSelectBounds = NULL;

	// if we're in owner draw mode, and there's been a new font,
	// we don't really know what the selection bounds is, so always use the bounds
	// in that case... unless we're in fullrowselect mode
	if (ListView_IsOwnerData(plv) && plv->flags & LVF_CUSTOMFONT &&
		!ListView_FullRowSelect(plv))
	{
		fSelectionOnly = FALSE;
	}

	// if we're owner draw, there's no such thing as selection only
	if (plv->hwnd->ci.style & LVS_OWNERDRAWFIXED)
	{
		fSelectionOnly = FALSE;
	}

	if (fSelectionOnly)
	{
		// In report mode non-fullrowselect,
		// we have to use the full label rectangle rather
		// than just the selection bounds, since the stuff outside the
		// rectangle might need redrawing, too.

		if (ListView_IsReportView(plv) && !ListView_FullRowSelect(plv))
		{
			prcLabel = &rc;
		}
		else
		{
			prcSelectBounds = &rc;
		}
	}
	else
	{

		// if _only_the_text_ or _only_the_image_ changed then limit the redraw
		switch (maskChanged)
		{

		case LVIF_IMAGE:
			prcIcon = &rc;
			break;

		case LVIF_TEXT:
			prcLabel = &rc;
			break;

		default:
			prcBounds = &rc;
			break;
		}
	}

	if (ListView_RedrawEnabled(plv))
	{

		ListView_GetRects(plv, iItem,
			prcIcon, prcLabel, prcBounds, prcSelectBounds);

		if (prcBounds != NULL)
		{
			if (plv->m_bEnableSelectedFitClient)
			{
				prcBounds->right = max(prcBounds->right, plv->sizeClient.cx);
			}
		}

		if (RECTS_IN_SIZE(plv->sizeClient, rc))
		{
			if (plv->exStyle & LVS_EX_BORDERSELECT)
			{
				InflateRect(&rc, 4 + plv->g_cxIconMargin, 4 + plv->g_cyIconMargin);    // account for selection border and seperation since drawing otside of icon
			}

			IUITRACE(_T("Invalidate Rect : %d %d %d %d[%d * %d]"),
				rc.left, rc.top, rc.right, rc.bottom, RECTWIDTH(rc), RECTHEIGHT(rc));

			InvalidateRect(plv->hwnd, &rc);
			//L RedrawWindow(GetHostWnd(plv->hwnd), &rc, NULL, fRedraw);
		}

	}
	else
	{

		// if we're not visible, we'll get a full
		// erase bk when we do become visible, so only do this stuff when
		// we're on setredraw false
		if (!(plv->flags & LVF_REDRAW))
		{

			// if we're invalidating that's new (thus hasn't been painted yet)
			// blow it off
			if ((plv->iFirstChangedNoRedraw != -1) &&
				(iItem >= plv->iFirstChangedNoRedraw))
			{
				return;
			}

			ListView_GetRects(plv, iItem,
				prcIcon, prcLabel, prcBounds, prcSelectBounds);

			// if it had the erase bit, add it to our region
			if (RECTS_IN_SIZE(plv->sizeClient, rc))
			{

				HRGN hrgn = CreateRectRgnIndirect(&rc);

				ListView_InvalidateRegion(plv, hrgn);

				if (fRedraw & RDW_ERASE)
				{
					plv->flags |= LVF_ERASE;
				}
			}
		}
	}
}

// this returns BF_* flags to indicate which if any edge the item I is touching
// or crossing...
UINT LV_IsItemOnViewEdge(SListViewData *plv, LISTITEM *pitem)
{
	RECT rcItem;
	RECT rcView = plv->rcView;
	UINT uRet = 0;

	// the view rect is enlarged a bit to allow for a little space around
	// the text (see ListView_Recompute())
	rcView.bottom -= GetBorderRect(plv->hwnd).bottom;
	rcView.right -= GetBorderRect(plv->hwnd).left;

	_ListView_GetRectsFromItem(plv, ListView_IsSmallView(plv), pitem,
		NULL, NULL, &rcItem, NULL);
	// translate from window coordinates to listview coordinate
	OffsetRect(&rcItem, plv->ptOrigin.x, plv->ptOrigin.y);

	if (rcItem.right >= rcView.right)
	{
		uRet |= BF_RIGHT;
	}

	if (rcItem.left <= rcView.left)
	{
		uRet |= BF_LEFT;
	}

	if (rcItem.top <= rcView.top)
	{
		uRet |= BF_TOP;
	}

	if (rcItem.bottom >= rcView.bottom)
	{
		uRet |= BF_BOTTOM;
	}

	return uRet;
}

// 设置Icon View Item图标的绝对位置。 在绘制时，Item图标、文本都根据这个位置来计算
// x: Item图标左上角水平坐标
// y: Item图标左上角垂直坐标
void LV_AdjustViewRectOnMove(SListViewData *plv, LISTITEM *pitem, int x, int y)
{
	plv->iFreeSlot = -1; // The "free slot" cache is no good once an item moves

	// if we have to recompute anyways, don't bother
	if (!ListView_IsOwnerData(plv))
	{
		if ((plv->rcView.left != RECOMPUTE) &&
			x != RECOMPUTE && y != RECOMPUTE &&
			pitem->cyFoldedLabel != SRECOMPUTE)
		{
			RECT rcAfter;
			RECT rcView = plv->rcView;

			// the view rect is enlarged a bit to allow for a little space around
			// the text (see ListView_Recompute())
			rcView.bottom -= GetBorderRect(plv->hwnd).bottom;
			rcView.right -= GetBorderRect(plv->hwnd).left;

			if (pitem->pt.x != RECOMPUTE)
			{
				UINT uEdges;

				uEdges = LV_IsItemOnViewEdge(plv, pitem);

				pitem->pt.x = x;
				pitem->pt.y = y;

				// before and after the move, they need to be touching the
				// same edges or not at all
				if (uEdges != LV_IsItemOnViewEdge(plv, pitem))
				{
					goto FullRecompute;
				}
			}
			else
			{
				// if the position wasn't set before
				// we just need to find out what it is afterwards and
				// enlarge the view... we don't need to shrink it
				pitem->pt.x = x;
				pitem->pt.y = y;


			}

			_ListView_GetRectsFromItem(plv, ListView_IsSmallView(plv), pitem,
				NULL, NULL, &rcAfter, NULL);
			// translate from window coordinates to listview coordinate
			OffsetRect(&rcAfter, plv->ptOrigin.x, plv->ptOrigin.y);

			// if we make it here, we just have to make sure the new view rect
			// encompases this new item
			UnionRect(&rcView, &rcView, &rcAfter);
			rcView.right += GetBorderRect(plv->hwnd).left;
			rcView.bottom += GetBorderRect(plv->hwnd).bottom;

			DebugMsg(TF_LISTVIEW, TEXT("Score! (%d %d %d %d) was (%d %d %d %d)"),
				rcView.left, rcView.top, rcView.right, rcView.bottom,
				plv->rcView.left, plv->rcView.top, plv->rcView.right, plv->rcView.bottom);

			plv->rcView = rcView;

		}
		else
		{
FullRecompute:
			plv->rcView.left = RECOMPUTE;
		}
	}

	DebugMsg(TF_LISTVIEW, TEXT("LV -- AdjustViewRect pitem %d -- (%x, %x)"),
		pitem,
		pitem->pt.x, pitem->pt.y);

	pitem->pt.x = x;
	pitem->pt.y = y;

	// Compute the workarea of this item if applicable
	ListView_FindWorkArea(plv, pitem->pt, &(pitem->iWorkArea));
}

BOOL ListView_OnSetItemPosition(SListViewData *plv, int i, int x, int y)
{
	LISTITEM *pitem;

	if (ListView_IsListView(plv))
	{
		return FALSE;
	}

	if (ListView_IsOwnerData(plv))
	{
		RIPMSG(0, "LVM_SETITEMPOSITION: Invalid for owner-data listview");
		return FALSE;
	}

	pitem = ListView_GetItemPtr(plv, i);
	if (!pitem)
	{
		return FALSE;
	}

	//
	// this is a hack to fix a bug in OLE drag/drop loop
	//
	if (x >= 0xF000 && x < 0x10000)
	{
		DebugMsg(TF_LISTVIEW, TEXT("LV -- On SetItemPosition fixing truncated negative number 0x%08X"), x);
		x = x - 0x10000;
	}

	if (y >= 0xF000 && y < 0x10000)
	{
		DebugMsg(TF_LISTVIEW, TEXT("LV -- On SetItemPosition fixing truncated negative number 0x%08X"), y);
		y = y - 0x10000;
	}

	ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);

	if (pitem->cyFoldedLabel == SRECOMPUTE)
	{
		ListView_RecomputeLabelSize(plv, pitem, i, NULL, FALSE);
	}

	// erase old

	if (y != pitem->pt.y || x != pitem->pt.x)
	{
		// Don't invalidate if it hasn't got a position yet
		if (pitem->pt.y != RECOMPUTE)
		{
			ListView_InvalidateItem(plv, i, FALSE, RDW_INVALIDATE | RDW_ERASE);
		}
		else if (plv->uUnplaced)
		{
			// this means an unplaced item got placed
			plv->uUnplaced--;
			if (!plv->uUnplaced)
			{
				MSG msg;
				// if this is now 0, pull out the postmessage
				PeekMessage(&msg, GetHostWnd(plv->hwnd), LVMI_PLACEITEMS, LVMI_PLACEITEMS, PM_REMOVE);
			}
		}

		if (y == RECOMPUTE)
		{
			// if they're setting the new position to be a "any open spot" post that we
			// need to calc this later
			if (!plv->uUnplaced)
			{
				PostMessage(GetHostWnd(plv->hwnd), LVMI_PLACEITEMS, 0, 0);
			}
			plv->uUnplaced++;
		}
	}

	DebugMsg(TF_LISTVIEW, TEXT("LV -- On SetItemPosition %d %d %d %d -- (%x, %x)"),
		plv->rcView.left, plv->rcView.top, plv->rcView.right, plv->rcView.bottom,
		pitem->pt.x, pitem->pt.y);


	LV_AdjustViewRectOnMove(plv, pitem, x, y);

	// and draw at new position
	ListView_RecalcRegion(plv, FALSE, TRUE);
	ListView_InvalidateItem(plv, i, FALSE, RDW_INVALIDATE);

	// If autoarrange is turned on, do it now...
	if (ListView_RedrawEnabled(plv))
	{
		if (plv->hwnd->ci.style & LVS_AUTOARRANGE)
		{
			ListView_OnArrange(plv, LVA_DEFAULT);
		}
		else
		{
			ListView_UpdateScrollBars(plv);
		}
	}

	return TRUE;
}

BOOL ListView_OnGetItemPosition(SListViewData *plv, int i, POINT *ppt)
{
	LISTITEM *pitem;

	//
	// This needs to handle all views as it is used to figure out
	// where the item is during drag and drop and the like
	//
	if (!ppt)
	{
		RIPMSG(0, "LVM_GETITEMPOSITION: Invalid ppt = NULL");
		return FALSE;
	}

	if (ListView_IsListView(plv) || ListView_IsReportView(plv)
		|| ListView_IsOwnerData(plv))
	{
		RECT rcIcon;
		ListView_GetRects(plv, i, &rcIcon, NULL, NULL, NULL);
		ppt->x = rcIcon.left;
		ppt->y = rcIcon.top;

	}
	else
	{

		pitem = ListView_GetItemPtr(plv, i);
		if (!pitem)
		{
			return FALSE;
		}

		if (pitem->pt.x == RECOMPUTE)
		{
			ListView_Recompute(plv);
		}

		ppt->x = pitem->pt.x;
		ppt->y = pitem->pt.y;
	}
	return TRUE;
}




BOOL ListView_OnGetOrigin(SListViewData *plv, POINT *ppt)
{
	if (!ppt)
	{
		DebugMsg(DM_ERROR, TEXT("ListView_OnGetOrigin: ppt is NULL"));
		return FALSE;
	}

	if (ListView_IsListView(plv) || ListView_IsReportView(plv))
	{
		return FALSE;
	}

	*ppt = plv->ptOrigin;
	return TRUE;
}



#ifdef UNICODE
int ListView_OnGetStringWidthA(SListViewData *plv, LPCSTR psz, HDC hdc)
{
	LPWSTR pszW = NULL;
	int iRet;

	if (!psz)
	{
		return 0;
	}

	if ((psz != NULL) && (pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, psz)) == NULL)
	{
		return 0;
	}

	iRet = ListView_OnGetStringWidth(plv, pszW, hdc);

	FreeProducedString(pszW);

	return iRet;
}
#endif

int ListView_OnGetStringWidth(SListViewData *plv, LPCTSTR psz, HDC hdc)
{
	SIZE siz;
	HDC hdcFree = NULL;
	HFONT hfontPrev = NULL;

	if (!psz || psz == LPSTR_TEXTCALLBACK)
	{
		return 0;
	}

	if (!hdc)
	{
		hdcFree = hdc = GetDC(GetHostWnd(plv->hwnd));
		hfontPrev = SelectFont(hdc, plv->hfontLabel);
	}

	GetTextExtentPoint(hdc, psz, lstrlen(psz), &siz);

	if (hdcFree)
	{
		SelectFont(hdc, hfontPrev);
		ReleaseDC(GetHostWnd(plv->hwnd), hdcFree);
	}

	return siz.cx;
}

int ListView_OnGetColumnWidth(SListViewData *plv, int iCol)
{
	if (ListView_IsReportView(plv))
	{
		return ListView_RGetColumnWidth(plv, iCol);
	}
	else if (ListView_IsListView(plv))
	{
		return plv->cxItem;
	}

	return 0;
}

BOOL ListView_ISetColumnWidth(SListViewData *plv, int iCol, int cx, BOOL fExplicit)
{

	if (ListView_IsListView(plv))
	{
		if (iCol != 0 || cx <= 0)
		{
			return FALSE;
		}

		// if it's different and this is an explicit set, or we've never set it explicitly
		if (plv->cxItem != cx && (fExplicit || !(plv->flags & LVF_COLSIZESET)))
		{
			// REVIEW: Should optimize what gets invalidated here...

			plv->cxItem = cx;
			if (fExplicit)
			{
				plv->flags |= LVF_COLSIZESET;    // Set the fact that we explictly set size!.
			}

			if (ListView_IsLabelTip(plv))
			{
				// A truncated label may have been exposed or vice versa.
				ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);
			}

			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
			ListView_UpdateScrollBars(plv);
		}
		return TRUE;
	}
	else if (ListView_IsReportView(plv))
	{
		if (ListView_IsLabelTip(plv))
		{
			// A truncated label may have been exposed or vice versa.
			ListView_InvalidateTTLastHit(plv, plv->iTTLastHit);
		}
		return ListView_RSetColumnWidth(plv, iCol, cx);
	}
	else
	{
		if (cx && plv->cxItem != cx && (fExplicit || !(plv->flags & LVF_COLSIZESET)))
		{
			// REVIEW: Should optimize what gets invalidated here...
			plv->cxItem = cx;
			if (fExplicit)
			{
				plv->flags |= LVF_COLSIZESET;    // Set the fact that we explictly set size!.
			}

			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
			ListView_UpdateScrollBars(plv);
		}
		// BUG-FOR-BUG COMPATIBILITY:  IE4 accidentally returned FALSE here.
	}
	return FALSE;
}

void ListView_Redraw(SListViewData *plv, HDC hdc, RECT *prcClip)
{
	int i;
	int cItem = ListView_Count(plv);
	DWORD dwType = plv->hwnd->ci.style & LVS_TYPEMASK;
	NMCUSTOMDRAW nmcd;
	LVDRAWITEM lvdi;

	SetBkMode(hdc, TRANSPARENT);
	SelectFont(hdc, plv->hfontLabel);

	nmcd.hdc = hdc;

	nmcd.uItemState = 0;
	nmcd.lItemlParam = 0;
	nmcd.rc = *prcClip;
	plv->hwnd->ci.dwCustom = CICustomDrawNotify(&plv->hwnd->ci, CDDS_PREPAINT, &nmcd);
	if (!(plv->hwnd->ci.dwCustom & CDRF_SKIPDEFAULT))
	{
		// Just before doing any painting, see if the region is up to date...
		ListView_RecalcRegion(plv, FALSE, TRUE);

		//
		// For list view and report view, we can save a lot of time
		// by calculating the index of the first item that may need
		// painting...
		//

		switch (dwType)
		{
		case LVS_REPORT:
			i = ListView_RYHitTest(plv, prcClip->top);
			cItem = ListView_RYHitTest(plv, prcClip->bottom) + 1;
			break;

		case LVS_LIST:
			i = ListView_LCalcViewItem(plv, prcClip->left, prcClip->top);
			cItem = ListView_LCalcViewItem(plv, prcClip->right, prcClip->bottom) + 1;
			break;

		default:
			if (ListView_IsOwnerData(plv))
			{
				ListView_CalcMinMaxIndex(plv, prcClip, &i, &cItem);
				break;
			}
			else
			{
				// REVIEW: we can keep a flag which tracks whether the view is
				// presently in pre-arranged order and bypass Zorder when it is
				i = 0;  // Icon views no such hint
			}
		}

		if (i < 0)
		{
			i = 0;
		}

		cItem = min(ListView_Count(plv), cItem);
		if (ListView_IsOwnerData(plv) && (cItem > i))
		{
			ListView_NotifyCacheHint(plv, i, cItem - 1);
			ListView_LazyCreateWinEvents(plv, i, cItem - 1);
		}

		lvdi.plv = plv;
		lvdi.nmcd.nmcd.hdc = hdc;
		lvdi.prcClip = prcClip;
		lvdi.pitem = NULL;

		for (; i < cItem; i++)
		{
			BOOL bSuccess;
			int i2;

			if ((dwType == LVS_ICON || dwType == LVS_SMALLICON)
				&& (!ListView_IsOwnerData(plv)))
			{
				LISTITEM *pitem;

				// Icon views: Draw back-to-front mapped through
				// Z-order array for proper Z order appearance - If autoarrange
				// is on, we don't need to do this as our arrange code is setup
				// to not overlap items!
				//
				// For the cases where we might have overlap, we sped this up,
				// by converting the hdpaZorder into a list of indexes instead
				// of pointers.  This ovoids the costly convert pointer to
				// index call.
				//
				i2 = (int)(UINT_PTR)DPA_FastGetPtr(plv->hdpaZOrder, (cItem - 1) - i);

				//
				// do a fast clip check on the item so we dont even try to
				// draw it unless it is visible
				//
				// for small icon view we cant clip on the left without
				// getting the text
				//
				// for large icon view we cant clip on the top without
				// getting the text
				//
				// for large icon view in NOLABELWRAP mode, we can't clip
				// on the top without getting the text, nor can we clip to
				// the left or right in case the text is long.
				//
				// we can always clip to the bottom
				//
				pitem = ListView_FastGetItemPtr(plv, i2);

				if (pitem->pt.x != RECOMPUTE)
				{
					if (pitem->pt.y - plv->ptOrigin.y > prcClip->bottom)
					{
						continue;
					}

					if (dwType == LVS_SMALLICON)
					{
						if (pitem->pt.x - plv->ptOrigin.x - plv->cxState > prcClip->right)
						{
							continue;
						}

						if (pitem->pt.y + plv->cyItem - plv->ptOrigin.y < prcClip->top)
						{
							continue;
						}
					}
					else if (!(plv->hwnd->ci.style & LVS_NOLABELWRAP))
					{
						if (pitem->pt.x - plv->cxIconSpacing - plv->ptOrigin.x > prcClip->right)
						{
							continue;
						}

						if (pitem->pt.x + plv->cxIconSpacing - plv->ptOrigin.x < prcClip->left)
						{
							continue;
						}
					}
				}
			}
			else
			{
				i2 = i;
			}

			plv->iItemDrawing = i2;

			lvdi.nmcd.nmcd.dwItemSpec = i2;

			// these may get changed
			lvdi.lpptOrg = NULL;
			lvdi.flags = 0;
			lvdi.nmcd.clrText = plv->clrText;
			lvdi.nmcd.clrTextBk = plv->clrTextBk;

			bSuccess = ListView_DrawItem(&lvdi);

			if (!bSuccess)
			{
				break;
			}
		}

		if ((dwType == LVS_ICON || dwType == LVS_SMALLICON)
			&& (ListView_IsOwnerData(plv)) &&
			plv->iFocus != -1)
		{
			// since there's no zorder in ownerdata, we explicitly draw the focus guy last (again)
			// so that it'll appear on top
			// we may potentially want to do this for all items that are selected
			plv->iItemDrawing = plv->iFocus;

			lvdi.nmcd.nmcd.dwItemSpec = plv->iItemDrawing;

			// these may get changed
			lvdi.lpptOrg = NULL;
			lvdi.flags = 0;
			lvdi.nmcd.clrText = plv->clrText;
			lvdi.nmcd.clrTextBk = plv->clrTextBk;

			ListView_DrawItem(&lvdi);
		}



		// this is an NT5/Memphis feature.

		if (ListView_Count(plv) == 0)
		{
			// there're no items in this view
			// check if we need to display some text in this case.

			if (_ListView_GetEmptyText(plv))
			{
				RECT rcClip;
				UINT flags = 0;

				// Put some edging between the text and the border of the
				// window so we don't slam up against the border.
				// This keeps DBCS from looking horrid.
				rcClip.left = GetBorderRect(plv->hwnd).left;
				rcClip.top = GetBorderRect(plv->hwnd).bottom;

#ifdef WINDOWS_ME
				if (plv->dwExStyle & WS_EX_RTLREADING)
				{
					flags |= SHDT_RTLREADING;
				}
#endif
				// if its a report view && we have a header then move the text down
				if (ListView_IsReportView(plv) && (!(plv->hwnd->ci.style & LVS_NOCOLUMNHEADER)))
				{
					rcClip.top += plv->cyItem;
				}

				// Note: Use the full sizeClient.cx as the right margin
				// in case pszEmptyText is wider than the client rectangle.

				rcClip.left -= (int)plv->ptlRptOrigin.x;
				rcClip.right = plv->sizeClient.cx;
				rcClip.bottom = rcClip.top + plv->cyItem;

				SHDrawText(hdc, plv->pszEmptyText,
					&rcClip, LVCFMT_LEFT, flags,
					plv->cyLabelChar, plv->cxEllipses,
					plv->clrText, plv->clrBk);
			}
		}

		plv->iItemDrawing = -1;

		// post painting.... this is to do any extra (non item) painting
		// such a grid lines
		switch (dwType)
		{
		case LVS_REPORT:
		case LVS_ICON:
			ListView_RAfterRedraw(plv, hdc);
			break;
		}

		// notify parent afterwards if they want us to
		if (plv->hwnd->ci.dwCustom & CDRF_NOTIFYPOSTPAINT)
		{
			CICustomDrawNotify(&plv->hwnd->ci, CDDS_POSTPAINT, &nmcd);
		}
	}
}

BOOL ListView_DrawItem(PLVDRAWITEM plvdi)
{
	BOOL bRet = TRUE;
	UINT state;

	if (!ListView_IsOwnerData(plvdi->plv))
	{
		plvdi->pitem = ListView_FastGetItemPtr(plvdi->plv, plvdi->nmcd.nmcd.dwItemSpec);
	}

	// notify on custom draw then do it!
	plvdi->nmcd.nmcd.uItemState = 0;
	plvdi->nmcd.nmcd.lItemlParam = (plvdi->pitem) ? plvdi->pitem->lParam : 0;

	if (!(plvdi->flags & LVDI_NOWAYFOCUS))
	{
		if (plvdi->plv->flags & LVF_FOCUSED)
		{

			// if we're ownerdraw or asked to callback, go
			// fetch the state
			if (!plvdi->pitem || (plvdi->plv->stateCallbackMask & (LVIS_SELECTED | LVIS_FOCUSED)))
			{

				state = (WORD) ListView_OnGetItemState(plvdi->plv, (int) plvdi->nmcd.nmcd.dwItemSpec,
						LVIS_SELECTED | LVIS_FOCUSED);
			}
			else
			{
				state = plvdi->pitem->state;
			}


			if (state & LVIS_FOCUSED)
			{
				plvdi->nmcd.nmcd.uItemState |= CDIS_FOCUS;
			}

			if (state & LVIS_SELECTED)
			{
				plvdi->nmcd.nmcd.uItemState |= CDIS_SELECTED;
			}
		}

		// NOTE:  This is a bug.  We should set CDIS_SELECTED only if the item
		// really is selected.  But this bug has existed forever so who knows
		// what apps are relying on it.  Standard workaround is for the client
		// to do a GetItemState and reconfirm the LVIS_SELECTED flag.
		// That's what we do in ListView_DrawImageEx.
		if (plvdi->plv->hwnd->ci.style & LVS_SHOWSELALWAYS)
		{
			plvdi->nmcd.nmcd.uItemState |= CDIS_SELECTED;
		}
	}

#ifdef KEYBOARDCUES
	if (!(CCGetUIState(&(plvdi->plv->hwnd->ci)) & UISF_HIDEFOCUS))
	{
		plvdi->nmcd.nmcd.uItemState |= CDIS_SHOWKEYBOARDCUES;
	}
#endif
	plvdi->nmcd.clrText = plvdi->plv->clrText;
	plvdi->nmcd.clrTextBk = (plvdi->plv->hwnd->ci.style & WS_DISABLED ? plvdi->plv->clrBk : plvdi->plv->clrTextBk);


	// PERF: if we decide to keep LVIS_EX_TWOCLICKACTIVATE, then we can
	// call ListView_OnGetItem for LVIF_TEXT | LVIF_IMAGE | LVIF_STATE
	// and pass the intem info into _ListView_DrawItem below.
	if (plvdi->plv->iHot == (int)plvdi->nmcd.nmcd.dwItemSpec)
	{
		// Handle the HOT case
		if (plvdi->plv->clrHotlight != CLR_DEFAULT)
		{
			plvdi->nmcd.clrText = plvdi->plv->clrHotlight;
		}
		else
		{
			plvdi->nmcd.clrText = GetSysColor(COLOR_HOTLIGHT);
		}
		// ie4 bug 47635: if hotlight color is the same as the background
		// color you don't see the text -- slam to a visible color in this case.
		if (plvdi->nmcd.clrText == plvdi->nmcd.clrTextBk)
		{
			if (COLORISLIGHT(plvdi->nmcd.clrTextBk))
			{
				plvdi->nmcd.clrText = 0x000000;    // black
			}
			else
			{
				plvdi->nmcd.clrText = 0xFFFFFF;    // white
			}
		}
		if ((plvdi->plv->exStyle & LVS_EX_ONECLICKACTIVATE) ||
			((plvdi->plv->exStyle & LVS_EX_TWOCLICKACTIVATE) &&
				ListView_OnGetItemState(plvdi->plv, (int) plvdi->nmcd.nmcd.dwItemSpec, LVIS_SELECTED)))
		{
			if ((plvdi->plv->exStyle & LVS_EX_UNDERLINEHOT) &&
				(plvdi->plv->hFontHot))
			{
				SelectFont(plvdi->nmcd.nmcd.hdc, plvdi->plv->hFontHot);
			}
			else
			{
				SelectFont(plvdi->nmcd.nmcd.hdc, plvdi->plv->hfontLabel);
			}
			plvdi->nmcd.nmcd.uItemState |= CDIS_HOT;
		}
	}
	else if ((plvdi->plv->exStyle & LVS_EX_ONECLICKACTIVATE) ||
		((plvdi->plv->exStyle & LVS_EX_TWOCLICKACTIVATE) &&
			ListView_OnGetItemState(plvdi->plv, (int) plvdi->nmcd.nmcd.dwItemSpec, LVIS_SELECTED)))
	{
		// Handle the non-hot webview case
		if ((plvdi->plv->exStyle & LVS_EX_UNDERLINECOLD) && (plvdi->plv->hFontHot))
		{
			SelectFont(plvdi->nmcd.nmcd.hdc, plvdi->plv->hFontHot);
		}
		else
		{
			SelectFont(plvdi->nmcd.nmcd.hdc, plvdi->plv->hfontLabel);
		}
	}
	else
	{
		// Handle the non-webview case
		SelectFont(plvdi->nmcd.nmcd.hdc, plvdi->plv->hfontLabel);
	}


	plvdi->dwCustom = CICustomDrawNotify(&plvdi->plv->hwnd->ci, CDDS_ITEMPREPAINT, &plvdi->nmcd.nmcd);

	plvdi->flags &= ~(LVDI_FOCUS | LVDI_SELECTED);
	if (plvdi->nmcd.nmcd.uItemState & CDIS_FOCUS)
	{
		plvdi->flags |= LVDI_FOCUS;
	}

	if (plvdi->nmcd.nmcd.uItemState & CDIS_SELECTED)
	{
		if (plvdi->plv->flags & LVF_FOCUSED)
		{
			plvdi->flags |= LVDI_SELECTED;
		}
		else
		{
			plvdi->flags |= LVDI_SELECTNOFOCUS;
		}
		if (plvdi->plv->iHot == (int)plvdi->nmcd.nmcd.dwItemSpec)
		{
			plvdi->flags |= LVDI_HOTSELECTED;
		}
	}

	if (!(plvdi->dwCustom & CDRF_SKIPDEFAULT))
	{

		if (!ListView_IsOwnerData(plvdi->plv))
		{

#ifdef DEBUG_NEWFONT
			if ((plvdi->nmcd.nmcd.dwItemSpec % 3) == 0)
			{
				plvdi->dwCustom |= CDRF_NEWFONT;
				SelectObject(plvdi->nmcd.nmcd.hdc, GetStockObject(SYSTEM_FONT));
			}
#endif

			if (plvdi->dwCustom & CDRF_NEWFONT)
			{
				ListView_RecomputeLabelSize(plvdi->plv, plvdi->pitem, (int) plvdi->nmcd.nmcd.dwItemSpec, plvdi->nmcd.nmcd.hdc, FALSE);
			}
		}

		bRet = _ListView_DrawItem(plvdi);


		if (plvdi->dwCustom & CDRF_NOTIFYPOSTPAINT)
		{
			plvdi->nmcd.iSubItem = 0;
			CICustomDrawNotify(&plvdi->plv->hwnd->ci, CDDS_ITEMPOSTPAINT, &plvdi->nmcd.nmcd);
		}
		if (plvdi->dwCustom & CDRF_NEWFONT)
		{
			SelectObject(plvdi->nmcd.nmcd.hdc, plvdi->plv->hfontLabel);
			plvdi->plv->flags |= LVF_CUSTOMFONT;
		}
	}
	return bRet;
}

// NOTE: this function requires a properly selected font.
//
void IUI::SHDrawText(HDC hdc, LPCTSTR pszText, RECT *prc, int fmt,
	UINT flags, int cyChar, int cxEllipses, COLORREF clrText, COLORREF clrTextBk)
{
	int cchText;
	COLORREF clrSave, clrSaveBk = 0;
	RECT rc;
	UINT uETOFlags = 0;
	BOOL fForeOnly = FALSE;
	TCHAR ach[CCHLABELMAX + CCHELLIPSES];

#ifdef WINDOWS_ME
	int align;
#endif

	// REVIEW: Performance idea:
	// We could cache the currently selected text color
	// so we don't have to set and restore it each time
	// when the color is the same.
	//
	if (!pszText)
	{
		return;
	}

	if (IsRectEmpty(prc))
	{
		return;
	}

#ifdef WINDOWS_ME
	if (flags & SHDT_RTLREADING)
	{
		align = GetTextAlign(hdc);
		SetTextAlign(hdc, align | TA_RTLREADING);
	}
#endif

	rc = *prc;

	// If needed, add in a little extra margin...
	//
	if (flags & SHDT_EXTRAMARGIN)
	{
		rc.left  += 1 * 3;
		rc.right -= 1 * 3;
	}
	else
	{
		rc.left  += 1;
		rc.right -= 1;
	}

	if ((rc.left >= rc.right) && !(flags & (SHDT_SELECTED | SHDT_DESELECTED | SHDT_SELECTNOFOCUS)))
	{
		return;
	}

	if ((flags & SHDT_ELLIPSES) &&
		ListView_NeedsEllipses(hdc, pszText, &rc, &cchText, cxEllipses))
	{
		// In some cases cchText was comming back bigger than
		// ARRYASIZE(ach), so we need to make sure we don't overflow the buffer

		// if cchText is too big for the buffer, truncate it down to size
		if (cchText >= ARRAYSIZE(ach) - CCHELLIPSES)
		{
			cchText = ARRAYSIZE(ach) - CCHELLIPSES - 1;
		}

		hmemcpy(ach, pszText, cchText * sizeof(TCHAR));
		StringCchCopy(ach + cchText, CCHLABELMAX + CCHELLIPSES - cchText, TEXT("..."));

		pszText = ach;

		// Left-justify, in case there's no room for all of ellipses
		//
		fmt = LVCFMT_LEFT;

		cchText += CCHELLIPSES;
	}
	else
	{
		cchText = lstrlen(pszText);
	}

	if (((clrTextBk == CLR_NONE) && !(flags & (SHDT_SELECTED | SHDT_SELECTNOFOCUS))) || (flags & SHDT_TRANSPARENT))
	{
		fForeOnly = TRUE;
		clrSave = ::SetTextColor(hdc, (flags & SHDT_TRANSPARENT) ? 0 : clrText);
	}
	else
	{
		HBRUSH hbrUse = NULL;
		HBRUSH hbrDelete = NULL;

		uETOFlags |= ETO_OPAQUE;

		if (flags & SHDT_SELECTED)
		{
			clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			clrTextBk = (flags & SHDT_HOTSELECTED) ? GetSysColor(COLOR_HOTLIGHT) : GetSysColor(COLOR_HIGHLIGHT);

			if (flags & SHDT_DRAWTEXT)
			{
				hbrUse = (flags & SHDT_HOTSELECTED) ? GetSysColorBrush(COLOR_HOTLIGHT) : GetSysColorBrush(COLOR_HIGHLIGHT);
			}

		}
		else if (flags & SHDT_SELECTNOFOCUS)
		{
			if ((clrTextBk == CLR_DEFAULT ? GetSysColor(COLOR_WINDOW) : clrTextBk) == GetSysColor(COLOR_BTNFACE))
			{
				// if the text background color in this mode is the same as the current
				// background, use the color highlight text so that you can actually see somehting
				clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
				clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
				if (flags & SHDT_DRAWTEXT)
				{
					hbrUse = GetSysColorBrush(COLOR_HIGHLIGHT);
				}
			}
			else
			{
				clrText = GetSysColor(COLOR_BTNTEXT);
				clrTextBk = GetSysColor(COLOR_BTNFACE);
				if (flags & SHDT_DRAWTEXT)
				{
					hbrUse = GetSysColorBrush(COLOR_BTNFACE);
				}
			}

#ifdef LVDEBUG
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
				clrText = g_clrBtnHighlight;
			}
#endif
		}
		else if (clrText == CLR_DEFAULT && clrTextBk == CLR_DEFAULT)
		{
			clrText = GetSysColor(COLOR_WINDOWTEXT);
			clrTextBk = GetSysColor(COLOR_WINDOW);

			if ((flags & (SHDT_DRAWTEXT | SHDT_DESELECTED)) ==
				(SHDT_DRAWTEXT | SHDT_DESELECTED))
			{
				hbrUse = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			}
		}
		else
		{
			if (clrText == CLR_DEFAULT)
			{
				clrText = GetSysColor(COLOR_WINDOWTEXT);
			}

			if (clrTextBk == CLR_DEFAULT)
			{
				clrTextBk = GetSysColor(COLOR_WINDOW);
			}

			if ((flags & (SHDT_DRAWTEXT | SHDT_DESELECTED)) ==
				(SHDT_DRAWTEXT | SHDT_DESELECTED))
			{
				hbrUse = CreateSolidBrush(GetNearestColor(hdc, clrTextBk));
				if (hbrUse)
				{
					hbrDelete = hbrUse;
				}
				else
				{
					hbrUse = (HBRUSH)GetStockObject(WHITE_BRUSH);
				}
			}
		}

		// now set it
		clrSave = ::SetTextColor(hdc, clrText);
		// 绘制文本背景色
		//clrSaveBk = SetBkColor(hdc, clrTextBk);
		if (hbrUse)
		{
			FillRect(hdc, prc, hbrUse);
			if (hbrDelete)
			{
				DeleteObject(hbrDelete);
			}
		}
	}

	// If we want the item to display as if it was depressed, we will
	// offset the text rectangle down and to the left
	if (flags & SHDT_DEPRESSED)
	{
		OffsetRect(&rc, 8, 8);
	}

	if (flags & SHDT_DRAWTEXT)
	{
		UINT uDTFlags = DT_LVWRAP | DT_END_ELLIPSIS;

		if (flags & SHDT_DTELLIPSIS)
		{
			uDTFlags |= DT_WORD_ELLIPSIS;
		}

		if (!(flags & SHDT_CLIPPED))
		{
			uDTFlags |= DT_NOCLIP;
		}

		if (flags & SHDT_NODBCSBREAK)
		{
			uDTFlags |= DT_NOFULLWIDTHCHARBREAK;
		}

		DrawText(hdc, pszText, cchText, &rc, uDTFlags);
	}
	else
	{
		if (fmt != LVCFMT_LEFT)
		{
			SIZE siz;

			GetTextExtentPoint(hdc, pszText, cchText, &siz);

			if (fmt == LVCFMT_CENTER)
			{
				rc.left = (rc.left + rc.right - siz.cx) / 2;
			}
			else    // fmt == LVCFMT_RIGHT
			{
				rc.left = rc.right - siz.cx;
			}
		}

		// Center vertically in case the bitmap (to the left) is larger than
		// the height of one line
		rc.top += (rc.bottom - rc.top - cyChar) / 2;

		if (flags & SHDT_CLIPPED)
		{
			uETOFlags |= ETO_CLIPPED;
		}

		// 发现即使调用SetBkMode(hdc, TRANSPARENT)，ExtTextOut也不能使文字透明，故改成DrawText
		DrawText(hdc, pszText, cchText, &rc, DT_SINGLELINE | DT_LEFT);
	}

	if (flags & (SHDT_SELECTED | SHDT_DESELECTED | SHDT_TRANSPARENT))
	{
		::SetTextColor(hdc, clrSave);
		if (!fForeOnly)
		{
			// 恢复文本背景色
			// SetBkColor(hdc, clrSaveBk);
		}
	}

#ifdef WINDOWS_ME
	if (flags & SHDT_RTLREADING)
	{
		SetTextAlign(hdc, align);
	}
#endif
}

/*----------------------------------------------------------------
** Create an imagelist to be used for dragging.
**
** 1) create mask and image bitmap matching the select bounds size
** 2) draw the text to both bitmaps (in black for now)
** 3) create an imagelist with these bitmaps
** 4) make a dithered copy of the image onto the new imagelist
**----------------------------------------------------------------*/
HIMAGELIST ListView_OnCreateDragImage(SListViewData *plv, int iItem, LPPOINT lpptUpLeft)
{
	HWND hwndLV = GetHostWnd(plv->hwnd);
	RECT rcBounds, rcImage, rcLabel;
	HDC hdcMem = NULL;
	HBITMAP hbmImage = NULL;
	HBITMAP hbmMask = NULL;
	HBITMAP hbmOld;
	HIMAGELIST himl = NULL;
	int dx, dy;
	HIMAGELIST himlSrc;
	LVITEMEX item;
	POINT ptOrg;
	LVDRAWITEM lvdi;
	RECT rcSelBounds;
	BOOL bMirroredWnd = (plv->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW);

	if (!lpptUpLeft)
	{
		return NULL;
	}

	if (iItem >= ListView_Count(plv))
	{
		return NULL;
	}

	if (plv->iHot == iItem)
	{
		ListView_OnSetHotItem(plv, -1);
		UpdateWindow(GetHostWnd(plv->hwnd));
	}

	ListView_GetRects(plv, iItem, &rcImage, &rcLabel, &rcBounds, &rcSelBounds);

	if (ListView_IsIconView(plv))
	{
		ListView_UnfoldRects(plv, iItem, &rcImage, &rcLabel,
			&rcBounds, &rcSelBounds);
		InflateRect(&rcImage, -plv->g_cxIconMargin, -plv->g_cyIconMargin);
	}

	// chop off any extra filler above icon
	ptOrg.x = rcBounds.left - rcSelBounds.left;
	ptOrg.y = rcBounds.top - rcImage.top;
	dx = rcSelBounds.right - rcSelBounds.left;
	dy = rcSelBounds.bottom - rcImage.top;

	lpptUpLeft->x = rcSelBounds.left;
	lpptUpLeft->y = rcImage.top;

	if (!(hdcMem = CreateCompatibleDC(NULL)))
	{
		goto CDI_Exit;
	}
	if (!(hbmImage = CreateColorBitmap(dx, dy)))
	{
		goto CDI_Exit;
	}
	if (!(hbmMask = CreateMonoBitmap(dx, dy)))
	{
		goto CDI_Exit;
	}

	//
	// Mirror the memory DC so that the transition from
	// mirrored(memDC)->non-mirrored(imagelist DCs)->mirrored(screenDC)
	// is consistent. [samera]
	//
	//L if (bMirroredWnd)
	//{
	//	SET_DC_RTL_MIRRORED(hdcMem);
	//}

	// prepare for drawing the item
	SelectObject(hdcMem, plv->hfontLabel);
	SetBkMode(hdcMem, TRANSPARENT);

	lvdi.plv = plv;
	lvdi.nmcd.nmcd.dwItemSpec = iItem;
	lvdi.pitem = NULL;  // make sure it is null as Owner data uses this to trigger things...
	lvdi.nmcd.nmcd.hdc = hdcMem;
	lvdi.lpptOrg = &ptOrg;
	lvdi.prcClip = NULL;
	lvdi.flags = LVDI_NOIMAGE | LVDI_TRANSTEXT | LVDI_NOWAYFOCUS | LVDI_UNFOLDED;
	/*
	** draw the text to both bitmaps
	*/
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmImage);
	// fill image with black for transparency
	PatBlt(hdcMem, 0, 0, dx, dy, BLACKNESS);
	ListView_DrawItem(&lvdi);
	//L if (bMirroredWnd)
	//{
	//	MirrorBitmapInDC(hdcMem, hbmImage);
	//}

	lvdi.flags = LVDI_NOIMAGE | LVDI_TRANSTEXT | LVDI_NOWAYFOCUS | LVDI_UNFOLDED;
	SelectObject(hdcMem, hbmMask);
	// fill mask with white for transparency
	PatBlt(hdcMem, 0, 0, dx, dy, WHITENESS);
	ListView_DrawItem(&lvdi);
	//L if (bMirroredWnd)
	//{
	//	MirrorBitmapInDC(hdcMem, hbmMask);
	//}

	// unselect objects that we used
	SelectObject(hdcMem, hbmOld);
	SelectObject(hdcMem, (HFONT)GetStockObject(SYSTEM_FONT));

	himlSrc = ListView_OnGetImageList(plv, !(ListView_IsIconView(plv)));

	/*
	** make an image list that for now only has the text
	** we use ImageList_Clone so we get a imagelist that
	** the same color depth as our own imagelist
	*/
	//L if (!(himl = ImageList_Clone(himlSrc, dx, dy, ILC_MASK, 1, 0)))
	//{
	//	goto CDI_Exit;
	//}

	ImageList_SetBkColor(himl, CLR_NONE);
	ImageList_Add(himl, hbmImage, hbmMask);

	/*
	** make a dithered copy of the image part onto our bitmaps
	** (need both bitmap and mask to be dithered)
	*/
	if (himlSrc)
	{
		item.iItem = iItem;
		item.iSubItem = 0;
		item.mask = LVIF_IMAGE | LVIF_STATE;
		item.stateMask = LVIS_OVERLAYMASK;
		ListView_OnGetItem(plv, &item);

		//L ImageList_CopyDitherImage(himl, 0, rcImage.left - rcSelBounds.left, 0,
		//	himlSrc, item.iImage,
		//	((plv->hwnd->ci.dwExStyle & dwExStyleRTLMirrorWnd) ? ILD_MIRROR : 0L) | (item.state & LVIS_OVERLAYMASK));
	}

CDI_Exit:
	if (hdcMem)
	{
		DeleteObject(hdcMem);
	}
	if (hbmImage)
	{
		DeleteObject(hbmImage);
	}
	if (hbmMask)
	{
		DeleteObject(hbmMask);
	}

	return himl;
}


//-------------------------------------------------------------------
// ListView_OnGetTopIndex -- Gets the index of the first visible item
// For list view and report view this calculates the actual index
// for iconic views it alway returns 0
//
int ListView_OnGetTopIndex(SListViewData *plv)
{
	if (ListView_IsReportView(plv))
	{
		return (int)((plv->ptlRptOrigin.y) / plv->cyItem);
	}

	else if (ListView_IsListView(plv))
	{
		return (plv->xOrigin / plv->cxItem) * plv->cItemCol;
	}

	else
	{
		return (0);
	}
}




//-------------------------------------------------------------------
// ListView_OnGetCountPerPage -- Gets the count of items that will fit
// on a page For list view and report view this calculates the
// count depending on the size of the window and for Iconic views it
// will always return the count of items in the list view.
//
int ListView_OnGetCountPerPage(SListViewData *plv)
{
	if (ListView_IsReportView(plv))
	{
		return (plv->sizeClient.cy - plv->yTop) / plv->cyItem;
	}

	else if (ListView_IsListView(plv))
		return ((plv->sizeClient.cx) / plv->cxItem)
			* plv->cItemCol;
	else
	{
		return (ListView_Count(plv));
	}
}


/*----------------------------------------------------------------------------
/ ListView_InvalidateFoldedItem implementation
/ -----------------------------
/ Purpose:
/   Provides support for invalidating items within list views.
/
/ Notes:
/   Copes with invalidating the extra region in the list view that requires
/   us to erase the background.  Design to optimise out the ERASURE of the
/   background.
/
/   For details on the API see ListView_InvalidateItem.
/
/ In:
/   plv->ListView structure to work with
/   iItem = item number
/   bSrelectionOnly = refesh the selection
/   fRedraw = Flags for RedrawWindow
/ Out:
/   -
/----------------------------------------------------------------------------*/
void ListView_InvalidateFoldedItem(SListViewData *plv, int iItem, BOOL fSelectionOnly, UINT fRedraw)
{
	ListView_InvalidateItem(plv, iItem, fSelectionOnly, fRedraw);

	if (ListView_IsIconView(plv) &&
		(!ListView_IsItemUnfolded(plv, iItem) || (fRedraw & RDW_ERASE)))
	{
		RECT rcLabel;

		if (ListView_GetUnfoldedRect(plv, iItem, &rcLabel))
		{
			RedrawWindow(GetHostWnd(plv->hwnd), &rcLabel, NULL, fRedraw | RDW_ERASE);
		}
	}
}


/*----------------------------------------------------------------------------
/ ListView_UnfoldedRects implementation
/ ----------------------
/ Purpose:
/   Having previously called get rects, then call this function to ensure
/   that they are correctly unfolded.
/
/ Notes:
/   -
/
/ In:
/   plv-> list view to unfold on
/   iItem = item number
/   prcIcon -> icon bounding box
/   prcLabel -> rectangle for the label structure
/   prcBounds -> bounds rectangle / == NULL for none    / These are currently the same for large icons
/   prcSelectBounds -> selection bounds / == NULL       /
/ Out: TRUE if unfolding the item was worth anything
/   -
/----------------------------------------------------------------------------*/
BOOL ListView_UnfoldRects(SListViewData *plv, int iItem,
	RECT *prcIcon, RECT *prcLabel,
	RECT *prcBounds, RECT *prcSelectBounds)
{
	LISTITEM item;
	LISTITEM *pitem = &item;
	BOOL fRc = FALSE;

	if (!ListView_IsIconView(plv))
	{
		return fRc;
	}

	// If we have a label pointer then expand as required
	// nb - different paths for owner data

	if (prcLabel)
	{
		if (!ListView_IsOwnerData(plv))
		{
			pitem = ListView_GetItemPtr(plv, iItem);
			if (!EVAL(pitem))
			{
				// DavidShi was able to get us into here with an invalid
				// item number during a delete notification.  So if the
				// item number is invalid, just return a blank rectangle
				// instead of faulting.
				SetRectEmpty(prcLabel);
				goto doneLabel;
			}
		}
		else
		{
			ListView_RecomputeLabelSize(plv, pitem, iItem, NULL, FALSE);
		}

		if (prcLabel->bottom != prcLabel->top + max(pitem->cyUnfoldedLabel, pitem->cyFoldedLabel))
		{
			fRc = TRUE;
		}

		prcLabel->bottom = prcLabel->top + pitem->cyUnfoldedLabel;

	}
doneLabel:

	// Build the unions if required

	if (prcBounds && prcIcon && prcLabel)
	{
		UnionRect(prcBounds, prcIcon, prcLabel);
	}
	if (prcSelectBounds && prcIcon && prcLabel)
	{
		UnionRect(prcSelectBounds, prcIcon, prcLabel);
	}

	return fRc;
}


//////////////////////////////////////////////////////////////////////////
// ListView Icon模式

#if defined(FE_IME) || !defined(WINNT)
static TCHAR const szIMECompPos[] = TEXT("IMECompPos");
#endif


__inline int ICONCXLABEL(SListViewData *plv, LISTITEM *pitem)
{
	if (plv->hwnd->ci.style & LVS_NOLABELWRAP)
	{
		_ASSERT(pitem->cxSingleLabel == pitem->cxMultiLabel);
	}
	return pitem->cxMultiLabel;
}

int LV_GetNewColWidth(SListViewData *plv, int iFirst, int iLast);
void LV_AdjustViewRectOnMove(SListViewData *plv, LISTITEM *pitem, int x, int y);
UINT LV_IsItemOnViewEdge(SListViewData *plv, LISTITEM *pitem);
void ListView_RecalcRegion(SListViewData *plv, BOOL fForce, BOOL fRedraw);
void _CalcSlotRect(SListViewData *plv, LISTITEM *pItem, int iSlot, int cSlot, BOOL fBias, LPRECT lprc);

extern BOOL g_fSlowMachine;

BOOL ListView_IconViewDrawItem(PLVDRAWITEM plvdi)
{
	RECT rcIcon;
	RECT rcLabel;
	RECT rcBounds;
	RECT rcT;
	TCHAR ach[CCHLABELMAX];
	LVITEMEX item;
	int i = (int)plvdi->nmcd.nmcd.dwItemSpec;
	SListViewData *plv = plvdi->plv;
	LISTITEM *pitem;
	BOOL fUnfolded;

	if (ListView_IsOwnerData(plv))
	{
		LISTITEM litem;

		// moved here to reduce call backs in OWNERDATA case
		item.iItem = i;
		item.iSubItem = 0;
		item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
		item.stateMask = LVIS_ALL;
		item.pszText = ach;
		item.cchTextMax = ARRAYSIZE(ach);
		ListView_OnGetItem(plv, &item);

		litem.pszText = item.pszText;
		ListView_GetRectsOwnerData(plv, i, &rcIcon, &rcLabel, &rcBounds, NULL, &litem);
		pitem = NULL;
	}
	else
	{
		pitem = ListView_GetItemPtr(plv, i);
		// NOTE this will do a GetItem LVIF_TEXT iff needed
		ListView_GetRects(plv, i, &rcIcon, &rcLabel, &rcBounds, NULL);
	}

	fUnfolded = FALSE;
	if ((plvdi->flags & LVDI_UNFOLDED) || ListView_IsItemUnfolded(plv, i))
	{
		ListView_UnfoldRects(plv, i, &rcIcon, &rcLabel, &rcBounds, NULL);
		fUnfolded = TRUE;
	}

	if (!plvdi->prcClip || IntersectRect(&rcT, &rcBounds, plvdi->prcClip))
	{
		UINT fText;

		if (!ListView_IsOwnerData(plv))
		{
			item.iItem = i;
			item.iSubItem = 0;
			item.mask = LVIF_TEXT | LVIF_STATE | LVIF_IMAGE | LVIF_BINDWL;
			item.stateMask = LVIS_ALL;
			item.pszText = ach;
			item.cchTextMax = ARRAYSIZE(ach);
			ListView_OnGetItem(plv, &item);

			// Make sure the listview hasn't been altered during
			// the callback to get the item info

			if (pitem != ListView_GetItemPtr(plv, i))
			{
				return FALSE;
			}
		}

		if (plvdi->lpptOrg)
		{
			OffsetRect(&rcIcon, plvdi->lpptOrg->x - rcBounds.left,
				plvdi->lpptOrg->y - rcBounds.top);
			OffsetRect(&rcLabel, plvdi->lpptOrg->x - rcBounds.left,
				plvdi->lpptOrg->y - rcBounds.top);
		}

		// 计算单元格坐标
		CIUIRect rcCell;
		int cSlot = ListView_GetSlotCount(plv, TRUE);
		_CalcSlotRect(plv, pitem, i, cSlot, FALSE, rcCell);

		// 在界面编辑器中显示单元格边界
		if (IsEditMode(plv->hwnd))
		{
			DrawFocusRect(plvdi->nmcd.nmcd.hdc, rcCell);
		}

		if (item.hBindWindowless != NULL)
		{
			//rcBindCtrl.left = plvdi->pitem->pt.x  + (plv->cxIcon / 2) - plv->cxIconSpacing / 2 - plv->ptOrigin.x;
			//rcBindCtrl.right = rcBindCtrl.left + plv->cxIconSpacing;
			//rcBindCtrl.top = pitem->pt.y - g_cyIconOffset;
			//rcBindCtrl.bottom = rcBindCtrl.top  + plv->cyIconSpacing;

			CIUIRect rcOld;
			GetWindowRectToParent(item.hBindWindowless, rcOld);

			if (rcOld != rcCell)
			{
				MoveWindow(item.hBindWindowless, &rcCell, FALSE);
			}

			// 备份视口、裁剪区域
			POINT ptOrg;
			GetViewportOrgEx(plvdi->nmcd.nmcd.hdc, &ptOrg);

			HRGN hOldRgn = CreateRectRgn(0, 0, 0, 0);
			GetClipRgn(plvdi->nmcd.nmcd.hdc, hOldRgn);

			// 绘制绑定的控件
			DrawControlAndChildren(item.hBindWindowless, plvdi->nmcd.nmcd.hdc, &rcBounds);

			// 恢复视口：
			// 注意：IUI默认绘制机制是先绘制父控件，等父控件整个绘制完后，再绘制子控件。
			// 虽然子控件绘制时，会修改HDC的视口，但这时父控件已彻底绘制完了，所以子控件
			// 修改HDC视口，也不会影响父控件的绘制。
			// 但由于ListView的特殊性，ListView是在绘制自己的时候，同时把Item绑定的控件
			// 进行了绘制，而在绘制完Item绑定的控件后，HDC的视口已不是ListView的视口了，
			// 而这时，ListView还未绘制完，这将导致ListView之后的绘制位置全部错误。
			// 同理，裁剪区域也需要恢复
			SetViewportOrgEx(plvdi->nmcd.nmcd.hdc, ptOrg.x, ptOrg.y, NULL);
			SelectClipRgn(plvdi->nmcd.nmcd.hdc, hOldRgn);
			DeleteObject(hOldRgn);
			hOldRgn = NULL;
		}
		else
		{
			if (ListView_IsIconView(plv))
			{
				fText = ListView_DrawImage(plv, &item, plvdi->nmcd.nmcd.hdc,
						rcIcon.left + IUIGetGloabelVar()->g_cxIconMargin, rcIcon.top + IUIGetGloabelVar()->g_cyIconMargin, plvdi->flags);

				// If linebreaking needs to happen, then use SHDT_DRAWTEXT.
				// Otherwise, use our (hopefully faster) internal SHDT_ELLIPSES
				if (rcLabel.bottom - rcLabel.top > plv->cyLabelChar)
				{
					fText |= SHDT_DRAWTEXT;
				}
				else
				{
					fText |= SHDT_ELLIPSES;
				}

				// We only use DT_NOFULLWIDTHCHARBREAK on Korean(949) Memphis and NT5
				//L if (949 == g_uiACP && (g_bRunOnNT5 || g_bRunOnMemphis))
				//    fText |= SHDT_NODBCSBREAK;

			}
			else
			{
				fText = ListView_DrawImage(plv, &item, plvdi->nmcd.nmcd.hdc,
						rcIcon.left, rcIcon.top, plvdi->flags);


			}
		}

		// Don't draw label if it's being edited...
		//
		if (plv->iEdit != i)
		{
			// If multiline label, then we need to use DrawText
			if (rcLabel.bottom - rcLabel.top > plv->cyLabelChar)
			{
				fText |= SHDT_DRAWTEXT;

				// If the text is folded, we need to clip and add ellipses

				if (!fUnfolded)
				{
					fText |= SHDT_CLIPPED | SHDT_DTELLIPSIS;
				}

				if (ListView_IsOwnerData(plv))
				{
					// If owner data, we have no z-order and if long names they will over lap each
					// other, better to truncate for now...
					if (ListView_IsSmallView(plv))
					{
						fText |= SHDT_ELLIPSES;
					}
				}

			}
			else
			{
				fText |= SHDT_ELLIPSES;
			}

			if (plvdi->flags & LVDI_TRANSTEXT)
			{
				fText |= SHDT_TRANSPARENT;
			}

			if ((fText & SHDT_SELECTED) && (plvdi->flags & LVDI_HOTSELECTED))
			{
				fText |= SHDT_HOTSELECTED;
			}

			if (item.pszText && (*item.pszText))
			{
#ifdef WINDOWS_ME
				if (plv->dwExStyle & WS_EX_RTLREADING)
				{
					fText |= SHDT_RTLREADING;
				}
#endif
				SHDrawText(plvdi->nmcd.nmcd.hdc, item.pszText, &rcLabel, LVCFMT_LEFT, fText,
					plv->cyLabelChar, plv->cxEllipses,
					plvdi->nmcd.clrText, plvdi->nmcd.clrTextBk);

				if ((plvdi->flags & LVDI_FOCUS) && (item.state & LVIS_FOCUSED)
#ifdef KEYBOARDCUES
					&& !(CCGetUIState(&(plvdi->plv->hwnd->ci)) & UISF_HIDEFOCUS)
#endif
				)
				{
					DrawFocusRect(plvdi->nmcd.nmcd.hdc, &rcLabel);
				}
			}
		}
	}
	return TRUE;
}

void ListView_RefoldLabelRect(SListViewData *plv, RECT *prcLabel, LISTITEM *pitem)
{
	int bottom = pitem->cyUnfoldedLabel;
	bottom = min(bottom, pitem->cyFoldedLabel);
	bottom = min(bottom, CLIP_HEIGHT);
	prcLabel->bottom = prcLabel->top + bottom;
}

int ListView_IItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem)
{
	int iHit;
	UINT flags;
	POINT pt;
	RECT rcLabel;
	RECT rcIcon;
	RECT rcState;

	if (piSubItem)
	{
		*piSubItem = 0;
	}

	// Map window-relative coordinates to view-relative coords...
	//
	pt.x = x + plv->ptOrigin.x;
	pt.y = y + plv->ptOrigin.y;

	// If there are any uncomputed items, recompute them now.
	//
	if (plv->rcView.left == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	flags = 0;

	if (ListView_IsOwnerData(plv))
	{
		int cSlots;
		POINT ptWnd;
		LISTITEM item;

		cSlots = ListView_GetSlotCount(plv, TRUE);
		iHit = ListView_CalcHitSlot(plv, pt, cSlots);
		if (iHit < ListView_Count(plv))
		{
			ListView_IGetRectsOwnerData(plv, iHit, &rcIcon, &rcLabel, &item, FALSE);
			ptWnd.x = x;
			ptWnd.y = y;
			if (PtInRect(&rcIcon, ptWnd))
			{
				flags = LVHT_ONITEMICON;
			}
			else if (PtInRect(&rcLabel, ptWnd))
			{
				flags = LVHT_ONITEMLABEL;
			}
		}
	}
	else
	{
		for (iHit = 0; (iHit < ListView_Count(plv)); iHit++)
		{
			LISTITEM *pitem = ListView_FastGetZItemPtr(plv, iHit);
			POINT ptItem;

			ptItem.x = pitem->pt.x;
			ptItem.y = pitem->pt.y;

			rcIcon.top = ptItem.y - IUIGetGloabelVar()->g_cyIconMargin;

			rcLabel.top = ptItem.y + plv->cyIcon + IUIGetGloabelVar()->g_cyLabelSpace;
			rcLabel.bottom = rcLabel.top + pitem->cyUnfoldedLabel;

			if (!ListView_IsItemUnfoldedPtr(plv, pitem))
			{
				ListView_RefoldLabelRect(plv, &rcLabel, pitem);
			}

			// Quick, easy rejection test...
			//
			if (pt.y < rcIcon.top || pt.y >= rcLabel.bottom)
			{
				continue;
			}

			rcIcon.left = ptItem.x - IUIGetGloabelVar()->g_cxIconMargin;
			rcIcon.right = ptItem.x + plv->cxIcon + IUIGetGloabelVar()->g_cxIconMargin;
			// We need to make sure there is no gap between the icon and label
			rcIcon.bottom = rcLabel.top;

			rcState.bottom = ptItem.y + plv->cyIcon;
			rcState.right = ptItem.x;
			rcState.top = rcState.bottom - plv->cyState;
			rcState.left = rcState.right - plv->cxState;

			rcLabel.left = ptItem.x + (plv->cxIcon / 2) - (ICONCXLABEL(plv, pitem) / 2);
			rcLabel.right = rcLabel.left + ICONCXLABEL(plv, pitem);

			if (plv->cxState && PtInRect(&rcState, pt))
			{
				flags = LVHT_ONITEMSTATEICON;
			}
			else if (PtInRect(&rcIcon, pt))
			{
				flags = LVHT_ONITEMICON;
			}
			else if (PtInRect(&rcLabel, pt))
			{
				flags = LVHT_ONITEMLABEL;
			}

			if (flags)
			{
				break;
			}
		}
	}

	if (flags == 0)
	{
		flags = LVHT_NOWHERE;
		iHit = -1;
	}
	else
	{
		if (!ListView_IsOwnerData(plv))
		{
			iHit = IUI_DPA_GetPtrIndex(plv->hdpa, ListView_FastGetZItemPtr(plv, iHit));
		}
	}

	*pflags = flags;
	return iHit;
}

// BUGBUG raymondc
// need to pass HDC here isnce it's sometimes called from the paint loop

void ListView_IGetRectsOwnerData(SListViewData *plv,
	int iItem,
	RECT *prcIcon,
	RECT *prcLabel,
	LISTITEM *pitem,
	BOOL fUsepitem)
{
	int itemIconXLabel;
	int cSlots;

	// calculate x, y from iItem
	cSlots = ListView_GetSlotCount(plv, TRUE);
	pitem->iWorkArea = 0;               // OwnerData doesn't support workareas
	ListView_SetIconPos(plv, pitem, iItem, cSlots);

	// calculate lable sizes from iItem
	ListView_RecomputeLabelSize(plv, pitem, iItem, NULL, fUsepitem);

	if (plv->hwnd->ci.style & LVS_NOLABELWRAP)
	{
		// use single label
		itemIconXLabel = pitem->cxSingleLabel;
	}
	else
	{
		// use multilabel
		itemIconXLabel = pitem->cxMultiLabel;
	}

	prcIcon->left = pitem->pt.x - IUIGetGloabelVar()->g_cxIconMargin - plv->ptOrigin.x;
	prcIcon->right = prcIcon->left + plv->cxIcon + 2 * IUIGetGloabelVar()->g_cxIconMargin;
	prcIcon->top = pitem->pt.y - IUIGetGloabelVar()->g_cyIconMargin - plv->ptOrigin.y;
	prcIcon->bottom = prcIcon->top + plv->cyIcon + 2 * IUIGetGloabelVar()->g_cyIconMargin;

	prcLabel->left = pitem->pt.x + (plv->cxIcon / 2) - (itemIconXLabel / 2) - plv->ptOrigin.x;
	prcLabel->right = prcLabel->left + itemIconXLabel;
	prcLabel->top = pitem->pt.y + plv->cyIcon + IUIGetGloabelVar()->g_cyLabelSpace - plv->ptOrigin.y;
	prcLabel->bottom = prcLabel->top + pitem->cyUnfoldedLabel;

	if (!ListView_IsItemUnfolded(plv, iItem))
	{
		ListView_RefoldLabelRect(plv, prcLabel, pitem);
	}
}


// 计算Icon view指定Item中图标和文本的实时坐标。 icon坐标包括icon边距
void ListView_IconViewGetRects(SListViewData *plv, LISTITEM *pitem, RECT *prcIcon, RECT *prcLabel, LPRECT prcBounds)
{
	int cxIconMargin;

	_ASSERT(!ListView_IsOwnerData(plv));

	if (pitem->pt.x == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	if (ListView_IsIconView(plv) && ((plv->cxIconSpacing - plv->cxIcon) < (2 * IUIGetGloabelVar()->g_cxIconMargin)))
	{
		cxIconMargin = (plv->cxIconSpacing - plv->cxIcon) / 2;
	}
	else
	{
		cxIconMargin = IUIGetGloabelVar()->g_cxIconMargin;
	}

	prcIcon->left = pitem->pt.x - cxIconMargin - plv->ptOrigin.x;
	prcIcon->right = prcIcon->left + plv->cxIcon + 2 * cxIconMargin;
	prcIcon->top = pitem->pt.y - IUIGetGloabelVar()->g_cyIconMargin - plv->ptOrigin.y;
	prcIcon->bottom = prcIcon->top + plv->cyIcon + 2 * IUIGetGloabelVar()->g_cyIconMargin;

	prcLabel->left = pitem->pt.x + (plv->cxIcon / 2) - (ICONCXLABEL(plv, pitem) / 2) - plv->ptOrigin.x;
	prcLabel->right = prcLabel->left + ICONCXLABEL(plv, pitem);
	prcLabel->top = pitem->pt.y + plv->cyIcon + IUIGetGloabelVar()->g_cyLabelSpace - plv->ptOrigin.y;
	prcLabel->bottom = prcLabel->top + pitem->cyUnfoldedLabel;

	if (!ListView_IsItemUnfoldedPtr(plv, pitem))
	{
		ListView_RefoldLabelRect(plv, prcLabel, pitem);
	}

}

int ListView_GetSlotCountEx(SListViewData *plv, BOOL fWithoutScrollbars, int iWorkArea)
{
	int cxScreen;
	int cyScreen;
	int dxItem;
	int dyItem;
	int iSlots = 1;
	BOOL fCheckWithScroll = FALSE;
	DWORD style = 0;

	// Always use the current client window size to determine
	//
	// REVIEW: Should we exclude any vertical scroll bar that may
	// exist when computing this?  progman.exe does not.
	//
	if ((iWorkArea >= 0) && (plv->nWorkAreas > 0))
	{
		_ASSERT(iWorkArea < plv->nWorkAreas);
		cxScreen = RECTWIDTH(plv->prcWorkAreas[iWorkArea]);
		cyScreen = RECTHEIGHT(plv->prcWorkAreas[iWorkArea]);
	}
	else
	{
		cxScreen = plv->sizeClient.cx;
		cyScreen = plv->sizeClient.cy;
	}

	if (fWithoutScrollbars)
	{
		style = ListView_GetWindowStyle(plv);

		if (style & WS_VSCROLL)
		{
			cxScreen += ListView_GetCxScrollbar(plv);
		}
		if (style & WS_HSCROLL)
		{
			cyScreen += ListView_GetCyScrollbar(plv);
		}
	}

	if (ListView_IsSmallView(plv))
	{
		dxItem = plv->cxItem;
	}
	else
	{
		dxItem = plv->cxIconSpacing;
	}

	if (ListView_IsSmallView(plv))
	{
		dyItem = plv->cyItem;
	}
	else
	{
		dyItem = plv->cyIconSpacing;
	}

	if (!dxItem)
	{
		dxItem = 1;
	}
	if (!dyItem)
	{
		dyItem = 1;
	}

	// Lets see which direction the view states
	switch (plv->hwnd->ci.style & LVS_ALIGNMASK)
	{
	case LVS_ALIGNBOTTOM:
	case LVS_ALIGNTOP:
#ifndef UNIX
		iSlots = max(1, (cxScreen) / dxItem);
#else
		iSlots = max(1, (cxScreen) / max(1, dxItem));
#endif
		fCheckWithScroll = (BOOL)(style & WS_VSCROLL);
		break;

	case LVS_ALIGNRIGHT:
	case LVS_ALIGNLEFT:
#ifndef UNIX
		iSlots = max(1, (cyScreen) / dyItem);
#else
		iSlots = max(1, (cyScreen) / max(1, dyItem));
#endif
		fCheckWithScroll = (BOOL)(style & WS_HSCROLL);
		break;

	default:
		_ASSERT(0);
		return 1;
	}

	// if we don't have enough slots total on the screen, we're going to have
	// a scrollbar, so recompute with the scrollbars on
	if (fWithoutScrollbars && fCheckWithScroll)
	{
		int iTotalSlots = (dxItem * dyItem);
		if (iTotalSlots < ListView_Count(plv))
		{
			iSlots = ListView_GetSlotCountEx(plv, FALSE, iWorkArea);
		}

	}

	return iSlots;
}


int ListView_GetSlotCount(SListViewData *plv, BOOL fWithoutScrollbars)
{
	// Make sure this function does exactly the same thing as when
	// we had no workareas
	return ListView_GetSlotCountEx(plv, fWithoutScrollbars, -1);
}

// get the pixel row (or col in left align) of pitem
int LV_GetItemPixelRow(SListViewData *plv, LISTITEM *pitem)
{
	if ((plv->hwnd->ci.style & LVS_ALIGNMASK) == LVS_ALIGNLEFT)
	{
		return pitem->pt.x;
	}
	else
	{
		return pitem->pt.y;
	}
}

// get the pixel row (or col in left align) of the lowest item
int LV_GetMaxPlacedItem(SListViewData *plv)
{
	int i;
	int iMaxPlacedItem = 0;

	for (i = 0; i < ListView_Count(plv); i++)
	{
		LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);
		if (pitem->pt.y != RECOMPUTE)
		{
			int iRow = LV_GetItemPixelRow(plv, pitem);
			// if the current item is "below" (on right if it's left aligned)
			// the lowest placed item, we can start appending
			if (!i || iRow > iMaxPlacedItem)
			{
				iMaxPlacedItem = iRow;
			}
		}
	}

	return iMaxPlacedItem;;
}

// Go through and recompute any icon positions and optionally
// icon label dimensions.
//
// This function also recomputes the view bounds rectangle.
//
// The algorithm is to simply search the list for any items needing
// recomputation.  For icon positions, we scan possible icon slots
// and check to see if any already-positioned icon intersects the slot.
// If not, the slot is free.  As an optimization, we start scanning
// icon slots from the previous slot we found.
//
void ListView_Recompute(SListViewData *plv)
{
	int i;
	int cSlots;
	int cWorkAreaSlots[LV_MAX_WORKAREAS];
	BOOL fUpdateSB;
	// if all the items are unplaced, we can just keep appending
	BOOL fAppendAtEnd = (((UINT)ListView_Count(plv)) == plv->uUnplaced);
	int iFree;

	plv->uUnplaced = 0;

	if (!(ListView_IsIconView(plv) || ListView_IsSmallView(plv)))
	{
		return;
	}

	if (plv->flags & LVF_INRECOMPUTE)
	{
		return;
	}
	plv->flags |= LVF_INRECOMPUTE;

	cSlots = ListView_GetSlotCount(plv, FALSE);

	if (plv->nWorkAreas > 0)
		for (i = 0; i < plv->nWorkAreas; i++)
		{
			cWorkAreaSlots[i] = ListView_GetSlotCountEx(plv, FALSE, i);
		}

	// Scan all items for RECOMPUTE, and recompute slot if needed.
	//
	fUpdateSB = (plv->rcView.left == RECOMPUTE);

	if (!ListView_IsOwnerData(plv))
	{
		LVFAKEDRAW lvfd;                    // in case client uses customdraw
		LVITEMEX item;                       // in case client uses customdraw
		int iMaxPlacedItem = RECOMPUTE;

		item.mask = LVIF_PARAM;
		item.iSubItem = 0;

		ListView_BeginFakeCustomDraw(plv, &lvfd, &item);

		if (!fAppendAtEnd)
		{
			iMaxPlacedItem = LV_GetMaxPlacedItem(plv);
		}

		// Must keep in local variable because ListView_SetIconPos will keep
		// invalidating the iFreeSlot cache while we're looping
		iFree = plv->iFreeSlot;
		for (i = 0; i < ListView_Count(plv); i++)
		{
			int cRealSlots;
			LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);
			BOOL fRedraw = FALSE;
			cRealSlots = (plv->nWorkAreas > 0) ? cWorkAreaSlots[pitem->iWorkArea] : cSlots;
			if (pitem->pt.y == RECOMPUTE)
			{
				if (pitem->cyFoldedLabel == SRECOMPUTE)
				{
					// Get the item lParam only if we need it for customdraw
					item.iItem = i;
					item.lParam = pitem->lParam;

					ListView_BeginFakeItemDraw(&lvfd);
					ListView_RecomputeLabelSize(plv, pitem, i, lvfd.nmcd.nmcd.hdc, FALSE);
					ListView_EndFakeItemDraw(&lvfd);
				}
				// BUGBUG: (dli) This function gets a new icon postion and then goes
				// through the whole set of items to see if that position is occupied
				// should let it know in the multi-workarea case, it only needs to go
				// through those who are in the same workarea.
				// This is okay for now because we cannot have too many items on the
				// desktop.
				iFree = ListView_FindFreeSlot(plv, i, iFree + 1, cRealSlots, &fUpdateSB, &fAppendAtEnd, lvfd.nmcd.nmcd.hdc);
				_ASSERT(iFree != -1);

				ListView_SetIconPos(plv, pitem, iFree, cRealSlots);

				if (!fAppendAtEnd)
				{
					//// optimization.  each time we calc a new free slot, we iterate through all the items to see
					// if any of the freely placed items collide with this.
					// fAppendAtEnd indicates that iFree is beyond any freely placed item
					//
					// if the current item is "below" (on right if it's left aligned)
					// the lowest placed item, we can start appending
					if (LV_GetItemPixelRow(plv, pitem) > iMaxPlacedItem)
					{
						fAppendAtEnd = TRUE;
					}
				}

				if (!fUpdateSB && LV_IsItemOnViewEdge(plv, pitem))
				{
					fUpdateSB = TRUE;
				}

				fRedraw = TRUE;
			}

			if (fRedraw)
			{
				ListView_InvalidateItem(plv, i, FALSE, RDW_INVALIDATE | RDW_ERASE);
			}
		}
		plv->iFreeSlot = iFree;
		ListView_EndFakeCustomDraw(&lvfd);

	}
	// If we changed something, recompute the view rectangle
	// and then update the scroll bars.
	//
	if (fUpdateSB || plv->rcView.left == RECOMPUTE)
	{

		TraceMsg(TF_GENERAL, "************ SListViewData: Expensive update! ******* ");

		// NOTE: No infinite recursion results because we're setting
		// plv->rcView.left != RECOMPUTE
		//
		SetRectEmpty(&plv->rcView);

		if (ListView_IsOwnerData(plv))
		{
			if (ListView_Count(plv) > 0)
			{
				RECT  rcLast;
				RECT  rcItem;
				int iSlots;
				int   iItem = ListView_Count(plv) - 1;

				ListView_GetRects(plv, 0, NULL, NULL, &plv->rcView, NULL);
				ListView_GetRects(plv, iItem, NULL, NULL, &rcLast, NULL);
				plv->rcView.right = rcLast.right;
				plv->rcView.bottom = rcLast.bottom;

				//
				// calc how far back in the list to check
				//
				iSlots = cSlots + 2;

				// REVIEW:  This cache hint notification causes a spurious
				//  hint, since this happens often but is always the last items
				//  available.  Should this hint be done at all and this information
				//  be cached local to the control?
				ListView_NotifyCacheHint(plv, max(0, iItem - iSlots), iItem);

				// move backwards from last item until either rc.right or
				// rc.left is greater than the last, then use that value.
				// Note: This code makes very little assumptions about the ordering
				// done.  We should be careful as multiple line text fields could
				// screw us up.
				for (iItem--;
					(iSlots > 0) && (iItem >= 0);
					iSlots--, iItem--)
				{
					RECT rcIcon;
					RECT rcLabel;

					ListView_GetRects(plv, iItem, &rcIcon, &rcLabel, &rcItem, NULL);
					ListView_UnfoldRects(plv, iItem, &rcIcon, &rcLabel, &rcItem, NULL);
					if (rcItem.right > rcLast.right)
					{
						plv->rcView.right = rcItem.right;
					}
					if (rcItem.bottom > rcLast.bottom)
					{
						plv->rcView.bottom = rcItem.bottom;
					}
				}
			}
		}
		else
		{
			for (i = 0; i < ListView_Count(plv); i++)
			{
				RECT rcIcon;
				RECT rcLabel;
				RECT rcItem;

				ListView_GetRects(plv, i, &rcIcon, &rcLabel, &rcItem, NULL);
				ListView_UnfoldRects(plv, i, &rcIcon, &rcLabel, &rcItem, NULL);
				UnionRect(&plv->rcView, &plv->rcView, &rcItem);
			}
		}
		// add a little space at the edges so that we don't bump text
		// completely to the end of the window
		plv->rcView.bottom += GetBorderRect(plv->hwnd).bottom;
		plv->rcView.right += GetBorderRect(plv->hwnd).bottom;

		OffsetRect(&plv->rcView, plv->ptOrigin.x, plv->ptOrigin.y);
		//TraceMsg(DM_TRACE, "RECOMPUTE: rcView %x %x %x %x", plv->rcView.left, plv->rcView.top, plv->rcView.right, plv->rcView.bottom);
		//TraceMsg(DM_TRACE, "Origin %x %x", plv->ptOrigin.x, plv->ptOrigin.y);

		ListView_UpdateScrollBars(plv);
	}
	ListView_RecalcRegion(plv, FALSE, TRUE);
	// Now state we are out of the recompute...
	plv->flags &= ~LVF_INRECOMPUTE;
}

void PASCAL NearestSlot(int *x, int *y, int cxItem, int cyItem, LPRECT prcWork)
{
	if (prcWork != NULL)
	{
		*x = *x - prcWork->left;
		*y = *y - prcWork->top;
	}

	if (*x < 0)
	{
		*x -= cxItem / 2;
	}
	else
	{
		*x += cxItem / 2;
	}

	if (*y < 0)
	{
		*y -= cyItem / 2;
	}
	else
	{
		*y += cyItem / 2;
	}

	*x = *x - (*x % cxItem);
	*y = *y - (*y % cyItem);

	if (prcWork != NULL)
	{
		*x = *x + prcWork->left;
		*y = *y + prcWork->top;
	}
}


void PASCAL NextSlot(SListViewData *plv, LPRECT lprc)
{
	int cxItem;

	if (ListView_IsSmallView(plv))
	{
		cxItem = plv->cxItem;
	}
	else
	{
		cxItem = plv->cxIconSpacing;
	}
	lprc->left += cxItem;
	lprc->right += cxItem;
}


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------

void ListView_CalcMinMaxIndex(SListViewData *plv, PRECT prcBounding, int *iMin, int *iMax)
{
	POINT pt;
	int cSlots;

	cSlots = ListView_GetSlotCount(plv, TRUE);

	pt.x = prcBounding->left + plv->ptOrigin.x;
	pt.y = prcBounding->top + plv->ptOrigin.y;
	*iMin = ListView_CalcHitSlot(plv, pt, cSlots);

	pt.x = prcBounding->right + plv->ptOrigin.x;
	pt.y = prcBounding->bottom + plv->ptOrigin.y;
	*iMax = ListView_CalcHitSlot(plv, pt, cSlots) + 1;
}
//-------------------------------------------------------------------
//
// Function: ListView_CalcHitSlot
//
// Summary: Given a point (relative to complete icon view), calculate
//    which slot that point is closest to.
//
// Arguments:
//    plv [in] - List view to work with
//    pt [in]  - location to check with
//    cslot [in]  - number of slots wide the current view is
//
// Notes: This does not guarentee that the point is hitting the item
//    located at that slot.  That should be checked by comparing rects.
//
// History:
//    Nov-1-1994  MikeMi   Added to improve Ownerdata hit testing
//
//-------------------------------------------------------------------

int ListView_CalcHitSlot(SListViewData *plv, POINT pt, int cSlot)
{
	int cxItem;
	int cyItem;
	int iSlot = 0;

	_ASSERT(plv);

	if (cSlot < 1)
	{
		cSlot = 1;
	}

	if (ListView_IsSmallView(plv))
	{
		cxItem = plv->cxItem;
		cyItem = plv->cyItem;
	}
	else
	{
		cxItem = plv->cxIconSpacing;
		cyItem = plv->cyIconSpacing;
	}

	// Lets see which direction the view states
	switch (plv->hwnd->ci.style & LVS_ALIGNMASK)
	{
	case LVS_ALIGNBOTTOM:
	// Assert False (Change default in shell2d.. to ALIGN_TOP)

	case LVS_ALIGNTOP:
		iSlot = (pt.x / cxItem) + (pt.y / cyItem) * cSlot;
		break;

	case LVS_ALIGNLEFT:
		iSlot = (pt.x / cxItem) * cSlot + (pt.y / cyItem);
		break;

	case LVS_ALIGNRIGHT:
		_ASSERT(FALSE);      // Not implemented yet...
		break;
	}

	return (iSlot);
}

void _GetCurrentItemSize(SListViewData *plv, int *pcx, int *pcy)
{
	if (ListView_IsSmallView(plv))
	{
		*pcx = plv->cxItem;
		*pcy = plv->cyItem;
	}
	else
	{
		*pcx = plv->cxIconSpacing;
		*pcy = plv->cyIconSpacing;
	}
}

DWORD ListView_IApproximateViewRect(SListViewData *plv, int iCount, int iWidth, int iHeight)
{
	int cxSave = plv->sizeClient.cx;
	int cySave = plv->sizeClient.cy;
	int cxItem;
	int cyItem;
	int cCols;
	int cRows;

	plv->sizeClient.cx = iWidth;
	plv->sizeClient.cy = iHeight;
	cCols = ListView_GetSlotCount(plv, TRUE);

	plv->sizeClient.cx = cxSave;
	plv->sizeClient.cy = cySave;

	cCols = min(cCols, iCount);
	if (cCols == 0)
	{
		cCols = 1;
	}
	cRows = (iCount + cCols - 1) / cCols;

	if (plv->hwnd->ci.style & (LVS_ALIGNLEFT | LVS_ALIGNRIGHT))
	{
		int c;

		c = cCols;
		cCols = cRows;
		cRows = c;
	}

	_GetCurrentItemSize(plv, &cxItem, &cyItem);

	iWidth = cCols * cxItem;
	iHeight = cRows * cyItem;

	return MAKELONG(iWidth + GetBorderRect(plv->hwnd).left, iHeight + GetBorderRect(plv->hwnd).bottom);
}

// 计算IconView槽坐标，按scroll为0算。在垂直滚动过程中，不需要重新计算。
void _CalcSlotRect(SListViewData *plv, LISTITEM *pItem, int iSlot, int cSlot, BOOL fBias, LPRECT lprc)
{
	int cxItem, cyItem;

	_ASSERT(plv);

	if (cSlot < 1)
	{
		cSlot = 1;
	}

	_GetCurrentItemSize(plv, &cxItem, &cyItem);

	// Lets see which direction the view states
	switch (plv->hwnd->ci.style & LVS_ALIGNMASK)
	{
	case LVS_ALIGNBOTTOM:
	// Assert False (Change default in shell2d.. to ALIGN_TOP)

	case LVS_ALIGNTOP:
		lprc->left = (iSlot % cSlot) * cxItem;
		lprc->top = (iSlot / cSlot) * cyItem;
		break;

	case LVS_ALIGNRIGHT:
		RIPMSG(0, "LVM_ARRANGE: Invalid listview icon arrangement style");
	// _ASSERT(FALSE);    // Not implemented yet...
	// fall through, use LVS_ALIGNLEFT instead

	case LVS_ALIGNLEFT:
		lprc->top = (iSlot % cSlot) * cyItem;
		lprc->left = (iSlot / cSlot) * cxItem;
		break;

	}

	if (fBias)
	{
		lprc->left -= plv->ptOrigin.x;
		lprc->top -= plv->ptOrigin.y;
	}
	lprc->bottom = lprc->top + cyItem;
	lprc->right = lprc->left + cxItem;

	// Multi-Workarea case offset from the workarea coordinate to the whole
	// listview coordinate.
	if (plv->nWorkAreas > 0)
	{
		_ASSERT(pItem);
		_ASSERT(pItem->iWorkArea < plv->nWorkAreas);
		OffsetRect(lprc, plv->prcWorkAreas[pItem->iWorkArea].left, plv->prcWorkAreas[pItem->iWorkArea].top);
	}

	OffsetRect(lprc, -plv->ptOrigin.x, -plv->ptOrigin.y);
}

// Intersect this rectangle with all items in this listview except myself,
// this will determine if this rectangle overlays any icons.
BOOL ListView_IsCleanRect(SListViewData *plv, RECT *prc, int iExcept, BOOL *pfUpdate, HDC hdc)
{
	int j;
	RECT rc;
	int cItems = ListView_Count(plv);
	for (j = cItems; j-- > 0;)
	{
		if (j == iExcept)
		{
			continue;
		}
		else
		{
			LISTITEM *pitem = ListView_FastGetItemPtr(plv, j);
			if (pitem->pt.y != RECOMPUTE)
			{
				// If the dimensions aren't computed, then do it now.
				//
				if (pitem->cyFoldedLabel == SRECOMPUTE)
				{
					ListView_RecomputeLabelSize(plv, pitem, j, hdc, FALSE);

					// Ensure that the item gets redrawn...
					//
					ListView_InvalidateItem(plv, j, FALSE, RDW_INVALIDATE | RDW_ERASE);

					// Set flag indicating that scroll bars need to be
					// adjusted.
					//
					if (LV_IsItemOnViewEdge(plv, pitem))
					{
						*pfUpdate = TRUE;
					}
				}


				ListView_GetRects(plv, j, NULL, NULL, &rc, NULL);

				if (IntersectRect(&rc, &rc, prc))
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

// Find an icon slot that doesn't intersect an icon.
// Start search for free slot from slot i.
//
int ListView_FindFreeSlot(SListViewData *plv, int iItem, int i, int cSlot, BOOL *pfUpdate,
	BOOL *pfAppend, HDC hdc)
{
	RECT rcSlot;
	RECT rcItem;
	RECT rc;
	LISTITEM *pItemLooking = ListView_FastGetItemPtr(plv, iItem);

	_ASSERT(!ListView_IsOwnerData(plv));

	// Horrible N-squared algorithm:
	// enumerate each slot and see if any items intersect it.
	//
	// REVIEW: This is really slow with long lists (e.g., 1000)
	//

	//
	// If the Append at end is set, we should be able to simply get the
	// rectangle of the i-1 element and check against it instead of
	// looking at every other item...
	//
	if (*pfAppend)
	{
		int iPrev = iItem - 1;
		// Be carefull about going of the end of the list. (i is a slot
		// number not an item index).

		if (plv->nWorkAreas > 0)
		{
			while (iPrev >= 0)
			{
				LISTITEM *pPrev = ListView_FastGetItemPtr(plv, iPrev);
				if (pPrev->iWorkArea == pItemLooking->iWorkArea)
				{
					break;
				}
				iPrev--;
			}
		}

		if (iPrev >= 0)
		{
			ListView_GetRects(plv, iPrev, NULL, NULL, &rcItem, NULL);
		}
		else
		{
			SetRect(&rcItem, 0, 0, 0, 0);
		}
	}

	for (; ; i++)
	{
		// Compute view-relative slot rectangle...
		//
		_CalcSlotRect(plv, pItemLooking, i, cSlot, TRUE, &rcSlot);

		if (*pfAppend)
		{
			if (!IntersectRect(&rc, &rcItem, &rcSlot))
			{
				return i;       // Found a free slot...
			}
		}

		if (ListView_IsCleanRect(plv, &rcSlot, iItem, pfUpdate, hdc))
		{
			break;
		}
	}

	return i;
}

// Recompute an item's label size (cxLabel/cyLabel).  For speed, this function
// is passed a DC to use for text measurement.
//
// If hdc is NULL, then this function will create and initialize a temporary
// DC, then destroy it.  If hdc is non-NULL, then it is assumed to have
// the correct font already selected into it.
//
// fUsepitem means not to use the text of the item.  Instead, use the text
// pointed to by the pitem structure.  This is used in two cases.
//
//  -   Ownerdata, because we don't have a real pitem.
//  -   Regulardata, where we already found the pitem text (as an optimizatin)
//
void ListView_RecomputeLabelSize(SListViewData *plv, LISTITEM *pitem, int i, HDC hdc, BOOL fUsepitem)
{
	TCHAR szLabel[CCHLABELMAX + 4];
	TCHAR szLabelFolded[ARRAYSIZE(szLabel) + CCHELLIPSES + CCHELLIPSES];
	int cchLabel;
	RECT rcSingle, rcFolded, rcUnfolded;
	LVFAKEDRAW lvfd;
	LVITEMEX item;

	_ASSERT(plv);

	// the following will use the passed in pitem text instead of calling
	// GetItem.  This would be two consecutive calls otherwise, in some cases.
	//
	if (fUsepitem && (pitem->pszText != LPSTR_TEXTCALLBACK))
	{
		IUIStr_GetPtr(pitem->pszText ? pitem->pszText : TEXT(""), szLabel, ARRAYSIZE(szLabel));
		item.lParam = pitem->lParam;
	}
	else
	{
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = szLabel;
		item.cchTextMax = ARRAYSIZE(szLabel);
		item.stateMask = 0;
		szLabel[0] = TEXT('\0');    // In case the OnGetItem fails
		ListView_OnGetItem(plv, &item);

		if (!item.pszText)
		{
			SetRectEmpty(&rcSingle);
			rcFolded = rcSingle;
			rcUnfolded = rcSingle;
			goto Exit;
		}

		if (item.pszText != szLabel)
		{
			lstrcpyn(szLabel, item.pszText, ARRAYSIZE(szLabel));
		}
	}

	cchLabel = lstrlen(szLabel);

	rcUnfolded.left = rcUnfolded.top = rcUnfolded.bottom = 0;
	rcUnfolded.right = plv->cxIconSpacing - IUIGetGloabelVar()->g_cxLabelMargin * 2;
	rcSingle = rcUnfolded;
	rcFolded = rcUnfolded;

	if (cchLabel > 0)
	{
		UINT flags;

		if (!hdc)                               // Set up fake customdraw
		{
			ListView_BeginFakeCustomDraw(plv, &lvfd, &item);
			ListView_BeginFakeItemDraw(&lvfd);
		}
		else
		{
			lvfd.nmcd.nmcd.hdc = hdc;    // Use the one the app gave us
		}

#if 0   // Don't do this - Stripping trailing spaces results in unwanted ellipses
		// Strip off spaces so they're not included in format
		// REVIEW: Is this is a DrawText bug?
		//
		while (cchLabel > 1 && szLabel[cchLabel - 1] == TEXT(' '))
		{
			szLabel[--cchLabel] = 0;
		}
#endif

		DrawText(lvfd.nmcd.nmcd.hdc, szLabel, cchLabel, &rcSingle, (DT_LV | DT_CALCRECT));

		if (plv->hwnd->ci.style & LVS_NOLABELWRAP)
		{
			flags = DT_LV | DT_CALCRECT;
		}
		else
		{
			flags = DT_LVWRAP | DT_CALCRECT;
			//L // We only use DT_NOFULLWIDTHCHARBREAK on Korean(949) Memphis and NT5
			//if (949 == g_uiACP && (g_bRunOnNT5 || g_bRunOnMemphis))
			//{
			//	flags |= DT_NOFULLWIDTHCHARBREAK;
			//}
		}

		DrawText(lvfd.nmcd.nmcd.hdc, szLabel, cchLabel, &rcUnfolded, flags);

		//
		//  DrawText with DT_MODIFYSTRING is quirky when you enable
		//  word ellipses.  Once it finds anything that requires ellipses,
		//  it stops and doesn't return anything else (even if those other
		//  things got displayed).
		//
		StringCchCopy(szLabelFolded, ARRAYSIZE(szLabel) + CCHELLIPSES + CCHELLIPSES, szLabel);
		DrawText(lvfd.nmcd.nmcd.hdc, szLabelFolded, cchLabel, &rcFolded, flags | DT_WORD_ELLIPSIS | DT_MODIFYSTRING);

		//  If we had to ellipsify, but you can't tell from looking at the
		//  rcFolded.bottom and rcUnfolded.bottom, then tweak rcFolded.bottom
		//  so the unfoldifier knows that unfolding is worthwhile.
		if (rcFolded.bottom == rcUnfolded.bottom &&
			lstrcmp(szLabel, szLabelFolded))
		{
			// The actual value isn't important, as long as it's greater
			// than rcUnfolded.bottom and CLIP_HEIGHT.  We take advantage
			// of the fact that CLIP_HEIGHT is only two lines, so the only
			// problem case is where you have a two-line item and only the
			// first line is ellipsified.
			rcFolded.bottom++;
		}

		if (!hdc)                               // Clean up fake customdraw
		{
			ListView_EndFakeItemDraw(&lvfd);
			ListView_EndFakeCustomDraw(&lvfd);
		}

	}
	else
	{
		rcFolded.bottom = rcUnfolded.bottom = rcUnfolded.top + plv->cyLabelChar;
	}

Exit:

	if (pitem)
	{
		int cyEdge;
		pitem->cxSingleLabel = (short)((rcSingle.right - rcSingle.left) + 2 * IUIGetGloabelVar()->g_cxLabelMargin);
		pitem->cxMultiLabel = (short)((rcUnfolded.right - rcUnfolded.left) + 2 * IUIGetGloabelVar()->g_cxLabelMargin);

		cyEdge = (plv->hwnd->ci.style & LVS_NOLABELWRAP) ? 0 : GetBorderRect(plv->hwnd).bottom;

		pitem->cyFoldedLabel = (short)((rcFolded.bottom - rcFolded.top) + cyEdge);
		pitem->cyUnfoldedLabel = (short)((rcUnfolded.bottom - rcUnfolded.top) + cyEdge);
	}

}

// 设置Icon位置。 如果位置没改变，返回FALSE.
// iSlot: Icon所在槽的索引
// cSlot: 每行可容纳的槽的数量
BOOL ListView_SetIconPos(SListViewData *plv, LISTITEM *pitem, int iSlot, int cSlot)
{
	RECT rc;

	_ASSERT(plv);

	//
	// Sort of a hack, this internal function return TRUE if small icon.

	_CalcSlotRect(plv, pitem, iSlot, cSlot, FALSE, &rc);

	if (ListView_IsIconView(plv))
	{
		// Icon水平居中
		rc.left += ((plv->cxIconSpacing - plv->cxIcon) / 2);
		rc.top += g_cyIconOffset;
	}

	if (rc.left != pitem->pt.x || rc.top != pitem->pt.y)
	{
		LV_AdjustViewRectOnMove(plv, pitem, rc.left, rc.top);

		return TRUE;
	}
	return FALSE;
}

void ListView_GetViewRect2(SListViewData *plv, RECT *prcView, int cx, int cy)
{

	if (plv->rcView.left == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	*prcView = plv->rcView;

	//
	// Offsets for scrolling.
	//
	OffsetRect(prcView, -plv->ptOrigin.x, -plv->ptOrigin.y);

	if (ListView_IsIconView(plv) || ListView_IsSmallView(plv))
	{
		//  don't do that funky half-re-origining thing.

		RECT rc;

		rc.left = 0;
		rc.top = 0;
		rc.right = cx;
		rc.bottom = cy;
		UnionRect(prcView, prcView, &rc);

#if 0
		// if we're scrolled way in the positive area (plv->ptOrigin > 0), make sure we
		// include our true origin
		if ((prcView->left > -plv->ptOrigin.x))
		{
			prcView->left = -plv->ptOrigin.x;
		}
		if ((prcView->top > -plv->ptOrigin.y))
		{
			prcView->top = -plv->ptOrigin.y;
		}

		// if we're scrolled way in the positive area (plv->ptOrigin > 0),
		// make sure our scrollbars include our current position
		if ((prcView->right < (plv->sizeClient.cx)))
		{
			prcView->right = plv->sizeClient.cx;
		}
		if ((prcView->bottom < (plv->sizeClient.cy)))
		{
			prcView->bottom = plv->sizeClient.cy;
		}
#endif
	}
}

// prcViewRect used only if fSubScroll is TRUE
DWORD ListView_GetClientRect(SListViewData *plv, RECT *prcClient, BOOL fSubScroll, RECT *prcViewRect)
{
	RECT rcClient;
	RECT rcView;
	DWORD style;

#if 1
	// do this instead of the #else below because
	// in new versus old apps, you may need to add in g_c?Border because of
	// the one pixel overlap...
	GetWindowRect(plv->hwnd, &rcClient);
	if (GetExStyle(plv->hwnd) & (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE))
	{
		rcClient.right -= 2 * GetBorderRect(plv->hwnd).left;
		rcClient.bottom -= 2 * GetBorderRect(plv->hwnd).bottom;
	}
	rcClient.right -= rcClient.left;
	rcClient.bottom -= rcClient.top;
	if (rcClient.right < 0)
	{
		rcClient.right = 0;
	}
	if (rcClient.bottom < 0)
	{
		rcClient.bottom = 0;
	}
	rcClient.top = rcClient.left = 0;
#else
	style = ListView_GetWindowStyle(plv);
	GetClientRect(plv->hwnd, &rcClient);
	if (style & WS_VSCROLL)
	{
		rcClient.right += ListView_GetCxScrollbar(plv);
	}
	if (style & WS_HSCROLL)
	{
		rcClient.bottom += ListView_GetCyScrollbar(plv);
	}
#endif

	style = 0L;
	if (fSubScroll)
	{
		ListView_GetViewRect2(plv, &rcView, 0, 0);
		if ((rcClient.left < rcClient.right) && (rcClient.top < rcClient.bottom))
		{
			do
			{
				if (!(style & WS_HSCROLL) &&
					(rcView.left < rcClient.left || rcView.right > rcClient.right))
				{
					style |= WS_HSCROLL;
					rcClient.bottom -= ListView_GetCyScrollbar(plv); // BUGBUG what if not SB yet?
				}
				if (!(style & WS_VSCROLL) &&
					(rcView.top < rcClient.top || rcView.bottom > rcClient.bottom))
				{
					style |= WS_VSCROLL;
					rcClient.right -= ListView_GetCxScrollbar(plv);
				}
			}
			while (!(style & WS_HSCROLL) && rcView.right > rcClient.right);
		}
		if (prcViewRect)
		{
			*prcViewRect = rcView;
		}
	}
	*prcClient = rcClient;
	return style;
}

int CALLBACK ArrangeIconCompare(LISTITEM *pitem1, LISTITEM *pitem2, LPARAM lParam)
{
	int v1, v2;

	// REVIEW: lParam can be 0 and we fault ... bug in caller, but we might want to be robust here.

	if (HIWORD(lParam))
	{
		// Vertical arrange
		v1 = pitem1->pt.x / GET_X_LPARAM(lParam);
		v2 = pitem2->pt.x / GET_X_LPARAM(lParam);

		if (v1 > v2)
		{
			return 1;
		}
		else if (v1 < v2)
		{
			return -1;
		}
		else
		{
			int y1 = pitem1->pt.y;
			int y2 = pitem2->pt.y;

			if (y1 > y2)
			{
				return 1;
			}
			else if (y1 < y2)
			{
				return -1;
			}
		}

	}
	else
	{
		v1 = pitem1->pt.y / (int)lParam;
		v2 = pitem2->pt.y / (int)lParam;

		if (v1 > v2)
		{
			return 1;
		}
		else if (v1 < v2)
		{
			return -1;
		}
		else
		{
			int x1 = pitem1->pt.x;
			int x2 = pitem2->pt.x;

			if (x1 > x2)
			{
				return 1;
			}
			else if (x1 < x2)
			{
				return -1;
			}
		}
	}
	return 0;
}

void PASCAL _ListView_GetRectsFromItem(SListViewData *plv, BOOL bSmallIconView,
	LISTITEM *pitem,
	LPRECT prcIcon, LPRECT prcLabel, LPRECT prcBounds, LPRECT prcSelectBounds)
{
	RECT rcIcon;
	RECT rcLabel;

	if (!prcIcon)
	{
		prcIcon = &rcIcon;
	}
	if (!prcLabel)
	{
		prcLabel = &rcLabel;
	}

	if (NULL != pitem)
	{
		// This routine is called during ListView_Recompute(), while
		// plv->rcView.left may still be == RECOMPUTE.  So, we can't
		// test that to see if recomputation is needed.
		//
		if (pitem->pt.y == RECOMPUTE || pitem->cyFoldedLabel == SRECOMPUTE)
		{
			ListView_Recompute(plv);
		}

		if (bSmallIconView)
		{
			ListView_SGetRects(plv, pitem, prcIcon, prcLabel, prcBounds);
		}
		else
			// ListView_IconViewGetRects already refolds as necessary
		{
			ListView_IconViewGetRects(plv, pitem, prcIcon, prcLabel, prcBounds);
		}

		if (prcBounds)
		{
			UnionRect(prcBounds, prcIcon, prcLabel);
			if (plv->himlState && (LV_StateImageValue(pitem)))
			{
				prcBounds->left -= plv->cxState;
			}
		}

	}
	else
	{
		SetRectEmpty(prcIcon);
		*prcLabel = *prcIcon;
		if (prcBounds)
		{
			*prcBounds = *prcIcon;
		}
	}

	if (prcSelectBounds)
	{
		UnionRect(prcSelectBounds, prcIcon, prcLabel);
	}
}

void _ListView_InvalidateItemPtr(SListViewData *plv, BOOL bSmallIcon, LISTITEM *pitem, UINT fRedraw)
{
	RECT rcBounds;

	_ASSERT(!ListView_IsOwnerData(plv));

	_ListView_GetRectsFromItem(plv, bSmallIcon, pitem, NULL, NULL, &rcBounds, NULL);
	RedrawWindow(GetHostWnd(plv->hwnd), &rcBounds, NULL, fRedraw);
}

// return TRUE if things still overlap
// this only happens if we tried to unstack things, and there was NOSCROLL set and
// items tried to go off the deep end
BOOL PASCAL ListView_IUnstackOverlaps(SListViewData *plv, HDPA hdpaSort, int iDirection)
{
	BOOL fRet = FALSE;
	int i;
	int iCount;
	BOOL bSmallIconView;
	RECT rcItem, rcItem2, rcTemp;
	int cxItem, cyItem;
	LISTITEM *pitem;
	LISTITEM *pitem2;

	_ASSERT(!ListView_IsOwnerData(plv));

	if (bSmallIconView = ListView_IsSmallView(plv))
	{
		cxItem = plv->cxItem;
		cyItem = plv->cyItem;
	}
	else
	{
		cxItem = plv->cxIconSpacing;
		cyItem = plv->cyIconSpacing;
	}
	iCount = ListView_Count(plv);

	// finally, unstack any overlaps
	for (i = 0; i < iCount; i++)
	{
		int j;
		pitem = (LISTITEM *)IUI_DPA_GetPtr(hdpaSort, i);

		if (bSmallIconView)
		{
			_ListView_GetRectsFromItem(plv, bSmallIconView, pitem, NULL, NULL, &rcItem, NULL);
		}

		// move all the items that overlap with us
		for (j = i + 1; j < iCount; j++)
		{
			POINT ptOldPos;

			pitem2 = (LISTITEM *)IUI_DPA_GetPtr(hdpaSort, j);
			ptOldPos = pitem2->pt;

			if (bSmallIconView)
			{

				// for small icons, we need to do an intersect rect
				_ListView_GetRectsFromItem(plv, bSmallIconView, pitem2, NULL, NULL, &rcItem2, NULL);

				if (IntersectRect(&rcTemp, &rcItem, &rcItem2))
				{
					// yes, it intersects.  move it out
					_ListView_InvalidateItemPtr(plv, bSmallIconView, pitem2, RDW_INVALIDATE | RDW_ERASE);
					do
					{
						pitem2->pt.x += (cxItem * iDirection);
					}
					while (PtInRect(&rcItem, pitem2->pt));
				}
				else
				{
					// no more intersect!
					break;
				}

			}
			else
			{
				// for large icons, just find the ones that share the x,y;
				if (pitem2->pt.x == pitem->pt.x && pitem2->pt.y == pitem->pt.y)
				{

					_ListView_InvalidateItemPtr(plv, bSmallIconView, pitem2, RDW_INVALIDATE | RDW_ERASE);
					pitem2->pt.x += (cxItem * iDirection);
				}
				else
				{
					// no more intersect!
					break;
				}
			}

			if (plv->hwnd->ci.style & LVS_NOSCROLL)
			{
				if (pitem2->pt.x < 0 || pitem2->pt.y < 0 ||
					pitem2->pt.x > (plv->sizeClient.cx - (cxItem / 2)) ||
					pitem2->pt.y > (plv->sizeClient.cy - (cyItem / 2)))
				{
					pitem2->pt = ptOldPos;
					fRet = TRUE;
				}
			}

			// invalidate the new position as well
			_ListView_InvalidateItemPtr(plv, bSmallIconView, pitem2, RDW_INVALIDATE | RDW_ERASE);
		}
	}
	return fRet;
}


BOOL PASCAL ListView_SnapToGrid(SListViewData *plv, HDPA hdpaSort)
{
	// this algorithm can't fit in the structure of the other
	// arrange loop without becoming n^2 or worse.
	// this algorithm is order n.

	// iterate through and snap to the nearest grid.
	// iterate through and push aside overlaps.

	int i;
	int iCount;
	LPARAM  xySpacing;
	int x, y;
	LISTITEM *pitem;
	BOOL bSmallIconView;
	int cxItem, cyItem;

	_ASSERT(!ListView_IsOwnerData(plv));

	if (bSmallIconView = ListView_IsSmallView(plv))
	{
		cxItem = plv->cxItem;
		cyItem = plv->cyItem;
	}
	else
	{
		cxItem = plv->cxIconSpacing;
		cyItem = plv->cyIconSpacing;
	}


	iCount = ListView_Count(plv);

	// first snap to nearest grid
	for (i = 0; i < iCount; i++)
	{
		pitem = (LISTITEM *)IUI_DPA_GetPtr(hdpaSort, i);

		x = pitem->pt.x;
		y = pitem->pt.y;

		if (!bSmallIconView)
		{
			x -= ((plv->cxIconSpacing - plv->cxIcon) / 2);
			y -= 16;
		}

		NearestSlot(&x, &y, cxItem, cyItem, (plv->nWorkAreas > 0) ? & (plv->prcWorkAreas[pitem->iWorkArea]) : NULL);
		if (!bSmallIconView)
		{
			x += ((plv->cxIconSpacing - plv->cxIcon) / 2);
			y += 16;
		}

		if (x != pitem->pt.x || y != pitem->pt.y)
		{
			_ListView_InvalidateItemPtr(plv, bSmallIconView, pitem, RDW_INVALIDATE | RDW_ERASE);
			if ((plv->hwnd->ci.style & LVS_NOSCROLL) && (plv->nWorkAreas == 0))
			{

				// if it's marked noscroll, make sure it's still on the client region
				while (x >= (plv->sizeClient.cx - (cxItem / 2)))
				{
					x -= cxItem;
				}

				while (x < 0)
				{
					x += cxItem;
				}

				while (y >= (plv->sizeClient.cy - (cyItem / 2)))
				{
					y -= cyItem;
				}

				while (y < 0)
				{
					y += cyItem;
				}
			}
			pitem->pt.x = x;
			pitem->pt.y = y;
			plv->iFreeSlot = -1; // The "free slot" cache is no good once an item moves

			_ListView_InvalidateItemPtr(plv, bSmallIconView, pitem, RDW_INVALIDATE | RDW_ERASE);
		}
	}

	// now resort the dpa
	switch (plv->hwnd->ci.style & LVS_ALIGNMASK)
	{
	case LVS_ALIGNLEFT:
	case LVS_ALIGNRIGHT:
		xySpacing = MAKELONG(bSmallIconView ? plv->cxItem : plv->cxIconSpacing, TRUE);
		break;
	default:
		xySpacing = MAKELONG(bSmallIconView ? plv->cyItem : plv->cyIconSpacing, FALSE);
	}

	if (!IUI_DPA_Sort(hdpaSort, (PFNDACOMPARE)ArrangeIconCompare, xySpacing))
	{
		return FALSE;
	}


	// go in one direction, if there are still overlaps, go in the other
	// direction as well
	if (ListView_IUnstackOverlaps(plv, hdpaSort, 1))
	{
		ListView_IUnstackOverlaps(plv, hdpaSort, -1);
	}
	return FALSE;
}


BOOL ListView_OnArrange(SListViewData *plv, UINT style)
{
	BOOL bSmallIconView;
	LPARAM  xySpacing;
	HDPA hdpaSort = NULL;

	bSmallIconView = ListView_IsSmallView(plv);

	if (!bSmallIconView && !ListView_IsIconView(plv))
	{
		return FALSE;
	}

	if (ListView_IsOwnerData(plv))
	{
		if (style & (LVA_SNAPTOGRID | LVA_SORTASCENDING | LVA_SORTDESCENDING))
		{
			RIPMSG(0, "LVM_ARRANGE: Cannot combine LVA_SNAPTOGRID or LVA_SORTxxx with owner-data");
			return (FALSE);
		}
	}

	// Make sure our items have positions and their text rectangles
	// caluculated
	if (plv->rcView.left == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	if (!ListView_IsOwnerData(plv))
	{
		// we clone plv->hdpa so we don't blow away indices that
		// apps have saved away.
		// we sort here to make the nested for loop below more bearable.
		hdpaSort = IUI_DPA_Clone(plv->hdpa, NULL);

		if (!hdpaSort)
		{
			return FALSE;
		}
	}
	switch (plv->hwnd->ci.style & LVS_ALIGNMASK)
	{
	case LVS_ALIGNLEFT:
	case LVS_ALIGNRIGHT:
		xySpacing = MAKELONG(bSmallIconView ? plv->cxItem : plv->cxIconSpacing, TRUE);
		break;
	default:
		xySpacing = MAKELONG(bSmallIconView ? plv->cyItem : plv->cyIconSpacing, FALSE);
	}

	if (ListView_IsOwnerData(plv))
	{
		ListView_CommonArrange(plv, style, NULL);
	}
	else
	{
		if (!IUI_DPA_Sort(hdpaSort, (PFNDACOMPARE)ArrangeIconCompare, xySpacing))
		{
			return FALSE;
		}

		ListView_CommonArrange(plv, style, hdpaSort);

		IUI_DPA_Destroy(hdpaSort);
	}

	MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(plv->hwnd), OBJID_CLIENT, 0);

	return TRUE;
}

// Arrange the icons given a sorted hdpa, and arrange them in the sub workareas
BOOL ListView_CommonArrangeEx(SListViewData *plv, UINT style, HDPA hdpaSort, int iWorkArea)
{
	int iSlot;
	int iItem;
	int cSlots;
	int cWorkAreaSlots[LV_MAX_WORKAREAS];
	BOOL fItemMoved;
	RECT rcLastItem;
	RECT rcT;
	int  xMin = 0;

	BOOL bSmallIconView = ListView_IsSmallView(plv);
	BOOL bIconView = ListView_IsIconView(plv);

	//
	//  when this is an autoarrange, then we dont need to worry about
	//  scrolling the origin, because we are going to arrange everything
	//  around the positive side of the origin
	//
	if (LVA_DEFAULT == style && (plv->hwnd->ci.style & LVS_AUTOARRANGE))
	{
		if (plv->ptOrigin.x < 0)
		{
			plv->ptOrigin.x = 0;
		}
		if (plv->ptOrigin.y < 0)
		{
			plv->ptOrigin.y = 0;
		}
	}

	// REVIEW, this causes a repaint if we are scrollled
	// we can probably avoid this some how

	fItemMoved = (plv->ptOrigin.x != 0) || (plv->ptOrigin.y != 0);

	if (!ListView_IsOwnerData(plv))
	{
		if (style == LVA_SNAPTOGRID)
		{
			// (dli) This function is fitting all the icons into just one rectangle,
			// namely sizeClient. We need to make it multi-workarea aware if we want
			// multi-workarea for the general case (i.e. other than just the desktop)
			// This is never called in the desktop case.
			fItemMoved |= ListView_SnapToGrid(plv, hdpaSort);

		}
		else
		{
			if (plv->nWorkAreas > 0)
			{
				int i;
				for (i = 0; i < plv->nWorkAreas; i++)
				{
					cWorkAreaSlots[i] = ListView_GetSlotCountEx(plv, TRUE, i);
				}
			}
			else
			{
				cSlots = ListView_GetSlotCount(plv, TRUE);
			}


			SetRectEmpty(&rcLastItem);

			// manipulate only the sorted version of the item list below!

			iSlot = 0;
			RECT rcSlot; // Item所在单元格的坐标
			for (iItem = 0; iItem < ListView_Count(plv); iItem++)
			{
				LISTITEM *pitem = (LISTITEM *)IUI_DPA_GetPtr(hdpaSort, iItem);
				// (dli) In the multi-workarea case, if this item is not in our
				// workarea, skip it.
				if (pitem->iWorkArea != iWorkArea)
				{
					continue;
				}

				int cRealSlots = (plv->nWorkAreas > 0) ? cWorkAreaSlots[pitem->iWorkArea] : cSlots;

				if (bSmallIconView || bIconView)
				{
					for (; ;)
					{
						_CalcSlotRect(plv, pitem, iSlot, cRealSlots, FALSE, &rcSlot);
						if (!IntersectRect(&rcT, &rcSlot, &rcLastItem))
						{
							break;
						}
						iSlot++;
					}
				}

				fItemMoved |= ListView_SetIconPos(plv, pitem, iSlot++, cRealSlots);

				// do this instead of ListView_GetRects() because we need
				// to use the pitem from the sorted hdpa, not the ones in *plv
				RECT rcIcon, rcLabel;
				_ListView_GetRectsFromItem(plv, bSmallIconView, pitem, &rcIcon, &rcLabel, &rcLastItem, NULL);
				// f-n above will return unfolded rects if there are any, we must make sure
				// we use folded ones for slot allocations
				if (bIconView)
				{
					if (ListView_IsItemUnfoldedPtr(plv, pitem))
					{
						ListView_RefoldLabelRect(plv, &rcLabel, pitem);
						UnionRect(&rcLastItem, &rcIcon, &rcLabel);
						if (plv->himlState && (LV_StateImageValue(pitem)))
						{
							rcLastItem.left -= plv->cxState;
						}
					}
				}
				//
				// Keep track of the minimum x as we don't want negative values
				// when we finish.
				if (rcLastItem.left < xMin)
				{
					xMin = rcLastItem.left;
				}
			}

			//
			// See if we need to scroll the items over to make sure that all of the
			// no items are hanging off the left hand side.
			//
			if (xMin < 0)
			{
				for (iItem = 0; iItem < ListView_Count(plv); iItem++)
				{
					LISTITEM *pitem = ListView_FastGetItemPtr(plv, iItem);
					pitem->pt.x -= xMin;        // scroll them over
				}
				plv->rcView.left = RECOMPUTE;   // need to recompute.
				fItemMoved = TRUE;
			}
		}
	}
	//
	// We might as well invalidate the entire window to make sure...
	if (fItemMoved)
	{
		if (ListView_RedrawEnabled(plv))
		{
			RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
		else
		{
			ListView_DeleteHrgnInval(plv);
			plv->hrgnInval = (HRGN)ENTIRE_REGION;
			plv->flags |= LVF_ERASE;
		}

		// ensure important items are visible
		iItem = (plv->iFocus >= 0) ? plv->iFocus : ListView_OnGetNextItem(plv, -1, LVNI_SELECTED);

		if (iItem >= 0)
		{
			ListView_OnEnsureVisible(plv, iItem, FALSE);
		}

		if (ListView_RedrawEnabled(plv))
		{
			ListView_UpdateScrollBars(plv);
		}
	}
	return TRUE;
}


// this arranges the icon given a sorted hdpa.
// Arrange the workareas one by one in the multi-workarea case.
BOOL ListView_CommonArrange(SListViewData *plv, UINT style, HDPA hdpaSort)
{
	if (plv->nWorkAreas < 1)
	{
		if (plv->exStyle & LVS_EX_MULTIWORKAREAS)
		{
			return TRUE;
		}
		else
		{
			return ListView_CommonArrangeEx(plv, style, hdpaSort, 0);
		}
	}
	else
	{
		int i;
		for (i = 0; i < plv->nWorkAreas; i++)
		{
			ListView_CommonArrangeEx(plv, style, hdpaSort, i);
		}
		return TRUE;
	}
}

void ListView_IUpdateScrollBars(SListViewData *plv)
{
	RECT rcClient;
	RECT rcView;
	DWORD style;
	DWORD styleOld;
	SCROLLINFO si;
	int ixDelta = 0, iyDelta = 0;
	int iNewPos;
	BOOL fReupdate = FALSE;

	styleOld = ListView_GetWindowStyle(plv);
	style = ListView_GetClientRect(plv, &rcClient, TRUE, &rcView);

	// Grow scrolling rect to origin if necessary.
	if (rcView.left > 0)
	{
		rcView.left = 0;
	}
	if (rcView.top > 0)
	{
		rcView.top = 0;
	}

	//TraceMsg(TF_LISTVIEW, "ListView_GetClientRect %x %x %x %x", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
	//TraceMsg(TF_LISTVIEW, "ListView_GetViewRect2 %x %x %x %x", rcView.left, rcView.top, rcView.right, rcView.bottom);
	//TraceMsg(TF_LISTVIEW, "rcView %x %x %x %x", plv->rcView.left, plv->rcView.top, plv->rcView.right, plv->rcView.bottom);
	//TraceMsg(TF_LISTVIEW, "Origin %x %x", plv->ptOrigin.x, plv->ptOrigin.y);

	si.cbSize = sizeof(SCROLLINFO);

	if (style & WS_HSCROLL)
	{
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = rcView.right - rcView.left - 1;
		//TraceMsg(TF_LISTVIEW, "si.nMax rcView.right - rcView.left - 1 %x", si.nMax);

		si.nPage = rcClient.right - rcClient.left;
		//TraceMsg(TF_LISTVIEW, "si.nPage %x", si.nPage);

		si.nPos = rcClient.left - rcView.left;
		if (si.nPos < 0)
		{
			// with the new rcView calculations, I don't think
			// rcView.left is ever larger than rcClient.left.  msq
			_ASSERT(0);
			si.nPos = 0;
		}
		//TraceMsg(TF_LISTVIEW, "si.nPos %x", si.nPos);

		ListView_SetScrollInfo(plv, SB_HORZ, &si, TRUE);

		// make sure our position and page doesn't hang over max
		if ((si.nPos + (LONG)si.nPage - 1 > si.nMax) && si.nPos > 0)
		{
			iNewPos = (int)si.nMax - (int)si.nPage + 1;
			if (iNewPos < 0)
			{
				iNewPos = 0;
			}
			if (iNewPos != si.nPos)
			{
				ixDelta = iNewPos - (int)si.nPos;
				fReupdate = TRUE;
			}
		}

	}
	else if (styleOld & WS_HSCROLL)
	{
		ListView_SetScrollRange(plv, SB_HORZ, 0, 0, TRUE);
	}

	if (style & WS_VSCROLL)
	{
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = rcView.bottom - rcView.top - 1;

		si.nPage = rcClient.bottom - rcClient.top;

		si.nPos = rcClient.top - rcView.top;
		if (si.nPos < 0)
		{
			// with the new rcView calculations, I don't think
			// rcView.top is ever larger than rcClient.top.  msq
			_ASSERT(0);
			si.nPos = 0;
		}

		ListView_SetScrollInfo(plv, SB_VERT, &si, TRUE);

		// make sure our position and page doesn't hang over max
		if ((si.nPos + (LONG)si.nPage - 1 > si.nMax) && si.nPos > 0)
		{
			iNewPos = (int)si.nMax - (int)si.nPage + 1;
			if (iNewPos < 0)
			{
				iNewPos = 0;
			}
			if (iNewPos != si.nPos)
			{
				iyDelta = iNewPos - (int)si.nPos;
				fReupdate = TRUE;
			}
		}
	}
	else if (styleOld & WS_VSCROLL)
	{
		ListView_SetScrollRange(plv, SB_VERT, 0, 0, TRUE);
	}

	if (fReupdate)
	{
		// we shouldn't recurse because the second time through, si.nPos >0
		ListView_IScroll2(plv, ixDelta, iyDelta, 0);
		ListView_IUpdateScrollBars(plv);
		TraceMsg(TF_WARNING, "LISTVIEW: ERROR: We had to recurse!");
	}

#if 0
	if ((styleOld ^ style) & (WS_HSCROLL | WS_VSCROLL))
	{
		SetWindowPos(GetHostWnd(plv->hwnd), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
#endif

}

// WM_*SCROLL的间接响应函数
// cLine: SB_LINEUP或SB_LINEDOWN时的步进
// cPage: SB_PAGEUP或SB_PAGEDOWN时的步进
void ListView_ComOnScroll(SListViewData *plv, UINT code, int posNew, int sb,
	int cLine, int cPage)
{
	int pos;
	SCROLLINFO si;
	BOOL fVert = (sb == SB_VERT);
	UINT uSmooth = 0;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

	if (!ListView_GetScrollInfo(plv, sb, &si))
	{
		return;
	}

	if (cPage != -1)
	{
		si.nPage = cPage;
	}

	// 因为ListView_ComOnScroll并不会改变滚动范围，所以si.nMax并不会被设置到滚动条上
	// 所以，这里借用一下si.nMax变量，测试一下，够不够滚动一屏（Page up）
	si.nMax -= (si.nPage - 1);

	if (si.nMax < si.nMin)
	{
		si.nMax = si.nMin;
	}

	pos = si.nPos; // current position

	switch (code)
	{
	case SB_LEFT:
		si.nPos = si.nMin;
		break;
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_LINELEFT:
		si.nPos -= cLine;
		break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;
	case SB_LINERIGHT:
		si.nPos += cLine;
		break;

	case SB_THUMBTRACK:
		si.nPos = posNew;
		uSmooth = SSW_EX_IMMEDIATE;
		break;

	case SB_ENDSCROLL:
		// When scroll bar tracking is over, ensure scroll bars
		// are properly updated...
		//
		ListView_UpdateScrollBars(plv);
		return;

	default:
		return;
	}

	// 更新ListView的滚动信息
	si.fMask = SIF_POS;
	si.nPos = ListView_SetScrollInfo(plv, sb, &si, TRUE);

	if (pos != si.nPos)
	{
		int delta = (int)si.nPos - pos;
		int dx = 0, dy = 0;
		if (fVert)
		{
			dy = delta;
		}
		else
		{
			dx = delta;
		}

		// 更新ptlRptOrigin。 通过更新ptlRptOrigin，来滚动视图
		_ListView_Scroll2(plv, dx, dy, uSmooth);
	}
}

//
//  We need a smoothscroll callback so our background image draws
//  at the correct origin.  If we don't have a background image,
//  then this work is superfluous but not harmful either.
//
int CALLBACK ListView_IScroll2_SmoothScroll(
	SListViewData *plv,
	int dx,
	int dy,
	CONST RECT *prcScroll,
	CONST RECT *prcClip,
	HRGN hrgnUpdate,
	LPRECT prcUpdate,
	UINT flags)
{
	if (plv)
	{
		plv->ptOrigin.x -= dx;
		plv->ptOrigin.y -= dy;
	}

	// Now do what SmoothScrollWindow would've done if we weren't
	// a callback

	return SIMPLEREGION;//L ScrollWindowEx(hwnd, dx, dy, prcScroll, prcClip, hrgnUpdate, prcUpdate, flags);
}



void ListView_IScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth)
{
	if (dx | dy)
	{
		if ((plv->clrBk == CLR_NONE) && (plv->pImgCtx == NULL))
		{
			plv->ptOrigin.x += dx;
			plv->ptOrigin.y += dy;
			LVSeeThruScroll(plv, NULL);
		}
		else
		{
			SMOOTHSCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SSIF_SCROLLPROC;
			si.hWLWnd = plv->hwnd;
			si.dx = -dx;
			si.dy = -dy;
			si.lprcSrc = NULL;
			si.lprcClip = NULL;
			si.hrgnUpdate = NULL;
			si.lprcUpdate = NULL;
			si.fuScroll = uSmooth | SW_INVALIDATE | SW_ERASE;
			si.pfnScrollProc = ListView_IScroll2_SmoothScroll;
			SmoothScrollWindow(&si);
		}
	}
}

void ListView_IOnScroll(SListViewData *plv, UINT code, int posNew, UINT sb)
{
	int cLine;

	if (sb == SB_VERT)
	{
		cLine = plv->cyIconSpacing / 2;
	}
	else
	{
		cLine = plv->cxIconSpacing / 2;
	}

	ListView_ComOnScroll(plv, code, posNew, sb,
		cLine, -1);

}

int ListView_IGetScrollUnitsPerLine(SListViewData *plv, UINT sb)
{
	int cLine;

	if (sb == SB_VERT)
	{
		cLine = plv->cyIconSpacing / 2;
	}
	else
	{
		cLine = plv->cxIconSpacing / 2;
	}

	return cLine;
}

// NOTE: there is very similar code in the treeview
//
// Totally disgusting hack in order to catch VK_RETURN
// before edit control gets it.
//
LRESULT CALLBACK ListView_EditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWLWND hBindListView = (HWLWND)GetProp(hwnd, _T("GETPROP_EDITPARENT_LISTVIEW"));
	SListViewData *plv = (SListViewData *)GetMember(hBindListView);
	LRESULT lret;

	_ASSERT(plv);

#if defined(FE_IME) || !defined(WINNT)
	if ((IUIGetGloabelVar()->g_fDBCSInputEnabled) && LOWORD(GetKeyboardLayout(0L)) == 0x0411)
	{
		// The following code adds IME awareness to the
		// listview's label editing. Currently just for Japanese.
		//
		DWORD dwGcs;

		if (msg == WM_SIZE)
		{
			// If it's given the size, tell it to an IME.

			ListView_SizeIME(hwnd);
		}
		else if (msg == EM_SETLIMITTEXT)
		{
			if (wParam < 13)
			{
				plv->flags |= LVF_DONTDRAWCOMP;
			}
			else
			{
				plv->flags &= ~LVF_DONTDRAWCOMP;
			}
		}
		// Give up to draw IME composition by ourselves in case
		// we're working on SFN. Win95d-5709
		else if (!(plv->flags & LVF_DONTDRAWCOMP))
		{
			switch (msg)
			{

			case WM_IME_STARTCOMPOSITION:
			case WM_IME_ENDCOMPOSITION:
				return 0L;


			case WM_IME_COMPOSITION:

				// If lParam has no data available bit, it implies
				// canceling composition.
				// ListView_InsertComposition() tries to get composition
				// string w/ GCS_COMPSTR then remove it from edit control if
				// nothing is available.
				//
				if (!lParam)
				{
					dwGcs = GCS_COMPSTR;
				}
				else
				{
					dwGcs = (DWORD)lParam;
				}

				ListView_InsertComposition(hwnd, wParam, dwGcs, plv);
				return 0L;

			case WM_PAINT:
				lret = CallWindowProc(plv->pfnEditWndProc, hwnd, msg, wParam, lParam);
				ListView_PaintComposition(hwnd, plv);
				return lret;

			case WM_IME_SETCONTEXT:

				// We draw composition string.
				//
				lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
				break;

			default:
				// the other messages should simply be processed
				// in this subclass procedure.
				break;
			}
		}
	}
#endif FE_IME

	switch (msg)
	{
	case WM_SETTEXT:
		SetWindowID(hwnd, 1);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			ListView_DismissEdit(plv, FALSE);
			return 0L;

		case VK_ESCAPE:
			ListView_DismissEdit(plv, TRUE);
			return 0L;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case VK_RETURN:
			// Eat the character, so edit control wont beep!
			return 0L;
		}
		break;

	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS | DLGC_HASSETSEL;        /* editing name, no dialog handling right now */
	}

	return CallWindowProc(plv->pfnEditWndProc, hwnd, msg, wParam, lParam);
}

//  Helper routine for SetEditSize
void ListView_ChangeEditRectForRegion(SListViewData *plv, LPRECT lprc)
{
	LISTITEM *pitem = ListView_GetItemPtr(plv, plv->iEdit);

	_ASSERT(!ListView_IsOwnerData(plv));
	_ASSERT(ListView_IsIconView(plv));

	if (!EqualRect((CONST RECT *)&pitem->rcTextRgn, (CONST RECT *)lprc))
	{
		// RecalcRegion knows to use rcTextRgn in the case where iEdit != -1,
		// so set it up before calling through.
		CopyRect(&pitem->rcTextRgn, (CONST RECT *)lprc);
		ListView_RecalcRegion(plv, TRUE, TRUE);

		// Invalidate the entire Edit and force a repaint from the listview
		// on down to make sure we don't leave turds...
		InvalidateRect(plv->hwndEdit, NULL, TRUE);
		UpdateWindow(GetHostWnd(plv->hwnd));
	}
}

// BUGBUG: very similar routine in treeview

void ListView_SetEditSize(SListViewData *plv)
{
	RECT rcLabel;
	UINT seips;

	if (!((plv->iEdit >= 0) && (plv->iEdit < ListView_Count(plv))))
	{
		ListView_DismissEdit(plv, TRUE);    // cancel edits
		return;
	}

	ListView_GetRects(plv, plv->iEdit, NULL, &rcLabel, NULL, NULL);
	if (plv->iSubItemEditLabel > 0 && ListView_IsReportView(plv))
	{
		CIUIRect rc;
		if (!ColumnHeader_GetItemRect(plv->hwndHdr, plv->iSubItemEditLabel, &rc))
		{
			return;
		}

		OffsetRect(&rc, -plv->ptlRptOrigin.x, 0);
		rcLabel.left = rc.left;
		rcLabel.right = rc.right;
	}

	// OffsetRect(&rc, rcLabel.left + g_cxLabelMargin + g_cxBorder,
	//         (rcLabel.bottom + rcLabel.top - rc.bottom) / 2 + g_cyBorder);
	// OffsetRect(&rc, rcLabel.left + g_cxLabelMargin , rcLabel.top);

	// get the text bounding rect

	if (ListView_IsIconView(plv))
	{
		// We should not adjust y-positoin in case of the icon view.
		InflateRect(&rcLabel, -IUIGetGloabelVar()->g_cxLabelMargin, -IUIGetGloabelVar()->g_cyBorder);
	}
	else
	{
		// Special case for single-line & centered
		InflateRect(&rcLabel, -IUIGetGloabelVar()->g_cxLabelMargin - IUIGetGloabelVar()->g_cxBorder, (-(rcLabel.bottom - rcLabel.top - plv->cyLabelChar) / 2) - IUIGetGloabelVar()->g_cyBorder);
	}

	seips = 0;
	if (ListView_IsIconView(plv) && !(plv->hwnd->ci.style & LVS_NOLABELWRAP))
	{
		seips |= SEIPS_WRAP;
	}
#ifdef DEBUG
	if (plv->hwnd->ci.style & LVS_NOSCROLL)
	{
		seips |= SEIPS_NOSCROLL;
	}
#endif

	SetEditInPlaceSize(plv->hwndEdit, &rcLabel, plv->hfontLabel, seips);

	if (plv->exStyle & LVS_EX_REGIONAL)
	{
		ListView_ChangeEditRectForRegion(plv, &rcLabel);
	}
}

// to avoid eating too much stack
void ListView_DoOnEditLabel(SListViewData *plv, int i, LPTSTR pszInitial)
{
	TCHAR szLabel[CCHLABELMAX];
	LVITEMEX item;

	item.mask = LVIF_TEXT;
	item.iItem = i;
	item.iSubItem = plv->iSubItemEditLabel;
	item.pszText = szLabel;
	item.cchTextMax = ARRAYSIZE(szLabel);
	ListView_OnGetItem(plv, &item);

	if (!item.pszText)
	{
		return;
	}

	// Make sure the edited item has the focus.
	if (plv->iFocus != i)
	{
		ListView_SetFocusSel(plv, i, TRUE, TRUE, FALSE);
	}

	// Make sure the item is fully visible
	ListView_OnEnsureVisible(plv, i, FALSE);        // fPartialOK == FALSE

	// Must subtract one from ARRAYSIZE(szLabel) because Edit_LimitText doesn't include
	// the terminating NULL

	plv->hwndEdit = CreateEditInPlaceWindow(GetHostWnd(plv->hwnd),
			pszInitial ? pszInitial : item.pszText, ARRAYSIZE(szLabel) - 1,
			ListView_IsIconView(plv) ?
			(WS_BORDER | WS_CLIPSIBLINGS | WS_CHILD | ES_CENTER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL) :
			(WS_BORDER | WS_CLIPSIBLINGS | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL), plv->hfontLabel);
	if (NULL != plv->hwndEdit)
	{
		::SetProp(plv->hwndEdit, _T("GETPROP_EDITPARENT_LISTVIEW"), plv->hwnd);

		LISTITEM *pitem;
		NMLVDISPINFOEX nm;

		// We create the edit window but have not shown it.  Ask the owner
		// if they are interested or not.
		// If we passed in initial text set the ID to be dirty...
		if (pszInitial)
		{
			SetWindowID(plv->hwndEdit, 1);
		}

		nm.item.mask = LVIF_PARAM;
		nm.item.iItem = i;
		nm.item.iSubItem = plv->iSubItemEditLabel;

		if (!ListView_IsOwnerData(plv))
		{
			if (!(pitem = ListView_GetItemPtr(plv, i)))
			{
				DestroyWindow(plv->hwndEdit);
				plv->hwndEdit = NULL;
				plv->iSubItemEditLabel = 0;
				return;
			}
			nm.item.lParam = pitem->lParam;
		}
		else
		{
			nm.item.lParam = (LPARAM)0;
		}


		plv->iEdit = i;

		// if they have LVS_EDITLABELS but return non-FALSE here, stop!
		if ((BOOL)CCSendNotify(&plv->hwnd->ci, LVN_BEGINLABELEDIT, &nm.hdr))
		{
			plv->iEdit = -1;
			DestroyWindow(plv->hwndEdit);
			plv->hwndEdit = NULL;
		}
	}
}

// List Host窗口过程，用来把List的Label Edit控件发送的消息，转发给List.
LRESULT CALLBACK ListView_HostWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	if (WM_COMMAND == uMsg)
	{
		HWND hSender = (HWND)lParam;
		HWLWND hBindListView = (HWLWND)GetProp(hSender, _T("GETPROP_EDITPARENT_LISTVIEW"));
		if (NULL != hBindListView)
		{
			// 从List中的Label Edit控件发来的WM_COMMAND消息转发给ListView
			SListViewData *plv = (SListViewData *)GetMember(hBindListView);

			ListView_OnCommand(plv, LOWORD(wParam), hSender, HIWORD(wParam));
		}
	}
	else if (WM_NOTIFY == uMsg)
	{
		NMHDR *pnmhdr = (NMHDR *)lParam;

		if (NULL != pnmhdr)
		{
			HWND hSender = (HWND)pnmhdr->hwndFrom;
			HWLWND hBindListView = (HWLWND)GetProp(hSender, _T("GETPROP_TOOLTIPPARENT_LISTVIEW"));
			if (NULL != hBindListView)
			{
				// 从List中的Tooltio控件发来的WM_NOTIFY消息转发给ListView.
				SListViewData *plv = (SListViewData *)GetMember(hBindListView);
				ListView_OnNotify(plv, wParam, pnmhdr);
			}
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HWND ListView_OnEditLabel(SListViewData *plv, int i, LPTSTR pszInitialText)
{

	// this eats stack
	ListView_DismissEdit(plv, FALSE);

	BOOL bEditLabels = (plv->hwnd->ci.style & LVS_EDITLABELS);
	HWND hHost = GetHostWnd(plv->hwnd);
	HWLWND hFocus = (HWLWND)::SendMessage(hHost, WM_GETWLFOCUS, 0, LPARAM(hHost));
	if (!bEditLabels || (hFocus != plv->hwnd) || (i == -1))
	{
		return (NULL);    // Does not support this.
	}

	ListView_DoOnEditLabel(plv, i, pszInitialText);

	if (NULL != plv->hwndEdit)
	{
		plv->pfnEditWndProc = SubclassWindow(plv->hwndEdit, ListView_EditWndProc);

		// 子类化list的Host，因为list中的edit控件是Host的子控件，会往Host发送消息，
		// 通过子类化，Host收到Edit控件的消息后，转发给List.
		BOOL bRet = SetWindowSubclass(GetHostWnd(plv->hwnd), ListView_HostWndProc, 1, 1);

#if defined(FE_IME) || !defined(WINNT)
		if (IUIGetGloabelVar()->g_fDBCSInputEnabled)
		{
			if (SendMessage(plv->hwndEdit, EM_GETLIMITTEXT, (WPARAM)0, (LPARAM)0) < 13)
			{
				plv->flags |= LVF_DONTDRAWCOMP;
			}

		}
#endif

		ListView_SetEditSize(plv);

		// Show the window and set focus to it.  Do this after setting the
		// size so we don't get flicker.
		SetFocus(plv->hwndEdit);
		ShowWindow(plv->hwndEdit, SW_SHOW);
		//ListView_InvalidateItem(plv, i, TRUE, RDW_INVALIDATE | RDW_ERASE);

		RescrollEditWindow(plv->hwndEdit);

		/* Due to a bizzare twist of fate, a certain mix of resolution / font size / icon
		/  spacing results in being able to see the previous label behind the edit control
		/  we have just created.  Therefore to overcome this problem we ensure that this
		/  label is erased.
		/
		/  As the label is not painted when we have an edit control we just invalidate the
		/  area and the background will be painted.  As the window is a child of the list view
		/  we should not see any flicker within it. */

		if (ListView_IsIconView(plv))
		{
			RECT rcLabel;

			ListView_GetRects(plv, i, NULL, &rcLabel, NULL, NULL);
			ListView_UnfoldRects(plv, i, NULL, &rcLabel, NULL, NULL);

			InvalidateRect(GetHostWnd(plv->hwnd), &rcLabel, TRUE);
			UpdateWindow(GetHostWnd(plv->hwnd));
		}
	}

	return plv->hwndEdit;
}


// BUGBUG: very similar code in treeview.c

BOOL ListView_DismissEdit(SListViewData *plv, BOOL fCancel)
{
	LISTITEM *pitem = NULL;
	BOOL fOkToContinue = TRUE;
	HWND hwndEdit = plv->hwndEdit;
	HWND hwnd = GetHostWnd(plv->hwnd);
	int nEditingItemIndex; // 当前编辑的Item的索引
	NMLVDISPINFOEX nm;
	TCHAR szLabel[CCHLABELMAX];
#if defined(FE_IME) || !defined(WINNT)
	HIMC himc;
#endif


	if (plv->fNoDismissEdit)
	{
		return FALSE;
	}

	if (!hwndEdit)
	{
		// Also make sure there are no pending edits...
		ListView_CancelPendingEdit(plv);
		return TRUE;    // It is OK to process as normal...
	}

	// If the window is not visible, we are probably in the process
	// of being destroyed, so assume that we are being destroyed
	if (!IsWindowVisible(GetHostWnd(plv->hwnd)))
	{
		fCancel = TRUE;
	}

	//
	// We are using the Window ID of the control as a BOOL to
	// state if it is dirty or not.
	switch (GetWindowID(hwndEdit))
	{
	case 0:
		// The edit control is not dirty so act like cancel.
		fCancel = TRUE;
	// Fall through to set window so we will not recurse!
	case 1:
		// The edit control is dirty so continue.
		SetWindowID(hwndEdit, 2);    // Don't recurse
		break;
	case 2:
		// We are in the process of processing an update now, bail out
		return TRUE;
	}

	// BUGBUG: this will fail if the program deleted the items out
	// from underneath us (while we are waiting for the edit timer).
	// make delete item invalidate our edit item
	// We uncouple the edit control and hwnd out from under this as
	// to allow code that process the LVN_ENDLABELEDIT to reenter
	// editing mode if an error happens.
	nEditingItemIndex = plv->iEdit;

	do
	{
		if (ListView_IsOwnerData(plv))
		{
			if (!((nEditingItemIndex >= 0) && (nEditingItemIndex < plv->cTotalItems)))
			{
				break;
			}
			nm.item.lParam = 0;
		}
		else
		{

			pitem = ListView_GetItemPtr(plv, nEditingItemIndex);
			_ASSERT(pitem);
			if (pitem == NULL)
			{
				break;
			}
			nm.item.lParam = pitem->lParam;
		}

		nm.item.iItem = nEditingItemIndex;
		nm.item.iSubItem = plv->iSubItemEditLabel;
		nm.item.cchTextMax = 0;
		nm.item.mask = 0;

		if (fCancel)
		{
			nm.item.pszText = NULL;
		}
		else
		{
			Edit_GetText(hwndEdit, szLabel, ARRAYSIZE(szLabel));
			nm.item.pszText = szLabel;
			nm.item.mask |= LVIF_TEXT;
			nm.item.cchTextMax = ARRAYSIZE(szLabel);
		}

		//
		// Notify the parent that we the label editing has completed.
		// We will use the NMLVDISPINFOEX structure to return the new
		// label in.  The parent still has the old text available by
		// calling the GetItemText function.
		//

		fOkToContinue = (BOOL)CCSendNotify(&plv->hwnd->ci, LVN_ENDLABELEDIT, &nm.hdr);
		if (!IsWindow(hwnd))
		{
			return FALSE;
		}
		if (fOkToContinue && !fCancel)
		{
			//
			// If the item has the text set as CALLBACK, we will let the
			// owner know that they are supposed to set the item text in
			// their own data structures.  Else we will simply update the
			// text in the actual view.
			//
			if (!ListView_IsOwnerData(plv) &&
				(pitem->pszText != LPSTR_TEXTCALLBACK))
			{
				// Set the item text (everything's set up in nm.item)
				//
				nm.item.mask = LVIF_TEXT;
				ListView_OnSetItem(plv, &nm.item);
			}
			else
			{
				CCSendNotify(&plv->hwnd->ci, LVN_SETDISPINFO, &nm.hdr);

				// Also we will assume that our cached size is invalid...
				plv->rcView.left = RECOMPUTE;
				if (!ListView_IsOwnerData(plv))
				{
					ListView_SetSRecompute(pitem);
				}
			}
		}

#if defined(FE_IME) || !defined(WINNT)
		if (IUIGetGloabelVar()->g_fDBCSInputEnabled)
		{
			if (LOWORD(GetKeyboardLayout(0L)) == 0x0411 && (himc = ImmGetContext(hwndEdit)))
			{
				ImmNotifyIME(himc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0L);
				ImmReleaseContext(hwndEdit, himc);
			}
		}
#endif

		// redraw
		ListView_InvalidateItem(plv, nEditingItemIndex, FALSE, RDW_INVALIDATE | RDW_ERASE);
	}
	while (FALSE);

	// If the hwnedit is still us clear out the variables
	if (hwndEdit == plv->hwndEdit)
	{
		plv->iEdit = -1;
		plv->hwndEdit = NULL;   // avoid being reentered
	}
	DestroyWindow(hwndEdit);

	// We've to recalc the region because the edit in place window has
	// added stuff to the region that we don't know how to remove
	// safely.
	ListView_RecalcRegion(plv, TRUE, TRUE);

	return fOkToContinue;
}

//
// This function will scall the icon positions that are stored in the
// item structures between large and small icon view.
//
void ListView_ScaleIconPositions(SListViewData *plv, BOOL fSmallIconView)
{
	int cxItem, cyItem;
	HWND hwnd;
	int i;

	if (fSmallIconView)
	{
		if (plv->flags & LVF_ICONPOSSML)
		{
			return;    // Already done
		}
	}
	else
	{
		if ((plv->flags & LVF_ICONPOSSML) == 0)
		{
			return;    // dito
		}
	}

	// Last but not least update our bit!
	plv->flags ^= LVF_ICONPOSSML;

	cxItem = plv->cxItem;
	cyItem = plv->cyItem;
	hwnd = GetHostWnd(plv->hwnd);

	// We will now loop through all of the items and update their coordinats
	// We will update th position directly into the view instead of calling
	// SetItemPosition as to not do 5000 invalidates and messages...
	if (!ListView_IsOwnerData(plv))
	{
		for (i = 0; i < ListView_Count(plv); i++)
		{
			LISTITEM *pitem = ListView_FastGetItemPtr(plv, i);

			if (pitem->pt.y != RECOMPUTE)
			{
				if (fSmallIconView)
				{
					pitem->pt.x = MulDiv(pitem->pt.x - g_cxIconOffset, cxItem, plv->cxIconSpacing);
					pitem->pt.y = MulDiv(pitem->pt.y - g_cyIconOffset, cyItem, plv->cyIconSpacing);
				}
				else
				{
					pitem->pt.x = MulDiv(pitem->pt.x, plv->cxIconSpacing, cxItem) + g_cxIconOffset;
					pitem->pt.y = MulDiv(pitem->pt.y, plv->cyIconSpacing, cyItem) + g_cyIconOffset;
				}
			}
		}

		plv->iFreeSlot = -1; // The "free slot" cache is no good once an item moves

		if (plv->hwnd->ci.style & LVS_AUTOARRANGE)
		{
			ListView_ISetColumnWidth(plv, 0,
				LV_GetNewColWidth(plv, 0, ListView_Count(plv) - 1), FALSE);
			// If autoarrange is turned on, the arrange function will do
			// everything that is needed.
			ListView_OnArrange(plv, LVA_DEFAULT);
			return;
		}
	}
	plv->rcView.left = RECOMPUTE;

	//
	// Also scale the origin
	//
	if (fSmallIconView)
	{
		plv->ptOrigin.x = MulDiv(plv->ptOrigin.x, cxItem, plv->cxIconSpacing);
		plv->ptOrigin.y = MulDiv(plv->ptOrigin.y, cyItem, plv->cyIconSpacing);
	}
	else
	{
		plv->ptOrigin.x = MulDiv(plv->ptOrigin.x, plv->cxIconSpacing, cxItem);
		plv->ptOrigin.y = MulDiv(plv->ptOrigin.y, plv->cyIconSpacing, cyItem);
	}

	// Make sure it fully redraws correctly
	RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}

// in:
//      hwndEdit        edit control to position in client coords of parent window
//      prc             bonding rect of the text, used to position everthing
//      hFont           font being used
//      flags
//          SEIPS_WRAP      if this is a wrapped type (multiline) edit
//          SEIPS_NOSCROLL  if the parent control does not have scrollbars
//
//      The SEIPS_NOSCROLL flag is used only in DEBUG.  Normally, the item
//      being edited should have been scrolled into view, but if the parent
//      doesn't have scrollbars, then clearly that's not possible, so we
//      shouldn't _ASSERT in that case.
//
// Notes:
//       The top-left corner of the bouding rectangle must be the position
//      the client uses to draw text. We adjust the edit field rectangle
//      appropriately.
//

// draw three pixel wide border for border selection.
void PASCAL ListView_DrawBorderSel(HIMAGELIST himl, HWND hwnd, HDC hdc, int x, int y, COLORREF clr)
{
	int dx, dy;
	RECT rc;
	COLORREF clrSave = SetBkColor(hdc, clr);

	ImageList_GetIconSize(himl, &dx, &dy);
	//left
	rc.left = x - 4;    // 1 pixel seperation + 3 pixel width.
	rc.top = y - 4;
	rc.right = x - 1;
	rc.bottom = y + dy + 4;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	//top
	rc.left = rc.right;
	rc.right = rc.left + dx + 2;
	rc.bottom = rc.top + 3;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	//right
	rc.left = rc.right;
	rc.right = rc.left + 3;
	rc.bottom = rc.top + dy + 8; // 2*3 pixel borders + 2*1 pixel seperation = 8
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	// bottom
	rc.top = rc.bottom - 3;
	rc.right = rc.left;
	rc.left = rc.right - dx - 2;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

	SetBkColor(hdc, clrSave);
	return;
}

//
//  If xMax >= 0, then the image will not be drawn past the x-coordinate
//  specified by xMax.  This is used only during report view drawing, where
//  we have to clip against our column width.
//
UINT PASCAL ListView_DrawImageEx(SListViewData *plv, LVITEMEX *pitem, HDC hdc, int x, int y, UINT fDraw, int xMax)
{
	UINT fText = SHDT_DESELECTED;
	UINT fImage = ILD_NORMAL;
	COLORREF clr = 0;
	HIMAGELIST himl;
	BOOL fBorderSel = (plv->exStyle & LVS_EX_BORDERSELECT);
	int cxIcon;

	fImage = (pitem->state & LVIS_OVERLAYMASK);
	fText = SHDT_DESELECTED;

	if (ListView_IsIconView(plv))
	{
		himl = plv->himl;
		cxIcon = plv->cxIcon;
	}
	else
	{
		himl = plv->himlSmall;
		cxIcon = plv->cxSmIcon;
	}

	// the item can have one of 4 states, for 3 looks:
	//    normal                    simple drawing
	//    selected, no focus        light image highlight, no text hi
	//    selected w/ focus         highlight image & text
	//    drop highlighting         highlight image & text

	if ((pitem->state & LVIS_DROPHILITED) ||
		((fDraw & LVDI_SELECTED) && (pitem->state & LVIS_SELECTED)))
	{
		fText = SHDT_SELECTED;
		if (!fBorderSel)    // do not effect color of icon on borderselect.
		{
			fImage |= ILD_BLEND50;
			clr = CLR_HILIGHT;
		}
	}

	if ((fDraw & LVDI_SELECTNOFOCUS) && (pitem->state & LVIS_SELECTED))
	{
		fText = SHDT_SELECTNOFOCUS;
		//fImage |= ILD_BLEND50;
		//clr = GetSysColor(COLOR_3DFACE);
	}

	if (pitem->state & LVIS_CUT)
	{
		fImage |= ILD_BLEND50;
		clr = plv->clrBk;
	}

#if 0   // dont do a selected but dont have the focus vis.
	else if (item.state & LVIS_SELECTED)
	{
		fImage |= ILD_BLEND25;
		clr = CLR_HILIGHT;
	}
#endif

	if (!(fDraw & LVDI_NOIMAGE))
	{
		if (himl)
		{
			COLORREF clrBk;

			if (plv->pImgCtx || ((plv->exStyle & LVS_EX_REGIONAL) && !g_fSlowMachine))
			{
				clrBk = CLR_NONE;
			}
			else
			{
				clrBk = plv->clrBk;
			}

			if (xMax >= 0)
			{
				cxIcon = min(cxIcon, xMax - x);
			}

			if (cxIcon > 0)
			{
				ImageList_DrawEx(himl, pitem->iImage, hdc, x, y, cxIcon, 0, clrBk, clr, fImage);
			}
		}

		if (plv->himlState)
		{
			if (LV_StateImageValue(pitem) &&
				(pitem->iSubItem == 0 ||
					plv->exStyle & LVS_EX_SUBITEMIMAGES)
			)
			{
				int iState = LV_StateImageIndex(pitem);
				int dyImage =
					(himl) ?
					((ListView_IsIconView(plv) ? plv->cyIcon : plv->cySmIcon) - plv->cyState)
					: 0;
				int xDraw = x - plv->cxState;
				cxIcon = plv->cxState;
				if (xMax >= 0)
				{
					cxIcon = min(cxIcon, xMax - xDraw);
				}
				if (cxIcon > 0)
					ImageList_DrawEx(plv->himlState, iState, hdc,
						xDraw,
						y + dyImage,
						cxIcon,
						0,
						CLR_DEFAULT,
						CLR_DEFAULT,
						ILD_NORMAL);
			}
		}
		// draw the border selection if appropriate.
		if (fBorderSel && !(fText & SHDT_DESELECTED))       // selected, draw the selection rect.
		{
			COLORREF clrBorder = (fDraw & LVDI_HOTSELECTED)
				? GetSysColor(COLOR_HOTLIGHT) : GetSysColor(COLOR_HIGHLIGHT);
			ListView_DrawBorderSel(himl, GetHostWnd(plv->hwnd), hdc, x, y, clrBorder);
		}
		else if (fBorderSel && (fText & SHDT_DESELECTED))   // erase possible selection rect.
		{
			ListView_DrawBorderSel(himl, GetHostWnd(plv->hwnd), hdc, x, y, plv->clrBk);
		}

	}

	return fText;
}

#if defined(FE_IME) || !defined(WINNT)
void PASCAL ListView_SizeIME(HWND hwnd)
{
	HIMC himc;
#ifdef _WIN32
	CANDIDATEFORM   candf;
#else
	CANDIDATEFORM16   candf;
#endif
	RECT rc;

	// If this subclass procedure is being called with WM_SIZE,
	// This routine sets the rectangle to an IME.

	GetClientRect(hwnd, &rc);


	// Candidate stuff
	candf.dwIndex = 0; // Bogus assumption for Japanese IME.
	candf.dwStyle = CFS_EXCLUDE;
	candf.ptCurrentPos.x = rc.left;
	candf.ptCurrentPos.y = rc.bottom;
	candf.rcArea = rc;

	if (himc = ImmGetContext(hwnd))
	{
		ImmSetCandidateWindow(himc, &candf);
		ImmReleaseContext(hwnd, himc);
	}
}

#ifndef UNICODE
LPSTR PASCAL DoDBCSBoundary(LPTSTR lpsz, int *lpcchMax)
{
	int i = 0;

	while (i < *lpcchMax && *lpsz)
	{
		i++;

		if (IsDBCSLeadByte(*lpsz))
		{

			if (i >= *lpcchMax)
			{
				--i; // Wrap up without the last leadbyte.
				break;
			}

			i++;
			lpsz += 2;
		}
		else
		{
			lpsz++;
		}
	}

	*lpcchMax = i;

	return lpsz;
}
#endif

void PASCAL DrawCompositionLine(HWND hwnd, HDC hdc, HFONT hfont, LPTSTR lpszComp, LPBYTE lpszAttr, int ichCompStart, int ichCompEnd, int ichStart)
{
	PTSTR pszCompStr;
	int ichSt, ichEnd;
	DWORD dwPos;
	BYTE bAttr;
	HFONT hfontOld;

	int  fnPen;
	HPEN hPen;
	COLORREF crDrawText;
	COLORREF crDrawBack;
	COLORREF crOldText;
	COLORREF crOldBk;


	while (ichCompStart < ichCompEnd)
	{

		// Get the fragment to draw
		//
		// ichCompStart,ichCompEnd -- index at Edit Control
		// ichSt,ichEnd            -- index at lpszComp

		ichEnd = ichSt = ichCompStart - ichStart;
		bAttr = lpszAttr[ichSt];

		while (ichEnd < ichCompEnd - ichStart)
		{
			if (bAttr == lpszAttr[ichEnd])
			{
				ichEnd++;
			}
			else
			{
				break;
			}
		}

		pszCompStr = (PTSTR)LocalAlloc(LPTR, sizeof(TCHAR) * (ichEnd - ichSt + 1 + 1)); // 1 for NULL.

		if (pszCompStr)
		{
			lstrcpyn(pszCompStr, &lpszComp[ichSt], ichEnd - ichSt + 1);
			pszCompStr[ichEnd - ichSt] = '\0';
		}


		// Attribute stuff
		switch (bAttr)
		{
		case ATTR_INPUT:
			fnPen = PS_DOT;
			crDrawText = GetSysColor(COLOR_WINDOWTEXT);
			crDrawBack = GetSysColor(COLOR_WINDOW);
			break;
		case ATTR_TARGET_CONVERTED:
		case ATTR_TARGET_NOTCONVERTED:
			fnPen = PS_DOT;
			crDrawText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			crDrawBack = GetSysColor(COLOR_HIGHLIGHT);
			break;
		case ATTR_CONVERTED:
			fnPen = PS_SOLID;
			crDrawText = GetSysColor(COLOR_WINDOWTEXT);
			crDrawBack = GetSysColor(COLOR_WINDOW);
			break;
		}
		crOldText = SetTextColor(hdc, crDrawText);
		crOldBk = SetBkColor(hdc, crDrawBack);

		hfontOld = (HFONT)SelectObject(hdc, hfont);

		// Get the start position of composition
		//
		dwPos = (DWORD)SendMessage(hwnd, EM_POSFROMCHAR, ichCompStart, 0);

		// Draw it.
		TextOut(hdc, GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos), pszCompStr, ichEnd - ichSt);
#ifndef DONT_UNDERLINE
		// Underline
		hPen = CreatePen(fnPen, 1, crDrawText);
		if (hPen)
		{

			HPEN hpenOld = (HPEN)SelectObject(hdc, hPen);
			int iOldBk = SetBkMode(hdc, TRANSPARENT);
			SIZE size;

			GetTextExtentPoint(hdc, pszCompStr, ichEnd - ichSt, &size);

			MoveToEx(hdc, GET_X_LPARAM(dwPos), size.cy + GET_Y_LPARAM(dwPos) - 1, NULL);

			LineTo(hdc, size.cx + GET_X_LPARAM(dwPos), size.cy + GET_Y_LPARAM(dwPos) - 1);

			SetBkMode(hdc, iOldBk);

			if (hpenOld)
			{
				SelectObject(hdc, hpenOld);
			}

			DeleteObject(hPen);
		}
#endif

		if (hfontOld)
		{
			SelectObject(hdc, hfontOld);
		}

		SetTextColor(hdc, crOldText);
		SetBkColor(hdc, crOldBk);

		LocalFree((HLOCAL)pszCompStr);

		//Next fragment
		//
		ichCompStart += ichEnd - ichSt;
	}
}

void PASCAL ListView_InsertComposition(HWND hwnd, WPARAM wParam, LPARAM lParam, SListViewData *plv)
{
	PSTR pszCompStr;

	int  cbComp = 0;
	int  cbCompNew;
	int  cchMax;
	int  cchText;
	DWORD dwSel;
	HIMC himc = (HIMC)0;


	// To prevent recursion..

	if (plv->flags & LVF_INSERTINGCOMP)
	{
		return;
	}
	plv->flags |= LVF_INSERTINGCOMP;

	// Don't want to redraw edit during inserting.
	//
	SendMessage(hwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

	// If we have RESULT STR, put it to EC first.

	if (himc = ImmGetContext(hwnd))
	{
#ifdef WIN32
		if (!(dwSel = PtrToUlong(GetProp(hwnd, szIMECompPos))))
		{
			dwSel = Edit_GetSel(hwnd);
		}

		// Becaues we don't setsel after inserting composition
		// in win32 case.
		Edit_SetSel(hwnd, GET_X_LPARAM(dwSel), GET_Y_LPARAM(dwSel));
#endif
		if (lParam & GCS_RESULTSTR)
		{
			// ImmGetCompositionString() returns length of buffer in bytes,
			// not in # of character
			cbComp = (int)ImmGetCompositionString(himc, GCS_RESULTSTR, NULL, 0);

			pszCompStr = (PSTR)LocalAlloc(LPTR, cbComp + sizeof(TCHAR));
			if (pszCompStr)
			{
				ImmGetCompositionString(himc, GCS_RESULTSTR, (PSTR)pszCompStr, cbComp + sizeof(TCHAR));

				// With ImmGetCompositionStringW, cbComp is # of bytes copied
				// character position must be calculated by cbComp / sizeof(TCHAR)
				//
				*(TCHAR *)(&pszCompStr[cbComp]) = TEXT('\0');
				Edit_ReplaceSel(hwnd, (LPTSTR)pszCompStr);
				LocalFree((HLOCAL)pszCompStr);
			}
#ifdef WIN32
			// There's no longer selection
			//
			RemoveProp(hwnd, szIMECompPos);

			// Get current cursor pos so that the subsequent composition
			// handling will do the right thing.
			//
			dwSel = Edit_GetSel(hwnd);
#endif
		}

		if (lParam & GCS_COMPSTR)
		{
			// ImmGetCompositionString() returns length of buffer in bytes,
			// not in # of character
			//
			cbComp = (int)ImmGetCompositionString(himc, GCS_COMPSTR, NULL, 0);
			pszCompStr = (PSTR)LocalAlloc(LPTR, cbComp + sizeof(TCHAR));
			if (pszCompStr)
			{
				ImmGetCompositionString(himc, GCS_COMPSTR, pszCompStr, cbComp + sizeof(TCHAR));

				// Get position of the current selection
				//
#ifndef WIN32
				dwSel = Edit_GetSel(hwnd);
#endif
				cchMax = (int)SendMessage(hwnd, EM_GETLIMITTEXT, 0, 0);
				cchText = Edit_GetTextLength(hwnd);

				// Cut the composition string if it exceeds limit.
				//
				cbCompNew = min((UINT)cbComp,
						sizeof(TCHAR) * (cchMax - (cchText - (HIWORD(dwSel) - LOWORD(dwSel)))));

				// wrap up the DBCS at the end of string
				//
				if (cbCompNew < cbComp)
				{
#ifndef UNICODE
					DoDBCSBoundary((LPSTR)pszCompStr, (int *)&cbCompNew);
#endif

					*(TCHAR *)(&pszCompStr[cbCompNew]) = TEXT('\0');

					// Reset composition string if we cut it.
					ImmSetCompositionString(himc, SCS_SETSTR, pszCompStr, cbCompNew, NULL, 0);
					cbComp = cbCompNew;
				}

				*(TCHAR *)(&pszCompStr[cbComp]) = TEXT('\0');

				// Replace the current selection with composition string.
				//
				Edit_ReplaceSel(hwnd, (LPTSTR)pszCompStr);

				LocalFree((HLOCAL)pszCompStr);
			}

			// Mark the composition string so that we can replace it again
			// for the next time.
			//

#ifdef WIN32
			// Don't setsel to avoid flicking
			if (cbComp)
			{
				dwSel = MAKELONG(LOWORD(dwSel), LOWORD(dwSel) + cbComp / sizeof(TCHAR));
				SetProp(hwnd, szIMECompPos, (HANDLE)dwSel);
			}
			else
			{
				RemoveProp(hwnd, szIMECompPos);
			}
#else
			// Still use SETSEL for 16bit.
			if (cbComp)
			{
				Edit_SetSel(hwnd, LOWORD(dwSel), LOWORD(dwSel) + cbComp);
			}
#endif

		}

		ImmReleaseContext(hwnd, himc);
	}

	SendMessage(hwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
	//
	// We want to update the size of label edit just once at
	// each WM_IME_COMPOSITION processing. ReplaceSel causes several EN_UPDATE
	// and it causes ugly flicking too.
	//
	RedrawWindow(hwnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	SetWindowID(plv->hwndEdit, 1);
	ListView_SetEditSize(plv);

	plv->flags &= ~LVF_INSERTINGCOMP;
}

void PASCAL ListView_PaintComposition(HWND hwnd, SListViewData *plv)
{
	BYTE szCompStr[CCHLABELMAX + 1];
	BYTE szCompAttr[CCHLABELMAX + 1];

	int  cchLine, ichLineStart;
	int  cbComp = 0;
	int  cchComp;
	int  nLine;
	int  ichCompStart, ichCompEnd;
	DWORD dwSel;
	int  cchMax, cchText;
	HIMC himc = (HIMC)0;
	HDC  hdc;


	if (plv->flags & LVF_INSERTINGCOMP)
	{
		// This is the case that ImmSetCompositionString() generates
		// WM_IME_COMPOSITION. We're not ready to paint composition here.
		return;
	}

	if (himc = ImmGetContext(hwnd))
	{

		cbComp = (UINT)ImmGetCompositionString(himc, GCS_COMPSTR, szCompStr, sizeof(szCompStr));

		ImmGetCompositionString(himc, GCS_COMPATTR, szCompAttr, sizeof(szCompStr));
		ImmReleaseContext(hwnd, himc);
	}

	if (cbComp)
	{

		// Get the position of current selection
		//
#ifdef WIN32

		if (!(dwSel = PtrToUlong(GetProp(hwnd, szIMECompPos))))
		{
			dwSel = 0L;
		}
#else
		dwSel = Edit_GetSel(hwnd);
#endif
		cchMax = (int)SendMessage(hwnd, EM_GETLIMITTEXT, 0, 0);
		cchText = Edit_GetTextLength(hwnd);
		cbComp = min((UINT)cbComp, sizeof(TCHAR) * (cchMax - (cchText - (HIWORD(dwSel) - LOWORD(dwSel)))));
#ifndef UNICODE
		DoDBCSBoundary((LPTSTR)szCompStr, (int *)&cbComp);
#endif
		*(TCHAR *)(&szCompStr[cbComp]) = TEXT('\0');



		/////////////////////////////////////////////////
		//                                             //
		// Draw composition string over the sel string.//
		//                                             //
		/////////////////////////////////////////////////


		hdc = GetDC(hwnd);


		ichCompStart = LOWORD(dwSel);

		cchComp = cbComp / sizeof(TCHAR);
		while (ichCompStart < (int)LOWORD(dwSel) + cchComp)
		{
			// Get line from each start pos.
			//
			nLine = Edit_LineFromChar(hwnd, ichCompStart);
			ichLineStart = Edit_LineIndex(hwnd, nLine);
			cchLine = Edit_LineLength(hwnd, ichLineStart);

			// See if composition string is longer than this line.
			//
			if (ichLineStart + cchLine > (int)LOWORD(dwSel) + cchComp)
			{
				ichCompEnd = LOWORD(dwSel) + cchComp;
			}
			else
			{
				// Yes, the composition string is longer.
				// Take the begining of the next line as next start.
				//
				if (ichLineStart + cchLine > ichCompStart)
				{
					ichCompEnd = ichLineStart + cchLine;
				}
				else
				{
					// If the starting position is not proceeding,
					// let's get out of here.
					break;
				}
			}

			// Draw the line
			//
			DrawCompositionLine(hwnd, hdc, plv->hfontLabel, (LPTSTR)szCompStr, szCompAttr, ichCompStart, ichCompEnd, LOWORD(dwSel));

			ichCompStart = ichCompEnd;
		}

		ReleaseDC(hwnd, hdc);
		// We don't want to repaint the window.
		ValidateRect(hwnd, NULL);
	}
}

#endif FE_IME


//////////////////////////////////////////////////////////////////////////
// ListView List模式

#define COLUMN_VIEW

BOOL ListView_LDrawItem(PLVDRAWITEM plvdi)
{
	RECT rcIcon;
	RECT rcLabel;
	RECT rcBounds;
	RECT rcT;
	LVITEMEX item;
	TCHAR ach[CCHLABELMAX];
	SListViewData *plv = plvdi->plv;
	int i = (int)plvdi->nmcd.nmcd.dwItemSpec;

	// moved here to reduce call backs in OWNERDATA case
	//
	item.iItem = i;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	item.stateMask = LVIS_ALL;
	item.pszText = ach;
	item.cchTextMax = ARRAYSIZE(ach);

	ListView_OnGetItem(plv, &item);

	ListView_LGetRects(plv, i, &rcIcon, &rcLabel, &rcBounds, NULL);

	if (!plvdi->prcClip || IntersectRect(&rcT, &rcBounds, plvdi->prcClip))
	{
		UINT fText;

		if (plvdi->lpptOrg)
		{
			OffsetRect(&rcIcon, plvdi->lpptOrg->x - rcBounds.left,
				plvdi->lpptOrg->y - rcBounds.top);
			OffsetRect(&rcLabel, plvdi->lpptOrg->x - rcBounds.left,
				plvdi->lpptOrg->y - rcBounds.top);
		}


		fText = ListView_DrawImage(plv, &item, plvdi->nmcd.nmcd.hdc,
				rcIcon.left, rcIcon.top, plvdi->flags) | SHDT_ELLIPSES;

		// Don't draw the label if it is being edited.
		if (plv->iEdit != i)
		{
			int ItemCxSingleLabel;
			UINT ItemState;

			if (ListView_IsOwnerData(plv))
			{
				LISTITEM listitem;

				// calculate lable sizes from iItem
				listitem.pszText = ach;
				ListView_RecomputeLabelSize(plv, &listitem, i, plvdi->nmcd.nmcd.hdc, TRUE);

				ItemCxSingleLabel = listitem.cxSingleLabel;
				ItemState = item.state;
			}
			else
			{
				ItemCxSingleLabel = plvdi->pitem->cxSingleLabel;
				ItemState = plvdi->pitem->state;
			}

			if (plvdi->flags & LVDI_TRANSTEXT)
			{
				fText |= SHDT_TRANSPARENT;
			}

			if (ItemCxSingleLabel == SRECOMPUTE)
			{
				ListView_RecomputeLabelSize(plv, plvdi->pitem, i, plvdi->nmcd.nmcd.hdc, FALSE);
				ItemCxSingleLabel = plvdi->pitem->cxSingleLabel;
			}

			if (ItemCxSingleLabel < rcLabel.right - rcLabel.left)
			{
				rcLabel.right = rcLabel.left + ItemCxSingleLabel;
			}

			if ((fText & SHDT_SELECTED) && (plvdi->flags & LVDI_HOTSELECTED))
			{
				fText |= SHDT_HOTSELECTED;
			}

#ifdef WINDOWS_ME
			if (plv->dwExStyle & WS_EX_RTLREADING)
			{
				fText |= SHDT_RTLREADING;
			}
#endif

			SHDrawText(plvdi->nmcd.nmcd.hdc, item.pszText, &rcLabel, LVCFMT_LEFT, fText,
				plv->cyLabelChar, plv->cxEllipses,
				plvdi->nmcd.clrText, plvdi->nmcd.clrTextBk);

			if ((plvdi->flags & LVDI_FOCUS) && (ItemState & LVIS_FOCUSED)
#ifdef KEYBOARDCUES
				&& !(CCGetUIState(&(plvdi->plv->hwnd->ci)) & UISF_HIDEFOCUS)
#endif
			)
			{
				DrawFocusRect(plvdi->nmcd.nmcd.hdc, &rcLabel);
			}
		}
	}
	return TRUE;
}

DWORD ListView_LApproximateViewRect(SListViewData *plv, int iCount, int iWidth, int iHeight)
{
	int cxItem = plv->cxItem;
	int cyItem = plv->cyItem;
	int cCols;
	int cRows;

	cRows = iHeight / cyItem;
	cRows = min(cRows, iCount);

	if (cRows == 0)
	{
		cRows = 1;
	}
	cCols = (iCount + cRows - 1) / cRows;

	iWidth = cCols * cxItem;
	iHeight = cRows * cyItem;

	return MAKELONG(iWidth + GetBorderRect(plv->hwnd).left, iHeight + GetBorderRect(plv->hwnd).bottom);
}



int ListView_LItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem)
{
	int iHit;
	int i;
	int iCol;
	int xItem; //where is the x in relation to the item
	UINT flags;
	LISTITEM *pitem;

	if (piSubItem)
	{
		*piSubItem = 0;
	}

	flags = LVHT_NOWHERE;
	iHit = -1;

#ifdef COLUMN_VIEW
	i = y / plv->cyItem;
	if (i >= 0 && i < plv->cItemCol)
	{
		iCol = (x + plv->xOrigin) / plv->cxItem;
		i += iCol * plv->cItemCol;
		if (i >= 0 && i < ListView_Count(plv))
		{
			iHit = i;

			xItem = x + plv->xOrigin - iCol * plv->cxItem;
			if (xItem < plv->cxState)
			{
				flags = LVHT_ONITEMSTATEICON;
			}
			else if (xItem < (plv->cxState + plv->cxSmIcon))
			{
				flags = LVHT_ONITEMICON;
			}
			else
			{
				int ItemCxSingleLabel;

				if (ListView_IsOwnerData(plv))
				{
					LISTITEM item;

					// calculate lable sizes from iItem
					ListView_RecomputeLabelSize(plv, &item, i, NULL, FALSE);
					ItemCxSingleLabel = item.cxSingleLabel;
				}
				else
				{
					pitem = ListView_FastGetItemPtr(plv, i);
					if (pitem->cxSingleLabel == SRECOMPUTE)
					{
						ListView_RecomputeLabelSize(plv, pitem, i, NULL, FALSE);
					}
					ItemCxSingleLabel = pitem->cxSingleLabel;
				}

				if (xItem < (plv->cxSmIcon + plv->cxState + ItemCxSingleLabel))
				{
					flags = LVHT_ONITEMLABEL;
				}
			}
		}
	}
#else
	i = x / plv->cxItem;
	if (i < plv->cItemCol)
	{
		i += ((y + plv->xOrigin) / plv->cyItem) * plv->cItemCol;
		if (i < ListView_Count(plv))
		{
			iHit = i;
			flags = LVHT_ONITEMICON;
		}
	}
#endif

	*pflags = flags;
	return iHit;
}

void ListView_LGetRects(SListViewData *plv, int i, RECT *prcIcon,
	RECT *prcLabel, RECT *prcBounds, RECT *prcSelectBounds)
{
	RECT rcIcon;
	RECT rcLabel;
	int x, y;
	int cItemCol = plv->cItemCol;

	if (cItemCol == 0)
	{
		// Called before other data has been initialized so call
		// update scrollbars which should make sure that that
		// we have valid data...
		ListView_UpdateScrollBars(plv);

		// but it's possible that updatescrollbars did nothing because of
		// LVS_NOSCROLL or redraw
		// BUGBUG raymondc v6.0:  Get it right even if no redraw. Fix for v6.
		if (plv->cItemCol == 0)
		{
			cItemCol = 1;
		}
		else
		{
			cItemCol = plv->cItemCol;
		}
	}

#ifdef COLUMN_VIEW
	x = (i / cItemCol) * plv->cxItem;
	y = (i % cItemCol) * plv->cyItem;
	rcIcon.left = x - plv->xOrigin + plv->cxState;
	rcIcon.top = y;
#else
	x = (i % cItemCol) * plv->cxItem;
	y = (i / cItemCol) * plv->cyItem;
	rcIcon.left = x;
	rcIcon.top = y - plv->xOrigin;
#endif

	rcIcon.right = rcIcon.left + plv->cxSmIcon;
	rcIcon.bottom = rcIcon.top + plv->cyItem;

	if (prcIcon)
	{
		*prcIcon = rcIcon;
	}

	rcLabel.left = rcIcon.right;
	rcLabel.right = rcIcon.left + plv->cxItem - plv->cxState;
	rcLabel.top = rcIcon.top;
	rcLabel.bottom = rcIcon.bottom;
	if (prcLabel)
	{
		*prcLabel = rcLabel;
	}

	if (prcBounds)
	{
		*prcBounds = rcLabel;
		prcBounds->left = rcIcon.left - plv->cxState;
	}

	if (prcSelectBounds)
	{
		*prcSelectBounds = rcLabel;
		prcSelectBounds->left = rcIcon.left;
	}
}


void ListView_LUpdateScrollBars(SListViewData *plv)
{
	RECT rcClient;
	int cItemCol;
	int cCol;
	int cColVis;
	SCROLLINFO si;

	_ASSERT(plv);

	ListView_GetClientRect(plv, &rcClient, FALSE, NULL);

#ifdef COLUMN_VIEW
	cColVis = (rcClient.right - rcClient.left) / plv->cxItem;
	cItemCol = max(1, (rcClient.bottom - rcClient.top) / plv->cyItem);
#else
	cColVis = (rcClient.bottom - rcClient.top) / plv->cyItem;
	cItemCol = max(1, (rcClient.right - rcClient.left) / plv->cxItem);
#endif

	cCol = (ListView_Count(plv) + cItemCol - 1) / cItemCol;

	// Make the client area smaller as appropriate, and
	// recompute cCol to reflect scroll bar.
	//
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nPage = cColVis;
	si.nMin = 0;

#ifdef COLUMN_VIEW
	rcClient.bottom -= ListView_GetCyScrollbar(plv);

	cItemCol = max(1, (rcClient.bottom - rcClient.top) / plv->cyItem);
	cCol = (ListView_Count(plv) + cItemCol - 1) / cItemCol;

	si.nPos = plv->xOrigin / plv->cxItem;
	si.nMax = cCol - 1;

	ListView_SetScrollInfo(plv, SB_HORZ, &si, TRUE);
#else
	rcClient.right -= ListView_GetCxScrollbar(plv);

	cItemCol = max(1, (rcClient.right - rcClient.left) / plv->cxItem);
	cCol = (ListView_Count(plv) + cItemCol - 1) / cItemCol;

	si.nPos = plv->xOrigin / plv->cyItem;
	si.nMax = cCol - 1;

	ListView_SetScrollInfo(plv, SB_VERT, &si, TRUE);
#endif

	// Update number of visible lines...
	//
	if (plv->cItemCol != cItemCol)
	{
		plv->cItemCol = cItemCol;
		InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
	}

	// make sure our position and page doesn't hang over max
	if ((si.nPos + (LONG)si.nPage - 1 > si.nMax) && si.nPos > 0)
	{
		int iNewPos, iDelta;
		iNewPos = (int)si.nMax - (int)si.nPage + 1;
		if (iNewPos < 0)
		{
			iNewPos = 0;
		}
		if (iNewPos != si.nPos)
		{
			iDelta = iNewPos - (int)si.nPos;
#ifdef COLUMN_VIEW
			ListView_LScroll2(plv, iDelta, 0, 0);
#else
			ListView_LScroll2(plv, 0, iDelta, 0);
#endif
			ListView_LUpdateScrollBars(plv);
		}
	}

	// List模式，最多只出现一个滚动条。
#ifdef COLUMN_VIEW
	ListView_SetScrollRange(plv, SB_VERT, 0, 0, TRUE);
#else
	ListView_SetScrollRange(plv, SB_HORZ, 0, 0, TRUE);
#endif
}

//
//  We need a smoothscroll callback so our background image draws
//  at the correct origin.  If we don't have a background image,
//  then this work is superfluous but not harmful either.
//
int CALLBACK ListView_LScroll2_SmoothScroll(
	SListViewData *plv,
	int dx,
	int dy,
	CONST RECT *prcScroll,
	CONST RECT *prcClip,
	HRGN hrgnUpdate,
	LPRECT prcUpdate,
	UINT flags)
{
	if (plv)
	{
#ifdef COLUMN_VIEW
		plv->xOrigin -= dx;
#else
		plv->xOrigin -= dy;
#endif
	}

	// Now do what SmoothScrollWindow would've done if we weren't
	// a callback

	return SIMPLEREGION;//L ScrollWindowEx(hwnd, dx, dy, prcScroll, prcClip, hrgnUpdate, prcUpdate, flags);
}



void ListView_LScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth)
{
#ifdef COLUMN_VIEW
	if (dx)
	{
		dx *= plv->cxItem;

		UpdateWindow(GetHostWnd(plv->hwnd));
	}
#else
	if (dy)
	{
		dy *= plv->cyItem;

		UpdateWindow(GetHostWnd(plv->hwnd));
	}
#endif
}

void ListView_LOnScroll(SListViewData *plv, UINT code, int posNew, UINT sb)
{
	RECT rcClient;
	int cPage;

	if (plv->hwndEdit)
	{
		ListView_DismissEdit(plv, FALSE);
	}

	ListView_GetClientRect(plv, &rcClient, TRUE, NULL);

#ifdef COLUMN_VIEW
	cPage = (rcClient.right - rcClient.left) / plv->cxItem;
	ListView_ComOnScroll(plv, code, posNew, SB_HORZ, 1,
		cPage ? cPage : 1);
#else
	cPage = (rcClient.bottom - rcClient.top) / plv->cyItem;
	ListView_ComOnScroll(plv, code, posNew, SB_VERT, 1,
		cPage ? cPage : 1);
#endif

}

int ListView_LGetScrollUnitsPerLine(SListViewData *plv, UINT sb)
{
	return 1;
}

//------------------------------------------------------------------------------
//
// Function: ListView_LCalcViewItem
//
// Summary: This function will calculate which item slot is at the x, y location
//
// Arguments:
//    plv [in] -  The list View to work with
//    x [in] - The x location
//    y [in] - The y location
//
// Returns: the valid slot the point was within.
//
//  Notes:
//
//  History:
//    Nov-3-94 MikeMi   Created
//
//------------------------------------------------------------------------------

int ListView_LCalcViewItem(SListViewData *plv, int x, int y)
{
	int iItem;
	int iRow = 0;
	int iCol = 0;

	_ASSERT(plv);

#ifdef COLUMN_VIEW
	iRow = y / plv->cyItem;
	iRow = max(iRow, 0);
	iRow = min(iRow, plv->cItemCol - 1);
	iCol = (x + plv->xOrigin) / plv->cxItem;
	iItem = iRow + iCol * plv->cItemCol;

#else
	iCol = x / plv->cxItem;
	iCol = max(iCol, 0);
	iCol = min(iCol, plv->cItemCol - 1);
	iRow = (y + plv->xOrigin) / plv->cyItem;
	iItem = iCol + iRow * plv->cItemCol;

#endif

	iItem = max(iItem, 0);
	iItem = min(iItem, ListView_Count(plv) - 1);

	return (iItem);
}

int LV_GetNewColWidth(SListViewData *plv, int iFirst, int iLast)
{
	int cxMaxLabel = 0;

	// Don't do anything stupid if there are no items to measure

	if (iFirst <= iLast)
	{
		LVFAKEDRAW lvfd;
		LVITEMEX lvitem;
		LISTITEM item;

		if (ListView_IsOwnerData(plv))
		{
			int iViewFirst;
			int iViewLast;


			iViewFirst = ListView_LCalcViewItem(plv, 1, 1);
			iViewLast = ListView_LCalcViewItem(plv,
					plv->sizeClient.cx - 1,
					plv->sizeClient.cy - 1);
			if ((iLast - iFirst) > (iViewLast - iViewFirst))
			{
				iFirst = max(iFirst, iViewFirst);
				iLast = min(iLast, iViewLast);
			}

			iLast = min(ListView_Count(plv), iLast);
			iFirst = max(0, iFirst);
			iLast = max(iLast, iFirst);

			ListView_NotifyCacheHint(plv, iFirst, iLast);
		}

		ListView_BeginFakeCustomDraw(plv, &lvfd, &lvitem);
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_PARAM;
		item.lParam = 0;

		while (iFirst <= iLast)
		{
			LISTITEM *pitem;

			if (ListView_IsOwnerData(plv))
			{
				pitem = &item;
				pitem->cxSingleLabel = SRECOMPUTE;
			}
			else
			{
				pitem = ListView_FastGetItemPtr(plv, iFirst);
			}

			if (pitem->cxSingleLabel == SRECOMPUTE)
			{
				lvitem.iItem = iFirst;
				lvitem.lParam = pitem->lParam;
				ListView_BeginFakeItemDraw(&lvfd);
				ListView_RecomputeLabelSize(plv, pitem, iFirst, lvfd.nmcd.nmcd.hdc, FALSE);
				ListView_EndFakeItemDraw(&lvfd);
			}

			if (pitem->cxSingleLabel > cxMaxLabel)
			{
				cxMaxLabel = pitem->cxSingleLabel;
			}

			iFirst++;
		}

		ListView_EndFakeCustomDraw(&lvfd);
	}

	// We have the max label width, see if this plus the rest of the slop will
	// cause us to want to resize.
	//
	cxMaxLabel += plv->cxSmIcon + plv->g_cxIconMargin + plv->cxState;
	if (cxMaxLabel > GetSystemMetrics(SM_CXSCREEN))
	{
		cxMaxLabel = GetSystemMetrics(SM_CXSCREEN);
	}

	return cxMaxLabel;
}


//------------------------------------------------------------------------------
// This function will see if the size of column should be changed for the listview
// It will check to see if the items between first and last exceed the current width
// and if so will see if the columns are currently big enough.  This wont happen
// if we are not currently in listview or if the caller has set an explicit size.
//
// OWNERDATA CHANGE
// This function is normally called with the complete list range,
// This will has been changed to be called only with currently visible
// to the user when in OWNERDATA mode.  This will be much more effiencent.
//
BOOL ListView_MaybeResizeListColumns(SListViewData *plv, int iFirst, int iLast)
{
	HDC hdc = NULL;
	int cxMaxLabel;

	if (!ListView_IsListView(plv) || (plv->flags & LVF_COLSIZESET))
	{
		return (FALSE);
	}

	cxMaxLabel = LV_GetNewColWidth(plv, iFirst, iLast);

	// Now see if we should resize the columns...
	if (cxMaxLabel > plv->cxItem)
	{
		int iScroll = plv->xOrigin / plv->cxItem;
		TraceMsg(TF_LISTVIEW, "SListViewData Resize Columns: %d", cxMaxLabel);
		ListView_ISetColumnWidth(plv, 0, cxMaxLabel, FALSE);
		plv->xOrigin = iScroll * plv->cxItem;
		return (TRUE);
	}

	return (FALSE);
}


//////////////////////////////////////////////////////////////////////////
// ListView Report模式


void ListView_RGetRectsEx(SListViewData *plv, int iItem, int iSubItem, LPRECT prcIcon, LPRECT prcLabel);
int ListView_RXHitTest(SListViewData *plv, int x);

#define ColumnHeader_GetItem(hwndHD, i, phdi) \
	(BOOL)SendMessage((hwndHD), HDM_GETITEM, (WPARAM)(int)(i), (LPARAM)(HD_ITEM *)(phdi))

#define ColumnHeader_InsertItem(hwndHD, i, phdi) \
	(int)SendMessage((hwndHD), HDM_INSERTITEM, (WPARAM)(int)(i), (LPARAM)(const HD_ITEM *)(phdi))

#define ColumnHeader_DeleteItem(hwndHD, i) \
	(BOOL)SendMessage((hwndHD), HDM_DELETEITEM, (WPARAM)(int)(i), 0L)

#define ColumnHeader_GetItemRect(hwndHD, iItem, lprc) \
	(BOOL)SendMessage((hwndHD), HDM_GETITEMRECT, (WPARAM)(iItem), (LPARAM)(lprc))

#define ColumnHeader_SetItem(hwndHD, i, phdi) \
	(BOOL)SendMessage((hwndHD), HDM_SETITEM, (WPARAM)(int)(i), (LPARAM)(const HD_ITEM *)(phdi))

#define ColumnHeader_Layout(hwndHD, playout) \
	(BOOL)SendMessage((hwndHD), HDM_LAYOUT, 0, (LPARAM)(HD_LAYOUT *)(playout))

DWORD ListView_RApproximateViewRect(SListViewData *plv, int iCount, int iWidth, int iHeight)
{
	RECT rc;

	ListView_RGetRects(plv, iCount, NULL, NULL, &rc, NULL);
	rc.bottom += plv->ptlRptOrigin.y;
	rc.right += plv->ptlRptOrigin.x;

	return MAKELONG(rc.right, rc.bottom);
}

void CCDrawRect(HDC hdc, int x, int y, int dx, int dy)
{
	RECT    rc;

	rc.left = x;
	rc.top = y;
	rc.right = x + dx;
	rc.bottom = y + dy;

	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}


void PASCAL ListView_RAfterRedraw(SListViewData *plv, HDC hdc)
{
	DWORD dwType = plv->hwnd->ci.style & LVS_TYPEMASK;

	// 绘制表格线
	if (plv->exStyle & LVS_EX_GRIDLINES)
	{
		COLORREF clrBk = SetBkColor(hdc, plv->crGridLine);

		int i;
		int x;
		if (dwType == LVS_REPORT)
		{
			x = -plv->ptlRptOrigin.x;

			// 绘制垂直网格线
			for (i = 0; (i < plv->cCol) && (x < plv->sizeClient.cx); i++)
			{
				HD_ITEM hitem;

				hitem.mask = HDI_WIDTH;

				SendMessage(plv->hwndHdr, HDM_GETITEM,
					SendMessage(plv->hwndHdr, HDM_ORDERTOINDEX, i, 0),
					(LPARAM)&hitem);
				x += hitem.cxy;
				x += plv->ptGridLineOffset.x;

				if (x > 0)
				{
					CCDrawRect(hdc, x, 0, IUIGetGloabelVar()->g_cxBorder, plv->sizeClient.cy);
				}
			}

			// 绘制水平网络线
			x = plv->yTop - 1;
			x += plv->ptGridLineOffset.y;
			if (!plv->bDrawFirstRowLine)
			{
				x += plv->cyItem;
			}
			for (; (x < plv->sizeClient.cy); x += plv->cyItem)
			{
				CCDrawRect(hdc, 0, x, plv->sizeClient.cx, IUIGetGloabelVar()->g_cxBorder);
			}
		}
		else if (dwType == LVS_ICON)
		{
			int cxItem = 0;
			int cyItem = 0;
			if (ListView_IsSmallView(plv))
			{
				cxItem = plv->cxItem;
				cyItem = plv->cyItem;
			}
			else
			{
				cxItem = plv->cxIconSpacing;
				cyItem = plv->cyIconSpacing;
			}

			// 绘制垂直网格线
			x = -plv->ptlRptOrigin.x;
			int cCol = plv->sizeClient.cx / cxItem;
			for (i = 0; (i < cCol) && (x < plv->sizeClient.cx); i++)
			{
				x += cxItem;
				if (x > 0)
				{
					CCDrawRect(hdc, x, 0, IUIGetGloabelVar()->g_cxBorder, plv->sizeClient.cy);
				}
			}

			// 绘制水平网络线
			x = plv->ptGridLineOffset.y;
			for (; (x < plv->sizeClient.cy); x += cyItem)
			{
				CCDrawRect(hdc, 0, x, plv->sizeClient.cx, IUIGetGloabelVar()->g_cxBorder);
			}

		}
		SetBkColor(hdc, clrBk);
	}
}


//
// Internal function to Get the CXLabel, taking into account if the listview
// has no item data and also if RECOMPUTE needs to happen.
//
SHORT PASCAL ListView_RGetCXLabel(SListViewData *plv, int i, LISTITEM *pitem,
	HDC hdc, BOOL fUseItem)
{
	SHORT cxLabel = SRECOMPUTE;


	if (!ListView_IsOwnerData(plv))
	{

		cxLabel = pitem->cxSingleLabel;
	}

	if (cxLabel == SRECOMPUTE)
	{
		LISTITEM item;

		if (!pitem)
		{
			_ASSERT(!fUseItem);
			pitem = &item;
			fUseItem = FALSE;
		}

		ListView_RecomputeLabelSize(plv, pitem, i, hdc, fUseItem);
		cxLabel = pitem->cxSingleLabel;

	}

	// add on the space around the label taken up by the select rect
	cxLabel += 2 * IUIGetGloabelVar()->g_cxLabelMargin;
	return (cxLabel);
}

// 得到非整行选中的Item的Label的宽度
int GetNotFullRowSelectStringWidth(int nColumnIndex, PLVDRAWITEM plvdi, LPTSTR pszText)
{
	int cxLabel;
	SListViewData *plv = plvdi->plv;

	// if selected or focused, the rectangle is more
	// meaningful and should correspond to the string
	//
	if (nColumnIndex == 0)
	{
		LISTITEM litem;
		LISTITEM *pitem = plvdi->pitem;

		if (pitem == NULL)
		{
			pitem = &litem;
			litem.pszText = pszText;
		}
		cxLabel = ListView_RGetCXLabel(plv, (int)plvdi->nmcd.nmcd.dwItemSpec,
				pitem, plvdi->nmcd.nmcd.hdc, TRUE);
	}
	else
	{
		// add g_cxLabelMargin * 6 because we use SHDT_EXTRAMARGIN
		// on nColumnIndex != 0
		// and if you look inside shdrawtext, there are 6 cxlabelmargins added...
		cxLabel = ListView_OnGetStringWidth(plv,
				pszText, plvdi->nmcd.nmcd.hdc) + IUIGetGloabelVar()->g_cxLabelMargin * 6;
	}

	return cxLabel;
}

// TODO: 如果pItem指定了自己的背景，要画自己的。
void ListView_DrawItemBk(SListViewData *plv, LISTITEM *pItem, HDC hdc, int eItemType, LPCRECT lpRect)
{
	BOOL bEnabled = IsWindowEnabled(plv->hwnd);

	// Item背景类型是否是颜色
	BOOL bDrawColor = TRUE;
	COLORREF crBkItem = IUIGetGloabelVar()->g_clrHighlight;
	BOOL bCombineItemBkImage = TRUE;
	HIUIIMAGE himgBkItem = NULL;
	IMAGE_RESIZE_MODE eImageResizeMode = IRM_9GRID;
	CIUIRect rcItemBkImage9GridArg;
	int nCombineBkItemImagePartCount = 2;
	int nDrawCombineBkItemImagePart = 0;

	if (ITEM_FOCUS == eItemType)
	{
		if (BKT_COLOR == plv->m_lvpItemFocus.m_eBkType)
		{
			crBkItem = plv->m_lvpItemFocus.m_crItemBk;
		}
		else if (BKT_IMAGE == plv->m_lvpItemFocus.m_eBkType)
		{
			bDrawColor = FALSE;
			bCombineItemBkImage = FALSE;

			himgBkItem = bEnabled ? plv->m_lvpItemFocus.m_hItemBkBmp : NULL;
			rcItemBkImage9GridArg = plv->m_lvpItemFocus.m_rcItemBkImage9GridArg;
		}
	}
	else if (ITEM_SELECTED == eItemType)
	{
		LV_ITEMPROPERTIES *pProp = bEnabled ? &plv->m_lvpItemS[0] : &plv->m_lvpItemS[1];

		if (BKT_COLOR == pProp->m_eBkType)
		{
			crBkItem = pProp->m_crItemBk;
		}
		else if (BKT_IMAGE == plv->m_lvpItemS[0].m_eBkType)
		{
			bDrawColor = FALSE;

			if (plv->m_himgSelectedItemCombineBk != NULL)
			{
				himgBkItem = plv->m_himgSelectedItemCombineBk;
				nCombineBkItemImagePartCount = 2;
				nDrawCombineBkItemImagePart = bEnabled ? 0 : 1;
			}
			else
			{
				bCombineItemBkImage = FALSE;

				himgBkItem = pProp->m_hItemBkBmp;
				rcItemBkImage9GridArg = pProp->m_rcItemBkImage9GridArg;
			}
		}
	}
	else if (ITEM_HIGHLIGHT == eItemType)
	{
		if (BKT_COLOR == plv->m_lvpItemH.m_eBkType)
		{
			crBkItem = plv->m_lvpItemH.m_crItemBk;
		}
		else if (BKT_IMAGE == plv->m_lvpItemH.m_eBkType)
		{
			bDrawColor = FALSE;
			bCombineItemBkImage = FALSE;

			himgBkItem = plv->m_lvpItemH.m_hItemBkBmp;
			rcItemBkImage9GridArg = plv->m_lvpItemH.m_rcItemBkImage9GridArg;
		}
	}
	else
	{
		// 正常状态，如果本Item没有特别指定背景，则直接返回
		if (pItem->pItemSpecBackground == NULL)
		{
			return;
		}
	}

	if (bDrawColor)
	{
		FillSolidRect(hdc, lpRect, crBkItem,
			IsUpdateLayeredWindow(GetHostWnd(plv->hwnd)) ? TRUE : FALSE);
	}
	else
	{
		if (bCombineItemBkImage)
		{
			IUIPartDrawImage(hdc, lpRect, himgBkItem, eImageResizeMode, rcItemBkImage9GridArg,
				nCombineBkItemImagePartCount, nDrawCombineBkItemImagePart);
		}
		else
		{
			IUIDrawImage(hdc, lpRect, himgBkItem, eImageResizeMode, rcItemBkImage9GridArg);
		}
	}
}

//
// Returns FALSE if no more items to draw.
//
BOOL ListView_RDrawItem(PLVDRAWITEM plvdi)
{
	BOOL fDrawFocusRect = FALSE;
	RECT rcIcon;
	RECT rcLabel;
	SListViewData *plv = plvdi->plv;
	UINT fText = 0;
	UINT uSubItemFlags = 0;
	int iColumnIndex = 0;
	DWORD dwCustom = 0;
	int iCol = 0;

	//
	// 检测Item范围是否超过绘制范围
	//
	RECT rcBounds;
	ListView_RGetRects(plv, (int)plvdi->nmcd.nmcd.dwItemSpec, NULL, NULL, &rcBounds, NULL);

	if (rcBounds.bottom <= plv->yTop)
	{
		return TRUE;
	}

	RECT rcT;
	if (plvdi->prcClip != NULL)
	{
		if (rcBounds.top >= plvdi->prcClip->bottom)
		{
			return FALSE;    // no more items need painting.
		}

		// Probably this condition won't happen very often...
		if (!IntersectRect(&rcT, &rcBounds, plvdi->prcClip))
		{
			return TRUE;
		}
	}


	// REVIEW: this would be faster if we did the GetClientRect
	// outside the loop.
	//
	if (rcBounds.top >= plv->sizeClient.cy)
	{
		return FALSE;
	}

	int xOffset = 0;
	int yOffset = 0;
	if (plvdi->lpptOrg != NULL)
	{
		xOffset = plvdi->lpptOrg->x - rcBounds.left;
		yOffset = plvdi->lpptOrg->y - rcBounds.top;
		OffsetRect(&rcBounds, xOffset, yOffset);
	}


	//
	// 获取Sub Item属性，定义相关变量item.
	//
	LVITEMEX item;
	item.iItem = (int)plvdi->nmcd.nmcd.dwItemSpec;
	item.stateMask = LVIS_ALL;

	// for first ListView_OnGetItem call
	item.state = 0;

	if (plv->hwnd->ci.style & LVS_OWNERDRAWFIXED)
	{
		goto SendOwnerDraw;
	}

	SetRectEmpty(&rcT);

	//
	// 绘制Sub Item
	//
	TCHAR ach[CCHLABELMAX];
	HDITEM hitem;
	for (iCol = 0; iCol < plv->cCol; iCol++)
	{
		dwCustom = 0;
		UINT uImageFlags;

		// Report风格的ListView的Header Item，是可以交换顺序的。
		// 但它们交换顺序后，索引是不变的。 HDM_ORDERTOINDEX消息就是通过视觉上的顺序
		// 得到Header Item真正的索引。
		iColumnIndex = (int)SendMessage(plv->hwndHdr, HDM_ORDERTOINDEX, iCol, 0);

SendOwnerDraw:

		if (iColumnIndex == 0)
		{
			item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_BINDWL;
		}
		else
		{
			// Next time through, we only want text for subitems...
			item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_BINDWL;
		}

		//
		// 得到Sub Item信息
		//
		item.iImage = -1;
		item.iSubItem = iColumnIndex;
		item.pszText = ach;
		item.cchTextMax = ARRAYSIZE(ach);
		ListView_OnGetItem(plv, &item);

		//
		// 准备Sub Item状态
		//
		int nState = ITEM_NORMAL;
		if (plv->iHot == item.iItem)
		{
			nState = ITEM_HIGHLIGHT;
		}
		if ((uSubItemFlags & LVDI_FOCUS) && (item.state & LVIS_FOCUSED))
		{
			nState = ITEM_FOCUS;
		}
		else if (item.state & LVIS_SELECTED)
		{
			nState = ITEM_SELECTED;
		}

		uSubItemFlags = plvdi->flags;

		if (iColumnIndex == 0)
		{

			// if it's owner draw, send off a message and return.
			// do this after we've collected state information above though
			if (plv->hwnd->ci.style & LVS_OWNERDRAWFIXED)
			{
				DRAWITEMSTRUCT di = { 0 };
				di.CtlType = ODT_LISTVIEW;
				di.CtlID = (UINT)(LPCTSTR)GetName(plv->hwnd);
				di.itemID = (int)plvdi->nmcd.nmcd.dwItemSpec;
				di.itemAction = ODA_DRAWENTIRE;
				di.hwndItem = (HWND)plv->hwnd;
				di.hDC = plvdi->nmcd.nmcd.hdc;
				di.rcItem = rcBounds;
				if (plvdi->pitem)
				{
					di.itemData = plvdi->pitem->lParam;
				}
				if (item.state & LVIS_FOCUSED)
				{
					di.itemState |= ODS_FOCUS;
				}
				if (item.state & LVIS_SELECTED)
				{
					di.itemState |= ODS_SELECTED;
				}

				RoutedEventArgs arg;
				arg.hSender = plv->hwnd;
				arg.hOriginalSender = plv->hwnd;
				arg.RoutedEvent = WM_DRAWITEM;
				arg.eRoutingStrategy = RS_BUBBLE;
				arg.lParam = (LPARAM)(DRAWITEMSTRUCT *)&di;
				RaiseEvent(arg.hOriginalSender, &arg);

				return TRUE;
			}

		}

		// 如果Sub Item 绑定了Windowless控件，则只绘制Windowless控件
		if (item.hBindWindowless != NULL)
		{
			ListView_RGetRectsEx(plv, (int)plvdi->nmcd.nmcd.dwItemSpec, iColumnIndex, &rcIcon, &rcLabel);

			if (!ListView_FullRowSelect(plv))
			{
				int cxLabel = GetNotFullRowSelectStringWidth(iColumnIndex, plvdi, item.pszText);
				if (rcLabel.right > rcLabel.left + cxLabel)
				{
					rcLabel.right = rcLabel.left + cxLabel;
				}
			}

			CIUIRect rcBindCtrl;
			rcBindCtrl.UnionRect(&rcIcon, &rcLabel);
			MoveWindow(item.hBindWindowless, &rcBindCtrl, FALSE);

			// 备份视口、裁剪区域
			POINT ptOrg;
			GetViewportOrgEx(plvdi->nmcd.nmcd.hdc, &ptOrg);

			HRGN hOldRgn = CreateRectRgn(0, 0, 0, 0);
			GetClipRgn(plvdi->nmcd.nmcd.hdc, hOldRgn);

			// 绘制绑定的控件
			DrawControlAndChildren(item.hBindWindowless, plvdi->nmcd.nmcd.hdc, &rcBindCtrl);

			// 恢复视口：
			// 注意：IUI默认绘制机制是先绘制父控件，等父控件整个绘制完后，再绘制子控件。
			// 虽然子控件绘制时，会修改HDC的视口，但这时父控件已彻底绘制完了，所以子控件
			// 修改HDC视口，也不会影响父控件的绘制。
			// 但由于ListView的特殊性，ListView是在绘制自己的时候，同时把Item绑定的控件
			// 进行了绘制，而在绘制完Item绑定的控件后，HDC的视口已不是ListView的视口了，
			// 而这时，ListView还未绘制完，这将导致ListView之后的绘制位置全部错误。
			// 同理，裁剪区域也需要恢复
			SetViewportOrgEx(plvdi->nmcd.nmcd.hdc, ptOrg.x, ptOrg.y, NULL);
			SelectClipRgn(plvdi->nmcd.nmcd.hdc, hOldRgn);
			DeleteObject(hOldRgn);
			hOldRgn = NULL;
		}
		else
		{
			//
			// 准备Sub Item坐标
			//
			hitem.mask = HDI_WIDTH | HDI_FORMAT;
			ColumnHeader_GetItem(plv->hwndHdr, iColumnIndex, &hitem);

			// first get the rects...
			ListView_RGetRectsEx(plv, (int)plvdi->nmcd.nmcd.dwItemSpec,
				iColumnIndex, &rcIcon, &rcLabel);
			OffsetRect(&rcIcon, xOffset, yOffset);
			OffsetRect(&rcLabel, xOffset, yOffset);

			if (iColumnIndex != 0)
			{
				// for right now, add this in because the get rects for
				// non 0 doesn't account for the icon (yet)
				if (item.iImage != -1)
				{
					rcLabel.left += plv->cxSmIcon;
				}

			}

			uImageFlags = uSubItemFlags;

			if (item.iImage == -1)
			{
				if (iColumnIndex != 0)
				{
					// just use ListView_DrawImage to get the fText
					uImageFlags |= LVDI_NOIMAGE;
				}
			}
			else if (ListView_FullRowSelect(plv) && iCol)
			{
				int iLeft = rcIcon.left;
				int iRight = rcIcon.right;
				// for full row select when the icon is not in the first column,
				// we need to explicitly paint the background so focus rect
				// remnants aren't left behind (jeffbog -- 07/09/96)

				/// need to deal with the state image if index == 0.
				// all the otehr columns don't have state images.
				if (iColumnIndex == 0)
				{
					rcIcon.left -= plv->cxState + GetBorderRect(plv->hwnd).left;
				}
				rcIcon.right = rcLabel.right;
				FillRect(plvdi->nmcd.nmcd.hdc, &rcIcon, plv->hbrBk);

				rcIcon.left = iLeft;
				rcIcon.right = iRight;
			}

			//
			// 绘制Sub Item图标，并返回文本状态
			//
			fText = ListView_DrawImageEx(plv, &item, plvdi->nmcd.nmcd.hdc,
					rcIcon.left, rcIcon.top, uSubItemFlags, rcLabel.right);

			if (ListView_FullRowSelect(plv) && (uSubItemFlags & LVDI_FOCUS))
			{
				// if we're doing a full row selection, collect the union
				// of the labels for the focus rect
				UnionRect(&rcT, &rcT, &rcLabel);
			}

			//
			// 绘制文本。 注意，这里是判断item.pszText != NULL，而不是判断pszText中是否有内容
			//
			if (item.pszText != NULL)
			{
				int xLabelRight = rcLabel.right;
				UINT textflags;

				// give all but the first columns extra margins so
				// left and right justified things don't stick together

				textflags = (iColumnIndex == 0) ? SHDT_ELLIPSES : SHDT_ELLIPSES | SHDT_EXTRAMARGIN;

				// rectangle limited to the size of the string
				textflags |= fText;

				if (!ListView_FullRowSelect(plv))
				{
					if ((fText & (SHDT_SELECTED | SHDT_SELECTNOFOCUS))
						|| (item.state & LVIS_FOCUSED))
					{
						int cxLabel = GetNotFullRowSelectStringWidth(iColumnIndex, plvdi, item.pszText);
						if (rcLabel.right > rcLabel.left + cxLabel)
						{
							rcLabel.right = rcLabel.left + cxLabel;
						}
					}
				}

				if ((iColumnIndex != 0) || (plv->iEdit != (int)plvdi->nmcd.nmcd.dwItemSpec))
				{
					COLORREF clrText;
					HFONT hFontTemp = NULL;
					int cxEllipses;

					clrText = plvdi->nmcd.clrText;
					if ((clrText == GetSysColor(COLOR_HOTLIGHT))
						|| ((plv->exStyle & LVS_EX_UNDERLINEHOT) && ((plv->exStyle & LVS_EX_ONECLICKACTIVATE) || ((plvdi->plv->exStyle & LVS_EX_TWOCLICKACTIVATE) && ListView_OnGetItemState(plvdi->plv, (int)plvdi->nmcd.nmcd.dwItemSpec, LVIS_SELECTED)))))
					{
						if (iColumnIndex != 0 && !ListView_FullRowSelect(plv))
						{

							hFontTemp = SelectFont(plvdi->nmcd.nmcd.hdc, plv->hfontLabel);
							if (hFontTemp != plv->hFontHot)
							{
								// they've overridden... leave it.
								SelectFont(plvdi->nmcd.nmcd.hdc, hFontTemp);
								hFontTemp = NULL;
							}
							clrText = plv->clrText;
						}
					}


					if ((textflags & SHDT_SELECTED) && (uSubItemFlags & LVDI_HOTSELECTED))
					{
						textflags |= SHDT_HOTSELECTED;
					}

					//TraceMsg(TF_LISTVIEW, "LISTVIEW: SHDrawText called.  style = %lx, WS_DISABLED = %lx, plvdi->clrBk = %lx, plvdi->nmcd.clrTextBk = %lx", (DWORD)plv->hwnd->ci.style, (DWORD)WS_DISABLED, plvdi->clrBk, plvdi->nmcd.clrTextBk);

#ifdef WINDOWS_ME
					if (plv->dwExStyle & WS_EX_RTLREADING)
					{
						//
						// temp hack for the find.files to see if LtoR/RtoL mixing
						// works. if ok, we'll take this out and make that lv ownerdraw
						//
						if ((item.pszText[0] != '\xfd') && (item.pszText[lstrlen(item.pszText) - 1] != '\xfd'))
						{
							textflags |= SHDT_RTLREADING;
						}
					}
#endif

					//
					//  If the app customized the font, we need to get the new
					//  ellipsis size.  We could try to optimize not doing this
					//  if ellipses aren't needed, but tough.  That's what you
					//  get if you use customdraw.
					//
					if ((plvdi->dwCustom | dwCustom) & CDRF_NEWFONT)
					{
						SIZE siz;
						GetTextExtentPoint(plvdi->nmcd.nmcd.hdc, IUIGetGloabelVar()->c_szEllipses, CCHELLIPSES, &siz);
						cxEllipses = siz.cx;
					}
					else
					{
						cxEllipses = plv->cxEllipses;
					}

					//
					// 绘制Item背景
					//
					if (iColumnIndex == 0)
					{
						RECT rcItemBk = rcLabel;
						if (ListView_FullRowSelect(plv))
						{
							rcItemBk = rcBounds;
							if (plv->m_bEnableSelectedFitClient)
							{
								rcItemBk.right = plv->sizeClient.cx;
							}
						}
						ListView_DrawItemBk(plv, plvdi->pitem, plvdi->nmcd.nmcd.hdc,
							nState, &rcItemBk);
					}

					//
					// 绘制文本
					//
					SHDrawText(plvdi->nmcd.nmcd.hdc, item.pszText, &rcLabel,
						hitem.fmt & HDF_JUSTIFYMASK, textflags,
						plv->cyLabelChar, cxEllipses,
						clrText, plvdi->nmcd.clrTextBk);

					//
					// 如果Item是焦点Item，则在Item的第一列上，画个焦点框。
					//
					if ((uSubItemFlags & LVDI_FOCUS) && (item.state & LVIS_FOCUSED)
#ifdef KEYBOARDCUES
						&& !(CCGetUIState(&(plvdi->plv->hwnd->ci)) & UISF_HIDEFOCUS)
#endif
					)
					{
						if (ListView_FullRowSelect(plv))
						{
							fDrawFocusRect = TRUE;
						}
						else
						{
							DrawFocusRect(plvdi->nmcd.nmcd.hdc, &rcLabel);
						}
					}

					// If we didn't SHDrawText into the full label rectangle
					// (because the selection or focus rectangle seemed more
					// applicable), draw emptiness into the unused part so
					// garbage doesn't show through.  Use SHDrawText so we
					// draw in exactly the same way that the rest of the code does.

					if (rcLabel.right < xLabelRight)
					{
						rcLabel.left = rcLabel.right;
						rcLabel.right = xLabelRight;
						SHDrawText(plvdi->nmcd.nmcd.hdc, IUIGetGloabelVar()->c_szNULL, &rcLabel,
							LVCFMT_LEFT,
							textflags & SHDT_TRANSPARENT,
							plv->cyLabelChar, cxEllipses,
							clrText, plvdi->nmcd.clrTextBk);

					}


					// restore the font
					if (hFontTemp)
					{
						SelectFont(plvdi->nmcd.nmcd.hdc, hFontTemp);
					}


				}
			}

			if (dwCustom & CDRF_NOTIFYPOSTPAINT)
			{
				CICustomDrawNotify(&plvdi->plv->hwnd->ci, CDDS_SUBITEM | CDDS_ITEMPOSTPAINT, &plvdi->nmcd.nmcd);
			}
		}
	}

	if (fDrawFocusRect)
	{
		DrawFocusRect(plvdi->nmcd.nmcd.hdc, &rcT);
	}

	return TRUE;
}

BOOL_PTR ListView_CreateHeader(SListViewData *plv)
{
	// enable drag drop always here... just fail the notify
	// if the bit in listview isn't set
	DWORD dwStyle = HDS_HORZ | WS_CHILD | HDS_DRAGDROP;

	if (plv->hwnd->ci.style & LVS_NOCOLUMNHEADER)
	{
		dwStyle |= HDS_HIDDEN;
	}
	if (!(plv->hwnd->ci.style & LVS_NOSORTHEADER))
	{
		dwStyle |= HDS_BUTTONS;
	}

	dwStyle |= HDS_FULLDRAG;

	HWLWND hHeader = CreateWindowEx(NULL, 0, IUI_COLUMNHEADER, NULL, dwStyle,
			0, 0, 0, 0, GetHostWnd(plv->hwnd), plv->hwnd, XNAME_COLUMNHEADER, NULL, NULL);

	if (hHeader == NULL)
	{
		return FALSE;
	}
	plv->hwndHdr = hHeader;

	// 设置ColumnHeader的Z-Order，把它放到滚动条下面，否则垂直滚动条的上按钮无法被点击
	SetZOrder(plv->hwndHdr, (HWLWND)HWND_BOTTOM);

	if (plv->hwndHdr)
	{

#ifdef POST_BETA
		NMLVHEADERCREATED nmhc;

		nmhc.hwndHdr = plv->hwndHdr;
		// some apps blow up if a notify is sent before the control is fully created.
		CCSendNotify(&plv->hwnd->ci, LVN_HEADERCREATED, &nmhc.hdr);
		plv->hwndHdr = nmhc.hwndHdr;
#endif

		//L FORWARD_WM_SETFONT(plv->hwndHdr, plv->hfontLabel, FALSE, SendMessage);
		if (plv->himlSmall)
		{
			SendMessage(plv->hwndHdr, HDM_SETIMAGELIST, 0, (LPARAM)plv->himlSmall);
		}
	}
	return (BOOL_PTR)plv->hwndHdr;
}

#ifdef UNICODE
int ListView_OnInsertColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	int iRet;

	//HACK ALERT -- this code assumes that LV_COLUMNA is exactly the same
	// as LV_COLUMNW except for the pointer to the string.
	_ASSERT(sizeof(LV_COLUMNA) == sizeof(LV_COLUMNW));

	if (!pcol)
	{
		return -1;
	}

	if ((pcol->mask & LVCF_TEXT) && (pcol->pszText != NULL))
	{
		pszC = pcol->pszText;
		if ((pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszC)) == NULL)
		{
			// NT's IE4 returned -1, so we keep doing it in IE5.
			return -1;
		}
		else
		{
			pcol->pszText = (LPSTR)pszW;
		}
	}

	iRet = ListView_OnInsertColumn(plv, iCol, (const LV_COLUMN *)pcol);

	if (pszW != NULL)
	{
		pcol->pszText = pszC;

		FreeProducedString(pszW);
	}

	return iRet;
}
#endif

int ListView_OnInsertColumn(SListViewData *plv, int iCol, const LV_COLUMN *pcol)
{
	int idpa = -1;
	HD_ITEM item;

	_ASSERT(LVCFMT_LEFT == HDF_LEFT);
	_ASSERT(LVCFMT_RIGHT == HDF_RIGHT);
	_ASSERT(LVCFMT_CENTER == HDF_CENTER);

	if (iCol < 0 || !pcol)
	{
		return -1;
	}

	if (plv->hwndHdr == NULL && !ListView_CreateHeader(plv))
	{
		return -1;
	}

	item.mask = (HDI_WIDTH | HDI_HEIGHT | HDI_FORMAT | HDI_LPARAM);

	if (pcol->mask & LVCF_IMAGE)
	{
		// do this only if this bit is set so that we don't fault on
		// old binaries
		item.iImage = pcol->iImage;
		item.mask |= HDI_IMAGE;
	}

	if (pcol->mask & LVCF_TEXT)
	{
		item.pszText = pcol->pszText;
		item.mask |= HDI_TEXT;
	}

	if (pcol->mask & LVCF_ORDER)
	{
		item.iOrder = pcol->iOrder;
		item.mask |= HDI_ORDER;
	}


	item.cxy = pcol->mask & LVCF_WIDTH ? pcol->cx : 10; // some random default
	item.fmt = ((pcol->mask & LVCF_FMT) && (iCol > 0)) ? pcol->fmt : LVCFMT_LEFT;
	item.hbm = NULL;

	item.lParam = pcol->mask & LVCF_SUBITEM ? pcol->iSubItem : 0;

	// Column 0 refers to the item list.  If we've already added a
	// column, make sure there are plv->cCol - 1 subitem ptr slots
	// in hdpaSubItems...
	//
	if (plv->cCol > 0)
	{
		if (!plv->hdpaSubItems)
		{
			plv->hdpaSubItems = IUI_DPA_CreateEx(8, plv->hheap);
			if (!plv->hdpaSubItems)
			{
				return -1;
			}
		}

		// WARNING:  the max(0, iCol-1) was min in Win95, which was
		// just wrong.  hopefully(!) no one has relied on this brokeness
		// if so, we may have to version switch it.
		idpa = IUI_DPA_InsertPtr(plv->hdpaSubItems, max(0, iCol - 1), NULL);
		if (idpa == -1)
		{
			return -1;
		}
	}

	iCol = ColumnHeader_InsertItem(plv->hwndHdr, iCol, &item);
	if (iCol == -1)
	{
		if (plv->hdpaSubItems && (idpa != -1))
		{
			IUI_DPA_DeletePtr(plv->hdpaSubItems, idpa);
		}
		return -1;
	}
	plv->xTotalColumnWidth = RECOMPUTE;
	plv->cCol++;
	ListView_UpdateScrollBars(plv);
	if (ListView_IsReportView(plv) && ListView_RedrawEnabled(plv))
	{
		RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	}
	return iCol;
}

// 必须加WINAPI修饰，否则枚举函数无法退出
int WINAPI ListView_FreeColumnData(LPVOID d, LPVOID p)
{
	LISTSUBITEM *plsi = (LISTSUBITEM *)d;
	ListView_FreeSubItem(plsi);
	return 1;
}


BOOL ListView_OnDeleteColumn(SListViewData *plv, int iCol)
{
	if (iCol < 0 || iCol >= plv->cCol)    // validate column index
	{
		RIPMSG(0, "LVM_DELETECOLUMN: Invalid column index: %d", iCol);
		return FALSE;
	}

	if (plv->hdpaSubItems)
	{
		int iDeleteColumn = -1;  // Default to "can't delete column".

		if (0 < iCol)
		{
			iDeleteColumn = iCol; // Deleting col 1+.  Simple delete OK.
		}
		else if (5 <= plv->hwnd->ci.iVersion)
		{
			//
			// Version 5 and later can delete column 0.  Prior versions can't.
			//
			if (1 < plv->cCol && !ListView_IsOwnerData(plv))
			{
				// if deleting column 0,
				// we have to do something a little special...
				// set all item 0 strings to what column 1 has and
				// delete column 1
				int i;
				int iCount = ListView_Count(plv);
				for (i = 0; i < iCount; i++)
				{

					LISTSUBITEM lsi;
					LVITEMEX lvi;
					ListView_GetSubItem(plv, i, 1, &lsi);
					lvi.iSubItem = 0;
					lvi.iItem = i;
					lvi.mask = LVIF_TEXT | LVIF_IMAGE;
					lvi.iImage = lsi.iImage;
					lvi.pszText = lsi.pszText;
					lvi.state = lsi.state;
					lvi.stateMask = 0xffffffff;
					ListView_OnSetItem(plv, &lvi);
				}
				iDeleteColumn = 1;
			}
		}

		if (0 < iDeleteColumn)
		{
			HDPA hdpa = (HDPA)IUI_DPA_DeletePtr(plv->hdpaSubItems, iDeleteColumn - 1);
			IUI_DPA_DestroyCallback(hdpa, (PFNDAENUMCALLBACK)ListView_FreeColumnData, 0);
		}
	}

	if (!ColumnHeader_DeleteItem(plv->hwndHdr, iCol))
	{
		return FALSE;
	}

	plv->cCol--;
	plv->xTotalColumnWidth = RECOMPUTE;
	ListView_UpdateScrollBars(plv);

	if (ListView_IsReportView(plv) && ListView_RedrawEnabled(plv))
	{
		RedrawWindow(GetHostWnd(plv->hwnd), NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	}
	return TRUE;
}

int ListView_RGetColumnWidth(SListViewData *plv, int iCol)
{
	HD_ITEM item;

	item.mask = HDI_WIDTH;

	ColumnHeader_GetItem(plv->hwndHdr, iCol, &item);

	return item.cxy;
}

// The FakeCustomDraw functions are used when you want the customdraw client
// to set up a HDC so you can do stuff like GetTextExtent.
//
//  Usage:
//
//      LVFAKEDRAW lvfd;
//      LVITEMEX item;
//      ListView_BeginFakeCustomDraw(plv, &lvfd, &item);
//      for each item you care about {
//          item.iItem = iItem;
//          item.iItem = iSubItem;
//          item.lParam = <item lParam>; // use ListView_OnGetItem to get it
//          ListView_BeginFakeItemDraw(&lvfd);
//          <party on the HDC in lvfd.nmcd.nmcd.hdc>
//          ListView_EndFakeItemDraw(&lvfd);
//      }
//      ListView_EndFakeCustomDraw(&lvfd);
//

void ListView_BeginFakeCustomDraw(SListViewData *plv, PLVFAKEDRAW plvfd, LVITEMEX *pitem)
{
	plvfd->nmcd.nmcd.hdc = GetDC(GetHostWnd(plv->hwnd));
	plvfd->nmcd.nmcd.uItemState = 0;
	plvfd->nmcd.nmcd.dwItemSpec = 0;
	plvfd->nmcd.nmcd.lItemlParam = 0;
	plvfd->hfontPrev = SelectFont(plvfd->nmcd.nmcd.hdc, plv->hfontLabel);

	//
	//  Since we aren't actually painting anything, we pass an empty
	//  paint rectangle.  Gosh, I hope no app faults when it sees an
	//  empty paint rectangle.
	//
	SetRectEmpty(&plvfd->nmcd.nmcd.rc);

	plvfd->plv = plv;
	plvfd->dwCustomPrev = plv->hwnd->ci.dwCustom;
	plvfd->pitem = pitem;

	plv->hwnd->ci.dwCustom = CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_PREPAINT, &plvfd->nmcd.nmcd);
}

DWORD ListView_BeginFakeItemDraw(PLVFAKEDRAW plvfd)
{
	SListViewData *plv = plvfd->plv;
	LVITEMEX *pitem;

	// Early-out:  If client doesn't use CustomDraw, then stop immediately.
	if (!(plv->hwnd->ci.dwCustom & CDRF_NOTIFYITEMDRAW))
	{
		return CDRF_DODEFAULT;
	}

	pitem = plvfd->pitem;

	// Note that if the client says CDRF_SKIPDEFAULT (i.e., is owner-draw)
	// we measure the item anyway, because that's what IE4 did.

	// Make sure we do have the lParam.  Office will fault if you give
	// bogus lParams during customdraw callbacks.
	plvfd->nmcd.nmcd.dwItemSpec = pitem->iItem;
	if (ListView_IsOwnerData(plv))
	{
		// OwnerData always gets lItemlParam = 0
		_ASSERT(plvfd->nmcd.nmcd.lItemlParam == 0);  // should still be 0
	}
	else
	{
		_ASSERT(pitem->mask & LVIF_PARAM);
		plvfd->nmcd.nmcd.lItemlParam = pitem->lParam;
	}

	if (!(plv->hwnd->ci.dwCustom & CDRF_SKIPDEFAULT))
	{
		plvfd->nmcd.iSubItem = 0;
		plvfd->dwCustomItem = CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_ITEMPREPAINT, &plvfd->nmcd.nmcd);
	}
	else
	{
		plvfd->dwCustomItem = CDRF_DODEFAULT;
	}

	//
	//  Only report view supports sub-items.
	//
	if (!ListView_IsReportView(plv))
	{
		plvfd->dwCustomItem &= ~CDRF_NOTIFYSUBITEMDRAW;
	}

	if (plvfd->dwCustomItem & CDRF_NOTIFYSUBITEMDRAW)
	{
		plvfd->nmcd.iSubItem = pitem->iSubItem;
		plvfd->dwCustomSubItem = CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_SUBITEM | CDDS_ITEMPREPAINT, &plvfd->nmcd.nmcd);
	}
	else
	{
		plvfd->dwCustomSubItem = CDRF_DODEFAULT;
	}

	return plvfd->dwCustomItem | plvfd->dwCustomSubItem;
}

void ListView_EndFakeItemDraw(PLVFAKEDRAW plvfd)
{
	SListViewData *plv = plvfd->plv;

	// Early-out:  If client doesn't use CustomDraw, then stop immediately.
	if (!(plv->hwnd->ci.dwCustom & CDRF_NOTIFYITEMDRAW))
	{
		return;
	}

	if (!(plvfd->dwCustomSubItem & CDRF_SKIPDEFAULT) &&
		(plvfd->dwCustomSubItem & CDRF_NOTIFYPOSTPAINT))
	{
		_ASSERT(plvfd->dwCustomItem & CDRF_NOTIFYSUBITEMDRAW);
		_ASSERT(plvfd->nmcd.iSubItem == plvfd->pitem->iSubItem);
		CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_SUBITEM | CDDS_ITEMPOSTPAINT, &plvfd->nmcd.nmcd);
	}

	if ((plvfd->dwCustomItem | plvfd->dwCustomSubItem) & CDRF_NEWFONT) // App changed font, so
	{
		SelectFont(plvfd->nmcd.nmcd.hdc, plv->hfontLabel);    // restore default font
	}

	if (!(plvfd->dwCustomItem & CDRF_SKIPDEFAULT) &&
		(plvfd->dwCustomItem & CDRF_NOTIFYPOSTPAINT))
	{
		plvfd->nmcd.iSubItem = 0;
		CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_ITEMPOSTPAINT, &plvfd->nmcd.nmcd);
	}
}

void ListView_EndFakeCustomDraw(PLVFAKEDRAW plvfd)
{
	SListViewData *plv = plvfd->plv;

	// notify parent afterwards if they want us to
	if (!(plv->hwnd->ci.dwCustom & CDRF_SKIPDEFAULT) &&
		plv->hwnd->ci.dwCustom & CDRF_NOTIFYPOSTPAINT)
	{
		CIFakeCustomDrawNotify(&plv->hwnd->ci, CDDS_POSTPAINT, &plvfd->nmcd.nmcd);
	}

	// Restore previous state
	plv->hwnd->ci.dwCustom = plvfd->dwCustomPrev;

	SelectObject(plvfd->nmcd.nmcd.hdc, plvfd->hfontPrev);
	ReleaseDC(GetHostWnd(plv->hwnd), plvfd->nmcd.nmcd.hdc);
}


BOOL PASCAL hasVertScroll
(
	SListViewData *plv
)
{
	RECT rcClient;
	RECT rcBounds;
	int cColVis;
	BOOL fHorSB;

	// Get the horizontal bounds of the items.
	ListView_GetClientRect(plv, &rcClient, FALSE, NULL);
	ListView_RGetRects(plv, 0, NULL, NULL, &rcBounds, NULL);
	fHorSB = (rcBounds.right - rcBounds.left > rcClient.right);
	cColVis = (rcClient.bottom - plv->yTop -
			(fHorSB ? ListView_GetCyScrollbar(plv) : 0)) / plv->cyItem;

	// check to see if we need a vert scrollbar
	if ((int)cColVis < ListView_Count(plv))
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

BOOL ListView_RSetColumnWidth(SListViewData *plv, int iCol, int cx)
{
	HD_ITEM item;
	HD_ITEM colitem;

	SIZE    siz;

	LVITEMEX lviItem;
	int     i;
	int     ItemWidth = 0;
	int     HeaderWidth = 0;
	TCHAR   szLabel[CCHLABELMAX + 4];      // CCHLABLEMAX == MAX_PATH
	int     iBegin;
	int     iEnd;

	// Should we compute the width based on the widest string?
	// If we do, include the Width of the Label, and if this is the
	// Last column, set the width so the right side is at the list view's right edge
	if (cx <= LVSCW_AUTOSIZE)
	{
		LVFAKEDRAW lvfd;                    // in case client uses customdraw

		if (cx == LVSCW_AUTOSIZE_USEHEADER)
		{
			// Special Cases:
			// 1) There is only 1 column.  Set the width to the width of the listview
			// 2) This is the rightmost column, set the width so the right edge of the
			//    column coinsides with to right edge of the list view.

			if (plv->cCol == 1)
			{
				RECT    rcClient;

				ListView_GetClientRect(plv, &rcClient, FALSE, NULL);
				HeaderWidth = rcClient.right - rcClient.left;
			}
			else if (iCol == (plv->cCol - 1))
			{
				// BUGBUG  This will only work if the listview as NOT
				// been previously horizontally scrolled
				RECT    rcClient;
				RECT    rcHeader;

				ListView_GetClientRect(plv, &rcClient, FALSE, NULL);
				if (!ColumnHeader_GetItemRect(plv->hwndHdr, plv->cCol - 2, &rcHeader))
				{
					rcHeader.right = 0;
				}

				// Is if visible
				if (rcHeader.right < (rcClient.right - rcClient.left))
				{
					HeaderWidth = (rcClient.right - rcClient.left) - rcHeader.right;
				}
			}

			// If we have a header width, then is is one of these special ones, so
			// we need to account for a vert scroll bar since we are using Client values
			if (HeaderWidth && hasVertScroll(plv))
			{
				HeaderWidth -= IUIGetGloabelVar()->g_cxVScroll;
			}

			// Get the Width of the label.
			// We assume that the app hasn't changed any attributes
			// of the header control - still has default font, margins, etc.
			colitem.mask = HDI_TEXT | HDI_FORMAT;
			colitem.pszText = szLabel;
			colitem.cchTextMax = ARRAYSIZE(szLabel);
			if (ColumnHeader_GetItem(plv->hwndHdr, iCol, &colitem))
			{
				HDC hdc = GetDC(GetHostWnd(plv->hwnd));
				HFONT hfPrev = SelectFont(hdc, plv->hfontLabel);

				GetTextExtentPoint(hdc, colitem.pszText,
					lstrlen(colitem.pszText), &siz);
				siz.cx += 2 * (3 * IUIGetGloabelVar()->g_cxLabelMargin);    // phd->iTextMargin
				if (colitem.fmt & HDF_IMAGE)
				{
					siz.cx += plv->cxSmIcon;
					siz.cx += 2 * (3 * IUIGetGloabelVar()->g_cxLabelMargin);    // pdh->iBmMargin
				}

				HeaderWidth = max(HeaderWidth, siz.cx);

				SelectFont(hdc, hfPrev);
				ReleaseDC(GetHostWnd(plv->hwnd), hdc);
			}
		}


		iBegin = 0;
		iEnd = ListView_Count(plv);

		//
		// Loop for each item in the view
		//
		if (ListView_IsOwnerData(plv))
		{
			iBegin = (int)((plv->ptlRptOrigin.y - plv->yTop)
					/ plv->cyItem);
			iEnd = (int)((plv->ptlRptOrigin.y + plv->sizeClient.cy - plv->yTop)
					/ plv->cyItem) + 1;

			iBegin = max(0, iBegin);
			iEnd = max(iEnd, iBegin + 1);
			iEnd = min(iEnd, ListView_Count(plv));

			ListView_NotifyCacheHint(plv, iBegin, iEnd - 1);
		}

		//
		//  To obtain the widths of the strings, we have to pretend that
		//  we are painting them, in case the custom-draw client wants
		//  to play with fonts (e.g., Athena).
		//
		ListView_BeginFakeCustomDraw(plv, &lvfd, &lviItem);

		//
		//  If column 0, then we also need to take indent into account.
		//
		lviItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		if (iCol == 0)
		{
			lviItem.mask |= LVIF_INDENT;
		}

		// Loop for each item in the List
		for (i = iBegin; i < iEnd; i++)
		{
			lviItem.iImage = -1;
			lviItem.iItem = i;
			lviItem.iSubItem = iCol;
			lviItem.pszText = szLabel;
			lviItem.cchTextMax = ARRAYSIZE(szLabel);
			lviItem.iIndent = 0;
			lviItem.stateMask = 0;
			ListView_OnGetItem(plv, &lviItem);

			// If there is a Text item, get its width
			if (lviItem.pszText || (lviItem.iImage != -1))
			{
				if (lviItem.pszText)
				{

					ListView_BeginFakeItemDraw(&lvfd);

					GetTextExtentPoint(lvfd.nmcd.nmcd.hdc, lviItem.pszText,
						lstrlen(lviItem.pszText), &siz);

					ListView_EndFakeItemDraw(&lvfd);

				}
				else
				{
					siz.cx = 0;
				}

				if (lviItem.iImage != -1)
				{
					siz.cx += plv->cxSmIcon + GetBorderRect(plv->hwnd).left;
				}
				siz.cx += lviItem.iIndent * plv->cxSmIcon;
				ItemWidth = max(ItemWidth, siz.cx);
			}
		}

		ListView_EndFakeCustomDraw(&lvfd);

		// Adjust by a reasonable border amount.
		// If col 0, add 2*g_cxLabelMargin + g_szSmIcon.
		// Otherwise add 6*g_cxLabelMargin.
		// These amounts are based on Margins added automatically
		// to the ListView in ShDrawText.

		// BUGBUG ListView Report format currently assumes and makes
		// room for a Small Icon.
		if (iCol == 0)
		{
			ItemWidth += plv->cxState + GetBorderRect(plv->hwnd).left;
			ItemWidth += 2 * IUIGetGloabelVar()->g_cxLabelMargin;
		}
		else
		{
			ItemWidth += 6 * IUIGetGloabelVar()->g_cxLabelMargin;
		}

		TraceMsg(TF_LISTVIEW, "ListView: HeaderWidth:%d ItemWidth:%d", HeaderWidth, ItemWidth);
		item.cxy = max(HeaderWidth, ItemWidth);
	}
	else
	{
		// Use supplied width
		item.cxy = cx;
	}
	plv->xTotalColumnWidth = RECOMPUTE;

	item.mask = HDI_WIDTH;
	return ColumnHeader_SetItem(plv->hwndHdr, iCol, &item);
}

#ifdef UNICODE
BOOL ListView_OnGetColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	BOOL fRet;

	//HACK ALERT -- this code assumes that LV_COLUMNA is exactly the same
	// as LV_COLUMNW except for the pointer to the string.
	_ASSERT(sizeof(LV_COLUMNA) == sizeof(LV_COLUMNW));

	if (!pcol)
	{
		return FALSE;
	}

	if ((pcol->mask & LVCF_TEXT) && (pcol->pszText != NULL))
	{
		pszC = pcol->pszText;
		pszW = (LPWSTR)LocalAlloc(LMEM_FIXED, pcol->cchTextMax * sizeof(WCHAR));
		if (pszW == NULL)
		{
			return FALSE;
		}
		pcol->pszText = (LPSTR)pszW;
	}

	fRet = ListView_OnGetColumn(plv, iCol, (LV_COLUMN *)pcol);

	if (pszW != NULL)
	{
		if (fRet && pcol->cchTextMax)
		{
			ConvertWToAN(plv->hwnd->ci.uiCodePage, pszC, pcol->cchTextMax, pszW, -1);
		}
		pcol->pszText = pszC;

		LocalFree(pszW);
	}

	return fRet;

}
#endif

BOOL ListView_OnGetColumn(SListViewData *plv, int iCol, LV_COLUMN *pcol)
{
	HD_ITEM item;
	UINT mask;

	if (!pcol)
	{
		RIPMSG(0, "LVM_GETCOLUMN: Invalid pcol = NULL");
		return FALSE;
	}

	mask = pcol->mask;

	if (!mask)
	{
		return TRUE;
	}

	item.mask = HDI_FORMAT | HDI_WIDTH | HDI_LPARAM | HDI_ORDER | HDI_IMAGE;

	if (mask & LVCF_TEXT)
	{
		if (pcol->pszText)
		{
			item.mask |= HDI_TEXT;
			item.pszText = pcol->pszText;
			item.cchTextMax = pcol->cchTextMax;
		}
		else
		{
			// For compatibility reasons, we don't fail the call if they
			// pass NULL.
			RIPMSG(0, "LVM_GETCOLUMN: Invalid pcol->pszText = NULL");
		}
	}

	if (!ColumnHeader_GetItem(plv->hwndHdr, iCol, &item))
	{
		RIPMSG(0, "LVM_GETCOLUMN: Invalid column number %d", iCol);
		return FALSE;
	}

	if (mask & LVCF_SUBITEM)
	{
		pcol->iSubItem = (int)item.lParam;
	}

	if (mask & LVCF_ORDER)
	{
		pcol->iOrder = (int)item.iOrder;
	}

	if (mask & LVCF_IMAGE)
	{
		pcol->iImage = item.iImage;
	}

	if (mask & LVCF_FMT)
	{
		pcol->fmt = item.fmt;
	}

	if (mask & LVCF_WIDTH)
	{
		pcol->cx = item.cxy;
	}

	return TRUE;
}

#ifdef UNICODE
BOOL ListView_OnSetColumnA(SListViewData *plv, int iCol, LV_COLUMNA *pcol)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	BOOL fRet;

	//HACK ALERT -- this code assumes that LV_COLUMNA is exactly the same
	// as LV_COLUMNW except for the pointer to the string.
	_ASSERT(sizeof(LV_COLUMNA) == sizeof(LV_COLUMNW));

	if (!pcol)
	{
		return FALSE;
	}

	if ((pcol->mask & LVCF_TEXT) && (pcol->pszText != NULL))
	{
		pszC = pcol->pszText;
		if ((pszW = ProduceWFromA(plv->hwnd->ci.uiCodePage, pszC)) == NULL)
		{
			return FALSE;
		}
		pcol->pszText = (LPSTR)pszW;
	}

	fRet = ListView_OnSetColumn(plv, iCol, (const LV_COLUMN *)pcol);

	if (pszW != NULL)
	{
		pcol->pszText = pszC;

		FreeProducedString(pszW);
	}

	return fRet;

}
#endif

BOOL ListView_OnSetColumn(SListViewData *plv, int iCol, const LV_COLUMN *pcol)
{
	HD_ITEM item;
	UINT mask;

	if (!pcol)
	{
		return FALSE;
	}

	mask = pcol->mask;
	if (!mask)
	{
		return TRUE;
	}

	item.mask = 0;
	if (mask & LVCF_SUBITEM)
	{
		item.mask |= HDI_LPARAM;
		item.lParam = iCol;
	}

	if (mask & LVCF_FMT)
	{
		item.mask |= HDI_FORMAT;
		item.fmt = (pcol->fmt | HDF_STRING);
	}

	if (mask & LVCF_WIDTH)
	{
		item.mask |= HDI_WIDTH;
		item.cxy = pcol->cx;
	}

	if (mask & LVCF_TEXT)
	{
		RIPMSG(pcol->pszText != NULL, "LVM_SETCOLUMN: LV_COLUMN.pszText should not be NULL");

		item.mask |= HDI_TEXT;
		item.pszText = pcol->pszText;
		item.cchTextMax = 0;
	}

	if (mask & LVCF_IMAGE)
	{
		item.mask |= HDI_IMAGE;
		item.iImage = pcol->iImage;
	}

	if (mask & LVCF_ORDER)
	{
		item.mask |= HDI_ORDER;
		item.iOrder = pcol->iOrder;
	}


	plv->xTotalColumnWidth = RECOMPUTE;
	return ColumnHeader_SetItem(plv->hwndHdr, iCol, &item);
}

BOOL ListView_SetSubItem(SListViewData *plv, const LVITEMEX *plvi)
{
	LISTSUBITEM lsi;
	BOOL fChanged = FALSE;
	int i;
	int idpa;
	HDPA hdpa;

	if (plvi->mask & ~(LVIF_DI_SETITEM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_BINDWL))
	{
		RIPMSG(0, "ListView: Invalid mask: %04x", plvi->mask);
		return FALSE;
	}

	if (!(plvi->mask & (LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_BINDWL)))
	{
		return TRUE;
	}

	i = plvi->iItem;
	if (!ListView_IsValidItemNumber(plv, i))
	{
		RIPMSG(0, "LVM_SETITEM: Invalid iItem: %d", plvi->iItem);
		return FALSE;
	}

	// sub item indices are 1-based...
	//
	idpa = plvi->iSubItem - 1;
	if (idpa < 0 || idpa >= plv->cCol - 1)
	{
		RIPMSG(0, "LVM_SETITEM: Invalid iSubItem: %d", plvi->iSubItem);
		return FALSE;
	}

	hdpa = ListView_GetSubItemDPA(plv, idpa);
	if (hdpa == NULL)
	{
		hdpa = IUI_DPA_CreateEx(LV_HDPA_GROW, plv->hheap);
		if (!hdpa)
		{
			return FALSE;
		}

		IUI_DPA_SetPtr(plv->hdpaSubItems, idpa, (void *)hdpa);
	}

	ListView_GetSubItem(plv, i, plvi->iSubItem, &lsi);

	if (plvi->mask & LVIF_TEXT)
	{
		if (lsi.pszText != plvi->pszText)
		{
			Str_Set(&lsi.pszText, plvi->pszText);
			fChanged = TRUE;
		}
	}

	if (plvi->mask & LVIF_IMAGE)
	{
		if (plvi->iImage != lsi.iImage)
		{
			lsi.iImage = (short)plvi->iImage;
			fChanged = TRUE;
		}
	}

	if (plvi->mask & LVIF_STATE)
	{
		DWORD dwChange;

		dwChange = (lsi.state ^ plvi->state) & plvi->stateMask;

		if (dwChange)
		{
			lsi.state ^= dwChange;
			fChanged = TRUE;
		}
	}

	if (plvi->mask & LVIF_BINDWL)
	{
		if (plvi->hBindWindowless != lsi.hBindWindowless)
		{
			lsi.hBindWindowless = plvi->hBindWindowless;

			SetBindItemControlParent(lsi.hBindWindowless, plv->hwnd, lsi.m_dwSubItemData);

			fChanged = TRUE;
		}
	}

	if (fChanged)
	{
		LISTSUBITEM *plsiReal = (LISTSUBITEM *)IUI_DPA_GetPtr(hdpa, i);
		if (!plsiReal)
		{
			plsiReal = (LISTSUBITEM *)LocalAlloc(LPTR, sizeof(LISTSUBITEM));
			if (!plsiReal)
			{
				// fail!  bail out
				return FALSE;
			}
		}
		*plsiReal = lsi;
		if (!IUI_DPA_SetPtr(hdpa, i, (void *)plsiReal))
		{

			ListView_FreeSubItem(plsiReal);
			return FALSE;
		}
	}

	// all's well... let's invalidate this
	if (ListView_IsReportView(plv))
	{
		RECT rc;
		ListView_RGetRectsEx(plv, plvi->iItem, plvi->iSubItem, NULL, &rc);
		RedrawWindow(GetHostWnd(plv->hwnd), &rc, NULL, RDW_ERASE | RDW_INVALIDATE);
	}
	return TRUE;
}

// 必须加WINAPI修饰，否则枚举函数无法退出
int WINAPI ListView_RDestroyColumn(LPVOID d, LPVOID p)
{
	HDPA hdpa = (HDPA)d;
	IUI_DPA_DestroyCallback(hdpa, (PFNDAENUMCALLBACK)ListView_FreeColumnData, 0);
	return 1;
}

void ListView_RDestroy(SListViewData *plv)
{
	IUI_DPA_DestroyCallback(plv->hdpaSubItems, (PFNDAENUMCALLBACK)ListView_RDestroyColumn, 0);
	plv->hdpaSubItems = NULL;
}

VOID ListView_RHeaderTrack(SListViewData *plv, HD_NOTIFY *pnm)
{
	// We want to update to show where the column header will be.
	HDC hdc;
	RECT rcBounds;

	// Statics needed from call to call
	static int s_xLast = -32767;

	hdc = GetDC(GetHostWnd(plv->hwnd));
	if (hdc == NULL)
	{
		return;
	}

	//
	// First undraw the last marker we drew.
	//
	if (s_xLast > 0)
	{
		PatBlt(hdc, s_xLast, plv->yTop, IUIGetGloabelVar()->g_cxBorder, plv->sizeClient.cy - plv->yTop, PATINVERT);
	}

	if (pnm->hdr.code == HDN_ENDTRACK)
	{
		s_xLast = -32767;       // Some large negative number...
	}
	else
	{

		RECT rc;

		//
		// First we need to calculate the X location of the column
		// To do this, we will need to know where this column begins
		// Note: We need the bounding rects to help us know the origin.
		ListView_GetRects(plv, 0, NULL, NULL, &rcBounds, NULL);

		if (!ColumnHeader_GetItemRect(plv->hwndHdr, pnm->iItem, &rc))
		{
			rc.left = 0;
		}
		rcBounds.left += rc.left;

		// Draw the new line...
		s_xLast = rcBounds.left + pnm->pitem->cxy;
		PatBlt(hdc, s_xLast, plv->yTop, IUIGetGloabelVar()->g_cxBorder, plv->sizeClient.cy - plv->yTop, PATINVERT);
	}

	ReleaseDC(GetHostWnd(plv->hwnd), hdc);
}

void PASCAL ListView_AdjustColumn(SListViewData *plv, int iWidth)
{
	int x;
	RECT rcClip;
	int dx = iWidth - plv->iSelOldWidth;

	if (iWidth == plv->iSelOldWidth)
	{
		return;
	}

	// find the x coord of the left side of the iCol
	// use rcClip as a temporary...
	if (!ColumnHeader_GetItemRect(plv->hwndHdr, plv->iSelCol, &rcClip))
	{
		x = 0;
	}
	else
	{
		x = rcClip.left;
	}
	x -= plv->ptlRptOrigin.x;

	// compute the area to the right of the adjusted column
	GetWindowRect(plv->hwndHdr, &rcClip);

	rcClip.left = x;
	rcClip.top = RECTHEIGHT(rcClip);
	rcClip.right = plv->sizeClient.cx;
	rcClip.bottom = plv->sizeClient.cy;

	if ((plv->pImgCtx == NULL) && (plv->clrBk != CLR_NONE) &&
		(plv->clrTextBk != CLR_NONE))
	{
		//
		// We have a solid color background,
		// so we can smooth scroll the right side columns.
		//
		rcClip.left += min(plv->iSelOldWidth, iWidth);

		// if we shrunk, invalidate the right most edge because
		// there might be junk there
		if (iWidth < plv->iSelOldWidth)
		{
			rcClip.right = rcClip.left + GetBorderRect(plv->hwnd).left;
			InvalidateRect(plv->hwnd, &rcClip);
		}

		plv->xTotalColumnWidth = RECOMPUTE;

		// adjust clipping rect to only redraw the adjusted column
		rcClip.left = x;
		rcClip.right = max(rcClip.left, x + iWidth);

		// Make the rectangle origin-based because ListView_UpdateScrollBars
		// may scroll us around.
		OffsetRect(&rcClip, plv->ptlRptOrigin.x, plv->ptlRptOrigin.y);

		ListView_UpdateScrollBars(plv);

		// Okay, now convert it back to client coordinates
		OffsetRect(&rcClip, -plv->ptlRptOrigin.x, -plv->ptlRptOrigin.y);

		InvalidateRect(plv->hwnd, &rcClip);
		//L RedrawWindow(GetHostWnd(plv->hwnd), &rcClip, NULL,
		//	RDW_INVALIDATE | RDW_UPDATENOW);
	}
	else
	{
		//
		// We don't have a solid color background,
		// erase and redraw the adjusted column and
		// everything to the right (sigh).
		//
		plv->xTotalColumnWidth = RECOMPUTE;
		ListView_UpdateScrollBars(plv);

		rcClip.left = x;
		RedrawWindow(GetHostWnd(plv->hwnd), &rcClip, NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}
}

BOOL ListView_ForwardHeaderNotify(SListViewData *plv, HD_NOTIFY *pnm)
{
	RoutedEventArgs arg;
	arg.hSender = plv->hwnd;
	arg.hOriginalSender = plv->hwnd;
	arg.RoutedEvent = pnm->hdr.code;
	arg.eRoutingStrategy = RS_BUBBLE;
	return RaiseEvent(arg.hOriginalSender, &arg);
	//L 	return BOOLFROMPTR(SendNotifyEx(plv->hwnd->ci.hwndParent, pnm->hdr.hwndFrom, pnm->hdr.code,
	// 				(NMHDR *)pnm, plv->hwnd->ci.bUnicode));
}

LRESULT ListView_HeaderNotify(SListViewData *plv, HD_NOTIFY *pnm)
{
	LRESULT lres = 0;
	switch (pnm->hdr.code)
	{
	case HDN_BEGINDRAG:
		if (!(plv->exStyle & LVS_EX_HEADERDRAGDROP))
		{
			return TRUE;
		}

		return ListView_ForwardHeaderNotify(plv, pnm);

	case HDN_ENDDRAG:
		if (pnm->pitem->iOrder != -1)
		{
			InvalidateRect(GetHostWnd(plv->hwnd), NULL, TRUE);
			return ListView_ForwardHeaderNotify(plv, pnm);
		}
		goto DoDefault;

	case HDN_ITEMCHANGING:
		if (pnm->pitem->mask & HDI_WIDTH)
		{
			HD_ITEM hitem;

			hitem.mask = HDI_WIDTH;
			ColumnHeader_GetItem(plv->hwndHdr, pnm->iItem, &hitem);
			plv->iSelCol = pnm->iItem;
			plv->iSelOldWidth = hitem.cxy;
			TraceMsg(TF_LISTVIEW, "HDN_ITEMCHANGING %d %d", hitem.cxy, pnm->pitem->cxy);
			return ListView_ForwardHeaderNotify(plv, pnm);
		}
		else if (pnm->pitem->mask & HDI_FILTER)
		{
			return ListView_ForwardHeaderNotify(plv, pnm);
		}
		goto DoDefault;

	// Header调整Item宽度时，会持续收到HDN_ITEMCHANGED通知
	case HDN_ITEMCHANGED:
		if (pnm->pitem->mask & HDI_WIDTH)
		{
			ListView_DismissEdit(plv, FALSE);
			if (pnm->iItem == plv->iSelCol)
			{
				// Must do this even if there are no items, since
				// we have to redo the scrollbar, and the client
				// may have custom-drawn gridlines or something.
				ListView_AdjustColumn(plv, pnm->pitem->cxy);
			}
			else
			{
				// sanity check.  we got screwed, so redraw all
				Invalidate(plv->hwnd);
			}
			plv->iSelCol = -1;
			lres = ListView_ForwardHeaderNotify(plv, pnm);
		}
		else if (pnm->pitem->mask & HDI_FILTER)
		{
			lres = ListView_ForwardHeaderNotify(plv, pnm);
		}
		else
		{
			goto DoDefault;
		}
		break;


	case HDN_ITEMCLICK:
		//
		// BUGBUG:: Need to pass this and other HDN_ notifications back to
		// parent.  Should we simply pass up the HDN notifications
		// or should we define equivlent LVN_ notifications...
		//
		// Pass column number in iSubItem, not iItem...
		//
		ListView_DismissEdit(plv, FALSE);
		ListView_Notify(plv, -1, pnm->iItem, LVN_COLUMNCLICK);
		lres = ListView_ForwardHeaderNotify(plv, pnm);
		SetFocus(GetHostWnd(plv->hwnd), plv->hwnd);
		break;

	case HDN_TRACK:
	case HDN_ENDTRACK:
		ListView_DismissEdit(plv, FALSE);
		ListView_RHeaderTrack(plv, pnm);
		lres = ListView_ForwardHeaderNotify(plv, pnm);
		SetFocus(GetHostWnd(plv->hwnd), plv->hwnd);
		break;

	case HDN_DIVIDERDBLCLICK:
		ListView_DismissEdit(plv, FALSE);
		ListView_RSetColumnWidth(plv, pnm->iItem, -1);
		lres = ListView_ForwardHeaderNotify(plv, pnm);
		SetFocus(GetHostWnd(plv->hwnd), plv->hwnd);
		break;

	case HDN_FILTERCHANGE:
	case HDN_FILTERBTNCLICK:
		return ListView_ForwardHeaderNotify(plv, pnm);

	case NM_RCLICK:
	{
		RoutedEventArgs arg;
		arg.hSender = plv->hwnd;
		arg.hOriginalSender = plv->hwnd;
		arg.RoutedEvent = NM_RCLICK;
		arg.eRoutingStrategy = RS_BUBBLE;
		return RaiseEvent(arg.hOriginalSender, &arg);

		//L return (UINT)SendNotifyEx(plv->hwnd->ci.hwndParent, plv->hwndHdr, NM_RCLICK, NULL, plv->hwnd->ci.bUnicode);
	}

	default:
DoDefault:
		if (plv->hwnd->ci.iVersion >= 5)
		{
			return ListView_ForwardHeaderNotify(plv, pnm);
		}
		break;
	}

	// in v < 5 we always returned 0
	// but for newer clients we'd like to have them deal with the notify
	if (plv->hwnd->ci.iVersion >= 5)
	{
		return lres;
	}
	return 0;
}

/*----------------------------------------------------------------
** Check for a hit in a report view.
**
** a hit only counts if it's on the icon or the string in the first
** column.  so we gotta figure out what this means exactly.  yuck.
**
** BONUS FEATURE:  If piSubItem is non-NULL, then we also hit-test
** against subitems.  But if we find nothing, we return iSubItem = 0
** for compatibility with the other hit-test functions.
**----------------------------------------------------------------*/
int ListView_RItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem)
{
	int iHit;
	int i, iSub;
	UINT flags;
	RECT rcLabel;
	RECT rcIcon;

	if (piSubItem)
	{
		*piSubItem = 0;
	}

	flags = LVHT_NOWHERE;
	iHit = -1;

	i = ListView_RYHitTest(plv, y);
	if (ListView_IsValidItemNumber(plv, i))
	{
		if (plv->hwnd->ci.style & LVS_OWNERDRAWFIXED)
		{
			flags = LVHT_ONITEM;
			iHit = i;
		}
		else
		{
			RECT rcSelect;
			ListView_GetRects(plv, i, &rcIcon, &rcLabel, NULL, &rcSelect);

			// is the hit in the first column?
			if ((x < rcIcon.left - GetBorderRect(plv->hwnd).left) && x > (rcIcon.left - plv->cxState))
			{
				iHit = i;
				flags = LVHT_ONITEMSTATEICON;
			}
			else if ((x >= rcIcon.left) && (x < rcIcon.right))
			{
				iHit = i;
				flags = LVHT_ONITEMICON;
			}
			else if (x >= rcLabel.left && (x < rcSelect.right))
			{
				iHit = i;
				flags = LVHT_ONITEMLABEL;

				if (ListView_FullRowSelect(plv))
				{
					// this is kinda funky...  in full row select mode
					// we're only really on the label if x is <= rcLabel.left + cxLabel
					// because GetRects returns a label rect of the full column width
					// and rcSelect has the full row in FullRowSelect mode
					// (it has the label only width in non-fullrow select mode.
					//
					// go figure..
					//
					int cxLabel;
					LISTITEM *pitem = NULL;

					if (!ListView_IsOwnerData(plv))
					{
						pitem = ListView_FastGetItemPtr(plv, i);
					}
					cxLabel = ListView_RGetCXLabel(plv, i, pitem, NULL, FALSE);

					if (x >= min(rcLabel.left + cxLabel, rcLabel.right))
					{
						if (!piSubItem)
						{
							flags = LVHT_ONITEM;
						}
						else
						{
							goto CheckSubItem;
						}
					}
				}
			}
			else if (x < rcSelect.right && ListView_FullRowSelect(plv))
			{
				// we can fall into this case if columns have been re-ordered
				iHit = i;
				flags = LVHT_ONITEM;
			}
			else if (piSubItem)
			{
CheckSubItem:
				iSub = ListView_RXHitTest(plv, x);
				if (iSub >= 0)
				{
					iHit = i;
					*piSubItem = iSub;
					// Flags still say LVHT_NOWHERE
				}
			}
		}
	}

	*pflags = flags;
	return iHit;
}

void ListView_GetSubItem(SListViewData *plv, int i, int iSubItem, LISTSUBITEM *plsi)
{
	HDPA hdpa;
	LISTSUBITEM *plsiSrc = NULL;

	_ASSERT(!ListView_IsOwnerData(plv));

	// Sub items are indexed starting at 1...
	//
	RIPMSG(iSubItem > 0 && iSubItem < plv->cCol, "ListView: Invalid iSubItem: %d", iSubItem);

#ifdef DEBUG
	// Avoid the assert in DPA_GetPtr if somebdy tries to get a subitem
	// when no columns have been added.  We already RIP'd above.
	hdpa = plv->cCol ? ListView_GetSubItemDPA(plv, iSubItem - 1) : NULL;
#else
	hdpa = ListView_GetSubItemDPA(plv, iSubItem - 1);
#endif
	if (hdpa != NULL)
	{
		plsiSrc = (LISTSUBITEM *)IUI_DPA_GetPtr(hdpa, i);
	}


	if (plsiSrc != NULL)
	{
		*plsi = *plsiSrc;
	}
	else
	{

		// item data exists.. give defaults
		plsi->pszText = LPSTR_TEXTCALLBACK;
		plsi->iImage = I_IMAGECALLBACK;
		plsi->state = 0;
		plsi->hBindWindowless = NULL;
	}
}

LPTSTR ListView_RGetItemText(SListViewData *plv, int i, int iSubItem)
{
	LISTSUBITEM lsi;

	ListView_GetSubItem(plv, i, iSubItem, &lsi);
	return lsi.pszText;
}

// this will return the rect of a subitem as requested.
void ListView_RGetRectsEx(SListViewData *plv, int iItem, int iSubItem, LPRECT prcIcon, LPRECT prcLabel)
{
	int x;
	int y;
	LONG ly;
	RECT rcLabel;
	RECT rcIcon;
	RECT rcHeader;

	if (iSubItem == 0)
	{
		ListView_RGetRects(plv, iItem, prcIcon, prcLabel, NULL, NULL);
		return;
	}

	// otherwise it's just the header's column right and left and the item's height
	ly = (LONG)iItem * plv->cyItem - plv->ptlRptOrigin.y + plv->yTop;
	x = -(int)plv->ptlRptOrigin.x;

	//
	// Need to check for y overflow into rectangle structure
	// if so we need to return something reasonable...
	// For now will simply set it to the max or min that will fit...
	//
	if (ly >= (INT_MAX - plv->cyItem))
	{
		y = INT_MAX - plv->cyItem;
	}
	else if (ly < INT_MIN)
	{
		y = INT_MIN;
	}
	else
	{
		y = (int)ly;
	}

	_ASSERT(iSubItem < plv->cCol);
	ColumnHeader_GetItemRect(plv->hwndHdr, iSubItem, &rcHeader);

	rcLabel.left = x + rcHeader.left;
	rcLabel.right = x + rcHeader.right;
	rcLabel.top = y;
	rcLabel.bottom = rcLabel.top + plv->cyItem;

	rcIcon = rcLabel;
	rcIcon.right = rcIcon.left + plv->cxSmIcon;

	if (SELECTOROF(prcIcon))
	{
		*prcIcon = rcIcon;
	}
	if (SELECTOROF(prcLabel))
	{
		*prcLabel = rcLabel;
	}
}

int ListView_RGetTotalColumnWidth(SListViewData *plv)
{
	if (plv->xTotalColumnWidth == RECOMPUTE)
	{
		plv->xTotalColumnWidth = 0;
		if (plv->cCol)
		{
			RECT rcLabel;
			int iIndex;

			// find the right edge of the last ordered item to get the total column width
			iIndex = (int)SendMessage(plv->hwndHdr, HDM_ORDERTOINDEX, plv->cCol - 1, 0);
			ColumnHeader_GetItemRect(plv->hwndHdr, iIndex, &rcLabel);
			plv->xTotalColumnWidth = rcLabel.right;
		}
	}
	return plv->xTotalColumnWidth;
}

// get the rects for report view
void ListView_RGetRects(SListViewData *plv, int iItem, RECT *prcIcon,
	RECT *prcLabel, RECT *prcBounds, RECT *prcSelectBounds)
{
	RECT rcIcon;
	RECT rcLabel;
	int x;
	int y;
	LONG ly;
	LVITEMEX lvitem;
	BOOL fItemSpecific = (prcIcon || prcLabel || prcSelectBounds);

	// use long math for cases where we have lots-o-items

	ly = (LONG)iItem * plv->cyItem - plv->ptlRptOrigin.y + plv->yTop;
	x = -(int)plv->ptlRptOrigin.x;

	//
	// Need to check for y overflow into rectangle structure
	// if so we need to return something reasonable...
	// For now will simply set it to the max or min that will fit...
	//
	if (ly >= (INT_MAX - plv->cyItem))
	{
		y = INT_MAX - plv->cyItem;
	}
	else
	{
		y = (int)ly;
	}


	if (ListView_Count(plv) && fItemSpecific)
	{
		//  move this over by the indent level as well
		lvitem.mask = LVIF_INDENT;
		lvitem.iItem = iItem;
		lvitem.iSubItem = 0;
		ListView_OnGetItem(plv, &lvitem);
	}
	else
	{
		lvitem.iIndent = 0;
	}

	rcIcon.left = x + plv->cxState + (lvitem.iIndent * plv->cxSmIcon) + GetBorderRect(plv->hwnd).left;
	rcIcon.right = rcIcon.left + plv->cxSmIcon;
	rcIcon.top = y;
	rcIcon.bottom = rcIcon.top + plv->cyItem;

	rcLabel.left = rcIcon.right;
	rcLabel.top = rcIcon.top;
	rcLabel.bottom = rcIcon.bottom;

	//
	// The label is assumed to be the first column.
	//
	rcLabel.right = x;
	if (plv->cCol > 0 && fItemSpecific)
	{
		RECT rc;
		ColumnHeader_GetItemRect(plv->hwndHdr, 0, &rc);
		rcLabel.right = x + rc.right;
		rcLabel.left += rc.left;
		rcIcon.left += rc.left;
		rcIcon.right += rc.left;
	}

	if (SELECTOROF(prcIcon))
	{
		*prcIcon = rcIcon;
	}

	// Save away the label bounds.
	if (SELECTOROF(prcLabel))
	{
		*prcLabel = rcLabel;
	}

	// See if they also want the Selection bounds of the item
	if (prcSelectBounds)
	{
		if (ListView_FullRowSelect(plv))
		{

			prcSelectBounds->left = x;
			prcSelectBounds->top = y;
			prcSelectBounds->bottom = rcLabel.bottom;
			prcSelectBounds->right = prcSelectBounds->left + ListView_RGetTotalColumnWidth(plv);

		}
		else
		{
			int cxLabel;
			LISTITEM *pitem = NULL;

			if (!ListView_IsOwnerData(plv))
			{
				pitem = ListView_FastGetItemPtr(plv, iItem);
			}
			cxLabel = ListView_RGetCXLabel(plv, iItem, pitem, NULL, FALSE);

			*prcSelectBounds = rcIcon;
			prcSelectBounds->right = rcLabel.left + cxLabel;
			if (prcSelectBounds->right > rcLabel.right)
			{
				prcSelectBounds->right = rcLabel.right;
			}
		}
	}

	// And also the Total bounds

	//
	// and now for the complete bounds...
	//
	if (SELECTOROF(prcBounds))
	{
		prcBounds->left = x;
		prcBounds->top = y;
		prcBounds->bottom = rcLabel.bottom;

		prcBounds->right = prcBounds->left + ListView_RGetTotalColumnWidth(plv);
	}
}

BOOL ListView_OnGetSubItemRect(SListViewData *plv, int iItem, LPRECT lprc)
{
	LPRECT pRects[LVIR_MAX];
	RECT rcTemp;

	int iSubItem;
	int iCode;

	if (!lprc)
	{
		return FALSE;
	}

	iSubItem = lprc->top;
	iCode = lprc->left;

	if (iSubItem == 0)
	{
		return ListView_OnGetItemRect(plv, iItem, lprc);
	}

	if (!ListView_IsReportView(plv) ||
		(iCode != LVIR_BOUNDS && iCode != LVIR_ICON && iCode != LVIR_LABEL))
	{
		return FALSE;
	}

	pRects[0] = NULL;
	pRects[1] = &rcTemp;  // LVIR_ICON
	pRects[2] = &rcTemp;  // LVIR_LABEL
	pRects[3] = NULL;

	if (iCode != LVIR_BOUNDS)
	{
		pRects[iCode] = lprc;
	}
	else
	{
		// choose either
		pRects[LVIR_ICON] = lprc;
	}

	ListView_RGetRectsEx(plv, iItem, iSubItem,
		pRects[LVIR_ICON], pRects[LVIR_LABEL]);

	if (iCode == LVIR_BOUNDS)
	{
		UnionRect(lprc, lprc, &rcTemp);
	}
	return TRUE;
}

int ListView_RXHitTest(SListViewData *plv, int x)
{
	int iSubItem;

	for (iSubItem = plv->cCol - 1; iSubItem >= 0; iSubItem--)
	{
		RECT rc;

		// see if its in this rect,
		if (!ColumnHeader_GetItemRect(plv->hwndHdr, iSubItem, &rc))
		{
			return -1;
		}

		OffsetRect(&rc, -plv->ptlRptOrigin.x, 0);
		if (rc.left <= x && x < rc.right)
		{
			break;
		}
	}
	return iSubItem;
}

int ListView_OnSubItemHitTest(SListViewData *plv, LPLVHITTESTINFO plvhti)
{
	int i = -1;
	int iSubItem = 0;
	UINT uFlags = LVHT_NOWHERE;

	if (!plvhti)
	{
		return -1;
	}

	if (ListView_IsReportView(plv))
	{
		iSubItem = ListView_RXHitTest(plv, plvhti->pt.x);
		if (iSubItem == -1)
		{
			goto Bail;
		}
	}

	if (iSubItem == 0)
	{
		// if we're in column 0, just hand it off to the old stuff
		ListView_OnHitTest(plv, plvhti);
		plvhti->iSubItem = 0;
		return plvhti->iItem;
	}

	if (!ListView_IsReportView(plv))
	{
		goto Bail;
	}

	i = ListView_RYHitTest(plv, plvhti->pt.y);
	if (i < ListView_Count(plv))
	{
		RECT rcIcon, rcLabel;

		if (i != -1)
		{
			ListView_RGetRectsEx(plv, i, iSubItem, &rcIcon, &rcLabel);
			if (plvhti->pt.x >= rcIcon.left && plvhti->pt.x <= rcIcon.right)
			{
				uFlags = LVHT_ONITEMICON;
			}
			else if (plvhti->pt.x >= rcLabel.left && plvhti->pt.x <= rcLabel.right)
			{
				uFlags = LVHT_ONITEMLABEL;
			}
			else
			{
				uFlags = LVHT_ONITEM;
			}
		}
	}
	else
	{
		i = -1;
	}

Bail:

	plvhti->iItem = i;
	plvhti->iSubItem = iSubItem;
	plvhti->flags = uFlags;

	return plvhti->iItem;
}



// BUGBUG: this is duplicate code with all the other views!
// See whether entire string will fit in *prc; if not, compute number of chars
// that will fit, including ellipses.  Returns length of string in *pcchDraw.
//
BOOL ListView_NeedsEllipses(HDC hdc, LPCTSTR pszText, RECT *prc, int *pcchDraw, int cxEllipses)
{
	int cchText;
	int cxRect;
	int ichMin, ichMax, ichMid;
	SIZE siz;
#if !defined(UNICODE)  // && defined(DBCS)
	LPCTSTR lpsz;
#endif

	cxRect = prc->right - prc->left;

	cchText = lstrlen(pszText);

	if (cchText == 0)
	{
		*pcchDraw = cchText;
		return FALSE;
	}

	GetTextExtentPoint(hdc, pszText, cchText, &siz);

	if (siz.cx <= cxRect)
	{
		*pcchDraw = cchText;
		return FALSE;
	}

	cxRect -= cxEllipses;

	// If no room for ellipses, always show first character.
	//
	ichMax = 1;
	if (cxRect > 0)
	{
		// Binary search to find character that will fit
		ichMin = 0;
		ichMax = cchText;
		while (ichMin < ichMax)
		{
			// Be sure to round up, to make sure we make progress in
			// the loop if ichMax == ichMin + 1.
			//
			ichMid = (ichMin + ichMax + 1) / 2;

			GetTextExtentPoint(hdc, &pszText[ichMin], ichMid - ichMin, &siz);

			if (siz.cx < cxRect)
			{
				ichMin = ichMid;
				cxRect -= siz.cx;
			}
			else if (siz.cx > cxRect)
			{
				ichMax = ichMid - 1;
			}
			else
			{
				// Exact match up up to ichMid: just exit.
				//
				ichMax = ichMid;
				break;
			}
		}

		// Make sure we always show at least the first character...
		//
		if (ichMax < 1)
		{
			ichMax = 1;
		}
	}

#if !defined(UNICODE) // && defined(DBCS)
	// b#8934
	lpsz = &pszText[ichMax];
	while (lpsz-- > pszText)
	{
		if (!IsDBCSLeadByte(*lpsz))
		{
			break;
		}
	}
	ichMax += ((&pszText[ichMax] - lpsz) & 1) ? 0 : 1;
#endif

	*pcchDraw = ichMax;
	return TRUE;
}

// 会计算Header的尺寸并移动Header;
// 计算第0个Item的top
// 更新滚动条信息
void ListView_RUpdateScrollBars(SListViewData *plv)
{
	RECT rcBounds;

	// report风格List，能显示Item的垂直高度（List高度减header
	// 高度减水平滚动条高度减Item与Header间距）
	int cyColVis;
	int iNewPos, iyDelta = 0, ixDelta = 0;
	BOOL fHorSB, fReupdate = FALSE;
	SCROLLINFO si;

	RECT rcClient;
	ListView_GetClientRect(plv, &rcClient, FALSE, NULL);

	if (plv->hwndHdr == NULL)
	{
		ListView_CreateHeader(plv);
	}

	if (plv->hwndHdr == NULL)
	{
		TraceMsg(TF_WARNING, "ListView_RUpdateScrollBars could not create hwndHdr");
		return;
	}

	WINDOWPOS wpos;
	HD_LAYOUT layout;
	layout.pwpos = &wpos;
	// For now lets try to handle scrolling the header by setting
	// its window pos.
	rcClient.left -= (int)plv->ptlRptOrigin.x;
	layout.prc = &rcClient;

	// 执行完ColumnHeader_Layout后，rcClient.top表示第0个Item的Top值。
	ColumnHeader_Layout(plv->hwndHdr, &layout);

	rcClient.left += (int)plv->ptlRptOrigin.x;    // Move it back over!

	MoveWindow(plv->hwndHdr, wpos.x, wpos.y, wpos.cx, wpos.cy, TRUE);
	ShowWindow(plv->hwndHdr, SW_SHOW);
	//L SetWindowPos(plv->hwndHdr, wpos.hwndInsertAfter, wpos.x, wpos.y,
	//	wpos.cx, wpos.cy, wpos.flags | SWP_SHOWWINDOW);

	// Get the horizontal bounds of the items.
	ListView_RGetRects(plv, 0, NULL, NULL, &rcBounds, NULL);

	// If v3 or better, take a cyEdge off the top
	if (plv->hwnd->ci.iVersion >= 3)
	{
		rcClient.top += GetBorderRect(plv->hwnd).bottom;
	}

	plv->yTop = rcClient.top + plv->cyItemHeaderSpace; // Header和Item间，留个空隙

	// fHorSB = Do I need a horizontal scrollbar?
	// cyColVis = number of pixels per screenful
	fHorSB = (rcBounds.right - rcBounds.left > rcClient.right);  // First guess.
	cyColVis = rcClient.bottom - rcClient.top - plv->cyItemHeaderSpace -
		(fHorSB ? ListView_GetCyScrollbar(plv) : 0);

	// If screen can't fit the entire listview...
	if (cyColVis < ListView_Count(plv) * plv->cyItem)
	{
		//then we're going to have a vertical scrollbar.. make sure our horizontal count is correct
		rcClient.right -= ListView_GetCxScrollbar(plv);

		if (!fHorSB)
		{
			// if we previously thought we weren't going to have a scrollbar, we could be wrong..
			// since the vertical bar shrunk our area
			fHorSB = (rcBounds.right - rcBounds.left > rcClient.right);  // First guess.
			cyColVis = rcClient.bottom - rcClient.top -
				(fHorSB ? ListView_GetCyScrollbar(plv) : 0);
		}
	}

	// Report风格List垂直方向可容纳完整Item个数。
	int cColVis = cyColVis / plv->cyItem;

	si.cbSize = sizeof(SCROLLINFO);

	si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nPos = (int)(plv->ptlRptOrigin.y / plv->cyItem);
	si.nPage = cColVis;
	si.nMin = 0;
	si.nMax = ListView_Count(plv) - 1;
	ListView_SetScrollInfo(plv, SB_VERT, &si, TRUE);

	// make sure our position and page doesn't hang over max
	if ((si.nPos > (int)si.nMax - (int)si.nPage + 1) && si.nPos > 0)
	{
		iNewPos = (int)si.nMax - (int)si.nPage + 1;
		if (iNewPos < 0)
		{
			iNewPos = 0;
		}
		if (iNewPos != si.nPos)
		{
			iyDelta = iNewPos - (int)si.nPos;
			fReupdate = TRUE;
		}
	}

	si.nPos = (int)plv->ptlRptOrigin.x;
	si.nPage = rcClient.right - rcClient.left;

	// We need to subtract 1 here because nMax is 0 based, and nPage is the actual
	// number of page pixels.  So, if nPage and nMax are the same we will get a
	// horz scroll, since there is 1 more pixel than the page can show, but... rcBounds
	// is like rcRect, and is the actual number of pixels for the whole thing, so
	// we need to set nMax so that: nMax - 0 == rcBounds.right - rcBound.left
	si.nMax = rcBounds.right - rcBounds.left - 1;
	ListView_SetScrollInfo(plv, SB_HORZ, &si, TRUE);

	// SWP_FRAMECHANGED redraws the background if the client
	// area has changed (taking into account scrollbars and
	// the Header window).  SetScrollInfo does this automatically
	// when it creates a scrollbar - we do it ourselves when
	// there is no scrollbar.
	if ((UINT)si.nPage > (UINT)si.nMax &&
		((plv->pImgCtx && plv->fImgCtxComplete) || plv->hbmBkImage))
	{
		SetWindowPos(plv->hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	// 当Header宽度大于ListView宽度时，把ListView水平滚动到最右侧，然后拖动Header最右边那个
	// Item的最右侧分隔线，往窄了调整Header Item 宽度时
	if ((si.nPos + (LONG)si.nPage - 1 > si.nMax) && si.nPos > 0)
	{
		iNewPos = (int)si.nMax - (int)si.nPage + 1;
		if (iNewPos < 0)
		{
			iNewPos = 0;
		}
		if (iNewPos != si.nPos)
		{
			ixDelta = iNewPos - (int)si.nPos;
			fReupdate = TRUE;
		}
	}

	if (fReupdate)
	{
		// we shouldn't recurse because the second time through, si.nPos >0
		ListView_RScroll2(plv, ixDelta, iyDelta, 0);
		ListView_RUpdateScrollBars(plv);
		TraceMsg(TF_LISTVIEW, "LISTVIEW: ERROR: We had to recurse!");
	}
}

//
//  We need a smoothscroll callback so our background image draws
//  at the correct origin.  If we don't have a background image,
//  then this work is superfluous but not harmful either.
//
int CALLBACK ListView_RScroll2_SmoothScroll(SListViewData *plv,
	int dx,
	int dy,
	CONST RECT *prcScroll,
	CONST RECT *prcClip,
	HRGN hrgnUpdate,
	LPRECT prcUpdate,
	UINT flags)
{
	if (plv != NULL)
	{
		plv->ptlRptOrigin.x -= dx;
		plv->ptlRptOrigin.y -= dy;
	}

	// Now do what SmoothScrollWindow would've done if we weren't
	// a callback

	return SIMPLEREGION;//L ScrollWindowEx(hwnd, dx, dy, prcScroll, prcClip, hrgnUpdate, prcUpdate, flags);
}


// 更新ptlRptOrigin。 通过更新ptlRptOrigin，来滚动视图
void ListView_RScroll2(SListViewData *plv, int dx, int dy, UINT uSmooth)
{
	LONG ldy;

	if (dx | dy)
	{
		RECT rc;

		GetClientRect(plv->hwnd, &rc);

		rc.top = plv->yTop;

		// We can not do a simple multiply here as we may run into
		// a case where this will overflow an int..
		ldy = (LONG)dy * plv->cyItem;

		// 更新ptlRptOrigin。 ptlRptOrigin表示ListView内容中哪个点，显示到ListView客户端左上角。
		// 所以，ptlRptOrigin的值，都为正值。

		// handle case where dy is large (greater than int...)
		if ((ldy > rc.bottom) || (ldy < -rc.bottom))
		{
			InvalidateRect(plv->hwnd, NULL);
		}
		plv->ptlRptOrigin.x += dx;
		plv->ptlRptOrigin.y += ldy;

		// 布局ColumnHeader
		if (dx != 0)
		{
			RECT rcHdr;
			GetWindowRect(plv->hwndHdr, &rcHdr);
			ScreenToClient(plv->hwnd, &rcHdr);
			MoveWindow(plv->hwndHdr, rcHdr.left - dx, rcHdr.top,
				rcHdr.right - rcHdr.left + dx,
				rcHdr.bottom - rcHdr.top, TRUE);
		}
	}
}

//-------------------------------------------------------------------
// Make sure that specified item is visible for report view.
// Must handle Large number of items...
BOOL ListView_ROnEnsureVisible(SListViewData *plv, int iItem, BOOL fPartialOK)
{
	LONG dy;
	LONG yTop;
	LONG lyTop;

	yTop = plv->yTop;

	// lyTop = where our item is right now
	lyTop = (LONG)iItem * plv->cyItem - plv->ptlRptOrigin.y + plv->yTop;

	// If visible below yTop and our bottom is visible above client bottom,
	// then we're happy.
	if ((lyTop >= (LONG)yTop) &&
		((lyTop + plv->cyItem) <= (LONG)plv->sizeClient.cy))
	{
		return (TRUE);    // we are visible
	}

	dy = lyTop - yTop;
	if (dy >= 0)
	{
		// dy = how many pixels we need to scroll to come into view
		dy = lyTop + plv->cyItem - plv->sizeClient.cy;
		if (dy < 0)
		{
			dy = 0;
		}
	}

	if (dy)
	{
		int iRound = ((dy > 0) ? 1 : -1) * (plv->cyItem - 1);

		// Now convert into the number of items to scroll...
		dy = (dy + iRound) / plv->cyItem;

#ifdef MAINWIN
		if (dy > 0)
		{
			dy -= 1;
		}
#endif

		ListView_RScroll2(plv, 0, (int)dy, 0);
		if (ListView_RedrawEnabled(plv))
		{
			ListView_UpdateScrollBars(plv);
		}
		else
		{
			ListView_DeleteHrgnInval(plv);
			plv->hrgnInval = (HRGN)ENTIRE_REGION;
			plv->flags |= LVF_ERASE;
		}
#ifdef MAINWIN
		return TRUE;
#endif
	}
#ifdef MAINWIN
	return FALSE;
#else
	return TRUE;
#endif
}



void ListView_ROnScroll(SListViewData *plv, UINT code, int posNew, UINT sb)
{
	// SB_LINEUP或SB_LINEDOWN时的步进
	int cLine;

	cLine = (sb == SB_VERT) ? 1 : plv->cxLabelChar;
	ListView_ComOnScroll(plv, code, posNew, sb, cLine, -1);
}


int ListView_RGetScrollUnitsPerLine(SListViewData *plv, UINT sb)
{
	int cLine;

	cLine = (sb == SB_VERT) ? 1 : plv->cxLabelChar;
	return cLine;
}


//////////////////////////////////////////////////////////////////////////
// ListView Small Icon模式


int ListView_SItemHitTest(SListViewData *plv, int x, int y, UINT *pflags, int *piSubItem)
{

	int iHit;
	UINT flags;
	POINT pt;
	RECT rcState;
	RECT rcLabel;
	RECT rcIcon;

	if (piSubItem)
	{
		*piSubItem = 0;
	}

	// Map window-relative coordinates to view-relative coords...
	//
	pt.x = x + plv->ptOrigin.x;
	pt.y = y + plv->ptOrigin.y;

	// If we find an uncomputed item, recompute them all now...
	//
	if (plv->rcView.left == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	flags = 0;

	if (ListView_IsOwnerData(plv))
	{
		int cSlots;
		POINT ptWnd;
		LISTITEM item;

		cSlots = ListView_GetSlotCount(plv, TRUE);
		iHit = ListView_CalcHitSlot(plv, pt, cSlots);
		ListView_SGetRectsOwnerData(plv, iHit, &rcIcon, &rcLabel, &item, FALSE);
		ptWnd.x = x;
		ptWnd.y = y;

		if (PtInRect(&rcIcon, ptWnd))
		{
			flags = LVHT_ONITEMICON;
		}
		else if (PtInRect(&rcLabel, ptWnd))
		{
			flags = LVHT_ONITEMLABEL;
		}
	}
	else
	{
		for (iHit = 0; iHit < ListView_Count(plv); iHit++)
		{
			LISTITEM *pitem = ListView_FastGetZItemPtr(plv, iHit);
			POINT ptItem;

			ptItem.x = pitem->pt.x;
			ptItem.y = pitem->pt.y;

			rcIcon.top = ptItem.y;
			rcIcon.bottom = ptItem.y + plv->cyItem;

			rcLabel.top = rcIcon.top;
			rcLabel.bottom = rcIcon.bottom;

			// Quick, easy rejection test...
			//
			if (pt.y < rcIcon.top || pt.y >= rcIcon.bottom)
			{
				continue;
			}

			rcIcon.left = ptItem.x;
			rcIcon.right = ptItem.x + plv->cxSmIcon;

			rcState.bottom = rcIcon.bottom;
			rcState.right = rcIcon.left;
			rcState.left = rcState.right - plv->cxState;
			rcState.top = rcState.bottom - plv->cyState;

			rcLabel.left = rcIcon.right;
			rcLabel.right = rcLabel.left + pitem->cxSingleLabel;

			if (PtInRect(&rcIcon, pt))
			{
				flags = LVHT_ONITEMICON;
			}
			else if (PtInRect(&rcLabel, pt))
			{
				flags = LVHT_ONITEMLABEL;
			}
			else if (PtInRect(&rcState, pt))
			{
				flags = LVHT_ONITEMSTATEICON;
			}

			if (flags)
			{
				break;
			}
		}
	}

	if (flags == 0)
	{
		flags = LVHT_NOWHERE;
		iHit = -1;
	}
	else
	{
		if (!ListView_IsOwnerData(plv))
		{
			iHit = IUI_DPA_GetPtrIndex(plv->hdpa, (void *)ListView_FastGetZItemPtr(plv, iHit));
		}
	}

	*pflags = flags;
	return iHit;
}


void ListView_SGetRectsOwnerData(SListViewData *plv,
	int iItem,
	RECT *prcIcon,
	RECT *prcLabel,
	LISTITEM *pitem,
	BOOL fUsepitem)
{
	RECT rcIcon;
	RECT rcLabel;
	int cSlots;

	// calculate itemx, itemy, itemsSingleLabel from iItem
	cSlots = ListView_GetSlotCount(plv, TRUE);
	pitem->iWorkArea = 0;               // OwnerData doesn't support workareas
	ListView_SetIconPos(plv, pitem, iItem, cSlots);

	// calculate lable sizes
	// Note the rect we return should be the min of the size returned and the slot size...
	ListView_RecomputeLabelSize(plv, pitem, iItem, NULL, fUsepitem);

	rcIcon.left = pitem->pt.x - plv->ptOrigin.x;
	rcIcon.right = rcIcon.left + plv->cxSmIcon;
	rcIcon.top = pitem->pt.y - plv->ptOrigin.y;
	rcIcon.bottom = rcIcon.top + plv->cyItem;
	*prcIcon = rcIcon;

	rcLabel.left = rcIcon.right;
	if (pitem->cxSingleLabel < (plv->cxItem - plv->cxSmIcon))
	{
		rcLabel.right = rcLabel.left + pitem->cxSingleLabel;
	}
	else
	{
		rcLabel.right = rcLabel.left + plv->cxItem - plv->cxSmIcon;
	}
	rcLabel.top = rcIcon.top;
	rcLabel.bottom = rcIcon.bottom;
	*prcLabel = rcLabel;
}


void ListView_SGetRects(SListViewData *plv, LISTITEM *pitem, RECT *prcIcon, RECT *prcLabel, LPRECT prcBounds)
{

	_ASSERT(!ListView_IsOwnerData(plv));

	if (pitem->pt.x == RECOMPUTE)
	{
		ListView_Recompute(plv);
	}

	prcIcon->left = pitem->pt.x - plv->ptOrigin.x;
	prcIcon->right = prcIcon->left + plv->cxSmIcon;
	prcIcon->top = pitem->pt.y - plv->ptOrigin.y;
	prcIcon->bottom = prcIcon->top + plv->cyItem;

	prcLabel->left = prcIcon->right;
	prcLabel->right = prcLabel->left + pitem->cxSingleLabel;
	prcLabel->top = prcIcon->top;
	prcLabel->bottom = prcIcon->bottom;
}

// Return the index of the first item >= *pszLookup.
//
int ListView_DoLookupString(SListViewData *plv, LPCTSTR pszLookup, UINT flags, int iStart, int j)
{
	int i;
	BOOL fExact;
	int k;
	LISTITEM *pitem;
	LISTITEM *pitemLast = NULL;

	_ASSERT(!ListView_IsOwnerData(plv));

	fExact = FALSE;
	i = iStart;
	while ((i >= iStart) && (i < j))
	{
		int result;
		k = (i + j) / 2;
		pitem = ListView_FastGetItemPtr(plv, k);

		if (pitem == pitemLast)
		{
			break;
		}
		pitemLast = pitem;

		result = ListView_CompareString(plv,
				k, pszLookup, flags, 0);

#ifdef MAINWIN
		// IEUNIX - Mainwin's lstrcmp is not compatable with WIN32.
		if (result < 0)
		{
			result = -1;
		}
		else if (result > 0)
		{
			result = 1;
		}
#endif

		if (plv->hwnd->ci.style & LVS_SORTDESCENDING)
		{
			result = -result;
		}

		switch (result)
		{
		case 0:
			fExact = TRUE;
		// fall through
		case 1:
			j = k;
			break;
		case -1:
			i = k + 1;
			break;
		}
	}
	// For substrings, return index only if exact match was found.
	//
	if (!(flags & (LVFI_SUBSTRING | LVFI_PARTIAL)) &&
		!fExact)
	{
		return -1;
	}

	if (i < 0)
	{
		i = 0;
	}

	if ((!(flags & LVFI_NEARESTXY)) &&
		ListView_CompareString(plv, i, pszLookup, flags, 1))
	{
		i = -1;
	}
	return i;
}

int ListView_LookupString(SListViewData *plv, LPCTSTR pszLookup, UINT flags, int iStart)
{
	int iret;

	if (!pszLookup)
	{
		return 0;
	}

	iret = ListView_DoLookupString(plv, pszLookup, flags, iStart, ListView_Count(plv));
	if (iret == -1 && (flags & LVFI_WRAP))
	{
		iret = ListView_DoLookupString(plv, pszLookup, flags, 0, iStart);
	}

	return iret;
}

#include "stdafx.h"
#include "../Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#define CCHLABELMAX MAX_PATH            // borrowed from listview.h
#define HDDF_NOIMAGE  0x0001
#define HDDF_NOEDGE  0x0002

#define HDI_ALL95 0x001f

#define TF_HEADER TF_LISTVIEW

#define HD_EDITCHANGETIMER 0x100

#define c_cxFilterBarEdge (1)
#define c_cyFilterBarEdge (1)

#define c_cxFilterImage   (13)
#define c_cyFilterImage   (12)

#define IUI_HEAD_EDIT_PARENT	_T("IUI_HEAD_EDIT_PARENT")

struct HDI
{
	int     x;              // this is the x position of the RIGHT side (divider) of this item
	int     cxy;
	int     fmt;
	LPTSTR  pszText;
	HBITMAP hbm;
	int     iImage;         // index of bitmap in imagelist
	LPARAM  lParam;
	int     xBm;            // cached values
	int     xText;          // for implementing text and bitmap in header
	int     cxTextAndBm;

	// information used for the filter contol
	UINT    idOperator;
	UINT    type;
	HD_TEXTFILTER textFilter;
	int     intFilter;
};

// BUGBUG: store the style here too, set at create time
struct HD
{
	HD()
	{
		hwnd = NULL;
		flags = 0;
		cxEllipses = 0;
		cxDividerSlop = 0;
		cyChar = 0;
		hfont = NULL;
		hFilterImage = NULL;
		hdsaHDI = NULL;

		iTrack = 0;
		bTrackPress = FALSE;
		fTrackSet = FALSE;
		fOwnerDraw = FALSE;
		fFocus = FALSE;
		fFilterChangePending = FALSE;
		flagsTrack = 0;
		dxTrack = 0;
		xTrack = 0;
		xMinTrack = 0;
		xTrackOldWidth = 0;
		himl = NULL;

		hdsaOrder = NULL;

		iHot = 0;
		himlDrag = NULL;
		iNewOrder = 0;

		iTextMargin = 0;
		iBmMargin = 0;

		iFocus = 0;
		iEdit = 0;
		iButtonDown = 0;
		iFilterChangeTimeout = 0;
		hwndEdit = NULL;
		pfnEditWndProc = NULL;
		typeOld = 0;
		pszFilterOld = NULL;
		intFilterOld = 0;
		m_himgBk[0] = NULL;
		m_himgBk[1] = NULL;

		// IUI Add
		m_bCombineButtonBkImage = FALSE;
		m_himgCombineButtonBk = NULL;
		for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			m_himgButtonBk[i] = NULL;
			m_crButtonText[i] = RGB(0, 0, 0);
			m_hIUIButtonFont[i] = NULL;
		}
		m_eButtonBkImageResizeMode = IRM_9GRID;
		m_bHeaderTrack = TRUE;
		m_bHeaderDragDrop = TRUE;
	}

	virtual ~HD()
	{
		Release();
	}

	int Release()
	{
		hwnd->ci.Release();

		int i = 0;
		for (i = 0; i < COMBINEIMAGESIZE2; ++i)
		{
			ReleaseIUIImage(m_himgBk[i]);
			m_himgBk[i] = NULL;
		}

		ReleaseIUIImage(m_himgCombineButtonBk);
		m_himgCombineButtonBk = NULL;
		for (i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			ReleaseIUIImage(m_himgButtonBk[i]);
			m_himgButtonBk[i] = NULL;

			ReleaseIUIFontInternal(m_hIUIButtonFont[i]);
			m_hIUIButtonFont[i] = NULL;
		}

		return 0;
	}

	HWLWND hwnd;

	UINT flags;
	int cxEllipses;
	int cxDividerSlop;
	// 原意为Header创建时，以指定字体输出"..."的文字的高度
	// 在IUI中，表示Header高度，可以用户设置，见HDM_SETHEIGHT
	int cyChar;
	HFONT hfont;
	HIMAGELIST hFilterImage;
	HDSA hdsaHDI;       // list of HDI's

	// tracking state info
	int iTrack;
	BITBOOL bTrackPress : 1;		// is the button pressed?
	BITBOOL fTrackSet: 1;
	BITBOOL fOwnerDraw: 1;
	BITBOOL fFocus: 1;
	BITBOOL fFilterChangePending: 1;
	UINT flagsTrack;
	int dxTrack;                    // the distance from the divider that the user started tracking
	int xTrack;                     // the current track position (or starting track position on a button drag)
	int xMinTrack;                  // the x of the end of the previous item (left limit)
	int xTrackOldWidth;
	HIMAGELIST himl;            // handle to our image list

	HDSA hdsaOrder;     // this is an index array of the hdsaHDI items.
	// this is the physical order of items

	// Highlight Item.
	int iHot;
	HIMAGELIST himlDrag;
	int iNewOrder;      // what's the new insertion point for a d/d?

	int iTextMargin; // The margin to place on either side of text or bitmaps
	int iBmMargin;   // Normally, 3 * (GetSystemMetrics(SM_CXBORDER) * 2)

	int iFocus;         // focus object
	int iEdit;          // editing object
	int iButtonDown;
	int iFilterChangeTimeout;
	HWND hwndEdit;
	WNDPROC pfnEditWndProc;
	int typeOld;
	LPTSTR pszFilterOld;
	int intFilterOld;

	// IUI Add
	HIUIIMAGE m_himgBk[COMBINEIMAGESIZE2];

	BOOL m_bCombineButtonBkImage;
	HIUIIMAGE m_himgCombineButtonBk;
	HIUIIMAGE m_himgButtonBk[COMBINEIMAGESIZE4];
	COLORREF m_crButtonText[COMBINEIMAGESIZE4];
	HIUIFONT m_hIUIButtonFont[COMBINEIMAGESIZE4];
	IMAGE_RESIZE_MODE m_eButtonBkImageResizeMode;
	CIUIRect m_rcButtonBkImage9GridArg;

	// 是否允许调整列宽
	BOOL m_bHeaderTrack;
	// 是否允许拖动Header Item
	BOOL m_bHeaderDragDrop;
};


// Message handler functions

BOOL Header_OnCreate(HD *phd, CREATESTRUCT *lpCreateStruct);
void Header_OnNCDestroy(HD *phd);

HIMAGELIST Header_OnSetImageList(HD *phd, HIMAGELIST himl);
HIMAGELIST Header_OnGetImageList(HD *phd);

void Header_OnPaint(HD *phd, HDC hdcIn);
#if 0
BOOL Header_OnEraseBkgnd(HD *phd, HDC hdc);
#endif
void Header_OnCommand(HD *phd, int id, HWND hwndCtl, UINT codeNotify);
void Header_OnEnable(HD *phd, BOOL fEnable);
UINT Header_OnGetDlgCode(HD *phd, MSG *lpmsg);
void Header_OnLButtonDown(HD *phd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
BOOL Header_IsTracking(HD *phd);
void Header_OnMouseMove(HD *phd, int x, int y, UINT keyFlags);
void Header_OnLButtonUp(HD *phd, int x, int y, UINT keyFlags);
void Header_OnSetFont(HD *plv, HFONT hfont, BOOL fRedraw);
int PASCAL Header_OnHitTest(HD *phd, HD_HITTESTINFO *phdht);
HFONT Header_OnGetFont(HD *plv);
HIMAGELIST Header_OnCreateDragImage(HD *phd, int i);
BOOL Header_OnGetItemRect(HD *phd, int i, RECT *prc);
void Header_Draw(HD *phd, HDC hdc, RECT *prcClip);
void Header_InvalidateItem(HD *phd, int i, UINT uFlags);
void Header_GetDividerRect(HD *phd, int i, LPRECT prc);
LPARAM Header_OnSetHotDivider(HD *phd, BOOL fPos, LPARAM lParam);
void Header_GetFilterRects(LPRECT prcItem, LPRECT prcHeader, LPRECT prcEdit, LPRECT prcButton);
BOOL Header_BeginFilterEdit(HD *phd, int i);
VOID Header_StopFilterEdit(HD *phd, BOOL fDiscardChanges);
VOID Header_FilterChanged(HD *phd, BOOL fWait);
VOID Header_OnFilterButton(HD *phd, INT i);
LRESULT Header_OnClearFilter(HD *phd, INT i);

// HDM_* Message handler functions

int Header_OnInsertItem(HD *phd, int i, const HD_ITEM *pitem);
BOOL Header_OnDeleteItem(HD *phd, int i);
BOOL Header_OnGetItem(HD *phd, int i, HD_ITEM *pitem);
BOOL Header_OnSetItem(HD *phd, int i, const HD_ITEM *pitem);
BOOL Header_OnLayout(HD *phd, HD_LAYOUT *playout);
BOOL Header_OnSetCursor(HD *phd, HWND hwndCursor, UINT codeHitTest, UINT msg);
void Header_DrawDivider(HD *phd, int x);
#ifdef UNICODE
int Header_OnInsertItemA(HD *phd, int i, HD_ITEMA *pitem);
BOOL Header_OnGetItemA(HD *phd, int i, HD_ITEMA *pitem);
BOOL Header_OnSetItemA(HD *phd, int i, HD_ITEMA *pitem);
#endif

void Header_EndDrag(HD *phd);
BOOL Header_SendChange(HD *phd, int i, int code, const HD_ITEM *pitem);
BOOL Header_Notify(HD *phd, int i, int iButton, int code);

#define Header_GetItemPtr(phd, i)   (HDI *)IUI_DSA_GetItemPtr((phd)->hdsaHDI, (i))
#define Header_GetCount(phd) (DSA_GetItemCount((phd)->hdsaHDI))

// returns -1 if failed to find the item
int Header_OnGetItemOrder(HD *phd, int i)
{
	int iIndex;

	// if there's no hdsaOrder, then it's in index order
	if (phd->hdsaOrder)
	{
		int j;
		int iData;

		iIndex = -1;

		for (j = 0; j < DSA_GetItemCount(phd->hdsaOrder); j++)
		{
			IUI_DSA_GetItem(phd->hdsaOrder, j, &iData);
			if (iData == i)
			{
				iIndex = j;
				break;
			}
		}

	}
	else
	{
		iIndex = i;
	}

	return iIndex;
}


int Header_ItemOrderToIndex(HD *phd, int iOrder)
{
	RIPMSG(iOrder < DSA_GetItemCount(phd->hdsaHDI), "HDM_ORDERTOINDEX: Invalid order %d", iOrder);
	if (phd->hdsaOrder)
	{
		_ASSERT(DSA_GetItemCount(phd->hdsaHDI) == DSA_GetItemCount(phd->hdsaOrder));
#ifdef DEBUG
		// DSA_GetItem will assert on an invalid index, so filter it out
		// so all we get is the RIP above.
		if (iOrder < DSA_GetItemCount(phd->hdsaOrder))
#endif
			IUI_DSA_GetItem(phd->hdsaOrder, iOrder, &iOrder);
	}

	return iOrder;
}

HDI *Header_GetItemPtrByOrder(HD *phd, int iOrder)
{
	int iIndex = Header_ItemOrderToIndex(phd, iOrder);
	return Header_GetItemPtr(phd, iIndex);
}

HDSA Header_InitOrderArray(HD *phd)
{
	int i;

	if (!phd->hdsaOrder && !(phd->hwnd->ci.style & HDS_OWNERDATA))
	{

		// not initialized yet..
		// create an array with i to i mapping
		phd->hdsaOrder = IUI_DSA_Create(sizeof(int), 4);

		if (phd->hdsaOrder)
		{
			for (i = 0; i < Header_GetCount(phd); i++)
			{
				if (IUI_DSA_InsertItem(phd->hdsaOrder, i, &i) == -1)
				{
					// faild to add... bail
					IUI_DSA_Destroy(phd->hdsaOrder);
					phd->hdsaOrder = NULL;
				}
			}
		}
	}
	return phd->hdsaOrder;
}

// this moves all items starting from iIndex over by dx
void Header_ShiftItems(HD *phd, int iOrder, int dx)
{
	for (; iOrder < Header_GetCount(phd); iOrder++)
	{
		HDI *phdi = Header_GetItemPtrByOrder(phd, iOrder);
		phdi->x += dx;
	}
}

void Header_OnSetItemOrder(HD *phd, int iIndex, int iOrder)
{
	if (iIndex < Header_GetCount(phd) &&
		iOrder < Header_GetCount(phd) &&
		Header_InitOrderArray(phd))
	{
		int iCurOrder = Header_OnGetItemOrder(phd, iIndex);

		// only do work if the order is changing
		if (iOrder != iCurOrder)
		{

			// delete the current order location
			HDI *phdi = Header_GetItemPtr(phd, iIndex);
			HDI *phdiOld = Header_GetItemPtrByOrder(phd, iOrder);

			// stop editing the filter
			Header_StopFilterEdit(phd, FALSE);

			// remove iIndex from the current order
			// (slide stuff to the right down by our width)
			Header_ShiftItems(phd, iCurOrder + 1, -phdi->cxy);
			IUI_DSA_DeleteItem(phd->hdsaOrder, iCurOrder);

			// insert it into the order and slide everything else over
			// (slide stuff to the right of the new position up by our width)
			IUI_DSA_InsertItem(phd->hdsaOrder, iOrder, &iIndex);
			// set our right edge to where their left edge was
			Header_ShiftItems(phd, iOrder + 1, phdi->cxy);

			if (iOrder == 0)
			{
				phdi->x = phdi->cxy;
			}
			else
			{
				phdiOld = Header_GetItemPtrByOrder(phd, iOrder - 1);
				phdi->x = phdiOld->x + phdi->cxy;
			}

			// BUGBUG: do something better...
			InvalidateRect(phd->hwnd, NULL);
		}
	}
}

void Header_SetHotItem(HD *phd, int i)
{
	if (i != phd->iHot)
	{
		Header_InvalidateItem(phd, i, RDW_INVALIDATE);
		Header_InvalidateItem(phd, phd->iHot, RDW_INVALIDATE);
		phd->iHot = i;
	}
}

LRESULT Header_OnGetOrderArray(HD *phd, int iCount, LPINT lpi)
{
	int i;

	if (Header_GetCount(phd) != iCount)
	{
		return FALSE;
	}

	for (i = 0; i < Header_GetCount(phd) ; i++)
	{
		lpi[i] = Header_ItemOrderToIndex(phd, i);
	}
	return TRUE;
}

LRESULT Header_OnSetOrderArray(HD *phd, int iCount, LPINT lpi)
{
	int i;

	if (Header_GetCount(phd) != iCount)
	{
		return FALSE;
	}

	for (i = 0; i < Header_GetCount(phd); i++)
	{
		Header_OnSetItemOrder(phd, lpi[i], i);
	}

	MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(phd->hwnd), OBJID_CLIENT, 0);

	return TRUE;
}

BOOL HDDragFullWindows(HD *phd)
{
	int g_fDragFullWindows;
	SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, sizeof(g_fDragFullWindows), &g_fDragFullWindows, 0);
	return (g_fDragFullWindows && (phd->hwnd->ci.style & HDS_FULLDRAG));
}

LRESULT WINAPI IUI::Header_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	HD *phd = (HD *)GetMember(hWnd);

#if 0
	if (uMsg >= WM_USER)
	{
		DebugMsg(DM_TRACE, TEXT("Header_WndProc %d %d %d"), uMsg, wParam, lParam);
	}
#endif

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (phd != NULL)
		{
			// was this key hit since the last time we asked?
			if ((uMsg == WM_CAPTURECHANGED && pe->eRoutingStrategy == RS_BUBBLE)
				|| (uMsg == WM_RBUTTONDOWN && pe->eRoutingStrategy == RS_TUNNEL)
				|| GetAsyncKeyState(VK_ESCAPE) & 0x01)
			{
				if (phd->himlDrag != NULL)
				{
					// if this is the end of a drag,
					// notify the user.
					HDITEM item;

					item.mask = HDI_ORDER;
					item.iOrder = -1; // abort order changing
					Header_EndDrag(phd);

					Header_SendChange(phd, phd->iTrack, HDN_ENDDRAG, &item);

				}
				else if (phd->flagsTrack & (HHT_ONDIVIDER | HHT_ONDIVOPEN))
				{
					HD_ITEM item;
					item.mask = HDI_WIDTH;
					item.cxy = phd->xTrackOldWidth;

					phd->flagsTrack = 0;
					::KillTimer(GetHostWnd(phd->hwnd), 1);
					CCReleaseCapture(&phd->hwnd->ci);

					Header_SendChange(phd, phd->iTrack, HDN_ENDTRACK, &item);
					if (HDDragFullWindows(phd))
					{

						// incase they changed something
						item.mask = HDI_WIDTH;
						item.cxy = phd->xTrackOldWidth;
						Header_OnSetItem(phd, phd->iTrack, &item);

						RedrawWindow(GetHostWnd(phd->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);

					}
					else
					{
						// Undraw the last divider we displayed
						Header_DrawDivider(phd, phd->xTrack);
					}
				}
			}
		}

		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (phd == NULL)
			{
				if (uMsg == WM_NCCREATE)
				{
					phd = new HD;

					if (phd == NULL)
					{
						return 0L;
					}

					phd->hwnd = hWnd;
					phd->hwnd->ci.hwndParent = GetParent(hWnd);
					//phd->hwnd->ci.hwndParent = ((LPCREATESTRUCT)lParam)->hwndParent;
					SetMember(hWnd, phd);

					// fall through to call DefWindowProc
				}

				return 0;
			}

			switch (uMsg)
			{
				HANDLE_MSG(phd, WM_CREATE, Header_OnCreate);
#if 0
				HANDLE_MSG(phd, WM_ERASEBKGND, Header_OnEraseBkgnd);
				HANDLE_MSG(phd, WM_ENABLE, Header_OnEnable);
#endif

			case WM_NCDESTROY:
			{
				Header_OnNCDestroy(phd);
				delete phd;
				phd = NULL;
				SetMember(hWnd, 0);
			}
			return 0;

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				// filter bar and not editing then take caret into edit first column
				if (phd->hwnd->ci.style & HDS_FILTERBAR)
				{
					phd->fFocus = (uMsg == WM_SETFOCUS);
					Header_InvalidateItem(phd, Header_ItemOrderToIndex(phd, phd->iFocus), RDW_INVALIDATE);
					UpdateWindow(GetHostWnd(phd->hwnd));
					return (0);
				}
				break;


			case WM_STYLECHANGED:
				if (wParam == GWL_STYLE)
				{
					LPSTYLESTRUCT pss = (LPSTYLESTRUCT)lParam;

					phd->hwnd->ci.style = pss->styleNew;

					// if the filter is changing then discard it if its active
					if ((pss->styleOld & HDS_FILTERBAR) != (pss->styleNew & HDS_FILTERBAR))
					{
						Header_StopFilterEdit(phd, TRUE);
					}

					// we don't cache our style so relay out and invaidate
					::InvalidateRect(GetHostWnd(phd->hwnd), NULL, TRUE);
				}
				return (0);

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			if (phd != NULL)
			{
				if ((uMsg >= WM_MOUSEFIRST)
					&& (uMsg <= WM_MOUSELAST)
					&& (phd->hwnd->ci.style & HDS_HOTTRACK)
					&& !phd->fTrackSet)
				{
					TRACKMOUSEEVENT tme;

					phd->fTrackSet = TRUE;

					tme.cbSize = sizeof(tme);
					tme.hwndTrack = GetHostWnd(phd->hwnd);
					tme.dwFlags = TME_LEAVE;

					TrackMouseEvent(&tme);
				}
			}

			switch (uMsg)
			{
				HANDLE_MSG(phd, WM_MOUSEMOVE, Header_OnMouseMove);
				HANDLE_MSG(phd, WM_LBUTTONDOWN, Header_OnLButtonDown);
				HANDLE_MSG(phd, WM_LBUTTONDBLCLK, Header_OnLButtonDown);
				HANDLE_MSG(phd, WM_LBUTTONUP, Header_OnLButtonUp);

			case WM_KEYDOWN:
				if (phd->fFocus)
				{
					// handle the key events that the header control receives, when the filter
					// bar is displayed we then allow the user to enter filter mode and drop the
					// filter menu.
					//
					//  F2 = enter filter mode
					//  F4 = drop filter menu
					//  -> = next column
					//  <- = previous column

					if (wParam == VK_F2)
					{
						// start editing the currently focused column
						Header_BeginFilterEdit(phd, Header_ItemOrderToIndex(phd, phd->iFocus));
#ifdef KEYBOARDCUES
						//notify of navigation key usage
						CCNotifyNavigationKeyUsage(&(phd->hwnd->ci), UISF_HIDEFOCUS);
#endif
						return 0L;
					}
					else if (wParam == VK_F4)
					{
						// drop the filter menu (this exits edit mode)
						Header_OnFilterButton(phd, Header_ItemOrderToIndex(phd, phd->iFocus));
#ifdef KEYBOARDCUES
						//notify of navigation key usage
						CCNotifyNavigationKeyUsage(&(phd->hwnd->ci), UISF_HIDEFOCUS);
#endif
						return 0L;
					}
					else if ((wParam == VK_LEFT) || (wParam == VK_RIGHT))
					{
						INT iFocus = phd->iFocus;

						// move to previous or next column
						if (wParam == VK_RIGHT)
						{
							phd->iFocus = (iFocus + 1) % Header_GetCount(phd);
						}
						else
						{
							phd->iFocus = iFocus - 1;
							if (phd->iFocus < 0)
							{
								phd->iFocus = max(Header_GetCount(phd) - 1, 0);
							}
						}

						// did the focused column change? if so then update the control
						// as required.
						if (iFocus != phd->iFocus)
						{
							Header_InvalidateItem(phd, Header_ItemOrderToIndex(phd, iFocus), RDW_INVALIDATE);
							Header_InvalidateItem(phd, Header_ItemOrderToIndex(phd, phd->iFocus), RDW_INVALIDATE);
							UpdateWindow(GetHostWnd(phd->hwnd));
						}
#ifdef KEYBOARDCUES
						//notify of navigation key usage
						CCNotifyNavigationKeyUsage(&(phd->hwnd->ci), UISF_HIDEFOCUS);
#endif
						return 0L;
					}
				}
				break;

			case WM_MOUSELEAVE:
				Header_SetHotItem(phd, -1);
				phd->fTrackSet = FALSE;
				break;

			case WM_RBUTTONUP:
				if (CCSendNotify(&phd->hwnd->ci, NM_RCLICK, NULL))
				{
					return (0);
				}
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (uMsg)
			{
				HANDLE_MSG(phd, WM_GETDLGCODE, Header_OnGetDlgCode);
				HANDLE_MSG(phd, WM_SETFONT, Header_OnSetFont);
				HANDLE_MSG(phd, WM_GETFONT, Header_OnGetFont);
				HANDLE_MSG(phd, WM_SETCURSOR, Header_OnSetCursor);

			case WM_PRINTCLIENT:
			case WM_PAINT:
			case WM_PRINT:
				Header_OnPaint(phd, (HDC)wParam);
				return (0);

			case WM_TIMER:
				if (wParam == HD_EDITCHANGETIMER)
				{
					Header_FilterChanged(phd, FALSE);
					return (0);
				}
				break;

			case WM_COMMAND:
				if ((phd->iEdit >= 0) && ((HWND)lParam == phd->hwndEdit))
				{
					// when filtering we will receive notifications that the filter
					// has been edited, therefore lets send those down to the
					// parent.

					if (HIWORD(wParam) == EN_CHANGE)
					{
						Header_FilterChanged(phd, TRUE);
						return (0);
					}
				}
				break;

			case HDM_GETITEMCOUNT:
				return (LPARAM)(UINT)DSA_GetItemCount(phd->hdsaHDI);

			case HDM_INSERTITEM:
				return (LPARAM)Header_OnInsertItem(phd, (int)wParam, (const HD_ITEM *)lParam);

			case HDM_DELETEITEM:
				return (LPARAM)Header_OnDeleteItem(phd, (int)wParam);

			case HDM_GETITEM:
				return (LPARAM)Header_OnGetItem(phd, (int)wParam, (HD_ITEM *)lParam);

			case HDM_SETITEM:
				return (LPARAM)Header_OnSetItem(phd, (int)wParam, (const HD_ITEM *)lParam);

			case HDM_LAYOUT:
				return (LPARAM)Header_OnLayout(phd, (HD_LAYOUT *)lParam);

			case HDM_HITTEST:
				return (LPARAM)Header_OnHitTest(phd, (HD_HITTESTINFO *)lParam);

			case HDM_GETITEMRECT:
				return (LPARAM)Header_OnGetItemRect(phd, (int)wParam, (LPRECT)lParam);

			case HDM_SETIMAGELIST:
				return (LRESULT)(ULONG_PTR)Header_OnSetImageList(phd, (HIMAGELIST)lParam);

			case HDM_GETIMAGELIST:
				return (LRESULT)(ULONG_PTR)phd->himl;

#ifdef UNICODE
			case HDM_INSERTITEMA:
				return (LPARAM)Header_OnInsertItemA(phd, (int)wParam, (HD_ITEMA *)lParam);

			case HDM_GETITEMA:
				return (LPARAM)Header_OnGetItemA(phd, (int)wParam, (HD_ITEMA *)lParam);

			case HDM_SETITEMA:
				return (LPARAM)Header_OnSetItemA(phd, (int)wParam, (HD_ITEMA *)lParam);
#endif

			case HDM_ORDERTOINDEX:
				return Header_ItemOrderToIndex(phd, (int)wParam);

			case HDM_CREATEDRAGIMAGE:
				return (LRESULT)Header_OnCreateDragImage(phd, Header_OnGetItemOrder(phd, (int)wParam));

			case HDM_SETORDERARRAY:
				return Header_OnSetOrderArray(phd, (int)wParam, (LPINT)lParam);

			case HDM_GETORDERARRAY:
				return Header_OnGetOrderArray(phd, (int)wParam, (LPINT)lParam);

			case HDM_SETHOTDIVIDER:
				return Header_OnSetHotDivider(phd, (int)wParam, lParam);

			case HDM_SETBITMAPMARGIN:
				phd->iBmMargin = (int)wParam;
				TraceMsg(TF_ALWAYS, "Setting bmMargin = %d", wParam);
				return TRUE;

			case HDM_GETBITMAPMARGIN:
				return phd->iBmMargin;

			case HDM_EDITFILTER:
				Header_StopFilterEdit(phd, (BOOL)LOWORD(lParam));
				return Header_BeginFilterEdit(phd, (int)wParam);

			case HDM_SETFILTERCHANGETIMEOUT:
				if (lParam)
				{
					int iOldTimeout = phd->iFilterChangeTimeout;
					phd->iFilterChangeTimeout = (int)lParam;
					return (iOldTimeout);
				}
				return (phd->iFilterChangeTimeout);

			case HDM_CLEARFILTER:
				return Header_OnClearFilter(phd, (int)wParam);

			case WM_GETOBJECT:
				if (lParam == OBJID_QUERYCLASSNAMEIDX)
				{
					return MSAA_CLASSNAMEIDX_HEADER;
				}
				break;

			// IUI ADD
			case HDM_SETHEIGHT:
			{
				int nOld = phd->cyChar;
				phd->cyChar = wParam;
				return nOld;
			}
			break;

			case HDM_COMBINE_BUTTON_BK_IMAGE:
				phd->m_bCombineButtonBkImage = lParam;
				return 0;

			case HDM_IS_COMBINE_BUTTON_BK_IMAGE:
				return phd->m_bCombineButtonBkImage;

			case HDM_SET_BUTTON_BK_IMAGE:
				if ((CONTROL_STATE)wParam >= CONTROL_STATE_NORMAL
					&& (CONTROL_STATE)wParam <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex((CONTROL_STATE)wParam);

					return IUISetControlImage(GetProject(hWnd),
							&phd->m_himgButtonBk[nStateIndex], LPCTSTR(lParam));
				}
				else if ((CONTROL_STATE)wParam == CONTROL_STATE_COMBINE)
				{
					return IUISetControlImage(GetProject(hWnd),
							&phd->m_himgCombineButtonBk, LPCTSTR(lParam));
				}
				return 0;

			case HDM_GET_BUTTON_BK_IMAGE:
				if ((CONTROL_STATE)wParam >= CONTROL_STATE_NORMAL
					&& (CONTROL_STATE)wParam <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex((CONTROL_STATE)wParam);

					return (LRESULT)(LPCTSTR)phd->m_himgButtonBk[nStateIndex]->GetSafeImageName();
				}
				else if ((CONTROL_STATE)wParam == CONTROL_STATE_COMBINE)
				{
					return (LRESULT)(LPCTSTR)phd->m_himgCombineButtonBk->GetSafeImageName();
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_NORMAL == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						phd->m_eButtonBkImageResizeMode = (IMAGE_RESIZE_MODE)lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_NORMAL == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							return phd->m_eButtonBkImageResizeMode;
						}
					}
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_NORMAL == LOWORD(wParam))
				{
					phd->m_rcButtonBkImage9GridArg = (LPCRECT)lParam;
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_NORMAL == LOWORD(wParam))
				{
					if (lParam != NULL)
					{
						*(LPRECT)lParam = phd->m_rcButtonBkImage9GridArg;
					}
				}
				return 0;

			case WLM_SET_TEXT_COLOR:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					phd->m_crButtonText[nStateIndex] = cr;
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
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);
					return phd->m_crButtonText[nStateIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_TEXT_FONT:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				return IUISetControlFont4(GetProject(hWnd), eControlState, phd->m_hIUIButtonFont,
						(LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam);
			}

			case WLM_GET_TEXT_FONT:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nIndex = GetButtonStateIndex(eControlState);
					return phd->m_hIUIButtonFont[nIndex] == NULL ? NULL : (LRESULT)(LPCTSTR)phd->m_hIUIButtonFont[nIndex]->m_strFontResID;
				}
			}
			return 0;

			case HDM_ENABLE_HEADER_TRACK:
				phd->m_bHeaderTrack = lParam;
				return 0;

			case HDM_IS_HEADER_TRACK_ENABLED:
				return phd->m_bHeaderTrack;

			case HDM_ENABLE_HEADER_DRAG:
				phd->m_bHeaderDragDrop = lParam;
				return 0;

			case HDM_IS_HEADER_DRAG_ENABLED:
				return phd->m_bHeaderDragDrop;

			case WLM_SET_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = LPCTSTR(lParam);

				if (eControlState == CONTROL_STATE_NORMAL
					|| eControlState == CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetLabelStateIndex(eControlState);

					return IUISetControlImage(GetProject(hWnd),
							&phd->m_himgBk[nStateIndex], lpszImage);
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					return IUISetControlImage(GetProject(hWnd),
							&phd->hwnd->ci.m_himgCombineBk, lpszImage);
				}
			}
			return 0;

			case WLM_GET_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_NORMAL
					|| eControlState == CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetLabelStateIndex(eControlState);

					return (LRESULT)(LPCTSTR)phd->m_himgBk[nStateIndex]->GetSafeImageName();
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					return (LRESULT)(LPCTSTR)phd->hwnd->ci.m_himgCombineBk->GetSafeImageName();
				}
			}
			return 0;

			default:
			{
				LRESULT lres;
				if (CCWndProc(&phd->hwnd->ci, uMsg, wParam, lParam, &lres))
				{
					return lres;
				}

				break;
			}
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

BOOL Header_SendChange(HD *phd, int i, int code, const HD_ITEM *pitem)
{
	NMHEADER nm;

	nm.iItem = i;
	nm.pitem = (HD_ITEM *)pitem;
	nm.iButton = 0;

	return !(BOOL)CCSendNotify(&phd->hwnd->ci, code, &nm.hdr);
}

BOOL Header_Notify(HD *phd, int i, int iButton, int code)
{
	NMHEADER nm;
	nm.iItem = i;
	nm.iButton = iButton;
	nm.pitem = NULL;

	return !(BOOL)CCSendNotify(&phd->hwnd->ci, code, &nm.hdr);
}


void Header_NewFont(HD *phd, HFONT hfont)
{
	HDC hdc;
	SIZE siz;

	hdc = GetDC(HWND_DESKTOP);

	if (hfont)
	{
		SelectFont(hdc, hfont);
	}

	GetTextExtentPoint(hdc, TEXT("..."), CCHELLIPSES, &siz);

	phd->cxEllipses = siz.cx;
	phd->cyChar = 17;//siz.cy;
	phd->hfont = hfont;
	phd->hwnd->ci.uiCodePage = GetCodePageForFont(hfont);

	ReleaseDC(HWND_DESKTOP, hdc);
}

BOOL Header_OnCreate(HD *phd, CREATESTRUCT *lpCreateStruct)
{
	_ASSERT(phd); // we are only called if phd is valid

#ifdef DEBUG
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		phd->hwnd->ci.style |= HDS_DRAGDROP;
	}
#endif
	phd->flags = 0;
	phd->hfont = NULL;
	phd->hFilterImage = NULL;

	phd->iNewOrder = -1;
	phd->iHot = -1;

	phd->iFocus = 0;
	phd->iEdit = -1;
	phd->iButtonDown = -1;
	phd->iFilterChangeTimeout = GetDoubleClickTime() * 2;
	phd->hwndEdit = NULL;

	phd->hdsaHDI = IUI_DSA_Create(sizeof(HDI), 4);

	if (!phd->hdsaHDI)
	{
		return (BOOL)(-1);
	}

	phd->cxDividerSlop = 8 * GetSystemMetrics(SM_CXBORDER);

	// Warning!  ListView_RSetColumnWidth knows these values.
	phd->iTextMargin = 3 * (GetSystemMetrics(SM_CXBORDER) * 2);
	phd->iBmMargin = 3 * (GetSystemMetrics(SM_CXBORDER) * 2);

	// phd->himl = NULL;
	Header_NewFont(phd, NULL);
	return TRUE;
}

// 必须用WINAPI修饰，否则枚举函数无法返回。
int WINAPI Header_DestroyItemCallback(LPVOID p, LPVOID d)
{
	HDI *phdi = (HDI *)p;
	if (phdi)
	{
		Str_Set(&phdi->pszText, NULL);

		if ((phdi->type & HDFT_ISMASK) == HDFT_ISSTRING)
		{
			Str_Set(&phdi->textFilter.pszText, NULL);
		}
	}
	return 1;
}

void Header_OnNCDestroy(HD *phd)
{
	// stop editing the filter
	if (phd->hFilterImage)
	{
		ImageList_Destroy(phd->hFilterImage);
	}

	Header_StopFilterEdit(phd, TRUE);

	// We must walk through and destroy all of the string pointers that
	// are contained in the structures before we pass it off to the
	// DSA_Destroy function...

	IUI_DSA_DestroyCallback(phd->hdsaHDI, (PFNDAENUMCALLBACK)Header_DestroyItemCallback, 0);
	phd->hdsaHDI = NULL;
	if (phd->hdsaOrder)
	{
		IUI_DSA_Destroy(phd->hdsaOrder);
		phd->hdsaOrder = NULL;
	}
}

HIMAGELIST Header_OnSetImageList(HD *phd, HIMAGELIST himl)
{
	HIMAGELIST hImageOld = phd->himl;
	phd->himl = himl;
	return hImageOld;
}

void Header_OnPaint(HD *phd, HDC hdc)
{
	if (phd == NULL || hdc == NULL)
	{
		_ASSERT(FALSE);
		return;
	}

	CIUIRect rc;
	GetClientRect(phd->hwnd, rc);

	Header_Draw(phd, hdc, rc);
}

UINT Header_OnGetDlgCode(HD *phd, MSG *lpmsg)
{
	return DLGC_WANTTAB | DLGC_WANTARROWS;
}


int Header_HitTest(HD *phd, int x, int y, UINT *pflags)
{
	UINT flags = 0;
	POINT pt;
	RECT rc;
	HDI *phdi = NULL;
	int i;

	pt.x = x;
	pt.y = y;

	GetClientRect(phd->hwnd, &rc);

	flags = 0;
	i = -1;
	if (x < rc.left)
	{
		flags |= HHT_TOLEFT;
	}
	else if (x >= rc.right)
	{
		flags |= HHT_TORIGHT;
	}
	if (y < rc.top)
	{
		flags |= HHT_ABOVE;
	}
	else if (y >= rc.bottom)
	{
		flags |= HHT_BELOW;
	}

	if (flags == 0)
	{
		int cItems = DSA_GetItemCount(phd->hdsaHDI);
		int xPrev = 0;
		BOOL fPrevZero = FALSE;
		int xItem;
		int cxSlop;

		//DebugMsg(DM_TRACE, "Hit Test begin");
		for (i = 0; i <= cItems; i++, phdi++, xPrev = xItem)
		{
			if (i == cItems)
			{
				xItem = rc.right;
			}
			else
			{
				phdi = Header_GetItemPtrByOrder(phd, i);
				xItem = phdi->x;
			}

			// DebugMsg(DM_TRACE, "x = %d xItem = %d xPrev = %d fPrevZero = %d", x, xItem, xPrev, xPrev == xItem);
			if (xItem == xPrev)
			{
				// Skip zero width items...
				//
				fPrevZero = TRUE;
				continue;
			}

			cxSlop = min((xItem - xPrev) / 4, phd->cxDividerSlop);

			if (x >= xPrev && x < xItem)
			{
				if (phd->hwnd->ci.style & HDS_FILTERBAR)
				{
					RECT rcItem;
					RECT rcHeader, rcFilter, rcButton;

					rcItem.left   = xPrev;
					rcItem.top    = rc.top;
					rcItem.right  = xItem;
					rcItem.bottom = rc.bottom ;

					Header_GetFilterRects(&rcItem, &rcHeader, &rcFilter, &rcButton);

					if (y >= rcFilter.top)
					{
						if (x >= rcFilter.right)
						{
							// hit check the entire button, forget about the divider
							// when over the filter glyph
							flags = HHT_ONFILTERBUTTON;
							break;
						}
						else
						{
							flags = HHT_ONFILTER;
						}
					}
					else if (y < rcHeader.bottom)
					{
						flags = HHT_ONHEADER;
					}
				}
				else
				{
					flags = HHT_ONHEADER;
				}

				if (i > 0 && x < xPrev + cxSlop)
				{
					i--;
					flags = HHT_ONDIVIDER;

					if (fPrevZero && x > xPrev)
					{
						flags = HHT_ONDIVOPEN;
					}
				}
				else if (x >= xItem - cxSlop)
				{
					flags = HHT_ONDIVIDER;
				}

				break;
			}
			fPrevZero = FALSE;
		}
		if (i == cItems)
		{
			i = -1;
			flags = HHT_NOWHERE;
		}
		else
		{
			// now convert order index to real index
			i = Header_ItemOrderToIndex(phd, i);
		}

	}
	*pflags = flags;
	return i;
}

int PASCAL Header_OnHitTest(HD *phd, HD_HITTESTINFO *phdht)
{
	if (phdht && phd)
	{
		phdht->iItem = Header_HitTest(phd, phdht->pt.x, phdht->pt.y, &phdht->flags);
		return phdht->iItem;
	}
	else
	{
		return -1;
	}
}

BOOL Header_OnSetCursor(HD *phd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	POINT pt;
	UINT flags;
	LPCTSTR lpCur;
	INT iItem;

	if (!phd)
	{
		return FALSE;
	}

	if (GetHostWnd(phd->hwnd) != hwndCursor || codeHitTest >= 0x8000)
	{
		return FALSE;
	}

	GetMessagePosClient(phd->hwnd, &pt);

	iItem = Header_HitTest(phd, pt.x, pt.y, &flags);

	switch (flags)
	{
	case HHT_ONDIVIDER:
		lpCur = (LPCTSTR)32644; // IDC_SIZEWE
		break;
	case HHT_ONDIVOPEN:
		lpCur = (LPCTSTR)32644; // IDC_SIZEWE
		break;

	case HHT_ONFILTER:
	{
		HDI *phdi = Header_GetItemPtrByOrder(phd, iItem);
		_ASSERT(phdi);

		lpCur = IDC_ARROW;              // default to the arrow

		switch (phdi->type & HDFT_ISMASK)
		{
		case HDFT_ISSTRING:
		case HDFT_ISNUMBER:
			lpCur = IDC_IBEAM;
			break;

		default:
			// BUGBUG: handle custom filters
			break;
		}
		break;
	}

	default:
		lpCur = IDC_ARROW;
		break;
	}
	SetCursor(LoadCursor(NULL, lpCur));
	return TRUE;
}

void Header_DrawDivider(HD *phd, int x)
{
	RECT rc;
	HDC hdc = GetDC(GetHostWnd(phd->hwnd));

	GetClientRect(phd->hwnd, &rc);
	rc.left = x;
	rc.right = x + GetSystemMetrics(SM_CXBORDER);

	InvertRect(hdc, &rc);

	ReleaseDC(GetHostWnd(phd->hwnd), hdc);
}

int Header_PinDividerPos(HD *phd, int x)
{
	x += phd->dxTrack;
	if (x < phd->xMinTrack)
	{
		x = phd->xMinTrack;
	}
	return x;
}

void Header_OnLButtonDown(HD *phd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	HD_ITEM hd;
	int i;
	UINT flags;

	if (!phd)
	{
		return;
	}

	Header_StopFilterEdit(phd, FALSE);

	i = Header_HitTest(phd, x, y, &flags);
	if (flags & (HHT_ONDIVIDER))
	{
		if (fDoubleClick)
		{
			Header_SendChange(phd, i, HDN_DIVIDERDBLCLICK, NULL);
		}
	}

	if ((flags & (HHT_ONDIVIDER | HHT_ONHEADER | HHT_ONDIVOPEN))
		&& !fDoubleClick)
	{
		phd->iTrack = i;
		phd->flagsTrack = flags;
		phd->xTrack = x;
		SetCapture(phd->hwnd);

		// this is just to get messages so we can
		// check for the escape key being hit
		SetTimer(GetHostWnd(phd->hwnd), 1, 100, NULL);
		GetAsyncKeyState(VK_ESCAPE);
	}

	if (flags & (HHT_ONDIVIDER | HHT_ONDIVOPEN) &&
		!fDoubleClick)
	{
		//
		// We should first send out the HDN_BEGINTRACK notification
		//
		HDI *phdi;

		int iOrder = Header_OnGetItemOrder(phd, i);
		phdi = Header_GetItemPtr(phd, i);
		phd->xMinTrack = phdi->x - phdi->cxy;
		phd->xTrack = phdi->x;
		phd->dxTrack = phd->xTrack - x;
		phd->xTrackOldWidth = phdi->cxy;

		hd.mask = HDI_WIDTH;
		hd.cxy = phd->xTrackOldWidth;
		if (!Header_SendChange(phd, i, HDN_BEGINTRACK, &hd))
		{
			// They said no!
			phd->flagsTrack = 0;
			CCReleaseCapture(&phd->hwnd->ci);
			KillTimer(GetHostWnd(phd->hwnd), 1);
			return;
		}

		if (!HDDragFullWindows(phd))
		{
			x = Header_PinDividerPos(phd, x);
			Header_DrawDivider(phd, x);
		}
	}
	else if ((flags & HHT_ONHEADER) && (phd->hwnd->ci.style & HDS_BUTTONS))
	{
		if (fDoubleClick)
		{
			Header_SendChange(phd, i, HDN_ITEMDBLCLICK, NULL);
		}
		else
		{
			phd->bTrackPress = TRUE;
			Header_InvalidateItem(phd, phd->iTrack, RDW_INVALIDATE | RDW_ERASE);
		}
	}

	if (flags & HHT_ONFILTER)
	{
		Header_BeginFilterEdit(phd, i);
	}

	if (flags & HHT_ONFILTERBUTTON)
	{
		Header_OnFilterButton(phd, i);
	}
}

void Header_StartDrag(HD *phd, int i, int x, int y)
{
	RECT rc;

	if ((phd->hwnd->ci.style & HDS_DRAGDROP) &&
		Header_Notify(phd, i, MK_LBUTTON, HDN_BEGINDRAG))
	{
		// clear the hot bit and
		// update before we do the BeginDrag so that the save bitmap won't
		// have the hot drawing on it.
		Header_SetHotItem(phd, -1);
		UpdateWindow(GetHostWnd(phd->hwnd));


		phd->himlDrag = Header_OnCreateDragImage(phd, Header_OnGetItemOrder(phd, i));
		if (!phd->himlDrag)
		{
			return;
		}

		// find the delta between the start of the item and the cursor
		Header_OnGetItemRect(phd, i, &rc);
		phd->dxTrack = rc.left - x;

		// 后两个参数是相对Image的坐标
		ImageList_BeginDrag(phd->himlDrag, 0, 0, 0);

		CIUIPoint pt;
		pt.x = x;
		pt.y = 0;
		ClientToScreen(phd->hwnd, &pt);
		ScreenToClient(GetHostWnd(phd->hwnd), &pt);
		ImageList_DragEnter(GetHostWnd(phd->hwnd), pt.x, pt.y);
	}
}

void Header_InvalidateDivider(HD *phd, int iItem)
{
	RECT rc;
	Header_GetDividerRect(phd, iItem, &rc);
	InvalidateRect(GetHostWnd(phd->hwnd), &rc, FALSE);
}

void _Header_SetHotDivider(HD *phd, int iNewOrder)
{
	if (iNewOrder != phd->iNewOrder)
	{
		if (phd->himlDrag)
		{
			ImageList_DragShowNolock(FALSE);
		}
		Header_InvalidateDivider(phd, phd->iNewOrder);
		Header_InvalidateDivider(phd, iNewOrder);
		phd->iNewOrder = iNewOrder;
		UpdateWindow(GetHostWnd(phd->hwnd));
		if (phd->himlDrag)
		{
			ImageList_DragShowNolock(TRUE);
		}
	}
}

LPARAM Header_OnSetHotDivider(HD *phd, BOOL fPos, LPARAM lParam)
{
	int iNewOrder = -1;
	if (fPos)
	{
		RECT rc;
		int y = GET_Y_LPARAM(lParam);
		int x = GET_X_LPARAM(lParam);

		// this means that lParam is the cursor position (in client coordinates)

		GetClientRect(phd->hwnd, &rc);
		InflateRect(&rc, 0, /*g_cyHScroll*/16 * 2);

		// show only if the y point is reasonably close to the header
		// (a la scrollbar)
		if (y >= rc.top &&
			y <= rc.bottom)
		{

			//
			// find out the new insertion point
			//
			if (x <= 0)
			{
				iNewOrder = 0;
			}
			else
			{
				UINT flags;
				int iIndex;
				iIndex = Header_HitTest(phd, x, (rc.top + rc.bottom) / 2, &flags);

				// if we didn't find an item, see if it's on the far right
				if (iIndex == -1)
				{

					int iLast = Header_ItemOrderToIndex(phd, Header_GetCount(phd) - 1);
					if (Header_OnGetItemRect(phd, iLast, &rc))
					{
						if (x >= rc.right)
						{
							iNewOrder = Header_GetCount(phd);
						}
					}

				}
				else
				{
					Header_OnGetItemRect(phd, iIndex, &rc);
					iNewOrder = Header_OnGetItemOrder(phd, iIndex);
					// if it was past the midpoint, the insertion point is the next one
					if (x > ((rc.left + rc.right) / 2))
					{
						// get the next item... translate to item order then back to index.
						iNewOrder++;
					}
				}
			}
		}
	}
	else
	{
		iNewOrder = (int)lParam;
	}
	_Header_SetHotDivider(phd, iNewOrder);
	return iNewOrder;
}

void Header_MoveDrag(HD *phd, int x, int y)
{
	LPARAM iNewOrder = -1;

	iNewOrder = Header_OnSetHotDivider(phd, TRUE, MAKELONG(x, y));

	if (iNewOrder == -1)
	{
		ImageList_DragShowNolock(FALSE);
	}
	else
	{
		ImageList_DragShowNolock(TRUE);

		CIUIPoint pt(x + phd->dxTrack, 0);
		ClientToScreen(phd->hwnd, &pt);
		ScreenToClient(GetHostWnd(phd->hwnd), &pt);
		ImageList_DragMove(pt.x, pt.y);
	}
}

void Header_EndDrag(HD *phd)
{
	ImageList_EndDrag();
	ImageList_Destroy(phd->himlDrag);
	phd->himlDrag = NULL;
	_Header_SetHotDivider(phd, -1);
}

// iOrder
void Header_GetDividerRect(HD *phd, int iOrder, LPRECT prc)
{
	int iIndex;
	BOOL fLeft;

	if (iOrder == -1)
	{
		SetRectEmpty(prc);
		return;
	}

	// if we're getting the divider slot of < N then
	// it's the left of the rect of item i.
	// otherwise it's the right of the last item.
	if (iOrder < Header_GetCount(phd))
	{
		fLeft = TRUE;
	}
	else
	{
		fLeft = FALSE;
		iOrder--;
	}

	iIndex = Header_ItemOrderToIndex(phd, iOrder);
	Header_OnGetItemRect(phd, iIndex, prc);
	if (fLeft)
	{
		prc->right = prc->left;
	}
	else
	{
		prc->left = prc->right;
	}
	InflateRect(prc, GetSystemMetrics(SM_CXBORDER), 0);
}

void Header_OnMouseMove(HD *phd, int x, int y, UINT keyFlags)
{
	UINT flags;
	int i;
	HD_ITEM hd;

	if (!phd)
	{
		return;
	}

	// do the hot tracking
	// but not if anything is ownerdraw or if we're in d/d mode
	if ((phd->hwnd->ci.style & HDS_HOTTRACK) && !phd->fOwnerDraw && !phd->himlDrag)
	{
		// only do this if we're in button mode meaning you can actually click
		if (phd->hwnd->ci.style & HDS_BUTTONS)
		{
			i = Header_HitTest(phd, x, y, &flags);
			Header_SetHotItem(phd, i);
		}
	}

	if (Header_IsTracking(phd))
	{
		if (phd->flagsTrack & (HHT_ONDIVIDER | HHT_ONDIVOPEN))
		{
			x = Header_PinDividerPos(phd, x);

			//
			// Let the Owner have a chance to update this.
			//
			hd.mask = HDI_WIDTH;
			hd.cxy = x - phd->xMinTrack;
			if (!HDDragFullWindows(phd) && !Header_SendChange(phd, phd->iTrack, HDN_TRACK, &hd))
			{
				// We need to cancel tracking
				phd->flagsTrack = 0;
				CCReleaseCapture(&phd->hwnd->ci);
				KillTimer(GetHostWnd(phd->hwnd), 1);

				// Undraw the last divider we displayed
				Header_DrawDivider(phd, phd->xTrack);
				return;
			}

			// We should update our x depending on what caller did
			x = hd.cxy + phd->xMinTrack;

			// if full window track is turned on, go ahead and set the width
			if (HDDragFullWindows(phd))
			{
				HD_ITEM item;

				item.mask = HDI_WIDTH;
				item.cxy = hd.cxy;

				DebugMsg(DM_TRACE, TEXT("Tracking header.  item %d gets width %d...  %d %d"), phd->iTrack, item.cxy, phd->xMinTrack, x);
				// Let the owner have a chance to say yes.
				Header_OnSetItem(phd, phd->iTrack, &item);

				InvalidateRect(phd->hwnd, NULL);
				InvalidateRect(GetHostWnd(phd->hwnd), NULL, FALSE);
			}
			else
			{

				// do the cheezy old stuff
				Header_DrawDivider(phd, phd->xTrack);
				Header_DrawDivider(phd, x);
			}

			phd->xTrack = x;

		}
		else if (phd->flagsTrack & HHT_ONHEADER)
		{
			i = Header_HitTest(phd, x, y, &flags);

			if (abs(x - phd->xTrack) >
				GetSystemMetrics(SM_CXDRAG))
			{
				if (!phd->himlDrag)
				{
					Header_StartDrag(phd, i, phd->xTrack, y);
				}
			}

			if (phd->himlDrag)
			{
				Header_MoveDrag(phd, x, y);
			}
			else
			{
				// if pressing on button and it's not pressed, press it
				if (flags & HHT_ONHEADER && i == phd->iTrack)
				{
					if ((!phd->bTrackPress) && (phd->hwnd->ci.style & HDS_BUTTONS))
					{
						phd->bTrackPress = TRUE;
						Header_InvalidateItem(phd, phd->iTrack, RDW_INVALIDATE | RDW_ERASE);
					}
				}
				// tracked off of button.  if pressed, pop it
				else if ((phd->bTrackPress) && (phd->hwnd->ci.style & HDS_BUTTONS))
				{
					phd->bTrackPress = FALSE;
					Header_InvalidateItem(phd, phd->iTrack, RDW_INVALIDATE | RDW_ERASE);
				}
			}
		}
	}
}

void Header_OnLButtonUp(HD *phd, int x, int y, UINT keyFlags)
{
	if (phd == NULL)
	{
		return;
	}

	if (Header_IsTracking(phd))
	{
		if (phd->flagsTrack & (HHT_ONDIVIDER | HHT_ONDIVOPEN))
		{
			HD_ITEM item;

			if (!HDDragFullWindows(phd))
			{
				Header_DrawDivider(phd, phd->xTrack);
			}

			item.mask = HDI_WIDTH;
			item.cxy = phd->xTrack - phd->xMinTrack;

			// Let the owner have a chance to say yes.


			if (Header_SendChange(phd, phd->iTrack, HDN_ENDTRACK, &item))
			{
				Header_OnSetItem(phd, phd->iTrack, &item);
			}

			RedrawWindow(GetHostWnd(phd->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
		else if ((phd->flagsTrack & HHT_ONHEADER)
			&& (phd->bTrackPress || phd->himlDrag))
		{
			if (phd->himlDrag)
			{
				// if this is the end of a drag,
				// notify the user.
				HDITEM item;

				item.mask = HDI_ORDER;
				item.iOrder = phd->iNewOrder; // BUGBUG:  FIXTHIS!


				if (item.iOrder > Header_OnGetItemOrder(phd, phd->iTrack))
				{
					// if the new order is greater than the old one,
					// we subtract one because it's leaving the old place
					// which decs the count by one.
					item.iOrder--;
				}

				Header_EndDrag(phd);

				if (Header_SendChange(phd, phd->iTrack, HDN_ENDDRAG, &item))
				{
					if (item.iOrder != -1)
					{
						// all's well... change the item order
						Header_OnSetItemOrder(phd, phd->iTrack, item.iOrder);

						MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(phd->hwnd), OBJID_CLIENT, 0);
					}
				}

			}
			else
			{
				// Notify the owner that the item has been clicked
				Header_Notify(phd, phd->iTrack, 0, HDN_ITEMCLICK);
			}
			phd->bTrackPress = FALSE;
			Header_InvalidateItem(phd, phd->iTrack, RDW_INVALIDATE | RDW_ERASE);
		}

		phd->flagsTrack = 0;
		CCReleaseCapture(&phd->hwnd->ci);
		KillTimer(GetHostWnd(phd->hwnd), 1);
	}
}


BOOL Header_IsTracking(HD *phd)
{
	if (!phd->flagsTrack)
	{
		return FALSE;
	}
	else if (GetCapture() != GetHostWnd(phd->hwnd))
	{
		phd->flagsTrack = 0;
		return FALSE;
	}

	return TRUE;
}

void Header_OnSetFont(HD *phd, HFONT hfont, BOOL fRedraw)
{
	if (!phd)
	{
		return;
	}

	if (hfont != phd->hfont)
	{
		Header_NewFont(phd, hfont);

		if (fRedraw)
		{
			RedrawWindow(GetHostWnd(phd->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
	}
}

HFONT Header_OnGetFont(HD *phd)
{
	if (!phd)
	{
		return NULL;
	}

	return phd->hfont;
}

//**********************************************************************

#ifdef UNICODE
int Header_OnInsertItemA(HD *phd, int i, HD_ITEMA *pitem)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	HD_TEXTFILTERW textFilterW;
	LPHD_TEXTFILTERA ptextFilterA = NULL;
	int iRet;


	//HACK ALERT -- this code assumes that HD_ITEMA is exactly the same
	// as HD_ITEMW except for the pointer to the string.
	_ASSERT(sizeof(HD_ITEMA) == sizeof(HD_ITEMW));

	if (!pitem || !phd)
	{
		return -1;
	}

	if ((pitem->mask & HDI_TEXT) && (pitem->pszText != LPSTR_TEXTCALLBACKA) &&
		(pitem->pszText != NULL))
	{
		pszC = pitem->pszText;
		if ((pszW = ProduceWFromA(phd->hwnd->ci.uiCodePage, pszC)) == NULL)
		{
			return -1;
		}
		pitem->pszText = (LPSTR)pszW;
	}

	if ((pitem->mask & HDI_FILTER) &&
		((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
	{
		// pick up the filter if there is one for us to thunk
		if (pitem->pvFilter)
		{
			ptextFilterA = (LPHD_TEXTFILTERA)pitem->pvFilter;
			_ASSERT(ptextFilterA);

			textFilterW.pszText = NULL;
			textFilterW.cchTextMax = ptextFilterA->cchTextMax;

			if (!(pitem->type & HDFT_HASNOVALUE))
			{
				textFilterW.pszText = ProduceWFromA(phd->hwnd->ci.uiCodePage, ptextFilterA->pszText);
				if (!textFilterW.pszText)
				{
					if (pszW)
						FreeProducedString(pszW)
						return -1;
				}
			}

			pitem->pvFilter = &textFilterW;
		}
	}

	iRet = Header_OnInsertItem(phd, i, (const HD_ITEM *) pitem);

	if (pszW != NULL)
	{
		pitem->pszText = pszC;

		FreeProducedString(pszW);
	}

	if (ptextFilterA)
	{
		pitem->pvFilter = ptextFilterA;
		FreeProducedString(textFilterW.pszText);
	}

	return iRet;
}
#endif

int Header_OnInsertItem(HD *phd, int i, const HD_ITEM *pitem)
{
	HDI hdi = {0};
	int x;
	HDI *phdi;
	int iOrder;
	int cxy;

	if (!pitem || !phd)
	{
		return -1;
	}

	if (pitem->mask == 0)
	{
		return -1;
	}

	cxy = pitem->cxy;
	if (cxy < 0)
	{
		cxy = 0;
	}

	x = cxy;

	if (i > DSA_GetItemCount(phd->hdsaHDI))
	{
		i = DSA_GetItemCount(phd->hdsaHDI);
	}

	// stop editing the filter
	Header_StopFilterEdit(phd, FALSE);

	iOrder = i;
	// can't have order info if it's owner data
	if (!(phd->hwnd->ci.style & HDS_OWNERDATA))
	{

		// the iOrder field wasn't there in win95...
		// so access it only if the bit is there.
		if (pitem->mask & HDI_ORDER)
		{

			if ((pitem->iOrder != i) && (pitem->iOrder <= Header_GetCount(phd)))
			{
				if (Header_InitOrderArray(phd))
				{
					iOrder = pitem->iOrder;
				}
			}
		}
	}

	if (iOrder > 0)
	{

		phdi = Header_GetItemPtrByOrder(phd, iOrder - 1);
		if (phdi)
		{
			x += phdi->x;
		}

	}

	// move everything else over
	Header_ShiftItems(phd, iOrder, cxy);

	if (phd->hdsaOrder != NULL)
	{
		int j;
		int iIndex;

		// an index is added, all the current indices
		// need to be incr by one
		for (j = 0; j < DSA_GetItemCount(phd->hdsaOrder); j++)
		{
			IUI_DSA_GetItem(phd->hdsaOrder, j, &iIndex);
			if (iIndex >= i)
			{
				iIndex++;
				IUI_DSA_SetItem(phd->hdsaOrder, j, &iIndex);
			}
		}
		IUI_DSA_InsertItem(phd->hdsaOrder, iOrder, &i);
	}

	hdi.x = x;
	hdi.lParam = pitem->lParam;
	hdi.fmt = pitem->fmt;
	//hdi.pszText = NULL;
	//hdi.iImage = 0;
	hdi.cxy = cxy;
	hdi.xText = hdi.xBm = RECOMPUTE;
	hdi.type = HDFT_ISSTRING | HDFT_HASNOVALUE;
	//hdi.textFilter.pszText = NULL;
	hdi.textFilter.cchTextMax = MAX_PATH;

	if ((pitem->mask & HDI_TEXT) && (pitem->pszText != NULL))
	{
		if (!Str_Set(&hdi.pszText, pitem->pszText))
		{
			return -1;
		}

		// Unless ownerdraw make sure the text bit is on!
		if ((pitem->mask & HDF_OWNERDRAW) == 0)
		{
			hdi.fmt |= HDF_STRING;
		}
	}
	else
	{
		hdi.fmt &= ~(HDF_STRING);
	}

	if ((pitem->mask & HDI_BITMAP) && (pitem->hbm != NULL))
	{

		hdi.hbm = pitem->hbm;

		// Unless ownerdraw make sure the text bit is on!
		if ((pitem->mask & HDF_OWNERDRAW) == 0)
		{
			hdi.fmt |= HDF_BITMAP;
		}
	}
	else
	{
		hdi.hbm = NULL;
		hdi.fmt &= ~(HDF_BITMAP);
	}

	if (pitem->mask & HDI_IMAGE)
	{
		hdi.iImage = pitem->iImage;

		// Unless ownerdraw make sure the image bit is on!
		if ((pitem->mask & HDF_OWNERDRAW) == 0)
		{
			hdi.fmt |= HDF_IMAGE;
		}
	}

	if (pitem->mask & HDI_FILTER)
	{
		// pick up the new filter, handling the case where the filter value is
		// being discarded, and/or there is none

		hdi.type = pitem->type;

		switch (hdi.type & HDFT_ISMASK)
		{
		case HDFT_ISSTRING:
		{
			if (pitem->pvFilter)
			{
				LPHDTEXTFILTER ptextFilter = (LPHDTEXTFILTER)pitem->pvFilter;
				_ASSERT(ptextFilter);

				if (!(pitem->type & HDFT_HASNOVALUE))
				{
					Str_Set(&hdi.textFilter.pszText, ptextFilter->pszText);
				}
				hdi.textFilter.cchTextMax = ptextFilter->cchTextMax;
			}
			break;
		}

		case HDFT_ISNUMBER:
		{
			if (!(pitem->type & HDFT_HASNOVALUE) && pitem->pvFilter)
			{
				hdi.intFilter = *((int *)pitem->pvFilter);
			}
			break;
		}
		}
	}


	i = IUI_DSA_InsertItem(phd->hdsaHDI, i, &hdi);
	if (i == -1)
	{
		// failed to add
		Str_Set(&hdi.pszText, NULL);
		if ((hdi.type & HDFT_ISMASK) == HDFT_ISSTRING)
		{
			Str_Set(&hdi.textFilter.pszText, NULL);
		}
	}
	else
	{
		RECT rc;

		// succeeded!  redraw
		GetClientRect(phd->hwnd, &rc);
		rc.left = x - cxy;
		RedrawWindow(GetHostWnd(phd->hwnd), &rc, NULL, RDW_INVALIDATE | RDW_ERASE);

		MyNotifyWinEvent(EVENT_OBJECT_CREATE, GetHostWnd(phd->hwnd), OBJID_CLIENT, i + 1);
	}

	return i;
}

BOOL Header_OnDeleteItem(HD *phd, int i)
{
	HDI hdi;
	RECT rc;
	int iWidth;
	int iOrder;

	if (!phd)
	{
		return FALSE;
	}

	if (!IUI_DSA_GetItem(phd->hdsaHDI, i, &hdi))
	{
		return FALSE;
	}

	MyNotifyWinEvent(EVENT_OBJECT_DESTROY, GetHostWnd(phd->hwnd), OBJID_CLIENT, i + 1);

	Header_StopFilterEdit(phd, FALSE);
	phd->iFocus = 0;

	GetClientRect(phd->hwnd, &rc);
	iWidth = rc.right;
	Header_OnGetItemRect(phd, i, &rc);
	InflateRect(&rc, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));

	// move everything else over
	iOrder = Header_OnGetItemOrder(phd, i);
	Header_ShiftItems(phd, iOrder, -hdi.cxy);

	if (!IUI_DSA_DeleteItem(phd->hdsaHDI, i))
	{
		return FALSE;
	}

	if (phd->hdsaOrder)
	{
		int j;
		int iIndex;
		IUI_DSA_DeleteItem(phd->hdsaOrder, iOrder);


		// an index is going away, all the current indices
		// need to be decremented by one
		for (j = 0; j < DSA_GetItemCount(phd->hdsaOrder); j++)
		{
			IUI_DSA_GetItem(phd->hdsaOrder, j, &iIndex);
			_ASSERT(iIndex != i);
			if (iIndex > i)
			{
				iIndex--;
				IUI_DSA_SetItem(phd->hdsaOrder, j, &iIndex);
			}
		}

	}

	Header_DestroyItemCallback(&hdi, NULL);

	rc.right = iWidth;
	InvalidateRect(GetHostWnd(phd->hwnd), &rc, TRUE);
	return TRUE;
}

#ifdef UNICODE
BOOL Header_OnGetItemA(HD *phd, int i, HD_ITEMA *pitem)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	HD_TEXTFILTERW textFilterW;
	LPHD_TEXTFILTERA ptextFilterA = NULL;
	BOOL fRet;

	//HACK ALERT -- this code assumes that HD_ITEMA is exactly the same
	// as HD_ITEMW except for the pointer to the string.
	_ASSERT(sizeof(HD_ITEMA) == sizeof(HD_ITEMW));

	if (!pitem || !phd)
	{
		return FALSE;
	}

	if ((pitem->mask & HDI_TEXT) && (pitem->pszText != LPSTR_TEXTCALLBACKA) &&
		(pitem->pszText != NULL))
	{
		pszC = pitem->pszText;
		pszW = (LPWSTR)LocalAlloc(LMEM_FIXED, pitem->cchTextMax * sizeof(WCHAR));
		if (pszW == NULL)
		{
			return FALSE;
		}
		pitem->pszText = (LPSTR)pszW;
	}

	if ((pitem->mask & HDI_FILTER) &&
		((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
	{
		if (pitem->pvFilter)
		{
			ptextFilterA = (LPHD_TEXTFILTERA)pitem->pvFilter;
			_ASSERT(ptextFilterA);

			textFilterW.pszText = (LPWSTR)LocalAlloc(LMEM_FIXED, ptextFilterA->cchTextMax * sizeof(WCHAR));
			textFilterW.cchTextMax = ptextFilterA->cchTextMax;

			if (!textFilterW.pszText)
			{
				if (pszW)
				{
					LocalFree(pszW);
				}
				return FALSE;
			}

			pitem->pvFilter = &textFilterW;
		}
	}

	fRet = Header_OnGetItem(phd, i, (HD_ITEM *) pitem);

	if (pszW != NULL)
	{
		ConvertWToAN(phd->hwnd->ci.uiCodePage, pszC, pitem->cchTextMax, pszW, -1);
		pitem->pszText = pszC;

		LocalFree(pszW);
	}

	if (ptextFilterA)
	{
		ConvertWToAN(phd->hwnd->ci.uiCodePage, ptextFilterA->pszText, ptextFilterA->cchTextMax,
			textFilterW.pszText, -1);
		pitem->pvFilter = ptextFilterA;
	}

	return fRet;
}
#endif

BOOL Header_OnGetItem(HD *phd, int i, HD_ITEM *pitem)
{
	HDI *phdi;
	UINT mask;
	NMHDDISPINFO nm;

	_ASSERT(pitem);

	if (!pitem || !phd)
	{
		return FALSE;
	}

	// Crappy hack to fix norton commander.  MFC has a bug where it
	// passes in stack trash (in addition to the desired bits) to HDM_GETITEM.
	// Fix it here by stripping down to Win95 bits if more bits than the
	// current valid bits are defined.
	if (pitem->mask & ~HDI_ALL)
	{
		pitem->mask &= HDI_ALL95;
	}

	nm.mask = 0;
	mask = pitem->mask;

#ifdef DEBUG
	if (i < 0 || i >= Header_GetCount(phd))
	{
		RIPMSG(0, "HDM_GETITEM: Invalid item number %d", i);
		return FALSE; // Return immediately so Header_GetItemPtr doesn't assert
	}
#endif

	phdi = Header_GetItemPtr(phd, i);
	if (!phdi)
	{
		return FALSE;
	}

	if (mask & HDI_WIDTH)
	{
		pitem->cxy = phdi->cxy;
	}

	if (mask & HDI_FORMAT)
	{
		pitem->fmt = phdi->fmt;
	}

	if (mask & HDI_ORDER)
	{
		pitem->iOrder = Header_OnGetItemOrder(phd, i);
	}

	if (mask & HDI_LPARAM)
	{
		pitem->lParam = phdi->lParam;
	}

	if (mask & HDI_TEXT)
	{
		if (phdi->pszText != LPSTR_TEXTCALLBACK)
		{

			// BUGBUG: warning... this is different than Chicago behavior.
			// if pszText was NULL and you tried to retrieve it, we would bail
			// and return FALSE, now we may return TRUE.
			IUIStr_GetPtr(phdi->pszText ? phdi->pszText : _T(""), pitem->pszText, pitem->cchTextMax);
		}
		else
		{
			// need to recalc the xText because they could keep changing it on us
			phdi->xText = RECOMPUTE;
			nm.mask |= HDI_TEXT;
		}
	}

	if (mask & HDI_BITMAP)
	{
		pitem->hbm = phdi->hbm;
	}

	if (mask & HDI_IMAGE)
	{
		if (phdi->iImage == I_IMAGECALLBACK)
		{
			nm.mask |= HDI_IMAGE;
		}
		else
		{
			pitem->iImage = phdi->iImage;
		}
	}

	if (mask & HDI_FILTER)
	{
		if (pitem->pvFilter)
		{
			if ((phdi->type & HDFT_ISMASK) != (pitem->type & HDFT_ISMASK))
			{
				return FALSE;
			}

			switch (phdi->type & HDFT_ISMASK)
			{
			case HDFT_ISSTRING:
			{
				LPHDTEXTFILTER ptextFilter = (LPHDTEXTFILTER)pitem->pvFilter;
				_ASSERT(ptextFilter);

				if (!IUIStr_GetPtr(phdi->textFilter.pszText, ptextFilter->pszText, ptextFilter->cchTextMax))
				{
					return FALSE;
				}

				ptextFilter->cchTextMax = phdi->textFilter.cchTextMax;
				break;
			}

			case HDFT_ISNUMBER:
			{
				*((int *)pitem->pvFilter) = phdi->intFilter;
				break;
			}

			default:
				return FALSE;
			}
		}

		pitem->type = phdi->type;
	}

	if (nm.mask)
	{
		// just in case HDI_IMAGE is set and callback doesn't fill it in
		// ... we'd rather have a -1 than watever garbage is on the stack
		nm.iImage = -1;
		nm.lParam = phdi->lParam;

		if (nm.mask & HDI_TEXT)
		{
			_ASSERT(pitem->pszText);
			nm.pszText = pitem->pszText;
			nm.cchTextMax = pitem->cchTextMax;

			// Make sure the buffer is zero terminated...
			if (nm.cchTextMax)
			{
				*nm.pszText = 0;
			}
		}

		CCSendNotify(&phd->hwnd->ci, HDN_GETDISPINFO, &nm.hdr);

		if (nm.mask & HDI_IMAGE)
		{
			pitem->iImage = nm.iImage;
		}
		if (nm.mask & HDI_TEXT)
			if (mask & HDI_TEXT)
			{
				pitem->pszText = CCReturnDispInfoText(nm.pszText, pitem->pszText, pitem->cchTextMax);
			}
			else
			{
				pitem->pszText = nm.pszText;
			}
	}

	if (phdi && (nm.mask & HDI_DI_SETITEM))
	{
		if (nm.mask & HDI_IMAGE)
		{
			phdi->iImage = nm.iImage;
		}

		if (nm.mask & HDI_TEXT)
			if (nm.pszText)
			{
				_ASSERT(phdi->pszText == LPSTR_TEXTCALLBACK);
				Str_Set(&phdi->pszText, nm.pszText);
			}
	}

	pitem->mask = mask;
	return TRUE;
}

#ifdef UNICODE
BOOL Header_OnSetItemA(HD *phd, int i, HD_ITEMA *pitem)
{
	LPWSTR pszW = NULL;
	LPSTR pszC = NULL;
	HD_TEXTFILTERW textFilterW;
	LPHD_TEXTFILTERA ptextFilterA = NULL;
	BOOL fRet;

	//HACK ALERT -- this code assumes that HD_ITEMA is exactly the same
	// as HD_ITEMW except for the pointer to the string.
	_ASSERT(sizeof(HD_ITEMA) == sizeof(HD_ITEMW));

	if (!pitem || !phd)
	{
		return FALSE;
	}


	if ((pitem->mask & HDI_TEXT) && (pitem->pszText != LPSTR_TEXTCALLBACKA) &&
		(pitem->pszText != NULL))
	{
		pszC = pitem->pszText;
		if ((pszW = ProduceWFromA(phd->hwnd->ci.uiCodePage, pszC)) == NULL)
		{
			return FALSE;
		}
		pitem->pszText = (LPSTR)pszW;
	}

	if ((pitem->mask & HDI_FILTER) &&
		((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
	{
		if (pitem->pvFilter)
		{
			ptextFilterA = (LPHD_TEXTFILTERA)pitem->pvFilter;
			_ASSERT(ptextFilterA);

			textFilterW.pszText = NULL;
			textFilterW.cchTextMax = ptextFilterA->cchTextMax;

			if (!(pitem->type & HDFT_HASNOVALUE))
			{
				textFilterW.pszText = ProduceWFromA(phd->hwnd->ci.uiCodePage, ptextFilterA->pszText);
				if (!textFilterW.pszText)
				{
					if (pszW)
						FreeProducedString(pszW)
						return FALSE;
				}
			}

			pitem->pvFilter = &textFilterW;
		}
	}

	fRet = Header_OnSetItem(phd, i, (const HD_ITEM *) pitem);

	if (pszW != NULL)
	{
		pitem->pszText = pszC;
		FreeProducedString(pszW);
	}

	if (ptextFilterA)
	{
		pitem->pvFilter = ptextFilterA;
		FreeProducedString(textFilterW.pszText);
	}

	return fRet;

}
#endif

BOOL Header_OnSetItem(HD *phd, int i, const HD_ITEM *pitem)
{
	HDI *phdi;
	UINT mask;
	int xOld;
	BOOL fInvalidate = FALSE;

	_ASSERT(pitem);

	if (!pitem || !phd)
	{
		return FALSE;
	}

#ifdef DEBUG
	if (i < 0 || i >= Header_GetCount(phd))
	{
		RIPMSG(0, "HDM_SETITEM: Invalid item number %d", i);
		return FALSE; // Return immediately so Header_GetItemPtr doesn't assert
	}
#endif

	phdi = Header_GetItemPtr(phd, i);
	if (!phdi)
	{
		return FALSE;
	}

	mask = pitem->mask;

	if (mask == 0)
	{
		return TRUE;
	}

	// stop editing the filter
	//Header_StopFilterEdit(phd, FALSE);

	if (!Header_SendChange(phd, i, HDN_ITEMCHANGING, pitem))
	{
		return FALSE;
	}

	xOld = phdi->x;
	if (mask & HDI_WIDTH)
	{
		RECT rcClip;
		int iOrder;
		int dx;
		int cxy = pitem->cxy;

		if (cxy < 0)
		{
			cxy = 0;
		}

		DebugMsg(DM_TRACE, TEXT("Header--SetWidth x=%d, cxyOld=%d, cxyNew=%d, dx=%d"),
			phdi->x, phdi->cxy, cxy, (cxy - phdi->cxy));
		dx = cxy - phdi->cxy;
		phdi->cxy = cxy;

		// scroll everything over
		GetClientRect(phd->hwnd, &rcClip);
		rcClip.left = phdi->x; // we want to scroll the divider as well

		// the scrolling rect needs to be the largest rect of the before
		// and after.  so if dx is negative, we want to enlarge the rect
		if (dx < 0)
		{
			rcClip.left += dx;
		}
		iOrder = Header_OnGetItemOrder(phd, i);
		Header_ShiftItems(phd, iOrder, dx);

		phdi->xText = phdi->xBm = RECOMPUTE;

		// now invalidate this item itself
		Header_OnGetItemRect(phd, i, &rcClip);
		InvalidateRect(phd->hwnd, &rcClip);

	}
	if (mask & HDI_FORMAT)
	{
		phdi->fmt = pitem->fmt;
		phdi->xText = phdi->xBm = RECOMPUTE;
		fInvalidate = TRUE;
	}
	if (mask & HDI_LPARAM)
	{
		phdi->lParam = pitem->lParam;
	}

	if (mask & HDI_TEXT)
	{
		if (!Str_Set(&phdi->pszText, pitem->pszText))
		{
			return FALSE;
		}
		phdi->xText = RECOMPUTE;
		fInvalidate = TRUE;
	}

	if (mask & HDI_BITMAP)
	{
		phdi->hbm = pitem->hbm;

		phdi->xBm = RECOMPUTE;
		fInvalidate = TRUE;
	}

	if (mask & HDI_IMAGE)
	{
		phdi->iImage = pitem->iImage;
		phdi->xBm = RECOMPUTE;
		fInvalidate = TRUE;
	}

	if (mask & HDI_ORDER)
	{
		if (pitem->iOrder >= 0 && pitem->iOrder < Header_GetCount(phd))
		{
			Header_OnSetItemOrder(phd, i, pitem->iOrder);
			MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(phd->hwnd), OBJID_CLIENT, 0);
		}
	}

	if (mask & HDI_FILTER)
	{
		if ((phdi->type & HDFT_ISMASK) == HDFT_ISSTRING)
		{
			Str_Set(&phdi->textFilter.pszText, NULL);
		}

		// pick up the new filter, handling the case where the filter value is
		// being discarded, and/or there is none

		phdi->type = pitem->type;

		switch (phdi->type & HDFT_ISMASK)
		{
		case HDFT_ISSTRING:
		{
			if (pitem->pvFilter)
			{
				LPHDTEXTFILTER ptextFilter = (LPHDTEXTFILTER)pitem->pvFilter;
				_ASSERT(ptextFilter);

				if (!(pitem->type & HDFT_HASNOVALUE))
				{
					Str_Set(&phdi->textFilter.pszText, ptextFilter->pszText);
				}
				phdi->textFilter.cchTextMax = ptextFilter->cchTextMax;
			}
			break;
		}

		case HDFT_ISNUMBER:
		{
			if (!(pitem->type & HDFT_HASNOVALUE) && pitem->pvFilter)
			{
				phdi->intFilter = *((int *)pitem->pvFilter);
			}
			break;
		}
		}

		fInvalidate = TRUE;
	}

	Header_SendChange(phd, i, HDN_ITEMCHANGED, pitem);

	if (mask & HDI_FILTER)
	{
		Header_Notify(phd, i, 0, HDN_FILTERCHANGE);    // send out a notify of change
	}

	if (fInvalidate)
	{
		if (xOld == phdi->x)
		{
			// no change in x
			Header_InvalidateItem(phd, i, RDW_INVALIDATE | RDW_ERASE);
		}
		else
		{
			RECT rc;
			GetClientRect(phd->hwnd, &rc);

			if (i > 0)
			{
				HDI *phdiTemp;
				phdiTemp = Header_GetItemPtrByOrder(phd, i - 1);
				if (phdiTemp)
				{
					rc.left = phdi->x;
				}
			}
			RedrawWindow(GetHostWnd(phd->hwnd), &rc, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
	}

	return TRUE;
}

// Compute layout for header bar, and leftover rectangle.
//
// HD_LAYOUT::prc: [in out]传入Header旧坐标，返回值的top表示Report ListView第0个Item的top坐标
// 其它的left, right和bottom值不变。
// HD_LAYOUT::pwpos [out] 返回Header新的坐标（如果Header包含Filter Bar，则新坐标也包含
// FilterBar）.
// 注意：HD_LAYOUT的两个成员都是指针，调用者需要为它们分配内存。
BOOL Header_OnLayout(HD *phd, HD_LAYOUT *playout)
{
	RIPMSG(playout != NULL, "HDM_LAYOUT: Invalid NULL pointer");

	if (playout == NULL || phd == NULL)
	{
		return FALSE;
	}

	if (playout->pwpos == NULL || playout->prc == NULL)
	{
		return FALSE;
	}

	int cyHeader;
	WINDOWPOS *pwpos = playout->pwpos;
	RECT *prc = playout->prc;

	cyHeader = phd->cyChar;// + 2 * GetSystemMetrics(SM_CYEDGE);

	// 如果有FilterBar，则留出FilterBar的空间。
	// FilterBar是Header下面的输入框。
	if (phd->hwnd->ci.style & HDS_FILTERBAR)
	{
		cyHeader += phd->cyChar + c_cyFilterBarEdge;
	}

	// BUGBUG: we should store the style at creat  time
	// internal hack style for use with LVS_REPORT|LVS_NOCOLUMNHEADER! edh
	if (phd->hwnd->ci.style & HDS_HIDDEN)
	{
		cyHeader = 0;
	}

	pwpos->hwndInsertAfter = NULL;
	pwpos->flags = SWP_NOZORDER | SWP_NOACTIVATE;

	// BUGBUG: _ASSERT(phd->style & HDS_HORZ);

	pwpos->x  = prc->left;
	pwpos->cx = prc->right - prc->left;
	pwpos->y  = prc->top;
	pwpos->cy = cyHeader;

	prc->top += cyHeader;

	return TRUE;
}

BOOL Header_OnGetItemRect(HD *phd, int i, RECT *prc)
{
	HDI *phdi;

	phdi = Header_GetItemPtr(phd, i);
	if (!phdi)
	{
		return FALSE;
	}

	GetClientRect(phd->hwnd, prc);

	prc->right = phdi->x;
	prc->left = prc->right - phdi->cxy;
	return TRUE;
}

void Header_InvalidateItem(HD *phd, int i, UINT uFlags)
{
	RECT rc;

	if (i != -1)
	{
		Header_OnGetItemRect(phd, i, &rc);
		InflateRect(&rc, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));
		RedrawWindow(GetHostWnd(phd->hwnd), &rc, NULL, uFlags);
	}
}

int _Header_DrawBitmap(HDC hdc, HIMAGELIST himl, HD_ITEM *pitem,
	RECT *prc, int fmt, UINT flags, LPRECT prcDrawn, int iMargin)
{
	// This routine returns either the left of the image
	// or the right of the image depending on the justification.
	// This return value is used in order to properly tack on the
	// bitmap when both the HDF_IMAGE and HDF_BITMAP flags are set.

	RECT rc;
	int xBitmap = 0;
	int yBitmap = 0;
	int cxBitmap;
	int cyBitmap;
	IMAGELISTDRAWPARAMS imldp;
	HBITMAP hbmOld = NULL;
	BITMAP bm;
	HDC hdcMem = NULL;
	int cxRc;

	SetRectEmpty(prcDrawn);

	if (IsRectEmpty(prc))
	{
		return prc->left;
	}

	rc = *prc;

	rc.left  += iMargin;
	rc.right -= iMargin;

	//  rc.right -= g_cxEdge; // handle edge

	if (rc.left >= rc.right)
	{
		return rc.left;
	}

	if (pitem->fmt & HDF_IMAGE)
	{
		ImageList_GetIconSize(himl, &cxBitmap, &cyBitmap);
	}

	else   // pitem->fmt & BITMAP
	{
		if (GetObject(pitem->hbm, sizeof(bm), &bm) != sizeof(bm))
		{
			return rc.left;    // could not get the info about bitmap.
		}


		hdcMem = CreateCompatibleDC(hdc);

		if (!hdcMem || ((hbmOld = (HBITMAP)SelectObject(hdcMem, pitem->hbm)) == ERROR))
		{
			return rc.left;    // an error happened.
		}

		cxBitmap = bm.bmWidth;
		cyBitmap = bm.bmHeight;
	}

	if (flags & SHDT_DEPRESSED)
	{
		OffsetRect(&rc, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));
	}

	// figure out all the formatting...

	cxRc = rc.right - rc.left;          // cache this value

	if (fmt == HDF_LEFT)
	{
		if (cxBitmap > cxRc)
		{
			cxBitmap = cxRc;
		}
	}
	else if (fmt == HDF_CENTER)
	{
		if (cxBitmap > cxRc)
		{
			xBitmap = (cxBitmap - cxRc) / 2;
			cxBitmap = cxRc;
		}
		else
		{
			rc.left = (rc.left + rc.right - cxBitmap) / 2;
		}
	}
	else  // fmt == HDF_RIGHT
	{
		if (cxBitmap > cxRc)
		{
			xBitmap =  cxBitmap - cxRc;
			cxBitmap = cxRc;
		}
		else
		{
			rc.left = rc.right - cxBitmap;
		}
	}

	// Now setup vertically
	if (cyBitmap > (rc.bottom - rc.top))
	{
		yBitmap = (cyBitmap - (rc.bottom - rc.top)) / 2;
		cyBitmap = rc.bottom - rc.top;
	}
	else
	{
		rc.top = (rc.bottom - rc.top - cyBitmap) / 2;
	}


	if (pitem->fmt & HDF_IMAGE)
	{
		imldp.cbSize = sizeof(imldp);
		imldp.himl   = himl;
		imldp.hdcDst = hdc;
		imldp.i      = pitem->iImage;
		imldp.x      = rc.left;
		imldp.y      = rc.top;
		imldp.cx     = cxBitmap;
		imldp.cy     = cyBitmap;
		imldp.xBitmap = xBitmap;
		imldp.yBitmap = yBitmap;
		imldp.rgbBk  = CLR_DEFAULT;
		imldp.rgbFg  = CLR_DEFAULT;
		imldp.fStyle = ILD_NORMAL;

		ImageList_DrawIndirect(&imldp);
	}

	else   // pitem->fmt & HDF_BITMAP
	{

		TraceMsg(TF_HEADER, "h_db: BitBlt to (%d,%d) from (%d, %d)", rc.left, rc.top, xBitmap, yBitmap);
		// Last but not least we will do the bitblt.
		BitBlt(hdc, rc.left, rc.top, cxBitmap, cyBitmap,
			hdcMem, xBitmap, yBitmap, SRCCOPY);

		// Unselect our object from the DC
		SelectObject(hdcMem, hbmOld);

		// Also free any memory dcs we may have created
		DeleteDC(hdcMem);
	}

	*prcDrawn = rc;
	prcDrawn->bottom = rc.top + cyBitmap;
	prcDrawn->right = rc.left + cxBitmap;
	return ((pitem->fmt & HDF_RIGHT) ? rc.left : rc.left + cxBitmap);
}

void Header_DrawButtonEdges(HD *phd, HDC hdc, LPRECT prc, BOOL fItemSunken)
{
	UINT uEdge;
	UINT uBF;
	if (phd->hwnd->ci.style & HDS_BUTTONS)
	{
		if (fItemSunken)
		{
			uEdge = EDGE_SUNKEN;
			uBF = BF_RECT | BF_SOFT | BF_FLAT;
		}
		else
		{
			uEdge = EDGE_RAISED;
			uBF = BF_RECT | BF_SOFT;
		}
	}
	else
	{
		uEdge = EDGE_ETCHED;
		if (phd->hwnd->ci.style & WS_BORDER)
		{
			uBF = BF_RIGHT;
		}
		else
		{
			uBF = BF_BOTTOMRIGHT;
		}
	}

	DrawEdge(hdc, prc, uEdge, uBF);

}

void Header_DrawFilterGlyph(HD *phd, HDC hdc, RECT *prc, BOOL fPressed)
{
	UINT uEdge = BDR_RAISEDOUTER | BDR_RAISEDINNER;
	UINT uBF = BF_RECT;
	RECT rc = *prc;

	if (fPressed)
	{
		uEdge = EDGE_SUNKEN;
		uBF = BF_RECT | BF_SOFT | BF_FLAT;
	}

	//L if (!phd->hFilterImage)
	//{
	//	phd->hFilterImage = ImageList_LoadBitmap(NULL, MAKEINTRESOURCE(IDB_FILTERIMAGE), c_cxFilterImage, 0, RGB(128, 0, 0));
	//	_ASSERT(phd->hFilterImage);

	//	if (!phd->hFilterImage)
	//	{
	//		return;
	//	}
	//}

	DrawEdge(hdc, &rc, uEdge, uBF | BF_MIDDLE);

	if (fPressed)
	{
		OffsetRect(&rc, GetSystemMetrics(SM_CXBORDER), GetSystemMetrics(SM_CYBORDER));
	}

	ImageList_Draw(phd->hFilterImage, 0, hdc,
		rc.left + (((rc.right - rc.left) - c_cxFilterImage) / 2),
		rc.top + (((rc.bottom - rc.top) - c_cyFilterImage) / 2),
		ILD_NORMAL);
}

//
//  Oh boy, here come the pictures.
//
//  For a left-justified header item, the items are arranged like this.
//
//          rcHeader.left                           rcHeader.right
//          |        iTextMargin   iTextMargin       |
//          |        ->| |<-        ->| |<-          |
//          |          | |            | |            |
//          v          |<--textSize-->| |            v
//          +----------------------------------------+
//          | |BMPBMP| | |TEXTTEXTTEXT| |            |
//          +----------------------------------------+
//          |<-bmSize->|              | |
//          | |      | |              | |
//        ->| |<-  ->| |<-            | |
//      iBmMargin iBmMargin           | |
//          |                         | |
//          |<-------cxTextAndBm------->|
//
//
//  For a right-justified header item, the items are arranged like this.
//
//          rcHeader.left                           rcHeader.right
//          |        iBmMargin   iBmMargin           |
//          |          ->| |<-  ->| |<-              |
//          |            | |      | |                |
//          v            |<-bmSize->|                v
//          +----------------------------------------+
//          |            | |BMPBMP| | |TEXTTEXTTEXT| |
//          +----------------------------------------+
//                       |          |<---textSize--->|
//                       |          | |            | |
//                       |        ->| |<-        ->| |<-
//                       |      iTextMargin     iTextMargin
//                       |                           |
//                       |<-------cxTextAndBm------->|
//
//  Obvious variations apply to center-justified, bitmap-on-right, etc.
//  The point is that all the sizes are accounted for in the manner above.
//  There are no gratuitous +1's or g_cxEdge's.
//

void Header_DrawItem(HD *phd, HDC hdc, int i, int iIndex, LPRECT prc, UINT uFlags)
{
	BOOL fTracking = Header_IsTracking(phd);

	RECT rcFilter;
	RECT rcHeader = rcFilter = *prc;         // private copies for us to dork

	// pointer to current header item
	HDI *phdi = Header_GetItemPtrByOrder(phd, i);

	BOOL fItemSunken = (fTracking && (phd->flagsTrack & HHT_ONHEADER) &&
			(phd->iTrack == iIndex) && phd->bTrackPress);

	// Note that SHDT_EXTRAMARGIN requires phd->iTextMargin >= 3*(GetSystemMetrics(SM_CXBORDER) * 2)
	UINT uDrawTextFlags = SHDT_ELLIPSES | SHDT_EXTRAMARGIN | SHDT_CLIPPED;

	if (fItemSunken)
	{
		uDrawTextFlags |= SHDT_DEPRESSED;
	}

	// 给父控件一个绘制Header Item的机会。
	DRAWITEMSTRUCT dis;

	phd->fOwnerDraw = TRUE;

	dis.CtlType = ODT_HEADER;
	dis.CtlID = GetWindowID(GetHostWnd(phd->hwnd));
	dis.itemID = iIndex;
	dis.itemAction = ODA_DRAWENTIRE;
	dis.itemState = (fItemSunken) ? ODS_SELECTED : 0;
	dis.hwndItem = (HWND)phd->hwnd;
	dis.hDC = hdc;
	dis.rcItem = *prc;
	dis.itemData = phdi->lParam;

	// Now send it off to my parent...
	RoutedEventArgs arg;
	arg.hSender = phd->hwnd;
	arg.hOriginalSender = phd->hwnd;
	arg.RoutedEvent = WM_DRAWITEM;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)&dis;
	RaiseEvent(arg.hOriginalSender, &arg);
	if (arg.bHandled)
	{
		return;
	}

	NMCUSTOMDRAW nmcd;
	nmcd.dwItemSpec = iIndex;
	nmcd.hdc = hdc;
	nmcd.rc = *prc;
	nmcd.uItemState = (fItemSunken) ? CDIS_SELECTED : 0;
	nmcd.lItemlParam = phdi->lParam;
#ifdef KEYBOARDCUES
	if (!(CCGetUIState(&(phd->hwnd->ci)) & UISF_HIDEFOCUS))
	{
		nmcd.uItemState |= CDIS_SHOWKEYBOARDCUES;
	}
#endif
	DWORD dwRet = CDRF_DODEFAULT;
	dwRet = CICustomDrawNotify(&phd->hwnd->ci, CDDS_ITEMPREPAINT, &nmcd);

	if (dwRet & CDRF_SKIPDEFAULT)
	{
		return;
	}

	// this is to fetch out any changes the caller might have changed
	COLORREF clrText = GetTextColor(hdc);
	COLORREF clrBk = GetBkColor(hdc);

	//
	// Now neet to handle the different combinatations of
	// text, bitmaps, and images...
	//
	RECT rcButton;
	if (phd->hwnd->ci.style & HDS_FILTERBAR)
	{
		Header_GetFilterRects(prc, &rcHeader, &rcFilter, &rcButton);
	}

	// item text clipping rect
	RECT rcBm;                          // item bitmap clipping rect
	RECT rcText = rcBm = rcHeader;

#ifdef _DEBUG
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		phdi->fmt ^= HDF_BITMAP_ON_RIGHT;
		phdi->xText = RECOMPUTE;
	}
#endif

	HD_ITEM item;                       // used for text callback
	TCHAR ach[CCHLABELMAX];             // used for text callback
	if (phdi->fmt & (HDF_STRING | HDF_IMAGE | HDF_BITMAP))
	{
		item.mask = HDI_TEXT | HDI_IMAGE | HDI_FORMAT | HDI_BITMAP;
		item.pszText = ach;
		item.cchTextMax = ARRAYSIZE(ach);
		Header_OnGetItem(phd, iIndex, &item);
	}

	//
	// 把Header Item当成Button绘制
	//
	int nStateIndex = 0;
	BOOL bDrawButtonImage = FALSE;
	if (phd->m_bCombineButtonBkImage)
	{
		if (phd->m_himgCombineButtonBk != NULL)
		{
			bDrawButtonImage = TRUE;
			IUIPartDrawImage(hdc, prc, phd->m_himgCombineButtonBk,
				phd->m_eButtonBkImageResizeMode, phd->m_rcButtonBkImage9GridArg,
				COMBINEIMAGESIZE4, nStateIndex);
		}
	}
	else
	{
		if (phd->m_himgButtonBk[0] != NULL)
		{
			bDrawButtonImage = TRUE;

			IUIDrawImage(hdc, prc, phd->m_himgCombineButtonBk,
				phd->m_eButtonBkImageResizeMode, phd->m_rcButtonBkImage9GridArg);
		}
	}

	//
	// 如果Header Item包含文本，并且还包含Image或Bitmap
	//
	if ((phdi->fmt & HDF_STRING) && (phdi->fmt & (HDF_BITMAP | HDF_IMAGE)))
	{
		// BEGIN RECOMPUTE ////////////////////////
		//
		if ((phdi->xText == RECOMPUTE) || (phdi->xBm == RECOMPUTE))
		{
			BITMAP bm;                          // used to calculate bitmap width

			// calculate the placement of bitmap rect and text rect
			SIZE textSize, bmSize;
			int dx;

			// get total textwidth
			GetTextExtentPoint(hdc, item.pszText, lstrlen(item.pszText),
				&textSize);
			TraceMsg(TF_HEADER, "h_di: GetTextExtentPoint returns %d", textSize.cx);
			textSize.cx += 2 * phd->iTextMargin;

			// get total bitmap width
			if (phdi->fmt & HDF_IMAGE)
			{
				ImageList_GetIconSize(phd->himl, (LPINT)&bmSize.cx, (LPINT)&bmSize.cy);
			}
			else    // phdi->fmt & HDF_BITMAP
			{
				GetObject(phdi->hbm, sizeof(bm), &bm);
				bmSize.cx = bm.bmWidth;
				TraceMsg(TF_HEADER, "h_di: Bitmap size is %d", bmSize.cx);
			}
			bmSize.cx += 2 * phd->iBmMargin;

			phdi->cxTextAndBm = bmSize.cx + textSize.cx;

			// calculate how much extra space we have, if any.
			dx = rcHeader.right - rcHeader.left - phdi->cxTextAndBm;
			if (dx < 0)
			{
				dx = 0;
				phdi->cxTextAndBm = rcHeader.right - rcHeader.left;
			}

			if (phdi->fmt & HDF_BITMAP_ON_RIGHT)
			{
				switch (phdi->fmt & HDF_JUSTIFYMASK)
				{
				case HDF_LEFT:
					phdi->xText = rcHeader.left;
					break;
				case HDF_RIGHT:
					phdi->xText = rcHeader.right - phdi->cxTextAndBm;
					break;
				case HDF_CENTER:
					phdi->xText = rcHeader.left + dx / 2;
					break;
				}

				// show as much of the bitmap as possible..
				// if we start running out of room, scoot the bitmap
				// back on.
				if (dx == 0)
				{
					phdi->xBm = rcHeader.right - bmSize.cx;
				}
				else
				{
					phdi->xBm = phdi->xText + textSize.cx;
				}

				// clip the values
				if (phdi->xBm < rcHeader.left)
				{
					phdi->xBm = rcHeader.left;
				}
			}
			else   // BITMAP_ON_LEFT
			{
				switch (phdi->fmt & HDF_JUSTIFYMASK)
				{
				case HDF_LEFT:
					phdi->xBm = rcHeader.left;
					break;
				case HDF_RIGHT:
					phdi->xBm = rcHeader.right - phdi->cxTextAndBm;
					break;
				case HDF_CENTER:
					phdi->xBm = rcHeader.left + dx / 2;
					break;
				}
				phdi->xText = phdi->xBm + bmSize.cx;
				// clip the values
				if (phdi->xText > rcHeader.right)
				{
					phdi->xText = rcHeader.right;
				}
			}

			// xBm and xText are now absolute coordinates..
			// change them to item relative coordinates
			phdi->xBm -= rcHeader.left;
			phdi->xText -= rcHeader.left;
			TraceMsg(TF_HEADER, "h_di: phdi->xBm = %d, phdi->xText=%d", phdi->xBm, phdi->xText);
		}

		//
		// END RECOMPUTE /////////////////////////////////

		// calculate text and bitmap rectangles
		//
		rcBm.left = phdi->xBm + rcHeader.left;
		rcText.left = phdi->xText + rcHeader.left;

		if (phdi->fmt & HDF_BITMAP_ON_RIGHT)
		{
			rcBm.right = rcText.left + phdi->cxTextAndBm;
			rcText.right = rcBm.left;
		}
		else   // BITMAP_ON_LEFT
		{
			rcBm.right = rcText.left;
			rcText.right = rcBm.left + phdi->cxTextAndBm;
		}
	}


	HRGN hrgnClip = NULL;

	//
	// 如果包含Image或Bitmap
	//
	if ((phdi->fmt & HDF_IMAGE) || (phdi->fmt & HDF_BITMAP))
	{
		BOOL fDrawBoth = FALSE;
		RECT rcDrawn;
		HRGN hrgn1 = NULL, hrgn2 = NULL;

		int temp;   // used to determine placement of bitmap.

		if ((phdi->fmt & HDF_IMAGE) && (phdi->fmt & HDF_BITMAP))
		{
			// we have to do both
			fDrawBoth = TRUE;

			// first do just the image... turn off the bitmap bit

			// HACK ALERT! -- Don't call _Header_DrawBitmap with
			//                both the bitmap and image flags on

			// Draw the image...
			item.fmt ^= HDF_BITMAP;    // turn off bitmap bit
		}

		if (!(uFlags & HDDF_NOIMAGE))
		{
			TraceMsg(TF_HEADER, "h_di: about to draw bitmap at rcBm= (%d,%d,%d,%d)",
				rcBm.left, rcBm.top, rcBm.right, rcBm.bottom);
			temp = _Header_DrawBitmap(hdc, phd->himl, &item, &rcBm,
					item.fmt & HDF_JUSTIFYMASK, uDrawTextFlags,
					&rcDrawn, phd->iBmMargin);
			hrgn1 = CreateRectRgnIndirect(&rcDrawn);
		}

		if (fDrawBoth)
		{
			// Tack on the bitmap...
			// Always tack the bitmap on the right of the image and
			// text unless we are right justified.  then, tack it on
			// left.

			item.fmt ^= HDF_BITMAP;    // turn on bitmap bit
			item.fmt ^= HDF_IMAGE;     // and turn off image bit
			if (item.fmt & HDF_RIGHT)
			{
				rcBm.right = temp;

				if (item.fmt & HDF_STRING)
				{
					rcBm.right = ((rcBm.left < rcText.left) ?
							rcBm.left : rcText.left);
				}
				rcBm.left = rcHeader.left;
			}
			else
			{
				rcBm.left = temp;

				if (item.fmt & HDF_STRING)
				{
					rcBm.left = ((rcBm.right > rcText.right) ? rcBm.right : rcText.right);
				}
				rcBm.right = rcHeader.right;
			}

			if (!(uFlags & HDDF_NOIMAGE))
			{
				_Header_DrawBitmap(hdc, phd->himl, &item, &rcBm,
					item.fmt & HDF_RIGHT, uDrawTextFlags,
					&rcDrawn, phd->iBmMargin);
				hrgn2 = CreateRectRgnIndirect(&rcDrawn);
			}

			item.fmt ^= HDF_IMAGE;     // turn on the image bit

		}

		// if there were any regions created, union them together
		if (hrgn1 && hrgn2)
		{
			hrgnClip = CreateRectRgn(0, 0, 0, 0);
			CombineRgn(hrgnClip, hrgn1, hrgn2, RGN_OR);
			DeleteObject(hrgn1);
			DeleteObject(hrgn2);
		}
		else if (hrgn1)
		{
			hrgnClip = hrgn1;
			hrgn1 = NULL;
		}
		else if (hrgn2)
		{
			hrgnClip = hrgn2;
			hrgn2 = NULL;
		}

		// this only happens in the drag/drop case
		if ((uFlags & HDDF_NOIMAGE) && !hrgnClip)
		{
			// this means we didn't draw the images, which means we
			// don't have the rects for them,
			// which means we need to create a dummy empty hrgnClip;
			hrgnClip = CreateRectRgn(0, 0, 0, 0);
		}

		SaveDC(hdc);
	}

	//
	// 绘制Header Item文本
	//
	if (phdi->fmt & HDF_STRING)
	{

#ifdef WINDOWS_ME
		if (item.fmt & HDF_RTLREADING)
		{
			uDrawTextFlags |= SHDT_RTLREADING;
		}
#endif

		TraceMsg(TF_HEADER, "h_di: about to draw text rcText=(%d,%d,%d,%d)",
			rcText.left, rcText.top, rcText.right, rcText.bottom);
		SHDrawText(hdc, item.pszText, &rcText,
			item.fmt & HDF_JUSTIFYMASK,
			uDrawTextFlags, phd->cyChar, phd->cxEllipses,
			clrText, clrBk);
		if (hrgnClip != NULL)
		{
			// if we're building a clipping region, add the text to it.
			HRGN hrgnText;

			hrgnText = CreateRectRgnIndirect(&rcText);
			CombineRgn(hrgnClip, hrgnText, hrgnClip, RGN_OR);
			DeleteObject(hrgnText);
		}
	}

	//
	// 如果包含Filter Bar，则绘制Filter Bar
	//
	if (phd->hwnd->ci.style & HDS_FILTERBAR)
	{
		TCHAR szBuffer[32] = {'\0'};
		LPTSTR pBuffer = szBuffer;
		DWORD dwButtonState = DFCS_BUTTONPUSH;

		uDrawTextFlags = SHDT_ELLIPSES | SHDT_EXTRAMARGIN | SHDT_CLIPPED;

#ifdef WINDOWS_ME
		if (item.fmt & HDF_RTLREADING)
		{
			uDrawTextFlags |= SHDT_RTLREADING;
		}
#endif

		if (phdi->type & HDFT_HASNOVALUE)
		{
			//BUGBUG: should be cached
			//L LocalizedLoadString(IDS_ENTERTEXTHERE, szBuffer, ARRAYSIZE(szBuffer));
			clrText = GetSysColor(COLOR_GRAYTEXT);
		}
		else
		{
			clrText = IUIGetGloabelVar()->g_clrWindowText;
			switch (phdi->type & HDFT_ISMASK)
			{
			case HDFT_ISSTRING:
				pBuffer = phdi->textFilter.pszText;
				break;

			case HDFT_ISNUMBER:
				StringCchPrintf(szBuffer, 32, TEXT("%d"), phdi->intFilter);
				break;

			default:
				_ASSERT(FALSE);
				break;
			}
		}

		SHDrawText(hdc, pBuffer, &rcFilter,
			0, uDrawTextFlags,
			phd->cyChar, phd->cxEllipses,
			clrText, IUIGetGloabelVar()->g_clrWindow);

		PatBlt(hdc, rcFilter.left, rcFilter.bottom, rcFilter.right - rcFilter.left, c_cyFilterBarEdge, BLACKNESS);
		Header_DrawFilterGlyph(phd, hdc, &rcButton, (i == phd->iButtonDown));

		if (hrgnClip)
		{
			// if we're building a clipping region, add the text to it.
			HRGN hrgnFilter;

			hrgnFilter = CreateRectRgn(rcFilter.left, rcButton.top, rcButton.right, rcButton.bottom);
			CombineRgn(hrgnClip, hrgnFilter, hrgnClip, RGN_OR);
			DeleteObject(hrgnFilter);
		}

		if (phd->fFocus && (phd->iFocus == i)
#ifdef KEYBOARDCUES
			&& !(CCGetUIState(&(phd->hwnd->ci)) & UISF_HIDEFOCUS)
#endif
		)
		{
			InflateRect(&rcFilter, -IUIGetGloabelVar()->g_cxEdge / 2, -GetSystemMetrics(SM_CYEDGE) / 2);
			SetTextColor(hdc, IUIGetGloabelVar()->g_clrWindowText);
			DrawFocusRect(hdc, &rcFilter);
		}
	}

	if (hrgnClip != NULL)
	{
		HRGN hrgnAll;
		HRGN hrgn;
		// hrgnClip is the union of everyplace we've drawn..
		// we want just the opposite.. so xor it
		hrgnAll = CreateRectRgnIndirect(&rcHeader);
		hrgn = CreateRectRgn(0, 0, 0, 0);
		CombineRgn(hrgn, hrgnAll, hrgnClip, RGN_XOR);

		SelectClipRgn(hdc, hrgn);

		ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcHeader, NULL, 0, NULL);
		RestoreDC(hdc, -1);

		DeleteObject(hrgnClip);
		DeleteObject(hrgn);
		DeleteObject(hrgnAll);
	}

	//
	// 绘制Header Item边框，使其看上去像个按钮（仅在未绘制Item背景图的情况下）
	//
	if (!bDrawButtonImage)
	{
		if (!(uFlags & HDDF_NOEDGE))
		{
			Header_DrawButtonEdges(phd, hdc, &rcHeader, fItemSunken);
		}
	}

	if (dwRet & CDRF_NOTIFYPOSTPAINT)
	{
		CICustomDrawNotify(&phd->hwnd->ci, CDDS_ITEMPOSTPAINT, &nmcd);
	}
}

void Header_Draw(HD *phd, HDC hdc, RECT *prcClip)
{
	HDC hdcMem = NULL;
	int iIndex;
	COLORREF clrText;

	BOOL fTracking = Header_IsTracking(phd);

	HFONT hfontOld = NULL;
	if (phd->hfont != NULL)
	{
		hfontOld = SelectFont(hdc, phd->hfont);
	}

	//
	// 给父控件一个绘制Header的机会。
	//
	NMCUSTOMDRAW nmcd;
	nmcd.hdc = hdc;
	nmcd.uItemState = 0;
	nmcd.lItemlParam = 0;
	nmcd.rc = *prcClip;
	phd->hwnd->ci.dwCustom = CICustomDrawNotify(&phd->hwnd->ci, CDDS_PREPAINT, &nmcd);

	CIUIRect rcClient;
	GetClientRect(phd->hwnd, rcClient);

	//
	// 绘制Header背景
	//
	if (phd->hwnd->m_bDrawBkColor)
	{
		FillSolidRect(hdc, rcClient,
			IsWindowEnabled(phd->hwnd) ? phd->hwnd->ci.m_crBkN : phd->hwnd->ci.m_crBkD, FALSE);
	}

	if (phd->hwnd->ci.m_bCombineBkImage)
	{
		if (phd->hwnd->ci.m_himgCombineBk != NULL)
		{
			IUIPartDrawImage(hdc, rcClient,
				phd->hwnd->ci.m_himgCombineBk, phd->hwnd->ci.m_eBkImageResizeMode,
				phd->hwnd->ci.m_rcBkImage9GridResizeArg,
				COMBINEIMAGESIZE2, IsWindowEnabled(phd->hwnd) ? 0 : 1);
		}
	}
	else
	{
		IUIDrawImage(hdc, rcClient,
			IsWindowEnabled(phd->hwnd) ? phd->m_himgBk[0] : phd->m_himgBk[1],
			phd->hwnd->ci.m_eBkImageResizeMode, phd->hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	//
	// 绘制Header Item，把Header Item当成Button来绘制
	//
	// number of items in header
	int cItems = DSA_GetItemCount(phd->hdsaHDI);
	int i;                          // index of current header item
	CIUIRect rcItem;                            // item clipping rect
	for (i = 0 ; i < cItems; i++)
	{
		// 由Item顺序，得到Item索引（Item可能交换了顺序）
		iIndex = Header_ItemOrderToIndex(phd, i);
		Header_OnGetItemRect(phd, iIndex, &rcItem);

		if (prcClip != NULL)
		{
			if (rcItem.right < prcClip->left)
			{
				continue;
			}
			if (rcItem.left >= prcClip->right)
			{
				break;
			}
		}

		if (iIndex == phd->iHot)
		{
			clrText = GetSysColor(COLOR_HOTLIGHT);
		}
		else
		{
			clrText = IUIGetGloabelVar()->g_clrBtnText;
		}

		SetTextColor(hdc, clrText);

		Header_DrawItem(phd, hdc, i, iIndex, &rcItem, 0);
	}

	//
	// 绘制右侧空白区域(如果未指定Header Item背景图)
	//
	if (i == cItems)
	{
		BOOL bDrawButtonImage = FALSE;
		if (phd->m_bCombineButtonBkImage)
		{
			if (phd->m_himgCombineButtonBk != NULL)
			{
				bDrawButtonImage = TRUE;
			}
		}
		else
		{
			if (phd->m_himgButtonBk[0] != NULL)
			{
				bDrawButtonImage = TRUE;
			}
		}

		if (!bDrawButtonImage)
		{
			// we got through the loop... now we need to do the blank area on the right
			rcItem.left = rcItem.right;
			rcItem.right = 32000;
			Header_DrawButtonEdges(phd, hdc, &rcItem, FALSE);
		}
	}

	if (!HDDragFullWindows(phd)
		&& fTracking
		&& (phd->flagsTrack & (HHT_ONDIVIDER | HHT_ONDIVOPEN)))
	{
		Header_DrawDivider(phd, phd->xTrack);
	}

	// draw the hot divider
	if (phd->iNewOrder != -1)
	{
		RECT rc;
		COLORREF clrHot = GetSysColor(COLOR_HOTLIGHT);

		Header_GetDividerRect(phd, phd->iNewOrder, &rc);
		FillRectClr(hdc, &rc, clrHot);

	}

	if (phd->hwnd->ci.dwCustom & CDRF_NOTIFYPOSTPAINT)
	{
		CICustomDrawNotify(&phd->hwnd->ci, CDDS_POSTPAINT, &nmcd);
	}

	if (hfontOld)
	{
		SelectFont(hdc, hfontOld);
	}
}

HIMAGELIST Header_OnCreateDragImage(HD *phd, int i)
{
	HDC hdcMem = NULL;
	RECT rc;
	HBITMAP hbmImage = NULL;
	HBITMAP hbmMask = NULL;
	HFONT hfontOld = NULL;
	HIMAGELIST himl = NULL;
	HIMAGELIST himlDither = NULL;
	HBITMAP hbmOld = NULL;
	BOOL bMirroredWnd = (phd->hwnd->ci.dwExStyle & RTL_MIRRORED_WINDOW);
	int iIndex = Header_ItemOrderToIndex(phd, i);

	// IEUNIX : Fixing crash in OE while dragging the message
	// header.
	if (!Header_OnGetItemRect(phd, iIndex, &rc))
	{
		goto Bail;
	}

	// draw the header into this bitmap
	OffsetRect(&rc, -rc.left, -rc.top);

	if (!(hdcMem = CreateCompatibleDC(NULL)))
	{
		goto Bail;
	}

	if (!(hbmImage = CreateColorBitmap(rc.right, rc.bottom)))
	{
		goto Bail;
	}
	if (!(hbmMask = CreateMonoBitmap(rc.right, rc.bottom)))
	{
		goto Bail;
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

	if (phd->hfont)
	{
		hfontOld = SelectFont(hdcMem, phd->hfont);
	}

	if (!(himl = ImageList_Create(rc.right, rc.bottom, ILC_MASK, 1, 0)))
	{
		goto Bail;
	}

	if (!(himlDither = ImageList_Create(rc.right, rc.bottom, ILC_MASK, 1, 0)))
	{
		goto Bail;
	}


	// have the darker background
	SetTextColor(hdcMem, IUIGetGloabelVar()->g_clrBtnText);
	SetBkColor(hdcMem, GetSysColor(COLOR_BTNSHADOW));
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmImage);
	Header_DrawItem(phd, hdcMem, i, iIndex, &rc, HDDF_NOEDGE);

	//
	// If the header is RTL mirrored, then
	// mirror the Memory DC, so that when copying back
	// we don't get any image-flipping. [samera]
	//
	//L if (bMirroredWnd)
	//{
	//	MirrorBitmapInDC(hdcMem, hbmImage);
	//}

	// fill the mask with all black
	SelectObject(hdcMem, hbmMask);
	PatBlt(hdcMem, 0, 0, rc.right, rc.bottom, BLACKNESS);

	// put the image into an imagelist
	SelectObject(hdcMem, hbmOld);
	ImageList_SetBkColor(himl, CLR_NONE);
	ImageList_Add(himl, hbmImage, hbmMask);


	// have the darker background
	// now put the text in undithered.
	SetTextColor(hdcMem, IUIGetGloabelVar()->g_clrBtnText);
	SetBkColor(hdcMem, GetSysColor(COLOR_BTNSHADOW));
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmImage);
	Header_DrawItem(phd, hdcMem, i, iIndex, &rc, HDDF_NOIMAGE | HDDF_NOEDGE);
	DrawEdge(hdcMem, &rc, EDGE_BUMP, BF_RECT | BF_FLAT);

	//
	// If the header is RTL mirrored, then
	// mirror the Memory DC, so that when copying back
	// we don't get any image-flipping. [samera]
	//
	//L if (bMirroredWnd)
	//{
	//	MirrorBitmapInDC(hdcMem, hbmImage);
	//}

	/*
	// initialize this to transparent
	SelectObject(hdcMem, hbmImage);
	PatBlt(hdcMem, 0, 0, rc.right, rc.bottom, BLACKNESS);
	SelectObject(hdcMem, hbmMask);
	PatBlt(hdcMem, 0, 0, rc.right, rc.bottom, WHITENESS);
	*/

	SelectObject(hdcMem, hbmOld);
	ImageList_AddMasked(himlDither, hbmImage, GetSysColor(COLOR_BTNSHADOW));

	// dither image into himlDithered
	//L ImageList_CopyDitherImage(himlDither, 0, 0, 0, himl, 0, 0);

Bail:

	if (himl)
	{
		ImageList_Destroy(himl);
	}

	if (hdcMem)
	{
		if (hbmOld)
		{
			SelectObject(hdcMem, hbmOld);
		}
		if (hfontOld)
		{
			SelectFont(hdcMem, hfontOld);
		}
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


	return himlDither;
}

void Header_GetFilterRects(LPRECT prcItem, LPRECT prcHeader, LPRECT prcFilter, LPRECT prcButton)
{
	INT cyFilter = ((prcItem->bottom - prcItem->top) - c_cyFilterBarEdge) / 2;
	*prcButton = *prcFilter = *prcHeader = *prcItem;
	prcHeader->bottom = prcHeader->top + cyFilter;
	prcButton->left = prcFilter->right = prcFilter->right -= (GetSystemMetrics(SM_CXBORDER) * 4) + c_cxFilterImage;
	prcButton->top = prcFilter->top = prcHeader->bottom;
	prcFilter->bottom = prcFilter->top + cyFilter;
}

//
// Subclass the edit control to ensure we get the keys we are interested in
//

LRESULT WINAPI Header_EditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWLWND hHeader = (HWLWND)GetProp(hwnd, IUI_HEAD_EDIT_PARENT);

	HD *phd = (HD *)GetMember(hHeader);
	_ASSERT(phd);

	switch (msg)
	{
	case WM_KILLFOCUS:
		Header_StopFilterEdit(phd, FALSE);
		return 0L;

	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN)
		{
			Header_StopFilterEdit(phd, FALSE);
			return 0L;
		}
		else if (wParam == VK_ESCAPE)
		{
			Header_StopFilterEdit(phd, TRUE);
			return 0L;
		}
		else if (wParam == VK_F4)
		{
			Header_OnFilterButton(phd, phd->iEdit);
			return 0L;
		}
		break;
	}

	case WM_CHAR:
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_TAB:
			return 0L;                              // eat these so we don't beep
		}
#ifdef KEYBOARDCUES
		//notify of navigation key usage
		CCNotifyNavigationKeyUsage(&(phd->hwnd->ci), UISF_HIDEFOCUS);
#endif
		break;
	}

	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS | DLGC_HASSETSEL;        /* editing name, no dialog handling right now */

#ifdef KEYBOARDCUES
	case WM_UPDATEUISTATE:
	{
		DWORD dwUIStateMask = MAKEWPARAM(0xFFFF, UISF_HIDEFOCUS);

		if (CCOnUIState(&(phd->hwnd->ci), WM_UPDATEUISTATE, wParam & dwUIStateMask, lParam))
		{
			InvalidateRect(hwnd, NULL, TRUE);
		}

		break;
	}
#endif
	}

	return CallWindowProc(phd->pfnEditWndProc, hwnd, msg, wParam, lParam);
}

//
// Begin to edit the given column, displaying the editor as required
//

BOOL Header_BeginFilterEdit(HD *phd, int i)
{
	RECT rc, rcHeader, rcFilter, rcButton;
	int iIndex = i;
	int cxEdit, cyEdit;
	TCHAR szBuffer[MAX_PATH];
	LPTSTR pBuffer = szBuffer;
	int cchBuffer = MAX_PATH;
	UINT uFlags = WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL;
	HDI *phdi = Header_GetItemPtr(phd, i);

	if (!phdi || (i < 0))
	{
		return FALSE;    // yikes
	}

	// lets create an edit control that allows the user to
	// modify the current filter, note that we first must
	// format the data to be displayed in the control

	Header_OnGetItemRect(phd, iIndex, &rc);
	Header_GetFilterRects(&rc, &rcHeader, &rcFilter, &rcButton);

	phd->typeOld = phdi->type;          // keep the type field safe

	switch (phdi->type & HDFT_ISMASK)
	{
	case HDFT_ISSTRING:
		Str_Set(&phd->pszFilterOld, phdi->textFilter.pszText);
		pBuffer = phdi->textFilter.pszText;
		// This count does not include the terminating null
		cchBuffer = phdi->textFilter.cchTextMax;
		break;

	case HDFT_ISNUMBER:
		phd->intFilterOld = phdi->intFilter;
		StringCchPrintf(szBuffer, MAX_PATH, _T("%d"), phdi->intFilter);
		cchBuffer = 11;                                  // 10 digits, plus sign
		uFlags |= ES_NUMBER;
		break;

	default:
		return FALSE;
	}

	cxEdit = (rcFilter.right - rcFilter.left) - ((GetSystemMetrics(SM_CXBORDER) * 2) * 6);
	cyEdit = (rcFilter.bottom - rcFilter.top) - (GetSystemMetrics(SM_CYEDGE) * 2);
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(GetHostWnd(phd->hwnd), GWL_HINSTANCE);
	phd->hwndEdit = CreateWindow(TEXT("EDIT"),
			!(phdi->type & HDFT_HASNOVALUE) ? pBuffer : TEXT(""),
			uFlags,
			rcFilter.left + ((GetSystemMetrics(SM_CXBORDER) * 2) * 3),
			rcFilter.top + GetSystemMetrics(SM_CYEDGE),
			cxEdit, cyEdit,
			GetHostWnd(phd->hwnd),
			NULL, hInst, NULL);
	if (phd->hwndEdit)
	{
		INT iOldFocus = phd->iFocus;

		//
		// Setup the edit mode for this object?
		//

		phd->iEdit = i;                                 // now editing this column
		phd->iFocus = Header_OnGetItemOrder(phd, i);

		Header_OnGetItemRect(phd,  Header_ItemOrderToIndex(phd, iOldFocus), &rc);                     // nb: iOldFocus
		Header_GetFilterRects(&rc, &rcHeader, &rcFilter, &rcButton);
		RedrawWindow(GetHostWnd(phd->hwnd), &rcFilter, NULL, RDW_INVALIDATE | RDW_ERASE);

		//
		// Now subclass the edit control so we can trap the keystrokes we are interested in
		//

		SetProp(phd->hwndEdit, IUI_HEAD_EDIT_PARENT, phd->hwnd);
		phd->pfnEditWndProc = SubclassWindow(phd->hwndEdit, Header_EditWndProc);
		_ASSERT(phd->pfnEditWndProc);

		Edit_LimitText(phd->hwndEdit, cchBuffer);
		Edit_SetSel(phd->hwndEdit, 0, -1);
		FORWARD_WM_SETFONT(phd->hwndEdit, phd->hfont, FALSE, SendMessage);

		SetFocus(phd->hwndEdit);
	}

	return (phd->hwndEdit != NULL);
}

//
// Stop editing the fitler, discarding the change if we need to, otherwise
// the item has the correct information stored within it.
//

VOID Header_StopFilterEdit(HD *phd, BOOL fDiscardChanges)
{
	if (phd->iEdit >= 0)
	{
		HDI *phdi = Header_GetItemPtr(phd, phd->iEdit);
		HD_ITEM hdi;
		HD_TEXTFILTER textFilter;
		int intFilter;
		_ASSERT(phdi);

		if (fDiscardChanges)
		{
			hdi.mask = HDI_FILTER;
			hdi.type = phd->typeOld;

			switch (phdi->type & HDFT_ISMASK)
			{
			case HDFT_ISSTRING:
				textFilter.pszText = phd->pszFilterOld;
				textFilter.cchTextMax = phdi->textFilter.cchTextMax;
				hdi.pvFilter = &textFilter;
				break;

			case HDFT_ISNUMBER:
				intFilter = phd->intFilterOld;
				hdi.pvFilter = &intFilter;
				break;
			}

			Header_OnSetItem(phd, phd->iEdit, &hdi);
		}
		else
		{
			Header_FilterChanged(phd, FALSE);          // ensure we flush the changes
		}

		if (phd->hwndEdit)
		{
			SubclassWindow(phd->hwndEdit, phd->pfnEditWndProc);
			DestroyWindow(phd->hwndEdit);
			phd->hwndEdit = NULL;
		}

		phd->iEdit = -1;
		phd->pszFilterOld = NULL;
	}
}

//
// Send a filter change to the parent, either now or wait until the timeout
// expires.
//

VOID Header_FilterChanged(HD *phd, BOOL fWait)
{
	if (phd->iEdit < 0)
	{
		return;
	}

	if (fWait)
	{
		// defering the notify, therefore lets set the timer (killing any
		// previous ones) and marking that we are waiting on it.

		KillTimer(GetHostWnd(phd->hwnd), HD_EDITCHANGETIMER);
		SetTimer(GetHostWnd(phd->hwnd), HD_EDITCHANGETIMER, phd->iFilterChangeTimeout, NULL);
		phd->fFilterChangePending = TRUE;
	}
	else
	{
		HDI *phdi = Header_GetItemPtrByOrder(phd, phd->iEdit);
		_ASSERT(phdi);

		// if we have a change notify pending then lets send it to
		// the parent window, otherwise we just swallow it.

		if (phd->fFilterChangePending)
		{
			TCHAR szBuffer[MAX_PATH];
			HD_ITEM hdi;
			HD_TEXTFILTER textFilter;
			int intFilter;

			KillTimer(GetHostWnd(phd->hwnd), HD_EDITCHANGETIMER);
			phd->fFilterChangePending = FALSE;

			hdi.mask = HDI_FILTER;
			hdi.type = phdi->type & ~HDFT_HASNOVALUE;

			if (!GetWindowText(phd->hwndEdit, szBuffer, ARRAYSIZE(szBuffer)))
			{
				hdi.type |= HDFT_HASNOVALUE;
			}

			switch (phdi->type & HDFT_ISMASK)
			{
			case HDFT_ISSTRING:
				textFilter.pszText = szBuffer;
				textFilter.cchTextMax = phdi->textFilter.cchTextMax;
				hdi.pvFilter = &textFilter;
				break;

			case HDFT_ISNUMBER:
				intFilter = StrToInt(szBuffer);
				hdi.pvFilter = &intFilter;
				break;
			}

			Header_OnSetItem(phd, phd->iEdit, &hdi);
		}
	}
}

//
// Handle the user displaying the filter menu
//

VOID Header_OnFilterButton(HD *phd, INT i)
{
	NMHDFILTERBTNCLICK fbc;
	RECT rc, rcHeader, rcFilter;

	// filter button being depressed so depress it, then tell the user
	// that it went down so they can display the UI they want, before
	// we pop the button.  if the notify returns TRUE then send
	// a change notify around.

	Header_StopFilterEdit(phd, FALSE);

	_ASSERT(phd->iButtonDown == -1);
	phd->iButtonDown = i;

	Header_InvalidateItem(phd, i, RDW_INVALIDATE);
	UpdateWindow(GetHostWnd(phd->hwnd));

	ZeroMemory(&fbc, sizeof(fbc));
	fbc.iItem = i;
	// fbc.rc = { 0, 0, 0, 0 };

	Header_OnGetItemRect(phd, i, &rc);
	Header_GetFilterRects(&rc, &rcHeader, &rcFilter, &fbc.rc);

	if (CCSendNotify(&phd->hwnd->ci, HDN_FILTERBTNCLICK, &fbc.hdr))
	{
		Header_Notify(phd, i, 0, HDN_FILTERCHANGE);
	}

	phd->iButtonDown = -1;
	Header_InvalidateItem(phd, i, RDW_INVALIDATE);
	UpdateWindow(GetHostWnd(phd->hwnd));
}

//
// Handle clearing the filter for the given item
//

LRESULT Header_OnClearFilter(HD *phd, INT i)
{
	HDI *phdi;
	HD_ITEM hdi;
	INT iChanged = 0;

	Header_StopFilterEdit(phd, FALSE);

	if (i == -1)
	{
		//
		// clear all filters by setting setting the HDFT_HASNOVALUEFLAG on all items
		// remember to release the filter data.  For each item we also send an item
		// changing indicating that the filter is changing and then a item changed
		// to indicat that we really did fix the value.
		//

		for (i = 0 ; i < DSA_GetItemCount(phd->hdsaHDI); i++)
		{
			phdi = Header_GetItemPtrByOrder(phd, i);
			_ASSERT(phdi);

			if (!(phdi->type & HDFT_HASNOVALUE))
			{
				hdi.mask = HDI_FILTER;
				hdi.type = phdi->type | HDFT_HASNOVALUE;
				hdi.pvFilter = NULL;

				if (Header_SendChange(phd, i, HDN_ITEMCHANGING, &hdi))
				{
					if ((phdi->type & HDFT_ISMASK) == HDFT_ISSTRING)
					{
						Str_Set(&phdi->textFilter.pszText, NULL);
					}

					phdi->type |= HDFT_HASNOVALUE;                      // item is now empty

					Header_SendChange(phd, i, HDN_ITEMCHANGED, &hdi);

					iChanged++;
				}
			}
		}

		if (iChanged)
		{
			//
			// item == -1 indicating that we are cleared all filters, then invalidate
			// the window so that the filter values are no longer visible
			//

			Header_Notify(phd, -1, 0, HDN_FILTERCHANGE);	       // send out a notify of change
			RedrawWindow(GetHostWnd(phd->hwnd), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
	}
	else
	{
		if ((i < 0) || (i > DSA_GetItemCount(phd->hdsaHDI)))
		{
			return 0L;
		}

		phdi = Header_GetItemPtrByOrder(phd, i);
		_ASSERT(phdi);

		if (!(phdi->type & HDFT_HASNOVALUE))
		{
			//
			// clear a single filter by setting the HDFT_HASNOVALUE flag
			//

			hdi.mask = HDI_FILTER;
			hdi.type = phdi->type | HDFT_HASNOVALUE;
			hdi.pvFilter = NULL;

			Header_OnSetItem(phd, i, &hdi);
		}
	}

	return 1L;
}

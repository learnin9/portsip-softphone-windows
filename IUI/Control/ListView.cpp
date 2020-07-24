#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


ListView::ListView()
{

}

ListView::~ListView()
{

}

int ListView::BindStyle(const CControlProp *pCtrlProp)
{
	return ListView_BindStyle(m_hWnd, (const CListViewProp *)pCtrlProp);
}

#ifdef _DEBUG
#define _VERIFY _ASSERT
#else
#define _VERIFY(f)          ((void)(f))
#endif

HWND ListView::GetToolTips() const
{
	_ASSERT(IsWindow());
	return (HWND)SendMessage(LVM_GETTOOLTIPS, 0, 0L);
}
HWND ListView::SetToolTips(HWND pTip)
{
	_ASSERT(IsWindow());
	return (HWND)SendMessage(LVM_SETTOOLTIPS, 0, (LPARAM) pTip);
}
COLORREF ListView::GetBkColor() const
{
	_ASSERT(IsWindow());
	return (COLORREF) SendMessage(LVM_GETBKCOLOR, 0, 0L);
}
BOOL ListView::SetBkColor(COLORREF cr)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETBKCOLOR, 0, cr);
}
HIMAGELIST ListView::GetImageList(int nImageList) const
{
	_ASSERT(IsWindow());
	return (HIMAGELIST) SendMessage(LVM_GETIMAGELIST, nImageList, 0L);
}
HIMAGELIST ListView::SetImageList(HIMAGELIST hImageList, int nImageList)
{
	_ASSERT(IsWindow());
	return (HIMAGELIST) SendMessage(LVM_SETIMAGELIST, nImageList, (LPARAM)hImageList);
}
int ListView::GetItemCount() const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETITEMCOUNT, 0, 0L);
}
BOOL ListView::GetItem(LVITEMEX *pItem) const
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_GETITEM, 0, (LPARAM)pItem);
}
BOOL ListView::SetItem(const LVITEMEX *pItem)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETITEM, 0, (LPARAM)pItem);
}
BOOL ListView::SetItemData(int nItem, DWORD_PTR dwData)
{
	_ASSERT(IsWindow());
	return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData, 0, NULL);
}
int ListView::InsertItem(const LVITEMEX *pItem)
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_INSERTITEM, 0, (LPARAM)pItem);
}
int ListView::InsertItem(int nItem, LPCTSTR lpszItem)
{
	_ASSERT(IsWindow());
	return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0);
}
int ListView::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	_ASSERT(IsWindow());
	return InsertItem(LVIF_TEXT | LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0);
}
BOOL ListView::DeleteItem(int nItem)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_DELETEITEM, nItem, 0L);
}
BOOL ListView::DeleteAllItems()
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_DELETEALLITEMS, 0, 0L);
}
UINT ListView::GetCallbackMask() const
{
	_ASSERT(IsWindow());
	return (UINT) SendMessage(LVM_GETCALLBACKMASK, 0, 0);
}
BOOL ListView::SetCallbackMask(UINT nMask)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETCALLBACKMASK, nMask, 0);
}
int ListView::GetNextItem(int nItem, int nFlags) const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0));
}
IUIPOS ListView::GetFirstSelectedItemPosition() const
{
	_ASSERT(IsWindow());
	return (IUIPOS)(DWORD_PTR)(1 + GetNextItem(-1, LVIS_SELECTED));
}
int ListView::GetNextSelectedItem(IUIPOS &pos) const
{
	_ASSERT(IsWindow());
	DWORD_PTR nOldPos = (DWORD_PTR)pos - 1;
	pos = (IUIPOS)(DWORD_PTR)(1 + GetNextItem((UINT)nOldPos, LVIS_SELECTED));
	return (UINT)nOldPos;
}
int ListView::FindItem(LVFINDINFO *pFindInfo, int nStart) const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_FINDITEM, nStart, (LPARAM)pFindInfo);
}
int ListView::HitTest(LVHITTESTINFO *pHitTestInfo) const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_HITTEST, 0, (LPARAM)pHitTestInfo);
}
BOOL ListView::SetItemPosition(int nItem, POINT pt)  // LVM_SETITEMPOSITION is not supported, only LVM_SETITEMPOSITION32 is.
{
	_ASSERT(IsWindow());
	_ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
	return (BOOL) SendMessage(LVM_SETITEMPOSITION32, nItem, (LPARAM)&pt);
}
BOOL ListView::GetItemPosition(int nItem, LPPOINT lpPoint) const
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint);
}
int ListView::GetStringWidth(LPCTSTR lpsz) const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETSTRINGWIDTH, 0, (LPARAM)lpsz);
}
BOOL ListView::EnsureVisible(int nItem, BOOL bPartialOK)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0));
}
BOOL ListView::Scroll(CIUISize size)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SCROLL, size.cx, size.cy);
}
BOOL ListView::RedrawItems(int nFirst, int nLast)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_REDRAWITEMS, nFirst, nLast);
}
BOOL ListView::Arrange(UINT nCode)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_ARRANGE, nCode, 0L);
}
HWND ListView::EditLabel(int nItem)
{
	_ASSERT(IsWindow());
	return (HWND)SendMessage(LVM_EDITLABEL, nItem, 0L);
}
HWND ListView::GetEditControl() const
{
	_ASSERT(IsWindow());
	return (HWND)SendMessage(LVM_GETEDITCONTROL, 0, 0L);
}
BOOL ListView::GetColumn(int nCol, LVCOLUMN *pColumn) const
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_GETCOLUMN, nCol, (LPARAM)pColumn);
}
BOOL ListView::SetColumn(int nCol, const LVCOLUMN *pColumn)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETCOLUMN, nCol, (LPARAM)pColumn);
}
int ListView::InsertColumn(int nCol, const LVCOLUMN *pColumn)
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn);
}
BOOL ListView::DeleteColumn(int nCol)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_DELETECOLUMN, nCol, 0);
}
int ListView::GetColumnWidth(int nCol) const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETCOLUMNWIDTH, nCol, 0);
}
BOOL ListView::SetColumnWidth(int nCol, int cx)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0));
}
BOOL ListView::GetViewRect(LPRECT lpRect) const
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_GETVIEWRECT, 0, (LPARAM)lpRect);
}
int ListView::GetTopIndex() const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETTOPINDEX, 0, 0);
}
int ListView::GetCountPerPage() const
{
	_ASSERT(IsWindow());
	return (int) SendMessage(LVM_GETCOUNTPERPAGE, 0, 0);
}
BOOL ListView::GetOrigin(LPPOINT lpPoint) const
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_GETORIGIN, 0, (LPARAM)lpPoint);
}
BOOL ListView::Update(int nItem)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_UPDATE, nItem, 0L);
}
BOOL ListView::SetItemState(int nItem, LVITEMEX *pItem)
{
	_ASSERT(IsWindow());
	return (BOOL) SendMessage(LVM_SETITEMSTATE, nItem, (LPARAM)pItem);
}
UINT ListView::GetItemState(int nItem, UINT nMask) const
{
	_ASSERT(IsWindow());
	return (UINT) SendMessage(LVM_GETITEMSTATE, nItem, nMask);
}
void ListView::SetItemCount(int nItems)
{
	_ASSERT(IsWindow());
	SendMessage(LVM_SETITEMCOUNT, nItems, 0);
}
BOOL ListView::SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
{
	_ASSERT(IsWindow());
	_ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
	return (BOOL) SendMessage(LVM_SORTITEMS, dwData, (LPARAM)pfnCompare);
}
BOOL ListView::SortItemsEx(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
{
	_ASSERT(IsWindow());
	_ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
	return (BOOL) SendMessage(LVM_SORTITEMSEX, dwData, (LPARAM)pfnCompare);
}
UINT ListView::GetSelectedCount() const
{
	_ASSERT(IsWindow());
	return (UINT) SendMessage(LVM_GETSELECTEDCOUNT, 0, 0L);
}
BOOL ListView::GetItemSpacing(BOOL fSmall, int *pnHorzSpacing, int *pnVertSpacing) const
{
	_ASSERT(IsWindow());
	_ASSERT(pnHorzSpacing != NULL);
	_ASSERT(pnVertSpacing != NULL);
	if (pnHorzSpacing == NULL || pnVertSpacing == NULL)
	{
		return FALSE;
	}
	DWORD dwSpacing = (DWORD)SendMessage(LVM_GETITEMSPACING, (WPARAM)fSmall, 0L);
	*pnHorzSpacing = (int)LOWORD(dwSpacing);
	*pnVertSpacing = (int)HIWORD(dwSpacing);
	return TRUE;
}

///////////////////////////////////////////////////////////

BOOL ListView::Create(
	CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_LISTVIEW, NULL,
			dwStyle, rect, hParent, pParent, lpszName);
}

BOOL ListView::CreateEx(
	CWLWndHandler *pUserHandler,
	DWORD dwExStyle,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::CreateEx(pUserHandler, dwExStyle, IUI_LISTVIEW, NULL,
			dwStyle, rect, hParent, pParent, lpszName);
}

BOOL ListView::GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
{
	_ASSERT(IsWindow());
	lpRect->left = nCode;
	return (BOOL) SendMessage(LVM_GETITEMRECT, (WPARAM)nItem,
			(LPARAM)lpRect);
}

BOOL ListView::SetItemCountEx(int iCount, DWORD dwFlags /* = LVSICF_NOINVALIDATEALL */)
{
	_ASSERT(IsWindow());

	// can't have dwFlags on a control that isn't virutal
	_ASSERT(dwFlags == 0 || (GetStyle() & LVS_OWNERDATA));

	return (BOOL) SendMessage(LVM_SETITEMCOUNT, (WPARAM) iCount,
			(LPARAM) dwFlags);
}

CIUISize ListView::SetIconSpacing(int cx, int cy)
{
	_ASSERT(IsWindow());
	DWORD dwRet = (DWORD) SendMessage(LVM_SETICONSPACING,
			0, (LPARAM) MAKELONG(cx, cy));

	return CIUISize(dwRet);
}

CIUISize ListView::SetIconSpacing(CIUISize size)
{
	_ASSERT(IsWindow());
	DWORD dwRet = (DWORD) SendMessage(LVM_SETICONSPACING,
			0, (LPARAM) MAKELONG(size.cx, size.cy));

	return CIUISize(dwRet);
}

ColumnHeader *ListView::GetHeaderCtrl() const
{
	_ASSERT(IsWindow());
	HWLWND hHeader = (HWLWND)SendMessage(LVM_GETHEADER, 0, 0);
	if (hHeader == NULL)
	{
		return NULL;
	}

	// 代替CWnd::FromHander。 这里生成的临时CWLWnd *，为了简单不使用map，而是直接保存到HWLWND中，
	return (ColumnHeader *)CWLWnd::FromHandle(hHeader);
}

BOOL ListView::GetSubItemRect(int iItem, int iSubItem, int nArea, CIUIRect &ref) const
{
	_ASSERT(IsWindow());
	_ASSERT(nArea == LVIR_BOUNDS || nArea == LVIR_ICON || nArea == LVIR_LABEL || nArea == LVIR_SELECTBOUNDS);

	RECT rect;
	rect.top = iSubItem;
	rect.left = nArea;
	BOOL bRet = (BOOL) SendMessage(LVM_GETSUBITEMRECT,
			iItem, (LPARAM) &rect);

	if (bRet)
	{
		ref = rect;
	}
	return bRet;
}

int ListView::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
	int nWidth, int nSubItem)
{
	LVCOLUMN column;
	column.mask = LVCF_TEXT | LVCF_FMT;
	column.pszText = (LPTSTR)lpszColumnHeading;
	column.fmt = nFormat;
	if (nWidth != -1)
	{
		column.mask |= LVCF_WIDTH;
		column.cx = nWidth;
	}
	if (nSubItem != -1)
	{
		column.mask |= LVCF_SUBITEM;
		column.iSubItem = nSubItem;
	}
	return ListView::InsertColumn(nCol, &column);
}

int ListView::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask,
	int nImage, LPARAM lParam)
{
	_ASSERT(IsWindow());
	LVITEMEX item;
	item.mask = nMask;
	item.iItem = nItem;
	item.iSubItem = 0;
	item.pszText = (LPTSTR)lpszItem;
	item.state = nState;
	item.stateMask = nStateMask;
	item.iImage = nImage;
	item.lParam = lParam;
	return ListView::InsertItem(&item);
}

int ListView::HitTest(CIUIPoint pt, UINT *pFlags) const
{
	_ASSERT(IsWindow());
	LVHITTESTINFO hti;
	hti.pt = pt;
	int nRes = (int) SendMessage(LVM_HITTEST, 0, (LPARAM)&hti);
	if (pFlags != NULL)
	{
		*pFlags = hti.flags;
	}
	return nRes;
}

BOOL ListView::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
	int nImage, UINT nState, UINT nStateMask, LPARAM lParam, int nIndent, HWLWND hBindWL)
{
	_ASSERT(IsWindow());
	_ASSERT((GetStyle() & LVS_OWNERDATA) == 0);

	LVITEMEX lvi;
	lvi.mask = nMask;
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.stateMask = nStateMask;
	lvi.state = nState;
	lvi.pszText = (LPTSTR) lpszItem;
	lvi.iImage = nImage;
	lvi.lParam = lParam;
	lvi.iIndent = nIndent;
	lvi.hBindWindowless = hBindWL;

	return (BOOL) SendMessage(LVM_SETITEM, 0, (LPARAM)&lvi);
}

BOOL ListView::SetItemState(int nItem, UINT nState, UINT nStateMask)
{
	_ASSERT(IsWindow());
	LVITEMEX lvi;
	lvi.stateMask = nStateMask;
	lvi.state = nState;
	return (BOOL) SendMessage(LVM_SETITEMSTATE, nItem, (LPARAM)&lvi);
}

BOOL ListView::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	_ASSERT(IsWindow());
	_ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
	LVITEMEX lvi;
	lvi.iSubItem = nSubItem;
	lvi.pszText = (LPTSTR) lpszText;
	return (BOOL) SendMessage(LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
}

CIUIString ListView::GetItemText(int nItem, int nSubItem) const
{
	_ASSERT(IsWindow());
	LVITEMEX lvi;
	memset(&lvi, 0, sizeof(LVITEMEX));
	lvi.iSubItem = nSubItem;
	CIUIString str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		lvi.cchTextMax = nLen;
		lvi.pszText = str.GetBufferSetLength(nLen);
		nRes  = (int)SendMessage(LVM_GETITEMTEXT, (WPARAM)nItem,
				(LPARAM)&lvi);
	}
	while (nRes >= nLen - 1);
	str.ReleaseBuffer();
	return str;
}

int ListView::GetItemText(__in int nItem, __in int nSubItem, LPTSTR lpszText, __in int nLen) const
{
	_ASSERT(IsWindow());
	LVITEMEX lvi;
	memset(&lvi, 0, sizeof(LVITEMEX));
	lvi.iSubItem = nSubItem;
	lvi.cchTextMax = nLen;
	lvi.pszText = lpszText;
	return (int)SendMessage(LVM_GETITEMTEXT, (WPARAM)nItem,
			(LPARAM)&lvi);
}

DWORD_PTR ListView::GetItemData(int nItem) const
{
	_ASSERT(IsWindow());
	LVITEMEX lvi;
	memset(&lvi, 0, sizeof(LVITEMEX));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	_VERIFY(SendMessage(LVM_GETITEM, 0, (LPARAM)&lvi));
	return lvi.lParam;
}

HIMAGELIST ListView::CreateDragImage(int nItem, LPPOINT lpPoint)
{
	_ASSERT(IsWindow());

	HIMAGELIST hImageList = (HIMAGELIST)SendMessage(LVM_CREATEDRAGIMAGE,
			nItem, (LPARAM)lpPoint);
	if (hImageList == NULL)
	{
		return NULL;
	}

	return hImageList;
}

////////////////////////////////////////////////////////////////////////////
// for ListBox

int ListView::FindString(LPCTSTR lpszText)
{
	return -1;
}

int ListView::FindStringExact(LPCTSTR lpszText)
{
	return -1;
}

int ListView::SetCurSel(int nSel)
{
	return -1;
}

int ListView::GetCurSel() const
{
	return -1;
}

int ListView::SetCaretIndex(int nItem)
{
	return -1;
}

int ListView::SetTopIndex(int nItem)
{
	return -1;
}

CIUIString ListView::GetText(int nItem) const
{
	return _T("");
}

int ListView::CaretOn()
{
	return -1;
}

BOOL ListView::SetItemBindWindowless(__in int nItem, __in int nSubItem, __in HWLWND hBind)
{
	_ASSERT(IsWindow());
	return SetItem(nItem, nSubItem, LVIF_BINDWL, NULL, 0, 0, 0, NULL, 0, hBind);
}

HWLWND ListView::GetItemBindWindowless(int nItem, int nSubItem) const
{
	_ASSERT(IsWindow());
	LVITEMEX lvi;
	memset(&lvi, 0, sizeof(LVITEMEX));
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.mask = LVIF_BINDWL;
	_VERIFY(SendMessage(LVM_GETITEM, 0, (LPARAM)&lvi));
	return lvi.hBindWindowless;
}

DWORD ListView::SetExtendedStyle(DWORD dwNewStyle)
{
	_ASSERT(IsWindow());
	return (DWORD) IUI::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM) dwNewStyle);
}
HCURSOR ListView::SetHotCursor(HCURSOR hc)
{
	_ASSERT(IsWindow() && hc != NULL);
	return (HCURSOR) IUI::SendMessage(m_hWnd, LVM_SETHOTCURSOR, 0, (LPARAM) hc);
}
int ListView::SetHotItem(int iIndex)
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, LVM_SETHOTITEM, (WPARAM) iIndex, 0);
}
void ListView::SetWorkAreas(int nWorkAreas, LPRECT lpRect)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, LVM_SETWORKAREAS, nWorkAreas, (LPARAM) lpRect);
}
int ListView::SubItemHitTest(LPLVHITTESTINFO pInfo)
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM) pInfo);
}
HCURSOR ListView::GetHotCursor() const
{
	_ASSERT(IsWindow());
	return (HCURSOR) IUI::SendMessage(m_hWnd, LVM_GETHOTCURSOR, 0, 0);
}
int ListView::GetHotItem() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, LVM_GETHOTITEM, 0, 0);
}
DWORD ListView::GetExtendedStyle() const
{
	_ASSERT(IsWindow());
	return (DWORD) IUI::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
}
CIUISize ListView::ApproximateViewRect(CIUISize sz, int iCount) const
{
	_ASSERT(IsWindow());
	return CIUISize((DWORD) IUI::SendMessage(m_hWnd, LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(sz.cx, sz.cy)));
}
BOOL ListView::GetBkImage(LVBKIMAGE *plvbkImage) const
{
	_ASSERT(IsWindow());
	return (BOOL) IUI::SendMessage(m_hWnd, LVM_GETBKIMAGE, 0, (LPARAM)plvbkImage);
}
DWORD ListView::GetHoverTime() const
{
	_ASSERT(IsWindow());
	return (DWORD) IUI::SendMessage(m_hWnd, LVM_GETHOVERTIME, 0, 0);
}
void ListView::GetWorkAreas(int nWorkAreas, LPRECT prc) const
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, LVM_GETWORKAREAS, nWorkAreas, (LPARAM)prc);
}
BOOL ListView::SetBkImage(LVBKIMAGE *plvbkImage)
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)plvbkImage);
}
DWORD ListView::SetHoverTime(DWORD dwHoverTime)
{
	_ASSERT(IsWindow());
	return (DWORD)IUI::SendMessage(m_hWnd, LVM_SETHOVERTIME, 0, dwHoverTime);
}
UINT ListView::GetNumberOfWorkAreas() const
{
	_ASSERT(IsWindow());
	UINT nWorkAreas;
	IUI::SendMessage(m_hWnd, LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)&nWorkAreas);
	return nWorkAreas;
}
int ListView::SetSelectionMark(int iIndex)
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, LVM_SETSELECTIONMARK, 0, (LPARAM) iIndex);
}
int ListView::GetSelectionMark() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, LVM_GETSELECTIONMARK, 0, 0);
}

// IUI Add

int ListView::SetHeaderHeight(int nHeight)
{
	return ColumnHeader_SetHeaderHeight(GetHeaderCtrl()->GetSafeHwnd(), nHeight);
}

int ListView::GetHeaderHeight() const
{
	ColumnHeader *pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
	{
		return -1;
	}

	CIUIRect rc;
	pHeader->GetWindowRectToParent(rc);

	return rc.Height();
}

int ListView::SetHeaderBkImage(LPCTSTR lpszImageNameHeaderBk)
{
	ColumnHeader *pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
	{
		return -1;
	}

	return pHeader->SetBkImage(CONTROL_STATE_NORMAL, lpszImageNameHeaderBk, NULL);
}

int ListView::GetHeaderBkImage(CIUIString *pstrImageNameHeaderBk) const
{
	ColumnHeader *pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
	{
		return -1;
	}

	return pHeader->GetBkImage(CONTROL_STATE_NORMAL, pstrImageNameHeaderBk, NULL);
}

int ListView::SetHeaderButtonImage(
	UINT uMask,
	LPCTSTR lpszImageNameHeaderN,
	LPCTSTR lpszImageNameHeaderH,
	LPCTSTR lpszImageNameHeaderS,
	LPCTSTR lpszImageNameHeaderD)
{
	ColumnHeader *pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
	{
		return -1;
	}

	return pHeader->SetButtonImage(uMask,
			lpszImageNameHeaderN, lpszImageNameHeaderH,
			lpszImageNameHeaderS, lpszImageNameHeaderD);
}

int ListView::GetHeaderButtonImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameHeaderN,
	CIUIString *pstrImageNameHeaderH,
	CIUIString *pstrImageNameHeaderS,
	CIUIString *pstrImageNameHeaderD) const
{
	ColumnHeader *pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
	{
		return -1;
	}

	return pHeader->GetButtonImage(uMask, pbCombineImage, pstrCombineImageName,
			pstrImageNameHeaderN, pstrImageNameHeaderH, pstrImageNameHeaderS, pstrImageNameHeaderD);
}

int ListView::EnableHeaderTrack(BOOL bEnable)
{
	//TODO: pMember->m_bHeaderTrack = bEnable;

	return 0;
}

BOOL ListView::IsHeaderTrackEnable() const
{
	return FALSE; //TODO: pMember->m_bHeaderTrack;
}

// Set the images for background
int ListView::SetBkImage(UINT uMask, LPCTSTR lpszImageNameN, LPCTSTR lpszImageNameD)
{
	IUISetControlBkImage2(m_hWnd, uMask, lpszImageNameN, lpszImageNameD);

	return 0;
}

// Get the images for background
int ListView::GetBkImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD) const
{
	_ASSERT(IsWindow());

	return IUIGetControlBkImage2(m_hWnd, uMask, pbCombineImage, pstrCombineImageName,
			pstrImageNameN, pstrImageNameD);
}

int ListView::ShowGridLine(BOOL bShow)
{
	DWORD dwStyle = GetExtendedStyle();
	SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES);

	return 0;
}

BOOL ListView::IsGridLineVisible()
{
	DWORD dwStyle = GetExtendedStyle();
	BOOL bShowGridLine = ((dwStyle & LVS_EX_GRIDLINES) == LVS_EX_GRIDLINES);
	return bShowGridLine;
}

int ListView::DrawFirstRow(BOOL bDraw0Row)
{
	return ListView_SetDrawFirstRowGridLine(m_hWnd, bDraw0Row);
}

BOOL ListView::IsDrawFirstRow() const
{
	return ListView_IsDrawFirstRowGridLine(m_hWnd);
}

int ListView::SetGridLineOffset(const LPPOINT lpPtOffset)
{
	if (lpPtOffset == NULL)
	{
		return -1;
	}

	return ListView_SetGridLineOffset(m_hWnd, *lpPtOffset);
}

int ListView::GetGridLineOffset(LPPOINT lpPtOffset) const
{
	if (lpPtOffset == NULL)
	{
		return -1;
	}

	*lpPtOffset = ListView_GetGridLineOffset(m_hWnd);

	return 0;
}

int ListView::SetGridLineColor(COLORREF crGridLine)
{
	return ListView_SetGridLineColor(m_hWnd, crGridLine);
}

COLORREF ListView::GetGridLineColor() const
{
	return ListView_GetGridLineColor(m_hWnd);
}

int ListView::EnableItemBkEffect(int nItem, BOOL bEnable)
{
	return SendMessage(WLM_ENABLE_SPEC_ITEM_EFFECT,	nItem, bEnable);
}

BOOL ListView::IsItemBkEffectEnabled(int nItem) const
{
	return SendMessage(WLM_IS_SPEC_ITEM_EFFECT_ENABLED, nItem, 0);
}

int ListView::SetItemBkType(int nItem, BACKGROUND_TYPE eBkType)
{
	return SendMessage(WLM_SET_SPEC_ITEM_BK_TYPE, nItem, eBkType);
}

int ListView::GetItemBkType(int nItem, BACKGROUND_TYPE *peBkType) const
{
	return (BACKGROUND_TYPE)SendMessage(WLM_GET_SPEC_ITEM_BK_TYPE, nItem, 0);
}

int ListView::SetItemBkColor(
	int nItem,
	UINT uMask,
	COLORREF crN,
	COLORREF crH,
	COLORREF crS,
	COLORREF crD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_NORMAL;
		ic.color = crN;
		SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_HOVER;
		ic.color = crH;
		SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_PRESSED;
		ic.color = crS;
		SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_DISABLED;
		ic.color = crD;
		SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	nItem, (LPARAM)&ic);
	}

	return 0;
}

int ListView::GetItemBkColor(
	int nItem,
	UINT uMask,
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrS,
	COLORREF *pcrD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pcrN == NULL)
		{
			return 0;
		}

		*pcrN = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, nItem, CONTROL_STATE_NORMAL);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pcrH == NULL)
		{
			return 0;
		}

		*pcrH = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, nItem, CONTROL_STATE_HOVER);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pcrS == NULL)
		{
			return 0;
		}

		*pcrS = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, nItem, CONTROL_STATE_PRESSED);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pcrD == NULL)
		{
			return 0;
		}

		*pcrD = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, nItem, CONTROL_STATE_DISABLED);
	}

	return 0;
}

int ListView::SetItemBkImage(
	int nItem,
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameS,
	LPCTSTR lpszImageNameD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		ITEM_IMAGE ii;
		ii.eControlState = CONTROL_STATE_NORMAL;
		ii.lpszImage = lpszImageNameN;
		SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, nItem, (LPARAM)&ii);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		ITEM_IMAGE ii;
		ii.eControlState = CONTROL_STATE_HOVER;
		ii.lpszImage = lpszImageNameH;
		SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, nItem, (LPARAM)&ii);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		ITEM_IMAGE ii;
		ii.eControlState = CONTROL_STATE_PRESSED;
		ii.lpszImage = lpszImageNameS;
		SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, nItem, (LPARAM)&ii);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		ITEM_IMAGE ii;
		ii.eControlState = CONTROL_STATE_DISABLED;
		ii.lpszImage = lpszImageNameD;
		SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, nItem, (LPARAM)&ii);
	}

	return 0;
}

int ListView::GetItemBkImage(
	int nItem,
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameS,
	CIUIString *pstrImageNameD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
	{
		if (pstrCombineImageName == NULL)
		{
			return -1;
		}

		*pstrCombineImageName = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
				nItem, CONTROL_STATE_COMBINE);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pstrImageNameN == NULL)
		{
			return -2;
		}

		*pstrImageNameN = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
				nItem, CONTROL_STATE_NORMAL);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pstrImageNameH == NULL)
		{
			return -3;
		}

		*pstrImageNameH = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
				nItem, CONTROL_STATE_HOVER);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pstrImageNameS == NULL)
		{
			return -4;
		}

		*pstrImageNameS = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
				nItem, CONTROL_STATE_PRESSED);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pstrImageNameD == NULL)
		{
			return -5;
		}

		*pstrImageNameD = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
				nItem, CONTROL_STATE_DISABLED);
	}

	return 0;
}

int ListView::SetItemBkImageResizeArg(int nItem, IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE, nItem, (LPARAM)eIrm);
	SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE_9GRID_ARG, nItem, (LPARAM)lprc9Grid);

	return 0;
}

int ListView::GetItemBkImageResizeArg(int nItem, IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE,
				nItem, 0);
	}
	SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE_9GRID_ARG, nItem, (LPARAM)lprc9Grid);

	return 0;
}

int ListView::SetItemTextColor(
	int nItem,
	UINT uMask,
	COLORREF crN,
	COLORREF crH,
	COLORREF crS,
	COLORREF crD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_NORMAL;
		ic.color = crN;
		SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_HOVER;
		ic.color = crH;
		SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_HOVER;
		ic.color = crS;
		SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, nItem, (LPARAM)&ic);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		ITEM_COLOR ic;
		ic.eControlState = CONTROL_STATE_DISABLED;
		ic.color = crD;
		SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, nItem, (LPARAM)&ic);
	}

	return 0;
}

int ListView::GetItemTextColor(
	int nItem,
	UINT uMask,
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrS,
	COLORREF *pcrD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pcrN == NULL)
		{
			return -1;
		}

		*pcrN = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, nItem, CONTROL_STATE_NORMAL);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pcrH == NULL)
		{
			return -1;
		}

		*pcrH = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, nItem, CONTROL_STATE_HOVER);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pcrS == NULL)
		{
			return -1;
		}

		*pcrS = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, nItem, CONTROL_STATE_PRESSED);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pcrD == NULL)
		{
			return -1;
		}

		*pcrD = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, nItem, CONTROL_STATE_DISABLED);
	}

	return 0;
}

// For selected item
int ListView::EnableSelectedFitClient(BOOL bEnable)
{
	return ListView_EnableSelectedFitClient(m_hWnd, bEnable);
}

BOOL ListView::IsEnableSelectedFitClient() const
{
	return ListView_IsEnableSelectedFitClient(m_hWnd);
}

int ListView::SetSelectedItemBkType(
	UINT uMask,
	BACKGROUND_TYPE eBkTypeN,
	BACKGROUND_TYPE eBkTypeD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), eBkTypeN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), eBkTypeD);
	}

	return 0;
}

int ListView::GetSelectedItemBkType(
	UINT uMask,
	BACKGROUND_TYPE *peBkTypeN,
	BACKGROUND_TYPE *peBkTypeD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (peBkTypeN == NULL)
		{
			return -1;
		}

		*peBkTypeN = (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (peBkTypeD == NULL)
		{
			return -2;
		}

		*peBkTypeD = (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ListView::SetSelectedItemBkColor(UINT uMask, COLORREF crN, COLORREF crD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_ITEM_BK_COLOR,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), crN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_ITEM_BK_COLOR,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), crD);
	}

	return 0;
}

int ListView::GetSelectedItemBkColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pcrN == NULL)
		{
			return -1;
		}

		*pcrN = SendMessage(WLM_GET_ITEM_BK_COLOR,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pcrD == NULL)
		{
			return -1;
		}

		*pcrD = SendMessage(WLM_GET_ITEM_BK_COLOR,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ListView::SetSelectedItemBkImage(
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_ITEM_BK_IMAGE,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_ITEM_BK_IMAGE,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), (LPARAM)lpszImageNameD);
	}

	return 0;
}

int ListView::GetSelectedItemBkImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
	{
		if (pstrCombineImageName == NULL)
		{
			return -1;
		}

		*pstrCombineImageName = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_COMBINE), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pstrImageNameN == NULL)
		{
			return -2;
		}

		*pstrImageNameN = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
	{
		if (pstrImageNameD == NULL)
		{
			return -3;
		}

		*pstrImageNameD = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ListView::SetSelectedItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
		MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
	SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::GetSelectedItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
	}

	SendMessage(WLM_GET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::SetSelectedItemTextColor(UINT uMask, COLORREF crN, COLORREF crD)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), crN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), crD);
	}

	return 0;
}

int ListView::GetSelectedItemTextColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrD) const
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pcrN == NULL)
		{
			return -1;
		}

		*pcrN = SendMessage(WLM_GET_ITEM_TEXT_COLOR,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pcrD == NULL)
		{
			return -1;
		}

		*pcrD = SendMessage(WLM_GET_ITEM_TEXT_COLOR,
				MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ListView::SetFocusItemBkType(BACKGROUND_TYPE eBkType)
{
	return SendMessage(WLM_SET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), eBkType);
}

BACKGROUND_TYPE ListView::GetFocusItemBkType() const
{
	return (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int ListView::SetFocusItemBkColor(COLORREF cr)
{
	return SendMessage(WLM_SET_ITEM_BK_COLOR,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), cr);
}

COLORREF ListView::GetFocusItemBkColor() const
{
	return SendMessage(WLM_GET_ITEM_BK_COLOR,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int ListView::SetFocusItemBkImage(LPCTSTR lpszImageNameBk)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE,
		MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameBk);

	return 0;
}

int ListView::GetFocusItemBkImage(CIUIString *pstrImageNameBk) const
{
	if (pstrImageNameBk == NULL)
	{
		return -1;
	}

	*pstrImageNameBk = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);

	return 0;
}

int ListView::SetFocusItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
		MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
	SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::GetFocusItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE,
				MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
	}
	SendMessage(WLM_GET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::SetFocusItemTextColor(COLORREF cr)
{
	return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), cr);
}

COLORREF ListView::GetFocusItemTextColor() const
{
	return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int ListView::SetHighlightItemBkType(BACKGROUND_TYPE eBkType)
{
	return SendMessage(WLM_SET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), eBkType);
}

BACKGROUND_TYPE ListView::GetHighlightItemBkType() const
{
	return (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int ListView::SetHighlightItemBkColor(COLORREF cr, BOOL bRedraw)
{
	SendMessage(WLM_SET_ITEM_BK_COLOR,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), cr);

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

COLORREF ListView::GetHighlightItemBkColor() const
{
	return SendMessage(WLM_GET_ITEM_BK_COLOR,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int ListView::SetHighlightItemBkImage(LPCTSTR lpszImageName)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)lpszImageName);

	return 0;
}

int ListView::GetHighlightItemBkImage(CIUIString *pstrImageName)
{
	if (pstrImageName == NULL)
	{
		return -1;
	}

	*pstrImageName = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);

	return 0;
}

int ListView::SetHighlightItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
	SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::GetHighlightItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE,
				MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
	}

	SendMessage(WLM_GET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ListView::SetHighlightItemTextColor(COLORREF cr)
{
	return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), cr);
}

COLORREF ListView::GetHighlightItemTextColor() const
{
	return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
			MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int ListView::GetHighlightItem() const
{
	return SendMessage(LVM_GET_HIGHLIGHT_ITEM, 0, 0);
}

int ListView::SetItemTextFont(
	LPCTSTR lpszFontIDN,
	LPCTSTR lpszFontIDH,
	LPCTSTR lpszFontIDS,
	LPCTSTR lpszFontIDND,
	LPCTSTR lpszFontIDSD)
{
	//LISTCTRLMEMBER *pMember = (LISTCTRLMEMBER *)m_pMember;

	//ASSERT(::IsWindow(m_hWnd));

	//if (lpszFontIDN != NULL)
	//{
	//	ReleaseIUIFontInternal(pMember->m_lvpItemN.m_hIUIFont);
	//	pMember->m_lvpItemN.m_hIUIFont = CreateIUIFontInternal(lpszFontIDN);
	//}

	//if (lpszFontIDH != NULL)
	//{
	//	ReleaseIUIFontInternal(pMember->m_lvpItemH.m_hIUIFont);
	//	pMember->m_lvpItemH.m_hIUIFont = CreateIUIFontInternal(lpszFontIDH);
	//}
	//if (lpszFontIDS != NULL)
	//{
	//	ReleaseIUIFontInternal(pMember->m_lvpItemS.m_hIUIFont);
	//	pMember->m_lvpItemS.m_hIUIFont = CreateIUIFontInternal(lpszFontIDS);
	//}

	//if (lpszFontIDND != NULL)
	//{
	//	ReleaseIUIFontInternal(pMember->m_lvpItemND.m_hIUIFont);
	//	pMember->m_lvpItemND.m_hIUIFont = CreateIUIFontInternal(lpszFontIDND);
	//}

	//if (lpszFontIDSD != NULL)
	//{
	//	ReleaseIUIFontInternal(pMember->m_lvpItemSD.m_hIUIFont);
	//	pMember->m_lvpItemSD.m_hIUIFont = CreateIUIFontInternal(lpszFontIDSD);
	//}

	//HFONT hFont = GetHFont(IsWindowEnabled() ? pMember->m_lvpItemN.m_hIUIFont : pMember->m_lvpItemND.m_hIUIFont);
	//CIUIString strDefaultFont;
	//if (hFont == NULL)
	//{
	//	strDefaultFont = CUIMgr::GetDefaultFont();
	//	hFont = CreateIUIFont(strDefaultFont);
	//	ReleaseIUIFont(strDefaultFont);
	//}

	//::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	//if (strDefaultFont.IsEmpty())
	//{
	//	ReleaseIUIFont(strDefaultFont);
	//}

	return 0;
}

int ListView::GetItemTextFont(
	CIUIString *pstrFontIDN,
	CIUIString *pstrFontIDH,
	CIUIString *pstrFontIDS,
	CIUIString *pstrFontIDND,
	CIUIString *pstrFontIDSD) const
{
	//LISTCTRLMEMBER *pMember = (LISTCTRLMEMBER *)m_pMember;

	//if (pstrFontIDN != NULL)
	//{
	//	GetFontResID(pMember->m_lvpItemN.m_hIUIFont, pstrFontIDN);
	//}
	//if (pstrFontIDH != NULL)
	//{
	//	GetFontResID(pMember->m_lvpItemH.m_hIUIFont, pstrFontIDH);
	//}
	//if (pstrFontIDS != NULL)
	//{
	//	GetFontResID(pMember->m_lvpItemS.m_hIUIFont, pstrFontIDS);
	//}
	//if (pstrFontIDND != NULL)
	//{
	//	GetFontResID(pMember->m_lvpItemND.m_hIUIFont, pstrFontIDND);
	//}
	//if (pstrFontIDSD != NULL)
	//{
	//	GetFontResID(pMember->m_lvpItemSD.m_hIUIFont, pstrFontIDSD);
	//}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// ListView API

int IUI::ListView_BindStyle(HWLWND hWnd, const CListViewProp *pLstProp)
{
	HWLWND_BindStyle(hWnd, pLstProp);

	ListView *pListView = (ListView *)CWLWnd::FromHandle(hWnd);

	CIUIProject *pProject = GetProject(hWnd);

	DWORD dwExStyle = pListView->GetExtendedStyle();
	if ((bool)pLstProp->m_bCheckBoxes)
	{
		dwExStyle |= LVS_EX_CHECKBOXES;
	}
	if ((bool)pLstProp->m_bFullRowSelect)
	{
		dwExStyle |= LVS_EX_FULLROWSELECT;
	}
	if ((bool)pLstProp->m_bHeaderDragDrop)
	{
		dwExStyle |= LVS_EX_HEADERDRAGDROP;
	}
	if ((bool)pLstProp->m_bInfoTooltip)
	{
		dwExStyle |= LVS_EX_INFOTIP;
	}
	if ((bool)pLstProp->m_bSubItemImages)
	{
		dwExStyle |= LVS_EX_SUBITEMIMAGES;
	}
	if ((bool)pLstProp->m_bEnableHighlightItemEffect)
	{
		dwExStyle |= LVS_EX_TRACKSELECT;
	}
	if ((bool)pLstProp->m_bTwoClickActivate)
	{
		dwExStyle |= LVS_EX_TWOCLICKACTIVATE;
	}

	LST_STYLE eListStyle = (LST_STYLE)pLstProp->m_eListStyle;
	if (eListStyle == LSTS_ICON)
	{
		//dwExStyle |= LVS_EX_JUSTIFYCOLUMNS;
	}

	pListView->SetExtendedStyle(dwExStyle);

	// Set background images
	{
		CIUIString strImageName[1 + COMBINEIMAGESIZE2];
		BOOL bCombineImage = TRUE;
		CControlProp::GetBackground2(pLstProp, &bCombineImage, strImageName);
		if (bCombineImage)
		{
			pListView->SetCombineBkImage(strImageName[0]);
		}
		else
		{
			pListView->SetBkImage(CONTROL_STATE_ND, strImageName[1], strImageName[2]);
		}

		pListView->SetBkImageResizeMode(pLstProp->m_eBkImageResizeMode);
		pListView->SetBkImage9GridResizeParam(&(RECT)pLstProp->m_rcBkImage9GridResizeArg);
	}

	// Set Item Font
	CIUIString strFont[5];
	int nIndex = 0;
	strFont[nIndex++] = (LPCTSTR)pLstProp->m_strResFontIDN;
	strFont[nIndex++] = (LPCTSTR)pLstProp->m_strResFontIDH;
	strFont[nIndex++] = (LPCTSTR)pLstProp->m_strResFontIDP;
	strFont[nIndex++] = (LPCTSTR)pLstProp->m_strResFontIDD;
	strFont[nIndex++] = (LPCTSTR)pLstProp->m_strResFontIDSD;

	pListView->SetItemTextFont(strFont[0], strFont[1], strFont[2], strFont[3], strFont[4]);

	// Set Header
	if (pListView->GetHeaderCtrl() != NULL)
	{
		// Header height
		long nHeight = pLstProp->m_nHeaderHeight;
		GetDPIHeight(nHeight);
		pListView->SetHeaderHeight(nHeight);
		ListView_SetItemHeaderSpace(hWnd, (LONG)pLstProp->m_lItemHeaderSpace);

		CControlProp *pHeaderStyle = pProject->GetStyleItem(STYLET_HEADERCTRL,
				(LPCTSTR)pLstProp->m_strHeaderStyleID);

		ColumnHeader *pHeader = pListView->GetHeaderCtrl();
		_ASSERT(pHeader != NULL);
		pHeader->BindStyle(pHeaderStyle);
	}

	pListView->EnableHeaderTrack((bool)pLstProp->m_bHeaderTrack);

	// Set item text color
	COLORREF cr[4] = {0};
	pProject->GetControlTextColor4(pLstProp, cr);
	pListView->SetHighlightItemTextColor(cr[1]);
	pListView->SetSelectedItemTextColor(CONTROL_STATE_ND, cr[2], cr[3]);
	COLORREF crSD = pProject->GetColor((LPCTSTR)pLstProp->m_strResColorIDSD);

	// Focus Item
	pListView->SetFocusItemBkType(pLstProp->m_eFocusItemBkType);
	if (BKT_COLOR == (BACKGROUND_TYPE)pLstProp->m_eFocusItemBkType)
	{
		COLORREF cr = pProject->GetColor(pLstProp->m_strFocusItemBkColorResID);
		pListView->SetFocusItemBkColor(cr);
	}
	else if (BKT_IMAGE == (BACKGROUND_TYPE)pLstProp->m_eFocusItemBkType)
	{
		pListView->SetFocusItemBkImage(pLstProp->m_strFocusItemBkImage);
		pListView->SetFocusItemBkImageResizeArg(IRM_9GRID, &(RECT)pLstProp->m_rcFocusItemBkImage9GridArg);
	}

	// Highlight Item
	pListView->SetHighlightItemBkType(pLstProp->m_eHighlightItemBkType);
	if (BKT_COLOR == (BACKGROUND_TYPE)pLstProp->m_eHighlightItemBkType)
	{
		COLORREF cr = pProject->GetColor(pLstProp->m_strHighlightItemBkColorResID);
		pListView->SetHighlightItemBkColor(cr, FALSE);
	}
	else if (BKT_IMAGE == (BACKGROUND_TYPE)pLstProp->m_eHighlightItemBkType)
	{
		pListView->SetHighlightItemBkImage(pLstProp->m_strHighlightItemBkImage);
		pListView->SetHighlightItemBkImageResizeArg(IRM_9GRID, &(RECT)pLstProp->m_rcHighlightItemBkImage9GridArg);
	}

	// Selected Item
	pListView->EnableSelectedFitClient((bool)pLstProp->m_bSelectedFitClient);
	pListView->SetSelectedItemBkType(CONTROL_STATE_ND,
		pLstProp->m_eSelectedItemBkType, pLstProp->m_eSelectedItemBkType);
	if (BKT_COLOR == (BACKGROUND_TYPE)pLstProp->m_eSelectedItemBkType)
	{
		COLORREF cr = pProject->GetColor(pLstProp->m_strSelectedItemBkColorResID);
		pListView->SetSelectedItemBkColor(CONTROL_STATE_ND, cr, cr);
	}
	else if (BKT_IMAGE == (BACKGROUND_TYPE)pLstProp->m_eSelectedItemBkType)
	{
		pListView->SetSelectedItemBkImage(CONTROL_STATE_ND,
			pLstProp->m_strSelectedItemBkImage, pLstProp->m_strSelectedItemBkImage);
		pListView->SetSelectedItemBkImageResizeArg(IRM_9GRID, &(RECT)pLstProp->m_rcSelectedItemBkImage9GridArg);
	}

	// Grid line
	if (pLstProp->m_bShowGridLine)
	{
		pListView->ShowGridLine(TRUE);
		pListView->DrawFirstRow((bool)pLstProp->m_bDraw0Row);

		POINT ptOffset;
		ptOffset.x = (LONG)pLstProp->m_lXOffset;
		ptOffset.y = 0;
		pListView->SetGridLineOffset(&ptOffset);

		COLORREF cr = pProject->GetColor((LPCTSTR)pLstProp->m_strGridLineColorRes);
		pListView->SetGridLineColor(cr);
	}

	// Icon view
	if (eListStyle == LSTS_ICON)
	{
		pListView->SetIconSpacing((LONG)pLstProp->m_lIconModeTotalItemWidth,
			(LONG)pLstProp->m_lIconModeTotalItemHeight);
	}

	// 绑定Scroll Bar。因为在ListView控件创建的时候，ScrollBar控件可能还未创建
	// 所以，ListView控件中，只记录ScrollBar绑定到哪个Style。 当ScrollBar创建时，
	// 使用记录的Style来初始化。
	// Horizontal scroll bar
	{
		CControlProp *pSbStyle = GetProject(hWnd)->GetStyleItem(STYLET_SCROLLBAR,
				(LPCTSTR)pLstProp->m_strHorScrollbarID);
		BindHorzScrollBarStyle(hWnd, (const CScrollBarProp *)pSbStyle);
	}

	// Vertical scroll bar
	CControlProp *pSbStyle = GetProject(hWnd)->GetStyleItem(STYLET_SCROLLBAR,
		(LPCTSTR)pLstProp->m_strVerScrollbarID);
	if (NULL != pSbStyle)
	{
		BindVertScrollBarStyle(hWnd, (const CScrollBarProp *)pSbStyle);

		// 界面编辑器里的滚动条宽度只针对绑定滚动条风格时，如果在界面编辑器中没有绑定风格，使用
		// Metrics::g_nScrollBarWidth作为默认宽度。
		// 否则Metrics::SetScrollBarWidth()设置的宽度永远无效。
		SetVScrollBarWidth(hWnd, (LONG)pLstProp->m_lScrollBarWidth);
		SetHScrollBarHeight(hWnd, (LONG)pLstProp->m_lScrollBarWidth);
	}

	ListView_SetItemHeight(hWnd, (LONG)pLstProp->m_lItemHeight);

	return 0;
}

int IUI::ListView_SetItemHeight(HWLWND hListView, int nHeight)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_SETITEMHEIGHT, nHeight, 0);
}

int IUI::ListView_GetItemHeight(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_GETITEMHEIGHT, 0, 0);
}

int IUI::ListView_SetItemHeaderSpace(HWLWND hListView, int nSpace)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_SETITEMHEADERSPACE, nSpace, 0);
}

int IUI::ListView_GetItemHeaderSpace(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_GETITEMHEADERSPACE, 0, 0);
}

int IUI::ListView_SetGridLineColor(HWLWND hListView, COLORREF cr)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_SETGRIDLINECOLOR, cr, 0);
}

COLORREF IUI::ListView_GetGridLineColor(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_GETGRIDLINECOLOR, 0, 0);
}

int IUI::ListView_SetDrawFirstRowGridLine(HWLWND hListView, BOOL bDraw)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_SETDRAWFIRSTROWLINE, bDraw, 0);
}

BOOL IUI::ListView_IsDrawFirstRowGridLine(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_ISDRAWFIRSTROWLINE, 0, 0);
}

int IUI::ListView_SetGridLineOffset(HWLWND hListView, POINT pt)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_SETGRIDLINEOFFSET, 0, (LPARAM)&pt);
}

POINT IUI::ListView_GetGridLineOffset(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	POINT pt;
	SendMessage(hListView, LVM_GETGRIDLINEOFFSET, 0, (LPARAM)&pt);
	return pt;
}

int IUI::ListView_EnableSelectedFitClient(HWLWND hListView, BOOL bEnable)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_ENABLE_SELECTED_FIT_CLIENT, 0, bEnable);
}

BOOL IUI::ListView_IsEnableSelectedFitClient(HWLWND hListView)
{
	_ASSERT(IsWindow(hListView));
	return SendMessage(hListView, LVM_IS_SELECTED_FIT_CLIENT_ENABLED, 0, 0);
}

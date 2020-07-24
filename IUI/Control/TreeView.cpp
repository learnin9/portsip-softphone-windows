#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


TreeView::TreeView()
{

}

TreeView::~TreeView()
{
    DestroyWindow();
}

BOOL TreeView::Create(
    __in CWLWndHandler *pUserHandler,
    __in DWORD dwStyle,
    __in const RECT &rect,
    __in HWND hParent,
    __in CWLWnd *pParent,
    __in LPCTSTR lpszName,
    __in BOOL bSmoothVScroll)
{
    CWLWnd *pWnd = this;
    return pWnd->Create(pUserHandler, IUI_TREEVIEW, NULL, dwStyle, rect, hParent, pParent, lpszName,
            (LPVOID)bSmoothVScroll);
}

BOOL TreeView::CreateEx(
    __in CWLWndHandler *pUserHandler,
    __in DWORD dwExStyle,
    __in DWORD dwStyle,
    __in const RECT &rect,
    __in HWND hParent,
    __in CWLWnd *pParent,
    __in LPCTSTR lpszName,
    __in BOOL bSmoothVScroll)
{
    BOOL bRet = Create(pUserHandler, dwStyle, rect, hParent, pParent, lpszName, bSmoothVScroll);

    if (bRet && dwExStyle != 0)
    {
        //bRet = ModifyStyleEx(0, dwExStyle);
    }

    return bRet;
}

int TreeView::BindStyle(const CControlProp *pCtrlProp)
{
    return TreeView_BindStyle(m_hWnd, (const CTreeViewProp *)pCtrlProp);
}

HTREEITEM TreeView::InsertItem(LPTVINSERTSTRUCT lpInsertStruct)
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct);
}
HTREEITEM TreeView::InsertItem(LPCTSTR lpszItem, int nImage,
    int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    _ASSERT(IsWindow());
    return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter);
}
HTREEITEM TreeView::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    _ASSERT(IsWindow());
    return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter);
}
BOOL TreeView::DeleteItem(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_DELETEITEM, 0, (LPARAM)hItem);
}
#pragma warning(disable: 4310)
BOOL TreeView::DeleteAllItems()
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
}
#pragma warning(default: 4310)
BOOL TreeView::Expand(HTREEITEM hItem, UINT nCode)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_EXPAND, nCode, (LPARAM)hItem);
}
UINT TreeView::GetCount() const
{
    _ASSERT(IsWindow());
    return (UINT)SendMessage(TVM_GETCOUNT, 0, 0);
}
UINT TreeView::GetIndent() const
{
    _ASSERT(IsWindow());
    return (UINT)SendMessage(TVM_GETINDENT, 0, 0);
}
void TreeView::SetIndent(UINT nIndent)
{
    _ASSERT(IsWindow());
    SendMessage(TVM_SETINDENT, nIndent, 0);
}
HIMAGELIST TreeView::GetImageList(int nImageList) const
{
    _ASSERT(IsWindow());
    return (HIMAGELIST)SendMessage(TVM_GETIMAGELIST, (WPARAM)nImageList, 0);
}
HIMAGELIST TreeView::SetImageList(HIMAGELIST pImageList, int nImageList)
{
    _ASSERT(IsWindow());
    return (HIMAGELIST)SendMessage(TVM_SETIMAGELIST, (WPARAM)nImageList, (LPARAM)pImageList);
}
UINT TreeView::SetScrollTime(UINT uScrollTime)
{
    _ASSERT(IsWindow());
    return (UINT) SendMessage(TVM_SETSCROLLTIME, (WPARAM) uScrollTime, 0);
}
HTREEITEM TreeView::GetNextItem(HTREEITEM hItem, UINT nCode) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, nCode, (LPARAM)hItem);
}
HTREEITEM TreeView::GetChildItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
}
HTREEITEM TreeView::GetNextSiblingItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
}
HTREEITEM TreeView::GetPrevSiblingItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem);
}
HTREEITEM TreeView::GetParentItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
}
HTREEITEM TreeView::GetFirstVisibleItem() const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
}
HTREEITEM TreeView::GetNextVisibleItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)hItem);
}
HTREEITEM TreeView::GetPrevVisibleItem(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)hItem);
}
HTREEITEM TreeView::GetLastVisibleItem() const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_LASTVISIBLE, 0);
}
HTREEITEM TreeView::GetSelectedItem() const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_CARET, 0);
}
UINT TreeView::GetScrollTime() const
{
    _ASSERT(IsWindow());
    return (UINT) SendMessage(TVM_GETSCROLLTIME, 0, 0);
}
HTREEITEM TreeView::GetDropHilightItem() const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
}
HTREEITEM TreeView::GetRootItem() const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_GETNEXTITEM, TVGN_ROOT, 0);
}
BOOL TreeView::Select(HTREEITEM hItem, UINT nCode)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_SELECTITEM, nCode, (LPARAM)hItem);
}
BOOL TreeView::SelectItem(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
}
BOOL TreeView::SelectDropTarget(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hItem);
}
BOOL TreeView::SelectSetFirstVisible(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hItem);
}
BOOL TreeView::GetItem(XTVITEM *pItem) const
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_GETITEM, 0, (LPARAM)pItem);
}
BOOL TreeView::SetItem(XTVITEM *pItem)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_SETITEM, 0, (LPARAM)pItem);
}
BOOL TreeView::SetItemText(HTREEITEM hItem, LPCTSTR lpszItem)
{
    _ASSERT(IsWindow());
    return SetItem(hItem, TVIF_TEXT, lpszItem, 0, 0, 0, 0, NULL, NULL);
}
BOOL TreeView::SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
{
    _ASSERT(IsWindow());
    return SetItem(hItem, TVIF_IMAGE | TVIF_SELECTEDIMAGE,
            NULL, nImage, nSelectedImage, 0, 0, NULL, NULL);
}
BOOL TreeView::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask)
{
    _ASSERT(IsWindow());
    return SetItem(hItem, TVIF_STATE, NULL, 0, 0, nState, nStateMask, NULL, NULL);
}
BOOL TreeView::SetItemData(HTREEITEM hItem, DWORD_PTR dwData)
{
    _ASSERT(IsWindow());
    return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData, NULL);
}
HWND TreeView::EditLabel(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (HWND)SendMessage(TVM_EDITLABEL, 0, (LPARAM)hItem);
}
HTREEITEM TreeView::HitTest(TVHITTESTINFO *pHitTestInfo) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM)SendMessage(TVM_HITTEST, 0, (LPARAM)pHitTestInfo);
}
HWND TreeView::GetEditControl() const
{
    _ASSERT(IsWindow());
    return (HWND)SendMessage(TVM_GETEDITCONTROL, 0, 0);
}
UINT TreeView::GetVisibleCount() const
{
    _ASSERT(IsWindow());
    return (UINT)SendMessage(TVM_GETVISIBLECOUNT, 0, 0);
}
BOOL TreeView::SortChildren(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_SORTCHILDREN, 0, (LPARAM)hItem);
}
BOOL TreeView::EnsureVisible(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_ENSUREVISIBLE, 0, (LPARAM)hItem);
}
BOOL TreeView::SortChildrenCB(LPTVSORTCB pSort)
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_SORTCHILDRENCB, 0, (LPARAM)pSort);
}
HWND TreeView::GetToolTips() const
{
    _ASSERT(IsWindow());
    return (HWND)SendMessage(TVM_GETTOOLTIPS, 0, 0L);
}
HWND TreeView::SetToolTips(HWND pWndTip)
{
    _ASSERT(IsWindow());
    return (HWND)SendMessage(TVM_SETTOOLTIPS, (WPARAM)pWndTip, 0L);
}
COLORREF TreeView::GetBkColor() const
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_GETBKCOLOR, 0, 0L);
}
COLORREF TreeView::SetBkColor(COLORREF clr)
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_SETBKCOLOR, 0, (LPARAM)clr);
}
SHORT TreeView::GetItemHeight() const
{
    _ASSERT(IsWindow());
    return (SHORT) SendMessage(TVM_GETITEMHEIGHT, 0, 0L);
}
SHORT TreeView::SetItemHeight(SHORT cyHeight)
{
    _ASSERT(IsWindow());
    return (SHORT) SendMessage(TVM_SETITEMHEIGHT, cyHeight, 0L);
}
SHORT TreeView::SetItemHeightEx(__in HTREEITEM hItem, __in SHORT cyHeight)
{
    _ASSERT(IsWindow());
    return (SHORT) SendMessage(TVM_SETITEMHEIGHTEX, cyHeight, (LPARAM)hItem);
}
COLORREF TreeView::GetTextColor() const
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_GETTEXTCOLOR, 0, 0L);
}
COLORREF TreeView::SetTextColor(COLORREF clr)
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_SETTEXTCOLOR, 0, (LPARAM)clr);
}
BOOL TreeView::SetInsertMark(HTREEITEM hItem, BOOL fAfter)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_SETINSERTMARK, fAfter, (LPARAM)hItem);
}
COLORREF TreeView::GetInsertMarkColor() const
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_GETINSERTMARKCOLOR, 0, 0L);
}
COLORREF TreeView::SetInsertMarkColor(COLORREF clrNew)
{
    _ASSERT(IsWindow());
    return (COLORREF) SendMessage(TVM_SETINSERTMARKCOLOR, 0, (LPARAM)clrNew);
}
BOOL TreeView::EndEditLabelNow(BOOL fCancelWithoutSave)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_ENDEDITLABELNOW, (WPARAM)fCancelWithoutSave, 0);
}

#if _WIN32_IE >= 0x0500
COLORREF TreeView::GetLineColor() const
{
    _ASSERT(IsWindow());
    return (COLORREF)SendMessage(TVM_GETLINECOLOR, 0, 0L);
}
COLORREF TreeView::SetLineColor(COLORREF clrNew /*= CLR_DEFAULT*/)
{
    _ASSERT(IsWindow());
    return (COLORREF)SendMessage(TVM_SETLINECOLOR, 0, (LPARAM)clrNew);
}
#endif

#if (_WIN32_WINNT >= 0x0501) && defined(UNICODE)
UINT TreeView::MapItemToAccId(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    return (UINT) SendMessage(TVM_MAPHTREEITEMTOACCID, (WPARAM)hItem, 0);
}
HTREEITEM TreeView::MapAccIdToItem(UINT uAccId) const
{
    _ASSERT(IsWindow());
    return (HTREEITEM) SendMessage(TVM_MAPACCIDTOHTREEITEM, (WPARAM)uAccId, 0);
}
#endif

#if (_WIN32_WINNT >= 0x0600) && defined(UNICODE)
UINT TreeView::GetSelectedCount()
{
    _ASSERT(IsWindow());
    return (UINT) SendMessage(TVM_GETSELECTEDCOUNT, 0, 0);
}
void TreeView::ShowInfoTip(HTREEITEM hItem)
{
    _ASSERT(IsWindow());
    SendMessage(TVM_SHOWINFOTIP, 0, (LPARAM)hItem);
}
BOOL TreeView::SetItemStateEx(HTREEITEM hItem, UINT uStateEx)
{
    _ASSERT(IsWindow());
    return (BOOL)SetItemEx(hItem, TVIF_STATEEX, 0, 0, 0, 0, 0, 0, uStateEx, 0, 0);
}
BOOL TreeView::SetItemExpandedImageIndex(HTREEITEM hItem, int nIndex)
{
    _ASSERT(IsWindow());
    return (BOOL)SetItemEx(hItem, TVIF_EXPANDEDIMAGE, 0, 0, 0, 0, 0, 0, TVIF_EXPANDEDIMAGE, 0, nIndex);
}
DWORD TreeView::GetExtendedStyle() const
{
    _ASSERT(IsWindow());
    return (DWORD) SendMessage(TVM_GETEXTENDEDSTYLE, (WPARAM)0, 0);
}
DWORD TreeView::SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles)
{
    _ASSERT(IsWindow());
    return (DWORD) SendMessage(TVM_SETEXTENDEDSTYLE, (WPARAM)dwExMask, (LPARAM)dwExStyles);
}
BOOL TreeView::SetAutoscrollInfo(UINT uPixelsPerSec, UINT uUpdateTime)
{
    _ASSERT(IsWindow());
    return (BOOL) SendMessage(TVM_SETAUTOSCROLLINFO, (WPARAM)uPixelsPerSec, (LPARAM)uUpdateTime);
}
#endif

BOOL TreeView::GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const
{
    _ASSERT(IsWindow());
    *(HTREEITEM *)lpRect = hItem;
    return (BOOL)SendMessage(TVM_GETITEMRECT, (WPARAM)bTextOnly,
            (LPARAM)lpRect);
}

CIUIString TreeView::GetItemText(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_TEXT;
    CIUIString str;
    int nLen = 128;
    int nRes;
    do
    {
        nLen *= 2;
        item.pszText = str.GetBufferSetLength(nLen);
        item.cchTextMax = nLen;
        SendMessage(TVM_GETITEM, 0, (LPARAM)&item);
        nRes = lstrlen(item.pszText);
    }
    while (nRes >= nLen - 1);
    str.ReleaseBuffer();
    return str;
}

BOOL TreeView::GetItemImage(HTREEITEM hItem, int &nImage, int &nSelectedImage) const
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    BOOL bRes = (BOOL)SendMessage(TVM_GETITEM, 0, (LPARAM)&item);
    if (bRes)
    {
        nImage = item.iImage;
        nSelectedImage = item.iSelectedImage;
    }
    return bRes;
}

UINT TreeView::GetItemState(HTREEITEM hItem, UINT nStateMask) const
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_STATE;
    item.stateMask = nStateMask;
    item.state = 0;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    return item.state;
}

DWORD_PTR TreeView::GetItemData(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    _ASSERT(hItem != NULL);
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_PARAM;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    return item.lParam;
}

BOOL TreeView::ItemHasChildren(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_CHILDREN;
    SendMessage(TVM_GETITEM, 0, (LPARAM)&item);
    return item.cChildren;
}

BOOL TreeView::SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
    int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HWLWND hBindWL)
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = nMask;
    item.pszText = (LPTSTR) lpszItem;
    item.iImage = nImage;
    item.iSelectedImage = nSelectedImage;
    item.state = nState;
    item.stateMask = nStateMask;
    item.lParam = lParam;
    item.hBindWindowless = hBindWL;
    return (BOOL)SendMessage(TVM_SETITEM, 0, (LPARAM)&item);
}

#if (_WIN32_IE >= 0x0600)
BOOL TreeView::SetItemEx(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
    int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
    UINT uStateEx, HWND hWnd, int iExpandedImage)
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = nMask;
    item.pszText = (LPTSTR) lpszItem;
    item.iImage = nImage;
    item.iSelectedImage = nSelectedImage;
    item.state = nState;
    item.stateMask = nStateMask;
    item.lParam = lParam;
    //item.uStateEx = uStateEx;
    //item.hwnd = hWnd;
    //item.iExpandedImage = iExpandedImage;
    return (BOOL)SendMessage(TVM_SETITEM, 0, (LPARAM)&item);
}
#endif

HTREEITEM TreeView::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
    int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
    HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    _ASSERT(IsWindow());
    TVINSERTSTRUCT tvis;
    tvis.hParent = hParent;
    tvis.hInsertAfter = hInsertAfter;
    tvis.item.mask = nMask;
    tvis.item.pszText = (LPTSTR) lpszItem;
    tvis.item.iImage = nImage;
    tvis.item.iSelectedImage = nSelectedImage;
    tvis.item.state = nState;
    tvis.item.stateMask = nStateMask;
    tvis.item.lParam = lParam;
    return (HTREEITEM)SendMessage(TVM_INSERTITEM, 0, (LPARAM)&tvis);
}

BOOL TreeView::MoveItemTo(__in HTREEITEM hItemBeMove,
	__in HTREEITEM hParent/* = TVI_ROOT*/, __in HTREEITEM hInsertAfter/* = TVI_LAST*/)
{
	_ASSERT(IsWindow());
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.hItem = hItemBeMove;
	return (BOOL)SendMessage(TVM_MOVEITEM, 0, (LPARAM)&tvis);
}

HTREEITEM TreeView::HitTest(CIUIPoint pt, UINT *pFlags) const
{
    _ASSERT(IsWindow());
    TVHITTESTINFO hti;
    hti.pt = pt;
    HTREEITEM h = (HTREEITEM)SendMessage(TVM_HITTEST, 0,
            (LPARAM)&hti);
    if (pFlags != NULL)
    {
        *pFlags = hti.flags;
    }
    return h;
}

void TreeView::RemoveImageList(int nImageList)
{
    HIMAGELIST h = (HIMAGELIST)SendMessage(TVM_GETIMAGELIST, (WPARAM)nImageList, 0);
    if (h != NULL)
    {
        SendMessage(TVM_SETIMAGELIST, (WPARAM)nImageList, NULL);
    }
}

void TreeView::OnDestroy()
{
    RemoveImageList(LVSIL_NORMAL);
    RemoveImageList(LVSIL_STATE);
}

HIMAGELIST TreeView::CreateDragImage(HTREEITEM hItem)
{
    _ASSERT(IsWindow());

    HIMAGELIST hImageList = (HIMAGELIST)SendMessage(TVM_CREATEDRAGIMAGE, 0, (LPARAM)hItem);
    if (hImageList == NULL)
    {
        return NULL;
    }

    return hImageList;
}

BOOL TreeView::GetCheck(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.mask = TVIF_HANDLE | TVIF_STATE;
    item.hItem = hItem;
    item.stateMask = TVIS_STATEIMAGEMASK;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    // Return zero if it's not checked, or nonzero otherwise.
    return ((BOOL)(item.state >> 12) - 1);
}

BOOL TreeView::SetCheck(HTREEITEM hItem, BOOL fCheck)
{
    _ASSERT(IsWindow());
    XTVITEM item;
    item.mask = TVIF_HANDLE | TVIF_STATE;
    item.hItem = hItem;
    item.stateMask = TVIS_STATEIMAGEMASK;

    // Since state images are one-based, passing a 1 to the
    // INDEXTOSTATEIMAGEMASK macro turns the check off, and
    // passing 2 turns it on.
    item.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));

    return (BOOL)SendMessage(TVM_SETITEM, 0, (LPARAM)&item);
}

#if (_WIN32_WINNT >= 0x0600) && defined(UNICODE)

BOOL TreeView::GetItemPartRect(HTREEITEM hItem, TVITEMPART nPart, LPRECT lpRect)
{
    _ASSERT(IsWindow());
    _ASSERT(lpRect);
    TVGETITEMPARTRECTINFO info;
    info.hti = hItem;
    info.prc = lpRect;
    info.partID = nPart;
    return (BOOL)::SendMessage((m_hWnd), TVM_GETITEMPARTRECT, 0, (LPARAM)&info);
}

UINT TreeView::GetItemStateEx(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    _ASSERT(hItem);

    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_STATEEX;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    return item.uStateEx;
}

int TreeView::GetItemExpandedImageIndex(HTREEITEM hItem) const
{
    _ASSERT(IsWindow());
    _ASSERT(hItem);

    XTVITEM item;
    item.hItem = hItem;

    item.mask = TVIF_EXPANDEDIMAGE;
    item.uStateEx = TVIF_EXPANDEDIMAGE;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    return item.iExpandedImage;
}
#endif

BOOL TreeView::IsSmoothVScroll() const
{
    _ASSERT(IsWindow());
    return (BOOL)SendMessage(TVM_GET_SMOOTH_VSCROLL, 0, 0);
}

BOOL TreeView::SetItemBindWindowless(HTREEITEM hItem, HWLWND hBind)
{
    _ASSERT(IsWindow());
    return SetItem(hItem, TVIF_BINDWL, NULL, 0, 0, 0, 0, NULL, hBind);
}

HWLWND TreeView::GetItemBindWindowless(HTREEITEM hItem) const
{
    if (NULL == hItem)
    {
        return NULL;
    }

    _ASSERT(IsWindow());
    XTVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_BINDWL;
    VERIFY(SendMessage(TVM_GETITEM, 0, (LPARAM)&item));
    return item.hBindWindowless;
}

// IUI Add interface
int TreeView::SetBkImage(
    UINT uMask,
    LPCTSTR lpszImageNameN,
    LPCTSTR lpszImageNameD,
    BOOL bRedraw)
{
    IUISetControlBkImage2(m_hWnd, uMask, lpszImageNameN, lpszImageNameD);

    if (bRedraw)
    {
        Invalidate();
    }

    return 0;
}

int TreeView::GetBkImage(
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

int TreeView::SetTextAlignHor(
    TEXT_ALIGN_HOR eHorAlignModeN,
    TEXT_ALIGN_HOR eHorAlignModeH,
    TEXT_ALIGN_HOR eHorAlignModeS,
    TEXT_ALIGN_HOR eHorAlignModeD,
    BOOL bRedraw)
{
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_NORMAL), eHorAlignModeN);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_HOVER), eHorAlignModeH);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_PRESSED), eHorAlignModeS);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_DISABLED), eHorAlignModeD);

    if (bRedraw)
    {
        Invalidate();
    }

    return 0;
}

int TreeView::GetTextAlignHor(
    TEXT_ALIGN_HOR *peHorAlignModeN,
    TEXT_ALIGN_HOR *peHorAlignModeH,
    TEXT_ALIGN_HOR *peHorAlignModeS,
    TEXT_ALIGN_HOR *peHorAlignModeD)
{
    if (peHorAlignModeN != NULL)
    {
        *peHorAlignModeN = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
    }
    if (peHorAlignModeH != NULL)
    {
        *peHorAlignModeH = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
    }
    if (peHorAlignModeS != NULL)
    {
        *peHorAlignModeS = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
    }
    if (peHorAlignModeD != NULL)
    {
        *peHorAlignModeD = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
    }

    return 0;
}

int TreeView::SetTextAlignVer(
    TEXT_ALIGN_VER eVerAlignModeN,
    TEXT_ALIGN_VER eVerAlignModeH,
    TEXT_ALIGN_VER eVerAlignModeS,
    TEXT_ALIGN_VER eVerAlignModeD,
    BOOL bRedraw/* = TRUE*/)
{
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_NORMAL), eVerAlignModeN);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_HOVER), eVerAlignModeH);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_PRESSED), eVerAlignModeS);
    SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_DISABLED), eVerAlignModeD);

    if (bRedraw)
    {
        Invalidate();
    }

    return 0;
}

int TreeView::GetTextAlignVer(
    TEXT_ALIGN_VER *peVerAlignModeN,
    TEXT_ALIGN_VER *peVerAlignModeH,
    TEXT_ALIGN_VER *peVerAlignModeS,
    TEXT_ALIGN_VER *peVerAlignModeD)
{
    if (peVerAlignModeN != NULL)
    {
        *peVerAlignModeN = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(1, CONTROL_STATE_NORMAL), 0);
    }
    if (peVerAlignModeH != NULL)
    {
        *peVerAlignModeH = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(1, CONTROL_STATE_HOVER), 0);
    }
    if (peVerAlignModeS != NULL)
    {
        *peVerAlignModeS = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(1, CONTROL_STATE_PRESSED), 0);
    }
    if (peVerAlignModeD != NULL)
    {
        *peVerAlignModeD = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
                MAKEWPARAM(1, CONTROL_STATE_DISABLED), 0);
    }

    return 0;
}

int TreeView::SetTextMargin(
    LPCRECT lprcTextMarginN,
    LPCRECT lprcTextMarginH,
    LPCRECT lprcTextMarginP,
    LPCRECT lprcTextMarginD,
    BOOL bRedraw/* = TRUE*/)
{
    SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
    SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lprcTextMarginH);
    SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lprcTextMarginP);
    SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

    if (bRedraw)
    {
        Invalidate();
    }

    return 0;
}

int TreeView::GetTextMargin(
    LPRECT lprcTextMarginN,
    LPRECT lprcTextMarginH,
    LPRECT lprcTextMarginP,
    LPRECT lprcTextMarginD) const
{
    SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
    SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lprcTextMarginH);
    SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lprcTextMarginP);
    SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

    return 0;
}

int TreeView::EnableItemLine(BOOL bEnable)
{
    return SendMessage(TVM_ENABLE_ITEM_LINE, bEnable, 0);
}

BOOL TreeView::IsEnableItemLine() const
{
    return SendMessage(TVM_IS_ENABLE_ITEM_LINE, 0, 0);
}

int TreeView::SetItemLineColor(COLORREF cr)
{
    return SendMessage(TVM_SET_ITEM_LINE_COLOR, 0, cr);
}

COLORREF TreeView::GetItemLineColor() const
{
    return SendMessage(TVM_GET_ITEM_LINE_COLOR, 0, 0);
}

int TreeView::SetCombineCollapseExpandedImage(LPCTSTR lpszImage)
{
    return SendMessage(TVM_SET_INDENT_IMAGE,
            MAKEWPARAM(0, CONTROL_STATE_COMBINE), (LPARAM)lpszImage);
}

int TreeView::SetCollapseImage(
    UINT uMask,
    LPCTSTR lpszImageNameCollapseN,
    LPCTSTR lpszImageNameCollapseH,
    LPCTSTR lpszImageNameCollapseP,
    LPCTSTR lpszImageNameCollapseD)
{
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(1, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameCollapseN);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(1, CONTROL_STATE_HOVER), (LPARAM)lpszImageNameCollapseH);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(1, CONTROL_STATE_PRESSED), (LPARAM)lpszImageNameCollapseP);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(1, CONTROL_STATE_DISABLED), (LPARAM)lpszImageNameCollapseD);

    return 0;
}

int TreeView::GetCollapseImage(
    UINT uMask,
    BOOL *pbCombineImage,
    CIUIString *pstrCombineImageName,
    CIUIString *pstrImageNameN,
    CIUIString *pstrImageNameH,
    CIUIString *pstrImageNameP,
    CIUIString *pstrImageNameD) const
{
    if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
    {
        if (pstrCombineImageName != NULL)
        {
            *pstrCombineImageName = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_COMBINE), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pstrImageNameN != NULL)
        {
            *pstrImageNameN = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(1, CONTROL_STATE_NORMAL), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        if (pstrImageNameH != NULL)
        {
            *pstrImageNameH = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(1, CONTROL_STATE_HOVER), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        if (pstrImageNameP != NULL)
        {
            *pstrImageNameP = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(1, CONTROL_STATE_PRESSED), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pstrImageNameD != NULL)
        {
            *pstrImageNameD = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(1, CONTROL_STATE_DISABLED), 0);
        }
    }

    return 0;
}

int TreeView::SetExpandedImage(
    UINT uMask,
    LPCTSTR lpszImageNameN,
    LPCTSTR lpszImageNameH,
    LPCTSTR lpszImageNameP,
    LPCTSTR lpszImageNameD)
{
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameN);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lpszImageNameH);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lpszImageNameP);
    SendMessage(TVM_SET_INDENT_IMAGE,
        MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lpszImageNameD);

    return 0;
}

int TreeView::GetExpandedImage(
    UINT uMask,
    BOOL *pbCombineImage,
    CIUIString *pstrCombineImageName,
    CIUIString *pstrImageNameN,
    CIUIString *pstrImageNameH,
    CIUIString *pstrImageNameP,
    CIUIString *pstrImageNameD) const
{
    if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
    {
        if (pstrCombineImageName != NULL)
        {
            *pstrCombineImageName = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_COMBINE), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pstrImageNameN != NULL)
        {
            *pstrImageNameN = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        if (pstrImageNameH != NULL)
        {
            *pstrImageNameH = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        if (pstrImageNameP != NULL)
        {
            *pstrImageNameP = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pstrImageNameD != NULL)
        {
            *pstrImageNameD = (LPCTSTR)SendMessage(TVM_GET_INDENT_IMAGE,
                    MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
        }
    }

    return 0;
}

int TreeView::EnableItemBkEffect(HTREEITEM hItem, BOOL bEnable)
{
    return SendMessage(WLM_ENABLE_SPEC_ITEM_EFFECT,	(WPARAM)hItem, bEnable);
}

BOOL TreeView::IsItemBkEffectEnabled(HTREEITEM hItem) const
{
    return SendMessage(WLM_IS_SPEC_ITEM_EFFECT_ENABLED, (WPARAM)hItem, 0);
}

int TreeView::SetItemBkType(HTREEITEM hItem, BACKGROUND_TYPE eBkType)
{
    return SendMessage(WLM_SET_SPEC_ITEM_BK_TYPE, (WPARAM)hItem, eBkType);
}

BACKGROUND_TYPE TreeView::GetItemBkType(HTREEITEM hItem) const
{
    return (BACKGROUND_TYPE)SendMessage(WLM_GET_SPEC_ITEM_BK_TYPE, (WPARAM)hItem, 0);
}

int TreeView::SetItemBkColor(
    HTREEITEM hItem,
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
        SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_HOVER;
        ic.color = crH;
        SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_PRESSED;
        ic.color = crS;
        SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_DISABLED;
        ic.color = crD;
        SendMessage(WLM_SET_SPEC_ITEM_BK_COLOR,	WPARAM(hItem), (LPARAM)&ic);
    }

    return 0;
}

int TreeView::GetItemBkColor(
    HTREEITEM hItem,
    UINT uMask,
    COLORREF *pcrN,
    COLORREF *pcrH,
    COLORREF *pcrS,
    COLORREF *pcrD) const
{
    if (hItem == NULL)
    {
        return 0;
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pcrN == NULL)
        {
            return 0;
        }

        *pcrN = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, (WPARAM)hItem, CONTROL_STATE_NORMAL);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        if (pcrH == NULL)
        {
            return 0;
        }

        *pcrH = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, (WPARAM)hItem, CONTROL_STATE_HOVER);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        if (pcrS == NULL)
        {
            return 0;
        }

        *pcrS = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, (WPARAM)hItem, CONTROL_STATE_PRESSED);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pcrD == NULL)
        {
            return 0;
        }

        *pcrD = SendMessage(WLM_GET_SPEC_ITEM_BK_COLOR, (WPARAM)hItem, CONTROL_STATE_DISABLED);
    }

    return 0;
}

int TreeView::SetItemBkImage(
    HTREEITEM hItem,
    UINT uMask,
    LPCTSTR lpszImageNameN,
    LPCTSTR lpszImageNameH,
    LPCTSTR lpszImageNameS,
    LPCTSTR lpszImageNameD)
{
    if (hItem == NULL)
    {
        return -1;
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        ITEM_IMAGE ii;
        ii.eControlState = CONTROL_STATE_NORMAL;
        ii.lpszImage = lpszImageNameN;
        SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, (WPARAM)hItem, (LPARAM)&ii);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        ITEM_IMAGE ii;
        ii.eControlState = CONTROL_STATE_HOVER;
        ii.lpszImage = lpszImageNameH;
        SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, (WPARAM)hItem, (LPARAM)&ii);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        ITEM_IMAGE ii;
        ii.eControlState = CONTROL_STATE_PRESSED;
        ii.lpszImage = lpszImageNameS;
        SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, (WPARAM)hItem, (LPARAM)&ii);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        ITEM_IMAGE ii;
        ii.eControlState = CONTROL_STATE_DISABLED;
        ii.lpszImage = lpszImageNameD;
        SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE, (WPARAM)hItem, (LPARAM)&ii);
    }

    return 0;
}

int TreeView::GetItemBkImage(
    HTREEITEM hItem,
    UINT uMask,
    BOOL *pbCombineImage,
    CIUIString *pstrCombineImageName,
    CIUIString *pstrImageNameN,
    CIUIString *pstrImageNameH,
    CIUIString *pstrImageNameS,
    CIUIString *pstrImageNameD)
{
    if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
    {
        if (pstrCombineImageName == NULL)
        {
            return -1;
        }

        *pstrCombineImageName = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
                WPARAM(hItem), CONTROL_STATE_COMBINE);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pstrImageNameN == NULL)
        {
            return -2;
        }

        *pstrImageNameN = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
                WPARAM(hItem), CONTROL_STATE_NORMAL);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        if (pstrImageNameH == NULL)
        {
            return -3;
        }

        *pstrImageNameH = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
                WPARAM(hItem), CONTROL_STATE_HOVER);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        if (pstrImageNameS == NULL)
        {
            return -4;
        }

        *pstrImageNameS = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
                WPARAM(hItem), CONTROL_STATE_PRESSED);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pstrImageNameD == NULL)
        {
            return -5;
        }

        *pstrImageNameD = (LPCTSTR)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE,
                WPARAM(hItem), CONTROL_STATE_DISABLED);
    }

    return 0;
}

int TreeView::SetItemBkImageResizeArg(
    HTREEITEM hItem,
    IMAGE_RESIZE_MODE eIrm,
    LPCRECT lprc9Grid)
{
    SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE, (WPARAM)hItem, (LPARAM)eIrm);
    SendMessage(WLM_SET_SPEC_ITEM_BK_IMAGE_9GRID_ARG, (WPARAM)hItem, (LPARAM)lprc9Grid);

    return 0;
}

int TreeView::GetItemBkImageResizeArg(
    HTREEITEM hItem,
    IMAGE_RESIZE_MODE *peIrm,
    LPRECT lprc9Grid)
{
    if (peIrm != NULL)
    {
        *peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE,
                (WPARAM)hItem, 0);
    }
    SendMessage(WLM_GET_SPEC_ITEM_BK_IMAGE_9GRID_ARG, (WPARAM)hItem, (LPARAM)lprc9Grid);

    return 0;
}

int TreeView::SetItemTextColor(
    HTREEITEM hItem,
    UINT uMask,
    COLORREF crN,
    COLORREF crH,
    COLORREF crS,
    COLORREF crD)
{
    if (hItem == NULL)
    {
        return -1;
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_NORMAL;
        ic.color = crN;
        SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_HOVER;
        ic.color = crH;
        SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_HOVER;
        ic.color = crS;
        SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, WPARAM(hItem), (LPARAM)&ic);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        ITEM_COLOR ic;
        ic.eControlState = CONTROL_STATE_DISABLED;
        ic.color = crD;
        SendMessage(WLM_SET_SPEC_ITEM_TEXT_COLOR, WPARAM(hItem), (LPARAM)&ic);
    }

    return 0;
}

int TreeView::GetItemTextColor(
    HTREEITEM hItem,
    UINT uMask,
    COLORREF *pcrN,
    COLORREF *pcrH,
    COLORREF *pcrS,
    COLORREF *pcrD) const
{
    if (hItem == NULL)
    {
        return -1;
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pcrN == NULL)
        {
            return -1;
        }

        *pcrN = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, (WPARAM)hItem, CONTROL_STATE_NORMAL);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
    {
        if (pcrH == NULL)
        {
            return -1;
        }

        *pcrH = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, (WPARAM)hItem, CONTROL_STATE_HOVER);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
    {
        if (pcrS == NULL)
        {
            return -1;
        }

        *pcrS = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, (WPARAM)hItem, CONTROL_STATE_PRESSED);
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pcrD == NULL)
        {
            return -1;
        }

        *pcrD = SendMessage(WLM_GET_SPEC_ITEM_TEXT_COLOR, (WPARAM)hItem, CONTROL_STATE_DISABLED);
    }

    return 0;
}

int TreeView::SetFocusItemBkType(BACKGROUND_TYPE eBkType)
{
    return SendMessage(WLM_SET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), eBkType);
}

BACKGROUND_TYPE TreeView::GetFocusItemBkType() const
{
    return (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetFocusItemBkColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetFocusItemBkColor() const
{
    return SendMessage(WLM_GET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetFocusItemBkImage(LPCTSTR lpszImageNameBk)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE,
        MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameBk);

    return 0;
}

int TreeView::GetFocusItemBkImage(CIUIString *pstrImageNameBk) const
{
    if (pstrImageNameBk == NULL)
    {
        return -1;
    }

    *pstrImageNameBk = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);

    return 0;
}

int TreeView::SetFocusItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
        MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
    SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
        MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

    return 0;
}

int TreeView::GetFocusItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
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

int TreeView::SetFocusItemTextColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetFocusItemTextColor() const
{
    return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_FOCUS, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetSelectedItemBkType(BACKGROUND_TYPE eBkType)
{
    return SendMessage(WLM_SET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), eBkType);
}

BACKGROUND_TYPE TreeView::GetSelectedItemBkType() const
{
    return (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetSelectedItemBkColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetSelectedItemBkColor() const
{
    return SendMessage(WLM_GET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetSelectedItemBkImage(
    UINT uMask,
    LPCTSTR lpszImageNameN,
    LPCTSTR lpszImageNameD)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE,
        MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameN);
    SendMessage(WLM_SET_ITEM_BK_IMAGE,
        MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), (LPARAM)lpszImageNameD);

    return 0;
}

int TreeView::GetSelectedItemBkImage(
    UINT uMask,
    BOOL *pbCombineImage,
    CIUIString *pstrCombineImageName,
    CIUIString *pstrImageNameN,
    CIUIString *pstrImageNameD) const
{
    if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
    {
        if (pstrCombineImageName != NULL)
        {
            *pstrCombineImageName = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_COMBINE), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pstrImageNameN != NULL)
        {
            *pstrImageNameN = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
    {
        if (pstrImageNameD != NULL)
        {
            *pstrImageNameD = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_DISABLED), 0);
        }
    }

    return 0;
}

int TreeView::SetSelectedItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
        MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
    SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
        MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

    return 0;
}

int TreeView::GetSelectedItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
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

int TreeView::SetSelectedItemTextColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetSelectedItemTextColor() const
{
    return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_SELECTED, CONTROL_STATE_NORMAL), 0);
}

int TreeView::EnableHighlightItemEffect(BOOL bEnable)
{
    if (bEnable)
    {
        AddStyle(TVS_TRACKSELECT);
    }
    else
    {
        RemoveStyle(TVS_TRACKSELECT);
    }

    return 0;
}

BOOL TreeView::IsHighlightItemEffectEnabled() const
{
    return IsIncludeFlag(GetStyle(), TVS_TRACKSELECT);
}

HTREEITEM TreeView::GetHighlightItem() const
{
    return (HTREEITEM)SendMessage(TVM_GET_HIGHLIGHT_ITEM, 0, 0);
}

int TreeView::SetHighlightItemBkType(BACKGROUND_TYPE eBkType)
{
    return SendMessage(WLM_SET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), eBkType);
}

BACKGROUND_TYPE TreeView::GetHighlightItemBkType() const
{
    return (BACKGROUND_TYPE)SendMessage(WLM_GET_ITEM_BK_TYPE,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetHighlightItemBkColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetHighlightItemBkColor() const
{
    return SendMessage(WLM_GET_ITEM_BK_COLOR,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetHighlightItemBkImage(
    UINT uMask,
    LPCTSTR lpszImageNameBkFocus,
    LPCTSTR lpszImageNameBkNoFocus)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE,
        MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_FOCUS), (LPARAM)lpszImageNameBkFocus);
    SendMessage(WLM_SET_ITEM_BK_IMAGE,
        MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)lpszImageNameBkNoFocus);

    return 0;
}

int TreeView::GetHighlightItemBkImage(
    UINT uMask,
    BOOL *pbCombineImage,
    CIUIString *pstrCombineImageName,
    CIUIString *pstrImageNameBkFocus,
    CIUIString *pstrImageNameBkNoFocus)
{
    if (IsIncludeFlag(uMask, CONTROL_STATE_COMBINE))
    {
        if (pstrCombineImageName != NULL)
        {
            *pstrCombineImageName = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_COMBINE), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_FOCUS))
    {
        if (pstrImageNameBkFocus != NULL)
        {
            *pstrImageNameBkFocus = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_FOCUS), 0);
        }
    }

    if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
    {
        if (pstrImageNameBkNoFocus != NULL)
        {
            *pstrImageNameBkNoFocus = (LPCTSTR)SendMessage(WLM_GET_ITEM_BK_IMAGE,
                    MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
        }
    }

    return 0;
}

int TreeView::SetHighlightItemBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
    SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
        MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
    SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
        MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

    return 0;
}

int TreeView::GetHighlightItemBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
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

int TreeView::SetHighlightItemTextColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), cr);
}

COLORREF TreeView::GetHighlightItemTextColor() const
{
    return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

int TreeView::SetDisabledItemTextColor(COLORREF cr)
{
    return SendMessage(WLM_SET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_DISABLED), cr);
}

COLORREF TreeView::GetDisabledItemTextColor() const
{
    return SendMessage(WLM_GET_ITEM_TEXT_COLOR,
            MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_DISABLED), 0);
}

int TreeView::SetTextFont(UINT uMask, LPCTSTR lpszFontIDN, LPCTSTR lpszFontIDD)
{
    //TREEMEMBER *pMember = (TREEMEMBER *)m_pMember;

    //IUISetControlFont2(uMask, pMember->m_hIUIFont, lpszFontIDN, lpszFontIDD);

    //HFONT hFont = NULL;
    //if (IsWindowEnabled())
    //{
    //	hFont = GetHFont(pMember->m_hIUIFont[0]);
    //}
    //else
    //{
    //	hFont = GetHFont(pMember->m_hIUIFont[1]);
    //}
    //::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

    return 0;
}

int TreeView::GetTextFont(UINT uMask, CIUIString *pstrFontIDN, CIUIString *pstrFontIDD/* = NULL*/) const
{
    //TREEMEMBER *pMember = (TREEMEMBER *)m_pMember;

    //if (pstrFontIDN != NULL)
    //{
    //	GetFontResID(pMember->m_hIUIFont[0], pstrFontIDN);
    //}

    //if (pstrFontIDD != NULL)
    //{
    //	GetFontResID(pMember->m_hIUIFont[1], pstrFontIDD);
    //}

    return 0;
}

int TreeView::SetHoverTime(DWORD dwHoverTime, BOOL bRepeat/*=FALSE*/)
{
    return SendMessage(TVM_SET_HOVER_TIME, dwHoverTime, bRepeat);
}

int TreeView::GetHoverTime(DWORD *pdwHoverTime, BOOL *pbRepeat) const
{
    return SendMessage(TVM_GET_HOVER_TIME, *pdwHoverTime, *pbRepeat);
}

// Not include children's children.
int TreeView::GetChildrenCount(HTREEITEM hItem) const
{
    int nCount = 0;

    hItem = GetChildItem(hItem);
    while (hItem != NULL)
    {
        ++nCount;

        hItem = GetNextItem(hItem, TVGN_NEXT);
    }

    return nCount;
}

int TreeView::CollapseAll()
{
    HTREEITEM hRoot = GetRootItem();
    while (hRoot != NULL)
    {
        Expand(hRoot, TVE_COLLAPSE);

        hRoot = GetNextSiblingItem(hRoot);
    }

    return 0;
}

int TreeView::SetMouseWheelStep(int nStep)
{
    return SendMessage(TVM_SET_MOUSE_WHEEL_STEP, 0, nStep);
}

int TreeView::GetMouseWheelStep() const
{
    return SendMessage(TVM_GET_MOUSE_WHEEL_STEP, 0, 0);
}


//////////////////////////////////////////////////////////////////
// TreeView API

int IUI::TreeView_BindStyle(HWLWND hWnd, const CTreeViewProp *pCtrlProp)
{
    HWLWND_BindStyle(hWnd, pCtrlProp);

    CTreeViewProp *pTreeViewProp = (CTreeViewProp *)pCtrlProp;
    CIUIProject *pProject = GetProject(hWnd);

    // Border
    // 	if (pTreeViewProp->m_bBorder)
    // 	{
    // 		dwStyle |= WS_BORDER;
    // 	}

    // Is check boxes
    DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pTreeViewProp);
    if (pTreeViewProp->m_bCheckBoxes)
    {
        dwStyle |= TVS_CHECKBOXES;
    }

    // Is Disable drag drop
    if (pTreeViewProp->m_bDisableDragDrop)
    {
        dwStyle |= TVS_DISABLEDRAGDROP;
    }

    // Is Edit labels
    if (pTreeViewProp->m_bEditLabels)
    {
        dwStyle |= TVS_EDITLABELS;
    }

    // Is Full row select
    if (pTreeViewProp->m_bFullRowSel)
    {
        dwStyle |= TVS_FULLROWSELECT;
    }

    // Is Has buttons
    if (pTreeViewProp->m_bHasButtons)
    {
        dwStyle |= TVS_HASBUTTONS;
    }

    // Is Has lines
    if (pTreeViewProp->m_bHasLines)
    {
        dwStyle |= TVS_HASLINES;
    }

    // Is Info tip
    if (pTreeViewProp->m_bInfoTip)
    {
        dwStyle |= TVS_INFOTIP;
    }

    // Is Line at root
    if (pTreeViewProp->m_bLinesAtRoot)
    {
        dwStyle |= TVS_LINESATROOT;
    }

    // Is Non even height
    if (pTreeViewProp->m_bNonEvenHeight)
    {
        dwStyle |= TVS_NONEVENHEIGHT;
    }

    // Is Scroll
    if (!(bool)pTreeViewProp->m_bScroll)
    {
        dwStyle |= TVS_NOSCROLL;
    }

    // Is Show select always
    if (pTreeViewProp->m_bShowSelAlways)
    {
        dwStyle |= TVS_SHOWSELALWAYS;
    }

    // Is Single expand
    if (pTreeViewProp->m_bSingleExpand)
    {
        dwStyle |= TVS_SINGLEEXPAND;
    }

    // Is Tool tips
    if (!(bool)pTreeViewProp->m_bToolTips)
    {
        dwStyle |= TVS_NOTOOLTIPS;
    }

    // Is Track select
    if ((bool)pTreeViewProp->m_bEnableHighlightItemEffect)
    {
        dwStyle |= TVS_TRACKSELECT;
    }

    DWORD dwOldStyle = GetStyle(hWnd);
    SetStyle(hWnd, dwOldStyle | dwStyle);

    TreeView *ptv = (TreeView *)CWLWnd::FromHandle(hWnd);

    // Set bitmaps for background
    CIUIString strImageName[1 + COMBINEIMAGESIZE2];
    BOOL bCombineImage = TRUE;
    CControlProp::GetBackground2(pCtrlProp, &bCombineImage, strImageName);
    if (bCombineImage)
    {
        SetCombineBkImage(hWnd, strImageName[0]);
    }
    else
    {
        ptv->SetBkImage(CONTROL_STATE_ND, strImageName[1], strImageName[2], FALSE);
    }

    ptv->SetBkImageResizeMode(pCtrlProp->m_eBkImageResizeMode);
    ptv->SetBkImage9GridResizeParam(&(RECT)pCtrlProp->m_rcBkImage9GridResizeArg);

    // Set bk color
    ptv->SetDrawBkColor((bool)pCtrlProp->m_bDrawBkColor);

    COLORREF crBkN = pProject->GetColor((LPCTSTR)pCtrlProp->m_strBkColorResIDN);
    COLORREF crBkD = pProject->GetColor((LPCTSTR)pCtrlProp->m_strBkColorResIDD);
    SetBkColor(hWnd, crBkN, crBkD);

    // Item Line
    ptv->EnableItemLine((bool)pTreeViewProp->m_bEnableItemLineEffect);
    COLORREF crItemLine = pProject->GetColor(pTreeViewProp->m_strItemLineColorResID);
    ptv->SetItemLineColor(crItemLine);

    // Collapse and Expanded image
    if (!CIUIString((LPCTSTR)pTreeViewProp->m_strCombineCollapseExpandedImage).IsEmpty())
    {
        ptv->SetCombineCollapseExpandedImage((LPCTSTR)pTreeViewProp->m_strCombineCollapseExpandedImage);
    }
    else
    {
        ptv->SetCollapseImage(CONTROL_STATE_ALL,
            (LPCTSTR)pTreeViewProp->m_strCollapseImageN,
            (LPCTSTR)pTreeViewProp->m_strCollapseImageH,
            (LPCTSTR)pTreeViewProp->m_strCollapseImageS,
            (LPCTSTR)pTreeViewProp->m_strCollapseImageD);

        ptv->SetExpandedImage(CONTROL_STATE_ALL,
            (LPCTSTR)pTreeViewProp->m_strExpandedImageN,
            (LPCTSTR)pTreeViewProp->m_strExpandedImageH,
            (LPCTSTR)pTreeViewProp->m_strExpandedImageS,
            (LPCTSTR)pTreeViewProp->m_strExpandedImageD);
    }

    // Text horizontal alignment
    ptv->SetTextAlignHor(
        pTreeViewProp->m_eTextHorAlignModeN,
        pTreeViewProp->m_eTextHorAlignModeH,
        pTreeViewProp->m_eTextHorAlignModeS,
        pTreeViewProp->m_eTextHorAlignModeD, FALSE);

    // Text horizontal alignment
    ptv->SetTextAlignVer(
        pTreeViewProp->m_eTextVerAlignModeN,
        pTreeViewProp->m_eTextVerAlignModeH,
        pTreeViewProp->m_eTextVerAlignModeS,
        pTreeViewProp->m_eTextVerAlignModeD, FALSE);

    // Text Margin
    RECT rcTextMarginN = (RECT)pTreeViewProp->m_rcTextMarginN;
    GetDPIPadding(&rcTextMarginN);
    RECT rcTextMarginH = (RECT)pTreeViewProp->m_rcTextMarginH;
    GetDPIPadding(&rcTextMarginH);
    RECT rcTextMarginS = (RECT)pTreeViewProp->m_rcTextMarginP;
    GetDPIPadding(&rcTextMarginS);
    RECT rcTextMarginD = (RECT)pTreeViewProp->m_rcTextMarginD;
    GetDPIPadding(&rcTextMarginD);
    ptv->SetTextMargin(&rcTextMarginN, &rcTextMarginH, &rcTextMarginS, &rcTextMarginD, FALSE);

    // Item Height
    if (!(bool)pTreeViewProp->m_bDefaultItemHeight)
    {
        int lItemHeight = GetDPIHeight((LONG)pTreeViewProp->m_nItemHeight);

        ptv->SetItemHeight((SHORT)lItemHeight);
    }

    // Set item font
    CIUIString strResFontID[2];
    CControlProp::GetControlFont2(pCtrlProp, strResFontID);
    ptv->SetTextFont(CONTROL_STATE_ND, strResFontID[0], strResFontID[1]);

    // Set item text color
    COLORREF cr[4] = {0};
    pProject->GetControlTextColor4(pTreeViewProp, cr);
    ptv->SetTextColor(cr[0]);
    ptv->SetHighlightItemTextColor(cr[1]);
    ptv->SetSelectedItemTextColor(cr[2]);
    ptv->SetDisabledItemTextColor(cr[3]);

    // Focus Item
    ptv->SetFocusItemBkType((BACKGROUND_TYPE)pTreeViewProp->m_eFocusItemBkType);
    if (BKT_COLOR == (BACKGROUND_TYPE)pTreeViewProp->m_eFocusItemBkType)
    {
        COLORREF cr = pProject->GetColor(pTreeViewProp->m_strFocusItemBkColorResID);
        ptv->SetFocusItemBkColor(cr);
    }
    else if (BKT_IMAGE == (BACKGROUND_TYPE)pTreeViewProp->m_eFocusItemBkType)
    {
        ptv->SetFocusItemBkImage((LPCTSTR)pTreeViewProp->m_strFocusItemBkImage);

        ptv->SetFocusItemBkImageResizeArg(IRM_9GRID,
            &(RECT)pTreeViewProp->m_rcFocusItemBkImage9GridResize);
    }

    // Selected Item
    ptv->SetSelectedItemBkType((BACKGROUND_TYPE)pTreeViewProp->m_eSelectedItemBkType);
    if (BKT_COLOR == (BACKGROUND_TYPE)pTreeViewProp->m_eSelectedItemBkType)
    {
        COLORREF cr = pProject->GetColor(pTreeViewProp->m_strSelectedItemBkColorResID);
        ptv->SetSelectedItemBkColor(cr);
    }
    else if (BKT_IMAGE == (BACKGROUND_TYPE)pTreeViewProp->m_eSelectedItemBkType)
    {
        ptv->SetSelectedItemBkImage(CONTROL_STATE_ND,
            (LPCTSTR)pTreeViewProp->m_strSelectedItemBkImageN,
            (LPCTSTR)pTreeViewProp->m_strSelectedItemBkImageD);

        ptv->SetSelectedItemBkImageResizeArg(IRM_9GRID,
            &(RECT)pTreeViewProp->m_rcSelectedItemBkImage9GridResize);
    }

    // Highlight Item
    ptv->EnableHighlightItemEffect((bool)pTreeViewProp->m_bEnableHighlightItemEffect);
    ptv->SetHighlightItemBkType((BACKGROUND_TYPE)pTreeViewProp->m_eHighlightItemBkType);
    if (BKT_COLOR == (BACKGROUND_TYPE)pTreeViewProp->m_eHighlightItemBkType)
    {
        COLORREF cr = pProject->GetColor(pTreeViewProp->m_strHighlightItemBkColorResID);
        ptv->SetHighlightItemBkColor(cr);
    }
    else if (BKT_IMAGE == (BACKGROUND_TYPE)pTreeViewProp->m_eHighlightItemBkType)
    {
        ptv->SetHighlightItemBkImage(CONTROL_STATE_ND,
            pTreeViewProp->m_strHighlightItemBkImageN,
            pTreeViewProp->m_strHighlightItemBkImageD);

        ptv->SetHighlightItemBkImageResizeArg(IRM_9GRID,
            &(RECT)pTreeViewProp->m_rcHighlightItemBkImage9GridResize);
    }

    // 绑定Scroll Bar。因为在TreeView控件创建的时候，ScrollBar控件可能还未创建
    // 所以，TreeView控件中，只记录ScrollBar绑定到哪个Style。 当ScrollBar创建时，
    // 使用记录的Style来初始化。
    // Horizontal scroll bar
    {
        CControlProp *pSbStyle = GetProject(hWnd)->GetStyleItem(STYLET_SCROLLBAR,
                (LPCTSTR)pTreeViewProp->m_strHorScrollbarID);
        BindHorzScrollBarStyle(hWnd, (const CScrollBarProp *)pSbStyle);
    }

    // Vertical scroll bar
    CControlProp *pSbStyle = GetProject(hWnd)->GetStyleItem(STYLET_SCROLLBAR,
            (LPCTSTR)pTreeViewProp->m_strVerScrollbarID);
	if (NULL != pSbStyle)
    {
        BindVertScrollBarStyle(hWnd, (const CScrollBarProp *)pSbStyle);

		// 界面编辑器里的滚动条宽度只针对绑定滚动条风格时，如果在界面编辑器中没有绑定风格，使用
		// Metrics::g_nScrollBarWidth作为默认宽度。
		// 否则Metrics::SetScrollBarWidth()设置的宽度永远无效。
		SetVScrollBarWidth(hWnd, (LONG)pCtrlProp->m_lScrollBarWidth);
		SetHScrollBarHeight(hWnd, (LONG)pCtrlProp->m_lScrollBarWidth);
	}

    return 0;
}

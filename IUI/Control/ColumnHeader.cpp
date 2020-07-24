#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


ColumnHeader::ColumnHeader(void)
{
}


ColumnHeader::~ColumnHeader(void)
{
	DestroyWindow();
}

BOOL ColumnHeader::Create(
	__in CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParentWnd,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_COLUMNHEADER, NULL,
			dwStyle, rect, hParentWnd, pParent, lpszName);
}

BOOL ColumnHeader::CreateEx(
	__in CWLWndHandler *pUserHandler,
	DWORD dwExStyle,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParentWnd,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	BOOL bRet = CWLWnd::CreateEx(pUserHandler, dwExStyle, IUI_COLUMNHEADER, NULL,
			dwStyle, rect, hParentWnd, pParent, lpszName);
	if (bRet && dwExStyle != 0)
	{
		//bRet = ModifyStyleEx(0, dwExStyle);
	}
	return bRet;
}

int ColumnHeader::BindStyle(const CControlProp *pCtrlProp)
{
	return ColumnHeader_BindStyle(m_hWnd, (const CColumnHeaderProp *)pCtrlProp);
}

int ColumnHeader::GetItemCount() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_GETITEMCOUNT, 0, 0L);
}
int ColumnHeader::InsertItem(int nPos, HDITEM *phdi)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_INSERTITEM, nPos, (LPARAM)phdi);
}
BOOL ColumnHeader::DeleteItem(int nPos)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_DELETEITEM, nPos, 0L);
}
BOOL ColumnHeader::GetItem(int nPos, HDITEM *pHeaderItem) const
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_GETITEM, nPos, (LPARAM)pHeaderItem);
}
BOOL ColumnHeader::SetItem(int nPos, HDITEM *pHeaderItem)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_SETITEM, nPos, (LPARAM)pHeaderItem);
}
BOOL ColumnHeader::Layout(HDLAYOUT *pHeaderLayout)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_LAYOUT, 0, (LPARAM)pHeaderLayout);
}
int ColumnHeader::HitTest(__inout LPHDHITTESTINFO pHeaderHitTestInfo)
{
	_ASSERT(IsWindow());
	return (int) SendMessage(HDM_HITTEST, 0, (LPARAM)pHeaderHitTestInfo);
}

BOOL ColumnHeader::GetItemRect(int nIndex, LPRECT lpRect) const
{
	_ASSERT(IsWindow());
	_ASSERT(lpRect != NULL);
	return (BOOL)SendMessage(HDM_GETITEMRECT, nIndex, (LPARAM)lpRect);
}
int ColumnHeader::OrderToIndex(int nOrder) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_ORDERTOINDEX, nOrder, 0);
}
int ColumnHeader::SetHotDivider(CIUIPoint pt)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_SETHOTDIVIDER, TRUE, MAKELPARAM(pt.x, pt.y));
}
int ColumnHeader::SetHotDivider(int nIndex)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_SETHOTDIVIDER, FALSE, nIndex);
}
HIMAGELIST ColumnHeader::CreateDragImage(int nIndex)
{
	_ASSERT(IsWindow());
	return (HIMAGELIST)SendMessage(HDM_CREATEDRAGIMAGE, nIndex, 0L);
}

#if _WIN32_IE >= 0x0500
int ColumnHeader::GetBitmapMargin() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_GETBITMAPMARGIN, 0, 0L);
}
int ColumnHeader::SetBitmapMargin(int nWidth)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_SETBITMAPMARGIN, nWidth, 0L);
}
int ColumnHeader::SetFilterChangeTimeout(DWORD dwTimeOut)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_SETFILTERCHANGETIMEOUT, 0, dwTimeOut);
}
int ColumnHeader::EditFilter(int nColumn, BOOL bDiscardChanges)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(HDM_EDITFILTER, nColumn, MAKELPARAM(bDiscardChanges, 0));
}
BOOL ColumnHeader::ClearFilter(int nColumn)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_CLEARFILTER, nColumn, 0L);
}
BOOL ColumnHeader::ClearAllFilters()
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(HDM_CLEARFILTER, (WPARAM)(-1), 0L);
}
#endif

#if (_WIN32_WINNT >= 0x600) && defined(UNICODE)
BOOL ColumnHeader::GetItemDropDownRect(int iItem, LPRECT lpRect) const
{
	_ASSERT(IsWindow());    // HDM_GETITEMDROPDOWNRECT
	return Header_GetItemDropDownRect(m_hWnd, iItem, lpRect);
}
BOOL ColumnHeader::GetOverflowRect(LPRECT lpRect) const
{
	_ASSERT(IsWindow());    // HDM_GETOVERFLOWRECT
	return Header_GetOverflowRect(m_hWnd, lpRect);
}
int ColumnHeader::GetFocusedItem() const
{
	_ASSERT(IsWindow());    // HDM_GETFOCUSEDITEM
	return Header_GetFocusedItem(m_hWnd);
}
BOOL ColumnHeader::SetFocusedItem(int iItem)
{
	_ASSERT(IsWindow());    // HDM_SETFOCUSEDITEM
	return Header_SetFocusedItem(m_hWnd, iItem);
}
#endif // _WIN32_WINNT >= 0x600 && defined(UNICODE)

// IUI Add
int ColumnHeader::SetBkImage(UINT uMask, LPCTSTR lpszImageNameN, LPCTSTR lpszImageNameD)
{
	IUISetControlBkImage2(m_hWnd, uMask, lpszImageNameN, lpszImageNameD);

	return 0;
}

int ColumnHeader::GetBkImage(
	UINT uMask,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD) const
{
	_ASSERT(IsWindow());

	return IUIGetControlBkImage2(m_hWnd, uMask, NULL, NULL,	pstrImageNameN, pstrImageNameD);
}

int ColumnHeader::SetBkImageResizeMode(IMAGE_RESIZE_MODE eImageResizeMode)
{
	return IUI::SetBkImageResizeMode(m_hWnd, eImageResizeMode);
}

IMAGE_RESIZE_MODE ColumnHeader::GetBkImageResizeMode()
{
	return IUI::GetBkImageResizeMode(m_hWnd);
}

int ColumnHeader::SetBkImage9GridArg(LPCRECT lpRect)
{
	return IUI::SetBkImage9GridResizeParam(m_hWnd, lpRect);
}

int ColumnHeader::GetBkImage9GridArg(LPRECT lpRect)
{
	return IUI::GetBkImage9GridResizeParam(m_hWnd, lpRect);
}

int ColumnHeader::CombineButtonBkImage(BOOL bCombine)
{
	return SendMessage(HDM_COMBINE_BUTTON_BK_IMAGE, 0, bCombine);
}

int ColumnHeader::IsCombineButtonBkImage() const
{
	return SendMessage(HDM_IS_COMBINE_BUTTON_BK_IMAGE, 0, 0);
}

int ColumnHeader::SetButtonCombineImage(LPCTSTR lpszImageNameBk)
{
	return SendMessage(HDM_SET_BUTTON_BK_IMAGE, CONTROL_STATE_COMBINE, (LPARAM)lpszImageNameBk);
}

int ColumnHeader::SetButtonImage(
	UINT uMask,
	LPCTSTR lpszImageNameBkN,
	LPCTSTR lpszImageNameBkH,
	LPCTSTR lpszImageNameBkP,
	LPCTSTR lpszImageNameBkD)
{
	SendMessage(HDM_SET_BUTTON_BK_IMAGE, CONTROL_STATE_NORMAL, (LPARAM)lpszImageNameBkN);
	SendMessage(HDM_SET_BUTTON_BK_IMAGE, CONTROL_STATE_HOVER, (LPARAM)lpszImageNameBkH);
	SendMessage(HDM_SET_BUTTON_BK_IMAGE, CONTROL_STATE_PRESSED, (LPARAM)lpszImageNameBkP);
	SendMessage(HDM_SET_BUTTON_BK_IMAGE, CONTROL_STATE_DISABLED, (LPARAM)lpszImageNameBkD);

	return 0;
}

int ColumnHeader::GetButtonImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameBkN,
	CIUIString *pstrImageNameBkH,
	CIUIString *pstrImageNameBkP,
	CIUIString *pstrImageNameBkD) const
{
	if (pbCombineImage == NULL)
	{
		return -1;
	}

	if (*pbCombineImage)
	{
		*pstrCombineImageName = (LPCTSTR)SendMessage(HDM_GET_BUTTON_BK_IMAGE,
				CONTROL_STATE_COMBINE, 0);
	}
	else
	{
		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			*pstrImageNameBkN = (LPCTSTR)SendMessage(HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_NORMAL, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
		{
			*pstrImageNameBkH = (LPCTSTR)SendMessage(HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_HOVER, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
		{
			*pstrImageNameBkP = (LPCTSTR)SendMessage(HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_PRESSED, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			*pstrImageNameBkD = (LPCTSTR)SendMessage(HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_DISABLED, 0);
		}
	}

	return 0;
}

int ColumnHeader::SetButtonBkImageResizeArg(IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
	SendMessage(WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ColumnHeader::GetButtonBkImageResizeArg(IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid) const
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE,
				MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), 0);
	}

	SendMessage(WLM_GET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int ColumnHeader::SetTextColor(COLORREF crN, COLORREF crH, COLORREF crS, COLORREF crD)
{
	_ASSERT(IsWindow());

	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), crH);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), crS);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	return 0;
}

int ColumnHeader::GetTextColor(
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrS,
	COLORREF *pcrD) const
{
	_ASSERT(IsWindow());

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrH != NULL)
	{
		*pcrH = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}
	if (pcrS != NULL)
	{
		*pcrS = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ColumnHeader::SetTextFont(
	UINT uMask,
	LPCTSTR lpszFontIDN,
	LPCTSTR lpszFontIDH,
	LPCTSTR lpszFontIDP,
	LPCTSTR lpszFontIDD
)
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lpszFontIDN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lpszFontIDH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lpszFontIDP);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lpszFontIDD);
	}

	return 0;
}

int ColumnHeader::GetTextFont(
	UINT uMask,
	CIUIString *pstrFontIDN,
	CIUIString *pstrFontIDH,
	CIUIString *pstrFontIDP,
	CIUIString *pstrFontIDD) const
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pstrFontIDN == NULL)
		{
			return -1;
		}

		*pstrFontIDN = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pstrFontIDH == NULL)
		{
			return -1;
		}

		*pstrFontIDH = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pstrFontIDP == NULL)
		{
			return -1;
		}

		*pstrFontIDP = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pstrFontIDD == NULL)
		{
			return -1;
		}

		*pstrFontIDD = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ColumnHeader::SetHeaderHeight(int nHeight)
{
	return ColumnHeader_SetHeaderHeight(m_hWnd, nHeight);
}

int ColumnHeader::GetHeaderHeight() const
{
	CIUIRect rc;
	GetWindowRectToParent(rc);
	return rc.Height();
}

int ColumnHeader::EnableHeaderTrack(BOOL bEnable)
{
	return SendMessage(HDM_ENABLE_HEADER_TRACK, 0, bEnable);
}

BOOL ColumnHeader::IsHeaderTrackEnable() const
{
	return SendMessage(HDM_IS_HEADER_TRACK_ENABLED, 0, 0);
}

int ColumnHeader::EnableHeaderDragDrop(BOOL bEnable)
{
	return SendMessage(HDM_ENABLE_HEADER_DRAG, 0, bEnable);
}

BOOL ColumnHeader::IsHeaderDragDropEnable() const
{
	return SendMessage(HDM_IS_HEADER_DRAG_ENABLED, 0, 0);
}

int ColumnHeader::HitTest(CIUIPoint pt)
{
	HDHITTESTINFO hdHitTestInfo;
	hdHitTestInfo.pt = pt;
	SendMessage(HDM_HITTEST, 0, LPARAM(&hdHitTestInfo));

	if (hdHitTestInfo.flags == HHT_ONDIVIDER)
	{
		return -1;
	}

	int nCount = GetItemCount();
	for (int i = 0; i < nCount; ++i)
	{
		CIUIRect rect;
		GetItemRect(i, rect);

		if (rect.PtInRect(pt))
		{
			return i;
		}
	}

	return -1;
}

///////////////////////////////////////////////////////////
// ColumnHeader API
int IUI::ColumnHeader_BindStyle(HWLWND hHeader, const CColumnHeaderProp *pProp)
{
	if (pProp == NULL)
	{
		return -1;
	}

	BindStyle(hHeader, pProp);

	ColumnHeader_EnableHeaderTrack(hHeader, (bool)pProp->m_bHeaderTrack);
	ColumnHeader_EnableHeaderDragDrop(hHeader, (bool)pProp->m_bHeaderDragDrop);

	// Background
	SetDrawBkColor(hHeader, (bool)pProp->m_bDrawBkColor);
	CIUIProject *pProject = GetProject(hHeader);
	COLORREF cr[2];
	pProject->GetControlBkColor2(pProp, cr);
	SetBkColor(hHeader, cr[0], cr[1]);

	CIUIString strImageName[1 + COMBINEIMAGESIZE2];
	BOOL bCombineImage = TRUE;
	CControlProp::GetBackground2(pProp, &bCombineImage, strImageName);
	CombineBkImage(hHeader, bCombineImage);
	if (bCombineImage)
	{
		SetCombineBkImage(hHeader, strImageName[0]);
	}
	else
	{
		IUISetControlBkImage2(hHeader, CONTROL_STATE_ND, strImageName[1], strImageName[2]);
	}

	SetBkImageResizeMode(hHeader, pProp->m_eBkImageResizeMode);
	SetBkImage9GridResizeParam(hHeader, &(RECT)pProp->m_rcBkImage9GridResizeArg);

	// Item±³¾°
	ColumnHeader_CombineButtonBkImage(hHeader, (bool)pProp->m_bCombineButtonImage);
	if ((bool)pProp->m_bCombineButtonImage)
	{
		ColumnHeader_SetButtonCombineImage(hHeader, pProp->m_strButtonBkCombine);
	}
	else
	{
		ColumnHeader_SetButtonImage(hHeader, CONTROL_STATE_ALL,
			pProp->m_strButtonBKImageN, pProp->m_strButtonBKImageH,
			pProp->m_strButtonBKImageP, pProp->m_strButtonBKImageD);
	}
	ColumnHeader_SetButtonBkImageResizeArg(hHeader,
		pProp->m_eButtonBkImageResizeMode, &(RECT)pProp->m_rcButtonBkImage9GridResizeArg);

	return 0;
}

int IUI::ColumnHeader_GetItemCount(HWLWND hHeader)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_GETITEMCOUNT, 0, 0L);
}
int IUI::ColumnHeader_InsertItem(HWLWND hHeader, int nPos, HDITEM *phdi)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_INSERTITEM, nPos, (LPARAM)phdi);
}
BOOL IUI::ColumnHeader_DeleteItem(HWLWND hHeader, int nPos)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_DELETEITEM, nPos, 0L);
}
BOOL IUI::ColumnHeader_GetItem(HWLWND hHeader, int nPos, HDITEM *pHeaderItem)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_GETITEM, nPos, (LPARAM)pHeaderItem);
}
BOOL IUI::ColumnHeader_SetItem(HWLWND hHeader, int nPos, HDITEM *pHeaderItem)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_SETITEM, nPos, (LPARAM)pHeaderItem);
}
BOOL IUI::ColumnHeader_Layout(HWLWND hHeader, HDLAYOUT *pHeaderLayout)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_LAYOUT, 0, (LPARAM)pHeaderLayout);
}
int IUI::ColumnHeader_HitTest(HWLWND hHeader, __inout LPHDHITTESTINFO pHeaderHitTestInfo)
{
	_ASSERT(IsWindow(hHeader));
	return (int) SendMessage(hHeader, HDM_HITTEST, 0, (LPARAM)pHeaderHitTestInfo);
}

BOOL IUI::ColumnHeader_GetItemRect(HWLWND hHeader, int nIndex, LPRECT lpRect)
{
	_ASSERT(IsWindow(hHeader));
	_ASSERT(lpRect != NULL);
	return (BOOL)SendMessage(hHeader, HDM_GETITEMRECT, nIndex, (LPARAM)lpRect);
}
int IUI::ColumnHeader_OrderToIndex(HWLWND hHeader, int nOrder)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_ORDERTOINDEX, nOrder, 0);
}
int IUI::ColumnHeader_SetHotDivider(HWLWND hHeader, CIUIPoint pt)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_SETHOTDIVIDER, TRUE, MAKELPARAM(pt.x, pt.y));
}
int IUI::ColumnHeader_SetHotDivider(HWLWND hHeader, int nIndex)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_SETHOTDIVIDER, FALSE, nIndex);
}
HIMAGELIST IUI::ColumnHeader_CreateDragImage(HWLWND hHeader, int nIndex)
{
	_ASSERT(IsWindow(hHeader));
	return (HIMAGELIST)SendMessage(hHeader, HDM_CREATEDRAGIMAGE, nIndex, 0L);
}

#if _WIN32_IE >= 0x0500
int IUI::ColumnHeader_GetBitmapMargin(HWLWND hHeader)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_GETBITMAPMARGIN, 0, 0L);
}
int IUI::ColumnHeader_SetBitmapMargin(HWLWND hHeader, int nWidth)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_SETBITMAPMARGIN, nWidth, 0L);
}
int IUI::ColumnHeader_SetFilterChangeTimeout(HWLWND hHeader, DWORD dwTimeOut)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_SETFILTERCHANGETIMEOUT, 0, dwTimeOut);
}
int IUI::ColumnHeader_EditFilter(HWLWND hHeader, int nColumn, BOOL bDiscardChanges)
{
	_ASSERT(IsWindow(hHeader));
	return (int)SendMessage(hHeader, HDM_EDITFILTER, nColumn, MAKELPARAM(bDiscardChanges, 0));
}
BOOL IUI::ColumnHeader_ClearFilter(HWLWND hHeader, int nColumn)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_CLEARFILTER, nColumn, 0L);
}
BOOL IUI::ColumnHeader_ClearAllFilters(HWLWND hHeader)
{
	_ASSERT(IsWindow(hHeader));
	return (BOOL)SendMessage(hHeader, HDM_CLEARFILTER, (WPARAM)(-1), 0L);
}
#endif

#if (_WIN32_WINNT >= 0x600) && defined(UNICODE)
BOOL IUI::ColumnHeader_GetItemDropDownRect(HWLWND hHeader, int iItem, LPRECT lpRect)
{
	_ASSERT(IsWindow(hHeader));    // HDM_GETITEMDROPDOWNRECT
	return Header_GetItemDropDownRect(m_hWnd, iItem, lpRect);
}
BOOL IUI::ColumnHeader_GetOverflowRect(HWLWND hHeader, LPRECT lpRect)
{
	_ASSERT(IsWindow(hHeader));    // HDM_GETOVERFLOWRECT
	return Header_GetOverflowRect(m_hWnd, lpRect);
}
int IUI::ColumnHeader_GetFocusedItem(HWLWND hHeader)
{
	_ASSERT(IsWindow(hHeader));    // HDM_GETFOCUSEDITEM
	return Header_GetFocusedItem(m_hWnd);
}
BOOL IUI::ColumnHeader_SetFocusedItem(HWLWND hHeader, int iItem)
{
	_ASSERT(IsWindow(hHeader));    // HDM_SETFOCUSEDITEM
	return Header_SetFocusedItem(m_hWnd, iItem);
}
#endif // _WIN32_WINNT >= 0x600 && defined(UNICODE)

// IUI Add
int IUI::ColumnHeader_SetBkImage(HWLWND hHeader, UINT uMask, LPCTSTR lpszImageNameN, LPCTSTR lpszImageNameD)
{
	IUISetControlBkImage2(hHeader, uMask, lpszImageNameN, lpszImageNameD);

	return 0;
}

int IUI::ColumnHeader_GetBkImage(
	HWLWND hHeader,
	UINT uMask,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD)
{
	_ASSERT(IsWindow(hHeader));

	return IUIGetControlBkImage2(hHeader, uMask, NULL, NULL,	pstrImageNameN, pstrImageNameD);
}

int IUI::ColumnHeader_SetBkImageResizeMode(HWLWND hHeader, IMAGE_RESIZE_MODE eImageResizeMode)
{
	return IUI::SetBkImageResizeMode(hHeader, eImageResizeMode);
}

IMAGE_RESIZE_MODE IUI::ColumnHeader_GetBkImageResizeMode(HWLWND hHeader)
{
	return IUI::GetBkImageResizeMode(hHeader);
}

int IUI::ColumnHeader_SetBkImage9GridArg(HWLWND hHeader, LPCRECT lpRect)
{
	return IUI::SetBkImage9GridResizeParam(hHeader, lpRect);
}

int IUI::ColumnHeader_GetBkImage9GridArg(HWLWND hHeader, LPRECT lpRect)
{
	return IUI::GetBkImage9GridResizeParam(hHeader, lpRect);
}

int IUI::ColumnHeader_CombineButtonBkImage(HWLWND hHeader, BOOL bCombine)
{
	return SendMessage(hHeader, HDM_COMBINE_BUTTON_BK_IMAGE, 0, bCombine);
}

int IUI::ColumnHeader_IsCombineButtonBkImage(HWLWND hHeader)
{
	return SendMessage(hHeader, HDM_IS_COMBINE_BUTTON_BK_IMAGE, 0, 0);
}

int IUI::ColumnHeader_SetButtonCombineImage(HWLWND hHeader, LPCTSTR lpszImageNameBk)
{
	return SendMessage(hHeader, HDM_SET_BUTTON_BK_IMAGE,
			CONTROL_STATE_COMBINE, (LPARAM)lpszImageNameBk);
}

int IUI::ColumnHeader_SetButtonImage(
	HWLWND hHeader,
	UINT uMask,
	LPCTSTR lpszImageNameBkN,
	LPCTSTR lpszImageNameBkH,
	LPCTSTR lpszImageNameBkP,
	LPCTSTR lpszImageNameBkD)
{
	SendMessage(hHeader, HDM_SET_BUTTON_BK_IMAGE,
		CONTROL_STATE_NORMAL, (LPARAM)lpszImageNameBkN);
	SendMessage(hHeader, HDM_SET_BUTTON_BK_IMAGE,
		CONTROL_STATE_HOVER, (LPARAM)lpszImageNameBkH);
	SendMessage(hHeader, HDM_SET_BUTTON_BK_IMAGE,
		CONTROL_STATE_PRESSED, (LPARAM)lpszImageNameBkP);
	SendMessage(hHeader, HDM_SET_BUTTON_BK_IMAGE,
		CONTROL_STATE_DISABLED, (LPARAM)lpszImageNameBkD);

	return 0;
}

int IUI::ColumnHeader_GetButtonImage(
	HWLWND hHeader,
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameBkN,
	CIUIString *pstrImageNameBkH,
	CIUIString *pstrImageNameBkP,
	CIUIString *pstrImageNameBkD)
{
	if (pbCombineImage == NULL)
	{
		return -1;
	}

	if (*pbCombineImage)
	{
		*pstrCombineImageName = (LPCTSTR)SendMessage(hHeader, HDM_GET_BUTTON_BK_IMAGE,
				CONTROL_STATE_COMBINE, 0);
	}
	else
	{
		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			*pstrImageNameBkN = (LPCTSTR)SendMessage(hHeader, HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_NORMAL, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
		{
			*pstrImageNameBkH = (LPCTSTR)SendMessage(hHeader, HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_HOVER, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
		{
			*pstrImageNameBkP = (LPCTSTR)SendMessage(hHeader, HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_PRESSED, 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			*pstrImageNameBkD = (LPCTSTR)SendMessage(hHeader, HDM_GET_BUTTON_BK_IMAGE,
					CONTROL_STATE_DISABLED, 0);
		}
	}

	return 0;
}

int IUI::ColumnHeader_SetButtonBkImageResizeArg(HWLWND hHeader, IMAGE_RESIZE_MODE eIrm, LPCRECT lprc9Grid)
{
	SendMessage(hHeader, WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)eIrm);
	SendMessage(hHeader, WLM_SET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int IUI::ColumnHeader_GetButtonBkImageResizeArg(HWLWND hHeader, IMAGE_RESIZE_MODE *peIrm, LPRECT lprc9Grid)
{
	if (peIrm != NULL)
	{
		*peIrm = (IMAGE_RESIZE_MODE)SendMessage(hHeader, WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE,
				MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), 0);
	}

	SendMessage(hHeader, WLM_GET_ITEM_BK_IMAGE_9GRID_ARG,
		MAKEWPARAM(ITEM_NORMAL, CONTROL_STATE_NORMAL), (LPARAM)lprc9Grid);

	return 0;
}

int IUI::ColumnHeader_SetTextColor(HWLWND hHeader, COLORREF crN, COLORREF crH, COLORREF crS, COLORREF crD)
{
	_ASSERT(IsWindow(hHeader));

	SendMessage(hHeader, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(hHeader, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), crH);
	SendMessage(hHeader, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), crS);
	SendMessage(hHeader, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	return 0;
}

int IUI::ColumnHeader_GetTextColor(
	HWLWND hHeader,
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrS,
	COLORREF *pcrD)
{
	_ASSERT(IsWindow(hHeader));

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(hHeader, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrH != NULL)
	{
		*pcrH = SendMessage(hHeader, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}
	if (pcrS != NULL)
	{
		*pcrS = SendMessage(hHeader, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(hHeader, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int IUI::ColumnHeader_SetTextFont(
	HWLWND hHeader,
	UINT uMask,
	LPCTSTR lpszFontIDN,
	LPCTSTR lpszFontIDH,
	LPCTSTR lpszFontIDP,
	LPCTSTR lpszFontIDD
)
{
	_ASSERT(IsWindow(hHeader));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(hHeader, WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lpszFontIDN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		SendMessage(hHeader, WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lpszFontIDH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		SendMessage(hHeader, WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lpszFontIDP);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(hHeader, WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lpszFontIDD);
	}

	return 0;
}

int IUI::ColumnHeader_GetTextFont(
	HWLWND hHeader,
	UINT uMask,
	CIUIString *pstrFontIDN,
	CIUIString *pstrFontIDH,
	CIUIString *pstrFontIDP,
	CIUIString *pstrFontIDD)
{
	_ASSERT(IsWindow(hHeader));

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pstrFontIDN == NULL)
		{
			return -1;
		}

		*pstrFontIDN = (LPCTSTR)SendMessage(hHeader, WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pstrFontIDH == NULL)
		{
			return -1;
		}

		*pstrFontIDH = (LPCTSTR)SendMessage(hHeader, WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pstrFontIDP == NULL)
		{
			return -1;
		}

		*pstrFontIDP = (LPCTSTR)SendMessage(hHeader, WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pstrFontIDD == NULL)
		{
			return -1;
		}

		*pstrFontIDD = (LPCTSTR)SendMessage(hHeader, WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int IUI::ColumnHeader_SetHeaderHeight(HWLWND hHeader, int nHeight)
{
	_ASSERT(hHeader != NULL);
	return SendMessage(hHeader, HDM_SETHEIGHT, nHeight, 0);
}

int IUI::ColumnHeader_GetHeaderHeight(HWLWND hHeader)
{
	CIUIRect rc;
	GetWindowRectToParent(hHeader, rc);
	return rc.Height();
}

int IUI::ColumnHeader_EnableHeaderTrack(HWLWND hHeader, BOOL bEnable)
{
	return SendMessage(hHeader, HDM_ENABLE_HEADER_TRACK, 0, bEnable);
}

BOOL IUI::ColumnHeader_IsHeaderTrackEnable(HWLWND hHeader)
{
	return SendMessage(hHeader, HDM_IS_HEADER_TRACK_ENABLED, 0, 0);
}

int IUI::ColumnHeader_EnableHeaderDragDrop(HWLWND hHeader, BOOL bEnable)
{
	return SendMessage(hHeader, HDM_ENABLE_HEADER_DRAG, 0, bEnable);
}

BOOL IUI::ColumnHeader_IsHeaderDragDropEnable(HWLWND hHeader)
{
	return SendMessage(hHeader, HDM_IS_HEADER_DRAG_ENABLED, 0, 0);
}

int IUI::ColumnHeader_HitTest(HWLWND hHeader, CIUIPoint pt)
{
	HDHITTESTINFO hdHitTestInfo;
	hdHitTestInfo.pt = pt;
	SendMessage(hHeader, HDM_HITTEST, 0, LPARAM(&hdHitTestInfo));

	if (hdHitTestInfo.flags == HHT_ONDIVIDER)
	{
		return -1;
	}

	int nCount = ColumnHeader_GetItemCount(hHeader);
	for (int i = 0; i < nCount; ++i)
	{
		CIUIRect rect;
		ColumnHeader_GetItemRect(hHeader, i, rect);

		if (rect.PtInRect(pt))
		{
			return i;
		}
	}

	return -1;
}

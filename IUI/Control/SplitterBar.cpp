// SplitterBar.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


// SplitterBar

SplitterBar::SplitterBar()
{
}

SplitterBar::~SplitterBar()
{
}

//////////////////////////////////////////////////////////////////////////

BOOL SplitterBar::Create(
	CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_SPLITTERBAR, NULL,
			dwStyle, rect, hParent, pParent, lpszName);
}

//int SplitterBar::BindStyle(const CTRLPROPERTIES *pCtrlProp)
//{
//	SPLITTERMEMBER *pMember = (SPLITTERMEMBER *)m_pMember;
//
//	if (!IsWindow(m_hWnd))
//	{
//		ASSERT(FALSE);
//		return -1;
//	}
//
//	if (pCtrlProp == NULL)
//	{
//		return -2;
//	}
//	CONTROL_TYPE ect = (CONTROL_TYPE)pCtrlProp->m_eControlType;
//	if (ect != CT_STYLE_SPLITTER && ect != CT_SPLITTER)
//	{
//		return -3;
//	}
//
//	// Set background bitmaps for control
//	CString strImageName[1 + COMBINEIMAGESIZE4];
//	BOOL bCombineImage = TRUE;
//	CTRLPROPERTIES::IUIGetBackground4(pCtrlProp, &bCombineImage, strImageName);
//	if (bCombineImage)
//	{
//		SetBkCombineImage(strImageName[0]);
//	}
//	else
//	{
//		SetBkImages(CONTROL_STATE_UNCHECKED_ALL,
//			strImageName[1], strImageName[2], strImageName[3], strImageName[4]);
//	}
//	for (int i = 0; i < 1 + COMBINEIMAGESIZE4; ++i)
//	{
//		ReleaseIUIImage(strImageName[i]);
//	}
//
//	SetOrientation(((SPLITTERPROPERTIES *)pCtrlProp)->m_eOrientation);
//
//	return 0;
//}


//////////////////////////////////////////////////////////////////////////
// public

int SplitterBar::SetBkImage(
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw)
{
	IUISetControlBkImage4(m_hWnd, FALSE, uMask,
		lpszImageNameN, lpszImageNameH, lpszImageNameP, lpszImageNameD);

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int SplitterBar::GetBkImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameP,
	CIUIString *pstrImageNameD) const
{
	_ASSERT(IsWindow());

	return IUIGetControlBkImage4(m_hWnd, FALSE, uMask, pbCombineImage, pstrCombineImageName,
			pstrImageNameN, pstrImageNameH, pstrImageNameP, pstrImageNameD);
}

int SplitterBar::SetHCursor(HCURSOR hCursor)
{
	_ASSERT(IsWindow());

	return SendMessage(SPB_SET_CURSOR, FALSE, (LPARAM)hCursor);
}

HCURSOR SplitterBar::GetHCursor()
{
	_ASSERT(IsWindow());

	return (HCURSOR)SendMessage(SPB_GET_CURSOR, FALSE, 0);
}

int SplitterBar::SetVCursor(HCURSOR hCursor)
{
	_ASSERT(IsWindow());

	return SendMessage(SPB_SET_CURSOR, TRUE, (LPARAM)hCursor);
}

HCURSOR SplitterBar::GetVCursor()
{
	_ASSERT(IsWindow());

	return (HCURSOR)SendMessage(SPB_GET_CURSOR, TRUE, 0);
}

int SplitterBar::SetOrientation(SPLITTER_ORIENTATION eOrientation)
{
	_ASSERT(IsWindow());

	return SendMessage(SPB_SET_ORIENTATION, 0, (LPARAM)eOrientation);
}

SPLITTER_ORIENTATION SplitterBar::GetOrientation() const
{
	_ASSERT(IsWindow());

	return (SPLITTER_ORIENTATION)SendMessage(SPB_GET_ORIENTATION, 0, 0);
}

int SplitterBar::AddPanel(HWLWND hPanel1, HWLWND hPanel2)
{
	_ASSERT(IsWindow());

	return (SPLITTER_ORIENTATION)SendMessage(SPB_ADD_PANEL, (WPARAM)hPanel1, (LPARAM)hPanel2);
}

HWLWND SplitterBar::GetPanel1()
{
	_ASSERT(IsWindow());

	return (HWLWND)SendMessage(SPB_GET_PANEL, FALSE, 0);
}

HWLWND SplitterBar::GetPanel2()
{
	_ASSERT(IsWindow());

	return (HWLWND)SendMessage(SPB_GET_PANEL, TRUE, 0);
}

HWLWND SplitterBar::ReplacePanel1(HWLWND hNewPanel1)
{
	_ASSERT(IsWindow());

	return (HWLWND)SendMessage(SPB_REPLACE_PANEL, FALSE, (LPARAM)hNewPanel1);
}

HWLWND SplitterBar::ReplacePanel2(HWLWND hNewPanel2)
{
	_ASSERT(IsWindow());

	return (HWLWND)SendMessage(SPB_REPLACE_PANEL, TRUE, (LPARAM)hNewPanel2);
}

int IUI::SplitterBar_BindStyle(HWLWND hWnd, const CSplitterBarProp *pCtrlProp)
{
	return 0;
}

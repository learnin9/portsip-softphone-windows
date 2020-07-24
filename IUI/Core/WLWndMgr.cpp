#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


int CWLWndMgr::SetEditMode(HWLWND hWnd, BOOL bEditMode)
{
	if (hWnd == NULL)
	{
		return -1;
	}

	return SetProp(hWnd, ISEDITMODE, (HANDLE)bEditMode);
}

BOOL CWLWndMgr::IsEditMode(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	return (BOOL)GetProp(hWnd, ISEDITMODE);
}

// 仅得到控件自己的Rgn
HRGN GetOnlyControlRgn(HWLWND hCtrl, BOOL bClientArea)
{
	if (hCtrl == NULL)
	{
		return NULL;
	}

	HRGN hRgnRet = NULL;

	CIUIRect rect;
	if (bClientArea)
	{
		GetClientRect(hCtrl, rect);
		ClientToScreen(hCtrl, rect);
		IUIScreenToClient(GetHostWnd(hCtrl), rect);
	}
	else
	{
		GetWindowRectToHWND(hCtrl, rect);
	}

	hRgnRet = CreateRectRgnIndirect(rect);

	HRGN hWindowRgn = NULL;
	GetWindowRgn(hCtrl, hWindowRgn);
	if (hWindowRgn != NULL)
	{
		HRGN hRgnCopy = NULL;
		CopyRgn(hRgnCopy, hWindowRgn);
		OffsetRgn(hRgnCopy, rect.left, rect.top);

		CombineRgn(hRgnRet, hRgnRet, hRgnCopy, RGN_AND);
		DeleteObject(hRgnCopy);
	}

	return hRgnRet;
}

// 递归获得窗口Rgn，相对于宿主窗口
HRGN GetCombineRgn(HWLWND hCtrl, BOOL bClientArea)
{
	if (hCtrl == NULL)
	{
		return NULL;
	}

	HRGN hRgnRet = GetOnlyControlRgn(hCtrl, bClientArea);

	HWLWND hParentCtrl = GetParent(hCtrl);

	if (hParentCtrl != NULL)
	{
		HRGN hParetnRgn = GetCombineRgn(hParentCtrl, FALSE);
		if (hParetnRgn != NULL)
		{
			CombineRgn(hRgnRet, hRgnRet, hParetnRgn, RGN_AND);
			DeleteObject(hParetnRgn);
		}
		else
		{
			_ASSERT(hParetnRgn != NULL);
		}
	}

	return hRgnRet;
}

int CWLWndMgr::SetClip(HWLWND hCtrl, HDC hDC, BOOL bClientArea)
{
	if (hCtrl == NULL || hDC == NULL)
	{
		return -1;
	}

	// 混合父窗口混合后HRGN（不是SetWindowRgn设置的）、自己坐标、自己SetWindowRgn设置的HRGN
	// 而父窗口混合后的HRGN，也是由它的父窗口混合后的Rgn，坐标、SetWindowRgn这些生成的.
	// 所以，需要递归获得
	HRGN hRgn = NULL;
	if (SendMessage(hCtrl, WM_WL_ISUSERECTF, 0, 0))
	{
		Gdiplus::RectF rectF;
		GetWindowRectFToParent(hCtrl, &rectF);
		//TODO: 这个坐标不对
		hRgn = CreateRectRgn(0, 0, (int)rectF.Width, (int)rectF.Height);
	}
	else
	{
		hRgn = GetCombineRgn(hCtrl, bClientArea);

#ifdef _DEBUG
		POINT pt;
		GetViewportOrgEx(hDC, &pt);

		CIUIRect rcRgn;
		GetRgnBox(hRgn, rcRgn);

		//IUITRACE(_T("%s(%08x) Viewport: x=%d, y=%d; HRGN(left=%d, top=%d, width=%d, height=%d\n"),
		//	pCtrl->GetClassName(), pCtrl,
		//	pt.x, pt.y, rcRgn.left, rcRgn.top, rcRgn.Width(), rcRgn.Height());
		int n = 0;
#endif
	}

	// SelectClipRgn内部会生成HRGN的拷贝，所以，调用SelectClipRgn之后，
	// 传入的HRGN可以被选入另外的HDC，或者可直接删除。
	// 另注：SelectClipRgn选入的HRGN，不受HDC视点影响，即：当HRGN的坐标为(0, 0, 100, 100)
	// 而视点为(100, 100)，则不会有任何内容被绘制；当视点为(80, 80)时，则只有(80,80)到
	// (100, 100)之间、面积为（20 * 20）区域的内容可被绘制出来。
	SelectClipRgn(hDC, hRgn);
	DeleteObject(hRgn);

	return 0;
}

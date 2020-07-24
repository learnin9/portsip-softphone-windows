#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{
	CPropertySheetInfo::CPropertySheetInfo()
	{
		m_Key = 0;
		m_bKeyIsRadioButton = false;
		m_hPropertySheet = NULL;
		m_pUserHandler = NULL;
	}

	CPropertySheetInfo::~CPropertySheetInfo()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	TabControl::TabControl()
	{
	}

	TabControl::~TabControl()
	{
	}

	BOOL TabControl::Create(
		CWLWndHandler *pUserHandler,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_TABCONTROL, NULL,
				dwStyle, rect, hParent, pParent, lpszName);
	}

	int TabControl::BindStyle(const CControlProp *pCtrlProp)
	{
		CWLWnd::BindStyle(pCtrlProp);

		CTabControlProp *pTabControlProp = (CTabControlProp *)pCtrlProp;

		return 0;
	}

	BOOL TabControl::Remove(CWLWnd *pControl)
	{
		return TabControl_Remove(m_hWnd, pControl->GetSafeHwnd());
	}

	void TabControl::RemoveAll()
	{
		TabControl_RemoveAll(m_hWnd);
	}

	UINT TabControl::GetCurPropertySheet() const
	{
		return TabControl_GetCurPropertySheet(m_hWnd);
	}

	int TabControl::AddPropertySheet(UINT_PTR uKey, HWLWND hSheet, CWLWndHandler *pUserHandler)
	{
		return TabControl_AddPropertySheet(m_hWnd, uKey, hSheet, pUserHandler);
	}

	int TabControl::AddPropertySheet(HWLWND hRadioButton, HWLWND hSheet, CWLWndHandler *pUserHandler)
	{
		return TabControl_AddPropertySheet(m_hWnd, hRadioButton, hSheet, pUserHandler);
	}

	int TabControl::AddPropertySheet(UINT_PTR uKey, LPCTSTR lpszXml, CWLWndHandler *pUserHandler)
	{
		return TabControl_AddPropertySheet(m_hWnd, uKey, lpszXml, pUserHandler);
	}

	int TabControl::AddPropertySheet(HWLWND hRadioButton, LPCTSTR lpszXml, CWLWndHandler *pUserHandler)
	{
		return TabControl_AddPropertySheet(m_hWnd, hRadioButton, lpszXml, pUserHandler);
	}

	int TabControl::SwitchPropertySheet(UINT_PTR uKey)
	{
		return TabControl_SwitchPropertySheet(m_hWnd, uKey);
	}

	int TabControl::SwitchPropertySheet(HWLWND hRadioButton)
	{
		return TabControl_SwitchPropertySheet(m_hWnd, hRadioButton);
	}

	int TabControl::DetachPropertySheetHandler(UINT_PTR uKey)
	{
		return TabControl_DetachPropertySheetHandler(m_hWnd, uKey);
	}

	int TabControl::DetachPropertySheetHandler(HWLWND hRadioButton)
	{
		return TabControl_DetachPropertySheetHandler(m_hWnd, hRadioButton);
	}

	CPropertySheetInfo *TabControl::GetPropertySheet(UINT_PTR uKey) const
	{
		return TabControl_FindPropertySheet(m_hWnd, uKey);
	}

	CPropertySheetInfo *TabControl::GetPropertySheet(HWLWND hRadioButton) const
	{
		return TabControl_FindPropertySheet(m_hWnd, hRadioButton);
	}

	int TabControl::RelayoutChild()
	{
		return TabControl_RelayoutChild(m_hWnd);
	}
}

//////////////////
// TabControl API

BOOL IUI::TabControl_Remove(HWLWND hWnd, HWLWND hControl)
{
	return TRUE;
}

int IUI::TabControl_RemoveAll(HWLWND hWnd)
{
	TB *pTb = (TB *)GetMember(hWnd);
	pTb->m_uCurKey = 0;

	//TODO：

	Invalidate(hWnd);

	return 0;
}

UINT IUI::TabControl_GetCurPropertySheet(HWLWND hWnd)
{
	TB *pTb = (TB *)GetMember(hWnd);
	return pTb->m_uCurKey;
}

int IUI::TabControl_AddPropertySheet(
	HWLWND hThis,
	UINT_PTR uKey,
	HWLWND hSheet,
	CWLWndHandler *pUserHandler)
{
	TB *pTb = (TB *)GetMember(hThis);
	CPropertySheetInfo info;
	info.m_Key = (void *)uKey;
	info.m_bKeyIsRadioButton = false;
	info.m_hPropertySheet = hSheet;
	info.m_pUserHandler = pUserHandler;

	pTb->m_pvSheets->push_back(info);

	return pTb->m_pvSheets->size() - 1;
}

int IUI::TabControl_AddPropertySheet(
	HWLWND hThis,
	HWLWND hRadioButton,
	HWLWND hSheet,
	CWLWndHandler *pUserHandler)
{
	TB *pTb = (TB *)GetMember(hThis);

	CPropertySheetInfo info;
	info.m_Key = hRadioButton;
	info.m_bKeyIsRadioButton = true;
	info.m_hPropertySheet = hSheet;
	info.m_pUserHandler = pUserHandler;

	// 把TabControl保存到RadioButton中。
	RadioButton_SetBindTabControl(hRadioButton, GetName(hThis));

	pTb->m_pvSheets->push_back(info);

	return pTb->m_pvSheets->size() - 1;
}

int IUI::TabControl_AddPropertySheet(
	HWLWND hThis,
	UINT_PTR uKey,
	LPCTSTR lpszXml,
	CWLWndHandler *pUserHandler)
{
	TB *pTb = (TB *)GetMember(hThis);

	CPropertySheetInfo info;
	info.m_Key = (void *)uKey;
	info.m_bKeyIsRadioButton = FALSE;
	info.m_strPropertySheetXml = lpszXml;
	info.m_pUserHandler = pUserHandler;

	pTb->m_pvSheets->push_back(info);

	return pTb->m_pvSheets->size() - 1;
}

int IUI::TabControl_AddPropertySheet(
	HWLWND hThis,
	HWLWND hRadioButton,
	LPCTSTR lpszXml,
	CWLWndHandler *pUserHandler)
{
	TB *pTb = (TB *)GetMember(hThis);

	CPropertySheetInfo info;
	info.m_Key = hRadioButton;
	info.m_bKeyIsRadioButton = TRUE;
	info.m_strPropertySheetXml = lpszXml;
	info.m_pUserHandler = pUserHandler;

	// 把TabControl保存到RadioButton中。
	RadioButton_SetBindTabControl(hRadioButton, GetName(hThis));

	pTb->m_pvSheets->push_back(info);

	return pTb->m_pvSheets->size() - 1;
}

int IUI::TabControl_SwitchPropertySheet(HWLWND hWnd, UINT_PTR uKey)
{
	TB *pTb = (TB *)GetMember(hWnd);

	if (pTb->m_uCurKey == uKey)
	{
		return 1;
	}

	UINT uOldKey = pTb->m_uCurKey;

	// Before switch
	RoutedEventArgs arg;
	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_TAB_SWITCHING;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.wParam = uOldKey;
	arg.lParam = uKey;
	LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		return lr;
	}

	SetULWNoRedraw(GetHostWnd(hWnd), TRUE);

	// 位置
	CIUIRect rcNewTaskPos;
	GetClientRect(hWnd, rcNewTaskPos);

	CIUIRect rcTaskWnd = rcNewTaskPos;

	// 选中或反选Radio Button
	std::vector<CPropertySheetInfo>::iterator it = pTb->m_pvSheets->begin();
	for (; it != pTb->m_pvSheets->end(); ++it)
	{
		CPropertySheetInfo *pInfo = &*it;

		if (!pInfo->m_bKeyIsRadioButton)
		{
			continue;
		}

		HWLWND hRadioButton = (HWLWND)pInfo->m_Key;

		if (!IsWindow(hRadioButton))
		{
			_ASSERT(FALSE);
		}
		else
		{
			if (hRadioButton == (HWLWND)uKey)
			{
				SendMessage(hRadioButton, BM_SETCHECK, BST_CHECKED, 0);

				//// If not set task window focus, set radio button focus
				//if (!bSetFocus)
				//{
				//	pRadioButton->SetFocus();
				//}
			}
			else
			{
				SendMessage(hRadioButton, BM_SETCHECK, BST_UNCHECKED, 0);
			}
		}
	}

	it = pTb->m_pvSheets->begin();
	for (; it != pTb->m_pvSheets->end(); ++it)
	{
		CPropertySheetInfo *pInfo = &*it;

		if (pInfo->m_Key == (void *)uKey)
		{
			// 如果没有创建，则先创建
			if (!IUI::IsWindow(pInfo->m_hPropertySheet))
			{
				CIUIProject *pProject = IUI::GetProject(hWnd);
				_ASSERT(pProject != NULL);
				CControlProp *pWindowProp = pProject->FindProjectItem(pInfo->m_strPropertySheetXml);

				if (pWindowProp == NULL)
				{
					return -1;
				}

				// 使用pWindowProp第0个孩子
				if (pWindowProp->GetChildCount() == 0)
				{
					return -1;
				}

				CControlProp *pChildProp = (CControlProp *)pWindowProp->GetChild(0);
				_ASSERT(pChildProp->IsControl());
				if ((CONTROL_TYPE)pWindowProp->m_eControlType == CT_UIWND)
				{
					pInfo->m_hPropertySheet = CreatePreDefineChildControl(pInfo->m_pUserHandler,
							NULL, pChildProp, hWnd, GetHostWnd(hWnd), (LPARAM)pInfo);

					// 忽略根控件在原始窗口中的边距
					SetControlMargin(pInfo->m_hPropertySheet, CIUIRect(0, 0, 0, 0));
				}
			}

			if (pInfo->m_hPropertySheet != NULL)
			{
				CanvasLayoutCtrl(pInfo->m_hPropertySheet);

				IUI::ShowWindow(pInfo->m_hPropertySheet, SW_SHOW);
				TabControl_RelayoutChild(hWnd);

				// 被切换到的属性页应该知道自己被显示了。
				IUI::SendMessage(pInfo->m_hPropertySheet, UE_PROPERTYSHEET_SHOW, uOldKey, uKey);
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		else if (pInfo->m_Key == (void *)uOldKey)
		{
			// 隐藏原来的属性页
			IUI::ShowWindow(pInfo->m_hPropertySheet, SW_HIDE);

			// 被隐藏的属性页应该知道自己被隐藏了。
			IUI::SendMessage(pInfo->m_hPropertySheet, UE_PROPERTYSHEET_HIDE, uOldKey, uKey);
		}
	}

	pTb->m_uCurKey = uKey;

	SetULWNoRedraw(GetHostWnd(hWnd), FALSE);
	Invalidate(hWnd);

	// After switch
	arg.RoutedEvent = UE_TAB_SWITCHED;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.wParam = uOldKey;
	arg.lParam = uKey;
	arg.lResult = 0;
	arg.bHandled = FALSE;
	lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		return lr;
	}

	return 0;
}

int IUI::TabControl_SwitchPropertySheet(HWLWND hWnd, HWLWND hRadioButton)
{
	CPropertySheetInfo *pInfo = TabControl_FindPropertySheet(hWnd, hRadioButton);
	if (pInfo == NULL)
	{
		return -1;
	}

	if (!pInfo->m_bKeyIsRadioButton)
	{
		return -2;
	}

	return TabControl_SwitchPropertySheet(hWnd, (UINT)hRadioButton);
}

int IUI::TabControl_DetachPropertySheetHandler(HWLWND hWnd, UINT uKey)
{
	CPropertySheetInfo *pTabInfo = TabControl_FindPropertySheet(hWnd, uKey);
	if (pTabInfo == NULL)
	{
		return -1;
	}

	RELEASE_HANDLER(pTabInfo->m_pUserHandler);

	// 因为属性页是在第一次显示时才创建，所以调用TabControl_DetachPropertySheetHandler
	// 时，属性页可能还未创建。
	if (pTabInfo->m_hPropertySheet != NULL)
	{
		pTabInfo->m_hPropertySheet->m_pUserBindWLWndHandler = NULL;
	}

	return 0;
}

int IUI::TabControl_DetachPropertySheetHandler(HWLWND hWnd, HWLWND hRadioButton)
{
	CPropertySheetInfo *pTabInfo = TabControl_FindPropertySheet(hWnd, hRadioButton);
	if (pTabInfo == NULL)
	{
		return -1;
	}

	RELEASE_HANDLER(pTabInfo->m_pUserHandler);

	// 因为属性页是在第一次显示时才创建，所以调用TabControl_DetachPropertySheetHandler
	// 时，属性页可能还未创建。
	if (pTabInfo->m_hPropertySheet != NULL)
	{
		pTabInfo->m_hPropertySheet->m_pUserBindWLWndHandler = NULL;
	}

	return 0;
}

CPropertySheetInfo *IUI::TabControl_FindPropertySheet(HWLWND hWnd, UINT uKey)
{
	TB *pTb = (TB *)GetMember(hWnd);

	CPropertySheetInfo *pInfo = NULL;

	std::vector<CPropertySheetInfo>::iterator it = pTb->m_pvSheets->begin();
	for (; it != pTb->m_pvSheets->end(); ++it)
	{
		CPropertySheetInfo *pPropertySheetInfo = &*it;

		if (!pPropertySheetInfo->m_bKeyIsRadioButton)
		{
			if (uKey == (UINT)pPropertySheetInfo->m_Key)
			{
				pInfo = pPropertySheetInfo;
				break;
			}
		}
	}

	return pInfo;
}

CPropertySheetInfo *IUI::TabControl_FindPropertySheet(HWLWND hWnd, HWLWND hRadioButton)
{
	TB *pTb = (TB *)GetMember(hWnd);

	CPropertySheetInfo *pInfo = NULL;

	std::vector<CPropertySheetInfo>::iterator it = pTb->m_pvSheets->begin();
	for (; it != pTb->m_pvSheets->end(); ++it)
	{
		CPropertySheetInfo *pPropertySheetInfo = &*it;

		if (pPropertySheetInfo->m_bKeyIsRadioButton)
		{
			if (pPropertySheetInfo->m_Key == hRadioButton)
			{
				pInfo = pPropertySheetInfo;
				break;
			}
		}
	}

	return pInfo;
}

int IUI::TabControl_RelayoutChild(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return -1;
	}

#ifdef DEBUG
	if (GetName(hWnd) == _T(""))
	{
		int n = 0;
	}
#endif // DEBUG

	CIUIRect rcTabClient;
	GetClientRect(hWnd, rcTabClient);

	HWLWND hChild = GetWindow(hWnd, GW_CHILD);
	for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDNEXT))
	{
		if (!IsWindowVisible(hChild))
		{
			continue;
		}

		CIUIRect rcNew = rcTabClient;

		CIUIRect rcChildOriginal;
		GetClientRect(hChild, rcChildOriginal);

		CIUIRect rcMargin;
		GetLayoutMargin(hChild, rcMargin);

		// Horizontal
		CONTROL_LAYOUT_HOR eHorLayoutMode = GetHorzLayoutMode(hChild);
		if (eHorLayoutMode == CLH_LEFT)
		{
			rcNew.left += rcMargin.left;
			rcNew.right = rcNew.left + rcChildOriginal.Width();
		}
		else if (eHorLayoutMode == CLH_FREE)
		{
			// 不变
		}
		else if (eHorLayoutMode == CLH_RIGHT)
		{
			rcNew.right -= rcMargin.right;
			rcNew.left = rcNew.right - rcChildOriginal.Width();
		}
		else if (eHorLayoutMode == CLH_CENTER)
		{
			rcNew.left = rcMargin.left + (rcTabClient.Width() - rcMargin.left - rcMargin.right - rcChildOriginal.Width()) / 2;
			rcNew.right = rcNew.left + rcChildOriginal.Width();
		}
		else if (eHorLayoutMode == CLH_FILL)
		{
			rcNew.left += rcMargin.left;
			rcNew.right -= rcMargin.right;
		}
		else
		{
			_ASSERT(FALSE);
		}

		// Vertical
		CONTROL_LAYOUT_VER eVerLayoutMode = GetVertLayoutMode(hChild);
		if (eVerLayoutMode == CLV_TOP)
		{
			rcNew.top += rcMargin.top;
			rcNew.bottom = rcNew.top + rcChildOriginal.Height();
		}
		else if (eVerLayoutMode == CLV_FREE)
		{
			// Keep
		}
		else if (eVerLayoutMode == CLV_BOTTOM)
		{
			rcNew.bottom -= rcMargin.bottom;
			rcNew.top = rcNew.bottom - rcChildOriginal.Height();
		}
		else if (eVerLayoutMode == CLV_CENTER)
		{
			rcNew.top = rcMargin.top + (rcTabClient.Height() - rcMargin.top - rcMargin.bottom - rcChildOriginal.Height()) / 2;
			rcNew.bottom = rcNew.top + rcChildOriginal.Height();
		}
		else if (eVerLayoutMode == CLV_FILL)
		{
			rcNew.top += rcMargin.top;
			rcNew.bottom -= rcMargin.bottom;
		}
		else
		{
			_ASSERT(FALSE);
		}

		MoveWindow(hChild, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), FALSE);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Core

int TabControl_OnPaint(TB *pTb, HDC hDC, LPCRECT lprcPaint)
{
	if (!IsRedraw(pTb->hwnd))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pTb->hwnd, rcClient);
	CIUIRect rcPaint;
	if (!::IntersectRect(&rcPaint, lprcPaint, rcClient))
	{
		return 2;
	}

	HDC hDCTarget = NULL;
	HDC hMemDC = NULL;
	HBITMAP hOldBmp = NULL;

	// 由于GDI不支持浮点数坐标，所以当使用RectF坐标时，采用GDI+绘制
	// RectF坐标常用于制作平滑移动的动画。 这时，先把控件内容画到一个32位alpha位图上
	// 最后再把位图绘制到指定坐标。
	HBITMAP hBmp = GetCachedAlphaBitmap(pTb->hwnd);
	if (IsUseRectF(pTb->hwnd))
	{
		hMemDC = ::CreateCompatibleDC(hDC);
		hDCTarget = hMemDC;

		void *pBits = NULL;
		if (hBmp != NULL)
		{
			DeleteObject(hBmp);
			hBmp = NULL;
		}

		hBmp = Create32BitsHBITMAP(rcClient.Width(), rcClient.Height(),
				32, BI_RGB, &pBits);
		SetCachedAlphaBitmap(pTb->hwnd, hBmp);
		hOldBmp = (HBITMAP)::SelectObject(hDCTarget, hBmp);
	}
	else
	{
		hDCTarget = hDC;
	}

	//
	// 1. 先绘制自己
	//
	BeforePaint(pTb->hwnd, hDCTarget);
	DrawBkColor(pTb->hwnd, hDCTarget, lprcPaint);
	DrawBorder(pTb->hwnd, hDCTarget);
	AfterPaint(pTb->hwnd, hDCTarget);

	if (IsUseRectF(pTb->hwnd))
	{
		if (hMemDC != NULL && hOldBmp != NULL)
		{
			// 生成m_hCacheAlphaBmp
			::SelectObject(hMemDC, hOldBmp);
			DeleteDC(hMemDC);

			// 因为GDI+绘制时用的坐标与GDI时不同，所以重设视口
			POINT pt;
			SetViewportOrgEx(hDC, 0, 0, &pt);

			Gdiplus::Graphics g(hDC);

			// 把HBITMAP转成Bitmap，Bitmap::FromHBITMAP有Bug
			Gdiplus::Bitmap *pBmp = CreateBitmapFromHBITMAP(hBmp);
			g.DrawImage(pBmp, *GetRectF(pTb->hwnd));
			::delete pBmp;
			pBmp = NULL;

			// 重设视口
			SetViewportOrgEx(hDC, pt.x, pt.y, NULL);
		}
	}
	else
	{
		if (IsCachedMode(pTb->hwnd) && hMemDC != NULL && hOldBmp != NULL)
		{
			BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
				hDCTarget, 0, 0, SRCCOPY);
			::SelectObject(hMemDC, hOldBmp);
			DeleteDC(hMemDC);
		}
	}

	return 0;
}

LRESULT WINAPI IUI::TabControl_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	TB *pTb = (TB *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pTb == NULL)
			{
				if (uMsg == WM_NCCREATE)
				{
					// WM_NCCREATE事件中只分配成员内存，不初始化
					// 初始化放到WM_CREATE中
					pTb = new TB;
					pTb->hwnd = hWnd;
					pTb->hwnd->ci.hwndParent = GetParent(hWnd);
					SetMember(hWnd, pTb);
					return 0;
				}

				return 0;
			}

			switch (uMsg)
			{
			case WM_DESTROY:
				pTb->Release();
				delete pTb;
				SetMember(hWnd, NULL);
				break;

			case WM_WINDOWPOSCHANGED:
				TabControl_RelayoutChild(hWnd);
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (uMsg)
			{
			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
				TabControl_OnPaint(pTb, HDC(wParam), LPCRECT(lParam));
			default:
				break;
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

int IUI::TabControl_BindStyle(HWLWND hWnd, const CTabControlProp *pCtrlProp)
{
	HWLWND_BindStyle(hWnd, pCtrlProp);

	CTabControlProp *pTabControlProp = (CTabControlProp *)pCtrlProp;

	return 0;
}

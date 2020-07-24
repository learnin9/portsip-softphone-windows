#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{
	DockPanel::DockPanel()
	{
		m_pvDockChildList = NULL;
	}

	DockPanel::~DockPanel()
	{
		delete m_pvDockChildList;
	}

	BOOL DockPanel::Create(
		__in CWLWndHandler *pUserHandler,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParentWnd,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_DOCKPANEL, NULL,
				dwStyle, rect, hParent, pParentWnd, lpszName);
	}

	int DockPanel::BindStyle(const CControlProp *pCtrlProp)
	{
		DockPanel_BindStyle(m_hWnd, (CDockPanelProp *)pCtrlProp);

		return 0;
	}

	int DockPanel::SetBkImage(
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

	int DockPanel::GetBkImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameN, CIUIString *pstrImageNameD) const
	{
		_ASSERT(IsWindow());

		return IUIGetControlBkImage2(m_hWnd, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameN, pstrImageNameD);
	}

	int DockPanel::SetLastChildFill(BOOL bFill)
	{
		_ASSERT(IsWindow());

		return IUI::SendMessage(m_hWnd, DPM_SET_LAST_CHILD_FILL, 0, bFill);
	}

	BOOL DockPanel::IsLastChildFill() const
	{
		_ASSERT(IsWindow());

		return IUI::SendMessage(m_hWnd, DPM_IS_LAST_CHILD_FILL, 0, 0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Core

	int DockPanel_OnPaint(_DOCK *pDock, HDC hDC, LPCRECT lprcPaint)
	{
		if (!IsRedraw(pDock->hwnd))
		{
			return 1;
		}

		CIUIRect rcClient;
		GetClientRect(pDock->hwnd, rcClient);
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
		HBITMAP hBmp = GetCachedAlphaBitmap(pDock->hwnd);
		if (IsUseRectF(pDock->hwnd))
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
			SetCachedAlphaBitmap(pDock->hwnd, hBmp);
			hOldBmp = (HBITMAP)::SelectObject(hDCTarget, hBmp);
		}
		else
		{
			hDCTarget = hDC;
		}

		//
		// 1. 先绘制自己
		//
		BeforePaint(pDock->hwnd, hDCTarget);
		DrawBkColor(pDock->hwnd, hDCTarget, lprcPaint);
		Label_DrawBkImage(pDock, hDCTarget, NULL);
		DrawBorder(pDock->hwnd, hDCTarget);
		AfterPaint(pDock->hwnd, hDCTarget);

		if (IsUseRectF(pDock->hwnd))
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
				g.DrawImage(pBmp, *GetRectF(pDock->hwnd));
				::delete pBmp;
				pBmp = NULL;

				// 重设视口
				SetViewportOrgEx(hDC, pt.x, pt.y, NULL);
			}
		}
		else
		{
			if (IsCachedMode(pDock->hwnd) && hMemDC != NULL && hOldBmp != NULL)
			{
				BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
					hDCTarget, 0, 0, SRCCOPY);
				::SelectObject(hMemDC, hOldBmp);
				DeleteDC(hMemDC);
			}
		}

		return 0;
	}

	int DockPanelLayoutElement(
		BOOL bLastChildFill,
		LPCRECT lprcLast, // DockPanel当前剩余空间（可用空间）
		HWLWND hCtrl,
		__out CIUIRect &rcNew,
		__out LPRECT lprcRemain) // 返回布局完hCtrl后的剩余空间

	{
		if (NULL == lprcLast
			|| !IsWindow(hCtrl)
			|| NULL == lprcRemain)
		{
			return -1;
		}

		if (!IsWindowVisible(hCtrl))
		{
			return 1;
		}

		//
		*lprcRemain = *lprcLast;

		//
		BOOL bLastChild = (NULL == hCtrl->spwndNext);

		// 只有Fill对齐模式才会改变控件的尺寸，而Fill对齐时，也不需要控件的原始宽高
		// 所以，我们不需要保存控件的原始坐标
		CIUIRect rcOld;
		GetWindowRectToParent(hCtrl, rcOld);

		CIUIRect rcControlMargin;
		GetControlMargin(hCtrl, rcControlMargin);

		CIUIRect rcLayoutMargin;
		GetLayoutMargin(hCtrl, rcLayoutMargin);

		DOCK eDockProperty = GetDockProperty(hCtrl);

		// 如果控件是DockPanel的孩子，控件同时设置了Layout Mode属性和DockProperty属性，
		// 并且这两种对齐方式如果有冲突，那么IUI是怎么放置呢？IUI的处理方式遵从WPF，
		// 规则如下：
		// 1.最后一个控件受LastChildFill属性的影响，当LastChildFill为true时，
		// 按Layout Mode的设置对齐; 当为false时，按DockProperty的设置对齐;
		// 2.其它前面的控件受DockProperty的影响，与Layout Mode设置无关。
		if (bLastChild)
		{
			if (bLastChildFill)
			{
// 				// 忽略控件在界面编辑器中生成的Control Margin。
				rcNew = *lprcLast;

				memset(lprcRemain, 0, sizeof(RECT));
			}
			else
			{
				goto DOCKLAYOUT;
			}
		}
		else
		{
DOCKLAYOUT:
			switch (eDockProperty)
			{
			case DOCK_LEFT:
				rcNew.left = lprcLast->left + rcLayoutMargin.left + rcControlMargin.left;
				rcNew.top = lprcLast->top + rcLayoutMargin.top + rcControlMargin.top;
				rcNew.right = rcNew.left + rcOld.Width();
				rcNew.bottom = lprcLast->bottom - rcLayoutMargin.bottom - rcControlMargin.bottom;

				lprcRemain->left = rcNew.right;
				break;

			case DOCK_TOP:
				rcNew.left = lprcLast->left + rcLayoutMargin.left + rcControlMargin.left;
				rcNew.right = lprcLast->right - rcLayoutMargin.right - rcControlMargin.right;
				rcNew.top = lprcLast->top + rcLayoutMargin.top + rcControlMargin.top;
				rcNew.bottom = rcNew.top + rcOld.Height();

				lprcRemain->top = rcNew.bottom;
				break;

			case DOCK_RIGHT:
				rcNew.right = lprcLast->right - rcLayoutMargin.right - rcControlMargin.right;
				rcNew.left = rcNew.right - rcOld.Width();
				rcNew.top = lprcLast->top + rcLayoutMargin.top + rcControlMargin.top;
				rcNew.bottom = lprcLast->bottom - rcLayoutMargin.bottom - rcControlMargin.bottom;

				lprcRemain->right = rcNew.left;
				break;

			case DOCK_BOTTOM:
				rcNew.left = lprcLast->left + rcLayoutMargin.left + rcControlMargin.left;
				rcNew.right = lprcLast->right - rcLayoutMargin.right - rcControlMargin.right;
				rcNew.bottom = lprcLast->bottom - rcLayoutMargin.bottom - rcControlMargin.bottom;
				rcNew.top = rcNew.bottom - rcOld.Height();

				lprcRemain->bottom = rcNew.top;
				break;

			default:
				_ASSERT(FALSE);
				break;
			}
		}

		return 0;
	}

	LRESULT WINAPI DockPanel_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
	{
		_DOCK *pDock = (_DOCK *)GetMember(hWnd);

		UINT uMsg = pe->RoutedEvent;
		WPARAM wParam = pe->wParam;
		LPARAM lParam = pe->lParam;

		if (pe->hOriginalSender == hWnd)
		{
			if (pe->eRoutingStrategy == RS_BUBBLE)
			{
				if (pDock == NULL)
				{
					if (uMsg == WM_NCCREATE)
					{
						// WM_NCCREATE事件中只分配成员内存，不初始化
						// 初始化放到WM_CREATE中
						pDock = new _DOCK;
						pDock->hwnd = hWnd;
						pDock->hwnd->ci.hwndParent = GetParent(hWnd);
						SetMember(hWnd, pDock);
						return 0;
					}

					return 0;
				}

				switch (uMsg)
				{
				case WM_DESTROY:
					pDock->Release();
					delete pDock;
					SetMember(hWnd, NULL);
					break;

				case WM_WINDOWPOSCHANGED:
				{
					CIUIRect rcLast;
					GetClientRect(hWnd, rcLast);

					HWLWND hChild = GetWindow(hWnd, GW_CHILD);
					for (; NULL != hChild; hChild = GetWindow(hChild, GW_HWNDNEXT))
					{
						CIUIRect rcNew;
						DockPanelLayoutElement(pDock->m_bLastChildFill,
							rcLast, hChild, rcNew, rcLast);
						MoveWindow(hChild,
							rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), FALSE);
					}
				}
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
					DockPanel_OnPaint(pDock, HDC(wParam), LPCRECT(lParam));
					break;

				case DPM_SET_LAST_CHILD_FILL:
					pDock->m_bLastChildFill = lParam;
					break;

				case DPM_IS_LAST_CHILD_FILL:
					return pDock->m_bLastChildFill;

				default:
					break;
				}
			}
		}

		// DockPanel就是Label
		return Label_WndProc(hWnd, pe);
	}

	////////////////////////////////////////////////////////////////////
	// DockPanel API
	int IUI::DockPanel_BindStyle(HWLWND hWnd, const CDockPanelProp *pCtrlProp)
	{
		Label_BindStyle(hWnd, pCtrlProp);

		CDockPanelProp *pProp = (CDockPanelProp *)pCtrlProp;

		DockPanel *pDockPanel = (DockPanel *)CWLWnd::FromHandle(hWnd);

		pDockPanel->SetLastChildFill((bool)pProp->m_bLastChildFill);

		return 0;
	}

}

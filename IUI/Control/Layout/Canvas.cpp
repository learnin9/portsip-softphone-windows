#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{
	Canvas::Canvas()
	{
		m_pvCanvasChildList = NULL;
	}

	Canvas::~Canvas()
	{
		delete m_pvCanvasChildList;
	}

	BOOL Canvas::Create(
		__in CWLWndHandler *pUserHandler,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParentWnd,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_CANVAS, NULL,
				dwStyle, rect, hParent, pParentWnd, lpszName);
	}

	int Canvas::BindStyle(const CControlProp *pCtrlProp)
	{
		Canvas_BindStyle(m_hWnd, (CCanvasProp *)pCtrlProp);

		return 0;
	}

	int Canvas::SetBkImage(
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

	int Canvas::GetBkImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameN, CIUIString *pstrImageNameD) const
	{
		_ASSERT(IsWindow());

		return IUIGetControlBkImage2(m_hWnd, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameN, pstrImageNameD);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Core

	int Canvas_OnPaint(_LABEL *pLb, HDC hDC, LPCRECT lprcPaint)
	{
		if (!IsRedraw(pLb->hwnd))
		{
			return 1;
		}

		CIUIRect rcClient;
		GetClientRect(pLb->hwnd, rcClient);
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
		HBITMAP hBmp = GetCachedAlphaBitmap(pLb->hwnd);
		if (IsUseRectF(pLb->hwnd))
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
			SetCachedAlphaBitmap(pLb->hwnd, hBmp);
			hOldBmp = (HBITMAP)::SelectObject(hDCTarget, hBmp);
		}
		else
		{
			hDCTarget = hDC;
		}

		//
		// 1. 先绘制自己
		//
		BeforePaint(pLb->hwnd, hDCTarget);
		DrawBkColor(pLb->hwnd, hDCTarget, lprcPaint);
		Label_DrawBkImage(pLb, hDCTarget, NULL);
		DrawBorder(pLb->hwnd, hDCTarget);
		AfterPaint(pLb->hwnd, hDCTarget);

		if (IsUseRectF(pLb->hwnd))
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
				g.DrawImage(pBmp, *GetRectF(pLb->hwnd));
				::delete pBmp;
				pBmp = NULL;

				// 重设视口
				SetViewportOrgEx(hDC, pt.x, pt.y, NULL);
			}
		}
		else
		{
			if (IsCachedMode(pLb->hwnd) && hMemDC != NULL && hOldBmp != NULL)
			{
				BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
					hDCTarget, 0, 0, SRCCOPY);
				::SelectObject(hMemDC, hOldBmp);
				DeleteDC(hMemDC);
			}
		}

		return 0;
	}

	LRESULT WINAPI Canvas_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
	{
		// Canvas是从Label派生的，所以Canvas的私有成员，至少要包含_LABEL
		// 因为Canvas::BindStyle会调用Label::BindStyle，而Label::BindStyle内部，
		// 会把m_pMember转成_LABLE *使用。
		_LABEL *pLb = (_LABEL *)GetMember(hWnd);

		UINT uMsg = pe->RoutedEvent;
		WPARAM wParam = pe->wParam;
		LPARAM lParam = pe->lParam;

		if (pe->hOriginalSender == hWnd)
		{
			if (pe->eRoutingStrategy == RS_BUBBLE)
			{
				if (pLb == NULL)
				{
					if (uMsg == WM_NCCREATE)
					{
						// WM_NCCREATE事件中只分配成员内存，不初始化
						// 初始化放到WM_CREATE中
						pLb = new _LABEL;
						pLb->hwnd = hWnd;
						pLb->hwnd->ci.hwndParent = GetParent(hWnd);
						SetMember(hWnd, pLb);
						return 0;
					}

					return 0;
				}

				switch (uMsg)
				{
				case WM_DESTROY:
					pLb->Release();
					delete pLb;
					SetMember(hWnd, NULL);
					break;

				// 执行IUI::DefEventHandler中的WM_WINDOWPOSCHANGED，
				// HWLWND本身默认就是Canvas布局
				case WM_WINDOWPOSCHANGED:
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
					Canvas_OnPaint(pLb, HDC(wParam), LPCRECT(lParam));
					break;

				default:
					break;
				}
			}
		}

		// Canvas就是Label
		return Label_WndProc(hWnd, pe);
	}

	////////////////////////////////////////////////////////////////////
	// Canvas API
	int IUI::Canvas_BindStyle(HWLWND hWnd, const CCanvasProp *pCtrlProp)
	{
		Label_BindStyle(hWnd, pCtrlProp);

		CCanvasProp *pProp = (CCanvasProp *)pCtrlProp;

		return 0;
	}

}

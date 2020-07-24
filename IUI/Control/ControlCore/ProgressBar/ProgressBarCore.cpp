#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


COLORREF g_clrHighlight	= RGB(0, 0, 255);
#define IDT_PROGRESSBAR_GIF			USER_TIMER_MAXIMUM - 1

typedef struct PRO_DATA
{
	PRO_DATA()
		: hwnd(NULL)
		, dwStyle(0)
		, iLow(0)
		, iHigh(0)
		, iPos(0)
		, iStep(0)
		, hfont(NULL)
		, _clrBk(RGB(255, 255, 255))
		, _clrBar(RGB(0, 0, 255))
		, m_pGifHelper(NULL)
		, m_bGifAnimationBegun(FALSE)
		, m_pThumb(NULL)
	{
		for (int i = 0; i < COMBINEIMAGESIZE2; ++i)
		{
			m_himgBk[i] = NULL;
		}
	}

	~PRO_DATA()
	{
		hwnd->ci.Release();

		for (int i = 0; i < COMBINEIMAGESIZE2; ++i)
		{
			ImageManager::ReleaseIUIImage(m_himgBk[i]);
			m_himgBk[i] = NULL;
		}

		if (m_pGifHelper != NULL)
		{
			delete m_pGifHelper;
			m_pGifHelper = NULL;
		}
		m_bGifAnimationBegun = FALSE;

		if (m_pThumb != NULL)
		{
			delete m_pThumb;
			m_pThumb = NULL;
		}
	}

	HWLWND hwnd;
	DWORD dwStyle;
	int iLow;	// 范围的最小值
	int iHigh;	// 范围的最大值
	int iPos;	// 当前值
	int iStep;
	HFONT hfont;
	COLORREF _clrBk;
	COLORREF _clrBar;

	// IUI Add
	HIUIIMAGE m_himgBk[COMBINEIMAGESIZE2];
	GifHelper *m_pGifHelper;
	// 如果背景是Gif，Gif定时器是否已经启动，如果已启动，将不再启动。
	BOOL m_bGifAnimationBegun;
	SProgressBarThumbMember *m_pThumb;
} PRO_DATA, NEAR *PPRO_DATA;    // ppd

int ProgressBar_DrawBkImage(PPRO_DATA ppd, HDC hDC, LPCRECT lprcDest);
int ProgressBar_DrawThumb(PPRO_DATA pPrg, HDC hDC);

int NEAR PASCAL UpdatePosition(PPRO_DATA ppd, int iNewPos, BOOL bAllowWrap)
{
	int iPosOrg = ppd->iPos;
	UINT uRedraw = RDW_INVALIDATE | RDW_UPDATENOW;

	if (ppd->iLow == ppd->iHigh)
	{
		iNewPos = ppd->iLow;
	}

	if (iNewPos < ppd->iLow)
	{
		if (!bAllowWrap)
		{
			iNewPos = ppd->iLow;
		}
		else
		{
			iNewPos = ppd->iHigh - ((ppd->iLow - iNewPos) % (ppd->iHigh - ppd->iLow));
			// wrap, erase old stuff too
			uRedraw |= RDW_ERASE;
		}
	}
	else if (iNewPos > ppd->iHigh)
	{
		if (!bAllowWrap)
		{
			iNewPos = ppd->iHigh;
		}
		else
		{
			iNewPos = ppd->iLow + ((iNewPos - ppd->iHigh) % (ppd->iHigh - ppd->iLow));
			// wrap, erase old stuff too
			uRedraw |= RDW_ERASE;
		}
	}

	// if moving backwards, erase old version
	if (iNewPos < iPosOrg)
	{
		uRedraw |= RDW_ERASE;
	}

	if (iNewPos != ppd->iPos)
	{
		ppd->iPos = iNewPos;
		// paint, maybe erase if we wrapped
		InvalidateRect(ppd->hwnd, NULL);

		MyNotifyWinEvent(EVENT_OBJECT_VALUECHANGE, GetHostWnd(ppd->hwnd), OBJID_CLIENT, 0);
	}
	return iPosOrg;
}

#define HIGHBG g_clrHighlight
#define HIGHFG g_clrHighlightText
#define LOWBG g_clrBtnFace
#define LOWFG g_clrBtnText

void NEAR PASCAL ProPaint(PPRO_DATA ppd, HDC hdc, const RECT &rcClip)
{
	if (ppd == NULL || hdc == NULL)
	{
		return;
	}

	if (!IsRedraw(ppd->hwnd))
	{
		return;
	}

	CIUIRect rcClient;
	GetClientRect(ppd->hwnd, rcClient);
	CIUIRect rcPaint;
	if (!::IntersectRect(&rcPaint, &rcClip, rcClient))
	{
		return;
	}

	BeforePaint(ppd->hwnd, hdc);
	DrawBkColor(ppd->hwnd, hdc, rcPaint);
	ProgressBar_DrawBkImage(ppd, hdc, NULL);
	ProgressBar_DrawThumb(ppd, hdc);

	//int x;	// 水平进度条进度的宽度
	//int dxSpace;	// 进度块间距
	//int dxBlock;	// 如果进度是一块一块的，定义水平进度每块的宽度（进度条高度的2/3）
	//int nBlocks, i;
	//RECT rc;	// 进度条去除边框后的坐标
	//int iStart;	// 水平进度条左起点坐标
	//int iEnd;	// 水平进度条右终点坐标

	////  give 1 pixel around the bar
	//InflateRect(&rcClient, -1, -1);
	//rc = rcClient;

	//if (ppd->dwStyle & PBS_VERTICAL)
	//{
	//	iStart = rc.top;
	//	iEnd = rc.bottom;
	//	dxBlock = (rc.right - rc.left) * 2 / 3;
	//}
	//else
	//{
	//	iStart = rc.left;
	//	iEnd = rc.right;
	//	dxBlock = (rc.bottom - rc.top) * 2 / 3;
	//}

	//x = MulDiv(iEnd - iStart, ppd->iPos - ppd->iLow, ppd->iHigh - ppd->iLow);

	//dxSpace = 2;
	//if (dxBlock == 0)
	//{
	//	dxBlock = 1;    // avoid div by zero
	//}

	//if (ppd->dwStyle & PBS_SMOOTH)
	//{
	//	dxBlock = 1;
	//	dxSpace = 0;
	//}

	//nBlocks = (x + (dxBlock + dxSpace) - 1) / (dxBlock + dxSpace); // round up

	//for (i = 0; i < nBlocks; i++)
	//{
	//	if (ppd->dwStyle & PBS_VERTICAL)
	//	{
	//		rc.top = rc.bottom - dxBlock;

	//		// are we past the end?
	//		if (rc.bottom <= rcClient.top)
	//		{
	//			break;
	//		}

	//		if (rc.top <= rcClient.top)
	//		{
	//			rc.top = rcClient.top + 1;
	//		}

	//	}
	//	else
	//	{
	//		rc.right = rc.left + dxBlock;

	//		// are we past the end?
	//		if (rc.left >= rcClient.right)
	//		{
	//			break;
	//		}

	//		if (rc.right >= rcClient.right)
	//		{
	//			rc.right = rcClient.right - 1;
	//		}
	//	}

	//	if (ppd->_clrBar == CLR_DEFAULT)
	//	{
	//		FillRectClr(hdc, &rc, g_clrHighlight);
	//	}
	//	else
	//	{
	//		FillRectClr(hdc, &rc, ppd->_clrBar);
	//	}


	//	if (ppd->dwStyle & PBS_VERTICAL)
	//	{
	//		rc.bottom = rc.top - dxSpace;
	//	}
	//	else
	//	{
	//		rc.left = rc.right + dxSpace;
	//	}
	//}

	AfterPaint(ppd->hwnd, hdc);
}

LRESULT NEAR PASCAL Progress_OnCreate(PPRO_DATA ppd, LPCREATESTRUCT pcs)
{
	// remove ugly double 3d edge
	ppd->iHigh = 100;        // default to 0-100
	ppd->iStep = 10;        // default to step of 10
	ppd->dwStyle = pcs->style;
	ppd->_clrBk = CLR_DEFAULT;
	ppd->_clrBar = CLR_DEFAULT;

#ifdef DEBUG
	if (GetAsyncKeyState(VK_SHIFT) < 0 &&
		GetAsyncKeyState(VK_CONTROL) < 0)
	{
		ppd->dwStyle |= PBS_SMOOTH;
	}

	if (GetAsyncKeyState(VK_SHIFT) < 0 &&
		GetAsyncKeyState(VK_MENU) < 0)
	{
		ppd->dwStyle |= PBS_VERTICAL;
		SetWindowPos(ppd->hwnd, NULL, 0, 0, 40, 100, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
#endif

	return 0;
}

int ProgressBar_DrawThumbColor(PPRO_DATA pPrg, HDC hDC, LPCRECT lprcThumb)
{
	if (hDC == NULL || lprcThumb == NULL)
	{
		return -1;
	}

	// 给父窗口处理的机会
	RoutedEventArgs arg;
	arg.hSender = pPrg->hwnd;
	arg.hOriginalSender = pPrg->hwnd;
	arg.RoutedEvent = UE_DRAWTHUMBCOLOR;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.wParam = (WPARAM)hDC;
	arg.lParam = (LPARAM)lprcThumb;
	LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		return 0;
	}

	if (!pPrg->m_pThumb->m_bDrawThumbColor)
	{
		return 1;
	}

	COLORREF crStart = pPrg->m_pThumb->m_crThumbColorStart;
	COLORREF crEnd = pPrg->m_pThumb->m_crThumbColorEnd;

	// 纯色
	if (crEnd == -1 || crStart == crEnd)
	{
		FillSolidRect(hDC, lprcThumb, crStart, IsUpdateLayeredWindow(GetHostWnd(pPrg->hwnd)));
	}
	else
	{
		TRIVERTEX vert[2] = { 0 };
		vert[0].x = lprcThumb->left;
		vert[0].y = lprcThumb->top;
		vert[0].Red = GetRValue(crStart) << 8;
		vert[0].Green = GetGValue(crStart) << 8;
		vert[0].Blue = GetBValue(crStart) << 8;
		vert[0].Alpha = 0x0000;

		vert[1].x = lprcThumb->right;
		vert[1].y = lprcThumb->bottom;
		vert[1].Red = GetRValue(crEnd) << 8;
		vert[1].Green = GetGValue(crEnd) << 8;
		vert[1].Blue = GetBValue(crEnd) << 8;
		vert[1].Alpha = 0x0000;

		GRADIENT_RECT gRect;
		gRect.UpperLeft = 0;
		gRect.LowerRight = 1;
		::GradientFill(hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
	}

	return 0;
}

// 图像分：Stretch、9宫格、裁剪、停靠四种缩放方式
int ProgressBar_DrawThumbImage(PPRO_DATA pPrg, HDC hDC, LPCRECT lprcThumb)
{
	if (hDC == NULL || lprcThumb == NULL)
	{
		return -1;
	}

	// 给父窗口处理的机会
	RoutedEventArgs arg;
	arg.hSender = pPrg->hwnd;
	arg.hOriginalSender = pPrg->hwnd;
	arg.RoutedEvent = UE_DRAWTHUMBIMAGE;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.wParam = (WPARAM)hDC;
	arg.lParam = (LPARAM)lprcThumb;
	LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		return 0;
	}

	if (pPrg->m_pThumb->m_bCombineThumbImage)
	{

	}
	else
	{
		CIUIString strThumbImage;
		if (IsWindowEnabled(pPrg->hwnd))
		{
			strThumbImage = pPrg->m_pThumb->m_strCombineThumbImage;
		}
		else
		{
			strThumbImage = pPrg->m_pThumb->m_strDisabledThumbImage;
		}

		if (!strThumbImage.IsEmpty())
		{
			HIUIIMAGE hImage = CreateIUIImage(GetProject(pPrg->hwnd), strThumbImage);
			CIUIRect rcResize(2, 2, 1, 1);
			IUIDrawImage(hDC, lprcThumb, hImage, IRM_9GRID, rcResize);
			ReleaseIUIImage(hImage);
		}
	}

	return 0;
}

int ProgressBar_DrawBkImage(PPRO_DATA ppd, HDC hDC, LPCRECT lprcDest)
{
	CIUIRect rcDest;
	if (lprcDest == NULL)
	{
		GetClientRect(ppd->hwnd, rcDest);
	}
	else
	{
		rcDest = lprcDest;
	}

	// 给父控件一个自绘的机会
	RoutedEventArgs arg;
	arg.hSender = ppd->hwnd;
	arg.hOriginalSender = ppd->hwnd;
	arg.RoutedEvent = UE_DRAW_BK_IMAGE;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.wParam = (WPARAM)hDC;
	arg.lParam = (LPARAM)(LPCRECT)rcDest;
	LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		return 0;
	}

	int nStateIndex = 0;
	if (!IsWindowEnabled(ppd->hwnd))
	{
		nStateIndex = 1;
	}

	if (ppd->hwnd->ci.m_bCombineBkImage)
	{
		IUIPartDrawImage(hDC, rcDest,
			ppd->hwnd->ci.m_himgCombineBk, ppd->hwnd->ci.m_eBkImageResizeMode,
			ppd->hwnd->ci.m_rcBkImage9GridResizeArg, COMBINEIMAGESIZE2, nStateIndex);
	}
	else
	{
		// 如果设置的背景图是Gif，则启动定时器
		HIUIIMAGE hImage = ppd->m_himgBk[nStateIndex];
		hImage->SafeLoadSavedImage();
		if (hImage->IsGif() && !ppd->m_bGifAnimationBegun)
		{
			if (ppd->hwnd->ci.m_eBkImageResizeMode == IRM_CENTER
				|| ppd->hwnd->ci.m_eBkImageResizeMode == IRM_STRETCH
				|| ppd->hwnd->ci.m_eBkImageResizeMode == IRM_STRETCH_HIGH_QUALITY
				|| ppd->hwnd->ci.m_eBkImageResizeMode == IRM_TILE
				|| ppd->hwnd->ci.m_eBkImageResizeMode == IRM_KEEPRATE)
			{
				LONG *plFrameDelay = NULL;
				UINT nFrameCount = 0;
				hImage->GetGifFrameDelay(&plFrameDelay, &nFrameCount);

				if (nFrameCount > 0)
				{
					if (ppd->m_pGifHelper == NULL)
					{
						ppd->m_pGifHelper = new GifHelper;
					}

					// 在循环次数有限的gif播放结束后，ppd->m_bGifAnimationBegun
					// 也不需要置FALSE，因为一旦置FALSE，下次绘制Label的时候，
					// 动画又会重新播放，动画就由有限次播放变成无限循环了。
					ppd->m_bGifAnimationBegun = TRUE;

					// 启动第一帧
					SetTimer(ppd->hwnd, IDT_PROGRESSBAR_GIF, *plFrameDelay, NULL);
				}
			}
		}

		IUIDrawImage(hDC, rcDest, hImage,
			ppd->hwnd->ci.m_eBkImageResizeMode, ppd->hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;
}

int ProgressBar_DrawThumb(PPRO_DATA pPrg, HDC hDC)
{
	if (hDC == NULL)
	{
		return -1;
	}

	if (pPrg->iHigh <= pPrg->iLow)
	{
	 	pPrg->iHigh = pPrg->iLow + 1;
	}
	
	if (pPrg->iPos > pPrg->iHigh)
	{
	 	pPrg->iPos = pPrg->iHigh;
	}
	
	if (pPrg->iPos < pPrg->iLow)
	{
	 	pPrg->iPos = pPrg->iLow;
	}
	
	if (pPrg->iHigh == pPrg->iLow)
	{
	 	_ASSERT(FALSE);
	 	return -2;
	}
	
	CIUIRect rcThumb;
	
	CIUIRect rcItemThis;
	GetClientRect(pPrg->hwnd, rcItemThis);
	if (pPrg->dwStyle & PBS_VERTICAL)
	{
	 	rcThumb.top = rcItemThis.Height() * (pPrg->iHigh - pPrg->iPos) / (pPrg->iHigh - pPrg->iLow);
	 	rcThumb.bottom = rcItemThis.Height();
	 	rcThumb.right = rcItemThis.Width();
	}
	else
	{
	 	rcThumb.right = rcItemThis.Width() * (pPrg->iPos - pPrg->iLow) / (pPrg->iHigh - pPrg->iLow);
	 	rcThumb.bottom = rcItemThis.Height();
	}
	
	// Thumb绘制时分两层，下层是颜色，上层是图片
	// 颜色分纯色、渐变色两种
	ProgressBar_DrawThumbColor(pPrg, hDC, rcThumb);
	
	// 绘制Thumb image
	// 图像分：Stretch、9宫格、裁剪、停靠四种缩放方式
	ProgressBar_DrawThumbImage(pPrg, hDC, rcThumb);

	return 0;
}

LRESULT WINAPI IUI::ProgressBar_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	PPRO_DATA ppd = (PPRO_DATA)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (ppd == NULL)
			{
				if (uMsg == WM_NCCREATE)
				{
					// WM_NCCREATE事件中只分配成员内存，不初始化
					// 初始化放到WM_CREATE中
					if (ppd == NULL)
					{
						ppd = new PRO_DATA;
						ppd->hwnd = hWnd;
						ppd->hwnd->ci.hwndParent = GetParent(hWnd);
						ppd->m_pThumb = new SProgressBarThumbMember;

						SetMember(hWnd, ppd);

						return 0;
					}
				}

				return 0;
			}

			switch (uMsg)
			{
			case WM_CREATE:
				return Progress_OnCreate(ppd, (LPCREATESTRUCT)lParam);

			case WM_DESTROY:
				//ppd->Release();
				delete ppd;
				SetMember(hWnd, NULL);
				return 0;

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
			case WM_SYSCOLORCHANGE:
				//InitGlobalColors();
				InvalidateRect(ppd->hwnd, NULL);
				break;

			case WM_SETFONT:
			{
				HFONT hFont = ppd->hfont;
				ppd->hfont = (HFONT)wParam;
				return (LRESULT)(UINT_PTR)hFont;
			}

			case WM_GETFONT:
				return (LRESULT)(UINT_PTR)ppd->hfont;

			case PBM_GETPOS:
				return ppd->iPos;

			case PBM_GETRANGE:
				if (lParam)
				{
					PPBRANGE ppb = (PPBRANGE)lParam;
					ppb->iLow = ppd->iLow;
					ppb->iHigh = ppd->iHigh;
				}
				return (wParam ? ppd->iLow : ppd->iHigh);

			case PBM_SETRANGE:
				// win95 compat
				wParam = LOWORD(lParam);
				lParam = HIWORD(lParam);
			// fall through

			case PBM_SETRANGE32:
			{
				LRESULT lret = MAKELONG(ppd->iLow, ppd->iHigh);

				// only repaint if something actually changed
				if ((int)wParam != ppd->iLow || (int)lParam != ppd->iHigh)
				{
					ppd->iHigh = (int)lParam;
					ppd->iLow  = (int)wParam;
					// force an invalidation/erase but don't redraw yet
					InvalidateRect(ppd->hwnd, NULL);
					UpdatePosition(ppd, ppd->iPos, FALSE);
				}
				return lret;
			}

			case PBM_SETPOS:
				return (LRESULT)UpdatePosition(ppd, (int) wParam, FALSE);

			case PBM_SETSTEP:
			{
				int x = ppd->iStep;
				ppd->iStep = (int)wParam;
				return (LRESULT)x;
			}

			case PBM_STEPIT:
				return (LRESULT)UpdatePosition(ppd, ppd->iStep + ppd->iPos, TRUE);

			case PBM_DELTAPOS:
				return (LRESULT)UpdatePosition(ppd, ppd->iPos + (int)wParam, FALSE);

			case PBM_SETBKCOLOR:
			{
				COLORREF clr = ppd->_clrBk;
				ppd->_clrBk = (COLORREF)lParam;
				InvalidateRect(ppd->hwnd, NULL);
				return clr;
			}

			case PBM_SETBARCOLOR:
			{
				COLORREF clr = ppd->_clrBar;
				ppd->_clrBar = (COLORREF)lParam;
				InvalidateRect(ppd->hwnd, NULL);
				return clr;
			}

			case PBM_SET_DRAW_THUMB_COLOR:
			{
				if (NULL != ppd->m_pThumb)
				{
					BOOL bDrawThumbColor = ppd->m_pThumb->m_bDrawThumbColor;
					ppd->m_pThumb->m_bDrawThumbColor = (BOOL)wParam;
					InvalidateRect(ppd->hwnd, NULL);
					return bDrawThumbColor;
				}
			}
			return FALSE;

			case PBM_GET_DRAW_THUMB_COLOR:
				if (NULL != ppd->m_pThumb)
				{
					return (LRESULT)ppd->m_pThumb->m_bDrawThumbColor;
				}
				return FALSE;

			case PBM_SET_THUMB_BK_COLOR:
				if (NULL != ppd->m_pThumb)
				{
					ppd->m_pThumb->m_crThumbColorStart = (COLORREF)wParam;
					ppd->m_pThumb->m_crThumbColorEnd = (COLORREF)lParam;

					InvalidateRect(ppd->hwnd, NULL);
					return 0;
				}
				return -1;

			case PBM_GET_THUMB_BK_COLOR:
				if (NULL != ppd->m_pThumb)
				{
					COLORREF *pcrStart = (COLORREF *)wParam;
					if (NULL != pcrStart)
					{
						*pcrStart = ppd->m_pThumb->m_crThumbColorStart;
					}

					COLORREF *pcrEnd = (COLORREF *)lParam;
					if (NULL != pcrEnd)
					{
						*pcrEnd = ppd->m_pThumb->m_crThumbColorEnd;
					}

					return 0;
				}
				return -1;

			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
				ProPaint(ppd, (HDC)wParam, *LPCRECT(lParam));
				return 0;

			case WLM_BIND_STYLE:
				ProgressBar_BindStyle(hWnd, (CProgressBarProp *)lParam);
				return 0;

			default:
				break;
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

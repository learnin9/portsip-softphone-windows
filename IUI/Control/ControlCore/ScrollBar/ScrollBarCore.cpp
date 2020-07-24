#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


void SBCtlSetup(SBWND *psbwnd);
void xxxTrackBox(HWLWND pwnd, UINT message, WPARAM wParam, LPARAM lParam, PSBCALC pSBCalc);
void xxxTrackThumb(HWLWND pwnd, UINT message, WPARAM wParam, LPARAM lParam, PSBCALC pSBCalc);
int SB_Draw(SBWND *pSb, HDC hDC, LPCRECT lprcClip);

int g_dtScroll = 400;

int SB_SetPSBTRACK(HWLWND hScrollBar, PSBTRACK pSBTrack)
{
	SetProp(hScrollBar, _T("PSBTRACK"), pSBTrack);
	return 0;
}

PSBTRACK SB_GetPSBTRACK(HWLWND hScrollBar)
{
	PSBTRACK pSBTrack = (PSBTRACK)GetProp(hScrollBar, _T("PSBTRACK"));
	return pSBTrack;
}

int SB_RemovePSBTRACK(HWLWND hScrollBar)
{
	RemoveProp(hScrollBar, _T("PSBTRACK"));
	return 0;
}

// Copy from E:\win2k source code\private\inet\mshtml\src\core\cdutil\genutil.cxx
BOOL FormsDrawScrollButtonArrowMark(HDC hdc, LPRECT lprc, Direction eDirection, int iThickness)
{
	if (eDirection == D_TOP)
	{
		DrawFrameControl(hdc, lprc, DFC_SCROLL, DFCS_SCROLLUP | DFCS_FLAT);
	}
	else if (eDirection == D_BOTTOM)
	{
		DrawFrameControl(hdc, lprc, DFC_SCROLL, DFCS_SCROLLDOWN | DFCS_FLAT);
	}
	else if (eDirection == D_LEFT)
	{
		DrawFrameControl(hdc, lprc, DFC_SCROLL, DFCS_SCROLLLEFT | DFCS_FLAT);
	}
	else if (eDirection == D_RIGHT)
	{
		DrawFrameControl(hdc, lprc, DFC_SCROLL, DFCS_SCROLLRIGHT | DFCS_FLAT);
	}

	return (TRUE);
}

#pragma hdrstop

void CalcSBStuff(
	ScrollBar *pwnd,
	PSBCALC pSBCalc,
	BOOL fVert);

#define IsScrollBarControl(h) (GETFNID(h) == FNID_SCROLLBAR)

/*
 * Now it is possible to selectively Enable/Disable just one arrow of a Window
 * scroll bar; Various bits in the 7th word in the rgwScroll array indicates which
 * one of these arrows are disabled; The following masks indicate which bit of the
 * word indicates which arrow;
 */
#define WSB_HORZ_LF  0x0001  // Represents the Left arrow of the horizontal scroll bar.
#define WSB_HORZ_RT  0x0002  // Represents the Right arrow of the horizontal scroll bar.
#define WSB_VERT_UP  0x0004  // Represents the Up arrow of the vert scroll bar.
#define WSB_VERT_DN  0x0008  // Represents the Down arrow of the vert scroll bar.

#define WSB_VERT (WSB_VERT_UP | WSB_VERT_DN)
#define WSB_HORZ   (WSB_HORZ_LF | WSB_HORZ_RT)

void DrawCtlThumb(HWND);

/*
 * RETURN_IF_PSBTRACK_INVALID:
 * This macro tests whether the pSBTrack we have is invalid, which can happen
 * if it gets freed during a callback.
 * This protects agains the original pSBTrack being freed and no new one
 * being allocated or a new one being allocated at a different address.
 * This does not protect against the original pSBTrack being freed and a new
 * one being allocated at the same address.
 * If pSBTrack has changed, we assert that there is not already a new one
 * because we are really not expecting this.
 */
#define RETURN_IF_PSBTRACK_INVALID(pSBTrack, pwnd) \
	if ((pSBTrack) != PWNDTOPSBTRACK(pwnd)) {      \
		UserAssert(PWNDTOPSBTRACK(pwnd) == NULL);  \
		return;                                    \
	}

/*
 * REEVALUATE_PSBTRACK
 * This macro just refreshes the local variable pSBTrack, in case it has
 * been changed during a callback.  After performing this operation, pSBTrack
 * should be tested to make sure it is not now NULL.
 */
#if DBG
#define REEVALUATE_PSBTRACK(pSBTrack, pwnd, str)          \
	if ((pSBTrack) != PWNDTOPSBTRACK(pwnd)) {             \
		RIPMSG3(RIP_WARNING,                              \
			"%s: pSBTrack changed from %#p to %#p",   \
			(str), (pSBTrack), PWNDTOPSBTRACK(pwnd)); \
	}                                                     \
	(pSBTrack) = PWNDTOPSBTRACK(pwnd)
#else
#define REEVALUATE_PSBTRACK(pSBTrack, pwnd, str)          \
	(pSBTrack) = PWNDTOPSBTRACK(pwnd)
#endif

/***************************************************************************\
* HitTestScrollBar
*
* 11/15/96      vadimg          ported from Memphis sources
\***************************************************************************/

int HitTestScrollBar(HWND pwnd, BOOL fVert, POINT pt)
{
	//	UINT wDisable;
	//	int px;
	//	BOOL fCtl = TRUE;//IsScrollBarControl(pwnd);
	//	SBCALC SBCalc, *pSBCalc;
	//
	//	if (fCtl)
	//	{
	//		wDisable = ((PSBWND)pwnd->m_pMember)->wDisableFlags;
	//	}
	//	else
	//	{
	//#ifdef USE_MIRRORING
	//		//
	//		// Reflect the click coordinates on the horizontal
	//		// scroll bar if the window is mirrored
	//		//
	//		if (TestWF(pwnd, WEFLAYOUTRTL) && !fVert)
	//		{
	//			pt.x = pwnd->rcWindow.right - pt.x;
	//		}
	//		else
	//#endif
	//			pt.x -= pwnd->rcWindow.left;
	//
	//		pt.y -= pwnd->rcWindow.top;
	//		wDisable = GetWndSBDisableFlags(pwnd, fVert);
	//	}
	//
	//	if ((wDisable & SB_DISABLE_MASK) == SB_DISABLE_MASK)
	//	{
	//		return HTERROR;
	//	}
	//
	//	if (fCtl)
	//	{
	//		pSBCalc = &(((PSBWND)pwnd)->SBCalc);
	//	}
	//	else
	//	{
	//		pSBCalc = &SBCalc;
	//		CalcSBStuff(pwnd, pSBCalc, fVert);
	//	}
	//
	//	px = fVert ? pt.y : pt.x;
	//
	//	if (px < pSBCalc->pxUpArrow)
	//	{
	//		if (wDisable & LTUPFLAG)
	//		{
	//			return HTERROR;
	//		}
	//		return HTSCROLLUP;
	//	}
	//	else if (px >= pSBCalc->pxDownArrow)
	//	{
	//		if (wDisable & RTDNFLAG)
	//		{
	//			return HTERROR;
	//		}
	//		return HTSCROLLDOWN;
	//	}
	//	else if (px < pSBCalc->pxThumbTop)
	//	{
	//		return HTSCROLLUPPAGE;
	//	}
	//	else if (px < pSBCalc->pxThumbBottom)
	//	{
	//		return HTSCROLLTHUMB;
	//	}
	//	else if (px < pSBCalc->pxDownArrow)
	//	{
	//		return HTSCROLLDOWNPAGE;
	//	}
	return HTERROR;
}

BOOL _SBGetParms(int code, SBCALC *pw, LPSCROLLINFO lpsi)
{
	if (lpsi->fMask & SIF_RANGE)
	{
		lpsi->nMin = pw->sbd.posMin;
		lpsi->nMax = pw->sbd.posMax;
	}

	if (lpsi->fMask & SIF_PAGE)
	{
		lpsi->nPage = pw->sbd.page;
	}

	if (lpsi->fMask & SIF_POS)
	{
		lpsi->nPos = pw->sbd.pos;
	}

	if (lpsi->fMask & SIF_TRACKPOS)
	{
		// posNew is in the context of psbiSB's window and bar code
		lpsi->nTrackPos = pw->nTrackPos;
	}
	return ((lpsi->fMask & SIF_ALL) ? TRUE : FALSE);
}

/***************************************************************************\
* GetWndSBDisableFlags
*
* This returns the scroll bar Disable flags of the scroll bars of a
*  given Window.
*
*
* History:
*  4-18-91 MikeHar Ported for the 31 merge
\***************************************************************************/

UINT GetWndSBDisableFlags(
	HWND pwnd,  // The window whose scroll bar Disable Flags are to be returned;
	BOOL fVert)  // If this is TRUE, it means Vertical scroll bar.
{
	//PSBINFO pw;

	//if ((pw = pwnd->pSBInfo) == NULL)
	//{
	//	RIPERR0(ERROR_NO_SCROLLBARS, RIP_VERBOSE, "");
	//	return 0;
	//}

	//return (fVert ? (pw->WSBflags & WSB_VERT) >> 2 : pw->WSBflags & WSB_HORZ);
	return 0;
}


/***************************************************************************\
*  xxxEnableSBCtlArrows()
*
*  This function can be used to selectively Enable/Disable
*     the arrows of a scroll bar Control
*
* History:
* 04-18-91 MikeHar      Ported for the 31 merge
\***************************************************************************/

BOOL xxxEnableSBCtlArrows(HWND pwnd, UINT wArrows)
{
	//UINT wOldFlags;

	//CheckLock(pwnd);
	//UserAssert(IsWinEventNotifyDeferredOK());

	//wOldFlags = ((PSBWND)pwnd)->wDisableFlags; // Get the original status

	//if (wArrows == ESB_ENABLE_BOTH)        // Enable both the arrows
	//{
	//	((PSBWND)pwnd)->wDisableFlags &= ~SB_DISABLE_MASK;
	//}
	//else
	//{
	//	((PSBWND)pwnd)->wDisableFlags |= wArrows;
	//}

	///*
	// * Check if the status has changed because of this call
	// */
	//if (wOldFlags == ((PSBWND)pwnd)->wDisableFlags)
	//{
	//	return FALSE;
	//}

	///*
	// * Else, redraw the scroll bar control to reflect the new state
	// */
	//if (IsWindowVisible(pwnd))
	//{
	//	xxxInvalidateRect(pwnd, NULL, TRUE);
	//}

	//if (FWINABLE())
	//{
	//	UINT wNewFlags = ((PSBWND)pwnd)->wDisableFlags;

	//	/*
	//	 * state change notifications
	//	 */
	//	if ((wOldFlags & ESB_DISABLE_UP) != (wNewFlags & ESB_DISABLE_UP))
	//	{
	//		xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_CLIENT,
	//			INDEX_SCROLLBAR_UP, WEF_USEPWNDTHREAD);
	//	}

	//	if ((wOldFlags & ESB_DISABLE_DOWN) != (wNewFlags & ESB_DISABLE_DOWN))
	//	{
	//		xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_CLIENT,
	//			INDEX_SCROLLBAR_DOWN, WEF_USEPWNDTHREAD);
	//	}
	//}

	return TRUE;
}


/***************************************************************************\
* xxxEnableWndSBArrows()
*
*  This function can be used to selectively Enable/Disable
*     the arrows of a Window Scroll bar(s)
*
* History:
*  4-18-91 MikeHar      Ported for the 31 merge
\***************************************************************************/

BOOL xxxEnableWndSBArrows(HWND pwnd, UINT wSBflags, UINT wArrows)
{
	//INT wOldFlags;
	//PSBINFO pw;
	//BOOL bRetValue = FALSE;
	//HDC hdc;

	//CheckLock(pwnd);
	//UserAssert(IsWinEventNotifyDeferredOK());

	//if ((pw = pwnd->pSBInfo) != NULL)
	//{
	//	wOldFlags = pw->WSBflags;
	//}
	//else
	//{

	//	/*
	//	 * Originally everything is enabled; Check to see if this function is
	//	 * asked to disable anything; Otherwise, no change in status; So, must
	//	 * return immediately;
	//	 */
	//	if (!wArrows)
	//	{
	//		return FALSE;    // No change in status!
	//	}

	//	wOldFlags = 0;    // Both are originally enabled;
	//	if ((pw = _InitPwSB(pwnd)) == NULL) // Allocate the pSBInfo for hWnd
	//	{
	//		return FALSE;
	//	}
	//}


	//if ((hdc = _GetWindowDC(pwnd)) == NULL)
	//{
	//	return FALSE;
	//}

	///*
	// *  First Take care of the Horizontal Scroll bar, if one exists.
	// */
	//if ((wSBflags == SB_HORZ) || (wSBflags == SB_BOTH))
	//{
	//	if (wArrows == ESB_ENABLE_BOTH)     // Enable both the arrows
	//	{
	//		pw->WSBflags &= ~SB_DISABLE_MASK;
	//	}
	//	else
	//	{
	//		pw->WSBflags |= wArrows;
	//	}

	//	/*
	//	 * Update the display of the Horizontal Scroll Bar;
	//	 */
	//	if (pw->WSBflags != wOldFlags)
	//	{
	//		bRetValue = TRUE;
	//		wOldFlags = pw->WSBflags;
	//		if (TestWF(pwnd, WFHPRESENT) && !TestWF(pwnd, WFMINIMIZED) &&
	//			IsWindowVisible(pwnd))
	//		{
	//			xxxDrawScrollBar(pwnd, hdc, FALSE);  // Horizontal Scroll Bar.
	//		}
	//	}
	//	if (FWINABLE())
	//	{
	//		// Left button
	//		if ((wOldFlags & ESB_DISABLE_LEFT) != (pw->WSBflags & ESB_DISABLE_LEFT))
	//		{
	//			xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_HSCROLL,
	//				INDEX_SCROLLBAR_UP, WEF_USEPWNDTHREAD);
	//		}

	//		// Right button
	//		if ((wOldFlags & ESB_DISABLE_RIGHT) != (pw->WSBflags & ESB_DISABLE_RIGHT))
	//		{
	//			xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_HSCROLL,
	//				INDEX_SCROLLBAR_DOWN, WEF_USEPWNDTHREAD);
	//		}
	//	}
	//}

	///*
	// *  Then take care of the Vertical Scroll bar, if one exists.
	// */
	//if ((wSBflags == SB_VERT) || (wSBflags == SB_BOTH))
	//{
	//	if (wArrows == ESB_ENABLE_BOTH)     // Enable both the arrows
	//	{
	//		pw->WSBflags &= ~(SB_DISABLE_MASK << 2);
	//	}
	//	else
	//	{
	//		pw->WSBflags |= (wArrows << 2);
	//	}

	//	/*
	//	 * Update the display of the Vertical Scroll Bar;
	//	 */
	//	if (pw->WSBflags != wOldFlags)
	//	{
	//		bRetValue = TRUE;
	//		if (TestWF(pwnd, WFVPRESENT) && !TestWF(pwnd, WFMINIMIZED) &&
	//			IsWindowVisible(pwnd))
	//		{
	//			xxxDrawScrollBar(pwnd, hdc, TRUE);  // Vertical Scroll Bar
	//		}

	//		if (FWINABLE())
	//		{
	//			// Up button
	//			if ((wOldFlags & (ESB_DISABLE_UP << 2)) != (pw->WSBflags & (ESB_DISABLE_UP << 2)))
	//			{
	//				xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_VSCROLL,
	//					INDEX_SCROLLBAR_UP, WEF_USEPWNDTHREAD);
	//			}

	//			// Down button
	//			if ((wOldFlags & (ESB_DISABLE_DOWN << 2)) != (pw->WSBflags & (ESB_DISABLE_DOWN << 2)))
	//			{
	//				xxxWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd, OBJID_VSCROLL,
	//					INDEX_SCROLLBAR_DOWN, WEF_USEPWNDTHREAD);
	//			}
	//		}
	//	}
	//}

	//_ReleaseDC(hdc);

	//return bRetValue;

	return 0;
}


/***************************************************************************\
* EnableScrollBar()
*
* This function can be used to selectively Enable/Disable
*     the arrows of a scroll bar; It could be used with Windows Scroll
*     bars as well as scroll bar controls
*
* History:
*  4-18-91 MikeHar Ported for the 31 merge
\***************************************************************************/

BOOL xxxEnableScrollBar(
	HWND pwnd,
	UINT wSBflags,  // Whether it is a Window Scroll Bar; if so, HORZ or VERT?
	// Possible values are SB_HORZ, SB_VERT, SB_CTL or SB_BOTH
	UINT wArrows)   // Which arrows must be enabled/disabled:
// ESB_ENABLE_BOTH = > Enable both arrows.
// ESB_DISABLE_LTUP = > Disable Left/Up arrow;
// ESB_DISABLE_RTDN = > DIsable Right/Down arrow;
// ESB_DISABLE_BOTH = > Disable both the arrows;
{
#define ES_NOTHING 0
#define ES_DISABLE 1
#define ES_ENABLE  2
	//UINT wOldFlags;
	//UINT wEnableWindow;

	//CheckLock(pwnd);

	//if (wSBflags != SB_CTL)
	//{
	//	return xxxEnableWndSBArrows(pwnd, wSBflags, wArrows);
	//}

	///*
	// *  Let us assume that we don't have to call EnableWindow
	// */
	//wEnableWindow = ES_NOTHING;

	//wOldFlags = ((PSBWND)pwnd)->wDisableFlags & (UINT)SB_DISABLE_MASK;

	///*
	// * Check if the present state of the arrows is exactly the same
	// *  as what the caller wants:
	// */
	//if (wOldFlags == wArrows)
	//{
	//	return FALSE ;    // If so, nothing needs to be done;
	//}

	///*
	// * Check if the caller wants to disable both the arrows
	// */
	//if (wArrows == ESB_DISABLE_BOTH)
	//{
	//	wEnableWindow = ES_DISABLE;      // Yes! So, disable the whole SB Ctl.
	//}
	//else
	//{

	//	/*
	//	 * Check if the caller wants to enable both the arrows
	//	 */
	//	if (wArrows == ESB_ENABLE_BOTH)
	//	{

	//		/*
	//		 * We need to enable the SB Ctl only if it was already disabled.
	//		 */
	//		if (wOldFlags == ESB_DISABLE_BOTH)
	//		{
	//			wEnableWindow = ES_ENABLE;    // EnableWindow(.., TRUE);
	//		}
	//	}
	//	else
	//	{

	//		/*
	//		 * Now, Caller wants to disable only one arrow;
	//		 * Check if one of the arrows was already disabled and we want
	//		 * to disable the other;If so, the whole SB Ctl will have to be
	//		 * disabled; Check if this is the case:
	//		 */
	//		if ((wOldFlags | wArrows) == ESB_DISABLE_BOTH)
	//		{
	//			wEnableWindow = ES_DISABLE;    // EnableWindow(, FALSE);
	//		}
	//	}
	//}
	//if (wEnableWindow != ES_NOTHING)
	//{

	//	/*
	//	 * EnableWindow returns old state of the window; We must return
	//	 * TRUE only if the Old state is different from new state.
	//	 */
	//	if (xxxEnableWindow(pwnd, (BOOL)(wEnableWindow == ES_ENABLE)))
	//	{
	//		return !(TestWF(pwnd, WFDISABLED));
	//	}
	//	else
	//	{
	//		return TestWF(pwnd, WFDISABLED);
	//	}
	//}

	return (BOOL)SendMessage(pwnd, SBM_ENABLE_ARROWS, (DWORD)wArrows, 0);
#undef ES_NOTHING
#undef ES_DISABLE
#undef ES_ENABLE
}

/***************************************************************************\
*
*  DrawSize() -
*
\***************************************************************************/
void DrawSize(HWND pwnd, HDC hdc, int cxFrame, int cyFrame)
{
	//	int     x, y;
	//	//HBRUSH  hbrSave;
	//
	//	if (TestWF(pwnd, WEFLEFTSCROLL))
	//	{
	//		x = cxFrame;
	//	}
	//	else
	//	{
	//		x = pwnd->rcWindow.right - pwnd->rcWindow.left - cxFrame - IUIGetGloabelVar()->g_cxVScroll;
	//	}
	//	y = pwnd->rcWindow.bottom - pwnd->rcWindow.top  - cyFrame - IUIGetGloabelVar()->g_cyHScroll;
	//
	//	// If we have a scrollbar control, or the sizebox is not associated with
	//	// a sizeable window, draw the flat gray sizebox.  Otherwise, use the
	//	// sizing grip.
	//	if (IsScrollBarControl(pwnd))
	//	{
	//		if (TestWF(pwnd, SBFSIZEGRIP))
	//		{
	//			goto DrawSizeGrip;
	//		}
	//		else
	//		{
	//			goto DrawBox;
	//		}
	//
	//	}
	//	else if (!SizeBoxHwnd(pwnd))
	//	{
	//DrawBox:
	//		{
	//			//hbrSave = GreSelectBrush(hdc, SYSHBR(3DFACE));
	//			//GrePatBlt(hdc, x, y, IUIGetGloabelVar()->g_cxVScroll, SYSMET(CYHSCROLL), PATCOPY);
	//			//GreSelectBrush(hdc, hbrSave);
	//
	//			POLYPATBLT PolyData;
	//
	//			PolyData.x         = x;
	//			PolyData.y         = y;
	//			PolyData.cx        = IUIGetGloabelVar()->g_cxVScroll;
	//			PolyData.cy        = IUIGetGloabelVar()->g_cyHScroll;
	//			PolyData.BrClr.hbr = SYSHBR(3DFACE);
	//
	//			GrePolyPatBlt(hdc, PATCOPY, &PolyData, 1, PPB_BRUSH);
	//
	//		}
	//	}
	//	else
	//	{
	//DrawSizeGrip:
	//		// Blt out the grip bitmap.
	//		BitBltSysBmp(hdc, x, y, TestWF(pwnd, WEFLEFTSCROLL) ? OBI_NCGRIP_L : OBI_NCGRIP);
	//	}
}

/***************************************************************************\
*
*  DrawGroove()
*
*  Draws lines & middle of thumb groove
*  Note that pw points into prc.  Moreover, note that both pw & prc are
*  NEAR pointers, so *prc better not be on the stack.
*
\***************************************************************************/
void DrawGroove(HDC hdc, HBRUSH  hbr, LPRECT prc, BOOL fVert)
{
	//if ((hbr == SYSHBR(3DHILIGHT)) || (hbr == gpsi->hbrGray))
	//{
	//	FillRect(hdc, prc, hbr);
	//}
	//else
	//{
	//	RECT    rc;

	//	// Draw sides
	//	CopyRect(&rc, prc);
	//	DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_ADJUST | BF_FLAT |
	//		(fVert ? BF_LEFT | BF_RIGHT : BF_TOP | BF_BOTTOM));

	//	// Fill middle
	//	FillRect(hdc, &rc, hbr);
	//}
}

// 当按住滑块滑块滚动条时，鼠标离开滑块一定范围内，都是可以滑动滑块的，
// 但当鼠标离开的距离大于这个范围，滑块将跳到滑动初始位置，
// 本函数就是计算这个允许滑动的范围
void CalcTrackDragRect(PSBTRACK pSBTrack)
{

	int     cx;
	int     cy;
	LPINT   pwX, pwY;

	//
	// Point pwX and pwY at the parts of the rectangle
	// corresponding to pSBCalc->pxLeft, pxTop, etc.
	//
	// pSBTrack->pSBCalc->pxLeft is the left edge of a vertical
	// scrollbar and the top edge of horizontal one.
	// pSBTrack->pSBCalc->pxTop is the top of a vertical
	// scrollbar and the left of horizontal one.
	// etc...
	//
	// Point pwX and pwY to the corresponding parts
	// of pSBTrack->rcTrack.
	//

	pwX = pwY = (LPINT)&pSBTrack->rcTrack;

	if (pSBTrack->fTrackVert)
	{
		cy = IUIGetGloabelVar()->g_cyVScroll;
		pwY++;
	}
	else
	{
		cy = IUIGetGloabelVar()->g_cxHScroll;
		pwX++;
	}
	/*
	 * Later5.0 GerardoB: People keep complaining about this tracking region
	 *  being too narrow so let's make it wider while PM decides what to do
	 *  about it.
	 * We also used to have some hard coded min and max values but that should
	 *  depend on some metric, if at all needed.
	 */
	cx = (pSBTrack->pSBCalc->pxRight - pSBTrack->pSBCalc->pxLeft) * 8;
	cy *= 2;

	*(pwX + 0) = pSBTrack->pSBCalc->pxLeft - cx;
	*(pwY + 0) = pSBTrack->pSBCalc->pxTop - cy;
	*(pwX + 2) = pSBTrack->pSBCalc->pxRight + cx;
	*(pwY + 2) = pSBTrack->pSBCalc->pxBottom + cy;
}

void RecalcTrackRect(PSBTRACK pSBTrack)
{
	LPINT pwX, pwY;
	RECT rcSB;

	pwX = (LPINT)&rcSB;
	pwY = pwX + 1;
	if (!pSBTrack->fTrackVert)
	{
		pwX = pwY--;
	}

	*(pwX + 0) = pSBTrack->pSBCalc->pxLeft;
	*(pwY + 0) = pSBTrack->pSBCalc->pxTop;
	*(pwX + 2) = pSBTrack->pSBCalc->pxRight;
	*(pwY + 2) = pSBTrack->pSBCalc->pxBottom;

	switch (pSBTrack->cmdSB)
	{
	case SB_LINEUP:
		*(pwY + 2) = pSBTrack->pSBCalc->pxUpArrow;
		break;
	case SB_LINEDOWN:
		*(pwY + 0) = pSBTrack->pSBCalc->pxDownArrow;
		break;
	case SB_PAGEUP:
		*(pwY + 0) = pSBTrack->pSBCalc->pxUpArrow;
		*(pwY + 2) = pSBTrack->pSBCalc->pxThumbTop;
		break;
	case SB_THUMBPOSITION:
		CalcTrackDragRect(pSBTrack);
		break;
	case SB_PAGEDOWN:
		*(pwY + 0) = pSBTrack->pSBCalc->pxThumbBottom;
		*(pwY + 2) = pSBTrack->pSBCalc->pxDownArrow;
		break;
	}

	if (pSBTrack->cmdSB != SB_THUMBPOSITION)
	{
		CopyRect(&pSBTrack->rcTrack, &rcSB);
	}
}

/***************************************************************************\
* DrawThumb2
*
*
*
* History:
* 01-03-94  FritzS   Chicago changes
\***************************************************************************/

void DrawThumb2(PSBCALC pSBCalc, HDC hdc, HBRUSH hbr, BOOL fVert, UINT wDisable)  /* Disabled flags for the scroll bar */
{
	int    *pLength;
	int    *pWidth;
	RECT   rcSB;
	PSBTRACK pSBTrack;

	//
	// Bail out if the scrollbar has an empty rect
	//
	if ((pSBCalc->pxTop >= pSBCalc->pxBottom) || (pSBCalc->pxLeft >= pSBCalc->pxRight))
	{
		return;
	}
	pLength = (LPINT)&rcSB;
	if (fVert)
	{
		pWidth = pLength++;
	}
	else
	{
		pWidth  = pLength + 1;
	}

	pWidth[0] = pSBCalc->pxLeft;
	pWidth[2] = pSBCalc->pxRight;

	/*
	 * If both scroll bar arrows are disabled, then we should not draw
	 * the thumb.  So, quit now!
	 */
	if (((wDisable & LTUPFLAG) && (wDisable & RTDNFLAG)) ||
		((pSBCalc->pxDownArrow - pSBCalc->pxUpArrow) < pSBCalc->cpxThumb))
	{
		pLength[0] = pSBCalc->pxUpArrow;
		pLength[2] = pSBCalc->pxDownArrow;

		DrawGroove(hdc, hbr, &rcSB, fVert);
		return;
	}

	if (pSBCalc->pxUpArrow < pSBCalc->pxThumbTop)
	{
		// Fill in space above Thumb
		pLength[0] = pSBCalc->pxUpArrow;
		pLength[2] = pSBCalc->pxThumbTop;

		DrawGroove(hdc, hbr, &rcSB, fVert);
	}

	if (pSBCalc->pxThumbBottom < pSBCalc->pxDownArrow)
	{
		// Fill in space below Thumb
		pLength[0] = pSBCalc->pxThumbBottom;
		pLength[2] = pSBCalc->pxDownArrow;

		DrawGroove(hdc, hbr, &rcSB, fVert);
	}

	//
	// Draw elevator
	//
	pLength[0] = pSBCalc->pxThumbTop;
	pLength[2] = pSBCalc->pxThumbBottom;

	// Not soft!
	//L DrawPushButton(hdc, &rcSB, 0, 0);

	/*
	 * If we're tracking a page scroll, then we've obliterated the hilite.
	 * We need to correct the hiliting rectangle, and rehilite it.
	 */
	pSBTrack = NULL;//L PWNDTOPSBTRACK(pwnd);

	if (pSBTrack && (pSBTrack->cmdSB == SB_PAGEUP || pSBTrack->cmdSB == SB_PAGEDOWN) &&
		(BOOL)pSBTrack->fTrackVert == fVert)
	{

		if (pSBTrack->fTrackRecalc)
		{
			RecalcTrackRect(pSBTrack);
			pSBTrack->fTrackRecalc = FALSE;
		}

		pLength = (int *)&pSBTrack->rcTrack;

		if (fVert)
		{
			pLength++;
		}

		if (pSBTrack->cmdSB == SB_PAGEUP)
		{
			pLength[2] = pSBCalc->pxThumbTop;
		}
		else
		{
			pLength[0] = pSBCalc->pxThumbBottom;
		}

		if (pLength[0] < pLength[2])
		{
			InvertRect(hdc, &pSBTrack->rcTrack);
		}
	}
}

/***************************************************************************\
* xxxDrawSB2
*
*
*
* History:
\***************************************************************************/

void xxxDrawSB2(
	HWND pwnd,
	PSBCALC pSBCalc,
	HDC hdc,
	BOOL fVert,
	UINT wDisable)
{

	int     cLength;
	int     cWidth;
	int     *pwX;
	int     *pwY;
	HBRUSH hbr;
	HBRUSH hbrSave;
	int cpxArrow;
	RECT    rc, rcSB;
	COLORREF crText, crBk;

	cLength = (pSBCalc->pxBottom - pSBCalc->pxTop) / 2;
	cWidth = (pSBCalc->pxRight - pSBCalc->pxLeft);

	if ((cLength <= 0) || (cWidth <= 0))
	{
		return;
	}
	if (fVert)
	{
		cpxArrow = IUIGetGloabelVar()->g_cyVScroll;
	}
	else
	{
		cpxArrow = IUIGetGloabelVar()->g_cxVScroll;
	}

	/*
	 * Save background and DC color, since they get changed in
	 * xxxGetColorObjects. Restore before we return.
	 */
	crBk = RGB(255, 255, 255);//L GreGetBkColor(hdc);
	crText = 0;//L GreGetTextColor(hdc);

	hbr = NULL;//L xxxGetColorObjects(pwnd, hdc);

	if (cLength > cpxArrow)
	{
		cLength = cpxArrow;
	}
	pwX = (int *)&rcSB;
	pwY = pwX + 1;
	if (!fVert)
	{
		pwX = pwY--;
	}

	pwX[0] = pSBCalc->pxLeft;
	pwY[0] = pSBCalc->pxTop;
	pwX[2] = pSBCalc->pxRight;
	pwY[2] = pSBCalc->pxBottom;

	hbrSave = IUIGetGloabelVar()->g_hbrBtnFace;

	//
	// BOGUS
	// Draw scrollbar arrows as disabled if the scrollbar itself is
	// disabled OR if the window it is a part of is disabled?
	//
	if (fVert)
	{
		if ((cLength == IUIGetGloabelVar()->g_cyVScroll) && (cWidth == IUIGetGloabelVar()->g_cxVScroll))
		{
			//L 			BitBltSysBmp(hdc, rcSB.left, rcSB.top, (wDisable & LTUPFLAG) ? OBI_UPARROW_I : OBI_UPARROW);
			// 			BitBltSysBmp(hdc, rcSB.left, rcSB.bottom - cLength, (wDisable & RTDNFLAG) ? OBI_DNARROW_I : OBI_DNARROW);
		}
		else
		{
			CopyRect(&rc, &rcSB);
			rc.bottom = rc.top + cLength;
			DrawFrameControl(hdc, &rc, DFC_SCROLL,
				DFCS_SCROLLUP | ((wDisable & LTUPFLAG) ? DFCS_INACTIVE : 0));

			rc.bottom = rcSB.bottom;
			rc.top = rcSB.bottom - cLength;
			DrawFrameControl(hdc, &rc, DFC_SCROLL,
				DFCS_SCROLLDOWN | ((wDisable & RTDNFLAG) ? DFCS_INACTIVE : 0));
		}
	}
	else
	{
		if ((cLength == 16) && (cWidth == 16))
		{
			//L BitBltSysBmp(hdc, rcSB.left, rcSB.top, (wDisable & LTUPFLAG) ? OBI_LFARROW_I : OBI_LFARROW);
			//BitBltSysBmp(hdc, rcSB.right - cLength, rcSB.top, (wDisable & RTDNFLAG) ? OBI_RGARROW_I : OBI_RGARROW);
		}
		else
		{
			CopyRect(&rc, &rcSB);
			rc.right = rc.left + cLength;
			DrawFrameControl(hdc, &rc, DFC_SCROLL,
				DFCS_SCROLLLEFT | ((wDisable & LTUPFLAG) ? DFCS_INACTIVE : 0));

			rc.right = rcSB.right;
			rc.left = rcSB.right - cLength;
			DrawFrameControl(hdc, &rc, DFC_SCROLL,
				DFCS_SCROLLRIGHT | ((wDisable & RTDNFLAG) ? DFCS_INACTIVE : 0));
		}
	}

	hbrSave = (HBRUSH)SelectObject(hdc, hbrSave);
	DrawThumb2(pSBCalc, hdc, hbr, fVert, wDisable);
	SelectObject(hdc, hbrSave);

	SetBkColor(hdc, crBk);
	SetTextColor(hdc, crText);
}

/***************************************************************************\
* zzzSetSBCaretPos
*
*
*
* History:
\***************************************************************************/

void zzzSetSBCaretPos(PSBWND psbwnd)
{

	//if ((HWND)psbwnd == PtiCurrent()->pq->spwndFocus)
	{
		bool bfVert = true;
		//L SetCaretPos((fVert ? psbwnd->SBCalc.pxLeft : psbwnd->SBCalc.pxThumbTop) + IUIGetGloabelVar()->g_cxEdge,
		//	(psbwnd->fVert ? psbwnd->SBCalc.pxThumbTop : psbwnd->SBCalc.pxLeft) + IUIGetGloabelVar()->g_cyEdge);
	}
}

// 计算上下按钮及滑块的坐标
void CalcSBStuff2(PSBCALC  pSBCalc, LPCRECT lprcScrollBar, CONST PSBDATA pw, BOOL fVert, UINT uButton1MaxHeight, UINT uButton2MaxHeight)
{
	if (fVert)
	{
		pSBCalc->pxTop = lprcScrollBar->top;
		pSBCalc->pxBottom = lprcScrollBar->bottom;
		pSBCalc->pxLeft = lprcScrollBar->left;
		pSBCalc->pxRight = lprcScrollBar->right;
		pSBCalc->cpxThumb = IUIGetGloabelVar()->g_cyVScroll;
	}
	else
	{

		/*
		 * For horiz scroll bars, "left" & "right" are "top" and "bottom",
		 * and vice versa.
		 */
		pSBCalc->pxTop = lprcScrollBar->left;
		pSBCalc->pxBottom = lprcScrollBar->right;
		pSBCalc->pxLeft = lprcScrollBar->top;
		pSBCalc->pxRight = lprcScrollBar->bottom;
		pSBCalc->cpxThumb = IUIGetGloabelVar()->g_cxHScroll;
	}

	pSBCalc->nButton1MaxLength = uButton1MaxHeight;
	pSBCalc->nButton2MaxLength = uButton1MaxHeight;

	pSBCalc->sbd.pos = pw->pos;
	pSBCalc->sbd.page = pw->page;
	pSBCalc->sbd.posMin = pw->posMin;
	pSBCalc->sbd.posMax = pw->posMax;

	DWORD dwRange = ((DWORD)(pSBCalc->sbd.posMax - pSBCalc->sbd.posMin)) + 1;

	//
	// For the case of short scroll bars that don't have enough
	// room to fit the full-sized up and down arrows, shorten
	// their sizes to make 'em fit
	//
	// 上下按钮的高度
	int cpx1 = min((pSBCalc->pxBottom - pSBCalc->pxTop) / 2, pSBCalc->nButton1MaxLength);
	int cpx2 = min((pSBCalc->pxBottom - pSBCalc->pxTop) / 2, pSBCalc->nButton2MaxLength);

	pSBCalc->pxUpArrow   = pSBCalc->pxTop    + cpx1;
	pSBCalc->pxDownArrow = pSBCalc->pxBottom - cpx2;

	if ((pw->page != 0) && (dwRange != 0))
	{
		// JEFFBOG -- This is the one and only place where we should
		// see 'range'.  Elsewhere it should be 'range - page'.

		/*
		 * The minimun thumb size used to depend on the frame/edge metrics.
		 * People that increase the scrollbar width/height expect the minimun
		 *  to grow with proportianally. So NT5 bases the minimun on
		 *  CXH/YVSCROLL, which is set by default in cpxThumb.
		 */
		/*
		 * i is used to keep the macro "max" from executing EngMulDiv twice.
		 */
		// 计算滑块的高度，滑块高度最小不能小于宽度除以2
		int i = MulDiv(pSBCalc->pxDownArrow - pSBCalc->pxUpArrow, pw->page, dwRange);
		pSBCalc->cpxThumb = max(pSBCalc->cpxThumb / 2, i);
	}

	pSBCalc->pxMin = pSBCalc->pxTop + cpx1;
	pSBCalc->cpx = pSBCalc->pxBottom - cpx2 - pSBCalc->cpxThumb - pSBCalc->pxMin;

	int denom = dwRange - (pw->page ? pw->page : 1);
	if (denom)
	{
		pSBCalc->pxThumbTop = MulDiv(pw->pos - pw->posMin, pSBCalc->cpx, denom) + pSBCalc->pxMin;

		int n = 0;
	}
	else
	{
		pSBCalc->pxThumbTop = pSBCalc->pxMin - 1;
	}

	pSBCalc->pxThumbBottom = pSBCalc->pxThumbTop + pSBCalc->cpxThumb;

}

// 计算上下按钮及滑块的坐标
void SBCtlSetup(SBWND *psbwnd)
{
	RECT rc;
	GetClientRect(psbwnd->hwnd, &rc);
	CalcSBStuff2(&psbwnd->SBCalc, &rc, (PSBDATA)&psbwnd->SBCalc, psbwnd->fVert,
		psbwnd->m_uButton1Length, psbwnd->m_uButton2Length);
}

/***************************************************************************\
* HotTrackSB
*
\***************************************************************************/

#ifdef COLOR_HOTTRACKING

DWORD GetTrackFlags(int ht, BOOL fDraw)
{
	if (fDraw)
	{
		switch (ht)
		{
		case HTSCROLLUP:
		case HTSCROLLUPPAGE:
			return LTUPFLAG;

		case HTSCROLLDOWN:
		case HTSCROLLDOWNPAGE:
			return RTDNFLAG;

		case HTSCROLLTHUMB:
			return LTUPFLAG | RTDNFLAG;

		default:
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

BOOL xxxHotTrackSB(HWND pwnd, int htEx, BOOL fDraw)
{
	SBCALC SBCalc;
	HDC  hdc;
	BOOL fVert = HIWORD(htEx);
	int ht = LOWORD(htEx);
	DWORD dwTrack = GetTrackFlags(ht, fDraw);

	CheckLock(pwnd);

	/*
	 * xxxDrawSB2 does not callback or leave the critical section when it's
	 * not a SB control and the window belongs to a different thread. It
	 * calls xxxDefWindowProc which simply returns the brush color.
	 */
	CalcSBStuff(pwnd, &SBCalc, fVert);
	hdc = _GetDCEx(pwnd, NULL, DCX_WINDOW | DCX_USESTYLE | DCX_CACHE);
	xxxDrawSB2(pwnd, &SBCalc, hdc, fVert, GetWndSBDisableFlags(pwnd, fVert), dwTrack);
	_ReleaseDC(hdc);
	return TRUE;
}

void xxxHotTrackSBCtl(PSBWND psbwnd, int ht, BOOL fDraw)
{
	DWORD dwTrack = GetTrackFlags(ht, fDraw);
	HDC hdc;

	CheckLock(psbwnd);

	SBCtlSetup(psbwnd);
	hdc = _GetDCEx((HWND)psbwnd, NULL, DCX_WINDOW | DCX_USESTYLE | DCX_CACHE);
	xxxDrawSB2((HWND)psbwnd, &psbwnd->SBCalc, hdc, psbwnd->fVert, psbwnd->wDisableFlags, dwTrack);
	_ReleaseDC(hdc);
}
#endif // COLOR_HOTTRACKING

BOOL SBSetParms(PSBDATA pw, LPSCROLLINFO lpsi, LPBOOL lpfScroll, LPLONG lplres)
{
	// pass the struct because we modify the struct but don't want that
	// modified version to get back to the calling app

	BOOL fChanged = FALSE;

	if (lpsi->fMask & SIF_RETURNOLDPOS)
		// save previous position
	{
		*lplres = pw->pos;
	}

	if (lpsi->fMask & SIF_RANGE)
	{
		// if the range MAX is below the range MIN -- then treat is as a
		// zero range starting at the range MIN.
		if (lpsi->nMax < lpsi->nMin)
		{
			lpsi->nMax = lpsi->nMin;
		}

		if ((pw->posMin != lpsi->nMin) || (pw->posMax != lpsi->nMax))
		{
			pw->posMin = lpsi->nMin;
			pw->posMax = lpsi->nMax;

			if (!(lpsi->fMask & SIF_PAGE))
			{
				lpsi->fMask |= SIF_PAGE;
				lpsi->nPage = pw->page;
			}

			if (!(lpsi->fMask & SIF_POS))
			{
				lpsi->fMask |= SIF_POS;
				lpsi->nPos = pw->pos;
			}

			fChanged = TRUE;
		}
	}

	if (lpsi->fMask & SIF_PAGE)
	{
		DWORD dwMaxPage = (DWORD) abs(pw->posMax - pw->posMin) + 1;

		// Clip page to 0, posMax - posMin + 1

		if (lpsi->nPage > dwMaxPage)
		{
			lpsi->nPage = dwMaxPage;
		}


		if (pw->page != (int)(lpsi->nPage))
		{
			pw->page = lpsi->nPage;

			if (!(lpsi->fMask & SIF_POS))
			{
				lpsi->fMask |= SIF_POS;
				lpsi->nPos = pw->pos;
			}

			fChanged = TRUE;
		}
	}

	if (lpsi->fMask & SIF_POS)
	{
		int iMaxPos = pw->posMax - ((pw->page) ? pw->page - 1 : 0);
		// Clip pos to posMin, posMax - (page - 1).

		if (lpsi->nPos < pw->posMin)
		{
			lpsi->nPos = pw->posMin;
		}
		else if (lpsi->nPos > iMaxPos)
		{
			lpsi->nPos = iMaxPos;
		}


		if (pw->pos != lpsi->nPos)
		{
			pw->pos = lpsi->nPos;
			fChanged = TRUE;
		}
	}

	if (!(lpsi->fMask & SIF_RETURNOLDPOS))
	{
		// Return the new position
		*lplres = pw->pos;
	}

	/*
	 * This was added by JimA as Cairo merge but will conflict
	 * with the documentation for SetScrollPos
	 */
	/*
	    else if (*lplres == pw->pos)
	        *lplres = 0;
	*/
	if (lpsi->fMask & SIF_RANGE)
	{
		if (*lpfScroll = (pw->posMin != pw->posMax))
		{
			goto checkPage;
		}
	}
	else if (lpsi->fMask & SIF_PAGE)
checkPage:
		*lpfScroll = (pw->page <= (pw->posMax - pw->posMin));

	return fChanged;
}


/***************************************************************************\
* CalcSBStuff
*
*
*
* History:
\***************************************************************************/

void CalcSBStuff(
	ScrollBar *pwnd,
	PSBCALC pSBCalc,
	BOOL fVert)
{
	//	RECT    rcT;
	//	RECT    rcClient;
	//#ifdef USE_MIRRORING
	//	int     cx, iTemp;
	//#endif
	//
	//	//
	//	// Get client rectangle.  We know that scrollbars always align to the right
	//	// and to the bottom of the client area.
	//	//
	//	GetRect(pwnd, &rcClient, GRECT_CLIENT | GRECT_WINDOWCOORDS);
	//#ifdef USE_MIRRORING
	//	if (TestWF(pwnd, WEFLAYOUTRTL))
	//	{
	//		cx             = pwnd->rcWindow.right - pwnd->rcWindow.left;
	//		iTemp          = rcClient.left;
	//		rcClient.left  = cx - rcClient.right;
	//		rcClient.right = cx - iTemp;
	//	}
	//#endif
	//
	//	if (fVert)
	//	{
	//		// Only add on space if vertical scrollbar is really there.
	//		if (TestWF(pwnd, WEFLEFTSCROLL))
	//		{
	//			rcT.right = rcT.left = rcClient.left;
	//			if (TestWF(pwnd, WFVPRESENT))
	//			{
	//				rcT.left -= IUIGetGloabelVar()->g_cxVScroll;
	//			}
	//		}
	//		else
	//		{
	//			rcT.right = rcT.left = rcClient.right;
	//			if (TestWF(pwnd, WFVPRESENT))
	//			{
	//				rcT.right += IUIGetGloabelVar()->g_cxVScroll;
	//			}
	//		}
	//
	//		rcT.top = rcClient.top;
	//		rcT.bottom = rcClient.bottom;
	//	}
	//	else
	//	{
	//		// Only add on space if horizontal scrollbar is really there.
	//		rcT.bottom = rcT.top = rcClient.bottom;
	//		if (TestWF(pwnd, WFHPRESENT))
	//		{
	//			rcT.bottom += IUIGetGloabelVar()->g_cyHScroll;
	//		}
	//
	//		rcT.left = rcClient.left;
	//		rcT.right = rcClient.right;
	//	}
	//
	//	// If InitPwSB stuff fails (due to our heap being full) there isn't anything reasonable
	//	// we can do here, so just let it go through.  We won't fault but the scrollbar won't work
	//	// properly either...
	//	if (_InitPwSB(pwnd))
	//	{
	//		CalcSBStuff2(pSBCalc, &rcT, (fVert) ? &pwnd->pSBInfo->Vert :  &pwnd->pSBInfo->Horz, fVert);
	//	}

}

/***************************************************************************\
*
*  DrawCtlThumb()
*
\***************************************************************************/
void DrawCtlThumb(PSBWND psb)
{
	HBRUSH  hbr, hbrSave;
	HWND hWnd = GetHostWnd(psb->hwnd);
	HDC hdc = GetWindowDC(hWnd);

	SBCtlSetup(psb);

	hbrSave = SelectBrush(hdc, hbr = IUIGetGloabelVar()->g_hbrBtnFace);

	//	DrawThumb2((HWND) psb, &psb->SBCalc, hdc, hbr, psb->fVert, psb->wDisableFlags);

	SelectBrush(hdc, hbrSave);
	ReleaseDC(hWnd, hdc);
}

/***************************************************************************\
* xxxDrawScrollBar
*
*
*
* History:
\***************************************************************************/

void xxxDrawScrollBar(
	HWND pwnd,
	HDC hdc,
	BOOL fVert)
{
	//SBCALC SBCalc;
	//PSBCALC pSBCalc;
	//PSBTRACK pSBTrack = PWNDTOPSBTRACK(pwnd);

	//CheckLock(pwnd);
	//if (pSBTrack && (pwnd == pSBTrack->spwndTrack) && (pSBTrack->fCtlSB == FALSE)
	//	&& (fVert == (BOOL)pSBTrack->fTrackVert))
	//{
	//	pSBCalc = pSBTrack->pSBCalc;
	//}
	//else
	//{
	//	pSBCalc = &SBCalc;
	//}
	//CalcSBStuff(pwnd, pSBCalc, fVert);

	//xxxDrawSB2(pwnd, pSBCalc, hdc, fVert, GetWndSBDisableFlags(pwnd, fVert));
}

// 由像素计算滚动条位置
// px: 	滑块新的Top值
int SBPosFromPx(PSBCALC pSBCalc, int px)
{
	if (px < pSBCalc->pxMin)
	{
		return pSBCalc->sbd.posMin;
	}

	if (px >= pSBCalc->pxMin + pSBCalc->cpx)
	{
		return (pSBCalc->sbd.posMax - (pSBCalc->sbd.page ? pSBCalc->sbd.page - 1 : 0));
	}

	if (pSBCalc->cpx)
	{
		return (pSBCalc->sbd.posMin + MulDiv(pSBCalc->sbd.posMax - pSBCalc->sbd.posMin -
					(pSBCalc->sbd.page ? pSBCalc->sbd.page - 1 : 0),
					px - pSBCalc->pxMin, pSBCalc->cpx));
	}
	else
	{
		return (pSBCalc->sbd.posMin - 1);
	}
}

/***************************************************************************\
* InvertScrollHilite
*
*
*
* History:
\***************************************************************************/

void InvertScrollHilite(
	HWND pwnd,
	PSBTRACK pSBTrack)
{
	HDC hdc;

	/*
	 * Don't invert if the thumb is all the way at the top or bottom
	 * or you will end up inverting the line between the arrow and the thumb.
	 */
	if (!IsRectEmpty(&pSBTrack->rcTrack))
	{
		if (pSBTrack->fTrackRecalc)
		{
			RecalcTrackRect(pSBTrack);
			pSBTrack->fTrackRecalc = FALSE;
		}

		hdc = (HDC)GetWindowDC(pwnd);
		InvertRect(hdc, &pSBTrack->rcTrack);
		ReleaseDC(pwnd, hdc);
	}
}

// 向滚动条绑定的窗口，发送WM_*SCROLL消息
void xxxDoScroll(HWLWND pwnd, HWLWND pNotifyCtrl, int cmd, int pos, BOOL fVert)
{
	// 因为下面的MAKELONG(cmd, pos)会把Track pos剪裁成16位，所以在剪裁前，把它保存起来
	SBWND *pSb = (SBWND *)pwnd->m_pMember;
	if (cmd == SB_THUMBTRACK)
	{
		pSb->SBCalc.nTrackPos = pos;
		pSb->SBCalc.sbd.pos = pos;
	}

	if (pNotifyCtrl != NULL)
	{
		SendMessage(pNotifyCtrl, (UINT)(fVert ? WM_VSCROLL : WM_HSCROLL), MAKELONG(cmd, pos), 0);
	}
	else
	{
		::SendMessage(GetHostWnd(pwnd), (UINT)(fVert ? WM_VSCROLL : WM_HSCROLL), MAKELONG(cmd, pos), 0);
	}

	// 常规情况下，滚动条向Host控件发送WM_VSCROLL消息，Host控件收到WM_VSCROLL消息后，
	// 更新滚动位置，并调用滚动条的SetScrollInfo来更新滚动条的滑块位置。
	// 但并不是所有Host控件都会回调滚动条的SetScrollInfo。 对于这类情况，滚动条在发送完WM_SCROLL
	// 消息后，自己刷新自己。
	if (pSb->m_bScrollBarRefreshSelf)
	{
		InvalidateRect(pwnd, NULL);
	}
}

// -------------------------------------------------------------------------
//
//  CheckScrollRecalc()
//
// -------------------------------------------------------------------------
//void CheckScrollRecalc(HWND pwnd, PSBSTATE pSBState, PSBCALC pSBCalc)
//{
//    if ((pSBState->pwndCalc != pwnd) || ((pSBState->nBar != SB_CTL) && (pSBState->nBar != ((pSBState->fVertSB) ? SB_VERT : SB_HORZ))))
//    {
//        // Calculate SB stuff based on whether it's a control or in a window
//        if (pSBState->fCtlSB)
//            SBCtlSetup((PSBWND) pwnd);
//        else
//            CalcSBStuff(pwnd, pSBCalc, pSBState->fVertSB);
//    }
//}


// 拖动滑块时，更新滑块坐标，并向绑定的窗口发送WM_*SCRLL消息
// px: 滑块新的top值
// 当点击轨道发生Page Up/Down时，并不会执行到这个函数
void xxxMoveThumb(HWLWND pwnd, PSBCALC  pSBCalc, int px)
{
	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);

	//	CheckLock(pwnd);

	if ((pSBTrack == NULL) || (px == pSBTrack->pxOld))
	{
		return;
	}

pxReCalc:

	pSBTrack->posNew = SBPosFromPx(pSBCalc, px);

	/* Tentative position changed -- notify the guy. */
	if (pSBTrack->posNew != pSBTrack->posOld)
	{
		xxxDoScroll(pSBTrack->spwndSB, pSBTrack->spwndSBNotify,
			SB_THUMBTRACK, pSBTrack->posNew, pSBTrack->fTrackVert);

		pSBTrack->posOld = pSBTrack->posNew;

		//
		// Anything can happen after the SendMessage above!
		// Make sure that the SBINFO structure contains data for the
		// window being tracked -- if not, recalculate data in SBINFO
		//
		//        CheckScrollRecalc(pwnd, pSBState, pSBCalc);
		// when we yield, our range can get messed with
		// so make sure we handle this

		if (px >= pSBCalc->pxMin + pSBCalc->cpx)
		{
			px = pSBCalc->pxMin + pSBCalc->cpx;
			goto pxReCalc;
		}

	}

	pSBCalc->pxThumbTop = px;
	pSBCalc->pxThumbBottom = pSBCalc->pxThumbTop + pSBCalc->cpxThumb;

	InvalidateRect(pwnd, NULL);

	pSBTrack->pxOld = px;
}

/***************************************************************************\
* zzzDrawInvertScrollArea
*
*
*
* History:
\***************************************************************************/

void zzzDrawInvertScrollArea(
	HWLWND pwnd,
	PSBTRACK pSBTrack,
	BOOL fHit,
	UINT cmd)
{
	//HDC hdc;
	//RECT rcTemp;
	//int cx, cy;
	//UINT bm;

	//if ((cmd != SB_LINEUP) && (cmd != SB_LINEDOWN))
	//{
	//	// not hitting on arrow -- just invert the area and return
	//	InvertScrollHilite(pwnd, pSBTrack);

	//	if (cmd == SB_PAGEUP)
	//	{
	//		if (fHit)
	//		{
	//			SetWF(pwnd, WFPAGEUPBUTTONDOWN);
	//		}
	//		else
	//		{
	//			ClrWF(pwnd, WFPAGEUPBUTTONDOWN);
	//		}
	//	}
	//	else
	//	{
	//		if (fHit)
	//		{
	//			SetWF(pwnd, WFPAGEDNBUTTONDOWN);
	//		}
	//		else
	//		{
	//			ClrWF(pwnd, WFPAGEDNBUTTONDOWN);
	//		}
	//	}

	//	if (FWINABLE())
	//	{
	//		zzzWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd,
	//			(pSBTrack->fCtlSB ? OBJID_CLIENT : (pSBTrack->fTrackVert ? OBJID_VSCROLL : OBJID_HSCROLL)),
	//			((cmd == SB_PAGEUP) ? INDEX_SCROLLBAR_UPPAGE : INDEX_SCROLLBAR_DOWNPAGE),
	//			WEF_USEPWNDTHREAD);
	//		// Note: after zzz, pSBTrack may no longer be valid (but we return now)
	//	}
	//	return;
	//}

	//if (pSBTrack->fTrackRecalc)
	//{
	//	RecalcTrackRect(pSBTrack);
	//	pSBTrack->fTrackRecalc = FALSE;
	//}

	//CopyRect(&rcTemp, &pSBTrack->rcTrack);

	//hdc = _GetWindowDC(pwnd);

	//if (pSBTrack->fTrackVert)
	//{
	//	cx = IUIGetGloabelVar()->g_cxVScroll;
	//	cy = SYSMET(CYVSCROLL);
	//}
	//else
	//{
	//	cx = IUIGetGloabelVar->g_cxHScroll;
	//	cy = IUIGetGloabelVar()->g_cyHScroll;
	//}

	//if ((cx == (rcTemp.right - rcTemp.left)) &&
	//	(cy == (rcTemp.bottom - rcTemp.top)))
	//{
	//	if (cmd == SB_LINEUP)
	//	{
	//		bm = (pSBTrack->fTrackVert) ? OBI_UPARROW : OBI_LFARROW;
	//	}
	//	else // SB_LINEDOWN
	//	{
	//		bm = (pSBTrack->fTrackVert) ? OBI_DNARROW : OBI_RGARROW;
	//	}

	//	if (fHit)
	//	{
	//		bm += DOBI_PUSHED;
	//	}

	//	BitBltSysBmp(hdc, rcTemp.left, rcTemp.top, bm);
	//}
	//else
	//{
	//	DrawFrameControl(hdc, &rcTemp, DFC_SCROLL,
	//		((pSBTrack->fTrackVert) ? DFCS_SCROLLVERT : DFCS_SCROLLHORZ) |
	//		((fHit) ? DFCS_PUSHED | DFCS_FLAT : 0) |
	//		((cmd == SB_LINEUP) ? DFCS_SCROLLMIN : DFCS_SCROLLMAX));
	//}

	//_ReleaseDC(hdc);


	//if (cmd == SB_LINEUP)
	//{
	//	if (fHit)
	//	{
	//		SetWF(pwnd, WFLINEUPBUTTONDOWN);
	//	}
	//	else
	//	{
	//		ClrWF(pwnd, WFLINEUPBUTTONDOWN);
	//	}
	//}
	//else
	//{
	//	if (fHit)
	//	{
	//		SetWF(pwnd, WFLINEDNBUTTONDOWN);
	//	}
	//	else
	//	{
	//		ClrWF(pwnd, WFLINEDNBUTTONDOWN);
	//	}
	//}

	//if (FWINABLE())
	//{
	//	zzzWindowEvent(EVENT_OBJECT_STATECHANGE, pwnd,
	//		(pSBTrack->fCtlSB ? OBJID_CLIENT : (pSBTrack->fTrackVert ? OBJID_VSCROLL : OBJID_HSCROLL)),
	//		(cmd == SB_LINEUP ? INDEX_SCROLLBAR_UP : INDEX_SCROLLBAR_DOWN),
	//		WEF_USEPWNDTHREAD);
	//	// Note: after zzz, pSBTrack may no longer be valid (but we return now)
	//}
}

/***************************************************************************\
* xxxEndScroll
*
*
*
* History:
\***************************************************************************/

void xxxEndScroll(
	HWLWND pwnd,
	BOOL fCancel)
{
	UINT oldcmd;
	// 	CheckLock(pwnd);
	// 	UserAssert(!IsWinEventNotifyDeferred());

	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);
	if (NULL != pSBTrack
		&& IUIGetThreadState()->m_pWLCapture == pwnd
		&& NULL != pSBTrack->xxxpfnSB)
	{
		oldcmd = pSBTrack->cmdSB;
		pSBTrack->cmdSB = 0;
		ReleaseCapture(pwnd);
		SBWND *pSb = (SBWND *)GetMember(pwnd);
		pSb->m_bRecordLButtonDown = FALSE;

		CIUIRect rcWnd;
		GetClientRect(pwnd, rcWnd);

		CIUIPoint point;
		GetCursorPos(&point);
		ScreenToClient(pwnd, &point);

		if (!rcWnd.PtInRect(point))
		{
			SetControlState(pwnd, CONTROL_STATE_NORMAL);
		}
		else
		{
			SetControlState(pwnd, CONTROL_STATE_HOVER);
		}

		InvalidateRect(pwnd, NULL);

		// After xxxReleaseCapture, revalidate pSBTrack
		pSBTrack = SB_GetPSBTRACK(pwnd);
		if (pSBTrack == NULL)
		{
			return;
		}

		if (pSBTrack->xxxpfnSB == xxxTrackThumb)
		{

			if (fCancel)
			{
				pSBTrack->posOld = pSBTrack->pSBCalc->sbd.pos;
			}

			/*
			 * DoScroll does thread locking on these two pwnds -
			 * this is ok since they are not used after this
			 * call.
			 */
			xxxDoScroll(pSBTrack->spwndSB, pSBTrack->spwndSBNotify,
				SB_THUMBPOSITION, pSBTrack->posOld, pSBTrack->fTrackVert);

			DrawCtlThumb((PSBWND)pwnd->m_pMember);
		}
		else if (pSBTrack->xxxpfnSB == xxxTrackBox)
		{
			DWORD lParam;
			POINT ptMsg;

			if (pSBTrack->hTimerSB != 0)
			{
				KillTimer(pwnd, IDSYS_SCROLL);
				pSBTrack->hTimerSB = 0;
			}
			lParam = GetMessagePos();

			CIUIRect rcWindow;
			GetWindowRect(pwnd, rcWindow);
#ifdef USE_MIRRORING
			if (TestWF(pwnd, WEFLAYOUTRTL))
			{
				ptMsg.x = rcWindow.right - GET_X_LPARAM(lParam);
			}
			else
#endif
			{
				ptMsg.x = GET_X_LPARAM(lParam) - rcWindow.left;
			}
			ptMsg.y = GET_Y_LPARAM(lParam) - rcWindow.top;
			if (PtInRect(&pSBTrack->rcTrack, ptMsg))
			{
				zzzDrawInvertScrollArea(pwnd, pSBTrack, FALSE, oldcmd);
				// Note: after zzz, pSBTrack may no longer be valid
			}
		}

		/*
		 * Always send SB_ENDSCROLL message.
		 *
		 * DoScroll does thread locking on these two pwnds -
		 * this is ok since they are not used after this
		 * call.
		 */

		// After xxxDrawThumb or zzzDrawInvertScrollArea, revalidate pSBTrack
		pSBTrack = SB_GetPSBTRACK(pwnd);
		if (pSBTrack == NULL)
		{
			return;
		}

		xxxDoScroll(pSBTrack->spwndSB, pSBTrack->spwndSBNotify,
			SB_ENDSCROLL, 0, pSBTrack->fTrackVert);

		// 		ClrWF(pwnd, WFSCROLLBUTTONDOWN);
		// 		ClrWF(pwnd, WFVERTSCROLLTRACK);

		// 		if (FWINABLE())
		// 		{
		// 			xxxWindowEvent(EVENT_SYSTEM_SCROLLINGEND, pwnd,
		// 				(pSBTrack->fCtlSB ? OBJID_CLIENT :
		// 					(pSBTrack->fTrackVert ? OBJID_VSCROLL : OBJID_HSCROLL)),
		// 				INDEXID_CONTAINER, 0);
		// 			// After xxxWindowEvent, revalidate pSBTrack
		// 			pSBTrack = SB_GetPSBTRACK(pwnd);
		// 			if (pSBTrack == NULL)
		// 			{
		// 				return;
		// 			}
		// 		}

		/*
		 * If this is a Scroll Bar Control, turn the caret back on.
		 */
		if (pSBTrack->spwndSB != NULL)
		{
			//ShowCaret(pSBTrack->spwndSB);
			// After zzz, revalidate pSBTrack
			pSBTrack = SB_GetPSBTRACK(pwnd);
			if (pSBTrack == NULL)
			{
				return;
			}
		}


		pSBTrack->xxxpfnSB = NULL;
	}
}


// 滚动条timer的回调，当按住上下按钮或上下轨道不释放鼠标时，要持续发送滚动消息。
VOID CALLBACK xxxContScroll(
	HWLWND pwnd,
	UINT message,
	UINT_PTR ID,
	DWORD lParam)
{
	LONG pt;
	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);

	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(lParam);

	if (pSBTrack == NULL)
	{
		return;
	}

	//CheckLock(pwnd);

	pt = GetMessagePos();

	CIUIRect rcWindow;
	GetWindowRect(pwnd, rcWindow);
#ifdef USE_MIRRORING
	if (TestWF(pwnd, WEFLAYOUTRTL))
	{
		pt = MAKELONG(rcWindow.right - GET_X_LPARAM(pt), GET_Y_LPARAM(pt) - rcWindow.top);
	}
	else
#endif
	{
		pt = MAKELONG(GET_X_LPARAM(pt) - rcWindow.left, GET_Y_LPARAM(pt) - rcWindow.top);
	}
	xxxTrackBox(pwnd, WM_NULL, 0, pt, NULL);
	// After xxxTrackBox, revalidate pSBTrack
	pSBTrack = SB_GetPSBTRACK(pwnd);
	if (pSBTrack == NULL)
	{
		return;
	}

	// 当鼠标在上下按钮中按下并且没有释放，当鼠标离开上下按钮，pSBTrack->fHitOld应该为false;
	// 当鼠标在下轨道按下并且没有释放，当滑块到过鼠标位置时，pSBTrack->fHitOld应该为false，
	// pSBTrack->fHitOld为false时，停止滚动，但timer仍然在运行，当鼠标回到上下按钮或在下
	// 轨道中往下移动一些，将会继续滚动
	if (pSBTrack->fHitOld)
	{
		// 先杀掉原来的长Timer，再启新的短Timer
		KillTimer(pwnd, IDSYS_SCROLL);

		pSBTrack->hTimerSB = SetTimer(pwnd, IDSYS_SCROLL, g_dtScroll / 8, xxxContScroll);

		/*
		 * DoScroll does thread locking on these two pwnds -
		 * this is ok since they are not used after this
		 * call.
		 */
		// 持续发送WM_*SCROLL消息
		xxxDoScroll(pSBTrack->spwndSB, pSBTrack->spwndSBNotify,
			pSBTrack->cmdSB, 0, pSBTrack->fTrackVert);
		// Note: after xxx, pSBTrack may no longer be valid (but we return now)
	}
	else
	{
		// 使用鼠标Line up/down时鼠标移出了上下按钮;
		// 或Page up/down时，鼠标移入了滑块
	}

	return;
}

// 在上下按钮或上下轨道中点击了鼠标，本函数是回调函数
void xxxTrackBox(
	HWLWND pwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	PSBCALC pSBCalc)
{
	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);
	int cmsTimer;

	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(pSBCalc);

	//CheckLock(pwnd);
	//UserAssert(IsWinEventNotifyDeferredOK());

	if (pSBTrack == NULL)
	{
		return;
	}

	if (message != WM_NULL && HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
	{
		return;
	}

	// lParam已是转过化的相对ScrollBar的坐标
	POINT ptHit;
	ptHit.x = GET_X_LPARAM(lParam);
	ptHit.y = GET_Y_LPARAM(lParam);

	// 当鼠标不在rcTrack中时，fHit返回FALSE。 滚动Timer的回调xxxContScroll中
	// 判断pSBTrack->fHitOld为FALSE后，将暂停滚动，直到鼠标重新移入rcTrack内。
	// 所以，在点击Track后，需要不断计算rcTrack
	RecalcTrackRect(pSBTrack);
	BOOL fHit = PtInRect(&pSBTrack->rcTrack, ptHit);

	if (fHit != (BOOL)pSBTrack->fHitOld)
	{
		zzzDrawInvertScrollArea(pwnd, pSBTrack, fHit, pSBTrack->cmdSB);
		// After zzz, pSBTrack may no longer be valid
		pSBTrack = SB_GetPSBTRACK(pwnd);
		if (pSBTrack == NULL)
		{
			return;
		}
	}

	cmsTimer = g_dtScroll / 8;

	switch (message)
	{
	case WM_LBUTTONUP:
		xxxEndScroll(pwnd, FALSE);
		// Note: after xxx, pSBTrack may no longer be valid
		break;

	case WM_LBUTTONDOWN:
		pSBTrack->hTimerSB = 0;
		cmsTimer = g_dtScroll;

	case WM_MOUSEMOVE:
		if (fHit && fHit != (BOOL)pSBTrack->fHitOld)
		{
			// 如果按住轨道不放，整个滚动过程中，这里只执行一次。
			KillTimer(pwnd, IDSYS_SCROLL);

			/*
			 * We moved back into the normal rectangle: reset timer
			 */
			// 当点击了上下按钮或上下轨道后，应该先滚动一次，400ms后，如果检测到鼠标未释放
			// 则启动一个50ms的定时器，持续发送滚动消息，直到释放鼠标或移出响应区域
			pSBTrack->hTimerSB = SetTimer(pwnd, IDSYS_SCROLL, cmsTimer, xxxContScroll);

			// 先滚动一次
			xxxDoScroll(pwnd, pSBTrack->spwndSBNotify, pSBTrack->cmdSB, 0, pSBTrack->fTrackVert);
			// Note: after xxx, pSBTrack may no longer be valid
		}
	}

	pSBTrack->fHitOld = fHit;
}


// 使用鼠标移动滑块时的回调函数
void xxxTrackThumb(HWLWND pwnd, UINT message,
	WPARAM wParam, LPARAM lParam, PSBCALC pSBCalc)
{
	// 滑块新的Top值
	int px;

	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);
	POINT pt;

	UNREFERENCED_PARAMETER(wParam);

	//CheckLock(pwnd);

	if (HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
	{
		return;
	}

	if (pSBTrack == NULL)
	{
		return;
	}

	// Make sure that the SBINFO structure contains data for the
	// window being tracked -- if not, recalculate data in SBINFO
	//    CheckScrollRecalc(pwnd, pSBState, pSBCalc);
	if (pSBTrack->fTrackRecalc)
	{
		RecalcTrackRect(pSBTrack);
		pSBTrack->fTrackRecalc = FALSE;
	}


	pt.y = GET_Y_LPARAM(lParam);
	pt.x = GET_X_LPARAM(lParam);

	// 当鼠标移出滚动条可响应区域（比滚动条大一圈）后，直接跳回刚开始滚动时的位置
	if (!PtInRect(&pSBTrack->rcTrack, pt))
	{
		px = pSBCalc->pxStart;
	}
	else
	{
		// px为滑块上坐标
		px = (pSBTrack->fTrackVert ? pt.y : pt.x) + pSBTrack->dpxThumb;

		if (px < pSBCalc->pxMin)
		{
			px = pSBCalc->pxMin;
		}
		else if (px >= pSBCalc->pxMin + pSBCalc->cpx)
		{
			px = pSBCalc->pxMin + pSBCalc->cpx;
		}
	}

	xxxMoveThumb(pwnd, pSBCalc, px);

	// 如果通过滚动菜单执行到这里的话，是收不到WM_LBUTTONUP消息的，所以
	// 直接检测鼠标状态
	if (message == WM_LBUTTONUP || GetKeyState(VK_LBUTTON) >= 0)
	{
		xxxEndScroll(pwnd, FALSE);
	}
}

/***************************************************************************\
* xxxSBTrackLoop
* 点击上下按钮或上下轨道后，如果不释放鼠标，进入消息循环
*
*
* History:
\***************************************************************************/

void xxxSBTrackLoop(
	HWLWND pwnd,
	LPARAM lParam,
	PSBCALC pSBCalc)
{
	MSG msg;
	UINT cmd;
	//	PTHREADINFO ptiCurrent;
	VOID (*xxxpfnSB)(HWLWND, UINT, WPARAM, LPARAM, PSBCALC);
	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);

	//	CheckLock(pwnd);
	//	UserAssert(IsWinEventNotifyDeferredOK());

	//	pSBTrack = PWNDTOPSBTRACK(pwnd);

	if ((pSBTrack == NULL) || (NULL == (xxxpfnSB = pSBTrack->xxxpfnSB)))
		// mode cancelled -- exit track loop
	{
		return;
	}

	// 	if (pSBTrack->fTrackVert)
	// 	{
	// 		SetWF(pwnd, WFVERTSCROLLTRACK);
	// 	}

	// 	if (FWINABLE())
	// 	{
	// 		xxxWindowEvent(EVENT_SYSTEM_SCROLLINGSTART, pwnd,
	// 			(pSBTrack->fCtlSB ? OBJID_CLIENT :
	// 				(pSBTrack->fTrackVert ? OBJID_VSCROLL : OBJID_HSCROLL)),
	// 			INDEXID_CONTAINER, 0);
	// 		// Note: after xxx, pSBTrack may no longer be valid
	// 	}

	// 先执行一次操作，如果鼠标未释放，则启动定时器，持续执行相同操作
	(*xxxpfnSB)(pwnd, WM_LBUTTONDOWN, 0, lParam, pSBCalc);
	// Note: after xxx, pSBTrack may no longer be valid

	//ptiCurrent = PtiCurrent();

	while (true)
	{
		if (IUIGetThreadState()->m_pWLCapture != pwnd)
		{
			KillTimer(pwnd, IDSYS_SCROLL);
			break;
		}

		if (!GetMessage(&msg, NULL, 0, 0))
		{
			// Note: after xxx, pSBTrack may no longer be valid
			break;
		}

		if (!CallMsgFilter(&msg, MSGF_SCROLLBAR))
		{
			cmd = msg.message;

			if (msg.hwnd == GetHostWnd(pwnd)
				&& ((cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
					|| (cmd >= WM_KEYFIRST && cmd <= WM_KEYLAST)))
			{
				cmd = SystoChar(cmd, msg.lParam);

				// After xxxWindowEvent, xxxpfnSB, xxxTranslateMessage or
				// xxxDispatchMessage, re-evaluate pSBTrack.
				// REEVALUATE_PSBTRACK(pSBTrack, pwnd, "xxxTrackLoop");
				if ((pSBTrack == NULL) || (NULL == (xxxpfnSB = pSBTrack->xxxpfnSB)))
					// mode cancelled -- exit track loop
				{
					return;
				}

				LPARAM lParam2 = msg.lParam;
				// 如果是鼠标消息，把坐标转成相对于滚动条
				if (cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
				{
					// msg.lParam是相对于Host窗口的坐标
					CIUIPoint pt(msg.lParam);
					ClientToScreen(GetHostWnd(pwnd), &pt);
					ScreenToClient(pwnd, &pt);
					lParam2 = MAKELPARAM(pt.x, pt.y);
				}

				(*xxxpfnSB)(pwnd, cmd, msg.wParam, lParam2, pSBCalc);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}


/***************************************************************************\
* xxxSBTrackInit
*
* History:
\***************************************************************************/

void xxxSBTrackInit(
	HWLWND pwnd,
	LPARAM lParam,
	int curArea,
	UINT uType)
{
	int px;	// 鼠标相对于滚动条的Y坐标
	LPINT pwX;
	LPINT pwY;
	UINT wDisable;     // Scroll bar disable flags;
	PSBCALC pSBCalc;
	RECT rcSB;
	PSBTRACK pSBTrack = SB_GetPSBTRACK(pwnd);
	SBWND *pSb = (SBWND *)pwnd->m_pMember;

	if (pSBTrack != NULL)
	{
		SB_RemovePSBTRACK(pwnd);

		delete pSBTrack;
		pSBTrack = NULL;
	}

	pSBTrack = new SBTRACK;//(PSBTRACK)UserAllocPoolWithQuota(sizeof(*pSBTrack), TAG_SCROLLTRACK);
	if (pSBTrack == NULL)
	{
		return;
	}

	// 把pSBTrack附加到ScrollBar上
	SB_SetPSBTRACK(pwnd, pSBTrack);

	pSBTrack->hTimerSB = 0;
	pSBTrack->fHitOld = FALSE;

	pSBTrack->xxxpfnSB = xxxTrackBox;

	pSBTrack->spwndSBNotify = NULL;
	//Lock(&pSBTrack->spwndTrack, pwnd);
	//PWNDTOPSBTRACK(pwnd) = pSBTrack;

	pSBTrack->spwndSB = pwnd;
	pSBTrack->fTrackVert = pSb->fVert;
	pSBTrack->spwndSBNotify = GetParent(pwnd);
	wDisable = pSb->wDisableFlags;
	pSBCalc = &pSb->SBCalc;
	pSBTrack->nBar = SB_CTL;

	pSBTrack->pSBCalc = pSBCalc;
	/*
	 *  Check if the whole scroll bar is disabled
	 */
	// 	if ((wDisable & SB_DISABLE_MASK) == SB_DISABLE_MASK)
	// 	{
	// 		Unlock(&pSBTrack->spwndSBNotify);
	// 		Unlock(&pSBTrack->spwndSB);
	// 		Unlock(&pSBTrack->spwndTrack);
	// 		UserFreePool(pSBTrack);
	// 		PWNDTOPSBTRACK(pwnd) = NULL;
	// 		return;  // It is a disabled scroll bar; So, do not respond.
	// 	}

	pwX = (LPINT)&rcSB;
	pwY = pwX + 1;
	if (!pSBTrack->fTrackVert)
	{
		pwX = pwY--;
	}

	// lParam已是转化过的相对于Host的坐标
	px = (pSBTrack->fTrackVert ? GET_Y_LPARAM(lParam) : GET_X_LPARAM(lParam));

	// pwX指向rcSB，先把它赋值为整个滚动条大小
	*(pwX + 0) = pSBCalc->pxLeft;
	*(pwY + 0) = pSBCalc->pxTop;
	*(pwX + 2) = pSBCalc->pxRight;
	*(pwY + 2) = pSBCalc->pxBottom;
	pSBTrack->cmdSB = (UINT)(-1);

	if (px < pSBCalc->pxUpArrow)
	{

		// 点击了上按钮; 判断是不是disable状态
		if (wDisable & LTUPFLAG)
		{
			return;         // Yes! disabled. Do not respond.
		}

		// LINEUP -- make rcSB the Up Arrow's Rectangle
		pSBTrack->cmdSB = SB_LINEUP;
		*(pwY + 2) = pSBCalc->pxUpArrow;
	}
	else if (px >= pSBCalc->pxDownArrow)
	{

		// 点击了下按钮; 判断是不是disable状态
		if (wDisable & RTDNFLAG)
		{
			return;// Yes! disabled. Do not respond.
		}

		// LINEDOWN -- make rcSB the Down Arrow's Rectangle
		pSBTrack->cmdSB = SB_LINEDOWN;
		*(pwY + 0) = pSBCalc->pxDownArrow;
	}
	else if (px < pSBCalc->pxThumbTop)
	{
		// PAGEUP -- make rcSB the rectangle between Up Arrow and Thumb
		// 记录上按钮与滑块之间的轨道的坐标
		pSBTrack->cmdSB = SB_PAGEUP;
		*(pwY + 0) = pSBCalc->pxUpArrow;
		*(pwY + 2) = pSBCalc->pxThumbTop;
	}
	else if (px < pSBCalc->pxThumbBottom)
	{

DoThumbPos:

		// 点击到滑块中
		/*
		 * Elevator isn't there if there's no room.
		 */
		if (pSBCalc->pxDownArrow - pSBCalc->pxUpArrow <= pSBCalc->cpxThumb)
		{
			return;
		}
		// THUMBPOSITION -- we're tracking with the thumb
		pSBTrack->cmdSB = SB_THUMBPOSITION;

		// 计算按住滑块滑动时的允许滑块响应的区域
		CalcTrackDragRect(pSBTrack);

		pSBTrack->xxxpfnSB = xxxTrackThumb;

		// 注意下面的算式是连等，而不是相减
		pSBTrack->pxOld = pSBCalc->pxStart = pSBCalc->pxThumbTop;
		pSBTrack->posNew = pSBTrack->posOld = pSBCalc->sbd.pos;

		pSBTrack->dpxThumb = pSBCalc->pxStart - px;

		// 设置捕获
		SetCapture(pwnd);

		// 先向绑定的窗口发送一次发送WM_*SCROLL消息
		xxxDoScroll(pwnd, pSBTrack->spwndSBNotify,
			SB_THUMBTRACK, pSBTrack->posOld, pSBTrack->fTrackVert);
		// Note: after xxx, pSBTrack may no longer be valid
	}
	else if (px < pSBCalc->pxDownArrow)
	{
		// PAGEDOWN -- make rcSB the rectangle between Thumb and Down Arrow
		// 记录滑块与下按钮之间的轨道的坐标
		pSBTrack->cmdSB = SB_PAGEDOWN;
		*(pwY + 0) = pSBCalc->pxThumbBottom;
		*(pwY + 2) = pSBCalc->pxDownArrow;
	}

	// 如果按住shift点击了滚动条中除了上下按钮之外的区域，那么滑块的中心点，直接滚动到鼠标点击处
	if ((uType == SCROLL_DIRECT && pSBTrack->cmdSB != SB_LINEUP && pSBTrack->cmdSB != SB_LINEDOWN) ||
		(uType == SCROLL_MENU))
	{
		if (pSBTrack->cmdSB != SB_THUMBPOSITION)
		{
			goto DoThumbPos;
		}
		pSBTrack->dpxThumb = -(pSBCalc->cpxThumb / 2);
	}

	SetCapture(pwnd);
	// After xxxCapture, revalidate pSBTrack
	//RETURN_IF_PSBTRACK_INVALID(pSBTrack, pwnd);

	if (pSBTrack->cmdSB != SB_THUMBPOSITION)
	{
		CopyRect(&pSBTrack->rcTrack, &rcSB);
	}

	// 如果按住上下按钮、轨道或滑块不释放鼠标，应该持续滚动
	xxxSBTrackLoop(pwnd, lParam, pSBCalc);

	// After xxx, re-evaluate pSBTrack
}

/***************************************************************************\
* GetScrollMenu
*
* History:
\***************************************************************************/

HMENU xxxGetScrollMenu(
	HWND pwnd,
	BOOL fVert)
{
	//PMENU pMenu;
	//PMENU *ppDesktopMenu;

	///*
	// * Grab the menu from the desktop.  If the desktop menu
	// * has not been loaded and this is not a system thread,
	// * load it now.  Callbacks cannot be made from a system
	// * thread or when a thread is in cleanup.
	// */
	//if (fVert)
	//{
	//	ppDesktopMenu = &pwnd->head.rpdesk->spmenuVScroll;
	//}
	//else
	//{
	//	ppDesktopMenu = &pwnd->head.rpdesk->spmenuHScroll;
	//}
	//pMenu = *ppDesktopMenu;
	//if (pMenu == NULL && !(PtiCurrent()->TIF_flags & (TIF_SYSTEMTHREAD | TIF_INCLEANUP)))
	//{
	//	UNICODE_STRING strMenuName;

	//	RtlInitUnicodeStringOrId(&strMenuName,
	//		fVert ? MAKEINTRESOURCE(ID_VSCROLLMENU) : MAKEINTRESOURCE(ID_HSCROLLMENU));
	//	pMenu = xxxClientLoadMenu(NULL, &strMenuName);
	//	LockDesktopMenu(ppDesktopMenu, pMenu);
	//}

	///*
	// * Return the handle to the scroll menu.
	// */
	//if (pMenu != NULL)
	//{
	//	return _GetSubMenu(pMenu, 0);
	//}

	return NULL;
}

/***************************************************************************\
* xxxDoScrollMenu
*
* History:
\***************************************************************************/

VOID
xxxDoScrollMenu(
	HWND pwndNotify,
	HWND pwndSB,
	BOOL fVert,
	LPARAM lParam)
{
	//PMENU pMenu;
	//SBCALC SBCalc, *pSBCalc;
	//UINT cmd;
	//POINT pt;
	//TL tlpmenu;
	//UINT wDisable;

	///*
	// * Check the compatibility flag.  Word 6.0 AV's when selecting an item
	// * in this menu.
	// * NOTE: If this hack is to be extended for other apps we should use
	// * another bit for GACF_NOSCROLLBARCTXMENU as the current one is re-used
	// *  MCostea #119380
	// */
	//if (GetAppCompatFlags(NULL) & GACF_NOSCROLLBARCTXMENU)
	//{
	//	return;
	//}

	///*
	// * Initialize some stuff.
	// */
	//POINTSTOPOINT(pt, lParam);
	//if (pwndSB)
	//{
	//	SBCtlSetup((PSBWND)pwndSB);
	//	pSBCalc = &(((PSBWND)pwndSB)->SBCalc);
	//	wDisable = ((PSBWND)pwndSB)->wDisableFlags;
	//	pt.x -= pwndSB->rcWindow.left;
	//	pt.y -= pwndSB->rcWindow.top;
	//}
	//else
	//{
	//	pSBCalc = &SBCalc;
	//	CalcSBStuff(pwndNotify, pSBCalc, fVert);
	//	wDisable = GetWndSBDisableFlags(pwndNotify, fVert);
	//	pt.x -= pwndNotify->rcWindow.left;
	//	pt.y -= pwndNotify->rcWindow.top;
	//}

	///*
	// * Make sure the scrollbar isn't disabled.
	// */
	//if ((wDisable & SB_DISABLE_MASK) == SB_DISABLE_MASK)
	//{
	//	return;
	//}

	///*
	// * Put up a menu and scroll accordingly.
	// */
	//if ((pMenu = xxxGetScrollMenu(pwndNotify, fVert)) != NULL)
	//{
	//	ThreadLockAlways(pMenu, &tlpmenu);
	//	cmd = xxxTrackPopupMenuEx(pMenu,
	//			TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
	//			GET_X_LPARAM(lParam),
	//			GET_Y_LPARAM(lParam),
	//			pwndNotify,
	//			NULL);
	//	ThreadUnlock(&tlpmenu);
	//	if (cmd)
	//	{
	//		if ((cmd & 0x00FF) == SB_THUMBPOSITION)
	//		{
	//			if (pwndSB)
	//			{
	//				xxxSBTrackInit(pwndSB, MAKELPARAM(pt.x, pt.y), 0, SCROLL_MENU);
	//			}
	//			else
	//			{
	//				xxxSBTrackInit(pwndNotify, lParam, fVert ? HTVSCROLL : HTHSCROLL, SCROLL_MENU);
	//			}
	//		}
	//		else
	//		{
	//			xxxDoScroll(pwndSB,
	//				pwndNotify,
	//				cmd & 0x00FF,
	//				0,
	//				fVert
	//			);
	//			xxxDoScroll(pwndSB,
	//				pwndNotify,
	//				SB_ENDSCROLL,
	//				0,
	//				fVert
	//			);
	//		}
	//	}
	//}
}

LRESULT WINAPI IUI::SB_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	LONG lres;
	int cx;
	int cy;
	UINT cmd;
	//	UINT uSide;
	HDC hdc = NULL;
	RECT rc = { 0, 0, 0, 0 };
	//	POINT pt;
	//	BOOL fSizeReal;
	HBRUSH hbrSave;
	BOOL fSize;
	//	PAINTSTRUCT ps;
	//TL tlpwndParent;
	SCROLLINFO      si;
	LPSCROLLINFO    lpsi = &si;
	BOOL            fRedraw = FALSE;
	BOOL            fScroll;
	SBWND *pSb = (SBWND *)GetMember(hWnd);

	//VALIDATECLASSANDSIZE(((HWND)psbwnd), message, wParam, lParam, FNID_SCROLLBAR, WM_CREATE);

	fSize = false;

	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pSb == NULL)
			{
				if (pe->RoutedEvent == WM_NCCREATE)
				{
					CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;

					// 分配并清空内存
					pSb = new SBWND;
					if (pSb == NULL)
					{
						OutputDebugString(TEXT("ScrollBar: Out of near memory"));
						return 0L;      // fail the window create
					}

					pSb->hwnd = hWnd;
					pSb->hwnd->ci.hwndParent = GetParent(hWnd);
					pSb->hwnd->ci.style = pcs->style;
					pSb->hwnd->ci.dwExStyle = pcs->dwExStyle;

					SetMember(hWnd, pSb);
				}
			}

			switch (pe->RoutedEvent)
			{
			case WM_CREATE:
				pSb->SBCalc.sbd.pos = pSb->SBCalc.sbd.posMin = 0;
				pSb->SBCalc.sbd.posMax = 0;
				pSb->fVert = ((LOBYTE(GetStyle(hWnd)) & SBS_VERT) != 0);
				pSb->SBCalc.sbd.page = 0;
				break;

			case WM_DESTROY:
				pSb->Release();
				delete pSb;
				pSb = NULL;
				SetMember(hWnd, NULL);
				break;

			case WM_SIZE:
				// scroll bar has the focus -- recalc it's thumb caret size
				// no need to DeferWinEventNotify() - see xxxCreateCaret below.
				DestroyCaret();

			//   |             |
			//   |  FALL THRU  |
			//   V             V

			case WM_SETFOCUS:
			{
				CIUIRect rcWindow;
				GetWindowRect(hWnd, rcWindow);
				SBCtlSetup(pSb);

				cx = (pSb->fVert ? rcWindow.Width() : pSb->SBCalc.cpxThumb) - 2 * IUIGetGloabelVar()->g_cxEdge;
				cy = (pSb->fVert ? pSb->SBCalc.cpxThumb : rcWindow.Height()) - 2 * IUIGetGloabelVar()->g_cyEdge;

				//CreateCaret((HWND)psbwnd, (HBITMAP)1, cx, cy);
				zzzSetSBCaretPos(pSb);
				//ShowCaret((HWND)psbwnd);
			}
			break;

			case WM_KILLFOCUS:
				DestroyCaret();
				break;

			case WM_ERASEBKGND:

				/*
				 * Do nothing, but don't let DefWndProc() do it either.
				 * It will be erased when its painted.
				 */
				return (LONG)TRUE;

			case WM_NCHITTEST:
				if (GetStyle(hWnd) & SBS_SIZEGRIP)
				{
#ifdef USE_MIRRORING
					/*
					 * If the scroll bar is RTL mirrored, then
					 * mirror the hittest of the grip location.
					 */
					if (TestWF((HWND)psbwnd, WEFLAYOUTRTL))
					{
						return HTBOTTOMLEFT;
					}
					else
#endif
						return HTBOTTOMRIGHT;
				}
				else
				{
					goto DoDefault;
				}
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			switch (pe->RoutedEvent)
			{
#ifdef COLOR_HOTTRACKING
			case WM_MOUSELEAVE:
				xxxHotTrackSBCtl(psbwnd, 0, FALSE);
				ht = 0;
				break;

			case WM_MOUSEMOVE:
			{
				int ht;

				if (ht == 0)
				{
					TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, HWq(psbwnd), 0};
					TrackMouseEvent(&tme);
				}

				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				ht = HitTestScrollBar((HWND)psbwnd, fVert, pt);
				if (ht != ht)
				{
					xxxHotTrackSBCtl(psbwnd, ht, TRUE);
					ht = ht;
				}
			}
			break;
#endif // COLOR_HOTTRACKING

			case WM_LBUTTONDBLCLK:
				cmd = SC_ZOOM;

			/*
			 *** FALL THRU **
			 */

			case WM_LBUTTONDOWN:
				//
				// Note that SBS_SIZEGRIP guys normally won't ever see button
				// downs.  This is because they return HTBOTTOMRIGHT to
				// WindowHitTest handling.  This will walk up the parent chain
				// to the first sizeable ancestor, bailing out at caption windows
				// of course.  That dude, if he exists, will handle the sizing
				// instead.
				//
				if (!fSize)
				{
					pSb->m_bRecordLButtonDown = TRUE;
					SetControlState(hWnd, CONTROL_STATE_PRESSED);

					if (IsIncludeFlag(GetStyle(hWnd), WS_TABSTOP))
					{
						SetFocus(GetHostWnd(hWnd), hWnd);
					}

					//HideCaret((HWND)psbwnd);
					SBCtlSetup(pSb);

					/*
					 * SBCtlSetup enters SEM_SB, and xxxSBTrackInit leaves it.
					 */
					xxxSBTrackInit(hWnd, lParam, 0, (GetKeyState(VK_SHIFT) < 0) ? SCROLL_DIRECT : SCROLL_NORMAL);
					break;
				}
				break;

			case WM_MOUSEMOVE:
			{
				if (!pSb->m_bRecordLButtonDown)
				{
					if (GetControlState(hWnd) != CONTROL_STATE_HOVER)
					{
						SetControlState(hWnd, CONTROL_STATE_HOVER);

						// when set RGN for highlight stats, show the tool tip when
						// mouse over the button(the tool tip and highlight
						// stats at the same time work).
						// don't call InvalidateRect(hWnd, NULL) after this "if" section
						InvalidateRect(hWnd, NULL);
					}
				}
			}
			break;

			case WM_MOUSELEAVE:
			case WM_NCMOUSELEAVE:
			{
				SetControlState(hWnd, CONTROL_STATE_NORMAL);
				InvalidateRect(hWnd, NULL);
			}
			return 0;


			case WM_KEYUP:
				switch (wParam)
				{
				case VK_HOME:
				case VK_END:
				case VK_PRIOR:
				case VK_NEXT:
				case VK_LEFT:
				case VK_UP:
				case VK_RIGHT:
				case VK_DOWN:

					/*
					 * Send end scroll message when user up clicks on keyboard
					 * scrolling.
					 *
					 * DoScroll does thread locking on these two pwnds -
					 * this is ok since they are not used after this
					 * call.
					 */
					xxxDoScroll(hWnd, GetParent(hWnd), SB_ENDSCROLL, 0, pSb->fVert);
					break;

				default:
					break;
				}
				break;

			case WM_KEYDOWN:
				switch (wParam)
				{
				case VK_HOME:
					wParam = SB_TOP;
					goto KeyScroll;

				case VK_END:
					wParam = SB_BOTTOM;
					goto KeyScroll;

				case VK_PRIOR:
					wParam = SB_PAGEUP;
					goto KeyScroll;

				case VK_NEXT:
					wParam = SB_PAGEDOWN;
					goto KeyScroll;

				case VK_LEFT:
				case VK_UP:
					wParam = SB_LINEUP;
					goto KeyScroll;

				case VK_RIGHT:
				case VK_DOWN:
					wParam = SB_LINEDOWN;
KeyScroll:

					/*
					 * DoScroll does thread locking on these two pwnds -
					 * this is ok since they are not used after this
					 * call.
					 */
					xxxDoScroll(hWnd, GetParent(hWnd), (int)wParam, 0, pSb->fVert);
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (pe->RoutedEvent)
			{
			case WM_PAINT:
			case WM_PRINT:
			case WM_PRINTCLIENT:
				SB_Draw(pSb, (HDC)wParam, LPCRECT(lParam));
				return 0;

			case WM_GETDLGCODE:
				return DLGC_WANTARROWS;

			case WM_CONTEXTMENU:
				//xxxDoScrollMenu(GetHostWnd(), (HWND)psbwnd, fVert, lParam);
				break;

			case SBM_GETPOS:
				return (LONG)pSb->SBCalc.sbd.pos;

			case SBM_GETRANGE:
				*((LPINT)wParam) = pSb->SBCalc.sbd.posMin;
				*((LPINT)lParam) = pSb->SBCalc.sbd.posMax;
				return MAKELRESULT(LOWORD(pSb->SBCalc.sbd.posMin), LOWORD(pSb->SBCalc.sbd.posMax));

			case SBM_GETSCROLLINFO:
				return (LONG)_SBGetParms(SB_CTL, &pSb->SBCalc, (LPSCROLLINFO) lParam);

			case SBM_SETRANGEREDRAW:
				fRedraw = TRUE;

			case SBM_SETRANGE:
				// Save the old values of Min and Max for return value
				si.cbSize = sizeof(si);
				//        si.nMin = LOWORD(lParam);
				//        si.nMax = HIWORD(lParam);
				si.nMin = (int)wParam;
				si.nMax = (int)lParam;
				si.fMask = SIF_RANGE | SIF_RETURNOLDPOS;
				goto SetInfo;

			case SBM_SETPOS:
				fRedraw = (BOOL) lParam;
				si.cbSize = sizeof(si);
				si.fMask = SIF_POS | SIF_RETURNOLDPOS;
				si.nPos  = (int)wParam;
				goto SetInfo;

			case SBM_SETMINTHUMBLENGTH:
				fRedraw = (BOOL) lParam;
				pSb->m_nMinThumbLength = (int)wParam;
				break;

			case SBM_GETMINTHUMBLENGTH:
				return pSb->m_nMinThumbLength;

			case SBM_SETSCROLLINFO:
				lpsi = (LPSCROLLINFO) lParam;
				fRedraw = (BOOL) wParam;
SetInfo:
				fScroll = TRUE;

				// 更新滚动信息，不保存track pos
				if (SBSetParms((PSBDATA)&pSb->SBCalc, lpsi, &fScroll, &lres)
					&& IsWindowEnabled(hWnd))
				{
					//L 			xxxWindowEvent(EVENT_OBJECT_VALUECHANGE, (HWND)psbwnd, OBJID_CLIENT,
					// 				INDEX_SCROLLBAR_SELF, WEF_USEPWNDTHREAD);
				}

				if (!fRedraw)
				{
					return lres;
				}

				InvalidateRect(hWnd, NULL);

				/*
				 ** The following zzzShowCaret() must be done after the DrawThumb2(),
				 ** otherwise this caret will be erased by DrawThumb2() resulting
				 ** in this bug:
				 ** Fix for Bug #9263 --SANKAR-- 02-09-90
				 *
				 */

				/*
				 *********** zzzShowCaret((HWND)psbwnd); ******
				 */

				//L if (/*_FChildVisible((HWND)psbwnd) && */fRedraw)
				//{
				//	UINT    wDisable;
				HBRUSH  hbrUse;

				//	if (!fScroll)
				//	{
				//		fScroll = !(lpsi->fMask & SIF_DISABLENOSCROLL);
				//	}

				//	wDisable = (fScroll) ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH;
				//	EnableScrollBar(wDisable);

				//	hdc = GetWindowDC((HWND)GetHostWnd());
				hbrSave = SelectBrush(hdc, hbrUse = IUIGetGloabelVar()->g_hbrBtnFace);

				//	/*
				//	 * Before we used to only hideshowthumb() if the mesage was
				//	 * not SBM_SETPOS.  I am not sure why but this case was ever
				//	 * needed for win 3.x but on NT it resulted in trashing the border
				//	 * of the scrollbar when the app called SetScrollPos() during
				//	 * scrollbar tracking.  - mikehar 8/26
				//	 */
				DrawThumb2(&pSb->SBCalc, hdc, hbrUse, pSb->fVert,
					/*wDisableFlags*/0);
				SelectBrush(hdc, hbrSave);
				//ReleaseDC(psbwnd->GetHostWnd(), hdc);
				//}

				/*
				 * This zzzShowCaret() has been moved to this place from above
				 * Fix for Bug #9263 --SANKAR-- 02-09-90
				 */
				//L ShowCaret((HWND)psbwnd);
				return lres;

			case SBM_SET_BUTTON_IMAGE:
			{
				BOOL bButton1 = (LOWORD(wParam) == 0);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = LPCTSTR(lParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bButton1)
					{
						return IUISetControlImage(GetProject(hWnd),
								&pSb->m_himgButton1Bk[nStateIndex], lpszImage);
					}
					else
					{
						return IUISetControlImage(GetProject(hWnd),
								&pSb->m_himgButton2Bk[nStateIndex], lpszImage);
					}
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					if (bButton1)
					{
						return IUISetControlImage(GetProject(hWnd),
								&pSb->m_himgCombineButton1Bk, lpszImage);
					}
					else
					{
						return IUISetControlImage(GetProject(hWnd),
								&pSb->m_himgCombineButton2Bk, lpszImage);
					}
				}
			}
			return 0;

			case SBM_GET_BUTTON_IMAGE:
			{
				BOOL bButton1 = (LOWORD(wParam) == 0);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bButton1)
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgButton1Bk[nStateIndex]->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgButton2Bk[nStateIndex]->GetSafeImageName();
					}
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					if (bButton1)
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgCombineButton1Bk->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgCombineButton2Bk->GetSafeImageName();
					}
				}
			}
			return 0;

			case SBM_SET_BUTTON_LENGTH:
			{
				fRedraw = (BOOL)lParam;
				BOOL bButton1 = (LOWORD(wParam) == 0);

				if (bButton1)
				{
					pSb->m_uButton1Length = lParam;
				}
				else
				{
					pSb->m_uButton2Length = lParam;
				}
			}
			return 0;

			case SBM_GET_BUTTON_LENGTH:
			{
				BOOL bButton1 = (LOWORD(wParam) == 0);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bButton1)
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgButton1Bk[nStateIndex]->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgButton2Bk[nStateIndex]->GetSafeImageName();
					}
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					if (bButton1)
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgCombineButton1Bk->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pSb->m_himgCombineButton2Bk->GetSafeImageName();
					}
				}
			}
			return 0;

			case SBM_SET_THUMB_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = LPCTSTR(lParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					return IUISetControlImage(GetProject(hWnd),
							&pSb->m_himgThumbBk[nStateIndex], lpszImage);
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					return IUISetControlImage(GetProject(hWnd),
							&pSb->m_himgCombineThumbBk, lpszImage);
				}
			}
			return 0;

			case SBM_GET_THUMB_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					return (LRESULT)(LPCTSTR)pSb->m_himgThumbBk[nStateIndex]->GetSafeImageName();
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					return (LRESULT)(LPCTSTR)pSb->m_himgCombineThumbBk->GetSafeImageName();
				}
			}
			return 0;

			case SBM_REFRSHSELF:
			{
				BOOL bOld = pSb->m_bScrollBarRefreshSelf;
				pSb->m_bScrollBarRefreshSelf = lParam;
				return bOld;
			}

			case SBM_ISREFRESHSELF:
				return pSb->m_bScrollBarRefreshSelf;

			default:
				break;
			}
		}
	}

DoDefault:

	return DefEventHandler(hWnd, pe);
}

int SB_DrawButton(SBWND *pSb, HDC hDC)
{
	CIUIRect rcClient;
	GetClientRect(pSb->hwnd, rcClient);

	HBRUSH hBrush = CreateSolidBrush(GetScrollButtonBkColorP());
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);

	// 布局两个按钮
	CIUIRect rcLTUP = rcClient;

	if (pSb->fVert)
	{
		// rcLTUP.DeflateRect(pBtnLTUP->GetMargin());
		rcLTUP.bottom = pSb->SBCalc.pxUpArrow;
	}
	else
	{
		rcLTUP.right = pSb->SBCalc.pxUpArrow;
	}
	FillSolidRect(hDC, rcLTUP, pSb->m_crScrollButtonBkN,
		IsUpdateLayeredWindow(GetHostWnd(pSb->hwnd)));

	if (pSb->fVert)
	{
		FormsDrawScrollButtonArrowMark(hDC, rcLTUP, D_TOP, 3);
	}
	else
	{
		FormsDrawScrollButtonArrowMark(hDC, rcLTUP, D_LEFT, 3);
	}

	CIUIRect rcRTDN = rcClient;
	if (pSb->fVert)
	{
		rcRTDN.top = pSb->SBCalc.pxDownArrow;
	}
	else
	{
		rcRTDN.left = pSb->SBCalc.pxDownArrow;
	}
	FillSolidRect(hDC, rcRTDN, pSb->m_crScrollButtonBkN,
		IsUpdateLayeredWindow(GetHostWnd(pSb->hwnd)));

	if (pSb->fVert)
	{
		FormsDrawScrollButtonArrowMark(hDC, rcRTDN, D_BOTTOM, 3);
	}
	else
	{
		FormsDrawScrollButtonArrowMark(hDC, rcRTDN, D_RIGHT, 3);
	}

	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hBrush);

	return 0;
}

// 现代界面，滚动条有这种效果：
// 当鼠标放到滑块上时，不显示上下按钮。 当鼠标移动到上下按钮区域后，才显示。
// 所以，轨道区域应该是完整的滚动条区域，并且早于上下按钮绘制。
int SB_DrawGroove(SBWND *pSb, HDC hDC)
{
	CIUIRect rcClient;
	GetClientRect(pSb->hwnd, rcClient);

	CIUIRect rcGroove = rcClient;

	CONTROL_STATE eButtonState = GetControlState(pSb->hwnd);
	int nStateIndex = GetButtonStateIndex(eButtonState);

	BOOL bDrawImage = FALSE;

	if (pSb->hwnd->ci.m_himgCombineBk)
	{
		pSb->hwnd->ci.m_himgCombineBk->SafeLoadSavedImage();

		if (pSb->hwnd->ci.m_himgCombineBk->GetSafeHBITMAP() != NULL)
		{
			bDrawImage = TRUE;
			IUIPartDrawImage(hDC, rcGroove, pSb->hwnd->ci.m_himgCombineBk,
				IRM_9GRID, pSb->hwnd->ci.m_rcBkImage9GridResizeArg, COMBINEIMAGESIZE4, nStateIndex);
		}
	}
	else
	{
		pSb->m_himgGroove[nStateIndex]->SafeLoadSavedImage();

		if (pSb->m_himgGroove[nStateIndex]->GetSafeHBITMAP() != NULL)
		{
			bDrawImage = TRUE;
			IUIDrawImage(hDC, rcGroove, pSb->m_himgGroove[nStateIndex],
				IRM_9GRID, pSb->hwnd->ci.m_rcBkImage9GridResizeArg);
		}
	}

	if (!bDrawImage)
	{
		FillSolidRect(hDC, rcGroove, pSb->m_crScrollGroove,
			IsUpdateLayeredWindow(GetHostWnd(pSb->hwnd)));
	}

	return 0;
}

int SB_DrawThumb(SBWND *pSb, HDC hDC)
{
	CIUIRect rcClient;
	GetClientRect(pSb->hwnd, rcClient);

	CIUIRect rcThumb = rcClient;

	CONTROL_STATE eButtonState = GetControlState(pSb->hwnd);
	int nStateIndex = GetButtonStateIndex(eButtonState);

	if (pSb->fVert)
	{
		rcThumb.top = pSb->SBCalc.pxThumbTop;
		rcThumb.bottom = pSb->SBCalc.pxThumbBottom;
	}
	else
	{
		rcThumb.left = pSb->SBCalc.pxThumbTop;
		rcThumb.right = pSb->SBCalc.pxThumbBottom;
	}

	// Thumb背景图是否有效，如果无效，使用颜色填充
	BOOL bDrawImage = FALSE;

	// 滑块只使用9宫格式一种缩放模式
	// 当支持DPI时，先9宫格缩放，然后再等比缩放
	CIUIRect rcRisize;
	ScrollBar_GetThumb9GridResizeParam(pSb->hwnd, rcRisize);
	if (pSb->m_himgCombineThumbBk)
	{
		pSb->m_himgCombineThumbBk->SafeLoadSavedImage();

		if (pSb->m_himgCombineThumbBk->GetSafeHBITMAP() != NULL)
		{
			bDrawImage = TRUE;
			IUIPartDrawImage(hDC, rcThumb, pSb->m_himgCombineThumbBk,
				IRM_9GRID, rcRisize, COMBINEIMAGESIZE4, nStateIndex);
		}
	}
	else
	{
		if (pSb->m_himgThumbBk[nStateIndex])
		{
			pSb->m_himgThumbBk[nStateIndex]->SafeLoadSavedImage();

			if (pSb->m_himgThumbBk[nStateIndex]->GetSafeHBITMAP() != NULL)
			{
				bDrawImage = TRUE;
				IUIDrawImage(hDC, rcThumb, pSb->m_himgThumbBk[nStateIndex],
					IRM_9GRID, rcRisize);
			}
		}
	}

	if (!bDrawImage)
	{
		FillSolidRect(hDC, rcThumb, pSb->m_crScrollThumbBkN,
			IsUpdateLayeredWindow(GetHostWnd(pSb->hwnd)));
	}

	return 0;
}

int SB_Draw(SBWND *pSb, HDC hDC, LPCRECT lprcClip)
{
	CIUIRect rcPaint;
	CIUIRect rcClient;
	GetClientRect(pSb->hwnd, rcClient);

	CIUIRect rcClip = lprcClip;
	if (rcClip.IsRectEmpty())
	{
		rcClip = rcClient;
	}
	if (!::IntersectRect(&rcPaint, rcClip, rcClient))
	{
		return 2;
	}

	HDC hDCTarget = NULL;
	HDC hMemDC = NULL;
	HBITMAP hOldBmp = NULL;

	hDCTarget = hDC;

	// 计算按钮和滑块坐标
	SBCtlSetup(pSb);

	BeforePaint(pSb->hwnd, hDCTarget);
	// 绘制轨道
	SB_DrawGroove(pSb, hDCTarget);
	// 绘制上下按钮
	SB_DrawButton(pSb, hDCTarget);
	// 绘制滑块
	SB_DrawThumb(pSb, hDCTarget);

	//OnDrawCustom(hDCTarget);
	//OnDrawBorder(hDCTarget);
	AfterPaint(pSb->hwnd, hDCTarget);

	if (hMemDC != NULL && hOldBmp != NULL)
	{
		BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
			hDCTarget, 0, 0, SRCCOPY);
		::SelectObject(hMemDC, hOldBmp);
		DeleteDC(hMemDC);
	}

	return 0;
}


/**************************** Module Header ********************************\
* Module Name: menu.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Keyboard Accelerator Routines
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

#include "stdafx.h"
#include "Menu.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#pragma hdrstop

PTHREADINFO IUI::ptiCurrent = new THREADINFO;
int IUI::gcyMenuScrollArrow;
int IUI::gcxMenuFontChar;
LASTINPUT IUI::glinp;
POPUPMENU IUI::gpopupMenu;
#define ACCF_FIRSTTICK                  0x00004000
DWORD IUI::gdwPUDFlags = ACCF_FIRSTTICK | PUDF_BEEP | PUDF_ANIMATE;
UINT IUI::guSFWLockCount;
HBRUSH IUI::g_hbr3DSHADOW = ::CreateSolidBrush(RGB(128, 128, 128));
HBRUSH IUI::g_hbr3DHILIGHT = ::CreateSolidBrush(RGB(192, 192, 216));
HBRUSH IUI::g_hbrMenu = ::CreateSolidBrush(RGB(100, 100, 200));
HBRUSH IUI::g_hbrHIGHLIGHTTEXT = ::CreateSolidBrush(RGB(255, 255, 255));
HFONT IUI::ghMenuFont = NULL;
int IUI::gcyMenuFontChar = 24;
HDC IUI::ghdcMem2 = NULL;
int IUI::gcyMenuFontAscent = 26;
int IUI::gcxMenuFontOverhang = 26;
HFONT IUI::ghMenuFontDef = NULL;
int gcyMenuFontExternLeading = 8;

/*
 * Fade animation globals
 */
FADE IUI::gfade;

/***********************************************************************\
* MNGetpItemIndex
*
* 11/19/96  GerardoB  Created
\***********************************************************************/
#if DBG
UINT DBGMNGetpItemIndex(PMENU pmenu, PITEM pitem)
{
	UINT uiPos;
	UserAssert((ULONG_PTR)pitem >= (ULONG_PTR)pmenu->rgItems);
	uiPos = _MNGetpItemIndex(pmenu, pitem);
	UserAssert(uiPos < pmenu->cItems);
	return uiPos;
}
#endif
/**************************************************************************\
* xxxMNDismiss
*
* 12/03/96 GerardoB     Created
\**************************************************************************/
void IUI::xxxMNDismiss(PMENUSTATE pMenuState)
{
	xxxMNCancel(pMenuState, 0, 0, 0);
}

/***************************************************************************\
* MNFadeSelection
*
* 2/5/1998   vadimg          created
\***************************************************************************/

BOOL MNFadeSelection(PMENU pmenu, PITEM pitem)
{
	//HWND pwnd;
	//HDC hdc;
	//RECT rc;
	//PPOPUPMENU ppopup;

	////if (!TestALPHA(SELECTIONFADE))
	////{
	////	return FALSE;
	////}

	///*
	// * Get the window for the currently active popup menu.
	// */
	//if ((ppopup = MNGetPopupFromMenu(pmenu, NULL)) == NULL)
	//{
	//	return FALSE;
	//}

	//if ((pwnd = ppopup->spwndPopupMenu) == NULL)
	//{
	//	return FALSE;
	//}

	//rc.left = pwnd->rcClient.left + pitem->xItem;
	//rc.top = pwnd->rcClient.top + pitem->yItem;
	//rc.right = rc.left + pitem->cxItem;
	//rc.bottom = rc.top + pitem->cyItem;

	///*
	// * Initialize the fade animation and get the DC to draw the selection into.
	// */
	//if ((hdc = CreateFade(NULL, &rc, CMS_SELECTIONFADE, 0)) == NULL)
	//{
	//	return FALSE;
	//}

	///*
	// * Read the current menu selection right from the screen, since the menu
	// * is still visible and it's always on top. In the worst case we could
	// * offset the origin of the DC and call xxxDrawMenuItem, but just reading
	// * from the screen is much faster.
	// */
	//GreBitBlt(hdc, 0, 0, pitem->cxItem, pitem->cyItem, gpDispInfo->hdcScreen,
	//	rc.left, rc.top, SRCCOPY, 0);

	//ShowFade();

	return TRUE;
}

/**************************************************************************\
* xxxMNDismissWithNotify
*
* Generates parameters for WM_COMMAND or WM_SYSCOMMAND message.
*
* 12/03/96 GerardoB     Created
\**************************************************************************/
void xxxMNDismissWithNotify(PMENUSTATE pMenuState, PMENU pmenu, PITEM pitem,
	UINT uPos, LPARAM lParam)
{
	UINT uMsg;
	UINT uCmd;

	if (pMenuState->pGlobalPopupMenu->fIsSysMenu)
	{
		uMsg = WM_SYSCOMMAND;
		uCmd = pitem->wID;
		/* lParam set by caller */
	}
	else if (pMenuState->fNotifyByPos)
	{
		uMsg = WM_MENUCOMMAND;
		uCmd = uPos;
		lParam = (LPARAM)pmenu;
	}
	else
	{
		uMsg = WM_COMMAND;
		uCmd = pitem->wID;
		lParam = 0;
	}

	/*
	 * The menu is about to go away, see if we want to fade out the selection.
	 */
	if (MNFadeSelection(pmenu, pitem))
	{
		//StartFade();
	}

	/*
	 * Dismiss the menu.
	 */
	xxxMNCancel(pMenuState, uMsg, uCmd, lParam);
}

/**************************************************************************\
* MNGetpItem
*
* 11/15/96 GerardoB     Created
\**************************************************************************/
PITEM IUI::MNGetpItem(PPOPUPMENU ppopup, UINT uIndex)
{
	if ((ppopup == NULL)
		|| (uIndex >= ppopup->spmenu->cItems))
	{

		return NULL;
	}

	return ppopup->spmenu->rgItems + uIndex;
}
/***************************************************************************\
* MNSetCapture
*
* History:
* 11/18/96 GerardoB  Created
\***************************************************************************/
void IUI::xxxMNSetCapture(PPOPUPMENU ppopup)
{
	//PTHREADINFO ptiCurrent = PtiCurrent();

	/*
	 * Set the capture and lock it so no one will be able to steal it
	 *  from us.
	 */
	//xxxCapture(ptiCurrent, ppopup->spwndNotify, SCREEN_CAPTURE);
	::SetCapture(ppopup->spwndNotify);
	//UserAssert(ptiCurrent->pq->spwndCapture == ppopup->spwndNotify);
	//ptiCurrent->pq->QF_flags |= QF_CAPTURELOCKED;
	//ptiCurrent->pMenuState->fSetCapture = TRUE;
#if DBG
	/*
	 * Unless we're in the foreground, this menu mode won't go away
	 *  when the user clicks outside the menu. This is because only
	 *  the foreground queue capture sees clicks outside its windows.
	 */
	if (ptiCurrent->pq != gpqForeground)
	{
		RIPMSG0(RIP_WARNING, "xxxMNSetCapture: Menu mode is not in foreground queue");
	}
#endif
}
/***************************************************************************\
* MNReleaseCapture
*
* History:
* 11/18/96 GerardoB  Created
\***************************************************************************/
void IUI::xxxMNReleaseCapture(void)
{
	//PTHREADINFO ptiCurrent = PtiCurrent();

	/*
	 * Bail if we didn't set capture
	 */
	//if ((ptiCurrent->pMenuState == NULL) ||
	//	(! ptiCurrent->pMenuState->fSetCapture))
	//{
	//	return;
	//}
	//ptiCurrent->pMenuState->fSetCapture = FALSE;

	/*
	 * Unlock capture and release it.
	 */
	//PtiCurrent()->pq->QF_flags &= ~QF_CAPTURELOCKED;
	::ReleaseCapture();
}
/***************************************************************************\
* MNCheckButtonDownState
*
* History:
* 11/14/96 GerardoB  Created
\***************************************************************************/
void IUI::MNCheckButtonDownState(PMENUSTATE pMenuState)
{
	/*
	 * Modeless menus don't capture the mouse so when a mouse down
	 *  goes off the window, we need to keep watching its state.
	 * We also might not see the button up when going on DoDragDrop loop
	 */
	UserAssert(pMenuState->fDragAndDrop || pMenuState->fModelessMenu);
	pMenuState->fButtonDown = ((GetKeyState(pMenuState->vkButtonDown) & 0x8000) != 0);
	if (!pMenuState->fButtonDown)
	{
		pMenuState->fDragging =
			pMenuState->fIgnoreButtonUp = FALSE;
		UnlockMFMWFPWindow(&pMenuState->uButtonDownHitArea);
	}
}
/***************************************************************************\
* GetMenuStateWindow
*
* This function is called when we need to post a message to the menu loop.
* The actual pwnd is not important since we just want to reach
*  xxxHandleMenuMessages or xxxMenuWindowProc. So we just pick a window that
*  has a good chance to be around as long as we are in menu mode.
*
* History:
* 10/31/96 GerardoB  Created
\***************************************************************************/
HWND IUI::GetMenuStateWindow(PMENUSTATE pMenuState)
{
	if (pMenuState == NULL)
	{
		return NULL;
	}
	else if (pMenuState->pGlobalPopupMenu->fIsTrackPopup)
	{
		return pMenuState->pGlobalPopupMenu->spwndPopupMenu;
	}
	else if (pMenuState->pGlobalPopupMenu->spwndNextPopup != NULL)
	{
		return pMenuState->pGlobalPopupMenu->spwndNextPopup;
	}
	else
	{
		return pMenuState->pGlobalPopupMenu->spwndActivePopup;
	}
}
/***************************************************************************\
* UnlockPopupMenuWindow
*
* This function is called when locking/unlocking a menu into a popup structure.
* It makes sure that pmenu doesn't keep the notification window locked
*  unneccessarily
*
* It unlocks pmenu->spwndNotify if the menu it's not locked into pmenu->spwndNotify
*  itself AND it's currently locked to pwnd.
* It's also unlocked if pmenu->spwndNotify is marked as destroyed
*
* History:
* 10/15/96 GerardoB  Created
\***************************************************************************/
void UnlockPopupMenuWindow(PMENU pmenu, HWND pwnd)
{
	/*
	 * Bail if there's nothing to unlock
	 */
	if ((pmenu == NULL)
		|| (pmenu->spwndNotify == NULL))
	{
		return;
	}
	/*
	 * if pmenu->spwndNotify owns the menu, bail
	 */
	//if ((pmenu == pmenu->spwndNotify->spmenu)
	//	|| (pmenu == pmenu->spwndNotify->spmenuSys))
	//{
	//	return;
	//}
	/*
	 * If pwnd doesn't own the menu, and pmenu->spwndNotify is not destroyed, bail
	 */
	//if ((pwnd != pmenu->spwndNotify)
	//	&& !TestWF(pmenu->spwndNotify, WFDESTROYED))
	//{
	//	return;
	//}
	/*
	 * Unlock it
	 */
	Unlock(&pmenu->spwndNotify);
	return;
}
/***************************************************************************\
* LockPopupMenu
*
* Locks a given menu into a popup strucuture and makes the
*  popup's notification window the owner of the menu
*
* History:
* 10/15/96 GerardoB  Created
\***************************************************************************/
PVOID IUI::LockPopupMenu(PPOPUPMENU ppopup, PMENU *pspmenu, PMENU pmenu)
{
	/*
	 * If you hit this assertion, you're probably not passing the right thing
	 */
	UserAssert((pspmenu == &ppopup->spmenu) || (pspmenu == &ppopup->spmenuAlternate));
	Validateppopupmenu(ppopup);
	/*
	 * This won't work properly if the popup hasn't locked the notification
	 *  window.
	 */
	UserAssert(ppopup->spwndNotify != NULL);

	/*
	 * When using modeless menus, menus can be shared by several active popups.
	 * If the menu has owner draw items, the app better knows how to draw them
	 *  correctly. This shouldn't happen with modal menus though
	 */
#if DBG
	if ((*pspmenu != NULL)
		&& ((*pspmenu)->spwndNotify != NULL)
		&& ((*pspmenu)->spwndNotify != ppopup->spwndNotify))
	{

		RIPMSG3(RIP_WARNING, "LockPopupMenu: Current Menu %#p shared by %#p and %#p",
			*pspmenu, (*pspmenu)->spwndNotify, ppopup->spwndNotify);
	}
#endif

	/*
	 * Unlock the current's menu spwndNotify if needed
	 */
	UnlockPopupMenuWindow(*pspmenu, ppopup->spwndNotify);

	/*
	 * Lock the notification window into the menu structure
	 */
	if (pmenu != NULL)
	{

		/*
		 * Display a warning if this menu is being shared
		 */
#if DBG
		if ((pmenu->spwndNotify != NULL)
			&& (pmenu->spwndNotify != ppopup->spwndNotify)
			&& (pmenu != pmenu->spwndNotify->head.rpdesk->spmenuDialogSys))
		{

			RIPMSG3(RIP_WARNING, "LockPopupMenu: New Menu %#p shared by %#p and %#p",
				pmenu, pmenu->spwndNotify, ppopup->spwndNotify);
		}
#endif

		/*
		 * spwndNotify "owns" this menu now.
		 */
		Lock(&pmenu->spwndNotify, ppopup->spwndNotify);
	}

	/*
	 * Lock the menu into the popup structure (unlock the previous one)
	 */
	return Lock(pspmenu, pmenu);
}
/***************************************************************************\
* UnlockPopupMenu
*
* Unlocks a given menu from a popup strucuture and makes sure that the
*  menu is no longer "owned" by the popup's notification window; if needed
*
* History:
* 10/15/96 GerardoB  Created
\***************************************************************************/
PVOID IUI::UnlockPopupMenu(PPOPUPMENU ppopup, PMENU *pspmenu)
{
	/*
	 * If you hit this assertion, you're probably not passing the right thing
	 */
	UserAssert((pspmenu == &ppopup->spmenu) || (pspmenu == &ppopup->spmenuAlternate));
	/*
	 * If nothing is locked, bail.
	 */
	if (*pspmenu == NULL)
	{
		return NULL;
	}

	/*
	 * This won't work properly if the popup already unlocked the notification
	 *  window. However, this can happen with the root popup if the
	 *  notification window gets destroyed while in menu mode.
	 */
	UserAssert((ppopup->spwndNotify != NULL) || IsRootPopupMenu(ppopup));

	/*
	 * When using modeless menus, menus can be shared by several active
	 *  popups/notification windows. If the menu has owner draw items,
	 *  the app better knows how to paint them right. It shouldn't
	 *  happen with modal menus though.
	 */
#if DBG
	if (((*pspmenu)->spwndNotify != NULL)
		&& (ppopup->spwndNotify != NULL)
		&& (ppopup->spwndNotify != (*pspmenu)->spwndNotify))
	{

		RIPMSG3(RIP_WARNING, "UnlockPopupMenu: Menu %#p shared by %#p and %#p",
			*pspmenu, (*pspmenu)->spwndNotify, ppopup->spwndNotify);
	}
#endif

	/*
	 * Unlock the menu's spwndNotify if needed
	 */
	UnlockPopupMenuWindow(*pspmenu, ppopup->spwndNotify);

	/*
	 * Unlock the menu from the popup structure
	 */
	Unlock(pspmenu);
	return pspmenu;
}
/***************************************************************************\
* LockWndMenu
*
* Locks a given menu into a window structure and locks the window into
*  the menu strucuture.
*
* History:
* 10/15/96 GerardoB  Created
\***************************************************************************/
PVOID LockWndMenu(HWND pwnd, PMENU *pspmenu, PMENU pmenu)
{
	/*
	 * If you hit this assertion, you're probably not passing the right thing
	 */
	//UserAssert((pspmenu == &pwnd->spmenu) || (pspmenu == &pwnd->spmenuSys));

	/*
	 * If the current menu is owned by this window, unlock it
	 */
	if ((*pspmenu != NULL) && ((*pspmenu)->spwndNotify == pwnd))
	{
		Unlock(&((*pspmenu)->spwndNotify));
	}

	/*
	 * If nobody owns the new menu, make this window the owner
	 */
	if ((pmenu != NULL) && (pmenu->spwndNotify == NULL))
	{
		Lock(&pmenu->spwndNotify, pwnd);
	}

	/*
	 * Lock the menu into the window structure (unlock the previous menu)
	 */
	return Lock(pspmenu, pmenu);

}
/***************************************************************************\
* UnlockWndMenu
*
* Unlocks a given menu from a window strucutre and the window from the
*  menu strucuture
*
* History:
* 10/15/96 GerardoB  Created
\***************************************************************************/
PVOID UnlockWndMenu(HWND pwnd, PMENU *pspmenu)
{
	/*
	 * If you hit this assertion, you're probably not passing the right thing
	 */
	//UserAssert((pspmenu == &pwnd->spmenu) || (pspmenu == &pwnd->spmenuSys));

	/*
	 * If nothing is locked, bail
	 */
	if (*pspmenu == NULL)
	{
		return NULL;
	}

	/*
	 * If this window owns the menu, unlock it from the menu strucutre
	 */
	if (pwnd == (*pspmenu)->spwndNotify)
	{
		Unlock(&((*pspmenu)->spwndNotify));
	}

	/*
	 * Unlock the menu from the window structure
	 */
	Unlock(pspmenu);
	return pspmenu;
}


/***************************************************************************\
* MNSetTop
*
*  sets the first visible item in a scrollable menu to the given iNewTop;
*  returns TRUE if iTop was changed; FALSE if iNewTop is already the
*  first visible item
*
* 08/13/96 GerardoB Ported From Memphis.
\***************************************************************************/
BOOL IUI::xxxMNSetTop(PPOPUPMENU ppopup, int iNewTop)
{
	PMENU   pMenu = ppopup->spmenu;
	int     dy;

	if (iNewTop < 0)
	{
		iNewTop = 0;
	}
	else if (iNewTop > pMenu->iMaxTop)
	{
		iNewTop = pMenu->iMaxTop;
	}

	/*
	 * If no change, done
	 */
	if (iNewTop == pMenu->iTop)
	{
		return FALSE;
	}

#if DBG
	/*
	 * We're going to scroll, so validate iMaxTop, cyMax and cyMenu
	 */
	UserAssert((pMenu->cyMax == 0) || (pMenu->cyMax >= pMenu->cyMenu));
	if ((UINT)pMenu->iMaxTop < pMenu->cItems)
	{
		PITEM pitemLast = pMenu->rgItems + pMenu->cItems - 1;
		PITEM pitemMaxTop = pMenu->rgItems + pMenu->iMaxTop;
		UINT uHeight = pitemLast->yItem + pitemLast->cyItem - pitemMaxTop->yItem;
		UserAssert(uHeight <= pMenu->cyMenu);
		/*
		 * Let's guess a max item height
		 */
		UserAssert(pMenu->cyMenu - uHeight <= 2 * pitemLast->cyItem);
	}
	else
	{
		UserAssert((UINT)pMenu->iMaxTop < pMenu->cItems);
	}
#endif


	/*
	 * if we've made it this far, the new iTop WILL change -- thus if the
	 * current iTop is at the top it won't be after this change -- same goes
	 * for iTop at the bottom
	 */
	if (pMenu->dwArrowsOn == MSA_ATTOP)
	{
		pMenu->dwArrowsOn = MSA_ON;
		if (pMenu->hbrBack == NULL)
		{
			//MNDrawArrow(NULL, ppopup, MFMWFP_UPARROW);
		}
	}
	else if (pMenu->dwArrowsOn == MSA_ATBOTTOM)
	{
		pMenu->dwArrowsOn = MSA_ON;
		if (pMenu->hbrBack == NULL)
		{
			//MNDrawArrow(NULL, ppopup, MFMWFP_DOWNARROW);
		}
	}

	UserAssert((UINT)iNewTop < pMenu->cItems);
	dy = MNGetToppItem(pMenu)->yItem - (pMenu->rgItems + iNewTop)->yItem;

	if ((dy > 0 ? dy : -dy) > (int)pMenu->cyMenu)
	{
		::InvalidateRect(ppopup->spwndPopupMenu, NULL, TRUE);
	}
	else
	{
		ScrollWindowEx(ppopup->spwndPopupMenu, 0, dy, NULL, NULL, NULL, NULL, SW_INVALIDATE | SW_ERASE);
	}

	pMenu->iTop = iNewTop;

	if (iNewTop == 0)
	{
		pMenu->dwArrowsOn = MSA_ATTOP;
		if (pMenu->hbrBack == NULL)
		{
			//MNDrawArrow(NULL, ppopup, MFMWFP_UPARROW);
		}
	}
	else if (iNewTop == pMenu->iMaxTop)
	{
		pMenu->dwArrowsOn = MSA_ATBOTTOM;
		if (pMenu->hbrBack == NULL)
		{
			//MNDrawArrow(NULL, ppopup, MFMWFP_DOWNARROW);
		}
	}

	if (pMenu->hbrBack != NULL)
	{
		//MNDrawFullNC(ppopup->spwndPopupMenu, NULL, ppopup);
	}

	return TRUE;

}

/***************************************************************************\
* xxxMNDoScroll
*
*  scrolls a scrollable menu (ppopup) if the given position (uArrow) is one of
*  the menu scroll arrows and sets a timer to auto-scroll when necessary;
*  returns FALSE if the given position was not a menu scroll arrow; returns
*  TRUE otherwise
*
* 08/13/96 GerardoB Ported From Memphis.
\***************************************************************************/
BOOL xxxMNDoScroll(PPOPUPMENU ppopup, UINT uArrow, BOOL fSetTimer)
{
	int iScrollTop = ppopup->spmenu->iTop;

	if (uArrow == MFMWFP_UPARROW)
	{
		iScrollTop--;
	}
	else if (uArrow == MFMWFP_DOWNARROW)
	{
		iScrollTop++;
	}
	else
	{
		return FALSE;
	}

	if (!xxxMNSetTop(ppopup, iScrollTop))
	{
		if (!fSetTimer)
		{
			KillTimer(ppopup->spwndPopupMenu, uArrow);
		}
	}
	else
	{
		/*
		 * Set this timer just like we do in the scrollbar code:
		 * the first time we wait a little longer.
		 */
		SetTimer(ppopup->spwndPopupMenu, uArrow,
			(fSetTimer ? 4 : 1), NULL);
	}

	return TRUE;
}

/***************************************************************************\
* MNCheckScroll
*
*  checks to see if the given menu (pMenu) can be displayed in it's entirety
*  or if it can't, in which case it sets the menu to be scrollable
*
* 08/13/96 GerardoB Ported From Memphis.
\***************************************************************************/
int MNCheckScroll(PMENU pMenu, HMONITOR pMonitor)
{
	int     i;
	UINT    cyMax;
	PITEM   pItem;

	/*
	 * Max height that fits on the monitor
	 */

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(pMonitor, &mi);
	cyMax = (mi.rcMonitor.bottom - mi.rcMonitor.top);

	/*
	 * If the menu has a valid max height, use it
	 */
	if ((pMenu->cyMax != 0) && (pMenu->cyMax < cyMax))
	{
		cyMax = pMenu->cyMax;
	}

	/*
	 * Bail if menu is either empty, multicolumn, or able to fit
	 *   without scrolling
	 */
	if ((pMenu->rgItems == 0)
		|| (pMenu->rgItems->cxItem != pMenu->cxMenu)
		|| (pMenu->cyMenu + (2 * SYSMET(CYFIXEDFRAME)) <= cyMax))
	{

		pMenu->dwArrowsOn = MSA_OFF;
		pMenu->iTop = 0;
		pMenu->iMaxTop = 0;
		return pMenu->cyMenu;
	}

	/*
	 * Max client height
	 */
	cyMax -= 2 * (SYSMET(CYFIXEDFRAME) + gcyMenuScrollArrow);

	/*
	 * Determine the menu height
	 * Find the first item that won't fit.
	 */
	pItem = pMenu->rgItems;
	for (i = 0; i < (int)pMenu->cItems; i++, pItem++)
	{
		if (pItem->yItem > (UINT)cyMax)
		{
			break;
		}
	}
	if (i != 0)
	{
		pItem--;
	}
	pMenu->cyMenu = pItem->yItem;

	/*
	 * compute the last possible top item when all remaining items are fully
	 * visible
	 */
	cyMax = 0;
	i = pMenu->cItems - 1;
	pItem = pMenu->rgItems + i;
	for (; i >= 0; i--, pItem--)
	{
		cyMax += pItem->cyItem;
		if (cyMax > pMenu->cyMenu)
		{
			break;
		}
	}
	if ((UINT)i != pMenu->cItems - 1)
	{
		i++;
	}
	pMenu->iMaxTop = i;

	/*
	 * Update top item and scroll state
	 */
	if (pMenu->iTop > i)
	{
		pMenu->iTop = i;
	}

	if (pMenu->iTop == i)
	{
		pMenu->dwArrowsOn = MSA_ATBOTTOM;
	}
	else if (pMenu->iTop == 0)
	{
		pMenu->dwArrowsOn = MSA_ATTOP;
	}
	else
	{
		pMenu->dwArrowsOn = MSA_ON;
	}

	/*
	 * This is funtion is called by MN_SIZEWINDOW which doesn't check
	 *  if the scroll bars are present but simply adds (2 * SYSMET(CYFIXEDFRAME))
	 *  to calculate the window height. So we add the scrollbars height
	 *  here. (I believe MN_SIZEWINDOW is a private-but-publicly-known message)
	 */
	return (pMenu->cyMenu + (2 * gcyMenuScrollArrow));
}

/***************************************************************************\
* MNIsPopupItem
*
*
\***************************************************************************/

BOOL MNIsPopupItem(ITEM *lpItem)
{
	return ((lpItem) && (lpItem->spSubMenu) &&
			!TestMFS(lpItem, MFS_GRAYED));
}

/***************************************************************************\
* Validateppopupmenu
*
* 05-15-96 GerardoB  Created
\***************************************************************************/
#if DBG
void Validateppopupmenu(PPOPUPMENU ppopupmenu)
{
	UserAssert(ppopupmenu != NULL);
	try
	{
		UserAssert(!ppopupmenu->fFreed);

		/*
		 * If this popup is being destroyed to soon, ppopupmenuRoot can be NULL
		 */
		if (ppopupmenu->ppopupmenuRoot != NULL)
		{
			if (ppopupmenu->ppopupmenuRoot != ppopupmenu)
			{
				/*
				 * This must be a hierarchical popup
				 */
				UserAssert(ppopupmenu->spwndPrevPopup != NULL);
				UserAssert(!ppopupmenu->fIsMenuBar && !ppopupmenu->fIsTrackPopup);
				Validateppopupmenu(ppopupmenu->ppopupmenuRoot);
			}
			else
			{
				/*
				 * This must be the root popupmenu
				 */
				UserAssert(ppopupmenu->spwndPrevPopup == NULL);
				UserAssert(ppopupmenu->fIsMenuBar || ppopupmenu->fIsTrackPopup);
			}
		}

		/*
		 * This can be NULL when called from xxxDeleteThreadInfo
		 */
		if (ppopupmenu->spwndPopupMenu != NULL)
		{
			UserAssert(ppopupmenu->spwndPopupMenu == RevalidateCatHwnd(HW(ppopupmenu->spwndPopupMenu)));
		}

		/*
		 * This can be NULL when called from xxxDestroyWindow (spwndNotify)
		 *  or from xxxDeleteThreadInfo
		 */
		if (ppopupmenu->spwndNotify != NULL)
		{
			UserAssert(ppopupmenu->spwndNotify == RevalidateCatHwnd(HW(ppopupmenu->spwndNotify)));
		}

	}
	except(W32ExceptionHandler(FALSE, RIP_ERROR))
	{
		RIPMSG1(RIP_ERROR, "Validateppopupmenu: Invalid popup:%#p", ppopupmenu);
	}
}
#endif
/***************************************************************************\
* xxxSwitchToAlternateMenu
*
* Switches to the alternate popupmenu. Returns TRUE if we switch
* else FALSE.
*
* History:
* 05-25-91 Mikehar      Ported from Win3.1
\***************************************************************************/

BOOL IUI::xxxMNSwitchToAlternateMenu(
	PPOPUPMENU ppopupmenu)
{
	PMENU pmenuSwap = NULL;
	PMENUSTATE pMenuState;
	//TL tlpwndPopupMenu;

	if (!ppopupmenu->fIsMenuBar || !ppopupmenu->spmenuAlternate)
	{
		/*
		 * Do nothing if no menu or not top level menu bar.
		 * ppopupmenu->spmenuAlternate can be NULL when an app has
		 *  either system menu or menu bar but not both. If that menu
		 *  has only one popup that it's not dropped, then hitting
		 *  VK_RIGHT or VK_LEFT causes xxxMNKeyDown to end up here.
		 * ppopupmenu->fIsMenuBar can be false when you drop the
		 *  system menu of an app with no menu bar; then hit VK_RIGHT
		 *  on an item that doesn't have a popup and you'll get here
		 * There might be some other situations like this; in any case
		 *  the assertion got to go
		 */
		return FALSE;
	}

	/*
	 * If we're getting out of menu mode, do nothing
	 */
	if (ppopupmenu->fDestroyed)
	{
		return FALSE;
	}

	/*
	 * Select no items in the current menu.
	 */
	ThreadLock(ppopupmenu->spwndPopupMenu, &tlpwndPopupMenu);
	UserAssert(ppopupmenu->spwndPopupMenu != NULL);
	pMenuState = GetpMenuState(ppopupmenu->spwndPopupMenu);
	if (pMenuState == NULL)
	{
		RIPMSG0(RIP_ERROR, "xxxMNSwitchToAlternateMenu: pMenuState == NULL");
		ThreadUnlock(&tlpwndPopupMenu);
		return FALSE;
	}
	xxxMNSelectItem(ppopupmenu, pMenuState, MFMWFP_NOITEM);


	UserAssert(ppopupmenu->spmenu->spwndNotify == ppopupmenu->spmenuAlternate->spwndNotify);
	Lock(&pmenuSwap, ppopupmenu->spmenuAlternate);
	Lock(&ppopupmenu->spmenuAlternate, ppopupmenu->spmenu);
	Lock(&ppopupmenu->spmenu, pmenuSwap);
	Unlock(&pmenuSwap);

	/*
	 * Set this global because it is used in SendMenuSelect()
	 */
	LONG lStyle = GetWindowLong(ppopupmenu->spwndNotify, GWL_STYLE);
	if (!(lStyle & WS_SYSMENU))
	{
		pMenuState->fIsSysMenu = FALSE;
	}
	//else if (ppopupmenu->spwndNotify->spmenuSys != NULL)
	//{
	//	pMenuState->fIsSysMenu = (ppopupmenu->spwndNotify->spmenuSys ==
	//			ppopupmenu->spmenu);
	//}
	else
	{
		pMenuState->fIsSysMenu = !!TestMF(ppopupmenu->spmenu, MFSYSMENU);
	}

	ppopupmenu->fIsSysMenu = pMenuState->fIsSysMenu;

	//if (FWINABLE())
	//{
	//	xxxWindowEvent(EVENT_SYSTEM_MENUEND, ppopupmenu->spwndNotify,
	//		(ppopupmenu->fIsSysMenu ? OBJID_MENU : OBJID_SYSMENU), INDEXID_CONTAINER, 0);
	//	xxxWindowEvent(EVENT_SYSTEM_MENUSTART, ppopupmenu->spwndNotify,
	//		(ppopupmenu->fIsSysMenu ? OBJID_SYSMENU : OBJID_MENU), INDEXID_CONTAINER, 0);
	//}

	ThreadUnlock(&tlpwndPopupMenu);

	return TRUE;
}

/***************************************************************************\
* MenuDestroyHandler
*
* cleans up after this menu
*
* History:
* 05-25-91 Mikehar      Ported from Win3.1
\***************************************************************************/

void xxxMNDestroyHandler(
	PPOPUPMENU ppopupmenu)
{
	PITEM pItem;
	//TL tlpwndT;

	if (ppopupmenu == NULL)
	{
		/*
		 * This can happen if WM_NCCREATE failed to allocate the ppopupmenu
		 * in xxxMenuWindowProc.
		 */
		RIPMSG0(RIP_WARNING, "xxxMNDestroyHandler: NULL \"ppopupmenu\"");
		return;
	}

#if DBG
	/*
	 * When destroying a desktop's spwndMenu that is not in use (i.e., the
	 *  desktop is going away), the ppopupmenu is not exactly valid (i.e.,
	 *  we're not in menu mode) but it should be properly NULLed out so
	 *  everything should go smoothly
	 */
	Validateppopupmenu(ppopupmenu);
#endif

	if (ppopupmenu->spwndNextPopup != NULL)
	{
		/*
		 * We used to send the message to spwndNextPopup here. The message should
		 *  go to the current popup so it'll close spwndNextPopup (not to the next
		 *  to close its next, if any).
		 * I don't see how the current spwndPopupMenu can be NULL but we better
		 *  handle it since we never accessed it before. This menu code is tricky...
		 */
		HWND pwnd;
		UserAssert(ppopupmenu->spwndPopupMenu != NULL);
		pwnd = (ppopupmenu->spwndPopupMenu != NULL ? ppopupmenu->spwndPopupMenu : ppopupmenu->spwndNextPopup);
		ThreadLockAlways(pwnd, &tlpwndT);
		SendMessage(pwnd, MN_CLOSEHIERARCHY, 0, 0);
		ThreadUnlock(&tlpwndT);
	}

	if ((ppopupmenu->spmenu != NULL) && MNIsItemSelected(ppopupmenu))
	{
		/*
		 * Unset the hilite bit on the hilited item.
		 */
		if (ppopupmenu->posSelectedItem < ppopupmenu->spmenu->cItems)
		{
			/*
			 * this extra check saves Ambiente 1.02 -- they have a menu with
			 * one item in it.  When that command is chosen, the app proceeds
			 * to remove that one item -- leaving us in the oh so strange state
			 * of having a valid hMenu with a NULL rgItems.
			 */
			pItem = &(ppopupmenu->spmenu->rgItems[ppopupmenu->posSelectedItem]);
			pItem->fState &= ~MFS_HILITE;
		}
	}

	if (ppopupmenu->fShowTimer)
	{
		KillTimer(ppopupmenu->spwndPopupMenu, IDSYS_MNSHOW);
	}

	if (ppopupmenu->fHideTimer)
	{
		KillTimer(ppopupmenu->spwndPopupMenu, IDSYS_MNHIDE);
	}

	/*
	 * Send WM_UNINITMENUPOPUP so the menu owner can clean up.
	 */
	if (ppopupmenu->fSendUninit
		&& (ppopupmenu->spwndNotify != NULL))
	{

		ThreadLockAlways(ppopupmenu->spwndNotify, &tlpwndT);
		SendMessage(ppopupmenu->spwndNotify, WM_UNINITMENUPOPUP,
			(WPARAM)ppopupmenu->spmenu,
			MAKELONG(0, (ppopupmenu->fIsSysMenu ? MF_SYSMENU : 0)));
		ThreadUnlock(&tlpwndT);
	}

	ppopupmenu->fDestroyed = TRUE;
	if (!ppopupmenu->fDesktopMenu)
	{

		if (ppopupmenu->spwndPopupMenu != NULL)
		{
			SetPopupMenu(ppopupmenu->spwndPopupMenu, NULL);
		}

	}

	if (!ppopupmenu->fDelayedFree)
	{
		MNFreePopup(ppopupmenu);
	}
	else if (ppopupmenu->ppopupmenuRoot != NULL)
	{
		ppopupmenu->ppopupmenuRoot->fFlushDelayedFree = TRUE;
#if DBG
		{
			/*
			 * If this is not the rootpopup,
			 * assert that this popup is linked in the delayed free list
			 */
			if (!IsRootPopupMenu(ppopupmenu))
			{
				BOOL fFound = FALSE;
				PPOPUPMENU ppm = ppopupmenu->ppopupmenuRoot;
				while (ppm->ppmDelayedFree != NULL)
				{
					if (ppm->ppmDelayedFree == ppopupmenu)
					{
						fFound = TRUE;
						break;
					}
					ppm = ppm->ppmDelayedFree;
				}
				UserAssert(fFound);
			}
		}
#endif
	}
	else
	{
		//UserAssertMsg1(FALSE, "Leaking ppopupmenu:%p?", ppopupmenu);
	}

}


/***************************************************************************\
* MenuCharHandler
*
* Handles char messages for the given menu. This procedure is called
* directly if the menu char is for the top level menu bar else it is called
* by the menu window proc on behalf of the window that should process the
* key.
*
* History:
* 05-25-91 Mikehar      Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNChar(
	PPOPUPMENU ppopupmenu,
	PMENUSTATE pMenuState,
	UINT character)
{
	//	PMENU pMenu;
	//	UINT flags;
	//	LRESULT result;
	//	int item;
	//	INT matchType;
	//	BOOL    fExecute = FALSE;
	//	TL tlpwndNotify;
	//
	//	pMenu = ppopupmenu->spmenu;
	//
	//	Validateppopupmenu(ppopupmenu);
	//
	//	/*
	//	 * If this comes in with a NULL pMenu, then we could have problems.
	//	 * This could happen if the xxxMNStartMenuState never gets called
	//	 * because the fInsideMenuLoop is set.
	//	 *
	//	 * This is placed in here temporarily until we can discover why
	//	 * this pMenu isn't set.  We will prevent the system from crashing
	//	 * in the meantime.
	//	 *
	//	 * HACK: ChrisWil
	//	 */
	//	if (pMenu == NULL)
	//	{
	//		UserAssert(pMenu);
	//		xxxMNDismiss(pMenuState);
	//		return;
	//	}
	//
	//	/*
	//	 * If we're getting out of menu mode, bail
	//	 */
	//	if (ppopupmenu->fDestroyed)
	//	{
	//		return;
	//	}
	//
	//	item = xxxMNFindChar(pMenu, character,
	//			ppopupmenu->posSelectedItem, &matchType);
	//	if (item != MFMWFP_NOITEM)
	//	{
	//		int item1;
	//		int firstItem = item;
	//
	//		/*
	//		 * Find first ENABLED menu item with the given mnemonic 'character'
	//		 * !!!  If none found, exit menu loop  !!!
	//		 */
	//		while (pMenu->rgItems[item].fState & MFS_GRAYED)
	//		{
	//			item = xxxMNFindChar(pMenu, character, item, &matchType);
	//			if (item == firstItem)
	//			{
	//				xxxMNDismiss(pMenuState);
	//				return;
	//			}
	//		}
	//		item1 = item;
	//
	//		/*
	//		 * Find next ENABLED menu item with the given mnemonic 'character'
	//		 * This is to see if we have a DUPLICATE MNEMONIC situation
	//		 */
	//		do
	//		{
	//			item = xxxMNFindChar(pMenu, character, item, &matchType);
	//		}
	//		while ((pMenu->rgItems[item].fState & MFS_GRAYED) && (item != firstItem));
	//
	//		if ((firstItem == item) || (item == item1))
	//		{
	//			fExecute = TRUE;
	//		}
	//
	//		item = item1;
	//	}
	//
	//	if ((item == MFMWFP_NOITEM) && ppopupmenu->fIsMenuBar && (character == TEXT(' ')))
	//	{
	//
	//		/*
	//		 * Handle the case of the user cruising through the top level menu bar
	//		 * without any popups dropped.  We need to handle switching to and from
	//		 * the system menu.
	//		 */
	//		if (ppopupmenu->fIsSysMenu)
	//		{
	//
	//			/*
	//			 * If we are on the system menu and user hits space, bring
	//			 * down thesystem menu.
	//			 */
	//			item = 0;
	//			fExecute = TRUE;
	//		}
	//		else if (ppopupmenu->spmenuAlternate != NULL)
	//		{
	//
	//			/*
	//			 * We are not currently on the system menu but one exists.  So
	//			 * switch to it and bring it down.
	//			 */
	//			item = 0;
	//			goto SwitchToAlternate;
	//		}
	//	}
	//
	//	if ((item == MFMWFP_NOITEM) && ppopupmenu->fIsMenuBar && ppopupmenu->spmenuAlternate)
	//	{
	//
	//		/*
	//		 * No matching item found on this top level menu (could be either the
	//		 * system menu or the menu bar).  We need to check the other menu.
	//		 */
	//		item = xxxMNFindChar(ppopupmenu->spmenuAlternate,
	//				character, 0, &matchType);
	//
	//		if (item != MFMWFP_NOITEM)
	//		{
	//SwitchToAlternate:
	//			if (xxxMNSwitchToAlternateMenu(ppopupmenu))
	//			{
	//				xxxMNChar(ppopupmenu, pMenuState, character);
	//			}
	//			return;
	//		}
	//	}
	//
	//	if (item == MFMWFP_NOITEM)
	//	{
	//		flags = (ppopupmenu->fIsSysMenu) ? MF_SYSMENU : 0;
	//
	//		if (!ppopupmenu->fIsMenuBar)
	//		{
	//			flags |= MF_POPUP;
	//		}
	//
	//		ThreadLock(ppopupmenu->spwndNotify, &tlpwndNotify);
	//		result = SendMessage(ppopupmenu->spwndNotify, WM_MENUCHAR,
	//				MAKELONG((WORD)character, (WORD)flags),
	//				(LPARAM)PtoH(ppopupmenu->spmenu));
	//		ThreadUnlock(&tlpwndNotify);
	//
	//		switch (HIWORD(result))
	//		{
	//		case MNC_IGNORE:
	//			xxxMessageBeep(0);
	//			/*
	//			 * If we're on the menu bar, cancel menu mode (fall through).
	//			 * We do this because you can really freak out an end user
	//			 *  who accidentally tapped the Alt key (causing us to go
	//			 *  into "invisible" menu mode) and now can't type anything!
	//			 */
	//			if (flags & MF_POPUP)
	//			{
	//				return;
	//			}
	//		/*
	//		 * Fall through.
	//		 */
	//
	//		case MNC_CLOSE:
	//			xxxMNDismiss(pMenuState);
	//			return;
	//
	//		case MNC_EXECUTE:
	//			fExecute = TRUE;
	//		/* fall thru */
	//
	//		case MNC_SELECT:
	//			item = (UINT)(short)LOWORD(result);
	//			if ((WORD) item >= ppopupmenu->spmenu->cItems)
	//			{
	//				RIPMSG1(RIP_WARNING, "Invalid item number returned from WM_MENUCHAR %#lx", result);
	//				return;
	//			}
	//			break;
	//		}
	//	}
	//
	//	if (item != MFMWFP_NOITEM)
	//	{
	//		xxxMNSelectItem(ppopupmenu, pMenuState, item);
	//
	//		if (fExecute)
	//		{
	//			xxxMNKeyDown(ppopupmenu, pMenuState, VK_RETURN);
	//		}
	//	}
}

/***************************************************************************\
*
*  GetMenuInheritedContextHelpId(PPOPUPMENU  ppopup)
* Given a ppopup, this function will see if that menu has a context help
*  id and return it. If it does not have a context help id, it will look up
*  in the parent menu, parent of the parent etc., all the way to the top
*  top level menu bar till it finds a context help id and returns it. If no
*  context help id is found, it returns a zero.
*
\***************************************************************************/

DWORD GetMenuInheritedContextHelpId(PPOPUPMENU  ppopup)
{
	HWND  pWnd;

	/*
	 * If we are already at the menubar, simply return it's ContextHelpId
	 */
	UserAssert(ppopup != NULL);
	if (ppopup->fIsMenuBar)
	{
		goto Exit_GMI;
	}

	while (TRUE)
	{
		UserAssert(ppopup != NULL);

		/*
		 * See if the given popup has a context help id.
		 */
		if (ppopup->spmenu->dwContextHelpId)
		{
			/* Found the context Id */
			break;
		}

		/*
		 * Get the previous popup menu;
		 * Check if the previous menu is the menu bar.
		 */
		if ((ppopup->fHasMenuBar) &&
			(ppopup->spwndPrevPopup == ppopup->spwndNotify))
		{

			ppopup = ppopup -> ppopupmenuRoot;
			break;
		}
		else
		{
			/*
			 * See if this has a valid prevPopup; (it could be TrackPopup menu)
			 */
			if ((pWnd = ppopup -> spwndPrevPopup) == NULL)
			{
				return ((DWORD)0);
			}

			ppopup = GetPopupMenu(pWnd);
		}
	}

Exit_GMI:
	return (ppopup->spmenu->dwContextHelpId);
}

/***************************************************************************\
* void MenuKeyDownHandler(PPOPUPMENU ppopupmenu, UINT key)
* effects: Handles a keydown for the given menu.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNKeyDown(
	PPOPUPMENU ppopupmenu,
	PMENUSTATE pMenuState,
	UINT key)
{
	//LRESULT dwMDIMenu;
	UINT item;
	BOOL fHierarchyWasDropped = FALSE;
	//TL tlpwndT;
	PPOPUPMENU ppopupSave;
	BOOL bFakedKey;
	UINT keyOrig = key;

	/*
	 * Blow off keyboard if mouse down.
	 */
	if ((pMenuState->fButtonDown) && (key != VK_F1))
	{
		/*
		 * Check if the user wants to cancel dragging.
		 */
		if (pMenuState->fDragging && (key == VK_ESCAPE))
		{
			RIPMSG0(RIP_WARNING, "xxxMNKeyDown: ESC while dragging");
			pMenuState->fIgnoreButtonUp = TRUE;
		}

		return;
	}

	switch (key)
	{
	case VK_MENU:
	case VK_F10:
	{
		/*
		 * Modeless don't go away when the menu key is hit. They just
		 *  ignore it.
		 */
		if (pMenuState->fModelessMenu)
		{
			return;
		}

		if (gwinOldAppHackoMaticFlags & WOAHACK_CHECKALTKEYSTATE)
		{

			/*
			 * Winoldapp is telling us to put up/down the system menu.  Due to
			 * possible race conditions, we need to check the state of the alt
			 * key before throwing away the menu.
			 */
			if (gwinOldAppHackoMaticFlags & WOAHACK_IGNOREALTKEYDOWN)
			{
				return;
			}
		}
		xxxMNDismiss(pMenuState);

		/*
		 * We're going to exit menu mode but the ALT key is down, so clear
		 *  pMenuState->fUnderline to cause xxxMNLoop not to erase the underlines
		 */
		if (key == VK_MENU)
		{
			pMenuState->fUnderline = FALSE;
		}
	}
	return;

	case VK_ESCAPE:
	{
		/*
		 * Escape key was hit.  Get out of one level of menus.  If no active
		 * popups or we are minimized and there are no active popups below
		 * this, we need to get out of menu mode.  Otherwise, we popup up
		 * one level in the hierarchy.
		 */
		LONG lStyle = GetWindowLong(ppopupmenu->ppopupmenuRoot->spwndNotify, GWL_STYLE);
		if (ppopupmenu->fIsMenuBar ||
			ppopupmenu == ppopupmenu->ppopupmenuRoot ||
			lStyle & WS_MINIMIZE)
		{
			xxxMNDismiss(pMenuState);
		}
		else
		{
			/*
			 * Pop back one level of menus.
			 */
			if (ppopupmenu->fHasMenuBar &&
				ppopupmenu->spwndPrevPopup == ppopupmenu->spwndNotify)
			{

				PPOPUPMENU ppopupmenuRoot = ppopupmenu->ppopupmenuRoot;

				ppopupmenuRoot->fDropNextPopup = FALSE;

#if 0
				/*
				 * We are on a menu bar hierarchy and there is only one popup
				 * visible.  We have to cancel this popup and put focus back on
				 * the menu bar.
				 */
				if (_IsIconic(ppopupmenuRoot->spwndNotify))
				{

					/*
					 * However, if we are iconic there really is no menu
					 * bar so let's make it easier for users and get out
					 * of menu mode completely.
					 */
					xxxMNDismiss(pMenuState);
				}
				else
#endif
					/*
					 * If the popup is closed, a modeless menu won't
					 *  have a window to get the keys. So modeless menu
					 *  cancel the menu at this point. Modal menus go
					 *  to the menu bar.
					 */
					if (pMenuState->fModelessMenu)
					{
						xxxMNDismiss(pMenuState);
					}
					else
					{
						xxxMNCloseHierarchy(ppopupmenuRoot, pMenuState);
					}
			}
			else
			{
				ThreadLock(ppopupmenu->spwndPrevPopup, &tlpwndT);
				::SendMessage(ppopupmenu->spwndPrevPopup, MN_CLOSEHIERARCHY,
					0, 0);
				ThreadUnlock(&tlpwndT);
			}
		}
	}
	return;

	case VK_UP:
	case VK_DOWN:
		if (ppopupmenu->fIsMenuBar)
		{

			/*
			 * If we are on the top level menu bar, try to open the popup if
			 * possible.
			 */
			if (xxxMNOpenHierarchy(ppopupmenu, pMenuState) == (HWND)(-1))
			{
				return;
			}
		}
		else
		{
			item = MNFindNextValidItem(ppopupmenu->spmenu,
					ppopupmenu->posSelectedItem, (key == VK_UP ? -1 : 1), 0);
			xxxMNSelectItem(ppopupmenu, pMenuState, item);
		}
		return;

	case VK_LEFT:
	case VK_RIGHT:
#ifdef USE_MIRRORING
		bFakedKey = (!!ppopupmenu->fRtoL) ^ (!!TestWF(ppopupmenu->spwndPopupMenu, WEFLAYOUTRTL));
#else
		bFakedKey = ppopupmenu->fRtoL;
#endif
		if (bFakedKey)
			/*
			 * turn the keys around, we drew the menu backwards.
			 */
		{
			key = (key == VK_LEFT) ? VK_RIGHT : VK_LEFT;
		}
		if (!ppopupmenu->fIsMenuBar && (key == VK_RIGHT) &&
			!ppopupmenu->spwndNextPopup)
		{
			/*
			 * Try to open the hierarchy at this item if there is one.
			 */
			if (xxxMNOpenHierarchy(ppopupmenu, pMenuState) == (HWND)(-1))
			{
				return;
			}
			if (ppopupmenu->fHierarchyDropped)
			{
				return;
			}
		}

		if (ppopupmenu->spwndNextPopup)
		{
			fHierarchyWasDropped = TRUE;
			if ((key == VK_LEFT) && !ppopupmenu->fIsMenuBar)
			{
				xxxMNCloseHierarchy(ppopupmenu, pMenuState);
				return;
			}
		}
		else if (ppopupmenu->fDropNextPopup)
		{
			fHierarchyWasDropped = TRUE;
		}

		ppopupSave = ppopupmenu;

		item = MNFindItemInColumn(ppopupmenu->spmenu,
				ppopupmenu->posSelectedItem,
				(key == VK_LEFT ? -1 : 1),
				(ppopupmenu->fHasMenuBar &&
					ppopupmenu == ppopupmenu->ppopupmenuRoot));

		if (item == MFMWFP_NOITEM)
		{

			/*
			 * No valid item found in the given direction so send it up to our
			 * parent to handle.
			 */
			if (ppopupmenu->fHasMenuBar &&
				ppopupmenu->spwndPrevPopup == ppopupmenu->spwndNotify)
			{

				/*
				 * if we turned the key round, then turn it back again.
				 */
				if (bFakedKey)
				{
					key = (key == VK_LEFT) ? VK_RIGHT : VK_LEFT;
				}
				/*
				 * Go to next/prev item in menu bar since a popup was down and
				 * no item on the popup to go to.
				 */
				xxxMNKeyDown(ppopupmenu->ppopupmenuRoot, pMenuState, key);
				return;
			}

			if (ppopupmenu == ppopupmenu->ppopupmenuRoot)
			{
				if (!ppopupmenu->fIsMenuBar)
				{

					/*
					 * No menu bar associated with this menu so do nothing.
					 */
					return;
				}
			}
			else
			{
				ThreadLock(ppopupmenu->spwndPrevPopup, &tlpwndT);
				::SendMessage(ppopupmenu->spwndPrevPopup, WM_KEYDOWN, keyOrig, 0);
				ThreadUnlock(&tlpwndT);
				return;
			}
		}

		if (!ppopupmenu->fIsMenuBar)
		{
			if (item != MFMWFP_NOITEM)
			{
				xxxMNSelectItem(ppopupmenu, pMenuState, item);
			}
			return;

		}
		else
		{

			/*
			 * Special handling if keydown occurred on a menu bar.
			 */
			if (item == MFMWFP_NOITEM)
			{

				//if (TestWF(ppopupmenu->spwndNotify, WFSYSMENU))
				//{
				//	PTHREADINFO ptiCurrent = PtiCurrent();
				//	HWND    pwndNextMenu;
				//	PMENU   pmenuNextMenu, pmenuUse;
				//	MDINEXTMENU mnm;
				//	TL tlpmenuNextMenu;
				//	TL tlpwndNextMenu;

				//	mnm.hmenuIn = (HMENU)0;
				//	mnm.hmenuNext = (HMENU)0;
				//	mnm.hwndNext = (HWND)0;

				//	/*
				//	 * We are in the menu bar and need to go up to the system menu
				//	 * or go from the system menu to the menu bar.
				//	 */
				//	pmenuNextMenu = ppopupmenu->fIsSysMenu ?
				//		_GetSubMenu(ppopupmenu->spmenu, 0) :
				//		ppopupmenu->spmenu;
				//	mnm.hmenuIn = PtoH(pmenuNextMenu);
				//	ThreadLock(ppopupmenu->spwndNotify, &tlpwndT);
				//	dwMDIMenu = SendMessage(ppopupmenu->spwndNotify,
				//			WM_NEXTMENU, (WPARAM)keyOrig, (LPARAM)&mnm);
				//	ThreadUnlock(&tlpwndT);

				//	pwndNextMenu = RevalidateHwnd(mnm.hwndNext);
				//	if (pwndNextMenu == NULL)
				//	{
				//		goto TryAlternate;
				//	}

				//	/*
				//	 * If this window belongs to another thread, we cannot
				//	 *  use it. The menu loop won't get any messages
				//	 *  directed to that thread.
				//	 */
				//	if (GETPTI(pwndNextMenu) != ptiCurrent)
				//	{
				//		RIPMSG1(RIP_WARNING, "xxxMNKeyDown: Ignoring mnm.hwndNext bacause it belongs to another thread: %#p", pwndNextMenu);
				//		goto TryAlternate;
				//	}


				//	pmenuNextMenu = RevalidateHmenu(mnm.hmenuNext);
				//	if (pmenuNextMenu == NULL)
				//	{
				//		goto TryAlternate;
				//	}

				//	ThreadLock(pmenuNextMenu, &tlpmenuNextMenu);
				//	ThreadLock(pwndNextMenu, &tlpwndNextMenu);

				//	/*
				//	 * If the system menu is for a minimized MDI child,
				//	 * make sure the menu is dropped to give the user a
				//	 * visual clue that they are in menu mode
				//	 */
				//	if (TestWF(pwndNextMenu, WFMINIMIZED))
				//	{
				//		fHierarchyWasDropped = TRUE;
				//	}

				//	xxxMNSelectItem(ppopupmenu, pMenuState, MFMWFP_NOITEM);

				//	pMenuState->fIsSysMenu = TRUE;
				//	UnlockPopupMenu(ppopupmenu, &ppopupmenu->spmenuAlternate);
				//	ppopupmenu->fToggle = FALSE;
				//	/*
				//	 * GetSystemMenu(pwnd, FALSE) and pwnd->spmenuSys are
				//	 * NOT equivalent -- GetSystemMenu returns the 1st submenu
				//	 * of pwnd->spmenuSys -- make up for that here
				//	 */
				//	pmenuUse = (((pwndNextMenu->spmenuSys != NULL)
				//				&& (_GetSubMenu(pwndNextMenu->spmenuSys, 0) == pmenuNextMenu))
				//			? pwndNextMenu->spmenuSys
				//			: pmenuNextMenu);
				//	/*
				//	 * We're going to change the notification window AND the menu.
				//	 * LockPopupMenu needs to unlock the current pmenu-spwndNotify
				//	 *  but also lock the new pmenu-spwndNotify. Since we cannot
				//	 *  give it the current AND the new pair, we unlock the
				//	 *  current one first, switch the notification window and
				//	 *  then call LockPopupMenu to lock the new pmenu-spwndNotify.
				//	 */
				//	UserAssert(IsRootPopupMenu(ppopupmenu));
				//	UnlockPopupMenu(ppopupmenu, &ppopupmenu->spmenu);
				//	Lock(&ppopupmenu->spwndNotify, pwndNextMenu);
				//	Lock(&ppopupmenu->spwndPopupMenu, pwndNextMenu);
				//	LockPopupMenu(ppopupmenu, &ppopupmenu->spmenu, pmenuUse);
				//	/*
				//	 * We just switched to a new notification window so
				//	 *  we need to Adjust capture accordingly
				//	 */
				//	if (!pMenuState->fModelessMenu)
				//	{
				//		ptiCurrent->pq->QF_flags &= ~QF_CAPTURELOCKED;
				//		xxxMNSetCapture(ppopupmenu);
				//	}


				//	if (!TestWF(pwndNextMenu, WFCHILD) &&
				//		ppopupmenu->spmenu != NULL)
				//	{

				//		/*
				//		 * This window has a system menu and a main menu bar
				//		 * Set the alternate menu to the appropriate menu
				//		 */
				//		if (pwndNextMenu->spmenu == ppopupmenu->spmenu)
				//		{
				//			LockPopupMenu(ppopupmenu, &ppopupmenu->spmenuAlternate,
				//				pwndNextMenu->spmenuSys);
				//			pMenuState->fIsSysMenu = FALSE;
				//		}
				//		else
				//		{
				//			LockPopupMenu(ppopupmenu, &ppopupmenu->spmenuAlternate,
				//				pwndNextMenu->spmenu);
				//		}
				//	}

				//	ThreadUnlock(&tlpwndNextMenu);
				//	ThreadUnlock(&tlpmenuNextMenu);

				//	ppopupmenu->fIsSysMenu = pMenuState->fIsSysMenu;

				//	item = 0;
				//}
				//else
				//TryAlternate:
				if (xxxMNSwitchToAlternateMenu(ppopupmenu))
				{
					/*
					 * go to first or last menu item int ppopup->hMenu
					 * based on 'key'
					 */
					int dir = (key == VK_RIGHT) ? 1 : -1;

					item = MNFindNextValidItem(ppopupmenu->spmenu, MFMWFP_NOITEM, dir, 0);
				}
			}

			if (item != MFMWFP_NOITEM)
			{
				/*
				 * we found a new menu item to go to
				 * 1) close up the previous menu if it was dropped
				 * 2) select the new menu item to go to
				 * 3) drop the new menu if the previous menu was dropped
				 */

				if (ppopupSave->spwndNextPopup)
				{
					xxxMNCloseHierarchy(ppopupSave, pMenuState);
				}

				xxxMNSelectItem(ppopupmenu, pMenuState, item);

				if (fHierarchyWasDropped)
				{
DropHierarchy:
					if (xxxMNOpenHierarchy(ppopupmenu, pMenuState) == (HWND)(-1))
					{
						return;
					}
				}
			}
		}
		return;

	case VK_RETURN:
	{
		BOOL fEnabled;
		PITEM  pItem;

		if (ppopupmenu->posSelectedItem >= ppopupmenu->spmenu->cItems)
		{
			xxxMNDismiss(pMenuState);
			return;
		}

		pItem = ppopupmenu->spmenu->rgItems + ppopupmenu->posSelectedItem;
		fEnabled = !(pItem->fState & MFS_GRAYED);
		if ((pItem->spSubMenu != NULL) && fEnabled)
		{
			goto DropHierarchy;
		}

		/*
		 * If no item is selected, throw away menu and return.
		 */
		if (fEnabled)
		{
			xxxMNDismissWithNotify(pMenuState, ppopupmenu->spmenu, pItem, ppopupmenu->posSelectedItem, 0);
		}
		else
		{
			xxxMNDismiss(pMenuState);
		}
		return;
	}

		//case VK_F1: /* Provide context sensitive help. */
		//{
		//	PITEM  pItem;

		//	pItem = ppopupmenu->spmenu->rgItems + ppopupmenu->posSelectedItem;
		//	ThreadLock(ppopupmenu->spwndNotify, &tlpwndT);
		//	xxxSendHelpMessage(ppopupmenu->spwndNotify, HELPINFO_MENUITEM, pItem->wID,
		//		PtoHq(ppopupmenu->spmenu),
		//		GetMenuInheritedContextHelpId(ppopupmenu));
		//	ThreadUnlock(&tlpwndT);
		//	break;
		//}

	}
}
/***************************************************************************\
* xxxMNPositionHierarchy
*
* Calculates the x.y postion to drop a hierarchy and returns the direction
*  to be used when animating (PAS_* value)
*
* 11/19/96  GerardoB  Extracted from xxxMNOpenHierarchy
\***************************************************************************/
UINT
xxxMNPositionHierarchy(
	PPOPUPMENU  ppopup,
	PITEM       pitem,
	int         cx,
	int         cy,
	int         *px,
	int         *py,
	HMONITOR    *ppMonitor)
{
	int         x, y;
	UINT        uPAS;
	HMONITOR    hMonitor;

	UserAssert(ppopup->fHierarchyDropped && (ppopup->spwndNextPopup != NULL));
	CIUIRect rcPopup;
	GetWindowRect(ppopup->spwndPopupMenu, rcPopup);

	if (ppopup->fIsMenuBar)
	{
		/*
		 * This is a menu being dropped from the top menu bar.  We need to
		 * position it differently than hierarchicals which are dropped from
		 * another popup.
		 */
		LONG lStyle = GetWindowLong(ppopup->spwndPopupMenu, GWL_STYLE);
		BOOL fIconic = ((lStyle & WS_MINIMIZE) != 0);
		RECT rcWindow;

		/*
		 * Menu bar popups animate down.
		 */
		uPAS = PAS_DOWN;

		CopyRect(&rcWindow, &rcPopup);
		if (fIconic && IsTrayWindow(ppopup->spwndPopupMenu))
		{
			xxxSendMinRectMessages(ppopup->spwndPopupMenu, &rcWindow);
		}

		/*
		 * x position
		 */
		if (!SYSMET(MENUDROPALIGNMENT) && !TestMF(ppopup->spmenu, MFRTL))
		{
			if (fIconic)
			{
				x = rcWindow.left;
			}
			else
			{
				x = rcWindow.left + pitem->xItem;
			}
		}
		else
		{
			ppopup->fDroppedLeft = TRUE;
			if (fIconic)
			{
				x = rcWindow.right - cx;
			}
			else
			{
				x = rcWindow.left + pitem->xItem + pitem->cxItem - cx;
			}
		}

		/*
		 * For a menu bar dropdown, pin to the monitor that owns the
		 * majority of the menu item.  Otherwise, pin to the monitor that
		 * owns the minimized window (the tray rect for min-to-tray dudes).
		 */
		if (!fIconic)
		{
			/*
			 * Use rcWindow as scratch for the menu bar item rect.  We want
			 * to pin this menu on whatever monitor owns most of the menu
			 * item clicked on.
			 */
			rcWindow.left += pitem->xItem;
			rcWindow.top  += pitem->yItem;
			rcWindow.right = rcWindow.left + pitem->cxItem;
			rcWindow.bottom = rcWindow.top + pitem->cyItem;
		}

		hMonitor = MonitorFromRect(&rcWindow, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);
		/*
		 * y position
		 */
		if (!fIconic)
		{
			y = rcWindow.bottom;
		}
		else
		{
			/*
			 * If the window is iconic, pop the menu up.  Since we're
			 * minimized, the sysmenu button doesn't really exist.
			 */
			y = rcWindow.top - cy;

			if (y < mi.rcMonitor.top)
			{
				y = rcWindow.bottom;
			}
		}

		/*
		 * Make sure the menu doesn't go off right side of monitor
		 */
		x = min(x, mi.rcMonitor.right - cx);

#ifdef USE_MIRRORING
		if (TestWF(ppopup->spwndPopupMenu, WEFLAYOUTRTL))
		{
			x = ppopup->spwndPopupMenu->rcWindow.right - x + ppopup->spwndPopupMenu->rcWindow.left - cx;
		}
#endif

	}
	else     /* if (ppopup->fIsMenuBar) */
	{

		/* Now position the hierarchical menu window.
		 * We want to overlap by the amount of the frame, to help in the
		 * 3D illusion.
		 */

		/*
		 * By default, hierachical popups animate to the right
		 */
		uPAS = PAS_RIGHT;
		x = rcPopup.left + pitem->xItem + pitem->cxItem;

		/* Note that we DO want the selections in the item and its popup to
		 * align horizontally.
		 */
		y = rcPopup.top + pitem->yItem;
		if (ppopup->spmenu->dwArrowsOn != MSA_OFF)
		{
			y += gcyMenuScrollArrow - MNGetToppItem(ppopup->spmenu)->yItem;
		}

		/*
		 * Try to make sure the menu doesn't go off right side of the
		 * monitor.  If it does, drop it left, overlapping the checkmark
		 * area.  Unless it would cover the previous menu...
		 *
		 * Use the monitor that the parent menu is on to keep all hierarchicals
		 * in the same place.
		 */
		hMonitor = MonitorFromWindow(
				ppopup->spwndPopupMenu, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);

#ifdef USE_MIRRORING
		if ((!!ppopup->fDroppedLeft) ^ (!!TestWF(ppopup->spwndPopupMenu, WEFLAYOUTRTL)))
		{
#else
		if (ppopup->fDroppedLeft)
		{
#endif
			int xTmp;

			/*
			 * If this menu has dropped left, see if our hierarchy can be made
			 * to drop to the left also.
			 */
			xTmp = rcPopup.left + SYSMET(CXFIXEDFRAME) - cx;
			if (xTmp >= mi.rcMonitor.left)
			{
				x = xTmp;
				uPAS = PAS_LEFT;
			}
		}

		/*
		 * Make sure the menu doesn't go off right side of screen.  Make it drop
		 * left if it does.
		 */
		if (x + cx > mi.rcMonitor.right)
		{
			x = rcPopup.left + SYSMET(CXFIXEDFRAME) - cx;
			uPAS = PAS_LEFT;
		}
#ifdef USE_MIRRORING
		if (TestWF(ppopup->spwndPopupMenu, WEFLAYOUTRTL))
		{
			uPAS ^= PAS_HORZ;
		}
#endif

	} /* else if (ppopup->fIsMenuBar) */

	/*
	 * Does the menu extend beyond bottom of monitor?
	 */
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &mi);

	UserAssert(hMonitor);
	if (y + cy > mi.rcMonitor.bottom)
	{
		y -= cy;

		/*
		 * Try to pop above menu bar first
		 */
		if (ppopup->fIsMenuBar)
		{
			y -= SYSMET(CYMENUSIZE);
			if (y >= mi.rcMonitor.top)
			{
				uPAS = PAS_UP;
			}
		}
		else
		{
			/*
			 * Account for nonclient frame above & below
			 */
			y += pitem->cyItem + 2 * SYSMET(CYFIXEDFRAME);
		}

		/*
		 * Make sure that starting point is on a monitor, and all of menu shows.
		 */
		if ((y < mi.rcMonitor.top) ||
			(y + cy > mi.rcMonitor.bottom))
			/*
			 * Pin it to the bottom.
			 */
		{
			y = mi.rcMonitor.bottom - cy;
		}
	}

	/*
	 * Make sure Upper Left corner of menu is always visible.
	 */
	x = max(x, mi.rcMonitor.left);
	y = max(y, mi.rcMonitor.top);

	/*
	 * Propagate position
	 */
	*px = x;
	*py = y;
	*ppMonitor = hMonitor;

	/*
	 * Return animation direction
	 */
	return uPAS;
}


/***************************************************************************\
* xxxCleanupDesktopMenu
*
* History:
* 10/19/98 GerardoB  Extracted from xxxMNCloseHierarchy
\***************************************************************************/
void xxxCleanupDesktopMenu(HWND pwndDeskMenu, PDESKTOP pdesk)
{
	//TL tlpwnd;
	CheckLock(pwndDeskMenu);
	/*
	 *  Put it on the message window tree so it is out of the way.
	 */
	UserAssert(GetParent(pwndDeskMenu) == GetDesktopWindow());
	ThreadLockAlways(pdesk->spwndMessage, &tlpwnd);
	//SetParent(pwndDeskMenu, pdesk->spwndMessage);
	ThreadUnlock(&tlpwnd);

	/*
	 * Give ownershipe back to the desktop thread
	 */
	//pwndDeskMenu->head.pti = pdesk->pDeskInfo->spwnd->head.pti;
	//Unlock(&pwndDeskMenu->spwndOwner);
}

/***************************************************************************\
* HWND MenuOpenHierarchyHandler(PPOPUPMENU ppopupmenu)
* effects: Drops one level of the hierarchy at the selection.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/
#ifdef _DEBUG
HWND IUI::g_pwndHierarchy = NULL;
#endif

// 创建并显示下级菜单
HWND IUI::xxxMNOpenHierarchy(
	PPOPUPMENU ppopupmenu, PMENUSTATE pMenuState)
{
	HWND        ret = 0;
	PITEM       pItem;
	HWND        pwndHierarchy;
	PPOPUPMENU  ppopupmenuHierarchy;
	LONG        sizeHierarchy;
	int         xLeft;
	int         yTop;
	int         cxPopup, cyPopup;
	//TL          tlpwndT;
	//TL          tlpwndHierarchy;
	//PTHREADINFO ptiCurrent = PtiCurrent();
	//PDESKTOP    pdesk = ptiCurrent->rpdesk;
	BOOL        fSendUninit = FALSE;
	HMENU       hmenuInit = NULL;
	HMONITOR    pMonitor;


	if (ppopupmenu->posSelectedItem == MFMWFP_NOITEM)
	{
		/*
		 *  No selection so fail.
		 */
		return NULL;
	}

	if (ppopupmenu->posSelectedItem >= ppopupmenu->spmenu->cItems)
	{
		return NULL;
	}

	if (ppopupmenu->fHierarchyDropped)
	{
		if (ppopupmenu->fHideTimer)
		{
			xxxMNCloseHierarchy(ppopupmenu, pMenuState);
		}
		else
		{
			/*
			 * Hierarchy already dropped. What are we doing here?
			 */
			UserAssert(!ppopupmenu->fHierarchyDropped);
			return NULL;
		}
	}

	if (ppopupmenu->fShowTimer)
	{
		::KillTimer(ppopupmenu->spwndPopupMenu, IDSYS_MNSHOW);
		ppopupmenu->fShowTimer = FALSE;
	}

	/*
	 * Get a pointer to the currently selected item in this menu.
	 */
	pItem = &(ppopupmenu->spmenu->rgItems[ppopupmenu->posSelectedItem]);

	if (pItem->spSubMenu == NULL)
	{
		goto Exit;
	}

	/*
	 * Send the initmenupopup message.
	 */
	if (!ppopupmenu->fNoNotify)
	{
		ThreadLock(ppopupmenu->spwndNotify, &tlpwndT);
		/*
		 * WordPerfect's Grammatik app doesn't know that TRUE means NON-ZERO,
		 * not 1.  So we must use 0 & 1 explicitly for fIsSysMenu here
		 * -- Win95B B#4947 -- 2/13/95 -- jeffbog
		 */
		//hmenuInit = PtoHq(pItem->spSubMenu);
		//SendMessage(ppopupmenu->spwndNotify, WM_INITMENUPOPUP,
		//	(WPARAM)hmenuInit, MAKELONG(ppopupmenu->posSelectedItem,
		//		(ppopupmenu->fIsSysMenu ? 1 : 0)));
		ThreadUnlock(&tlpwndT);
		fSendUninit = TRUE;
	}


	/*
	 * B#1517
	 * Check if we're still in menu loop
	 */
	if (!pMenuState->fInsideMenuLoop)
	{
		RIPMSG0(RIP_WARNING, "Menu loop ended unexpectedly by WM_INITMENUPOPUP");
		ret = (HWND)(-1);
		goto Exit;
	}

	/*
	 * The WM_INITMENUPOPUP message may have resulted in a change to the
	 * menu.  Make sure the selection is still valid.
	 */
	if (ppopupmenu->posSelectedItem >= ppopupmenu->spmenu->cItems)
	{
		/*
		 * Selection is out of range, so fail.
		 */
		goto Exit;
	}

	/*
	 * Get a pointer to the currently selected item in this menu.
	 * Bug #17867 - the call can cause this thing to change, so reload it.
	 */
	pItem = &(ppopupmenu->spmenu->rgItems[ppopupmenu->posSelectedItem]);

	if (TestMFS(pItem, MFS_GRAYED) || (pItem->spSubMenu == NULL) || (pItem->spSubMenu->cItems == 0))
	{
		/*
		 * The item is disabled, no longer a popup, or empty so don't drop.
		 */
		/*
		 * No items in menu.
		 */
		goto Exit;
	}

	/*
	 * Let's make sure that the current thread is in menu mode and
	 *  it uses this pMenuState. Otherwise the window we're about to
	 *  create (or set the thread to) will point to a different pMenuState
	 */
	//UserAssert(ptiCurrent->pMenuState == pMenuState);

	//if (ppopupmenu->fIsMenuBar && (pdesk->spwndMenu != NULL) &&
	//	(!(pdesk->dwDTFlags & DF_MENUINUSE)) &&
	//	!TestWF(pdesk->spwndMenu, WFVISIBLE))
	//{

	//	pdesk->dwDTFlags |= DF_MENUINUSE;

	//	if (HMPheFromObject(pdesk->spwndMenu)->bFlags & HANDLEF_DESTROY)
	//	{
	//		PPROCESSINFO ppi = pdesk->rpwinstaParent->pTerm->ptiDesktop->ppi;
	//		PPROCESSINFO ppiSave;
	//		HWND         pwndMenu;
	//		DWORD        dwDisableHooks;

	//		/*
	//		 * the menu window is destroyed -- recreate it
	//		 *
	//		 * clear the desktopMenu flag so that the popup is
	//		 * freed.
	//		 */
	//		UserAssert(ppopupmenu->fDesktopMenu);

	//		ppopupmenu->fDesktopMenu = FALSE;
	//		ppopupmenu->fDelayedFree = FALSE;

	//		Unlock(&pdesk->spwndMenu);
	//		ppiSave  = ptiCurrent->ppi;
	//		ptiCurrent->ppi = ppi;

	//		/*
	//		 * HACK HACK HACK!!! (adams) In order to create the menu window
	//		 * with the correct desktop, we set the desktop of the current thread
	//		 * to the new desktop. But in so doing we allow hooks on the current
	//		 * thread to also hook this new desktop. This is bad, because we don't
	//		 * want the menu window to be hooked while it is created. So we
	//		 * temporarily disable hooks of the current thread or desktop,
	//		 * and reenable them after switching back to the original desktop.
	//		 */

	//		dwDisableHooks = ptiCurrent->TIF_flags & TIF_DISABLEHOOKS;
	//		ptiCurrent->TIF_flags |= TIF_DISABLEHOOKS;

	//		pwndMenu = xxxCreateWindowEx(
	//				WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE,
	//				(PLARGE_STRING)MENUCLASS,
	//				NULL,
	//				WS_POPUP | WS_BORDER,
	//				0,
	//				0,
	//				100,
	//				100,
	//				NULL,
	//				NULL,
	//				hModuleWin,
	//				NULL,
	//				WINVER);

	//		UserAssert(ptiCurrent->TIF_flags & TIF_DISABLEHOOKS);
	//		ptiCurrent->TIF_flags = (ptiCurrent->TIF_flags & ~TIF_DISABLEHOOKS) | dwDisableHooks;

	//		Lock(&(pdesk->spwndMenu), pwndMenu);

	//		UserAssert(((PMENUWND)pwndMenu)->ppopupmenu != NULL);

	//		/*
	//		 * Set the desktopMenu flag to mark that this is the popup
	//		 * allocated for pdesk->spwndMenu
	//		 * Unlock spwndPopupMenu to avoid this special case in the common code path.
	//		 */
	//		((PMENUWND)pwndMenu)->ppopupmenu->fDesktopMenu = TRUE;
	//		Unlock(&((PMENUWND)pwndMenu)->ppopupmenu->spwndPopupMenu);

	//		ptiCurrent->ppi = ppiSave;

	//		HMChangeOwnerThread(pdesk->spwndMenu, pdesk->rpwinstaParent->pTerm->ptiDesktop);
	//	}
	//	else
	//	{
	//		TL tlpwndDesk;

	//		ThreadLockAlways(pdesk->spwndMenu, &tlpwndT);
	//		ThreadLockAlways(pdesk->pDeskInfo->spwnd, &tlpwndDesk);
	//		xxxSetParent(pdesk->spwndMenu, pdesk->pDeskInfo->spwnd);
	//		ThreadUnlock(&tlpwndDesk);
	//		ThreadUnlock(&tlpwndT);

	//	}


	//	pwndHierarchy = pdesk->spwndMenu;
	//	Lock(&pwndHierarchy->spwndOwner, ppopupmenu->spwndNotify);
	//	pwndHierarchy->head.pti = ptiCurrent;

	//	/*
	//	 * Make the topmost state match the menu mode
	//	 */
	//	if ((TestWF(pdesk->spwndMenu, WEFTOPMOST) && pMenuState->fModelessMenu)
	//		|| (!TestWF(pdesk->spwndMenu, WEFTOPMOST) && !pMenuState->fModelessMenu))
	//	{

	//		ThreadLock(pdesk->spwndMenu, &tlpwndHierarchy);
	//		xxxSetWindowPos(pdesk->spwndMenu,
	//			(pMenuState->fModelessMenu ? HWND_NOTOPMOST : HWND_TOPMOST),
	//			0, 0, 0, 0,
	//			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
	//		ThreadUnlock(&tlpwndHierarchy);
	//	}

	//	ppopupmenuHierarchy = ((PMENUWND)pwndHierarchy)->ppopupmenu;

	//	UserAssert(ppopupmenuHierarchy->fDesktopMenu);
	//	/*
	//	 * clear any leftover data from the last time we used it
	//	 * Assert that we're not zapping any locks here
	//	 */
	//	UserAssert(ppopupmenuHierarchy->spwndPopupMenu == NULL);
	//	UserAssert(ppopupmenuHierarchy->spwndNextPopup == NULL);
	//	UserAssert(ppopupmenuHierarchy->spwndPrevPopup == NULL);
	//	UserAssert(ppopupmenuHierarchy->spmenu == NULL);
	//	UserAssert(ppopupmenuHierarchy->spmenuAlternate == NULL);
	//	UserAssert(ppopupmenuHierarchy->spwndNotify == NULL);
	//	UserAssert(ppopupmenuHierarchy->spwndActivePopup == NULL);

	//	RtlZeroMemory((PVOID)ppopupmenuHierarchy, sizeof(POPUPMENU));
	//	ppopupmenuHierarchy->fDesktopMenu = TRUE;

	//	ppopupmenuHierarchy->posSelectedItem = MFMWFP_NOITEM;
	//	Lock(&ppopupmenuHierarchy->spwndPopupMenu, pdesk->spwndMenu);

	//	Lock(&(ppopupmenuHierarchy->spwndNotify), ppopupmenu->spwndNotify);
	//	LockPopupMenu(ppopupmenuHierarchy, &ppopupmenuHierarchy->spmenu, pItem->spSubMenu);

	//}
	//else
	{

		ThreadLock(ppopupmenu->spwndNotify, &tlpwndT);

		pwndHierarchy = ::CreateWindowEx(
				WS_EX_TOOLWINDOW/* | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE*/,
				MENUCLASS,
				NULL,
				WS_POPUP | WS_BORDER,
				0, 0, 200, 100,
				ppopupmenu->spwndNotify,
				NULL,
				(HINSTANCE)GetWindowLong(ppopupmenu->spwndNotify, GWL_HINSTANCE),
				(LPVOID)pItem->spSubMenu);

		ThreadUnlock(&tlpwndT);

		if (!pwndHierarchy)
		{
			goto Exit;
		}
#ifdef _DEBUG
		g_pwndHierarchy = pwndHierarchy;
#endif

		/*
		 * Do this so old apps don't get weird borders on the popups of
		 * hierarchical items!
		 */
		//ClrWF(pwndHierarchy, WFOLDUI);

		ppopupmenuHierarchy = GetPopupMenu(pwndHierarchy);

	}

	/*
	 * Mark this as fDelayedFree and link it
	 */
	ppopupmenuHierarchy->fDelayedFree = TRUE;
	ppopupmenuHierarchy->ppmDelayedFree = ppopupmenu->ppopupmenuRoot->ppmDelayedFree;
	ppopupmenu->ppopupmenuRoot->ppmDelayedFree = ppopupmenuHierarchy;

#ifdef USE_MIRRORING
	if (TestWF(ppopupmenu->spwndPopupMenu, WEFLAYOUTRTL))
	{
		SetWF(pwndHierarchy, WEFLAYOUTRTL);
	}
	else
	{
		ClrWF(pwndHierarchy, WEFLAYOUTRTL);
	}
#endif

	//
	// 为ppopupmenuHierarchy各成员赋值
	//
	Lock(&(ppopupmenuHierarchy->spwndNotify), ppopupmenu->spwndNotify);
#if DBG
	/*
	 * We should associate ppopupmenuHierarchy to the same menu we sent the
	 *  WM_INITMsENUPOPUP message. Otherwise, the WM_UNINITMENUPOPUP
	 *  will go to the wrong window. It would be the app's fault...
	 */
	if (!ppopupmenu->fNoNotify && (hmenuInit != PtoHq(pItem->spSubMenu)))
	{
		RIPMSG2(RIP_WARNING, "xxxMNOpenHierarchy: bad app changed submenu from %#p to %#p",
			hmenuInit, PtoHq(pItem->spSubMenu));
	}
#endif
	LockPopupMenu(ppopupmenuHierarchy, &ppopupmenuHierarchy->spmenu, pItem->spSubMenu);
	Lock(&(ppopupmenu->spwndNextPopup), pwndHierarchy);
	ppopupmenu->posDropped              = ppopupmenu->posSelectedItem;
	Lock(&(ppopupmenuHierarchy->spwndPrevPopup), ppopupmenu->spwndPopupMenu);
	ppopupmenuHierarchy->ppopupmenuRoot = ppopupmenu->ppopupmenuRoot;
	ppopupmenuHierarchy->fHasMenuBar = ppopupmenu->fHasMenuBar;
	ppopupmenuHierarchy->fIsSysMenu = ppopupmenu->fIsSysMenu;
	ppopupmenuHierarchy->fNoNotify      = ppopupmenu->fNoNotify;
	ppopupmenuHierarchy->fSendUninit = TRUE;
	ppopupmenuHierarchy->fRtoL = ppopupmenu->fRtoL;

	/*
	 * The menu window has been created and intialized so if
	 *  something fails, the WM_UNINITMENUPOPUP message will
	 *  be sent from xxxMNDestroyHandler
	 */
	fSendUninit = FALSE;

	/*
	 * Set/clear the underline flag
	 */
	if (pMenuState->fUnderline)
	{
		SetMF(ppopupmenuHierarchy->spmenu, MFUNDERLINE);
	}
	else
	{
		ClearMF(ppopupmenuHierarchy->spmenu, MFUNDERLINE);
	}

	ppopupmenuHierarchy->fAboutToHide   = FALSE;

	/*
	 * Find the size of the menu window and actually size it (wParam = 1)
	 */
	ThreadLock(pwndHierarchy, &tlpwndHierarchy);
	sizeHierarchy = (LONG)::SendMessage(pwndHierarchy, MN_SIZEWINDOW, MNSW_SIZE, 0);

	if (!sizeHierarchy)
	{
		/*
		 * No size for this menu so zero it and blow off.
		 */
		UserAssert(ppopupmenuHierarchy->fDelayedFree);
		if (ppopupmenuHierarchy->fDesktopMenu)
		{
			xxxMNDestroyHandler(ppopupmenuHierarchy);
			//xxxCleanupDesktopMenu(pwndHierarchy, pdesk);
		}

		//if (ThreadUnlock(&tlpwndHierarchy))
		//{
		//	if (!ppopupmenuHierarchy->fDesktopMenu)
		//	{
		//		xxxDestroyWindow(pwndHierarchy);
		//	}
		//}

		Unlock(&ppopupmenu->spwndNextPopup);
		goto Exit;
	}

	cxPopup = LOWORD(sizeHierarchy);
	cyPopup = HIWORD(sizeHierarchy);

	ppopupmenu->fHierarchyDropped = TRUE;

	/*
	 * Find out the x,y position to drop the hierarchy and the animation
	 *  direction
	 */
	ppopupmenuHierarchy->iDropDir = xxxMNPositionHierarchy(
			ppopupmenu, pItem, cxPopup, cyPopup, &xLeft, &yTop, &pMonitor);

	if (ppopupmenu->fIsMenuBar && GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		/*
		 * If the menu had to be pinned to the bottom of the screen and
		 * the mouse button is down, make sure the mouse isn't over the
		 * menu rect.
		 */
		RECT rc;
		RECT rcParent;
		int xrightdrop;
		int xleftdrop;

		/*
		 * Get rect of hierarchical
		 */
		CIUIRect rcWindow;
		::GetWindowRect(pwndHierarchy, rcWindow);
		CopyOffsetRect(
			&rc,
			&rcWindow,
			xLeft - rcWindow.left,
			yTop  - rcWindow.top);

		/*
		 * Get the rect of the menu bar popup item
		 */
		CIUIRect rcPopup;
		::GetWindowRect(ppopupmenu->spwndPopupMenu, rcPopup);

		rcParent.left = pItem->xItem + rcPopup.left;
		rcParent.top = pItem->yItem + rcPopup.top;
		rcParent.right = rcParent.left + pItem->cxItem;
		rcParent.bottom = rcParent.top + pItem->cyItem;

		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(pMonitor, &mi);

		if (IntersectRect(&rc, &rc, &rcParent))
		{

			/*
			 * Oh, oh...  The cursor will sit right on top of a menu item.
			 * If the user up clicks, a menu will be accidently selected.
			 *
			 * Calc x position of hierarchical if we dropped it to the
			 * right/left of the menu bar item.
			 */
			xrightdrop = rcPopup.left +
				pItem->xItem + pItem->cxItem + cxPopup;

			if (xrightdrop > mi.rcMonitor.right)
			{
				xrightdrop = 0;
			}

			xleftdrop = rcPopup.left +
				pItem->xItem - cxPopup;

			if (xleftdrop < mi.rcMonitor.left)
			{
				xleftdrop = 0;
			}

			if (((SYSMET(MENUDROPALIGNMENT) || TestMFT(pItem, MFT_RIGHTORDER))
					&& xleftdrop) || !xrightdrop)
			{
				xLeft = rcPopup.left +
					pItem->xItem - cxPopup;
				ppopupmenuHierarchy->iDropDir = PAS_LEFT;
			}
			else if (xrightdrop)
			{
				xLeft = rcPopup.left +
					pItem->xItem + pItem->cxItem;
				ppopupmenuHierarchy->iDropDir = PAS_RIGHT;
			}
		}
	}

	/*
	 * Take care of fDropNextPopup (menu bar) or fDroppedLeft (popups)
	 * Set animation flag
	 */
	if (ppopupmenu->fIsMenuBar)
	{
		/*
		 * Only the first popup being dropped off the menu bar
		 * is animated.
		 */
		if (!ppopupmenu->fDropNextPopup)
		{
			ppopupmenuHierarchy->iDropDir |= PAS_OUT;
		}

		/*
		 * Propagate right-to-left direction.
		 */
		if (ppopupmenu->fDroppedLeft || (ppopupmenuHierarchy->iDropDir == PAS_LEFT))
		{
			ppopupmenuHierarchy->fDroppedLeft = TRUE;
		}
		/*
		 * Once a popup is dropped from the menu bar, moving to the next
		 *  item on the menu bar should drop the popup.
		 */
		ppopupmenu->fDropNextPopup = TRUE;
	}
	else
	{
		/*
		 * Submenus always animate.
		 */
		ppopupmenuHierarchy->iDropDir |= PAS_OUT;

		/*
		 * Is this popup a lefty?
		 */
		if (ppopupmenuHierarchy->iDropDir == PAS_LEFT)
		{
			ppopupmenuHierarchy->fDroppedLeft = TRUE;
		}
	}

	/*
	 * The previous active dude must be visible
	 */
	UserAssert((ppopupmenu->ppopupmenuRoot->spwndActivePopup == NULL)
		|| ::IsWindowVisible(ppopupmenu->ppopupmenuRoot->spwndActivePopup));

	/*
	 * This is the new active popup
	 */
	Lock(&(ppopupmenu->ppopupmenuRoot->spwndActivePopup), pwndHierarchy);

	/*
	 * Paint the owner window before the popup menu comes up so that
	 * the proper bits are saved.
	 */
	if (ppopupmenuHierarchy->spwndNotify != NULL)
	{
		ThreadLockAlways(ppopupmenuHierarchy->spwndNotify, &tlpwndT);
		UpdateWindow(ppopupmenuHierarchy->spwndNotify);
		ThreadUnlock(&tlpwndT);
	}

	/*
	 * If this is a drag and drop menu, then we need to register the window
	 *  as a drop target.
	 */
	if (pMenuState->fDragAndDrop)
	{
		if (!NT_SUCCESS(xxxClientRegisterDragDrop(pwndHierarchy)))
		{
			//RIPMSG1(RIP_ERROR, "xxxMNOpenHierarchy: xxxClientRegisterDragDrop failed:%#p", pwndHierarchy);
		}
	}

	/*
	 * Show the window. Modeless menus are not topmost and get activated.
	 *  Modal menus are topmost but don't get activated.
	 */
	//PlayEventSound(USER_SOUND_MENUPOPUP);

	::SetWindowPos(pwndHierarchy,
		(pMenuState->fModelessMenu ? HWND_TOP : HWND_TOPMOST),
		xLeft, yTop, 0, 0,
		SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOOWNERZORDER
		| (pMenuState->fModelessMenu ? 0 : SWP_NOACTIVATE));

	//if (FWINABLE())
	//{
	//	xxxWindowEvent(EVENT_SYSTEM_MENUPOPUPSTART, pwndHierarchy, OBJID_CLIENT, INDEXID_CONTAINER, 0);
	//}

	/*
	 * Select the first item IFF we're in keyboard mode.  This fixes a
	 * surprising number of compatibility problems with keyboard macros,
	 * scripts, etc.
	 */
	if (pMenuState->mnFocus == KEYBDHOLD)
	{
		::SendMessage(pwndHierarchy, MN_SELECTITEM, 0, 0L);
	}

	/*
	 * This is needed so that popup menus are properly drawn on sys
	 * modal dialog boxes.
	 */
	UpdateWindow(pwndHierarchy);

	ret = pwndHierarchy;
	ThreadUnlock(&tlpwndHierarchy);

Exit:
	/*
	 * send matching WM_UNINITMENUPOPUP if needed (i.e, something
	 *  failed).
	 */
	if (fSendUninit
		&& (ppopupmenu->spwndNotify != NULL))
	{

		ThreadLockAlways(ppopupmenu->spwndNotify, &tlpwndT);
		::SendMessage(ppopupmenu->spwndNotify, WM_UNINITMENUPOPUP,
			(WPARAM)hmenuInit,
			MAKELONG(0, (ppopupmenu->fIsSysMenu ? MF_SYSMENU : 0)));
		ThreadUnlock(&tlpwndT);
	}

	return ret;
}

/***************************************************************************\
*
*  MNHideNextHierarchy()
*
*  Closes any submenu coming off of this popup.
*
\***************************************************************************/
BOOL xxxMNHideNextHierarchy(PPOPUPMENU ppopup)
{
	if (ppopup->spwndNextPopup != NULL)
	{
		//TL tlpwndT;

		ThreadLockAlways(ppopup->spwndNextPopup, &tlpwndT);
		if (ppopup->spwndNextPopup != ppopup->spwndActivePopup)
		{
			SendMessage(ppopup->spwndNextPopup, MN_CLOSEHIERARCHY, 0, 0L);
		}

		SendMessage(ppopup->spwndNextPopup, MN_SELECTITEM, (WPARAM)(-1), 0L);
		ThreadUnlock(&tlpwndT);
		return TRUE;
	}
	return FALSE;
}


/***************************************************************************\
* void MenuCloseHierarchyHandler(PPOPUPMENU ppopupmenu)
* effects: Close all hierarchies from this window down.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNCloseHierarchy(
	PPOPUPMENU ppopupmenu, PMENUSTATE pMenuState)
{
	//TL           tlpwndNext;
	//TL           tlpwnd;
	//TL           tlpopup;
	//PTHREADINFO  ptiCurrent = PtiCurrent();
	//PDESKTOP     pdesk;
	HWND         pwndNext;

	Validateppopupmenu(ppopupmenu);

	/*
	 * Terminate any animation
	 */
	//MNAnimate(pMenuState, FALSE);

	/*
	 * If a hierarchy exists, close all childen below us.  Do it in reversed
	 * order so savebits work out.
	 */
	if (!ppopupmenu->fHierarchyDropped)
	{
		/*
		 * Assert that there's no next or it might not get closed
		 */
		UserAssert(ppopupmenu->spwndNextPopup == NULL);
		return;
	}

	if (ppopupmenu->fHideTimer)
	{
		::KillTimer(ppopupmenu->spwndPopupMenu, IDSYS_MNHIDE);
		ppopupmenu->fHideTimer = FALSE;
	}

	pwndNext = ppopupmenu->spwndNextPopup;
	if (pwndNext != NULL)
	{

		ThreadLockAlways(pwndNext, &tlpwndNext);

		// 递归删除各级弹出的菜单
		::SendMessage(pwndNext, MN_CLOSEHIERARCHY, 0, 0);

		/*
		 * If modeless menu, activate the this popup since we're about
		 *  to destroy the current active one. We want to keep activation
		 *  on a menu window so we can get the keys. Also, modeless menus
		 *  are canceled when a non-menu window is activated in their queue
		 */
		if (pMenuState->fModelessMenu
			&& pMenuState->fInsideMenuLoop
			&& !ppopupmenu->fIsMenuBar)
		{

			ThreadLockAlways(ppopupmenu->spwndPopupMenu, &tlpwnd);
			SetActiveWindow(ppopupmenu->spwndPopupMenu);
			ThreadUnlock(&tlpwnd);
		}

		//if (FWINABLE())
		//{
		//	xxxWindowEvent(EVENT_SYSTEM_MENUPOPUPEND, pwndNext, OBJID_CLIENT, INDEXID_CONTAINER, 0);
		//}

		/*
		 * If the current thread is not in the right pdesk, then that could
		 *  be the cause of the stuck menu bug.
		 * In other words, are we nuking this menu out of context?
		 */
		//UserAssert(ptiCurrent->pMenuState != NULL);
		//pdesk = ptiCurrent->rpdesk;

		//		if (pwndNext == pdesk->spwndMenu)
		//		{
		//			PPOPUPMENU ppopupmenuReal;
		//
		//			UserAssert(pdesk->dwDTFlags & DF_MENUINUSE);
		//
		//			/*
		//			 * If this is our precreated real popup window,
		//			 * initialize ourselves and just hide.
		//			 */
		//			xxxShowWindow(pwndNext, SW_HIDE | TEST_PUDF(PUDF_ANIMATE));
		//
		//			/*
		//			 * Its possible that during Logoff the above xxxShowWindow
		//			 * won't get prossessed and because this window is a special
		//			 * window that is owned by they desktop we have to manually mark
		//			 * it as invisible.
		//			 */
		//			if (TestWF(pwndNext, WFVISIBLE))
		//			{
		//				SetVisible(pwndNext, SV_UNSET);
		//			}
		//
		//#ifdef HAVE_MN_GETPPOPUPMENU
		//			ppopupmenuReal = (PPOPUPMENU)SendMessage(pwndNext,
		//					MN_GETPPOPUPMENU, 0, 0L);
		//#else
		//			ppopupmenuReal = ((PMENUWND)pwndNext)->ppopupmenu;
		//#endif
		//			UserAssert(ppopupmenuReal->fDesktopMenu == TRUE);
		//
		//			/*
		//			 * We don't want this window to be a drop target anymore.
		//			 * Non cached menu windows revoke it on WM_FINALDESTROY.
		//			 */
		//			if (pMenuState->fDragAndDrop)
		//			{
		//				if (!NT_SUCCESS(xxxClientRevokeDragDrop(HW(pwndNext))))
		//				{
		//					RIPMSG1(RIP_ERROR, "xxxMNCloseHierarchy: xxxClientRevokeRegisterDragDrop failed:%#p", pwndNext);
		//				}
		//			}
		//
		//			if (ppopupmenuReal != NULL)
		//			{
		//				xxxMNDestroyHandler(ppopupmenuReal);
		//				/*
		//				 * We used to clear the popup contents here but the popup might be
		//				 *  still in use if this is happening during a callback. So we let
		//				 *  MNFreePopup do that. If it didn't happen during the call above,
		//				 *  it'll happen when MNFlushDestroyedPopups is executed.
		//				 */
		//			}
		//
		//			xxxCleanupDesktopMenu(pwndNext, pdesk);
		//
		//			ThreadUnlock(&tlpwndNext);
		//		}
		//		else  if (ThreadUnlock(&tlpwndNext))
		{
			/*
			 * We know this is not the current thread's desktop menu window.
			 * Let's assert that it's not the menu window of another desktop.
			 */
			//UserAssert(pwndNext != pwndNext->head.rpdesk->spwndMenu);
			::DestroyWindow(pwndNext);
		}

		Unlock(&ppopupmenu->spwndNextPopup);
		ppopupmenu->fHierarchyDropped = FALSE;

	}

	if (ppopupmenu->fIsMenuBar)
	{
		Unlock(&ppopupmenu->spwndActivePopup);
	}
	else
	{
		Lock(&(ppopupmenu->ppopupmenuRoot->spwndActivePopup),
			ppopupmenu->spwndPopupMenu);
	}

	if (pMenuState->fInsideMenuLoop &&
		(ppopupmenu->posSelectedItem != MFMWFP_NOITEM))
	{
		/*
		 * Send a menu select as if this item had just been selected.  This
		 * allows people to easily update their menu status bars when a
		 * hierarchy from this item has been closed.
		 */
		HWND pwnd = ppopupmenu->ppopupmenuRoot->spwndNotify;
		if (pwnd)
		{
			ThreadLockAlways(pwnd, &tlpwnd);
			ThreadLockAlways(ppopupmenu->spwndPopupMenu, &tlpopup);
			xxxSendMenuSelect(pwnd, ppopupmenu->spwndPopupMenu,
				ppopupmenu->spmenu, ppopupmenu->posSelectedItem);
			ThreadUnlock(&tlpopup);
			ThreadUnlock(&tlpwnd);
		}
	}

}

/***************************************************************************\
*
*  MNDoubleClick()
*
*  If an item isn't a hierarchical, then the double-click works just like
*  single click did.  Otherwise, we traverse the submenu hierarchy to find
*  a valid default element.  If we reach a submenu that has no valid default
*  subitems and it itself has a valid ID, that becomes the valid default
*  element.
*
*  Note:   This function does not remove the double click message
*          from the message queue, so the caller must do so.
*
*  BOGUS
*  How about opening the hierarchies if we don't find anything?
*
*  Returns TRUE if handled.
*
\***************************************************************************/
BOOL IUI::xxxMNDoubleClick(PMENUSTATE pMenuState, PPOPUPMENU ppopup, int idxItem)
{
	PMENU  pMenu;
	PITEM  pItem;
	MSG   msg;
	UINT uPos;

	/*
	 * This code to swallow double clicks isn't executed!  MNLoop will
	 * swallow all double clicks for us.  Swallow the up button for the
	 * double dude instead.  Word will not be happy if they get a spurious
	 * WM_LBUTTONUP on the menu bar if their code to close the MDI child
	 * doesn't swallow it soon enough.
	 */

	/*
	 * Eat the click.
	 */
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOYIELD))
	{
		if ((msg.message == WM_LBUTTONUP) ||
			(msg.message == WM_NCLBUTTONUP))
		{
			PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
		}
#if DBG
		else if (msg.message == WM_LBUTTONDBLCLK ||
			msg.message == WM_NCLBUTTONDBLCLK)
		{
			UserAssertMsg0(FALSE, "xxxMNDoubleClick found a double click");
		}
#endif
	}

	/*
	 * Get current item.
	 */
	pMenu = ppopup->spmenu;
	if ((pMenu == NULL) || ((UINT)idxItem >= pMenu->cItems))
	{
		xxxMNDoScroll(ppopup, ppopup->posSelectedItem, FALSE);
		goto Done;
	}

	pItem = pMenu->rgItems + idxItem;
	uPos = idxItem;

	/*
	 * Do nothing if item is disabled.
	 */
	if (pItem->fState & MFS_GRAYED)
	{
		goto Done;
	}

	/*
	 * Traverse the hierarchy down as far as possible.
	 */
	do
	{
		if (pItem->spSubMenu != NULL)
		{
			/*
			 * The item is a popup menu, so continue traversing.
			 */
			pMenu = pItem->spSubMenu;
			idxItem = (UINT)_GetMenuDefaultItem(pMenu, MF_BYPOSITION, 0);

			if (idxItem != -1)
			{
				pItem = pMenu->rgItems + idxItem;
				uPos = idxItem;
				continue;
			}
			else   /* if (lpItem->wID == -1) How do we know this popup has an ID? */
			{
				break;
			}
		}

		/*
		 * We've found a leaf node of some kind, either a MFS_DEFAULT popup
		 * with a valid cmd ID that has no valid MFS_DEFAULT children, or
		 * a real cmd with MFS_DEFAULT style.
		 *
		 * Exit menu mode and send command ID.
		 */

		/*
		 * For old apps we need to generate a WM_MENUSELECT message first.
		 * Old apps, esp. Word 6.0, can't handle double-clicks on maximized
		 * child sys menus because they never get a WM_MENUSELECT for the
		 * item, unlike with normal keyboard/mouse choosing.  We need to
		 * fake it so they won't fault.  Several VB apps have a similar
		 * problem.
		 */
		//if (!TestWF(ppopup->ppopupmenuRoot->spwndNotify, WFWIN40COMPAT))
		//{
		//	TL tlpwndNotify, tlpopup;

		//	ThreadLock(ppopup->ppopupmenuRoot->spwndNotify, &tlpwndNotify);
		//	ThreadLock(ppopup->spwndPopupMenu, &tlpopup);
		//	xxxSendMenuSelect(ppopup->ppopupmenuRoot->spwndNotify,
		//		ppopup->spwndPopupMenu, pMenu, idxItem);
		//	ThreadUnlock(&tlpopup);
		//	ThreadUnlock(&tlpwndNotify);
		//}

		xxxMNDismissWithNotify(pMenuState, pMenu, pItem, uPos, 0);
		return TRUE;
	}
	while (TRUE);

Done:
	return (FALSE);
}


/***************************************************************************\
* UINT MenuSelectItemHandler(PPOPUPMENU ppopupmenu, int itemPos)
*
* Unselects the old selection, selects the item at itemPos and highlights it.
*
* MFMWFP_NOITEM if no item is to be selected.
*
* Returns the item flags of the item being selected.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

PITEM IUI::xxxMNSelectItem(
	PPOPUPMENU ppopupmenu,
	PMENUSTATE pMenuState,
	UINT itemPos)
{
	PITEM pItem = NULL;
	//TL tlpwndNotify;
	//TL tlpwndPopup;
	//TL tlpmenu;
	HWND pwndNotify = NULL;
	PMENU pmenu = NULL;

	if (ppopupmenu->posSelectedItem == itemPos)
	{

		/*
		 * If this item is already selectected, just return its flags.
		 */
		if ((itemPos != MFMWFP_NOITEM) && (itemPos < ppopupmenu->spmenu->cItems))
		{
			return &(ppopupmenu->spmenu->rgItems[itemPos]);
		}
		return NULL;
	}

	/*
	 * Terminate any animation
	 */
	//MNAnimate(pMenuState, FALSE);

	if (ppopupmenu->fShowTimer)
	{
		::KillTimer(ppopupmenu->spwndPopupMenu, IDSYS_MNSHOW);
		ppopupmenu->fShowTimer = FALSE;
	}

	ThreadLock(pmenu = ppopupmenu->spmenu, &tlpmenu);
	ThreadLock(pwndNotify = ppopupmenu->spwndNotify, &tlpwndNotify);

	if (ppopupmenu->fAboutToHide)
	{
		PPOPUPMENU ppopupPrev = GetPopupMenu(ppopupmenu->spwndPrevPopup);

		::KillTimer(ppopupPrev->spwndPopupMenu, IDSYS_MNHIDE);
		ppopupPrev->fHideTimer = FALSE;
		if (ppopupPrev->fShowTimer)
		{
			::KillTimer(ppopupPrev->spwndPopupMenu, IDSYS_MNSHOW);
			ppopupPrev->fShowTimer = FALSE;
		}

		if (ppopupPrev->posSelectedItem != ppopupPrev->posDropped)
		{
			//TL tlpmenuPopupMenuPrev;
			ThreadLock(ppopupPrev->spmenu, &tlpmenuPopupMenuPrev);
			if (ppopupPrev->posSelectedItem != MFMWFP_NOITEM)
			{
				xxxMNInvertItem(ppopupPrev, ppopupPrev->spmenu,
					ppopupPrev->posSelectedItem, ppopupPrev->spwndNotify, FALSE);
			}

			ppopupPrev->posSelectedItem = ppopupPrev->posDropped;

			xxxMNInvertItem(ppopupPrev, ppopupPrev->spmenu,
				ppopupPrev->posDropped, ppopupPrev->spwndNotify, TRUE);
			ThreadUnlock(&tlpmenuPopupMenuPrev);
		}

		ppopupmenu->fAboutToHide = FALSE;
		Lock(&ppopupmenu->ppopupmenuRoot->spwndActivePopup, ppopupmenu->spwndPopupMenu);
	}

	if (MNIsItemSelected(ppopupmenu))
	{
		/*
		 * Something else is selected so we need to unselect it.
		 */
		if (ppopupmenu->spwndNextPopup)
		{
			if (ppopupmenu->fIsMenuBar)
			{
				xxxMNCloseHierarchy(ppopupmenu, pMenuState);
			}
			else
			{
				MNSetTimerToCloseHierarchy(ppopupmenu);
			}
		}

		goto DeselectItem;
	}
	else if (MNIsScrollArrowSelected(ppopupmenu))
	{
		::KillTimer(ppopupmenu->spwndPopupMenu, ppopupmenu->posSelectedItem);
DeselectItem:

		xxxMNInvertItem(ppopupmenu, pmenu,
			ppopupmenu->posSelectedItem, pwndNotify, FALSE);
	}

	ppopupmenu->posSelectedItem = itemPos;

	if (itemPos != MFMWFP_NOITEM)
	{
		/*
		 * If an item is selected, no autodismiss plus this means
		 *  that the mouse is on the menu
		 */
		pMenuState->fAboutToAutoDismiss =
			pMenuState->fMouseOffMenu = FALSE;

		if (pMenuState->fButtonDown)
		{
			xxxMNDoScroll(ppopupmenu, itemPos, TRUE);
		}

		pItem = xxxMNInvertItem(ppopupmenu, pmenu,
				itemPos, pwndNotify, TRUE);
		ThreadUnlock(&tlpwndNotify);
		ThreadUnlock(&tlpmenu);
		return pItem;

	}
	//else if (FWINABLE())
	//{
	//	/*
	//	 * Notify that nothing is now focused in this menu.
	//	 */
	//	xxxWindowEvent(EVENT_OBJECT_FOCUS, ppopupmenu->spwndPopupMenu,
	//		((ppopupmenu->spwndNotify != ppopupmenu->spwndPopupMenu) ? OBJID_CLIENT :
	//			(ppopupmenu->fIsSysMenu ? OBJID_SYSMENU : OBJID_MENU)), 0, 0);
	//}

	ThreadUnlock(&tlpwndNotify);
	ThreadUnlock(&tlpmenu);

	if (ppopupmenu->spwndPrevPopup != NULL)
	{
		PPOPUPMENU pp;

		/*
		 * Get the popupMenu data for the previous menu
		 * Use the root popupMenu if the previous menu is the menu bar
		 */
		if (ppopupmenu->fHasMenuBar && (ppopupmenu->spwndPrevPopup ==
				ppopupmenu->spwndNotify))
		{
			pp = ppopupmenu->ppopupmenuRoot;
		}
		else
		{
#ifdef HAVE_MN_GETPPOPUPMENU
			TL tlpwndPrevPopup;
			ThreadLock(ppopupmenu->spwndPrevPopup, &tlpwndPrevPopup);
			pp = (PPOPUPMENU)SendMessage(ppopupmenu->spwndPrevPopup,
					MN_GETPPOPUPMENU, 0, 0L);
			ThreadUnlock(&tlpwndPrevPopup);
#else
			pp = GetPopupMenu(ppopupmenu->spwndPrevPopup);
#endif
		}

		/*
		 * Generate a WM_MENUSELECT for the previous menu to re-establish
		 * it's current item as the SELECTED item
		 */
		ThreadLock(pp->spwndNotify, &tlpwndNotify);
		ThreadLock(pp->spwndPopupMenu, &tlpwndPopup);
		xxxSendMenuSelect(pp->spwndNotify, pp->spwndPopupMenu, pp->spmenu, pp->posSelectedItem);
		ThreadUnlock(&tlpwndPopup);
		ThreadUnlock(&tlpwndNotify);
	}

	return NULL;
}

/***************************************************************************\
*
*  MNItemHitTest()
*
*  Given a hMenu and a point in screen coordinates, returns the position
*  of the item the point is in.  Returns -1 if no item exists there.
*
\***************************************************************************/
UINT MNItemHitTest(PMENU pMenu, HWND pwnd, POINT pt)
{
	PITEM  pItem;
	UINT    iItem;
	RECT    rect;

	//PTHREADINFO ptiCurrent = PtiCurrent();

	if (pMenu->cItems == 0)
	{
		return MFMWFP_NOITEM;
	}


	CIUIRect rcWindow;
	GetWindowRect(pwnd, rcWindow);
	CIUIRect rcClient;
	GetWindowRect(pwnd, rcClient);
	/*
	 * This point is screen-relative.  Menu bar coordinates relative
	 * to the window.  But popup menu coordinates are relative to the client.
	 */
	if (TestMF(pMenu, MFISPOPUP))
	{

		/*
		 * Bail if it's outside rcWindow
		 */

		CopyInflateRect(&rect, &rcWindow,
			-SYSMET(CXFIXEDFRAME), -SYSMET(CYFIXEDFRAME));

		if (!PtInRect(&rect, pt))
		{
			return MFMWFP_NOITEM;
		}

		/* ScreenToClient */
#ifdef USE_MIRRORING
		if (TestWF(pwnd, WEFLAYOUTRTL))
		{
			pt.x = pwnd->rcClient.right - pt.x;
		}
		else
#endif
		{
			pt.x -= rcClient.left;
		}
		pt.y -= rcClient.top;

		/*
		 * If on the non client area, then it's on the scroll arrows
		 */
		if (pt.y < 0)
		{
			return MFMWFP_UPARROW;
		}
		else if (pt.y > (int)pMenu->cyMenu)
		{
			return MFMWFP_DOWNARROW;
		}

	}
	else
	{
		/* ScreenToWindow */
#ifdef USE_MIRRORING
		if (TestWF(pwnd, WEFLAYOUTRTL) &&
			(
				(ptiCurrent->pq->codeCapture == SCREEN_CAPTURE) || (ptiCurrent->pq->codeCapture == NO_CAP_SYS)
			)
		)
		{
			pt.x = rcWindow.right - pt.x;
		}
		else
#endif
		{
			pt.x -= rcWindow.left;
		}
		pt.y -= rcWindow.top;
	}

	/*
	 * Step through all the items in the menu.
	 * If scrollable menu
	 */
	if (pMenu->dwArrowsOn != MSA_OFF)
	{
		UserAssert(TestMF(pMenu, MFISPOPUP));
		pItem = MNGetToppItem(pMenu);
		rect.left = rect.top = 0;
		rect.right = pItem->cxItem;
		rect.bottom = pItem->cyItem;
		for (iItem = pMenu->iTop; (iItem < (int)pMenu->cItems) && (rect.top < (int)pMenu->cyMenu); iItem++)
		{

			if (PtInRect(&rect, pt))
			{
				return iItem;
			}

			pItem++;
			rect.top = rect.bottom;
			rect.bottom += pItem->cyItem;
		}
	}
	else
	{
		/*
		 * No scroll bars.
		 */
		for (iItem = 0, pItem = pMenu->rgItems; iItem < pMenu->cItems; iItem++, pItem++)
		{
			/* Is the mouse inside this item's rectangle? */
			rect.left       = pItem->xItem;
			rect.top        = pItem->yItem;
			rect.right      = pItem->xItem + pItem->cxItem;
			rect.bottom     = pItem->yItem + pItem->cyItem;

			if (PtInRect(&rect, pt))
			{
				return (iItem);
			}
		}
	}


	return (MFMWFP_NOITEM);
}
/***************************************************************************\
* LockMFMWFPWindow
*
* This function is called when we need to save the return value of
*  xxxMNFindWindowFromPoint.
*
* History:
* 11/14/96  GerardoB  Created
\***************************************************************************/
void IUI::LockMFMWFPWindow(PULONG_PTR puHitArea, ULONG_PTR uNewHitArea)
{
	/*
	 * Bail if there is nothing to do.
	 */
	if (*puHitArea == uNewHitArea)
	{
		return;
	}

	/*
	 * Unlock current hit area
	 */
	UnlockMFMWFPWindow(puHitArea);

	/*
	 * Lock new hit area
	 */
	if (IsMFMWFPWindow(uNewHitArea))
	{
		Lock(puHitArea, uNewHitArea);
	}
	else
	{
		*puHitArea = uNewHitArea;
	}
}
/***************************************************************************\
* UnlockMFMWFPWindow
*
* You must called this if you ever called LockMFMWFPWindow
*
* History:
* 11/14/96  GerardoB  Created
\***************************************************************************/
void IUI::UnlockMFMWFPWindow(PULONG_PTR puHitArea)
{
	if (IsMFMWFPWindow(*puHitArea))
	{
		Unlock(puHitArea);
	}
	else
	{
		*puHitArea = MFMWFP_OFFMENU;
	}
}
/***************************************************************************\
* IsMFMWFPWindow
*
* Test whether or not the return value of xxxMNFindWindowFromPoint is
*  a window. Not that uHitArea could be an HWND or a HWND.
*
* History:
*   10-02-96 GerardoB   Created
\***************************************************************************/
BOOL IUI::IsMFMWFPWindow(ULONG_PTR uHitArea)
{
	switch (uHitArea)
	{
	case MFMWFP_OFFMENU:
	case MFMWFP_NOITEM:
	case MFMWFP_ALTMENU:
		return FALSE;

	default:
		return TRUE;
	}
}
/***************************************************************************\
* LONG MenuFindMenuWindowFromPoint(
*         PPOPUPMENU ppopupmenu, PUINT pIndex, POINT screenPt)
*
* effects: Determines in which window the point lies.
*
* Returns
*   - HWND of the hierarchical menu the point is on,
*   - MFMWFP_ALTMENU if point lies on the alternate popup menu.
*   - MFMWFP_NOITEM if there is no item at that point on the menu or the
*      point lies on the menu bar.
*   - MFMWFP_OFFMENU if point lies elsewhere.
*
* Returns in pIndex
*   - the index of the item hit,
*   - MFMWFP_NOITEM if there is no item at that point on the menu or
*      point lies on the menu bar.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
*   8-11-92 Sanfords added MFMWFP_ constants
\***************************************************************************/

LONG_PTR IUI::xxxMNFindWindowFromPoint(
	PPOPUPMENU ppopupmenu,
	PUINT pIndex,
	POINTS screenPt)
{
	POINT pt;
	RECT rect;
	LONG_PTR longHit;
	UINT itemHit;
	HWND pwnd;
	//TL tlpwndT;
#ifdef USE_MIRRORING
	int cx;
#endif


	*pIndex = 0;

	if (ppopupmenu->spwndNextPopup)
	{

		/*
		 * Check if this point is on any of our children before checking if it
		 * is on ourselves.
		 */
		ThreadLockAlways(ppopupmenu->spwndNextPopup, &tlpwndT);
		longHit = ::SendMessage(ppopupmenu->spwndNextPopup,
				MN_FINDMENUWINDOWFROMPOINT, (WPARAM)&itemHit,
				MAKELONG(screenPt.x, screenPt.y));
		ThreadUnlock(&tlpwndT);

		/*
		 * If return value is an hwnd, convert to pwnd.
		 */
		if (IsMFMWFPWindow(longHit))
		{
			longHit = (LONG_PTR)(HWND)longHit;
		}

		if (longHit)
		{

			/*
			 * Hit occurred on one of our children.
			 */

			*pIndex = itemHit;
			return longHit;
		}
	}

	if (ppopupmenu->fIsMenuBar)
	{
		int cBorders;

		/*
		 * Check if this point is on the menu bar
		 */
		pwnd = ppopupmenu->spwndNotify;
		if (pwnd == NULL)
		{
			return MFMWFP_OFFMENU;
		}

		pt.x = screenPt.x;
		pt.y = screenPt.y;

		if (ppopupmenu->fIsSysMenu)
		{

			//if (!_HasCaptionIcon(pwnd))
			{
				/*
				 * no system menu rect to click in if it doesn't have an icon
				 */
				return (0L);
			}

			/*
			 * Check if this is a click on the system menu icon.
			 */
			//if (TestWF(pwnd, WFMINIMIZED))
			//{

			//	/*
			//	 * If the window is minimized, then check if there was a hit in
			//	 * the client area of the icon's window.
			//	 */

			//	/*
			//	 * Mikehar 5/27
			//	 * Don't know how this ever worked. If we are the system menu of an icon
			//	 * we want to punt the menus if the click occurs ANYWHERE outside of the
			//	 * menu.
			//	 * Johnc 03-Jun-1992 the next 4 lines were commented out for Mike's
			//	 * problem above but that made clicking on a minimized window with
			//	 * the system menu already up, bring down the menu and put it right
			//	 * up again (bug 10951) because the mnloop wouldn't swallow the mouse
			//	 * down click message.  The problem Mike mentions no longer shows up.
			//	 */

			//	if (PtInRect(&(rcWindow), pt))
			//	{
			//		return MFMWFP_NOITEM;
			//	}

			//	/*
			//	 * It's an iconic window, so can't be hitting anywhere else.
			//	 */
			//	return MFMWFP_OFFMENU;
			//}

			/*
			 * Check if we are hitting on the system menu rectangle on the top
			 * left of windows.
			 */
			rect.top = rect.left = 0;
			rect.right  = SYSMET(CXSIZE);
			rect.bottom = SYSMET(CYSIZE);

			cBorders = 4;//GetWindowBorders(pwnd->style, pwnd->ExStyle, TRUE, FALSE);

			CIUIRect rcWindow;
			::GetWindowRect(pwnd, rcWindow);
			OffsetRect(&rect, rcWindow.left + cBorders * SYSMET(CXBORDER),
				rcWindow.top + cBorders * SYSMET(CYBORDER));
#ifdef USE_MIRRORING
			//Mirror the rect because the buttons in the left hand side of the window if it mirrored
			if (TestWF(pwnd, WEFLAYOUTRTL))
			{
				cx         = rect.right - rect.left;
				rect.right = rcWindow.right - (rect.left - rcWindow.left);
				rect.left  = rect.right - cx;
			}
#endif
			if (PtInRect(&rect, pt))
			{
				*pIndex = 0;
				return (MFMWFP_NOITEM);
			}
			/*
			 * Check if we hit in the alternate menu if available.
			 */
			if (ppopupmenu->spmenuAlternate)
			{
				itemHit = MNItemHitTest(ppopupmenu->spmenuAlternate, pwnd, pt);
				if (itemHit != MFMWFP_NOITEM)
				{
					*pIndex = itemHit;
					return MFMWFP_ALTMENU;
				}
			}
			return MFMWFP_OFFMENU;
		}
		else
		{
			if (IsIconic(ppopupmenu->spwndNotify))
			{

				/*
				 * If we are minimized, we can't hit on the main menu bar.
				 */
				return MFMWFP_OFFMENU;
			}
		}
	}
	else
	{
		pwnd = ppopupmenu->spwndPopupMenu;

		/*
		 * else this is a popup window and we need to check if we are hitting
		 * anywhere on this popup window.
		 */
		pt.x = screenPt.x;
		pt.y = screenPt.y;
		CIUIRect rcWindow;
		::GetWindowRect(pwnd, rcWindow);
		if (!PtInRect(&rcWindow, pt))
		{

			/*
			 * Point completely outside the popup menu window so return 0.
			 */
			return MFMWFP_OFFMENU;
		}
	}

	pt.x = screenPt.x;
	pt.y = screenPt.y;

	itemHit = MNItemHitTest(ppopupmenu->spmenu, pwnd, pt);

	if (ppopupmenu->fIsMenuBar)
	{

		/*
		 * If hit is on menu bar but no item is there, treat it as if the user
		 * hit nothing.
		 */
		if (itemHit == MFMWFP_NOITEM)
		{

			/*
			 * Check if we hit in the alternate menu if available.
			 */
			if (ppopupmenu->spmenuAlternate)
			{
				itemHit = MNItemHitTest(ppopupmenu->spmenuAlternate, pwnd, pt);
				if (itemHit != MFMWFP_NOITEM)
				{
					*pIndex = itemHit;
					return MFMWFP_ALTMENU;
				}
			}
			return MFMWFP_OFFMENU;
		}

		*pIndex = itemHit;
		return MFMWFP_NOITEM;
	}
	else
	{

		/*
		 * If hit is on popup menu but no item is there, itemHit
		 * will be MFMWFP_NOITEM
		 */
		*pIndex = itemHit;
		return (LONG_PTR)pwnd;
	}
	return MFMWFP_OFFMENU;
}

/***************************************************************************\
*void MenuCancelMenus(PPOPUPMENU ppopupmenu,
*                                UINT cmd, BOOL fSend)
* Should only be sent to the top most ppopupmenu/menu window in the
* hierarchy.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNCancel(
	PMENUSTATE pMenuState,
	UINT uMsg,
	UINT cmd,
	LPARAM lParam)
{
	PPOPUPMENU ppopupmenu = pMenuState->pGlobalPopupMenu;
	BOOL fSynchronous   = ppopupmenu->fSynchronous;
	BOOL fTrackFlagsSet = ppopupmenu->fIsTrackPopup;
	BOOL fIsSysMenu     = ppopupmenu->fIsSysMenu;
	BOOL fIsMenuBar     = ppopupmenu->fIsMenuBar;
	BOOL fNotify        = !ppopupmenu->fNoNotify;
	HWND pwndT;
	//TL tlpwndT;
	//TL tlpwndPopupMenu;

	Validateppopupmenu(ppopupmenu);

	pMenuState->fInsideMenuLoop = FALSE;
	pMenuState->fButtonDown = FALSE;
	/*
	 * Mark the popup as destroyed so people will not use it anymore.
	 * This means that root popups can be marked as destroyed before
	 * actually being destroyed (nice and confusing).
	 */
	ppopupmenu->fDestroyed = TRUE;

	/*
	 * Only the menu loop owner can destroy the menu windows (i.e, xxxMNCloseHierarchy)
	 */
	//if (PtiCurrent() != pMenuState->ptiMenuStateOwner)
	//{
	//	RIPMSG1(RIP_WARNING, "xxxMNCancel: Thread %#p doesn't own the menu loop", PtiCurrent());
	//	return;
	//}

	/*
	 * If the menu loop is running on a thread different than the thread
	 *  that owns spwndNotify, we can have two threads trying to cancel
	 *  this popup at the same time.
	 */
	if (ppopupmenu->fInCancel)
	{
		//RIPMSG1(RIP_WARNING, "xxxMNCancel: already in cancel. ppopupmenu:%#p", ppopupmenu);
		return;
	}
	ppopupmenu->fInCancel = TRUE;

	ThreadLock(ppopupmenu->spwndPopupMenu, &tlpwndPopupMenu);

	/*
	 * Close all hierarchies from this point down.
	 */
	xxxMNCloseHierarchy(ppopupmenu, pMenuState);

	/*
	 * Unselect any items on this top level window
	 */
	xxxMNSelectItem(ppopupmenu, pMenuState, MFMWFP_NOITEM);

	pMenuState->fMenuStarted = FALSE;

	pwndT = ppopupmenu->spwndNotify;

	ThreadLock(pwndT, &tlpwndT);

	xxxMNReleaseCapture();

	if (fTrackFlagsSet)
	{
		/*
		 * Send a POPUPEND so people watching see them paired
		 */
		//if (FWINABLE())
		//{
		//	xxxWindowEvent(EVENT_SYSTEM_MENUPOPUPEND,
		//		ppopupmenu->spwndPopupMenu, OBJID_CLIENT, 0, 0);
		//}

		//UserAssert(ppopupmenu->spwndPopupMenu != ppopupmenu->spwndPopupMenu->head.rpdesk->spwndMenu);
		::DestroyWindow(ppopupmenu->spwndPopupMenu);
	}

	if (pwndT == NULL)
	{
		ThreadUnlock(&tlpwndT);
		ThreadUnlock(&tlpwndPopupMenu);
		return;
	}

	/*
	 * SMS_NOMENU hack so we can send MenuSelect messages with
	 * (loword(lparam) = -1) when
	 * the menu pops back up for the CBT people. In 3.0, all WM_MENUSELECT
	 * messages went through the message filter so go through the function
	 * SendMenuSelect. We need to do this in 3.1 since WordDefect for Windows
	 * depends on this.
	 */
	xxxSendMenuSelect(pwndT, NULL, SMS_NOMENU, MFMWFP_NOITEM);

	//if (FWINABLE())
	//{
	//	xxxWindowEvent(EVENT_SYSTEM_MENUEND, pwndT, (fIsSysMenu ?
	//			OBJID_SYSMENU : (fIsMenuBar ? OBJID_MENU : OBJID_WINDOW)),
	//		INDEXID_CONTAINER, 0);
	//}

	if (fNotify)
	{
		/*
		 * Notify app we are exiting the menu loop.  Mainly for WinOldApp 386.
		 * wParam is 1 if a TrackPopupMenu else 0.
		 */
		::SendMessage(pwndT, WM_EXITMENULOOP,
			((fTrackFlagsSet && !fIsSysMenu) ? 1 : 0), 0);
	}

	if (uMsg != 0)
	{
		//PlayEventSound(USER_SOUND_MENUCOMMAND);
		pMenuState->cmdLast = cmd;
		if (!fSynchronous)
		{
			if (!fIsSysMenu && fTrackFlagsSet/* && !TestWF(pwndT, WFWIN31COMPAT)*/)
			{
				::SendMessage(pwndT, uMsg, cmd, lParam);
			}
			else
			{
				::PostMessage(pwndT, uMsg, cmd, lParam);
			}
		}
	}
	else
	{
		pMenuState->cmdLast = 0;
	}

	ThreadUnlock(&tlpwndT);

	ThreadUnlock(&tlpwndPopupMenu);

}
/***************************************************************************\
* void MenuButtonDownHandler(PPOPUPMENU ppopupmenu, int posItemHit)
* effects: Handles a mouse down on the menu associated with ppopupmenu at
* item index posItemHit.  posItemHit could be MFMWFP_NOITEM if user hit on a
* menu where no item exists.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNButtonDown(
	PPOPUPMENU ppopupmenu,
	PMENUSTATE pMenuState,
	UINT posItemHit, BOOL fClick)
{
	PITEM  pItem;
	BOOL   fOpenHierarchy;

	/*
	 * A different item was hit than is currently selected, so select it
	 * and drop its menu if available.  Make sure we toggle click state.
	 */
	if (ppopupmenu->posSelectedItem != posItemHit)
	{
		/*
		 * We are clicking on a new item, not moving the mouse over to it.
		 * So reset cancel toggle state.  We don't want button up from
		 * this button down to cancel.
		 */
		if (fClick)
		{
			fOpenHierarchy = TRUE;
			ppopupmenu->fToggle = FALSE;
		}
		else
		{
			fOpenHierarchy = (ppopupmenu->fDropNextPopup != 0);
		}


		/*
		 * If the item has a popup and isn't disabled, open it.  Note that
		 * selecting this item will cancel any hierarchies associated with
		 * the previously selected item.
		 */
		pItem = xxxMNSelectItem(ppopupmenu, pMenuState, posItemHit);
		if (MNIsPopupItem(pItem) && fOpenHierarchy)
		{
			/* Punt if menu was destroyed. */
			if (xxxMNOpenHierarchy(ppopupmenu, pMenuState) == (HWND)(-1))
			{
				return;
			}
		}
	}
	else
	{
		/*
		 * We are moving over to the already-selected item.  If we are
		 * clicking for real, reset cancel toggle state.  We want button
		 * up to cancel if on same item.  Otherwise, do nothing if just
		 * moving...
		 */
		if (fClick)
		{
			ppopupmenu->fToggle = TRUE;
		}

		if (!xxxMNHideNextHierarchy(ppopupmenu) && fClick && xxxMNOpenHierarchy(ppopupmenu, pMenuState))
		{
			ppopupmenu->fToggle = FALSE;
		}
	}

	if (fClick)
	{
		pMenuState->fButtonDown = TRUE;
		xxxMNDoScroll(ppopupmenu, posItemHit, TRUE);
	}
}
UINT gdtMNDropDown = 400;

/***************************************************************************\
* MNSetTimerToAutoDissmiss
*
* History:
*  11/14/96 GerardoB  Created
\***************************************************************************/
void MNSetTimerToAutoDismiss(PMENUSTATE pMenuState, HWND pwnd)
{
	if (pMenuState->fAutoDismiss && !pMenuState->fAboutToAutoDismiss)
	{
		if (SetTimer(pwnd, IDSYS_MNAUTODISMISS, 16 * gdtMNDropDown, NULL))
		{
			pMenuState->fAboutToAutoDismiss = TRUE;
		}
		else
		{
			RIPMSG0(RIP_WARNING, "xxxMNMouseMove: Failed to set autodismiss timer");
		}
	}
}
/***************************************************************************\
* void MenuMouseMoveHandler(PPOPUPMENU ppopupmenu, POINT screenPt)
* Handles a mouse move to the given point.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNMouseMove(
	PPOPUPMENU ppopup,
	PMENUSTATE pMenuState,
	POINTS ptScreen)
{
	LONG_PTR cmdHitArea;
	UINT uFlags;
	UINT cmdItem;
	HWND pwnd;
	//TL tlpwndT;


	if (!IsRootPopupMenu(ppopup))
	{
		RIPMSG0(RIP_ERROR,
			"MenuMouseMoveHandler() called for a non top most menu");
		return;
	}

	/*
	 * Ignore mouse moves that aren't really moves.  MSTEST jiggles
	 * the mouse for some reason.  And windows coming and going will
	 * force mouse moves, to reset the cursor.
	 */
	if ((ptScreen.x == pMenuState->ptMouseLast.x) && (ptScreen.y == pMenuState->ptMouseLast.y))
	{
		return;
	}

	pMenuState->ptMouseLast.x = ptScreen.x;
	pMenuState->ptMouseLast.y = ptScreen.y;

	/*
	 * Find out where this mouse move occurred.
	 */
	cmdHitArea = xxxMNFindWindowFromPoint(ppopup, &cmdItem, ptScreen);

	/*
	 * If coming from an IDropTarget call out, remember the hit test
	 */
	if (pMenuState->fInDoDragDrop)
	{
		xxxMNUpdateDraggingInfo(pMenuState, cmdHitArea, cmdItem);
	}

	if (pMenuState->mnFocus == KEYBDHOLD)
	{
		/*
		 * Ignore mouse moves when in keyboard mode if the mouse isn't over any
		 * menu at all.  Also ignore mouse moves if over minimized window,
		 * because we pretend that its entire window is like system menu.
		 */
		if ((cmdHitArea == MFMWFP_OFFMENU) ||
			((cmdHitArea == MFMWFP_NOITEM) && IsIconic(ppopup->spwndNotify)))
		{
			return;
		}

		pMenuState->mnFocus = MOUSEHOLD;
	}

	if (cmdHitArea == MFMWFP_ALTMENU)
	{
		/*
		 * User clicked in the other menu so switch to it ONLY IF
		 * MOUSE IS DOWN.  Usability testing proves that people frequently
		 * get kicked into the system menu accidentally when browsing the
		 * menu bar.  We support the Win3.1 behavior when the mouse is
		 * down however.
		 */
		if (pMenuState->fButtonDown)
		{
			xxxMNSwitchToAlternateMenu(ppopup);
			cmdHitArea = MFMWFP_NOITEM;
		}
		else
		{
			goto OverNothing;
		}
	}

	if (cmdHitArea == MFMWFP_NOITEM)
	{
		/*
		 * Mouse move occurred to an item in the main menu bar. If the item
		 * is different than the one already selected, close up the current
		 * one, select the new one and drop its menu. But if the item is the
		 * same as the one currently selected, we need to pull up any popups
		 * if needed and just keep the current level visible.  Hey, this is
		 * the same as a mousedown so lets do that instead.
		 */
		xxxMNButtonDown(ppopup, pMenuState, cmdItem, FALSE);
		return;
	}
	else if (cmdHitArea != 0)
	{
		/* This is a popup window we moved onto. */
		pwnd = (HWND)(cmdHitArea);
		ThreadLock(pwnd, &tlpwndT);

		UserAssert(::IsWindowVisible(pwnd));

		/*
		 * Modeless menus don't capture the mouse, so track it to know
		 *  when it leaves the popup.
		 */
		ppopup = GetPopupMenu(pwnd);
		if (pMenuState->fModelessMenu
			&& !pMenuState->fInDoDragDrop
			&& !ppopup->fTrackMouseEvent)
		{

			TRACKMOUSEEVENT tme;

			/* tme.cbSize = sizeof(TRACKMOUSEEVENT); Not checked on kernel side */
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = pwnd;
			TrackMouseEvent(&tme);
			ppopup->fTrackMouseEvent = TRUE;

			/*
			 * We just entered this window so make sure the cursor
			 *  is properly set.
			 */
			::SendMessage(pwnd, WM_SETCURSOR, (WPARAM)pwnd, MAKELONG(MSGF_MENU, 0));

		}

		/*
		 * Select the item.
		 */
		uFlags = (UINT)::SendMessage(pwnd, MN_SELECTITEM, (WPARAM)cmdItem, 0L);
		if ((uFlags & MF_POPUP) && !(uFlags & MFS_GRAYED))
		{
			/*
			 * User moved back onto an item with a hierarchy.  Hide the
			 * the dropped popup.
			 */
			// SendMessage(pwnd, MN_SETTIMERTOOPENHIERARCHY)发起了一次用户模式回调。
			// 在这次回调中，攻击者可以销毁Menu窗口从而释放tagPOPUPMENU对象并占位重用。
			// 当回调返回内核之后，如果xxxMNmouseMove并没有对已释放的ppopup进行验证。
			// 之后ppopup被传入xxxMNHideNextHierarchy，
			// xxxMNHideNextHierarchy会对tagPOPUPMENU.spwndNextPopup发送消息，
			// 攻击者创建合适的对象占用被释放的tagPOPUPMENU内存，
			// 构造好tagPOPUPMENU.spwndNextPopup的数据，即可达成内核任意代码执行
			// 见微软补丁：CVE-2015-2546
			// 参考：http://xlab.baidu.com/cve-2015-2546%EF%BC%9A%E4%BB%8E%E8%A1%A5%E4%B8%81%E6%AF%94%E5%AF%B9%E5%88%B0exploit/
			if (!::SendMessage(pwnd, MN_SETTIMERTOOPENHIERARCHY, 0, 0L))
			{
				xxxMNHideNextHierarchy(ppopup);
			}
		}
		ThreadUnlock(&tlpwndT);
	}
	else
OverNothing:
	{
		/* We moved off all menu windows... */
		if (ppopup->spwndActivePopup != NULL)
		{
			pwnd = ppopup->spwndActivePopup;

			ThreadLock(pwnd, &tlpwndT);
			::SendMessage(pwnd, MN_SELECTITEM, MFMWFP_NOITEM, 0L);
			MNSetTimerToAutoDismiss(pMenuState, pwnd);
			ThreadUnlock(&tlpwndT);
		}
		else
		{
			xxxMNSelectItem(ppopup, pMenuState, MFMWFP_NOITEM);
		}

	}
}


/***************************************************************************\
* void MenuButtonUpHandler(PPOPUPMENU ppopupmenu, int posItemHit)
* effects: Handles a mouse button up at the given point.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/

void IUI::xxxMNButtonUp(
	PPOPUPMENU ppopup,
	PMENUSTATE pMenuState,
	UINT posItemHit,
	LPARAM lParam)
{
	PITEM pItem;

	if (!pMenuState->fButtonDown)
	{

		/*
		 * Ignore if button was never down...  Really shouldn't happen...
		 */
		return;
	}

	if (posItemHit == MFMWFP_NOITEM)
	{
		RIPMSG0(RIP_WARNING, "button up on no item");
		goto ExitButtonUp;
	}

	if (ppopup->posSelectedItem != posItemHit)
	{
		goto ExitButtonUp;
	}

	if (ppopup->fIsMenuBar)
	{

		/*
		 * Handle button up in menubar specially.
		 */
		if (ppopup->fHierarchyDropped)
		{
			if (!ppopup->fToggle)
			{
				goto ExitButtonUp;
			}
			else
			{
				/*
				 * Cancel menu now.
				 */
				ppopup->fToggle = FALSE;
				xxxMNDismiss(pMenuState);
				return;
			}
		}
	}
	else if (ppopup->fShowTimer)
	{
		ppopup->fToggle = FALSE;

		/*
		 * Open hierarchy on popup
		 */
		xxxMNOpenHierarchy(ppopup, pMenuState);

		goto ExitButtonUp;
	}

	/*
	 * If nothing is selected, get out.  This occurs mainly on unbalanced
	 * multicolumn menus where one of the columns isn't completely full.
	 */
	if (ppopup->posSelectedItem == MFMWFP_NOITEM)
	{
		goto ExitButtonUp;
	}

	if (ppopup->posSelectedItem >= ppopup->spmenu->cItems)
	{
		goto ExitButtonUp;
	}

	/*
	 * Get a pointer to the currently selected item in this menu.
	 */
	pItem = &(ppopup->spmenu->rgItems[ppopup->posSelectedItem]);

	/*
	 * Kick out of menu mode if user clicked on a non-separator, enabled,
	 * non-hierarchical item.
	 *
	 * BOGUS
	 * Why doesn't MFS_GRAYED check work for separators now?  Find out later.
	 */
	if (!(pItem->fType & MFT_SEPARATOR)
		&& !(pItem->fState & MFS_GRAYED)
		&& (pItem->spSubMenu == NULL))
	{

		xxxMNDismissWithNotify(pMenuState, ppopup->spmenu, pItem,
			ppopup->posSelectedItem, lParam);
		return;
	}

ExitButtonUp:
	pMenuState->fButtonDown =
		pMenuState->fButtonAlwaysDown = FALSE;
}


/***************************************************************************\
*UINT MenuSetTimerToOpenHierarchy(PPOPUPMENU ppopupmenu)
* Given the current selection, set a timer to show this hierarchy if
* valid else return 0. If a timer should be set but couldn't return -1.
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
\***************************************************************************/
UINT MNSetTimerToOpenHierarchy(
	PPOPUPMENU ppopup)
{
	PITEM pItem;

	/*
	 * No selection so fail
	 */
	if (ppopup->posSelectedItem == MFMWFP_NOITEM)
	{
		return (0);
	}

	if (ppopup->posSelectedItem >= ppopup->spmenu->cItems)
	{
		return (0);
	}

	/*
	 * Is item an enabled popup?
	 * Get a pointer to the currently selected item in this menu.
	 */
	pItem = ppopup->spmenu->rgItems + ppopup->posSelectedItem;
	if ((pItem->spSubMenu == NULL) || (pItem->fState & MFS_GRAYED))
	{
		return (0);
	}

	if (ppopup->fShowTimer
		|| (ppopup->fHierarchyDropped
			&& (ppopup->posSelectedItem == ppopup->posDropped)))
	{

		/*
		 * A timer is already set or the hierarchy is already opened.
		 */
		return 1;
	}

	if (!SetTimer(ppopup->spwndPopupMenu, IDSYS_MNSHOW, gdtMNDropDown, NULL))
	{
		return (UINT)(-1);
	}

	ppopup->fShowTimer = TRUE;

	return 1;
}



/***************************************************************************\
* MNSetTimerToCloseHierarchy
*
\***************************************************************************/

UINT IUI::MNSetTimerToCloseHierarchy(PPOPUPMENU ppopup)
{

	if (!ppopup->fHierarchyDropped)
	{
		return (0);
	}

	if (ppopup->fHideTimer)
	{
		return (1);
	}

	if (!::SetTimer(ppopup->spwndPopupMenu, IDSYS_MNHIDE, gdtMNDropDown, NULL))
	{
		return (UINT)(-1);
	}

	ppopup->fHideTimer = TRUE;

	ppopup = GetPopupMenu(ppopup->spwndNextPopup);
	ppopup->fAboutToHide = TRUE;

	return (1);
}


/***************************************************************************\
* xxxCallHandleMenuMessages
*
* Modeless menus don't have a modal loop so we don't see the messages until
*  they are dispatched to xxxMenuWindowProc. So we call this function to
*  process the message just like we would in the modal case, only that
*  the message has already been pulled out of the queue.
* This is also calledfrom xxxScanSysQueue to pass mouse messages on the menu
*  bar or from xxxMNDragOver to upadate the mouse position when being draged over.
*
* History:
* 10/25/96 GerardoB  Created
\***************************************************************************/
BOOL IUI::xxxCallHandleMenuMessages(PMENUSTATE pMenuState, HWND pwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL fHandled;
	MSG msg;

	CheckLock(pwnd);
	CIUIRect rcClient;
	::GetClientRect(pwnd, rcClient);

	UserAssert(pMenuState->fModelessMenu || pMenuState->fInDoDragDrop);

	/*
	 * Since modeless menus don't capture the mouse, then we need to
	 *  keep checking on the mouse button when the mouse is off the
	 *  menu.
	 * Note that we do not set fMouseOffMenu if fInDoDragDrop is set
	 */
	if (pMenuState->fMouseOffMenu && pMenuState->fButtonDown)
	{
		UserAssert(!pMenuState->fInDoDragDrop && pMenuState->fModelessMenu);
		MNCheckButtonDownState(pMenuState);
	}

	/*
	 * Setup the msg structure
	 */
	msg.hwnd = pwnd;
	msg.message = message;
	msg.wParam = wParam;

	/*
	 * xxxHandleMenuMessages expects screen coordinates
	 */
	if ((message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST))
	{
		msg.lParam = MAKELONG(GET_X_LPARAM(lParam) + rcClient.left,
				GET_Y_LPARAM(lParam) + rcClient.top);
	}
	else
	{
		msg.lParam = lParam;
	}

	/*
	 * Not used by xxxHandleMenuMessages
	 */
	msg.time = 0;
	msg.pt.x = msg.pt.x = 0;


	UserAssert(pMenuState->pGlobalPopupMenu != NULL);

	pMenuState->fInCallHandleMenuMessages = TRUE;
	fHandled = xxxHandleMenuMessages(&msg, pMenuState, pMenuState->pGlobalPopupMenu);
	pMenuState->fInCallHandleMenuMessages = FALSE;

	/*
	 * If the message was handled and this is a modeless menu,
	 *  check to see if it's time to go.
	 */
	if (fHandled
		&& pMenuState->fModelessMenu
		&& ExitMenuLoop(pMenuState, pMenuState->pGlobalPopupMenu))
	{

		xxxEndMenuLoop(pMenuState, pMenuState->pGlobalPopupMenu);
		xxxMNEndMenuState(TRUE);
	}

	return fHandled;
}
/***************************************************************************\
*
* History:
*  05-25-91 Mikehar Ported from Win3.1
*  08-12-96 jparsons Catch NULL lParam on WM_CREATE [51986]
\***************************************************************************/

LRESULT WINAPI IUI::xxxMenuWindowProc(
	HWND pwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	BOOL fIsRecursedMenu;
	LRESULT lRet;
	PAINTSTRUCT ps;
	PPOPUPMENU ppopupmenu = NULL;
	PMENUSTATE pMenuState;
	PMENU      pmenu;
	PITEM      pItem;
	//TL tlpmenu;
	//TL tlpwndNotify;
	//PDESKTOP pdesk = pwnd->head.rpdesk;
	POINT ptOrg;
	HDC hdcAni;

	CheckLock(pwnd);

	CIUIRect rcClient;
	::GetClientRect(pwnd, rcClient);
	CIUIRect rcWindow;
	::GetWindowRect(pwnd, rcWindow);

	//VALIDATECLASSANDSIZE(pwnd, message, wParam, lParam, FNID_MENU, WM_NCCREATE);

	/*
	 * If we're not in menu mode or this window is just being created,
	 *  there are only few messages we care about.
	 */
	HWND pwndOwner = (HWND)GetWindowLong(pwnd, GWL_HWNDPARENT);

	// pMenuState应该保存到线程信息THREADINFO::pMenuState中，线程唯一。 目前暂时先做成全局变量。
	pMenuState = GetpMenuState(NULL);

	ppopupmenu = GetPopupMenu(pwnd);
	pmenu = (ppopupmenu != NULL ? ppopupmenu->spmenu : NULL);
	if ((pMenuState == NULL) || (pmenu == NULL))
	{
		switch (message)
		{
		case WM_NCCREATE:
		case WM_FINALDESTROY:
			break;

		case MN_SETHMENU:
			if (ppopupmenu != NULL)
			{
				break;
			}
			else
			{
				return 0;
			}

		default:
			goto CallDWP;
		}
	}
	else
	{
		/*
		 * TPM_RECURSE support: make sure we grab the proper pMenuState.
		 */
		fIsRecursedMenu = FALSE;//((ppopupmenu->ppopupmenuRoot != NULL) && IsRecursedMenuState(pMenuState, ppopupmenu));
		if (fIsRecursedMenu)
		{
			while (IsRecursedMenuState(pMenuState, ppopupmenu)
				&& (pMenuState->pmnsPrev != NULL))
			{
				pMenuState = pMenuState->pmnsPrev;
			}
			UserAssert(pMenuState->pGlobalPopupMenu == ppopupmenu->ppopupmenuRoot);
		}

		Validateppopupmenu(ppopupmenu);

		/*
		 * If this is a modeless menu, give xxxHandleMenuMessages the first
		 *  shot at the message
		 */
		if (pMenuState->fModelessMenu && !pMenuState->fInCallHandleMenuMessages)
		{
			/*
			 * If this is a recursed menu, we don't want to process any
			 *  input for it until the current menu goes away.
			 */
			if (fIsRecursedMenu)
			{
				if (((message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST))
					|| ((message >= WM_KEYFIRST) && (message <= WM_KEYLAST))
					|| ((message >= WM_NCMOUSEFIRST) && (message <= WM_NCMOUSELAST)))
				{

					goto CallDWP;
				}
			}
			else
			{
				if (xxxCallHandleMenuMessages(pMenuState, pwnd, message, wParam, lParam))
				{
					return 0;
				}
			}
		}
	} /* else of if ((pMenuState == NULL) || (ppopupmenu == NULL)) */

	switch (message)
	{
	case WM_NCCREATE:
	{
		/*
		 * Ignore evil messages to prevent leaks.
		 * Use RIP_ERROR for a while to make sure to see if we're getting here
		 */

		//if (((PMENUWND)pwnd)->ppopupmenu != NULL)
		//{
		//	//RIPMSG1(RIP_ERROR, "xxxMenuWindowProc: evil WM_NCCREATE. already initialized. pwnd:%p", pwnd);
		//	return FALSE;
		//}
		ppopupmenu = MNAllocPopup(TRUE);
		if (ppopupmenu == NULL)
		{
			return FALSE;
		}
		SetPopupMenu(pwnd, ppopupmenu);
		ppopupmenu->posSelectedItem = MFMWFP_NOITEM;
		Lock(&(ppopupmenu->spwndPopupMenu), pwnd);
	}
	return TRUE;

	case WM_NCCALCSIZE:
		DefWindowProc(pwnd, message, wParam, lParam);
		if (pmenu->dwArrowsOn != MSA_OFF)
		{
			InflateRect((PRECT)lParam, 0, -gcyMenuScrollArrow);
		}
		break;

	//case WM_ERASEBKGND:
	//	if (pmenu->hbrBack != NULL)
	//	{
	//		MNEraseBackground((HDC) wParam, pmenu,
	//			0, 0,
	//			rcClient.right - rcClient.left,
	//			rcClient.bottom - rcClient.top);
	//		return TRUE;
	//	}
	//	else
	//	{
	//		goto CallDWP;
	//	}
	//	break;

	case WM_PRINT:
		/*
		 * default processing of WM_PRINT does not handle custom non-
		 * client painting -- which scrollable menus have -- so take
		 * care of drawing nonclient area and then let DefWindowProc
		 * handle the rest
		 */
		//if ((lParam & PRF_NONCLIENT) && (pmenu->dwArrowsOn != MSA_OFF))
		//{

		//	MNDrawFullNC(pwnd, (HDC)wParam, ppopupmenu);
		//	GreGetWindowOrg((HDC)wParam, &ptOrg);
		//	GreSetWindowOrg((HDC)wParam,
		//		ptOrg.x - MNXBORDER,
		//		ptOrg.y - MNYBORDER - gcyMenuScrollArrow,
		//		NULL);
		//	DefWindowProc(pwnd, message, wParam, lParam & ~PRF_NONCLIENT);
		//	GreSetWindowOrg((HDC)wParam, ptOrg.x, ptOrg.y, NULL);

		//}
		//else
	{
		goto CallDWP;
	}
	break;

	case WM_WINDOWPOSCHANGING:
	{
		if (!(((LPWINDOWPOS)lParam)->flags & SWP_SHOWWINDOW))
		{
			goto CallDWP;
		}

		//		if (!TestEffectUP(MENUANIMATION) || !(ppopupmenu->iDropDir & PAS_OUT)
		//			|| (GetAppCompatFlags2(VER40) & GACF2_ANIMATIONOFF))
		//		{
		//NoAnimation:
		//			ppopupmenu->iDropDir &= ~PAS_OUT;
		//			goto CallDWP;
		//		}

		/*
		 * Create the animation bitmap.
		 */
		pMenuState->cxAni = rcWindow.right - rcWindow.left;
		pMenuState->cyAni = rcWindow.bottom - rcWindow.top;

		//if (TestALPHA(MENUFADE))
		//{
		//	if ((hdcAni = CreateFade(pwnd, NULL, CMS_MENUFADE,
		//					FADE_SHOW | FADE_MENU)) == NULL)
		//	{
		//		goto NoAnimation;
		//	}
		//}
		//else
		{

			//if (!MNCreateAnimationBitmap(pMenuState, pMenuState->cxAni,
			//		pMenuState->cyAni))
			//{
			//	goto NoAnimation;
			//}

			/*
			 * We shouldn't be animating at this time.
			 */
			UserAssert(pMenuState->hdcWndAni == NULL);

			/*
			 * This window must be the active popup
			 */
			//UserAssert(pMenuState->pGlobalPopupMenu->spwndActivePopup == pwnd);

			/*
			 * Initialize animation info
			 */
			pMenuState->hdcWndAni = GetDCEx(pwnd, HRGN_FULL, DCX_WINDOW | DCX_USESTYLE | DCX_INTERSECTRGN);
			pMenuState->iAniDropDir = ppopupmenu->iDropDir;
			pMenuState->ixAni = (pMenuState->iAniDropDir & PAS_HORZ) ? 0 : pMenuState->cxAni;
			pMenuState->iyAni = (pMenuState->iAniDropDir & PAS_VERT) ? 0 : pMenuState->cyAni;
			hdcAni = pMenuState->hdcAni;
		}

		/*
		 * MFWINDOWDC is used by MNEraseBackground to determine where the
		 *  brush org should be set.
		 */
		SetMF(pmenu, MFWINDOWDC);

		::SendMessage(pwnd, WM_PRINT, (WPARAM)hdcAni, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND);

		ClearMF(pmenu, MFWINDOWDC);

		/*
		 *If owner draw, we just passed hdcAni to the client side.
		 *  The app might have deleted it (??); no blue screen seems to
		 *  happen but only painting on that DC will fail from
		 *  now on. I won't waste time handling this unless it turns
		 *  out to be a problem (it's unlikely an app would do so).
		 */
		//UserAssert(GreValidateServerHandle(hdcAni, DC_TYPE));

		/*
		 * While the window is still hidden, load the first fade animation
		 * frame to avoid flicker when the window is actually shown.
		 *
		 * There would still be flicker with slide animations, though. It
		 * could be fixed by using the window region, similar to
		 * AnimateWindow. For now, too many functions would become xxx, so
		 * let's not do it, unless it becomes a big issue.
		 */
		//if (TestFadeFlags(FADE_MENU))
		//{
		//	ShowFade();
		//}
	}
	goto CallDWP;

	case WM_WINDOWPOSCHANGED:
	{
		if (!(((LPWINDOWPOS)lParam)->flags & SWP_SHOWWINDOW))
		{
			goto CallDWP;
		}

		/*
		 * If not animating, nothing else to do here.
		 */
		if (!(ppopupmenu->iDropDir & PAS_OUT))
		{
			goto CallDWP;
		}

		/*
		 * Start the animation cycle now.
		 */
		//if (TestFadeFlags(FADE_MENU))
		//{
		//	StartFade();
		//}
		//else
		//{
		//	pMenuState->dwAniStartTime = GetTickCount();
		//	SetTimer(pwnd, IDSYS_MNANIMATE, 1, NULL);
		//}
		ppopupmenu->iDropDir &= ~PAS_OUT;
	}
	goto CallDWP;

	// 	case WM_NCPAINT:
	//
	// 		if (ppopupmenu->iDropDir & PAS_OUT)
	// 		{
	//
	// 			/*
	// 			 * When animating, validate itself to ensure no further drawing
	// 			 * that is not related to the animation.
	// 			 */
	// 			ValidateRect(pwnd, NULL);
	// 		}
	// 		else
	// 		{
	//
	// 			/*
	// 			 * If we have scroll bars, draw them
	// 			 */
	// 			if (pmenu->dwArrowsOn != MSA_OFF)
	// 			{
	//
	// 				HDC hdc = GetDCEx(pwnd, (HRGN)wParam,
	// 						DCX_USESTYLE | DCX_WINDOW | DCX_INTERSECTRGN | /*DCX_NODELETERGN | */DCX_LOCKWINDOWUPDATE);
	// 				//MNDrawFullNC(pwnd, hdc, ppopupmenu);
	// 				ReleaseDC(pwnd, hdc);
	// 			}
	// 			else
	// 			{
	// 				goto CallDWP;
	// 			}
	// 		}
	// 		break;

	case WM_PRINTCLIENT:
		ThreadLock(pmenu, &tlpmenu);
		xxxMenuDraw((HDC)wParam, pmenu);
		ThreadUnlock(&tlpmenu);
		break;

	case WM_FINALDESTROY:
		/*
		 * If we're animating, we must haved been killed in a rude way....
		 */
		UserAssert((pMenuState == NULL) || (pMenuState->hdcWndAni == NULL));

		/*
		 * If this is a drag and drop menu, then call RevokeDragDrop.
		 */
		if ((pMenuState != NULL) && pMenuState->fDragAndDrop)
		{
			//if (!SUCCEEDED(xxxClientRevokeDragDrop(pwnd)))
			//{
			//	//RIPMSG1(RIP_ERROR, "xxxMenuWindowProc: xxxClientRevokeRegisterDragDrop failed:%#p", pwnd);
			//}
		}

		xxxMNDestroyHandler(ppopupmenu);
		return 0;


	case WM_PAINT:
		ThreadLock(pmenu, &tlpmenu);
		BeginPaint(pwnd, &ps);
		xxxMenuDraw(ps.hdc, pmenu);
		EndPaint(pwnd, &ps);
		ThreadUnlock(&tlpmenu);
		break;

	case WM_CHAR:
	case WM_SYSCHAR:
		xxxMNChar(ppopupmenu, pMenuState, (UINT)wParam);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		xxxMNKeyDown(ppopupmenu, pMenuState, (UINT)wParam);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case IDSYS_MNSHOW:
			/*
			 * Open the window and kill the show timer.
			 *
			 * Cancel any toggle state we might have.  We don't
			 * want to dismiss this on button up if shown from
			 * button down.
			 */
			ppopupmenu->fToggle = FALSE;
			xxxMNOpenHierarchy(ppopupmenu, pMenuState);
			break;

		case IDSYS_MNHIDE:
			ppopupmenu->fToggle = FALSE;
			xxxMNCloseHierarchy(ppopupmenu, pMenuState);
			break;

		case IDSYS_MNUP:
		case IDSYS_MNDOWN:
			if (pMenuState->fButtonDown)
			{
				xxxMNDoScroll(ppopupmenu, (UINT)wParam, FALSE);
			}
			else
			{
				::KillTimer(pwnd, (UINT)wParam);
			}
			break;

		//case IDSYS_MNANIMATE:
		//	if (pMenuState->hdcWndAni != NULL)
		//	{
		//		//MNAnimate(pMenuState, TRUE);
		//	}
		//	else
		//	{
		//		/*
		//		 * This timer shouldn't be set. Left over in msg queue?
		//		 */
		//		UserAssert(pMenuState->hdcWndAni != NULL);
		//	}
		//	break;

		case IDSYS_MNAUTODISMISS:
			/*
			 * This is a one shot timer, so kill it.
			 * Dismiss the popup if the flag hasn't been reset.
			 */
			::KillTimer(pwnd, IDSYS_MNAUTODISMISS);
			if (pMenuState->fAboutToAutoDismiss)
			{
				goto EndMenu;
			}
		}
		break;

	/*
	 * Menu messages.
	 */
	case MN_SETHMENU:

		/*
		 * wParam - new hmenu to associate with this menu window
		 * Don't let them set the spmenu to NULL of we have to deal with
		 *  that all over. Use RIP_ERROR for a while to make sure this is OK
		 */
		//if (wParam != 0)
		//{
		//	if ((wParam = (WPARAM)ValidateHmenu((HMENU)wParam)) == 0)
		//	{
		//		break;
		//	}
		//	LockPopupMenu(ppopupmenu, &(ppopupmenu->spmenu), (PMENU)wParam);
		//}
		//else
		//{
		//	RIPMSG1(RIP_ERROR, "xxxMenuWindowProc: MN_SETHMENU ignoring NULL wParam. pwnd:%p", pwnd);
		//}
		break;

	//case MN_GETHMENU:

	//	/*
	//	 * returns the hmenu associated with this menu window
	//	 */
	//	return (LRESULT)PtoH(pmenu);

	case MN_SIZEWINDOW:
	{

		/*
		 * Computes the size of the menu associated with this window and resizes
		 * it if needed.  Size is returned x in loword, y in highword.  wParam
		 * is 0 to just return new size.  wParam is non zero if we should also resize
		 * window.
		 * When called by xxxMNUpdateShownMenu, we might need to redraw the
		 *  frame (i.e, the scrollbars). So we check for MNSW_DRAWFRAME in wParam.
		 *  If some app is sending this message and that bit is set, then we'll
		 *  do some extra work, but I think everything should be cool.
		 */
		int         cx, cy;
		HMONITOR    hMonitor;

		/*
		 * Call menucomputeHelper directly since this is the entry point for
		 * non toplevel menu bars.
		 */
		if (pmenu == NULL)
		{
			break;
		}

		ThreadLockAlways(pmenu, &tlpmenu);
		ThreadLock(ppopupmenu->spwndNotify, &tlpwndNotify);
		UserAssert(pmenu->spwndNotify == ppopupmenu->spwndNotify);

		// 计算菜单项及菜单的尺寸，并保存到菜单项及菜单属性结构体中
		xxxMNCompute(pmenu, ppopupmenu->spwndNotify, 0, 0, 0, 0);
		ThreadUnlock(&tlpwndNotify);
		ThreadUnlock(&tlpmenu);

		hMonitor = MonitorFromWindow(pwnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);

		cx = pmenu->cxMenu;
		cy = MNCheckScroll(pmenu, hMonitor);

		// 调整菜单窗口的尺寸
		if (wParam != 0)
		{
			LONG    lPos;
			int     x, y;
			DWORD   dwFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

			/*
			 * Need to redraw the frame?
			 */
			if (wParam & MNSW_DRAWFRAME)
			{
				dwFlags |= SWP_DRAWFRAME;
			}

			/*
			 * If the window is visible, it's being resized while
			 *  shown. So make sure that it still fits on the screen
			 *  (i.e, move it to the best pos).
			 */
			if (::IsWindowVisible(pwnd))
			{
				lPos = FindBestPos(
						rcWindow.left,
						rcWindow.top,
						cx,
						cy,
						NULL,
						0,
						ppopupmenu,
						hMonitor);

				x = GET_X_LPARAM(lPos);
				y = GET_Y_LPARAM(lPos);
			}
			else
			{
				dwFlags |= SWP_NOMOVE;
				x = 0;
				y = 0;
			}

			::SetWindowPos(
				pwnd,
				HWND_TOP,
				x,
				y,
				cx,
				cy,
				dwFlags);

		}

		return MAKELONG(cx, cy);
	}

	case MN_OPENHIERARCHY:
	{
		HWND pwndT;
		/*
		 * Opens one level of the hierarchy at the selected item, if
		 * present. Return 0 if error, else hwnd of opened hierarchy.
		 */
		pwndT = xxxMNOpenHierarchy(ppopupmenu, pMenuState);
		return (LRESULT)pwndT;
	}

	case MN_CLOSEHIERARCHY:
		xxxMNCloseHierarchy(ppopupmenu, pMenuState);
		break;

	case MN_SELECTITEM:
		/*
		 * wParam - the item to select. Must be a valid
		 * Returns the item flags of the wParam (0 if failure)
		 */
		if ((wParam >= pmenu->cItems) && (wParam < MFMWFP_MINVALID))
		{
			//UserAssertMsg1(FALSE, "Bad wParam %x for MN_SELECTITEM", wParam);
			break;
		}

		pItem = xxxMNSelectItem(ppopupmenu, pMenuState, (UINT)wParam);
		if (pItem != NULL)
		{
			return ((LONG)(DWORD)(WORD)(pItem->fState |
						((pItem->spSubMenu != NULL) ? MF_POPUP : 0)));
		}

		break;

	case MN_SELECTFIRSTVALIDITEM:
	{
		UINT item;

		item = MNFindNextValidItem(pmenu, -1, 1, TRUE);
		::SendMessage(pwnd, MN_SELECTITEM, item, 0L);
		return (LONG)item;
	}

	case MN_CANCELMENUS:

		/*
		 * Cancels all menus, unselects everything, destroys windows, and cleans
		 * everything up for this hierarchy.  wParam is the command to send and
		 * lParam says if it is valid or not.
		 */
		xxxMNCancel(pMenuState, (UINT)wParam, (BOOL)LOWORD(lParam), 0);
		break;

	case MN_FINDMENUWINDOWFROMPOINT:
		/*
		 * lParam is point to search for from this hierarchy down.
		 * returns MFMWFP_* value or a pwnd.
		 */
		lRet = xxxMNFindWindowFromPoint(ppopupmenu, (PUINT)wParam, MAKEPOINTS(lParam));

		/*
		 * Convert return value to a handle.
		 */
		if (IsMFMWFPWindow(lRet))
		{
			return (LRESULT)(HWND)lRet;
		}
		else
		{
			return lRet;
		}


	case MN_SHOWPOPUPWINDOW:
		/*
		 * Forces the dropped down popup to be visible and if modeless, also active.
		 */
		//PlayEventSound(USER_SOUND_MENUPOPUP);
		::ShowWindow(pwnd, (pMenuState->fModelessMenu ? SW_SHOW : SW_SHOWNOACTIVATE));
		break;

	//case MN_ACTIVATEPOPUP:
	//	/*
	//	 * Activates a popup. This messages is posted in response to WM_ACTIVATEAPP
	//	 *  or WM_ACTIVATE
	//	 */
	//	UserAssert(pMenuState->fModelessMenu);
	//	xxxActivateThisWindow(pwnd, 0, 0);
	//	break;

	case MN_ENDMENU:
		/*
		 * End the menu. This message is posted to avoid ending the menu
		 *  at randmom moments. By posting the message, the request is
		 *  queued after any pending/current processing.
		 */
EndMenu:
		xxxEndMenuLoop(pMenuState, pMenuState->pGlobalPopupMenu);
		if (pMenuState->fModelessMenu)
		{
			UserAssert(!pMenuState->fInCallHandleMenuMessages);
			xxxMNEndMenuState(TRUE);
		}
		return 0;

	case MN_DODRAGDROP:
		/*
		 * Let the app know that the user is dragging.
		 */
		if (pMenuState->fDragging
			&& (ppopupmenu->spwndNotify != NULL)
			&& IsMFMWFPWindow(pMenuState->uButtonDownHitArea))
		{
			/*
			 * Get the pmenu that contains the item being dragged
			 */
			pmenu = GetPopupMenu((HWND)pMenuState->uButtonDownHitArea)->spmenu;
			/*
			 * If this is a modal menu, release the capture lock so
			 *  DoDragDrop (if called) can get it.
			 */
			if (!pMenuState->fModelessMenu)
			{
				//UserAssert(PtiCurrent()->pq->QF_flags & QF_CAPTURELOCKED);
				//PtiCurrent()->pq->QF_flags &= ~QF_CAPTURELOCKED;
			}

			LockMenuState(pMenuState);
			ThreadLockAlways(ppopupmenu->spwndNotify, &tlpwndNotify);

			/*
			 * Give them a chance to call DoDragDrop
			 */
			pMenuState->fInDoDragDrop = TRUE;
			lRet = ::SendMessage(ppopupmenu->spwndNotify, WM_MENUDRAG,
					pMenuState->uButtonDownIndex, (LPARAM)pmenu);
			pMenuState->fInDoDragDrop = FALSE;

			if (lRet == MND_ENDMENU)
			{
				/*
				 * Go away.
				 */
				ThreadUnlock(&tlpwndNotify);
				if (!xxxUnlockMenuState(pMenuState))
				{
					goto EndMenu;
				}
				else
				{
					return 0;
				}
				break;
			}
			else
			{
				/*
				 * If the user starts dragging, we always
				 *  ignore the following button up
				 */
				pMenuState->fIgnoreButtonUp = TRUE;
			}

			/*
			 * Check the button state since we might have not seen the button up
			 * If so, this will cancel the dragging state
			 */
			MNCheckButtonDownState(pMenuState);

			/*
			 * If this is a modal menu, make sure we recover capture
			 */
			if (!pMenuState->fModelessMenu)
			{
				xxxMNSetCapture(ppopupmenu);
			}

			ThreadUnlock(&tlpwndNotify);
			xxxUnlockMenuState(pMenuState);
		}
		return 0;

	case MN_BUTTONDOWN:

		/*
		 * wParam is position (index) of item the button was clicked on.
		 * Must be a valid
		 */
		if ((wParam >= pmenu->cItems) && (wParam < MFMWFP_MINVALID))
		{
			//UserAssertMsg1(FALSE, "Bad wParam %x for MN_BUTTONDOWN", wParam);
			break;
		}
		xxxMNButtonDown(ppopupmenu, pMenuState, (UINT)wParam, TRUE);
		break;

	case MN_MOUSEMOVE:

		/*
		 * lParam is mouse move coordinate wrt screen.
		 */
		xxxMNMouseMove(ppopupmenu, pMenuState, MAKEPOINTS(lParam));
		break;

	case MN_BUTTONUP:

		/*
		 * wParam is position (index) of item the button was up clicked on.
		 */
		if ((wParam >= pmenu->cItems) && (wParam < MFMWFP_MINVALID))
		{
			//UserAssertMsg1(FALSE, "Bad wParam %x for MN_BUTTONUP", wParam);
			break;
		}
		xxxMNButtonUp(ppopupmenu, pMenuState, (UINT)wParam, lParam);
		break;

	case MN_SETTIMERTOOPENHIERARCHY:

		/*
		 * Given the current selection, set a timer to show this hierarchy if
		 * valid else return 0.
		 */
		return (LONG)(WORD)MNSetTimerToOpenHierarchy(ppopupmenu);

	case MN_DBLCLK:
		//
		// User double-clicked on item.  wParamLo is the item.
		//
		xxxMNDoubleClick(pMenuState, ppopupmenu, (int)wParam);
		break;

	case WM_MOUSELEAVE:
		UserAssert(pMenuState->fModelessMenu);
		/*
		 * If we're in DoDragDrop loop, we don't track the mouse
		 *  when it goes off the menu window
		 */
		pMenuState->fMouseOffMenu = !pMenuState->fInDoDragDrop;
		ppopupmenu->fTrackMouseEvent = FALSE;
		/*
		 * See if we need to set the timer to autodismiss
		 */
		MNSetTimerToAutoDismiss(pMenuState, pwnd);
		/*
		 * If we left the active popup, remove the selection
		 */
		if (ppopupmenu->spwndPopupMenu == pMenuState->pGlobalPopupMenu->spwndActivePopup)
		{
			xxxMNSelectItem(ppopupmenu, pMenuState, MFMWFP_NOITEM);
		}
		break;

	case WM_ACTIVATEAPP:
		if (pMenuState->fModelessMenu
			&& (pwnd == pMenuState->pGlobalPopupMenu->spwndActivePopup))
		{
			/*
			 * If the application is getting activated,  we post a message
			 *  to let the dust settle and then re-activate spwndPopupActive
			 */
			if (wParam)
			{
				PostMessage(pwnd, MN_ACTIVATEPOPUP, 0, 0);
				/*
				 * If we're not in the foregruond queue, we want to keep
				 *  the frame off.
				 * This flag will also tell us that if we lose activation
				 *  while coming to the foregrund (later), we don't want
				 *  to dismiss the menu.
				 */
				//pMenuState->fActiveNoForeground = (gpqForeground != PtiCurrent()->pq);
			}

			/*
			 * Make the notification window frame show that we're active/inactive.
			 * If the application is inactive but the user moves the mouse
			 *  over the menu, then we can get this message when the first
			 *  window in the app gets activated (i.e., the move causes a popup to
			 *  be closed/opened). So turn on the frame only if we're in
			 *  the foreground.
			 */
			//if (ppopupmenu->spwndNotify != NULL)
			//{
			//	ThreadLockAlways(ppopupmenu->spwndNotify, &tlpwndNotify);
			//	xxxDWP_DoNCActivate(ppopupmenu->spwndNotify,
			//		((wParam && !pMenuState->fActiveNoForeground) ? NCA_ACTIVE : NCA_FORCEFRAMEOFF),
			//		HRGN_FULL);
			//	ThreadUnlock(&tlpwndNotify);
			//}
		}
		break;

	case WM_ACTIVATE:
		//if (pMenuState->fModelessMenu)
		//{
		//	/*
		//	 * If activation is NOT going to a menu window or
		//	 *  it's going to a recursed menu, bail
		//	 */
		//	if ((LOWORD(wParam) == WA_INACTIVE)
		//		&& !pMenuState->fInCallHandleMenuMessages
		//		&& !pMenuState->pGlobalPopupMenu->fInCancel)
		//	{

		//		lParam = (LPARAM)(HWND)lParam;
		//		if ((lParam != 0)
		//			&& (/*(GETFNID((HWND)lParam) != FNID_MENU)
		//				|| */IsRecursedMenuState(pMenuState, ((PMENUWND)lParam)->ppopupmenu)))
		//		{
		//			/*
		//			 * If we're just coming to the foreground, then
		//			 *  activate the popup later and stay up.
		//			 */
		//			if (pMenuState->fActiveNoForeground
		//				/*&& (gpqForeground == PtiCurrent()->pq)*/)
		//			{

		//				pMenuState->fActiveNoForeground = FALSE;
		//				PostMessage(pwnd, MN_ACTIVATEPOPUP, 0, 0);
		//			}
		//			else
		//			{
		//				/*
		//				 * Since the menu window is active, ending the menu
		//				 *  now would set a new active window, messing the
		//				 *  current activation that sent us this message.
		//				 *  so end the menu later.
		//				 */
		//				PostMessage(pwnd, MN_ENDMENU, 0, 0);
		//				break;
		//			}
		//		}
		//	}
		//	goto CallDWP;
		//} /* if (pMenuState->fModelessMenu) */

		/*
		 * We must make sure that the menu window does not get activated.
		 * Powerpoint 2.00e activates it deliberately and this causes problems.
		 * We try to activate the previously active window in such a case.
		 * Fix for Bug #13961 --SANKAR-- 09/26/91--
		 */
		/*
		 * In Win32, wParam has other information in the hi 16bits, so to
		 * prevent infinite recursion, we need to mask off those bits
		 * Fix for NT bug #13086 -- 23-Jun-1992 JonPa
		 *
		 */

		if (LOWORD(wParam))
		{
			//TL tlpwnd;
			/*
			 * This is a super bogus hack. Let's start failing this for 5.0 apps.
			 */
			//if (Is500Compat(PtiCurrent()->dwExpWinVer))
			//{
			//	RIPMSG1(RIP_ERROR, "xxxMenuWindowProc: Menu window activated:%#p", pwnd);
			//	_PostMessage(pwnd, MN_ENDMENU, 0, 0);
			//	break;
			//}

#if 0
			/*
			 * Activate the previously active wnd
			 */
			xxxActivateWindow(pwnd, AW_SKIP2);
#else
			/*
			 * Try the previously active window.
			 */
			//if ((gpqForegroundPrev != NULL) &&
			//	!FBadWindow(gpqForegroundPrev->spwndActivePrev) &&
			//	!ISAMENU(gpqForegroundPrev->spwndActivePrev))
			//{
			//	pwnd = gpqForegroundPrev->spwndActivePrev;
			//}
			//else
			//{

			//	/*
			//	 * Find a new active window from the top-level window list.
			//	 * Bug 78131: Make sure we don't loop for ever. This is a pretty
			//	 *  unusual scenario (in addtion, normally we should not hit this code path)
			//	 *  So let's use a counter to rule out the possibility that another
			//	 *  weird window configuration is going to make us loop for ever
			//	 */
			//	HWND pwndMenu = pwnd;
			//	UINT uCounter = 0;
			//	do
			//	{
			//		pwnd = NextTopWindow(PtiCurrent(), pwnd, NULL, 0);
			//		if (pwnd && !FBadWindow(pwnd->spwndLastActive) &&
			//			!ISAMENU(pwnd->spwndLastActive))
			//		{
			//			pwnd = pwnd->spwndLastActive;
			//			uCounter = 0;
			//			break;
			//		}
			//	}
			//	while ((pwnd != NULL) && (uCounter++ < 255));
			//	/*
			//	 * If we couldn't find a window, just bail.
			//	 */
			//	if (uCounter != 0)
			//	{
			//		RIPMSG0(RIP_ERROR, "xxxMenuWindowProc: couldn't fix active window");
			//		_PostMessage(pwndMenu, MN_ENDMENU, 0, 0);
			//		break;
			//	}
			//}

			//if (pwnd != NULL)
			//{
			//	PTHREADINFO pti = PtiCurrent();
			//	ThreadLockAlwaysWithPti(pti, pwnd, &tlpwnd);

			//	/*
			//	 * If GETPTI(pwnd) isn't pqCurrent this is a AW_SKIP* activation
			//	 * we'll want to a do a xxxSetForegroundWindow().
			//	 */
			//	if (GETPTI(pwnd)->pq != pti->pq)
			//	{

			//		/*
			//		 * Only allow this if we're on the current foreground queue.
			//		 */
			//		if (gpqForeground == pti->pq)
			//		{
			//			xxxSetForegroundWindow(pwnd, FALSE);
			//		}
			//	}
			//	else
			//	{
			//		xxxActivateThisWindow(pwnd, 0, ATW_SETFOCUS);
			//	}

			//	ThreadUnlock(&tlpwnd);
			//}
#endif
		}
		break;

	case WM_SIZE:
	case WM_MOVE:
		/*
		 * When a popup has been sized/moved, we need to make
		 *  sure any dropped hierarchy is moved accordingly.
		 */
		if (ppopupmenu->spwndNextPopup != NULL)
		{
			pItem = MNGetpItem(ppopupmenu, ppopupmenu->posDropped);
			if (pItem != NULL)
			{
				int      x, y;
				HMONITOR pMonitorDummy;

				/*
				 * If the dropped hierarchy needs to be recomputed, do it
				 */
#define pmenuNext (GetPopupMenu(ppopupmenu->spwndNextPopup)->spmenu)
				if (pmenuNext->cxMenu == 0)
				{
					::SendMessage(ppopupmenu->spwndNextPopup, MN_SIZEWINDOW, MNSW_RETURNSIZE, 0L);
				}

				/*
				 * Find out the new position
				 */
				xxxMNPositionHierarchy(ppopupmenu, pItem,
					pmenuNext->cxMenu + (2 * SYSMET(CXFIXEDFRAME)),
					pmenuNext->cyMenu + (2 * SYSMET(CXFIXEDFRAME)),
					&x, &y, &pMonitorDummy);

				/*
				 * Move it
				 */
				ThreadLockAlways(ppopupmenu->spwndNextPopup, &tlpwndNotify);
				::SetWindowPos(ppopupmenu->spwndNextPopup, NULL,
					x, y, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);
				ThreadUnlock(&tlpwndNotify);
#undef pmenuNext
			} /* if (pItem != NULL) */
		} /* if (ppopupmenu->spwndNextPopup != NULL) */
		break;

	case WM_NCHITTEST:
		/*
		 * Since modeless menus don't capture the mouse, we
		 *  process this message to make sure that we always receive
		 *  a mouse move when the mouse in our window.
		 * This also causes us to receive the WM_MOUSELEAVE only when
		 *  the mouse leaves the window and not just the  client area.
		 */
		if (pMenuState->fModelessMenu)
		{
			ptOrg.x = GET_X_LPARAM(lParam);
			ptOrg.y = GET_Y_LPARAM(lParam);
			if (PtInRect(&rcWindow, ptOrg))
			{
				return HTCLIENT;
			}
			else
			{
				return HTNOWHERE;
			}
		}
		else
		{
			goto CallDWP;
		}


	default:
CallDWP:
		return DefWindowProc(pwnd, message, wParam, lParam);
	}

	return 0;
}


//////////////////////////////////////////////////////////

/***************************************************************************\
* MNLookUpItem
*
* Return a pointer to the menu item specified by wCmd and wFlags
*
* History:
*   10-11-90 JimA       Translated from ASM
*   01-07-93 FritzS     Ported from Chicago
\***************************************************************************/

PITEM IUI::MNLookUpItem(
	PMENU pMenu,
	UINT wCmd,
	BOOL fByPosition,
	PMENU *ppMenuItemIsOn)
{
	PITEM pItem;
	PITEM pItemRet = NULL;
	PITEM  pItemMaybe = NULL;
	PMENU   pMenuMaybe = NULL;
	int i;

	if (ppMenuItemIsOn != NULL)
	{
		*ppMenuItemIsOn = NULL;
	}

	if (pMenu == NULL || !pMenu->cItems || wCmd == MFMWFP_NOITEM)
	{
		//      RIPERR0(ERROR_INVALID_PARAMETER, RIP_WARNING, "MNLookUpItem: invalid item");
		return NULL;
	}

	/*
	 * dwFlags determines how we do the search
	 */
	if (fByPosition)
	{
		if (wCmd < (UINT)pMenu->cItems)
		{
			pItemRet = pMenu->rgItems + wCmd;//&((PITEM)REBASEALWAYS(pMenu, rgItems))[wCmd];
			if (ppMenuItemIsOn != NULL)
			{
				*ppMenuItemIsOn = pMenu;
			}
			return (pItemRet);
		}
		else
		{
			return NULL;
		}
	}
	/*
	 * Walk down the menu and try to find an item with an ID of wCmd.
	 * The search procedes from the end of the menu (as was done in
	 * assembler).
	 */

	/* this is the Chicago code, which walks from the front of the menu -- Fritz */


	//        for (pItem = &pMenu->rgItems[i - 1]; pItemRet == NULL && i--; --pItem) {
	for (i = 0, pItem = (PITEM)REBASEALWAYS(pMenu, rgItems); i < (int)pMenu->cItems;
		i++, pItem++)
	{

		/*
		 * If the item is a popup, recurse down the tree
		 */
		if (pItem->spSubMenu != NULL)
		{
			//
			// COMPAT:
			// Allow apps to pass in menu handle as ID in menu APIs.  We
			// remember that this popup had a menu handle with the same ID
			// value.  This is a 2nd choice though.  We still want to see
			// if there's some actual command that has this ID value first.
			//
			if (pItem->wID == wCmd)
			{
				pMenuMaybe = pMenu;
				pItemMaybe = pItem;
			}

			pItemRet = MNLookUpItem((PMENU)REBASEPTR(pMenu, pItem->spSubMenu),
					wCmd, FALSE, ppMenuItemIsOn);
			if (pItemRet != NULL)
			{
				return pItemRet;
			}
		}
		else if (pItem->wID == wCmd)
		{

			/*
			 * Found the item, now save things for later
			 */
			if (ppMenuItemIsOn != NULL)
			{
				*ppMenuItemIsOn = pMenu;
			}
			return pItem;
		}
	}

	if (pMenuMaybe)
	{
		// no non popup menu match found -- use the 2nd choice popup menu
		// match
		if (ppMenuItemIsOn != NULL)
		{
			*ppMenuItemIsOn = pMenuMaybe;
		}
		return (pItemMaybe);
	}

	return (NULL);
}

BOOL IsNamedWindow(HWND hwnd, LPCTSTR pszClass)
{
	TCHAR szClass[32];

	GetClassName(hwnd, szClass, ARRAYSIZE(szClass));
	return lstrcmp(szClass, pszClass) == 0;
}
const TCHAR c_szExploreClass[] = TEXT("ExploreWClass");
BOOL IsExplorerWindow(HWND hwnd)
{
	return IsNamedWindow(hwnd, c_szExploreClass);
}

BOOL IUI::IsTrayWindow(HWND hwnd)
{
	return IsNamedWindow(hwnd, TEXT(WNDCLASS_TRAYNOTIFY));
}

void IUI::CopyOffsetRect(LPRECT out, LPRECT in, int x, int y)
{
	*out = *in;
	OffsetRect(out, x, y);
}

DWORD IUI::_GetMenuDefaultItem(PMENU pMenu, BOOL fByPosition, UINT uFlags)
{
	int iItem;
	int cItems;
	PITEM pItem;
	PMENU pSubMenu;

	pItem = (PITEM)REBASEALWAYS(pMenu, rgItems);
	cItems = pMenu->cItems;

	/*
	 * Walk the list of items sequentially until we find one that has
	 * MFS_DEFAULT set.
	 */
	for (iItem = 0; iItem < cItems; iItem++, pItem++)
	{
		if (TestMFS(pItem, MFS_DEFAULT))
		{
			if ((uFlags & GMDI_USEDISABLED) || !TestMFS(pItem, MFS_GRAYED))
			{
				if ((uFlags & GMDI_GOINTOPOPUPS) && (pItem->spSubMenu != NULL))
				{
					DWORD id;

					/*
					 * Is there a valid submenu default?  If not, we'll use
					 * this one.
					 */
					pSubMenu = (PMENU)REBASEPTR(pMenu, pItem->spSubMenu);
					id = _GetMenuDefaultItem(pSubMenu, fByPosition, uFlags);
					if (id != MFMWFP_NOITEM)
					{
						return (id);
					}
				}

				break;
			}
		}
	}

	if (iItem < cItems)
	{
		return (fByPosition ? iItem : pItem->wID);
	}
	else
	{
		return (MFMWFP_NOITEM);
	}
}

/************************************************************************\
* CopyInflateRect (API)
*
* This function copies the rect from prcSrc to prcDst, and inflates it.
*
* History:
* 12-16-93  FritzS
\************************************************************************/

BOOL IUI::CopyInflateRect(
	LPRECT prcDst,
	CONST RECT *prcSrc,
	int cx, int cy)
{
	prcDst->left   = prcSrc->left   - cx;
	prcDst->right  = prcSrc->right  + cx;
	prcDst->top    = prcSrc->top    - cy;
	prcDst->bottom = prcSrc->bottom + cy;
	return TRUE;
}

/***************************************************************************\
* xxxSendMinRectMessages
*
* History:
\***************************************************************************/

BOOL IUI::xxxSendMinRectMessages(HWND pwnd, RECT *lpRect)
{
	BOOL fRet = FALSE;
	HWND hwnd = pwnd;
	//PTHREADINFO pti = PtiCurrent();
	//PDESKTOPINFO pdeskinfo;
	//DWORD nPwndShellHook;
	//HWND pwndShellHook;

	//if (IsHooked(pti, WHF_SHELL))
	//{
	//	xxxCallHook(HSHELL_GETMINRECT, (WPARAM)hwnd,
	//		(LPARAM)lpRect, WH_SHELL);
	//	fRet = TRUE;
	//}

	//pdeskinfo = GETDESKINFO(pti);
	//if (pdeskinfo->pvwplShellHook == NULL)
	//{
	//	return fRet;
	//}

	//nPwndShellHook = 0;
	//pwndShellHook = NULL;
	//while (pwndShellHook = VWPLNext(pdeskinfo->pvwplShellHook, pwndShellHook, &nPwndShellHook))
	//{
	//	TL tlpwnd;
	//	ULONG_PTR dwRes;

	//	ThreadLock(pwndShellHook, &tlpwnd);
	//	if (xxxSendMessageTimeout(pwndShellHook, WM_KLUDGEMINRECT, (WPARAM)(hwnd), (LPARAM)lpRect,
	//			SMTO_NORMAL, 100, &dwRes))
	//	{
	//		fRet = TRUE;
	//	}

	//	/*
	//	 * pdeskinfo->pvwplShellHook may have been realloced to a different
	//	 * location and size during the WM_KLUDGEMINRECT callback.
	//	 */
	//	ThreadUnlock(&tlpwnd);
	//}
	//return fRet;

	return TRUE;
}

/**************************************************************************\
* xxxClientRegisterDragDrop
*
* 10/28/96 GerardoB     Created
\**************************************************************************/
NTSTATUS IUI::xxxClientRegisterDragDrop(HWND hwnd)
{
	return 0;//xxxUserModeCallback(FI_CLIENTREGISTERDRAGDROP, &hwnd, sizeof(&hwnd), NULL, 0);
}

void IUI::xxxSendMenuSelect(
	HWND pwndNotify,
	HWND pwndMenu,
	PMENU pMenu,
	int idx)
{
	UINT cmd;       // Menu ID if applicable.
	UINT flags;     // MF_ values if any
	MSG msg;
	PMENUSTATE pMenuState;

	CheckLock(pwndNotify);
	CheckLock(pwndMenu);


	/*
	 * We must be hacking or passing valid things.
	 */
	UserAssert((pMenu != SMS_NOMENU) || (idx == MFMWFP_NOITEM));


	if ((idx >= 0) && (pMenu->cItems > (UINT)idx))
	{
		PITEM pItem = &(pMenu->rgItems[idx]);

		flags   = (pItem->fType  & MFT_OLDAPI_MASK) |
			(pItem->fState & MFS_OLDAPI_MASK);

		if (pItem->spSubMenu != NULL)
		{
			flags |= MF_POPUP;
		}

		flags &= (~(MF_SYSMENU | MF_MOUSESELECT));

		/*
		 * WARNING!
		 * Under Windows the menu handle was always returned but additionally
		 * if the menu was a pop-up the pop-up menu handle was returned
		 * instead of the ID.  In NT we don't have enough space for 2 handles
		 * and flags so if it is a pop-up we return the pop-up index
		 * and the main Menu handle.
		 */

		if (flags & MF_POPUP)
		{
			cmd = idx;    // index of popup-menu
		}
		else
		{
			cmd = pItem->wID;
		}

		pMenuState = GetpMenuState(NULL);
		if (pMenuState != NULL)
		{
			if (pMenuState->mnFocus == MOUSEHOLD)
			{
				flags |= MF_MOUSESELECT;
			}

			if (pMenuState->fIsSysMenu)
			{
				flags |= MF_SYSMENU;
			}

		}
	}
	else
	{
		/*
		 * idx assumed to be MFMWFP_NOITEM
		 */
		if (pMenu == SMS_NOMENU)
		{

			/*
			 * Hack so we can send MenuSelect messages with MFMWFP_MAINMENU
			 * (loword(lparam)=-1) when the menu pops back up for the CBT people.
			 */
			flags = MF_MAINMENU;
		}
		else
		{
			flags = 0;
		}

		cmd = 0;    // so MAKELONG(cmd, flags) == MFMWFP_MAINMENU
		pMenu = 0;
		idx = -1;   // so that idx+1 == 0, meaning nothing for zzzWindowEvent()
	}

	/*
	 * Call msgfilter so help libraries can hook WM_MENUSELECT messages.
	 */
	msg.hwnd = pwndNotify;
	msg.message = WM_MENUSELECT;
	msg.wParam = (DWORD)MAKELONG(cmd, flags);
	msg.lParam = (LPARAM)pMenu;
	if (!CallMsgFilter((LPMSG)&msg, MSGF_MENU))
	{
		::SendMessage(pwndNotify, WM_MENUSELECT, msg.wParam, msg.lParam);
	}

	//if (FWINABLE() && pwndMenu)
	//{
	//	xxxWindowEvent(EVENT_OBJECT_FOCUS, pwndMenu,
	//		((pwndMenu != pwndNotify) ? OBJID_CLIENT : ((flags & MF_SYSMENU) ? OBJID_SYSMENU : OBJID_MENU)),
	//		idx + 1, 0);
	//}
}

/**************************************************************************\
* xxxMNUpdateDraggingInfo
*
* 10/28/96 GerardoB     Created
\**************************************************************************/
void IUI::xxxMNUpdateDraggingInfo(PMENUSTATE pMenuState, ULONG_PTR uHitArea, UINT uIndex)
{
	BOOL fCross;
	int y, iIndexDelta;
	PITEM pItem;
	PPOPUPMENU ppopup;
	//TL tlLastHitArea;
	ULONG_PTR uLastHitArea;
	UINT uLastIndex, uLastFlags = 0;

	/*
	 * Remember current dragging area so we can detected when
	 *  crossing item/gap boundries.
	 */
	UserAssert((pMenuState->uDraggingHitArea == 0) || IsMFMWFPWindow(pMenuState->uDraggingHitArea));
	ThreadLock((PWND)pMenuState->uDraggingHitArea, &tlLastHitArea);
	uLastHitArea = pMenuState->uDraggingHitArea;
	uLastIndex = pMenuState->uDraggingIndex;
	//uLastFlags = pMenuState->uDraggingFlags & MNGOF_GAP;

	/*
	 * Store new dragging area.
	 */
	LockMFMWFPWindow(&pMenuState->uDraggingHitArea, uHitArea);
	pMenuState->uDraggingIndex = uIndex;

	/*
	 * If we're not on a popup, done.
	 */
	if (!IsMFMWFPWindow(pMenuState->uDraggingHitArea))
	{
		pMenuState->uDraggingHitArea = MFMWFP_OFFMENU;
		pMenuState->uDraggingIndex = MFMWFP_NOITEM;
		ThreadUnlock(&tlLastHitArea);
		return;
	}

	/*
	 * Get the popup and item we're on
	 */
	ppopup = GetPopupMenu((HWND)pMenuState->uDraggingHitArea);
	pItem = MNGetpItem(ppopup, pMenuState->uDraggingIndex);

	/*
	 * Find out if we're on the gap, that is, the "virtual" space
	 *  between items. Some apps want to distinguish between a drop
	 *  ON the item and a drop BEFORE/AFTER the item; there is no
	 *  actual space between items so we define a virtual gap
	 *
	 */
	pMenuState->uDraggingFlags = 0;
	if (pItem != NULL)
	{
		/*
		 * Map the point to client coordinates and then to "menu"
		 *  coordinates (to take care of scrollable menus)
		 */

		CIUIRect rcClient;
		::GetClientRect((HWND)pMenuState->uDraggingHitArea, rcClient);
		y = pMenuState->ptMouseLast.y;
		y -= rcClient.top;
		y += MNGetToppItem(ppopup->spmenu)->yItem;
#if DBG
		if ((y < (int)pItem->yItem)
			|| (y > (int)(pItem->yItem + pItem->cyItem)))
		{
			RIPMSG4(RIP_ERROR, "xxxMNUpdateDraggingInfo: y Point not in selected item. "
				"pwnd:%#lx ppopup:%#lx Index:%#lx pItem:%#lx",
				pMenuState->uDraggingHitArea, ppopup, pMenuState->uDraggingIndex, pItem);
		}
#endif

		/*
		 * Top/bottom gap check
		 */
		if (y <= (int)(pItem->yItem + SYSMET(CYDRAG)))
		{
			pMenuState->uDraggingFlags = MNGOF_TOPGAP;
		}
		else if (y >= (int)(pItem->yItem + pItem->cyItem - SYSMET(CYDRAG)))
		{
			pMenuState->uDraggingFlags = MNGOF_BOTTOMGAP;
		}
	}

	/*
	 * Have we crossed an item/gap boundary?
	 * We don't cross a boundary when we move from the bottom
	 *  of an item to the top of the next, or, from the top
	 *  of an item to the bottom of the previous.
	 *  (Item N is on top of and previous to item N+1).
	 */
	fCross = (uLastHitArea != pMenuState->uDraggingHitArea);
	if (!fCross)
	{
		iIndexDelta = (int)pMenuState->uDraggingIndex - (int)uLastIndex;
		switch (iIndexDelta)
		{
		case 0:
			/*
			 * We're on the same item.
			 */
			fCross = (uLastFlags != pMenuState->uDraggingFlags);
			break;

		case 1:
			/*
			 * We've moved to the next item
			 */
			fCross = !((pMenuState->uDraggingFlags == MNGOF_TOPGAP)
					&& (uLastFlags == MNGOF_BOTTOMGAP));
			break;

		case -1:
			/*
			 * We've moved to the previous item
			 */
			fCross = !((pMenuState->uDraggingFlags == MNGOF_BOTTOMGAP)
					&& (uLastFlags == MNGOF_TOPGAP));
			break;

		default:
			/*
			 * We've skipped more than one item.
			 */
			fCross = TRUE;
		}
	}

	//if (fCross)
	//{
	//	//pMenuState->uDraggingFlags |= MNGOF_CROSSBOUNDARY;

	//	/*
	//	 * Update the insertion bar state.
	//	 */
	//	xxxMNSetGapState(uLastHitArea, uLastIndex, uLastFlags, FALSE);
	//	xxxMNSetGapState(pMenuState->uDraggingHitArea,
	//		pMenuState->uDraggingIndex,
	//		pMenuState->uDraggingFlags,
	//		TRUE);
	//}

	ThreadUnlock(&tlLastHitArea);
}

/***************************************************************************\
* xxxMenuCompute2
*
* !
*
* History:
\***************************************************************************/

int IUI::xxxMNCompute(
	PMENU pMenu,
	HWND pwndNotify,
	DWORD yMenuTop,
	DWORD xMenuLeft,
	DWORD cxMax,
	LPDWORD lpdwMenuHeight)
{
	DWORD        cMaxHeight;		// 菜单项（不是菜单）最大高度
	DWORD        temp;				// 菜单的宽度（每遍历一个菜单项后，都更新这个值为最宽菜单项）
	POINT        ptMNItemSize;
	BOOL         fMenuBreak;
	LPWSTR       lpsz;
	//PTHREADINFO  ptiCurrent = PtiCurrent();

	CheckLock(pMenu);
	CheckLock(pwndNotify);

	/*
	 * Whoever computes the menu, becomes the owner.
	 */
	if (pwndNotify != pMenu->spwndNotify)
	{
		Lock(&pMenu->spwndNotify, pwndNotify);
	}


	DWORD menuHeight = 0;
	if (lpdwMenuHeight != NULL)
	{
		menuHeight = *lpdwMenuHeight;
	}

	/*
	 * Empty menus have a height of zero.
	 */
	int ret = 0;
	if (pMenu->cItems == 0)
	{
		return ret;
	}


	// 由于菜单的宽度以字符串最长的Item为准，所以，要根据字符串的字体，来计算字符串的宽度
	// 进而确认菜单的宽度
	HDC hdc = GetDCEx(NULL, NULL, DCX_WINDOW | DCX_CACHE);
	HFONT hOldFont = (HFONT)SelectObject(hdc, ghMenuFont);

	/*
	 * Try to make a non-multirow menu first.
	 */
	pMenu->fFlags &= (~MFMULTIROW);

	BOOL fPopupMenu = TestMF(pMenu, MFISPOPUP);

	if (fPopupMenu)
	{

		/*
		 * Reset the menu bar height to 0 if this is a popup since we are
		 * being called from menu.c MN_SIZEWINDOW.
		 */
		menuHeight = 0;
	}
	else if (pwndNotify != NULL)
	{
		pMenu->cxMenu = cxMax;
	}

	/*
	 * Initialize the computing variables.
	 */
	DWORD cyItemKeep = 0;
	int cMaxWidth = 0;			// 菜单项的最大宽度
	UINT cItemBegCol = 0;		// 如果是可折行显示的菜单，记录从哪个菜单项开始折行。

	DWORD yPopupTop = yMenuTop;
	DWORD cyItem = 0;			// 菜单项的Y坐标
	DWORD cxItem = xMenuLeft;	// 菜单项的X坐标

	PITEM pCurItem = (PITEM)&pMenu->rgItems[0];
	/*
	 * cxTextAlign is used to align the text in all items; this is useful
	 *  in popup menus that  text only items with bitmap-text items. It's
	 *  set to the max bitmap width plus some spacing.
	 * Do this for new menus wich use string AND bitmaps on the same item
	 */
	BOOL fStringAndBitmapItems = FALSE;
	pMenu->cxTextAlign = 0;

	/*
	 * Process each item in the menu.
	 */
	BOOL fOwnerDrawItems = FALSE;
	for (UINT cItem = 0; cItem < pMenu->cItems; cItem++)
	{

		/*
		 * If it's not a separator, find the dimensions of the object.
		 */
		//if (TestMFT(pCurItem, MFT_SEPARATOR) &&
		//	(!TestMFT(pCurItem, MFT_OWNERDRAW) ||
		//		(LOWORD(ptiCurrent->dwExpWinVer) < VER40)))
		//{
		//	/*
		//	* If version is less than 4.0  don't test the MFT_OWNERDRAW
		//	* flag. Bug 21922; App MaxEda has both separator and Ownerdraw
		//	* flags on. In 3.51 we didn't test the OwnerDraw flag
		//	*/

		//	//
		//	// This is a separator.  It's drawn as wide as the menu area,
		//	// leaving some space above and below.  Since the menu area is
		//	// the max of the items' widths, we set our width to 0 so as not
		//	// to affect the result.
		//	//
		//	pCurItem->cxItem = 0;
		//	pCurItem->cyItem = SYSMET(CYMENUSIZE) / 2;


		//}
		//else
		{
			/*
			 * Are we using NT5 strings and bitmaps?
			 */
			fStringAndBitmapItems |= ((pCurItem->hbmp != NULL) && (pCurItem->lpstr != NULL));

			// 计算这个Item的宽度和高度
			if (xxxMNItemSize(pMenu, pwndNotify, hdc, pCurItem, fPopupMenu, &ptMNItemSize))
			{
				fOwnerDrawItems = TRUE;
			}

			pCurItem->cxItem = ptMNItemSize.x;
			pCurItem->cyItem = ptMNItemSize.y;
			if (!fPopupMenu && ((pCurItem->hbmp == NULL) || (pCurItem->lpstr != NULL)))
			{
				pCurItem->cxItem += gcxMenuFontChar * 2;
			}
		}

		if (menuHeight != 0)
		{
			pCurItem->cyItem = menuHeight;
		}

		/*
		 * If this is the first item, initialize cMaxHeight.
		 */
		if (cItem == 0)
		{
			cMaxHeight = pCurItem->cyItem;
		}

		/*
		 * Is this a Pull-Down menu?
		 */
		if (fPopupMenu)
		{
			// 如果本Item有个break(换行)或者本Item是最后一个Item。
			if ((fMenuBreak = TestMFT(pCurItem, MFT_BREAK)) ||
				pMenu->cItems == cItem + (UINT)1)
			{

				/*
				 * Keep cMaxWidth around if this is not the last item.
				 */
				temp = cMaxWidth;
				if (pMenu->cItems == cItem + (UINT)1)
				{
					if ((int)(pCurItem->cxItem) > cMaxWidth)
					{
						temp = pCurItem->cxItem;
					}
				}

				/*
				 * Get new width of string from RecalcTabStrings.
				 */
				temp = 200;//MNRecalcTabStrings(hdc, pMenu, cItemBegCol, (UINT)(cItem + (fMenuBreak ? 0 : 1)), temp, cxItem);

				/*
				 * If this item has a break, account for it.
				 */
				if (fMenuBreak)
				{
					//
					// Add on space for the etch and a border on either side.
					// NOTE:  For old apps that do weird stuff with owner
					// draw, keep 'em happy by adding on the same amount
					// of space we did in 3.1.
					//
					//if (fOwnerDrawItems && !TestWF(pwndNotify, WFWIN40COMPAT))
					//{
					//	cxItem = temp + SYSMET(CXBORDER);
					//}
					//else
					{
						cxItem = temp + 2 * SYSMET(CXEDGE);
					}

					/*
					 * Reset the cMaxWidth to the current item.
					 */
					cMaxWidth = pCurItem->cxItem;

					/*
					 * Start at the top.
					 */
					cyItem = yPopupTop;

					/*
					 * Save the item where this column begins.
					 */
					cItemBegCol = cItem;

					/*
					 * If this item is also the last item, recalc for this
					 * column.
					 */
					if (pMenu->cItems == (UINT)(cItem + 1))
					{
						temp = 200;//MNRecalcTabStrings(hdc, pMenu, cItem, (UINT)(cItem + 1), cMaxWidth, cxItem);
					}
				}

				// 当遍历完所有菜单项后，temp就是最宽的菜单项，用它来作为菜单的最终宽度
				if (pMenu->cItems == cItem + (UINT)1)
				{
					pMenu->cxMenu = temp;
				}
			}

			pCurItem->xItem = cxItem;
			pCurItem->yItem = cyItem;

			cyItem += pCurItem->cyItem;

			if (cyItemKeep < cyItem)
			{
				cyItemKeep = cyItem;
			}

		}
		else
		{

			/*
			 * This a Top Level menu, not a Pull-Down.
			 */

			/*
			 * Adjust right aligned items before testing for multirow
			 */
			if (pCurItem->lpstr != NULL)
			{
				lpsz = TextPointer(pCurItem->lpstr);
				if ((lpsz != NULL) && (*lpsz == CH_HELPPREFIX))
				{
					pCurItem->cxItem -= gcxMenuFontChar;
				}
			}


			/*
			 * If this is a new line or a menu break.
			 */
			if ((TestMFT(pCurItem, MFT_BREAK)) ||
				(((cxItem + pCurItem->cxItem + gcxMenuFontChar) >
						pMenu->cxMenu) && (cItem != 0)))
			{
				cyItem += cMaxHeight;

				cxItem = xMenuLeft;
				cMaxHeight = pCurItem->cyItem;
				pMenu->fFlags |= MFMULTIROW;
			}

			pCurItem->yItem = cyItem;

			pCurItem->xItem = cxItem;
			cxItem += pCurItem->cxItem;
		}

		// 菜单最大宽度取所有Item中最宽者
		if (cMaxWidth < (int)(pCurItem->cxItem))
		{
			cMaxWidth = pCurItem->cxItem;
		}

		if (cMaxHeight != pCurItem->cyItem)
		{
			if (cMaxHeight < pCurItem->cyItem)
			{
				cMaxHeight = pCurItem->cyItem;
			}

			if (!fPopupMenu)
			{
				menuHeight = cMaxHeight;
			}
		}

		if (!fPopupMenu)
		{
			cyItemKeep = cyItem + cMaxHeight;
		}

		pCurItem++;

	} /* of for loop */
	/*
	 * Determine where the strings should be drawn so they are aligned
	 * The alignment is only for popup (vertical) menus (see xxxRealDrawMenuItem)
	 * The actual space depends on the MNS_NOCHECK and MNS_CHECKORBMP styles
	 * Multicolumn menus don't get aligment (now that we have scrollbars, multicolum is out)
	 */
	if (!fStringAndBitmapItems || (cItemBegCol != 0))
	{
		pMenu->cxTextAlign = 0;
	}
	else if (TestMF(pMenu, MNS_NOCHECK))
	{
		pMenu->cxTextAlign += MNXSPACE;
	}
	else if (TestMF(pMenu, MNS_CHECKORBMP))
	{
		pMenu->cxTextAlign = max(pMenu->cxTextAlign, 0);
		pMenu->cxTextAlign += MNXSPACE;
	}
	else
	{
		pMenu->cxTextAlign += MNXSPACE;
	}
	/*
	 * Add the left margin
	 */
	if (pMenu->cxTextAlign != 0)
	{
		pMenu->cxTextAlign += MNLEFTMARGIN;
	}


	if (cItemBegCol && pMenu->cItems &&
		TestMFT(((PITEM)&pMenu->rgItems[0]), MFT_RIGHTJUSTIFY))
	{
		//
		// multi-column, if we are in RtoL mode, reverse the columns
		//
		pCurItem = (PITEM)&pMenu->rgItems[0];

		for (UINT cItem = 0; cItem < pMenu->cItems; cItem++)
		{
			pCurItem->xItem = pMenu->cxMenu -
				(pCurItem->xItem + pCurItem->cxItem);
			pCurItem++;
		}
	}

	SelectObject(hdc, hOldFont);
	ReleaseDC(NULL, hdc);

	pMenu->cyMenu = cyItemKeep - yMenuTop;
	ret = pMenu->cyMenu;

	if (lpdwMenuHeight != NULL)
	{
		*lpdwMenuHeight = menuHeight;
	}

	return ret;
}

BOOL IUI::xxxMNItemSize(
	PMENU pMenu,
	HWND pwndNotify,
	HDC hdc,
	PITEM pItem,
	BOOL fPopup,
	LPPOINT lppt)
{
	lppt->x = 200;
	lppt->y = 24;

	return 0;
}

/***************************************************************************\
* xxxEndMenu
*
* !
* Revalidation notes:
* o  xxxEndMenu must be called with a valid non-NULL pwnd.
* o  Revalidation is not required in this routine: pwnd is used at the start
*    to obtain pMenuState, and not used again.
*
* History:
\***************************************************************************/

void IUI::xxxEndMenu(
	PMENUSTATE pMenuState)
{
	//BOOL fMenuStateOwner;
	PPOPUPMENU  ppopup;
	//PTHREADINFO ptiCurrent;

	if ((ppopup = pMenuState->pGlobalPopupMenu) == NULL)
	{

		/*
		 * We're not really in menu mode. This can happen
		 *  if we are forced out of menu loop too soon; i.e, from
		 *  inside xxxMNGetPopup or xxxTrackPopupMenuEx.
		 */
		UserAssert(!pMenuState->fInsideMenuLoop && !pMenuState->fMenuStarted);
		return;
	}



	pMenuState->fInsideMenuLoop = FALSE;
	pMenuState->fMenuStarted = FALSE;
	/*
	 * Mark the popup as destroyed so people will not use it anymore.
	 * This means that root popups can be marked as destroyed before
	 * actually being destroyed (nice and confusing).
	 */
	ppopup->fDestroyed = TRUE;

	/*
	 * Determine if this is the menu loop owner before calling back.
	 * Only the owner can destroy the menu windows
	 */
	//ptiCurrent = PtiCurrent();
	//fMenuStateOwner = (ptiCurrent == pMenuState->ptiMenuStateOwner);

	/*
	 * Release mouse capture if we got it in xxxStartMenuState
	 */
	//if (ptiCurrent->pq->spwndCapture == pMenuState->pGlobalPopupMenu->spwndNotify)
	{
		xxxMNReleaseCapture();
	}

	/*
	 * Bail if this is not the menu loop owner
	 */
	//if (!fMenuStateOwner)
	//{
	//	//RIPMSG1(RIP_WARNING, "xxxEndMenu: Thread %#p doesn't own the menu loop", ptiCurrent);
	//	return;
	//}
	/*
	 * If the menu loop is running on a thread different than the thread
	 *  that owns spwndNotify, we can have two threads trying to end
	 *  this menu at the same time.
	 */
	if (pMenuState->fInEndMenu)
	{
		//RIPMSG1(RIP_WARNING, "xxxEndMenu: already in EndMenu. pMenuState:%#p", pMenuState);
		return;
	}
	pMenuState->fInEndMenu = TRUE;

	if (pMenuState->pGlobalPopupMenu->spwndNotify != NULL)
	{
		if (!pMenuState->pGlobalPopupMenu->fInCancel)
		{
			xxxMNDismiss(pMenuState);
		}
	}
	else
	{
		BOOL    fTrackedPopup = ppopup->fIsTrackPopup;

		/*
		 * This should do the same stuff as MenuCancelMenus but not send any
		 * messages...
		 */
		xxxMNCloseHierarchy(ppopup, pMenuState);

		if (fTrackedPopup)
		{
			::DestroyWindow(ppopup->spwndPopupMenu);
		}
	}
}

int IUI::SetPopupMenu(HWND hMenuWnd, PPOPUPMENU pPopupMenu)
{
	return SetWindowLong(hMenuWnd, 0, (LONG)pPopupMenu);
}

PPOPUPMENU IUI::GetPopupMenu(HWND hMenuWnd)
{
	return (PPOPUPMENU)GetWindowLong(hMenuWnd, 0);
}

PMENU IUI::HIUIMENUFromHMENU(HMENU hMenu)
{
	int nItemCount = GetMenuItemCount(hMenu);
	if (nItemCount <= 0)
	{
		return NULL;
	}

	PMENU pMenu = new MENU;
	ZeroMemory(pMenu, sizeof(MENU));
	pMenu->fFlags |= MFISPOPUP;
	for (int i = 0; i < nItemCount; ++i)
	{
		MENUITEMINFOW mii;
		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		// 经测试，在Win7上，fMask如果包含MIIM_TYPE标记，GetMenuItemInfo将返回FALSE。
		mii.fMask = MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE
			| MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
		BOOL bRet = GetMenuItemInfoW(hMenu, i, TRUE, &mii);

		if (!bRet)
		{
			delete pMenu;
			pMenu = NULL;
			break;
		}

		// 分配菜单项字符串所需的内存
		mii.dwTypeData = new WCHAR[mii.cch + 1];
		// 必须+=1，否则得到的字符串少最后一个字符。
		mii.cch += 1;
		bRet = GetMenuItemInfoW(hMenu, i, TRUE, &mii);

		// 由于有MIIM_SUBMENU标记，所以xxxInsertMenuItem内部会递归调用PMENUFromHMENU
		xxxInsertMenuItem(pMenu, i, TRUE, &mii, mii.dwTypeData);
	}

	return pMenu;
}

// Same as API LoadMenu
PMENU IUILoadMenu(HINSTANCE hInstance, LPCTSTR lpMenuName)
{
	HMENU hMenu = ::LoadMenu(hInstance, lpMenuName);
	if (hMenu == NULL)
	{
		return NULL;
	}

	return HIUIMENUFromHMENU(hMenu);
}


/***************************************************************************\
* StopFade
*
* 2/5/1998   vadimg          created
\***************************************************************************/

void IUI::StopFade(void)
{
	//	DWORD dwRop = SRCCOPY;
	//	HWND pwnd;
	//
	//	UserAssert(gfade.hdc != NULL);
	//	UserAssert(gfade.hbm != NULL);
	//
	//	/*
	//	 * Stop the fade animation timer.
	//	 */
	//	//_KillSystemTimer(gTermIO.spwndDesktopOwner, IDSYS_FADE);
	//
	//	pwnd = DeleteFadeSprite();
	//
	//	/*
	//	 * If showing and the animation isn't completed, blt the last frame.
	//	 */
	//	if (!(gfade.dwFlags & FADE_COMPLETED) && (gfade.dwFlags & FADE_SHOW))
	//	{
	//		int x, y;
	//		HDC hdc;
	//
	//		/*
	//		 * For a windowed fade, make sure we observe the current visrgn.
	//		 */
	//		if (pwnd != NULL)
	//		{
	//			hdc = _GetDCEx(pwnd, NULL, DCX_WINDOW | DCX_CACHE);
	//			x = 0;
	//			y = 0;
	//		}
	//		else
	//		{
	//			hdc = gpDispInfo->hdcScreen;
	//			x = gfade.ptDst.x;
	//			y = gfade.ptDst.y;
	//		}
	//
	//#ifdef USE_MIRRORING
	//		/*
	//		 * If the destination DC is RTL mirrored, then BitBlt call should mirror the
	//		 * content, since we want the menu to preserve it text (i.e. not to
	//		 * be flipped). [samera]
	//		 */
	//		if (GreGetLayout(hdc) & LAYOUT_RTL)
	//		{
	//			dwRop |= NOMIRRORBITMAP;
	//		}
	//#endif
	//
	//		GreBitBlt(hdc, x, y, gfade.size.cx, gfade.size.cy, gfade.hdc, 0, 0, dwRop, 0);
	//		_ReleaseDC(hdc);
	//	}
	//
	//	/*
	//	 * Clean up the animation data.
	//	 */
	//	GreSelectBitmap(gfade.hdc, GreGetStockObject(PRIV_STOCK_BITMAP));
	//	GreDeleteObject(gfade.hbm);
	//
	//	gfade.hbm = NULL;
	//	gfade.dwFlags = 0;
}

/***************************************************************************\
* FindCharPosition
*
* Finds position of character ch in lpString.  If not found, the length
* of the string is returned.
*
* History:
*   11-13-90 JimA                Created.
\***************************************************************************/

DWORD IUI::FindCharPosition(
	LPWSTR lpString,
	WCHAR ch)
{
	DWORD dwPos = 0L;

	while (*lpString && *lpString != ch)
	{
		++lpString;
		++dwPos;
	}
	return dwPos;
}

HRGN
CreateEmptyRgn(void)
{
	CIUIRect rect(0, 0, 0, 0);
	return CreateRectRgnIndirect(rect);
}

COLORREF IUI::SYSRGB(int nIndex)
{
	switch (nIndex)
	{
	case MENUTEXT:
		return RGB(0, 0, 0);
	case treeDFACE:
		return RGB(128, 128, 128);
	case GRAYTEXT:
		return RGB(100, 100, 100);
	}

	return 0;
}

HBRUSH IUI::SYSHBR(int nIndex)
{
	switch (nIndex)
	{
	case threeDSHADOW:
		return g_hbr3DSHADOW;
	case threeDHILIGHT:
		return g_hbr3DHILIGHT;
	case MENUHBR:
		return g_hbrMenu;
	case HIGHLIGHTTEXT:
		return g_hbrHIGHLIGHTTEXT;
	}

	return NULL;
}

/***************************************************************************\
* DrawFrame
*
* History:
\***************************************************************************/

BOOL IUI::DrawFrame(
	HDC   hdc,
	PRECT prc,
	int   clFrame,
	int   cmd)
{
	//int        x;
	//int        y;
	//int        cx;
	//int        cy;
	//int        cxWidth;
	//int        cyWidth;
	//HANDLE     hbrSave;
	//LONG       rop;
	//POLYPATBLT PolyData[4];

	//x = prc->left;
	//y = prc->top;

	//cxWidth = SYSMETRTL(CXBORDER) * clFrame;
	//cyWidth = SYSMETRTL(CYBORDER) * clFrame;

	//cx = prc->right - x - cxWidth;
	//cy = prc->bottom - y - cyWidth;

	//rop = ((cmd & DF_ROPMASK) ? PATINVERT : PATCOPY);

	//if ((cmd & DF_HBRMASK) == DF_GRAY)
	//{
	//	hbrSave = gpsi->hbrGray;
	//}
	//else
	//{
	//	UserAssert(((cmd & DF_HBRMASK) >> 3) < COLOR_MAX);
	//	hbrSave = SYSHBRUSH((cmd & DF_HBRMASK) >> 3);
	//}

	//PolyData[0].x         = x;
	//PolyData[0].y         = y;
	//PolyData[0].cx        = cxWidth;
	//PolyData[0].cy        = cy;
	//PolyData[0].BrClr.hbr = hbrSave;

	//PolyData[1].x         = x + cxWidth;
	//PolyData[1].y         = y;
	//PolyData[1].cx        = cx;
	//PolyData[1].cy        = cyWidth;
	//PolyData[1].BrClr.hbr = hbrSave;

	//PolyData[2].x         = x;
	//PolyData[2].y         = y + cy;
	//PolyData[2].cx        = cx;
	//PolyData[2].cy        = cyWidth;
	//PolyData[2].BrClr.hbr = hbrSave;

	//PolyData[3].x         = x + cx;
	//PolyData[3].y         = y + cyWidth;
	//PolyData[3].cx        = cxWidth;
	//PolyData[3].cy        = cy;
	//PolyData[3].BrClr.hbr = hbrSave;

	//UserPolyPatBlt(hdc, rop, &PolyData[0], 4, PPB_BRUSH);

	return TRUE;
}

/***************************************************************************\
* xxxPSMTextOut
*
* Outputs the text and puts and _ below the character with an &
* before it. Note that this routine isn't used for menus since menus
* have their own special one so that it is specialized and faster...
*
* NOTE: A very similar routine (UserLpkPSMTextOut) exists on the client
*       side in drawtext.c.  Any non-kernel specific changes to this
*       routine most likely need to be made in UserLpkPSMTextOut as well.
*
* History:
* 11-13-90 JimA         Ported to NT.
* 30-Nov-1992 mikeke    Client side version
* 8-Apr-1998 MCostea    Added dwFlags
\***************************************************************************/

void IUI::xxxPSMTextOut(
	HDC hdc,
	int xLeft,
	int yTop,
	LPWSTR lpsz,
	int cch,
	DWORD dwFlags)
{
	// 	int cx;
	// 	LONG textsize, result;
	// 	/*
	// 	 * In the kernel we have a limited amount of stack. So it should be a stack
	// 	 * variable in user mode and static in kernel mode where it is thread safe
	// 	 * since we are in the crit section.
	// 	 */
	// 	static WCHAR achWorkBuffer[255];
	// 	WCHAR *pchOut = achWorkBuffer;
	// 	TEXTMETRICW textMetric;
	// 	SIZE size;
	// 	RECT rc;
	// 	COLORREF color;
	// 	//PTHREADINFO ptiCurrent = PtiCurrentShared();
	//
	// 	if (CALL_LPK(ptiCurrent))
	// 	{
	// 		/*
	// 		 * A user mode LPK is installed for layout and shaping.
	// 		 * Perform callback and return.
	// 		 */
	// 		UNICODE_STRING ustrStr;
	//
	// 		RtlInitUnicodeString(&ustrStr, lpsz);
	// 		xxxClientPSMTextOut(hdc, xLeft, yTop, &ustrStr, cch, dwFlags);
	// 		return;
	// 	}
	//
	// 	if (cch > sizeof(achWorkBuffer) / sizeof(WCHAR))
	// 	{
	// 		pchOut = (WCHAR *)UserAllocPool((cch + 1) * sizeof(WCHAR), TAG_RTL);
	// 		if (pchOut == NULL)
	// 		{
	// 			return;
	// 		}
	// 	}
	//
	// 	result = GetPrefixCount(lpsz, cch, pchOut, cch);
	//
	// 	if (!(dwFlags & DT_PREFIXONLY))
	// 	{
	// 		_TextOutW(hdc, xLeft, yTop, pchOut, cch - HIWORD(result));
	// 	}
	//
	// 	/*
	// 	 * Any true prefix characters to underline?
	// 	 */
	// 	if (LOWORD(result) == 0xFFFF || dwFlags & DT_HIDEPREFIX)
	// 	{
	// 		if (pchOut != achWorkBuffer)
	// 		{
	// 			UserFreePool(pchOut);
	// 		}
	// 		return;
	// 	}
	//
	// 	if (!_GetTextMetricsW(hdc, &textMetric))
	// 	{
	// 		textMetric.tmOverhang = 0;
	// 		textMetric.tmAscent = 0;
	// 	}
	//
	// 	/*
	// 	 * For proportional fonts, find starting point of underline.
	// 	 */
	// 	if (LOWORD(result) != 0)
	// 	{
	//
	// 		/*
	// 		 * How far in does underline start (if not at 0th byte.).
	// 		 */
	// 		GreGetTextExtentW(hdc, (LPWSTR)pchOut, LOWORD(result), &size, GGTE_WIN3_EXTENT);
	// 		xLeft += size.cx;
	//
	// 		/*
	// 		 * Adjust starting point of underline if not at first char and there is
	// 		 * an overhang.  (Italics or bold fonts.)
	// 		 */
	// 		xLeft = xLeft - textMetric.tmOverhang;
	// 	}
	//
	// 	/*
	// 	 * Adjust for proportional font when setting the length of the underline and
	// 	 * height of text.
	// 	 */
	// 	GreGetTextExtentW(hdc, (LPWSTR)(pchOut + LOWORD(result)), 1, &size, GGTE_WIN3_EXTENT);
	// 	textsize = size.cx;
	//
	// 	/*
	// 	 * Find the width of the underline character.  Just subtract out the overhang
	// 	 * divided by two so that we look better with italic fonts.  This is not
	// 	 * going to effect embolded fonts since their overhang is 1.
	// 	 */
	// 	cx = LOWORD(textsize) - textMetric.tmOverhang / 2;
	//
	// 	/*
	// 	 * Get height of text so that underline is at bottom.
	// 	 */
	// 	yTop += textMetric.tmAscent + 1;
	//
	// 	/*
	// 	 * Draw the underline using the foreground color.
	// 	 */
	// 	SetRect(&rc, xLeft, yTop, xLeft + cx, yTop + 1);
	// 	color = GreSetBkColor(hdc, GreGetTextColor(hdc));
	// 	GreExtTextOutW(hdc, xLeft, yTop, ETO_OPAQUE, &rc, TEXT(""), 0, NULL);
	// 	GreSetBkColor(hdc, color);
	//
	// 	if (pchOut != achWorkBuffer)
	// 	{
	// 		UserFreePool(pchOut);
	// 	}
}

BOOL IUI::xxxPSMGetTextExtent(
	HDC hdc,
	LPWSTR lpstr,
	int cch,
	PSIZE psize)
{
	//     int result;
	//     WCHAR szTemp[255], *pchOut;
	//     PTHREADINFO ptiCurrent = PtiCurrentShared();
	//     TL tl;
	//
	//     if (cch > sizeof(szTemp)/sizeof(WCHAR)) {
	//         pchOut = (WCHAR*)UserAllocPool((cch+1) * sizeof(WCHAR), TAG_RTL);
	//         if (pchOut == NULL)
	//             return FALSE;
	//         ThreadLockPool(ptiCurrent, pchOut, &tl);
	//     } else {
	//         pchOut = szTemp;
	//     }
	//
	//     result = HIWORD(GetPrefixCount(lpstr, cch, pchOut, cch));
	//
	//     if (result) {
	//         lpstr = pchOut;
	//         cch -= result;
	//     }
	//     if (CALL_LPK(ptiCurrent)) {
	//         xxxClientGetTextExtentPointW(hdc, lpstr, cch, psize);
	//     } else {
	//         UserGetTextExtentPointW(hdc, lpstr, cch, psize);
	//     }
	//     if (pchOut != szTemp)
	//         ThreadUnlockAndFreePool(ptiCurrent, &tl);

	/*
	 * IanJa everyone seems to ignore the ret val
	 */
	return TRUE;
}

/***************************************************************************\
* GetSysMenuHandle
*
* Returns a handle to the system menu of the given window. NULL if
* the window doesn't have a system menu.
*
* History:
\***************************************************************************/

PMENU IUI::xxxGetSysMenuHandle(
	HWND pwnd)
{
	return NULL;
	//     PMENU pMenu;
	//
	//     CheckLock(pwnd);
	//
	//     if (TestWF(pwnd, WFSYSMENU)) {
	//         pMenu = pwnd->spmenuSys;
	//
	//         /*
	//          * If the window doesn't have a System Menu, use the default one.
	//          */
	//         if (pMenu == NULL) {
	//
	//             /*
	//              * Grab the menu from the desktop.  If the desktop menu
	//              * has not been loaded and this is not a system thread,
	//              * load it now.  Callbacks cannot be made from a system
	//              * thread or when a thread is in cleanup.
	//              */
	//             pMenu = pwnd->head.rpdesk->spmenuSys;
	//
	//             /*
	//              * Do not do callbacks if the thread is exiting.  We ran into this when
	//              * destroying a thread's window and the window it was promoting to
	//              * foreground was a hard error popup.
	//              */
	//             if (pMenu == NULL && !(PtiCurrent()->TIF_flags & (TIF_SYSTEMTHREAD | TIF_INCLEANUP))) {
	//
	//                 pMenu = xxxLoadSysDesktopMenu (&pwnd->head.rpdesk->spmenuSys, ID_SYSMENU);
	//             }
	//         }
	//     } else {
	//         pMenu = NULL;
	//     }
	//
	//     return pMenu;
}
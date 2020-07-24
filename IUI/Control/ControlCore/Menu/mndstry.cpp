/**************************** Module Header ********************************\
* Module Name: mndstry.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Menu Destruction Routines
*
* History:
* 10-10-90 JimA       Created.
* 02-11-91 JimA       Added access checks.
* 03-18-91 IanJa      Window revalidation added (none required)
\***************************************************************************/

#include "stdafx.h"
#include "menu.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#pragma hdrstop

/***************************************************************************\
* HMMarkObjectDestroy
*
* Marks an object for destruction.
*
* Returns TRUE if the object can be destroyed; that is, if it's
* lock count is 0.
*
* 02-10-92 ScottLu      Created.
\***************************************************************************/

BOOL HMMarkObjectDestroy(
	PVOID pobj)
{
	//	PHE phe;
	//
	//	phe = HMPheFromObject(pobj);
	//
	//#if DEBUGTAGS
	//	/*
	//	 * Record where the object was marked for destruction.
	//	 */
	//	if (IsDbgTagEnabled(DBGTAG_TrackLocks))
	//	{
	//		if (!(phe->bFlags & HANDLEF_DESTROY))
	//		{
	//			HMRecordLock(LOCKRECORD_MARKDESTROY, pobj, ((PHEAD)pobj)->cLockObj);
	//		}
	//	}
	//#endif
	//
	//	/*
	//	 * Set the destroy flag so our unlock code will know we're trying to
	//	 * destroy this object.
	//	 */
	//	phe->bFlags |= HANDLEF_DESTROY;
	//
	//	/*
	//	 * If this object can't be destroyed, then CLEAR the HANDLEF_INDESTROY
	//	 * flag - because this object won't be currently "in destruction"!
	//	 * (if we didn't clear it, when it was unlocked it wouldn't get destroyed).
	//	 */
	//	if (((PHEAD)pobj)->cLockObj != 0)
	//	{
	//		phe->bFlags &= ~HANDLEF_INDESTROY;
	//
	//		/*
	//		 * Return FALSE because we can't destroy this object.
	//		 */
	//		return FALSE;
	//	}
	//
	//#if DBG
	//	/*
	//	 * Ensure that this function only returns TRUE once.
	//	 */
	//	UserAssert(!(phe->bFlags & HANDLEF_MARKED_OK));
	//	phe->bFlags |= HANDLEF_MARKED_OK;
	//#endif

	/*
	 * Return TRUE because Lock count is zero - ok to destroy this object.
	 */
	return TRUE;
}

/***************************************************************************\
* HMFreeObject
*
* Frees an object - the handle and the referenced memory.
*
* 01-13-92 ScottLu      Created.
\***************************************************************************/

BOOL HMFreeObject(
	PVOID pobj)
{
	//	PHE         pheT;
	//	WORD        wUniqT;
	//	PHANDLEPAGE php;
	//	DWORD       i;
	//	ULONG_PTR    iheCurrent, *piheCurrentHead;
	//	BYTE        bCreateFlags;
	//	PDESKTOP    pdesk;
	//	PPROCESSINFO ppiQuotaCharge = NULL;
	//#if DBG
	//	PLR         plrT, plrNextT;
	//	unsigned char  notUsed;
	//#endif
	//
	//	UserAssert(((PHEAD)pobj)->cLockObj == 0);
	//	UserAssert(pobj == HtoPqCat(PtoHq(pobj)));
	//	/*
	//	 * Free the object first.
	//	 */
	//	pheT = HMPheFromObject(pobj);
	//	bCreateFlags = gahti[pheT->bType].bObjectCreateFlags;
	//
	//	UserAssertMsg1(pheT->bType != TYPE_FREE,
	//		"Object already marked as freed!!! %#p", pobj);
	//
	//	/*
	//	 * decr process handle use
	//	 */
	//	if (bCreateFlags & OCF_PROCESSOWNED)
	//	{
	//		ppiQuotaCharge = (PPROCESSINFO)pheT->pOwner;
	//		UserAssert(ppiQuotaCharge != NULL);
	//	}
	//	else if (bCreateFlags & OCF_THREADOWNED)
	//	{
	//		ppiQuotaCharge = (PPROCESSINFO)(((PTHREADINFO)(pheT->pOwner))->ppi);
	//		UserAssert(ppiQuotaCharge != NULL);
	//	}
	//	else
	//	{
	//		ppiQuotaCharge = NULL;
	//	}
	//
	//	if (ppiQuotaCharge != NULL)
	//	{
	//		ppiQuotaCharge->UserHandleCount--;
	//	}
	//
	//	if (pheT->bFlags & HANDLEF_GRANTED)
	//	{
	//		HMCleanupGrantedHandle(pheT->phead->h);
	//		pheT->bFlags &= ~HANDLEF_GRANTED;
	//	}
	//
	//#if DBG
	//	/*
	//	 *   performance counters
	//	 */
	//	gaPerfhti[pheT->bType].lCount--;
	//
	//	if ((bCreateFlags & OCF_DESKTOPHEAP) && ((PDESKOBJHEAD)pobj)->rpdesk)
	//	{
	//		pdesk = ((PDESKOBJHEAD)pobj)->rpdesk;
	//		gaPerfhti[pheT->bType].lSize -= RtlSizeHeap(Win32HeapGetHandle(pdesk->pheapDesktop), 0, pobj);
	//	}
	//	else if (bCreateFlags & OCF_SHAREDHEAP)
	//	{
	//		gaPerfhti[pheT->bType].lSize -= RtlSizeHeap(Win32HeapGetHandle(gpvSharedAlloc), 0, pobj);
	//	}
	//	else
	//	{
	//		gaPerfhti[pheT->bType].lSize -= ExQueryPoolBlockSize(pobj, &notUsed);
	//	}
	//
	//#endif // DBG
	//
	//	if ((bCreateFlags & OCF_DESKTOPHEAP))
	//	{
	//#if DBG
	//		BOOL bSuccess;
	//#endif
	//		UserAssert(((PDESKOBJHEAD)pobj)->rpdesk != NULL);
	//
	//		pdesk = ((PDESKOBJHEAD)pobj)->rpdesk;
	//		UnlockDesktop(&((PDESKOBJHEAD)pobj)->rpdesk, LDU_OBJ_DESK, (ULONG_PTR)pobj);
	//
	//		if (pheT->bFlags & HANDLEF_POOL)
	//		{
	//			UserFreePool(pobj);
	//		}
	//		else
	//		{
	//
	//#if DBG
	//			bSuccess =
	//#endif
	//				DesktopFree(pdesk, pobj);
	//#if DBG
	//			if (!bSuccess)
	//			{
	//				/*
	//				 * We would hit this assert in HYDRA trying to free the
	//				 * mother desktop window which was allocated out of pool
	//				 */
	//				RIPMSG1(RIP_ERROR, "Object already freed from desktop heap! %#p", pobj);
	//			}
	//#endif
	//		}
	//
	//	}
	//	else if (bCreateFlags & OCF_SHAREDHEAP)
	//	{
	//		SharedFree(pobj);
	//	}
	//	else
	//	{
	//		UserFreePool(pobj);
	//	}
	//
	//#if DBG
	//	/*
	//	 * Go through and delete the lock records, if they exist.
	//	 */
	//	for (plrT = pheT->plr; plrT != NULL; plrT = plrNextT)
	//	{
	//
	//		/*
	//		 * Remember the next one before freeing this one.
	//		 */
	//		plrNextT = plrT->plrNext;
	//		FreeLockRecord((HANDLE)plrT);
	//	}
	//#endif
	//
	//	/*
	//	 * Clear the handle contents. Need to remember the uniqueness across
	//	 * the clear. Also, advance uniqueness on free so that uniqueness checking
	//	 * against old handles also fails.
	//	 */
	//	wUniqT = (WORD)((pheT->wUniq + 1) & HMUNIQBITS);
	//	RtlZeroMemory(pheT, sizeof(HANDLEENTRY));
	//	pheT->wUniq = wUniqT;
	//
	//	/*
	//	 * Change the handle type to TYPE_FREE so we know what type this handle
	//	 * is. (TYPE_FREE is defined as zero)
	//	 */
	//	/* pheT->bType = TYPE_FREE; */
	//	UserAssert(pheT->bType == TYPE_FREE);
	//
	//	/*
	//	 * Put the handle on the free list of the appropriate page.
	//	 */
	//	php = gpHandlePages;
	//	iheCurrent = pheT - gSharedInfo.aheList;
	//	for (i = 0; i < gcHandlePages; ++i, ++php)
	//	{
	//		if (iheCurrent < php->iheLimit)
	//		{
	//			piheCurrentHead = (iheCurrent & 0x1 ? &php->iheFreeOdd : &php->iheFreeEven);
	//			pheT->phead = (PHEAD) * piheCurrentHead;
	//			*piheCurrentHead = iheCurrent;
	//			DBGHMValidateFreeLists();
	//			break;
	//		}
	//	}
	//	/*
	//	 * We must have found it.
	//	 */
	//	UserAssert(i < gcHandlePages);
	//
	//	UserAssert(pheT->pOwner == NULL);
	//
	//	DBGValidateHandleQuota();

	return TRUE;
}

/***************************************************************************\
* DestroyMenu
*
* Destroy a menu and free its memory.
*
* History:
* 10-11-90 JimA         Translated from ASM.
* 02-11-91 JimA         Added access checks.
\***************************************************************************/

BOOL IUI::_DestroyMenu(
	PMENU pMenu)
{
	PITEM pItem;
	int i;
	PDESKTOP rpdeskLock;

	if (pMenu == NULL)
	{
		return FALSE;
	}

	/*
	 * If the object is locked, just mark it for destroy and don't
	 * free it yet.
	 */
	if (!HMMarkObjectDestroy(pMenu))
	{
		return TRUE;
	}

	/*
	 * Go down the item list and free the items
	 */
	pItem = pMenu->rgItems;
	for (i = pMenu->cItems; i--; ++pItem)
	{
		MNFreeItem(pMenu, pItem, TRUE);
	}

	/*
	 * free the menu items
	 */
	if (pMenu->rgItems)
	{
		delete []pMenu->rgItems;
	}

	/*
	 * Because menus are the only objects on the desktop owned
	 * by the process and process cleanup is done after thread
	 * cleanup, this may be the last reference to the desktop.
	 * We must lock the desktop before unlocking
	 * the parent desktop reference and freeing the menu to
	 * ensure that the desktop will not be freed until after
	 * the menu is freed.  Don't use static locks because
	 * the pti for this thread will not be valid during
	 * process cleanup.
	 */
	rpdeskLock = NULL;
	//LockDesktop(&rpdeskLock, pMenu->head.rpdesk, LDL_FN_DESTROYMENU, (ULONG_PTR)PtiCurrent());

	/*
	 * Unlock all menu objects.
	 */
	Unlock(&pMenu->spwndNotify);

	HMFreeObject(pMenu);

	//UnlockDesktop(&rpdeskLock, LDU_FN_DESTROYMENU, (ULONG_PTR)PtiCurrent());

	return TRUE;
}

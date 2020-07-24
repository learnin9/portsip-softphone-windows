#include "stdafx.h"
#include "Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


BOOL fHaveIMMProcs;
BOOL fHaveNLSProcs = FALSE;							// TRUE if procs are loaded
BOOL fHaveIMEShareProcs = FALSE;

char *win95userprocs[] =
{
	"GetKeyboardLayout", 							// 14
	"GetKeyboardLayoutList", 						// 15
	""
};
char *win95gdiprocs[] =
{
	"TranslateCharsetInfo",							// 16
	""
};
char *imeshareprocs[] =
{
	"FSupportSty",									// 17
	"PIMEStyleFromAttr",							// 18
	"PColorStyleTextFromIMEStyle",					// 19
	"PColorStyleBackFromIMEStyle",					// 20
	"FBoldIMEStyle",								// 21
	"FItalicIMEStyle",								// 22
	"FUlIMEStyle",									// 23
	"IdUlIMEStyle",									// 24
	"RGBFromIMEColorStyle",							// 25
	""
};
char *immlibprocs[] =
{
	"ImmGetCompositionStringA", 					// proc # 0
	"ImmGetContext", 								// 1
	"ImmSetCompositionFontA", 						// 2
	"ImmSetCompositionWindow", 						// 3
	"ImmReleaseContext",							// 4
	"ImmGetProperty",								// 5
	"ImmGetCandidateWindow",						// 6
	"ImmSetCandidateWindow",						// 7
	"ImmNotifyIME",									// 8
	"ImmAssociateContext",							// 9
	"ImmGetVirtualKey",								// 10
	"ImmSetOpenStatus",								// 11
	"ImmGetConversionStatus",						// 12
	"ImmEscapeA",									// 13
	""
};

LibsAndProcs libAndProcList[] =  					// loop support.
{
	"imm32", immlibprocs, &fHaveIMMProcs, 0,
	"user32", win95userprocs, &fHaveNLSProcs, 14,
	"gdi32", win95gdiprocs, &fHaveNLSProcs, 16,
	"imeshare", imeshareprocs, &fHaveIMEShareProcs, 17,
	"", NULL, NULL, (SHORT)MAX_PROCS
};

#if (_MSC_VER <= 1200) // VC6.0

fnDPA_InsertPtr IUI::IUI_DPA_InsertPtr = NULL;
fnDPA_DeletePtr IUI::IUI_DPA_DeletePtr = NULL;
fnDPA_GetPtr IUI::IUI_DPA_GetPtr = NULL;
fnDPA_SetPtr IUI::IUI_DPA_SetPtr = NULL;
fnDPA_EnumCallback IUI::IUI_DPA_EnumCallback = NULL;
fnDPA_DeleteAllPtrs IUI::IUI_DPA_DeleteAllPtrs = NULL;
fnDPA_DestroyCallback IUI::IUI_DPA_DestroyCallback = NULL;
fnDPA_Destroy IUI::IUI_DPA_Destroy = NULL;
fnDPA_Sort IUI::IUI_DPA_Sort = NULL;
fnDPA_Create IUI::IUI_DPA_Create = NULL;
fnDPA_CreateEx IUI::IUI_DPA_CreateEx = NULL;
fnDPA_Grow IUI::IUI_DPA_Grow = NULL;
fnDPA_Clone IUI::IUI_DPA_Clone = NULL;
fnDPA_GetPtrIndex IUI::IUI_DPA_GetPtrIndex = NULL;

fnDSA_Destroy IUI::IUI_DSA_Destroy = NULL;
fnDSA_GetItemPtr IUI::IUI_DSA_GetItemPtr = NULL;
fnDSA_InsertItem IUI::IUI_DSA_InsertItem = NULL;
fnDSA_Create IUI::IUI_DSA_Create = NULL;
fnDSA_GetItem IUI::IUI_DSA_GetItem = NULL;
fnDSA_SetItem IUI::IUI_DSA_SetItem = NULL;
fnDSA_DeleteItem IUI::IUI_DSA_DeleteItem = NULL;
fnDSA_DestroyCallback IUI::IUI_DSA_DestroyCallback = NULL;
fnReleaseActCtx IUI::IUI_ReleaseActCtx = NULL;

struct IUISUBCLASS_CALL
{
	SUBCLASSPROC pfnSubclass;        // subclass procedure
	WPARAM uIdSubclass;        // unique subclass identifier
	DWORD_PTR dwRefData;          // optional ref data
};

BOOL IUI::IUI_SetWindowSubclass(
	HWND hWnd,
	SUBCLASSPROC pfnSubclass,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData)
{
	BOOL bRet = _SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
	if (!bRet)
	{
		return FALSE;
	}

	std::map<HWND, std::set<void *> > *pMap = iuiMapSubclass(TRUE);
	_ASSERT(pMap != NULL);

	// 先看看HWND是否设置过subclass列表，没有就设置一个
	std::map<HWND, std::set<void *> >::iterator itMap = pMap->find(hWnd);
	if (itMap == pMap->end())
	{
		std::set<void *> s;
		(*pMap)[hWnd] = s;
	}

	itMap = pMap->find(hWnd);

	// 得到窗口绑定的subclass列表
	std::set<void *> *pSet = &(itMap->second);

	// 看看subclass列表中是否已经存在这个子类信息，如果有就返回这个节点
	IUISUBCLASS_CALL *pSubclassInfo = NULL;
	std::set<void *>::iterator itSet = pSet->begin();
	for (; itSet != pSet->end(); ++itSet)
	{
		IUISUBCLASS_CALL *pCall = (IUISUBCLASS_CALL *)(*itSet);

		if (pCall->pfnSubclass == pfnSubclass
			&& pCall->uIdSubclass == uIdSubclass)
		{
			pSubclassInfo = pCall;
			break;
		}
	}

	// 如果没有找到，就新建一个节点
	if (pSubclassInfo == NULL)
	{
		pSubclassInfo = new IUISUBCLASS_CALL;

		pSet->insert(pSubclassInfo);
	}

	pSubclassInfo->pfnSubclass = pfnSubclass;
	pSubclassInfo->uIdSubclass = uIdSubclass;
	pSubclassInfo->dwRefData = dwRefData;

	return TRUE;
}

BOOL IUI::IUI_GetWindowSubclass(
	HWND hWnd,
	SUBCLASSPROC pfnSubclass,
	UINT_PTR uIdSubclass,
	DWORD_PTR *pdwRefData)
{
	if (NULL == pdwRefData)
	{
		return FALSE;
	}

	*pdwRefData = 0;

	std::map<HWND, std::set<void *> > *pMap = iuiMapSubclass(FALSE);
	if (NULL == pMap)
	{
		return FALSE;
	}

	// 先看看HWND是否设置过subclass列表
	std::map<HWND, std::set<void *> >::iterator itMap = pMap->find(hWnd);
	if (itMap == pMap->end())
	{
		return FALSE;
	}

	// 得到窗口绑定的subclass列表
	std::set<void *> *pSet = &(itMap->second);

	// 看看subclass列表中是否已经存在这个子类信息
	IUISUBCLASS_CALL *pSubclassInfo = NULL;
	std::set<void *>::iterator itSet = pSet->begin();
	for (; itSet != pSet->end(); ++itSet)
	{
		IUISUBCLASS_CALL *pCall = (IUISUBCLASS_CALL *)(*itSet);

		if (pCall->pfnSubclass == pfnSubclass
			&& pCall->uIdSubclass == uIdSubclass)
		{
			*pdwRefData = pCall->dwRefData;
			break;
		}
	}

	return TRUE;
}

BOOL IUI::IUI_RemoveWindowSubclass(
	HWND hWnd,
	SUBCLASSPROC pfnSubclass,
	UINT_PTR uIdSubclass)
{
	BOOL bRet = _RemoveWindowSubclass(hWnd, pfnSubclass, uIdSubclass);
	if (!bRet)
	{
		return FALSE;
	}

	std::map<HWND, std::set<void *> > *pMap = iuiMapSubclass(FALSE);
	if (NULL == pMap)
	{
		return FALSE;
	}

	// 先看看HWND是否设置过subclass列表
	std::map<HWND, std::set<void *> >::iterator itMap = pMap->find(hWnd);
	if (itMap == pMap->end())
	{
		return FALSE;
	}

	// 得到窗口绑定的subclass列表
	std::set<void *> *pSet = &(itMap->second);

	// 看看subclass列表中是否已经存在这个子类信息
	IUISUBCLASS_CALL *pSubclassInfo = NULL;
	std::set<void *>::iterator itSet = pSet->begin();
	for (; itSet != pSet->end(); ++itSet)
	{
		IUISUBCLASS_CALL *pCall = (IUISUBCLASS_CALL *)(*itSet);

		if (pCall->pfnSubclass == pfnSubclass
			&& pCall->uIdSubclass == uIdSubclass)
		{
			pSet->erase(itSet);
			break;
		}
	}

	return TRUE;
}


fnSetWindowSubclass IUI::_SetWindowSubclass = NULL;
fnGetWindowSubclass IUI::_GetWindowSubclass = NULL;
fnRemoveWindowSubclass IUI::_RemoveWindowSubclass = NULL;
fnDefSubclassProc IUI::IUI_DefSubclassProc = NULL;

#endif // #if (_MSC_VER <= 1200) // VC6.0

LRESULT PASCAL CIHandleNotifyFormat(LibUIDK_LPCONTROLINFO lpci, LPARAM lParam)
{
	if (lParam == NF_QUERY)
	{
#ifdef UNICODE
		return NFR_UNICODE;
#else
		return NFR_ANSI;
#endif
	}
	else if (lParam == NF_REQUERY)
	{
		LRESULT uiResult;

		uiResult = SendMessage(GetHostWnd(lpci->hwnd), WM_NOTIFYFORMAT,
				(WPARAM)GetHostWnd(lpci->hwnd), NF_QUERY);

		lpci->bUnicode = BOOLIFY(uiResult == NFR_UNICODE);

		return uiResult;
	}
	return 0;
}

LRESULT FAR PASCAL IUI::IUI_CIHandleNotifyFormat(LPIUICONTROLINFO lpci, LPARAM lParam)
{
	if (lParam == NF_QUERY)
	{
#ifdef UNICODE
		return NFR_UNICODE;
#else
		return NFR_ANSI;
#endif
	}
	else if (lParam == NF_REQUERY)
	{
		LRESULT uiResult;

		uiResult = ::SendMessage(lpci->hwndParent, WM_NOTIFYFORMAT,
				(WPARAM)lpci->hwnd, NF_QUERY);

		lpci->bUnicode = BOOLIFY(uiResult == NFR_UNICODE);

		return uiResult;
	}
	return 0;
}

void IUI::FlipPoint(LPPOINT lppt)
{
	SWAP(lppt->x, lppt->y, int);
}

void FAR PASCAL IUI::FlipRect(LPRECT prc)
{
	SWAP(prc->left, prc->top, int);
	SWAP(prc->right, prc->bottom, int);
}

//  this is implemented in toolbar.c, but we should be able to use
//  as well as long as we always set fHorizMode to FALSE
void PASCAL DrawInsertMark(HDC hdc, LPRECT prc, BOOL fHorizMode, COLORREF clr)
{
	HPEN hPnMark = CreatePen(PS_SOLID, 1, clr);
	HPEN hOldPn;
	POINT rgPoint[4];
	if (!hPnMark)
	{
		hPnMark = (HPEN)GetStockObject(BLACK_PEN);    // fallback to draw with black pen
	}
	hOldPn = (HPEN)SelectObject(hdc, (HGDIOBJ)hPnMark);

	if (fHorizMode)
	{
		int iXCentre = (prc->left + prc->right) / 2;

		rgPoint[0].x = iXCentre + 1;
		rgPoint[0].y = prc->top + 2;
		rgPoint[1].x = iXCentre + 3;
		rgPoint[1].y = prc->top;
		rgPoint[2].x = iXCentre - 2;
		rgPoint[2].y = prc->top;
		rgPoint[3].x = iXCentre;
		rgPoint[3].y = prc->top + 2;

		// draw the top bit...
		Polyline(hdc, rgPoint, 4);

		rgPoint[0].x = iXCentre;
		rgPoint[0].y = prc->top;
		rgPoint[1].x = iXCentre;
		rgPoint[1].y = prc->bottom - 1;
		rgPoint[2].x = iXCentre + 1;
		rgPoint[2].y = prc->bottom - 1;
		rgPoint[3].x = iXCentre + 1;
		rgPoint[3].y = prc->top;

		// draw the middle...
		Polyline(hdc, rgPoint, 4);

		rgPoint[0].x = iXCentre + 1;
		rgPoint[0].y = prc->bottom - 3;
		rgPoint[1].x = iXCentre + 3;
		rgPoint[1].y = prc->bottom - 1;
		rgPoint[2].x = iXCentre - 2;
		rgPoint[2].y = prc->bottom - 1;
		rgPoint[3].x = iXCentre;
		rgPoint[3].y = prc->bottom - 3;

		// draw the bottom bit...
		Polyline(hdc, rgPoint, 4);
	}
	else
	{
		int iYCentre = (prc->top + prc->bottom) / 2;

		rgPoint[0].x = prc->left + 2;
		rgPoint[0].y = iYCentre;
		rgPoint[1].x = prc->left;
		rgPoint[1].y = iYCentre - 2;
		rgPoint[2].x = prc->left;
		rgPoint[2].y = iYCentre + 3;
		rgPoint[3].x = prc->left + 2;
		rgPoint[3].y = iYCentre + 1;

		// draw the top bit...
		Polyline(hdc, rgPoint, 4);

		rgPoint[0].x = prc->left;
		rgPoint[0].y = iYCentre;
		rgPoint[1].x = prc->right - 1;
		rgPoint[1].y = iYCentre;
		rgPoint[2].x = prc->right - 1;
		rgPoint[2].y = iYCentre + 1;
		rgPoint[3].x = prc->left;
		rgPoint[3].y = iYCentre + 1;

		// draw the middle...
		Polyline(hdc, rgPoint, 4);

		rgPoint[0].x = prc->right - 3;
		rgPoint[0].y = iYCentre;
		rgPoint[1].x = prc->right - 1;
		rgPoint[1].y = iYCentre - 2;
		rgPoint[2].x = prc->right - 1;
		rgPoint[2].y = iYCentre + 3;
		rgPoint[3].x = prc->right - 3;
		rgPoint[3].y = iYCentre + 1;

		// draw the bottom bit...
		Polyline(hdc, rgPoint, 4);
	}

	SelectObject(hdc, hOldPn);
	DeleteObject((HGDIOBJ)hPnMark);
}

////////////////////////////////

LRESULT IUI::SendNotifyEx(HWLWND hwndTo, HWLWND hwndFrom, int code, NMHDR *pnmhdr, BOOL bUnicode)
{
	IUI_CONTROL_INFO ci;

	if (hwndTo == NULL)
	{
		if (IsWindow(hwndFrom))
		{
			hwndTo = GetParent(hwndFrom);
		}
		if (hwndTo == NULL)
		{
			return 0;
		}
	}


	ci.hwndParent = hwndTo;
	ci.hwnd = hwndFrom;
	ci.bUnicode = bUnicode;
#if TODO
	ci.uiCodePage = CP_ACP;
#endif

	return CCSendNotify(&ci, code, pnmhdr);
}

LRESULT IUI::CCSendNotify(IUI_CONTROL_INFO *pci, int code, LPNMHDR pnmhdr)
{
	NMHDR nmhdr;
	LONG_PTR id;
#ifdef UNICODE
	THUNKSTATE ts = { 0 };
#define pvThunk1 ts.ts_pvThunk1
#define pvThunk2 ts.ts_pvThunk2
#define dwThunkSize ts.ts_dwThunkSize
	LRESULT lRet;
	BOOL  bSet = FALSE;
#endif
	HWLWND hwndParent = pci->hwndParent;
	DWORD dwParentPid;

	// -1 means Requery on each notify
	if (hwndParent == (HWLWND)(-1))
	{
		hwndParent = GetParent(pci->hwnd);
	}

	//
	// If pci->hwnd is -1, then a WM_NOTIFY is being forwared
	// from one control to a parent.  EG:  Tooltips sent
	// a WM_NOTIFY to toolbar, and toolbar is forwarding it
	// to the real parent window.
	//

	if (pci->hwnd != (HWLWND)(-1))
	{

		//
		// If this is a child then get its ID.  We need to go out of our way to
		// avoid calling GetDlgCtrlID on toplevel windows since it will return
		// a pseudo-random number (those of you who know what this number is
		// keep quiet).  Anyway it's kinda hard to figure this out in Windows
		// because of the following:
		//
		//  - a window can SetWindowLong(GWL_STYLE, WS_CHILD) but this only
		//    does about half the work - hence checking the style is out.
		//  - GetParent will return your OWNER if you are toplevel.
		//  - there is no GetWindow(...GW_HWNDPARENT) to save us.
		//
		// Hence we are stuck with calling GetParent and then checking to see
		// if it lied and gave us the owner instead.  Yuck.
		//
		id = 0;
		if (pci->hwnd != NULL)
		{
			HWLWND hwndParent = GetParent(pci->hwnd);

			//L if (hwndParent && (hwndParent != GetWindow(pci->hwnd, GW_OWNER)))
			//{
			//	id = GetDlgCtrlID(pci->hwnd);
			//}
		}

		if (!pnmhdr)
		{
			pnmhdr = &nmhdr;
		}

		pnmhdr->hwndFrom = (HWND)pci->hwnd;
		pnmhdr->idFrom = id;
		pnmhdr->code = code;
	}
	else
	{

		id = pnmhdr->idFrom;
		code = pnmhdr->code;
	}


	// OLE in its massively componentized world sometimes creates
	// a control whose parent belongs to another process.  (For example,
	// when there is a local server embedding.)  WM_NOTIFY
	// messages can't cross process boundaries, so stop the message
	// from going there lest we fault the recipient.
	if (!GetWindowThreadProcessId(GetHostWnd(pci->hwnd), &dwParentPid) ||
		dwParentPid != GetCurrentProcessId())
	{
		//L TraceMsg(TF_WARNING, "nf: Not sending WM_NOTIFY %08x across processes", code);
		return 0;
	}

#ifdef NEED_WOWGETNOTIFYSIZE_HELPER
	_ASSERT(code >= 0 || WOWGetNotifySize(code));
#endif // NEED_WOWGETNOTIFYSIZE_HELPER

#ifdef UNICODE
	/*
	 * All the thunking for Notify Messages happens here
	 */
	if (!pci->bUnicode)
	{
		BOOL fThunked = TRUE;
		switch (code)
		{
		case LVN_ODFINDITEMW:
			pnmhdr->code = LVN_ODFINDITEMA;
			goto ThunkLV_FINDINFO;

		case LVN_INCREMENTALSEARCHW:
			pnmhdr->code = LVN_INCREMENTALSEARCHA;
			goto ThunkLV_FINDINFO;

ThunkLV_FINDINFO:
			{
				LV_FINDINFO *plvfi;

				// Hack Alert!  This code assumes that all fields of LV_FINDINFOA and
				// LV_FINDINFOW are exactly the same except for the string pointers.
				COMPILETIME_ASSERT(sizeof(LV_FINDINFOA) == sizeof(LV_FINDINFOW));

				// Since WCHARs are bigger than char, we will just use the
				// wchar buffer to hold the chars, and not worry about the extra
				// room at the end.
				COMPILETIME_ASSERT(sizeof(WCHAR) >= sizeof(char));

				plvfi = &((PNM_FINDITEM)pnmhdr)->lvfi;
				if (plvfi->flags & (LVFI_STRING | LVFI_PARTIAL | LVFI_SUBSTRING))
				{
					pvThunk1 = (PVOID)plvfi->psz;
					dwThunkSize = lstrlen((LPCWSTR)pvThunk1) + 1;
					plvfi->psz = (LPWSTR)ProduceAFromW(pci->uiCodePage, plvfi->psz);
				}
			}
			break;

		case LVN_GETDISPINFOW:
		{
			LV_ITEMW *pitem;

			pnmhdr->code = LVN_GETDISPINFOA;

			// Hack Alert!  This code assumes that all fields of LV_DISPINFOA and
			// LV_DISPINFOW are exactly the same except for the string pointers.

			COMPILETIME_ASSERT(sizeof(LV_DISPINFOA) == sizeof(LV_DISPINFOW));

			// Since WCHARs are bigger than char, we will just use the
			// wchar buffer to hold the chars, and not worry about the extra
			// room at the end.
			COMPILETIME_ASSERT(sizeof(WCHAR) >= sizeof(char));

			//
			// Some sleazebag code (shell32.dll) just changes the pszText
			// pointer to point to the name, so capture the original pointer
			// so we can detect this and not smash their data.
			//
			pitem = &(((LV_DISPINFOW *)pnmhdr)->item);
			if (!IsFlagPtr(pitem) && (pitem->mask & LVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
			}
			break;
		}


		// LVN_ENDLABELEDIT uses an INOUT parameter, never explicitly
		// documented as such, but it just happened to be that way,
		// and I don't want to take the chance that somebody was relying
		// on it.

		case LVN_ENDLABELEDITW:
			pnmhdr->code = LVN_ENDLABELEDITA;
			goto ThunkLV_DISPINFO;

		case LVN_BEGINLABELEDITW:
			pnmhdr->code = LVN_BEGINLABELEDITA;
			goto ThunkLV_DISPINFO;

		case LVN_SETDISPINFOW:
			pnmhdr->code = LVN_SETDISPINFOA;
			goto ThunkLV_DISPINFO;

		case LVN_GETEMPTYTEXTW:
			pnmhdr->code = LVN_GETEMPTYTEXTA;
			goto ThunkLV_DISPINFO;

ThunkLV_DISPINFO:
			{
				LV_ITEMW *pitem;

				COMPILETIME_ASSERT(sizeof(LV_ITEMA) == sizeof(LV_ITEMW));
				pitem = &(((LV_DISPINFOW *)pnmhdr)->item);

				if (pitem->mask & LVIF_TEXT)
				{
					if (!InOutWtoA(pci, &ts, &pitem->pszText, pitem->cchTextMax))
					{
						return 0;
					}
				}
				break;
			}

		case LVN_GETINFOTIPW:
		{
			NMLVGETINFOTIPW *pgit = (NMLVGETINFOTIPW *)pnmhdr;

			COMPILETIME_ASSERT(sizeof(NMLVGETINFOTIPA) == sizeof(NMLVGETINFOTIPW));
			pnmhdr->code = LVN_GETINFOTIPA;

			if (!InOutWtoA(pci, &ts, &pgit->pszText, pgit->cchTextMax))
			{
				return 0;
			}
		}
		break;


		case TVN_GETINFOTIPW:
		{
			NMTVGETINFOTIPW *pgit = (NMTVGETINFOTIPW *)pnmhdr;

			pnmhdr->code = TVN_GETINFOTIPA;

			pvThunk1 = pgit->pszText;
			dwThunkSize = pgit->cchTextMax;
		}
		break;

		case TBN_GETINFOTIPW:
		{
			NMTBGETINFOTIPW *pgit = (NMTBGETINFOTIPW *)pnmhdr;

			pnmhdr->code = TBN_GETINFOTIPA;

			pvThunk1 = pgit->pszText;
			dwThunkSize = pgit->cchTextMax;
		}
		break;

		case TVN_SELCHANGINGW:
			pnmhdr->code = TVN_SELCHANGINGA;
			bSet = TRUE;
		// fall through

		case TVN_SELCHANGEDW:
			if (!bSet)
			{
				pnmhdr->code = TVN_SELCHANGEDA;
				bSet = TRUE;
			}

		/*
		 * These msgs have a NM_TREEVIEW with both TV_ITEMs filled in
		 *
		 * FALL THROUGH TO TVN_DELETEITEM to thunk itemOld then go on for
		 * the other structure.
		 */

		// fall through

		case TVN_DELETEITEMW:
		{
			/*
			 * This message has a NM_TREEVIEW in lParam with itemOld filled in
			 */
			LPTV_ITEMW pitem;

			if (!bSet)
			{
				pnmhdr->code = TVN_DELETEITEMA;
				bSet = TRUE;
			}

			pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemOld);

			// thunk itemOld
			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk2 = pitem->pszText;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk2);
			}

			// if this is deleteitem then we are done
			if (pnmhdr->code == TVN_DELETEITEMA)
			{
				break;
			}

			/* FALL THROUGH TO TVN_ITEMEXPANDING to thunk itemNew */
		}
		// fall through

		case TVN_ITEMEXPANDINGW:
			if (!bSet)
			{
				pnmhdr->code = TVN_ITEMEXPANDINGA;
				bSet = TRUE;
			}
		// fall through

		case TVN_ITEMEXPANDEDW:
			if (!bSet)
			{
				pnmhdr->code = TVN_ITEMEXPANDEDA;
				bSet = TRUE;
			}
		// fall through

		case TVN_BEGINDRAGW:
			if (!bSet)
			{
				pnmhdr->code = TVN_BEGINDRAGA;
				bSet = TRUE;
			}
		// fall through

		case TVN_BEGINRDRAGW:
		{
			/* these msgs have a NM_TREEVIEW with itemNew TV_ITEM filled in */
			LPTV_ITEMW pitem;

			if (!bSet)
			{
				pnmhdr->code = TVN_BEGINRDRAGA;
			}

			pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemNew);

			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk1);
			}

			break;
		}

		case TVN_SETDISPINFOW:
			pnmhdr->code = TVN_SETDISPINFOA;
			goto ThunkTV_DISPINFO;

		case TVN_BEGINLABELEDITW:
			pnmhdr->code = TVN_BEGINLABELEDITA;
			goto ThunkTV_DISPINFO;


		// TVN_ENDLABELEDIT uses an INOUT parameter, never explicitly
		// documented as such, but it just happened to be that way,
		// and I don't want to take the chance that somebody was relying
		// on it.

		case TVN_ENDLABELEDITW:
			pnmhdr->code = TVN_ENDLABELEDITA;
			goto ThunkTV_DISPINFO;

ThunkTV_DISPINFO:
			{
				/*
				 * All these messages have a TV_DISPINFO in lParam.
				 */

				LPTV_ITEMW pitem;

				pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

				if (pitem->mask & TVIF_TEXT)
				{
					if (!InOutWtoA(pci, &ts, &pitem->pszText, pitem->cchTextMax))
					{
						return 0;
					}
				}
				break;
			}

#if !defined(UNIX) || defined(ANSI_SHELL32_ON_UNIX)
		/* UNIX shell32 TVN_GETDISPINFOA TVN_GETDISPINFOW does the same thing */
		case TVN_GETDISPINFOW:
		{
			/*
			 * All these messages have a TV_DISPINFO in lParam.
			 */
			LPTV_ITEMW pitem;

			pnmhdr->code = TVN_GETDISPINFOA;

			pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText) && pitem->cchTextMax)
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
				pvThunk2 = LocalAlloc(LPTR, pitem->cchTextMax * sizeof(char));
				pitem->pszText = (LPWSTR)pvThunk2;
				pitem->pszText[0] = TEXT('\0');
			}

			break;
		}
#endif

		case HDN_ITEMCHANGINGW:
			pnmhdr->code = HDN_ITEMCHANGINGA;
			bSet = TRUE;
		// fall through

		case HDN_ITEMCHANGEDW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMCHANGEDA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ITEMCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ITEMDBLCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMDBLCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_DIVIDERDBLCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_DIVIDERDBLCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_BEGINTRACKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_BEGINTRACKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ENDTRACKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ENDTRACKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_TRACKW:
		{
			HD_ITEMW *pitem;

			if (!bSet)
			{
				pnmhdr->code = HDN_TRACKA;
			}

			pitem = ((HD_NOTIFY *)pnmhdr)->pitem;

			if (!IsFlagPtr(pitem) && (pitem->mask & HDI_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk1);
			}


			if (!IsFlagPtr(pitem) && (pitem->mask & HDI_FILTER) && pitem->pvFilter)
			{
				if (!(pitem->type & HDFT_HASNOVALUE) &&
					((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
				{
					LPHD_TEXTFILTER ptextFilter = (LPHD_TEXTFILTER)pitem->pvFilter;
					pvThunk2 = ptextFilter->pszText;
					dwThunkSize = ptextFilter->cchTextMax;
					ptextFilter->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk2);
				}
			}


			break;
		}

		case CBEN_ENDEDITW:
		{
			LPNMCBEENDEDITW peew = (LPNMCBEENDEDITW) pnmhdr;
			LPNMCBEENDEDITA peea = (LPNMCBEENDEDITA)LocalAlloc(LPTR, sizeof(NMCBEENDEDITA));

			if (!peea)
			{
				return 0;
			}

			peea->hdr  = peew->hdr;
			peea->hdr.code = CBEN_ENDEDITA;

			peea->fChanged = peew->fChanged;
			peea->iNewSelection = peew->iNewSelection;
			peea->iWhy = peew->iWhy;
			ConvertWToAN(pci->uiCodePage, peea->szText, ARRAYSIZE(peea->szText),
				peew->szText, -1);

			pvThunk1 = pnmhdr;
			pnmhdr = &peea->hdr;
			_ASSERT((LPVOID)pnmhdr == (LPVOID)peea);
			break;
		}

		case CBEN_DRAGBEGINW:
		{
			LPNMCBEDRAGBEGINW pdbw = (LPNMCBEDRAGBEGINW) pnmhdr;
			LPNMCBEDRAGBEGINA pdba = (LPNMCBEDRAGBEGINA)LocalAlloc(LPTR, sizeof(NMCBEDRAGBEGINA));

			if (!pdba)
			{
				return 0;
			}

			pdba->hdr  = pdbw->hdr;
			pdba->hdr.code = CBEN_DRAGBEGINA;
			pdba->iItemid = pdbw->iItemid;
			ConvertWToAN(pci->uiCodePage, pdba->szText, ARRAYSIZE(pdba->szText),
				pdbw->szText, -1);

			pvThunk1 = pnmhdr;
			pnmhdr = &pdba->hdr;
			_ASSERT((LPVOID)pnmhdr == (LPVOID)pdba);
			break;
		}


		case CBEN_GETDISPINFOW:
		{
			PNMCOMBOBOXEXW pnmcbe = (PNMCOMBOBOXEXW)pnmhdr;

			pnmhdr->code = CBEN_GETDISPINFOA;

			if (pnmcbe->ceItem.mask  & CBEIF_TEXT
				&& !IsFlagPtr(pnmcbe->ceItem.pszText) && pnmcbe->ceItem.cchTextMax)
			{
				pvThunk1 = pnmcbe->ceItem.pszText;
				dwThunkSize = pnmcbe->ceItem.cchTextMax;
				pvThunk2 = LocalAlloc(LPTR, pnmcbe->ceItem.cchTextMax * sizeof(char));
				pnmcbe->ceItem.pszText = (LPWSTR)pvThunk2;
				pnmcbe->ceItem.pszText[0] = TEXT('\0');
			}

			break;
		}

		case HDN_GETDISPINFOW:
		{
			LPNMHDDISPINFOW pHDDispInfoW;

			pnmhdr->code = HDN_GETDISPINFOA;

			pHDDispInfoW = (LPNMHDDISPINFOW) pnmhdr;

			pvThunk1 = pHDDispInfoW->pszText;
			dwThunkSize = pHDDispInfoW->cchTextMax;
			pHDDispInfoW->pszText = (LPWSTR)LocalAlloc(LPTR, pHDDispInfoW->cchTextMax * sizeof(char));

			if (!pHDDispInfoW->pszText)
			{
				pHDDispInfoW->pszText = (LPWSTR) pvThunk1;
				break;
			}

			WideCharToMultiByte(pci->uiCodePage, 0, (LPWSTR)pvThunk1, -1,
				(LPSTR)pHDDispInfoW->pszText, pHDDispInfoW->cchTextMax,
				NULL, NULL);
			break;
		}


		case TBN_GETBUTTONINFOW:
		{
			LPTBNOTIFYW pTBNW;

			pnmhdr->code = TBN_GETBUTTONINFOA;

			pTBNW = (LPTBNOTIFYW)pnmhdr;

			pvThunk1 = pTBNW->pszText;
			dwThunkSize = pTBNW->cchText;
			pvThunk2 = LocalAlloc(LPTR, pTBNW->cchText * sizeof(char));

			if (!pvThunk2)
			{
				break;
			}
			pTBNW->pszText = (LPWSTR)pvThunk2;

			WideCharToMultiByte(pci->uiCodePage, 0, (LPWSTR)pvThunk1, -1,
				(LPSTR)pTBNW->pszText, pTBNW->cchText,
				NULL, NULL);

		}
		break;

		case TTN_NEEDTEXTW:
		{
			LPTOOLTIPTEXTA lpTTTA;
			LPTOOLTIPTEXTW lpTTTW = (LPTOOLTIPTEXTW) pnmhdr;

			lpTTTA = (LPTOOLTIPTEXTA)LocalAlloc(LPTR, sizeof(TOOLTIPTEXTA));

			if (!lpTTTA)
			{
				return 0;
			}

			lpTTTA->hdr = lpTTTW->hdr;
			lpTTTA->hdr.code = TTN_NEEDTEXTA;

			lpTTTA->lpszText = lpTTTA->szText;
			lpTTTA->hinst    = lpTTTW->hinst;
			lpTTTA->uFlags   = lpTTTW->uFlags;
			lpTTTA->lParam   = lpTTTW->lParam;

			WideCharToMultiByte(pci->uiCodePage, 0, lpTTTW->szText, -1, lpTTTA->szText, ARRAYSIZE(lpTTTA->szText), NULL, NULL);
			pvThunk1 = pnmhdr;
			pnmhdr = (NMHDR *)lpTTTA;
		}
		break;

		case DTN_USERSTRINGW:
		{
			LPNMDATETIMESTRINGW lpDateTimeString = (LPNMDATETIMESTRINGW) pnmhdr;

			pnmhdr->code = DTN_USERSTRINGA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeString->pszUserString);
			lpDateTimeString->pszUserString = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_WMKEYDOWNW:
		{
			LPNMDATETIMEWMKEYDOWNW lpDateTimeWMKeyDown =
				(LPNMDATETIMEWMKEYDOWNW) pnmhdr;

			pnmhdr->code = DTN_WMKEYDOWNA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeWMKeyDown->pszFormat);
			lpDateTimeWMKeyDown->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_FORMATQUERYW:
		{
			LPNMDATETIMEFORMATQUERYW lpDateTimeFormatQuery =
				(LPNMDATETIMEFORMATQUERYW) pnmhdr;

			pnmhdr->code = DTN_FORMATQUERYA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeFormatQuery->pszFormat);
			lpDateTimeFormatQuery->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_FORMATW:
		{
			LPNMDATETIMEFORMATW lpDateTimeFormat =
				(LPNMDATETIMEFORMATW) pnmhdr;

			pnmhdr->code = DTN_FORMATA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeFormat->pszFormat);
			lpDateTimeFormat->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		default:
			fThunked = FALSE;
			break;
		}

#ifdef NEED_WOWGETNOTIFYSIZE_HELPER
		_ASSERT(code >= 0 || WOWGetNotifySize(code));
#endif // NEED_WOWGETNOTIFYSIZE_HELPER

		RoutedEventArgs arg;
		arg.hSender = pci->hwnd;
		arg.hOriginalSender = pci->hwnd;
		arg.RoutedEvent = code;
		arg.eRoutingStrategy = RS_BUBBLE;
		lRet = RaiseEvent(pci->hwnd, &arg);
		//L lRet = SendMessage(hwndParent, WM_NOTIFY, (WPARAM)id, (LPARAM)pnmhdr);

		/*
		 * All the thunking for Notify Messages happens here
		 */
		if (fThunked)
		{
			switch (pnmhdr->code)
			{
			case LVN_ODFINDITEMA:
			case LVN_INCREMENTALSEARCHA:
			{
				LV_FINDINFO *plvfi = &((PNM_FINDITEM)pnmhdr)->lvfi;
				if (pvThunk1)
				{
					FreeProducedString((LPWSTR)plvfi->psz);
					plvfi->psz = (LPCWSTR)pvThunk1;
				}
			}
			break;

			case LVN_GETDISPINFOA:
			{
				LV_ITEMA *pitem = &(((LV_DISPINFOA *)pnmhdr)->item);

				// BUGBUG what if pointer is to large buffer?
				if (!IsFlagPtr(pitem) && (pitem->mask & LVIF_TEXT) && !IsFlagPtr(pitem->pszText))
				{
					StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pitem->pszText);
				}
			}
			break;

			case LVN_ENDLABELEDITA:
			case LVN_BEGINLABELEDITA:
			case LVN_SETDISPINFOA:
			case LVN_GETEMPTYTEXTA:
			{
				LV_ITEMA *pitem = &(((LV_DISPINFOA *)pnmhdr)->item);
				InOutAtoW(pci, &ts, &pitem->pszText);
			}
			break;

			case LVN_GETINFOTIPA:
			{
				NMLVGETINFOTIPA *pgit = (NMLVGETINFOTIPA *)pnmhdr;
				InOutAtoW(pci, &ts, &pgit->pszText);
			}
			break;

			case TVN_GETINFOTIPA:
			{
				NMTVGETINFOTIPA *pgit = (NMTVGETINFOTIPA *)pnmhdr;
				StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pgit->pszText);
			}
			break;

			case TBN_GETINFOTIPA:
			{
				NMTBGETINFOTIPA *pgit = (NMTBGETINFOTIPA *)pnmhdr;
				StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pgit->pszText);
			}
			break;
			case TVN_SELCHANGINGA:
			case TVN_SELCHANGEDA:
			case TVN_DELETEITEMA:
			{
				LPTV_ITEMW pitem;

				if (!IsFlagPtr(pvThunk2))
				{
					pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemOld);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPWSTR)pvThunk2;
				}

				// if this is delitem, then we are done
				if (code == TVN_DELETEITEM)
				{
					break;
				}

				/* FALL THROUGH TO TVN_ITEMEXPANDING to unthunk itemNew */
			}
			// fall through

			case TVN_ITEMEXPANDINGA:
			case TVN_ITEMEXPANDEDA:
			case TVN_BEGINDRAGA:
			case TVN_BEGINRDRAGA:
			{
				/* these msgs have a NM_TREEVIEW with itemNew TV_ITEM filled in */
				LPTV_ITEMW pitem;

				if (!IsFlagPtr(pvThunk1))
				{
					pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemNew);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPWSTR)pvThunk1;
				}

				break;
			}

			case TVN_SETDISPINFOA:
			case TVN_BEGINLABELEDITA:
			case TVN_ENDLABELEDITA:
			{
				LPTV_ITEMA pitem;
				pitem = &(((TV_DISPINFOA *)pnmhdr)->item);
				InOutAtoW(pci, &ts, &pitem->pszText);
			}
			break;

			case TVN_GETDISPINFOA:
			{
				/*
				 * This message has a TV_DISPINFO in lParam that wass filled in
				 * during the callback and needs to be unthunked.
				 */
				LPTV_ITEMW pitem;

				pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

				if (!IsFlagPtr(pvThunk1) && (pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
				{
					ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)pitem->pszText, -1);
					pitem->pszText = (LPWSTR)pvThunk1;
					LocalFree(pvThunk2);
				}

				break;
			}

			case HDN_ITEMCHANGINGA:
			case HDN_ITEMCHANGEDA:
			case HDN_ITEMCLICKA:
			case HDN_ITEMDBLCLICKA:
			case HDN_DIVIDERDBLCLICKA:
			case HDN_BEGINTRACKA:
			case HDN_ENDTRACKA:
			case HDN_TRACKA:
			{
				HD_ITEMW *pitem;

				pitem = ((HD_NOTIFY *)pnmhdr)->pitem;

				if (!IsFlagPtr(pitem) && (pitem->mask & HDI_TEXT) && !IsFlagPtr(pvThunk1))
				{
					ConvertAToWN(pci->uiCodePage, (LPTSTR)pvThunk1, dwThunkSize, (LPSTR)(pitem->pszText), -1);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPTSTR)pvThunk1;
				}

				if (!IsFlagPtr(pitem) && (pitem->mask & HDI_FILTER) && pitem->pvFilter && pvThunk2)
				{
					if (!(pitem->type & HDFT_HASNOVALUE) &&
						((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
					{
						LPHD_TEXTFILTER ptextFilter = (LPHD_TEXTFILTER)pitem->pvFilter;
						ConvertAToWN(pci->uiCodePage, (LPTSTR)pvThunk2, dwThunkSize, (LPSTR)(ptextFilter->pszText), -1);
						FreeProducedString(ptextFilter->pszText);
						ptextFilter->pszText = (LPTSTR)pvThunk2;
					}
				}

				break;
			}

			case CBEN_ENDEDITA:
			{
				LPNMCBEENDEDITW peew = (LPNMCBEENDEDITW) pvThunk1;
				LPNMCBEENDEDITA peea = (LPNMCBEENDEDITA) pnmhdr;

				// Don't unthunk the string since that destroys unicode round-trip
				// and the client shouldn't be modifying it anyway.
				// ConvertAToWN(pci->uiCodePage, peew->szText, ARRAYSIZE(peew->szText),
				//              peea->szText, -1);
				LocalFree(peea);
			}
			break;

			case CBEN_DRAGBEGINA:
			{
				LPNMCBEDRAGBEGINW pdbw = (LPNMCBEDRAGBEGINW) pvThunk1;
				LPNMCBEDRAGBEGINA pdba = (LPNMCBEDRAGBEGINA) pnmhdr;

				// Don't unthunk the string since that destroys unicode round-trip
				// and the client shouldn't be modifying it anyway.
				// ConvertAToWN(pci->uiCodePage, pdbw->szText, ARRAYSIZE(pdbw->szText),
				//              pdba->szText, -1);
				LocalFree(pdba);
			}
			break;

			case CBEN_GETDISPINFOA:
			{
				PNMCOMBOBOXEXW pnmcbeW;

				pnmcbeW = (PNMCOMBOBOXEXW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPTSTR)pvThunk1, dwThunkSize, (LPSTR)(pnmcbeW->ceItem.pszText), -1);

				if (pvThunk2)
				{
					LocalFree(pvThunk2);
				}
				pnmcbeW->ceItem.pszText = (LPTSTR)pvThunk1;

			}
			break;


			case HDN_GETDISPINFOA:
			{
				LPNMHDDISPINFOW pHDDispInfoW;

				pHDDispInfoW = (LPNMHDDISPINFOW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPTSTR)pvThunk1, dwThunkSize, (LPSTR)(pHDDispInfoW->pszText), -1);

				LocalFree(pHDDispInfoW->pszText);
				pHDDispInfoW->pszText = (LPTSTR)pvThunk1;

			}
			break;

			case TBN_GETBUTTONINFOA:
			{
				LPTBNOTIFYW pTBNW;

				pTBNW = (LPTBNOTIFYW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPTSTR)pvThunk1, dwThunkSize, (LPSTR)(pTBNW->pszText), -1);

				pTBNW->pszText = (LPTSTR)pvThunk1;
				LocalFree(pvThunk2);

			}
			break;


			case TTN_NEEDTEXTA:
			{
				LPTOOLTIPTEXTA lpTTTA = (LPTOOLTIPTEXTA) pnmhdr;
				LPTOOLTIPTEXTW lpTTTW = (LPTOOLTIPTEXTW) pvThunk1;

				ThunkToolTipTextAtoW(lpTTTA, lpTTTW, pci->uiCodePage);
				LocalFree(lpTTTA);
			}
			break;

			case DTN_USERSTRINGA:
			case DTN_WMKEYDOWNA:
			case DTN_FORMATQUERYA:
			{
				FreeProducedString(pvThunk1);
			}
			break;

			case DTN_FORMATA:
			{
				LPNMDATETIMEFORMATA lpDateTimeFormat = (LPNMDATETIMEFORMATA) pnmhdr;

				FreeProducedString(pvThunk1);

				//
				// pszDisplay and szDisplay are special cases.
				//

				if (lpDateTimeFormat->pszDisplay && *lpDateTimeFormat->pszDisplay)
				{

					//
					// if pszDisplay still points at szDisplay then thunk
					// in place.  Otherwise allocate memory and copy the
					// display string.  This buffer will be freeded in monthcal.c
					//

					if (lpDateTimeFormat->pszDisplay == lpDateTimeFormat->szDisplay)
					{
						CHAR szDisplay[64];

						lstrcpynA(szDisplay, lpDateTimeFormat->szDisplay, 64);

						ConvertAToWN(pci->uiCodePage, (LPWSTR)lpDateTimeFormat->szDisplay, 64,
							szDisplay, -1);
					}
					else
					{
						lpDateTimeFormat->pszDisplay =
							(LPSTR) ProduceWFromA(pci->uiCodePage, lpDateTimeFormat->pszDisplay);
					}

				}

			}
			break;

			default:
				/* No thunking needed */
				break;
			}
		}
		return lRet;
	}
	else
#endif
	{
		RoutedEventArgs arg;
		arg.hSender = pci->hwnd;
		arg.hOriginalSender = pci->hwnd;
		arg.RoutedEvent = WM_NOTIFY;
		arg.eRoutingStrategy = RS_BUBBLE;
		pnmhdr->code = code;
		arg.lParam = (LPARAM)pnmhdr;
		return RaiseEvent(arg.hOriginalSender, &arg);

		//L return (SendMessage(hwndParent, WM_NOTIFY, (WPARAM)id, (LPARAM)pnmhdr));
	}
#undef pvThunk1
#undef pvThunk2
#undef dwThunkSize
}

/*
 * Creates a buffer for a unicode string, and then copies the ANSI text
 * into it (converting it to unicode in the process)
 *
 * The returned pointer should be freed with LocalFree after use.
 */
LPWSTR IUI::ProduceWFromA(UINT uiCodePage, LPCSTR psz)
{
	LPWSTR pszW;
	int cch;

	if (psz == NULL || psz == LPSTR_TEXTCALLBACKA)
	{
		return (LPWSTR)psz;
	}

	// The old code would call lstrlen and lstrcpy which would fault internal to the
	// api, this should do about the same...
	if (IsBadReadPtr(psz, 1))
	{
		return NULL;    // For now lets try not setting a string...
	}

	cch = MultiByteToWideChar(uiCodePage, 0, psz, -1, NULL, 0);

	if (cch == 0)
	{
		cch = 1;
	}

	pszW = (LPWSTR)LocalAlloc(LMEM_FIXED, cch * sizeof(WCHAR));

	if (pszW != NULL)
	{
		if (MultiByteToWideChar(uiCodePage, MB_PRECOMPOSED, psz, -1, pszW,
				cch) == FALSE)
		{
			LocalFree(pszW);
			pszW = NULL;
		}
	}

	return pszW;

}

#define TTF_MEMALLOCED          0x0200

//*************************************************************
//
//  ThunkToolTipTextAtoW()
//
//  Purpose:    Thunks a TOOLTIPTEXTA structure to a TOOLTIPTEXTW
//              structure.
//
//  Return:     (BOOL) TRUE if successful
//                     FALSE if an error occurs
//
//*************************************************************

BOOL IUI::ThunkToolTipTextAtoW(LPTOOLTIPTEXTA lpTttA, LPTOOLTIPTEXTW lpTttW, UINT uiCodePage)
{
	int iResult;


	if (!lpTttA || !lpTttW)
	{
		return FALSE;
	}

	//
	// Thunk the NMHDR structure.
	//
	lpTttW->hdr.hwndFrom = lpTttA->hdr.hwndFrom;
	lpTttW->hdr.idFrom   = lpTttA->hdr.idFrom;
	lpTttW->hdr.code     = TTN_NEEDTEXTW;

	lpTttW->hinst  = lpTttA->hinst;
	lpTttW->uFlags = lpTttA->uFlags;
	lpTttW->lParam = lpTttA->lParam;

	//
	// Thunk the string to the new structure.
	// Special case LPSTR_TEXTCALLBACK.
	//

	if (lpTttA->lpszText == LPSTR_TEXTCALLBACKA)
	{
		lpTttW->lpszText = LPSTR_TEXTCALLBACKW;

	}
	else if (!IS_INTRESOURCE(lpTttA->lpszText))
	{

		//
		//  Transfer the lpszText into the lpTttW...
		//
		//  First see if it fits into the buffer, and optimistically assume
		//  it will.
		//
		lpTttW->lpszText = lpTttW->szText;
		iResult = MultiByteToWideChar(uiCodePage, 0, lpTttA->lpszText, -1,
				lpTttW->szText, ARRAYSIZE(lpTttW->szText));
		if (!iResult)
		{
			//
			//  Didn't fit into the small buffer; must alloc our own.
			//
			lpTttW->lpszText = ProduceWFromA(uiCodePage, lpTttA->lpszText);
			lpTttW->uFlags |= TTF_MEMALLOCED;
		}

	}
	else
	{
		lpTttW->lpszText = (LPWSTR)lpTttA->lpszText;
	}

	return TRUE;
}

void IUI::InOutAtoW(IUI_CONTROL_INFO *pci, THUNKSTATE *pts, LPSTR *ppsz)
{
	if (!IsFlagPtr(pts->ts_pvThunk1))
	{
		if (!IsFlagPtr(*ppsz) &&
			lstrcmpA((LPCSTR)pts->ts_pvThunk2, (LPSTR)*ppsz) != 0)
		{
			StringBufferAtoW(pci->uiCodePage, pts->ts_pvThunk1, pts->ts_dwThunkSize, ppsz);
		}
		LocalFree(pts->ts_pvThunk2);
	}
	*ppsz = (LPSTR)pts->ts_pvThunk1;
}

void IUI::IUI_InOutAtoW(IUICONTROLINFO *pci, THUNKSTATE *pts, LPSTR *ppsz)
{
	if (!IsFlagPtr(pts->ts_pvThunk1))
	{
		if (!IsFlagPtr(*ppsz) &&
			lstrcmpA((LPCSTR)pts->ts_pvThunk2, (LPSTR)*ppsz) != 0)
		{
			StringBufferAtoW(pci->uiCodePage, pts->ts_pvThunk1, pts->ts_dwThunkSize, ppsz);
		}
		LocalFree(pts->ts_pvThunk2);
	}
	*ppsz = (LPSTR)pts->ts_pvThunk1;
}


void IUI::StringBufferAtoW(UINT uiCodePage, LPVOID pvOrgPtr, DWORD dwOrgSize, CHAR **ppszText)
{
	if (pvOrgPtr == *ppszText)
	{
		// the pointer has not been changed by the callback...
		// must convert from A to W in-place

		if (dwOrgSize)
		{
			LPWSTR pszW = ProduceWFromA(uiCodePage, *ppszText);
			if (pszW)
			{
				lstrcpynW((WCHAR *)(*ppszText), pszW, dwOrgSize);   // this becomes a W buffer
				FreeProducedString(pszW);
			}
		}
	}
	else
	{
		// the pointer has been changed out from underneath us, copy
		// unicode back into the original buffer.

		ConvertAToWN(uiCodePage, (LPWSTR)pvOrgPtr, dwOrgSize, *ppszText, -1);
		*ppszText = (CHAR *)pvOrgPtr;
	}
}

//
//  InOutWtoA/InOutAtoW is for thunking INOUT string parameters.
//
//  INOUT parameters suck.
//
// We need to save both the original ANSI and the
// original UNICODE strings, so that if the app doesn't
// change the ANSI string, we leave the original UNICODE
// string alone.  That way, UNICODE item names don't get
// obliterated by the thunk.
//
// The original buffer is saved in pvThunk1.
// We allocate two ANSI buffers.
// pvThunk2 contains the original ANSIfied string.
// pvThunk2+cchTextMax is the buffer we pass to the app.
// On the way back, we compare pvThunk2 with pvThunk2+cchTextMax.
// If they are different, then we unthunk the string; otherwise,
// we leave the original UNICODE buffer alone.

BOOL IUI::InOutWtoA(IUI_CONTROL_INFO *pci, THUNKSTATE *pts, LPWSTR *ppsz, DWORD cchTextMax)
{
	pts->ts_pvThunk1 = *ppsz;               // Save original buffer
	pts->ts_dwThunkSize = cchTextMax;

	if (!IsFlagPtr(pts->ts_pvThunk1))
	{
		pts->ts_pvThunk2 = LocalAlloc(LPTR, cchTextMax * 2 * sizeof(char));
		if (!ConvertWToAN(pci->uiCodePage, (LPSTR)pts->ts_pvThunk2, pts->ts_dwThunkSize, (LPWSTR)pts->ts_pvThunk1, -1))
		{
			LocalFree(pts->ts_pvThunk2);
			return 0;
		}
		*ppsz = (LPWSTR)((LPSTR)pts->ts_pvThunk2 + cchTextMax);
		StringCchCopyA((LPSTR)*ppsz, cchTextMax, (LPCSTR)pts->ts_pvThunk2);
	}
	return TRUE;
}

//
//  InOutWtoA/InOutAtoW is for thunking INOUT string parameters.
//
//  INOUT parameters suck.
//
// We need to save both the original ANSI and the
// original UNICODE strings, so that if the app doesn't
// change the ANSI string, we leave the original UNICODE
// string alone.  That way, UNICODE item names don't get
// obliterated by the thunk.
//
// The original buffer is saved in pvThunk1.
// We allocate two ANSI buffers.
// pvThunk2 contains the original ANSIfied string.
// pvThunk2+cchTextMax is the buffer we pass to the app.
// On the way back, we compare pvThunk2 with pvThunk2+cchTextMax.
// If they are different, then we unthunk the string; otherwise,
// we leave the original UNICODE buffer alone.

BOOL IUI::IUI_InOutWtoA(IUICONTROLINFO *pci, THUNKSTATE *pts, LPWSTR *ppsz, DWORD cchTextMax)
{
	pts->ts_pvThunk1 = *ppsz;               // Save original buffer
	pts->ts_dwThunkSize = cchTextMax;

	if (!IsFlagPtr(pts->ts_pvThunk1))
	{
		pts->ts_pvThunk2 = LocalAlloc(LPTR, cchTextMax * 2 * sizeof(char));
		if (!ConvertWToAN(pci->uiCodePage, (LPSTR)pts->ts_pvThunk2, pts->ts_dwThunkSize, (LPWSTR)pts->ts_pvThunk1, -1))
		{
			LocalFree(pts->ts_pvThunk2);
			return 0;
		}
		*ppsz = (LPWSTR)((LPSTR)pts->ts_pvThunk2 + cchTextMax);
		StringCchCopyA((LPSTR)*ppsz, cchTextMax, (LPCSTR)pts->ts_pvThunk2);
	}
	return TRUE;
}

/*
 * Creates a buffer for a unicode string, and then copies the ANSI text
 * into it (converting it to unicode in the process)
 *
 * The returned pointer should be freed with LocalFree after use.
 */
LPSTR IUI::ProduceAFromW(UINT uiCodePage, LPCWSTR psz)
{
	LPSTR pszA;
	int cch;

	if (psz == NULL || psz == LPSTR_TEXTCALLBACKW)
	{
		return (LPSTR)psz;
	}

	cch = WideCharToMultiByte(uiCodePage, 0, psz, -1, NULL, 0, NULL, NULL);

	if (cch == 0)
	{
		cch = 1;
	}

	pszA = (LPSTR)LocalAlloc(LMEM_FIXED, cch * sizeof(char));

	if (pszA != NULL)
	{
		if (WideCharToMultiByte(uiCodePage, 0, psz, -1, pszA, cch, NULL, NULL) ==
			FALSE)
		{
			LocalFree(pszA);
			pszA = NULL;
		}
	}

	return pszA;

}

// --------------------------------------------------------------------------
//
//  MyNotifyWinEvent()
//
//  This tries to get the proc address of NotifyWinEvent().  If it fails, we
//  remember that and do nothing.
//
//  NOTE TO NT FOLKS:
//  Don't freak out about this code.  It will do nothing on NT, nothing yet
//  that is.  Active Accessibility will be ported to NT for Service Pack #1
//  or at worst #2 after NT SUR ships, this code will work magically when
//  that is done/
//
// --------------------------------------------------------------------------
void IUI::MyNotifyWinEvent(UINT event, HWND hwnd, LONG idContainer, LONG_PTR idChild)
{
	static NOTIFYWINEVENTPROC s_pfnNotifyWinEvent = NULL;

	if (!s_pfnNotifyWinEvent)
	{
		HMODULE hmod;

		if (hmod = GetModuleHandle(TEXT("USER32")))
			s_pfnNotifyWinEvent = (NOTIFYWINEVENTPROC)GetProcAddress(hmod,
					"NotifyWinEvent");

		if (!s_pfnNotifyWinEvent)
		{
			s_pfnNotifyWinEvent = DONOTHING_NOTIFYWINEVENT;
		}
	}

	if (s_pfnNotifyWinEvent != DONOTHING_NOTIFYWINEVENT)
	{
		(* s_pfnNotifyWinEvent)(event, hwnd, idContainer, idChild);
	}
}

// MSDN上说，LocalAlloc速度比较慢，建议用heap function
void *IUI::CCLocalReAlloc(void *p, UINT uBytes)
{
	if (uBytes)
	{
		if (p)
		{
			return HeapReAlloc(GetProcessHeap(), 0, p, uBytes); //L LocalReAlloc(p, uBytes, LMEM_MOVEABLE | LMEM_ZEROINIT);
		}
		else
		{
			return HeapAlloc(GetProcessHeap(), 0, uBytes); //L LocalAlloc(LPTR, uBytes);
		}
	}
	else
	{
		if (p)
		{
			HeapFree(GetProcessHeap(), 0, p); //L LocalFree(p);
		}
		return NULL;
	}
}

BOOL IUI::Str_Set(LPTSTR *ppsz, LPCTSTR psz)
{
	if (!psz || (psz == LPSTR_TEXTCALLBACK))
	{
		if (*ppsz)
		{
			if (*ppsz != (LPSTR_TEXTCALLBACK))
			{
				HeapFree(GetProcessHeap(), 0, *ppsz); //L LocalFree(*ppsz);
			}
		}
		*ppsz = (LPTSTR)psz;
	}
	else
	{
		LPTSTR pszNew = *ppsz;
		UINT cbSize = (lstrlen(psz) + 1) * sizeof(TCHAR);

		if (pszNew == LPSTR_TEXTCALLBACK)
		{
			pszNew = NULL;
		}

		pszNew = (LPTSTR)CCLocalReAlloc(pszNew, cbSize);

		if (!pszNew)
		{
			return FALSE;
		}

		StringCbCopy(pszNew, cbSize, psz);
		*ppsz = pszNew;
	}

	return TRUE;
}

/*----------------------------------------------------------
Purpose: This function converts a multi-byte string to a
         wide-char string.

         If pszBuf is non-NULL and the converted string can fit in
         pszBuf, then *ppszWide will point to the given buffer.
         Otherwise, this function will allocate a buffer that can
         hold the converted string.

         If pszAnsi is NULL, then *ppszWide will be freed.  Note
         that pszBuf must be the same pointer between the call
         that converted the string and the call that frees the
         string.

Returns: TRUE
         FALSE (if out of memory)

*/
BOOL UnicodeFromAnsi(
	LPWSTR *ppwszWide,
	LPCSTR pszAnsi,           // NULL to clean up
	LPWSTR pwszBuf,
	int cchBuf)
{
	BOOL bRet;

	// Convert the string?
	if (pszAnsi)
	{
		// Yes; determine the converted string length
		int cch;
		LPWSTR pwsz;
		int cchAnsi = lstrlenA(pszAnsi) + 1;

		cch = MultiByteToWideChar(CP_ACP, 0, pszAnsi, cchAnsi, NULL, 0);

		// String too big, or is there no buffer?
		if (cch > cchBuf || NULL == pwszBuf)
		{
			// Yes; allocate space
			cchBuf = cch + 1;
			pwsz = (LPWSTR)LocalAlloc(LPTR, CbFromCchW(cchBuf));
		}
		else
		{
			// No; use the provided buffer
			pwsz = pwszBuf;
		}

		if (pwsz)
		{
			// Convert the string
			cch = MultiByteToWideChar(CP_ACP, 0, pszAnsi, cchAnsi, pwsz, cchBuf);
			bRet = (0 < cch);
		}
		else
		{
			bRet = FALSE;
		}

		*ppwszWide = pwsz;
	}
	else
	{
		// No; was this buffer allocated?
		if (*ppwszWide && pwszBuf != *ppwszWide)
		{
			// Yes; clean up
			LocalFree((HLOCAL)*ppwszWide);
			*ppwszWide = NULL;
		}
		bRet = TRUE;
	}

	return bRet;
}

BOOL CDECL IUI::CcshellRipMsgW(BOOL f, LPCSTR pszMsg, ...)         // (this is deliberately CHAR)
{
	WCHAR ach[1024 + 40];
	va_list vArgs;

	if (!f)
	{
		LPWSTR pwsz;
		WCHAR wszBuf[128];
		OutputDebugString(_T("c_szRipMsg"));

		// (We convert the string, rather than simply input an
		// LPCWSTR parameter, so the caller doesn't have to wrap
		// all the string constants with the TEXT() macro.)

		ach[0] = L'\0';     // In case this fails
		if (UnicodeFromAnsi(&pwsz, pszMsg, wszBuf, SIZECHARS(wszBuf)))
		{
			va_start(vArgs, pszMsg);
			StringCchVPrintfW(ach, 1024 + 40, pwsz, vArgs);
			va_end(vArgs);
			UnicodeFromAnsi(&pwsz, NULL, wszBuf, 0);
		}

		OutputDebugStringW(ach);
		OutputDebugStringA("c_szNewline");


		//L if (_IsShellProcess() || IsFlagSet(g_dwBreakFlags, BF_RIP))
		//{
		//	// MSDEV USERS:  This is not the real assert.  Hit
		//	//               Shift-F11 to jump back to the caller.
		//	DEBUG_BREAK;                                                            // _ASSERT
		//}
	}
	return FALSE;
}

/***************************************************************************\
* MirrorIcon
*
* Mirror an Icon , given an Icon handle so that when these icons are displayed
* on a Mirrored DC, they end would be displayed normal.
*
* History:
* 04-Feb-1998 samera    Created
\***************************************************************************/
BOOL IUI::MirrorIcon(HICON *phiconSmall, HICON *phiconLarge)
{
	//L	HDC      hdcScreen;
	//	HBITMAP  hbm, hbmMask, hbmOld, hbmOldMask;
	//	BITMAP   bm;
	//	HICON    hicon[2] = {NULL, NULL};
	//	HICON    hiconNew[2] = {NULL, NULL};
	//	ICONINFO ii ;
	//	int      i;
	//#ifdef WINNT
	//#define      IPIXELOFFSET 0
	//#else // !WINNT
	//#define      IPIXELOFFSET 2
	//#endif WINNT
	//
	//	//
	//	// Synchronize access to global DCs now!
	//	// Allocate DCs if we didn't so far.
	//	//
	//	ENTERCRITICAL;
	//	HDC g_hdc = NULL;
	//	HDC g_hdcMask = NULL;
	//	if (!g_hdc && !g_hdcMask)
	//	{
	//		g_hdc = CreateCompatibleDC(NULL);
	//		if (g_hdc)
	//		{
	//#ifndef UNIX
	//#ifndef WINNT
	//			SetObjectOwner(g_hdc, NULL);
	//#endif
	//#endif /* !UNIX */
	//			g_hdcMask = CreateCompatibleDC(NULL);
	//
	//			if (g_hdcMask)
	//			{
	//#ifndef UNIX
	//#ifndef WINNT
	//				SetObjectOwner(g_hdcMask, HINST_THISDLL);
	//#endif
	//#endif /* !UNIX */
	//
	//				// L				SET_DC_RTL_MIRRORED(g_hdc);
	//				// 				SET_DC_RTL_MIRRORED(g_hdcMask);
	//			}
	//			else
	//			{
	//				DeleteDC(g_hdc);
	//				g_hdc = NULL;
	//			}
	//		}
	//	}
	//
	//	if (phiconSmall)
	//	{
	//		hicon[0] = *phiconSmall;
	//	}
	//
	//	if (phiconLarge)
	//	{
	//		hicon[1] = *phiconLarge;
	//	}
	//
	//	//
	//	// Acquire the screen DC
	//	//
	//	hdcScreen = GetDC(NULL);
	//
	//	if (g_hdc && g_hdcMask && hdcScreen)
	//	{
	//		for (i = 0 ; i < (sizeof(hicon) / sizeof(HICON)) ; i++)
	//		{
	//			if (hicon[i])
	//			{
	//				if (GetIconInfo(hicon[i], &ii) &&
	//					GetObject(ii.hbmColor, sizeof(BITMAP), &bm))
	//				{
	//					//
	//					// I don't want these.
	//					//
	//					DeleteObject(ii.hbmMask);
	//					DeleteObject(ii.hbmColor);
	//					ii.hbmMask = ii.hbmColor = NULL;
	//
	//					hbm = CreateCompatibleBitmap(hdcScreen, bm.bmWidth, bm.bmHeight);
	//					hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	//					hbmOld = (HBITMAP)SelectObject(g_hdc, hbm);
	//					hbmOldMask = (HBITMAP)SelectObject(g_hdcMask, hbmMask);
	//
	//					DrawIconEx(g_hdc, IPIXELOFFSET, 0, hicon[i], bm.bmWidth, bm.bmHeight, 0,
	//						NULL, DI_IMAGE);
	//
	//					DrawIconEx(g_hdcMask, IPIXELOFFSET, 0, hicon[i], bm.bmWidth, bm.bmHeight, 0,
	//						NULL, DI_MASK);
	//
	//					SelectObject(g_hdc, hbmOld);
	//					SelectObject(g_hdcMask, hbmOldMask);
	//
	//					//
	//					// create the new mirrored icon, and delete bmps
	//					//
	//					ii.hbmMask  = hbmMask;
	//					ii.hbmColor = hbm;
	//					hiconNew[i] = CreateIconIndirect(&ii);
	//
	//					DeleteObject(hbm);
	//					DeleteObject(hbmMask);
	//				}
	//			}
	//		}
	//	}
	//
	//	ReleaseDC(NULL, hdcScreen);
	//
	//	//
	//	// Now we can reuse the global DCs
	//	//
	//	LEAVECRITICAL;
	//
	//	//
	//	// Update icons if needed, and destroy old ones!
	//	//
	//	if (hicon[0] && hiconNew[0])
	//	{
	//		*phiconSmall = hiconNew[0];
	//		DestroyIcon(hicon[0]);
	//	}
	//
	//	if (hicon[1] && hiconNew[1])
	//	{
	//		*phiconLarge = hiconNew[1];
	//
	//		//
	//		// Don't delete twice
	//		//
	//		if (hicon[1] != hicon[0])
	//		{
	//			DestroyIcon(hicon[1]);
	//		}
	//	}

	return TRUE;
}

void IUI::FillRectClr(HDC hdc, LPRECT prc, COLORREF clr)
{
	COLORREF clrSave = ::SetBkColor(hdc, clr);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, prc, NULL, 0, NULL);
	::SetBkColor(hdc, clrSave);
}

HBITMAP IUI::CreateColorBitmap(int cx, int cy)
{
	HBITMAP hbm;
	HDC hdc;

	hdc = GetDC(NULL);

	//
	// on a multimonitor system with mixed bitdepths
	// always use a 32bit bitmap for our work buffer
	// this will prevent us from losing colors when
	// blting to and from the screen.  this is mainly
	// important for the drag & drop offscreen buffers.
	//
	if (!(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) &&
		GetSystemMetrics(SM_CMONITORS) > 1 &&
		GetSystemMetrics(SM_SAMEDISPLAYFORMAT) == 0)
	{
		LPVOID p;
#ifndef UNIX
		BITMAPINFO bi = {sizeof(BITMAPINFOHEADER), cx, cy, 1, 32};
#else
		BITMAPINFO bi;
		bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth    = cx;
		bi.bmiHeader.biHeight   = cy;
		bi.bmiHeader.biPlanes   = 1 ;
		bi.bmiHeader.biBitCount = 32;
#endif
		hbm = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &p, NULL, 0);
	}
	else
	{
		hbm = CreateCompatibleBitmap(hdc, cx, cy);
	}

	ReleaseDC(NULL, hdc);
	return hbm;
}

STDAPI_(HCURSOR) IUI::LoadHandCursor(DWORD dwRes)
{
	//if (g_bRunOnNT5 || g_bRunOnMemphis)
	//{
	//	HCURSOR hcur = LoadCursor(NULL, IDC_HAND);  // from USER, system supplied
	//	if (hcur)
	//	{
	//		return hcur;
	//	}
	//}

	//L
	return LoadCursor(NULL, IDC_HAND);
}

BOOL PASCAL IUI::ChildOfActiveWindow(HWND hwndChild)
{
	HWND hwnd = hwndChild;
	HWND hwndActive = GetForegroundWindow();

	while (hwnd)
	{
		if (hwnd == hwndActive)
		{
			return TRUE;
		}
		else
		{
			hwnd = ::GetParent(hwnd);
		}
	}
	return FALSE;
}

void IUI::CCPlaySound(LPCTSTR lpszName)
{
	//L TCHAR szFileName[MAX_PATH];
	//LONG cbSize = SIZEOF(szFileName);
	//TCHAR szKey[CCH_KEYMAX];

	//if (g_fNeverPlaySound)
	//{
	//	return;
	//}

	//// check the registry first
	//// if there's nothing registered, we blow off the play,
	//// but we don't set the MM_DONTLOAD flag so taht if they register
	//// something we will play it
	//wsprintf(szKey, TEXT("AppEvents\\Schemes\\Apps\\.Default\\%s\\.current"), lpszName);
	//if ((RegQueryValue(HKEY_CURRENT_USER, szKey, szFileName, &cbSize) == ERROR_SUCCESS) &&
	//	(cbSize > SIZEOF(szFileName[0])))
	//{

	//	PLAYSOUNDFN pfnPlaySound;
	//	UINTVOIDFN pfnwaveOutGetNumDevs;

	//	HANDLE hMM;

	//	hMM = GetModuleHandle(c_szWinMMDll);
	//	if (!hMM)
	//	{
	//		hMM = LoadLibrary(c_szWinMMDll);
	//	}

	//	if (!hMM)
	//	{
	//		return;
	//	}

	//	/// are there any devices?
	//	pfnwaveOutGetNumDevs = (UINTVOIDFN)GetProcAddress(hMM, c_szwaveOutGetNumDevs);
	//	pfnPlaySound = (PLAYSOUNDFN)GetProcAddress(hMM, c_szPlaySound);
	//	if (!pfnPlaySound || !pfnwaveOutGetNumDevs || !pfnwaveOutGetNumDevs())
	//	{
	//		g_fNeverPlaySound = TRUE;
	//		return;
	//	}

	//	pfnPlaySound(szFileName, NULL, SND_FILENAME | SND_ASYNC);
	//}
}

//
//  When we want to turn a tooltip into an infotip, we set its
//  width to 300 "small pixels", where there are 72 small pixels
//  per inch when you are in small fonts mode.
//
//  Scale this value based on the magnification in effect
//  on the owner's monitor.  But never let the tooltip get
//  bigger than 3/4 of the screen.
//
void IUI::CCSetInfoTipWidth(HWND hwndOwner, HWND hwndToolTips)
{
	HDC hdc = GetDC(hwndOwner);
	int iWidth = MulDiv(GetDeviceCaps(hdc, LOGPIXELSX), 300, 72);
	int iMaxWidth = GetDeviceCaps(hdc, HORZRES) * 3 / 4;
	::SendMessage(hwndToolTips, TTM_SETMAXTIPWIDTH, 0, min(iWidth, iMaxWidth));
	ReleaseDC(hwndOwner, hdc);
}

LONG IUI::GetMessagePosClient(HWLWND hWnd, LPPOINT ppt)
{
	LPARAM lParam;
	POINT pt;
	if (!ppt)
	{
		ppt = &pt;
	}

	lParam = GetMessagePos();
	ppt->x = GET_X_LPARAM(lParam);
	ppt->y = GET_Y_LPARAM(lParam);
	ScreenToClient(hWnd, ppt);

	return MAKELONG(ppt->x, ppt->y);
}

// returns TRUE if handled
BOOL IUI::CCWndProc(IUI_CONTROL_INFO *pci, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plres)
{
	if (uMsg >= CCM_FIRST && uMsg < CCM_LAST)
	{
		LRESULT lres = 0;
		switch (uMsg)
		{
		case CCM_SETUNICODEFORMAT:
			lres = pci->bUnicode;
			pci->bUnicode = BOOLFROMPTR(wParam);
			break;

		case CCM_GETUNICODEFORMAT:
			lres = pci->bUnicode;
			break;

		case CCM_SETVERSION:
			if (wParam <= COMCTL32_VERSION)
			{
				lres = pci->iVersion;
				pci->iVersion = (int)wParam;
			}
			else
			{
				lres = -1;
			}
			break;

		case CCM_GETVERSION:
			lres = pci->iVersion;
			break;

		}

		_ASSERT(plres);
		*plres = lres;

		return TRUE;
	}

	return FALSE;
}

// returns TRUE if handled
BOOL IUI::IUI_CCWndProc(IUICONTROLINFO *pci, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plres)
{
	if (uMsg >= CCM_FIRST && uMsg < CCM_LAST)
	{
		LRESULT lres = 0;
		switch (uMsg)
		{
		case CCM_SETUNICODEFORMAT:
			lres = pci->bUnicode;
			pci->bUnicode = BOOLFROMPTR(wParam);
			break;

		case CCM_GETUNICODEFORMAT:
			lres = pci->bUnicode;
			break;

		case CCM_SETVERSION:
			if (wParam <= COMCTL32_VERSION)
			{
				lres = pci->iVersion;
				pci->iVersion = (int)wParam;
			}
			else
			{
				lres = -1;
			}
			break;

		case CCM_GETVERSION:
			lres = pci->iVersion;
			break;

		}

		_ASSERT(plres);
		*plres = lres;

		return TRUE;
	}

	return FALSE;
}

int IUI::GetIncrementSearchString(PISEARCHINFO pis, LPTSTR lpsz)
{
	if (IsISearchTimedOut(pis))
	{
		pis->iIncrSearchFailed = 0;
		pis->ichCharBuf = 0;
	}

	if (pis->ichCharBuf && lpsz)
	{
		lstrcpyn(lpsz, pis->pszCharBuf, pis->ichCharBuf + 1);
		lpsz[pis->ichCharBuf] = TEXT('\0');
	}
	return pis->ichCharBuf;
}

#ifdef UNICODE
/*
 * Thunk for LVM_GETISEARCHSTRINGA
 */
int IUI::GetIncrementSearchStringA(PISEARCHINFO pis, UINT uiCodePage, LPSTR lpsz)
{
	if (IsISearchTimedOut(pis))
	{
		pis->iIncrSearchFailed = 0;
		pis->ichCharBuf = 0;
	}

	if (pis->ichCharBuf && lpsz)
	{
		ConvertWToAN(uiCodePage, lpsz, pis->ichCharBuf, pis->pszCharBuf, pis->ichCharBuf);
		lpsz[pis->ichCharBuf] = '\0';
	}
	return pis->ichCharBuf;
}
#endif

void IUI::RelayToToolTips(HWND hwndToolTips, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (hwndToolTips)
	{
		MSG msg;
		msg.lParam = lParam;
		msg.wParam = wParam;
		msg.message = wMsg;
		msg.hwnd = hWnd;
		::SendMessage(hwndToolTips, TTM_RELAYEVENT, 0, (LPARAM)(LPMSG)&msg);
	}
}

void IUI::InitGlobalMetrics(WPARAM wParam)
{

}

//
//  IncrementSearchString - Add or clear the search string
//
//      ch == 0:  Reset the search string.  Return value meaningless.
//
//      ch != 0:  Append the character to the search string, starting
//                a new search string if we timed out the last one.
//                lplpstr receives the string so far.
//                Return value is TRUE if a new search string was
//                created, or FALSE if we appended to an existing one.
//

BOOL IUI::IncrementSearchString(PISEARCHINFO pis, UINT ch, LPTSTR *lplpstr)
{
	BOOL fRestart = FALSE;

	if (!ch)
	{
		pis->ichCharBuf = 0;
		pis->iIncrSearchFailed = 0;
		return FALSE;
	}

	if (IsISearchTimedOut(pis))
	{
		pis->iIncrSearchFailed = 0;
		pis->ichCharBuf = 0;
	}

	if (pis->ichCharBuf == 0)
	{
		fRestart = TRUE;
	}

	pis->timeLast = GetMessageTime();

	// Is there room for new character plus zero terminator?
	//
	if (pis->ichCharBuf + 1 + 1 > pis->cbCharBuf)
	{
		LPTSTR psz = (LPTSTR)ReAlloc(pis->pszCharBuf, ((pis->cbCharBuf + 16) * sizeof(TCHAR)));
		if (!psz)
		{
			return fRestart;
		}

		pis->cbCharBuf += 16;
		pis->pszCharBuf = psz;
	}

	pis->pszCharBuf[pis->ichCharBuf++] = (TCHAR)ch;
	pis->pszCharBuf[pis->ichCharBuf] = 0;

	*lplpstr = pis->pszCharBuf;

	return fRestart;
}

__inline HANDLE IUI::GetSharedHeapHandle(void)
{
	if (IUIGetGloabelVar()->g_hSharedHeap)
	{
		return IUIGetGloabelVar()->g_hSharedHeap;
	}
	else
	{
		return InitSharedHeap();
	}
}

void *IUI::Alloc(long cb)
{
	// I will assume that this is the only one that needs the checks to
	// see if the heap has been previously created or not
#if defined(WINNT) || defined(MAINWIN)
	return (void *)LocalAlloc(LPTR, cb);
#else
	HANDLE hHeap = GetSharedHeapHandle();

	// If still NULL we have problems!
	if (hHeap == NULL)
	{
		return (NULL);
	}

	return HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cb);
#endif
}

void *IUI::ReAlloc(void *pb, long cb)
{
	if (pb == NULL)
	{
		return Alloc(cb);
	}
#if defined(WINNT) || defined(MAINWIN)
	return (void *)LocalReAlloc((HLOCAL)pb, cb, LMEM_ZEROINIT | LMEM_MOVEABLE);
#else
	return HeapReAlloc(IUIGetGloabelVar()->g_hSharedHeap, HEAP_ZERO_MEMORY, pb, cb);
#endif
}

#ifndef WINNT
HANDLE IUI::InitSharedHeap(void)
{
	//L ENTERCRITICAL;
	if (IUIGetGloabelVar()->g_hSharedHeap == NULL)
	{
		IUIGetGloabelVar()->g_hSharedHeap = HeapCreate(HEAP_SHARED, 1, MAXHEAPSIZE);
	}
	//L LEAVECRITICAL;
	return IUIGetGloabelVar()->g_hSharedHeap;
}
#endif

UINT IUI::RTLSwapLeftRightArrows(IUI_CONTROL_INFO *pci, WPARAM wParam)
{
	if (pci->dwExStyle & RTL_MIRRORED_WINDOW)
	{
		return CCSwapKeys(wParam, VK_LEFT, VK_RIGHT);
	}
	return (UINT)wParam;
}

UINT IUI::CCSwapKeys(WPARAM wParam, UINT vk1, UINT vk2)
{
	if (wParam == vk1)
	{
		return vk2;
	}
	if (wParam == vk2)
	{
		return vk1;
	}
	return (UINT)wParam;
}

// Beep only on the first failure.

void IUI::IncrementSearchBeep(PISEARCHINFO pis)
{
	if (!pis->iIncrSearchFailed)
	{
		pis->iIncrSearchFailed = TRUE;
		MessageBeep(0);
	}
}

//---------------------------------------------------------------------------------------
//
//  Returns previous window bits.

DWORD IUI::SetWindowBits(HWND hWnd, int iWhich, DWORD dwBits, DWORD dwValue)
{
	DWORD dwStyle;
	DWORD dwNewStyle;

	dwStyle = GetWindowLong(hWnd, iWhich);
	dwNewStyle = (dwStyle & ~dwBits) | (dwValue & dwBits);
	if (dwStyle != dwNewStyle)
	{
		dwStyle = SetWindowLong(hWnd, iWhich, dwNewStyle);
	}
	return dwStyle;
}

/*----------------------------------------------------------
Purpose: Release the capture and tell the parent we've done so.

Returns: Whether the control is still alive.
*/
BOOL IUI::CCReleaseCapture(IUI_CONTROL_INFO *pci)
{
	NMHDR nmhdr = {0};

	ReleaseCapture(pci->hwnd);

	// Tell the parent we've released the capture
	CCSendNotify(pci, NM_RELEASEDCAPTURE, &nmhdr);

	return IsWindow(pci->hwnd);
}

// Copy from windows 2000 source code.
// should caller pass in message that indicates termination
// (WM_LBUTTONUP, WM_RBUTTONUP)?
//
// in:
//      hwnd    to do check on
//      x, y    in client coordinates
//
// returns:
//      TRUE    the user began to drag (moved mouse outside double click rect)
//      FALSE   mouse came up inside click rect
//
// BUGBUG, should support VK_ESCAPE to cancel

BOOL PASCAL IUI::CheckForDragBegin(HWLWND hWnd, int x, int y)
{
	RECT rc;
	int dxClickRect = GetSystemMetrics(SM_CXDRAG);
	int dyClickRect = GetSystemMetrics(SM_CYDRAG);

	if (dxClickRect < 4)
	{
		dxClickRect = dyClickRect = 4;
	}

	// See if the user moves a certain number of pixels in any direction

	SetRect(&rc, x - dxClickRect, y - dyClickRect, x + dxClickRect, y + dyClickRect);
	ClientToScreen(hWnd, &rc); // client -> screen

	//
	//  SUBTLE!  We use PeekMessage+WaitMessage instead of GetMessage,
	//  because WaitMessage will return when there is an incoming
	//  SendMessage, whereas GetMessage does not.  This is important,
	//  because the incoming message might've been WM_CAPTURECHANGED.
	//

	SetCapture(hWnd);
	do
	{
		MSG msg32;
		if (PeekMessage(&msg32, NULL, 0, 0, PM_REMOVE))
		{
			// See if the application wants to process the message...
			if (CallMsgFilter(&msg32, MSGF_COMMCTRL_BEGINDRAG) != 0)
			{
				continue;
			}

			switch (msg32.message)
			{
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				ReleaseCapture(hWnd);
				return FALSE;

			case WM_MOUSEMOVE:
				if (::IsWindow(GetHostWnd(hWnd)) && !PtInRect(&rc, msg32.pt))
				{
					ReleaseCapture(hWnd);
					return TRUE;
				}
				break;

			default:
				TranslateMessage(&msg32);
				DispatchMessage(&msg32);
				break;
			}
		}
		else
		{
			WaitMessage();
		}

		// WM_CANCELMODE messages will unset the capture, in that
		// case I want to exit this loop
	}
	while (::IsWindow(GetHostWnd(hWnd)) && GetCapture(NULL) == hWnd);

	return FALSE;
}

HFONT IUI::CCGetHotFont(HFONT hFont, HFONT *phFontHot)
{
	if (!*phFontHot)
	{
		LOGFONT lf;

		// create the underline font
		GetObject(hFont, sizeof(lf), &lf);
#ifndef DONT_UNDERLINE
		lf.lfUnderline = TRUE;
#endif
		*phFontHot = CreateFontIndirect(&lf);
	}
	return *phFontHot;
}

//
// Translate the given font to a code page used for thunking text
//
UINT IUI::GetCodePageForFont(HFONT hFont)
{
#ifdef WINNT
	LOGFONT lf;
	TCHAR szFontName[MAX_PATH];
	CHARSETINFO csi;
	DWORD dwSize, dwType;
	HKEY hKey;


	if (!GetObject(hFont, sizeof(lf), &lf))
	{
		return CP_ACP;
	}


	//
	// Check for font substitutes
	//

	lstrcpy(szFontName, lf.lfFaceName);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes"),
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{

		dwSize = MAX_PATH * sizeof(TCHAR);
		RegQueryValueEx(hKey, lf.lfFaceName, NULL, &dwType,
			(LPBYTE) szFontName, &dwSize);

		RegCloseKey(hKey);
	}


	//
	//  This is to fix office for locales that use non 1252 versions
	//  of Ms Sans Serif and Ms Serif.  These fonts incorrectly identify
	//  themselves as having an Ansi charset, so TranslateCharsetInfo will
	//  return the wrong value.
	//
	//  NT bug 260697: Office 2000 uses Tahoma.
	//
	if ((lf.lfCharSet == ANSI_CHARSET) &&
		(!lstrcmpi(L"Helv", szFontName) ||
			!lstrcmpi(L"Ms Sans Serif", szFontName) ||
			!lstrcmpi(L"Ms Serif", szFontName) ||
			!lstrcmpi(L"Tahoma", szFontName)))
	{
		return CP_ACP;
	}
	//
	//  This is to fix FE office95a and Pro. msofe95.dll sets wrong charset when create
	//  listview control. so TranslateCharsetInfo will return the wrong value.
	//  Korea  : DotumChe.
	//  Taiwan : New MingLight
	//  China  : SongTi

	if ((lf.lfCharSet == SHIFTJIS_CHARSET) &&
		(!lstrcmpi(L"\xb3cb\xc6c0\xccb4", lf.lfFaceName))        || // Korea
		(!lstrcmpi(L"\x65b0\x7d30\x660e\x9ad4", lf.lfFaceName))  || // Taiwan
		(!lstrcmpi(L"\x5b8b\x4f53", lf.lfFaceName)))                // PRC
	{
		return CP_ACP;
	}

	if (!TranslateCharsetInfo((DWORD *) lf.lfCharSet, &csi, TCI_SRCCHARSET))
	{
		return CP_ACP;
	}

	return csi.ciACP;
#else

	return CP_ACP;

#endif
}

//
//  New for v5.01:
//
//  Accessibility (and some other callers, sometimes even us) relies on
//  a XXM_GETITEM call filling the buffer and not just redirecting the
//  pointer.  Accessibility is particularly screwed by this because they
//  live outside the process, so the redirected pointer means nothing
//  to them.  Here, we copy the result back into the app buffer and return
//  the raw pointer.  The caller will return the raw pointer back to the
//  app, so the answer is in two places, either the app buffer, or in
//  the raw pointer.
//
//  Usage:
//
//      if (nm.item.mask & LVIF_TEXT)
//          pitem->pszText = CCReturnDispInfoText(nm.item.pszText,
//                              pitem->pszText, pitem->cchTextMax);
//
LPTSTR IUI::CCReturnDispInfoText(LPTSTR pszSrc, LPTSTR pszDest, UINT cchDest)
{
	// Test pszSrc != pszDest first since the common case is that they
	// are equal.
	if (pszSrc != pszDest && !IsFlagPtr(pszSrc) && !IsFlagPtr(pszDest))
	{
		StrCpyN(pszDest, pszSrc, cchDest);
	}
	return pszSrc;
}

//===========================================================================
//
// String ptr management routines
//
// Copy as much of *psz to *pszBuf as will fit
//
// Warning:  this same code is duplicated below.
//
int IUI::IUIStr_GetPtr(LPCTSTR pszCurrent, LPTSTR pszBuf, int cchBuf)
{
	int cchToCopy;

	if (!pszCurrent)
	{
		_ASSERT(FALSE);

		if (cchBuf > 0)
		{
			*pszBuf = TEXT('\0');
		}

		return 0;
	}

	cchToCopy = lstrlen(pszCurrent);

	// if pszBuf is NULL, or they passed cchBuf = 0, return the needed buff size
	if (!pszBuf || !cchBuf)
	{
		return cchToCopy + 1;
	}

	if (cchToCopy >= cchBuf)
	{
		cchToCopy = cchBuf - 1;
	}

	hmemcpy(pszBuf, pszCurrent, cchToCopy * sizeof(TCHAR));
	pszBuf[cchToCopy] = TEXT('\0');

	return cchToCopy + 1;
}

/*----------------------------------------------------------
Purpose: Wide-char version of CcshellDebugMsgA.  Note this
         function deliberately takes an ANSI format string
         so our trace messages don't all need to be wrapped
         in TEXT().

*/
void IUIAPI CDECL CcshellDebugMsgW(DWORD flag, LPCSTR pszMsg, ...)         // (this is deliberately CHAR)
{
	//L WCHAR ach[1024 + 40];  // Largest path plus extra
	//va_list vArgs;

	//if (TF_ALWAYS == flag || (IsFlagSet(g_dwTraceFlags, flag) && flag))
	//{
	//	int cch;
	//	WCHAR wszBuf[1024];
	//	LPWSTR pwsz;

	//	SetPrefixStringW(ach, flag);
	//	cch = lstrlenW(ach);
	//	va_start(vArgs, pszMsg);

	//	// (We convert the string, rather than simply input an
	//	// LPCWSTR parameter, so the caller doesn't have to wrap
	//	// all the string constants with the TEXT() macro.)

	//	if (UnicodeFromAnsi(&pwsz, pszMsg, wszBuf, SIZECHARS(wszBuf)))
	//	{
	//		wvsprintfW(&ach[cch], pwsz, vArgs);
	//		UnicodeFromAnsi(&pwsz, NULL, wszBuf, 0);
	//	}

	//	va_end(vArgs);
	//	OutputDebugStringW(ach);
	//	OutputDebugStringW(c_wszNewline);

	//	if (TF_ALWAYS != flag &&
	//		((flag & TF_ERROR) && IsFlagSet(g_dwBreakFlags, BF_ONERRORMSG) ||
	//			(flag & TF_WARNING) && IsFlagSet(g_dwBreakFlags, BF_ONWARNMSG)))
	//	{
	//		// MSDEV USERS:  This is not the real assert.  Hit
	//		//               Shift-F11 to jump back to the caller.
	//		DEBUG_BREAK;                                                // _ASSERT
	//	}
	//}
}

void ScrollShrinkRect(int x, int y, LPRECT lprc)
{
	if (lprc)
	{
		if (x > 0)
		{
			lprc->left += x;
		}
		else
		{
			lprc->right += x;
		}

		if (y > 0)
		{
			lprc->top += y;
		}
		else
		{
			lprc->bottom += y;
		}

	}
}

#define SUBSCROLLS 100

#define DEFAULT_MAXSCROLLTIME ((GetDoubleClickTime() / 2) + 1)  // Ensure >= 1
#define DEFAULT_MINSCROLL 8

int IUI::SmoothScrollWindow(PSMOOTHSCROLLINFO psi)
{
	int dx = psi->dx;
	int dy = psi->dy;
	LPCRECT lprcSrc = psi->lprcSrc;
	LPCRECT lprcClip = psi->lprcClip;
	HRGN hrgnUpdate = psi->hrgnUpdate;
	LPRECT lprcUpdate = psi->lprcUpdate;
	UINT fuScroll = psi->fuScroll;
	int iRet = SIMPLEREGION;
	RECT rcUpdate;
	RECT rcSrc;
	RECT rcClip;
	int xStep;
	int yStep;
	int iSlicesDone = 0;
	int iSlices;
	DWORD dwTimeStart, dwTimeNow;
	HRGN hrgnLocalUpdate;
	UINT cxMinScroll = psi->cxMinScroll;
	UINT cyMinScroll = psi->cyMinScroll;
	UINT uMaxScrollTime = psi->uMaxScrollTime;
	int iSubScrolls;
	PFNSMOOTHSCROLLPROC pfnScrollProc = NULL;

	if (!lprcUpdate)
	{
		lprcUpdate = &rcUpdate;
	}
	SetRectEmpty(lprcUpdate);

	if (psi->cbSize != sizeof(SMOOTHSCROLLINFO))
	{
		return 0;
	}

	// check the defaults
	if (!(psi->fMask & SSIF_MINSCROLL)
		|| cxMinScroll == SSI_DEFAULT)
	{
		cxMinScroll = DEFAULT_MINSCROLL;
	}

	if (!(psi->fMask & SSIF_MINSCROLL)
		|| cyMinScroll == SSI_DEFAULT)
	{
		cyMinScroll = DEFAULT_MINSCROLL;
	}

	if (!(psi->fMask & SSIF_MAXSCROLLTIME)
		|| uMaxScrollTime == SSI_DEFAULT)
	{
		uMaxScrollTime = DEFAULT_MAXSCROLLTIME;
	}

	if (uMaxScrollTime < SUBSCROLLS)
	{
		uMaxScrollTime = SUBSCROLLS;
	}


	if ((!(fuScroll & SSW_EX_IGNORESETTINGS)) &&
		(!IUIGetGloabelVar()->g_fSmoothScroll))
	{
		fuScroll |= SSW_EX_IMMEDIATE;
	}

	if ((psi->fMask & SSIF_SCROLLPROC) && psi->pfnScrollProc)
	{
		pfnScrollProc = psi->pfnScrollProc;
	}
	else
	{
		//_ASSERT(FALSE);
		//pfnScrollProc = ScrollWindowEx;
	}

#ifdef ScrollWindowEx
#undef ScrollWindowEx
#endif

	if (fuScroll & SSW_EX_IMMEDIATE)
	{
		SListViewData *plv = (SListViewData *)GetMember(psi->hWLWnd);

		if (pfnScrollProc != NULL)
		{
			return pfnScrollProc(plv, dx, dy, lprcSrc, lprcClip, hrgnUpdate,
					lprcUpdate, LOWORD(fuScroll));
		}

		return 0;
	}

	// copy input rects locally
	if (lprcSrc)
	{
		rcSrc = *lprcSrc;
		lprcSrc = &rcSrc;
	}
	if (lprcClip)
	{
		rcClip = *lprcClip;
		lprcClip = &rcClip;
	}

	if (!hrgnUpdate)
	{
		hrgnLocalUpdate = CreateRectRgn(0, 0, 0, 0);
	}
	else
	{
		hrgnLocalUpdate = hrgnUpdate;
	}

	//set up initial vars
	dwTimeStart = GetTickCount();

	if (fuScroll & SSW_EX_NOTIMELIMIT)
	{
		xStep = cxMinScroll * (dx < 0 ? -1 : 1);
		yStep = cyMinScroll * (dy < 0 ? -1 : 1);
	}
	else
	{
		iSubScrolls = (uMaxScrollTime / DEFAULT_MAXSCROLLTIME) * SUBSCROLLS;
		if (!iSubScrolls)
		{
			iSubScrolls = SUBSCROLLS;
		}
		xStep = dx / iSubScrolls;
		yStep = dy / iSubScrolls;
	}

	if (xStep == 0 && dx)
	{
		xStep = dx < 0 ? -1 : 1;
	}

	if (yStep == 0 && dy)
	{
		yStep = dy < 0 ? -1 : 1;
	}

	while (dx || dy)
	{
		int x, y;
		RECT rcTempUpdate;

		if (fuScroll & SSW_EX_NOTIMELIMIT)
		{
			x = xStep;
			y = yStep;
			if (abs(x) > abs(dx))
			{
				x = dx;
			}

			if (abs(y) > abs(dy))
			{
				y = dy;
			}

		}
		else
		{
			int iTimePerScroll = uMaxScrollTime / iSubScrolls;
			if (!iTimePerScroll)
			{
				iTimePerScroll = 1;
			}

			dwTimeNow = GetTickCount();

			iSlices = ((dwTimeNow - dwTimeStart) / iTimePerScroll) - iSlicesDone;
			if (iSlices < 0)
			{
				iSlices = 0;
			}


			do
			{

				int iRet = 0;

				iSlices++;
				if ((iSlicesDone + iSlices) <= iSubScrolls)
				{
					x = xStep * iSlices;
					y = yStep * iSlices;

					// this could go over if we rounded ?Step up to 1(-1) above
					if (abs(x) > abs(dx))
					{
						x = dx;
					}

					if (abs(y) > abs(dy))
					{
						y = dy;
					}

				}
				else
				{
					x = dx;
					y = dy;
				}

				//DebugMsg(DM_TRACE, "SmoothScrollWindowCallback %d", iRet);

				if (x == dx && y == dy)
				{
					break;
				}

				if ((((UINT)(abs(x)) >= cxMinScroll) || !x) &&
					(((UINT)(abs(y)) >= cyMinScroll) || !y))
				{
					break;
				}

			}
			while (1);
		}

		// we don't need to do this always because if iSlices >= iSlicesDone, we'll have scrolled blanks
		//if (iSlices < iSlicesDone)
		Invalidate(psi->hWLWnd);

		//RedrawWindow(psi->hwnd, NULL, hrgnLocalUpdate, RDW_ERASE | RDW_ERASENOW | RDW_INVALIDATE);

		UnionRect(lprcUpdate, &rcTempUpdate, lprcUpdate);

		ScrollShrinkRect(x, y, (LPRECT)lprcSrc);
		ScrollShrinkRect(x, y, (LPRECT)lprcClip);

		dx -= x;
		dy -= y;
		iSlicesDone += iSlices;
	}

	//Bail:

	if (fuScroll & SW_SCROLLCHILDREN)
	{
		Invalidate(psi->hWLWnd);
	}

	if (hrgnLocalUpdate != hrgnUpdate)
	{
		DeleteObject(hrgnLocalUpdate);
	}

	return iRet;
}

DWORD IUI::CICustomDrawNotify(LibUIDK_LPCONTROLINFO lpci, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd)
{
	DWORD dwRet = CDRF_DODEFAULT;


	// bail if...


	// this is an item notification, but an item notification wasn't asked for
	if ((dwStage & CDDS_ITEM) && !(lpci->dwCustom & CDRF_NOTIFYITEMDRAW))
	{
		return dwRet;
	}

	lpnmcd->dwDrawStage = dwStage;
	dwRet = (DWORD) CCSendNotify(lpci, NM_CUSTOMDRAW, &lpnmcd->hdr);

	// validate the flags
	if (dwRet & ~CDRF_VALIDFLAGS)
	{
		return CDRF_DODEFAULT;
	}

	return dwRet;
}

HBITMAP IUI::CreateMonoBitmap(int cx, int cy)
{
	return CreateBitmap(cx, cy, 1, 1, NULL);
}

//
//  Too many apps encounter strange behavior when we send out
//  NM_CUSTOMDRAW messages at times unrelated to painting.
//  E.g., NetMeeting and MFC recurse back into ListView_RecomputeLabelSize.
//  CryptUI will fault if it's asked to NM_CUSTOMDRAW before it gets
//  WM_INITDIALOG.  So all this fake customdraw stuff is v5 only.
//
//  And since it is very popular to call back into the control during
//  the handling of NM_CUSTOMDRAW, we protect against recursing ourselves
//  to death by blowing off nested fake customdraw messages.


DWORD IUI::CIFakeCustomDrawNotify(LibUIDK_LPCONTROLINFO lpci, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd)
{
	DWORD dwRet = CDRF_DODEFAULT;

	if (lpci->iVersion >= 5 && !lpci->bInFakeCustomDraw)
	{
		lpci->bInFakeCustomDraw = TRUE;
		dwRet = CICustomDrawNotify(lpci, dwStage, lpnmcd);
		_ASSERT(lpci->bInFakeCustomDraw);
		lpci->bInFakeCustomDraw = FALSE;
	}

	return dwRet;
}

/***************************************************************************\
* SystoChar
*
* EXIT: If the message was not made with the ALT key down, convert
*       the message from a WM_SYSKEY* to a WM_KEY* message.
*
* IMPLEMENTATION:
*     The 0x2000 bit in the hi word of lParam is set if the key was
*     made with the ALT key down.
*
* History:
*   11/30/90 JimA       Ported.
\***************************************************************************/

UINT IUI::SystoChar(UINT message, LPARAM lParam)
{
	if (CheckMsgFilter(message, WM_SYSKEYDOWN, WM_SYSDEADCHAR) &&
		!(HIWORD(lParam) & SYS_ALTERNATE))
	{
		return (message - (WM_SYSKEYDOWN - WM_KEYDOWN));
	}

	return message;
}

int IUI::GetLabelStateIndex(CONTROL_STATE eControlState)
{
	if (eControlState == CONTROL_STATE_NORMAL)
	{
		return 0;
	}
	else if (eControlState == CONTROL_STATE_DISABLED)
	{
		return 1;
	}

	_ASSERT(FALSE);
	return 0;
}

int IUI::GetButtonStateIndex(CONTROL_STATE eControlState)
{
	if (eControlState == CONTROL_STATE_NORMAL)
	{
		return 0;
	}
	else if (eControlState == CONTROL_STATE_HOVER)
	{
		return 1;
	}
	else if (eControlState == CONTROL_STATE_PRESSED)
	{
		return 2;
	}
	else if (eControlState == CONTROL_STATE_DISABLED)
	{
		return 3;
	}
	else if (eControlState == CONTROL_STATE_SELECTED)
	{
		return 4;
	}

	_ASSERT(FALSE);
	return 0;
}

int ControlResMgr::IUISetControlImage2(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIIMAGE *phIUIImage,
	LPCTSTR lpszImageN,
	LPCTSTR lpszImageD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageD);
	}

	return 0;
}

int ControlResMgr::IUISetControlCheckedImage4(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIIMAGE *phIUIImage,
	LPCTSTR lpszImageNameCN,
	LPCTSTR lpszImageNameCH,
	LPCTSTR lpszImageNameCS,
	LPCTSTR lpszImageNameCD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameCN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameCH);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameCS);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameCD);
	}

	return 0;
}

int ControlResMgr::IUISetControlForeground2(
	IN CIUIProject *pOwner,
	UINT uMask,
	__inout HIUIIMAGE *phIUIImage,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameD)
{
	int nIndex = 0;

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameN);
	}
	nIndex++;

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		IUISetControlImage(pOwner, phIUIImage + nIndex, lpszImageNameD);
	}

	return 0;
}

int ControlResMgr::IUIGetControlImage(HIUIIMAGE hIUIImage, CIUIString *pstrImageName)
{
	if (hIUIImage == NULL || pstrImageName == NULL)
	{
		return -1;
	}

	*pstrImageName = hIUIImage->GetSafeImageName();

	return 0;
}

int ControlResMgr::IUIGetControlImage2(
	UINT uMask,
	HIUIIMAGE hIUICombineImage,
	HIUIIMAGE *phIUIImage,
	BOOL bCombineImage,
	CIUIString *pstrImageNameCombine,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameD)
{
	if (bCombineImage)
	{
		if (pstrImageNameCombine == NULL || hIUICombineImage == NULL)
		{
			return -1;
		}

		*pstrImageNameCombine = hIUICombineImage->GetSafeImageName();
	}
	else
	{
		if (phIUIImage == NULL)
		{
			return -1;
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			if (pstrImageNameN == NULL)
			{
				return -1;
			}

			*pstrImageNameN = (*phIUIImage)->GetSafeImageName();
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			if (pstrImageNameD == NULL)
			{
				return -1;
			}

			*pstrImageNameD = (*(phIUIImage + 1))->GetSafeImageName();
		}
	}

	return 0;
}

void IUI::SetEditInPlaceSize(HWND hwndEdit, RECT *prc, HFONT hFont, UINT seips)
{
	RECT rc, rcClient, rcFormat;
	TCHAR szLabel[CCHLABELMAX + 1];
	int cchLabel, cxIconTextWidth;
	HDC hdc;
	HWND hwndParent = ::GetParent(hwndEdit);
	UINT flags;

	// was #ifdef DBCS
	// short wRightMgn;
	// #endif

	cchLabel = Edit_GetText(hwndEdit, szLabel, ARRAYSIZE(szLabel));
	if (szLabel[0] == 0)
	{
		StringCchCopy(szLabel, CCHLABELMAX + 1, TEXT(" "));
		cchLabel = 1;
	}

	hdc = GetDC(hwndParent);

#ifdef DEBUG
	//DrawFocusRect(hdc, prc);       // this is the rect they are passing in
#endif

	SelectFont(hdc, hFont);

	cxIconTextWidth = 10;
	rc.left = rc.top = rc.bottom = 0;
	rc.right = cxIconTextWidth;      // for DT_LVWRAP

	// REVIEW: we might want to include DT_EDITCONTROL in our DT_LVWRAP

	if (seips & SEIPS_WRAP)
	{
		flags = DT_LVWRAP | DT_CALCRECT;
		// We only use DT_NOFULLWIDTHCHARBREAK on Korean(949) Memphis and NT5
		//L if (949 == g_uiACP && (g_bRunOnNT5 || g_bRunOnMemphis))
		//{
		//	flags |= DT_NOFULLWIDTHCHARBREAK;
		//}
	}
	else
	{
		flags = DT_LV | DT_CALCRECT;
	}
	// If the string is NULL display a rectangle that is visible.
	DrawText(hdc, szLabel, cchLabel, &rc, flags);

	// Minimum text box size is 1/4 icon spacing size
	if (rc.right < 10)
	{
		rc.right = 10;
	}

	// position the text rect based on the text rect passed in
	// if wrapping, center the edit control around the text mid point

	OffsetRect(&rc,
		(seips & SEIPS_WRAP) ? prc->left + ((prc->right - prc->left) - (rc.right - rc.left)) / 2 : prc->left,
		(seips & SEIPS_WRAP) ? prc->top : prc->top + ((prc->bottom - prc->top) - (rc.bottom - rc.top)) / 2);

	// give a little space to ease the editing of this thing
	if (!(seips & SEIPS_WRAP))
	{
		rc.right += 8 * 4;
	}
	rc.right += 8;   // try to leave a little more for dual blanks

#ifdef DEBUG
	//DrawFocusRect(hdc, &rc);
#endif

	ReleaseDC(hwndParent, hdc);

	//
	// #5688: We need to make it sure that the whole edit window is
	//  always visible. We should not extend it to the outside of
	//  the parent window.
	//
	{
		BOOL fSuccess;
		::GetClientRect(hwndParent, &rcClient);
		fSuccess = IntersectRect(&rc, &rc, &rcClient);
#ifdef DEBUG
		_ASSERT(fSuccess || IsRectEmpty(&rcClient) || (seips & SEIPS_NOSCROLL));
#endif // DEBUG
	}

	//
	// Inflate it after the clipping, because it's ok to hide border.
	//
	// EM_GETRECT already takes EM_GETMARGINS into account, so don't use both.

	::SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)(LPRECT)&rcFormat);

	// Turn the margins inside-out so we can AdjustWindowRect on them.
	rcFormat.top = -rcFormat.top;
	rcFormat.left = -rcFormat.left;
	AdjustWindowRectEx(&rcFormat, GetWindowStyle(hwndEdit), FALSE,
		GetWindowExStyle(hwndEdit));

	InflateRect(&rc, -rcFormat.left, -rcFormat.top);

	HideCaret(hwndEdit);

	::SetWindowPos(hwndEdit, NULL, rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);

	CopyRect(prc, (CONST RECT *)&rc);

	::InvalidateRect(hwndEdit, NULL, TRUE);

	ShowCaret(hwndEdit);
}

void RescrollEditWindow(HWND hwndEdit)
{
	Edit_SetSel(hwndEdit, -1, -1);      // move to the end
	Edit_SetSel(hwndEdit, 0, -1);       // select all text
}
// BUGBUG: very similar code in treeview.c

HWND IUI::CreateEditInPlaceWindow(HWND hwnd, LPCTSTR lpText, int cbText, LONG style, HFONT hFont)
{
	HWND hwndEdit;

	// Create the window with some nonzero size so margins work properly
	// The caller will do a SetEditInPlaceSize to set the real size
	// But make sure the width is huge so when an app calls SetWindowText,
	// USER won't try to scroll the window.
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	hwndEdit = ::CreateWindowEx(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_RTLREADING,
			TEXT("EDIT"), lpText, style,
			0, 0, 16384, 20, hwnd, NULL, hInst, NULL);

	if (hwndEdit)
	{

		Edit_LimitText(hwndEdit, cbText);

		Edit_SetSel(hwndEdit, 0, 0);    // move to the beginning

		FORWARD_WM_SETFONT(hwndEdit, hFont, FALSE, ::SendMessage);

	}

	return hwndEdit;
}
// BUGBUG: very similar routine in treeview

BOOL SameChars(LPTSTR lpsz, TCHAR c)
{
	while (*lpsz)
	{
		if (*lpsz++ != c)
		{
			return FALSE;
		}
	}
	return TRUE;
}

#ifndef UNICODE

// TruncateString - bugbug: the same logic in shdocvw
//
// purpose: cut a string at the given length in dbcs safe manner
//          the string may be truncated at cch-1 if the sz[cch] points
//          to a lead byte that would result in cutting in the middle
//          of double byte character.
//          It is caller's responsibility to reserve enough buffer for
//          sz so we can put sz[cch]=0 safely.
//
//          *Note this logic is not much perf hit when called with sbcs
//          string, as it just bails out at the tail character always.
//
void  TruncateString(char *sz, int cchBufferSize)
{
	int cch = cchBufferSize - 1;
	LPSTR psz = &sz[cch];

	if (!sz || cchBufferSize <= 0)
	{
		return;
	}

	while (psz > sz)
	{
		psz--;
		if (!IsDBCSLeadByte(*psz))
		{
			// Found non-leadbyte for the first time.
			// This is either a trail byte of double byte char
			// or a single byte character we've first seen.
			// Thus, the next pointer must be at either of a leadbyte
			// or &sz[cch]
			psz++;
			break;
		}
	}
	if (((&sz[cch] - psz) & 1) && cch > 0)
	{
		// we're truncating the string in the middle of dbcs
		cch--;
	}
	sz[cch] = '\0';
	return;
}
#endif

HIMAGELIST CreateCheckBoxImagelist(HIMAGELIST himl, BOOL fTree, BOOL fUseColorKey, BOOL fMirror)
{
	int cxImage, cyImage;
	HBITMAP hbm;
	HBITMAP hbmTemp;
	COLORREF clrMask;
	HDC hdcDesk = GetDC(NULL);
	HDC hdc;
	RECT rc;
	int nImages = fTree ? 3 : 2;

	if (!hdcDesk)
	{
		return NULL;
	}

	hdc = CreateCompatibleDC(hdcDesk);
	ReleaseDC(NULL, hdcDesk);

	if (!hdc)
	{
		return NULL;
	}

	// Must protect against ImageList_GetIconSize failing in case app
	// gave us a bad himl
	if (himl && ImageList_GetIconSize(himl, &cxImage, &cyImage))
	{
		// cxImage and cyImage are okay
	}
	else
	{
		cxImage = GetSystemMetrics(SM_CXSMICON);
		cyImage = GetSystemMetrics(SM_CYSMICON);
	}

	himl = ImageList_Create(cxImage, cyImage, ILC_MASK, 0, nImages);
	hbm = CreateColorBitmap(cxImage * nImages, cyImage);

	if (fUseColorKey)
	{
		clrMask = RGB(255, 000, 255); // magenta
		if (clrMask == GetSysColor(COLOR_WINDOW))
		{
			clrMask = RGB(000, 000, 255);    // blue
		}
	}
	else
	{
		clrMask = GetSysColor(COLOR_WINDOW);
	}

	// fill
	hbmTemp = (HBITMAP)SelectObject(hdc, hbm);

	rc.left = rc.top = 0;
	rc.bottom = cyImage;
	rc.right = cxImage * nImages;
	FillRectClr(hdc, &rc, clrMask);

	rc.right = cxImage;
	// now draw the real controls on
	InflateRect(&rc, -8, -8);
	rc.right++;
	rc.bottom++;

	if (fTree)
	{
		OffsetRect(&rc, cxImage, 0);
	}

	DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT |
		(fUseColorKey ? 0 : DFCS_TRANSPARENT));
	OffsetRect(&rc, cxImage, 0);
	// [msadek]; For the mirrored case, there is an off-by-one somewhere in MirrorIcon() or System API.
	// Since I will not be touching MirrorIcon() by any mean and no chance to fix a system API,
	// let's compensate for it here.
	if (fMirror)
	{
		OffsetRect(&rc, -1, 0);
	}

	DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_FLAT | DFCS_CHECKED |
		(fUseColorKey ? 0 : DFCS_TRANSPARENT));

	SelectObject(hdc, hbmTemp);

	if (fUseColorKey)
	{
		ImageList_AddMasked(himl, hbm, clrMask);
	}
	else
	{
		ImageList_Add(himl, hbm, NULL);
	}

	if (fMirror)
	{
		HICON hIcon = ImageList_ExtractIcon(0, himl, nImages - 1);
		MirrorIcon(&hIcon, NULL);
		ImageList_ReplaceIcon(himl, nImages - 1, hIcon);
	}

	DeleteDC(hdc);
	DeleteObject(hbm);
	return himl;
}

HFONT IUI::CCCreateStatusFont(void)
{
	NONCLIENTMETRICS ncm;

	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

	return CreateFontIndirect(&ncm.lfStatusFont);
}

LRESULT WINAPI IUI::IUI_CCSendNotify(IUICONTROLINFO *pci, int code, LPNMHDR pnmhdr)
{
	NMHDR nmhdr;
	LONG_PTR id;
#ifdef UNICODE
	THUNKSTATE ts = { 0 };
#define pvThunk1 ts.ts_pvThunk1
#define pvThunk2 ts.ts_pvThunk2
#define dwThunkSize ts.ts_dwThunkSize
	LRESULT lRet;
	BOOL  bSet = FALSE;
#endif
	HWND hwndParent = pci->hwndParent;
	DWORD dwParentPid;

	// -1 means Requery on each notify
	if (hwndParent == (HWND) - 1)
	{
		hwndParent = ::GetParent(pci->hwnd);
	}

	// unlikely but it can technically happen -- avoid the rips
	if (hwndParent == NULL)
	{
		return 0;
	}

	//
	// If pci->hwnd is -1, then a WM_NOTIFY is being forwared
	// from one control to a parent.  EG:  Tooltips sent
	// a WM_NOTIFY to toolbar, and toolbar is forwarding it
	// to the real parent window.
	//

	if (pci->hwnd != (HWND) - 1)
	{

		//
		// If this is a child then get its ID.  We need to go out of our way to
		// avoid calling GetDlgCtrlID on toplevel windows since it will return
		// a pseudo-random number (those of you who know what this number is
		// keep quiet).  Anyway it's kinda hard to figure this out in Windows
		// because of the following:
		//
		//  - a window can SetWindowLong(GWL_STYLE, WS_CHILD) but this only
		//    does about half the work - hence checking the style is out.
		//  - GetParent will return your OWNER if you are toplevel.
		//  - there is no GetWindow(...GW_HWNDPARENT) to save us.
		//
		// Hence we are stuck with calling GetParent and then checking to see
		// if it lied and gave us the owner instead.  Yuck.
		//
		id = 0;
		if (pci->hwnd)
		{
			HWND hwndParent = ::GetParent(pci->hwnd);

			if (hwndParent && (hwndParent != ::GetWindow(pci->hwnd, GW_OWNER)))
			{
				id = ::GetDlgCtrlID(pci->hwnd);
			}
		}

		if (!pnmhdr)
		{
			pnmhdr = &nmhdr;
		}

		pnmhdr->hwndFrom = pci->hwnd;
		pnmhdr->idFrom = id;
		pnmhdr->code = code;
	}
	else
	{

		id = pnmhdr->idFrom;
		code = pnmhdr->code;
	}


	// OLE in its massively componentized world sometimes creates
	// a control whose parent belongs to another process.  (For example,
	// when there is a local server embedding.)  WM_NOTIFY
	// messages can't cross process boundaries, so stop the message
	// from going there lest we fault the recipient.
	if (!GetWindowThreadProcessId(hwndParent, &dwParentPid) ||
		dwParentPid != GetCurrentProcessId())
	{
		//TraceMsg(TF_WARNING, "nf: Not sending WM_NOTIFY %08x across processes", code);
		return 0;
	}

#ifdef NEED_WOWGETNOTIFYSIZE_HELPER
	_ASSERT(code >= 0 || WOWGetNotifySize(code));
#endif // NEED_WOWGETNOTIFYSIZE_HELPER

#ifdef UNICODE
	/*
	 * All the thunking for Notify Messages happens here
	 */
	if (!pci->bUnicode)
	{
		BOOL fThunked = TRUE;
		switch (code)
		{
		case LVN_ODFINDITEMW:
			pnmhdr->code = LVN_ODFINDITEMA;
			goto ThunkLV_FINDINFO;

		case LVN_INCREMENTALSEARCHW:
			pnmhdr->code = LVN_INCREMENTALSEARCHA;
			goto ThunkLV_FINDINFO;

ThunkLV_FINDINFO:
			{
				LV_FINDINFO *plvfi;

				// Hack Alert!  This code assumes that all fields of LV_FINDINFOA and
				// LV_FINDINFOW are exactly the same except for the string pointers.
				COMPILETIME_ASSERT(sizeof(LV_FINDINFOA) == sizeof(LV_FINDINFOW));

				// Since WCHARs are bigger than char, we will just use the
				// wchar buffer to hold the chars, and not worry about the extra
				// room at the end.
				COMPILETIME_ASSERT(sizeof(WCHAR) >= sizeof(char));

				plvfi = &((PNM_FINDITEM)pnmhdr)->lvfi;
				if (plvfi->flags & (LVFI_STRING | LVFI_PARTIAL | LVFI_SUBSTRING))
				{
					pvThunk1 = (PVOID)plvfi->psz;
					dwThunkSize = lstrlen((LPCWSTR)pvThunk1) + 1;
					plvfi->psz = (LPWSTR)ProduceAFromW(pci->uiCodePage, plvfi->psz);
				}
			}
			break;

		case LVN_GETDISPINFOW:
		{
			LV_ITEMW *pitem;

			pnmhdr->code = LVN_GETDISPINFOA;

			// Hack Alert!  This code assumes that all fields of LV_DISPINFOA and
			// LV_DISPINFOW are exactly the same except for the string pointers.

			COMPILETIME_ASSERT(sizeof(LV_DISPINFOA) == sizeof(LV_DISPINFOW));

			// Since WCHARs are bigger than char, we will just use the
			// wchar buffer to hold the chars, and not worry about the extra
			// room at the end.
			COMPILETIME_ASSERT(sizeof(WCHAR) >= sizeof(char));

			//
			// Some sleazebag code (shell32.dll) just changes the pszText
			// pointer to point to the name, so capture the original pointer
			// so we can detect this and not smash their data.
			//
			pitem = &(((LV_DISPINFOW *)pnmhdr)->item);
			if (!IsFlagPtr(pitem) && (pitem->mask & LVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
			}
			break;
		}


		// LVN_ENDLABELEDIT uses an INOUT parameter, never explicitly
		// documented as such, but it just happened to be that way,
		// and I don't want to take the chance that somebody was relying
		// on it.

		case LVN_ENDLABELEDITW:
			pnmhdr->code = LVN_ENDLABELEDITA;
			goto ThunkLV_DISPINFO;

		case LVN_BEGINLABELEDITW:
			pnmhdr->code = LVN_BEGINLABELEDITA;
			goto ThunkLV_DISPINFO;

		case LVN_SETDISPINFOW:
			pnmhdr->code = LVN_SETDISPINFOA;
			goto ThunkLV_DISPINFO;

		case LVN_GETEMPTYTEXTW:
			pnmhdr->code = LVN_GETEMPTYTEXTA;
			goto ThunkLV_DISPINFO;

ThunkLV_DISPINFO:
			{
				LV_ITEMW *pitem;

				COMPILETIME_ASSERT(sizeof(LV_ITEMA) == sizeof(LV_ITEMW));
				pitem = &(((LV_DISPINFOW *)pnmhdr)->item);

				if (pitem->mask & LVIF_TEXT)
				{
					if (!IUI_InOutWtoA(pci, &ts, &pitem->pszText, pitem->cchTextMax))
					{
						return 0;
					}
				}
				break;
			}

		case LVN_GETINFOTIPW:
		{
			NMLVGETINFOTIPW *pgit = (NMLVGETINFOTIPW *)pnmhdr;

			COMPILETIME_ASSERT(sizeof(NMLVGETINFOTIPA) == sizeof(NMLVGETINFOTIPW));
			pnmhdr->code = LVN_GETINFOTIPA;

			if (!IUI_InOutWtoA(pci, &ts, &pgit->pszText, pgit->cchTextMax))
			{
				return 0;
			}
		}
		break;


		case TVN_GETINFOTIPW:
		{
			NMTVGETINFOTIPW *pgit = (NMTVGETINFOTIPW *)pnmhdr;

			pnmhdr->code = TVN_GETINFOTIPA;

			pvThunk1 = pgit->pszText;
			dwThunkSize = pgit->cchTextMax;
		}
		break;

		case TBN_GETINFOTIPW:
		{
			NMTBGETINFOTIPW *pgit = (NMTBGETINFOTIPW *)pnmhdr;

			pnmhdr->code = TBN_GETINFOTIPA;

			pvThunk1 = pgit->pszText;
			dwThunkSize = pgit->cchTextMax;
		}
		break;

		case TVN_SELCHANGINGW:
			pnmhdr->code = TVN_SELCHANGINGA;
			bSet = TRUE;
		// fall through

		case TVN_SELCHANGEDW:
			if (!bSet)
			{
				pnmhdr->code = TVN_SELCHANGEDA;
				bSet = TRUE;
			}

		/*
		 * These msgs have a NM_TREEVIEW with both TV_ITEMs filled in
		 *
		 * FALL THROUGH TO TVN_DELETEITEM to thunk itemOld then go on for
		 * the other structure.
		 */

		// fall through

		case TVN_DELETEITEMW:
		{
			/*
			 * This message has a NM_TREEVIEW in lParam with itemOld filled in
			 */
			LPTV_ITEMW pitem;

			if (!bSet)
			{
				pnmhdr->code = TVN_DELETEITEMA;
				bSet = TRUE;
			}

			pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemOld);

			// thunk itemOld
			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk2 = pitem->pszText;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk2);
			}

			// if this is deleteitem then we are done
			if (pnmhdr->code == TVN_DELETEITEMA)
			{
				break;
			}

			/* FALL THROUGH TO TVN_ITEMEXPANDING to thunk itemNew */
		}
		// fall through

		case TVN_ITEMEXPANDINGW:
			if (!bSet)
			{
				pnmhdr->code = TVN_ITEMEXPANDINGA;
				bSet = TRUE;
			}
		// fall through

		case TVN_ITEMEXPANDEDW:
			if (!bSet)
			{
				pnmhdr->code = TVN_ITEMEXPANDEDA;
				bSet = TRUE;
			}
		// fall through

		case TVN_BEGINDRAGW:
			if (!bSet)
			{
				pnmhdr->code = TVN_BEGINDRAGA;
				bSet = TRUE;
			}
		// fall through

		case TVN_BEGINRDRAGW:
		{
			/* these msgs have a NM_TREEVIEW with itemNew TV_ITEM filled in */
			LPTV_ITEMW pitem;

			if (!bSet)
			{
				pnmhdr->code = TVN_BEGINRDRAGA;
			}

			pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemNew);

			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk1);
			}

			break;
		}

		case TVN_SETDISPINFOW:
			pnmhdr->code = TVN_SETDISPINFOA;
			goto ThunkTV_DISPINFO;

		case TVN_BEGINLABELEDITW:
			pnmhdr->code = TVN_BEGINLABELEDITA;
			goto ThunkTV_DISPINFO;


		// TVN_ENDLABELEDIT uses an INOUT parameter, never explicitly
		// documented as such, but it just happened to be that way,
		// and I don't want to take the chance that somebody was relying
		// on it.

		case TVN_ENDLABELEDITW:
			pnmhdr->code = TVN_ENDLABELEDITA;
			goto ThunkTV_DISPINFO;

ThunkTV_DISPINFO:
			{
				/*
				 * All these messages have a TV_DISPINFO in lParam.
				 */

				LPTV_ITEMW pitem;

				pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

				if (pitem->mask & TVIF_TEXT)
				{
					if (!IUI_InOutWtoA(pci, &ts, &pitem->pszText, pitem->cchTextMax))
					{
						return 0;
					}
				}
				break;
			}

#if !defined(UNIX) || defined(ANSI_SHELL32_ON_UNIX)
		/* UNIX shell32 TVN_GETDISPINFOA TVN_GETDISPINFOW does the same thing */
		case TVN_GETDISPINFOW:
		{
			/*
			 * All these messages have a TV_DISPINFO in lParam.
			 */
			LPTV_ITEMW pitem;

			pnmhdr->code = TVN_GETDISPINFOA;

			pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

			if ((pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText) && pitem->cchTextMax)
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
				pvThunk2 = LocalAlloc(LPTR, pitem->cchTextMax * sizeof(char));
				pitem->pszText = (LPWSTR)pvThunk2;
				pitem->pszText[0] = TEXT('\0');
			}

			break;
		}
#endif

		case HDN_ITEMCHANGINGW:
			pnmhdr->code = HDN_ITEMCHANGINGA;
			bSet = TRUE;
		// fall through

		case HDN_ITEMCHANGEDW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMCHANGEDA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ITEMCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ITEMDBLCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ITEMDBLCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_DIVIDERDBLCLICKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_DIVIDERDBLCLICKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_BEGINTRACKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_BEGINTRACKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_ENDTRACKW:
			if (!bSet)
			{
				pnmhdr->code = HDN_ENDTRACKA;
				bSet = TRUE;
			}
		// fall through

		case HDN_TRACKW:
		{
			HD_ITEMW *pitem;

			if (!bSet)
			{
				pnmhdr->code = HDN_TRACKA;
			}

			pitem = ((HD_NOTIFY *)pnmhdr)->pitem;

			if (!IsFlagPtr(pitem) && (pitem->mask & HDI_TEXT) && !IsFlagPtr(pitem->pszText))
			{
				pvThunk1 = pitem->pszText;
				dwThunkSize = pitem->cchTextMax;
				pitem->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk1);
			}


			if (!IsFlagPtr(pitem) && (pitem->mask & HDI_FILTER) && pitem->pvFilter)
			{
				if (!(pitem->type & HDFT_HASNOVALUE) &&
					((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
				{
					LPHD_TEXTFILTER ptextFilter = (LPHD_TEXTFILTER)pitem->pvFilter;
					pvThunk2 = ptextFilter->pszText;
					dwThunkSize = ptextFilter->cchTextMax;
					ptextFilter->pszText = (LPWSTR)ProduceAFromW(pci->uiCodePage, (LPCWSTR)pvThunk2);
				}
			}


			break;
		}

		case CBEN_ENDEDITW:
		{
			LPNMCBEENDEDITW peew = (LPNMCBEENDEDITW) pnmhdr;
			LPNMCBEENDEDITA peea = (LPNMCBEENDEDITA)LocalAlloc(LPTR, sizeof(NMCBEENDEDITA));

			if (!peea)
			{
				return 0;
			}

			peea->hdr  = peew->hdr;
			peea->hdr.code = CBEN_ENDEDITA;

			peea->fChanged = peew->fChanged;
			peea->iNewSelection = peew->iNewSelection;
			peea->iWhy = peew->iWhy;
			ConvertWToAN(pci->uiCodePage, peea->szText, ARRAYSIZE(peea->szText),
				peew->szText, -1);

			pvThunk1 = pnmhdr;
			pnmhdr = &peea->hdr;
			_ASSERT((LPVOID)pnmhdr == (LPVOID)peea);
			break;
		}

		case CBEN_DRAGBEGINW:
		{
			LPNMCBEDRAGBEGINW pdbw = (LPNMCBEDRAGBEGINW) pnmhdr;
			LPNMCBEDRAGBEGINA pdba = (LPNMCBEDRAGBEGINA)LocalAlloc(LPTR, sizeof(NMCBEDRAGBEGINA));

			if (!pdba)
			{
				return 0;
			}

			pdba->hdr  = pdbw->hdr;
			pdba->hdr.code = CBEN_DRAGBEGINA;
			pdba->iItemid = pdbw->iItemid;
			ConvertWToAN(pci->uiCodePage, pdba->szText, ARRAYSIZE(pdba->szText),
				pdbw->szText, -1);

			pvThunk1 = pnmhdr;
			pnmhdr = &pdba->hdr;
			_ASSERT((LPVOID)pnmhdr == (LPVOID)pdba);
			break;
		}


		case CBEN_GETDISPINFOW:
		{
			PNMCOMBOBOXEXW pnmcbe = (PNMCOMBOBOXEXW)pnmhdr;

			pnmhdr->code = CBEN_GETDISPINFOA;

			if (pnmcbe->ceItem.mask  & CBEIF_TEXT
				&& !IsFlagPtr(pnmcbe->ceItem.pszText) && pnmcbe->ceItem.cchTextMax)
			{
				pvThunk1 = pnmcbe->ceItem.pszText;
				dwThunkSize = pnmcbe->ceItem.cchTextMax;
				pvThunk2 = LocalAlloc(LPTR, pnmcbe->ceItem.cchTextMax * sizeof(char));
				pnmcbe->ceItem.pszText = (LPWSTR)pvThunk2;
				pnmcbe->ceItem.pszText[0] = TEXT('\0');
			}

			break;
		}

		case HDN_GETDISPINFOW:
		{
			LPNMHDDISPINFOW pHDDispInfoW;

			pnmhdr->code = HDN_GETDISPINFOA;

			pHDDispInfoW = (LPNMHDDISPINFOW) pnmhdr;

			pvThunk1 = pHDDispInfoW->pszText;
			dwThunkSize = pHDDispInfoW->cchTextMax;
			pHDDispInfoW->pszText = (LPWSTR)LocalAlloc(LPTR, pHDDispInfoW->cchTextMax * sizeof(char));

			if (!pHDDispInfoW->pszText)
			{
				pHDDispInfoW->pszText = (LPWSTR) pvThunk1;
				break;
			}

			WideCharToMultiByte(pci->uiCodePage, 0, (LPWSTR)pvThunk1, -1,
				(LPSTR)pHDDispInfoW->pszText, pHDDispInfoW->cchTextMax,
				NULL, NULL);
			break;
		}


		case TBN_GETBUTTONINFOW:
		{
			LPTBNOTIFYW pTBNW;

			pnmhdr->code = TBN_GETBUTTONINFOA;

			pTBNW = (LPTBNOTIFYW)pnmhdr;

			pvThunk1 = pTBNW->pszText;
			dwThunkSize = pTBNW->cchText;
			pvThunk2 = LocalAlloc(LPTR, pTBNW->cchText * sizeof(char));

			if (!pvThunk2)
			{
				break;
			}
			pTBNW->pszText = (LPWSTR)pvThunk2;

			WideCharToMultiByte(pci->uiCodePage, 0, (LPWSTR)pvThunk1, -1,
				(LPSTR)pTBNW->pszText, pTBNW->cchText,
				NULL, NULL);

		}
		break;

		case TTN_NEEDTEXTW:
		{
			LPTOOLTIPTEXTA lpTTTA;
			LPTOOLTIPTEXTW lpTTTW = (LPTOOLTIPTEXTW) pnmhdr;

			lpTTTA = (LPTOOLTIPTEXTA)LocalAlloc(LPTR, sizeof(TOOLTIPTEXTA));

			if (!lpTTTA)
			{
				return 0;
			}

			lpTTTA->hdr = lpTTTW->hdr;
			lpTTTA->hdr.code = TTN_NEEDTEXTA;

			lpTTTA->lpszText = lpTTTA->szText;
			lpTTTA->hinst    = lpTTTW->hinst;
			lpTTTA->uFlags   = lpTTTW->uFlags;
			lpTTTA->lParam   = lpTTTW->lParam;

			WideCharToMultiByte(pci->uiCodePage, 0, lpTTTW->szText, -1, lpTTTA->szText, ARRAYSIZE(lpTTTA->szText), NULL, NULL);
			pvThunk1 = pnmhdr;
			pnmhdr = (NMHDR FAR *)lpTTTA;
		}
		break;

		case DTN_USERSTRINGW:
		{
			LPNMDATETIMESTRINGW lpDateTimeString = (LPNMDATETIMESTRINGW) pnmhdr;

			pnmhdr->code = DTN_USERSTRINGA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeString->pszUserString);
			lpDateTimeString->pszUserString = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_WMKEYDOWNW:
		{
			LPNMDATETIMEWMKEYDOWNW lpDateTimeWMKeyDown =
				(LPNMDATETIMEWMKEYDOWNW) pnmhdr;

			pnmhdr->code = DTN_WMKEYDOWNA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeWMKeyDown->pszFormat);
			lpDateTimeWMKeyDown->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_FORMATQUERYW:
		{
			LPNMDATETIMEFORMATQUERYW lpDateTimeFormatQuery =
				(LPNMDATETIMEFORMATQUERYW) pnmhdr;

			pnmhdr->code = DTN_FORMATQUERYA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeFormatQuery->pszFormat);
			lpDateTimeFormatQuery->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		case DTN_FORMATW:
		{
			LPNMDATETIMEFORMATW lpDateTimeFormat =
				(LPNMDATETIMEFORMATW) pnmhdr;

			pnmhdr->code = DTN_FORMATA;

			pvThunk1 = ProduceAFromW(pci->uiCodePage, lpDateTimeFormat->pszFormat);
			lpDateTimeFormat->pszFormat = (LPWSTR) pvThunk1;
		}
		break;

		default:
			fThunked = FALSE;
			break;
		}

#ifdef NEED_WOWGETNOTIFYSIZE_HELPER
		_ASSERT(code >= 0 || WOWGetNotifySize(code));
#endif // NEED_WOWGETNOTIFYSIZE_HELPER

		lRet = SendMessage(hwndParent, WM_NOTIFY, (WPARAM)id, (LPARAM)pnmhdr);

		/*
		 * All the thunking for Notify Messages happens here
		 */
		if (fThunked)
		{
			switch (pnmhdr->code)
			{
			case LVN_ODFINDITEMA:
			case LVN_INCREMENTALSEARCHA:
			{
				LV_FINDINFO *plvfi = &((PNM_FINDITEM)pnmhdr)->lvfi;
				if (pvThunk1)
				{
					FreeProducedString((LPWSTR)plvfi->psz);
					plvfi->psz = (LPCWSTR)pvThunk1;
				}
			}
			break;

			case LVN_GETDISPINFOA:
			{
				LV_ITEMA *pitem = &(((LV_DISPINFOA *)pnmhdr)->item);

				// BUGBUG what if pointer is to large buffer?
				if (!IsFlagPtr(pitem) && (pitem->mask & LVIF_TEXT) && !IsFlagPtr(pitem->pszText))
				{
					StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pitem->pszText);
				}
			}
			break;

			case LVN_ENDLABELEDITA:
			case LVN_BEGINLABELEDITA:
			case LVN_SETDISPINFOA:
			case LVN_GETEMPTYTEXTA:
			{
				LV_ITEMA *pitem = &(((LV_DISPINFOA *)pnmhdr)->item);
				IUI_InOutAtoW(pci, &ts, &pitem->pszText);
			}
			break;

			case LVN_GETINFOTIPA:
			{
				NMLVGETINFOTIPA *pgit = (NMLVGETINFOTIPA *)pnmhdr;
				IUI_InOutAtoW(pci, &ts, &pgit->pszText);
			}
			break;

			case TVN_GETINFOTIPA:
			{
				NMTVGETINFOTIPA *pgit = (NMTVGETINFOTIPA *)pnmhdr;
				StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pgit->pszText);
			}
			break;

			case TBN_GETINFOTIPA:
			{
				NMTBGETINFOTIPA *pgit = (NMTBGETINFOTIPA *)pnmhdr;
				StringBufferAtoW(pci->uiCodePage, pvThunk1, dwThunkSize, &pgit->pszText);
			}
			break;
			case TVN_SELCHANGINGA:
			case TVN_SELCHANGEDA:
			case TVN_DELETEITEMA:
			{
				LPTV_ITEMW pitem;

				if (!IsFlagPtr(pvThunk2))
				{
					pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemOld);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPWSTR)pvThunk2;
				}

				// if this is delitem, then we are done
				if (code == TVN_DELETEITEM)
				{
					break;
				}

				/* FALL THROUGH TO TVN_ITEMEXPANDING to unthunk itemNew */
			}
			// fall through

			case TVN_ITEMEXPANDINGA:
			case TVN_ITEMEXPANDEDA:
			case TVN_BEGINDRAGA:
			case TVN_BEGINRDRAGA:
			{
				/* these msgs have a NM_TREEVIEW with itemNew TV_ITEM filled in */
				LPTV_ITEMW pitem;

				if (!IsFlagPtr(pvThunk1))
				{
					pitem = &(((LPNM_TREEVIEWW)pnmhdr)->itemNew);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPWSTR)pvThunk1;
				}

				break;
			}

			case TVN_SETDISPINFOA:
			case TVN_BEGINLABELEDITA:
			case TVN_ENDLABELEDITA:
			{
				LPTV_ITEMA pitem;
				pitem = &(((TV_DISPINFOA *)pnmhdr)->item);
				IUI_InOutAtoW(pci, &ts, &pitem->pszText);
			}
			break;

			case TVN_GETDISPINFOA:
			{
				/*
				 * This message has a TV_DISPINFO in lParam that wass filled in
				 * during the callback and needs to be unthunked.
				 */
				LPTV_ITEMW pitem;

				pitem = &(((TV_DISPINFOW *)pnmhdr)->item);

				if (!IsFlagPtr(pvThunk1) && (pitem->mask & TVIF_TEXT) && !IsFlagPtr(pitem->pszText))
				{
					ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)pitem->pszText, -1);
					pitem->pszText = (LPWSTR)pvThunk1;
					LocalFree(pvThunk2);
				}

				break;
			}

			case HDN_ITEMCHANGINGA:
			case HDN_ITEMCHANGEDA:
			case HDN_ITEMCLICKA:
			case HDN_ITEMDBLCLICKA:
			case HDN_DIVIDERDBLCLICKA:
			case HDN_BEGINTRACKA:
			case HDN_ENDTRACKA:
			case HDN_TRACKA:
			{
				HD_ITEMW *pitem;

				pitem = ((HD_NOTIFY *)pnmhdr)->pitem;

				if (!IsFlagPtr(pitem) && (pitem->mask & HDI_TEXT) && !IsFlagPtr(pvThunk1))
				{
					ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)(pitem->pszText), -1);

					FreeProducedString(pitem->pszText);
					pitem->pszText = (LPWSTR)pvThunk1;
				}

				if (!IsFlagPtr(pitem) && (pitem->mask & HDI_FILTER) && pitem->pvFilter && pvThunk2)
				{
					if (!(pitem->type & HDFT_HASNOVALUE) &&
						((pitem->type & HDFT_ISMASK) == HDFT_ISSTRING))
					{
						LPHD_TEXTFILTER ptextFilter = (LPHD_TEXTFILTER)pitem->pvFilter;
						ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk2, dwThunkSize, (LPSTR)(ptextFilter->pszText), -1);
						FreeProducedString(ptextFilter->pszText);
						ptextFilter->pszText = (LPWSTR)pvThunk2;
					}
				}

				break;
			}

			case CBEN_ENDEDITA:
			{
				LPNMCBEENDEDITW peew = (LPNMCBEENDEDITW) pvThunk1;
				LPNMCBEENDEDITA peea = (LPNMCBEENDEDITA) pnmhdr;

				// Don't unthunk the string since that destroys unicode round-trip
				// and the client shouldn't be modifying it anyway.
				// ConvertAToWN(pci->uiCodePage, peew->szText, ARRAYSIZE(peew->szText),
				//              peea->szText, -1);
				LocalFree(peea);
			}
			break;

			case CBEN_DRAGBEGINA:
			{
				LPNMCBEDRAGBEGINW pdbw = (LPNMCBEDRAGBEGINW) pvThunk1;
				LPNMCBEDRAGBEGINA pdba = (LPNMCBEDRAGBEGINA) pnmhdr;

				// Don't unthunk the string since that destroys unicode round-trip
				// and the client shouldn't be modifying it anyway.
				// ConvertAToWN(pci->uiCodePage, pdbw->szText, ARRAYSIZE(pdbw->szText),
				//              pdba->szText, -1);
				LocalFree(pdba);
			}
			break;

			case CBEN_GETDISPINFOA:
			{
				PNMCOMBOBOXEXW pnmcbeW;

				pnmcbeW = (PNMCOMBOBOXEXW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)(pnmcbeW->ceItem.pszText), -1);

				if (pvThunk2)
				{
					LocalFree(pvThunk2);
				}
				pnmcbeW->ceItem.pszText = (LPWSTR)pvThunk1;

			}
			break;


			case HDN_GETDISPINFOA:
			{
				LPNMHDDISPINFOW pHDDispInfoW;

				pHDDispInfoW = (LPNMHDDISPINFOW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)(pHDDispInfoW->pszText), -1);

				LocalFree(pHDDispInfoW->pszText);
				pHDDispInfoW->pszText = (LPWSTR)pvThunk1;

			}
			break;

			case TBN_GETBUTTONINFOA:
			{
				LPTBNOTIFYW pTBNW;

				pTBNW = (LPTBNOTIFYW)pnmhdr;
				ConvertAToWN(pci->uiCodePage, (LPWSTR)pvThunk1, dwThunkSize, (LPSTR)(pTBNW->pszText), -1);

				pTBNW->pszText = (LPWSTR)pvThunk1;
				LocalFree(pvThunk2);

			}
			break;


			case TTN_NEEDTEXTA:
			{
				LPTOOLTIPTEXTA lpTTTA = (LPTOOLTIPTEXTA) pnmhdr;
				LPTOOLTIPTEXTW lpTTTW = (LPTOOLTIPTEXTW) pvThunk1;

				ThunkToolTipTextAtoW(lpTTTA, lpTTTW, pci->uiCodePage);
				LocalFree(lpTTTA);
			}
			break;

			case DTN_USERSTRINGA:
			case DTN_WMKEYDOWNA:
			case DTN_FORMATQUERYA:
			{
				FreeProducedString(pvThunk1);
			}
			break;

			case DTN_FORMATA:
			{
				LPNMDATETIMEFORMATA lpDateTimeFormat = (LPNMDATETIMEFORMATA) pnmhdr;

				FreeProducedString(pvThunk1);

				//
				// pszDisplay and szDisplay are special cases.
				//

				if (lpDateTimeFormat->pszDisplay && *lpDateTimeFormat->pszDisplay)
				{

					//
					// if pszDisplay still points at szDisplay then thunk
					// in place.  Otherwise allocate memory and copy the
					// display string.  This buffer will be freeded in monthcal.c
					//

					if (lpDateTimeFormat->pszDisplay == lpDateTimeFormat->szDisplay)
					{
						CHAR szDisplay[64];

						lstrcpynA(szDisplay, lpDateTimeFormat->szDisplay, 64);

						ConvertAToWN(pci->uiCodePage, (LPWSTR)lpDateTimeFormat->szDisplay, 64,
							szDisplay, -1);
					}
					else
					{
						lpDateTimeFormat->pszDisplay =
							(LPSTR) ProduceWFromA(pci->uiCodePage, lpDateTimeFormat->pszDisplay);
					}

				}

			}
			break;

			default:
				/* No thunking needed */
				break;
			}
		}
		return lRet;
	}
	else
#endif
		return (::SendMessage(hwndParent, WM_NOTIFY, (WPARAM)id, (LPARAM)pnmhdr));

#undef pvThunk1
#undef pvThunk2
#undef dwThunkSize
}

LRESULT IUI::IUI_SendNotifyEx(HWND hwndTo, HWND hwndFrom, int code, NMHDR *pnmhdr, BOOL bUnicode)
{
	IUICONTROLINFO ci;

	if (hwndTo == NULL)
	{
		if (::IsWindow(hwndFrom))
		{
			hwndTo = ::GetParent(hwndFrom);
		}
		if (hwndTo == NULL)
		{
			return 0;
		}
	}


	ci.hwndParent = hwndTo;
	ci.hwnd = hwndFrom;
	ci.bUnicode = bUnicode;
#if TODO
	ci.uiCodePage = CP_ACP;
#endif

	return IUI_CCSendNotify(&ci, code, pnmhdr);
}

// common control info helpers
void FAR PASCAL IUI::CIInitialize(LPIUICONTROLINFO lpci, HWND hwnd, LPCREATESTRUCT lpcs)
{
	lpci->hwnd = hwnd;
	lpci->hwndParent = lpcs->hwndParent;
	lpci->style = lpcs->style;
	lpci->uiCodePage = CP_ACP;
	lpci->dwExStyle = lpcs->dwExStyle;

	lpci->bUnicode = lpci->hwndParent &&
		::SendMessage(lpci->hwndParent, WM_NOTIFYFORMAT,
			(WPARAM)lpci->hwnd, NF_QUERY) == NFR_UNICODE;

#ifdef KEYBOARDCUES
	if (lpci->hwndParent)
	{
		LRESULT lRes = SendMessage(lpci->hwndParent, WM_QUERYUISTATE, 0, 0);
		lpci->wUIState = LOWORD(lRes);
	}
#endif
}

// strips out the accelerators.  they CAN be the same buffers.
void PASCAL IUI::StripAccelerators(LPTSTR lpszFrom, LPTSTR lpszTo, BOOL fAmpOnly)
{

	BOOL fRet = FALSE;

	while (*lpszTo = *lpszFrom)
	{
#if !defined(UNICODE)  //  && defined(DBCS)
		if (IsDBCSLeadByte(*lpszFrom))
		{
			(*((WORD FAR *)lpszTo)) = (*((WORD FAR *)lpszFrom));
			lpszTo += 2;
			lpszFrom += 2;
			continue;
		}
#endif
		if (!fAmpOnly && (IUIGetGloabelVar()->g_fDBCSInputEnabled))
		{
			if (*lpszFrom == TEXT('(') && *(lpszFrom + 1) == CH_PREFIX)
			{
				int i;
				LPTSTR psz = lpszFrom + 2;

				for (i = 0; i < 2 && *psz; i++, psz = FastCharNext(psz))
					;


				if (*psz == '\0')
				{
					*lpszTo = 0;
					break;
				}
				else if (i == 2 && *psz == TEXT(')'))
				{
					lpszTo--;
					lpszFrom = psz + 1;
					continue;
				}
			}
		}

		if (*lpszFrom == TEXT('\t'))
		{
			*lpszTo = TEXT('\0');
			break;
		}

		if ((*lpszFrom++ != CH_PREFIX) || (*lpszFrom == CH_PREFIX))
		{
			lpszTo++;
		}
	}
}

BOOL FAR PASCAL IUI::MGetTextExtent(HDC hdc, LPCTSTR lpstr, int cnt, int FAR *pcx, int FAR *pcy)
{
	BOOL fSuccess;
	SIZE size = {0, 0};

	if (cnt == -1)
	{
		cnt = lstrlen(lpstr);
	}

	fSuccess = GetTextExtentPoint(hdc, lpstr, cnt, &size);
	if (pcx)
	{
		*pcx = size.cx;
	}
	if (pcy)
	{
		*pcy = size.cy;
	}

	return fSuccess;
}

void IUI::SlideAnimate(HWND hwnd, LPCRECT prc)
{
	DWORD dwPos, dwFlags;

	dwPos = GetMessagePos();
	if (GET_Y_LPARAM(dwPos) > prc->top + (prc->bottom - prc->top) / 2)
	{
		dwFlags = AW_VER_NEGATIVE;
	}
	else
	{
		dwFlags = AW_VER_POSITIVE;
	}

	AnimateWindow(hwnd, CMS_TOOLTIP, dwFlags | AW_SLIDE);
}

STDAPI_(void) IUI::CoolTooltipBubble(IN HWND hwnd, IN LPCRECT prc, BOOL fAllowFade, BOOL fAllowAnimate)
{
	_ASSERT(prc);

	BOOL g_bRunOnNT5 = TRUE;
	BOOL g_bRunOnMemphis = TRUE;
	if (g_bRunOnNT5 || g_bRunOnMemphis)
	{
		// Memphis doesn't support the tooltip SPI's, so we piggyback
		// off of SPI_GETSCREENREADER instead.  Note that we want to
		// animate if SPI_GETSCREENREADER is >off<, so we need to do some
		// flippery.  Fortunately, the compiler will optimize all this out.
		// 如果开启“屏幕阅读（讲述人）”，则Tooltips不动画弹出
		BOOL fScreenRead = FALSE;
		BOOL fAnimate;
		SystemParametersInfo(SPI_GETSCREENREADER, 0, &fScreenRead, 0);
		fAnimate = !fScreenRead;

		if (fAnimate)
		{
			if (fAnimate && fAllowFade)
			{
				// 淡入淡出弹出Tooltips
				AnimateWindow(hwnd, CMS_TOOLTIP, AW_BLEND);
			}
			else if (fAllowAnimate)
			{
				// 滑出Tooltips
				SlideAnimate(hwnd, prc);
			}
			else
			{
				goto UseSetWindowPos;
			}
		}
		else
		{
			goto UseSetWindowPos;
		}
	}
	else
	{
UseSetWindowPos:
		// 直接弹出Tooltips
		::SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
	}
}

////-----------------------------------------------------------------------------
//// GetWindowSubclass
////
//// this procedure retrieves the reference data for the specified window
//// subclass callback
////
////-----------------------------------------------------------------------------
//BOOL IUI::GetWindowSubclass(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass,
//	DWORD_PTR *pdwRefData)
//{
//	SUBCLASS_HEADER *pHeader;
//	SUBCLASS_CALL *pCall;
//	BOOL fResult = FALSE;
//	DWORD_PTR dwRefData = 0;
//
//	//
//	// sanity
//	//
//	if (!IsWindow(hWnd))
//	{
//		AssertMsg(FALSE, TEXT("error: GetWindowSubclass - %08X not a window"), hWnd);
//		goto ReturnResult;
//	}
//
//	//
//	// more sanity
//	//
//	if (!pfnSubclass
//#ifdef DEBUG
//		|| IsBadCodePtr((PROC)pfnSubclass)
//#endif
//		)
//	{
//		AssertMsg(FALSE, TEXT("error: GetWindowSubclass - invalid callback %08X"), pfnSubclass);
//		goto ReturnResult;
//	}
//
//#ifdef FREETHREADEDSUBCLASSGOOP
//	ENTERCRITICAL;
//#else
//	ASSERT(IsWindowOnCurrentThread(hWnd));
//#endif
//
//	//
//	// if we've subclassed it and they are a client then get the refdata
//	//
//	if (((pHeader = GetSubclassHeader(hWnd)) != NULL) &&
//		((pCall = FindCallRecord(pHeader, pfnSubclass, uIdSubclass)) != NULL))
//	{
//		//
//		// fetch the refdata and note success
//		//
//		dwRefData = pCall->dwRefData;
//		fResult = TRUE;
//	}
//
//#ifdef FREETHREADEDSUBCLASSGOOP
//	LEAVECRITICAL;
//#else
//	ASSERT(IsWindowOnCurrentThread(hWnd));
//#endif
//
//	//
//	// we always fill in/zero pdwRefData regradless of result
//	//
//ReturnResult:
//	if (pdwRefData)
//		*pdwRefData = dwRefData;
//
//	return fResult;
//}

// lprcParentClip相对于hCtrl父控件的裁剪区域
int IUI::DrawControlAndChildren(HWLWND hCtrl, HDC hMemDC, LPCRECT lprcParentClip)
{
	if (!IsWindowVisible(hCtrl))
	{
		return -1;
	}

	//
	// 1. 修改视口，使孩子在绘制的时候，相对于孩子，从(0, 0)开始，
	// 就像绘制到孩子自己的客户端一样。
	//
	CIUIRect rcCtrlToHWND;
	GetWindowRectToHWND(hCtrl, rcCtrlToHWND);
	SetViewportOrgEx(hMemDC, rcCtrlToHWND.left, rcCtrlToHWND.top, NULL);

	//
	// 2. 计算裁剪区域，并映射到控件上。
	//
	BOOL bParentClip = FALSE;
	HWLWND hParentCtrl = NULL;
	hParentCtrl = GetParent(hCtrl);
	if (hParentCtrl != NULL)
	{
		bParentClip = IsClip(hParentCtrl);
	}

	// 相对于hCtrl的裁剪区域
	CIUIRect rcCtrlClip;

	if (bParentClip)
	{
		CIUIRect rcCtrlToParent;
		GetWindowRectToParent(hCtrl, rcCtrlToParent);

		BOOL bRet = IntersectRect(rcCtrlClip, lprcParentClip, rcCtrlToParent);
		if (bRet)
		{
			// 执行到这里rcCtrlClip是相对hCtrl父控件的坐标
			// 把它转成相对hCtrl的坐标

			// 把剪裁区域，映射到控件上
			if (hParentCtrl != NULL)
			{
				ClientToScreen(hParentCtrl, rcCtrlClip);
				ScreenToClient(hCtrl, rcCtrlClip);
			}
		}
	}
	else
	{
		GetClientRect(hCtrl, rcCtrlClip);
	}

	//
	// 3. 设置客户区裁剪。 在界面编辑器编辑时，不应该设置裁剪
	//
	CWLWndMgr::SetClip(hCtrl, hMemDC, TRUE);

	// 只绘制Root
	LRESULT lr = IUI::SendMessage(hCtrl, WM_PRINT, (WPARAM)hMemDC, (LPARAM)(LPCRECT)rcCtrlClip);

	// 绘制孩子时，以控件整个区域为裁剪区域，因为滚动条在非客户区
	if (IsClip(hCtrl))
	{
		CWLWndMgr::SetClip(hCtrl, hMemDC, FALSE);
	}
	else
	{
		SelectClipRgn(hMemDC, NULL);
	}

	// 通过向孩子向送WM_PRINT消息，递归绘制孩子，绘制孩子的任务，不应该由CWLWnd执行。
	// HWLWND只管绘制自己
	HWLWND hChild = GetWindow(hCtrl, GW_CHILD);
	while (hChild != NULL)
	{
		DrawControlAndChildren(hChild, hMemDC, rcCtrlClip);

		// 得到一下个子控件
		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}

	// 重置视口原点
	SetViewportOrgEx(hMemDC, 0, 0, NULL);

	AfterChildrenPaint(hCtrl, hMemDC);

	// 清空裁剪区域
	if (IsClip(hCtrl))
	{
		// IUITRACE(_T("%s(%08x) SelectClipRgn to NULL\n"), pCtrl->GetClassName(), pCtrl);

		SelectClipRgn(hMemDC, NULL);
	}

	return SUCCEEDED(lr) ? 0 : -1;
}

int _BTN::Release()
{
	_LABEL::Release();

	ImageManager::ReleaseIUIImage(m_himgCombineFg);
	m_himgCombineFg = NULL;

	ImageManager::ReleaseIUIImage(m_himgFocusedImage);
	m_himgFocusedImage = NULL;

	for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
	{
		ImageManager::ReleaseIUIImage(m_himgBk[i]);
		m_himgBk[i] = NULL;

		ImageManager::ReleaseIUIImage(m_himgFg[i]);
		m_himgFg[i] = NULL;

		ReleaseIUIFontInternal(m_hIUIFont[i]);
		m_hIUIFont[i] = NULL;

		ImageManager::ReleaseIUIImage(m_himgBkC[i]);
		m_himgBkC[i] = NULL;
		ImageManager::ReleaseIUIImage(m_himgFgC[i]);
		m_himgFgC[i] = NULL;

		ReleaseIUIFontInternal(m_hIUIFontC[i]);
		m_hIUIFontC[i] = NULL;
	}

	return 0;
}

int tagCBox::Release()
{
	hwnd->ci.Release();
	hwnd = NULL;

	int i = 0;
	for (i = 0; i < COMBINEIMAGESIZE4; ++i)
	{
		ReleaseIUIImage(m_himgBk[i]);
		m_himgBk[i] = NULL;

		ReleaseIUIFontInternal(m_hIUIFont[i]);
		m_hIUIFont[i] = NULL;
	}

	ReleaseIUIImage(m_himgCombineButtonBk);
	m_himgCombineButtonBk = NULL;

	for (i = 0; i < COMBINEIMAGESIZE4 + 1; ++i)
	{
		ReleaseIUIImage(m_himgButtonBk[i]);
		m_himgButtonBk[i] = NULL;
	}

	DestroyWindow(spwndEdit);
	::DestroyWindow(spwndList);

	return 0;
}

int REBox::Release()
{
	hwnd->ci.Release();
	hwnd = NULL;

	if (m_pTextHost != NULL)
	{
		delete m_pTextHost;
		m_pTextHost = NULL;
	}

	for (int i = 0; i < 4; ++i)
	{
		ReleaseIUIImage(m_himgBk[i]);
		m_himgBk[i] = NULL;
	}

	return 0;
}

int TB::Release()
{
	delete m_pvSheets;
	m_pvSheets = NULL;

	return 0;
}

int SBWND::Release()
{
	hwnd->ci.Release();
	hwnd = NULL;

	ReleaseIUIImage(m_himgCombineButton1Bk);
	m_himgCombineButton1Bk = NULL;
	ReleaseIUIImage(m_himgCombineButton2Bk);
	m_himgCombineButton2Bk = NULL;
	ReleaseIUIImage(m_himgCombineThumbBk);
	m_himgCombineThumbBk = NULL;

	for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
	{
		ReleaseIUIImage(m_himgGroove[i]);
		m_himgGroove[i] = NULL;
		ReleaseIUIImage(m_himgButton1Bk[i]);
		m_himgButton1Bk[i] = NULL;
		ReleaseIUIImage(m_himgButton2Bk[i]);
		m_himgButton2Bk[i] = NULL;
		ReleaseIUIImage(m_himgThumbBk[i]);
		m_himgThumbBk[i] = NULL;
	}

	return 0;
}

int _DOCK::Release()
{
	_LABEL::Release();

	m_bLastChildFill = TRUE;
	m_pvChildren->clear();

	return 0;
}

int SP::Release()
{
	_LABEL::Release();

	m_eDirection = D_LEFT;
	m_pvChildren->clear();

	return 0;
}

int DATEPICK::Release()
{
	hwnd->ci.Release();
	hwnd = NULL;

	for (int i = 0; i < COMBINEIMAGESIZE4; ++i)
	{
		ReleaseIUIImage(m_himgBk[i]);
		m_himgBk[i] = NULL;
	}

	return 0;
}

int IUI::MakeControlMargin(
	HWND hParent,
	HWLWND pParent,
	int x,
	int y,
	int nWidth,
	int nHeight,
	__out LPRECT lprcMargin)
{
	if (lprcMargin == NULL)
	{
		return -1;
	}

	CIUIRect rcParent;
	if (pParent != NULL)
	{
		if (GetClassName(pParent) == IUI_DOCKPANEL
			|| GetClassName(pParent) == IUI_STACKPANEL)
		{
			// 当控件的父窗口是DockPanel等面板时，忽略控件在界面编辑器中生成的Control Margin。
			memset(lprcMargin, 0, sizeof(RECT));
			return 0;
		}

		IUI::GetClientRect(pParent, rcParent);
	}
	else
	{
		::GetClientRect(hParent, rcParent);
	}

	lprcMargin->left = x;
	lprcMargin->top = y;
	lprcMargin->right = rcParent.right - (x + nWidth);
	lprcMargin->bottom = rcParent.bottom - (y + nHeight);

	return 0;
}

int IUI::MakeControlMargin(
	HWND hParent,
	HWLWND pParent,
	__in LPCRECT lprcControl,
	__out LPRECT lprcMargin)
{
	return MakeControlMargin(hParent, pParent,
			lprcControl->left, lprcControl->top,
			RECTWIDTH(*lprcControl), RECTHEIGHT(*lprcControl),
			lprcMargin);
}

void CConvertStr::Free()
{
	//TRACEBEGIN(TRCSUBSYSWRAP, TRCSCOPEINTERN, "CConvertStr::Free");

	if (_pstr != _ach && HIWORD(_pstr) != 0)
	{
		delete[] _pstr;
	}

	_pstr = NULL;
}

//+---------------------------------------------------------------------------
//
//  Member:     CStrIn::CStrIn
//
//  Synopsis:   Inits the class.
//
//  NOTE:       Don't inline these functions or you'll increase code size
//              by pushing -1 on the stack for each call.
//
//----------------------------------------------------------------------------

CStrIn::CStrIn(LPCWSTR pwstr)
{
	//TRACEBEGIN(TRCSUBSYSWRAP, TRCSCOPEINTERN, "CStrIn::CStrIn");

	Init(pwstr, -1);
}

CStrIn::CStrIn(LPCWSTR pwstr, int cwch)
{
	//TRACEBEGIN(TRCSUBSYSWRAP, TRCSCOPEINTERN, "CStrIn::CStrIn");

	Init(pwstr, cwch);
}

//+---------------------------------------------------------------------------
//
//  Function:   MbcsFromUnicode
//
//  Synopsis:   Converts a string to MBCS from Unicode.
//
//  Arguments:  [pstr]  -- The buffer for the MBCS string.
//              [cch]   -- The size of the MBCS buffer, including space for
//                              NULL terminator.
//
//              [pwstr] -- The Unicode string to convert.
//              [cwch]  -- The number of characters in the Unicode string to
//                              convert, including NULL terminator.  If this
//                              number is -1, the string is assumed to be
//                              NULL terminated.  -1 is supplied as a
//                              default argument.
//				[codepage]	-- the code page to use (CP_ACP is default)
//				[flags]		-- indicates if WCH_EMBEDDING should be treated
//							with special handling.
//
//  Returns:    If [pstr] is NULL or [cch] is 0, 0 is returned.  Otherwise,
//              the number of characters converted, including the terminating
//              NULL, is returned (note that converting the empty string will
//              return 1).  If the conversion fails, 0 is returned.
//
//  Modifies:   [pstr].
//
//----------------------------------------------------------------------------

enum UN_FLAGS
{
	UN_NOOBJECTS = 1,
	UN_CONVERT_WCH_EMBEDDING = 2
};

int MbcsFromUnicode(LPSTR pstr, int cch, LPCWSTR pwstr,
	int cwch = -1, UINT codepage = CP_ACP,
	UN_FLAGS flags = UN_CONVERT_WCH_EMBEDDING);

int
MbcsFromUnicode(LPSTR pstr, int cch, LPCWSTR pwstr, int cwch, UINT codepage,
	UN_FLAGS flags)
{
	//TRACEBEGIN(TRCSUBSYSWRAP, TRCSCOPEINTERN, "MbcsFromUnicode");

	int ret;
	LPWSTR pwstrtemp;
	LONG i;
	CTempWcharBuf twcb;

	_ASSERT(cch >= 0);
	if (!pstr || cch == 0)
	{
		return 0;
	}

	_ASSERT(pwstr);
	_ASSERT(cwch == -1 || cwch > 0);

	// if we have to convert WCH_EMBEDDINGs, scan through and turn
	// them into spaces.  This is necessary for richedit1.0 compatibity,
	// as WideCharToMultiByte will turn WCH_EMBEDDING into a
	// '?'
	if (flags == UN_CONVERT_WCH_EMBEDDING)
	{
		if (cwch == -1)
		{
			cwch = wcslen(pwstr) + 1;
		}

		pwstrtemp = twcb.GetBuf(cwch);

		if (pwstrtemp)
		{
			for (i = 0; i < cwch; i++)
			{
				pwstrtemp[i] = pwstr[i];

				if (pwstr[i] == WCH_EMBEDDING)
				{
					pwstrtemp[i] = L' ';
				}
			}

			pwstr = pwstrtemp;
		}
	}

	ret = WideCharToMultiByte(codepage, 0, pwstr, cwch, pstr, cch, NULL, NULL);

	return ret;
}

//+---------------------------------------------------------------------------
//
//  Member:     CStrIn::Init
//
//  Synopsis:   Converts a LPWSTR function argument to a LPSTR.
//
//  Arguments:  [pwstr] -- The function argument.  May be NULL or an atom
//                              (HIWORD(pwstr) == 0).
//
//              [cwch]  -- The number of characters in the string to
//                          convert.  If -1, the string is assumed to be
//                          NULL terminated and its length is calculated.
//
//  Modifies:   [this]
//
//----------------------------------------------------------------------------

void
CStrIn::Init(LPCWSTR pwstr, int cwch)
{
	//TRACEBEGIN(TRCSUBSYSWRAP, TRCSCOPEINTERN, "CStrIn::Init");

	int cchBufReq;

	_cchLen = 0;

	// Check if string is NULL or an atom.
	if (HIWORD(pwstr) == 0)
	{
		_pstr = (LPSTR)pwstr;
		return;
	}

	_ASSERT(cwch == -1 || cwch > 0);

	//
	// Convert string to preallocated buffer, and return if successful.
	//

	_cchLen = MbcsFromUnicode(_ach, ARRAY_SIZE(_ach), pwstr, cwch);

	if (_cchLen > 0)
	{
		if (_ach[_cchLen - 1] == 0)
		{
			_cchLen--;    // account for terminator
		}
		_pstr = _ach;
		return;
	}

	//
	// Alloc space on heap for buffer.
	//

	//TRACEINFOSZ("CStrIn: Allocating buffer for wrapped function argument.");
	//UniWrapBreak();

	cchBufReq = WideCharToMultiByte(
			CP_ACP, 0, pwstr, cwch, NULL, 0, NULL, NULL);

	_ASSERT(cchBufReq > 0);
	_pstr = new char[cchBufReq];
	if (!_pstr)
	{
		// On failure, the argument will point to the empty string.
		//TRACEINFOSZ("CStrIn: No heap space for wrapped function argument.");
		//UniWrapBreak();
		_ach[0] = 0;
		_pstr = _ach;
		return;
	}

	_ASSERT(HIWORD(_pstr));
	_cchLen = -1 + MbcsFromUnicode(_pstr, cchBufReq, pwstr, cwch);

	_ASSERT(_cchLen >= 0);
}

/*
*	CTempBuf::GetBuf
*
*	@mfunc	Get a buffer for temporary use
*
*	@rdesc	Pointer to buffer if one could be allocated otherwise NULL.
*
*
*/
void *CTempBuf::GetBuf(
	LONG cb)				//@parm Size of buffer needed in bytes
{
	if (_cb >= cb)
	{
		// Currently allocated buffer is big enough so use it
		return _pv;
	}

	// Free our current buffer
	FreeBuf();

	// Allocate a new buffer if we can
	_pv = new BYTE[cb];

	if (NULL == _pv)
	{
		// Could not allocate a buffer so reset to our initial state and
		// return NULL.
		Init();
		return NULL;
	}

	// Store the size of our new buffer.
	_cb = cb;

	// Returnt he pointer to the buffer.
	return _pv;
}

/*
*	CTempBuf::Init
*
*	@mfunc	Set object to its initial state using the stack buffer
*
*/
void CTempBuf::Init()
{
	_pv = (void *)&_chBuf[0];
	_cb = MAX_STACK_BUF;
}

/*
*	CTempBuf::FreeBuf
*
*	@mfunc	Free an allocated buffer if there is one
*
*/
void CTempBuf::FreeBuf()
{
	if (_pv != &_chBuf[0])
	{
		delete _pv;
	}
}

int IUI::CCLoadStringEx(UINT uID, LPWSTR lpBuffer, int nBufferMax, WORD wLang)
{
	HINSTANCE hIns = IUIGetInstanceHandle();
	return -1;
	//	return CCLoadStringExInternal(hIns, uID, lpBuffer, nBufferMax, wLang);
}

/*
-    LIncrWord
-
*    Purpose:
*        Increment (or decrement) an integer by a specified amount,
*        given the constraints nMic and nMac.
*        Returns the amount of carry into the following (or preceding)
*        field, or zero if none.
*
*        Intended for use with incrementing date/times.
*
*    Arguments:
*        pn        Pointer to integer to be modified.
*        nDelta    Amount by which to modify *pn; may be positive,
*                negative or zero.
*        nMic    Minimum value for *pn;  if decrementing below this,
*                a carry is performed.
*        nMac    Maximum value for *pn;  if incrementing above this,
*                a carry is performed.
*
*    Returns:
*        Zero if modification done within constraints, otherwise the
*        amount of carry (positive in incrementing, negative if
*        decrementing).
*
*/
LONG LIncrWord(WORD *pn, LONG nDelta, int nMic, int nMac)
{
	LONG lNew, lIncr;

	lIncr = 0;
	lNew = *pn + nDelta;

	while (lNew >= nMac)
	{
		lNew -= nMac - nMic;
		lIncr++;
	}

	if (!lIncr)
	{
		while (lNew < nMic)
		{
			lNew += nMac - nMic;
			lIncr--;
		}
	}

	*pn = (WORD)lNew;

	return (lIncr);
}

int IUI::CmpSystemtime(const SYSTEMTIME *pst1, const SYSTEMTIME *pst2)
{
	int iRet;

	if (pst1->wYear < pst2->wYear)
	{
		iRet = -1;
	}
	else if (pst1->wYear > pst2->wYear)
	{
		iRet = 1;
	}
	else if (pst1->wMonth < pst2->wMonth)
	{
		iRet = -1;
	}
	else if (pst1->wMonth > pst2->wMonth)
	{
		iRet = 1;
	}
	else if (pst1->wDay < pst2->wDay)
	{
		iRet = -1;
	}
	else if (pst1->wDay > pst2->wDay)
	{
		iRet = 1;
	}
	else if (pst1->wHour < pst2->wHour)
	{
		iRet = -1;
	}
	else if (pst1->wHour > pst2->wHour)
	{
		iRet = 1;
	}
	else if (pst1->wMinute < pst2->wMinute)
	{
		iRet = -1;
	}
	else if (pst1->wMinute > pst2->wMinute)
	{
		iRet = 1;
	}
	else if (pst1->wSecond < pst2->wSecond)
	{
		iRet = -1;
	}
	else if (pst1->wSecond > pst2->wSecond)
	{
		iRet = 1;
	}
	else
	{
		iRet = 0;
	}

	return (iRet);
}

int IUI::CmpDate(const SYSTEMTIME *pst1, const SYSTEMTIME *pst2)
{
	int iRet;

	if (pst1->wYear < pst2->wYear)
	{
		iRet = -1;
	}
	else if (pst1->wYear > pst2->wYear)
	{
		iRet = 1;
	}
	else if (pst1->wMonth < pst2->wMonth)
	{
		iRet = -1;
	}
	else if (pst1->wMonth > pst2->wMonth)
	{
		iRet = 1;
	}
	else if (pst1->wDay < pst2->wDay)
	{
		iRet = -1;
	}
	else if (pst1->wDay > pst2->wDay)
	{
		iRet = 1;
	}
	else
	{
		iRet = 0;
	}

	return (iRet);
}

void IUI::IncrSystemTime(SYSTEMTIME *pstSrc, SYSTEMTIME *pstDest, LONG nDelta, LONG flag)
{
	int cdyMon;

	if (pstSrc != pstDest)
	{
		*pstDest = *pstSrc;
	}

	switch (flag)
	{
	case INCRSYS_SECOND:
		if (!(nDelta = LIncrWord(&pstDest->wSecond, nDelta, 0, 60)))
		{
			break;
		}

	case INCRSYS_MINUTE:
		if (!(nDelta = LIncrWord(&pstDest->wMinute, nDelta, 0, 60)))
		{
			break;
		}

	case INCRSYS_HOUR:
		if (!(nDelta = LIncrWord(&pstDest->wHour, nDelta, 0, 24)))
		{
			break;
		}

	case INCRSYS_DAY:
IDTday:
		if (nDelta >= 0)
		{
			cdyMon = GetDaysForMonth(pstDest->wYear, pstDest->wMonth);
			while (pstDest->wDay + nDelta > cdyMon)
			{
				nDelta -= cdyMon + 1 - pstDest->wDay;
				pstDest->wDay = 1;
				IncrSystemTime(pstDest, pstDest, 1, INCRSYS_MONTH);
				cdyMon = GetDaysForMonth(pstDest->wYear, pstDest->wMonth);
			}
		}
		else
		{
			while (pstDest->wDay <= -nDelta)
			{
				nDelta += pstDest->wDay;
				IncrSystemTime(pstDest, pstDest, -1, INCRSYS_MONTH);
				cdyMon = GetDaysForMonth(pstDest->wYear, pstDest->wMonth);
				pstDest->wDay = (WORD)cdyMon;
			}
		}

		pstDest->wDay += (WORD)nDelta;
		break;

	case INCRSYS_MONTH:
		if (!(nDelta = LIncrWord(&pstDest->wMonth, nDelta, 1, 13)))
		{
			cdyMon = GetDaysForMonth(pstDest->wYear, pstDest->wMonth);
			if (pstDest->wDay > cdyMon)
			{
				pstDest->wDay = (WORD)cdyMon;
			}
			break;
		}

	case INCRSYS_YEAR:
		pstDest->wYear += (WORD)nDelta;
		cdyMon = GetDaysForMonth(pstDest->wYear, pstDest->wMonth);
		if (pstDest->wDay > cdyMon)
		{
			pstDest->wDay = (WORD)cdyMon;
		}
		break;

	case INCRSYS_WEEK:
		nDelta *= 7;
		goto IDTday;
		break;
	}
}


int mpcdymoAccum[13] =
{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

/*
-    CdyBetweenYmd
-
*    Purpose:
*        Calculate the number of days between two dates as expressed
*        in YMD's.
*
*    Parameters:
*        pymdStart        start day of range.
*        pymdEnd            end day of range.
*
*    Returns:
*        Number of days between two dates.  The number
*        of days does not include the starting day, but does include
*        the last day. ie 1/24/1990-1/25/1990 = 1 day.
*/
DWORD IUI::DaysBetweenDates(const SYSTEMTIME *pstStart, const SYSTEMTIME *pstEnd)
{
	DWORD cday;
	WORD yr;

	// Calculate number of days between the start month/day and the
	// end month/day as if they were in the same year - since cday
	// is unsigned, cday could be really large if the end month/day
	// is before the start month.day.
	// This will be cleared up when we account for the days between
	// the years.
	_ASSERT(pstEnd->wMonth >= 1 && pstEnd->wMonth <= 12);
	cday = mpcdymoAccum[pstEnd->wMonth - 1] - mpcdymoAccum[pstStart->wMonth - 1] +
		pstEnd->wDay - pstStart->wDay;
	yr = pstStart->wYear;

	// Check to see if the start year is before the end year,
	// and if the end month is after February and
	// if the end year is a leap year, then add an extra day
	// for to account for Feb. 29 in the end year.
	if (((yr < pstEnd->wYear) || (pstStart->wMonth <= 2)) &&
		pstEnd->wMonth > 2 &&
		(pstEnd->wYear & 03) == 0 &&
		(pstEnd->wYear <= 1750 || pstEnd->wYear % 100 != 0 || pstEnd->wYear % 400 == 0))
	{
		cday++;
	}

	// Now account for the leap years in between the start and end dates
	// as well as accounting for the days in each year.
	if (yr < pstEnd->wYear)
	{
		// If the start date is before march and the start year is
		// a leap year then add an extra day to account for Feb. 29.
		if (pstStart->wMonth <= 2 &&
			(yr & 03) == 0 &&
			(yr <= 1750 || yr % 100 != 0 || yr % 400 == 0))
		{
			cday++;
		}

		// Account for the days in each year (disregarding leap years).
		cday += 365;
		yr++;

		// Keep on accounting for the days in each year including leap
		// years until we reach the end year.
		while (yr < pstEnd->wYear)
		{
			cday += 365;
			if ((yr & 03) == 0 && (yr <= 1750 || yr % 100 != 0 || yr % 400 == 0))
			{
				cday++;
			}
			yr++;
		}
	}

	return (cday);
}

/*
-    DowStartOfYrMo
-
*    Purpose:
*        Find the day of the week the indicated month begins on
*
*    Parameters:
*        yr        year, must be > 0
*        mo        month, number 1-12
*
*    Returns:
*        day of the week (0-6) on which the month begins
*        (0 = Sunday, 1 = Monday etc.)
*/
int IUI::GetStartDowForMonth(int yr, int mo)
{
	int dow;

	// we want monday = 0, sunday = 6
	// dow = 6 + (yr - 1) + ((yr - 1) >> 2);
	dow = 5 + (yr - 1) + ((yr - 1) >> 2);
	if (yr > 1752)
	{
		dow += ((yr - 1) - 1600) / 400 - ((yr - 1) - 1700) / 100 - 11;
	}
	else if (yr == 1752 && mo > 9)
	{
		dow -= 11;
	}
	dow += mpcdymoAccum[mo - 1];
	if (mo > 2 && (yr & 03) == 0 && (yr <= 1750 || yr % 100 != 0 || yr % 400 == 0))
	{
		dow++;
	}
	dow %= 7;

	return (dow);
}

int IUI::DowFromDate(const SYSTEMTIME *pst)
{
	int dow;

	dow = GetStartDowForMonth(pst->wYear, pst->wMonth);
	dow = (dow + pst->wDay - 1) % 7;

	return (dow);
}

int IUI::GetDaysForMonth(int yr, int mo)
{
	int cdy;

	if (yr == 1752 && mo == 9)
	{
		return (19);
	}
	cdy = mpcdymoAccum[mo] - mpcdymoAccum[mo - 1];
	if (mo == 2 && (yr & 03) == 0 && (yr <= 1750 || yr % 100 != 0 || yr % 400 == 0))
	{
		cdy++;
	}

	return (cdy);
}

/*
-    NweekNumber
-
*    Purpose:
*        Calculates week number in which a given date occurs, based
*        on a specified start-day of week.
*        Adjusts based on how a calendar would show this week
*        (ie. week 53 is probably week 1 on the calendar).
*
*    Arguments:
*        pdtm            Pointer to date in question
*        dowStartWeek    Day-of-week on which weeks starts (0 - 6).
*
*    Returns:
*        Week number of the year, in which *pdtr occurs.
*
*/
// TODO: this currently ignores woyFirst
// it uses the 1st week containing 4+ days as the first week (woyFirst = 2)
// need to make appropriate changes so it handles woyFirst = 0 and = 1...
int IUI::GetWeekNumber(const SYSTEMTIME *pst, int dowFirst, int woyFirst)
{
	int day, ddow, ddowT, nweek;
	SYSTEMTIME st;

	st.wYear = pst->wYear;
	st.wMonth = 1;
	st.wDay = 1;

	ddow = GetStartDowForMonth(st.wYear, st.wMonth) - dowFirst;
	if (ddow < 0)
	{
		ddow += 7;
	}

	if (pst->wMonth == 1 && pst->wDay < 8 - ddow)
	{
		nweek = 0;
	}
	else
	{
		if (ddow)
		{
			st.wDay = 8 - ddow;
		}

		nweek = (DaysBetweenDates(&st, pst) / 7) + 1;
	}
	if (ddow && ddow <= 3)
	{
		nweek++;
	}

	// adjust if necessary for calendar
	if (!nweek)
	{
		if (!ddow)
		{
			return (1);
		}

		// check what week Dec 31 is on
		st.wYear--;
		st.wMonth = 12;
		st.wDay = 31;
		return (GetWeekNumber(&st, dowFirst, woyFirst));
	}
	else if (nweek >= 52)
	{
		ddowT = (GetStartDowForMonth(pst->wYear, pst->wMonth) +
				pst->wDay - 1 + 7 - dowFirst) % 7;
		day = pst->wDay + (7 - ddowT);
		if (day > 31 + 4)
		{
			nweek = 1;
		}
	}

	return (nweek);
}

// ignores day of week and time-related fields...
// BUGBUG also validate years in range
BOOL IUI::IsValidDate(const SYSTEMTIME *pst)
{
	int cDay;

	if (pst && pst->wMonth >= 1 && pst->wMonth <= 12)
	{
		cDay = GetDaysForMonth(pst->wYear, pst->wMonth);
		if (pst->wDay >= 1 && pst->wDay <= cDay)
		{
			return (TRUE);
		}
	}
	return (FALSE);
}

// ignores milliseconds and date-related fields...
BOOL IUI::IsValidTime(const SYSTEMTIME *pst)
{
	return (pst->wHour <= 23 &&
			pst->wMinute <= 59 &&
			pst->wSecond <= 59);
}

// ignores day of week
BOOL IUI::IsValidSystemtime(const SYSTEMTIME *pst)
{
	if (pst && pst->wMonth >= 1 && pst->wMonth <= 12)
	{
		int cDay = GetDaysForMonth(pst->wYear, pst->wMonth);
		if (pst->wDay >= 1 &&
			pst->wDay <= cDay &&
			pst->wHour <= 23 &&
			pst->wMinute <= 59 &&
			pst->wSecond <= 59 &&
			pst->wMilliseconds < 1000)
		{
			return (TRUE);
		}
	}
	return (FALSE);
}

// @DEVNOTE -- we cannot fire asserts in here as were not completely initialized...
void IUI::InitNLSProcTable()
{
#ifndef MACPORT

	INT			procCount, maxProcs;

	LibsAndProcs	*pLibAndProcList;

	char			**pProcList;

	HMODULE		hDLL;

	pLibAndProcList = libAndProcList;
	while (*pLibAndProcList->libName)
	{
		hDLL = LoadLibraryA(pLibAndProcList->libName);
		if (NULL != hDLL)
		{
			pProcList = pLibAndProcList->procNameList;
			procCount = pLibAndProcList->iProcStart;
			maxProcs = pLibAndProcList[1].iProcStart;
			while (**pProcList && procCount < maxProcs)
			{
				if (NULL == (nlsProcTable[procCount++] = GetProcAddress(hDLL, *pProcList++)))
				{
					goto nextLib;
				}
			}

			*pLibAndProcList->fLoaded = TRUE;	// was static init FALSE
		}
nextLib:
		if (NULL != hDLL)
		{
			FreeLibrary(hDLL);
			hDLL = NULL;
		}
		pLibAndProcList++;
	}
#else // MACPORT

	if (LANG_JAPANESE == PRIMARYLANGID(LANGIDFROMLCID(GetUserDefaultLCID())))
	{
		nlsProcTable[0] = (FARPROC)ImmGetCompositionString;
		nlsProcTable[1] = (FARPROC)ImmGetContext;
		// not supported in crayon v1. nlsProcTable[2]		= (FARPROC) ImmSetCompositionFont;
		// not supported in crayon v1. nlsProcTable[3]		= ImmSetCompositionWindow;
		nlsProcTable[4] = (FARPROC)ImmReleaseContext;
		// not supported in crayon v1. nlsProcTable[5]		= ImmGetProperty;
		// not supported in crayon v1. nlsProcTable[6]		= ImmGetCandidateWindow;
		// not supported in crayon v1. nlsProcTable[7]		= ImmSetCandidateWindow;
		nlsProcTable[8] = (FARPROC)ImmNotifyIME;
		nlsProcTable[9] = (FARPROC)ImmAssociateContext;
		// not supported in crayon v1. nlsProcTable[10]	= ImmGetVirtualKey;
		nlsProcTable[11] = (FARPROC)ImmSetOpenStatus;
		// not supported in crayon v1. nlsProcTable[12]	= ImmGetConversionStatus

		fHaveIMMProcs = TRUE;
	}

#endif
}

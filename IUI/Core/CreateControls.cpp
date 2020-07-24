#include "stdafx.h"
#include <ERRNO.H>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

DWORD CreateControls::GetControlBaseStyle(const CControlProp *pCtrlProp)
{
	DWORD dwStyle = 0;

	if (pCtrlProp->m_bVisible)
	{
		dwStyle |= WS_VISIBLE;
	}
	if (!(bool)pCtrlProp->m_bEnabled)
	{
		dwStyle |= WS_DISABLED;
	}
	if (pCtrlProp->m_bTabStop)
	{
		dwStyle |= WS_TABSTOP;
	}
	if (pCtrlProp->m_bClipChildren)
	{
		dwStyle |= WS_CLIPCHILDREN;
	}

	return dwStyle;
}

HWLWND CreateControls::CreateButton(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CButtonProp *pBtn,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	if (pBtn == NULL || hParent == NULL)
	{
		return NULL;
	}

	DWORD dwStyle = BS_PUSHBUTTON | GetControlBaseStyle(pBtn);
	if (pBtn->m_bDefPushButton)
	{
		dwStyle |= BS_DEFPUSHBUTTON;
	}
	if (pBtn->m_bMultiline)
	{
		dwStyle |= BS_MULTILINE;
	}

	CIUIRect rect = pBtn->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	BOOL bRet = FALSE;
	CIUIProject *pProject = (CIUIProject *)::SendMessage(hParent, WM_GET_PROJECT, 0, 0);
	CIUIString strContent = CControlProp::GetLanguageKey(pProject, pBtn, &bRet);
	if (bRet)
	{
		pBtn->m_strCaption = (LPCTSTR)strContent;
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_BUTTON, pBtn->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pBtn->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	// CWLWnd through DM_GETDEFID to handle default button
	if ((dwStyle & BS_DEFPUSHBUTTON) == BS_DEFPUSHBUTTON)
	{
		// If already has default push button. set its style
		// See the remark section of DM_SETDEFID in MSDN.

		int nRet = (int)::SendMessage(hParent, DM_GETDEFID, 0, 0);
		if (nRet != 0)
		{
			int nHas = HIWORD(nRet);
			int nPreviousID = LOWORD(nRet);
			//CWLWnd *pPreviousBtn = pParent->GetDlgItem(nPreviousID);

			//if (nHas == DC_HASDEFID && pPreviousBtn->GetSafeHwnd() != NULL)
			//{
			//	LONG lStyle = GetWindowLong(pChild->GetSafeHwnd(), GWL_STYLE);
			//	lStyle &= ~BS_DEFPUSHBUTTON;
			//	pPreviousBtn->SendMessage(BM_SETSTYLE, lStyle, MAKELPARAM(FALSE, 0));
			//}
		}

		// BOOL bRet = (BOOL)SendMessage(pParent->GetHostWnd(), DM_SETDEFID, nID, 0);
	}

	return hWnd;
}

HWLWND CreateControls::CreateCheckBox(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CCheckBoxProp *pChk,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// 	int nID = LONG(pChk->m_nID);
	// 	_ASSERT(nID != -1);

	DWORD dwStyle = BS_AUTOCHECKBOX | GetControlBaseStyle(pChk);
	if (pChk->m_bMultiline)
	{
		dwStyle |= BS_MULTILINE;
	}

	CIUIRect rect = pChk->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	BOOL bRet = FALSE;
	CIUIProject *pProject = (CIUIProject *)::SendMessage(hParent, WM_GET_PROJECT, 0, 0);
	CIUIString strContent = CControlProp::GetLanguageKey(pProject, pChk, &bRet);
	if (bRet)
	{
		pChk->m_strCaption = (LPCTSTR)strContent;
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_BUTTON, pChk->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pChk->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateRadioButton(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CRadioButtonProp *pRad,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// Properties
	DWORD dwStyle = BS_AUTORADIOBUTTON | GetControlBaseStyle(pRad);
	if (pRad->m_bGroup)
	{
		dwStyle |= WS_GROUP;
	}
	if (pRad->m_bMultiline)
	{
		dwStyle |= BS_MULTILINE;
	}

	CIUIRect rect = pRad->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	BOOL bRet = FALSE;
	CIUIProject *pProject = (CIUIProject *)::SendMessage(hParent, WM_GET_PROJECT, 0, 0);
	CIUIString strContent = CControlProp::GetLanguageKey(pProject, pRad, &bRet);
	if (bRet)
	{
		pRad->m_strCaption = (LPCTSTR)strContent;
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_BUTTON, pRad->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pRad->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateComboBox(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CComboBoxProp *pCmb,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	CIUIRect rect = pCmb->m_rcControl;

	DWORD dwStyle = GetControlBaseStyle(pCmb);
	switch ((CMB_STYLE)pCmb->m_eComboStyle)
	{
	case CS_SIMPLE:
		dwStyle |= CBS_SIMPLE;
		break;
	case CS_DROPDOWN:
		dwStyle |= CBS_DROPDOWN;
		break;
	case CS_DROPDOWNLIST:
		dwStyle |= CBS_DROPDOWNLIST;
		break;
	default:
		break;
	}
	if (pCmb->m_bVScroll)
	{
		dwStyle |= WS_VSCROLL;
	}
	if (pCmb->m_bAutoHScroll)
	{
		dwStyle |= CBS_AUTOHSCROLL;
	}
	if (pCmb->m_bDisableNoScroll)
	{
		dwStyle |= CBS_DISABLENOSCROLL;
	}
	if (pCmb->m_bSort)
	{
		dwStyle |= CBS_SORT;
	}
	if (pCmb->m_bUppercase)
	{
		dwStyle |= CBS_UPPERCASE;
	}
	if (pCmb->m_bLowercase)
	{
		dwStyle |= CBS_LOWERCASE;
	}

	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_COMBOBOX, pCmb->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pCmb->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateLabel(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CLabelProp *pProp,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	DWORD dwStyle = SS_NOTIFY | GetControlBaseStyle(pProp);

	CIUIRect rect = pProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	BOOL bRet = FALSE;
	CIUIProject *pProject = (CIUIProject *)::SendMessage(hParent, WM_GET_PROJECT, 0, 0);
	CIUIString strContent = CControlProp::GetLanguageKey(pProject, pProp, &bRet);
	if (bRet)
	{
		pProp->m_strCaption = (LPCTSTR)strContent;
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_LABEL, pProp->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateSpinButton(
	__inout CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CSpinButtonProp *pSpn,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pSpn->m_nID);
	//_ASSERT(nID != -1);

	DWORD dwStyle = GetControlBaseStyle(pSpn);
	if (!(bool)pSpn->m_bVertical)
	{
		dwStyle |= UDS_HORZ;
	}
	if (pSpn->m_bAutoBuddy)
	{
		dwStyle |= UDS_AUTOBUDDY;
	}
	if (pSpn->m_bSetBuddyInt)
	{
		dwStyle |= UDS_SETBUDDYINT;
	}
	if (pSpn->m_bNoThousands)
	{
		dwStyle |= UDS_NOTHOUSANDS;
	}
	if (pSpn->m_bWrap)
	{
		dwStyle |= UDS_WRAP;
	}
	if (pSpn->m_bArrowKeys)
	{
		dwStyle |= UDS_ARROWKEYS;
	}
	if (pSpn->m_bHotTrack)
	{
		dwStyle |= UDS_HOTTRACK;
	}
	if (SPNA_LEFT == (SPN_ALIGN)pSpn->m_eAlignMode)
	{
		dwStyle |= UDS_ALIGNLEFT;
	}
	else if (SPNA_RIGHT == (SPN_ALIGN)pSpn->m_eAlignMode)
	{
		dwStyle |= UDS_ALIGNRIGHT;
	}

	CIUIRect rect = pSpn->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_SPINBUTTON, pSpn->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pSpn->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateProgressBar(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CProgressBarProp *pProp,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pProp);
	if (pProp->m_bVertical)
	{
		dwStyle |= PBS_VERTICAL;
	}
	if (pProp->m_bTabStop)
	{
		dwStyle |= WS_TABSTOP;
	}

	CIUIRect rect = pProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_PROGRESSBAR, pProp->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateSlider(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CSliderProp *pSld,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pSld);

	// 包含TBS_FIXEDLENGTH风格的Slider的滑块的尺寸是固定的（这个固定
	// 值默认由水平滚动条的高度算出来, 也可以由程序后期设置），
	// 如果没有这个风格，如果Slider的尺寸较小时，滑块也会自动缩小。
	dwStyle |= TBS_FIXEDLENGTH;

	if (pSld->m_bVertical)
	{
		dwStyle |= TBS_VERT;
	}
	if (pSld->m_bTabStop)
	{
		dwStyle |= WS_TABSTOP;
	}
	if (pSld->m_bThumbPositionMode)
	{
		dwStyle |= TBS_THUMBPOSITION;
	}

	CIUIRect rect = pSld->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_SLIDER, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pSld->m_strName, hInst, (LPVOID)lCreateParam);

	return hWnd;
}

// CSkinHotKeyCtrl *CreateControls::CreateHotKey(HOTPROPERTIES *pHot, CWLWnd *pParent,
// 	HIUIIMAGE himgN, HIUIIMAGE himgH, HIUIIMAGE himgS, HIUIIMAGE himgD)
// {
// 	int nID = LONG(pHot->m_nID);
// 	_ASSERT(nID != -1);
//
// 	CSkinHotKeyCtrl *pChild = new CSkinHotKeyCtrl;
// 	_ASSERT(pChild != NULL);
//
// 	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pHot);
//
// 	CIUIRect rect = pHot->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
// 	if (!pChild->Create(dwStyle, rect, pParent, nID))
// 	{
// 		delete pChild;
// 		return NULL;
// 	}
//
// 	pChild->BindStyle(pHot);
//
// 	bool bSpecifyBackgroundImages = pHot->m_bSpecifyBackgroundImages;
// 	if (!bSpecifyBackgroundImages)
// 	{
// 		SetHotKeyImagesByParent(pHot->m_rcControl, pChild, himgN, himgH, himgS, himgD);
// 	}
//
// 	return pChild;
// }
//
// CSkinIPAddressCtrl *CreateControls::CreateIPAddress(IPAPROPERTIES *pIPA, CWLWnd *pParent,
// 	HIUIIMAGE himgN, HIUIIMAGE himgH, HIUIIMAGE himgS, HIUIIMAGE himgD)
// {
// 	int nID = LONG(pIPA->m_nID);
// 	_ASSERT(nID != -1);
//
// 	CSkinIPAddressCtrl *pChild = new CSkinIPAddressCtrl;
// 	_ASSERT(pChild != NULL);
//
// 	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pIPA);
//
// 	CIUIRect rect = pIPA->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
// 	if (!pChild->Create(dwStyle, rect, pParent, nID))
// 	{
// 		delete pChild;
// 		return NULL;
// 	}
//
// 	pChild->BindStyle(pIPA);
//
// 	bool bSpecifyBackgroundImages = pIPA->m_bSpecifyBackgroundImages;
// 	if (!bSpecifyBackgroundImages)
// 	{
// 		SetIPAddressImagesByParent(pIPA->m_rcControl, pChild, himgN, himgH, himgS, himgD);
// 	}
//
// 	return pChild;
// }
//

HWLWND CreateControls::CreateListView(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CListViewProp *pList,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pList);
	LST_STYLE eListStyle = (LST_STYLE)pList->m_eListStyle;
	switch (eListStyle)
	{
	case LSTS_ICON:
		dwStyle |= LVS_ICON;
		break;
	case LSTS_SMALLICON:
		dwStyle |= LVS_SMALLICON;
		break;
	case LSTS_LIST:
		dwStyle |= LVS_LIST;
		break;
	case LSTS_REPORT:
		dwStyle |= LVS_REPORT;
		break;
	default:
		break;
	}
	switch ((LST_ALIGN)pList->m_eListAlign)
	{
	case 0:
		dwStyle |= LVS_ALIGNTOP;
		break;
	case 1:
		dwStyle |= LVS_ALIGNLEFT;
		break;
	default:
		break;
	}
	switch ((LST_SORT)pList->m_eListSort)
	{
	case 0:
		break;
	case LSTST_ASCENDING:
		dwStyle |= LVS_SORTASCENDING;
		break;
	case LSTST_DESCENDING:
		dwStyle |= LVS_SORTDESCENDING;
		break;
	default:
		break;
	}

	// Is Auto Arrange
	if (pList->m_bAutoArrange)
	{
		dwStyle |= LVS_AUTOARRANGE;
	}
	// Is Edit Labels
	if (pList->m_bEditLabels)
	{
		dwStyle |= LVS_EDITLABELS;
	}
	// Is No Header
	if (pList->m_bNoHeader)
	{
		dwStyle |= LVS_NOCOLUMNHEADER;
	}
	// Is No Label Wrap
	if (pList->m_bNoLabelWrap)
	{
		dwStyle |= LVS_NOLABELWRAP;
	}
	// Is No Scroll
	if (pList->m_bNoScroll)
	{
		dwStyle |= LVS_NOSCROLL;
	}
	// Is No Sort Header
	if (pList->m_bNoSortHeader)
	{
		dwStyle |= LVS_NOSORTHEADER;
	}
	// Is Show Selection Always
	if (pList->m_bShowSelAlways)
	{
		dwStyle |= LVS_SHOWSELALWAYS;
	}
	// Is Single Selection
	if (pList->m_bSingleSel)
	{
		dwStyle |= LVS_SINGLESEL;
	}
	// Is owner data
	if (pList->m_bOwnerData)
	{
		dwStyle |= LVS_OWNERDATA;
	}

	CIUIRect rect = pList->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_LISTVIEW, pList->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pList->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateTreeView(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CTreeViewProp *pTree,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pTree);

	DWORD dwExStyle = 0;
	if ((bool)pTree->m_bSmoothVScroll)
	{
		dwExStyle |= TVS_EX_SMOOTHVSCROLL;
	}

	RECT rect = pTree->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(&rect);
	}

	// TVS_XXX已经把0xFFFF的16位占满; TVS_EX_XXX在IUI中的Tree中未使用，但commctrl.h头文件
	// 中预定义了一些TVS_EX_XXX，只剩下0x0001和0x8000未使用。 所以，我们定义自己的扩展风格：
	// #define TVS_EX_SMOOTHVSCROLL 0x8000
	HWLWND hWnd = CreateWindowEx(pUserHandler, dwExStyle, IUI_TREEVIEW, pTree->m_strCaption,
			dwStyle, rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pTree->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

// CTreeListCtrl *CreateControls::CreateTreeList(TREELISTPROPERTIES *pTreeList, CWLWnd *pParent,
// 	HIUIIMAGE himgN, HIUIIMAGE himgD)
// {
// 	int nID = LONG(pTreeList->m_nID);
// 	_ASSERT(nID != -1);
//
// 	// Style
// 	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pTreeList);
//
// 	CTreeListCtrl *pChild = new CTreeListCtrl;
// 	_ASSERT(pChild != NULL);
//
// 	CIUIRect rect = pTreeList->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
// 	if (pTreeList->m_bVariableItemHeight)
// 	{
// 		if (!pChild->Create(dwStyle, rect, pParent, nID, 24, TRUE, 16))
// 		{
// 			delete pChild;
// 			return NULL;
// 		}
// 	}
// 	else
// 	{
// 		if (!pChild->Create(dwStyle, rect, pParent, nID))
// 		{
// 			delete pChild;
// 			return NULL;
// 		}
// 	}
//
// 	pChild->BindStyle(pTreeList);
//
// 	bool bSpecifyBackgroundImages = pTreeList->m_bSpecifyBackgroundImages;
// 	if (!bSpecifyBackgroundImages)
// 	{
// 		HIUIIMAGE hBmpN = NULL;
// 		HIUIIMAGE hBmpD = NULL;
// 		CIUIRect rect = pTreeList->m_rcControl;
// 		IUIGetBackgroundFromParent2(pTreeList, rect, himgN, himgD, &hBmpN, &hBmpD);
// 		pChild->SetBkImage(CONTROL_STATE_UNCHECKED_ND, hBmpN->GetSafeImageName(), hBmpD->GetSafeImageName());
// 	}
//
// 	return pChild;
// }
//
// CIUIString g_strRichEditClassName;
// CIUIString CreateControls::GetRichEditClassName()
// {
// 	if (g_strRichEditClassName.IsEmpty())
// 	{
// 		g_strRichEditClassName = RICHEDIT_CLASS;
// 	}
//
// 	return g_strRichEditClassName;
// }
//
HWLWND CreateControls::CreateRichTextBox(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CRichTextBoxProp *pRichEdt,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//#if(_MSC_VER > 1200) // VC6.0
	//	if (!AfxInitRichEdit2())
	//#else
	//	if (!AfxInitRichEdit())
	//#endif
	//	{
	//		return NULL;
	//	}

	//int nID = LONG(pRichEdt->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pRichEdt);
	if (pRichEdt->m_bHScroll)
	{
		dwStyle |= WS_HSCROLL;
	}
	if (pRichEdt->m_bVScroll)
	{
		dwStyle |= WS_VSCROLL;
	}
	if (pRichEdt->m_bWantReturn)
	{
		dwStyle |= ES_WANTRETURN;
	}
	if (pRichEdt->m_bAutoHScroll)
	{
		dwStyle |= ES_AUTOHSCROLL;
	}
	if (pRichEdt->m_bAutoVScroll)
	{
		dwStyle |= ES_AUTOVSCROLL;
	}
	if (pRichEdt->m_bLowercase)
	{
		dwStyle |= ES_LOWERCASE;
	}
	if (pRichEdt->m_bUppercase)
	{
		dwStyle |= ES_UPPERCASE;
	}
	if (pRichEdt->m_bNumber)
	{
		dwStyle |= ES_NUMBER;
	}
	if (pRichEdt->m_bPassword)
	{
		dwStyle |= ES_PASSWORD;
	}
	if (pRichEdt->m_bReadOnly)
	{
		dwStyle |= ES_READONLY;
	}
	if (pRichEdt->m_bMultiline)
	{
		dwStyle |= ES_MULTILINE;
	}
	if (TAH_CENTER == (TEXT_ALIGN_HOR)pRichEdt->m_eTextHorAlignMode)
	{
		dwStyle |= ES_CENTER;
	}
	else if (TAH_RIGHT == (TEXT_ALIGN_HOR)pRichEdt->m_eTextHorAlignMode)
	{
		dwStyle |= ES_RIGHT;
	}

	CIUIRect rect = pRichEdt->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	//#if(_MSC_VER <= 1200) // VC6.0
	//	if (!pChild->CreateEx(WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
	//			_T("RICHEDIT20A"), _T(""), dwStyle, rect, pParent, nID))
	//#else
	//	if (!pChild->CreateEx(WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY, dwStyle, rect, pParent, nID))
	//#endif // (_MSC_VER <= 1200)
	//	{
	//		delete pChild;
	//		return NULL;
	//	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_RICHTEXTBOX, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pRichEdt->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateIM(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CIMRichTextBoxProp *pRichEdt,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// 制作IM聊天记录控件时，RichEdit恒设为禁止水平滚动、只读、多行、非password、非数字
	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pRichEdt);
	dwStyle |= WS_VSCROLL;
	dwStyle |= ES_AUTOVSCROLL;
	dwStyle |= ES_MULTILINE;
	dwStyle |= ES_LEFT;
	dwStyle |= ES_WANTRETURN;
	// 虽然IM控件是只读的，但是不通过SetReadOnly(TRUE)来设置为只读
	// 因为这样设置后，影响内容的布局（缩进），所以通过禁用键盘输入、拖放
	// 来达到只读的目的。
	//dwStyle |= ES_READONLY;
	//dwStyle |= ES_NOOLEDRAGDROP; // Same as ES_UPPERCASE
	dwStyle &= ~ES_AUTOHSCROLL;
	dwStyle &= ~ES_LOWERCASE;
	dwStyle &= ~ES_UPPERCASE;

	CIUIRect rect = pRichEdt->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	//#if(_MSC_VER <= 1200) // VC6.0
	//	if (!pChild->CreateEx(WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
	//			_T("RICHEDIT20A"), _T(""), dwStyle, rect, pParent, nID))
	//#else
	//	if (!pChild->CreateEx(WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY, dwStyle, rect, pParent, nID))
	//#endif // (_MSC_VER <= 1200)
	//	{
	//		delete pChild;
	//		return NULL;
	//	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_RICHTEXTBOX, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pRichEdt->m_strName, hInst, (LPVOID)lCreateParam);

	if (NULL == hWnd)
	{
		return NULL;
	}

	// 由于RichEdit只要不是ES_READONLY，就会调用RegisterDragDrop注册成拖放目标
	// 所以在创建完后，要取消注册。
	RevokeDragDrop(hWnd);

	fnWLEventHandler fnOldHandler = SubclassWindowless(hWnd, IMRichTextBox_WndProc);
	SetProp(hWnd, _T("GetProp_Old_Handler"), fnOldHandler);

	return hWnd;
}

//
// CIMRichEditCtrl *CreateControls::CreateIMRichEdit(IMREPROPERTIES *pRichEdt, CWLWnd *pParent,
// 	HIUIIMAGE himgN, HIUIIMAGE himgH, HIUIIMAGE himgS, HIUIIMAGE himgD)
// {
// #if(_MSC_VER > 1200) // VC6.0
// 	if (!AfxInitRichEdit2())
// #else
// 	if (!AfxInitRichEdit())
// #endif
// 	{
// 		return NULL;
// 	}
//
// 	int nID = LONG(pRichEdt->m_nID);
// 	_ASSERT(nID != -1);
//
// 	// 制作IM聊天记录控件时，RichEdit恒设为禁止水平滚动、只读、多行、非password、非数字
// 	// Style
// 	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
// 	dwStyle |= WS_VSCROLL;
// 	dwStyle |= ES_AUTOVSCROLL;
// 	dwStyle |= ES_MULTILINE;
// 	dwStyle |= ES_LEFT;
// 	dwStyle |= ES_WANTRETURN;
// 	dwStyle &= ~ES_AUTOHSCROLL;
// 	dwStyle &= ~ES_LOWERCASE;
// 	dwStyle &= ~ES_UPPERCASE;
//
// 	// 加了只读属性后，就无法设置左缩进
// 	//	dwStyle |= ES_READONLY;
//
//
// 	CIMRichEditCtrl *pChild = new CIMRichEditCtrl();
// 	_ASSERT(pChild != NULL);
//
// 	CIUIRect rect = pRichEdt->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
//
// #if(_MSC_VER <= 1200) // VC6.0
// 	if (!pChild->CreateEx(WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
// 			_T("RICHEDIT20A"), _T(""), dwStyle, rect, pParent, nID))
// #else
// 	if (!pChild->CreateEx(WS_EX_TRANSPARENT, dwStyle, rect, pParent, nID))
// #endif // (_MSC_VER <= 1200)
// 	{
// 		delete pChild;
// 		return NULL;
// 	}
//
// 	// 由于使用不同的VS，创建的RichEdit的类名也不同，所以，把这个类名记录下来，
// 	// 在UIWnd绘制控件的时候，需要根据这个类名，查找RichEdit，进而绘制RichEdit。
// 	GetClassName(pChild->GetSafeHwnd(), g_strRichEditClassName.GetBufferSetLength(MAX_PATH), MAX_PATH);
// 	g_strRichEditClassName.ReleaseBuffer();
//
// 	pChild->CIMRichEditCtrlBase::BindStyle(pRichEdt);
//
// 	return pChild;
// }
//
HWLWND CreateControls::CreateHtmlView(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CHtmlViewProp *pHtmlViewProp,
	HWND hParent,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pHtmlViewProp->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pHtmlViewProp);

	CIUIRect rect = pHtmlViewProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_HTMLVIEW, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hParent,
			hWLParent, pHtmlViewProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateSplitterBar(
	__inout CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CSplitterBarProp *pSplitter,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pSplitter->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pSplitter);

	SplitterBar *pChild = new SplitterBar;
	_ASSERT(pChild != NULL);

	CIUIRect rect = pSplitter->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_SPLITTERBAR, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pSplitter->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}


HWLWND CreateControls::CreateDateTimePicker(
	__inout CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CDateTimePickerProp *pDateTimeCtrlProp,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pDateTimeCtrlProp->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pDateTimeCtrlProp);
	DATETIME_FORMAT eFormat = (DATETIME_FORMAT)pDateTimeCtrlProp->m_eFormat;
	if (eFormat == DTF_SHORT_DATE)
	{
		dwStyle |= DTS_SHORTDATEFORMAT;
	}
	else if (eFormat == DTF_LONG_DATE)
	{
		dwStyle |= DTS_LONGDATEFORMAT;
	}
	else if (eFormat == DTF_TIME)
	{
		dwStyle |= DTS_TIMEFORMAT;
	}
	else
	{
		_ASSERT(FALSE);
	}

	if (pDateTimeCtrlProp->m_bAllowEdit)
	{
		dwStyle |= DTS_APPCANPARSE;
	}
	if (pDateTimeCtrlProp->m_bShowNone)
	{
		dwStyle |= DTS_SHOWNONE;
	}
	if (pDateTimeCtrlProp->m_bUseSpinControl)
	{
		dwStyle |= DTS_UPDOWN;
	}

	CIUIRect rect = pDateTimeCtrlProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_DATETIMEPICKER, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pDateTimeCtrlProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

// CWaveCtrl *CreateControls::CreateWaveCtrl(WAVECTRLPROPERTIES *pWaveCtrl, CWLWnd *pParent,
// 	HIUIIMAGE hbmpN, HIUIIMAGE hbmpH, HIUIIMAGE hbmpS, HIUIIMAGE hbmpD)
// {
// 	int nID = LONG(pWaveCtrl->m_nID);
// 	_ASSERT(nID != -1);
//
// 	// Style
// 	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pWaveCtrl);
//
// 	CWaveCtrl *pChild = new CWaveCtrl;
// 	_ASSERT(pChild != NULL);
//
// 	CIUIRect rect = pWaveCtrl->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
// 	if (!pChild->Create(dwStyle, rect, pParent, nID))
// 	{
// 		delete pChild;
// 		return NULL;
// 	}
//
// 	pChild->BindStyle(pWaveCtrl);
//
// 	// Set bitmaps for background
// 	bool bSpecifyBackgroundImages = pWaveCtrl->m_bSpecifyBackgroundImages;
// 	if (!bSpecifyBackgroundImages)
// 	{
// 		HIUIIMAGE hBmpN = NULL;
// 		HIUIIMAGE hBmpD = NULL;
// 		CIUIRect rect = pWaveCtrl->m_rcControl;
// 		IUIGetBackgroundFromParent2(pWaveCtrl, rect, hBmpN, hbmpD, &hBmpN, &hBmpD);
// 		pChild->SetBkImage(CONTROL_STATE_UNCHECKED_ND,
// 			hBmpN->GetSafeImageName(), hBmpD->GetSafeImageName());
// 	}
//
// 	return pChild;
// }
//
// CMenuBar *CreateControls::CreateMenuBar(MENUBARPROPERTIES *pMenuBar, CWLWnd *pParent)
// {
// 	int nID = LONG(pMenuBar->m_nID);
// 	_ASSERT(nID != -1);
//
// 	// Style
// 	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pMenuBar);
//
// 	CMenuBar *pChild = new CMenuBar;
// 	_ASSERT(pChild != NULL);
//
// 	//////////////////////////////////////////////////////////////////////////
// 	// Set menu item properties before the menu bar be created.
//
// 	// Set menu border
// 	MENUPROPERTIES menuPropDefault;
// 	MENUPROPERTIES *pMenuProp = (MENUPROPERTIES *)CUIMgr::GetStyleItem(STYLET_MENU,
// 			(LPCTSTR)pMenuBar->m_strMenuStyleID);
// 	if (pMenuProp == NULL)
// 	{
// 		pMenuProp = &menuPropDefault;
// 	}
//
// 	// Bind menu style
// 	pChild->BindMenuStyle(pMenuProp);
//
// 	//////////////////////////////////////////////////////////////////////////
// 	// For menu bar's button
//
// 	// Set bitmaps for menu bar's button background
// 	if (pMenuBar->m_bCombineBackgroundImages)
// 	{
// 		pChild->SetMenuButtonImages((LPCTSTR)pMenuBar->m_strBKCombine);
// 		pChild->SetBkImageResizeMode(pMenuBar->m_ptImageResize);
// 	}
// 	else
// 	{
// 		pChild->SetMenuButtonImages(CONTROL_STATE_UNCHECKED_ALL,
// 			(LPCTSTR)pMenuBar->m_strBKN, (LPCTSTR)pMenuBar->m_strBKH,
// 			(LPCTSTR)pMenuBar->m_strBKS, (LPCTSTR)pMenuBar->m_strBKD);
//
// 		pChild->SetMenuButtonCheckedImages(CONTROL_STATE_CHECKED_ALL,
// 			(LPCTSTR)pMenuBar->m_strBKCN, (LPCTSTR)pMenuBar->m_strBKCH,
// 			(LPCTSTR)pMenuBar->m_strBKCS, (LPCTSTR)pMenuBar->m_strBKCD);
//
// 		pChild->SetBkImageResizePoint(pMenuBar->m_ptImageResize);
// 	}
//
// 	// Set color for menu bar's button
// 	COLORREF cr[4] = {0};
// 	IUIGetControlColor4(pMenuBar, &cr[0], &cr[1], &cr[2], &cr[3]);
// 	pChild->SetMenuButtonTextColor(cr[0], cr[1], cr[2], cr[3]);
//
// 	// Set font for menu bar's button
// 	pChild->SetMenuButtonTextFont(pMenuBar->m_strResFontIDN, pMenuBar->m_strResFontIDH,
// 		pMenuBar->m_strResFontIDS, pMenuBar->m_strResFontIDD);
//
// 	CIUIRect rect = pMenuBar->m_rcControl;
// 	if (CUIMgr::IsEnableDPI())
// 	{
// 		GetDPIRect(rect);
// 	}
// 	if (!pChild->Create(dwStyle, rect, pParent, nID))
// 	{
// 		delete pChild;
// 		return NULL;
// 	}
//
// 	return pChild;
// }

HWLWND CreateControls::CreateCanvas(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CCanvasProp *pCanvas,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pCanvas);

	RECT rect = pCanvas->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(&rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_CANVAS, pCanvas->m_strCaption, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pCanvas->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateTabControl(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CTabControlProp *pTabControlProp,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pTaskWndMgrProp->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pTabControlProp);

	CIUIRect rect = pTabControlProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_TABCONTROL, pTabControlProp->m_strCaption,
			dwStyle, rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pTabControlProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateDockPanel(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CDockPanelProp *pDockPanelProp,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pDockPanelProp->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pDockPanelProp);

	CIUIRect rect = pDockPanelProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_DOCKPANEL, pDockPanelProp->m_strCaption,
			dwStyle, rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pDockPanelProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

HWLWND CreateControls::CreateStackPanel(
	__in CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CStackPanelProp *pStackPanelProp,
	HWND hHost,
	HWLWND hWLParent,
	LPARAM lCreateParam)
{
	//int nID = LONG(pStackPanelProp->m_nID);
	//_ASSERT(nID != -1);

	// Style
	DWORD dwStyle = WS_CHILD | GetControlBaseStyle(pStackPanelProp);

	CIUIRect rect = pStackPanelProp->m_rcControl;
	if (IsEnableDPI())
	{
		GetDPIRect(rect);
	}

	HWLWND hWnd = CreateWindowEx(pUserHandler, 0, IUI_STACKPANEL, pStackPanelProp->m_strCaption,
			dwStyle, rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect), hHost,
			hWLParent, pStackPanelProp->m_strName, hInst, (LPVOID)lCreateParam);

	if (hWnd == NULL)
	{
		return NULL;
	}

	return hWnd;
}

// 参考：CreateDialogIndirect
// 由控件属性pCtrlProp直接创建出控件及其子控件
// 通过调试，API CreateDialogIndirect在发出WM_NCCREATE和WM_CREATE时，并未创建子控件，
// 而在发出WM_INITDIALOG时，子控件已创建完毕。
HWLWND CreateControls::CreatePreDefineChildControl(
	CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	CControlProp *pCtrlProp,
	HWLWND hParent,
	HWND hHost,
	LPARAM lCreateParam)
{
	if (pCtrlProp == NULL || hHost == NULL)
	{
		return NULL;
	}

	DWORD dwStyle = GetControlBaseStyle(pCtrlProp);
	BOOL bVisible = IsIncludeFlag(dwStyle, WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	HWLWND hNewCtrl = NULL;

	BOOL bRet = FALSE;
	CONTROL_TYPE eControlType = (CONTROL_TYPE)pCtrlProp->m_eControlType;

	switch (eControlType)
	{
	case CT_STATIC:
		hNewCtrl = CreateLabel(pUserHandler, hInst, (CLabelProp *)pCtrlProp,
				hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		Label_BindStyle(hNewCtrl, (CLabelProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PUSHBUTTON:
		hNewCtrl = CreateButton(pUserHandler, hInst,
				(CButtonProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		PushButton_BindStyle(hNewCtrl, (CButtonProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_CHECK:
		hNewCtrl = CreateCheckBox(pUserHandler, hInst,
				(CCheckBoxProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		ToggleButton_BindStyle(hNewCtrl, (CToggleButtonProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_RADIO:
		hNewCtrl = CreateRadioButton(pUserHandler, hInst,
				(CRadioButtonProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		ToggleButton_BindStyle(hNewCtrl, (CToggleButtonProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_RICHEDIT:
		hNewCtrl = CreateRichTextBox(pUserHandler, hInst,
				(CRichTextBoxProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		RichTextBox_BindStyle(hNewCtrl, (CRichTextBoxProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_IM:
		hNewCtrl = CreateIM(pUserHandler, hInst,
				(CIMRichTextBoxProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		IMRichTextBox_BindStyle(hNewCtrl, (CIMRichTextBoxProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_COMBOBOX:
		hNewCtrl = CreateComboBox(pUserHandler, hInst,
				(CComboBoxProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		ComboBox_BindStyle(hNewCtrl, (CComboBoxProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PROGRESS:
		hNewCtrl = CreateProgressBar(pUserHandler, hInst,
				(CProgressBarProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		ProgressBar_BindStyle(hNewCtrl, (CProgressBarProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_SLIDER:
		hNewCtrl = CreateSlider(pUserHandler, hInst,
				(CSliderProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		Slider_BindStyle(hNewCtrl, (CSliderProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_LISTVIEW:
		hNewCtrl = CreateListView(pUserHandler, hInst,
				(CListViewProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		ListView_BindStyle(hNewCtrl, (CListViewProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_TREEVIEW:
		hNewCtrl = CreateTreeView(pUserHandler, hInst,
				(CTreeViewProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		TreeView_BindStyle(hNewCtrl, (CTreeViewProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_HTMLVIEW:
		hNewCtrl = CreateHtmlView(pUserHandler, hInst,
				(CHtmlViewProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		HtmlView_BindStyle(hNewCtrl, (CHtmlViewProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_SPLITTER:
		hNewCtrl = CreateSplitterBar(pUserHandler, hInst,
				(CSplitterBarProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		SplitterBar_BindStyle(hNewCtrl, (CSplitterBarProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_DATETIMEPICKER:
		hNewCtrl = CreateDateTimePicker(pUserHandler, hInst,
				(CDateTimePickerProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		DateTimePicker_BindStyle(hNewCtrl, (CDateTimePickerProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_SPIN:
		hNewCtrl = CreateSpinButton(pUserHandler, hInst,
				(CSpinButtonProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		SpinButton_BindStyle(hNewCtrl, (CSpinButtonProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PNL_CANVAS:
		hNewCtrl = CreateCanvas(pUserHandler, hInst,
				(CCanvasProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		Canvas_BindStyle(hNewCtrl, (CCanvasProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PNL_TABCONTROL:
		hNewCtrl = CreateTabControl(pUserHandler, hInst,
				(CTabControlProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		TabControl_BindStyle(hNewCtrl, (CTabControlProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PNL_DOCK:
		hNewCtrl = CreateDockPanel(pUserHandler, hInst,
				(CDockPanelProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		DockPanel_BindStyle(hNewCtrl, (CDockPanelProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	case CT_PNL_STACK:
		hNewCtrl = CreateStackPanel(pUserHandler, hInst,
				(CStackPanelProp *)pCtrlProp, hHost, hParent, lCreateParam);
		SetRedraw(hNewCtrl, FALSE);
		StackPanel_BindStyle(hNewCtrl, (CStackPanelProp *)pCtrlProp);
		SetRedraw(hNewCtrl, TRUE);
		break;

	default:
		_ASSERT(FALSE);
		break;
	}

	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hNewCtrl;
	arg.RoutedEvent = UE_SETCONTROLPROP;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)pCtrlProp;
	RaiseEvent(hNewCtrl, &arg);

	//
	// Create children
	//
	int nChildCount = pCtrlProp->GetChildCount();
	for (int i = 0; i < nChildCount; i++)
	{
		CControlBaseProp *pChild = pCtrlProp->GetChild(i);

		if (pChild->IsControl())
		{
			CreatePreDefineChildControl(NULL, hInst,
				(CControlProp *)pChild, hNewCtrl, hHost, lCreateParam);
		}
	}

	// Win32中，对话框在收到WM_INITDIALOG时，窗口也是不可见的。
	// 注意：::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0)会使窗口可见，
	{
		RoutedEventArgs arg;
		arg.hSender = hNewCtrl;
		arg.hOriginalSender = hNewCtrl;
		arg.RoutedEvent = WM_INITDIALOG;
		arg.eRoutingStrategy = RS_BUBBLE;
		RaiseEvent(arg.hOriginalSender, &arg);
	}

	if (bVisible)
	{
		ShowWindow(hNewCtrl, SW_SHOW);
	}

	return hNewCtrl;
}

HWLWND CreateControls::CreateXMLWindowChild(
	CWLWndHandler *pUserHandler,
	HINSTANCE hInst,
	LPCTSTR lpszXML,
	HWLWND hParent,
	HWND hHost,
	LPARAM lCreateParam)
{
	if (NULL == lpszXML || hHost == NULL)
	{
		return NULL;
	}

	CIUIProject *pProject = (CIUIProject *)::SendMessage(hHost, WM_GET_PROJECT, 0, 0);
	if (NULL == pProject)
	{
		return NULL;
	}

	CControlProp *pContactProp = pProject->FindProjectItem(lpszXML);
	if (NULL == pContactProp)
	{
		return NULL;
	}

	CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
	if (NULL == pBindContactProp)
	{
		return NULL;
	}

	HWLWND hRet = CreatePreDefineChildControl(pUserHandler, hInst, pBindContactProp,
			hParent, hHost, lCreateParam);

	return hRet;
}

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


////////////////////////////////////////////////
ComboBox::ComboBox()
{
}

ComboBox::~ComboBox()
{
}

BOOL ComboBox::Create(
	__in CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParentWnd,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_COMBOBOX, NULL,
			dwStyle, rect, hParent, pParentWnd, lpszName, 0);
}

int ComboBox::BindStyle(const CControlProp *pCtrlProp)
{
	return ComboBox_BindStyle(m_hWnd, (CComboBoxProp *)pCtrlProp);
}

int ComboBox::SetButtonWidth(int nWidth)
{
	return SendMessage(CBM_SET_BUTTON_WIDTH, nWidth, 0);
}

int ComboBox::GetButtonWidth() const
{
	return SendMessage(CBM_GET_BUTTON_WIDTH, 0, 0);
}

int ComboBox::SetButtonMargin(LPCRECT lprcMargin)
{
	return SendMessage(CBM_SET_BUTTON_MARGIN, 0, (LPARAM)lprcMargin);
}

int ComboBox::GetButtonMargin(LPRECT lprcMargin) const
{
	return SendMessage(CBM_GET_BUTTON_WIDTH, 0, (LPARAM)lprcMargin);
}

int ComboBox::SetBkColor(UINT uMask, COLORREF crBkN, COLORREF crBkH, COLORREF crBkS, COLORREF crBkD)
{
	_ASSERT(IsWindow());

	SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_NORMAL), crBkN);
	SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_HOVER), crBkH);
	SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_PRESSED), crBkS);
	SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_DISABLED), crBkD);

	Invalidate();

	return 0;
}

int ComboBox::GetBkColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH, COLORREF *pcrS, COLORREF *pcrD) const
{
	_ASSERT(IsWindow());

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrH != NULL)
	{
		*pcrH = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_HOVER), 0);
	}
	if (pcrS != NULL)
	{
		*pcrS = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_PRESSED), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ComboBox::SetBkImage(
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	BOOL bRedraw/* = TRUE*/)
{
	_ASSERT(IsWindow());

	IUISetControlBkImage4(m_hWnd, FALSE, uMask,
		lpszImageNameN, lpszImageNameH, lpszImageNameP, lpszImageNameD);

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int ComboBox::GetBkImage(
	UINT uMask,
	BOOL *pbCombineImage,
	CIUIString *pstrCombineImageName,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameP,
	CIUIString *pstrImageNameD) const
{
	_ASSERT(IsWindow());

	if (pbCombineImage == NULL)
	{
		return -1;
	}

	return IUIGetControlBkImage4(m_hWnd, FALSE, uMask, pbCombineImage, pstrCombineImageName,
			pstrImageNameN, pstrImageNameH, pstrImageNameP, pstrImageNameD);
}

int ComboBox::SetBorderColor(UINT uMask,
	COLORREF crN, COLORREF crH, COLORREF crS, COLORREF crD, BOOL bRedraw)
{
	_ASSERT(IsWindow());

	SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), crH);
	SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), crS);
	SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	Invalidate();

	return 0;
}

int ComboBox::GetBorderColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH,
	COLORREF *pcrS, COLORREF *pcrD) const
{
	_ASSERT(IsWindow());

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrH != NULL)
	{
		*pcrH = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}
	if (pcrS != NULL)
	{
		*pcrS = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ComboBox::SetTextColor(UINT uMask, COLORREF crN, COLORREF crH,
	COLORREF crS, COLORREF crD, BOOL bRedraw)
{
	_ASSERT(IsWindow());

	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), crH);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), crS);
	SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	Invalidate();

	return 0;
}

int ComboBox::GetTextColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH,
	COLORREF *pcrS, COLORREF *pcrD) const
{
	_ASSERT(IsWindow());

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrH != NULL)
	{
		*pcrH = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}
	if (pcrS != NULL)
	{
		*pcrS = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ComboBox::SetFocusedTextColor(COLORREF cr)
{
	CBOX *pcbox = (CBOX *)GetMember();
	pcbox->m_crFocusedText = cr;

	return 0;
}

COLORREF ComboBox::GetFocusedTextColor() const
{
	CBOX *pcbox = (CBOX *)GetMember();
	return pcbox->m_crFocusedText;
}

int ComboBox::SetTextFont(
	UINT uMask,
	LPCTSTR lpszFontIDN,
	LPCTSTR lpszFontIDH,
	LPCTSTR lpszFontIDS,
	LPCTSTR lpszFontIDD,
	BOOL bRedraw)
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lpszFontIDN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lpszFontIDH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lpszFontIDS);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(WLM_SET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lpszFontIDD);
	}

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int ComboBox::GetTextFont(
	UINT uMask,
	CIUIString *pstrFontIDN,
	CIUIString *pstrFontIDH,
	CIUIString *pstrFontIDS,
	CIUIString *pstrFontIDD) const
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pstrFontIDN == NULL)
		{
			return -1;
		}

		*pstrFontIDN = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pstrFontIDH == NULL)
		{
			return -1;
		}

		*pstrFontIDH = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pstrFontIDS == NULL)
		{
			return -1;
		}

		*pstrFontIDS = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pstrFontIDD == NULL)
		{
			return -1;
		}

		*pstrFontIDD = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

int ComboBox::SetTextMargin(
	UINT uMask,
	LPCRECT lprcTextMarginN,
	LPCRECT lprcTextMarginH,
	LPCRECT lprcTextMarginS,
	LPCRECT lprcTextMarginD,
	BOOL bRedraw)
{
	CBOX *pcbox = (CBOX *)GetMember();

	pcbox->m_rcTextMargin[0] = *lprcTextMarginN;
	pcbox->m_rcTextMargin[1] = *lprcTextMarginH;
	pcbox->m_rcTextMargin[2] = *lprcTextMarginS;
	pcbox->m_rcTextMargin[3] = *lprcTextMarginD;

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int ComboBox::GetTextMargin(
	UINT uMask,
	LPRECT lprcTextMarginN,
	LPRECT lprcTextMarginH,
	LPRECT lprcTextMarginS,
	LPRECT lprcTextMarginD) const
{
	CBOX *pcbox = (CBOX *)GetMember();

	if (lprcTextMarginN != NULL)
	{
		*lprcTextMarginN = pcbox->m_rcTextMargin[0];
	}
	if (lprcTextMarginH != NULL)
	{
		*lprcTextMarginH = pcbox->m_rcTextMargin[1];
	}
	if (lprcTextMarginS != NULL)
	{
		*lprcTextMarginS = pcbox->m_rcTextMargin[2];
	}
	if (lprcTextMarginD != NULL)
	{
		*lprcTextMarginD = pcbox->m_rcTextMargin[3];
	}

	return 0;
}

int ComboBox::SetEditMargin(LPCRECT lprcMargin)
{
	_ASSERT(IsWindow());

	return SendMessage(CBM_SET_EDIT_MARGIN, 0, (LPARAM)lprcMargin);
}

int ComboBox::GetEditMargin(LPRECT lprcMargin)
{
	_ASSERT(IsWindow());

	return SendMessage(CBM_GET_EDIT_MARGIN, 0, (LPARAM)lprcMargin);
}

int ComboBox::SetTextAlignHor(
	UINT uMask,
	TEXT_ALIGN_HOR eHorAlignModeN,
	TEXT_ALIGN_HOR eHorAlignModeH,
	TEXT_ALIGN_HOR eHorAlignModeS,
	TEXT_ALIGN_HOR eHorAlignModeD,
	BOOL bRedraw)
{
	CBOX *pcbox = (CBOX *)GetMember();

	pcbox->m_eTextHorAlignMode[0] = eHorAlignModeN;
	pcbox->m_eTextHorAlignMode[1] = eHorAlignModeH;
	pcbox->m_eTextHorAlignMode[2] = eHorAlignModeS;
	pcbox->m_eTextHorAlignMode[3] = eHorAlignModeD;

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int ComboBox::GetTextAlignHor(
	UINT uMask,
	TEXT_ALIGN_HOR *peHorAlignModeN,
	TEXT_ALIGN_HOR *peHorAlignModeH,
	TEXT_ALIGN_HOR *peHorAlignModeS,
	TEXT_ALIGN_HOR *peHorAlignModeD) const
{
	CBOX *pcbox = (CBOX *)GetMember();

	if (peHorAlignModeN != NULL)
	{
		*peHorAlignModeN = pcbox->m_eTextHorAlignMode[0];
	}
	if (peHorAlignModeH != NULL)
	{
		*peHorAlignModeH = pcbox->m_eTextHorAlignMode[1];
	}
	if (peHorAlignModeS != NULL)
	{
		*peHorAlignModeS = pcbox->m_eTextHorAlignMode[2];
	}
	if (peHorAlignModeD != NULL)
	{
		*peHorAlignModeD = pcbox->m_eTextHorAlignMode[3];
	}

	return 0;
}

int ComboBox::SetTextAlignVer(
	UINT uMask,
	TEXT_ALIGN_VER eVerAlignModeN,
	TEXT_ALIGN_VER eVerAlignModeH,
	TEXT_ALIGN_VER eVerAlignModeS,
	TEXT_ALIGN_VER eVerAlignModeD,
	BOOL bRedraw)
{
	CBOX *pcbox = (CBOX *)GetMember();

	pcbox->m_eTextVerAlignMode[0] = eVerAlignModeN;
	pcbox->m_eTextVerAlignMode[1] = eVerAlignModeH;
	pcbox->m_eTextVerAlignMode[2] = eVerAlignModeS;
	pcbox->m_eTextVerAlignMode[3] = eVerAlignModeD;

	return 0;
}

int ComboBox::GetTextAlignVer(
	UINT uMask,
	TEXT_ALIGN_VER *peVerAlignModeN,
	TEXT_ALIGN_VER *peVerAlignModeH,
	TEXT_ALIGN_VER *peVerAlignModeS,
	TEXT_ALIGN_VER *peVerAlignModeD) const
{
	CBOX *pcbox = (CBOX *)GetMember();

	if (peVerAlignModeN != NULL)
	{
		*peVerAlignModeN = pcbox->m_eTextVerAlignMode[0];
	}
	if (peVerAlignModeH != NULL)
	{
		*peVerAlignModeH = pcbox->m_eTextVerAlignMode[1];
	}
	if (peVerAlignModeS != NULL)
	{
		*peVerAlignModeS = pcbox->m_eTextVerAlignMode[2];
	}
	if (peVerAlignModeD != NULL)
	{
		*peVerAlignModeD = pcbox->m_eTextVerAlignMode[3];
	}

	return 0;
}

BOOL ComboBox::SetDrawDropButtonBkColor(BOOL bDraw)
{
	_ASSERT(IsWindow());

	SendMessage(CBM_SET_DRAW_DROP_BUTTON_BK_COLOR, (WPARAM)bDraw, 0);

	return 0;
}

BOOL ComboBox::IsDrawDropButtonBkColor() const
{
	_ASSERT(IsWindow());

	(BOOL)SendMessage(CBM_GET_DRAW_DROP_BUTTON_BK_COLOR, 0, 0);

	return 0;
}

int ComboBox::SetDropButtonBkColor(
	UINT uMask,
	COLORREF crN,
	COLORREF crH,
	COLORREF crP,
	COLORREF crD,
	COLORREF crS,
	BOOL bRedraw)
{
	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)crN);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)crH);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)crP);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)crD);
	}

	if (IsIncludeFlag(uMask, CONTROL_STATE_SELECTED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_SELECTED), (LPARAM)crS);
	}

	if (bRedraw)
	{
		Invalidate();
	}

	return 0;
}

int ComboBox::GetDropButtonBkColor(
	UINT uMask,
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrP,
	COLORREF *pcrD,
	COLORREF *pcrS) const
{
	return 0;
}

int ComboBox::SetDropButtonCombineBkImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return SendMessage(CBM_SET_DROP_BUTTON_BK_IMAGE,
			MAKEWPARAM(0, CONTROL_STATE_COMBINE), (LPARAM)lpszImage);
}

CIUIString ComboBox::GetDropButtonCombineBkImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)SendMessage(CBM_GET_DROP_BUTTON_BK_IMAGE,
			MAKEWPARAM(0, CONTROL_STATE_COMBINE), 0);
}

int ComboBox::SetDropButtonBkImage(
	UINT uMask,
	LPCTSTR lpszImageNameN,
	LPCTSTR lpszImageNameH,
	LPCTSTR lpszImageNameP,
	LPCTSTR lpszImageNameD,
	LPCTSTR lpszImageNameS,
	BOOL bRedraw)
{
	_ASSERT(IsWindow());

	return 0;
}

int ComboBox::GetDropButtonBkImage(
	UINT uMask,
	CIUIString *pstrImageNameN,
	CIUIString *pstrImageNameH,
	CIUIString *pstrImageNameP,
	CIUIString *pstrImageNameD,
	CIUIString *pstrImageNameS) const
{
	_ASSERT(IsWindow());

	return 0;
}

BOOL ComboBox::SetDrawDropButtonBorderColor(BOOL bDraw)
{
	_ASSERT(IsWindow());
	return SendMessage(CBM_SET_DRAW_DROP_BUTTON_BORDER_COLOR, bDraw, 0);
}

BOOL ComboBox::IsDrawDropButtonBorderColor() const
{
	_ASSERT(IsWindow());
	return SendMessage(CBM_GET_DRAW_DROP_BUTTON_BORDER_COLOR, 0, 0);
}

int ComboBox::SetDropButtonBorderColor(
	UINT uMask,
	COLORREF crN,
	COLORREF crH,
	COLORREF crP,
	COLORREF crD,
	COLORREF crS,
	BOOL bRedraw)
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BORDER_COLOR,
			MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BORDER_COLOR,
			MAKEWPARAM(0, CONTROL_STATE_HOVER), crH);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BORDER_COLOR,
			MAKEWPARAM(0, CONTROL_STATE_PRESSED), crP);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BORDER_COLOR,
			MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_SELECTED))
	{
		SendMessage(CBM_SET_DROP_BUTTON_BORDER_COLOR,
			MAKEWPARAM(0, CONTROL_STATE_SELECTED), crS);
	}

	return 0;
}

int ComboBox::GetDropButtonBorderColor(
	UINT uMask,
	COLORREF *pcrN,
	COLORREF *pcrH,
	COLORREF *pcrP,
	COLORREF *pcrD,
	COLORREF *pcrS) const
{
	_ASSERT(IsWindow());

	if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
	{
		if (pcrN == NULL)
		{
			return -1;
		}

		*pcrN = SendMessage(CBM_GET_DROP_BUTTON_BORDER_COLOR,
				MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_HOVER))
	{
		if (pcrH == NULL)
		{
			return -1;
		}

		*pcrH = SendMessage(CBM_GET_DROP_BUTTON_BORDER_COLOR,
				MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_PRESSED))
	{
		if (pcrH == NULL)
		{
			return -1;
		}

		*pcrH = SendMessage(CBM_GET_DROP_BUTTON_BORDER_COLOR,
				MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
	{
		if (pcrD == NULL)
		{
			return -1;
		}

		*pcrD = SendMessage(CBM_GET_DROP_BUTTON_BORDER_COLOR,
				MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}
	if (IsIncludeFlag(uMask, CONTROL_STATE_SELECTED))
	{
		if (pcrS == NULL)
		{
			return -1;
		}

		*pcrS = SendMessage(CBM_GET_DROP_BUTTON_BORDER_COLOR,
				MAKEWPARAM(0, CONTROL_STATE_SELECTED), 0);
	}

	return 0;
}

int ComboBox::GetCount() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETCOUNT, 0, 0);
}
int ComboBox::GetCurSel() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETCURSEL, 0, 0);
}
int ComboBox::SetCurSel(int nSelect)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETCURSEL, nSelect, 0);
}
DWORD ComboBox::GetEditSel() const
{
	_ASSERT(IsWindow());
	return SendMessage(CB_GETEDITSEL, 0, 0);
}
BOOL ComboBox::LimitText(int nMaxChars)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(CB_LIMITTEXT, nMaxChars, 0);
}
BOOL ComboBox::SetEditSel(int nStartChar, int nEndChar)
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
}
DWORD ComboBox::GetItemData(int nIndex) const
{
	_ASSERT(IsWindow());
	return SendMessage(CB_GETITEMDATA, nIndex, 0);
}
int ComboBox::SetItemData(int nIndex, DWORD dwItemData)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
}
void *ComboBox::GetItemDataPtr(int nIndex) const
{
	_ASSERT(IsWindow());
	return (LPVOID)GetItemData(nIndex);
}
int ComboBox::SetItemDataPtr(int nIndex, void *pData)
{
	_ASSERT(IsWindow());
	return SetItemData(nIndex, (DWORD)(LPVOID)pData);
}
int ComboBox::GetLBText(int nIndex, LPTSTR lpszText) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
}
int ComboBox::GetLBTextLen(int nIndex) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETLBTEXTLEN, nIndex, 0);
}
void ComboBox::ShowDropDown(BOOL bShowIt)
{
	_ASSERT(IsWindow());
	SendMessage(CB_SHOWDROPDOWN, bShowIt, 0);
}
int ComboBox::AddString(LPCTSTR lpszString)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_ADDSTRING, 0, (LPARAM)lpszString);
}
int ComboBox::DeleteString(UINT nIndex)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_DELETESTRING, nIndex, 0);
}
int ComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
}
void ComboBox::ResetContent()
{
	_ASSERT(IsWindow());
	SendMessage(CB_RESETCONTENT, 0, 0);
}
int ComboBox::Dir(UINT attr, LPCTSTR lpszWildCard)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_DIR, attr, (LPARAM)lpszWildCard);
}
int ComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_FINDSTRING, nStartAfter,
			(LPARAM)lpszString);
}
int ComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SELECTSTRING,
			nStartAfter, (LPARAM)lpszString);
}
void ComboBox::Clear()
{
	_ASSERT(IsWindow());
	SendMessage(WM_CLEAR, 0, 0);
}
void ComboBox::Copy()
{
	_ASSERT(IsWindow());
	SendMessage(WM_COPY, 0, 0);
}
void ComboBox::Cut()
{
	_ASSERT(IsWindow());
	SendMessage(WM_CUT, 0, 0);
}
void ComboBox::Paste()
{
	_ASSERT(IsWindow());
	SendMessage(WM_PASTE, 0, 0);
}
int ComboBox::SetItemHeight(int nIndex, UINT cyItemHeight)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
}
int ComboBox::GetItemHeight(int nIndex) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETITEMHEIGHT, nIndex, 0L);
}
int ComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
}
int ComboBox::SetExtendedUI(BOOL bExtended)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETEXTENDEDUI, bExtended, 0L);
}
BOOL ComboBox::GetExtendedUI() const
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(CB_GETEXTENDEDUI, 0, 0L);
}
void ComboBox::GetDroppedControlRect(LPRECT lprect) const
{
	_ASSERT(IsWindow());
	SendMessage(CB_GETDROPPEDCONTROLRECT, 0, (DWORD)lprect);
}
BOOL ComboBox::GetDroppedState() const
{
	_ASSERT(IsWindow());
	return (BOOL)SendMessage(CB_GETDROPPEDSTATE, 0, 0L);
}
LCID ComboBox::GetLocale() const
{
	_ASSERT(IsWindow());
	return (LCID)SendMessage(CB_GETLOCALE, 0, 0);
}
LCID ComboBox::SetLocale(LCID nNewLocale)
{
	_ASSERT(IsWindow());
	return (LCID)SendMessage(CB_SETLOCALE, (WPARAM)nNewLocale, 0);
}
int ComboBox::GetTopIndex() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETTOPINDEX, 0, 0);
}
int ComboBox::SetTopIndex(int nIndex)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETTOPINDEX, nIndex, 0);
}
int ComboBox::InitStorage(int nItems, UINT nBytes)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_INITSTORAGE, (WPARAM)nItems, nBytes);
}
void ComboBox::SetHorizontalExtent(UINT nExtent)
{
	_ASSERT(IsWindow());
	SendMessage(CB_SETHORIZONTALEXTENT, nExtent, 0);
}
UINT ComboBox::GetHorizontalExtent() const
{
	_ASSERT(IsWindow());
	return (UINT)SendMessage(CB_GETHORIZONTALEXTENT, 0, 0);
}
int ComboBox::SetDroppedWidth(UINT nWidth)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_SETDROPPEDWIDTH, nWidth, 0);
}
int ComboBox::GetDroppedWidth() const
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CB_GETDROPPEDWIDTH, 0, 0);
}

HWND ComboBox::GetDroppedWnd() const
{
	_ASSERT(IsWindow());
	return (HWND)SendMessage(CBM_GETDROPPEDWND, 0, 0);
}

BOOL ComboBox::SetItemBindWindowless(int nIndex, HWLWND hBind)
{
	_ASSERT(IsWindow());
	return (int)SendMessage(CBM_SET_ITEM_BIND, nIndex, (LPARAM)hBind);
}

HWLWND ComboBox::GetItemBindWindowless(int nIndex) const
{
	_ASSERT(IsWindow());
	return (HWLWND)SendMessage(CBM_GET_ITEM_BIND, nIndex, 0);
}

int ComboBox::SetHighlightItemBkColor(COLORREF cr)
{
	_ASSERT(IsWindow());

	return SendMessage(WLM_SET_ITEM_BK_COLOR,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), cr);
}

COLORREF ComboBox::GetHighlightItemBkColor()
{
	_ASSERT(IsWindow());
	return SendMessage(WLM_GET_ITEM_BK_COLOR,
		MAKEWPARAM(ITEM_HIGHLIGHT, CONTROL_STATE_NORMAL), 0);
}

//////////////////////////////////////////////////////////////////////////////
// ComboBox API

int IUI::ComboBox_BindStyle(HWLWND hWnd, const CComboBoxProp *pProp)
{
	//
	// 基类属性
	//
	HWLWND_BindStyle(hWnd, pProp);

	ComboBox *pCmb = (ComboBox *)CWLWnd::FromHandle(hWnd);

	// 布局
	SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pProp->m_eHorLayoutMode);
	SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pProp->m_eVerLayoutMode);
	SetDockProperty(hWnd, (DOCK)pProp->m_eDockProperty);

	SetLayoutMargin(hWnd, &(RECT)pProp->m_rcLayoutMargin);
	SetLayoutInset(hWnd, &(RECT)pProp->m_rcInset);

	//
	// ComboBox类属性
	//
	CBOX *pcbox = (CBOX *)GetMember(hWnd);

	// 背景
	SetDrawBkColor(hWnd, (bool)pProp->m_bDrawBkColor);
	COLORREF cr[COMBINEIMAGESIZE4] = {0};
	GetProject(hWnd)->GetControlBkColor4(pProp, cr);
	pCmb->SetBkColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3]);

	CIUIString strImageName[1 + COMBINEIMAGESIZE4];
	BOOL bCombineImage = TRUE;
	CControlProp::GetBackground4(pProp, &bCombineImage, strImageName);
	if (bCombineImage)
	{
		CombineBkImage(hWnd, TRUE);
		SetCombineBkImage(hWnd, strImageName[0]);
	}
	else
	{
		CombineBkImage(hWnd, FALSE);
		pCmb->SetBkImage(CONTROL_STATE_ALL,
			strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
	}
	SetBkImageResizeMode(hWnd, pProp->m_eBkImageResizeMode);
	SetBkImage9GridResizeParam(hWnd, &(RECT)pProp->m_rcBkImage9GridResizeArg);

	// 边框
	SetDrawBorderColor(hWnd, (bool)pProp->m_bDrawBorderColor);
	GetProject(hWnd)->GetControlBorderColor4(pProp, cr);
	pCmb->SetBorderColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

	// Edit box
	RECT rcEditMargin = pProp->m_rcEditMargin;
	pCmb->SetEditMargin(&rcEditMargin);

	// Button
	pCmb->SetButtonWidth((LONG)pProp->m_lBtnWidth);
	RECT rcBtnMargin = pProp->m_rcButtonMargin;
	pCmb->SetButtonMargin(&rcBtnMargin);

	// 下拉按钮背景颜色
	CIUIProject *pProject = GetProject(hWnd);
	bool bDrawDropButtonBkColor = pProp->m_bDrawDropButtonBkColor;
	pCmb->SetDrawDropButtonBkColor(bDrawDropButtonBkColor);
	if (bDrawDropButtonBkColor)
	{
		// 下拉按钮多一个选中状态
		COLORREF cr[COMBINEIMAGESIZE4 + 1] = {0};

		CColorProp *pColorPropN = pProject->GetColorProp(
				(LPCTSTR)pProp->m_strDropButtonBkResColorIDN);

		if (pColorPropN == NULL)
		{
			// Default color
			CColorProp *pResDefaultColor = pProject->GetDefaultColorProp();
			if (pResDefaultColor != NULL)
			{
				cr[0] = pResDefaultColor->m_crColor;
			}
		}
		else
		{
			cr[0] = pColorPropN->m_crColor;
		}

		CIUIString strIDH = (LPCTSTR)pProp->m_strDropButtonBkResColorIDH;
		if (strIDH.IsEmpty())
		{
			cr[1] = cr[0];
		}
		else
		{
			CColorProp *pColorPropH = pProject->GetColorProp(strIDH);
			if (pColorPropH != NULL)
			{
				cr[1] = pColorPropH->m_crColor;
			}
		}

		CIUIString strIDP = (LPCTSTR)pProp->m_strDropButtonBkResColorIDP;
		if (strIDP.IsEmpty())
		{
			cr[2] = cr[0];
		}
		else
		{
			CColorProp *pColorPropP = pProject->GetColorProp(strIDP);
			if (pColorPropP != NULL)
			{
				cr[2] = pColorPropP->m_crColor;
			}
		}

		CIUIString strIDD = (LPCTSTR)pProp->m_strDropButtonBkResColorIDD;
		if (strIDD.IsEmpty())
		{
			cr[3] = cr[0];
		}
		else
		{
			CColorProp *pColorPropD = pProject->GetColorProp(strIDD);
			if (pColorPropD != NULL)
			{
				cr[3] = pColorPropD->m_crColor;
			}
		}

		CIUIString strIDS = (LPCTSTR)pProp->m_strDropButtonBkResColorIDS;
		if (strIDS.IsEmpty())
		{
			cr[4] = cr[0];
		}
		else
		{
			CColorProp *pColorPropS = pProject->GetColorProp(strIDS);
			if (pColorPropS != NULL)
			{
				cr[4] = pColorPropS->m_crColor;
			}
		}

		pCmb->SetDropButtonBkColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], cr[4], FALSE);
	}

	// 下拉按钮背景图
	{
		CIUIString strDropButtonCombineBkImage = (LPCTSTR)pProp->m_strDropButtonCombineBkImage;
		pCmb->SetDropButtonCombineBkImage(strDropButtonCombineBkImage);
		if (strDropButtonCombineBkImage.IsEmpty())
		{
			pCmb->SetDropButtonBkImage(CONTROL_STATE_ALL,
				pProp->m_strDropButtonBkImageN,
				pProp->m_strDropButtonBkImageH,
				pProp->m_strDropButtonBkImageP,
				pProp->m_strDropButtonBkImageD,
				pProp->m_strDropButtonBkImageS,
				FALSE);
		}
	}

	// 下拉按钮边框颜色
	bool bDrawDropButtonBorderColor = pProp->m_bDrawDropButtonBorderColor;
	pCmb->SetDrawDropButtonBorderColor(bDrawDropButtonBorderColor);
	if (bDrawDropButtonBorderColor)
	{
		// 下拉按钮多一个选中状态
		COLORREF cr[COMBINEIMAGESIZE4 + 1] = {0};

		CColorProp *pColorPropN = pProject->GetColorProp(
				(LPCTSTR)pProp->m_strDropButtonBorderResColorIDN);

		if (pColorPropN == NULL)
		{
			// Default color
			CColorProp *pResDefaultColor = pProject->GetDefaultColorProp();
			if (pResDefaultColor != NULL)
			{
				cr[0] = pResDefaultColor->m_crColor;
			}
		}
		else
		{
			cr[0] = pColorPropN->m_crColor;
		}

		CIUIString strIDH = (LPCTSTR)pProp->m_strDropButtonBorderResColorIDH;
		if (strIDH.IsEmpty())
		{
			cr[1] = cr[0];
		}
		else
		{
			CColorProp *pColorPropH = pProject->GetColorProp(strIDH);
			cr[1] = pColorPropH->m_crColor;
		}

		CIUIString strIDP = (LPCTSTR)pProp->m_strDropButtonBorderResColorIDP;
		if (strIDP.IsEmpty())
		{
			cr[2] = cr[0];
		}
		else
		{
			CColorProp *pColorPropP = pProject->GetColorProp(strIDP);
			cr[2] = pColorPropP->m_crColor;
		}

		CIUIString strIDD = (LPCTSTR)pProp->m_strDropButtonBorderResColorIDD;
		if (strIDD.IsEmpty())
		{
			cr[3] = cr[0];
		}
		else
		{
			CColorProp *pColorPropD = pProject->GetColorProp(strIDD);
			cr[3] = pColorPropD->m_crColor;
		}

		CIUIString strIDS = (LPCTSTR)pProp->m_strDropButtonBorderResColorIDS;
		if (strIDS.IsEmpty())
		{
			cr[4] = cr[0];
		}
		else
		{
			CColorProp *pColorPropS = pProject->GetColorProp(strIDS);
			cr[4] = pColorPropS->m_crColor;
		}

		pCmb->SetDropButtonBorderColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], cr[4], FALSE);
	}

	// Button Arrow

	// DropDownList Static control
	GetProject(hWnd)->GetControlTextColor4(pProp, cr);
	pCmb->SetTextColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

	CIUIString strResFontID[COMBINEIMAGESIZE4];
	CControlProp::GetControlFont4(pProp, strResFontID);
	pCmb->SetTextFont(CONTROL_STATE_ALL,
		strResFontID[0], strResFontID[1], strResFontID[2], strResFontID[3], FALSE);

	RECT rcTextMargin = pProp->m_rcPadding4Text;
	// GetDPIPadding(&rcTextMargin);
	pCmb->SetTextMargin(CONTROL_STATE_ALL, &rcTextMargin, &rcTextMargin,
		&rcTextMargin, &rcTextMargin, FALSE);

	pCmb->SetTextAlignHor(CONTROL_STATE_ALL,
		pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode,
		pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode, FALSE);

	pCmb->SetTextAlignVer(CONTROL_STATE_ALL,
		pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode,
		pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode, FALSE);

	// Drop List
	ComboBox_SetDropHeight(hWnd, (LONG)pProp->m_lDropListHeight);

	COLORREF crItemBk = pProject->GetColor(pProp->m_strHighlightItemBkColorResID);
	pCmb->SetHighlightItemBkColor(crItemBk);

	pCmb = NULL;

	return 0;
}

int IUI::ComboBox_SetDropHeight(HWLWND hComboBox, LONG lHeight)
{
	return SendMessage(hComboBox, CBM_SET_DROP_HEIGHT, 0, lHeight);
}

int IUI::ComboBox_GetDropHeight(HWLWND hComboBox)
{
	return SendMessage(hComboBox, CBM_GET_DROP_HEIGHT, 0, 0);
}

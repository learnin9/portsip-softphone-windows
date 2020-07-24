#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{


	Button::Button()
	{
	}

	Button::~Button()
	{

	}

	BOOL Button::Create(
		__in CWLWndHandler *pUserHandler,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_BUTTON, lpszCaption,
				dwStyle, rect, hParent, pParent, lpszName);
	}

	int Button::BindStyle(const CControlProp *pCtrlProp)
	{
		BOOL bCheckBox = IsIncludeFlag(GetStyle(), BS_AUTOCHECKBOX);
		BOOL bRadioButton = IsIncludeFlag(GetStyle(), BS_AUTORADIOBUTTON);
		if (bCheckBox || bRadioButton)
		{
			return ToggleButton_BindStyle(m_hWnd, (CToggleButtonProp *)pCtrlProp);
		}

		return PushButton_BindStyle(m_hWnd, (CButtonProp *)pCtrlProp);
	}

	int Button::SetButtonClickArg(LPCTSTR lpszArg)
	{
		_ASSERT(IsWindow());
		return SendMessage(BM_SET_BUTTON_CLICK_ARG, (WPARAM)lpszArg, 0);
	}

	CIUIString Button::GetButtonClickArg() const
	{
		_ASSERT(IsWindow());
		return (LPCTSTR)SendMessage(BM_GET_BUTTON_CLICK_ARG, 0, 0);
	}

	int Button::SetBkColor(COLORREF crBkN, COLORREF crBkH, COLORREF crBkS, COLORREF crBkD)
	{
		_ASSERT(IsWindow());

		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_NORMAL), crBkN);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_HOVER), crBkH);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_PRESSED), crBkS);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_DISABLED), crBkD);

		Invalidate();

		return 0;
	}

	int Button::GetBkColor(COLORREF *pcrN, COLORREF *pcrH, COLORREF *pcrS, COLORREF *pcrD) const
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

	int Button::SetCheckedBkColor(COLORREF crCN, COLORREF crCH, COLORREF crCS, COLORREF crCD)
	{
		_ASSERT(IsWindow());

		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_NORMAL), crCN);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_HOVER), crCH);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_PRESSED), crCS);
		SendMessage(WLM_SET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_DISABLED), crCD);

		Invalidate();

		return 0;
	}

	int Button::GetCheckedBkColor(COLORREF *pcrCN, COLORREF *pcrCH, COLORREF *pcrCS, COLORREF *pcrCD) const
	{
		_ASSERT(IsWindow());

		if (pcrCN != NULL)
		{
			*pcrCN = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_NORMAL), 0);
		}
		if (pcrCH != NULL)
		{
			*pcrCH = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_HOVER), 0);
		}
		if (pcrCS != NULL)
		{
			*pcrCS = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_PRESSED), 0);
		}
		if (pcrCD != NULL)
		{
			*pcrCD = SendMessage(WLM_GET_BK_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetBkImage(
		UINT uMask,
		LPCTSTR lpszImageNameN,
		LPCTSTR lpszImageNameH,
		LPCTSTR lpszImageNameP,
		LPCTSTR lpszImageNameD,
		BOOL bRedraw/* = TRUE*/)
	{
		IUISetControlBkImage4(m_hWnd, FALSE, uMask,
			lpszImageNameN, lpszImageNameH, lpszImageNameP, lpszImageNameD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Button::GetBkImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameN,
		CIUIString *pstrImageNameH,
		CIUIString *pstrImageNameP,
		CIUIString *pstrImageNameD) const
	{
		_ASSERT(IsWindow());

		return IUIGetControlBkImage4(m_hWnd, FALSE, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameN, pstrImageNameH, pstrImageNameP, pstrImageNameD);
	}

	int Button::SetCheckedBkImage(
		UINT uMask,
		LPCTSTR lpszImageNameCN,
		LPCTSTR lpszImageNameCH,
		LPCTSTR lpszImageNameCP,
		LPCTSTR lpszImageNameCD,
		BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		IUISetControlBkImage4(m_hWnd, TRUE, uMask,
			lpszImageNameCN, lpszImageNameCH, lpszImageNameCP, lpszImageNameCD);

		if (bRedraw)
		{
			IUI::InvalidateRect(m_hWnd, NULL);
		}

		return 0;
	}

	int Button::GetCheckedBkImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameCN,
		CIUIString *pstrImageNameCH,
		CIUIString *pstrImageNameCP,
		CIUIString *pstrImageNameCD) const
	{
		_ASSERT(IsWindow());

		if (pbCombineImage == NULL)
		{
			return -1;
		}

		return IUIGetControlBkImage4(m_hWnd, TRUE, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameCN, pstrImageNameCH, pstrImageNameCP, pstrImageNameCD);
	}

	int Button::SetFocusedBkImage(LPCTSTR lpszImage)
	{
		_BTN *pBtn = (_BTN *)GetMember();
		IUISetControlImage(GetProject(), &pBtn->m_himgFocusedImage, lpszImage);
		Invalidate();

		return 0;
	}

	LPCTSTR Button::GetFocusedBkImage() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_himgFocusedImage->GetSafeImageName();
	}

	BOOL Button::CombineFgImage(BOOL bCombine)
	{
		return SendMessage(WLM_COMBINE_FG_IMAGE, 0, bCombine);
	}

	BOOL Button::IsCombineFgImage() const
	{
		return SendMessage(WLM_IS_COMBINE_FG_IMAGE, 0, 0);
	}

	int Button::SetCombineFgImage(LPCTSTR lpszImage)
	{
		_BTN *pBtn = (_BTN *)GetMember();
		IUISetControlImage(GetProject(), &pBtn->m_himgCombineFg, lpszImage);
		Invalidate();

		return 0;
	}

	LPCTSTR Button::GetCombineFgImage() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_himgCombineFg->GetSafeImageName();
	}

	int Button::SetFgImage(
		UINT uMask,
		LPCTSTR lpszImageNameN,
		LPCTSTR lpszImageNameH,
		LPCTSTR lpszImageNameP,
		LPCTSTR lpszImageNameD,
		BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		IUISetControlFgImage4(m_hWnd, FALSE, uMask,
			lpszImageNameN, lpszImageNameH, lpszImageNameP, lpszImageNameD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Button::GetFgImage(
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

		return IUIGetControlFgImage4(m_hWnd, FALSE, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameN, pstrImageNameH, pstrImageNameP, pstrImageNameD);
	}

	int Button::SetCheckedFgImage(
		UINT uMask,
		LPCTSTR lpszImageNameCN,
		LPCTSTR lpszImageNameCH,
		LPCTSTR lpszImageNameCP,
		LPCTSTR lpszImageNameCD,
		BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		IUISetControlFgImage4(m_hWnd, TRUE, uMask,
			lpszImageNameCN, lpszImageNameCH, lpszImageNameCP, lpszImageNameCD);

		if (bRedraw)
		{
			IUI::InvalidateRect(m_hWnd, NULL);
		}

		return 0;
	}

	int Button::GetCheckedFgImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameCN,
		CIUIString *pstrImageNameCH,
		CIUIString *pstrImageNameCP,
		CIUIString *pstrImageNameCD) const
	{
		return IUIGetControlFgImage4(m_hWnd, TRUE, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameCN, pstrImageNameCH, pstrImageNameCP, pstrImageNameCD);
	}

	int Button::SetFgImageMargin(LPCRECT lprcMargin)
	{
		if (lprcMargin == NULL)
		{
			return -1;
		}

		_BTN *pBtn = (_BTN *)GetMember();
		pBtn->m_rcForegroundImageMargin = lprcMargin;

		return 0;
	}

	int Button::GetFgImageMargin(LPRECT lprcMargin) const
	{
		if (lprcMargin == NULL)
		{
			return -1;
		}

		_BTN *pBtn = (_BTN *)GetMember();
		*lprcMargin = pBtn->m_rcForegroundImageMargin;

		return 0;
	}

	FOREGROUND_ALIGN_HOR Button::SetFgImageAlignHor(FOREGROUND_ALIGN_HOR eAlign)
	{
		_BTN *pBtn = (_BTN *)GetMember();
		FOREGROUND_ALIGN_HOR eOld = pBtn->m_eForegroundImageAlignHor;
		pBtn->m_eForegroundImageAlignHor = eAlign;
		return eOld;
	}

	FOREGROUND_ALIGN_HOR Button::GetFgImageAlignHor() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_eForegroundImageAlignHor;
	}

	FOREGROUND_ALIGN_VER Button::SetFgImageAlignVer(FOREGROUND_ALIGN_VER eAlign)
	{
		_BTN *pBtn = (_BTN *)GetMember();
		FOREGROUND_ALIGN_VER eOld = pBtn->m_eForegroundImageAlignVer;
		pBtn->m_eForegroundImageAlignVer = eAlign;
		return eOld;
	}

	FOREGROUND_ALIGN_VER Button::GetFgImageAlignVer() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_eForegroundImageAlignVer;
	}

	int Button::SetBorderColor(UINT uMask,
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

	int Button::GetBorderColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH,
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

	int Button::SetCheckedBorderColor(COLORREF crCN, COLORREF crCH, COLORREF crCS, COLORREF crCD)
	{
		_ASSERT(IsWindow());

		SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_NORMAL), crCN);
		SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_HOVER), crCH);
		SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_PRESSED), crCS);
		SendMessage(WLM_SET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_DISABLED), crCD);

		IUI::InvalidateRect(m_hWnd, NULL);

		return 0;
	}

	int Button::GetCheckedBorderColor(COLORREF *pcrCN, COLORREF *pcrCH, COLORREF *pcrCS, COLORREF *pcrCD) const
	{
		_ASSERT(IsWindow());

		if (pcrCN != NULL)
		{
			*pcrCN = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_NORMAL), 0);
		}
		if (pcrCH != NULL)
		{
			*pcrCH = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_HOVER), 0);
		}
		if (pcrCS != NULL)
		{
			*pcrCS = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_PRESSED), 0);
		}
		if (pcrCD != NULL)
		{
			*pcrCD = SendMessage(WLM_GET_BORDER_COLOR, MAKEWPARAM(BST_CHECKED, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetTextColor(UINT uMask, COLORREF crN, COLORREF crH,
		COLORREF crS, COLORREF crD, BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_NORMAL), crN);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_HOVER), crH);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_PRESSED), crS);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_DISABLED), crD);

		Invalidate();

		return 0;
	}

	int Button::GetTextColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH,
		COLORREF *pcrS, COLORREF *pcrD) const
	{
		_ASSERT(IsWindow());

		if (pcrN != NULL)
		{
			*pcrN = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_NORMAL), 0);
		}
		if (pcrH != NULL)
		{
			*pcrH = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_HOVER), 0);
		}
		if (pcrS != NULL)
		{
			*pcrS = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_PRESSED), 0);
		}
		if (pcrD != NULL)
		{
			*pcrD = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(FALSE, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetCheckedTextColor(UINT uMask, COLORREF crN, COLORREF crH,
		COLORREF crS, COLORREF crD, BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_NORMAL), crN);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_HOVER), crH);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_PRESSED), crS);
		SendMessage(WLM_SET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_DISABLED), crD);

		Invalidate();

		return 0;
	}

	int Button::GetCheckedTextColor(UINT uMask, COLORREF *pcrN, COLORREF *pcrH,
		COLORREF *pcrS, COLORREF *pcrD) const
	{
		_ASSERT(IsWindow());

		if (pcrN != NULL)
		{
			*pcrN = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_NORMAL), 0);
		}
		if (pcrH != NULL)
		{
			*pcrH = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_HOVER), 0);
		}
		if (pcrS != NULL)
		{
			*pcrS = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_PRESSED), 0);
		}
		if (pcrD != NULL)
		{
			*pcrD = SendMessage(WLM_GET_TEXT_COLOR, MAKEWPARAM(TRUE, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetFocusedTextColor(COLORREF cr)
	{
		_BTN *pBtn = (_BTN *)GetMember();
		pBtn->m_dwFocusedTextColor = cr;

		return 0;
	}

	COLORREF Button::GetFocusedTextColor() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_dwFocusedTextColor;
	}

	int Button::SetTextFont(
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

	int Button::GetTextFont(
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

	int Button::SetCheckedTextFont(
		UINT uMask,
		LPCTSTR lpszFontIDCN,
		LPCTSTR lpszFontIDCH,
		LPCTSTR lpszFontIDCS,
		LPCTSTR lpszFontIDCD,
		BOOL bRedraw)
	{
		_BTN *pBtn = (_BTN *)GetMember();

		IUISetControlCheckedFont4(GetProject(), uMask, pBtn->m_hIUIFontC,
			lpszFontIDCN, lpszFontIDCH, lpszFontIDCS, lpszFontIDCD);

		if (bRedraw)
		{
			IUI::InvalidateRect(m_hWnd, NULL);
		}

		return 0;
	}

	int Button::GetCheckedTextFont(
		UINT uMask,
		CIUIString *pstrFontIDCN,
		CIUIString *pstrFontIDCH,
		CIUIString *pstrFontIDCS,
		CIUIString *pstrFontIDCD) const
	{
		_BTN *pBtn = (_BTN *)GetMember();

		if (pstrFontIDCN != NULL)
		{
			GetFontResID(pBtn->m_hIUIFontC[0], pstrFontIDCN);
		}
		if (pstrFontIDCH != NULL)
		{
			GetFontResID(pBtn->m_hIUIFontC[1], pstrFontIDCH);
		}
		if (pstrFontIDCS != NULL)
		{
			GetFontResID(pBtn->m_hIUIFontC[2], pstrFontIDCS);
		}
		if (pstrFontIDCD != NULL)
		{
			GetFontResID(pBtn->m_hIUIFontC[3], pstrFontIDCD);
		}

		return 0;
	}

	int Button::SetTextMargin(
		UINT uMask,
		LPCRECT lprcTextMarginN,
		LPCRECT lprcTextMarginH,
		LPCRECT lprcTextMarginP,
		LPCRECT lprcTextMarginD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lprcTextMarginH);
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lprcTextMarginP);
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Button::GetTextMargin(
		UINT uMask,
		LPRECT lprcTextMarginN,
		LPRECT lprcTextMarginH,
		LPRECT lprcTextMarginP,
		LPRECT lprcTextMarginD) const
	{
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_HOVER), (LPARAM)lprcTextMarginH);
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_PRESSED), (LPARAM)lprcTextMarginP);
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

		return 0;
	}

	int Button::SetTextAlignHor(
		UINT uMask,
		TEXT_ALIGN_HOR eHorAlignModeN,
		TEXT_ALIGN_HOR eHorAlignModeH,
		TEXT_ALIGN_HOR eHorAlignModeS,
		TEXT_ALIGN_HOR eHorAlignModeD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_NORMAL), eHorAlignModeN);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_HOVER), eHorAlignModeH);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_PRESSED), eHorAlignModeS);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_DISABLED), eHorAlignModeD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Button::GetTextAlignHor(
		UINT uMask,
		TEXT_ALIGN_HOR *peHorAlignModeN,
		TEXT_ALIGN_HOR *peHorAlignModeH,
		TEXT_ALIGN_HOR *peHorAlignModeS,
		TEXT_ALIGN_HOR *peHorAlignModeD) const
	{
		if (peHorAlignModeN != NULL)
		{
			*peHorAlignModeN = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
		}
		if (peHorAlignModeH != NULL)
		{
			*peHorAlignModeH = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_HOVER), 0);
		}
		if (peHorAlignModeS != NULL)
		{
			*peHorAlignModeS = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_PRESSED), 0);
		}
		if (peHorAlignModeD != NULL)
		{
			*peHorAlignModeD = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetTextAlignVer(
		UINT uMask,
		TEXT_ALIGN_VER eVerAlignModeN,
		TEXT_ALIGN_VER eVerAlignModeH,
		TEXT_ALIGN_VER eVerAlignModeS,
		TEXT_ALIGN_VER eVerAlignModeD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_NORMAL), eVerAlignModeN);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_HOVER), eVerAlignModeH);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_PRESSED), eVerAlignModeS);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_DISABLED), eVerAlignModeD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Button::GetTextAlignVer(
		UINT uMask,
		TEXT_ALIGN_VER *peVerAlignModeN,
		TEXT_ALIGN_VER *peVerAlignModeH,
		TEXT_ALIGN_VER *peVerAlignModeS,
		TEXT_ALIGN_VER *peVerAlignModeD) const
	{
		if (peVerAlignModeN != NULL)
		{
			*peVerAlignModeN = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_NORMAL), 0);
		}
		if (peVerAlignModeH != NULL)
		{
			*peVerAlignModeH = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_HOVER), 0);
		}
		if (peVerAlignModeS != NULL)
		{
			*peVerAlignModeS = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_PRESSED), 0);
		}
		if (peVerAlignModeD != NULL)
		{
			*peVerAlignModeD = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Button::SetCheck(int nCheck)
	{
		_ASSERT(IsWindow());
		return SendMessage(BM_SETCHECK, nCheck, 0);
	}

	int Button::GetCheck() const
	{
		_BTN *pBtn = (_BTN *)GetMember();
		return pBtn->m_nCheck;
	}

	int Button::SetBindModalDlgID(UINT uID)
	{
		return Button_SetBindModalDlgID(m_hWnd, uID);
	}

	UINT Button::GetBindModalDlgID() const
	{
		return Button_GetBindModalDlgID(m_hWnd);
	}

	int Button::SetBindModelessWndID(UINT uID)
	{
		return Button_SetBindModelessWndID(m_hWnd, uID);
	}

	UINT Button::GetBindModelessWndID() const
	{
		return Button_GetBindModelessWndID(m_hWnd);
	}

	int Button::SetBindTabControl(LPCTSTR lpszTabControlName)
	{
		return RadioButton_SetBindTabControl(m_hWnd, lpszTabControlName);
	}

	LPCTSTR Button::GetBindTabControl() const
	{
		return RadioButton_GetBindTabControl(m_hWnd);
	}

	int Button::SetBindPropertySheet(LPCTSTR lpszTabControlName)
	{
		return RadioButton_SetBindPropertySheet(m_hWnd, lpszTabControlName);
	}

	LPCTSTR Button::GetBindPropertySheet() const
	{
		return RadioButton_GetBindPropertySheet(m_hWnd);
	}

	int Button::SetButtonType(int nButtonType)
	{
		return Button_SetButtonType(m_hWnd, nButtonType);
	}

	int Button::GetButtonType() const
	{
		return Button_GetButtonType(m_hWnd);
	}

	//////////////////////////////////////////////////////////////////////////
	// Button API
	int IUI::PushButton_BindStyle(HWLWND hWnd, const CButtonProp *pProp)
	{
		Button *pButton = (Button *)CWLWnd::FromHandle(hWnd);

		// 布局
		SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pProp->m_eHorLayoutMode);
		SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pProp->m_eVerLayoutMode);
		SetDockProperty(hWnd, (DOCK)pProp->m_eDockProperty);

		SetLayoutMargin(hWnd, &(RECT)pProp->m_rcLayoutMargin);
		SetLayoutInset(hWnd, &(RECT)pProp->m_rcInset);

		//
		// Button类属性
		//

		RECT rcControl = pProp->m_rcControl;
		MakeControlMargin(GetHostWnd(hWnd), GetParent(hWnd), &rcControl, hWnd->m_rcControlMargin);
		SetMouseTransparent(hWnd, (bool)pProp->m_bMouseTransparent ? TRUE : FALSE);

		pButton->SetButtonType((BTN_TYPE)pProp->m_eBtnType);

		// 背景颜色
		pButton->SetDrawBkColor((bool)pProp->m_bDrawBkColor);

		COLORREF cr[COMBINEIMAGESIZE4] = {0};
		GetProject(hWnd)->GetControlBkColor4(pProp, cr);
		pButton->SetBkColor(cr[0], cr[1], cr[2], cr[3]);

		// 背景图片
		CIUIString strImageName[1 + COMBINEIMAGESIZE4];
		BOOL bCombineImage = TRUE;
		CControlProp::GetBackground4(pProp, &bCombineImage, strImageName);
		if (bCombineImage)
		{
			pButton->CombineBkImage(TRUE);
			pButton->SetCombineBkImage(strImageName[0]);
		}
		else
		{
			pButton->CombineBkImage(FALSE);
			pButton->SetBkImage(CONTROL_STATE_ALL,
				strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
		}
		SetBkImageResizeMode(hWnd, pProp->m_eBkImageResizeMode);
		SetBkImage9GridResizeParam(hWnd, &(RECT)pProp->m_rcBkImage9GridResizeArg);

		// 前景
		CIUIString strImageNameFg[1 + COMBINEIMAGESIZE4];
		int nRet = CControlProp::GetButtonForeground4(pProp, &bCombineImage, strImageNameFg);
		if (nRet == 0)
		{
			if (bCombineImage)
			{
				pButton->SetCombineFgImage(strImageNameFg[0]);
				pButton->CombineFgImage(TRUE);
			}
			else
			{
				pButton->SetFgImage(CONTROL_STATE_ALL,
					strImageNameFg[1], strImageNameFg[2],
					strImageNameFg[3], strImageNameFg[4], FALSE);
				pButton->CombineFgImage(FALSE);
			}

			CIUIRect rcFgMargin = pProp->m_rcPadding4Foreground;
			//GetDPIPadding(rcFgMargin);
			pButton->SetFgImageMargin(rcFgMargin);
			pButton->SetFgImageAlignHor(pProp->m_eForegroundHorAlignMode);
			pButton->SetFgImageAlignVer(pProp->m_eForegroundVerAlignMode);
		}

		// 边框
		pButton->SetDrawBorderColor((bool)pProp->m_bDrawBorderColor);

		GetProject(hWnd)->GetControlBorderColor4(pProp, cr);
		pButton->SetBorderColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

		// 文本
		BindTextAlignStyle(hWnd, pProp);

		pButton->SetTextAlignHor(CONTROL_STATE_ALL,
			pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode,
			pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode, FALSE);

		pButton->SetTextAlignVer(CONTROL_STATE_ALL,
			pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode,
			pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode, FALSE);

		GetProject(hWnd)->GetControlTextColor4(pProp, cr);
		pButton->SetTextColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

		CIUIString strResFontID[COMBINEIMAGESIZE4];
		CControlProp::GetControlFont4(pProp, strResFontID);
		pButton->SetTextFont(CONTROL_STATE_ALL,
			strResFontID[0], strResFontID[1], strResFontID[2], strResFontID[3], FALSE);

		RECT rcTextMargin = pProp->m_rcPadding4Text;
		// GetDPIPadding(&rcTextMargin);
		pButton->SetTextMargin(CONTROL_STATE_ALL, &rcTextMargin, &rcTextMargin,
			&rcTextMargin, &rcTextMargin, FALSE);

		// Tooltip
		CControlProp *pToolTipsStyle = GetProject(hWnd)->GetStyleItem(STYLET_TOOLTIPS,
				(LPCTSTR)pProp->m_strToolTipStyle);
		Button_SetToolTips(hWnd, (const CToolTipCtrlProp *)pToolTipsStyle,
			(LPCTSTR)pProp->m_strToolTip);

		HWND hToolTips = GetToolTips(hWnd);
		if (hToolTips != NULL)
		{
			ToolTipCtrl *pToolTipCtrl = (ToolTipCtrl *)Window::FromHandlePermanent(hToolTips);
			if ((LONG)pProp->m_lAutoPopDelayTime != GetDoubleClickTime() * 10)
			{
				pToolTipCtrl->SetDelayTime(TTDT_AUTOPOP, (LONG)pProp->m_lAutoPopDelayTime);
			}

			if ((LONG)pProp->m_lInitialDelayTime != 500)
			{
				pToolTipCtrl->SetDelayTime(TTDT_INITIAL, (LONG)pProp->m_lInitialDelayTime);
			}

			if ((LONG)pProp->m_lReshowDelayTime != 100)
			{
				pToolTipCtrl->SetDelayTime(TTDT_RESHOW, (LONG)pProp->m_lReshowDelayTime);
			}
		}

		//
		pButton->SetButtonClickArg((LPCTSTR)pProp->m_strButtonClickArg);

		pButton = NULL;

		return 0;
	}

	int IUI::ToggleButton_BindStyle(HWLWND hWnd, const CToggleButtonProp *pProp)
	{
		// 布局
		Button *pButton = (Button *)CWLWnd::FromHandle(hWnd);
		//
		// 基类属性
		//
		SetWindowText(hWnd, pProp->m_strCaption);

		// 布局
		SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pProp->m_eHorLayoutMode);
		SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pProp->m_eVerLayoutMode);
		SetDockProperty(hWnd, (DOCK)pProp->m_eDockProperty);

		SetLayoutMargin(hWnd, &(RECT)pProp->m_rcLayoutMargin);
		SetLayoutInset(hWnd, &(RECT)pProp->m_rcInset);

		//
		// Button类属性
		//

		RECT rcControl = pProp->m_rcControl;
		MakeControlMargin(GetHostWnd(hWnd), GetParent(hWnd), &rcControl, hWnd->m_rcControlMargin);
		SetMouseTransparent(hWnd, (bool)pProp->m_bMouseTransparent ? TRUE : FALSE);

		pButton->SetButtonType((CHK_TYPE)pProp->m_eBtnType);

		// 背景颜色
		pButton->SetDrawBkColor((bool)pProp->m_bDrawBkColor);

		COLORREF cr[COMBINEIMAGESIZE4] = {0};
		GetProject(hWnd)->GetControlBkColor4(pProp, cr);
		pButton->SetBkColor(cr[0], cr[1], cr[2], cr[3]);

		GetProject(hWnd)->GetControlCheckedBkColor4(pProp, cr);
		pButton->SetCheckedBkColor(cr[0], cr[1], cr[2], cr[3]);

		// 背景图片
		CIUIString strImageName[1 + COMBINEIMAGESIZE8];
		BOOL bCombineImage = TRUE;
		CControlProp::GetToggleButtonBackground8(pProp, &bCombineImage, strImageName);
		if (bCombineImage)
		{
			CombineBkImage(hWnd, TRUE);
			SetCombineBkImage(hWnd, strImageName[0]);
		}
		else
		{
			CombineBkImage(hWnd, FALSE);
			pButton->SetBkImage(CONTROL_STATE_ALL,
				strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);

			pButton->SetCheckedBkImage(CONTROL_STATE_ALL,
				strImageName[5], strImageName[6], strImageName[7], strImageName[8], FALSE);
		}
		SetBkImageResizeMode(hWnd, pProp->m_eBkImageResizeMode);
		SetBkImage9GridResizeParam(hWnd, &(RECT)pProp->m_rcBkImage9GridResizeArg);

		// 前景
		CIUIString strImageNameFg[1 + COMBINEIMAGESIZE8];
		int nRet = CControlProp::GetToggleButtonForeground8(pProp, &bCombineImage, strImageNameFg);
		if (nRet == 0)
		{
			CombineFgImage(hWnd, bCombineImage);

			if (bCombineImage)
			{
				SetCombineFgImage(hWnd, strImageNameFg[0]);
			}
			else
			{
				pButton->SetFgImage(CONTROL_STATE_ALL,
					strImageNameFg[1], strImageNameFg[2],
					strImageNameFg[3], strImageNameFg[4], FALSE);
				pButton->SetCheckedFgImage(CONTROL_STATE_ALL,
					strImageNameFg[5], strImageNameFg[6],
					strImageNameFg[7], strImageNameFg[8], FALSE);
			}

			CIUIRect rcFgMargin = pProp->m_rcPadding4Foreground;
			//GetDPIPadding(rcFgMargin);
			pButton->SetFgImageMargin(rcFgMargin);
			pButton->SetFgImageAlignHor(pProp->m_eForegroundHorAlignMode);
			pButton->SetFgImageAlignVer(pProp->m_eForegroundVerAlignMode);
		}

		// 边框
		pButton->SetDrawBorderColor((bool)pProp->m_bDrawBorderColor);

		GetProject(hWnd)->GetControlBorderColor4(pProp, cr);
		pButton->SetBorderColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

		GetProject(hWnd)->GetControlCheckedBorderColor4(pProp, cr);
		pButton->SetCheckedBorderColor(cr[0], cr[1], cr[2], cr[3]);

		// 文本
		BindTextAlignStyle(hWnd, pProp);

		pButton->SetTextAlignHor(CONTROL_STATE_ALL,
			pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode,
			pProp->m_eTextHorAlignMode, pProp->m_eTextHorAlignMode, FALSE);

		pButton->SetTextAlignVer(CONTROL_STATE_ALL,
			pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode,
			pProp->m_eTextVerAlignMode, pProp->m_eTextVerAlignMode, FALSE);

		GetProject(hWnd)->GetControlTextColor4(pProp, cr);
		pButton->SetTextColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

		GetProject(hWnd)->GetControlCheckedTextColor4(pProp, cr);
		pButton->SetCheckedTextColor(CONTROL_STATE_ALL, cr[0], cr[1], cr[2], cr[3], FALSE);

		CIUIString strResFontID[COMBINEIMAGESIZE8];
		CControlProp::GetControlFont8(pProp, strResFontID);
		pButton->SetTextFont(CONTROL_STATE_ALL,
			strResFontID[0], strResFontID[1], strResFontID[2], strResFontID[3], FALSE);
		pButton->SetCheckedTextFont(CONTROL_STATE_ALL,
			strResFontID[4], strResFontID[5], strResFontID[6], strResFontID[7], FALSE);

		RECT rcTextMargin = pProp->m_rcPadding4Text;
		// GetDPIPadding(&rcTextMargin);
		pButton->SetTextMargin(CONTROL_STATE_ALL, &rcTextMargin, &rcTextMargin,
			&rcTextMargin, &rcTextMargin, FALSE);

		//pButton->SetBindModalDlgID(pProp->m_strBindXml);

		BOOL bCheckBox = IsIncludeFlag(GetStyle(hWnd), BS_AUTOCHECKBOX);
		BOOL bRadioButton = IsIncludeFlag(GetStyle(hWnd), BS_AUTORADIOBUTTON);
		if (bCheckBox)
		{
			const CCheckBoxProp *pCheckBoxProp = dynamic_cast<const CCheckBoxProp *>(pProp);
			pButton->SetButtonType((CHK_TYPE)pCheckBoxProp->m_eChkType);
		}
		else if (bRadioButton)
		{
			const CRadioButtonProp *pRadioButtonProp = dynamic_cast<const CRadioButtonProp *>(pProp);
			pButton->SetBindTabControl((LPCTSTR)pRadioButtonProp->m_strBindTaskMgrID);
			pButton->SetBindPropertySheet((LPCTSTR)pRadioButtonProp->m_strBindTaskWndID);
		}

		// Tooltip
		CControlProp *pToolTipsStyle = GetProject(hWnd)->GetStyleItem(STYLET_TOOLTIPS,
				(LPCTSTR)pProp->m_strToolTipStyle);
		Button_SetToolTips(hWnd, (const CToolTipCtrlProp *)pToolTipsStyle,
			(LPCTSTR)pProp->m_strToolTip);

		HWND hToolTips = GetToolTips(hWnd);
		if (hToolTips != NULL)
		{
			ToolTipCtrl *pToolTipCtrl = (ToolTipCtrl *)Window::FromHandlePermanent(hToolTips);
			if ((LONG)pProp->m_lAutoPopDelayTime != GetDoubleClickTime() * 10)
			{
				pToolTipCtrl->SetDelayTime(TTDT_AUTOPOP, (LONG)pProp->m_lAutoPopDelayTime);
			}

			if ((LONG)pProp->m_lInitialDelayTime != 500)
			{
				pToolTipCtrl->SetDelayTime(TTDT_INITIAL, (LONG)pProp->m_lInitialDelayTime);
			}

			if ((LONG)pProp->m_lReshowDelayTime != 100)
			{
				pToolTipCtrl->SetDelayTime(TTDT_RESHOW, (LONG)pProp->m_lReshowDelayTime);
			}
		}

		//
		pButton->SetButtonClickArg((LPCTSTR)pProp->m_strButtonClickArg);
		
		pButton = NULL;

		return 0;
	}

	int IUI::Button_SetBindModalDlgID(HWLWND hButton, UINT uID)
	{
		return 0;
	}

	UINT IUI::Button_GetBindModalDlgID(HWLWND hButton)
	{
		return 0;
	}

	int IUI::Button_SetBindModelessWndID(HWLWND hButton, UINT uID)
	{
		return 0;
	}

	UINT IUI::Button_GetBindModelessWndID(HWLWND hButton)
	{
		return 0;
	}

	int IUI::RadioButton_SetBindTabControl(HWLWND hButton, LPCTSTR lpszTabControlName)
	{
		_BTN *pBtn = (_BTN *)GetMember(hButton);

		pBtn->m_strBindTabControlName = lpszTabControlName;

		return 0;
	}

	LPCTSTR IUI::RadioButton_GetBindTabControl(HWLWND hButton)
	{
		_BTN *pBtn = (_BTN *)GetMember(hButton);

		return pBtn->m_strBindTabControlName;
	}

	int IUI::RadioButton_SetBindPropertySheet(HWLWND hButton, LPCTSTR lpszPropertySheetName)
	{
		_BTN *pBtn = (_BTN *)GetMember(hButton);

		pBtn->m_strBindPropertySheetName = lpszPropertySheetName;

		return 0;
	}

	LPCTSTR IUI::RadioButton_GetBindPropertySheet(HWLWND hButton)
	{
		_BTN *pBtn = (_BTN *)GetMember(hButton);

		return pBtn->m_strBindPropertySheetName;

		return 0;
	}

	int IUI::Button_SetButtonType(HWLWND hButton, int nButtonType)
	{
		_ASSERT(IsWindow(hButton));
		return SendMessage(hButton, BM_SET_BUTTON_TYPE, nButtonType, 0);
	}

	int IUI::Button_GetButtonType(HWLWND hButton)
	{
		_ASSERT(IsWindow(hButton));
		return SendMessage(hButton, BM_GET_BUTTON_TYPE, 0, 0);
	}

	int IUI::Button_SetToolTips(
		HWLWND hButton,
		const CToolTipCtrlProp *pToolTipsCtrlProp,
		LPCTSTR lpszText)
	{
		_ASSERT(IsWindow(hButton));
		return SendMessage(hButton, BM_SETTOOLTIPS,
				(WPARAM)pToolTipsCtrlProp, (LPARAM)lpszText);
	}
}

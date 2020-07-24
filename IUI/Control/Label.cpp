#include "StdAfx.h"


#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{

	Label::Label()
	{
	}

	Label::~Label()
	{
	}

	BOOL Label::Create(
		__in CWLWndHandler *pUserHandler,
		LPCTSTR lpszCaption,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_LABEL, lpszCaption,
				dwStyle, rect, hParent, pParent, lpszName);
	}

	int Label::BindStyle(const CControlProp *pCtrlProp)
	{
		return IUI::Label_BindStyle(m_hWnd, (CLabelProp *)pCtrlProp);
	}

	UINT Label::SetTextStyle(UINT uStyle)
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		UINT uOld = pLb->hwnd->ci.m_uTextStyle;
		pLb->hwnd->ci.m_uTextStyle = uStyle;
		Invalidate();

		return uOld;
	}

	UINT Label::GetTextStyle() const
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		return pLb->hwnd->ci.m_uTextStyle;
	}

	UINT Label::AddTextStyle(UINT uAddStyle)
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		UINT uOld = pLb->hwnd->ci.m_uTextStyle;
		pLb->hwnd->ci.m_uTextStyle |= uAddStyle;
		Invalidate();

		return uOld;
	}

	UINT Label::RemoveTextStyle(UINT uRomoveStyle)
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		UINT uOld = pLb->hwnd->ci.m_uTextStyle;
		pLb->hwnd->ci.m_uTextStyle &= ~uRomoveStyle;
		Invalidate();

		return uOld;
	}

	int Label::SetTextFont(UINT uMask, LPCTSTR lpszFontIDN, LPCTSTR lpszFontIDD, BOOL bRedraw)
	{
		_ASSERT(IsWindow());

		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			SendMessage(WLM_SET_TEXT_FONT,
				MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lpszFontIDN);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			SendMessage(WLM_SET_TEXT_FONT,
				MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lpszFontIDD);
		}

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Label::GetTextFont(UINT uMask, CIUIString *pstrFontIDN, CIUIString *pstrFontIDD) const
	{
		_ASSERT(IsWindow());

		if (IsIncludeFlag(uMask, CONTROL_STATE_NORMAL))
		{
			if (pstrFontIDN == NULL)
			{
				return -1;
			}

			*pstrFontIDN = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT,
					MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
		}

		if (IsIncludeFlag(uMask, CONTROL_STATE_DISABLED))
		{
			if (pstrFontIDD == NULL)
			{
				return -1;
			}

			*pstrFontIDD = (LPCTSTR)SendMessage(WLM_GET_TEXT_FONT,
					MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Label::SetTextMargin(
		UINT uMask,
		LPCRECT lprcTextMarginN,
		LPCRECT lprcTextMarginD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
		SendMessage(WLM_SET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Label::GetTextMargin(
		UINT uMask,
		LPRECT lprcTextMarginN,
		LPRECT lprcTextMarginD) const
	{
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_NORMAL), (LPARAM)lprcTextMarginN);
		SendMessage(WLM_GET_TEXT_MARGIN, MAKEWPARAM(0, CONTROL_STATE_DISABLED), (LPARAM)lprcTextMarginD);

		return 0;
	}

	int Label::SetTextAlignHor(
		UINT uMask,
		TEXT_ALIGN_HOR eHorAlignModeN,
		TEXT_ALIGN_HOR eHorAlignModeD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_NORMAL), eHorAlignModeN);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(0, CONTROL_STATE_DISABLED), eHorAlignModeD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Label::GetTextAlignHor(
		UINT uMask,
		TEXT_ALIGN_HOR *peHorAlignModeN,
		TEXT_ALIGN_HOR *peHorAlignModeD) const
	{
		if (peHorAlignModeN != NULL)
		{
			*peHorAlignModeN = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
		}
		if (peHorAlignModeD != NULL)
		{
			*peHorAlignModeD = (TEXT_ALIGN_HOR)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	int Label::SetTextAlignVer(
		UINT uMask,
		TEXT_ALIGN_VER eVerAlignModeN,
		TEXT_ALIGN_VER eVerAlignModeD,
		BOOL bRedraw)
	{
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_NORMAL), eVerAlignModeN);
		SendMessage(WLM_SET_TEXT_ALIGN_MODE, MAKEWPARAM(1, CONTROL_STATE_DISABLED), eVerAlignModeD);

		if (bRedraw)
		{
			Invalidate();
		}

		return 0;
	}

	int Label::GetTextAlignVer(
		UINT uMask,
		TEXT_ALIGN_VER *peVerAlignModeN,
		TEXT_ALIGN_VER *peVerAlignModeD) const
	{
		if (peVerAlignModeN != NULL)
		{
			*peVerAlignModeN = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_NORMAL), 0);
		}
		if (peVerAlignModeD != NULL)
		{
			*peVerAlignModeD = (TEXT_ALIGN_VER)SendMessage(WLM_GET_TEXT_ALIGN_MODE,
					MAKEWPARAM(1, CONTROL_STATE_DISABLED), 0);
		}

		return 0;
	}

	BOOL Label::SetWidthMatchText(BOOL bMatch)
	{
		_LABEL *pLb = (_LABEL *)GetMember();

		BOOL bOld = pLb->m_bWidthMatchText;
		pLb->m_bWidthMatchText = bMatch;

		// 如果宽度与文本自适应，调整它的宽度。
		if (pLb->m_bWidthMatchText)
		{
			CIUIString strText = CWLWnd::GetWindowText();

			if (!strText.IsEmpty() && GetHostWnd() != NULL)
			{
				// 计算文字的宽度
				HFONT hFont = pLb->m_hIUIFont[0]->GetSafeHFont();
				HDC hdcN = ::GetWindowDC(NULL);
				HFONT hOldFont = (HFONT)::SelectObject(hdcN, hFont);
				CIUIRect rcText = GetWindowRectToParent();
				int nHeight = ::DrawText(hdcN, strText, strText.GetLength(), &rcText,
						DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
				::SelectObject(hdcN, hOldFont);
				::ReleaseDC(NULL, hdcN);

				// 设置控件的宽度
				BOOL bOldDPIState = IsDPIEnabled();
				EnableDPI(false); //rcText是已经调整好的

				CIUIRect rcWnd;
				GetWindowRectToParent(rcWnd);
				CIUIRect rcTextPadding = GetTextPadding();
				rcWnd.right = rcWnd.left + rcText.Width()
					+ rcTextPadding.left + rcTextPadding.right;
				MoveWindow(rcWnd);

				EnableDPI(bOldDPIState);
			}
		}

		return bOld;
	}

	BOOL Label::IsWidthMatchText() const
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		return pLb->m_bWidthMatchText;
	}

	BOOL Label::SetHeightMatchText(BOOL bMatch)
	{
		_LABEL *pLb = (_LABEL *)GetMember();

		BOOL bOld = pLb->m_bHeightMatchText;
		pLb->m_bHeightMatchText = bMatch;

		// 如果高度与文本自适应，调整它的高度。
		if (pLb->m_bHeightMatchText)
		{
			CIUIString strText = CWLWnd::GetWindowText();

			if (!strText.IsEmpty() && GetHostWnd() != NULL)
			{
				// 计算文字的尺寸
				HFONT hFont = pLb->m_hIUIFont[0]->GetSafeHFont();
				HDC hdcN = ::GetWindowDC(NULL);
				HFONT hOldFont = (HFONT)::SelectObject(hdcN, hFont);
				CIUIRect rcText = GetWindowRectToParent();
				int nHeight = ::DrawText(hdcN, strText, strText.GetLength(), &rcText,
						DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
				::SelectObject(hdcN, hOldFont);
				::ReleaseDC(NULL, hdcN);

				// 设置控件的高度
				BOOL bOldDPIState = IsDPIEnabled();
				EnableDPI(false); //rcText是已经调整好的

				CIUIRect rcWnd;
				GetWindowRectToParent(rcWnd);

				CIUIRect rcTextPadding = GetTextPadding();
				rcWnd.bottom = rcWnd.top + rcText.Height()
					+ rcTextPadding.top + rcTextPadding.bottom;
				MoveWindow(rcWnd);

				EnableDPI(bOldDPIState);
			}
		}

		return bOld;
	}

	BOOL Label::IsHeightMatchText() const
	{
		_LABEL *pLb = (_LABEL *)GetMember();
		return pLb->m_bHeightMatchText;
	}

	SIZE Label::EstimateSize(SIZE szAvailable)
	{
		return CWLWnd::EstimateSize(szAvailable);
	}

	int Label::SetBkImage(
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

	int Label::GetBkImage(
		UINT uMask,
		BOOL *pbCombineImage,
		CIUIString *pstrCombineImageName,
		CIUIString *pstrImageNameN, CIUIString *pstrImageNameD) const
	{
		_ASSERT(IsWindow());

		return IUIGetControlBkImage2(m_hWnd, uMask, pbCombineImage, pstrCombineImageName,
				pstrImageNameN, pstrImageNameD);
	}

	int Label::SetButtonClickArg(LPCTSTR lpszArg)
	{
		_ASSERT(IsWindow());
		return SendMessage(BM_SET_BUTTON_CLICK_ARG, (WPARAM)lpszArg, 0);
	}

	CIUIString Label::GetButtonClickArg() const
	{
		_ASSERT(IsWindow());
		return (LPCTSTR)SendMessage(BM_GET_BUTTON_CLICK_ARG, 0, 0);
	}

	SIZE Label::EstimateTextSize(LPCTSTR lpszText)
	{
		SIZE size = {0};

		if (lpszText == NULL)
		{
			return size;
		}

		_LABEL *pLb = (_LABEL *)GetMember();

		CIUIString strText = lpszText;
		if (!strText.IsEmpty() && GetHostWnd() != NULL)
		{
			// 计算文字的宽度
			HFONT hFont = pLb->m_hIUIFont[0]->GetSafeHFont();
			HDC hdcN = ::GetWindowDC(NULL);
			HFONT hOldFont = (HFONT)::SelectObject(hdcN, hFont);
			RECT rcText = GetWindowRectToParent();
			int nHeight = ::DrawText(hdcN, strText, strText.GetLength(), &rcText,
					DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
			::SelectObject(hdcN, hOldFont);
			::ReleaseDC(NULL, hdcN);

			size.cx = rcText.right - rcText.left;
			size.cy = rcText.bottom - rcText.top;
		}

		return size;
	}

} // namespace IUI

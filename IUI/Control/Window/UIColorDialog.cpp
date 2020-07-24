#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{


	CColorDialogUI::CColorDialogUI(
		COLORREF clrInit/* = 0*/,
		DWORD dwFlags/* = 0*/,
		HWND hParent/* = NULL*/)
		: m_hParent(hParent)
	{
		memset(&m_cc, 0, sizeof(m_cc));

		m_cc.lStructSize = sizeof(m_cc);
		static COLORREF acrCustClr[16] = {0};
		m_cc.lpCustColors = (LPDWORD)acrCustClr;
		m_cc.Flags = CC_FULLOPEN | CC_RGBINIT | dwFlags;

		if ((m_cc.rgbResult = clrInit) != 0)
		{
			m_cc.Flags |= CC_RGBINIT;
		}

		m_hWndTop = NULL;
	}

	CColorDialogUI::~CColorDialogUI()
	{

	}

	INT CColorDialogUI::DoModal()
	{
		HWND hWnd = GetSafeOwner_(m_hParent, &m_hWndTop);

		if (hWnd == NULL)
		{
			hWnd = m_hParent;
		}

		m_cc.hwndOwner = hWnd;
		BOOL bResult = ::ChooseColor(&m_cc);
		return bResult ? bResult : IDCANCEL;
	}

	COLORREF CColorDialogUI::GetColor()
	{
		return m_cc.rgbResult;
	}
}    // namespace IUI

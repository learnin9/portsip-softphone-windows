#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{

	CFontDialogUI::CFontDialogUI(
		LPLOGFONT lplfInitial/* = NULL*/,
		DWORD dwFlags/* = CF_EFFECTS | CF_SCREENFONTS*/,
		HWND hParent/* = NULL*/)
		: m_hParent(hParent)
	{
		memset(&m_cf, 0, sizeof(m_cf));
		memset(&m_lf, 0, sizeof(m_lf));
		memset(&m_szStyleName, 0, sizeof(m_szStyleName));

		//         m_nIDHelp = AFX_IDD_FONT;

		m_cf.lStructSize = sizeof(m_cf);
		m_cf.lpszStyle = (LPTSTR)&m_szStyleName;
		m_cf.Flags = dwFlags/* | CF_ENABLEHOOK*/;
		//        m_cf.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

		if (lplfInitial)
		{
			m_cf.lpLogFont = lplfInitial;
			m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
			IUICopyMemory(&m_lf, sizeof(LOGFONT), m_cf.lpLogFont, sizeof(LOGFONT));
		}
		else
		{
			m_cf.lpLogFont = &m_lf;
		}

		m_hWndTop = NULL;
	}

	CFontDialogUI::~CFontDialogUI()
	{

	}

	INT CFontDialogUI::DoModal()
	{
		HWND hWnd = GetSafeOwner_(m_hParent, &m_hWndTop);

		if (hWnd == NULL)
		{
			hWnd = m_hParent;
		}

		BOOL bRet = ChooseFont(&m_cf);
		return bRet ? bRet : IDCANCEL;
	}


}    // namespace IUI

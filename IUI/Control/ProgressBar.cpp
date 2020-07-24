#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{
	ProgressBar::ProgressBar()
	{
	}

	ProgressBar::~ProgressBar()
	{
		DestroyWindow();
	}

	int ProgressBar::BindStyle(const CControlProp *pCtrlProp)
	{
		return ProgressBar_BindStyle(m_hWnd, (const CProgressBarProp *)pCtrlProp);
	}

#define AFX_WNDCOMMCTL_PROGRESS_REG     0x00200

	BOOL ProgressBar::Create(
		CWLWndHandler *pUserHandler,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_PROGRESSBAR, NULL,
				dwStyle, rect, hParent, pParent, lpszName);
	}

	BOOL ProgressBar::CreateEx(
		CWLWndHandler *pUserHandler,
		DWORD dwExStyle,
		DWORD dwStyle,
		const RECT &rect,
		HWND hParent,
		CWLWnd *pParent,
		LPCTSTR lpszName)
	{
		return CWLWnd::CreateEx(pUserHandler, dwExStyle, IUI_PROGRESSBAR, NULL,
				dwStyle, rect, hParent, pParent, lpszName);
	}

	void ProgressBar::SetRange(short nLower, short nUpper)
	{
		_ASSERT(IsWindow());
		SendMessage(PBM_SETRANGE, 0, MAKELPARAM(nLower, nUpper));
	}

	void ProgressBar::SetRange32(int nLower, int nUpper)
	{
		_ASSERT(IsWindow());
		SendMessage(PBM_SETRANGE32, (WPARAM) nLower, (LPARAM) nUpper);
	}

	void ProgressBar::GetRange(int &nLower, int &nUpper) const
	{
		_ASSERT(IsWindow());
		PBRANGE range;
		SendMessage(PBM_GETRANGE, (WPARAM) FALSE, (LPARAM) &range);
		nLower = range.iLow;
		nUpper = range.iHigh;
		return;
	}

	int ProgressBar::GetPos() const
	{
		_ASSERT(IsWindow());
		return (int) SendMessage(PBM_GETPOS, 0, 0);
	}

	int ProgressBar::SetPos(int nPos)
	{
		_ASSERT(IsWindow());
		return (int) SendMessage(PBM_SETPOS, nPos, 0L);
	}

	int ProgressBar::OffsetPos(int nPos)
	{
		_ASSERT(IsWindow());
		return (int) SendMessage(PBM_DELTAPOS, nPos, 0L);
	}

	int ProgressBar::SetStep(int nStep)
	{
		_ASSERT(IsWindow());
		return (int) SendMessage(PBM_SETSTEP, nStep, 0L);
	}

	COLORREF ProgressBar::SetBkColor(COLORREF clrNew)
	{
		_ASSERT(IsWindow());
		return (COLORREF) SendMessage(PBM_SETBKCOLOR, 0, (LPARAM) clrNew);
	}

#if (_WIN32_IE >= 0x0400)
	COLORREF ProgressBar::SetBarColor(COLORREF clrBar)
	{
		_ASSERT(IsWindow());
		return (COLORREF) SendMessage(PBM_SETBARCOLOR, 0, clrBar);
	}
#endif	// _WIN32_IE >= 0x0400

#if (_WIN32_WINNT >= 0x0501) && defined(UNICODE)
	BOOL ProgressBar::SetMarquee(BOOL fMarqueeMode, int nInterval)
	{
		_ASSERT(IsWindow());
		return (BOOL) SendMessage(PBM_SETMARQUEE, (WPARAM)fMarqueeMode, (LPARAM)nInterval);
	}
#endif	// _WIN32_WINNT >= 0x0501 && defined(UNICODE)

	int ProgressBar::StepIt()
	{
		_ASSERT(IsWindow());
		return (int) SendMessage(PBM_STEPIT, 0, 0L);
	}

	BOOL ProgressBar::SetDrawThumbColor(BOOL bDraw)
	{
		_ASSERT(IsWindow());
		return (BOOL)SendMessage(PBM_SET_DRAW_THUMB_COLOR, (WPARAM)bDraw, 0L);
	}

	BOOL ProgressBar::IsDrawThumbColor() const
	{
		_ASSERT(IsWindow());
		return (BOOL)SendMessage(PBM_GET_DRAW_THUMB_COLOR, 0, 0L);
	}

	int ProgressBar::SetThumbBkColor(COLORREF crStart, COLORREF crEnd)
	{
		_ASSERT(IsWindow());
		return (int)SendMessage(PBM_SET_THUMB_BK_COLOR, (WPARAM)crStart, (LPARAM)crEnd);
	}

	int ProgressBar::GetThumbBkColor(OUT COLORREF *pcrStart, OUT COLORREF *pcrEnd) const
	{
		_ASSERT(IsWindow());
		return (int)SendMessage(PBM_GET_THUMB_BK_COLOR, (WPARAM)pcrStart, (LPARAM)pcrEnd);
	}

	COLORREF ProgressBar::SetThumbBorderColor(COLORREF cr)
	{
		_ASSERT(IsWindow());
		return (COLORREF)SendMessage(PBM_SET_THUMB_BODER_COLOR, (WPARAM)cr, 0L);
	}

	COLORREF ProgressBar::GetThumbBorderColor() const
	{
		_ASSERT(IsWindow());
		return (COLORREF)SendMessage(PBM_GET_THUMB_BODER_COLOR, 0, 0L);
	}

	BOOL ProgressBar::CombineThumbImage(BOOL bCombine)
	{
		return 0;
	}

	BOOL ProgressBar::IsCombineThumbImage() const
	{
		return 0;
	}

	int ProgressBar::SetCombineThumbImage(LPCTSTR lpszImage)
	{
		return 0;
	}

	LPCTSTR ProgressBar::GetCombineThumbImage() const
	{
		return NULL;
	}

	int ProgressBar::SetNormalThumbImage(LPCTSTR lpszImage)
	{
		return 0;
	}

	LPCTSTR ProgressBar::GetNormalThumbImage() const
	{
		return NULL;
	}

	int ProgressBar::SetDisabledThumbImage(LPCTSTR lpszImage)
	{
		return 0;
	}

	LPCTSTR ProgressBar::GetDisabledThumbImage() const
	{
		return 0;
	}

}

/////////////////////////////////////////////////////////////////////
// ProgressBar API

int IUI::ProgressBar_BindStyle(HWLWND hWnd, const CProgressBarProp *pProp)
{
	HWLWND_BindStyle(hWnd, pProp);

	ProgressBar *pBar = (ProgressBar *)CWLWnd::FromHandle(hWnd);

	pBar->SetDrawThumbColor((bool)pProp->m_bDrawThumbColor);

	CIUIProject *pProject = GetProject(hWnd);
	{
		COLORREF cr = 0;
		CColorProp *pColorProp = pProject->GetColorProp((LPCTSTR)pProp->m_strThumbBkColorRes);
		if (pColorProp == NULL)
		{
			// Default color
			CColorProp *pResDefaultColor = pProject->GetDefaultColorProp();
			if (pResDefaultColor != NULL)
			{
				cr = pResDefaultColor->m_crColor;
			}
		}
		else
		{
			cr = pColorProp->m_crColor;
		}
		pBar->SetThumbBkColor(cr, -1);
	}
	{
		COLORREF cr = 0;
		CColorProp *pColorProp = pProject->GetColorProp((LPCTSTR)pProp->m_strThumbBorderColorRes);
		if (pColorProp == NULL)
		{
			// Default color
			CColorProp *pResDefaultColor = pProject->GetDefaultColorProp();
			if (pResDefaultColor != NULL)
			{
				cr = pResDefaultColor->m_crColor;
			}
		}
		else
		{
			cr = pColorProp->m_crColor;
		}
		pBar->SetThumbBorderColor(cr);
	}

	pBar->CombineThumbImage((bool)pProp->m_bCombineThumbImages);
	if (pBar->IsCombineThumbImage())
	{
		pBar->SetCombineThumbImage(pProp->m_strThumbCombine);
	}
	else
	{
		pBar->SetNormalThumbImage(pProp->m_strThumbN);
		pBar->SetDisabledThumbImage(pProp->m_strThumbD);
	}

	pBar = NULL;

	return 0;
}

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#define AFX_WNDCOMMCTL_BAR_REG          0x01000
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)

BOOL Slider::Create(
	__in CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_SLIDER, NULL,
			dwStyle, rect, hParent, pParent, lpszName);
}

int Slider::BindStyle(const CControlProp *pCtrlProp)
{
	return Slider_BindStyle(m_hWnd, (const CSliderProp *)pCtrlProp);
}

BOOL Slider::CreateEx(
	__in CWLWndHandler *pUserHandler,
	DWORD dwExStyle,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::CreateEx(pUserHandler, dwExStyle, IUI_SLIDER, NULL,
			dwStyle, rect, hParent, pParent, lpszName);
}

Slider::~Slider()
{
	DestroyWindow();
}

void Slider::GetRange(int &nMin, int &nMax) const
{
	_ASSERT(IsWindow());
	nMin = GetRangeMin();
	nMax = GetRangeMax();
}

void Slider::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	SetRangeMin(nMin, bRedraw); // instead of TBM_SETRANGE, use TBM_SETRANGEMIN and
	SetRangeMax(nMax, bRedraw); // TBM_SETRANGEMAX which use LONGs instead of WORDs.
}

void Slider::GetSelection(int &nMin, int &nMax) const
{
	_ASSERT(IsWindow());
	nMin = int(IUI::SendMessage(m_hWnd, TBM_GETSELSTART, 0, 0L));
	nMax = int(IUI::SendMessage(m_hWnd, TBM_GETSELEND, 0, 0L));
}

void Slider::SetSelection(int nMin, int nMax)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETSELSTART, 0, (LPARAM)nMin); // instead of TBM_SETSEL, use TBM_SETSELSTART and
	IUI::SendMessage(m_hWnd, TBM_SETSELEND, 0, (LPARAM)nMax);   // TBM_SETSELEND which use LONGs instead of WORDs.
}

Slider::Slider()
{ }

int Slider::GetLineSize() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETLINESIZE, 0, 0l);
}
int Slider::SetLineSize(int nSize)
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_SETLINESIZE, 0, nSize);
}
int Slider::GetPageSize() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETPAGESIZE, 0, 0l);
}
int Slider::SetPageSize(int nSize)
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_SETPAGESIZE, 0, nSize);
}
int Slider::GetRangeMax() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0l);
}
int Slider::GetRangeMin() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0l);
}
void Slider::SetRangeMin(int nMin, BOOL bRedraw)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETRANGEMIN, bRedraw, nMin);
}
void Slider::SetRangeMax(int nMax, BOOL bRedraw)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETRANGEMAX, bRedraw, nMax);
}
void Slider::ClearSel(BOOL bRedraw)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_CLEARSEL, bRedraw, 0l);
}
void Slider::GetChannelRect(LPRECT lprc) const
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_GETCHANNELRECT, 0, (LPARAM)lprc);
}
void Slider::GetThumbRect(LPRECT lprc) const
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)lprc);
}
int Slider::GetPos() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETPOS, 0, 0l);
}
void Slider::SetPos(int nPos)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos);
}
void Slider::ClearTics(BOOL bRedraw)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_CLEARTICS, bRedraw, 0l);
}
UINT Slider::GetNumTics() const
{
	_ASSERT(IsWindow());
	return (UINT) IUI::SendMessage(m_hWnd, TBM_GETNUMTICS, 0, 0l);
}
DWORD *Slider::GetTicArray() const
{
	_ASSERT(IsWindow());
	return (DWORD *) IUI::SendMessage(m_hWnd, TBM_GETPTICS, 0, 0l);
}
int Slider::GetTic(int nTic) const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETTIC, nTic, 0L);
}
int Slider::GetTicPos(int nTic) const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETTICPOS, nTic, 0L);
}
BOOL Slider::SetTic(int nTic)
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, TBM_SETTIC, 0, nTic);
}
void Slider::SetTicFreq(int nFreq)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETTICFREQ, nFreq, 0L);
}
HWLWND Slider::GetBuddy(BOOL fLocation) const
{
	_ASSERT(IsWindow());
	return (HWLWND) IUI::SendMessage(m_hWnd, TBM_GETBUDDY, fLocation, 0l);
}
HWLWND Slider::SetBuddy(HWLWND hWndBuddy, BOOL fLocation)
{
	_ASSERT(IsWindow());
	return (HWLWND) IUI::SendMessage(m_hWnd, TBM_SETBUDDY, fLocation, (LPARAM)hWndBuddy);
}
ToolTipCtrl *Slider::GetToolTips() const
{
	_ASSERT(IsWindow());
	return (ToolTipCtrl *)(HWND)IUI::SendMessage(m_hWnd, TBM_GETTOOLTIPS, 0, 0L);
}
void Slider::SetToolTips(ToolTipCtrl *pTip)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETTOOLTIPS, (WPARAM)pTip, 0L);
}
int Slider::SetTipSide(int nLocation)
{
	_ASSERT(IsWindow());
	return (int)IUI::SendMessage(m_hWnd, TBM_SETTIPSIDE, nLocation, 0L);
}
void Slider::SetThumbLength(int nLength)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETTHUMBLENGTH, (WPARAM)nLength, 0L);
}
int Slider::GetThumbLength() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETTHUMBLENGTH, 0, 0L);
}

void Slider::SetThumbWidth(__in int nWidth)
{
	_ASSERT(IsWindow());
	IUI::SendMessage(m_hWnd, TBM_SETTHUMBWIDTH, (WPARAM)nWidth, 0L);
}

int Slider::GetThumbWidth() const
{
	_ASSERT(IsWindow());
	return (int) IUI::SendMessage(m_hWnd, TBM_GETTHUMBWIDTH, 0, 0L);
}

int Slider::SetScrollAreaMargin(__in LPCRECT lprcMargin)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_SCROLL_AREA_MARGIN, 0, (LPARAM)lprcMargin);
}

int Slider::GetScrollAreaMargin(__out LPRECT lprcMargin) const
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_GET_SCROLL_AREA_MARGIN, 0, (LPARAM)lprcMargin);
}

int Slider::SetBkImage(
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

int Slider::GetBkImage(
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

BOOL Slider::SetDrawThumbColor(BOOL bDraw)
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, TBM_SET_DRAW_THUMB_COLOR, 0, bDraw);
}

BOOL Slider::IsDrawThumbColor() const
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, TBM_GET_DRAW_THUMB_COLOR, 0, 0);
}

COLORREF Slider::SetThumbBkColor(COLORREF cr)
{
	_ASSERT(IsWindow());
	return (COLORREF)IUI::SendMessage(m_hWnd, TBM_SET_THUMB_BK_COLOR, 0, cr);
}

COLORREF Slider::GetThumbBkColor() const
{
	_ASSERT(IsWindow());
	return (COLORREF)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_BK_COLOR, 0, 0);
}

COLORREF Slider::SetThumbBorderColor(COLORREF cr)
{
	_ASSERT(IsWindow());
	return (COLORREF)IUI::SendMessage(m_hWnd, TBM_SET_THUMB_BORDER_COLOR, 0, cr);
}

COLORREF Slider::GetThumbBorderColor() const
{
	_ASSERT(IsWindow());
	return (COLORREF)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_BORDER_COLOR, 0, 0);
}

BOOL Slider::CombineThumbImage(BOOL bCombine)
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, TBM_COMBINE_THUMB_IMAGE, 0, bCombine);
}

BOOL Slider::IsCombineThumbImage() const
{
	_ASSERT(IsWindow());
	return (BOOL)IUI::SendMessage(m_hWnd, TBM_IS_COMBINE_THUMB_IMAGE, 0, 0);
}

int Slider::SetCombineThumbImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_THUMB_IMAGE, CONTROL_STATE_COMBINE, (LPARAM)lpszImage);
}

LPCTSTR Slider::GetCombineThumbImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_IMAGE, CONTROL_STATE_COMBINE, 0);
}

int Slider::SetNormalThumbImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_THUMB_IMAGE, CONTROL_STATE_NORMAL, (LPARAM)lpszImage);
}

LPCTSTR Slider::GetNormalThumbImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_IMAGE, CONTROL_STATE_NORMAL, 0);
}

int Slider::SetHoverThumbImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_THUMB_IMAGE, CONTROL_STATE_HOVER, (LPARAM)lpszImage);
}

LPCTSTR Slider::GetHoverThumbImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_IMAGE, CONTROL_STATE_HOVER, 0);
}

int Slider::SetPressedThumbImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_THUMB_IMAGE, CONTROL_STATE_PRESSED, (LPARAM)lpszImage);
}

LPCTSTR Slider::GetPressedThumbImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_IMAGE, CONTROL_STATE_PRESSED, 0);
}

int Slider::SetDisabledThumbImage(LPCTSTR lpszImage)
{
	_ASSERT(IsWindow());
	return IUI::SendMessage(m_hWnd, TBM_SET_THUMB_IMAGE, CONTROL_STATE_DISABLED, (LPARAM)lpszImage);
}

LPCTSTR Slider::GetDisabledThumbImage() const
{
	_ASSERT(IsWindow());
	return (LPCTSTR)IUI::SendMessage(m_hWnd, TBM_GET_THUMB_IMAGE, CONTROL_STATE_DISABLED, 0);
}

////////////////////////////
// Slider API

int IUI::Slider_BindStyle(HWLWND hWnd, const CSliderProp *pCtrlProp)
{
	HWLWND_BindStyle(hWnd, pCtrlProp);

	Slider *pBar = (Slider *)CWLWnd::FromHandle(hWnd);

	// ±³¾°
	CIUIString strImageName[1 + COMBINEIMAGESIZE4];
	BOOL bCombineImage = TRUE;
	CControlProp::GetBackground4(pCtrlProp, &bCombineImage, strImageName);
	if (bCombineImage)
	{
		pBar->CombineBkImage(TRUE);
		pBar->SetCombineBkImage(strImageName[0]);
	}
	else
	{
		pBar->CombineBkImage(FALSE);
		pBar->SetBkImage(CONTROL_STATE_ALL,
			strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
	}
	SetBkImageResizeMode(hWnd, pCtrlProp->m_eBkImageResizeMode);
	SetBkImage9GridResizeParam(hWnd, &(RECT)pCtrlProp->m_rcBkImage9GridResizeArg);

	// »¬¿é
	pBar->SetDrawThumbColor((bool)pCtrlProp->m_bDrawThumbColor);

	CIUIProject *pProject = GetProject(hWnd);
	{
		COLORREF cr = 0;
		CColorProp *pColorProp = pProject->GetColorProp((LPCTSTR)pCtrlProp->m_strThumbBkColorRes);
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
		pBar->SetThumbBkColor(cr);
	}
	{
		COLORREF cr = 0;
		CColorProp *pColorProp = pProject->GetColorProp((LPCTSTR)pCtrlProp->m_strThumbBorderColorRes);
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

	pBar->SetThumbLength((LONG)pCtrlProp->m_lThumbHeight);
	pBar->SetThumbWidth((LONG)pCtrlProp->m_lThumbWidth);
	pBar->SetScrollAreaMargin(&RECT(pCtrlProp->m_rcScrollAreaMargin));

	pBar->CombineThumbImage((bool)pCtrlProp->m_bCombineThumbImages);
	if (pBar->IsCombineThumbImage())
	{
		pBar->SetCombineThumbImage(pCtrlProp->m_strThumbCombine);
	}
	else
	{
		pBar->SetNormalThumbImage(pCtrlProp->m_strThumbN);
		pBar->SetDisabledThumbImage(pCtrlProp->m_strThumbD);
	}

	pBar = NULL;

	return 0;
}

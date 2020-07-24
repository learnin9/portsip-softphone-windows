#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

SpinButton::SpinButton()
{
}
SpinButton::~SpinButton()
{
	DestroyWindow();
}
BOOL SpinButton::Create(
	__inout CWLWndHandler *pUserHandler,
	__in DWORD dwStyle,
	__in const RECT &rect,
	__in HWND hHost,
	__in CWLWnd *pParent,
	__in LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_SPINBUTTON, NULL,
			dwStyle, rect, hHost, pParent, lpszName);
}

BOOL SpinButton::CreateEx(
	CWLWndHandler *pUserHandler,
	DWORD dwExStyle,
	DWORD dwStyle,
	const RECT &rect,
	HWND hHost,
	CWLWnd *pParentWnd,
	LPCTSTR lpszName)
{
	BOOL bRet = CWLWnd::CreateEx(pUserHandler, dwExStyle, IUI_SPINBUTTON, NULL,
			dwStyle, rect, hHost, pParentWnd, lpszName);
	if (bRet && dwExStyle != 0)
	{
		bRet = SetExStyle(dwExStyle);
	}
	return bRet;
}

void SpinButton::GetRange(int &lower, int &upper) const
{
	_ASSERT(IsWindow());
	LRESULT dw = SendMessage(UDM_GETRANGE, 0, 0l);
	lower = (int)(short)HIWORD(dw);
	upper = (int)(short)LOWORD(dw);
}

UINT SpinButton::GetAccel(_In_ int nAccel, _Out_ UDACCEL *pAccel) const
{
	_ASSERT(IsWindow());
	return (UINT)LOWORD(SendMessage(UDM_GETACCEL, nAccel, (LPARAM)pAccel));
}
UINT SpinButton::GetBase() const
{
	_ASSERT(IsWindow());
	return (UINT)LOWORD(SendMessage(UDM_GETBASE, 0, 0l));
}
HWLWND SpinButton::GetBuddy() const
{
	_ASSERT(IsWindow());
	return (HWLWND) SendMessage(UDM_GETBUDDY, 0, 0l);
}
int SpinButton::GetPos() const
{
	_ASSERT(IsWindow());
	return (int)(SendMessage(UDM_GETPOS, 0, 0l));
}
DWORD SpinButton::GetRange() const
{
	_ASSERT(IsWindow());
	return (DWORD) SendMessage(UDM_GETRANGE, 0, 0l);
}
BOOL SpinButton::SetAccel(_In_ int nAccel, _In_ UDACCEL *pAccel)
{
	_ASSERT(IsWindow());
	return (BOOL)LOWORD(SendMessage(UDM_SETACCEL, nAccel, (LPARAM)pAccel));
}
int SpinButton::SetBase(_In_ int nBase)
{
	_ASSERT(IsWindow());
	return (int) SendMessage(UDM_SETBASE, nBase, 0L);
}
HWLWND SpinButton::SetBuddy(_In_ HWLWND pWndBuddy)
{
	_ASSERT(IsWindow());
	return (HWLWND) SendMessage(UDM_SETBUDDY, (WPARAM)pWndBuddy, 0L);
}
int SpinButton::SetPos(_In_ int nPos)
{
	_ASSERT(IsWindow());
	return (int)(short)LOWORD(SendMessage(UDM_SETPOS, 0, MAKELPARAM(nPos, 0)));
}
void SpinButton::SetRange(_In_ short nLower, _In_ short nUpper)
{
	_ASSERT(IsWindow());
	SendMessage(UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower));
}

int IUI::SpinButton_BindStyle(HWLWND hWnd, const CSpinButtonProp *pSpnProp)
{
	CONTROL_TYPE ect = (CONTROL_TYPE)pSpnProp->m_eControlType;
	if (ect != CT_STYLE_SPIN && ect != CT_SPIN)
	{
		return -3;
	}

	HWLWND_BindStyle(hWnd, pSpnProp);

	SpinButton *pSpinButton = (SpinButton *)CWLWnd::FromHandle(hWnd);

	// images
	CIUIString strImageName[1 + COMBINEIMAGESIZE4];
	BOOL bCombineImage = TRUE;
	CControlProp::GetBackground4(pSpnProp, &bCombineImage, strImageName);
	if (bCombineImage)
	{
		pSpinButton->CombineBkImage(TRUE);
		pSpinButton->SetCombineBkImage(strImageName[0]);
	}
	else
	{
		pSpinButton->CombineBkImage(FALSE);
		//pSpinButton->SetBkImages(CONTROL_STATE_ALL,
		//	strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
	}

	return 0;
}

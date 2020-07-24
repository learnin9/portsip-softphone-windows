#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


RepeatButton::RepeatButton()
{

}

RepeatButton::~RepeatButton()
{

}

int RepeatButton::BindStyle(const CControlProp *pCtrlProp)
{
	Button::BindStyle(pCtrlProp);

	return 0;
}

BOOL RepeatButton::Create(
	CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParent,
	LPCTSTR lpszName)
{
	return CWLWnd::Create(pUserHandler, IUI_REPEATBUTTON, NULL,
		dwStyle, rect, hParent, pParent, lpszName);
}

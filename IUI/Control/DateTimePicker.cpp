// DateTimePicker.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{
	DateTimePicker::DateTimePicker()
	{ }
	HFONT DateTimePicker::GetMonthCalFont() const
	{
		_ASSERT(IsWindow());
		return (HFONT) SendMessage(DTM_GETMCFONT, 0, 0);
	}
	HWND DateTimePicker::GetMonthCalCtrl() const
	{
		_ASSERT(IsWindow());
		return (HWND) SendMessage(DTM_GETMONTHCAL, 0, 0);
	}
	void DateTimePicker::SetMonthCalFont(_In_ HFONT hFont, _In_ BOOL bRedraw /* = TRUE */)
	{
		_ASSERT(IsWindow());
		SendMessage(DTM_SETMCFONT, (WPARAM)hFont, MAKELONG(bRedraw, 0));
	}
	COLORREF DateTimePicker::SetMonthCalColor(_In_ int iColor, _In_ COLORREF ref)
	{
		_ASSERT(IsWindow());
		return (COLORREF) SendMessage(DTM_SETMCCOLOR, (WPARAM)iColor, (LPARAM)ref);
	}
	DWORD DateTimePicker::GetTime(_Out_ LPSYSTEMTIME pTimeDest) const
	{
		_ASSERT(IsWindow());
		_ASSERT(pTimeDest != NULL);
		return (DWORD) SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM)pTimeDest);
	}
	COLORREF DateTimePicker::GetMonthCalColor(_In_ int iColor) const
	{
		_ASSERT(IsWindow());
		return (COLORREF) SendMessage(DTM_GETMCCOLOR, (WPARAM)iColor, 0);
	}
	BOOL DateTimePicker::SetFormat(_In_z_ LPCTSTR pstrFormat)
	{
		_ASSERT(IsWindow());
		return (BOOL) SendMessage(DTM_SETFORMAT, 0, (LPARAM)pstrFormat);
	}
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	DWORD DateTimePicker::SetMonthCalStyle(_In_ DWORD dwStyle)
	{
		_ASSERT(IsWindow());
		return (DWORD)DateTime_SetMonthCalStyle(m_hWnd, dwStyle);
	} // DTM_SETMCSTYLE
	DWORD DateTimePicker::GetMonthCalStyle() const
	{
		_ASSERT(IsWindow());
		return (DWORD)DateTime_GetMonthCalStyle(m_hWnd);
	} // DTM_GETMCSTYLE
	BOOL DateTimePicker::GetDateTimePickerInfo(_Out_ LPDATETIMEPICKERINFO pDateTimePickerInfo) const
	{
		_ASSERT(IsWindow());
		_ASSERT(pDateTimePickerInfo != NULL);
		if (pDateTimePickerInfo == NULL)
		{
			return FALSE;
		}
		pDateTimePickerInfo->cbSize = sizeof(DATETIMEPICKERINFO);
		return (BOOL)DateTime_GetDateTimePickerInfo(m_hWnd, pDateTimePickerInfo); // DTM_GETDATETIMEPICKERINFO
	}
	BOOL DateTimePicker::GetIdealSize(_Out_ LPSIZE pSize) const
	{
		_ASSERT(IsWindow());
		return DateTime_GetIdealSize(m_hWnd, pSize);
	} // DTM_GETIDEALSIZE
	void DateTimePicker::CloseMonthCal()
	{
		_ASSERT(IsWindow());
		DateTime_CloseMonthCal(m_hWnd);
	} // DTM_CLOSEMONTHCAL
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

	BOOL DateTimePicker::Create(
		__inout CWLWndHandler *pUserHandler,
		__in DWORD dwStyle,
		__in const RECT &rect,
		__in HWND hHost,
		__in CWLWnd *pParent,
		__in LPCTSTR lpszName)
	{
		return CWLWnd::Create(pUserHandler, IUI_DATETIMEPICKER, NULL,
				dwStyle, rect, hHost, pParent, lpszName);
	}

	DWORD DateTimePicker::GetRange(_Out_ SYSTEMTIME *pMinTime, _Out_ SYSTEMTIME *pMaxTime) const
	{
		_ASSERT(IsWindow());
		SYSTEMTIME sysTimes[2];
		memset(sysTimes, 0, sizeof(sysTimes));

		DWORD dwResult = DWORD(SendMessage(DTM_GETRANGE, 0, (LPARAM)sysTimes));

		if (pMinTime != NULL)
		{
			if (dwResult & GDTR_MIN)
			{
				*pMinTime = sysTimes[0];
			}
		}

		if (pMaxTime != NULL)
		{
			if (dwResult & GDTR_MAX)
			{
				*pMaxTime = sysTimes[1];
			}
		}

		return dwResult;
	}

	BOOL DateTimePicker::SetRange(_In_ const SYSTEMTIME *pMinTime, _In_ const SYSTEMTIME *pMaxTime)
	{
		_ASSERT(IsWindow());

		SYSTEMTIME sysTimes[2];

		WPARAM wFlags = 0;
		if (pMinTime != NULL)
		{
			sysTimes[0] = *pMinTime;
			wFlags |= GDTR_MIN;
		}

		if (pMaxTime != NULL)
		{
			sysTimes[1] = *pMaxTime;
			wFlags |= GDTR_MAX;
		}

		return (BOOL) SendMessage(DTM_SETRANGE, wFlags, (LPARAM)sysTimes);
	}

	BOOL DateTimePicker::SetTime(_In_ LPSYSTEMTIME pTimeNew /* = NULL */)
	{
		_ASSERT(IsWindow());
		WPARAM wParam = (pTimeNew == NULL) ? GDT_NONE : GDT_VALID;
		return (BOOL) SendMessage(DTM_SETSYSTEMTIME,
				wParam, (LPARAM)pTimeNew);
	}

	DateTimePicker::~DateTimePicker()
	{
		DestroyWindow();
	}

	int DateTimePicker_BindStyle(HWLWND hWnd, const CDateTimePickerProp *pDateTimeProp)
	{
		HWLWND_BindStyle(hWnd, pDateTimeProp);

		DateTimePicker *pDateTimePicker = (DateTimePicker *)CWLWnd::FromHandle(hWnd);

		// ²¼¾Ö
		SetHorzLayoutMode(hWnd, (CONTROL_LAYOUT_HOR)pDateTimeProp->m_eHorLayoutMode);
		SetVertLayoutMode(hWnd, (CONTROL_LAYOUT_VER)pDateTimeProp->m_eVerLayoutMode);
		SetDockProperty(hWnd, (DOCK)pDateTimeProp->m_eDockProperty);

		SetLayoutMargin(hWnd, &(RECT)pDateTimeProp->m_rcLayoutMargin);
		SetLayoutInset(hWnd, &(RECT)pDateTimeProp->m_rcInset);

		SetDrawBorderColor(hWnd, (bool)pDateTimeProp->m_bDrawBorderColor);

		// images
		CIUIString strImageName[1 + COMBINEIMAGESIZE4];
		BOOL bCombineImage = TRUE;
		CControlProp::GetBackground4(pDateTimeProp, &bCombineImage, strImageName);
		if (bCombineImage)
		{
			pDateTimePicker->CombineBkImage(TRUE);
			pDateTimePicker->SetCombineBkImage(strImageName[0]);
		}
		else
		{
			pDateTimePicker->CombineBkImage(FALSE);
			//pDateTimePicker->SetBkImage(CONTROL_STATE_ALL,
			//	strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
		}
		SetBkImageResizeMode(hWnd, pDateTimeProp->m_eBkImageResizeMode);
		SetBkImage9GridResizeParam(hWnd, &(RECT)pDateTimeProp->m_rcBkImage9GridResizeArg);

		//
		//pDateTimePicker->SetButtonWidth((LONG)pDateTimeProp->m_lButtonWidth);

		//// Set font
		//CIUIString strResFontID[4];
		//CControlProp::GetControlFont4(pDateTimeProp, strResFontID);
		//pDateTimePicker->SetTextFont(CONTROL_STATE_ALL,
		//	strResFontID[0], strResFontID[1], strResFontID[2], strResFontID[3]);

		////
		//POINT ptTextOffset = pDateTimeProp->m_ptTextOffset;
		//pDateTimePicker->SetTextOffset(&ptTextOffset);

		//// Set Spin button
		//if (m_ctrlSpinButton.GetSafeHwnd() != NULL)
		//{
		//	m_ctrlSpinButton.BindStyle((LPCTSTR)pDateTimeProp->m_strSpinButtonStyleID);
		//}

		return 0;
	}
}

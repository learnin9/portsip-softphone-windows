#include "stdafx.h"
#include "AddCallerCanvasHandler.h"


CAddCallerCanvasHandler::CAddCallerCanvasHandler()
	: m_pStaAddCallerTitle(NULL)
	, m_pStaAddCallerFace(NULL)
	, m_pBtnAddCallerCancel(NULL)
{
}

CAddCallerCanvasHandler::~CAddCallerCanvasHandler()
{
}

int CAddCallerCanvasHandler::SetContact(CContactDB *pContact)
{
	m_pContactInfo = pContact;
	return 0;
}

int CAddCallerCanvasHandler::InitControls()
{
	m_pStaAddCallerTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ADD_CALLER_TITLE")));
	_ASSERT(m_pStaAddCallerTitle != NULL);
	m_pStaAddCallerFace = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ADD_CALLER_FACE")));
	_ASSERT(m_pStaAddCallerFace != NULL);
	m_pBtnAddCallerCancel = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_ADD_CALLER_CANCEL")));
	_ASSERT(m_pBtnAddCallerCancel != NULL);

	return 0;
}

int CAddCallerCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	return 0;
}

LRESULT CAddCallerCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
		}
		else if (RS_DIRECT == pe->eRoutingStrategy)
		{
			if (UE_PROPERTYSHEET_SHOW == pe->RoutedEvent)
			{
				return OnPropertySheetShow(pe);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}
	
	return 0;
}

LRESULT CAddCallerCanvasHandler::OnBtnAddCallerCancel(RoutedEventArgs *pe)
{
	MessageBox(GetHostWnd(GetBindWLWnd()), _T("IDC_BTN_ADD_CALLER_CANCEL"), NULL, MB_OK);
	return 0;
}
LRESULT CAddCallerCanvasHandler::OnPropertySheetShow(RoutedEventArgs *pe)
{
	if (NULL != m_pStaAddCallerTitle && NULL != m_pContactInfo)
	{
		CIUIString strCallTitle = _T("Call ");
		CIUIString strContactName;
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pContactInfo);
		strCallTitle += strSrcUserName;
		m_pStaAddCallerTitle->SetWindowText(strCallTitle);
	}

	return 0;
}

LRESULT CAddCallerCanvasHandler::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_ADD_CALLER_FACE"))
	{
		Label *pStaFace = (Label *)CWLWnd::FromHandle(pe->hOriginalSender);
		if (NULL != m_pContactInfo)
		{
			CIUIRect rc;
			GetClientRect(pe->hOriginalSender, rc);
			DrawFace((HDC)pe->lParam, rc,PortUtility::Utf82Unicode_ND(m_pContactInfo->m_strHeadImage).c_str() );
			
			HIUIFONT hDefFont = GetProject(GetBindWLWnd())->GetFontMgr()->GetDefaultFont();
			_ASSERT(hDefFont != NULL);

			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pContactInfo);

			if (m_pContactInfo->GetSetUserFaceImage() == false)
			{
				CIUIString strLeft = strSrcUserName.Left(1);
				DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
					CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
			}
		}
	}

	return 0;
}

LRESULT CAddCallerCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnAddCallerCancel->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnAddCallerCancel(pe);
	}

	return 0;
}

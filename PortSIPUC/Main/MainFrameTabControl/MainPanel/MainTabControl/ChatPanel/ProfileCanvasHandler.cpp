#include "stdafx.h"
#include "ProfileCanvasHandler.h"


CProfileCanvasHandler::CProfileCanvasHandler()
	: m_pStaUserName(NULL)
	, m_pBtnCloseProfile(NULL)
	, m_pStaFace(NULL)
	, m_pStaAboutTitle(NULL)
	, m_pStaAbout(NULL)
	, m_pStaWorkTitle(NULL)
	, m_pStaWork(NULL)
	, m_pStaEmailTitle(NULL)
	, m_pStaEmail(NULL)
	, m_pStaSplitter1(NULL)
	, m_pStaSharedNoteTitle(NULL)
	, m_pStaSharedNote(NULL)
	, m_pStaSplitter2(NULL)
	, m_pStaEmailsTitle(NULL)
	, m_pBtnViewAllEmails(NULL)
	, m_pSta14(NULL)
	, m_pStaSpiltter3(NULL)

	, m_pContact(NULL)
{
}

CProfileCanvasHandler::~CProfileCanvasHandler()
{
}

int CProfileCanvasHandler::InitControls()
{
	m_pStaUserName = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_USER_NAME")));
	_ASSERT(m_pStaUserName != NULL);
	m_pBtnCloseProfile = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_CLOSE_PROFILE")));
	_ASSERT(m_pBtnCloseProfile != NULL);
	m_pStaFace = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_FACE")));
	_ASSERT(m_pStaFace != NULL);
	m_pStaAboutTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ABOUT_TITLE")));
	_ASSERT(m_pStaAboutTitle != NULL);
	m_pStaAbout = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ABOUT")));
	_ASSERT(m_pStaAbout != NULL);
	m_pStaWorkTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_WORK_TITLE")));
	_ASSERT(m_pStaWorkTitle != NULL);
	m_pStaWork = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_WORK")));
	_ASSERT(m_pStaWork != NULL);
	m_pStaEmailTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_EMAIL_TITLE")));
	_ASSERT(m_pStaEmailTitle != NULL);
	m_pStaEmail = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_EMAIL")));
	_ASSERT(m_pStaEmail != NULL);
	m_pStaSplitter1 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER_1")));
	_ASSERT(m_pStaSplitter1 != NULL);
	m_pStaSharedNoteTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SHARED_NOTE_TITLE")));
	_ASSERT(m_pStaSharedNoteTitle != NULL);
	m_pStaSharedNote = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SHARED_NOTE")));
	_ASSERT(m_pStaSharedNote != NULL);
	m_pStaSplitter2 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER_2")));
	_ASSERT(m_pStaSplitter2 != NULL);
	m_pStaEmailsTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_EMAILS_TITLE")));
	_ASSERT(m_pStaEmailsTitle != NULL);
	m_pBtnViewAllEmails = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_VIEW_ALL_EMAILS")));
	_ASSERT(m_pBtnViewAllEmails != NULL);
	m_pSta14 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_14")));
	_ASSERT(m_pSta14 != NULL);
	m_pStaSpiltter3 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPILTTER_3")));
	_ASSERT(m_pStaSpiltter3 != NULL);

	return 0;
}

int CProfileCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	if (NULL != m_pContact)
	{
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, m_pContact);
		m_pStaUserName->SetWindowText(strSrcUserName);
		CIUIString strEmail;
		if (m_pContact->m_strEmailAdress.empty()==false)
		{
			strEmail=PortUtility::Utf82Unicode_ND(m_pContact->m_strEmailAdress).c_str();
			m_pStaEmail->SetWindowText(strEmail);
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}

LRESULT CProfileCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
	}

	return 0;
}

LRESULT CProfileCanvasHandler::OnBtnCloseProfile(RoutedEventArgs *pe)
{
	DestroyWindow(GetBindWLWnd());
	pe->bHandled = TRUE;
	return 0;
}

LRESULT CProfileCanvasHandler::OnBtnViewAllEmails(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CProfileCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnCloseProfile->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnCloseProfile(pe);
	}
	else if (m_pBtnViewAllEmails->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnViewAllEmails(pe);
	}

	return 0;
}

int CProfileCanvasHandler::SetContact(CContactDB *pContact)
{
	m_pContact = pContact;
	return 0;
}

CContactDB *CProfileCanvasHandler::GetContact()
{
	return m_pContact;
}

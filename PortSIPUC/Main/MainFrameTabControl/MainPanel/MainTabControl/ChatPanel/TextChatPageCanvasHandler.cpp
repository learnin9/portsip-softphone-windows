#include "stdafx.h"
#include "TextChatPageCanvasHandler.h"
#include "ProfileCanvasHandler.h"


CTextChatPageCanvasHandler::CTextChatPageCanvasHandler()
	: m_pBtnFace(NULL)
	, m_pStaAccount(NULL)
	, m_pStaSign(NULL)
	, m_pBtnPhone(NULL)
	, m_pBtnVideo(NULL)
	, m_pBtnProfile(NULL)
	, m_pStaSplitter(NULL)
	, m_pCanvasChat(NULL)
	, m_hProfileWnd(NULL)
	, m_pContactInfo(NULL)
{
}

CTextChatPageCanvasHandler::~CTextChatPageCanvasHandler()
{
}

int CTextChatPageCanvasHandler::InitControls()
{
	m_pBtnFace = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_FACE")));
	_ASSERT(m_pBtnFace != NULL);
	SetProp(FindControl(GetBindWLWnd(), _T("IDC_BTN_FACE")), GETPROP_CONTACT_FACE, m_pContactInfo);
	m_pStaAccount = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_ACCOUNT")));
	_ASSERT(m_pStaAccount != NULL);
	m_pStaSign = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SIGN")));
	_ASSERT(m_pStaSign != NULL);
	m_pBtnPhone = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PHONE")));
	_ASSERT(m_pBtnPhone != NULL);
	m_pBtnVideo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_VIDEO")));
	_ASSERT(m_pBtnVideo != NULL);
	m_pBtnProfile = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PROFILE")));
	_ASSERT(m_pBtnProfile != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pCanvasChat = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_CHAT")));
	_ASSERT(m_pCanvasChat != NULL);
	EnableWindow(m_pBtnFace->GetSafeHwnd(), TRUE);
	return 0;
}

void CTextChatPageCanvasHandler::UpdateContactInfoInChat(TAG_RECENT *pContactInfo)
{
	if (pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
	{
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactInfo->m_unionContactInfo.m_pContact);

		m_pStaAccount->SetWindowText(strSrcUserName);
	}
	else
	{
		m_pStaAccount->SetWindowText(PortUtility::Utf82Unicode_ND(pContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str());
	}
	
}
int CTextChatPageCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	_ASSERT(m_pContactInfo != NULL);
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{

		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

		std::string strRemark = pContactDB->m_strShareNote;

		m_pStaAccount->SetWindowText(strSrcUserName);
		m_pStaSign->SetWindowText(strSrcSipNumber);
	}
	else
	{
		CTalkGroup *pGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
		CIUIString strSrcUserName;
		strSrcUserName = PortUtility::Utf82Unicode_ND(m_pContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str();


		m_pStaAccount->SetWindowText(strSrcUserName);
	}

	return 0;
}

LRESULT CTextChatPageCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
		else if (WM_NCDESTROY == pe->RoutedEvent)
		{
			if (m_hProfileWnd == pe->hOriginalSender)
			{
				m_hProfileWnd = NULL;
				pe->bHandled = TRUE;
			}
		}
	}

	return 0;
}



LRESULT CTextChatPageCanvasHandler::OnBtnFace(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CTextChatPageCanvasHandler::OnBtnPhone(RoutedEventArgs *pe)
{
	if (m_pContactInfo != NULL)
	{
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			pContactDB->SetCallType(ENUM_CALL_OUT);
		}
		else
		{
			CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
			pTalkGroup->SetCallType(ENUM_CALL_OUT);
		}
	}
	return 0;
}

LRESULT CTextChatPageCanvasHandler::OnBtnVideo(RoutedEventArgs *pe)
{
	if (m_pContactInfo !=NULL)
	{
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

			pContactDB->SetCallType(ENUM_CALL_OUT);
		}
		else
		{
			CTalkGroup *pTalkGroup = m_pContactInfo->m_unionContactInfo.m_pChartGroup;
			pTalkGroup->SetCallType(ENUM_CALL_OUT);
		}
	}
	return 0;
}

LRESULT CTextChatPageCanvasHandler::OnBtnProfile(RoutedEventArgs *pe)
{
	if (NULL == m_hProfileWnd)
	{
		CControlProp *pProfileWndProp = GetProject(GetBindWLWnd())->FindProjectItem(_T("IDW_PROFILE.xml"));
		if (NULL != pProfileWndProp)
		{
			_ASSERT(pProfileWndProp != NULL);
			CControlProp *pProfileProp = (CControlProp *)pProfileWndProp->GetChild(0);
			if (NULL != pProfileProp)
			{
				_ASSERT(pProfileProp != NULL);

				CProfileCanvasHandler *pProfileCanvasHandler = ALLOC_HANDLER(CProfileCanvasHandler);
				ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
				if (localRecentContactType == ENUM_SINGLE_CONTACT)
				{
					CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
					pProfileCanvasHandler->SetContact(pContactDB);
					m_hProfileWnd = CreatePreDefineChildControl(pProfileCanvasHandler,
						NULL, pProfileProp, GetBindWLWnd(),
						GetHostWnd(GetBindWLWnd()), NULL);
					ShowWindow(m_hProfileWnd, SW_HIDE);

					if (NULL != m_hProfileWnd)
					{
						SetControlMargin(m_hProfileWnd, CIUIRect(0, 0, 0, 192));
						SetLayoutMargin(m_hProfileWnd, CIUIRect(0, 0, 0, 0));
						RelayoutChild(m_hProfileWnd);
						ShowWindow(m_hProfileWnd, SW_SHOW);
					}
					else
					{
						_ASSERT(FALSE);
					}
				}
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		else
		{
			_ASSERT(FALSE);
		}
	}

	if (NULL != m_hProfileWnd)
	{
		ShowWindow(m_hProfileWnd, SW_SHOW);
	}

	return 0;
}

LRESULT CTextChatPageCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnFace->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnFace(pe);
	}
	else if (m_pBtnPhone->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnPhone(pe);
	}
	else if (m_pBtnVideo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnVideo(pe);
	}
	else if (m_pBtnProfile->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnProfile(pe);
	}

	return 0;
}

LRESULT CTextChatPageCanvasHandler::OnAfterPaint(RoutedEventArgs *pe)
{
	if (m_pBtnFace->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (m_pContactInfo==NULL)
		{
			return 0;
		}
		CIUIRect rc;
		GetClientRect(pe->hOriginalSender, rc);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
		switch (localRecentContactType)
		{
			case ENUM_SINGLE_CONTACT:
			{
				CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
				DrawFace((HDC)pe->lParam, rc, PortUtility::Utf82Unicode_ND(pContactDB->m_strHeadImage).c_str());

				HIUIFONT hDefFont = GetProject(GetBindWLWnd())->GetFontMgr()->GetDefaultFont();
				_ASSERT(hDefFont != NULL);

				CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
				FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

				if (pContactDB->GetSetUserFaceImage() == false)
				{
					CIUIString strLeft = strSrcUserName.Left(1);
					DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
						CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
				}
				
			}
			break;
		case ENUM_MULTI_CONTACT:
			{
				DrawGroupFace((HDC)pe->lParam, rc, m_pContactInfo);
			}
			break;
		default:
			break;
		}
		
	}

	return 0;
}

int CTextChatPageCanvasHandler::SetContactInfo(TAG_RECENT *pContactInfo)
{
	m_pContactInfo = pContactInfo;
	return 0;
}

TAG_RECENT *CTextChatPageCanvasHandler::GetContactInfo()
{
	return m_pContactInfo;
}

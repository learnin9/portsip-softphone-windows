#include "stdafx.h"
#include "PopupChatDlg.h"
#include "PortGo.h"
#include "System/SystemEx.h"
CPopupChatDlg::CPopupChatDlg()
	: m_pCanvasVideoChat(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnMin(NULL)
	, m_pChkMax(NULL)
	, m_pBtnClose(NULL)
	, m_pStaSplitter(NULL)
	, m_pMain(NULL)
	, m_pCanvasTextChat(NULL)
	, m_pCanvasGroupContacts(NULL)
	, m_pStaGroupTitle(NULL)
	, m_pTreeGroupContacts(NULL)
	, m_pBtnForbiddenTone(NULL)
	, m_pBtnRelieveForbiddenTone(NULL)
	, m_pBtnMore(NULL)
	, m_hTextChat(NULL)
	, m_hTextChatOldParent(NULL)
{
	m_bIsConferenceRoom = false;
	m_strTitle =  L"";
	m_pContactInfo = NULL;
}

CPopupChatDlg::~CPopupChatDlg()
{
}
void CPopupChatDlg::SetContact(TAG_RECENT *pConatctInfo)
{
	m_pContactInfo = pConatctInfo;
	m_bIsConferenceRoom = false;
}
void CPopupChatDlg::SetConferenceNumber(CONFERENCE_ROOM_INFO tagConferenceRoomInfo)
{
	m_oConferenceRoomInfo = tagConferenceRoomInfo;
	m_bIsConferenceRoom = true;
	m_strTitle = PortUtility::Utf82Unicode_ND(m_oConferenceRoomInfo.m_strConferenceTitle).c_str();
	if (!::IsWindow(this->m_hWnd))
	{
		return;
	}
	if (m_pStaTitle!=NULL)
	{
		m_pStaTitle->SetWindowText(m_strTitle);
	}
	
	
}
int CPopupChatDlg::InitControls()
{
	m_pCanvasVideoChat = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_VIDEO_CHAT")));
	_ASSERT(m_pCanvasVideoChat != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pMain = (DockPanel *)CWLWnd::FromHandle(FindControl(_T("IDC_PNL_DOCK_MAIN")));
	_ASSERT(m_pMain != NULL);
	m_pCanvasTextChat = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_TEXT_CHAT")));
	_ASSERT(m_pCanvasTextChat != NULL);
	m_pCanvasGroupContacts = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_GROUP_CONTACTS")));
	_ASSERT(m_pCanvasGroupContacts != NULL);
	m_pStaGroupTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_GROUP_TITLE")));
	_ASSERT(m_pStaGroupTitle != NULL);
	m_pTreeGroupContacts = (TreeView *)CWLWnd::FromHandle(FindControl(_T("IDC_TREECTRL_GROUP_CONTACTS")));
	_ASSERT(m_pTreeGroupContacts != NULL);
	m_pBtnForbiddenTone = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_FORBIDDEN_TONE")));
	_ASSERT(m_pBtnForbiddenTone != NULL);
	m_pBtnRelieveForbiddenTone = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_RELIEVE_FORBIDDEN_TONE")));
	_ASSERT(m_pBtnRelieveForbiddenTone != NULL);
	m_pBtnMore = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MORE")));
	_ASSERT(m_pBtnMore != NULL);
	if (m_oConferenceRoomInfo.m_vecConferenceNumber.size()<=0)
	{
		m_pCanvasGroupContacts->ShowWindow(SW_HIDE);
		m_pCanvasGroupContacts->GetParent()->RelayoutChild();
	}
	if(!m_strTitle.IsEmpty())
		m_pStaTitle->SetWindowText(m_strTitle);

	m_pTreeGroupContacts->ShowWindow(SW_SHOW);
	int nConferenceNumber = m_oConferenceRoomInfo.m_vecConferenceNumber.size();
	if (nConferenceNumber  >=0)
	{
		CIUIString strNumberList;
		CIUIString strNumberCount;
		strNumberList.Format(_T("%s"), CSystemEx::GetInstance()->GetParticipantsText());
		strNumberCount.Format(L"(%d)", (int)nConferenceNumber);
		strNumberList += strNumberCount;
		m_pStaGroupTitle->SetWindowText(strNumberList);
		RefreshMember(m_oConferenceRoomInfo);
	}

	if (m_bIsConferenceRoom==false)
	{
		if (m_pContactInfo!=NULL)
		{
			if (m_pContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
			{
				CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
				CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
				FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

				m_pStaTitle->SetWindowText(strSrcUserName);
			}
		}
	}

	m_pBtnForbiddenTone->SetWindowText(CSystemEx::GetInstance()->GetMuteMeetingText());
	m_pBtnRelieveForbiddenTone->SetWindowText(CSystemEx::GetInstance()->GetUnMuteMettingText());
	m_pBtnMore->SetWindowText(CSystemEx::GetInstance()->GetParticipantsMoreText());
	return 0;
}

LRESULT CPopupChatDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	SetParent(m_hTextChat, m_pCanvasTextChat->GetSafeHwnd());
	CanvasLayoutCtrl(m_hTextChat);
	IUI::ShowWindow(m_hTextChat, SW_SHOW);

	::SetWindowPos(m_hWnd, NULL, 0, 0, 800, 500, SWP_NOMOVE);
	CenterWindow();

	return 0;
}

LRESULT CPopupChatDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CPopupChatDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}
	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CPopupChatDlg::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CPopupChatDlg::OnChkMax(RoutedEventArgs *pe)
{
	return 0;
}
HTREEITEM CPopupChatDlg::InsertContactItem(
	CControlProp *pBindProp,
	LPCTSTR lpszIconFile,
	LPCTSTR lpszName,
	LPCTSTR lpszPhone, const CContactDB *pContact)
{
	HTREEITEM hItem = m_pTreeGroupContacts->InsertItem(_T(""));

	HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
		m_pTreeGroupContacts->GetSafeHwnd(), m_hWnd, 0);
	m_pTreeGroupContacts->SetItemBindWindowless(hItem, hBind);

	HWLWND hFace = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
	_ASSERT(hFace != NULL);

	

	USES_CONVERSION;
	Gdiplus::Image *pImage = Gdiplus::Image::FromFile(PortUtility::string2WString(pContact->m_strHeadImage).c_str());
	Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
	IUI::SetUserData(hFace, (LPARAM)ptBrush);
	IUI::SetProp(hFace, GETPROP_CONTACT, (HANDLE)pContact);

	HWLWND hStaName = IUI::FindControl(hBind, _T("IDC_STA_USER"));
	_ASSERT(hStaName != NULL);
	SetWindowText(hStaName, lpszName);

	HWLWND hBtnLeft = IUI::FindControl(hBind, _T("IDC_BTN_LEFT"));
	_ASSERT(hBtnLeft != NULL);
	IUI::ShowWindow(hBtnLeft, SW_HIDE);

	HWLWND hBtnRight = IUI::FindControl(hBind, _T("IDC_BTN_RIGHT"));
	_ASSERT(NULL != hBtnRight);
	IUI::ShowWindow(hBtnRight, SW_HIDE);
	return hItem;
}
void CPopupChatDlg::RefreshMember(CONFERENCE_ROOM_INFO tatConferenceRoomInfo)
{
	CControlProp *pItemProp = g_pProject->FindProjectItem(_T("IDW_GROUP_MEMBER_ITEM.xml"));
	_ASSERT(pItemProp != NULL);
	CControlProp *pBindItemProp = (CControlProp *)pItemProp->GetChild(0);
	_ASSERT(pBindItemProp != NULL);
	auto vecRoomMember = tatConferenceRoomInfo.m_vecConferenceNumber;
	auto iterRoomMember = vecRoomMember.begin();

	if (vecRoomMember.size()<=0)
	{
		return;		
	}
	m_pTreeGroupContacts->DeleteAllItems();
	for (iterRoomMember;iterRoomMember!=vecRoomMember.end();iterRoomMember++)
	{
		std::string strSipNumber = iterRoomMember->m_strSipURL;
		TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
		if (pContactInfo == NULL)
		{
			pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSipNumber);
		}
		CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
		CIUIString strHeadImage, strUserName, strSipNumberFormat;
		FormatItemContact(strUserName, strHeadImage, strSipNumberFormat, pContact);
		HTREEITEM hItem = InsertContactItem(pBindItemProp, strHeadImage,
			strUserName, strSipNumberFormat, pContact);
		m_pTreeGroupContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);
	}
	
	CIUIString strNumberList;
	CIUIString strNumberCount;
	strNumberList.Format(_T("%s"), CSystemEx::GetInstance()->GetParticipantsText());
	strNumberCount.Format(L"(%d)", (int)(int)vecRoomMember.size());
	strNumberList += strNumberCount;
	m_pStaGroupTitle->SetWindowText(strNumberList);
	
	//SetRedraw(m_pTreeGroupContacts->GetSafeHwnd(), TRUE);
}

LRESULT CPopupChatDlg::OnBtnClose(RoutedEventArgs *pe)
{
	SetParent(m_hTextChat, m_hTextChatOldParent);
	CanvasLayoutCtrl(m_hTextChat);
	IUI::ShowWindow(m_hTextChat, SW_HIDE);

	return 0;
}

LRESULT CPopupChatDlg::OnBtnForbiddenTone(RoutedEventArgs *pe)
{
	//m_pCanvasGroupContacts->ShowWindow(SW_HIDE);
	//m_pCanvasGroupContacts->GetParent()->RelayoutChild();
	return 0;
}

LRESULT CPopupChatDlg::OnBtnRelieveForbiddenTone(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CPopupChatDlg::OnBtnMore(RoutedEventArgs *pe)
{
	return 0;
}


LRESULT CPopupChatDlg::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnMin->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMin(pe);
	}
	else if (m_pChkMax->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkMax(pe);
	}
	else if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}
	else if (m_pBtnForbiddenTone->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnForbiddenTone(pe);
	}
	else if (m_pBtnRelieveForbiddenTone->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnRelieveForbiddenTone(pe);
	}
	else if (m_pBtnMore->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMore(pe);
	}
	return 0;
}

int CPopupChatDlg::SetTextChatControl(HWLWND hTextChat, HWLWND hOldParent)
{
	m_hTextChat = hTextChat;
	m_hTextChatOldParent = hOldParent;

	return 0;
}


LRESULT CPopupChatDlg::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
	{
		CIUIRect rc;
		IUI::GetClientRect(pe->hOriginalSender, rc);
		int nDeflate = 4;
		rc.DeflateRect(nDeflate, nDeflate, nDeflate, nDeflate);

		CContactDB *pContact = (CContactDB *)IUI::GetProp(pe->hOriginalSender, GETPROP_CONTACT);
		if (pContact == NULL)
		{
			return 0;
		}



		Gdiplus::TextureBrush *ptBrush = (Gdiplus::TextureBrush *)IUI::GetUserData(pe->hOriginalSender);
		DrawFace((HDC)pe->lParam, rc, ptBrush);

		HIUIFONT hDefFont = GetProject()->GetFontMgr()->GetDefaultFont();
		_ASSERT(hDefFont != NULL);

		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContact);

		if (pContact->GetSetUserFaceImage() == false)
		{
			CIUIString strLeft = strSrcUserName.Left(1);
			DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
				CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));

		}
		
	}

	return 0;
}

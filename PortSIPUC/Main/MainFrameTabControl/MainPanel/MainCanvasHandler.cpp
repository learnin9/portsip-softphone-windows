#include "stdafx.h"
#include "../../../PortGo.h"
#include "MainCanvasHandler.h"
#include "MainTabControl/InboxPanel/InboxCanvasHandler.h"
#include "MainTabControl/ContactsPanel/ContactsCanvasHandler.h"
#include "MainTabControl/ChatPanel/ChatTabControlHandler.h"
#include "../../../Settings/SettingsFrameDlg.h"
#include "../../SpeakerDlg.h"
#include "../../MeetingWnd.h"
#include "System/SystemEx.h"
#include "../../../AddContactWnd.h"
int CALLBACK SortByRecentTime(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CMainCanvasHandler* pRecentTree = (CMainCanvasHandler*)lParamSort;


 	TAG_RECENT *pContact1 =(TAG_RECENT*)lParam1;
	TAG_RECENT *pContact2 =(TAG_RECENT*)lParam2;

	ENUM_RECENT_CONTACT_TYPE enumContact1 = pContact1->m_enumRecentContact;
	ENUM_RECENT_CONTACT_TYPE enumContact2 = pContact2->m_enumRecentContact;
	if (pContact1!=NULL &&pContact2!=NULL)
	{
		time_t enumContactTime1, enumContactTime2;
		if (enumContact1 == ENUM_SINGLE_CONTACT)
		{
			enumContactTime1 = pContact1->m_unionContactInfo.m_pContact->m_tRecentChat;
		}
		else
		{
			enumContactTime1 = pContact1->m_unionContactInfo.m_pChartGroup->m_tRecentChat;
		}

		if (enumContact2 == ENUM_SINGLE_CONTACT)
		{
			enumContactTime2 = pContact2->m_unionContactInfo.m_pContact->m_tRecentChat;
		}
		else
		{
			enumContactTime2 = pContact2->m_unionContactInfo.m_pChartGroup->m_tRecentChat;
		}


		if (enumContactTime1 > enumContactTime2)
		{
			return -1;
		}
		else if (enumContactTime1 < enumContactTime2)
		{
			return 1;
		}
		else
		{
			return 0;
		}

	}
	return 0;
}
CMainCanvasHandler::CMainCanvasHandler()
	: m_pCanvasToolbar(NULL)
	, m_pBtnCall(NULL)
	, m_pStaToolbarSplitter(NULL)
	, m_pBtnMeetings(NULL)
	, m_pBtnSpeaker(NULL)
	, m_pBtnPrev(NULL)
	, m_pBtnNext(NULL)
	, m_pBtnRefresh(NULL)
	, m_pBtnSearch(NULL)
	, m_pStaSearchIcon(NULL)
	, m_pStaSearch(NULL)
	, m_pStaContact(NULL)
	, m_pStaContactState(NULL)
	, m_pBtnMyFace(NULL)
	, m_pBtnFaceMenu(NULL)
	, m_pTabMain(NULL)
	, m_pCanvasNavigate(NULL)
	, m_pRadInbox(NULL)
	, m_pRadContacts(NULL)
	, m_pStaSplitter(NULL)
	, m_pStaRecentContacts(NULL)
	, m_pTreeRecentContacts(NULL)
	, m_pLastContactInfo(NULL)
{
}

CMainCanvasHandler::~CMainCanvasHandler()
{
}

int CMainCanvasHandler::InitControls()
{
	m_pCanvasToolbar = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_TOOLBAR")));
	_ASSERT(m_pCanvasToolbar != NULL);
	m_pBtnCall = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_CALL")));
	_ASSERT(m_pBtnCall != NULL);
	m_pStaToolbarSplitter = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_TOOLBAR_SPLITTER")));
	_ASSERT(m_pStaToolbarSplitter != NULL);
	m_pBtnMeetings = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MEETINGS")));
	_ASSERT(m_pBtnMeetings != NULL);
	m_pBtnSpeaker = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SPEAKER")));
	_ASSERT(m_pBtnSpeaker != NULL);
	m_pBtnPrev = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PREV")));
	_ASSERT(m_pBtnPrev != NULL);
	m_pBtnNext = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NEXT")));
	_ASSERT(m_pBtnNext != NULL);
	m_pBtnRefresh = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_REFRESH")));
	_ASSERT(m_pBtnRefresh != NULL);
	m_pBtnSearch = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEARCH")));
	_ASSERT(m_pBtnSearch != NULL);
	m_pStaSearchIcon = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SEARCH_ICON")));
	_ASSERT(m_pStaSearchIcon != NULL);
	m_pStaSearch = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SEARCH")));
	_ASSERT(m_pStaSearch != NULL);
	m_pStaContact = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_CONTACT")));
	_ASSERT(m_pStaContact != NULL);
	m_pStaContactState = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_CONTACT_STATE")));
	_ASSERT(m_pStaContactState != NULL);
	m_pBtnMyFace = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MY_FACE")));
	_ASSERT(m_pBtnMyFace != NULL);
	m_pBtnFaceMenu = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_FACE_MENU")));
	_ASSERT(m_pBtnFaceMenu != NULL);
	m_pTabMain = (TabControl *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PNL_TAB_MAIN")));
	_ASSERT(m_pTabMain != NULL);
	m_pCanvasNavigate = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_NAVIGATE")));
	_ASSERT(m_pCanvasNavigate != NULL);
	m_pRadInbox = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_INBOX")));
	_ASSERT(m_pRadInbox != NULL);
	m_pRadContacts = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_CONTACTS")));
	_ASSERT(m_pRadContacts != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pStaRecentContacts = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_RECENT_CONTACTS")));
	_ASSERT(m_pStaRecentContacts != NULL);
	m_pTreeRecentContacts = (TreeView *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_TREECTRL_RECENT_CONTACTS")));
	_ASSERT(m_pTreeRecentContacts != NULL);

	CSystemEx::GetInstance()->SetMainCanvasHandler(this);
	return 0;
}
int CMainCanvasHandler::InitPersonInfo()
{
	CUser *theGlobleUser = CSystemEx::GetInstance()->GetGlobalUser();
	CIUIString strDisPlayName = PortUtility::Utf82Unicode_ND(theGlobleUser->GetPrintfDisplayName()).c_str();
	CIUIString strExtersionNumber =PortUtility::Utf82Unicode_ND(theGlobleUser->m_strSipNumber).c_str();

	m_pStaContactState->SetWindowText(strDisPlayName);
	m_pStaContact->SetWindowText(strExtersionNumber);

	return 0;
}
int CMainCanvasHandler::InitRecentContactsTree()
{
	CIUIProject *pProject = GetProject(GetBindWLWnd());
	_ASSERT(pProject != NULL);
	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_ORG_TEMPLATE.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	std::vector <string > vecRestAPISipNumber= CSystemEx::GetInstance()->GetRecentContact();
	std::vector<CChatRecord> vecUnreadChatRecord;
	if (vecRestAPISipNumber.size()>0)
	{
		auto iterSimpNumber = vecRestAPISipNumber.begin();
		
		
		for (iterSimpNumber; iterSimpNumber!=vecRestAPISipNumber.end();iterSimpNumber++)
		{
			std::string strSipNumber = *iterSimpNumber;
			TAG_RECENT* pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(strSipNumber);
			if (pContactInfo == NULL)
			{
				pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(strSipNumber);
			}
			std::string strLastUnReadMsgID = "0";
			std::string strSipURL = strSipNumber + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
			strLastUnReadMsgID = CSystemEx::GetInstance()->GetDBOprate()->GetLastUnReadMsgID(strSipURL, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
			vecUnreadChatRecord = CSystemEx::GetInstance()->GetUnReadMsgByContact(pContactInfo->m_unionContactInfo.m_pContact, strLastUnReadMsgID, SPECIFY_LIST_AFTER, OFF_LINE_MSG_COUNT);
			
			auto iterRecordOffLineMsg = vecUnreadChatRecord.begin();
			for (iterRecordOffLineMsg; iterRecordOffLineMsg < vecUnreadChatRecord.end(); iterRecordOffLineMsg++)
			{
				CChatRecord theLocalOneChatRecord = *iterRecordOffLineMsg;
				if(ENUM_MSG_SHAKE != theLocalOneChatRecord.m_nMSGType)
				{
					CSystemEx::GetInstance()->GetDBOprate()->saveOneOffLineChatRecord(theLocalOneChatRecord);
				}
				else
				{
					continue;
				}
			}
			std::vector<CChatRecord> theUnReadMsg = CSystemEx::GetInstance()->GetDBOprate()->searchUnReadRecordByContact(strSipURL, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
			CSystemEx::GetInstance()->AddUnReadMsg(theUnReadMsg);
			pContactInfo->m_unionContactInfo.m_pContact->m_nOffLineUnReadMsgNum = theUnReadMsg.size();
		}
	}

	DWORD d = GetTickCount();
	std::vector<TagRecentContactInfo> vecLocalSipInfo = CSystemEx::GetInstance()->GetDBOprate()->getRecentContactSipNumberByChatHistory(MAX_HISTORY_NUM,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	std::vector<TAG_RECENT*> vecContactDB = CSystemEx::GetInstance()->GetMainDlg()->FindMemContactByVecSipInfo(vecLocalSipInfo);
	
	int nSelectSize = MAX_HISTORY_NUM-vecContactDB.size();
	if (nSelectSize < 0)
	{
		nSelectSize = MAX_CALL_VIEW_NUM;
	}
	auto vecTalkGroup = CSystemEx::GetInstance()->GetDBOprate()->getRecentGroup(nSelectSize, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	std::vector<TAG_RECENT*> ::iterator it = vecContactDB.begin();
	for (; it != vecContactDB.end(); ++it)
	{
		TAG_RECENT *pContactInfo = *it;
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
			HTREEITEM hItem = m_pTreeRecentContacts->InsertItem(_T(""));

			m_pTreeRecentContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);

			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTreeRecentContacts->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
			m_pTreeRecentContacts->SetItemBindWindowless(hItem, hBind);
			HWLWND hFace = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
			_ASSERT(hFace != NULL);

			USES_CONVERSION;
			Gdiplus::Image *pImage = Gdiplus::Image::FromFile(PortUtility::string2WString(pContact->m_strHeadImage).c_str());
			Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
			IUI::SetUserData(hFace, (LPARAM)ptBrush);
			OutputDebugString(L"Gdiplus::TextureBrush *ptBrush New\n");
			IUI::SetProp(hFace, GETPROP_CONTACT, pContactInfo);

			HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
			_ASSERT(hName != NULL);
			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContact);

			SetWindowText(hName, strSrcUserName);

			HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));
			_ASSERT(NULL != hUnRead);

			if (pContact->m_nOffLineUnReadMsgNum != 0)
			{
				CIUIString strOffLineUnReadNum;
				strOffLineUnReadNum.Format(L"%d", pContact->m_nOffLineUnReadMsgNum);
				SetWindowText(hUnRead, strOffLineUnReadNum);
				ShowWindow(hUnRead, SW_SHOW);
			}
			else
			{
				ShowWindow(hUnRead, SW_HIDE);
			}

		}
	}


	auto iterTalkGroup = vecTalkGroup.begin();
	for (;iterTalkGroup!=vecTalkGroup.end();iterTalkGroup++)
	{
		CreateGroupContact(*iterTalkGroup);
	}
	DWORD dd = GetTickCount() - d;

	SetRedraw(m_pTreeRecentContacts->GetSafeHwnd(), TRUE);
	ReListItem();
	return 0;
}
BOOL CALLBACK fnFindContactItem(HWLWND hTree, HTREEITEM hItem, void *pCallbackParam)
{
	TreeView *pTreeView = (TreeView *)CWLWnd::FromHandle(hTree);
	SFindContactItem *pfci = (SFindContactItem *)pCallbackParam;
	TAG_RECENT *pContactInfo = (TAG_RECENT*)pTreeView->GetItemData(hItem);
	if (pContactInfo == NULL)
	{
		return FALSE;
	}
	ENUM_RECENT_CONTACT_TYPE enumType = pContactInfo->m_enumRecentContact;
	TAG_RECENT *pContactInfoSrc = (TAG_RECENT*)pfci->pCallbackParam;
	if (pfci->pCallbackParam == NULL)
	{
		return FALSE;
	}
	if ((DWORD_PTR)pContactInfo == (DWORD_PTR)pfci->pCallbackParam)
	{
		pfci->hFound = hItem;
		return TRUE;
	}
	else if (enumType == ENUM_SINGLE_CONTACT && pContactInfoSrc->m_enumRecentContact == ENUM_SINGLE_CONTACT)
	{

		ENUM_RECENT_CONTACT_TYPE enumTypeSrc = pContactInfo->m_enumRecentContact;
		if (enumTypeSrc == ENUM_SINGLE_CONTACT)
		{
			if (PortUtility::compareStringNoCase(pContactInfoSrc->m_unionContactInfo.m_pContact->m_strSIPNum, pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
			{
				if (pContactInfoSrc->m_unionContactInfo.m_pContact->GetCallType() == ENUM_CALL_OUT)
				{
					pContactInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_OUT);
				}
				pfci->hFound = hItem;
				return TRUE;
			}
		}
	}
	else if (pContactInfoSrc->m_enumRecentContact == ENUM_MULTI_CONTACT)
	{
		return FALSE;
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}
void CMainCanvasHandler::InsertRecentContact(TAG_RECENT* pContactInfo)
{
	SFindContactItem fci;
	fci.hFound = NULL;
	fci.pCallbackParam = pContactInfo;
	WalkTree(m_pTreeRecentContacts->GetSafeHwnd(), NULL, fnFindContactItem, (void *)&fci);

	if (NULL != fci.hFound)
	{
		return;
	}

	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		CIUIProject *pProject = GetProject(GetBindWLWnd());
		_ASSERT(pProject != NULL);
		CControlProp *pProp = pProject->FindProjectItem(_T("IDW_ORG_TEMPLATE.xml"));
		_ASSERT(pProp != NULL);
		CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
		_ASSERT(pBindProp != NULL);

		HTREEITEM hItem = m_pTreeRecentContacts->InsertItem(_T(""));

		m_pTreeRecentContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);

		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
			m_pTreeRecentContacts->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
		m_pTreeRecentContacts->SetItemBindWindowless(hItem, hBind);
		HWLWND hFace = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
		_ASSERT(hFace != NULL);
		IUI::SetProp(hFace, GETPROP_CONTACT, pContactInfo);

		USES_CONVERSION;
		Gdiplus::Image *pImage = Gdiplus::Image::FromFile(PortUtility::string2WString(pContactDB->m_strHeadImage).c_str());
		Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
		IUI::SetUserData(hFace, (LPARAM)ptBrush);
		OutputDebugString(L"Gdiplus::TextureBrush *ptBrush2 New\n");
		HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
		_ASSERT(hName != NULL);
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
		SetWindowText(hName, strSrcUserName);
		HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));

		_ASSERT(NULL != hUnRead);

		if (pContactDB->m_nOffLineUnReadMsgNum != 0)
		{
			CIUIString strOffLineUnReadNum;
			strOffLineUnReadNum.Format(L"%d", pContactDB->m_nOffLineUnReadMsgNum);
			SetWindowText(hUnRead, strOffLineUnReadNum);
			ShowWindow(hUnRead, SW_SHOW);
		}
		else
		{
			ShowWindow(hUnRead, SW_HIDE);
		}
		ReListItem();
	}
	
}
int CMainCanvasHandler::UnselectContactTreeItem()
{
	m_pTreeRecentContacts->SelectItem(NULL);

	return 0;
}

int CMainCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	InitRecentContactsTree();
	InitPersonInfo();
	m_pTabMain->AddPropertySheet(m_pRadInbox->GetSafeHwnd(),
		_T("IDW_INBOX_PANEL.xml"), ALLOC_HANDLER(CInboxCanvasHandler));
	m_pContactsCanvasHandler = ALLOC_HANDLER(CContactsCanvasHandler);
	m_pTabMain->AddPropertySheet(m_pRadContacts->GetSafeHwnd(),
		_T("IDW_CONTACTS_PANEL.xml"), m_pContactsCanvasHandler);

	m_pTabMain->SwitchPropertySheet(m_pRadInbox->GetSafeHwnd());
	m_pTabMain->SwitchPropertySheet(m_pRadContacts->GetSafeHwnd());
	m_pTabMain->SwitchPropertySheet(m_pRadInbox->GetSafeHwnd());
	CIUIString strImage;
	strImage.Format(_T("%s\\Skins\\PortGo\\bg_user.png"), GetRunFolder());

	CSystemEx::GetInstance()->GetGlobalUser()->SetFaceImage(PortUtility::Unicode2Utf8_ND(strImage.GetBuffer(strImage.GetLength())));

	return 0;
}

LRESULT CMainCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
			else if (WM_DESTROY == pe->RoutedEvent)
			{
				m_pTabMain->DetachPropertySheetHandler(m_pRadInbox->GetSafeHwnd());
				m_pTabMain->DetachPropertySheetHandler(m_pRadContacts->GetSafeHwnd());
			}
		}
		else if (RS_DIRECT == pe->eRoutingStrategy)
		{
			if (WM_SWITCH_TO_CONTACT == pe->RoutedEvent)
			{
				OnSwitchToContact(pe);
			}
		}
	}
	else if (m_pTreeRecentContacts->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (WM_NOTIFY == pe->RoutedEvent)
		{
			NMHDR *pnmhdr = (NMHDR *)pe->lParam;

			if (TVN_SELCHANGED == pnmhdr->code)
			{
				HTREEITEM hSelItem = m_pTreeRecentContacts->GetSelectedItem();
				OnSelectItem(hSelItem);
				UpdateMsgStatusBySipNumber(hSelItem);

			}
			else if (NM_RCLICK == pnmhdr->code)
			{
				OnRClickRecentContactsTree(pe);
			}
			else if (NM_CLICK ==pnmhdr->code)
			{
				HTREEITEM hSelItem = m_pTreeRecentContacts->GetSelectedItem();
				OnSelectItem(hSelItem);
				UpdateMsgStatusBySipNumber(hSelItem);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
		else if (WM_NCCREATE == pe->RoutedEvent)
		{
		}
		else if (UE_AFTERPAINT == pe->RoutedEvent)
		{
			return OnAfterPaint(pe);
		}
	}

	return 0;
}

LRESULT CMainCanvasHandler::OnAfterPaint(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
	{
		CIUIRect rc;
		GetClientRect(pe->hOriginalSender, rc);

		Gdiplus::TextureBrush *ptBrush = (Gdiplus::TextureBrush *)IUI::GetUserData(pe->hOriginalSender);
		BOOL bGroup = FALSE;
		if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
		{
			TAG_RECENT * pContactInfo = (TAG_RECENT *)GetProp(pe->hOriginalSender, GETPROP_CONTACT);
			if (pContactInfo == NULL)
			{
				return 0;
			}
			ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
			if (localRecentContactType == ENUM_MULTI_CONTACT)
			{
				DrawGroupFace((HDC)pe->lParam,rc,pContactInfo);
			}
			else
			{
				Gdiplus::TextureBrush *ptBrush = (Gdiplus::TextureBrush *)IUI::GetUserData(pe->hOriginalSender);
				DrawFace((HDC)pe->lParam, rc, ptBrush);

				HIUIFONT hDefFont = GetProject(GetBindWLWnd())->GetFontMgr()->GetDefaultFont();
				_ASSERT(hDefFont != NULL);

				CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
				FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactInfo->m_unionContactInfo.m_pContact);

				if (pContactInfo->m_unionContactInfo.m_pContact->GetSetUserFaceImage() == false)
				{
					CIUIString strLeft = strSrcUserName.Left(1);
					DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
						CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
				}
				
				if (GetName(pe->hOriginalSender) == _T("IDC_STA_FACE"))
				{
					CContactDB *pContact = (CContactDB *)pContactInfo->m_unionContactInfo.m_pContact;
					if (NULL != pContact)
					{
						if (PortUtility::compareStringNoCase(pContact->m_strSIPNum, "110") == 0)
						{
							printf("");
						}
					
						if (ONLINESTATE_OFFLINE == pContact->GetOnlineState())
						{
							HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, _T("GrayFaceMask.png"));
							CIUIRect rcMask;
							GetClientRect(pe->hOriginalSender, rcMask);
							IUIDrawImage((HDC)pe->lParam, rcMask, hFace, IRM_CENTER, NULL);
							ImageManager::ReleaseIUIImage(hFace);
							hFace = NULL;
						}
						if (ONLINESTATE_ONLINE==pContact->GetOnlineState())
						{

						}
					}
					else
					{
						//_ASSERT(FALSE);
					}
				}
			}
		}
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_MY_FACE"))
	{
		CIUIRect rc;
		GetClientRect(pe->hOriginalSender, rc);
		CIUIString strFaceImagePath = PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage()).c_str();
		if (!strFaceImagePath.IsEmpty())
		{
			DrawFace((HDC)pe->lParam, rc, strFaceImagePath);
			HIUIFONT hDefFont = GetProject(GetBindWLWnd())->GetFontMgr()->GetDefaultFont();
			_ASSERT(hDefFont != NULL);
			CIUIString strSipNumber = PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str();
			{
				CIUIString strLeft = strSipNumber.Left(1);
				DrawFormatText((HDC)pe->lParam, strLeft, 1, rc,
					CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));
			}
		}
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_FACE"))
	{
		return 0;
		TAG_RECENT * pContactInfo = (TAG_RECENT *)GetProp(pe->hOriginalSender, GETPROP_CONTACT_FACE);
		if (pContactInfo == NULL)
		{
			return 0;
		}
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_MULTI_CONTACT)
		{
		}
		else
		{
			CContactDB *pContact = (CContactDB *)pContactInfo->m_unionContactInfo.m_pContact;
			if (NULL != pContact)
			{
				if (ONLINESTATE_OFFLINE == pContact->GetOnlineState())
				{
					HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, _T("GrayFaceMask.png"));
					CIUIRect rcMask;
					GetClientRect(pe->hOriginalSender, rcMask);
					IUIDrawImage((HDC)pe->lParam, rcMask, hFace, IRM_CENTER, NULL);
					ImageManager::ReleaseIUIImage(hFace);
					hFace = NULL;
				}

			}
		}
	}

	return 0;
}

LRESULT CMainCanvasHandler::OnBtnCall(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnMeetings(RoutedEventArgs *pe)
{
	CIUIRect rcMeeting;
	m_pBtnMeetings->GetWindowRect(rcMeeting);

	CMeetingWnd wndMeeting;
#ifdef _DEBUG
	for (int i = 0; i < 6; ++i)
	{
		CMeeting meeting;
		meeting.m_strMeetingID.Format(_T("New Meeting %d"), i);
		meeting.m_time = 111 * (1 + i);
		meeting.m_dwDuration = 222;

		wndMeeting.InsertMeetingItem(&meeting);
	}
#endif // _DEBUG

	TCHAR szMeeting[MAX_PATH] = { 0 };
	MENUWNDRESULT eRet = wndMeeting.TrackPopupWnd(TPM_TOPALIGN, rcMeeting.left - 24,
			rcMeeting.bottom, GetHostWnd(GetBindWLWnd()), NULL, szMeeting, MAX_PATH);

	if (MENUWNDRESULT_NEW_MEETING == eRet)
	{
		
	}
	else if (MENUWNDRESULT_MEETING_LIST == eRet)
	{
		
	}
	else if (MENUWNDRESULT_MEETING_ITEM == eRet)
	{
		
	}
	else if (MENUWNDRESULT_UNVILID == eRet)
	{
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}

LRESULT CMainCanvasHandler::OnBtnSpeaker(RoutedEventArgs *pe)
{
	if (CSystemEx::GetInstance()->GetInitPortSip()==false||CSystemEx::GetInstance()->GetRegister()==false)
	{
		CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
		return 0;
	}
	
	CSpeakerDlg dlg;
	dlg.SetProject(GetProject(GetBindWLWnd()));
	dlg.SetXml(_T("IDD_SPEAKER.xml"));
	INT_PTR nRet = dlg.DoModal();
	if (IDOK == nRet)
	{
		//dlg.saveOptions();
	}

	return 0;
}

LRESULT CMainCanvasHandler::OnBtnPrev(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnNext(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnRefresh(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnSearch(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnMyFace(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMainCanvasHandler::OnBtnFaceMenu(RoutedEventArgs *pe)
{
	CIUIRect rcFace;
	IUI::GetWindowRect(pe->hOriginalSender, rcFace);

	HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_RECENT_CONTACT));
	HMENU hSubMenu = GetSubMenu(hMenu, 0);
	_ASSERT(NULL != hSubMenu);
	ModifyMenu(hSubMenu, 0, MF_BYPOSITION | MF_STRING, ID_RECENT_CONTACT_PROFILE, CSystemEx::GetInstance()->GetProfileText());
	ModifyMenu(hSubMenu, 1, MF_BYPOSITION | MF_STRING, ID_RECENT_CONTACT_APP_SETTINGS, CSystemEx::GetInstance()->GetAppSettingsText());
	ModifyMenu(hSubMenu, 2, MF_BYPOSITION | MF_STRING, ID_RECENT_CONTACT_ADD_CONTACT, CSystemEx::GetInstance()->GetAddContactText());
	ModifyMenu(hSubMenu, 4, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_ONLINE, CSystemEx::GetInstance()->GetOnLineText());
	ModifyMenu(hSubMenu, 5, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_OFFLINE, CSystemEx::GetInstance()->GetOffLineText());
	ModifyMenu(hSubMenu, 6, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_AWAY, CSystemEx::GetInstance()->GetAwayText());
	ModifyMenu(hSubMenu, 7, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_BUSY, CSystemEx::GetInstance()->GetBusyText());
	ModifyMenu(hSubMenu, 8, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_DONOTDISTURB, CSystemEx::GetInstance()->GetDoNotDisturbText());
	ModifyMenu(hSubMenu, 10, MF_BYPOSITION | MF_STRING, ID_RECENTCONTACT_SIGNOUT, CSystemEx::GetInstance()->GetSignOutText());

	UINT uRet = ::TrackPopupMenu(hSubMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
			rcFace.left, rcFace.bottom, 0, GetHostWnd(GetBindWLWnd()), rcFace);

	if (ID_RECENT_CONTACT_PROFILE == uRet)
	{
	}
	else if (ID_RECENT_CONTACT_APP_SETTINGS == uRet)
	{
		if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
		{
			CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
			return 0;
		}

		CSettingsFrameDlg dlg;
		dlg.SetProject(GetProject(GetBindWLWnd()));
		dlg.SetXml(_T("IDD_SETTINGS_FRAME.xml"));
		dlg.DoModal();
	}
	else if (ID_RECENT_CONTACT_ADD_CONTACT == uRet)
	{
		CAddContactWnd *pAddContactWnd = new CAddContactWnd(ENUM_ADD_CONTACT);
		if (NULL != pAddContactWnd)
		{
			pAddContactWnd->SetProject(g_pProject);
			pAddContactWnd->SetXml(_T("IDW_ADD_CONTACT.xml"));
			//
			pAddContactWnd->DoModal();
			//pAddContactWnd->CenterWindow();
			//pAddContactWnd->CreatePopupWindow(_T("IDW_ADD_CONTACT.xml"), GetHostWnd(GetBindWLWnd()));
			//
		}
	}
	else if (ID_RECENTCONTACT_ONLINE ==uRet)
	{
		CSystemEx::GetInstance()->SetOnline();
	}
	else if (ID_RECENTCONTACT_OFFLINE==uRet)
	{
		CSystemEx::GetInstance()->SetOffline();
	}
	else if (ID_RECENTCONTACT_AWAY==uRet)
	{
		if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
		{
			CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
			return 0;
		}
		CSystemEx::GetInstance()->SetOtherStatus(ENUM_AWAY);
	}
	else if (ID_RECENTCONTACT_BUSY==uRet)
	{
		if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
		{
			CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
			return 0;
		}
		CSystemEx::GetInstance()->SetOtherStatus(ENUM_BUSY);
	}
	else if (ID_RECENTCONTACT_DONOTDISTURB==uRet)
	{
		if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
		{
			CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
			return 0;
		}
		CSystemEx::GetInstance()->SetOtherStatus(ENUM_NODISTURB);
	}
	else if (ID_RECENTCONTACT_SIGNOUT ==uRet)
	{
		CSystemEx::GetInstance()->GetGlobalUser()->m_bAutoLogin = false;
		CSystemEx::GetInstance()->GetDBOprate()->saveActiveAccount(CSystemEx::GetInstance()->GetGlobalUser());
		std::string strFullPath = "";
		PortUtility::getCurrentlyPath(strFullPath);
		strFullPath += "\\Keeper.exe";
		ShellExecute(NULL, _T("OPEN"), PortUtility::Utf82Unicode_ND(strFullPath).c_str(), NULL, NULL, SW_NORMAL);
		//CSystemEx::GetInstance()->TerminateAllThread();
		TerminateProcess(GetCurrentProcess(), 0);
	}
	return 0;
}

LRESULT CMainCanvasHandler::OnRadInbox(RoutedEventArgs *pe)
{
	UnselectContactTreeItem();
	return 0;
}

LRESULT CMainCanvasHandler::OnRadContacts(RoutedEventArgs *pe)
{
	UnselectContactTreeItem();
	return 0;
}

LRESULT CMainCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnCall->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnCall(pe);
	}
	else if (m_pBtnMeetings->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMeetings(pe);
	}
	else if (m_pBtnSpeaker->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSpeaker(pe);
	}
	else if (m_pBtnPrev->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnPrev(pe);
	}
	else if (m_pBtnNext->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNext(pe);
	}
	else if (m_pBtnRefresh->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnRefresh(pe);
	}
	else if (m_pBtnSearch->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSearch(pe);
	}
	else if (m_pBtnMyFace->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMyFace(pe);
	}
	else if (m_pBtnFaceMenu->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnFaceMenu(pe);
	}
	else if (m_pRadInbox->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadInbox(pe);
	}
	else if (m_pRadContacts->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadContacts(pe);
	}

	return 0;
}

LRESULT CMainCanvasHandler::OnSwitchToContact(RoutedEventArgs *pe)
{
	CHAT_TAB eContactPage = (CHAT_TAB)pe->wParam;
	TAG_RECENT* pContactInfo = (TAG_RECENT*)pe->lParam;
	ProcessSwitch2Contact(pContactInfo, eContactPage, pe);
	return 0;
}
void CMainCanvasHandler::AutoCleanMsgOfTreeItem()
{
	HTREEITEM hSelItem = m_pTreeRecentContacts->GetSelectedItem();
	
	UpdateMsgStatusBySipNumber(hSelItem);
}
void CMainCanvasHandler::CreateGroupContact(TAG_RECENT* pContactInfo )
{
	DWORD d = GetTickCount();
	CIUIProject *pProject = GetProject(GetBindWLWnd());
	_ASSERT(pProject != NULL);
	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_ORG_TEMPLATE.xml"));
	_ASSERT(pProp != NULL);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	HTREEITEM hItem = m_pTreeRecentContacts->InsertItem(_T(""));

	m_pTreeRecentContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);

	HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
		m_pTreeRecentContacts->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
	m_pTreeRecentContacts->SetItemBindWindowless(hItem, hBind);
	HWLWND hFace = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
	_ASSERT(hFace != NULL);

	IUI::SetProp(hFace, GETPROP_CONTACT, pContactInfo);
	HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));
	_ASSERT(NULL != hUnRead);

	ShowWindow(hUnRead, SW_HIDE);
	
	HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
	_ASSERT(hName != NULL);

	SetWindowText(hName, PortUtility::Utf82Unicode_ND(pContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str());

	SetRedraw(m_pTreeRecentContacts->GetSafeHwnd(), TRUE);
	CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet((UINT)hItem);

	if (NULL == pSheetInfo)
	{
		CChatTabControlHandler *pHandler = ALLOC_HANDLER(CChatTabControlHandler);


		m_pTabMain->AddPropertySheet((UINT)hItem,
			_T("IDW_CHAT_PANEL.xml"), pHandler);

		pHandler->SetContactInfo(pContactInfo);

		m_vecChatTabControlHandler.push_back(pHandler);
	}
	m_pTabMain->SwitchPropertySheet((UINT)hItem);
	CreatePropertySheet(hItem);
}
bool CMainCanvasHandler::IsCalling(CChatTabControlHandler* pTabControlHandler)
{
	return pTabControlHandler->GetCalling();
}
bool CMainCanvasHandler::ChangeContact(TAG_RECENT* pContactInfo )
{
	if (pContactInfo!=NULL)
	{
		CIUIString strSipNumber =(PortUtility::Utf82Unicode_ND(pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum)).c_str();
		OutputDebugString(L"\n");
		OutputDebugString(strSipNumber);
		OutputDebugString(L"\n");
	}
	if (m_pLastContactInfo==NULL)
	{
		m_pSrcContactInfo = pContactInfo;
		m_pLastContactInfo = pContactInfo;
		
		return false;
	}
	else
	{
		if (pContactInfo==m_pLastContactInfo)
		{
			return false;
		}
		else
		{
			m_pSrcContactInfo = m_pLastContactInfo;
			m_pLastContactInfo = pContactInfo;
			
			return true;
		}
	}
}
void CMainCanvasHandler::ProcessSwitch2Contact(TAG_RECENT* pContactInfo, CHAT_TAB tabPage, RoutedEventArgs *pe)
{
	CHAT_TAB eContactPage = tabPage;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
	switch (localRecentContactType)
	{
	case ENUM_SINGLE_CONTACT:
	{
		CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
		if (NULL != pContactDB)
		{
			pContactDB->SetUserData(eContactPage);
			SFindContactItem fci;
			fci.hFound = NULL;
			fci.pCallbackParam = pContactInfo;
			WalkTree(m_pTreeRecentContacts->GetSafeHwnd(), NULL, fnFindContactItem, (void *)&fci);

			if (NULL != fci.hFound)
			{
				if (fci.hFound != m_pTreeRecentContacts->GetSelectedItem())
				{
					m_pTreeRecentContacts->SelectItem(fci.hFound);
				}

				UINT uKey = m_pTabMain->GetCurPropertySheet();
				CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet(uKey);
				if (NULL != pSheetInfo)
				{
					
					bool bChange = false, bSrcCalling = false, bCurCalling = false;
					if ((CHAT_TAB)pe->wParam==CT_VIDEO||(CHAT_TAB)pe->wParam == CT_AUDIO)
					{
						bChange = ChangeContact(pContactInfo);
						CIUIString strChange;
						strChange.Format(_T("\n15 change %d\n"), bChange);
						OutputDebugString(strChange);
					}
					
					IUI::SendMessage(pSheetInfo->m_hPropertySheet, WM_SWITCH_TO_CONTACT,
						pe->wParam, pe->lParam);

	
					if (bChange == true)
					{
						CChatTabControlHandler* pChatSrcTabControlHandler = NULL;
						if (m_pSrcContactInfo != NULL)
						{
							pChatSrcTabControlHandler = FindChatTabControlHandlerByContactDBPointer(m_pSrcContactInfo);
						}
						if (pChatSrcTabControlHandler != NULL)
						{
							bSrcCalling = pChatSrcTabControlHandler->GetCalling();
							if (bSrcCalling)
							{
								pChatSrcTabControlHandler->ChangeHold(true);
							}
						}
					}
					CChatTabControlHandler* pChatCurrentTabControlHandler = NULL;
					pChatCurrentTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
					if (pChatCurrentTabControlHandler != NULL)
					{
						bCurCalling = pChatCurrentTabControlHandler->GetCalling();
					}
					if (bCurCalling)
					{
						pChatCurrentTabControlHandler->ChangeHold(false);
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			else
			{
				DWORD d = GetTickCount();
				CIUIProject *pProject = GetProject(GetBindWLWnd());
				_ASSERT(pProject != NULL);
				CControlProp *pProp = pProject->FindProjectItem(_T("IDW_ORG_TEMPLATE.xml"));
				_ASSERT(pProp != NULL);
				CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
				_ASSERT(pBindProp != NULL);

				HTREEITEM hItem = m_pTreeRecentContacts->InsertItem(_T(""));


				m_pTreeRecentContacts->SetItemData(hItem, (DWORD_PTR)pContactInfo);

				HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
					m_pTreeRecentContacts->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);
				m_pTreeRecentContacts->SetItemBindWindowless(hItem, hBind);
				HWLWND hFace = IUI::FindControl(hBind, _T("IDC_STA_FACE"));
				_ASSERT(hFace != NULL);

				USES_CONVERSION;
				CIUIString strHeadImage, strUserName, strSipNumber;
				FormatItemContact(strUserName, strHeadImage, strSipNumber, pContactDB);

				Gdiplus::Image *pImage = NULL;
				if (!(pContactDB->m_strHeadImage).empty())
				{
					pImage = Gdiplus::Image::FromFile(strHeadImage);
				}


				if (pImage != NULL)
				{
					Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
					OutputDebugString(L"Gdiplus::TextureBrush *ptBrush3 New\n");
					IUI::SetUserData(hFace, (LPARAM)ptBrush);
				}
				else
				{
					CIUIString strImage;
					strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));

					Gdiplus::Image *pImage = Gdiplus::Image::FromFile(strImage);
					Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
					OutputDebugString(L"Gdiplus::TextureBrush *ptBrush4 New\n");
					//IUI::SetUserData(hFace, (LPARAM)ptBrush);

					//SetBkImage(hFace, FALSE, CONTROL_STATE_NORMAL, _T("bg_user.png"));
					SetWindowText(hFace, strUserName.Left(1));
				}
				IUI::SetProp(hFace, GETPROP_CONTACT, pContactInfo);
				HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));
				_ASSERT(NULL != hUnRead);

				if (pContactDB->m_nOffLineUnReadMsgNum != 0)
				{
					CIUIString strOffLineUnReadNum;
					strOffLineUnReadNum.Format(L"%d", pContactDB->m_nOffLineUnReadMsgNum);
					SetWindowText(hUnRead, strOffLineUnReadNum);
					ShowWindow(hUnRead, SW_SHOW);
				}
				else
				{
					ShowWindow(hUnRead, SW_HIDE);
				}
				HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
				_ASSERT(hName != NULL);

				SetWindowText(hName, strUserName);

				DWORD dd = GetTickCount() - d;

				std::stringstream strDebugInfo;
				strDebugInfo << "\nwaste" << dd << "time";
				OutputDebugString(PortUtility::string2WString(strDebugInfo.str()).c_str());
				SetRedraw(m_pTreeRecentContacts->GetSafeHwnd(), TRUE);
				CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet((UINT)hItem);

				if (NULL == pSheetInfo)
				{
					CChatTabControlHandler *pHandler = ALLOC_HANDLER(CChatTabControlHandler);


					m_pTabMain->AddPropertySheet((UINT)hItem,
						_T("IDW_CHAT_PANEL.xml"), pHandler);

					pHandler->SetContactInfo(pContactInfo);

					m_vecChatTabControlHandler.push_back(pHandler);
				}
				m_pTabMain->SwitchPropertySheet((UINT)hItem);
				CreatePropertySheet(hItem);

				UINT uKey = m_pTabMain->GetCurPropertySheet();
				pSheetInfo = m_pTabMain->GetPropertySheet(uKey);
				if (NULL != pSheetInfo)
				{
					CChatTabControlHandler* pChatTabControlHandler = NULL;
					bool bChange = false, bSrcCalling = false, bCurCalling = false;
					if ((CHAT_TAB)pe->wParam == CT_VIDEO || (CHAT_TAB)pe->wParam == CT_AUDIO)
					{
						pChatTabControlHandler = (CChatTabControlHandler*)pSheetInfo;
						bChange = ChangeContact(pContactInfo);
						CIUIString strChange;
						strChange.Format(_T("\n 12 change%d\n"), bChange);
						OutputDebugString(strChange);
					}

					IUI::SendMessage(pSheetInfo->m_hPropertySheet, WM_SWITCH_TO_CONTACT,
						pe->wParam, pe->lParam);

					if (bChange == true)
					{
						CChatTabControlHandler* pChatSrcTabControlHandler = NULL;
						if (m_pSrcContactInfo != NULL)
						{
							pChatSrcTabControlHandler = FindChatTabControlHandlerByContactDBPointer(m_pSrcContactInfo);
						}
						if (pChatSrcTabControlHandler != NULL)
						{
							bSrcCalling = pChatSrcTabControlHandler->GetCalling();
							if (bSrcCalling)
							{
								pChatSrcTabControlHandler->ChangeHold(true);
							}
						}
					}
					CChatTabControlHandler* pChatCurrentTabControlHandler = NULL;
					pChatCurrentTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
					if (pChatCurrentTabControlHandler != NULL)
					{
						bCurCalling = pChatCurrentTabControlHandler->GetCalling();
					}
					if (bCurCalling)
					{
						pChatCurrentTabControlHandler->ChangeHold(false);
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
		}

	}
	break;
	case ENUM_MULTI_CONTACT:
	{
		CTalkGroup *pTalkGroup = pContactInfo->m_unionContactInfo.m_pChartGroup;
		if (NULL != pTalkGroup)
		{
			pTalkGroup->SetUserData(eContactPage);
			SFindContactItem fci;
			fci.hFound = NULL;
			fci.pCallbackParam = pContactInfo;
			WalkTree(m_pTreeRecentContacts->GetSafeHwnd(), NULL, fnFindContactItem, (void *)&fci);

			if (NULL != fci.hFound)
			{
				if (fci.hFound != m_pTreeRecentContacts->GetSelectedItem())
				{
					m_pTreeRecentContacts->SelectItem(fci.hFound);
				}
			
				UINT uKey = m_pTabMain->GetCurPropertySheet();
				CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet(uKey);
				if (NULL != pSheetInfo)
				{
					CChatTabControlHandler* pChatTabControlHandler = NULL;
					bool bChange = false, bSrcCalling = false, bCurCalling = false;
					if ((CHAT_TAB)pe->wParam == CT_VIDEO || (CHAT_TAB)pe->wParam == CT_AUDIO)
					{
						pChatTabControlHandler = (CChatTabControlHandler*)pSheetInfo;
						bChange = ChangeContact(pContactInfo);
						CIUIString strChange;
						strChange.Format(_T("\n 13 change%d\n"), bChange);
						OutputDebugString(strChange);
					}

					IUI::SendMessage(pSheetInfo->m_hPropertySheet, WM_SWITCH_TO_CONTACT,
						pe->wParam, pe->lParam);

					if (bChange == true)
					{
						CChatTabControlHandler* pChatSrcTabControlHandler = NULL;
						if (m_pSrcContactInfo != NULL)
						{
							pChatSrcTabControlHandler = FindChatTabControlHandlerByContactDBPointer(m_pSrcContactInfo);
						}
						if (pChatSrcTabControlHandler != NULL)
						{
							bSrcCalling = pChatSrcTabControlHandler->GetCalling();
							if (bSrcCalling)
							{
								pChatSrcTabControlHandler->ChangeHold(true);
							}
						}
					}
					CChatTabControlHandler* pChatCurrentTabControlHandler = NULL;
					pChatCurrentTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
					if (pChatCurrentTabControlHandler != NULL)
					{
						bCurCalling = pChatCurrentTabControlHandler->GetCalling();
					}
					if (bCurCalling)
					{
						pChatCurrentTabControlHandler->ChangeHold(false);
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
		}
	}
		break;
	default:
		break;
	}
	return ;
}
void CMainCanvasHandler::ChangeContactByInside(TAG_RECENT* pContactInfo)
{
	bool bChange = false, bSrcCalling = false, bCurCalling = false;
	
	bChange = ChangeContact(pContactInfo);
	CIUIString strChange;
	strChange.Format(_T("\n16 change %d\n"), bChange);
	OutputDebugString(strChange);
	
	if (bChange == true)
	{
		CChatTabControlHandler* pChatSrcTabControlHandler = NULL;
		if (m_pSrcContactInfo != NULL)
		{
			pChatSrcTabControlHandler = FindChatTabControlHandlerByContactDBPointer(m_pSrcContactInfo);
		}
		if (pChatSrcTabControlHandler != NULL)
		{
			bSrcCalling = pChatSrcTabControlHandler->GetCalling();
			if (bSrcCalling)
			{
				pChatSrcTabControlHandler->ChangeHold(true);
			}
		}
	}
	CChatTabControlHandler* pChatCurrentTabControlHandler = NULL;
	pChatCurrentTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
	if (pChatCurrentTabControlHandler != NULL)
	{
		bCurCalling = pChatCurrentTabControlHandler->GetCalling();
	}
	if (bCurCalling)
	{
		pChatCurrentTabControlHandler->ChangeHold(false);
	}
}
CChatTabControlHandler* CMainCanvasHandler::FindChatTabControlHandlerByContactDBPointer(TAG_RECENT *pContactDB)
{
	if (pContactDB==NULL)
	{
		return NULL;
	}
	if (m_vecChatTabControlHandler.size()==0)
	{
		std::string strErrorInfo = "Didn't Have Chat Tab Data";
		LOG4_INFO(strErrorInfo.c_str());
		return NULL;
	}
	CContactDB *pLocalContactDB = NULL;
	CChatTabControlHandler *pChatTabControlHandler = NULL;
	TAG_RECENT *pContactInfo;
	bool bFind = false;
	for (int i = 0; i < m_vecChatTabControlHandler.size(); i++)
	{
		pChatTabControlHandler = m_vecChatTabControlHandler.at(i);
		if (pChatTabControlHandler!=NULL)
		{
			pContactInfo = pChatTabControlHandler->GetContactInfo();
			if (pContactInfo == pContactDB)
			{
				bFind = true;
				break;
			}
			if (pContactInfo->m_enumRecentContact !=pContactDB->m_enumRecentContact)
			{
				continue;
			}
			if (pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
			{
				continue;
			}
			if (PortUtility::compareStringNoCase(pContactDB->m_unionContactInfo.m_pContact->m_strSIPNum, pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
			{
				bFind = true;
				break;;
			}
		}
	}
	if (!bFind)
	{
		std::stringstream strStream;
		//strStream << pContactDB->m_strSIPNum << "ID:" << pContactDB->m_nContactID;
		std::string strErrorInfo = "Didn't Find Chat Tab Data By"+ strStream.str();
		LOG4_INFO(strErrorInfo.c_str());
		return NULL;
	}
	return pChatTabControlHandler;
}
void CMainCanvasHandler::CreatePropertySheet(HTREEITEM hItem)
{
	if (NULL != hItem)
	{
		CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet((UINT)hItem);
		TAG_RECENT *pRecentContactInfo = (TAG_RECENT *)m_pTreeRecentContacts->GetItemData(hItem);

		if (NULL == pSheetInfo)
		{
			CChatTabControlHandler *pHandler = ALLOC_HANDLER(CChatTabControlHandler);
			pHandler->SetContactInfo(pRecentContactInfo);
			m_vecChatTabControlHandler.push_back(pHandler);

			m_pTabMain->AddPropertySheet((UINT)hItem,
				_T("IDW_CHAT_PANEL.xml"), pHandler);
		}

		m_pTabMain->SwitchPropertySheet((UINT)hItem);

		UINT uKey = m_pTabMain->GetCurPropertySheet();
		pSheetInfo = m_pTabMain->GetPropertySheet(uKey);
		if (NULL != pSheetInfo)
		{
			CHAT_TAB eChatTab = CT_TEXT_CHAT;
			if (pRecentContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
			{
				eChatTab = (CHAT_TAB)pRecentContactInfo->m_unionContactInfo.m_pChartGroup->GetUserData();
			}
			else
			{
				eChatTab = (CHAT_TAB)pRecentContactInfo->m_unionContactInfo.m_pContact->GetUserData();
			}
			
			if (eChatTab < CT_TEXT_CHAT)
			{
				eChatTab = CT_TEXT_CHAT;
			}
			if (eChatTab > CT_VIDEO)
			{
				eChatTab = CT_VIDEO;
			}
			CChatTabControlHandler* pChatTabControlHandler = NULL;
			bool bChange = false, bSrcCalling = false,bCurCalling = false;
			if (eChatTab == CT_VIDEO || eChatTab == CT_AUDIO)
			{
				pChatTabControlHandler = (CChatTabControlHandler*)pSheetInfo;
				bChange = ChangeContact(pRecentContactInfo);
				CIUIString strChange;
				strChange.Format(_T("\n 14 change%d\n"), bChange);
				OutputDebugString(strChange);
			}

			IUI::SendMessage(pSheetInfo->m_hPropertySheet, WM_SWITCH_TO_CONTACT,
				eChatTab, (LPARAM)pRecentContactInfo);

			if (bChange == true)
			{
				CChatTabControlHandler* pChatSrcTabControlHandler = NULL;
				if (m_pSrcContactInfo != NULL)
				{
					pChatSrcTabControlHandler = FindChatTabControlHandlerByContactDBPointer(m_pSrcContactInfo);
				}
				if (pChatSrcTabControlHandler != NULL)
				{
					bSrcCalling = pChatSrcTabControlHandler->GetCalling();
					if (bSrcCalling)
					{
						pChatSrcTabControlHandler->ChangeHold(true);
					}
				}
			}
			CChatTabControlHandler* pChatCurrentTabControlHandler = NULL;
			pChatCurrentTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pRecentContactInfo);
			if (pChatCurrentTabControlHandler != NULL)
			{
				bCurCalling = pChatCurrentTabControlHandler->GetCalling();
			}
			if (bCurCalling)
			{
				pChatCurrentTabControlHandler->ChangeHold(false);
			}
		}
		else
		{
			_ASSERT(FALSE);
		}
	}
}

LRESULT CMainCanvasHandler::OnSelectItem(HTREEITEM hSelItem)
{
	CreatePropertySheet(hSelItem);
	return 0;
}

LRESULT CMainCanvasHandler::OnRClickRecentContactsTree(RoutedEventArgs *pe)
{
	CIUIPoint ptScreen;
	GetCursorPos(&ptScreen);
	CIUIPoint ptClient = ptScreen;
	m_pTreeRecentContacts->ScreenToClient(&ptClient);
	HTREEITEM hItem = m_pTreeRecentContacts->HitTest(ptClient);
	_ASSERT(NULL != hItem);
	m_pTreeRecentContacts->SelectItem(hItem);
	TAG_RECENT *pRecentContactInfo = (TAG_RECENT *)m_pTreeRecentContacts->GetItemData(hItem);
	_ASSERT(pRecentContactInfo->m_unionContactInfo.m_pContact != NULL);
	return 0;
	HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_RECENT_CONTACT));
	HMENU hRecentContactMenu = GetSubMenu(hMenu, 0);
	_ASSERT(NULL != hRecentContactMenu);
	UINT uRet = ::TrackPopupMenu(hRecentContactMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
			ptScreen.x, ptScreen.y, 0, GetHostWnd(GetBindWLWnd()), NULL);

	if (ID_RECENT_CONTACT_PROFILE == uRet)
	{
		CIUIString strInfo;
		CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pRecentContactInfo->m_unionContactInfo.m_pContact);
	}

	return 0;
}
void CMainCanvasHandler::UpdateMsgStatusBySipNumber(HTREEITEM hItem)
{
	if (NULL != hItem)
	{
		CPropertySheetInfo *pSheetInfo = m_pTabMain->GetPropertySheet((UINT)hItem);
		TAG_RECENT *pRecentContactInfo = (TAG_RECENT *)m_pTreeRecentContacts->GetItemData(hItem);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pRecentContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT)
		{
			CContactDB* pContact = pRecentContactInfo->m_unionContactInfo.m_pContact;
			if (pContact)
			{
				vector<long long > vecOffLineMsgID;
				std::string strSipURL = pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
				vecOffLineMsgID = CSystemEx::GetInstance()->GetDBOprate()->getOffLineMsgBySipNumber(strSipURL,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
				CSystemEx::GetInstance()->GetRestAPI()->UpdateOffLineMsg(vecOffLineMsgID);
				CSystemEx::GetInstance()->GetDBOprate()->updateStatusBySipNumber(ENUM_MSG_STATUS_READ, strSipURL);
				CleanUnReadNumByItem(hItem);
				CSystemEx::GetInstance()->EraseUnReadMsgByContact(pContact);
			}
		}
	}
}

void CMainCanvasHandler::RefreshRecentContantOnlineStatus(CContactDB *pContact,bool & bUpdate)
{
	if (m_pTreeRecentContacts == NULL)
		return; 
	bUpdate = false;
	HTREEITEM hVisiable =m_pTreeRecentContacts-> GetFirstVisibleItem();
	if (hVisiable)
	{
		TAG_RECENT *pRecentContactInfo = (TAG_RECENT *)m_pTreeRecentContacts->GetItemData(hVisiable);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pRecentContactInfo->m_enumRecentContact;
		if (localRecentContactType != ENUM_SINGLE_CONTACT)
		{

		}
		else
		{
			CContactDB* pTreeContact = pRecentContactInfo->m_unionContactInfo.m_pContact;
			if (pTreeContact != NULL)
			{
				if (PortUtility::compareStringNoCase(pContact->m_strSIPNum, pTreeContact->m_strSIPNum) == 0)
				{
					pTreeContact->CopyOnlineStatus(pContact);
					::Invalidate(GetBindWLWnd());
					bUpdate = true;
					return;
				}
			}
		}
		
		hVisiable = m_pTreeRecentContacts->GetNextVisibleItem(hVisiable);
		while (hVisiable !=NULL)
		{
			TAG_RECENT *pRecentContactInfo = (TAG_RECENT *)m_pTreeRecentContacts->GetItemData(hVisiable);
			ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pRecentContactInfo->m_enumRecentContact;
			if (localRecentContactType == ENUM_SINGLE_CONTACT)
			{
				CContactDB* pTreeContact = pRecentContactInfo->m_unionContactInfo.m_pContact; ;
				if (pTreeContact != NULL)
				{
					if (PortUtility::compareStringNoCase(pContact->m_strSIPNum, pTreeContact->m_strSIPNum) == 0)
					{
						pTreeContact->CopyOnlineStatus(pContact);
						::Invalidate(GetBindWLWnd());
						bUpdate = true;
						return;
					}
				}
			}
			hVisiable = m_pTreeRecentContacts->GetNextVisibleItem(hVisiable);
		}
		OutputDebugString(_T("end find\n"));
	}
	else
	{
		bUpdate = true;
		return;
	}
}

void CMainCanvasHandler::RefreshLocalContactOnlineStatus(CContactDB *pContact)
{
	if (m_pContactsCanvasHandler !=NULL)
	{
		m_pContactsCanvasHandler->RefreshLocalContactOnlineStatus(pContact);
	}
}
vector<HTREEITEM> CMainCanvasHandler::FindGroupItemByContactDB(TAG_RECENT *pContact)
{
	vector<HTREEITEM> vechItem;
	if (pContact == NULL)
	{
		return vechItem;
	}
	bool bFind = false;
	CTalkGroup  *pLocalFind = NULL;

	HTREEITEM hRoot = m_pTreeRecentContacts->GetRootItem();
	while (hRoot != NULL)
	{

		TAG_RECENT *pRecentContactInfo = (TAG_RECENT*)m_pTreeRecentContacts->GetItemData(hRoot);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pRecentContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_MULTI_CONTACT)
		{
			pLocalFind = pRecentContactInfo->m_unionContactInfo.m_pChartGroup;
			if (pLocalFind)
			{
				if (pLocalFind->ContactIsInGroup(pContact)==true)
				{
					bFind = true;
					vechItem.push_back(hRoot);
				}
			}
		}

		hRoot = m_pTreeRecentContacts->GetNextSiblingItem(hRoot);
		if (hRoot == NULL)
		{
			break;
		}
	}
	if (bFind == false)
	{
		return vechItem;
	}
	return vechItem;
}
HTREEITEM CMainCanvasHandler::FindItemByContactDB(TAG_RECENT *pContact)
{
	if (pContact == NULL)
	{
		return NULL;
	}
	bool bFind = false;
	CContactDB *pLocalFind = NULL;
	HTREEITEM hRoot = m_pTreeRecentContacts->GetRootItem();
	while (hRoot != NULL)
	{
		TAG_RECENT *pRecentContactInfo = (TAG_RECENT*)m_pTreeRecentContacts->GetItemData(hRoot);
		ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pRecentContactInfo->m_enumRecentContact;
		if (localRecentContactType == ENUM_SINGLE_CONTACT&& pContact->m_enumRecentContact==ENUM_SINGLE_CONTACT)
		{
			pLocalFind = pRecentContactInfo->m_unionContactInfo.m_pContact;
			if (pLocalFind)
			{
				if (PortUtility::compareStringNoCase(pLocalFind->m_strSIPNum, pContact->m_unionContactInfo.m_pContact->m_strSIPNum) == 0)
				{
					bFind = true;
					break;;
				}
			}
		}
		else if (localRecentContactType==ENUM_MULTI_CONTACT&&pContact->m_enumRecentContact==ENUM_MULTI_CONTACT)
		{
			CTalkGroup *pTalkGroupTree = pRecentContactInfo->m_unionContactInfo.m_pChartGroup;
			if (pTalkGroupTree->m_nChatGroupID== pContact->m_unionContactInfo.m_pChartGroup->m_nChatGroupID)
			{
				bFind = true;
				break;
			}
		}
		
		hRoot = m_pTreeRecentContacts->GetNextSiblingItem(hRoot);
		if (hRoot == NULL)
		{
			break;
		}
	}
	if (bFind==false)
	{
		LOG4_ERROR("ERROR NO CONTACT IN RECENT TREE £¬it is un normal\n");
		return NULL;
	}
	return hRoot;
}
void CMainCanvasHandler::CleanUnReadNumByItem(HTREEITEM itemContact)
{
	HWLWND hBind = m_pTreeRecentContacts->GetItemBindWindowless(itemContact);
	if (hBind == NULL)
	{
		LOG4_ERROR("ERROR NO hBind IN RECENT TREE  hFindItem£¬it is un normal");
		return;
	}
	HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));
	ShowWindow(hUnRead, SW_HIDE);
}
void CMainCanvasHandler::RefreshLocalContactInfo(TAG_RECENT *pContact)
{
	OutputDebugString(L"333RefreshLocalContactInfo\n");
	HTREEITEM hFindItem = FindItemByContactDB(pContact);
	if (hFindItem == NULL)
	{
		return;
	}
	//has been exist,need refresh data
	UpdateContactInfoInTree(pContact);
	UpdateContactInfoInChat(pContact);

	CInboxCanvasHandler *pInboxCanvasHandler = CSystemEx::GetInstance()->GetInboCanvasHandler();
	if (pInboxCanvasHandler != NULL)
	{
		pInboxCanvasHandler->RefreshCallInfo();
	}

	CContactsCanvasHandler *pContactCanvasHandler = CSystemEx::GetInstance()->GetContactCanvasHandler();
	if (pContactCanvasHandler)
	{
		pContactCanvasHandler->UpdateFavorites();
		pContactCanvasHandler->UpdateContacts();
		pContactCanvasHandler->UpdateExtersion();
		pContactCanvasHandler->UpdateRemote();
	}
	
}

void CMainCanvasHandler::UpdateContactInfoInChat(TAG_RECENT *pContactInfo)
{
	CChatTabControlHandler* pChatTabControlHandler = NULL;
	if (pContactInfo != NULL)
	{
		pChatTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
	}
	if (pChatTabControlHandler != NULL)
	{
		pChatTabControlHandler->UpdateContactInfoInChat(pContactInfo);
	}
}
void   CMainCanvasHandler::UpdateSingleContactInfo(TAG_RECENT *pContactInfo)
{
	//contact find item update contact
	HTREEITEM hFindItem = FindItemByContactDB(pContactInfo);
	if (hFindItem == NULL)
	{
		return;
	}
	HWLWND hBind = m_pTreeRecentContacts->GetItemBindWindowless(hFindItem);
	if (hBind == NULL)
	{
		LOG4_ERROR("ERROR NO hBind IN RECENT TREE  hFindItem£¬it is un normal");
		return;
	}
	TAG_RECENT *pRecentContactInfo = (TAG_RECENT*)m_pTreeRecentContacts->GetItemData(hFindItem);
	if (pRecentContactInfo->m_enumRecentContact==ENUM_SINGLE_CONTACT)
	{
		pRecentContactInfo->m_unionContactInfo.m_pContact->CopyProfileMemData(pContactInfo->m_unionContactInfo.m_pContact);
	}
	HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));

	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactInfo->m_unionContactInfo.m_pContact);

	SetWindowText(hName, strSrcUserName);

	UpdateContactInfoInChat(pContactInfo);
}

void CMainCanvasHandler::UpdateGroupInfo(TAG_RECENT *pContactInfo)
{
	HTREEITEM hFindItem = FindItemByContactDB(pContactInfo);
	if (hFindItem == NULL)
	{
		return;
	}
	HWLWND hBind = m_pTreeRecentContacts->GetItemBindWindowless(hFindItem);
	if (hBind == NULL)
	{
		LOG4_ERROR("ERROR NO hBind IN RECENT TREE  hFindItem£¬it is un normal");
		return;
	}
	TAG_RECENT *pRecentContactInfo = (TAG_RECENT*)m_pTreeRecentContacts->GetItemData(hFindItem);
	if (pRecentContactInfo->m_enumRecentContact == ENUM_SINGLE_CONTACT)
	{
		pRecentContactInfo->m_unionContactInfo.m_pContact->CopyProfileMemData(pContactInfo->m_unionContactInfo.m_pContact);
	}

	HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
	SetWindowText(hName, PortUtility::Utf82Unicode_ND(pContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str());
	::PostMessage(CSystemEx::GetInstance()->GetMainDlg()->GetHWND(), WM_CONTACT_INFO_UPDATE, (WPARAM)pRecentContactInfo, (LPARAM)this);
}
void CMainCanvasHandler::UpdateMultiContatctInfo(TAG_RECENT *pContactInfo)
{
	vector<HTREEITEM> vecGroupTreeItem = FindGroupItemByContactDB(pContactInfo);
	auto iterVecGroup = vecGroupTreeItem.begin();
	for (iterVecGroup;iterVecGroup!= vecGroupTreeItem.end();iterVecGroup++)
	{
		HTREEITEM hFindItem = *iterVecGroup;
		HWLWND hBind = m_pTreeRecentContacts->GetItemBindWindowless(hFindItem);
		if (hBind == NULL)
		{
			LOG4_ERROR("ERROR NO hBind IN RECENT TREE  hFindItem£¬it is un normal");
			return;
		}
		HWLWND hName = IUI::FindControl(hBind, _T("IDC_STA_NAME"));
		_ASSERT(hName != NULL);
		TAG_RECENT *pRecentContactInfo = (TAG_RECENT*)m_pTreeRecentContacts->GetItemData(hFindItem);
		pRecentContactInfo->m_unionContactInfo.m_pChartGroup->UpdateGroupNameByContact(pContactInfo);
		SetWindowText(hName, PortUtility::Utf82Unicode_ND(pRecentContactInfo->m_unionContactInfo.m_pChartGroup->m_strChatGroupName).c_str());
	}
}
void 	CMainCanvasHandler::UpdateContactInfoInTree(TAG_RECENT *pContactInfo)
{
	UpdateSingleContactInfo(pContactInfo);
	UpdateMultiContatctInfo(pContactInfo);
}
void CMainCanvasHandler::RefreshLocalContactUnReadNum(TAG_RECENT *pContactInfo, CChatRecord oChatRecord)
{
	HTREEITEM hFindItem = FindItemByContactDB(pContactInfo);
	if (hFindItem==NULL)
	{
		return;
	}

	HTREEITEM hSelectItem = m_pTreeRecentContacts->GetSelectedItem();
	bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
	if ((hSelectItem==hFindItem&&bMinSize==false&& bVisible==true))
	{//no min size
		CleanUnReadNumByItem(hSelectItem);
		return;
	}
	else if ((hSelectItem == hFindItem && bVisible == true&& bMinSize!=true))
	{//no close
		CleanUnReadNumByItem(hSelectItem);
		return;
	}
	else
	{
		CSystemEx::GetInstance()->AddUnReadMsg(oChatRecord);
	}
	CContactDB *pContact = pContactInfo->m_unionContactInfo.m_pContact;
	HWLWND hBind = m_pTreeRecentContacts->GetItemBindWindowless(hFindItem);
	if (hBind==NULL)
	{
		LOG4_ERROR("ERROR NO hBind IN RECENT TREE  hFindItem£¬it is un normal");
		return;
	}
	HWLWND hUnRead = IUI::FindControl(hBind, _T("IDC_STA_UNREAD"));
	std::string strSipURL = pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	std::vector<CChatRecord> theUnReadMsg = CSystemEx::GetInstance()->GetDBOprate()->searchUnReadRecordByContact(strSipURL, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
	if (theUnReadMsg.size()==0)
	{
		ShowWindow(hUnRead, SW_HIDE);
	}
	else if (theUnReadMsg.size()>=99)
	{
		CString strUnRead;
		SetWindowText(hUnRead, _T("99+"));
	}
	else
	{
		ShowWindow(hUnRead, SW_SHOW);
		CString strUnRead;
		strUnRead.Format(L"%d", theUnReadMsg.size());
		SetWindowText(hUnRead, strUnRead);
	}
		
	CChatTabControlHandler* pChatTabControlHandler = FindChatTabControlHandlerByContactDBPointer(pContactInfo);
	if (pChatTabControlHandler)
	{
		pChatTabControlHandler->ReFreshUnReadMsg(theUnReadMsg);
	}
}

void CMainCanvasHandler::ReListItem()
{
	if (m_pTreeRecentContacts==NULL)
	{
		return;
	}
	TVSORTCB tvs;
	tvs.hParent = NULL;// CTreeCtrl HTREEITEM
	tvs.lpfnCompare = SortByRecentTime;
	tvs.lParam = (LPARAM)this;
	m_pTreeRecentContacts->SortChildrenCB(&tvs);
}
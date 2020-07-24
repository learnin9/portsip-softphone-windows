#include "stdafx.h"
#include "ChatTabControlHandler.h"
#include "TextChatPageCanvasHandler.h"
#include "AudioPageCanvasHandler.h"
#include "VideoCanvasHandler.h"
#include "../../../../PopupChatDlg.h"
#include "../../../../TextChatCanvasHandler.h"
#include "System/SystemEx.h"
#include "../../MainCanvasHandler.h"

CChatTabControlHandler::CChatTabControlHandler()
	: m_pContactInfo(NULL)
	, m_hCvsTextChat(NULL)
	, m_bFirstSwitched(TRUE)
{
	m_pTextChatCanvasHandler = NULL;
	m_pVideoCanvasHandler = NULL;
	m_pTextChatPageCanvasHandler = NULL;
	m_bIsConference = false;
}

CChatTabControlHandler::~CChatTabControlHandler()
{
}

int CChatTabControlHandler::InitControls()
{
	return 0;
}
void CChatTabControlHandler::SetIsConference(bool bConference)
{
	m_bIsConference = bConference;

	if (m_pVideoCanvasHandler)
	{
		m_pVideoCanvasHandler->SetIsConference(bConference);
	}
	
}

int CChatTabControlHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	_ASSERT(m_pContactInfo != NULL);
#if _DEBUG
	TIMER_START(OnInitPanel);
#endif



	TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
	_ASSERT(pTbChat != NULL);

	CTextChatPageCanvasHandler *pChatPageCanvasHandler = ALLOC_HANDLER(CTextChatPageCanvasHandler);
	pChatPageCanvasHandler->SetContactInfo(m_pContactInfo);
	m_pTextChatPageCanvasHandler = pChatPageCanvasHandler;
	pTbChat->AddPropertySheet(CT_TEXT_CHAT, _T("IDW_TEXT_CHAT_PAGE.xml"), pChatPageCanvasHandler);

	CAudioPageCanvasHandler *pCallingPageCanvasHandler = ALLOC_HANDLER(CAudioPageCanvasHandler);
	pCallingPageCanvasHandler->SetContactInfo(m_pContactInfo);
	CSystemEx::GetInstance()->SaveAudioCavasHandler2Contact(pCallingPageCanvasHandler, m_pContactInfo);
	pTbChat->AddPropertySheet(CT_AUDIO, _T("IDW_AUDIO_PAGE.xml"), pCallingPageCanvasHandler);

	CVideoCanvasHandler *pVideoCanvasHandler = new CVideoCanvasHandler(m_pContactInfo);
	pVideoCanvasHandler->SetContactInfo(m_pContactInfo);
	m_pVideoCanvasHandler = pVideoCanvasHandler;
	CSystemEx::GetInstance()->SaveVideoCanvasHandler2Contact(pVideoCanvasHandler, m_pContactInfo);
	pTbChat->AddPropertySheet(CT_VIDEO, _T("IDW_VIDEO_PAGE.xml"), pVideoCanvasHandler);
	CreateTextChatControl();
#if _DEBUG
	TIMER_STOP(OnInitPanel);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" CChatTabControlHandler OnInitPanel Cost %d ms.\n", TIMER_MSEC(OnInitPanel));
	OutputDebugString(strFormatInfo);
#endif
	return 0;
}

int CChatTabControlHandler::CreateTextChatControl()
{
	CControlProp *pTextChatProp = GetProject(GetBindWLWnd())->FindProjectItem(_T("IDW_TEXT_CHAT.xml"));
	_ASSERT(pTextChatProp != NULL);
	CControlProp *pProp = (CControlProp *)pTextChatProp->GetChild(0);
	_ASSERT(pProp != NULL);
	m_pTextChatCanvasHandler = new CTextChatCanvasHandler();
	if (m_pTextChatCanvasHandler==NULL)
	{
		return 0;
	}
	if (m_pContactInfo != NULL)
	{
		m_pTextChatCanvasHandler->SetContactInfo(m_pContactInfo);
	}

	m_hCvsTextChat = CreatePreDefineChildControl(m_pTextChatCanvasHandler, NULL, pProp,
			GetBindWLWnd(), GetHostWnd(GetBindWLWnd()), NULL);

	
	SetControlMargin(m_hCvsTextChat, CIUIRect(0, 0, 0, 0));
	ShowWindow(m_hCvsTextChat, SW_HIDE);

	return 0;
}
bool CChatTabControlHandler::GetCalling()
{
	TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
	if (pTbChat==NULL)
	{
		return false;
	}
	CPropertySheetInfo *pVideoPage = pTbChat->GetPropertySheet(CT_VIDEO);
	bool bRet = false;
	if (NULL != pVideoPage && NULL != pVideoPage->m_pUserHandler)
	{
		CVideoCanvasHandler *pVideoHandler = (CVideoCanvasHandler *)pVideoPage->m_pUserHandler;
		bRet = pVideoHandler->GetIsConnectedNoLine();
		if (bRet)
		{
			return bRet;
		}
	}
	CPropertySheetInfo *pAudioPage = pTbChat->GetPropertySheet(CT_AUDIO);
	if (NULL != pAudioPage && NULL != pAudioPage->m_pUserHandler)
	{
		CAudioPageCanvasHandler *pAudioHandler = (CAudioPageCanvasHandler*)pAudioPage->m_pUserHandler;
		bRet = pAudioHandler->GetIsConnectedNoLine();
		if (bRet)
		{
			return bRet;
		}
	}
	return bRet;
}
void CChatTabControlHandler::ChangeHold(bool bHold)
{
	TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
	if (pTbChat==NULL)
	{
		return;
	}
	CPropertySheetInfo *pVideoPage = pTbChat->GetPropertySheet(CT_VIDEO);
	bool bRet = false;
	if (NULL != pVideoPage && NULL != pVideoPage->m_pUserHandler)
	{
		CVideoCanvasHandler *pVideoHandler = (CVideoCanvasHandler *)pVideoPage->m_pUserHandler;
		pVideoHandler->ProcessSwtichLine(bHold);
		
	}
	CPropertySheetInfo *pAudioPage = pTbChat->GetPropertySheet(CT_AUDIO);
	if (NULL != pAudioPage && NULL != pAudioPage->m_pUserHandler)
	{
		CAudioPageCanvasHandler *pAudioHandler = (CAudioPageCanvasHandler*)pAudioPage->m_pUserHandler;
		pAudioHandler->ProcessSwtichLine(bHold);
	}
}

void CChatTabControlHandler::UpdateContactInfoInChat(TAG_RECENT *pContactInfo)
{
	if (m_pTextChatPageCanvasHandler != NULL)
	{
		m_pTextChatPageCanvasHandler->UpdateContactInfoInChat(pContactInfo);
	}
}
LRESULT CChatTabControlHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
			else if (UE_TAB_SWITCHED == pe->RoutedEvent)
			{
				return OnTabSwitched(pe);
			}
		}
		else if (RS_DIRECT == pe->eRoutingStrategy)
		{
			if (WM_SWITCH_TO_CONTACT == pe->RoutedEvent)
			{
				return OnSwitchToContact(pe);
			}
			else if (UE_PROPERTYSHEET_HIDE == pe->RoutedEvent)
			{
				TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
				_ASSERT(pTbChat != NULL);
				CPropertySheetInfo *pVideoPage = pTbChat->GetPropertySheet(CT_VIDEO);
				if (NULL != pVideoPage && NULL != pVideoPage->m_pUserHandler)
				{
					CVideoCanvasHandler *pVideoHandler = (CVideoCanvasHandler *)pVideoPage->m_pUserHandler;
					pVideoHandler->ShowVideoWindow(FALSE);
				}
			}
			else if (UE_PROPERTYSHEET_SHOW == pe->RoutedEvent)
			{
				TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
				_ASSERT(pTbChat != NULL);
				if (pTbChat->GetCurPropertySheet() == CT_VIDEO)
				{
					CPropertySheetInfo *pVideoPage = pTbChat->GetPropertySheet(CT_VIDEO);
					if (NULL != pVideoPage && NULL != pVideoPage->m_pUserHandler)
					{
						CVideoCanvasHandler *pVideoHandler = (CVideoCanvasHandler *)pVideoPage->m_pUserHandler;
						pVideoHandler->ShowVideoWindow(TRUE);
					}
				}
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

LRESULT CChatTabControlHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_BTN_PHONE"))
	{
		TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
		_ASSERT(pTbChat != NULL);

		
		pTbChat->SwitchPropertySheet(CT_AUDIO);
		CSystemEx::GetInstance()->GetMainCanvasHandler()->ChangeContactByInside(m_pContactInfo);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_VIDEO"))
	{
		TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
		_ASSERT(pTbChat != NULL);
	
		pTbChat->SwitchPropertySheet(CT_VIDEO);
		CSystemEx::GetInstance()->GetMainCanvasHandler()->ChangeContactByInside(m_pContactInfo);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_MSG"))
	{
		OnBtnMsg(pe);
	}
	else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_HANGUP"))
	{
		OnBtnHangup(pe);
	}
	return 0;
}

LRESULT CChatTabControlHandler::OnBtnMsg(RoutedEventArgs *pe)
{
	if (m_dlgPopupChat.GetSafeHwnd())
	{
		::SendMessage(m_dlgPopupChat.GetSafeHwnd(), WM_CLOSE, 0, 0);
	}

	m_dlgPopupChat.SetProject(GetProject(GetBindWLWnd()));
	if (m_bIsConference)
	{
		m_dlgPopupChat.SetConferenceNumber(m_oConferenceRoomInfo);
	}
	else
	{
		m_dlgPopupChat.SetContact(m_pContactInfo);
	}
	m_dlgPopupChat.SetTextChatControl(m_hCvsTextChat, GetBindWLWnd());
	m_dlgPopupChat.CreatePopupWindow(_T("IDD_POPUP_CHAT.xml"), GetHostWnd(GetBindWLWnd()));
	
	return 0;
}
LRESULT CChatTabControlHandler::OnBtnHangup(RoutedEventArgs *pe)
{
	if (m_dlgPopupChat.GetSafeHwnd())
	{
		::SendMessage(m_dlgPopupChat.GetSafeHwnd(), WM_CLOSE, 0, 0);
	}

	return 0;
}

LRESULT CChatTabControlHandler::OnTabSwitched(RoutedEventArgs *pe)
{
	UINT uKey = pe->lParam;
	if (CT_TEXT_CHAT == uKey)
	{
		HWLWND hParent = FindControl(GetBindWLWnd(), _T("IDC_CANVAS_CHAT"));

		SetParent(m_hCvsTextChat, hParent);
		CanvasLayoutCtrl(m_hCvsTextChat);
		ShowWindow(m_hCvsTextChat, SW_SHOW);
	}
	if (CT_AUDIO==uKey)
	{
		CHAT_TAB eChatTab = (CHAT_TAB)pe->wParam;
		TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
		
		CPropertySheetInfo *pPropertySheetInfo = pTbChat->GetPropertySheet(CT_AUDIO);
		CAudioPageCanvasHandler *pCallingPageCanvasHandler =(CAudioPageCanvasHandler *) pPropertySheetInfo->m_pUserHandler;
		if (pCallingPageCanvasHandler)
		{
			ENUM_RECENT_CONTACT_TYPE localEnumContactType;
			localEnumContactType = pCallingPageCanvasHandler->GetContactInfo()->m_enumRecentContact;
			switch (localEnumContactType)
			{
			case ENUM_SINGLE_CONTACT:
			{
				if (pCallingPageCanvasHandler->GetContactInfo()->m_unionContactInfo.m_pContact->GetCallType() == ENUM_CALL_OUT)
				{
					if (pCallingPageCanvasHandler->GetCallStatus() == false)
					{
						pCallingPageCanvasHandler->CallTheContact();
					}
				}
			}
			break;
			case ENUM_MULTI_CONTACT:
				if (pCallingPageCanvasHandler->GetContactInfo()->m_unionContactInfo.m_pChartGroup->GetCallType() == ENUM_CALL_OUT)
				{
					if (pCallingPageCanvasHandler->GetCallStatus() == false)
					{
						pCallingPageCanvasHandler->CallTheContact();
					}
				}
				break;
			default:
				break;
			}
		}
	}
	else if (CT_VIDEO==uKey)
	{
		CHAT_TAB eChatTab = (CHAT_TAB)pe->wParam;
		TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());

		CPropertySheetInfo *pPropertySheetInfo = pTbChat->GetPropertySheet(CT_VIDEO);
		CVideoCanvasHandler *pCallingPageCanvasHandler = (CVideoCanvasHandler *)pPropertySheetInfo->m_pUserHandler;
		if (pCallingPageCanvasHandler)
		{
			ENUM_RECENT_CONTACT_TYPE localEnumContactType;
			localEnumContactType = pCallingPageCanvasHandler->GetContactInfo()->m_enumRecentContact;
		
			switch (localEnumContactType)
			{
			case ENUM_SINGLE_CONTACT:
			{
				CContactDB* pContact = pCallingPageCanvasHandler->GetContactInfo()->m_unionContactInfo.m_pContact;
				if (pContact->GetCallType() == ENUM_CALL_OUT)
				{
					if (pCallingPageCanvasHandler->GetCallStatus() == false)
					{
						pCallingPageCanvasHandler->CallTheContact();
					}
				}
			}
			break;
			case ENUM_MULTI_CONTACT:
				if (pCallingPageCanvasHandler->GetContactInfo()->m_unionContactInfo.m_pChartGroup->GetCallType() == ENUM_CALL_OUT)
				{

					if (pCallingPageCanvasHandler->GetCallStatus() == false)
					{
						pCallingPageCanvasHandler->CallTheContact();
					}
				}
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

LRESULT CChatTabControlHandler::OnSwitchToContact(RoutedEventArgs *pe)
{
	CHAT_TAB eChatTab = (CHAT_TAB)pe->wParam;

	TabControl *pTbChat = (TabControl *)CWLWnd::FromHandle(GetBindWLWnd());
	_ASSERT(pTbChat != NULL);
	pTbChat->SwitchPropertySheet(eChatTab);
	if (m_bFirstSwitched)
	{
		IMRichTextBox *m_pHistory = (IMRichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_IM_HISTORY")));
		_ASSERT(m_pHistory != NULL);
		m_pHistory->SendMessage(EM_SCROLL, SB_BOTTOM, 0);

		m_bFirstSwitched = FALSE;
	}
	if (eChatTab ==CT_VIDEO)
	{
		m_pVideoCanvasHandler->UpdateVideoHwndInfo();
	
	}
	
	return 0;
}

int CChatTabControlHandler::SetContactInfo(TAG_RECENT *pContactInfo)
{
	m_pContactInfo = pContactInfo;

	if (m_pTextChatCanvasHandler!=NULL)
	{
		m_pTextChatCanvasHandler->SetContactInfo(m_pContactInfo);
	}
	return 0;
}

TAG_RECENT *CChatTabControlHandler::GetContactInfo()
{
	return m_pContactInfo;
}

void CChatTabControlHandler::ReFreshUnReadMsg(vector<CChatRecord> vecUnReadMsg)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->ReFreshUnReadMsg(vecUnReadMsg);
	}
}
void CChatTabControlHandler::RefreshMsgStatus(int nMsgID,bool bSucc)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->RefreshMsgStatus(nMsgID, !bSucc);
	}
}
void CChatTabControlHandler::PaticipantsUpdate(std::string strStatusText)
{
	if (m_pTextChatCanvasHandler!=NULL)
	{
		CONFERENCE_ROOM_INFO theConferenceRoomInfo;
		bool bRet = ParseRoomInfo(strStatusText, theConferenceRoomInfo);
		if (m_dlgPopupChat.GetSafeHwnd())
		{
			//m_dlgPopupChat.
			if (bRet)
			{
				m_dlgPopupChat.RefreshMember(theConferenceRoomInfo);
			}
			else
			{
				std::string strErrorInfo = "parse room info error by status update";
				LOG4_INFO(strErrorInfo.c_str());
			}
		    
		}
	
		m_oConferenceRoomInfo = theConferenceRoomInfo;
	}
}
bool CChatTabControlHandler::ParseRoomInfo(std::string strStatusText, CONFERENCE_ROOM_INFO& tagConferenceRoomInfo)
{
	if (strStatusText.empty())
	{
		return false;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;
	if (!json_reader.parse(strStatusText, json_root))
	{
		LOG4_ERROR("json parse error \n");
		return false;
	}
	auto val = json_root[key_G_CONFERENCE_TYPE];
	if (!val.empty() && val.isString())
	{
		string strMsgType = val.asString();
		if (PortUtility::compareStringNoCase(key_G_CONFERENCE_TYPE_VALUE,strMsgType)==0)
		{
		}
		else
		{
			return false;
		}
	}
	val = json_root[key_G_CONFERENCE_ID];
	if (!val.empty() && val.isInt64())
	{
		tagConferenceRoomInfo.m_lConferenceRoomID = val.asInt64();
	}
	else if (!val.empty() && val.isString())
	{
		long long ConferenceRoomID = 0;
		PortUtility::stringToLL(val.asString(), ConferenceRoomID);
		tagConferenceRoomInfo.m_lConferenceRoomID = ConferenceRoomID;
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_NUMBER];
	if (!val.empty() && val.isString())
	{
		tagConferenceRoomInfo.m_strConferenceNumber = val.asString();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_SUBJECT];
	if (!val.empty() && val.isString())
	{
		tagConferenceRoomInfo.m_strConferenceTitle = val.asString();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_MAX_NUMBER];
	if (!val.empty() && val.isInt())
	{
		tagConferenceRoomInfo.m_nMaxNumber = val.asInt();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_MODE];
	if (!val.empty() && val.isInt())
	{
		tagConferenceRoomInfo.m_enumConferenceType = (ENUM_PLAYER_TYPE)(val.asInt()-1);
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_RECORD];
	if (!val.empty() && val.isBool())
	{
		tagConferenceRoomInfo.m_bRecord = val.asBool();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_LOCK];
	if (!val.empty() && val.isBool())
	{
		tagConferenceRoomInfo.m_bLocked = val.asBool();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_MUTED];
	if (!val.empty() && val.isBool())
	{
		tagConferenceRoomInfo.m_bLocked = val.asBool();
	}
	else
	{
		return false;
	}
	val = json_root[key_G_CONFERENCE_PARTICIPANTS];
	if (!val.empty() && val.isArray()&&!val.isNull())
	{
		int nDataSize = val.size();
		for (int i = 0; i < val.size(); i++)
		{
			CONFERENCE_NUMBER_INFO tagNumberInfo;
			tagNumberInfo.m_strSipURL = val[i][key_G_CONFERENCE_PARTICIPANT].asString();
			tagNumberInfo.m_bMuted = val[i][key_G_CONFERENCE_PARTICIPANT_MUTEED].asBool();
			tagConferenceRoomInfo.m_vecConferenceNumber.push_back(tagNumberInfo);
		}
	}
	else
	{
		return false;
	}
	
	return true;
}


void CChatTabControlHandler::InsertMsg(std::string strMsg, std::string strDateTime,int nMsgID,CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsg(strMsg,false, nMsgID, strDisplayName);
	}
}
void CChatTabControlHandler::InsertMsgAudio(std::string srFileName, std::string strDateTime,float fProgress,int lMsgID, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgAudio(srFileName, strDateTime,  fProgress, lMsgID, strDisplayName);
	}
}

void CChatTabControlHandler::InsertCallEventMsg(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertCallEventMsg(oChatRecord, enumInsertHistoryMode);
	}
}
void CChatTabControlHandler::InsertMsgVideo(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgVideo(srFileName, strDateTime, fProgress ,lMsgID, strDisplayName);
	}

}
void CChatTabControlHandler::InsertMsgPic(std::string srFileName, std::string strDateTime,  float fProgress, int lMsgID, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgPic(srFileName, strDateTime, fProgress, lMsgID, strDisplayName);
	}
}
void CChatTabControlHandler::UpdateContactInfo()
{
	if (m_pTextChatCanvasHandler)
	{
		m_pTextChatCanvasHandler->UpdateContactInfo();
	}
}

void CChatTabControlHandler:: ProcessDownLoadFileOK(long nMsgID,bool bComplete ,std::string strFileName, ENUM_MSG_TYPE  enumFileType, CIUIString strDisplayName, IUI::HWLWND hWnd)
{
	if (m_pTextChatCanvasHandler)
	{
		m_pTextChatCanvasHandler->DownLoadFileOK(nMsgID,bComplete,strFileName,enumFileType,strDisplayName, hWnd);
	}
}
void CChatTabControlHandler::ProcessDownLoadFileFailed(int nMsgID)
{
	if (m_pTextChatCanvasHandler)
	{
		m_pTextChatCanvasHandler->ProcessDownLoadFileFailed(nMsgID);
	}
}
void CChatTabControlHandler::InsertMsgFile(std::string srFileName, std::string strDateTime,  float fProgress, int lMsgID, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgFile(srFileName,  fProgress, lMsgID, strDisplayName);
	}
}
void CChatTabControlHandler::InsertMsgEmoji(std::string srFileName, std::string strDateTime, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgEmoji(srFileName, strDateTime, strDisplayName);
	}
}
void CChatTabControlHandler::InsertMsgOther(std::string srFileName, std::string strDateTime, CIUIString strDisplayName)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->InsertMsgOther(srFileName, strDateTime, strDisplayName);
	}
}
void CChatTabControlHandler::UpdateDownLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateDownLoadAudioFileProgress(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateDownLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateDownLoadVideoFileProgress(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateDownLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateDownLoadPicFileProgress(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateDownLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateDownLoadFileProgress(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateUpLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateUpLoadAudioFileProgress(pDownLoadParam);
	}
}

void  CChatTabControlHandler::UpdateUpLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateUpLoadVideoFileProgress(pDownLoadParam);
	}
}
void CChatTabControlHandler::OnUpdateFileOK(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->OnUpdateFileOK(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateUpLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateUpLoadPicFileProgress(pDownLoadParam);
	}
}
void  CChatTabControlHandler::UpdateUpLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	if (m_pTextChatCanvasHandler != NULL)
	{
		m_pTextChatCanvasHandler->UpdateUpLoadFileProgress(pDownLoadParam);
	}
}
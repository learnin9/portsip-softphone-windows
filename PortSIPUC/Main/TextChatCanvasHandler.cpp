#include "stdafx.h"
#include "TextChatCanvasHandler.h"
#include "EmoticonWnd.h"
#include <ShellAPI.h>

#include "MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/ScreenPrint.h"
#include "MainFrameTabControl/MainPanel/MainCanvasHandler.h"
#include "OleVideoCanvasHandler.h"
#include "System/SystemEx.h"
#define   MORE_MSG_ID           0
#if (_MSC_VER <= 1200)	// VC6.0
#include <atlbase.h>
#endif


CTextChatCanvasHandler::CTextChatCanvasHandler()
	: m_pHistory(NULL)
	, m_pCanvasInput(NULL)
	, m_pStaSplitterBar(NULL)
	, m_pChkEmoticon(NULL)
	, m_pBtnVibrate(NULL)
	, m_pChkScreenshot(NULL)
	, m_pBtnSendImage(NULL)
	, m_pBtnSendFile(NULL)
	, m_pBtnSendVideo(NULL)
	, m_pBtnSendAudio(NULL)
	, m_pReInput(NULL)
	, m_pBtnSendMsg(NULL)
	, m_pBtnMsgHistory(NULL)
	, m_nHistoryInputYSpace(0)
{
	m_pContactInfo = NULL;
	m_nCurrentMinHistoryMsgID = -1;
}

CTextChatCanvasHandler::~CTextChatCanvasHandler()
{
}

int CTextChatCanvasHandler::InitControls()
{
	m_pHistory = (IMRichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_IM_HISTORY")));
	_ASSERT(m_pHistory != NULL);
	m_pCanvasInput = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_INPUT")));
	_ASSERT(m_pCanvasInput != NULL);
	m_pStaSplitterBar = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER_BAR")));
	_ASSERT(m_pStaSplitterBar != NULL);
	m_pChkEmoticon = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_EMOTICON")));
	_ASSERT(m_pChkEmoticon != NULL);
	m_pBtnVibrate = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_VIBRATE")));
	_ASSERT(m_pBtnVibrate != NULL);
	m_pChkScreenshot = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_SCREENSHOT")));
	_ASSERT(m_pChkScreenshot != NULL);
	m_pBtnSendImage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEND_IMAGE")));
	_ASSERT(m_pBtnSendImage != NULL);
	m_pBtnSendFile = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEND_FILE")));
	_ASSERT(m_pBtnSendFile != NULL);
	m_pBtnSendVideo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEND_VIDEO")));
	_ASSERT(m_pBtnSendVideo != NULL);
	m_pBtnSendAudio = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEND_AUDIO")));
	_ASSERT(m_pBtnSendAudio != NULL);
	m_pReInput = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_INPUT")));
	_ASSERT(m_pReInput != NULL);
	m_pBtnSendMsg = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_SEND_MSG")));
	_ASSERT(m_pBtnSendMsg != NULL);
	m_pBtnMsgHistory = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_MSG_HISTORY")));
	_ASSERT(m_pBtnMsgHistory != NULL);
	m_pBtnMsgHistory->ShowWindow(SW_HIDE);
	return 0;
}

int CTextChatCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
#if _DEBUG
	TIMER_START(x);
#endif
	RevokeDragDrop(m_pReInput->GetSafeHwnd());
	InitChatHistory();
#if _DEBUG
	TIMER_STOP(x);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" OnInitPanel 1 Cost %d ms.\n", TIMER_MSEC(x));
	OutputDebugString(strFormatInfo);
#endif
	DWORD dwStyle = m_pCanvasInput->GetStyle();
	m_pCanvasInput->SetStyle(dwStyle | WS_THICKFRAME);
	m_pCanvasInput->SetBorderThickness(CIUIRect(0, 10, 0, 0));
	m_pCanvasInput->SetResizeFlag(GRF_TOP);

	CIUIRect rcHistroy;
	m_pHistory->GetWindowRectToParent(rcHistroy);
	CIUIRect rcInput;
	m_pCanvasInput->GetWindowRectToParent(rcInput);
	m_nHistoryInputYSpace = rcInput.top - rcHistroy.bottom;

	return 0;
}

LRESULT CTextChatCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (pe->hOriginalSender == m_pHistory->GetSafeHwnd())
	{
		if (pe->RoutedEvent == WM_COMMAND)
		{
			if (HIWORD(pe->wParam) == EN_CHANGE)
			{
				int n = 9;
			}
		}
	}

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

	if (RS_TUNNEL == pe->eRoutingStrategy)
	{
		if (WM_KEYDOWN == pe->RoutedEvent)
		{
			OnPreviewKeyDown(pe);
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
		else if (WM_EXITSIZEMOVE == pe->RoutedEvent)
		{
			return OnExitSizeMove(pe);
		}
		else if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			return OnLButtonDblClk(pe);
		}
		else if (UE_IM_RESEND_MSG == pe->RoutedEvent)
		{
			// resend msg¡£
			CIMMsg *pMsg = (CIMMsg *)pe->lParam;
			pMsg->SetUserData(false);
			bool bFind = false;
			CChatRecord theMsg = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByMsgID(pMsg->GetMsgID(), bFind);

			if (bFind == true)
			{
				if (theMsg.m_nIsMyself)
				{
					if (theMsg.m_nMSGType== ENUM_MSG_TEXT)
					{
						ResendMsg(theMsg);
					}
					else if (theMsg.m_enumFileStatus==ENUM_OK)
					{
						ResendMsg(theMsg);
					}
					else
					{
						ReDownLoadFile(theMsg);
					}
					pMsg->SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
				}
				else
				{
					ReDownLoadFile(theMsg);
				}
				
			}
			pe->bHandled = TRUE;
			
		}
		else if (UE_IM_SHOW_MORE_MSG == pe->RoutedEvent)
		{
			int nMsgID = GetLocalRecordMSGID();
			vector<CChatRecord> vecMoreMSG;
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			std::string strSipURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
			std::string strSignMsgID;
			if (pContactDB != NULL)
			{
				CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
				FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

				//add_friends
				HIUIIMAGE hFace = ImageManager::CreateIUIImage(IUI::GetProject(m_pHistory->GetSafeHwnd()),
					strSrcHeadImage);
				ImageManager::ReleaseIUIImage(hFace);
				int nCurrentMinHistortMsgID = GetLocalRecordMSGID();
				vecMoreMSG = CSystemEx::GetInstance()->GetDBOprate()->searchRecordForwardByContact(nCurrentMinHistortMsgID,strSipURL, CHAT_MSG_PER_PAGE, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
				auto iterChatMsg = vecMoreMSG.begin();
				int nIndex = 0;
				for (iterChatMsg; iterChatMsg < vecMoreMSG.end(); iterChatMsg++)
				{
					
					CChatRecord theLocalChatMsg = *iterChatMsg;
					OutputDebugString(PortUtility::Utf82Unicode_ND(theLocalChatMsg.m_strMSGTime).c_str());
					OutputDebugString(L"\n");
					if (m_nCurrentMinHistoryMsgID > theLocalChatMsg.m_nMsgID||m_nCurrentMinHistoryMsgID==-1)
					{
						m_nCurrentMinHistoryMsgID = theLocalChatMsg.m_nMsgID;
					}
					if (iterChatMsg==vecMoreMSG.begin())
					{
						strSignMsgID = theLocalChatMsg.m_strSignMsgID;
					}
					if (theLocalChatMsg.m_nMSGType == ENUM_MSG_VIDEO || theLocalChatMsg.m_nMSGType == ENUM_MSG_AUDIO)
					{
						CallEventFromDB(theLocalChatMsg, INSERT_BACKWARD);
					}

					if (theLocalChatMsg.m_nMSGType == ENUM_MSG_TEXT)
					{
						TextMsgFromDB(theLocalChatMsg,INSERT_BACKWARD);
					}
					else if (theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE || theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE_PIC || \
						theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE_VIDEO || theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE_AUDIO || \
						theLocalChatMsg.m_nMSGType == ENUM_MSG_SCREEN_PIC)
					{
						std::string strErrorInfo = "ENUM_MSG_SCREEN_PIC Begin  ";
						LOG4_INFO(strErrorInfo.c_str());

						if (theLocalChatMsg.m_nIsMyself)
						{
							std::string strSipNumber = CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber;
							strSrcUserName.Format(L"%s", PortUtility::Utf82Unicode_ND(strSipNumber).c_str());
						}
						FileMsgFromDB(theLocalChatMsg, strSrcUserName.GetBuffer(strSrcUserName.GetLength()),INSERT_BACKWARD);
						strErrorInfo = "ENUM_MSG_SCREEN_PIC End  ";
						LOG4_INFO(strErrorInfo.c_str());
					}
					if (theLocalChatMsg.m_nMSGStatus == ENUM_MSG_STATUS_SENDFAILD)
					{
						m_pHistory->ShowResendButton(theLocalChatMsg.m_nMsgID, TRUE);
#if _DEBUG
						CIUIString strInfo;
						strInfo.Format(L"%ld down load failed", theLocalChatMsg.m_nMsgID);
						OutputDebugString(strInfo);
#endif
						Invalidate(m_pHistory->GetSafeHwnd());
					}
				}
				
				std::string strLastUnReadMsgID = "0";
				std::string strSipURL = strSipURL;
				if (!strSignMsgID.empty())
				{
					strLastUnReadMsgID = strSignMsgID;
				}
#if _DEBUG
				TIMER_START(YY);
#endif
				std::vector<CChatRecord> vecUnreadChatRecord;
				vecUnreadChatRecord = CSystemEx::GetInstance()->GetUnReadMsgByContact(pContactDB, strLastUnReadMsgID, SPECIFY_LIST_BEFORE);

#if _DEBUG
				TIMER_STOP(YY);
				CIUIString strFormatInfo;
				strFormatInfo.Format(L" MoreMsg 1 Cost %d ms.\n", TIMER_MSEC(YY));
				OutputDebugString(strFormatInfo);
#endif
				auto iterRecordOffLineMsg = vecUnreadChatRecord.begin();
				for (iterRecordOffLineMsg; iterRecordOffLineMsg < vecUnreadChatRecord.end(); iterRecordOffLineMsg++)
				{
					CChatRecord theLocalOneChatRecord = *iterRecordOffLineMsg;
					if (ENUM_MSG_SHAKE != theLocalOneChatRecord.m_nMSGType)
					{
						CSystemEx::GetInstance()->GetDBOprate()->saveOneOffLineChatRecord(theLocalOneChatRecord);
					}
				}
			}
			

			
			//if has more msg
			bool bHaveMoreMsg = CSystemEx::GetInstance()->GetDBOprate()->searchRecordForwardMoreMsg(m_nCurrentMinHistoryMsgID, strSipURL,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
			if (bHaveMoreMsg==false)
			{
				CancleMoreMsg();
			}
			else
			{
				//CancleMoreMsg();
				//m_pHistory->InsertShowMoreMessage(0, _T("more message"));
			}
			m_pHistory->EnsureMsgVisible(m_nCurrentMinHistoryMsgID);

		
		}
		else if (WM_DRAWITEM == pe->RoutedEvent)
		{
		if (pe->hOriginalSender == m_pHistory->GetSafeHwnd())
		{
			return OnIMDrawItem(pe);
		}
		}
	}
	else if (RS_DIRECT == pe->eRoutingStrategy)
	{
		if (WM_SELECTED_EMOTICON == pe->RoutedEvent)
		{
			return OnSelectedEmoticon(pe);
		}
	}

	return 0;
}

LRESULT CTextChatCanvasHandler::OnChkEmoticon(RoutedEventArgs *pe)
{
	if (NULL == m_pEmoticonWnd)
	{
		m_pEmoticonWnd = new CEmoticonWnd(GetBindWLWnd());
	}

	if (NULL != m_pEmoticonWnd)
	{
		if (!IsWindow(m_pEmoticonWnd->GetSafeHwnd()))
		{
			m_pEmoticonWnd->SetProject(GetProject(GetBindWLWnd()));
			BOOL bRet = m_pEmoticonWnd->CreatePopupWindow(_T("IDW_EMOTICON.xml"), GetHostWnd(GetBindWLWnd()));
			if (bRet==FALSE)
			{
				LOG4_ERROR("create failed emotion wnd");
				return 0;
			}
		}
		if (IsWindow(m_pEmoticonWnd->GetSafeHwnd()))
		{
			CIUIRect rcButton;
			m_pChkEmoticon->GetWindowRect(rcButton);

			CIUIRect rcWnd;
			m_pEmoticonWnd->GetWindowRect(rcWnd);

			::MoveWindow(m_pEmoticonWnd->GetSafeHwnd(), rcButton.left, rcButton.top - rcWnd.Height(),
				rcWnd.Width(), rcWnd.Height(), FALSE);
			m_pEmoticonWnd->ShowWindow(SW_SHOW);
			m_pEmoticonWnd->Invalidate();
		}
	}

	return 0;
}

LRESULT CTextChatCanvasHandler::OnBtnVibrate(RoutedEventArgs *pe)
{
	if (GroupEnableFun()==true)
	{
		return 0; 
	}

	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

	std::string strMsg = CSystemEx::GetInstance()->ConstructShakeMessage();
	CSystemEx::GetInstance()->SendJsonMsg(pContactDB->m_strSIPNum, strMsg);

	CSystemEx::GetInstance()->GetMainDlg()->ShakeSelf();
	return 0;
}
bool CTextChatCanvasHandler::GroupEnableFun()
{
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = m_pContactInfo->m_enumRecentContact;
	if (localRecentContactType == ENUM_MULTI_CONTACT)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"In TalkGroup this fun is disable,waiting open", NULL);
		return true;
	}
	else
	{
		return false;
	}
}
LRESULT CTextChatCanvasHandler::OnChkScreenshot(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}

	CScreenPrint theScreenDlg;
	theScreenDlg.Init();
	char szFileName[256] = { 0 };
	std::string strFilePath;
	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);
	CIUIString strTempDataFolder = CSystemEx::GetInstance()->GetDataFolder();
	if (strTempDataFolder.IsEmpty())
	{
		LOG4_ERROR("save file path is error");
		strTempDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
	}
	strFilePath = PortUtility::wstring2String(strTempDataFolder.GetBuffer(strTempDataFolder.GetLength())) + "\\cacheFiles\\screenprint";
	CIUIString wstrFilePath = strTempDataFolder + L"\\cacheFiles\\screenprint";
	PortUtility::CheckFilePath(strFilePath);

	int nResponse = theScreenDlg.ShowDlg(stoll(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber), wstrFilePath.GetBuffer(wstrFilePath.GetLength()), szFileName,256);
	if (nResponse== IDCANCEL)
	{
		return 0;
	}
	Add2UploadListParam *pAdd2UploadListParam = new Add2UploadListParam();

	pAdd2UploadListParam->strFilePath = CSystemEx::GetInstance()->GetFileSavePathByMsgType(ENUM_MSG_SCREEN_PIC).c_str();
	pAdd2UploadListParam->strFileName = szFileName;
	HWLWND hWnd = NULL;
	InsertInputImageOle(m_pReInput->GetSafeHwnd(), pAdd2UploadListParam->strFilePath+ pAdd2UploadListParam->strFileName.GetBuffer(pAdd2UploadListParam->strFileName.GetLength()), -1, hWnd);
	pAdd2UploadListParam->strFileType = MESSAGE_TYPE_IMAGE;
	pAdd2UploadListParam->hWnd = hWnd;
	pAdd2UploadListParam->enumFileType = ENUM_MSG_FILE_PIC;

	IUI::SetProp(hWnd, GETPROP_FILE_INFO, pAdd2UploadListParam);
	return 0;
}

LRESULT CTextChatCanvasHandler::OnBtnSendImage(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}
	
	FileDialog dlg(_T("Image files(*.bmp;*.jpg;*.jpeg;*.gif;*.png)|*.bmp;*.jpg;*.jpeg;*.gif;*.png|All Files(*.*)|*.*||"));
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return 0;
	}
	HWLWND hWnd = NULL;
	InsertInputImageOle(m_pReInput->GetSafeHwnd(), dlg.GetPathName(), -1, hWnd);
	Add2UploadListParam *pAdd2UploadListParam = new Add2UploadListParam();

	pAdd2UploadListParam->strFilePath = dlg.GetPathName();
	pAdd2UploadListParam->strFileName = dlg.GetFileName();
	int nPos = pAdd2UploadListParam->strFilePath.Find(pAdd2UploadListParam->strFileName);
	if (nPos >= 0)
	{
		pAdd2UploadListParam->strFilePath = pAdd2UploadListParam->strFilePath.Left(nPos);
	}
	pAdd2UploadListParam->strFileType = MESSAGE_TYPE_IMAGE;
	pAdd2UploadListParam->hWnd = hWnd;
	pAdd2UploadListParam->enumFileType = ENUM_MSG_FILE_PIC;
	
	IUI::SetProp(hWnd, GETPROP_FILE_INFO, pAdd2UploadListParam);
	return 0;
}

LRESULT CTextChatCanvasHandler::OnBtnSendFile(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}


	FileDialog dlg(_T("All files(*.*)|*.*||"));
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return 0;
	}
	HWLWND hWnd = NULL;
	InsertInputFileOle(m_pReInput->GetSafeHwnd(), dlg.GetFileName(), dlg.GetPathName(), -1, hWnd);
	Add2UploadListParam *pAdd2UploadListParam = new Add2UploadListParam();

	pAdd2UploadListParam->strFilePath  = dlg.GetPathName();
	pAdd2UploadListParam->strFileName  = dlg.GetFileName();
	int nPos = pAdd2UploadListParam->strFilePath.Find(pAdd2UploadListParam->strFileName);
	if (nPos >= 0)
	{
		pAdd2UploadListParam->strFilePath = pAdd2UploadListParam->strFilePath.Left(nPos);
	}
	pAdd2UploadListParam->strFileType = MESSAGE_TYPE_FILE;
	pAdd2UploadListParam->hWnd = hWnd;
	pAdd2UploadListParam->enumFileType = ENUM_MSG_FILE;
	IUI::SetProp(hWnd, GETPROP_FILE_INFO, pAdd2UploadListParam);
	return 0;
}

LRESULT CTextChatCanvasHandler::OnBtnSendVideo(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}
	FileDialog dlg(_T("Video files(*.mpeg;*.avi;*.wmv;*.mp4;)|*.mpeg;*.avi;*.wmv;*.mp4|All Files(*.*)|*.*||"));
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return 0;
	}

	HWLWND hWnd = NULL;
	
	Add2UploadListParam *pAdd2UploadListParam = new Add2UploadListParam();

	pAdd2UploadListParam->strFilePath = dlg.GetPathName();
	pAdd2UploadListParam->strFileName = dlg.GetFileName();
	std::string strFilePath = PortUtility::Unicode2Utf8_ND(pAdd2UploadListParam->strFilePath.GetBuffer(pAdd2UploadListParam->strFilePath.GetLength()));
	int nPos = pAdd2UploadListParam->strFilePath.Find(pAdd2UploadListParam->strFileName);
	if (nPos >= 0)
	{
		pAdd2UploadListParam->strFilePath = pAdd2UploadListParam->strFilePath.Left(nPos);
	}
	int nDuration = 0;


	pAdd2UploadListParam->strPlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFilePath, nDuration);
	InsertInputVideoOle(m_pReInput->GetSafeHwnd(), dlg.GetFileName(), pAdd2UploadListParam->strFilePath, pAdd2UploadListParam->strPlayTime, hWnd, L"");
	pAdd2UploadListParam->strFileType = MESSAGE_TYPE_AUDIO;
	pAdd2UploadListParam->hWnd = hWnd;
	pAdd2UploadListParam->enumFileType = ENUM_MSG_FILE_VIDEO;
	IUI::SetProp(hWnd, GETPROP_FILE_INFO, pAdd2UploadListParam);
	return 0;
}

LRESULT CTextChatCanvasHandler::OnBtnSendAudio(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}
	FileDialog dlg(_T("Audio files(*.mp3;*.wma;*.wav)|*.mp3;*.wma;*.wav|All Files(*.*)|*.*||"));
	INT_PTR nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return 0;
	}
	
	HWLWND hWnd = NULL;

	Add2UploadListParam *pAdd2UploadListParam = new Add2UploadListParam();

	pAdd2UploadListParam->strFilePath = dlg.GetPathName();
	pAdd2UploadListParam->strFileName = dlg.GetFileName();
	std::string strFilePath = PortUtility::Unicode2Utf8_ND(pAdd2UploadListParam->strFilePath.GetBuffer(pAdd2UploadListParam->strFilePath.GetLength()));

	int nPos = pAdd2UploadListParam->strFilePath.Find(pAdd2UploadListParam->strFileName);
	if (nPos >= 0)
	{
		pAdd2UploadListParam->strFilePath = pAdd2UploadListParam->strFilePath.Left(nPos);
	}
	int nDuration = 0;


	pAdd2UploadListParam->strPlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFilePath, nDuration);
	InsertInputAudioOle(m_pReInput->GetSafeHwnd(), dlg.GetFileName(), pAdd2UploadListParam->strFilePath, pAdd2UploadListParam->strPlayTime, hWnd, L"");
	pAdd2UploadListParam->strFileType = MESSAGE_TYPE_VIDEO;
	pAdd2UploadListParam->hWnd = hWnd;
	pAdd2UploadListParam->enumFileType = ENUM_MSG_FILE_AUDIO;
	IUI::SetProp(hWnd, GETPROP_FILE_INFO, pAdd2UploadListParam);
	return 0;
}
void CTextChatCanvasHandler::AddFileOleByRecvMSG(HWLWND hReInput, LPCTSTR lpszVideo, int cp, HWLWND &hOleWnd)
{

}
LRESULT CTextChatCanvasHandler::OnBtnSendMsg(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}
	if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
	{
		CIUIString strInfo = CSystemEx::GetInstance()->GetOffLineCanNotDoText();
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
		return 0;
	}
	CComPtr<IRichEditOle> pRichEditOle = m_pReInput->GetIRichEditOle();
	_ASSERT(NULL != (IRichEditOle *)pRichEditOle);

	// OLEcp (character position)
	int nPreviousOleCp = 0;

	//HIUIIMAGE hFace = ImageManager::CreateIUIImage(IUI::GetProject(m_pHistory->GetSafeHwnd()),
	//		_T("add_friends.png"));
	CIUIString strFaceImage =PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage()).c_str();
	HIUIIMAGE hFace = ImageManager::CreateIUIImage(IUI::GetProject(m_pHistory->GetSafeHwnd()),
		strFaceImage);
	std::vector<CIMMsg> vAloneMsg;
	std::string strSipNumber = CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber;
	std::wstring wsSender = PortUtility::string2WString(strSipNumber);
	CIMMsg msg;
	msg.SetMsgSenderTitle(1);
	msg.SetSelf(TRUE);
	msg.SetMsgSender(wsSender.c_str());
	msg.SetFace(hFace);
	msg.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
	msg.SetMsgSenderID(wsSender.c_str());
	CIUIString strTemplateTextMsg = "";
	CIUIString strRicheditText = "";
	LONG lOleCount = pRichEditOle->GetObjectCount();
	for (LONG i = 0; i < lOleCount; ++i)
	{
		REOBJECT reObj;
		reObj.cbStruct = sizeof(REOBJECT);
		pRichEditOle->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);

		int cp = reObj.cp;
		if (cp > nPreviousOleCp)
		{
			CIUIString strPreviousString;
			m_pReInput->GetTextRange(nPreviousOleCp, cp, strPreviousString);

			CRichMsgItem item;
			item.m_strMsgContent = strPreviousString;
			strTemplateTextMsg += strPreviousString;
			msg.AddMsgItem(&item);
		}

		reObj.poleobj->Release();
		OleControl *pOleControl = NULL;
		reObj.poleobj->QueryInterface(IID_IUIOleControl, (void **)&pOleControl);
		if (NULL != pOleControl)
		{
			HWLWND hControl = pOleControl->GetOleBindWindowless();
			Add2UploadListParam *pUpLoadParam = (Add2UploadListParam *)IUI::GetPropW(hControl, GETPROP_FILE_INFO);

			if (pUpLoadParam==NULL)
			{
				CIUISize sizeOle;
				LPARAM lParam = IUI::GetUserData(hControl);
				if (lParam != 0)
				{
					sizeOle.cx = LOWORD(lParam);
					sizeOle.cy = HIWORD(lParam);
				}
				else
				{
					CIUIRect rcOle;
					GetWindowRectToParent(hControl, rcOle);
					sizeOle.cx = rcOle.Width();
					sizeOle.cy = rcOle.Height();
				}

				CRichMsgItem item;
				item.m_bOleControl = TRUE;
				item.m_hOleBindWindowless = hControl;
				item.m_size = sizeOle;

				CIUIString strFileName = GetTag(hControl);
				msg.AddMsgItem(&item);
			
				CIUIString strFormatEmoji = FormatEmojiText(strFileName);
				strTemplateTextMsg += strFormatEmoji;
			}
			else
			{
				HWLWND hNewControl = NULL;
				CChatRecord theLocalChatRecord;
				theLocalChatRecord.m_nIsMyself = true;

				theLocalChatRecord.m_nMSGType = pUpLoadParam->enumFileType;
				theLocalChatRecord.m_strTalkerURL = m_pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
				theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
				std::wstring  strFileName = L"", strFileFullPath = L"";
				
				strFileFullPath = pUpLoadParam->strFilePath ;
				strFileName = pUpLoadParam->strFileName;
				CIUIString strPlayTime,  strtemplateFilePath;
				strtemplateFilePath = (pUpLoadParam)->strFilePath + (pUpLoadParam)->strFileName;
				int nDuration = 0;
				switch (pUpLoadParam->enumFileType)
				{
				case ENUM_MSG_FILE_AUDIO:
				case ENUM_MSG_FILE_VIDEO:
					strPlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(PortUtility::Unicode2Utf8_ND(strtemplateFilePath.GetBuffer(strtemplateFilePath.GetLength())), nDuration);
					theLocalChatRecord.m_strPlayTime =PortUtility::Unicode2Utf8_ND(strPlayTime.GetBuffer(strPlayTime.GetLength()));
					break;
				default:
					break;
				}
				theLocalChatRecord.m_strSendLocalFilePath = PortUtility::Unicode2Utf8_ND(strFileFullPath);
				theLocalChatRecord.m_strSendLocalFileName = PortUtility::Unicode2Utf8_ND(strFileName);
				theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_SENDSUCC;
				pUpLoadParam->m_lMsgID=CSystemEx::GetInstance()->GetDBOprate()->saveOneChatRecord(theLocalChatRecord);
				switch (pUpLoadParam->enumFileType)
				{
				case ENUM_MSG_FILE_PIC:
				case  ENUM_MSG_SCREEN_PIC:
					InsertHistoryImageOle(m_pHistory->GetSafeHwnd(), TRUE, pUpLoadParam->strFilePath , pUpLoadParam->strFileName, hNewControl, wsSender.c_str(), pUpLoadParam->m_lMsgID,INSERT_FORWARD, strFaceImage, wsSender.c_str());
					break;
				case ENUM_MSG_FILE:
					InsertHistoryFileOle(m_pHistory->GetSafeHwnd(), TRUE, pUpLoadParam->strFileName, pUpLoadParam->strFilePath, hNewControl, wsSender.c_str(),pUpLoadParam->m_lMsgID, INSERT_FORWARD, strFaceImage, wsSender.c_str());
					break;
				case ENUM_MSG_FILE_AUDIO:
					InsertHistoryAudioOle(m_pHistory->GetSafeHwnd(), TRUE, pUpLoadParam->strFileName, pUpLoadParam->strFilePath, pUpLoadParam->strPlayTime, hNewControl, wsSender.c_str(), pUpLoadParam->m_lMsgID, INSERT_FORWARD, strFaceImage, wsSender.c_str());
					break;
				case ENUM_MSG_FILE_VIDEO:
					InsertHistoryVideoOle(m_pHistory->GetSafeHwnd(), TRUE, pUpLoadParam->strFileName, pUpLoadParam->strFilePath, pUpLoadParam->strPlayTime, hNewControl,NULL, wsSender.c_str(), pUpLoadParam->m_lMsgID, INSERT_FORWARD, strFaceImage, wsSender.c_str());
					break;
				default:
					break;
				}
				pUpLoadParam->hWnd = hNewControl;
				if (pUpLoadParam != NULL)
				{
					if (pUpLoadParam->hWnd != NULL)
					{
						Add2UploadList(pUpLoadParam->strFilePath, pUpLoadParam->strFileName, pUpLoadParam->strFileType, pUpLoadParam->enumFileType, hNewControl,pUpLoadParam->m_lMsgID);
						//delete pUpLoadParam;
						//pUpLoadParam = NULL;
					}
				}
			}
		}
		else
		{
			_ASSERT(FALSE);
		}

		//
		nPreviousOleCp = cp + 1;
	}
	//
	//
	CRichMsgItem item;
	int nTotalLength = m_pReInput->GetTextLength();
	if (nTotalLength > nPreviousOleCp)
	{
		CIUIString strLastString;
		m_pReInput->GetTextRange(nPreviousOleCp, nTotalLength, strLastString);
		strTemplateTextMsg += strLastString;
		strRicheditText += strLastString;
		item.m_strMsgContent = strLastString;
	}
	int nMsgID = 0;
	if (!strTemplateTextMsg.IsEmpty())
	{
		if (!strRicheditText.IsEmpty())
		{
			//item.m_strMsgContent = strRicheditText;
		}
		if (m_pContactInfo != NULL)
		{
			CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
			//FORMAT: sipnumber@sipdomain:port
			std::wstring strMsg = strTemplateTextMsg;
			std::string strSendMsg = PortUtility::Unicode2Utf8_ND(strMsg);

			if (!strSendMsg.empty())
			{
				long lRet = Send(strSendMsg, ENUM_TYPE_PURE);
				if (lRet == -1)
				{

				}
				else
				{
					//SAVE DATE TO DB
					CChatRecord theLocalChatRecord;
					theLocalChatRecord.m_nIsMyself = true;
					theLocalChatRecord.m_strMSG = PortUtility::Unicode2Utf8_ND(strMsg);
					theLocalChatRecord.m_nMSGType = ENUM_MSG_TEXT;
					theLocalChatRecord.m_strTalkerURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
					theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
					std::stringstream strSignMsgID;
					strSignMsgID<<lRet;

					theLocalChatRecord.m_strLocalSignMsgID = strSignMsgID.str();
					if (lRet < 0)
					{
						theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_SENDFAILD;
					}
					else
					{
						theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_SENDING;
					}
					nMsgID = CSystemEx::GetInstance()->GetDBOprate()->saveOneChatRecord(theLocalChatRecord);
				}
				msg.SetMsgID(nMsgID);
				msg.AddMsgItem(&item);
				msg.SetMsgSenderID(wsSender.c_str());
			}
		}
		else
		{
			std::string strErrorInfo = "Can't Find Contact Info  ";
			LOG4_INFO(strErrorInfo.c_str());
		}
	}
	
	//
	//
	if (!msg.GetMsgItems()->empty())
	{
		m_pHistory->InsertMsg(INSERT_FORWARD,&msg);
		ImageManager::ReleaseIUIImage(hFace);
	}
	
	std::vector<CIMMsg>::iterator it = vAloneMsg.begin();
	for (; it != vAloneMsg.end(); ++it)
	{
		CIMMsg *pMsg = &(*it);
		m_pHistory->InsertMsg(INSERT_FORWARD, pMsg);
		ImageManager::ReleaseIUIImage(hFace);
	}

	//RefreshMsgStatus(nMsgID, false);
	m_pHistory->SendMessage(EM_SCROLL, SB_BOTTOM, 0);
	m_pReInput->SetWindowText(_T(""));
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
	return 0;
}
long CTextChatCanvasHandler::Send(std::string strSendMsg,ENUM_SENDMSG_TYPE enumSendMsgType)
{
	if (strSendMsg.length()> MSG_MAX_LENGTH)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", _T("The content of the message sent is too long. Please send it in sections."),GetHostWnd(GetBindWLWnd()));
		return -1;
	}
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	std::string strRecverSipNumber = pContactDB->m_strSIPNum;
	std::string strSipDomain = CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	std::stringstream strSipPort;
	strSipPort << CSystemEx::GetInstance()->GetSipDomainPort();
	std::string strSendTo = strRecverSipNumber + "@" + strSipDomain + ":" + strSipPort.str();
	long lResulr = -1;
	switch (enumSendMsgType)
	{
	case ENUM_TYPE_PURE:
		lResulr = CSystemEx::GetInstance()->SendMsg(strSendTo, strSendMsg);
		break;
	case ENUM_TYPE_JSON:
		lResulr = CSystemEx::GetInstance()->SendJsonMsg(strSendTo, strSendMsg);
		break;
	default:
		break;
	}
	CSystemEx::GetInstance()->RefreshRecentTime(m_pContactInfo);
	return lResulr;
}
LRESULT CTextChatCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkEmoticon->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkEmoticon(pe);
	}
	else if (m_pBtnVibrate->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnVibrate(pe);
	}
	else if (m_pChkScreenshot->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkScreenshot(pe);
	}
	else if (m_pBtnSendImage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSendImage(pe);
	}
	else if (m_pBtnSendFile->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSendFile(pe);
	}
	else if (m_pBtnSendVideo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSendVideo(pe);
	}
	else if (m_pBtnSendAudio->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSendAudio(pe);
	}
	else if (m_pBtnSendMsg->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSendMsg(pe);
	}
	else if (m_pBtnMsgHistory->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMsgHistory(pe);
	}
	
	return 0;
}
void CTextChatCanvasHandler::CancleMoreMsg()
{
	m_pHistory->DeleteMsg(0);
}
std::wstring CTextChatCanvasHandler::FormatFilePath(bool bSelf, ENUM_MSG_TYPE enumMsgType,CChatRecord oChatRecord,std::wstring wstrFileName,bool bHasDownload)
{
	std::wstring strFilePath = L"";
	if (!bSelf|| (bHasDownload==false))
	{
		strFilePath = CSystemEx::GetInstance()->GetFileSavePathByMsgType(enumMsgType);
	}
	else
	{
		if (oChatRecord.m_strSendLocalFilePath.empty())
		{
			strFilePath = CSystemEx::GetInstance()->GetFileSavePathByMsgType(enumMsgType);
		}
		else
		{
			strFilePath = PortUtility::Utf82Unicode_ND(oChatRecord.m_strSendLocalFilePath);
		}
		
	}
	return strFilePath;
}
void CTextChatCanvasHandler::RefreshMsgStatus(int nMsgID, bool bSucc)
{
	m_pHistory->ShowResendButton(nMsgID, (BOOL)bSucc);
	Invalidate(m_pHistory->GetSafeHwnd());
#if _DEBUG
	if (bSucc)
	{
		CIUIString strInfo;
		strInfo.Format(L"%ld down load failed", nMsgID);
		OutputDebugString(strInfo);
	}
	else
	{
		CIUIString strInfo;
		strInfo.Format(L"%ld down load OK", nMsgID);
		OutputDebugString(strInfo);
	}
#endif
}
bool CTextChatCanvasHandler::HaveEmoji(std::string strMsg, vector<EMOJI_INFO> &vecEmojiInfo)
{
	bool bHaveEmoji = false;
	char *szMSGData = new char[strMsg.length()];
	memcpy(szMSGData, strMsg.c_str(), strMsg.length());
	int num = 1;
	int nStartPos = -1,nEndPos = -1;
	for (int i = 0; i < strMsg.length(); ++i)
	{
		if (szMSGData[i] == '[')
		{
			if (i!=strMsg.length()-1)
			{
				if (szMSGData[i+1]==':')
				{
					++num;
					nStartPos = i;
				}
			}
		}
		else if (szMSGData[i] == ']')
		{
			--num;
			nEndPos = i;
		}
		if (num == 1&& nStartPos!=-1)
		{
			bHaveEmoji= true;
			EMOJI_INFO theLocalEmoji;
			theLocalEmoji.nStartPos = nStartPos;
			theLocalEmoji.nEndPos = nEndPos;
			theLocalEmoji.strFind.append(szMSGData + nStartPos+2, szMSGData + nEndPos);
			vecEmojiInfo.push_back(theLocalEmoji);
			nStartPos = -1;
		}
	}
	delete szMSGData;
	return bHaveEmoji;
}
void CTextChatCanvasHandler::ReDownLoadFile(CChatRecord reDownLoadRecord)
{
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;

	if (pContactDB == NULL)
		return;

	UpLoadDownLoadParam *pUpLoadParam  = new UpLoadDownLoadParam();
	if (reDownLoadRecord.m_nIsMyself)
	{
		return;
	}

	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

	FileMsgFromDB(reDownLoadRecord, strSrcUserName.GetBuffer(strSrcUserName.GetLength()), INSERT_BACKWARD);

	m_pHistory->ShowResendButton(reDownLoadRecord.m_nMsgID, (BOOL)FALSE);
#if  _DEBUG

	CIUIString strInfo;
	strInfo.Format(L"%ld down load failed", reDownLoadRecord.m_nMsgID);
	OutputDebugString(strInfo);
#endif
	Invalidate(m_pHistory->GetSafeHwnd());

}
void CTextChatCanvasHandler::ResendMsg(CChatRecord theResendMsg)
{
	std::string strSendMsg = theResendMsg.m_strMSG;;
	long lSignMsgID = -1;
	switch (theResendMsg.m_nMSGType)
	{
	case ENUM_MSG_FILE_AUDIO:
	case ENUM_MSG_FILE_VIDEO:
	case ENUM_MSG_FILE_PIC:
	case ENUM_MSG_FILE:
		if (!strSendMsg.empty())
		{
			lSignMsgID = Send(strSendMsg, ENUM_TYPE_JSON);
		}
		break;
	case ENUM_MSG_EMOJI:
		break;
	case ENUM_MSG_TEXT:
		if (!strSendMsg.empty())
		{
			lSignMsgID = Send(strSendMsg, ENUM_TYPE_PURE);
		}
		break;
	}
	if (lSignMsgID!=-1)
	{
		CSystemEx::GetInstance()->GetDBOprate()->updateSignMsgID(theResendMsg.m_nMsgID, lSignMsgID);
	}
}
unsigned int  CTextChatCanvasHandler::AddDownLoadThread(void* pParam)
{
	UpLoadDownLoadParam *pUpLoadParam = (UpLoadDownLoadParam *)pParam;
	
	CSystemEx::GetInstance()->AddFole2DownLoadList(pUpLoadParam);
	return 0;
}

unsigned int CTextChatCanvasHandler::AddUpLoadThread(void* pParam)
{
	UpLoadDownLoadParam *pUpLoadParam = (UpLoadDownLoadParam *)pParam;
	CSystemEx::GetInstance()->AddFile2UploadList(pUpLoadParam);

	return 0;
}
void CTextChatCanvasHandler::TextMsgFromDB(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode)
{
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	ENUM_RECENT_CONTACT_TYPE enum_LocalContactType = m_pContactInfo->m_enumRecentContact;
	if (enum_LocalContactType == ENUM_MULTI_CONTACT)
	{
		return;
	}
	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

	std::string strMsg = oChatRecord.m_strMSG;
	CIMMsg msg7;
	msg7.SetMsgSenderTitle(0);
	CIUIString strFaceImage = L"";
	msg7.SetUserData(false);
	if (oChatRecord.m_nIsMyself)
	{
		msg7.SetSelf(TRUE);
		msg7.SetMsgSender(PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str());
		strFaceImage=PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage()).c_str();
		msg7.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
		msg7.SetMsgSenderID(PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str());
	}
	else
	{
		msg7.SetMsgSender(strSrcUserName);
		msg7.SetSelf(FALSE);
		strFaceImage= strSrcHeadImage;
		msg7.SetMsgSenderID(strSrcSipNumber);
	}
	
	HIUIIMAGE hFace = ImageManager::CreateIUIImage(IUI::GetProject(m_pHistory->GetSafeHwnd()),
		strFaceImage);
	if(hFace!=NULL)
	{
		msg7.SetFace(hFace);
	}
	msg7.SetMsgSenderTitle(0);
	msg7.SetMsgID(oChatRecord.m_nMsgID);
	vector<EMOJI_INFO> vecEmoji;
	bool bHaveEmoji = HaveEmoji(strMsg, vecEmoji);
	if (bHaveEmoji)
	{
		int nMsgStartPos = 0, nRecvMsgStartPos = 0, nRecvEmojiStartPos = 0;
		auto iterEmoji = vecEmoji.begin();
		EMOJI_INFO theLocalRecvEmojiInfo;
		for (iterEmoji; iterEmoji != vecEmoji.end();)
		{
			if (nRecvMsgStartPos >= strMsg.length())
			{
				break;
			}
			theLocalRecvEmojiInfo = *iterEmoji;
			nRecvEmojiStartPos = theLocalRecvEmojiInfo.nStartPos;
			std::string strUTFImage = "skins\\PortGo\\emoji\\" + theLocalRecvEmojiInfo.strFind + ".png";
			if (nRecvEmojiStartPos == nRecvMsgStartPos)
			{
				OleControl *pOleImage = new OleControl(3, m_pHistory->GetSafeHwnd());
				if (NULL == pOleImage)
				{
					return;
				}
				HWLWND hStaImage = IUI::CreateWindowEx(NULL, 0, IUI_LABEL, NULL, WS_VISIBLE,
					0, 0, 0, 0,
					GetHostWnd(m_pHistory->GetSafeHwnd()), m_pHistory->GetSafeHwnd(),
					_T("IDC_STA_IMAGE"), 0, NULL);

				if (NULL == hStaImage)
				{
					_ASSERT(FALSE);
					return;
				}
				CIUIString wImageName = PortUtility::Utf82Unicode_ND(strUTFImage).c_str();
				CIUIString strImage = GetRunFolder() + wImageName;// _T("skins\\emoji\\Title_Center.png");

				IUI::SetProp(hStaImage, GETPROP_OLETYPE, (HANDLE)OT_IMAGE);
				IUI::SetTag(hStaImage, strImage);
				CombineBkImage(hStaImage, FALSE);
				SetBkImage(hStaImage, FALSE, CONTROL_STATE_NORMAL, strImage);
				SetBkImageResizeMode(hStaImage, IRM_KEEPRATE);
				pOleImage->SetOleBindWindowless(hStaImage);

				CRichMsgItem item;
				item.m_bOleControl = TRUE;
				item.m_hOleBindWindowless = hStaImage;
				item.m_size = CIUISize(30, 30);
				msg7.AddMsgItem(&item);
				int nEmojiLength = theLocalRecvEmojiInfo.nEndPos - nRecvEmojiStartPos;
				nRecvMsgStartPos = nRecvMsgStartPos + nEmojiLength + 1;
				iterEmoji++;
			}
			else if (nRecvMsgStartPos < nRecvEmojiStartPos)
			{
				CRichMsgItem item;

				int nTextLength = nRecvEmojiStartPos - nRecvMsgStartPos;
				item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg.substr(nRecvMsgStartPos, nTextLength)).c_str(); //PortUtility::string2WString(strMsg).c_str();
				msg7.AddMsgItem(&item);
				nRecvMsgStartPos = nRecvMsgStartPos + nTextLength;
			}
			else
			{
				OutputDebugString(L"no normal\n");
			}
		}
		//the end
		if (nRecvMsgStartPos > nRecvEmojiStartPos)
		{
			int nTextLength = nRecvMsgStartPos - nRecvEmojiStartPos;
			if (nTextLength >= 0)
			{
				CRichMsgItem item;
				item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg.substr(nRecvMsgStartPos, nTextLength)).c_str();
				msg7.AddMsgItem(&item);
				nRecvMsgStartPos = nRecvMsgStartPos + nTextLength;
			}
		}
	}
	else
	{
		CRichMsgItem item;

		item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg).c_str(); //PortUtility::string2WString(strMsg).c_str();
		msg7.AddMsgItem(&item);
	}


	m_pHistory->InsertMsg(enumInsertHistoryMode, &msg7);
	m_pHistory->SendMessage(EM_SCROLL, SB_BOTTOM, 0);
}
int CTextChatCanvasHandler::CallEventFromDB(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode)
{
	CControlProp *pLabelProp = g_pProject->GetStyleItem(STYLET_LABEL,
		_T("IDC_STA_IM_LABEL"));
	_ASSERT(NULL != pLabelProp);

	CIUIString strTips,strTime,strCallEventType;
	CCallHistory oLocalHistory;
	long long nCallHistoryID = atoll(oChatRecord.m_strMSG.c_str());
	CSystemEx::GetInstance()->GetDBOprate()->GetCallMsgByID(nCallHistoryID, oLocalHistory);

	if (oChatRecord.m_nMSGType == ENUM_MSG_AUDIO)
	{
		strCallEventType = CSystemEx::GetInstance()->GetEventCallAudioTypeText();
	}
	else
	{
		strCallEventType = CSystemEx::GetInstance()->GetEventCallVideoTypeText();
	}

	std::string strFormatTime;
	PortUtility::FormatDateTime(oLocalHistory.m_strBeginTime, strFormatTime, (ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLastLanguage);
	strTime = PortUtility::string2WString(strFormatTime).c_str();


	if ( oLocalHistory.m_enumCallStatus == ENUM_CALL_STATUS_SUCC)
	{
		if (oLocalHistory.m_strBeginTime.empty()|| oLocalHistory.m_strEndTime.empty())
		{
			strTips = strTime + L" " + CSystemEx::GetInstance()->GetEventCallSuccessText() +L"00:00";
		}
		else
		{
			time_t stuCallTime = PortUtility::StringToDatetime(oLocalHistory.m_strEndTime.c_str()) - PortUtility::StringToDatetime(oLocalHistory.m_strBeginTime.c_str());
			tm *pTmp = localtime(&stuCallTime);
			//int nTimeHour = pTmp->tm_hour;
			std::string strFormat;
			int nMin = pTmp->tm_min;
			int nSec = pTmp->tm_sec;
			char szTime[MAX_PATH] = { 0 };
			_snprintf(szTime, MAX_PATH - 1, "%02d:%02d", nMin, nSec);
			strFormat = strFormat.append(szTime);
			strTips = strTime + L" " + CSystemEx::GetInstance()->GetEventCallSuccessText() + PortUtility::string2WString(strFormat).c_str();
			OutputDebugString(strTips);
		}
		
	}
	else if (oLocalHistory.m_enumCallType == ENUM_CALL_OUT && oLocalHistory.m_enumCallStatus!= ENUM_CALL_STATUS_SUCC)
	{
		strTips = strTime + L" "  + CSystemEx::GetInstance()->GetEventCallFailedText();
		OutputDebugString(strTips);
	}
	else if (oLocalHistory.m_enumCallType == ENUM_CALL_IN && oLocalHistory.m_enumCallStatus != ENUM_CALL_STATUS_SUCC)
	{
		if (oChatRecord.m_nMSGType == ENUM_MSG_AUDIO)
		{
			strTips = strTime  +L" "+ CSystemEx::GetInstance()->GetEventCallMissedAudioText();
		}
		else
		{
			strTips = strTime + L" " + CSystemEx::GetInstance()->GetEventCallMissedVideoText();
		}
		OutputDebugString(strTips);
	}

	int nIndex =m_pHistory->InsertLabel(enumInsertHistoryMode, oChatRecord.m_nMsgID, strTips,
		CIUISize(400, 20), (CLabelProp *)pLabelProp);
	m_pHistory->SendMessage(EM_SCROLL, SB_BOTTOM, 0);
	return nIndex;
}
void CTextChatCanvasHandler::FileMsgFromDB(CChatRecord oChatRecord,wstring wstrSender, INSERT_HISTORY_MODE enumInsertHistoryMode, bool isScreenPic)
{
#if _DEBUG
	TIMER_START(XX);
#endif
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	CIUIString strSenderID;
	if (oChatRecord.m_nIsMyself==false)
	{
		FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);
		strSenderID = strSrcSipNumber;
	}
	else
	{
		std::string strFaceImage = CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage();
		strSrcHeadImage = PortUtility::Utf82Unicode_ND(strFaceImage).c_str();
		strSenderID = PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str();
	}
	const Json::Value *val;
	std::string  strFileName = "", strMsgType = "";
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;
	if (!json_reader.parse(oChatRecord.m_strMSG, json_root))
	{
		LOG4_ERROR("json parse error \n");
		LOG4_INFO(oChatRecord.m_strMSG.c_str());
		return;
	}
	val = json_root.find(key_G_FILE_NAME, key_G_FILE_NAME + strlen(key_G_FILE_NAME));
	if (val && val->isString())
	{
		strFileName = val->asString();
	}
	val = json_root.find(key_G_MESSAGE_TYPE, key_G_MESSAGE_TYPE + strlen(key_G_MESSAGE_TYPE));
	
	if (val&&val->isString())
	{
		strMsgType = val->asString();
	}
	ENUM_MSG_TYPE enumMsgType = PortUtility::GetMsgTypeByString(strMsgType);
	
	std::wstring wstrFileName = PortUtility::Utf82Unicode_ND(strFileName);
	HWLWND hOleHwnd = NULL;
	//
	bool bHasDownLoad =oChatRecord.m_enumFileStatus== ENUM_OK?true:false;
	std::wstring strFilePath = FormatFilePath(oChatRecord.m_nIsMyself, enumMsgType, oChatRecord, wstrFileName, bHasDownLoad);
	UpLoadDownLoadParam *pUpLoadParam = NULL;
	
	std::string strTrueFileName = ""; 

	if (bHasDownLoad == FALSE)
	{
		 pUpLoadParam = new UpLoadDownLoadParam();
	}
	std::string strHttpUrl = "";
	val = json_root.find(key_G_FILE_URL, key_G_FILE_URL + strlen(key_G_FILE_URL));

	if (val&&val->isString())
	{
		strHttpUrl = val->asString();
	}
	INT64 nFileSize = 0;
	val = json_root.find(key_G_FILE_SIZE, key_G_FILE_SIZE + strlen(key_G_FILE_SIZE));
	if (val&&val->isInt64())
	{
		nFileSize = val->asInt64();
	}
	
	if (pUpLoadParam != NULL && !strHttpUrl.empty()&& bHasDownLoad==false)
	{
		switch (enumMsgType)
		{
		case ENUM_MSG_FILE_AUDIO:
		{
			CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, strFileName, strHttpUrl, ENUM_MSG_FILE_AUDIO, m_pContactInfo, nFileSize,oChatRecord.m_nMsgID);
			CBaseThread				FileDownLoadThread;
			std::wstring strPlayTime = PortUtility::string2WString(oChatRecord.m_strPlayTime);
			InsertHistoryAudioOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), strPlayTime.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
			FileDownLoadThread.BeginThread(AddDownLoadThread, pUpLoadParam);
		}
		break;
		case ENUM_MSG_FILE_VIDEO:
		{
			CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, strFileName, strHttpUrl, ENUM_MSG_FILE_VIDEO, m_pContactInfo, nFileSize, oChatRecord.m_nMsgID);
			CBaseThread				FileDownLoadThread;
			std::wstring strPlayTime = PortUtility::string2WString(oChatRecord.m_strPlayTime);
			void *pNewVideoOleWnd = NULL;
			InsertHistoryVideoOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), strPlayTime.c_str(), hOleHwnd, &pNewVideoOleWnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
			FileDownLoadThread.BeginThread(AddDownLoadThread, pUpLoadParam);
		}

		break;
		case ENUM_MSG_FILE_PIC:
		{
			CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, strFileName, strHttpUrl, ENUM_MSG_FILE_PIC, m_pContactInfo, nFileSize, oChatRecord.m_nMsgID);
			CBaseThread				FileDownLoadThread;
			
			std::wstring  strFullFilePath = L"";
			//strFullFilePath = strFilePath + wstrFileName;
			strFullFilePath = strFilePath;
			InsertHistoryImageOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, strFullFilePath.c_str(), wstrFileName.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
			FileDownLoadThread.BeginThread(AddDownLoadThread, pUpLoadParam);
		}
		break;
		case ENUM_MSG_FILE:
		{
			CSystemEx::GetInstance()->FormatDownParam(pUpLoadParam, strFileName, strHttpUrl, ENUM_MSG_FILE, m_pContactInfo, nFileSize, oChatRecord.m_nMsgID);
			CBaseThread				FileDownLoadThread;
			InsertHistoryFileOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
			FileDownLoadThread.BeginThread(AddDownLoadThread, pUpLoadParam);
		}

		break;
		case ENUM_MSG_EMOJI:
			//todo
			break;
		}
	}
	else
	{
		switch (enumMsgType)
		{
		case ENUM_MSG_FILE_AUDIO:
		{
			std::wstring strPlayTime = PortUtility::string2WString(oChatRecord.m_strPlayTime);
			InsertHistoryAudioOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), strPlayTime.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
		}
		break;
		case ENUM_MSG_FILE_VIDEO:
		{
			std::wstring strPlayTime = PortUtility::string2WString(oChatRecord.m_strPlayTime);
			void *pNewVideoOleWnd = NULL;
			InsertHistoryVideoOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), strPlayTime.c_str(), hOleHwnd, &pNewVideoOleWnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
		}
		break;
		case ENUM_MSG_FILE_PIC:
		{
			std::wstring  strFullFilePath = L"";
			strFullFilePath = strFilePath ;
			InsertHistoryImageOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, strFullFilePath.c_str(),wstrFileName.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
		}
		break;
		case ENUM_MSG_FILE:
		{
			InsertHistoryFileOle(m_pHistory->GetSafeHwnd(), oChatRecord.m_nIsMyself, wstrFileName.c_str(), strFilePath.c_str(), hOleHwnd, wstrSender, oChatRecord.m_nMsgID, enumInsertHistoryMode, strSrcHeadImage, strSenderID);
		}
		break;
		}
	}
	//OutputDebugString(L"CHAT KKKKKKKK\n");
#if _DEBUG
	TIMER_STOP(XX);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" FileMsgFromDB 1 Cost %d ms.\n", TIMER_MSEC(XX));
	OutputDebugString(strFormatInfo);
#endif
}
int CTextChatCanvasHandler::InitChatHistory()
{
	m_pHistory->SetSenderIndent(40);
	m_pHistory->SetMsgLeftIndent(52);
	m_pHistory->SetMsgRightIndent(50);
	m_pHistory->SetMaxRelayoutWidth(1000);
	//m_pHistory->SetMsgSenderTitleAlias(0, _T(""));
	//m_pHistory->SetMsgSenderTitleAlias(1, _T(""));
	m_pHistory->ShowMsgSenderTitle(TRUE);
	m_pHistory->ShowMsgSenderAlias(FALSE);
	m_pHistory->SetTitleSpaceBefore(5); 
	m_pHistory->SetTitleSpaceAfter(8); 
	m_pHistory->SetTitleIndents(10); 
	m_pHistory->SetTitleColor(RGB(0, 255, 0)); 

	m_pHistory->SetCombineMsg(TRUE);
	m_pHistory->SetSenderTopQiPao(_T("pop_left_up.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSenderMidQiPao(_T("pop_left_mid.png"), CIUIRect(1, 15, 15, 15));
	m_pHistory->SetSenderBottomQiPao(_T("pop_left_end.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSelfTopQiPao(_T("pop_right_up.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetSelfMidQiPao(_T("pop_right_mid.png"), CIUIRect(15, 15, 1, 15));
	m_pHistory->SetSelfBottomQiPao(_T("pop_right_end.png"), CIUIRect(15, 15, 15, 15));
	m_pHistory->SetResendButtonSize(20, 20);
	m_pHistory->SetResendButtonMarign(20, 0);
	LONG l = m_pHistory->GetEventMask();
	m_pHistory->SetEventMask(l | ENM_CHANGE);
	//m_pHistory->SetShowMoreMsgButtonTextColor(RGB(18, 183, 245));
	CControlProp *pResendButtonStyle = g_pProject->GetStyleItem(STYLET_PUSHBUTTON,
		_T("IDC_BTN_RESEND_BUTTON"));
	_ASSERT(NULL != pResendButtonStyle);
	m_pHistory->SetResendButtonStyle((const CButtonProp *)pResendButtonStyle);

	CControlProp *pShowMoreMsgButtonStyle = g_pProject->GetStyleItem(STYLET_PUSHBUTTON,
		_T("IDC_BTN_SHOW_MORE_MSG"));
	_ASSERT(NULL != pShowMoreMsgButtonStyle);
	m_pHistory->SetShowMoreMsgButtonStyle((const CButtonProp *)pShowMoreMsgButtonStyle);
	m_pHistory->RevokeDragDrop();
	if (m_pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
	{
		return 0;
	}
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	std::string strSipURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;

	
	vector<CChatRecord> vecLocalChatMsg;

	if (pContactDB != NULL)
	{
		
		vecLocalChatMsg = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByContact(strSipURL, CHAT_MSG_PER_PAGE,CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
		auto iterChatMsg = vecLocalChatMsg.rbegin();

		for (iterChatMsg; iterChatMsg < vecLocalChatMsg.rend(); iterChatMsg++)
		{

			CChatRecord theLocalChatMsg = *iterChatMsg;
			if (m_nCurrentMinHistoryMsgID > theLocalChatMsg.m_nMsgID || m_nCurrentMinHistoryMsgID == -1)
			{
				m_nCurrentMinHistoryMsgID = theLocalChatMsg.m_nMsgID;
			}
		}
		bool bHaveMoreMsg = CSystemEx::GetInstance()->GetDBOprate()->searchRecordForwardMoreMsg(m_nCurrentMinHistoryMsgID, strSipURL, CSystemEx::GetInstance()->GetGlobalUser()->GetUID());
		if (bHaveMoreMsg == false)
		{
			CancleMoreMsg();
		}
		else
		{
			//CancleMoreMsg();
			m_pHistory->InsertShowMoreMessage(0, CSystemEx::GetInstance()->GetMoreMsgText());
		}
		iterChatMsg = vecLocalChatMsg.rbegin();
		
		int nIndex = 0;
		for (iterChatMsg; iterChatMsg < vecLocalChatMsg.rend(); iterChatMsg++)
		{
			CChatRecord theLocalChatMsg = *iterChatMsg;
			OutputDebugString(PortUtility::Utf82Unicode_ND(theLocalChatMsg.m_strMSGTime).c_str());
			OutputDebugString(L"\n");

			if (m_nCurrentMinHistoryMsgID > theLocalChatMsg.m_nMsgID || m_nCurrentMinHistoryMsgID == -1)
			{
				m_nCurrentMinHistoryMsgID = theLocalChatMsg.m_nMsgID;
			}

			CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
			FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

			if (theLocalChatMsg.m_nMSGType==ENUM_MSG_TEXT)
			{
				TextMsgFromDB(theLocalChatMsg,INSERT_FORWARD);
			}
			else if (theLocalChatMsg.m_nMSGType ==ENUM_MSG_FILE|| theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE_PIC||\
				 theLocalChatMsg.m_nMSGType == ENUM_MSG_FILE_VIDEO||theLocalChatMsg.m_nMSGType== ENUM_MSG_FILE_AUDIO||\
				theLocalChatMsg.m_nMSGType == ENUM_MSG_SCREEN_PIC)
			{
				std::string strErrorInfo = "ENUM_MSG_SCREEN_PIC Begin  ";
				LOG4_INFO(strErrorInfo.c_str());

				if (theLocalChatMsg.m_nIsMyself)
				{
					std::string strSipNumber = CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber;
					strSrcUserName.Format(L"%s", PortUtility::Utf82Unicode_ND(strSipNumber).c_str());
				}
				FileMsgFromDB(theLocalChatMsg, strSrcUserName.GetBuffer(strSrcUserName.GetLength()), INSERT_FORWARD);
				strErrorInfo = "ENUM_MSG_SCREEN_PIC End  ";
				LOG4_INFO(strErrorInfo.c_str());
			}
			if (theLocalChatMsg.m_nMSGType == ENUM_MSG_VIDEO || theLocalChatMsg.m_nMSGType == ENUM_MSG_AUDIO)
			{
				CallEventFromDB(theLocalChatMsg, INSERT_FORWARD);
			}
			if (theLocalChatMsg.m_nMSGStatus == ENUM_MSG_STATUS_SENDFAILD)
			{
				m_pHistory->ShowResendButton(theLocalChatMsg.m_nMsgID, TRUE);
#if  _DEBUG

				CIUIString strInfo;
				strInfo.Format(L"%ld down load failed", theLocalChatMsg.m_nMsgID);
				OutputDebugString(strInfo);
#endif
				Invalidate(m_pHistory->GetSafeHwnd());
			}
		}
	}
	
	return 0;
}

LRESULT CTextChatCanvasHandler::OnExitSizeMove(RoutedEventArgs *pe)
{
	if (pe->hOriginalSender == m_pCanvasInput->GetSafeHwnd())
	{
		CIUIRect rcNow;
		m_pCanvasInput->GetWindowRectToParent(rcNow);

		CIUIRect rcHistoryMargin;
		m_pHistory->GetControlMargin(rcHistoryMargin);

		CIUIRect rcParent;
		if (GetParent(pe->hOriginalSender) == NULL)
		{
			::GetClientRect(GetHostWnd(pe->hOriginalSender), rcParent);
		}
		else
		{
			IUI::GetClientRect(GetParent(pe->hOriginalSender), rcParent);
		}

		rcHistoryMargin.bottom = rcParent.Height() - rcNow.top + m_nHistoryInputYSpace;
		m_pHistory->SetControlMargin(rcHistoryMargin);
		CanvasLayoutCtrl(m_pHistory->GetSafeHwnd());

		Invalidate(pe->hOriginalSender);
	}

	return 0;
}

LRESULT CTextChatCanvasHandler::OnSelectedEmoticon(RoutedEventArgs *pe)
{
	CIUIString strGif = (LPCTSTR)pe->lParam;
	strGif.Insert(0, GetProject(m_pReInput->m_hWnd)->GetProjectPathWithoutTitle());

	OleControl *pGifOle = new OleControl(1, m_pReInput->m_hWnd);

	HWLWND hBindGifPanel = CreateXMLWindowChild(NULL, NULL, _T("IDW_EMOTICON_ICON.xml"),
			m_pReInput->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

	HWLWND hStaGif = GetWindow(hBindGifPanel, GW_CHILD);
	Label *pStaGif = (Label *)CWLWnd::FromHandle(hStaGif);

	pStaGif->SetBkImage(CONTROL_STATE_NORMAL, strGif, NULL, FALSE);
	pGifOle->SetOleBindWindowless(hBindGifPanel);

	int nStartPos = strGif.ReverseFind('\\');
	strGif.Delete(0, nStartPos+1);
	int nEndPos = strGif.Find(L".");
	CIUIString strPureName = strGif.Left(nEndPos);
	IUI::SetTag(hBindGifPanel, strPureName);
	m_pReInput->InsertOle(pGifOle, REO_CP_SELECTION, 30, 30);

	pe->bHandled = TRUE;

	return 0;
}

LRESULT CTextChatCanvasHandler::OnPreviewKeyDown(RoutedEventArgs *pe)
{
	HWLWND hFocusCtlr = Window::GetWLFocus(GetHostWnd(GetBindWLWnd()));
	if (m_pReInput->GetSafeHwnd() == hFocusCtlr)
	{
		if ('S' == pe->wParam)
		{
			if (GetKeyState(VK_CONTROL) < 0)
			{
				OnBtnSendMsg(pe);
				pe->bHandled = TRUE;
			}
		}
		if (VK_RETURN == pe->wParam)
		{
			if (GetKeyState(VK_CONTROL)>= 0)
			{
				OnBtnSendMsg(pe);
				pe->bHandled = TRUE;
			}
		}
	}

	return 0;
}

LRESULT CTextChatCanvasHandler::OnLButtonDblClk(RoutedEventArgs *pe)
{
	HWLWND hSender = pe->hOriginalSender;
	if (NULL != hSender)
	{
		OLE_TYPE eOleType = (OLE_TYPE)(int)IUI::GetProp(hSender, GETPROP_OLETYPE);
		if (OT_IMAGE == eOleType)
		{
			CIUIString strImage = GetTag(hSender);
			if (!strImage.IsEmpty())
			{
				_ASSERT(!strImage.IsEmpty());
				ShellExecute(NULL, _T("open"), strImage, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}
void CTextChatCanvasHandler::ReFreshUnReadMsg(vector<CChatRecord> vecChatRecord)
{
	return;
}
void  CTextChatCanvasHandler::InsertMsg(std::string strMsg,bool bSelf, int lMsgID,CIUIString strDisplayName)
{
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	ENUM_RECENT_CONTACT_TYPE enum_LocalContactType = m_pContactInfo->m_enumRecentContact;
	if (enum_LocalContactType==ENUM_MULTI_CONTACT)
	{
		return;
	}
	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

	
	CIMMsg msg7;
	msg7.SetMsgSenderTitle(0);
	msg7.SetMsgID(lMsgID);
	msg7.SetUserData(false);
	CIUIString strFaceImage = L"";
	if (bSelf)
	{
		msg7.SetSelf(TRUE);
		msg7.SetMsgSender(PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str());
		strFaceImage = PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage()).c_str();
		msg7.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
		msg7.SetMsgSenderID(PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str());
	}
	else
	{
		if (!strDisplayName.IsEmpty())
		{
			msg7.SetMsgSender(strDisplayName);
		}
		else
		{
			msg7.SetMsgSender(strSrcUserName);
		}
		msg7.SetMsgSenderID(strSrcSipNumber);
		msg7.SetSelf(FALSE);
		strFaceImage = strSrcHeadImage;
	}
	
	HIUIIMAGE hFace = ImageManager::CreateIUIImage(IUI::GetProject(m_pHistory->GetSafeHwnd()),
		strFaceImage);
	if (hFace != NULL)
		msg7.SetFace(hFace);
	

	vector<EMOJI_INFO> vecEmoji;
	bool bHaveEmoji = HaveEmoji(strMsg, vecEmoji);
	if (bHaveEmoji)
	{
		int nMsgStartPos = 0, nRecvMsgStartPos = 0,nRecvEmojiStartPos = 0;
		auto iterEmoji = vecEmoji.begin();
		EMOJI_INFO theLocalRecvEmojiInfo;
		for (iterEmoji; iterEmoji!=vecEmoji.end();)
		{ 
			if (nRecvMsgStartPos>=strMsg.length())
			{
				break;
			}
			theLocalRecvEmojiInfo = *iterEmoji;
			nRecvEmojiStartPos = theLocalRecvEmojiInfo.nStartPos;
			std::string strUTFImage = "skins\\PortGo\\emoji\\"+theLocalRecvEmojiInfo.strFind+".png";
			if (nRecvEmojiStartPos==nRecvMsgStartPos)
			{
				OleControl *pOleImage = new OleControl(3, m_pHistory->GetSafeHwnd());
				if (NULL == pOleImage)
				{
					return ;
				}

				HWLWND hStaImage = IUI::CreateWindowEx(NULL, 0, IUI_LABEL, NULL, WS_VISIBLE,
					0, 0, 0, 0,
					GetHostWnd(m_pHistory->GetSafeHwnd()), m_pHistory->GetSafeHwnd(),
					_T("IDC_STA_IMAGE"), 0, NULL);

				if (NULL == hStaImage)
				{
					_ASSERT(FALSE);
					return ;
				}
				CIUIString wImageName = PortUtility::Utf82Unicode_ND(strUTFImage).c_str();
				CIUIString strImage = GetRunFolder() + wImageName;// _T("skins\\emoji\\Title_Center.png");

				IUI::SetProp(hStaImage, GETPROP_OLETYPE, (HANDLE)OT_IMAGE);
				IUI::SetTag(hStaImage, strImage);
				CombineBkImage(hStaImage, FALSE);
				SetBkImage(hStaImage, FALSE, CONTROL_STATE_NORMAL, strImage);
				SetBkImageResizeMode(hStaImage, IRM_KEEPRATE);

				pOleImage->SetOleBindWindowless(hStaImage);

				CRichMsgItem item;
				item.m_bOleControl = TRUE;
				item.m_hOleBindWindowless = hStaImage;
				item.m_size = CIUISize(30, 30);
				
				msg7.AddMsgItem(&item);
				int nEmojiLength = theLocalRecvEmojiInfo.nEndPos - nRecvEmojiStartPos;
				nRecvMsgStartPos = nRecvMsgStartPos + nEmojiLength+1;
				iterEmoji++;
			}
			else if (nRecvMsgStartPos<nRecvEmojiStartPos)
			{
				CRichMsgItem item;
		
				int nTextLength = nRecvEmojiStartPos - nRecvMsgStartPos;
				item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg.substr(nRecvMsgStartPos, nTextLength)).c_str(); //PortUtility::string2WString(strMsg).c_str();
				msg7.AddMsgItem(&item);
				nRecvMsgStartPos = nRecvMsgStartPos + nTextLength;
			}
			else
			{
				OutputDebugString(L"no normal\n");
			}
		}

		//the end
		if (nRecvMsgStartPos > nRecvEmojiStartPos)
		{
			int nTextLength = nRecvMsgStartPos - nRecvEmojiStartPos;
			if (nTextLength >= 0)
			{
				CRichMsgItem item;
				item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg.substr(nRecvMsgStartPos, nTextLength)).c_str();
				msg7.AddMsgItem(&item);
				nRecvMsgStartPos = nRecvMsgStartPos + nTextLength;
			}
		}
	}
	else
	{
		CRichMsgItem item;
		
		item.m_strMsgContent = PortUtility::Utf82Unicode_ND(strMsg).c_str(); //PortUtility::string2WString(strMsg).c_str();
		msg7.AddMsgItem(&item);
	}


	m_pHistory->InsertMsg(INSERT_FORWARD, &msg7);
	m_pHistory->SendMessage(EM_SCROLL, SB_BOTTOM, 0);
	
}

void  CTextChatCanvasHandler::InsertMsgAudio(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID,  CIUIString strDisplayName)
{
	if (fProgress > 100 - 0.000001 && fProgress < 100 + 0.00001)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (m_pHistory != NULL)
		{
			CIMMsg theMsg;
			int nIndex = m_pHistory->FindMsg(lMsgID, &theMsg);
			if (nIndex >= 0)
			{
				UpdateMsgFile(theMsg, ENUM_MSG_FILE_AUDIO, fProgress);
			}
			else
			{
				AddMsgFile(srFileName, strDisplayName, lMsgID, ENUM_MSG_FILE_AUDIO);
			}
		}
	}
}
void  CTextChatCanvasHandler::InsertMsgVideo(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID,  CIUIString strDisplayName)
{
	if (fProgress > 100 - 0.000001 && fProgress < 100 + 0.00001)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (m_pHistory != NULL)
		{
			CIMMsg theMsg;
			int nIndex = m_pHistory->FindMsg(lMsgID, &theMsg);
			if (nIndex >= 0)
			{
				UpdateMsgFile(theMsg, ENUM_MSG_FILE_VIDEO, fProgress);
			}
			else
			{
				AddMsgFile(srFileName, strDisplayName, lMsgID, ENUM_MSG_FILE_VIDEO);
			}
		}
	}
	
}

void CTextChatCanvasHandler::InsertMsgPic(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName)
{
	if (fProgress > 100 - 0.000001 && fProgress < 100 + 0.00001)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (m_pHistory!=NULL)
		{
			CIMMsg theMsg;
			int nIndex = m_pHistory->FindMsg(lMsgID, &theMsg);
			if (nIndex>=0)
			{
				UpdateMsgFile(theMsg, ENUM_MSG_FILE_PIC, fProgress);
			}
			else
			{
				AddMsgFile(srFileName,strDisplayName,lMsgID,ENUM_MSG_FILE_PIC);
			}
		}
	}
}

void CTextChatCanvasHandler::UpdateMsgFile(CIMMsg theMsg,ENUM_MSG_TYPE enumMsgType,float fProgress)
{
	auto vecRichMsgItem = theMsg.GetMsgItems();
	auto iterRichMsgItem = vecRichMsgItem->begin();
	for (iterRichMsgItem; iterRichMsgItem != vecRichMsgItem->end(); iterRichMsgItem++)
	{
		BOOL bOleCtrl = iterRichMsgItem->m_bOleControl;
		HWLWND hOleHwnd = iterRichMsgItem->m_hOleBindWindowless;

		if (hOleHwnd != NULL && bOleCtrl == TRUE)
		{
			switch (enumMsgType)
			{
			case ENUM_MSG_FILE_PIC:
			{
				UpdatePicOleProgress(hOleHwnd, fProgress);
			}
			
				break;
			case ENUM_MSG_FILE:
			{
				
				UpdateFileOleProgress(hOleHwnd, fProgress);
				
			}
				break;
			
			case ENUM_MSG_FILE_AUDIO:
			{
				UpdateAudioOleProgresss(hOleHwnd, fProgress);
			}
				
				break;
			case ENUM_MSG_FILE_VIDEO:
				UpdateVideoOleProgress(hOleHwnd, fProgress);
				break;
			default:
				break;
			}
			
		}
	}
}

void CTextChatCanvasHandler::AddMsgFile(std::string srFileName,CIUIString strDisplayName,long lMsgID, ENUM_MSG_TYPE enumMsgType)
{
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	std::wstring strFilePath = CSystemEx::GetInstance()->GetFileSavePathByMsgType(enumMsgType);
	std::wstring strWideFileName = PortUtility::string2WString(srFileName);
	std::wstring strFullFilePath = strFilePath + strWideFileName;

	CIUIString strSrcHeadImage, strSrcUserName, strSrcSipNumber;
	FormatItemContact(strSrcUserName, strSrcHeadImage, strSrcSipNumber, pContactDB);

	CIUIString strSenderID;
	bool bIsMySelf = false;
	bool bFind = false;
	CChatRecord theMsg = CSystemEx::GetInstance()->GetDBOprate()->searchRecordByMsgID(lMsgID, bFind);

	if (bFind == true)
	{
		bIsMySelf = theMsg.m_nIsMyself;
	}
	if (bIsMySelf == false && !strDisplayName.IsEmpty())
	{
		strSrcUserName = strDisplayName;
	}
	
	if (bIsMySelf)
	{
		strSenderID = PortUtility::Utf82Unicode_ND(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber).c_str();
		std::string strFaceImage = CSystemEx::GetInstance()->GetGlobalUser()->GetFaceImage();
		strSrcHeadImage = PortUtility::Utf82Unicode_ND(strFaceImage).c_str();
	}
	else
	{
		strSenderID = strSrcSipNumber;
	}
	wstring wstrSender = strSrcUserName;
	HWLWND hOleHwnd = NULL;

	switch (enumMsgType)
	{
	case ENUM_MSG_FILE_PIC:
		InsertHistoryImageOle(m_pHistory->GetSafeHwnd(), bIsMySelf, strFilePath.c_str(), strWideFileName.c_str(), hOleHwnd, wstrSender, lMsgID, INSERT_FORWARD, strSrcHeadImage, strSenderID);
	case ENUM_MSG_FILE:
		InsertHistoryFileOle(m_pHistory->GetSafeHwnd(), bIsMySelf, strWideFileName.c_str(), strFilePath.c_str(), hOleHwnd, wstrSender, lMsgID, INSERT_FORWARD, strSrcHeadImage, strSenderID);
		break;

	case ENUM_MSG_FILE_AUDIO:
		InsertHistoryAudioOle(m_pHistory->GetSafeHwnd(), bIsMySelf, strWideFileName.c_str(), strFilePath.c_str(), L"00:00", hOleHwnd, wstrSender, lMsgID, INSERT_FORWARD, strSrcHeadImage, strSenderID);
		break;
	case ENUM_MSG_FILE_VIDEO:
	{
		void *pNewVideoOleWnd = NULL;
		InsertHistoryVideoOle(m_pHistory->GetSafeHwnd(), bIsMySelf, strWideFileName.c_str(), strFilePath.c_str(), L"00:00", hOleHwnd, &pNewVideoOleWnd, wstrSender, lMsgID, INSERT_FORWARD, strSrcHeadImage, strSenderID);
	}
		break;
	default:
		break;
	}
	
}
void CTextChatCanvasHandler::UpdateContactInfo()
{
	Invalidate(m_pHistory->GetSafeHwnd());
}
void CTextChatCanvasHandler::DownLoadFileOK(long nMsgID,bool bComplete,std::string strFileName, ENUM_MSG_TYPE  enumFileType, CIUIString strDisplayName, IUI::HWLWND hWnd)
{
	if (bComplete==false)
	{
		//todo 
#if  _DEBUG

		CIUIString strInfo;
		strInfo.Format(L"%ld down load failed", nMsgID);
		OutputDebugString(strInfo);
#endif
		m_pHistory->ShowResendButton(nMsgID, (BOOL)TRUE);
	}
	if (enumFileType==ENUM_MSG_FILE_VIDEO)
	{
		if (hWnd != NULL)
		{
			COleVideoCanvasHandler *pWndVideo = (COleVideoCanvasHandler*)CWLWnd::FromHandle(hWnd)->m_hWnd->m_pUserBindWLWndHandler;
			if (pWndVideo != NULL)
			{
				pWndVideo->ResetVideoThumbnailImage();
			}
		}
	}
	Invalidate(m_pHistory->GetSafeHwnd());
}
void CTextChatCanvasHandler::ProcessDownLoadFileFailed(int nMsgID)
{
#if  _DEBUG

	CIUIString strInfo;
	strInfo.Format(L"%ld down load failedProcessDownLoadFileFailed", nMsgID);
	OutputDebugString(strInfo);
#endif
	m_pHistory->ShowResendButton(nMsgID, (BOOL)TRUE);
	Invalidate(m_pHistory->GetSafeHwnd());
}
void CTextChatCanvasHandler::InsertMsgFile(std::string srFileName, float fProgress, int lMsgID, CIUIString strDisplayName)
{
	if (fProgress > 100 - 0.000001 && fProgress < 100 + 0.00001)
	{
		CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
		if (m_pHistory != NULL)
		{
			CIMMsg theMsg;
			int nIndex = m_pHistory->FindMsg(lMsgID, &theMsg);
			if (nIndex >= 0)
			{
				UpdateMsgFile(theMsg, ENUM_MSG_FILE, fProgress);
			}
			else
			{
				AddMsgFile(srFileName, strDisplayName, lMsgID, ENUM_MSG_FILE);
			}
		}
	}
	
}
void CTextChatCanvasHandler::InsertMsgEmoji(std::string srFileName, std::string strDateTime, CIUIString strDisplayName)
{
}
void CTextChatCanvasHandler::InsertMsgOther(std::string srFileName, std::string strDateTime, CIUIString strDisplayName)
{
}

void CTextChatCanvasHandler::UpdateDownLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
}
void CTextChatCanvasHandler::UpdateDownLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
}
void CTextChatCanvasHandler::UpdateDownLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
}
void CTextChatCanvasHandler::UpdateDownLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
}

void CTextChatCanvasHandler::UpdateUpLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
	if (pDownLoadParam->m_hWnd != NULL && pDownLoadParam != NULL)
	{
		ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(pDownLoadParam->m_hWnd, _T("IDC_PRG_UPLOAD")));
		if (pProgressBar != NULL)
		{
			pProgressBar->SetPos(pDownLoadParam->m_fProcessData);
			if (bComplete == true)
			{
				pProgressBar->ShowWindow(SW_HIDE);
			}
		}
	}
}
void CTextChatCanvasHandler::UpdateUpLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
	if (pDownLoadParam->m_hWnd!=NULL&&pDownLoadParam!=NULL)
	{
		ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(pDownLoadParam->m_hWnd, _T("IDC_PRG_UPLOAD")));
		if (pProgressBar!=NULL)
		{
			pProgressBar->SetPos(pDownLoadParam->m_fProcessData);
			if (bComplete==true)
			{
				pProgressBar->ShowWindow(SW_HIDE);
			}
		}
	}
	
	if (bComplete==false)
	{
		return;
	}
}
void CTextChatCanvasHandler::OnUpdateFileOK(UpLoadDownLoadParam* pDownLoadParam)
{
	std::string strSendMsg = "";
	int nDuration = 0;
	std::string strFilePath;
	CIUIString strPlayTime;
	switch (pDownLoadParam->m_enumFileType)
	{
	case ENUM_MSG_FILE_PIC:
		strSendMsg = CSystemEx::GetInstance()->ConstructImageMessage((pDownLoadParam)->m_strFileName, (pDownLoadParam)->m_strHttpUrl, pDownLoadParam->m_strMediaType, (pDownLoadParam)->m_lFileSize,0,0);
		break;
	case ENUM_MSG_FILE:
		
		strSendMsg = CSystemEx::GetInstance()->ConstructFileMessage((pDownLoadParam)->m_strFileName, (pDownLoadParam)->m_strHttpUrl, pDownLoadParam->m_strMediaType, (pDownLoadParam)->m_lFileSize);
		break;
	case ENUM_MSG_FILE_AUDIO:
	{
		strFilePath = (pDownLoadParam)->m_strFilePath + (pDownLoadParam)->m_strFileName;
		strPlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFilePath, nDuration);
		strSendMsg = CSystemEx::GetInstance()->ConstructAudioMessage((pDownLoadParam)->m_strFileName, (pDownLoadParam)->m_strHttpUrl, pDownLoadParam->m_strMediaType, (pDownLoadParam)->m_lFileSize, nDuration);
	}
		break;
	case ENUM_MSG_FILE_VIDEO:
	{
		strFilePath = (pDownLoadParam)->m_strFilePath + (pDownLoadParam)->m_strFileName;
		strPlayTime= CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFilePath, nDuration);
		strSendMsg = CSystemEx::GetInstance()->ConstructVideoMessage((pDownLoadParam)->m_strFileName, (pDownLoadParam)->m_strHttpUrl, pDownLoadParam->m_strMediaType, (pDownLoadParam)->m_lFileSize, nDuration);
	}
		break;
	default:
		break;
	}
	CSystemEx::GetInstance()->GetDBOprate()->UpdateRecordFileStatus(pDownLoadParam->m_lMsgID, ENUM_OK);
	if (strSendMsg.empty())
	{
		return;
	}

	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	long lRet = Send(strSendMsg,ENUM_TYPE_JSON);
	std::wstring wstrSendMsg = PortUtility::string2WString(strSendMsg);
	//TODO SAVE RECORD
	std::stringstream ssStrMsgID;
	ssStrMsgID<<lRet;

	if (lRet == -1)
	{
		return ;
	}
	if (lRet>0)
	{
		CChatRecord theUpdateInfo;
		theUpdateInfo.m_strMSG = strSendMsg;
		theUpdateInfo.m_strPlayTime = PortUtility::Unicode2Utf8_ND(strPlayTime.GetBuffer(strPlayTime.GetLength()));
		theUpdateInfo.m_strLocalSignMsgID = ssStrMsgID.str();
		//update msg todo
		CSystemEx::GetInstance()->GetDBOprate()->UpdateRecordInfo(pDownLoadParam->m_lMsgID, theUpdateInfo);
	}
}
void CTextChatCanvasHandler::UpdateUpLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
	if (pDownLoadParam->m_hWnd != NULL && pDownLoadParam != NULL)
	{
		ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(pDownLoadParam->m_hWnd, _T("IDC_PRG_IMAGE_OLE")));
		if (pProgressBar != NULL)
		{
			pProgressBar->SetPos(pDownLoadParam->m_fProcessData);
			if (bComplete == true)
			{
				pProgressBar->ShowWindow(SW_HIDE);
			}
		}
	}
	
	
}
LRESULT CTextChatCanvasHandler::OnBtnMsgHistory(RoutedEventArgs *pe)
{
	if (GroupEnableFun() == true)
	{
		return 0;
	}
	//mark CONTACT NULL
	CContactDB *pContact =NULL;

	PopupMsgHistoryWnd(pContact, GetHostWnd(GetBindWLWnd()));

	return 0;
}

void CTextChatCanvasHandler::UpdateUpLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam)
{
	bool bComplete = CheckProgressIsComplete((pDownLoadParam)->m_fProcessData);
	if (pDownLoadParam->m_hWnd != NULL && pDownLoadParam != NULL)
	{
		ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(pDownLoadParam->m_hWnd, _T("IDC_PRG_UPLOAD")));
		if (pProgressBar != NULL)
		{
			pProgressBar->SetPos(pDownLoadParam->m_fProcessData);
			if (bComplete == true)
			{
				pProgressBar->ShowWindow(SW_HIDE);
			}
		}
	}
	
}
bool CTextChatCanvasHandler::CheckProgressIsComplete(double dProgress)
{
	if (dProgress <0)
	{
		return false;
	}
	if (dProgress > 100 - 0.0000001 && dProgress < 100 + 0.000001)
	{
		return true;
	}
	return false;
}
void CTextChatCanvasHandler::SetContactInfo(TAG_RECENT *pContactInfo)
{
	m_pContactInfo = pContactInfo;
};

void CTextChatCanvasHandler::Add2UploadList(CIUIString strIUIFilePath, CIUIString strIUIFileName,const char *fileType, ENUM_MSG_TYPE enumFileType,HWLWND hWnd,int nMsgID)
{
	if (m_pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
	{
		return;
	}
	std::wstring  strFileName = L"", strFileFullPath = L"";
	UpLoadDownLoadParam *pUpLoadParam = new UpLoadDownLoadParam;
	strFileFullPath = strIUIFilePath.GetBuffer(strIUIFilePath.GetLength());
	pUpLoadParam->m_strFilePath = PortUtility::wstring2String(strFileFullPath);
	PortUtility::CheckFilePath(pUpLoadParam->m_strFilePath);
	strFileName = strIUIFileName.GetBuffer(strIUIFileName.GetLength());
	pUpLoadParam->m_strFileName = PortUtility::wstring2String(strFileName);
	pUpLoadParam->m_pContact = m_pContactInfo;
	pUpLoadParam->m_strMediaType = fileType;
	pUpLoadParam->m_hWnd = hWnd;
	pUpLoadParam->m_enumFileType = enumFileType;
	pUpLoadParam->m_lMsgID = nMsgID;
	pUpLoadParam->m_hWndMsgProcess = CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd();
	CBaseThread				FileUpLoadThread;
	FileUpLoadThread.BeginThread(AddUpLoadThread, pUpLoadParam); 
}

void	CTextChatCanvasHandler::UpdateFileOleProgress(HWLWND hWndOle, float fProgress)
{
	ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(hWndOle, _T("IDC_PRG_UPLOAD")));
	if (pProgressBar!=NULL)
	{
		pProgressBar->SetPos((int)fProgress);
	}
	if (CheckProgressIsComplete(fProgress) == true)
	{
		pProgressBar->ShowWindow(SW_HIDE);
	}
	else
	{
		pProgressBar->ShowWindow(SW_SHOW);
	}
}
void    CTextChatCanvasHandler::UpdatePicOleProgress(HWLWND hWndOle, float fProgress)
{
	ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(hWndOle, _T("IDC_PRG_IMAGE_OLE")));
	if (pProgressBar != NULL)
	{
		pProgressBar->SetPos((int)fProgress);
	}
	else
	{
		return;
	}
	if (CheckProgressIsComplete(fProgress) == true)
	{
		pProgressBar->ShowWindow(SW_HIDE);
	}
	else
	{
		pProgressBar->ShowWindow(SW_SHOW);
	}
}
void	CTextChatCanvasHandler::UpdateVideoOleProgress(HWLWND hWndOle,/* void *pVideoCanvasHandler,*/ float fProgress)
{
	ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(hWndOle, _T("IDC_PRG_UPLOAD")));
	
	//COleVideoCanvasHandler *pWndVideo = (COleVideoCanvasHandler*)pVideoCanvasHandler;
	
	if (pProgressBar != NULL)
	{
		pProgressBar->SetPos((int)fProgress);
	}
	
	if (CheckProgressIsComplete(fProgress) == true)
	{
		if (pProgressBar!=NULL)
		{
			pProgressBar->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (pProgressBar != NULL)
		{
			pProgressBar->ShowWindow(SW_SHOW);
		}
	}
}
void	CTextChatCanvasHandler::UpdateAudioOleProgresss(HWLWND hWndOle, float fProgress)
{
	ProgressBar *pProgressBar = (ProgressBar *)CWLWnd::FromHandle(FindControl(hWndOle, _T("IDC_SLD_PROGRESS")));
	Label *pLabelPlayTime = (Label *)CWLWnd::FromHandle(FindControl(hWndOle, _T("IDC_STA_DURATION")));
	if (pProgressBar != NULL)
	{
		pProgressBar->SetPos((int)fProgress);
	}
	if (CheckProgressIsComplete(fProgress) == true)
	{
		pProgressBar->ShowWindow(SW_SHOW);
	}
	else
	{
		pProgressBar->ShowWindow(SW_HIDE);
	}
}
LRESULT CTextChatCanvasHandler::OnIMDrawItem(RoutedEventArgs *pe)
{
	DRAWITEMSTRUCT *pDrawItem = (DRAWITEMSTRUCT *)pe->lParam;
	if (NULL != pDrawItem)
	{
		if (ODT_IMFACE == pDrawItem->CtlType)
		{
			CIMMsg *pMsg = (CIMMsg *)pDrawItem->itemData;
			if (NULL != pMsg)
			{
				DrawFace(pDrawItem->hDC, &pDrawItem->rcItem, pMsg->GetFace()->GetSafeImageName());

				HIUIFONT hDefFont = GetProject(GetBindWLWnd())->GetFontMgr()->GetDefaultFont();
				_ASSERT(hDefFont != NULL);
				bool bSetUserFaceImage =(bool) pMsg->GetUserData();

				CIUIString strSenderTitle = pMsg->GetMsgSender();
				if (!strSenderTitle.IsEmpty())
				{
					
					DrawFormatText(pDrawItem->hDC, strSenderTitle, 1, &pDrawItem->rcItem,
						CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont->GetSafeHFont(), RGB(30, 30, 30), IsUpdateLayeredWindow(GetHostWnd(pe->hOriginalSender)));

				}
				else
				{
					printf("");
				}
				pe->bHandled = TRUE;
				return 1;
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

	return 0;
}

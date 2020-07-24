#include "stdafx.h"
#include "DynamicText.h"
#include "Utility/utility.hxx"

CIUIString szLanguage[]= {
	L"EN",L"CN",L"IT",L"SP",L"FR",L"RU",L"PO"
};

CDynamicText::CDynamicText( )
{
	m_strAppName = L"";

	m_strFileFullPath = "";
	m_senderTitleSelf = L"";
	m_senderTitleOther = L"";
	m_strTipsDateOut = L"";
	m_strTipsNoRecordPath = L"";
	m_strTipsSaveDstPath = L"";
	m_strClassicEmotion = L"";
	m_strSymbolEmotion = L"";
	m_strBeginRecord = L"";
	m_strWaitingAnswerPre = L"";
	m_strWaitingAnswerEnd = L"";
	m_strTransferPre = L"";
	
	m_strAddGroupPre = L"";
	m_strAddGroupEnd = L"";
	m_strJoinGroupEnd = L"";

	m_strStartTransferPre = L"";
	m_strStartTransferEnd = L"";
	 m_strProfile = L"";
	 m_strAppSettings = L"";
	 m_strAddContact = L"";
	 m_strOnLine = L"";
	 m_strOffLine = L"";
	 m_strAway = L"";
	 m_strBusy = L"";
	 m_strDoNotDisturb = L"";

	 m_strForward = L"";
	 m_strDeleteThisContact = L"";
	 m_strAddToLocalContact = L"";
	 m_strEditLocalContact = L"";
	 m_strAddToFavoriteContact = L"";
	 m_strDeleteThisRecord = L"";
	 m_strCancelFavorite = L"";

	 m_strShowMain = L"";
	 m_strAbout = L"";
	 m_strExit = L"";

	 m_strAccount = L"";
	 m_strPassword = L"";
	 m_strSipDomain = L"";

	  m_strApplication = L"";
	  m_strAudioCodecs = L"";
	  m_strVideoCodecs = L"";
	  m_strMediaQuality = L"";
	  m_strMisc = L"";
	  m_strLanguages = L"";

	  m_strMoreMsg = L"";

	  m_strParticipants = L"";
	  m_strMuteMeeting = L"";
	  m_strUnMuteMetting = L"";
	  m_strMore = L"";
	  m_strOffLineCanNotDo = L"";
	  m_strConfrenceNum = L"";
	 
	  m_strConfrenceMemberMax = L"";
	  m_strSignOut = L"";

	  m_strEventCallFailed = L"";
	  m_strEventCallAudioType = L"";
	  m_strEventCallVideoType = L"";
	  m_strEventCallSuccess = L"";
	  m_strEventCallMissedAudio = L"";
	  m_strEventCallMissedVideo = L"";
	  m_strCallOut = L"";
	  m_strCallIn = L"";
}

CDynamicText::~CDynamicText()
{
}
CIUIString CDynamicText::GetSenderTitle0()
{
	return m_senderTitleOther;
}
CIUIString CDynamicText::GetSenderTitle1()
{
	return m_senderTitleSelf;
}
CIUIString CDynamicText::GetTipsDateOut()
{
	return m_strTipsDateOut;
}
CIUIString CDynamicText::GetTipsNoRecordPath()
{
	return m_strTipsNoRecordPath;
}
CIUIString CDynamicText::GetTipsSaveDstPath()
{
	return m_strTipsSaveDstPath;
}
CIUIString CDynamicText::GetClassicEmotion()
{
	return m_strClassicEmotion;
}
CIUIString CDynamicText::GetSymbolEmotion()
{
	return m_strSymbolEmotion;
}
CIUIString CDynamicText::GetBeginRecord()
{
	return m_strBeginRecord;
}
CIUIString CDynamicText::GetWaitingAnswerPre()
{
	return m_strWaitingAnswerPre;
}
CIUIString CDynamicText::GetWaitingAnswerEnd()
{
	return m_strWaitingAnswerEnd;
}
CIUIString CDynamicText::GetTransferPre()
{
	return m_strTransferPre;
}

CIUIString CDynamicText::GetAddGroupPre()
{
	return m_strAddGroupPre;
}
CIUIString CDynamicText::GetAddGroupEnd()
{
	return m_strAddGroupEnd;
}
CIUIString CDynamicText::GetJoinGroupPre()
{
	return m_strAddGroupPre;
}
CIUIString CDynamicText::GetJoinGroupEnd()
{
	return m_strJoinGroupEnd;
}
CIUIString CDynamicText::GetStartTransferPre()
{
	return m_strStartTransferPre;
}

CIUIString CDynamicText::GetStartTransferEnd()
{
	return m_strStartTransferEnd;
}

CIUIString CDynamicText::GetAPPStringName()
{
	return m_strAppName;
}


bool CDynamicText::LoadDynamicText(const char* szFileName, ENUM_LANGUAGE enumLanguage)
{
	if (strcmp(szFileName,"")==0)
	{
		LOG4_ERROR(L"otto file name is empty");
		return false;
	}
	std::string strFileName;
	std::stringstream strFileIn;
	strFileIn<<"DB slecet "<<enumLanguage;
	LOG4_INFO(strFileIn.str().c_str());
	bool bRet = false;

	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);

	std::string strFilePath;
	strFilePath = strFullPath + "/" + szFileName;
	OutputDebugString(PortUtility::Utf82Unicode_ND(strFilePath).c_str());
	OutputDebugString(L"\n");

	m_strFileFullPath = strFilePath;
	m_enumLanguage = enumLanguage;

	TCHAR szAppName[MAX_PATH] = {0};
#ifdef BZXX
	::GetPrivateProfileString(L"APPNAME",L"BZXX", L"PortGo", szAppName, sizeof(szAppName), PortUtility::Utf82Unicode_ND(strFilePath).c_str());
#endif

#ifdef KCXX
	::GetPrivateProfileString(L"APPNAME", L"KCXX", L"PortGo", szAppName, sizeof(szAppName), PortUtility::Utf82Unicode_ND(strFilePath).c_str());
#endif
	
	m_strAppName =szAppName;
	if (m_strAppName.IsEmpty())
	{
		LOG4_ERROR(L"CFG Lauguages.otto ERROR");
	}
	else
	{
		bRet = true;
	}
	m_senderTitleOther = LoadLanguageKeyValue(L"SenderTitle", L"SenderTitle1", L"PortGo");
	m_senderTitleSelf = LoadLanguageKeyValue(L"SenderTitle", L"SenderTitle0", L"PortGo");
	m_strTipsDateOut = LoadLanguageKeyValue(L"Tips", L"TipsDateOut", L"The usage time has expired. \
		Please contact the administrator and the system will exit automatically");
	m_strTipsNoRecordPath = LoadLanguageKeyValue(L"Tips", L"TipsNoRecordPath", L"Recording or recording saving path is not set, please set \
		My>>App Settings>>Application>>File Location>>Browser");
	m_strTipsSaveDstPath = LoadLanguageKeyValue(L"Tips", L"TipsSaveDstPath", L"Select Save folder");
	m_strClassicEmotion = LoadLanguageKeyValue(L"Tips", L"ClassicEmotion", L"Classic Emotion");
	m_strSymbolEmotion = LoadLanguageKeyValue(L"Tips", L"SymbolEmotion", L"Symbol Emotion");
	m_strBeginRecord = LoadLanguageKeyValue(L"Tips", L"BeginRecord", L"Record Started");
	m_strWaitingAnswerPre = LoadLanguageKeyValue(L"Tips", L"WaitingAnswerPre", L"wait");
	m_strWaitingAnswerEnd = LoadLanguageKeyValue(L"Tips", L"WaitingAnswerEnd", L"answer the call");
	m_strTransferPre = LoadLanguageKeyValue(L"Tips", L"TransferPre", L"The connect is available,transfer the call to");
	m_strAddGroupPre = LoadLanguageKeyValue(L"Tips", L"AddGroupPre", L"Add");
	m_strAddGroupEnd = LoadLanguageKeyValue(L"Tips", L"AddGroupEnd", L"to this call");
	m_strJoinGroupEnd   = LoadLanguageKeyValue(L"Tips", L"JoinGroupEnd", L"join in this call");
	m_strStartTransferPre = LoadLanguageKeyValue(L"Tips", L"StartTransferPre", L"Transfer");
	m_strStartTransferEnd = LoadLanguageKeyValue(L"Tips", L"StartTransferEnd", L"join in this call");
	m_strProfile = LoadLanguageKeyValue(L"ProfileMenu", L"Profile", L"Profile");
	m_strAppSettings = LoadLanguageKeyValue(L"ProfileMenu", L"AppSettings", L"App Setting");
	m_strAddContact = LoadLanguageKeyValue(L"ProfileMenu", L"AddContact", L"Add Contact");
	m_strOnLine = LoadLanguageKeyValue(L"ProfileMenu", L"OnLine", L"OnLine");
	m_strOffLine = LoadLanguageKeyValue(L"ProfileMenu", L"OffLine", L"OffLine");
	m_strAway = LoadLanguageKeyValue(L"ProfileMenu", L"Away", L"Away");
	m_strBusy = LoadLanguageKeyValue(L"ProfileMenu", L"Busy", L"Busy");
	m_strDoNotDisturb = LoadLanguageKeyValue(L"ProfileMenu", L"DoNotDisturb", L"Do not disturb");
	m_strForward = LoadLanguageKeyValue(L"ContactMenu", L"Forward", L"Forward");
	m_strDeleteThisContact = LoadLanguageKeyValue(L"ContactMenu", L"DeleteThisContact", L"Delete this contact");
	m_strAddToLocalContact = LoadLanguageKeyValue(L"ContactMenu", L"AddToFavoriteContact", L"Add to favorite contacts");
	m_strEditLocalContact = LoadLanguageKeyValue(L"ContactMenu", L"EditLocalContact", L"Edit local contact");
	m_strAddToFavoriteContact = LoadLanguageKeyValue(L"ContactMenu", L"AddToFavoriteContact", L"Add to favorite contacts");
	m_strDeleteThisRecord = LoadLanguageKeyValue(L"ContactMenu", L"DeleteThisRecord", L"Delete this  record");
	m_strCancelFavorite = LoadLanguageKeyValue(L"ContactMenu", L" CancelFavorite", L"Cancel Favorite");
	m_strShowMain = LoadLanguageKeyValue(L"ToolBarMenu", L" ShowMain", L"Show Client");
	m_strAbout = LoadLanguageKeyValue(L"ToolBarMenu", L" About", L"About");
	m_strExit = LoadLanguageKeyValue(L"ToolBarMenu", L" Exit", L"Exit");
	m_strAccount = LoadLanguageKeyValue(L"EditText", L" Account", L"Account");
	m_strPassword = LoadLanguageKeyValue(L"EditText", L"Password", L"Password");
	m_strSipDomain = LoadLanguageKeyValue(L"EditText", L"SipDomain", L"Sip Domain");
	m_strApplication = LoadLanguageKeyValue(L"SetingsTreeItem", L"Application", L"Application");
	m_strAudioCodecs = LoadLanguageKeyValue(L"SetingsTreeItem", L"AudioCodecs", L"Audio Codecs");
	m_strVideoCodecs = LoadLanguageKeyValue(L"SetingsTreeItem", L"VideoCodecs", L"Video Codecs");
	m_strMediaQuality = LoadLanguageKeyValue(L"SetingsTreeItem", L"MediaQuality", L"Media Quality");
	m_strMisc = LoadLanguageKeyValue(L"SetingsTreeItem", L"Misc", L"Misc");
	m_strLanguages = LoadLanguageKeyValue(L"SetingsTreeItem", L"Languages", L"Languages");;
	m_strMoreMsg= LoadLanguageKeyValue(L"Tips", L"MoreMsg", L"more message");
	m_strOffLineCanNotDo = LoadLanguageKeyValue(L"Tips", L"OffLineCanNotDo", L"This operation cannot be performed currently because client is offline");
	m_strSignOut = LoadLanguageKeyValue(L"ContactMenu", L"SignOut", L"Sign Out");
	m_strParticipants = LoadLanguageKeyValue(L"Participant", L"Participants", L"Participants");;
	m_strMuteMeeting = LoadLanguageKeyValue(L"Participant", L"MuteMeeting", L"Mute meeting");;
	m_strUnMuteMetting = LoadLanguageKeyValue(L"Participant", L"UnMuteMetting", L"Un-mute metting");;
	m_strMore = LoadLanguageKeyValue(L"Participant", L"More", L"More");;
	m_strConfrenceNum = LoadLanguageKeyValue(L"Tips", L"LocalConfrenceNum", L"Only one local confrence is  allowed to be created");
	m_strConfrenceMemberMax = LoadLanguageKeyValue(L"Tips", L"LocalConfrenceMemberMax", L"Up to six  members will be able to added");

	m_strEventCallFailed = LoadLanguageKeyValue(L"Tips", L"EventCallFailed", L"call failed");
	m_strEventCallAudioType = LoadLanguageKeyValue(L"Tips", L"EventCallAudioType", L"Audio");
	m_strEventCallVideoType = LoadLanguageKeyValue(L"Tips", L"EventCallVideoType", L"Video");
	m_strEventCallSuccess = LoadLanguageKeyValue(L"Tips", L"EventCallSuccess", L"Talk time:");
	m_strEventCallMissedAudio = LoadLanguageKeyValue(L"Tips", L"EventCallMissedAudio", L"Missed audio call");
	m_strEventCallMissedVideo = LoadLanguageKeyValue(L"Tips", L"EventCallMissedVideo", L"Missed video call");

	m_strCallOut = LoadLanguageKeyValue(L"Tips", L"CallOut", L"inbound");
	m_strCallIn = LoadLanguageKeyValue(L"Tips", L"CallIn", L"outbound");
	return bRet;
}

CIUIString CDynamicText::LoadLanguageKeyValue(CIUIString strKey, CIUIString strSubKey, CIUIString strDefaultValue)
{
	CIUIString strRet;
	CIUIString strTemplateKey = strSubKey+ szLanguage[m_enumLanguage];
	TCHAR strValue[MAX_PATH] = { 0 };
	::GetPrivateProfileStringW(strKey, strTemplateKey, strDefaultValue, strValue, sizeof(strValue),\
		PortUtility::Utf82Unicode_ND(m_strFileFullPath).c_str());
	return strValue;
}
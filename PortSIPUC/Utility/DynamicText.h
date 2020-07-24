#pragma once
class CDynamicText
{
public:

	CDynamicText();
	~CDynamicText();

	bool LoadDynamicText(const char* szFileName, ENUM_LANGUAGE enumLanguage);

	CIUIString GetAPPStringName();

	CIUIString GetSenderTitle0();
	CIUIString GetSenderTitle1();
	CIUIString GetTipsDateOut();
	CIUIString GetTipsNoRecordPath();
	CIUIString GetTipsSaveDstPath();
	CIUIString GetClassicEmotion();
	CIUIString GetSymbolEmotion();
	CIUIString GetBeginRecord();
	CIUIString GetWaitingAnswerPre();
	CIUIString GetWaitingAnswerEnd();
	CIUIString GetTransferPre();
	CIUIString GetAddGroupPre();
	CIUIString GetAddGroupEnd();
	CIUIString GetJoinGroupPre();
	CIUIString GetJoinGroupEnd();
	CIUIString GetStartTransferPre();
	CIUIString GetStartTransferEnd();

	CIUIString GetProfile()
	{
		return m_strProfile;
	}
	CIUIString GetAppSettings()
	{
		return m_strAppSettings;
	}
	CIUIString GetAddContact()
	{
		return m_strAddContact;
	}
	CIUIString GetOnLine()
	{
		return m_strOnLine;
	}
	CIUIString GetOffLine()
	{
		return  m_strOffLine;
	}
	CIUIString  GetAway()
	{
		return  m_strAway;
	}
	CIUIString  GetBusy()
	{
		return  m_strBusy;
	}
	CIUIString GetDoNotDisturb()
	{
		return  m_strDoNotDisturb;
	}

	CIUIString GetForward()
	{
		return  m_strForward;
	}
	CIUIString GetDeleteThisContact()
	{
		return  m_strDeleteThisContact;
	}
	CIUIString GetAddToLocalContact()
	{
		return  m_strAddToLocalContact;
	}
	CIUIString GetEditLocalContact()
	{
		return  m_strEditLocalContact;
	}
	CIUIString  GetAddToFavoriteContact()
	{
		return  m_strAddToFavoriteContact;
	}
	CIUIString  GetDeleteThisRecord()
	{
		return  m_strDeleteThisRecord;
	}
	CIUIString  GetCancelFavorite()
	{
		return  m_strCancelFavorite;
	}

	CIUIString  GetShowMain()
	{
		return  m_strShowMain;
	}
	CIUIString   GetAbout()
	{
		return  m_strAbout;
	}

	CIUIString    GetExit()
	{
		return  m_strExit;
	}

	CIUIString   GetAccount()
	{
		return  m_strAccount;
	}
	CIUIString   GetPassword()
	{
		return  m_strPassword;
	}
	CIUIString   GetSipDomain()
	{
		return  m_strSipDomain;
	}

	CIUIString   GetApplication()
	{
		return  m_strApplication;
	}
	CIUIString   GetAudioCodecs()
	{
		return  m_strAudioCodecs;
	}
	CIUIString   GetVideoCodecs()
	{
		return  m_strVideoCodecs;
	}
	CIUIString   GetMediaQuality()
	{
		return  m_strMediaQuality;
	}
	CIUIString   GetMisc()
	{
		return  m_strMisc;
	}
	CIUIString   GetLanguages()
	{
		return  m_strLanguages;
	}

	CIUIString GetMoreMsg()
	{
		return m_strMoreMsg;
	}
	CIUIString   GetParticipants()
	{
		return  m_strParticipants;
	}
	CIUIString   GetMuteMeeting()
	{
		return  m_strMuteMeeting;
	}

	CIUIString GetUnMuteMetting()
	{
		return m_strUnMuteMetting;
	}

	CIUIString GetParticipantsMore()
	{
		return m_strMore;
	}

	CIUIString GetOffLineCanNotDo()
	{
		return m_strOffLineCanNotDo;
	}
	CIUIString GetSignOut()
	{
		return m_strSignOut;
	}
	CIUIString GetConfrenceNum()
	{
		return m_strConfrenceNum;
	}

	CIUIString GetConfrenceMemberMax()
	{
		return m_strConfrenceMemberMax;
	}

	CIUIString GetEventCallFailed()
	{
		return m_strEventCallFailed;
	}
	CIUIString GetEventCallAudioType()
	{
		return m_strEventCallAudioType;
	}
	CIUIString GetEventCallVideoType()
	{
		return m_strEventCallVideoType;
	}
	CIUIString GetEventCallSuccess()
	{
		return m_strEventCallSuccess;
	}
	CIUIString GetEventCallMissedAudio()
	{
		return m_strEventCallMissedAudio;
	}
	CIUIString GetEventCallMissedVideo()
	{
		return m_strEventCallMissedVideo;
	}

	CIUIString GetCallIn()
	{
		return m_strCallIn;
	}

	CIUIString GetCallOut()
	{
		return m_strCallOut;
	}
private:
	CIUIString LoadLanguageKeyValue(CIUIString strKey, CIUIString strSubKey, CIUIString strDefaultValue);
	ENUM_LANGUAGE m_enumLanguage;
	string m_strFileFullPath;
	CIUIString m_strAppName;
	CIUIString m_senderTitleSelf;
	CIUIString m_senderTitleOther;
	CIUIString m_strTipsDateOut;
	CIUIString m_strTipsNoRecordPath;
	CIUIString m_strTipsSaveDstPath;
	CIUIString m_strClassicEmotion;
	CIUIString m_strSymbolEmotion;
	CIUIString m_strBeginRecord;
	CIUIString m_strWaitingAnswerPre;
	CIUIString m_strWaitingAnswerEnd;
	CIUIString m_strTransferPre;
	CIUIString m_strAddGroupPre;
	CIUIString m_strAddGroupEnd;
	CIUIString m_strJoinGroupEnd;
	CIUIString m_strStartTransferPre;
	CIUIString m_strStartTransferEnd;
	CIUIString m_strConfrenceNum;
	CIUIString m_strConfrenceMemberMax;
	CIUIString m_strProfile;
	CIUIString m_strAppSettings;
	CIUIString m_strAddContact;
	CIUIString m_strOnLine;
	CIUIString m_strOffLine; 
	CIUIString m_strAway;
	CIUIString m_strBusy;
	CIUIString m_strDoNotDisturb;

	CIUIString m_strForward;
	CIUIString m_strDeleteThisContact;
	CIUIString m_strAddToLocalContact;
	CIUIString m_strEditLocalContact;
	CIUIString m_strAddToFavoriteContact;
	CIUIString m_strDeleteThisRecord;
	CIUIString m_strCancelFavorite;

	CIUIString m_strShowMain;
	CIUIString m_strAbout;
	CIUIString m_strExit;

	CIUIString m_strMoreMsg;
	
	CIUIString m_strOffLineCanNotDo;
	CIUIString m_strSignOut;
	CIUIString m_strAccount;
	CIUIString m_strPassword;
	CIUIString m_strSipDomain;

	CIUIString m_strApplication;
	CIUIString m_strAudioCodecs;
	CIUIString m_strVideoCodecs;
	CIUIString m_strMediaQuality;
	CIUIString m_strMisc;
	CIUIString m_strLanguages;

	//participant
	CIUIString m_strParticipants;
	CIUIString m_strMuteMeeting;
	CIUIString m_strUnMuteMetting;
	CIUIString m_strMore;

	//history record call event tips
	CIUIString m_strEventCallFailed;
	CIUIString m_strEventCallAudioType;
	CIUIString m_strEventCallVideoType;
	CIUIString m_strEventCallSuccess;
	CIUIString m_strEventCallMissedAudio;
	CIUIString m_strEventCallMissedVideo;

	CIUIString m_strCallOut;
	CIUIString m_strCallIn;
};

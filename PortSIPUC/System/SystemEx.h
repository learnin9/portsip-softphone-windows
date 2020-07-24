#ifndef SYSTEMEX_H
#define SYSTEMEX_H
#include <string>
#include <vector>
#include "db/portsql.hxx"
#include "VoipCallBackEvent.h"
#include "PortGo.h"
#include "SystemEx.h"
#include "Session.h"
#include "RestAPI.h"
#include "../Main/MainFrameTabControl/MainPanel/MainTabControl/InboxPanel/InboxCanvasHandler.h"
#include "../Main/MainFrameTabControl/MainPanel/MainTabControl/ContactsPanel/ContactsCanvasHandler.h"
#include "../Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/VideoCanvasHandler.h"
#include "../Main/MainFrameTabControl/MainPanel/MainTabControl/ChatPanel/AudioPageCanvasHandler.h"
#include "../Main/MainFrameTabControl/MainPanel/MainCanvasHandler.h"
#include "Login/LoginDlg.h"
#include <Windows.h>
#include "BaseThread.h"
#include "CriticalSection.h"
#include "Utility/DynamicText.h"
#include "../Main/CallerBarWndMgr.h"
class CLoginDlg;
using namespace  std;
class CSystemEx
{
public:
	CSystemEx();
	~CSystemEx(void);

	static CSystemEx* GetInstance();
	static void ReleaseInstance(void);
	static CSystemEx	*m_pInstance;
	void SetMainDlg(CMainWnd * pMainDlg) { m_pMainWidget = pMainDlg; };
	CMainWnd* GetMainDlg() { return m_pMainWidget; };
	void SetLoginDlg(CLoginDlg* pLoginDlg) {
		m_pLoginDlg = pLoginDlg;
	};
	CLoginDlg* GetCruuentLoginDlg() {
		return m_pLoginDlg;
	};
	void InitService();
	
	bool LoadDynamicText();
	PortSQL* GetDBOprate();
	CUser* GetGlobalUser() { return m_pUser; };
	void SetGobalUser(CUser* pUser) { m_pUser = pUser; };
	void SetRegister(bool bOk) { m_SIPRegistered = bOk; };
	bool GetRegister() { return m_SIPRegistered; }
	bool GetInitPortSip() {
		return m_SIPInitialized;
	}
	COptions* GetGlobalOptions() { return m_pOptions; }
	//void  CreateOneUser(std::string strSipNumber, std::string strPassWord, std::string strSipdomain, CUser *oUser);
	bool InitSIPByUser(TRANSPORT_TYPE enumTransport, int strLogLevel, std::string strLogPath, int nAudioDeviceLayer, int nVideoDeviceLayer, \
		bool bVerifyTLSCertificate, std::string &strErrorInfo, CUser* pUser);
	bool InitUserLocalSIPSettings();
	HANDLE GetSipLib() {
		if (m_hSIPLib == NULL)
		{
			LOG4_INFO("Sip Handle Is Not Init\n");
		}
		return m_hSIPLib;
	};
	void InitDefaultAudioCodecs();
	void InitDefaultVideoCodecs();
	std::string GetSipErrorInfo(int nErrorCode);
	int SubContactStatus(std::vector<TAG_RECENT*> vecContactDB);
	void SetInboCanvasHandler(CInboxCanvasHandler* pInboxCanvasHandler) { m_pInboxCanvasHandler = pInboxCanvasHandler; }
	CInboxCanvasHandler* GetInboCanvasHandler() {
		return m_pInboxCanvasHandler;
	}
	void SetMainCanvasHandler(CMainCanvasHandler* pMainCanvasHandler) { m_pMainCanvasHandler = pMainCanvasHandler; };
	CMainCanvasHandler* GetMainCanvasHandler()
	{
		return m_pMainCanvasHandler;
	}
	void SetContactCanvasHandler(CContactsCanvasHandler* pContactsCanvasHandler) { m_pContactCanvasHandler = pContactsCanvasHandler; }
	CContactsCanvasHandler* GetContactCanvasHandler() {
		return m_pContactCanvasHandler;
	}

	long SendMsg(std::string strSendTo, std::string strMsg);
	long SendJsonMsg(std::string strSendTo, std::string strJsonMsg);
	int  GetSipDomainPort() { return m_nSipPort; }
	bool sendVideoStream(int nCurrentLine, bool state);
	int FindFreeSession();
	int FindSessionBySessionID(int nSessionID);
	bool OnStartPlayAudiofile(std::string strFilePath);
	bool OnStopPlayAudiofile(std::string strFilePath);
	bool SendVideo(int nActiveSeesion);
	void StopVideo(int nActiveSession);
	bool CallOneNumberAudio(std::string strSipNumber, bool bSendSDP, int &nActiveSeesion, long& nCallHistoryPrimaryKeyID,std::string& strErrorInfo);
	bool CallOneNumberVideo(std::string strSipNumber, bool bSendSDP, int &nActiveSeesion, long& nCallHistoryPrimaryKeyID, std::string& strErrorInfo, HWND hRemoteVideoHwnd, HWND hLoclVideoHwnd = NULL,bool bConfence = false);
	void Answer(bool bHasVideo, int nActiveSeesion, HWND hWnd = NULL);
	bool GetCallState(int nActiveSeesion);
	void Hangup(int nActiveSeesion);
	void Hangup();
	//void MuteMicrophine(bool bMute);
	void MuteMicrophineSession(bool bMute,long nSessionID);
	void MuteSession(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera, long nSessionID);
	void Hold(int nAcitiveLine,bool bAutoHold = true);
	void UnHold(int nAcitiveLine, bool bAutoHold = true);
	void SwtichLineUpdateHold(vector<int > vecActiveLine);
	void SwtichLineUpdateUnHold(vector<int> vecActiveLine);
	bool joinConference(int index);
	int ProcessComingCall(ICallbackParameters * parameters, std::string strUserName);
	ENUM_LOGIN GetLocalStatus() {
		return m_enumLocalStatus;
	};
	void SetLocalStatus(ENUM_LOGIN enumLoginStatus) { m_enumLocalStatus = enumLoginStatus; };
	void ProcessTimer(UINT idEvent, UINT uMsg);
	static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	bool ReadOptions();
	void InitUserOptions();
	void InitAudioCodecsFormDB();
	
	vector <CAVCodec> GetAudioCodec();
	vector <CAVCodec> GetVideoCodec();
	CAVCodec GetCodecByID(int nCodecID,vector<CAVCodec>::iterator &itFind);
	int  GetAudioCodecOpen();
	void RefreshCodecInfoSwitch(bool bOpen,int nCodecID);
	void RefreshCodecInfoItemIndex(int nItemIndex, int nCodecID);
	void UpdateAudioCodec2DB();
	void UpdateVideoCodec2DB();
	void UpdateCallRecordByIndex(int nIndex);
	bool Transfer(std::string strTransferSipNumber, int nActiveLine, std::string& strErrorInfo);
	bool AttendTransfer(int nTransferedLine, int nTransferLine, std::string strTransferNumber, std::string &strErrorInfo);
	bool SetLocalVideoHwnd(HWND hWndLocalVideo,bool bVideoDeviceConf,std::string &strErrorInfo);

	bool StopLocalVideoHwnd(HWND hWndLocalVideo, bool bVideoDeviceConf, std::string &strErrorInfo);
	std::string ConstructTextMessage(std::string strText);
	std::string ConstructFileMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize);
	std::string ConstructImageMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int width, int height);
	std::string ConstructEmojiMessage(std::string strEmojiType);
	std::string ConstructVideoMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int nDuration);
	std::string ConstructAudioMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int nDuration);
	std::string ConstructShakeMessage();
	std::wstring GetFileSavePathByMsgType(ENUM_MSG_TYPE enumMsgType);
	void AddFile2UploadList(UpLoadDownLoadParam* pDownLoadParam);
	void AddFole2DownLoadList(UpLoadDownLoadParam* pDownLoadParam);
	void HangupByRemote(int nActiveSession,bool  bConnected = true,bool bJumpPage = true);
	void ProcessTransfer(int nActiveSession, bool bSucess,int nErrorCode =0);
	void HoldByRemote(int nActiveSession);
	void UnHoldByRemote(int nActiveSession);
	void AnswerByRemote(int nActiveSession,bool bConferenceRoom);
	void SaveAudioCavasHandler2Contact(CAudioPageCanvasHandler* pAudioPageCanvasHandler, TAG_RECENT *pContact);
	void SaveVideoCanvasHandler2Contact(CVideoCanvasHandler *pVideoCanvasHandler, TAG_RECENT *pContact);
	void EraseAudioCavasHandler2Contact(TAG_RECENT *pContact);
	void EraseVideoCanvasHandler2Contact(TAG_RECENT *pContact);
	CAudioPageCanvasHandler* GetAudioPageCanvasHandlerByContact(TAG_RECENT *pContactDB);
	CVideoCanvasHandler* GetVideoCanvasHandlerByContact(TAG_RECENT *pContactDB);
	void SaveSession2AudioCanvasHandler(int nActiveSession, CAudioPageCanvasHandler * pAudioPageCanvasHandler);
	void SaveSession2VideoCanvasHandler(int nActiveSession, CVideoCanvasHandler * pVideoPageCanvasHandler);
	void EraseSession2CanvasHandler(int nActiveSession);
	CAudioPageCanvasHandler* GetAudioPageCanvasHandler(int nActiveSession);
	CVideoCanvasHandler * GetVideoCanvasHandler(int nActiveSession);
	bool isAutoRun();
	void SetAutoRun();
	void CancelAutoRun();
	void AutoLogin();
	CCallerBarWndMgr GetCallerBarWndMGR() { return m_oCallerBarWndMgr; };
	int GetMetricsX() { return m_nScreenX; };
	int GetMetricsY() { return m_nScreenY; };
	bool InitRestAPIByUser();
	
	std::vector<string > GetRecentContact();
	std::vector<CChatRecord > GetUnReadMsgByContact(CContactDB *pContactDB,std::string strLastUnReadMsgID,ENUM_LIST_MODE enumListMode,int nPageCount=10);
	void ProcessVoiceMailGetInfo(vector<VoiceMailParam> vecVoiceMail);
	void TranslateParam2VoiceMailClass(VoiceMailParam &stuVoiceMailParam, CVoiceMail &oVoiceMail,std::string& strFileName);
	void TranslateParam2VoiceMailDownLoadParam(VoiceMailParam &stuVoiceMailParam, VoiceMailDownLoadParam &stuVoiceMailDownLoadParam);
	std::string GetVoiceMailDownLoadPath();
	void BeginVoiceMailPollThread();
	void UpdateAllOptionData();
	void AddUnReadMsg(std::vector<CChatRecord> vecUnReadMsg);
	void AddUnReadMsg(CChatRecord theUnReadMsg);
	void EraseUnReadMsg(vector<int> vecMsgID);
	void EraseUnReadMsgByContact(CContactDB* pContact);
	void EraseUnReadMsgAll();
	void EnableIcoFlash(bool bOpen);
	
	CRestAPI * GetRestAPI() { return m_pRestAPI; }
	void TipsFunNoRun();
	bool StartRecord(int nActiveLine,std::string strFilePath, std::string strFileName);
	void EndRecord(int nActiveSession, std::string strSipNumber, ENUM_VA_RECORD enumAVRecordType);
	CAVPlayer* GetGlobalAVPlayer() { return m_pAVPlayer; };
	BOOL SwtichEnable(int &nCurrentIndex, int &nMaxPage, bool  bTipsCheckSwitch);
	CString RefreshCallTime(int nActiveSession);
	void    ReSetCallTime(int nActiveSession);

	void    BtnAudio(char code);
	void    SendDTMF(int nActiveSession, int nTone);
	void    CreateConfence(ENUM_CONFENCE_TYPE enumConfenceType,HWND videoHwnd);
	void    DestoryConfence();
	void	InitVideoCodecs();
	void	InitAudioCodecs();
	void SetSRTPMode();
	void playSystemWaveFile(const string & waveFile);
	void loopPlaySystemWaveFile(const string & waveFile);
	void stopLoopPlaySystemWaveFile();
	void InitCallForwardingSettings();
	std::string FormatDownParam(UpLoadDownLoadParam *pUpLoadParam, std::string srFileName, std::string strHttpURL, ENUM_MSG_TYPE enumMsgFileType, TAG_RECENT *pContact,  INT64 nFileSize,long lMsgID);
	//unregister
	void SipUnRegister();
	//get line session id 
	long GetSessionID(int index);
	TagScreenMetrics GetLocalMetrics();
	TagScreenMetrics GetPersonMetrics(int nLineIndex);
	string GetSipHeaderValue(const char * szHeaderKey, const char *signMsg);
	void ExitSystem();
	void StopWorkThread();
	bool ReConnect();
	bool ReConnectThread();
	void SetOffline();
	void SetOnline();
	void SetOtherStatus(ENUM_LOGIN enumOtherStatus);
	static DWORD WINAPI _ConnectThread(LPVOID pParam);
	int  RegisteModule();
	bool GetConferenceStatus(std::string strConferenceNum,long &lSubID);
	bool UnGetConferenceStatus(long lSubID);
	void RefreshRecentTime(TAG_RECENT* pContactInfo);

	bool QueryFolderDataRegKey( LPCWSTR strValueName, CIUIString& strValue, int length= MAX_PATH);
private:
	
	void InitDeviceSettings();
	void InitOhterSettings();
	void InitQosSettings();
	bool IsAutoAnswer() {
		return m_bIsAutoAnswer;
	};
	void SetAutoAnswer(bool bIsAutoAnswer)
	{
		m_bIsAutoAnswer = bIsAutoAnswer;
	};
	bool AutoAnswer(int nSessionIndex , ICallbackParameters * parameters);

	void RfreshData2CInboxCanvasHandler();
	static unsigned int _DoUpLoadThread(void* pParam);
	static unsigned int _DoDownLoadThread(void* pParam);
	static unsigned int _DoProcessVoiceMail(void *pParam);
	
	void DoDownLoad();
	void DoUpLoad();
	void DoVoiceMailPoll();
	
public:
	void InitFileServer();

	void TerminateAllThread();
	//init just set value
	void InitCameraSwtich(bool bEnable)
	{
		m_bSwtichCamera = bEnable;
	}
	void InitAudioInSwtich(bool bEnable)
	{
		m_bSwtichAudioInDevice = bEnable;
	}
	void InitAudioOutSwtich(bool bEnable)
	{
		m_bSwtichAudioOutDevice = bEnable;
	}
	void ChangeDeviceSwtich(bool bEnableAudioOut,bool bEnableAudioIn,bool bEnableCamera  = false);
	
	CIUIString GetDataFolder() {
		return m_strDataFolder;
	};
	bool GetCameraEnable()
	{
		return m_bSwtichCamera;
	}
	bool GetAudioInEnable()
	{
		return m_bSwtichAudioInDevice;
	}
	bool GetAudioOutEnable()
	{
		return m_bSwtichAudioOutDevice;
	}

	CIUIString GetDynamicTextAppName()
	{
		return m_oDynamicText.GetAPPStringName();
	}
	
	CIUIString GetSenderTitleOtherText()
	{
		return m_oDynamicText.GetSenderTitle0();
	}

	CIUIString GetTipsDateOutText()
	{
		return m_oDynamicText.GetTipsDateOut();
	}
	CIUIString GetTipsNoRecordPathText()
	{
		return m_oDynamicText.GetTipsNoRecordPath();
	}
	CIUIString GetTipsSaveDstPathText()
	{
		return m_oDynamicText.GetTipsSaveDstPath();
	}
	CIUIString GetClassicEmotionText()
	{
		return m_oDynamicText.GetClassicEmotion();
	}
	CIUIString GetSymbolEmotionText()
	{
		return m_oDynamicText.GetSymbolEmotion();
	}
	CIUIString GetBeginRecordText()
	{
		return m_oDynamicText.GetBeginRecord();
	}
	CIUIString GetWaitingAnswerPreText()
	{
		return m_oDynamicText.GetWaitingAnswerPre();
	}
	CIUIString GetWaitingAnswerEndText()
	{
		return m_oDynamicText.GetWaitingAnswerEnd();
	}
	CIUIString GetTransferPreText()
	{
		return m_oDynamicText.GetTransferPre();
	}
	CIUIString GetAddGroupPreText()
	{
		return m_oDynamicText.GetAddGroupPre();
	}
	CIUIString GetAddGroupEndText()
	{
		return m_oDynamicText.GetAddGroupEnd();
	}
	CIUIString GetJoinGroupPreText()
	{
		return m_oDynamicText.GetJoinGroupPre();
	}
	CIUIString GetJoinGroupEndText()
	{
		return m_oDynamicText.GetJoinGroupEnd();
	}

	CIUIString GetStartTransferPreText()
	{
		return m_oDynamicText.GetStartTransferPre();
	}

	CIUIString GetStartTransferEndText()
	{
		return m_oDynamicText.GetStartTransferEnd();
	}
	CIUIString GetProfileText()
	{
		return m_oDynamicText.GetProfile();
	}
	CIUIString GetAppSettingsText()
	{
		return m_oDynamicText.GetAppSettings();
	}
	CIUIString GetAddContactText()
	{
		return m_oDynamicText.GetAddContact();
	}
	CIUIString GetOnLineText()
	{
		return m_oDynamicText.GetOnLine();
	}
	CIUIString GetOffLineText()
	{
		return m_oDynamicText.GetOffLine();
	}
	CIUIString  GetAwayText()
	{
		return m_oDynamicText.GetAway();
	}
	CIUIString  GetBusyText()
	{
		return m_oDynamicText.GetBusy();
	}
	CIUIString GetDoNotDisturbText()
	{
		return m_oDynamicText.GetDoNotDisturb();
	}

	CIUIString GetForwardText()
	{
		return m_oDynamicText.GetForward();
	}
	CIUIString GetDeleteThisContactText()
	{
		return m_oDynamicText.GetDeleteThisContact();
	}
	CIUIString GetAddToLocalContactText()
	{
		return m_oDynamicText.GetAddToLocalContact();
	}
	CIUIString GetEditLocalContactText()
	{
		return m_oDynamicText.GetEditLocalContact();
	}
	CIUIString  GetAddToFavoriteContactText()
	{
		return m_oDynamicText.GetAddToFavoriteContact();
	}
	CIUIString  GetDeleteThisRecordText()
	{
		return m_oDynamicText.GetDeleteThisRecord();
	}
	CIUIString  GetCancelFavoriteText()
	{
		return m_oDynamicText.GetCancelFavorite();
	}

	CIUIString  GetShowMainText()
	{
		return m_oDynamicText.GetShowMain();
	}
	CIUIString   GetAboutText()
	{
		return m_oDynamicText.GetAbout();
	}

	CIUIString    GetExitText()
	{
		return m_oDynamicText.GetExit();
	}

	//编辑框文字
	CIUIString   GetAccountText()
	{
		return m_oDynamicText.GetAccount();
	}
	CIUIString   GetPasswordText()
	{
		return m_oDynamicText.GetPassword();
	}
	CIUIString   GetSipDomainText()
	{
		return m_oDynamicText.GetSipDomain();
	}

	CIUIString   GetApplicationText()
	{
		return m_oDynamicText.GetApplication();
	}
	CIUIString   GetAudioCodecsText()
	{
		return m_oDynamicText.GetAudioCodecs();
	}
	CIUIString   GetVideoCodecsText()
	{
		return m_oDynamicText.GetVideoCodecs();
	}
	CIUIString   GetMediaQualityText()
	{
		return m_oDynamicText.GetMediaQuality();
	}
	CIUIString   GetMiscText()
	{
		return m_oDynamicText.GetMisc();
	}
	CIUIString   GetLanguagesText()
	{
		return m_oDynamicText.GetLanguages();
	}


	CIUIString GetMoreMsgText()
	{
		return m_oDynamicText.GetMoreMsg();
	}
	CIUIString   GetParticipantsText()
	{
		return  m_oDynamicText.GetParticipants();
	}
	CIUIString   GetMuteMeetingText()
	{
		return  m_oDynamicText.GetMuteMeeting();
	}

	CIUIString GetUnMuteMettingText()
	{
		return m_oDynamicText.GetUnMuteMetting();
	}

	CIUIString GetParticipantsMoreText()
	{
		return m_oDynamicText.GetParticipantsMore();
	}

	CIUIString GetOffLineCanNotDoText()
	{
		return m_oDynamicText.GetOffLineCanNotDo();
	}

	CIUIString GetConfrenceNumText()
	{
		return m_oDynamicText.GetConfrenceNum();
	}

	CIUIString GetConfrenceNumberMaxText()
	{
		return m_oDynamicText.GetConfrenceMemberMax();
	}

	CIUIString GetSignOutText()
	{
		return m_oDynamicText.GetSignOut();
	}

	CIUIString GetEventCallFailedText()
	{
		return m_oDynamicText.GetEventCallFailed();
	}
	CIUIString GetEventCallAudioTypeText()
	{
		return m_oDynamicText.GetEventCallAudioType();
	}
	CIUIString GetEventCallVideoTypeText()
	{
		return m_oDynamicText.GetEventCallVideoType();
	}
	CIUIString GetEventCallSuccessText()
	{
		return m_oDynamicText.GetEventCallSuccess();
	}
	CIUIString GetEventCallMissedAudioText()
	{
		return m_oDynamicText.GetEventCallMissedAudio();
	}
	CIUIString GetEventCallMissedVideoText()
	{
		return m_oDynamicText.GetEventCallMissedVideo();
	}
	CIUIString GetCallInText()
	{
		return m_oDynamicText.GetCallIn();
	}
	CIUIString GetCallOutText()
	{
		return m_oDynamicText.GetCallOut();
	}
private:
	CMainWnd*  m_pMainWidget;
	PortSQL* m_pDB;
	HANDLE m_hSIPLib;
	CVoipCallBackEvent* m_pVipCallBackEvent;
	CUser * m_pUser;
	COptions* m_pOptions;
	Session m_SessionArray[MAX_LINES];
	int m_ActiveLine;
	CInboxCanvasHandler*	m_pInboxCanvasHandler;
	CRestAPI*				m_pRestAPI;
	CContactsCanvasHandler *m_pContactCanvasHandler;
	CMainCanvasHandler		*m_pMainCanvasHandler;
	int						m_nSipPort;
	CLoginDlg*				m_pLoginDlg;
	ENUM_LOGIN				m_enumLocalStatus;
	bool					m_bIsAutoAnswer;
	HWND					m_hLocalHwnd;
	CCallerBarWndMgr        m_oCallerBarWndMgr;
	CBaseThread				m_oFileDownLoadThread;
	CBaseThread				m_oFileUploadThread;
	CBaseThread				m_oProcessVoiceMailThread;
	vector<UpLoadDownLoadParam*> m_vecFileUpLoad;
	vector<UpLoadDownLoadParam*> m_vecFileDownLoad;
	int							m_nVoiceMailCheckTime;
	long					    m_lLastCheckTime;
	CCriticalSection			m_oFileDownLoadLock;
	CCriticalSection			m_oFileUpLoadLock;
	CCriticalSection			m_oMapSession2Canvas;
	CCriticalSection            m_oLockLocalVideo;
	CCriticalSection			m_oSessionLock;
	HANDLE						m_hDownLoadEvent;
	HANDLE                      m_hUpLoadEvent;
	bool						m_bDownLoadActive;
	bool						m_bUpLoadActive;
	bool                        m_bVoiceMailActive;
	bool                        m_bExitDownLoadThread;
	bool                        m_bExitUpLoadThread;
	bool                        m_bExitVoiceLoadThread;
	map <int, CAudioPageCanvasHandler *> m_mapAudioCanvasHandler;
	map < int,CVideoCanvasHandler*> m_mapVideoCanvasHandler;
	map<CAudioPageCanvasHandler *,TAG_RECENT* > m_mapAudioCanvasHandler2Contact;
	map<CVideoCanvasHandler *, TAG_RECENT* >	m_mapVideoCanvasHandler2Contact;
	map<HWND,bool>	m_mapLocalVideoHwnd;
	int m_nScreenX;
	int m_nScreenY;
	
	std::vector<CChatRecord> m_vecUnReadChatRecord;
	CCriticalSection		 m_oUnReadChartRecordLock;
	CAVPlayer* m_pAVPlayer;
	vector<CAVCodec> m_vecAudioCodec;
	vector<CAVCodec> m_vecVideoCodec;

	bool  m_bExistConfence;
	bool m_SIPInitialized;
	bool m_SIPRegistered;
	bool  m_bPlayCallingAudio;
	//Globel Switch
	bool  m_bSwtichCamera;
	bool  m_bSwtichAudioOutDevice;
	bool  m_bSwtichAudioInDevice;
	CDynamicText m_oDynamicText;
	CIUIString   m_strDataFolder;
};
#endif

#include "stdafx.h"
#include "SystemEx.h"
#include "Utility/utility.hxx"
#include <algorithm>

extern wchar_t *G_TCHAR_TRANSPORT[];
CSystemEx* CSystemEx::m_pInstance = NULL;
void CALLBACK CSystemEx::TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (TIMER_MAX >=idEvent &&idEvent>=TIMER_BASE)
	{
		CSystemEx::GetInstance()->ProcessTimer(idEvent, uMsg);
	}
}

bool CodecCompare(CAVCodec a, CAVCodec b)
{
	return a.m_nItemIndex < b.m_nItemIndex;
}
CSystemEx::CSystemEx( )
{
	m_pMainWidget = NULL;
	m_hSIPLib = NULL;
	m_pVipCallBackEvent = NULL;
	m_pDB = NULL;
	m_ActiveLine = 0; 
	m_pInboxCanvasHandler = NULL;
	m_ActiveLine = 0;
	m_pRestAPI = NULL;
	m_pContactCanvasHandler = NULL;
	m_nSipPort = 5060;
	m_pLoginDlg = NULL;
	m_enumLocalStatus = ENUM_OFFLINE;
	m_bIsAutoAnswer = false;
	m_pMainCanvasHandler = NULL;

	m_nScreenX = GetSystemMetrics(SM_CXSCREEN);
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	m_nScreenY = rt.bottom-rt.top;

	m_nVoiceMailCheckTime = VOICE_MAIL_CHECK_TIME;
	m_lLastCheckTime = 0;
	m_pOptions = new COptions;
	m_bExistConfence = false;
	m_SIPRegistered = false;
	m_SIPInitialized = false;
	m_bPlayCallingAudio = false;

	m_bSwtichCamera = false;
	m_bSwtichAudioOutDevice = false;
	m_bSwtichAudioInDevice = false;

	m_hDownLoadEvent = NULL;
	m_hUpLoadEvent = NULL;

	m_bDownLoadActive = true;
	m_bUpLoadActive = true;
	m_bVoiceMailActive = true;

	m_bExitDownLoadThread = false;
	m_bExitUpLoadThread = false;
	m_bExitVoiceLoadThread = false;

	m_strDataFolder  = _T("");
}

CSystemEx::~CSystemEx(void)
{
	if (m_pDB)
	{
		m_pDB->closeDB();
		delete m_pDB;
		m_pDB = NULL;
	}

	if (m_pVipCallBackEvent)
	{
		delete m_pVipCallBackEvent;
		m_pVipCallBackEvent = NULL;
	}

	if (m_pRestAPI)
	{
		delete m_pRestAPI;
		m_pRestAPI = NULL;
	}

	if (m_pOptions!=NULL)
	{
		delete m_pOptions;
		m_pOptions = NULL;
	}
	if (m_hDownLoadEvent!=NULL)
	{
		CloseHandle(m_hDownLoadEvent);
		m_hDownLoadEvent = NULL;
	}
	if (m_hUpLoadEvent!=NULL)
	{
		CloseHandle(m_hUpLoadEvent);
		m_hUpLoadEvent = NULL;
	}
}
CSystemEx* CSystemEx::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CSystemEx();	
		m_pInstance->InitService();
	}
	return m_pInstance;
}

void CSystemEx::ReleaseInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
	
	return;
}

bool CSystemEx::LoadDynamicText()
{
	bool bRet = ReadOptions();
	if (bRet == false)
	{
		LOG4_FATAL("Can't  ReadOptions");
		TerminateProcess(GetCurrentProcess(), 0);
	}

	bRet=m_oDynamicText.LoadDynamicText("Lauguages.otto", (ENUM_LANGUAGE)GetGlobalOptions()->m_nLanguage);
	if (bRet == false)
	{
		LOG4_FATAL("Can't Load DyanmicText");
		TerminateProcess(GetCurrentProcess(), 0);
	}

	return bRet;
}
void CSystemEx::InitService()
{
	std::string strFullPath ="";
	PortUtility::getCurrentlyPath(strFullPath);
	bool bReadRegKey = CSystemEx::GetInstance()->QueryFolderDataRegKey(_T("DataPath"), m_strDataFolder);
	if (bReadRegKey==false)
	{
		LOG4_INFO("Read FolderDataKeyFailed\n");
		m_strDataFolder = _T("C:\\ProgramData\\PortSIP\\softphone\\");
	}
	else
	{
#if  _DEBUG
		m_strDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
		m_strDataFolder  = m_strDataFolder+L"\\";
#endif
		
	}
	strFullPath = PortUtility::wstring2String(m_strDataFolder.GetBuffer(m_strDataFolder.GetLength()));
	std::string strLogFullPath  = strFullPath + "\\clientlog";
	PortUtility::CheckFilePath(strLogFullPath);
	if (!LoggerLocal::init(strFullPath,L"[%D{%Y-%m-%d %H:%M:%S,%Q}] [%t] %-5p - %m%n")) {
		TerminateProcess(GetCurrentProcess(), 0);
	}
	LoggerLocal::setloglevel("TRACE");
	LOG4_INFO("Begin To Start Service");
	if (strFullPath.empty() == true)
	{
		return;
	}

	std::string strDBPath = PortUtility::Unicode2Utf8_ND(m_strDataFolder.GetBuffer(m_strDataFolder.GetLength())) + DB_DATA;
	m_pDB = new PortSQL();
	m_pDB->openDB(strDBPath);
	m_pVipCallBackEvent = new CVoipCallBackEvent();
	PortUtility::initializeWinsock();
	m_pAVPlayer = new CAVPlayer();
	m_hDownLoadEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hUpLoadEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

bool CSystemEx::InitSIPByUser(TRANSPORT_TYPE enumTransport, int nLogLevel, std::string strLogPath, int nAudioDeviceLayer, int nVideoDeviceLayer, \
	bool bVerifyTLSCertificate, std::string &strErrorInfo, CUser* pUser)
{
	strErrorInfo = "";
	bool bReslut = false;
	int nErrcode = 0;
	srand(GetTickCount());
	int localSIPPort = SIPPORT_MIN + rand() % 5000;

	//find a Available port
	/*for (;;)
	{
		if (PortUtility::isPortAvailable(localSIPPort) == true)
		{
			break;
		}
		else
		{
			localSIPPort = SIPPORT_MIN + rand() % 5000;
		}
	}*/
	
	//initialize->setLicenseKey->setUser
	m_hSIPLib = PortSIP_initialize(m_pVipCallBackEvent,
		(TRANSPORT_TYPE)enumTransport,
		"0.0.0.0", localSIPPort,
		(PORTSIP_LOG_LEVEL)nLogLevel,
		strLogPath.c_str(),
		MAX_LINES,
		USER_AGENT, nAudioDeviceLayer, nVideoDeviceLayer, NULL, NULL, bVerifyTLSCertificate,
		&nErrcode);
	if (m_hSIPLib == NULL || nErrcode != 0)
	{
		std::stringstream ssErrorCode;
		ssErrorCode << "Init SIPByUser Error Code :" << nErrcode;
		strErrorInfo = ssErrorCode.str();
		//TODO:TIPS 2 WINDOW
		return bReslut;
	}

	int nReslut = 0;

	if (m_pUser->m_nOutboundPort != 0)
	{
		m_nSipPort = m_pUser->m_nOutboundPort;
	}

	/*TODO:UTF8?*/
	m_SIPInitialized = true;
	 nReslut = PortSIP_setUser(m_hSIPLib, pUser->m_strSipNumber.c_str(),
		 pUser->m_strDisplayName.empty() == true ? NULL : pUser->m_strDisplayName.c_str(),
		 pUser->m_strAuthName.empty() == true ? NULL : pUser->m_strAuthName.c_str(),
		 pUser->m_strPassword.c_str(),
		 pUser->m_strSipDomain.empty() == true ? NULL : pUser->m_strSipDomain.c_str(),
		 pUser->m_strOutboundServer.empty() == true ? pUser->GetSipServer().c_str() : pUser->m_strOutboundServer.c_str(),
		 m_nSipPort,
		 pUser->m_strStunServer.empty()==true?NULL: pUser->m_strStunServer.c_str(),\
		 pUser->m_nStunPort, 
		 /*pUser->m_strOutboundServer.empty() == true ? NULL : pUser->m_strOutboundServer.c_str()*/NULL,\
		 /*pUser->m_nOutboundPort*/NULL);

	if (nReslut != 0)
	{
		std::stringstream ssErrorCode;
		std::string strErrorCodeInfo;
		strErrorCodeInfo = GetSipErrorInfo(nReslut);
		ssErrorCode << "Set  User Info Error Code :" << nReslut<<" Info:"<< strErrorCodeInfo;
		strErrorInfo = ssErrorCode.str();
		LOG4_INFO(strErrorInfo.c_str());
		//TODO:TIPS 2 WINDOW
		PortSIP_unInitialize(m_hSIPLib);
		delete m_pVipCallBackEvent;
		m_hSIPLib = NULL;
		m_pVipCallBackEvent = NULL;
		m_SIPInitialized = false;
		return bReslut;
	}

	PortSIP_setPresenceMode(GetSipLib(), pUser->m_nPresencMode);
	
	if (PortSIP_registerServer(m_hSIPLib, 60,0) != 0)
	{
		PortSIP_unInitialize(m_hSIPLib);
		m_hSIPLib = NULL;
		delete m_pVipCallBackEvent;
		m_pVipCallBackEvent = NULL;
		strErrorInfo = "Register Server Failed";
		//TODO:TIPS 2 WINDOW
		return false;
	}
	//SetSRTPMode();
	//
	PortSIP_setDefaultSubscriptionTime(m_hSIPLib,m_pUser->m_nSubscribeRefreshTime);
	PortSIP_setDefaultPublicationTime(m_hSIPLib, m_pUser->m_nPublishRefreshTime);
	return true;
}

void CSystemEx::ChangeDeviceSwtich(bool bEnableAudioOut,bool bEnableAudioIn,bool bEnableCamera)
{
	auto iterAudioCavasHanler = m_mapAudioCanvasHandler2Contact.begin();
	for (iterAudioCavasHanler; iterAudioCavasHanler != m_mapAudioCanvasHandler2Contact.end(); iterAudioCavasHanler++)
	{
		CAudioPageCanvasHandler *pAudioPageCanvasHandler = iterAudioCavasHanler->first;
		if (pAudioPageCanvasHandler != NULL)
		{
			pAudioPageCanvasHandler->ChangeDeviceSwtich(bEnableAudioOut, bEnableAudioIn, false);
		}
	}

	auto iterVideoCavasHanler = m_mapVideoCanvasHandler2Contact.begin();
	for (iterVideoCavasHanler; iterVideoCavasHanler != m_mapVideoCanvasHandler2Contact.end(); iterVideoCavasHanler++)
	{
		CVideoCanvasHandler *pVideoPageCanvasHandler = iterVideoCavasHanler->first;
		if (pVideoPageCanvasHandler != NULL)
		{
			pVideoPageCanvasHandler->ChangeDeviceSwtich(bEnableAudioOut, bEnableAudioIn, bEnableCamera);
		}
		return;
	}
}
bool CSystemEx::InitRestAPIByUser()
{
	bool bRet = false;
	std::string strRestURL = m_pUser->GetSipServer() ;
	std::string strOutBound = CSystemEx::GetInstance()->GetGlobalUser()->m_strOutboundServer;
	std::string strSipDomain = strOutBound.empty() == true ? m_pUser->m_strSipDomain : strOutBound;
	m_pRestAPI = new CRestAPI(strSipDomain);
	if (m_pRestAPI!=NULL)
	{
		std::wstring wstrProtocol = G_TCHAR_TRANSPORT[GetGlobalUser()->GetProtocolMode()];
		std::string strProtocol = PortUtility::wstring2String(wstrProtocol);

		bRet = m_pRestAPI->RestAPILogin(m_pUser->m_strSipNumber, m_pUser->m_strPassword, m_pUser->m_strSipDomain, strProtocol,m_nSipPort);
		if (bRet)
		{
			std::string strSIPNumberID = m_pRestAPI->GetExtensionID(CSystemEx::GetInstance()->GetGlobalUser()->m_strSipNumber);
		}
		//m_pRestAPI->GetContacts();
	}
	return bRet;
}
void CSystemEx::BeginVoiceMailPollThread()
{
	m_oProcessVoiceMailThread.BeginThread(_DoProcessVoiceMail, this);
}
void CSystemEx::TranslateParam2VoiceMailClass(VoiceMailParam &stuVoiceMailParam, CVoiceMail &oVoiceMail, std::string& strFileName)
{
	oVoiceMail.m_ID = stuVoiceMailParam.m_lID;
	oVoiceMail.m_strCalled = stuVoiceMailParam.m_strCalled;
	oVoiceMail.m_strCaller = stuVoiceMailParam.m_strCaller;
	std::stringstream ssFileName;
	ssFileName << oVoiceMail.m_ID<<".wav";
	strFileName = ssFileName.str();
	oVoiceMail.m_strFileName = strFileName; 
	oVoiceMail.m_strTime = stuVoiceMailParam.m_strTime;
}
void CSystemEx::TranslateParam2VoiceMailDownLoadParam(VoiceMailParam &stuVoiceMailParam, VoiceMailDownLoadParam &stuVoiceMailDownLoadParam)
{
	stuVoiceMailDownLoadParam.m_lID = stuVoiceMailParam.m_lID;
	stuVoiceMailDownLoadParam.m_fProcessData = 0;
}
std::string CSystemEx::GetVoiceMailDownLoadPath()
{
	std::string strFilePath;
	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);
	if (m_strDataFolder.IsEmpty())
	{
		LOG4_ERROR("save file path is error");
		m_strDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
	}

	strFilePath = PortUtility::wstring2String(m_strDataFolder.GetBuffer(m_strDataFolder.GetLength())) + "\\cacheFiles\\voicemail\\";

	PortUtility::CheckFilePath(strFilePath);
	return strFilePath;
}
void CSystemEx::ProcessVoiceMailGetInfo(vector<VoiceMailParam> vecVoiceMail)
{
	auto iterVoiceMail = vecVoiceMail.begin();
	for (iterVoiceMail;iterVoiceMail!=vecVoiceMail.end();iterVoiceMail++)
	{
		VoiceMailParam theLocalVoiceMailParam = *iterVoiceMail;
		VoiceMailDownLoadParam stuLocalVoiceDownLoadParam;
		CVoiceMail theVoiceMail;
		
		bool bExist = GetDBOprate()->IsExistVoiceInfo(theLocalVoiceMailParam.m_lID);
		if (bExist)
		{
			bool bUnDownLoad = GetDBOprate()->IsDownLoad(theLocalVoiceMailParam.m_lID);
			if (bUnDownLoad ==false)
			{
				std::string strFileName;
				TranslateParam2VoiceMailClass(theLocalVoiceMailParam, theVoiceMail, strFileName);
				TranslateParam2VoiceMailDownLoadParam(theLocalVoiceMailParam, stuLocalVoiceDownLoadParam);

				bool bRet = m_pRestAPI->DownLoadVoiceMail(theLocalVoiceMailParam.m_strFileHttpURL, strFileName, GetVoiceMailDownLoadPath(), stuLocalVoiceDownLoadParam);
				if (bRet)
				{
					theVoiceMail.m_enumDownLoadStatus = ENUM_OK;
				}
				std::string strFileTemplatePath = GetVoiceMailDownLoadPath() + strFileName;

				int nSencond = 0;
				CIUIString strTemplatePlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFileTemplatePath, nSencond);
				theVoiceMail.m_strPlayTime = PortUtility::Unicode2Utf8_ND(strTemplatePlayTime.GetBuffer(strTemplatePlayTime.GetLength()));
				theVoiceMail.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();

				GetDBOprate()->AddOneVoiceMail(theVoiceMail);

				::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_DOWNLOAD_VOICE_MAIL, (WPARAM)&stuLocalVoiceDownLoadParam, 0);
			}
			continue;
		}
		else
		{
			std::string strFileName;
			TranslateParam2VoiceMailClass(theLocalVoiceMailParam, theVoiceMail, strFileName);
			TranslateParam2VoiceMailDownLoadParam(theLocalVoiceMailParam, stuLocalVoiceDownLoadParam);
		
			bool bRet = m_pRestAPI->DownLoadVoiceMail(theLocalVoiceMailParam.m_strFileHttpURL, strFileName, GetVoiceMailDownLoadPath(), stuLocalVoiceDownLoadParam);
			if (bRet)
			{
				theVoiceMail.m_enumDownLoadStatus = ENUM_OK;
			}
			std::string strFileTemplatePath = GetVoiceMailDownLoadPath() + strFileName;

			int nSencond = 0;
			CIUIString strTemplatePlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFileTemplatePath,nSencond);
			theVoiceMail.m_strPlayTime = PortUtility::Unicode2Utf8_ND(strTemplatePlayTime.GetBuffer(strTemplatePlayTime.GetLength()));
			theVoiceMail.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
			
			GetDBOprate()->AddOneVoiceMail(theVoiceMail);

			::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_DOWNLOAD_VOICE_MAIL, (WPARAM)&stuLocalVoiceDownLoadParam, 0);
		}
	}
}
bool CSystemEx::InitUserLocalSIPSettings()
{
	InitCallForwardingSettings();
	InitDeviceSettings();
	InitOhterSettings();
	InitAudioCodecs();
	InitVideoCodecs();
	InitQosSettings();

	return true;
}
void CSystemEx::InitQosSettings()
{
	if (!GetSipLib())
	{
		return;
	}

	if (m_pOptions->m_nAudioDSCP != 0)
	{
		PortSIP_enableAudioQos(GetSipLib(), true);
	}
	else
	{
		PortSIP_enableAudioQos(GetSipLib(), false);
	}

	if (m_pOptions->m_nVideoDSCP != 0)
	{
		PortSIP_enableVideoQos(GetSipLib(), true);
	}
	else
	{
		PortSIP_enableVideoQos(GetSipLib(), false);
	}
}


void CSystemEx::InitCallForwardingSettings()
{
	PortSIP_disableCallForward(CSystemEx::GetInstance()->GetSipLib());

	std::string strForwardTo = m_pOptions->m_strForwardTo+"@"+CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	std::string strBusyFowardTo = m_pOptions->m_strBusyForwardTo + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	if (m_pOptions->m_strForwardTo.empty() == false&&m_pOptions->m_bEnableForword==true)
	{
		PortSIP_enableCallForward(CSystemEx::GetInstance()->GetSipLib(), false, strForwardTo.c_str());
	}
	else
	{
		if (m_pOptions->m_strBusyForwardTo.empty() == false&&m_pOptions->m_bEnableOnPhoneForword==true)
		{
			PortSIP_enableCallForward(CSystemEx::GetInstance()->GetSipLib(), true, strBusyFowardTo.c_str());
		}
	}

	if (m_pOptions->m_strForwardTo.empty() == true && m_pOptions->m_strBusyForwardTo.empty() == true)
	{
		PortSIP_disableCallForward(CSystemEx::GetInstance()->GetSipLib());
	}
}


std::string  CSystemEx::GetSipErrorInfo(int errorCode)
{
	string strErrorInfo = "";
	switch (errorCode)
	{
	case 0:
		break;
	case INVALID_SESSION_ID:
		strErrorInfo = "invalid session id ";
		break;
	case ECoreAlreadyInitialized:
		strErrorInfo = "Core Already Initialized ";
		break;
	case ECoreNotInitialized:
		strErrorInfo = "Core Not Initialized ";
		break;
	case ECoreSDKObjectNull:
		strErrorInfo = "Core SDK Object Is Null ";
		break;
	case ECoreArgumentNull:
		strErrorInfo = "Core SDK Object Is Null ";
		break;
	case ECoreInitializeWinsockFailure:
		strErrorInfo = "Core Initialize Winsock Failure ";
		break;
	case ECoreUserNameAuthNameEmpty:
		strErrorInfo = "Core UserName AuthName Empty";
		break;
	case ECoreInitializeStackFailure:
		strErrorInfo = "Core Initialize Stack Failure";
		break;
	case ECorePortOutOfRange:
		strErrorInfo = "Core Port Out Of Range";
		break;
	case ECoreAddTcpTransportFailure:
		strErrorInfo = "Core Add Tcp Transport Failure";
		break;
	case ECoreAddTlsTransportFailure:
		strErrorInfo = "Core Add Tls Transport Failure";
		break;
	case ECoreAddUdpTransportFailure:
		strErrorInfo = "Core Add Udp Transport Failure";
		break;
	case ECoreMiniAudioPortOutOfRange:
		strErrorInfo = "Core Mini Audio Port Out Of Range";
		break;
	case ECoreMaxAudioPortOutOfRange:
		strErrorInfo = "Core Max Audio Port Out Of Range";
		break;
	case ECoreMiniVideoPortOutOfRange:
		strErrorInfo = "Core Mini Video Port Out Of Range";
		break;
	case ECoreMaxVideoPortOutOfRange:
		strErrorInfo = "Core Max Video Port Out Of Range";
		break;
	case ECoreMiniAudioPortNotEvenNumber:
		strErrorInfo = "Core Mini Audio Port Not Even Number";
		break;
	case ECoreMaxAudioPortNotEvenNumber:
		strErrorInfo = "Core Max Audio Port Not Even Number";
		break;
	case ECoreMiniVideoPortNotEvenNumber:
		strErrorInfo = "Core Mini Video Port Not Even Number";
		break;
	case ECoreMaxVideoPortNotEvenNumber:
		strErrorInfo = "Core Max Video Port Not Even Number";
		break;
	case ECoreAudioVideoPortOverlapped:
		strErrorInfo = "Core Audio Video Port Overlapped";
		break;
	case ECoreAudioVideoPortRangeTooSmall:
		strErrorInfo = "Core Audio Video Port Range Too Small";
		break;
	case ECoreAlreadyRegistered:
		strErrorInfo = "Core	Already	Registered";
		break;
	case ECoreSIPServerEmpty:
		strErrorInfo = "Core SIP Server Empty";
		break;
	case ECoreExpiresValueTooSmall:
		strErrorInfo = "Core Expires Value Too Small";
		break;
	case ECoreCallIdNotFound:
		strErrorInfo = "Core Call Id Not Found";
		break;
	case ECoreNotRegistered:
		strErrorInfo = "Core Not Registered";
		break;
	case ECoreCalleeEmpty:
		strErrorInfo = "Core Callee Empty";
		break;
	case ECoreInvalidUri:
		strErrorInfo = "Core Invalid Uri";
		break;
	case ECoreAudioVideoCodecEmpty:
		strErrorInfo = "Core Audio Video Codec Empty";
		break;
	case ECoreNoFreeDialogSession:
		strErrorInfo = "Core No Free Dialog Session";
		break;
	case ECoreCreateAudioChannelFailed:
		strErrorInfo = "Core Create Audio Channel Failed";
		break;
	case ECoreSessionTimerValueTooSmall:
		strErrorInfo = "Core Session Timer Value Too Small";
		break;
	case ECoreAudioHandleNull:
		strErrorInfo = "Core Audio Handle Null";
		break;
	case ECoreVideoHandleNull:
		strErrorInfo = "Video Handle Null";
		break;
	case ECoreCallIsClosed:
		strErrorInfo = "Call Is Closed";
		break;
	case ECoreCallAlreadyHold:
		strErrorInfo = "Core Call Already Hold";
		break;
	case ECoreCallNotEstablished:
		strErrorInfo = "Call Not Established";
		break;
	case ECoreCallNotHold:
		strErrorInfo = "Call Call Not Hold ";
		break;
	case ECoreSipMessaegEmpty:
		strErrorInfo = "Sip	Messaeg	Empty ";
		break;
	case ECoreSipHeaderNotExist:
		strErrorInfo = "Sip	Header	Not	Exist";
		break;
	case ECoreSipHeaderValueEmpty:
		strErrorInfo = "Sip	Header Value Empty";
		break;
	case ECoreSipHeaderBadFormed:
		strErrorInfo = "Sip	Header	Bad	Formed";
		break;
	case ECoreBufferTooSmall:
		strErrorInfo = "Core Buffer Too Small";
		break;
	case ECoreSipHeaderValueListEmpty:
		strErrorInfo = "ECore Sip Header Value List Empty";
		break;
	case ECoreSipHeaderParserEmpty:
		strErrorInfo = "Core Sip Header Parser Empty";
		break;
	case ECoreSipHeaderValueListNull:
		strErrorInfo = "Core Sip Header Value List Null";
		break;
	case ECoreSipHeaderNameEmpty:
		strErrorInfo = "Core Sip Header Name Empty";
		break;
	case ECoreAudioSampleNotmultiple:
		strErrorInfo = "Core Audio Sample Notmultiple";
		break;	//	The audio sample should be multiple of 10
	case ECoreAudioSampleOutOfRange:
		strErrorInfo = "Core Audio Sample OutOfRange";
		break;	//	The audio sample range is 10 - 60
	case ECoreInviteSessionNotFound:
		strErrorInfo = "Core Invite Session Not Found";
		break;
	case ECoreStackException:
		strErrorInfo = "Core Stack Exception";
		break;
	case ECoreMimeTypeUnknown:
		strErrorInfo = "Core MimeType Unknown";
		break;
	case ECoreDataSizeTooLarge:
		strErrorInfo = "Core Data Size Too Large";
		break;
	case ECoreSessionNumsOutOfRange:
		strErrorInfo = "Core Session Nums OutOfRange";
		break;
	case ECoreNotSupportCallbackMode:
		strErrorInfo = "Core Not Support Callback Mode";
		break;
	case ECoreNotFoundSubscribeId:
		strErrorInfo = "Core Core Not Found Subscribe Id";
		break;
	case ECoreCodecNotSupport:
		strErrorInfo = "Core Codec Not	Support";
		break;
	case ECoreCodecParameterNotSupport:
		strErrorInfo = "Core Codec Parameter Not Support";
		break;
	case ECorePayloadOutofRange:
		strErrorInfo = "Core Payload Out of Range";
		break;
	case ECorePayloadHasExist:
		strErrorInfo = "Core Payload Has Exist";
		break;
	case ECoreFixPayloadCantChange:
		strErrorInfo = "Core Fix Payload Cant Change";
		break;

		//new add
	case ECoreCodecTypeInvalid:
		strErrorInfo = "Core Payload Out of Range";
		break;
	case ECoreCodecWasExist:
		strErrorInfo = "Core Payload Has Exist";
		break;
	case ECorePayloadTypeInvalid:
		strErrorInfo = "Core ECorePayloadTypeInvalid";
		break;
	case ECoreArgumentTooLong:
		strErrorInfo = "Core ECoreArgumentTooLong";
		break;
	case ECoreMiniRtpPortMustIsEvenNum:
		strErrorInfo = "Core ECoreMiniRtpPortMustIsEvenNum";
		break;
	case ECoreCallInHold:
		strErrorInfo = "Core ECoreCallInHold";
		break;
	case ECoreNotIncomingCall:
		strErrorInfo = "Core ECoreNotIncomingCall";
		break;
	case ECoreCreateMediaEngineFailure:
		strErrorInfo = "Core ECoreCreateMediaEngineFailure";
		break;
	case ECoreAudioCodecEmptyButAudioEnabled:
		strErrorInfo = "Core ECoreAudioCodecEmptyButAudioEnabled";
		break;
	case ECoreVideoCodecEmptyButVideoEnabled:
		strErrorInfo = "Core ECoreVideoCodecEmptyButVideoEnabled";
		break;
	case ECoreNetworkInterfaceUnavailable:
		strErrorInfo = "Core ECoreNetworkInterfaceUnavailable";
		break;
	case ECoreWrongDTMFTone:
		strErrorInfo = "Core ECoreWrongDTMFTone";
		break;
		//add2
	case ECoreWrongLicenseKey:
		strErrorInfo = "Core ECoreWrongLicenseKey";
		break;
	case ECoreTrialVersionLicenseKey:
		strErrorInfo = "Core ECoreTrialVersionLicenseKey";
		break;
	case ECoreOutgoingAudioMuted:
		strErrorInfo = "Core ECoreOutgoingAudioMuted";
		break;
	case ECoreOutgoingVideoMuted:
		strErrorInfo = "Core ECoreOutgoingVideoMuted";
		break;
	case ECoreFailedCreateSdp:
		strErrorInfo = "Core ECoreFailedCreateSdp";
		break;
	case ECoreTrialVersionExpired:
		strErrorInfo = "ECoreTrialVersionExpired";
		break;
	case ECoreStackFailure:
		strErrorInfo = "ECoreStackFailure";
		break;
	case ECoreTransportExists:
		strErrorInfo = "ECoreTransportExists";
		break;
	case ECoreUnsupportTransport:
		strErrorInfo = "ECoreUnsupportTransport";
		break;
	case ECoreAllowOnlyOneUser:
		strErrorInfo = "ECoreAllowOnlyOneUser";
		break;
	case ECoreUserNotFound:
		strErrorInfo = "ECoreUserNotFound";
		break;
	case ECoreTransportsIncorrect:
		strErrorInfo = "ECoreTransportsIncorrect";
		break;
	case ECoreCreateTransportFailure:
		strErrorInfo = "ECoreCreateTransportFailure";
		break;
	case ECoreTransportNotSet:
		strErrorInfo = "ECoreTransportNotSet";
		break;
	case ECoreECreateSignalingFailure:
		strErrorInfo = "ECoreECreateSignalingFailure";
		break;
	case ECoreArgumentIncorrect:
		strErrorInfo = "ECoreArgumentIncorrect";
		break;
	case ECoreSipMethodNameEmpty:
		strErrorInfo = "ECoreSipMethodNameEmpty";
		break;

		//IVR
	case ECoreIVRObjectNull:
		strErrorInfo = "ECoreIVRObjectNull";
		break;
	case ECoreIVRIndexOutOfRange:
		strErrorInfo = "ECoreIVRIndexOutOfRange";
		break;
	case ECoreIVRReferFailure:
		strErrorInfo = "ECoreIVRReferFailure";
		break;
	case ECoreIVRWaitingTimeOut:
		strErrorInfo = "ECoreIVRWaitingTimeOut";
		break;
		// audio
	case EAudioFileNameEmpty:
		strErrorInfo = "Audio File Name Empty";
		break;
	case EAudioChannelNotFound:
		strErrorInfo = "Audio Channel Not Found";
		break;
	case EAudioStartRecordFailure:
		strErrorInfo = "Audio Start Record Failure";
		break;
	case EAudioRegisterRecodingFailure:
		strErrorInfo = "Audio Register Recoding Failure";
		break;
	case EAudioRegisterPlaybackFailure:
		strErrorInfo = "EAudioRegisterPlaybackFailure";
		break;
	case EAudioGetStatisticsFailure:
		strErrorInfo = "Audio Get Statistics Failure";
		break;


		// video
	case EVideoFileNameEmpty:
		strErrorInfo = "Video File NameEmpty";
		break;
	case EVideoGetDeviceNameFailure:
		strErrorInfo = "Video Get Device Name Failure";
		break;
	case EVideoGetDeviceIdFailure:
		strErrorInfo = "Video Get Device Id Failure";
		break;
	case EVideoStartCaptureFailure:
		strErrorInfo = "Video Start Capture Failure";
		break;
	case EVideoChannelNotFound:
		strErrorInfo = "Video Channel Not Found";
		break;
	case EVideoStartSendFailure:
		strErrorInfo = "Video Start Send Failure";
		break;
	case EVideoGetStatisticsFailure:
		strErrorInfo = "Video Get Statistics Failure";
		break;
		// Device
	case EDeviceGetDeviceNameFailure:
		strErrorInfo = "Device Get Device Name Failure";
		break;
	default:
	{
		std::stringstream strCode;
		strCode <<"errorCode:"<< errorCode;
		strErrorInfo = strCode.str();
		break;
	}
		
	}
	return strErrorInfo;
}
void CSystemEx::InitDefaultVideoCodecs()
{
	PortSIP_clearAudioCodec(GetSipLib());
	PortSIP_addVideoCodec(GetSipLib(), VIDEO_CODEC_H264);
	PortSIP_addVideoCodec(GetSipLib(), VIDEO_CODEC_VP8);
	PortSIP_addVideoCodec(GetSipLib(), VIDEO_CODEC_VP9);
}
void CSystemEx::InitDefaultAudioCodecs()
{
	PortSIP_clearAudioCodec(GetSipLib());


	// Default we just used PCMA, PCMU, G729
	//PortSIP_addAudioCodec(GetSipLib(), AUDIOCODEC_PCMA);
	//PortSIP_addAudioCodec(GetSipLib(), AUDIOCODEC_PCMU);
	//PortSIP_addAudioCodec(GetSipLib(), AUDIOCODEC_G729);

	// The DTMF MUST last added
	PortSIP_addAudioCodec(GetSipLib(), AUDIOCODEC_DTMF);
}

void CSystemEx::InitDeviceSettings()
{
	int nLocalAudioInputDeviceID = -1, nLocalOutputAudioDeviceID = -1;
	int nNumSpeakerDevice = 0, nMicDevice = 0,nVideoDevice = 0;
	if (m_pOptions->m_nAudioInputDeviceID!=-1&& m_pOptions->m_nAudioOutputDeviceID !=-1)
	{
		//PortSIP_setAudioDeviceId(GetSipLib(), m_oOptions.m_nAudioInputDeviceID, m_oOptions.m_nAudioOutputDeviceID);
	}
	else if (m_pOptions->m_nAudioInputDeviceID == -1&& m_pOptions->m_nAudioOutputDeviceID != -1)
	{
		nMicDevice = PortSIP_getNumOfRecordingDevices(GetSipLib());
		if (nMicDevice <=0)
		{
			std::string strErrorInfo = "can not find any mic device";
			//TODO MESSAGE BOX
			return;
		}
		m_pOptions->m_nAudioInputDeviceID = 0;
	}
	else if (m_pOptions->m_nAudioInputDeviceID != -1 && m_pOptions->m_nAudioOutputDeviceID == -1)
	{
	
		nNumSpeakerDevice = PortSIP_getNumOfPlayoutDevices(GetSipLib());
		if (nNumSpeakerDevice<=0)
		{
			std::string strErrorInfo = "can not find any speaker device";
			//TODO MESSAGE BOX
			return;
		}
		m_pOptions->m_nAudioOutputDeviceID = 0;
	}//no set
	else
	{
		nMicDevice = PortSIP_getNumOfRecordingDevices(GetSipLib());
		if (nLocalAudioInputDeviceID <= 0)
		{
			std::string strErrorInfo = "can not find any mic device";
			//TODO MESSAGE BOX
			return;
		}
		nNumSpeakerDevice = PortSIP_getNumOfPlayoutDevices(GetSipLib());
		if (nNumSpeakerDevice <= 0)
		{
			std::string strErrorInfo = "can not find any speaker device";
			//TODO MESSAGE BOX
			return;
		}
		m_pOptions->m_nAudioInputDeviceID = 0;
		m_pOptions->m_nAudioOutputDeviceID = 0;
	}
	nLocalAudioInputDeviceID = m_pOptions->m_nAudioInputDeviceID;
	int nLocalAudioOutputDeviceID = m_pOptions->m_nAudioOutputDeviceID;

	if (m_pOptions->m_bEnableAudioInputDev==false)
	{
		nLocalAudioInputDeviceID = -1;
		InitAudioInSwtich(false);
	}
	else
	{
		InitAudioInSwtich(true);
	}
	if (m_pOptions->m_bEnableAudioOutputDev==false)
	{
		nLocalAudioOutputDeviceID = -1;
		InitAudioOutSwtich(false);
	}
	else
	{
		InitAudioOutSwtich(true);
	}

	int nReslut = PortSIP_setAudioDeviceId(GetSipLib(), nLocalAudioInputDeviceID, nLocalAudioOutputDeviceID);
	if (nReslut < 0)
	{
		std::string strErrorInfo = GetSipErrorInfo(nReslut);
		LOG4_INFO(strErrorInfo.c_str());
	}

	int nVideoDeviceID = m_pOptions->m_nVideoDeviceID;
	if (m_pOptions->m_bEnableVideoDev==false)
	{
		nVideoDeviceID = -1;
		InitCameraSwtich(false);
	}
	else
	{
		InitCameraSwtich(true);
	}
	int nErrorCode = PortSIP_setVideoDeviceId(GetSipLib(), nVideoDeviceID);
	if (nErrorCode < 0)
	{
		std::string strError = GetSipErrorInfo(nErrorCode);
		LOG4_INFO(strError.c_str());
		return;
	}
	//GetDBOprate()->updateAVDeviceOptions(m_pOptions);
}

void CSystemEx::InitOhterSettings()
{
	if (m_pOptions->m_bAEC)
	{
		PortSIP_enableAEC(GetSipLib(), EC_DEFAULT);
	}
	else
	{
		PortSIP_enableAEC(GetSipLib(), EC_NONE);
	}
	if (m_pOptions->m_bAGC)
	{
		PortSIP_enableAGC(GetSipLib(), AGC_DEFAULT);
	}
	else
	{
		PortSIP_enableAGC(GetSipLib(), AGC_NONE);
	}

	if (m_pOptions->m_bANS)
	{
		PortSIP_enableANS(GetSipLib(), NS_DEFAULT);
	}
	else
	{
		PortSIP_enableANS(GetSipLib(), NS_NONE);
	}
	PortSIP_enableVAD(GetSipLib(), m_pOptions->m_bVAD);
	PortSIP_enableCNG(GetSipLib(), m_pOptions->m_bCNG);
	TagScreenMetrics tagLocalVideoMetrics = PortUtility::transVideoResolution(m_pOptions->m_nVideoResolution);
	PortSIP_setVideoResolution(GetSipLib(), tagLocalVideoMetrics.nFrameWidth, tagLocalVideoMetrics.nFrameHeight);
	if (m_pOptions->m_nAudioRtpMinPort>=0&&m_pOptions->m_nAudioRtpMaxPort>=0&&m_pOptions->m_nVideoRtpMinPort>=0&&m_pOptions->m_nVideoRtpMaxPort>=0)
	{

		PortSIP_setRtpPortRange(GetSipLib(),
			m_pOptions->m_nAudioRtpMinPort,
			m_pOptions->m_nAudioRtpMaxPort,
			m_pOptions->m_nVideoRtpMinPort,
			m_pOptions->m_nVideoRtpMaxPort);
	}
}

void CSystemEx::InitAudioCodecs()
{
	PortSIP_clearAudioCodec(GetSipLib());

	auto iterFind = m_vecAudioCodec.begin();
	CAVCodec theAVCodec;
	for (iterFind; iterFind != m_vecAudioCodec.end(); iterFind++)
	{
		theAVCodec = *iterFind;
		if (theAVCodec.m_bAVCodecOpen==true)
		{
			PortSIP_addAudioCodec(GetSipLib(), PortUtility::getAudioCodecByName(theAVCodec.m_strCodecName));
		}
	}
	//no set any codec
	if (PortSIP_isAudioCodecEmpty(CSystemEx::GetInstance()->GetSipLib()) == true)
	{
		CSystemEx::GetInstance()->InitDefaultAudioCodecs();
	}

	PortSIP_addAudioCodec(GetSipLib(), AUDIOCODEC_DTMF);
}

void CSystemEx::InitVideoCodecs()
{
	PortSIP_clearVideoCodec(GetSipLib());

	auto iterFind = m_vecVideoCodec.begin();
	CAVCodec theAVCodec;
	for (iterFind; iterFind != m_vecVideoCodec.end(); iterFind++)
	{
		theAVCodec = *iterFind;
		if (theAVCodec.m_bAVCodecOpen == true)
		{
			PortSIP_addVideoCodec(GetSipLib(), PortUtility::getVideoCodecByName(theAVCodec.m_strCodecName));
		}
	}
	PortSIP_setVideoNackStatus(GetSipLib(), true);
}

int CSystemEx::SubContactStatus(std::vector<TAG_RECENT*> vecContactDB)
{
	std::string strSipDomain  = GetGlobalUser()->m_strSipDomain;
	std::vector<TAG_RECENT*>::iterator iterContact = vecContactDB.begin();
	int nRet = 0;
	for (iterContact; iterContact< vecContactDB.end();iterContact++)
	{
		TAG_RECENT* pLocalContactInfo = *iterContact;
		if (pLocalContactInfo ==NULL||pLocalContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
		{
			continue;
		}
		CContactDB* pLocalContact = pLocalContactInfo->m_unionContactInfo.m_pContact;
		nRet = PortSIP_presenceSubscribe(GetSipLib(), pLocalContact->m_strSIPNum.c_str(), "Hello");
	}
	return nRet;
}
PortSQL* CSystemEx::GetDBOprate()
{
	if(m_pDB != NULL)
	{
		if (m_pDB->isConnected()==false)
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
	return m_pDB;
};
long CSystemEx::SendMsg(std::string strSendTo, std::string strMsg)
{
	if (strSendTo.empty()||strMsg.empty())
	{
		return -1;
	}
	

	long lReslut = PortSIP_sendOutOfDialogMessage(GetSipLib(), strSendTo.c_str(), "text", "plain", false, (const unsigned char *)strMsg.c_str(),strMsg.length());
	if (lReslut <0)
	{
		std::string strErrorInfo= GetSipErrorInfo((int)lReslut);
		LOG4_INFO(strErrorInfo.c_str());
		return lReslut;
	}
	return lReslut;
}
long CSystemEx::SendJsonMsg(std::string strSendTo, std::string strJsonMsg)
{
	if (strSendTo.empty() || strJsonMsg.empty())
	{
		return -1;
	}
	long nReslut = PortSIP_sendOutOfDialogMessage(GetSipLib(), strSendTo.c_str(), "application", "json", false, (const unsigned char *)strJsonMsg.c_str(), strJsonMsg.length());
	if (nReslut < 0)
	{
		std::string strErrorInfo = GetSipErrorInfo(nReslut);
		LOG4_INFO(strErrorInfo.c_str());
		return nReslut;
	}
	return nReslut;
}
bool CSystemEx::sendVideoStream(int nCurrentLine,bool state)
{
	if (nCurrentLine>MAX_LINES)
	{
		LOG4_INFO("Current Line Out Of Range");
		return false;
	}
	if (m_SessionArray[nCurrentLine].getCallState() == false)
	{
		return false;
	}

	m_SessionArray[nCurrentLine].setSendVideoState(state);

	int rt = PortSIP_sendVideo(GetSipLib(), m_SessionArray[nCurrentLine].getSessionId(), state);
	if (rt != 0)
	{
		return false;
	}

	return true;
}

int CSystemEx::FindFreeSession()
{
	int i = LINE_BASE;
	for (i = LINE_BASE; i < MAX_LINES; ++i)
	{
		if (m_SessionArray[i].getCallState() == false && m_SessionArray[i].getRecvCallState() == false)
		{
			//m_SessionArray[i].setRecvCallState(FALSE);
			return i;
		}
	}

	return -1;
}
bool CSystemEx::CallOneNumberAudio(std::string strSipNumber, bool bSendSDP , int &nActiveSeesion,  long & nCallHistoryPrimaryKeyID,std::string& strErrorInfo)
{
	if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
	{
		CIUIString strInfoUnicode = CSystemEx::GetInstance()->GetOffLineCanNotDoText();

		std::wstring strInfo = strInfoUnicode.GetBuffer(strInfoUnicode.GetLength());
		strErrorInfo = PortUtility::wstring2String(strInfo);
		return false;
	}
	if (PortSIP_isAudioCodecEmpty(CSystemEx::GetInstance()->GetSipLib()) == true)
	{
		CSystemEx::GetInstance()->InitDefaultAudioCodecs();
	}

	
	
	m_ActiveLine =FindFreeSession();
	nActiveSeesion = m_ActiveLine;

	if (nActiveSeesion == -1)
	{
		std::string strErrorInfo = "active line shoudle not be a available line";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}

	CCallHistory localCallHistory;
	localCallHistory.m_enumCallType = ENUM_CALL_OUT;
	localCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	localCallHistory.m_strSIPNum = strSipNumber;
	localCallHistory.m_enumContentType = CALL_PHONE;
	localCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	localCallHistory.m_bRead = true;
	long sessionId = PortSIP_call(CSystemEx::GetInstance()->GetSipLib(), strSipNumber.c_str(), bSendSDP, false);

	int nRecordPrimaryKeyID = -1;
	GetDBOprate()->saveCallMsg(localCallHistory, nRecordPrimaryKeyID);
	
	nCallHistoryPrimaryKeyID = nRecordPrimaryKeyID;
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;
	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = ENUM_MSG_AUDIO;
	theLocalChatRecord.m_strTalkerURL = strSipNumber + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_READ;
	GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	
	RfreshData2CInboxCanvasHandler();
	if (sessionId <= 0)
	{
		strErrorInfo = CSystemEx::GetInstance()->GetSipErrorInfo(sessionId);
		LOG4_INFO(strErrorInfo.c_str());
	
		m_ActiveLine = -1;
		nActiveSeesion = m_ActiveLine;
		
		return false;
	}
	else
	{
		m_SessionArray[m_ActiveLine].setSessionId(sessionId);
		m_SessionArray[m_ActiveLine].m_oSessionInfo.setState(true);
		m_SessionArray[m_ActiveLine].setCallState(true);


		bool bEnableAudioInputDev = CSystemEx::GetInstance()->GetAudioInEnable();
		bool bEnableAudioOutputDev = CSystemEx::GetInstance()->GetAudioOutEnable();
		bool bEnableVideoDev = CSystemEx::GetInstance()->GetCameraEnable();
		CSystemEx::GetInstance()->MuteSession(!bEnableAudioOutputDev, !bEnableAudioInputDev, !bEnableVideoDev, sessionId);
	}
	return true;
}
bool CSystemEx::CallOneNumberVideo(std::string strSipNumber, bool bSendSDP, int &nActiveSeesion, long& nCallHistoryPrimaryKeyID, std::string& strErrorInfo, HWND hRemoteVideoHwnd, HWND hLoclVideoHwnd,bool bConfence )
{
#if _DEBUG
	TIMER_START(CallOneNumberVideo);
#endif
	if (CSystemEx::GetInstance()->GetInitPortSip() == false || CSystemEx::GetInstance()->GetRegister() == false)
	{
		CIUIString strInfoUnicode = CSystemEx::GetInstance()->GetOffLineCanNotDoText();

		std::wstring strInfo = strInfoUnicode.GetBuffer(strInfoUnicode.GetLength());
		strErrorInfo = PortUtility::wstring2String(strInfo);
		LOG4_ERROR(strErrorInfo.c_str());
		return false;
	}

	if (PortSIP_isVideoCodecEmpty(CSystemEx::GetInstance()->GetSipLib()) == true)
	{
		CSystemEx::GetInstance()->InitDefaultAudioCodecs();
	}
	
	if (hRemoteVideoHwnd==NULL)
	{
		std::string strErrorInfo = "RemoteVideoHwnd  shoudle not be NULL ";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	
	m_ActiveLine = FindFreeSession();
	nActiveSeesion = m_ActiveLine;

	if (nActiveSeesion == -1)
	{
		std::string strErrorInfo = "active line shoudle not be a available line";
		LOG4_INFO(strErrorInfo.c_str());
	}

	CCallHistory localCallHistory;
	localCallHistory.m_enumCallType = ENUM_CALL_OUT;
	localCallHistory.m_enumCallStatus = ENUM_CALL_STATUS_FAILD;
	localCallHistory.m_strSIPNum = strSipNumber;
	localCallHistory.m_enumContentType = CALL_VIDEO;
	localCallHistory.m_bRead = true;
	localCallHistory.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	long sessionId = PortSIP_call(CSystemEx::GetInstance()->GetSipLib(), strSipNumber.c_str(), bSendSDP, true);

	
	int nRecordPrimaryKeyID = -1;
	GetDBOprate()->saveCallMsg(localCallHistory, nRecordPrimaryKeyID);
	nCallHistoryPrimaryKeyID = nRecordPrimaryKeyID;
	std::stringstream strMsgID;
	strMsgID << nRecordPrimaryKeyID;
	CChatRecord theLocalChatRecord;
	theLocalChatRecord.m_nIsMyself = true;
	theLocalChatRecord.m_strMSG = strMsgID.str();
	theLocalChatRecord.m_nMSGType = ENUM_MSG_AUDIO;
	theLocalChatRecord.m_strTalkerURL = strSipNumber + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
	theLocalChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	theLocalChatRecord.m_nMSGStatus = ENUM_MSG_STATUS_READ;
	GetDBOprate()->saveOneCallRecord(theLocalChatRecord, nRecordPrimaryKeyID);
	
	RfreshData2CInboxCanvasHandler();
	if (sessionId <= 0)
	{
		strErrorInfo = CSystemEx::GetInstance()->GetSipErrorInfo(sessionId);
		LOG4_INFO(strErrorInfo.c_str());
		m_ActiveLine = -1;
		nActiveSeesion = m_ActiveLine;
		return false;
	}
	else
	{
		m_SessionArray[m_ActiveLine].setSessionId(sessionId);
		m_SessionArray[m_ActiveLine].m_oSessionInfo.setState(true);
		m_SessionArray[m_ActiveLine].setCallState(true);
		int nBitRate = PortUtility::transVideoBitrate(m_pOptions->m_nVideoImageQualityLevel);
		PortSIP_setVideoBitrate(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId(), nBitRate);
		CVideoCanvasHandler *pVideoCnavasHandler = GetVideoCanvasHandler(m_ActiveLine);
		if (bConfence != true)
		{
			PortSIP_setRemoteVideoWindow(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId(), hRemoteVideoHwnd);
		}
		
		if (hLoclVideoHwnd !=NULL)
		{
			PortSIP_setLocalVideoWindow(GetSipLib(), hLoclVideoHwnd);
		}
		//PortSIP_setLocalVideoWindow(GetSipLib(), hRemoteVideoHwnd);
		PortSIP_sendVideo(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId(), true);

		bool bEnableAudioInputDev = CSystemEx::GetInstance()->GetAudioInEnable();
		bool bEnableAudioOutputDev = CSystemEx::GetInstance()->GetAudioOutEnable();
		bool bEnableVideoDev = CSystemEx::GetInstance()->GetCameraEnable();
		CSystemEx::GetInstance()->MuteSession(!bEnableAudioOutputDev, !bEnableAudioInputDev, !bEnableVideoDev, sessionId);
	}
#if _DEBUG
	TIMER_STOP(CallOneNumberVideo);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" CallOneNumberVideo Cost %d ms.\n", TIMER_MSEC(CallOneNumberVideo));
	OutputDebugString(strFormatInfo);
#endif
	return true;
}
bool CSystemEx::StopLocalVideoHwnd(HWND hWndLocalVideo, bool bVideoDeviceConf, std::string &strErrorInfo)
{
	if (hWndLocalVideo == NULL && m_mapLocalVideoHwnd.size() > 0)
	{
		return false;
	}
	if (hWndLocalVideo == NULL && m_mapLocalVideoHwnd.size() <= 0)
	{
		PortSIP_displayLocalVideo(CSystemEx::GetInstance()->GetSipLib(), false,true);
		PortSIP_setLocalVideoWindow(GetSipLib(), NULL);
		LOG4_INFO("Truely realse dev2\n");
		return true;
	}
	if (bVideoDeviceConf&&m_mapLocalVideoHwnd.size() > 0)
	{
		strErrorInfo = "Making Video Calls Now";
		return false;
	}
	
	CCriticalAutoLock loLock(m_oLockLocalVideo);
	auto iterLocalVideo = m_mapLocalVideoHwnd.find(hWndLocalVideo);
	if (iterLocalVideo!= m_mapLocalVideoHwnd.end())
	{//
		m_mapLocalVideoHwnd.erase(iterLocalVideo);
	}
	if (m_mapLocalVideoHwnd.size() ==0)
	{//
		PortSIP_displayLocalVideo(GetSipLib(), false,true);
		PortSIP_setLocalVideoWindow(GetSipLib(), NULL);
		LOG4_INFO("Truely realse dev3\n");
	}
	return false;

}

bool CSystemEx::SetLocalVideoHwnd(HWND hWndLocalVideo,bool bVideoDeviceConf,std::string &strErrorInfo)
{
	if (hWndLocalVideo==NULL)
	{
		//PortSIP_setLocalVideoWindow(GetSipLib(), hWndLocalVideo);
		//PortSIP_displayLocalVideo(GetSipLib(), false);
		return false;
	}
	bool bRet = false;
	CCriticalAutoLock lolock(m_oLockLocalVideo);
	if (m_mapLocalVideoHwnd.size() > 0 && bVideoDeviceConf)
	{
		strErrorInfo = "Making Video Calls Now";
		bRet =  false;
		return false;
	}
	else
	{
		bRet = true;
	}
	if (bVideoDeviceConf==false)
	{
		m_mapLocalVideoHwnd.insert(make_pair(hWndLocalVideo,bVideoDeviceConf));
	}
	if (bRet)
	{
		PortSIP_displayLocalVideo(GetSipLib(), false,true);
		PortSIP_setLocalVideoWindow(GetSipLib(), hWndLocalVideo);
		PortSIP_displayLocalVideo(GetSipLib(), true,true);
	}
	return bRet;
}
bool CSystemEx::OnStartPlayAudiofile(std::string strFilePath)
{
	int nReslut =PortSIP_playAudioFileToRemote(GetSipLib(),
		m_SessionArray[m_ActiveLine].getSessionId(),
		strFilePath.c_str(),
		16000,
		false);

	if (nReslut<0)
	{
		std::string strError = GetSipErrorInfo(nReslut);
		LOG4_INFO(strError.c_str());
		return false;
	}
	else
	{
		return true;
	}
}

bool CSystemEx::OnStopPlayAudiofile(std::string strFilePath)
{
	if (m_SessionArray[m_ActiveLine].getCallState() == false)
	{
		return false;
	}

	int nReslut = PortSIP_stopPlayAudioFileToRemote(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId());
	if (nReslut < 0)
	{
		std::string strError = GetSipErrorInfo(nReslut);
		LOG4_INFO(strError.c_str());
		return false;
	}
	else
	{
		return true;
	}
}

bool CSystemEx::SendVideo(int nActiveSeesion)
{
	if (GetCameraEnable()==false)
	{
		return false;
	}
	if (m_SessionArray[nActiveSeesion].getCallState() == false && m_SessionArray[nActiveSeesion].getRecvCallState() == false)
	{
		return false;
	}

	int nReslut = PortSIP_sendVideo(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId(), true);
	if (nReslut < 0)
	{
		std::string strError = GetSipErrorInfo(nReslut);
		LOG4_INFO(strError.c_str());
		return false ;
	}
	else
	{
		return true;
	}
}

void CSystemEx::StopVideo(int nActiveSeesion)
{
	if (m_SessionArray[nActiveSeesion].getCallState() == false)
	{
		return;
	}

	int nReslut = PortSIP_sendVideo(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId(), false);
	if (nReslut < 0)
	{
		std::string strError = GetSipErrorInfo(nReslut);
		LOG4_INFO(strError.c_str());
		return;
	}
	else
	{
		return;
	}
}
void CSystemEx::ProcessTransfer(int nActiveSession, bool bSucess, int nErrorCode)
{
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = GetAudioPageCanvasHandler(nActiveSession);
	if (pAudioPageCanvasHandler != NULL)
	{
		pAudioPageCanvasHandler->ProcessTranseferSuccess(bSucess,  nErrorCode);
		return;
	}
	CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler(nActiveSession);
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->ProcessTranseferSuccess(bSucess,  nErrorCode);
		return;
	}
}
void CSystemEx::HangupByRemote(int nActiveSession,bool bConnected,bool bJumpPage)
{
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = GetAudioPageCanvasHandler(nActiveSession);
	if (pAudioPageCanvasHandler!=NULL)
	{
		pAudioPageCanvasHandler->HangupByRemote(bConnected,nActiveSession, bJumpPage);
		EraseSession2CanvasHandler(nActiveSession);
		 return;
	}
	CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler(nActiveSession);
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->HangupByRemote(bConnected, nActiveSession, bJumpPage);
		EraseSession2CanvasHandler(nActiveSession);
		return;
	}
	//if (pVideoCanvasHandler==NULL&&pAudioPageCanvasHandler==NULL)
	{
		CSystemEx::GetCallerBarWndMGR().CloseByRemote(nActiveSession);
	}
	
}

void CSystemEx::HoldByRemote(int nActiveSession)
{
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = GetAudioPageCanvasHandler(nActiveSession);
	if (pAudioPageCanvasHandler != NULL)
	{
		pAudioPageCanvasHandler->HoldByRemote();
		return;
	}
	CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler(nActiveSession);
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->HoldByRemote();
		return;
	}
}

void CSystemEx::UnHoldByRemote(int nActiveSession)
{
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = GetAudioPageCanvasHandler(nActiveSession);
	if (pAudioPageCanvasHandler != NULL)
	{
		pAudioPageCanvasHandler->UnHoldByRemote();
		return;
	}
	CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler(nActiveSession);
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->UnHoldByRemote();
		return;
	}
}

void CSystemEx::AnswerByRemote(int nActiveSession,  bool bConferenceRoom)
{
	CAudioPageCanvasHandler *pAudioPageCanvasHandler = GetAudioPageCanvasHandler(nActiveSession);
	if (pAudioPageCanvasHandler != NULL)
	{
		pAudioPageCanvasHandler->AnswerByRemote(nActiveSession, bConferenceRoom);
		return;
	}
	CVideoCanvasHandler *pVideoCanvasHandler = GetVideoCanvasHandler(nActiveSession);
	if (pVideoCanvasHandler)
	{
		pVideoCanvasHandler->AnswerByRemote(nActiveSession, bConferenceRoom);
		return;
	}
}
CString CSystemEx::RefreshCallTime(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oSessionLock);
	if (nActiveSession >= MAX_LINES)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return L"";
	}
	if (nActiveSession < LINE_BASE)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return L"";
	}
	return m_SessionArray[nActiveSession].refreshCallTimer();
}
void CSystemEx::ReSetCallTime(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oSessionLock);
	if (nActiveSession >= MAX_LINES)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return ;
	}
	if (nActiveSession < LINE_BASE)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return ;
	}
	m_SessionArray[nActiveSession].resetCallTimer();
}
void CSystemEx::Answer(bool bHasVideo, int nActiveSeesion,HWND hWnd)
{
	CCriticalAutoLock loLock(m_oSessionLock);
	if (m_SessionArray[nActiveSeesion].getRecvCallState() == false)
	{
		LOG4_INFO("Current line does not has incoming call, please switch a line.");
		return;
	}
	m_SessionArray[nActiveSeesion].setCallState(true);
	m_SessionArray[nActiveSeesion].setRecvCallState(false);

	PortSIP_setRemoteVideoWindow(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId(), hWnd);
	if (PortSIP_answerCall(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId(), bHasVideo) == 0)
	{
		stringstream log;
		log << "Answered the call on line " << nActiveSeesion << "";
		LOG4_INFO(log.str().c_str());
		//joinConference(nActiveSeesion);
	}
	else
	{
		m_SessionArray[m_ActiveLine].reset();

		stringstream log;
		log << "Answer call failed on line " << m_ActiveLine << "";
		LOG4_INFO(log.str().c_str());
	}
}
bool CSystemEx::GetCallState(int nActiveSeesion)
{
	CCriticalAutoLock loLock(m_oSessionLock);
	if (nActiveSeesion >= MAX_LINES)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	if (nActiveSeesion < LINE_BASE)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	return m_SessionArray[nActiveSeesion].getCallState();
}
void CSystemEx::Hangup(int nActiveSeesion)
{
	CCriticalAutoLock loLock(m_oSessionLock);
	if (nActiveSeesion>=MAX_LINES)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	if (nActiveSeesion< LINE_BASE)
	{
		std::string strErrorInfo = "range of max lines";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	if (m_SessionArray[nActiveSeesion].getCallState() == true)
	{
		PortSIP_hangUp(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId());
		m_SessionArray[nActiveSeesion].reset();

		stringstream log;
		log << "Hang up call on line " << nActiveSeesion << "";
		std::string strErrorInfo = log.str();
		LOG4_INFO(strErrorInfo.c_str());
	}
	else if (m_SessionArray[nActiveSeesion].getRecvCallState() == true)
	{
		PortSIP_rejectCall(GetSipLib(), m_SessionArray[nActiveSeesion].getSessionId(), 486);
		m_SessionArray[nActiveSeesion].reset();

		stringstream log;
		log << "Rejected call on line " << m_ActiveLine << "";
		std::string strErrorInfo = log.str();
		LOG4_INFO(strErrorInfo.c_str());
	}
	else 
	{
	}
}
void CSystemEx::Hangup()
{
	Hangup(m_ActiveLine);
}
/*void CSystemEx::MuteMicrophine(bool bMute)
{
	PortSIP_muteMicrophone(GetSipLib(), bMute);
}*/
void CSystemEx::MuteMicrophineSession(bool bMute, long nSessionID)
{
	if (GetAudioOutEnable()==false)
	{
		return;
	}
	PortSIP_muteSession(GetSipLib(), nSessionID, false, bMute, false, false);
}

void CSystemEx::MuteSession(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera, long nSessionID)
{
	PortSIP_muteSession(GetSipLib(), nSessionID, bEnableAudioOut, bEnableAudioIn, false, false);

	bool bSend = !bEnableCamera;
	PortSIP_sendVideo(GetSipLib(), nSessionID, bSend);
}
void CSystemEx::Hold(int nAcitiveLine, bool bAutoHold)
{
	if (m_SessionArray[nAcitiveLine].getCallState() == false)
	{
		return;
	}

	if (m_SessionArray[nAcitiveLine].getHoldState() == true)
	{
		return;
	}

	PortSIP_hold(GetSipLib(), m_SessionArray[nAcitiveLine].getSessionId());
	m_SessionArray[nAcitiveLine].setHoldState(true);
	std::stringstream ssHoldLineInfo;
	std::string strHoldInfo;
	ssHoldLineInfo << "Set  Hold Line :" << nAcitiveLine ;
	strHoldInfo = ssHoldLineInfo.str();
	LOG4_INFO(strHoldInfo.c_str());
}


void CSystemEx::UnHold(int nAcitiveLine, bool bAutoHold)
{
	if (m_SessionArray[nAcitiveLine].getCallState() == false)
	{
		return;
	}


	if (m_SessionArray[nAcitiveLine].getHoldState() == false)
	{
		return;
	}

	PortSIP_unHold(GetSipLib(), m_SessionArray[nAcitiveLine].getSessionId());
	m_SessionArray[nAcitiveLine].setHoldState(false);
	std::stringstream ssHoldLineInfo;
	std::string strHoldInfo;
	ssHoldLineInfo << "Set UN Hold Line :" << nAcitiveLine;
	strHoldInfo = ssHoldLineInfo.str();
	LOG4_INFO(strHoldInfo.c_str());
}
long CSystemEx::GetSessionID(int index)
{
	return m_SessionArray[index].getSessionId();
}
bool CSystemEx::joinConference(int index)
{
	long sessionId = m_SessionArray[index].getSessionId();
	PortSIP_setRemoteVideoWindow(GetSipLib(), sessionId, NULL);

	int rt = PortSIP_joinToConference(GetSipLib(), sessionId);
	if (rt!=0)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "join to confenrence error: " << rt << "Index:" << index;
		LOG4_INFO(strErrorInfo.str().c_str());
		std::string strError = GetSipErrorInfo(rt);
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Error", PortUtility::string2WString(strError).c_str(), NULL);

		return false;
	}

	// We need to unhold the line in conference
	if (m_SessionArray[index].getCallState())
	{
		PortSIP_unHold(GetSipLib(), sessionId);
		m_SessionArray[index].setHoldState(false);
		std::stringstream ssHoldLineInfo;
		std::string strHoldInfo;
		ssHoldLineInfo << "Set UN Hold Line :" << index;
		strHoldInfo = ssHoldLineInfo.str();
		LOG4_INFO(strHoldInfo.c_str());
	}

	return true;
}

int CSystemEx::FindSessionBySessionID(int nSessionID)
{
	int index = -1;
	for (int i = LINE_BASE; i < MAX_LINES; ++i)
	{
		if (nSessionID == m_SessionArray[i].getSessionId())
		{
			index = i;
			break;
		}
	}
	return index;
}

int CSystemEx::ProcessComingCall(ICallbackParameters * parameters,std::string strUserName)
{
	int index = FindFreeSession();
	if (index <= 0)
	{
		PortSIP_rejectCall(GetSipLib(), parameters->getSessionId(), 486);
		//todo 
		return -1;
	}

	const char* caller = parameters->getCaller();

	m_SessionArray[index].m_oSessionInfo.startCall();
	m_SessionArray[index].m_oSessionInfo.setCaller(PortUtility::ansi2Unicode(caller));
	m_SessionArray[index].m_oSessionInfo.setCallee(PortUtility::ansi2Unicode(strUserName.c_str()));
	m_SessionArray[index].m_oSessionInfo.setCallType(SessionInfo::call_in);

	m_SessionArray[index].setHasVideo(parameters->getExistsVideo());
	string temp = caller;
	string callName = temp;
	int pos = temp.find("@");
	if (pos != string::npos)
	{
		callName = temp.substr(0, pos);
	}

	const char* displayName= parameters->getCallerDisplayName();


	if (GetLocalStatus() == ENUM_NODISTURB)
	{
		PortSIP_rejectCall(GetSipLib(), parameters->getSessionId(), 486);
		m_SessionArray[index].setRecvCallState(false);
		return -1;
	}
	
	m_SessionArray[index].setRecvCallState(true);
	bool needignoreAutoAnswer = false;
	for (int i = LINE_BASE; i < MAX_LINES; ++i)
	{
		if (m_SessionArray[i].getCallState() == true)
		{
			needignoreAutoAnswer = true;
			break;
		}
	}
	m_SessionArray[index].setSessionId(parameters->getSessionId());
	
	if (needignoreAutoAnswer == false && IsAutoAnswer() == true)
	{
		AutoAnswer(index, parameters);
		return index;
	}
	/*
	//todo 
	if (index == m_currentLine && needignoreAutoAnswer == false)
	{
		playDTMF('i');
		SetTimer(INCOMING_CALL_TIMER, 4000, 0);
	}*/
	return index;
}

bool CSystemEx::AutoAnswer(int nSessionIndex, ICallbackParameters * parameters)
{
	if (m_SessionArray[m_ActiveLine].getRecvCallState() == false)
	{
		return false;
	}
	::KillTimer(NULL,INCOMING_CALL_TIMER);


	m_SessionArray[m_ActiveLine].setCallState(true);
	m_SessionArray[m_ActiveLine].setRecvCallState(false);

	const char* caller = parameters->getCaller();
	HWND  handlerOfVideoWindow = NULL;
	handlerOfVideoWindow = GetMainDlg()->GetHwndBySipNumber(caller);
	if (handlerOfVideoWindow==NULL)
	{
		m_SessionArray[m_ActiveLine].setCallState(false);
		m_SessionArray[m_ActiveLine].setRecvCallState(true);
		return false;
	}
	PortSIP_setRemoteVideoWindow(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId(), handlerOfVideoWindow);
	PortSIP_answerCall(GetSipLib(), m_SessionArray[m_ActiveLine].getSessionId(), true);

	m_SessionArray[m_ActiveLine].m_oSessionInfo.startMediaStream();
	switch (m_ActiveLine)
	{
	case 1:
		::SetTimer(NULL, CALLSESSION_TIMER1, 1000, TimerProc);
		break;

	case 2:
		::SetTimer(NULL, CALLSESSION_TIMER2, 1000, TimerProc);
		break;

	case 3:
		::SetTimer(NULL, CALLSESSION_TIMER3, 1000, TimerProc);
		break;

	case 4:
		::SetTimer(NULL, CALLSESSION_TIMER4, 1000, TimerProc);
		break;

	case 5:
		::SetTimer(NULL,CALLSESSION_TIMER5, 1000, TimerProc);
		break;

	}
	return true;
}

void CSystemEx::ProcessTimer(UINT idEvent, UINT uMsg)
{
	switch (idEvent)
	{
	default:
		break;
	}
	static int nTestPrintfTimerInfo = 0;
	if (nTestPrintfTimerInfo==100)
	{
		std::stringstream strErrorInfo;
		strErrorInfo << "id: " << idEvent << "MSG:" << uMsg;
		LOG4_INFO(strErrorInfo.str().c_str());
	}
}
bool CSystemEx::ReadOptions()
{
	bool bRet = false;
	CSystemEx::GetInstance()->GetDBOprate()->getOptions(m_pOptions, bRet);


	
	return bRet;
}
void CSystemEx::InitUserOptions()
{
	if (m_pOptions==NULL)
	{
		std::string strErrorInfo = "get local opntions failed";
		LOG4_INFO(strErrorInfo.c_str());
		return;
	}
	//bool bRet = false;
	//CSystemEx::GetInstance()->GetDBOprate()->getOptions(m_pOptions,bRet);
	//here we check record path ,if no exist path we create this floder
	if (m_pOptions->m_strRecordFolder.empty()==false)
	{
		PortUtility::CheckFilePath(m_pOptions->m_strRecordFolder);
	}
	InitAudioCodecsFormDB();
	InitUserLocalSIPSettings();

}

void CSystemEx::InitAudioCodecsFormDB()
{
	m_vecAudioCodec = GetDBOprate()->GetCodec(ENUM_VA_CODEC_AUDIO);
	m_vecVideoCodec = GetDBOprate()->GetCodec(ENUM_VA_CODEC_VIDEO);

	std::sort(m_vecAudioCodec.begin(), m_vecAudioCodec.end(), CodecCompare);
	std::sort(m_vecVideoCodec.begin(), m_vecVideoCodec.end(), CodecCompare);
}

vector <CAVCodec> CSystemEx::GetAudioCodec()
{
	return m_vecAudioCodec;
}
vector <CAVCodec> CSystemEx::GetVideoCodec()
{
	return m_vecVideoCodec;
}

CAVCodec CSystemEx::GetCodecByID(int nCodecID, vector<CAVCodec>::iterator &itFind)
{
	auto iterFind = m_vecAudioCodec.begin();
	CAVCodec theAVCodec;
	for (iterFind; iterFind != m_vecAudioCodec.end();iterFind++)
	{
		theAVCodec = *iterFind;
		
		if (theAVCodec.m_nAVCodecID == nCodecID)
		{
			itFind = iterFind;
			return theAVCodec;
		}
	}

	iterFind = m_vecVideoCodec.begin();

	for (iterFind; iterFind != m_vecVideoCodec.end(); iterFind++)
	{
		 theAVCodec = *iterFind;

		if (theAVCodec.m_nAVCodecID == nCodecID)
		{
			itFind = iterFind;
			return theAVCodec;
		}
	}
	itFind = iterFind;
	return theAVCodec;
}
int CSystemEx::GetAudioCodecOpen()
{
	CAVCodec theAVCodec;
	int nIndex = 0;
	auto iterFind = m_vecAudioCodec.begin();
	for (iterFind; iterFind != m_vecAudioCodec.end(); iterFind++)
	{
		theAVCodec = *iterFind;

		if (theAVCodec.m_bAVCodecOpen == true)
		{
			nIndex++;
		}
	}
	return nIndex;
}

void CSystemEx::RefreshCodecInfoSwitch(bool bOpen, int nCodecID)
{
	vector<CAVCodec>::iterator iterFind;
	CAVCodec theLocalCodes = GetCodecByID(nCodecID, iterFind);
	switch (theLocalCodes.m_nAVCodecType)
	{
	case ENUM_VA_CODEC_AUDIO :
		if (iterFind != m_vecAudioCodec.end())
		{
			iterFind->m_bAVCodecOpen = bOpen;
		}
		break;
	case ENUM_VA_CODEC_VIDEO :
		if (iterFind != m_vecVideoCodec.end())
		{
			iterFind->m_bAVCodecOpen = bOpen;
		}
		break;
	}
}

void CSystemEx::RefreshCodecInfoItemIndex(int nItemIndex, int nCodecID)
{
	vector<CAVCodec>::iterator iterFind;
	CAVCodec theLocalCodes = GetCodecByID(nCodecID, iterFind);
	switch (theLocalCodes.m_nAVCodecType)
	{
	case ENUM_VA_CODEC_AUDIO:
		if (iterFind != m_vecAudioCodec.end())
		{
			iterFind->m_nItemIndex = nItemIndex;
		}
		std::sort(m_vecAudioCodec.begin(),m_vecAudioCodec.end(),CodecCompare);
		break;
	case ENUM_VA_CODEC_VIDEO:
		if (iterFind != m_vecVideoCodec.end())
		{
			iterFind->m_nItemIndex = nItemIndex;
		}
		std::sort(m_vecVideoCodec.begin(), m_vecVideoCodec.end(), CodecCompare);
		break;
	}

}
void CSystemEx::UpdateAudioCodec2DB()
{
	auto iterFind = m_vecAudioCodec.begin();
	CAVCodec theAVCodec;
	for (iterFind; iterFind != m_vecAudioCodec.end(); iterFind++)
	{
		theAVCodec = *iterFind;
		GetDBOprate()->UpdateCodecInfo(theAVCodec.m_nItemIndex, theAVCodec.m_bAVCodecOpen, theAVCodec.m_nAVCodecID);
	}
}
void CSystemEx::UpdateVideoCodec2DB()
{
	auto iterFind = m_vecVideoCodec.begin();
	CAVCodec theAVCodec;
	for (iterFind; iterFind != m_vecVideoCodec.end(); iterFind++)
	{
		theAVCodec = *iterFind;
		GetDBOprate()->UpdateCodecInfo(theAVCodec.m_nItemIndex, theAVCodec.m_bAVCodecOpen, theAVCodec.m_nAVCodecID);
	}
}
void CSystemEx::RfreshData2CInboxCanvasHandler()
{
}

void CSystemEx::UpdateCallRecordByIndex(int nIndex)
{
	int nCallRecordID = m_SessionArray[m_ActiveLine].getPrimaryKey();
	if (nCallRecordID!=-1)
	{
	}
	else
	{
		std::string strErrorInfo = "Primart key has something worry";
		LOG4_INFO(strErrorInfo.c_str());
	}
}

bool CSystemEx::Transfer(std::string strTransferSipNumber, int nActiveLine,std::string &strErrorInfo)
{
	if (m_SessionArray[nActiveLine].m_oSessionInfo.getState() == false)
	{
		strErrorInfo = "Need to make the call established first";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	int errorCodec = PortSIP_refer(GetSipLib(), m_SessionArray[nActiveLine].getSessionId(), strTransferSipNumber.c_str());
	if (errorCodec != 0)
	{
		strErrorInfo = GetSipErrorInfo(errorCodec);
		LOG4_INFO(strErrorInfo.c_str());
		//strErrorInfo = "PortSIP_refer failed";
		return false;
	}
	stringstream log;
	log << "Transfer the call on line " << m_ActiveLine << "";
	LOG4_INFO(log.str().c_str());
	return true;
}
bool CSystemEx::AttendTransfer(int nTransferedLine, int nTransferLine, std::string strTransferNumber, std::string &strErrorInfo)
{
	int errorCodec = PortSIP_attendedRefer(GetSipLib(), m_SessionArray[nTransferLine].getSessionId(), m_SessionArray[nTransferedLine].getSessionId(), strTransferNumber.c_str());
	if (errorCodec != 0)
	{
		strErrorInfo = GetSipErrorInfo(errorCodec);
		LOG4_INFO(strErrorInfo.c_str());
		strErrorInfo = "PortSIP_refer failed";
		return false;
	}
	return true;
}

void CSystemEx::SetSRTPMode()
{
	SRTP_POLICY enumSRTPMode = (SRTP_POLICY)GetGlobalUser()->m_nSrtpMode;
	PortSIP_setSrtpPolicy(GetSipLib(), enumSRTPMode, true);
}
string  CSystemEx::ConstructTextMessage(std::string strText)
{
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_TEXT;
	info[key_G_TEXT_CONTENT] = strText;
	std::string strInParam = writer.write(info);
	return strInParam;
}

std::string CSystemEx::ConstructFileMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize)
{
	auto strExtension = PortUtility::getFileExtension(strFileName);
	auto strFileMimeType = PortUtility::getMimeType(strExtension);

	std::wstring wstrFileName = PortUtility::string2WString(strFileName);
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_FILE;
	info[key_G_FILE_NAME] = PortUtility::Unicode2Utf8_ND(wstrFileName) ;
	info[key_G_MIME] = strFileMimeType;
	info[key_G_FILE_URL] = strURL;
	info[key_G_FILE_SIZE] = lFileSize;
	info[key_G_FILE_PATH] = "/storage/emulated";
	std::string strInParam = writer.write(info);
	return strInParam;
}
std::string CSystemEx::ConstructImageMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int width, int height)
{
	auto strExtension = PortUtility::getFileExtension(strFileName);
	auto strFileMimeType = PortUtility::getMimeType(strExtension);

	std::wstring wstrFileName = PortUtility::string2WString(strFileName);
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_IMAGE;
	info[key_G_FILE_NAME] = PortUtility::Unicode2Utf8_ND(wstrFileName);
	info[key_G_MIME] = strFileMimeType;
	info[key_G_FILE_URL] = strURL;
	info[key_G_FILE_SIZE] = lFileSize;
	info[key_G_RESOLUTION_HEIGHT] = width;
	info[key_G_RESOLUTION_WIDTH] = height;
	info[key_G_FILE_PATH] = "/storage/emulated";
	std::string strInParam = writer.write(info);
	return strInParam;
}
std::string CSystemEx::ConstructEmojiMessage(std::string strEmojiType)
{
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_EMOJI;
	info[key_G_MIME] = strEmojiType;
	std::string strInParam = writer.write(info);
	return strInParam;
}
std::string CSystemEx::ConstructVideoMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int nDuration)
{
	std::wstring wstrFileName = PortUtility::string2WString(strFileName);
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_VIDEO;
	info[key_G_FILE_NAME] = PortUtility::Unicode2Utf8_ND(wstrFileName);
	info[key_G_MIME] = "video/mp4";
	info[key_G_FILE_URL] = strURL;
	info[key_G_FILE_SIZE] = lFileSize;
	info[key_G_AV_DURATION] = nDuration;
	std::string strInParam = writer.write(info);
	return strInParam;
}
std::string CSystemEx::ConstructShakeMessage()
{
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_SHAKE;
	std::string strInParam = writer.write(info);
	return strInParam;
}
std::string CSystemEx::ConstructAudioMessage(std::string strFileName, std::string strURL, std::string strMimeType, long lFileSize, int nDuration)
{
	std::wstring wstrFileName = PortUtility::string2WString(strFileName);
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_MESSAGE_TYPE] = MESSAGE_TYPE_AUDIO;
	info[key_G_FILE_NAME] = PortUtility::Unicode2Utf8_ND(wstrFileName);
	info[key_G_MIME] = "audio/wav";
	info[key_G_FILE_URL] = strURL;
	info[key_G_FILE_SIZE] = lFileSize;
	info[key_G_AV_DURATION] = nDuration;
	std::string strInParam = writer.write(info);
	return strInParam;
}

unsigned int CSystemEx::_DoUpLoadThread(void* pParam)
{
	if (pParam == NULL)
	{
		return 0;
	}
	CSystemEx *pThis = (CSystemEx *)pParam;
	pThis->DoUpLoad();
	return 1;
}

unsigned int CSystemEx::_DoDownLoadThread(void* pParam)
{
	if (pParam == NULL)
	{
		return 0;
	}
	CSystemEx *pThis = (CSystemEx *)pParam;
	pThis->DoDownLoad();
	return 1;
}
unsigned int CSystemEx::_DoProcessVoiceMail(void *pParam)
{
	if (pParam==NULL)
	{
		return 0;
	}
	CSystemEx *pThis = (CSystemEx *)pParam;
	pThis->DoVoiceMailPoll();
	return 1;
}
void CSystemEx::DoVoiceMailPoll()
{
	while (true)
	{
		::Sleep(1);
		long nowTime = clock();		
		double endtime = (double)(nowTime - m_lLastCheckTime) / CLOCKS_PER_SEC;

		if (m_bVoiceMailActive == false)
		{
			break;
		}
		int nEexpendTime = (int)endtime + REST_API_TIME_EXPIRES_STEP - (m_nVoiceMailCheckTime*60);
		if (nEexpendTime >= 0|| m_lLastCheckTime==0)
		{
			
			vector<VoiceMailParam> vecVoiceMail;
			m_pRestAPI->ListRecordVoiceMail(vecVoiceMail);
			ProcessVoiceMailGetInfo(vecVoiceMail);
			m_lLastCheckTime = clock();
		}
		
	}
	m_bExitVoiceLoadThread = true;
}
void CSystemEx::DoDownLoad()
{
	while (true)
	{
		::Sleep(1);
		WaitForSingleObject(m_hDownLoadEvent, 15000);

		CCriticalAutoLock loLock(m_oFileDownLoadLock);
		auto iterDownload = m_vecFileDownLoad.begin();
		UpLoadDownLoadParam  *pDownParam = NULL;;
	
		if (m_bDownLoadActive==false)
		{
			break;
		}
		for (iterDownload; iterDownload != m_vecFileDownLoad.end(); )
		{
			
			pDownParam = *iterDownload;
			if (pDownParam==NULL)
			{
				iterDownload++;
				continue;;
			}
			if (pDownParam->m_enumFileStatus== ENUM_UNPROCESS)
			{
				LOG4_DEBUG(L"kk Ssystem1\n");
				OutputDebugString(L"kk Ssystem1\n");
				m_pRestAPI->DownLoadFile(pDownParam->m_strHttpUrl, pDownParam->m_strFileName, pDownParam->m_strFilePath, pDownParam);
				LOG4_DEBUG(L"ee1Ssystem1\n");
				OutputDebugString(L"ee1Ssystem1\n");
				iterDownload = m_vecFileDownLoad.erase(iterDownload);
			}
			else if (pDownParam->m_enumFileStatus== ENUM_PROCESSING)
			{
			}
			else if(pDownParam->m_enumFileStatus ==ENUM_OK)
			{
				LOG4_DEBUG(L"kk Ssystem2\n");
				OutputDebugString(L"kk Ssystem2\n");
				iterDownload=  m_vecFileDownLoad.erase(iterDownload);
				LOG4_DEBUG(L"ee1Ssystem2\n");
				OutputDebugString(L"ee1Ssystem2\n");
			}
		}
	}

	m_bExitDownLoadThread = true;
}
void CSystemEx::DoUpLoad()
{
	while (true)
	{
		::Sleep(1);
		WaitForSingleObject(m_hUpLoadEvent, 15000);
		CCriticalAutoLock loLock(m_oFileUpLoadLock);

		if (m_bUpLoadActive==false)
		{
			break;
		}
		vector<UpLoadDownLoadParam*>::iterator iterUpload = m_vecFileUpLoad.begin();
		UpLoadDownLoadParam *pUpLoadParam = NULL;
		for (iterUpload; iterUpload!= m_vecFileUpLoad.end(); )
		{
			pUpLoadParam = *iterUpload;
			if (pUpLoadParam->m_enumFileStatus == ENUM_UNPROCESS )
			{
				pUpLoadParam->m_strHttpUrl =m_pRestAPI->UpLoadFile(pUpLoadParam->m_strFilePath, pUpLoadParam->m_strFileName, pUpLoadParam->m_strMediaType, pUpLoadParam);
				iterUpload = m_vecFileUpLoad.erase(iterUpload);
				if (pUpLoadParam != NULL)
				{
					delete pUpLoadParam;
					pUpLoadParam = NULL;
				}
				
			}
			else if (pUpLoadParam->m_enumFileStatus == ENUM_PROCESSING)
			{
			}
			else if (pUpLoadParam->m_enumFileStatus == ENUM_OK)
			{
				iterUpload = m_vecFileUpLoad.erase(iterUpload);

				if (pUpLoadParam != NULL)
				{
					delete pUpLoadParam;
					pUpLoadParam = NULL;
				}
			}
		
		}
	}
	m_bExitUpLoadThread = true;
}

std::wstring CSystemEx::GetFileSavePathByMsgType(ENUM_MSG_TYPE enumMsgType)
{
	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);
	if (m_strDataFolder.IsEmpty())
	{
		LOG4_ERROR("save file path is error");
		m_strDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
	}

	std::wstring strWorkPath = m_strDataFolder.GetBuffer(m_strDataFolder.GetLength());
	strWorkPath = strWorkPath+L"\\cacheFiles\\";
	switch (enumMsgType)
	{
	case ENUM_MSG_FILE_AUDIO:
		strWorkPath = strWorkPath + L"audio\\";
		break;
	case ENUM_MSG_FILE_VIDEO:
		strWorkPath = strWorkPath + L"video\\";
		break;
	case ENUM_MSG_FILE_PIC:
		strWorkPath = strWorkPath + L"pic\\";
		break;
	case ENUM_MSG_SCREEN_PIC:
		strWorkPath = strWorkPath + L"screenprint\\";
		break;
	case ENUM_MSG_FILE:
		strWorkPath = strWorkPath + L"file\\";
		break;
	case ENUM_MSG_EMOJI:
		break;
	case ENUM_MSG_OTHER:
		break;
	case ENUM_MSG_TEXT:
		break;
	case ENUM_SNAPSHOT:
		strWorkPath = strWorkPath + L"snapshot\\";
		break;
	default:
		break;
	}
	return strWorkPath;
}

void CSystemEx::InitFileServer()
{
	m_oFileDownLoadThread.BeginThread(_DoDownLoadThread, this);
	m_oFileUploadThread.BeginThread(_DoUpLoadThread, this);
}

void CSystemEx::AddFile2UploadList(UpLoadDownLoadParam*  pDownLoadParam)
{
	CCriticalAutoLock loLock(m_oFileUpLoadLock);
	m_vecFileUpLoad.push_back(pDownLoadParam);

	SetEvent(m_hUpLoadEvent);
}
void CSystemEx::AddFole2DownLoadList(UpLoadDownLoadParam* pDownLoadParam)
{
	CCriticalAutoLock loLock(m_oFileDownLoadLock);
	m_vecFileDownLoad.push_back(pDownLoadParam);

	SetEvent(m_hDownLoadEvent);
}
/*
void CSystemEx::CreateOneUser(std::string strSipNumber, std::string strPassWord, std::string strSipdomain,CUser *pUser)
{
	pUser->m_strSipNumber = strSipNumber;
	pUser->m_strSipDomain = strSipdomain;
	pUser->m_strPassword = strPassWord;
}*/
void CSystemEx::SaveSession2AudioCanvasHandler(int nActiveSession, CAudioPageCanvasHandler * pAudioPageCanvasHandler)
{
	CCriticalAutoLock loLock(m_oMapSession2Canvas);
	m_mapAudioCanvasHandler.insert(make_pair(nActiveSession,pAudioPageCanvasHandler));
}
void CSystemEx::SaveSession2VideoCanvasHandler(int nActiveSession, CVideoCanvasHandler * pVideoPageCanvasHandler)
{
	CCriticalAutoLock loLock(m_oMapSession2Canvas);
	m_mapVideoCanvasHandler.insert(make_pair(nActiveSession, pVideoPageCanvasHandler));
}
void CSystemEx::EraseSession2CanvasHandler(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oMapSession2Canvas);
	
	auto iterVideoCanvasHandler = m_mapVideoCanvasHandler.find(nActiveSession);
	if (iterVideoCanvasHandler !=m_mapVideoCanvasHandler.end())
	{
		iterVideoCanvasHandler = m_mapVideoCanvasHandler.erase(iterVideoCanvasHandler);
		return;
	}
	auto iterAudioCanvasHandler = m_mapAudioCanvasHandler.find(nActiveSession);
	if (iterAudioCanvasHandler != m_mapAudioCanvasHandler.end())
	{
		iterAudioCanvasHandler = m_mapAudioCanvasHandler.erase(iterAudioCanvasHandler);
		return;
	}
	return;
}

CAudioPageCanvasHandler* CSystemEx::GetAudioPageCanvasHandler(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oMapSession2Canvas);
	auto iterCanvasHandler = m_mapAudioCanvasHandler.find(nActiveSession);
	if (iterCanvasHandler != m_mapAudioCanvasHandler.end())
	{
		return iterCanvasHandler->second;
	}
	return NULL;
}
CVideoCanvasHandler * CSystemEx::GetVideoCanvasHandler(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oMapSession2Canvas);
	auto iterCanvasHandler = m_mapVideoCanvasHandler.find(nActiveSession);
	if (iterCanvasHandler != m_mapVideoCanvasHandler.end())
	{
		return iterCanvasHandler->second;
	}
	return NULL;
}

void CSystemEx::SaveAudioCavasHandler2Contact(CAudioPageCanvasHandler* pAudioPageCanvasHandler, TAG_RECENT *pContact)
{
	m_mapAudioCanvasHandler2Contact.insert(make_pair(pAudioPageCanvasHandler, pContact));
}

void CSystemEx::SaveVideoCanvasHandler2Contact(CVideoCanvasHandler *pVideoCanvasHandler, TAG_RECENT *pContact)
{
	m_mapVideoCanvasHandler2Contact.insert(make_pair(pVideoCanvasHandler, pContact));
}
void CSystemEx::SwtichLineUpdateHold(vector<int > vecAcitiveLine)
{
	int nSizeIndex = vecAcitiveLine.size();
	if (nSizeIndex==0)
	{
		return;
	}
	for (int nIndex = 0;nIndex<nSizeIndex&&nIndex< MAX_LINES;nIndex++)
	{
		int nActiveLine = vecAcitiveLine.at(nIndex);
		
		if (m_SessionArray[nActiveLine].getCallState() == false)
		{
			continue;
		}

		if (m_SessionArray[nActiveLine].getHoldState() == true)
		{
			continue;
		}

		PortSIP_hold(GetSipLib(), m_SessionArray[nActiveLine].getSessionId());
		m_SessionArray[nActiveLine].setHoldState(true);
		std::stringstream ssHoldLineInfo;
		std::string strHoldInfo;
		ssHoldLineInfo << "Set  Hold Line :" << nActiveLine;
		strHoldInfo = ssHoldLineInfo.str();
		LOG4_INFO(strHoldInfo.c_str());
	}
}

void CSystemEx::SwtichLineUpdateUnHold(vector<int> vecActiveLine)
{
	int nSizeIndex = vecActiveLine.size();
	if (nSizeIndex == 0)
	{
		return;
	}
	for (int nIndex = 0; nIndex <nSizeIndex&&nIndex < MAX_LINES; nIndex++)
	{
		int nAcitiveLine = vecActiveLine.at(nIndex);
	
		if (m_SessionArray[nAcitiveLine].getCallState() == false)
		{
			return;
		}


		if (m_SessionArray[nAcitiveLine].getHoldState() == false)
		{
			return;
		}

		PortSIP_unHold(GetSipLib(), m_SessionArray[nAcitiveLine].getSessionId());
		m_SessionArray[nAcitiveLine ].setHoldState(false);
		std::stringstream ssHoldLineInfo;
		std::string strHoldInfo;
		ssHoldLineInfo << "Set UN Hold Line :" << nAcitiveLine;
		strHoldInfo = ssHoldLineInfo.str();
		LOG4_INFO(strHoldInfo.c_str());
	}
}
void CSystemEx::EraseAudioCavasHandler2Contact(TAG_RECENT *pContact)
{
	auto iterAudioCavasHanler = m_mapAudioCanvasHandler2Contact.begin();
	for (iterAudioCavasHanler;iterAudioCavasHanler!=m_mapAudioCanvasHandler2Contact.end();iterAudioCavasHanler)
	{
		if (iterAudioCavasHanler->second ==pContact)
		{
			iterAudioCavasHanler = m_mapAudioCanvasHandler2Contact.erase(iterAudioCavasHanler);
		}
		else
		{
			iterAudioCavasHanler++;
		}
	}
}

void CSystemEx::EraseVideoCanvasHandler2Contact(TAG_RECENT *pContact)
{
	auto iterAudioCavasHanler = m_mapVideoCanvasHandler2Contact.begin();
	for (iterAudioCavasHanler; iterAudioCavasHanler != m_mapVideoCanvasHandler2Contact.end(); iterAudioCavasHanler)
	{
		if (iterAudioCavasHanler->second == pContact)
		{
			iterAudioCavasHanler = m_mapVideoCanvasHandler2Contact.erase(iterAudioCavasHanler);
		}
		else
		{
			iterAudioCavasHanler++;
		}
	}
}
CAudioPageCanvasHandler* CSystemEx::GetAudioPageCanvasHandlerByContact(TAG_RECENT *pContactDB)
{
	CAudioPageCanvasHandler* pLocalAudioPageCanvasHandler = NULL;
	auto iterAudioCavasHanler = m_mapAudioCanvasHandler2Contact.begin();
	for (iterAudioCavasHanler; iterAudioCavasHanler != m_mapAudioCanvasHandler2Contact.end(); iterAudioCavasHanler++)
	{
		if (iterAudioCavasHanler->second == pContactDB)
		{
			pLocalAudioPageCanvasHandler = iterAudioCavasHanler->first;
			break;
		}
	}
	return pLocalAudioPageCanvasHandler;
}

CVideoCanvasHandler* CSystemEx::GetVideoCanvasHandlerByContact(TAG_RECENT *pContactDB)
{
	CVideoCanvasHandler* pLocalVideoCanvasHandler = NULL;
	auto iterAudioCavasHanler = m_mapVideoCanvasHandler2Contact.begin();
	for (iterAudioCavasHanler; iterAudioCavasHanler != m_mapVideoCanvasHandler2Contact.end(); iterAudioCavasHanler++)
	{
		if (iterAudioCavasHanler->second == pContactDB)
		{
			pLocalVideoCanvasHandler = iterAudioCavasHanler->first;
			break;
		}
	}
	return pLocalVideoCanvasHandler;

}

void CSystemEx::SipUnRegister()
{
	if (!GetSipLib())
	{
		return;
	}

	for (int i = LINE_BASE; i < MAX_LINES; i++)
	{
		if (m_SessionArray[i].getRecvCallState() == true)
		{
			PortSIP_rejectCall(GetSipLib(), m_SessionArray[i].getSessionId(), 486);
		}
		else if (m_SessionArray[i].getCallState() == true)
		{
			PortSIP_hangUp(GetSipLib(), m_SessionArray[i].getSessionId());
		}

		m_SessionArray[i].reset();
	}
	if (m_SIPRegistered == true)
	{
		PortSIP_unRegisterServer(GetSipLib());
		PortSIP_removeUser(GetSipLib());
	}

	if (m_SIPInitialized == true)
	{
		PortSIP_unInitialize(GetSipLib());
	}

	m_SIPRegistered = false;
	m_SIPInitialized = false;
}
bool CSystemEx::isAutoRun()
{
	HKEY key;
	if (RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &key) != ERROR_SUCCESS)
	{
		return false;
	}

	char buffer[1024] = { 0 };
	DWORD type = REG_SZ;

	DWORD length = 1024;

	RegQueryValueExA(key, "PortGo", NULL, &type, (BYTE *)buffer, &length);
	RegCloseKey(key);

	if (strlen(buffer) <= 0)
	{
		return false;
	}

	return true;
}
void CSystemEx::SetAutoRun()
{
	char buff[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buff, MAX_PATH);

	

	HKEY key;
	if (RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &key) != ERROR_SUCCESS)
	{
		return;
	}



	string keyValue = buff;
	RegSetValueExA(key, "PortGo", 0, REG_SZ, (const unsigned char *)keyValue.c_str(), keyValue.length());

	RegCloseKey(key);

}

void CSystemEx::CancelAutoRun()
{
	char buff[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buff, MAX_PATH);


	HKEY key;
	if (RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &key) != ERROR_SUCCESS)
	{
		return;
	}


	RegDeleteValueA(key, "PortGo");
	RegCloseKey(key);
}
void CSystemEx::AutoLogin()
{
	CUser* theLocalUser = GetGlobalUser();
}

std::vector<string >CSystemEx::GetRecentContact()
{
	std::vector<string> vecSipNumber;
	if (m_pRestAPI==NULL)
	{
		return vecSipNumber;
	}
	vecSipNumber = m_pRestAPI->GetRecentContacts();
	return vecSipNumber;
}

std::vector<CChatRecord > CSystemEx::GetUnReadMsgByContact(CContactDB* pContactDB, std::string strLastUnReadMsgID,ENUM_LIST_MODE enumListMode, int nPageCount)
{
	std::string strReciverDomainSipNumber = GetGlobalUser()->m_strSipNumber + "@" + GetGlobalUser()->GetSipServer();
	std::string strSendDomainSipNumber = pContactDB->m_strSIPNum + "@" + GetGlobalUser()->GetSipServer();
	//m_pRestAPI->GetOffLineNum(strReciverDomainSipNumber, strSendDomainSipNumber);
	std::vector<CChatRecord > vecChatRecord = m_pRestAPI->GetOffLineMsg(strReciverDomainSipNumber, strSendDomainSipNumber,  strLastUnReadMsgID, enumListMode, nPageCount);
	return vecChatRecord;
}
void CSystemEx::UpdateAllOptionData()
{
	GetDBOprate()->updateOptions(m_pOptions);
}


void CSystemEx::AddUnReadMsg(CChatRecord theUnReadMsg)
{
	CCriticalAutoLock olock(m_oUnReadChartRecordLock);
	m_vecUnReadChatRecord.push_back(theUnReadMsg);
	EnableIcoFlash(true);
}
void CSystemEx::AddUnReadMsg(std::vector<CChatRecord> vecUnReadMsg)
{
	if (vecUnReadMsg.size()==0)
	{
		return;
	}
	CCriticalAutoLock olock(m_oUnReadChartRecordLock);
	auto iterUnReadMsg = vecUnReadMsg.begin();
	for (iterUnReadMsg;iterUnReadMsg<vecUnReadMsg.end();iterUnReadMsg++)
	{
		CChatRecord theIterChatRecord = *iterUnReadMsg;
		m_vecUnReadChatRecord.push_back(theIterChatRecord);
	}
	if (m_vecUnReadChatRecord.size()>0)
	{
		EnableIcoFlash(true);
	}
	
}
void CSystemEx::EraseUnReadMsg(vector<int> vecMsgID)
{
	CCriticalAutoLock olock(m_oUnReadChartRecordLock);
	auto iterFind = m_vecUnReadChatRecord.begin();

	for (iterFind;iterFind!=m_vecUnReadChatRecord.end();)
	{
		CChatRecord theLocalRecord = *iterFind;
		auto iterSlectMsg = vecMsgID.begin();
		bool bFind = false;
		for (iterSlectMsg; iterSlectMsg<vecMsgID.end();iterSlectMsg++)
		{
			int nMsgID = *iterSlectMsg;
			if (theLocalRecord.m_nMsgID == nMsgID)
			{
				bFind = true;
				break;
			}
		}
		if (bFind)
		{
			iterFind = m_vecUnReadChatRecord.erase(iterFind);
		}
		else
		{
			iterFind++;
		}
	}

	if (m_vecUnReadChatRecord.size()>=1)
	{
		EnableIcoFlash(true);
	}
	else
	{
		EnableIcoFlash(false);
	}
}


void CSystemEx::EraseUnReadMsgByContact(CContactDB * pContact)
{
	CCriticalAutoLock olock(m_oUnReadChartRecordLock);
	auto iterFind = m_vecUnReadChatRecord.begin();

	for (iterFind; iterFind != m_vecUnReadChatRecord.end();)
	{
		CChatRecord theLocalRecord = *iterFind;
		bool bFind = false;
		std::string strSIPURL = pContact->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
		
		if (PortUtility::compareStringNoCase(theLocalRecord.m_strTalkerURL, strSIPURL)==0  )
		{
			bFind = true;
		}
		
		if (bFind)
		{
			iterFind = m_vecUnReadChatRecord.erase(iterFind);
		}
		else
		{
			iterFind++;
		}
	}

	if (m_vecUnReadChatRecord.size() >= 1)
	{
		EnableIcoFlash(true);
	}
	else
	{
		EnableIcoFlash(false);
	}
}

void CSystemEx::EraseUnReadMsgAll()
{
	CCriticalAutoLock olock(m_oUnReadChartRecordLock);
	m_vecUnReadChatRecord.clear();
	EnableIcoFlash(false);
}
void CSystemEx::EnableIcoFlash(bool bOpen)
{
	FlickerTrayIcon(NULL, bOpen);
}

void CSystemEx::TipsFunNoRun()
{
	GetMainDlg()->TipsFunNoRun();
}

bool CSystemEx::StartRecord(int nActiveLine,std::string strFilePath,std::string strFileName)
{
	
	if (nActiveLine<LINE_BASE || nActiveLine>MAX_LINES)
	{
		LOG4_INFO("Active line  error ，where start record");
		return false;
	}
	if (m_SessionArray[nActiveLine].getCallState()==false)
	{
		LOG4_INFO("Active line  error ，where start record");
		return false;
	}
	if (m_SessionArray[nActiveLine].m_bRecord == true)
	{
		LOG4_INFO("Active line  is recording ，where start record");
		return false;
	}

	int nResult = PortSIP_startRecord(GetSipLib(),
		m_SessionArray[nActiveLine].getSessionId(),
		strFilePath.c_str(),
		strFileName.c_str(),
		false,
		FILEFORMAT_WAVE,
		RECORD_BOTH,
		VIDEO_CODEC_H264,
		RECORD_RECV);
	if (nResult!=0)
	{
		
		std::string strError;
		std::stringstream log;
		log << "Failed to tart record conversation." << "error code is " << nResult;
		strError = log.str();

		log << GetSipErrorInfo(nResult);
		LOG4_INFO(log.str().c_str());

		
		if (GetMainDlg() != NULL)
		{
			GetMainDlg()->CreatInfoWnd(_T("ERROR"), PortUtility::Utf82Unicode_ND(strError).c_str(), NULL, 5000);
		}
		return false;
	}
	m_SessionArray[nActiveLine].m_strFileName = strFileName;
	m_SessionArray[nActiveLine].m_strFilePath = strFilePath;
	m_SessionArray[nActiveLine].m_bRecord = true;
	if (GetMainDlg() != NULL)
	{
		GetMainDlg()->CreatInfoWnd(_T("INFO"), CSystemEx::GetInstance()->GetBeginRecordText(), NULL, 5000);
	}
	return true;
}

void CSystemEx::EndRecord(int nActiveSession , std::string strSipNumber,  ENUM_VA_RECORD enumAVRecordType)
{
	if (nActiveSession >= MAX_LINES)
	{
		return;
	}
	if (nActiveSession<LINE_BASE || nActiveSession>MAX_LINES)
	{
		LOG4_INFO("Active line  error ，where start record");
		return;
	}
	int nResult = PortSIP_stopRecord(GetSipLib(), m_SessionArray[nActiveSession].getSessionId());
	if (nResult != 0&&nResult!= ECoreCallIdNotFound)
	{
		std::string strError;
		std::stringstream log;
		log << "Failed to end record conversation." << "error code is " << nResult;
		strError = log.str();

		log << GetSipErrorInfo(nResult);
		LOG4_INFO(log.str().c_str());
		return;
	}
	m_SessionArray[nActiveSession].m_bRecord = false;
	CVARecord oVARecord;
	oVARecord.m_strBeginTime = "";
	oVARecord.m_strFileName = m_SessionArray[nActiveSession].m_strFilePath +"//"+ m_SessionArray[nActiveSession].m_strFileName;
	oVARecord.m_strSIPNum = strSipNumber;
	oVARecord.m_enumVARecordType = enumAVRecordType;
	oVARecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	std::string strFileTemplatePath;
	if (enumAVRecordType== ENUM_VA_RECORD_AUDIO)
	{
		strFileTemplatePath = oVARecord.m_strFileName + ".wav";
	}
	else
	{
		strFileTemplatePath = oVARecord.m_strFileName + ".avi";
	}
	int nSencond = 0;
	CIUIString strTemplatePlayTime = CSystemEx::GetInstance()->GetGlobalAVPlayer()->GetTemplateTime(strFileTemplatePath, nSencond);
	oVARecord.m_strPlayTime = PortUtility::Unicode2Utf8_ND(strTemplatePlayTime.GetBuffer(strTemplatePlayTime.GetLength()));
	CSystemEx::GetInstance()->GetDBOprate()->SaveOneAVRecord(oVARecord);

	CInboxCanvasHandler *pInboxCanvasHandler = GetInboCanvasHandler();
	if (pInboxCanvasHandler != NULL)
	{
		pInboxCanvasHandler->RefreshVARecord();
	}
	
}
BOOL CSystemEx::SwtichEnable(int &nCurrentIndex, int &nMaxPage, bool  bTipsCheckSwitch)
{
	if (nCurrentIndex > nMaxPage)
	{
		nCurrentIndex = nMaxPage-1;
		if (bTipsCheckSwitch)
		{
			GetMainDlg()->CreatInfoWnd(L"Info", L"It is the Last Page", NULL);
		}

		return FALSE;
	}
	if (nCurrentIndex < 1)
	{
		nCurrentIndex = 1;
		if (bTipsCheckSwitch)
		{
			GetMainDlg()->CreatInfoWnd(L"Info", L"It is the First Page", NULL);
		}

		return FALSE;
	}
	return TRUE;
}
void CSystemEx::playSystemWaveFile(const string & waveFile)
{
	if (waveFile.length() <= 0)
	{
		return;
	}

	PortUtility utility;

	wchar_t buf[2048] = { 0 };

	string fullFilePath;
	utility.getCurrentlyPath(fullFilePath);
	fullFilePath += "\\";
	fullFilePath += "sound\\";
	fullFilePath += waveFile;
	fullFilePath += ".wav";

	PlaySound(PortUtility::string2WString(fullFilePath).c_str(), NULL, SND_ASYNC);
}
void CSystemEx::loopPlaySystemWaveFile(const string & waveFile)
{
	if (m_bPlayCallingAudio==true)
	{
		return;
	}
	if (waveFile.length() <= 0)
	{
		return;
	}

	PortUtility utility;

	wchar_t buf[2048] = { 0 };

	string fullFilePath;
	utility.getCurrentlyPath(fullFilePath);
	fullFilePath += "\\";
	fullFilePath += "sound\\";
	fullFilePath += waveFile;
	fullFilePath += ".wav";

	PlaySound(PortUtility::string2WString(fullFilePath).c_str(), NULL, SND_FILENAME|SND_ASYNC|SND_LOOP);
	m_bPlayCallingAudio = true;
}
void CSystemEx::stopLoopPlaySystemWaveFile()
{
	m_bPlayCallingAudio = false;
	PlaySound(NULL, NULL, SND_FILENAME);
}

void    CSystemEx::BtnAudio(char code)
{
	string key;

	int dtmfTone = 0;
	switch (code)
	{
	case '0':
		key = "0";
		break;
	case '1':
		key = "1";
		break;
	case '2':
		key = "2";
		break;
	case '3':
		key = "3";
		break;
	case '4':
		key = "4";
		break;
	case '5':
		key = "5";
		break;
	case '6':
		key = "6";
		break;
	case '7':
		key = "7";
		break;
	case '8':
		key = "8";
		break;
	case '9':
		key = "9";
		break;
	case '*':
		key = "*";
		break;
	case '#':
		key = "#";
		break;
	case 'h':
		key = "h";
		break;
	case 'r':
		key = "r";
		break;
	case 'w':
		key = "w";
		break;
	default:
		key = "a";
		break;
	}
	playSystemWaveFile(key);
}
void    CSystemEx::SendDTMF(int nActiveSession, int nTone)
{
	if (nActiveSession<LINE_BASE || nActiveSession>MAX_LINES)
	{
		LOG4_INFO("Active line  error ，where start record");
		return ;
	}
	if (m_SessionArray[nActiveSession].getCallState() == true)
	{
		PortSIP_sendDtmf(GetSipLib(), m_SessionArray[nActiveSession].getSessionId(), DTMF_RFC2833, nTone, 160, true);
	}
}
void CSystemEx::CreateConfence(ENUM_CONFENCE_TYPE enumConfenceType, HWND videoHwnd)
{
	if (m_bExistConfence==true)
	{
		GetMainDlg()->CreatInfoWnd(L"Info",GetConfrenceNumText(),NULL,5000);
		return;
	}
	int rt = -1;
	switch (enumConfenceType)
	{
	case ENUM_AUDIO:
	{
		rt = PortSIP_createAudioConference(GetSipLib());
		if (rt != 0)
		{
		}
		else
		{
			m_bExistConfence = true;
		}
		break;
	}
	case ENUM_VIDEO:
	{
		TagScreenMetrics tagLocalVideoMetrics = PortUtility::transVideoResolution(m_pOptions->m_nVideoResolution);
		if (videoHwnd != NULL)
		{
			rt = PortSIP_createVideoConference(GetSipLib(), videoHwnd, tagLocalVideoMetrics.nFrameWidth,tagLocalVideoMetrics.nFrameHeight, true);
			if (rt != 0)
			{
			}
			else
			{
				m_bExistConfence = true;
			}
		}
		break;
	}

	default:
		break;
	}
}

void CSystemEx::DestoryConfence()
{
	int sessionIds[GROUP_LINE];
	for (int i = LINE_BASE; i < GROUP_LINE; ++i)
	{
		sessionIds[i] = 0;
	}

	for (int i = LINE_BASE; i < GROUP_LINE; ++i)
	{
		if (m_SessionArray[i].getCallState() == true && m_SessionArray[i].getHoldState() == false)
		{
			sessionIds[i] = m_SessionArray[i].getSessionId();

			if (m_ActiveLine != i)
			{
				// Hold the line 
				PortSIP_hold(GetSipLib(), sessionIds[i]);

				m_SessionArray[i].setHoldState(true);

				std::stringstream ssHoldLineInfo;
				std::string strHoldInfo;
				ssHoldLineInfo << "Set  Hold Line :" << i;
				strHoldInfo = ssHoldLineInfo.str();
				LOG4_INFO(strHoldInfo.c_str());
			}
		}
	}

	PortSIP_destroyConference(GetSipLib());
	m_bExistConfence = false;
}

std::string CSystemEx::FormatDownParam(UpLoadDownLoadParam *pUpLoadParam, std::string srFileName, std::string strHttpURL, ENUM_MSG_TYPE enumMsgFileType, TAG_RECENT *pContact,INT64 nFileSize,long lMsgID)
{
	std::string strTrueFileName;
	std::wstring strWFilePath = GetFileSavePathByMsgType(enumMsgFileType);
	pUpLoadParam->m_strFilePath = PortUtility::wstring2String(strWFilePath);
	pUpLoadParam->m_enumFileType = enumMsgFileType;
	pUpLoadParam->m_lMsgID = lMsgID;
	pUpLoadParam->m_hWndMsgProcess = CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd();
	std::wstring wstrFileName = PortUtility::Utf82Unicode_ND(srFileName);
	PortUtility::CheckFilePath(pUpLoadParam->m_strFilePath);
	pUpLoadParam->m_strFileName = PortUtility::CheckFileExist(pUpLoadParam->m_strFilePath, PortUtility::wstring2String(wstrFileName));
	strTrueFileName = pUpLoadParam->m_strFileName;
	pUpLoadParam->m_pContact = pContact;
	pUpLoadParam->m_strHttpUrl = REST_COMMON_HTTP + strHttpURL;
	pUpLoadParam->m_lFileSize = nFileSize;
	return strTrueFileName;
}

TagScreenMetrics CSystemEx::GetLocalMetrics()
{
	TagScreenMetrics  theLocalVideoMetrics;
	theLocalVideoMetrics.nFrameWidth = 0;
	theLocalVideoMetrics.nFrameWidth = 0;
	if (m_pOptions==NULL)
	{
	}
	else
	{
		theLocalVideoMetrics = PortUtility::transVideoResolution(m_pOptions->m_nVideoResolution);
	}
	return theLocalVideoMetrics;
}

TagScreenMetrics CSystemEx::GetPersonMetrics(int nLineIndex)
{
	int  sendBytes, sendPackets, sendPacketsLost, sendFractionLost, sendRttMS, sendCodecType, sendFrameWidth, sendFrameHeight, sendBitrateBPS, sendFramerate;
	//Receiver
	int  recvBytes, recvPackets, recvPacketsLost, recvFractionLost, recvCodecType, recvFrameWidth, recvFrameHeight, recvBitrateBPS, recvFramerate;
	TagScreenMetrics theLocalScreenMetrics;
	if (nLineIndex<0||nLineIndex>MAX_LINES)
	{
		theLocalScreenMetrics.nFrameWidth = 0;
		theLocalScreenMetrics.nFrameHeight = 0;
		return theLocalScreenMetrics;
	}
	int  nSessionID = GetSessionID(nLineIndex);
	
	int nErrorCode = PortSIP_getVideoStatistics(CSystemEx::GetInstance()->GetSipLib(), nSessionID, \
		&sendBytes,
		&sendPackets,
		&sendPacketsLost,
		&sendFractionLost,
		&sendRttMS,
		&sendCodecType,
		&sendFrameWidth,
		&sendFrameHeight,
		&sendBitrateBPS,
		&sendFramerate,
		&recvBytes,
		&recvPackets,
		&recvPacketsLost,
		&recvFractionLost,
		&recvCodecType,
		&recvFrameWidth,
		&recvFrameHeight,
		&recvBitrateBPS,
		&recvFramerate);

	if (nErrorCode)
	{
		std::stringstream ssErrorCode;
		std::string strErrorCodeInfo;
		strErrorCodeInfo = GetSipErrorInfo(nErrorCode);
		ssErrorCode << "Set  License Key Info Error Code :" << nErrorCode << " Info:" << strErrorCodeInfo;
		strErrorCodeInfo = ssErrorCode.str();
		LOG4_INFO(strErrorCodeInfo.c_str());
	}
	theLocalScreenMetrics.nFrameWidth = recvFrameWidth;
	theLocalScreenMetrics.nFrameHeight = recvFrameHeight;
	return theLocalScreenMetrics;
}


string CSystemEx::GetSipHeaderValue(const char * szHeaderKey, const char *signMsg/*SIP msg*/)
{
	std::string strHeaderValue = "";
	char szHeaderValue[MAX_PATH] = { 0 };
	int nReslut = PortSIP_getSipMessageHeaderValue(CSystemEx::GetInstance()->GetSipLib(), signMsg, szHeaderKey, szHeaderValue, sizeof(szHeaderValue));
	if (nReslut < 0)
	{
		std::string strErrorCodeInfo = CSystemEx::GetInstance()->GetSipErrorInfo(nReslut);
		std::stringstream ssErrorCode;
		ssErrorCode << "PortSIP_getSipMessageHeaderValue SIP_HEADER_MSG_ID Error Code :" << nReslut << " Info:" << strErrorCodeInfo;
		std::string strErrorInfo = ssErrorCode.str();
		LOG4_INFO(strErrorInfo.c_str());
		return strHeaderValue;
	}

	strHeaderValue.append(szHeaderValue);
	return strHeaderValue;
}


void CSystemEx::ExitSystem()
{
	SipUnRegister();
	StopWorkThread();
}

void CSystemEx::StopWorkThread()
{
	m_bDownLoadActive = false;
	INT64 i64Start = CBaseThread::GetSystemTime();
	SetEvent(m_hDownLoadEvent);

	while (!m_bExitDownLoadThread)
	{
		if (CBaseThread::GetSystemTime() - i64Start >= THREAD_TIME_OUT)
		{
			break;
		}

		CBaseThread::Sleep(5);
	}
	if (!m_bExitDownLoadThread)
	{
		HANDLE hDownLoad = m_oFileDownLoadThread.GetCurrentThreadHandle();
		TerminateThread(hDownLoad, 0);
		CloseHandle(hDownLoad);
	}
	m_bUpLoadActive = false;
	i64Start = CBaseThread::GetSystemTime();
	SetEvent(m_hUpLoadEvent);

	while (!m_bExitUpLoadThread)
	{
		if (CBaseThread::GetSystemTime() - i64Start >= THREAD_TIME_OUT)
		{
			break;
		}
		CBaseThread::Sleep(5);
	}

	if (!m_bExitUpLoadThread)
	{
		HANDLE hUpLoad = m_oFileUploadThread.GetCurrentThreadHandle();
		TerminateThread(hUpLoad, 0);
		CloseHandle(hUpLoad);
	}
	m_bVoiceMailActive = false;
	i64Start = CBaseThread::GetSystemTime();
	while (!m_bExitVoiceLoadThread)
	{
		if (CBaseThread::GetSystemTime() - i64Start >= THREAD_TIME_OUT)
		{
			break;
		}
		CBaseThread::Sleep(5);
	}

	if (!m_bExitVoiceLoadThread)
	{
		HANDLE hProcessVoiceMail = m_oProcessVoiceMailThread.GetCurrentThreadHandle();
		TerminateThread(hProcessVoiceMail, 0);
		CloseHandle(hProcessVoiceMail);
	}
}

int CSystemEx::RegisteModule()
{
	std::string strErrorInfo;
	bool bRet = InitRestAPIByUser();
	if (bRet == false)
	{
		return -1;
	}
	std::string strFullPath = "";
	CIUIString strTempDataFolder = GetDataFolder();
	if (strTempDataFolder.IsEmpty())
	{
		LOG4_ERROR("save file path is error");
		PortUtility::getCurrentlyPath(strFullPath);
		strTempDataFolder = PortUtility::Utf82Unicode_ND(strFullPath).c_str();
	}
	strFullPath = PortUtility::Unicode2Utf8_ND(strTempDataFolder.GetBuffer(strTempDataFolder.GetLength()));
	PortUtility::CheckFilePath(strFullPath);
	bRet = CSystemEx::GetInstance()->InitSIPByUser((TRANSPORT_TYPE)CSystemEx::GetInstance()->GetGlobalUser()->GetProtocolMode(), PORTSIP_LOG_NONE, strFullPath.c_str(), 0, 0, false, strErrorInfo, m_pUser);
	if (bRet == false)
	{
		return -1;
	}
	else
	{

	}
	return 0;
}
DWORD WINAPI CSystemEx::_ConnectThread(LPVOID pParam)
{
	CSystemEx *p = (CSystemEx *)pParam;

	while (TRUE)
	{
		if (p->RegisteModule() == 0)
		{
			break;
		}

		Sleep(10000);
	}

	return 0;
}
bool CSystemEx::ReConnect()
{
	if (m_SIPRegistered)
	{
		return false;
	}
	RegisteModule();
	DWORD dwThreadId = 0;
	//HANDLE h = CreateThread(NULL, 0, _ConnectThread, (LPVOID)this, 0, &dwThreadId);
	//if (h)
	//{
	//	CloseHandle(h);
	//}

	return true;
}

bool CSystemEx::ReConnectThread()
{
	if (m_SIPRegistered)
	{
		return false;
	}
	RegisteModule();
	DWORD dwThreadId = 0;
	HANDLE h = CreateThread(NULL, 0, _ConnectThread, (LPVOID)this, 0, &dwThreadId);
	if (h)
	{
		CloseHandle(h);
	}

	return true;
}
void CSystemEx::SetOffline()
{
	if (GetRegister()==false)
	{
		return;
	}
	SipUnRegister();
	GetMainDlg()->ResetOnLineStatus(false);
}
void CSystemEx::SetOnline()
{
	if (GetRegister()==true)
	{
		return;
	}
	ReConnect();
}
void CSystemEx::SetOtherStatus(ENUM_LOGIN enumOtherStatus)
{
	if (CSystemEx::GetInstance()->GetGlobalUser()->m_nPresencMode == 0)
	{
		printf("");
	}
	string strStatusText ;
	int nRet = 0;
	switch (enumOtherStatus)
	{
	case  ENUM_BUSY:
		strStatusText = "Busy";
		nRet = PortSIP_setPresenceStatus(GetSipLib(), 0, strStatusText.c_str());
		break;
	case ENUM_AWAY:
		strStatusText = "Away";
		nRet = PortSIP_setPresenceStatus(GetSipLib(), 0, strStatusText.c_str());
		break;
	case  ENUM_NODISTURB:
		strStatusText = "Donotdisturb";
		nRet = PortSIP_setPresenceStatus(GetSipLib(), 0, strStatusText.c_str());
		break;
	default:
		break;
	}
	if (nRet < 0)
	{
		std::string strErrorCodeInfo = CSystemEx::GetInstance()->GetSipErrorInfo(nRet);
		std::stringstream ssErrorCode;
		ssErrorCode << "PortSIP_setPresenceStatus  Error Code :" << nRet << " Info:" << strErrorCodeInfo;
		std::string strErrorInfo = ssErrorCode.str();
		LOG4_INFO(strErrorInfo.c_str());
		return ;
	}
	SetLocalStatus(enumOtherStatus);
}

bool CSystemEx::GetConferenceStatus(std::string strConferenceNum,long& lSubID)
{
	if (GetSipLib()==NULL)
	{
		return false;
	}
	int nRet = PortSIP_presenceSubscribe(GetSipLib(), strConferenceNum.c_str(), "participants_list");
	if (nRet < 0)
	{
		std::string strErrorCodeInfo = CSystemEx::GetInstance()->GetSipErrorInfo(nRet);
		std::stringstream ssErrorCode;
		ssErrorCode << "PortSIP_presenceSubscribe  Error Code :" << nRet << " Info:" << strErrorCodeInfo;
		std::string strErrorInfo = ssErrorCode.str();
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	lSubID = nRet;
	return true;
}

bool CSystemEx::UnGetConferenceStatus(long lSubID)
{
	if (GetSipLib()==NULL)
	{
		return false;
	}
	int nRet = PortSIP_presenceTerminateSubscribe(GetSipLib(), lSubID);
	if (nRet < 0)
	{
		std::string strErrorCodeInfo = CSystemEx::GetInstance()->GetSipErrorInfo(nRet);
		std::stringstream ssErrorCode;
		ssErrorCode << "PortSIP_presenceSubscribe  Error Code :" << nRet << " Info:" << strErrorCodeInfo;
		std::string strErrorInfo = ssErrorCode.str();
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	return true;
}

void CSystemEx::RefreshRecentTime(TAG_RECENT* pContactInfo)
{
	if (pContactInfo==NULL)
	{
		return;
	}
	time_t timep;
	time(&timep);
	if (pContactInfo->m_enumRecentContact == ENUM_SINGLE_CONTACT)
	{
		pContactInfo->m_unionContactInfo.m_pContact->m_tRecentChat = timep;
	}
	else
	{
		pContactInfo->m_unionContactInfo.m_pChartGroup->m_tRecentChat = timep;
	}

	if (GetMainCanvasHandler()!=NULL)
	{
		GetMainCanvasHandler()->ReListItem();
	}
	
}

void CSystemEx::TerminateAllThread()
{
	HANDLE hDownLoadThread = m_oFileUploadThread.GetCurrentThreadHandle();
	if (hDownLoadThread!=NULL)
	{
		CBaseThread::TerminateThreadEx(hDownLoadThread);
	}


	HANDLE hUpLoadThread = m_oFileUploadThread.GetCurrentThreadHandle();
	if (hUpLoadThread != NULL)
	{
		CBaseThread::TerminateThreadEx(hUpLoadThread);
	}

	HANDLE hVoiceMailThread = m_oProcessVoiceMailThread.GetCurrentThreadHandle();
	if (hVoiceMailThread != NULL)
	{
		CBaseThread::TerminateThreadEx(hVoiceMailThread);
	}
}

bool CSystemEx::QueryFolderDataRegKey( LPCWSTR strValueName, CIUIString& strValue, int length)
{
	DWORD dwType = REG_SZ;
	DWORD dwLen = MAX_PATH;

	wchar_t data[MAX_PATH];
	HKEY hSubKey;
	LPCWSTR sw = _T("SOFTWARE\\WOW6432Node\\PortSIP Solutions, Inc.\\PortSIP Softphone");

	if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, sw, &hSubKey))
	{
		TCHAR buf[256] = { 0 };

		if (ERROR_SUCCESS == RegQueryValueEx(hSubKey, strValueName, 0, &dwType, (LPBYTE)data, &dwLen))
		{
			strValue = data;
			RegCloseKey(hSubKey);
			return true;
		}
	}
	RegCloseKey(hSubKey);
	
	return false;
}
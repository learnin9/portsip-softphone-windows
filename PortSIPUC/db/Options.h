#ifndef OPTIONS_H
#define OPTIONS_H
/*
Reserved             VARCHAR(1024),
*/
class COptions
{
public:
	COptions();
	~COptions();
	//ID
	int					m_nOptionsID;
	/************************************************************************/
	/*              Setting1 page: Application								*/
	/************************************************************************/
	bool					m_bDefaultSoftphone;
	bool					m_bLocalVideoInRoom;
	bool                 m_bDisplayidle;
	bool					m_bShowTimeStamp;
	std::string         m_strRecordFolder;
	/************************************************************************/
	/*  Setting2 page: AudioDocode	                                        */
	/************************************************************************/
	/************************************************************************/
	/*  Setting4 page: MideaQos	                                        */
	/************************************************************************/
	//AEC-reduce echos form speakers(AES)
	bool					 m_bAEC;
	//ANS-reduce bachgroud noise
	int					m_bANS;
	//AGC-automatic gain control
	bool					 m_bAGC;
	//VAD-voice activity detection
	bool					 m_bVAD;
	//CNG-confort noise generator
	bool					 m_bCNG;
	//Ebable audio Qos
	bool					m_bEAQ;
	//enable video Qos
	bool                    m_bEVQ;
	int					m_nVideoResolution;
	int					m_nVideoImageQualityLevel;
	/************************************************************************/
	/*  Setting5 page: misc												    */
	/************************************************************************/
	int					m_nAudioRtpMinPort;
	int					m_nAudioRtpMaxPort;
	int					m_nVideoRtpMinPort;
	int					m_nVideoRtpMaxPort;
	bool					m_bRTPKeepLive;
	bool					m_bEnableMWI;
	bool                    m_bEnableSessionTimer;
	int					m_nSessionTimerTime;
	bool                m_bEnableForword;
	std::string			m_strForwardTo;
	bool				m_bEnableOnPhoneForword;
	std::string			m_strBusyForwardTo;
	/************************************************************************/
	/*  Setting page6:											*/
	/************************************************************************/
	int			m_nLanguage;
	int         m_nLastLanguage;
	/************************************************************************/
	/*  Setting page: device												*/
	/************************************************************************/
	bool				m_bEnableVideoDev;
	int					 m_nVideoDeviceID;
	bool                m_bEnableAudioOutputDev;
	int					 m_nAudioOutputDeviceID;
	bool                m_bEnableAudioInputDev;
	int					 m_nAudioInputDeviceID;
	
	/************************************************************************/
	std::string			 m_strARPath;
	std::string		     m_strARName;
	std::string			 m_strVRPath;
	std::string		     m_strVRName;
	int					 m_nDTMFMode;
	int					 m_nDTMFPayload;
	int					 m_nDTMFDuration;
	int			         m_nJitterBuffer;
	int					m_nKeepAlive;
	bool					m_bReverseLocalVideo;
	bool					m_bReverseReceivedVideo;
	bool					m_bReverseSendingVideo;
	std::string         m_strNetworkInterface;
	int					m_nDisableCABAC;
	std::string			m_strNoAnswerFowardTo;
	int					m_nNoAnswerWaitSeconds;
	std::string			m_strLastLoginURL;
	int					m_nVideoServiceType;
	int					m_nVideoDSCP;
	int					m_nAudioDSCP;
	int					m_nAudioSeviceType;
	std::string			m_strTemp;
	std::string			m_strSt;
	int					m_nLogLevel;
	std::string			m_strlogPath;
	int					m_nRtpKeepAlivePayload;
	int					m_nDeltaTransmitTimeMS;
	int					m_nPriority;
	bool					m_bCallImmediately;
	int					m_nAutoSendVideo;
	bool                m_bSaveLanguage;
	std::string			m_strReserved;
};
#endif
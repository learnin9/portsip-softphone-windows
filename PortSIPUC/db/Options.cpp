#include "stdafx.h"
#include "Options.h"
COptions::COptions()
{
	m_nOptionsID			= 0;
	m_bAGC					= 0;
	m_bAEC					= 0;
	m_bVAD					= 0;
    m_bCNG					= 0;
    m_strARPath				= "";
	m_strARName				= "";
	m_strVRPath				= "";
	m_strVRName				= "";
	m_nDTMFMode				= 0;
	m_nDTMFPayload			= 0;
	m_nDTMFDuration			= 0;
	m_nJitterBuffer		    = 0;
	m_nAudioInputDeviceID	= 0;
	m_nAudioOutputDeviceID	= 0;
	m_nVideoDeviceID		= 0;
	m_nKeepAlive			= 0;
	m_bReverseLocalVideo	= false;
	m_bReverseReceivedVideo = false;
	m_bReverseSendingVideo  = false;
	m_nVideoImageQualityLevel = 0;
	m_strForwardTo			  = "";
	m_strBusyForwardTo		  = "";
	m_strNetworkInterface	  = "";
	m_nDisableCABAC			  = 0;
	m_strNoAnswerFowardTo		="";
	m_nNoAnswerWaitSeconds		=0;
	m_nAudioRtpMinPort			= 0;
	m_nAudioRtpMaxPort			= 0;
	m_nVideoRtpMinPort			= 0;
	m_nVideoRtpMaxPort			= 0;
	m_nLanguage					=0;
	m_bEnableMWI				=0;
	m_bRTPKeepLive = 0;
	m_strLastLoginURL			="";
	m_bLocalVideoInRoom = false;
	m_nVideoResolution			= 0;
	m_nSessionTimerTime			= 0;
	m_nVideoServiceType			= 0;
	m_nVideoDSCP				= 0;
	m_nAudioDSCP				= 0;
	m_nAudioSeviceType		    = 0;
	m_strTemp					= "";
	m_strSt						= "";
	m_nLogLevel					=0;
	m_strlogPath				= "";
	m_nRtpKeepAlivePayload		= 0;
	m_nDeltaTransmitTimeMS		= 0;
	m_nPriority					= 0;
	m_bDefaultSoftphone			= 0;
	m_bDisplayidle = 0;
	m_bShowTimeStamp			= false;
	m_bCallImmediately			= false;
	m_strRecordFolder			= "";
	m_bANS = false;
	m_nAutoSendVideo			= 0;
	m_strReserved				= "";

	m_bEAQ = false;
	m_bEVQ = false;
	m_bEnableSessionTimer = false;
	m_bEnableForword = false;
	m_bEnableOnPhoneForword = false;
	m_bEnableVideoDev = false;
	m_bEnableAudioOutputDev = true;
	m_bEnableAudioInputDev = true;

	m_nLastLanguage = m_nLanguage;
	m_bSaveLanguage = false;
}
COptions::~COptions()
{

}

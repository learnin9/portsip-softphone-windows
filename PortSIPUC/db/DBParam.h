#ifndef DB_PARAM_H
#define DB_PARAM_H
#include <string>

#define DB_DATA				"Param.db"
#define DB_BAK				"PortGo_Back.db"

#define T_USER						     "T_USER"
#define T_OPTIONS						 "T_OPTIONS"
#define T_CONTACTS						 "T_CONTACTS"
#define T_CHAT_RECORD					 "T_CHAT_RECORD"
#define T_TALK_GROUP					 "T_TALK_GROUP"
#define T_TALK_GROUP_NUMBER				 "T_TALK_GROUP_NUMBER"
#define  T_CALL_HISTORY					 "T_CALL_HISTORY"
#define T_VEDIO_AUDIO_RECORD			 "T_VEDIO_AUDIO_RECORD"
#define  T_CONTACT_GROUP				 "T_CONTACT_GROUP"
#define  T_VOICE_MAIL					 "T_VOICE_MAIL"
#define  T_AV_CODEC						 "T_AV_CODEC"
/*
************************************************************************************************************************************************************
************************************************************************************************************************************************************
*/
//
struct MAIN_VERSION
{
	MAIN_VERSION::MAIN_VERSION()
	{
		m_strMainVer = "";
	}
	std::string  m_strMainVer;
};
struct CONTACT_EMAIL_INFO
{
	CONTACT_EMAIL_INFO::CONTACT_EMAIL_INFO()
	{
		m_nContactID = 0;
		m_strEmail	 = "";
	}
	int	m_nContactID;
	std::string		m_strEmail;
};
struct CONTACT_SIP_NUM
{
	CONTACT_SIP_NUM::CONTACT_SIP_NUM()
	{
		m_nContactID = 0;
		m_strSipNum = "";
	}
	int	m_nContactID;
	std::string		m_strSipNum;
};
struct CONATCT_GROUP
{
	CONATCT_GROUP::CONATCT_GROUP()
	{
		m_strGroupName = "";
	}
	std::string		m_strGroupName;
};
enum ENUM_MSG_TYPE
{
	ENUM_MSG_TEXT = 0,	 
	ENUM_MSG_AUDIO,		 
	ENUM_MSG_VIDEO,		 
	ENUM_MSG_FILE_PIC,	 
	ENUM_MSG_FILE,		 
	ENUM_MSG_EMOJI,		 
	ENUM_MSG_FILE_AUDIO, 
	ENUM_MSG_FILE_VIDEO, 
	ENUM_MSG_SHAKE,	     
	ENUM_MSG_OTHER,		 
	ENUM_MSG_SCREEN_PIC,	  
	ENUM_SNAPSHOT			
};

enum ENUM_MSG_STATUS
{
	ENUM_MSG_STATUS_SENDSUCC  =0,		
	ENUM_MSG_STATUS_SENDING,			
	ENUM_MSG_STATUS_SENDFAILD,			
	ENUM_MSG_STATUS_SENDTIMEOUT,		
	ENUM_MSG_STATUS_UNREAD,				
	ENUM_MSG_STATUS_READ				
};


enum  ENUM_CALL_TYPE
{
	ENUM_CALL_IN	 = 0,
	ENUM_CALL_OUT		 
};

enum  ENUM_CALL_STATUS
{
	ENUM_CALL_STATUS_SUCC = 0,
	ENUM_CALL_STATUS_FAILD = 1,
	ENUM_CALL_ING
	
};
enum ENUM_VA_RECORD
{
	ENUM_VA_RECORD_VIDEO = 0,
	ENUM_VA_RECORD_AUDIO	 
};
enum ENUM_AV_CODEC
{
	ENUM_VA_CODEC_AUDIO	=0 ,
	ENUM_VA_CODEC_VIDEO 
};
enum ENUM_CONTENT_TYPE
{
	CALL_VOICE_MSG = 0, 
	CALL_PHONE,			
	CALL_VIDEO			
};

enum EOnlineState
{
	ONLINESTATE_ONLINE = 1,			
	ONLINESTATE_OFFLINE = 2,		
	ONLINESTATE_AWAY = 3,			
	ONLINESTATE_BUSY = 4,			
	ONLINESTATE_HIDE = 5,			
	ONLINESTATE_DONOT_DISTURB = 6	
};

struct TagRecentContactInfo
{
	bool        m_bIsMyself;
	std::string m_strTalkerURL;
	std::string m_strDateTime;
};

#define		MAX_HISTORY_NUM     1000
#define     MAX_PAGE_SHOW_CONTACT			34
#define     MAX_PAGE_SHOW_INBOX				34
#define     MAX_CONTACT_NUM		1000
#define     MAX_CHAT_NUM		5000
//
#define		PAGE_SIZE_COMMON    100
#endif
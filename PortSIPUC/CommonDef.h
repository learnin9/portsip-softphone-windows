#ifndef		PORTSIP_COMMON_DEF
#define		PORTSIP_COMMON_DEF
#include <IUI.h>
using namespace IUI;

#include "db/Contact.h"
#include "db/TalkGroup.h"
#define		USER_AGENT			"PortGo v1.0.0, Build 2020.06.22"
#define		SIPPORT_MIN			10000
#define     VOICE_MAIL_NUMBER   "999"

//#define   KCXX				 1
#define     BZXX                 1

#ifndef BZXX
#define KCXX                     1
#endif


//shell notify
#define		WM_SHELLNOTIFY WM_USER+100

// SIP OR XMPP CALLBACK EVENTS
#define		WM_SIPEVENT					 WM_USER + 8000
#define		WM_XMPPEVENT				 WM_USER + 9000
#define     WM_DOWNLOAD_FILE			 WM_USER	+ 10001
#define     WM_UPLOAD_FILE				 WM_USER + 10002
#define     WM_DOWNLOAD_VOICE_MAIL		 WM_USER + 10003
#define     WM_CREATE_INFO_WND			 WM_USER + 10004
#define     WM_CLOSE_INFO_WND			 WM_USER+  10005
#define     WM_UPLOAD_FILE_GET_OK        WM_USER + 10006
#define     WM_DOWNLOAD_FILE_OK			 WM_USER + 10007
//#define     WM_UPLOAD_FILE_PROCESS_OK    WM_USER +10008
#define     WM_CONFENCE_ACTION				 WM_USER +10009
#define     WM_RESTORE_MAIN_DLG              WM_USER+10010
#define     WM_VIDEO_SHOW_OK                 WM_USER+10011
//no-blocking msg to make sure quickly exit ui
#define     WM_CLOSE_LOCAL_VIDEO_WND         WM_USER+10212
#define     WM_CONTACT_INFO_UPDATE           WM_USER+10213
#define     WM_CALL_ACTION_SINGLE            WM_USER+10214
#define     WM_ANSWER_BY_REMOTE			     WM_USER+10315
/************************************************************************/
/************************************************************************/
#define    WM_PLAY_BEGIN		 WM_USER+10012
#define    WM_PLAY_END			 WM_USER+10013
#define    WM_PLAY_POS_CHANGED   WM_USER+10014
#define    WM_PLAY_DRAG_POS      WM_USER+10015
#define    WM_PLAY_TIME_CHANGED  WM_USER+10016

/************************************************************************/
/************************************************************************/
#define WM_SET_IS_CONFERENCE  WM_USER +10101
#define WM_JOIN_CONFERENCE    WM_USER +10102
#define WM_EXIT_CONFERENCE    WM_USER +10104



#define     MSG_MAX_LENGTH              1024*5
#define		LINE_BASE 1

#define     GROUP_LINE 10
#define		MAX_LINES GROUP_LINE-1

#define		TIMER_BASE				200
#define		INCOMING_CALL_TIMER		TIMER_BASE+3
#define		RINGING_TIMER			TIMER_BASE+4

#define		CALLSESSION_TIMER1		TIMER_BASE+10
#define		CALLSESSION_TIMER2		TIMER_BASE+11
#define		CALLSESSION_TIMER3		TIMER_BASE+12
#define		CALLSESSION_TIMER4		TIMER_BASE+13
#define		CALLSESSION_TIMER5		TIMER_BASE+14
#define		CALLSESSION_TIMER6		TIMER_BASE+15
#define		CALLSESSION_TIMER7		TIMER_BASE+16
#define		CALLSESSION_TIMER8		TIMER_BASE+17

#define     TIMER_MAX				TIMER_BASE+17

#define  MAX_INFO_WND			3
#define  MAX_CALLER_WND         10
#define  VOICE_MAIL_CHECK_TIME  10
#define  MAX_CALL_VIEW_NUM 6
//
#define  MAX_MERBER_CONFRENCE 4
enum ENUM_PLAYER_TYPE
{
	ENUM_AUDIO = 0,
	ENUM_VIDEO
};
enum ENUM_PRESENCE_MODE
{
	ENUM_PEER2PEER  =0,
	ENUM_AGENT
};

enum ENUM_LANGUAGE
{
	ENUM_ENGLISH = 0,//
	ENUM_CHINAESE_S,
	ENUM_CHINAESE_T,
	ENUM_ITAlIANA,
	ENUM_SPAIN,
	ENUM_FRANCH,
	ENUM_RUSSIA,
	ENUM_PORTUGUESE
};

enum ENUM_LOGIN
{
	ENUM_ONLINE =0, 
	ENUM_BUSY,		
	ENUM_AWAY,		
	ENUM_NODISTURB, 
	ENUM_OFFLINE	
};
enum ENUM_FILE_STATUS
{
	ENUM_FAILED_PRE = 0,
	ENUM_UNPROCESS,
	ENUM_PROCESSING,
	ENUM_OK,
	ENUM_FAILED_END
};

enum ITEM_POS
{
	ITEM_FRONT,
	ITEM_BEHIND,
	ITEM_SAME,
	ITEM_UNKNOW
};

enum ENUM_VIDEO_CODEC_TYPE
{
	ENUM_VIDEO_H264 =0,
	ENUM_VIDEO_VP8,
	ENUM_VIDEO_VP9
};

enum ENUM_AUDIO_CODEC_TYPE
{
	ENUM_AUDIO_OPUS =0,
	ENUM_AUDIO_G729,
	ENUM_AUDIO_PCMU,
	ENUM_AUDIO_PCMA,
	ENUM_AUDIO_ISACSWB,
	ENUM_AUDIO_G722,
	ENUM_AUDIO_ILBC,
	ENUM_AUDIO_AMR,
	ENUM_AUDIO_AMR_WB,
	ENUM_AUDIO_AMR_SPEEX,
	ENUM_AUDIO_SPEEX_WB
};


struct VoiceMailParam
{
	INT64 m_lID;
	std::string m_strCaller;
	std::string m_strCalled;
	std::string m_strTime;
	std::string m_strFileHttpURL;
	std::string m_strStatus;
};

struct CallActionParam
{

};
enum WND_INFO_LEVEL
{
	INFO_WND = 0,
	ERROR_WND,
	WARN_WND
};
enum CHAT_TAB
{
	CT_TEXT_CHAT = 1,
	CT_AUDIO,
	CT_VIDEO
};
enum ENUM_RECENT_CONTACT_TYPE
{
	ENUM_SINGLE_CONTACT,
	ENUM_MULTI_CONTACT
};
union RECENT_CONTACT_INFO
{
	RECENT_CONTACT_INFO::RECENT_CONTACT_INFO()
	{
		m_pContact = NULL;
		m_pChartGroup = NULL;
	}
	CContactDB* m_pContact;
	CTalkGroup *m_pChartGroup;
};
struct TAG_RECENT
{
	RECENT_CONTACT_INFO m_unionContactInfo;
	ENUM_RECENT_CONTACT_TYPE   m_enumRecentContact;
};

struct UpLoadDownLoadParam
{
	UpLoadDownLoadParam::UpLoadDownLoadParam()
	{
		m_strFilePath = "";
		m_strFileName = "";
		m_strMediaType = "";
		m_enumFileStatus = ENUM_UNPROCESS;
		m_fProcessData = 0.0;
		m_strHttpUrl = "";
		m_pContact = NULL;
		m_strToken = "";
		m_lFileSize = 0;
		m_hWnd = NULL;
		m_enumFileType = ENUM_MSG_FILE;
		m_nDuration = 0;
		m_hWndMsgProcess = NULL;
	}
	std::string m_strFilePath;
	std::string m_strFileName;
	std::string m_strMediaType;
	float		m_fProcessData;			
	ENUM_FILE_STATUS m_enumFileStatus;	
	ENUM_MSG_TYPE    m_enumFileType;	
	long		m_lMsgID;
	INT64		m_lFileSize;
	std::string m_strHttpUrl;
	std::string m_strToken;
	bool        m_bIsLocation;
	TAG_RECENT *m_pContact;
	IUI::HWLWND      m_hWnd;
	int				m_nDuration;
	HWND            m_hWndMsgProcess;
	CIUIString      m_strDisPlayName;
};
typedef struct CONFERENCE_NUMBER_INFO
{
	CONFERENCE_NUMBER_INFO()
	{
		m_bMuted = false;
		m_strSipURL = "";
	}

	~CONFERENCE_NUMBER_INFO()
	{

	}
	bool				m_bMuted;		
	std::string 		m_strSipURL;	

}STRUCT_CONFERENCE_NUMBER_INFO, *LPSTRUCT_CONFERENCE_NUMBER_INFO;
typedef struct CONFERENCE_ROOM_INFO
{
	CONFERENCE_ROOM_INFO()
	{
		m_lConferenceRoomID		= 0;
		m_strConferenceNumber	= "";
		m_strConferenceNumber	= "";
		m_nMaxNumber			= 0;
		m_enumConferenceType    = ENUM_AUDIO;
		m_bRecord = false;
		m_bLocked = false;
		m_bMuted = false;
	}

	~CONFERENCE_ROOM_INFO()
	{

	}
	long long				m_lConferenceRoomID;	
	std::string 		m_strConferenceNumber;	
	std::string 		m_strConferenceTitle;	
	int					m_nMaxNumber;			
	ENUM_PLAYER_TYPE	m_enumConferenceType;	
	bool				m_bRecord;			   
	bool				m_bLocked;			    
	bool        		m_bMuted;				
	vector<CONFERENCE_NUMBER_INFO> m_vecConferenceNumber;
}STRUCT_CONFERENCE_ROOM_INFO, *LPSTRUCT_CONFERENCE_ROOM_INFO;

struct VoiceMailDownLoadParam
{
	VoiceMailDownLoadParam::VoiceMailDownLoadParam()
	{
		m_fProcessData = 0.0;
		m_lID = 0;
	}
	float		m_fProcessData;
	long long		m_lID;				 
};
struct	TAG_ASYNC_SIP_CONFERENCE_PARAM
{
	std::string m_strSipNumber;
	INT64       m_nStartTime;
};
struct Add2UploadListParam
{
	Add2UploadListParam::Add2UploadListParam()
	{
		strFilePath = L"";
		strFileName = L"";
		hWnd = NULL;
		m_lMsgID = 0;
	}
	long m_lMsgID;
	CIUIString strFilePath;
	CIUIString strFileName;
	const char* strFileType;
	ENUM_MSG_TYPE enumFileType;
	CIUIString strPlayTime;
	HWLWND hWnd;
};
struct  InfoWndParam
{
	CIUIString strTitle;
	CIUIString strMsg;
	int        nAutoTimer;
};
struct SFindContactItem
{
	HTREEITEM hFound;
	void *pCallbackParam;
};

struct TagScreenMetrics
{
	int nFrameHeight;
	int nFrameWidth;
};
/*struct  tagVideoWidthPerHeight
{
	int nWidth;
	int nHeight;
};*/
#define  MAX_LOCAL_VIDEO_WIDTH_NORMAL  160
#define  MAX_LOCAL_VIDEO_HEIGHT_NORMAL 120
#define  CHAT_MSG_PER_PAGE             5
#define  CHAT_RECORD_MSG_PERPAGE       5

enum INSERT_HISTORY_MODE
{
	INSERT_FORWARD = -1,//
	INSERT_BACKWARD = 1 //
};

struct OLE_VIDEO_INFO
{
	HWLWND hWndOle;
	void *pOleVideoCanvasHandler;
};
const char* const key_G_FILEUPDATE_RESULT_KEY_FID = "fid";
const char* const key_G_FILEUPDATE_RESULT_KEY_fILENAME = "fileName";
const char* const key_G_FILEUPDATE_RESULT_KEY_ETAG = "eTag";
const char* const key_G_FILEUPDATE_RESULT_KEY_SIZE = "size";
const char* const key_G_FILEUPDATE_RESULT_KEY_URL = "fileUrl";
const char* const key_G_FILEUPDATE_RESULT_KEY_ID = "id";
const char* const key_G_FILEUPDATE_RESULT_KEY_FILEID = "fileid";
const char* const key_G_MIME = "mime";//urlmime

const char* const MESSAGE_TYPE_TEXT = "text";
const char* const MESSAGE_TYPE_AUDIO = "audio";
const char* const MESSAGE_TYPE_VIDEO = "video";
const char* const MESSAGE_TYPE_IMAGE = "image";
const char* const MESSAGE_TYPE_FILE = "file";
const char* const MESSAGE_TYPE_EMOJI = "emoji";
const char* const MESSAGE_TYPE_SHAKE = "shake";

const char* const key_G_AV_DURATION = "duration";
const char* const key_G_RESOLUTION_WIDTH = "width";
const char* const key_G_RESOLUTION_HEIGHT = "height";

const INT64 THREAD_TIME_OUT = 5000;


const char *const key_G_CONFERENCE_TYPE = "type";
const char *const key_G_CONFERENCE_TYPE_VALUE = "conf";
const char *const key_G_CONFERENCE_ID = "id";
const char *const key_G_CONFERENCE_NUMBER = "extension_number";
const char *const key_G_CONFERENCE_SUBJECT = "subject";
const char *const key_G_CONFERENCE_MAX_NUMBER = "max_participants";
const char *const key_G_CONFERENCE_MODE = "mode";
const char *const key_G_CONFERENCE_RECORD = "recording";
const char *const key_G_CONFERENCE_LOCK = "locked";
const char *const key_G_CONFERENCE_MUTED = "muted";
const char *const key_G_CONFERENCE_PARTICIPANTS = "participants";
const char *const key_G_CONFERENCE_PARTICIPANT = "participant";
const char *const key_G_CONFERENCE_PARTICIPANT_MUTEED = "muted";
#include <windows.h>
class CLocalCriticalSection
{
public:
	CLocalCriticalSection()
	{
		InitializeCriticalSection(&m_cSection);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cSection);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_cSection);
	}

	virtual ~CLocalCriticalSection()
	{
		DeleteCriticalSection(&m_cSection);
	}
private:
	CRITICAL_SECTION                        m_cSection;
};

#endif
#ifndef __REST_API_H__
#define __REST_API_H__
#include "HttpClient.h"
#include "CriticalSection.h"
#define REST_COMMON_HTTP	"http://"
#define REST_COMMON_HTTPS	"https://"
#define REST_HTTP_PORT		":8899"
#define REST_HTTPS_PORT		":8900"
#define FILE_HTTP_PORT		":9333"
#define FILE_HTTP_PORT2     ":8080"

enum ENUM_LIST_MODE
{
	NORMAL,
	LAST_LIST,
	TIME_DISTANCE,
	SPECIFY_LIST_BEFORE,
	SPECIFY_LIST_AFTER
};
#define  REST_API_TIME_EXPIRES_STEP 20
#define  PAGE_SIZE "8"
#define  NUM_TOTAL_PAGE 1000
#define  OFF_LINE_MSG_COUNT 40
#define  LIST_MODE_DEFAULT "default"
const char* const key_G_VERSION_KEY = "api_version";

const char* const REST_API_URL_VERIFY = "/api/account/extension/sip/verify";
//ttl means save file 30 days
const char* const REST_API_URL_UPLOAD = "/submit?ttl=30d";
const char* const REST_API_URL_DELETE = "/delete";



const char* const REST_API_URL_MSG_EXTENSIONS_LIST = "/api/extensions/list";
const char* const REST_API_URL_MSG_GROUP_LIST = "/api/contacts/group/list";
const char* const REST_API_URL_MSG_REMOTE_CONTACT_LIST = "/api/contacts/list";
const char* const REST_API_URL_SYSTEM_EXTENTIONS = "/api/system_extensions/list";
const char* const REST_API_URL_SYSTEM_CONFERENCE_ROOM_LIST = "/api/conference_room/list";
const char* const REST_API_URL_VOICEMAIL_LIST = "/api/recordfiles/voicemail/list";
/************************************************************************/
/* about offline msg                                                    */
/************************************************************************/
const char* const REST_API_URL_CONTACT_LIST = "/api/comm_message/contact_list";
const char* const REST_API_URL_MSG_LIST = "/api/comm_message/list";
const char* const REST_API_URL_UNREAD_COUNT = "/api/comm_message/unread_count/show";
const char* const REST_API_URL_MSG_UPDATE = "/api/comm_message/update";
const char* const REST_API_URL_SEARCH_EXTERSIONS = "/api/extensions/search";
const char* const REST_API_URL_LIST_EXTERSIONS = "/api/extensions/list";

const char* const key_G_TOKEN = "access_token";
const char* const key_G_ID = "id";
const char* const key_G_DOMAIN = "domain";
const char* const key_G_NAME =  "extension_number";
const char* const key_G_PWD = "sip_password";
const char* const key_G_TENENTID = "tenantid";
const char* const key_G_FILEID = "fid";
const char* const key_G_FILEID_PAGESIZE = "pagesize";
const char* const key_G_SORT = "sort_by";
const char* const key_G_CONTACT_GROUP = "contact_grpid";
const char* const key_G_LIMIT_MSG_COUNT = "limit_count";


const char* const key_G_VERIFY_RESULT_KEY_EXPIRES = "expires";
const char* const key_G_UPDATE_KEY_MSGID = "msg_ids";

const char* const key_G_UPDATE_KEY_PAG = "pagination";
const char* const key_G_UPDATE_KEY_STATUS = "status";
const char* const key_G_CONTACT_RESULT_KEY_COUNT = "count";
const char* const key_G_CONTACT_RESULT_KEY_EXTENSIONS = "extensions";
const char* const key_G_CONTACT_RESULT_KEY_EXTENSION_NUM = "extension_number";


const char* const  key_G_UNREAD_KEY_SEND = "sender_extension";
const char* const  key_G_UNREAD_KEY_RECEIVER = "receiver_extension";
const char* const  key_G_UNREAD_RESULT_KEY_COUNT = "count";
const char* const  key_G_UNREAD_STATUS = "UNREAD";


const char* const  key_G_LIST_KEY_SENDER = "sender_extension";
const char* const  key_G_LIST_KEY_RECEIVER = "receiver_extension";
const char* const  key_G_LIST_KEY_PAG = "pagination";
const char* const  key_G_LIST_KEY_MESSAGEID = "specify_msg_id";
const char* const  key_G_LIST_KEY_MOD = "list_mode";
const char* const  key_G_LIST_KEY_TIME_STAR = "time_start";
const char* const  key_G_LIST_KEY_TIME_STOP = "time_end";

const char* const  key_G_SCHEAM_LIST_MOD_MORMAL = "NORMAL";
const char* const  key_G_SCHEAM_LIST_MOD_AFTER = "SPECIFY_LIST_AFTER";
const char* const  key_G_SCHEAM_LIST_MOD_BEFORE = "SPECIFY_LIST_BEFORE"; 
const char* const  key_G_SCHEAM_LIST_MOD_TIME = "TIME_DISTANCE";
const char* const  key_G_SCHEAM_LIST_MOD_LAST = "LAST_LIST";
const char* const  key_G_SCHEAM_LIST_MOD_NORMAL = "NORMAL";
const char* const  key_G_UPLOAD_KEY_MEDIATYPE = "media_type";



const char* const  key_G_TRANSPORT = "transports";
const char* const  key_G_TRANSTYPE = "protocol";
const char* const  key_G_PORT = "port";

const char* const key_G_MESSAGE_TYPE = "type";
const char* const key_G_TEXT_CONTENT = "content";


const char* const key_G_FILE_NAME = "fileName";
const char* const key_G_FILE_PATH = "filePath";
const char* const key_G_FILE_SIZE = "fileSize";
const char* const key_G_FILE_URL = "url";
const char* const key_G_FILE_MIME_TYPE = "mime";


struct TagUnreadMsgNumInfo 
{
	std::string m_strSend;	
	std::string m_strRecvl;	
	int			m_nNum;
};
struct  TagUnreadMsgInfo
{
	std::string	m_strSender;
	long long     m_llMsgID;	
	std::string   m_strBody;	
	std::string   m_strMsgType; 
	long		  m_lPostTime;  
	bool          m_bIsMySelf;  
};

class CRestAPI:public CHttpClient
{
public:
	CRestAPI(std::string strHttpURL/*,std::string strHttpsURL*/);
	~CRestAPI();

	std::string GetUserServerExtersionID() {
		return m_strID;
	}
	bool RestAPILogin(std::string strUserName, std::string strPwd,std::string strSIPDomain,std::string strProtcol,int& nSipDomainPort);
	bool ParseLoginResult(std::string strResponse, std::string strProtcol,int& nSipDomainPort);
	std::vector<std::string> GetRecentContacts();
	void  ParseContacts(std::string strResponse, std::vector<TAG_RECENT*> & vecContact,int& nCurrentNum);
	void ParseConferenceRoomID(std::string strResponse, std::vector<string > &vecConference, int &nCurrentNum);
	std::vector<string> ParseRecentContacts(std::string strResponse);
	vector<CChatRecord> GetOffLineMsg(std::string strReceiverSipURL, std::string strSendExtensionURL,std::string  strLastMsgID, ENUM_LIST_MODE enumListMode = SPECIFY_LIST_AFTER,int nPageCount =10);
	void       GetOffLineNum(std::string strReceiverSipNumber, std::string strSendExtension);
	void  UpdateOffLineMsg(vector<long long > vecOffLineMsgID);
	void  ParseRemoteContacts(std::string strResponse, std::vector<TAG_RECENT*> & vecContact, int& nCurrentNum);
	std::vector<INT64> GetContactGroup(std::string strResponse);
	std::vector<TAG_RECENT*> GetRemoteContacts();
	std::vector<TAG_RECENT*> GetRemoteContactsFromGroup(INT64 n64GroupID,int nStartPage,int &nCurrentNum);
	std::vector<TAG_RECENT*> GetExtensions();
	std::string GetVoiceMailNum();
	std::string GetExtensionID(std::string strSipNuber);
	std::string ParseExtensionID(std::string strResponse, string strSipNumber, bool &bFind,int& nDataSize);
	//std::vector<TagUnreadMsgNumInfo> GetUnreadMsgNumInfo();
	//std::vector<TagUnreadMsgInfo>	GetUnreadMsgAllInfo();
	/************************************************************************/
	/*about file serveice                                                   */
	/************************************************************************/
	std::string GetMessageURL(std::string  strScheam);
	std::string GetFileURL(std::string strScheam);
	std::string  UpLoadFile(const std::string strFilePath, std::string strFileName, std::string strMediaType, UpLoadDownLoadParam* pDownloadParam);
	void DownLoadFile(const std::string strFileURL, std::string strFileName, std::string strDestFilePath, UpLoadDownLoadParam * pDownloadParam);
	bool DownLoadVoiceMail(const std::string strFileURL, std::string strFileName, std::string strDesFilePath, VoiceMailDownLoadParam cbVoiceMailParam);
	void ListRecordVoiceMail(vector<VoiceMailParam> &vecVoiceMail);
	void ProcessOffLineMsg(TagUnreadMsgInfo stuUnReadMsgInfo, std::string strSender, CChatRecord &oChatRecord);


protected:
	void IsTimeOutRfresh();
	
private:
	std::string  m_strHttpURL;
	std::string  m_strToken;
	int          m_nResponseCode;
	int          m_nExpires;
	long         m_lLoginTime;

	string		m_strUserName;
	string		m_strPwd;
	string		m_strDomain;
	
	std::string m_strExtensionToken;
	CCriticalSection			m_csRestAPI;
	std::string					m_strProtocol;
	std::string                 m_strID;
	std::string                 m_strVersion;
};

#endif
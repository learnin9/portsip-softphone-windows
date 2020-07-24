#include "StdAfx.h"
#include "RestAPI.h"
#include<ctime>
#include "SystemEx.h"


CRestAPI::CRestAPI(std::string strHttpURL/*,std::string strHttpsURL*/)
{
	m_strHttpURL = strHttpURL;
	m_strToken = "";
	m_nResponseCode = 0;
	m_strUserName = "";
	m_strPwd	  = "";
	m_strDomain	  = "";
	m_strProtocol = "UDP";
	m_strID = "";
	m_strVersion = "";
}


CRestAPI::~CRestAPI()
{

}

bool CRestAPI::RestAPILogin(std::string strUserName, std::string strPwd, std::string strSIPDomain, string strProtcol,int &nSipDomainPort)
{
	CCriticalAutoLock loLock(m_csRestAPI);
	///account/credentials/verify
	std::string strRequstURL = REST_COMMON_HTTP +m_strHttpURL + REST_HTTP_PORT + REST_API_URL_VERIFY;
	Json::Value info;
	Json::FastWriter writer;
	info[key_G_NAME] = strUserName;
	info[key_G_PWD] = strPwd;
	//TODO 
	info[key_G_DOMAIN] = strSIPDomain;

	std::string strInParam = writer.write(info);
	std::string strResponse;
#if _DEBUG
	TIMER_START(RestAPILogin);
#endif
	int nResult = Post(strRequstURL, strInParam, strResponse);
#if _DEBUG
	TIMER_STOP(RestAPILogin);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" RestAPILogin  Cost %d ms.\n", TIMER_MSEC(RestAPILogin));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	if (nResult==0)
	{
		
		bool bRet = ParseLoginResult(strResponse, strProtcol,nSipDomainPort);
		if (bRet ==true)
		{
			m_strUserName = strUserName;
			m_strPwd      = strPwd;
			m_strDomain   = strSIPDomain;
		}
		
		return bRet;
	}
	else
	{
		LOG4_INFO("HTTP REQUST account/credentials/verify FAILED");
		return false;
	}
	
}

bool CRestAPI::ParseLoginResult(std::string strResponse, std::string strProtcol,int& nSipDomainPort)
{
	if (strResponse.empty()==true)
	{
		return false;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;
	
	if (!json_reader.parse(strResponse ,json_root))
	{
		LOG4_ERROR("ParseLoginResult json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return	false;
	}
	m_strToken = json_root[key_G_TOKEN].asString();
	m_strID	   = json_root[key_G_ID].asString();
	m_nExpires = json_root[key_G_VERIFY_RESULT_KEY_EXPIRES].asInt();
	m_strVersion = json_root[key_G_VERSION_KEY].asString();

	for (unsigned int i = 0; i < json_root[key_G_TRANSPORT].size(); i++)
	{
		string name = json_root[key_G_TRANSPORT][i][key_G_TRANSTYPE].asString();
		if (PortUtility::compareStringNoCase(name, strProtcol) == 0)
		{
			nSipDomainPort = json_root[key_G_TRANSPORT][i][key_G_PORT].asInt();
		}

		
	}
	
	// int n
	//nSipDomainPort = json_root[]
	if (m_strToken.empty())
	{
		LOG4_ERROR("HTTP Server Return No OK ");
		return false;
	}
	m_lLoginTime = clock();	

	return true;
}
std::vector<TAG_RECENT*> CRestAPI::GetExtensions()
{
	std::vector<TAG_RECENT*> vecLocalContact;
	vecLocalContact.empty();

	bool bHasMoreData = true;
	int nPageStart = 1;
	while (bHasMoreData)
	{
		int nCurrentNum = 0;
		IsTimeOutRfresh();
		std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_MSG_EXTENSIONS_LIST;
		std::string strResponse = "";
		std::vector<std::string> vecHttpHeaderKeyValue;
		std::string strKey = key_G_TOKEN;
		std::string strInParam = strKey + ": " + m_strToken;
	
		map<string, string> mapInParam;
		mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));
		std::stringstream strFormat;
		strFormat << nPageStart;
		//map<string, string> mapGetParam;
		//mapGetParam.insert(make_pair(key_G_UPDATE_KEY_PAG , strFormat.str()));
		//mapGetParam.insert(make_pair(key_G_FILEID_PAGESIZE, PAGE_SIZE));
		//mapGetParam.insert(make_pair(key_G_SORT, LIST_MODE_DEFAULT));

		vector<UrlParam> vecUrlParam;
		UrlParam theGetParam;
		theGetParam.strKey = key_G_UPDATE_KEY_PAG;
		theGetParam.strValue = strFormat.str();
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_FILEID_PAGESIZE;
		theGetParam.strValue = PAGE_SIZE;
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_SORT;
		theGetParam.strValue = LIST_MODE_DEFAULT;
		vecUrlParam.push_back(theGetParam);

		std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);

		CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
		TIMER_START(ParseLoginResult);
#endif
		int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
		TIMER_STOP(ParseLoginResult);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" ParseLoginResult  Cost %d ms.\n", TIMER_MSEC(ParseLoginResult));
		LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
		
		if (nResult == 0)
		{
			ParseContacts(strResponse, vecLocalContact, nCurrentNum);
		}
		else
		{
			LOG4_INFO("HTTP REQUST account/credentials/verify FAILED");
		}
		if (nCurrentNum>=atoi(PAGE_SIZE)&& vecLocalContact.size()<=NUM_TOTAL_PAGE)
		{
			nPageStart++;
			bHasMoreData = true;
		}
		else
		{
			bHasMoreData = false;
		}
	}
	return vecLocalContact;
	//TODO:nPageNum NO REAL RETURN

}
std::vector<TAG_RECENT*> CRestAPI::GetRemoteContactsFromGroup(INT64 n64GroupID, int nStartPage, int &nCurrentNum)
{
	std::vector<TAG_RECENT*> vecLocalContact;
	vecLocalContact.empty();
	
		IsTimeOutRfresh();
		std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_MSG_REMOTE_CONTACT_LIST;
		std::string strResponse = "";
		std::vector<std::string> vecHttpHeaderKeyValue;
		std::string strKey = key_G_TOKEN;
		std::string strInParam = strKey + ": " + m_strToken;

		map<string, string> mapInParam;
		mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));

		std::stringstream strFormat;
		//map<string, string> mapGetParam;
		//mapGetParam.insert(make_pair(key_G_UPDATE_KEY_PAG, strFormat.str()));
		//mapGetParam.insert(make_pair(key_G_FILEID_PAGESIZE, strFormat.str()));
		//mapGetParam.insert(make_pair(key_G_SORT, LIST_MODE_DEFAULT));
		//strFormat.clear();
		//strFormat << n64GroupID;
		//mapGetParam.insert(make_pair(key_G_CONTACT_GROUP, strFormat.str()));

		strFormat << nStartPage - 1;
		vector<UrlParam> vecUrlParam;
		UrlParam theGetParam;
		theGetParam.strKey = key_G_UPDATE_KEY_PAG;
		theGetParam.strValue = strFormat.str();
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_FILEID_PAGESIZE;
		theGetParam.strValue = PAGE_SIZE;
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_SORT;
		theGetParam.strValue = LIST_MODE_DEFAULT;
		vecUrlParam.push_back(theGetParam);
		strFormat.clear();
		strFormat << n64GroupID;
		theGetParam.strKey = key_G_CONTACT_GROUP;
		theGetParam.strValue = strFormat.str();
		vecUrlParam.push_back(theGetParam);

		std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
#if _DEBUG
		TIMER_START(GetRemoteContactsFromGroup);
#endif
		int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
		TIMER_STOP(GetRemoteContactsFromGroup);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" GetRemoteContactsFromGroup  Cost %d ms.\n", TIMER_MSEC(GetRemoteContactsFromGroup));
		LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
		if (nResult == 0)
		{
			ParseRemoteContacts(strResponse, vecLocalContact,nCurrentNum);
		}
		else
		{
			LOG4_INFO("HTTP REQUST REST_API_URL_MSG_GROUP_LIST FAILED");
		}
	
	return vecLocalContact;
}
std::vector<TAG_RECENT*> CRestAPI::GetRemoteContacts()
{
	std::vector<TAG_RECENT*> vecLocalContact;
	vecLocalContact.empty();

	int nPageStart = 1;
	int nCurrentNumGroup = 0;
	IsTimeOutRfresh();
	std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_MSG_GROUP_LIST;
	std::string strResponse = "";
	std::vector<std::string> vecHttpHeaderKeyValue;
	std::string strKey = key_G_TOKEN;
	std::string strInParam = strKey + ": " + m_strToken;

		map<string, string> mapInParam;
		mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));

		std::stringstream strFormat;
		strFormat << nPageStart - 1;
		//map<string, string> mapGetParam;
		//mapGetParam.insert(make_pair(key_G_UPDATE_KEY_PAG, strFormat.str()));
		//mapGetParam.insert(make_pair(key_G_FILEID_PAGESIZE, "15"));
		//mapGetParam.insert(make_pair(key_G_SORT, LIST_MODE_DEFAULT));

		vector<UrlParam> vecUrlParam;
		UrlParam theGetParam;
		theGetParam.strKey = key_G_UPDATE_KEY_PAG;
		theGetParam.strValue = strFormat.str();
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_FILEID_PAGESIZE;
		theGetParam.strValue = PAGE_SIZE;
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_SORT;
		theGetParam.strValue = LIST_MODE_DEFAULT;
		vecUrlParam.push_back(theGetParam);


		std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
		CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
		TIMER_START(GetRemoteContacts);
#endif
		int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
		TIMER_STOP(GetRemoteContacts);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" GetRemoteContacts  Cost %d ms.\n", TIMER_MSEC(GetRemoteContacts));
		LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
		vector<INT64> vecContantGroup;
		if (nResult == 0)
		{
			vecContantGroup = GetContactGroup(strResponse);
			if (vecContantGroup.size() > 0)
			{
				auto iterContactGroup = vecContantGroup.begin();
				for (iterContactGroup; iterContactGroup !=vecContantGroup.end(); iterContactGroup++)
				{
					std::vector<TAG_RECENT*> vecLocalContactGroup;
					INT64 nGroupID = *iterContactGroup;
					vecLocalContactGroup = GetRemoteContactsFromGroup(nGroupID, nPageStart, nCurrentNumGroup);
					if (vecLocalContactGroup.size()>=0&& vecLocalContact.size()<MAX_CALL_VIEW_NUM*MAX_PAGE_SHOW_CONTACT)
					{
						vecLocalContact.insert(vecLocalContact.end(), vecLocalContactGroup.begin(), vecLocalContactGroup.end());
					}
					else
					{
						break;
					}
					
				}
			}
		}
		else
		{
			LOG4_INFO("HTTP REQUST account/credentials/verify FAILED");
		}
	
	return vecLocalContact;
}
std::vector<INT64> CRestAPI::GetContactGroup(std::string strResponse)
{
	std::vector<INT64> theContantGroup;
	theContantGroup.clear();

	if (strResponse.empty() == true)
	{
		LOG4_INFO("HTTP GetContactGroup empty");
		return theContantGroup;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("GetContactGroup json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return theContantGroup;
	}

	Json::Value json_contactGroups = json_root["contactGroups"];
	if (json_contactGroups.isArray() == false)
	{
		LOG4_ERROR("GetContactGroup json parse error json_contacts si not a array \n");
		return theContantGroup;
	}

	

	for (int i = 0; i < json_contactGroups.size(); i++)
	{
		long long lContactGroup = 0;
		if (json_contactGroups[i]["id"].isString())
		{
			PortUtility::stringToLL(json_contactGroups[i]["id"].asString(), lContactGroup);
		}
		if (json_contactGroups[i]["id"].isInt64())
		{
			lContactGroup = json_contactGroups[i]["id"].asInt64();
		}
		INT64 i64contactGroup = lContactGroup;
		theContantGroup.push_back(i64contactGroup);
	}

	return theContantGroup;
}
std::vector<string> CRestAPI::GetRecentContacts()
{
	std::vector<string> vecLocalContact;
	vecLocalContact.empty();
	IsTimeOutRfresh();
	//extensions / call
	//api/comm_message/contact_list
	std::string strResponse;
	std::string strRequstURL = REST_COMMON_HTTP+m_strHttpURL + REST_HTTP_PORT + REST_API_URL_CONTACT_LIST;
	std::string strKey = key_G_TOKEN;
	std::string strInParam = strKey + ": " + m_strToken;
	std::vector<std::string> vecHttpHeaderKeyValue;
	vecHttpHeaderKeyValue.push_back(strInParam);
	strKey = key_G_UPDATE_KEY_PAG;
	strInParam = strKey + ": " + "1";
	vecHttpHeaderKeyValue.push_back(strInParam);
	//map<string, string> mapGetParam;
	//mapGetParam.insert(make_pair(key_G_UPDATE_KEY_STATUS, key_G_UNREAD_STATUS));
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_PAG, "0"));

	vector<UrlParam> vecUrlParam;
	UrlParam theGetParam;
	theGetParam.strKey = key_G_UPDATE_KEY_STATUS;
	theGetParam.strValue = key_G_UNREAD_STATUS;
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_LIST_KEY_PAG;
	theGetParam.strValue = "0";
	vecUrlParam.push_back(theGetParam);

	std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
	
	CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
	TIMER_START(GetRecentContacts);
#endif
	int nResult = Get(strRequstTemplateURL, vecHttpHeaderKeyValue, strResponse);
#if _DEBUG
	TIMER_STOP(GetRecentContacts);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" GetRecentContacts  Cost %d ms.\n", TIMER_MSEC(GetRecentContacts));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	
	if (nResult == 0)
	{
		vecLocalContact = ParseRecentContacts(strResponse);
	}
	else
	{
		LOG4_INFO("HTTP REQUST account/credentials/verify FAILED");
	}
	return vecLocalContact;
}
std::vector<string> CRestAPI::ParseRecentContacts(std::string strResponse)
{
	std::vector<string> vecLocalContact;
	if (strResponse.empty() == true)
	{
		LOG4_INFO("HTTP Extension empty");
		return vecLocalContact;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return vecLocalContact;
	}

	Json::Value json_contacts = json_root["extensions"];
	if (json_contacts.isArray() == false)
	{
		LOG4_ERROR("json parse error json_contacts si not a array \n");
		return vecLocalContact;
	}
	for (int i = 0; i < json_contacts.size(); i++)
	{
		std::string strSipNumber = json_contacts[i]["extension_number"].asString();
		vecLocalContact.push_back(strSipNumber);
	}
	return vecLocalContact;
}


void CRestAPI::ParseRemoteContacts(std::string strResponse, std::vector<TAG_RECENT *> & vecContact, int& nCurrentNum)
{
	if (strResponse.empty() == true)
	{
		LOG4_INFO("HTTP Extension empty");
		return;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("ParseRemoteContacts json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return;
	}

	Json::Value json_contacts = json_root["contacts"];
	if (json_contacts.isArray() == false)
	{
		LOG4_ERROR("json parse error json_contacts si not a array \n");
		return;
	}
	for (int i = 0; i < json_contacts.size(); i++)
	{
		TAG_RECENT *pContactInfo = new TAG_RECENT;
		CContactDB* pLocalContact = new CContactDB;
		pContactInfo->m_enumRecentContact = ENUM_SINGLE_CONTACT;
		pContactInfo->m_unionContactInfo.m_pContact = pLocalContact;
		pLocalContact->m_strSIPNum = json_contacts[i]["mobile_phone"].asString();
		pLocalContact->m_strFirstName = json_contacts[i]["first_name"].asString();
		pLocalContact->m_strLastName = json_contacts[i]["last_name"].asString();
		pLocalContact->m_eOnlineState = ONLINESTATE_ONLINE;
		pLocalContact->m_strEmailAdress = json_contacts[i]["email"].asString();

		int  nRand = rand() % 6;
		CIUIString strImage;
		// 在vs2017中，必须是GetRunFolder().GetBuffer(0)，不能是GetRunFolder()，否则格式化后，永远是0.png
		//strImage.Format(_T("%sSkins\\PortGo\\%d.png"), GetRunFolder().GetBuffer(0), nRand);
		strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));
		std::wstring strImagePath = strImage;
		pLocalContact->m_strHeadImage = PortUtility::Unicode2Utf8_ND(strImagePath);

		vecContact.push_back(pContactInfo);
	}
	nCurrentNum = json_contacts.size();
}
void CRestAPI::ParseContacts(std::string strResponse, std::vector<TAG_RECENT*> & vecContact,int& nCurrentNum)
{
	nCurrentNum = 0;
	if (strResponse.empty()==true)
	{
		LOG4_INFO("HTTP Extension empty");
		return;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("ParseContacts json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return	;
	}
	
	Json::Value json_contacts = json_root["extensions"];
	if (json_contacts.isArray() == false)
	{
		LOG4_ERROR("json parse error json_contacts si not a array \n");
		return;
	}
	for (int i = 0; i < json_contacts.size(); i++) 
	{
		TAG_RECENT *pContactInfo = new TAG_RECENT;
		CContactDB* pLocalContact = new CContactDB;
		pContactInfo->m_enumRecentContact = ENUM_SINGLE_CONTACT;
		pContactInfo->m_unionContactInfo.m_pContact = pLocalContact;
		pLocalContact->m_strSIPNum    = json_contacts[i]["extension_number"].asString();
		pLocalContact->m_strFirstName = json_contacts[i]["first_name"].asString();
		pLocalContact->m_strLastName  = json_contacts[i]["last_name"].asString();
		std::string strOnlineStatus   = json_contacts[i]["status"].asString();
		int  nRand = rand() % 6;
		CIUIString strImage;
		// 在vs2017中，必须是GetRunFolder().GetBuffer(0)，不能是GetRunFolder()，否则格式化后，永远是0.png
		//strImage.Format(_T("%sSkins\\PortGo\\%d.png"), GetRunFolder().GetBuffer(0), nRand);
		strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));
		std::wstring strImagePath = strImage;
		pLocalContact->m_strHeadImage = PortUtility::Unicode2Utf8_ND(strImagePath);

		if (PortUtility::compareStringNoCase("OFFLINE",strOnlineStatus)==0)
		{
			pLocalContact->m_eOnlineState = ONLINESTATE_OFFLINE;
		}
		else
		{
			pLocalContact->m_eOnlineState = ONLINESTATE_ONLINE;
		}
		pLocalContact->m_strEmailAdress = json_contacts[i]["email"].asString();
		vecContact.push_back(pContactInfo);
	}
	nCurrentNum = json_contacts.size();
}

void CRestAPI::ParseConferenceRoomID(std::string strResponse, std::vector<string > &vecConference, int &nCurrentNum)
{
	nCurrentNum = 0;
	if (strResponse.empty() == true)
	{
		LOG4_INFO("HTTP ConferenceRoom empty");
		return;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("ParseConferenceRoomID json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return;
	}

	Json::Value json_conference_rooms = json_root["servers"];
	if (json_conference_rooms.isArray() == false)
	{
		LOG4_ERROR("json parse error json_conference_rooms si not a array \n");
		return;
	}
	for (int i = 0; i < json_conference_rooms.size(); i++)
	{
		std::string strConferenceNumber = json_conference_rooms[i][key_G_ID].asString();
		vecConference.push_back(strConferenceNumber);
	}
	nCurrentNum = json_conference_rooms.size();
}
std::string CRestAPI::GetExtensionID(std::string strSipNuber)
{
	IsTimeOutRfresh();
	bool bHasMoreData = true;
	int nPageStart = 1;
	std::string strSipID = "";
	while (bHasMoreData)
	{
		int nCurrentNum = 0;

		std::string strResponse;
		
		///
		std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + "/api/extensions/list";// REST_API_URL_SEARCH_EXTERSIONS;
		map<string, string> mapInParam;
		mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));

		std::stringstream strFormat;
		strFormat << nPageStart;
		vector<UrlParam> vecUrlParam;
		UrlParam theGetParam;
		theGetParam.strKey = key_G_UPDATE_KEY_PAG;
		theGetParam.strValue = strFormat.str();
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_FILEID_PAGESIZE;
		theGetParam.strValue = PAGE_SIZE;
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_SORT;
		theGetParam.strValue = LIST_MODE_DEFAULT;
		vecUrlParam.push_back(theGetParam);
		theGetParam.strKey = key_G_CONTACT_RESULT_KEY_EXTENSION_NUM;
		theGetParam.strValue = strSipNuber;
		//vecUrlParam.push_back(theGetParam);

		std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
		CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
		TIMER_START(GetExtensionID);
#endif
		int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
		TIMER_STOP(GetExtensionID);
		CIUIString strFormatInfo;
		strFormatInfo.Format(L" GetExtensionID  Cost %d ms.\n", TIMER_MSEC(GetExtensionID));
		LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
		if (nResult == 0)
		{
			bool bFind = false;
			int nDataSize = 0;
			strSipID = ParseExtensionID(strResponse, strSipNuber, bFind, nDataSize);
			if (bFind == true)
			{
				break;
			}
			if (nDataSize >= atoi(PAGE_SIZE))
			{
				nPageStart++;
				bHasMoreData = true;
			}
			else
			{
				bHasMoreData = false;
			}
		}
		else
		{
			LOG4_INFO("HTTP REQUST account/credentials/verify FAILED");
		}
	}
	return strSipID;
}
std::string CRestAPI::ParseExtensionID(std::string strResponse,string strSipNumber,bool &bFind,  int& nDataSize)
{
	bFind = false;
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;
	std::string strNumber = "";
	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("ParseExtensionID json parse error \n");
		LOG4_INFO(strResponse.c_str());
		return "";;
	}

	Json::Value json_extensions = json_root["extensions"];


	if (!json_extensions.isNull() && json_extensions.isArray())
	{
		nDataSize = json_extensions.size();
		for (int i = 0; i < json_extensions.size(); i++)
		{
			std::string strSipNumberLocal = json_extensions[i]["extension_number"].asString();
			if (PortUtility::compareStringNoCase(strSipNumber,strSipNumberLocal)==0)
			{
				strNumber = json_extensions[i]["id"].asString();
				bFind = true;
				break;
			}
		}
		
	}
	return strNumber;
}
std::string  CRestAPI::GetVoiceMailNum()
{
	IsTimeOutRfresh();
	/// voice_mail / show
	std::string strResponse ;
	std::string strRequstURL = REST_COMMON_HTTP+ m_strHttpURL + REST_HTTP_PORT + "voice_mail/show";
	CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
	TIMER_START(GetVoiceMailNum);
#endif
	int nResult = Get(strRequstURL, strResponse);
#if _DEBUG
	TIMER_STOP(GetVoiceMailNum);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" GetVoiceMailNum  Cost %d ms.\n", TIMER_MSEC(GetVoiceMailNum));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	if (nResult == 0)
	{
		Json::Reader json_reader(Json::Features::strictMode());
		Json::Value json_root;

		if (!json_reader.parse(strResponse, json_root))
		{
			LOG4_ERROR("GetVoiceMailNum json parse error \n");
			return "";;
		}

		Json::Value json_voice_mail_num = json_root["voice_mail_number"];
		
		if (!json_voice_mail_num.isNull()&&json_voice_mail_num.isString())
		{
			return json_voice_mail_num.asString();
		}
	}
	else
	{
		LOG4_INFO("HTTP REQUST voice_mail/show FAILED");
	}
	return "";
}

void CRestAPI::IsTimeOutRfresh()
{
	long nowTime = clock();		//程序结束用时
	double endtime = (double)(nowTime - m_lLoginTime) / CLOCKS_PER_SEC;
	
	int nEexpendTime = (int)endtime + REST_API_TIME_EXPIRES_STEP - m_nExpires;
	if (nEexpendTime>=0)
	{
		if (!m_strUserName.empty() && !m_strPwd.empty() && !m_strDomain.empty())
		{
			int nSipPort = 0;
			RestAPILogin(m_strUserName, m_strPwd, m_strDomain, m_strProtocol,nSipPort);
		}
	}
}

std::string CRestAPI ::GetMessageURL(std::string  strScheam) {
	return REST_COMMON_HTTP + m_strHttpURL  + REST_HTTP_PORT + strScheam;
}

std::string CRestAPI::GetFileURL(std::string  strScheam) {
	return REST_COMMON_HTTP + m_strHttpURL  + FILE_HTTP_PORT + strScheam;
}
std::string  CRestAPI::UpLoadFile(const std::string strFilePath, std::string strFileName,  std::string strMediaType, UpLoadDownLoadParam* pUploadParam )
{
	IsTimeOutRfresh();
	std::string strURL = GetFileURL(REST_API_URL_UPLOAD);
	std::string strResponse = "";
	map<string, string> mapInParam;
	mapInParam.insert(make_pair(key_G_UPLOAD_KEY_MEDIATYPE, "application/x-export-json"));
	mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));

	CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
	TIMER_START(UploadFile);
#endif
	
	UploadFile(strURL, strFilePath, strFileName, mapInParam, pUploadParam,strResponse);
#if _DEBUG
	TIMER_STOP(UploadFile);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" UploadFile %s  Cost %d ms.\n", strFileName.c_str(),TIMER_MSEC(UploadFile));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif 
	std::string strUpLoadURL = "";
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(strResponse, json_root))
	{
		LOG4_ERROR("UpLoadFile json parse error \n");
		return "";
	}

	LOG4_INFO(strResponse.c_str());
	Json::Value jsonError = json_root["error"];
	if (jsonError.isString())
	{
		std::string strError = jsonError.asString();
		return "";
	}

	Json::Value jsonFileURL = json_root["fileUrl"];
	if (!jsonFileURL.isNull() && jsonFileURL.isString())
	{
		strUpLoadURL = jsonFileURL.asString();

		//DWORD dwSizeHigh;
		std::string strTemplatePath = strFilePath + "//" + strFileName;
		std::wstring wstrTemlatePath = PortUtility::string2WString(strTemplatePath);
		ULARGE_INTEGER uliFileSize;
		uliFileSize.LowPart = GetCompressedFileSize(wstrTemlatePath.c_str(),
			&uliFileSize.HighPart);

		pUploadParam->m_lFileSize = uliFileSize.QuadPart;
		pUploadParam->m_strHttpUrl = /*REST_COMMON_HTTP +*/ strUpLoadURL;
		HWND hMsgProcess = pUploadParam->m_hWndMsgProcess;
		if (hMsgProcess != NULL)
		{
			::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_UPLOAD_FILE_GET_OK, (WPARAM)pUploadParam, 0);
		}
	}
	return strUpLoadURL;
}

void CRestAPI::DownLoadFile(const std::string strFileURL, std::string strFileName, std::string strDestFilePath, UpLoadDownLoadParam* pDownloadParam)
{
	//CCriticalAutoLock loLock(m_csRestAPI);
	HTTPDownLoadFile(strFileURL, strDestFilePath, strFileName, pDownloadParam);
}
bool CRestAPI::DownLoadVoiceMail(const std::string strFileURL, std::string strFileName, std::string strDesFilePath, VoiceMailDownLoadParam cbVoiceMailParam)
{
	CCriticalAutoLock loLock(m_csRestAPI);
	std::string strTrulyURL;
	strTrulyURL = REST_COMMON_HTTP + m_strHttpURL + FILE_HTTP_PORT +"/"+ strFileURL;
	return HTTPDownLoadVoiceMail(strTrulyURL, strDesFilePath , strFileName, cbVoiceMailParam);
}
void  CRestAPI::GetOffLineNum(std::string strReceiverSipNumber, std::string strSendExtension)
{
	if (strReceiverSipNumber.empty()||strSendExtension.empty())
	{
		return;
	}
	CCriticalAutoLock loLock(m_csRestAPI);
	////api/comm_message/unread_count/show
	std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_UNREAD_COUNT;
	std::stringstream ssValue;

	map<string, string> mapInParam;
	mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));

	//map<string, string> mapGetParam;
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_SENDER, strSendExtension));
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_RECEIVER, strReceiverSipNumber));

	vector<UrlParam> vecUrlParam;
	UrlParam theGetParam;
	theGetParam.strKey = key_G_LIST_KEY_SENDER;
	theGetParam.strValue = strSendExtension;
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_LIST_KEY_RECEIVER;
	theGetParam.strValue = strReceiverSipNumber;
	vecUrlParam.push_back(theGetParam);

	std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
	std::string strResponse;
#if _DEBUG
	TIMER_START(GetOffLineNum);
#endif


	int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
	TIMER_STOP(GetOffLineNum);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" GetOffLineNum   Cost %d ms.\n",  TIMER_MSEC(GetOffLineNum));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	if (nResult == 0)
	{
	}
}
void  CRestAPI::UpdateOffLineMsg(vector<long long > vecOffLineMsgID)
{
	if (vecOffLineMsgID.size() == 0)
		return;
	IsTimeOutRfresh();
	CCriticalAutoLock loLock(m_csRestAPI);
	///account/credentials/verify
	std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_MSG_UPDATE;
	Json::Value info;
	Json::FastWriter writer;
	Json::Value ITEMS;
	auto iterOffLineMsgID = vecOffLineMsgID.begin();
	for (iterOffLineMsgID; iterOffLineMsgID!=vecOffLineMsgID.end(); iterOffLineMsgID++)
	{
		std::stringstream ssOffLineMsgID;
		long long nOffLineMsgID = *iterOffLineMsgID;
		ssOffLineMsgID << nOffLineMsgID;
		ITEMS.append(ssOffLineMsgID.str());
	};
	info[key_G_UPDATE_KEY_MSGID] = ITEMS;
	//TODO 
	//info[key_G_TOKEN] = m_strToken;

	map<string, string> mapHeaderParam;
	mapHeaderParam.insert(make_pair(key_G_TOKEN, m_strToken));
	std::string strInParam = writer.write(info);
	std::string strResponse;
#if _DEBUG
	TIMER_START(UpdateOffLineMsg);
#endif



	int nResult = Post(strRequstURL, mapHeaderParam, strInParam, strResponse);
#if _DEBUG
	TIMER_STOP(UpdateOffLineMsg);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" UpdateOffLineMsg  Cost %d ms.\n", TIMER_MSEC(UpdateOffLineMsg));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	if (nResult == 0)
	{
	}
}
vector<CChatRecord>  CRestAPI::GetOffLineMsg(std::string strReceiverSipURL, std::string strSendExtensionURL, std::string  strLastMsgID, ENUM_LIST_MODE enumListMode, int nPageCount)
{
	IsTimeOutRfresh();
#if _DEBUG
	TIMER_START(GetOffLineMsgPre);
#endif
	std::string strResponse;
	std::string strRequstURL = REST_COMMON_HTTP + m_strHttpURL + REST_HTTP_PORT + REST_API_URL_MSG_LIST;
	
	vector<CChatRecord> vecChatRecord;
	map<string, string> mapInParam;
	mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));
	//map<string, string> mapGetParam;
	//ssValue << 247531682746470400;
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_MESSAGEID, ssValue.str()));
	//mapGetParam.insert(make_pair(key_G_SORT, key_G_SCHEAM_LIST_MOD_AFTER));////key_G_SCHEAM_LIST_MOD_AFTER
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_SENDER, strSendExtension));
	//mapGetParam.insert(make_pair(key_G_LIST_KEY_RECEIVER, strReceiverSipNumber));
	vector<UrlParam> vecUrlParam;
	UrlParam theGetParam;

	switch (enumListMode)
	{
	case LAST_LIST:
	case TIME_DISTANCE:
		LOG4_INFO(L"un suport this mode");
		return vecChatRecord;
		break;
	case NORMAL:
		theGetParam.strKey = key_G_LIST_KEY_MOD;
		theGetParam.strValue = key_G_SCHEAM_LIST_MOD_NORMAL;
		vecUrlParam.push_back(theGetParam);
		break;
	case SPECIFY_LIST_BEFORE:
	
		theGetParam.strKey = key_G_LIST_KEY_MOD;
		theGetParam.strValue = key_G_SCHEAM_LIST_MOD_BEFORE;
		vecUrlParam.push_back(theGetParam);
		break;
	case SPECIFY_LIST_AFTER:
		theGetParam.strKey = key_G_LIST_KEY_MOD;
		theGetParam.strValue = key_G_SCHEAM_LIST_MOD_AFTER;
		vecUrlParam.push_back(theGetParam);
		break;
	default:
		break;
	}
	if (enumListMode == SPECIFY_LIST_BEFORE || enumListMode == SPECIFY_LIST_AFTER)
	{
		theGetParam.strKey = key_G_LIST_KEY_MESSAGEID;
		theGetParam.strValue = strLastMsgID;
		vecUrlParam.push_back(theGetParam);
	}


	theGetParam.strKey = key_G_LIST_KEY_SENDER;
	theGetParam.strValue = strSendExtensionURL; //strSendExtensionURL; //strReceiverSipURL;// 
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_LIST_KEY_RECEIVER;
	theGetParam.strValue = strReceiverSipURL;// strReceiverSipURL; //;// 
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_LIMIT_MSG_COUNT;
	std::stringstream strValue;
	strValue << nPageCount;
	theGetParam.strValue = strValue.str();
	vecUrlParam.push_back(theGetParam);
	/*theGetParam.strKey = key_G_UPDATE_KEY_PAG;
	theGetParam.strValue = "1";
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_FILEID_PAGESIZE;
	theGetParam.strValue = PAGE_SIZE;
	vecUrlParam.push_back(theGetParam);*/
	std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);
	//std::string strTestRequstURL = "http://192.168.1.213:8899/api/comm_message/list?sort_by=SPECIFY_LIST_AFTER&specify_msg_id=247648854500052992&receiver_extension=104%40192.168.1.213&sender_extension=120%40192.168.1.213";
	CCriticalAutoLock loLock(m_csRestAPI);
	std::wstring wstrTemplateURL = PortUtility::string2WString(strRequstTemplateURL);
	std::string strUTF8URL = PortUtility::Unicode2Utf8_ND(wstrTemplateURL);
#if _DEBUG
	TIMER_STOP(GetOffLineMsgPre);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" GetOffLineMsgPre  Cost %d ms.\n", TIMER_MSEC(GetOffLineMsgPre));
	OutputDebugString(strFormatInfo);

	TIMER_START(GetOffLineMsg);
#endif
	int nResult = Get(strUTF8URL, mapInParam, strResponse);
#if _DEBUG
	TIMER_STOP(GetOffLineMsg);
	//CIUIString strFormatInfo;
	strFormatInfo.Format(L" GetOffLineMsg  Cost %d ms.\n", TIMER_MSEC(GetOffLineMsg));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
	OutputDebugString(strFormatInfo);
#endif
 	if (nResult == 0)
	{
		Json::Reader json_reader(Json::Features::strictMode());
		Json::Value json_root;
#if _DEBUG
		TIMER_START(jsonRecordMsg);
#endif

		

		
		if (!json_reader.parse(strResponse, json_root))
		{
			LOG4_ERROR("GetOffLineMsg json parse error \n");
			LOG4_INFO(strResponse.c_str());
			return vecChatRecord;
		}
		Json::Value jsonRecordMsg = json_root["messages"];
		if (!jsonRecordMsg.isNull() && jsonRecordMsg.isArray())
		{
			for (int i = 0; i < jsonRecordMsg.size(); i++)
			{
				TagUnreadMsgInfo stuUnReadMsgInfo;
				stuUnReadMsgInfo.m_strBody = jsonRecordMsg[i]["msg_body"].asString();
				long long llMsgID = 0,llPostTime = 0;
				if (jsonRecordMsg[i]["id"].isString())
				{
					 PortUtility::stringToLL(jsonRecordMsg[i]["id"].asString(), llMsgID);
				}
				if (jsonRecordMsg[i]["id"].isInt64())
				{
					llMsgID = jsonRecordMsg[i]["id"].asInt64();
				}
				stuUnReadMsgInfo.m_llMsgID = llMsgID;
				stuUnReadMsgInfo.m_strMsgType = jsonRecordMsg[i]["msg_type"].asString();
				

				if (jsonRecordMsg[i]["post_time"].isString())
				{
					PortUtility::stringToLL(jsonRecordMsg[i]["post_time"].asString(), llPostTime);
				}
				if (jsonRecordMsg[i]["post_time"].isInt64())
				{
					llPostTime = jsonRecordMsg[i]["post_time"].asInt64();
				}
				stuUnReadMsgInfo.m_lPostTime = llPostTime;
				std::string strMsgSenderExternsionID = jsonRecordMsg[i]["sender_extension_id"].asString();
				
				stuUnReadMsgInfo.m_bIsMySelf = false;
				if (!strMsgSenderExternsionID.empty())
				{
					if (PortUtility::compareStringNoCase(strMsgSenderExternsionID,m_strID)==0)
					{
						stuUnReadMsgInfo.m_bIsMySelf = true;
					}
					else
					{
						stuUnReadMsgInfo.m_bIsMySelf = false;
					}
				}
		
				CChatRecord theOffLineMSG;
				ProcessOffLineMsg(stuUnReadMsgInfo, strSendExtensionURL, theOffLineMSG);
				vecChatRecord.push_back(theOffLineMSG);
			}
#if _DEBUG
			TIMER_STOP(jsonRecordMsg);
			CIUIString strFormatInfo;
			strFormatInfo.Format(L" jsonRecordMsg 1 Cost %d ms.\n", TIMER_MSEC(jsonRecordMsg));
			OutputDebugString(strFormatInfo);
#endif
		}
	}
	else
	{
		LOG4_INFO("HTTP REQUST voice_mail/show FAILED");
	}
	return vecChatRecord;
}
void CRestAPI::ListRecordVoiceMail( vector<VoiceMailParam> &vecVoiceMail)
{ 
	IsTimeOutRfresh();
	////recordfiles/voicemail/list
	int nPageStart = 1;
	std::string strResponse;
	std::string strRequstURL = REST_COMMON_HTTP+ m_strHttpURL + REST_HTTP_PORT + REST_API_URL_VOICEMAIL_LIST;
	std::stringstream ssValue;
	ssValue << nPageStart;
	
	map<string, string> mapInParam;
	mapInParam.insert(make_pair(key_G_TOKEN, m_strToken));
	//map<string, string> mapGetParam;
	//mapGetParam.insert(make_pair(key_G_FILEID_PAGESIZE, "15"));
	//mapGetParam.insert(make_pair(key_G_UPDATE_KEY_PAG, ssValue.str()));
	//mapGetParam.insert(make_pair(key_G_SORT, LIST_MODE_DEFAULT));
	vector<UrlParam> vecUrlParam;
	UrlParam theGetParam;
	theGetParam.strKey = key_G_FILEID_PAGESIZE;
	theGetParam.strValue = "15";
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_UPDATE_KEY_PAG;
	theGetParam.strValue = ssValue.str();
	vecUrlParam.push_back(theGetParam);
	theGetParam.strKey = key_G_SORT;
	theGetParam.strValue = LIST_MODE_DEFAULT;
	vecUrlParam.push_back(theGetParam);
	std::string strRequstTemplateURL = FormatURLByGetFun(strRequstURL, vecUrlParam);

	CCriticalAutoLock loLock(m_csRestAPI);
#if _DEBUG
	TIMER_START(ListRecordVoiceMail);
#endif
	int nResult = Get(strRequstTemplateURL, mapInParam, strResponse);
#if _DEBUG
	TIMER_STOP(ListRecordVoiceMail);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" ListRecordVoiceMail  Cost %d ms.\n", TIMER_MSEC(ListRecordVoiceMail));
	LOG4_INFO(PortUtility::Unicode2Utf8_ND(strFormatInfo.GetBuffer(strFormatInfo.GetLength())).c_str());
#endif
	if (nResult == 0)
	{
		Json::Reader json_reader(Json::Features::strictMode());
		Json::Value json_root;

		if (!json_reader.parse(strResponse, json_root))
		{
			LOG4_ERROR("json parse error \n");
			LOG4_INFO(strResponse.c_str());
			return;
		}
		Json::Value json_voice_mail_num = json_root["voicemails"];
		if (!json_voice_mail_num.isNull() && json_voice_mail_num.isArray())
		{
			for (int i = 0; i < json_voice_mail_num.size(); i++)
			{
				VoiceMailParam theVoiceMail;

				long long llVoiceMailID = 0;
				if (json_voice_mail_num[i]["id"].isString())
				{
					PortUtility::stringToLL(json_voice_mail_num[i]["id"].asString(), llVoiceMailID);
				}
				if (json_voice_mail_num[i]["id"].isInt64())
				{
					llVoiceMailID = json_voice_mail_num[i]["id"].asInt64();
				}

				theVoiceMail.m_lID = llVoiceMailID;
				theVoiceMail.m_strCaller =  json_voice_mail_num[i]["caller"].asString();
				theVoiceMail.m_strCalled = json_voice_mail_num[i]["callee"].asString();
				theVoiceMail.m_strTime = json_voice_mail_num[i]["time"].asString();
				theVoiceMail.m_strFileHttpURL = json_voice_mail_num[i]["path"].asString();
				theVoiceMail.m_strStatus = json_voice_mail_num[i]["status"].asString();
				vecVoiceMail.push_back(theVoiceMail);
			}
		}
	}
	else
	{
		LOG4_INFO("HTTP REQUST voice_mail/show FAILED");
	}
	return;
}

void CRestAPI::ProcessOffLineMsg(TagUnreadMsgInfo stuUnReadMsgInfo, std::string strSender, CChatRecord &oChatRecord)
{
	ENUM_MSG_TYPE enumMsgType = ENUM_MSG_OTHER;
	if (PortUtility::compareStringNoCase(stuUnReadMsgInfo.m_strMsgType, "text/plain")==0)
	{
		enumMsgType = ENUM_MSG_TEXT;
	}
	else if (PortUtility::compareStringNoCase(stuUnReadMsgInfo.m_strMsgType, "application/json")==0)
	{

		std::string strMsgType = "";
		Json::Reader json_reader(Json::Features::strictMode());
		Json::Value json_root;
		if (!json_reader.parse(stuUnReadMsgInfo.m_strBody, json_root))
		{
			LOG4_ERROR("json parse error \n");
				LOG4_INFO(stuUnReadMsgInfo.m_strBody.c_str());;
			return;
		}
		const Json::Value *val = json_root.find(key_G_MESSAGE_TYPE, key_G_MESSAGE_TYPE + strlen(key_G_MESSAGE_TYPE));

		if (val && val->isString())
		{
			strMsgType = val->asString();
			enumMsgType = PortUtility::GetMsgTypeByString(strMsgType);
		}
		else
		{
			enumMsgType = ENUM_MSG_TEXT;
		}

	}
	else
	{
		enumMsgType = ENUM_MSG_TEXT;
	}
	stringstream strSignMsgID;
	strSignMsgID<< stuUnReadMsgInfo.m_llMsgID;
	oChatRecord.m_strSignMsgID = strSignMsgID.str();
	oChatRecord.m_nIsMyself		= stuUnReadMsgInfo.m_bIsMySelf;
	oChatRecord.m_nMSGStatus	= ENUM_MSG_STATUS_UNREAD;
	oChatRecord.m_nMSGType		= enumMsgType;
	oChatRecord.m_strMSG		= stuUnReadMsgInfo.m_strBody;
	oChatRecord.m_strMSGTime	= PortUtility::UnixTimeToDateTime(stuUnReadMsgInfo.m_lPostTime);
	oChatRecord.m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	string strSend2SIP = strSender;

	if (PortUtility::compareStringNoCase(oChatRecord.m_strSignMsgID, "318207498521808896") == 0)
	{
		printf("");
	}
	//auto nPos = strSend2SIP.find("@");
	//strSend2SIP = strSend2SIP.substr(0, nPos);
	oChatRecord.m_strTalkerURL = strSend2SIP;
	//TODO
	//oChatRecord.m_strSenderURL = pContactDB->m_strSIPNum + "@" + CSystemEx::GetInstance()->GetGlobalUser()->m_strSipDomain;
}
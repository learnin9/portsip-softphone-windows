#include "stdafx.h"
#include "httpclient.h"
#include <string>
#include "curl/curl.h"
#include "System/SystemEx.h"
#include <iostream>
#include <fstream>
CHttpClient::CHttpClient(void) : 
m_bDebug(false)
{

}

CHttpClient::~CHttpClient(void)
{

}

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	char szInfo[256] = { 0 };
	std::string strOutInfo = "";
	strOutInfo.append(pData, pData+size);
	if(itype == CURLINFO_TEXT)
	{
		_snprintf(szInfo, sizeof(szInfo), "[TEXT]%s", strOutInfo.c_str());
		LOG4_INFO(szInfo);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		_snprintf(szInfo, sizeof(szInfo), "[HEADER_IN]%s", strOutInfo.c_str());
		LOG4_INFO(szInfo);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		_snprintf(szInfo, sizeof(szInfo), "[HEADER_OUT]%s", strOutInfo.c_str());
		LOG4_INFO(szInfo);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		_snprintf(szInfo, sizeof(szInfo), "[DATA_IN]%s", strOutInfo.c_str());
		LOG4_INFO(szInfo);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		_snprintf(szInfo, sizeof(szInfo), "[DATA_OUT]%s", strOutInfo.c_str());
		LOG4_INFO(szInfo);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}
int CHttpClient::Post(const std::string & strUrl, map<string, string> mapHeaderKeyValueParams, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_slist*  headers = NULL;
	
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);


	for (auto iter = mapHeaderKeyValueParams.begin(); iter != mapHeaderKeyValueParams.end(); iter++) {
		std::string strKey = iter->first.c_str();
		std::string strValue = iter->second.c_str();
		std::string strInParam = strKey + ": " + strValue;
		headers = curl_slist_append(headers, strInParam.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}
int CHttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);

	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

	
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}
int CHttpClient::Get(const std::string & strUrl, std::vector<std::string> vecOneHttpHeaderKeyValue, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

	struct curl_slist*  headers = NULL;
	std::vector<string>::iterator iterHttpHeaderKeyValue = vecOneHttpHeaderKeyValue.begin();
	for (iterHttpHeaderKeyValue; iterHttpHeaderKeyValue< vecOneHttpHeaderKeyValue.end();iterHttpHeaderKeyValue++)
	{
		std::string strOneHttpHeaderKeyValue = *iterHttpHeaderKeyValue;
		headers = curl_slist_append(headers, strOneHttpHeaderKeyValue.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Get(const std::string & strUrl, map<string, string> mapParams, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	struct curl_slist*  headers = NULL;
	for (auto iter = mapParams.begin(); iter != mapParams.end(); iter++) {
		std::string strKey = iter->first.c_str();
		std::string strValue = iter->second.c_str();
		std::string strInParam = strKey + ": " + strValue;
		headers = curl_slist_append(headers, strInParam.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int CHttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	if(m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CHttpClient::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
}

bool CHttpClient::ResponseHaveError(const char* szResponse)
{
	if (strlen(szResponse)==0)
	{
		return false;
	}
	Json::Reader json_reader(Json::Features::strictMode());
	Json::Value json_root;

	if (!json_reader.parse(szResponse, json_root))
	{
		return false;
	}
	Json::Value jsonError = json_root["error"];
	if (jsonError.isString())
	{
		std::string strError = jsonError.asString();
		LOG4_ERROR(strError.c_str());
		return true;
	}
	return false;
}
bool CHttpClient::HTTPDownLoadFile(const std::string strFileHttpURL, const std::string strFileSavePath, std::string strFileName,UpLoadDownLoadParam* pDownloadParam)
{
	OutputDebugString(L"HTTPDownLoadFile BEGIN");
	std::string strFileTemplatePath = strFileSavePath + strFileName;
	bool nRet = false;
	FILE *outfile = fopen(strFileTemplatePath.c_str(), "wb");

	if (outfile==NULL)
	{
		std::string strError = "file is null where download";
		LOG4_INFO(strError.c_str());
		return false;
	}

	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::string strErrorInfo = "curl env  Error";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	if (outfile==NULL)
	{
		std::string strErrorInfo = "Download File Path Error";
		LOG4_INFO(strErrorInfo.c_str());
	}
	std::string strResponse;
	curl_easy_setopt(curl, CURLOPT_URL, strFileHttpURL.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CHttpClient::CallBackWriteFun);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CHttpClient::CallBackProcessDownLoadFun);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, pDownloadParam);
	int res = curl_easy_perform(curl);
	fclose(outfile);
	char szResponse[MAX_PATH] = {0};
	/* always cleanup */
	curl_easy_cleanup(curl);
	ifstream inFile(strFileTemplatePath.c_str(), ios::in | ios::binary);
	if (inFile)
	{
		inFile.getline(szResponse, MAX_PATH);
	}
	bool bRet = ResponseHaveError(szResponse);
	if (bRet==true)
	{
		pDownloadParam->m_enumFileStatus = ENUM_FAILED_END;
		PortUtility::removeFile(strFileTemplatePath);
		bRet = false;
	}
	if (res!=0)
	{
		std::string strErrorInfo = "something error by curl download File";
		LOG4_INFO(strErrorInfo.c_str());
		pDownloadParam->m_enumFileStatus = ENUM_FAILED_END;

		PortUtility::removeFile(strFileTemplatePath);
		bRet =  false;
	}
	HWND hMsgProcess = pDownloadParam->m_hWndMsgProcess;
	if (hMsgProcess != NULL)
	{
		::PostMessage(hMsgProcess, WM_DOWNLOAD_FILE_OK, (WPARAM)pDownloadParam, 0);
	}
	
	OutputDebugString(L"HTTPDownLoadFile END");
	return bRet;
}
bool CHttpClient::HTTPDownLoadVoiceMail(const std::string strFileHttpURL, const std::string strFileSavePath, std::string strFileName, VoiceMailDownLoadParam cbMailDownLoadParam)
{
	std::string strFileTemplatePath = strFileSavePath + strFileName;
	bool nRet = false;
	FILE *outfile = fopen(strFileTemplatePath.c_str(), "wb");

	if (outfile == NULL)
	{
		std::string strError = "file is null where download";
		LOG4_INFO(strError.c_str());
		return false;
	}

	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		std::string strErrorInfo = "curl env  Error";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	if (outfile == NULL)
	{
		std::string strErrorInfo = "Download File Path Error";
		LOG4_INFO(strErrorInfo.c_str());
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, strFileHttpURL.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CHttpClient::CallBackWriteFun);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, TRUE);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CHttpClient::CallBackProcessDownLoadVoiceMailFun);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &cbMailDownLoadParam);

	int res = curl_easy_perform(curl);

	fclose(outfile);
	/* always cleanup */
	curl_easy_cleanup(curl);

	if (res != 0)
	{
		std::string strErrorInfo = "something error by curl download VoiceMail";
		LOG4_INFO(strErrorInfo.c_str());
		return false;
	}
	
	return true;
}
size_t CHttpClient::CallBackWriteFun(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}
size_t CHttpClient::CallBackFreadFun(void* buffer, int size, size_t nmemb, void* userp) {
	std::string * str = dynamic_cast<std::string *>((std::string *)userp);
	str->append((char *)buffer, size * nmemb);
	return nmemb;
}
int CHttpClient::CallBackProcessDownLoadFun (void *ptrParam,
	double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow)
{
	if (ptrParam==NULL)
	{
		return 0;
	}
	if (t > 0 - 0.0000001 && t < 0 + 0.000001)
	{
		return 0;
	}
	UpLoadDownLoadParam  *pThisDownLoadParam = (UpLoadDownLoadParam*)ptrParam;
	if (t > d - 0.0000001 && t < d + 0.000001)
	{
		pThisDownLoadParam->m_enumFileStatus = ENUM_PROCESSING;
	} 
	else if (pThisDownLoadParam->m_enumFileStatus != ENUM_PROCESSING)
	{
		return 0;
	} 

	long nMsgid = pThisDownLoadParam->m_lMsgID;
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pThisDownLoadParam->m_pContact->m_enumRecentContact;
	if (localRecentContactType == ENUM_SINGLE_CONTACT)
	{
		std::string strSipNumber = (pThisDownLoadParam->m_pContact->m_unionContactInfo.m_pContact)->m_strSIPNum;

		double nDownLoadPer = d * 100.0 / t;
		pThisDownLoadParam->m_fProcessData = nDownLoadPer;

		if (pThisDownLoadParam->m_enumFileStatus != ENUM_OK)
		{
			//download end call the dest modlue process this result
			HWND hMsgProcess = pThisDownLoadParam->m_hWndMsgProcess;
			if (hMsgProcess != NULL)
			{
				::PostMessage(hMsgProcess, WM_DOWNLOAD_FILE, (WPARAM)pThisDownLoadParam, 0);
			}
			//::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_DOWNLOAD_FILE, (WPARAM)pThisDownLoadParam, 0);
		}

		if (nDownLoadPer > 100 - 0.0000001 && nDownLoadPer < 100 + 0.000001)
		{
			pThisDownLoadParam->m_enumFileStatus = ENUM_OK;
		}

		std::stringstream strDebugInfo;
		strDebugInfo << "CallBackProcessDownLoadFun now is: " << nDownLoadPer << "\n";
		OutputDebugString(PortUtility::string2WString(strDebugInfo.str()).c_str());
	}
	return 0;
}
int CHttpClient::CallBackProcessUpLoadFun(void* ptrParam, double t, double d, double ultotal, double ulnow)
{
	if (ptrParam == NULL)
		return 0;
	if (ultotal > 0 - 0.0000001 && ultotal < 0 + 0.000001)
	{
		return 0;
	}
	UpLoadDownLoadParam  *pThisUpLoadParam = (UpLoadDownLoadParam*)ptrParam;
	double nDownLoadPer = ulnow * 100.0 / ultotal;
	pThisUpLoadParam->m_fProcessData = nDownLoadPer;
	if (pThisUpLoadParam->m_enumFileStatus != ENUM_OK)
	{
		//download end call the dest modlue process this result
		HWND hMsgProcess = pThisUpLoadParam->m_hWndMsgProcess;
		if (hMsgProcess != NULL)
		{
			::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_UPLOAD_FILE, (WPARAM)pThisUpLoadParam, 0);
		}
	}
	if (nDownLoadPer > 100 - 0.0000001 && nDownLoadPer < 100 + 0.000001)
	{
		pThisUpLoadParam->m_enumFileStatus = ENUM_OK;
	}
	std::stringstream strDebugInfo;
	strDebugInfo << "CallBackProcessUpLoadFun now is: " << nDownLoadPer<<"\n";
	OutputDebugString(PortUtility::string2WString(strDebugInfo.str()).c_str());
	return 0;
}

int CHttpClient::CallBackProcessDownLoadVoiceMailFun(void* ptrParam, double t, double d, double ultotal, double ulnow)
{
	if (ptrParam == NULL)
		return 1;
	if (t > 0 - 0.000000001 && t < 0 + 0.00000001)
	{
		return 0;
	}
	VoiceMailDownLoadParam  *pThisVoiceParam = (VoiceMailDownLoadParam*)ptrParam;
	double nDownLoadPer = d * 100.0 / t;
	pThisVoiceParam->m_fProcessData = nDownLoadPer;

	std::stringstream strDebugInfo;
	strDebugInfo << "CallBackProcessDownLoadVoiceMailFun now is: " << nDownLoadPer << "\n";
	OutputDebugString(PortUtility::string2WString(strDebugInfo.str()).c_str());
	return 0;
}

bool CHttpClient::UploadFile(const std::string strFileHttpURL, const std::string strFilePath, std::string strFileName, map<string, string> mapParams, UpLoadDownLoadParam *pUploadParam,std::string & strResponse)
{
	CURL *curl;
	CURLcode ret;
	curl = curl_easy_init();
	struct curl_httppost* post = NULL;
	struct curl_httppost* last = NULL;
	struct curl_slist *headerlist = NULL;
	std::string strFileTemplatePath = strFilePath + strFileName;
	if (!strFilePath.empty()&&!strFileName.empty()) {
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "file",
			CURLFORM_FILE,/* "./test.jpg"*/strFileTemplatePath.c_str(),
			CURLFORM_FILENAME,/* "hello.jpg"*/strFileName.c_str(),
			CURLFORM_END);// form-data key(file) "./test.jpg"is file path
	}
	for (auto iter = mapParams.begin(); iter != mapParams.end(); iter++) {
		std::string strKey = iter->first.c_str();
		std::string strValue = iter->second.c_str();
		std::string strInParam = strKey + ": " + strValue;
		headerlist = curl_slist_append(headerlist, strInParam.c_str());
	}
	bool bRet = true;
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_URL, (char *)strFileHttpURL.c_str());
		//curl_easy_setopt(curl, CURLOPT_POST, 1);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostParam.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CHttpClient::CallBackFreadFun);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);       
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &CHttpClient::CallBackProcessUpLoadFun);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, pUploadParam);
		ret = curl_easy_perform(curl);
		if (ret == 0) {
		}
		else {
			pUploadParam->m_enumFileStatus = ENUM_FAILED_PRE;
		}
		curl_easy_cleanup(curl);
		HWND hMsgProcess = pUploadParam->m_hWndMsgProcess;
		if (hMsgProcess != NULL)
		{
			//::SendMessage(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd(), WM_UPLOAD_FILE_PROCESS_OK, (WPARAM)pUploadParam, 0);
		}
		
		return 0;
	}
	else
	{
		return false;
	}
}

std::string CHttpClient::FormatURLByGetFun(std::string strURL, vector<UrlParam> vecUrlParam)
{
	std::string strTemplateURL = "";
	if (strURL.empty())
	{
		LOG4_INFO("http url is empty ");
		return "";
	}
	if (vecUrlParam.size()==0)
	{
		return strURL;
	}
	strTemplateURL = strURL + "?";
	bool bIsFirst = false;
	for (auto iter = vecUrlParam.begin(); iter != vecUrlParam.end(); iter++) {
		std::string strKey = iter->strKey.c_str();
		std::string strValue = iter->strValue.c_str();
		std::string strInParam = "";
		if (bIsFirst==false)
		{
			strInParam = strKey + "=" + strValue;
			bIsFirst = true;
		}
		else
		{
			strInParam ="&" +strKey + "=" + strValue;
		}
		strTemplateURL += strInParam;
	}
	return strTemplateURL;
}
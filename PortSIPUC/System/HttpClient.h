#ifndef __HTTP_CURL_H__
#define __HTTP_CURL_H__

#include <string>
struct UrlParam
{
	std::string strKey;
	std::string strValue;
};
class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

public:
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);
	int Post(const std::string & strUrl, map<string, string> mapHeaderKeyValueParams, const std::string & strPost, std::string & strResponse);
	int Get(const std::string & strUrl, std::string & strResponse);
	int Get(const std::string & strUrl, std::vector<std::string> vecOneHttpHeaderKeyValue, std::string & strResponse);
	int Get(const std::string & strUrl, map<string, string> mapParams, std::string & strResponse);
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);
	std::string FormatURLByGetFun(std::string strURL, vector<UrlParam> vecUrlParam);
	bool HTTPDownLoadFile(const std::string strFileHttpURL, const std::string strFileSavePath,std::string strFileName, UpLoadDownLoadParam* pDownloadParam);
	bool HTTPDownLoadVoiceMail(const std::string strFileHttpURL, const std::string strFileSavePath, std::string strFileName, VoiceMailDownLoadParam cbMailDownLoadParam);
	static size_t CallBackWriteFun(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static size_t CallBackFreadFun(void* buffer, int size, size_t nmemb, void* userp);
	static int CallBackProcessDownLoadFun(void *ptr,
		double t, /* dltotal */
		double d, /* dlnow */
		double ultotal,
		double ulnow);
	static int CallBackProcessDownLoadVoiceMailFun(void *ptr,
		double t, /* dltotal */
		double d, /* dlnow */
		double ultotal,
		double ulnow);
	static int CallBackProcessUpLoadFun(void* ptr,
		double t, /* dltotal */
		double d, /* dlnow */
		double ultotal,
		double ulnow);
	bool UploadFile(const std::string strFileHttpURL, const std::string strFilePath, std::string strFileName, map<string, string> mapParams, UpLoadDownLoadParam* pUploadParam, std::string & strResponse);
public:
	void SetDebug(bool bDebug);

	bool ResponseHaveError(const char *szResponse);

private:
	bool m_bDebug;
};

#endif

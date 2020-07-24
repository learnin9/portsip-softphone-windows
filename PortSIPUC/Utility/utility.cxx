
#include "stdafx.h"
#include "utility.hxx"
#include <io.h>
#include <direct.h>
#include <iomanip>
#include <regex>
using namespace std;
const char *g_LanguagesCode[] = { "en","zh_ch","zh_ch","it-IT","es","fr-fr","ru-RU","pt-PT" };
const bool isCharHex[256] =
{
	// 0       1       2       3       4       5       6       7       8       9       a   b   c   d   e   f
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //0
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //1
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //2
	true,   true,   true,   true,   true,   true,   true,   true,   true,   true,  false,  false,  false,  false,  false,  false,  //3
	false,   true,   true,   true,   true,   true,   true,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //4
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //5
	false,   true,   true,   true,   true,   true,   true,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //6
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //8
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //9
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //a
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //b
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //c
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //d
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  //e
	false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false   //f
};
static const char inversehexmap[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,     // 0 - 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,             //   - 47

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,         // 48 ...

	-1, -1,    // 58 -
	-1, -1, -1, -1, -1,                         //  -64

	10, 11, 12, 13, 14, 15,               // 65 ...

	-1, -1, -1, -1, -1, -1, -1, -1, -1,     // 71 ...
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1,                 //   - 96

	10, 11, 12, 13, 14, 15                // 97 ...
};


bool isHex(unsigned char c)
{
	return isCharHex[c];
}

PortUtility::PortUtility()
{
	
}


PortUtility::~PortUtility()
{

}
void PortUtility::deleteAllMark(string &s, const string &mark)
{
	size_t nSize = mark.size();
	while (1)
	{
		size_t pos = s.find(mark); 
		if (pos == string::npos)
		{
			return;
		}

		s.erase(pos, nSize);
	}
}
int PortUtility::compareStringNoCase(const string & s1, const string & s2)
{
	
	string::const_iterator iter1 = s1.begin();
	string::const_iterator iter2 = s2.begin();


	while ((iter1!=s1.end()) && (iter2!=s2.end()))
	{
		if (toupper(*iter1) != toupper(*iter2))
		{
			return (toupper(*iter1)<toupper(*iter2)) ? -1 : 1;
		}
		++iter1;
		++iter2;
	}

	return (s2.size() == s1.size()) ? 0 : (s1.size() < s2.size()) ? -1 : 1;	
}

bool PortUtility::removeFile(const std::string strFilePath)
{
	if (strFilePath.empty()==true)
	{
		return false;
	}
	if (remove(strFilePath.c_str()) == 0)
	{
		return true;
	}
	return false;
}
bool PortUtility::getLocalIP(string & ip)
{
	/*
	if (initWinsock() == false)
	{
		return false;
	}
	*/
	/*
	char szLocalIP[20] ={0};
	char szHostName[128 + 1] = "\0";
	hostent * phe;
	int i;
	if (gethostname(szHostName, 128) == 0)
	{
		phe = gethostbyname(szHostName);
		for (i = 0; phe != NULL && phe->h_addr_list[i] != NULL; i++)
		{
			sprintf(szLocalIP, "%d.%d.%d.%d", (UINT) ((UCHAR *) phe->h_addr_list[i])[0],
				(UINT) ((UCHAR *) phe->h_addr_list[i])[1],
				(UINT) ((UCHAR *) phe->h_addr_list[i])[2],
				(UINT) ((UCHAR *) phe->h_addr_list[i])[3]);
		}
	}

	ip = szLocalIP;
	*/
	return true;
}

bool PortUtility::initWinsock()
{
	//TO DO WSAStartup
	WSADATA       wsd;
	INT			  nRet;

	nRet = WSAStartup(MAKEWORD(2,2), &wsd);
	if (nRet != 0)
	{
		return false;
	}
	
	return true;
}


bool PortUtility::initializeWinsock()
{
	WSADATA       wsd;
	INT			  nRet;

	nRet = WSAStartup(MAKEWORD(2,2), &wsd);
	if (nRet != 0)
	{
		return false;
	}

	return true;
}
char* PortUtility::wchar2Char(const wchar_t *wstr)
{
	if (!wstr) return 0;

	int dCharacters = WideCharToMultiByte(CP_ACP, 0, wstr, wcslen(wstr), 0, 0, 0, 0);
	char *str = new char[dCharacters + 1];
	WideCharToMultiByte(CP_ACP,0, wstr, wcslen(wstr), str, dCharacters, 0, 0);
	str[dCharacters] = '\0';
	return str;
};
void PortUtility::freeChar(char* str)
{
	if (str)
	{
		delete[]str;
	}
}
wchar_t* PortUtility::char2Wchar(const char *str)
{
	if (!str) return 0;
	int dCharacters = MultiByteToWideChar( CP_ACP, 0, str,strlen(str), NULL,0);
	wchar_t *wszStr = new wchar_t[dCharacters+1];          
	MultiByteToWideChar( CP_ACP, 0, str,strlen(str), wszStr,dCharacters);
	wszStr[dCharacters] = '\0';
	return wszStr;
};
void PortUtility::freeWchar(wchar_t* str)
{
	if (str)
	{
		delete[]str;
	}
}
std::wstring PortUtility::string2WString(const std::string& s)
{
	wchar_t* buf = char2Wchar(s.c_str());
	std::wstring r(buf);
	freeWchar(buf);
	return r;
}
std::string PortUtility::wstring2String(const std::wstring &s)
{
	char* buf = wchar2Char(s.c_str());
	std::string r(buf);
	freeChar(buf);
	return r;
}





CString PortUtility::ansi2Unicode(const char * ansi)
{
	WCHAR temp[1024 * 4] = { 0 };

	MultiByteToWideChar(CP_ACP,
		0,
		ansi,
		strlen(ansi) + 1,
		temp,
		sizeof(temp) / sizeof(temp[0]));

	return (CString)temp;
}
std::wstring PortUtility::Utf82Unicode_ND(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
	if (convresult != widesize)
	{
		throw std::exception("Error in conversion.");
	}
	return std::wstring(&resultstring[0]);
}
string PortUtility::WideByte2Acsi_ND(wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);
	if (convresult != asciisize)
	{
		throw std::exception("Error in conversion.");
	}
	return std::string(&resultstring[0]);
}
string PortUtility::UTF_82ASCII_ND(string& strUtf8Code)
{
	string strRet("");
	wstring wstr = Utf82Unicode_ND(strUtf8Code);
	strRet = WideByte2Acsi_ND(wstr);
	return strRet;
}
/////////////////////////////////////////////////////////////////////// 
wstring PortUtility::Acsi2WideByte_ND(string& strascii)
{
	int widesize = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);
	if (convresult != widesize)
	{
		throw std::exception("Error in conversion.");
	}
	return std::wstring(&resultstring[0]);
}
std::string PortUtility::Unicode2Utf8_ND(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8size == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(utf8size);
	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);
	if (convresult != utf8size)
	{
		throw std::exception("Error in conversion.");
	}
	return std::string(&resultstring[0]);
}
string PortUtility::ASCII2UTF_8_ND(string& strAsciiCode)
{
	string strRet("");
	wstring wstr = Acsi2WideByte_ND(strAsciiCode);
	strRet = Unicode2Utf8_ND(wstr);
	return strRet;
}

void PortUtility::UnicodeToUtf8(CString strUnicode, string strUTF8)
{
	int len;
	LPTSTR lpStrUnicode = strUnicode.GetBuffer(strUnicode.GetLength());
	len = WideCharToMultiByte(CP_UTF8, 0, lpStrUnicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, lpStrUnicode, -1, szUtf8, len, NULL, NULL);
	strUTF8 = szUtf8;

}
int PortUtility::transVideoBitrate(int nBitrateType)
{
	int nBitRate = 128;
	switch (nBitrateType)
	{
	case 0:
		nBitRate= 128;
		break;
	case 1:
		nBitRate =  256;
		break;
	case 2:
		nBitRate = 512;
		break;
	case 3:
		nBitRate = 1024;
		break;
	case 4:
		nBitRate = 1200;
		break;
	case 5:
		nBitRate = 1500;
		break;
	case 6:
		nBitRate = 2000;
		break;
	}
	return nBitRate;
}

TagScreenMetrics PortUtility::transVideoResolution(int nVideoResolution)
{
	int width = 352;
	int height = 288;
	switch (nVideoResolution)
	{
	case 0:
		width = 176;
		height = 144;
		break;
	case 1:
		width = 352;
		height = 288;
		break;
	case 2:
		width = 640;
		height = 480;
		break;
	case 3:
		width = 800;
		height = 600;
		break;
	case 4:
		width = 1024;
		height = 768;
		break;
	case 5:
		width = 1280;
		height = 720;
		break;
	}

	TagScreenMetrics theLocalScreenMetrics;
	theLocalScreenMetrics.nFrameWidth = width;
	theLocalScreenMetrics.nFrameHeight = height;

	return theLocalScreenMetrics;
}

ENUM_MSG_TYPE PortUtility::GetMsgTypeByString(std::string strMsgType)
{
	ENUM_MSG_TYPE enumMsgType = ENUM_MSG_TEXT;
	if (compareStringNoCase(MESSAGE_TYPE_TEXT,strMsgType)==0)
	{
		return ENUM_MSG_TEXT;
	}
	if (compareStringNoCase(MESSAGE_TYPE_AUDIO, strMsgType) == 0)
	{
		return ENUM_MSG_FILE_AUDIO;
	}
	if (compareStringNoCase(MESSAGE_TYPE_VIDEO, strMsgType) == 0)
	{
		return ENUM_MSG_FILE_VIDEO;
	}
	if (compareStringNoCase(MESSAGE_TYPE_IMAGE, strMsgType) == 0)
	{
		return ENUM_MSG_FILE_PIC;
	}
	if (compareStringNoCase(MESSAGE_TYPE_FILE, strMsgType) == 0)
	{
		return ENUM_MSG_FILE;
	}
	if (compareStringNoCase(MESSAGE_TYPE_EMOJI, strMsgType) == 0)
	{
		return ENUM_MSG_TEXT;
	}
	if (compareStringNoCase(MESSAGE_TYPE_SHAKE, strMsgType) == 0)
	{
		return ENUM_MSG_SHAKE;
	}
	return ENUM_MSG_OTHER;
}
VIDEOCODEC_TYPE PortUtility::getVideoCodecByName(const string & codecName)
{
	VIDEOCODEC_TYPE codec = (VIDEOCODEC_TYPE)-1;

	if (compareStringNoCase("H.263", codecName) == 0)
	{
		return VIDEO_CODEC_H263;
	}


	if (compareStringNoCase("H.263+(1998)", codecName) == 0)
	{
		return VIDEO_CODEC_H263_1998;
	}


	if (compareStringNoCase("H.264", codecName) == 0|| compareStringNoCase("H264", codecName) == 0)
	{
		return VIDEO_CODEC_H264;
	}
	if (compareStringNoCase("VP9",codecName)==0)
	{
		return VIDEO_CODEC_VP9;
	}
	if (compareStringNoCase("VP8",codecName)==0)
	{
		return VIDEO_CODEC_VP8;
	}
	return codec;
}

AUDIOCODEC_TYPE PortUtility::getAudioCodecByName(const string & codecName)
{
	AUDIOCODEC_TYPE codec = AUDIOCODEC_TYPE(-1);

	PortUtility utility;
	if (utility.compareStringNoCase("Opus",codecName)==0)
	{
		return AUDIOCODEC_OPUS;
	}
	if (utility.compareStringNoCase("G.729", codecName) == 0)
	{
		return AUDIOCODEC_G729;
	}
	if (utility.compareStringNoCase("PCMU", codecName) == 0)
	{
		return AUDIOCODEC_PCMU;
	}

	if (utility.compareStringNoCase("PCMA", codecName) == 0)
	{
		return AUDIOCODEC_PCMA;
	}


	if (utility.compareStringNoCase("GSM", codecName) == 0)
	{
		return AUDIOCODEC_GSM;
	}

	if (utility.compareStringNoCase("iLBC", codecName) == 0)
	{
		return AUDIOCODEC_ILBC;
	}

	if (utility.compareStringNoCase("G.722", codecName) == 0)
	{
		return AUDIOCODEC_G722;
	}

	if (utility.compareStringNoCase("G.722.1", codecName) == 0)
	{
#ifdef FULL_VERSION
		return AUDIOCODEC_G7221;
#else
		return AUDIOCODEC_TYPE(-2);
#endif
	}

	if (utility.compareStringNoCase("SPEEX", codecName) == 0)
	{
		return AUDIOCODEC_SPEEX;
	}

	if (utility.compareStringNoCase("SPEEX-WB", codecName) == 0)
	{
		return AUDIOCODEC_SPEEXWB;
	}

	if (utility.compareStringNoCase("AMR-WB", codecName) == 0)
	{
#ifdef FULL_VERSION
		return AUDIOCODEC_AMRWB;
#else
		return AUDIOCODEC_TYPE(-3);
#endif
	}

	if (utility.compareStringNoCase("AMR", codecName) == 0)
	{
#ifdef FULL_VERSION
		return AUDIOCODEC_AMR;
#else
		return AUDIOCODEC_TYPE(-4);
#endif
	}

	if (utility.compareStringNoCase("ISACWB", codecName) == 0)
	{
#ifdef FULL_VERSION
		return AUDIOCODEC_ISACWB;
#else
		return AUDIOCODEC_TYPE(-5);
#endif
	}
	if (utility.compareStringNoCase("ISACSWB", codecName) == 0)
	{
#ifdef FULL_VERSION
		return AUDIOCODEC_ISACSWB;
#else
		return AUDIOCODEC_TYPE(-6);
#endif
	}
	return codec;
}

void PortUtility::supersplit(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.emplace_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != len)
		v.emplace_back(s.substr(pos1));
}
bool PortUtility::isPortAvailable(int nPort)
{ 
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  
	sockAddr.sin_family = PF_INET;  
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);;  
	sockAddr.sin_port = htons(nPort); 
	int nReslut = bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	bool isAvailable = false;
	if (nReslut== SOCKET_ERROR)
	{
		isAvailable = false;
	}
	else 
	{
		nReslut = listen(servSock, 10);
		if (nReslut == SOCKET_ERROR)
		{
			isAvailable = false;
		}
		else
		{
			isAvailable = true;
		}
		
	}
	
	closesocket(servSock);
	WSACleanup();
	return isAvailable;
}

int PortUtility::findStringLike(std::string strSrc, std::string strKeyString)
{
	return ci_find_substr(strSrc, strKeyString);
}
std::string PortUtility::UnixTimeToDateTime(time_t time)
{
	std::stringstream ssTime;
	tm *pTmp = localtime(&time);
	if (pTmp == NULL)
	{
		return FALSE;
	}
	ssTime << std::setfill('0') << std::setw(4) <<pTmp->tm_year + 1900 << "-"  <<std::setfill('0') << std::setw(2) << pTmp->tm_mon + 1 << "-" << std::setfill('0') << std::setw(2) << pTmp->tm_mday  << " " << std::setfill('0') << std::setw(2) << pTmp->tm_hour << ":" << std::setfill('0') << std::setw(2) << pTmp->tm_min << ":" << std::setfill('0') << std::setw(2) << pTmp->tm_sec;
	return ssTime.str();
}
void PortUtility::FormatDateTimeGMT(std::string strDateTime, std::string &strOutString, ENUM_LANGUAGE enumLanguage)
{
	std::string strAMPM = "";
	if (strDateTime.empty())
	{
		strOutString = "";
		return;
	}
	Calendar *cal;
	DateFormat *fmt;
	SimpleDateFormat *simplefmt;
	UErrorCode status = U_ZERO_ERROR;//initial ErrorCode to 0
	UnicodeString result;
	UDate date;
	string::size_type position = strDateTime.find(":");
	if (position != 0)
	{
		int nHour = atoi(strDateTime.substr(position - 2, position).c_str());
		if (nHour >= 12)
		{
			strAMPM = "AM";
		}
		else
		{
			strAMPM = "PM";
		}
	}
	std::string  strOri = strDateTime;// +strAMPM;
	UnicodeString sourceTime = strOri.c_str();
	char* engTimeFormat = "yyyy-MMMM-dd HH:mm:ss zzz";
	//char* engTimeFormat = "MMMM dd,yyyy hh:mm aa";
	simplefmt = new SimpleDateFormat(UnicodeString(engTimeFormat), Locale::getEnglish(), status);
	//TimeZone *londonTimezone = TimeZone::createTimeZone("Europe/London");// TimeZone::TimeZone::createDefault();//;
	//simplefmt->setTimeZone(*londonTimezone);
	date = simplefmt->parse(sourceTime, status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("simplefmt::parse error");
		return;
	}
	cal = Calendar::createInstance(status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("Calendar::createInstance error");
		return;
	}
	cal->clear();
	cal->setTime(date, status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("Calendar::setTime error");
		return;
	}

	fmt = DateFormat::createDateTimeInstance(
		DateFormat::kFull, DateFormat::kShort, Locale(g_LanguagesCode[enumLanguage]));
	UnicodeString strTimeZoneIn;
	cal->adoptTimeZone(TimeZone::TimeZone::createDefault());
	fmt->setCalendar(*cal);
	fmt->format(date, result, status);

	char *buf = 0;
	int32_t len = result.length();
	int32_t bufLen = len + 16;
	int32_t actualLen;
	buf = new char[bufLen + 1];//buffer
	actualLen = result.extract(0, len, buf); // Default codepage conversion
	buf[actualLen] = 0;
	strOutString = buf;

	delete buf;
	delete fmt;
}
void PortUtility::FormatDateTime(std::string strDateTime, std::string &strOutString,ENUM_LANGUAGE enumlanguage) {
	std::string strAMPM = "";
	if (strDateTime.empty())
	{
		strOutString = "";
		return;
	}
	Calendar *cal;
	DateFormat *fmt;
	SimpleDateFormat *simplefmt;
	UErrorCode status = U_ZERO_ERROR;//initial ErrorCode to 0
	UnicodeString result;
	UDate date;
	string::size_type position = strDateTime.find(":");
	if (position!=0)
	{
		int nHour = atoi(strDateTime.substr(position - 2, position).c_str());
		if (nHour>=12)
		{
			strAMPM = "AM";
			//strAMPM = "PM";
		}
		else
		{
			strAMPM = "PM";
		}
	}
	std::string  strOri = strDateTime;// +strAMPM;
	UnicodeString sourceTime = strOri.c_str();
	char* engTimeFormat = "yyyy-MMMM-dd HH:mm:ss";
	//char* engTimeFormat = "MMMM dd,yyyy hh:mm aa";
	simplefmt = new SimpleDateFormat(UnicodeString(engTimeFormat), Locale::getEnglish(), status);
	TimeZone *londonTimezone = TimeZone::createTimeZone("Europe/London");// TimeZone::TimeZone::createDefault();//;
	simplefmt->setTimeZone(*londonTimezone);
	date = simplefmt->parse(sourceTime, status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("simplefmt::parse error");
		return;
	}
	cal = Calendar::createInstance(status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("Calendar::createInstance error");
		return;
	}
	cal->clear();
	cal->setTime(date, status);
	if (U_FAILURE(status))
	{
		LOG4_INFO("Calendar::setTime error");
		return;
	}
	
	fmt = DateFormat::createDateTimeInstance(
		DateFormat::kFull, DateFormat::kShort, Locale(g_LanguagesCode[enumlanguage]));
	UnicodeString strTimeZoneIn;
	cal->adoptTimeZone(TimeZone::TimeZone::createTimeZone("Europe/London"));
	fmt->setCalendar(*cal);
	fmt->format(date, result, status);

	char *buf = 0;
	int32_t len = result.length();
	int32_t bufLen = len + 16;
	int32_t actualLen;
	buf = new char[bufLen + 1];//buffer
	actualLen = result.extract(0, len, buf); // Default codepage conversion
	buf[actualLen] = 0;
	strOutString = buf;

	delete buf;
	delete fmt;
}
std::string PortUtility::GetSipNumberBySenderFormatNoSip(std::string strSender)
{
	size_t nPos = strSender.find("@");
	if (nPos>=0)
	{
		return strSender.substr(0,nPos);
	}
	return strSender;
}
std::string PortUtility::GetSipNumberBySenderFormat(std::string strSender)
{
	size_t nStartPos = strSender.find(":");
	size_t nEndPos = strSender.find("@");
	if (nStartPos>=0&&nEndPos>=0&&nStartPos<nEndPos)
	{
		return strSender.substr(nStartPos+1, nEndPos-nStartPos-1);
	}
	return "";
}
void PortUtility::getCurrentlyPath(string & path, bool withSplit)
{
#ifndef UNDER_CE
	char buff[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buff, MAX_PATH);
	PathRemoveFileSpecA(buff);

	path = buff;

	if (withSplit == true)
	{
		return;
	}

	path += "\\";
#else
	WCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buff, MAX_PATH);
	WCHAR* prt=wcsrchr(buff,TEXT('\\'));
	*prt = L'\0';

	char* pPath = FromWStringL(buff);
	if(pPath)
	{
		path = pPath;
		delete[] pPath;
	}

	if (withSplit == true)
	{
		return;
	}

	path += "\\";
#endif//UNDER_CE
	return;
}

bool PortUtility::fromHex(const string & str, char * outBuf, int & outBufSize)
{
	size_t mSize = str.length();
	bool oddSize = mSize & 1;
	size_t resultSize = (mSize + (mSize & 1)) / 2;
	if (outBufSize < resultSize)
	{
		return false;
	}
	outBufSize = resultSize;

	char * mBuf = new char[mSize * 3];
	memset(mBuf, 0, mSize * 3);
	memcpy(mBuf, str.data(), mSize);

	const unsigned char* p = (unsigned char*)mBuf;
	char* r = outBuf;
	size_t i = 0;
	if (oddSize)
	{
		if (!isHex(*p))
		{
			return false;
		}
		else
		{
			*r++ = inversehexmap[*p++];
		}

		i++;
	}
	for (; i < mSize; i += 2)
	{
		const unsigned char high = *p++;
		const unsigned char low = *p++;
		if (!isHex(high) || !isHex(low))
		{
			return false;
		}
		else
		{
			*r++ = (inversehexmap[high] << 4) + inversehexmap[low];
		}
	}

	delete[] mBuf;

	return true;
}

string PortUtility::getSelfFullFilePathName()
{
	string name = "";
#ifndef UNDER_CE
	char buff[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buff, MAX_PATH);

	name = buff;
#else
	WCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buff, MAX_PATH);
	char* pName = FromWStringL(buff);
	if(pName)
	{
		name = pName;
		delete[] pName;
	}

#endif
	return name;
}

string PortUtility::removePath(const char * filePathName)
{
	if (!filePathName)
	{
		return "";
	}

	const char* tempFile = filePathName + strlen(filePathName);
	while (tempFile != filePathName &&
		*tempFile != '\\')
	{
		--tempFile;
	}
	if (tempFile != filePathName)
	{
		++tempFile;
	}

	return tempFile;
}


/*bool PortUtility::resolveIp(const string & domain, string & ip)
{
	if (domain.empty() == true)
	{
		return false;
	}

	HOSTENT * hostEntry = gethostbyname(domain.c_str());
	if (!hostEntry)
	{
		return false;
	}

	struct in_addr addr;
	addr.s_addr = *(u_long *) hostEntry->h_addr_list[0]; 
	ip = inet_ntoa(addr);

	return true;

}
*/

/*
bool PortUtility::isVistaAnd7()
{

#ifndef UNDER_CE

	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&os);

	if (os.dwMajorVersion >= 6)
	{
		return true;
	}

#endif
	return false;
}
*/

bool PortUtility::getPortSIPVpnIp(string & ip)
{
	initializeWinsock();

	/*PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);


	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			string adapterName = pAdapter->AdapterName;
			string adapterDescription = pAdapter->Description;

			transform(adapterName.begin(), adapterName.end(), adapterName.begin(), tolower);
			transform(adapterDescription.begin(), adapterDescription.end(), adapterDescription.begin(), tolower);

			if (adapterDescription.find("portsip") != string::npos)
			{

				ip = pAdapter->IpAddressList.IpAddress.String;

				return true;
			}

			pAdapter = pAdapter->Next;
		}
	}
	*/
	return false;

}


/*
bool PortUtility::openSocketPair(const string & localIp, SOCKET & rtpSocket, SOCKET & rtcpSocket, int & portBase)
{
	unsigned int interfaceIp = 0;

	rtpSocket = -1;
	rtcpSocket = -1;

	rtpSocket = openPort(portBase, interfaceIp);
	rtcpSocket = openPort(portBase+1, interfaceIp);
	if (rtpSocket == -1)
	{
		return false;
	}

	return true;
}
*/
/*
SOCKET PortUtility::openPort(int port, unsigned int interfaceIp)
{
	SOCKET s  = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		return -1;
	}

	struct sockaddr_in addr;
	memset((char*) &(addr),0, sizeof((addr)));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		int e = WSAGetLastError();
		closesocket(s);
		s = -1;
	}

	return s;
}
*/

std::string PortUtility::hexEncode(unsigned char * data, int length) 
{
	std::string tmp;
	unsigned const char *c = data;
	char buf[3];
	while(length != 0) 
	{
		sprintf(buf, "%02X", (unsigned char) *c);

		tmp += buf;
		c++;
		--length;
	}
	return tmp;
}   

/*
void PortUtility::hexDecode(std::string str, unsigned char * data, int & dataSize) 
{   
	if (str.length()%2 > dataSize)
	{
		return;
	}

	dataSize = 0;

	const char *c = str.c_str();    
	char  * x = (char *)data;      
	while(*c != 0) 
	{        
		sscanf(c, "%2X", x++);                 
		c += 2;    
		dataSize++;
	}        
} 
*/
void PortUtility::CheckFilePath(std::string  strFilePath)
{
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		int flag = _mkdir(strFilePath.c_str());
		if (flag != 0)
		{
			std::string strMsgError = "there is something error where create dir:";
			strMsgError += strFilePath;
			LOG4_INFO(strMsgError.c_str());
		}
	}
}
std::string  PortUtility::CheckFileExist(std::string strFilePath, std::string strFileName)
{
	struct stat buffer;
	std::string strLocalTempleteFilePath = strFilePath + strFileName;
	std::string strFileNewName = "";
	int nIndex = 0;
	while (true)
	{
		strLocalTempleteFilePath = strFilePath + strFileName;
		if (stat(strLocalTempleteFilePath.c_str(), &buffer) != 0)
		{
			strFileNewName = strFileName;
			break;
		}
		else
		{
			strLocalTempleteFilePath.clear();
			nIndex++;
			std::stringstream strFileReName;
			strFileReName << "(" << nIndex << ")";
			std::string strFileNameBehind; 
			std::string strTempleteFileNameFront;
			int nPos= strFileName.find(".");
			if (nPos>=0)
			{
				strFileNameBehind = strFileName.substr(nPos,strFilePath.length());
				strTempleteFileNameFront = strFileName.substr(0, nPos);
				strFileNewName = strTempleteFileNameFront + strFileReName.str()+ strFileNameBehind;
				strLocalTempleteFilePath = strFilePath + strFileNewName;
			}
			else
			{
				strFileNewName = strTempleteFileNameFront + strFileReName.str();
				strLocalTempleteFilePath = strFilePath + strFileNewName;
			}
			if (stat(strLocalTempleteFilePath.c_str(), &buffer) != 0)
			{
				break;
			}
		}
	}
	return strFileNewName;
}
time_t PortUtility::StringToDatetime(const char *str)
{
	tm tm_;
	int year, month, day, hour, minute, second;
	sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;
	time_t t_ = mktime(&tm_); 
	return t_;  
}

/*
 *param [in] dwSize   xx.xxB ¡¢xx.xxKb ¡¢xx.xxM
 */

LPCTSTR PortUtility::FileSizeToFormat(DWORD dwSize)
{
	TCHAR* strSize = new TCHAR[20];
	ZeroMemory(strSize, sizeof(TCHAR) * 20);

	double i = pow((double)2, 10);

	if (dwSize < pow((double)2, 10))//dwSize < 1024
	{
		_stprintf_s(strSize, 20, TEXT("%dB"), dwSize);// B
	}
	else if (pow((double)2, 10) <= dwSize && dwSize < pow((double)2, 20))// 1024 <= dwSize < 1024*1024
	{
		float fSize = (float)(dwSize * 100 / 1024) / 100;
		_stprintf_s(strSize, 20, TEXT("%.2fKB"), fSize);
	}
	else if (pow((double)2, 20) <= dwSize && dwSize < pow((double)2, 30))// 1024*1024 <= dwSize < 1024*1024*1024
	{
		float fSize = (float)(dwSize / 1024 * 100 / 1024) / 100;
		_stprintf_s(strSize, 20, TEXT("%.2fM"), fSize);// M
	}
	else if (pow((double)2, 30) <= dwSize && dwSize < pow((double)2, 40)) // 1024*1024*1024 <= dwSize < 1024*1024*1024*1024
	{
		float fSize = (float)(dwSize / 1024 * 100 / 1024 / 1024) / 100;
		_stprintf_s(strSize, 20, TEXT("%.2fG"), fSize);// G 
	}
	else
	{
		float fSize = (float)(dwSize / 1024 * 100 / 1024 / 1024 / 1024) / 100;
		_stprintf_s(strSize, 20, TEXT("%.2fT"), fSize);// T
	}

	return strSize;
}

int  PortUtility::GetLocalTimeZone()
{
	TIME_ZONE_INFORMATION tmp;
	GetTimeZoneInformation(&tmp);

	return tmp.Bias / (-60);
}

bool PortUtility::StrIsAllNum(std::string strSearch)
{
	stringstream sin(strSearch);
	double d;
	char c;
	if (!(sin >> d))
	{
		return false;
	}
	if (sin >> c)
	{
		return false;
	}
	return true;
}

vector<string> PortUtility::split2(string str, char del)
{
	stringstream ss(str);
	string tok;
	vector<string> ret;
	while (getline(ss, tok, del))
	{
		if (tok > "")
			ret.push_back(tok);
	}
	return ret;
};
std::string PortUtility::getMimeType(std::string & strEXtension)
{
	return mime_types::extension_to_type(strEXtension);
}

int PortUtility::gcd2(int w, int h) {
	if (w == 0) {
		return h;
	}
	if (h == 0) {
		return w;
	}
	if (~w & 1) {
		if (h & 1) {
			return gcd2(w >> 1, h);
		}
		else {
			return gcd2(w >> 1, h >> 1) << 1;
		}
	}
	if (~h & 1) {
		return gcd2(w, h >> 1);
	}
	if (w > h) {
		return gcd2((w - h) >> 1, h);
	}
	return gcd2((h - w) >> 1, w);
}
std::string PortUtility::getFileExtension(string strFileName)
{
	auto nPos = strFileName.rfind(".");
	if (nPos)
	{
		return strFileName.substr(nPos, strFileName.length());
	}
	else
	{
		return ".txt";
	}
}
CIUIString PortUtility::FormatPlayTime(std::string strPlayTime)
{
	if (strPlayTime.empty()||compareStringNoCase(strPlayTime,"00:00")==0||compareStringNoCase("00:00:00",strPlayTime)==0)
	{
		return _T("--:-- / -- : --");
	}
	CIUIString strDuration, strTotal, strCurrent;
	strTotal = PortUtility::string2WString(strPlayTime).c_str();

	int nStrPos = 0;
	int num = 0;
	while ((nStrPos = strTotal.Find(L':', nStrPos)) != -1)
	{
		nStrPos++;
		num++;
	}
	if (num > 1)
	{
		strCurrent = L"00:00:00";
	}
	else
	{
		strCurrent = L"00:00";
	}
	strDuration = strCurrent + L"/" + strTotal;
	return strDuration;
}

bool PortUtility::CheckEmailAddress(CIUIString csEmailAddress)
{
	if (csEmailAddress.IsEmpty() == TRUE)
	{
		return false;
	}
	std::smatch matchResult;
	regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
	std::string search_string = PortUtility::Unicode2Utf8_ND(csEmailAddress.GetBuffer(csEmailAddress.GetLength())).c_str();
	if (regex_match(search_string, matchResult,pattern))
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool PortUtility::CheckExtersionNumber(CIUIString csEmailAddress)
{
	if (csEmailAddress.IsEmpty()==TRUE)
	{
		return false;
	}
	regex pattern("([a-zA-Z0-9#*]{1,})");
	std::string search_string = PortUtility::Unicode2Utf8_ND(csEmailAddress.GetBuffer(csEmailAddress.GetLength())).c_str();
	if (regex_match(search_string, pattern))
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

HRESULT  PortUtility::GetShellThumbnailImage(LPCWSTR pszPath, HBITMAP* pThumbnail)
{
	HRESULT hr;
	*pThumbnail = NULL;

	LPITEMIDLIST pidlItems = NULL, pidlURL = NULL, pidlWorkDir = NULL;
	WCHAR szBasePath[MAX_PATH], szFileName[MAX_PATH];
	WCHAR* p;
	wcscpy(szBasePath, pszPath);
	p = wcsrchr(szBasePath, L'\\');
	if (p) *(p + 1) = L'\0';
	wcscpy(szFileName, pszPath + (p - szBasePath) + 1);

	do
	{
		CComPtr<IShellFolder> psfDesktop;
		hr = SHGetDesktopFolder(&psfDesktop);
		if (FAILED(hr)) break;

		CComPtr<IShellFolder> psfWorkDir;
		DWORD dwAttribs = SFGAO_COMPRESSED;
		ULONG cbEaten;
		hr = psfDesktop->ParseDisplayName(NULL, NULL, szBasePath, &cbEaten, &pidlWorkDir, NULL);
		if (FAILED(hr)) break;
		hr = psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir);
		if (FAILED(hr)) break;

		hr = psfWorkDir->ParseDisplayName(NULL, NULL, szFileName, NULL, &pidlURL, NULL);
		if (FAILED(hr)) break;
		CComPtr<IExtractImage> peiURL;
		hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);
		if (FAILED(hr)) break;
		SIZE size = { 256, 256 };
		DWORD dwPriority = 0, dwFlags = IEIFLAG_ASPECT;
		WCHAR pszImagePath[MAX_PATH];
		hr = peiURL->GetLocation(pszImagePath, MAX_PATH, &dwPriority, &size, 16, &dwFlags);
		if (FAILED(hr)) break;

		hr = peiURL->Extract(pThumbnail);
		if (FAILED(hr)) break;

		break;
	} while (0);

	if (pidlWorkDir) CoTaskMemFree(pidlWorkDir);
	if (pidlURL) CoTaskMemFree(pidlURL);
	return hr;
}
bool PortUtility::stringToLL(const std::string &str, long long &val)
{
	bool bRet = false;
	const char *nptr = str.c_str();
	char *endptr = NULL;
	errno = 0;
	val = strtoll(nptr, &endptr, 10);
	if ((errno == ERANGE && (val == LLONG_MAX || val == LLONG_MIN))
		|| (errno != 0 && val == 0))
	{
		val = 0;
		LOG4_INFO("error ocur\n");
	}
	else if (endptr == nptr)
	{
		LOG4_INFO("no digit find \n");
		val = 0;
	}
	else if (*endptr != '\0')
	{
		LOG4_INFO("Further characters after number");
		val = 0;
	}
	else
	{
		bRet = true;
	}
	return bRet;
}

 bool  PortUtility::IsContainsStr(string strSrc, string strContain)
{
	string::size_type idx = strSrc.find(strContain);
	if (idx != string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
};

 ENUM_LANGUAGE PortUtility::GetSystemDefaultLang()
 {
	 ENUM_LANGUAGE enumLanguage = ENUM_ENGLISH;
	 LANGID lid = GetSystemDefaultLangID();
	 switch (lid)
	 {
	 case 0x0804:
		 enumLanguage= ENUM_CHINAESE_S;
		 LOG4_INFO("system choise china");
		 break;
	 case 0x0409:
		 enumLanguage= ENUM_ENGLISH;
		 LOG4_INFO("system choise england");
		 break;
	 default:
		 LOG4_INFO("system choise england");
		 break;
	 }
	 return enumLanguage;
 }
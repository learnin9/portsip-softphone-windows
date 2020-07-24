#ifndef USER_H
#define USER_H
/*
Reserved             VARCHAR(1024),
*/
/*
*MARK: Êý¾Ý¿âÈ±ÉÙ×Ö¶Î¡¾sipDomain¡¿
*/
class CUser
{
public:
	CUser();
	~CUser();
	int GetUID() const { return m_nUserID; };
	int SetFaceImage(std::string strImagePath);
	void SetPresenceMode(int nPresenceMode);
	int GetProsenceMode() { return m_nPresencMode; }
	int GetProtocolMode() {
		return m_nTransport;
	}
	void SetUserFaceImage(bool bSet)
	{
		m_bSetUserFaceImage = bSet;
	}
	bool GetUserFaceImageSet()
	{
		return m_bSetUserFaceImage;
	}
	std::string  GetFaceImage() const ;
	std::string	 GetSipServer() const;
	int          GetSipPort()	const;
	std::string  GetSIPURL()
	{
		std::string strURL = m_strSipNumber + "@" + m_strSipDomain;
		return strURL;
	}
	std::string GetPrintfDisplayName()
	{
		if (!m_strDisplayName.empty())
		{
			return m_strDisplayName;
		}
		return m_strSipDomain;
	}
	//ID
	int        m_nUserID;
	std::string m_strSipNumber;
	std::string m_strPassword;
	std::string m_strFaceImage;
	std::string			m_strRecenTtime;
	std::string m_strProxyServer;
	int			m_nProxyPort;
	std::string m_strStunServer;
	int			m_nStunPort;
	bool        m_bKeepAlive;
	int         m_nRport;
	bool        m_bRemberMe;
	bool		m_bAutoLogin;
	bool		m_bTLSVerify;
	int			m_nSrtpMode;
	std::string m_strDisplayName;
	std::string m_strAuthName;
	int			m_nTransport;
	std::string m_strOutboundServer;
	int         m_nOutboundPort;
	bool		m_bIPv6;
	std::string m_strSign;
	int m_nSubscribeRefreshTime;
	int m_nPublishRefreshTime;
	int m_nPresencMode;
	//SIPIP+PORT
	std::string m_strSipURL;
	//SIP DOMAIN
	std::string m_strSipDomain;
	std::string m_strReserved;
	std::string m_strHomeNumber;
	bool m_bSetUserFaceImage;
};
#endif
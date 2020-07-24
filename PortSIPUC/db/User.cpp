#include "stdafx.h"
#include "User.h"
CUser::CUser()
{
	 m_nUserID			= 0;
	 m_strPassword		="";
	// m_strSipInfo		="";
	 m_strRecenTtime	 ="";
	 m_strProxyServer   ="";
	 m_nProxyPort		=0;
	 m_strStunServer	="";
	 m_nStunPort		=0;
	 m_bKeepAlive		=true;
	 m_nRport			=0;
	 m_bRemberMe		= true;
	 m_bTLSVerify		=false;
	 m_nSrtpMode		=0;
	 m_strDisplayName	="";
	 m_nTransport		=0;
	 m_strOutboundServer ="";
	 m_nOutboundPort	 =0;
	 m_bIPv6			 = false;
	 m_strSign			 = "";
	 m_nSubscribeRefreshTime = 900;
	 m_nPublishRefreshTime = 900;
	 m_nPresencMode		 = 0;
	 m_strSipURL		 = "";
	 m_strReserved		 = "";
	 m_strFaceImage		 = "";
	 m_strSipDomain = "";
	 m_strAuthName = "";
	 m_strHomeNumber = "";
	 m_bAutoLogin = false;
}
CUser::~CUser()
{
	m_bSetUserFaceImage = false;
}

int CUser::SetFaceImage(std::string strImagePath)
{
	m_strFaceImage = strImagePath;
	return 0;
}
void CUser::SetPresenceMode(int nPresenceMode)
{
	m_nPresencMode = nPresenceMode;
}
std::string CUser::GetFaceImage() const
{
	return m_strFaceImage;
}

std::string	 CUser::GetSipServer() const
{
	int nPos = m_strSipDomain.find(":");
	if (nPos!= string::npos)
	{
		std::string strSIP= m_strSipDomain.substr(0,nPos);
		return strSIP;
	}
	else
	{
		return m_strSipDomain;
	}
}
int CUser::GetSipPort()	const
{
	int nPos = m_strSipDomain.find(":");
	if (nPos != string::npos)
	{
		int nPort = atoi(m_strSipDomain.substr(nPos+1, m_strSipDomain.length()).c_str());
		return nPort;
	}
	else
	{
		return 5060;
	}
}
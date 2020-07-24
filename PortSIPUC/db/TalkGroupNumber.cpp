#include "stdafx.h"
#include "TalkGroupNumber.h"
#include "../Utility/utility.hxx"
CTalkGroupNumber::CTalkGroupNumber()
{
	m_nTalkGroupID = -1;
	m_strSIPNumber = "";
	m_strSelfDisplayName = "";
	m_bIsManager = false;
	m_bIsCreator = false;
	m_bFavorite = false;
	m_bIsDisable = false;
	m_nCallHistoryID = -1;
	m_nActiveLine = -1;
	m_bIsConnected = true;
}
CTalkGroupNumber::~CTalkGroupNumber()
{

}
#include "stdafx.h"
#include "portsql.hxx"
#include "../Utility/utility.hxx"
#include <sstream>
using namespace std;
#include "../System/SystemEx.h"
PortSQL::PortSQL()
		:m_pSQLDB(NULL)
{
	m_bIsConnected = false;
}


PortSQL::~PortSQL()
{
	closeDB();
}
std::string PortSQL::getStringFromColumnChar(const unsigned char *pval)
{
	if (pval==NULL)
	{
		return "";
	}
	string strTempValue =(char *) pval;
	return strTempValue;
}
/***************************************************T_USER****************************************************************/
bool PortSQL::deleteAccount(int accountID)
{
	std::stringstream ssSql;
	ssSql << "DELETE FROM " << T_USER << " WHREE (UserID=" << accountID <<" )";

	return true;
}
int PortSQL::GetVoiceMailCount()
{
	std::stringstream ssSql;
	ssSql << "SELECT COUNT(*) FROM" << T_VOICE_MAIL;
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0, nPrimaryKeyID = -1;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				nPrimaryKeyID = atoi(dbResult[nIndex]);
				break;
			}
		}
	}
	return nPrimaryKeyID;
}
CVoiceMail * PortSQL::GetVoiceMailByID(long long nID )
{
	bool bRet = false;
	std::stringstream ssSql;
	ssSql << "select * from " << T_VOICE_MAIL << " where ID =  " << nID <<";";
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		CVoiceMail* pLocalVoiceMail = new CVoiceMail;
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
		
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseVoiceMail(pLocalVoiceMail, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			bRet = true;
			return pLocalVoiceMail;
		}
	}
	else
	{
		bRet = false;
	}
	sqlite3_free_table(dbResult);
	return NULL;
}
const std::vector<CVoiceMail> PortSQL::GetVoiceMail(int nUserID,int nPageNum, int nMaxNum)
{
	std::vector<CVoiceMail> theVecVoiceMail;
	theVecVoiceMail.clear();

	CVoiceMail theLocalVoiceMail;
	//eg
	// "select * from T_CONTACTS limit (nPageNum-1)*nMaxNum,nMaxNum", //size:每页显示条数，index页码
	//	select * from T_CONTACTS limit (1-1)*3,3
	//	select * from T_CONTACTS limit (2-1)*3,3
	std::stringstream ssSql;
	ssSql << "select * from "<<T_VOICE_MAIL<<" where UserID = "<<nUserID<<" limit "<<nPageNum -1<<"*"<<nMaxNum<<","<< nMaxNum;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{

		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CVoiceMail theLocalVoiceMail;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseVoiceMail(theLocalVoiceMail, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			theVecVoiceMail.push_back(theLocalVoiceMail);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecVoiceMail;
}

const std::vector<CVoiceMail*> PortSQL::GetVoiceMailDownLoadSuccess(int nUserID,int nPageNum, int nMaxNum )
{
	int MsgSize = nPageNum * MAX_CALL_VIEW_NUM;
	std::vector<CVoiceMail*> theVecVoiceMail;
	theVecVoiceMail.clear();
	//eg
	// "select * from T_CONTACTS limit (nPageNum-1)*nMaxNum,nMaxNum", 
	//	select * from T_CONTACTS limit (1-1)*3,3
	//	select * from T_CONTACTS limit (2-1)*3,3
	std::stringstream ssSql;
	//ssSql << "select * from " << T_VOICE_MAIL <<" where DownLoadStatus="<<int(ENUM_OK)<< " limit " << nPageNum - 1 << "*" << nMaxNum << "," << nMaxNum;
	ssSql << "select * from " << T_VOICE_MAIL << " where DownLoadStatus=" << int(ENUM_OK) <<" and UserID = "<<nUserID<< " order by Time desc limit " << MsgSize  <<";" ;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CVoiceMail* pLocalVoiceMail = new CVoiceMail;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseVoiceMail(pLocalVoiceMail, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			theVecVoiceMail.push_back(pLocalVoiceMail);
		}
		sqlite3_free_table(dbResult);
	}
	return theVecVoiceMail;
}
void PortSQL::ParseVoiceMail(CVoiceMail& oVoiceMail, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_ID = atoll(columnValue);
	}
	tempName = "Caller";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_strCaller = (columnValue);
	}
	tempName = "Called";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_strCalled = (columnValue);
	}
	tempName = "Time";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_strTime = (columnValue);
	}
	tempName = "FileName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_strFileName = columnValue;
	}
	tempName = "FileStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_enumFileStatus = (ENUM_MSG_STATUS)atoi(columnValue);
	}
	tempName = "DownLoadStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_enumDownLoadStatus = (ENUM_FILE_STATUS)atoi(columnValue);
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oVoiceMail.m_strReserved = columnValue;
	}
	tempName = "ReadStatus";
	if (PortUtility::compareStringNoCase(name,tempName)==0)
	{
		oVoiceMail.m_bRead = (bool)atoi(columnValue);
	}
}
void PortSQL::ParseVoiceMail(CVoiceMail* pVoiceMail, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_ID = atoll(columnValue);
	}
	tempName = "Caller";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strCaller = (columnValue);
	}
	tempName = "Called";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strCalled = (columnValue);
	}
	tempName = "Time";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strTime = (columnValue);
	}
	tempName = "FileName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strFileName = columnValue;
	}
	tempName = "FileStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_enumFileStatus = (ENUM_MSG_STATUS)atoi(columnValue);
	}
	tempName = "DownLoadStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_enumDownLoadStatus = (ENUM_FILE_STATUS)atoi(columnValue);
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strReserved = columnValue;
	}
	tempName = "ReadStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_bRead = (bool)atoi(columnValue);
	}
	tempName = "PlayTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVoiceMail->m_strPlayTime = (columnValue);
	}
}
bool PortSQL::AddOneVoiceMail(CVoiceMail oVoiceMail)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;
	ssSql << "INSERT  INTO  " << T_VOICE_MAIL << "(ID, Caller, Called, Time, FileName, FileStatus, DownLoadStatus, \
	Reserved, PlayTime,UserID) VALUES(" \
		<< oVoiceMail.m_ID << ",'" << oVoiceMail.m_strCaller << "','" << oVoiceMail.m_strCalled << "','" << oVoiceMail.m_strTime \

		<< "','" <<oVoiceMail.m_strFileName<< "','" <<(int)oVoiceMail.m_enumFileStatus << "','" <<(int)oVoiceMail.m_enumDownLoadStatus << "','" << oVoiceMail.m_strReserved << "','" \
		<<oVoiceMail.m_strPlayTime<<"',"<<oVoiceMail.m_nUserID<< ")";
	string errnoMessage;

	return exec(PortUtility::ASCII2UTF_8_ND(ssSql.str()), NULL, NULL, errnoMessage);

}
bool PortSQL::UpdateOneVoiceMailDownLoadSuccess(long long nID)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;
	
	ssSql << "update  " << T_VOICE_MAIL << " set DownLoadStatus ="<< (int)ENUM_OK\
		<< " where ID = " << nID;
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::IsExistVoiceInfo(long long nID)
{
	bool bRet = false;
	std::stringstream ssSql;
	ssSql << "SELECT ID FROM " << T_VOICE_MAIL << " WHERE ID='" << nID << "';";
	bool existVoice = false;
	std::string strSql = ssSql.str();
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, strSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			existVoice = true;
		}
		sqlite3_finalize(statement);
	}
	return existVoice;
}
bool PortSQL::IsDownLoad(long long nID)
{
	bool bRet = false;
	std::stringstream ssSql;
	//mark
	ssSql << "SELECT ID FROM " << T_VOICE_MAIL << " WHERE ID= '" << nID << "' "<<"and DownLoadStatus = "<<(int)ENUM_OK << ";";
	bool existVoice = false;
	std::string strSql = ssSql.str();
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, strSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			existVoice = true;
		}
		sqlite3_finalize(statement);
	}
	return existVoice;
}
void PortSQL::DeleteVoiceMail(long long  nCallHistoryID)
{
	if (!m_pSQLDB)
	{
		return;
	}
	std::stringstream ssSql;
	ssSql << "delete  * from  " << T_VOICE_MAIL << "where ID = " << nCallHistoryID;
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
const CUser* PortSQL::getActiveAccount(bool& bReslut)
{
	CUser* pLocalUser = new CUser;
	if (!m_pSQLDB)
	{
		LOG4_INFO("DataBase Didn't Connect");
		bReslut = false;
		return pLocalUser;
	}

	std::stringstream ssSql;
	ssSql << "SELECT * FROM " << T_USER << " order by datetime(UpdateTime) desc limit 1";

	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, ssSql.str().c_str(), -1, &statement, NULL) == SQLITE_OK) {
		bReslut = true;
		if (sqlite3_step(statement) == SQLITE_ROW) {
			pLocalUser->m_nUserID = sqlite3_column_int(statement, 0);
			pLocalUser->m_strPassword = getStringFromColumnChar(sqlite3_column_text(statement, 1));
			//theLocalUser.m_strSipInfo = getStringFromColumnChar(sqlite3_column_text(statement, 2));
			pLocalUser->m_strRecenTtime = getStringFromColumnChar(sqlite3_column_text(statement, 3));
			pLocalUser->m_strProxyServer = getStringFromColumnChar(sqlite3_column_text(statement, 4));
			pLocalUser->m_nProxyPort = sqlite3_column_int(statement, 5);
			pLocalUser->m_strStunServer = getStringFromColumnChar(sqlite3_column_text(statement, 6));
			pLocalUser->m_nStunPort = sqlite3_column_int(statement, 7);
			pLocalUser->m_bKeepAlive = (bool)sqlite3_column_int(statement, 8);
			pLocalUser->m_bRemberMe = (bool)sqlite3_column_int(statement, 9);
			pLocalUser->m_nRport = sqlite3_column_int(statement, 10);
			pLocalUser->m_bTLSVerify = sqlite3_column_int(statement, 11);
			pLocalUser->m_nSrtpMode = sqlite3_column_int(statement, 12);
			pLocalUser->m_strDisplayName = getStringFromColumnChar(sqlite3_column_text(statement, 13));
			pLocalUser->m_nTransport = sqlite3_column_int(statement, 14);
			pLocalUser->m_strOutboundServer = getStringFromColumnChar(sqlite3_column_text(statement, 15));
			pLocalUser->m_nOutboundPort = sqlite3_column_int(statement, 16);
			pLocalUser->m_bIPv6 = (bool)sqlite3_column_int(statement, 17);
			pLocalUser->m_strSign = getStringFromColumnChar(sqlite3_column_text(statement, 18));
			pLocalUser->m_nSubscribeRefreshTime= sqlite3_column_int(statement, 19);
			pLocalUser->m_nPublishRefreshTime = sqlite3_column_int(statement, 20);
			pLocalUser->m_nPresencMode = sqlite3_column_int(statement, 21);
			pLocalUser->m_strSipNumber = getStringFromColumnChar(sqlite3_column_text(statement, 22));
			pLocalUser->m_strReserved = getStringFromColumnChar(sqlite3_column_text(statement, 23));
			pLocalUser->m_strFaceImage = getStringFromColumnChar(sqlite3_column_text(statement, 24));
			pLocalUser->m_bAutoLogin = (bool)sqlite3_column_int(statement, 26);
		}
		sqlite3_finalize(statement);
	}
	else
	{
		bReslut = false;
	}
	return pLocalUser;
}
std::list< CUser*> PortSQL::getAllAccount()
{
	m_lstAccount.clear();

	if (!m_pSQLDB)
	{
		LOG4_INFO("DataBase Didn't Connect");
		return m_lstAccount;
	}

	std::stringstream ssSql;
	//datetime()
	ssSql << "SELECT * FROM " << T_USER << " order by UpdateTime desc";

	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, ssSql.str().c_str(), -1, &statement, NULL) == SQLITE_OK) {
		while(sqlite3_step(statement) == SQLITE_ROW) {
			CUser* pLocalUser = new CUser;
			pLocalUser->m_nUserID = sqlite3_column_int(statement, 0);
			pLocalUser->m_strPassword = getStringFromColumnChar(sqlite3_column_text(statement, 1));
			pLocalUser->m_strSipDomain = getStringFromColumnChar(sqlite3_column_text(statement,2));
			pLocalUser->m_strRecenTtime = getStringFromColumnChar(sqlite3_column_text(statement, 3));
			pLocalUser->m_strProxyServer = getStringFromColumnChar(sqlite3_column_text(statement, 4));
			pLocalUser->m_nProxyPort	 = sqlite3_column_int(statement, 5);
			pLocalUser->m_strStunServer = getStringFromColumnChar(sqlite3_column_text(statement, 6));
			pLocalUser->m_nStunPort	 = sqlite3_column_int(statement, 7);
			pLocalUser->m_bKeepAlive	 = (bool)sqlite3_column_int(statement, 8);
			pLocalUser->m_bRemberMe     = (bool)sqlite3_column_int(statement, 9);
			pLocalUser->m_nRport		 = sqlite3_column_int(statement, 10);
			pLocalUser->m_bTLSVerify	 = sqlite3_column_int(statement, 11);
			pLocalUser->m_nSrtpMode	 = sqlite3_column_int(statement, 12);
			pLocalUser->m_strDisplayName	= getStringFromColumnChar(sqlite3_column_text(statement, 13));
			pLocalUser->m_nTransport		= sqlite3_column_int(statement, 14);
			pLocalUser->m_strOutboundServer	= getStringFromColumnChar(sqlite3_column_text(statement, 15));
			pLocalUser->m_nOutboundPort		= sqlite3_column_int(statement, 16);
			pLocalUser->m_bIPv6				=(bool)sqlite3_column_int(statement, 17);
			pLocalUser->m_strSign				= getStringFromColumnChar(sqlite3_column_text(statement, 18));
			pLocalUser->m_nSubscribeRefreshTime = sqlite3_column_int(statement, 19);
			pLocalUser->m_nPublishRefreshTime	= sqlite3_column_int(statement, 20);
			pLocalUser->m_nPresencMode			= sqlite3_column_int(statement, 21);
			pLocalUser->m_strSipNumber			= getStringFromColumnChar(sqlite3_column_text(statement, 22));
			pLocalUser->m_strReserved			= getStringFromColumnChar(sqlite3_column_text(statement, 23));
			pLocalUser->m_strFaceImage			= getStringFromColumnChar(sqlite3_column_text(statement, 24));
			pLocalUser->m_bAutoLogin            = (bool)sqlite3_column_int(statement, 26);
			m_lstAccount.push_back(pLocalUser);
		}
		sqlite3_finalize(statement);
	}

	return m_lstAccount;
}

int PortSQL::saveActiveAccount(CUser* pUser)
{
	int nUserID = -1;

	if (!m_pSQLDB)
	{
		return  nUserID;
	}
	std::stringstream ssSql;
	ssSql << "SELECT * FROM " << T_USER << " WHERE SipNumber='" << pUser->m_strSipNumber << "' AND SipDomain= '" <<pUser->m_strSipDomain<<"';";// << "'and SipDomain='" << oUser.m_strSipDomain << "'";
	bool existAccount = false;
	std::string strSql = ssSql.str();
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, strSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			existAccount = true;
			nUserID = sqlite3_column_int(statement, 0);	
		}
		sqlite3_finalize(statement);
	}
	string serializedAccount;
	ssSql.clear();
	ssSql.str("");
	if (existAccount)
	{
		ssSql << "UPDATE " << T_USER << " SET    UpdateTime= datetime('now','localtime'),Password =  "<< pUser->m_strPassword <<", SipDomain= '"<< pUser->m_strSipDomain \
			<<"', Password ="<< pUser->m_strPassword<<",ProxyServerPort  = "<< pUser->m_nProxyPort\
			<<", ProxyServer =  '"<<(pUser->m_strProxyServer)<<"',StunServer = '"<< (pUser->m_strStunServer)\
			<<"', StunServerPort = "<< pUser->m_nStunPort
			<<", KeepLive = "<<(pUser->m_bKeepAlive==false? 0:1)<< ", RemberMe = "<< (pUser->m_bRemberMe == false ? 0 : 1)\
			<<", rport = "<< pUser->m_nRport<< ", TLSVerify = "<< (pUser->m_bTLSVerify == false ? 0 : 1)\
			<<", SrtpMode = " << pUser->m_nSrtpMode<< ", DisplayName = '" << (pUser->m_strDisplayName)\
			<<"', Transport = "<< pUser->m_nTransport<< ", OutboundServer = '" << (pUser->m_strOutboundServer)\
			<<"', OutboundPort = " << pUser->m_nOutboundPort << ", ipv6 = " << (pUser->m_bIPv6 == false ? 0 : 1)\
			<< ", Sign = '" << (pUser->m_strSign) << "', SipPollTime = " << pUser->m_nSubscribeRefreshTime\
			<< ", SipFreshTime = " << pUser->m_nPublishRefreshTime << ", AutoLogin = " << (int)pUser->m_bAutoLogin << ", PresencMode = " << pUser->m_nPresencMode\
			<< ", Reserved = '" << (pUser->m_strReserved)  <<"'  where UserID = "<<nUserID;
	}
	else
	{
		ssSql << "INSERT INTO " << T_USER << " (Password, SipDomain,UpdateTime,ProxyServer,ProxyServerPort,StunServer,StunServerPort,\
				KeepLive,RemberMe,rport,TLSVerify,SrtpMode,DisplayName,Transport,OutboundServer,OutboundPort,ipv6,Sign,\
			SipPollTime,SipFreshTime,PresencMode,SipNumber,Reserved,AutoLogin) values('" << pUser->m_strPassword << "','" << pUser->m_strSipDomain  \
			<< "', datetime('now','localtime')" << ",'" << pUser->m_strProxyServer << "','" << pUser->m_nProxyPort <<"','"<< (pUser->m_strStunServer) << "','" << pUser->m_nStunPort \
			<<  "','" << (pUser->m_bKeepAlive == false ? 0 : 1) << "','" << (pUser->m_bRemberMe == false ? 0 : 1) << "','" << pUser->m_nRport\
			<< "','" << (pUser->m_bTLSVerify == false ? 0 : 1) << "','" << pUser->m_nSrtpMode << "','" << (pUser->m_strDisplayName)\
			<< "','" << pUser->m_nTransport << "','" << (pUser->m_strOutboundServer) << "','" << pUser->m_nOutboundPort << "','" << (pUser->m_bIPv6 == false ? 0 : 1)\
			<< "','" << pUser->m_strSign << "','" << pUser->m_nSubscribeRefreshTime << "','" << pUser->m_nPublishRefreshTime << "','" << pUser->m_nPresencMode << "','"\
			<< pUser->m_strSipNumber << "','" << (pUser->m_strReserved) << "','" << (int)pUser->m_bAutoLogin<<"');";
		
	}
	string errnoMessage;
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	bool bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	if (bRet==true&&!existAccount)
	{
		nUserID = GetPrimaryKeyIDWhenInsert(T_USER);
	}
	return nUserID;
}

void  PortSQL::getOptions(COptions* pOptions,bool& bReslut)
{
	if (!m_pSQLDB)
	{
		LOG4_INFO("DataBase Didn't Connect");
		return ;
	}

	string sql = "select * from ";
	sql += T_OPTIONS;
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn=0,nIndex = 0;
	if (exec(sql, &dbResult, &nRow,&nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseOptions(pOptions, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
		}
		sqlite3_free_table(dbResult);
		bReslut = true;
		return ;
	}
	else
	{
		bReslut = false;
		sqlite3_free_table(dbResult);
		return;
	}
	
}


bool PortSQL::openDB(const string & dbPathName)
{
	if (dbPathName.empty() == true)
	{
		return false;
	}


	closeDB();

	if (sqlite3_open(dbPathName.c_str(), &m_pSQLDB) != SQLITE_OK)
	{
		char szErrorMsg[128] = { 0 };
		_snprintf(szErrorMsg, sizeof(szErrorMsg) - 1, "Open local database faile.path is :%s\n may the path shoudle be english", dbPathName.c_str());
		LOG4_INFO(szErrorMsg);
		return false;
	}
	m_bIsConnected = true;
	return true;
}


void PortSQL::closeDB()
{
	if (m_pSQLDB)
	{
		sqlite3_close(m_pSQLDB);
		m_pSQLDB = NULL;
	}
}

void PortSQL::ParseCallHistory(CCallHistory& oCallHistory, char * columnValue, char * columnName)
{
	if (columnValue ==NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_nCallHistoryID = atoi(columnValue);
	}
	tempName = "ContectType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_enumContentType = (ENUM_CONTENT_TYPE)atoi(columnValue) ;
	}
	tempName = "CallType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_enumCallType = (ENUM_CALL_TYPE)atoi(columnValue);
	}
	tempName = "Status";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_enumCallStatus = (ENUM_CALL_STATUS)atoi(columnValue);
	}
	tempName = "BeginTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_strBeginTime = columnValue;
	}
	tempName = "BeginTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_strBeginTime = columnValue;
	}
	tempName = "EndTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_strEndTime = columnValue;
	}
	tempName = "SIPNum";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_strSIPNum = columnValue;
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oCallHistory.m_strReserved = columnValue;
	};
	tempName = "ReadStatus";
	if (PortUtility::compareStringNoCase(name,tempName)==0)
	{
		oCallHistory.m_bRead = (bool)atoi(columnValue);
	}

}
void PortSQL::ParseOptions(COptions* pOptions, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nOptionsID = atoi(columnValue);
	}
	tempName = "AGC";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bAGC = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "AEC";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bAEC = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "VAD";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bVAD = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "CNG";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bCNG = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "ARPath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strARPath = columnValue;
	}
	tempName = "ARName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strARName = columnValue;
	}

	tempName = "VRPath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strVRPath = columnValue;
	}
	tempName = "VRName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strVRName = columnValue;
	}
	/*tempName = "AutoLoginSIPUri";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oOptions.
		//g_options.AutoLoginSIPUri = columnValue[i];

	}
	*/
	tempName = "DTMFMode";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFMode = atoi(columnValue);
	}
	tempName = "DTMFPayload";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFPayload = atoi(columnValue);
	}
	tempName = "DTMFDuration";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFDuration = atoi(columnValue);
	}
	tempName = "JitBuffer";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nJitterBuffer = atoi(columnValue);
	}
	tempName = "AudioInputDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioInputDeviceID = atoi(columnValue);
	}
	tempName = "AudioOutputDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioOutputDeviceID = atoi(columnValue);
	}

	tempName = "VideoDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoDeviceID = atoi(columnValue);
	}

	tempName = "KeepLive";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nKeepAlive = atoi(columnValue) == 0 ? false : true;
	}
	
	tempName = "ReverseLocalVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseLocalVideo = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "ReverseReceivedVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseReceivedVideo = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "ReverseSendingVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseSendingVideo = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "VideoImageQualityLevel";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoImageQualityLevel = atoi(columnValue);
	}
	tempName = "ForwardTo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strForwardTo = columnValue;
	}
	tempName = "BusyForwardTo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strBusyForwardTo = columnValue;
	}
	tempName = "NetworkInterface";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strNetworkInterface = columnValue;
	}
	tempName = "VRPath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strVRPath = columnValue;
	}
	tempName = "VRName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strVRName = columnValue;
	}
	/*tempName = "AutoLoginSIPUri";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oOptions.
		//g_options.AutoLoginSIPUri = columnValue[i];

	}
	*/
	tempName = "DTMFMode";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFMode = atoi(columnValue);
	}
	tempName = "DTMFPayload";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFPayload = atoi(columnValue);
	}
	tempName = "DTMFDuration";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDTMFDuration = atoi(columnValue);
	}
	tempName = "JitBuffer";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nJitterBuffer = atoi(columnValue);
	}
	tempName = "AudioInputDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioInputDeviceID = atoi(columnValue);
	}
	tempName = "AudioOutputDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioOutputDeviceID = atoi(columnValue);
	}

	tempName = "VideoDeviceId";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoDeviceID = atoi(columnValue);
	}

	tempName = "KeepLive";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nKeepAlive = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "ReverseLocalVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseLocalVideo = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "ReverseReceivedVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseReceivedVideo = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "ReverseSendingVideo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bReverseSendingVideo = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "VideoImageQualityLevel";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoImageQualityLevel = atoi(columnValue);
	}
	tempName = "ForwardTo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strForwardTo = columnValue;
	}
	tempName = "BusyForwardTo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strBusyForwardTo = columnValue;
	}
	tempName = "NetworkInterface";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strNetworkInterface = columnValue;
	}

	tempName = "DisableCABAC";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDisableCABAC = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "NoAnswerForwardTo";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strNoAnswerFowardTo = columnValue;
	}

	tempName = "NoAnwerWaitSeconds";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nNoAnswerWaitSeconds = atoi(columnValue);
	}

	tempName = "AudioRtpMiniPort";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioRtpMinPort = atoi(columnValue);
	}

	tempName = "AudioRtpMaxPort";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioRtpMaxPort = atoi(columnValue);
	}
	tempName = "VideoRtpMiniPort";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoRtpMinPort = atoi(columnValue);
	}
	tempName = "VideoRtpMaxPort";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoRtpMaxPort = atoi(columnValue);
	}

	tempName = "Language";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nLanguage =atoi( columnValue);
		pOptions->m_nLastLanguage = pOptions->m_nLanguage;
	}

	tempName = "SaveLanguage";
	if (PortUtility::compareStringNoCase(name, tempName)==0)
	{
		pOptions->m_bSaveLanguage = (bool)atoi(columnValue);
	}
	tempName = "EnableMwi";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableMWI = atoi(columnValue) == 0 ? false : true;
	}

	tempName = "RtpKeepLive";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bRTPKeepLive = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "LastLoginUri";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strLastLoginURL = columnValue;
	}
	tempName = "LocalVideoInRoom";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bLocalVideoInRoom = atoi(columnValue) == 1 ? true : false;
	}
	tempName = "VideoResolution";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoResolution = atoi(columnValue);
	}
	tempName = "SessionTimerTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nSessionTimerTime = atoi(columnValue);
	}
	tempName = "AudioDSCP";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioDSCP = atoi(columnValue);
	}
	tempName = "AudioServiceType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nAudioSeviceType = atoi(columnValue);
	}
	tempName = "VideoDSCP";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoDSCP = atoi(columnValue);
	}
	tempName = "VideoServiceType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nVideoServiceType = atoi(columnValue);
	}
	tempName = "temp";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strTemp = columnValue;
	}
	tempName = "st";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strSt = columnValue;
	}
	tempName = "RtpKeepAlivePayload";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nRtpKeepAlivePayload = atoi(columnValue);
	}
	tempName = "DeltaTransmitTimeMS";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nDeltaTransmitTimeMS = atoi(columnValue);
	}
	tempName = "Priority";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nPriority = atoi(columnValue);
	}
	tempName = "defaultSoftphone";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bDefaultSoftphone = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "displayIdle";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bDisplayidle = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "showTimeStamp";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bShowTimeStamp = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "callImmediately";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bCallImmediately = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "recordFolder";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strRecordFolder = columnValue;
	}
	tempName = "ANS";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bANS = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "logLevel";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_nLogLevel = atoi(columnValue);
	}
	tempName = "logPath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_strRecordFolder = columnValue;
	}
	tempName = "EnableForword";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableForword = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "EnableBusyForword";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableOnPhoneForword = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "EnableVideoDev";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableVideoDev = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "EnableAudioOutputDev";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableAudioOutputDev = atoi(columnValue) == 0 ? false : true;
	}
	tempName = "EnableAudioInputDev";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pOptions->m_bEnableAudioInputDev = atoi(columnValue) == 0 ? false : true;
	}
}


bool PortSQL::exec(const string & sql, portsql_callback  callback, void * privateData, string & errormsg)
{
	if (sql.empty() == true)
	{
		return false;
	}

	if (!m_pSQLDB)
	{
		return false;
	}
	char * erro = NULL;
	if (sqlite3_exec(m_pSQLDB, sql.c_str(), callback, privateData, &erro) != SQLITE_OK)
	{
		errormsg = erro;
		std::stringstream ssStr;
		ssStr << errormsg;
		std::string strSqlError = "Exe SQL ERROR " + ssStr.str();
		LOG4_ERROR(strSqlError.c_str());
		LOG4_ERROR(sql.c_str());
		return false;
	}
	return true;
}

int PortSQL::GetPrimaryKeyIDWhenInsert(const char *szTable)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:select last_insert_rowid() from T_CALL_HISTORY limit 1;
	*/
	ssSql << "select last_insert_rowid() from  " << szTable << " limit 1";
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0, nPrimaryKeyID = -1;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CCallHistory theLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				nPrimaryKeyID = atoi(dbResult[nIndex]);
				break;
			}
		}
	}
	return nPrimaryKeyID;
}
bool PortSQL::saveCallMsg(CCallHistory oCallHistory, int &nRecordPrimaryKeyID)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:insert into T_CALL_HISTORY (ContentType,CallType,BeginTime,SIPNum) values('1','1',datetime('now'),'888888','1');
	*/
	ssSql << "INSERT INTO " << T_CALL_HISTORY << " (ContentType,Status,CallType,BeginTime,SIPNum,UserID) \
	 VALUES('" << (int)oCallHistory.m_enumContentType << "','" <<oCallHistory.m_enumCallStatus<< "','" <<(int)oCallHistory.m_enumCallType << "'," << "  datetime('now','localtime')" << ",'" << oCallHistory.m_strSIPNum << "','" <<oCallHistory.m_nUserID<< "')";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	if (bRet ==true)
	{
		nRecordPrimaryKeyID = GetPrimaryKeyIDWhenInsert(T_CALL_HISTORY);
	}
	return bRet;
}

bool PortSQL::updateCallMsg(CCallHistory oCallHistory, LONG lDataTime)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_CALL_HISTORY << " set(ContentType, CallType,Status,BeginTime,EndTime,SIPNum,Reserved) ('"\
		<< oCallHistory.m_enumContentType << "','" << oCallHistory.m_enumCallType << "','" << oCallHistory.m_enumCallStatus << "','"\
		<< "' ,' datetime('now','localtime')'" << "','" << "' ,' datetime('now','localtime')'" << "','" << oCallHistory.m_strSIPNum << "','" << oCallHistory.m_strReserved
		<< "',')"<<"where ID = "<< oCallHistory.m_nCallHistoryID;
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}

bool PortSQL::UpdateCallMsgEndTimeByID(long nCallHistoryPrimaryKeyID)
{
	if (!m_pSQLDB)
	{
		return false;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_CALL_HISTORY << " set EndTime =  datetime('now','localtime') where ID = " << nCallHistoryPrimaryKeyID<<" ;";
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::UpdateCallMsgStatus(long nCallHistoryPrimaryKeyID, ENUM_CALL_STATUS enumCallStatus)
{
	if (!m_pSQLDB)
	{
		return false;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_CALL_HISTORY << " set Status = "<<int (enumCallStatus)<<" where ID = " << nCallHistoryPrimaryKeyID << " ;";
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL:: updateMissReadStatus()
{
	if (!m_pSQLDB)
	{
		return ;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_CALL_HISTORY << " set ReadStatus = " << int(true) << " where ReadStatus = " <<  int(false) << " or  ReadStatus  is null;";
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::updateVoiceMailStatus()
{

	if (!m_pSQLDB)
	{
		return;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_VOICE_MAIL << " set ReadStatus = " << int(true) << " where ReadStatus = " << int(false) << " or  ReadStatus  is null;";
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::updateRecordStatus()
{
	if (!m_pSQLDB)
	{
		return;
	}
	std::stringstream ssSql;
	ssSql << "update  " << T_VEDIO_AUDIO_RECORD << " set ReadStatus = " << int(true) << " where ReadStatus = " << int(false) << " or  ReadStatus  is null;";
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::DeleteCallMsg(long long  nCallHistoryID)
{
	if (!m_pSQLDB)
	{
		return  ;
	}
	std::stringstream ssSql;
	ssSql << "delete   from  " << T_CALL_HISTORY << " where ID = " << nCallHistoryID;
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
std::vector<CCallHistory> PortSQL::getCallMsg(int nCallMsgSize, int nCallerID, bool bConnected)
{
	std::vector<CCallHistory> theListLocalCallHistory;
	if (!m_pSQLDB)
	{
		return theListLocalCallHistory;
	}
	if (nCallerID<0)
	{
		LOG4_INFO("caller id is error");
		return theListLocalCallHistory;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CALL_HISTORY <<" where UserID = "<< nCallerID /*<<" and CallType = "<<(int) bConnected*/ << " order by BeginTime desc limit " << (nCallMsgSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CCallHistory theLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				if (nColumnIndex == 6)
				{
					nIndex = nIndex;
				}
				ParseCallHistory(theLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theListLocalCallHistory.push_back(theLocalCallHistory);
		}
	}
	sqlite3_free_table(dbResult);
	return theListLocalCallHistory;
}

std::vector<CCallHistory> PortSQL::GetAllCallMsg(int nMaxPage,int nUserID)
{
	int MsgSize = nMaxPage * MAX_CALL_VIEW_NUM;
	std::vector<CCallHistory> theListLocalCallHistory;
	if (!m_pSQLDB)
	{
		return theListLocalCallHistory;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CALL_HISTORY/* <<" where status =  "<<int(ENUM_CALL_STATUS_SUCC)<<*/" where UserID = "<< nUserID << " order by BeginTime desc limit " << (MsgSize);
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CCallHistory theLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseCallHistory(theLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theListLocalCallHistory.push_back(theLocalCallHistory);
		}
	}
	sqlite3_free_table(dbResult);
	return theListLocalCallHistory;

}

std::vector<CCallHistory> PortSQL::GetMissedCallMsg(int nMaxPage, int nUserID)
{
	int MsgSize = nMaxPage * MAX_CALL_VIEW_NUM;
	std::vector<CCallHistory> theListLocalCallHistory;
	if (!m_pSQLDB)
	{
		return theListLocalCallHistory;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CALL_HISTORY << " where status =  "<< int(ENUM_CALL_STATUS_FAILD) <<" and UserID = "<< nUserID << " order by BeginTime desc limit " << (MsgSize);
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CCallHistory theLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseCallHistory(theLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theListLocalCallHistory.push_back(theLocalCallHistory);
		}
	}
	sqlite3_free_table(dbResult);
	return theListLocalCallHistory;
}

std::vector<CCallHistory> PortSQL::getCallMsg(int nCallMsgSize)
{
	std::vector<CCallHistory> theListLocalCallHistory;
	if (!m_pSQLDB)
	{
		return theListLocalCallHistory;
	}

	std::stringstream ssSql;
	ssSql<< "select * from "<< T_CALL_HISTORY<< " order by BeginTime desc limit "<< (nCallMsgSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CCallHistory theLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseCallHistory(theLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theListLocalCallHistory.push_back(theLocalCallHistory);
		}
	}
	sqlite3_free_table(dbResult);
	return theListLocalCallHistory;
}
bool PortSQL::GetCallMsgByID(long long nCallHistoryID, CCallHistory &theLocalCallHistory)
{
	bool bRet = false;
	if (!m_pSQLDB)
	{
		return bRet;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CALL_HISTORY << " where ID= " << (nCallHistoryID);
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseCallHistory(theLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			bRet = true;
		}
	}
	else
	{
		bRet = false;
	}
	sqlite3_free_table(dbResult);
	return bRet;
}
bool PortSQL::addOneContact(CContactDB* pContact, string &errnoMessage)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	bool bExist = searchContact(pContact->m_strSIPNum);
	if (bExist)
	{
		errnoMessage = "exist same sip number";
		return false;
	}
	std::stringstream ssSql;
	//EG
	//INSERT OR REPLACE INTO T_CONTACTS(UserID, SipNumber, Work, ShareNote, HeadImage, WorkPhone, MobilePhone, FirstName, LastName, GroupName, Favorites, Reserved, HomeNumber, EmailAdress)VALUES(1, '88887', 'worker', '我就是我，不一样的烟火'
	//	, '', '07316555888', '18711173166', 'jim', 'green', '', 1, '', '07318888888', 'licancool@sohu.com')
	ssSql << "INSERT OR REPLACE INTO  " << T_CONTACTS << "(UserID, SipNumber, Work, ShareNote, HeadImage, WorkPhone, MobilePhone, \
	FirstName, LastName, GroupName, Favorites, Reserved, HomeNumber, EmailAdress) VALUES(" \
	<< pContact->m_nUserID << ",'" << pContact->m_strSIPNum << "','"<<pContact->m_strWork << "','"<<pContact->m_strShareNote<< "','"<<pContact->m_strHeadImage << "','"<<pContact->m_strWorkPhone << "','" << pContact->m_strMobilePhone\
	<< "','"<<pContact->m_strFirstName<< "','"<<pContact->m_strLastName << "','" << pContact->m_strGroupName << "'," << (pContact->m_bFavorites == false ? 0 : 1) << ",'" << pContact->m_strReserved\
	<< "','" <<pContact->m_strHomeNumber << "','"<<pContact->m_strEmailAdress << "')";
	
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::updateContact(CContactDB* pContact)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");

	ssSql << "UPDATE " << T_CONTACTS << " SET Favorites = " << (pContact->m_bFavorites == false ? 0 : 1) \
		<< "  ,SipNumber = '" << pContact->m_strSIPNum\
		<< "' ,WorkPhone = '" << pContact->m_strWorkPhone\
		<< " ' , MobilePhone = '" << pContact->m_strMobilePhone\
		<< " ' , FirstName = '" << pContact->m_strFirstName\
		<< " ' , LastName = '" << pContact->m_strLastName\
		<< " ',HomeNumber = '" << pContact->m_strHomeNumber\
		<< " ', EmailAdress = '" << pContact->m_strEmailAdress\
		<< "' where SipNumber = '" << pContact->m_strSIPNum\
		<<"';";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}
bool PortSQL::updateContactFavorite(CContactDB *pContact)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_CHAT_RECORD SET Status = 2 WHERE MsgID = 1;
	*/
	int nResult = (pContact->m_bFavorites) == false ? 0 : 1;
	ssSql << "UPDATE " << T_CONTACTS << " SET Favorites = " << nResult << " where SipNumber = " << pContact->m_strSIPNum;


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}
bool PortSQL::deleteContact(std::string  strSIPNum,int nUserID)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;
	ssSql << "delete  from  " << T_CONTACTS << " where SipNumber =  " << strSIPNum<<" and UserID = "<< nUserID;
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
std::vector<CContactDB> PortSQL::getContact()
{
	std::vector<CContactDB> theLocalContactDB;
	return theLocalContactDB;
}
bool PortSQL::searchContact(std::string strSipNumber)
{
	PortUtility::deleteAllMark(strSipNumber, " ");
	std::stringstream ssSql;
	ssSql << "SELECT * FROM " << T_CONTACTS << " WHERE SipNumber='" << strSipNumber << "';";
	bool existAccount = false;
	std::string strSql = ssSql.str();
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, strSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			existAccount = true;
		}
		
	}
	sqlite3_finalize(statement);
	return existAccount;
}

bool PortSQL::exec(const string & sql, char *** result, int * row, int * column, string & errormsg)
{
	if (sql.empty() == true)
	{
		return false;
	}

	if (!m_pSQLDB)
	{
		return false;
	}

	char * erro = NULL;

	if (sqlite3_get_table(m_pSQLDB, sql.c_str(), result, row, column, &erro ) != SQLITE_OK)
	{
		errormsg = erro;
		std::stringstream ssStr;
		ssStr << errormsg;
		std::string strSqlError = "Exe SQL ERROR " + ssStr.str();
		LOG4_ERROR(strSqlError.c_str());
		LOG4_ERROR(sql.c_str());
		return false;
	}

	return true;
}


void PortSQL::releaseResult(char ** result)
{
	if (!m_pSQLDB || !result)
	{
		return;
	}

	sqlite3_free_table(result);
}
bool PortSQL::tableExists(const string & tableName)
{
	if (tableName.empty() == true)
	{
		return false;
	}

	if (!m_pSQLDB)
	{
		return false;
	}

	string sql = "select * from sqlite_master where type = 'table' and name = '";
	sql += tableName;
	sql += "'";

	char * erro = NULL;
	char ** result = NULL;
	int row = 0;
	int colum = 0;

	if (sqlite3_get_table(m_pSQLDB, sql.c_str(), &result,&row, &colum, &erro) != SQLITE_OK)
	{
		return false;
	}
	if (row==0 && colum==0)
	{
		return false;
	}

	return true;
}

std::vector<TAG_RECENT*> PortSQL::getAllContacts(int nContactSize,int nUserID)
{
	std::vector<TAG_RECENT*> theListLocalContactDB;
	if (!m_pSQLDB)
	{
		return theListLocalContactDB;
	}
	
	std::stringstream ssSql;
	ssSql << "select * from " << T_CONTACTS<<" where UserID = "<< nUserID <<" limit "<< (nContactSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			TAG_RECENT* pLocalContactInfo = new TAG_RECENT;
			CContactDB *pLocalCallHistory = new CContactDB();

			pLocalContactInfo->m_enumRecentContact = ENUM_SINGLE_CONTACT;
			pLocalContactInfo->m_unionContactInfo.m_pContact = pLocalCallHistory;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseContact(pLocalCallHistory, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			pLocalCallHistory->m_bTrueContact = true;
			theListLocalContactDB.push_back(pLocalContactInfo);
		}
	}
	sqlite3_free_table(dbResult);
	return theListLocalContactDB;
}

void PortSQL::ParseContact(CContactDB* pCallHistory, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "UserID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_nUserID = atoi(columnValue);
	}
	tempName = "SipNumber";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strSIPNum = columnValue;
	}
	tempName = "Work";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strWork = (columnValue);
	}
	tempName = "ShareNote";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strShareNote = (columnValue);
	}
	tempName = "HeadImage";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strHeadImage = (columnValue);
	}
	tempName = "WorkPhone";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strWorkPhone = columnValue;
	}
	tempName = "MobilePhone";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strMobilePhone = columnValue;
	}
	tempName = "FirstName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strFirstName = columnValue;
	}
	tempName = "LastName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strLastName = columnValue;
	}

	tempName = "GroupName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strGroupName = columnValue;
	}
	tempName = "Favorites";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_bFavorites =(bool) atoi(columnValue);
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strReserved = columnValue;
	};
	tempName = "EmailAdress";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strEmailAdress = columnValue;
	};
	tempName = "HomeNumber";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pCallHistory->m_strHomeNumber = columnValue;
	};
}

int PortSQL::saveOneChatRecord(CChatRecord oChatRecord)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:insert into T_CHAT_RECORD (TYPE,DATETIME,IsMyself,Msg,Status,TalkerURL) values('1',datetime('now'),'0','121212121','0','4','555@sip.com','666@sip.com');
	*/
	ssSql << "INSERT INTO " << T_CHAT_RECORD << " (TYPE,DATETIME,IsMyself,Msg,Status,SignMsgID,LocalSignMsgID,SendLocalFilePath,SendLocalFilePath,TalkerURL,UserID,FileStatus,PlayTime) \
	 VALUES('" << (int)oChatRecord.m_nMSGType << "'," << " datetime('now','localtime')" << ",'" << (oChatRecord.m_nIsMyself == false ? 0 : 1) \
	<< "','" << oChatRecord.m_strMSG<< "','"  <<  (int)oChatRecord.m_nMSGStatus<< "','" <<oChatRecord.m_strSignMsgID<< "','" <<oChatRecord.m_strLocalSignMsgID\
	<< "','" << (oChatRecord.m_strSendLocalFilePath.empty() == false ? oChatRecord.m_strSendLocalFilePath : "") << "','" << (oChatRecord.m_strSendLocalFileName.empty() == false ? oChatRecord.m_strSendLocalFileName : "") << "','"<<oChatRecord.m_strTalkerURL<<"',"<<oChatRecord.m_nUserID << "," <<(int)oChatRecord.m_enumFileStatus << ",'" << oChatRecord.m_strPlayTime<<"')";

	
	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	int nMsgID = -1;
	if (bRet == true )
	{
		nMsgID = GetPrimaryKeyIDWhenInsert(T_USER);
	}
	return nMsgID;
}
bool PortSQL::saveOneOffLineChatRecord(CChatRecord oChatRecord)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	ssSql << "SELECT SignMsgID FROM " << T_CHAT_RECORD << " WHERE SignMsgID='" << oChatRecord.m_strSignMsgID << "' and UserID = "<< oChatRecord .m_nUserID<<";";
	bool existAccount = false;
	std::string strSql = ssSql.str();
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(m_pSQLDB, strSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		if (sqlite3_step(statement) == SQLITE_ROW) {
			existAccount = true;
		}
		sqlite3_finalize(statement);
	}

	if (existAccount==true)
	{
		return true;
	}

	ssSql.clear();
	ssSql.str("");
	/*
	*EG:insert into T_CHAT_RECORD (TYPE,DATETIME,IsMyself,Msg,Status,TalkerURL) values('1',datetime('now'),'0','121212121','0','4','555@sip.com');
	*/
	ssSql << "INSERT INTO " << T_CHAT_RECORD << " (TYPE,DATETIME,IsMyself,Msg,Status,SignMsgID,LocalSignMsgID,TalkerURL,UserID,FileStatus) \
	 VALUES('" << (int)oChatRecord.m_nMSGType << "','" << oChatRecord.m_strMSGTime << "','" << (oChatRecord.m_nIsMyself == false ? 0 : 1) \
		<< "','" << oChatRecord.m_strMSG << "','" << (int)oChatRecord.m_nMSGStatus << "','" \
		<< oChatRecord.m_strSignMsgID  << "','" << oChatRecord.m_strLocalSignMsgID << "','" << oChatRecord.m_strTalkerURL <<  "',"<<oChatRecord.m_nUserID << "," << (int)oChatRecord.m_enumFileStatus <<")";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	return bRet;
}
bool PortSQL::deleteOneCallRecord(long lChatRecord)
{
	if (!m_pSQLDB)
	{
		return false;
	}
	std::stringstream ssSql;
	ssSql << "delete   from  " << T_CHAT_RECORD << " where MSG = " << lChatRecord <<" and( TYPE="<<(int)ENUM_MSG_AUDIO<<" or TYPE = "<< (int)ENUM_MSG_VIDEO<<");";
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::saveOneCallRecord(CChatRecord oChatRecord,int &nRecordPrimaryKeyID)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:insert into T_CHAT_RECORD (TYPE,DATETIME,IsMyself,Msg,Status,TalkerURL) values('1',datetime('now'),'0','121212121','0','4','555@sip.com');
	*/
	ssSql << "INSERT INTO " << T_CHAT_RECORD << " (TYPE,DATETIME,IsMyself,Msg,Status,TalkerURL,UserID,FileStatus) \
	 VALUES('" << (int)oChatRecord.m_nMSGType << "'," << "  datetime('now','localtime')" << ",'" << (oChatRecord.m_nIsMyself == false ? 0 : 1) \
		<< "','" << oChatRecord.m_strMSG << "','" << (int)oChatRecord.m_nMSGStatus \
		<< "','" << oChatRecord.m_strTalkerURL << "',"<<oChatRecord.m_nUserID << "," << (int)oChatRecord.m_enumFileStatus <<")";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	if (bRet == true)
	{
		nRecordPrimaryKeyID = GetPrimaryKeyIDWhenInsert(T_CHAT_RECORD);
	}
	return bRet;
}
std::vector<CChatRecord> PortSQL::searchUnReadRecordByContact(std::string strSipURL,int nUserID)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL)<<"' and UserID ="<<nUserID<<" and Status = "<< (int)ENUM_MSG_STATUS_UNREAD;
	
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}
std::string PortSQL::GetLastUnReadMsgID(std::string strSipURL,int nUserID)
{
	std::string strLastUnReadMsgID = "";
	if (!strSipURL.empty())
	{
		std::stringstream ssSql;
		ssSql << "select SignMsgID from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL) <<"' and UserID = "<<nUserID  << " and SignMsgID is not null order by SignMsgID desc limit 1;";
		OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
		string errnoMessage;
		char **dbResult;
		int nRow = 0, nColumn = 0, nIndex = 0;
		if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
		{
			nIndex = nColumn;
			for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
			{
				long long nOffLineMsgID = 0;
				std::string  theLocalChatRecordID;
				for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
				{
					nOffLineMsgID = ParseOffLineMsgID(theLocalChatRecordID, dbResult[nIndex], dbResult[nColumnIndex]);
					nIndex++;
				}
				std::stringstream ssLastUnReadMsgID;
				ssLastUnReadMsgID << nOffLineMsgID;
				strLastUnReadMsgID = ssLastUnReadMsgID.str();
			}
			if (nRow==0)
			{
				strLastUnReadMsgID = "0";
			}
		}
		else
		{
			strLastUnReadMsgID = "0";
		}
		sqlite3_free_table(dbResult);

	}
	else
	{
		strLastUnReadMsgID = "0";
	}
	return strLastUnReadMsgID;
}
std::vector<long long > PortSQL::getOffLineMsgBySipNumber(std::string strSipURL,  int nUserID)
{
	std::vector<long long > vecOffLineMsgID;
	if (!strSipURL.empty())
	{
		std::stringstream ssSql;
		/*EG:
		select SignMsgID from T_CHAT_RECORD where TalkerURL= '120@sip.com' and  Status = 4 and SignMsgID is not null
		*/
		ssSql << "select SignMsgID from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL) << "' and Status = " << (int)ENUM_MSG_STATUS_UNREAD<<" and UserID="<<nUserID<<" and SignMsgID is not null;";
		OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
		string errnoMessage;
		char **dbResult;
		int nRow = 0, nColumn = 0, nIndex = 0;
		if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
		{
			nIndex = nColumn;
			for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
			{
				long long nOffLineMsgID = 0;
				std::string  theLocalChatRecordID;
				for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
				{
					nOffLineMsgID = ParseOffLineMsgID(theLocalChatRecordID, dbResult[nIndex], dbResult[nColumnIndex]);
					nIndex++;
				}
				vecOffLineMsgID.push_back(nOffLineMsgID);
			}
		}
		sqlite3_free_table(dbResult);
	}
	return vecOffLineMsgID;
}
std::vector<CChatRecord> PortSQL::searchRecordByContact(std::string strSipURL, int nUserID)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL)<<"' and UserID="<<nUserID<<";";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}

std::vector<CChatRecord> PortSQL::searchRecordByContact(std::string strSipURL, int nRecordCount, int nUserID)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}

	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL)<<"' and UserID= "<<nUserID<<" order by DATETIME desc " << " limit "<<nRecordCount<<";";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}

std::vector<CChatRecord> PortSQL::searchRecordForwardByContact(int nMsgID, std::string strSipURL, int nRecordCount, int nUserID)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}
	/*
	*EG:select * from T_CHAT_RECORD where TalkerURL ='1001@sipiw.com' and MsgID<2653 and TYPE <>2 and TYPE <>2  order by DATETIME desc limit 20
	*/
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL) <<"' and MsgID<"<<nMsgID \
		 <<" and UserID ="<<nUserID<< " order by  DATETIME desc" << " limit " << nRecordCount<<";";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}
bool PortSQL::searchRecordForwardMoreMsg(int nMsgID, std::string strSipURL, int nUserID)
{
	if (!m_pSQLDB)
	{
		return false;
	}
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where TalkerURL ='" << (strSipURL) << "' and MsgID<" << nMsgID \
		<< " and UserID =" << nUserID << " and TYPE <>" << ENUM_MSG_VIDEO << " and TYPE <>" << ENUM_MSG_AUDIO << " order by  DATETIME desc" << " limit 1;";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			return true;
		}
	}
	sqlite3_free_table(dbResult);
	return false;
}
CChatRecord PortSQL::searchRecordBySignMsgID(int nSignMsgID, bool &bFind)
{
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where SignMsgID ='" << (nSignMsgID) << "';";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	CChatRecord theLocalChatRecord;

	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
				bFind = true;
			}
		}
	}
	sqlite3_free_table(dbResult);
	return theLocalChatRecord;
}

CChatRecord PortSQL::searchRecordByLocalSignMsgID(int nLocalSignMsgID, bool &bFind)
{
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where LocalSignMsgID ='" << (nLocalSignMsgID) << "';";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	CChatRecord theLocalChatRecord;

	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
				bFind = true;
			}
		}
	}
	sqlite3_free_table(dbResult);
	return theLocalChatRecord;
}
void PortSQL::updateSignMsgID(int nMsgID, long lSignMsgID)
{
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET LocalSignMsgID = '" << lSignMsgID << "' where MsgID = " << nMsgID<<";";

	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);

}
CChatRecord PortSQL::searchRecordByCallHistoryID(int nCallHistoryID, bool &bFind)
{
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where  (Type=1 or Type=2) and Msg='" << (nCallHistoryID) << "' ;";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	CChatRecord theLocalChatRecord;

	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
				bFind = true;
			}
		}
	}
	sqlite3_free_table(dbResult);
	return theLocalChatRecord;
}
CChatRecord PortSQL::searchRecordByMsgID(int nMsgID, bool &bFind)
{
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << " where MsgID ='" << (nMsgID) << "';";
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	CChatRecord theLocalChatRecord;

	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
				bFind = true;
			}
		}
	}
	sqlite3_free_table(dbResult);
	return theLocalChatRecord;
}
std::vector<CChatRecord> PortSQL::searchAllRecord(int nContactSize)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}
	//查找字段所有不同的数据 最大支持多少条数据 nContactSize
	//select * from stu_info1 where s_score in (select s_score from stu_info1 group by s_score having count(1)=1)
	std::stringstream ssSql;
	ssSql << "select * from " << T_CHAT_RECORD << "where TalkerURL in   (select s_score from " << T_CHAT_RECORD<<"group by TalkerURL having count(1)=1)"<<"limit  "<<(nContactSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}

std::vector<CChatRecord> PortSQL::searchMsg(std::string strKey, int nUserID)
{
	std::vector<CChatRecord> theVecLocalChartRecord;
	if (!m_pSQLDB)
	{
		return theVecLocalChartRecord;
	}
	//select * from stu_info1 where s_score in (select s_score from stu_info1 group by s_score having count(1)=1)
	std::stringstream ssSql;
	ssSql << "select (IsMyself,MsgID,Msg) from " << T_CHAT_RECORD << "where Msg like "<< strKey <<" in   (select (IsMyself,MsgID，Msg) from " << T_CHAT_RECORD << "limit )" <<MAX_SEARCH_CHAT_MSG;
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CChatRecord theLocalChatRecord;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseChatRecord(theLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			theVecLocalChartRecord.push_back(theLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return theVecLocalChartRecord;
}

bool PortSQL::updateStausByMsgID(ENUM_MSG_STATUS enumMsgStatus, int nMsgID)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_CHAT_RECORD SET Status = 2 WHERE MsgID = 1;
	*/
	ssSql << "UPDATE" << T_CHAT_RECORD << " SET Status = "<< enumMsgStatus<<"where MsgID = " << nMsgID;


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	
	return bRet;
}

bool PortSQL::UpdateStatusBySignMsgID(ENUM_MSG_STATUS enumMsgStatus, long nSignMsgID)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_CHAT_RECORD SET Status = 2 WHERE SignMsgID = 1;
	*/
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET Status = " << enumMsgStatus << " where LocalSignMsgID = '" << nSignMsgID<<"';";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}

bool PortSQL::UpdateServerMSGIDBySignMsgID(long lSignMsgID, string strServerMSGID)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_CHAT_RECORD SET Status = 2 WHERE SignMsgID = 1;
	*/
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET SignMsgID = " << strServerMSGID << " where LocalSignMsgID = '" << lSignMsgID << "';";


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}
bool PortSQL::updateStatusBySipNumber(ENUM_MSG_STATUS enumMsgStatus, std::string strSipURL)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_CHAT_RECORD SET Status = 2 WHERE MsgID = 1;
	*/
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET Status = " << enumMsgStatus << " where TalkerURL = '" << strSipURL <<"'  and Status =="<< ENUM_MSG_STATUS_UNREAD;


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}
long long  PortSQL::ParseOffLineMsgID(string strOffLineMsgID, char * columnValue, char * columnName)
{

	if (columnValue == NULL)
	{
		return -1;
	}
	string name = columnName;
	string tempName = "SignMsgID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		strOffLineMsgID = (columnValue);
		long long nOffLineMsgID = atoll(strOffLineMsgID.c_str());
		return nOffLineMsgID;
	}
	
}

void PortSQL::ParseChatRecord(CChatRecord& oChatRecord, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "MsgID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_nMsgID = atoi(columnValue);
	}
	tempName = "TYPE";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_nMSGType = (ENUM_MSG_TYPE) atoi(columnValue);
	}
	tempName = "DATETIME";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strMSGTime = (columnValue);
	}
	tempName = "IsMyself";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_nIsMyself = atoi(columnValue);
	}
	tempName = "Msg";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strMSG= (columnValue);
	}
	tempName = "TalkerURL";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strTalkerURL = columnValue;
	}
	
	tempName = "TalkerType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		//oChatRecord.m_strMobilePhone = columnValue;
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strReserved = columnValue;
	}
	tempName = "Status";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_nMSGStatus = (ENUM_MSG_STATUS)atoi(columnValue);
	}
	tempName = "SendLocalFilePath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strSendLocalFilePath = columnValue;
	}
	tempName = "SendLocalFileName";
	if (PortUtility::compareStringNoCase(name,tempName) ==0)
	{
		oChatRecord.m_strSendLocalFileName = columnValue;
	}
	tempName = "UserID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_nUserID = atoi(columnValue);
	}
	tempName = "FileStatus";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_enumFileStatus = (ENUM_FILE_STATUS)atoi(columnValue);
	}
	tempName = "PlayTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strPlayTime = columnValue;
	}
	tempName = "SignMsgID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oChatRecord.m_strSignMsgID = columnValue;
	}
}
void PortSQL::UpdateRecordFileStatus(long lMsgID, ENUM_FILE_STATUS enumFileStatus)
{
	std::stringstream ssSql;
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET FileStatus = " << (int)enumFileStatus \
		<< " where MsgID = " << lMsgID;


	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::UpdateRecordInfo(long lMsgID, CChatRecord oUpdateInfo)
{
	//以下字段全部需要重置更新
	//m_strMSG strPlayTime m_strLocalSignMsgID
	std::stringstream ssSql;
	ssSql << "UPDATE " << T_CHAT_RECORD << " SET Msg =' " << oUpdateInfo.m_strMSG \
		<<"' , PlayTime = '"<<oUpdateInfo.m_strPlayTime\
		<<"' , LocalSignMsgID = '"<<oUpdateInfo.m_strLocalSignMsgID\
		<< "' where MsgID = " << lMsgID;


	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
std::vector<CContactDB*> PortSQL::getRecentContactByChatHistory(int nContactSize,int nUserID)
{
	
	std::vector<CContactDB*> vecCContactDB;
	if (!m_pSQLDB)
	{
		return vecCContactDB;
	}
	////eg:SELECT * FROM T_CONTACTS WHERE SipNumber = (select SenderURL from T_CHAT_RECORD group by SenderURL) limit 10
	//select * from stu_info1 where s_score in (select s_score from stu_info1 group by s_score having count(1)=1)
	std::stringstream ssSql;
	ssSql << "select * from " << T_CONTACTS << " where SipNumber =   (select SenderURL from " << T_CHAT_RECORD << " group by SenderURL)" << " and UserID =" << nUserID << " limit  " << (nContactSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CContactDB* pLocalChatRecord = new CContactDB();
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseContact(pLocalChatRecord, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			vecCContactDB.push_back(pLocalChatRecord);
		}
	}
	sqlite3_free_table(dbResult);
	return vecCContactDB;
}

std::vector<TagRecentContactInfo> PortSQL::getRecentContactSipNumberByChatHistory(int nContactSize, int  nUserID)
{
	std::vector<TagRecentContactInfo> vecRecentContactInfo;
	if (!m_pSQLDB)
	{
		return vecRecentContactInfo;
	}
	//eg:SELECT * FROM T_CONTACTS WHERE SipNumber = (select SenderURL from T_CHAT_RECORD group by SenderURL)  order by DATETIME DESC limit 10
	//select * from stu_info1 where s_score in (select s_score from stu_info1 group by s_score having count(1)=1)
	std::stringstream ssSql;
	ssSql << "select max(DATETIME) DATETIME,isMyself, TalkerURL from " << T_CHAT_RECORD <<" where UserID ="<< nUserID <<" group by TalkerURL  " << " limit  " << (nContactSize);
	OutputDebugString(PortUtility::string2WString(ssSql.str()).c_str());
	string errnoMessage;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			std::string strSipNumber ;
			TagRecentContactInfo stuRecentContact;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseSipNumber(stuRecentContact, dbResult[nIndex], dbResult[nColumnIndex]);
				nIndex++;
			}
			vecRecentContactInfo.push_back(stuRecentContact);
		}
	}
	sqlite3_free_table(dbResult);

	
	return vecRecentContactInfo;
}

void PortSQL::ParseSipNumber(TagRecentContactInfo & stuRecentContactInfo, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;

	string tempName = "TalkerURL";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		stuRecentContactInfo.m_strTalkerURL = (columnValue);
	}
	tempName = "IsMyself";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		stuRecentContactInfo.m_bIsMyself =(bool)atoi (columnValue);
	}
	
	 tempName = "DATETIME";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		stuRecentContactInfo.m_strDateTime = (columnValue);
	}

}

bool PortSQL::updateAVDeviceOptions(COptions* pOptions)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_OPTIONS  SET  AudioInputDeviceID = 1, AudioOutputDeviceID = 1, VideoDeviceID = 1  where ID = 1
	*/
	ssSql << "UPDATE " << T_OPTIONS << " SET AudioInputDeviceID = " << pOptions->m_nAudioInputDeviceID \
		<< " ,  AudioOutputDeviceID = " << pOptions->m_nAudioOutputDeviceID \
		<< " ,  VideoDeviceID = " << pOptions->m_nVideoDeviceID \
		<< " where ID = " << pOptions->m_nOptionsID;


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}

bool PortSQL::updateAudioDeviceOptions(COptions* pOptions)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	/*
	*EG:UPDATE T_OPTIONS  SET  AudioInputDeviceID = 1, AudioOutputDeviceID = 1 where ID = 1
	*/
	ssSql << "UPDATE " << T_OPTIONS << " SET AudioInputDeviceID = " << pOptions->m_nAudioInputDeviceID \
		<< " ,  AudioOutputDeviceID = " << pOptions->m_nAudioOutputDeviceID \
		<< " where ID = " << pOptions->m_nOptionsID;


	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}
void PortSQL::ParaseAVCodec(CAVCodec &oAVCodec, char * columnValue, char * columnName,ENUM_AV_CODEC enumAVCodec)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oAVCodec.m_nAVCodecID = atoi(columnValue);
	}
	tempName = "ItemIndex";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oAVCodec.m_nItemIndex = atoi(columnValue);
	}
	tempName = "CodecCode";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oAVCodec.m_nCodecCode = atoi(columnValue);
	}
	tempName = "CodecType";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		switch (enumAVCodec)
		{
		case ENUM_VA_CODEC_AUDIO:
			oAVCodec.m_nAVCodecType = (ENUM_AV_CODEC)atoi(columnValue);
			break;
		case  ENUM_VA_CODEC_VIDEO:
			oAVCodec.m_nAVCodecType = (ENUM_AV_CODEC)atoi(columnValue);
			break;
		}
	}
	tempName = "CodecOpen";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oAVCodec.m_bAVCodecOpen = (bool)atoi(columnValue);
	}
	tempName = "CodecName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		oAVCodec.m_strCodecName = (columnValue);
	}
}
std::vector<CAVCodec> PortSQL::GetCodec(ENUM_AV_CODEC enumAVCodec)
{
	std::vector<CAVCodec> vecCodec;

	std::stringstream ssSql;
	ssSql << "select * from " << T_AV_CODEC << " where CodecType = " << (int)enumAVCodec;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CAVCodec itemCodec ;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParaseAVCodec(itemCodec, dbResult[nIndex], dbResult[nColumnIndex], enumAVCodec);
				++nIndex;
			}
			vecCodec.push_back(itemCodec);
		}
	}
	sqlite3_free_table(dbResult);
	return vecCodec;
}

void PortSQL::UpdateCodecInfo(int nItemIndex, bool bOpen, int nCodecID)
{
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	
	ssSql << "UPDATE " << T_AV_CODEC << " SET ItemIndex = " << nItemIndex \
		<< " ,  CodecOpen = " << (int)bOpen \
		<< " where ID = " << nCodecID;


	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::updateOptions(COptions *pOptions)
{
	bool bRet = false;
	string serializedAccount;
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	
	/*
	*EG:UPDATE T_OPTIONS  SET  AudioInputDeviceID = 1, AudioOutputDeviceID = 1 where ID = 1
	*/
	ssSql << "UPDATE " << T_OPTIONS << " SET AGC = " << (int)pOptions->m_bAGC \
		<< " ,  AEC = " << (int)pOptions->m_bAEC \
		<< " ,  VAD = " << (int)pOptions->m_bVAD \
		<< " ,  CNG = " << (int)pOptions->m_bCNG \
		<< " ,  KeepLive = " << (int)pOptions->m_nKeepAlive \
		<< " ,  VideoImageQualityLevel = " << pOptions->m_nVideoImageQualityLevel \
		<< " ,  ForwardTo = '" << pOptions->m_strForwardTo \
		<< "' ,  BusyForwardTo = '" << pOptions->m_strBusyForwardTo \
		<< "' ,  NoAnswerFowardTo = '" << pOptions->m_strNoAnswerFowardTo \
		<< "' ,  NoAnswerWaitSeconds = " << pOptions->m_nNoAnswerWaitSeconds \
		<< " ,  AudioRtpMinPort = " << (int)pOptions->m_nAudioRtpMaxPort \
		<< " ,  AudioRtpMaxPort = " << (int)pOptions->m_nAudioRtpMaxPort \
		<< " ,  VideoRtpMinPort = " << (int)pOptions->m_nVideoRtpMinPort \
		<< " ,  VideoRtpMaxPort = " << (int)pOptions->m_nVideoRtpMaxPort \
		<< " ,  Language = " << pOptions->m_nLanguage \
		<< " ,  SaveLanguage =" << (int)pOptions->m_bSaveLanguage\
		<< " ,  EnableMwi = " <<(int) pOptions->m_bEnableMWI \
		<< " ,  RtpKeepLive = " << (int)pOptions->m_bRTPKeepLive \
		<< " ,  LocalVideoInRoom = " << (int)pOptions->m_bLocalVideoInRoom \
		<< " ,  VideoResolution = " << (int)pOptions->m_nVideoResolution \
		<< " ,  SessionTimerTime = " << (int)pOptions->m_nSessionTimerTime \
		<< " ,  VideoServiceType = " << (int)pOptions->m_nVideoServiceType \
		<< " ,  RtpKeepAlivePayload = " << (int)pOptions->m_nRtpKeepAlivePayload \
		<< " ,  defaultSoftphone = " << (int)pOptions->m_bDefaultSoftphone \
		<< " ,  displayidle = " << (int)pOptions->m_bDisplayidle \
		<< " ,  showTimeStamp = " << (int)pOptions->m_bShowTimeStamp \
		<< " ,  recordFolder = '" << pOptions->m_strRecordFolder \
		<< "' ,  ANS = " << (int)pOptions->m_bANS \
		<< " ,  EAQ = " << (int)pOptions->m_bEAQ \
		<< " ,  EVQ = " << (int)pOptions->m_bEVQ \
		<< " ,  EnableSessionTimer = " << (int)pOptions->m_bEnableSessionTimer \
		<< " ,  EnableForword = " << (int)pOptions->m_bEnableForword \
		<< " ,  EnableBusyForword = " << (int)pOptions->m_bEnableOnPhoneForword \
		<< " where ID = " << pOptions->m_nOptionsID;

	string errnoMessage;
	bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);

	return bRet;
}


void PortSQL::SaveOneAVRecord(CVARecord oVARecord)
{
	std::stringstream ssSql;
	ssSql.clear();
	ssSql.str("");
	std::string strFileFormat;
	switch (oVARecord.m_enumVARecordType)
	{
	case ENUM_VA_RECORD_AUDIO:
		strFileFormat = ".wav";
		break;
	case ENUM_VA_RECORD_VIDEO:
		strFileFormat = ".avi";
		break;
	default:
		break;
	}
	ssSql << "INSERT INTO " << T_VEDIO_AUDIO_RECORD << " (FilePath,EndTime,Type,SIPNUM,UserID,PlayTime) \
	 VALUES('" << oVARecord.m_strFileName+ strFileFormat << "'," << "  datetime('now','localtime')" << ",'" <<(int) oVARecord.m_enumVARecordType \
		<< "','" << oVARecord.m_strSIPNum << "',"<< oVARecord.m_nUserID <<",'"<< oVARecord.m_strPlayTime <<"');";
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}

std::vector<CVARecord*> PortSQL::GetPageAVRecord(int nUserID ,int nPageNum,int nMaxNum)
{
	int MsgSize = nPageNum * MAX_CALL_VIEW_NUM;
	vector<CVARecord*> vecVARecord;
	std::stringstream ssSql;
	//ssSql << "select * from " << T_VEDIO_AUDIO_RECORD << " order by EndTime desc " << " limit " << nPageNum - 1 << "*" << nMaxNum << "," << nMaxNum;
	ssSql << "select * from " << T_VEDIO_AUDIO_RECORD<<" where UserID = "<<nUserID << " order by EndTime desc " << " limit " << MsgSize << ";" ;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CVARecord *pLocalVARecord = new CVARecord();
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseRecord(pLocalVARecord, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			vecVARecord.push_back(pLocalVARecord);
		}
		sqlite3_free_table(dbResult);
	}

	return vecVARecord;
}
void PortSQL::DeleteRecord(long long  nID)
{
	if (!m_pSQLDB)
	{
		return;
	}
	std::stringstream ssSql;
	ssSql << "delete  * from  " << T_VEDIO_AUDIO_RECORD << "where ID = " << nID;
	string errnoMessage;
	exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::ParseRecord(CVARecord* pVARecord, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_nCallHistoryID = atol(columnValue);
	}
	tempName = "FilePath";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_strFileName = (columnValue);
	}
	tempName = "BeginTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_strBeginTime = (columnValue);
	}
	tempName = "EndTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_strEndTime = (columnValue);
	}
	tempName = "Type";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_enumVARecordType = (ENUM_VA_RECORD)atoi( columnValue);
	}
	tempName = "SIPNUM";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_strSIPNum = (columnValue);
	}
	tempName = "PlayTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pVARecord->m_strPlayTime = columnValue;
	}
}

bool PortSQL::CreateGroup( TAG_RECENT *pContactInfo)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
	if (localRecentContactType != ENUM_MULTI_CONTACT)
	{
		return false;
	}
	CTalkGroup *pChartGroup = pContactInfo->m_unionContactInfo.m_pChartGroup;
	std::stringstream ssSql;
	
	ssSql << "INSERT  INTO  " << T_TALK_GROUP << "( UserID,ChatGroupName,DateTime) VALUES(" \
		<<pChartGroup->m_nUserID << ",'" << pChartGroup->m_strChatGroupName << "' , datetime('now','localtime'))";
	string errnoMessage;

	bool bRet = exec(ssSql.str(), NULL, NULL, errnoMessage);
	if (bRet )
	{
		pChartGroup->m_nChatGroupID = GetPrimaryKeyIDWhenInsert(T_TALK_GROUP);
		int nMemberCount = pChartGroup->GetMemberCount();
		for (int nIndex = 0;nIndex< nMemberCount;nIndex++)
		{
			CTalkGroupNumber *pTalkGroupNumber = pChartGroup->GetMember(nIndex);
			pTalkGroupNumber->m_nTalkGroupID = pChartGroup->m_nChatGroupID;
			if (pTalkGroupNumber!=NULL)
			{
				 bRet = AddContact2Group(pContactInfo, pTalkGroupNumber);
				 if (bRet==false)
				 {
					 break;
				 }
			}
		}
	}
	return bRet;
}
bool PortSQL::AddContact2Group(TAG_RECENT *pContactInfo, CTalkGroupNumber* pGroupNumber)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	ENUM_RECENT_CONTACT_TYPE  localRecentContactType = pContactInfo->m_enumRecentContact;
	if (localRecentContactType != ENUM_MULTI_CONTACT)
	{
		return false;
	}
	CTalkGroup *pChartGroup = pContactInfo->m_unionContactInfo.m_pChartGroup;
	std::stringstream ssSql;
	
	
	/*
	*EG:insert into T_GROUP(GroupID, SipURL, IsManager, IsCreator,DispalyName,Favorite,Disable) values(1, '555@SIP.COM', 0,0,0, '888888', 1,1);
	*/
	ssSql << "INSERT INTO   " << T_TALK_GROUP_NUMBER << "(TalkGroupID, SipNumber,DispalyName, IsManager, IsCreator,Favorite,Disable) values("
		<< pChartGroup->m_nChatGroupID << ",'" << pGroupNumber->m_strSIPNumber << "','"<<pGroupNumber->m_strSelfDisplayName<<"',"<<(int)pGroupNumber->m_bIsManager<<","\
		<< (int)pGroupNumber->m_bIsCreator<< "," << (int)pGroupNumber->m_bFavorite<< "," << (int)pGroupNumber->m_bIsDisable<<");";
	string errnoMessage;

	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
bool PortSQL::UpdateGroupRecentTime(int nTalkGroupID)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;

	ssSql << "update  " << T_TALK_GROUP << " set DateTime = ' datetime('now','localtime')'" \
		<< " where TalkGroupID = " << nTalkGroupID;
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}

vector<TAG_RECENT*> PortSQL::getRecentGroup(int nRecordSize, int nUserID)
{
	std::vector<TAG_RECENT*> vecTalkGroup;
	vecTalkGroup.clear();

	std::stringstream ssSql;
	ssSql << "select * from " << T_TALK_GROUP <<" where UserID = "<<nUserID<< " limit " << nRecordSize;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			TAG_RECENT *pContactInfo = new TAG_RECENT;
			CTalkGroup *pChatGroup = new CTalkGroup;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParseGroup(pChatGroup, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			auto vecTlakGroupNumber = GetTalkGroupNumber(pChatGroup->m_nChatGroupID);
			auto iterTalkGroupNumber = vecTlakGroupNumber.begin();
			for (iterTalkGroupNumber;iterTalkGroupNumber!=vecTlakGroupNumber.end();iterTalkGroupNumber++)
			{
				CTalkGroupNumber *pTalkGroupNumber = *iterTalkGroupNumber;
				TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(pTalkGroupNumber->m_strSIPNumber);
				if (pContactInfo==NULL)
				{
					pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(pTalkGroupNumber->m_strSIPNumber);
				}
				CContactDB *pContactDB = pContactInfo->m_unionContactInfo.m_pContact;
				pTalkGroupNumber->SetContactDB(pContactDB);
				pChatGroup->AddContact2Group(pTalkGroupNumber);

			}
			pContactInfo->m_enumRecentContact = ENUM_MULTI_CONTACT;
			pContactInfo->m_unionContactInfo.m_pChartGroup = pChatGroup;

			vecTalkGroup.push_back(pContactInfo);
		}
	}
	sqlite3_free_table(dbResult);
	return vecTalkGroup;
}
vector<CTalkGroupNumber*>PortSQL::GetTalkGroupNumber(int nTalkGroupID)
{
	std::vector<CTalkGroupNumber*> vecGroupNumber;

	std::stringstream ssSql;
	ssSql << "select * from " << T_TALK_GROUP_NUMBER << " where TalkGroupID = " << nTalkGroupID ;
	char **dbResult;
	int nRow = 0, nColumn = 0, nIndex = 0;
	string errnoMessage;
	if (exec(ssSql.str(), &dbResult, &nRow, &nColumn, errnoMessage) == true)
	{
		nIndex = nColumn;
		for (int nRowIndex = 0; nRowIndex < nRow; nRowIndex++)
		{
			CTalkGroupNumber *pChatGroupNumber = new CTalkGroupNumber;
			for (int nColumnIndex = 0; nColumnIndex < nColumn; nColumnIndex++)
			{
				ParaseGroupNumber(pChatGroupNumber, dbResult[nIndex], dbResult[nColumnIndex]);
				++nIndex;
			}
			vecGroupNumber.push_back(pChatGroupNumber);
		}
	}
	sqlite3_free_table(dbResult);
	return vecGroupNumber;
}
void PortSQL::ParaseGroupNumber(CTalkGroupNumber *pTalkGroupNumber, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "TalkGroupID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_nTalkGroupID = atol(columnValue);
	}
	tempName = "SipNumber";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_strSIPNumber = (columnValue);
	}
	tempName = "IsManager";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_bIsManager = (bool) atoi(columnValue);
	}
	tempName = "IsCreator";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_bIsCreator = (bool)atoi(columnValue);
	}
	tempName = "DispalyName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_strSelfDisplayName = columnValue;
	}
	tempName = "Favorite";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_bFavorite = (bool)atoi(columnValue);
	}
	tempName = "Disable";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pTalkGroupNumber->m_bIsDisable = (bool)atoi(columnValue);
	}

}
bool PortSQL::UpdateGroupName(int m_nChatGroupID, string strGroupName)
{
	if (!m_pSQLDB)
	{
		return  false;
	}
	std::stringstream ssSql;

	ssSql << "update  " << T_TALK_GROUP << " set ChatGroupName ='" << strGroupName\
		<< "' where ID = " << m_nChatGroupID;
	string errnoMessage;
	return exec(ssSql.str(), NULL, NULL, errnoMessage);
}
void PortSQL::ParseGroup(CTalkGroup *pChatGroup, char * columnValue, char * columnName)
{
	if (columnValue == NULL)
	{
		return;
	}
	string name = columnName;
	string tempName = "ID";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pChatGroup->m_nChatGroupID = atol(columnValue);
	}
	tempName = "ChatGroupName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pChatGroup->m_strChatGroupName = (columnValue);
	}
	tempName = "SelfDisplayName";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pChatGroup->m_strSelfDisplayName = (columnValue);
	}
	tempName = "DateTime";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pChatGroup->m_strRecentDateTime = (columnValue);
	}
	tempName = "Reserved";
	if (PortUtility::compareStringNoCase(name, tempName) == 0)
	{
		pChatGroup->m_strReserved = columnValue;
	}
}
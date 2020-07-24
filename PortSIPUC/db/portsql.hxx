/*
    PortSIP
    Copyright (C) 2007 PortSIP Solutions, Inc.
   
    support@portsip.com

    Visit us at http://www.portsip.com
*/

#ifndef PORTSIP_PORTSQL_hxx
#define PORTSIP_PORTSQL_hxx
#include <string>
#include <list>
#include "sqlite3.h"
#include "User.h"
#include "Options.h"
#include "CallHistory.h"
#include "TalkGroup.h"
#include "TalkGroupNumber.h"
#include "ChatRecord.h"
#include "Contact.h"
#include "VARecord.h"
#include "VoiceMail.h"
#include "DBParam.h"
#include "AVCodec.h"

#define MAX_SEARCH_CHAT_MSG 500
typedef int (*portsql_callback)(void*, int, char**, char**);
//TODO: 查询返回的结果集部分是new出来的指针，是没有释放的，到时候优化的时候修改成结构体对象
class PortSQL
{
public:
	PortSQL();
	virtual ~PortSQL();

public:
	bool openDB(const std::string & dbPathName);
	void closeDB();
	bool isConnected() {
		return m_bIsConnected;
	};
	//获取当前Insert时产生的自增主键ID
	int GetPrimaryKeyIDWhenInsert(const char *szTable);

	//语音邮件
	/************************************************************************/
	/* T_VOICE_MAIL                                                         */
	/************************************************************************/
	int GetVoiceMailCount();
	const std::vector<CVoiceMail> GetVoiceMail(int nUserID,int nPageNum,int nMaxNum = PAGE_SIZE_COMMON);
	const std::vector<CVoiceMail*> GetVoiceMailDownLoadSuccess(int nUserID,int nPageNum, int nMaxNum = PAGE_SIZE_COMMON);
	bool AddOneVoiceMail(CVoiceMail oVoiceMail);
	bool UpdateOneVoiceMailDownLoadSuccess(long long nID);
	bool IsExistVoiceInfo(long long nID);
	bool IsDownLoad(long long nID);
	void ParseVoiceMail(CVoiceMail& oVoiceMail, char * columnValue, char * columnName);
	void ParseVoiceMail(CVoiceMail *pVoiceMail, char * columnValue, char * columnName);
	void DeleteVoiceMail(long long nID);
	CVoiceMail * GetVoiceMailByID(long long nID);
	void updateVoiceMailStatus();
	/***************************************************T_USER****************************************************************/
	bool deleteAccount(int accountID);
	bool addAccount(CUser* objUser, std::string &strErrorMsg);
	const CUser* getActiveAccount(bool& bReslut);
	std::list< CUser*> getAllAccount();
	int  saveActiveAccount(CUser* account);
	bool updateAccount(CUser* objUser);

	/**************************************************T_OPNTIONS*************************************************************/
	void getOptions(COptions *pOptions,bool& bReslut);
	void ParseOptions(COptions *pOptions, char * columnValue, char * columnName);
	bool updateAVDeviceOptions(COptions *pOptions);
	bool updateAudioDeviceOptions(COptions *pOptions);
	bool updateOptions(COptions *pOptions);
	/************************************************T_CALL_HISTORY***********************************************************/
	bool saveCallMsg(CCallHistory oCallHistory,int &nRecordPrimaryKeyID);
	bool updateCallMsg(CCallHistory oCallHistory,LONG lDataTime);
	std::vector<CCallHistory> getCallMsg(int nCallMsgSize);
	std::vector<CCallHistory> getCallMsg(int nCallMsgSize,int nCallerID,bool bConnected);
	bool GetCallMsgByID(long long nCallHistoryID, CCallHistory &theLocalCallHistory);
	std::vector<CCallHistory> GetAllCallMsg(int nMaxPage,int nUserID);
	std::vector<CCallHistory> GetMissedCallMsg(int nMaxPage, int nUserID);
	void ParseCallHistory(CCallHistory& oCallHistory, char * columnValue, char * columnName);
	bool UpdateCallMsgEndTimeByID(long nCallHistoryPrimaryKeyID);
	bool UpdateCallMsgStatus( long nCallHistoryPrimaryKeyID, ENUM_CALL_STATUS enumCallStatus);
	void DeleteCallMsg(long long nCallHistoryID);
	void updateMissReadStatus();
	/************************************************T_CONTACTS**************************************************************/
	bool addOneContact(CContactDB* pContact, string &errnoMessage);
	bool updateContactFavorite(CContactDB* pContact);
	bool updateContact(CContactDB* pContact);
	bool deleteContact(std::string  strSIPNum, int nUserID);
	std::vector<CContactDB> getContact();
	bool searchContact(std::string strSipNum);
	std::vector<TAG_RECENT*> getAllContacts(int nContactSize,int nUserID);
	void ParseContact(CContactDB* pCallHistory, char * columnValue, char * columnName);
	/************************************************************************/
	/*                             T_AV_CODEC								*/
	/************************************************************************/
	std::vector<CAVCodec> GetCodec(ENUM_AV_CODEC enumAVCodec);
	void UpdateCodecInfo(int nItemIndex, bool bOpen, int nCodecID);
	void ParaseAVCodec(CAVCodec &oAVCodec, char * columnValue, char * columnName, ENUM_AV_CODEC enumAVCodec);
	/***********************************************T_ChatRecord***************************************************************/
	int  saveOneChatRecord(CChatRecord oChatRecord);
	bool saveOneOffLineChatRecord(CChatRecord oChatRecord);
	bool saveOneCallRecord(CChatRecord oChatRecord,int &nRecordPrimaryKeyID);
	bool deleteOneCallRecord(long lChatRecord);
	std::vector<CChatRecord> searchRecordByContact(std::string strSipURL, int nUserID);
	std::vector<CChatRecord> searchRecordByContact(std::string strSipURL, int nRecordCount, int nUserID);
	std::vector<CChatRecord> searchRecordForwardByContact(int nMsgID,std::string strSipURL, int nRecordCount, int nUserID);
	bool   searchRecordForwardMoreMsg(int nMsgID, std::string strSipURL, int nUserID);
	std::vector<CChatRecord> searchAllRecord(int nContactSize);
	bool updateStausByMsgID(ENUM_MSG_STATUS enumMsgStatus, int nMsgID);
	bool updateStatusBySipNumber(ENUM_MSG_STATUS enumMsgStatus, std::string strSipURL);
	void ParseChatRecord(CChatRecord & oCallHistory, char * columnValue, char * columnName);
	bool UpdateStatusBySignMsgID(ENUM_MSG_STATUS enumMsgStatus, long nSignMsgID);
	bool UpdateServerMSGIDBySignMsgID(long lSignMsgID, string strServerMSGID);
	CChatRecord searchRecordByMsgID(int nMsgID,bool &bFind);
	CChatRecord searchRecordBySignMsgID(int nSignMsgID, bool &bFind);
	CChatRecord searchRecordByLocalSignMsgID(int nLocalSignMsgID, bool &bFind);
	CChatRecord searchRecordByCallHistoryID(int nCallHistoryID, bool &bFind);
	void updateSignMsgID(int nMsgID,long lSignMsgID);
	std::vector<CChatRecord> searchUnReadRecordByContact(std::string strSipURL,int nUserID);
	std::vector<CChatRecord> searchMsg(std::string strKey,int nUserID);
	std::string GetLastUnReadMsgID(std::string strSipURL,int nUserID);
	std::vector<long long > getOffLineMsgBySipNumber(std::string strSipURL,int nUserID);
	long long  ParseOffLineMsgID(string strOffLineMsgID,char * columnValue, char * columnName);
	void UpdateRecordFileStatus(long lMsgID, ENUM_FILE_STATUS enumFileStatus);
	void UpdateRecordInfo(long lMsgID, CChatRecord oUpdateInfo);
	/***********************************************T_ChatRecord&&T_CONTACTS*********************************************/
	std::vector<CContactDB*> getRecentContactByChatHistory(int nContactSize, int nUserID);
	std::vector<TagRecentContactInfo> getRecentContactSipNumberByChatHistory(int nContactSize,int nUserID);//
	void ParseSipNumber(TagRecentContactInfo & stuRecentContactInfo,char * columnValue, char * columnName);
	/************************************************************************/
	/* T_VEDIO_AUDIO_RECORD                                                 */
	/************************************************************************/
	void SaveOneAVRecord(CVARecord oVARecord);
	std::vector<CVARecord*> GetPageAVRecord(int nUserID,int nPageNum, int nMaxNum = PAGE_SIZE_COMMON);
	void ParseRecord(CVARecord *pVARecord, char * columnValue, char * columnName);
	void DeleteRecord(long long nID);
	void updateRecordStatus();
	/************************************************************************/
	/************************************************************************/
	bool CreateGroup(TAG_RECENT *pContactInfo);
	bool AddContact2Group(TAG_RECENT *pContactInfo, CTalkGroupNumber* pGroupNumber);
	bool UpdateGroupRecentTime(int nTalkGroupID);			
	vector<TAG_RECENT*> getRecentGroup(int nRecordSize,int nUserID);				
	void ParseGroup(CTalkGroup *pVoiceMail, char * columnValue, char * columnName);
	vector<CTalkGroupNumber*> GetTalkGroupNumber(int nTalkGroupID);
	void ParaseGroupNumber(CTalkGroupNumber *pTalkGroupNumber, char * columnValue, char * columnName);
	bool UpdateGroupName(int m_nChatGroupID, string strGroupName);
public:
	bool exec(const std::string & sql, portsql_callback  callback, void * privateData, std::string & errormsg);
	bool exec(const std::string & sql, char *** result, int * row, int * column, std::string & errormsg);
private:
	void releaseResult(char ** result);

	bool tableExists(const std::string & tableName);
	std::string getStringFromColumnChar(const unsigned char *pval);
protected:

private:
	sqlite3 * m_pSQLDB;
	std::list<CUser*> m_lstAccount;
	bool			 m_bIsConnected;
};



#endif

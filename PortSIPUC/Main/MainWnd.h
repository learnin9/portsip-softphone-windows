#pragma once
#include "system/OnLineStatusMgr.h"
#include "../db/portsql.hxx"
#include "CallerBarWnd.h"
#include "../Main/InfoWndMgr.h"
#include "shobjidl_core.h"
#include "System/ConferenceStatusSubMgr.h"
enum MAINFRAME_TAB_KEY
{
	MTK_MAIN = 1,
	MTK_CALL,
	MTK_SEARCH
};
class CMainCanvasHandler;
class CMainWnd : public Window
{
public:
	CMainWnd();
	virtual ~CMainWnd();

	std::vector< CCallHistory> GetCallHistory() { return m_lstCallHistoryOK; };
	void                     UpdateContacts();

	std::vector<TAG_RECENT*> FindMemContactByVecSipInfo(vector<TagRecentContactInfo > vecSipInfo);;
	TAG_RECENT * CreateNewContactBySipNumber(std::string strSipNumber);
	TAG_RECENT * CreateNewTalkGroupByVecSContatct(vector<CContactDB*> vecContact);
	HWND GetHwndBySipNumber(std::string strSipNumber);
	HWND FindHwndByContact(TAG_RECENT* pContactDB);
	void ProcessJsonMsg(std::string strTime, std::string strMsg, std::string  strSipNumber, Json::Value & json_data,CChatRecord &oChatRecord, std::string strMsgID, CIUIString strDisPlayName);
	void ProcessPureMsg(std::string strTime, std::string strMsg, std::string  strSipNumber, CChatRecord &oChatRecord,std::string strMsgID,CIUIString strDisPlayName);
	void InsertCallEventMsg(std::string strSipNumber,CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode);
	TAG_RECENT * FindTagRecentByContact(CContactDB *pContactDB);
	TAG_RECENT * FindContactDBBySipNumber(std::string strSipNumber);
	TAG_RECENT * FindContactDBByVecSipNumber(vector<CContactDB*> vecSipNumber);
	void RefreshRecentContantOnlineStatus(CContactDB *pContact,bool &bUpdate);
	void RefreshLocalContactOnlineStatus(CContactDB *pContact);
	void RefreshLocalContactUnReadNum(std::string  strSipNumber, CChatRecord oChatRecord);
	void RefreshLocalContactInfo(TAG_RECENT *pContact);
	/************************************************************************/
	void Add2Favorite(TAG_RECENT* pContact);								
	void Add2Contact(TAG_RECENT* pContact);									
	void Add2DBContact(TAG_RECENT *pContact);							
	void Add2RemoteContact(TAG_RECENT* pContact);							
	void Add2Extension(TAG_RECENT* pContact);								
	void EraseContactFavorite(std::string strSipNumber, int nUserID);							
	void EraseContactLocal(std::string strSipNumber,int nUserID);							
	void EraseContactRemoteContact(long long nDataID);						
	void EraseContactExtension(long long nDataID);							
 
	void UpdateFavorite(TAG_RECENT* pContact);								
	void UpdateContact(TAG_RECENT* pContact);								
	void UpdateRemoteContact(TAG_RECENT* pContact);							
	void UpdateExtension(TAG_RECENT* pContact);								
	void InitFavorite();

	void DeleteContactInfo(TAG_RECENT* pContact);							
	std::vector<TAG_RECENT*> GetContacts() {
		return m_vecContact;
	}
	std::vector<TAG_RECENT*> GetPureContact();
	std::vector<TAG_RECENT*> CMainWnd::GetDBContact();
	std::vector<TAG_RECENT*> GetFavorite()
	{
		return m_vecFavoriteContact;
	}
	std::vector<TAG_RECENT*> GetRemoteContact()
	{
		return m_vecRemoteContact;
	}
	std::vector<TAG_RECENT*> GetExtension()
	{
		return m_vecExtersionContact;
	}
	int  SearchFromContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString,bool bGroupSwtich = false);
	int  SearchFromContactInfo(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString);
	void SearchContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString,bool bGroupSwtich );
	void SearchFromRemoteContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString);
	void SearchFromExtensionContact(std::vector<TAG_RECENT*> &vecContact, std::string strKeyString);
	TAG_RECENT * SearchFromExtensionContact(std::string strKeySipNumber);
	void ResetOnLineStatus(bool bOnLine);
	void ReConnect2Server();
	void ExtersionExistContactInLocal();
	void RemoteExistContactInLocal();
	//shake self
	void ShakeSelf();
	void resetContactOnlineStatusOffLine();

	HRESULT FlashInTaskBar();
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnEventCreate(RoutedEventArgs *pe);
	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnIcon(RoutedEventArgs *pe);
	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnChkMax(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnSwitchToContact(WPARAM wParam, LPARAM lParam);
	LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnSIPCallbackEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnProcessDownLoadFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnProcessUpLoadFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnUploadFileGetOk(WPARAM wParam, LPARAM lParam);
	LRESULT OnDownLoadVoiceMailOK(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateInfoWnd(WPARAM wParam, LPARAM lParam);
	LRESULT OnVideoConfenceAction(WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetIsConference(WPARAM wParam, LPARAM lParam);
	LRESULT OnAnswerByRemote(WPARAM wParam, LPARAM lParam);
	LRESULT OnUnGetConferenceStatus(WPARAM wParam,LPARAM lParam);
	void OnSIPIMCallbackEvent(ICallbackParameters * parameters);
	void OnPresenceOffline(ICallbackParameters * parameters);
	void OnPresenceOnline(ICallbackParameters * parameters);
	void OnPresenceRecvSubscribe(ICallbackParameters * parameters);
	void OnRecvOutOfDialogMessage(ICallbackParameters * parameters);
	void OnSendOutOfDialogMessageFailure(ICallbackParameters * parameters);
	void OnSendOutOfDialogMessageSuccess(ICallbackParameters * parameters);
	void onSIPCallIncoming(ICallbackParameters * parameters);
	void onSIPCallTrying(ICallbackParameters * parameters);
	void onSIPCallSessionProgress(ICallbackParameters * parameters);
	void onSIPCallRinging(ICallbackParameters * parameters);
	void onSIPCallAnswered(ICallbackParameters * parameters);
	void onSIPCallFailure(ICallbackParameters * parameters);
	void onSIPInviteUpdated(ICallbackParameters * parameters);
	void onSIPInviteConnected(ICallbackParameters * parameters);
	void onSIPCallForwarding(ICallbackParameters * parameters);
	void onSIPCallClosed(ICallbackParameters * parameters);
	void onSIPDialogStateUpdated(ICallbackParameters * parameters);
	void onSIPCallRemoteHold(ICallbackParameters * parameters);
	void onSIPCallRemoteUnhold(ICallbackParameters * parameters);
	void onSIPReceivedRefer(ICallbackParameters * parameters);
	void onSIPReferAccepted(ICallbackParameters * parameters);
	void onSIPReferRejected(ICallbackParameters * parameters);
	void onSIPRecvOptions(ICallbackParameters * parameters);
	void onSIPRecvInfo(ICallbackParameters * parameters);

	void onSIPRecvNotifyOfSubscription(ICallbackParameters * parameter);
	void onSIPSubscriptionFailure(ICallbackParameters * parameter);
	void onSIPSubscriptionTerminated(ICallbackParameters * parameter);
	void onSIPSendMessageSuccess(ICallbackParameters * parameters);
	void onSIPSendMessageFailure(ICallbackParameters * parameters);
	void onSIPPlayAudioFileFinished(ICallbackParameters * parameters);
	void onSIPPlayVideoFileFinished(ICallbackParameters * parameters);
	void onSIPReceivedDtmfTone(ICallbackParameters * parameters);
	void onSIPTransferTrying(ICallbackParameters * parameters);
	void onSIPTransferRinging(ICallbackParameters * parameters);
	void onSIPACTVTrasferFailure(ICallbackParameters * parameters);
	void onSIPACTVTransferSuccess(ICallbackParameters * parameters);
	void InitControlsData();
	void PrintJsonKeyValueError(std::string strJsonValue);
	
	LRESULT OnRestoreMainDlg(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseLocalVideoWnd(WPARAM wParam, LPARAM lParam);
	LRESULT ProcessLoginReslut(bool bLoginSuccess, std::string strErrorInfo);
	LRESULT OnCallAction(WPARAM wParam, LPARAM lParam);
	void ProcessIsConferenceConfStatus(TAG_RECENT *pContactInfo, std::string strStatusText);
public:
	int SwitchToPropertySheet(MAINFRAME_TAB_KEY ePage);
	void InsertMsg2Chat(TAG_RECENT* pContact, std::string strMsg,std::string strMsgTime,int lMsgID,CIUIString strDisPlayName);
	void RefreshMsgStatus(TAG_RECENT*pContact, long lSignMsgID,bool bSucc);
	/*InsertJsonFileMsg2Chat(pContactDB, strFileName, strMimeType, strMsgType);*/
	void InsertJsonFileMsg2DownLoadList(TAG_RECENT*pContact,std::string  strFileName,std::string strMimeType);
	void InsertJsonOffLineMsg2Chat(TAG_RECENT*pContact, std::string  strMsg);	
	void ProcessMsgText(std::string strTime,std::string strMsg,TAG_RECENT *pContact,int lMsgID);
	std::string  ProcessMsgAudio(long msgID,std::string strTime,std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact,INT64 nFileSize);
	std::string  ProcessMsgVideo(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize);
	std::string  ProcessMsgPic(long msgID, std::string strTime, std::string srFileName, std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize);
	std::string  ProcessMsgFile(long msgID, std::string strTime, std::string srFileName,std::string strHttpURL, TAG_RECENT *pContact, INT64 nFileSize);

	void UpdateContactInfo(WPARAM wParam, LPARAM lParam);
	void ProcessDownLoadFileOK(WPARAM wParam, LPARAM lParam);
	void ProcessMsgEmoji(std::string strTime, std::string srFileName, TAG_RECENT *pContact);
	void ProcessMsgOther(std::string strTime, std::string srFileName, TAG_RECENT *pContact);
	
	
	CMainCanvasHandler * TestGetMainCanvasHandler();
	
	void TestFunDownLoad();
	void TestFunUpLoad();
	void TipsFunNoRun();

	void CreatInfoWnd(LPCTSTR lpTitle, LPCTSTR lpMsgBody, HWND hParent, int nAutoCloseTime = 5000);
	void ClosedInfoWnd(CInfoWnd *);

	void  EraseItemRemoteContact();
	void  EraseItemExtension();

	void reSubContactStatus();
protected:
	Button *m_pBtnIcon;
	Label *m_pStaTitle;
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	TabControl *m_pTabMainFrame;
	Canvas *m_pCanvasModalMask;
protected:
	Button *m_pBtnCall;
	Button *m_pBtnCloseCallPage;
	Button *m_pBtnSearch;
	Button *m_pBtnCloseSearchPage;
	CMainCanvasHandler* m_pMainCanvasHandler;
	COnLineStatusMgr  m_OnLineStatusMgr;
	CInfoWndMgr			    m_oInfoMgr;
protected:
	BOOL m_bTrayLButtonDown;
	CLocalCriticalSection    m_oLockContact;
	std::vector< CCallHistory> m_lstCallHistoryOK;

	std::vector<CCallerBarWnd *> m_vecCallerBarWnd;
	std::vector<TAG_RECENT*>  m_vecFavoriteContact;
	std::vector<TAG_RECENT*>   m_vecContact;
	std::vector<TAG_RECENT*>  m_vecDBContact;
	std::vector<TAG_RECENT*>   m_vecRemoteContact;
	std::vector<TAG_RECENT*>   m_vecExtersionContact;
	CConferenceStatusMgr       m_oConferenceMgr;
	ITaskbarList3* m_pTaskBarlist;
};

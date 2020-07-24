#pragma once
#include "../../../../PopupChatDlg.h"
class CTextChatCanvasHandler;
class CVideoCanvasHandler;
class CTextChatPageCanvasHandler;
class CChatTabControlHandler : public CWLWndHandler
{
public:
	CChatTabControlHandler();
	virtual ~CChatTabControlHandler();
	CVideoCanvasHandler *GetVideoCanvasHandler()
	{
		return m_pVideoCanvasHandler;
	};
	bool GetCalling();
	void ChangeHold(bool bHold);
	void UpdateContactInfoInChat(TAG_RECENT *pContactInfo);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	int CreateTextChatControl();


	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnMsg(RoutedEventArgs *pe);
	LRESULT OnBtnHangup(RoutedEventArgs *pe);
	LRESULT OnTabSwitched(RoutedEventArgs *pe);
	LRESULT OnSwitchToContact(RoutedEventArgs *pe);
	
public:
	int SetContactInfo(TAG_RECENT *pContact);
	TAG_RECENT *GetContactInfo();
	void InsertMsg(std::string strMsg, std::string strDateTime,int lMsgID,CIUIString strDisplayName=L"");
	void InsertMsgAudio( std::string srFileName, std::string strDateTime, float fProgress,int lMsgID, CIUIString strDisplayName = L"");
	void InsertMsgVideo(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName = L"");
	void InsertMsgPic(std::string srFileName, std::string strDateTime, float fProgress , int lMsgID, CIUIString strDisplayName = L"");
	void InsertMsgFile(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName = L"");
	void InsertMsgEmoji(std::string srFileName, std::string strDateTime, CIUIString strDisplayName = L"");
	void InsertMsgOther(std::string srFileName, std::string strDateTime, CIUIString strDisplayName = L"");
	void InsertCallEventMsg(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode);

	void UpdateContactInfo();
	void ProcessDownLoadFileOK(long nMsgID,bool bComplete, std::string strFileName, ENUM_MSG_TYPE  enumFileType, CIUIString strDisplayName, IUI::HWLWND hWnd);
	void ProcessDownLoadFileFailed(int nMsgID);
	void ReFreshUnReadMsg(vector<CChatRecord>);
	void UpdateDownLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void OnUpdateFileOK(UpLoadDownLoadParam *pDownLoadParam);
	void RefreshMsgStatus(int nMsgID,bool bSucc);
	void PaticipantsUpdate(std::string strStatusText);
	void SetIsConference(bool bConference);
	bool ParseRoomInfo(std::string strStatusText,CONFERENCE_ROOM_INFO& tagConferenceRoomInfo);
protected:
	TAG_RECENT *m_pContactInfo;
	HWLWND m_hCvsTextChat;
	CTextChatCanvasHandler *m_pTextChatCanvasHandler;
	CVideoCanvasHandler		*m_pVideoCanvasHandler;
	CTextChatPageCanvasHandler *m_pTextChatPageCanvasHandler;
	BOOL m_bFirstSwitched;
	CPopupChatDlg m_dlgPopupChat;
	bool          m_bIsConference;
	CONFERENCE_ROOM_INFO m_oConferenceRoomInfo;
};

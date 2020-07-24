#pragma once
#include "AVPlay/AVPlayer.h"
// IDW_INBOX_PANEL

enum TabPage
{
	ENUM_CALL_PAGE = 0,
	ENUM_MISS_PAGE,
	ENUM_VOICE_MAIL_PAGE,
	ENUM_RECORD_PAGE
};
struct ItemUserData
{
	TAG_RECENT *m_pContactInfo;
	long m_nHistoryDataID;
};
class CInboxCanvasHandler : public CWLWndHandler
{
public:
	CInboxCanvasHandler();
	virtual ~CInboxCanvasHandler();

	void InitHistory();
	void InitMiss();
	void InitVoiceMail();
	void InitRecord();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	int InitInboxCalls(RoutedEventArgs *pe);
	int InitInboxMissed(RoutedEventArgs *pe);
	int InitInboxVoiceMails(RoutedEventArgs *pe);
	int InitInboxRecordings(RoutedEventArgs *pe);
	int InitInboxMessages(RoutedEventArgs *pe);

	int OnCreate(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanged(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanging(RoutedEventArgs *pe);
	LRESULT OnLButtonDblClk(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

	LRESULT OnRadCalls(RoutedEventArgs *pe);
	LRESULT OnRadMissed(RoutedEventArgs *pe);
	LRESULT OnRadVoiceMails(RoutedEventArgs *pe);
	LRESULT OnRadRecordings(RoutedEventArgs *pe);
	LRESULT OnRadMessages(RoutedEventArgs *pe);
	LRESULT OnItemBtnAudio(RoutedEventArgs *pe);
	LRESULT OnItemBtnVideo(RoutedEventArgs *pe);
	LRESULT OnItemBtnMore(RoutedEventArgs *pe);
	LRESULT OnBtnFirstPage(RoutedEventArgs *pe);
	LRESULT OnBtnPrePage(RoutedEventArgs *pe);
	LRESULT OnBtnNextPage(RoutedEventArgs *pe);
	LRESULT OnBtnLastPage(RoutedEventArgs *pe);
	LRESULT OnBtnPlayPause(RoutedEventArgs *pe);
	LRESULT OnBtnPlayVideo(RoutedEventArgs *pe);
	LRESULT OnHScroll(RoutedEventArgs *pe);

	/************************************************************************/
	/************************************************************************/
	LRESULT OnPlay(RoutedEventArgs *pe);
	LRESULT OnPlayEnd(RoutedEventArgs *pe);
	LRESULT OnPlayPosChanged(RoutedEventArgs *pe);
	LRESULT OnDragPos(RoutedEventArgs *pe);
	LRESULT OnPlayTimeChanged(RoutedEventArgs *pe);
	/************************************************************************/
	/************************************************************************/
	void DoFlushRecord(int nIndex );
	/************************************************************************/
	/************************************************************************/
	void Swtich2CallPage(bool bTipsCheckSwitch = false);
	void Swtich2MissPage(bool bTipsCheckSwitch = false);
	void Swtich2VoiceMailPage(bool bTipsCheckSwitch = false);
	void Swtich2RecordPage(bool bTipsCheckSwitch = false);
	BOOL SwtichEnable(int& nCurrentIndex, int &nMaxPage,bool bTipsCheckSwitch );


public:
	/************************************************************************/
	/************************************************************************/
	void Insert2Call(CCallHistory oAddCallHistory);
	void Insert2Miss(CCallHistory oAddMissCallHistory);
	void Insert2VoiceMail(CVoiceMail *pVoiceMail);
	void Insert2Record(CVARecord *pRecord);
	/************************************************************************/
	/************************************************************************/
	void EraseItemInCall(long long nDataID);
	void EraseItemInCallNoDB(long long nDataID);
	void EraseItemInMiss(long long nDataID);
	void EraseItemInMissNoDB(long long nDataID);
	void EraseItemVoiceMail(long long nDataID);
	void EraseItemRecord(long long nDataID);

	void RefreshVARecord();
	CIUIString FormatTimeType(CCallHistory oCallHistory, ENUM_CALL_STATUS enumCallStatus);

	void UpdateReadStatusMiss();
	void UpdateReadStatusVoiceMail();
	void UpdateReadStatusRecord();

	bool HaveUnReadMiss();
	bool HaveUnReadVoiceMail();
	bool HaveUnReadRecord();


	void RefreshMiss();
	void RefreshVoicemail();
	void RefreshRecord();
	void RefreshMsg();

	void RefreshCallInfo();
protected:
	Button *m_pRadCalls;
	Label *m_pStaUnreadCalls;
	Button *m_pRadMissed;
	Label *m_pStaUnreadMissed;
	Button *m_pRadVoiceMails;
	Label *m_pStaUnreadVoicemail;
	Button *m_pRadRecordings;
	Label *m_pStaUnreadRecord;
	Button *m_pRadMessages;
	Label *m_pStaUnreadMessage;
	TabControl *m_pTabInbox;
	Label *m_pStaSplitter;
	Button *m_pBtnFirstPage;
	Button *m_pBtnPrePage;
	Button *m_pBtnNextPage;
	Button *m_pBtnLastPage;
	
protected:
	HWLWND m_hOriginalSenderCallHistory;
	HWLWND m_hOriginalSenderMissed;
	HWLWND m_hOriginalSenderVoicemails;
	HWLWND m_hOriginalSenderRecords;
	HWLWND m_hOriginalSenderMsgs;
protected:
	TreeView *m_pTvCalls;
	TreeView *m_pTvMissed;
	TreeView *m_pTvMessages;
protected:
	Button *m_pBtnPlaying;
	Slider* m_pSldDuration;
	HWLWND  m_hStaDuration;
private:
	
	CVoiceMail m_oCurrentVoiceMail;
	bool       m_bIsPlaying;
	HWLWND     m_hCurrentPlayingButton;
	HWLWND     m_hLastPlayButton;
	vector<CVoiceMail*>  m_vecVoiceMail;
	vector<CVARecord*>   m_vecVARecord;
	std::vector< CCallHistory> m_vecCallHistory;
	std::vector<CCallHistory> m_vecMissedHistory;
private:
	int m_nVoiceMailIndex;
	int m_nRecordIndex;
	int m_nMissCallIndex;
	int m_nLocalCallIndex;

	int m_nVoiceMailIndexMax;
	int m_nRecordIndexMax;
	int m_nMissCallIndexMax;
	int m_nLocalCallIndexMax;

	TabPage m_nTabPage;
	TabPage m_nLastTabPage;

	CControlProp *m_pAudioItemProp;
	CControlProp *m_pBindVideoProp ;
};

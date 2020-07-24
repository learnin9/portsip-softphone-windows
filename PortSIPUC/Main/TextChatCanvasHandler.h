#pragma once

enum ENUM_SENDMSG_TYPE
{
	ENUM_TYPE_PURE,
	ENUM_TYPE_JSON
};


class CEmoticonWnd;

class CTextChatCanvasHandler : public CWLWndHandler
{
public:
	CTextChatCanvasHandler();
	virtual ~CTextChatCanvasHandler();
	void		SetContactInfo(TAG_RECENT *pContactInfo);
	void  ReFreshUnReadMsg(vector<CChatRecord> vecChatRecord);
	int    InsertCallEventMsg(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode)
	{
		return CallEventFromDB(oChatRecord, enumInsertHistoryMode);
	}
	void		InsertMsg(std::string strMsg, bool bSelf, int lMsgID, CIUIString strDisplayName);
	void		InsertMsgAudio(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName);
	void		InsertMsgVideo(std::string srFileName, std::string strDateTime, float fProgress, int lMsgID, CIUIString strDisplayName);
	void		InsertMsgPic(std::string srFileName, std::string strDateTime, float fProgress,int lMsgID, CIUIString strDisplayName);
	void		InsertMsgFile(std::string srFileName, float fProgress, int lMsgID, CIUIString strDisplayName);
	void		InsertMsgEmoji(std::string srFileName, std::string strDateTime, CIUIString strDisplayName);
	void		InsertMsgOther(std::string srFileName, std::string strDateTime,CIUIString strDisplayName);


	void       UpdateMsgFile(CIMMsg theMsg,ENUM_MSG_TYPE enumMsgType, float fProgress);
	void       AddMsgFile(std::string strFileName,CIUIString strDisplayName,long lMsgID, ENUM_MSG_TYPE enumMsgType);

	void        UpdateContactInfo();
	void		DownLoadFileOK(long lMsgID,bool bComplete, std::string strFileName, ENUM_MSG_TYPE  enumFileType, CIUIString strDisplayName,IUI::HWLWND hWnd);
	void	   ProcessDownLoadFileFailed(int nMsgID);
	void		UpdateFileOleProgress(HWLWND hWndOle,float fProgress);
	void        UpdatePicOleProgress(HWLWND hWndOle, float fProgress);
	void		UpdateVideoOleProgress(HWLWND hWndOle, /*void *pVideoCanvasHandler,*/ float fProgress);
	void		UpdateAudioOleProgresss(HWLWND hWndOle, float fProgress);
	void UpdateDownLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateDownLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadAudioFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadVideoFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadPicFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void UpdateUpLoadFileProgress(UpLoadDownLoadParam *pDownLoadParam);
	void OnUpdateFileOK(UpLoadDownLoadParam* pDownLoadParam);
	bool CheckProgressIsComplete(double dProgress);
	long Send(std::string strSendMsg, ENUM_SENDMSG_TYPE enumSendMsgType);
	void FileMsgFromDB(CChatRecord oChatRecord,wstring wstrSender, INSERT_HISTORY_MODE enumInsertHistoryMode,bool isScreenPic = false);
	//call event tips
	int CallEventFromDB(CChatRecord oChatRecord, INSERT_HISTORY_MODE enumInsertHistoryMode);
	void TextMsgFromDB(CChatRecord oChatRecord,INSERT_HISTORY_MODE enumInsertHistoryMode);
	std::wstring FormatFilePath(bool bSelf, ENUM_MSG_TYPE enumMsgType, CChatRecord oChatRecord, std::wstring wstrFileName,bool bHasDownload=true);
	bool GroupEnableFun();
	static unsigned int AddDownLoadThread(void* pParam);
	static unsigned int AddUpLoadThread(void* pParam);
	void ResendMsg(CChatRecord theResendMsg);
	void ReDownLoadFile(CChatRecord reDownLoadRecord);
	void RefreshMsgStatus(int nMsgID ,bool bSucc);
	bool HaveEmoji(std::string strMsg,vector<EMOJI_INFO> &vecEmojiInfo);
	int GetLocalRecordMSGID() {
		return m_nCurrentMinHistoryMsgID;
	};
	void CancleMoreMsg();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkEmoticon(RoutedEventArgs *pe);
	LRESULT OnBtnVibrate(RoutedEventArgs *pe);
	LRESULT OnChkScreenshot(RoutedEventArgs *pe);
	LRESULT OnBtnSendImage(RoutedEventArgs *pe);
	LRESULT OnBtnSendFile(RoutedEventArgs *pe);
	LRESULT OnBtnSendVideo(RoutedEventArgs *pe);
	LRESULT OnBtnSendAudio(RoutedEventArgs *pe);
	LRESULT OnBtnSendMsg(RoutedEventArgs *pe);
	LRESULT OnBtnMsgHistory(RoutedEventArgs *pe);

	int InitChatHistory();
	LRESULT OnExitSizeMove(RoutedEventArgs *pe);
	LRESULT OnSelectedEmoticon(RoutedEventArgs *pe);
	LRESULT OnPreviewKeyDown(RoutedEventArgs *pe);
	LRESULT OnLButtonDblClk(RoutedEventArgs *pe);
	LRESULT OnIMDrawItem(RoutedEventArgs *pe);
	void Add2UploadList(CIUIString strIUIFilePath, CIUIString strIUIFileName,const char *fileType, ENUM_MSG_TYPE enumFileType,HWLWND hWnd,int nMsgID);
	void AddFileOleByRecvMSG(HWLWND hReInput, LPCTSTR lpszVideo, int cp, HWLWND &hOleWnd);
protected:
	IMRichTextBox *m_pHistory;
	Canvas *m_pCanvasInput;
	Label *m_pStaSplitterBar;
	Button *m_pChkEmoticon;
	Button *m_pBtnVibrate;
	Button *m_pChkScreenshot;
	Button *m_pBtnSendImage;
	Button *m_pBtnSendFile;
	Button *m_pBtnSendVideo;
	Button *m_pBtnSendAudio;
	RichTextBox *m_pReInput;
	Button *m_pBtnSendMsg;
	Button *m_pBtnMsgHistory;
protected:
	int m_nHistoryInputYSpace;
	int m_nCurrentMinHistoryMsgID;
	TAG_RECENT		*m_pContactInfo;
	CEmoticonWnd	*m_pEmoticonWnd;
	std::map<wstring, HWLWND> m_mapFile2Ole;
	std::map<wstring, HWLWND> m_mapFilePic2Ole;
	std::map<wstring, HWLWND> m_mapFileAudio2Ole;
	std::map<wstring, OLE_VIDEO_INFO> m_mapFileVideo2Ole;
};

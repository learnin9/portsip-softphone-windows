#pragma once
#include "MediaToolbarWnd.h"
#include "CallingPageDialog/CallingPageTransfer/AddCallerDlg.h"
#include "CallingPageDialog/CallingPageTransfer/CallingTransferDlg.h"
#include "MiniKeyboardWnd.h"
#include "VideoHostWnd.h"
#define  SELF_VIDEO_SIZE_RATIO  0.7
// IDW_VIDEO_PAGE
class CVideoCanvasHandler : public CWLWndHandler
{
public:
	CVideoCanvasHandler(TAG_RECENT *pContactInfo);
	virtual ~CVideoCanvasHandler();

	void    SetIsConference(bool bIsConference);

	bool	CallTheContact();
	bool    OnCallTheContact();
	bool    OnCallTheGroup();
	void    HangupByRemote(bool bConnected,int nActiveLine, bool bJumpPage = true);
	HWND    GetPlayHwnd();
	HWND    GetLocalHwnd();
	bool	GetIsConnected(int nActiveLine) ;
	bool    GetIsConnectedNoLine();
	bool    GetCallStatus();
	
	void    AnswerByRemote(int nActiveLine,bool bConferenceRoom);
	void    UnHoldByRemote();
	void    HoldByRemote();
	void   SetConnected(bool bConnected, int nActiveLine);
	void ProcessTranseferSuccess(bool bSuccess, int nErrorCode = 0);
	void ClearnCallData();
	void TransferClosed();
	int ShowVideoWindow(BOOL bShow);
	void DoSingleContactHangup(bool bBtnSelf=true, bool bJumpPage = false);
	void DoTalkGroupHangup();
	void CloseWnd();
	void ExistOneLine(int nExitActiveLine,bool bConnected = true);
	void AddContact2Group(TAG_RECENT * pConctactInfo);
	void GroupEnableFun();
	bool GetIsTalkGroupMode() {
		if (m_pContactInfo)
		{
			if (m_pContactInfo->m_enumRecentContact==ENUM_MULTI_CONTACT)
			{
				return true;
			}
		}
		return false;
	};
	void OnProcessConfenceAction(ENUM_RECENT_CONTACT_TYPE  SrcContactType);
	void CleanChkStatus();
	void MuteMicroPhineSession();
	void ProcessSwtichLine(bool bHold);
	void ProcessSwtichLineAboutMyLocalVideo(); 
	void UpdateVideoHwndInfo();
	void ChangeMySelfWndSize();
	void ChangeOtherPersonWndSize(bool bIsFullScreen);
	HWND CreateWaitVideoHost(HWND hParent);

	void ShowWaitGif();
	void HideWaitGif();
	bool OnCallAction(TAG_RECENT *pContactInfo);
	bool ShowLocalVideoInConference();
	void CloseDTMF();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	int OnWindowPosChanged(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkRec(RoutedEventArgs *pe);
	LRESULT OnChkMute(RoutedEventArgs *pe);
	LRESULT OnChkPause(RoutedEventArgs *pe);
	LRESULT OnBtnTransfer(RoutedEventArgs *pe);
	LRESULT OnBtnAdd(RoutedEventArgs *pe);
	LRESULT OnBtnKeyboard(RoutedEventArgs *pe);
	LRESULT OnBtnShare(RoutedEventArgs *pe);
	LRESULT OnBtnMsg(RoutedEventArgs *pe);
	LRESULT OnBtnNoVideo(RoutedEventArgs *pe);
	LRESULT OnBtnHangup(RoutedEventArgs *pe);
	

	int ToggleVideoWnd();
	int LayoutVideoWnd(WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProcSession(HWND hlWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static std::map<int, CVideoCanvasHandler*> m_mapHwnd2VideoPage;
	void BeginTimer();
	void   ProcessTimer();
	void CloseTransferDlg();
	void CloseAddCallerDlg();
	void ProcessTransferFailed();
	void ProcessAddContactFailed();
	void  CloseLocalVideoShow();
public:
	void ChangeDeviceSwtich(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera = false);
	
	int SetContactInfo(TAG_RECENT *pContactInfo);
	TAG_RECENT *GetContactInfo();
/*protected:
	Canvas *m_pCanvasMediaToolbar;

protected:
	Button *m_pBtnRec;
	Button *m_pBtnMute;
	Button *m_pBtnPause;
	Button *m_pBtnTransfer;
	Button *m_pBtnAdd;
	Button *m_pBtnKeyboard;
	Button *m_pBtnShare;
	Button *m_pBtnMsg;
	Button *m_pBtnNoVideo;
	Button *m_pBtnHangup;
*/
	BOOL m_bOldVideoVisibleState;//

	TAG_RECENT *m_pContactInfo;
	HWND m_hVideoHost;		
	HWND m_hVideoWnd;		
	HWND m_hSmallVideoWnd;  

	CMediaToolbarWnd  *m_pWndVideoToolbar;
	CCallingTransferDlg *m_pCallingTransferDlg;
	CAddCallerDlg		*m_pAddCallerDlg;
	TAG_RECENT * m_pSrcContact;
	TAG_RECENT * m_pAddContact;

	int			 m_hDecodeTimer;
	CMiniKeyboardWnd *m_pDTMFKeyboardWnd;
	POINT m_ptNormal;
	bool  m_bSaveNormalPT;
	bool                   m_bRecordVideoWHRate;
	TagScreenMetrics	   m_stuLocalMyVideo;
	bool				m_bIsConference;
	CVideoHostWnd *m_pwndVideoHost;
};

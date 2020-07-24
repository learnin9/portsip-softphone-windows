#pragma once
#include "CallingPageDialog/CallingPageTransfer/CallingTransferDlg.h"
#include "CallingPageDialog/CallingPageTransfer/AddCallerDlg.h"
#include "MiniKeyboardWnd.h"
// IDW_AUDIO_PAGE
class CAudioPageCanvasHandler : public CWLWndHandler
{
public:
	CAudioPageCanvasHandler();
	virtual ~CAudioPageCanvasHandler();

	void    SetIsConference(bool bIsConference);

	bool	GetIsConnected(int nActiveLine);
	bool     GetIsConnectedNoLine();
	bool	GetCallStatus();
	bool	CallTheContact();
	bool    CallTheGroup();
	void    HangupByRemote(bool bConnected,int nActive,bool bJumpPage = true);
	void    UnHoldByRemote( );
	void    HoldByRemote();
	int     GetActiveLine();
	void    AnswerByRemote(int nActiveLine, bool bConferenceRoom);
	static void CALLBACK TimerProcSession(HWND hlWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static std::map<int, CAudioPageCanvasHandler*> m_mapHwnd2AudioPage;
	void   SetConnected(bool bConnected,int nActiveLine);
	void   ResetBtnStatus();
	void   ProcessTimer();
	void   ProcessTranseferSuccess(bool bSuccess, int nErrorCode = 0);
	void  TransferClosed();
	void DoHangup(bool bBtnSelf ,bool bJumpPage = false);
	void DoSingleContactHangup(bool bBtnSelf = false, bool bJumpPage = false);
	void DoTalkGroupHangup();
	void ExistOneLine(int nExitActiveLine,bool bConnected = true);
	void SetFunBtnStat(BOOL bEnable,bool isTalkGroup =false);
	void CloseWnd();
	void CloseDTMF();
	void AddContact2Group(TAG_RECENT * pConctactInfo);
	void CleanChkStatus();
	void MuteMicroPhineSession();
	void ProcessSwtichLine(bool bHold);
	void ChangeDeviceSwtich(bool bEnableAudioOut, bool bEnableAudioIn, bool bEnableCamera = false);
	void CloseTransferDlg();
	void CloseAddCallerDlg();
	void ProcessTransferFailed();
	void ProcessAddContactFailed();
public:
	void   BeginTimer();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

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

	LRESULT OnAfterPaint(RoutedEventArgs *pe);

public:
	int SetContactInfo(TAG_RECENT *pContactInfo);
	TAG_RECENT *GetContactInfo();

protected:
	Canvas *m_pCanvasCenter;
	Label *m_pStaFace;
	Label *m_pStaName;
	Label *m_pStaSign;
	Button *m_pChkRec;
	Button *m_pChkMute;
	Button *m_pChkPause;
	Button *m_pBtnTransfer;
	Button *m_pBtnAdd;
	Button *m_pBtnKeyboard;
	Button *m_pBtnShare;
	Button *m_pBtnMsg;
	Button *m_pBChkNoVideo;
	Button *m_pBtnHangup;
	Label *m_pStaState;
protected:
	TAG_RECENT			  *m_pContactInfo;
	int			 m_hSessionTimer;
	CCallingTransferDlg *m_pCallingTransferDlg;
	CAddCallerDlg		*m_pAddCallerDlg;
	//DTMF WINDOW
	CMiniKeyboardWnd *m_pDTMFKeyboardWnd;
	bool m_bHoldBySelf;
	bool m_bIsConference;
};

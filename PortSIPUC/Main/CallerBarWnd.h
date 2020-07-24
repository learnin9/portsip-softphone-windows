#pragma once


// IDW_CALLER_BAR
class CCallerBarWnd : public Window
{
public:
	CCallerBarWnd();
	virtual ~CCallerBarWnd();
	void SetParameters(ICallbackParameters * pCallParameters,int nActiveLine,bool bIsConference);
	ICallbackParameters * GetParameters() ;
	//int  GetSessionID() { return m_nSessionID; }
	int  GetActiveLine() { return m_nActiveLine; };
	void CloseByRemote();
protected:
	int InitControls();
	void InitControlData();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnAudio(RoutedEventArgs *pe);
	LRESULT OnBtnVideo(RoutedEventArgs *pe);
	LRESULT OnBtnHangup(RoutedEventArgs *pe);
	LRESULT OnAfterPaint(RoutedEventArgs *pe);

	void InitProcessData();
protected:
	Label *m_pStaFace;
	Label *m_pStaCaller;
	Button *m_pBtnAudio;
	Button *m_pBtnVideo;
	Button *m_pBtnHangup;
	Label *m_pStaPhoneNumber;
	Label *m_pStaSign;
	Label *m_pStaCall;
	Label *m_pStaSplitter;
	Label *m_pStaCall2;
	Label *m_pStaCaller2;
	Label *m_pStaPhoneNumber2;
	Label *m_pStaSign2;
	ICallbackParameters* m_pCallBackParameters;
	CCallHistory m_oProcessComing;
	TAG_RECENT *m_pContactDBInfo;
	int			m_nSessionID;
	int         m_nActiveLine;
	bool        m_bIsConfrence;
	ENUM_CONTENT_TYPE m_enumCallType;
};

#pragma once


// IDW_MEDIA_TOOLBAR
class CMediaToolbarWnd : public Window
{
public:
	CMediaToolbarWnd(CHAT_TAB eChatTab, ENUM_RECENT_CONTACT_TYPE enumContactType);
	virtual ~CMediaToolbarWnd();
	void SetFunBtnStat(BOOL bEnable, bool isTalkGroup = false);
	void ResetBtnStatus(bool isTalkGroup = false);
	int GetRecCheck();
	void SetRecCheck(int nMode);
	BOOL GetRecEnable();
	void SetActiveLine(int nLine) {
		m_nActiveLine = nLine;
	};
	int GetMuteCheck();
	void SetMuteCheck(int nMode);


	int GetHoldCheck();
	void SetHoldCheck(int nMode);

	int GetVideoCheck();
	void SetVideoCheck(int nMode);

	bool GetHoldBySelf()
	{
		return m_bHoldBySelf;
	}
	void SetHoldBySelf(bool bSelf)
	{
		m_bHoldBySelf = bSelf;
	}
public:
	int SetBindVideoPanel(HWLWND hBindVideo);
	HWLWND GetBindVideoPanel() const;

	Button *m_pBtnKeyboard;
	Button *m_pChkPause;
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

protected:
	Canvas *m_pCanvasMediaToolbar;
	Button *m_pChkRec;
	Button *m_pChkMute;
	
	Button *m_pBtnTransfer;
	Button *m_pBtnAdd;
	
	Button *m_pBtnShare;
	Button *m_pBtnMsg;
	Button *m_pChkNoVideo;
	Button *m_pBtnHangup;
	int			  m_nActiveLine;
protected:
	CHAT_TAB m_eChatTab;
	HWLWND m_hBindVideo;
	ENUM_RECENT_CONTACT_TYPE   m_enumContactTpye;
	bool  m_bHoldBySelf;
};

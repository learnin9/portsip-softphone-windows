#pragma once
extern wchar_t *G_TCHAR_TRANSPORT[];
extern wchar_t *G_TCHAR_SRTP[];
extern wchar_t *G_CHAR_PRESENCE__MODE[];
#define      STUN_SERVER_STRING_LENGTH 512
#define      PROXY_SERVER_STRING_LENGTH 512
#define      DISPLAY_NAME_STRING_LENGTH	1024
#define      AUTH_NAME_STRING_LENGTH    512
#define      OUT_BOUND_STRING_LENGTH    512

class CLoginSettingsWnd : public Window
{
public:
	CLoginSettingsWnd();
	virtual ~CLoginSettingsWnd();
	CUser* ReturnCurrentUser() { return m_pUser; };
	void   SetCurrentUser(CUser* pUser) { m_pUser = pUser; };
protected:
	int InitControls();

	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnChkMax(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnChkVerifyTls(RoutedEventArgs *pe);
	LRESULT OnBtnReturn(RoutedEventArgs *pe);
	LRESULT OnBtnSave(RoutedEventArgs *pe);

	// Message handler
	bool VerifyParam(std::string& strErrorMsg);
	void Save2Memery();
protected:
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	Canvas *m_pCanvasMain;
	Label *m_pStaSettings;
	Label *m_pStaAccount;
	Label *m_pStaDisplayName;
	RichTextBox *m_pReDisplayName;
	Label *m_pStaAuthroizationName;
	RichTextBox *m_pReAuthroizationName;
	Label *m_pStaOutboundServer;
	RichTextBox *m_pReOutboundServer;
	Label *m_pStaTransport;
	Label *m_pStaSignalingTransport;
	ComboBox *m_pCmbSignalingTransport;
	Label *m_pStaSrtp;
	ComboBox *m_pCmbSrtp;
	Button *m_pChkVerifyTls;
	//Label *m_pStaPresnece;
	//Label *m_pStaPresenceMode;
	//ComboBox *m_pCmbPresenceMode;
	Label *m_pStaPollTime;
	RichTextBox *m_pRePollTime;
	Label *m_pStaRefreshInterval;
	RichTextBox *m_pReRefreshInterval;
	Button *m_pBtnReturn;
	Button *m_pBtnSave;


	CUser* m_pUser;
};

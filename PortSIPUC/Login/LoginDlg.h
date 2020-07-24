#pragma once
#include "System/SystemEx.h"
#include "db/portsql.hxx"
#include "../Main/InfoWndMgr.h"
class CLoginDlg : public Window
{
public:
	CLoginDlg(void);
	~CLoginDlg(void);
	void ProcessLoginReslut(bool bLoginSuccess,std::string strErrorInfo = "");
	void DoLogin();
	void ShowLoginWaiting();
	void EndLoginWaiting();
	static unsigned int _DoLoginThread(void* pParam);
	static unsigned int _AutoLoginThread(void* pParam);
	void DoLoginNext();
	void DoAutoLogin();
	bool CheckTime(int nCode);
	void EnableWindowBtn();
	void UnEnableWindowBtn();
private:
	std::list< CUser*> m_lstUser;
	CUser*		      m_pActiveUser;
protected:
	int InitControls();
	void InitControlsData();
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	bool VerifyLocalParam(std::string &strError);
	void ResetUserData(int nColNum);
protected:
	bool   m_bHaveLocalUserData;
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	Canvas *m_pCanvasMain;

	Label *m_pLogo;
	Label *m_pStartConnectTips;
	ComboBox *m_pCmbAccount;
	RichTextBox *m_pRePassword;
	RichTextBox *m_pReSipdomain;
	Button * m_pRemberMe;
	Button * m_pAutoLogin;
	Button * m_pAdvanced;
	Button * m_pLogin;
	CUser*	 m_pUser;
	Canvas *m_pCanvasLogging;
	Label *m_pStaLogging;
};


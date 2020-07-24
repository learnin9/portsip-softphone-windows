#pragma once
#include "System/SystemEx.h"
#include "db/portsql.hxx"
#include "../Main/InfoWndMgr.h"
class CLoginLoadingDlg : public Window
{
public:
	CLoginLoadingDlg(void);
	~CLoginLoadingDlg(void);
	void ProcessLoginReslut(bool bLoginSuccess,std::string strErrorInfo = "");
	void ShowLoginWaiting();
	void EndLoginWaiting();
protected:
	int InitControls();
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
protected:
	Canvas *m_pCanvasLogging;
	Label *m_pStaLogging;
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
};


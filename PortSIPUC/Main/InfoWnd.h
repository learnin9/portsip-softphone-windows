#pragma once
#include "CommonDef.h"
// IDW_INFO
class CInfoWnd : public Window
{
public:
	CInfoWnd(WND_INFO_LEVEL enumInfoWndLevel,bool bInMain = true);
	virtual ~CInfoWnd();
	
public:
	int SetAutoCloseTime(int nSecond);
	int SetTitle(LPCTSTR lpszTitle);
	int SetInfo(LPCTSTR lpszInfo);
	void CloseWndByOutSide();
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);

protected:
	Label *m_pStaTitle;
	Label *m_pStaInfo;
	Button *m_pBtnClose;
	WND_INFO_LEVEL m_enumWndInfoLevel;
protected:
	int m_nAutoCloseTime;
	bool m_bInMainDlg;
};

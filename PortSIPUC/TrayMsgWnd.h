#pragma once


// IDW_TRAY_MSG
class CTrayMsgWnd : public Window
{
public:
	CTrayMsgWnd();
	virtual ~CTrayMsgWnd();

protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnMenu(RoutedEventArgs *pe);
	LRESULT OnBtnIgnore(RoutedEventArgs *pe);

	LRESULT OnAfterPaint(RoutedEventArgs *pe);

protected:
	Label *m_pStaName;
	Button *m_pBtnIgnore;
	TreeView *m_pTreeMsg;
};

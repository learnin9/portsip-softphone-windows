#pragma once


// IDW_MSG_HISTORY
class CMsgHistoryWnd : public Window
{
public:
	CMsgHistoryWnd();
	virtual ~CMsgHistoryWnd();

protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnIcon(RoutedEventArgs *pe);
	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnChkMax(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnRadAllMsg(RoutedEventArgs *pe);
	LRESULT OnRadPicMsg(RoutedEventArgs *pe);
	LRESULT OnRadFileMsg(RoutedEventArgs *pe);
	LRESULT OnBtnFirstPage(RoutedEventArgs *pe);
	LRESULT OnBtnPrePage(RoutedEventArgs *pe);
	LRESULT OnBtnNextPage(RoutedEventArgs *pe);
	LRESULT OnBtnLastPage(RoutedEventArgs *pe);

protected:
	Button *m_pBtnIcon;
	Label *m_pStaTitle;
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	Button *m_pRadAllMsg;
	Button *m_pRadPicMsg;
	Button *m_pRadFileMsg;
	Button *m_pBtnFirstPage;
	Button *m_pBtnPrePage;
	Button *m_pBtnNextPage;
	Button *m_pBtnLastPage;
	IMRichTextBox *m_pHistory;
};

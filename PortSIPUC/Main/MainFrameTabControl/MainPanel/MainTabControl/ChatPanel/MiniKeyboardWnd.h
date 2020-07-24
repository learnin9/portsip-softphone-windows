#pragma once


// IDW_MINI_KEYBOARD
class CMiniKeyboardWnd : public Window
{
public:
	CMiniKeyboardWnd();
	virtual ~CMiniKeyboardWnd();
	void SetActiveLine(int nActiveLine) { m_nActiveSession = nActiveLine; };
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtn1(RoutedEventArgs *pe);
	LRESULT OnBtn2(RoutedEventArgs *pe);
	LRESULT OnBtn3(RoutedEventArgs *pe);
	LRESULT OnBtn4(RoutedEventArgs *pe);
	LRESULT OnBtn5(RoutedEventArgs *pe);
	LRESULT OnBtn6(RoutedEventArgs *pe);
	LRESULT OnBtn7(RoutedEventArgs *pe);
	LRESULT OnBtn8(RoutedEventArgs *pe);
	LRESULT OnBtn9(RoutedEventArgs *pe);
	LRESULT OnBtnXing(RoutedEventArgs *pe);
	LRESULT OnBtn0(RoutedEventArgs *pe);
	LRESULT OnBtnSharp(RoutedEventArgs *pe);

protected:
	Button *m_pBtn1;
	Button *m_pBtn2;
	Button *m_pBtn3;
	Button *m_pBtn4;
	Button *m_pBtn5;
	Button *m_pBtn6;
	Button *m_pBtn7;
	Button *m_pBtn8;
	Button *m_pBtn9;
	Button *m_pBtnXing;
	Button *m_pBtn0;
	Button *m_pBtnSharp;
	int m_nActiveSession;
};

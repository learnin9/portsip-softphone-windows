#pragma once

#include "System/SystemEx.h"
// 窗口IDW_CALL_PAGE下的唯一根控件IDC_CANVAS_CALL的事件响应类
class CCallCanvasHandler : public CWLWndHandler
{
public:
	CCallCanvasHandler();
	virtual ~CCallCanvasHandler();

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	void OnBtnDBClick(CIUIString strBtn);
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnCloseCallPage(RoutedEventArgs *pe);
	LRESULT OnBtnNum1(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum2(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum3(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum4(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum5(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum6(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum7(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum8(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum9(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNumxing(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnNum0(RoutedEventArgs *pe, CIUIString &  strREString);
	LRESULT OnBtnNumsharp(RoutedEventArgs *pe, CIUIString & strREString);
	LRESULT OnBtnAudio(RoutedEventArgs *pe, CIUIString strREString);
	LRESULT OnBtnCalling(RoutedEventArgs *pe, CIUIString strREString);
	LRESULT OnBtnVideo(RoutedEventArgs *pe, CIUIString strREString);

	LRESULT OnCommand(RoutedEventArgs *pe);
	LRESULT OnAfterPaintBkImage(RoutedEventArgs *pe);




protected:
	Label *m_pStaCallTitle;
	Button *m_pBtnCloseCallPage;
	RichTextBox *m_pReInput;
	Canvas *m_pCanvasKeyboard;
	Button *m_pBtnNum1;
	Button *m_pBtnNum2;
	Button *m_pBtnNum3;
	Button *m_pBtnNum4;
	Button *m_pBtnNum5;
	Button *m_pBtnNum6;
	Button *m_pBtnNum7;
	Button *m_pBtnNum8;
	Button *m_pBtnNum9;
	Button *m_pBtnNumxing;
	Button *m_pBtnNum0;
	Button *m_pBtnNumsharp;
	Button *m_pBtnAudio;
	Button *m_pBtnCalling;
	Button *m_pBtnVideo;
private:
	CIUIString  m_strSipNumber;
};

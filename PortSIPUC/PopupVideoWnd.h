#pragma once
#include "./Utility/BitmapHelper.h"

// IDW_POPUP_VIDEO
class CPopupVideoWnd : public Window
{
public:
	CPopupVideoWnd();
	virtual ~CPopupVideoWnd();

	void SetPlayParam(HBITMAP pHBitmap,/*Button *pSrcChkPlayPause,*/CIUIString strFilePath, CIUIString strTotalPlayTime,\
	CIUIString strFileName,int		nCrruentVolumn = 100);
	void AutoPlay();
	HWND  GetPlayHwnd() { return  m_hVideoWnd; };
protected:
	int InitControls();
	void SetVol();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnChkMax(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnChkPlayPause(RoutedEventArgs *pe);
	LRESULT OnChkMute(RoutedEventArgs *pe);
	LRESULT OnBtnFullScreen(RoutedEventArgs *pe);
	LRESULT OnHScroll(RoutedEventArgs *pe);
	LRESULT OnMouseLBDown(RoutedEventArgs *pe);
	LRESULT OnMouseLBUp(RoutedEventArgs *pe);

	LRESULT OnAfterPaint(RoutedEventArgs *pe);
	/************************************************************************/
	/* process play                                                         */
	/************************************************************************/
	LRESULT OnPlay(WPARAM wParam,LPARAM lParam);
	LRESULT OnPlayEnd(WPARAM wParam, LPARAM lParam);
	LRESULT OnPlayPosChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnDragPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnPlayTimeChanged(WPARAM wParam, LPARAM lParam);
protected:
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	Canvas *m_pCanvasVideo;
	Button *m_pChkPlayPause;
	Label *m_pStaDuration;
	Slider *m_pSldDuration;
	Button *m_pChkMute;
	Slider *m_pSldVolume;
	Button *m_pBtnFullScreen;

protected:
	HWND m_hVideoWnd;
	HBITMAP  m_pBitMap;
	BitmapHelper *m_pBitmapHelper;
protected:
	CIUIString m_strFilePath;
	CIUIString m_strTotalPlayTime;
	CIUIString m_strFileName;
	int		m_nCrruentVolumn;
};

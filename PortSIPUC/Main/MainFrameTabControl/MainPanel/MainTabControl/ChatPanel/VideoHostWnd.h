#pragma once

#define GETPROP_MEDIA_TOOLBAR	    _T("GETPROP_MEDIA_TOOLBAR")
#define GETPROP_VIDEO_HOST			_T("GETPROP_VIDEO_HOST")
#define FULL_SCREEN_VIDEO_WND_CLASS _T("FullScreenVideoWndClass")
#define GETPROP_MY_VIDEO			_T("GETPROP_MY_VIDEO")
#define VIDEO_WND_CLASS _T("PortGoVideoWndClass")
// IDW_VIDEO_HOST
class CVideoHostWnd : public Window
{
public:
	CVideoHostWnd();
	virtual ~CVideoHostWnd();

	void HideLabelWait();
	void ShowLabelWait();
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

protected:
	Label *m_pStaWait;
};

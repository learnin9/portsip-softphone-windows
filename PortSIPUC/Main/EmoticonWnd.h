#pragma once


// IDW_EMOTICON
class CEmoticonWnd : public Window
{
public:
	CEmoticonWnd(HWLWND hNotify);
	virtual ~CEmoticonWnd();

protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnRadClassicalEmoticon(RoutedEventArgs *pe);
	LRESULT OnRadSymbolEmoticon(RoutedEventArgs *pe);
	LRESULT OnStaEmotion(RoutedEventArgs *pe);

protected:
	Button *m_pRadClassicalEmoticon;
	Button *m_pRadSymbolEmoticon;
	TabControl *m_pTabEmoticon;

protected:
	HWLWND m_hNotify;
};

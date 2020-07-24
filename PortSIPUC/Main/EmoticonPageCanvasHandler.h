#pragma once


enum EMOTICON_PAGE
{
	EP_CLASSICAL,
	EP_SYMBOL
};

class CEmoticonPageCanvasHandler : public CWLWndHandler
{
public:
	CEmoticonPageCanvasHandler();
	virtual ~CEmoticonPageCanvasHandler();

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

protected:
	Label *m_pStaEmoticonTitle;
	ListView *m_pLstEmoticon;
};

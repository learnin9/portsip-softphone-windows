#pragma once

class COleImageCanvasHandler : public CWLWndHandler
{
public:
	COleImageCanvasHandler();
	virtual ~COleImageCanvasHandler();

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

protected:
	Label *m_pStaImage;
	ProgressBar *m_pPrgDownload;
};

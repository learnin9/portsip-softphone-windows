#pragma once

class COleFileCanvasHandler : public CWLWndHandler
{
public:
	COleFileCanvasHandler();
	virtual ~COleFileCanvasHandler();

	void SetFilePath(LPCTSTR lpszFilePath);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnMore(RoutedEventArgs *pe);

protected:
	Label *m_pStaIcon;
	Label *m_pStaFileName;
	Button *m_pBtnMore;
	ProgressBar *m_pPrgUpload;


	CIUIString m_strFilePath;
};

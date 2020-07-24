#pragma once

class COleVideoCanvasHandler : public CWLWndHandler
{
public:
	COleVideoCanvasHandler();
	virtual ~COleVideoCanvasHandler();

	void SetVideoParam(LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime,LPCTSTR lpszFileName);
	void ResetVideoThumbnailImage( );
	static unsigned int ResetVideoThumbnailImageThread(void* pParam);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnPlay(RoutedEventArgs *pe);
protected:
	Label *m_pStaVideo;
	ProgressBar *m_pPrgUpload;
	Label *m_pStaUploadFailed;
	Button *m_pBtnPlay;

	int		m_nCrruentVolumn;
public:
	CIUIString m_strFilePath;
	CIUIString m_strTotalPlayTime;
	CIUIString m_strFileName;	
};

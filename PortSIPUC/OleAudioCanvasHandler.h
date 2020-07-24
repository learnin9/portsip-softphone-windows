#pragma once

class COleAudioCanvasHandler : public CWLWndHandler
{
public:
	COleAudioCanvasHandler();
	virtual ~COleAudioCanvasHandler();

	void SetAudioParam(LPCTSTR lpszFilePath,LPCTSTR lpszTotalPlayTime,LPCTSTR lpszStrFileName);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkPlayPause(RoutedEventArgs *pe);
	LRESULT OnChkMute(RoutedEventArgs *pe);
	LRESULT OnPlay(RoutedEventArgs *pe);
	LRESULT OnPlayEnd(RoutedEventArgs *pe);
	LRESULT OnPlayPosChanged(RoutedEventArgs *pe);
	LRESULT OnDragPos(RoutedEventArgs *pe);
	LRESULT OnPlayTimeChanged(RoutedEventArgs *pe);
protected:
	Canvas *m_pCanvasToolbar;
	Button *m_pChkPlayPause;
	Label *m_pStaDuration;
	Slider *m_pSldProgress;
	Button *m_pChkMute;
	Slider *m_pSldVolumn;
	Label *m_pStaFile;
	Label *m_pStaInfo;
	ProgressBar *m_pPrgAudioUpload;

	CIUIString m_strFilePath;
	CIUIString m_strTotalPlayTime;
	CIUIString m_strFileName;
	int		m_nCrruentVolumn;
};

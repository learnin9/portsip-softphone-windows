#pragma once


// IDW_SETTINGS_APPLICATION
class CSettingsApplicationPageHandler : public CWLWndHandler
{
public:
	CSettingsApplicationPageHandler();
	virtual ~CSettingsApplicationPageHandler();


	void SaveOptions();
	void Save();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkReadReceipts(RoutedEventArgs *pe);
	LRESULT OnBtnBrowser(RoutedEventArgs *pe);
	LRESULT OnBtnOpenFolder(RoutedEventArgs *pe);
	LRESULT OnChkLocalVideoInRoom(RoutedEventArgs *pe);
	LRESULT OnChkDisplayIdle(RoutedEventArgs *pe);
	LRESULT OnChkStartWithSystem(RoutedEventArgs *pe);
	LRESULT OnChkShowTimestamp(RoutedEventArgs *pe);

protected:
	Label *m_pStaSetDefault;
	Button *m_pChkReadReceipts;
	Label *m_pStaFileLocation;
	Button *m_pBtnBrowser;
	Button *m_pBtnOpenFolder;
	RichTextBox *m_pReFile;
	Label *m_pStaPromptUpgrade;
	Button *m_pChkLocalVideoInRoom;
	Label *m_pStaDisplayIdle;
	Button *m_pChkDisplayIdle;
	Label *m_pStaStartWithSystem;
	Button *m_pChkStartWithSystem;
	Label *m_pStaShowTimestamp;
	Button *m_pChkShowTimestamp;
	Label *m_pStaSplitter1;
	Label *m_pStaSplitter2;
	Label *m_pStaSplitter3;
	Label *m_pStaSplitter4;
	Label *m_pStaSplitter5;

private:
	void LoadOptions();

	std::wstring m_strRecordFolder;
};

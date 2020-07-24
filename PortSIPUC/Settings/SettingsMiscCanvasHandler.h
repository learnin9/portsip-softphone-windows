#pragma once


// IDW_SETTINGS_MISC
class CSettingsMiscCanvasHandler : public CWLWndHandler
{
public:
	CSettingsMiscCanvasHandler();
	virtual ~CSettingsMiscCanvasHandler();
	void Save();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkEnableRtp(RoutedEventArgs *pe);
	LRESULT OnChkEnableMwi(RoutedEventArgs *pe);
	LRESULT OnChkEnableSessionTimer(RoutedEventArgs *pe);
	LRESULT OnChkAlwaysForwardTo(RoutedEventArgs *pe);
	LRESULT OnChkOnPhone(RoutedEventArgs *pe);
protected:
	Label *m_pStaRtpPorts;
	Label *m_pStaVideoRange;
	RichTextBox *m_pReVideoFrom;
	Label *m_pStaTo;
	RichTextBox *m_pReVideoTo;
	Label *m_pStaAudioRange;
	RichTextBox *m_pReAudioFrom;
	Label *m_pStaAudioTo;
	RichTextBox *m_pReAudioTo;
	Label *m_pStaEnableRtp;
	Button *m_pChkEnableRtp;
	Label *m_pStaSplitter3;
	Label *m_pStaEnableMwi;
	Button *m_pChkEnableMwi;
	Label *m_pStaSplitter4;
	Label *m_pStaEnableSessionTimer;
	Button *m_pChkEnableSessionTimer;
	RichTextBox *m_pReSessionTimer;
	Label *m_pStaSessionTimer;
	Label *m_pStaSplitter5;
	Label *m_pStaAlwaysForwardTo;
	RichTextBox *m_pReAlwaysForwardTo;
	Label *m_pStaSplitter6;
	Label *m_pStaOnPhone;
	RichTextBox *m_pReOnPhone;
	Label *m_pStaSplitter7;
	Button *m_pChkAlwaysForwardTo;
	Button *m_pChkOnPhone;
private:
	void LoadOptions();



	bool   m_bIsOpen;
};

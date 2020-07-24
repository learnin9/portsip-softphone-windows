#pragma once


// IDW_SETTINGS_LANGUAGES
class CSettingsLanguagesCanvasHandler : public CWLWndHandler
{
public:
	CSettingsLanguagesCanvasHandler();
	virtual ~CSettingsLanguagesCanvasHandler();

	void Save();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	void SelectLanguage();
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnRadEnglish(RoutedEventArgs *pe);
	LRESULT OnRadChs(RoutedEventArgs *pe);
	LRESULT OnRadCht(RoutedEventArgs *pe);

protected:
	Button *m_pRadEnglish;
	Label *m_pStaSplitter1;
	Button *m_pRadChs;
	Label *m_pStaSplitter2;
	Button *m_pRadCht;
	Label *m_pStaSplitter3;


	bool   m_bIsOpen;

	ENUM_LANGUAGE m_enumLLanguage;
	ENUM_LANGUAGE m_enumCLanguage;
};

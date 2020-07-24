#pragma once


// IDW_SETTINGS_MEDIA_QUALITY
class CSettingsMediaQualityHandler : public CWLWndHandler
{
public:
	CSettingsMediaQualityHandler();
	virtual ~CSettingsMediaQualityHandler();
	void Save();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnChkAes(RoutedEventArgs *pe);
	LRESULT OnChkReduceNoise(RoutedEventArgs *pe);
	LRESULT OnChkAutomaticControl(RoutedEventArgs *pe);
	LRESULT OnChkVoiceDetection(RoutedEventArgs *pe);
	LRESULT OnChkComfortNoiseGenerator(RoutedEventArgs *pe);
	LRESULT OnChkEnableAudioQos(RoutedEventArgs *pe);
	LRESULT OnChkEnableVideoQos(RoutedEventArgs *pe);

protected:
	Label *m_pStaAes;
	Button *m_pChkAes;
	Label *m_pStaSplitter1;
	Label *m_pStaReduceNoise;
	Button *m_pChkReduceNoise;
	Label *m_pStaSplitter2;
	Label *m_pStaAutomaticControl;
	Button *m_pChkAutomaticControl;
	Label *m_pStaSplitter3;
	Label *m_pStaVoiceDetection;
	Button *m_pChkVoiceDetection;
	Label *m_pStaSplitter4;
	Label *m_pStaComfortNoiseGenerator;
	Button *m_pChkComfortNoiseGenerator;
	Label *m_pStaSplitter5;
	Label *m_pStaEnableAudioQos;
	Button *m_pChkEnableAudioQos;
	Label *m_pStaSplitter6;
	Label *m_pStaEnableVideoQos;
	Button *m_pChkEnableVideoQos;
	Label *m_pStaSplitter7;
	Label *m_pStaVideoQuality;
	Label *m_pStaDes;
	Label *m_pStaResolution;
	ComboBox *m_pCmbResolution;
	Label *m_pStaBitrate;
	ComboBox *m_pCmbBitrate;
	Label *m_pStaKbps;
private:
	void LoadOptions();


	bool   m_bIsOpen;
};

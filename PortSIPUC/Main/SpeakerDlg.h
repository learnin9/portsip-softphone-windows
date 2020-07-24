#pragma once


// IDD_SPEAKER
class CSpeakerDlg : public Window
{
public:
	CSpeakerDlg();
	virtual ~CSpeakerDlg();

	void LoadMicphoneVolume(int deviceId);
	void loadSpeakerVolume(int deviceId);
	void loadOptions();
	void saveOptions();
protected:
	int InitControls();
	void UpdateVideoData();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnChkCameraOn(RoutedEventArgs *pe);
	LRESULT OnChkSpeaker(RoutedEventArgs *pe);
	LRESULT OnBtnTestSound(RoutedEventArgs *pe);
	LRESULT OnChkMicrophone(RoutedEventArgs *pe);
	LRESULT OnBtnOk(RoutedEventArgs *pe);
	LRESULT SelectCamera(RoutedEventArgs *pe);
	LRESULT OnHScroll(RoutedEventArgs *pe);
protected:
	Label *m_pStaTitle;
	Button *m_pBtnClose;
	Label *m_pStaPic;
	Label *m_pStaCameraOn;
	Button *m_pChkCameraOn;
	Label *m_pStaSpeaker;
	Button *m_pChkSpeaker;
	ComboBox *m_pCmbSpeaker;
	Button *m_pBtnTestSound;
	Label *m_pStaMicrophone;
	Button *m_pChkMicrophone;
	ComboBox *m_pCmbMicrophone;
	Button *m_pBtnOk;

	ComboBox *m_pCmbSelectDevice;
	Slider *m_pSldSpeaker;
	Slider *m_pSldMicrophone;
	
	HWND     m_hLocalVideoDlg;
private:
	BOOL m_AudioLoopbackTest;
};

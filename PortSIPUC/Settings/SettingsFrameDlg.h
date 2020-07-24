#pragma once
#include "SettingsApplicationPageHandler.h"
#include "SettingsAudioCodecsPageHandler.h"
#include "SettingsVideoCodecsPageHandler.h"
#include "SettingsMediaQualityHandler.h"
#include "SettingsMiscCanvasHandler.h"
#include "SettingsLanguagesCanvasHandler.h"
// IDD_SETTINGS_FRAME
class CSettingsFrameDlg : public Window
{
public:
	CSettingsFrameDlg();
	virtual ~CSettingsFrameDlg();

	void Save();
protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnBtnSave(RoutedEventArgs *pe);

protected:
	LRESULT OnTreeNavigateSelectItem(HTREEITEM hSelItem);

protected:
	TreeView *m_pTreeNavigate;
	Label *m_pStaTitle;
	Button *m_pBtnClose;
	Label *m_pStaSplitter;
	TabControl *m_pTabSettingsPage;
	Button *m_pBtnSave;

protected:
	HTREEITEM m_hItemApplication;
	HTREEITEM m_hItemAudioCodecs;
	HTREEITEM m_hItemVideoCodecs;
	HTREEITEM m_hItemMediaQuality;
	HTREEITEM m_hItemMisc;
	HTREEITEM m_hItemLanguages;
private:
	CSettingsApplicationPageHandler * m_pPageApplication;
	CSettingsAudioCodecsPageHandler * m_pPageAudioCodes;
	CSettingsVideoCodecsPageHandler * m_pPageVideoCodes;
	CSettingsMediaQualityHandler	* m_pPageMediaQuality;
	CSettingsMiscCanvasHandler		* m_pPageMisc;
	CSettingsLanguagesCanvasHandler * m_pLanguages;
	void SaveAllData();

};

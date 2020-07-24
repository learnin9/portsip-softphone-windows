#pragma once

#include "TreeItemDrag.h"
// IDW_SETTINGS_AUDIO_CODECS
class CSettingsAudioCodecsPageHandler : public CWLWndHandler
{
public:
	CSettingsAudioCodecsPageHandler();
	virtual ~CSettingsAudioCodecsPageHandler();
	bool Save();
	void RefreshItemIndex();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnButtonClickItemBtn(RoutedEventArgs *pe);
protected:
	TreeView *m_pTreeAudioCodec;
	UINT m_uCodecTreeDropItemClipFormat;


private:
	void LoadOptions();

	bool m_bIsOpen;
};

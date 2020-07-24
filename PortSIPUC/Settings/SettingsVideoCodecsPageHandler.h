#pragma once
#include "TreeItemDrag.h"
// IDW_SETTINGS_VIDEO_CODECSÏÂ
class CSettingsVideoCodecsPageHandler : public CWLWndHandler
{
public:
	CSettingsVideoCodecsPageHandler();
	virtual ~CSettingsVideoCodecsPageHandler();
	void Save();
	void RefreshItemIndex();
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnButtonClickItemBtn(RoutedEventArgs *pe);
protected:
	TreeView *m_pTreeVideoCodec;
	UINT m_uCodecTreeDropItemClipFormat;
private:
	void LoadOptions();


	bool   m_bIsOpen;
};

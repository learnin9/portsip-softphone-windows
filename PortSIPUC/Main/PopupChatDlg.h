#pragma once
#include "CommonDef.h"

// ´°¿ÚIDD_POPUP_CHAT
class CPopupChatDlg : public Window
{
public:
	CPopupChatDlg();
	virtual ~CPopupChatDlg();

	void SetConferenceNumber(CONFERENCE_ROOM_INFO tagConferenceRoomInfo);
	void SetContact(TAG_RECENT *pConatctInfo);
protected:
	int InitControls();
	HTREEITEM InsertContactItem(
		CControlProp *pBindProp,
		LPCTSTR lpszIconFile,
		LPCTSTR lpszName,
		LPCTSTR lpszPhone,
		const CContactDB *pContact
	);
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnChkMax(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnBtnForbiddenTone(RoutedEventArgs *pe);
	LRESULT OnBtnRelieveForbiddenTone(RoutedEventArgs *pe);
	LRESULT OnBtnMore(RoutedEventArgs *pe);

	LRESULT OnAfterPaint(RoutedEventArgs *pe);
public:
	int SetTextChatControl(HWLWND hTextChat, HWLWND hOldParent);
	void RefreshMember(CONFERENCE_ROOM_INFO tatConferenceRoomInfo);
protected:
	Canvas *m_pCanvasVideoChat;
	Label *m_pStaTitle;
	Button *m_pBtnMin;
	Button *m_pChkMax;
	Button *m_pBtnClose;
	Label *m_pStaSplitter;
	DockPanel *m_pMain;
	Canvas *m_pCanvasTextChat;
	Canvas *m_pCanvasGroupContacts;
	Label *m_pStaGroupTitle;
	TreeView *m_pTreeGroupContacts;
	Button *m_pBtnForbiddenTone;
	Button *m_pBtnRelieveForbiddenTone;
	Button *m_pBtnMore;
protected:
	HWLWND m_hTextChat;
	HWLWND m_hTextChatOldParent;
	CONFERENCE_ROOM_INFO m_oConferenceRoomInfo;
	TAG_RECENT *m_pContactInfo;

	CIUIString m_strTitle;
	bool   m_bIsConferenceRoom;
};

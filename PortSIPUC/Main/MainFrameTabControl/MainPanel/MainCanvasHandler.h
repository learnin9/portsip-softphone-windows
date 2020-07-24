#pragma once

class CChatTabControlHandler;
class CContactsCanvasHandler;
// IDW_MAIN
class CMainCanvasHandler : public CWLWndHandler
{
public:
	CMainCanvasHandler();
	virtual ~CMainCanvasHandler();

	void InsertRecentContact(TAG_RECENT* pContact);
	CChatTabControlHandler* FindChatTabControlHandlerByContactDBPointer(TAG_RECENT *pContactDB);
	void RefreshRecentContantOnlineStatus(CContactDB *pContact,bool &bUpdate);
	void RefreshLocalContactOnlineStatus(CContactDB *pContact);
	void RefreshLocalContactUnReadNum(TAG_RECENT *pContact, CChatRecord oChatRecord);
	void RefreshLocalContactInfo(TAG_RECENT *pContact);

	void CleanUnReadNumByItem(HTREEITEM itemContact);
	HTREEITEM FindItemByContactDB(TAG_RECENT *pContact);
	vector<HTREEITEM> FindGroupItemByContactDB(TAG_RECENT *pContact);
	void ReListItem();
	void CreateGroupContact(TAG_RECENT *pContactInfo);
	bool ChangeContact(TAG_RECENT* pContactInfo);
	void ChangeContactByInside(TAG_RECENT* pContactInfo);
	void AutoCleanMsgOfTreeItem();
	void UpdateContactInfoInTree(TAG_RECENT *pContactInfo);
	void UpdateContactInfoInChat(TAG_RECENT *pContactInfo);
	void UpdateGroupInfo(TAG_RECENT *pContactInfo);
	void UpdateSingleContactInfo(TAG_RECENT *pContactInfo);
	void UpdateMultiContatctInfo(TAG_RECENT *pContactInfo);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);


	bool IsCalling(CChatTabControlHandler* pTabControlHandler);
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnCall(RoutedEventArgs *pe);
	LRESULT OnBtnMeetings(RoutedEventArgs *pe);
	LRESULT OnBtnSpeaker(RoutedEventArgs *pe);
	LRESULT OnBtnPrev(RoutedEventArgs *pe);
	LRESULT OnBtnNext(RoutedEventArgs *pe);
	LRESULT OnBtnRefresh(RoutedEventArgs *pe);
	LRESULT OnBtnSearch(RoutedEventArgs *pe);
	LRESULT  OnBtnMyFace(RoutedEventArgs *pe);
	LRESULT OnBtnFaceMenu(RoutedEventArgs *pe);
	LRESULT OnRadInbox(RoutedEventArgs *pe);
	LRESULT OnRadContacts(RoutedEventArgs *pe);

	int InitRecentContactsTree();
	int InitPersonInfo();
	int UnselectContactTreeItem();
	LRESULT OnAfterPaint(RoutedEventArgs *pe);
	LRESULT OnSwitchToContact(RoutedEventArgs *pe);
	LRESULT OnSwitchToSingleContact(RoutedEventArgs *pe);
	LRESULT OnSelectItem(HTREEITEM hItem);
	LRESULT OnRClickRecentContactsTree(RoutedEventArgs *pe);

	void CreatePropertySheet(HTREEITEM hItem);
	
protected:
	void UpdateMsgStatusBySipNumber(HTREEITEM hItem);
	void ProcessSwitch2Contact(TAG_RECENT* pContactInfo, CHAT_TAB tabPage,RoutedEventArgs *pe);
	
protected:
	Canvas *m_pCanvasToolbar;
	Button *m_pBtnCall;
	Label *m_pStaToolbarSplitter;
	Button *m_pBtnMeetings;
	Button *m_pBtnSpeaker;
	Button *m_pBtnPrev;
	Button *m_pBtnNext;
	Button *m_pBtnRefresh;
	Button *m_pBtnSearch;
	Label *m_pStaSearchIcon;
	Label *m_pStaSearch;
	Label *m_pStaContact;
	Label *m_pStaContactState;
	Button *m_pBtnMyFace;
	Button *m_pBtnFaceMenu;
	TabControl *m_pTabMain;
	Canvas *m_pCanvasNavigate;
	Button *m_pRadInbox;
	Button *m_pRadContacts;
	Label *m_pStaSplitter;
	Label *m_pStaRecentContacts;
public:
	TreeView *m_pTreeRecentContacts;
	CContactsCanvasHandler *m_pContactsCanvasHandler;
protected:
	vector<CChatTabControlHandler*> m_vecChatTabControlHandler;
	TAG_RECENT * m_pLastContactInfo;
	TAG_RECENT * m_pSrcContactInfo;
};

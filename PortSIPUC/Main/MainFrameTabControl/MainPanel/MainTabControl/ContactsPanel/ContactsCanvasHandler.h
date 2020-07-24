#pragma once
enum TabContactPage
{
	ENUM_FAVORITE_PAGE = 0,
	ENUM_LOCAL_CONTACT_PAGE,
	ENUM_REMOTE_CONTACT_PAGE,
	ENUM_EXTERSION_PAGE
};
class CContactsCanvasHandler : public CWLWndHandler
{
public:
	CContactsCanvasHandler();
	virtual ~CContactsCanvasHandler();

	void InitContacts();
	void InitFavorites();
	void InitExtersions();
	void InitRemoteContacts();


	void SearchFromContactOfRemoteContact(std::vector<CContactDB*> &vecContact, std::string strKeyString);
	void SearchFromContactOfExtension(std::vector<CContactDB*> &vecContact, std::string strKeyString);
	void UpdateContacts();
	void UpdateFavorites();
	void UpdateExtersion();
	void UpdateRemote();

	void UpdateContactByContact(CContactDB* pConactDB);
	void UpdateFavoriteByContact(CContactDB* pConactDB);
	void RefreshLocalContactOnlineStatus(CContactDB* pContactDB);
protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	int InitContactsFavorites(RoutedEventArgs *pe);
	int InitContactsLocalContacts(RoutedEventArgs *pe);
	int InitContactsRemoteContacts(RoutedEventArgs *pe);
	int InitContactsExtensions(RoutedEventArgs *pe);

	

	int OnCreate(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanged(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanging(RoutedEventArgs *pe);
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);

	LRESULT OnRadFavorites(RoutedEventArgs *pe);
	LRESULT OnRadLocalContacts(RoutedEventArgs *pe);
	LRESULT OnRadRemoteContacts(RoutedEventArgs *pe);
	LRESULT OnRadExtensions(RoutedEventArgs *pe);
	LRESULT OnItemBtnAudio(RoutedEventArgs *pe);
	LRESULT OnItemBtnVideo(RoutedEventArgs *pe);
	LRESULT OnItemBtnMore(RoutedEventArgs *pe);
	LRESULT OnLButtonDblClk(RoutedEventArgs *pe);
	LRESULT OnBtnFirstPage(RoutedEventArgs *pe);
	LRESULT OnBtnPrePage(RoutedEventArgs *pe);
	LRESULT OnBtnNextPage(RoutedEventArgs *pe);
	LRESULT OnBtnLastPage(RoutedEventArgs *pe);
	// Message handler
	LRESULT OnAfterPaint(RoutedEventArgs *pe);

	void  GetAllRemoteContactsChildNodes();
	void  SearchRemoteContactsFromTree(HTREEITEM hParent, vector<CContactDB *> &vecContact);
	/************************************************************************/
	/************************************************************************/
	void Swtich2Favorite(bool bCheck= true,bool bTipsCheckSwitch = true);
	void Swtich2Contact(bool bCheck = true, bool bTipsCheckSwitch = true);
	void Swtich2RemoteContact(bool bCheck = true, bool bTipsCheckSwitch = true);
	void Swtich2Extension(bool bCheck = true, bool bTipsCheckSwitch = true);
public:
	/************************************************************************/
	/************************************************************************/
	void Insert2Favorite(TAG_RECENT* pContact);
	void Insert2Contact(TAG_RECENT* pConatct);
	void Insert2DBContact(TAG_RECENT *pContact);
	void Insert2RemoteContact(TAG_RECENT* pConatct);
	void Insert2Extension(TAG_RECENT* pConatct);
	/************************************************************************/
	/************************************************************************/
	void EraseItemFavorite(std::string strSipNumber, int nUserID);
	void EraseItemContact(std::string strSipNumber, int nUserID);
	void EraseItemRemoteContact(long long nDataID);
	void EraseItemExtension(long long nDataID);


	void EraseItemRemoteContact();
	void EraseItemExtension();
	void ProcessMenuItemMsg(int nItemID, TreeView *pTreeView, HTREEITEM hItem);
protected:
	Button *m_pRadFavorites;
	Button *m_pRadLocalContacts;
	Button *m_pRadRemoteContacts;
	Button *m_pRadExtensions;
	TabControl *m_pContacts;
	Label *m_pStaSplitter;
	Button *m_pBtnFirstPage;
	Button *m_pBtnPrePage;
	Button *m_pBtnNextPage;
	Button *m_pBtnLastPage;
protected:
	TreeView *m_pTvFavorites;
	TreeView *m_pTvLocalContacts;
	TreeView *m_pTvRemoteContacts;
	TreeView *m_pTvExtensions;

protected:
	HWLWND m_hOriginalSenderFavorites;
	HWLWND m_hOriginalSenderLocal;
	HWLWND m_hOriginalSenderRemote;
	HWLWND m_hOriginalSenderExtensions;
private:
	vector<TAG_RECENT *> m_vecFavoriteContact;
	vector<TAG_RECENT *> m_vecLocalContact;
	vector<TAG_RECENT *> m_vecDBLocalContact;
	vector<TAG_RECENT *> m_vecRemoteContact;
	vector<TAG_RECENT *> m_vecExtensionsContact;

	int m_nFavoritePageMax;
	int m_nLocalContactPageMax;
	int m_nRemoteContactPageMax;
	int m_nRemoteExtensionPageMax;

	int m_nFavoritePage;
	int m_nLocalContactPage;
	int m_nRemoteContactPage;
	int m_nRemoteExtensionPage;
	TabContactPage m_nTabPage;
	TabContactPage m_nLastTabPage;
};

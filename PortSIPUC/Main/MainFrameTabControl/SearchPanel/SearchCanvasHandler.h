#pragma once

enum SEARCH_PANEL
{
	SP_EMPTY = 1,
	SP_FOUND,
	SP_UNFOUND
};

class CSearchCanvasHandler : public CWLWndHandler
{
public:
	CSearchCanvasHandler();
	virtual ~CSearchCanvasHandler();

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	int UpdateSearchResult(LPCTSTR lpszSearchText);
	//TODO:SAVE CONTACT 
	int InsertContactItem(HTREEITEM hParent, CControlProp *pBindProp, TAG_RECENT *pContact);
	int InsertMessageItem(
		HTREEITEM hParent,
		CControlProp *pBindProp,
		LPCTSTR lpszIconFile,
		LPCTSTR lpszMsgTitle,
		LPCTSTR lpszMsgTime,
		LPCTSTR lpszMsgContent
	);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnCloseSearchPage(RoutedEventArgs *pe);
	LRESULT OnCommand(RoutedEventArgs *pe);
	LRESULT OnAfterPaintBkImage(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanged(RoutedEventArgs *pe);
	LRESULT OnLButtonDblClk(RoutedEventArgs *pe);
	LRESULT OnItemBtnAudio(RoutedEventArgs *pe);
	LRESULT OnItemBtnVideo(RoutedEventArgs *pe);
	LRESULT OnItemBtnMore(RoutedEventArgs *pe);
	LRESULT OnTreeViewHotChanging(RoutedEventArgs *pe);
protected:
	Label *m_pStaSearchTitle;
	Button *m_pBtnCloseSearchPage;
	RichTextBox *m_pReSearch;
	TreeView *m_pTreeSerachResult;
};

#pragma once
#include "AddCallerCanvasHandler.h"

enum ADDCALLER_PAGE
{
	AP_SEARCH = 1,
	AP_ASK,
	AP_ADD_CALLER 
};
enum ENUM_CONFENCE_TYPE
{
	ENUM_CONFENCE_AUDIO,
	ENUM_CONFENCE_VIDEO
};

// IDD_CALLING_TRANSFER
class CAddCallerDlg : public Window
{
public:
	CAddCallerDlg(CWLWndHandler *pSrcCallingPage, ENUM_CONFENCE_TYPE enumConfenceType, CHAT_TAB enumChatTab = CT_AUDIO);
	virtual ~CAddCallerDlg();
	void SetCurrentContact(TAG_RECENT *pContact)
	{
		m_pSrcContact = pContact;
	};
	void DoAddedContact();
	void SetType(CHAT_TAB typeTab) { m_typeTab = typeTab; };
	void CleanCalingPageData();
	//tab
	CHAT_TAB GetAddType() { return m_typeTab; };
	CWLWndHandler* GetSrcCalingPage() {
		return m_pSrcCalingPage;
	};
	CWLWndHandler*  GetDesPagePointer(CWLWndHandler *pDesCalingPage)
	{
		return m_pDesCalingPage;
	}
	void SetDesPagePointer(CWLWndHandler *pDesCalingPage) { m_pDesCalingPage = pDesCalingPage; };

	ENUM_RECENT_CONTACT_TYPE GetSrcRecentContactType();
protected:
	int InitControls();
	int InitControlData();
	void DoAdd(std::string strNum);
	bool DoAskFirst();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);
	LRESULT OnBtnGo(RoutedEventArgs *pe);
	LRESULT OnBtnOk(RoutedEventArgs *pe);
	LRESULT OnBtnCancelAddCaller(RoutedEventArgs *pe);
	LRESULT OnItemDBClick(RoutedEventArgs * pe);

	LRESULT OnAfterPaint(RoutedEventArgs *pe);

protected:
	int UpdateSearchResult(LPCTSTR lpszSearchText);
	bool DoSelectContactOk();
	HTREEITEM InsertContactItem(
		CControlProp *pBindProp,
		LPCTSTR lpszIconFile,
		LPCTSTR lpszName,
		LPCTSTR lpszPhone,
		const CContactDB *pContact
	);

	LRESULT OnTabSwitched(RoutedEventArgs *pe);
	LRESULT OnCommand(RoutedEventArgs *pe);
	LRESULT OnNotify(RoutedEventArgs *pe);
	
protected:
	Canvas *m_pCanvasMain;
	Label *m_pStaTitle;
	Button *m_pBtnClose;
	Label *m_pStaSplitter1;
	TabControl *m_pTabTransfer;
	Label *m_pCaller;
protected:
	RichTextBox *m_pReSearch;
	Button *m_pBtnGo;
	TreeView *m_pTvResult;
	Button *m_pBtnTransferAsk;
	Button *m_pBtnTransferNow;
	TAG_RECENT *m_pSrcContact;
	TAG_RECENT *m_pAddContactInfo;
	ENUM_CONFENCE_TYPE m_enumConfenceType;
	std::vector<TAG_RECENT*> m_vecContact;
	HWND         m_hRemoteHwnd;
	CAddCallerCanvasHandler *m_pAddCallerCanvasHandler;
	ADDCALLER_PAGE m_enumPage;
	CHAT_TAB        m_typeTab;
	CWLWndHandler *m_pSrcCalingPage;
	//
	CWLWndHandler *m_pDesCalingPage;
};

#pragma once
#include "CommonDef.h"

enum TRANSFER_PAGE
{
	TP_SEARCH = 1,
	TP_ASK,
	TP_TIPS_PAGE,
};
enum TRANSFER_TYPE
{
	NO_CONDITION,
	ASK_FIRST
};
enum ASK_TRANSFER_STEP
{
	STEP_ASK ,
	STEP_SUCC
};
class CContactDB;
// IDD_CALLING_TRANSFER
class CCallingTransferDlg : public Window
{
public:
	CCallingTransferDlg(CWLWndHandler *pSrcCallingPage,CHAT_TAB enumChatTab = CT_AUDIO);
	virtual ~CCallingTransferDlg();
	void SetTransferedContact(TAG_RECENT *pContactInfo)
	{
		m_pTransferedContactInfo = pContactInfo;
	};
	void SetType(CHAT_TAB typeTab) { m_typeTab = typeTab; };
	void ProcessTranseferSuccess(bool bSuccess, int nErrorCode = 0);
	void ProcessTransferByRemote(bool bSuccess);
	void OnBtnCancle();
	//void DoAttendTransfer();
	void DoAttendTransferOk();

	CHAT_TAB GetTransferType() { return m_typeTab; };


	void SetDesPagePointer(CWLWndHandler *pDesCalingPage) { m_pDesCalingPage = pDesCalingPage; };
	CWLWndHandler* GetSrcCalingPage() {
		return m_pSrcCalingPage;
	};
	CWLWndHandler*  GetDesPagePointer(CWLWndHandler *pDesCalingPage)
	{
		return m_pDesCalingPage;
	}

	void CleanCalingPageData();

	void SetParentHwnd(HWND hWnd)
	{
		m_hSetParentHwnd = hWnd;
	}
protected:
	virtual BOOL CreateEx(
		DWORD dwExStyle,
		LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU nIDorHMenu,
		LPVOID lpParam);

	int InitControls();
	int InitControlData();
	
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnItemDBClick(RoutedEventArgs * pe);
	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);

protected:
	int UpdateSearchResult(LPCTSTR lpszSearchText);
	HTREEITEM InsertContactItem(
		CControlProp *pBindProp,
		LPCTSTR lpszIconFile,
		LPCTSTR lpszName,
		LPCTSTR lpszPhone,
		const CContactDB *pContact
	);

	void RefreshTipsPage();
	LRESULT OnTabSwitched(RoutedEventArgs *pe);
	LRESULT OnCommand(RoutedEventArgs *pe);
	LRESULT OnNotify(RoutedEventArgs *pe);
	LRESULT OnBtnGo(RoutedEventArgs *pe);


	LRESULT OnAfterPaint(RoutedEventArgs *pe);
	void DoNext(bool bClick = false);
	void DoTransfer(std::string strNum);
	bool DoAskFirst();
	bool  DoSelectContactOk();
protected:
	Canvas *m_pCanvasMain;
	Label *m_pStaTitle;
	Button *m_pBtnClose;
	Label *m_pStaSplitter1;
	TabControl *m_pTabTransfer;

protected:
	RichTextBox *m_pReSearch;
	TreeView *m_pTvResult;
	Button *m_pBtnTransferNow;
	Button *m_pBtnAskFirst;
	Button *m_pBtnGo;
	Button *m_pBtnCancle;
	Button *m_pBtnTransferOK;
	Label  *m_pInfoTansfer;
	Button *m_pBtnOK;
	// A->B中的B
	TAG_RECENT *m_pTransferContactInfo;
	// A->B中的A
	TAG_RECENT *m_pTransferedContactInfo;
	CHAT_TAB      m_typeTab;
	TRANSFER_TYPE m_enumTransferType;
	bool		 m_bClickTree;
	CWLWndHandler *m_pSrcCalingPage;
	CWLWndHandler *m_pDesCalingPage;
	TRANSFER_PAGE m_enumPage;

	HWND  m_hSetParentHwnd;
	ASK_TRANSFER_STEP   m_enumAskTransferStep;
};

#pragma once


// IDW_TEXT_CHAT_PAGE
class CTextChatPageCanvasHandler : public CWLWndHandler
{
public:
	CTextChatPageCanvasHandler();
	virtual ~CTextChatPageCanvasHandler();

	void UpdateContactInfoInChat(TAG_RECENT *pContactInfo);

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnFace(RoutedEventArgs *pe);
	LRESULT OnBtnPhone(RoutedEventArgs *pe);
	LRESULT OnBtnVideo(RoutedEventArgs *pe);
	LRESULT OnBtnProfile(RoutedEventArgs *pe);

	LRESULT OnAfterPaint(RoutedEventArgs *pe);

public:
	int SetContactInfo(TAG_RECENT *pContactInfo);
	TAG_RECENT *GetContactInfo();

protected:
	Button *m_pBtnFace;
	Label *m_pStaAccount;
	Label *m_pStaSign;
	Button *m_pBtnPhone;
	Button *m_pBtnVideo;
	Button *m_pBtnProfile;
	Label *m_pStaSplitter;
	Canvas *m_pCanvasChat;

protected:
	HWLWND m_hProfileWnd;
	TAG_RECENT *m_pContactInfo;
};

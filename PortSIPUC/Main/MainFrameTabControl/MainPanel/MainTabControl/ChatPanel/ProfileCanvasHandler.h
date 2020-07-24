#pragma once

class CProfileCanvasHandler : public CWLWndHandler
{
public:
	CProfileCanvasHandler();
	virtual ~CProfileCanvasHandler();

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);

	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnCloseProfile(RoutedEventArgs *pe);
	LRESULT OnBtnViewAllEmails(RoutedEventArgs *pe);

public:
	int SetContact(CContactDB *pContact);
	CContactDB *GetContact();

protected:
	Label *m_pStaUserName;
	Button *m_pBtnCloseProfile;
	Label *m_pStaFace;
	Label *m_pStaAboutTitle;
	Label *m_pStaAbout;
	Label *m_pStaWorkTitle;
	Label *m_pStaWork;
	Label *m_pStaEmailTitle;
	Label *m_pStaEmail;
	Label *m_pStaSplitter1;
	Label *m_pStaSharedNoteTitle;
	Label *m_pStaSharedNote;
	Label *m_pStaSplitter2;
	Label *m_pStaEmailsTitle;
	Button *m_pBtnViewAllEmails;
	Label *m_pSta14;
	Label *m_pStaSpiltter3;

protected:
	CContactDB *m_pContact;
};

#pragma once


// IDD_CALLER_ADD
class CAddCallerCanvasHandler : public CWLWndHandler
{
public:
	CAddCallerCanvasHandler();
	virtual ~CAddCallerCanvasHandler();

public:
	int SetContact(CContactDB *pContactInfo);

protected:
	int InitControls();
	int OnInitPanel(RoutedEventArgs *pe);
	
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnAddCallerCancel(RoutedEventArgs *pe);
	LRESULT OnPropertySheetShow(RoutedEventArgs *pe);
	LRESULT OnAfterPaint(RoutedEventArgs *pe);

protected:
	Label *m_pStaAddCallerTitle;
	Label *m_pStaAddCallerFace;
	Button *m_pBtnAddCallerCancel;

protected:
	CContactDB *m_pContactInfo;
};

#pragma once

enum ENUM_CONTACT_TYPE
{
	ENUM_ADD_CONTACT = 0,
	ENUM_ADD_EXTERSION,
	ENUM_UPDATE_CONTACT
};
// IDW_ADD_CONTACT
class CAddContactWnd : public Window
{
public:
	CAddContactWnd(ENUM_CONTACT_TYPE enumContactType ,TAG_RECENT* pContact = NULL);
	virtual ~CAddContactWnd();

protected:
	int InitControls();
	bool VerifyData(CIUIString &strErrorInfo);
	void InitControlsData();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnDelete(RoutedEventArgs *pe);
	LRESULT OnBtnDelete2(RoutedEventArgs *pe);
	LRESULT OnBtnAddSipNumber(RoutedEventArgs *pe);
	LRESULT OnBtnReturn(RoutedEventArgs *pe);
	LRESULT OnBtnSave(RoutedEventArgs *pe);
	LRESULT OnBtnMin(RoutedEventArgs *pe);
	LRESULT OnBtnClose(RoutedEventArgs *pe);

	void SaveToDB();
protected:
	Canvas *m_pCanvasAddContact;
	Label *m_pStaTitle;
	//Label *m_pStaFace;
	Label *m_pStaFirstName;
	RichTextBox *m_pReFirstName;
	Label *m_pStaLastName;
	RichTextBox *m_pReLastName;
	Label *m_pStaEmailAddress;
	RichTextBox *m_pReEmailAddress;
	Label *m_pStaHomeNumber;
	RichTextBox *m_pReHomeNumber;
	Label *m_pStaMobileNumber;
	RichTextBox *m_pReMobileNumber;
	Label *m_pStaWorkNumber;
	RichTextBox *m_pReWorkNumber;
	Label *m_pStaSipNumber;
	RichTextBox *m_pReSipNumber;
	Button *m_pBtnDelete;
	RichTextBox *m_pReSipNumber2;
	Button *m_pBtnDelete2;
	Button *m_pBtnAddSipNumber;
	Button *m_pBtnReturn;
	Button *m_pBtnSave;
	Button *m_pBtnMin;
	Button *m_pBtnClose;
private:
	TAG_RECENT*   m_pContactInfo;
	ENUM_CONTACT_TYPE m_enumContactType;
	bool m_bExistNumber;
};

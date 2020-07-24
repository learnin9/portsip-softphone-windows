#include "stdafx.h"
#include "AddContactWnd.h"
#include "System/SystemEx.h"

CAddContactWnd::CAddContactWnd(ENUM_CONTACT_TYPE enumContactType , TAG_RECENT* pContacInfo)
	: m_pCanvasAddContact(NULL)
	, m_pStaTitle(NULL)
	//, m_pStaFace(NULL)
	, m_pStaFirstName(NULL)
	, m_pReFirstName(NULL)
	, m_pStaLastName(NULL)
	, m_pReLastName(NULL)
	, m_pStaEmailAddress(NULL)
	, m_pReEmailAddress(NULL)
	, m_pStaHomeNumber(NULL)
	, m_pReHomeNumber(NULL)
	, m_pStaMobileNumber(NULL)
	, m_pReMobileNumber(NULL)
	, m_pStaWorkNumber(NULL)
	, m_pReWorkNumber(NULL)
	, m_pStaSipNumber(NULL)
	, m_pReSipNumber(NULL)
	, m_pBtnDelete(NULL)
	, m_pReSipNumber2(NULL)
	, m_pBtnDelete2(NULL)
	, m_pBtnAddSipNumber(NULL)
	, m_pBtnReturn(NULL)
	, m_pBtnSave(NULL)
	, m_pBtnMin(NULL)
	, m_pBtnClose(NULL)
{
	m_pContactInfo = new TAG_RECENT();
	CContactDB *pContact = new CContactDB;
	m_pContactInfo->m_enumRecentContact = ENUM_SINGLE_CONTACT;
	m_pContactInfo->m_unionContactInfo.m_pContact = pContact;
	m_enumContactType = enumContactType;
	if (pContacInfo ==NULL)
	{//新增模式下是没有ContactDB指针的,所以先构建一个空的联系人指针
		m_bExistNumber = false;
	}
	else
	{//暂时不允许修改sipnumber
		m_pContactInfo->m_unionContactInfo.m_pContact->m_strFirstName = pContacInfo->m_unionContactInfo.m_pContact->m_strFirstName;
		m_pContactInfo->m_unionContactInfo.m_pContact->m_strLastName = pContacInfo->m_unionContactInfo.m_pContact->m_strLastName;
		m_pContactInfo->m_unionContactInfo.m_pContact->m_strEmailAdress = pContacInfo->m_unionContactInfo.m_pContact->m_strEmailAdress;
		m_pContactInfo->m_unionContactInfo.m_pContact->m_strSIPNum = pContacInfo->m_unionContactInfo.m_pContact->m_strSIPNum;
		m_pContactInfo->m_unionContactInfo.m_pContact->m_bFavorites = pContacInfo->m_unionContactInfo.m_pContact->m_bFavorites;
		m_bExistNumber = true;
	}
}

CAddContactWnd::~CAddContactWnd()
{
}

int CAddContactWnd::InitControls()
{
	m_pCanvasAddContact = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_ADD_CONTACT")));
	_ASSERT(m_pCanvasAddContact != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	//m_pStaFace = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_FACE")));
	//_ASSERT(m_pStaFace != NULL);
	m_pStaFirstName = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_FIRST_NAME")));
	_ASSERT(m_pStaFirstName != NULL);
	m_pReFirstName = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_FIRST_NAME")));
	_ASSERT(m_pReFirstName != NULL);
	m_pStaLastName = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_LAST_NAME")));
	_ASSERT(m_pStaLastName != NULL);
	m_pReLastName = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_LAST_NAME")));
	_ASSERT(m_pReLastName != NULL);
	m_pStaEmailAddress = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_EMAIL_ADDRESS")));
	_ASSERT(m_pStaEmailAddress != NULL);
	m_pReEmailAddress = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_EMAIL_ADDRESS")));
	_ASSERT(m_pReEmailAddress != NULL);
	m_pStaHomeNumber = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_HOME_NUMBER")));
	_ASSERT(m_pStaHomeNumber != NULL);
	m_pReHomeNumber = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_HOME_NUMBER")));
	_ASSERT(m_pReHomeNumber != NULL);
	m_pStaMobileNumber = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_MOBILE_NUMBER")));
	_ASSERT(m_pStaMobileNumber != NULL);
	m_pReMobileNumber = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_MOBILE_NUMBER")));
	_ASSERT(m_pReMobileNumber != NULL);
	m_pStaWorkNumber = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_WORK_NUMBER")));
	_ASSERT(m_pStaWorkNumber != NULL);
	m_pReWorkNumber = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_WORK_NUMBER")));
	_ASSERT(m_pReWorkNumber != NULL);
	m_pStaSipNumber = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SIP_NUMBER")));
	_ASSERT(m_pStaSipNumber != NULL);
	m_pReSipNumber = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_SIP_NUMBER")));
	_ASSERT(m_pReSipNumber != NULL);
	//m_pBtnDelete = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_DELETE")));
	//_ASSERT(m_pBtnDelete != NULL);
	//m_pReSipNumber2 = (RichTextBox *)CWLWnd::FromHandle(FindControl(_T("IDC_RE_SIP_NUMBER2")));
	//_ASSERT(m_pReSipNumber2 != NULL);
	//m_pBtnDelete2 = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_DELETE2")));
	//_ASSERT(m_pBtnDelete2 != NULL);
	//m_pBtnAddSipNumber = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_ADD_SIP_NUMBER")));
	//_ASSERT(m_pBtnAddSipNumber != NULL);
	m_pBtnReturn = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_RETURN")));
	_ASSERT(m_pBtnReturn != NULL);
	m_pBtnSave = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_SAVE")));
	_ASSERT(m_pBtnSave != NULL);
	//m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	//_ASSERT(m_pBtnMin != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	//m_pBtnDelete->ShowWindow(SW_HIDE);
	//m_pBtnAddSipNumber->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT CAddContactWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();
	//if (m_enumContactType==ENUM_UPDATE_CONTACT||m_enumContactType== ENUM_ADD_CONTACT)
	{
		InitControlsData();
	}
	CenterWindow();
	return 0;
}

LRESULT CAddContactWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CAddContactWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CAddContactWnd::OnBtnDelete(RoutedEventArgs *pe)
{
	m_pReSipNumber->SetWindowText(L"");
	return 0;
}

LRESULT CAddContactWnd::OnBtnDelete2(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddContactWnd::OnBtnAddSipNumber(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddContactWnd::OnBtnReturn(RoutedEventArgs *pe)
{
	DestroyWindow(m_hWnd);
	delete this;
	pe->bHandled = TRUE;
	pe->lResult = 1;
	return 0;
}

void CAddContactWnd::InitControlsData()
{
	if (m_pContactInfo == NULL)
	{
		return ;
	}
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	CIUIString strFirstName = L"", strSecondName = L"", strEmailAddress = L"", \
		strHomeNumber = L"", strMobileNumber = L"", strWorkNum = L"", strSipNumber = L"";
	if (pContactDB->m_strSIPNum.empty() != true)
	{
		strSipNumber = PortUtility::Utf82Unicode_ND(pContactDB->m_strSIPNum).c_str();
	}
	if (pContactDB->m_strFirstName.empty() != true)
	{
		strFirstName = PortUtility::Utf82Unicode_ND(pContactDB->m_strFirstName).c_str();
	}
	if (pContactDB->m_strLastName.empty() != true)
	{
		strSecondName = PortUtility::Utf82Unicode_ND(pContactDB->m_strLastName).c_str();
	}
	if (pContactDB->m_strEmailAdress.empty()!=true)
	{
		strEmailAddress = PortUtility::Utf82Unicode_ND(pContactDB->m_strEmailAdress).c_str();
	}
	if (pContactDB->m_strHomeNumber.empty() != true)
	{
		strHomeNumber = PortUtility::Utf82Unicode_ND(pContactDB->m_strHomeNumber).c_str();
	}
	if (pContactDB->m_strMobilePhone.empty() != true)
	{
		strMobileNumber = PortUtility::Utf82Unicode_ND(pContactDB->m_strMobilePhone).c_str();
	}
	if (pContactDB->m_strWorkPhone.empty() != true)
	{
		strWorkNum = PortUtility::Utf82Unicode_ND(pContactDB->m_strWorkPhone).c_str();
	}
	  m_pReFirstName->SetWindowText(strFirstName);
	  m_pReLastName->SetWindowText(strSecondName);
	  m_pReEmailAddress->SetWindowText(strEmailAddress);
	  m_pReHomeNumber->SetWindowText(strHomeNumber);
	  m_pReMobileNumber->SetWindowText(strMobileNumber);
	  m_pReWorkNumber->SetWindowText(strWorkNum);
	  m_pReSipNumber->SetWindowText(strSipNumber);
	  if (m_bExistNumber )
	  {
		  EnableWindow(m_pReSipNumber->GetSafeHwnd(), false);
	  }
}
LRESULT CAddContactWnd::OnBtnSave(RoutedEventArgs *pe)
{
	if (m_pContactInfo ==NULL)
	{
		return 0;
	}
	CContactDB *pContactDB = m_pContactInfo->m_unionContactInfo.m_pContact;
	CIUIString theErrorInfo;
	bool bRet = VerifyData(theErrorInfo);
	if (bRet==false)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("ERROR"), theErrorInfo, GetSafeHwnd());
		return 0;
	}
	CIUIString strFirstName, strSecondName, strEmailAddress, \
		strHomeNumber, strMobileNumber, strWorkNum, strSipNumber;
	
	strFirstName = m_pReFirstName->GetWindowText();
	strSecondName = m_pReLastName->GetWindowText();
	strEmailAddress = m_pReEmailAddress->GetWindowText();
	strHomeNumber = m_pReHomeNumber->GetWindowText();
	strMobileNumber = m_pReMobileNumber->GetWindowText();
	strWorkNum = m_pReWorkNumber->GetWindowText();
	strSipNumber = m_pReSipNumber->GetWindowText();
	pContactDB->m_nUserID = CSystemEx::GetInstance()->GetGlobalUser()->GetUID();
	wstring strBuffer = strFirstName.GetBuffer(strFirstName.GetLength());
	pContactDB->m_strFirstName = PortUtility::Unicode2Utf8_ND(strBuffer);
	strBuffer = strSecondName.GetBuffer(strSecondName.GetLength());
	pContactDB->m_strLastName = PortUtility::Unicode2Utf8_ND(strBuffer);
	if (!strEmailAddress.IsEmpty())
	{
		strBuffer = strEmailAddress.GetBuffer(strEmailAddress.GetLength());
		pContactDB->m_strEmailAdress = PortUtility::Unicode2Utf8_ND(strBuffer);
	}
	if (!strHomeNumber.IsEmpty())
	{
		strBuffer = strHomeNumber.GetBuffer(strHomeNumber.GetLength());
		pContactDB->m_strHomeNumber = PortUtility::Unicode2Utf8_ND(strBuffer);
	}
	if (!strMobileNumber.IsEmpty())
	{
		strBuffer = strMobileNumber.GetBuffer(strMobileNumber.GetLength());
		pContactDB->m_strMobilePhone = PortUtility::Unicode2Utf8_ND(strBuffer);
	}
	if (!strWorkNum.IsEmpty())
	{
		strBuffer = strWorkNum.GetBuffer(strWorkNum.GetLength());
		pContactDB->m_strWorkPhone = PortUtility::Unicode2Utf8_ND(strBuffer);
	}
	if (!strSipNumber.IsEmpty())
	{
		strSipNumber.Delete(_T(' '));
		strBuffer = strSipNumber.GetBuffer(strSipNumber.GetLength());
		pContactDB->m_strSIPNum = PortUtility::Unicode2Utf8_ND(strBuffer);
	}
	int  nRand = rand() % 6;
	CIUIString strImage;
	strImage.Format(_T("%sSkins\\PortGo\\bg_user.png"), GetRunFolder().GetBuffer(0));
	//strImage.Format(_T("%sSkins\\PortGo\\%d.png"), GetRunFolder().GetBuffer(0), nRand);
	std::wstring strImagePath = strImage;
	pContactDB->m_strHeadImage = PortUtility::Unicode2Utf8_ND(strImagePath);
	
	SaveToDB();

	DestroyWindow(m_hWnd);
	delete this;
	pe->bHandled = TRUE;
	pe->lResult = 1;
	return 0;
}

LRESULT CAddContactWnd::OnBtnMin(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CAddContactWnd::OnBtnClose(RoutedEventArgs *pe)
{
	DestroyWindow(m_hWnd);
	delete this;
	pe->bHandled = TRUE;
	pe->lResult = 1;
	return 1;
}

LRESULT CAddContactWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnDelete->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnDelete(pe);
	}
	else if (m_pBtnDelete2->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnDelete2(pe);
	}
	else if (m_pBtnAddSipNumber->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnAddSipNumber(pe);
	}
	else if (m_pBtnReturn->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnReturn(pe);
	}
	else if (m_pBtnSave->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSave(pe);
	}
	else if (m_pBtnMin->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnMin(pe);
	}
	else if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}

	return 0;
}


bool CAddContactWnd::VerifyData(CIUIString &strErrorInfo)
{
	bool bRet = false;
	CIUIString strFormatInfo = m_pReFirstName->GetWindowText();
	if (strFormatInfo.IsEmpty())
	{
		strErrorInfo = _T("First Name Is Empty");
		return false;
	}

	strFormatInfo = m_pReLastName->GetWindowText();
	if (strFormatInfo.IsEmpty())
	{
		strErrorInfo = _T("Last Name Is Empty");
		return false;
	}
	strFormatInfo = m_pReEmailAddress->GetWindowText();
	if (!strFormatInfo.IsEmpty())
	{
		bool bEmailFormatOk = PortUtility::CheckEmailAddress(strFormatInfo);
		if (bEmailFormatOk==false)
		{
			strErrorInfo = _T("Incorrect mailbox format");
			return false;
		}
	}
	return true;
}

void CAddContactWnd::SaveToDB()
{
	string strErrorMsg;
	if (m_enumContactType==ENUM_ADD_CONTACT||m_enumContactType==ENUM_ADD_EXTERSION)
	{
		
		bool bRet = CSystemEx::GetInstance()->GetDBOprate()->addOneContact(m_pContactInfo->m_unionContactInfo.m_pContact, strErrorMsg);
		if (bRet)
		{
			CSystemEx::GetInstance()->GetMainDlg()->Add2DBContact(m_pContactInfo);
		}
		else
		{
			CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(_T("ERROR"), PortUtility::Utf82Unicode_ND(strErrorMsg).c_str(), GetSafeHwnd());
		}
	}
	else if(m_enumContactType==ENUM_UPDATE_CONTACT)
	{
		bool bRet = CSystemEx::GetInstance()->GetDBOprate()->updateContact(m_pContactInfo->m_unionContactInfo.m_pContact);
		if (bRet)
		{
			OutputDebugString(L"UpdateContact Begin");
			CSystemEx::GetInstance()->GetMainDlg()->UpdateContact(m_pContactInfo);
			OutputDebugString(L"UpdateContact Begin");
		}
	}
	
}
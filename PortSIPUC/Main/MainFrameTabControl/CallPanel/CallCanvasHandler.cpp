#include "stdafx.h"
#include "CallCanvasHandler.h"


CCallCanvasHandler::CCallCanvasHandler()
	: m_pStaCallTitle(NULL)
	, m_pBtnCloseCallPage(NULL)
	, m_pReInput(NULL)
	, m_pCanvasKeyboard(NULL)
	, m_pBtnNum1(NULL)
	, m_pBtnNum2(NULL)
	, m_pBtnNum3(NULL)
	, m_pBtnNum4(NULL)
	, m_pBtnNum5(NULL)
	, m_pBtnNum6(NULL)
	, m_pBtnNum7(NULL)
	, m_pBtnNum8(NULL)
	, m_pBtnNum9(NULL)
	, m_pBtnNumxing(NULL)
	, m_pBtnNum0(NULL)
	, m_pBtnNumsharp(NULL)
	, m_pBtnAudio(NULL)
	, m_pBtnCalling(NULL)
	, m_pBtnVideo(NULL)
{
	m_strSipNumber = "";

}

CCallCanvasHandler::~CCallCanvasHandler()
{
}

int CCallCanvasHandler::InitControls()
{
	m_pStaCallTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_CALL_TITLE")));
	_ASSERT(m_pStaCallTitle != NULL);
	m_pBtnCloseCallPage = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_CLOSE_CALL_PAGE")));
	_ASSERT(m_pBtnCloseCallPage != NULL);
	m_pReInput = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_INPUT")));
	_ASSERT(m_pReInput != NULL);
	m_pCanvasKeyboard = (Canvas *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CANVAS_KEYBOARD")));
	_ASSERT(m_pCanvasKeyboard != NULL);
	m_pBtnNum1 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM1")));
	_ASSERT(m_pBtnNum1 != NULL);
	m_pBtnNum2 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM2")));
	_ASSERT(m_pBtnNum2 != NULL);
	m_pBtnNum3 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM3")));
	_ASSERT(m_pBtnNum3 != NULL);
	m_pBtnNum4 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM4")));
	_ASSERT(m_pBtnNum4 != NULL);
	m_pBtnNum5 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM5")));
	_ASSERT(m_pBtnNum5 != NULL);
	m_pBtnNum6 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM6")));
	_ASSERT(m_pBtnNum6 != NULL);
	m_pBtnNum7 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM7")));
	_ASSERT(m_pBtnNum7 != NULL);
	m_pBtnNum8 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM8")));
	_ASSERT(m_pBtnNum8 != NULL);
	m_pBtnNum9 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM9")));
	_ASSERT(m_pBtnNum9 != NULL);
	m_pBtnNumxing = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUMXING")));
	_ASSERT(m_pBtnNumxing != NULL);
	m_pBtnNum0 = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUM0")));
	_ASSERT(m_pBtnNum0 != NULL);
	m_pBtnNumsharp = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_NUMSHARP")));
	_ASSERT(m_pBtnNumsharp != NULL);
	m_pBtnAudio = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_AUDIO")));
	_ASSERT(m_pBtnAudio != NULL);
	m_pBtnCalling = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_CALLING")));
	_ASSERT(m_pBtnCalling != NULL);
	m_pBtnVideo = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_VIDEO")));
	_ASSERT(m_pBtnVideo != NULL);

	return 0;
}

int CCallCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	// 加上ENM_CHANGE就可以收到EN_CHANGE事件了。
	m_pReInput->SetEventMask(m_pReInput->GetEventMask() | ENM_CHANGE);

	return 0;
}

LRESULT CCallCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			// IDC_CANVAS_CALL自已的初始化
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
		else if (WM_COMMAND == pe->RoutedEvent)
		{
			OnCommand(pe);
		}
		else if (UE_AFTERPAINTBKIMAGE == pe->RoutedEvent)
		{
			OnAfterPaintBkImage(pe);
		}
	}
	if (RS_TUNNEL == pe->eRoutingStrategy)
	{
		if (WM_LBUTTONDBLCLK == pe->RoutedEvent)
		{
			if (GetClassName(pe->hOriginalSender) == IUI_BUTTON)
			{
				for (int i = 0; i <= 9; ++i)
				{
					CIUIString strID;
					CIUIString strIDNum;
					strIDNum.Format(_T("%d"), i);
					strID = _T("IDC_BTN_NUM");
					strID += strIDNum;

					if (GetName(pe->hOriginalSender) == strID)
					{
						m_pReInput->SetSel(-1, -1);
						m_pReInput->ReplaceSel(strIDNum);
						OnBtnDBClick(strIDNum);
					}
				}

				if (GetName(pe->hOriginalSender) == _T("IDC_BTN_NUMXING"))
				{
					m_pReInput->SetSel(-1, -1);
					m_pReInput->ReplaceSel(_T("*"));
					OnBtnDBClick(_T("*"));
				}
				else if (GetName(pe->hOriginalSender) == _T("IDC_BTN_NUMSHARP"))
				{
					m_pReInput->SetSel(-1, -1);
					m_pReInput->ReplaceSel(_T("#"));
					OnBtnDBClick(_T("#"));
				}
			}
		}
	}
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnCloseCallPage(RoutedEventArgs *pe)
{
	return 0;
}

void CCallCanvasHandler::OnBtnDBClick(CIUIString strBtn)
{
	char szBuffer[2];
	memcpy_s(szBuffer, sizeof(szBuffer), PortUtility::Unicode2Utf8_ND(strBtn.GetBuffer(strBtn.GetLength())).c_str(), strBtn.GetLength());
	char btn = szBuffer[0];
	CSystemEx::GetInstance()->BtnAudio(btn);
	Sleep(100);
	CSystemEx::GetInstance()->BtnAudio(btn);
}

LRESULT CCallCanvasHandler::OnBtnNum1(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"1";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('1');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum2(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"2";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('2');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum3(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"3";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('3');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum4(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"4";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('4');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum5(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"5";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('5');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum6(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"6";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('6');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum7(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"7";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('7');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum8(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"8";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('8');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum9(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"9";
	m_pReInput->SetWindowTextW(strREString);
	CSystemEx::GetInstance()->BtnAudio('9');
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNumxing(RoutedEventArgs *pe, CIUIString  &strREString)
{
	strREString += L"*";
	CSystemEx::GetInstance()->BtnAudio('*');
	m_pReInput->SetWindowTextW(strREString);
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNum0(RoutedEventArgs *pe, CIUIString & strREString)
{
	strREString += L"0";
	CSystemEx::GetInstance()->BtnAudio('0');
	m_pReInput->SetWindowTextW(strREString);
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnNumsharp(RoutedEventArgs *pe, CIUIString  & strREString)
{
	strREString += L"#";
	CSystemEx::GetInstance()->BtnAudio('#');
	m_pReInput->SetWindowTextW(strREString);
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnAudio(RoutedEventArgs *pe, CIUIString  strREString)
{
	
	string tempNumber = VOICE_MAIL_NUMBER;// PortUtility::wstring2String(strBuffer);

	//这里的逻辑应该是这样的-如果是语音电话，则停留在MTK_CALL页面即可
	//如果是拨打的联系人，那么应当跳转到联系人界面，如果么有联系人，那么久需要新建立一个联系人，并保持部分信息到数据库
	bool bRet = false;
	std::string strErrprInfo = "";
	int			nActiveSession = -1;
	long         lCallHistoryID = -1;

	//跳转到当前联系人的通话界面
	//CSystemEx::GetInstance()->GetMainDlg()->SwitchToPropertySheet();
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(tempNumber);
	if (pContactInfo == NULL)
	{
		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(tempNumber);
	}

	pContactInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_OUT);
	SwitchToContact(pContactInfo, CT_AUDIO);

	return 0;
}

LRESULT CCallCanvasHandler::OnBtnCalling(RoutedEventArgs *pe, CIUIString  strREString)
{
	if (strREString.IsEmpty())
	{
		return 0;
	}
	LPWSTR pBuffer = strREString.GetBuffer(strREString.GetLength());
	wstring strBuffer = pBuffer;
	string tempNumber = PortUtility::wstring2String(strBuffer);
	bool bIsNum = PortUtility::CheckExtersionNumber(strREString);
	if (bIsNum==false)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"please provide  an available number ", NULL);
		return 0;
	}

	//这里的逻辑应该是这样的-如果是语音电话，则停留在MTK_CALL页面即可
	//如果是拨打的联系人，那么应当跳转到联系人界面，如果么有联系人，那么久需要新建立一个联系人，并保持部分信息到数据库
	bool bRet = false;
	std::string strErrprInfo = "";
	int			nActiveSession = -1;
	long         lCallHistoryID = -1;
	
	//跳转到当前联系人的通话界面
	//CSystemEx::GetInstance()->GetMainDlg()->SwitchToPropertySheet();
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(tempNumber);
	if (pContactInfo == NULL)
	{
		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(tempNumber);
	}
	
	pContactInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_OUT);
	SwitchToContact(pContactInfo, CT_AUDIO);
	return 0;
}

LRESULT CCallCanvasHandler::OnBtnVideo(RoutedEventArgs *pe, CIUIString  strREString)
{
	if (strREString.IsEmpty())
	{
		return 0;
	}

	LPWSTR pBuffer = strREString.GetBuffer(strREString.GetLength());
	wstring strBuffer = pBuffer;
	string tempNumber = PortUtility::wstring2String(strBuffer);
	bool bIsNum = PortUtility::CheckExtersionNumber(strREString);
	if (bIsNum == false)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"please provide  an available number ", NULL);
		return 0;
	}
	//这里的逻辑应该是这样的-如果是语音电话，则停留在MTK_CALL页面即可
	//如果是拨打的联系人，那么应当跳转到联系人界面，如果么有联系人，那么久需要新建立一个联系人，并保持部分信息到数据库
	std::string strErrprInfo = "";
	TAG_RECENT *pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->FindContactDBBySipNumber(tempNumber);
	if (pContactInfo == NULL)
	{
		pContactInfo = CSystemEx::GetInstance()->GetMainDlg()->CreateNewContactBySipNumber(tempNumber);
	}
	pContactInfo->m_unionContactInfo.m_pContact->SetCallType(ENUM_CALL_OUT);
#if _DEBUG
	TIMER_START(SwitchToContact);
#endif
	SwitchToContact(pContactInfo, CT_VIDEO);
	
#if _DEBUG
	TIMER_STOP(SwitchToContact);
	CIUIString strFormatInfo;
	strFormatInfo.Format(L" WM_SWITCH_TO_CONTACT Cost %d ms.\n", TIMER_MSEC(SwitchToContact));
	OutputDebugString(strFormatInfo);
#endif
	return 0;
}

LRESULT CCallCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	CIUIString strREString = m_pReInput->GetWindowText();

	if (m_pBtnCloseCallPage->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnCloseCallPage(pe);
	}
	else if (m_pBtnNum1->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum1(pe, strREString);
	}
	else if (m_pBtnNum2->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum2(pe, strREString);
	}
	else if (m_pBtnNum3->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum3(pe, strREString);
	}
	else if (m_pBtnNum4->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum4(pe, strREString);
	}
	else if (m_pBtnNum5->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum5(pe, strREString);
	}
	else if (m_pBtnNum6->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum6(pe, strREString);
	}
	else if (m_pBtnNum7->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum7(pe, strREString);
	}
	else if (m_pBtnNum8->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum8(pe, strREString);
	}
	else if (m_pBtnNum9->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum9(pe, strREString);
	}
	else if (m_pBtnNumxing->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNumxing(pe, strREString);
	}
	else if (m_pBtnNum0->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNum0(pe, strREString);
	}
	else if (m_pBtnNumsharp->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnNumsharp(pe, strREString);
	}
	else if (m_pBtnAudio->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnAudio(pe, strREString);
	}
	else if (m_pBtnCalling->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnCalling(pe,strREString);
	}
	else if (m_pBtnVideo->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnVideo(pe, strREString);
	}

	return 0;
}

LRESULT CCallCanvasHandler::OnCommand(RoutedEventArgs *pe)
{
	if (m_pReInput->GetSafeHwnd() == pe->hOriginalSender)
	{
		// 当输入框的内容发生改变后，会收到EN_CHANGE消息
		if (EN_CHANGE == HIWORD(pe->wParam))
		{
			m_strSipNumber = m_pReInput->GetWindowText();
		}
	}

	return 0;
}

LRESULT CCallCanvasHandler::OnAfterPaintBkImage(RoutedEventArgs *pe)
{
	if (GetName(pe->hOriginalSender) == _T("IDC_RE_SEARCH")
		&& GetClassName(pe->hOriginalSender) == IUI_RICHTEXTBOX)
	{
		// 当搜索框没有文本时，背景显示提示方案
		CIUIString strText = GetWindowText(pe->hOriginalSender);
		if (strText.IsEmpty())
		{
			HDC hDC = (HDC)pe->lParam;
			CIUIRect rcClient;
			IUI::GetClientRect(pe->hOriginalSender, rcClient);

			RECT rcTextPadding = GetTextPadding(pe->hOriginalSender);

			CIUIProject *pProject = GetProject(pe->hOriginalSender);
			HFONT hFont = FontManager::CreateIUIFont(pProject, _T("IDF_YAHEI_START_CONNECT"));
			COLORREF crText = pProject->GetColor(_T("IDR_COLOR_ADVANCE_N"));
			_ASSERT(hFont != NULL);
			CIUIString strPrompt = _T("Search");
			DrawFormatText(hDC, strPrompt, strPrompt.GetLength(),
				rcClient, &rcTextPadding,
				TAH_CENTER, TAV_TOP, DT_SINGLELINE, hFont,
				crText, FALSE);
		}
	}

	return 0;
}

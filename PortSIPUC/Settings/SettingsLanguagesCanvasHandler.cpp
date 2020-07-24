#include "stdafx.h"
#include "SettingsLanguagesCanvasHandler.h"
#include "System/SystemEx.h"

CSettingsLanguagesCanvasHandler::CSettingsLanguagesCanvasHandler()
	: m_pRadEnglish(NULL)
	, m_pStaSplitter1(NULL)
	, m_pRadChs(NULL)
	, m_pStaSplitter2(NULL)
	, m_pRadCht(NULL)
	, m_pStaSplitter3(NULL)
{
	m_bIsOpen = false;
	m_enumCLanguage = m_enumLLanguage = (ENUM_LANGUAGE)CSystemEx::GetInstance()->GetGlobalOptions()->m_nLanguage;
}

CSettingsLanguagesCanvasHandler::~CSettingsLanguagesCanvasHandler()
{
}

int CSettingsLanguagesCanvasHandler::InitControls()
{
	m_pRadEnglish = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_ENGLISH")));
	_ASSERT(m_pRadEnglish != NULL);
	m_pStaSplitter1 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER1")));
	_ASSERT(m_pStaSplitter1 != NULL);
	m_pRadChs = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_CHS")));
	_ASSERT(m_pRadChs != NULL);
	m_pStaSplitter2 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER2")));
	_ASSERT(m_pStaSplitter2 != NULL);
	m_pRadCht = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RAD_CHT")));
	_ASSERT(m_pRadCht != NULL);
	m_pStaSplitter3 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER3")));
	_ASSERT(m_pStaSplitter3 != NULL);


	m_pRadCht->ShowWindow(SW_HIDE);
	m_pStaSplitter3->ShowWindow(SW_HIDE);
	SelectLanguage();
	return 0;
}
void CSettingsLanguagesCanvasHandler::SelectLanguage()
{
	COptions *pOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pOptions == NULL)
	{
		return;
	}
	switch (m_enumCLanguage)
	{
	case ENUM_ENGLISH:
		m_pRadEnglish->SetCheck(BST_CHECKED);
		break;
	case ENUM_CHINAESE_S:
		m_pRadChs->SetCheck(BST_CHECKED);
		break;
	case ENUM_CHINAESE_T:
		m_pRadCht->SetCheck(BST_CHECKED);
		break;
	case ENUM_ITAlIANA:
		break;
	case ENUM_SPAIN:
		break;
	case ENUM_FRANCH:
		break;
	case ENUM_RUSSIA:
		break;
	case ENUM_PORTUGUESE:
		break;
	default:
		break;
	}
}
int CSettingsLanguagesCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	m_bIsOpen = true;
	return 0;
}

LRESULT CSettingsLanguagesCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
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
	}

	return 0;
}

LRESULT CSettingsLanguagesCanvasHandler::OnRadEnglish(RoutedEventArgs *pe)
{
	m_enumCLanguage = ENUM_ENGLISH;
	return 0;
}

LRESULT CSettingsLanguagesCanvasHandler::OnRadChs(RoutedEventArgs *pe)
{
	m_enumCLanguage = ENUM_CHINAESE_S;
	return 0;
}

LRESULT CSettingsLanguagesCanvasHandler::OnRadCht(RoutedEventArgs *pe)
{
	m_enumCLanguage = ENUM_CHINAESE_T;
	return 0;
}

LRESULT CSettingsLanguagesCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pRadEnglish->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadEnglish(pe);
	}
	else if (m_pRadChs->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadChs(pe);
	}
	else if (m_pRadCht->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnRadCht(pe);
	}

	return 0;
}
void CSettingsLanguagesCanvasHandler::Save()
{
	if (m_bIsOpen == false)
	{
		return;
	}
	COptions *pOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pOptions == NULL)
	{
		return;
	}
	if (m_enumLLanguage!=m_enumCLanguage)
	{
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"To change the language, you need to restart the client to take effect", NULL);
	}
	pOptions->m_nLanguage = (ENUM_LANGUAGE)m_enumCLanguage;
	pOptions->m_bSaveLanguage = true;
	m_enumLLanguage = m_enumCLanguage;
}
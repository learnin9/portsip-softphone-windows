#include "stdafx.h"
#include "SettingsApplicationPageHandler.h"
#include "System/SystemEx.h"
#include <shlobj.h>
#include <shellapi.h>
CSettingsApplicationPageHandler::CSettingsApplicationPageHandler()
	: m_pStaSetDefault(NULL)
	, m_pChkReadReceipts(NULL)
	, m_pStaFileLocation(NULL)
	, m_pBtnBrowser(NULL)
	, m_pBtnOpenFolder(NULL)
	, m_pReFile(NULL)
	, m_pStaPromptUpgrade(NULL)
	, m_pChkLocalVideoInRoom(NULL)
	, m_pStaDisplayIdle(NULL)
	, m_pChkDisplayIdle(NULL)
	, m_pStaStartWithSystem(NULL)
	, m_pChkStartWithSystem(NULL)
	, m_pStaShowTimestamp(NULL)
	, m_pChkShowTimestamp(NULL)
	, m_pStaSplitter1(NULL)
	, m_pStaSplitter2(NULL)
	, m_pStaSplitter3(NULL)
	, m_pStaSplitter4(NULL)
	, m_pStaSplitter5(NULL)
	, m_strRecordFolder (L"")
{
}

CSettingsApplicationPageHandler::~CSettingsApplicationPageHandler()
{
}

int CSettingsApplicationPageHandler::InitControls()
{
	m_pStaSetDefault = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SET_DEFAULT")));
	_ASSERT(m_pStaSetDefault != NULL);
	m_pChkReadReceipts = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_READ_RECEIPTS")));
	_ASSERT(m_pChkReadReceipts != NULL);
	m_pStaFileLocation = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_FILE_LOCATION")));
	_ASSERT(m_pStaFileLocation != NULL);
	m_pBtnBrowser = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_BROWSER")));
	_ASSERT(m_pBtnBrowser != NULL);
	m_pBtnOpenFolder = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_OPEN_FOLDER")));
	_ASSERT(m_pBtnOpenFolder != NULL);
	m_pReFile = (RichTextBox *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_RE_FILE")));
	_ASSERT(m_pReFile != NULL);
	m_pStaPromptUpgrade = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_PROMPT_UPGRADE")));
	_ASSERT(m_pStaPromptUpgrade != NULL);
	m_pChkLocalVideoInRoom = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_PROMPT_UPGRADE")));
	_ASSERT(m_pChkLocalVideoInRoom != NULL);
	m_pStaDisplayIdle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_DISPLAY_IDLE")));
	_ASSERT(m_pStaDisplayIdle != NULL);
	m_pChkDisplayIdle = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_DISPLAY_IDLE")));
	_ASSERT(m_pChkDisplayIdle != NULL);
	m_pStaStartWithSystem = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_START_WITH_SYSTEM")));
	_ASSERT(m_pStaStartWithSystem != NULL);
	m_pChkStartWithSystem = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_START_WITH_SYSTEM")));
	_ASSERT(m_pChkStartWithSystem != NULL);
	m_pStaShowTimestamp = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SHOW_TIMESTAMP")));
	_ASSERT(m_pStaShowTimestamp != NULL);
	m_pChkShowTimestamp = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_SHOW_TIMESTAMP")));
	_ASSERT(m_pChkShowTimestamp != NULL);
	m_pStaSplitter1 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER1")));
	_ASSERT(m_pStaSplitter1 != NULL);
	m_pStaSplitter2 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER2")));
	_ASSERT(m_pStaSplitter2 != NULL);
	m_pStaSplitter3 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER3")));
	_ASSERT(m_pStaSplitter3 != NULL);
	m_pStaSplitter4 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER4")));
	_ASSERT(m_pStaSplitter4 != NULL);
	m_pStaSplitter5 = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_SPLITTER5")));
	_ASSERT(m_pStaSplitter5 != NULL);

	return 0;
}

int CSettingsApplicationPageHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	LoadOptions();
	return 0;
}
void CSettingsApplicationPageHandler::LoadOptions()
{
	COptions* pOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	m_pChkReadReceipts->SetCheck(pOptions->m_bDefaultSoftphone);
	m_pChkLocalVideoInRoom->SetCheck(pOptions->m_bLocalVideoInRoom);
	m_pChkDisplayIdle->SetCheck(pOptions->m_bDisplayidle);
	if (CSystemEx::GetInstance()->isAutoRun() == true)
	{
		m_pChkStartWithSystem->SetCheck(TRUE);
	}
	else
	{
		m_pChkStartWithSystem->SetCheck(FALSE);
	}
	m_pChkShowTimestamp->SetCheck(pOptions->m_bShowTimeStamp);

	m_strRecordFolder = PortUtility::Utf82Unicode_ND(pOptions->m_strRecordFolder);
	m_pReFile->SetWindowText(m_strRecordFolder.c_str());
}
LRESULT CSettingsApplicationPageHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

LRESULT CSettingsApplicationPageHandler::OnChkReadReceipts(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnBtnBrowser(RoutedEventArgs *pe)
{
	TCHAR szFilePath[1024] = { 0 };   
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szFilePath;
	bi.lpszTitle = CSystemEx::GetInstance()->GetTipsSaveDstPathText();
	bi.ulFlags = BIF_BROWSEINCLUDEFILES;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return 0;
	}
	SHGetPathFromIDList(idl, szFilePath);
	m_strRecordFolder = szFilePath;
	m_pReFile->SetWindowText(m_strRecordFolder.c_str());
	
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnBtnOpenFolder(RoutedEventArgs *pe)
{
	if (m_strRecordFolder.empty())
	{
		return 0;
	}
	ShellExecute(NULL, _T("OPEN"), m_strRecordFolder.c_str(), NULL, NULL, SW_NORMAL);
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnChkLocalVideoInRoom(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnChkDisplayIdle(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnChkStartWithSystem(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsApplicationPageHandler::OnChkShowTimestamp(RoutedEventArgs *pe)
{
	return 0;
}


void CSettingsApplicationPageHandler::SaveOptions()
{

}
LRESULT CSettingsApplicationPageHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pChkReadReceipts->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkReadReceipts(pe);
	}
	else if (m_pBtnBrowser->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnBrowser(pe);
	}
	else if (m_pBtnOpenFolder->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnOpenFolder(pe);
	}
	else if (m_pChkLocalVideoInRoom->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkLocalVideoInRoom(pe);
	}
	else if (m_pChkDisplayIdle->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkDisplayIdle(pe);
	}
	else if (m_pChkStartWithSystem->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkStartWithSystem(pe);
	}
	else if (m_pChkShowTimestamp->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnChkShowTimestamp(pe);
	}
	else if (m_pChkLocalVideoInRoom->GetSafeHwnd()==pe->hOriginalSender)
	{
		OnChkLocalVideoInRoom(pe);
	}
	return 0;
}

void CSettingsApplicationPageHandler::Save()
{
	std::wstring strFileFolder;
	strFileFolder =m_pReFile->GetWindowText();
	COptions *pGlobalOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pGlobalOptions == NULL)
		return;
	pGlobalOptions->m_strRecordFolder = PortUtility::Unicode2Utf8_ND(strFileFolder);

	int defaultSoftphone = 0;
	int autoUpdate = 0;
	int displayIdle = 0;
	int showTimeStamp = 0;
	if (m_pChkReadReceipts->GetCheck()==true)
	{
		pGlobalOptions->m_bDefaultSoftphone = true;
	}
	else
	{
		pGlobalOptions->m_bDefaultSoftphone = false;
	}
	if (m_pChkLocalVideoInRoom->GetCheck())
	{
		pGlobalOptions->m_bLocalVideoInRoom = true;
	}
	else
	{
		pGlobalOptions->m_bLocalVideoInRoom = false;
	}
	if (m_pChkDisplayIdle->GetCheck())
	{
		pGlobalOptions->m_bDisplayidle = true;
	}
	else
	{
		pGlobalOptions->m_bDisplayidle = false;
	}
	if (m_pChkStartWithSystem->GetCheck())
	{
		CSystemEx::GetInstance()->SetAutoRun();
	}
	else
	{
		CSystemEx::GetInstance()->CancelAutoRun();
	}
	if (m_pChkShowTimestamp->GetCheck())
	{
		pGlobalOptions->m_bShowTimeStamp = true;
	}
	else
	{
		pGlobalOptions->m_bShowTimeStamp = false;
	}

	if (m_pChkLocalVideoInRoom->GetCheck())
	{
		pGlobalOptions->m_bLocalVideoInRoom = true;
	}
	else
	{
		pGlobalOptions->m_bLocalVideoInRoom = false;
	}
	
	/*
	stringstream sql;
	sql << "Update ";
	sql << TABLE_OPTIONS;
	sql << " set ";
	sql << " ARPath = '";
	sql << PortCoders::base64Encode(g_options.ARPath);
	sql << "', VRPath = '";
	sql << PortCoders::base64Encode(g_options.VRPath);
	sql << "', recordFolder = '";
	sql << PortCoders::base64Encode(g_options.recordFolder);
	sql << "', defaultSoftphone = '";
	sql << defaultSoftphone;
	sql << "', LocalVideoInRoom = '";
	sql << autoUpdate;
	sql << "', displayIdle = '";
	sql << displayIdle;
	sql << "', showTimeStamp = '";
	sql << showTimeStamp;
	sql << "', callImmediately = '";
	sql << callImmediately;
	sql << "'";


	string errnoMessage;
	g_optionsDB.exec(sql.str().c_str(), NULL, NULL, errnoMessage);
	*/
}
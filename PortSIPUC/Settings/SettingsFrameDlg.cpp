#include "stdafx.h"
#include "SettingsFrameDlg.h"

#include "System/SystemEx.h"

CSettingsFrameDlg::CSettingsFrameDlg()
	: m_pTreeNavigate(NULL)
	, m_pStaTitle(NULL)
	, m_pBtnClose(NULL)
	, m_pStaSplitter(NULL)
	, m_pTabSettingsPage(NULL)
	, m_pBtnSave(NULL)
{

	 m_pPageApplication   = NULL;
	 m_pPageAudioCodes = NULL;
	 m_pPageVideoCodes = NULL;
	 m_pPageMediaQuality = NULL;
	 m_pPageMisc = NULL;
	 m_pLanguages = NULL;
}

CSettingsFrameDlg::~CSettingsFrameDlg()
{
}

int CSettingsFrameDlg::InitControls()
{
	m_pTreeNavigate = (TreeView *)CWLWnd::FromHandle(FindControl(_T("IDC_TREECTRL_NAVIGATE")));
	_ASSERT(m_pTreeNavigate != NULL);
	m_pStaTitle = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_TITLE")));
	_ASSERT(m_pStaTitle != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pStaSplitter = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_SPLITTER")));
	_ASSERT(m_pStaSplitter != NULL);
	m_pTabSettingsPage = (TabControl *)CWLWnd::FromHandle(FindControl(_T("IDC_PNL_TAB_SETTINGS_PAGE")));
	_ASSERT(m_pTabSettingsPage != NULL);
	m_pBtnSave = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_SAVE")));
	_ASSERT(m_pBtnSave != NULL);

	return 0;
}

LRESULT CSettingsFrameDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	m_hItemApplication = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetApplicationText());
	m_hItemAudioCodecs = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetAudioCodecsText());
	m_hItemVideoCodecs = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetVideoCodecsText());
	m_hItemMediaQuality = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetMediaQualityText());
	m_hItemMisc = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetMiscText());
	m_hItemLanguages = m_pTreeNavigate->InsertItem(CSystemEx::GetInstance()->GetLanguagesText());


	m_pPageApplication = ALLOC_HANDLER(CSettingsApplicationPageHandler);
	m_pPageAudioCodes = ALLOC_HANDLER(CSettingsAudioCodecsPageHandler);
	m_pPageVideoCodes = ALLOC_HANDLER(CSettingsVideoCodecsPageHandler);
	m_pPageMediaQuality = ALLOC_HANDLER(CSettingsMediaQualityHandler);
	m_pPageMisc = ALLOC_HANDLER(CSettingsMiscCanvasHandler);
	m_pLanguages = ALLOC_HANDLER(CSettingsLanguagesCanvasHandler);

	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemApplication,
		_T("IDW_SETTINGS_APPLICATION.xml"), m_pPageApplication);
	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemAudioCodecs,
		_T("IDW_SETTINGS_AUDIO_CODECS.xml"), m_pPageAudioCodes);
	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemVideoCodecs,
		_T("IDW_SETTINGS_VIDEO_CODECS.xml"), m_pPageVideoCodes);
	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemMediaQuality,
		_T("IDW_SETTINGS_MEDIA_QUALITY.xml"), m_pPageMediaQuality);
	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemMisc,
		_T("IDW_SETTINGS_MISC"), m_pPageMisc);
	m_pTabSettingsPage->AddPropertySheet((UINT_PTR)m_hItemLanguages,
		_T("IDW_SETTINGS_LANGUAGES.xml"), m_pLanguages);

	m_pTabSettingsPage->SwitchPropertySheet((UINT_PTR)m_hItemApplication);

	CenterWindow();

	return 0;
}

LRESULT CSettingsFrameDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CSettingsFrameDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			OnButtonClicked(pe);
		}
		else if (m_pTreeNavigate->GetSafeHwnd() == pe->hOriginalSender)
		{
			if (WM_NOTIFY == pe->RoutedEvent)
			{
				NMHDR *pnmhdr = (NMHDR *)pe->lParam;

				if (TVN_SELCHANGED == pnmhdr->code)
				{
					HTREEITEM hSelItem = m_pTreeNavigate->GetSelectedItem();
					OnTreeNavigateSelectItem(hSelItem);
				}
			}
		}
	}

	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CSettingsFrameDlg::OnBtnClose(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CSettingsFrameDlg::OnBtnSave(RoutedEventArgs *pe)
{
	bool bSaveAudioCodes = false;
	if (m_pPageApplication !=NULL)
	{
		m_pPageApplication->Save();
	}
	if (m_pPageAudioCodes !=NULL)
	{
		bSaveAudioCodes = m_pPageAudioCodes->Save();
	}
	if (m_pPageVideoCodes != NULL)
	{
		m_pPageVideoCodes->Save();
	}
	if (m_pPageMediaQuality != NULL)
	{
		m_pPageMediaQuality->Save();
	}
	if (m_pPageMisc != NULL)
	{
		m_pPageMisc->Save();
	}
	if (m_pLanguages != NULL)
	{
		m_pLanguages->Save();
	}

	COptions *pOption = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pOption)
	{
		SaveAllData();
	}                              
	if (bSaveAudioCodes==true)
	{
		EndDialog(IDOK);
	}
	else
	{
		m_pTabSettingsPage->SwitchPropertySheet((UINT_PTR)m_hItemAudioCodecs);
	}

	return 0;
}

void CSettingsFrameDlg::SaveAllData()
{
	CSystemEx::GetInstance()->UpdateAllOptionData();
}

LRESULT CSettingsFrameDlg::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnClose->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnClose(pe);
	}
	else if (m_pBtnSave->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnSave(pe);
	}

	return 0;
}

LRESULT CSettingsFrameDlg::OnTreeNavigateSelectItem(HTREEITEM hSelItem)
{
	if (NULL != hSelItem)
	{
		m_pTabSettingsPage->SwitchPropertySheet((UINT)hSelItem);
	}

	return 0;
}

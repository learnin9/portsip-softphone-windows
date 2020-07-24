#include "stdafx.h"
#include "EmoticonPageCanvasHandler.h"
#include "system/LocalEmojiRes.h"
#include "System/SystemEx.h"
CEmoticonPageCanvasHandler::CEmoticonPageCanvasHandler()
	: m_pStaEmoticonTitle(NULL)
	, m_pLstEmoticon(NULL)
{
}

CEmoticonPageCanvasHandler::~CEmoticonPageCanvasHandler()
{
}

int CEmoticonPageCanvasHandler::InitControls()
{
	m_pStaEmoticonTitle = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_EMOTICON_TITLE")));
	_ASSERT(m_pStaEmoticonTitle != NULL);
	m_pLstEmoticon = (ListView *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_LSTCTRL_EMOTICON")));
	_ASSERT(m_pLstEmoticon != NULL);

	return 0;
}

int CEmoticonPageCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	if (EP_CLASSICAL == GetUserData())
	{
		m_pStaEmoticonTitle->SetWindowText(CSystemEx::GetInstance()->GetClassicEmotionText());

		CIUIProject *pProject = GetProject(GetBindWLWnd());
		CControlProp *pContactProp = pProject->FindProjectItem(_T("IDW_EMOTICON_ICON.xml"));
		_ASSERT(pContactProp != NULL);
		CControlProp *pBindContactProp = (CControlProp *)pContactProp->GetChild(0);
		_ASSERT(pBindContactProp != NULL);

		int nSize = vecEmojiRes.size();
		int i = 0;
		for (i; i< nSize; i++)
		{
			CIUIString strFileName = PortUtility::Utf82Unicode_ND(vecEmojiRes.at(i)).c_str();
			strFileName= L"\\emoji\\"+ strFileName+L".png";
			int nIndex = m_pLstEmoticon->InsertItem(i, _T(""));

			HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindContactProp,
				m_pLstEmoticon->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

			Label *pStaEmoticon = (Label *)CWLWnd::FromHandle(FindControl(hBind, _T("IDC_STA_EMOTICON")));
			_ASSERT(pStaEmoticon != NULL);

			pStaEmoticon->SetBkImage(CONTROL_STATE_NORMAL, strFileName, NULL, FALSE);
			
			m_pLstEmoticon->SetItemBindWindowless(nIndex, 0, hBind);
		}
	}
	else if (EP_SYMBOL == GetUserData())
	{
		m_pStaEmoticonTitle->SetWindowText(CSystemEx::GetInstance()->GetSymbolEmotionText());
	}

	return 0;
}

LRESULT CEmoticonPageCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

LRESULT CEmoticonPageCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{

	return 0;
}

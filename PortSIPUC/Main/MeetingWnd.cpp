#include "stdafx.h"
#include "MeetingWnd.h"
#include <time.h>


CMeetingWnd::CMeetingWnd()
	: m_pBtnNewMeeting(NULL)
	, m_pStaIcon(NULL)
	, m_pStaNewMeeting(NULL)
	, m_pStaStartOrSchedule(NULL)
	, m_pBtnMeetingList(NULL)
	, m_pStaIcon2(NULL)
	, m_pStaMeetingList(NULL)
	, m_pStaJoinByMeetingId(NULL)
	, m_pTreeMeetingList(NULL)
	, m_nMeetingItemHeight(58)
{
}

CMeetingWnd::~CMeetingWnd()
{
}

int CMeetingWnd::InitControls()
{
	m_pBtnNewMeeting = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_NEW_MEETING")));
	_ASSERT(m_pBtnNewMeeting != NULL);
	m_pStaIcon = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_ICON")));
	_ASSERT(m_pStaIcon != NULL);
	m_pStaNewMeeting = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_NEW_MEETING")));
	_ASSERT(m_pStaNewMeeting != NULL);
	m_pStaStartOrSchedule = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_START_OR_SCHEDULE")));
	_ASSERT(m_pStaStartOrSchedule != NULL);
	m_pBtnMeetingList = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MEETING_LIST")));
	_ASSERT(m_pBtnMeetingList != NULL);
	m_pStaIcon2 = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_ICON2")));
	_ASSERT(m_pStaIcon2 != NULL);
	m_pStaMeetingList = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_MEETING_LIST")));
	_ASSERT(m_pStaMeetingList != NULL);
	m_pStaJoinByMeetingId = (Label *)CWLWnd::FromHandle(FindControl(_T("IDC_STA_JOIN_BY_MEETING_ID")));
	_ASSERT(m_pStaJoinByMeetingId != NULL);
	m_pTreeMeetingList = (TreeView *)CWLWnd::FromHandle(FindControl(_T("IDC_TREECTRL_MEETING_LIST")));
	_ASSERT(m_pTreeMeetingList != NULL);

	return 0;
}

LRESULT CMeetingWnd::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	return 0;
}

LRESULT CMeetingWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_DESTROY == uMsg)
	{
		std::list<CMeeting *>::iterator it = m_lstMeetings.begin();

		for (; it != m_lstMeetings.end(); ++it)
		{
			CMeeting *pMeeting = *it;

			delete pMeeting;
		}

		m_lstMeetings.clear();
	}

	return Window::WindowProc(uMsg, wParam, lParam);
}

LRESULT CMeetingWnd::OnControlEvent(WPARAM wParam, LPARAM lParam)
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

LRESULT CMeetingWnd::OnBtnNewMeeting(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMeetingWnd::OnBtnMeetingList(RoutedEventArgs *pe)
{
	return 0;
}

LRESULT CMeetingWnd::OnButtonClicked(RoutedEventArgs *pe)
{
	return 0;
}

int CMeetingWnd::InitMenuWnd()
{
	m_pTreeMeetingList->SetItemHeight(m_nMeetingItemHeight);

	CIUIProject *pProject = GetProject();
	CControlProp *pProp = pProject->FindProjectItem(_T("IDW_MEETING_LIST_ITEM.xml"));
	_ASSERT(pProp != NULL);
	_ASSERT(pProp->GetChildCount() == 1);
	CControlProp *pBindProp = (CControlProp *)pProp->GetChild(0);
	_ASSERT(pBindProp != NULL);

	std::list<CMeeting *>::iterator it = m_lstMeetings.begin();

	for (; it != m_lstMeetings.end(); ++it)
	{
		CMeeting *pMeeting = *it;

		HTREEITEM hItem = m_pTreeMeetingList->InsertItem(_T(""));
		m_pTreeMeetingList->SetItemData(hItem, (DWORD_PTR)pMeeting);

		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindProp,
				m_pTreeMeetingList->GetSafeHwnd(), m_hWnd, 0);

		Label *pStaTime = (Label *)CWLWnd::FromHandle(IUI::FindControl(hBind,
					_T("IDC_STA_TIME")));
		if (NULL != pStaTime)
		{
			tm *ptm = gmtime(&pMeeting->m_time);
			CIUIString strTime;
			strTime.Format(_T("%02d:%02d"), ptm->tm_hour, ptm->tm_min);
			pStaTime->SetWindowText(strTime);
		}
		else
		{
			_ASSERT(FALSE);
		}

		Label *pStaMeetingName = (Label *)CWLWnd::FromHandle(IUI::FindControl(hBind,
					_T("IDC_STA_MEETING_NAME")));
		if (NULL != pStaMeetingName)
		{
			pStaMeetingName->SetWindowText(pMeeting->m_strMeetingID);
		}
		else
		{
			_ASSERT(FALSE);
		}

		Label *pStaDuration = (Label *)CWLWnd::FromHandle(IUI::FindControl(hBind,
					_T("IDC_STA_DURATION")));
		if (NULL != pStaDuration)
		{
			CIUIString strDuration;
			strDuration.Format(_T("%d:%d:%d"),
				pMeeting->m_dwDuration / 3600,
				pMeeting->m_dwDuration / 60,
				pMeeting->m_dwDuration % 60);

			pStaDuration->SetWindowText(strDuration);
		}
		else
		{
			_ASSERT(FALSE);
		}

		Label *pStaDate = (Label *)CWLWnd::FromHandle(IUI::FindControl(hBind,
					_T("IDC_STA_DATE")));
		if (NULL != pStaDate)
		{
			//GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE,)

			USES_CONVERSION;
			pStaDate->SetWindowText(A2T(ctime(&pMeeting->m_time)));
		}
		else
		{
			_ASSERT(FALSE);
		}

		m_pTreeMeetingList->SetItemBindWindowless(hItem, hBind);
	}

	return 0;
}

int CMeetingWnd::RelayoutMenuWnd(int x, int y)
{
	CIUIRect rcSecondItem;
	m_pBtnMeetingList->GetWindowRect(rcSecondItem);
	IUIScreenToClient(m_hWnd, rcSecondItem);
	int nListHeight = m_nMeetingItemHeight * min(5, m_lstMeetings.size());
	m_pTreeMeetingList->MoveWindow(0, rcSecondItem.bottom, rcSecondItem.Width(), nListHeight);

	int nHeight = rcSecondItem.bottom + nListHeight;
	::MoveWindow(m_hWnd, x, y, rcSecondItem.Width(), nHeight, TRUE);

	return 0;
}

int CMeetingWnd::SetMeetingItemHeight(int nHeight)
{
	m_nMeetingItemHeight = nHeight;

	return 0;
}

int CMeetingWnd::InsertMeetingItem(const CMeeting *pMeeting)
{
	CMeeting *pNewMeeting = new CMeeting;
	*pNewMeeting = *pMeeting;
	m_lstMeetings.push_back(pNewMeeting);

	return 0;
}

MENUWNDRESULT CMeetingWnd::TrackPopupWnd(
	UINT uFlags,
	int x,
	int y,
	HWND hWnd,
	RECT *prcRect,
	__out LPTSTR lpszRet,
	DWORD dwRetCount)
{
	SetProject((CIUIProject *)::SendMessage(hWnd, WM_GET_PROJECT, 0, 0));
	CreatePopupWindow(_T("IDW_MEETING.xml"), hWnd);

	InitMenuWnd();
	RelayoutMenuWnd(x, y);

	MENUWNDRESULT eMenuWndRet = MENUWNDRESULT_UNVILID;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (WM_LBUTTONDOWN == msg.message)
		{
			if (msg.hwnd != m_hWnd)
			{
				DestroyWindow(m_hWnd);
				break;
			}
		}
		else if (WM_LBUTTONUP == msg.message)
		{
			if (msg.hwnd == m_hWnd)
			{
				HWLWND hControl = IUI::WindowFromPoint(m_hWnd, CIUIPoint(msg.lParam));
				if (NULL != hControl)
				{
					CIUIString strRet = (LPCTSTR)GetName(hControl);
					if (_T("IDC_BTN_NEW_MEETING") == strRet)
					{
						eMenuWndRet = MENUWNDRESULT_NEW_MEETING;
					}
					else if (_T("IDC_BTN_MEETING_LIST") == strRet)
					{
						eMenuWndRet = MENUWNDRESULT_MEETING_LIST;
					}
					else if (_T("IDC_BTN_MEETING") == strRet)
					{
						eMenuWndRet = MENUWNDRESULT_MEETING_ITEM;

						if (NULL != lpszRet)
						{
							CIUIPoint pt(msg.lParam);
							::ClientToScreen(m_hWnd, &pt);
							IUI::ScreenToClient(m_pTreeMeetingList->GetSafeHwnd(), &pt);
							HTREEITEM hTreeItem = m_pTreeMeetingList->HitTest(pt);
							if (NULL != hTreeItem)
							{
								CMeeting *pMeeting = (CMeeting *)m_pTreeMeetingList->GetItemData(hTreeItem);
								_ASSERT(NULL != pMeeting);

								StringCchCopy(lpszRet, dwRetCount, pMeeting->m_strMeetingID);
							}
						}
					}

					::DestroyWindow(m_hWnd);
					break;
				}
			}
		}
		else if (WM_KEYDOWN == msg.message)
		{
			if (VK_ESCAPE == msg.wParam || VK_RETURN == msg.wParam)
			{
				DestroyWindow(m_hWnd);
				break;
			}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return eMenuWndRet;
}

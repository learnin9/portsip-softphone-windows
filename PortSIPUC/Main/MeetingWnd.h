#pragma once

#include <list>

class CMeeting
{
public:
	CIUIString m_strMeetingID;
	time_t m_time;
	DWORD m_dwDuration;
};

enum MENUWNDRESULT
{
	MENUWNDRESULT_UNVILID = 0,
	MENUWNDRESULT_NEW_MEETING = 1,	// click NEW MEETING return
	MENUWNDRESULT_MEETING_LIST,		// click MEETING LISTreturn
	MENUWNDRESULT_MEETING_ITEM		// click Meeting return£¬TrackPopupWnd->lpszRet means Meeting item
};

// IDW_MEETING
class CMeetingWnd : public Window
{
public:
	CMeetingWnd();
	virtual ~CMeetingWnd();

protected:
	int InitControls();
	// Event handler
	virtual LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnButtonClicked(RoutedEventArgs *pe);
	LRESULT OnBtnNewMeeting(RoutedEventArgs *pe);
	LRESULT OnBtnMeetingList(RoutedEventArgs *pe);

protected:
	int InitMenuWnd();
	int RelayoutMenuWnd(int x, int y);

public:
	int SetMeetingItemHeight(int nHeight);
	int InsertMeetingItem(const CMeeting *pMeeting);
	MENUWNDRESULT TrackPopupWnd(
		UINT uFlags,
		int x,
		int y,
		HWND hWnd,
		RECT *prcRect,
		__out LPTSTR lpszRet,
		DWORD dwRetCount
	);

protected:
	Button *m_pBtnNewMeeting;
	Label *m_pStaIcon;
	Label *m_pStaNewMeeting;
	Label *m_pStaStartOrSchedule;
	Button *m_pBtnMeetingList;
	Label *m_pStaIcon2;
	Label *m_pStaMeetingList;
	Label *m_pStaJoinByMeetingId;
	TreeView *m_pTreeMeetingList;

	int m_nMeetingItemHeight;
	std::list<CMeeting *> m_lstMeetings;
};

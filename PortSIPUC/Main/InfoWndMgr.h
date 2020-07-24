#pragma once

#include "InfoWnd.h"
#include "system/CriticalSection.h"
struct INFO_WND_MGR_PARAM 
{
	INFO_WND_MGR_PARAM::INFO_WND_MGR_PARAM()
	{
		 m_nIndex = 0;
	     m_bIsFree = true;
		 m_pInfoWnd = NULL;
	}
	INFO_WND_MGR_PARAM & operator =(const INFO_WND_MGR_PARAM & other)
	{
		if (this == &other)
			return *this;
		m_nIndex = other.m_nIndex;
		m_bIsFree = other.m_bIsFree;
		m_rcWnd = other.m_rcWnd;
		m_pInfoWnd = other.m_pInfoWnd;
	}
	int			m_nIndex;
	bool	    m_bIsFree;
	RECT		m_rcWnd;
	CInfoWnd *  m_pInfoWnd;
};
class CInfoWndMgr
{
public:
	CInfoWndMgr();
	~CInfoWndMgr();

	void CreatInfoWnd(LPCTSTR lpTitle, LPCTSTR lpInfo, HWND hParent,int nAutoCloseTime );
	void ClosedInfoWnd(CInfoWnd *pInfoWnd);
	int  GetFreeMetricsY(int nIndex);
	int  GetFreeIndex(bool &bFreeInfoWnd);
	vector<INFO_WND_MGR_PARAM * > m_vecInfoWnd;
	CCriticalSection			m_oInfoWndCriticalWnd;
private:

};


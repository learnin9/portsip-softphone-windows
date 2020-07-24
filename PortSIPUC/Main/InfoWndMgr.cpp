#include "stdafx.h"
#include"InfoWndMgr.h"
#include "System/SystemEx.h"
CInfoWndMgr::CInfoWndMgr()
{
	for (int nIndex = 0;nIndex<MAX_INFO_WND;nIndex++)
	{
		INFO_WND_MGR_PARAM * pLocalInfoWnd = new INFO_WND_MGR_PARAM;
		pLocalInfoWnd->m_nIndex = nIndex;
		m_vecInfoWnd.push_back(pLocalInfoWnd);
	}
}

CInfoWndMgr::~CInfoWndMgr()
{
}
int CInfoWndMgr::GetFreeIndex(bool &bFreeInfoWnd)
{
	int nFindCount = 0;
	INFO_WND_MGR_PARAM* pLocalInfoWnd;
	auto iterInfoWnd = m_vecInfoWnd.begin();
	for (iterInfoWnd;iterInfoWnd!=m_vecInfoWnd.end();iterInfoWnd++)
	{
		pLocalInfoWnd = *iterInfoWnd;
		if ((*iterInfoWnd)->m_bIsFree == true)
		{
			(*iterInfoWnd)->m_bIsFree = false;
			return (*iterInfoWnd)->m_nIndex;
		}
		else
		{
			nFindCount++;
		}
		
	}
	iterInfoWnd = m_vecInfoWnd.begin();
	pLocalInfoWnd = *iterInfoWnd;
	if (nFindCount==MAX_INFO_WND)
	{
		bFreeInfoWnd = true;
		pLocalInfoWnd->m_pInfoWnd->CloseWndByOutSide();
		pLocalInfoWnd->m_bIsFree == true;
		if (pLocalInfoWnd->m_pInfoWnd!=NULL)
		{
			ClosedInfoWnd(pLocalInfoWnd->m_pInfoWnd);
		}
		
		return 0;
	}
	return -1;
}
void CInfoWndMgr::CreatInfoWnd(LPCTSTR lpTitle, LPCTSTR lpInfo, HWND hParent, int nAutoCloseTime )
{
	if (hParent==NULL||!::IsWindow(hParent))
	{
		LOG4_INFO(L"CreatInfoWnd ERROR");
		return;
	}
	OutputDebugString(L"CreatInfoWnd");
	CCriticalAutoLock loLock(m_oInfoWndCriticalWnd);
	bool bFreeInfoWnd = false;
	int nIndex = GetFreeIndex(bFreeInfoWnd);
	if (nIndex == -1)
	{
		return;
	}
	
	WND_INFO_LEVEL  enumInfoLeverl = INFO_WND;
	CInfoWnd * pInfoWnd = new CInfoWnd(enumInfoLeverl);
	m_vecInfoWnd.at(nIndex)->m_pInfoWnd = pInfoWnd;


	if (NULL != pInfoWnd)
	{
		pInfoWnd->SetAutoCloseTime(nAutoCloseTime);
		pInfoWnd->SetProject(g_pProject);
		pInfoWnd->CreatePopupWindow(_T("IDW_INFO.xml"), hParent);

		pInfoWnd->SetTitle(lpTitle);
		pInfoWnd->SetInfo(lpInfo);
		
		RECT rcClient;
		pInfoWnd->GetWindowRect(&rcClient);
		
		int nWidth = rcClient.right - rcClient.left;
		int nHeight = rcClient.bottom - rcClient.top;
		int nStartX = CSystemEx::GetInstance()->GetMetricsX() - nWidth;
		int nStartY = CSystemEx::GetInstance()->GetMetricsY() - (nHeight*(nIndex + 1));
		m_vecInfoWnd.at(nIndex)->m_rcWnd.left = nStartX;
		m_vecInfoWnd.at(nIndex)->m_rcWnd.top = nStartY;
	

		MoveWindow(pInfoWnd->GetSafeHwnd(), m_vecInfoWnd.at(nIndex)->m_rcWnd.left, m_vecInfoWnd.at(nIndex)->m_rcWnd.top, nWidth, nHeight, true);
		ShowWindow(pInfoWnd->GetSafeHwnd(), SW_SHOW);
		pInfoWnd->Invalidate();

	}
}
void CInfoWndMgr::ClosedInfoWnd(CInfoWnd *pInfoWnd)
{
	CCriticalAutoLock loLock(m_oInfoWndCriticalWnd);
	bool bFind = false;
	INFO_WND_MGR_PARAM* pLocalInfoWnd;
	auto iterInfoWnd = m_vecInfoWnd.begin();
	for (iterInfoWnd; iterInfoWnd != m_vecInfoWnd.end(); iterInfoWnd++)
	{
		pLocalInfoWnd = *iterInfoWnd;
		if ((*iterInfoWnd)->m_pInfoWnd == pInfoWnd&& pInfoWnd!=NULL)
		{
			(*iterInfoWnd)->m_bIsFree = true;
			(*iterInfoWnd)->m_pInfoWnd = NULL;
			bFind = true;
		}
	}
}
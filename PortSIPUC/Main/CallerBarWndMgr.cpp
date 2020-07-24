#include "stdafx.h"
#include"CallerBarWndMgr.h"
#include "System/SystemEx.h"
CCallerBarWndMgr::CCallerBarWndMgr()
{
	for (int nIndex = 0; nIndex < MAX_INFO_WND; nIndex++)
	{
		CALLER_WND_MGR_PARAM * pLocalCallerBarWnd = new CALLER_WND_MGR_PARAM;
		pLocalCallerBarWnd->m_nIndex = nIndex;
		m_vecCallerBarWnd.push_back(pLocalCallerBarWnd);
	}
}

CCallerBarWndMgr::~CCallerBarWndMgr()
{
}

int CCallerBarWndMgr::GetFreeIndex(bool &bFreeInfoWnd)
{
	int nFindCount = 0;
	CALLER_WND_MGR_PARAM* pLocalCallerBarWnd;
	auto iterCallerBarWnd = m_vecCallerBarWnd.begin();
	for (iterCallerBarWnd; iterCallerBarWnd != m_vecCallerBarWnd.end(); iterCallerBarWnd++)
	{
		pLocalCallerBarWnd = *iterCallerBarWnd;
		if ((*iterCallerBarWnd)->m_bIsFree == true)
		{
			(*iterCallerBarWnd)->m_bIsFree = false;
			return (*iterCallerBarWnd)->m_nIndex;
		}
		else
		{
			nFindCount++;
		}
	}
	iterCallerBarWnd = m_vecCallerBarWnd.begin();
	pLocalCallerBarWnd = *iterCallerBarWnd;
	if (nFindCount == MAX_INFO_WND)
	{//NOT FIND 
		bFreeInfoWnd = true;
		LOG4_INFO("arrive max line");
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"arrive max line", NULL);
		return 0;
	}
	return -1;
}

void CCallerBarWndMgr::CreateCallerBarWnd(ICallbackParameters * pCallParameters,HWND hParent, int nActiveLine, bool bIsConference)
{
	CCriticalAutoLock loLock(m_oCallerBarWndCriticalWnd);
	bool bFreeInfoWnd = false;
	int nIndex = GetFreeIndex(bFreeInfoWnd);
	if (nIndex==-1)
	{
		return;
	}
	CCallerBarWnd *pCallerWnd1 = new CCallerBarWnd;
	m_vecCallerBarWnd.at(nIndex)->m_pCallerWnd = pCallerWnd1;
	pCallerWnd1->SetParameters(pCallParameters,nActiveLine, bIsConference);
	if (NULL != pCallerWnd1)
	{
		pCallerWnd1->SetProject(g_pProject);
		pCallerWnd1->CreatePopupWindow(_T("IDW_CALLER_BAR.xml"), hParent);
		

		RECT rcClient;
		pCallerWnd1->GetWindowRect(&rcClient);

		int nWidth = rcClient.right - rcClient.left;
		int nHeight = rcClient.bottom - rcClient.top;
		int nStartX = (CSystemEx::GetInstance()->GetMetricsX() - nWidth)/2;
		int nStartY = (nHeight*(nIndex ));
		m_vecCallerBarWnd.at(nIndex)->m_rcWnd.left = nStartX;
		m_vecCallerBarWnd.at(nIndex)->m_rcWnd.top = nStartY;

		bool bMinSize = IsIconic(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		BOOL bVisible = IsWindowVisible(CSystemEx::GetInstance()->GetMainDlg()->GetSafeHwnd());
		HWND hMainDlg = CSystemEx::GetInstance()->GetMainDlg()->m_hWnd;
		if (bMinSize)
		{
			SetForegroundWindow(hMainDlg);
			::SendMessage(hMainDlg, WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else
		{
			if (bVisible == FALSE)
			{
				::ShowWindow(hMainDlg, SW_SHOW);
			}
		}
		MoveWindow(pCallerWnd1->GetSafeHwnd(), m_vecCallerBarWnd.at(nIndex)->m_rcWnd.left, m_vecCallerBarWnd.at(nIndex)->m_rcWnd.top, nWidth, nHeight, true);
		ShowWindow(pCallerWnd1->GetSafeHwnd(), SW_SHOW);
		pCallerWnd1->Invalidate();

	}
	string key = "ringin";
	CSystemEx::GetInstance()->loopPlaySystemWaveFile(key);
}

void CCallerBarWndMgr::CloseCallerBarWnd(CCallerBarWnd *pCallerBarWnd)
{
	CCriticalAutoLock loLock(m_oCallerBarWndCriticalWnd);
	bool bFind = false;
	CALLER_WND_MGR_PARAM * pLocalCallerBarWnd;
	auto iterCallerBarWnd = m_vecCallerBarWnd.begin();
	for (iterCallerBarWnd; iterCallerBarWnd !=m_vecCallerBarWnd.end(); iterCallerBarWnd++)
	{
		pLocalCallerBarWnd = *iterCallerBarWnd;
		if (pLocalCallerBarWnd != NULL&& pLocalCallerBarWnd->m_pCallerWnd!=NULL &&pCallerBarWnd == pLocalCallerBarWnd->m_pCallerWnd)
		{
			bFind = true;
			(*iterCallerBarWnd)->m_pCallerWnd = NULL;
			(*iterCallerBarWnd)->m_bIsFree = true;
		}
	}
	CSystemEx::GetInstance()->stopLoopPlaySystemWaveFile();
}

void CCallerBarWndMgr::CloseByRemote(int nActiveSession)
{
	CCriticalAutoLock loLock(m_oCallerBarWndCriticalWnd);
	bool bFind = false;
	CALLER_WND_MGR_PARAM * pLocalCallerBarWnd;
	auto iterCallerBarWnd = m_vecCallerBarWnd.begin();
	int nCloseNum = 0;
	for (iterCallerBarWnd; iterCallerBarWnd < m_vecCallerBarWnd.end(); iterCallerBarWnd++)
	{
		pLocalCallerBarWnd = *iterCallerBarWnd;
		if (pLocalCallerBarWnd->m_pCallerWnd!=NULL)
		{
			if (pLocalCallerBarWnd->m_pCallerWnd->GetParameters()!=NULL)
			{
				int nIndex = pLocalCallerBarWnd->m_pCallerWnd->GetActiveLine();
				if (nActiveSession == pLocalCallerBarWnd->m_pCallerWnd->GetActiveLine()&& pLocalCallerBarWnd->m_pCallerWnd->GetActiveLine()!=-1)
				{
					bFind = true;
					pLocalCallerBarWnd->m_pCallerWnd->CloseByRemote();
					CSystemEx::GetInstance()->Hangup(nIndex);
					(*iterCallerBarWnd)->m_pCallerWnd = NULL;
					(*iterCallerBarWnd)->m_bIsFree = true;
				}
			}
			
		}
		if ((*iterCallerBarWnd)->m_bIsFree)
		{
			nCloseNum++;
		}
		//all coming call  is close
		if (nCloseNum == m_vecCallerBarWnd.size())
		{
			CSystemEx::GetInstance()->stopLoopPlaySystemWaveFile();
		}
	}
}
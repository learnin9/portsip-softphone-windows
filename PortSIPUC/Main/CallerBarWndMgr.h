#pragma  once
#include "CallerBarWnd.h"


#include "system/CriticalSection.h"
struct CALLER_WND_MGR_PARAM
{
	CALLER_WND_MGR_PARAM::CALLER_WND_MGR_PARAM()
	{
		m_nIndex = 0;
		m_bIsFree = true;
		m_pCallerWnd = NULL;
	}

	CALLER_WND_MGR_PARAM & operator =(const CALLER_WND_MGR_PARAM & other)
	{
		if (this == &other)
			return *this;
		m_nIndex = other.m_nIndex;
		m_bIsFree = other.m_bIsFree;
		m_rcWnd = other.m_rcWnd;
		m_pCallerWnd = other.m_pCallerWnd;
	}
	int			m_nIndex;
	bool	    m_bIsFree;
	RECT		m_rcWnd;
	CCallerBarWnd *  m_pCallerWnd;
};
class CCallerBarWndMgr
{
public:
	CCallerBarWndMgr();
	~CCallerBarWndMgr();

	void CreateCallerBarWnd(ICallbackParameters * pCallParameters,HWND hParent,int nActiveLine,bool bIsConference);
	void CloseCallerBarWnd(CCallerBarWnd *pInfoWnd);
	vector<CALLER_WND_MGR_PARAM *> m_vecCallerBarWnd;
	int  GetFreeMetricsY(int nIndex);
	int  GetFreeIndex(bool &bFreeInfoWnd);
	void CloseByRemote(int nActiveSession);
	//
	CCriticalSection			m_oCallerBarWndCriticalWnd;
private:

};

#include "stdafx.h"
#include "BaseThread.h"


#include <sys/types.h> 
#include <sys/timeb.h>
#include <stdio.h>
#include <time.h>

#include <assert.h>

#ifndef ASSERT
#define ASSERT assert
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseThread::CBaseThread()
{

}

CBaseThread::~CBaseThread()
{

}

BOOL CBaseThread::BeginThread(unsigned int (*apStartFunc)(void *),void *apParam)
{
    STRU_THREAD_INFO *lpThreadInfo = NULL;
    lpThreadInfo = new struct STRU_THREAD_INFO;
    if (NULL == lpThreadInfo)
    {
        return FALSE;
    }
    lpThreadInfo->mpUserParam = apParam;
    lpThreadInfo->mpThreadFunc = apStartFunc;
    unsigned int luThreadID = 0;
    m_hThreadHandle = _beginthreadex(NULL,0,Win32ThreadFunc,lpThreadInfo,0,&luThreadID);
    if (-1 == m_hThreadHandle)
    {
        return FALSE;
    }
    CloseHandle((HANDLE)m_hThreadHandle);
    return TRUE;

}

unsigned int CBaseThread::Win32ThreadFunc(void * lpParam)
{
    STRU_THREAD_INFO *lpThreadInfo = (STRU_THREAD_INFO *)lpParam;

    ASSERT(lpThreadInfo != NULL);
    ASSERT(lpThreadInfo->mpThreadFunc != NULL);

	lpThreadInfo->mpThreadFunc(lpThreadInfo->mpUserParam);

	if (lpThreadInfo != NULL)
	{
		delete lpThreadInfo;
		lpThreadInfo = NULL;
	}
    _endthreadex(0);
    return 1;
}

void CBaseThread::Sleep(DWORD dwMilliseconds)
{
    ::Sleep(dwMilliseconds);
}


INT64 CBaseThread::GetSystemTime()
{
    struct timeb loTimeb;
    //memset(&loTimeb, 0 , sizeof(timeb));
    ftime(&loTimeb);
    return ((INT64)loTimeb.time * 1000) + loTimeb.millitm;
}
 tm*  CBaseThread::SystemTime2LocalTime(INT64 ai64SystemTime)
{
	struct timeb tp;
	tp.time = ai64SystemTime / 1000;
	return  localtime(&( tp.time ));
}

 HANDLE CBaseThread::GetCurrentThreadHandle()
 {
	HANDLE hHandle = GetCurrentThread();
	 DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),
		 &hHandle,0,FALSE,DUPLICATE_SAME_ACCESS);
	 return hHandle;
 }

 void CBaseThread::TerminateThreadEx(HANDLE ahThreadRealHandle)
 {
	 TerminateThread(ahThreadRealHandle,0);
 }
#include "pch.h"
#include "BaseThread.h"


#include <sys/types.h> 
#include <sys/timeb.h>

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

#ifdef WIN32

    unsigned int luThreadID = 0;
    m_hThreadHandle = _beginthreadex(NULL,0,Win32ThreadFunc,lpThreadInfo,0,&luThreadID);
    if (-1 == m_hThreadHandle)
    {
        return FALSE;
    }

    CloseHandle((HANDLE)m_hThreadHandle);
    return TRUE;

#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&m_thread,&attr,LinuxThreadFunc,lpThreadInfo);
    return TRUE;

#endif
}

#ifdef WIN32
unsigned int CBaseThread::Win32ThreadFunc(void * lpParam)
{
    STRU_THREAD_INFO *lpThreadInfo = (STRU_THREAD_INFO *)lpParam;
	lpThreadInfo->mpThreadFunc(lpThreadInfo->mpUserParam);

	if (lpThreadInfo != NULL)
	{
		delete lpThreadInfo;
		lpThreadInfo = NULL;
	}
    _endthreadex(0);
    return 1;
}

#else

void * CBaseThread::LinuxThreadFunc(void * lpParam)
{
    struct STRU_THREAD_INFO *lpThreadInfo = (struct STRU_THREAD_INFO *)lpParam;

    ASSERT(lpThreadInfo != NULL);
    ASSERT(lpThreadInfo->mpThreadFunc != NULL);

    lpThreadInfo->mpThreadFunc(lpThreadInfo->mpUserParam);

	if (lpThreadInfo != NULL)
	{
		delete lpThreadInfo;
		lpThreadInfo = NULL;
	}
    return NULL;
}
#endif

void CBaseThread::Sleep(DWORD dwMilliseconds)
{
#ifdef _WIN32
    ::Sleep(dwMilliseconds);
#else
	DWORD ldwSleep = 1000*dwMilliseconds;
  	usleep(ldwSleep);
#endif
}


INT64 CBaseThread::GetSystemTime()
{
    struct timeb loTimeb;
    //memset(&loTimeb, 0 , sizeof(timeb));
    ftime(&loTimeb);
    return ((INT64)loTimeb.time * 1000) + loTimeb.millitm;
}


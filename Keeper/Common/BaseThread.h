#if !defined(AFX_BASETHREAD_H__A8A55BE7_E276_471E_8B2B_158221B86192__INCLUDED_)
#define AFX_BASETHREAD_H__A8A55BE7_E276_471E_8B2B_158221B86192__INCLUDED_
#include "TypeDeff.h"

#ifdef WIN32
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#else
#include <pthread.h>	
#endif

class CBaseThread  
{
public:
	CBaseThread();
	~CBaseThread();
public:
    BOOL BeginThread(unsigned int (*apStartFunc)(void *),void *apParam);
    void static Sleep(DWORD dwMilliseconds);
    INT64 static GetSystemTime();
    //BOOL Close(WORD awWaitMilliSecond);
private:
    struct STRU_THREAD_INFO
    {
        void * mpUserParam;
        unsigned int (*mpThreadFunc)(void *);
    };
#ifdef WIN32

    uintptr_t m_hThreadHandle;
    static unsigned int __stdcall Win32ThreadFunc(void * apParam);

#else

    pthread_t m_thread;
    static void * LinuxThreadFunc(void * lpParam);

#endif
};

#endif // !defined(AFX_BASETHREAD_H__A8A55BE7_E276_471E_8B2B_158221B86192__INCLUDED_)

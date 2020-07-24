#if !defined(AFX_BASETHREAD_H__)
#define AFX_BASETHREAD_H__
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
enum ThreadStatus
{
	ENUM_START = 0,
	ENUM_STOP  ,
	ENUM_SLEEP  
};
class CBaseThread  
{
public:
	CBaseThread();
	~CBaseThread();
public:
    BOOL BeginThread(unsigned int (*apStartFunc)(void *),void *apParam);
    void static Sleep(DWORD dwMilliseconds);
    INT64 static GetSystemTime();
	static tm* SystemTime2LocalTime(INT64 ai64SystemTime);
    //BOOL Close(WORD awWaitMilliSecond);
	static HANDLE GetCurrentThreadHandle();
	static void TerminateThreadEx(HANDLE ahThreadRealHandle);
private:
    struct STRU_THREAD_INFO
    {
        void * mpUserParam;
        unsigned int (*mpThreadFunc)(void *);
    };

    uintptr_t m_hThreadHandle;
    static unsigned int __stdcall Win32ThreadFunc(void * apParam);
};

#endif // !defined(AFX_BASETHREAD_H__)

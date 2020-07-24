#pragma once
#include <windows.h>
#include "pch.h"
#include <Shlwapi.h>
#include "CriticalSection.h"
#include <map>
#include <string>
using namespace std;
#define  PROCESS_NAME "PortGo_vs2017.exe"
#define  PROCESS_NUMBER 1
class CProcessMgr
{
	map<string, HANDLE> m_mapProcess;
	CCriticalSection  m_oCriticalSection;

	BOOL		m_bIsActive;
	long		m_lThreadCount;
	string		m_strPath;
public:
	CProcessMgr(void);
	~CProcessMgr(void);
	BOOL Init();
	bool Create();
	void Destory();

	BOOL GetCurDir(char* exeFullPath,int aiMaxLen);
private:

	void Check();
	BOOL RunDaemon(void);
	BOOL StartProcess(string strFileName,HANDLE & hProcess) ;
	BOOL CheckProcess(string strFileName,HANDLE & hProcess);

	static unsigned int CheckThread(void *ap);
	void CheckThreadExecute();   


};

#include "pch.h"
#include "ProcessMgr.h"
#include <direct.h>
#include "BaseThread.h"

CProcessMgr::CProcessMgr(void)
{
	m_bIsActive = TRUE;
	m_lThreadCount = 0;
	m_strPath = "";
}

CProcessMgr::~CProcessMgr(void)
{

}

bool CProcessMgr::Create()
{
	HANDLE hProcessHandler = NULL;
	m_mapProcess.insert(make_pair(PROCESS_NAME, hProcessHandler));
	m_bIsActive = TRUE;
	CBaseThread loThread;
	return TRUE == loThread.BeginThread(CheckThread,this);
}

void CProcessMgr::Destory()
{
	m_bIsActive = FALSE;
	TerminateProcess(GetCurrentProcess(), 0);
}

BOOL CProcessMgr::GetCurDir(char* exeFullPath,int aiMaxLen)
{
	TCHAR szTemp[MAX_PATH] = {0};
 	GetModuleFileName(NULL,szTemp,MAX_PATH);
 	*strrchr( szTemp, '\\') = 0;
 	if((int)strlen(szTemp) >= aiMaxLen)
	{
		return FALSE;
 	}
 	strcpy_s(exeFullPath,aiMaxLen,szTemp);
	return TRUE;
}

BOOL CProcessMgr::Init()
{
	CCriticalAutoLock loLock(m_oCriticalSection);
	
	char buff[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buff, MAX_PATH);
	
	PathRemoveFileSpec(buff);
	m_strPath = buff;
	return TRUE;
}
 
BOOL CProcessMgr::StartProcess(string strFileName,HANDLE & hProcess)   
{   
	std::string strTempFileName = m_strPath+"\\" + strFileName;
	//
	/*STARTUPINFO si;   
	PROCESS_INFORMATION pi;   
	memset(&si,0,sizeof(STARTUPINFO));   
	si.cb = sizeof(STARTUPINFO); 
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	
	
	if(CreateProcess(NULL,(LPSTR)strTempFileName.c_str(),NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL, NULL,&si,&pi))
	{   
		hProcess = pi.hProcess;   
		CloseHandle(pi.hThread);   
	}   
	else   
	{   
		hProcess = NULL;   
		return FALSE;   
	}*/
	ShellExecute(NULL, ("OPEN"), strTempFileName.c_str(), NULL, NULL, SW_NORMAL);
	return TRUE;   
} 
BOOL CProcessMgr::CheckProcess(string strFileName,HANDLE & hProcess)   
{   
	DWORD dwRet=0;   
	if(GetExitCodeProcess(hProcess,&dwRet))   
	{   
		if(STILL_ACTIVE == dwRet)   
		{   
			return TRUE;   
		}     
		if( hProcess )  
		{
			CloseHandle(hProcess);  
		}
		hProcess = NULL;   
		return StartProcess(strFileName,hProcess);   
	}   
	return FALSE;   
} 

unsigned int CProcessMgr::CheckThread(void *ap)
{
	if (NULL == ap)
	{
		return 1;
	}
	CProcessMgr* lpThis = (CProcessMgr*)ap;

	lpThis->m_lThreadCount++;
	lpThis->CheckThreadExecute();  
	lpThis->m_lThreadCount--;


	lpThis->Destory();
	return 0;
}

void CProcessMgr::CheckThreadExecute()   
{  

	INT64 i64LoadTimeLast = 0;
	INT64 i64CheckTimeLast = 0; 
	INT64 i64Now = 0;

	while(m_bIsActive)   
	{   
		try
		{
			i64Now = CBaseThread::GetSystemTime();
			if (i64Now - i64LoadTimeLast >= 10 )
			{
				Init();
				i64LoadTimeLast = i64Now;
			}
			if (i64Now - i64CheckTimeLast >= 2000)
			{
				Check();
				i64CheckTimeLast = i64Now;
			}
		}
		catch(...)
		{

		}
		CBaseThread::Sleep(200);  

	}   
} 

void CProcessMgr::Check()
{
	CCriticalAutoLock loLock(m_oCriticalSection);
	
	int iMaxProcess = (int)m_mapProcess.size();

	map<string,HANDLE>::iterator loIter = m_mapProcess.begin();
	for(loIter; m_mapProcess.end() != loIter ;loIter++)   
	{   
		if(0 == loIter->second)   
		{   
			if (StartProcess(loIter->first,loIter->second))
			{
				m_bIsActive = FALSE;
			}		 
		}   
		else   
		{   
			CheckProcess(loIter->first,loIter->second);   
			{
				m_bIsActive = FALSE;
			}
		}   
	}   
}
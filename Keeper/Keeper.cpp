
#include "pch.h"
#include "ProcessMgr.h"
#include <iostream>
//后台运行
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
int main()
{
	CProcessMgr mgrProcess;
	mgrProcess.Create();

	while (true)
	{
		Sleep(10);
	}
}

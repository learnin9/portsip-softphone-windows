#include "stdafx.h"


BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		::DisableThreadLibraryCalls((HMODULE)hModule);

		//IUI::WindowHelper::SetInstance((HINSTANCE)hModule);
		break;
	}

	return TRUE;
}


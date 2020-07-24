#include "stdafx.h"
#include "Global.h"
#include "../RichEditInc/dbug32.h"

PFNASSERTHOOK pfnAssert;
PFNTRACEHOOK pfnTrace;
DWORD dwDebugOptions;

void WINAPI Dbug32AssertSzFn(LPSTR, LPSTR, int)
{

}

void WINAPI TraceMsg(DWORD, DWORD, DWORD, LPSTR, int)
{

}

void WINAPI Tracef(DWORD, LPSTR szFmt, ...)
{

}

void WINAPI TraceError(LPSTR sz, LONG sc)
{

}

void WINAPI TrackBlock(void *pMem)
{

}

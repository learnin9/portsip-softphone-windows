#include "stdafx.h"

#ifdef DEBUG_RICHEDIT
#include "../Control/ControlCore/RichTextBox/RichEditSrc/_common.h"
#include "../Control/ControlCore/RichTextBox/RichEditSrc/_edit.h"
#include "../Control/ControlCore/RichTextBox/RichEditSrc/_host.h"
#include "../Control/ControlCore/RichTextBox/RichEditSrc/_nlsprcs.h"
#include "../Control/ControlCore/RichTextBox/RichEditSrc/_clasfyc.h"
extern UINT gWM_MouseRoller;
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


_IUI_THREAD_STATE::_IUI_THREAD_STATE()
{
#ifdef _DEBUG
	m_nDisablePumpCount = 0;
#endif
	m_msgCur.message = WM_NULL;
	m_nMsgLast = WM_NULL;
	//::GetCursorPos(&(m_ptCursorLast));
	m_ptCursorLast = CIUIPoint(0, 0);
	m_pWLWndInit = NULL;
	m_hHookWLCreate = NULL;

	m_pWLCapture = NULL;
	m_pWLMouseIn = NULL;

	m_pMainWnd = NULL;
	m_pActiveWnd = NULL;
}

_IUI_THREAD_STATE::~_IUI_THREAD_STATE()
{
	// unhook windows hooks
	if (m_hHookOldMsgFilter != NULL)
	{
		::UnhookWindowsHookEx(m_hHookOldMsgFilter);
	}
	if (m_hHookOldCbtFilter != NULL)
	{
		::UnhookWindowsHookEx(m_hHookOldCbtFilter);
	}

	// free safety pool buffer
	if (m_pSafetyPoolBuffer != NULL)
	{
		free(m_pSafetyPoolBuffer);
	}

	// parking window must have already been cleaned up by now!
	_ASSERT(m_pWndPark == NULL);
}

_IUI_THREAD_STATE *IUI::IUIGetThreadState()
{
	_IUI_THREAD_STATE *pState = _iuiThreadState.GetData();
	ENSURE(pState != NULL);
	return pState;
}

CIUIThreadLocal<_IUI_THREAD_STATE> IUI::_iuiThreadState;

HINSTANCE AfxGetInstanceHandle()
{
	_ASSERT(iuiCurrentInstanceHandle != NULL);
	return iuiCurrentInstanceHandle;
}

class _IUI_BASE_MODULE_STATE : public IUI_MODULE_STATE
{
public:
#ifdef _AFXDLL
	_IUI_BASE_MODULE_STATE() : IUI_MODULE_STATE(TRUE, AfxWndProcBase, _MFC_VER)
#else
	_IUI_BASE_MODULE_STATE() : IUI_MODULE_STATE(TRUE)
#endif
	{ }
};

#ifdef _AFXDLL
IUI_MODULE_STATE::IUI_MODULE_STATE(BOOL bDLL, WNDPROC pfnAfxWndProc,
	DWORD dwVersion, BOOL bSystem)
#else
IUI_MODULE_STATE::IUI_MODULE_STATE(BOOL bDLL)
#endif
{
#ifndef _AFX_NO_OLE_SUPPORT
	//m_factoryList.Construct(offsetof(COleObjectFactory, m_pNextFactory));
#endif
	//m_classList.Construct(offsetof(CRuntimeClass, m_pNextClass));

	m_fRegisteredClasses = 0;
	m_bDLL = (BYTE)bDLL;
#ifdef _AFXDLL
	m_pfnAfxWndProc = pfnAfxWndProc;
	m_dwVersion = dwVersion;
	m_bSystem = (BYTE)bSystem;
#endif
	BOOL bEnable = TRUE;
	//TRY
	//{
	//	//Preallocate the registered classes string, but CRT memory leak report is
	//	//called before the string frees memory, so need to disable tracking.
	//	bEnable = AfxEnableMemoryTracking(FALSE);
	//	m_strUnregisterList.Preallocate(4096);
	//	AfxEnableMemoryTracking(bEnable);
	//}
	//CATCH(CMemoryException, e)
	//{
	//	AfxEnableMemoryTracking(bEnable);
	//	DELETE_EXCEPTION(e);
	//}
	//END_CATCH
	// app starts out in "user control"
	m_bUserCtrl = TRUE;

#ifndef _AFX_NO_OCC_SUPPORT
	//m_lockList.Construct(offsetof(COleControlLock, m_pNextLock));
#endif
#ifdef _AFXDLL
	m_libraryList.Construct(offsetof(CDynLinkLibrary, m_pNextDLL));
#endif


	//	bEnable = AfxEnableMemoryTracking(FALSE);
	//	//Fusion: allocate dll wrappers array.
	//	m_pDllIsolationWrappers = new CDllIsolationWrapperBase*[_AFX_ISOLATION_WRAPPER_ARRAY_SIZE];
	//#ifndef _AFX_NO_AFXCMN_SUPPORT
	//	m_pDllIsolationWrappers[_AFX_COMCTL32_ISOLATION_WRAPPER_INDEX] = new CComCtlWrapper;
	//#endif
	//	m_pDllIsolationWrappers[_AFX_COMMDLG_ISOLATION_WRAPPER_INDEX] = new CCommDlgWrapper;
	//	m_pDllIsolationWrappers[_AFX_SHELL_ISOLATION_WRAPPER_INDEX] = new CShellWrapper;
	//	AfxEnableMemoryTracking(bEnable);
	m_bSetAmbientActCtx = TRUE;
	m_hActCtx = NULL;
	m_bInitNetworkAddressControl = FALSE;
	m_bInitNetworkAddressControlCalled = FALSE;
	m_pProject = NULL;
}

IUI_MODULE_STATE::~IUI_MODULE_STATE()
{
#ifndef _AFX_NO_DAO_SUPPORT
	//delete m_pDaoState;
#endif

	// clean up type lib cache map, if any
	//if (m_pTypeLibCacheMap != NULL)
	//{
	//	m_pTypeLibCacheMap->RemoveAll(&m_typeLibCache);
	//	delete m_pTypeLibCacheMap;
	//}
	//Fusion: delete each member of the array and the array itself
	//#ifndef _AFX_NO_AFXCMN_SUPPORT
	//	delete m_pDllIsolationWrappers[_AFX_COMCTL32_ISOLATION_WRAPPER_INDEX];
	//#endif
	//	delete m_pDllIsolationWrappers[_AFX_COMMDLG_ISOLATION_WRAPPER_INDEX];
	//	delete m_pDllIsolationWrappers[_AFX_SHELL_ISOLATION_WRAPPER_INDEX];
	//	delete [] m_pDllIsolationWrappers;
	if (m_hActCtx != NULL && m_hActCtx != INVALID_HANDLE_VALUE)
	{
		IUI_ReleaseActCtx(m_hActCtx);
		m_hActCtx = INVALID_HANDLE_VALUE;
	}
}

IUI_MODULE_THREAD_STATE::IUI_MODULE_THREAD_STATE()
{
	m_nLastHit = static_cast<INT_PTR>(-1);
	m_nLastStatus = static_cast<INT_PTR>(-1);
	m_pLastInfo = NULL;

	//m_frameList.Construct(offsetof(CFrameWnd, m_pNextFrameWnd));

	// Note: it is only necessary to initialize non-zero data
	//m_pfnNewHandler = &AfxNewHandler;
}

IUI_MODULE_THREAD_STATE::~IUI_MODULE_THREAD_STATE()
{
	// cleanup thread local tooltip window
	//if (m_pToolTip != NULL)
	//{
	//	m_pToolTip->DestroyToolTipCtrl();
	//}

	delete m_pLastInfo;

	// cleanup temp/permanent maps (just the maps themselves)
	delete m_pmapHWND;
	//delete m_pmapHMENU;
	//delete m_pmapHDC;
	//delete m_pmapHGDIOBJ;
	//delete m_pmapHIMAGELIST;

	//#ifndef _AFX_NO_SOCKET_SUPPORT
	//	// cleanup socket notification list
	//	if (m_plistSocketNotifications != NULL)
	//	{
	//		while (!m_plistSocketNotifications->IsEmpty())
	//		{
	//			delete m_plistSocketNotifications->RemoveHead();
	//		}
	//	}
	//#ifndef _AFXDLL
	//	// cleanup dynamically allocated socket maps
	//	delete m_pmapSocketHandle;
	//	delete m_pmapDeadSockets;
	//	delete m_plistSocketNotifications;
	//#endif
	//#endif //!_AFX_NO_SOCKET_SUPPORT
}


CIUIProcessLocal<_IUI_BASE_MODULE_STATE> _iuiBaseModuleState;

IUI_MODULE_STATE *IUI::IUIGetModuleState()
{
	_IUI_THREAD_STATE *pState = _iuiThreadState;
	ENSURE(pState);
	IUI_MODULE_STATE *pResult;
	if (pState->m_pModuleState != NULL)
	{
		// thread state's module state serves as override
		pResult = pState->m_pModuleState;
	}
	else
	{
		// otherwise, use global app state
		pResult = _iuiBaseModuleState.GetData();
	}
	ENSURE(pResult != NULL);
	return pResult;
}

IUI_MODULE_THREAD_STATE *IUI::IUIGetModuleThreadState()
{
	IUI_MODULE_THREAD_STATE *pResult = IUIGetModuleState()->m_thread.GetData();
	ENSURE(pResult != NULL);
	return pResult;
}

CIUIWinThread *IUI::IUIGetThread()
{
	// check for current thread in module thread state
	IUI_MODULE_THREAD_STATE *pState = IUIGetModuleThreadState();
	CIUIWinThread *pThread = pState->m_pCurrentWinThread;
	return pThread;
}

ULONG_PTR g_gdiplusToken = 0;

BOOL IUI::IUIInit(__in HINSTANCE hInstance, __in HINSTANCE hPrevInstance,
	__in LPTSTR lpCmdLine, __in int nCmdShow)
{
	_ASSERT(hPrevInstance == NULL);


	// handle critical errors and avoid Windows message boxes
	SetErrorMode(SetErrorMode(0) |
		SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	// 初始化RichEdit
#ifdef DEBUG_RICHEDIT
	// Copy from DllMain in reinit.cpp
	InitializeCriticalSection(&g_CriticalSection);
	InitUnicodeWrappers();			 	// Init UNICODE wrappers for Chicago
	GetSysParms();						// Define screen/window parms
	RegisterFETCs();					// Register new clipboard formats
	CreateFormatCaches();				// Create global format caches
	if (!InitKinsokuClassify())		// Init tables for classifying Unicode chars.
	{
		return FALSE;
	}
	// Magellan Mouse.
	gWM_MouseRoller = RegisterWindowMessageW(MSH_MOUSEWHEEL);
#endif
	InitNLSProcTable();					// Get National Language Support Functions.

	// set resource handles
	IUI_MODULE_STATE *pModuleState = IUIGetModuleState();
	pModuleState->m_hCurrentInstanceHandle = hInstance;
	pModuleState->m_hCurrentResourceHandle = hInstance;
	//pModuleState->CreateActivationContext();

	// fill in the initial state for the application
	//CWinApp *pApp = AfxGetApp();
	//if (pApp != NULL)
	//{
	//	// Windows specific initialization (not done if no CWinApp)
	//	pApp->m_hInstance = hInstance;
	//	hPrevInstance; // Obsolete.
	//	pApp->m_lpCmdLine = lpCmdLine;
	//	pApp->m_nCmdShow = nCmdShow;
	//	pApp->SetCurrentHandles();
	//}

	// initialize thread specific data (for main thread)
	if (!iuiContextIsDLL)
	{
		IUIInitThread();
	}

#if (_MSC_VER <= 1200) // VC6.0
	HMODULE hComCtrl32 = LoadLibrary(_T("ComCtl32.dll"));

	IUI_DPA_InsertPtr = (fnDPA_InsertPtr)GetProcAddress(hComCtrl32, _T("DPA_InsertPtr"));
	_ASSERT(IUI_DPA_InsertPtr != NULL);

	IUI_DPA_DeletePtr = (fnDPA_DeletePtr)GetProcAddress(hComCtrl32, _T("DPA_DeletePtr"));
	_ASSERT(IUI_DPA_DeletePtr != NULL);

	IUI_DPA_GetPtr = (fnDPA_GetPtr)GetProcAddress(hComCtrl32, _T("DPA_GetPtr"));
	_ASSERT(IUI_DPA_GetPtr != NULL);

	IUI_DPA_SetPtr = (fnDPA_SetPtr)GetProcAddress(hComCtrl32, _T("DPA_SetPtr"));
	_ASSERT(IUI_DPA_SetPtr != NULL);

	IUI_DPA_EnumCallback = (fnDPA_EnumCallback)GetProcAddress(hComCtrl32, _T("DPA_EnumCallback"));
	_ASSERT(IUI_DPA_EnumCallback != NULL);

	IUI_DPA_DeleteAllPtrs = (fnDPA_DeleteAllPtrs)GetProcAddress(hComCtrl32, _T("DPA_DeleteAllPtrs"));
	_ASSERT(IUI_DPA_DeleteAllPtrs != NULL);

	IUI_DPA_DestroyCallback = (fnDPA_DestroyCallback)GetProcAddress(hComCtrl32, _T("DPA_DestroyCallback"));
	_ASSERT(IUI_DPA_DestroyCallback != NULL);

	IUI_DPA_Destroy = (fnDPA_Destroy)GetProcAddress(hComCtrl32, _T("DPA_Destroy"));
	_ASSERT(IUI_DPA_Destroy != NULL);

	IUI_DPA_Sort = (fnDPA_Sort)GetProcAddress(hComCtrl32, _T("DPA_Sort"));
	_ASSERT(IUI_DPA_Sort != NULL);

	IUI_DPA_Create = (fnDPA_Create)GetProcAddress(hComCtrl32, _T("DPA_Create"));
	_ASSERT(IUI_DPA_Create != NULL);

	IUI_DPA_CreateEx = (fnDPA_CreateEx)GetProcAddress(hComCtrl32, _T("DPA_CreateEx"));
	_ASSERT(IUI_DPA_CreateEx != NULL);

	IUI_DPA_Grow = (fnDPA_Grow)GetProcAddress(hComCtrl32, _T("DPA_Grow"));
	_ASSERT(IUI_DPA_Grow != NULL);

	IUI_DPA_Clone = (fnDPA_Clone)GetProcAddress(hComCtrl32, _T("DPA_Clone"));
	_ASSERT(IUI_DPA_Clone != NULL);

	IUI_DPA_GetPtrIndex = (fnDPA_GetPtrIndex)GetProcAddress(hComCtrl32, _T("DPA_GetPtrIndex"));
	_ASSERT(IUI_DPA_GetPtrIndex != NULL);

	// DSA_*
	IUI_DSA_Destroy = (fnDSA_Destroy)GetProcAddress(hComCtrl32, _T("DSA_Destroy"));
	_ASSERT(IUI_DSA_Destroy != NULL);

	IUI_DSA_GetItemPtr = (fnDSA_GetItemPtr)GetProcAddress(hComCtrl32, _T("DSA_GetItemPtr"));
	_ASSERT(IUI_DSA_GetItemPtr != NULL);

	IUI_DSA_InsertItem = (fnDSA_InsertItem)GetProcAddress(hComCtrl32, _T("DSA_InsertItem"));
	_ASSERT(IUI_DSA_InsertItem != NULL);

	IUI_DSA_Create = (fnDSA_Create)GetProcAddress(hComCtrl32, _T("DSA_Create"));
	_ASSERT(IUI_DSA_Create != NULL);

	IUI_DSA_GetItem = (fnDSA_GetItem)GetProcAddress(hComCtrl32, _T("DSA_GetItem"));
	_ASSERT(IUI_DSA_GetItem != NULL);

	IUI_DSA_SetItem = (fnDSA_SetItem)GetProcAddress(hComCtrl32, _T("DSA_SetItem"));
	_ASSERT(IUI_DSA_SetItem != NULL);

	IUI_DSA_DeleteItem = (fnDSA_DeleteItem)GetProcAddress(hComCtrl32, _T("DSA_DeleteItem"));
	_ASSERT(IUI_DSA_DeleteItem != NULL);

	IUI_DSA_DestroyCallback = (fnDSA_DestroyCallback)GetProcAddress(hComCtrl32, _T("DSA_DestroyCallback"));
	_ASSERT(IUI_DSA_DestroyCallback != NULL);

	// subclass function

	// comctl32.dll version 5.8 or later
	_SetWindowSubclass = (fnSetWindowSubclass)GetProcAddress(hComCtrl32, _T("SetWindowSubclass"));
	_ASSERT(_SetWindowSubclass != NULL);

	//comctl32.dll version 6.0 or later
	// 	IUI_GetWindowSubclass = (fnGetWindowSubclass)GetProcAddress(hComCtrl32, _T("GetWindowSubclass"));
	// 	_ASSERT(IUI_GetWindowSubclass != NULL);

	// comctl32.dll version 5.8 or later
	_RemoveWindowSubclass = (fnRemoveWindowSubclass)GetProcAddress(hComCtrl32, _T("RemoveWindowSubclass"));
	_ASSERT(_RemoveWindowSubclass != NULL);

	IUI_DefSubclassProc = (fnDefSubclassProc)GetProcAddress(hComCtrl32, _T("DefSubclassProc"));
	_ASSERT(IUI_DefSubclassProc != NULL);

	FreeLibrary(hComCtrl32);
	hComCtrl32 = NULL;

	HMODULE hKernel32 = LoadLibrary("Kernel32.dll");

	IUI_ReleaseActCtx = (fnReleaseActCtx)GetProcAddress(hKernel32, _T("ReleaseActCtx"));
	_ASSERT(IUI_ReleaseActCtx != NULL);

	FreeLibrary(hKernel32);
	hKernel32 = NULL;
#endif // #if (_MSC_VER <= 1200) // VC6.0

	OleInitialize(NULL);

	// Init GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

	// 注册剪贴板格式
	RegisterFETCs();

	return TRUE;
}

BOOL IUI::IUIUninit()
{
	// Release GDI+
	if (g_gdiplusToken != 0)
	{
		Gdiplus::GdiplusShutdown(g_gdiplusToken);
	}
	g_gdiplusToken = NULL;

	IUITermLocalData(NULL, TRUE);

	IUICriticalTerm();
	IUITlsRelease();

	return 0;
}

LRESULT CALLBACK _IUIMsgFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
	CIUIWinThread *pThread;

	if (iuiContextIsDLL
		|| (code < 0 && code != MSGF_DDEMGR)
		|| (pThread = IUIGetThread()) == NULL)
	{
		return ::CallNextHookEx(_iuiThreadState->m_hHookOldMsgFilter,
				code, wParam, lParam);
	}

	_ASSERT(pThread != NULL);
	return (LRESULT)pThread->ProcessMessageFilter(code, (LPMSG)lParam);
}

void IUI::IUIInitThread()
{
	if (!iuiContextIsDLL)
	{
		// set message filter proc
		_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
		_ASSERT(pThreadState->m_hHookOldMsgFilter == NULL);
		pThreadState->m_hHookOldMsgFilter = ::SetWindowsHookEx(WH_MSGFILTER,
				_IUIMsgFilterHook, NULL, ::GetCurrentThreadId());
	}
}

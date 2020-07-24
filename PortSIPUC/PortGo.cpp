// PortGo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#if _DEBUG
//#include <vld.h>
#endif
#include "PortGo.h"
#include "Login/LoginDlg.h"
#include "Main/MainWnd.h"
#include "System/SystemEx.h"
#include "client/windows/handler/exception_handler.h"
#define MAX_LOADSTRING 100

// Global Variables:
CMainWnd g_wndMain;
CMainWnd *PortGo::g_pMainFrame = &g_wndMain;
CIUIProject *g_pProject = NULL;

// Forward declarations of functions included in this code module:
BOOL InitInstance(HINSTANCE, int);
int ExitInstance();
static unsigned int LoginLoadUIThread(void* pParam);
using namespace google_breakpad;
static ExceptionHandler* handler = NULL;

void SelectLauguage(CIUIString strPath, ENUM_LANGUAGE enumSystemDefault)
{
	COptions *pOptions = CSystemEx::GetInstance()->GetGlobalOptions();
	if (pOptions == NULL)
	{
		return;
	}
	if (pOptions->m_bSaveLanguage==false)
	{
		pOptions->m_nLanguage = enumSystemDefault;
		CSystemEx::GetInstance()->UpdateAllOptionData();
	}
	switch (pOptions->m_nLanguage)
	{
	case ENUM_ENGLISH:
		g_pProject->SwitchLanguage(strPath + _T("Enu.lan"));
		break;
	case ENUM_CHINAESE_S:
		g_pProject->SwitchLanguage(strPath + _T("Chs.lan"));
		break;
	case ENUM_CHINAESE_T:
		break;
	case ENUM_ITAlIANA:
		break;
	case ENUM_SPAIN:
		break;
	case ENUM_FRANCH:
		break;
	case ENUM_RUSSIA:
		break;
	case ENUM_PORTUGUESE:
		break;
	default:
		break;
	}
}

bool CrashCallBack(const wchar_t *dump_path, const wchar_t *id,
	void *context, EXCEPTION_POINTERS *exinfo,
	MDRawAssertionInfo *assertion,
	bool succeeded)
{
	if (succeeded) {
		CIUIString strInfo;
		strInfo.Format(_T("crash dump id is %s"), id);
		OutputDebugString(strInfo);
	}
	else {
		OutputDebugString(_T("crash dump failed\n"));
	}
	CSystemEx::GetInstance()->ExitSystem();

	NOTIFYICONDATA tnd;
	//tnd.cbSize = sizeof( NOTIFYICONDATA );//if use sizeof£¬BoundsChecker will be error
	tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
	tnd.hWnd = CSystemEx::GetInstance()->GetMainDlg()->m_hWnd;
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYNOTIFY;
	CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
#ifdef BZXX
	tnd.uID = IDI_BZXX_PG;
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
	tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
#endif

#ifdef KCXX
	tnd.uID = IDI_KCXX_PG;
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
	tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
#endif


	//Shell_NotifyIcon(NIM_DELETE, &tnd);
	return succeeded;
}

int APIENTRY _tWinMain(__in HINSTANCE hInstance,
	__in HINSTANCE hPrevInstance,
	__in LPTSTR    lpCmdLine,
	__in int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	IUIInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	Metrics::SetFloatScrollBar(TRUE);
	int nReturnCode = -1;
	std::string strFullPath = "";
	PortUtility::getCurrentlyPath(strFullPath);

	handler = new google_breakpad::ExceptionHandler (
		PortUtility::string2WString(strFullPath), NULL, CrashCallBack, NULL,
		google_breakpad::ExceptionHandler::HANDLER_ALL);


	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		goto InitFailure;
		nReturnCode = ExitInstance();

		goto InitFailure;
	}



	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PORTGO));

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message != WM_KICKIDLE && !IUI::IUIPreTranslateMessage(&msg))
		{
#if _DEBUG
			//TIMER_START(TranslateMessage);
#endif
			

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			//TIMER_STOP(TranslateMessage);
			//CIUIString strFormatInfo;
			//strFormatInfo.Format(L" TranslateMessage %d ms. current msg %d\n", TIMER_MSEC(TranslateMessage),msg.message);
			//OutputDebugString(strFormatInfo);
		}
	}

	nReturnCode = msg.wParam;
	
	ExitInstance();
	
	IUIUninit();
	TerminateProcess(GetCurrentProcess(), 0);
InitFailure:
	TerminateProcess(GetCurrentProcess(), 0);
	return nReturnCode;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	
	CIUIString strPath = GetRunFolder();
	strPath += _T("Skins\\PortGo\\");

	HANDLE hMutex = CreateMutex(NULL, FALSE,_T( "ONLY_ONE_PROCESS"));
	if (NULL == hMutex)
	{
		LOG4_ERROR("Failed to create Mutex!");
		return false;
	}
	if (hMutex && ERROR_ALREADY_EXISTS == GetLastError())
	{
		MessageBox(NULL, L"Client already exists!", L"Error!", MB_OK);
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}
	g_pProject = new CIUIProject;
	g_pProject->Load(strPath + _T("the.uixproj"));
	CSystemEx::GetInstance()->ReadOptions();
	ENUM_LANGUAGE enumSystemDefault = PortUtility::GetSystemDefaultLang();
	SelectLauguage(strPath, enumSystemDefault);
	CSystemEx::GetInstance()->LoadDynamicText();
	
	CControlProp *pVerSbStyle = g_pProject->GetStyleItem(STYLET_SCROLLBAR,
		_T("IDC_SBEX_VER"));
	_ASSERT(NULL != pVerSbStyle);
	Metrics::SetVerScrollBarBindStyle((CScrollBarProp *)pVerSbStyle);
	Metrics::SetFloatScrollBar(TRUE);
	Metrics::SetScrollBarWidth(11);

	HIUIIMAGE hImgLogging = ImageManager::CreateIUIImage(g_pProject, _T("loading.gif"));
	HIUIIMAGE hImgWaiting = ImageManager::CreateIUIImage(g_pProject, _T("Callerloading.gif"));
	CLoginDlg dlg;
	dlg.SetProject(g_pProject);
	dlg.SetXml(_T("IDW_LOGIN.xml"));



	CSystemEx::GetInstance()->SetLoginDlg(&dlg);
	ImageManager::ReleaseIUIImage(hImgLogging);
	hImgLogging = NULL;

	ImageManager::ReleaseIUIImage(hImgWaiting);
	hImgWaiting = NULL;


	int nRet = dlg.DoModal();
	if (nRet != IDOK)
	{
		return false;
	}
	CSystemEx::GetInstance()->SetLoginDlg(NULL);
	g_wndMain.SetProject(g_pProject);
	
	CBaseThread				oLoginLoadUIThread;
	oLoginLoadUIThread.BeginThread(LoginLoadUIThread, NULL);
	BOOL bRet = g_wndMain.CreatePopupWindow(_T("IDW_MAIN_FRAME.xml"), NULL);


	if (!bRet)
	{
		return FALSE;
	}
	LONG lStyle = GetWindowLong(g_wndMain.GetSafeHwnd(), GWL_STYLE);
	SetWindowLong(g_wndMain.GetSafeHwnd(), GWL_STYLE, lStyle | WS_MAXIMIZEBOX);

	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	pThreadState->m_pMainWnd = &g_wndMain;
	g_wndMain.CenterWindow();
	g_wndMain.ShowWindow(nCmdShow);
	g_wndMain.Invalidate();
	ClosePopupLoginLoadWnd();
	return TRUE;
}

int ExitInstance()
{
	CSystemEx::GetInstance()->ExitSystem();

	//g_pProject->Release();
	//delete g_pProject;
	//g_pProject = NULL;

	
	return 0;
}
unsigned int  LoginLoadUIThread(void* pParam)
{
	PopupLoginLoadWnd(NULL);
	return 0;
}
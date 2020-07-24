// 仿HWND实现
//
//  HWND的本质就是一个窗口过程DefWindowProc，去处理所有消息，并且设置HWND数据结构
//  不同类型的窗口，有各自私有数据结构，通过SetWindowLong(HWND, 0, void *)设置到窗口上。
//  而不同类型的窗口，也有各自私有的窗口过程，私有的窗口过程，处理私有数据。
//  Windowless窗口的数据结构为HWLWND，默认窗口过程为DefEventHandler
//
//  不同类型控件，有不同的私有窗口过程。 私有窗口过程通过RegisterWLClass，在注册windowless
//  控件类时，与控件类名绑定。
//
//  IUI新增了事件路由机制，当某个控件发出一个路由事件后，事件可按指定方式向上、向下或直接由
//  控件到Host窗口之间沿线上的控件处理。 所以，Windowless的窗口过程可以处理三个方向的路由事件

#include "stdafx.h"
#include "../../../Core/DropTargetEx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#define IDT_TRANSLATE_TRANSFORM_X		10
#define IDT_TRANSLATE_TRANSFORM_Y		11
#define IDT_TRANSLATE_TRANSFORM_POS		12

// 模拟	RegisterClass。 把注册过的类信息，存放到全局变量m_gWLClassMap.m_mapWLSuperClasses中。
class CWLClassMap
{
public:
	CWLClassMap() {}
	~CWLClassMap()
	{
		std::map<CIUIString, CONST WLWNDCLASS *>::iterator it = m_mapWLSuperClasses.begin();
		for (; it != m_mapWLSuperClasses.end(); ++it)
		{
			const WLWNDCLASS *p = it->second;
			delete (WLWNDCLASS *)p;
		}
	}

public:
	std::map<CIUIString, CONST WLWNDCLASS *> m_mapWLSuperClasses;
};

CWLClassMap m_gWLClassMap;


BOOL IUI::RegisterWLClass(CONST WLWNDCLASS *lpWndClass)
{
	if (lpWndClass == NULL)
	{
		return FALSE;
	}

	if (m_gWLClassMap.m_mapWLSuperClasses.find(lpWndClass->lpszClassName) != m_gWLClassMap.m_mapWLSuperClasses.end())
	{
		return TRUE;
	}

	WLWNDCLASS *pNew = new WLWNDCLASS;
	*pNew = *lpWndClass;

	m_gWLClassMap.m_mapWLSuperClasses[lpWndClass->lpszClassName] = pNew;

	return TRUE;
}

// 失败返回0，成功返回非0值
int RegisterWindowlessClass(
	LPCTSTR lpszClassName,
	fnWLEventHandler lpfnEventHandler)
{
	if (lpszClassName == NULL || lpfnEventHandler == NULL)
	{
		_ASSERT(FALSE);
		return 0;
	}

	WLWNDCLASS wc = { 0 };
	wc.style = 0;
	wc.lpfnEventHandler = lpfnEventHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = lpszClassName;
	BOOL bRet = RegisterWLClass(&wc);
	if (bRet)
	{
		return 1;
	}

	return 0;
}

int PreRegisterWLClass(LPCTSTR lpszClassName)
{
	if (lpszClassName == NULL)
	{
		return -1;
	}

	// 是否已注册
	BOOL bAlreadyRegist = (m_gWLClassMap.m_mapWLSuperClasses.find(lpszClassName) != m_gWLClassMap.m_mapWLSuperClasses.end());
	if (bAlreadyRegist)
	{
		return 0;
	}

	// 如果是内置类型，在这里注册
	CIUIString strClass = lpszClassName;
	if (strClass == IUI_LABEL)
	{
		RegisterWindowlessClass(IUI_LABEL, Label_WndProc);
		return 0;
	}
	else if (strClass == IUI_BUTTON)
	{
		RegisterWindowlessClass(IUI_BUTTON, Button_WndProc);
		return 0;
	}
	else if (strClass == IUI_RICHTEXTBOX)
	{
		RegisterWindowlessClass(IUI_RICHTEXTBOX, RichTextBox_WndProc);
		return 0;
	}
	else if (strClass == IUI_IM)
	{
		RegisterWindowlessClass(IUI_IM, RichTextBox_WndProc);
		return 0;
	}
	else if (strClass == IUI_COMBOBOX)
	{
		RegisterWindowlessClass(IUI_COMBOBOX, ComboBox_WndProc);
		return 0;
	}
	else if (strClass == IUI_COMBOLBOX)
	{
		_ASSERT(FALSE);
		return 0;
	}
	else if (strClass == IUI_REPEATBUTTON)
	{
		_ASSERT(FALSE);
		return 0;
	}
	else if (strClass == IUI_PROGRESSBAR)
	{
		RegisterWindowlessClass(IUI_PROGRESSBAR, ProgressBar_WndProc);
		return 0;
	}
	else if (strClass == IUI_SLIDER)
	{
		RegisterWindowlessClass(IUI_SLIDER, Slider_WndProc);
		return 0;
	}
	else if (strClass == IUI_COLUMNHEADER)
	{
		RegisterWindowlessClass(IUI_COLUMNHEADER, Header_WndProc);
		return 0;
	}
	else if (strClass == IUI_LISTVIEW)
	{
		RegisterWindowlessClass(IUI_LISTVIEW, ListView_WndProc);
		return 0;
	}
	else if (strClass == IUI_TREEVIEW)
	{
		RegisterWindowlessClass(IUI_TREEVIEW, TV_WndProc);
		return 0;
	}
	else if (strClass == IUI_HTMLVIEW)
	{
		RegisterWindowlessClass(IUI_HTMLVIEW, HtmlView_WndProc);
		return 0;
	}
	else if (strClass == IUI_SPLITTERBAR)
	{
		RegisterWindowlessClass(IUI_SPLITTERBAR, SplitterBar_WndProc);
		return 0;
	}
	else if (strClass == IUI_DATETIMEPICKER)
	{
		RegisterWindowlessClass(IUI_DATETIMEPICKER, DateTimePicker_WndProc);
		return 0;
	}
	else if (strClass == IUI_SPINBUTTON)
	{
		RegisterWindowlessClass(IUI_SPINBUTTON, SpinButton_WndProc);
		return 0;
	}
	else if (strClass == IUI_CANVAS)
	{
		RegisterWindowlessClass(IUI_CANVAS, Canvas_WndProc);
		return 0;
	}
	else if (strClass == IUI_TABCONTROL)
	{
		RegisterWindowlessClass(IUI_TABCONTROL, TabControl_WndProc);
		return 0;
	}
	else if (strClass == IUI_DOCKPANEL)
	{
		RegisterWindowlessClass(IUI_DOCKPANEL, DockPanel_WndProc);
		return 0;
	}
	else if (strClass == IUI_STACKPANEL)
	{
		RegisterWindowlessClass(IUI_STACKPANEL, StackPanel_WndProc);
		return 0;
	}
	else if (strClass == IUI_SCROLLBAR)
	{
		RegisterWindowlessClass(IUI_SCROLLBAR, SB_WndProc);
		return 0;
	}
	else if (strClass == IUI_WINDOW)
	{
		_ASSERT(FALSE);
		return 0;
	}
	else
	{
		_ASSERT(FALSE);
	}

	return -1;
}

fnWLEventHandler GetClassEventHandler(LPCTSTR lpszClassName)
{
	if (lpszClassName == NULL)
	{
		return NULL;
	}

	std::map<CIUIString, CONST WLWNDCLASS *>::iterator it = m_gWLClassMap.m_mapWLSuperClasses.find(lpszClassName);
	if (it == m_gWLClassMap.m_mapWLSuperClasses.end())
	{
		return NULL;
	}

	return it->second->lpfnEventHandler;
}

// 把hWnd从HWLWND树中删除
BOOL IUI::RemoveHWLWND(HWLWND hWnd)
{
	HWLWND hPrev = hWnd->spwndPrev;
	HWLWND hNext = hWnd->spwndNext;

	if (hPrev == NULL)
	{
		if (NULL != hWnd->spwndParent)
		{
			if (hWnd->m_bInBindChild)
			{
				hWnd->spwndParent->spwndBindItemChild = hNext;
			}
			else
			{
				hWnd->spwndParent->spwndChild = hNext;
			}
		}

		if (hNext != NULL)
		{
			hNext->spwndPrev = NULL;
		}
	}
	else
	{
		hPrev->spwndNext = hNext;
		if (hNext != NULL)
		{
			hNext->spwndPrev = hPrev;
		}
	}

	hWnd->spwndPrev = NULL;
	hWnd->spwndNext = NULL;
	hWnd->spwndParent = NULL;

	return TRUE;
}

BOOL AddHWLWND(HWLWND hWnd, HWLWND hParent, BOOL bAddToBindItemList)
{
	if (hParent == NULL || hWnd == NULL)
	{
		return FALSE;
	}

	// 先看一下hParent中，是否已包含hWnd
	BOOL bExist = FALSE;
	HWLWND hChild = NULL;
	if (bAddToBindItemList)
	{
		hChild = GetWindow(hParent, GW_CHILDBINDITEM);
	}
	else
	{
		hChild = GetWindow(hParent, GW_CHILD);
	}
	HWLWND hLast = NULL;
	for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDNEXT))
	{
		if (hChild == hWnd)
		{
			bExist = TRUE;
			break;
		}

		hLast = hChild;
	}

	if (bExist)
	{
		return TRUE;
	}

	hWnd->spwndParent = hParent;
	hWnd->m_bInBindChild = bAddToBindItemList;

	if (hLast == NULL)
	{
		if (bAddToBindItemList)
		{
			hParent->spwndBindItemChild = hWnd;
		}
		else
		{
			hParent->spwndChild = hWnd;
		}
	}
	else
	{
		hLast->spwndNext = hWnd;
		hWnd->spwndPrev = hLast;
	}

	return TRUE;
}

// 释放HWLWND各成员，并初始化
int ReleaseHWLWND(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return -1;
	}

	if (-1 == int(hWnd->m_uID))
	{
		hWnd->m_hHostWnd = NULL;
	}

	return 0;
}

// 请参考xxxCreateWindowEx的逻辑
HWLWND IUI::CreateWindowEx(
	CWLWndHandler *pUserHandler,
	DWORD dwExStyle,
	LPCTSTR lpszClassName,
	LPCTSTR lpszCaption,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hParent,
	HWLWND pParent,
	LPCTSTR lpszName,
	HINSTANCE hInstance,
	LPVOID lpParam)
{
	if (lpszClassName == NULL
		|| lpszName == NULL
		|| !::IsWindow(hParent))
	{
		_ASSERT(FALSE);
		return NULL;
	}

	// 如果pParent为NULL，则设置为窗口虚拟根控件
	if (pParent == NULL)
	{
		HWLWND hVirtualRoot = (HWLWND)::SendMessage(hParent, WM_GET_INTERNAL_ROOT, 0, 0);
		_ASSERT(hVirtualRoot != NULL);
		pParent = hVirtualRoot;
		_ASSERT(pParent->m_hHostWnd == hParent);
	}

	// TODO: 判断hParent是否是IUI Window窗口

	// 如果内置的窗口类未注册，先注册窗口类
	int nRet = PreRegisterWLClass(lpszClassName);
	if (nRet != 0)
	{
		return NULL;
	}

	HWLWND hWnd = new WLWND;
	hWnd->m_strClassName = lpszClassName;
	hWnd->lpfnEventHandler = GetClassEventHandler(lpszClassName);

	// 代替CIInitialize
	hWnd->ci.hwnd = hWnd;
	hWnd->ci.style = dwStyle;
	hWnd->ci.dwExStyle = dwExStyle;
	hWnd->ci.uiCodePage = CP_ACP;
	hWnd->ci.bUnicode = 1;

	hWnd->m_strCaption = lpszCaption;
	hWnd->m_rcItem = CIUIRect(x, y, x + nWidth, y + nHeight);
	MakeControlMargin(hParent, pParent, x, y, nWidth, nHeight, hWnd->m_rcControlMargin);
	hWnd->spwndParent = pParent;
	hWnd->m_strName = lpszName;

	//
	// 加到HWLWND树中
	//
	AddHWLWND(hWnd, pParent, FALSE);

	// CWLWnd *与HWLWND绑定
	_IUI_THREAD_STATE *pThreadState = _iuiThreadState.GetData();
	if (pThreadState->m_pWLWndInit != NULL)
	{
		pThreadState->m_pWLWndInit->Attach(hWnd);
		hWnd->ci.pThis = pThreadState->m_pWLWndInit;

		// 调用Hook函数
		if (pThreadState->m_hHookWLCreate != NULL)
		{
			pThreadState->m_hHookWLCreate(hWnd, NULL);
		}

		pThreadState->m_pWLWndInit = NULL;
	}

	//
	// 发出WM_NCCREATE消息时，窗口本身已经创建好
	// 根据验证，WM_NCCREATE及WM_CREATE中的LPARAM(CREATESTRUCT *)参数
	// 是只读的，外部在处理这两个事件时，即使修改了LPARAM中的值，
	// 【我们也不应该使用这些值】
	// 另外，如果用户响应了WM_NCCREATE事件，并且调用SetStyle设置了新的风格
	// 我们在传递WM_CREATE事件的LPARAM时，仍然要传旧的。
	// 另外，在Win32编程中，CREATESTRUCT::style的值，与GetWindowLong(HWND, GWL_STYLE)
	// 得到的值，并不一定相同。
	//
	CREATESTRUCT cs;
	cs.lpCreateParams = lpParam;
	cs.hInstance = IUIGetInstanceHandle();
	cs.hMenu = 0;
	cs.hwndParent = hParent;
	cs.cx = nWidth;
	cs.cy = nHeight;
	cs.y = y;
	cs.x = x;
	cs.style = dwStyle;
	cs.lpszName = lpszName;
	cs.lpszClass = lpszClassName;
	cs.dwExStyle = dwExStyle;

	CREATESTRUCT csBackup = cs;

#ifdef _DEBUG
	HWLWND hWLParent = GetParent(hWnd);
	IUITRACE(_T("Create %s(%08x), its parent is %s(%08x)\n"),
		lpszClassName, hWnd, hWLParent == NULL ? _T("NULL") : GetClassName(hWLParent), hWLParent);
#endif

	// 分配Handler。 通过为CreateWindowEx指定CWLWndHandler *参数设置Handler比后期
	// 用UE_GET_HANDLER可省节大量的代码，并且也更容易理解，
	// 也比用UE_GET_HANDLER更精确，因为多个窗口创建时的CREATESTRUCT *参数的值可能完全相同
	// 这将导致外部无法区分是哪个窗口在创建。
	// TODO：可以考虑把CWLWndHandler *与HWLWND做成Key-Value，保存到map中，
	// 当CWLWndHandler销毁时，可以自动把HWLWND->m_pUserBindWLWndHandler置NULL，
	// 这样可减小崩溃问题的发生，否则HWLWND->m_pUserBindWLWndHandler很容易成为野指针。
	if (pUserHandler == NULL)
	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = UE_GET_HANDLER;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&csBackup;
		LRESULT lr = RaiseEvent(arg.hOriginalSender, &arg);
		if (arg.bHandled)
		{
			hWnd->m_pUserBindWLWndHandler = (CWLWndHandler *)arg.lResult;
		}
	}
	else
	{
		hWnd->m_pUserBindWLWndHandler = pUserHandler;
	}
	if (hWnd->m_pUserBindWLWndHandler != NULL)
	{
		hWnd->m_pUserBindWLWndHandler->m_hBindWLWnd = hWnd;
	}

	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_NCCREATE;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&cs;
		LRESULT lr = RaiseEvent(arg.hOriginalSender, &arg);
		if (lr != 0)
		{
			DestroyWindow(hWnd);

			return NULL;
		}
	}

	cs = csBackup;

	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_CREATE;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&cs;
		RaiseEvent(arg.hOriginalSender, &arg);
	}

	// 发送窗口移动事件
	{
		RoutedEventArgs arg;
		arg.RoutedEvent = WM_SIZE;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.lParam = MAKELPARAM(hWnd->m_rcItem.Width(), hWnd->m_rcItem.Height());
		RaiseEvent(arg.hOriginalSender, &arg);
	}

	if (x != 0 && y != 0 && nWidth != 0 && nHeight != 0)
	{
		MoveWindow(hWnd, CIUIRect(x, y, x + nWidth, y + nHeight), FALSE);
	}

	{
		RoutedEventArgs arg;
		arg.RoutedEvent = WM_INIT;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.lParam = (LPARAM)&cs;
		arg.lResult = 0;
		arg.bHandled = FALSE;
		RaiseEvent(arg.hOriginalSender, &arg);
	}

	return hWnd;
}

BOOL IUI::DestroyWindow(HWLWND hWnd)
{
	if (!IsWindow(hWnd))
	{
		return FALSE;
	}

	if (IsWindowVisible(hWnd))
	{
		// 不发送事件，否则外面处理这个事件后，再调用控件的ShowWindow(SW_SHOW)会
		// 引起死循环。
		RemoveStyle(hWnd, WS_VISIBLE);
	}

	// 销毁定时器
	// 由于真正的定时器是在HWND上，所以，如果销毁HWLWND时不销毁HWLWND的定时器，
	// 会导致定时器仍然有效，但对应的HWLWND无效的问题，这样，当这个定时器访问HWLWND时，
	// 就会发生崩溃。
	std::map<UINT_PTR, HTIMER>::iterator it = hWnd->m_mapTimer.begin();
	for (; it != hWnd->m_mapTimer.end(); it = hWnd->m_mapTimer.begin())
	{
		WLTIMERINFO *pTimerInfo = (WLTIMERINFO *)(it->second);
		_ASSERT(pTimerInfo != NULL);

		IUI::KillTimer(hWnd, pTimerInfo->uIDEvent);
	}

	// Win32的WM_DESTROY消息：当窗口收到WM_DESTROY消息时，子窗口（如果有）仍然有效
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = WM_DESTROY;
	arg.eRoutingStrategy = RS_BUBBLE;
	RaiseEvent(hWnd, &arg);

	// 销毁子控件
	HWLWND hChild = GetWindow(hWnd, GW_CHILD);
	for (; hChild != NULL; hChild = GetWindow(hWnd, GW_CHILD))
	{
		// 下面的DestroyWindow会把hChild从HWLWND体系中删除，所以for中使用GW_CHILD
		DestroyWindow(hChild);
	}
	HWLWND hChildBind = GetWindow(hWnd, GW_CHILDBINDITEM);
	for (; hChildBind != NULL; hChildBind = GetWindow(hWnd, GW_CHILDBINDITEM))
	{
		DestroyWindow(hChildBind);
	}

	// Win32的WM_NCDESTROY消息：当窗口收到WM_NCDESTROY消息时，子窗口（如果有）已被销毁
	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_NCDESTROY;
		arg.eRoutingStrategy = RS_BUBBLE;
		RaiseEvent(hWnd, &arg);
	}

	// IUI::DestroyWindow只相当于让CWnd的m_hWnd为NULL，但不释放CWnd对象
	// 释放内存放到Window类的析构函数中。
	if (-1 == int(hWnd->m_uID))
	{
		hWnd->m_hHostWnd = NULL;
	}

	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	if (pThreadState != NULL)
	{
		if (pThreadState->m_pWLMouseIn == hWnd)
		{
			pThreadState->m_pWLMouseIn = NULL;
		}

		if (pThreadState->m_pWLCapture == hWnd)
		{
			pThreadState->m_pWLCapture = NULL;
		}
	}

	// TODO: 研究控件自己删除时，父控件中是否还有自己，父控件什么时候没有的自己。
	// 把下面这段代码，放到合适的时机
	// 把自己从HWLWND树中删除
	RemoveHWLWND(hWnd);
	delete hWnd;

	return TRUE;
}

BOOL IUI::IsWindow(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	_ASSERT(NULL != hWnd->spwndParent->spwndChild || NULL != hWnd->spwndParent->spwndBindItemChild);

	if (hWnd->spwndPrev != NULL)
	{
		_ASSERT(hWnd->spwndPrev->spwndNext == hWnd);
	}

	if (hWnd->spwndNext != NULL)
	{
		_ASSERT(hWnd->spwndNext->spwndPrev == hWnd);
	}

	return GetHostWnd(hWnd) == NULL ? FALSE : TRUE;
}

HWLWND IUI::SetBindItemControlParent(
	HWLWND hBindItemControl,
	HWLWND hWndNewParent,
	DWORD_PTR dwBindItemData)
{
	if (NULL == hBindItemControl || NULL == hWndNewParent)
	{
		return NULL;
	}

	HWLWND hOld = hBindItemControl->spwndParent;
	if (NULL != hOld && (int)hOld->m_uID == -1)
	{
		// 如果本控件是根控件，不允许设置成其它子控件的孩子
		return NULL;
	}

	// 从原父控件中移除
	RemoveHWLWND(hBindItemControl);

	AddHWLWND(hBindItemControl, hWndNewParent, TRUE);

	hBindItemControl->m_dwBindItemData = dwBindItemData;

	return hOld;
}

ULONG_PTR IUI::GetBindItemControlData(HWLWND hBindItemControl)
{
	if (NULL == hBindItemControl)
	{
		return NULL;
	}

	if (-1 == (int)hBindItemControl->m_uID)
	{
		return NULL;
	}

	return hBindItemControl->m_dwBindItemData;
}

HWLWND IUI::SetParent(HWLWND hWndChild, HWLWND hWndNewParent)
{
	if (hWndChild == NULL || hWndNewParent == NULL)
	{
		return NULL;
	}

	HWLWND hOld = hWndChild->spwndParent;
	if (NULL != hOld && (int)hOld->m_uID == -1)
	{
		// 如果本控件是根控件，不允许设置成其它子控件的孩子
		return NULL;
	}

	if (hOld == hWndNewParent)
	{
		return hOld;
	}

	// 从原父控件中移除
	RemoveHWLWND(hWndChild);

	AddHWLWND(hWndChild, hWndNewParent, FALSE);

	// 可能hWndChild和hWndNewParent不在同一个Host窗口内，
	// 但是我们从虚拟根控件取Host窗口句柄，所以，不需要更改Host窗口。
	return hOld;
}

HWLWND IUI::GetParent(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return NULL;
	}

	// 自由控件的父控件为NULL
	if (hWnd->spwndParent == NULL || hWnd->spwndParent->m_uID == (UINT)(-1))
	{
		return NULL;
	}

	return hWnd->spwndParent;
}

HWLWND IUI::GetOwner(HWLWND hWnd)
{
	return GetWindow(hWnd, GW_OWNER);
}

int IUI::SetZOrder(HWLWND hWnd, HWLWND hAfter)
{
	if (hWnd == NULL || hAfter == NULL || hAfter == hWnd)
	{
		return 0;
	}

	if (hWnd->spwndParent == NULL)
	{
		return 1;
	}

	if (hAfter == (HWLWND)HWND_TOP)
	{
		// hWnd已经是Top了(最后创建的在最上面)
		if (NULL == hWnd->spwndNext)
		{
			return 0;
		}

		// 备份三个节点
		HWLWND hOldParentChild = hWnd->m_bInBindChild
			? hWnd->spwndBindItemChild : hWnd->spwndParent->spwndChild;
		_ASSERT(hOldParentChild != NULL);
		HWLWND hOldPrev = hWnd->spwndPrev;
		HWLWND hOldNext = hWnd->spwndNext;
		_ASSERT(hOldNext != NULL);

		if (hOldPrev != NULL)
		{
			hOldPrev->spwndNext = hOldNext;
		}

		// 如果hWnd是父控件第一个孩子，则父控件第一个孩子需重新指向
		if (hWnd == hOldParentChild)
		{
			hWnd->spwndPrev = NULL;
			if (hWnd->m_bInBindChild)
			{
				hWnd->spwndParent->spwndBindItemChild = hOldNext;
			}
			else
			{
				hWnd->spwndParent->spwndChild = hOldNext;
			}
		}

		hOldNext->spwndPrev = hOldPrev;

		// 加到末尾
		AddHWLWND(hWnd, hWnd->spwndParent, FALSE);

		hWnd->spwndNext = NULL;
	}
	else if (hAfter == (HWLWND)HWND_BOTTOM)
	{
		// hWnd已经是Bottom了。
		if (hWnd->spwndPrev == NULL)
		{
			return 0;
		}

		// 备份三个节点
		HWLWND hOldParentChild = hWnd->m_bInBindChild
			? hWnd->spwndParent->spwndBindItemChild : hWnd->spwndParent->spwndChild;
		_ASSERT(hOldParentChild != NULL);
		HWLWND hOldPrev = hWnd->spwndPrev;
		_ASSERT(hOldPrev != NULL);
		HWLWND hOldNext = hWnd->spwndNext;

		// 重新指向
		if (hWnd->m_bInBindChild)
		{
			hWnd->spwndParent->spwndBindItemChild = hWnd;
		}
		else
		{
			hWnd->spwndParent->spwndChild = hWnd;
		}

		hWnd->spwndPrev = NULL;
		hWnd->spwndNext = hOldParentChild;

		hOldParentChild->spwndPrev = hWnd;

		hOldPrev->spwndNext = hOldNext;

		if (hOldNext != NULL)
		{
			hOldNext->spwndPrev = hOldPrev;
		}
	}
	else
	{
		// TODO: 重新指向
		_ASSERT(FALSE);
		RemoveHWLWND(hWnd);
		hAfter->spwndNext = hWnd;
	}

	return 0;
}

CIUIString IUI::GetName(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return _T("");
	}

	return hWnd->m_strName;
}

CIUIString IUI::SetName(HWLWND hWnd, LPCTSTR pstrName)
{
	CIUIString strOld = hWnd->m_strName;
	hWnd->m_strName = pstrName;
	return strOld;
}

int IUI::SetNotify(HWLWND hWnd, BOOL bNotify)
{
	hWnd->m_bNotify = bNotify;

	return 0;
}

BOOL IUI::IsNotify(HWLWND hWnd)
{
	return hWnd->m_bNotify;
}

LONG IUI::SetStyle(HWLWND hWnd, LONG lStyle)
{
	LONG lOldStyle = hWnd->ci.style;

	if (lOldStyle == lStyle)
	{
		return lOldStyle;
	}

	STYLESTRUCT ss;
	ss.styleOld = lOldStyle;
	ss.styleNew = lStyle;

	// WM_STYLECHANGING
	if (hWnd->m_bNotify)
	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_STYLECHANGING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = GWL_STYLE;
		arg.lParam = (LPARAM)&ss;
		LRESULT lr = RaiseEvent(hWnd, &arg);
	}

	// WM_STYLECHANGING事件中，可能修改过的窗口风格
	hWnd->ci.style = ss.styleNew;

	// WM_STYLECHANGED
	if (hWnd->m_bNotify)
	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_STYLECHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = GWL_STYLE;
		arg.lParam = (LPARAM)&ss;
		LRESULT lr = RaiseEvent(hWnd, &arg);
	}

	return lOldStyle;
}

LONG IUI::GetStyle(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return 0;
	}

	return hWnd->ci.style;
}

LONG IUI::AddStyle(HWLWND hWnd, LONG lAdd)
{
	LONG lOld = hWnd->ci.style;

	LONG lNew = (lOld | lAdd);

	SetStyle(hWnd, lNew);

	return lOld;
}

LONG IUI::RemoveStyle(HWLWND hWnd, LONG lRemove)
{
	LONG lOld = hWnd->ci.style;

	LONG lNew = lOld;
	lNew &= ~lRemove;

	SetStyle(hWnd, lNew);

	return lOld;
}

LONG IUI::SetExStyle(HWLWND hWnd, LONG lExStyle)
{
	LONG lOld = hWnd->ci.dwExStyle;

	if (lOld == lExStyle)
	{
		return lOld;
	}

	STYLESTRUCT ss;
	ss.styleOld = lOld;
	ss.styleNew = lExStyle;

	// WM_STYLECHANGING
	if (hWnd->m_bNotify)
	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_STYLECHANGING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = GWL_EXSTYLE;
		arg.lParam = (LPARAM)&ss;
		LRESULT lr = RaiseEvent(hWnd, &arg);
	}

	// WM_STYLECHANGING事件中，可能修改过的窗口扩展风格
	hWnd->ci.dwExStyle = ss.styleNew;

	// WM_STYLECHANGED
	if (hWnd->m_bNotify)
	{
		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_STYLECHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = GWL_EXSTYLE;
		arg.lParam = (LPARAM)&ss;
		LRESULT lr = RaiseEvent(hWnd, &arg);
	}

	return lOld;
}

LONG IUI::GetExStyle(HWLWND hWnd)
{
	return hWnd->ci.dwExStyle;
}

LONG IUI::AddExStyle(HWLWND hWnd, LONG lAdd)
{
	LONG lOld = hWnd->ci.dwExStyle;

	LONG lNew = (lOld | lAdd);

	SetExStyle(hWnd, lNew);

	Invalidate(hWnd);

	return lOld;
}

LONG IUI::RemoveExStyle(HWLWND hWnd, LONG lRemove)
{
	LONG lOld = hWnd->ci.dwExStyle;

	LONG lNew = lOld;
	lNew &= ~lRemove;

	SetExStyle(hWnd, lNew);

	Invalidate(hWnd);

	return lOld;
}

HWND IUI::GetHostWnd(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return NULL;
	}

	// 从虚拟根控件取Host窗口句柄
	HWND hRet = NULL;
	HWLWND hParent = hWnd;
	while (NULL != hParent)
	{
		if (-1 == int(hParent->m_uID))
		{
			hRet = hParent->m_hHostWnd;
			break;
		}

		hParent = hParent->spwndParent;
	}

	return hRet;
}

CIUIString IUI::GetClassName(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return _T("");
	}

	return hWnd->m_strClassName;
}

int IUI::GetClassName(HWLWND hWnd, LPTSTR lpClassName, int nMaxCount)
{
	if (hWnd == NULL)
	{
		return 0;
	}

	int nTextLen = hWnd->m_strClassName.GetLength();
	StringCchCopy(lpClassName, nMaxCount, hWnd->m_strClassName);

	if (nTextLen >= nMaxCount)
	{
		return nMaxCount - 1;
	}
	else
	{
		return nTextLen;
	}
}

int IUI::SetDlgCtrlID(HWLWND hWnd, int nID)
{
	hWnd->m_uID = nID;

	return 0;
}

int IUI::GetDlgCtrlID(HWLWND hWnd)
{
	return hWnd->m_uID;
}

CIUIProject *IUI::GetProject(HWLWND hWnd)
{
	_ASSERT(GetHostWnd(hWnd) != NULL);
	return (CIUIProject *)::SendMessage(GetHostWnd(hWnd), WM_GET_PROJECT, 0, 0);
}

// xxxSetCapture
// 注意：Win32中，Disabled窗口或不可见窗口也可以捕获鼠标
// 经验证，这两种窗口捕获鼠标后，也可以响应鼠标消息了。
HWLWND IUI::SetCapture(HWLWND hWnd)
{
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	HWLWND hOld = pThreadState->m_pWLCapture;

	// xxxCapture
	{
		if (hWnd == NULL && hOld != NULL)
		{
			//zzzSetFMouseMoved();
		}

		// send  WM_CAPTURECHANGED.
		if (hOld != NULL)
		{
			SendMessage(hOld, WM_CAPTURECHANGED, FALSE, LPARAM(hWnd));
		}

		RoutedEventArgs arg;
		arg.hSender = hWnd;
		arg.hOriginalSender = hWnd;
		arg.RoutedEvent = UE_SETCAPTURE;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = (WPARAM)hWnd;
		arg.lParam = (LPARAM)hOld;
		LRESULT lr = RaiseEvent(hWnd, &arg);
		// 父亲禁止设置Capture
		if (lr != 0)
		{
			return hOld;
		}

		HWND hOld = ::GetCapture();
		HWND hHost = GetHostWnd(hWnd);
		if (hOld != hHost)
		{
			::SetCapture(hHost);
		}

		pThreadState->m_pWLCapture = hWnd;
	}

	return hOld;
}

HWLWND IUI::GetCapture(HWLWND hWnd)
{
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	return pThreadState->m_pWLCapture;
}

BOOL IUI::ReleaseCapture(HWLWND hWnd)
{
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	HWLWND hOld = pThreadState->m_pWLCapture;
	pThreadState->m_pWLCapture = NULL;

	RoutedEventArgs arg;
	arg.hSender = hOld;
	arg.hOriginalSender = hOld;
	arg.RoutedEvent = WM_CAPTURECHANGING;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)GetCapture(hWnd);
	LRESULT lr = RaiseEvent(hOld, &arg);
	if (!arg.bHandled && lr == 0)
	{
		// ComboBox下拉列表中，如果有自定义的滚动条，在滚动条停止滚动后，
		// 会调用IUI::ReleaseCapture释放鼠标，但这会导致把下拉列表框的鼠标捕获释放掉
		// 通过处理WM_CAPTURECHANGING，可禁止释放掉下拉列表框的鼠标捕获。
		::ReleaseCapture();
	}

	if (hOld != NULL)
	{
		RoutedEventArgs arg;
		arg.hSender = hOld;
		arg.hOriginalSender = hOld;
		arg.RoutedEvent = WM_CAPTURECHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)GetCapture(hWnd);// TODO, GetCapture不应该带HWLWND参数
		LRESULT lr = RaiseEvent(hOld, &arg);
	}

	return TRUE;
}

HWLWND IUI::GetTopLevelWindow(HWLWND hWnd)
{
	_ASSERT(IsWindow(hWnd));

	HWLWND hWndParent = hWnd;
	HWLWND hWndTmp = hWnd;

	do
	{
		hWndParent = hWndTmp;
		hWndTmp = GetParent(hWndParent);
	}
	while (hWndTmp != NULL && hWndTmp->m_uID != UINT(-1));

	return hWndParent;
}

int IUI::GetWindowTextLength(HWLWND hWnd)
{
	_ASSERT(IsWindow(hWnd));
	return SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0);
}

CIUIString IUI::GetWindowText(HWLWND hWnd)
{
	if (!IsWindow(hWnd))
	{
		return _T("");
	}

	int nLen = GetWindowTextLength(hWnd);
	nLen = max(4, nLen + 1);
	TCHAR *p = new TCHAR[nLen];
	p[0] = 0;

	// 编译为ANSI，当向RichEdit发送WM_GETTEXT时，由于RichEdit内部使用Unicode，
	// 内部可能会把传入的长度除以2再减去1，为了使结果大于0，传入的最小长度为4.
	// 否则在释放p的时候，可能会引起断言错误。
	SendMessage(hWnd, WM_GETTEXT, nLen, (LPARAM)p);

	CIUIString rString = p;
	delete []p;
	p = NULL;
	return rString;
}

int IUI::GetWindowText(HWLWND hWnd, LPTSTR lpString, int nMaxCount)
{
	if (!IsWindow(hWnd))
	{
		return 0;
	}

	int nLen = GetWindowTextLength(hWnd);

	nLen = max(4, nLen + 1);
	TCHAR *p = new TCHAR[nLen];
	p[0] = 0;

	// 编译为ANSI，当向RichEdit发送WM_GETTEXT时，由于RichEdit内部使用Unicode，
	// 内部可能会把传入的长度除以2再减去1，为了使结果大于0，传入的最小长度为4.
	// 否则在释放p的时候，可能会引起断言错误。
	SendMessage(hWnd, WM_GETTEXT, nLen, (LPARAM)p);

	size_t nTextLen = 0;
	StringCchLength(p, nLen, &nTextLen);

	StringCchCopy(lpString, nMaxCount, p);
	delete[]p;
	p = NULL;

	if (nTextLen >= (size_t)nMaxCount)
	{
		return nMaxCount - 1;
	}
	else
	{
		return nTextLen;
	}
}

BOOL IUI::SetWindowText(HWLWND hWnd, LPCTSTR pstrText)
{
	if (pstrText == NULL)
	{
		return FALSE;
	}

	return SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)pstrText);
}

BOOL IUI::SetResText(HWLWND hWnd, HINSTANCE hRes, UINT uID)
{
	CIUIString strText;
	BOOL bRet = strText.LoadString(uID);

	if (!bRet)
	{
		return FALSE;
	}

	SetWindowText(hWnd, strText);

	return TRUE;
}

#if (_MSC_VER > 1200)	// VC6.0
BOOL IUI::SetXResText(HWLWND hWnd, HXSRESOURCE hXRes, LPCTSTR lpszSectionID, LPCTSTR lpszStringID)
{
	LPCWSTR lpszString = CParseLanguageFile::LoadXMLString(hXRes, lpszSectionID, lpszStringID);

	if (lpszString != NULL)
	{
		USES_CONVERSION;
		IUI::SetWindowText(hWnd, W2T((LPWSTR)lpszString));
	}

	return TRUE;
}
#endif // #if (_MSC_VER > 1200)	// VC6.0

RECT IUI::GetTextPadding(HWLWND hWnd)
{
	return hWnd->m_rcTextPadding;
}

int IUI::SetTextPadding(HWLWND hWnd, RECT rc)
{
	hWnd->m_rcTextPadding = rc;

	Invalidate(hWnd);

	return 0;
}

int IUI::SetTextColor(HWLWND hWnd, COLORREF crN, COLORREF crD)
{
	_ASSERT(IsWindow(hWnd));

	SendMessage(hWnd, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(hWnd, WLM_SET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	Invalidate(hWnd);

	return 0;
}

int IUI::GetTextColor(HWLWND hWnd, COLORREF *pcrN, COLORREF *pcrD)
{
	_ASSERT(IsWindow(hWnd));

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(hWnd, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(hWnd, WLM_GET_TEXT_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

BOOL IUI::SetDrawBkColor(HWLWND hWnd, BOOL bDraw)
{
	BOOL bOld = hWnd->m_bDrawBkColor;

	hWnd->m_bDrawBkColor = bDraw;

	return bOld;
}

BOOL IUI::IsDrawBkColor(HWLWND hWnd)
{
	return hWnd->m_bDrawBkColor;
}

int IUI::SetBkColor(HWLWND hWnd, COLORREF crBkN, COLORREF crBkD)
{
	_ASSERT(IsWindow(hWnd));

	SendMessage(hWnd, WLM_SET_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crBkN);
	SendMessage(hWnd, WLM_SET_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crBkD);

	Invalidate(hWnd);

	return 0;
}

int IUI::GetBkColor(HWLWND hWnd, COLORREF *pcrBkN, COLORREF *pcrBkD)
{
	_ASSERT(IsWindow(hWnd));

	if (pcrBkN != NULL)
	{
		*pcrBkN = SendMessage(hWnd, WLM_GET_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrBkD != NULL)
	{
		*pcrBkD = SendMessage(hWnd, WLM_GET_BK_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

BOOL IUI::CombineBkImage(HWLWND hWnd, BOOL bCombine)
{
	BOOL bOld = hWnd->ci.m_bCombineBkImage;
	hWnd->ci.m_bCombineBkImage = bCombine;
	return bOld;
}

BOOL IUI::IsCombineBkImage(HWLWND hWnd)
{
	return hWnd->ci.m_bCombineBkImage;
}

int IUI::SetCombineBkImage(HWLWND hWnd, LPCTSTR lpszImage)
{
	// TODO: 应该发WLM_SET_BK_IMAGE消息，并且控件中应该响应这个消息
	return IUISetControlImage(GetProject(hWnd), &hWnd->ci.m_himgCombineBk, lpszImage);
}

int IUI::GetCombineBkImage(HWLWND hWnd, CIUIString *pstrCombineImage)
{
	if (pstrCombineImage == NULL)
	{
		return -1;
	}

	*pstrCombineImage = hWnd->ci.m_himgCombineBk->GetSafeImageName();

	return 0;
}

int IUI::SetBkImage(HWLWND hWnd, BOOL bChecked, CONTROL_STATE eControlState, LPCTSTR lpszImage)
{
	return SendMessage(hWnd, WLM_SET_BK_IMAGE,
			MAKEWPARAM(bChecked ? BST_CHECKED : BST_UNCHECKED, eControlState), LPARAM(lpszImage));
}

int IUI::GetBkImage(HWLWND hWnd, BOOL bChecked, CONTROL_STATE eControlState, CIUIString *pstrImage)
{
	if (pstrImage == NULL)
	{
		return -1;
	}

	*pstrImage = (LPCTSTR)SendMessage(hWnd, WLM_GET_BK_IMAGE,
			MAKEWPARAM(bChecked ? BST_CHECKED : BST_UNCHECKED, eControlState), 0);

	return 0;
}

int IUI::SetBkImageResizeMode(HWLWND hWnd, IMAGE_RESIZE_MODE eImageResizeMode)
{
	if (!IsWindow(hWnd))
	{
		return -1;
	}

	hWnd->ci.m_eBkImageResizeMode = eImageResizeMode;
	return 0;
}

IMAGE_RESIZE_MODE IUI::GetBkImageResizeMode(HWLWND hWnd)
{
	if (!IsWindow(hWnd))
	{
		return IRM_UNKNOWN;
	}

	return hWnd->ci.m_eBkImageResizeMode;
}

int IUI::SetBkImage9GridResizeParam(HWLWND hWnd, LPCRECT lprcParam)
{
	if (!IsWindow(hWnd))
	{
		return -1;
	}

	if (lprcParam == NULL)
	{
		return -2;
	}

	hWnd->ci.m_rcBkImage9GridResizeArg = lprcParam;
	return 0;
}

int IUI::GetBkImage9GridResizeParam(HWLWND hWnd, LPRECT lprcParam)
{
	if (!IsWindow(hWnd))
	{
		return -1;
	}

	if (lprcParam == NULL)
	{
		return -2;
	}

	*lprcParam = hWnd->ci.m_rcBkImage9GridResizeArg;
	return 0;
}

BOOL IUI::CombineFgImage(HWLWND hWnd, BOOL bCombine)
{
	return SendMessage(hWnd, WLM_COMBINE_FG_IMAGE, 0, bCombine);
}

BOOL IUI::IsCombineFgImage(HWLWND hWnd)
{
	return SendMessage(hWnd, WLM_IS_COMBINE_FG_IMAGE, 0, 0);
}

int IUI::SetCombineFgImage(HWLWND hWnd, LPCTSTR lpszImage)
{
	return SendMessage(hWnd, WLM_SET_FG_IMAGE,
			MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_COMBINE), LPARAM(lpszImage));
}

int IUI::GetCombineFgImage(HWLWND hWnd, CIUIString *pstrCombineImage)
{
	if (pstrCombineImage == NULL)
	{
		return -1;
	}

	*pstrCombineImage = (LPCTSTR)SendMessage(hWnd, WLM_GET_FG_IMAGE,
			MAKEWPARAM(BST_UNCHECKED, CONTROL_STATE_COMBINE), 0);

	return 0;
}

int IUI::SetFgImage(HWLWND hWnd, BOOL bChecked, CONTROL_STATE eControlState, LPCTSTR lpszImage)
{
	return SendMessage(hWnd, WLM_SET_FG_IMAGE,
			MAKEWPARAM(bChecked ? BST_CHECKED : BST_UNCHECKED, eControlState), LPARAM(lpszImage));
}

int IUI::GetFgImage(HWLWND hWnd, BOOL bChecked, CONTROL_STATE eControlState, CIUIString *pstrImage)
{
	if (pstrImage == NULL)
	{
		return -1;
	}

	*pstrImage = (LPCTSTR)SendMessage(hWnd, WLM_GET_FG_IMAGE,
			MAKEWPARAM(bChecked ? BST_CHECKED : BST_UNCHECKED, eControlState), 0);

	return 0;
}

BOOL IUI::SetDrawBorderColor(HWLWND hWnd, BOOL bDraw)
{
	BOOL bOld = hWnd->m_bDrawBorderColor;
	hWnd->m_bDrawBorderColor = bDraw;

	Invalidate(hWnd);

	return bOld;
}

BOOL IUI::IsDrawBorderColor(HWLWND hWnd)
{
	return hWnd->m_bDrawBorderColor;
}

int IUI::SetBorderColor(HWLWND hWnd, COLORREF crN, COLORREF crD)
{
	_ASSERT(IsWindow(hWnd));

	SendMessage(hWnd, WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), crN);
	SendMessage(hWnd, WLM_SET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), crD);

	Invalidate(hWnd);

	return 0;
}

int IUI::GetBorderColor(HWLWND hWnd, COLORREF *pcrN, COLORREF *pcrD)
{
	_ASSERT(IsWindow(hWnd));

	if (pcrN != NULL)
	{
		*pcrN = SendMessage(hWnd, WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_NORMAL), 0);
	}
	if (pcrD != NULL)
	{
		*pcrD = SendMessage(hWnd, WLM_GET_BORDER_COLOR, MAKEWPARAM(0, CONTROL_STATE_DISABLED), 0);
	}

	return 0;
}

DWORD IUI::GetFocusBorderColor(HWLWND hWnd)
{
	return hWnd->m_dwFocusBorderColor;
}

void IUI::SetFocusBorderColor(HWLWND hWnd, DWORD dwBorderColor)
{
	if (hWnd->m_dwFocusBorderColor == dwBorderColor)
	{
		return;
	}

	hWnd->m_dwFocusBorderColor = dwBorderColor;
	Invalidate(hWnd);
}

int IUI::SetBorderRect(HWLWND hWnd, const RECT &rcBorder)
{
	hWnd->m_rcBorder = rcBorder;
	return 0;
}

CIUIRect IUI::GetBorderRect(HWLWND hWnd)
{
	return hWnd->m_rcBorder;
}

void IUI::SetBorderRound(HWLWND hWnd, SIZE cxyRound)
{
	hWnd->m_cxyBorderRound = cxyRound;
	Invalidate(hWnd);
}

SIZE IUI::GetBorderRound(HWLWND hWnd)
{
	return hWnd->m_cxyBorderRound;
}

int IUI::SetWindowRgn(HWLWND hWnd, HRGN hRgn, BOOL bRedraw)
{
	WINDOWPOS wp;
	wp.cx = hWnd->m_rcItem.Width();
	wp.cy = hWnd->m_rcItem.Height();
	wp.flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER;
	wp.hwnd = (HWND)hWnd;
	wp.hwndInsertAfter = NULL;
	wp.x = hWnd->m_rcItem.left;
	wp.y = hWnd->m_rcItem.top;

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	hWnd->m_hrgnClip = hRgn;

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	if (bRedraw)
	{
		Invalidate(hWnd);
	}

	return 0;
}

int IUI::GetWindowRgn(HWLWND hWnd, HRGN hRgn)
{
	if (NULL == hWnd)
	{
		return -1;
	}

	hRgn = hWnd->m_hrgnClip;

	return 0;
}

int IUI::RelayoutChild(HWLWND hWnd)
{
	CanvasLayoutCtrl(hWnd);

	return 0;
}

void IUI::EnableDPI(HWLWND hWnd, BOOL bEnable)
{
	if (NULL == hWnd)
	{
		return;
	}

	hWnd->m_bEnableDPI = bEnable;
}

BOOL IUI::IsDPIEnabled(HWLWND hWnd)
{
	if (NULL == hWnd)
	{
		return FALSE;
	}

	return hWnd->m_bEnableDPI;
}

const CIUIRect &IUI::GetWindowRectToParent(HWLWND hWnd)
{
	if (NULL == hWnd)
	{
		_ASSERT(FALSE);
	}

	return hWnd->m_rcItem;
}

int IUI::GetWindowRectToParent(HWLWND hWnd, LPRECT lprc)
{
	if (NULL == hWnd || NULL == lprc)
	{
		return -1;
	}

	*lprc = hWnd->m_rcItem;

	return 0;
}

int IUI::GetWindowRectFToParent(HWLWND hWnd, Gdiplus::RectF *pRect)
{
	return SendMessage(hWnd, WM_WL_GETRECTF, 0, (LPARAM)pRect);
}

int IUI::ClientToScreen(HWLWND hWnd, LPRECT lpRect)
{
	if (hWnd == NULL || lpRect == NULL)
	{
		return -1;
	}

	CIUIRect rcThis;
	GetWindowRectToHWND(hWnd, rcThis);
	CIUIRect rcHost;
	::GetWindowRect(GetHostWnd(hWnd), rcHost);

	// 得到自己的屏幕坐标
	rcThis.OffsetRect(rcHost.TopLeft());

	CIUIRect rcNew = lpRect;
	rcNew.OffsetRect(rcThis.TopLeft());

	*lpRect = rcNew;

	return 0;
}

int IUI::ClientToScreen(HWLWND hWnd, LPPOINT lpPt)
{
	if (lpPt == NULL)
	{
		return -1;
	}

	CIUIRect rcThis;
	GetWindowRectToHWND(hWnd, rcThis);
	CIUIRect rcHost;
	::GetWindowRect(GetHostWnd(hWnd), rcHost);

	// 得到自己的屏幕坐标
	rcThis.OffsetRect(rcHost.TopLeft());

	CIUIPoint ptNew = *lpPt;
	ptNew += rcThis.TopLeft();

	*lpPt = ptNew;

	return 0;
}

int IUI::ScreenToClient(HWLWND hWnd, LPRECT lpRect)
{
	if (lpRect == NULL)
	{
		return -1;
	}

	CIUIRect rcThis;
	GetWindowRectToHWND(hWnd, rcThis);
	CIUIRect rcHost;
	::GetWindowRect(GetHostWnd(hWnd), rcHost);

	// 得到自己的屏幕坐标
	rcThis.OffsetRect(rcHost.TopLeft());

	CIUIRect rcNew = lpRect;
	rcNew.OffsetRect(-rcThis.TopLeft());

	*lpRect = rcNew;

	return 0;
}

int IUI::ScreenToClient(HWLWND hWnd, LPPOINT lpPt)
{
	if (lpPt == NULL)
	{
		return -1;
	}

	CIUIRect rcThis;
	GetWindowRectToHWND(hWnd, rcThis);
	CIUIRect rcHost;
	::GetWindowRect(GetHostWnd(hWnd), rcHost);

	// 得到自己的屏幕坐标
	rcThis.OffsetRect(rcHost.TopLeft());

	CIUIPoint ptNew = *lpPt;
	ptNew -= rcThis.TopLeft();

	*lpPt = ptNew;

	return 0;
}

// TODO: 需要研究API MoveWindow、SetWindowPos逻辑，及向外部发头的消息的顺序。
void IUI::MoveWindow(HWLWND hWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint/* = TRUE*/)
{
	if (GetHostWnd(hWnd) == NULL)
	{
		_ASSERT(FALSE);
		return;
	}

	CIUIRect rc(x, y, x + nWidth, y + nHeight);
	if (rc.right < rc.left)
	{
		rc.right = rc.left;
	}

	if (rc.bottom < rc.top)
	{
		rc.bottom = rc.top;
	}

	// 向祖宗依次发送尺寸改变消息
	WINDOWPOS wp;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOZORDER;
	wp.hwnd = (HWND)hWnd;
	wp.hwndInsertAfter = NULL;
	wp.x = rc.left;
	wp.y = rc.top;

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	// 在WM_WINDOWPOSCHANGING事件中，外部用户可能修改了坐标值
	rc.SetRect(wp.x, wp.y, wp.x + wp.cx, wp.y + wp.cy);

	// WM_MOVING的LPARAM为控件屏幕坐标
	HWLWND hOwner = GetOwner(hWnd);
	HWND hHost = GetHostWnd(hWnd);
	RECT rect = rc;
	if (hOwner == NULL)
	{
		IUIClientToScreen(hHost, &rect);
	}
	else
	{
		ClientToScreen(hOwner, &rect);
	}
	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_MOVING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&rect;
		RaiseEvent(hWnd, &arg);
	}
	if (hOwner == NULL)
	{
		IUIScreenToClient(hHost, &rect);
	}
	else
	{
		ScreenToClient(hOwner, &rect);
	}
	rc = rect;

	//
	CIUIRect rcOld;
	GetWindowRectToHWND(hWnd, rcOld);

	hWnd->m_rcItem = rc;

	// 向祖宗依次发送尺寸改变消息
	{
		wp.cx = hWnd->m_rcItem.Width();
		wp.cy = hWnd->m_rcItem.Height();
		wp.flags = SWP_NOZORDER;
		wp.hwnd = (HWND)hWnd;
		wp.hwndInsertAfter = NULL;
		wp.x = hWnd->m_rcItem.left;
		wp.y = hWnd->m_rcItem.top;

		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_MOVE;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = MAKELPARAM(hWnd->m_rcItem.left, hWnd->m_rcItem.top);
		RaiseEvent(hWnd, &arg);
	}

	// 刷新新旧区域的并集
	if (bRepaint)
	{
		CIUIRect rcNew;
		GetWindowRectToHWND(hWnd, rcNew);

		CIUIRect rcInvalid;
		BOOL bRet = UnionRect(rcInvalid, rcOld, rcNew);
		if (bRet && GetHostWnd(hWnd) != NULL)
		{
			::InvalidateRect(GetHostWnd(hWnd), rcInvalid, FALSE);
		}
	}
}

void IUI::MoveWindow(HWLWND hWnd, LPCRECT lpRect, BOOL bRepaint/* = TRUE*/)
{
	if (lpRect == NULL || GetHostWnd(hWnd) == NULL)
	{
		_ASSERT(FALSE);
		return;
	}

	IUI::MoveWindow(hWnd, lpRect->left, lpRect->top,
		RECTWIDTH(*lpRect), RECTHEIGHT(*lpRect), bRepaint);
}

BOOL IUI::SetWindowPos(HWLWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	CIUIRect rcNew = hWnd->m_rcItem;
	int nOldX = hWnd->m_rcItem.left;
	int nOldY = hWnd->m_rcItem.top;
	int nOldWidth = hWnd->m_rcItem.Width();
	int nOldHeight = hWnd->m_rcItem.Height();
	if (!IsIncludeFlag(uFlags, SWP_NOMOVE))
	{
		rcNew.left = X;
		rcNew.top = Y;
		rcNew.right = rcNew.left + nOldWidth;
		rcNew.bottom = rcNew.top + nOldHeight;
	}

	if (!IsIncludeFlag(uFlags, SWP_NOSIZE))
	{
		rcNew.right = rcNew.left + cx;
		rcNew.bottom = rcNew.top + cy;
	}

	BOOL bRedraw = !IsIncludeFlag(uFlags, SWP_NOREDRAW);

	MoveWindow(hWnd, rcNew, bRedraw);

	return 0;
}

int IUI::GetClientRect(HWLWND hWnd, LPRECT lpRect)
{
	if (lpRect == NULL)
	{
		return -1;
	}

	CIUIRect rcClient = hWnd->m_rcItem;
	rcClient.OffsetRect(-rcClient.TopLeft());

	// 查看是否有水平和（或）垂直滚动条
	BOOL bVert = IsIncludeFlag(GetStyle(hWnd), WS_VSCROLL);
	if (bVert)
	{
		if (IsWindow(hWnd->m_hVerticalScrollBar))
		{
			CIUIRect rcScroll;
			GetWindowRect(hWnd->m_hVerticalScrollBar, rcScroll);

			rcClient.right -= rcScroll.Width();
		}
	}

	BOOL bHorz = IsIncludeFlag(GetStyle(hWnd), WS_HSCROLL);
	if (bHorz)
	{
		if (IsWindow(hWnd->m_hHorizontalScrollBar))
		{
			CIUIRect rcScroll;
			GetWindowRect(hWnd->m_hHorizontalScrollBar, rcScroll);

			rcClient.bottom -= rcScroll.Height();
		}
	}

	*lpRect = rcClient;

	return 0;
}

// 相对于屏幕
int IUI::GetWindowRect(HWLWND hWnd, LPRECT lpRect)
{
	if (lpRect == NULL || !IsWindow(hWnd))
	{
		return -1;
	}

	HWLWND hParent = GetParent(hWnd);
	CIUIRect rcWnd = hWnd->m_rcItem;

	if (hParent == NULL)
	{
		IUIClientToScreen(GetHostWnd(hWnd), rcWnd);
		*lpRect = rcWnd;
	}
	else
	{
		CIUIRect rcParent;
		GetWindowRect(hParent, rcParent);

		rcWnd.OffsetRect(rcParent.TopLeft());
		*lpRect = rcWnd;
	}

	return 0;
}

// 相对于宿主窗口
int IUI::GetWindowRectToHWND(HWLWND hWnd, LPRECT lpRect)
{
	if (lpRect == NULL || hWnd == NULL || GetHostWnd(hWnd) == NULL)
	{
		return -1;
	}

	HWLWND hParent = GetParent(hWnd);

	if (hParent == NULL)
	{
		*lpRect = hWnd->m_rcItem;
	}
	else
	{
		CIUIRect rcWnd = hWnd->m_rcItem;

		CIUIRect rcParent;
		GetWindowRectToHWND(hParent, rcParent);

		rcWnd.OffsetRect(rcParent.TopLeft());
		*lpRect = rcWnd;
	}

	return 0;
}

void IUI::SetFixedWidth(HWLWND hWnd, int cx)
{
	if (cx < 0)
	{
		return;
	}

	hWnd->m_bFixedWidth = true;

	cx = JG_DPIGetDPIWidth(cx, IsDPIEnabled(hWnd));

	hWnd->m_cxyFixed.cx = cx;

	// 向祖宗依次发送尺寸改变消息
	{
		WINDOWPOS wp;
		wp.cx = hWnd->m_rcItem.Width();
		wp.cy = hWnd->m_rcItem.Height();
		wp.flags = SWP_NOMOVE;
		wp.hwnd = (HWND)hWnd;
		wp.hwndInsertAfter = NULL;
		wp.x = hWnd->m_rcItem.left;
		wp.y = hWnd->m_rcItem.top;

		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	Invalidate(hWnd);
}

int IUI::GetFixedWidth(HWLWND hWnd)
{
	return hWnd->m_cxyFixed.cx;
}

void IUI::SetFixedHeight(HWLWND hWnd, int cy)
{
	if (cy < 0)
	{
		return;
	}

	hWnd->m_bFixedHeight = true;

	hWnd->m_cxyFixed.cy = cy;

	// 向祖宗依次发送尺寸改变消息
	{
		WINDOWPOS wp;
		wp.cx = hWnd->m_rcItem.Width();
		wp.cy = hWnd->m_rcItem.Height();
		wp.flags = SWP_NOMOVE;
		wp.hwnd = (HWND)hWnd;
		wp.hwndInsertAfter = NULL;
		wp.x = hWnd->m_rcItem.left;
		wp.y = hWnd->m_rcItem.top;

		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = WM_WINDOWPOSCHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.lParam = (LPARAM)&wp;
		RaiseEvent(hWnd, &arg);
	}

	Invalidate(hWnd);
}

int IUI::GetFixedHeight(HWLWND hWnd)
{
	return hWnd->m_cxyFixed.cy;
}

BOOL IUI::IsFixedWidth(HWLWND hWnd)
{
	return hWnd->m_bFixedWidth;
}

BOOL IUI::IsFixedHeight(HWLWND hWnd)
{
	return hWnd->m_bFixedHeight;
}

void IUI::ClearFixedWidth(HWLWND hWnd)
{
	hWnd->m_bFixedWidth = false;
	hWnd->m_cxyFixed.cx = 0;

	Invalidate(hWnd);
}

void IUI::ClearFixedHeight(HWLWND hWnd)
{
	hWnd->m_bFixedHeight = false;
	hWnd->m_cxyFixed.cy = 0;

	Invalidate(hWnd);
}

void IUI::SetMinWidth(HWLWND hWnd, int cx)
{
	if (hWnd->m_cxyMin.cx == cx)
	{
		return;
	}

	if (cx < 0)
	{
		return;
	}

	hWnd->m_cxyMin.cx = JG_DPIGetDPIWidth(cx, IsDPIEnabled(hWnd));

	Invalidate(hWnd);
}

int IUI::GetMinWidth(HWLWND hWnd)
{
	return hWnd->m_cxyMin.cx;
}

void IUI::SetMaxWidth(HWLWND hWnd, int cx)
{
	if (hWnd->m_cxyMax.cx == cx)
	{
		return;
	}

	if (cx < 0)
	{
		return;
	}

	cx = JG_DPIGetDPIWidth(cx, IsDPIEnabled(hWnd));

	hWnd->m_cxyMax.cx = cx;

	Invalidate(hWnd);
}

int IUI::GetMaxWidth(HWLWND hWnd)
{
	return hWnd->m_cxyMax.cx;
}

void IUI::SetMinHeight(HWLWND hWnd, int cy)
{
	if (hWnd->m_cxyMin.cy == cy)
	{
		return;
	}

	if (cy < 0)
	{
		return;
	}

	hWnd->m_cxyMin.cy = cy;

	Invalidate(hWnd);
}

int IUI::GetMinHeight(HWLWND hWnd)
{
	return hWnd->m_cxyMin.cy;
}

void IUI::SetMaxHeight(HWLWND hWnd, int cy)
{
	if (hWnd->m_cxyMax.cy == cy)
	{
		return;
	}

	if (cy < 0)
	{
		return;
	}

	hWnd->m_cxyMax.cy = JG_DPIGetDPIHeight(cy, IsDPIEnabled(hWnd));

	Invalidate(hWnd);
}

int IUI::GetMaxHeight(HWLWND hWnd)
{
	return hWnd->m_cxyMax.cy;
}

int IUI::SetTitleBarHeight(HWLWND hWnd, int nHeight)
{
	hWnd->m_nTitlebarHeight = nHeight;
	return 0;
}

int IUI::GetTitleBarHeight(HWLWND hWnd)
{
	return hWnd->m_nTitlebarHeight;
}

int IUI::SetBorderThickness(HWLWND hWnd, LPCRECT lprcBorder)
{
	if (lprcBorder == NULL)
	{
		return -1;
	}

	hWnd->m_rcBorderThickness = lprcBorder;

	return 0;
}

int IUI::GetBorderThickness(HWLWND hWnd, LPRECT lprcBorder)
{
	if (lprcBorder == NULL)
	{
		return -1;
	}

	*lprcBorder = hWnd->m_rcBorderThickness;

	return 0;
}

int IUI::SetControlMargin(HWLWND hWnd, LPCRECT lprc)
{
	if (lprc == NULL)
	{
		return -1;
	}

	hWnd->m_rcControlMargin = *lprc;

	return 0;
}

int IUI::GetControlMargin(HWLWND hWnd, LPRECT lprc)
{
	if (lprc == NULL)
	{
		return -1;
	}

	*lprc = hWnd->m_rcControlMargin;

	return 0;
}

Gdiplus::RectF *IUI::GetRectF(HWLWND hWnd)
{
	return hWnd->m_pRectF;
}

UINT IUI::SetResizeFlag(HWLWND hWnd, UINT uFlag)
{
	UINT uOldFlag = hWnd->m_dwCanDragResize;

	hWnd->m_dwCanDragResize = uFlag;

	return uOldFlag;
}

UINT IUI::GetResizeFlag(HWLWND hWnd)
{
	return hWnd->m_dwCanDragResize;
}

// lpScreenPoint: 相对屏幕坐标。 因为MSDN中，WM_NCHITTEST的坐标就是相对屏幕的。
int IUI::HitTest(HWLWND hWnd, LPPOINT lpScreenPoint)
{
	if (hWnd == NULL || lpScreenPoint == NULL)
	{
		return -1;
	}

	// Disabled control refuse hit test.
	if (!IsWindowEnabled(hWnd) || !IsWindowVisible(hWnd))
	{
		return -2;
	}

	// 给祖宗一个改变HitTest结果的机会
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = WM_NCHITTEST;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = MAKELPARAM(lpScreenPoint->x, lpScreenPoint->y);
	arg.lResult = HTCLIENT;
	LRESULT lr = RaiseEvent(hWnd, &arg);
	if (arg.bHandled)
	{
		hWnd->m_uLButtonDownHitTestRet = lr;
		return arg.lResult;
	}

	return arg.lResult;
}

BOOL IUI::SetMouseTransparent(HWLWND hWnd, BOOL bMouseTransparent)
{
	if (NULL == hWnd)
	{
		return FALSE;
	}

	BOOL bOld = hWnd->m_bMouseTransparent;
	hWnd->m_bMouseTransparent = bMouseTransparent;
	return bOld;
}

BOOL IUI::IsMouseTransparent(HWLWND hWnd)
{
	if (NULL == hWnd)
	{
		return FALSE;
	}

	return hWnd->m_bMouseTransparent;
}

CONTROL_LAYOUT_HOR IUI::SetHorzLayoutMode(HWLWND hWnd, CONTROL_LAYOUT_HOR eLayout)
{
	if (NULL == hWnd)
	{
		return CLH_UNKNOWN;
	}

	CONTROL_LAYOUT_HOR eOld = hWnd->m_eHAlign;
	hWnd->m_eHAlign = eLayout;
	return eOld;
}

CONTROL_LAYOUT_HOR IUI::GetHorzLayoutMode(HWLWND hWnd)
{
	return hWnd->m_eHAlign;
}

CONTROL_LAYOUT_VER IUI::SetVertLayoutMode(HWLWND hWnd, CONTROL_LAYOUT_VER eLayout)
{
	if (NULL == hWnd)
	{
		return CLV_UNKNOWN;
	}

	CONTROL_LAYOUT_VER eOld = hWnd->m_eVAlign;
	hWnd->m_eVAlign = eLayout;
	return eOld;
}

CONTROL_LAYOUT_VER IUI::GetVertLayoutMode(HWLWND hWnd)
{
	return hWnd->m_eVAlign;
}

DOCK IUI::SetDockProperty(HWLWND hWnd, DOCK eNewDock)
{
	if (NULL == hWnd)
	{
		return DOCK_UNKNOWN;
	}

	DOCK eOld = hWnd->m_eDockProperty;
	hWnd->m_eDockProperty = eNewDock;
	return eOld;
}

DOCK IUI::GetDockProperty(HWLWND hWnd)
{
	return hWnd->m_eDockProperty;
}

int IUI::SetLayoutMargin(HWLWND hWnd, LPCRECT lprcMargin)
{
	if (lprcMargin == NULL)
	{
		return -1;
	}

	hWnd->m_rcLayoutMargin = *lprcMargin;

	if (hWnd->m_bRedraw)
	{
		Invalidate(hWnd);
	}

	return 0;
}

int IUI::GetLayoutMargin(HWLWND hWnd, LPRECT lprcMargin)
{
	if (NULL == hWnd || NULL == lprcMargin)
	{
		return -1;
	}

	*lprcMargin = hWnd->m_rcLayoutMargin;

	return 0;
}

int IUI::SetLayoutInset(HWLWND hWnd, LPCRECT lprcInset)
{
	if (NULL == hWnd || lprcInset == NULL)
	{
		return -1;
	}

	hWnd->m_rcInset = *lprcInset;

	if (hWnd->m_bRedraw)
	{
		Invalidate(hWnd);
	}

	return 0;
}

int IUI::GetLayoutInset(HWLWND hWnd, LPRECT lprcInset)
{
	if (NULL == hWnd || lprcInset == NULL)
	{
		return -1;
	}

	*lprcInset = hWnd->m_rcInset;

	return 0;
}

void IUI::SetToolTip(HWLWND hWnd, LPCTSTR pstrText)
{
	if (NULL == hWnd)
	{
		return;
	}

	CIUIString strTemp(pstrText);
	strTemp.Replace(_T("<n>"), _T("\r\n"));
	hWnd->m_sToolTip = strTemp;
}

CIUIString IUI::GetToolTip(HWLWND hWnd)
{
	if (NULL == hWnd)
	{
		return _T("");
	}

	return hWnd->m_sToolTip;
}

void IUI::SetResToolTip(HWLWND hWnd, HINSTANCE hRes, UINT uID)
{
	if (NULL == hWnd)
	{
		return;
	}

	CIUIString strText;
	BOOL bRet = strText.LoadString(uID);

	if (!bRet)
	{
		return;
	}

	SetToolTip(hWnd, strText);
}

#if (_MSC_VER > 1200)	// VC6.0
void IUI::SetXResToolTip(HWLWND hWnd, HXSRESOURCE hXRes, LPCTSTR lpszSectionID, LPCTSTR lpszStringID)
{
	LPCWSTR lpszString = CParseLanguageFile::LoadXMLString(hXRes, lpszSectionID, lpszStringID);

	if (lpszString != NULL)
	{
		USES_CONVERSION;
		SetToolTip(hWnd, W2T((LPWSTR)lpszString));
	}
}
#endif // #if (_MSC_VER > 1200)	// VC6.0

void IUI::SetToolTipWidth(HWLWND hWnd, int nWidth)
{
	hWnd->m_nTooltipWidth = nWidth;
}

int IUI::GetToolTipWidth(HWLWND hWnd)
{
	return hWnd->m_nTooltipWidth;
}

void IUI::SetUserData(HWLWND hWnd, LPARAM lParam)
{
	hWnd->m_lUserData = lParam;
}

LPARAM IUI::GetUserData(HWLWND hWnd)
{
	return hWnd->m_lUserData;
}

void IUI::SetTag(HWLWND hWnd, LPCTSTR lpszTag)
{
	hWnd->m_strTag = lpszTag;
}

LPCTSTR IUI::GetTag(HWLWND hWnd)
{
	return hWnd->m_strTag;
}

UINT IUI::SetState(HWLWND hWnd, UINT uState)
{
	UINT uOld = hWnd->m_uState;
	hWnd->m_uState = uState;
	return uOld;
}

UINT IUI::GetState(HWLWND hWnd)
{
	return hWnd->m_uState;
}

UINT IUI::AddState(HWLWND hWnd, UINT uState)
{
	hWnd->m_uState |= uState;

	if (hWnd->m_bRedraw)
	{
		Invalidate(hWnd);
	}

	return hWnd->m_uState;
}

UINT IUI::RemoveState(HWLWND hWnd, UINT uState)
{
	hWnd->m_uState &= ~uState;
	return hWnd->m_uState;
}

BOOL IUI::IsWindowVisible(HWLWND hWnd)
{
	return IsIncludeFlag(GetStyle(hWnd), WS_VISIBLE);
}

BOOL IUI::IsAncestryVisible(HWLWND hWnd)
{
	HWLWND hParent = hWnd;

	while (NULL != hParent)
	{
		if (!IsIncludeFlag(GetStyle(hParent), WS_VISIBLE))
		{
			return FALSE;
		}

		hParent = GetParent(hParent);
	}

	return ::IsWindowVisible(GetHostWnd(hWnd));
}

BOOL CALLBACK WalkShowWindowHWLWnd(HWLWND hWndShowWindow, void *pCallbackParam)
{
	SendMessage(hWndShowWindow, WLM_PARENTSHOWWINDOW, 0, LPARAM(pCallbackParam));

	return FALSE;
}

BOOL IUI::ShowWindow(HWLWND hWnd, int nCmdShow)
{
	if (!IsWindow(hWnd))
	{
		return FALSE;
	}

	// 	if (IsWindowVisible(hWnd))
	// 	{
	// 		if (nCmdShow == SW_SHOW)
	// 		{
	// 			return TRUE;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (nCmdShow == SW_HIDE)
	// 		{
	// 			return TRUE;
	// 		}
	// 	}

	BOOL v = IsWindowVisible(hWnd);
	if (nCmdShow == SW_SHOW)
	{
		AddStyle(hWnd, WS_VISIBLE);

		// 如果原来不可见，变成可见后，要重新布局一下，因为在隐藏期间，父控件布局时会忽略子控件
		if (!v)
		{
			// 如果是内置滚动条，在由隐藏变显示后，不重新布局
			// 滚动条不使用CanvasLayoutCtrl布局
			// TODO: 其实这里也不应该是CanvasLayoutCtrl，而是根据控件自己的布局来决定调用哪种布局。
			BOOL bWithinScrollBar = FALSE;
			BOOL bParentIsCanvas = FALSE;
			HWLWND hParent = GetParent(hWnd);
			if (NULL != hParent)
			{
				if (GetHorizontalScrollBar(hParent) == hWnd
					|| GetVerticalScrollBar(hParent) == hWnd)
				{
					bWithinScrollBar = TRUE;
				}
				else if (GetClassName(hParent) == IUI_CANVAS)
				{
					bParentIsCanvas = TRUE;
				}
			}
			else
			{
				bParentIsCanvas = TRUE;
			}

			if (!bWithinScrollBar)
			{
				if (bParentIsCanvas)
				{
					CanvasLayoutCtrl(hWnd);
				}
			}
		}
	}
	else
	{
		RemoveStyle(hWnd, WS_VISIBLE);
	}

	// 向它的祖宗依次发送消息
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = WM_SHOWWINDOW;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)IsWindowVisible(hWnd);
	RaiseEvent(hWnd, &arg);

	// 向它所有后代所送消息。因为它的后代可能需要感知祖先被显示或隐藏了。
	WalkHWLWNDChild(hWnd, WalkShowWindowHWLWnd, (void *)IsWindowVisible(hWnd));

	if (IsWindowVisible(hWnd) != v)
	{
		Invalidate(hWnd);
	}

	//if (nCmdShow == SW_HIDE)
	//{
	//	IUITRACE(_T("隐藏控件%s[%s]\r\n"), GetClassName(hWnd), GetWindowText(hWnd));
	//}
	//else
	//{
	//	IUITRACE(_T("显示控件%s[%s]\r\n"), GetClassName(hWnd), GetWindowText(hWnd));
	//}

	return TRUE;
}

BOOL IUI::EnableWindow(HWLWND hWnd, BOOL bEnabled)
{
	BOOL bOld = IsWindowEnabled(hWnd);

	if (bOld == bEnabled)
	{
		return !bOld;
	}

	if (bEnabled)
	{
		RemoveStyle(hWnd, WS_DISABLED);
	}
	else
	{
		AddStyle(hWnd, WS_DISABLED);
	}

	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = WM_ENABLE;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.wParam = bOld;
	arg.lParam = bEnabled;
	RaiseEvent(hWnd, &arg);

	if (hWnd->m_bRedraw)
	{
		Invalidate(hWnd);
	}

	return !bOld;
}

BOOL IUI::IsWindowEnabled(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return FALSE;
	}

	return !IsIncludeFlag(GetStyle(hWnd), WS_DISABLED);
}

// IUI::SetFocus的处理程序。 向失去和得到焦点的控件发送WM_KILLFOCUS和WM_SETFOCUS事件
// 并设置hHost中的焦点控件。
void xxxSendFocusMessages(HWND hHost, HWLWND hWnd)
{
	if (hHost == NULL)
	{
		return;
	}

	HWLWND hLoseFocus = (HWLWND)::SendMessage(hHost, WM_GETWLFOCUS, 0, 0);
	HWLWND hOldFocus = (HWLWND)::SendMessage(hHost, WM_SETWLFOCUS, 0, (LPARAM)hWnd);

	// 如果WM_SETWLFOCUS返回-1,表示设置失败
	if (hOldFocus == HWLWND(-1))
	{
		return;
	}

	_ASSERT(hLoseFocus == hOldFocus);

	if (hLoseFocus != NULL)
	{
		// 在本控件得到焦点后，逐级通知父控件
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hLoseFocus;
		arg.RoutedEvent = WM_KILLFOCUS;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = (WPARAM)hWnd;
		RaiseEvent(arg.hOriginalSender, &arg);
	}

	if (hWnd != NULL)
	{
		// 发送WM_SETFOCUS
		// 不过，仅在我们设置的焦点仍然还是焦点控件是发送！
		// 因为我们允许调用者通过捕获WM_NCACTIVATE消息后return FALSE
		// 或者在WM_KILLFOCUS处理程序中调用IUI::SetFocus阻止失去焦点
		HWLWND hNowFocus = (HWLWND)::SendMessage(hHost, WM_GETWLFOCUS, 0, 0);
		if (hWnd == hNowFocus)
		{
			// 在本控件得到焦点后，逐级通知父控件
			RoutedEventArgs arg;
			arg.hSender = arg.hOriginalSender = hWnd;
			arg.RoutedEvent = WM_SETFOCUS;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = (WPARAM)hOldFocus;
			RaiseEvent(arg.hOriginalSender, &arg);
		}
	}
}

// TODO: 真正的Windows行为是：
// 先设置真正获得焦点的窗口A，这时可以通过GetFocus得到A。
// 再向失去焦点的窗口B发送WM_KILLFOCUS消息，所以在B中，通过GetFocus是可以得到A的
// 最后向窗口A发送WM_SETFOCUS
// 因为我们允许设置焦点控件为NULL，但当hWnd为NULL时，无法通过hWnd得到Host窗口
// 这将导致我们无法为Host窗口内的焦点控件设置成NULL，故需要hHost参数。
// 已遵守Windows API SetFocus逻辑
HWLWND IUI::SetFocus(HWND hHost, HWLWND hWnd)
{
	if (hHost == NULL)
	{
		return NULL;
	}

	HWLWND hwndOldFocus = (HWLWND)::SendMessage(hHost, WM_GETWLFOCUS, 0, 0);

	// 允许为NULL
	if (hWnd == NULL)
	{
		xxxSendFocusMessages(hHost, hWnd);
		return hwndOldFocus;
	}

	// 如果其祖先的窗口被最小化或禁用，则不要设置焦点。
	HWLWND hwndTemp = hWnd;
	for (; hwndTemp != NULL; hwndTemp = GetParent(hwndTemp))
	{
		if (!IsWindowEnabled(hwndTemp)/* || IsIconic(hwndTemp)*/)
		{
			return NULL;
		}
	}

	// 先让Host设置焦点
	if (::GetFocus() != GetHostWnd(hWnd))
	{
		::SetFocus(GetHostWnd(hWnd));
	}

	if (hWnd != hwndOldFocus)
	{
		xxxSendFocusMessages(GetHostWnd(hWnd), hWnd);
	}

	return hwndOldFocus;
}

BOOL IUI::IsFocused(HWLWND hWnd)
{
	HWLWND hFocus = Window::GetWLFocus(GetHostWnd(hWnd));

	return (hFocus == hWnd);
}

int IUI::SetControlState(HWLWND hWnd, CONTROL_STATE eNewState)
{
	if (!IsWindow(hWnd))
	{
		return -1;
	}

	CONTROL_STATE eOldControlState = hWnd->ci.m_eControlState;

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = UE_CONTROL_STATE_CHANGING;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = eOldControlState;// Old
		arg.lParam = eNewState; // New
		LRESULT lr = RaiseEvent(hWnd, &arg);
		if (arg.lResult != 0)
		{
			return 1;
		}
	}

	hWnd->ci.m_eControlState = eNewState;

	{
		RoutedEventArgs arg;
		arg.hSender = arg.hOriginalSender = hWnd;
		arg.RoutedEvent = UE_CONTROL_STATE_CHANGED;
		arg.eRoutingStrategy = RS_BUBBLE;
		arg.wParam = eOldControlState;// Old
		arg.lParam = hWnd->ci.m_eControlState; // New
		LRESULT lr = RaiseEvent(hWnd, &arg);
	}

	return 0;
}

CONTROL_STATE IUI::GetControlState(HWLWND hWnd)
{
	if (hWnd == NULL)
	{
		return CONTROL_STATE_UNKNOWN;
	}

	return hWnd->ci.m_eControlState;
}

int IUI::BindHorzScrollBarStyle(HWLWND hTreeView, const CScrollBarProp *pBindStyle)
{
	_ASSERT(IsWindow(hTreeView));
	return SendMessage(hTreeView, WLM_SET_BIND_SCROLLBAR_STYLE, TRUE, (LPARAM)pBindStyle);
}

int IUI::BindVertScrollBarStyle(HWLWND hTreeView, const CScrollBarProp *pBindStyle)
{
	_ASSERT(IsWindow(hTreeView));
	return SendMessage(hTreeView, WLM_SET_BIND_SCROLLBAR_STYLE, FALSE, (LPARAM)pBindStyle);
}

HWLWND IUI::FindControl(HWLWND hWndParent, LPCTSTR lpszName)
{
	if (NULL == hWndParent || NULL == lpszName)
	{
		return NULL;
	}

	HWLWND hFound = NULL;

	HWLWND hChild = hWndParent->spwndChild;
	for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDNEXT))
	{
		// 选判断孩子自己是不是要找的
		if (hChild->m_strName == lpszName)
		{
			hFound = hChild;
			break;
		}

		// 再递归判断孩子的孩子
		hFound = FindControl(hChild, lpszName);
		if (hFound != NULL)
		{
			break;
		}
	}

	return hFound;
}

HWLWND IUI::HitTestChild(HWLWND hWnd, LPPOINT lpPt)
{
	// 传进来的坐标，是相对于hWnd父亲的
	if (lpPt == NULL)
	{
		return NULL;
	}

	BOOL bVirtualRoot = (hWnd->m_uID == UINT(-1));
	HWLWND hRet = NULL;

	if (!bVirtualRoot)
	{
		// 忽略不可见和disabled的
		if (!IsWindowVisible(hWnd))
		{
			return NULL;
		}

		if (!IsWindowEnabled(hWnd))
		{
			return NULL;
		}

		if (IsEditMode(hWnd))
		{
			// TODO: HWLWND支持逆序遍历了。
			// 编辑模式时，在父窗口外面的子控件，也应该被命中，因为可能要移动或缩放子控件。
			std::vector<HWLWND> vChildren;
			GetAllChildren(hWnd, TRUE, &vChildren);

			// 逆序遍历
			std::vector<HWLWND>::reverse_iterator it = vChildren.rbegin();
			for (; it != vChildren.rend(); ++it)
			{
				HWLWND hChildWnd = *it;

				if (!IsWindowVisible(hChildWnd))
				{
					continue;
				}

				if (!IsWindowEnabled(hChildWnd))
				{
					continue;
				}

				CIUIRect rcChild;
				GetWindowRectToHWND(hChildWnd, rcChild);

				if (rcChild.PtInRect(*lpPt))
				{
					hRet = hChildWnd;
					break;
				}
			}
		}
		else
		{
			//
			// 先看看是否命中自己，如果没命中，肯定也不会命中自己的孩子，编辑框除外
			//
			CIUIPoint ptNew = *lpPt;
			if (!hWnd->m_rcItem.PtInRect(*lpPt))
			{
				return NULL;
			}

			//
			// 把坐标转成相对于hWnd自己的，因为孩子的坐标，就是相对自己的
			//
			ptNew.Offset(-hWnd->m_rcItem.TopLeft());

			// 考虑SetWindowRgn设置的裁剪区域
			if (hWnd->m_hrgnClip != NULL)
			{
				if (!PtInRegion(hWnd->m_hrgnClip, ptNew.x, ptNew.y))
				{
					return NULL;
				}
			}

			// 至少命中了自己(忽略“透明”控件)
			if (!IsMouseTransparent(hWnd))
			{
				hRet = hWnd;
			}

			//
			// 命中孩子测试。 倒序测试，因为后创建的在上面。
			// TreeView之类控件的真正的子控件的优先级高于绑定到Item上的控件
			//
			BOOL bHittestChild = FALSE;
			HWLWND hChild = GetWindow(hWnd, GW_CHILD);
			hChild = GetWindow(hChild, GW_HWNDLAST);

			for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDPREV))
			{
				// 递归命中测试
				HWLWND hChildRet = HitTestChild(hChild, &ptNew);
				if (hChildRet != NULL)
				{
					if (!IsMouseTransparent(hChildRet))
					{
						bHittestChild = TRUE;
						hRet = hChildRet;
						break;
					}
				}
			}

			//
			// 如果hWnd是TreeView、ListView、ComboBox下拉列表、菜单项等
			// 命中绑定到Item上的控件。通过发送消息，让控件自己处理。优先级低于控件真正的子控件
			// 所以，只有在子控件没有命中时，才做Bind到Item上的控件的命中测试。
			//
			if (!bHittestChild)
			{
				HWLWND hBindItem = (HWLWND)IUI::SendMessage(hWnd, WLM_HITTESTBINDITEM,
						0, MAKELPARAM(ptNew.x, ptNew.y));
				if (NULL != hBindItem)
				{
					hRet = hBindItem;
				}
			}
		}
	}

	// 如果是虚拟根控件，则依次命中它的孩子
	if (bVirtualRoot)
	{
		// 逆序遍历
		HWLWND hRoot = GetWindow(hWnd, GW_CHILD);
		hRoot = GetWindow(hRoot, GW_HWNDLAST);

		for (; hRoot != NULL; hRoot = GetWindow(hRoot, GW_HWNDPREV))
		{
			hRet = HitTestChild(hRoot, lpPt);
			if (hRet != NULL)
			{
				break;
			}
		}
	}

	return hRet;
}

// Point：相对于hHost的坐标
HWLWND IUI::WindowFromPoint(HWND hHost, POINT Point)
{
	HWLWND hFound = NULL;

	// 倒序命中
	HWLWND hChild = (HWLWND)::SendMessage(hHost, WM_GETCHILD, 0, 0);
	hChild = GetWindow(hChild, GW_HWNDLAST);
	for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDPREV))
	{
		hFound = HitTestChild(hChild, &Point);
		if (hFound != NULL)
		{
			break;
		}
	}

	return hFound;
}

BOOL IUI::Invalidate(HWLWND hWnd)
{
	if (!IsWindowVisible(hWnd))
	{
		// 如果控件被隐藏，则刷新父控件
		CIUIRect rcInParent;
		IUI::GetWindowRectToParent(hWnd, rcInParent);

		HWLWND hParent = GetParent(hWnd);
		if (NULL != hParent)
		{
			IUI::InvalidateRect(hParent, rcInParent);
		}
		else
		{
			::InvalidateRect(GetHostWnd(hWnd), rcInParent, FALSE);
		}

		return TRUE;
	}

	if (!hWnd->m_bRedraw)
	{
		return FALSE;
	}

	BOOL bUpdateLayeredWindow = IsUpdateLayeredWindow(GetHostWnd(hWnd));

	if (bUpdateLayeredWindow)
	{
		Window *pWnd = Window::FromHandlePermanent(GetHostWnd(hWnd));
		_ASSERT(pWnd != NULL);
		pWnd->Invalidate();
	}
	else
	{
		CIUIRect rcInvalidate;
		GetClientRect(hWnd, rcInvalidate);
		return InvalidateRect(hWnd, rcInvalidate);
	}

	return TRUE;
}

BOOL IUI::InvalidateRect(HWLWND hWnd, LPCRECT lpRect)
{
	// 自己或祖先中只要有一个不可见，就返回。
	if (!IsAncestryVisible(hWnd))
	{
		return FALSE;
	}

// 	static int m = 0;
// 	IUITRACE(_T("%d InvalidateRect 0x%08x %s %s\r\n"), m++, hWnd, (LPCTSTR)GetClassName(hWnd), (LPCTSTR)GetName(hWnd));

	if (GetHostWnd(hWnd) != NULL)
	{
		if (IsUpdateLayeredWindow(GetHostWnd(hWnd)))
		{
			Window *pWindow = Window::FromHandlePermanent(GetHostWnd(hWnd));
			_ASSERT(pWindow != NULL);
			pWindow->Invalidate(FALSE);
		}
		else
		{
			CIUIRect rc;
			if (lpRect == NULL)
			{
				// 大部分情况下，滚动条区域也需要刷新，所以使用控件整个区域坐标。
				GetWindowRectToHWND(hWnd, rc);
			}
			else
			{
				rc = lpRect;

				// 转成相对于Host的坐标
				ClientToScreen(hWnd, rc);
				IUIScreenToClient(GetHostWnd(hWnd), rc);
			}

			::InvalidateRect(GetHostWnd(hWnd), rc, FALSE);
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	return TRUE;
}

BOOL IUI::SetUseRectF(HWLWND hWnd, BOOL bUse)
{
	BOOL bOld = hWnd->m_bUseRectF;
	hWnd->m_bUseRectF = bUse;
	return bOld;
}

BOOL IUI::IsUseRectF(HWLWND hWnd)
{
	return hWnd->m_bUseRectF;
}

BOOL IUI::IsCachedMode(HWLWND hWnd)
{
	return hWnd->m_bCacheMode;
}

// TODO: 不应该提供这个接口
HBITMAP IUI::GetCachedAlphaBitmap(HWLWND hWnd)
{
	return hWnd->m_hCacheAlphaBmp;
}

int IUI::SetCachedAlphaBitmap(HWLWND hWnd, HBITMAP hbmp)
{
	hWnd->m_hCacheAlphaBmp = hbmp;

	return 0;
}

// uIDEvent: ::SetTimer指定的Timer ID.
HTIMER FromTimerEventPermanent(UINT_PTR uIDEvent)
{
	std::map<UINT_PTR, HTIMER> *pMap = iuiMapTimer(TRUE);
	HTIMER hTimer = NULL;
	if (pMap != NULL)
	{
		// only look in the permanent map - does no allocations
		std::map<UINT_PTR, HTIMER>::iterator it = pMap->find(uIDEvent);
		if (it != pMap->end())
		{
			hTimer = it->second;
		}
		_ASSERT(hTimer == NULL || ((WLTIMERINFO *)hTimer)->uTimerID == uIDEvent);
	}

	return hTimer;
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	// IUITRACE(_T("TimerProc hWnd: 0x%08x, id: %d\r\n"), hwnd, idEvent);

	if (hwnd == NULL && uMsg == WM_TIMER)
	{
		HTIMER hTimer = FromTimerEventPermanent(idEvent);
		_ASSERT(hTimer != NULL);

		WLTIMERINFO *pTimer = (WLTIMERINFO *)hTimer;

		if (pTimer->lpTimerFunc != NULL)
		{
			// 如果Timer回调不为NULL，则执行回调
			pTimer->lpTimerFunc(pTimer->hOwner, uMsg, pTimer->uIDEvent, dwTime);
		}
		else
		{
			// 如果Timer回调为NULL，则发送WM_TIMER消息
			SendMessage(pTimer->hOwner, WM_TIMER, pTimer->uIDEvent, NULL);
		}
	}
}

UINT_PTR IUI::SetTimer(HWLWND hWnd, UINT_PTR nIDEvent, UINT uElapse, WLTIMERPROC lpTimerFunc)
{
	// 先查找ID为nIDEvent的Timer是否存在
	BOOL bAlreadExist = FALSE;

	std::map<UINT_PTR, HTIMER>::iterator it;
	if (!hWnd->m_mapTimer.empty())
	{
		it = hWnd->m_mapTimer.find(nIDEvent);
		if (it != hWnd->m_mapTimer.end())
		{
			bAlreadExist = TRUE;
		}
	}

	UINT_PTR uRet = 0;
	// 如果已存在，替换旧的
	if (bAlreadExist)
	{
		WLTIMERINFO *pTimerInfo = (WLTIMERINFO *)(it->second);
		_ASSERT(pTimerInfo != NULL);

		pTimerInfo->uElapse = uElapse;
		pTimerInfo->lpTimerFunc = lpTimerFunc;

		// 替换Timer
		uRet = ::SetTimer(NULL, pTimerInfo->uTimerID, uElapse, TimerProc);
		if (uRet != 0)
		{
			uRet = nIDEvent;
		}
		else
		{
			_ASSERT(FALSE);
		}
	}
	// 如果不存在，创建新的。
	else
	{
		UINT uNewID = ::SetTimer(NULL, 0, uElapse, TimerProc);
		if (uNewID != 0)
		{
			WLTIMERINFO *pTimerInfo = new WLTIMERINFO;
			pTimerInfo->hOwner = hWnd;
			pTimerInfo->uTimerID = uNewID;
			pTimerInfo->uIDEvent = nIDEvent;
			pTimerInfo->uElapse = uElapse;
			pTimerInfo->lpTimerFunc = lpTimerFunc;
			pTimerInfo->dwStartTime = GetTickCount();

			uRet = nIDEvent;

			// 一共有两个Timer map，第一个是HWLWND::m_mapTimer，记录HWLWND的
			// Timer ID与HTIMER的关系；第二个是IUI_MODULE_THREAD_STATE::m_pmapTimer
			// 记录::SetTimer真正的Timer ID与HHTIMER对应关系。
			hWnd->m_mapTimer[pTimerInfo->uIDEvent] = pTimerInfo;

			// 保存到全局map中。
			std::map<UINT_PTR, HTIMER> *pMap = iuiMapTimer(TRUE);
			_ASSERT(pMap != NULL);
			(*pMap)[uNewID] = pTimerInfo;
		}
		else
		{
			_ASSERT(FALSE);
		}
	}

	return uRet;
}

BOOL IUI::KillTimer(HWLWND hWnd, UINT_PTR nIDEvent)
{
	// 先查找ID为nIDEvent的Timer是否存在
	BOOL bAlreadExist = FALSE;

	std::map<UINT_PTR, HTIMER>::iterator it;
	if (!hWnd->m_mapTimer.empty())
	{
		it = hWnd->m_mapTimer.find(nIDEvent);
		if (it != hWnd->m_mapTimer.end())
		{
			bAlreadExist = TRUE;
		}
	}

	if (!bAlreadExist)
	{
		return FALSE;
	}

	WLTIMERINFO *pTimerInfo = (WLTIMERINFO *)(it->second);
	_ASSERT(pTimerInfo != NULL);

	UINT_PTR uTimerID = pTimerInfo->uTimerID;
	BOOL bRet = ::KillTimer(NULL, uTimerID);
	if (!bRet)
	{
		return bRet;
	}

	// 从两个map中移除
	hWnd->m_mapTimer.erase(it);

	std::map<UINT_PTR, HTIMER> *pMap = iuiMapTimer(FALSE);
	_ASSERT(pMap != NULL);
	pMap->erase(pTimerInfo->uTimerID);

	delete pTimerInfo;

	return bRet;
}

BOOL IUI::IsTimerExist(HWLWND hWnd, UINT_PTR nIDEvent)
{
	BOOL bAlreadExist = FALSE;

	if (!hWnd->m_mapTimer.empty())
	{
		std::map<UINT_PTR, HTIMER>::iterator it = hWnd->m_mapTimer.find(nIDEvent);
		if (it != hWnd->m_mapTimer.end())
		{
			bAlreadExist = TRUE;
		}
	}

	return bAlreadExist;
}

HRESULT IUI::RegisterDragDrop(HWLWND hwnd, IDropTarget *pDropTarget)
{
	if (!IsWindow(hwnd))
	{
		return DRAGDROP_E_INVALIDHWND;
	}

	if (NULL != hwnd->m_pDropTarget)
	{
		return DRAGDROP_E_ALREADYREGISTERED;
	}

	// 向Host窗口注册。Host窗口和hwnd控件用的是不同的IDropTarget
	// 向Host窗口注册的pDropTargetHost，在窗口WM_NCDESTROY消息执行函数OnNcDestroy
	// 中调用RevokeDragDrop时，内部会调用pDropTargetHost->Release来释放。
	CHWNDDropTargetEx *pDropTargetHost = new CHWNDDropTargetEx(GetHostWnd(hwnd));
	HRESULT hr = ::RegisterDragDrop(GetHostWnd(hwnd), pDropTargetHost);
	if (DRAGDROP_E_ALREADYREGISTERED == hr)
	{
		// 失败时，RegisterDragDrop不会调用pDropTargetHost->AddRef，所以由delete来释放
		delete pDropTargetHost;
	}
	else if (DRAGDROP_E_INVALIDHWND == hr)
	{
		delete pDropTargetHost;
		return DRAGDROP_E_INVALIDHWND;
	}
	else if (E_OUTOFMEMORY == hr)
	{
		delete pDropTargetHost;
		OutputDebugString(_T("请在调用RegisterDragDrop前调用OleInitialize.\r\n"));
		return E_OUTOFMEMORY;
	}

	ULONG uRef = pDropTarget->AddRef();
	hwnd->m_pDropTarget = pDropTarget;

	return S_OK;
}

HRESULT IUI::RevokeDragDrop(HWLWND hwnd)
{
	if (!IsWindow(hwnd))
	{
		return DRAGDROP_E_INVALIDHWND;
	}

	if (NULL == hwnd->m_pDropTarget)
	{
		return DRAGDROP_E_NOTREGISTERED;
	}

	hwnd->m_pDropTarget->Release();
	hwnd->m_pDropTarget = NULL;

	return S_OK;
}

SIZE IUI::EstimateSize(HWLWND hWnd)
{
	return hWnd->m_cxyFixed;
}

// TODO: Timer应该启在动画中，而不是控件中。
int IUI::BeginAnimation(HWLWND hWnd, ANIMATION_TARGET_PROPERTY eTarget, Animation *pAnimation)
{
	// 定时器应该放到动画中，而不是控件中
	if (pAnimation == NULL)
	{
		return -1;
	}

	hWnd->m_mapAnimation[eTarget] = pAnimation;

	if (eTarget == ATP_TRANSLATE_TRANSFORM_X)
	{
		SetTimer(hWnd, IDT_TRANSLATE_TRANSFORM_X, 10, NULL);
	}
	else if (eTarget == ATP_TRANSLATE_TRANSFORM_Y)
	{
		SetTimer(hWnd, IDT_TRANSLATE_TRANSFORM_Y, 10, NULL);
	}
	else if (eTarget == ATP_TRANSLATE_TRANSFORM_POS)
	{
		int nDuration = pAnimation->GetDuration();

		SetTimer(hWnd, IDT_TRANSLATE_TRANSFORM_POS, 10, NULL);
	}

	return 0;
}

int IUI::BeforePaint(HWLWND hWnd, HDC hDC)
{
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_BEFOREPAINT;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)hDC;
	RaiseEvent(hWnd, &arg);
	return 0;
}

int IUI::AfterPaint(HWLWND hWnd, HDC hDC)
{
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_AFTERPAINT;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)hDC;
	RaiseEvent(hWnd, &arg);
	return 0;
}

int IUI::AfterPaintBkColor(HWLWND hWnd, HDC hDC)
{
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_AFTERPAINTBKCOLOR;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)hDC;
	RaiseEvent(hWnd, &arg);
	return 0;
}

int IUI::AfterPaintBkImage(HWLWND hWnd, HDC hDC)
{
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_AFTERPAINTBKIMAGE;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)hDC;
	RaiseEvent(hWnd, &arg);
	return 0;
}

int IUI::AfterChildrenPaint(HWLWND hWnd, HDC hDC)
{
	RoutedEventArgs arg;
	arg.hSender = arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_AFTERCHILDRENPAINT;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)hDC;
	RaiseEvent(hWnd, &arg);
	return 0;
}

int IUI::DrawBkColor(HWLWND hWnd, HDC hDC, LPCRECT lpRect)
{
	if (!IsDrawBkColor(hWnd))
	{
		return 1;
	}

	// Prepare state index
	COLORREF crBk = hWnd->ci.m_crBkN;
	if (!IsWindowEnabled(hWnd))
	{
		crBk = hWnd->ci.m_crBkD;
	}

	CIUIRect rcClient;
	GetClientRect(hWnd, rcClient);

	FillSolidRect(hDC, rcClient, crBk, IsUpdateLayeredWindow(GetHostWnd(hWnd)));

	return 0;
}

// hDC: 为宿主窗口的内存兼容DC，但绘制原点，已设置为相对自己为(0, 0)
// rcClip: BeginPaint中返回的裁剪矩形，不过已修正为相对自己的坐标
int DefPaint(HWLWND hWnd, HDC hDC, const RECT &rcClip)
{
	if (!hWnd->m_bRedraw)
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(hWnd, rcClient);

	if (hWnd->m_bClip)
	{
		if (!::IntersectRect(&hWnd->m_rcPaint, &rcClip, rcClient))
		{
			return 2;
		}
	}
	else
	{
		hWnd->m_rcPaint = rcClient;
	}

	BeforePaint(hWnd, hDC);
	DrawBkColor(hWnd, hDC, hWnd->m_rcPaint);
	AfterPaint(hWnd, hDC);

	return 0;
}

int WL_DrawBorder(HWLWND hWnd, HDC hDC, IUI_CONTROL_INFO *pci)
{
	if (pci == NULL)
	{
		return -1;
	}

	// Prepare state index
	COLORREF crBorder = hWnd->ci.m_crBorderN;
	if (!IsWindowEnabled(hWnd))
	{
		crBorder = hWnd->ci.m_crBorderD;
	}

	CIUIRect rcWnd;
	GetWindowRect(hWnd, rcWnd);
	rcWnd.OffsetRect(-rcWnd.TopLeft());

	HBRUSH hbrBorder = CreateSolidBrush(crBorder);
	FrameRect(hDC, rcWnd, hbrBorder);
	DeleteObject(hbrBorder);

	return 0;
}

int IUI::DrawBorder(HWLWND hWnd, HDC hDC)
{
	if (!IsDrawBorderColor(hWnd))
	{
		return 1;
	}

	return WL_DrawBorder(hWnd, hDC, &hWnd->ci);
}

void IUI::SetRedraw(HWLWND hWnd, BOOL bRedraw/* = true*/)
{
	SendMessage(hWnd, WM_SETREDRAW, bRedraw, 0);
}

BOOL IUI::IsRedraw(HWLWND hWnd)
{
	return hWnd->m_bRedraw;
}

int IUI::SetClip(HWLWND hWnd, BOOL bClip)
{
	return SendMessage(hWnd, WLM_SET_CLIP, bClip, 0);
}

BOOL IUI::IsClip(HWLWND hWnd)
{
	return SendMessage(hWnd, WLM_GET_CLIP, 0, 0);
}

// xxxSendMessage内部调用xxxSendMessageTimeout
// 直接调用hWnd的窗口过程
LRESULT IUI::SendMessage(HWLWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == hWnd)
	{
		return 0;
	}

	RoutedEventArgs arg;
	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.RoutedEvent = uMsg;
	arg.eRoutingStrategy = RS_DIRECT;
	arg.wParam = wParam;
	arg.lParam = lParam;
	LRESULT lr = RaiseEvent(hWnd, &arg);

	return lr;
}

// 处理非客户区鼠标消息
LRESULT OnNcMouseMessage(HWLWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCLBUTTONDOWN)
	{
		CIUIPoint pt(lParam);
		hWnd->m_ptLButtonDown = pt;
		hWnd->m_rcLButtonDownRelateParent = hWnd->m_rcItem;

		if (hWnd->m_uLButtonDownHitTestRet != HTCLIENT)
		{
			SetCapture(hWnd);
		}
		else
		{
			//TODO: 应该只有允许有键盘焦点（可Tab）控件，才能设置焦点。
			// RichTextBox的焦点设置时机，由RichEdit内部决定，
			// 通过ITextHost.TxSetFocus告诉控件。
			if (GetClassName(hWnd) != IUI_RICHTEXTBOX
				&& GetClassName(hWnd) != IUI_IM)
			{
				SetFocus(GetHostWnd(hWnd), hWnd);
			}
		}
	}
	else if (uMsg == WM_NCLBUTTONUP)
	{
		if (IUI::GetCapture(NULL) == hWnd)
		{
			RoutedEventArgs arg;
			arg.hSender = hWnd;
			arg.hOriginalSender = hWnd;
			arg.RoutedEvent = WM_EXITSIZEMOVE;
			arg.eRoutingStrategy = RS_BUBBLE;
			RaiseEvent(arg.hOriginalSender, &arg);
		}

		hWnd->m_ptLButtonDown = CIUIPoint(-1, -1);
		hWnd->m_rcLButtonDownRelateParent.SetRectEmpty();
		ReleaseCapture(hWnd);
		hWnd->m_uLButtonDownHitTestRet = HTCLIENT;
	}
	else if (uMsg == WM_NCMOUSEMOVE)
	{
		CIUIPoint pt(lParam);

		// 当鼠标按下后，要么是移动控件，要么是缩放控件，这时候，不再进行HitTest
		BOOL bSelected = GetKeyState(VK_LBUTTON) < 0;
		if (!bSelected)
		{
			CIUIPoint ptScreen = pt;
			ClientToScreen(hWnd, &ptScreen);
			hWnd->m_uLButtonDownHitTestRet = HitTest(hWnd, &ptScreen);
		}

		if (IUI::GetCapture(NULL) == hWnd)
		{
			if (hWnd->m_uLButtonDownHitTestRet == HTCAPTION)	// Drag move the windowless window
			{
				if (bSelected)
				{
					CIUIRect rcOld;
					GetWindowRectToParent(hWnd, rcOld);

					if (pt.x != hWnd->m_ptLButtonDown.x
						&& pt.y != hWnd->m_ptLButtonDown.y)
					{
						rcOld.OffsetRect(pt.x - hWnd->m_ptLButtonDown.x,
							pt.y - hWnd->m_ptLButtonDown.y);
						MoveWindow(hWnd, rcOld, TRUE);
					}
				}
			}
			else	// Drag resize the windowless window.
			{
				if (bSelected)
				{
					CIUIRect rcOld;
					GetWindowRectToParent(hWnd, rcOld);

					if ((hWnd->m_uLButtonDownHitTestRet == HTLEFT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_LEFT))
					{
						rcOld.left += (pt.x - hWnd->m_ptLButtonDown.x);

						if (rcOld.left > rcOld.right)
						{
							rcOld.left = rcOld.right;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTTOPLEFT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOPLEFT))
					{
						rcOld.left += (pt.x - hWnd->m_ptLButtonDown.x);
						rcOld.top += (pt.y - hWnd->m_ptLButtonDown.y);

						if (rcOld.left > rcOld.right)
						{
							rcOld.left = rcOld.right;
						}
						if (rcOld.top > rcOld.bottom)
						{
							rcOld.top = rcOld.bottom;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTTOP)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOP))
					{
						rcOld.top += (pt.y - hWnd->m_ptLButtonDown.y);

						if (rcOld.top > rcOld.bottom)
						{
							rcOld.top = rcOld.bottom;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTTOPRIGHT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOPRIGHT))
					{
						rcOld.top += (pt.y - hWnd->m_ptLButtonDown.y);
						rcOld.right = hWnd->m_rcLButtonDownRelateParent.right + (pt.x - hWnd->m_ptLButtonDown.x);

						if (rcOld.top > rcOld.bottom)
						{
							rcOld.top = rcOld.bottom;
						}
						if (rcOld.right < rcOld.left)
						{
							rcOld.right = rcOld.left;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTRIGHT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_RIGHT))
					{
						rcOld.right = hWnd->m_rcLButtonDownRelateParent.right + (pt.x - hWnd->m_ptLButtonDown.x);

						if (rcOld.right < rcOld.left)
						{
							rcOld.right = rcOld.left;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTBOTTOMRIGHT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOMRIGHT))
					{
						rcOld.right = hWnd->m_rcLButtonDownRelateParent.right + (pt.x - hWnd->m_ptLButtonDown.x);
						rcOld.bottom = hWnd->m_rcLButtonDownRelateParent.bottom + (pt.y - hWnd->m_ptLButtonDown.y);

						if (rcOld.right < rcOld.left)
						{
							rcOld.right = rcOld.left;
						}
						if (rcOld.bottom < rcOld.top)
						{
							rcOld.bottom = rcOld.top;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTBOTTOM)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOM))
					{
						rcOld.bottom = hWnd->m_rcLButtonDownRelateParent.bottom + (pt.y - hWnd->m_ptLButtonDown.y);

						if (rcOld.bottom < rcOld.top)
						{
							rcOld.bottom = rcOld.top;
						}
					}
					else if ((hWnd->m_uLButtonDownHitTestRet == HTBOTTOMLEFT)
						&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOMLEFT))
					{
						rcOld.left += (pt.x - hWnd->m_ptLButtonDown.x);
						rcOld.bottom = hWnd->m_rcLButtonDownRelateParent.bottom + (pt.y - hWnd->m_ptLButtonDown.y);

						if (rcOld.left > rcOld.right)
						{
							rcOld.left = rcOld.right;
						}
						if (rcOld.bottom < rcOld.top)
						{
							rcOld.bottom = rcOld.top;
						}
					}

					MoveWindow(hWnd, rcOld, TRUE);
				}
			}
		}
	}

	return 0;
}

int IUI::AddEventHandler(HWLWND hWnd, CWLWndHandler *pHandler)
{
	if (hWnd == NULL)
	{
		return -1;
	}

	hWnd->m_pUserBindWLWndHandler = pHandler;

	return 0;
}

LRESULT IUI::DefEventHandler(HWLWND hWnd, RoutedEventArgs *pe)
{
	if (!::IsWindow(GetHostWnd(hWnd)))
	{
		return -1;
	}

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			switch (uMsg)
			{
			case WM_DESTROY:
			{
				// Kill All Timers
				std::map<UINT_PTR, HTIMER>::iterator it = hWnd->m_mapTimer.begin();
				for (; it != hWnd->m_mapTimer.end(); ++it)
				{
					WLTIMERINFO *pTimer = (WLTIMERINFO *)it->second;

					::KillTimer(NULL, pTimer->uTimerID);
				}
				hWnd->m_mapTimer.clear();
			}
			break;

			case WM_NCHITTEST:
			{
				BOOL bSetCursor = wParam;
				CIUIPoint pt(lParam); // LPARAM：相对屏幕坐标
				ScreenToClient(hWnd, &pt);

				CIUIRect rcClient;
				GetWindowRectToParent(hWnd, &rcClient);
				rcClient.OffsetRect(-rcClient.TopLeft());

				BOOL bSizingBorder = (GetStyle(hWnd) & WS_THICKFRAME);
				CIUIRect rc;

				// left top
				rc.left = rc.top = 0;
				rc.right = hWnd->m_rcBorderThickness.left;
				rc.bottom = hWnd->m_rcBorderThickness.top;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOPLEFT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
					}
					pe->lResult = HTTOPLEFT;
					return 0;
				}

				// top
				rc.left = hWnd->m_rcBorderThickness.left;
				rc.right = rcClient.right - hWnd->m_rcBorderThickness.right;
				rc.bottom = hWnd->m_rcBorderThickness.top;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOP))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENS));
					}
					pe->lResult = HTTOP;
					return 0;
				}

				// right top
				rc.left = rcClient.right - hWnd->m_rcBorderThickness.right;
				rc.right = rcClient.right;
				rc.bottom = hWnd->m_rcBorderThickness.top;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_TOPRIGHT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENESW));
					}
					pe->lResult = HTTOPRIGHT;
					return 0;
				}

				// right
				rc.top = hWnd->m_rcBorderThickness.top;
				rc.bottom = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				rc.left = rcClient.right - hWnd->m_rcBorderThickness.right;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_RIGHT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					}
					pe->lResult = HTRIGHT;
					return 0;
				}

				// right bottom
				rc.top = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				rc.bottom = rcClient.bottom;
				rc.left = rcClient.right - hWnd->m_rcBorderThickness.right;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOMRIGHT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
					}
					pe->lResult = HTBOTTOMRIGHT;
					return 0;
				}

				// bottom
				rc.left = hWnd->m_rcBorderThickness.left;
				rc.right = rcClient.right - hWnd->m_rcBorderThickness.right;
				rc.top = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOM))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENS));
					}
					pe->lResult = HTBOTTOM;
					return 0;
				}

				// left bottom
				rc.left = 0;
				rc.right = hWnd->m_rcBorderThickness.left;
				rc.top = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_BOTTOMLEFT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZENESW));
					}
					pe->lResult = HTBOTTOMLEFT;
					return 0;
				}

				// left
				rc.top = hWnd->m_rcBorderThickness.top;
				rc.bottom = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				rc.right = hWnd->m_rcBorderThickness.left;
				if (bSizingBorder && PtInRect(&rc, pt)
					&& IsIncludeFlag(hWnd->m_dwCanDragResize, GRF_LEFT))
				{
					if (bSetCursor)
					{
						SetCursor(LoadCursor(NULL, IDC_SIZEWE));
					}
					pe->lResult = HTLEFT;
					return 0;
				}

				// Title bar
				rc.top = hWnd->m_rcBorderThickness.top;
				rc.left = hWnd->m_rcBorderThickness.left;
				rc.right = rcClient.right - hWnd->m_rcBorderThickness.right;
				if (hWnd->m_nTitlebarHeight == -1)	// All client as title bar
				{
					rc.bottom = rcClient.bottom - hWnd->m_rcBorderThickness.bottom;
				}
				else
				{
					rc.bottom = hWnd->m_nTitlebarHeight;
				}

				if (PtInRect(&rc, pt))
				{
					pe->lResult = HTCAPTION;
					return 0;
				}
			}
			break;

			case UE_ISHIDEOLEBORDER:
				pe->lResult = hWnd->m_bHideOleBorder;
				return 0;

			case WM_WINDOWPOSCHANGED:
			{
				HWLWND hChild = GetWindow(hWnd, GW_CHILD);
				for (; hChild != NULL; hChild = GetWindow(hChild, GW_HWNDNEXT))
				{
					if (hChild != hWnd->m_hHorizontalScrollBar
						&& hChild != hWnd->m_hVerticalScrollBar)
					{
						CanvasLayoutCtrl(hChild);
					}
				}
			}
			return 0;

			default:
				break;
			}


		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			// 把所有鼠标消息，转给窗口过程处理
			// 注意，WM_MOUSELEAVE等鼠标消息，不在下面这个范围内
			// 因为鼠标事件即有下沉路由，又有冒泡路由，所以，只处理下沉路由
			if (pe->RoutedEvent >= WM_NCMOUSEMOVE
				&& pe->RoutedEvent <= WM_NCXBUTTONDBLCLK)
			{
				OnNcMouseMessage(hWnd, uMsg, wParam, lParam);
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (uMsg)
			{
			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
				DefPaint(hWnd, (HDC)wParam, *LPCRECT(lParam));
				return 0;

			case WM_SETREDRAW:
				hWnd->m_bRedraw = wParam;
				return 0;

			case WM_SETTEXT:
				if (hWnd->m_strCaption == (LPCTSTR)lParam)
				{
					return FALSE;
				}

				hWnd->m_strCaption = (LPCTSTR)lParam;
				Invalidate(hWnd);

				return TRUE;

			case WM_GETTEXTLENGTH:
				return hWnd->m_strCaption.GetLength();

			case WM_GETTEXT:
			{
				int nLen = wParam;
				LPTSTR lpszBuffer = (LPTSTR)lParam;
				int nCaptionLen = hWnd->m_strCaption.GetLength();
				if (nCaptionLen >= nLen)
				{
					_tcsncpy(lpszBuffer, (LPCTSTR)hWnd->m_strCaption, nLen - 1);
					return nLen - 1;
				}
				else
				{
					StringCchCopy(lpszBuffer, nLen, (LPCTSTR)hWnd->m_strCaption);
					return nCaptionLen;
				}
			}
			return 0;

			case WLM_SET_CLIP:
				hWnd->m_bClip = wParam == 0 ? FALSE : TRUE;
				return 0;

			case WLM_GET_CLIP:
				return hWnd->m_bClip;

			case WM_WL_ISUSERECTF:
				return hWnd->m_bUseRectF;

			case WM_WL_GETRECTF:
			{
				Gdiplus::RectF *pRectF = (Gdiplus::RectF *)lParam;
				*pRectF = *hWnd->m_pRectF;
			}
			return 0;

			case WM_TIMER:
			{
				UINT_PTR uIDEvent = wParam;

				WLTIMERINFO *pTimer = NULL;
				std::map<UINT_PTR, HTIMER>::iterator it = hWnd->m_mapTimer.begin();
				if (it != hWnd->m_mapTimer.end())
				{
					pTimer = (WLTIMERINFO *)it->second;
				}
				if (pTimer != NULL)
				{
					if (uIDEvent == IDT_TRANSLATE_TRANSFORM_X
						|| uIDEvent == IDT_TRANSLATE_TRANSFORM_Y)
					{
						DataAnimation *pAnimation = NULL;

						if (uIDEvent == IDT_TRANSLATE_TRANSFORM_X)
						{
							pAnimation = (DataAnimation *)hWnd->m_mapAnimation[ATP_TRANSLATE_TRANSFORM_X];
						}
						else if (uIDEvent == IDT_TRANSLATE_TRANSFORM_Y)
						{
							pAnimation = (DataAnimation *)hWnd->m_mapAnimation[ATP_TRANSLATE_TRANSFORM_Y];
						}
						else
						{
							_ASSERT(FALSE);
						}

						if (pAnimation->GetDuration() > 0)
						{
							bool bEnd = false;
							DWORD dwCurTime = GetTickCount();
							if (dwCurTime - pTimer->dwStartTime >= pAnimation->GetDuration())
							{
								// 当Duration到了时，准备下一步动作
								pAnimation->TimerBehaviorStrategy(pTimer, &dwCurTime, &bEnd);
							}

							if (dwCurTime - pTimer->dwStartTime > pAnimation->GetDuration())
							{
								dwCurTime = pTimer->dwStartTime + pAnimation->GetDuration();
							}

							Gdiplus::REAL nRange = pAnimation->GetTo() - pAnimation->GetFrom();
							Gdiplus::REAL nCurValue = 0;
							if (pAnimation->m_bCurReverse)
							{
								nCurValue = pAnimation->GetTo() - (dwCurTime - pTimer->dwStartTime) * nRange / pAnimation->GetDuration();
							}
							else
							{
								nCurValue = pAnimation->GetFrom() + (dwCurTime - pTimer->dwStartTime) * nRange / pAnimation->GetDuration();
							}

							CIUIRect rect;
							GetWindowRectToParent(hWnd, rect);

							hWnd->m_bUseRectF = true;

							if (uIDEvent == IDT_TRANSLATE_TRANSFORM_X)
							{
								Gdiplus::RectF r;
								r.X = nCurValue;
								r.Y = (Gdiplus::REAL)rect.top;
								r.Width = (Gdiplus::REAL)rect.Width();
								r.Height = (Gdiplus::REAL)rect.Height();
								*hWnd->m_pRectF = r;
							}
							else if (uIDEvent == IDT_TRANSLATE_TRANSFORM_Y)
							{
								Gdiplus::RectF r;
								r.X = (Gdiplus::REAL)rect.left;
								r.Y = nCurValue;
								r.Width = (Gdiplus::REAL)rect.Width();
								r.Height = (Gdiplus::REAL)rect.Height();
								*hWnd->m_pRectF = r;
							}
							else
							{
								_ASSERT(FALSE);
							}

							if (bEnd)
							{
								if (pAnimation->GetFillBehavior() == FB_STOP)
								{
									if (uIDEvent == IDT_TRANSLATE_TRANSFORM_X)
									{
										rect.OffsetRect((int)pAnimation->GetFrom() - rect.left, 0);
									}
									else if (uIDEvent == IDT_TRANSLATE_TRANSFORM_Y)
									{
										rect.OffsetRect(0, (int)pAnimation->GetFrom() - rect.top);
									}
									else
									{
										_ASSERT(FALSE);
									}
									MoveWindow(hWnd, rect, TRUE);
								}
							}

							::InvalidateRect(GetHostWnd(hWnd), NULL, FALSE);
							::UpdateWindow(GetHostWnd(hWnd));
						}
					}
					else if (uIDEvent == IDT_TRANSLATE_TRANSFORM_POS)
					{
						DataAnimationUsingPath *pAnimation = (DataAnimationUsingPath *)hWnd->m_mapAnimation[ATP_TRANSLATE_TRANSFORM_POS];

						if (pAnimation->GetDuration() > 0)
						{
							if (!IsWindowVisible(hWnd))
							{
								ShowWindow(hWnd, SW_SHOW);
							}

							bool bEnd = false;
							DWORD dwCurTime = GetTickCount();
							if (dwCurTime - pTimer->dwStartTime >= pAnimation->GetDuration())
							{
								// 当Duration到了时，准备下一步动作
								pAnimation->TimerBehaviorStrategy(pTimer, &dwCurTime, &bEnd);
							}

							if (dwCurTime - pTimer->dwStartTime > pAnimation->GetDuration())
							{
								dwCurTime = pTimer->dwStartTime + pAnimation->GetDuration();
							}

							CFlattenPathPoint pt;
							if (pAnimation->m_bCurReverse)
							{
								pAnimation->GetPoint(pAnimation->GetDuration() - (dwCurTime - pTimer->dwStartTime), &pt);
							}
							else
							{
								pAnimation->GetPoint(dwCurTime - pTimer->dwStartTime, &pt);
							}

							CIUIRect rect;
							GetWindowRectToParent(hWnd, rect);

							CIUIPoint ptBase = (LPARAM)pAnimation->GetTargetValue();

							Gdiplus::RectF r;
							r.X = pt.GetPoint().X - ptBase.x;
							r.Y = pt.GetPoint().Y - ptBase.y;
							r.Width = (Gdiplus::REAL)rect.Width();
							r.Height = (Gdiplus::REAL)rect.Height();

							hWnd->m_bUseRectF = true;
							*hWnd->m_pRectF = r;

							if (bEnd)
							{
								if (pAnimation->GetFillBehavior() == FB_STOP)
								{
								}
							}

							::InvalidateRect(GetHostWnd(hWnd), NULL, FALSE);
							::UpdateWindow(GetHostWnd(hWnd));
						}
					}
				}
			}
			return 0;

			case WLM_COMBINE_BK_IMAGE:
			{
				BOOL bOld = hWnd->ci.m_bCombineBkImage;
				hWnd->ci.m_bCombineBkImage = wParam;
				return bOld;
			}

			case WLM_IS_COMBINE_BK_IMAGE:
				return hWnd->ci.m_bCombineBkImage;

			case WLM_SET_BK_COLOR:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					hWnd->ci.m_crBkN = cr;
					return 0;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					hWnd->ci.m_crBkD = cr;
					return 0;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_BK_COLOR:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					return hWnd->ci.m_crBkN;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					return hWnd->ci.m_crBkD;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_BORDER_COLOR:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					hWnd->ci.m_crBorderN = cr;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					hWnd->ci.m_crBorderD = cr;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_BORDER_COLOR:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					return hWnd->ci.m_crBorderN;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					return hWnd->ci.m_crBorderD;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_BIND_SCROLLBAR_STYLE:
				if (wParam)
				{
					hWnd->m_pBindHorzScrollBarStyle = (CScrollBarProp *)lParam;

					if (IsWindow(hWnd->m_hHorizontalScrollBar))
					{
						ScrollBar_BindStyle(hWnd->m_hHorizontalScrollBar,
							hWnd->m_pBindHorzScrollBarStyle);
					}
				}
				else
				{
					hWnd->m_pBindVertScrollBarStyle = (CScrollBarProp *)lParam;

					if (IsWindow(hWnd->m_hVerticalScrollBar))
					{
						ScrollBar_BindStyle(hWnd->m_hVerticalScrollBar,
							hWnd->m_pBindVertScrollBarStyle);
					}
				}
				return 0;

			case WLM_GET_BIND_SCROLLBAR_STYLE:
				if (wParam)
				{
					return (LRESULT)hWnd->m_pBindHorzScrollBarStyle;
				}
				else
				{
					return (LRESULT)hWnd->m_pBindVertScrollBarStyle;
				}
				break;

			case WLM_SETHIDEOLEBORDER:
			{
				BOOL bOld = hWnd->m_bHideOleBorder;
				hWnd->m_bHideOleBorder = lParam;
				return bOld;
			}
			case WLM_GETHIDEOLEBORDER:
				return hWnd->m_bHideOleBorder;

			default:
				break;
			}
		}
	}

	return 0;
}

LRESULT IUI::CallIUIHandlerMap(HWLWND hWnd, RoutedEventArgs *pe)
{
	if (hWnd == NULL || pe == NULL)
	{
		_ASSERT(FALSE);
		return 0;
	}

	if (hWnd->m_pUserBindWLWndHandler == NULL)
	{
		// Must return 0 to continue message
		return 0;
	}

	return hWnd->m_pUserBindWLWndHandler->EventHandler(hWnd, pe);
}

LRESULT IUI::RaiseEvent(HWLWND hTargetWnd, RoutedEventArgs *pe)
{
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();
	if (pThreadState != NULL)
	{
		pThreadState->m_LastEvent = *pe;
	}
	else
	{
		_ASSERT(FALSE);
	}

	if (!IsWindow(hTargetWnd))
	{
		return -1;
	}

	LRESULT lr = 0;
	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		// 依次向目标窗口及其祖宗分发事件
		HWLWND hCurTargetWnd = hTargetWnd;
		while (hCurTargetWnd != NULL)
		{
			// 当前的消息是谁发送的
			pe->hSender = hCurTargetWnd;

			// 分发事件
			if (hCurTargetWnd->lpfnEventHandler != NULL)
			{
				// 先执行用户指定的Handler
				if (hCurTargetWnd->m_pUserBindWLWndHandler != NULL)
				{
					lr = CallIUIHandlerMap(hCurTargetWnd, pe);
					if (pe->bHandled)
					{
						break;
					}
				}

				// 再执行控件自己的Handler
				lr = hCurTargetWnd->lpfnEventHandler(hCurTargetWnd, pe);
			}
			else
			{
				_ASSERT(FALSE);
			}

			if (pe->bHandled)
			{
				break;
			}

			// 在上面的hParent->EventHandler中，可能会释放掉控件
			if (!::IsWindow(GetHostWnd(hTargetWnd)))
			{
				pe->bHandled = true;
				break;
			}

			hCurTargetWnd = GetParent(hCurTargetWnd);
		}

		// 每一个事件，不光视觉树上每一个控件应该收到，宿主窗口也应该收到，
		// 所以，最后向宿主窗口发送消息。
		// 执行到这里，hTargetWnd可能已经无效了，调用GetHostWnd(hTargetWnd)可能引起程序崩溃，
		// 所以先判断pe->bHandled
		if (!pe->bHandled && GetHostWnd(hTargetWnd) != NULL)
		{
			lr = ::SendMessage(GetHostWnd(hTargetWnd), WM_CONTROLEVENT, pe->RoutedEvent, (LPARAM)pe);
		}
	}
	else if (pe->eRoutingStrategy == RS_TUNNEL)
	{
		// 记录视觉树
		std::vector<HWLWND> vElement;

		HWLWND hParent = hTargetWnd;
		while (hParent != NULL)
		{
			vElement.push_back(hParent);
			hParent = GetParent(hParent);
		}

		// 依次调用视觉树每个元素的

		// 每一个事件，不光视觉树上每一个控件应该收到，宿主窗口也应该收到，
		// 所以，最先向宿主窗口发送消息。
		if (GetHostWnd(hTargetWnd) != NULL)
		{
			lr = ::SendMessage(GetHostWnd(hTargetWnd), WM_CONTROLEVENT, pe->RoutedEvent, (LPARAM)pe);
		}

		if (pe->bHandled)
		{
			return lr;
		}
		// 在上面的WM_CONTROLEVENT中，可能会释放掉控件
		if (!::IsWindow(GetHostWnd(hTargetWnd)))
		{
			pe->bHandled = true;
			return -1;
		}

		std::vector<HWLWND>::reverse_iterator it = vElement.rbegin();
		for (; it != vElement.rend(); it++)
		{
			HWLWND hCurTargetWnd = *it;

			pe->hSender = hCurTargetWnd;

			// 分发事件
			if (hCurTargetWnd->lpfnEventHandler != NULL)
			{
				// 先执行用户指定的Handler
				if (hCurTargetWnd->m_pUserBindWLWndHandler != NULL)
				{
					lr = CallIUIHandlerMap(hCurTargetWnd, pe);
					if (pe->bHandled)
					{
						break;
					}
				}

				lr = hCurTargetWnd->lpfnEventHandler(hCurTargetWnd, pe);
			}
			else
			{
				_ASSERT(FALSE);
			}

			// 在上面的EventHandler中，可能会释放掉控件, 所以，后续一定不要访问控件本身的内存。
			if (pe->bHandled)
			{
				break;
			}
		}

		if (!pe->bHandled)
		{
			// 然后再发出冒泡事件
			RoutedEventArgs arg;
			arg.hSender = pe->hSender;
			arg.hOriginalSender = pe->hOriginalSender;
			arg.RoutedEvent = pe->RoutedEvent;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = pe->wParam;
			arg.lParam = pe->lParam;
			RaiseEvent(arg.hOriginalSender, &arg);

			int n = 0;
		}
	}
	else if (pe->eRoutingStrategy == RS_DIRECT)
	{
		if (hTargetWnd->lpfnEventHandler != NULL)
		{
			// 先执行用户指定的Handler
			if (hTargetWnd->m_pUserBindWLWndHandler != NULL)
			{
				lr = CallIUIHandlerMap(hTargetWnd, pe);
				if (pe->bHandled)
				{
					return lr;
				}
			}

			lr = hTargetWnd->lpfnEventHandler(hTargetWnd, pe);
			return lr;
		}
		else
		{
			_ASSERT(FALSE);
			return 0;
		}
	}

	return lr;
}

BOOL IUI::EnableScrollBar(HWLWND hWnd, UINT wSBflags, UINT wArrows)
{
	if (!IsWindow(hWnd))
	{
		return FALSE;
	}

	BOOL bEnableVertical = ((wSBflags == SB_BOTH) || (wSBflags == SB_VERT));
	BOOL bEnableHorizontal = ((wSBflags == SB_BOTH) || (wSBflags == SB_HORZ));

	if (bEnableVertical && hWnd->m_hVerticalScrollBar == NULL)
	{
		HWLWND hVertScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_VERT,
				0, 0, 0, 0, GetHostWnd(hWnd), hWnd, XNAME_VERTSCROLLBAR, NULL, NULL);

		hWnd->m_hVerticalScrollBar = hVertScrollBar;
	}
	else if (!bEnableVertical && hWnd->m_hVerticalScrollBar)
	{
		delete hWnd->m_hVerticalScrollBar;
		hWnd->m_hVerticalScrollBar = NULL;
	}

	if (bEnableHorizontal && hWnd->m_hHorizontalScrollBar == NULL)
	{
		HWLWND hHorzScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_HORZ,
				0, 0, 0, 0, GetHostWnd(hWnd), hWnd, XNAME_HORZSCROLLBAR, NULL, NULL);

		hWnd->m_hHorizontalScrollBar = hHorzScrollBar;
	}
	else if (!bEnableHorizontal && hWnd->m_hHorizontalScrollBar)
	{
		delete hWnd->m_hHorizontalScrollBar;
		hWnd->m_hHorizontalScrollBar = NULL;
	}

	Invalidate(hWnd);

	return TRUE;
}

void IUI::EnableFloatScrollBar(HWLWND hWnd, BOOL bFloatVBar, BOOL bFloatHBar)
{
	hWnd->m_bFloatVBar = bFloatVBar;
	hWnd->m_bFloatHBar = bFloatHBar;
}

BOOL IUI::IsFloatVBar(HWLWND hWnd)
{
	return hWnd->m_bFloatVBar;
}

BOOL IUI::IsFloatHBar(HWLWND hWnd)
{
	return hWnd->m_bFloatHBar;
}

int IUI::SetScrollRange(HWLWND hWnd, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw)
{
	if (nMaxPos < nMinPos)
	{
		nMaxPos = nMinPos;
	}

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;
	si.nMin = nMinPos;
	si.nMax = nMaxPos;

	return SetScrollInfo(hWnd, nBar, &si, bRedraw);
}

int IUI::GetScrollRange(HWLWND hWnd, int nBar, int *pnMinPos, int *pnMaxPos)
{
	if (pnMinPos == NULL || pnMaxPos == NULL)
	{
		return -1;
	}

	if (nBar == SB_VERT)
	{
		return SendMessage(hWnd->m_hVerticalScrollBar,
				SBM_GETRANGE, (WPARAM)pnMinPos, (LPARAM)pnMaxPos);
	}
	else if (nBar == SB_HORZ)
	{
		return SendMessage(hWnd->m_hHorizontalScrollBar,
				SBM_GETRANGE, (WPARAM)pnMinPos, (LPARAM)pnMaxPos);
	}

	return 0;
}

int IUI::SetScrollPos(HWLWND hWnd, int nBar, int nPos, BOOL bRedraw/* = TRUE*/)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	si.nPos = nPos;

	return SetScrollInfo(hWnd, nBar, &si, bRedraw);
}

int IUI::GetScrollPos(HWLWND hWnd, int nBar)
{
	if (hWnd->pSBInfo == NULL)
	{
		return 0;
	}

	SBDATA *pSBData = NULL;
	if (nBar == SB_HORZ)
	{
		pSBData = &hWnd->pSBInfo->Horz;
	}
	else if (nBar == SB_VERT)
	{
		pSBData = &hWnd->pSBInfo->Vert;
	}

	if (pSBData == NULL)
	{
		return 0;
	}

	return pSBData->pos;
}

int LayoutScrollBar(HWLWND hWnd)
{
	CIUIRect rcWnd;
	GetWindowRectToParent(hWnd, rcWnd);
	rcWnd.OffsetRect(-rcWnd.TopLeft());

	DWORD dwStyle = GetStyle(hWnd);
	BOOL bShowHScrollBar = IsIncludeFlag(dwStyle, WS_HSCROLL);
	BOOL bShowVScrollBar = IsIncludeFlag(dwStyle, WS_VSCROLL);

	DWORD dwExStyle = GetExStyle(hWnd);
	BOOL bLeftVScrollBar = IsIncludeFlag(dwExStyle, WS_EX_LEFTSCROLLBAR);

	// 如果水平或垂直滚动条为NULL，则创建
	if (bShowHScrollBar && hWnd->m_hHorizontalScrollBar == NULL)
	{
		HWLWND hHorzScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_HORZ,
				0, 0, 0, 0, GetHostWnd(hWnd), hWnd, XNAME_HORZSCROLLBAR, NULL, NULL);
		if (hHorzScrollBar == NULL)
		{
			return -1;
		}

		hWnd->m_hHorizontalScrollBar = hHorzScrollBar;
		// Bind Style
		const CScrollBarProp *pBindStyle = (CScrollBarProp *)SendMessage(hWnd,
				WLM_GET_BIND_SCROLLBAR_STYLE, TRUE, 0);
		if (NULL == pBindStyle)
		{
			pBindStyle = g_pHorScrollBarBindProp;
		}
		ScrollBar_BindStyle(GetHorizontalScrollBar(hWnd), pBindStyle);
	}

	if (bShowVScrollBar && hWnd->m_hVerticalScrollBar == NULL)
	{
		HWLWND hVertScrollBar = CreateWindowEx(NULL, 0, IUI_SCROLLBAR, NULL, SBS_VERT,
				0, 0, 0, 0, GetHostWnd(hWnd), hWnd, XNAME_VERTSCROLLBAR, NULL, NULL);
		if (hVertScrollBar == NULL)
		{
			return -2;
		}

		hWnd->m_hVerticalScrollBar = hVertScrollBar;
		// Bind Style
		const CScrollBarProp *pBindStyle = (CScrollBarProp *)SendMessage(hWnd,
				WLM_GET_BIND_SCROLLBAR_STYLE, FALSE, 0);
		if (NULL == pBindStyle)
		{
			pBindStyle = g_pVerScrollBarBindProp;
		}
		ScrollBar_BindStyle(GetVerticalScrollBar(hWnd), pBindStyle);
	}

	if (bShowHScrollBar)
	{
		CIUIRect rcHScrollBar(rcWnd);
		rcHScrollBar.top = rcHScrollBar.bottom - GetHScrollBarHeight(hWnd);

		if (bShowVScrollBar)
		{
			if (bLeftVScrollBar)
			{
				rcHScrollBar.left += GetVScrollBarWidth(hWnd);
			}
			else
			{
				rcHScrollBar.right -= GetVScrollBarWidth(hWnd);
			}
		}

		CIUIRect rcOld;
		GetWindowRectToParent(GetHorizontalScrollBar(hWnd), rcOld);
		if (rcOld != rcHScrollBar)
		{
			IUI::MoveWindow(GetHorizontalScrollBar(hWnd), rcHScrollBar, TRUE);
		}

		if (!IsWindowVisible(GetHorizontalScrollBar(hWnd)))
		{
			ShowWindow(GetHorizontalScrollBar(hWnd), SW_SHOW);
		}
	}
	else
	{
		ShowWindow(GetHorizontalScrollBar(hWnd), SW_HIDE);
	}

	if (bShowVScrollBar)
	{
		CIUIRect rcVScrollBar(rcWnd);

		if (bLeftVScrollBar)
		{
			rcVScrollBar.right = rcVScrollBar.left + GetVScrollBarWidth(hWnd);
		}
		else
		{
			rcVScrollBar.left = rcVScrollBar.right - GetVScrollBarWidth(hWnd);
		}

		if (bShowHScrollBar)
		{
			rcVScrollBar.bottom -= GetHScrollBarHeight(hWnd);
		}

		CIUIRect rcOld;
		GetWindowRectToParent(GetVerticalScrollBar(hWnd), rcOld);
		if (rcOld != rcVScrollBar)
		{
			MoveWindow(GetVerticalScrollBar(hWnd), rcVScrollBar, TRUE);
		}

		// 在ULW模式，ShowWindow会引起UpdateLayeredWindow调用
		// 判断一下，减小调用次数
		if (!IsWindowVisible(GetVerticalScrollBar(hWnd)))
		{
			ShowWindow(GetVerticalScrollBar(hWnd), SW_SHOW);
		}
	}
	else
	{
		ShowWindow(GetVerticalScrollBar(hWnd), SW_HIDE);
	}

	return 0;
}

// 返回滚动条当前位置
int IUI::SetScrollInfo(HWLWND hWnd, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw)
{
	if (hWnd == NULL || lpsi == NULL)
	{
		return 0;
	}

	if (hWnd->pSBInfo == NULL)
	{
		hWnd->pSBInfo = new SBINFO;
		ZeroMemory(hWnd->pSBInfo, sizeof(SBINFO));
	}

	SBDATA *pSBData = NULL;
	if (fnBar == SB_HORZ)
	{
		pSBData = &hWnd->pSBInfo->Horz;
	}
	else if (fnBar == SB_VERT)
	{
		pSBData = &hWnd->pSBInfo->Vert;
	}

	if (pSBData == NULL)
	{
		return 0;
	}

	if (IsIncludeFlag(lpsi->fMask, SIF_RANGE))
	{
		pSBData->posMin = lpsi->nMin;
		pSBData->posMax = lpsi->nMax;
		if (pSBData->posMax < pSBData->posMin)
		{
			pSBData->posMax = pSBData->posMin;
		}
	}

	if (IsIncludeFlag(lpsi->fMask, SIF_POS))
	{
		pSBData->pos = lpsi->nPos;
	}

	if (IsIncludeFlag(lpsi->fMask, SIF_PAGE))
	{
		pSBData->page = lpsi->nPage;
	}

	if (pSBData->page < 0)
	{
		pSBData->page = 0;
	}
	if (pSBData->page > pSBData->posMax - pSBData->posMin + 1)
	{
		pSBData->page = pSBData->posMax - pSBData->posMin + 1;
	}

	if (pSBData->pos < pSBData->posMin)
	{
		pSBData->pos = pSBData->posMin;
	}
	if (pSBData->pos > pSBData->posMax - max(pSBData->page - 1, 0))
	{
		pSBData->pos = pSBData->posMax - max(pSBData->page - 1, 0);
	}

	// 判断是否需要出现滚动条
	// 不能用lpsi判断是否出现滚动条，因为lpsi->fMask可能只包含SIF_POS
	BOOL bShowScroll = FALSE;
	if (int(pSBData->page) < (pSBData->posMax + 1) && pSBData->page > 0)
	{
		bShowScroll = TRUE;
	}

	if (fnBar == SB_VERT)
	{
		if (bShowScroll)
		{
			AddStyle(hWnd, WS_VSCROLL);
		}
		else
		{
			RemoveStyle(hWnd, WS_VSCROLL);
		}
	}
	else if (fnBar == SB_HORZ)
	{
		if (bShowScroll)
		{
			AddStyle(hWnd, WS_HSCROLL);
		}
		else
		{
			RemoveStyle(hWnd, WS_HSCROLL);
		}
	}

	// 先设置滚动条位置，因为下面为滚动条设置SetScrollInfo时，会
	// 利用滚动条坐标，计算上下按钮及滑块坐标
	LayoutScrollBar(hWnd);

	if (IsIncludeFlag(lpsi->fMask, SIF_TRACKPOS))
	{
		// MSDN中的SetScrollInfo不允许设置Track pos
		_ASSERT(FALSE);
	}

	// 把滚动信息设置到滚动条控件上
	HWLWND hScrollBar = (fnBar == SB_HORZ ? hWnd->m_hHorizontalScrollBar : hWnd->m_hVerticalScrollBar);
	if (hScrollBar != NULL)
	{
		SCROLLINFO si;
		si = *lpsi;

		// 因为SBM_SETSCROLLINFO内部对SCROLLINFO的值进行合法性检查时，可能会修改它的值
		// 而外面调用SetScrollInfo后，可能还要使用lpsi，所以，这里传一个lpsi的拷贝给
		// SBM_SETSCROLLINFO消息。 (解决调整Header 最右侧Item的右分隔线时，ListView水平滚动
		// 错乱的问题)
		SendMessage(hScrollBar, SBM_SETSCROLLINFO, fRedraw, (LPARAM)&si);
	}

	return pSBData->pos;
}

BOOL IUI::GetScrollInfo(HWLWND hWnd, int nBar, LPSCROLLINFO lpScrollInfo)
{
	if (lpScrollInfo == NULL)
	{
		return FALSE;
	}

	if (hWnd->pSBInfo == NULL)
	{
		return FALSE;
	}

	SBDATA *pSBData = NULL;
	if (nBar == SB_HORZ)
	{
		pSBData = &hWnd->pSBInfo->Horz;
	}
	else if (nBar == SB_VERT)
	{
		pSBData = &hWnd->pSBInfo->Vert;
	}

	if (pSBData == NULL)
	{
		return FALSE;
	}

	if (IsIncludeFlag(lpScrollInfo->fMask, SIF_RANGE))
	{
		lpScrollInfo->nMin = pSBData->posMin;
		lpScrollInfo->nMax = pSBData->posMax;
	}

	if (IsIncludeFlag(lpScrollInfo->fMask, SIF_PAGE))
	{
		lpScrollInfo->nPage = pSBData->page;
	}

	if (IsIncludeFlag(lpScrollInfo->fMask, SIF_POS))
	{
		lpScrollInfo->nPos = pSBData->pos;
	}

	if (IsIncludeFlag(lpScrollInfo->fMask, SIF_TRACKPOS))
	{
		// Track Pos是滚动条控件滑块的实时位置，只能从控件上得到。
		HWLWND hScrollBar = NULL;
		if (nBar == SB_VERT)
		{
			hScrollBar = hWnd->m_hVerticalScrollBar;
		}
		else if (nBar == SB_HORZ)
		{
			hScrollBar = hWnd->m_hHorizontalScrollBar;
		}

		if (hScrollBar == NULL)
		{
			return FALSE;
		}

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		SendMessage(hScrollBar, SBM_GETSCROLLINFO, 0, (LPARAM)&si);

		lpScrollInfo->nTrackPos = si.nTrackPos;
	}

	return TRUE;
}

BOOL IUI::ShowScrollBar(HWLWND hWnd, int wBar, BOOL bShow)
{
	if (wBar == SB_VERT)
	{
		if (hWnd->m_hVerticalScrollBar != NULL)
		{
			ShowWindow(hWnd->m_hVerticalScrollBar, bShow ? SW_SHOW : SW_HIDE);
			return TRUE;
		}
	}
	else if (wBar == SB_HORZ)
	{
		if (hWnd->m_hHorizontalScrollBar != NULL)
		{
			ShowWindow(hWnd->m_hHorizontalScrollBar, bShow ? SW_SHOW : SW_HIDE);
			return TRUE;
		}
	}

	return FALSE;
}

int IUI::SetHScrollBarHeight(HWLWND hWnd, int nHeight)
{
	int nOld = hWnd->m_nHScrollBarHeight;
	hWnd->m_nHScrollBarHeight = nHeight;
	return nOld;
}

int IUI::GetHScrollBarHeight(HWLWND hWnd)
{
	return hWnd->m_nHScrollBarHeight;
}

int IUI::SetVScrollBarWidth(HWLWND hWnd, int nWidth)
{
	int nOld = hWnd->m_nVScrollBarWidth;
	hWnd->m_nVScrollBarWidth = nWidth;
	return nOld;
}

int IUI::GetVScrollBarWidth(HWLWND hWnd)
{
	return hWnd->m_nVScrollBarWidth;
}

HWLWND IUI::GetVerticalScrollBar(HWLWND hWnd)
{
	return hWnd->m_hVerticalScrollBar;
}

HWLWND IUI::GetHorizontalScrollBar(HWLWND hWnd)
{
	return hWnd->m_hHorizontalScrollBar;
}

void IUI::SetMember(HWLWND hWnd, void *pMember)
{
	hWnd->m_pMember = pMember;
}

void *IUI::GetMember(HWLWND hWnd)
{
	return hWnd->m_pMember;
}

// See: InternalSetProp
// 原生SetProp/GetProp不通过消息来设置，所以我们也不通过消息来设置.
// 我们把控件的旧的EventHandler通过SetProp设置给控件，
// 在控件新的EventHandler中，通过GetProp来得到旧的EventHandler，
// 如果SetProp/GetProp是通过消息来设置，将会引起死循环。
BOOL IUI::SetProp(HWLWND hCtrl, LPCTSTR lpString, HANDLE hData)
{
	if (hCtrl == NULL || lpString == NULL)
	{
		return FALSE;
	}

	HPROPLIST pmapProp = hCtrl->m_pMapProp;
	if (pmapProp == NULL)
	{
		return FALSE;
	}

	(*pmapProp)[lpString] = hData;

	return TRUE;
}

HANDLE IUI::GetProp(HWLWND hCtrl, LPCTSTR lpString)
{
	if (hCtrl == NULL || lpString == NULL)
	{
		return NULL;
	}

	HPROPLIST pmapProp = hCtrl->m_pMapProp;
	if (pmapProp == NULL)
	{
		return NULL;
	}

	std::map<CIUIString, HANDLE>::iterator it = pmapProp->find(lpString);
	if (it == pmapProp->end())
	{
		return NULL;
	}

	return it->second;
}

HANDLE IUI::RemoveProp(HWLWND hCtrl, LPCTSTR lpString)
{
	if (hCtrl == NULL || lpString == NULL)
	{
		return NULL;
	}

	HPROPLIST pmapProp = hCtrl->m_pMapProp;
	if (pmapProp == NULL)
	{
		return NULL;
	}

	std::map<CIUIString, HANDLE>::iterator it = pmapProp->find(lpString);
	if (it == pmapProp->end())
	{
		return NULL;
	}

	HANDLE hRet = it->second;

	pmapProp->erase(it);

	return hRet;
}

HWND IUI::SetToolTips(HWLWND hWnd, HWND pWndTip)
{
	_ASSERT(IsWindow(hWnd));
	return (HWND)SendMessage(hWnd, WLM_SETTOOLTIPS, 0, (LPARAM)pWndTip);
}

HWND IUI::GetToolTips(HWLWND hWnd)
{
	_ASSERT(IsWindow(hWnd));
	return (HWND)SendMessage(hWnd, WLM_GETTOOLTIPS, 0, 0L);
}

// 子类化windowless控件
fnWLEventHandler IUI::SubclassWindowless(HWLWND hWnd, fnWLEventHandler lpfnNew)
{
	if (hWnd == NULL || lpfnNew == NULL)
	{
		_ASSERT(FALSE);
		return NULL;
	}

	RoutedEventArgs arg;
	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_SUBCLASSING;
	arg.eRoutingStrategy = RS_DIRECT;
	arg.wParam = (WPARAM)hWnd->lpfnEventHandler;
	arg.lParam = (LPARAM)lpfnNew;
	LRESULT lr = RaiseEvent(hWnd, &arg);
	if (arg.bHandled)
	{
		return hWnd->lpfnEventHandler;
	}

	fnWLEventHandler lpfnOld = hWnd->lpfnEventHandler;
	hWnd->lpfnEventHandler = lpfnNew;
	SetProp(hWnd, _T("GetProp_Old_Handler"), lpfnOld);

	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_SUBCLASSED;
	arg.eRoutingStrategy = RS_DIRECT;
	arg.wParam = (WPARAM)lpfnOld;
	arg.lParam = (LPARAM)lpfnNew;
	lr = RaiseEvent(hWnd, &arg);

	return lpfnOld;
}

// 超类化windowless控件
fnWLEventHandler IUI::SuperclassWindowless(LPCTSTR lpszClassName, fnWLEventHandler lpfnNew)
{
	if (lpszClassName == NULL || lpfnNew == NULL)
	{
		_ASSERT(FALSE);
		return NULL;
	}

	std::map<CIUIString, CONST WLWNDCLASS *>::iterator it = m_gWLClassMap.m_mapWLSuperClasses.find(lpszClassName);
	if (it == m_gWLClassMap.m_mapWLSuperClasses.end())
	{
		return NULL;
	}

	WLWNDCLASS *pwc = (WLWNDCLASS *)it->second;

	fnWLEventHandler lpfnOld = pwc->lpfnEventHandler;

	//RegisterWindowlessClass(lpszClassName, lpfnNew);
	pwc->lpfnEventHandler = lpfnNew;

	return lpfnOld;
}

LRESULT WINAPI IUI::CallEventHandler(fnWLEventHandler lpPrevEventHandler, HWLWND hWnd, RoutedEventArgs *pe)
{
	if (lpPrevEventHandler == NULL)
	{
		return 0;
	}

	return lpPrevEventHandler(hWnd, pe);
}

int IUI::BindStyle(HWLWND hWnd, const CControlProp *pProp)
{
	return SendMessage(hWnd, WLM_BIND_STYLE, 0, (LPARAM)pProp);
}

BOOL IUI::WalkHWLWNDChild(HWLWND hWndParent, WalkHWLWNDCallBack fn, void *pCallbackParam)
{
	if (hWndParent == NULL)
	{
		return FALSE;
	}

	HWLWND hChild = GetWindow(hWndParent, GW_CHILD);

	while (hChild != NULL)
	{
#ifdef _DEBUG
		CIUIString str = GetName(hChild);
#endif // _DEBUG

		BOOL bRet = fn(hChild, pCallbackParam);
		if (bRet)
		{
			// 不再遍历，提前结束
			return TRUE;
		}

		bRet = WalkHWLWNDChild(hChild, fn, pCallbackParam);
		if (bRet)
		{
			// 孩子提前结束
			return TRUE;
		}

		hChild = GetWindow(hChild, GW_HWNDNEXT);
	}

	return FALSE;
}

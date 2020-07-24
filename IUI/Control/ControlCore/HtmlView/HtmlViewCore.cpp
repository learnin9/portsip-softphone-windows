#include "stdafx.h"
#include <ExDispid.h>
#if(_MSC_VER > 1200) // VC6.0
#include <atlstr.h> // for CString
#include <atltypes.h> // for CRect
#else
#include <atlbase.h>
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


LRESULT CALLBACK BrowserHost_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Host缩放时，调整WebBrowser的尺寸
	if (uMsg == WM_SIZE)
	{
		HWND hIE = ::GetWindow(hWnd, GW_CHILD);
		if (::IsWindow(hIE))
		{
			CIUIRect rcWnd;
			::GetClientRect(hWnd, rcWnd);
			::MoveWindow(hIE, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), TRUE);
		}
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateBrowserHost(DWORD dwStyle, const RECT &rect, HWND hParent)
{
	CIUIString strClassName = _T("IUI_HTMLVIEWHOST");
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = BrowserHost_WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = IUIGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = strClassName;
	ATOM ret = ::RegisterClass(&wc);
	_ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);

	HWND hWnd = ::CreateWindow(strClassName, NULL, dwStyle,
			rect.left, rect.top, RECTWIDTH(rect), RECTHEIGHT(rect),
			hParent, NULL, wc.hInstance, NULL);

	return hWnd;
}

HtmlViewCore::HtmlViewCore()
{
	pHtml = new HTMLV;
	pHtml->pHtmlViewCore = this;
}

HtmlViewCore::~HtmlViewCore()
{
	Release();
	delete pHtml;
}

//IUnKnown

HRESULT HtmlViewCore::QueryInterface(REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;
	if (riid == IID_IOleClientSite)
	{
		*ppvObject = (IOleClientSite *)this;
	}
	if (riid == IID_IUnknown)
	{
		*ppvObject = this;
	}
	if (riid == IID_IDispatch)
	{
		*ppvObject = (IDispatch *)this;
	}

	if (!pHtml->m_bInPlaced)
	{
		if (riid == IID_IOleInPlaceSite)
		{
			*ppvObject = (IOleInPlaceSite *)this;
		}
		if (riid == IID_IOleInPlaceFrame)
		{
			*ppvObject = (IOleInPlaceFrame *)this;
		}
		if (riid == IID_IOleInPlaceUIWindow)
		{
			*ppvObject = (IOleInPlaceUIWindow *)this;
		}
	}
	if (riid == DIID_DWebBrowserEvents2)
	{
		*ppvObject = (DWebBrowserEvents2 *)this;
	}
	if (riid == IID_IDocHostUIHandler)
	{
		*ppvObject = (IDocHostUIHandler *)this;
	}
	if (*ppvObject)
	{
		AddRef();
		return ERROR_SUCCESS;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG HtmlViewCore::AddRef(void)
{
	InterlockedExchangeAdd((PLONG)&pHtml->m_dwRefCount, 1);
	return ERROR_SUCCESS;
}

ULONG HtmlViewCore::Release(void)
{
	InterlockedExchangeAdd((PLONG)&pHtml->m_dwRefCount, -1);

	return ERROR_SUCCESS;
}

//IDispatch

HRESULT HtmlViewCore::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames,
	UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

// 关于dispIdMember各事件参数，请参考MSDN:DWebBrowserEvents2接口
// 例如DISPID_BEFORENAVIGATE2事件，对应DWebBrowserEvents2::BeforeNavigate2
// DWebBrowserEvents2::BeforeNavigate2的7个参数，都保存在pDispParams中，
// DWebBrowserEvents2::BeforeNavigate2的第一个参数pDisp对应((*pDispParams).rgvarg)[6]
// 第二个参数对应((*pDispParams).rgvarg)[5]，依此类推。 dispIdMember事件对应的DWebBrowserEvents2
// 接口中的方法有几个参数，(*pDispParams).rgvarg就包含几个参数。
HRESULT HtmlViewCore::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	// 给父亲一个执行的机会。
	//SHtmlViewInvoke HtmlInvoke;
	//HtmlInvoke.dispIdMember = dispIdMember;
	////	HtmlInvoke.riid = riid;
	//HtmlInvoke.lcid = lcid;
	//HtmlInvoke.wFlags = wFlags;
	//HtmlInvoke.pDispParams = pDispParams;
	//HtmlInvoke.pVarResult = pVarResult;
	//HtmlInvoke.pExcepInfo = pExcepInfo;
	//HtmlInvoke.puArgErr = puArgErr;

	//RoutedEventArgs arg;
	//arg.hSender = pHtml->hwnd;
	//arg.hOriginalSender = pHtml->hwnd;
	//arg.RoutedEvent = UE_IE_INVOKE;
	//arg.eRoutingStrategy = RS_BUBBLE;
	//arg.lParam = (LPARAM)&HtmlInvoke;
	//LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	//if (lr != 0)
	//{
	//	return 0;
	//}

	//switch (dispIdMember)
	//{
	//case DISPID_NAVIGATECOMPLETE2:
	//{
	//	_ASSERT(V_VT(pDispParams->rgvarg->pvarVal) == VT_BSTR);
	//	CIUIString str(V_BSTR(pDispParams->rgvarg->pvarVal));
	//	OnNavigateComplete2(str);
	//}
	//break;

	//case DISPID_BEFORENAVIGATE2:
	//	if (((*pDispParams).rgvarg)[5].vt == (VT_BYREF | VT_VARIANT))
	//	{
	//		// 解析参数
	//		IDispatch *pDisp = ((*pDispParams).rgvarg)[6].pdispVal;

	//		VARIANT *URL = ((*pDispParams).rgvarg)[5].pvarVal;
	//		_ASSERT(V_VT(URL) == VT_BSTR);

	//		VARIANT *Flags = ((*pDispParams).rgvarg)[4].pvarVal;

	//		VARIANT *TargetFrameName = ((*pDispParams).rgvarg)[3].pvarVal;
	//		_ASSERT(V_VT(TargetFrameName) == VT_BSTR);

	//		VARIANT *PostData = ((*pDispParams).rgvarg)[2].pvarVal;
	//		_ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));

	//		VARIANT *Headers = ((*pDispParams).rgvarg)[1].pvarVal;
	//		_ASSERT(V_VT(Headers) == VT_BSTR);

	//		VARIANT_BOOL *Cancel = ((*pDispParams).rgvarg)[0].pboolVal;
	//		_ASSERT(Cancel != NULL);

	//		// 生成PostData
	//		VARIANT *vtPostedData = V_VARIANTREF(PostData);
	//		// CByteArray array;
	//		std::vector<BYTE> vPostData;
	//		if (V_VT(vtPostedData) & VT_ARRAY)
	//		{
	//			// must be a vector of bytes
	//			_ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

	//			vtPostedData->vt |= VT_UI1;
	//			COleSafeArray safe(vtPostedData);

	//			DWORD dwSize = safe.GetOneDimSize();
	//			LPVOID pVoid;
	//			safe.AccessData(&pVoid);

	//			for (DWORD i = 0; i < dwSize; ++i)
	//			{
	//				vPostData.push_back(*((BYTE *)pVoid + i));
	//			}
	//			// array.SetSize(dwSize);
	//			// LPBYTE lpByte = array.GetData();
	//			// memcpy_s(lpByte, dwSize, pVoid, dwSize);
	//			safe.UnaccessData();
	//		}

	//		// make real parameters out of the notification

	//		CIUIString strTargetFrameName(V_BSTR(TargetFrameName));
	//		CIUIString strURL(V_BSTR(URL));
	//		CIUIString strHeaders(V_BSTR(Headers));
	//		DWORD nFlags = V_I4(Flags);


	//		BOOL bCancel = FALSE;
	//		// notify the user's class
	//		OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
	//			vPostData, strHeaders, &bCancel);

	//		if (bCancel)
	//		{
	//			*Cancel = -1;
	//		}
	//		else
	//		{
	//			*Cancel = 0;
	//		}
	//	}
	//	else
	//	{
	//		_ASSERT(FALSE);
	//	}
	//	break;

	//case DISPID_STATUSTEXTCHANGE:
	//	OnStatusTextChange(pDispParams->rgvarg[0].bstrVal);
	//	break;

	//case DISPID_PROGRESSCHANGE:
	//	OnProgressChange(
	//		pDispParams->rgvarg[1].lVal,
	//		pDispParams->rgvarg[0].lVal);
	//	break;

	//case DISPID_COMMANDSTATECHANGE:
	//	OnCommandStateChange(
	//		pDispParams->rgvarg[1].lVal,
	//		pDispParams->rgvarg[0].boolVal);
	//	break;

	//case DISPID_DOWNLOADBEGIN:
	//	OnDownloadBegin();
	//	break;

	//case DISPID_DOWNLOADCOMPLETE:
	//	OnDownloadComplete();
	//	break;

	//case DISPID_TITLECHANGE:
	//	OnTitleChange(pDispParams->rgvarg->bstrVal);
	//	break;

	//case DISPID_PROPERTYCHANGE:
	//	OnPropertyChange(pDispParams->rgvarg->bstrVal);
	//	break;

	//case DISPID_NEWWINDOW2:
	//{
	//	VARIANT_BOOL *pvbCancel = ((*pDispParams).rgvarg)[0].pboolVal;
	//	BOOL bCancel = FALSE;

	//	OnNewWindow2(((*pDispParams).rgvarg)[1].ppdispVal, &bCancel);
	//	if (pvbCancel)
	//	{
	//		*pvbCancel = bCancel ? (-1) : 0;
	//	}
	//}
	//break;

	//case DISPID_DOCUMENTCOMPLETE:
	//{
	//	_ASSERT(V_VT(pDispParams->rgvarg->pvarVal) == VT_BSTR);
	//	CIUIString str(V_BSTR(pDispParams->rgvarg->pvarVal));
	//	OnDocumentComplete(str);
	//}
	//break;

	//case DISPID_ONQUIT:
	//	OnQuit();
	//	break;

	//case DISPID_ONVISIBLE:
	//	OnVisible(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_ONTOOLBAR:
	//	OnToolBar(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_ONMENUBAR:
	//	OnMenuBar(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_ONSTATUSBAR:
	//	OnStatusBar(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_ONFULLSCREEN:
	//	OnFullScreen(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_ONTHEATERMODE:
	//	OnTheaterMode(pDispParams->rgvarg->boolVal);
	//	break;

	//case DISPID_NAVIGATEERROR:
	//{
	//	// 解析参数
	//	IDispatch *pDisp = ((*pDispParams).rgvarg)[4].pdispVal;
	//	VARIANT *pvURL = ((*pDispParams).rgvarg)[3].pvarVal;
	//	VARIANT *pvFrame = ((*pDispParams).rgvarg)[2].pvarVal;
	//	VARIANT *pvStatusCode = ((*pDispParams).rgvarg)[1].pvarVal;
	//	VARIANT_BOOL *pvbCancel = ((*pDispParams).rgvarg)[0].pboolVal;

	//	_ASSERT(pvURL != NULL);
	//	_ASSERT(pvFrame != NULL);
	//	_ASSERT(pvStatusCode != NULL);
	//	_ASSERT(pvbCancel != NULL);
	//	_ASSERT(V_VT(pvURL) == VT_BSTR);
	//	_ASSERT(V_VT(pvFrame) == VT_BSTR);

	//	CIUIString strURL(V_BSTR(pvURL));
	//	CIUIString strFrame(V_BSTR(pvFrame));
	//	DWORD dwError = V_I4(pvStatusCode);

	//	BOOL bCancel = FALSE;
	//	// notify the user's class
	//	OnNavigateError(strURL, strFrame, dwError, &bCancel);

	//	if (pvbCancel)
	//	{
	//		*pvbCancel = bCancel ? (-1) : 0;
	//	}
	//}
	//break;

	//}

	return E_NOTIMPL;
}

//IOleWindow

HRESULT HtmlViewCore::GetWindow(HWND *phwnd)
{
	*phwnd = pHtml->m_hHostWnd;
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

//IOleInPlaceUIWindow

HRESULT HtmlViewCore::GetBorder(LPRECT lprectBorder)
{
	::GetClientRect(pHtml->m_hHostWnd, lprectBorder);
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	return ERROR_SUCCESS;
}

//IOleInPlaceFrame

HRESULT HtmlViewCore::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::RemoveMenus(HMENU hmenuShared)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::SetStatusText(LPCOLESTR pszStatusText)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	return TRUE;
}

//IOleClientSite

HRESULT HtmlViewCore::SaveObject(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	*ppmk = NULL;
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::GetContainer(IOleContainer **ppContainer)
{
	*ppContainer = NULL;
	return E_FAIL;
}

HRESULT HtmlViewCore::ShowObject(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnShowWindow(BOOL fShow)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::RequestNewObjectLayout(void)
{
	return ERROR_SUCCESS;
}

//IDocHostUIHandler

HRESULT HtmlViewCore::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	if (pHtml->m_dwXDocHostUI_Flag & DOCHOSTUIFLAG_DISABLE_HELP_MENU)
	{
		return S_OK;
	}

	return S_FALSE;
}

HRESULT HtmlViewCore::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = pHtml->m_dwXDocHostUI_Flag;
	pInfo->dwDoubleClick = pHtml->m_dwXDocHostUI_DblClk;

	return S_OK;
}

HRESULT HtmlViewCore::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject,
	IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::HideUI(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::UpdateUI(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnDocWindowActivate(BOOL fActivate)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnFrameWindowActivate(BOOL fActivate)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::GetExternal(IDispatch **ppDispatch)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return ERROR_SUCCESS;
}

//IOleInPlaceSite

HRESULT HtmlViewCore::CanInPlaceActivate(void)
{
	if (pHtml->m_bInPlaced)
	{
		return S_FALSE;
	}
	else
	{
		pHtml->m_bInPlaced = TRUE;
		return S_OK;
	}
}

HRESULT HtmlViewCore::OnInPlaceActivate(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnUIActivate(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
	LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	RECT rect;

	*ppFrame = (IOleInPlaceFrame *)this;
	AddRef();
	*ppDoc = NULL;
	::GetClientRect(pHtml->m_hHostWnd, &rect);
	*lprcPosRect = rect;
	*lprcClipRect = rect;
	lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
	lpFrameInfo->hwndFrame = ::GetParent(pHtml->m_hHostWnd);
	lpFrameInfo->cAccelEntries = 0;
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->haccel = NULL;
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT HtmlViewCore::OnUIDeactivate(BOOL fUndoable)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnInPlaceDeactivate(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::DiscardUndoState(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::DeactivateAndUndo(void)
{
	return ERROR_SUCCESS;
}

HRESULT HtmlViewCore::OnPosRectChange(LPCRECT lprcPosRect)
{
	return ERROR_SUCCESS;
}

LRESULT WINAPI IUI::HtmlView_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	HtmlViewCore *pHtmlView = (HtmlViewCore *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pe->RoutedEvent == WM_NCCREATE)
			{
				if (pHtmlView == NULL)
				{
					pHtmlView = new HtmlViewCore;
					pHtmlView->pHtml->hwnd = hWnd;
					pHtmlView->pHtml->hwnd->ci.hwndParent = GetParent(hWnd);

					SetMember(hWnd, pHtmlView);

					return 0;
				}
			}
			else if (pe->RoutedEvent == WM_CREATE)
			{
				pHtmlView->pHtml->m_dwXDocHostUI_DblClk = 0;
				pHtmlView->pHtml->m_dwXDocHostUI_Flag = 0;

				// 在CWLWnd::Create中，会发送WM_NCCREATE消息给外部，外部可能修改了窗口风格。
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;

				if (!IsEditMode(hWnd))
				{
					// 创建一个Browser的Host窗口
					CIUIRect rcBrowser(lpcs->x, lpcs->y, lpcs->x + lpcs->cx, lpcs->y + lpcs->cy);
					if (GetParent(hWnd) != NULL)
					{
						ClientToScreen(GetParent(hWnd), rcBrowser);
						IUIScreenToClient(lpcs->hwndParent, rcBrowser);
					}
					HWND hBrowserHost = CreateBrowserHost(lpcs->style | WS_CHILD,
							rcBrowser, lpcs->hwndParent);
					if (!::IsWindow(hBrowserHost))
					{
						return FALSE;
					}

					// Error checking omitted for clarity
					CComPtr<IOleObject> spOleObj;

					// Create WebBrowser--store pointer in class member variable m_spWebBrowser
					CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void **)&pHtmlView->pHtml->m_pBrowserApp);

					// Query WebBrowser for IOleObject pointer
					pHtmlView->pHtml->m_pBrowserApp->QueryInterface(IID_IOleObject, (void **)&spOleObj);

					pHtmlView->pHtml->m_hHostWnd = hBrowserHost;
					// Set client site
					spOleObj->SetClientSite(pHtmlView);

					// In-place activate the WebBrowser control
					RECT rcClient;
					::GetClientRect(hBrowserHost, &rcClient);
					spOleObj->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pHtmlView, 0, hBrowserHost, &rcClient);

					// Register container to intercept WebBrowser events
					//	AtlAdvise(m_spWebBrowser, GetUnknown(), DIID_DWebBrowserEvents2, &m_dwCookie);
				}


				return 0;
			}
			else if (pe->RoutedEvent == WM_NCDESTROY)
			{
				if (!IsEditMode(hWnd))
				{
					if (pHtmlView->pHtml->m_pOleInPlaceActiveObject != NULL)
					{
						pHtmlView->pHtml->m_pOleInPlaceActiveObject->Release();
					}
					if (pHtmlView->pHtml->m_pOleInPlaceObject != NULL)
					{
						pHtmlView->pHtml->m_pOleInPlaceObject->Release();
					}
					if (pHtmlView->pHtml->m_pBrowserApp != NULL)
					{
						pHtmlView->pHtml->m_pBrowserApp->Release();
					}
					if (pHtmlView->pHtml->m_pOleObject != NULL)
					{
						pHtmlView->pHtml->m_pOleObject->Close(OLECLOSE_NOSAVE);
						pHtmlView->pHtml->m_pOleObject->Release();
					}
				}

				// TODO: 加上后，测试带HtmlView的窗口在关闭时会崩溃。
				//delete pHtmlView;
				//pHtmlView = NULL;
				//SetMember(hWnd, NULL);
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (pe->RoutedEvent)
			{
			case WLM_BIND_STYLE:
				HtmlView_BindStyle(hWnd, (CHtmlViewProp *)lParam);
				return 0;

			default:
				break;
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

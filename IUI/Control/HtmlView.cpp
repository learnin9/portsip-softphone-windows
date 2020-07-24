#include "stdafx.h"
#include <ExDispid.h>
#if(_MSC_VER <= 1200) // VC6.0
#include <ATLBASE.H>
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

#if (_MSC_VER <= 1200) // VC6.0

const GUID IID_IHTMLDocument3 = {0x3050f485, 0x98b5, 0x11cf, {0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b}};

#endif

HtmlView::HtmlView()
{

}

HtmlView::~HtmlView()
{

}

int HtmlView::BindStyle(const CControlProp *pCtrlProp)
{
	return HtmlView_BindStyle(m_hWnd, (const CHtmlViewProp *)pCtrlProp);
}

BOOL HtmlView::Create(
	__in CWLWndHandler *pUserHandler,
	DWORD dwStyle,
	const RECT &rect,
	HWND hParent,
	CWLWnd *pParentWnd,
	LPCTSTR lpszName)
{
	BOOL bRet = CWLWnd::Create(pUserHandler, IUI_HTMLVIEW, NULL,
			dwStyle, rect, hParent, pParentWnd, lpszName);
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;
}

IOleObject *HtmlView::GetIOleObject()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	return pHtml->m_pOleObject;
}

IWebBrowser2 *HtmlView::GetIWebBrowser2()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	return pHtml->m_pBrowserApp;
}

IOleInPlaceObject *HtmlView::GetIOleInPlaceObject()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	return pHtml->m_pOleInPlaceObject;
}

IOleInPlaceActiveObject *HtmlView::GetIOleInPlaceActiveObject()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	return pHtml->m_pOleInPlaceActiveObject;
}

/////////////////////////////////////////////////////////////////////////////
// HtmlViewCore properties

CIUIString HtmlView::GetType() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CComBSTR bstr;
	HRESULT hr = pHtml->m_pBrowserApp->get_Type(&bstr);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return CIUIString();
	}

	CIUIString retVal(bstr);
	return retVal;
}

HWND HtmlView::GetHWND() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	long result = 0;
	HRESULT hr = pHtml->m_pBrowserApp->get_HWND(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return NULL;
		// AfxThrowUserException();
	}

	return (HWND)result;
}

long HtmlView::GetLeft() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	long result = 0;
	HRESULT hr = pHtml->m_pBrowserApp->get_Left(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return hr;
		// AfxThrowUserException();
	}

	return result;
}


long HtmlView::GetTop() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	long result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Top(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return hr;

		//AfxThrowUserException();
	}

	return result;
}

int HtmlView::GetToolBar() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	int result;
	HRESULT hr = pHtml->m_pBrowserApp->get_ToolBar(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return hr;

		//AfxThrowUserException();
	}

	return result;
}

int HtmlView::GetHeight() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	long result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Height(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return hr;

		//AfxThrowUserException();
	}

	return result;
}

int HtmlView::GetWidth() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	long result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Width(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return hr;

		//AfxThrowUserException();
	}

	return result;
}

BOOL HtmlView::GetVisible() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Visible(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

CIUIString HtmlView::GetLocationName() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CComBSTR bstr;
	HRESULT hr = pHtml->m_pBrowserApp->get_LocationName(&bstr);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return CIUIString();
	}

	CIUIString retVal(bstr);
	return retVal;
}

CIUIString HtmlView::GetLocationURL() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CComBSTR bstr;
	HRESULT hr = pHtml->m_pBrowserApp->get_LocationURL(&bstr);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return CIUIString();
	}

	CIUIString retVal(bstr);
	return retVal;
}

BOOL HtmlView::GetBusy() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Busy(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

READYSTATE HtmlView::GetReadyState() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	READYSTATE result;
	HRESULT hr = pHtml->m_pBrowserApp->get_ReadyState(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return result;

		//AfxThrowUserException();
	}

	return result;
}

BOOL HtmlView::GetOffline() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Offline(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetSilent() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Silent(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

LPDISPATCH HtmlView::GetApplication() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	LPDISPATCH result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Application(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return NULL;
	}

	return result;
}


LPDISPATCH HtmlView::GetParentBrowser() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	LPDISPATCH result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Parent(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return NULL;
	}

	return result;
}

LPDISPATCH HtmlView::GetContainer() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	LPDISPATCH result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Container(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return NULL;
	}

	return result;
}

LPDISPATCH HtmlView::GetHtmlDocument() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	LPDISPATCH result;
	HRESULT hr = pHtml->m_pBrowserApp->get_Document(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return NULL;
	}

	return result;
}

BOOL HtmlView::GetTopLevelContainer() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);


	VARIANT_BOOL result;
	pHtml->m_pBrowserApp->get_TopLevelContainer(&result);
	return result;
}

BOOL HtmlView::GetMenuBar() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_MenuBar(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetFullScreen() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_FullScreen(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetStatusBar() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_StatusBar(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

OLECMDF HtmlView::QueryStatusWB(OLECMDID cmdID) const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	OLECMDF result;
	HRESULT hr = pHtml->m_pBrowserApp->QueryStatusWB(cmdID, &result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return result;

		//AfxThrowUserException();
	}

	return result;
}

void HtmlView::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
	VARIANT *pvaIn, VARIANT *pvaOut)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	pHtml->m_pBrowserApp->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

/////////////////////////////////////////////////////////////////////////////
// HtmlView operations

BOOL HtmlView::GetSource(CIUIString &refString)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	ENSURE(pHtml->m_pBrowserApp != NULL);

	BOOL bRetVal = FALSE;

	CComPtr<IDispatch> spDisp;
	pHtml->m_pBrowserApp->get_Document(&spDisp);
	if (spDisp != NULL)
	{
		HGLOBAL hMemory;
		hMemory = LocalAlloc(GMEM_MOVEABLE, 0);
		if (hMemory != NULL)
		{
			CComQIPtr<IPersistStreamInit> spPersistStream = spDisp;
			if (spPersistStream != NULL)
			{
				CComPtr<IStream> spStream;
				if (SUCCEEDED(CreateStreamOnHGlobal(hMemory, TRUE, &spStream)))
				{
					spPersistStream->Save(spStream, FALSE);

					STATSTG statStg;
					spStream->Stat(&statStg, STATFLAG_NONAME);

					LPCSTR pstr = static_cast<LPCSTR>(GlobalLock(hMemory));
					if (pstr != NULL)
					{
						// Stream is expected to be ANSI (CP-ACP). CIUIString constructor
						// will convert implicitly, and truncate to correct length.

						bRetVal = TRUE;
						//TRY
						{
							refString = CIUIString(pstr, statStg.cbSize.LowPart);
						}
						//CATCH_ALL(e)
						//{
						//	bRetVal = FALSE;
						//	DELETE_EXCEPTION(e);
						//}
						//END_CATCH_ALL

						if (bRetVal == FALSE)
						{
							LocalFree(hMemory);
						}
						else
						{
							GlobalUnlock(hMemory);
						}
					}
					else
					{
						LocalFree(hMemory);
					}
				}
				else
				{
					LocalFree(hMemory);
				}
			}
			else
			{
				LocalFree(hMemory);
			}
		}
	}

	return bRetVal;
}

BOOL HtmlView::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = IUIGetInstanceHandle();
	_ASSERT(hInstance != NULL);

	CIUIString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
	{
		bRetVal = FALSE;
	}

	delete [] lpszModule;
	return bRetVal;
}

BOOL HtmlView::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = IUIGetInstanceHandle();
	_ASSERT(hInstance != NULL);

	CIUIString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
	{
		bRetVal = FALSE;
	}

	delete [] lpszModule;
	return bRetVal;
}

void HtmlView::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	CIUIString strURL(lpszURL);
	CComBSTR bstrURL;
	bstrURL.Attach(strURL.AllocSysString());

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
		{
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);
		}

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	pHtml->m_pBrowserApp->Navigate(bstrURL,
		CIUIOleVariant((long) dwFlags, VT_I4),
		CIUIOleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		CIUIOleVariant(lpszHeaders, VT_BSTR));
}

void HtmlView::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CIUIOleVariant vPIDL(pIDL);
	CIUIOleVariant empty;

	pHtml->m_pBrowserApp->Navigate2(vPIDL,
		CIUIOleVariant((long) dwFlags, VT_I4),
		CIUIOleVariant(lpszTargetFrameName, VT_BSTR),
		empty, empty);
}

void HtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */,
	LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
		{
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);
		}

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	CIUIOleVariant vURL(lpszURL, VT_BSTR);
	CIUIOleVariant vHeaders(lpszHeaders, VT_BSTR);
	CIUIOleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	CIUIOleVariant vFlags((long) dwFlags, VT_I4);

	pHtml->m_pBrowserApp->Navigate2(vURL,
		vFlags, vTargetFrameName, vPostData, vHeaders);
}

void HtmlView::PutProperty(LPCTSTR lpszProperty, const VARIANT &vtValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CIUIString strProp(lpszProperty);
	BSTR bstrProp = strProp.AllocSysString();
	pHtml->m_pBrowserApp->PutProperty(bstrProp, vtValue);
	::SysFreeString(bstrProp);
}

BOOL HtmlView::GetProperty(LPCTSTR lpszProperty, CIUIString &strValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CIUIString strProperty(lpszProperty);
	BSTR bstrProperty = strProperty.AllocSysString();

	BOOL bResult = FALSE;
	VARIANT vReturn;
	vReturn.vt = VT_BSTR;
	vReturn.bstrVal = NULL;
	HRESULT hr = pHtml->m_pBrowserApp->GetProperty(bstrProperty, &vReturn);

	if (SUCCEEDED(hr))
	{
		strValue = CIUIString(vReturn.bstrVal);
		bResult = TRUE;
	}

	::SysFreeString(bstrProperty);
	return bResult;
}

#define VTS_WBSTR           "\x08"      // an 'LPCOLESTR'

#if defined(_UNICODE)
#define VTS_BSTR            VTS_WBSTR// an 'LPCOLESTR'
#define VT_BSTRT            VT_BSTR
#else
#define VTS_BSTR            "\x0E"  // an 'LPCSTR'
#define VT_BSTRA            14
#define VT_BSTRT            VT_BSTRA
#endif

CIUIOleVariant HtmlView::GetProperty(LPCTSTR lpszProperty)
{
	CIUIOleVariant result;

	//static BYTE parms[] = VTS_BSTR;
	//m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
	//	VT_VARIANT, (void *)&result, parms, lpszProperty);

	return result;
}

CIUIString HtmlView::GetFullName() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	CComBSTR bstr;
	HRESULT hr = pHtml->m_pBrowserApp->get_FullName(&bstr);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return CIUIString();
	}

	CIUIString retVal(bstr);
	return retVal;
}

HRESULT HtmlView::QueryFormsCommand(DWORD dwCommandID,
	BOOL *pbSupported, BOOL *pbEnabled, BOOL *pbChecked)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	ENSURE(pHtml->m_pBrowserApp != NULL);
	HRESULT hr = E_FAIL;

	CComPtr<IDispatch> spDispDocument;
	pHtml->m_pBrowserApp->get_Document(&spDispDocument);
	CComQIPtr<IHTMLDocument2> spDoc = spDispDocument;
	if (spDoc != NULL)
	{
		CComQIPtr<IOleCommandTarget> spCmdTarget = spDoc;
		if (spCmdTarget != NULL)
		{
			OLECMD cmdInfo;
			cmdInfo.cmdID = dwCommandID;
			cmdInfo.cmdf = 0;

			hr = spCmdTarget->QueryStatus(&CMDSETID_Forms3, 1, &cmdInfo, NULL);

			if (SUCCEEDED(hr))
			{
				if (pbSupported != NULL)
				{
					*pbSupported = (cmdInfo.cmdf & OLECMDF_SUPPORTED) ? TRUE : FALSE;
				}
				if (pbEnabled != NULL)
				{
					*pbEnabled = (cmdInfo.cmdf & OLECMDF_ENABLED) ? TRUE : FALSE;
				}
				if (pbChecked != NULL)
				{
					*pbChecked = (cmdInfo.cmdf & OLECMDF_LATCHED) ? TRUE : FALSE;
				}
			}
		}
	}

	return hr;
}

HRESULT HtmlView::ExecFormsCommand(DWORD dwCommandID,
	VARIANT *pVarIn, VARIANT *pVarOut)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	ENSURE(pHtml->m_pBrowserApp != NULL);
	HRESULT hr = E_FAIL;
	CComPtr<IDispatch> spDispDocument;
	pHtml->m_pBrowserApp->get_Document(&spDispDocument);
	CComQIPtr<IHTMLDocument2> spDoc = spDispDocument;
	if (spDoc != NULL)
	{
		CComQIPtr<IOleCommandTarget> spCmdTarget = spDoc;
		if (spCmdTarget != NULL)
			hr = spCmdTarget->Exec(&CMDSETID_Forms3, dwCommandID,
					OLECMDEXECOPT_DONTPROMPTUSER, pVarIn, pVarOut);
	}

	return hr;
}

void HtmlView::FilePrint()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	if (pHtml->m_pBrowserApp != NULL)
	{
		CComPtr<IDispatch> spDisp;
		pHtml->m_pBrowserApp->get_Document(&spDisp);
		if (spDisp != NULL)
		{
			// the control will handle all printing UI

			CComQIPtr<IOleCommandTarget> spTarget = spDisp;
			if (spTarget != NULL)
			{
				spTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL, NULL);
			}
		}
	}
}

BOOL HtmlView::GetRegisterAsBrowser() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_RegisterAsBrowser(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetRegisterAsDropTarget() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_RegisterAsDropTarget(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetTheaterMode() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_TheaterMode(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

BOOL HtmlView::GetAddressBar() const
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	HRESULT hr = pHtml->m_pBrowserApp->get_AddressBar(&result);
	if (FAILED(hr))
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	return result;
}

void HtmlView::SetRegisterAsBrowser(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_RegisterAsBrowser((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetRegisterAsDropTarget(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_RegisterAsDropTarget((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetTheaterMode(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_TheaterMode((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetVisible(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Visible((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetMenuBar(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_MenuBar((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetToolBar(int nNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_ToolBar(nNewValue);
}

void HtmlView::SetOffline(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Offline((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetSilent(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Silent((short)(bNewValue ? (-1) : 0));
}

void HtmlView::GoBack()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->GoBack();
}

void HtmlView::GoForward()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->GoForward();
}

void HtmlView::GoHome()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->GoHome();
}

void HtmlView::GoSearch()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->GoSearch();
}

void HtmlView::Refresh()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->Refresh();
}

void HtmlView::Refresh2(int nLevel)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);

	pHtml->m_pBrowserApp->Refresh2(CIUIOleVariant((long) nLevel, VT_I4));
}

void HtmlView::Stop()
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->Stop();
}

void HtmlView::SetFullScreen(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_FullScreen((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetAddressBar(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_AddressBar((short)(bNewValue ? (-1) : 0));
}

void HtmlView::SetHeight(long nNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Height(nNewValue);
}

void HtmlView::SetWidth(long nNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Width(nNewValue);
}

void HtmlView::PutProperty(LPCTSTR lpszPropertyName, long lValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	PutProperty(lpszPropertyName, CIUIOleVariant(lValue, VT_UI4));
}

void HtmlView::PutProperty(LPCTSTR lpszPropertyName, short nValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	PutProperty(lpszPropertyName, CIUIOleVariant(nValue, VT_UI2));
}

void HtmlView::PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	PutProperty(lpszPropertyName, CIUIOleVariant(lpszValue, VT_BSTR));
}

void HtmlView::PutProperty(LPCTSTR lpszPropertyName, double dValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	PutProperty(lpszPropertyName, CIUIOleVariant(dValue));
}

void HtmlView::SetTop(long nNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Top(nNewValue);
}

void HtmlView::SetLeft(long nNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_Left(nNewValue);
}

void HtmlView::SetStatusBar(BOOL bNewValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	_ASSERT(pHtml->m_pBrowserApp != NULL);
	pHtml->m_pBrowserApp->put_StatusBar((short)(bNewValue ? (-1) : 0));
}

// Event
void HtmlView::OnNavigateComplete2(LPCTSTR strURL)
{

}

void HtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
	LPCTSTR lpszTargetFrameName, std::vector<BYTE> &baPostedData,
	LPCTSTR lpszHeaders, BOOL *pbCancel)
{

}

void HtmlView::OnStatusTextChange(LPCTSTR lpszText)
{

}

void HtmlView::OnProgressChange(long nProgress, long nProgressMax)
{

}

void HtmlView::OnCommandStateChange(long nCommand, BOOL bEnable)
{

}

void HtmlView::OnDownloadBegin()
{

}

void HtmlView::OnDownloadComplete()
{

}

void HtmlView::OnTitleChange(LPCTSTR lpszText)
{

}

void HtmlView::OnPropertyChange(LPCTSTR lpszProperty)
{

}

void HtmlView::OnNewWindow2(LPDISPATCH *ppDisp, BOOL *Cancel)
{

}

void HtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	// user will override to handle this notification
	if (!CIUIString(lpszURL).IsEmpty())
	{
		pHtml->m_JScript.SetScriptDocument(GetHtmlDocument());
	}
}

void HtmlView::OnQuit()
{

}

void HtmlView::OnVisible(BOOL bVisible)
{

}

void HtmlView::OnToolBar(BOOL bToolBar)
{

}

void HtmlView::OnMenuBar(BOOL bMenuBar)
{

}

void HtmlView::OnStatusBar(BOOL bStatusBar)
{

}

void HtmlView::OnFullScreen(BOOL bFullScreen)
{

}

void HtmlView::OnTheaterMode(BOOL bTheaterMode)
{

}

void HtmlView::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{

}

int HtmlView::ShowBorder(BOOL bShow, BOOL bRedraw/*=TRUE*/)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	// #if(_MSC_VER <= 1200) // VC6.0
	// 	DWORD dwFlags = pHtml->m_pOccManager.m_pCustSite->GetXDocHostUI_Flag();
	// 	if (bShow)
	// 	{
	// 		dwFlags &= ~DOCHOSTUIFLAG_NO3DBORDER;
	// 	}
	// 	else
	// 	{
	// 		dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
	// 	}
	// 	pHtml->m_pOccManager.m_pCustSite->SetXDocHostUI_Flag(dwFlags);
	// #else
	if (bShow)
	{
		pHtml->m_dwXDocHostUI_Flag &= ~DOCHOSTUIFLAG_NO3DBORDER;
	}
	else
	{
		pHtml->m_dwXDocHostUI_Flag |= DOCHOSTUIFLAG_NO3DBORDER;
	}
	//#endif

	if (bRedraw)
	{
		BSTR bstr;
		pHtml->m_pBrowserApp->get_LocationURL(&bstr);
		USES_CONVERSION;
		Navigate2(W2T(bstr));
	}

	return 0;
}

int HtmlView::ShowIEScrollBar(BOOL bShow, BOOL bRedraw/*=TRUE*/)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	// #if(_MSC_VER <= 1200) // VC6.0
	// 	DWORD dwFlags = pHtml->m_pOccManager.m_pCustSite->GetXDocHostUI_Flag();
	// 	if (bShow)
	// 	{
	// 		dwFlags &= ~DOCHOSTUIFLAG_SCROLL_NO;
	// 	}
	// 	else
	// 	{
	// 		dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
	// 	}
	// 	pHtml->m_pOccManager.m_pCustSite->SetXDocHostUI_Flag(dwFlags);
	// #else
	if (bShow)
	{
		pHtml->m_dwXDocHostUI_Flag &= ~DOCHOSTUIFLAG_SCROLL_NO;
	}
	else
	{
		pHtml->m_dwXDocHostUI_Flag |= DOCHOSTUIFLAG_SCROLL_NO;
	}
	//#endif

	if (bRedraw)
	{
		BSTR bstr;
		pHtml->m_pBrowserApp->get_LocationURL(&bstr);
		USES_CONVERSION;
		Navigate2(W2T(bstr));
	}

	return 0;
}

int HtmlView::ShowContextMenu(BOOL bShow)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;
	// #if(_MSC_VER <= 1200) // VC6.0
	// 	DWORD dwFlags = pHtml->m_pOccManager.m_pCustSite->GetXDocHostUI_Flag();
	// 	if (bShow)
	// 	{
	// 		dwFlags &= ~DOCHOSTUIFLAG_DISABLE_HELP_MENU;
	// 	}
	// 	else
	// 	{
	// 		dwFlags |= DOCHOSTUIFLAG_DISABLE_HELP_MENU;
	// 	}
	// 	pHtml->m_pOccManager.m_pCustSite->SetXDocHostUI_Flag(dwFlags);
	// #else
	if (bShow)
	{
		pHtml->m_dwXDocHostUI_Flag &= ~DOCHOSTUIFLAG_DISABLE_HELP_MENU;
	}
	else
	{
		pHtml->m_dwXDocHostUI_Flag |= DOCHOSTUIFLAG_DISABLE_HELP_MENU;
	}
	//#endif

	return 0;
}

BOOL HtmlView::PutHtmlCode(PWCHAR pszCode)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	BOOL bRet = FALSE;
	VARIANT vUrl;
	IDispatch *pDispatch = NULL;
	IHTMLDocument2 *htmlDoc2 = NULL;
	SAFEARRAY *sfArray = NULL;
	SAFEARRAYBOUND ArrayBound = {1, 0};
	VARIANT *pVar = NULL;

	if (pszCode == NULL)
	{
		return bRet;
	}
	__try
	{
		VariantInit(&vUrl);
		vUrl.vt = VT_BSTR;
		vUrl.bstrVal = SysAllocString(L"about:blank");
		if (!vUrl.bstrVal)
		{
			__leave;
		}
		pHtml->m_pBrowserApp->Navigate2(&vUrl, NULL, NULL, NULL, NULL);
		VariantClear(&vUrl);
		if (pHtml->m_pBrowserApp->get_Document(&pDispatch) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (pDispatch == NULL)
		{
			__leave;
		}
		if (pDispatch->QueryInterface(IID_IHTMLDocument2, (void **)&htmlDoc2) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (htmlDoc2 == NULL)
		{
			__leave;
		}
		sfArray = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND *)&ArrayBound);
		if (sfArray == NULL)
		{
			__leave;
		}
		if (SafeArrayAccessData(sfArray, (void **)&pVar) != ERROR_SUCCESS)
		{
			__leave;
		}
		pVar->vt = VT_BSTR;
		pVar->bstrVal = SysAllocString(pszCode);
		htmlDoc2->write(sfArray);
		htmlDoc2->close();
		SafeArrayDestroy(sfArray);
		bRet = TRUE;
	}
	__finally
	{
		if (htmlDoc2)
		{
			htmlDoc2->Release();
		}
		if (pDispatch)
		{
			pDispatch->Release();
		}
	}
	return bRet;
}

BOOL HtmlView::GetHtmlCode(PWCHAR pszCode, int *iszCount)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	BOOL bRet = FALSE;
	IDispatch *pDispatch = NULL;
	IHTMLDocument2 *htmlDoc2 = NULL;
	IHTMLElement *pBodyElement = NULL;
	PWCHAR pHtmlCode = NULL;
	int iLen = 0;

	if (pszCode == NULL || iszCount == NULL)
	{
		return bRet;
	}
	__try
	{
		if (pHtml->m_pBrowserApp->get_Document(&pDispatch) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (pDispatch == NULL)
		{
			__leave;
		}
		if (pDispatch->QueryInterface(IID_IHTMLDocument2, (void **)&htmlDoc2) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (htmlDoc2 == NULL)
		{
			__leave;
		}
		htmlDoc2->get_body(&pBodyElement);
		if (pBodyElement == NULL)
		{
			__leave;
		}
		pBodyElement->get_innerHTML(&pHtmlCode);
		if (pHtmlCode == NULL)
		{
			__leave;
		}
		iLen = lstrlenW(pHtmlCode) + 1;//还有个'\0'
		if (pszCode)
		{
			if (*iszCount >= iLen)
			{
				ZeroMemory(pszCode, *iszCount * sizeof(WCHAR));
				CopyMemory(pszCode, pHtmlCode, iLen * sizeof(WCHAR));
			}
		}
		else
		{
			*iszCount = iLen;
			__leave;
		}
		bRet = TRUE;
	}
	__finally
	{
		if (pBodyElement)
		{
			pBodyElement->Release();
		}
		if (htmlDoc2)
		{
			htmlDoc2->Release();
		}
		if (pDispatch)
		{
			pDispatch->Release();
		}
	}
	return bRet;
}

BOOL HtmlView::ClickElementByID(PWCHAR pID)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	BOOL bRet = FALSE;
	IDispatch *pDispatch = NULL;
	IHTMLDocument3 *htmlDoc3 = NULL;
	IHTMLElement *pElement = NULL;

	if (pID == NULL)
	{
		return bRet;
	}
	__try
	{
		if (pHtml->m_pBrowserApp->get_Document(&pDispatch) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (pDispatch == NULL)
		{
			__leave;
		}
		if (pDispatch->QueryInterface(IID_IHTMLDocument3, (void **)&htmlDoc3) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (htmlDoc3 == NULL)
		{
			__leave;
		}
		htmlDoc3->getElementById(pID, &pElement);
		if (pElement == NULL)
		{
			__leave;
		}
		pElement->click();
		bRet = TRUE;
	}
	__finally
	{
		if (pElement)
		{
			pElement->Release();
		}
		if (htmlDoc3)
		{
			htmlDoc3->Release();
		}
		if (pDispatch)
		{
			pDispatch->Release();
		}
	}
	return bRet;
}

BOOL HtmlView::SetElementValueByID(PWCHAR pID, PWCHAR pszValue)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	BOOL bRet = FALSE;
	IDispatch *pDispatch = NULL;
	IHTMLDocument3 *htmlDoc3 = NULL;
	IHTMLElement *pElement = NULL;
	VARIANT variant;

	if (pID == NULL || pszValue == NULL)
	{
		return bRet;
	}
	__try
	{
		if (pHtml->m_pBrowserApp->get_Document(&pDispatch) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (pDispatch == NULL)
		{
			__leave;
		}
		if (pDispatch->QueryInterface(IID_IHTMLDocument3, (void **)&htmlDoc3) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (htmlDoc3 == NULL)
		{
			__leave;
		}
		htmlDoc3->getElementById(pID, &pElement);
		if (pElement == NULL)
		{
			__leave;
		}
		VariantInit(&variant);
		variant.vt = VT_BSTR;
		variant.bstrVal = SysAllocString(pszValue);
		if (variant.bstrVal == NULL)
		{
			__leave;
		}
		pElement->setAttribute(L"value", variant);
		VariantClear(&variant);
		bRet = TRUE;
	}
	__finally
	{
		if (pElement)
		{
			pElement->Release();
		}
		if (htmlDoc3)
		{
			htmlDoc3->Release();
		}
		if (pDispatch)
		{
			pDispatch->Release();
		}
	}
	return bRet;
}

BOOL HtmlView::GetElementValueByID(PWCHAR pID, PWCHAR pszValue, int *iszCount/*字符数*/)
{
	HTMLV *pHtml = ((HtmlViewCore *)GetMember())->pHtml;

	BOOL bRet = FALSE;
	IDispatch *pDispatch = NULL;
	IHTMLDocument3 *htmlDoc3 = NULL;
	IHTMLElement *pElement = NULL;
	VARIANT variant;
	int iLen;

	if (iszCount == NULL)
	{
		return bRet;
	}
	__try
	{
		if (pHtml->m_pBrowserApp->get_Document(&pDispatch) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (pDispatch == NULL)
		{
			__leave;
		}
		if (pDispatch->QueryInterface(IID_IHTMLDocument3, (void **)&htmlDoc3) != ERROR_SUCCESS)
		{
			__leave;
		}
		if (htmlDoc3 == NULL)
		{
			__leave;
		}
		htmlDoc3->getElementById(pID, &pElement);
		if (pElement == NULL)
		{
			__leave;
		}
		VariantInit(&variant);
		pElement->getAttribute(L"value", 0, &variant);
		iLen = lstrlenW(variant.bstrVal) + 1;
		if (*iszCount >= iLen && pszValue)
		{
			ZeroMemory(pszValue, *iszCount * sizeof(WCHAR));
			CopyMemory(pszValue, variant.bstrVal, iLen * sizeof(WCHAR));
		}
		else
		{
			*iszCount = iLen;
			VariantClear(&variant);
			__leave;
		}
		VariantClear(&variant);
		bRet = TRUE;
	}
	__finally
	{
		if (pElement)
		{
			pElement->Release();
		}
		if (htmlDoc3)
		{
			htmlDoc3->Release();
		}
		if (pDispatch)
		{
			pDispatch->Release();
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////
// HtmlView API

int IUI::HtmlView_BindStyle(HWLWND hWnd, const CHtmlViewProp *pHtmlCtrlProp)
{
	if (IsEditMode(hWnd))
	{
		return 1;
	}

	HtmlView *pHtmlView = (HtmlView *)CWLWnd::FromHandle(hWnd);

	RECT rcControl = pHtmlCtrlProp->m_rcControl;
	MakeControlMargin(GetHostWnd(hWnd), GetParent(hWnd), &rcControl, hWnd->m_rcControlMargin);

	pHtmlView->ShowBorder(!(bool)pHtmlCtrlProp->m_bHideBorder, FALSE);
	pHtmlView->ShowIEScrollBar(!(bool)pHtmlCtrlProp->m_bHideScrollBar, FALSE);
	pHtmlView->ShowContextMenu(!(bool)pHtmlCtrlProp->m_bHideContextMenu);

	// 向父亲发送事件，允许父亲提供URL
	RoutedEventArgs arg;
	arg.hSender = hWnd;
	arg.hOriginalSender = hWnd;
	arg.RoutedEvent = UE_IE_REQUEST_URL;
	arg.eRoutingStrategy = RS_BUBBLE;
	TCHAR szUrl[4096] = { 0 };
	arg.lParam = (LPARAM)szUrl;
	LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
	if (lr != 0)
	{
		pHtmlView->Navigate2(szUrl);
	}
	else
	{
		CIUIString strUrl = (LPCTSTR)pHtmlCtrlProp->m_strStartPage;
		if (strUrl.GetLength() > 0)
		{
			pHtmlView->Navigate2(strUrl);
		}
	}

	return 0;
}

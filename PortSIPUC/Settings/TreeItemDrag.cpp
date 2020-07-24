#include "stdafx.h"
#include"TreeItemDrag.h"
STDMETHODIMP CTreeItemDataObject::QueryInterface(
	REFIID riid,			// @parm Reference to requested interface ID
	void **ppv)			// @parm out parm for interface ptr
{
	if (!ppv)
	{
		return E_INVALIDARG;
	}

	*ppv = NULL;

	//if(IsZombie())							// Check for range zombie
	//	return CO_E_RELEASED;

	HRESULT		hresult = E_NOINTERFACE;

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = (IUnknown *)this;
		AddRef();
		hresult = NOERROR;
	}
	else if (IsEqualIID(riid, IID_IDataObject))
	{
		*ppv = (IDataObject *)this;
		AddRef();
		hresult = NOERROR;
	}

	return hresult;
}
#define TRACEBEGIN(ss, sc, sz)  

/*
*	CTreeItemDataObject::AddRef()
*
*	@mfunc
*		IUnknown method
*
*	@rdesc
*		ULONG - incremented reference count
*/
STDMETHODIMP_(ULONG) CTreeItemDataObject::AddRef()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::AddRef");

	return ++_crefs;
}

/*
*	CTreeItemDataObject::Release()
*
*	@mfunc
*		IUnknown method
*
*	@rdesc
*		ULONG - decremented reference count
*/
STDMETHODIMP_(ULONG) CTreeItemDataObject::Release()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::Release");

	_crefs--;

	if (_crefs == 0)
	{
		delete this;
		return 0;
	}

	return _crefs;
}

/*
*	CTreeItemDataObject::DAdvise (pFormatetc, advf, pAdvSink, pdwConnection)
*
*	@mfunc
*		establish an advisory connection
*
*	@rdesc
*		HRESULT = OLE_E_ADVISENOTSUPPORTED
*
*	@devnote
*		this is a data transfer object, thus the data is a "snapshot" and
*		cannot change -- no advises are supported.
*/
STDMETHODIMP CTreeItemDataObject::DAdvise(
	FORMATETC *pFormatetc,
	DWORD advf,
	IAdviseSink *pAdvSink,
	DWORD *pdwConnection)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::DAdvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CTreeItemDataObject::DUnadvise (dwConnection)
*
*	@mfunc
*		destroy an advisory connection
*
*	@rdesc
*		HRESULT = OLE_E_ADVISENOTSUPPORTED
*
*	@devnote
*		this is a data transfer object, thus the data is a "snapshot" and
*		cannot change -- no advises are supported.
*/
STDMETHODIMP CTreeItemDataObject::DUnadvise(
	DWORD dwConnection)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::DUnadvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CTreeItemDataObject::EnumDAdvise (ppenumAdvise)
*
*	@mfunc
*		enumerate advisory connections
*
*	@rdesc
*		HRESULT = OLE_E_ADVISENOTSUPPORTED
*
*	@devnote
*		this is a data transfer object, thus the data is a "snapshot" and
*		cannot change -- no advises are supported.
*/
STDMETHODIMP CTreeItemDataObject::EnumDAdvise(
	IEnumSTATDATA **ppenumAdvise)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::EnumDAdvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CTreeItemDataObject::EnumFormatEtc (dwDirection, ppenumFormatEtc)
*
*	@mfunc
*		returns an enumerator which lists all of the available formats in
*		this data transfer object
*
*	@rdesc
*		HRESULT
*
*	@devnote
*		we have no 'set' formats for this object
*/
STDMETHODIMP CTreeItemDataObject::EnumFormatEtc(
	DWORD dwDirection,					// @parm DATADIR_GET/SET
	IEnumFORMATETC **ppenumFormatEtc)	// @parm out parm for enum FETC interface
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::EnumFormatEtc");

	if (!ppenumFormatEtc)
	{
		return E_INVALIDARG;
	}

	*ppenumFormatEtc = NULL;

	//if(IsZombie())							// Check for range zombie
	//	return CO_E_RELEASED;

	HRESULT hr = NOERROR;


#ifdef DEBUG
	if (dwDirection == DATADIR_SET/* && !_ped->Get10Mode()*/)
	{
		//Tracef(TRCSEVNONE, "RichEdit 2.0 EnumFormatEtc called with DATADIR_SET");
	}
#endif

	//Need riched10 compatibility hack to ignore dwDirection
	//if( (dwDirection == DATADIR_GET) || _ped->Get10Mode())
	//{
	//	hr = CEnumFormatEtc::Create(_prgFormats, _cTotal, ppenumFormatEtc);
	//}

	return hr;
}

/*
*	CTreeItemDataObject::GetCanonicalFormatEtc( pformatetc, pformatetcOut)
*
*	@mfunc
*		from the given formatetc, return a more standard (or canonical)
*		format.
*
*	@rdesc
*		HRESULT = E_NOTIMPL
*
*	@devnote
*		(alexgo): we may need to write this routine if we ever do anything
*		snazzy with printers
*/
STDMETHODIMP CTreeItemDataObject::GetCanonicalFormatEtc(
	FORMATETC *pformatetc,
	FORMATETC *pformatetcOut)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::GetCanonicalFormatEtc");


	return E_NOTIMPL;
}

/*
*	CTreeItemDataObject::GetData (pformatetcIn, pmedium)
*
*	@mfunc
*		retrieves data of the specified format
*
*	@rdesc
*		HRESULT
*
*	@devnote
*      @todo (joseogl) delayed rendering for embbeded object data and metafile.
*                      tie in with replace range notification for activate object.
*
*/
// 当Drop后，拖放目标通过本接口请求数据。
STDMETHODIMP CTreeItemDataObject::GetData(
	FORMATETC *pformatetcIn,
	STGMEDIUM *pmedium)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::GetData");

	if (NULL == pformatetcIn || NULL == pmedium)
	{
		return E_FAIL;
	}

	if ((m_etc.cfFormat == pformatetcIn->cfFormat)
		&& m_etc.tymed == pformatetcIn->tymed)
	{
		memset(pmedium, '\0', sizeof(STGMEDIUM));
		pmedium->tymed = TYMED_NULL;

		pmedium->hGlobal = m_hDragItem;

		return S_OK;
	}


	return DV_E_FORMATETC;
}

/*
*	CTreeItemDataObject::GetDataHere (pformatetc, pmedium)
*
*	@mfunc
*		retrieves data of the specified format into the given medium
*
*	@rdesc
*		HRESULT = E_NOTIMPL
*
*	@todo (alexgo): implement this for OLE-type objects
*	@todo (joseogl): MAC work to be done.
*
*	@devnote (alexgo): technically, we're supposed to support transfers
*		into hglobals, but I'd rather not at the moment.
*/
STDMETHODIMP CTreeItemDataObject::GetDataHere(
	FORMATETC *pformatetc,
	STGMEDIUM *pmedium)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::GetDataHere");

	CLIPFORMAT	cf = pformatetc->cfFormat;
	HRESULT		hr = DV_E_FORMATETC;

// 	if ((cf == cf_EMBEDDEDOBJECT ||
// 		cf == cf_EMBEDSOURCE) &&
// 		(pformatetc->tymed & TYMED_ISTORAGE))
// 	{
// 		// For some reason the NT4.0 and Win95 Shell
// 		//          ask for the EMBEDSOURCE format.
// 		_pObjStg = GetDataForEmbeddedObject(_pOleObj, pmedium->pstg);
// 		pmedium->tymed = TYMED_ISTORAGE;
// 		hr = pmedium->pstg != NULL ? NOERROR : hr;
// 		return hr;
// 	}
// 
// 	if (cf == cf_OBJECTDESCRIPTOR &&
// 		(pformatetc->tymed & TYMED_HGLOBAL) &&
// 		_hObjDesc)
// 	{
// 		pmedium->hGlobal = DuplicateHGlobal(_hObjDesc);
// 		pmedium->tymed = TYMED_HGLOBAL;
// 		return NOERROR;
// 	}

	return E_NOTIMPL;
}

// 查询是否支持这种格式
STDMETHODIMP CTreeItemDataObject::QueryGetData(
	FORMATETC *pformatetc)		// @parm FETC to look for
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::QueryGetData");
	IUITRACE(_T("QueryGetData enter\r\n"));

	if (NULL == pformatetc)
	{
		IUITRACE(_T("QueryGetData pformatetc is NULL\r\n"));

		return DV_E_FORMATETC;
	}

// 	IUITRACE(_T("QueryGetData cfFormat is %d, tymed is %d\r\n"), pformatetc->cfFormat, pformatetc->tymed);

	if ((m_etc.cfFormat == pformatetc->cfFormat
		&& m_etc.tymed == pformatetc->tymed))
	{
		IUITRACE(_T("QueryGetData return S_OK\r\n"));

		return S_OK;
	}

	IUITRACE(_T("QueryGetData return DV_E_FORMATETC\r\n"));
	return DV_E_FORMATETC;
}

/*
*	CTreeItemDataObject::SetData (pformatetc, pmedium, fRelease)
*
*	@mfunc
*		allows data to be set into this data object
*
*	@rdesc
*		HRESULT = E_FAIL
*
*	@devnote
*		as we are a data transfer object with a "snapshot" of data,
*		we do not allow it to be replaced
*/
STDMETHODIMP CTreeItemDataObject::SetData(
	FORMATETC *pformatetc,
	STGMEDIUM *pmedium,
	BOOL fRelease)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::SetData");

	if (NULL == pformatetc || NULL == pmedium)
	{
		return E_FAIL;
	}

	if ((pformatetc->cfFormat == m_etc.cfFormat)
		&& (pformatetc->tymed == m_etc.tymed))
	{
		m_hDragItem = (HTREEITEM)pmedium->hGlobal;
		return S_OK;
	}

	return E_FAIL;
}

/*
*	CTreeItemDataObject::Create(ped)
*
*	@mfunc
*		static function to create CTreeItemDataObject. Used to force users
*		not to create this object on the stack, which would break OLE's
*		liveness rules.
*
*	@rdesc
*		new CTreeItemDataObject *
*/
// static
CTreeItemDataObject *CTreeItemDataObject::Create(HWLWND hBindWnd, UINT uClipFormat, HTREEITEM hItem)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::Create");

	CTreeItemDataObject *pdo = new CTreeItemDataObject;
	if (NULL == pdo)
	{
		return NULL;
	}

	pdo->m_hBindWnd = hBindWnd;
	pdo->m_etc.cfFormat = uClipFormat;
	pdo->m_etc.tymed = TYMED_HGLOBAL;
	pdo->m_hDragItem = hItem;

	IUITRACE(_T("CTreeItemDataObject::Create cfFormat is %d, tymed is %d\r\n"), uClipFormat, TYMED_HGLOBAL);

	return pdo;
}

CTreeItemDataObject::CTreeItemDataObject()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::CTreeItemDataObject");

	_crefs = 1;
	m_hBindWnd = NULL;
	m_hDragItem = NULL;
}

/*
*	CTreeItemDataObject::~CTreeItemDataObject
*
*	@mfunc
*		Private destructor
*/
CTreeItemDataObject::~CTreeItemDataObject()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CTreeItemDataObject::~CTreeItemDataObject");
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CCodecTreeDropSource::QueryInterface(REFIID riid, void **ppv)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::QueryInterface");

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = (IUnknown *)this;
	}
	else if (IsEqualIID(riid, IID_IDropSource))
	{
		*ppv = (IDropSource *)this;
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return NOERROR;
}

/*
*	CProtectedDropSource::AddRef
*/
STDMETHODIMP_(ULONG) CCodecTreeDropSource::AddRef()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::AddRef");

	return ++_crefs;
}

/*
*	CProtectedDropSource::Release
*
*	@devnote.  Do not even think about making an outgoing call here.
*			   If you do, be sure make sure all callers use a
*			   SafeReleaseAndNULL (null the pointer before releasing)
*			   technique to avoid re-entrancy problems.
*/
STDMETHODIMP_(ULONG) CCodecTreeDropSource::Release()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::Release");

	_crefs--;

	if (_crefs == 0)
	{
		delete this;
		return 0;
	}

	return _crefs;
}

/*
*	CProtectedDropSource::QueryContinueDrag (fEscapePressed, grfKeyState)
*
*	Purpose:
*		determines whether or not to continue a drag drop operation
*
*	Algorithm:
*		if the escape key has been pressed, cancel
*		if the left mouse button has been release, then attempt to
*			do a drop
*/
STDMETHODIMP CCodecTreeDropSource::QueryContinueDrag(BOOL fEscapePressed,
	DWORD grfKeyState)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::QueryContinueDrag");

	if (fEscapePressed)
	{
		return DRAGDROP_S_CANCEL;
	}
	else if (!(grfKeyState & MK_LBUTTON) && !(grfKeyState & MK_RBUTTON))
	{
		return DRAGDROP_S_DROP;
	}
	else
	{
		return NOERROR;
	}
}

/*
*	CProtectedDropSource::GiveFeedback (dwEffect)
*
*	Purpose:
*		gives feedback during a drag drop operation
*
*	Notes:
*		FUTURE (alexgo): maybe put in some neater feedback effects
*		than the standard OLE stuff??
*/
STDMETHODIMP CCodecTreeDropSource::GiveFeedback(DWORD dwEffect)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::GiveFeedback");

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

/*
*	CProtectedDropSource::CProtectedDropSource
*/
CCodecTreeDropSource::CCodecTreeDropSource()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::CProtectedDropSource");

	_crefs = 1;
}

//
//	CProtectedDropSource PRIVATE methods
//

/*
*	CProtectedDropSource::~CProtectedDropSource
*/
CCodecTreeDropSource::~CCodecTreeDropSource()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::~CProtectedDropSource");

	;
}

// Helper for creating default FORMATETC from cfFormat
LPFORMATETC IUIAPI _IUIFillFormatEtc(
	LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill)
{
	ENSURE(lpFormatEtcFill != NULL);
	if (lpFormatEtc == NULL && cfFormat != 0)
	{
		lpFormatEtc = lpFormatEtcFill;
		lpFormatEtc->cfFormat = cfFormat;
		lpFormatEtc->ptd = NULL;
		lpFormatEtc->dwAspect = DVASPECT_CONTENT;
		lpFormatEtc->lindex = -1;
		lpFormatEtc->tymed = TYMED_HGLOBAL;
	}
	return lpFormatEtc;
}

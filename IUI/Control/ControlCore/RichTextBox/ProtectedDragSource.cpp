#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


FORMATETC IUI::g_rgFETC[] =
{
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },	// CF_RTF
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_ISTORAGE },// EmbObject
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_ISTORAGE },// EmbSource
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },	// ObjDesc
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },	// LnkSource
	{ CF_METAFILEPICT,	NULL, DVASPECT_CONTENT, -1, TYMED_MFPICT },
	{ CF_DIB,			NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
	{ CF_BITMAP,			NULL, DVASPECT_CONTENT, -1, TYMED_GDI },
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }, // RTF with no objs
	{ CF_UNICODETEXT,	NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
	{ CF_TEXT,			NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },	// Filename
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },	// CF_RTFASTEXT
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_ISTORAGE },// Text with objs
	{ 0,					NULL, DVASPECT_CONTENT, -1, TYMED_ISTORAGE },// Richedit
};

//////////////////////////////////////////////////////////////////////////

/*
 *	RegisterFETCs()
 *
 *	@func
 *		Register nonstandard format ETCs.  Called when DLL is loaded
 *
 *	@todo
 *		Register other RTF formats (and add places for them in g_rgFETC[])
 */
void IUI::RegisterFETCs()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "RegisterFETCs");

#ifdef RTF_HASHCACHE
	HashKeyword_Init();			// init the rtf control keyword hash table.
#endif
	g_rgFETC[iRtfFETC].cfFormat
		= RegisterClipboardFormatA("Rich Text Format");

	g_rgFETC[iRtfAsTextFETC].cfFormat
		= RegisterClipboardFormatA("RTF As Text");

	g_rgFETC[iRichEdit].cfFormat
		= RegisterClipboardFormatA("RICHEDIT");

	g_rgFETC[iObtDesc].cfFormat
		= RegisterClipboardFormatA(CF_OBJECTDESCRIPTOR);

	g_rgFETC[iEmbObj].cfFormat
		= RegisterClipboardFormatA(CF_EMBEDDEDOBJECT);

	g_rgFETC[iEmbSrc].cfFormat
		= RegisterClipboardFormatA(CF_EMBEDSOURCE);

	g_rgFETC[iLnkSrc].cfFormat
		= RegisterClipboardFormatA(CF_LINKSOURCE);

	g_rgFETC[iRtfNoObjs].cfFormat
		= RegisterClipboardFormatA("Rich Text Format Without Objects");

	g_rgFETC[iTxtObj].cfFormat
		= RegisterClipboardFormatA("RichEdit Text and Objects");

	g_rgFETC[iFilename].cfFormat
		= RegisterClipboardFormatA(CF_FILENAME);
}


//////////////////////////////////////////////////////////////////////////

/*
*	CProtectedDataObject::QueryInterface (riid, ppv)
*
*	@mfunc
*		 QueryInterface for CProtectedDataObject
*
*	@rdesc
*		HRESULT
*/
STDMETHODIMP CProtectedDataObject::QueryInterface(
	REFIID riid,			// @parm Reference to requested interface ID
	void **ppv)			// @parm out parm for interface ptr
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::QueryInterface");

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

/*
*	CProtectedDataObject::AddRef()
*
*	@mfunc
*		IUnknown method
*
*	@rdesc
*		ULONG - incremented reference count
*/
STDMETHODIMP_(ULONG) CProtectedDataObject::AddRef()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::AddRef");

	return ++_crefs;
}

/*
*	CProtectedDataObject::Release()
*
*	@mfunc
*		IUnknown method
*
*	@rdesc
*		ULONG - decremented reference count
*/
STDMETHODIMP_(ULONG) CProtectedDataObject::Release()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::Release");

	_crefs--;

	if (_crefs == 0)
	{
		delete this;
		return 0;
	}

	return _crefs;
}

/*
*	CProtectedDataObject::DAdvise (pFormatetc, advf, pAdvSink, pdwConnection)
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
STDMETHODIMP CProtectedDataObject::DAdvise(
	FORMATETC *pFormatetc,
	DWORD advf,
	IAdviseSink *pAdvSink,
	DWORD *pdwConnection)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::DAdvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CProtectedDataObject::DUnadvise (dwConnection)
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
STDMETHODIMP CProtectedDataObject::DUnadvise(
	DWORD dwConnection)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::DUnadvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CProtectedDataObject::EnumDAdvise (ppenumAdvise)
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
STDMETHODIMP CProtectedDataObject::EnumDAdvise(
	IEnumSTATDATA **ppenumAdvise)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::EnumDAdvise");

	return OLE_E_ADVISENOTSUPPORTED;
}

/*
*	CProtectedDataObject::EnumFormatEtc (dwDirection, ppenumFormatEtc)
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
STDMETHODIMP CProtectedDataObject::EnumFormatEtc(
	DWORD dwDirection,					// @parm DATADIR_GET/SET
	IEnumFORMATETC **ppenumFormatEtc)	// @parm out parm for enum FETC interface
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::EnumFormatEtc");

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
*	CProtectedDataObject::GetCanonicalFormatEtc( pformatetc, pformatetcOut)
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
STDMETHODIMP CProtectedDataObject::GetCanonicalFormatEtc(
	FORMATETC *pformatetc,
	FORMATETC *pformatetcOut)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::GetCanonicalFormatEtc");


	return E_NOTIMPL;
}

/*
*	CProtectedDataObject::GetData (pformatetcIn, pmedium)
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
STDMETHODIMP CProtectedDataObject::GetData(
	FORMATETC *pformatetcIn,
	STGMEDIUM *pmedium)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::GetData");

	memset(pmedium, '\0', sizeof(STGMEDIUM));
	pmedium->tymed = TYMED_NULL;

	//if(IsZombie())							// Check for range zombie
	//	return CO_E_RELEASED;

	CLIPFORMAT	cf = pformatetcIn->cfFormat;
	HRESULT		hr = DV_E_FORMATETC;

	if ((cf == cf_EMBEDDEDOBJECT ||
			cf == cf_EMBEDSOURCE) &&
		(pformatetcIn->tymed & TYMED_ISTORAGE))
	{
		_pObjStg = GetDataForEmbeddedObject(_pOleObj, pmedium->pstg);
		pmedium->tymed = TYMED_ISTORAGE;
		hr = _pObjStg != NULL ? NOERROR : hr;
		return hr;
	}

	if (cf == cf_OBJECTDESCRIPTOR &&
		(pformatetcIn->tymed & TYMED_HGLOBAL) &&
		_hObjDesc)
	{
		pmedium->hGlobal = DuplicateHGlobal(_hObjDesc);
		pmedium->tymed = TYMED_HGLOBAL;
		return NOERROR;
	}

	if (cf == CF_METAFILEPICT)
	{
		pmedium->hMetaFilePict = OleDuplicateData(_hMFPict, CF_METAFILEPICT, 0);
		pmedium->tymed = TYMED_MFPICT;
		return NOERROR;
	}

	// now handle 'native' richedit formats.

	if (cf && pformatetcIn->tymed & TYMED_HGLOBAL)
	{
		if (cf == CF_UNICODETEXT)
		{
			pmedium->hGlobal = DuplicateHGlobal(TextToHglobal(_hPlainText, tPlain));
		}

		else if (cf == CF_TEXT)
		{
			pmedium->hGlobal = TextHGlobalWtoA(TextToHglobal(_hPlainText, tPlain));
		}

		else if (cf == cf_RTF || cf == cf_RTFASTEXT || cf == cf_RTFNOOBJS)
		{
			pmedium->hGlobal = DuplicateHGlobal(TextToHglobal(_hRtfText, tRtf));
		}

		else
		{
			return DV_E_FORMATETC;
		}

		hr = E_OUTOFMEMORY;							// Default not enuf RAM
		if (pmedium->hGlobal)						// Succeeded
		{
			pmedium->tymed = TYMED_HGLOBAL;
			hr = NOERROR;

#ifdef MACPORT
			// Mac note: pmedium->hGlobal is a Mac Handle - we have decided that Forms/Richedit
			//          internal functions and classes will pass and store WLM HANDLES so
			//          we need to unwrap the HANDLE that phdl points to before storing it
			//          in the pmedium structure.
			if (!UnwrapHandle(pmedium->hGlobal, (Handle *)&pmedium->hGlobal))
			{
				hr = E_OUTOFMEMORY;
			}
#endif
		}
	}

	return hr;
}

/*
*	CProtectedDataObject::GetDataForEmbeddedObject (pformatetc, lpstgdest)
*
*	@mfunc
*		retrieves data for embedded object
*
*	@rdesc
*		LPSTORAGE
*
*/
LPSTORAGE CProtectedDataObject::GetDataForEmbeddedObject(
	LPOLEOBJECT	 pOleObj,
	LPSTORAGE	 lpstgdest)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::GetDataForEmbeddedObject");

	HRESULT			 hr, hr1;
	LPPERSISTSTORAGE pperstg;

	if (_pObjStg != NULL && lpstgdest != NULL)
	{
		// We saved the data previously. Copy it to destination.
		hr = _pObjStg->CopyTo(0, NULL, NULL, lpstgdest);
		if (hr == NOERROR)
		{
			lpstgdest->Commit(STGC_DEFAULT);
			return _pObjStg;
		}
		return NULL;
	}

	if (_pObjStg != NULL && lpstgdest == NULL)
	{
		// We saved the data previously.  Return a reference
		_pObjStg->AddRef();
		return _pObjStg;
	}

	// We don't have a saved copy.  Create One.
	hr = pOleObj->QueryInterface(IID_IPersistStorage, (void **)&pperstg);
	if (hr != NOERROR)
	{
		return NULL;
	}

	if (lpstgdest == NULL)
	{
		// It is null.  We have to create our own.
		LPLOCKBYTES lpLockBytes = NULL;
		hr = CreateILockBytesOnHGlobal(NULL, TRUE, // delete on release
				(LPLOCKBYTES *)&lpLockBytes);
		if (hr != NOERROR)
		{
			pperstg->Release();
			return NULL;
		}
		hr = StgCreateDocfileOnILockBytes(
				lpLockBytes,
				STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
				0,	// reserved
				&lpstgdest
			);
		lpLockBytes->Release();
		if (hr != NOERROR)
		{
			pperstg->Release();
			return NULL;
		}
		_pObjStg = lpstgdest;
	}
	else
	{
		// Force the data to be saved
		_pObjStg = GetDataForEmbeddedObject(_pOleObj, NULL);
		pperstg->Release();
		return GetDataForEmbeddedObject(_pOleObj, lpstgdest);
	}

	// OLE2NOTE: even if OleSave returns an error you should still call
	// SaveCompleted.
	hr = OleSave(pperstg, lpstgdest, FALSE /* fSameAsLoad */);
	hr1 = pperstg->SaveCompleted(NULL);
	if (hr != NOERROR || hr1 != NOERROR)			// Should we use SUCCEED macros ????
	{
		lpstgdest = NULL;
	}
	pperstg->Release();
	return _pObjStg;
}

/*
*	CProtectedDataObject::GetDataorObjectDescriptor (pformatetc, pmedium)
*
*	@mfunc
*		retrieves data for embedded object descriptor
*
*	@rdesc
*		HRESULT
*
*/
HGLOBAL CProtectedDataObject::GetDataForObjectDescriptor(
	LPOLEOBJECT	 pOleObj,
	DWORD		 dwAspect
)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::GetDataForObjectDescriptor");

	POINTL ptl = { 0 };
	SIZEL sizel = { 0 };

	if (_hObjDesc == NULL)
	{
		_hObjDesc = OleGetObjectDescriptorDataFromOleObject(
				pOleObj,
				dwAspect,
				ptl,
				&sizel
			);
	}

	return _hObjDesc;
}

HGLOBAL CProtectedDataObject::UnicodePlainTextFromRange()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::UnicodePlainTextFromRange");

	IMRichTextBox *pIMRe = (IMRichTextBox *)CWLWnd::FromHandle(m_hRe);

	LONG	cpMin, cpMost;
	pIMRe->GetSel(cpMin, cpMost);
	LONG	cch = cpMost - cpMin;
	LONG	cchT = 2 * (cch + 1);
	HGLOBAL	hText;
	TCHAR 	*pText;

	hText = GlobalAlloc((GMEM_SHARE | GMEM_MOVEABLE),	// Allocate 2* in
			cchT * sizeof(TCHAR));			//  case all CRs
	if (!hText)
	{
		return NULL;
	}

	pText = (TCHAR *)GlobalLock(hText);
	if (!pText)
	{
		return NULL;
	}

	if (cch)
	{
		CIUIString strSelText = pIMRe->GetSelText();
		StringCchCopy(pText, cchT, strSelText);
		cch = strSelText.GetLength();
		//AssertSz(cch <= cchT,
		//	"CLightDTEngine::UnicodePlainTextFromRange: got too much text");
	}

	//*(pText + cch) = '\0';
	hText = GlobalReAlloc(hText, 2 * (cch + 1), (GMEM_SHARE | GMEM_MOVEABLE));

	GlobalUnlock(hText);

	return hText;
}

/*
*	CProtectedDataObject::GetDataHere (pformatetc, pmedium)
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
STDMETHODIMP CProtectedDataObject::GetDataHere(
	FORMATETC *pformatetc,
	STGMEDIUM *pmedium)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::GetDataHere");

	CLIPFORMAT	cf = pformatetc->cfFormat;
	HRESULT		hr = DV_E_FORMATETC;

	//if(IsZombie())							// Check for range zombie
	//	return CO_E_RELEASED;

	if ((cf == cf_EMBEDDEDOBJECT ||
			cf == cf_EMBEDSOURCE) &&
		(pformatetc->tymed & TYMED_ISTORAGE))
	{
		// For some reason the NT4.0 and Win95 Shell
		//          ask for the EMBEDSOURCE format.
		_pObjStg = GetDataForEmbeddedObject(_pOleObj, pmedium->pstg);
		pmedium->tymed = TYMED_ISTORAGE;
		hr = pmedium->pstg != NULL ? NOERROR : hr;
		return hr;
	}

	if (cf == cf_OBJECTDESCRIPTOR &&
		(pformatetc->tymed & TYMED_HGLOBAL) &&
		_hObjDesc)
	{
		pmedium->hGlobal = DuplicateHGlobal(_hObjDesc);
		pmedium->tymed = TYMED_HGLOBAL;
		return NOERROR;
	}

	return E_NOTIMPL;
}

/*
*	CProtectedDataObject::QueryGetData (pformatetc)
*
*	@mfunc
*		Queries whether the given format is available in this data object
*
*	@rdesc
*		HRESULT
*/
STDMETHODIMP CProtectedDataObject::QueryGetData(
	FORMATETC *pformatetc)		// @parm FETC to look for
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::QueryGetData");

	//if(IsZombie())							// Check for range zombie
	//	return CO_E_RELEASED;

	DWORD	cFETC = _cTotal;

	while (cFETC--)				// Maybe faster to search from start
	{
		if (pformatetc->cfFormat == _prgFormats[cFETC].cfFormat &&
			(pformatetc->tymed & _prgFormats[cFETC].tymed))
		{
			return NOERROR;
		}
	}

	return DV_E_FORMATETC;
}

/*
*	CProtectedDataObject::SetData (pformatetc, pmedium, fRelease)
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
STDMETHODIMP CProtectedDataObject::SetData(
	FORMATETC *pformatetc,
	STGMEDIUM *pmedium,
	BOOL fRelease)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::SetData");

	return E_FAIL;
}

/*
*	CProtectedDataObject::Create(ped)
*
*	@mfunc
*		static function to create CProtectedDataObject. Used to force users
*		not to create this object on the stack, which would break OLE's
*		liveness rules.
*
*	@rdesc
*		new CProtectedDataObject *
*/
// static
CProtectedDataObject *CProtectedDataObject::Create(HWLWND hIM, LONG lStreamFormat)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::Create");

	CProtectedDataObject *pdo = new CProtectedDataObject;
	pdo->m_hRe = hIM;

	if (!pdo)
	{
		return NULL;
	}

	IMRichTextBox *pIMRe = (IMRichTextBox *)CWLWnd::FromHandle(hIM);

	pdo->_hPlainText = NULL;
	pdo->_hRtfText = NULL;
	pdo->_pOleObj = NULL;
	pdo->_hObjDesc = NULL;
	pdo->_pObjStg = NULL;
	pdo->_hMFPict = NULL;

	LONG cpMin = -1;
	LONG cpMost = -1;
	pIMRe->GetSel(cpMin, cpMost);
	pdo->_cch = cpMost - cpMin;
	pdo->_cpMin = cpMin;

	pdo->_lStreamFormat = lStreamFormat;

	//Set the object count.
	//if( ped->HasObjects() )
	//{
	//	pdo->_cObjs = ped->_pobjmgr->CountObjectsInRange(cr.cpMin, cr.cpMost);
	//}
	//else
	{
		pdo->_cObjs = 0;
	}

	//if( !ped->IsRich() )
	//{
	//	// plain text case

	//	pdo->_cTotal = 2;
	//	pdo->_prgFormats = new FORMATETC[2];

	//	if( !pdo->_prgFormats )
	//	{
	//		goto cleanup;
	//	}

	//	pdo->_prgFormats[0] = g_rgFETC[iAnsiFETC];
	//	pdo->_prgFormats[1] = g_rgFETC[iUnicodeFETC];

	//	return pdo;
	//}
	//else if( ped->HasObjects() && pdo->_cch == 1 )
	//{
	//	if( prg->GetChar(&ch) == NOERROR && ch == WCH_EMBEDDING )
	//	{
	//		COleObject *pobj = ped->_pobjmgr->GetObjectFromCp(pdo->_cpMin);
	//		IUnknown   *punk = pobj->GetIUnknown();

	//		if (punk &&
	//			punk->QueryInterface(IID_IOleObject,
	//			(void **) &pdo->_pOleObj) == NOERROR)
	//		{
	//			// have an OLE object; we can offer all OLE formats
	//			// plus RTF

	//			pdo->_cTotal = 5;

	//			pdo->_prgFormats = new FORMATETC[pdo->_cTotal];

	//			if( !pdo->_prgFormats )
	//			{
	//				goto cleanup;
	//			}

	//			pdo->_prgFormats[0] = g_rgFETC[iEmbObj];	// EmbeddedObject
	//			pdo->_prgFormats[1] = g_rgFETC[iObtDesc];	// ObjectDescriptor
	//			pdo->_prgFormats[2] = g_rgFETC[iMfPict];	// Metafile
	//			pdo->_prgFormats[3] = g_rgFETC[iRtfFETC];	// RTF
	//			pdo->_prgFormats[4] = g_rgFETC[iRtfNoObjs];	// RTF with no objects

	//			// Get the embedde object formats now
	//			pdo->_hObjDesc =
	//				pdo->GetDataForObjectDescriptor( pdo->_pOleObj,
	//				pobj->GetDvaspect() );
	//			pdo->_pObjStg = pdo->GetDataForEmbeddedObject( pdo->_pOleObj, NULL );
	//			pdo->_hMFPict = (HMETAFILE) OleStdGetMetafilePictFromOleObject(
	//				pdo->_pOleObj,
	//				pobj->GetDvaspect(),
	//				NULL,
	//				NULL);
	//			return pdo;
	//		}
	//	}
	//}

	// regular rich text case

	// We only offer up the five formats that we know how to handle in GetData.
	pdo->_cTotal = 5;
	pdo->_prgFormats = new FORMATETC[pdo->_cTotal];

	if (!pdo->_prgFormats)
	{
		goto cleanup;
	}

	pdo->_prgFormats[0] = g_rgFETC[iRtfFETC];		// RTF
	pdo->_prgFormats[1] = g_rgFETC[iRtfNoObjs];		// RTF with no objects
	pdo->_prgFormats[2] = g_rgFETC[iRtfAsTextFETC];	// RTF as Text
	pdo->_prgFormats[3] = g_rgFETC[iAnsiFETC];		// ANSI plain text
	pdo->_prgFormats[4] = g_rgFETC[iUnicodeFETC];	// Unicode plain text

	return pdo;

cleanup:
	pdo->_cTotal = 0;
	pdo->Release();
	return NULL;
}

/*
*	CProtectedDataObject::TextToHglobal(hText)
*
*	@mfunc
*		Instatntiates text on demand for the data object.
*/
HGLOBAL CProtectedDataObject::TextToHglobal(HGLOBAL &hText, TEXTKIND tKind)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::PlainTextToHglobal");

	if (hText == NULL)
	{
		if (tKind == tPlain)
		{
			hText = UnicodePlainTextFromRange();
		}
		else
		{
			//hText = _ped->GetDTE()->RtfFromRange(&rg, _lStreamFormat);
		}
	}

	return hText;
}

//
//	CProtectedDataObject PRIVATE methods
//

/*
*	CProtectedDataObject::CProtectedDataObject()
*
*	@mfunc
*		Private constructor
*/

CProtectedDataObject::CProtectedDataObject()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::CProtectedDataObject");

	_crefs = 1;
	_cTotal = CFETC;
	_prgFormats = g_rgFETC;
	_cch = 0;
	_cObjs = 0;
}

/*
*	CProtectedDataObject::~CProtectedDataObject
*
*	@mfunc
*		Private destructor
*/
CProtectedDataObject::~CProtectedDataObject()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDataObject::~CProtectedDataObject");

	if (_prgFormats && _prgFormats != g_rgFETC)
	{
		delete[] _prgFormats;
	}

	if (_pOleObj)
	{
		_pOleObj->Release();
	}

	if (_pObjStg)
	{
		_pObjStg->Release();
	}

	if (_hMFPict)
	{
		(void)DeleteMetaFile(_hMFPict);
	}
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CProtectedDropSource::QueryInterface(REFIID riid, void **ppv)
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
STDMETHODIMP_(ULONG) CProtectedDropSource::AddRef()
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
STDMETHODIMP_(ULONG) CProtectedDropSource::Release()
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
STDMETHODIMP CProtectedDropSource::QueryContinueDrag(BOOL fEscapePressed,
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
STDMETHODIMP CProtectedDropSource::GiveFeedback(DWORD dwEffect)
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::GiveFeedback");

	return DRAGDROP_S_USEDEFAULTCURSORS;
}

/*
*	CProtectedDropSource::CProtectedDropSource
*/
CProtectedDropSource::CProtectedDropSource()
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
CProtectedDropSource::~CProtectedDropSource()
{
	TRACEBEGIN(TRCSUBSYSDTE, TRCSCOPEINTERN, "CProtectedDropSource::~CProtectedDropSource");

	;
}

#ifndef TREEITEMDRAG_H
#define TREEITEMDRAG_H
// 
class CTreeItemDataObject : public IDataObject
{
public:

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDataObject methods
	STDMETHOD(DAdvise)(FORMATETC *pFormatetc, DWORD advf,
		IAdviseSink *pAdvSink, DWORD *pdwConnection);
	STDMETHOD(DUnadvise)(DWORD dwConnection);
	STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise);
	STDMETHOD(EnumFormatEtc)(DWORD dwDirection,
		IEnumFORMATETC **ppenumFormatEtc);
	STDMETHOD(GetCanonicalFormatEtc)(FORMATETC *pformatetc,
		FORMATETC *pformatetcOut);
	STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium);
	STDMETHOD(GetDataHere)(FORMATETC *pformatetc, STGMEDIUM *pmedium);
	STDMETHOD(QueryGetData)(FORMATETC *pformatetc);
	STDMETHOD(SetData)(FORMATETC *pformatetc, STGMEDIUM *pmedium,
		BOOL fRelease);

	static	CTreeItemDataObject *Create(HWLWND hRe, UINT uClipFormat, HTREEITEM hItem);

private:
	// NOTE: private cons/destructor, may not be allocated on the stack as
	// this would break OLE's current object liveness rules
	CTreeItemDataObject();
	~CTreeItemDataObject();

	ULONG		_crefs;

protected:
	HWLWND m_hBindWnd;

	FORMATETC m_etc; // Tree item drag & drop
	HTREEITEM m_hDragItem;
};

class CCodecTreeDropSource : public IDropSource
{
public:
	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppv);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDropSource methods
	STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHOD(GiveFeedback)(DWORD dwEffect);

	CCodecTreeDropSource();

private:
	// NOTE: private destructor, may not be allocated on the stack as
	// this would break OLE's current object liveness rules
	~CCodecTreeDropSource();

	ULONG		_crefs;
};

LPFORMATETC IUIAPI _IUIFillFormatEtc(
	LPFORMATETC lpFormatEtc, CLIPFORMAT cfFormat, LPFORMATETC lpFormatEtcFill);
#endif
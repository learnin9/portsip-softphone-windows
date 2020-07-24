
#pragma once


typedef struct
{
	__in LPDATAOBJECT lpDataObject;
	__in DWORD dwKeyState;
	__inout DWORD dropEffect;
	__in POINT point;
	__in HWND hWnd;
} DUI_DropInfo;

#define DUI_DROP_MSG                (_T("drop")) ///<用于无窗拖拽
typedef struct
{
	DUI_DropInfo info;
	DWORD_PTR        dwData;
} DUI_DropInfoMsg;

// 这是给HWND准备的IDropTarget，HWLWND不要用
class CHWNDDropTargetEx : public IDropTarget
{
public:
	CHWNDDropTargetEx(HWND hWnd);
	~CHWNDDropTargetEx(void);
public:
	bool DragDropRegister(HWND hWnd, DWORD AcceptKeyState = MK_LBUTTON);
	bool DragDropRevoke(HWND hWnd);
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __out void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE DragEnter(__in IDataObject *pDataObj, DWORD grfKeyState,
		POINTL pt, __inout DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, __out DWORD *pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave();
	HRESULT STDMETHODCALLTYPE Drop(__in IDataObject *pDataObj, DWORD grfKeyState, POINTL pt,
		__inout DWORD *pdwEffect);

private:
	HWND m_hWnd;
	IDropTargetHelper *m_piDropHelper;
	bool    m_bUseDnDHelper;
	DWORD m_dAcceptKeyState;
	ULONG  m_lRefCount;

	// Drag & drop时最后一个命中的控件，主要用来向它发送Leave事件。
	// 只有m_hLastDragHitWnd->m_pDropTarget有效的HWLWND才会被记录。
	HWLWND m_hLastDragHitWnd;
	IDataObject *m_pDataObject;
	BOOL m_bDraging;
};
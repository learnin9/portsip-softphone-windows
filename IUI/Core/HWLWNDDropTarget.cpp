#include "StdAfx.h"
#include <Shlwapi.h>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


CHWLWNDDropTarget::CHWLWNDDropTarget(HWLWND hWnd)
	: m_hWnd(hWnd)
	, m_lRefCount(1)
{
}


CHWLWNDDropTarget::~CHWLWNDDropTarget(void)
{
}

HRESULT STDMETHODCALLTYPE CHWLWNDDropTarget::QueryInterface(REFIID riid,
	__RPC__deref_out void **ppvObject)
{
	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObject = (IUnknown *)this;
	}
	else if (IsEqualIID(riid, IID_IDropTarget))
	{
		*ppvObject = (IDropTarget *)this;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	AddRef();

	return NOERROR;
}

ULONG STDMETHODCALLTYPE CHWLWNDDropTarget::AddRef()
{
	return InterlockedIncrement((volatile LONG *)&m_lRefCount);
}

ULONG STDMETHODCALLTYPE CHWLWNDDropTarget::Release()
{
	ULONG lRef = InterlockedDecrement((volatile LONG *)&m_lRefCount);

	if (0 == lRef)
	{
		CHWLWNDDropTarget *p = this;
		delete p;
		p = NULL;
		return 0;
	}

	return lRef;
}

// pt: 屏幕坐标
HRESULT STDMETHODCALLTYPE CHWLWNDDropTarget::DragEnter(__RPC__in_opt IDataObject *pDataObject,
	DWORD dwKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
{
	if (pDataObject == NULL || pdwEffect == NULL)
	{
		OutputDebugString(TEXT("pDataObject == NULL || pdwEffect == NULL"));
		return E_INVALIDARG;
	}

	// 发送消息给目标窗口，目标窗口返回是否对这个拖放的数据感兴趣。 即:是否接收拖放，如果接收拖放，
	// 通过返回pdwEffect，告诉DoDragDrop，是移动、复制或是链接。
	DropInfo info = { 0 };
	info.dropEffect = *pdwEffect;
	info.dwKeyState = 0;
	info.lpDataObject = pDataObject;
	info.point.x = pt.x;
	info.point.y = pt.y;
	info.hWnd = m_hWnd;
	RoutedEventArgs args;
	args.hOriginalSender = args.hSender = m_hWnd;
	args.eRoutingStrategy = RS_BUBBLE;
	args.RoutedEvent = DDM_OLEDROPTARGET;
	args.wParam = DDT_DRAGENTER;
	args.lParam = (LPARAM)&info;
	LRESULT lr = RaiseEvent(m_hWnd, &args);
	*pdwEffect = info.dropEffect;
	if (0 != lr)
	{
		return S_OK;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHWLWNDDropTarget::DragOver(DWORD grfKeyState, POINTL pt,
	__RPC__inout DWORD *pdwEffect)
{
	if (pdwEffect == NULL)
	{
		OutputDebugString(TEXT("pdwEffect == NULL"));
		return E_INVALIDARG;
	}

	DropInfo info = { 0 };
	info.dropEffect = *pdwEffect;
	info.dwKeyState = 0;
	info.lpDataObject = NULL;
	info.point.x = pt.x;
	info.point.y = pt.y;
	info.hWnd = m_hWnd;

	RoutedEventArgs args;
	args.hOriginalSender = args.hSender = m_hWnd;
	args.eRoutingStrategy = RS_BUBBLE;
	args.RoutedEvent = DDM_OLEDROPTARGET;
	args.wParam = DDT_DRAGOVER;
	args.lParam = (LPARAM)&info;
	LRESULT lr = RaiseEvent(m_hWnd, &args);
	*pdwEffect = info.dropEffect;
	if (0 != lr)
	{
		return S_OK;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHWLWNDDropTarget::DragLeave()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHWLWNDDropTarget::Drop(__RPC__in_opt IDataObject *pDataObject,
	DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
{
	if (pDataObject == NULL || pdwEffect == NULL)
	{
		OutputDebugString(TEXT("pDataObject == NULL || pdwEffect == NULL"));
		return E_INVALIDARG;
	}

	DropInfo info = { 0 };
	info.dropEffect = *pdwEffect;
	info.dwKeyState = 0;
	info.lpDataObject = pDataObject;
	info.point.x = pt.x;
	info.point.y = pt.y;
	info.hWnd = m_hWnd;
	RoutedEventArgs args;
	args.hOriginalSender = args.hSender = m_hWnd;
	args.eRoutingStrategy = RS_BUBBLE;
	args.RoutedEvent = DDM_OLEDROPTARGET;
	args.wParam = DDT_DROP;
	args.lParam = (LPARAM)&info;
	LRESULT lr = RaiseEvent(m_hWnd, &args);
	*pdwEffect = info.dropEffect;
	if (0 != lr)
	{
		return S_OK;
	}

	return S_OK;
}

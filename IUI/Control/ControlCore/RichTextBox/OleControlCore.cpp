#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

// {F8A25CD9-7C66-4daf-947A-02E0089C6C57}
static const IID IUI::IID_IUIOleControl =
{ 0xf8a25cd9, 0x7c66, 0x4daf, { 0x94, 0x7a, 0x2, 0xe0, 0x8, 0x9c, 0x6c, 0x57 } };

//////////////////////////////////////////////////////////////////////////

CRichMsgItem::CRichMsgItem()
	: m_bOleControl(FALSE)
	, m_hOleBindWindowless(NULL)
	, m_crText(RGB(0, 0, 0))
	, m_lParam(0)
{
}

CRichMsgItem::~CRichMsgItem()
{

}

// OleControl作为OLE子控件的容器。
// OleControl可以加载界面编辑器中的窗口，加载不同的窗口，实现不同的OLE
// 例如，在界面编辑器中创建一个窗口，窗口里创建一个图片控件。 OleControl加载这个窗口后，就变成一个Image Ole
// 在界面编辑器中创建一个窗口，里面创建一个视频控件，OleControl加载后，就变成一个视频OLE
// OleControl只需要提供以下三大类功能即可：
// 1. OleControl与其子控件的交互
// 2. 让其子控件与RichEdit交互
// 3. 绘制子控件
// IUI仅内置制作IM软件最基础的图片OLE。 其它诸如文件、名片OLE，由用户从OleControl派生自己实现
OleControl::OleControl(UINT uID, HWLWND hParent)
{
	IDD = uID;
	m_pOleClientSite = NULL;
	m_lParam = 0;
	m_bHideBorder = FALSE;
	m_hParent = hParent;
	_ASSERT(NULL != m_hParent);
}

OleControl::~OleControl()
{

}

// IUnknown抽象类接口
HRESULT OleControl::QueryInterface(
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (this == NULL)
	{
		return S_FALSE;
	}

	// 必须把this指针类型转换为相应的指针。 否则Ole无法插入到RichEdit中。
	if (riid == IID_IViewObject)
	{
		*ppvObject = (IViewObject *)this;
	}
	else if (riid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
	}
	else if (riid == IID_IUnknown || riid == IID_IOleObject)
	{
		*ppvObject = (IOleObject *)this;
	}
	else if (riid == IID_IUIOleControl)
	{
		*ppvObject = this;
	}
	else
	{
		return E_NOINTERFACE;
	}

	// 加不加都行，反正一个Ole对应一个C++对象
	AddRef();

	return S_OK;
}

ULONG OleControl::AddRef(void)
{
	return S_OK;
}

ULONG OleControl::Release(void)
{
	return S_OK;
}

// 由于CRichEditCtrlBase::InsertOle在InsertObject之后，会调用SetClientSite
// 所以可以认为，当m_pOleClientSite的值有效时，就表示OLE已被插入到了RichEdit中。
HRESULT OleControl::SetClientSite(/* [unique][in] */ IOleClientSite *pClientSite)
{
	m_pOleClientSite = pClientSite;

	if (m_pOleClientSite == NULL)
	{
		return S_FALSE;
	}

	return S_OK;
}

HRESULT OleControl::GetClientSite(/* [out] */IOleClientSite **ppClientSite)
{
	if (ppClientSite == NULL)
	{
		return E_INVALIDARG;
	}

	*ppClientSite = m_pOleClientSite;

	return S_OK;
}

HRESULT OleControl::GetUserClassID(/* [out] */ CLSID *pClsid)
{
	*pClsid = GUID_NULL;

	return S_OK;
}

HRESULT OleControl::SetExtent(/* [in] */ DWORD dwDrawAspect, /* [in] */ SIZEL *psizel)
{
	m_sizeExtent = *psizel;
	return S_OK;
}

HRESULT OleControl::GetExtent(/* [in] */ DWORD dwDrawAspect, /* [out] */ SIZEL *psizel)
{
	if (psizel == NULL)
	{
		return E_INVALIDARG;
	}

	*psizel = m_sizeExtent;

	return S_OK;
}

HRESULT OleControl::Advise(/* [unique][in] */ IAdviseSink *pAdvSink, /* [out] */ DWORD *pdwConnection)
{

	return S_OK;
}

HRESULT OleControl::Unadvise(
	/* [in] */ DWORD dwConnection)
{

	return S_OK;
}

// IViewObject2 需要实现的方法
HRESULT OleControl::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
	return S_OK;
}

HRESULT OleControl::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return S_OK;
}

HRESULT OleControl::Draw(
	DWORD dwDrawAspect,
	LONG lindex,
	void *pvAspect,
	DVTARGETDEVICE *ptd,
	HDC hdcTargetDev,
	HDC hdcDraw,
	LPCRECTL lprcBounds,
	LPCRECTL lprcWBounds,
	BOOL(STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue),
	ULONG_PTR dwContinue)
{
	HDC hDC = hdcDraw;

	// lprcBounds的坐标，是相对于RichEdit控件的。
	m_rect = (RECT *)lprcBounds;

	// 绘制自己
	if (IDD == -1)
	{
		return S_FALSE;
	}

	// 绘制绑定的控件
	if (NULL != m_hBindWindowless)
	{
		CIUIRect rcOld;
		GetWindowRectToParent(m_hBindWindowless, rcOld);

		if (rcOld != m_rect)
		{
			MoveWindow(m_hBindWindowless, &m_rect, FALSE);
		}

		// 备份视口、裁剪区域
		POINT ptOrg;
		GetViewportOrgEx(hDC, &ptOrg);

		HRGN hOldRgn = CreateRectRgn(0, 0, 0, 0);
		GetClipRgn(hDC, hOldRgn);

		// 绘制绑定的控件
		DrawControlAndChildren(m_hBindWindowless, hDC, &m_rect);

		// 恢复视口：
		// 注意：IUI默认绘制机制是先绘制父控件，等父控件整个绘制完后，再绘制子控件。
		// 虽然子控件绘制时，会修改HDC的视口，但这时父控件已彻底绘制完了，所以子控件
		// 修改HDC视口，也不会影响父控件的绘制。
		// 但由于ListView的特殊性，ListView是在绘制自己的时候，同时把Item绑定的控件
		// 进行了绘制，而在绘制完Item绑定的控件后，HDC的视口已不是ListView的视口了，
		// 而这时，ListView还未绘制完，这将导致ListView之后的绘制位置全部错误。
		// 同理，裁剪区域也需要恢复
		SetViewportOrgEx(hDC, ptOrg.x, ptOrg.y, NULL);
		SelectClipRgn(hDC, hOldRgn);
		DeleteObject(hOldRgn);
		hOldRgn = NULL;

	}

	// 通过反色，达到不绘制OLE边框的效果
	// 只有OLE控件被单选时，才绘制OLE边框
	BOOL bHideBorder = m_bHideBorder;
	if (NULL != m_hBindWindowless)
	{
		RoutedEventArgs args;
		args.eRoutingStrategy = RS_BUBBLE;
		args.hOriginalSender = m_hBindWindowless;
		args.hSender = m_hBindWindowless;
		args.RoutedEvent = UE_ISHIDEOLEBORDER;
		RaiseEvent(m_hBindWindowless, &args);
		bHideBorder = args.lResult;
	}

	if (bHideBorder)
	{
		RichTextBox *pRe = (RichTextBox *)CWLWnd::FromHandle(m_hParent);
		_ASSERT(NULL != pRe);
		LONG nSelMinCp = -1;
		LONG nSelMostCp = -1;
		pRe->GetSel(nSelMinCp, nSelMostCp);
		if (nSelMinCp >= 0 && nSelMinCp + 1 == nSelMostCp) // 单选
		{
			REOBJECT reo;
			reo.cbStruct = sizeof(REOBJECT);
			int nOleIndex = FindObjectNearCp(pRe, nSelMinCp, &reo);
			// 自己被单选
			if (reo.cp == nSelMinCp
				&& reo.poleobj == this
				&& (reo.dwFlags & REO_SELECTED))
			{
				CIUIRect rcLeft = m_rect;
				rcLeft.right = rcLeft.left + 1;
				InvertRect(hDC, rcLeft);
				CIUIRect rcTop = m_rect;
				rcTop.bottom = rcTop.top + 1;
				rcTop.left += 1;
				InvertRect(hDC, rcTop);
				CIUIRect rcRight = m_rect;
				rcRight.left = rcRight.right - 1;
				rcRight.top += 1;
				InvertRect(hDC, rcRight);
				CIUIRect rcBottom = m_rect;
				rcBottom.DeflateRect(1, 1, 1, 0);
				rcBottom.top = rcBottom.bottom - 1;
				InvertRect(hDC, rcBottom);
			}
		}
	}

	return S_OK;
}

HRESULT OleControl::GetExtent(
	DWORD dwDrawAspect,
	LONG lindex,
	DVTARGETDEVICE *ptd,
	LPSIZEL lpsizel)
{
	return S_OK;
}

HWLWND OleControl::SetOleBindWindowless(HWLWND hNewBind)
{
	HWLWND hOld = m_hBindWindowless;

	m_hBindWindowless = hNewBind;
	// 绑定到OLE后的控件，从HWLND子控件体系中移除，加入到HWLWND绑定子控件体系中。
	SetBindItemControlParent(m_hBindWindowless, m_hParent, (ULONG_PTR)this);

	IUI::SetProp(m_hBindWindowless, _T("GETPROP_BINDOLE"), this);

	return hOld;
}

HWLWND OleControl::GetOleBindWindowless()
{
	return m_hBindWindowless;
}

LPARAM OleControl::SetUserData(LPARAM lParam)
{
	LPARAM lOld = m_lParam;
	m_lParam = lParam;
	return lOld;
}

LPARAM OleControl::GetUserData() const
{
	return m_lParam;
}

BOOL OleControl::SetHideBorder(BOOL bHide)
{
	BOOL bOld = m_bHideBorder;
	m_bHideBorder = bHide;
	return bOld;
}

BOOL OleControl::IsHideBorder() const
{
	return m_bHideBorder;
}

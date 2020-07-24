#include "stdafx.h"

#ifdef _DEBUG
#define new IUI_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImageOleCtrl::CImageOleCtrl(HWLWND hRichEdit)
	: OleControl(-1, hRichEdit)
{
	m_hIUIImage = NULL;
}

CImageOleCtrl::~CImageOleCtrl()
{
	ReleaseObject();
}

int CImageOleCtrl::ReleaseObject()
{
	ReleaseIUIImage(m_hIUIImage);
	m_hIUIImage = NULL;

	return 0;
}

int CImageOleCtrl::SetImage(LPCTSTR lpszImagePath)
{
	m_hIUIImage = CreateIUIImage(GetProject(m_hBindWindowless), lpszImagePath);

	return 0;
}

int CImageOleCtrl::SetImage(HBITMAP hBitmap)
{
	return 0;
}

HIUIIMAGE CImageOleCtrl::GetImage() const
{
	return m_hIUIImage;
}


HRESULT CImageOleCtrl::SetClientSite(
	/* [unique][in] */ IOleClientSite *pClientSite)
{
	OleControl::SetClientSite(pClientSite);

	if (m_pOleClientSite != NULL)
	{
		// 如果加载到的图片是Gif格式，向宿主窗口发消息，通知宿主窗口，需要起动定时器
		if (GetParent(m_hBindWindowless) != NULL && m_hIUIImage != NULL)
		{
			if (m_hIUIImage->IsGif() && m_hIUIImage->GetGifFrameCount() > 1)
			{
				SendMessage(GetParent(m_hBindWindowless), REM_ADD_GIF_OLE, 0, (LPARAM)this);
			}
		}
	}

	return S_OK;
}

HRESULT CImageOleCtrl::Draw(
	DWORD dwDrawAspect, LONG lindex,
	void *pvAspect,
	DVTARGETDEVICE *ptd,
	HDC hdcTargetDev,
	HDC hdcDraw,
	LPCRECTL lprcBounds,
	LPCRECTL lprcWBounds,
	BOOL(STDMETHODCALLTYPE *pfnContinue)(ULONG_PTR dwContinue),
	ULONG_PTR dwContinue)
{
	OleControl::Draw(dwDrawAspect, lindex, pvAspect, ptd,
		hdcTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue, dwContinue);

	CIUIRect rc = (RECT *)lprcBounds;
	// Ole边框
	rc.DeflateRect(1, 1, 1, 1);

	m_hIUIImage->SafeLoadSavedImage();
	if (m_hIUIImage->GetSafeHBITMAP() != NULL)
	{
		int nImageWidth = m_hIUIImage->GetWidth();
		int nImageHeight = m_hIUIImage->GetHeight();

		CIUIRect rcSrc(0, 0, nImageWidth, nImageHeight);

		if (m_hIUIImage->IsGif())
		{
			int nCurFrame = m_hIUIImage->GetGifCurFrame();
			rcSrc.left = nCurFrame * nImageWidth;
			rcSrc.right = rcSrc.left + nImageWidth;

			nCurFrame++;
			if (nCurFrame >= (int)m_hIUIImage->GetGifFrameCount())
			{
				nCurFrame = 0;
			}
			m_hIUIImage->SetGifCurFrame(nCurFrame);
		}

		IUIPartStretchBlt(hdcDraw, rc, m_hIUIImage, rcSrc, IRM_STRETCH);
	}

	return S_OK;
}

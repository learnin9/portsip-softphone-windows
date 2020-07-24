// 基础绘制引擎
#include "stdafx.h"
#include <MATH.H>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


Gdiplus::PointF Render::Point2PointF(POINT pt)
{
	Gdiplus::PointF ptf((Gdiplus::REAL)pt.x, (Gdiplus::REAL)pt.y);
	return ptf;
}

POINT Render::PointF2Point(Gdiplus::PointF pt)
{
	POINT ptRet = { (LONG)pt.X, (LONG)pt.Y };
	return ptRet;
}

Gdiplus::RectF Render::Rect2RectF(LPCRECT lprc)
{
	if (lprc == NULL)
	{
		return Gdiplus::RectF();
	}

	Gdiplus::RectF r;
	r.X = (Gdiplus::REAL)lprc->left;
	r.Y = (Gdiplus::REAL)lprc->top;
	r.Width = (Gdiplus::REAL)RECTWIDTH(*lprc);
	r.Height = (Gdiplus::REAL)RECTHEIGHT(*lprc);

	return r;
}

RECT Render::RectF2Rect(const Gdiplus::RectF *prc)
{
	RECT r = { 0 };
	if (prc == NULL)
	{
		return r;
	}

	r.left = (LONG)prc->X;
	r.top = (LONG)prc->Y;
	r.right = r.left + (LONG)prc->Width;
	r.bottom = r.top + (LONG)prc->Height;

	return r;
}

CFlattenPathPoint::CFlattenPathPoint()
	: m_nLengthToStart(0)
	, m_pptF(new Gdiplus::PointF)
{
}

CFlattenPathPoint::CFlattenPathPoint(const CFlattenPathPoint &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenPathPoint) == 12);
#else
	_ASSERT(sizeof(CFlattenPathPoint) == 8);
#endif
	if (this == &rSrc)
	{
		return;
	}

	if (m_pptF != NULL)
	{
		delete m_pptF;
		m_pptF = NULL;
	}

	m_pptF = new Gdiplus::PointF;
	*m_pptF = *rSrc.m_pptF;

	m_nLengthToStart = rSrc.m_nLengthToStart;
}

CFlattenPathPoint::~CFlattenPathPoint()
{
	delete m_pptF;
	m_pptF = NULL;
}

CFlattenPathPoint &CFlattenPathPoint::operator=(const CFlattenPathPoint &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenPathPoint) == 12);
#else
	_ASSERT(sizeof(CFlattenPathPoint) == 8);
#endif

	if (this == &rSrc)
	{
		return *this;
	}

	if (m_pptF != NULL)
	{
		delete m_pptF;
		m_pptF = NULL;
	}

	m_pptF = new Gdiplus::PointF;
	*m_pptF = *rSrc.m_pptF;

	m_nLengthToStart = rSrc.m_nLengthToStart;

	return *this;
}

int CFlattenPathPoint::SetPoint(Gdiplus::PointF Pt)
{
	*m_pptF = Pt;

	return 0;
}

Gdiplus::PointF CFlattenPathPoint::GetPoint() const
{
	return *m_pptF;
}

int CFlattenPathPoint::SetLengthToStart(Gdiplus::REAL nLength)
{
	m_nLengthToStart = nLength;

	return 0;
}

Gdiplus::REAL CFlattenPathPoint::GetLengthToStart() const
{
	return m_nLengthToStart;
}

//////////////////////////////////////////////////////////////////////////

CFlattenAnimationSubPath::CFlattenAnimationSubPath()
	: m_pvpPoints(new std::vector<CFlattenPathPoint *>)
	, m_bArc(false)
	, m_prcArcBoundsF(new Gdiplus::RectF)
	, m_nStartAngle(0)
	, m_nSweepAngle(0)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 36);
#else
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 24);
#endif
}

CFlattenAnimationSubPath::CFlattenAnimationSubPath(const CFlattenAnimationSubPath &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 36);
#else
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 24);
#endif
	if (this == &rSrc)
	{
		return;
	}

	if (m_pvpPoints != NULL)
	{
		delete m_pvpPoints;
		m_pvpPoints = NULL;
	}
	m_pvpPoints = new std::vector < CFlattenPathPoint * > ;
	m_pvpPoints->insert(m_pvpPoints->begin(), rSrc.m_pvpPoints->begin(), rSrc.m_pvpPoints->end());

	m_bArc = rSrc.m_bArc;

	if (m_prcArcBoundsF != NULL)
	{
		delete m_prcArcBoundsF;
		m_prcArcBoundsF = NULL;
	}
	m_prcArcBoundsF = new Gdiplus::RectF;
	*m_prcArcBoundsF = *rSrc.m_prcArcBoundsF;

	m_nStartAngle = rSrc.m_nStartAngle;
	m_nSweepAngle = rSrc.m_nSweepAngle;
}

CFlattenAnimationSubPath::~CFlattenAnimationSubPath()
{
	delete m_pvpPoints;
	m_pvpPoints = NULL;

	delete m_prcArcBoundsF;
	m_prcArcBoundsF = NULL;
}

CFlattenAnimationSubPath &CFlattenAnimationSubPath::operator =(const CFlattenAnimationSubPath &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 36);
#else
	_ASSERT(sizeof(CFlattenAnimationSubPath) == 24);
#endif
	if (this == &rSrc)
	{
		return *this;
	}

	if (m_pvpPoints != NULL)
	{
		delete m_pvpPoints;
		m_pvpPoints = NULL;
	}
	m_pvpPoints = new std::vector < CFlattenPathPoint * > ;
	m_pvpPoints->insert(m_pvpPoints->begin(), rSrc.m_pvpPoints->begin(), rSrc.m_pvpPoints->end());

	m_bArc = rSrc.m_bArc;

	if (m_prcArcBoundsF != NULL)
	{
		delete m_prcArcBoundsF;
		m_prcArcBoundsF = NULL;
	}
	m_prcArcBoundsF = new Gdiplus::RectF;
	*m_prcArcBoundsF = *rSrc.m_prcArcBoundsF;

	m_nStartAngle = rSrc.m_nStartAngle;
	m_nSweepAngle = rSrc.m_nSweepAngle;

	return *this;
}

Gdiplus::REAL CFlattenAnimationSubPath::SetPath(Gdiplus::GraphicsPath *pPath)
{
	if (pPath == NULL)
	{
		return -1;
	}

	Gdiplus::GraphicsPath *pFlattenPath = pPath->Clone();
	if (pFlattenPath == NULL)
	{
		return -2;
	}

	Gdiplus::GraphicsPathIterator it(pFlattenPath);
	int nSubPathCount = it.GetSubpathCount();
	if (nSubPathCount != 1)
	{
		_ASSERT(FALSE);
		return -3;
	}

	// 遍平化（把曲线路径转成多段连续的直线段）路径
	pFlattenPath->Flatten(NULL, 8);

	// 从扁平化后的路径中，提取端点
	Gdiplus::PathData subPathData;
	pFlattenPath->GetPathData(&subPathData);

	// 注意：这些点仍然在曲线上
	Gdiplus::REAL nTotalLength = 0;

	for (INT j = 0; j < subPathData.Count; ++j)
	{
		CFlattenPathPoint *pPoint = new CFlattenPathPoint;
		Gdiplus::PointF pt = subPathData.Points[j];
		pPoint->SetPoint(pt);

		if (j == 0)
		{
			pPoint->SetLengthToStart(0);
		}
		else
		{
			// 得到上一个点
			CFlattenPathPoint *pPreviousPoint = (*m_pvpPoints)[j - 1];

			// 通过三角函数计算两个点的直线距离
			Gdiplus::REAL nWidth = pt.X - pPreviousPoint->m_pptF->X;
			Gdiplus::REAL nHeight = pt.Y - pPreviousPoint->m_pptF->Y;
			Gdiplus::REAL nLength = sqrt(nWidth * nWidth + nHeight * nHeight);

			nTotalLength = pPreviousPoint->GetLengthToStart() + nLength;

			pPoint->SetLengthToStart(nTotalLength);
		}

		m_pvpPoints->push_back(pPoint);
	}

	return nTotalLength;
}

Gdiplus::REAL CFlattenAnimationSubPath::GetLength() const
{
	if (m_pvpPoints->empty())
	{
		return 0;
	}

	return m_pvpPoints->back()->m_nLengthToStart;
}

//////////////////////////////////////////////////////////////////////////

CFlattenAnimationPath::CFlattenAnimationPath()
	: m_pvpPaths(new std::vector<CFlattenAnimationSubPath *>)
	, m_dwDuration(1000)
	, m_nTotalLength(0)
{
	for (int i = 0; i < 4; ++i)
	{
		m_pPtFBezier[i] = new Gdiplus::PointF;
	}

#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationPath) == 56);
#else
	_ASSERT(sizeof(CFlattenAnimationPath) == 32);
#endif
}

CFlattenAnimationPath::CFlattenAnimationPath(const CFlattenAnimationPath &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationPath) == 56);
#else
	_ASSERT(sizeof(CFlattenAnimationPath) == 32);
#endif
	if (this == &rSrc)
	{
		return;
	}

	if (m_pvpPaths != NULL)
	{
		delete m_pvpPaths;
		m_pvpPaths = NULL;
	}
	m_pvpPaths = new std::vector < CFlattenAnimationSubPath * > ;
	m_pvpPaths->insert(m_pvpPaths->begin(), rSrc.m_pvpPaths->begin(), rSrc.m_pvpPaths->end());

	m_dwDuration = rSrc.m_dwDuration;
	m_nTotalLength = rSrc.m_nTotalLength;

	for (int i = 0; i < 4; ++i)
	{
		if (m_pPtFBezier[i] != NULL)
		{
			delete m_pPtFBezier[i];
			m_pPtFBezier[i] = NULL;
		}
		m_pPtFBezier[i] = new Gdiplus::PointF;

		*(m_pPtFBezier[i]) = *(rSrc.m_pPtFBezier[i]);
	}
}

CFlattenAnimationPath::~CFlattenAnimationPath()
{
	delete m_pvpPaths;
	m_pvpPaths = NULL;

	for (int i = 0; i < 4; ++i)
	{
		delete m_pPtFBezier[i];
		m_pPtFBezier[i] = NULL;
	}
}

CFlattenAnimationPath &CFlattenAnimationPath::operator =(const CFlattenAnimationPath &rSrc)
{
#ifdef _WIN64
	_ASSERT(sizeof(CFlattenAnimationPath) == 56);
#else
	_ASSERT(sizeof(CFlattenAnimationPath) == 32);
#endif
	if (this == &rSrc)
	{
		return *this;
	}

	if (m_pvpPaths != NULL)
	{
		delete m_pvpPaths;
		m_pvpPaths = NULL;
	}
	m_pvpPaths = new std::vector < CFlattenAnimationSubPath * > ;
	m_pvpPaths->insert(m_pvpPaths->begin(), rSrc.m_pvpPaths->begin(), rSrc.m_pvpPaths->end());

	m_dwDuration = rSrc.m_dwDuration;
	m_nTotalLength = rSrc.m_nTotalLength;

	for (int i = 0; i < 4; ++i)
	{
		if (m_pPtFBezier[i] != NULL)
		{
			delete m_pPtFBezier[i];
			m_pPtFBezier[i] = NULL;
		}
		m_pPtFBezier[i] = new Gdiplus::PointF;

		*(m_pPtFBezier[i]) = *(rSrc.m_pPtFBezier[i]);
	}

	return *this;
}

int CFlattenAnimationPath::AddPath(Gdiplus::GraphicsPath *pPath)
{
	if (pPath == NULL)
	{
		return -1;
	}

	// Path由多个子Path组成
	Gdiplus::GraphicsPathIterator it(pPath);
	int nSubPathCount = it.GetSubpathCount();
	for (int i = 0; i < nSubPathCount; ++i)
	{
		Gdiplus::GraphicsPath pathNext;
		BOOL bClosed = FALSE;
		it.NextSubpath(&pathNext, &bClosed);

		CFlattenAnimationSubPath *pSubPath = new CFlattenAnimationSubPath;
		Gdiplus::REAL nLength = pSubPath->SetPath(&pathNext);

		m_nTotalLength += nLength;

		m_pvpPaths->push_back(pSubPath);
	}

	return 0;
}

int CFlattenAnimationPath::AddArcPath(Gdiplus::GraphicsPath *pPath,
	Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL width, Gdiplus::REAL height,
	Gdiplus::REAL startAngle, Gdiplus::REAL sweepAngle)
{
	int nRet = AddPath(pPath);
	if (nRet != 0)
	{
		return nRet;
	}

	if (m_pvpPaths->empty())
	{
		return -1;
	}

	CFlattenAnimationSubPath *pLastPath = m_pvpPaths->back();
	pLastPath->m_bArc = true;
	pLastPath->m_prcArcBoundsF->X = x;
	pLastPath->m_prcArcBoundsF->Y = y;
	pLastPath->m_prcArcBoundsF->Width = width;
	pLastPath->m_prcArcBoundsF->Height = height;
	pLastPath->m_nStartAngle = startAngle;
	pLastPath->m_nSweepAngle = sweepAngle;

	return 0;
}

int CFlattenAnimationPath::SetDuration(DWORD dwDuration)
{
	m_dwDuration = dwDuration;
	return 0;
}

DWORD CFlattenAnimationPath::GetDuration() const
{
	return m_dwDuration;
}

// 得到指定时刻，帧的坐标，这个坐标，可能是插值出来的
int CFlattenAnimationPath::GetPos(DWORD t, CFlattenPathPoint *pOutPoint)
{
	if (m_dwDuration <= 0 || t > m_dwDuration || pOutPoint == NULL)
	{
		return -1;
	}

	// 下面是不考虑使用Bezier曲线调整速度的计算方法

	// 计算时间t走过的路长
	Gdiplus::REAL fPassPath = t * m_nTotalLength / m_dwDuration;
	if (t == m_dwDuration)
	{
		// 对于浮点数来说，一个浮点值乘以A再除以A后，得到的值不一定等于原值。
		// 所以，对于终点值来说，特殊处理一下。
		fPassPath = m_nTotalLength;
	}

	// 计算时间点在哪条曲线上
	Gdiplus::REAL nPass = 0;
	std::vector<CFlattenAnimationSubPath *>::iterator it = m_pvpPaths->begin();
	for (; it != m_pvpPaths->end(); ++it)
	{
		CFlattenAnimationSubPath *pSubPath = *it;

		nPass += pSubPath->GetLength();
		if (nPass >= fPassPath)
		{
			nPass -= pSubPath->GetLength(); // 本SubPath起点位置
			break;
		}
	}

	if (it == m_pvpPaths->end())
	{
		_ASSERT(FALSE);
		return -2;
	}

	// 采用二分查找法，找到时间点在哪条线段上
	CFlattenAnimationSubPath *pSubPath = *it;

	// 如果子路径是正圆，采用圆的函数或坐标系旋转的方法以提高精度
	if (pSubPath->m_bArc)
	{
		if (pSubPath->GetLength() > 0)
		{
			// 在圆内扫过的角度，全部计算一定要使用浮点数，否则误差会很大。
			// 比如扇形的总角度是20度，如果采用int表示走过的角度，不论定时器间隔多小
			// 只能得到20帧， 因为中间计算出来带小数点的角度，都四舍五入了。
			Gdiplus::REAL fSweepAngle = (fPassPath - nPass) * pSubPath->m_nSweepAngle / pSubPath->GetLength();

			// 第三象限
			// 圆的中心
			Gdiplus::PointF ptCenter;
			ptCenter.X = pSubPath->m_prcArcBoundsF->X + pSubPath->m_prcArcBoundsF->Width / 2.0f;
			ptCenter.Y = pSubPath->m_prcArcBoundsF->Y + pSubPath->m_prcArcBoundsF->Height / 2.0f;
			// 半径
			Gdiplus::REAL nRadius = pSubPath->m_prcArcBoundsF->Width / 2.0f;

			Gdiplus::REAL fAngle = fSweepAngle + pSubPath->m_nStartAngle - 90;
			Gdiplus::REAL nXOffset = sin(fAngle * 3.1415926f / 180) * nRadius;
			Gdiplus::REAL nYOffset = cos(fAngle * 3.1415926f / 180) * nRadius;
			Gdiplus::PointF pt;
			pt.X = ptCenter.X - nXOffset;
			pt.Y = ptCenter.Y + nYOffset;

			pOutPoint->SetPoint(pt);
		}
	}
	else
	{
		int nStartIndex = 0;
		int nEndIndex = pSubPath->m_pvpPoints->size() - 1;
		int nIndex = nStartIndex;
		while (nEndIndex > nStartIndex)
		{
			nIndex = nStartIndex + (nEndIndex - nStartIndex) / 2;
			CFlattenPathPoint *pSubPoint = (*pSubPath->m_pvpPoints)[nIndex];

			Gdiplus::REAL nStartLenght = 0;
			if (nIndex > 0)
			{
				nStartLenght = (*pSubPath->m_pvpPoints)[nIndex - 1]->GetLengthToStart();
			}

			if (nStartLenght <= fPassPath && fPassPath <= pSubPoint->GetLengthToStart())
			{
				break;
			}

			// 从左侧找
			if (fPassPath < nStartLenght)
			{
				nEndIndex = nIndex;
			}
			else if (fPassPath > pSubPoint->GetLengthToStart())
			{
				nStartIndex = nIndex;
			}
		}

		// 计算t在小线段内的坐标
		CFlattenPathPoint *ppt1 = NULL;
		if (nIndex <= 0)
		{
			ppt1 = (*pSubPath->m_pvpPoints)[0];
		}
		else
		{
			ppt1 = (*pSubPath->m_pvpPoints)[nIndex - 1];
		}

		CFlattenPathPoint *ppt2 = (*pSubPath->m_pvpPoints)[nIndex];

		Gdiplus::REAL nLineLength = ppt2->GetLengthToStart() - ppt1->GetLengthToStart();
		Gdiplus::REAL nLengthToEnd = ppt2->GetLengthToStart() - fPassPath;

		Gdiplus::PointF pt;
		pt.X = ppt2->m_pptF->X - (ppt2->m_pptF->X - ppt1->m_pptF->X) * nLengthToEnd / nLineLength;
		pt.Y = ppt2->m_pptF->Y - (ppt2->m_pptF->Y - ppt1->m_pptF->Y) * nLengthToEnd / nLineLength;

		pOutPoint->SetPoint(pt);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Render function

void IUI::Render::TransparentBlt2(HDC hdcDest,
	int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
	HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight,
	UINT crTransparent)
{
	if (nDestWidth <= 0 || nDestHeight <= 0 || nSrcWidth <= 0 || nSrcHeight <= 0)
	{
		return;
	}

	HBITMAP hOldImageBMP;
	HBITMAP hImageBMP = ::CreateCompatibleBitmap(hdcDest, nDestWidth, nDestHeight);
	_ASSERT(hImageBMP != NULL);
	HBITMAP hOldMaskBMP;
	HBITMAP hMaskBMP = ::CreateBitmap(nDestWidth, nDestHeight, 1, 1, NULL);
	HDC     hImageDC = ::CreateCompatibleDC(hdcDest);
	HDC     hMaskDC = ::CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP)::SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP)::SelectObject(hMaskDC, hMaskBMP);


	// copy source DC to temp DC
	if (nDestWidth == nSrcWidth && nDestHeight == nSrcHeight)
	{
		BitBlt(hImageDC, 0, 0, nDestWidth, nDestHeight, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	}
	else
	{
		StretchBlt(hImageDC,
			0, 0, nDestWidth, nDestHeight,
			hdcSrc,
			nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
			SRCCOPY);
	}


	// Set transparent color
	::SetBkColor(hImageDC, crTransparent);

	// Create a bitmap with white as transparent area, and black as other area
	BitBlt(hMaskDC, 0, 0, nDestWidth, nDestHeight, hImageDC, 0, 0, SRCCOPY);

	// Create a bitmap with black as transparent area, and keep up other area
	::SetBkColor(hImageDC, RGB(0, 0, 0));
	::SetTextColor(hImageDC, RGB(255, 255, 255));
	BitBlt(hImageDC, 0, 0, nDestWidth, nDestHeight, hMaskDC, 0, 0, SRCAND);


	// keep up screen within transparent area, and change other area to black
	::SetBkColor(hdcDest, RGB(255, 255, 255));
	::SetTextColor(hdcDest, RGB(0, 0, 0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nDestWidth, nDestHeight, hMaskDC, 0, 0, SRCAND);

	// "or" operate to final
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nDestWidth, nDestHeight, hImageDC, 0, 0, SRCPAINT);

	// free object
	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}

void IUI::Render::TransparentBltHBITMAP(HDC hdcDest,
	int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
	HBITMAP hBmpSrc,
	int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight,
	UINT crTransparent)
{
	HDC hDC = ::GetDC(::GetDesktopWindow());
	HDC memDC = ::CreateCompatibleDC(hDC);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, hBmpSrc);
	TransparentBlt(hdcDest,
		nXOriginDest, nYOriginDest, nDestWidth, nDestHeight,
		memDC,
		nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
		crTransparent);
	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);
	::ReleaseDC(::GetDesktopWindow(), hDC);
}

int IUI::Render::TileBlt(HDC hdcDest,
	int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
	HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight,
	BYTE btSourceConstantAlpha/* = 255*/)
{
	if (hdcDest == NULL || hdcSrc == NULL)
	{
		return -1;
	}
	if (nDestWidth <= 0 || nDestHeight <= 0)
	{
		return -2;
	}
	if (nXOriginSrc < 0 || nYOriginSrc < 0)
	{
		return -3;
	}
	if (nSrcWidth <= 0 || nSrcHeight <= 0)
	{
		return -4;
	}

	HDC hMidDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP hMidBmp = ::CreateCompatibleBitmap(hdcDest, nSrcWidth, nSrcHeight);
	_ASSERT(GetObjectType(hMidBmp) == OBJ_BITMAP);
	if (hMidBmp == NULL)
	{
		::DeleteDC(hMidDC);
		return -5;
	}
	HBITMAP hOldMidBmp = (HBITMAP)::SelectObject(hMidDC, hMidBmp);
	_ASSERT(hOldMidBmp != hMidBmp);
	::BitBlt(hMidDC, 0, 0, nSrcWidth, nSrcHeight, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	::SelectObject(hMidDC, hOldMidBmp);
	HBRUSH brush = ::CreatePatternBrush(hMidBmp);

	// Save hdcDest's old state
	POINT pt;
	::GetBrushOrgEx(hdcDest, &pt);

	// Set hdcDest's new state
	::SetBrushOrgEx(hdcDest, nXOriginDest, nYOriginDest, NULL);

	::FillRect(hdcDest,
		CIUIRect(nXOriginDest, nYOriginDest, nXOriginDest + nDestWidth, nYOriginDest + nDestHeight),
		brush);

	// Restored hdcDest's original state
	::SetBrushOrgEx(hdcDest, pt.x, pt.y, NULL);

	::DeleteObject(brush);
	::DeleteObject(hMidBmp);
	::DeleteDC(hMidDC);

	return 0;
}

int IUI::Render::AlphaTileBlt(HDC hdcDest,
	int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
	HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight,
	BYTE btSourceConstantAlpha/* = 255*/)
{
	if (nSrcWidth == 1 && nSrcHeight == 1)
	{
		return StretchAlphaBlend(hdcDest,
				nXOriginDest, nYOriginDest, nDestWidth, nDestHeight,
				hdcSrc,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
				btSourceConstantAlpha);
	}
	else
	{
		if (hdcDest == NULL || hdcSrc == NULL)
		{
			return -1;
		}
		if (nDestWidth <= 0 || nDestHeight <= 0)
		{
			return -2;
		}
		if (nXOriginSrc < 0 || nYOriginSrc < 0)
		{
			return -3;
		}
		if (nSrcWidth <= 0 || nSrcHeight <= 0)
		{
			return -4;
		}

		int nXCount = nDestWidth / nSrcWidth;
		int nYCount = nDestHeight / nSrcHeight;

		if (nSrcWidth == 1)
		{
			for (int y = 0; y < nYCount; ++y)
			{
				StretchAlphaBlend(hdcDest,
					nXOriginDest, nYOriginDest + nSrcHeight * y, nDestWidth, nSrcHeight,
					hdcSrc,
					nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
					btSourceConstantAlpha);
			}

			// Alpha tile the last block at bottom.
			int nYLast = nDestHeight % nSrcHeight;
			if (nYLast > 0)
			{
				StretchAlphaBlend(hdcDest,
					nXOriginDest, nYOriginDest + nSrcHeight * nYCount, nDestWidth, nYLast,
					hdcSrc,
					nXOriginSrc, nYOriginSrc, nSrcWidth, nYLast,
					btSourceConstantAlpha);
			}
		}
		else if (nSrcHeight == 1)
		{
			for (int x = 0; x < nXCount; ++x)
			{
				StretchAlphaBlend(hdcDest,
					nXOriginDest + nSrcWidth * x, nYOriginDest, nSrcWidth, nDestHeight,
					hdcSrc,
					nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
					btSourceConstantAlpha);
			}

			// Alpha tile the last block at right.
			int nXLast = nDestWidth % nSrcWidth;
			if (nXLast > 0)
			{
				StretchAlphaBlend(hdcDest,
					nXOriginDest + nSrcWidth * nXCount, nYOriginDest, nXLast, nDestHeight,
					hdcSrc,
					nXOriginSrc, nYOriginSrc, nXLast, nSrcHeight,
					btSourceConstantAlpha);
			}
		}
		else
		{
			// 这段效率极低，需要优化
			// Alpha tile in vertical
			for (int y = 0; y < nYCount; ++y)
			{
				// Alpha tile in horizontal
				for (int x = 0; x < nXCount; ++x)
				{
					StretchAlphaBlend(hdcDest,
						nXOriginDest + nSrcWidth * x,
						nYOriginDest + nSrcHeight * y,
						nSrcWidth, nSrcHeight,
						hdcSrc,
						nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, btSourceConstantAlpha);
				}

				// Alpha tile the last block in horizontal
				int nXLastDestWidth = nDestWidth % nSrcWidth;
				if (nXLastDestWidth > 0)
				{
					StretchAlphaBlend(hdcDest,
						nXOriginDest + nSrcWidth * nXCount,
						nYOriginDest + nSrcHeight * y,
						nXLastDestWidth,
						nSrcHeight,
						hdcSrc,
						nXOriginSrc,
						nYOriginSrc,
						nXLastDestWidth,
						nSrcHeight,
						btSourceConstantAlpha);
				}
			}

			// Alpha tile the last block in vertical
			int nYLastDestHeight = nDestHeight % nSrcHeight;
			if (nYLastDestHeight > 0)
			{
				// Alpha tile in horizontal
				for (int x = 0; x < nXCount; ++x)
				{
					StretchAlphaBlend(hdcDest,
						nXOriginDest + nSrcWidth * x,
						nYOriginDest + nSrcHeight * nYCount,
						nSrcWidth,
						nYLastDestHeight,
						hdcSrc,
						nXOriginSrc,
						nYOriginSrc,
						nSrcWidth,
						nYLastDestHeight,
						btSourceConstantAlpha);
				}

				// Alpha tile the last block in right-bottom corner.
				int nXLastDestWidth = nDestWidth % nSrcWidth;
				if (nXLastDestWidth > 0)
				{
					StretchAlphaBlend(hdcDest,
						nXOriginDest + nSrcWidth * nXCount,
						nYOriginDest + nSrcHeight * nYCount,
						nXLastDestWidth,
						nYLastDestHeight,
						hdcSrc,
						nXOriginSrc,
						nYOriginSrc,
						nXLastDestWidth,
						nYLastDestHeight,
						btSourceConstantAlpha);
				}
			}
		}
	}

	return 0;
}

// 因为Label的文本对齐属性只有一个，而Button有4个，所以保留TEXT_ALIGN_HOR和eTextVerAlign并
// 未归类到uTextFormat中
int IUI::Render::DrawFormatText(
	HDC hdcDest,
	LPCTSTR lpszText,
	int nTextLength,
	LPCRECT lprcDest,
	LPCRECT lprcPadding4Text,
	TEXT_ALIGN_HOR eTextHorAlign,
	TEXT_ALIGN_VER eTextVerAlign,
	UINT uTextFormat,
	HFONT hTextFont,
	COLORREF crText,
	BOOL bUseGdiplus)
{
	if (hdcDest == NULL || lprcDest == NULL)
	{
		return -1;
	}

	if (lpszText == NULL || nTextLength <= 0)
	{
		return 1;
	}

	SetBkMode(hdcDest, TRANSPARENT);
	HFONT hFontOld = NULL;

	hFontOld = (HFONT)SelectObject(hdcDest, hTextFont);
	::SetTextColor(hdcDest, crText);

	CIUIRect rcDraw = *lprcDest;
	if (lprcPadding4Text != NULL)
	{
		rcDraw.DeflateRect(lprcPadding4Text);
	}

	switch (eTextHorAlign)
	{
	case TAH_LEFT:
	default: // left
		uTextFormat |= DT_LEFT;
		break;
	case TAH_RIGHT:
		uTextFormat |= DT_RIGHT;
		break;
	case TAH_CENTER:
		uTextFormat |= DT_CENTER;
		break;
	}

	switch (eTextVerAlign)
	{
	case TAV_TOP:
	default: // top
		uTextFormat |= DT_TOP;
		break;
	case TAV_BOTTOM:
	{
		CIUIRect rcTest(rcDraw);
		if ((uTextFormat & DT_WORDBREAK) == DT_WORDBREAK)
		{
			int nHeight = DrawText(hdcDest, lpszText, nTextLength, rcTest,
					uTextFormat | DT_CALCRECT);

			rcDraw.top += (rcDraw.Height() - nHeight);
		}
		else
		{
			uTextFormat |= DT_BOTTOM;
		}
		break;
	}
	case TAV_CENTER:
	{
		// DrawText不支持多行时垂直居中，所以，先按Top设置计算出文本占用的
		// 尺寸，然后修改目标尺寸的top坐标
		CIUIRect rcTest(*lprcDest);
		if ((uTextFormat & DT_WORDBREAK) == DT_WORDBREAK)
		{
			int nHeight = DrawText(hdcDest, lpszText, nTextLength, rcTest,
					uTextFormat | DT_CALCRECT);

			int nAdjustHeight = (lprcDest->bottom - lprcDest->top - nHeight) / 2;
			rcDraw.DeflateRect(0, nAdjustHeight, 0, 0);
		}
		else
		{
			uTextFormat |= DT_VCENTER;
		}
		break;
	}
	}

	// #if(_MSC_VER > 1200) // VC6.0
	// 	if (m_bShadowText)
	// 	{
	// #ifdef _UNICODE
	// 		WCHAR *pwText = _bstr_t(strText);
	// 		DrawShadowText(pMemDC->GetSafeHdc(), pwText, strText.GetLength(), rcDraw, uFormat,
	//          bEnabled ? m_crN : m_crD, m_crTextShadow, m_ptTextShadowOffset.x,
	//          m_ptTextShadowOffset.y);
	// #else
	// 		pMemDC->DrawText(strText, rcDraw, uFormat);
	// #endif
	// 	}
	// 	else
	// #endif
	{
		if (bUseGdiplus)
		{
			Gdiplus::Graphics g(hdcDest);

			Gdiplus::RectF rc = Rect2RectF(rcDraw);
			return DrawTextGp(&g, lpszText, nTextLength,
					hTextFont, crText, &rc, uTextFormat);
		}
		else
		{
			DrawText(hdcDest, lpszText, nTextLength, rcDraw, uTextFormat);
		}
	}

	SelectObject(hdcDest, hFontOld);

	return 0;
}

int IUI::Render::DrawTextGp(
	Gdiplus::Graphics *pGraphicsDst,
	LPCTSTR lpszString,
	int nLength,
	HFONT hFont,
	COLORREF crText,
	const Gdiplus::RectF *prc,
	UINT format)
{
	if (pGraphicsDst == NULL || lpszString == NULL || prc == NULL)
	{
		return -1;
	}

	HDC hDC = pGraphicsDst->GetHDC();
	pGraphicsDst->ReleaseHDC(hDC);
	Gdiplus::Font font(hDC, hFont);

#ifdef _DEBUG
#ifdef new
#undef new
#endif // new
#endif // _DEBUG

	// 	if (hFont != NULL)
	// 	{
	// 		HDC hDC = pGraphicsDst->GetHDC();
	// 		pFont = new Font(hDC, hFont);
	// 		pGraphicsDst->ReleaseHDC(hDC);
	// 	}

#ifdef _DEBUG
#define new IUI_DEBUG_NEW
#endif // _DEBUG

	Gdiplus::StringFormat eAlign;
	eAlign.SetAlignment(Gdiplus::StringAlignmentNear);
	eAlign.SetLineAlignment(Gdiplus::StringAlignmentNear);

	if ((format & DT_SINGLELINE) == DT_SINGLELINE)
	{
		// Horizontal
		if ((format & DT_CENTER) == DT_CENTER)
		{
			eAlign.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((format & DT_RIGHT) == DT_RIGHT)
		{
			eAlign.SetAlignment(Gdiplus::StringAlignmentFar);
		}

		// Vertical
		if ((format & DT_VCENTER) == DT_VCENTER)
		{
			eAlign.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((format & DT_BOTTOM) == DT_BOTTOM)
		{
			eAlign.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}
	}
	// TODO:有时候即使使用GDI+绘制文本，文本区域也是全透明。 不知道是否和字体有关
	// 透明度改为254就可以了。 这或许和背景图是24位不带alpha通道的Png有关
	Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetRValue(crText), GetGValue(crText), GetBValue(crText)));

	// Draw string.
	pGraphicsDst->DrawString(
		(LPWSTR)_bstr_t(lpszString),
		nLength,
		&font,
		*prc,
		&eAlign,
		&brush);

	return 0;
}

int IUI::Render::DrawLine(HDC hDstDC, COLORREF crLine, int nLineWidht, POINT ptBegin, POINT ptEnd)
{
	return DrawLine(hDstDC,
			crLine, nLineWidht, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
}

int IUI::Render::DrawLine(HDC hDstDC, COLORREF crLine,
	int nLineWidht, int nX1, int nY1, int nX2, int nY2)
{
	LOGPEN lpLine;
	lpLine.lopnColor = crLine;
	lpLine.lopnStyle = PS_SOLID;
	lpLine.lopnWidth = CIUIPoint(nLineWidht, nLineWidht);
	HPEN hPenLine = ::CreatePenIndirect(&lpLine);
	HPEN hOldPen = (HPEN)::SelectObject(hDstDC, hPenLine);

	CIUIPoint point;
	VERIFY(::MoveToEx(hDstDC, nX1, nY1, &point));
	::LineTo(hDstDC, nX2, nY2);

	::SelectObject(hDstDC, hOldPen);
	DeleteObject(hPenLine);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// IUIImage

int IUI::Render::StretchAlphaBlend(
	HDC hdcDest,
	int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
	HDC hdcSrc,
	int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight,
	BYTE btSourceConstantAlpha/* = 255*/)
{
	BLENDFUNCTION blend;
	blend.SourceConstantAlpha = btSourceConstantAlpha;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// If the nDestWidth or nDestHeight not equal the image size, may return FALSE.
	BOOL bResult = ::AlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nDestWidth, nDestHeight,
			hdcSrc, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, blend);
	if (!bResult)
	{
		return -1;
	}

	return 0;
}

// the resize point in bitmap, // best
int IUI::Render::NineGridBltG(
	HDC hdcDest,
	int nDestX,
	int nDestY,
	int nDestWidth,
	int nDestHeight,
	HBITMAP hbmpSrc,
	LPCRECT lprcResizeArg)
{
	if (hbmpSrc == NULL)
	{
		return -1;
	}

	HDC memDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, hbmpSrc);

	BITMAP bmp;
	int nCount = ::GetObject(hbmpSrc, sizeof(bmp), &bmp);
	_ASSERT(nCount != 0);

	int nTilePixelX = bmp.bmWidth - lprcResizeArg->left - lprcResizeArg->right;
	int nTilePixelY = bmp.bmHeight - lprcResizeArg->top - lprcResizeArg->bottom;

	int nDestCol1Width = nDestWidth - lprcResizeArg->left - lprcResizeArg->right;
	int nDestLine1Height = nDestHeight - lprcResizeArg->top - lprcResizeArg->bottom;

	int nDestCol2Left = nDestWidth - lprcResizeArg->right;
	int nDestLine2Top = nDestHeight - lprcResizeArg->bottom;

	// if the dest size less than source size, not stretch.
	if (nDestWidth <= bmp.bmWidth && nDestHeight <= bmp.bmHeight)
	{
		::BitBlt(hdcDest, nDestX, nDestY, nDestWidth, nDestHeight, memDC, 0, 0, SRCCOPY);
	}
	else if (nDestWidth <= bmp.bmWidth && nDestHeight > bmp.bmHeight)
	{
		// top
		::BitBlt(hdcDest, nDestX, nDestY, nDestWidth, lprcResizeArg->top, memDC, 0, 0, SRCCOPY);
		// center, not stretch at horizontal(ignore lprcResizeArg->left and nTilePixelX), tile at vertical.
		if (lprcResizeArg->top < bmp.bmHeight)	// in picture
		{
			TileBlt(hdcDest, nDestX, nDestY + lprcResizeArg->top, nDestWidth, nDestLine1Height,
				memDC, 0, lprcResizeArg->top, nDestWidth, nTilePixelY, 255);
		}

		// bottom
		::BitBlt(hdcDest, nDestX, nDestY + nDestLine2Top, nDestWidth, lprcResizeArg->bottom,
			memDC, 0, bmp.bmHeight - lprcResizeArg->bottom, SRCCOPY);
	}
	else if (nDestWidth > bmp.bmWidth && nDestHeight <= bmp.bmHeight)
	{
		// left
		::BitBlt(hdcDest, nDestX, nDestY, lprcResizeArg->left, nDestHeight, memDC, 0, 0, SRCCOPY);
		// center, not stretch at vertical, tile at horizontal.
		if (lprcResizeArg->left < bmp.bmWidth)	// in picture
		{
			TileBlt(hdcDest, nDestX + lprcResizeArg->left, nDestY, nDestCol1Width, nDestHeight,
				memDC, lprcResizeArg->left, 0, nTilePixelX, nDestHeight, 255);
		}
		// right
		::BitBlt(hdcDest, nDestX + nDestCol2Left, nDestY, lprcResizeArg->right, nDestHeight,
			memDC, bmp.bmWidth - lprcResizeArg->right, 0, SRCCOPY);
	}
	else
	{
		// area 0
		if (lprcResizeArg->left > 0 && lprcResizeArg->top > 0)
		{
			::BitBlt(hdcDest, nDestX, nDestY, lprcResizeArg->left, lprcResizeArg->top, memDC, 0, 0, SRCCOPY);
		}
		// area 1
		if (lprcResizeArg->top > 0)
		{
			TileBlt(hdcDest, nDestX + lprcResizeArg->left, nDestY, nDestCol1Width, lprcResizeArg->top,
				memDC, lprcResizeArg->left, 0, nTilePixelX, lprcResizeArg->top, 255);
		}
		// area 2
		if (lprcResizeArg->left < bmp.bmWidth - 1 && lprcResizeArg->top > 0)
		{
			::BitBlt(hdcDest,
				nDestX + nDestCol2Left, nDestY,
				lprcResizeArg->right, lprcResizeArg->top,
				memDC,
				bmp.bmWidth - lprcResizeArg->right, 0, SRCCOPY);
		}

		// area 3
		if (lprcResizeArg->left > 0)
		{
			TileBlt(hdcDest, nDestX, nDestY + lprcResizeArg->top, lprcResizeArg->left, nDestLine1Height,
				memDC, 0, lprcResizeArg->top, lprcResizeArg->left, nTilePixelY, 255);
		}
		// area 4
		_ASSERT(nDestCol1Width > 0 && nDestLine1Height > 0);
		TileBlt(hdcDest,
			nDestX + lprcResizeArg->left, nDestY + lprcResizeArg->top, nDestCol1Width, nDestLine1Height,
			memDC, lprcResizeArg->left, lprcResizeArg->top, nTilePixelX, nTilePixelY, 255);
		// area 5
		if (lprcResizeArg->right > 0)
		{
			TileBlt(hdcDest,
				nDestX + nDestCol2Left, nDestY + lprcResizeArg->top,
				lprcResizeArg->right, nDestLine1Height,
				memDC,
				bmp.bmWidth - lprcResizeArg->right, lprcResizeArg->top,
				lprcResizeArg->right, nTilePixelY, 255);
		}

		// area 6
		if (lprcResizeArg->left > 0 && lprcResizeArg->bottom > 0)
		{
			::BitBlt(hdcDest,
				nDestX, nDestY + nDestLine2Top, lprcResizeArg->left, lprcResizeArg->bottom,
				memDC,
				0, bmp.bmHeight - lprcResizeArg->bottom, SRCCOPY);
		}
		// area 7
		if (lprcResizeArg->bottom > 0)
		{
			TileBlt(hdcDest,
				nDestX + lprcResizeArg->left, nDestY + nDestLine2Top,
				nDestCol1Width, lprcResizeArg->bottom,
				memDC,
				lprcResizeArg->left, bmp.bmHeight - lprcResizeArg->bottom,
				nTilePixelX, lprcResizeArg->bottom, 255);
		}
		// area 8
		if (lprcResizeArg->right > 0 && lprcResizeArg->bottom > 0)
		{
			::BitBlt(hdcDest,
				nDestX + nDestCol2Left, nDestY + nDestLine2Top,
				lprcResizeArg->right, lprcResizeArg->bottom,
				memDC,
				bmp.bmWidth - lprcResizeArg->right,
				bmp.bmHeight - lprcResizeArg->bottom, SRCCOPY);
		}
	}

	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::NineGridBltG(
	HDC hdcDest,
	LPCRECT lprcDest,
	HBITMAP hbmpSrc,
	LPCRECT lprcResizeArg)
{
	if (hdcDest == NULL || lprcDest == NULL)
	{
		return -1;
	}

	return NineGridBltG(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hbmpSrc, lprcResizeArg);
}

int IUI::Render::PartNineGridBltG(
	HDC hdcDest,
	int nDestX,
	int nDestY,
	int nDestWidth,
	int nDestHeight,
	HBITMAP hbmpSrc,
	int nPart,
	int nPartIndex,
	LPCRECT lprcResizeArg)
{
	if (hbmpSrc == NULL)
	{
		return -1;
	}
	if (nPart <= 0)
	{
		return -2;
	}
	if (nPartIndex < 0)
	{
		return -3;
	}
	if (nPartIndex >= nPart)
	{
		return -4;
	}

	SIZE sizeSrc;
	IUIImage::GetBitmapSizeG(hbmpSrc, &sizeSrc);

	CIUIRect rcSrc;
	rcSrc.left = sizeSrc.cx * nPartIndex / nPart;
	rcSrc.top = 0;
	rcSrc.right = rcSrc.left + sizeSrc.cx / nPart;
	rcSrc.bottom = rcSrc.top + sizeSrc.cy;

	return PartNineGridBltG(hdcDest,
			CIUIRect(nDestX, nDestY, nDestX + nDestWidth, nDestY + nDestHeight),
			hbmpSrc, rcSrc, lprcResizeArg);
}

int IUI::Render::PartNineGridBltG(
	HDC hdcDest,
	LPCRECT lprcDest,
	HBITMAP hbmpSrc,
	int nPart,
	int nPartIndex,
	LPCRECT lprcResizeArg)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	return PartNineGridBltG(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hbmpSrc,
			nPart, nPartIndex,
			lprcResizeArg);
}

int IUI::Render::PartNineGridBltG(
	HDC hdcDest,
	LPCRECT lprcDest,
	HBITMAP hbmpSrc,
	LPCRECT lprcSrc,
	LPCRECT lprcResizeArg)
{
	if (hbmpSrc == NULL || lprcDest == NULL)
	{
		return -1;
	}

	CIUIRect rcDest(lprcDest);

	CIUIRect rcSrc(lprcSrc);
	if (rcSrc.IsRectEmpty())
	{
		SIZE size;
		IUIImage::GetBitmapSizeG(hbmpSrc, &size);
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = size.cx;
		rcSrc.bottom = size.cy;
	}

	HDC memDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, hbmpSrc);

	BITMAP bmpSrc;
	::GetObject(hbmpSrc, sizeof(bmpSrc), &bmpSrc);

	int nTilePixelX = RECTWIDTH(*lprcSrc) - lprcResizeArg->left - lprcResizeArg->right;
	int nTilePixelY = RECTHEIGHT(*lprcSrc) - lprcResizeArg->top - lprcResizeArg->bottom;

	int nDestCol1Width = RECTWIDTH(*lprcDest) - lprcResizeArg->left - lprcResizeArg->right;
	int nDestLine1Height = RECTHEIGHT(*lprcDest) - lprcResizeArg->top - lprcResizeArg->bottom;

	int nDestCol2Left = RECTWIDTH(*lprcDest) - lprcResizeArg->right;
	int nDestLine2Top = RECTHEIGHT(*lprcDest) - lprcResizeArg->bottom;

	// if the dest size less than source size, not stretch.
	if (rcDest.Width() <= rcSrc.Width() && rcDest.Height() <= rcSrc.Height())
	{
		::BitBlt(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
			memDC, rcSrc.left, rcSrc.top, SRCCOPY);
	}
	else if (rcDest.Width() <= rcSrc.Width() && rcDest.Height() > rcSrc.Height())
	{
		// top
		::BitBlt(hdcDest, rcDest.left, rcDest.top, rcDest.Width(), lprcResizeArg->top,
			memDC, rcSrc.left, rcSrc.top, SRCCOPY);

		// center, not stretch at horizontal(ignore lprcResizeArg->left and nTilePixelX), tile at vertical.
		if (lprcResizeArg->top < rcSrc.Height())	// in picture
		{
			TileBlt(hdcDest,
				rcDest.left, rcDest.top + lprcResizeArg->top, rcDest.Width(), nDestLine1Height,
				memDC,
				rcSrc.left, rcSrc.top + lprcResizeArg->top, rcDest.Width(), nTilePixelY, 255);
		}

		// bottom
		::BitBlt(hdcDest, rcDest.left, rcDest.top + nDestLine2Top, rcDest.Width(), lprcResizeArg->bottom,
			memDC, rcSrc.left, rcSrc.top + RECTHEIGHT(*lprcSrc) - lprcResizeArg->bottom, SRCCOPY);
	}
	else if (rcDest.Width() > rcSrc.Width() && rcDest.Height() <= rcSrc.Height())
	{
		// left
		::BitBlt(hdcDest, rcDest.left, rcDest.top, lprcResizeArg->left, rcDest.Height(),
			memDC, rcSrc.left, rcSrc.top, SRCCOPY);

		// center, not stretch at vertical, tile at horizontal.
		if (lprcResizeArg->left < rcSrc.Width())	// in picture
		{
			TileBlt(hdcDest, rcDest.left + lprcResizeArg->left, rcDest.top, nDestCol1Width, rcDest.Height(),
				memDC, rcSrc.left + lprcResizeArg->left, rcSrc.top, nTilePixelX, rcDest.Height(), 255);
		}

		// right
		::BitBlt(hdcDest, rcDest.left + nDestCol2Left, rcDest.top, lprcResizeArg->right, rcDest.Height(),
			memDC, rcSrc.left + RECTWIDTH(*lprcSrc) - lprcResizeArg->right, rcSrc.top, SRCCOPY);
	}
	else
	{
		// area 0
		if (lprcResizeArg->left > 0 && lprcResizeArg->top > 0)
		{
			::BitBlt(hdcDest, rcDest.left, rcDest.top, lprcResizeArg->left, lprcResizeArg->top,
				memDC, rcSrc.left, rcSrc.top, SRCCOPY);
		}

		// area 1
		if (lprcResizeArg->top > 0)
		{
			TileBlt(hdcDest, rcDest.left + lprcResizeArg->left, rcDest.top, nDestCol1Width, lprcResizeArg->top,
				memDC, rcSrc.left + lprcResizeArg->left, rcSrc.top, nTilePixelX, lprcResizeArg->top, 255);
		}

		// area 2
		if (lprcResizeArg->left < rcSrc.Width() - 1 && lprcResizeArg->top > 0)
		{
			::BitBlt(hdcDest,
				rcDest.left + nDestCol2Left, rcDest.top, lprcResizeArg->right, lprcResizeArg->top,
				memDC, rcSrc.left + RECTWIDTH(*lprcSrc) - lprcResizeArg->right, rcSrc.top, SRCCOPY);
		}

		// area 3
		if (lprcResizeArg->left > 0)
		{
			TileBlt(hdcDest,
				rcDest.left, rcDest.top + lprcResizeArg->top, lprcResizeArg->left, nDestLine1Height,
				memDC, rcSrc.left, rcSrc.top + lprcResizeArg->top, lprcResizeArg->left, nTilePixelY, 255);
		}

		// area 4
		_ASSERT(nDestCol1Width > 0 && nDestLine1Height > 0);
		TileBlt(hdcDest,
			rcDest.left + lprcResizeArg->left,
			rcDest.top + lprcResizeArg->top, nDestCol1Width, nDestLine1Height,
			memDC, rcSrc.left + lprcResizeArg->left, rcSrc.top + lprcResizeArg->top, nTilePixelX, nTilePixelY, 255);

		// area 5
		if (lprcResizeArg->right > 0)
		{
			TileBlt(hdcDest,
				rcDest.left + nDestCol2Left, rcDest.top + lprcResizeArg->top,
				lprcResizeArg->right, nDestLine1Height,
				memDC,
				rcSrc.left + RECTWIDTH(*lprcSrc) - lprcResizeArg->right, rcSrc.top + lprcResizeArg->top,
				lprcResizeArg->right, nTilePixelY, 255);
		}

		// area 6
		if (lprcResizeArg->left > 0 && lprcResizeArg->bottom > 0)
		{
			::BitBlt(hdcDest,
				rcDest.left, rcDest.top + nDestLine2Top, lprcResizeArg->left, lprcResizeArg->bottom,
				memDC,
				rcSrc.left, rcSrc.top + RECTHEIGHT(*lprcSrc) - lprcResizeArg->bottom, SRCCOPY);
		}

		// area 7
		if (lprcResizeArg->bottom > 0)
		{
			TileBlt(hdcDest,
				rcDest.left + lprcResizeArg->left, rcDest.top + nDestLine2Top,
				nDestCol1Width, lprcResizeArg->bottom,
				memDC,
				rcSrc.left + lprcResizeArg->left, rcSrc.top + RECTHEIGHT(*lprcSrc) - lprcResizeArg->bottom,
				nTilePixelX, lprcResizeArg->bottom, 255);
		}

		// area 8
		if (lprcResizeArg->right > 0 && lprcResizeArg->bottom > 0)
		{
			::BitBlt(hdcDest,
				rcDest.left + nDestCol2Left, rcDest.top + nDestLine2Top,
				lprcResizeArg->right, lprcResizeArg->bottom,
				memDC,
				rcSrc.left + RECTWIDTH(*lprcSrc) - lprcResizeArg->right, rcSrc.top + RECTHEIGHT(*lprcSrc) - lprcResizeArg->bottom, SRCCOPY);
		}
	}

	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::StretchBltG(HDC hdcDest, LPCRECT lprcDest, HBITMAP hbmpSrc)
{
	if (hbmpSrc == NULL)
	{
		return -1;
	}

	HDC memDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, hbmpSrc);

	CIUISize sizeSrc;
	IUIImage::GetBitmapSizeG(hbmpSrc, &sizeSrc);
	::StretchBlt(hdcDest, lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
		memDC, 0, 0, sizeSrc.cx, sizeSrc.cy, SRCCOPY);

	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::BitBltG(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HBITMAP hSrcBmp, int nXSrc, int nYSrc, DWORD dwRop)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}

	HDC memDC = ::CreateCompatibleDC(hdcDest);

	// store the Font object before call DefWindowProc
	HFONT hFontOld = (HFONT)::GetCurrentObject(memDC, OBJ_FONT);

	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, hSrcBmp);

	::BitBlt(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight, memDC, nXSrc, nYSrc, dwRop);

	::SelectObject(memDC, pSrcBmpOld);

	if (hFontOld != NULL)
	{
		::SelectObject(memDC, hFontOld);
	}

	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::BitBltG(HDC hdcDest, LPCRECT lprcDest,
	HBITMAP hSrcBmp, int nXSrc, int nYSrc, DWORD dwRop)
{
	return BitBltG(hdcDest, lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hSrcBmp, nXSrc, nYSrc, dwRop);
}

int IUI::Render::CenterBitBltG(HDC hDC, LPCRECT lprcDest, HBITMAP hBmp, DWORD dwRop)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	CIUISize size;
	int nRet = IUIImage::GetBitmapSizeG(hBmp, &size);
	if (nRet != 0)
	{
		return nRet;
	}

	CIUIRect rcNewDest;
	rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - size.cx) / 2;
	rcNewDest.right = rcNewDest.left + size.cx;
	rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - size.cy) / 2;
	rcNewDest.bottom = rcNewDest.top + size.cy;

	return BitBltG(hDC, rcNewDest.left, rcNewDest.top, rcNewDest.Width(), rcNewDest.Height(),
			hBmp, 0, 0, dwRop);
}

int IUI::Render::DrawForegroundG(HDC hDstDC, LPCRECT lprcClient,
	LPCRECT lprcForegroundMargin, HBITMAP hbmpFg,
	FOREGROUND_ALIGN_HOR eForegroundAlignHor, FOREGROUND_ALIGN_VER eForegroundAlignVer,
	COLORREF crMask)
{
	if (hDstDC == NULL || hbmpFg == NULL || lprcClient == NULL)
	{
		return -1;
	}

	CIUISize sizeImg;
	IUIImage::GetBitmapSizeG(hbmpFg, &sizeImg);

	CIUIRect rcControl(lprcClient);
	if (lprcForegroundMargin != NULL)
	{
		rcControl.DeflateRect(lprcForegroundMargin);
	}

	int nX = rcControl.left;
	int nY = rcControl.top;

	if (eForegroundAlignHor == FAH_CENTER) // center
	{
		nX += (rcControl.Width() - sizeImg.cx) / 2;
	}
	else if (eForegroundAlignHor == FAH_LEFT || eForegroundAlignHor == FAH_STRETCH) // left or Stretch
	{
		// Keep.
	}
	else if (eForegroundAlignHor == FAH_RIGHT) // right
	{
		nX += (rcControl.Width() - sizeImg.cx);
	}
	else
	{
		_ASSERT(FALSE);
	}

	if (eForegroundAlignVer == FAV_CENTER) // center
	{
		nY += (rcControl.Height() - sizeImg.cy) / 2;
	}
	else if (eForegroundAlignVer == FAV_TOP || eForegroundAlignVer == FAV_STRETCH) // top or Stretch
	{
		// Keep.
	}
	else if (eForegroundAlignVer == FAV_BOTTOM) // bottom
	{
		nY += (rcControl.Height() - sizeImg.cy);
	}
	else
	{
		_ASSERT(FALSE);
	}

	HDC hMemDC = ::CreateCompatibleDC(hDstDC);
	HBITMAP hbmpOld = (HBITMAP)::SelectObject(hMemDC, hbmpFg);

	TransparentBlt2(hDstDC, nX, nY, sizeImg.cx, sizeImg.cy,
		hMemDC, 0, 0, sizeImg.cx, sizeImg.cy, crMask);

	::SelectObject(hMemDC, hbmpOld);
	::DeleteDC(hMemDC);

	return 0;
}

int IUI::Render::DrawImageG(
	HDC hDstDC,
	LPCRECT lprcImage,
	HBITMAP hImage,
	IMAGE_RESIZE_MODE eImgResizeMode,
	LPCRECT lprcResizeArg)
{
	if (hDstDC == NULL || lprcImage == NULL)
	{
		return -1;
	}
	if (hImage == NULL)
	{
		return -2;
	}

	if (eImgResizeMode == IRM_9GRID)
	{
		NineGridBltG(hDstDC, lprcImage, hImage, lprcResizeArg);
	}
	else if (eImgResizeMode == IRM_STRETCH
		|| eImgResizeMode == IRM_STRETCH_HIGH_QUALITY)
	{
		StretchBltG(hDstDC, lprcImage, hImage);
	}

	return 0;
}

// 早期的IUINineGridBlt函数，声明如下：
// int IUI::IUINineGridBlt(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
//	   HIUIIMAGE himgSrc, int nResizeX, int nResizeY, COLORREF crColorKey = -1, BYTE btSourceConstantAlpha = 255)
// himgSrc参数的下一个参数是int nResizeX, 表示himgSrc的水平缩放点，
// 现在，缩放点已移植到了HIUIIMAGE内部，故新的IUINineGridBlt把int nResizeX参数删除。声明如下
// int IUI::IUINineGridBlt(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
//	   HIUIIMAGE himgSrc, COLORREF crColorKey = -1, BYTE btSourceConstantAlpha = 255)
// 但是删除后，会导致一个问题。 假如原来的代码如下：
// IUINineGridBlt(hDC, 0, 0, 5, 5, hImg, 3, 3);
// 上面的3, 3,表示缩放点。 但在修改声明后，上面的代码仍然可以编译通过，编译时并不会
// 导致编译错误或警告。 但这里的第一个3，表示的是crColorKey，第二个3表示btSourceConstantAlpha。
// 意义已发生了变化，这将导致潜在的bug。 所以，解决方案有两个：
// 1. 在crColorKey之前，增加一个double参数，使上面的代码
// 在编译期，就被编译器识别出来。 如果当时IUINineGridBlt没有默认参数，删除缩放点参数后，
// 也会在编译期识别。 所以，以后定义函数，禁止使用默认参数
// 2. 修改IUINineGridBlt的参数。把原来的默认参数都删除掉，重新加一个带原来默认参数的IUINineGridBltEx。
// IUI采用第二种方法。
int IUI::Render::IUINineGridBlt(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg)
{
	return IUINineGridBltEx(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
			himgSrc, lprcResizeArg, -1, 255);
}

int IUI::Render::IUINineGridBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE hbmpSrc, LPCRECT lprcResizeArg)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	return IUINineGridBltEx(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hbmpSrc, lprcResizeArg, -1, 255);
}

int IUI::Render::IUINineGridBltEx(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, COLORREF crColorKey, BYTE btSourceConstantAlpha)
{
	if (himgSrc == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();

	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -2;
	}

	int nTilePixelX = himgSrc->GetWidth() - lprcResizeArg->left - lprcResizeArg->right;
	int nTilePixelY = himgSrc->GetHeight() - lprcResizeArg->top - lprcResizeArg->bottom;

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC memDC = ::CreateCompatibleDC(hdcDest);
		HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, himgSrc->GetSafeHBITMAP());

		int nDestCol1Width = nDestWidth - lprcResizeArg->left - lprcResizeArg->right;
		int nDestLine1Height = nDestHeight - lprcResizeArg->top - lprcResizeArg->bottom;

		int nDestCol2Left = nDestWidth - lprcResizeArg->right;
		int nDestLine2Top = nDestHeight - lprcResizeArg->bottom;

		int nRet = -1;
		while (true)
		{
			// if the dest size less than source size, not stretch.
			if (nDestWidth <= himgSrc->GetWidth() && nDestHeight <= himgSrc->GetHeight())
			{
				nRet = StretchAlphaBlend(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
						memDC, 0, 0, nDestWidth, nDestHeight, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}
			}
			else if (nDestWidth <= himgSrc->GetWidth() && nDestHeight > himgSrc->GetHeight())
			{
				// top
				nRet = StretchAlphaBlend(hdcDest, nXDest, nYDest, nDestWidth, lprcResizeArg->top,
						memDC, 0, 0, nDestWidth, lprcResizeArg->top, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}

				// center, not stretch at horizontal(ignore lprcResizeArg->left and nTilePixelX),
				// tile at vertical.
				if (lprcResizeArg->top < himgSrc->GetWidth())	// in picture
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest, nYDest + lprcResizeArg->top, nDestWidth, nDestLine1Height,
							memDC,
							0, lprcResizeArg->top, nDestWidth, nTilePixelY, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}

				// bottom
				nRet = StretchAlphaBlend(hdcDest,
						nXDest, nYDest + nDestLine2Top, nDestWidth, lprcResizeArg->bottom,
						memDC,
						0, himgSrc->GetHeight() - lprcResizeArg->bottom, nDestWidth, lprcResizeArg->bottom, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}
			}
			else if (nDestWidth > himgSrc->GetWidth() && nDestHeight <= himgSrc->GetHeight())
			{
				// left
				nRet = StretchAlphaBlend(hdcDest,
						nXDest, nYDest, lprcResizeArg->left, nDestHeight,
						memDC,
						0, 0, lprcResizeArg->left, nDestHeight, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}

				// center, not stretch at vertical, tile at horizontal.
				if (lprcResizeArg->left < himgSrc->GetWidth())	// in picture
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest + lprcResizeArg->left, nYDest, nDestCol1Width, nDestHeight,
							memDC,
							lprcResizeArg->left, 0, nTilePixelX, nDestHeight, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}

				// right
				nRet = StretchAlphaBlend(hdcDest,
						nXDest + nDestCol2Left, nYDest, lprcResizeArg->right, nDestHeight,
						memDC,
						himgSrc->GetWidth() - lprcResizeArg->right, 0, lprcResizeArg->right, nDestHeight, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}
			}
			else
			{
				// area 0
				if (lprcResizeArg->left > 0 && lprcResizeArg->top > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							nXDest, nYDest, lprcResizeArg->left, lprcResizeArg->top,
							memDC,
							0, 0, lprcResizeArg->left, lprcResizeArg->top, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 1
				if (lprcResizeArg->top > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest + lprcResizeArg->left, nYDest,
							nDestCol1Width, lprcResizeArg->top,
							memDC,
							lprcResizeArg->left, 0,
							nTilePixelX, lprcResizeArg->top, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 2
				if (lprcResizeArg->left < himgSrc->GetWidth() - 1 && lprcResizeArg->top > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							nXDest + nDestCol2Left, nYDest,
							lprcResizeArg->right, lprcResizeArg->top,
							memDC,
							himgSrc->GetWidth() - lprcResizeArg->right, 0,
							lprcResizeArg->right, lprcResizeArg->top, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}

				// area 3
				if (lprcResizeArg->left > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest, nYDest + lprcResizeArg->top,
							lprcResizeArg->left, nDestLine1Height,
							memDC,
							0, lprcResizeArg->top,
							lprcResizeArg->left, nTilePixelY, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 4
				// 平铺性能非常低，故改成缩放
				_ASSERT(nDestCol1Width > 0 && nDestLine1Height > 0);
				nRet = StretchAlphaBlend(hdcDest,
						nXDest + lprcResizeArg->left, nYDest + lprcResizeArg->top,
						nDestCol1Width, nDestLine1Height,
						memDC,
						lprcResizeArg->left, lprcResizeArg->top,
						nTilePixelX, nTilePixelY, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}
				// area 5
				if (lprcResizeArg->right > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest + nDestCol2Left, nYDest + lprcResizeArg->top,
							lprcResizeArg->right, nDestLine1Height,
							memDC,
							himgSrc->GetWidth() - lprcResizeArg->right, lprcResizeArg->top,
							lprcResizeArg->right, nTilePixelY, btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}

				// area 6
				if (lprcResizeArg->left > 0 && lprcResizeArg->bottom > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							nXDest, nYDest + nDestLine2Top,
							lprcResizeArg->left, lprcResizeArg->bottom,
							memDC,
							0, himgSrc->GetHeight() - lprcResizeArg->bottom,
							lprcResizeArg->left, lprcResizeArg->bottom,
							btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 7
				if (lprcResizeArg->bottom > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							nXDest + lprcResizeArg->left, nYDest + nDestLine2Top,
							nDestCol1Width, lprcResizeArg->bottom,
							memDC,
							lprcResizeArg->left, himgSrc->GetHeight() - lprcResizeArg->bottom,
							nTilePixelX, lprcResizeArg->bottom,
							btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 8
				if (lprcResizeArg->right > 0 && lprcResizeArg->bottom > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							nXDest + nDestCol2Left, nYDest + nDestLine2Top,
							lprcResizeArg->right, lprcResizeArg->bottom,
							memDC,
							himgSrc->GetWidth() - lprcResizeArg->right, himgSrc->GetHeight() - lprcResizeArg->bottom,
							lprcResizeArg->right, lprcResizeArg->bottom,
							btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
			}

			break;
		}

		::SelectObject(memDC, pSrcBmpOld);
		::DeleteDC(memDC);

		return nRet;
	}
	else
	{
		return NineGridBltG(hdcDest,
				nXDest, nYDest, nDestWidth, nDestHeight,
				himgSrc->GetSafeHBITMAP(), lprcResizeArg);
	}
}

int IUI::Render::IUINineGridBltEx(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE hbmpSrc, LPCRECT lprcResizeArg, COLORREF crColorKey, BYTE btSourceConstantAlpha)
{
	if (lprcDest == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}

	return IUINineGridBltEx(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hbmpSrc, lprcResizeArg, crColorKey, btSourceConstantAlpha);
}

int IUI::Render::IUIPartNineGridBlt(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, int nPart, int nPartIndex)
{
	return IUIPartNineGridBltEx(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
			himgSrc, lprcResizeArg, nPart, nPartIndex, -1, 255, FALSE);
}

int IUI::Render::IUIPartNineGridBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, int nPart, int nPartIndex)
{
	return IUIPartNineGridBltEx(hdcDest, lprcDest, himgSrc, lprcResizeArg, nPart, nPartIndex, -1, 255, FALSE);
}

int IUI::Render::IUIPartNineGridBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, LPCRECT lprcSrc)
{
	return IUIPartNineGridBltEx(hdcDest, lprcDest, himgSrc, lprcResizeArg, lprcSrc, -1, 255, FALSE);
}

// 与IUIPartNineGridBlt作用一样，仅仅需要重设一下resize point，设置成相对于截取部分
int IUI::Render::IUIPartNineGridBltResetResizePoint(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, LPCRECT lprcSrc)
{
	return IUIPartNineGridBltEx(hdcDest, lprcDest, himgSrc, lprcResizeArg, lprcSrc, -1, 255, TRUE);
}

int IUI::Render::IUIPartNineGridBltEx(HDC hdcDest, int nXDest, int nYDest,
	int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, LPCRECT lprcResizeArg, int nPart, int nPartIndex,
	COLORREF crColorKey/* = -1*/, BYTE btSourceConstantAlpha/* = 255*/,
	BOOL bResetResizePoint/* = FALSE*/)
{
	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -1;
	}
	if (nPart <= 0)
	{
		return -2;
	}
	if (nPartIndex < 0)
	{
		return -3;
	}
	if (nPartIndex >= nPart)
	{
		return -4;
	}
	if (lprcResizeArg == NULL)
	{
		return -5;
	}

	CIUIRect rcSrc;
	rcSrc.left = himgSrc->GetWidth() * nPartIndex / nPart;
	rcSrc.top = 0;
	rcSrc.right = rcSrc.left + himgSrc->GetWidth() / nPart;
	rcSrc.bottom = rcSrc.top + himgSrc->GetHeight();

	return IUIPartNineGridBltEx(hdcDest,
			CIUIRect(nXDest, nYDest, nXDest + nDestWidth, nYDest + nDestHeight),
			himgSrc, lprcResizeArg, rcSrc,
			crColorKey, btSourceConstantAlpha, bResetResizePoint);
}

int IUI::Render::IUIPartNineGridBltEx(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE hbmpSrc, LPCRECT lprcResizeArg, int nPart, int nPartIndex,
	COLORREF crColorKey/* = -1*/, BYTE btSourceConstantAlpha/* = 255*/,
	BOOL bResetResizePoint/* = FALSE*/)
{
	if (lprcDest == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}

	return IUIPartNineGridBltEx(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hbmpSrc, lprcResizeArg, nPart, nPartIndex,
			crColorKey, btSourceConstantAlpha, bResetResizePoint);
}

int IUI::Render::IUIPartNineGridBltEx(
	HDC hdcDest,
	LPCRECT lprcDest,
	HIUIIMAGE himgSrc,
	LPCRECT lprcResizeArg,
	LPCRECT lprcSrc,
	COLORREF crColorKey/* = -1*/, BYTE btSourceConstantAlpha/* = 255*/,
	BOOL bResetResizePoint/* = FALSE*/)
{
	if (himgSrc == NULL || lprcDest == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();

	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -1;
	}

	CIUIRect rcDest(lprcDest);

	CIUIRect rcSrc(lprcSrc);
	if (rcSrc.IsRectEmpty())
	{
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = himgSrc->GetWidth();
		rcSrc.bottom = himgSrc->GetHeight();
	}

	// 传入的缩放点，是相对于原图的。 换算成相对于part部分的图。
	CIUIRect rcResizeArg = *lprcResizeArg;

	int nTilePixelX = rcSrc.Width() - rcResizeArg.left - rcResizeArg.right;
	int nTilePixelY = rcSrc.Height() - rcResizeArg.top - rcResizeArg.bottom;

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC memDC = ::CreateCompatibleDC(hdcDest);
		HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, himgSrc->GetSafeHBITMAP());

		int nDestCol1Width = rcDest.Width() - rcResizeArg.left - rcResizeArg.right;
		int nDestLine1Height = rcDest.Height() - rcResizeArg.top - rcResizeArg.bottom;

		int nDestCol2Left = rcDest.Width() - rcResizeArg.right;
		int nDestLine2Top = rcDest.Height() - rcResizeArg.bottom;

		int nRet = -1;
		while (true)
		{
			// if the dest size less than source size, not stretch.
			if (rcDest.Width() <= rcSrc.Width() && rcDest.Height() <= rcSrc.Height())
			{
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
						memDC,
						rcSrc.left, rcSrc.top, rcDest.Width(), rcDest.Height(), btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}
			}
			else if (rcDest.Width() <= rcSrc.Width() && rcDest.Height() > rcSrc.Height())
			{
				// top
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left, rcDest.top, rcDest.Width(), rcResizeArg.top,
						memDC,
						rcSrc.left, rcSrc.top, rcDest.Width(), rcResizeArg.top, btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}

				// center, not stretch at horizontal(ignore rcResizeArg.left and nTilePixelX),
				// tile at vertical.
				if (rcResizeArg.top < rcSrc.Height())	// in picture
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left, rcDest.top + rcResizeArg.top, rcDest.Width(), nDestLine1Height,
							memDC,
							rcSrc.left, rcSrc.top + rcResizeArg.top, rcDest.Width(), nTilePixelY,
							btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}

				// bottom
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left, rcDest.top + nDestLine2Top, rcDest.Width(), rcResizeArg.bottom,
						memDC,
						rcSrc.left, rcSrc.top + himgSrc->GetHeight() - rcResizeArg.bottom, rcDest.Width(), rcResizeArg.bottom,
						btSourceConstantAlpha);

				if (nRet != 0)
				{
					break;
				}
			}
			else if (rcDest.Width() > rcSrc.Width() && rcDest.Height() <= rcSrc.Height())
			{
				// left
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left, rcDest.top, rcResizeArg.left, rcDest.Height(),
						memDC,
						rcSrc.left, rcSrc.top, rcResizeArg.left, rcDest.Height(),
						btSourceConstantAlpha);
				if (nRet != 0)
				{
					break;
				}

				// center, not stretch at vertical, tile at horizontal.
				if (rcResizeArg.left < rcSrc.Width())	// in picture
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left + rcResizeArg.left, rcDest.top, nDestCol1Width, rcDest.Height(),
							memDC,
							rcSrc.left + rcResizeArg.left, rcSrc.top, nTilePixelX, rcDest.Height(),
							btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}

				// right
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left + nDestCol2Left, rcDest.top, rcResizeArg.right, rcDest.Height(),
						memDC,
						rcSrc.left + rcSrc.Width() - rcResizeArg.right, rcSrc.top,
						rcResizeArg.right, rcDest.Height(),
						btSourceConstantAlpha);

				if (nRet != 0)
				{
					break;
				}
			}
			else
			{
				// area 0
				if (rcResizeArg.left > 0 && rcResizeArg.top > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							rcDest.left, rcDest.top, rcResizeArg.left, rcResizeArg.top,
							memDC, rcSrc.left, rcSrc.top, rcResizeArg.left, rcResizeArg.top,
							btSourceConstantAlpha);
					if (nRet != 0)
					{
						break;
					}
				}
				// area 1
				if (rcResizeArg.top > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left + rcResizeArg.left, rcDest.top, nDestCol1Width, rcResizeArg.top,
							memDC,
							rcSrc.left + rcResizeArg.left, rcSrc.top, nTilePixelX, rcResizeArg.top,
							btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}
				// area 2
				if (rcResizeArg.left < rcSrc.Width() - 1 && rcResizeArg.top > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							rcDest.left + nDestCol2Left, rcDest.top, rcResizeArg.right, rcResizeArg.top,
							memDC,
							rcSrc.left + rcSrc.Width() - rcResizeArg.right, rcSrc.top, rcResizeArg.right, rcResizeArg.top,
							btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}

				// area 3
				if (rcResizeArg.left > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left, rcDest.top + rcResizeArg.top, rcResizeArg.left, nDestLine1Height,
							memDC, rcSrc.left, rcSrc.top + rcResizeArg.top, rcResizeArg.left, nTilePixelY,
							btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}
				// area 4
				_ASSERT(nDestCol1Width > 0 && nDestLine1Height > 0);
				// TODO: 平铺操作很慢，换成缩放
				//nRet = AlphaTileBlt(hdcDest,
				//		rcDest.left + rcResizeArg.left, rcDest.top + rcResizeArg.top,
				//		nDestCol1Width, nDestLine1Height,
				//		memDC,
				//		rcSrc.left + rcResizeArg.left, rcSrc.top + rcResizeArg.top, nTilePixelX, nTilePixelY,
				//		btSourceConstantAlpha);
				nRet = StretchAlphaBlend(hdcDest,
						rcDest.left + rcResizeArg.left, rcDest.top + rcResizeArg.top,
						nDestCol1Width, nDestLine1Height,
						memDC,
						rcSrc.left + rcResizeArg.left, rcSrc.top + rcResizeArg.top, nTilePixelX, nTilePixelY,
						btSourceConstantAlpha);

				if (nRet != 0)
				{
					break;
				}
				// area 5
				if (rcResizeArg.right > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left + nDestCol2Left, rcDest.top + rcResizeArg.top,
							rcResizeArg.right, nDestLine1Height,
							memDC,
							rcSrc.left + rcSrc.Width() - rcResizeArg.right, rcSrc.top + rcResizeArg.top,
							rcResizeArg.right, nTilePixelY, btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}

				// area 6
				if (rcResizeArg.left > 0 && rcResizeArg.bottom > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							rcDest.left, rcDest.top + nDestLine2Top, rcResizeArg.left, rcResizeArg.bottom,
							memDC,
							rcSrc.left, rcSrc.top + rcSrc.Height() - rcResizeArg.bottom,
							rcResizeArg.left, rcResizeArg.bottom,
							btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}
				// area 7
				if (rcResizeArg.bottom > 0)
				{
					nRet = AlphaTileBlt(hdcDest,
							rcDest.left + rcResizeArg.left, rcDest.top + nDestLine2Top,
							nDestCol1Width, rcResizeArg.bottom,
							memDC,
							rcSrc.left + rcResizeArg.left, rcSrc.top + rcSrc.Height() - rcResizeArg.bottom,
							nTilePixelX, rcResizeArg.bottom, btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}
				// area 8
				if (rcResizeArg.right > 0 && rcResizeArg.bottom > 0)
				{
					nRet = StretchAlphaBlend(hdcDest,
							rcDest.left + nDestCol2Left, rcDest.top + nDestLine2Top,
							rcResizeArg.right, rcResizeArg.bottom,
							memDC,
							rcSrc.left + rcSrc.Width() - rcResizeArg.right,
							rcSrc.top + rcSrc.Height() - rcResizeArg.bottom,
							rcResizeArg.right, rcResizeArg.bottom, btSourceConstantAlpha);

					if (nRet != 0)
					{
						break;
					}
				}
			}

			break;
		}

		::SelectObject(memDC, pSrcBmpOld);
		::DeleteDC(memDC);

		return nRet;
	}
	else
	{
		return PartNineGridBltG(hdcDest, rcDest,
				himgSrc->GetSafeHBITMAP(),
				rcSrc, rcResizeArg);
	}
}

int IUI::Render::IUIStretchBlt(HDC hdcDest,
	int nXDest,
	int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc,
	int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight)
{
	if (hdcDest == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	::SetStretchBltMode(hdcDest, COLORONCOLOR);

	HDC memDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, (HBITMAP)(*himgSrc));

	if (himgSrc->IsHasAlphaChannel())
	{
		StretchAlphaBlend(hdcDest,
			nXDest, nYDest, nDestWidth, nDestHeight,
			memDC,
			nXSrc, nYSrc, nSrcWidth, nSrcHeight, 255);
	}
	else
	{
		::StretchBlt(hdcDest,
			nXDest, nYDest, nDestWidth, nDestHeight,
			memDC,
			nXSrc, nYSrc, nSrcWidth, nSrcHeight, SRCCOPY);
	}

	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::IUIStretchBlt(HDC hdcDest,
	int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc)
{
	if (hdcDest == NULL)
	{
		return -1;
	}

	if (himgSrc == NULL)
	{
		return -2;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (nDestWidth <= 0 || nDestHeight <= 0)
	{
		return -4;
	}

	return IUIStretchBlt(hdcDest,
			nXDest, nYDest, nDestWidth, nDestHeight,
			himgSrc,
			0, 0, himgSrc->GetWidth(), himgSrc->GetHeight());
}

int IUI::Render::IUIStretchBlt(HDC hdcDest, LPCRECT lprcDest, HIUIIMAGE himgSrc)
{
	if (hdcDest == NULL)
	{
		return -1;
	}

	if (himgSrc == NULL)
	{
		return -2;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	CIUIRect rcDest(lprcDest);
	if (rcDest.IsRectEmpty() || rcDest.IsRectNull())
	{
		return -4;
	}

	return IUIStretchBlt(hdcDest,
			rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), himgSrc);
}

int IUI::Render::IUIAlphaStretchBlt(HDC hdcDest,
	int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, BYTE btSourceConstantAlpha, IMAGE_RESIZE_MODE eStretchQuality)
{
	if (hdcDest == NULL || himgSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (eStretchQuality == IRM_STRETCH_HIGH_QUALITY)
	{
		Gdiplus::Graphics g(hdcDest);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		Gdiplus::Bitmap *p = CreateBitmapFromHBITMAP(himgSrc->GetSafeHBITMAP());
		Gdiplus::Rect rc(nXDest, nYDest, nDestWidth, nDestHeight);
		g.DrawImage(p, rc);

		::delete p;
		p = NULL;
	}
	else
	{
		::SetStretchBltMode(hdcDest, COLORONCOLOR);

		HDC memDC = ::CreateCompatibleDC(hdcDest);
		HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, (HBITMAP)(*himgSrc));

		if (himgSrc->IsHasAlphaChannel())
		{
			if (himgSrc->IsGif())
			{
				int nX = himgSrc->GetWidth() * himgSrc->GetGifCurFrame();
				StretchAlphaBlend(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
					memDC, nX, 0, himgSrc->GetWidth(), himgSrc->GetHeight(), btSourceConstantAlpha);
			}
			else
			{
				StretchAlphaBlend(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
					memDC, 0, 0, himgSrc->GetWidth(), himgSrc->GetHeight(), btSourceConstantAlpha);
			}
		}
		else
		{
			if (himgSrc->IsGif())
			{
				int nX = himgSrc->GetWidth() * himgSrc->GetGifCurFrame();
				::StretchBlt(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
					memDC, nX, 0, himgSrc->GetWidth(), himgSrc->GetHeight(), SRCCOPY);
			}
			else
			{
				::StretchBlt(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
					memDC, 0, 0, himgSrc->GetWidth(), himgSrc->GetHeight(), SRCCOPY);
			}
		}

		::SelectObject(memDC, pSrcBmpOld);
		::DeleteDC(memDC);
	}

	return 0;
}

int IUI::Render::IUIAlphaStretchBlt(HDC hdcDest,
	LPCRECT lprcDest, HIUIIMAGE himgSrc, BYTE btSourceConstantAlpha,
	IMAGE_RESIZE_MODE eStretchQuality)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	return IUIAlphaStretchBlt(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			himgSrc, btSourceConstantAlpha, eStretchQuality);
}

int IUI::Render::IUIPartStretchBlt(HDC hdcDest, int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc, int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight,
	IMAGE_RESIZE_MODE eStretchQuality)
{
	if (hdcDest == NULL || himgSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	::SetStretchBltMode(hdcDest, COLORONCOLOR);

	HDC memDC = ::CreateCompatibleDC(hdcDest);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(memDC, (HBITMAP)(*himgSrc));

	if (himgSrc->IsHasAlphaChannel())
	{
		StretchAlphaBlend(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
			memDC, nXSrc, nYSrc, nSrcWidth, nSrcHeight, 255);
	}
	else
	{
		::StretchBlt(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
			memDC, nXSrc, nYSrc, nSrcWidth, nSrcHeight, SRCCOPY);
	}

	::SelectObject(memDC, pSrcBmpOld);
	::DeleteDC(memDC);

	return 0;
}

int IUI::Render::IUIPartStretchBlt(HDC hdcDest, LPCRECT lprcDest, HIUIIMAGE himgSrc, LPCRECT lprcSrc,
	IMAGE_RESIZE_MODE eStretchQuality)
{
	if (hdcDest == NULL || lprcDest == NULL || himgSrc == NULL || lprcSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	return IUIPartStretchBlt(hdcDest,
			lprcDest->left,
			lprcDest->top,
			lprcDest->right - lprcDest->left,
			lprcDest->bottom - lprcDest->top,
			himgSrc,
			lprcSrc->left,
			lprcSrc->top,
			lprcSrc->right - lprcSrc->left,
			lprcSrc->bottom - lprcSrc->top,
			eStretchQuality);
}

int IUI::Render::IUIPartStretchBlt(HDC hdcDest,
	int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE himgSrc,
	int nPart, int nPartIndex, IMAGE_RESIZE_MODE eStretchQuality)
{
	if (hdcDest == NULL || himgSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -2;
	}

	if (nPart <= 0)
	{
		return -3;
	}
	if (nPartIndex < 0)
	{
		return -4;
	}
	if (nPartIndex >= nPart)
	{
		return -5;
	}

	int nSrcWidth = himgSrc->GetWidth() / nPart;

	return IUIPartStretchBlt(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
			himgSrc, nSrcWidth * nPartIndex, 0, nSrcWidth, himgSrc->GetHeight(),
			eStretchQuality);
}

int IUI::Render::IUIPartStretchBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE himgSrc, int nPart, int nPartIndex, IMAGE_RESIZE_MODE eStretchQuality)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	return IUIPartStretchBlt(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			himgSrc,
			nPart, nPartIndex, eStretchQuality);
}

int IUI::Render::IUITileBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE himgSrc,
	int nXOriginSrc/* = 0*/, int nYOriginSrc/* = 0*/,
	int nSrcWidth/* = -1*/, int nSrcHeight/* = -1*/)
{
	if (himgSrc == NULL || lprcDest == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	HDC hdcMem = CreateCompatibleDC(hdcDest);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, (HBITMAP)(*himgSrc));

	if (nSrcWidth == -1)
	{
		nSrcWidth = himgSrc->GetWidth();
	}
	if (nSrcHeight == -1)
	{
		nSrcHeight = himgSrc->GetHeight();
	}

	if (himgSrc->IsHasAlphaChannel())
	{
		AlphaTileBlt(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hdcMem, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, 255);
	}
	else
	{
		TileBlt(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hdcMem, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, 255);
	}

	SelectObject(hdcMem, hbmpOld);
	DeleteDC(hdcMem);

	return 0;
}

int IUI::Render::IUICenterBitBlt(HDC hDC, LPCRECT lprcDest, HIUIIMAGE himgSrc, DWORD dwRop)
{
	if (lprcDest == NULL || himgSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	CIUIRect rcNewDest;
	rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - himgSrc->GetWidth()) / 2;
	rcNewDest.right = rcNewDest.left + himgSrc->GetWidth();
	rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - himgSrc->GetHeight()) / 2;
	rcNewDest.bottom = rcNewDest.top + himgSrc->GetHeight();

	return IUIBitBlt(hDC,
			rcNewDest.left, rcNewDest.top, rcNewDest.Width(), rcNewDest.Height(),
			himgSrc, 0, 0, dwRop);
}

int IUI::Render::IUIBitBlt(HDC hdcDest,
	int nXDest, int nYDest, int nDestWidth, int nDestHeight,
	HIUIIMAGE hbmpSrc,
	int nXSrc, int nYSrc, DWORD dwRop)
{
	if (hbmpSrc == NULL)
	{
		return -1;
	}

	hbmpSrc->SafeLoadSavedImage();
	if (hbmpSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	_ASSERT(hdcDest != NULL);

	if (hbmpSrc->IsHasAlphaChannel())
	{
		HDC hdcMem = CreateCompatibleDC(hdcDest);

		HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, (HBITMAP)(*hbmpSrc));
		int nRet = StretchAlphaBlend(hdcDest,
				nXDest, nYDest, nDestWidth, nDestHeight,
				hdcMem,
				nXSrc, nYSrc, nDestWidth, nDestHeight, 255);
		SelectObject(hdcMem, hbmpOld);

		DeleteDC(hdcMem);

		return nRet;
	}
	else
	{
		return BitBltG(hdcDest, nXDest, nYDest, nDestWidth, nDestHeight,
				*hbmpSrc, nXSrc, nYSrc, dwRop);
	}
}

int IUI::Render::IUIBitBlt(HDC hdcDest, LPCRECT lprcDest,
	HIUIIMAGE hSrcBmp, int nXSrc, int nYSrc, DWORD dwRop)
{
	if (lprcDest == NULL)
	{
		return -1;
	}

	return IUIBitBlt(hdcDest,
			lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hSrcBmp, nXSrc, nYSrc, dwRop);
}

int IUI::Render::IUITileClipImage(
	Gdiplus::Graphics *pGraphics,
	Gdiplus::GraphicsPath *pDestClipPath,
	Gdiplus::Image *pSrcImage, LPCRECT lprcSrc)
{
	if (pGraphics == NULL
		|| pDestClipPath == NULL
		|| pSrcImage == NULL)
	{
		return -1;
	}

	pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	// constructe a TextureBrush object need long time.
	// If you need tile the same TextureBrush many times,
	// you can call Graphics::FillPath directly, instead of IUITileClipImage.
	Gdiplus::TextureBrush *pTextureBrush = NULL;
	if (lprcSrc != NULL)
	{
		Gdiplus::Rect rect(
			lprcSrc->left,
			lprcSrc->top,
			lprcSrc->right - lprcSrc->left,
			lprcSrc->bottom - lprcSrc->top);

#ifdef _DEBUG
#ifdef new
#undef new
#endif // new
#endif // _DEBUG
		pTextureBrush = new Gdiplus::TextureBrush(pSrcImage, rect);
#ifdef _DEBUG
#define new IUI_DEBUG_NEW
#endif // _DEBUG
	}
	else
	{
#ifdef _DEBUG
#ifdef new
#undef new
#endif // new
#endif // _DEBUG
		pTextureBrush = new Gdiplus::TextureBrush(pSrcImage);
#ifdef _DEBUG
#define new IUI_DEBUG_NEW
#endif // _DEBUG
	}

	pTextureBrush->TranslateTransform(0, 0);
	pGraphics->FillPath(pTextureBrush, pDestClipPath);

	if (NULL != pTextureBrush)
	{
		delete pTextureBrush;
		pTextureBrush = NULL;
	}
#ifdef _DEBUG
#define new IUI_DEBUG_NEW
#endif

	return 0;
}

int IUI::Render::IUIDrawForeground(HDC hDstDC, LPCRECT lprcClient, LPCRECT lprcForegroundMargin,
	HIUIIMAGE himgFg,
	FOREGROUND_ALIGN_HOR eForegroundAlignHor, FOREGROUND_ALIGN_VER eForegroundAlignVer,
	COLORREF crMask, BYTE btFgImgTransparent/* = 255*/)
{
	if (hDstDC == NULL
		|| lprcClient == NULL
		|| himgFg == NULL)
	{
		return -1;
	}

	himgFg->SafeLoadSavedImage();
	if (himgFg->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	CIUIRect rcControl(lprcClient);
	if (lprcForegroundMargin != NULL)
	{
		rcControl.DeflateRect(lprcForegroundMargin);
	}

	int nX = rcControl.left;
	int nY = rcControl.top;
	int nDestWidth = himgFg->GetWidth();
	int nDestHeight = himgFg->GetHeight();

	if (eForegroundAlignHor == FAH_CENTER) // center
	{
		// If the dest rect less then image, stretch it.
		int nOffset = (rcControl.Width() - himgFg->GetWidth()) / 2;
		if (nOffset < 0)
		{
			nOffset = 0;

			nDestWidth = rcControl.Width();
		}
		nX += nOffset;
	}
	else if (eForegroundAlignHor == FAH_LEFT)
	{
		// Keep.
	}
	else if (eForegroundAlignHor == FAH_STRETCH)
	{
		nDestWidth = rcControl.Width();
	}
	else if (eForegroundAlignHor == FAH_RIGHT) // right
	{
		nX += (rcControl.Width() - himgFg->GetWidth());
	}
	else
	{
		_ASSERT(FALSE);
	}

	if (eForegroundAlignVer == FAV_CENTER) // center
	{
		// If the dest rect less then image, stretch it.
		int nOffset = (rcControl.Height() - himgFg->GetHeight()) / 2;
		if (nOffset < 0)
		{
			nOffset = 0;

			nDestHeight = rcControl.Height();
		}
		nY += nOffset;
	}
	else if (eForegroundAlignVer == FAV_TOP)
	{
		// Keep.
	}
	else if (eForegroundAlignVer == FAV_STRETCH)
	{
		nDestHeight = rcControl.Height();
	}
	else if (eForegroundAlignVer == FAV_BOTTOM) // bottom
	{
		nY += (rcControl.Height() - himgFg->GetHeight());
	}
	else
	{
		_ASSERT(FALSE);
	}

	HDC hMemDC = ::CreateCompatibleDC(hDstDC);
	HBITMAP hbmpOld = (HBITMAP)::SelectObject(hMemDC, himgFg->GetSafeHBITMAP());

	if (himgFg->IsHasAlphaChannel())
	{
		StretchAlphaBlend(hDstDC, nX, nY, nDestWidth, nDestHeight,
			hMemDC,	0, 0, himgFg->GetWidth(), himgFg->GetHeight(), btFgImgTransparent);
	}
	else
	{
		TransparentBlt2(hDstDC, nX, nY, himgFg->GetWidth(), himgFg->GetHeight(),
			hMemDC, 0, 0, himgFg->GetWidth(), himgFg->GetHeight(), crMask);
	}

	::SelectObject(hMemDC, hbmpOld);
	DeleteDC(hMemDC);

	return 0;
}

int IUI::Render::IUIPartDrawForeground(HDC hDstDC,
	LPCRECT lprcDest, LPCRECT lprcForegroundMargin,
	HIUIIMAGE himgFg, LPCRECT lprcFgPart,
	FOREGROUND_ALIGN_HOR eForegroundAlignHor, FOREGROUND_ALIGN_VER eForegroundAlignVer,
	COLORREF crMask, BYTE btSourceConstantAlpha/* = 255*/)
{
	if (hDstDC == NULL || lprcDest == NULL)
	{
		return -1;
	}

	if (himgFg->GetSafeHBITMAP() == NULL)
	{
		himgFg->SafeLoadSavedImage();
	}

	if (himgFg->GetSafeHBITMAP() == NULL)
	{
		return -1;
	}

	CIUIRect rcControl(lprcDest);
	if (lprcForegroundMargin != NULL)
	{
		rcControl.DeflateRect(lprcForegroundMargin);
	}

	CIUIRect rcFgPart(lprcFgPart);
	if (rcFgPart.IsRectEmpty())
	{
		rcFgPart.left = 0;
		rcFgPart.top = 0;
		rcFgPart.right = himgFg->GetWidth();
		rcFgPart.bottom = himgFg->GetHeight();
	}

	int nX = rcControl.left;
	int nY = rcControl.top;
	int nDestWidth = rcFgPart.Width();
	int nDestHeight = rcFgPart.Height();

	if (eForegroundAlignHor == FAH_CENTER) // center
	{
		// If the dest rect less then image, stretch it.
		int nOffset = (rcControl.Width() - nDestWidth) / 2;
		if (nOffset < 0)
		{
			nOffset = 0;

			nDestWidth = rcControl.Width();
		}
		nX += nOffset;
	}
	else if (eForegroundAlignHor == FAH_LEFT)
	{
		// Keep.
	}
	else if (eForegroundAlignHor == FAH_STRETCH)
	{
		nDestWidth = rcControl.Width();
	}
	else if (eForegroundAlignHor == FAH_RIGHT) // right
	{
		nX += (rcControl.Width() - nDestWidth);
	}
	else
	{
		_ASSERT(FALSE);
	}

	if (eForegroundAlignVer == FAV_CENTER) // center
	{
		// If the dest rect less then image, stretch it.
		int nOffset = (rcControl.Height() - nDestHeight) / 2;
		if (nOffset < 0)
		{
			nOffset = 0;

			nDestHeight = rcControl.Height();
		}
		nY += nOffset;
	}
	else if (eForegroundAlignVer == FAV_TOP)
	{
		// Keep.
	}
	else if (eForegroundAlignVer == FAV_STRETCH)
	{
		nDestHeight = rcControl.Height();
	}
	else if (eForegroundAlignVer == FAV_BOTTOM) // bottom
	{
		nY += (rcControl.Height() - nDestHeight);
	}
	else
	{
		_ASSERT(FALSE);
	}

	IUIPartStretchBlt(hDstDC,
		CIUIRect(nX, nY, nX + nDestWidth, nY + nDestHeight),
		himgFg, rcFgPart, IRM_STRETCH);

	return 0;
}

int IUI::Render::IUIDrawImage(HDC hDstDC, LPCRECT lprcDest,
	HIUIIMAGE hImage, IMAGE_RESIZE_MODE eImageResizeMode, LPCRECT lprcResizeArg)
{
	if (hDstDC == NULL || lprcDest == NULL)
	{
		return -1;
	}
	if (hImage == NULL)
	{
		return -2;
	}

	hImage->SafeLoadSavedImage();
	if (hImage->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (eImageResizeMode == IRM_9GRID)
	{
		IUINineGridBlt(hDstDC, lprcDest, hImage, lprcResizeArg);
	}
	else if (eImageResizeMode == IRM_STRETCH
		|| eImageResizeMode == IRM_STRETCH_HIGH_QUALITY)
	{
		IUIAlphaStretchBlt(hDstDC, lprcDest, hImage, 255, eImageResizeMode);
	}
	else if (eImageResizeMode == IRM_CENTER)
	{
		int nImageWidth = hImage->GetWidth();
		int nImageHeight = hImage->GetHeight();

		CIUIRect rcNewDest;
		rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - nImageWidth) / 2;
		rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - nImageHeight) / 2;
		rcNewDest.right = rcNewDest.left + nImageWidth;
		rcNewDest.bottom = rcNewDest.top + nImageHeight;

		IUIAlphaStretchBlt(hDstDC, rcNewDest, hImage, 255, eImageResizeMode);
	}
	else if (eImageResizeMode == IRM_KEEPRATE)
	{
		int nImageWidth = hImage->GetWidth();
		int nImageHeight = hImage->GetHeight();

		// 相当于居中
		if (RECTWIDTH(*lprcDest) >= nImageWidth
			&& RECTHEIGHT(*lprcDest) >= nImageHeight)
		{
			IUIDrawImage(hDstDC, lprcDest, hImage, IRM_CENTER, lprcResizeArg);
		}
		else
		{
			// 找到以哪条边为基准来缩放
			BOOL bWidthBase = TRUE;	// 是否以宽边为缩放基准
			if (nImageWidth > RECTWIDTH(*lprcDest) && nImageHeight > RECTHEIGHT(*lprcDest))
			{
				// 图像的两边都大于目标的两边
				if (nImageWidth * RECTHEIGHT(*lprcDest) >= nImageHeight * RECTWIDTH(*lprcDest))
				{
					bWidthBase = TRUE;
				}
				else
				{
					bWidthBase = FALSE;
				}
			}
			else if (nImageWidth > RECTWIDTH(*lprcDest))
			{
				// 只有图像的宽边大于目标的宽边
				bWidthBase = TRUE;
			}
			else
			{
				// 只有图像的高边大于目标的高度
				bWidthBase = FALSE;
			}

			int nDestWidth = 0;
			int nDestHeight = 0;
			if (bWidthBase)
			{
				nDestWidth = RECTWIDTH(*lprcDest);
				nDestHeight = MulDiv(nImageHeight, RECTWIDTH(*lprcDest), nImageWidth);
			}
			else
			{
				nDestWidth = MulDiv(nImageWidth, RECTHEIGHT(*lprcDest), nImageHeight);
				nDestHeight = RECTHEIGHT(*lprcDest);
			}

			CIUIRect rcNewDest;
			rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - nDestWidth) / 2;
			rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - nDestHeight) / 2;
			rcNewDest.right = rcNewDest.left + nDestWidth;
			rcNewDest.bottom = rcNewDest.top + nDestHeight;

			IUIAlphaStretchBlt(hDstDC, rcNewDest, hImage, 255, eImageResizeMode);
		}
	}
	else if (eImageResizeMode == IRM_TILE)
	{
		IUITileBlt(hDstDC, lprcDest, hImage, 0, 0, hImage->GetWidth(), hImage->GetHeight());
	}

	return 0;
}

int IUI::Render::IUIPartDrawImage(HDC hDstDC, LPCRECT lprcDest,
	HIUIIMAGE hImage, IMAGE_RESIZE_MODE eImageResizeMode, LPCRECT lprcResizeArg,
	LPCRECT lprcSrc)
{
	if (hDstDC == NULL || lprcDest == NULL)
	{
		return -1;
	}
	if (hImage == NULL)
	{
		return -2;
	}

	hImage->SafeLoadSavedImage();
	if (hImage->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	CIUIRect rcSrc(lprcSrc);
	if (rcSrc.IsRectEmpty())
	{
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = hImage->GetWidth();
		rcSrc.bottom = hImage->GetHeight();
	}

	if (eImageResizeMode == IRM_9GRID)
	{
		IUIPartNineGridBlt(hDstDC, lprcDest, hImage, lprcResizeArg, lprcSrc);
	}
	else if (eImageResizeMode == IRM_STRETCH
		|| eImageResizeMode == IRM_STRETCH_HIGH_QUALITY)
	{
		IUIStretchBlt(hDstDC, lprcDest->left, lprcDest->top, RECTWIDTH(*lprcDest), RECTHEIGHT(*lprcDest),
			hImage, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
	}
	else if (eImageResizeMode == IRM_CENTER)
	{
		int nImageWidth = rcSrc.Width();
		int nImageHeight = rcSrc.Height();

		CIUIRect rcNewDest;
		rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - nImageWidth) / 2;
		rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - nImageHeight) / 2;
		rcNewDest.right = rcNewDest.left + nImageWidth;
		rcNewDest.bottom = rcNewDest.top + nImageHeight;

		IUIStretchBlt(hDstDC, rcNewDest.left, rcNewDest.top, rcNewDest.Width(), rcNewDest.Height(),
			hImage, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
	}
	else if (eImageResizeMode == IRM_KEEPRATE)
	{
		int nImageWidth = hImage->GetWidth();
		int nImageHeight = hImage->GetHeight();

		// 相当于居中
		if (RECTWIDTH(*lprcDest) >= nImageWidth
			&& RECTHEIGHT(*lprcDest) >= nImageHeight)
		{
			IUIDrawImage(hDstDC, lprcDest, hImage, IRM_CENTER, lprcResizeArg);
		}
		else
		{
			// 找到以哪条边为基准来缩放
			BOOL bWidthBase = TRUE;	// 是否以宽边为缩放基准
			if (nImageWidth > RECTWIDTH(*lprcDest) && nImageHeight > RECTHEIGHT(*lprcDest))
			{
				// 图像的两边都大于目标的两边
				if (nImageWidth * RECTHEIGHT(*lprcDest) >= nImageHeight * RECTWIDTH(*lprcDest))
				{
					bWidthBase = TRUE;
				}
				else
				{
					bWidthBase = FALSE;
				}
			}
			else if (nImageWidth > RECTWIDTH(*lprcDest))
			{
				// 只有图像的宽边大于目标的宽边
				bWidthBase = TRUE;
			}
			else
			{
				// 只有图像的高边大于目标的高度
				bWidthBase = FALSE;
			}

			int nDestWidth = 0;
			int nDestHeight = 0;
			if (bWidthBase)
			{
				nDestWidth = RECTWIDTH(*lprcDest);
				nDestHeight = MulDiv(nImageHeight, RECTWIDTH(*lprcDest), nImageWidth);
			}
			else
			{
				nDestWidth = MulDiv(nImageWidth, RECTHEIGHT(*lprcDest), nImageHeight);
				nDestHeight = RECTHEIGHT(*lprcDest);
			}

			CIUIRect rcNewDest;
			rcNewDest.left = lprcDest->left + (RECTWIDTH(*lprcDest) - nDestWidth) / 2;
			rcNewDest.top = lprcDest->top + (RECTHEIGHT(*lprcDest) - nDestHeight) / 2;
			rcNewDest.right = rcNewDest.left + nDestWidth;
			rcNewDest.bottom = rcNewDest.top + nDestHeight;

			IUIStretchBlt(hDstDC, rcNewDest.left, rcNewDest.top, rcNewDest.Width(), rcNewDest.Height(),
				hImage, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());
		}
	}

	return 0;
}

int IUI::Render::IUIPartDrawImage(HDC hDstDC, LPCRECT lprcDest,
	HIUIIMAGE hImage, IMAGE_RESIZE_MODE eImageResizeMode, LPCRECT lprcResizeArg,
	int nPartCount, int nPartIndex)
{
	if (hDstDC == NULL || lprcDest == NULL || hImage == NULL || nPartCount <= 0)
	{
		return -1;
	}

	hImage->SafeLoadSavedImage();

	if (hImage->GetSafeHBITMAP() == NULL)
	{
		return -2;
	}

	CIUIRect rcSrc;
	rcSrc.left = hImage->GetWidth() * nPartIndex / nPartCount;
	rcSrc.right = rcSrc.left + hImage->GetWidth() / nPartCount;
	rcSrc.bottom = hImage->GetHeight();

	return IUIPartDrawImage(hDstDC, lprcDest, hImage, eImageResizeMode, lprcResizeArg, rcSrc);
}

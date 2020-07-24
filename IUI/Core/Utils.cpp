#include "stdafx.h"

#ifndef DEBUG_RICHEDIT
// 在调试微软RichEdit代码时，如果宏定义new，程序会崩溃。
#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG
#endif // DEBUG_RICHEDIT

namespace IUI
{
	CIUISize::CIUISize()
	{
		cx = cy = 0;
	}

	CIUISize::CIUISize(const SIZE &src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CIUISize::CIUISize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CIUISize::CIUISize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}

	CIUISize::CIUISize(POINT initPt)
	{
		*(POINT *)this = initPt;
	}

	CIUISize::CIUISize(DWORD dwSize)
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}

	//////////////////////////////////////////////////////////////////////////
	// CIUIRect
	CIUIRect::CIUIRect() throw()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	CIUIRect::CIUIRect(int l, int t, int r, int b) throw()
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	CIUIRect::CIUIRect(const RECT &srcRect) throw()
	{
		::CopyRect(this, &srcRect);
	}

	CIUIRect::CIUIRect(LPCRECT lpSrcRect) throw()
	{
		::CopyRect(this, lpSrcRect);
	}

	CIUIRect::CIUIRect(POINT point, SIZE size) throw()
	{
		right = (left = point.x) + size.cx;
		bottom = (top = point.y) + size.cy;
	}

	CIUIRect::CIUIRect(POINT topLeft, POINT bottomRight) throw()
	{
		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}

	int CIUIRect::Width() const throw()
	{
		return right - left;
	}

	int CIUIRect::Height() const throw()
	{
		return bottom - top;
	}

	CIUISize CIUIRect::Size() const throw()
	{
		return CIUISize(right - left, bottom - top);
	}

	CIUIPoint &CIUIRect::TopLeft() throw()
	{
		return *((CIUIPoint *)this);
	}

	CIUIPoint &CIUIRect::BottomRight() throw()
	{
		return *((CIUIPoint *)this + 1);
	}

	const CIUIPoint &CIUIRect::TopLeft() const throw()
	{
		return *((CIUIPoint *)this);
	}

	const CIUIPoint &CIUIRect::BottomRight() const throw()
	{
		return *((CIUIPoint *)this + 1);
	}

	CIUIPoint CIUIRect::CenterPoint() const throw()
	{
		return CIUIPoint((left + right) / 2, (top + bottom) / 2);
	}

	void CIUIRect::SwapLeftRight() throw()
	{
		SwapLeftRight(LPRECT(this));
	}

	void CIUIRect::SwapLeftRight(LPRECT lpRect) throw()
	{
		LONG temp = lpRect->left;
		lpRect->left = lpRect->right;
		lpRect->right = temp;
	}

	CIUIRect::operator LPRECT() throw()
	{
		return this;
	}

	CIUIRect::operator LPCRECT() const throw()
	{
		return this;
	}

	BOOL CIUIRect::IsRectEmpty() const throw()
	{
		return ::IsRectEmpty(this);
	}

	BOOL CIUIRect::IsRectNull() const throw()
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

	BOOL CIUIRect::PtInRect(POINT point) const throw()
	{
		return ::PtInRect(this, point);
	}

	void CIUIRect::SetRect(int x1, int y1, int x2, int y2) throw()
	{
		::SetRect(this, x1, y1, x2, y2);
	}

	void CIUIRect::SetRect(POINT topLeft, POINT bottomRight) throw()
	{
		::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	}

	void CIUIRect::SetRectEmpty() throw()
	{
		::SetRectEmpty(this);
	}

	void CIUIRect::CopyRect(LPCRECT lpSrcRect) throw()
	{
		::CopyRect(this, lpSrcRect);
	}

	BOOL CIUIRect::EqualRect(LPCRECT lpRect) const throw()
	{
		return ::EqualRect(this, lpRect);
	}

	void CIUIRect::InflateRect(int x, int y) throw()
	{
		::InflateRect(this, x, y);
	}

	void CIUIRect::InflateRect(SIZE size) throw()
	{
		::InflateRect(this, size.cx, size.cy);
	}

	void CIUIRect::InflateRect(LPCRECT lpRect) throw()
	{
		left -= lpRect->left;
		top -= lpRect->top;
		right += lpRect->right;
		bottom += lpRect->bottom;
	}

	void CIUIRect::InflateRect(int l, int t, int r, int b) throw()
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}

	void CIUIRect::DeflateRect(int x, int y) throw()
	{
		::InflateRect(this, -x, -y);
	}

	void CIUIRect::DeflateRect(SIZE size) throw()
	{
		::InflateRect(this, -size.cx, -size.cy);
	}

	void CIUIRect::DeflateRect(LPCRECT lpRect) throw()
	{
		left += lpRect->left;
		top += lpRect->top;
		right -= lpRect->right;
		bottom -= lpRect->bottom;
	}

	void CIUIRect::DeflateRect(int l, int t, int r, int b) throw()
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	void CIUIRect::OffsetRect(int x, int y) throw()
	{
		::OffsetRect(this, x, y);
	}

	void CIUIRect::OffsetRect(POINT point) throw()
	{
		::OffsetRect(this, point.x, point.y);
	}

	void CIUIRect::OffsetRect(SIZE size) throw()
	{
		::OffsetRect(this, size.cx, size.cy);
	}

	void CIUIRect::NormalizeRect() throw()
	{
		int nTemp;

		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}

		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	void CIUIRect::MoveToY(int y) throw()
	{
		bottom = Height() + y;
		top = y;
	}

	void CIUIRect::MoveToX(int x) throw()
	{
		right = Width() + x;
		left = x;
	}

	void CIUIRect::MoveToXY(int x, int y) throw()
	{
		MoveToX(x);
		MoveToY(y);
	}

	void CIUIRect::MoveToXY(POINT pt) throw()
	{
		MoveToX(pt.x);
		MoveToY(pt.y);
	}

	BOOL CIUIRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{
		return ::IntersectRect(this, lpRect1, lpRect2);
	}

	BOOL CIUIRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{
		return ::UnionRect(this, lpRect1, lpRect2);
	}

	BOOL CIUIRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
	{
		return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
	}

	void CIUIRect::operator=(const RECT &srcRect) throw()
	{
		::CopyRect(this, &srcRect);
	}

	BOOL CIUIRect::operator==(const RECT &rect) const throw()
	{
		return ::EqualRect(this, &rect);
	}

	BOOL CIUIRect::operator!=(const RECT &rect) const throw()
	{
		return !::EqualRect(this, &rect);
	}

	void CIUIRect::operator+=(POINT point) throw()
	{
		::OffsetRect(this, point.x, point.y);
	}

	void CIUIRect::operator+=(SIZE size) throw()
	{
		::OffsetRect(this, size.cx, size.cy);
	}

	void CIUIRect::operator+=(LPCRECT lpRect) throw()
	{
		InflateRect(lpRect);
	}

	void CIUIRect::operator-=(POINT point) throw()
	{
		::OffsetRect(this, -point.x, -point.y);
	}

	void CIUIRect::operator-=(SIZE size) throw()
	{
		::OffsetRect(this, -size.cx, -size.cy);
	}

	void CIUIRect::operator-=(LPCRECT lpRect) throw()
	{
		DeflateRect(lpRect);
	}

	void CIUIRect::operator&=(const RECT &rect) throw()
	{
		::IntersectRect(this, this, &rect);
	}

	void CIUIRect::operator|=(const RECT &rect) throw()
	{
		::UnionRect(this, this, &rect);
	}

	CIUIRect CIUIRect::operator+(POINT pt) const throw()
	{
		CIUIRect rect(*this);
		::OffsetRect(&rect, pt.x, pt.y);
		return rect;
	}

	CIUIRect CIUIRect::operator-(POINT pt) const throw()
	{
		CIUIRect rect(*this);
		::OffsetRect(&rect, -pt.x, -pt.y);
		return rect;
	}

	CIUIRect CIUIRect::operator+(SIZE size) const throw()
	{
		CIUIRect rect(*this);
		::OffsetRect(&rect, size.cx, size.cy);
		return rect;
	}

	CIUIRect CIUIRect::operator-(SIZE size) const throw()
	{
		CIUIRect rect(*this);
		::OffsetRect(&rect, -size.cx, -size.cy);
		return rect;
	}

	CIUIRect CIUIRect::operator+(LPCRECT lpRect) const throw()
	{
		CIUIRect rect(this);
		rect.InflateRect(lpRect);
		return rect;
	}

	CIUIRect CIUIRect::operator-(LPCRECT lpRect) const throw()
	{
		CIUIRect rect(this);
		rect.DeflateRect(lpRect);
		return rect;
	}

	CIUIRect CIUIRect::operator&(const RECT &rect2) const throw()
	{
		CIUIRect rect;
		::IntersectRect(&rect, this, &rect2);
		return rect;
	}

	CIUIRect CIUIRect::operator|(const RECT &rect2) const throw()
	{
		CIUIRect rect;
		::UnionRect(&rect, this, &rect2);
		return rect;
	}

	CIUIRect CIUIRect::MulDiv(int nMultiplier, int nDivisor) const throw()
	{
		return CIUIRect(
				::MulDiv(left, nMultiplier, nDivisor),
				::MulDiv(top, nMultiplier, nDivisor),
				::MulDiv(right, nMultiplier, nDivisor),
				::MulDiv(bottom, nMultiplier, nDivisor));
	}

	CIUIRect CIUIRect::FitRect(int nWidth, int nHeight) const throw()
	{
		CIUIRect rcRet;

		if (nWidth <= 0 || nHeight <= 0)
		{
			return rcRet;
		}

		if (Width() * nHeight > Height() * nWidth)
		{
			rcRet.top = top;
			rcRet.bottom = bottom;
			int nDestWidth = ::MulDiv(Height(), nWidth, nHeight);
			rcRet.left = left + (Width() - nDestWidth) / 2;
			rcRet.right = rcRet.left + nDestWidth;
		}
		else
		{
			rcRet.left = left;
			rcRet.right = right;
			int nDestHeight = ::MulDiv(Width(), nHeight, nWidth);
			rcRet.top = top + (Height() - nDestHeight) / 2;
			rcRet.bottom = rcRet.top + nDestHeight;
		}

		return rcRet;
	}

	//////////////////////////////////////////////////////////////////////////

	CIUIPoint::CIUIPoint() throw()
	{
		// VS2008未初始化x, y；而VS2010把x, y初始化为0.
		x = 0;
		y = 0;
	}

	CIUIPoint::CIUIPoint(int initX, int initY) throw()
	{
		x = initX;
		y = initY;
	}

	CIUIPoint::CIUIPoint(POINT initPt) throw()
	{
		*(POINT *)this = initPt;
	}

	CIUIPoint::CIUIPoint(SIZE initSize) throw()
	{
		*(SIZE *)this = initSize;
	}

	CIUIPoint::CIUIPoint(LPARAM dwPoint) throw()
	{
		x = (short)GET_X_LPARAM(dwPoint);
		y = (short)GET_Y_LPARAM(dwPoint);
	}

	void CIUIPoint::Offset(int xOffset, int yOffset) throw()
	{
		x += xOffset;
		y += yOffset;
	}

	void CIUIPoint::Offset(POINT point) throw()
	{
		x += point.x;
		y += point.y;
	}

	void CIUIPoint::Offset(SIZE size) throw()
	{
		x += size.cx;
		y += size.cy;
	}

	void CIUIPoint::SetPoint(int X, int Y) throw()
	{
		x = X;
		y = Y;
	}

	BOOL CIUIPoint::operator==(POINT point) const throw()
	{
		return (x == point.x && y == point.y);
	}

	BOOL CIUIPoint::operator!=(POINT point) const throw()
	{
		return (x != point.x || y != point.y);
	}

	void CIUIPoint::operator+=(SIZE size) throw()
	{
		x += size.cx;
		y += size.cy;
	}

	void CIUIPoint::operator-=(SIZE size) throw()
	{
		x -= size.cx;
		y -= size.cy;
	}

	void CIUIPoint::operator+=(POINT point) throw()
	{
		x += point.x;
		y += point.y;
	}

	void CIUIPoint::operator-=(POINT point) throw()
	{
		x -= point.x;
		y -= point.y;
	}

	CIUIPoint CIUIPoint::operator+(SIZE size) const throw()
	{
		return CIUIPoint(x + size.cx, y + size.cy);
	}

	CIUIPoint CIUIPoint::operator-(SIZE size) const throw()
	{
		return CIUIPoint(x - size.cx, y - size.cy);
	}

	CIUIPoint CIUIPoint::operator-() const throw()
	{
		return CIUIPoint(-x, -y);
	}

	CIUIPoint CIUIPoint::operator+(POINT point) const throw()
	{
		return CIUIPoint(x + point.x, y + point.y);
	}

	CIUISize CIUIPoint::operator-(POINT point) const throw()
	{
		return CIUISize(x - point.x, y - point.y);
	}

	CIUIRect CIUIPoint::operator+(const RECT *lpRect) const throw()
	{
		return CIUIRect(lpRect) + *this;
	}

	CIUIRect CIUIPoint::operator-(const RECT *lpRect) const throw()
	{
		return CIUIRect(lpRect) - *this;
	}

	///////////////////////////////////////////////////////////////////
	//

	/////////////////////////////////////////////////////////////////////////////
	// CIUIOleVariant class

	void AfxVariantInit(LPVARIANT pVar)
	{
		memset(pVar, 0, sizeof(*pVar));
	}

	void AfxCheckError(SCODE sc)
	{
		if (FAILED(sc))
		{
			if (sc == E_OUTOFMEMORY)
			{
				IUITRACE(_T("E_OUTOFMEMORY\n"));
			}
			else
			{
				IUITRACE(_T("E_OleException %d\n"), sc);
			}
		}
	}

	void AfxSafeArrayInit(COleSafeArray *psa)
	{
		if (psa != NULL)
		{
			memset(psa, 0, sizeof(*psa));
		}
	}

	BOOL _AfxCompareSafeArrays(SAFEARRAY *parray1, SAFEARRAY *parray2)
	{
		BOOL bCompare = FALSE;

		// If one is NULL they must both be NULL to compare
		if (parray1 == NULL || parray2 == NULL)
		{
			return parray1 == parray2;
		}

		// Dimension must match and if 0, then arrays compare
		DWORD dwDim1 = ::SafeArrayGetDim(parray1);
		DWORD dwDim2 = ::SafeArrayGetDim(parray2);
		if (dwDim1 != dwDim2)
		{
			return FALSE;
		}
		else if (dwDim1 == 0)
		{
			return TRUE;
		}

		// Element size must match
		DWORD dwSize1 = ::SafeArrayGetElemsize(parray1);
		DWORD dwSize2 = ::SafeArrayGetElemsize(parray2);
		if (dwSize1 != dwSize2)
		{
			return FALSE;
		}

		long *pLBound1 = NULL;
		long *pLBound2 = NULL;
		long *pUBound1 = NULL;
		long *pUBound2 = NULL;

		void *pData1 = NULL;
		void *pData2 = NULL;

		//try
		{
			// Bounds must match
			pLBound1 = new long[dwDim1];
			pLBound2 = new long[dwDim2];
			pUBound1 = new long[dwDim1];
			pUBound2 = new long[dwDim2];

			size_t nTotalElements = 1;

			// Get and compare bounds
			for (DWORD dwIndex = 0; dwIndex < dwDim1; dwIndex++)
			{
				AfxCheckError(::SafeArrayGetLBound(
						parray1, dwIndex + 1, &pLBound1[dwIndex]));
				AfxCheckError(::SafeArrayGetLBound(
						parray2, dwIndex + 1, &pLBound2[dwIndex]));
				AfxCheckError(::SafeArrayGetUBound(
						parray1, dwIndex + 1, &pUBound1[dwIndex]));
				AfxCheckError(::SafeArrayGetUBound(
						parray2, dwIndex + 1, &pUBound2[dwIndex]));

				// Check the magnitude of each bound
				if (pUBound1[dwIndex] - pLBound1[dwIndex] !=
					pUBound2[dwIndex] - pLBound2[dwIndex])
				{
					delete[] pLBound1;
					delete[] pLBound2;
					delete[] pUBound1;
					delete[] pUBound2;

					return FALSE;
				}

				// Increment the element count
				nTotalElements *= pUBound1[dwIndex] - pLBound1[dwIndex] + 1;
			}

			// Access the data
			AfxCheckError(::SafeArrayAccessData(parray1, &pData1));
			AfxCheckError(::SafeArrayAccessData(parray2, &pData2));

			// Calculate the number of bytes of data and compare
			size_t nSize = nTotalElements * dwSize1;
			int nOffset = memcmp(pData1, pData2, nSize);
			bCompare = nOffset == 0;

			// Release the array locks
			AfxCheckError(::SafeArrayUnaccessData(parray1));
			AfxCheckError(::SafeArrayUnaccessData(parray2));
		}
		//catch (e)
		//{
		//	// Clean up bounds arrays
		//	delete[] pLBound1;
		//	delete[] pLBound2;
		//	delete[] pUBound1;
		//	delete[] pUBound2;

		//	// Release the array locks
		//	if (pData1 != NULL)
		//	{
		//		AfxCheckError(::SafeArrayUnaccessData(parray1));
		//	}
		//	if (pData2 != NULL)
		//	{
		//		AfxCheckError(::SafeArrayUnaccessData(parray2));
		//	}

		//	THROW_LAST();
		//}
		//END_CATCH_ALL

		// Clean up bounds arrays
		delete[] pLBound1;
		delete[] pLBound2;
		delete[] pUBound1;
		delete[] pUBound2;

		return bCompare;
	}


	CIUIOleVariant::CIUIOleVariant()
	{
		AfxVariantInit(this);
	}
	CIUIOleVariant::~CIUIOleVariant()
	{
		VERIFY(::VariantClear(this) == NOERROR);
	}

	void CIUIOleVariant::Clear()
	{
		VERIFY(::VariantClear(this) == NOERROR);
	}
	CIUIOleVariant::CIUIOleVariant(LPCTSTR lpszSrc)
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}
	CIUIOleVariant::CIUIOleVariant(CIUIString &strSrc)
	{
		vt = VT_EMPTY;
		*this = strSrc;
	}
	CIUIOleVariant::CIUIOleVariant(BYTE nSrc)
	{
		vt = VT_UI1;
		bVal = nSrc;
	}
	//CIUIOleVariant::CIUIOleVariant(const COleCurrency &curSrc)
	//{
	//	vt = VT_CY;
	//	cyVal = curSrc.m_cur;
	//}
#if (_WIN32_WINNT >= 0x0501) || defined(_ATL_SUPPORT_VT_I8)
	CIUIOleVariant::CIUIOleVariant(LONGLONG nSrc)
	{
		vt = VT_I8;
		llVal = nSrc;
	}
	CIUIOleVariant::CIUIOleVariant(ULONGLONG nSrc)
	{
		vt = VT_UI8;
		ullVal = nSrc;
	}
#endif
	CIUIOleVariant::CIUIOleVariant(float fltSrc)
	{
		vt = VT_R4;
		fltVal = fltSrc;
	}
	CIUIOleVariant::CIUIOleVariant(double dblSrc)
	{
		vt = VT_R8;
		dblVal = dblSrc;
	}
	//CIUIOleVariant::CIUIOleVariant(const COleDateTime &dateSrc)
	//{
	//	vt = VT_DATE;
	//	date = dateSrc;
	//}
	//CIUIOleVariant::CIUIOleVariant(const CByteArray &arrSrc)
	//{
	//	vt = VT_EMPTY;
	//	*this = arrSrc;
	//}
	//CIUIOleVariant::CIUIOleVariant(const CLongBinary &lbSrc)
	//{
	//	vt = VT_EMPTY;
	//	*this = lbSrc;
	//}
	CIUIOleVariant::CIUIOleVariant(LPCITEMIDLIST pidl)
	{
		AfxVariantInit(this);

		if (pidl != NULL)
		{
			// walk through entries in the list and accumulate their size

			UINT cbTotal = 0;
			SAFEARRAY *psa = NULL;
			LPCITEMIDLIST pidlWalker = pidl;

			while (pidlWalker->mkid.cb)
			{
				cbTotal += pidlWalker->mkid.cb;
				pidlWalker = (LPCITEMIDLIST)
					(((LPBYTE)pidlWalker) + pidlWalker->mkid.cb);
			}

			// add the base structure size
			cbTotal += sizeof(ITEMIDLIST);

			// get a safe array for them
			psa = SafeArrayCreateVector(VT_UI1, 0, cbTotal);

			// copy it and set members
			if (psa != NULL)
			{
				IUICopyMemory(psa->pvData, cbTotal, (LPBYTE) pidl, cbTotal);
				vt = VT_ARRAY | VT_UI1;
				parray = psa;
			}
		}
	}

	BOOL CIUIOleVariant::operator==(LPCVARIANT pSrc) const
	{
		return *this == *pSrc;
	}
	CIUIOleVariant::operator LPVARIANT()
	{
		return this;
	}
	CIUIOleVariant::operator LPCVARIANT() const
	{
		return this;
	}

	CIUIOleVariant::CIUIOleVariant(const VARIANT &varSrc)
	{
		AfxVariantInit(this);
		AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
	}

	CIUIOleVariant::CIUIOleVariant(LPCVARIANT pSrc)
	{
		AfxVariantInit(this);
		AfxCheckError(::VariantCopy(this, (LPVARIANT)pSrc));
	}

	CIUIOleVariant::CIUIOleVariant(const CIUIOleVariant &varSrc)
	{
		AfxVariantInit(this);
		AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
	}

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) UNREFERENCED_PARAMETER(x)
#endif

#if defined(_UNICODE) || defined(OLE2ANSI)
#define VTS_BSTR            VTS_WBSTR// an 'LPCOLESTR'
#define VT_BSTRT            VT_BSTR
#else
#define VTS_BSTR            "\x0E"  // an 'LPCSTR'
#define VT_BSTRA            14
#define VT_BSTRT            VT_BSTRA
#endif

	CIUIOleVariant::CIUIOleVariant(LPCTSTR lpszSrc, VARTYPE vtSrc)
	{
#if defined (UNICODE)
		_ASSERT(vtSrc == VT_BSTR);
#else
		_ASSERT(vtSrc == VT_BSTR || vtSrc == VT_BSTRT);
#endif
		UNUSED(vtSrc);

		vt = VT_BSTR;
		bstrVal = NULL;

		if (lpszSrc != NULL)
		{
#ifndef _UNICODE
			if (vtSrc == VT_BSTRT)
			{
				int nLen = lstrlen(lpszSrc);
				bstrVal = ::SysAllocStringByteLen(lpszSrc, nLen);

				if (bstrVal == NULL)
				{
					return;
					//AfxThrowMemoryException();
				}
			}
			else
#endif
			{
				USES_CONVERSION;
				bstrVal = ::SysAllocString(T2COLE(lpszSrc));
			}
		}
	}



	void CIUIOleVariant::SetString(LPCTSTR lpszSrc, VARTYPE vtSrc)
	{
#if defined (UNICODE)
		_ASSERT(vtSrc == VT_BSTR);
#else
		_ASSERT(vtSrc == VT_BSTR || vtSrc == VT_BSTRT);
#endif
		UNUSED(vtSrc);

		// Free up previous VARIANT
		Clear();

		vt = VT_BSTR;
		bstrVal = NULL;

		if (lpszSrc != NULL)
		{
#ifndef _UNICODE
			if (vtSrc == VT_BSTRT)
			{
				int nLen = lstrlen(lpszSrc);
				bstrVal = ::SysAllocStringByteLen(lpszSrc, nLen);

				if (bstrVal == NULL)
				{
					return;
					//AfxThrowMemoryException();
				}
			}
			else
#endif
			{
				USES_CONVERSION;
				bstrVal = ::SysAllocString(T2COLE(lpszSrc));
			}
		}
	}

	CIUIOleVariant::CIUIOleVariant(short nSrc, VARTYPE vtSrc)
	{
		_ASSERT(vtSrc == VT_I2 || vtSrc == VT_BOOL);

		if (vtSrc == VT_BOOL)
		{
			vt = VT_BOOL;
			if (!nSrc)
			{
				V_BOOL(this) = 0;
			}
			else
			{
				V_BOOL(this) = (-1);
			}
		}
		else
		{
			vt = VT_I2;
			iVal = nSrc;
		}
	}

	CIUIOleVariant::CIUIOleVariant(long lSrc, VARTYPE vtSrc)
	{
		_ASSERT(vtSrc == VT_I4 || vtSrc == VT_ERROR || vtSrc == VT_BOOL
			|| vtSrc == VT_UINT || vtSrc == VT_INT || vtSrc == VT_UI4
			|| vtSrc == VT_HRESULT);

		if (vtSrc == VT_ERROR)
		{
			vt = VT_ERROR;
			scode = lSrc;
		}
		else if (vtSrc == VT_BOOL)
		{
			vt = VT_BOOL;
			if (!lSrc)
			{
				V_BOOL(this) = 0;
			}
			else
			{
				V_BOOL(this) = (-1);
			}
		}
		else if (vtSrc == VT_INT)
		{
			vt = VT_INT;
			V_INT(this) = lSrc;
		}
		else if (vtSrc == VT_UINT)
		{
			vt = VT_UINT;
			V_UINT(this) = lSrc;
		}
		else if (vtSrc == VT_HRESULT)
		{
			vt = VT_HRESULT;
			V_ERROR(this) = lSrc;
		}
		else if (vtSrc == VT_UI4)
		{
			vt = VT_UI4;
			lVal = lSrc;
		}
		else
		{
			vt = VT_I4;
			lVal = lSrc;
		}
	}

	// Operations
	void CIUIOleVariant::ChangeType(VARTYPE vartype, LPVARIANT pSrc)
	{
		// If pSrc is NULL, convert type in place
		if (pSrc == NULL)
		{
			pSrc = this;
		}
		if (pSrc != this || vartype != vt)
		{
			AfxCheckError(::VariantChangeType(this, pSrc, 0, vartype));
		}
	}

	void CIUIOleVariant::Attach(VARIANT &varSrc)
	{
		// Free up previous VARIANT
		Clear();

		// give control of data to CIUIOleVariant
		IUICopyMemory(this, sizeof(VARIANT), &varSrc, sizeof(varSrc));
		varSrc.vt = VT_EMPTY;
	}

	VARIANT CIUIOleVariant::Detach()
	{
		VARIANT varResult = *this;
		vt = VT_EMPTY;
		return varResult;
	}

	//void CIUIOleVariant::GetByteArrayFromVariantArray(CByteArray &bytes)
	//{
	//	_ASSERT(V_ISARRAY(this));

	//	LPVOID pSrc;
	//	LPVOID pDest;
	//	HRESULT hResult;
	//	ULONG nDim;
	//	LONG iLowerBound;
	//	LONG iUpperBound;
	//	LONG nElements;
	//	ULONG nBytes;

	//	hResult = ::SafeArrayAccessData(V_ARRAY(this), &pSrc);
	//	AfxCheckError(hResult);

	//	nDim = ::SafeArrayGetDim(V_ARRAY(this));
	//	_ASSERT((nDim == 0) || (nDim == 1));

	//	if (nDim == 1)
	//	{
	//		::SafeArrayGetLBound(V_ARRAY(this), 1, &iLowerBound);
	//		::SafeArrayGetUBound(V_ARRAY(this), 1, &iUpperBound);
	//		nElements = (iUpperBound - iLowerBound) + 1;
	//		nBytes = nElements*::SafeArrayGetElemsize(V_ARRAY(this));
	//		bytes.SetSize(nBytes);
	//		pDest = bytes.GetData();
	//		IUICopyMemory(pDest, nBytes, pSrc, nBytes);
	//	}
	//	else
	//	{
	//		bytes.SetSize(0);
	//	}

	//	::SafeArrayUnaccessData(V_ARRAY(this));
	//}

	// Literal comparison. Types and values must match.
	BOOL CIUIOleVariant::operator==(const VARIANT &var) const
	{
		if (&var == this)
		{
			return TRUE;
		}

		// Variants not equal if types don't match
		if (var.vt != vt)
		{
			return FALSE;
		}

		// Check type specific values
		switch (vt)
		{
		case VT_EMPTY:
		case VT_NULL:
			return TRUE;

		case VT_BOOL:
			return V_BOOL(&var) == V_BOOL(this);

		case VT_I1:
			return var.cVal == cVal;

		case VT_UI1:
			return var.bVal == bVal;

		case VT_I2:
			return var.iVal == iVal;

		case VT_UI2:
			return var.uiVal == uiVal;

		case VT_I4:
			return var.lVal == lVal;

		case VT_UI4:
			return var.ulVal == ulVal;

		case VT_I8:
			return var.llVal == llVal;

		case VT_UI8:
			return var.ullVal == ullVal;

		case VT_CY:
			return (var.cyVal.Hi == cyVal.Hi && var.cyVal.Lo == cyVal.Lo);

		case VT_R4:
			return var.fltVal == fltVal;

		case VT_R8:
			return var.dblVal == dblVal;

		case VT_DATE:
			return var.date == date;

		case VT_BSTR:
			return SysStringByteLen(var.bstrVal) == SysStringByteLen(bstrVal) &&
				memcmp(var.bstrVal, bstrVal, SysStringByteLen(bstrVal)) == 0;

		case VT_ERROR:
			return var.scode == scode;

		case VT_DISPATCH:
		case VT_UNKNOWN:
			return var.punkVal == punkVal;

		default:
			if (vt & VT_ARRAY && !(vt & VT_BYREF))
			{
				return _AfxCompareSafeArrays(var.parray, parray);
			}
			else
			{
				_ASSERT(FALSE);    // VT_BYREF not supported
			}
			// fall through
		}

		return FALSE;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(const VARIANT &varSrc)
	{
		if (static_cast<LPVARIANT>(this) != &varSrc)
		{
			AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
		}

		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(LPCVARIANT pSrc)
	{
		if (static_cast<LPCVARIANT>(this) != pSrc)
		{
			AfxCheckError(::VariantCopy(this, (LPVARIANT)pSrc));
		}

		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(const CIUIOleVariant &varSrc)
	{
		if (this != &varSrc)
		{
			AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
		}

		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(const LPCTSTR lpszSrc)
	{
		// Free up previous VARIANT
		Clear();

		vt = VT_BSTR;
		if (lpszSrc == NULL)
		{
			bstrVal = NULL;
		}
		else
		{
			USES_CONVERSION;
			bstrVal = ::SysAllocString(T2COLE(lpszSrc));
		}
		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(const CIUIString &strSrc)
	{
		// Free up previous VARIANT
		Clear();

		vt = VT_BSTR;
		bstrVal = strSrc.AllocSysString();

		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(BYTE nSrc)
	{
		// Free up previous VARIANT if necessary
		if (vt != VT_UI1)
		{
			Clear();
			vt = VT_UI1;
		}

		bVal = nSrc;
		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(short nSrc)
	{
		if (vt == VT_I2)
		{
			iVal = nSrc;
		}
		else if (vt == VT_BOOL)
		{
			if (!nSrc)
			{
				V_BOOL(this) = 0;
			}
			else
			{
				V_BOOL(this) = (-1);
			}
		}
		else
		{
			// Free up previous VARIANT
			Clear();
			vt = VT_I2;
			iVal = nSrc;
		}

		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(long lSrc)
	{
		if (vt == VT_I4)
		{
			lVal = lSrc;
		}
		else if (vt == VT_ERROR)
		{
			scode = lSrc;
		}
		else if (vt == VT_BOOL)
		{
			if (!lSrc)
			{
				V_BOOL(this) = 0;
			}
			else
			{
				V_BOOL(this) = (-1);
			}
		}
		else
		{
			// Free up previous VARIANT
			Clear();
			vt = VT_I4;
			lVal = lSrc;
		}

		return *this;
	}

#if (_WIN32_WINNT >= 0x0501) || defined(_ATL_SUPPORT_VT_I8)
	const CIUIOleVariant &CIUIOleVariant::operator=(LONGLONG nSrc)
	{
		if (vt != VT_I8)
		{
			Clear();
			vt = VT_I8;
		}

		llVal = nSrc;
		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(ULONGLONG nSrc)
	{
		if (vt != VT_UI8)
		{
			Clear();
			vt = VT_UI8;
		}

		ullVal = nSrc;
		return *this;
	}
#endif

	//const CIUIOleVariant &CIUIOleVariant::operator=(const COleCurrency &curSrc)
	//{
	//	// Free up previous VARIANT if necessary
	//	if (vt != VT_CY)
	//	{
	//		Clear();
	//		vt = VT_CY;
	//	}

	//	cyVal = curSrc.m_cur;
	//	return *this;
	//}

	const CIUIOleVariant &CIUIOleVariant::operator=(float fltSrc)
	{
		// Free up previous VARIANT if necessary
		if (vt != VT_R4)
		{
			Clear();
			vt = VT_R4;
		}

		fltVal = fltSrc;
		return *this;
	}

	const CIUIOleVariant &CIUIOleVariant::operator=(double dblSrc)
	{
		// Free up previous VARIANT if necessary
		if (vt != VT_R8)
		{
			Clear();
			vt = VT_R8;
		}

		dblVal = dblSrc;
		return *this;
	}

	//const CIUIOleVariant &CIUIOleVariant::operator=(const COleDateTime &dateSrc)
	//{
	//	// Free up previous VARIANT if necessary
	//	if (vt != VT_DATE)
	//	{
	//		Clear();
	//		vt = VT_DATE;
	//	}

	//	date = dateSrc;
	//	return *this;
	//}

	//const CIUIOleVariant &CIUIOleVariant::operator=(const CByteArray &arrSrc)
	//{
	//	INT_PTR nSize = arrSrc.GetSize();
	//	if (nSize > LONG_MAX)
	//	{
	//		AfxThrowMemoryException();
	//	}

	//	// Set the correct type and make sure SafeArray can hold data
	//	_AfxCreateOneDimArray(*this, (DWORD)nSize);

	//	// Copy the data into the SafeArray
	//	_AfxCopyBinaryData(parray, arrSrc.GetData(), (DWORD)nSize);

	//	return *this;
	//}

	//const CIUIOleVariant &CIUIOleVariant::operator=(const CLongBinary &lbSrc)
	//{
	//	// Set the correct type and make sure SafeArray can hold data
	//	if (lbSrc.m_dwDataLength > LONG_MAX)
	//	{
	//		AfxThrowMemoryException();
	//	}
	//	_AfxCreateOneDimArray(*this, (ULONG)lbSrc.m_dwDataLength);

	//	// Copy the data into the SafeArray
	//	BYTE *pData = (BYTE *)::GlobalLock(lbSrc.m_hData);
	//	_AfxCopyBinaryData(parray, pData, (ULONG)lbSrc.m_dwDataLength);
	//	::GlobalUnlock(lbSrc.m_hData);

	//	return *this;
	//}


	//////////////////////////////////////////////////////////////////////
	//

	COleSafeArray::COleSafeArray()
	{
		AfxSafeArrayInit(this);
		vt = VT_EMPTY;
	}
	COleSafeArray::~COleSafeArray()
	{
		Clear();
	}
	void COleSafeArray::Clear()
	{
		VERIFY(::VariantClear(this) == NOERROR);
	}
	COleSafeArray::operator LPVARIANT()
	{
		return this;
	}
	COleSafeArray::operator LPCVARIANT() const
	{
		return this;
	}
	DWORD COleSafeArray::GetDim()
	{
		return ::SafeArrayGetDim(parray);
	}
	DWORD COleSafeArray::GetElemSize()
	{
		return ::SafeArrayGetElemsize(parray);
	}

	COleSafeArray::COleSafeArray(const SAFEARRAY &saSrc, VARTYPE vtSrc)
	{
		AfxSafeArrayInit(this);
		vt = (VARTYPE)(vtSrc | VT_ARRAY);
		AfxCheckError(::SafeArrayCopy((LPSAFEARRAY)&saSrc, &parray));
		m_dwDims = GetDim();
		m_dwElementSize = GetElemSize();
	}

	COleSafeArray::COleSafeArray(LPCSAFEARRAY pSrc, VARTYPE vtSrc)
	{
		AfxSafeArrayInit(this);
		vt = (VARTYPE)(vtSrc | VT_ARRAY);
		AfxCheckError(::SafeArrayCopy((LPSAFEARRAY)pSrc, &parray));
		m_dwDims = GetDim();
		m_dwElementSize = GetElemSize();
	}

	COleSafeArray::COleSafeArray(const COleSafeArray &saSrc)
	{
		AfxSafeArrayInit(this);
		*this = saSrc;
		m_dwDims = GetDim();
		m_dwElementSize = GetElemSize();
	}

	COleSafeArray::COleSafeArray(const VARIANT &varSrc)
	{
		AfxSafeArrayInit(this);
		*this = varSrc;
		m_dwDims = GetDim();
		m_dwElementSize = GetElemSize();
	}

	//COleSafeArray::COleSafeArray(const CIUIOleVariant &varSrc)
	//{
	//	AfxSafeArrayInit(this);
	//	*this = varSrc;
	//	m_dwDims = GetDim();
	//	m_dwElementSize = GetElemSize();
	//}

	COleSafeArray::COleSafeArray(LPCVARIANT pSrc)
	{
		AfxSafeArrayInit(this);
		*this = pSrc;
		m_dwDims = GetDim();
		m_dwElementSize = GetElemSize();
	}

	// Operations
	void COleSafeArray::Attach(VARIANT &varSrc)
	{
		_ASSERT(varSrc.vt & VT_ARRAY);

		if (!(varSrc.vt & VT_ARRAY))
		{
			_ASSERT(FALSE);
			return;
		}

		// Free up previous safe array if necessary
		Clear();

		// give control of data to COleSafeArray
		IUICopyMemory(this, sizeof(VARIANT), &varSrc, sizeof(varSrc));
		varSrc.vt = VT_EMPTY;
	}

	VARIANT COleSafeArray::Detach()
	{
		VARIANT varResult = *this;
		vt = VT_EMPTY;
		return varResult;
	}

	//void COleSafeArray::GetByteArray(CByteArray &bytes)
	//{
	//	LPVOID pSrc;
	//	LPVOID pDest;
	//	HRESULT hResult;
	//	ULONG nDim;
	//	LONG iLowerBound;
	//	LONG iUpperBound;
	//	LONG nElements;
	//	ULONG nBytes;

	//	_ASSERT(V_ISARRAY(this));

	//	hResult = ::SafeArrayAccessData(V_ARRAY(this), &pSrc);
	//	AfxCheckError(hResult);

	//	nDim = ::SafeArrayGetDim(V_ARRAY(this));
	//	_ASSERT((nDim == 0) || (nDim == 1));

	//	if (nDim == 1)
	//	{
	//		::SafeArrayGetLBound(V_ARRAY(this), 1, &iLowerBound);
	//		::SafeArrayGetUBound(V_ARRAY(this), 1, &iUpperBound);
	//		nElements = (iUpperBound - iLowerBound) + 1;
	//		nBytes = nElements*::SafeArrayGetElemsize(V_ARRAY(this));
	//		bytes.SetSize(nBytes);
	//		pDest = bytes.GetData();
	//		IUICopyMemory(pDest, nBytes, pSrc, nBytes);
	//	}
	//	else
	//	{
	//		bytes.SetSize(0);
	//	}

	//	::SafeArrayUnaccessData(V_ARRAY(this));
	//}

	// Assignment operators
	COleSafeArray &COleSafeArray::operator=(const COleSafeArray &saSrc)
	{
		_ASSERT(saSrc.vt & VT_ARRAY);

		if (!(saSrc.vt & VT_ARRAY))
		{
			_ASSERT(FALSE);
			return *this;
			// AfxThrowInvalidArgException();
		}

		AfxCheckError(::VariantCopy(this, (LPVARIANT)&saSrc));
		return *this;
	}

	COleSafeArray &COleSafeArray::operator=(const VARIANT &varSrc)
	{
		_ASSERT(varSrc.vt & VT_ARRAY);

		if (!(varSrc.vt & VT_ARRAY))
		{
			_ASSERT(FALSE);
			return *this;
			// AfxThrowInvalidArgException();
		}

		AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
		return *this;
	}

	COleSafeArray &COleSafeArray::operator=(LPCVARIANT pSrc)
	{
		_ASSERT(pSrc->vt & VT_ARRAY);

		if (!(pSrc->vt & VT_ARRAY))
		{
			_ASSERT(FALSE);
			return *this;
			// AfxThrowInvalidArgException();
		}

		AfxCheckError(::VariantCopy(this, (LPVARIANT)pSrc));
		return *this;
	}

	//COleSafeArray &COleSafeArray::operator=(const CIUIOleVariant &varSrc)
	//{
	//	_ASSERT(varSrc.vt & VT_ARRAY);

	//	if (!(varSrc.vt & VT_ARRAY))
	//	{
	//		_ASSERT(FALSE);
	//		return *this;
	//		// AfxThrowInvalidArgException();
	//	}

	//	AfxCheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
	//	return *this;
	//}

	// Comparison operators
	BOOL COleSafeArray::operator==(const SAFEARRAY &saSrc) const
	{
		return _AfxCompareSafeArrays(parray, (LPSAFEARRAY)&saSrc);
	}

	BOOL COleSafeArray::operator==(LPCSAFEARRAY pSrc) const
	{
		return _AfxCompareSafeArrays(parray, (LPSAFEARRAY)pSrc);
	}

	BOOL COleSafeArray::operator==(const COleSafeArray &saSrc) const
	{
		if (vt != saSrc.vt)
		{
			return FALSE;
		}

		return _AfxCompareSafeArrays(parray, saSrc.parray);
	}

	BOOL COleSafeArray::operator==(const VARIANT &varSrc) const
	{
		if (vt != varSrc.vt)
		{
			return FALSE;
		}

		return _AfxCompareSafeArrays(parray, varSrc.parray);
	}

	BOOL COleSafeArray::operator==(LPCVARIANT pSrc) const
	{
		if (vt != pSrc->vt)
		{
			return FALSE;
		}

		return _AfxCompareSafeArrays(parray, pSrc->parray);
	}

	//BOOL COleSafeArray::operator==(const CIUIOleVariant &varSrc) const
	//{
	//	if (vt != varSrc.vt)
	//	{
	//		return FALSE;
	//	}

	//	return _AfxCompareSafeArrays(parray, varSrc.parray);
	//}

	void COleSafeArray::CreateOneDim(VARTYPE vtSrc, DWORD dwElements,
		const void *pvSrcData, long nLBound)
	{
		if (dwElements <= 0)
		{
			_ASSERT(FALSE);
			return;
		}

		// Setup the bounds and create the array
		SAFEARRAYBOUND rgsabound;
		rgsabound.cElements = dwElements;
		rgsabound.lLbound = nLBound;
		Create(vtSrc, 1, &rgsabound);

		// Copy over the data if neccessary
		if (pvSrcData != NULL)
		{
			void *pvDestData;
			AccessData(&pvDestData);


			unsigned __int64 tmp_64 = static_cast<unsigned __int64>(GetElemSize()) * static_cast<unsigned __int64>(dwElements);

			if (tmp_64 > INT_MAX)
			{
				_ASSERT(FALSE);
				return;
			}


			IUICopyMemory(pvDestData, static_cast<size_t>(tmp_64),
				pvSrcData, static_cast<size_t>(tmp_64));
			UnaccessData();
		}
	}

	DWORD COleSafeArray::GetOneDimSize()
	{
		if (GetDim() != 1)
		{
			_ASSERT(FALSE);
			return 0;
		}

		long nUBound, nLBound;

		GetUBound(1, &nUBound);
		GetLBound(1, &nLBound);

		return nUBound + 1 - nLBound;
	}

	void COleSafeArray::ResizeOneDim(DWORD dwElements)
	{
		_ASSERT(GetDim() == 1);

		if (!(GetDim() == 1))
		{
			_ASSERT(FALSE);
			return;

			//AfxThrowInvalidArgException();
		}

		SAFEARRAYBOUND rgsabound;

		rgsabound.cElements = dwElements;
		rgsabound.lLbound = 0;

		Redim(&rgsabound);
	}

	void COleSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, DWORD *rgElements)
	{
		_ASSERT(rgElements != NULL);

		if (rgElements == NULL)
		{
			_ASSERT(FALSE);
			return;
			//AfxThrowInvalidArgException();
		}

		// Allocate and fill proxy array of bounds (with lower bound of zero)
		SAFEARRAYBOUND *rgsaBounds = new SAFEARRAYBOUND[dwDims];

		for (DWORD dwIndex = 0; dwIndex < dwDims; dwIndex++)
		{
			// Assume lower bound is 0 and fill in element count
			rgsaBounds[dwIndex].lLbound = 0;
			rgsaBounds[dwIndex].cElements = rgElements[dwIndex];
		}

		//TRY
		{
			Create(vtSrc, dwDims, rgsaBounds);
		}
		//CATCH_ALL(e)
		//{
		//	// Must free up memory
		//	delete[] rgsaBounds;
		//	THROW_LAST();
		//}
		//END_CATCH_ALL

		delete[] rgsaBounds;
	}

	void COleSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, SAFEARRAYBOUND *rgsabound)
	{
		_ASSERT(dwDims > 0);
		_ASSERT(rgsabound != NULL);

		// Validate the VARTYPE for SafeArrayCreate call
		_ASSERT(!(vtSrc & VT_ARRAY));
		_ASSERT(!(vtSrc & VT_BYREF));
		_ASSERT(!(vtSrc & VT_VECTOR));
		_ASSERT(vtSrc != VT_EMPTY);
		_ASSERT(vtSrc != VT_NULL);

		if (dwDims == 0 			||
			rgsabound == NULL 	||
			(vtSrc & VT_ARRAY) 	||
			(vtSrc & VT_BYREF) 	||
			(vtSrc & VT_VECTOR) 	||
			vtSrc == VT_EMPTY 	||
			vtSrc == VT_NULL)
		{
			_ASSERT(FALSE);
			return;
			// AfxThrowInvalidArgException();
		}

		// Free up old safe array if necessary
		Clear();

		parray = ::SafeArrayCreate(vtSrc, dwDims, rgsabound);

		if (parray == NULL)
		{
			_ASSERT(FALSE);
			return;
			// AfxThrowMemoryException();
		}

		vt = unsigned short(vtSrc | VT_ARRAY);
		m_dwDims = dwDims;
		m_dwElementSize = GetElemSize();
	}

	void COleSafeArray::AccessData(void **ppvData)
	{
		AfxCheckError(::SafeArrayAccessData(parray, ppvData));
	}

	void COleSafeArray::UnaccessData()
	{
		AfxCheckError(::SafeArrayUnaccessData(parray));
	}

	void COleSafeArray::AllocData()
	{
		AfxCheckError(::SafeArrayAllocData(parray));
	}

	void COleSafeArray::AllocDescriptor(DWORD dwDims)
	{
		AfxCheckError(::SafeArrayAllocDescriptor(dwDims, &parray));
	}

	void COleSafeArray::Copy(LPSAFEARRAY *ppsa)
	{
		AfxCheckError(::SafeArrayCopy(parray, ppsa));
	}

	void COleSafeArray::GetLBound(DWORD dwDim, long *pLbound)
	{
		AfxCheckError(::SafeArrayGetLBound(parray, dwDim, pLbound));
	}

	void COleSafeArray::GetUBound(DWORD dwDim, long *pUbound)
	{
		AfxCheckError(::SafeArrayGetUBound(parray, dwDim, pUbound));
	}

	void COleSafeArray::GetElement(long *rgIndices, void *pvData)
	{
		AfxCheckError(::SafeArrayGetElement(parray, rgIndices, pvData));
	}

	void COleSafeArray::PtrOfIndex(long *rgIndices, void **ppvData)
	{
		AfxCheckError(::SafeArrayPtrOfIndex(parray, rgIndices, ppvData));
	}

	void COleSafeArray::PutElement(long *rgIndices, void *pvData)
	{
		AfxCheckError(::SafeArrayPutElement(parray, rgIndices, pvData));
	}

	void COleSafeArray::Redim(SAFEARRAYBOUND *psaboundNew)
	{
		AfxCheckError(::SafeArrayRedim(parray, psaboundNew));
	}

	void COleSafeArray::Lock()
	{
		AfxCheckError(::SafeArrayLock(parray));
	}

	void COleSafeArray::Unlock()
	{
		AfxCheckError(::SafeArrayUnlock(parray));
	}

	void COleSafeArray::Destroy()
	{
		AfxCheckError(::SafeArrayDestroy(parray));
		// The underlying SafeArray object was destroyed, so we need to detach the object to void operating on it anymore.
		// We don't care the destroyed object, so we can just simply call Detach after ::SafeArrayDestroy to set vt to VT_EMPTY.
		Detach();
	}

	void COleSafeArray::DestroyData()
	{
		AfxCheckError(::SafeArrayDestroyData(parray));
	}

	void COleSafeArray::DestroyDescriptor()
	{
		AfxCheckError(::SafeArrayDestroyDescriptor(parray));
	}


	/////////////////////////////////////////////////////////////////////////////
	//
	//

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);

		while (len-- > 0)
		{
			i = (i << 5) + i + Key[len];
		}

		return i;
	}

	static UINT HashKey(const CIUIString &Key)
	{
		return HashKey((LPCTSTR)Key);
	};


	void InitPdata(WCHAR **pDest, CIUIString str)
	{
		int len = str.GetLength();
		WCHAR *dest = NULL;
		dest = (WCHAR *)malloc(sizeof(WCHAR) * (len + 1));
		dest[len] = 0;

		::memcpy(dest, str, len * sizeof(WCHAR));
		*pDest = dest;
	}

	CIUIString StringFromInt(int nValue)
	{
		TCHAR szValue[64] = {0};
#if(_MSC_VER <= 1310) // VC7.1
		_itot(nValue, szValue, 10);
#else
		_itot_s(nValue, szValue, 10);
#endif

		return szValue;
	}

	CIUIString StringFromUINT(UINT n)
	{
		TCHAR szBuf[32] = {0};
#if(_MSC_VER <= 1310) // VC7.1
		_ultot(n, szBuf, 10);
#else
		_ultot_s(n, szBuf, 10);
#endif
		return szBuf;
	}

	unsigned long UINTFromString(LPCTSTR lpString)
	{
		unsigned long ulValue = 0;
		CIUIString strValue = lpString;

		if (strValue.Left(2) == _T("0x"))
		{
			ulValue = _tcstoul(strValue, NULL, 16);
		}
		else
		{
			ulValue = _tcstoul(strValue, NULL, 10);
		}

		return ulValue;
	}

	unsigned long UINTFromHexString(LPCTSTR lpString)
	{
		unsigned long ulValue = _tcstoul(lpString, NULL, 16);

		return ulValue;
	}

	int IntFromString(LPCTSTR lpString)
	{
		long lValue = 0;
		CIUIString strValue(lpString);

		if (strValue.Left(2) == _T("0x"))
		{
			lValue = _tcstol(strValue, NULL, 16);
		}
		else
		{
			lValue = _tcstol(strValue, NULL, 10);
		}

		return lValue;
	}

	int IntFromHexString(LPCTSTR lpString)
	{
		long lValue = _tcstol(lpString, NULL, 16);

		return lValue;
	}

	long LongFromString(LPCTSTR lpString)
	{
		long lValue = 0;
		CIUIString strValue(lpString);

		if (strValue.Left(2) == _T("0x"))
		{
			lValue = _tcstol(strValue, NULL, 16);
		}
		else
		{
			lValue = _tcstol(strValue, NULL, 10);
		}

		return lValue;
	}

	CIUIString GetExecutePath()
	{
		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);

		CIUIString strPath(szPath);

		if (strPath.Right(1) != CIUIString('\\') && strPath.Right(1) != CIUIString('/'))
		{
			strPath += '\\';
		}

		return strPath;
	}

	// 拆分文件路径
	int IUI::TSPLITPATH(LPCTSTR path,
		LPTSTR drive, size_t driveSizeInCharacters,
		LPTSTR dir, size_t dirSizeInCharacters,
		LPTSTR fname, size_t nameSizeInCharacters,
		LPTSTR ext, size_t extSizeInBytes)
	{
		if (path == NULL)
		{
			return -1;
		}

#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
		_tsplitpath(path, drive, dir, fname, ext);
#else
		_tsplitpath_s(path, drive, driveSizeInCharacters, dir, dirSizeInCharacters, fname,
			nameSizeInCharacters, ext, extSizeInBytes);
#endif // (_MSC_VER <= 1310)

		return 0;
	}

	void IUI::TMAKEPATH(
		LPTSTR path,
		size_t sizeInBytes,
		LPCTSTR drive,
		LPCTSTR dir,
		LPCTSTR fname,
		LPCTSTR ext)
	{
		if (path == NULL)
		{
			return;
		}

#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
		_tmakepath(path, drive, dir, fname, ext);
#else
		_tmakepath_s(path, sizeInBytes, drive, dir, fname, ext);
#endif // (_MSC_VER <= 1310)

		return;
	}


	// 得到文件扩展名
	CIUIString IUI::GetFileExt(const CIUIString &strFile, BOOL bHasDot/* = TRUE*/)
	{
		CIUIString strRet;
		TCHAR szExt[MAX_PATH] = {0};
		TSPLITPATH(strFile, NULL, 0, NULL, 0, NULL, 0, szExt, MAX_PATH);
		strRet = szExt;

		if (!bHasDot && !strRet.IsEmpty() && strRet.Left(1) == '.')
		{
			strRet.Delete(0, 1);
		}

		CIUIString s = (LPCTSTR)strRet;
		return s;
	}

	HBITMAP Create32BitsHBITMAP(int nWidth, int nHeight, int nBPP, DWORD eCompression, void **m_ppBits,
		const DWORD *pdwBitmasks/* = NULL*/, DWORD dwFlags/* = 0*/)
	{
		LPBITMAPINFO pbmi = NULL;

		if (dwFlags)
		{
			_ASSERT((nBPP == 32) && (eCompression == BI_RGB));
		}

		pbmi = NULL;
		pbmi = (LPBITMAPINFO)malloc(sizeof(char) * (sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256));

		if (pbmi == NULL)
		{
			return NULL;
		}

		memset(&pbmi->bmiHeader, 0, sizeof(pbmi->bmiHeader));
		pbmi->bmiHeader.biSize = sizeof(pbmi->bmiHeader);
		pbmi->bmiHeader.biWidth = nWidth;
		pbmi->bmiHeader.biHeight = nHeight;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = USHORT(nBPP);
		pbmi->bmiHeader.biCompression = eCompression;

		if (nBPP <= 8)
		{
			_ASSERT(eCompression == BI_RGB);
#pragma warning(push)
#pragma warning(disable:4068) //Disable unknown pragma warning that prefast pragma causes.
#pragma prefast(push)
#pragma prefast(disable:203, "no buffer overrun here, buffer was alocated properly")
			memset(pbmi->bmiColors, 0, 256 * sizeof(RGBQUAD));
#pragma prefast(pop)
#pragma warning(pop)
		}
		else
		{
			if (eCompression == BI_BITFIELDS)
			{
				_ASSERT(pdwBitmasks != NULL);
				memcpy(pbmi->bmiColors, pdwBitmasks, 3 * sizeof(DWORD));
			}
		}

		HBITMAP hBitmap = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, m_ppBits, NULL, 0);

		if (hBitmap == NULL)
		{
			free(pbmi);
			return NULL;
		}

		free(pbmi);
		pbmi = NULL;

		return hBitmap;
	}

	// 解决GDIplus加载HBITMAP时，丢弃alpha值的问题
	Gdiplus::Bitmap *CreateBitmapFromHBITMAP(IN HBITMAP hBitmap)
	{
		BITMAP bmp = {0};
		if (0 == GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bmp))
		{
			return FALSE;
		}

		// Although we can get bitmap data address by bmp.bmBits member of BITMAP
		// which is got by GetObject function sometime,
		// we can determine the bitmap data in the HBITMAP is arranged bottom-up
		// or top-down, so we should always use GetDIBits to get bitmap data.
		BYTE *piexlsSrc = NULL;
		LONG cbSize = bmp.bmWidthBytes * bmp.bmHeight;
		piexlsSrc = new BYTE[cbSize];

		BITMAPINFO bmpInfo = { 0 };
		// We should initialize the first six members of BITMAPINFOHEADER structure.
		// A bottom-up DIB is specified by setting the height to a positive number,
		// while a top-down DIB is specified by setting the height to a negative number.
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		bmpInfo.bmiHeader.biHeight = bmp.bmHeight; // 正数，说明数据从下到上，如未负数，则从上到下
		bmpInfo.bmiHeader.biPlanes = bmp.bmPlanes;
		bmpInfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		HDC hdcScreen = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		LONG cbCopied = GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight,
				piexlsSrc, &bmpInfo, DIB_RGB_COLORS);
		DeleteDC(hdcScreen);
		if (0 == cbCopied)
		{
			delete [] piexlsSrc;
			return FALSE;
		}

		// Create an GDI+ Bitmap has the same dimensions with hbitmap
		Gdiplus::Bitmap *pBitmap = ::new Gdiplus::Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat32bppPARGB);

		// Access to the Gdiplus::Bitmap's pixel data
		Gdiplus::BitmapData bitmapData;
		Gdiplus::Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
		if (Gdiplus::Ok != pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead,
			PixelFormat32bppPARGB, &bitmapData))
		{
			::delete pBitmap;
			return NULL;
		}

		BYTE *pixelsDest = (BYTE *)bitmapData.Scan0;
		int nLinesize = bmp.bmWidth * sizeof(UINT);
		int nHeight = bmp.bmHeight;

		// Copy pixel data from HBITMAP by bottom-up.
		for (int y = 0; y < nHeight; y++)
		{
			IUICopyMemory(
				(pixelsDest + y * nLinesize),
				nLinesize,
				(piexlsSrc + (nHeight - y - 1) * nLinesize),
				nLinesize);
		}

		// Copy the data in temporary buffer to pBitmap
		if (Gdiplus::Ok != pBitmap->UnlockBits(&bitmapData))
		{
			delete pBitmap;
		}

		delete [] piexlsSrc;
		return pBitmap;
	}

	int StretchAlphaBlend(
		HDC hdcDest,
		int nXOriginDest, int nYOriginDest, int nDestWidth, int nDestHeight,
		HDC hdcSrc,
		int nXOriginSrc, int nYOriginSrc, int nSrcWidth, int nSrcHeight, BOOL bAlphaFormat,
		BYTE btSourceConstantAlpha/* = 255*/)
	{
		BLENDFUNCTION blend;
		blend.SourceConstantAlpha = btSourceConstantAlpha;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = bAlphaFormat ? AC_SRC_ALPHA : 0;

		// If the nDestWidth or nDestHeight not equal the image size, may return FALSE.
		typedef BOOL (WINAPI * LPALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int,
			BLENDFUNCTION);
		static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(
					_T("msimg32.dll")), "AlphaBlend");
		BOOL bResult = lpAlphaBlend(hdcDest,
				nXOriginDest, nYOriginDest, nDestWidth, nDestHeight,
				hdcSrc,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight,
				blend);

		if (!bResult)
		{
			return -1;
		}

		return 0;
	}

	int StretchAlphaBlendImage(
		HDC hdcDest,
		LPCRECT lprcDest,
		HBITMAP hbmpSrc,
		LPCRECT lprcSrc,
		BYTE btSourceConstantAlpha/* = 255*/)
	{
		if (hdcDest == NULL || hbmpSrc == NULL)
		{
			return -1;
		}

		if (lprcDest == NULL || lprcSrc == NULL)
		{
			return -2;
		}

		if (JG_DPIIsSmoothStretch(true))
		{
			Gdiplus::Graphics g(hdcDest);

			Gdiplus::Image *pImage = CreateBitmapFromHBITMAP(hbmpSrc);

			Gdiplus::Rect rcDest;
			rcDest.X = lprcDest->left;
			rcDest.Y = lprcDest->top;
			rcDest.Width = lprcDest->right - lprcDest->left;
			rcDest.Height = lprcDest->bottom - lprcDest->top;

			// 设置一下，否则对于纯色图拉大后，右边缘会产生渐变
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

			g.DrawImage(pImage, rcDest, lprcSrc->left, lprcSrc->top,
				lprcSrc->right - lprcSrc->left, lprcSrc->bottom - lprcSrc->top, Gdiplus::UnitPixel, NULL);

			::delete pImage;
		}
		else
		{
			HDC hdcMem = ::CreateCompatibleDC(hdcDest);
			HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMem, hbmpSrc);

			StretchAlphaBlend(hdcDest,
				lprcDest->left,
				lprcDest->top,
				lprcDest->right - lprcDest->left,
				lprcDest->bottom - lprcDest->top,
				hdcMem,
				lprcSrc->left,
				lprcSrc->top,
				lprcSrc->right - lprcSrc->left,
				lprcSrc->bottom - lprcSrc->top,
				btSourceConstantAlpha);

			::SelectObject(hdcMem, hbmpOld);
			::DeleteDC(hdcMem);
		}

		return 0;
	}


	int CopyHBITMAP(HBITMAP hbmpSrc, HBITMAP *phbmpDest, BOOL bAlphaFormat)
	{
		if (hbmpSrc == NULL)
		{
			return -1;
		}

		if (phbmpDest == NULL)
		{
			return -2;
		}

		HDC hdcN = ::GetDC(NULL);
		HDC hdcMemDest = ::CreateCompatibleDC(hdcN);

		BITMAP bm;
		GetObject(hbmpSrc, sizeof(bm), &bm);

		HDC hdcMemSrc = CreateCompatibleDC(hdcN);
		HBITMAP hbmpSrcOld = (HBITMAP)SelectObject(hdcMemSrc, (HBITMAP)hbmpSrc);

		HBITMAP hOldBitmapDest = NULL;

		if (bm.bmBitsPixel == 32)
		{
			void *pBits;
			*phbmpDest = Create32BitsHBITMAP(bm.bmWidth, bm.bmHeight, 32, BI_RGB, &pBits);
			hOldBitmapDest = (HBITMAP)::SelectObject(hdcMemDest, *phbmpDest);

			int nRet = StretchAlphaBlend(hdcMemDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemSrc, 0, 0, bm.bmWidth,
					bm.bmHeight, bAlphaFormat);
		}
		else
		{
			*phbmpDest = ::CreateCompatibleBitmap(hdcN, bm.bmWidth, bm.bmHeight);
			hOldBitmapDest = (HBITMAP)::SelectObject(hdcMemDest, *phbmpDest);

			int nRet = BitBlt(hdcMemDest, 0, 0, bm.bmWidth, bm.bmHeight, hdcMemSrc, 0, 0, SRCCOPY);
		}

		SelectObject(hdcMemSrc, hbmpSrcOld);
		DeleteDC(hdcMemSrc);
		SelectObject(hdcMemDest, hOldBitmapDest);

		return 0;
	}

	HWND GetSafeOwner_(HWND hParent, HWND *pWndTop)
	{
		// get window to start with
		HWND hWnd = hParent;

		if (hWnd == NULL)
		{
			hWnd = IUIGetMainWnd()->GetSafeHwnd();
		}

		// a popup window cannot be owned by a child window
		while (hWnd != NULL && (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD))
		{
			hWnd = ::GetParent(hWnd);
		}

		// determine toplevel window to disable as well
		HWND hWndTop = hWnd, hWndTemp = hWnd;

		for (;;)
		{
			if (hWndTemp == NULL)
			{
				break;
			}
			else
			{
				hWndTop = hWndTemp;
			}

			hWndTemp = ::GetParent(hWndTop);
		}

		// get last active popup of first non-child that was found
		if (hParent == NULL && hWnd != NULL)
		{
			hWnd = ::GetLastActivePopup(hWnd);
		}

		// disable and store top level parent window if specified
		if (pWndTop != NULL)
		{
			if (hWndTop != NULL && ::IsWindowEnabled(hWndTop) && hWndTop != hWnd)
			{
				*pWndTop = hWndTop;
				::EnableWindow(hWndTop, FALSE);
			}
			else
			{
				*pWndTop = NULL;
			}
		}

		return hWnd;    // return the owner as HWND
	}

	LPSTR nstrchr(LPCSTR pbyte, int ncb, int c)
	{
		for (int i = 0; i < ncb ; i++)
		{
			LPSTR pb = LPSTR(pbyte + i);

			if (*pb == c)
			{
				return pb;
			}
		}

		return NULL;
	}

	LPWSTR wnstrchr(LPCWSTR pbyte, int ncb, int c)
	{
		for (int i = 0; i < ncb ; i++)
		{
			LPWSTR pb = LPWSTR(pbyte + i);

			if (*pb == c)
			{
				return pb;
			}
		}

		return NULL;
	}

	BOOL AfxExtractSubStringA(
		std::string *pString,
		LPCSTR lpszFullString,
		int nFullStringCount,
		int iSubString,
		CHAR chSep)
	{
		if (pString == NULL || lpszFullString == NULL)
		{
			return FALSE;
		}

		while (iSubString--)
		{
			lpszFullString = nstrchr(lpszFullString, nFullStringCount, chSep);

			if (lpszFullString == NULL)
			{
				pString->erase(pString->begin(), pString->end());    // return empty string as well
				return FALSE;
			}

			lpszFullString++;       // point past the separator
		}

		LPCSTR lpchEnd = nstrchr(lpszFullString, nFullStringCount, chSep);
		int nLen = (lpchEnd == NULL) ? nFullStringCount : (int)(lpchEnd - lpszFullString);
		_ASSERT(nLen >= 0);
		CHAR *p = ::new CHAR [nLen + 1];
		memset(p, 0, sizeof(CHAR) * (nLen + 1));
		memcpy(p, lpszFullString, nLen * sizeof(CHAR));

		*pString = p;
		delete [] p;
		return TRUE;
	}

	BOOL AfxExtractSubStringW(
		std::wstring *pString,
		LPCWSTR lpszFullString,
		int nFullStringCount,
		int iSubString,
		WCHAR chSep)
	{
		if (pString == NULL || lpszFullString == NULL)
		{
			return FALSE;
		}

		while (iSubString--)
		{
			lpszFullString = wnstrchr(lpszFullString, nFullStringCount, chSep);

			if (lpszFullString == NULL)
			{
				pString->erase(pString->begin(), pString->end());    // return empty string as well
				return FALSE;
			}

			lpszFullString++;       // point past the separator
		}

		LPCWSTR lpchEnd = wnstrchr(lpszFullString, nFullStringCount, chSep);
		int nLen = (lpchEnd == NULL) ? nFullStringCount : (int)(lpchEnd - lpszFullString);
		_ASSERT(nLen >= 0);
		WCHAR *p = new WCHAR [nLen + 1];
		memset(p, 0, sizeof(WCHAR) * (nLen + 1));
		memcpy(p, lpszFullString, nLen * sizeof(WCHAR));

		*pString = p;
		delete [] p;
		return TRUE;
	}

	void JG_DPIEnableDPI(bool)
	{

	}

	int JG_DPIGetDPILogFont(LOGFONT *plf, bool bDPI)
	{
		return 0;
	}

	RECT JG_DPIGetDPIRect(RECT rcSrc, bool bDPI)
	{
		return rcSrc;
		RECT rc;
		rc.left = 0;
		return rc;
	}

	RECT JG_DPIGetDPIPadding(RECT rcSrc, BOOL bDPI)
	{
		return rcSrc;
		RECT rc;
		rc.left = 0;
		return rc;
	}

	SIZE JG_DPIGetDPISize(SIZE sizeSrc, BOOL bDPI)
	{
		return sizeSrc;
		SIZE size;
		size.cx = 0;
		return size;
	}

	BOOL JG_DPIIsSmoothStretch(BOOL bDPI)
	{
		return TRUE;
	}

	int JG_DPIGetDPIPointX(int nSrc, BOOL bDPI)
	{
		return nSrc;
		return 0;
	}

	int JG_DPIGetDPIPointY(int nSrc, BOOL bDPI)
	{
		return nSrc;
		return 0;
	}

	int JG_DPIGetDPIWidth(int nSrc, BOOL bDPI)
	{
		return nSrc;
		return 0;
	}

	int JG_DPIGetDPIHeight(int nSrc, BOOL bDPI)
	{
		return nSrc;
	}

} // namespace IUI

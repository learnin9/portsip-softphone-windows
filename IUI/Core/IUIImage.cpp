// IUIImage.cpp: implementation of the IUIImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#ifndef ATLASSERT
#define ATLASSERT(expr) _ASSERTE(expr)
#endif

template< typename N >
inline N WINAPI _IUIAtlAlignUp(N n, ULONG nAlign) throw()
{
	return (N((n + (nAlign - 1)) & ~(N(nAlign) - 1)));
}

#define CHECK_AND_LOAD(RETURN_VALUE) if (this == NULL)\
	{\
		return (RETURN_VALUE);\
	}\
	if (m_hBitmap == NULL)\
	{\
		((IUIImage *)this)->SafeLoadSavedImage();\
	}\
	if (m_hBitmap == NULL)\
	{\
		return (RETURN_VALUE);\
	}\

LPCTSTR GetFileExt(LPCTSTR lpszFile, BOOL bHasDot/* = TRUE*/)
{
	CIUIString strRet;

	// 一定要把lpszFile赋值为CIUIString，因为传入的lpszFile如果是数字强转成字符串的话
	// 用这样的lpszFile调用TSPLITPATH有可能会崩溃。
	CIUIString strFile = lpszFile;
	TCHAR szExt[MAX_PATH] = {0};
	TSPLITPATH(strFile, NULL, 0, NULL, 0, NULL, 0, szExt, MAX_PATH);
	strRet = szExt;
	if (!bHasDot && !strRet.IsEmpty() && strRet.Left(1) == '.')
	{
		strRet.Delete(0, 1);
	}

	return strRet;
}

std::string GetHexCodeW(LPCWSTR lpszSource)
{
	size_t nLen = wcslen(lpszSource);

	size_t nSize = nLen * sizeof(WCHAR);
	BYTE *pData = new BYTE[nSize];

	// Ignore 0.
	size_t nIndex = 0;
	BYTE *pSource = (BYTE *)lpszSource;
	for (size_t i = 0; i < nSize; ++i)
	{
		if (pSource[i] != 0)
		{
			pData[nIndex++] = pSource[i];
		}
	}

	CIUIString strRet;
	for (size_t j = 0; j < nIndex; ++j)
	{
		BYTE bt = pData[j];

		BYTE nHi = bt >> 4;
		BYTE nLo = bt << 4;
		nLo = nLo >> 4;

		strRet += HexStringFromInt(nHi, FALSE, FALSE, 1);
		strRet += HexStringFromInt(nLo, FALSE, FALSE, 1);
	}

	delete [] pData;

	return (LPCSTR)_bstr_t(strRet);
}

std::wstring GetFileTitleExW(LPCWSTR lpszFile)
{
	std::wstring wstrRet;
	if (lpszFile == NULL)
	{
		return wstrRet;
	}

	WCHAR szName[MAX_PATH] = {0};
#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
	_wsplitpath(lpszFile, NULL, NULL, szName, NULL);
#else
	_wsplitpath_s(lpszFile, NULL, 0, NULL, 0, szName, MAX_PATH, NULL, 0);
#endif // (_MSC_VER <= 1310)

	wstrRet = szName;
	return wstrRet;
}

std::wstring GetFileExtW(LPCWSTR lpszFile, BOOL bHasDot/* = TRUE*/)
{
	std::wstring wstrRet;
	if (lpszFile == NULL)
	{
		return wstrRet;
	}

	WCHAR szExt[MAX_PATH] = {0};
#if(_MSC_VER <= 1310) // VC6.0 or VC7.1
	_wsplitpath(lpszFile, NULL, NULL, NULL, szExt);
#else
	_wsplitpath_s(lpszFile, NULL, 0, NULL, 0, NULL, 0, szExt, MAX_PATH);
#endif // (_MSC_VER <= 1310)

	wstrRet = szExt;

	// Remove dot
	if (!bHasDot && !wstrRet.empty() && wstrRet[0] == '.')
	{
		wstrRet.erase(wstrRet.begin());
	}

	return wstrRet;
}

//////////////////////////////////////////////////////////////////////
// GifHelper

GifHelper::GifHelper()
{
	m_nCurFrame = 0;
	m_nCurLoop = 0;
}

GifHelper::~GifHelper()
{

}

int GifHelper::SetCurFrame(int nCur)
{
	m_nCurFrame = nCur;
	return 0;
}

int GifHelper::GetCurFrame() const
{
	return m_nCurFrame;
}

int GifHelper::SetCurLoop(int nCur)
{
	m_nCurLoop = nCur;
	return 0;
}

int GifHelper::GetCurLoop() const
{
	return m_nCurLoop;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const DWORD IUIImage::createAlphaChannel = 0x01;

IUIImage::IUIImage() throw()
	: m_nAddRef(0)
	, m_pOwner(NULL)
	, m_hBitmap(NULL)
	, m_pBits(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nPitch(0)
	, m_nBPP(0)
	, m_iTransparentColor(-1)
	, m_clrTransparentColor((COLORREF)(-1))
	, m_bGif(FALSE)
	, m_nFrameCount(1)
	, m_nCurFrame(0)
	, m_plFrameDelay(NULL)
	, m_nLoopCount(0)
	, m_bHasAlphaChannel(false)
	, m_bIsDIBSection(false)
{
	m_rcShadow.SetRect(0, 0, 0, 0);
}

IUIImage::~IUIImage() throw()
{
	if (m_nAddRef == 0)
	{
		Destroy();
	}
}

int IUIImage::AddRef()
{
	m_nAddRef++;

	return m_nAddRef;
}

int IUIImage::Release()
{
	m_nAddRef--;

	if (m_nAddRef <= 0)
	{
		Destroy();
	}

	return m_nAddRef;
}

CIUIString IUIImage::GetSafeImageName() const throw()
{
	if (this == NULL)
	{
		return _T("");
	}

	return m_strImageName;
}

HBITMAP IUIImage::GetSafeHBITMAP() const throw()
{
	if (this == NULL)
	{
		return NULL;
	}

	return m_hBitmap;
}

IUIImage::operator HBITMAP() const throw()
{
	return m_hBitmap;
}

int IUIImage::SetProject(class CIUIProject *pProject)
{
	m_pOwner = pProject;

	return 0;
}

CIUIProject *IUIImage::GetOwner()
{
	return m_pOwner;
}

int IUIImage::Load(__inout IStream *pStream) throw()
{
	Gdiplus::Bitmap bmSrc(pStream);
	if (bmSrc.GetLastStatus() != Gdiplus::Ok)
	{
		return E_FAIL;
	}

	return CreateFromGdiplusBitmap(bmSrc);
}

// 文件完整路径
int IUIImage::Load(LPCTSTR pszFileName) throw()
{
	CIUIString strFileExt = PathFindExtension(pszFileName);

	if (strFileExt.CompareNoCase(_T(".bmp")) == 0)
	{
		m_hBitmap = LoadHBITMAP(pszFileName);
		if (m_hBitmap == NULL)
		{
			return E_FAIL;
		}

		SIZE size;
		int nRet = GetBitmapSizeG(m_hBitmap, &size);
		if (nRet != 0)
		{
			return E_FAIL;
		}

		m_nWidth = size.cx;
		m_nHeight = size.cy;

		return S_OK;
	}
	else
	{
		Gdiplus::Bitmap bmSrc((const wchar_t *)(_bstr_t)pszFileName);
		if (bmSrc.GetLastStatus() != Gdiplus::Ok)
		{
			return E_FAIL;
		}

		// 因为使用Gdiplus绘制性能太差，所以，如果加载到的是gif,需要把Gif每一帧从左至右排列
		// 成一个新的HBITMAP，然后用GDI绘制
		GUID guid;
		bmSrc.GetRawFormat(&guid);
		if (guid == Gdiplus::ImageFormatGIF)
		{
			//
			// 得到帧数
			//
			UINT nCount = bmSrc.GetFrameDimensionsCount();
			GUID *pDimensionIDs = new GUID [nCount];
			bmSrc.GetFrameDimensionsList(pDimensionIDs, nCount);
			m_nFrameCount = bmSrc.GetFrameCount(&pDimensionIDs[0]);
			delete []pDimensionIDs;
			pDimensionIDs = NULL;

			//
			// 得到帧时间间隔，(pFrameDelayPropertyItem + i)->value为各帧时间间隔，每帧间隔可能不等。
			// 单位：百分之一秒，这个值乘以10，就是毫秒数。
			//

			// 得到帧延时属性所需占用的内存大小
			UINT nFrameDelayPropertySize = bmSrc.GetPropertyItemSize(PropertyTagFrameDelay);
			if (nFrameDelayPropertySize > 0)
			{
				// 分配帧延时属性内存
				Gdiplus::PropertyItem *pFrameDelayPropertyItem = (Gdiplus::PropertyItem *)new BYTE[nFrameDelayPropertySize];

				// 得到帧延时属性
				bmSrc.GetPropertyItem(PropertyTagFrameDelay,
					nFrameDelayPropertySize, pFrameDelayPropertyItem);
				_ASSERT(pFrameDelayPropertyItem->type == PropertyTagTypeLong);

				// plFrameDelay为保存帧延时的数组
				// PropertyItem::length表示PropertyItem::value占用的字节数
				// PropertyItem::type表示PropertyItem::value保存的数据类型
				// 比如PropertyItem::type为PropertyTagTypeLong时，表示PropertyItem::value
				// 是一个LONG数组

				m_plFrameDelay = new LONG [m_nFrameCount];

				_ASSERT(pFrameDelayPropertyItem->length / sizeof(LONG) == m_nFrameCount);
				LONG *plFrameDelay = (LONG *)pFrameDelayPropertyItem->value;
				for (UINT j = 0; j < m_nFrameCount; ++j)
				{
					m_plFrameDelay[j] = (*(plFrameDelay + j)) * 10;
					// 有些Gif帧间隔为0，这种情况，我们把它改为40ms，表示1秒播放25帧。
					// TODO：研究一下业界有没有规定如何处理0间隔。
					if (m_plFrameDelay[j] == 0)
					{
						m_plFrameDelay[j] = 40;
					}
				}

				delete [] pFrameDelayPropertyItem;
				pFrameDelayPropertyItem = NULL;
			}

			//
			// 得到循环播放次数。
			//
			UINT nLoopCountPropertySize = bmSrc.GetPropertyItemSize(PropertyTagLoopCount);
			if (nLoopCountPropertySize > 0)
			{
				Gdiplus::PropertyItem *pLoopCountItem = (Gdiplus::PropertyItem *)new BYTE[nLoopCountPropertySize];

				bmSrc.GetPropertyItem(PropertyTagLoopCount,
					nLoopCountPropertySize, pLoopCountItem);
				_ASSERT(pLoopCountItem->type == PropertyTagTypeShort);
				_ASSERT(pLoopCountItem->length / sizeof(short) == 1);
				m_nLoopCount = *((short *)(pLoopCountItem->value));

				delete [](BYTE *)pLoopCountItem;
			}

			int nWidth = bmSrc.GetWidth();
			m_nHeight = bmSrc.GetHeight();

			// 把每一帧从右到右排列
			Gdiplus::Bitmap *pImageAll = NULL;
			if (m_nFrameCount > 1)
			{
				pImageAll = ::new Gdiplus::Bitmap(nWidth * m_nFrameCount, m_nHeight, PixelFormat32bppARGB);
				Gdiplus::Graphics gra(pImageAll);

				for (int i = 0; i < (int)m_nFrameCount; ++i)
				{
					GUID activeGuid = Gdiplus::FrameDimensionTime;
					bmSrc.SelectActiveFrame(&activeGuid, i);
					Gdiplus::Rect rcDraw(i * nWidth, 0, nWidth, m_nHeight);
					gra.DrawImage(&bmSrc, rcDraw);
				}
			}

			if (pImageAll == NULL)
			{
				return -2;
			}

			int nRet = CreateFromGdiplusBitmap(*pImageAll);
			::delete pImageAll;
			pImageAll = NULL;

			// 由于CreateFromGdiplusBitmap会设置很多属性，所以需要重置这些属性
			m_bGif = TRUE;
			m_nWidth = nWidth;

			return nRet;
		}
		else
		{
			// 判断是否是自定义格式的动画序列帧图片
			// 只要找到"图片名.ani"文件（例如"button.png.ani"），就认为图片是动画序列帧图片
			// "图片名.ani"文件描述了"图片名"这个动画图片的信息，包含帧间隔，播放次数等
			// .ani文件格式：
			// [SequentialFrames]
			// FrameCount=5	// 帧数
			// FrameDelay="100,100,120,150,100" // 帧延时，每帧延时用逗号分隔，有多少帧，有就多少延时
			// LoopCount=3	// 循环播放次数，0为无限循环
			CIUIString strAni = pszFileName;
			strAni += _T(".ani");
			if (PathFileExists(strAni))
			{
				//
				// 得到帧数
				//
#define ANI_APP_NAME	_T("SequentialFrames")
				m_nFrameCount = GetPrivateProfileInt(ANI_APP_NAME, _T("FrameCount"), 1, strAni);

				if (m_nFrameCount > 0)
				{
					m_plFrameDelay = new LONG[m_nFrameCount];
					memset(m_plFrameDelay, 40, sizeof(LONG) * m_nFrameCount);

					//
					// 得到帧时间间隔，各帧时间间隔，每帧间隔可能不等。
					//
					TCHAR *szFrameDelay = new TCHAR[m_nFrameCount * 8];
					DWORD dwSize = GetPrivateProfileString(ANI_APP_NAME, _T("FrameDelay"),
							NULL, szFrameDelay, m_nFrameCount * 8, strAni);

					CIUIString strDelay;
					int i = 0;
					while (ExtractSubString(strDelay, szFrameDelay, i, ','))
					{
						if (i >= (int)m_nFrameCount)
						{
							break;
						}

						LONG lDelay = IntFromString(strDelay);
						m_plFrameDelay[i] = lDelay;

						i++;
					}

					delete szFrameDelay;
					szFrameDelay = NULL;

					//
					// 得到循环播放次数。
					//
					m_nLoopCount = GetPrivateProfileInt(ANI_APP_NAME, _T("LoopCount"), 1, strAni);

					int nRet = CreateFromGdiplusBitmap(bmSrc);

					// 由于CreateFromGdiplusBitmap会设置很多属性，所以需要重置这些属性
					m_bGif = TRUE;

					if (m_nFrameCount > 0)
					{
						m_nWidth = m_nWidth / m_nFrameCount;
					}
					else
					{
						_ASSERT(FALSE);
					}

					return nRet;
				}
				else
				{
					_ASSERT(FALSE);
					return -1;
				}
			}
			else
			{
				// 普通图片
				return CreateFromGdiplusBitmap(bmSrc);
			}
		}
	}
}

int IUIImage::LoadFromResource(LPCTSTR lpszID, LPCTSTR lpszResType)
{
	// 由于这里的lpszID有可能是通过MAKEINTRESOURCE(ID)把数字强转成的ID，
	// 所以不要用PathFindExtension来得到扩展名，否则会引起崩溃。
	CIUIString strExt = GetFileExt(lpszID, FALSE);
	if (strExt.CompareNoCase(_T("bmp")) == 0)
	{
		m_hBitmap = ::LoadBitmap(IUIGetResourceHandle(), lpszID);
		if (m_hBitmap == NULL)
		{
			return E_FAIL;
		}

		SIZE size;
		int nRet = GetBitmapSizeG(m_hBitmap, &size);
		if (nRet != 0)
		{
			return E_FAIL;
		}

		m_nWidth = size.cx;
		m_nHeight = size.cy;
	}
	else
	{
		HRSRC hRsrc = FindResource(NULL, lpszID, lpszResType);
		if (NULL == hRsrc)
		{
			return E_FAIL;
		}

		DWORD dwSize = SizeofResource(NULL, hRsrc);
		if (0 == dwSize)
		{
			return E_FAIL;
		}

		HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
		if (NULL == hGlobal)
		{
			return E_FAIL;
		}

		LPVOID pBuffer = LockResource(hGlobal);
		if (NULL == pBuffer)
		{
			return E_FAIL;
		}

		HGLOBAL hData = LocalAlloc(GPTR, dwSize);
		if (hData == NULL)
		{
			LocalFree(hData);
			return E_FAIL;
		}

		char *pS = (char *)GlobalLock(hData);
		if (pS == NULL)
		{
			GlobalUnlock(hData);
			LocalFree(hData);
			return E_FAIL;
		}

		memcpy(pS, (LPCTSTR)pBuffer, dwSize);

		IStream *pIStream = NULL;
		if (CreateStreamOnHGlobal(hData, FALSE, &pIStream) == S_OK)
		{
			int nRet = Load(pIStream);
			pIStream->Release();

			if (nRet != 0)
			{
				GlobalUnlock(hData);
				LocalFree(hData);

				return -1;
			}
		}

		GlobalUnlock(hData);
		LocalFree(hData);
	}

	return 0;
}

// 图片名（不含路径）或图片的资源ID
int IUIImage::AutoLoad(LPCTSTR lpszImageName) throw()
{
	if (m_pOwner == NULL)
	{
		_ASSERT(FALSE);
		return -1;
	}

	if (m_pOwner->IsLoadSkinFromResource())
	{
		return LoadFromResource(GetFileNameHexCode(lpszImageName, FALSE),
				m_pOwner->GetUIResourceType());
	}
	else
	{
		_ASSERT(m_pOwner != NULL);
		CIUIString strFolder = m_pOwner->GetProjectPathWithoutTitle();
		return Load(strFolder + lpszImageName);
	}
}

int IUIImage::SafeLoadSavedImage()
{
	if (this == NULL)
	{
		return -1;
	}

	if (m_hBitmap != NULL)
	{
		return 1;
	}

	int nRet = AutoLoad(GetSafeImageName());
	if (nRet != 0)
	{
		nRet = Load(GetSafeImageName());

		if (nRet)
		{
			return nRet;
		}
	}

	return 0;
}

int IUIImage::Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags/* = 0*/) throw()
{
	return CreateEx(nWidth, nHeight, nBPP, BI_RGB, NULL, dwFlags);
}

int IUIImage::CreateEx(int nWidth, int nHeight, int nBPP, DWORD eCompression, const DWORD *pdwBitmasks/* = NULL*/, DWORD dwFlags/* = 0*/) throw()
{
	LPBITMAPINFO pbmi = NULL;

	if (dwFlags & createAlphaChannel)
	{
		_ASSERT((nBPP == 32) && (eCompression == BI_RGB));
	}

	pbmi = (LPBITMAPINFO)new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	if (pbmi == NULL)
	{
		return -1;
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

	// If you pass a pointer to the 4th param of CreateDIBSection, Must set to NULL after not use it,
	// otherwise, BoundsChecker report: 因重新分配发生内存泄漏：由 CreateDIBSection 分配的地址 0xXXXXXXXX (XX)。
	// If you want use the value of pBits, you can call:
	// DIBSECTION dibsection;
	// ::GetObject(m_hBitmap, sizeof(DIBSECTION), &dibsection);
	// the dibsection.dsBm.bmBits is you want
	HBITMAP hBitmap = ::CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, NULL, NULL, 0);

	delete [] pbmi;
	if (hBitmap == NULL)
	{
		return -2;
	}

	Attach(hBitmap, (nHeight < 0) ? DIBOR_TOPDOWN : DIBOR_BOTTOMUP);

	if (dwFlags & createAlphaChannel)
	{
		m_bHasAlphaChannel = true;
	}

	return 0;
}

int IUIImage::Attach(HBITMAP hBitmap, DIBOrientation eOrientation/* = DIBOR_DEFAULT*/) throw()
{
	if (hBitmap == NULL)
	{
		return -1;
	}

	_ASSERT(m_hBitmap == NULL);

	m_hBitmap = hBitmap;

	UpdateBitmapInfo(eOrientation);

	return 0;
}

int IUIImage::Attach(HGPBITMAP hgpBitmap)
{
	if (hgpBitmap == NULL)
	{
		return -1;
	}

	return CreateFromGdiplusBitmap(*hgpBitmap);
}

// Move himgSrc to this, and clear himgSrc.
int IUIImage::Attach(IUIImage &himgSrc)
{
	m_hBitmap = himgSrc.m_hBitmap;
	m_pBits = himgSrc.m_pBits;
	m_nWidth = himgSrc.m_nWidth;
	m_nHeight = himgSrc.m_nHeight;
	m_nBPP = himgSrc.m_nBPP;
	m_nPitch = himgSrc.m_nPitch;
	m_iTransparentColor = himgSrc.m_iTransparentColor;
	m_clrTransparentColor = himgSrc.m_clrTransparentColor;
	m_bGif = himgSrc.m_bGif;
	m_nFrameCount = himgSrc.m_nFrameCount;
	m_nCurFrame = himgSrc.m_nCurFrame;
	if (himgSrc.m_plFrameDelay != NULL && m_nFrameCount > 0)
	{
		memcpy(m_plFrameDelay, himgSrc.m_plFrameDelay, sizeof(LONG) * m_nFrameCount);
	}
	m_nLoopCount = himgSrc.m_nLoopCount;
	m_bHasAlphaChannel = himgSrc.m_bHasAlphaChannel;
	m_bIsDIBSection = himgSrc.m_bIsDIBSection;
	m_rcShadow = himgSrc.m_rcShadow;

	himgSrc.Detach();

	return 0;
}

HBITMAP IUIImage::Detach() throw()
{
	HBITMAP hBitmap;

	//	_ASSERT(m_hBitmap != NULL);

	hBitmap = m_hBitmap;
	m_hBitmap = NULL;
	m_pBits = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_nPitch = 0;
	m_iTransparentColor = -1;
	m_clrTransparentColor = (COLORREF)(-1);
	m_bGif = false;
	m_nFrameCount = 1;
	m_nCurFrame = 0;
	m_plFrameDelay = NULL;
	m_nLoopCount = 0;
	m_bHasAlphaChannel = false;
	m_bIsDIBSection = false;
	m_rcShadow.SetRect(0, 0, 0, 0);

	return hBitmap;
}

int IUIImage::ConvertToDPIImage(int nCurDPIX, int nCurDPIY)
{
	if (this == NULL)
	{
		return -2;
	}

	if (m_hBitmap == NULL)
	{
		return -1;
	}

	int nDPIX = 96;
	int nDPIY = 96;

	// DPI not change
	if (nDPIX == nCurDPIX && nDPIY == nCurDPIY)
	{
		return 0;
	}
	else
	{
		int nNewWidth = MulDiv(nCurDPIX, GetWidth(), nDPIX);
		int nNewHeight = MulDiv(nCurDPIY, GetHeight(), nDPIY);

		//
		if (nDPIX == -1 || nDPIX == 0 || nDPIY == -1 || nDPIY == 0)
		{
			nNewWidth = GetWidth();
			nNewHeight = GetHeight();
		}

		HIUIIMAGE himgNew = NULL;
		int nRet = StretchBitmap(this, &himgNew, nNewWidth, nNewHeight, 0, 0, -1, -1);
		if (nRet != 0)
		{
			return nRet;
		}

		return Attach(himgNew->GetSafeHBITMAP());
	}
}

int IUIImage::CreateFromGdiplusBitmap(__inout Gdiplus::Bitmap &bmSrc) throw()
{
	//
	// Adjust image
	//
	Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
	UINT nBPP = 32;
	DWORD dwFlags = 0;
	Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
	if (eSrcPixelFormat & PixelFormatGDI)
	{
		nBPP = Gdiplus::GetPixelFormatSize(eSrcPixelFormat);
		eDestPixelFormat = eSrcPixelFormat;
	}

	if (Gdiplus::IsAlphaPixelFormat(eSrcPixelFormat))
	{
		nBPP = 32;
		dwFlags |= createAlphaChannel;
		eDestPixelFormat = PixelFormat32bppARGB;
	}

	//
	// Convert Gdiplus image to HBITMAP
	//
	{
		int nRet = Create(bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags);
		if (nRet != 0)
		{
			return E_FAIL;
		}
	}

	//
	//
	//
	// Adjust
	Gdiplus::ColorPalette *pPalette = NULL;
	if (Gdiplus::IsIndexedPixelFormat(eSrcPixelFormat))
	{
		UINT nPaletteSize = bmSrc.GetPaletteSize();
		pPalette = (Gdiplus::ColorPalette *)new char[nPaletteSize];
		if (pPalette == NULL)
		{
			return E_OUTOFMEMORY;
		}

		bmSrc.GetPalette(pPalette, nPaletteSize);

		RGBQUAD argbPalette[256];
		// ATLENSURE_RETURN( (pPalette->Count > 0) && (pPalette->Count <= 256) );
		for (UINT iColor = 0; iColor < pPalette->Count; iColor++)
		{
			Gdiplus::ARGB color = pPalette->Entries[iColor];
			argbPalette[iColor].rgbRed = (BYTE)((color >> RED_SHIFT) & 0xff);
			argbPalette[iColor].rgbGreen = (BYTE)((color >> GREEN_SHIFT) & 0xff);
			argbPalette[iColor].rgbBlue = (BYTE)((color >> BLUE_SHIFT) & 0xff);
			argbPalette[iColor].rgbReserved = 0;
		}

		// SetColorTable(0, pPalette->Count, argbPalette);
	}

	if (eDestPixelFormat == eSrcPixelFormat)
	{
		// The pixel formats are identical, so just memcpy the rows.
		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(0, 0, m_nWidth, m_nHeight);
		if (bmSrc.LockBits(&rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data) != Gdiplus::Ok)
		{
			return E_OUTOFMEMORY;
		}

		size_t nBytesPerRow = _IUIAtlAlignUp(nBPP * m_nWidth, 8) / 8;
		BYTE *pbDestRow = static_cast<BYTE *>(m_pBits);
		BYTE *pbSrcRow = static_cast<BYTE *>(data.Scan0);
		for (int y = 0; y < m_nHeight; y++)
		{
			memcpy(pbDestRow, pbSrcRow, nBytesPerRow);

			// the pbDestRow will out of range at last step of loop.
			if (y < m_nHeight - 1)
			{
				pbDestRow += m_nPitch;
			}

			pbSrcRow += data.Stride;
		}

		bmSrc.UnlockBits(&data);
	}
	else
	{
		// Let GDI+ work its magic
		Gdiplus::Bitmap bmDest(m_nWidth, m_nHeight, m_nPitch, eDestPixelFormat, static_cast<BYTE *>(m_pBits));
		Gdiplus::Graphics gDest(&bmDest);

		gDest.DrawImage(&bmSrc, 0, 0);
	}

	if (m_nBPP == 32) // More, Has alpha channel
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			for (int y = 0; y < m_nHeight; y++)
			{
				byte *pByte = (byte *)(m_pBits) + (y * m_nPitch) + ((x * m_nBPP) / 8);
				pByte[0] = pByte[0] * pByte[3] / 255;
				pByte[1] = pByte[1] * pByte[3] / 255;
				pByte[2] = pByte[2] * pByte[3] / 255;
			}
		}
	}

	return S_OK;
}

int IUIImage::UpdateBitmapInfo(DIBOrientation eOrientation)
{
	DIBSECTION dibsection;
	int nBytes;

	nBytes = ::GetObject(m_hBitmap, sizeof(DIBSECTION), &dibsection);
	if (nBytes == sizeof(DIBSECTION))
	{
		m_bIsDIBSection = true;
		m_nWidth = dibsection.dsBmih.biWidth;
		m_nHeight = abs(dibsection.dsBmih.biHeight);
		m_nBPP = dibsection.dsBmih.biBitCount;
		m_nPitch = ComputePitch(m_nWidth, m_nBPP);
		void *pOrigBits = dibsection.dsBm.bmBits;
		if (eOrientation == DIBOR_DEFAULT)
		{
			eOrientation = (dibsection.dsBmih.biHeight > 0) ? DIBOR_BOTTOMUP : DIBOR_TOPDOWN;
		}
		if (eOrientation == DIBOR_BOTTOMUP)
		{
			m_pBits = LPBYTE(pOrigBits) + ((m_nHeight - 1) * m_nPitch);
			m_nPitch = -m_nPitch;
		}

		pOrigBits = NULL;
	}
	else
	{
		// Non-DIBSection
		_ASSERT(nBytes == sizeof(BITMAP));
		m_bIsDIBSection = false;
		m_nWidth = dibsection.dsBm.bmWidth;
		m_nHeight = dibsection.dsBm.bmHeight;
		m_nBPP = dibsection.dsBm.bmBitsPixel;
		m_nPitch = 0;
		m_pBits = 0;
	}

	m_iTransparentColor = -1;
	m_bHasAlphaChannel = false;

	return 0;
}

int IUIImage::Destroy() throw()
{
	if (m_hBitmap != NULL)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	m_hBitmap = NULL;
	m_pBits = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_nPitch = 0;
	m_iTransparentColor = -1;
	m_clrTransparentColor = (COLORREF)(-1);
	m_bGif = FALSE;
	m_nFrameCount = 1;
	m_nCurFrame = 0;
	delete [] m_plFrameDelay;
	m_plFrameDelay = NULL;
	m_nLoopCount = 0;
	m_bHasAlphaChannel = false;
	m_bIsDIBSection = false;
	m_rcShadow.SetRect(0, 0, 0, 0);

	return 0;
}

int IUIImage::GetBPP() const throw()
{
	_ASSERT(m_hBitmap != NULL);

	return (m_nBPP);
}

int IUIImage::GetHeight() const throw()
{
	CHECK_AND_LOAD(0);

	return m_nHeight;
}

int IUIImage::GetWidth() const throw()
{
	CHECK_AND_LOAD(0);

	return m_nWidth;
}

bool IUIImage::IsHasAlphaChannel() const throw()
{
	if (this == NULL)
	{
		return false;
	}

	IUIImage *pThis = (IUIImage *)this;
	pThis->SafeLoadSavedImage();

	return m_bHasAlphaChannel;
}

BOOL IUIImage::IsGif() const throw()
{
	return (this == NULL) ? FALSE : m_bGif;
}

UINT IUIImage::GetGifFrameCount() const throw()
{
	return m_nFrameCount;
}

int IUIImage::GetGifCurFrame() const throw()
{
	return m_nCurFrame;
}

int IUIImage::SetGifCurFrame(int nCurFrame) throw()
{
	if (nCurFrame >= (int)m_nFrameCount)
	{
		return -1;
	}

	m_nCurFrame = nCurFrame;

	return 0;
}

int IUIImage::GetGifFrameDelay(__out LONG **pplFrameDelay, __out UINT *pnFrameCount) const
{
	if (pplFrameDelay == NULL || pnFrameCount == NULL)
	{
		return -1;
	}

	*pplFrameDelay = m_plFrameDelay;
	*pnFrameCount = m_nFrameCount;

	return 0;
}

int IUIImage::GetGifLoopCount() const
{
	return m_nLoopCount;
}

int IUIImage::SetShadow(LPCRECT lprcShadow) throw()
{
	if (lprcShadow == NULL)
	{
		return -1;
	}

	m_rcShadow = *lprcShadow;

	return 0;
}

CIUIRect IUIImage::GetShowdow() const throw()
{
	return m_rcShadow;
}

//////////////////////////////////////////////////////////////
// static 函数。 对图片操作

//   _____________
//   | 0 | 1 | 2 |
//   |------------
//   | 3 | 4 | 5 |
//   |------------
//   | 6 | 7 | 8 |
//    ------------
int IUIImage::NineGridBitmapG(
	HBITMAP *phDstBmp,
	int nDestWidth,
	int nDestHeight,
	HBITMAP hSrcBmp,
	LPCRECT lprcResizeArg)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}
	if (phDstBmp == NULL)
	{
		return -2;
	}

	_ASSERT(phDstBmp != NULL);
	HDC hDC = ::GetDC(::GetDesktopWindow());

	// store the Font object before call DefWindowProc
	HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

	HDC memDst = ::CreateCompatibleDC(hDC);
	*phDstBmp = ::CreateCompatibleBitmap(hDC, nDestWidth, nDestHeight);
	_ASSERT(*phDstBmp != NULL);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, *phDstBmp);

	NineGridBltG(memDst, 0, 0, nDestWidth, nDestHeight, hSrcBmp, lprcResizeArg);

	::SelectObject(memDst, hBmpOld);
	::DeleteDC(memDst);

	::SelectObject(hDC, hFontOld);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::PartNineGridBitmapG(
	HBITMAP *phDstBmp,
	int nDestWidth,
	int nDestHeight,
	HBITMAP hSrcBmp,
	LPCRECT lprcResizeArg,
	int nXOriginSrc,
	int nYOriginSrc,
	int nSrcWidth,
	int nSrcHeight)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}
	if (phDstBmp == NULL)
	{
		return -2;
	}

	SIZE size;
	GetBitmapSizeG(hSrcBmp, &size);

	if (-1 == nSrcWidth)
	{
		nSrcWidth = size.cx;
	}
	if (-1 == nSrcHeight)
	{
		nSrcHeight = size.cy;
	}

	_ASSERT(phDstBmp != NULL);
	HDC hDC = ::GetDC(::GetDesktopWindow());

	// store the Font object before call DefWindowProc
	HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

	HDC memDst = ::CreateCompatibleDC(hDC);
	*phDstBmp = ::CreateCompatibleBitmap(hDC, nDestWidth, nDestHeight);
	_ASSERT(*phDstBmp != NULL);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, *phDstBmp);

	PartNineGridBltG(memDst, CIUIRect(0, 0, nDestWidth, nDestHeight), hSrcBmp,
		CIUIRect(nXOriginSrc, nYOriginSrc, nXOriginSrc + nSrcWidth, nYOriginSrc + nSrcHeight),
		lprcResizeArg);

	::SelectObject(memDst, hBmpOld);
	::DeleteDC(memDst);

	::SelectObject(hDC, hFontOld);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

// Not use 9 Grid
int IUIImage::StretchBitmapG(
	HBITMAP hSrcBmp,
	HBITMAP *phDstBmp,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc,
	int nYOriginSrc,
	int nSrcWidth,
	int nSrcHeight)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}
	if (phDstBmp == NULL)
	{
		return -2;
	}

	SIZE size;
	GetBitmapSizeG(hSrcBmp, &size);

	if (-1 == nSrcWidth)
	{
		nSrcWidth = size.cx;
	}

	if (-1 == nSrcHeight)
	{
		nSrcHeight = size.cy;
	}

	_ASSERT(phDstBmp != NULL);
	HDC hDC = ::GetDC(::GetDesktopWindow());

	// store the Font object before call DefWindowProc
	HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

	HDC memDst = ::CreateCompatibleDC(hDC);
	*phDstBmp = ::CreateCompatibleBitmap(hDC, nDestWidth, nDestHeight);
	_ASSERT(*phDstBmp != NULL);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, *phDstBmp);

	HDC memDC = ::CreateCompatibleDC(hDC);
	HBITMAP hSrcOld = (HBITMAP)::SelectObject(memDC, hSrcBmp);

	::SetStretchBltMode(memDst, COLORONCOLOR);
	::StretchBlt(memDst, 0, 0, nDestWidth, nDestHeight,
		memDC, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, SRCCOPY);
	::SelectObject(memDC, hSrcOld);

	::SelectObject(memDst, hBmpOld);
	::DeleteDC(memDst);

	::SelectObject(hDC, hFontOld);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::TileBitmapG(
	HBITMAP hSrcBmp,
	HBITMAP *phDstBmp,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc/* = 0*/,
	int nYOriginSrc/* = 0*/,
	int nSrcWidth/* = -1*/,
	int nSrcHeight/* = -1*/)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}
	if (phDstBmp == NULL)
	{
		return -2;
	}

	_ASSERT(phDstBmp != NULL);
	HDC hDC = ::GetDC(::GetDesktopWindow());

	// store the Font object before call DefWindowProc
	HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

	HDC memDst = ::CreateCompatibleDC(hDC);
	*phDstBmp = ::CreateCompatibleBitmap(hDC, nDestWidth, nDestHeight);
	_ASSERT(*phDstBmp != NULL);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, *phDstBmp);

	HDC memSrc = ::CreateCompatibleDC(hDC);
	HBITMAP hbmpOldSrc = (HBITMAP)::SelectObject(memSrc, hSrcBmp);

	TileBlt(memDst, 0, 0, nDestWidth, nDestHeight,
		memSrc, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, 255);

	::SelectObject(memSrc, hbmpOldSrc);
	::SelectObject(memDst, hBmpOld);
	::DeleteDC(memDst);

	::SelectObject(hDC, hFontOld);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::CopyBitmapG(HBITMAP hSrc, HBITMAP *phDest)
{
	if (phDest == NULL)
	{
		return -1;
	}

	CIUISize size;
	GetBitmapSizeG(hSrc, &size);

	HDC hDC = ::GetDC(::GetDesktopWindow());

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	*phDest = ::CreateCompatibleBitmap(hDC, size.cx, size.cy);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, *phDest);

	HDC hMemDCSrc = ::CreateCompatibleDC(hDC);
	HBITMAP hOldSrc = (HBITMAP)::SelectObject(hMemDCSrc, hSrc);

	::BitBlt(hMemDC, 0, 0, size.cx, size.cy, hMemDCSrc, 0, 0, SRCCOPY);

	::SelectObject(hMemDCSrc, hOldSrc);
	::SelectObject(hMemDC, hOldBitmap);

	::DeleteDC(hMemDCSrc);
	::DeleteDC(hMemDC);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::SubBitmapG(HBITMAP hSrcBmp, HBITMAP *phDstBmp, const CIUIRect &rcSub)
{
	if (hSrcBmp == NULL)
	{
		return -1;
	}
	if (phDstBmp == NULL)
	{
		return -2;
	}

	HDC hDC = ::GetDC(::GetDesktopWindow());

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(hMemDC, hSrcBmp);

	HDC hMemDestDC = ::CreateCompatibleDC(hDC);
	*phDstBmp = ::CreateCompatibleBitmap(hDC, rcSub.Width(), rcSub.Height());
	_ASSERT(*phDstBmp != NULL);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(hMemDestDC, *phDstBmp);

	::BitBlt(hMemDestDC, 0, 0, rcSub.Width(), rcSub.Height(), hMemDC, rcSub.left, rcSub.top, SRCCOPY);
	::SelectObject(hMemDC, pSrcBmpOld);
	::SelectObject(hMemDestDC, hBmpOld);

	::DeleteDC(hMemDestDC);
	::DeleteDC(hMemDC);
	::ReleaseDC(::GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::SubBitmapG(HBITMAP hSrcBmp, HBITMAP *phDstBmp, int nX, int nY, int nWidth, int nHeight)
{
	return SubBitmapG(hSrcBmp, phDstBmp, CIUIRect(nX, nY, nX + nWidth, nY + nHeight));
}

int IUIImage::GetBitmapSizeG(HBITMAP obj, LPSIZE size)
{
	if (obj == NULL)
	{
		return -1;
	}

	BITMAP bitmap;
	int nCount = ::GetObject(obj, sizeof(BITMAP), &bitmap);
	if (nCount == 0)
	{
		return -2;
	}

	size->cx = bitmap.bmWidth;
	size->cy = bitmap.bmHeight;

	return 0;
}

int IUIImage::ZoomImage(
	HIUIIMAGE himgSrc,
	IMAGE_RESIZE_MODE eIrm,
	LPCRECT lprcResizeArg,
	HIUIIMAGE *phimgDest,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc,
	int nYOriginSrc,
	int nSrcWidth,
	int nSrcHeight)
{
	if (himgSrc == NULL || phimgDest == NULL || nDestWidth <= 0 || nDestHeight <= 0)
	{
		return -1;
	}

	if (eIrm == IRM_9GRID)
	{
		PartNineGridBitmap(himgSrc, lprcResizeArg, phimgDest, nDestWidth, nDestHeight,
			nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);
	}
	else if (eIrm == IRM_TILE)
	{
		TileBitmap(himgSrc, phimgDest, nDestWidth, nDestHeight,
			nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);
	}
	else if (eIrm == IRM_STRETCH || eIrm == IRM_STRETCH_HIGH_QUALITY)
	{
		StretchBitmap(himgSrc, phimgDest, nDestWidth, nDestHeight,
			nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);
	}
	else
	{
	}

	return 0;
}

// 9 Grid
int IUIImage::NineGridBitmap(HIUIIMAGE himgSrc, LPCRECT lprcResizeArg,
	HIUIIMAGE *phimgDest, int nDestWidth, int nDestHeight)
{
	if (himgSrc == NULL)
	{
		return -1;
	}
	if (phimgDest == NULL)
	{
		return -2;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC hDC = ::GetDC(::GetDesktopWindow());

		// store the Font object before call DefWindowProc
		HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

		HDC memDst = ::CreateCompatibleDC(hDC);

		*phimgDest = new IUIImage;
		(*phimgDest)->Create(nDestWidth, nDestHeight, 32, IUIImage::createAlphaChannel);

		GUID guid;
		CoCreateGuid(&guid);
		wchar_t wszName[64] = {0};
		StringFromGUID2(guid, wszName, sizeof(wszName));
		AddCustomImage(himgSrc->GetOwner(), (LPCTSTR)_bstr_t(wszName), (*phimgDest));

		HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, (*phimgDest)->GetSafeHBITMAP());

		int nRet = IUINineGridBlt(memDst, 0, 0, nDestWidth, nDestHeight, himgSrc, lprcResizeArg);

		::SelectObject(memDst, hBmpOld);
		::DeleteDC(memDst);

		::SelectObject(hDC, hFontOld);
		::ReleaseDC(::GetDesktopWindow(), hDC);

		return nRet;
	}
	else
	{
		int nResizingX = lprcResizeArg->left;
		int nResizingY = lprcResizeArg->top;
		int nTilePixelX = lprcResizeArg->right;
		int nTilePixelY = lprcResizeArg->bottom;

		HBITMAP hbmpDest = NULL;
		int nRet = NineGridBitmapG(&hbmpDest, nDestWidth, nDestHeight,
				himgSrc->GetSafeHBITMAP(), lprcResizeArg);
		if (nRet != 0)
		{
			return nRet;
		}

		*phimgDest = new IUIImage;
		return (*phimgDest)->Attach(hbmpDest);
	}
}

int IUIImage::PartNineGridBitmap(
	HIUIIMAGE himgSrc,
	LPCRECT lprcResizeArg,
	int nPart,
	int nPartIndex,
	HIUIIMAGE *phimgDest,
	int nDestWidth,
	int nDestHeight)
{
	himgSrc->SafeLoadSavedImage();

	if (himgSrc->GetSafeHBITMAP() == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}
	if (phimgDest == NULL)
	{
		return -2;
	}
	if (nPart <= 0)
	{
		return -5;
	}
	if (nPartIndex < 0)
	{
		return -3;
	}
	if (nPartIndex >= nPart)
	{
		return -4;
	}
	if (nDestWidth <= 0 || nDestHeight <= 0)
	{
		return -5;
	}

	int nX = himgSrc->GetWidth() * nPartIndex / nPart;
	return PartNineGridBitmap(himgSrc, lprcResizeArg, phimgDest, nDestWidth, nDestHeight,
			nX, 0, himgSrc->GetWidth() / nPart, himgSrc->GetHeight());
}

int IUIImage::PartNineGridBitmap(
	HIUIIMAGE himgSrc,
	LPCRECT lprcResizeArg,
	HIUIIMAGE *phimgDest,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc,
	int nYOriginSrc,
	int nSrcWidth,
	int nSrcHeight)
{
	himgSrc->SafeLoadSavedImage();

	if (himgSrc->GetSafeHBITMAP() == NULL || lprcResizeArg == NULL)
	{
		return -1;
	}
	if (phimgDest == NULL)
	{
		return -2;
	}
	if (nDestWidth <= 0 || nDestHeight <= 0)
	{
		return -5;
	}

	if (-1 == nSrcWidth)
	{
		nSrcWidth = himgSrc->GetWidth();
	}
	if (-1 == nSrcHeight)
	{
		nSrcHeight = himgSrc->GetHeight();
	}

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC hDC = ::GetDC(::GetDesktopWindow());

		// store the Font object before call DefWindowProc
		HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

		HDC memDst = ::CreateCompatibleDC(hDC);

		*phimgDest = new IUIImage;
		(*phimgDest)->m_pOwner = himgSrc->m_pOwner;
		(*phimgDest)->Create(nDestWidth, nDestHeight, 32, IUIImage::createAlphaChannel);
		HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, (*phimgDest)->GetSafeHBITMAP());
		_ASSERT(hBmpOld != NULL);

		int nRet = IUIPartNineGridBlt(memDst, CIUIRect(0, 0, nDestWidth, nDestHeight),
				himgSrc, lprcResizeArg,
				CIUIRect(nXOriginSrc, nYOriginSrc, nXOriginSrc + nSrcWidth, nYOriginSrc + nSrcHeight));

		::SelectObject(memDst, hBmpOld);
		::DeleteDC(memDst);

		::SelectObject(hDC, hFontOld);
		::ReleaseDC(::GetDesktopWindow(), hDC);

		AddCustomImage(himgSrc->GetOwner(), *phimgDest);

		return nRet;
	}
	else
	{
		HBITMAP hbmpDest = NULL;
		int nRet = PartNineGridBitmapG(&hbmpDest, nDestWidth, nDestHeight,
				himgSrc->GetSafeHBITMAP(), lprcResizeArg,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);
		if (nRet != 0)
		{
			return nRet;
		}

		*phimgDest = new IUIImage;
		(*phimgDest)->Attach(hbmpDest);
		AddCustomImage(himgSrc->GetOwner(), *phimgDest);

		return 0;
	}
}

int IUIImage::StretchBitmap(
	HIUIIMAGE himgSrc,
	HIUIIMAGE *phimgDest,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc,
	int nYOriginSrc,
	int nSrcWidth,
	int nSrcHeight)
{
	if (himgSrc == NULL)
	{
		return -1;
	}
	if (phimgDest == NULL)
	{
		return -2;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (-1 == nSrcWidth)
	{
		nSrcWidth = himgSrc->GetWidth();
	}
	if (-1 == nSrcHeight)
	{
		nSrcHeight = himgSrc->GetHeight();
	}

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC hDC = ::GetDC(::GetDesktopWindow());

		// store the Font object before call DefWindowProc
		HFONT hFontOld = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);

		HDC memDst = ::CreateCompatibleDC(hDC);
		(*phimgDest)->Create(nDestWidth, nDestHeight, 32, IUIImage::createAlphaChannel);
		HBITMAP hBmpOld = (HBITMAP)::SelectObject(memDst, (*phimgDest)->GetSafeHBITMAP());

		int nRet = IUIPartStretchBlt(memDst, 0, 0, nDestWidth, nDestHeight, himgSrc,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight, IRM_STRETCH);

		::SelectObject(memDst, hBmpOld);
		::DeleteDC(memDst);

		::SelectObject(hDC, hFontOld);
		::ReleaseDC(::GetDesktopWindow(), hDC);

		return nRet;
	}
	else
	{
		HBITMAP hbmpDest = NULL;
		int nRet = StretchBitmapG(*himgSrc, &hbmpDest, nDestWidth, nDestHeight,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);

		GUID guid;
		CoCreateGuid(&guid);
		wchar_t wszName[64] = {0};
		StringFromGUID2(guid, wszName, sizeof(wszName));

		USES_CONVERSION;
		*phimgDest = CreateEmptyIUIImage(himgSrc->GetOwner(), W2T(wszName));
		(*phimgDest)->Attach(hbmpDest);

		return nRet;
	}

	return -3;
}

int IUIImage::TileBitmap(
	HIUIIMAGE himgSrc,
	HIUIIMAGE *phimgDest,
	int nDestWidth,
	int nDestHeight,
	int nXOriginSrc/* = 0*/,
	int nYOriginSrc/* = 0*/,
	int nSrcWidth/* = -1*/,
	int nSrcHeight/* = -1*/)
{
	if (himgSrc == NULL)
	{
		return -1;
	}
	if (phimgDest == NULL)
	{
		return -2;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (nSrcWidth == -1)
	{
		nSrcWidth = himgSrc->GetWidth();
	}
	if (nSrcHeight == -1)
	{
		nSrcHeight = himgSrc->GetHeight();
	}

	HDC hDC = ::GetDC(::GetDesktopWindow());

	if (himgSrc->IsHasAlphaChannel())
	{
		//
		HDC dcMemDest = ::CreateCompatibleDC(hDC);
		BOOL bRet = (*phimgDest)->Create(nDestWidth, nDestHeight, 32,
				IUIImage::createAlphaChannel);
		HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcMemDest, (HBITMAP)(*phimgDest));

		//
		int nRet = IUITileBlt(dcMemDest, CIUIRect(0, 0, nDestWidth, nDestHeight),
				himgSrc, nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);

		::SelectObject(dcMemDest, hBmpOld);

		// release resource
		::DeleteDC(dcMemDest);
		::ReleaseDC(::GetDesktopWindow(), hDC);

		return nRet;
	}
	else
	{
		HBITMAP hbmpDest = NULL;
		int nRet = TileBitmapG(*himgSrc, &hbmpDest, nDestWidth, nDestHeight,
				nXOriginSrc, nYOriginSrc, nSrcWidth, nSrcHeight);
		(*phimgDest)->Attach(hbmpDest);

		return nRet;
	}

	return -3;
}

int IUIImage::CopyBitmap(HIUIIMAGE himgSrc, HIUIIMAGE *phimgDest)
{
	if (himgSrc == NULL)
	{
		return -1;
	}
	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (phimgDest == NULL)
	{
		return -2;
	}

	HDC hDC = GetDC(GetDesktopWindow());
	HDC hMemDC = CreateCompatibleDC(hDC);

	if (himgSrc->IsHasAlphaChannel())
	{
		(*phimgDest)->Create(himgSrc->GetWidth(), himgSrc->GetHeight(), 32,
			IUIImage::createAlphaChannel);

		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, (*phimgDest)->GetSafeHBITMAP());

		IUIBitBlt(hMemDC, 0, 0, himgSrc->GetWidth(), himgSrc->GetHeight(),
			himgSrc, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
	}
	else
	{
		HBITMAP hbmpDest = ::CreateCompatibleBitmap(hDC,
				himgSrc->GetWidth(), himgSrc->GetHeight());
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hbmpDest);

		IUIBitBlt(hMemDC, 0, 0, himgSrc->GetWidth(), himgSrc->GetHeight(),
			himgSrc, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);

		(*phimgDest)->Attach(hbmpDest);
	}

	DeleteDC(hMemDC);
	ReleaseDC(GetDesktopWindow(), hDC);

	return 0;
}

int IUIImage::SubBitmap(HIUIIMAGE himgSrc, HIUIIMAGE *phimgDest, const CIUIRect &rcSub)
{
	if (himgSrc == NULL)
	{
		return -1;
	}

	himgSrc->SafeLoadSavedImage();
	if (himgSrc->GetSafeHBITMAP() == NULL)
	{
		return -3;
	}

	if (himgSrc->IsHasAlphaChannel())
	{
		HDC hDC = GetDC(GetDesktopWindow());

		//
		HDC hMemDestDC = CreateCompatibleDC(hDC);

		*phimgDest = new IUIImage;
		int nRet = (*phimgDest)->Create(rcSub.Width(), rcSub.Height(), 32,
				IUIImage::createAlphaChannel);

		GUID guid;
		CoCreateGuid(&guid);
		wchar_t wszName[64] = {0};
		StringFromGUID2(guid, wszName, sizeof(wszName));
		AddCustomImage(himgSrc->GetOwner(), (LPCTSTR)_bstr_t(wszName), (*phimgDest));

		HBITMAP hBmpOld = (HBITMAP)::SelectObject(hMemDestDC,
				(*phimgDest)->GetSafeHBITMAP());

		//
		HDC hMemSrcDC = CreateCompatibleDC(hDC);
		HBITMAP pSrcBmpOld = (HBITMAP)::SelectObject(hMemSrcDC, (HBITMAP)(*himgSrc));

		nRet = StretchAlphaBlend(hMemDestDC, 0, 0, rcSub.Width(), rcSub.Height(),
				hMemSrcDC, rcSub.left, rcSub.top, rcSub.Width(), rcSub.Height(), 255);

		::SelectObject(hMemSrcDC, pSrcBmpOld);
		::SelectObject(hMemDestDC, hBmpOld);

		DeleteDC(hMemSrcDC);
		DeleteDC(hMemDestDC);
		ReleaseDC(GetDesktopWindow(), hDC);

		return 0;
	}
	else
	{
		HBITMAP hbmpDest = NULL;
		int nRet = SubBitmapG(*himgSrc, &hbmpDest, rcSub);
		if (nRet != 0 || hbmpDest == NULL)
		{
			return nRet;
		}

		GUID guid;
		CoCreateGuid(&guid);
		wchar_t wszName[64] = {0};
		StringFromGUID2(guid, wszName, sizeof(wszName));

		AddCustomImage(himgSrc->GetOwner(), (LPCTSTR)_bstr_t(wszName), hbmpDest);
		*phimgDest = CreateIUIImage(himgSrc->GetOwner(), (LPCTSTR)_bstr_t(wszName));
		ReleaseIUIImage(himgSrc->GetOwner(), (LPCTSTR)_bstr_t(wszName));

		return 0;
	}
}

int IUIImage::SubBitmap(HIUIIMAGE himgSrc, HIUIIMAGE *phimgDst,
	int nX, int nY, int nWidth, int nHeight)
{
	return SubBitmap(himgSrc, phimgDst, CIUIRect(nX, nY, nX + nWidth, nY + nHeight));
}

HBITMAP IUIImage::LoadHBITMAP(LPCTSTR lpszFile)
{
	return (HBITMAP)LoadImage(IUIGetResourceHandle(),
			lpszFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

HBITMAP IUIImage::LoadHBITMAPFromRes(UINT uID)
{
	return ::LoadBitmap(IUIGetResourceHandle(), MAKEINTRESOURCE(uID));
}

// Auto load image from file or rc use gdi mode by CUIMgr::m_bLoadFromFile.
HBITMAP IUIImage::AutoLoadHBITMAP(CIUIProject *pProject, LPCTSTR lpszFile)
{
	if (pProject->IsLoadSkinFromResource())
	{
		CIUIString strFile = GetFileNameHexCode(lpszFile, FALSE);
		return ::LoadBitmap(IUIGetResourceHandle(), strFile);
	}
	else
	{
		return LoadHBITMAP(lpszFile);
	}
}

LPCTSTR IUIImage::GetFileNameHexCode(LPCTSTR lpszImageName, BOOL bEncodeExt)
{
	// Must utf-16.
	std::wstring wstrFileName = (LPCWSTR)_bstr_t(lpszImageName);
	// ABC.bmp and abc.bmp must be the same one hex code.
	std::transform(wstrFileName.begin(), wstrFileName.end(), wstrFileName.begin(), towlower);

	if (bEncodeExt)
	{
		return (LPCTSTR)_bstr_t(GetHexCodeW(wstrFileName.c_str()).c_str());
	}

	// Prefix, the hex code string may a number, So need a prefix.
	std::wstring wstrFileHexCode = L"I";

	std::wstring wstrTitle = GetFileTitleExW(wstrFileName.c_str());
	std::wstring wstrExt = GetFileExtW(wstrFileName.c_str(), TRUE);
	wstrFileHexCode += (LPCWSTR)_bstr_t(GetHexCodeW(wstrTitle.c_str()).c_str());
	wstrFileHexCode += wstrExt;

	return (LPCTSTR)_bstr_t(wstrFileHexCode.c_str());
}

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


// iuiChNil is left for backward compatibility
static WCHAR IUI::iuiChNilW = '\0';

// For an empty string, m_pchData will point here
// (note: avoids special case of checking for NULL m_pchData)
// empty string data (and locked)
int _afxInitDataW[] = { -1, 0, 0, 0 };
CIUIStringDataW *_iuiDataNilW = (CIUIStringDataW *) &_afxInitDataW;
LPCWSTR IUI::_iuiPchNilW = (LPCWSTR)(((BYTE *)&_afxInitDataW) + sizeof(CIUIStringDataW));

///////////////////////////////////////////////////////////////////////////////
// CIUIStringW conversion helpers (these use the current system locale)

#if (_MSC_VER == 1200)	// VC6.0
int _mbstowcsz(wchar_t *wcstr, const char *mbstr, size_t count)
{
	if (count == 0 && wcstr != NULL)
	{
		return 0;
	}

	int result = ::MultiByteToWideChar(CP_ACP, 0, mbstr, -1,
			wcstr, count);
	_ASSERT(wcstr == NULL || result <= (int)count);
	if (result > 0)
	{
		wcstr[result - 1] = 0;
	}
	return result;
}
#endif // (_MSC_VER == 1200)	// VC6.0

static LPWSTR __cdecl IUICharNextW(LPCWSTR psz) throw()
{
	return const_cast< LPWSTR >(psz + 1);
}

#if(_MSC_VER <= 1200) // VC6.0
size_t __cdecl _tclen(const wchar_t *_cpc)
{
	return (_cpc, 1);
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

#ifdef _AFXDLL
CIUIStringW::CIUIStringW()
{
	Init();
}
#else
CIUIStringW::CIUIStringW()
{
	m_pchData = iuiEmptyStringW.m_pchData;
	m_pchDataConvert = NULL;
}
#endif // _AFXDLL

CIUIStringW::CIUIStringW(const CIUIStringW &stringSrc)
{
	_ASSERT(stringSrc.GetData()->nRefs != 0);
	if (stringSrc.GetData()->nRefs >= 0)
	{
		_ASSERT(stringSrc.GetData() != _iuiDataNilW);
		m_pchData = stringSrc.m_pchData;
		InterlockedIncrement(&GetData()->nRefs);
	}
	else
	{
		Init();
		*this = stringSrc.m_pchData;
	}

	m_pchDataConvert = NULL;
}

CIUIStringW::CIUIStringW(WCHAR ch, int nLength/* = 1*/)
{
	Init();
	if (nLength >= 1)
	{
		AllocBuffer(nLength);
#ifdef _UNICODE
		for (int i = 0; i < nLength; i++)
		{
			m_pchData[i] = ch;
		}
#else
		memset(m_pchData, ch, nLength);
#endif
	}

	m_pchDataConvert = NULL;
}

CIUIStringW::CIUIStringW(LPCWSTR lpsz)
{
	Init();
	if (lpsz != NULL && HIWORD(lpsz) == NULL)
	{
		UINT nID = LOWORD((DWORD)lpsz);
		if (!LoadString(nID))
		{
			IUITRACEA("Warning: implicit LoadString(%u) failed\n", nID);
		}
	}
	else
	{
		int nLen = SafeStrlen(lpsz);
		if (nLen != 0)
		{
			AllocBuffer(nLen);
			memcpy(m_pchData, lpsz, nLen * sizeof(WCHAR));
		}
	}

	m_pchDataConvert = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion constructors

CIUIStringW::CIUIStringW(LPCSTR lpsz)
{
	Init();
	int nSrcLen = lpsz != NULL ? lstrlenA(lpsz) : 0;
	if (nSrcLen != 0)
	{
		AllocBuffer(nSrcLen);
		_mbstowcsz(m_pchData, lpsz, nSrcLen + 1);
		ReleaseBuffer();
	}

	m_pchDataConvert = NULL;
}

CIUIStringW::CIUIStringW(LPCWSTR lpch, int nLength)
{
	Init();
	if (nLength != 0)
	{
		_ASSERT(IUIIsValidAddress(lpch, nLength, FALSE));
		AllocBuffer(nLength);
		memcpy(m_pchData, lpch, nLength * sizeof(WCHAR));
	}

	m_pchDataConvert = NULL;
}

CIUIStringW::CIUIStringW(LPCSTR lpsz, int nLength)
{
	Init();
	if (nLength != 0)
	{
		AllocBuffer(nLength);
		int n = ::MultiByteToWideChar(CP_ACP, 0, lpsz, nLength, m_pchData, nLength + 1);
		ReleaseBuffer(n >= 0 ? n : -1);
	}

	m_pchDataConvert = NULL;
}

CIUIStringW::CIUIStringW(const unsigned char *lpsz)
{
	Init();
	*this = (LPCSTR)lpsz;

	m_pchDataConvert = NULL;
}

void CIUIStringW::AllocBuffer(int nLen)
// always allocate one extra character for '\0' termination
// assumes [optimistically] that data length will equal allocation length
{
	_ASSERT(nLen >= 0);
	_ASSERT(nLen <= INT_MAX - 1);  // max size (enough room for 1 extra)

	if (nLen == 0)
	{
		Init();
	}
	else
	{
		// 如果执行到下面的new操作崩溃，大部分原因是把一种类型的指针，强转成另一种
		// 类型后使用，从而把内存破坏引起的。 基本上与下面的new无关
		int nL = sizeof(CIUIStringDataW) + (nLen + 1) * sizeof(WCHAR);
		CIUIStringDataW *pData = (CIUIStringDataW *)new BYTE[nL];
		pData->nAllocLength = nLen;
		pData->nRefs = 1;
		pData->data()[nLen] = '\0';
		pData->nDataLength = nLen;
		m_pchData = pData->data();
	}
}

void CIUIStringW::FreeData(CIUIStringDataW *pData)
{
	delete[](BYTE *)pData;
}

void CIUIStringW::Release()
{
	if (GetData() != _iuiDataNilW)
	{
		_ASSERT(GetData()->nRefs != 0);
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
		{
			FreeData(GetData());
		}
		Init();
	}
}

void PASCAL CIUIStringW::Release(CIUIStringDataW *pData)
{
	if (pData != _iuiDataNilW)
	{
		_ASSERT(pData->nRefs != 0);
		if (InterlockedDecrement(&pData->nRefs) <= 0)
		{
			FreeData(pData);
		}
	}
}

void CIUIStringW::Empty()
{
	if (GetData()->nDataLength == 0)
	{
		return;
	}
	if (GetData()->nRefs >= 0)
	{
		Release();
	}
	else
	{
		*this = &iuiChNilW;
	}
	_ASSERT(GetData()->nDataLength == 0);
	_ASSERT(GetData()->nRefs < 0 || GetData()->nAllocLength == 0);
}

void CIUIStringW::CopyBeforeWrite()
{
	if (GetData()->nRefs > 1)
	{
		CIUIStringDataW *pData = GetData();
		Release();
		AllocBuffer(pData->nDataLength);
		memcpy(m_pchData, pData->data(), (pData->nDataLength + 1) * sizeof(WCHAR));
	}
	_ASSERT(GetData()->nRefs <= 1);
}

void CIUIStringW::AllocBeforeWrite(int nLen)
{
	if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
	{
		Release();
		AllocBuffer(nLen);
	}
	_ASSERT(GetData()->nRefs <= 1);
}

CIUIStringW::~CIUIStringW()
//  free any attached data
{
	if (GetData() != _iuiDataNilW)
	{
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
		{
			FreeData(GetData());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

void CIUIStringW::AllocCopy(CIUIStringW &dest, int nCopyLen, int nCopyIndex,
	int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen;
	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		dest.AllocBuffer(nNewLen);
		memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen * sizeof(WCHAR));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostic support

// #ifdef _DEBUG
// 	CDumpContext &operator<<(CDumpContext &dc, const CIUIStringW &string)
// 	{
// 		dc << string.m_pchData;
// 		return dc;
// 	}
// #endif //_DEBUG

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
//  All assign a new value to the string
//      (a) first see if the buffer is big enough
//      (b) if enough room, copy on top of old buffer, set size and type
//      (c) otherwise free old string data, and create a new one
//
//  All routines return the new string (but as a 'const CIUIStringW&' so that
//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

void CIUIStringW::AssignCopy(int nSrcLen, LPCWSTR lpszSrcData)
{
	AllocBeforeWrite(nSrcLen);
	memcpy(m_pchData, lpszSrcData, nSrcLen * sizeof(WCHAR));
	GetData()->nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
}

const CIUIStringW &CIUIStringW::operator=(const CIUIStringW &stringSrc)
{
	if (m_pchData != stringSrc.m_pchData)
	{
		if ((GetData()->nRefs < 0 && GetData() != _iuiDataNilW) ||
			stringSrc.GetData()->nRefs < 0)
		{
			// actual copy necessary since one of the strings is locked
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
		}
		else
		{
			// can just copy references around
			Release();
			_ASSERT(stringSrc.GetData() != _iuiDataNilW);
			m_pchData = stringSrc.m_pchData;
			InterlockedIncrement(&GetData()->nRefs);
		}
	}
	return *this;
}

const CIUIStringW &CIUIStringW::operator=(LPCWSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringW(lpsz));
	AssignCopy(SafeStrlen(lpsz), lpsz);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion assignment

const CIUIStringW &CIUIStringW::operator=(LPCSTR lpsz)
{
	int nSrcLen = lpsz != NULL ? lstrlenA(lpsz) : 0;
	AllocBeforeWrite(nSrcLen);
	_mbstowcsz(m_pchData, lpsz, nSrcLen + 1);
	ReleaseBuffer();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//      There are three variants:
//          CIUIStringW + CIUIStringW
// and for ? = WCHAR, LPCWSTR
//          CIUIStringW + ?
//          ? + CIUIStringW

void CIUIStringW::ConcatCopy(int nSrc1Len, LPCWSTR lpszSrc1Data,
	int nSrc2Len, LPCWSTR lpszSrc2Data)
{
	// -- master concatenation routine
	// Concatenate two sources
	// -- assume that 'this' is a new CIUIStringW object

	int nNewLen = nSrc1Len + nSrc2Len;
	if (nNewLen != 0)
	{
		AllocBuffer(nNewLen);
		memcpy(m_pchData, lpszSrc1Data, nSrc1Len * sizeof(WCHAR));
		memcpy(m_pchData + nSrc1Len, lpszSrc2Data, nSrc2Len * sizeof(WCHAR));
	}
}

CIUIStringW IUI::operator+(const CIUIStringW &string1, const CIUIStringW &string2)
{
	CIUIStringW s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
		string2.GetData()->nDataLength, string2.m_pchData);
	return s;
}

CIUIStringW IUI::operator+(const CIUIStringW &string1, WCHAR ch)
{
	CIUIStringW s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData, 1, &ch);
	return s;
}

CIUIStringW IUI::operator+(WCHAR ch, const CIUIStringW &string)
{
	CIUIStringW s;
	s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pchData);
	return s;
}

CIUIStringW IUI::operator+(const CIUIStringW &string, LPCWSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringW(lpsz));
	CIUIStringW s;
	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData,
		CIUIStringW::SafeStrlen(lpsz), lpsz);
	return s;
}

CIUIStringW IUI::operator+(LPCWSTR lpsz, const CIUIStringW &string)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringW(lpsz));
	CIUIStringW s;
	s.ConcatCopy(CIUIStringW::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength,
		string.m_pchData);
	return s;
}

#ifdef _UNICODE
// 	friend CIUIStringW IUIAPI operator+(const CIUIStringW &string, char ch);
// 	friend CIUIStringW IUIAPI operator+(char ch, const CIUIStringW &string);
#endif // _UNICODE

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void CIUIStringW::ConcatInPlace(int nSrcLen, LPCWSTR lpszSrcData)
{
	//  -- the main routine for += operators

	// concatenating an empty string is a no-op!
	if (nSrcLen == 0)
	{
		return;
	}

	// if the buffer is too small, or we have a width mis-match, just
	//   allocate a new buffer (slow but sure)
	if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
	{
		// we have to grow the buffer, use the ConcatCopy routine
		CIUIStringDataW *pOldData = GetData();
		ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
		_ASSERT(pOldData != NULL);
		CIUIStringW::Release(pOldData);
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(m_pchData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(WCHAR));
		GetData()->nDataLength += nSrcLen;
		_ASSERT(GetData()->nDataLength <= GetData()->nAllocLength);
		m_pchData[GetData()->nDataLength] = '\0';
	}
}

const CIUIStringW &CIUIStringW::operator+=(LPCWSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringW(lpsz));
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const CIUIStringW &CIUIStringW::operator+=(WCHAR ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const CIUIStringW &CIUIStringW::operator+=(const CIUIStringW &string)
{
	ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
	return *this;
}

#ifndef _AFX_NO_BSTR_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// OLE BSTR support

BSTR CIUIStringW::AllocSysString() const
{
	BSTR bstr = ::SysAllocStringLen(m_pchData, GetData()->nDataLength);
	if (bstr == NULL)
	{
		return NULL;
		// AfxThrowMemoryException();
	}

	return bstr;
}

BSTR CIUIStringW::SetSysString(BSTR *pbstr) const
{
	_ASSERT(IUIIsValidAddress(pbstr, sizeof(BSTR)));

	if (!::SysReAllocStringLen(pbstr, m_pchData, GetData()->nDataLength))
	{
		return NULL;
		// AfxThrowMemoryException();
	}

	_ASSERT(*pbstr != NULL);
	return *pbstr;
}

#endif // _AFX_NO_BSTR_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

LPWSTR CIUIStringW::GetBuffer(int nMinBufLength)
{
	_ASSERT(nMinBufLength >= 0);

	if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
	{
#ifdef _DEBUG
		// give a warning in case locked string becomes unlocked
		if (GetData() != _iuiDataNilW && GetData()->nRefs < 0)
		{
			IUITRACEA("Warning: GetBuffer on locked CIUIStringW creates unlocked CIUIStringW!\n");
		}
#endif
		// we have to grow the buffer
		CIUIStringDataW *pOldData = GetData();
		int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
		{
			nMinBufLength = nOldLen;
		}
		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, pOldData->data(), (nOldLen + 1) * sizeof(WCHAR));
		GetData()->nDataLength = nOldLen;
		CIUIStringW::Release(pOldData);
	}
	_ASSERT(GetData()->nRefs <= 1);

	// return a pointer to the character storage for this string
	_ASSERT(m_pchData != NULL);
	return m_pchData;
}

void CIUIStringW::ReleaseBuffer(int nNewLength)
{
	CopyBeforeWrite();  // just in case GetBuffer was not called

	if (nNewLength == -1)
	{
		nNewLength = lstrlenW(m_pchData);    // zero terminated
	}

	_ASSERT(nNewLength <= GetData()->nAllocLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
}

LPWSTR CIUIStringW::GetBufferSetLength(int nNewLength)
{
	_ASSERT(nNewLength >= 0);

	GetBuffer(nNewLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
	return m_pchData;
}

void CIUIStringW::FreeExtra()
{
	_ASSERT(GetData()->nDataLength <= GetData()->nAllocLength);
	if (GetData()->nDataLength != GetData()->nAllocLength)
	{
		CIUIStringDataW *pOldData = GetData();
		AllocBuffer(GetData()->nDataLength);
		memcpy(m_pchData, pOldData->data(), pOldData->nDataLength * sizeof(WCHAR));
		_ASSERT(m_pchData[GetData()->nDataLength] == '\0');
		CIUIStringW::Release(pOldData);
	}
	_ASSERT(GetData() != NULL);
}

LPWSTR CIUIStringW::LockBuffer()
{
	LPWSTR lpsz = GetBuffer(0);
	GetData()->nRefs = -1;
	return lpsz;
}

void CIUIStringW::UnlockBuffer()
{
	_ASSERT(GetData()->nRefs == -1);
	if (GetData() != _iuiDataNilW)
	{
		GetData()->nRefs = 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Commonly used routines (rarely used routines in STREX.CPP)

int CIUIStringW::Find(WCHAR ch) const
{
	return Find(ch, 0);
}

int CIUIStringW::Find(WCHAR ch, int nStart) const
{
	int nLength = GetData()->nDataLength;
	if (nStart >= nLength)
	{
		return -1;
	}

	// find first single character
	LPWSTR lpsz = wcschr(m_pchData + nStart, (_TUCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

int CIUIStringW::FindOneOf(LPCWSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	LPWSTR lpsz = wcspbrk(m_pchData, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

void CIUIStringW::MakeUpper()
{
	CopyBeforeWrite();

	_wcsupr(m_pchData);
}

void CIUIStringW::MakeLower()
{
	CopyBeforeWrite();
	_wcslwr(m_pchData);
}

void CIUIStringW::MakeReverse()
{
	CopyBeforeWrite();
	_wcsrev(m_pchData);
}

void CIUIStringW::SetAt(int nIndex, WCHAR ch)
{
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);

	CopyBeforeWrite();
	m_pchData[nIndex] = ch;
}

// LPWSTR AfxA2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars)
// {
// 	if (lpa == NULL)
// 		return NULL;
// 	_ASSERT(lpw != NULL);
// 	// verify that no illegal character present
// 	// since lpw was allocated based on the size of lpa
// 	// don't worry about the number of chars
// 	lpw[0] = '\0';
// 	VERIFY(MultiByteToWideChar(CP_ACP, 0, lpa, -1, lpw, nChars));
// 	return lpw;
// }
//
// LPSTR AfxW2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars)
// {
// 	if (lpw == NULL)
// 		return NULL;
// 	_ASSERT(lpa != NULL);
// 	// verify that no illegal character present
// 	// since lpa was allocated based on the size of lpw
// 	// don't worry about the number of chars
// 	lpa[0] = '\0';
// 	VERIFY(WideCharToMultiByte(CP_ACP, 0, lpw, -1, lpa, nChars, NULL, NULL));
// 	return lpa;
// }

///////////////////////////////////////////////////////////////////////////////

// CIUIStringW
CIUIStringDataW *CIUIStringW::GetData() const
{
	_ASSERT(m_pchData != NULL);
	return ((CIUIStringDataW *)m_pchData) - 1;
}
void CIUIStringW::Init()
{
	m_pchData = iuiEmptyStringW.m_pchData;
}
const CIUIStringW &CIUIStringW::operator=(const unsigned char *lpsz)
{
	*this = (LPCSTR)lpsz;
	return *this;
}
const CIUIStringW &CIUIStringW::operator+=(char ch)
{
	*this += (WCHAR)ch;
	return *this;
}
const CIUIStringW &CIUIStringW::operator=(char ch)
{
	*this = (WCHAR)ch;
	return *this;
}
CIUIStringW operator+(const CIUIStringW &string, char ch)
{
	return string + (WCHAR)ch;
}
CIUIStringW operator+(char ch, const CIUIStringW &string)
{
	return (WCHAR)ch + string;
}

int CIUIStringW::GetLength() const
{
	return GetData()->nDataLength;
}
int CIUIStringW::GetAllocLength() const
{
	return GetData()->nAllocLength;
}
BOOL CIUIStringW::IsEmpty() const
{
	return GetData()->nDataLength == 0;
}
CIUIStringW::operator LPCWSTR() const
{
	return m_pchData;
}
int PASCAL CIUIStringW::SafeStrlen(LPCWSTR lpsz)
{
	return (lpsz == NULL) ? 0 : lstrlenW(lpsz);
}

// CIUIStringW support (windows specific)
int CIUIStringW::Compare(LPCWSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // MBCS/Unicode aware
	return wcscmp(m_pchData, lpsz);
}
int CIUIStringW::CompareNoCase(LPCWSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // MBCS/Unicode aware
	return _wcsicmp(m_pchData, lpsz);
}
// CIUIStringW::Collate is often slower than Compare but is MBSC/Unicode
//  aware as well as locale-sensitive with respect to sort order.
int CIUIStringW::Collate(LPCWSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // locale sensitive
	return wcscoll(m_pchData, lpsz);
}
int CIUIStringW::CollateNoCase(LPCWSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // locale sensitive
	return _wcsicoll(m_pchData, lpsz);
}

WCHAR CIUIStringW::GetAt(int nIndex) const
{
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
WCHAR CIUIStringW::operator[](int nIndex) const
{
	// same as GetAt
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
bool IUI::operator==(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) == 0;
}
bool IUI::operator==(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) == 0;
}
bool IUI::operator==(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) == 0;
}
bool IUI::operator!=(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) != 0;
}
bool IUI::operator!=(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) != 0;
}
bool IUI::operator!=(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) != 0;
}
bool IUI::operator<(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) < 0;
}
bool IUI::operator<(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) < 0;
}
bool IUI::operator<(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) > 0;
}
bool IUI::operator>(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) > 0;
}
bool IUI::operator>(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) > 0;
}
bool IUI::operator>(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) < 0;
}
bool IUI::operator<=(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) <= 0;
}
bool IUI::operator<=(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) <= 0;
}
bool IUI::operator<=(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) >= 0;
}
bool IUI::operator>=(const CIUIStringW &s1, const CIUIStringW &s2)
{
	return s1.Compare(s2) >= 0;
}
bool IUI::operator>=(const CIUIStringW &s1, LPCWSTR s2)
{
	return s1.Compare(s2) >= 0;
}
bool IUI::operator>=(LPCWSTR s1, const CIUIStringW &s2)
{
	return s2.Compare(s1) <= 0;
}

/////////////////////////////////////////////////////////////////////////////
// Windows extensions to strings

#ifdef _UNICODE
#define CHAR_FUDGE 1    // one WCHAR unused is good enough
#else
#define CHAR_FUDGE 2    // two BYTES unused for case of DBC last char
#endif

int IUILoadString(UINT nID, LPWSTR lpszBuf, UINT nMaxBuf)
{
	// 		_ASSERT(lpszBuf != NULL);
	// #ifdef _DEBUG
	// 		// LoadString without annoying warning from the Debug kernel if the
	// 		//  segment containing the string is not present
	// 		if (::FindResource(AfxGetResourceHandle(),
	// 				MAKEINTRESOURCE((nID >> 4) + 1), RT_STRING) == NULL)
	// 		{
	// 			lpszBuf[0] = '\0';
	// 			return 0; // not found
	// 		}
	// #endif //_DEBUG
	// 		int nLen = ::LoadString(AfxGetResourceHandle(), nID, lpszBuf, nMaxBuf);
	// 		if (nLen == 0)
	// 		{
	// 			lpszBuf[0] = '\0';
	// 		}
	// 		return nLen;
	return 0;
}

BOOL CIUIStringW::LoadString(UINT nID)
{
	// try fixed buffer first (to avoid wasting space in the heap)
	WCHAR szTemp[256];
	int nLen = IUILoadString(nID, szTemp, _countof(szTemp));
	if (_countof(szTemp) - nLen > CHAR_FUDGE)
	{
		*this = szTemp;
		return nLen > 0;
	}

	// try buffer size of 512, then larger size until entire string is retrieved
	int nSize = 256;
	do
	{
		nSize += 256;
		nLen = IUILoadString(nID, GetBuffer(nSize - 1), nSize);
	}
	while (nSize - nLen <= CHAR_FUDGE);
	ReleaseBuffer();

	return nLen > 0;
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators

const CIUIStringW &CIUIStringW::operator=(WCHAR ch)
{
	AssignCopy(1, &ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

int CIUIStringW::Delete(int nIndex, int nCount /* = 1 */)
{
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	int nNewLength = GetData()->nDataLength;
	if (nCount > 0 && nIndex < nNewLength)
	{
		CopyBeforeWrite();
		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

		memcpy(m_pchData + nIndex,
			m_pchData + nIndex + nCount, nBytesToCopy * sizeof(WCHAR));
		GetData()->nDataLength = nNewLength - nCount;
	}

	return nNewLength;
}

int CIUIStringW::Insert(int nIndex, WCHAR ch)
{
	CopyBeforeWrite();

	if (nIndex < 0)
	{
		nIndex = 0;
	}

	int nNewLength = GetData()->nDataLength;
	if (nIndex > nNewLength)
	{
		nIndex = nNewLength;
	}
	nNewLength++;

	if (GetData()->nAllocLength < nNewLength)
	{
		CIUIStringDataW *pOldData = GetData();
		LPWSTR pstr = m_pchData;
		AllocBuffer(nNewLength);
		memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(WCHAR));
		CIUIStringW::Release(pOldData);
	}

	// move existing bytes down
	memcpy(m_pchData + nIndex + 1,
		m_pchData + nIndex, (nNewLength - nIndex) * sizeof(WCHAR));
	m_pchData[nIndex] = ch;
	GetData()->nDataLength = nNewLength;

	return nNewLength;
}

int CIUIStringW::Insert(int nIndex, LPCWSTR pstr)
{
	if (nIndex < 0)
	{
		nIndex = 0;
	}

	int nInsertLength = SafeStrlen(pstr);
	int nNewLength = GetData()->nDataLength;
	if (nInsertLength > 0)
	{
		CopyBeforeWrite();
		if (nIndex > nNewLength)
		{
			nIndex = nNewLength;
		}
		nNewLength += nInsertLength;

		if (GetData()->nAllocLength < nNewLength)
		{
			CIUIStringDataW *pOldData = GetData();
			LPWSTR pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(WCHAR));
			CIUIStringW::Release(pOldData);
		}

		// move existing bytes down
		memcpy(m_pchData + nIndex + nInsertLength,
			m_pchData + nIndex,
			(nNewLength - nIndex - nInsertLength + 1) * sizeof(WCHAR));
		memcpy(m_pchData + nIndex,
			pstr, nInsertLength * sizeof(WCHAR));
		GetData()->nDataLength = nNewLength;
	}

	return nNewLength;
}

int CIUIStringW::Replace(WCHAR chOld, WCHAR chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		// otherwise modify each character that matches in the string
		CopyBeforeWrite();
		LPWSTR psz = m_pchData;
		LPWSTR pszEnd = psz + GetData()->nDataLength;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = IUICharNextW(psz);
		}
	}
	return nCount;
}

int CIUIStringW::Replace(LPCWSTR lpszOld, LPCWSTR lpszNew)
{
	// can't have empty or NULL lpszOld

	int nSourceLen = SafeStrlen(lpszOld);
	if (nSourceLen == 0)
	{
		return 0;
	}
	int nReplacementLen = SafeStrlen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	LPWSTR lpszStart = m_pchData;
	LPWSTR lpszEnd = m_pchData + GetData()->nDataLength;
	LPWSTR lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += lstrlenW(lpszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		CopyBeforeWrite();

		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		int nOldLength = GetData()->nDataLength;
		int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;
		if (GetData()->nAllocLength < nNewLength || GetData()->nRefs > 1)
		{
			CIUIStringDataW *pOldData = GetData();
			LPWSTR pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, pOldData->nDataLength * sizeof(WCHAR));
			CIUIStringW::Release(pOldData);
		}
		// else, we just do it in-place
		lpszStart = m_pchData;
		lpszEnd = m_pchData + GetData()->nDataLength;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - m_pchData + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(WCHAR));
				memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(WCHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += lstrlenW(lpszStart) + 1;
		}
		_ASSERT(m_pchData[nNewLength] == '\0');
		GetData()->nDataLength = nNewLength;
	}

	return nCount;
}

int CIUIStringW::Remove(WCHAR chRemove)
{
	CopyBeforeWrite();

	LPWSTR pstrSource = m_pchData;
	LPWSTR pstrDest = m_pchData;
	LPWSTR pstrEnd = m_pchData + GetData()->nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = IUICharNextW(pstrDest);
		}
		pstrSource = IUICharNextW(pstrSource);
	}
	*pstrDest = '\0';
	int nCount = pstrSource - pstrDest;
	GetData()->nDataLength -= nCount;

	return nCount;
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

CIUIStringW CIUIStringW::Mid(int nFirst) const
{
	return Mid(nFirst, GetData()->nDataLength - nFirst);
}

CIUIStringW CIUIStringW::Mid(int nFirst, int nCount) const
{
	// out-of-bounds requests return sensible things
	if (nFirst < 0)
	{
		nFirst = 0;
	}
	if (nCount < 0)
	{
		nCount = 0;
	}

	if (nFirst + nCount > GetData()->nDataLength)
	{
		nCount = GetData()->nDataLength - nFirst;
	}
	if (nFirst > GetData()->nDataLength)
	{
		nCount = 0;
	}

	_ASSERT(nFirst >= 0);
	_ASSERT(nFirst + nCount <= GetData()->nDataLength);

	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == GetData()->nDataLength)
	{
		return *this;
	}

	CIUIStringW dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CIUIStringW CIUIStringW::Right(int nCount) const
{
	if (nCount < 0)
	{
		nCount = 0;
	}
	if (nCount >= GetData()->nDataLength)
	{
		return *this;
	}

	CIUIStringW dest;
	AllocCopy(dest, nCount, GetData()->nDataLength - nCount, 0);
	return dest;
}

CIUIStringW CIUIStringW::Left(int nCount) const
{
	if (nCount < 0)
	{
		nCount = 0;
	}
	if (nCount >= GetData()->nDataLength)
	{
		return *this;
	}

	CIUIStringW dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

// strspn equivalent
CIUIStringW CIUIStringW::SpanIncluding(LPCWSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	return Left(wcsspn(m_pchData, lpszCharSet));
}

// strcspn equivalent
CIUIStringW CIUIStringW::SpanExcluding(LPCWSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	return Left(wcscspn(m_pchData, lpszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding

int CIUIStringW::ReverseFind(WCHAR ch) const
{
	// find last single character
	LPWSTR lpsz = wcsrchr(m_pchData, (_TUCHAR)ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

// find a sub-string (like strstr)
int CIUIStringW::Find(LPCWSTR lpszSub) const
{
	return Find(lpszSub, 0);
}

int CIUIStringW::Find(LPCWSTR lpszSub, int nStart) const
{
	_ASSERT(lpszSub != NULL);

	int nLength = GetData()->nDataLength;
	if (nStart > nLength)
	{
		return -1;
	}

	// find first matching substring
	LPWSTR lpsz = wcsstr(m_pchData + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}


/////////////////////////////////////////////////////////////////////////////
// CIUIStringW formatting

#define TCHAR_ARG   WCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

void CIUIStringW::FormatV(LPCWSTR lpszFormat, va_list argList)
{
	_ASSERT(lpszFormat != NULL);

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCWSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = IUICharNextW(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = IUICharNextW(lpsz)) == '%')
		{
			nMaxLen += wcslen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = IUICharNextW(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
			{
				nMaxLen += 2;    // for '0x'
			}
			else if (*lpsz == '*')
			{
				nWidth = va_arg(argList, int);
			}
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' || *lpsz == ' ')
			{
				;
			}
			else // hit non-flag character
			{
				break;
			}
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _wtoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = IUICharNextW(lpsz))
			{
				;
			}
		}
		_ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = IUICharNextW(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = IUICharNextW(lpsz);
			}
			else
			{
				nPrecision = _wtoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = IUICharNextW(lpsz))
					;
			}
			_ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (wcsncmp(lpsz, L"I64", 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			_ASSERT(FALSE);
#endif
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz = IUICharNextW(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = IUICharNextW(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = IUICharNextW(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, TCHAR_ARG);
			break;
		case 'c' | FORCE_ANSI:
		case 'C' | FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;
		case 'c' | FORCE_UNICODE:
		case 'C' | FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, WCHAR_ARG);
			break;

		// strings
		case 's':
		{
			LPCWSTR pstrNextArg = va_arg(argList, LPCWSTR);
			if (pstrNextArg == NULL)
			{
				nItemLen = 6;    // "(null)"
			}
			else
			{
				nItemLen = lstrlenW(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;

		case 'S':
		{
#ifndef _UNICODE
			LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
			if (pstrNextArg == NULL)
			{
				nItemLen = 6;    // "(null)"
			}
			else
			{
				nItemLen = wcslen(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
#else
			LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
			if (pstrNextArg == NULL)
			{
				nItemLen = 6;    // "(null)"
			}
			else
			{
				nItemLen = lstrlenA(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
#endif
		}
		break;

		case 's' | FORCE_ANSI:
		case 'S' | FORCE_ANSI:
		{
			LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
			if (pstrNextArg == NULL)
			{
				nItemLen = 6;    // "(null)"
			}
			else
			{
				nItemLen = lstrlenA(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;

		case 's' | FORCE_UNICODE:
		case 'S' | FORCE_UNICODE:
		{
			LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
			if (pstrNextArg == NULL)
			{
				nItemLen = 6;    // "(null)"
			}
			else
			{
				nItemLen = wcslen(pstrNextArg);
				nItemLen = max(1, nItemLen);
			}
		}
		break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
			{
				nItemLen = min(nItemLen, nPrecision);
			}
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
				{
					va_arg(argList, __int64);
				}
				else
				{
					va_arg(argList, int);
				}
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, DOUBLE_ARG);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

			case 'f':
			{
				double f;
				LPWSTR pszTemp;

				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				// 6 == adjustment in case precision is not specified,
				//   which means that the precision defaults to 6
				pszTemp = (LPWSTR)_alloca(max(nWidth, 312 + nPrecision + 6));

				f = va_arg(argList, double);
#if(_MSC_VER <= 1500) // vs2008
				swprintf(pszTemp, L"%*.*f", nWidth, nPrecision + 6, f);
#else
				swprintf_s(pszTemp, max(nWidth, 312 + nPrecision + 6), L"%*.*f", nWidth, nPrecision + 6, f);
#endif
				nItemLen = wcslen(pszTemp);
			}
			break;

			case 'p':
				va_arg(argList, void *);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth + nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int *);
				break;

			default:
				_ASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	GetBuffer(nMaxLen);
#if(_MSC_VER <= 1500) // vs2008
	VERIFY(vswprintf(m_pchData, lpszFormat, argListSave) <= GetAllocLength());
#else
	VERIFY(vswprintf_s(m_pchData, nMaxLen + 1, lpszFormat, argListSave) <= GetAllocLength());
#endif
	ReleaseBuffer();

	va_end(argListSave);
}

// formatting (using wsprintf style formatting)
void CIUIStringW::Format(LPCWSTR lpszFormat, ...)
{
	_ASSERT(lpszFormat != NULL);

	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}

void CIUIStringW::Format(UINT nFormatID, ...)
{
	CIUIStringW strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	va_list argList;
	va_start(argList, nFormatID);
	FormatV(strFormat, argList);
	va_end(argList);
}

// formatting (using FormatMessage style formatting)
void CIUIStringW::FormatMessage(LPCWSTR lpszFormat, ...)
{
	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, lpszFormat);
	LPWSTR lpszTemp;

	if (::FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			lpszFormat, 0, 0, (LPWSTR)&lpszTemp, 0, &argList) == 0 ||
		lpszTemp == NULL)
	{
		return;
		//AfxThrowMemoryException();
	}

	// assign lpszTemp into the resulting string and free the temporary
	*this = lpszTemp;
	LocalFree(lpszTemp);
	va_end(argList);
}

void CIUIStringW::FormatMessage(UINT nFormatID, ...)
{
	// get format string from string table
	CIUIStringW strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, nFormatID);
	LPWSTR lpszTemp;
	if (::FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			strFormat, 0, 0, (LPWSTR)&lpszTemp, 0, &argList) == 0 ||
		lpszTemp == NULL)
	{
		return;
		//AfxThrowMemoryException();
	}

	// assign lpszTemp into the resulting string and free lpszTemp
	*this = lpszTemp;
	LocalFree(lpszTemp);
	va_end(argList);
}

void CIUIStringW::TrimRight(LPCWSTR lpszTargetList)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPWSTR lpsz = m_pchData;
	LPWSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (wcschr(lpszTargetList, *lpsz) != NULL)
		{
			if (lpszLast == NULL)
			{
				lpszLast = lpsz;
			}
		}
		else
		{
			lpszLast = NULL;
		}
		lpsz = IUICharNextW(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringW::TrimRight(WCHAR chTarget)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPWSTR lpsz = m_pchData;
	LPWSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (*lpsz == chTarget)
		{
			if (lpszLast == NULL)
			{
				lpszLast = lpsz;
			}
		}
		else
		{
			lpszLast = NULL;
		}
		lpsz = IUICharNextW(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringW::TrimRight()
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPWSTR lpsz = m_pchData;
	LPWSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
			{
				lpszLast = lpsz;
			}
		}
		else
		{
			lpszLast = NULL;
		}
		lpsz = IUICharNextW(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringW::TrimLeft(LPCWSTR lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (SafeStrlen(lpszTargets) == 0)
	{
		return;
	}

	CopyBeforeWrite();
	LPCWSTR lpsz = m_pchData;

	while (*lpsz != '\0')
	{
		if (wcschr(lpszTargets, *lpsz) == NULL)
		{
			break;
		}
		lpsz = IUICharNextW(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(WCHAR));
		GetData()->nDataLength = nDataLength;
	}
}

void CIUIStringW::TrimLeft(WCHAR chTarget)
{
	// find first non-matching character

	CopyBeforeWrite();
	LPCWSTR lpsz = m_pchData;

	while (chTarget == *lpsz)
	{
		lpsz = IUICharNextW(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(WCHAR));
		GetData()->nDataLength = nDataLength;
	}
}

void CIUIStringW::TrimLeft()
{
	// find first non-space character

	CopyBeforeWrite();
	LPCWSTR lpsz = m_pchData;

	while (_istspace(*lpsz))
	{
		lpsz = IUICharNextW(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(WCHAR));
		GetData()->nDataLength = nDataLength;
	}
}

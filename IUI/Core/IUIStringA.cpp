#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


// iuiChNil is left for backward compatibility
static CHAR IUI::iuiChNilA = '\0';

// For an empty string, m_pchData will point here
// (note: avoids special case of checking for NULL m_pchData)
// empty string data (and locked)
int _afxInitDataA[] = { -1, 0, 0, 0 };
CIUIStringDataA *_iuiDataNilA = (CIUIStringDataA *) &_afxInitDataA;
LPCSTR IUI::_iuiPchNilA = (LPCSTR)(((BYTE *)&_afxInitDataA) + sizeof(CIUIStringDataA));

///////////////////////////////////////////////////////////////////////////////
// CIUIStringA conversion helpers (these use the current system locale)

#if (_MSC_VER <= 1200)	// VC6.0
int _wcstombsz(char *mbstr, const wchar_t *wcstr, size_t count)
{
	if (count == 0 && mbstr != NULL)
	{
		return 0;
	}

	int result = ::WideCharToMultiByte(CP_ACP, 0, wcstr, -1,
			mbstr, count, NULL, NULL);
	_ASSERT(mbstr == NULL || result <= (int)count);
	if (result > 0)
	{
		mbstr[result - 1] = 0;
	}
	return result;
}
#endif //(_MSC_VER == 1200)	// VC6.0

static char *__cdecl IUICharNextA(const char *p) throw()
{
	return reinterpret_cast<char *>(_mbsinc(reinterpret_cast<const unsigned char *>(p)));
}

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

#ifdef _AFXDLL
CIUIStringA::CIUIStringA()
{
	Init();
}
#else
CIUIStringA::CIUIStringA()
{
	m_pchData = iuiEmptyStringA.m_pchData;
	m_pchDataConvert = NULL;
}
#endif // _AFXDLL

CIUIStringA::CIUIStringA(const CIUIStringA &stringSrc)
{
	_ASSERT(stringSrc.GetData()->nRefs != 0);
	if (stringSrc.GetData()->nRefs >= 0)
	{
		_ASSERT(stringSrc.GetData() != _iuiDataNilA);
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

CIUIStringA::CIUIStringA(CHAR ch, int nLength/* = 1*/)
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

CIUIStringA::CIUIStringA(LPCSTR lpsz)
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
			memcpy(m_pchData, lpsz, nLen * sizeof(CHAR));
		}
	}

	m_pchDataConvert = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion constructors

CIUIStringA::CIUIStringA(LPCWSTR lpsz)
{
	Init();
	int nSrcLen = lpsz != NULL ? wcslen(lpsz) : 0;
	if (nSrcLen != 0)
	{
		AllocBuffer(nSrcLen * 2);
		_wcstombsz(m_pchData, lpsz, (nSrcLen * 2) + 1);
		ReleaseBuffer();
	}

	m_pchDataConvert = NULL;
}

CIUIStringA::CIUIStringA(LPCSTR lpch, int nLength)
{
	Init();
	if (nLength != 0)
	{
		_ASSERT(IUIIsValidAddress(lpch, nLength, FALSE));
		AllocBuffer(nLength);
		memcpy(m_pchData, lpch, nLength * sizeof(CHAR));
	}

	m_pchDataConvert = NULL;
}

CIUIStringA::CIUIStringA(LPCWSTR lpsz, int nLength)
{
	Init();
	if (nLength != 0)
	{
		AllocBuffer(nLength * 2);
		int n = ::WideCharToMultiByte(CP_ACP, 0, lpsz, nLength, m_pchData, (nLength * 2) + 1, NULL, NULL);
		ReleaseBuffer(n >= 0 ? n : -1);
	}

	m_pchDataConvert = NULL;
}

CIUIStringA::CIUIStringA(const unsigned char *lpsz)
{
	Init();
	*this = (LPCSTR)lpsz;

	m_pchDataConvert = NULL;
}

void CIUIStringA::AllocBuffer(int nLen)
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
		int nL = sizeof(CIUIStringDataA) + (nLen + 1) * sizeof(CHAR);
		CIUIStringDataA *pData = (CIUIStringDataA *)new BYTE[nL];
		pData->nAllocLength = nLen;
		pData->nRefs = 1;
		pData->data()[nLen] = '\0';
		pData->nDataLength = nLen;
		m_pchData = pData->data();
	}
}

void CIUIStringA::FreeData(CIUIStringDataA *pData)
{
	delete[](BYTE *)pData;
}

void CIUIStringA::Release()
{
	if (GetData() != _iuiDataNilA)
	{
		_ASSERT(GetData()->nRefs != 0);
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
		{
			FreeData(GetData());
		}
		Init();
	}
}

void PASCAL CIUIStringA::Release(CIUIStringDataA *pData)
{
	if (pData != _iuiDataNilA)
	{
		_ASSERT(pData->nRefs != 0);
		if (InterlockedDecrement(&pData->nRefs) <= 0)
		{
			FreeData(pData);
		}
	}
}

void CIUIStringA::Empty()
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
		*this = &iuiChNilA;
	}
	_ASSERT(GetData()->nDataLength == 0);
	_ASSERT(GetData()->nRefs < 0 || GetData()->nAllocLength == 0);
}

void CIUIStringA::CopyBeforeWrite()
{
	if (GetData()->nRefs > 1)
	{
		CIUIStringDataA *pData = GetData();
		Release();
		AllocBuffer(pData->nDataLength);
		memcpy(m_pchData, pData->data(), (pData->nDataLength + 1) * sizeof(CHAR));
	}
	_ASSERT(GetData()->nRefs <= 1);
}

void CIUIStringA::AllocBeforeWrite(int nLen)
{
	if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
	{
		Release();
		AllocBuffer(nLen);
	}
	_ASSERT(GetData()->nRefs <= 1);
}

CIUIStringA::~CIUIStringA()
//  free any attached data
{
	if (GetData() != _iuiDataNilA)
	{
		if (InterlockedDecrement(&GetData()->nRefs) <= 0)
		{
			FreeData(GetData());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

void CIUIStringA::AllocCopy(CIUIStringA &dest, int nCopyLen, int nCopyIndex,
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
		memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen * sizeof(CHAR));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostic support

// #ifdef _DEBUG
// 	CDumpContext &operator<<(CDumpContext &dc, const CIUIStringA &string)
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
//  All routines return the new string (but as a 'const CIUIStringA&' so that
//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

void CIUIStringA::AssignCopy(int nSrcLen, LPCSTR lpszSrcData)
{
	AllocBeforeWrite(nSrcLen);
	memcpy(m_pchData, lpszSrcData, nSrcLen * sizeof(CHAR));
	GetData()->nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
}

const CIUIStringA &CIUIStringA::operator=(const CIUIStringA &stringSrc)
{
	if (m_pchData != stringSrc.m_pchData)
	{
		if ((GetData()->nRefs < 0 && GetData() != _iuiDataNilA) ||
			stringSrc.GetData()->nRefs < 0)
		{
			// actual copy necessary since one of the strings is locked
			AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
		}
		else
		{
			// can just copy references around
			Release();
			_ASSERT(stringSrc.GetData() != _iuiDataNilA);
			m_pchData = stringSrc.m_pchData;
			InterlockedIncrement(&GetData()->nRefs);
		}
	}
	return *this;
}

const CIUIStringA &CIUIStringA::operator=(LPCSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringA(lpsz));
	AssignCopy(SafeStrlen(lpsz), lpsz);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion assignment

const CIUIStringA &CIUIStringA::operator=(LPCWSTR lpsz)
{
	int nSrcLen = lpsz != NULL ? wcslen(lpsz) : 0;
	AllocBeforeWrite(nSrcLen * 2);
	_wcstombsz(m_pchData, lpsz, (nSrcLen * 2) + 1);
	ReleaseBuffer();
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//      There are three variants:
//          CIUIStringA + CIUIStringA
// and for ? = CHAR, LPCSTR
//          CIUIStringA + ?
//          ? + CIUIStringA

void CIUIStringA::ConcatCopy(int nSrc1Len, LPCSTR lpszSrc1Data,
	int nSrc2Len, LPCSTR lpszSrc2Data)
{
	// -- master concatenation routine
	// Concatenate two sources
	// -- assume that 'this' is a new CIUIStringA object

	int nNewLen = nSrc1Len + nSrc2Len;
	if (nNewLen != 0)
	{
		AllocBuffer(nNewLen);
		memcpy(m_pchData, lpszSrc1Data, nSrc1Len * sizeof(CHAR));
		memcpy(m_pchData + nSrc1Len, lpszSrc2Data, nSrc2Len * sizeof(CHAR));
	}
}

CIUIStringA IUI::operator+(const CIUIStringA &string1, const CIUIStringA &string2)
{
	CIUIStringA s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData,
		string2.GetData()->nDataLength, string2.m_pchData);
	return s;
}

CIUIStringA IUI::operator+(const CIUIStringA &string1, CHAR ch)
{
	CIUIStringA s;
	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData, 1, &ch);
	return s;
}

CIUIStringA IUI::operator+(CHAR ch, const CIUIStringA &string)
{
	CIUIStringA s;
	s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pchData);
	return s;
}

CIUIStringA IUI::operator+(const CIUIStringA &string, LPCSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringA(lpsz));
	CIUIStringA s;
	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData,
		CIUIStringA::SafeStrlen(lpsz), lpsz);
	return s;
}

CIUIStringA IUI::operator+(LPCSTR lpsz, const CIUIStringA &string)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringA(lpsz));
	CIUIStringA s;
	s.ConcatCopy(CIUIStringA::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength,
		string.m_pchData);
	return s;
}

#ifdef _UNICODE
// 	friend CIUIStringA IUIAPI operator+(const CIUIStringA &string, char ch);
// 	friend CIUIStringA IUIAPI operator+(char ch, const CIUIStringA &string);
#endif // _UNICODE

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void CIUIStringA::ConcatInPlace(int nSrcLen, LPCSTR lpszSrcData)
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
		CIUIStringDataA *pOldData = GetData();
		ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData);
		_ASSERT(pOldData != NULL);
		CIUIStringA::Release(pOldData);
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(m_pchData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(CHAR));
		GetData()->nDataLength += nSrcLen;
		_ASSERT(GetData()->nDataLength <= GetData()->nAllocLength);
		m_pchData[GetData()->nDataLength] = '\0';
	}
}

const CIUIStringA &CIUIStringA::operator+=(LPCSTR lpsz)
{
	_ASSERT(lpsz == NULL || IUIIsValidStringA(lpsz));
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const CIUIStringA &CIUIStringA::operator+=(CHAR ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const CIUIStringA &CIUIStringA::operator+=(const CIUIStringA &string)
{
	ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
	return *this;
}

#ifndef _AFX_NO_BSTR_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// OLE BSTR support

BSTR CIUIStringA::AllocSysString() const
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, m_pchData,
			GetData()->nDataLength, NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, nLen);
	if (bstr == NULL)
	{
		return NULL;
		// AfxThrowMemoryException();
	}
	MultiByteToWideChar(CP_ACP, 0, m_pchData, GetData()->nDataLength,
		bstr, nLen);

	return bstr;
}

BSTR CIUIStringA::SetSysString(BSTR *pbstr) const
{
	_ASSERT(IUIIsValidAddress(pbstr, sizeof(BSTR)));

	int nLen = MultiByteToWideChar(CP_ACP, 0, m_pchData,
			GetData()->nDataLength, NULL, NULL);
	if (!::SysReAllocStringLen(pbstr, NULL, nLen))
	{
		return NULL;
		// AfxThrowMemoryException();
	}
	MultiByteToWideChar(CP_ACP, 0, m_pchData, GetData()->nDataLength,
		*pbstr, nLen);

	_ASSERT(*pbstr != NULL);
	return *pbstr;
}

#endif // _AFX_NO_BSTR_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

LPSTR CIUIStringA::GetBuffer(int nMinBufLength)
{
	_ASSERT(nMinBufLength >= 0);

	if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
	{
#ifdef _DEBUG
		// give a warning in case locked string becomes unlocked
		if (GetData() != _iuiDataNilA && GetData()->nRefs < 0)
		{
			IUITRACEA("Warning: GetBuffer on locked CIUIStringA creates unlocked CIUIStringA!\n");
		}
#endif
		// we have to grow the buffer
		CIUIStringDataA *pOldData = GetData();
		int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
		if (nMinBufLength < nOldLen)
		{
			nMinBufLength = nOldLen;
		}
		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, pOldData->data(), (nOldLen + 1) * sizeof(CHAR));
		GetData()->nDataLength = nOldLen;
		CIUIStringA::Release(pOldData);
	}
	_ASSERT(GetData()->nRefs <= 1);

	// return a pointer to the character storage for this string
	_ASSERT(m_pchData != NULL);
	return m_pchData;
}

void CIUIStringA::ReleaseBuffer(int nNewLength)
{
	CopyBeforeWrite();  // just in case GetBuffer was not called

	if (nNewLength == -1)
	{
		nNewLength = lstrlenA(m_pchData);    // zero terminated
	}

	_ASSERT(nNewLength <= GetData()->nAllocLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
}

LPSTR CIUIStringA::GetBufferSetLength(int nNewLength)
{
	_ASSERT(nNewLength >= 0);

	GetBuffer(nNewLength);
	GetData()->nDataLength = nNewLength;
	m_pchData[nNewLength] = '\0';
	return m_pchData;
}

void CIUIStringA::FreeExtra()
{
	_ASSERT(GetData()->nDataLength <= GetData()->nAllocLength);
	if (GetData()->nDataLength != GetData()->nAllocLength)
	{
		CIUIStringDataA *pOldData = GetData();
		AllocBuffer(GetData()->nDataLength);
		memcpy(m_pchData, pOldData->data(), pOldData->nDataLength * sizeof(CHAR));
		_ASSERT(m_pchData[GetData()->nDataLength] == '\0');
		CIUIStringA::Release(pOldData);
	}
	_ASSERT(GetData() != NULL);
}

LPSTR CIUIStringA::LockBuffer()
{
	LPSTR lpsz = GetBuffer(0);
	GetData()->nRefs = -1;
	return lpsz;
}

void CIUIStringA::UnlockBuffer()
{
	_ASSERT(GetData()->nRefs == -1);
	if (GetData() != _iuiDataNilA)
	{
		GetData()->nRefs = 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Commonly used routines (rarely used routines in STREX.CPP)

int CIUIStringA::Find(CHAR ch) const
{
	return Find(ch, 0);
}

int CIUIStringA::Find(CHAR ch, int nStart) const
{
	int nLength = GetData()->nDataLength;
	if (nStart >= nLength)
	{
		return -1;
	}

	// find first single character
	LPSTR lpsz = strchr(m_pchData + nStart, (_TUCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

int CIUIStringA::FindOneOf(LPCSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	LPSTR lpsz = strpbrk(m_pchData, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

void CIUIStringA::MakeUpper()
{
	CopyBeforeWrite();
	_strupr(m_pchData);
}

void CIUIStringA::MakeLower()
{
	CopyBeforeWrite();
	_strlwr(m_pchData);
}

void CIUIStringA::MakeReverse()
{
	CopyBeforeWrite();
	_strrev(m_pchData);
}

void CIUIStringA::SetAt(int nIndex, CHAR ch)
{
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);

	CopyBeforeWrite();
	m_pchData[nIndex] = ch;
}

#ifndef _UNICODE
void CIUIStringA::AnsiToOem()
{
	CopyBeforeWrite();
	::AnsiToOem(m_pchData, m_pchData);
}
void CIUIStringA::OemToAnsi()
{
	CopyBeforeWrite();
	::OemToAnsi(m_pchData, m_pchData);
}
#endif

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

// CIUIStringA
CIUIStringDataA *CIUIStringA::GetData() const
{
	_ASSERT(m_pchData != NULL);
	return ((CIUIStringDataA *)m_pchData) - 1;
}
void CIUIStringA::Init()
{
	m_pchData = iuiEmptyStringA.m_pchData;
}
const CIUIStringA &CIUIStringA::operator=(const unsigned char *lpsz)
{
	*this = (LPCSTR)lpsz;
	return *this;
}

int CIUIStringA::GetLength() const
{
	return GetData()->nDataLength;
}
int CIUIStringA::GetAllocLength() const
{
	return GetData()->nAllocLength;
}
BOOL CIUIStringA::IsEmpty() const
{
	return GetData()->nDataLength == 0;
}
CIUIStringA::operator LPCSTR() const
{
	return m_pchData;
}
int PASCAL CIUIStringA::SafeStrlen(LPCSTR lpsz)
{
	return (lpsz == NULL) ? 0 : lstrlenA(lpsz);
}

// CIUIStringA support (windows specific)
int CIUIStringA::Compare(LPCSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // MBCS/Unicode aware
	return strcmp(m_pchData, lpsz);
}
int CIUIStringA::CompareNoCase(LPCSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // MBCS/Unicode aware
	return _stricmp(m_pchData, lpsz);
}
// CIUIStringA::Collate is often slower than Compare but is MBSC/Unicode
//  aware as well as locale-sensitive with respect to sort order.
int CIUIStringA::Collate(LPCSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // locale sensitive
	return strcoll(m_pchData, lpsz);
}
int CIUIStringA::CollateNoCase(LPCSTR lpsz) const
{
	_ASSERT(lpsz != NULL);    // locale sensitive
	return _stricoll(m_pchData, lpsz);
}

CHAR CIUIStringA::GetAt(int nIndex) const
{
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
CHAR CIUIStringA::operator[](int nIndex) const
{
	// same as GetAt
	_ASSERT(nIndex >= 0);
	_ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
bool IUI::operator==(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) == 0;
}
bool IUI::operator==(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) == 0;
}
bool IUI::operator==(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) == 0;
}
bool IUI::operator!=(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) != 0;
}
bool IUI::operator!=(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) != 0;
}
bool IUI::operator!=(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) != 0;
}
bool IUI::operator<(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) < 0;
}
bool IUI::operator<(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) < 0;
}
bool IUI::operator<(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) > 0;
}
bool IUI::operator>(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) > 0;
}
bool IUI::operator>(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) > 0;
}
bool IUI::operator>(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) < 0;
}
bool IUI::operator<=(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) <= 0;
}
bool IUI::operator<=(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) <= 0;
}
bool IUI::operator<=(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) >= 0;
}
bool IUI::operator>=(const CIUIStringA &s1, const CIUIStringA &s2)
{
	return s1.Compare(s2) >= 0;
}
bool IUI::operator>=(const CIUIStringA &s1, LPCSTR s2)
{
	return s1.Compare(s2) >= 0;
}
bool IUI::operator>=(LPCSTR s1, const CIUIStringA &s2)
{
	return s2.Compare(s1) <= 0;
}

/////////////////////////////////////////////////////////////////////////////
// Windows extensions to strings

#ifdef _UNICODE
#define CHAR_FUDGE 1    // one CHAR unused is good enough
#else
#define CHAR_FUDGE 2    // two BYTES unused for case of DBC last char
#endif

int IUILoadString(UINT nID, LPSTR lpszBuf, UINT nMaxBuf)
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

BOOL CIUIStringA::LoadString(UINT nID)
{
	// try fixed buffer first (to avoid wasting space in the heap)
	CHAR szTemp[256];
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

const CIUIStringA &CIUIStringA::operator=(CHAR ch)
{
	AssignCopy(1, &ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

int CIUIStringA::Delete(int nIndex, int nCount /* = 1 */)
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
			m_pchData + nIndex + nCount, nBytesToCopy * sizeof(CHAR));
		GetData()->nDataLength = nNewLength - nCount;
	}

	return nNewLength;
}

int CIUIStringA::Insert(int nIndex, CHAR ch)
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
		CIUIStringDataA *pOldData = GetData();
		LPSTR pstr = m_pchData;
		AllocBuffer(nNewLength);
		memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(CHAR));
		CIUIStringA::Release(pOldData);
	}

	// move existing bytes down
	memcpy(m_pchData + nIndex + 1,
		m_pchData + nIndex, (nNewLength - nIndex) * sizeof(CHAR));
	m_pchData[nIndex] = ch;
	GetData()->nDataLength = nNewLength;

	return nNewLength;
}

int CIUIStringA::Insert(int nIndex, LPCSTR pstr)
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
			CIUIStringDataA *pOldData = GetData();
			LPSTR pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(CHAR));
			CIUIStringA::Release(pOldData);
		}

		// move existing bytes down
		memcpy(m_pchData + nIndex + nInsertLength,
			m_pchData + nIndex,
			(nNewLength - nIndex - nInsertLength + 1) * sizeof(CHAR));
		memcpy(m_pchData + nIndex,
			pstr, nInsertLength * sizeof(CHAR));
		GetData()->nDataLength = nNewLength;
	}

	return nNewLength;
}

int CIUIStringA::Replace(CHAR chOld, CHAR chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		// otherwise modify each character that matches in the string
		CopyBeforeWrite();
		LPSTR psz = m_pchData;
		LPSTR pszEnd = psz + GetData()->nDataLength;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = IUICharNextA(psz);
		}
	}
	return nCount;
}

int CIUIStringA::Replace(LPCSTR lpszOld, LPCSTR lpszNew)
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
	LPSTR lpszStart = m_pchData;
	LPSTR lpszEnd = m_pchData + GetData()->nDataLength;
	LPSTR lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += lstrlenA(lpszStart) + 1;
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
			CIUIStringDataA *pOldData = GetData();
			LPSTR pstr = m_pchData;
			AllocBuffer(nNewLength);
			memcpy(m_pchData, pstr, pOldData->nDataLength * sizeof(CHAR));
			CIUIStringA::Release(pOldData);
		}
		// else, we just do it in-place
		lpszStart = m_pchData;
		lpszEnd = m_pchData + GetData()->nDataLength;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = strstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - m_pchData + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen,
					nBalance * sizeof(CHAR));
				memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(CHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += lstrlenA(lpszStart) + 1;
		}
		_ASSERT(m_pchData[nNewLength] == '\0');
		GetData()->nDataLength = nNewLength;
	}

	return nCount;
}

int CIUIStringA::Remove(CHAR chRemove)
{
	CopyBeforeWrite();

	LPSTR pstrSource = m_pchData;
	LPSTR pstrDest = m_pchData;
	LPSTR pstrEnd = m_pchData + GetData()->nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = IUICharNextA(pstrDest);
		}
		pstrSource = IUICharNextA(pstrSource);
	}
	*pstrDest = '\0';
	int nCount = pstrSource - pstrDest;
	GetData()->nDataLength -= nCount;

	return nCount;
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

CIUIStringA CIUIStringA::Mid(int nFirst) const
{
	return Mid(nFirst, GetData()->nDataLength - nFirst);
}

CIUIStringA CIUIStringA::Mid(int nFirst, int nCount) const
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

	CIUIStringA dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CIUIStringA CIUIStringA::Right(int nCount) const
{
	if (nCount < 0)
	{
		nCount = 0;
	}
	if (nCount >= GetData()->nDataLength)
	{
		return *this;
	}

	CIUIStringA dest;
	AllocCopy(dest, nCount, GetData()->nDataLength - nCount, 0);
	return dest;
}

CIUIStringA CIUIStringA::Left(int nCount) const
{
	if (nCount < 0)
	{
		nCount = 0;
	}
	if (nCount >= GetData()->nDataLength)
	{
		return *this;
	}

	CIUIStringA dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

// strspn equivalent
CIUIStringA CIUIStringA::SpanIncluding(LPCSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	return Left(strspn(m_pchData, lpszCharSet));
}

// strcspn equivalent
CIUIStringA CIUIStringA::SpanExcluding(LPCSTR lpszCharSet) const
{
	_ASSERT(lpszCharSet != NULL);
	return Left(strcspn(m_pchData, lpszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding

int CIUIStringA::ReverseFind(CHAR ch) const
{
	// find last single character
	LPSTR lpsz = strrchr(m_pchData, (_TUCHAR)ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

// find a sub-string (like strstr)
int CIUIStringA::Find(LPCSTR lpszSub) const
{
	return Find(lpszSub, 0);
}

int CIUIStringA::Find(LPCSTR lpszSub, int nStart) const
{
	_ASSERT(lpszSub != NULL);

	int nLength = GetData()->nDataLength;
	if (nStart > nLength)
	{
		return -1;
	}

	// find first matching substring
	LPSTR lpsz = strstr(m_pchData + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}


/////////////////////////////////////////////////////////////////////////////
// CIUIStringA formatting

#define TCHAR_ARG   CHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

void CIUIStringA::FormatV(LPCSTR lpszFormat, va_list argList)
{
	_ASSERT(lpszFormat != NULL);

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = IUICharNextA(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = IUICharNextA(lpsz)) == '%')
		{
			nMaxLen += strlen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = IUICharNextA(lpsz))
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
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = IUICharNextA(lpsz))
			{
				;
			}
		}
		_ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = IUICharNextA(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = IUICharNextA(lpsz);
			}
			else
			{
				nPrecision = atoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = IUICharNextA(lpsz))
					;
			}
			_ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(lpsz, "I64", 3) == 0)
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
				lpsz = IUICharNextA(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = IUICharNextA(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = IUICharNextA(lpsz);
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
				LPSTR pszTemp;

				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				// 6 == adjustment in case precision is not specified,
				//   which means that the precision defaults to 6
				pszTemp = (LPSTR)_alloca(max(nWidth, 312 + nPrecision + 6));

				f = va_arg(argList, double);
#if(_MSC_VER <= 1500) // vs2008
				sprintf(pszTemp, "%*.*f", nWidth, nPrecision + 6, f);
#else
				sprintf_s(pszTemp, max(nWidth, 312 + nPrecision + 6), "%*.*f", nWidth, nPrecision + 6, f);
#endif
				nItemLen = strlen(pszTemp);
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
	VERIFY(vsprintf(m_pchData, lpszFormat, argListSave) <= GetAllocLength());
#else
	VERIFY(vsprintf_s(m_pchData, nMaxLen + 1, lpszFormat, argListSave) <= GetAllocLength());
#endif
	ReleaseBuffer();

	va_end(argListSave);
}

// formatting (using wsprintf style formatting)
void CIUIStringA::Format(LPCSTR lpszFormat, ...)
{
	_ASSERT(lpszFormat != NULL);

	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}

void CIUIStringA::Format(UINT nFormatID, ...)
{
	CIUIStringA strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	va_list argList;
	va_start(argList, nFormatID);
	FormatV(strFormat, argList);
	va_end(argList);
}

// formatting (using FormatMessage style formatting)
void CIUIStringA::FormatMessage(LPCSTR lpszFormat, ...)
{
	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, lpszFormat);
	LPSTR lpszTemp;

	if (::FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			lpszFormat, 0, 0, (LPSTR)&lpszTemp, 0, &argList) == 0 ||
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

void CIUIStringA::FormatMessage(UINT nFormatID, ...)
{
	// get format string from string table
	CIUIStringA strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, nFormatID);
	LPSTR lpszTemp;
	if (::FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			strFormat, 0, 0, (LPSTR)&lpszTemp, 0, &argList) == 0 ||
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

void CIUIStringA::TrimRight(LPCSTR lpszTargetList)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPSTR lpsz = m_pchData;
	LPSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargetList, *lpsz) != NULL)
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
		lpsz = IUICharNextA(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringA::TrimRight(CHAR chTarget)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPSTR lpsz = m_pchData;
	LPSTR lpszLast = NULL;

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
		lpsz = IUICharNextA(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringA::TrimRight()
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)

	CopyBeforeWrite();
	LPSTR lpsz = m_pchData;
	LPSTR lpszLast = NULL;

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
		lpsz = IUICharNextA(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
		GetData()->nDataLength = lpszLast - m_pchData;
	}
}

void CIUIStringA::TrimLeft(LPCSTR lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (SafeStrlen(lpszTargets) == 0)
	{
		return;
	}

	CopyBeforeWrite();
	LPCSTR lpsz = m_pchData;

	while (*lpsz != '\0')
	{
		if (strchr(lpszTargets, *lpsz) == NULL)
		{
			break;
		}
		lpsz = IUICharNextA(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(CHAR));
		GetData()->nDataLength = nDataLength;
	}
}

void CIUIStringA::TrimLeft(CHAR chTarget)
{
	// find first non-matching character

	CopyBeforeWrite();
	LPCSTR lpsz = m_pchData;

	while (chTarget == *lpsz)
	{
		lpsz = IUICharNextA(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(CHAR));
		GetData()->nDataLength = nDataLength;
	}
}

void CIUIStringA::TrimLeft()
{
	// find first non-space character

	CopyBeforeWrite();
	LPCSTR lpsz = m_pchData;

	while (_istspace(*lpsz))
	{
		lpsz = IUICharNextA(lpsz);
	}

	if (lpsz != m_pchData)
	{
		// fix up data and length
		int nDataLength = GetData()->nDataLength - (lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(CHAR));
		GetData()->nDataLength = nDataLength;
	}
}

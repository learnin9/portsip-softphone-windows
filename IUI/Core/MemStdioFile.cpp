#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	CMemStdioFile::CMemStdioFile()
		: m_hResource(NULL)
		, m_nGrowBytes(1024)
		, m_nPosition(0)
		, m_nBufferSize(0)
		, m_nFileSize(0)
		, m_lpBuffer(NULL)
		, m_bAutoDelete(TRUE)
	{

	}

	CMemStdioFile::~CMemStdioFile()
	{
		if (m_lpBuffer)
		{
			Close();
		}

		_ASSERT(m_lpBuffer == NULL);

		m_nGrowBytes = 0;
		m_nPosition = 0;
		m_nBufferSize = 0;
		m_nFileSize = 0;
	}

	BOOL CMemStdioFile::OpenResource(HINSTANCE hRes, LPCTSTR lpszID, LPCTSTR lpszType)
	{
		HRSRC hRsrc = FindResource(hRes, lpszID, lpszType);

		if (NULL == hRsrc)
		{
			return FALSE;
		}

		DWORD dwSize = SizeofResource(hRes, hRsrc);

		if (0 == dwSize)
		{
			return FALSE;
		}

		HGLOBAL hGlobal = LoadResource(hRes, hRsrc);

		if (NULL == hGlobal)
		{
			return FALSE;
		}

		LPVOID pBuffer = LockResource(hGlobal);

		if (NULL == pBuffer)
		{
			return FALSE;
		}

		BYTE *pBuf = Alloc(dwSize + 2);
		memset(pBuf, 0, dwSize + 2);
		memcpy(pBuf, pBuffer, dwSize);
		Attach((BYTE *)pBuf, dwSize + 2, 100);

		return TRUE;
	}

	BOOL CMemStdioFile::CloseResource()
	{
		_ASSERT(m_lpBuffer != NULL && m_nBufferSize != 0 && !m_bAutoDelete);
		_ASSERT(m_nFileSize <= m_nBufferSize);

		m_nGrowBytes = 0;
		m_nPosition = 0;
		m_nBufferSize = 0;
		m_nFileSize = 0;

		if (m_lpBuffer && !m_bAutoDelete)
		{
			Free(m_lpBuffer);
		}

		m_lpBuffer = NULL;

		return TRUE;
	}

	void CMemStdioFile::Attach(BYTE *lpBuffer, UINT nBufferSize, UINT nGrowBytes)
	{
		if (lpBuffer == NULL && nBufferSize != 0)
		{
		}

		_ASSERT(m_lpBuffer == NULL);

		m_nGrowBytes = nGrowBytes;
		m_nPosition = 0;
		m_nBufferSize = nBufferSize;
		m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
		m_lpBuffer = lpBuffer;
		m_bAutoDelete = FALSE;
	}

	BYTE *CMemStdioFile::Detach()
	{
		BYTE *lpBuffer = m_lpBuffer;
		m_lpBuffer = NULL;
		m_nFileSize = 0;
		m_nBufferSize = 0;
		m_nPosition = 0;

		return lpBuffer;
	}

	BYTE *CMemStdioFile::Alloc(SIZE_T nBytes)
	{
		return (BYTE *)malloc(nBytes);
	}

	BYTE *CMemStdioFile::Realloc(BYTE *lpMem, SIZE_T nBytes)
	{
		_ASSERT(nBytes > 0);    // nBytes == 0 means free
		return (BYTE *)realloc(lpMem, nBytes);
	}

	BYTE *CMemStdioFile::Memcpy(BYTE *lpMemTarget, const BYTE *lpMemSource,    SIZE_T nBytes)
	{
		_ASSERT(lpMemTarget != NULL);
		_ASSERT(lpMemSource != NULL);

		IUICopyMemory(lpMemTarget, nBytes, lpMemSource, nBytes);
		return lpMemTarget;
	}

	void CMemStdioFile::Free(BYTE *lpMem)
	{
		_ASSERT(lpMem != NULL);

		free(lpMem);
	}

	void CMemStdioFile::GrowFile(SIZE_T dwNewLen)
	{
		if (dwNewLen > m_nBufferSize)
		{
			// grow the buffer
			SIZE_T dwNewBufferSize = m_nBufferSize;

			// watch out for buffers which cannot be grown!
			_ASSERT(m_nGrowBytes != 0);

			if (m_nGrowBytes == 0)
			{
				OutputDebugString(_T("CMemStdioFile::GrowFile Error"));
			}

			// determine new buffer size
			while (dwNewBufferSize < dwNewLen)
			{
				dwNewBufferSize += m_nGrowBytes;
			}

			// allocate new buffer
			BYTE *lpNew;

			if (m_lpBuffer == NULL)
			{
				lpNew = Alloc(dwNewBufferSize);
			}
			else
			{
				lpNew = Realloc(m_lpBuffer, dwNewBufferSize);
			}

			if (lpNew == NULL)
			{
				OutputDebugString(_T("CMemStdioFile::GrowFile MemoryException"));
				return;
			}

			m_lpBuffer = lpNew;
			m_nBufferSize = dwNewBufferSize;
		}
	}

	BOOL CMemStdioFile::IsUnicode()
	{
		if (m_lpBuffer[0] == 0xFF && m_lpBuffer[1] == 0xFE)    // Unicode
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL CMemStdioFile::ReadString(CIUIString *pString)
	{
		if (pString == NULL)
		{
			return FALSE;
		}

		// check unicode
		if (m_lpBuffer[0] == 0xFF && m_lpBuffer[1] == 0xFE)    // Unicode
		{
			std::wstring str;

			if (!ReadStringW(&str))
			{
				return FALSE;
			}

			USES_CONVERSION;
			*pString = W2CT(str.c_str());
		}
		else
		{
			std::string str;

			if (!ReadStringA(&str))
			{
				return FALSE;
			}

			*pString = (TCHAR *)_bstr_t(str.c_str());
		}

		return TRUE;
	}

	BOOL CMemStdioFile::ReadStringA(std::string *pString)
	{
		if (pString == NULL)
		{
			return FALSE;
		}

		ULONGLONG dwPos = GetPosition();

		if (dwPos == m_nBufferSize)
		{
			return FALSE;
		}

		BOOL bRet = AfxExtractSubStringA(pString,
				(LPCSTR)m_lpBuffer + dwPos, int(m_nBufferSize - dwPos), 0, '\r');

		if (bRet)
		{
			size_t nAdd = pString->length() + 1;

			if (dwPos + nAdd > m_nBufferSize)
			{
				Seek(0, SEEK_END);
			}
			else
			{
				Seek(dwPos + nAdd, SEEK_SET);
			}
		}
		else
		{
			Seek(0, SEEK_END);
		}

		return bRet;
	}

	BOOL CMemStdioFile::ReadStringW(std::wstring *pString)
	{
		if (pString == NULL)
		{
			return FALSE;
		}

		ULONGLONG dwPos = GetPosition();

		if (dwPos == m_nBufferSize)
		{
			return FALSE;
		}

		if (m_lpBuffer[0] == 0xFF && m_lpBuffer[1] == 0xFE)
		{
			dwPos += 2;
		}

		BOOL bRet = AfxExtractSubStringW(pString,
				(LPCWSTR)((LPCSTR)m_lpBuffer + dwPos), int(m_nBufferSize - dwPos), 0, '\r');

		if (bRet)
		{
			size_t nAdd = (pString->length() + 1) * (sizeof(WCHAR) / sizeof(CHAR));

			if (dwPos + nAdd > m_nBufferSize)
			{
				Seek(0, SEEK_END);
			}
			else
			{
				Seek(dwPos + nAdd, SEEK_SET);
			}
		}
		else
		{
			Seek(0, SEEK_END);
		}

		return bRet;
	}

	ULONGLONG CMemStdioFile::Seek(LONGLONG lOff, UINT nFrom)
	{
		_ASSERT(nFrom == SEEK_SET || nFrom == SEEK_END || nFrom == SEEK_CUR);

		LONGLONG lNewPos = m_nPosition;

		if (nFrom == SEEK_SET)
		{
			lNewPos = lOff;
		}
		else if (nFrom == SEEK_CUR)
		{
			lNewPos += lOff;
		}
		else if (nFrom == SEEK_END)
		{
			if (lOff > 0)
			{
				OutputDebugString(_T("CMemStdioFile::Seek CFileException::badSeek"));// offsets must be negative when seeking from the end
			}

			lNewPos = m_nFileSize + lOff;
		}
		else
		{
			return m_nPosition;
		}

		if (lNewPos < 0)
		{
			OutputDebugString(_T("CMemStdioFile::Seek CFileException::badSeek"));
		}

		if (static_cast<DWORD>(lNewPos) > m_nFileSize)
		{
			GrowFile((SIZE_T)lNewPos);
		}

		m_nPosition = (SIZE_T)lNewPos;

		return m_nPosition;
	}

	void CMemStdioFile::SetLength(ULONGLONG dwNewLen)
	{
#ifdef WIN32

		if (dwNewLen > ULONG_MAX)
		{
			OutputDebugString(_T("CMemStdioFile::SetLength MemoryException"));
		}

#endif  // WIN32

		if (dwNewLen > m_nBufferSize)
		{
			GrowFile((SIZE_T)dwNewLen);
		}

		if (dwNewLen < m_nPosition)
		{
			m_nPosition = (SIZE_T)dwNewLen;
		}

		m_nFileSize = (SIZE_T)dwNewLen;
	}

	ULONGLONG CMemStdioFile::GetPosition() const
	{
		return m_nPosition;
	}

	void CMemStdioFile::Close()
	{
		_ASSERT((m_lpBuffer == NULL && m_nBufferSize == 0) || !m_bAutoDelete);
		_ASSERT(m_nFileSize <= m_nBufferSize);

		m_nGrowBytes = 0;
		m_nPosition = 0;
		m_nBufferSize = 0;
		m_nFileSize = 0;

		if (m_lpBuffer && m_bAutoDelete)
		{
			Free(m_lpBuffer);
		}

		m_lpBuffer = NULL;
	}

} // namespace IUI

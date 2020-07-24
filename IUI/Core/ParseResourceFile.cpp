#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{


	CParseResourceFile::CParseResourceFile()
	{
	}


	CParseResourceFile::~CParseResourceFile()
	{
	}

	int LoadIDs(CMemStdioFile *pFile, HXRESOURCE hMapIDs)
	{
		if (pFile == NULL || hMapIDs == NULL)
		{
			return -1;
		}

		hMapIDs->clear();

		std::string strA;

		while (pFile->ReadStringA(&strA))
		{
			CIUIString str = (LPCTSTR)_bstr_t(strA.c_str());
			int nTest = str.Find(_T("#define"));

			if (nTest != -1)
			{
				// ¹ýÂËµô×Ö·û´®×ÖÒå
				int nPos = str.Find(_T("\""));

				if (nPos < 0)
				{
					str.TrimRight();
					int nRight = str.ReverseFind(' ');
					CIUIString strID;
					strID = str.Right(str.GetLength() - nRight - 1);

					UINT uID = _ttoi(strID);
					if (uID != 0)
					{
						str.TrimLeft();
						str.TrimRight(strID);
						str.TrimRight();
						str.TrimLeft(_T("#define"));
						str.TrimLeft();

						//
						(*hMapIDs)[(LPCSTR)_bstr_t(str.GetBuffer(0))] = uID;
					}
				}
			}
		}

		return 0;
	}

	int CParseResourceFile::SetResourceFile(LPCTSTR lpszFile)
	{
		if (lpszFile == NULL)
		{
			return -1;
		}

		if (!PathFileExists(lpszFile))
		{
			return -2;
		}

		m_varMember.m_strResourceFile = lpszFile;

		// Parse language file
		CMemStdioFile fileLanguage;

#if(_MSC_VER > 1200) // VC6.0
		FILE *fp = NULL;
		errno_t e = _wfopen_s(&fp, (LPCWSTR)(_bstr_t)lpszFile, L"rb");
#else
		FILE *fp = _wfopen((LPCWSTR)(_bstr_t)lpszFile, L"rb");
#endif

		if (
#if(_MSC_VER > 1200) // VC6.0
			e != 0 || 
#endif
			fp == NULL)
		{
			OutputDebugString(_T("Failed to open language file!"));
			return -1;
		}

		m_varMember.m_mapIDs.clear();

		fseek(fp, 0, SEEK_END);
		int nLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		BYTE *pBuf = new BYTE[nLen];
		memset(pBuf, 0, nLen);
		fread(pBuf, nLen, 1, fp);
		fclose(fp);

		if (nLen < 2)
		{
			delete [] pBuf;
			pBuf = NULL;
			return -2;
		}

		fileLanguage.Attach(pBuf, nLen, 0);

		LoadIDs(&fileLanguage, &m_varMember.m_mapIDs);

		pBuf = fileLanguage.Detach();
		delete [] pBuf;
		pBuf = NULL;

		return 0;
	}

	int CParseResourceFile::SetResourceFile(HINSTANCE hRes, LPCTSTR lpszResType, UINT uResID)
	{
		CMemStdioFile fileResource;
		fileResource.OpenResource(hRes, MAKEINTRESOURCE(uResID), lpszResType);

		LoadIDs(&fileResource, &m_varMember.m_mapIDs);
		fileResource.CloseResource();

		return 0;
	}

	LPCTSTR CParseResourceFile::GetResourceFile() const
	{
		return m_varMember.m_strResourceFile;
	}

	HXRESOURCE CParseResourceFile::GetResourceHandle()
	{
		return &m_varMember.m_mapIDs;
	}

	UINT CParseResourceFile::GetID(HXRESOURCE hResource, LPCTSTR lpszID)
	{
		if (hResource == NULL || lpszID == NULL)
		{
			return -1;
		}

		if (hResource->empty())
		{
			return -1;
		}

		std::map<std::string, UINT>::iterator it = hResource->find((LPCSTR)_bstr_t(lpszID));

		if (it == hResource->end())
		{
			return -1;
		}

		return it->second;
	}

} // namespace IUI

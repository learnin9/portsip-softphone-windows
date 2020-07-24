#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


namespace IUI
{


	BOOL AfxExtractSubString(CIUIString &rString, LPCTSTR lpszFullString, int iSubString, TCHAR chSep)
	{
		if (lpszFullString == NULL)
		{
			return FALSE;
		}

		while (iSubString--)
		{
			lpszFullString = _tcschr(lpszFullString, chSep);

			if (lpszFullString == NULL)
			{
				rString.Empty();        // return empty string as well
				return FALSE;
			}

			lpszFullString++;       // point past the separator
		}

		LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
		int nLen = (lpchEnd == NULL) ?
			lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
		_ASSERT(nLen >= 0);
		IUICopyMemory(rString.GetBufferSetLength(nLen), nLen * sizeof(TCHAR),
			lpszFullString, nLen * sizeof(TCHAR));
		rString.ReleaseBuffer();    // Need to call ReleaseBuffer
		// after calling GetBufferSetLength
		return TRUE;
	}


	CParseLanguageFile::CParseLanguageFile(CIUIProject *pProject)
		: m_pProject(pProject)
	{
	}


	CParseLanguageFile::~CParseLanguageFile()
	{
	}

	CIUIStringW LoadNameProperty(XML::IXMLDOMElementPtr pControlNode)
	{
		XML::IXMLDOMElementPtr pEleName = pControlNode->selectSingleNode("name");
		if (NULL == pEleName)
		{
			return _T("");
		}
		_variant_t var = pEleName->getAttribute("Value");

		if (var.vt != VT_NULL)
		{
			var.wReserved1 = 0;
			var.wReserved2 = 0;
			var.wReserved3 = 0;
		}

		CIUIString strRet = (LPCWSTR)_bstr_t(var);
		return strRet;
	}

	CIUIStringW LoadContentProperty(XML::IXMLDOMElementPtr pControlNode)
	{
		XML::IXMLDOMElementPtr pEleName = pControlNode->selectSingleNode("Content");
		if (NULL == pEleName)
		{
			return _T("");
		}
		_variant_t var = pEleName->getAttribute("Value");

		if (var.vt != VT_NULL)
		{
			var.wReserved1 = 0;
			var.wReserved2 = 0;
			var.wReserved3 = 0;
		}

		CIUIString strRet = (LPCWSTR)_bstr_t(var);
		return strRet;
	}

	// 递归加载控件及其后代控件
	int LoadControlContentProperty(
		XML::IXMLDOMElementPtr pControl,
		LPCWSTR lpszParentName,
		std::map<std::wstring, std::wstring> *pmapContents)
	{
		if (pControl == NULL || pmapContents == NULL)
		{
			return -1;
		}

		BSTR bstr;
		pControl->get_nodeName(&bstr);
		CIUIString strType = (LPCTSTR)_bstr_t(bstr);
		if (bstr != NULL)
		{
			SysFreeString(bstr);
		}

		CIUIStringW strName = LoadNameProperty(pControl);
		CIUIStringW strContent = LoadContentProperty(pControl);

		CIUIStringW strKey = lpszParentName;
		strKey += L"|";
		strKey += strName;

		(*pmapContents)[(LPCWSTR)strKey] = (LPCWSTR)strContent;

		//
		// Load children
		//
		XML::IXMLDOMNodePtr pControls = pControl->selectSingleNode(_T("Controls"));
		if (pControls != NULL)
		{
			XML::IXMLDOMNodePtr pChildCtrl = pControls->GetfirstChild();
			while (pChildCtrl != NULL)
			{
				_bstr_t strName = pChildCtrl->GetnodeName();

				LoadControlContentProperty(pChildCtrl, strKey, pmapContents);

				pChildCtrl = pChildCtrl->GetnextSibling();
			}
		}

		return 0;
	}

	int LoadWindowName(
		XML::IXMLDOMElementPtr pWindow,
		std::map<std::wstring, std::wstring> *pmapContents)
	{
		if (pWindow == NULL || pmapContents == NULL)
		{
			return -1;
		}

#ifdef _DEBUG
		BSTR bstr;
		pWindow->get_nodeName(&bstr);
		CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

		if (bstr != NULL)
		{
			SysFreeString(bstr);
		}

		_ASSERT(strValue == _T("Window"));
#endif

		// Load Window
		CIUIStringW strWindow = LoadNameProperty(pWindow);
		CIUIStringW strContent = LoadContentProperty(pWindow);
		(*pmapContents)[(LPCWSTR)strWindow] = (LPCWSTR)strContent;

		// Children
		XML::IXMLDOMElementPtr pRoot = pWindow->selectSingleNode(_T("Root"));
		if (pRoot == NULL)
		{
			return 0;
		}

		// 支持多Root
		XML::IXMLDOMNodePtr pRootNode = pRoot->GetfirstChild();
		while (pRootNode != NULL)
		{
			// 递归加载控件及其后代控件
			LoadControlContentProperty(pRootNode, strWindow, pmapContents);

			pRootNode = pRootNode->GetnextSibling();
		}

		return 0;
	}

	int CParseLanguageFile::SetLanguageFile(
		LPCTSTR lpszFile,
		std::map<std::wstring, std::wstring> *pmapContents)
	{
		if (NULL == lpszFile || NULL == pmapContents)
		{
			_ASSERT(FALSE);
			return -1;
		}

		if (!PathFileExists(lpszFile))
		{
			return -2;
		}

		pmapContents->clear();

		// Parse language file
		if (m_pProject == NULL)
		{
			_ASSERT(FALSE);
			return -1;
		}

		// init windows tree struct
		XML::IXMLDOMDocumentPtr xmlDoc(__uuidof(DOMDocument));
		xmlDoc->async = FALSE;
		HRESULT hr = E_FAIL;

		if (m_pProject->IsLoadSkinFromResource())
		{
			// 			LPVOID lpBuffer = NULL;
			// 			DWORD dwSize = 0;
			// 			int nRet = LoadCustomResourceA((LPCSTR)_bstr_t(m_strXmlID),
			// 					(LPCSTR)_bstr_t(m_pProject->GetUIResourceType()),
			// 					&lpBuffer, &dwSize);
			//
			// 			if (nRet != 0)
			// 			{
			// 				return E_NOIUIRC;
			// 			}
			//
			// 			// Transform UTF-8 to UTF-16
			// 			std::wstring wstrXml = MultiByteToUTF16((char *)lpBuffer + 3, dwSize - 3, CP_UTF8);   // Ignore BOM
			// 			hr = xmlDoc->loadXML(wstrXml.c_str());
		}
		else
		{
			hr = xmlDoc->load(lpszFile);
		}

		XML::IXMLDOMElementPtr pRootElement = xmlDoc->GetdocumentElement();

		if (pRootElement == NULL)
		{
			return E_NOROOTELEMENT;
		}

		CIUIString strRootType;
		BSTR bstr;
		pRootElement->get_nodeName(&bstr);
		strRootType = bstr;
		if (bstr != NULL)
		{
			SysFreeString(bstr);
		}

		if (strRootType != _T("IUI_LANGUAGE"))
		{
			return -10;
		}

		// 枚举 window
		XML::IXMLDOMNodePtr pWindowNode = pRootElement->GetfirstChild();
		while (NULL != pWindowNode)
		{
			_bstr_t strName = pWindowNode->GetnodeName();

			CIUIString strNode = (LPCTSTR)strName;

			if (strNode == IUI_WINDOW)
			{
				LoadWindowName(pWindowNode, pmapContents);
			}

			pWindowNode = pWindowNode->GetnextSibling();
		}

		return 0;
	}

	int CParseLanguageFile::SetLanguageFile(HINSTANCE hRes, LPCTSTR lpszResType, UINT uResID)
	{
		CMemStdioFile fileLanguage;
		fileLanguage.OpenResource(hRes, MAKEINTRESOURCE(uResID), lpszResType);

		// 		LoadCaption(&fileLanguage, &m_varMember.m_mapControlText);
		fileLanguage.CloseResource();

		return 0;
	}

	LPCWSTR CParseLanguageFile::LoadXMLString(
		HXSRESOURCE hMultiLanguageRes,
		LPCTSTR lpszSectionID,
		LPCTSTR lpszStringID)
	{
		if (hMultiLanguageRes == NULL || lpszSectionID == NULL)
		{
			return NULL;
		}

		USES_CONVERSION;
		std::wstring strKey = T2CW(lpszSectionID);
		strKey += L"/";
		strKey += T2CW(lpszStringID);

		std::map<std::wstring, std::wstring>::iterator it = hMultiLanguageRes->find(strKey);

		if (it == hMultiLanguageRes->end())
		{
			return NULL;
		}

		// 如果字符串里有“\\r\\n”，把它们翻译成字符串的回车
		CIUIStringW s = (LPCWSTR)it->second.c_str();
		s.Replace(L"\\r", L"\r");
		s.Replace(L"\\n", L"\n");
		it->second = (LPCWSTR)s;

		return it->second.c_str();
	}

} // namespace IUI

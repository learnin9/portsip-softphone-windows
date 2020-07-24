#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


CImageMgr::CImageMgr()
{

}

CImageMgr::~CImageMgr()
{
	// Release image
	if (!m_mapImages.empty())
	{
		_ASSERT(FALSE);

		IUITRACE(_T("[IUI]===Begin track unrelease images\r\n"));
		std::map<CIUIString, HIUIIMAGE>::iterator it = m_mapImages.begin();
		for (; it != m_mapImages.end(); ++it)
		{
			IUITRACE(_T("\t%s\r\n"), it->second->GetSafeImageName());

			_ASSERT(it->second != NULL);

			int nRef = it->second->Destroy();

			if (nRef > 0)
			{
				IUITRACE(_T("[IUI]===Image '%s' need call ReleaseIUIImage to release\n"),
					it->second->GetSafeImageName());
			}

			it->second->Destroy();
			delete it->second;
		}
		IUITRACE(_T("[IUI]===End track unrelease images\r\n"));
		m_mapImages.clear();
	}
}

///////////////////////////////////////////////////////////////////////

// 内部使用
HIUIIMAGE ResourceMgr::CreateEmptyIUIImage(IN CIUIProject *pOwner, LPCTSTR lpszImageFile)
{
	if (pOwner == NULL || lpszImageFile == NULL || CIUIString(lpszImageFile).IsEmpty())
	{
		return NULL;
	}

#ifdef _DEBUG
	if (CIUIString(lpszImageFile).Find(_T("1.gif")) >= 0)
	{
		int n = 0;
	}
#endif
	std::map<CIUIString, HIUIIMAGE>::iterator it = pOwner->GetImageMgr()->m_mapImages.find(
			lpszImageFile);

	// Not found the image, create the image, and add to map.
	if (it == pOwner->GetImageMgr()->m_mapImages.end())
	{
		IUIImage *pImage = new IUIImage;

		if (pImage != NULL)
		{
			pImage->SetProject(pOwner);
			pImage->m_strImageName = lpszImageFile;
			pImage->AddRef();

			pOwner->GetImageMgr()->m_mapImages[CIUIString(lpszImageFile)] = pImage;

			return pImage;
		}
	}
	else
	{
		it->second->AddRef();
		return it->second;
	}

	return NULL;
}

// 外部使用
int IUI::ImageManager::IUISetControlImage(
	IN CIUIProject *pOwner,
	__inout HIUIIMAGE *phIUIImage,
	LPCTSTR lpszImage)
{
	if (pOwner == NULL || phIUIImage == NULL)
	{
		return -1;
	}

	CIUIString strNewImage = lpszImage;

	if (*phIUIImage != NULL)
	{
		// TODO: 本来不论新旧Image是否一样，都应该释放旧的Image, 因这虽然图片名一样，但内容可能变了。
		// 但由于在界面编辑器中，修改任意一个属性，都会导致重新调用控件的BindStyle
		// 进而调用本函数重新加载图片，为了性能考虑，只释放不同文件名的图片。
		if (strNewImage.Compare((LPCTSTR)(*phIUIImage)->GetSafeImageName()) != 0)
		{
			// 如果新旧Image不一样，释放旧Image
			ReleaseIUIImage(*phIUIImage);
		}
		else
		{
			// 如果一样，返回
			return 0;
		}
	}

	// 输入ID有效
	if (!strNewImage.IsEmpty())
	{
		*phIUIImage = CreateEmptyIUIImage(pOwner, lpszImage);
	}
	else
	{
		*phIUIImage = NULL;
	}

	return 0;
}

int IUI::ImageManager::AddCustomImage(IN CIUIProject *pOwner,
	LPCTSTR lpszImageFile, HBITMAP hImage)
{
	if (pOwner == NULL
		|| lpszImageFile == NULL
		|| hImage == NULL
		|| CIUIString(lpszImageFile).IsEmpty())
	{
		return -1;
	}

	// Find the file exist
	std::map<CIUIString, HIUIIMAGE>::iterator it = pOwner->GetImageMgr()->m_mapImages.find(
			lpszImageFile);

	// Not found the Image, create the Image, and add to map.
	if (it != pOwner->GetImageMgr()->m_mapImages.end())
	{
		return -2; // Already exist
	}

	HIUIIMAGE hIUIImage = new IUIImage();
	hIUIImage->SetProject(pOwner);
	hIUIImage->Attach(hImage);
	hIUIImage->m_strImageName = lpszImageFile;
	hIUIImage->AddRef();

	pOwner->GetImageMgr()->m_mapImages[CIUIString(lpszImageFile)] = hIUIImage;

	return 0;
}

int IUI::ImageManager::AddCustomImage(IN CIUIProject *pOwner,
	LPCTSTR lpszImageFile, HIUIIMAGE hImage)
{
	if (pOwner == NULL
		|| lpszImageFile == NULL
		|| hImage->GetSafeHBITMAP() == NULL
		|| CIUIString(lpszImageFile).IsEmpty())
	{
		return -1;
	}

	// Find the file exist
	std::map<CIUIString, HIUIIMAGE>::iterator it = pOwner->GetImageMgr()->m_mapImages.find(
			lpszImageFile);

	// Not found the Image, create the Image, and add to map.
	if (it != pOwner->GetImageMgr()->m_mapImages.end())
	{
		return -2; // Already exist
	}

	hImage->m_strImageName = lpszImageFile;
	hImage->AddRef();

	pOwner->GetImageMgr()->m_mapImages[CIUIString(lpszImageFile)] = hImage;

	return 0;
}

int IUI::ImageManager::AddCustomImage(IN CIUIProject *pOwner,
	HBITMAP hImage, __out CIUIString *pstrImageName)
{
	if (pOwner == NULL || hImage == NULL || pstrImageName == NULL)
	{
		return -1;
	}

	GUID guid;
	CoCreateGuid(&guid);
	wchar_t wszName[64] = {0};
	StringFromGUID2(guid, wszName, sizeof(wszName));

	*pstrImageName = (LPCTSTR)_bstr_t(wszName);

	return AddCustomImage(pOwner, *pstrImageName, hImage);
}

int IUI::ImageManager::AddCustomImage(IN CIUIProject *pOwner, HIUIIMAGE hImage)
{
	if (pOwner == NULL || hImage == NULL)
	{
		return -1;
	}

	GUID guid;
	CoCreateGuid(&guid);
	wchar_t wszName[64] = {0};
	StringFromGUID2(guid, wszName, sizeof(wszName));

	return AddCustomImage(pOwner, (LPCTSTR)_bstr_t(wszName), hImage);
}

HIUIIMAGE IUI::ImageManager::CreateIUIImage(IN CIUIProject *pOwner, LPCTSTR lpszImageFile)
{
	HIUIIMAGE hRet = CreateEmptyIUIImage(pOwner, lpszImageFile);
	if (hRet == NULL)
	{
		return NULL;
	}

	if (hRet->GetSafeHBITMAP() == NULL)
	{
		BOOL bIsAbsolutePath = FALSE;
		int nLen = (int)_tcslen(lpszImageFile);
		int i = 0;
		for (i = 0; i < nLen; ++i)
		{
			if (lpszImageFile[i] == ':')
			{
				bIsAbsolutePath = TRUE;
				break;
			}
		}

		if (bIsAbsolutePath)
		{
			hRet->Load(lpszImageFile);
		}
		else
		{
			hRet->AutoLoad(lpszImageFile);
		}
	}

	return hRet;
}

HIUIIMAGE IUI::ImageManager::GetIUIImage(IN CIUIProject *pOwner, LPCTSTR lpszImageFile)
{
	if (pOwner == NULL || CIUIString(lpszImageFile).IsEmpty())
	{
		return NULL;
	}

	std::map<CIUIString, HIUIIMAGE>::iterator it = pOwner->GetImageMgr()->m_mapImages.find(
			lpszImageFile);

	// Not found the image
	if (it == pOwner->GetImageMgr()->m_mapImages.end())
	{
		return NULL;
	}

	return it->second;
}

int IUI::ImageManager::ReleaseIUIImage(HIUIIMAGE hIUIImage)
{
	if (hIUIImage == NULL)
	{
		return -1;
	}

	CIUIString strFileName = hIUIImage->GetSafeImageName();

	return ReleaseIUIImage(hIUIImage->GetOwner(), strFileName);
}

int IUI::ImageManager::ReleaseIUIImage(IN CIUIProject *pOwner, LPCTSTR lpszImageFile)
{
	if (pOwner == NULL
		|| lpszImageFile == NULL
		|| CIUIString(lpszImageFile).IsEmpty()
		|| pOwner->GetImageMgr()->m_mapImages.empty())
	{
		return -1;
	}

	std::map<CIUIString, HIUIIMAGE>::iterator it = pOwner->GetImageMgr()->m_mapImages.find(
			lpszImageFile);

	// Not found the Image
	if (it == pOwner->GetImageMgr()->m_mapImages.end())
	{
		return -2;
	}

	_ASSERT(it->second != NULL);

	int nAddRef = it->second->Release();
	if (nAddRef == 0)
	{
		delete it->second;
		pOwner->GetImageMgr()->m_mapImages.erase(it);
	}

	return nAddRef;
}

//////////////////////////////////////////////////////////////
// font

CFontMgr::CFontMgr()
{

}

CFontMgr::~CFontMgr()
{
	// 释放默认字体
	CIUIString strDefaultFontID = GetDefaultFontID();
	std::map<CIUIString, HIUIFONT>::iterator itDef = m_mapFonts.find(strDefaultFontID);
	if (itDef != m_mapFonts.end())
	{
		int nRef = itDef->second->Release();

		if (nRef > 0)
		{
			IUITRACE(_T("[LibUIDK]===Font '%s' need call ReleaseIUIFont to release\n"),
				itDef->second->m_strFontResID);
		}

		DeleteObject((HFONT)(*itDef->second));
		delete itDef->second;
		m_mapFonts.erase(itDef);
	}

	// Release font
	if (!m_mapFonts.empty())
	{
		_ASSERT(FALSE);

		IUITRACE(_T("[IUI]===Begin track unrelease font\r\n"));
		std::map<CIUIString, HIUIFONT>::iterator it = m_mapFonts.begin();
		while (it != m_mapFonts.end())
		{
			_ASSERT(it->second != NULL);

			int nRef = it->second->Release();

			if (nRef > 0)
			{
				IUITRACE(_T("\t[IUI]===Font '%s' need call ReleaseIUIFont to release\n"),
					it->second->m_strFontResID);
			}

			DeleteObject((HFONT)(*it->second));
			delete it->second;
			m_mapFonts.erase(it);

			it = m_mapFonts.begin();
		}
		IUITRACE(_T("[IUI]===End track unrelease font\r\n"));
	}
}

HIUIFONT CFontMgr::GetDefaultFont() const
{
	std::map<CIUIString, HIUIFONT>::const_iterator it = m_mapFonts.find(GetDefaultFontID());

	if (it == m_mapFonts.end())
	{
		return NULL;
	}

	return it->second;
}

// 内部使用
HIUIFONT ResourceMgr::CreateEmptyIUIFontInternal(IN CIUIProject *pOwner, LPCTSTR lpszFontResID)
{
	if (lpszFontResID == NULL || CIUIString(lpszFontResID).IsEmpty())
	{
		return NULL;
	}

	std::map<CIUIString, HIUIFONT>::iterator it = pOwner->GetFontMgr()->m_mapFonts.find(
			lpszFontResID);

	// Not found the font, create the font, and add to map.
	if (it == pOwner->GetFontMgr()->m_mapFonts.end())
	{
		CIUIFont *pFont = new CIUIFont;

		if (pFont != NULL)
		{
			pFont->m_strFontResID = lpszFontResID;
			pFont->SetOwner(pOwner);
			pFont->AddRef();

			pOwner->GetFontMgr()->m_mapFonts[lpszFontResID] = pFont;

			return pFont;
		}
	}
	else
	{
		it->second->AddRef();
		return it->second;
	}

	return NULL;
}

HIUIFONT ResourceMgr::CreateIUIFontInternal(IN CIUIProject *pOwner, LPCTSTR lpszFontResID)
{
	HIUIFONT hRet = CreateEmptyIUIFontInternal(pOwner, lpszFontResID);
	if (hRet == NULL)
	{
		return NULL;
	}

	if (*hRet == NULL)
	{
		hRet->Load(lpszFontResID);
	}

	return hRet;
}

int ResourceMgr::ReleaseIUIFontInternal(HIUIFONT hIUIFont)
{
	if (hIUIFont == NULL)
	{
		return -1;
	}

	CIUIString strFontResID;
	GetFontResID(hIUIFont, &strFontResID);

	return ReleaseIUIFont(hIUIFont->GetOwner(), strFontResID);
}

HFONT ResourceMgr::GetHFont(HIUIFONT hIUIFont)
{
	if (hIUIFont == NULL)
	{
		return NULL;
	}

	hIUIFont->SafeLoadSavedFont();

	return HFONT(*hIUIFont);
}

int ResourceMgr::GetFontResID(HIUIFONT hIUIFont, CIUIString *pstrFontResID)
{
	if (hIUIFont == NULL || pstrFontResID == NULL)
	{
		return -1;
	}

	*pstrFontResID = hIUIFont->m_strFontResID;

	return 0;
}

// 外部使用
int FontManager::AddCustomFont(IN CIUIProject *pOwner, LPCTSTR lpszFontResID, HFONT hFont)
{
	if (pOwner == NULL
		|| lpszFontResID == NULL
		|| hFont == NULL
		|| CIUIString(lpszFontResID).IsEmpty())
	{
		return -1;
	}

	// Find the id exist
	std::map<CIUIString, HIUIFONT>::iterator it = pOwner->GetFontMgr()->m_mapFonts.find(lpszFontResID);

	// Not found the font, create the font, and add to map.
	if (it != pOwner->GetFontMgr()->m_mapFonts.end())
	{
		return -2; // Already exist
	}

	HIUIFONT hIUIFont = new CIUIFont(hFont);
	hIUIFont->SetOwner(pOwner);
	hIUIFont->AddRef();

	pOwner->GetFontMgr()->m_mapFonts[lpszFontResID] = hIUIFont;

	return 0;
}

int FontManager::UpdateIUIFont(IN CIUIProject *pOwner,
	LPCTSTR lpszFontResID, HFONT hFont, BOOL bDeleteOld)
{
	if (pOwner == NULL
		|| lpszFontResID == NULL
		|| hFont == NULL
		|| CIUIString(lpszFontResID).IsEmpty())
	{
		return -1;
	}

	// Find the id exist
	std::map<CIUIString, HIUIFONT>::iterator it = pOwner->GetFontMgr()->m_mapFonts.find(
			lpszFontResID);

	// Not found the font
	if (it == pOwner->GetFontMgr()->m_mapFonts.end())
	{
		return -2;
	}

	if (bDeleteOld)
	{
		DeleteObject((HFONT)it->second);
	}
	else
	{
		it->second->Release();
	}

	HIUIFONT hIUIFont = new CIUIFont(hFont);
	hIUIFont->SetOwner(pOwner);
	hIUIFont->AddRef();

	pOwner->GetFontMgr()->m_mapFonts[lpszFontResID] = hIUIFont;

	return 0;
}

BOOL FontManager::FindIUIFont(IN CIUIProject *pOwner, LPCTSTR lpszFontResID)
{
	if (pOwner == NULL || lpszFontResID == NULL || CIUIString(lpszFontResID).IsEmpty())
	{
		return FALSE;
	}

	// Find the id exist
	std::map<CIUIString, HIUIFONT>::iterator it = pOwner->GetFontMgr()->m_mapFonts.find(
			lpszFontResID);

	// Not found the font
	if (it == pOwner->GetFontMgr()->m_mapFonts.end())
	{
		return FALSE;
	}

	return TRUE;
}

HFONT FontManager::CreateIUIFont(IN CIUIProject *pOwner, LPCTSTR lpszFontResID)
{
	if (pOwner == NULL)
	{
		return NULL;
	}

	HIUIFONT hIUIFont = CreateIUIFontInternal(pOwner, lpszFontResID);
	if (hIUIFont == NULL)
	{
		return NULL;
	}

	return hIUIFont->GetSafeHFont();
}

int FontManager::ReleaseIUIFont(IN CIUIProject *pOwner, LPCTSTR lpszFontResID)
{
	if (pOwner == NULL
		|| lpszFontResID == NULL
		|| CIUIString(lpszFontResID).IsEmpty()
		|| pOwner->GetFontMgr()->m_mapFonts.empty())
	{
		return -1;
	}

	std::map<CIUIString, HIUIFONT>::iterator it = pOwner->GetFontMgr()->m_mapFonts.find(
			lpszFontResID);

	// Not found the font
	if (it == pOwner->GetFontMgr()->m_mapFonts.end())
	{
		return -2;
	}

	_ASSERT(it->second != NULL);

	int nAddRef = it->second->Release();
	if (nAddRef == 0)
	{
		delete it->second;
		pOwner->GetFontMgr()->m_mapFonts.erase(it);
	}

	return nAddRef;
}

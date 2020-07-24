#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


HFONT CreateControlFont(__in CFontProp *pResFont, const LOGFONT *plfDefault)
{
	HFONT hFont = NULL;

	if (pResFont == NULL)
	{
		//if (CUIMgr::IsEnableDPI())
		//{
		//	LOGFONT lfDPI;
		//	memcpy(&lfDPI, plfDefault, sizeof(LOGFONT));
		//	GetDPILogFont(&lfDPI);

		//	hFont = CreateFontIndirect(&lfDPI);
		//}
		//else
		{
			hFont = CreateFontIndirect(plfDefault);
		}
	}
	else
	{
		//if (CUIMgr::IsEnableDPI())
		//{
		//	LOGFONT lfDPI;
		//	memcpy(&lfDPI, &(LOGFONT)pResFont->m_pFontResProp->m_logFont, sizeof(LOGFONT));
		//	GetDPILogFont(&lfDPI);

		//	hFont = CreateFontIndirect(&lfDPI);
		//}
		//else
		{
#ifdef _DEBUG
			LOGFONT lf = (LOGFONT)pResFont->m_logFont;
#endif
			hFont = CreateFontIndirect(&(LOGFONT)pResFont->m_logFont);
		}
	}

	return hFont;
}

CIUIFont::CIUIFont()
{
	m_nAddRef = 0;
	m_hFont = NULL;
	m_pOwner = NULL;
}

CIUIFont::CIUIFont(HFONT hFont)
{
	m_nAddRef = 0;
	m_hFont = hFont;
	m_pOwner = NULL;
}

CIUIFont::~CIUIFont()
{
	if (m_nAddRef == 0)
	{
		::DeleteObject(m_hFont);
	}
}

int CIUIFont::SetOwner(CIUIProject *pProject)
{
	m_pOwner = pProject;
	return 0;
}

CIUIProject *CIUIFont::GetOwner()
{
	return m_pOwner;
}

CIUIFont::operator HFONT() const
{
	return m_hFont;
}

int CIUIFont::AddRef()
{
	m_nAddRef++;

	return m_nAddRef;
}

int CIUIFont::Release()
{
	m_nAddRef--;

	if (m_nAddRef <= 0)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	return m_nAddRef;
}

int CIUIFont::DeleteObject()
{
	::DeleteObject(m_hFont);
	m_hFont = NULL;

	return 0;
}

int CIUIFont::Load(LPCTSTR lpszFontResID)
{
	// Default font，取默认HIUIFONT，而不是CFontProp，因为CFontProp是临时的，生成HIUIFONT后
	// 即被销毁

	// 如果是默认字体
	if (GetDefaultFontID() == lpszFontResID)
	{
		CFontProp *pDefFontProp = m_pOwner->GetDefaultFontProp();

		LOGFONT lfDefault;
		ZeroMemory(&lfDefault, sizeof(LOGFONT));
		HFONT hFont = CreateControlFont(pDefFontProp, &lfDefault);

		if (hFont != NULL)
		{
			m_hFont = hFont;
			m_strFontResID = lpszFontResID;

			return 0;
		}
	}
	else
	{
		LOGFONT lfDefault;
		HIUIFONT  hDefaultFont = m_pOwner->GetFontMgr()->GetDefaultFont();
		if (hDefaultFont != NULL)
		{
			GetObject(hDefaultFont, sizeof(LOGFONT), &lfDefault);
			//GetDPILogFont(&lfDefault);
		}
		else
		{
			_ASSERT(FALSE);
		}

		CFontProp *pResFont = m_pOwner->GetFontProp(lpszFontResID);
		if (pResFont != NULL)
		{
			HFONT hFont = CreateControlFont(pResFont, &lfDefault);

			if (hFont != NULL)
			{
				m_hFont = hFont;
				m_strFontResID = lpszFontResID;

				return 0;
			}
		}
	}

	return 0;
}

int CIUIFont::SafeLoadSavedFont()
{
	if (this == NULL)
	{
		return -1;
	}

	if (m_hFont != NULL)
	{
		return 1;
	}

	int nRet = Load(m_strFontResID);
	if (nRet != 0)
	{
		return nRet;
	}

	return 0;
}

HFONT CIUIFont::GetSafeHFont()
{
	if (this == NULL)
	{
		return NULL;
	}

	SafeLoadSavedFont();

	return m_hFont;
}

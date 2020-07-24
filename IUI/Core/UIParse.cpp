// UIMgr.cpp: implementation of the COldUIMgr and CUIParse class.
//

#include "stdafx.h"
using namespace IUI;

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


#define WM_ISUIWND                    WM_USER+15    // Is the window Window or its derive class.
// GetProp
#define GETPROP_OLDWNDPROC                    _T("IUI_OLD_WNDPROC")
#define GETPROP_PANEL                        _T("IUI_HWND_PANEL")
#define GETPROP_SPLITTER_POINTER            _T("IUI_SPLITTER_POINTER")
#define GETPROP_UIWND_STRINGID                _T("IUI_UIWND_STRINGID")
#define GETPROP_MENUBAR_BUTTON_INDEX        _T("IUI_MENUBAR_BUTTON_INDEX")
#define GETPROP_TASKWNDMGR_TIMERSTRUCT        _T("IUI_TASKWNDMGR_TIMER_STRUCT")
#define GETPROP_WLMGR                        _T("IUI_WLMGR")


// 当使用微软RichEdit源代码调试时，如果不注释掉，delete内存时会引起崩溃，这是由于微软RichEdit
// 源代码里，重载了new和delete。
//#ifdef _DEBUG
//#define new IUI_DEBUG_NEW
//#endif


#define IUI_BUILD_VERSION        2        // The version of xml format

int CheckFileWritable(const CIUIString &strFile, BOOL bWarning/* = TRUE*/)
{
	BOOL bRet = PathFileExists(strFile);

	if (!bRet)
	{
		// If file not exist, create it first.
		FILE *fp = TFOPEN(strFile, _T("wb"));

		if (fp == NULL)
		{
			return -1;
		}

		fclose(fp);
	}

	DWORD dwAttrib = GetFileAttributes(strFile);

	if (dwAttrib & FILE_ATTRIBUTE_READONLY)
	{
		if (bWarning)
		{
			CIUIString strInfo;
			strInfo.Format(_T("The file \"%s\" is readonly!"), strFile);
			::MessageBox(NULL, strInfo, NULL, MB_OK);
		}

		return -2;
	}

	return 0;
}

BOOL g_bCheckChangeList =
	TRUE;    // if the the.iui load by IUIEditor.exe, don't check change list, otherwise, can't load the the.iui.


//////////////////////////////////////////////////////////////////////////
// for load

int LoadFont(XML::IXMLDOMElementPtr pFontNode, LOGFONT *plf)
{
	if (pFontNode == NULL)
	{
		return -1;
	}

	if (plf == NULL)
	{
		return -2;
	}

	_variant_t var = pFontNode->getAttribute(_T("Height"));

	if (var.vt != VT_NULL)
	{
		plf->lfHeight = (LONG)var;
	}

	var = pFontNode->getAttribute(_T("Width"));

	if (var.vt != VT_NULL)
	{
		plf->lfWidth = (LONG)var;
	}

	var = pFontNode->getAttribute("lfEscapement");

	if (var.vt != VT_NULL)
	{
		plf->lfEscapement = (LONG)var;
	}

	var = pFontNode->getAttribute(_T("Orientation"));

	if (var.vt != VT_NULL)
	{
		plf->lfOrientation = (LONG)var;
	}

	var = pFontNode->getAttribute(_T("Weight"));

	if (var.vt != VT_NULL)
	{
		plf->lfWeight = (LONG)var;
	}

	var = pFontNode->getAttribute(_T("Italic"));

	if (var.vt != VT_NULL)
	{
		plf->lfItalic = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("Underline"));

	if (var.vt != VT_NULL)
	{
		plf->lfUnderline = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("StrikeOut"));

	if (var.vt != VT_NULL)
	{
		plf->lfStrikeOut = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("CharSet"));

	if (var.vt != VT_NULL)
	{
		plf->lfCharSet = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("OutPrecision"));

	if (var.vt != VT_NULL)
	{
		plf->lfOutPrecision = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("ClipPrecision"));

	if (var.vt != VT_NULL)
	{
		plf->lfClipPrecision = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("Quality"));

	if (var.vt != VT_NULL)
	{
		plf->lfQuality = (BYTE)var;
	}

	var = pFontNode->getAttribute(_T("PitchAndFamily"));

	if (var.vt != VT_NULL)
	{
		plf->lfPitchAndFamily = (BYTE)var;
	}

	_variant_t varFacename = pFontNode->getAttribute(_T("FaceName"));

	if (varFacename.vt != VT_NULL)
	{
		StringCchCopy(plf->lfFaceName, 32, _bstr_t(varFacename));
	}

	return 0;
}

int LoadElementAttributeVariant(
	XML::IXMLDOMElementPtr pParentElement,
	UIVARIANT *pVariant,
	CControlBaseProp *pCtrlProperties)
{
	_ASSERT(pVariant != NULL);
	_ASSERT(pVariant->varC4251.strIUIElementName.length() > 0);

	XML::IXMLDOMElementPtr pElement = pParentElement->selectSingleNode(
			pVariant->varC4251.strIUIElementName);

#ifdef _DEBUG
	CIUIString sText = (LPCTSTR)pVariant->varC4251.strIUIElementName;
	if (sText == _T("FontIndex"))
	{
		int n = 0;
	}
#endif

	if (pElement == NULL)
	{
		return 1;
	}

	if (pVariant->vtEx == VT_EMPTY
		|| pVariant->vtEx == VT_EX_CONTROL_ID_MACRO
		|| pVariant->vtEx == VT_EX_RESOURCE_FONT
		|| pVariant->vtEx == VT_EX_RESOURCE_COLOR
		|| pVariant->vtEx == VT_EX_STYLE_MENU
		|| pVariant->vtEx == VT_EX_STYLE_HEADERCTRL
		|| pVariant->vtEx == VT_EX_STYLE_SPIN
		|| pVariant->vtEx == VT_EX_STYLE_SCROLLBAR
		|| pVariant->vtEx == VT_EX_STYLE_TOOLTIPS
		|| pVariant->vtEx == VT_EX_WINDOW_ID
		|| pVariant->vtEx == VT_EX_WINDOW_ID_NOT_INCLUDE_SELF
		|| pVariant->vtEx == VT_EX_TASKMGR_ID)
	{
		switch (pVariant->varC4275.varThis.vt)
		{
		case VT_BOOL:
		{
			_variant_t var = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

			if (var.vt != VT_NULL)
			{
				var.wReserved1 = 0;
				var.wReserved2 = 0;
				var.wReserved3 = 0;

				CIUIString str = StringFromVariant(var);

				if (str.CompareNoCase(_T("false")) == 0 || str.CompareNoCase(_T("0")) == 0)
				{
					*pVariant = false;
				}
				else
				{
					*pVariant = true;
				}

				pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
			}
		}
		break;

		case VT_I4:
		case VT_INT:
		{
			_variant_t var = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

			if (var.vt != VT_NULL)
			{
				var.wReserved1 = 0;
				var.wReserved2 = 0;
				var.wReserved3 = 0;

				*pVariant = LONG(var);
				pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
			}
		}
		break;

		case VT_BSTR:
		default:
		{
			_variant_t var = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

			if (var.vt != VT_NULL)
			{
				var.wReserved1 = 0;
				var.wReserved2 = 0;
				var.wReserved3 = 0;

				*pVariant = var;
				pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
			}
		}
		break;
		}

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_RECT)
	{
		_ASSERT(pVariant->varC4275.varThis.vt == (VT_ARRAY | VT_I4));

		CIUIRect rect(0, 0, 0, 0);
		_variant_t varLeft = pElement->getAttribute(_T("Left"));

		if (varLeft.vt != VT_NULL)
		{
			rect.left = varLeft;
		}

		_variant_t varTop = pElement->getAttribute(_T("Top"));

		if (varTop.vt != VT_NULL)
		{
			rect.top = varTop;
		}

		_variant_t varRight = pElement->getAttribute(_T("Right"));

		if (varRight.vt != VT_NULL)
		{
			rect.right = varRight;
		}

		_variant_t varBottom = pElement->getAttribute(_T("Bottom"));

		if (varBottom.vt != VT_NULL)
		{
			rect.bottom = varBottom;
		}

		*pVariant = rect;
		pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_RECT_WH)
	{
		_ASSERT(pVariant->varC4275.varThis.vt == (VT_ARRAY | VT_I4));

		_variant_t varSize = pElement->getAttribute(_T("Size"));
		CIUIString strFirst, strSecond;
		RECT rect;
		rect.left = rect.top = rect.right = rect.bottom = 0;
		_variant_t varLocation = pElement->getAttribute(_T("Location"));

		if (varLocation.vt != VT_NULL)
		{
			CIUIString strLocation = StringFromVariant(varLocation);
			ExtractSubString(strFirst, strLocation, 0, ',');
			ExtractSubString(strSecond, strLocation, 1, ',');

			rect.left = IntFromString(strFirst);
			rect.top = IntFromString(strSecond);
		}

		if (varSize.vt != VT_NULL)
		{
			CIUIString strSize = StringFromVariant(varSize);
			ExtractSubString(strFirst, strSize, 0, ',');
			ExtractSubString(strSecond, strSize, 1, ',');
			int nWidth = IntFromString(strFirst);
			int nHeight = IntFromString(strSecond);

			rect.right = rect.left + nWidth;
			rect.bottom = rect.top + nHeight;
		}

		*pVariant = rect;
		pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_POINT)
	{
		_variant_t varX = pElement->getAttribute(_T("X"));
		_variant_t varY = pElement->getAttribute(_T("Y"));

		if (varX.vt != VT_NULL)
		{
			pVariant->SetPointX(varX);
			pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
		}

		if (varY.vt != VT_NULL)
		{
			pVariant->SetPointY(varY);
			pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
		}

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_COLOR)
	{
		_variant_t var = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

		if (var.vt == VT_NULL)
		{
			return 2;
		}

		CIUIString strValue = StringFromVariant(var);

		if (strValue.GetLength() > 0 && strValue[0] == '#')
		{
			strValue.Delete(0, 1);
			strValue.Insert(0, _T("0x"));
		}

		*pVariant = LongFromString(strValue);
		pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_FONT)
	{
		LOGFONT lf;
		LoadFont(pElement, &lf);
		*pVariant = lf;
		pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_BITMAPPATH)
	{
		_variant_t var = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

		if (var.vt != VT_NULL)
		{
			var.wReserved1 = 0;
			var.wReserved2 = 0;
			var.wReserved3 = 0;

			*pVariant = var;
			pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
		}

		return 0;
	}
	else if (pVariant->vtEx == VT_EX_IMAGE_RESIZE_MODE
		|| pVariant->vtEx == VT_EX_LAYOUT_HOR
		|| pVariant->vtEx == VT_EX_LAYOUT_VER
		|| pVariant->vtEx == VT_EX_DOCK
		|| pVariant->vtEx == VT_EX_ROOT_LAYOUT_TYPE
		|| pVariant->vtEx == VT_EX_TEXT_ALIGN_HOR
		|| pVariant->vtEx == VT_EX_TEXT_ALIGN_VER
		|| pVariant->vtEx == VT_EX_FOREGROUND_ALIGN_HOR
		|| pVariant->vtEx == VT_EX_FOREGROUND_ALIGN_VER
		|| pVariant->vtEx == VT_EX_BUTTON_TYPE
		|| pVariant->vtEx == VT_EX_CHECKBOX_TYPE
		|| pVariant->vtEx == VT_EX_WINDOW_TYPE
		|| pVariant->vtEx == VT_EX_COMBOBOX_STYLE
		|| pVariant->vtEx == VT_EX_SPN_ALIGN
		|| pVariant->vtEx == VT_EX_SLD_EXT_STYLE
		|| pVariant->vtEx == VT_EX_LST_STYLE
		|| pVariant->vtEx == VT_EX_LST_ALIGN
		|| pVariant->vtEx == VT_EX_LST_SORT
		|| pVariant->vtEx == VT_EX_DATETIME_FORMAT
		|| pVariant->vtEx == VT_EX_BACKGROUND_TYPE
		|| pVariant->vtEx == VT_EX_PROGRESS_THUMB_MODE
		|| pVariant->vtEx == VT_EX_PROGRESS_THUMB_POSITION
		|| pVariant->vtEx == VT_EX_ALPHA_FORMAT
		|| pVariant->vtEx == VT_EX_MENU_BORDER_STYLE
		|| pVariant->vtEx == VT_EX_SPLITTER_ORIENTATION
		//|| pVariant->vtEx == VT_EX_STACK_PANEL_ORIENTATION
		|| pVariant->vtEx == VT_EX_PEN_STYLE)
	{
		_variant_t strValue = pElement->getAttribute(pVariant->varC4251.strIUIAttributeName);

		if (strValue.vt != VT_NULL)
		{
			strValue.wReserved1 = 0;
			strValue.wReserved2 = 0;
			strValue.wReserved3 = 0;

			UIVARIANT var = strValue;
			pVariant->InitByEnumString((LPCTSTR)var);
			pVariant->varC4275.varThis.wReserved1 = 1;    // means the value is loaded from *.iui file.
		}

		return 0;
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}

int LoadIUIVersion(XML::IXMLDOMElementPtr pRootElement, LONG *plVersion, LONG *plBuild,
	LONG *plChangeList)
{
	_ASSERT(pRootElement != NULL);

	XML::IXMLDOMElementPtr pVersionElement = pRootElement->selectSingleNode("Version");

	if (pVersionElement == NULL)
	{
		return -1;
	}

	_variant_t str = pVersionElement->getAttribute("Version");

	if (plVersion != NULL && str.vt != VT_NULL)
	{
		*plVersion = LONG(str);
	}

	str = pVersionElement->getAttribute("Build");

	if (plBuild != NULL && str.vt != VT_NULL)
	{
		*plBuild = LONG(str);
	}

	str = pVersionElement->getAttribute("ChangeList");

	if (plChangeList != NULL && str.vt != VT_NULL)
	{
		*plChangeList = LONG(str);
	}

	return 0;
}

int LoadProjectSettings(XML::IXMLDOMElementPtr pRootElement, CIUIString &strOpenProgram)
{
	_ASSERT(pRootElement != NULL);

	//     BOOL bExist;// = IsNodeExist(pRootElement, "Configuration");
	//     if (!bExist)
	//         return -1;

	XML::IXMLDOMElementPtr pConfiguration = pRootElement->selectSingleNode("Configuration");

	if (pConfiguration == NULL)
	{
		return -1;
	}

	_variant_t str = pConfiguration->getAttribute("OpenProgram");
	strOpenProgram = StringFromVariant(str);

	return 0;
}

// Since only save different property of control while save story frame.
// If load story frame item, use near previous control's properties as default value.
// Use control's ID to find the near previous control's properties.
int LoadControlProperties(
	XML::IXMLDOMElementPtr pControl,
	__in __out CControlProp *pCtrlProperties,
	BOOL bLoadStoryFrameItem = FALSE,
	const CWindowProp *pParentUIProp = NULL,
	int nCurFrame = -1)
{
	if (pControl == NULL || pCtrlProperties == NULL)
	{
		return -1;
	}

	if (bLoadStoryFrameItem && pParentUIProp == NULL)
	{
		return -2;
	}

	LONG lVarCount = pCtrlProperties->GetPropertyCount();
	CONTROL_TYPE eControlType = (CONTROL_TYPE)pCtrlProperties->m_eControlType;

	for (LONG i = 0; i < lVarCount ; i++)
	{
		UIVARIANT *pVariant = pCtrlProperties->GetProperty(i);

		if (pVariant->vtEx == VT_EX_STRUCT_SIZE
			|| pVariant->vtEx == VT_EX_CONTROL_ID
			|| pVariant->vtEx == VT_EX_CONTROL_TYPE
			|| !pVariant->bSaveToSkinFile)
		{
			continue;
		}

		int nRet = LoadElementAttributeVariant(pControl, pVariant, pCtrlProperties);

		if (nRet != 0)   // The property not found in *.iui file.
		{
			continue;
		}

		//// If the property is a font, convert is to resource list.
		//if (pVariant->vtEx == VT_EX_FONT && pVariant->bNeedToConvert)
		//{
		//    // Find the font exist or not.
		//    CFontProp *pFoundFontProp = CUIParse::FindFontResource(*pVariant);
		//    if (pFoundFontProp == NULL)
		//    {
		//        // Create a new font resource.
		//        pFoundFontProp = new CFontProp();

		//        // ID
		//        CIUIString strFontResID;
		//        strFontResID.Format(_T("%s%d"), CControlProp::GetIDPrefix(CT_RESOURCE_FONT), ++CUIParse::m_nNextFontResID);
		//        pFoundFontProp->strID = strFontResID;

		//        // Font content
		//        pFoundFontProp->logFont = (LOGFONT)*pVariant;
		//        if (COldUIMgr::IsEnableDPI())
		//            GetDPILogFont(&pFoundFontProp->logFont);

		//        // Save to the font resource list
		//        CFontResItem *pFri = new CFontResItem;
		//        pFri->m_bConditionFont = false;
		//        pFri->m_pFontResProp = pFoundFontProp;
		//        CUIParse::AddFontResourceItem(pFri);
		//    }

		//    // The old font property convert to use the new font resource.
		//    CIUIString strIUIElementName = (LPCTSTR)pVariant->strIUIElementName;
		//    if (strIUIElementName == _T("FontN"))
		//    {
		//        pCtrlProperties->strResFontIDN = (LPCTSTR)pFoundFontProp->strID;    // Only string field need to be converted.
		//    }
		//    else if (strIUIElementName == _T("FontH"))
		//    {
		//        pCtrlProperties->strResFontIDH = (LPCTSTR)pFoundFontProp->strID;
		//    }
		//    else if (strIUIElementName == _T("FontS"))
		//    {
		//        pCtrlProperties->strResFontIDS = (LPCTSTR)pFoundFontProp->strID;
		//    }
		//    else if (strIUIElementName == _T("FontD"))
		//    {
		//        pCtrlProperties->strResFontIDD = (LPCTSTR)pFoundFontProp->strID;
		//    }
		//}
		//// If the property is a color, convert is to resource list.
		//else if (pVariant->vtEx == VT_EX_COLOR && pVariant->bNeedToConvert)
		//{
		//    // Find the color exist.
		//    CColorProp *pFoundColorProp = (CColorProp *)CUIParse::FindColorResource(*pVariant);
		//    if (pFoundColorProp == NULL)
		//    {
		//        // Create a new color resource.
		//        pFoundColorProp = new CColorProp();

		//        // ID
		//        CIUIString strColorResID;
		//        strColorResID.Format(_T("%s%d"), CControlProp::GetIDPrefix(CT_RESOURCE_COLOR), ++CUIParse::m_nNextColorResID);
		//        pFoundColorProp->strID = strColorResID;

		//        // Color content
		//        pFoundColorProp->crColor = (LONG)(COLORREF)*pVariant;

		//        // Save to the color resource list
		//        CUIParse::AddColorResourceItem(pFoundColorProp);
		//    }

		//    // The old color property convert to use the new color resource.
		//    CIUIString strIUIElementName = (LPCTSTR)pVariant->strIUIElementName;
		//    CIUIString strIUIAttributeName = (LPCTSTR)pVariant->varC4251.strIUIAttributeName;
		//    if (eControlType == CT_UIWND)
		//    {
		//        CWindowProp *pUIProp = (CWindowProp *)pCtrlProperties;
		//        if (strIUIElementName == (LPCTSTR)pUIProp->crMask.strIUIElementName)
		//        {
		//            pUIProp->strMaskColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//    }
		//    else if (eControlType == CT_TREEVIEW)
		//    {
		//        CTreeViewProp *pTreeProp = (CTreeViewProp *)pCtrlProperties;

		//        if (strIUIElementName == (LPCTSTR)pTreeProp->crItemLine.strIUIElementName
		//            && strIUIAttributeName == (LPCTSTR)pTreeProp->crItemLine.strIUIAttributeName)
		//        {
		//            pTreeProp->strItemLineColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//        else if (strIUIElementName == (LPCTSTR)pTreeProp->crSelectedItemBkColor.strIUIElementName
		//            && strIUIAttributeName == (LPCTSTR)pTreeProp->crSelectedItemBkColor.strIUIAttributeName)
		//        {
		//            pTreeProp->strSelectedItemBkColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//        else if (strIUIElementName == (LPCTSTR)pTreeProp->crHighlightItemBkColor.strIUIElementName
		//            && strIUIAttributeName == (LPCTSTR)pTreeProp->crHighlightItemBkColor.strIUIAttributeName)
		//        {
		//            pTreeProp->strHighlightItemBkColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//    }
		//    else if (eControlType == CT_WAVECTRL)
		//    {
		//        WAVECTRLPROPERTIES *pWaveProp = (WAVECTRLPROPERTIES *)pCtrlProperties;

		//        if (strIUIElementName == (LPCTSTR)pWaveProp->crGridLine.strIUIElementName && strIUIAttributeName == (LPCTSTR)pWaveProp->crGridLine.strIUIAttributeName)
		//        {
		//            pWaveProp->strGridLineColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//        else if (strIUIElementName == (LPCTSTR)pWaveProp->crBackground.strIUIElementName && strIUIAttributeName == (LPCTSTR)pWaveProp->crBackground.strIUIAttributeName)
		//        {
		//            pWaveProp->strBackgroundColorResID = (LPCTSTR)pFoundColorProp->strID;
		//        }
		//    }

		//    // All control have the following properties.
		//    if (strIUIElementName == _T("ColorN"))
		//    {
		//        pCtrlProperties->strResColorIDN = (LPCTSTR)pFoundColorProp->strID;    // Only string field need to be converted.
		//    }
		//    else if (strIUIElementName == _T("ColorH"))
		//    {
		//        pCtrlProperties->strResColorIDH = (LPCTSTR)pFoundColorProp->strID;
		//    }
		//    else if (strIUIElementName == _T("ColorS"))
		//    {
		//        pCtrlProperties->strResColorIDS = (LPCTSTR)pFoundColorProp->strID;
		//    }
		//    else if (strIUIElementName == _T("ColorD"))
		//    {
		//        pCtrlProperties->strResColorIDD = (LPCTSTR)pFoundColorProp->strID;
		//    }
		//}
	}

	return 0;
}

int Global::LoadResourceStyleProperties(XML::IXMLDOMElementPtr pResourceStyle,
	CControlBaseProp *pResStyleProp)
{
	if (pResourceStyle == NULL || pResStyleProp == NULL)
	{
		return -1;
	}

	LONG lVarCount = pResStyleProp->GetPropertyCount();

	for (LONG i = 0; i < lVarCount ; i++)
	{
		UIVARIANT *pVariant = pResStyleProp->GetProperty(i);

		if (pVariant->vtEx == VT_EX_STRUCT_SIZE || pVariant->vtEx == VT_EX_CONTROL_ID
			|| pVariant->vtEx == VT_EX_CONTROL_TYPE || !pVariant->bSaveToSkinFile)
		{
			continue;
		}

		int nRet = LoadElementAttributeVariant(pResourceStyle, pVariant, pResStyleProp);
	}

	return 0;
}

CONTROL_TYPE Global::GetControlTypeEnum(LPCTSTR lpszControlType)
{
	CONTROL_TYPE ect = CT_ARROW;

	for (int i = CT_ARROW; i < CT_LAST; ++i)
	{
		CIUIString strTypeString = GetControlTypeString((CONTROL_TYPE)i, FALSE);

		if (strTypeString == lpszControlType)
		{
			ect = (CONTROL_TYPE)i;
			break;
		}
	}

	return ect;
}

int LoadControl(XML::IXMLDOMElementPtr pControl, CControlProp *pParentProp)
{
	if (pControl == NULL || pParentProp == NULL)
	{
		return -1;
	}

	BSTR bstr;
	pControl->get_nodeName(&bstr);
	CIUIString strType = (LPCTSTR)_bstr_t(bstr);
	CONTROL_TYPE eControlType = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr));

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	CControlProp *pCtrlProperties = NULL;

	switch (eControlType)
	{
	case CT_PUSHBUTTON:
	{
		pCtrlProperties = new CButtonProp;
	}
	break;

	case CT_CHECK:
	{
		pCtrlProperties = new CCheckBoxProp;
	}
	break;

	case CT_RADIO:
	{
		pCtrlProperties = new CRadioButtonProp;
	}
	break;

	case CT_STATIC:
	{
		pCtrlProperties = new CLabelProp;
	}
	break;

	case CT_RICHEDIT:
	{
		pCtrlProperties = new CRichTextBoxProp;
	}
	break;

	case CT_COMBOBOX:
	{
		pCtrlProperties = new CComboBoxProp;
	}
	break;

	case CT_SPIN:
	{
		pCtrlProperties = new CSpinButtonProp;
	}
	break;

	case CT_PROGRESS:
	{
		pCtrlProperties = new CProgressBarProp;
	}
	break;

	case CT_SLIDER:
	{
		pCtrlProperties = new CSliderProp;
	}
	break;

	case CT_HOTKEY:
		//{
		//    pCtrlProperties = new CUIHotKeyProp;
		//}
		break;

	case CT_IPADDRESS:
		//{
		//    pCtrlProperties = new CUIIPAddressProp;
		//}
		break;

	case CT_LISTVIEW:
	{
		pCtrlProperties = new CListViewProp;
	}
	break;

	case CT_TREEVIEW:
	{
		pCtrlProperties = new CTreeViewProp;
	}
	break;

	case CT_SCROLLBAR:
	{
		pCtrlProperties = new CScrollBarProp;
	}
	break;

	case CT_TREELIST:
		//{
		//    pCtrlProperties = new TREELISTPROPERTIES;
		//}
		break;

	case CT_HTMLVIEW:
	{
		pCtrlProperties = new CHtmlViewProp;
	}
	break;

	case CT_SPLITTER:
	{
		pCtrlProperties = new CSplitterBarProp;
	}
	break;

	case CT_IM:
	{
		pCtrlProperties = new CIMRichTextBoxProp;
	}
	break;

	case CT_DATETIMEPICKER:
	{
		pCtrlProperties = new CDateTimePickerProp;
	}
	break;

	case CT_MENUBAR:
		//{
		//    pCtrlProperties = new MENUBARPROPERTIES;
		//}
		break;

	// Panel
	case CT_PNL_TABCONTROL:
	{
		pCtrlProperties = new CTabControlProp;
		pCtrlProperties->m_eControlType = CT_PNL_TABCONTROL;
	}
	break;

	case CT_PNL_CANVAS:
	{
		pCtrlProperties = new CCanvasProp;
		pCtrlProperties->m_eControlType = CT_PNL_CANVAS;
	}
	break;

	case CT_PNL_DOCK:
	{
		pCtrlProperties = new CDockPanelProp;
		pCtrlProperties->m_eControlType = CT_PNL_DOCK;
	}
	break;

	case CT_UNIFORM_GRID:
	{
		pCtrlProperties = new CUniformGridProp;
		pCtrlProperties->m_eControlType = CT_UNIFORM_GRID;
	}
	break;

	case CT_PNL_STACK:
	{
		pCtrlProperties = new CStackPanelProp;
		pCtrlProperties->m_eControlType = CT_PNL_STACK;
	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	LoadControlProperties(pControl, pCtrlProperties);

	pParentProp->AddChild(pCtrlProperties);

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

			LoadControl(pChildCtrl, pCtrlProperties);

			pChildCtrl = pChildCtrl->GetnextSibling();
		}
	}

	return 0;
}

int LoadControls(XML::IXMLDOMElementPtr pControls, CWindowProp *pUIProp)
{
	if (pControls == NULL || pUIProp == NULL)
	{
		return -1;
	}

#ifdef _DEBUG
	BSTR bstr;
	pControls->get_nodeName(&bstr);
	CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	_ASSERT(strValue == _T("Controls"));
#endif

	XML::IXMLDOMNode *pChildNode = NULL;
	HRESULT hr = pControls->get_firstChild(&pChildNode);
	int nControlIndex4Debug = 0;

	while (!FAILED(hr) && pChildNode != NULL)
	{
		LoadControl(pChildNode, pUIProp);

		hr = pChildNode->get_nextSibling(&pChildNode);
		nControlIndex4Debug++;
	}

	return 0;
}

int LoadWindow(XML::IXMLDOMElementPtr pWindow, CWindowProp *pUIProp)
{
	if (pWindow == NULL || pUIProp == NULL)
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
	LoadControlProperties(pWindow, pUIProp);

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
		LoadControl(pRootNode, pUIProp);

		pRootNode = pRootNode->GetnextSibling();
	}

	return 0;
}

int LoadColorResources(XML::IXMLDOMElementPtr pResources, CColorProp *pResDefaultColor,
	std::vector<CColorProp *> *pvpColorRes)
{
	if (pResources == NULL || pvpColorRes == NULL)
	{
		return -1;
	}

#ifdef _DEBUG
	BSTR bstr;
	pResources->get_nodeName(&bstr);
	CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	_ASSERT(strValue == _T("Resources"));
#endif

	//
	// Default Color
	//
	XML::IXMLDOMElementPtr pColor = pResources->selectSingleNode("DefaultColor");

	if (pColor != NULL)
	{
		XML::IXMLDOMNode *pChildNode = NULL;
		HRESULT hr = pColor->get_firstChild(&pChildNode);

		while (!FAILED(hr) && pChildNode != NULL)
		{
#ifdef _DEBUG
			pChildNode->get_nodeName(&bstr);
			CONTROL_TYPE ect = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr));

			if (bstr != NULL)
			{
				SysFreeString(bstr);
			}

			_ASSERT(ect == CT_RESOURCE_COLOR);
#endif // _DEBUG

			LoadResourceStyleProperties(pChildNode, pResDefaultColor);

			hr = pChildNode->get_nextSibling(&pChildNode);
		}
	}

	//
	// Color
	//
	XML::IXMLDOMElementPtr pColors = pResources->selectSingleNode("Colors");

	if (pColors != NULL)
	{
		XML::IXMLDOMNode *pChildNode = NULL;
		HRESULT hr = pColors->get_firstChild(&pChildNode);

		while (!FAILED(hr) && pChildNode != NULL)
		{
#ifdef _DEBUG
			pChildNode->get_nodeName(&bstr);
			CONTROL_TYPE ect = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr));

			if (bstr != NULL)
			{
				SysFreeString(bstr);
			}

			_ASSERT(ect == CT_RESOURCE_COLOR);
#endif // _DEBUG

			CColorProp *pColorRes = new CColorProp();
			LoadResourceStyleProperties(pChildNode, pColorRes);
			pvpColorRes->push_back(pColorRes);

			hr = pChildNode->get_nextSibling(&pChildNode);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIParse::CUIParse()
{
	m_bDelayLoadWindow = TRUE;
	m_pProject = NULL;
	m_lIUIMajorVersion = -1;
	m_lIUIMinorVersion = -1;
	m_lIUIBuildVersion = -1;
	m_lIUIChangeList = -1;
	m_lIUIMajorVersion = 1;
	m_lIUIMinorVersion = 0;
	m_lIUIBuildVersion = IUI_BUILD_VERSION;
	m_pWindow = NULL;
	m_bOutsizeSetWinProp = false;

	m_pvIDs = new std::vector < std::pair<CIUIString, int> > ;
	m_pmapStrIntIDs = new std::map < CIUIString, int > ;
	m_pmapIntStrIDs = new std::map < int, CIUIString > ;
}

CUIParse::~CUIParse()
{
	if (!m_bOutsizeSetWinProp && m_pWindow != NULL)
	{
		delete m_pWindow;
		m_pWindow = NULL;
	}

	delete m_pvIDs;
	m_pvIDs = NULL;
	delete m_pmapStrIntIDs;
	m_pmapStrIntIDs = NULL;
	delete m_pmapIntStrIDs;
	m_pmapIntStrIDs = NULL;
}


#define LOAD_STYLE(StyleGroupName, vpStyle, StructPropName) \
	{ \
		XML::IXMLDOMElementPtr pStyleGroup = pStyles->selectSingleNode(StyleGroupName); \
		if (pStyleGroup != NULL) \
		{ \
			XML::IXMLDOMNode *pChildNode = NULL; \
			HRESULT hr = pStyleGroup->get_firstChild(&pChildNode); \
			while (!FAILED(hr) && pChildNode != NULL) \
			{ \
				BSTR bstr; \
				pChildNode->get_nodeName(&bstr); \
				CONTROL_TYPE m_eControlType = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr)); \
				if (bstr != NULL) \
					SysFreeString(bstr); \
				StructPropName *pStyleProp = new StructPropName(); \
				pStyleProp->m_eControlType = m_eControlType; \
				LoadResourceStyleProperties(pChildNode, pStyleProp); \
				vpStyle.push_back(pStyleProp); \
				hr = pChildNode->get_nextSibling(&pChildNode); \
			} \
		} \
	}


int CUIParse::LoadSkin()
{
	DWORD dw1 = GetTickCount();
	// Load XMLResource.h
	LoadIDDefined();

	DWORD dw2 = GetTickCount() - dw1;
	IUITRACE(_T("[IUI]=== Resource.h spend %d ms\n"), dw2);

	XML::IXMLDOMDocumentPtr xmlDoc(__uuidof(DOMDocument));
	xmlDoc->async = FALSE;
	HRESULT hr = E_FAIL;

	if (m_pProject->IsLoadSkinFromResource())
	{
		LPVOID lpBuffer = NULL;
		DWORD dwSize = 0;
		int nRet = LoadCustomResourceA((LPCSTR)_bstr_t(m_strXmlID),
				(LPCSTR)_bstr_t(m_pProject->GetUIResourceType()),
				&lpBuffer, &dwSize);

		if (nRet != 0)
		{
			return E_NOIUIRC;
		}

		// Transform UTF-8 to UTF-16
		std::wstring wstrXml = MultiByteToUTF16((char *)lpBuffer + 3, dwSize - 3, CP_UTF8);   // Ignore BOM
		hr = xmlDoc->loadXML(wstrXml.c_str());
	}
	else
	{
		hr = xmlDoc->load(m_strUIPath.GetBuffer(0));
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

	if (strRootType != _T("IUI"))
	{
		return -10;
	}

	// Load project settings
	CIUIString strOpenProgram;
	LoadProjectSettings(pRootElement, strOpenProgram);

	// 	// Load version of *.iui file.
	// 	LONG lVersion = 0;
	// 	int nRet = LoadIUIVersion(pRootElement, &lVersion, &m_lIUIBuildVersion, &m_lIUIChangeList);
	//
	// 	if (nRet != 0)
	// 	{
	// 		return nRet;
	// 	}
	//
	// 	m_lIUIMajorVersion = lVersion / 100;
	// 	m_lIUIMinorVersion = lVersion % 100;
	//
	// 	BOOL bPromptConvert = FALSE;
	// 	BOOL bPromptCannotOpen = FALSE;
	//
	// 	if (m_lIUIMajorVersion < m_lIUIMajorVersion)
	// 	{
	// 		bPromptCannotOpen = TRUE;
	// 	}
	// 	else if (m_lIUIMajorVersion == m_lIUIMajorVersion)
	// 	{
	// 		if (m_lIUIMinorVersion < m_lIUIMinorVersion)
	// 		{
	// 			bPromptConvert = TRUE;
	// 		}
	// 		else if (m_lIUIMinorVersion == m_lIUIMinorVersion)
	// 		{
	// 			if (m_lIUIBuildVersion < m_lIUIBuildVersion)
	// 			{
	// 				bPromptConvert = TRUE;
	// 			}
	// 			else if (m_lIUIBuildVersion == m_lIUIBuildVersion)
	// 			{
	//
	// 			}
	// 			else
	// 			{
	// 				bPromptCannotOpen = TRUE;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			bPromptCannotOpen = TRUE;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		bPromptCannotOpen = TRUE;
	// 	}
	//
	// 	if (bPromptConvert)
	// 	{
	// 		return -1;
	// 		_ASSERT(!bPromptCannotOpen);
	// 		CIUIString strInfo;
	// 		strInfo.Format(
	// 			_T("The project\r\n\r\n'%s'\r\n\r\nmust be converted to the current project format. After it has been converted,\r\nyou will not be able to edit this project in previous versions of IUIEditor.\r\nConvert and open this project?"),
	// 			m_strUIPath);
	//
	// 		// Backup old the.iui
	// 		CopyFile(m_strUIPath, m_strUIPath + _T(".bak"), TRUE);
	// 	}
	//
	// 	if (bPromptCannotOpen)
	// 	{
	// 		_ASSERT(!bPromptConvert);
	// 		::MessageBox(NULL,
	// 			_T("The selected file is a IUI skin file, but was created by a newer version of this application and cannot be opened."), NULL,
	// 			MB_ICONSTOP);
	// 		return E_NEWERVERSION;
	// 	}

	// Load window
	if (m_pWindow == NULL)
	{
		m_pWindow = new CWindowProp;
	}

	XML::IXMLDOMElementPtr pWindow = pRootElement->selectSingleNode(_T("Window"));
	if (pWindow != NULL)
	{
		LoadWindow(pWindow, m_pWindow);
	}
	else
	{
		XML::IXMLDOMElementPtr pRootCtrl = pRootElement->selectSingleNode(_T("Root"));
		if (pRootCtrl != NULL)
		{
			// 标志这个CWindowProp不是真正的窗口，而是个子布局
			m_pWindow->m_bWindow = false;

			// 仅有一个成员
			XML::IXMLDOMNodePtr pRootLayout = pRootCtrl->GetfirstChild();
			XML::IXMLDOMNodePtr pTest = pRootLayout->GetnextSibling();
			_ASSERT(pTest == NULL);

			if (pRootLayout != NULL)
			{
				LoadControl(pRootLayout, m_pWindow);
			}
		}
	}

	return 0;
}

int CUIParse::LoadIDDefined()
{
	m_pvIDs->clear();
	m_pmapStrIntIDs->clear();
	m_pmapIntStrIDs->clear();
	m_fileResourceH.Detach();

	if (m_pProject->IsLoadSkinFromResource())
	{
		m_fileResourceH.OpenResource(NULL,
			m_pProject->GetXMLResourceID(), m_pProject->GetUIResourceType());
	}
	else
	{
		CIUIString strResourcePath = m_pProject->GetProjectPathWithoutTitle()
			+ m_pProject->GetXMLResourceID();
		_ASSERT(!strResourcePath.IsEmpty());

		FILE *fp = TFOPEN(strResourcePath.GetBuffer(strResourcePath.GetLength()), _T("rb"));

		if (fp == NULL)
		{
			return -1;
		}

		fseek(fp, 0, SEEK_END);
		int nLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		BYTE *pBuf = new BYTE[nLen];
		memset(pBuf, 0, nLen);
		fread(pBuf, nLen, 1, fp);
		fclose(fp);
		m_fileResourceH.Attach(pBuf, nLen, 0);
	}

	std::string strA;

	while (m_fileResourceH.ReadStringA(&strA))
	{
		CIUIString str = (LPCTSTR)_bstr_t(strA.c_str());
		int nTest = str.Find(_T("#define"));

		if (nTest != -1)
		{
			str.TrimRight();
			int nRight = str.ReverseFind(' ');
			CIUIString strID;
			strID = str.Right(str.GetLength() - nRight - 1);

			str.TrimLeft();
			str.TrimRight(strID);
			str.TrimRight();
			str.TrimLeft(_T("#define"));
			str.TrimLeft();

			//
			std::pair<CIUIString, int> aID;
			aID.first = str;
			aID.second = _ttoi(strID);
			m_pvIDs->push_back(aID);

			//
			(*m_pmapStrIntIDs)[aID.first] = aID.second;
			(*m_pmapIntStrIDs)[aID.second] = aID.first;
		}
	}

	if (!m_pProject->IsLoadSkinFromResource())
	{
		BYTE *pBuf = m_fileResourceH.Detach();
		delete []pBuf;
	}

	return 0;
}

int CUIParse::DelayLoadWindow(CWindowProp *pUIWndProp)
{
	return 0;
}

BOOL GenerateOneID(LPCTSTR lpszID, int &rnNextID, std::vector<std::pair<CIUIString, int> > &IdArray)
{
	CIUIString strPreDefinedID[] =
	{
		_T("IDC_STATIC"),

		_T("IDOK"),
		_T("IDCANCEL"),
		_T("IDABORT"),
		_T("IDRETRY"),
		_T("IDIGNORE"),
		_T("IDYES"),
		_T("IDNO"),
#if(WINVER >= 0x0400)
		_T("IDCLOSE"),
		_T("IDHELP"),
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
		_T("IDTRYAGAIN"),
		_T("IDCONTINUE"),
#endif /* WINVER >= 0x0500 */
	};

	int nPreDefinedIDCount = sizeof(strPreDefinedID) / sizeof(CIUIString);

	// Check the lpszID exist or not.
	BOOL bIsExist = FALSE;

	// If the id equal to the pre-defined ID
	for (int j = 0; j < nPreDefinedIDCount; ++j)
	{
		if (strPreDefinedID[j] == lpszID)
		{
			bIsExist = TRUE;
			break;
		}
	}

	if (bIsExist)
	{
		return FALSE;
	}

	size_t sizeTemp = IdArray.size();

	for (size_t i = 0; i < sizeTemp; ++i)
	{
		const std::pair<CIUIString, int> *pIDs = &IdArray[i];

		// If the id equal to the pre-defined ID
		if (pIDs->first.Compare(lpszID) == 0)
		{
			bIsExist = TRUE;
			break;
		}
	}

	if (bIsExist)
	{
		return FALSE;
	}

	// If the new ID not exist, add to ID list IdArray.
	IdArray.push_back(std::pair<CIUIString, int>(lpszID, rnNextID));

	rnNextID++;

	return TRUE;
}

int CUIParse::SetDelayLoadWindow(BOOL bDelayLoadWindow)
{
	m_bDelayLoadWindow = bDelayLoadWindow;

	return 0;
}

BOOL CUIParse::IsDelayLoadWindow()
{
	return m_bDelayLoadWindow;
}

int CompareVersion(int nCurMajor, int nCurMinor, int nHighlightMajor, int nHighlightManor)
{
	if (nCurMajor < nHighlightMajor)
	{
		return -1;
	}

	if (nCurMajor > nHighlightMajor)
	{
		return 1;
	}

	// nCurMajor == nHighlightMajor
	if (nCurMinor < nHighlightManor)
	{
		return -1;
	}

	if (nCurMinor > nHighlightManor)
	{
		return 1;
	}

	// nCurMajor == nHighlightMajor && nCurMinor == nHighlightManor
	return 0;
}

int CUIParse::SetProject(CIUIProject *pOwner)
{
	m_pProject = pOwner;
	return 0;
}

CIUIProject *CUIParse::GetProject()
{
	return m_pProject;
}

int CUIParse::SetUIPath(LPCTSTR lpszUIPath, BOOL bCheckVersion/* = TRUE*/)
{
	if (m_pProject == NULL)
	{
		_ASSERT(FALSE);
		return -1;
	}

	_ASSERT(lpszUIPath != NULL);
	_ASSERT(IUIIsValidString(lpszUIPath));

	if (m_pProject->IsLoadSkinFromResource())
	{
		m_strXmlID = lpszUIPath;
	}
	else
	{
		BOOL bIsAbsolutePath = FALSE;
		int nLen = (int)_tcslen(lpszUIPath);
		int i = 0;

		for (i = 0; i < nLen; ++i)
		{
			if (lpszUIPath[i] == ':')
			{
				bIsAbsolutePath = TRUE;
				break;
			}
		}

		if (bIsAbsolutePath)
		{
			m_strUIPath = lpszUIPath;
		}
		else
		{
			m_strUIPath = m_pProject->GetProjectPathWithoutTitle() + lpszUIPath;
		}

		if (!PathFileExists(m_strUIPath))
		{
			m_strUIPath.Empty();

			return E_SETUIPATH;
		}

		// init path of resource.h file.
		TCHAR szDisc[_MAX_PATH] = { 0 };
		TCHAR szDir[_MAX_PATH] = { 0 };
		TSPLITPATH(m_strUIPath, szDisc, _MAX_PATH, szDir, _MAX_PATH, NULL, 0, NULL, 0);
		StringCchCat(szDisc, _MAX_PATH, szDir);

		PathAddBackslash(szDir);
		StringCchCat(szDisc, _MAX_PATH, m_pProject->GetXMLResourceID());
	}

	// init windows tree struct
	DWORD dwTime = GetTickCount();
	int nRet = LoadSkin();
	DWORD dwTime2 = GetTickCount() - dwTime;
	IUITRACE(_T("[IUI]===Load skins spend %d ms\n"), dwTime2);

	if (nRet != 0)
	{
		return nRet;
	}

	// 	if (CompareVersion(GetIUIMajorVersion(), GetIUIMinorVersion(), GetIUIMajorVersion(), GetIUIMinorVersion()) < 0)
	// 	{
	// 		IUITRACEA(_T("[IUI]===Warning: The version of skin file older than the IUI, so some features will be lost.\n"));
	//
	// 		return E_OLDERVERSION;
	// 	}

	return 0;
}

int CUIParse::SetSource(LPCTSTR lpszUIPath)
{
	m_strUIPath = lpszUIPath;
	return 0;
}

CIUIString CUIParse::GetUIPath()
{
	return m_strUIPath;
}

CIUIString CUIParse::GetUIPathWithoutTitle()
{
	TCHAR szDisc[_MAX_PATH] = {0};
	TCHAR szPath[_MAX_PATH] = {0};
	TSPLITPATH(m_strUIPath, szDisc, _MAX_PATH, szPath, _MAX_PATH, NULL, 0, NULL, 0);
	m_strUIPathWithoutTitle = szDisc;
	m_strUIPathWithoutTitle += szPath;

	if (m_strUIPathWithoutTitle.Right(1) != _T("/") && m_strUIPathWithoutTitle.Right(1) != _T("\\"))
	{
		m_strUIPathWithoutTitle += "\\";
	}

	return m_strUIPathWithoutTitle;
}

int CUIParse::SetWindowProp(CWindowProp *pUIWndProp)
{
	m_bOutsizeSetWinProp = TRUE;
	m_pWindow = pUIWndProp;

	return 0;
}

CWindowProp *CUIParse::GetWindowProp()
{
	return m_pWindow;
}

CWindowProp *CUIParse::AllocUIWNDPROPERTIES()
{
	return new CWindowProp;
}

int CUIParse::FreeUIWNDPROPERTIES(CWindowProp *pUIProp)
{
	delete pUIProp;

	return 0;
}

struct EnumWindowParam
{
	std::map<HWND, CIUIString> *pMapCreatedHWndStringID;
	DWORD dwCurrentProcID;
};

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
	EnumWindowParam *pEwp = (EnumWindowParam *)lParam;

	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);

	if (dwPid == pEwp->dwCurrentProcID)
	{
		BOOL bUIWnd = (BOOL)::SendMessage(hWnd, WM_ISUIWND, 0, 0);

		if (bUIWnd)
		{
#ifdef _DEBUG
			TCHAR szbuf[MAX_PATH] = {0};
			::GetWindowText(hWnd, szbuf, MAX_PATH);
#endif
			LPCTSTR lpStringID = (LPCTSTR)::GetProp(hWnd, GETPROP_UIWND_STRINGID);
			(*(pEwp->pMapCreatedHWndStringID))[hWnd] = lpStringID;
		}

		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}

	return TRUE;
}

// Get all running Window
int CUIParse::InitRunningUIWnd()
{
	m_pmapCreatedHWndStringID->clear();

	EnumWindowParam ewp;
	ewp.pMapCreatedHWndStringID = m_pmapCreatedHWndStringID;
	ewp.dwCurrentProcID = ::GetCurrentProcessId();

	EnumChildWindows(NULL, EnumChildWindowCallBack, (LPARAM)&ewp);

	return 0;
}

int CUIParse::FindCtrlProp(/*in*/const UIVARIANT *pVarCtrlProp,
	/*out*/CControlBaseProp **ppCtrlProp)
{
	if (pVarCtrlProp == NULL || ppCtrlProp == NULL)
	{
		return -2;
	}

	*ppCtrlProp = NULL;

	// Find in window list
	CWindowProp *pUIProp = m_pWindow;

	LONG lVarCount = pUIProp->GetPropertyCount();

	for (LONG lVar = 0; lVar < lVarCount; ++lVar)
	{
		UIVARIANT *pVariant = pUIProp->GetProperty(lVar);

		if (pVariant == pVarCtrlProp)
		{
			*ppCtrlProp = pUIProp;

			break;
		}
	}

	if (*ppCtrlProp != NULL)
	{
		return -3;
	}

	// Find children
	size_t nChildrenCount = pUIProp->GetChildCount();

	for (size_t j = 0; j < nChildrenCount; ++j)
	{
		CControlBaseProp *pCtrlProp = pUIProp->GetChild(j);

		LONG lVarCount = pCtrlProp->GetPropertyCount();

		for (LONG lVar = 0; lVar < lVarCount; ++lVar)
		{
			UIVARIANT *pVariant = pCtrlProp->GetProperty(lVar);

			if (pVariant == pVarCtrlProp)
			{
				*ppCtrlProp = pCtrlProp;

				break;
			}
		}

		if (*ppCtrlProp != NULL)
		{
			break;
		}
	}

	if (*ppCtrlProp != NULL)
	{
		return -4;
	}

	return (*ppCtrlProp != NULL) ? 0 : -1;
}

std::vector<CIUIString> GetUseImage(const CControlProp *pCtrlProp)
{
	std::vector<CIUIString> vImages;

	// Check each properties.
	LONG lVarCount = pCtrlProp->GetPropertyCount();

	for (LONG i = 0; i < lVarCount ; i++)
	{
		const UIVARIANT *pVariant = pCtrlProp->GetProperty(i);

		if (pVariant->vtEx != VT_EX_BITMAPPATH)
		{
			continue;
		}

		CIUIString strImage = (LPCTSTR)(*pVariant);

		if (strImage.IsEmpty())
		{
			continue;
		}

		vImages.push_back(strImage);
	}

	return vImages;
}

LONG CUIParse::GetIUIMajorVersion()
{
	return m_lIUIMajorVersion;
}

LONG CUIParse::GetIUIMinorVersion()
{
	return m_lIUIMinorVersion;
}

LONG CUIParse::GetIUIBuildVersion()
{
	return m_lIUIBuildVersion;
}

LONG CUIParse::GetIUIChangeList()
{
	return m_lIUIChangeList;
}

#define RELEASE_RESOURCE_STYLE(vpResourceStyle, StructResStyleName) \
	nCount = vpResourceStyle.size(); \
	for (i=0; i<nCount; ++i) \
	{ \
		StructResStyleName *pResStyleProp = vpResourceStyle[i]; \
		delete pResStyleProp; \
	} \
	vpResourceStyle.clear();

int CUIParse::ReleaseSkin()
{
	// Don't release ole at here. if you do, you will:
	// 1. Always crash while close the UI editor that opened a skin.
	// 2. Can't update recent file list.
	// Release ole.
	// AfxOleTerm();

	BYTE *pBuf = m_fileResourceH.Detach();
	delete []pBuf;

	m_strUIPath.Empty();
	m_strUIPathWithoutTitle.Empty();

	// Release window.
	CWindowProp *pUIProp = m_pWindow;

	// controls
	pUIProp->DeleteAllChildren();

	delete pUIProp;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////

CFontParse::CFontParse()
{
	m_pProject = NULL;
}

CFontParse::~CFontParse()
{

}

int LoadFontResource(XML::IXMLDOMElementPtr pFontNode, CFontProp *presFont)
{
	if (pFontNode == NULL)
	{
		return -1;
	}
	if (presFont == NULL)
	{
		return -2;
	}

	BSTR bstr;
	pFontNode->get_nodeName(&bstr);
	CONTROL_TYPE ect = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr));
	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	if (ect == CT_RESOURCE_FONT)
	{
		LoadResourceStyleProperties(pFontNode, presFont);
	}

	return 0;
}

int CFontParse::SetProject(CIUIProject *pOwner)
{
	m_pProject = pOwner;

	return 0;
}

CIUIProject *CFontParse::GetProject()
{
	return m_pProject;
}

int CFontParse::Load(IUnknown *pFonts,
	__inout CFontProp **ppDefFontProp,
	__inout std::vector<CFontProp *> *pvpResFont)
{
	if (pFonts == NULL || ppDefFontProp == NULL || pvpResFont == NULL)
	{
		return -1;
	}

	XML::IXMLDOMElementPtr pFontsElement = (XML::IXMLDOMElementPtr)pFonts;

	//
	// Default Font
	//
	XML::IXMLDOMElementPtr pDefaultFont = pFontsElement->selectSingleNode("DefaultFont");
	if (pDefaultFont != NULL)
	{
		XML::IXMLDOMElementPtr pFont = pDefaultFont->selectSingleNode("FONT");
		if (pFont != NULL)
		{
			*ppDefFontProp = new CFontProp;
			LoadFontResource(pFont, *ppDefFontProp);

			// Assign guid as default font id.
			(*ppDefFontProp)->m_strName = (LPCTSTR)GetDefaultFontID();

			// Default font not show font ID, must be "Default".
			(*ppDefFontProp)->m_strName.bShowInPropertiesWnd = false;
			(*ppDefFontProp)->m_strName.bSaveToSkinFile = false;

			// 因为默认字体几乎一定会用到，所以提前创建出来，并AddRef使其不被删除
			// TODO: 如果要提前创建出来，一定要找合适的位置释放，否则会泄漏。
			//CreateIUIFont(m_pProject, (*ppDefFontProp)->m_strName);
		}
	}

	//
	// Font
	//
	XML::IXMLDOMElementPtr pGeneralFonts = pFontsElement->selectSingleNode("GeneralFonts");
	if (pGeneralFonts != NULL)
	{
		XML::IXMLDOMNode *pChildNode = NULL;
		HRESULT hr = pGeneralFonts->get_firstChild(&pChildNode);
		while (!FAILED(hr) && pChildNode != NULL)
		{
			CFontProp *pFri = new CFontProp;
			LoadFontResource(pChildNode, pFri);

			pvpResFont->push_back(pFri);

			hr = pChildNode->get_nextSibling(&pChildNode);
		}
	}

	// If not load the default font. specify one.
	if ((*ppDefFontProp) == NULL)
	{
		(*ppDefFontProp) = new CFontProp;
		(*ppDefFontProp)->m_strName.bShowInPropertiesWnd = false;
		(*ppDefFontProp)->m_strName.bSaveToSkinFile = false;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////


CColorParse::CColorParse()
{
	m_pProject = NULL;
}

CColorParse::~CColorParse()
{

}

int LoadColorResource(XML::IXMLDOMElementPtr pColorNode, CColorProp *presColor)
{
	if (pColorNode == NULL)
	{
		return -1;
	}
	if (presColor == NULL)
	{
		return -2;
	}

	BSTR bstr;
	pColorNode->get_nodeName(&bstr);
	CONTROL_TYPE ect = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr));
	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	if (ect == CT_RESOURCE_COLOR)
	{
		LoadResourceStyleProperties(pColorNode, presColor);
	}

	return 0;
}

int CColorParse::SetProject(CIUIProject *pOwner)
{
	m_pProject = pOwner;

	return 0;
}

CIUIProject *CColorParse::GetProject()
{
	return m_pProject;
}

int CColorParse::Load(IN IUnknown *pColors,
	__inout CColorProp **ppDefColorProp,
	__inout std::vector<CColorProp *> *pvpResColor)
{
	if (pColors == NULL || ppDefColorProp == NULL || pvpResColor == NULL)
	{
		return -1;
	}

	XML::IXMLDOMElementPtr pColorsElement = (XML::IXMLDOMElementPtr)pColors;

	//
	// Default Color
	//
	XML::IXMLDOMElementPtr pDefaultColor = pColorsElement->selectSingleNode("DefaultColor");
	if (pDefaultColor != NULL)
	{
		XML::IXMLDOMElementPtr pColor = pDefaultColor->selectSingleNode("COLOR");
		if (pColor != NULL)
		{
			*ppDefColorProp = new CColorProp;
			LoadColorResource(pColor, *ppDefColorProp);

			// Assign guid as default Color id.
			(*ppDefColorProp)->m_strName = GetDefaultColorID();

			// Default Color not show Color ID, must be "Default".
			(*ppDefColorProp)->m_strName.bShowInPropertiesWnd = false;
			(*ppDefColorProp)->m_strName.bSaveToSkinFile = false;
		}
	}

	//
	// Color
	//
	XML::IXMLDOMElementPtr pGeneralColors = pColorsElement->selectSingleNode("GeneralColors");
	if (pGeneralColors != NULL)
	{
		XML::IXMLDOMNode *pChildNode = NULL;
		HRESULT hr = pGeneralColors->get_firstChild(&pChildNode);
		while (!FAILED(hr) && pChildNode != NULL)
		{
			CColorProp *pFri = new CColorProp;
			LoadColorResource(pChildNode, pFri);

			pvpResColor->push_back(pFri);

			hr = pChildNode->get_nextSibling(&pChildNode);
		}
	}

	// If not load the default Color. specify one.
	if ((*ppDefColorProp) == NULL)
	{
		(*ppDefColorProp) = new CColorProp;
		(*ppDefColorProp)->m_strName.bShowInPropertiesWnd = false;
		(*ppDefColorProp)->m_strName.bSaveToSkinFile = false;
		(*ppDefColorProp)->m_strName = GetDefaultColorID();
	}

	return 0;
}

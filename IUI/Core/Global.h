#pragma once

namespace Global
{

	int IUISetControlFont(
		IN CIUIProject *pOwner,
		__inout HIUIFONT *phIUIFont,
		LPCTSTR lpszFontResID);

	int IUISetControlFont2(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIFONT *phIUIFont,
		LPCTSTR lpszFontResIDN,
		LPCTSTR lpszFontResIDD);

	int IUISetControlFont4(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIFONT *phIUIFont,
		LPCTSTR lpszFontResIDN,
		LPCTSTR lpszFontResIDH,
		LPCTSTR lpszFontResIDS,
		LPCTSTR lpszFontResIDD);

	int IUISetControlCheckedFont4(
		IN CIUIProject *pOwner,
		UINT uMask,
		__inout HIUIFONT *phIUIFont,
		LPCTSTR lpszFontResIDCN,
		LPCTSTR lpszFontResIDCH,
		LPCTSTR lpszFontResIDCS,
		LPCTSTR lpszFontResIDCD);

	int LoadResourceStyleProperties(XML::IXMLDOMElementPtr pResourceStyle,
		CControlBaseProp *pResStyleProp);
	CONTROL_TYPE GetControlTypeEnum(LPCTSTR lpszControlType);


} // namespace Global

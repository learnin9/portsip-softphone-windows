#pragma once


namespace ResourceMgr
{

	HIUIIMAGE CreateEmptyIUIImage(IN CIUIProject *pOwner, LPCTSTR lpszImageFile);

	HIUIFONT CreateEmptyIUIFontInternal(IN CIUIProject *pOwner, LPCTSTR lpszFontResID);
	HIUIFONT CreateIUIFontInternal(IN CIUIProject *pOwner, LPCTSTR lpszFontResID);
	int ReleaseIUIFontInternal(HIUIFONT hIUIFont);

	HFONT GetHFont(HIUIFONT hIUIFont);
	int GetFontResID(HIUIFONT hIUIFont, CIUIString *pstrFontResID);

} // namespace ResourceMgr

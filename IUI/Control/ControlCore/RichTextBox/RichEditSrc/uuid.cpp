/*
 *	UUID.C
 *
 *	Purpose:
 *		provide definitions for locally used GUID's
 *
 *	Copyright (c) 1995-1996, Microsoft Corporation. All rights reserved.
 */
#include "stdafx.h"
#include "_common.h"

//set these two GUIDs up for export in our file

#undef IID_RichEditOle
#undef IID_IRichEditOleCallback
__declspec(dllexport)DEFINE_GUID(IID_IRichEditOle,         0x00020D00, 0, 0,
	0xC0, 0, 0, 0, 0, 0, 0, 0x46);
__declspec(dllexport)DEFINE_GUID(IID_IRichEditOleCallback, 0x00020D03, 0, 0,
	0xC0, 0, 0, 0, 0, 0, 0, 0x46);

#undef DEFINE_GUID
#undef DEFINE_OLEGUID

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const IID name \
		= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#define DEFINE_OLEGUID(name, l, w1, w2) \
	DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)


// REMARK: presumably TOM should have official MS GUIDs
// To make pre-compiled headers work better, we just copy the
// guid definitions here.  Make sure they don't change!

DEFINE_GUID(LIBID_TOM,			0x8CC497C9,	0xA1DF, 0x11ce, 0x80, 0x98, 0x00, 0xAA,
	0x00, 0x47, 0xBE, 0x5D);
DEFINE_GUID(IID_ITextSelection,	0x8CC497C1,	0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA,
	0x00, 0x47, 0xBE, 0x5D);
DEFINE_GUID(IID_ITextRange,		0x8CC497C2,	0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA,
	0x00, 0x47, 0xBE, 0x5D);
DEFINE_GUID(IID_ITextFont,		0x8CC497C3,	0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA,
	0x00, 0x47, 0xBE, 0x5D);
DEFINE_GUID(IID_ITextPara,		0x8CC497C4,	0xA1DF, 0x11CE, 0x80, 0x98, 0x00, 0xAA,
	0x00, 0x47, 0xBE, 0x5D);

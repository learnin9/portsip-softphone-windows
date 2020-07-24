// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_)
#define AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_

#pragma once


#define _CRT_SECURE_NO_DEPRECATE

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // Windows Server 2003, Windows XP
#endif

#include <windows.h>
#include <olectl.h>
#include <CommCtrl.h>
#include <atlconv.h>
#include <ShlObj.h>
#include <algorithm>
using namespace std;

#include "IUI.h"
using namespace IUI;
using namespace ImageManager;
using namespace Render;
using namespace TimeLine;
using namespace CreateControls;
using namespace DPI;
using namespace Metrics;
#pragma comment (lib, "Msimg32.lib") //for AlphaBlend
#include "Core/ResourceMgr.h"
using namespace ResourceMgr;
#include "Control/ControlCore/ControlMember.h"
using namespace ControlMember;
#include "Control/ControlCore/Win32Global.h"
#include "Core/WLWndMgr.h"
#include "Core/Global.h"
using namespace CWLWndMgr;
using namespace ControlResMgr;
using namespace FontManager;
using namespace Global;

#define CLAMP(x,a,b) (min(b,max(a,x)))


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// 最后包含strsafe.h，否则像strcpy之类的编译不过
#include <strsafe.h>

#define IUI_DEBUG_NEW	new(_NORMAL_BLOCK, __FILE__, __LINE__)

int LoadControlProperties(
	XML::IXMLDOMElementPtr pControl,
	__in __out CControlProp *pCtrlProperties,
	BOOL bLoadStoryFrameItem/* = FALSE*/,
	const CWindowProp *pParentUIProp/* = NULL*/,
	int nCurFrame/* = -1*/);
int LoadControl(XML::IXMLDOMElementPtr pControl, CControlProp *pParentProp);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E30B2003_188B_4EB4_AB99_3F3734D6CE6C__INCLUDED_)

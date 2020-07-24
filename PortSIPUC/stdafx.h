// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include<memory>
#define  FULL_VERSION 1
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

#include "json/json.h"

#include <PortSIPLib/portsipuc.hxx>
#include <PortSIPLib/PortSIPErrors.hxx>
#include <PortSIPLib/CallbackParameters.hxx>
#include <PortSIPLib/AbstractCallbackDispatcher.hxx>
using namespace PortSIP;
#include <Mmsystem.h>

#ifdef WIN32
#pragma comment(lib, "./lib/x86/portsip_uc_sdk.lib")
#pragma  comment(lib,"./lib/x86/libcurl_a.lib")
#if _DEBUG
#pragma comment(lib, "./lib/x86/log4cplusUD.lib")
#pragma comment(lib,"libjsoncppD.lib")
#pragma comment(lib,"libbreakpadd.lib")
#pragma comment(lib,"libbreakpad_clientd.lib")
#else
#pragma comment(lib, "./lib/x86/log4cplusU.lib")
#pragma comment(lib,"libjson.lib")
#pragma comment(lib,"libbreakpad.lib")
#pragma comment(lib,"libbreakpad_client.lib")
#endif
#else
#pragma comment(lib, "./lib/x64/portsip_sdk.lib")
#pragma comment(lib, "./lib/x86/log4cplusU.lib")
#pragma  comment(lib,"./lib/x86/libcurl_a.lib.lib")

#endif

#include "winsock2.h"
#pragma  comment(lib,"ws2_32.lib")
#pragma  comment(lib,"icuuc.lib")
#pragma  comment(lib,"icuin.lib")
#pragma  comment(lib, "Winmm.lib")

#include "CommonDef.h"
#include "log/Logger.h"
#include "Utility/utility.hxx"

#include <IUI.h>
using namespace IUI;
using namespace ImageManager;
using namespace Render;
using namespace TimeLine;
using namespace CreateControls;

#include "Global.h"
using namespace PortGo;

#if (_MSC_VER == 1200)	// VC6.0
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_60ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_60udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_60d.lib")
#else // /MTd
#pragma comment (lib, "IUI_60dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_60u.lib")
#else // /MT
#pragma comment (lib, "IUI_60uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_60.lib")
#else // /MT
#pragma comment (lib, "IUI_60MT.lib")
#endif
#endif // end _UNICODE
#endif
#include <ATLCONV.H>
#endif // (_MSC_VER == 1200)	// VC6.0

#if (_MSC_VER == 1310)	// VC2003
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2003ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2003udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2003d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2003dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2003u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2003uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2003.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2003MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1310)	// VC2003

#if (_MSC_VER == 1400)	// VC2005
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2005ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2005udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2005d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2005dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2005u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2005uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2005.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2005MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1400)	// VC2005

#if (_MSC_VER == 1500)	// VC2008
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2008ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2008udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2008d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2008dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2008u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2008uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2008.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2008MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1500)	// VC2008

#if (_MSC_VER == 1600)	// VC2010
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2010ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2010udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2010d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2010dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2010u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2010uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2010.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2010MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1600)	// VC2010

#if (_MSC_VER == 1700)	// VC2012
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2012ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2012udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2012d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2012dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2012u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2012uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2012.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2012MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1700)	// VC2012

#if (_MSC_VER == 1800)	// VC2013
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2013ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2013udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2013d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2013dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2013u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2013uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2013.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2013MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1800)	// VC2013

#if (_MSC_VER == 1900)	// VC2015
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2015ud.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2015udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2015d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2015dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2015u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2015uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2015.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2015MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER == 1900)	// VC2015

#if (_MSC_VER >= 1910)	// VC2017
#ifdef _DEBUG
#ifdef _UNICODE
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2017udMDd.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2017udMTd.lib")
#endif
#else // ANSI
#ifdef _DLL	// /MDd
#pragma comment (lib, "IUI_vs2017d.lib")
#else // /MTd
#pragma comment (lib, "IUI_vs2017dMTd.lib")
#endif
#endif // end _UNICODE
#else // Release
#ifdef _UNICODE
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2017u.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2017uMT.lib")
#endif
#else // ANSI
#ifdef _DLL // /MD
#pragma comment (lib, "IUI_vs2017.lib")
#else // /MT
#pragma comment (lib, "IUI_vs2017MT.lib")
#endif
#endif // end _UNICODE
#endif
#endif // (_MSC_VER >= 1910)	// VC2017

// TODO: reference additional headers your program requires here

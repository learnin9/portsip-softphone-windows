#pragma once


namespace IUI
{
	typedef struct tagFADE
	{
		HANDLE hsprite;
		HDC hdc;
		HBITMAP hbm;
		POINT ptDst;
		SIZE size;
		DWORD dwTime;
		DWORD dwStart;
		DWORD dwFlags;
	} FADE, *PFADE;

	extern FADE gfade;
	extern int gcxMenuFontChar;
	extern int gcyMenuFontChar;
	extern HDC ghdcMem2;
	extern int gcxMenuFontOverhang;
	extern int gcyMenuFontAscent;
	extern HFONT ghMenuFontDef;
	extern int gcyMenuFontExternLeading;
	extern HFONT ghMenuFont;

#define cxSysFontChar			24
#define SYSMET(CXSIZEFRAME)		4


#define DSS_INACTIVE	0x0040

#define MENUTEXT		1
#define treeDFACE		2
#define GRAYTEXT		3
	COLORREF SYSRGB(int nIndex);

	/*
	 * The following is the header of all objects managed in the handle list.
	 * (allocated as part of the object for easy access).  All object
	 * headers must start with the members of a HEAD structure.
	 */
	typedef struct _HEAD
	{
		HANDLE h;
		DWORD cLockObj;
	} HEAD, *PHEAD;

	/*
	 * Monitor information structure.
	 *
	 *     This structure defines the attributes of a single monitor
	 *     in a virtual display.
	 */
	typedef struct tagMONITOR
	{
		HEAD                    head;            // object handle stuff

		struct tagMONITOR      *pMonitorNext;    // next monitor in free or used list
		DWORD                   dwMONFlags;      // flags
		RECT                    rcMonitor;       // location of monitor in virtual screen coordinates
		RECT                    rcWork;          // work area of monitor in virtual screen coordinates
		HRGN                    hrgnMonitor;     // monitor region in virtual screen coordinates
		short                   cFullScreen;     // number of fullscreen apps on this monitor
		short                   cWndStack;       // number of tiled top-level windows
		HANDLE                  hDev;            // hdev associated with this monitor
	} MONITOR, *PMONITOR;

	typedef struct _PROCOBJHEAD : public HEAD
	{
		//HEAD;
		DWORD hTaskWow;
	} PROCOBJHEAD, *PPROCOBJHEAD;

	typedef struct tagDESKTOP           *PDESKTOP;

	typedef struct _DESKHEAD
	{
		PDESKTOP rpdesk;
		PBYTE pSelf;
	} DESKHEAD, *PDESKHEAD;

	typedef struct _PROCDESKHEAD : public PROCOBJHEAD, DESKHEAD
	{
		//PROCOBJHEAD;
		//DESKHEAD;
	} PROCDESKHEAD, *PPROCDESKHEAD;

	/*
	 * Menu Item Structure
	 */
	typedef struct tagITEM
	{
		UINT    fType;          // Item Type  Flags
		UINT    fState;         // Item State Flags
		UINT    wID;
		struct tagMENU *spSubMenu;  /* Handle to a popup */
		HANDLE  hbmpChecked;    /* Bitmap for an on  check */
		HANDLE  hbmpUnchecked;   /* Bitmap for an off check */
		LPWSTR  lpstr;          //item's text
		DWORD   cch;                /* String: WCHAR count */
		ULONG_PTR dwItemData;
		DWORD   xItem;			// Item的X坐标
		DWORD   yItem;			// Item的Y坐标
		DWORD   cxItem;			// Item的宽度
		DWORD   cyItem;			// Item的高度
		DWORD   dxTab;
		DWORD   ulX;            /* String: Underline start */
		DWORD   ulWidth;        /* String: underline width */
		HBITMAP hbmp;           // item's bitmap
		int     cxBmp;          // bitmap width
		int     cyBmp;          // bitmap height
	} ITEM, *PITEM, *LPITEM;

	/*
	 * MENULIST structure, holds the PMENUs that contain a submenu
	 * We store a list of menus in MENU.pParentMenus as a menu
	 * can be submenu in more items
	 */
	typedef struct tagMENULIST
	{
		struct tagMENULIST   *pNext;
		struct tagMENU       *pMenu;
	} MENULIST, * PMENULIST;

	/*
	 * Menu Structure
	 */
	typedef struct tagMENU
	{
		PROCDESKHEAD    head;
		DWORD           fFlags;         /* Menu Flags */
		int             iItem;          /* Contains the position of the selected
                                       item in the menu. -1 if no selection */
		UINT            cAlloced;       // rgItems数据的长度（注意不是rgItems中数据的个数）
		UINT            cItems;         // rgItems中数据的个数

		DWORD           cxMenu;
		DWORD           cyMenu;
		DWORD           cxTextAlign;    /* Text align offset for popups*/
		HWND            spwndNotify;     /* The owner hwnd of this menu */
		PITEM           rgItems;        /* The list of items in this menu */
		PMENULIST       pParentMenus;   // The list of parents (menus that have this as submenu)
		DWORD           dwContextHelpId;// Context help Id for the whole menu
		DWORD           cyMax;          /* max menu height after which menu scrolls */
		DWORD dwMenuData;     /* app-supplied menu data */

		HBRUSH         hbrBack;        // background brush for menu
		int             iTop;           // Scroll top
		int             iMaxTop;        // Scroll MaxTop
		DWORD           dwArrowsOn: 2;  // Scroll flags
	} MENU, * PMENU;

	typedef struct tagPOPUPMENU         *PPOPUPMENU;
	typedef struct tagTHREADINFO        *PTHREADINFO;

	/* NEW MENU STUFF */
	typedef struct tagPOPUPMENU
	{

		DWORD  fIsMenuBar: 1;       /* This is a hacked struct which refers to the
                              * menu bar associated with a app. Only true if
                              * in the root ppopupMenuStruct.
                              */
		DWORD  fHasMenuBar: 1;      /* This popup is part of a series which has a
                              * menu bar (either a sys menu or top level menu
                              * bar)
                              */
		DWORD  fIsSysMenu: 1;   /* The system menu is here. */
		DWORD  fIsTrackPopup: 1;   /* Is TrackPopup popup menu */
		DWORD  fDroppedLeft: 1;
		DWORD  fHierarchyDropped: 1; /* If true, a submenu has been dropped off this popup */
		DWORD  fRightButton: 1;    /* Allow right button in menu */
		DWORD  fToggle: 1;         /* If TRUE, button up cancels the popup */
		DWORD  fSynchronous: 1;    /* For synchronous return value of cmd chosen */
		DWORD  fFirstClick: 1;      /* Keep track if this was the first click on the
                              * top level menu bar item.  If the user down/up
                              * clicks on a top level menu bar item twice, we
                              * want to cancel menu mode.
                              */
		DWORD  fDropNextPopup: 1;  /* Should we drop hierarchy of next menu item w/ popup? */
		DWORD  fNoNotify: 1;       /* Don't send WM_ msgs to owner, except WM_COMMAND  */
		DWORD  fAboutToHide: 1;    // Same purpose as fHideTimer?
		DWORD  fShowTimer: 1;      // TRUE if the IDSYS_MNSHOW timer is set
		DWORD  fHideTimer: 1;      // TRUE if the IDSYS_MNHIDE timer is set

		DWORD  fDestroyed: 1;       /* Set when the owner menu window has been destroyed
                              *  so the popup can be freed once it's no longer needed
                              * Also set in root popupmenu when menu mode must end
                              */

		DWORD  fDelayedFree: 1;    /* Avoid freeing the popup when the owner menu
                             *  window is destroyed.
                             * If set, it must be a root popupmenu or must
                             *  be linked in ppmDelayedFree
                             * This is eventually set for all hierarchical popups
                             */

		DWORD  fFlushDelayedFree: 1; /* Used in root popupmenus only.
                               * Set when a hierarchical popup marked as fDelayedFree
                               *  has been destroyed.
                               */


		DWORD  fFreed: 1;          /* Popup has been freed. Used for debug only */

		DWORD  fInCancel: 1;       /* Popup has been passed to xxxMNCancel */

		DWORD  fTrackMouseEvent: 1; /* TrackMouseEvent has been called */
		DWORD  fSendUninit: 1;     /* Send WM_UNINITMENUPOPUP */
		DWORD  fRtoL: 1;           /* For going backwards with the keys */
		DWORD  fDesktopMenu: 1;    /* Set if this popup belons to the pdesk->spwndMenu window */
		DWORD  iDropDir: 5;        /* Animation direction */


		HWND           spwndNotify;
		/* Window who gets the notification messages. If this
		 * is a window with a menu bar, then this is the same
		 * as hwndPopupMenu.
		 */
		HWND           spwndPopupMenu;
		/* The window associated with this ppopupMenu struct.
		 * If this is a top level menubar, then hwndPopupMenu
		 * is the window the menu bar. ie. it isn't really a
		 * popup menu window.
		 */
		HWND spwndNextPopup; // 当前菜单弹出的下级菜单，如果下级菜单被销毁，或无下级菜单，则为NULL.
		HWND           spwndPrevPopup;
		/* The previous popup in the hierarchy. NULL if at top
		 */
		PMENU          spmenu;/* The PMENU displayed in this window
                         */
		PMENU          spmenuAlternate;
		/* Alternate PMENU. If the system menu is displayed,
		 * and a menubar menu exists, this will contain the
		 * menubar menu. If menubar menu is displayed, this
		 * will contain the system menu. Use only on top level
		 * ppopupMenu structs so that we can handle windows
		 * with both a system menu and a menu bar menu.  Only
		 * used in the root ppopupMenuStruct.
		 */

		HWND           spwndActivePopup; /* This is the popup the mouse/"keyboard focus" is on */

		PPOPUPMENU     ppopupmenuRoot;

		PPOPUPMENU     ppmDelayedFree;       /* List of hierarchical popups marked
                                        *  as fDelayedFree.
                                        */

		UINT   posSelectedItem;  /* Position of the selected item in this menu
                            */
		UINT   posDropped;

	} POPUPMENU;

	/*
	 * Menu animation GDI objects.
	 */
	typedef struct tagMENUANIDC
	{
		HDC     hdcAni;         // Scratch dc for animation
	} MENUANIDC;

	/*
	 * Menu Control Structure
	 */
	typedef struct tagMENUSTATE : public MENUANIDC
	{
		PPOPUPMENU pGlobalPopupMenu;
		DWORD   fMenuStarted : 1;
		DWORD   fIsSysMenu : 1;
		DWORD   fInsideMenuLoop : 1;
		DWORD   fButtonDown: 1;
		DWORD   fInEndMenu: 1;
		DWORD   fUnderline: 1;              /* Shorcut key underlines are shown */
		DWORD   fButtonAlwaysDown: 1;       /* The mouse has always been down since the menu started */
		DWORD   fDragging: 1;               /* Dragging (in DoDragDrop) or about to */
		DWORD   fModelessMenu: 1;           /* No modal loop */
		DWORD   fInCallHandleMenuMessages: 1; /* processing a msg from CallHandleMM */
		DWORD   fDragAndDrop: 1;            /* This menu can do drag and drop */
		DWORD   fAutoDismiss: 1;            /* This menu goes away on its own if mouse is off for certain time */
		DWORD   fAboutToAutoDismiss: 1;     /* Autodismiss will take place when timer goes off */
		DWORD   fIgnoreButtonUp: 1;         /* Eat next button up, i.e, cancel dragging */
		DWORD   fMouseOffMenu: 1;           /* Mouse is off the menu - modeless menus only */
		DWORD   fInDoDragDrop: 1;           /* in a WM_MENUDODRAGDROP callback */
		DWORD   fActiveNoForeground: 1;     /* A menu window is active but we're not in the foreground */
		DWORD   fNotifyByPos: 1;            /* Use WM_MENUCOMMAND */
		DWORD   fSetCapture: 1;             /* True if the menu mode set capture */
		DWORD   iAniDropDir: 5;             /* direction of animation */

		POINT   ptMouseLast;
		int     mnFocus;
		int     cmdLast;
		PTHREADINFO ptiMenuStateOwner;

		DWORD   dwLockCount;

		struct tagMENUSTATE *pmnsPrev;      /* Previous menustate for nested/context menus */

		POINT   ptButtonDown;               /* Mouse down position (begin drag position) */
		ULONG_PTR uButtonDownHitArea;        /* Return from xxxMNFindWindowFromPoint on button down */
		UINT    uButtonDownIndex;           /* Index of the item being dragged */

		int     vkButtonDown;               /* Mouse button being dragged */

		ULONG_PTR uDraggingHitArea;          /* Last hit area while InDoDragDrop */
		UINT    uDraggingIndex;             /* Last index  */
		UINT    uDraggingFlags;             /* Gap flags */

		HDC     hdcWndAni;      // window DC while animating
		DWORD   dwAniStartTime; // starting time of animation
		int     ixAni;          // current x-step in animation
		int     iyAni;          // current y-step in animation
		int     cxAni;          // total x in animation
		int     cyAni;          // total y in animation
		HBITMAP hbmAni;         // Scratch bmp for animation.

		/*
		 * Important: The following structure must be the last
		 *  thing in tagMENUSTATE. MNAllocMenuState doesn't NULL out
		 *  this structure
		 */
		//MENUANIDC;

	} MENUSTATE, *PMENUSTATE;

#define CWINHOOKS       (WH_MAX - WH_MIN + 1)

	typedef struct tagTHREADINFO
	{
		//W32THREAD;

		//***************************************** begin: USER specific fields

		//PTL             ptl;                // Listhead for thread lock list

		//PPROCESSINFO    ppi;                // process info struct for this thread

		//PQ              pq;                 // keyboard and mouse input queue

		//PKL             spklActive;         // active keyboard layout for this thread

		//PCLIENTTHREADINFO pcti;             // Info that must be visible from client

		//PDESKTOP        rpdesk;
		//PDESKTOPINFO    pDeskInfo;          // Desktop info visible to client
		//PCLIENTINFO     pClientInfo;        // Client info stored in TEB

		DWORD           TIF_flags;          // TIF_ flags go here.

		//PUNICODE_STRING pstrAppName;        // Application module name.

		//PSMS            psmsSent;           // Most recent SMS this thread has sent
		//PSMS            psmsCurrent;        // Received SMS this thread is currently processing
		//PSMS            psmsReceiveList;    // SMSs to be processed

		LONG            timeLast;           // Time, position, and ID of last message
		ULONG_PTR        idLast;

		int             cQuit;
		int             exitCode;

		HDESK           hdesk;              // Desktop handle
		int             cPaintsReady;
		UINT            cTimersReady;

		PMENUSTATE      pMenuState;

		//union
		//{
		//	PTDB            ptdb;           // Win16Task Schedule data for WOW thread
		//	PWINDOWSTATION  pwinsta;        // Window station for SYSTEM thread
		//};

		//PSVR_INSTANCE_INFO psiiList;        // thread DDEML instance list
		DWORD           dwExpWinVer;
		DWORD           dwCompatFlags;      // The Win 3.1 Compat flags
		DWORD           dwCompatFlags2;     // new DWORD to extend compat flags for NT5+ features

		//PQ              pqAttach;           // calculation variabled used in
		// zzzAttachThreadInput()

		PTHREADINFO     ptiSibling;         // pointer to sibling thread info

		//PMOVESIZEDATA   pmsd;

		DWORD           fsHooks;                // WHF_ Flags for which hooks are installed
		HHOOK           sphkCurrent;            // Hook this thread is currently processing

		//PSBTRACK        pSBTrack;

		HANDLE          hEventQueueClient;
		//PKEVENT         pEventQueueServer;
		LIST_ENTRY      PtiLink;            // Link to other threads on desktop
		int             iCursorLevel;       // keep track of each thread's level
		POINT           ptLast;

		HWND            spwndDefaultIme;    // Default IME Window for this thread
		//PIMC            spDefaultImc;       // Default input context for this thread
		HKL             hklPrev;            // Previous active keyboard layout
		int             cEnterCount;
		//MLIST           mlPost;             // posted message list.
		USHORT          fsChangeBitsRemoved;// Bits removed during PeekMessage
		WCHAR           wchInjected;        // character from last VK_PACKET
		DWORD           fsReserveKeys;      // Keys that must be sent to the active
		// active console window.
		//PKEVENT        *apEvent;            // Wait array for xxxPollAndWaitForSingleObject
		ACCESS_MASK     amdesk;             // Granted desktop access
		UINT            cWindows;           // Number of windows owned by this thread
		UINT            cVisWindows;        // Number of visible windows on this thread

		HHOOK           aphkStart[CWINHOOKS];   // Hooks registered for this thread
		//CLIENTTHREADINFO  cti;              // Use this when no desktop is available
	} THREADINFO;

	/* OEM Bitmap Information Structure */
	typedef struct tagOEMBITMAPINFO
	{
		tagOEMBITMAPINFO()
		{
			x = y = 0;
			cx = cy = 0;
		}
		int     x;
		int     y;
		int     cx;
		int     cy;
	} OEMBITMAPINFO, *POEMBITMAPINFO;

	/*
	 * Menu data to be passed to xxxRealDrawMenuItem from xxxDrawState
	 */
	typedef struct
	{
		PMENU pMenu;
		PITEM pItem;
	} GRAYMENU;
	typedef GRAYMENU *PGRAYMENU;

	//BlColor flags
#define BC_INVERT             0x00000001
#define BC_NOMIRROR           0x00000002


	typedef struct _TL
	{
		int iBogus;
	} TL;

	typedef struct _TL *PTL;
#define CheckLock(pobj)
#define Lock(ppobj, pobj) (*ppobj = pobj)
#define Unlock(ppobj) (*ppobj = NULL)
#define RIPERR0(a,b,c)
#define RIP_WARNING
#define UserAssert _ASSERT

#define IDSYS_MNSHOW 0x0000FFFEL
#define MENUCLASS		_T("IUIMENUCLASS")

#define SYSMET(CXSMSIZE)	4
#define CXMENU3DEDGE 1
#define CYMENU3DEDGE 1
#define OBI_MENUARROWUP     65
#define OBI_MENUARROWDOWN   68


	// 未找到定义，自已定义
#define MFS_HOTTRACKDRAWN	0x00000001

#ifndef MFS_HOTTRACK
#define MFS_HOTTRACK        0x00000100L
#endif // !MFS_HOTTRACK
#define DFCS_MENUARROWUP        0x0008                      // ;Internal 4.1
#define DFCS_MENUARROWDOWN      0x0010                      // ;Internal 4.1
#define DF_3DFACE           (COLOR_3DFACE << 3)

	extern HBRUSH g_hbr3DSHADOW;
	extern HBRUSH g_hbr3DHILIGHT;
	extern HBRUSH g_hbrMenu;
	extern HBRUSH g_hbrHIGHLIGHTTEXT;


#define threeDSHADOW	1
#define threeDHILIGHT	2
#define MENUHBR			3
#define HIGHLIGHTTEXT	4
	HBRUSH SYSHBR(int nIndex);

#define TestMF(pmenu, flag) ((pmenu)->fFlags & (flag))
#define RIPMSG2(a, b, c, d)
#define ThreadLockAlways(a,b)
#define ThreadUnlock(a)
#define ThreadLock(a, b)
#define DesktopFree(pdesk, p)   HeapFree(pdesk->pheapDesktop, p)
#define UNDERLINE_RECALC    0x7FFFFFFF      // MAXINT; tells us to recalc underline position
#define SYSMET(CYFIXEDFRAME) 4
#define SYSMET(CYFIXEDFRAME) 4
#define IDSYS_MNHIDE        0x0000FFFFL
#define MNF_DONTSKIPSEPARATORS      0x0001
#define TextPointer(h) ((LPWSTR)h)
#define CH_HELPPREFIX   0x08
#define OBI_MENUCHECK       63      // menu check mark

	extern int gcyMenuScrollArrow;

	/*
	 * Menu commands
	 */
	//#define MENUBIT             (0x8000)
	//#define MENUUP              (0x8000 | VK_UP)
	//#define MENUDOWN            (0x8000 | VK_DOWN)
	//#define MENULEFT            (0x8000 | VK_LEFT)
	//#define MENURIGHT           (0x8000 | VK_RIGHT)
	//#define MENUEXECUTE         TEXT('\r')      /* Return character */
#define MENUSYSMENU         TEXT(' ')       /* Space character */
#define MENUCHILDSYSMENU    TEXT('-')       /* Hyphen */

#define MF_ALLSTATE         0x00FF
#define MF_MAINMENU         0xFFFF
#define MFMWFP_OFFMENU      0
#define MFMWFP_MAINMENU     0x0000FFFF
#define MFMWFP_NOITEM       0xFFFFFFFF
#define MFMWFP_UPARROW      0xFFFFFFFD  /* Warning: Also used to define IDSYS_MNUP */
#define MFMWFP_DOWNARROW    0xFFFFFFFC  /* Warning: Also used to define IDSYS_MNDOWN */
#define MFMWFP_MINVALID     0xFFFFFFFC
#define MFMWFP_ALTMENU      0xFFFFFFFB
#define MFMWFP_FIRSTITEM    0

	/*
	 * NOTE: PUDF_ANIMATE must have the same value as MINMAX_ANIMATE.
	 */
#define PUDF_ANIMATE                    0x00010000

#define ACCF_KEYBOARDPREF               0x00020000
#define ACCF_SCREENREADER               0x00040000
#define PUDF_BEEP                       0x00080000  /* Warning beeps allowed?                   */
#define PUDF_EXTENDEDSOUNDS             0x00100000  /* Extended sounds enabling                 */
#define PUDF_DRAGFULLWINDOWS            0x00200000  /* Drag xor rect or full windows            */
#define PUDF_ICONTITLEWRAP              0x00400000  /* Wrap icon titles or just use single line */
#define PUDF_FONTSARELOADED             0x00800000
#define PUDF_POPUPINUSE                 0x01000000
#define PUDF_MENUSTATEINUSE             0x02000000
#define PUDF_VDMBOUNDSACTIVE            0x04000000
#define PUDF_ALLOWFOREGROUNDACTIVATE    0x08000000
#define PUDF_DRAGGINGFULLWINDOW         0x10000000
#define PUDF_LOCKFULLSCREEN             0x20000000
#define PUDF_GSMWPINUSE                 0x40000000

	extern DWORD gdwPUDFlags;
#define TEST_FLAG(field, flag) ((field) & (flag))
#define TEST_PUDF(f) TEST_FLAG(gdwPUDFlags, f)
#define SET_FLAG(field, flag) ((field) |= (flag))
#define SET_PUDF(f) SET_FLAG(gdwPUDFlags, f)
	extern POPUPMENU gpopupMenu;
#define Validateppopupmenu(ppopupmenu)
#define CLEAR_FLAG(field, flag) ((field) &= ~(flag))
#define CLEAR_PUDF(f) CLEAR_FLAG(gdwPUDFlags, f)
#define RIPMSG0(a, b)
#define DBGIncModalMenuCount()

	/*
	 * Defines for Menu focus
	 */
#define FREEHOLD    0
#define MOUSEHOLD  -1 /* Mouse button held down and dragging */
#define KEYBDHOLD   1

#define MN_SETHMENU            0x01E0
#define MN_GETHMENU            0x01E1
#define MN_SIZEWINDOW          0x01E2
#define MN_OPENHIERARCHY       0x01E3
#define MN_CLOSEHIERARCHY      0x01E4
#define MN_SELECTITEM          0x01E5
#define MN_CANCELMENUS         0x01E6
#define MN_SELECTFIRSTVALIDITEM    0x01E7
#define MN_GETPPOPUPMENU       0x01EA
#define MN_FINDMENUWINDOWFROMPOINT 0x01EB
#define MN_SHOWPOPUPWINDOW     0x01EC
#define MN_BUTTONDOWN          0x01ED
#define MN_MOUSEMOVE           0x01EE
#define MN_BUTTONUP            0x01EF
#define MN_SETTIMERTOOPENHIERARCHY 0x01F0

#define LINP_KEYBOARD       0x00000001
#define LINP_SCREENSAVER    0x00000002
#define LINP_LOWPOWER       0x00000004
#define LINP_POWEROFF       0x00000008
#define LINP_POWERTIMEOUTS  (LINP_LOWPOWER | LINP_POWEROFF)
#define LINP_INPUTTIMEOUTS  (LINP_SCREENSAVER | LINP_LOWPOWER | LINP_POWEROFF)

	/*
	 * Internal menu flags stored in pMenu->fFlags.
	 * High order bits are used for public MNS_ flags defined in winuser.w
	 */
#define MFISPOPUP               0x00000001
#define MFMULTIROW              0x00000002
#define MFUNDERLINE             0x00000004
#define MFWINDOWDC              0x00000008  /* Window DC vs Client area DC when drawing*/
#define MFINACTIVE              0x00000010
#define MFRTL                   0x00000020
#define MFDESKTOP               0x00000040 /* Set on the desktop menu AND its submenus */
#define MFSYSMENU               0x00000080 /* Set on desktop menu but NOT on its submenus */
#define MFAPPSYSMENU            0x00000100 /* Set on (sub)menu we return to the app via GetSystemMenu */
#define MFLAST                  0x00000100

	/*
	 * NOTE: SMF() can only be used on single bit flags (NOT MRGFDISABLED!).
	 */
#define SetMF(pmenu, flag)    ((pmenu)->fFlags |=  (flag))
#define ClearMF(pmenu, flag)  ((pmenu)->fFlags &= ~(flag))
#define TestMF(pmenu, flag)   ((pmenu)->fFlags &   (flag))

#define SetMFS(pitem, flag)   ((pitem)->fState |=  (flag))
#define TestMFS(pitem, flag)  ((pitem)->fState &   (flag))
#define ClearMFS(pitem, flag) ((pitem)->fState &= ~(flag))

#define SetMFT(pitem, flag)   ((pitem)->fType |=  (flag))
#define TestMFT(pitem, flag)  ((pitem)->fType &   (flag))
#define ClearMFT(pitem, flag) ((pitem)->fType &= ~(flag))

	// 未找到TPM_FIRSTANIBITPOS的定义
#define TPM_FIRSTANIBITPOS 1
#define MN_DBLCLK                  0x01F1
#define MN_DODRAGDROP              0x01F3
#define DSS_DEFAULT			1

	/* Undocumented(?) window messages
	 */
#define WM_SETVISIBLE       0x0009
#define WM_ALTTABACTIVE     0x0029
#define WM_ISACTIVEICON     0x0035
#define WM_QUERYPARKICON    0x0036
#define WM_SYNCPAINT        0x0088
#define WM_SYSTIMER     0x0118
#define WM_LBTRACKPOINT     0x0131
#define WM_ENTERMENULOOP    0x0211
#define WM_EXITMENULOOP     0x0212
#define WM_NEXTMENU     0x0213
#define WM_DROPOBJECT       0x022A
#define WM_QUERYDROPOBJECT  0x022B
#define WM_BEGINDRAG        0x022C
#define WM_DRAGLOOP     0x022D
#define WM_DRAGSELECT       0x022E
#define WM_DRAGMOVE     0x022F
#define WM_ENTERSIZEMOVE    0x0231
#define WM_EXITSIZEMOVE     0x0232

	/*
	 * Defines for WinOldAppHackoMatic flags which win386 oldapp can send to us.
	 * These are kept in user's global variable winOldAppHackoMaticFlags
	 */
#define WOAHACK_CHECKALTKEYSTATE 1
#define WOAHACK_IGNOREALTKEYDOWN 2

	extern UINT gwinOldAppHackoMaticFlags;

	/*
	 * Animation flags (pMenuState->iAniDropDir)
	 */
#define PAS_RIGHT       (TPM_HORPOSANIMATION >> TPM_FIRSTANIBITPOS)
#define PAS_LEFT        (TPM_HORNEGANIMATION >> TPM_FIRSTANIBITPOS)
#define PAS_DOWN        (TPM_VERPOSANIMATION >> TPM_FIRSTANIBITPOS)
#define PAS_UP          (TPM_VERNEGANIMATION >> TPM_FIRSTANIBITPOS)
#define PAS_OUT         0x10
#define PAS_HORZ        (PAS_LEFT | PAS_RIGHT)
#define PAS_VERT        (PAS_UP | PAS_DOWN)

	/*
	 * xxxMNUpdateShownMenu flags
	 */
#define MNUS_DEFAULT      0x00000001
#define MNUS_DELETE       0x00000002
#define MNUS_DRAWFRAME    0x00000004

	/*
	 * MN_SIZEWINDOW wParam flag. xxxMNUpdateShownMenu sends this
	 *  message, so keep MNSW_ and MNUS_ in sync.
	 */
#define MNSW_RETURNSIZE  0
#define MNSW_SIZE        MNUS_DEFAULT
#define MNSW_DRAWFRAME   MNUS_DRAWFRAME

#define MFT_OLDAPI_MASK     0x00004B64L
#define MFS_OLDAPI_MASK     0x0000008BL
#define MFT_NONSTRING       0x00000904L
#define MFT_BREAK           0x00000060L

#define MNS_VALID       0x00000001
#define MFNOCHECK       0x08
#define WM_FINALDESTROY    0x0070
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK
#define HRGN_FULL           ((HRGN)1)
#define IDSYS_MNANIMATE     0x0000FFFBL
#define IDSYS_MNUP          MFMWFP_UPARROW   /* 0xFFFFFFFD */
#define IDSYS_MNDOWN        MFMWFP_DOWNARROW /* 0xFFFFFFFC */
#define FNID_MENU 0x0000029C

#define WNDCLASS_TRAYNOTIFY     "Shell_TrayWnd"
#define DMI_INVERT 0x00000001

#define COPY_FLAG(fieldDst, fieldSrc, flag)     ((fieldDst) ^= ((fieldDst) ^ (fieldSrc)) & (flag))

#define RECT_ONLEFT     0
#define RECT_ONTOP      1
#define RECT_ONRIGHT    2
#define RECT_ONBOTTOM   3
#define RECT_ORG        4


#define MN_ENDMENU			WM_USER + 10000
#define MN_ACTIVATEPOPUP	WM_USER + 10001

#define REBASEALWAYS(p, elem) ((PVOID)(((PBYTE)(p) + ((PBYTE)(p)->elem - (p)->head.pSelf))))
#define REBASEPTR(obj, p) ((PVOID)((PBYTE)(p) - ((PBYTE)(obj)->head.pSelf - (PBYTE)(obj))))

#define DCX_WINDOW          0x00000001L
#define DCX_CACHE           0x00000002L
#define DCX_CLIPCHILDREN    0x00000008L
#define DCX_CLIPSIBLINGS    0x00000010L
#define DCX_PARENTCLIP      0x00000020L
#define DCX_EXCLUDERGN      0x00000040L
#define DCX_INTERSECTRGN    0x00000080L
#define DCX_LOCKWINDOWUPDATE 0x00000400L
#define DCX_USESTYLE        0x00010000L

	/*
	 * Scroll menu arrow flags
	 */
#define MSA_OFF         0
#define MSA_ON          1
#define MSA_ATTOP       2
#define MSA_ATBOTTOM    3

#define DTAG_CLASS              0x0001
#define DTAG_DESKTOPINFO        0x0002
#define DTAG_CLIENTTHREADINFO   0x0003
#define DTAG_TEXT               0x0004
#define DTAG_HANDTABL           0x0005
#define DTAG_SBINFO             0x0006
#define DTAG_MENUITEM           0x0007
#define DTAG_MENUTEXT           0x0008
#define DTAG_IMETEXT            0x0009
#define DTAG_PROPLIST           0x000A

#define MFT_MASK            0x4B64
#define MFS_MASK            0x108B

#define MNXBORDER (SYSMET(CXBORDER) + SYSMET(CXEDGE))
#define MNYBORDER (SYSMET(CYBORDER) + SYSMET(CYEDGE))
#define MNXSPACE  (SYSMET(CXEDGE))
#define MNLEFTMARGIN (SYSMET(CXEDGE))

	/* This tells xxxMNItemSize that the bitamp size is not avilable */
#define MNIS_MEASUREBMP -1

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define IDSYS_MNAUTODISMISS 0x0000FFF9L

	/*
	 * Sprite and Fade related functions and defines.
	 */
#define FADE_SHOW           0x00000001
#define FADE_COMPLETED      0x00000002
#define FADE_SHOWN          0x00000004
#define FADE_WINDOW         0x00000008
#define FADE_MENU           0x00000010
#define FADE_TOOLTIP        0x00000020

#define CMS_QANIMATION          165

#if DBG
#define MNGetpItemIndex DBGMNGetpItemIndex
	UINT DBGMNGetpItemIndex(PMENU pmenu, PITEM pitem);
#else // DBG
#define MNGetpItemIndex _MNGetpItemIndex
#endif // DBG
	__inline UINT _MNGetpItemIndex(PMENU pmenu, PITEM pitem)
	{
		return (UINT)(((ULONG_PTR)pitem - (ULONG_PTR)pmenu->rgItems) / sizeof(ITEM));
	}

	//typedef struct THREADINFO
	//{
	//	PMENUSTATE pMenuState;
	//} PTHREADINFO;
	/*
	 * Menu macros
	 */

	typedef struct tagLASTINPUT    /* linp */
	{
		DWORD timeLastInputMessage;
		DWORD dwFlags;
		PTHREADINFO ptiLastWoken;  /* Last thread woken by key or click  */
		/* It can be NULL                     */
		POINT ptLastClick;         /* point of the last mouse click      */
	} LASTINPUT, PLASTINPUT;

	extern LASTINPUT glinp;
	extern UINT guSFWLockCount;
	__inline void IncSFWLockCount()
	{
		guSFWLockCount++;
	}

	__inline BOOL IsRootPopupMenu(PPOPUPMENU ppopupmenu)
	{
		return (ppopupmenu == ppopupmenu->ppopupmenuRoot);
	}
	__inline BOOL ExitMenuLoop(PMENUSTATE pMenuState, PPOPUPMENU ppopupmenu)
	{
		return (!pMenuState->fInsideMenuLoop || ppopupmenu->fDestroyed);
	}

	extern PTHREADINFO ptiCurrent;
	__inline PMENUSTATE GetpMenuState(HWND pwnd)
	{
		// 得到pwnd所在线程
		return ptiCurrent->pMenuState;
	}
	__inline PPOPUPMENU GetpGlobalPopupMenu(HWND pwnd)
	{
		return (GetpMenuState(pwnd) ? GetpMenuState(pwnd)->pGlobalPopupMenu : NULL);
	}
	__inline BOOL IsInsideMenuLoop(PTHREADINFO pti)
	{
		return FALSE;//((pti->pMenuState != NULL) && pti->pMenuState->fInsideMenuLoop);
	}
	__inline BOOL IsMenuStarted(PTHREADINFO pti)
	{
		return TRUE;//((pti->pMenuState != NULL) && pti->pMenuState->fMenuStarted);
	}
	__inline PITEM MNGetToppItem(PMENU pMenu)
	{
		return pMenu->rgItems + pMenu->iTop;
	}
	__inline BOOL MNIsItemSelected(PPOPUPMENU ppopupmenu)
	{
		return ((int)ppopupmenu->posSelectedItem >= 0);
	}
	__inline PITEM MNGetSelectedpitem(PPOPUPMENU ppopupmenu)
	{
		return ppopupmenu->spmenu->rgItems + ppopupmenu->posSelectedItem;
	}
	__inline BOOL MNIsScrollArrowSelected(PPOPUPMENU ppopupmenu)
	{
		return ((ppopupmenu->posSelectedItem == MFMWFP_UPARROW)
				|| (ppopupmenu->posSelectedItem == MFMWFP_DOWNARROW));
	}
	__inline BOOL IsModelessMenuNotificationWindow(HWND pwnd)
	{
		PMENUSTATE pMenuState;
		return (((pMenuState = GetpMenuState(pwnd)) != NULL)
				&& pMenuState->fModelessMenu
				&& (pMenuState->pGlobalPopupMenu->spwndNotify == pwnd));
	}
	__inline BOOL IsRecursedMenuState(PMENUSTATE pMenuState, PPOPUPMENU ppopupmenu)
	{
		return (pMenuState->pGlobalPopupMenu != ppopupmenu->ppopupmenuRoot);
	}
	__inline void LockMenuState(PMENUSTATE pMenuState)
	{
		(pMenuState->dwLockCount)++;
	}

	__inline DWORD TestFadeFlags(DWORD dwFlags)
	{
		return (gfade.dwFlags & dwFlags);
	}

	BOOL xxxUnlockMenuState(PMENUSTATE pMenuState);

	PVOID LockPopupMenu(PPOPUPMENU ppopup, PMENU *pspmenu, PMENU pmenu);

	VOID MNFreePopup(
		PPOPUPMENU ppopupmenu);

	void xxxMNDismiss(PMENUSTATE pMenuState);
	PITEM MNGetpItem(PPOPUPMENU ppopup, UINT uIndex);
	void xxxMNSetCapture(PPOPUPMENU ppopup);
	void xxxMNReleaseCapture(void);
	void MNCheckButtonDownState(PMENUSTATE pMenuState);
	HWND GetMenuStateWindow(PMENUSTATE pMenuState);
	PVOID LockPopupMenu(PPOPUPMENU ppopup, PMENU *pspmenu, PMENU pmenu);
	PVOID UnlockPopupMenu(PPOPUPMENU ppopup, PMENU *pspmenu);
	PVOID LockWndMenu(HWND pwnd, PMENU *pspmenu, PMENU pmenu);
	PVOID UnlockWndMenu(HWND pwnd, PMENU *pspmenu);
	UINT MNSetTimerToCloseHierarchy(PPOPUPMENU ppopup);
	BOOL xxxMNSetTop(PPOPUPMENU ppopup, int iNewTop);
	LRESULT WINAPI xxxMenuWindowProc(HWND, UINT, WPARAM, LPARAM);
	VOID xxxMNButtonUp(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT posItemHit, LPARAM lParam);
	VOID xxxMNButtonDown(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT posItemHit, BOOL fClick);
	PITEM xxxMNSelectItem(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT itemPos);
	BOOL xxxMNSwitchToAlternateMenu(PPOPUPMENU ppopupMenu);
	void xxxMNCancel(PMENUSTATE pMenuState, UINT uMsg, UINT cmd, LPARAM lParam);
	VOID xxxMNKeyDown(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT key);
	BOOL xxxMNDoubleClick(PMENUSTATE pMenuState, PPOPUPMENU ppopup, int idxItem);
	VOID xxxMNCloseHierarchy(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState);
	HWND xxxMNOpenHierarchy(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState);
	void LockMFMWFPWindow(PULONG_PTR puHitArea, ULONG_PTR uNewHitArea);
	void UnlockMFMWFPWindow(PULONG_PTR puHitArea);
	BOOL IsMFMWFPWindow(ULONG_PTR uHitArea);
	LONG_PTR xxxMNFindWindowFromPoint(PPOPUPMENU ppopupMenu, PUINT pIndex, POINTS screenPt);
	VOID xxxMNMouseMove(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, POINTS screenPt);
	int xxxMNCompute(PMENU pMenu, HWND pwndNotify, DWORD yMenuTop,
		DWORD xMenuLeft, DWORD cxMax, LPDWORD lpdwHeight);
	VOID xxxMNRecomputeBarIfNeeded(HWND pwndNotify, PMENU pMenu);
	VOID xxxMenuDraw(HDC hdc, PMENU pMenu);
	UINT  MNFindNextValidItem(PMENU pMenu, int i, int dir, UINT flags);
	VOID MNFreeItem(PMENU pMenu, PITEM pItem, BOOL fFreeItemPopup);
	BOOL   xxxMNStartMenu(PPOPUPMENU ppopupMenu, int mn);
	VOID MNPositionSysMenu(HWND pwnd, PMENU pSysMenu);

	PITEM xxxMNInvertItem(PPOPUPMENU ppopupmenu, PMENU pMenu, int itemNumber, HWND pwndNotify, BOOL fOn);

	VOID   xxxSendMenuSelect(HWND pwndNotify, HWND pwndMenu, PMENU pMenu, int idx);
#define SMS_NOMENU      (PMENU)(-1)


	BOOL   xxxSetSystemMenu(HWND pwnd, PMENU pMenu);
	BOOL   xxxSetDialogSystemMenu(HWND pwnd);

	VOID xxxMNChar(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT character);
	PPOPUPMENU MNAllocPopup(BOOL fForceAlloc);
	VOID MNFreePopup(PPOPUPMENU ppopupmenu);

	/*
	 * Menu entry points used by the rest of USER
	 */
	VOID xxxMNKeyFilter(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, UINT ch);
	int  xxxMenuBarCompute(PMENU pMenu, HWND pwndNotify, DWORD yMenuTop,
		DWORD xMenuLeft, int cxMax);
	VOID xxxEndMenu(PMENUSTATE pMenuState);
	BOOL xxxCallHandleMenuMessages(PMENUSTATE pMenuState, HWND pwnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL xxxHandleMenuMessages(LPMSG lpmsg, PMENUSTATE pMenuState, PPOPUPMENU ppopupmenu);
	void xxxEndMenuLoop(PMENUSTATE pMenuState, PPOPUPMENU ppopupmenu);
	int  xxxMNLoop(PPOPUPMENU ppopupMenu, PMENUSTATE pMenuState, LPARAM lParam, BOOL fDblClk);
	VOID xxxSetSysMenu(HWND pwnd);
	PMENU xxxGetSysMenuHandle(HWND pwnd);
	PMENU xxxGetSysMenu(HWND pwnd, BOOL fSubMenu);
	PMENU MakeMenuRtoL(PMENU pMenu, BOOL bRtoL);
	HDC CreateCompatiblePublicDC(HDC hdc, HBITMAP *pbmDCGray);
	void xxxPSMTextOut(HDC hdc, int xLeft, int yTop, LPWSTR lpsz, int cch, DWORD dwFlags);
	BOOL xxxPSMGetTextExtent(HDC hdc, LPWSTR lpstr, int cch, PSIZE psize);
	PMENUSTATE xxxMNAllocMenuState(PTHREADINFO ptiCurrent, PTHREADINFO ptiNotify, PPOPUPMENU ppopupmenuRoot);
	/*
	 * Menu Drag and Drop
	 */
	NTSTATUS xxxClientRegisterDragDrop(HWND hwnd);
	NTSTATUS xxxClientRevokeDragDrop(HWND hwnd);
	NTSTATUS xxxClientLoadOLE(VOID);
	void xxxMNSetGapState(ULONG_PTR uHitArea, UINT uIndex, UINT uFlags, BOOL fSet);
	//BOOL xxxMNDragOver(POINT *ppt, PMNDRAGOVERINFO pmndoi);
	BOOL xxxMNDragLeave(VOID);
	void xxxMNUpdateDraggingInfo(PMENUSTATE pMenuState, ULONG_PTR uHitArea, UINT uIndex);

	LONG
	FindBestPos(
		int         x,
		int         y,
		int         cx,
		int         cy,
		LPRECT      prcExclude,
		UINT        wFlags,
		PPOPUPMENU  ppopupmenu,
		HMONITOR    pMonitor);

	void xxxMNEndMenuState(BOOL fFreePopup);

	void MNFlushDestroyedPopups(PPOPUPMENU ppopupmenu, BOOL fUnlock);

	PITEM MNLookUpItem(
		PMENU pMenu,
		UINT wCmd,
		BOOL fByPosition,
		PMENU *ppMenuItemIsOn);

	PPOPUPMENU MNGetPopupFromMenu(PMENU pMenu, PMENUSTATE *ppMenuState);
	void xxxMNUpdateShownMenu(PPOPUPMENU ppopup, PITEM pItem, UINT uFlags);
	UINT MNFindItemInColumn(
		PMENU pMenu,
		UINT idxB,
		int dir,
		BOOL fRoot);

	BOOL IsTrayWindow(HWND hwnd);
	BOOL xxxSendMinRectMessages(HWND pwnd, RECT *lpRect);

	void CopyOffsetRect(LPRECT out, LPRECT in, int x, int y);
	DWORD _GetMenuDefaultItem(PMENU pMenu, BOOL fByPosition, UINT uFlags);
	BOOL CopyInflateRect(
		LPRECT prcDst,
		CONST RECT *prcSrc,
		int cx, int cy);

	BOOL
	TryRect(
		UINT        wRect,
		int         x,
		int         y,
		int         cx,
		int         cy,
		LPRECT      prcExclude,
		LPPOINT     ppt,
		HMONITOR    pMonitor);

	BOOL xxxMNItemSize(
		PMENU pMenu,
		HWND pwndNotify,
		HDC hdc,
		PITEM pItem,
		BOOL fPopup,
		LPPOINT lppt);

	BOOL xxxInsertMenuItem(
		PMENU pMenu,
		UINT wIndex,
		BOOL fByPosition,
		LPMENUITEMINFOW lpmii,
		LPCWSTR pstrItem);

	BOOL xxxSetMenuItemInfo(
		PMENU pMenu,
		UINT wIndex,
		BOOL fByPosition,
		LPMENUITEMINFOW lpmii,
		LPCWSTR pstrItem);

	PMENU _CreateMenu();
	BOOL _DestroyMenu(
		PMENU pMenu);

	void MNDestroyAnimationBitmap(PMENUSTATE pMenuState);

	BOOL DrawFrame(
		HDC   hdc,
		PRECT prc,
		int   clFrame,
		int   cmd);

	void StopFade(void);

	DWORD FindCharPosition(
		LPWSTR lpString,
		WCHAR ch);

	HRGN
	CreateEmptyRgn(void);

	void xxxDrawMenuItem(
		HDC hdc,
		PMENU pMenu,
		PITEM pItem,
		DWORD dwFlags);

	BOOL CALLBACK xxxRealDrawMenuItem(
		HDC hdc,
		PGRAYMENU pGray,
		int cx,
		int cy);

	int SetPopupMenu(HWND hMenuWnd, PPOPUPMENU pPopupMenu);
	PPOPUPMENU GetPopupMenu(HWND hMenuWnd);
#ifdef _DEBUG
	extern HWND g_pwndHierarchy;
#endif

} // namespace IUI

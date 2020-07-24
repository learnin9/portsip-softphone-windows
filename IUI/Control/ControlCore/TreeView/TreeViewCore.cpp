#include "StdAfx.h"
#include "../Win32Global.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

#define TREE_MININDENT 5		// 缩进的最小值
#define TREE_INDENT    3
#define TREE_HORZLINE  5

// flags for TV_DrawItem
#define TVDI_NOIMAGE    0x0001  // don't draw image
#define TVDI_NOTREE     0x0002  // don't draw indent, lines, +/-
#define TVDI_TRANSTEXT  0x0004  // draw text transparently in black
#define TVDI_ERASE      0x0008  // erase while drawing
#define TVDI_GRAYTEXT   0x0010  // text is gray (disabled item)
#define TVDI_GRAYCTL    0x0020  // text and background is gray (disabled control)
#define TVDI_FORCEIMAGE 0x0040  // Always draw image
#define TVDI_NOBK       0x0080

// Internal flags for TV_SelectItem
#define TVC_INTERNAL   0x1000

typedef struct _TREE
{
	_TREE()
		: m_hWnd(NULL)
		, m_bHorz(FALSE)
		, m_bVert(FALSE)
		, m_bFocus(FALSE)
		, m_bNameEditPending(FALSE)
		, m_bRedraw(FALSE)
		, m_bScrollWait(FALSE)
		, m_bCreatedFont(FALSE)
		, m_bNoDismissEdit(FALSE)
		, m_bIndentSet(FALSE)
		, m_bTrackSet(FALSE)
		, m_bPlaceTooltip(FALSE)
		, m_bCyItemSet(FALSE)
		, m_bInsertAfter(FALSE)
		, m_bRestoreOldDrop(FALSE)
		, m_bSmoothVScroll(FALSE)
		, m_bOverflowScrollBar(FALSE)
		, m_hRootItem(NULL)
		, m_hCaretItem(NULL)
		, m_hDropTargetItem(NULL)
		, m_hOldDropItem(NULL)
		, m_hEditingItem(NULL)
		, m_hHotItem(NULL)
		, m_hToolTipItem(NULL)
		, m_hInsertItem(NULL)
		, m_hSearchItem(NULL)
		, m_hDragItem(NULL)
		, m_hDpaWatch(NULL)
		, m_hImageList(NULL)
		, m_hImlState(NULL)
		, m_hHotItemCursor(NULL)
		, m_nPuntChar(0)
		, m_cxState(0)
		, m_cyState(0)
		, uDBCSChar(0)
		, hbrBk(NULL)
		, hFont(NULL)
		, hFontHot(NULL)
		, hFontBold(NULL)
		, hFontBoldHot(NULL)
		, hStartBmp(NULL)
		, hBmp(NULL)
		, hdcBits(NULL)
		, hItemPainting(NULL)
		, hheap(NULL)
		, ptCapture()
		, clrText(0)
		, clrBk(0)
		, clrim(0)
		, clrLine(0)
		, cxImage(0)
		, cyImage(0)
		, cyText(0)
		, cyItem(0)
		, cxBorder(0)
		, cyBorder(0)
		, cxIndent(0)
		, cxWnd(0)
		, cyWnd(0)
		, cxMax(0)
		, cFullVisible(0)
		, xPos(0)
		, cShowing(0)
		, cItems(0)
		, hTop(0)
		, uMaxScrollTime(0)
		, wSmoothVScroll(0)
		, hwndEdit(0)
		, pfnEditWndProc(NULL)
		, hwndToolTips(NULL)
		, pszTip(NULL)
#ifdef UNICODE
		, pszTipA(NULL)
#endif
		, m_bEnableItemLine(FALSE)
		, m_crItemLine(RGB(128, 128, 128))
		, m_himgCombineCollapseExpanded(NULL)
		, m_eCollapseExpandedImageResizeMode(IRM_CENTER)
		, m_crFocusItemText(IUIGetGloabelVar()->g_clrHighlightText)
		, m_eFocusItemBkType(BKT_COLOR)
		, m_crFocusItemBK(IUIGetGloabelVar()->g_clrHighlight)
		, m_himgFocusItemBk(NULL)
		, m_crSelectedItemText(RGB(255, 255, 255))
		, m_eHighlightItemBkType(BKT_COLOR)
		, m_eSelectedItemBkType(BKT_COLOR)
		, m_crSelectedItemBK(RGB(0, 0, 128))
		, m_bSelectedItemBkCombine(TRUE)
		, m_himgSelectedItemCombineBk(NULL)
		, m_crHighlightItemBK(RGB(241, 241, 241))
		, m_crHighlightItemText(RGB(255, 255, 255))
		, m_bHighlightItemBkCombine(TRUE)
		, m_himgHighlightItemCombineBk(NULL)
		, m_crDisabledItemText(RGB(128, 128, 128))
		, m_dwHoverTime(HOVER_DEFAULT)
		, m_bHoverRepeat(FALSE)
		, m_nMouseWheelStep(20)
	{
		int i = 0;
		for (i = 0; i < COMBINEIMAGESIZE2; ++i)
		{
			m_himgBk[i] = NULL;
		}

		for (i = 0; i < 2; ++i)
		{
			m_himgSelectedItemBk[i] = NULL;
			m_himgHighlightItemBk[i] = NULL;
		}

		for (i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			m_eTextHorAlignMode[i] = TAH_LEFT;
			m_eTextVerAlignMode[i] = TAV_CENTER;
			m_himgCollapse[i] = NULL;
			m_himgExpanded[i] = NULL;
		}
	}

	virtual ~_TREE()
	{
		Release();
	}

	int Release()
	{
		m_hWnd->ci.Release();

		int i = 0;
		for (i = 0; i < COMBINEIMAGESIZE2; ++i)
		{
			ReleaseIUIImage(m_himgBk[i]);
			m_himgBk[i] = NULL;
		}

		for (i = 0; i < COMBINEIMAGESIZE4; ++i)
		{
			ReleaseIUIImage(m_himgCollapse[i]);
			m_himgCollapse[i] = NULL;
			ReleaseIUIImage(m_himgExpanded[i]);
			m_himgExpanded[i] = NULL;
		}

		ReleaseIUIImage(m_himgFocusItemBk);
		m_himgFocusItemBk = NULL;

		ReleaseIUIImage(m_himgSelectedItemCombineBk);
		m_himgSelectedItemCombineBk = NULL;
		ReleaseIUIImage(m_himgHighlightItemCombineBk);
		m_himgHighlightItemCombineBk = NULL;
		for (i = 0; i < 2; ++i)
		{
			ReleaseIUIImage(m_himgSelectedItemBk[i]);
			m_himgSelectedItemBk[i] = NULL;
			ReleaseIUIImage(m_himgHighlightItemBk[i]);
			m_himgHighlightItemBk[i] = NULL;
		}

		return 0;
	}

	HWLWND m_hWnd;	// HWLWND里包含IUI_CONTROL_INFO

					// Flags
	BITBOOL        m_bHorz : 1;       // 当前是否有水平滚动条
	BITBOOL        m_bVert : 1;
	BITBOOL        m_bFocus : 1;      // 当前是否拥有焦点
	BITBOOL        m_bNameEditPending : 1; // Label编辑是否被挂起?
	BITBOOL        m_bRedraw : 1;
	BITBOOL        m_bScrollWait : 1; // are we waiting for a dblclk to not scroll?
	BITBOOL        m_bCreatedFont : 1; // 自己创建字体
	BITBOOL        m_bNoDismissEdit : 1; // 不解除in-place编辑框
	BITBOOL        m_bIndentSet : 1;   // is the parent managing the indent size?
	BITBOOL        m_bTrackSet : 1;   // 控制TrackMouseEvent
	BITBOOL        m_bPlaceTooltip : 1; // 是否在文本上放置Tooltip
	BITBOOL        m_bCyItemSet : 1;   // 是否父项设置了Item高度
	BITBOOL        m_bInsertAfter : 1; // 在m_hInsertItem之后，而不是之前插入insert mark（插入标记）
	BITBOOL        m_bRestoreOldDrop : 1; // 需要将m_hOldDropItem还原到m_hDropTargetItem

	// IUI Add
	// 基于像素而不是Item的平滑垂直滚动。由于平滑滚动模式时的Item高度与常规滚动模式
	// 的Item高度无法运行时换算，故平滑滚动模式只能在TreeView创建之前设置
	BITBOOL	m_bSmoothVScroll : 1;

	// 是否使用悬浮滚动条（鼠标位于Tree内显示，否则不显示）
	BITBOOL m_bOverflowScrollBar : 1;

	// m_hRootItem是内置的虚拟根节点，即使没有往Tree中插入任何数据，m_hRootItem也是有效的
	// 但它并不等于GetRootItem，GetRootItem返回的实际上m_hRootItem->hKids.
	HTREEITEM   m_hRootItem;
	HTREEITEM   m_hCaretItem;         // 当前焦点Item
	HTREEITEM   m_hDropTargetItem;    // Drop目标Item
	HTREEITEM   m_hOldDropItem;       // 旧的Drop目标Item

	// 正在被编辑的Item。 这时，会创建一个Edit控件来输入它的新内容
	HTREEITEM   m_hEditingItem;

	// Highlight Item
	HTREEITEM m_hHotItem;

	HTREEITEM   m_hToolTipItem;       // the current item set in tooltips
	HTREEITEM   m_hInsertItem;      // item that is relative to the insert mark
	HTREEITEM   m_hSearchItem;      // item active in most recent incremental search
	HTREEITEM   m_hDragItem;        // item that's being dragged.
	HDPA        m_hDpaWatch;      // array of PTVWATCHEDITEMs - items being watched
	HIMAGELIST  m_hImageList;     // image list
	HIMAGELIST  m_hImlState;      // state image list

								// Highlight Item的光标形状
	HCURSOR m_hHotItemCursor;

	int         m_nPuntChar;      // number of wm_char's to punt
	int         m_cxState;
	int         m_cyState;

	UINT        uDBCSChar;      // DBCS character for incremental search

								// 主要用于绘制缩进区域的图案
	HBRUSH hbrBk;

	HFONT       hFont;          // tree font
	HFONT       hFontHot;       // underlined for hot tracking
	HFONT       hFontBold;      // bold tree font
	HFONT       hFontBoldHot;       // underlined for hot tracking
	HBITMAP     hStartBmp;      // initial DC mono bitmap

								// 选入hdcBits中的位图，参见hdcBits
								// 保存了绘制TreeView缩进区域所有+-号、连接线的图案
								// 从左向右等间距（间距为TREE::cxIndent）排列。
	HBITMAP hBmp;

	// 保存了绘制TreeView缩进区域所有+-号、连接线的图案
	// 从左向右等间距（间距为TREE::cxIndent）排列。
	HDC hdcBits;

	// 正在绘制的Item
	HTREEITEM hItemPainting;

	HANDLE      hheap;          // heap for allocs for win32

	POINT       ptCapture;      // Point where the mouse was capture

	COLORREF    clrText;
	COLORREF    clrBk;
	COLORREF    clrim;          // insert mark color.
	COLORREF    clrLine;        // line color

								// Dimensions
	SHORT       cxImage;        // image width
	SHORT       cyImage;        // image height
	SHORT       cyText;         // text height

								// Item基础高度
	SHORT cyItem;

	// Item边框的厚度，注意是Item边框，不是Tree边框
	SHORT cxBorder;
	SHORT cyBorder;

	// 每个缩进单位的宽度，缩进指下一级Item相对上一级Item的左缩进
	SHORT cxIndent;

	// cxWnd和cyWnd指TreeView客户区的宽和高，不包含滚动条及边框等非客户区
	SHORT cxWnd;
	SHORT cyWnd;

	// Scroll Positioners
	WORD cxMax; // 所有Item中，最宽的那一样，用来计算水平滚动信息

				// 用于SCROLLINFO的nPage属性
				// 当fSmoothVScroll为TRUE时，表示Tree客户区高度，这时，它的值与cyWnd相等
				// 当fSmoothVScroll为FASE时，表示Tree控件垂直方向能完整显示的Item数量，Item高度按cyItem计算
				// 这时，它的值与(cyWnd / cyItem)相等.
	WORD cFullVisible;

	// 水平滚动位置
	SHORT xPos;

	// cShowing的值等于SCROLLINFO::nMax + 1.
	// 当fSmoothVScroll为TRUE时，表示可见（非折叠）Item总高度；
	// 当fSmoothVScroll为FASE时，表示可见（非折叠）Item总数量，包含TREEITEM::iIntegral,
	// 即如果Tree只有一个Item，但Item的iIntegral为2的话（2倍基础Item高），
	// cShowing的值为2而不是1.
	UINT cShowing;

	// 总Item数
	UINT cItems;

	// 第一个视觉可见的Item（在TreeView客户区顶部那个Item）
	HTREEITEM hTop;

	UINT        uMaxScrollTime; // the maximum smooth scroll timing

								// fSmoothVScroll为TRUE时，表示每行垂直滚动(SB_LINEDOWN/SB_LINEUP)步进的像素
	WORD wSmoothVScroll;

	// WM_MOUSEWHEEL时的步进
	int m_nMouseWheelStep;

	// stuff for edit in place
	HWND        hwndEdit;       // Edit window for name editing.
	WNDPROC     pfnEditWndProc; // edit field subclass proc

								//tooltip stuff
	HWND        hwndToolTips;
	LPTSTR      pszTip;         // store current tooltip/infotip string.
#ifdef UNICODE
	LPSTR       pszTipA;        // store current ANSI tooltip/infotip string.
#endif

								// 增量搜索，即在TreeView中，按下键盘的几个字母，
								// TreeView快速定位到以这几个字母开头的Item。
	ISEARCHINFO is;

	// IUI Add
	HIUIIMAGE m_himgBk[COMBINEIMAGESIZE2];
	TEXT_ALIGN_HOR m_eTextHorAlignMode[COMBINEIMAGESIZE4];
	TEXT_ALIGN_VER m_eTextVerAlignMode[COMBINEIMAGESIZE4];
	CIUIRect m_rcTextMargin[COMBINEIMAGESIZE4];

	// for item line
	BOOL m_bEnableItemLine;
	COLORREF m_crItemLine;

	// for Collapse and expand image
	HIUIIMAGE m_himgCombineCollapseExpanded;
	IMAGE_RESIZE_MODE m_eCollapseExpandedImageResizeMode;
	CIUIRect m_rcCollapseExpandedImageResize;
	HIUIIMAGE m_himgCollapse[COMBINEIMAGESIZE4];
	HIUIIMAGE m_himgExpanded[COMBINEIMAGESIZE4];

	// 默认情况下，TreeView用鼠标只能选中一个Item，所以，
	// 在大部分情况下，focus Item和Selected Item是同一个Item，
	// 但TreeView允许用户使用代码，多选Item，这时，focus item甚至都不是selected item.
	// for focus(caret) item
	COLORREF m_crFocusItemText;
	BACKGROUND_TYPE m_eFocusItemBkType;
	COLORREF m_crFocusItemBK;
	HIUIIMAGE m_himgFocusItemBk;// Focus Item无Disabled状态
	CIUIRect m_rcFocusItemBkImage9GridArg;

	// for selected item
	BACKGROUND_TYPE m_eSelectedItemBkType;
	COLORREF m_crSelectedItemBK;
	BOOL m_bSelectedItemBkCombine;
	HIUIIMAGE m_himgSelectedItemCombineBk;
	HIUIIMAGE m_himgSelectedItemBk[2];
	CIUIRect m_rcSelectedItemBkImage9GridArg;
	COLORREF m_crSelectedItemText;

	// for highlight item
	BACKGROUND_TYPE m_eHighlightItemBkType;
	COLORREF m_crHighlightItemBK;
	BOOL m_bHighlightItemBkCombine;
	HIUIIMAGE m_himgHighlightItemCombineBk;
	HIUIIMAGE m_himgHighlightItemBk[2];
	CIUIRect m_rcHighlightItemBkImage9GridArg;
	COLORREF m_crHighlightItemText;

	// for disabled text color
	COLORREF m_crDisabledItemText;

	// for mouse hover
	DWORD m_dwHoverTime;
	BOOL m_bHoverRepeat;
} TREE, *PTREE;

#define TV_StateIndex(pitem) ((int)(((DWORD)((pitem)->state) >> 12) & 0xF))

#define KIDS_COMPUTE            0    // use hKids to determine if a node has children
#define KIDS_FORCE_YES          1    // force a node to have kids (ignore hKids)
#define KIDS_FORCE_NO           2    // force a node to not have kids (ignore hKids)
#define KIDS_CALLBACK           3    // callback to see if a node has kids
#define KIDS_INVALID            4    // all values this and above are bogus

#define MAXLABELTEXT            MAX_PATH

// BUGBUG: OINK OINK

//
//  Note that there are multiple senses of "visible" going on.
//
//  TREE.hTop tracks visibility in the sense of "will it be painted?"
//
//  TREEITEM.iShownIndex tracks visibility in the sense of "not collapsed".
//  You can be off the screen but as long as your parent is expanded
//  you get an iShownIndex.
//
//

typedef struct _TREEITEM
{
	_TREEITEM()
		: hParent(NULL)
		, hNext(NULL)
		, hKids(NULL)
		, lpstr(NULL)
		, lParam(NULL)
		, state(0)
		, iImage(0)
		, iSelectedImage(0)
		, iWidth(0)
		, iShownIndex(0)
		, iLevel(0)
		, fKids(0)
		, iIntegral(0)
		, bIntegralSet(0)
		, wSignature(0)
		, hBindWindowless(NULL)
		, pItemSpecBackground(NULL)
		, nBranchHeight(0)
	{

	}

	HTREEITEM hParent;          // allows us to walk back out of the tree
	HTREEITEM hNext;            // next sibling
	HTREEITEM hKids;            // first child
	LPTSTR    lpstr;            // item text, can be LPSTR_TEXTCALLBACK
	LPARAM lParam;              // item data

	WORD      state;            // TVIS_ state flags
	WORD      iImage;           // normal state image at iImage
	WORD      iSelectedImage;   // selected state image
	WORD      iWidth;           // cached: width of text area (for hit test, drawing)

								// 相对于TreeView top(不是Top Item)的垂直位置，永远为正值或-1。 在滚动过程中，
								// 这个值不会改变，但在插入Item、删除Item、展开或折叠Item等操作时，这个值需要改变。
								// 当TREE::fSmoothVScroll为TRUE时，表示Item top相对TreeView top的Y距离.
								// 当TREE::fSmoothVScroll为FALSE时，表示包含了iIntegral的显示索引，
								// 例如hItem上方那个Item的iShownIndex为3，iIntegral为2, 则hItem的iShownIndex为5。
								// 当索引为-1时，表示不可见（父节点不可见或被折叠）
	WORD iShownIndex;

	BYTE      iLevel;           // cached: level of item (indent)
	BYTE      fKids;            // KIDS_ values

								// 当TREE::fSmoothVScroll为TRUE时，表示Item行高，最大值：32767.
								// 支持每个Item有不同的高度;
								// 当TREE::fSmoothVScroll为FALSE时，表示基础行高的倍数
	WORD iIntegral;
	// 在垂直平滑滚动模式时，表示是否单独设置过这个Item的iIntegral（行高）
	// 重新设置TreeView行高时，忽略bIntegralSet为TRUE的Item.
	BOOL bIntegralSet;

	WORD      wSignature;       // for parameter validation, put at end of struct

								// IUI Add
	HWLWND hBindWindowless; // 绑定的控件
	ITEM_BACKGROUND *pItemSpecBackground;

	// TODO: 暂时未用到，供优化使用。 IUI Add
	// 当本Item是展开状态时，表示所有可见（展开的，不一定在可视范围内）子孙Item的高度之和，
	// 不包括自己, 当本Item是折叠状态时，表示展开时，能多展出来的高度。
	// 当本item无子item时，nBranchHeight为零。
	// 定义这个变量的目的是当展开或折叠某个Item时，能快速更新tree内容的总高度，
	// 而不需要遍历tree。 在垂直滚动Tree的时候，能快速设置第一个可见的Item TREE::hTop
	// 当增加、删除、展开、折叠或修改item高度时，应该更新祖宗Item的这个值
	int nBranchHeight;
} TREEITEM;

//
//  The signature is intentionally not ASCII characters, so it's
//  harder to run into by mistake.  I choose a value greater than
//  0x8000 so it can't be the high word of a pointer.
//
#define TV_SIG      0xABCD

#define TV_MarkAsDead(hti)      ((hti)->wSignature = 0)

#define ITEM_VISIBLE(hti) ((hti)->iShownIndex != (WORD)-1)

// get the parent, avoiding the hidden root node
#define VISIBLE_PARENT(hItem) (!(hItem)->iLevel ? NULL : (hItem)->hParent)

// REVIEW: make this a function if the optimizer doesn't do well with this
#define FULL_WIDTH(pTree, hItem)  (ITEM_OFFSET(pTree,hItem) + hItem->iWidth)
int PASCAL ITEM_OFFSET(PTREE pTree, HTREEITEM hItem);

#define VTI_NULLOK      1
BOOL ValidateTreeItem(HTREEITEM hItem, UINT flags);

#ifdef DEBUG
#define DBG_ValidateTreeItem(hItem, flags) ValidateTreeItem(hItem, flags)
#else
#define DBG_ValidateTreeItem(hItem, flags)
#endif

//
//  TVWATCHEDITEM
//
//  Structure that tracks items being watched.
//
//  See TV_StartWatch for more information, and TV_DoExpandRecurse
//  for an example.
//
//  The hti field is a bit odd.
//
//  if fStale == FALSE, then hti is the item being watched.
//  if fStale == TRUE , then hti is the item *after* the item being watched.
//
//  We keep this strange semantic for fStale==TRUE so that TV_NextWatchItem
//  can successfully step to the item after a deleted item.  (Normally,
//  trying to do anything with a deleted item will fault.)
//

typedef struct TVWATCHEDITEM
{
	HTREEITEM   hti;                    // current item
	BOOL        fStale;                 // has the original item been deleted?
} TVWATCHEDITEM, *PTVWATCHEDITEM;

BOOL TV_StartWatch(PTREE pTree, PTVWATCHEDITEM pwi, HTREEITEM htiStart);
BOOL TV_EndWatch(PTREE pTree, PTVWATCHEDITEM pwi);
#define TV_GetWatchItem(pTree, pwi) ((pwi)->hti)
#define TV_RestartWatch(pTree, pwi, htiStart) \
	((pwi)->hti = (htiStart), (pwi)->fStale = FALSE)
#define TV_IsWatchStale(pTree, pwi) ((pwi)->fStale)
#define TV_IsWatchValid(pTree, pwi) (!(pwi)->fStale)

//
//  TV_NextWatchItem - Enumerate the item after the watched item.
//                     This works even if the watched item was deleted.
//
#define TV_NextWatchItem(pTree, pwi) \
	((pwi)->fStale || ((pwi)->hti = (pwi)->hti->hNext)), \
	(pwi)->fStale = FALSE

BOOL TV_OnGetItem(PTREE pTree, LPXTVITEM ptvi);
// in TVSCROLL.C
BOOL       TV_ScrollBarsAfterAdd(PTREE, HTREEITEM);
BOOL       TV_ScrollBarsAfterRemove(PTREE, HTREEITEM);
BOOL       TV_ScrollBarsAfterMove(PTREE);
BOOL       TV_ScrollBarsAfterExpand(PTREE, HTREEITEM);
BOOL       TV_ScrollBarsAfterCollapse(PTREE, HTREEITEM);
void       TV_ScrollBarsAfterResize(PTREE, HTREEITEM, int, UINT);
BOOL       TV_ScrollBarsAfterSetWidth(PTREE, HTREEITEM);

BOOL TV_OffsetItemLevelRecurse(HTREEITEM hItem, int nNewLevel);

BOOL       TV_HorzScroll(PTREE, UINT, UINT);
BOOL       TV_VertScroll(PTREE, UINT, UINT);
BOOL       TV_SetLeft(PTREE, int);
#define TV_SetTopItem(pTree, i) TV_SmoothSetTopItem(pTree, i, 0)
BOOL       TV_SmoothSetTopItem(PTREE, UINT, UINT);
BOOL       TV_CalcScrollBars(PTREE);
BOOL       TV_ScrollIntoView(PTREE, HTREEITEM);
BOOL       TV_ScrollVertIntoView(PTREE, HTREEITEM);
HTREEITEM  TV_GetShownIndexItem(PTREE, HTREEITEM, UINT);
UINT       TV_ScrollBelow(PTREE, HTREEITEM, BOOL, BOOL);
BOOL       TV_SortChildren(PTREE, HTREEITEM, BOOL);
BOOL       TV_SortChildrenCB(PTREE, LPTV_SORTCB, BOOL);
void       TV_ComputeItemWidth(PTREE pTree, HTREEITEM hItem, HDC hdc);

// in TVPAINT.C
void        TV_GetBackgroundBrush(PTREE pTree, HDC hdc);
void        TV_UpdateTreeWindow(PTREE, BOOL);
void        TV_ChangeColors(PTREE);
void        TV_CreateIndentBmps(PTREE);
void        TV_Paint(PTREE, HDC);
HIMAGELIST  TV_CreateDragImage(PTREE pTree, HTREEITEM hItem);
BOOL        TV_ShouldItemDrawBlue(PTREE pTree, XTVITEM *ti, UINT flags);
LRESULT     TV_GenerateDragImage(PTREE ptree, SHDRAGIMAGE *pshdi);

BOOL TV_GetInsertMarkRect(PTREE pTree, LPRECT prc);

// in TVMEM.C

#define TVDI_NORMAL             0x0000  // TV_DeleteItem flags
#define TVDI_NONOTIFY           0x0001
#define TVDI_CHILDRENONLY       0x0002
#define TVDI_NOSELCHANGE        0x0004

BOOL       TV_DeleteItem(PTREE, HTREEITEM, UINT);
HTREEITEM  TV_InsertItem(PTREE pTree, LPTV_INSERTSTRUCT lpis);
BOOL  TV_MoveItem(PTREE pTree, LPTV_INSERTSTRUCT lpis);
void       TV_DestroyTree(PTREE);
LRESULT    TV_OnCreate(HWLWND hWnd, LPCREATESTRUCT);


#ifdef UNICODE
HTREEITEM  TV_InsertItemA(PTREE pTree, LPTV_INSERTSTRUCTA lpis);
#endif


// in TREEVIEW.C
BOOL      TV_GetItemRect(PTREE, HTREEITEM, LPRECT, BOOL);
BOOL      TV_Expand(PTREE pTree, WPARAM wCode, TREEITEM *hItem, BOOL fNotify);
HTREEITEM TV_GetNextItem(PTREE, HTREEITEM, WPARAM);
void      TV_GetItem(PTREE pTree, HTREEITEM hItem, UINT mask, LPXTVITEM lpItem);
void      TV_PopBubble(PTREE pTree);

// Flags for TV_SelectItem
#define TVSIF_NOTIFY            0x0001
#define TVSIF_UPDATENOW         0x0002
#define TVSIF_NOSINGLEEXPAND    0x0004

BOOL      TV_SelectItem(PTREE, WPARAM, HTREEITEM, UINT, UINT);
BOOL      TV_SendChange(PTREE, HTREEITEM, int, UINT, UINT, UINT, int, int);
HTREEITEM TV_GetNextVisItem(HTREEITEM);
HTREEITEM TV_GetPrevItem(HTREEITEM);
HTREEITEM TV_GetPrevVisItem(HTREEITEM);
void      TV_CalcShownItems(PTREE, HTREEITEM hItem);
void      TV_OnSetFont(PTREE, HFONT, BOOL);
BOOL      TV_SizeWnd(PTREE, UINT, UINT);
void      TV_InvalidateItem(PTREE, HTREEITEM, UINT uFlags);
VOID PASCAL TV_CreateBoldFont(PTREE pTree);
BOOL TV_SetInsertMark(PTREE pTree, HTREEITEM hItem, BOOL fAfter);

LRESULT CALLBACK TV_EditWndProc(HWND, UINT, WPARAM, LPARAM);
void TV_Terminate(BOOL fSystemExit);

LRESULT    TV_Timer(PTREE pTree, UINT uTimerId);
HWND       TV_OnEditLabel(PTREE pTree, HTREEITEM hItem);
void       TV_SetEditSize(PTREE pTree);
BOOL       TV_DismissEdit(PTREE pTree, BOOL fCancel);
void       TV_CancelPendingEdit(PTREE pTree);
int        TV_UpdateShownIndexes(PTREE pTree, HTREEITEM hWalk);


void TV_UnsubclassToolTips(PTREE pTree);
LRESULT WINAPI TV_SubClassWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void TV_SubclassToolTips(PTREE pTree);
BOOL TV_UpdateToolTip(PTREE pTree);
BOOL TV_SetToolTipTarget(PTREE pTree, HTREEITEM hItem);
void TV_OnSetBkColor(PTREE pTree, COLORREF clr);
void TV_InitCheckBoxes(PTREE pTree);

WORD TV_GetIntegral(PTREE pTree, HTREEITEM hItem);

#define TVMP_CALCSCROLLBARS (TV_FIRST + 0x1000)

// Fake customdraw.  See comment block in tvscroll.c

typedef struct TVFAKEDRAW
{
	NMTVCUSTOMDRAW nmcd;
	PTREE pTree;
	HFONT hfontPrev;
	DWORD dwCustomPrev;
	DWORD dwCustomItem;
} TVFAKEDRAW, *PTVFAKEDRAW;

void TreeView_BeginFakeCustomDraw(PTREE pTree, PTVFAKEDRAW ptvfd);
DWORD TreeView_BeginFakeItemDraw(PTVFAKEDRAW plvfd, HTREEITEM hitem);
void TreeView_EndFakeItemDraw(PTVFAKEDRAW ptvfd);
void TreeView_EndFakeCustomDraw(PTVFAKEDRAW ptvfd);

// BUGBUG -- penwin.h is screwy; define local stuff for now
#define HN_BEGINDIALOG                  40              // Lens/EditText/garbage detection dialog is about
// to come up on this hedit/bedit
#define HN_ENDDIALOG                       41           // Lens/EditText/garbage detection dialog has
// just been destroyed

//---------------------------------------------------------
#define IDT_SCROLLWAIT 43

//-----------------------
// ToolTip stuff...
//
#define REPEATTIME      SendMessage(pTree->hwndToolTips,TTM_GETDELAYTIME,(WPARAM)TTDT_RESHOW, 0)
#define CHECKFOCUSTIME  (REPEATTIME)
#define IDT_TOOLTIPWAIT   2
#define IDT_FOCUSCHANGE   3

void TV_HandleStateIconClick(PTREE pTree, HTREEITEM hItem);
HWND TV_EditLabel(PTREE pTree, HTREEITEM hItem, LPTSTR pszInitial);
void TV_CancelEditTimer(PTREE pTree);
BOOL TV_SetItem(PTREE pTree, LPCXTVITEM ptvi);
void TV_DeleteHotFonts(PTREE pTree);
BOOL TV_IsShowing(HTREEITEM hItem);

LRESULT TV_OnScroll(PTREE ptv, LPNMHDR pnm);

#define TVBD_FROMWHEEL      0x0001
#define TVBD_WHEELFORWARD   0x0002
#define TVBD_WHEELBACK      0x0004

WORD TV_GetIntegral(PTREE pTree, HTREEITEM hItem)
{
	if (pTree == NULL || hItem == NULL)
	{
		_ASSERT(FALSE);
		return 1;
	}

	if (pTree->m_bSmoothVScroll)
	{
		return hItem->bIntegralSet ? hItem->iIntegral : pTree->cyItem;
	}
	else
	{
		return hItem->iIntegral;
	}
}

BOOL ValidateTreeItem(TREEITEM *hItem, UINT flags)
{
	BOOL fValid = TRUE;

	/*
	 *  Check the values to make sure the new Win64-compatible values
	 *  are consistent with the old Win32 values.
	 */
	COMPILETIME_ASSERT(
		(DWORD)(ULONG_PTR)TVI_ROOT  == 0xFFFF0000 &&
		(DWORD)(ULONG_PTR)TVI_FIRST == 0xFFFF0001 &&
		(DWORD)(ULONG_PTR)TVI_LAST  == 0xFFFF0002 &&
		(DWORD)(ULONG_PTR)TVI_SORT  == 0xFFFF0003);

	if (hItem)
	{
		if (HIWORD64(hItem) == HIWORD64(TVI_ROOT))
		{
			switch (LOWORD(hItem))
			{
			//#pragma warning(disable:4309)
			case LOWORD(TVI_ROOT):
			case LOWORD(TVI_FIRST):
			case LOWORD(TVI_LAST):
			case LOWORD(TVI_SORT):
				//#pragma warning(default:4309)
				break;

			default:
				//L AssertMsg(FALSE, TEXT("ValidateTreeItem() Invalid special item"));
				fValid = FALSE;
				break;
			}
		}
		else
		{
			__try
			{
				// Use "volatile" to force memory access at start of struct
				*(volatile LPVOID *)hItem;
				fValid = hItem->wSignature == TV_SIG;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				fValid = FALSE;
			}
			//L __endexcept
		}

	}
	else if (!flags)                // The only flag is VTI_NULLOK
	{
		//L DebugMsg(DM_ERROR, TEXT("ValidateTreeItem(): NULL HTREEITEM"));
		fValid = FALSE;
	}

	return fValid;
}

// ----------------------------------------------------------------------------
//
//  Initialize TreeView on library entry -- register SysTreeView class
//
// ----------------------------------------------------------------------------

#pragma code_seg(CODESEG_INIT)

const TCHAR c_szTreeViewClass[] = _T("TreeView");

#pragma code_seg()


// 弹出Tooltip
void TV_PopBubble(PTREE pTree)
{
	if (pTree->hwndToolTips && pTree->m_hToolTipItem)
	{
		pTree->m_hToolTipItem = NULL;
		SendMessage(pTree->hwndToolTips, TTM_POP, 0L, 0L);
	}
}


// ----------------------------------------------------------------------------
//
//  Sends a TVN_BEGINDRAG or TVN_BEGINRDRAG notification with information in the ptDrag and
//  itemNew fields of an NM_TREEVIEW structure
//
// ----------------------------------------------------------------------------

BOOL TV_SendBeginDrag(PTREE pTree, int code, TREEITEM *hItem, int x, int y)
{
	NM_TREEVIEW nm;

	TV_PopBubble(pTree);            // dismiss the infotip if we start to drag

	nm.itemNew.hItem = hItem;
	nm.itemNew.state = hItem->state;
	nm.itemNew.lParam = hItem->lParam;
	nm.itemNew.mask = (TVIF_HANDLE | TVIF_STATE | TVIF_PARAM);
	nm.itemOld.mask = 0;
	nm.ptDrag.x = x;
	nm.ptDrag.y = y;

	return (BOOL)CCSendNotify(&pTree->m_hWnd->ci, code, &nm.hdr);
}


// ----------------------------------------------------------------------------
//
//  Sends a TVN_ITEMEXPANDING or TVN_ITEMEXPANDED notification with information
//  in the action and itemNew fields of an NM_TREEVIEW structure
//
//  Returns FALSE to allow processing to continue, or TRUE to stop.
//
//  If the hItem is destroyed by the callback, then we always return TRUE.
//
//  Note that the application cannot stop a TVN_ITEMEXPANDED, so the only
//  way a TVN_ITEMEXPANDED can return "Stop" is if the item got destroyed.
//
// ----------------------------------------------------------------------------

BOOL TV_SendItemExpand(PTREE pTree, int code, TREEITEM *hItem, WPARAM action)
{
	NM_TREEVIEW nm;
	TVWATCHEDITEM wi;
	BOOL fResult;
	BOOL fWatched;

	_ASSERT(code == TVN_ITEMEXPANDING || code == TVN_ITEMEXPANDED);

	nm.itemNew.mask = 0;
	nm.itemNew.hItem = hItem;
	if (hItem == TVI_ROOT)
	{
		hItem = pTree->m_hRootItem;
	}
	nm.itemNew.state = hItem->state;
	nm.itemNew.lParam = hItem->lParam;
	nm.itemNew.iImage = hItem->iImage;
	nm.itemNew.iSelectedImage = hItem->iSelectedImage;
	switch (hItem->fKids)
	{
	case KIDS_CALLBACK:
	case KIDS_FORCE_YES:
		nm.itemNew.cChildren = 1;
		nm.itemNew.mask = TVIF_CHILDREN;
		break;
	case KIDS_FORCE_NO:
		nm.itemNew.cChildren = 0;
		nm.itemNew.mask = TVIF_CHILDREN;
		break;
	}
	nm.itemNew.mask |= (TVIF_HANDLE | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE);
	nm.itemOld.mask = 0;

	nm.action = (UINT)(action & TVE_ACTIONMASK);

	//
	//  Some apps will delete the item while it is being expanded, since
	//  during expansion, they will realize, "Hey, the thing represented
	//  by this item no longer exists, I'd better delete it."  (E.g,.
	//  Explorer.)  So keep an eye on the item so we don't fault when
	//  this happens.
	//

	// If we can't start a watch, then tough, just send the notification
	// the unsafe way.
	fWatched = TV_StartWatch(pTree, &wi, hItem);

	fResult = (BOOL)CCSendNotify(&pTree->m_hWnd->ci, code, &nm.hdr);

	// The app return code from TVN_ITEMEXPANDED is ignored.
	// You can't stop a TVN_ITEMEXPANDED; it's already happened.
	if (code == TVN_ITEMEXPANDED)
	{
		fResult = FALSE;    // Continue processing
	}

	if (fWatched)
	{
		if (!TV_IsWatchValid(pTree, &wi))
		{
			fResult = TRUE;    // Oh no!  Stop!
		}

		TV_EndWatch(pTree, &wi);
	}

	return fResult;
}


// ----------------------------------------------------------------------------
//
//  Sends a TVN_SELCHANGING or TVN_SELCHANGED notification with information in
//  the itemOld and itemNew fields of an NM_TREEVIEW structure
//
// ----------------------------------------------------------------------------

BOOL TV_SendSelChange(PTREE pTree, int code, TREEITEM *hOldItem, TREEITEM *hNewItem, UINT action)
{
	NM_TREEVIEW nm;

	nm.action = action;

	nm.itemNew.hItem = hNewItem;
	nm.itemNew.state = hNewItem ? hNewItem->state : 0;
	nm.itemNew.lParam = hNewItem ? hNewItem->lParam : 0;
	nm.itemNew.mask = (TVIF_HANDLE | TVIF_STATE | TVIF_PARAM);

	nm.itemOld.hItem = hOldItem;
	nm.itemOld.state = hOldItem ? hOldItem->state : 0;
	nm.itemOld.lParam = hOldItem ? hOldItem->lParam : 0;
	nm.itemOld.mask = (TVIF_HANDLE | TVIF_STATE | TVIF_PARAM);

	return (BOOL)CCSendNotify(&pTree->m_hWnd->ci, code, &nm.hdr);
}

// ----------------------------------------------------------------------------
//
//  返回指定Item上方与指定Item最近的可见Item.
//
// ----------------------------------------------------------------------------
TREEITEM *TV_GetPrevVisItem(TREEITEM *hItem)
{
	TREEITEM *hParent = hItem->hParent;
	TREEITEM *hWalk;

	DBG_ValidateTreeItem(hItem, 0);

	// 如果父Item只有自己一个孩子
	if (hParent->hKids == hItem)
	{
		return VISIBLE_PARENT(hItem);
	}

	// 得到自己的上一个兄弟
	for (hWalk = hParent->hKids; hWalk->hNext != hItem; hWalk = hWalk->hNext)
	{
		;
	}

checkKids:
	// 如果上一个兄弟Item包含子Item，并且是展开状态
	if (hWalk->hKids && (hWalk->state & TVIS_EXPANDED))
	{
		// 得到上一个兄弟Item最后一个孩子
		for (hWalk = hWalk->hKids; hWalk->hNext != NULL; hWalk = hWalk->hNext)
		{
			;
		}

		goto checkKids;
	}

	return hWalk;
}


// ----------------------------------------------------------------------------
//
//  返回指定Item下方与指定Item最靠近的可见Item(按List顺序).
//
// ----------------------------------------------------------------------------

TREEITEM *TV_GetNextVisItem(TREEITEM *hItem)
{
	DBG_ValidateTreeItem(hItem, 0);

	// 如果Item有孩子，并且Item是展开的，则返回第一个孩子
	if (hItem->hKids != NULL && (hItem->state & TVIS_EXPANDED))
	{
		return hItem->hKids;
	}

checkNext:
	// 如果Item有下一个兄弟Item，则返回下一个兄弟Item
	if (hItem->hNext != NULL)
	{
		return hItem->hNext;
	}

	// 向上沿线查找
	hItem = hItem->hParent;
	if (hItem != NULL)
	{
		goto checkNext;
	}

	return NULL;
}

// ----------------------------------------------------------------------------
//
//  检测指定客户区坐标处命中了哪个item的哪部分
//  如果给定坐标位于客户区之外，返回NULL，并且wHitCode返
//  回TVHT_TOLEFT, TVHT_TORIGHT, TVHT_ABOVE, 和/或 TVHT_BELOW标记；
//  如果指定坐标在最后一个item下面的空隙区域，返回NULL，并且wHitCode返回TVHT_NOWHERE.
//  当TREE::fSmoothVScroll为TRUE时，最后一个Item下面永远不会有空隙区域，所以wHitCode也
//  不可能返回TVHT_NOWHERE。
//  否则, 返回命中的Item, wHitCode返回TVHT_ONITEMINDENT,
//  TVHT_ONITEMBUTTON, TVHT_ONITEMICON, TVHT_ONITEMLABEL, 或TVHT_ONITEMRIGHT
//
// ----------------------------------------------------------------------------
TREEITEM *TV_CheckHit(PTREE pTree, int x, int y, UINT *wHitCode)
{
	*wHitCode = 0;

	if (x < 0)
	{
		*wHitCode |= TVHT_TOLEFT;
	}
	else if (x > (int) pTree->cxWnd)
	{
		*wHitCode |= TVHT_TORIGHT;
	}

	if (y < 0)
	{
		*wHitCode |= TVHT_ABOVE;
	}
	else if (y > (int) pTree->cyWnd)
	{
		*wHitCode |= TVHT_BELOW;
	}

	if (*wHitCode)
	{
		return NULL;
	}

	TREEITEM *hItem = pTree->hTop;

	// 先命中Item
	if (pTree->m_bSmoothVScroll)
	{
		int nVScrollPos = GetScrollPos(pTree->m_hWnd, SB_VERT);

		// 初始hItem为hTop Item，如果指定y坐标大于hItem的下坐标，则往下遍历
		while (hItem != NULL && y > hItem->iShownIndex + TV_GetIntegral(pTree, hItem) - nVScrollPos)
		{
			hItem = TV_GetNextVisItem(hItem);
		}
	}
	else
	{
		int index = y / pTree->cyItem;

		while (hItem != NULL && index >= TV_GetIntegral(pTree, hItem))
		{
			index -= TV_GetIntegral(pTree, hItem);
			hItem = TV_GetNextVisItem(hItem);
		}
	}

	if (hItem == NULL)
	{
		*wHitCode = TVHT_NOWHERE;
		return NULL;
	}

	x -= (pTree->cxBorder + (hItem->iLevel * pTree->cxIndent));
	x += pTree->xPos;

	if ((pTree->m_hWnd->ci.style & (TVS_HASLINES | TVS_HASBUTTONS)) &&
		(pTree->m_hWnd->ci.style & TVS_LINESATROOT))
	{
		// Subtract some more to make up for the pluses at the root
		x -= pTree->cxIndent;
	}

	XTVITEM sItem;
	TV_GetItem(pTree, hItem, TVIF_CHILDREN, &sItem);
	int cxState = TV_StateIndex(&sItem) ? pTree->m_cxState : 0;
	if (x <= (int)(hItem->iWidth + pTree->cxImage + cxState))
	{

		if (x >= 0)
		{
			if (pTree->m_hImlState && (x < cxState))
			{
				*wHitCode = TVHT_ONITEMSTATEICON;
			}
			else if (pTree->m_hImageList && (x < (int) pTree->cxImage + cxState))
			{
				*wHitCode = TVHT_ONITEMICON;
			}
			else
			{
				*wHitCode = TVHT_ONITEMLABEL;
			}
		}
		else if ((x >= -pTree->cxIndent) && sItem.cChildren && (pTree->m_hWnd->ci.style & TVS_HASBUTTONS))
		{
			*wHitCode = TVHT_ONITEMBUTTON;
		}
		else
		{
			*wHitCode = TVHT_ONITEMINDENT;
		}
	}
	else
	{
		*wHitCode = TVHT_ONITEMRIGHT;
	}

	return hItem;
}

//  This is tricky because CheckForDragBegin yields and the app may have
//  destroyed the item we are thinking about dragging
//
//  To give the app some feedback, we give the hItem the drop highlight
//  if it isn't already the caret.  This also allows us to check if the
//  item got deleted behind our back - TV_DeleteItemRecurse makes sure
//  that deleted items are never the m_hCaretItem or m_hDropTargetItem.
//
//  After TV_CheckForDragBegin, the caller must call TV_FinishCheckDrag
//  to clean up the UI changes that TV_CheckForDragBegin temporarily
//  performed.
//
BOOL TV_CheckForDragBegin(PTREE pTree, HTREEITEM hItem, int x, int y)
{
	BOOL fDrag;

	//
	//  If the item is not the caret, then make it the (temporary)
	//  drop target so the user gets some feedback.
	//
	//  BUGBUG raymondc - If hItem == pTree->m_hCaretItem, it still might not
	//  be visible if the control doesn't yet have focus and the treeview
	//  is not marked showselalways.  Maybe we should just always set
	//  hItem to DROPHILITE.
	//
	if (hItem == pTree->m_hCaretItem)
	{
		pTree->m_hOldDropItem = NULL;
		pTree->m_bRestoreOldDrop = FALSE;
	}
	else
	{
		pTree->m_hOldDropItem = pTree->m_hDropTargetItem;
		pTree->m_bRestoreOldDrop = TRUE;
		TV_SelectItem(pTree, TVGN_DROPHILITE, hItem, 0, TVC_BYMOUSE);
		_ASSERT(hItem == pTree->m_hDropTargetItem);
	}

	//
	//  We are dragging the hItem if CheckForDragBegin says okay,
	//  and TV_DeleteItemRecurse didn't wipe us out.
	//
	fDrag = CheckForDragBegin(pTree->m_hWnd, x, y) &&
		(hItem == pTree->m_hDropTargetItem || hItem == pTree->m_hCaretItem);

	return fDrag;
}

void TV_FinishCheckDrag(PTREE pTree)
{
	//
	//  Clean up our temporary UI changes that happened when we started
	//  dragging.
	//
	if (pTree->m_bRestoreOldDrop)
	{
		HTREEITEM hOldDrop = pTree->m_hOldDropItem;
		pTree->m_bRestoreOldDrop = FALSE;
		pTree->m_hOldDropItem = NULL;
		TV_SelectItem(pTree, TVGN_DROPHILITE, hOldDrop, 0, TVC_BYMOUSE);
	}
}

void TV_SendRButtonDown(PTREE pTree, int x, int y)
{
	BOOL fRet = FALSE;
	UINT wHitCode;
	TREEITEM *hItem = TV_CheckHit(pTree, x, y, &wHitCode);

	if (!TV_DismissEdit(pTree, FALSE))   // end any previous editing (accept it)
	{
		return;    // Something happened such that we should not process button down
	}

	//
	// Need to see if the user is going to start a drag operation
	//

	GetMessagePosClient(pTree->m_hWnd, &pTree->ptCapture);

	if (TV_CheckForDragBegin(pTree, hItem, x, y))
	{
		// let them start dragging
		if (hItem)
		{
			pTree->m_hDragItem = hItem;
			TV_SendBeginDrag(pTree, TVN_BEGINRDRAG, hItem, x, y);
		}
	}
	else if (!IsWindow(pTree->m_hWnd))
	{
		return;             // bail!
	}
	else
	{
		SetFocus(GetHostWnd(pTree->m_hWnd), pTree->m_hWnd);  // Activate this window like listview...
		fRet = !CCSendNotify(&pTree->m_hWnd->ci, NM_RCLICK, NULL);
	}

	// Don't finish the CheckForDragBegin until after the NM_RCLICK
	// because apps want to display the context menu while the
	// temporary drag UI is still active.
	TV_FinishCheckDrag(pTree);

	if (fRet)
	{
		// 应该用CWLWnd::RaiseEvent();
		// SendMessage(pTree->m_hWnd->ci.hwndParent, WM_CONTEXTMENU, (WPARAM)GetHostWnd(pTree->m_hWnd), GetMessagePos());
	}
}


// ----------------------------------------------------------------------------
//
//  If the given item is visible in the client area, the rectangle that
//  surrounds that item is invalidated
//
// ----------------------------------------------------------------------------

void TV_InvalidateItem(PTREE pTree, TREEITEM *hItem, UINT fRedraw)
{
	RECT rc;

	if (hItem != NULL
		&& pTree->m_bRedraw
		&& TV_GetItemRect(pTree, hItem, &rc, FALSE))
	{
		InvalidateRect(pTree->m_hWnd, &rc);
	}
}

//
//  Given an item, compute where the text of this item ends up being painted.
//  Basically, stare at TV_DrawItem and dutifully reproduce all the code that
//  messes with the x-coordinate.
//
int PASCAL ITEM_OFFSET(PTREE pTree, HTREEITEM hItem)
{
	int x = pTree->cxBorder + (hItem->iLevel * pTree->cxIndent);

	// state image
	// BUGBUG -- doesn't handle TVCDRF_NOIMAGES - whose idea was that?
	if (pTree->m_hImlState && TV_StateIndex(hItem))
	{
		x += pTree->m_cxState;
	}

	// image
	if (pTree->m_hImageList)
	{
		// even if not drawing image, draw text in right place
		x += pTree->cxImage;
	}

	// "plus" at the front of the tree
	if ((pTree->m_hWnd->ci.style & TVS_LINESATROOT) &&
		(pTree->m_hWnd->ci.style & (TVS_HASLINES | TVS_HASBUTTONS)))
	{
		x += pTree->cxIndent;
	}


	return x;
}

// ----------------------------------------------------------------------------
//
//  如果Item是展开的，得到Item坐标
//
//  如果Item是展开的，返回TRUE，否则FALSE.
//
// ----------------------------------------------------------------------------
BOOL TV_GetItemRect(PTREE pTree, TREEITEM *hItem, LPRECT lprc, BOOL bItemRect)
{
	if (hItem == NULL)
	{
		return FALSE;
	}

	DBG_ValidateTreeItem(hItem, 0);

	if (!ITEM_VISIBLE(hItem))
	{
		return FALSE;
	}

	if (bItemRect)
	{
		// Calculate where X position should start...
		lprc->left = -pTree->xPos + ITEM_OFFSET(pTree, hItem);
		lprc->right = lprc->left + hItem->iWidth;
	}
	else
	{
		lprc->left = 0;
		lprc->right = pTree->cxWnd;
	}

	if (pTree->m_bSmoothVScroll)
	{
		lprc->top = hItem->iShownIndex - GetScrollPos(pTree->m_hWnd, SB_VERT);
		lprc->bottom = lprc->top + TV_GetIntegral(pTree, hItem);
	}
	else
	{
		// iOffset是被计算的Item相对第一个视觉可见的Item的垂直偏移
		UINT iOffset = hItem->iShownIndex - pTree->hTop->iShownIndex;

		lprc->top = iOffset * pTree->cyItem;
		lprc->bottom = lprc->top + (pTree->cyItem * TV_GetIntegral(pTree, hItem)) ;
	}

	return TRUE;
}

void TV_OnSetRedraw(PTREE pTree, BOOL fRedraw)
{
	pTree->m_bRedraw = TRUE && fRedraw;
	if (pTree->m_bRedraw)
	{
		// This use to only refresh the items from hTop down, this is bad as if items are inserted
		// before the visible point within the tree then we would fail!
		if (pTree->m_hRootItem != NULL)
		{
			pTree->cShowing = TV_UpdateShownIndexes(pTree, pTree->m_hRootItem);
		}

		//  Must force recalculation of all tree items to get the right cxMax.
		TV_ScrollBarsAfterSetWidth(pTree, NULL);

		InvalidateRect(pTree->m_hWnd, NULL); //REVIEW: could be smarter
	}
}

//  Treeview item watching implementation
//
//  You need to "watch" an item any time you hold onto its HTREEITEM
//  and then yield control to the application.  If you didn't watch
//  the item, then if the app deletes the item, you end up with a
//  stale HTREEITEM pointer and fault.
//
//  To begin watching an item, call TV_StartWatch with the item you
//  want to start watching.  When finished watching, call TV_EndWatch.
//
//  In between, you can call TV_IsWatchStale() which tells you if the
//  item has been deleted behind your back and you shouldn't use it.
//  Alternatively, use TV_IsWatchValid() which says if it's okay.
//
//  Additional bonus behavior for enumeration:  If the watched item
//  is deleted, we cache the hNext item so that you can step to the
//  item after the one that got deleted.  Note that this works even
//  if the hNext item gets deleted before you get a chance to look,
//  because we just move the cached item to the hNext's hNext.
//
//  Sample usage for watching:
//
//  TVWATCHEDITEM wi;
//  if (TV_StartWatch(pTree, &wi, htiStartHere)) {
//      FunctionThatYields();
//      if (TV_IsWatchValid(pTree, &wi)) {
//          KeepUsing(htiStartHere);
//      } else {
//          // item was deleted while we yielded; stop using it
//      }
//      TV_EndWatch(pTree, &wi);
//  }
//
//  Sample usage for enumerating:
//
//  TVWATCHEDITEM wi;
//  if (TV_StartWatch(pTree, &wi, htiFirst)) {
//      while (TV_GetWatchItem(pTree, &wi)) {
//          FunctionThatYields(TV_GetWatchItem(pTree, &wi));
//          if (TV_IsWatchValid(pTree, &wi)) {
//              KeepUsing(htiStartHere);
//          } else {
//              // item was deleted while we yielded; stop using it
//          }
//          TV_NextWatchItem(pTree, &wi);
//      }
//      TV_EndWatch(pTree, &wi);
//  }
//
//
//

//
//  TV_StartWatch - Begin watching an item.
//
//  Returns FALSE if out of memory.
//
BOOL TV_StartWatch(PTREE pTree, PTVWATCHEDITEM pwi, HTREEITEM htiStart)
{
	pwi->hti = htiStart;
	pwi->fStale = FALSE;
	return DPA_AppendPtr(pTree->m_hDpaWatch, pwi) != -1;
}

//
//  TV_EndWatch - Remove the item from the watch list.
//
BOOL TV_EndWatch(PTREE pTree, PTVWATCHEDITEM pwi)
{
	int i = DPA_GetPtrCount(pTree->m_hDpaWatch);
	while (--i >= 0)
	{
		PTVWATCHEDITEM pwiT = (PTVWATCHEDITEM)DPA_FastGetPtr(pTree->m_hDpaWatch, i);
		_ASSERT(pwiT);
		if (pwi == pwiT)
		{
			IUI_DPA_DeletePtr(pTree->m_hDpaWatch, i);
			return TRUE;
		}
	}
	_ASSERT(!"TV_EndWatch: Item not in list");
	return FALSE;
}

//  End of treeview item watching implementation

void TV_SetItemRecurse(PTREE pTree, TREEITEM *hItem, LPXTVITEM ptvi)
{
	// Note:  This code assumes nobody will try to delete an item
	//        during a SetItem notification.
	while (hItem)
	{
		ptvi->hItem = hItem;
		TV_SetItem(pTree, ptvi);
		if (hItem->hKids)
		{
			TV_SetItemRecurse(pTree, hItem->hKids, ptvi);
		}

		hItem = hItem->hNext;
	}
}

BOOL TV_DoExpandRecurse(PTREE pTree, TREEITEM *hItem, BOOL fNotify)
{
	TVWATCHEDITEM wi;
	BOOL fRc = FALSE;

	if (TV_StartWatch(pTree, &wi, hItem))
	{
		while ((hItem = TV_GetWatchItem(pTree, &wi)))
		{

			// was the escape key pressed at any point since the last check?
			if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
			{
				goto failed;
			}

			TV_Expand(pTree, TVE_EXPAND, hItem, fNotify); // yields
			if (TV_IsWatchValid(pTree, &wi))
			{
				if (hItem->hKids)
				{
					if (!TV_DoExpandRecurse(pTree, hItem->hKids, fNotify))
					{
						goto failed;
					}
				}
			}
			TV_NextWatchItem(pTree, &wi);
		}
		fRc = TRUE;
failed:
		TV_EndWatch(pTree, &wi);
	}
	return fRc;
}


void TV_ExpandRecurse(PTREE pTree, TREEITEM *hItem, BOOL fNotify)
{
	BOOL fRedraw = pTree->m_bRedraw;

	TV_OnSetRedraw(pTree, FALSE);

	// we're going to check this after each expand so clear it first
	GetAsyncKeyState(VK_ESCAPE);

	TV_Expand(pTree, TVE_EXPAND, hItem, fNotify);
	// BUGBUG hItem may have gone bad during that TV_Expand
	TV_DoExpandRecurse(pTree, hItem->hKids, fNotify);
	TV_OnSetRedraw(pTree, fRedraw);
}

void TV_ExpandParents(PTREE pTree, TREEITEM *hItem, BOOL fNotify)
{
	hItem = hItem->hParent;
	if (hItem)
	{
		TVWATCHEDITEM wi;
		if (TV_StartWatch(pTree, &wi, hItem))
		{
			TV_ExpandParents(pTree, hItem, fNotify);

			// Item may have gone invalid during expansion
			if (TV_IsWatchValid(pTree, &wi) &&

				// make sure this item is not in a collapsed branch
				!(hItem->state & TVIS_EXPANDED))
			{

				TV_Expand(pTree, TVE_EXPAND, hItem, fNotify);
			}
			TV_EndWatch(pTree, &wi);
		}
	}
}

// makes sure an item is expanded and scrolled into view

BOOL TV_EnsureVisible(PTREE pTree, TREEITEM *hItem)
{
	TV_ExpandParents(pTree, hItem, TRUE);
	return TV_ScrollIntoView(pTree, hItem);
}

//
//  Walk up the tree towards the root until we find the item at level iLevel.
//  Note the cast to (char) because iLevel is a BYTE, so the root's level is
//  0xFF.  Casting to (char) turns 0xFF it into -1.
//
HTREEITEM TV_WalkToLevel(HTREEITEM hWalk, int iLevel)
{
	int i;
	for (i = (char)hWalk->iLevel - iLevel; i > 0; i--)
	{
		hWalk = hWalk->hParent;
	}
	return hWalk;
}

// this is to handle single expand mode.
// The new selection is toggled, and the old selection is collapsed

// assume that parents of hNewSel are already fully expanded
// to do this, we build a parent dpa for the old and new
// then go through find the first parent node of the old selection that's not in
// the new sel tree.  and expand that.
void TV_ExpandOnSelChange(PTREE pTree, TREEITEM *hNewSel, TREEITEM *hOldSel)
{
	LRESULT dwAbort;
	NM_TREEVIEW nm;
	BOOL fCollapsing;
	TVWATCHEDITEM wiOld, wiNew;

	// Revalidate hNewSel and hOldSel since they may have been deleted
	// during all the notifications that occurred in the meantime.
	if (!ValidateTreeItem(hOldSel, VTI_NULLOK) ||
		!ValidateTreeItem(hNewSel, VTI_NULLOK))
	{
		return;
	}

	if (TV_StartWatch(pTree, &wiOld, hOldSel))
	{
		if (TV_StartWatch(pTree, &wiNew, hNewSel))
		{
			// Let the app clean up after itself
			nm.itemOld.hItem = hOldSel;
			if (hOldSel)
			{
				nm.itemOld.lParam = hOldSel->lParam;
			}
			nm.itemOld.mask = (TVIF_HANDLE | TVIF_PARAM);

			nm.itemNew.hItem = hNewSel;
			if (hNewSel)
			{
				nm.itemNew.lParam = hNewSel->lParam;
			}
			nm.itemNew.mask = (TVIF_HANDLE | TVIF_PARAM);

			dwAbort = CCSendNotify(&pTree->m_hWnd->ci, TVN_SINGLEEXPAND, &nm.hdr);

			UpdateWindow(GetHostWnd(pTree->m_hWnd));

			// Revalidate hNewSel and hOldSel since they may have been deleted
			// by that notification.
			if (!TV_IsWatchValid(pTree, &wiOld) ||
				!TV_IsWatchValid(pTree, &wiNew))
			{
				goto cleanup;
			}

			// Collapse if the NewSel currently expanded.
			fCollapsing = hNewSel && (hNewSel->state & TVIS_EXPANDED);

			// Note that Ctrl+select allows the user to suppress the collapse
			// of the old selection.
			if ((!(dwAbort & TVNRET_SKIPOLD)) && hOldSel  && (GetKeyState(VK_CONTROL) >= 0))
			{

				//
				//  Collapse parents until we reach the common ancestor between
				//  hOldSel and hNewSel.  Note carefully that we don't cache
				//  any HTREEITEMs to avoid revalidation problems.
				//

				//
				//  Find the common ancestor, which might be the tree root.
				//
				int iLevelCommon;

				if (!hNewSel)
				{
					iLevelCommon = -1;    // common ancestor is root
				}
				else
				{
					HTREEITEM hItemO, hItemN;
					iLevelCommon = min((char)hOldSel->iLevel, (char)hNewSel->iLevel);
					hItemO = TV_WalkToLevel(hOldSel, iLevelCommon);
					hItemN = TV_WalkToLevel(hNewSel, iLevelCommon);
					while (iLevelCommon >= 0 && hItemO != hItemN)
					{
						iLevelCommon--;
						hItemO = hItemO->hParent;
						hItemN = hItemN->hParent;
					}
				}

				//
				//  Now walk up the tree from hOldSel, collapsing everything
				//  until we reach the common ancestor.  Do not collapse the
				//  common ancestor.
				//

				while ((char)hOldSel->iLevel > iLevelCommon)
				{
					TV_Expand(pTree, TVE_COLLAPSE, hOldSel, TRUE);
					if (!TV_IsWatchValid(pTree, &wiOld))
					{
						break;
					}
					hOldSel = hOldSel->hParent;
					TV_RestartWatch(pTree, &wiOld, hOldSel);
				}

			}

			if ((!(dwAbort & TVNRET_SKIPNEW)) && hNewSel && TV_IsWatchValid(pTree, &wiNew))
			{
				TV_Expand(pTree, TVE_TOGGLE, hNewSel, TRUE);
				UpdateWindow(GetHostWnd(pTree->m_hWnd));

			}

cleanup:
			TV_EndWatch(pTree, &wiNew);
		}
		TV_EndWatch(pTree, &wiOld);
	}
}

// ----------------------------------------------------------------------------
//
//  Notify the parent that the selection is about to change.  If the change is
//  accepted, de-select the current selected item and select the given item
//
//  sets m_hCaretItem
//
// in:
//      hItem   item to become selected
//      wType   TVGN_ values (TVGN_CARET, TVGN_DROPHILIGHT are only valid values)
//      flags   combination of flags
//          TVSIF_NOTIFY        - send notify to parent window
//          TVSIF_UPDATENOW     - do UpdateWindow() to force sync painting
//          TVSIF_NOSINGLEEXPAND- don't do single-expand stuff
//      action  action code to send identifying how selection is being made
//
//  NOTE: Multiple Selection still needs to be added -- this multiplesel code
//        is garbage
//
// ----------------------------------------------------------------------------

BOOL TV_SelectItem(PTREE pTree, WPARAM wType, TREEITEM *hItem, UINT flags, UINT action)
{
	UINT uRDWFlags = RDW_INVALIDATE;

	if (pTree->m_hImageList && (ImageList_GetBkColor(pTree->m_hImageList) == (COLORREF)(-1)))
	{
		uRDWFlags |= RDW_ERASE;
	}

	if (!ValidateTreeItem(hItem, VTI_NULLOK))
	{
		return FALSE;    // Invalid parameter
	}

	switch (wType)
	{

	case TVGN_FIRSTVISIBLE:
		if (!hItem)
		{
			return FALSE;
		}

		TV_EnsureVisible(pTree, hItem);
		if (pTree->m_bVert)
		{
			TV_SetTopItem(pTree, hItem->iShownIndex);
		}
		break;

	case TVGN_DROPHILITE:

		_ASSERT(hItem == NULL || ITEM_VISIBLE(hItem));

		if (hItem != pTree->m_hDropTargetItem)
		{
			if (pTree->m_hDropTargetItem)
			{
				pTree->m_hDropTargetItem->state &= ~TVIS_DROPHILITED;
				TV_InvalidateItem(pTree, pTree->m_hDropTargetItem, uRDWFlags);
			}

			if (hItem)
			{
				hItem->state |= TVIS_DROPHILITED;
				TV_InvalidateItem(pTree, hItem, uRDWFlags);
			}
			pTree->m_hDropTargetItem = hItem;

			if (pTree->m_hCaretItem)
			{
				TV_InvalidateItem(pTree, pTree->m_hCaretItem, uRDWFlags);
			}


			if (flags & TVSIF_UPDATENOW)
			{
				UpdateWindow(GetHostWnd(pTree->m_hWnd));
			}
		}
		break;

	case TVGN_CARET:

		// REVIEW: we may want to scroll into view in this case
		// it's already the selected item, just return
		if (pTree->m_hCaretItem != hItem)
		{

			TREEITEM *hOldSel;

			if ((flags & TVSIF_NOTIFY) && TV_SendSelChange(pTree, TVN_SELCHANGING, pTree->m_hCaretItem, hItem, action))
			{
				return FALSE;
			}

			// 去掉旧的焦点Item
			if (pTree->m_hCaretItem != NULL)
			{
				pTree->m_hCaretItem->state &= ~TVIS_SELECTED;
				TV_InvalidateItem(pTree, pTree->m_hCaretItem, uRDWFlags);
			}

			// 设置新的焦点Item
			hOldSel = pTree->m_hCaretItem;
			pTree->m_hCaretItem = hItem;

			if (hItem != NULL)
			{
				hItem->state |= TVIS_SELECTED;

				// make sure this item is not in a collapsed branch
				TV_ExpandParents(pTree, hItem, (flags & TVSIF_NOTIFY));

				TV_InvalidateItem(pTree, hItem, uRDWFlags);

				if (action == TVC_BYMOUSE)
				{
					// if selected by mouse, let's wait a doubleclick sec before scrolling
					SetTimer(pTree->m_hWnd, IDT_SCROLLWAIT, GetDoubleClickTime(), NULL);
					pTree->m_bScrollWait = TRUE;
				}
				else if (pTree->m_bRedraw)
				{
					TV_ScrollVertIntoView(pTree, hItem);
				}
			}
			if (pTree->hwndToolTips)
			{
				TV_Timer(pTree, IDT_TOOLTIPWAIT);
			}

			if (flags & TVSIF_NOTIFY)
			{
				TV_SendSelChange(pTree, TVN_SELCHANGED, hOldSel, hItem, action);
			}

			if ((pTree->m_hWnd->ci.style & TVS_SINGLEEXPAND) &&
				!(flags & TVSIF_NOSINGLEEXPAND) &&
				action != TVC_BYKEYBOARD)
			{
				TV_ExpandOnSelChange(pTree, pTree->m_hCaretItem, hOldSel);
			}

			if (flags & TVSIF_UPDATENOW)
			{
				Invalidate(pTree->m_hWnd);
			}

			MyNotifyWinEvent(EVENT_OBJECT_FOCUS, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
				(LONG_PTR)hItem);
			MyNotifyWinEvent(EVENT_OBJECT_SELECTION, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
				(LONG_PTR)hItem);
		}
		break;

	default:
		//L DebugMsg(DM_TRACE, TEXT("Invalid type passed to TV_SelectItem"));
		return FALSE;
	}

	return TRUE;        // success
}

// remove all the children, but pretend they are still there

BOOL TV_ResetItem(PTREE pTree, HTREEITEM hItem)
{
	TV_DeleteItem(pTree, hItem, TVDI_CHILDRENONLY);

	hItem->state &= ~TVIS_EXPANDEDONCE;
	hItem->fKids = KIDS_FORCE_YES;      // force children

	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  展开或折叠一个Item的孩子
//  如果有改变，返回TRUE，否则返回FALSE.
//
// ----------------------------------------------------------------------------
BOOL TV_Expand(PTREE pTree, WPARAM wCode, TREEITEM *hItem, BOOL fNotify)
{
	// deal with the evil invisible root for multiple root trees.
	if ((hItem == NULL) || (hItem == TVI_ROOT))
	{
		hItem = pTree->m_hRootItem;
	}

	DBG_ValidateTreeItem(hItem, 0);

	XTVITEM sItem;
	TV_GetItem(pTree, hItem, TVIF_CHILDREN, &sItem);

	if (!(wCode & TVE_ACTIONMASK) || sItem.cChildren == 0)
	{
		// 没有孩子让你展开或折叠
		return FALSE;
	}

	if ((wCode & TVE_ACTIONMASK) == TVE_TOGGLE)
	{
		wCode = (wCode & ~TVE_ACTIONMASK);

		// if it's not expaned, or not fully expanded, expand now
		wCode |=
			(((!(hItem->state & TVIS_EXPANDED)) ||
					hItem->state & TVIS_EXPANDPARTIAL) ? TVE_EXPAND : TVE_COLLAPSE);
	}

	if (((wCode & TVE_ACTIONMASK) == TVE_EXPAND) && !(hItem->state & TVIS_EXPANDEDONCE))
	{
		// if its the first expand, ALWAYS notify the parent
		fNotify = TRUE;
	}

	// at this point the children may be added if they aren't already there (callback)

	// 发送TVN_ITEMEXPANDING消息
	TREEITEM *hItemExpanding = hItem;
	if (fNotify && TV_SendItemExpand(pTree, TVN_ITEMEXPANDING, hItemExpanding, wCode))
	{
		return FALSE;
	}

	if (hItem->hKids == NULL)
	{
		// kids we removed, or never there
		TV_InvalidateItem(pTree, hItem, RDW_INVALIDATE);
		return FALSE;
	}

	WORD fOldState = hItem->state;

	if (hItem->hParent != NULL) // never turn off TVIS_EXPANED for the invisible root
	{
		if ((wCode & TVE_ACTIONMASK) == TVE_EXPAND)
		{
			hItem->state |= TVIS_EXPANDED;
		}
		else
		{
			hItem->state &= ~(TVIS_EXPANDED | TVIS_EXPANDPARTIAL);
		}

		if (wCode & TVE_EXPANDPARTIAL)
		{
			hItem->state |= TVIS_EXPANDPARTIAL;
		}
		else
		{
			hItem->state &= ~(TVIS_EXPANDPARTIAL);
		}
	}

	// if we're not changing the expanded state
	// check to see if we're supposed to collapse reset
	if (!(fOldState & TVIS_EXPANDED)
		&& !(hItem->state & TVIS_EXPANDED))
	{
		if ((wCode & (TVE_ACTIONMASK | TVE_COLLAPSERESET)) == (TVE_COLLAPSE | TVE_COLLAPSERESET))
		{
			TV_ResetItem(pTree, hItem);
		}

		return FALSE;
	}

	// 如果修改了展开状态，需要重新计算滚动信息
	if ((fOldState ^ hItem->state) & TVIS_EXPANDED)
	{
		UINT cntVisDescendants = TV_ScrollBelow(pTree,
				hItem, TRUE, hItem->state & TVIS_EXPANDED);

		if (hItem->state & TVIS_EXPANDED)
		{
			TV_ScrollBarsAfterExpand(pTree, hItem);

			InvalidateRect(pTree->m_hWnd, NULL);

			// 让展开的最后一个Item滚动到TreeView视图内
			UINT wNewTop = pTree->hTop->iShownIndex;
			UINT wTopOffset = hItem->iShownIndex - wNewTop;
			UINT wLastKid = wTopOffset + cntVisDescendants + 1;

			if (wLastKid > pTree->cFullVisible)
			{
				wNewTop += min(wLastKid - pTree->cFullVisible, wTopOffset);
				TV_SetTopItem(pTree, wNewTop);
			}
		}
		else
		{
			TV_ScrollBarsAfterCollapse(pTree, hItem);
			TV_ScrollVertIntoView(pTree, hItem);

			InvalidateRect(pTree->m_hWnd, NULL);

			// If we collapsed the subtree that contains the caret, then
			// pop the caret back to the last visible ancestor
			// Pass TVIS_NOSINGLEEXPAND so we won't expand an item right
			// after we collapsed it (d'oh!)
			if (pTree->m_hCaretItem)
			{
				TREEITEM *hWalk = TV_WalkToLevel(pTree->m_hCaretItem, hItem->iLevel);

				if (hWalk == hItem)
				{
					TV_SelectItem(pTree, TVGN_CARET, hItem, (fNotify ? TVSIF_NOTIFY : 0) | TVSIF_UPDATENOW | TVSIF_NOSINGLEEXPAND, TVC_UNKNOWN);
				}
			}
		}
	}
	else if ((fOldState ^ hItem->state) & TVIS_EXPANDPARTIAL)
	{
		// we didn't change the expanded state, only the expand partial
		TV_InvalidateItem(pTree, hItem, RDW_INVALIDATE);
	}

	if (fNotify && TV_SendItemExpand(pTree, TVN_ITEMEXPANDED, hItem, wCode))
	{
		return FALSE;
	}

	hItem->state |= TVIS_EXPANDEDONCE;

	if ((wCode & (TVE_ACTIONMASK | TVE_COLLAPSERESET)) == (TVE_COLLAPSE | TVE_COLLAPSERESET))
	{
		TV_ResetItem(pTree, hItem);
	}

	// BUGBUG raymondc v6 we generate a notification even if nothing happened,
	// which freaks out accessibility.  E.g., app tried to expand something
	// that was already expanded.  Explorer Band does this when you navigate.
	MyNotifyWinEvent(EVENT_OBJECT_STATECHANGE, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
		(LONG_PTR)hItem);

	return TRUE;
}

BOOL PASCAL BetweenItems(PTREE pTree, HTREEITEM hItem, HTREEITEM hItemStart, HTREEITEM hItemEnd)
{
	if (hItemStart)
	{
		while ((hItemStart = TV_GetNextVisItem(hItemStart)) && (hItemEnd != hItemStart))
		{
			if (hItem == hItemStart)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

#ifdef  FE_IME
// Now only Korean version is interested in incremental search with composition string.

#define FREE_COMP_STRING(pszCompStr)    LocalFree((HLOCAL)(pszCompStr))

BOOL TV_OnImeComposition(PTREE pTree, WPARAM wParam, LPARAM lParam)
{
	LPTSTR lpsz;
	int iCycle = 0;
	HTREEITEM hItem;
	TCHAR szTemp[MAXLABELTEXT];
	XTVITEM ti;
	LPTSTR lpszAlt = NULL; // use only if SameChar
	int iLen;
	HIMC hImc;
	TCHAR *pszCompStr;
	BOOL fRet = TRUE;

	if (hImc = ImmGetContext(GetHostWnd(pTree->m_hWnd)))
	{
		if (lParam & GCS_RESULTSTR)
		{
			fRet = FALSE;
			pszCompStr = GET_COMP_STRING(hImc, GCS_RESULTSTR);
			if (pszCompStr)
			{
				IncrementSearchImeCompStr(&pTree->is, FALSE, pszCompStr, &lpsz);
				FREE_COMP_STRING(pszCompStr);
			}
		}
		if (lParam & GCS_COMPSTR)
		{
			fRet = TRUE;
			pszCompStr = GET_COMP_STRING(hImc, GCS_COMPSTR);
			if (pszCompStr)
			{
				if (IncrementSearchImeCompStr(&pTree->is, TRUE, pszCompStr, &lpsz))
				{
					if (pTree->m_hCaretItem)
					{
						pTree->m_hSearchItem = pTree->m_hCaretItem;
					}
					else if (pTree->m_hRootItem && pTree->m_hRootItem->hKids)
					{
						pTree->m_hSearchItem = pTree->m_hRootItem->hKids;
					}
					else
					{
						return fRet;
					}
				}

				if (!lpsz || !*lpsz || !pTree->m_hRootItem || !pTree->m_hRootItem->hKids)
				{
					return fRet;
				}

				hItem = pTree->m_hSearchItem;
				ti.cchTextMax  = sizeof(szTemp);
				iLen = lstrlen(lpsz);
#ifdef UNICODE
				if (iLen > 1 && SameChars(lpsz, lpsz[0]))
				{
					lpszAlt = lpsz + iLen - 1;
				}
#else
				if (iLen > 2 && SameDBCSChars(lpsz, (WORD)((BYTE)lpsz[0] << 8 | (BYTE)lpsz[1])))
				{
					lpszAlt = lpsz + iLen - 2;
				}
#endif

				do
				{
					ti.pszText = szTemp;
					hItem = TV_GetNextVisItem(hItem);
					if (!hItem)
					{
						iCycle++;
						hItem = pTree->m_hRootItem->hKids;
					}

					TV_GetItem(pTree, hItem, TVIF_TEXT, &ti);
					if ((ti.pszText != LPSTR_TEXTCALLBACK) &&
						HIWORD64(ti.pszText))
					{
						// DebugMsg(DM_TRACE, "treesearch %d %s %s", (LPSTR)lpsz, (LPSTR)lpsz, (LPSTR)ti.pszText);
						if (IntlStrEqNI(lpsz, ti.pszText, iLen) ||
#ifdef UNICODE
							(lpszAlt && IntlStrEqNI(lpszAlt, ti.pszText, 1) &&
#else
							(lpszAlt && IntlStrEqNI(lpszAlt, ti.pszText, 2) &&
#endif
								BetweenItems(pTree, hItem, pTree->m_hCaretItem, pTree->m_hSearchItem)))
						{
							DebugMsg(DM_TRACE, TEXT("Selecting"));
							TV_SelectItem(pTree, TVGN_CARET, hItem, TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_BYKEYBOARD);
#ifdef KEYBOARDCUES
							//notify of navigation key usage
							CCNotifyNavigationKeyUsage(&(pTree->m_hWnd->ci), UISF_HIDEFOCUS);
#endif
							return fRet;
						}
					}
				}
				while (iCycle < 2);

				// if they hit the same key twice in a row at the beginning of
				// the search, and there was no item found, they likely meant to
				// retstart the search
				if (lpszAlt)
				{

					// first clear out the string so that we won't recurse again
					IncrementSearchString(&pTree->is, 0, NULL);
					TV_OnImeComposition(pTree, wParam, lParam);
				}
				else
				{
					IncrementSearchBeep(&pTree->is);
				}
#ifdef KEYBOARDCUES
				//notify of navigation key usage
				CCNotifyNavigationKeyUsage(&(pTree->m_hWnd->ci), UISF_HIDEFOCUS);
#endif
				FREE_COMP_STRING(pszCompStr);
			}
		}
		ImmReleaseContext(GetHostWnd(pTree->m_hWnd), hImc);
	}
	return fRet;
}
#endif


void TV_OnChar(PTREE pTree, UINT ch, int cRepeat)
{
	LPTSTR lpsz = NULL;
	int iCycle = 0;
	HTREEITEM hItem;
	TCHAR szTemp[MAXLABELTEXT];
	XTVITEM ti;
	LPTSTR lpszAlt = NULL; // use only if SameChar
	int iLen;

	if (IncrementSearchString(&pTree->is, ch, &lpsz) || !pTree->m_hSearchItem)
	{
		if (pTree->m_hCaretItem)
		{
			pTree->m_hSearchItem = pTree->m_hCaretItem;
		}
		else if (pTree->m_hRootItem && pTree->m_hRootItem->hKids)
		{
			pTree->m_hSearchItem = pTree->m_hRootItem->hKids;
		}
		else
		{
			return;
		}
	}

	if (!lpsz || !*lpsz || !pTree->m_hRootItem || !pTree->m_hRootItem->hKids)
	{
		return;
	}

	hItem = pTree->m_hSearchItem;
	ti.cchTextMax  = ARRAYSIZE(szTemp);
	iLen = lstrlen(lpsz);
	if (iLen > 1 && SameChars(lpsz, lpsz[0]))
	{
		lpszAlt = lpsz + iLen - 1;
	}

	do
	{
		ti.pszText = szTemp;
		hItem = TV_GetNextVisItem(hItem);
		if (!hItem)
		{
			iCycle++;
			hItem = pTree->m_hRootItem->hKids;
		}

		TV_GetItem(pTree, hItem, TVIF_TEXT, &ti);
		if ((ti.pszText != LPSTR_TEXTCALLBACK) &&
			HIWORD64(ti.pszText))
		{
			// DebugMsg(DM_TRACE, TEXT("treesearch %d %s %s"), (LPTSTR)lpsz, (LPTSTR)lpsz, (LPTSTR)ti.pszText);
			if (IntlStrEqNI(lpsz, ti.pszText, iLen) ||
				(lpszAlt && IntlStrEqNI(lpszAlt, ti.pszText, 1) &&
					BetweenItems(pTree, hItem, pTree->m_hCaretItem, pTree->m_hSearchItem)))
			{
				//L DebugMsg(DM_TRACE, TEXT("Selecting"));
				TV_SelectItem(pTree, TVGN_CARET, hItem, TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_BYKEYBOARD);
#ifdef KEYBOARDCUES
				//notify of navigation key usage
				CCNotifyNavigationKeyUsage(&(pTree->m_hWnd->ci), UISF_HIDEFOCUS);
#endif
				return;
			}
		}
	}
	while (iCycle < 2);

	// if they hit the same key twice in a row at the beginning of
	// the search, and there was no item found, they likely meant to
	// retstart the search
	if (lpszAlt)
	{

		// first clear out the string so that we won't recurse again
		IncrementSearchString(&pTree->is, 0, NULL);
		TV_OnChar(pTree, ch, cRepeat);
	}
	else
	{
		IncrementSearchBeep(&pTree->is);
	}
#ifdef KEYBOARDCUES
	//notify of navigation key usage
	CCNotifyNavigationKeyUsage(&(pTree->m_hWnd->ci), UISF_HIDEFOCUS);
#endif
}

// ----------------------------------------------------------------------------
//
//  WM_KEYDOWN消息响应函数
//  如果Ctrl键被按下，把按键当成滚动作用；否则改变caret（焦点Item）位置
//
// ----------------------------------------------------------------------------
BOOL TV_KeyDown(PTREE pTree, WPARAM wKey, LPARAM dwKeyData)
{
	BOOL ret = TRUE;

	// 发送TVN_KEYDOWN事件
	TV_KEYDOWN nm;
	nm.wVKey = (WORD)wKey;
	BOOL fPuntChar = (BOOL)CCSendNotify(&pTree->m_hWnd->ci, TVN_KEYDOWN, &nm.hdr);

	wKey = RTLSwapLeftRightArrows(&pTree->m_hWnd->ci, wKey);

	if (GetKeyState(VK_CONTROL) < 0)
	{
		// Ctrl键被按下
		UINT wScrollCode;

		switch (wKey)
		{
		case VK_LEFT:
			TV_HorzScroll(pTree, SB_LINEUP, 0);
			break;

		case VK_RIGHT:
			TV_HorzScroll(pTree, SB_LINEDOWN, 0);
			break;

		case VK_PRIOR:
			wScrollCode = SB_PAGEUP;
			goto kdVertScroll;

		case VK_HOME:
			wScrollCode = SB_TOP;
			goto kdVertScroll;

		case VK_NEXT:
			wScrollCode = SB_PAGEDOWN;
			goto kdVertScroll;

		case VK_END:
			wScrollCode = SB_BOTTOM;
			goto kdVertScroll;

		case VK_UP:
			wScrollCode = SB_LINEUP;
			goto kdVertScroll;

		case VK_DOWN:
			wScrollCode = SB_LINEDOWN;
kdVertScroll:
			TV_VertScroll(pTree, wScrollCode, 0);
			break;

		default:
			ret = FALSE;
		}

	}
	else
	{
		TREEITEM *hItem = NULL;
		UINT wShownIndex = 0;

		switch (wKey)
		{
		case VK_RETURN:
			fPuntChar = (BOOL)CCSendNotify(&pTree->m_hWnd->ci, NM_RETURN, NULL);
			break;

		case VK_PRIOR:
			if (pTree->m_hCaretItem && (pTree->m_hCaretItem->iShownIndex > (pTree->cFullVisible - 1)))
			{
				wShownIndex = pTree->m_hCaretItem->iShownIndex - (pTree->cFullVisible - 1);
				goto selectIndex;
			}
		// fall thru

		case VK_HOME:
			wShownIndex = 0;
			goto selectIndex;

		case VK_NEXT:
			if (!pTree->m_hCaretItem)
			{
				wShownIndex = 0;
				goto selectIndex;
			}
			wShownIndex = pTree->m_hCaretItem->iShownIndex + (pTree->cFullVisible - 1);
			if (wShownIndex < pTree->cShowing)
			{
				goto selectIndex;
			}
		// fall thru

		case VK_END:
			wShownIndex = pTree->cShowing - 1;
selectIndex:
			hItem = TV_GetShownIndexItem(pTree, pTree->m_hRootItem->hKids, wShownIndex);
			goto kdSetCaret;
			break;

		case VK_SUBTRACT:
			if (pTree->m_hCaretItem)
			{
				fPuntChar = TRUE;
				TV_Expand(pTree, TVE_COLLAPSE, pTree->m_hCaretItem, TRUE);
			}
			break;

		case VK_ADD:
			if (pTree->m_hCaretItem)
			{
				fPuntChar = TRUE;
				TV_Expand(pTree, TVE_EXPAND, pTree->m_hCaretItem, TRUE);
			}
			break;

		case VK_MULTIPLY:
			if (pTree->m_hCaretItem)
			{
				fPuntChar = TRUE;
				TV_ExpandRecurse(pTree, pTree->m_hCaretItem, TRUE);
			}
			break;

		case VK_LEFT:
			if (pTree->m_hCaretItem && (pTree->m_hCaretItem->state & TVIS_EXPANDED))
			{
				TV_Expand(pTree, TVE_COLLAPSE, pTree->m_hCaretItem, TRUE);
				break;
			}
			else if (pTree->m_hCaretItem)
			{
				hItem = VISIBLE_PARENT(pTree->m_hCaretItem);
				goto kdSetCaret;
			}
			break;

		case VK_BACK:
			// get the parent, avoiding the root item
			fPuntChar = TRUE;
			if (pTree->m_hCaretItem)
			{
				hItem = VISIBLE_PARENT(pTree->m_hCaretItem);
				goto kdSetCaret;
			}
			break;

		case VK_UP:
			if (pTree->m_hCaretItem)
			{
				hItem = TV_GetPrevVisItem(pTree->m_hCaretItem);
			}
			else
			{
				hItem = pTree->m_hRootItem->hKids;
			}

			goto kdSetCaret;
			break;


		case VK_RIGHT:
			if (pTree->m_hCaretItem && !(pTree->m_hCaretItem->state & TVIS_EXPANDED))
			{
				TV_Expand(pTree, TVE_EXPAND, pTree->m_hCaretItem, TRUE);
				break;
			} // else fall through

		case VK_DOWN:
			if (pTree->m_hCaretItem != NULL)
			{
				hItem = TV_GetNextVisItem(pTree->m_hCaretItem);
			}
			else
			{
				hItem = pTree->m_hRootItem->hKids;
			}

kdSetCaret:
			if (hItem != NULL)
			{
				TV_SelectItem(pTree, TVGN_CARET, hItem,
					TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_BYKEYBOARD);
			}

			break;

		case VK_SPACE:
			if ((pTree->m_hWnd->ci.style & TVS_CHECKBOXES) && pTree->m_hCaretItem)
			{
				TV_HandleStateIconClick(pTree, pTree->m_hCaretItem);
				fPuntChar = TRUE; // don't beep
			}
			break;

		default:
			ret = FALSE;
		}
	}

	if (fPuntChar)
	{
		pTree->m_nPuntChar++;
	}
	else if (pTree->m_nPuntChar != 0)
	{
		// this is tricky...  if we want to punt the char, just increment the
		// count.  if we do NOT, then we must clear the queue of WM_CHAR's
		// this is to preserve the m_nPuntChar to mean "punt the next n WM_CHAR messages
		MSG msg;
		while ((pTree->m_nPuntChar > 0)
			&& PeekMessage(&msg, GetHostWnd(pTree->m_hWnd), WM_CHAR, WM_CHAR, PM_REMOVE))
		{
			pTree->m_nPuntChar--;
		}
		_ASSERT(pTree->m_nPuntChar == 0);
	}

#ifdef KEYBOARDCUES
	if (VK_MENU != wKey)
	{
		//notify of navigation key usage
		CCNotifyNavigationKeyUsage(&(pTree->m_hWnd->ci), UISF_HIDEFOCUS);
	}
#endif

	return ret;
}


// ----------------------------------------------------------------------------
//
//  Sets the tree's indent width per hierarchy level and recompute widths.
//
//  sets cxIndent
//
// ----------------------------------------------------------------------------

void TV_SetIndent(PTREE pTree, WPARAM cxIndent)
{
	if (pTree->m_hImageList)
	{
		if ((SHORT)cxIndent < pTree->cxImage)
		{
			cxIndent = pTree->cxImage;
		}
	}

	if ((SHORT)cxIndent < pTree->cyText)
	{
		cxIndent = pTree->cyText;
	}

	if (cxIndent < TREE_MININDENT)
	{
		cxIndent = TREE_MININDENT;
	}

	pTree->cxIndent = (SHORT)cxIndent;

	TV_CreateIndentBmps(pTree);
	TV_ScrollBarsAfterSetWidth(pTree, NULL);
}

// ----------------------------------------------------------------------------
//
//  Sets the tree's item height to be the maximum of the image height and text
//  height.  Then recompute the tree's full visible count.
//
//  sets cyItem, cFullVisible
//
// ----------------------------------------------------------------------------

void TV_SetItemHeight(PTREE pTree)
{
	// 当TreeView包含TVS_HASLINES风格时，Item高度必须是偶数。
	// 这是因为绘制的Item的连接线是虚线，只有偶数的Item行高，才能完美连接上下
	// 两个Item的虚线。
	if (!pTree->m_bCyItemSet)
	{
		pTree->cyItem = (max(pTree->cyImage, pTree->cyText) + 1);
	}
	// height not always even not, only on haslines style.
	if (pTree->cyItem <= 1)
	{
		pTree->cyItem = 1;          // Don't let it go zero or negative!
	}
	else if (!(pTree->m_hWnd->ci.style & TVS_NONEVENHEIGHT))
	{
		pTree->cyItem &= ~1;
	}

	if (pTree->m_bSmoothVScroll)
	{
		pTree->cFullVisible = pTree->cyWnd;
	}
	else
	{
		pTree->cFullVisible = pTree->cyWnd / pTree->cyItem;
	}

	TV_CreateIndentBmps(pTree);
	TV_CalcScrollBars(pTree);
}

// BUGBUG: does not deal with hfont == NULL

void TV_OnSetFont(PTREE pTree, HFONT hNewFont, BOOL fRedraw)
{
	HDC hdc;
	HFONT hfontSel;
	TCHAR c = TEXT('J');       // for bog
	SIZE size;

	if (pTree->m_bCreatedFont && pTree->hFont)
	{
		DeleteObject(pTree->hFont);
		pTree->hFont = NULL;
		pTree->m_bCreatedFont = FALSE;
	}

	if (hNewFont == NULL)
	{
		LOGFONT lf;
		SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, FALSE);
#ifdef WINNT
		// B#210235 - because NT4 initializes icontitle logfont with Ansi charset
		// no matter what font is selected, following A/W conversion would fail
		// on non US environment if we use this logfont to get codepage.
		// The ACP is guaranteed to work with any Ansi apps because these apps
		// assume ACP to be matching to their desired codepage.
		if (staticIsOS(OS_NT4) && !staticIsOS(OS_NT5))
		{
			CHARSETINFO csi;
			TranslateCharsetInfo((DWORD *)g_uiACP, &csi, TCI_SRCCODEPAGE);
			lf.lfCharSet = (BYTE)csi.ciCharset;
		}
#endif
		hNewFont = CreateFontIndirect(&lf);
		pTree->m_bCreatedFont = TRUE;         // make sure we delete it
	}

	hdc = GetDC(GetHostWnd(pTree->m_hWnd));

	hfontSel = hNewFont ? (HFONT)SelectObject(hdc, hNewFont) : NULL;

	// Office9 Setup had a bug where they installed a bogus font,
	// which created okay but all APIs against it (e.g., GetTextExtentPoint)
	// failed!  Protect against failure by pre-setting the value to something
	// non-garbage.
	size.cy = 0;
	GetTextExtentPoint(hdc, &c, 1, &size);
	pTree->cyText = (SHORT)(size.cy + (IUIGetGloabelVar()->g_cyBorder * 2));

	if (hfontSel)
	{
		SelectObject(hdc, hfontSel);
	}

	ReleaseDC(GetHostWnd(pTree->m_hWnd), hdc);

	pTree->hFont = hNewFont;
	if (pTree->hFontBold)
	{
		TV_CreateBoldFont(pTree);
	}
	pTree->m_hWnd->ci.uiCodePage = GetCodePageForFont(hNewFont);

	TV_DeleteHotFonts(pTree);

	if (pTree->cxIndent == 0)   // first time init?
	{
		if (!pTree->cyItem)
		{
			pTree->cyItem = pTree->cyText;
		}
		TV_SetIndent(pTree, 16 /*g_cxSmIcon*/ + TREE_INDENT);
	}

	TV_ScrollBarsAfterSetWidth(pTree, NULL);
	TV_SetItemHeight(pTree);

	if (pTree->hwndToolTips)
	{
		SendMessage(pTree->hwndToolTips, WM_SETFONT, (WPARAM)pTree->hFont, (LPARAM)TRUE);
	}

	// REVIEW: does this happen as a result of the above?
	if (fRedraw)
		IUI::InvalidateRect(pTree->m_hWnd, NULL);
}

VOID PASCAL TV_CreateBoldFont(PTREE pTree)
{
	LOGFONT lf;

	if (pTree->hFontBold)
	{
		DeleteObject(pTree->hFontBold);
	}

	GetObject(pTree->hFont, sizeof(lf), &lf);
	lf.lfWeight = FW_BOLD;
	pTree->hFontBold = CreateFontIndirect(&lf);
}


HIMAGELIST TV_SetImageList(PTREE pTree, HIMAGELIST hImage, int iImageIndex)
{
	int cx, cy;
	HIMAGELIST hImageOld = NULL;

	switch (iImageIndex)
	{

	case TVSIL_STATE:

		hImageOld = pTree->m_hImlState;
		pTree->m_hImlState = hImage;
		if (hImage)
		{
			ImageList_GetIconSize(hImage, &pTree->m_cxState, &pTree->m_cyState);
		}
		else
		{
			pTree->m_cxState = 0;
		}
		break;

	case TVSIL_NORMAL:
		hImageOld = pTree->m_hImageList;
		if (hImage && ImageList_GetIconSize(hImage, &cx, &cy))
		{
			pTree->cxImage = (cx + TREE_INDENT);
			pTree->cyImage = (SHORT)cy;
			if (pTree->cxIndent < pTree->cxImage)
			{
				TV_SetIndent(pTree, pTree->cxImage);
			}
			pTree->m_hImageList = hImage;

			if (!hImageOld && pTree->m_hWnd->ci.style & TVS_CHECKBOXES)
			{
				TV_InitCheckBoxes(pTree);
			}
		}
		else
		{
			pTree->cxImage = pTree->cyImage = 0;
			pTree->m_hImageList = NULL;
		}
		break;

	default:
		//L DebugMsg(DM_TRACE, TEXT("sh TR - TVM_SETIMAGELIST: unrecognized iImageList"));
		break;

	}

	TV_ScrollBarsAfterSetWidth(pTree, NULL);
	TV_SetItemHeight(pTree);

	return hImageOld;
}


// ----------------------------------------------------------------------------
//
//  Gets the item with the described relationship to the given item, NULL if
//  no item can be found with that relationship.
//
// ----------------------------------------------------------------------------

TREEITEM *TV_GetNextItem(PTREE pTree, TREEITEM *hItem, WPARAM wGetCode)
{
	switch (wGetCode)
	{
	case TVGN_ROOT:
		return pTree->m_hRootItem->hKids;

	case TVGN_DROPHILITE:
		return pTree->m_hDropTargetItem;

	case TVGN_CARET:
		return pTree->m_hCaretItem;

	case TVGN_FIRSTVISIBLE:
		return pTree->hTop;

	case TVGN_LASTVISIBLE:
		return TV_GetShownIndexItem(pTree, pTree->m_hRootItem->hKids, pTree->cShowing - 1);

	case TVGN_CHILD:
		if (!hItem || (hItem == TVI_ROOT))
		{
			return pTree->m_hRootItem->hKids;
		}
		break;
	}

	// all of these require a valid hItem
	if (!ValidateTreeItem(hItem, 0))
	{
		return NULL;
	}

	switch (wGetCode)
	{
	case TVGN_NEXTVISIBLE:
		return TV_GetNextVisItem(hItem);

	case TVGN_PREVIOUSVISIBLE:
		return TV_GetPrevVisItem(hItem);

	case TVGN_NEXT:
		return hItem->hNext;

	case TVGN_PREVIOUS:
		if (hItem->hParent->hKids == hItem)
		{
			// 如果父Item只有自己一个孩子，则返回NULL
			return NULL;
		}
		else
		{
			TREEITEM *hWalk;
			for (hWalk = hItem->hParent->hKids; hWalk->hNext != hItem; hWalk = hWalk->hNext)
			{
				;
			}
			return hWalk;
		}

	case TVGN_PARENT:
		return VISIBLE_PARENT(hItem);

	case TVGN_CHILD:
		return hItem->hKids;
	}

	return NULL;
}


// ----------------------------------------------------------------------------
//
//  Returns the number of items (including the partially visible item at the
//  bottom based on the given flag) that fit in the tree's client window.
//
// ----------------------------------------------------------------------------

LRESULT TV_GetVisCount(PTREE pTree, BOOL fIncludePartial)
{
	int  i;

	if (!fIncludePartial)
	{
		return (MAKELRESULTFROMUINT(pTree->cFullVisible));
	}

	i = pTree->cFullVisible;

	if (pTree->cyWnd - (i * pTree->cyItem))
	{
		i++;
	}

	return i;
}


void TV_InvalidateInsertMarkRect(PTREE pTree, BOOL fErase)
{
	RECT rc;
	if (TV_GetInsertMarkRect(pTree, &rc))
	{
		InvalidateRect(pTree->m_hWnd, &rc);
	}
}

// ----------------------------------------------------------------------------
//
//  以TreeView客户端尺寸，重新计算TREE的cxWnd, cyWnd, cFullVisible属性
//
// ----------------------------------------------------------------------------
BOOL TV_SizeWnd(PTREE pTree, UINT cxWnd, UINT cyWnd)
{
	RECT rc;
	UINT cxOld = pTree->cxWnd;

	// 如果cxWnd或cyWnd为0，则重新取客户端尺寸
	if (cxWnd == 0 || cyWnd == 0)
	{
		GetClientRect(pTree->m_hWnd, &rc);
		cxWnd = rc.right;
		cyWnd = rc.bottom;
	}
	pTree->cxWnd = (SHORT)cxWnd;
	pTree->cyWnd = (SHORT)cyWnd;
	if (pTree->m_bSmoothVScroll)
	{
		pTree->cFullVisible = cyWnd;
	}
	else
	{
		pTree->cFullVisible = cyWnd / pTree->cyItem;
	}

	if (pTree->m_hWnd->ci.style & TVS_NOSCROLL)
	{
		pTree->cxMax = (WORD) cxWnd;
	}

	TV_CalcScrollBars(pTree);
	if (pTree->cxBorder)
	{
		rc.top = 0;
		rc.bottom = cyWnd;
		rc.right = cxOld;
		rc.left = cxOld - pTree->cxBorder;
		if (rc.left < (int)cxWnd)
		{
			// invalidate so clipping happens on right on size.
			InvalidateRect(pTree->m_hWnd, &rc);
		}
	}

	TV_InvalidateInsertMarkRect(pTree, TRUE);

	return TRUE;
}


void TV_HandleStateIconClick(PTREE pTree, HTREEITEM hItem)
{
	XTVITEM tvi;
	int iState;

	tvi.stateMask = TVIS_STATEIMAGEMASK;
	TV_GetItem(pTree, hItem, TVIF_STATE, &tvi);

	iState = STATEIMAGEMASKTOINDEX(tvi.state & tvi.stateMask);
	iState %= (ImageList_GetImageCount(pTree->m_hImlState) - 1);
	iState++;

	tvi.mask = TVIF_STATE;
	tvi.state = INDEXTOSTATEIMAGEMASK(iState);
	tvi.hItem = hItem;
	TV_SetItem(pTree, &tvi);

}


//
//  Eudora is a pile of crap.
//
//  When they get a NM_DBLCLK notification from a treeview, they say,
//  "Oh, I know that treeview allocates its NMHDR from the stack, and
//  there's this local variable on Treeview's stack I'm really interested
//  in, so I'm going to hard-code an offset from the pnmhdr and read the
//  DWORD at that location so I can get at the local variable.  I will then
//  stop working if this value is zero."
//
//  The conversion to UNICODE changed our stack layout enough that they
//  end up always getting zero -- it's the NULL parameter which is the
//  final argument to CCSendNotify.  Since all this stack layout stuff is
//  sensitive to how the compiler's optimizer feels today, we create a
//  special notify structure Just For Eudora which mimics the stack layout
//  they expected to see in Win95.
//
typedef struct NMEUDORA
{
	NMHDR   nmhdr;
	BYTE    Padding[48];
	DWORD   MustBeNonzero;      // Eudora fails to install if this is zero
} NMEUDORA;

// ----------------------------------------------------------------------------
//
//  WM_LBUTTONDBLCLK message -- toggle expand/collapse state of item's children
//  WM_LBUTTONDOWN message -- on item's button, do same as WM_LBUTTONDBLCLK,
//  otherwise select item and ensure that item is fully visible
//
// ----------------------------------------------------------------------------

void TV_ButtonDown(PTREE pTree, UINT wMsg, UINT wFlags, int x, int y, UINT TVBD_flags)
{
	UINT wHitCode;
	TREEITEM *hItem;
	LRESULT lResult;
#ifdef _X86_
	NMEUDORA nmeu;
	nmeu.MustBeNonzero = 1;
	COMPILETIME_ASSERT(FIELD_OFFSET(NMEUDORA, MustBeNonzero) == 0x3C);
#endif

	GetMessagePosClient(pTree->m_hWnd, &pTree->ptCapture);

	if (!TV_DismissEdit(pTree, FALSE))   // end any previous editing (accept it)
	{
		return;    // Something happened such that we should not process button down
	}


	hItem = TV_CheckHit(pTree, x, y, &wHitCode);

	// Excel likes to destroy the entire tree when it gets a double-click
	// so we need to watch the item in case it vanishes behind our back.

	if (wMsg == WM_LBUTTONDBLCLK)
	{
		//
		// Cancel any name editing that might happen.
		//

		TV_CancelEditTimer(pTree);

		if (wHitCode & (TVHT_ONITEM | TVHT_ONITEMBUTTON))
		{
			goto ExpandItem;
		}

		//
		// Collapses node above the line double clicked on
		//
		else if ((pTree->m_hWnd->ci.style & TVS_HASLINES) && (wHitCode & TVHT_ONITEMINDENT) &&
			(abs(x % pTree->cxIndent - pTree->cxIndent / 2) <= IUIGetGloabelVar()->g_cxDoubleClk))
		{

			int i;

			for (i = hItem->iLevel - x / pTree->cxIndent + ((pTree->m_hWnd->ci.style & TVS_LINESATROOT) ? 1 : 0); i > 1; i--)
			{
				hItem = hItem->hParent;
			}

ExpandItem:
#ifdef _X86_
			lResult = CCSendNotify(&pTree->m_hWnd->ci, wFlags & MK_RBUTTON ? NM_RDBLCLK : NM_DBLCLK, &nmeu.nmhdr);
#else
			lResult = CCSendNotify(&pTree->m_hWnd->ci, wFlags & MK_RBUTTON ? NM_RDBLCLK : NM_DBLCLK, NULL);
#endif
			if (!IsWindow(pTree->m_hWnd))
			{
				goto bail;
			}
			if (!lResult)
			{
				// don't auto expand this if we're in single expand mode because the first click did it already
				if (!(pTree->m_hWnd->ci.style & TVS_SINGLEEXPAND))
				{
					TV_Expand(pTree, TVE_TOGGLE, hItem, TRUE);
				}
			}

		}

		pTree->m_bScrollWait = FALSE;

	}
	else        // WM_LBUTTONDOWN
	{

		if (wHitCode == TVHT_ONITEMBUTTON)
		{
			if (!CCSendNotify(&pTree->m_hWnd->ci, NM_CLICK, NULL))
			{
				if (TVBD_flags & TVBD_FROMWHEEL)
				{
					TV_Expand(pTree, (TVBD_flags & TVBD_WHEELFORWARD) ? TVE_EXPAND : TVE_COLLAPSE, hItem, TRUE);
				}
				else
				{
					TV_Expand(pTree, TVE_TOGGLE, hItem, TRUE);
				}
			}
		}
		else if (wHitCode & TVHT_ONITEM ||
			((pTree->m_hWnd->ci.style & TVS_FULLROWSELECT) && (wHitCode & (TVHT_ONITEMRIGHT | TVHT_ONITEMINDENT))))
		{
			_ASSERT(hItem != NULL);

			BOOL fSameItem = (hItem == pTree->m_hCaretItem);

			BOOL bDragging = FALSE;
			if (TVBD_flags & TVBD_FROMWHEEL)
			{
				bDragging = FALSE;
			}
			else if (pTree->m_hWnd->ci.style & TVS_DISABLEDRAGDROP)
			{
				bDragging = FALSE;
			}
			else
			{
				bDragging = TV_CheckForDragBegin(pTree, hItem, x, y);
				TV_FinishCheckDrag(pTree);
			}

			if (bDragging)
			{
				pTree->m_hDragItem = hItem;
				TV_SendBeginDrag(pTree, TVN_BEGINDRAG, hItem, x, y);
				return;
			}

			if (!CCSendNotify(&pTree->m_hWnd->ci, NM_CLICK, NULL))
			{

				if (wHitCode == TVHT_ONITEMSTATEICON &&
					(pTree->m_hWnd->ci.style & TVS_CHECKBOXES))
				{
					TV_HandleStateIconClick(pTree, hItem);
				}
				else
				{

					// Only set the caret (selection) if not dragging
					TV_SelectItem(pTree, TVGN_CARET, hItem, TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_BYMOUSE);

					if (fSameItem && (wHitCode & TVHT_ONITEMLABEL) && pTree->m_bFocus)
					{
						//
						// The item and window are currently selected and user clicked
						// on label.  Try to enter into name editing mode.
						//
						SetTimer(pTree->m_hWnd, IDT_NAMEEDIT, GetDoubleClickTime(), NULL);
						pTree->m_bNameEditPending = TRUE;
					}

					if (fSameItem && pTree->m_hWnd->ci.style & TVS_SINGLEEXPAND)
					{
						// single click on the focus item toggles expand state
						TV_Expand(pTree, TVE_TOGGLE, pTree->m_hCaretItem, TRUE);
					}
				}
			}
		}
		else
		{
			CCSendNotify(&pTree->m_hWnd->ci, NM_CLICK, NULL);
		}
	}

	if (!pTree->m_bFocus)
	{
		SetFocus(GetHostWnd(pTree->m_hWnd), pTree->m_hWnd);
	}

bail:
	;
}


// ----------------------------------------------------------------------------
//
//  Gets the item's text, data, and/or image.
//
// ----------------------------------------------------------------------------
BOOL TV_OnGetItem(PTREE pTree, LPXTVITEM ptvi)
{
	if (!ptvi)
	{
		return FALSE;
	}

	if (!ValidateTreeItem(ptvi->hItem, 0))
	{
		return FALSE;    // Invalid parameter
	}

	TV_GetItem(pTree, ptvi->hItem, ptvi->mask, ptvi);

	return TRUE;        // success
}

#ifdef UNICODE
BOOL TV_OnGetItemA(PTREE pTree, LPTVITEMEXA ptvi)
{
	BOOL bRet;
	LPSTR pszA = NULL;
	LPWSTR pszW = NULL;

	//HACK Alert!  This code assumes that TVITEMA is exactly the same
	// as TVITEMW except for the text pointer in the XTVITEM
	_ASSERT(sizeof(TVITEMA) == sizeof(TVITEMW));

	if (!IsFlagPtr(ptvi) && (ptvi->mask & TVIF_TEXT) && !IsFlagPtr(ptvi->pszText))
	{
		pszA = ptvi->pszText;
		pszW = (LPTSTR)LocalAlloc(LMEM_FIXED, ptvi->cchTextMax * sizeof(WCHAR));
		if (pszW == NULL)
		{
			return FALSE;
		}
		ptvi->pszText = (LPSTR)pszW;
	}
	bRet = TV_OnGetItem(pTree, (LPXTVITEMW)ptvi);
	if (pszA)
	{
		if (bRet && ptvi->cchTextMax)
		{
			ConvertWToAN(pTree->m_hWnd->ci.uiCodePage, pszA, ptvi->cchTextMax, (LPWSTR)(ptvi->pszText), -1);
		}
		LocalFree(pszW);
		ptvi->pszText = pszA;
	}
	return bRet;
}
#endif

// ----------------------------------------------------------------------------
//
//  Sets the item's text, data, and/or image.
//
// ----------------------------------------------------------------------------

#ifdef UNICODE
BOOL TV_SetItemA(PTREE pTree, LPTVITEMEXA ptvi)
{
	LPSTR pszA = NULL;
	BOOL lRet;

	//HACK Alert!  This code assumes that TVITEMA is exactly the same
	// as TVITEMW except for the text pointer in the XTVITEM
	_ASSERT(sizeof(TVITEMA) == sizeof(TVITEMW));

	if (!IsFlagPtr(ptvi) && (ptvi->mask & TVIF_TEXT) && !IsFlagPtr(ptvi->pszText))
	{
		pszA = ptvi->pszText;
		ptvi->pszText = (LPSTR)ProduceWFromA(pTree->m_hWnd->ci.uiCodePage, pszA);

		if (ptvi->pszText == NULL)
		{
			ptvi->pszText = pszA;
			return -1;
		}
	}

	lRet = TV_SetItem(pTree, (LPCXTVITEM)ptvi);

	if (pszA)
	{
		FreeProducedString(ptvi->pszText);
		ptvi->pszText = pszA;
	}

	return lRet;
}
#endif

BOOL TV_SetItem(PTREE pTree, LPCXTVITEM ptvi)
{
	UINT uRDWFlags = RDW_INVALIDATE;
	BOOL fEraseIfTransparent = FALSE;
	HTREEITEM hItem;
	BOOL bActualChange = FALSE; // HACK: We want to keep track of which
	// attributes were changed from CALLBACK to
	// "real", and don't invalidate if those were
	// the only changes
	int iIntegralPrev;
	BOOL fName = FALSE;
	BOOL fFocusSel = FALSE;
	BOOL fRecalcWidth = FALSE;
	BOOL fStateImageChange = FALSE;

	if (!ptvi)
	{
		return FALSE;
	}

	hItem = ptvi->hItem;

	// deal with the evil invisible root for multiple root trees.
	if (hItem == TVI_ROOT)
	{
		hItem = pTree->m_hRootItem;
	}

	if (!ValidateTreeItem(hItem, 0))
	{
		return FALSE;
	}

	iIntegralPrev = TV_GetIntegral(pTree, hItem);

	// BUGBUG: send ITEMCHANING and ITEMCHANGED msgs

	if (ptvi->mask & TVIF_TEXT)
	{
		uRDWFlags = RDW_INVALIDATE | RDW_ERASE;
		bActualChange = TRUE;

		if (!ptvi->pszText)
		{
			Str_Set(&hItem->lpstr, LPSTR_TEXTCALLBACK);
		}
		else
		{
			if (!Str_Set(&hItem->lpstr, ptvi->pszText))
			{
				//
				// Memory allocation failed -  The best we can do now
				// is to set the item back to callback, and hope that
				// the top level program can handle it.
				//
				//L DebugMsg(DM_ERROR, TEXT("TreeView: Out of memory"));
				hItem->lpstr = LPSTR_TEXTCALLBACK;
			}
		}

		fRecalcWidth = TRUE;
		fName = TRUE;
	}

	if (ptvi->mask & TVIF_PARAM)
	{
		bActualChange = TRUE;
		hItem->lParam = ptvi->lParam;
	}

	if (ptvi->mask & TVIF_BINDWL)
	{
		bActualChange = TRUE;
		hItem->hBindWindowless = ptvi->hBindWindowless;
		SetBindItemControlParent(hItem->hBindWindowless, pTree->m_hWnd, (ULONG_PTR)hItem);
	}

	if (ptvi->mask & TVIF_IMAGE)
	{
		if (hItem->iImage != (WORD)I_IMAGECALLBACK)
		{
			bActualChange = TRUE;
			fEraseIfTransparent = TRUE;
			if (pTree->m_hImageList && (ImageList_GetBkColor(pTree->m_hImageList) == (COLORREF)(-1)))
			{
				uRDWFlags |= RDW_ERASE;
			}

		}
		hItem->iImage = (SHORT)ptvi->iImage;
	}

	if (ptvi->mask & TVIF_SELECTEDIMAGE)
	{
		if (hItem->iSelectedImage != (WORD)I_IMAGECALLBACK)
		{
			bActualChange = TRUE;
		}
		hItem->iSelectedImage = (SHORT)ptvi->iSelectedImage;
	}

	if (ptvi->mask & TVIF_CHILDREN)
	{
		if (hItem->fKids != KIDS_CALLBACK)
		{
			bActualChange = TRUE;
		}

		if (ptvi->cChildren == I_CHILDRENCALLBACK)
		{
			hItem->fKids = KIDS_CALLBACK;
		}
		else
		{
			if (ptvi->cChildren)
			{
				hItem->fKids = KIDS_FORCE_YES;
			}
			else
			{
				hItem->fKids = KIDS_FORCE_NO;
			}
		}

		//
		// If this item currently has no kid, reset the item.
		//
		if ((ptvi->cChildren == I_CHILDRENCALLBACK) && (hItem->hKids == NULL))
		{
			hItem->state &= ~TVIS_EXPANDEDONCE;
			if (hItem->hParent)
			{
				hItem->state &= ~TVIS_EXPANDED;
			}
		}
	}

	if (ptvi->mask & TVIF_INTEGRAL)
	{
		if (LOWORD(ptvi->iIntegral) > 0)
		{
			hItem->iIntegral = LOWORD(ptvi->iIntegral);
		}
	}

	if (ptvi->mask & TVIF_STATE)
	{
		// don't & ptvi->state with TVIS_ALL because win95 didn't
		// and setting TVIS_FOCUS was retrievable even though we don't use it
		UINT change = (hItem->state ^ ptvi->state) & ptvi->stateMask;

		if (change)
		{
			// BUGBUG: (TVIS_SELECTED | TVIS_DROPHILITED) changes
			// should effect tree state
			hItem->state ^= change;
			bActualChange = TRUE;
			fEraseIfTransparent = TRUE;

			if (hItem->state & TVIS_BOLD)
			{
				if (!pTree->hFontBold)
				{
					TV_CreateBoldFont(pTree);
				}
			}

			if (change & TVIS_BOLD)
			{
				// do this because changing the boldness
				uRDWFlags |= RDW_ERASE;
				fRecalcWidth = TRUE;
			}

			fStateImageChange = change & TVIS_STATEIMAGEMASK;
			if (fStateImageChange)
			{
				uRDWFlags |= RDW_ERASE;
				// Adding/removing a state image changes the ITEM_OFFSET
				// If old image was 0, then we are adding.
				// If new image is 0, then we are removing.
				// (If old=new, then we don't get into this code path, so we
				// don't have to worry about that case.)
				if (!(hItem->state & TVIS_STATEIMAGEMASK) || // new
					!((hItem->state ^ change) & TVIS_STATEIMAGEMASK))   // old
				{
					fRecalcWidth = TRUE;
				}
			}

			fFocusSel = ((change & TVIS_SELECTED) != 0);
		}
	}

	if (fRecalcWidth)
	{
		hItem->iWidth = 0;          // Invalidate old width
		if (TV_IsShowing(hItem))
		{
			TV_ScrollBarsAfterSetWidth(pTree, hItem);
		}
	}

	// force a redraw if something changed AND if we are not
	// inside of a paint of this guy (callbacks will set the
	// item on the paint callback to implement lazy data schemes)

	if (bActualChange && (pTree->hItemPainting != hItem))
	{
		if (fEraseIfTransparent)
		{
			if (pTree->m_hImageList)
			{
				if (ImageList_GetBkColor(pTree->m_hImageList) == CLR_NONE)
				{
					uRDWFlags |= RDW_ERASE;
				}
			}

		}

		// If item height changed, then we've got a lot of cleaning up
		// to do.
		if (TV_GetIntegral(pTree, hItem) != iIntegralPrev)
		{
			TV_ScrollBarsAfterResize(pTree, hItem, iIntegralPrev, uRDWFlags);
		}
		else
		{
			TV_InvalidateItem(pTree, hItem, uRDWFlags);
		}

		// REVIEW: we might need to update the scroll bars if the
		// text length changed!
	}

	if (bActualChange)
	{
		if (fName)
			MyNotifyWinEvent(EVENT_OBJECT_NAMECHANGE, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
				(LONG_PTR)hItem);

		if (fFocusSel)
		{
			MyNotifyWinEvent(EVENT_OBJECT_FOCUS, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
				(LONG_PTR)hItem);
			MyNotifyWinEvent(((hItem->state & TVIS_SELECTED) ?
					EVENT_OBJECT_SELECTIONADD : EVENT_OBJECT_SELECTIONREMOVE),
				GetHostWnd(pTree->m_hWnd), OBJID_CLIENT, (LONG_PTR)hItem);
		}

		if (fStateImageChange)
			MyNotifyWinEvent(EVENT_OBJECT_STATECHANGE, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
				(LONG_PTR)hItem);
	}
	return TRUE;
}

// ----------------------------------------------------------------------------
//
//  Calls TV_CheckHit to get the hit test results and then package it in a
//  structure back to the app.
//
// ----------------------------------------------------------------------------

HTREEITEM TV_OnHitTest(PTREE pTree, LPTV_HITTESTINFO lptvh)
{
	if (!lptvh)
	{
		return 0;    //BUGBUG: Validate LPTVHITTEST
	}

	lptvh->hItem = TV_CheckHit(pTree, lptvh->pt.x, lptvh->pt.y, &lptvh->flags);

	return lptvh->hItem;
}

BOOL TV_IsItemTruncated(PTREE pTree, TREEITEM *hItem, LPRECT lprc)
{
	if (TV_GetItemRect(pTree, hItem, lprc, TRUE))
	{
		lprc->left -= IUIGetGloabelVar()->g_cxEdge;
		lprc->top -= IUIGetGloabelVar()->g_cyBorder;
		if ((lprc->left + hItem->iWidth) > pTree->cxWnd)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL TV_HandleTTNShow(PTREE pTree, LPNMHDR lpnm)
{
	if (pTree->m_hToolTipItem && pTree->m_bPlaceTooltip)
	{
		LPNMTTSHOWINFO psi = (LPNMTTSHOWINFO)lpnm;
		RECT rc;
		XTVITEM item;

		// Now get the text associated with that item
		item.stateMask = TVIS_BOLD;
		TV_GetItem(pTree, pTree->m_hToolTipItem, TVIF_STATE, &item);
		SendMessage(pTree->hwndToolTips, WM_SETFONT, (WPARAM)((item.state & TVIS_BOLD) ? pTree->hFontBold : pTree->hFont), 0);

		TV_GetItemRect(pTree, pTree->m_hToolTipItem, &rc, TRUE);

		MapWindowRect(GetHostWnd(pTree->m_hWnd), HWND_DESKTOP, &rc);
		// We draw the text with margins, so take those into account too.
		// These values come from TV_DrawItem...
		rc.top += IUIGetGloabelVar()->g_cyBorder;
		rc.left += IUIGetGloabelVar()->g_cxLabelMargin;

		//
		//  At this point, (rc.left, rc.top) are the coordinates we pass
		//  to DrawText.  Ask the tooltip how we should position it so the
		//  tooltip text shows up in precisely the same location.
		//
		// BUGBUG raymondc v6: wrong coordinates if app has used TVM_SETITEMHEIGHT

		SendMessage(pTree->hwndToolTips, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc);
		SetWindowPos(pTree->hwndToolTips, NULL, rc.left, rc.top, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		// This is an inplace tooltip, so disable animation.
		psi->dwStyle |= TTS_NOANIMATE;
		// handled!
		return TRUE;
	}

	return FALSE;
}

//
//  Copy the font from the treeview item into the tooltip so the tooltip
//  shows up in the correct font.
//
BOOL TV_HandleTTCustomDraw(PTREE pTree, LPNMTTCUSTOMDRAW pnm)
{
	if (pTree->m_hToolTipItem && pTree->m_bPlaceTooltip &&
		(pnm->nmcd.dwDrawStage == CDDS_PREPAINT ||
			pnm->nmcd.dwDrawStage == CDDS_ITEMPREPAINT))
	{
		//
		//  Set up the customdraw DC to match the font of the TV item.
		//
		TVFAKEDRAW tvfd;
		DWORD dwCustom = 0;
		TreeView_BeginFakeCustomDraw(pTree, &tvfd);
		dwCustom = TreeView_BeginFakeItemDraw(&tvfd, pTree->m_hToolTipItem);

		// If client changed the font, then transfer the font
		// from our private hdc into the tooltip's HDC.  We use
		// a private HDC because we only want to let the app change
		// the font, not the colors or anything else.
		if (dwCustom & CDRF_NEWFONT)
		{
			SelectObject(pnm->nmcd.hdc, GetCurrentObject(tvfd.nmcd.nmcd.hdc, OBJ_FONT));
		}
		TreeView_EndFakeItemDraw(&tvfd);
		TreeView_EndFakeCustomDraw(&tvfd);

		// Don't return other wacky flags to TT, since all we
		// did was change the font (if even that)
		return dwCustom & CDRF_NEWFONT;

	}
	return CDRF_DODEFAULT;

}

BOOL TV_SetToolTipTarget(PTREE pTree, HTREEITEM hItem)
{
	// update the item we're showing the bubble for...
	if (pTree->m_hToolTipItem != hItem)
	{
		// the hide will keep us from flashing
		ShowWindow(pTree->hwndToolTips, SW_HIDE);
		UpdateWindow(pTree->hwndToolTips);
		pTree->m_hToolTipItem = hItem;
		SendMessage(pTree->hwndToolTips, TTM_UPDATE, 0, 0);
		return TRUE;
	}
	return FALSE;
}

TREEITEM *TV_ItemAtCursor(PTREE pTree, LPRECT prc)
{
	RECT rc;
	UINT wHitCode;
	TREEITEM *hItem;

	GetCursorPos((LPPOINT)&rc);
	ScreenToClient(pTree->m_hWnd, (LPPOINT)&rc);
	hItem = TV_CheckHit(pTree, rc.left, rc.top, &wHitCode);

	if (prc)
	{
		*prc = rc;
	}
	if (!(wHitCode & TVHT_ONITEM))
	{
		hItem = NULL;
	}

	return hItem;
}

BOOL TV_UpdateToolTipTarget(PTREE pTree)
{
	RECT rc;
	TREEITEM *hItem = TV_ItemAtCursor(pTree, &rc);

	if (!(pTree->m_hWnd->ci.style & TVS_NOTOOLTIPS)
		&& !TV_IsItemTruncated(pTree, hItem, &rc)
		&& !(pTree->m_hWnd->ci.style & TVS_INFOTIP))
	{
		hItem = NULL;
	}
	//    else if (!(pTree->m_hWnd->ci.style & TVS_NOTOOLTIPS)
	//                    || (pTree->m_hWnd->ci.style & TVS_INFOTIP))
	return TV_SetToolTipTarget(pTree, hItem);
}

BOOL TV_UpdateToolTip(PTREE pTree)
{
	if (pTree->hwndToolTips && pTree->m_bRedraw)
	{
		return (TV_UpdateToolTipTarget(pTree));
	}
	return TRUE;
}

BOOL TV_SetInsertMark(PTREE pTree, HTREEITEM hItem, BOOL fAfter)
{
	if (!ValidateTreeItem(hItem, VTI_NULLOK))   // NULL means remove insert mark
	{
		return FALSE;
	}

	TV_InvalidateInsertMarkRect(pTree, TRUE); // Make sure the old one gets erased

	pTree->m_bInsertAfter = BOOLIFY(fAfter);
	pTree->m_hInsertItem = hItem;

	TV_InvalidateInsertMarkRect(pTree, FALSE); // Make sure the new one gets drawn

	return TRUE;
}

BOOL TV_GetInfoTip(PTREE pTree, LPTOOLTIPTEXT lpttt, HTREEITEM hti, LPTSTR szBuf, int cch)
{
	NMTVGETINFOTIP git;

	szBuf[0] = 0;
	git.pszText = szBuf;
	git.cchTextMax = cch;
	git.hItem = hti;
	git.lParam = hti->lParam;

	// for folded items pszText is prepopulated with the
	// item text, clients should append to this

	CCSendNotify(&pTree->m_hWnd->ci, TVN_GETINFOTIP, &git.hdr);

	CCSetInfoTipWidth(GetHostWnd(pTree->m_hWnd), pTree->hwndToolTips);
	Str_Set(&pTree->pszTip, git.pszText);
	lpttt->lpszText = pTree->pszTip;
#ifdef WINDOWS_ME
	if (pTree->m_hWnd->ci.style & TVS_RTLREADING)
	{
		lpttt->uFlags |= TTF_RTLREADING;
	}
#endif // WINDOWS_ME
	return lpttt->lpszText && lpttt->lpszText[0];
}




void TV_HandleNeedText(PTREE pTree, LPTOOLTIPTEXT lpttt)
{
	XTVITEM tvItem;
	TCHAR szBuf[INFOTIPSIZE];
	RECT rc;
	HTREEITEM hItem;

	// No distracting tooltips while in-place editing, please
	if (pTree->m_hEditingItem)
	{
		return;
	}

	// If the cursor isn't over anything, then stop
	hItem = TV_ItemAtCursor(pTree, &rc);
	if (!hItem)
	{
		return;
	}

	// If the item has an infotip, then use it
	if (pTree->m_hWnd->ci.style & TVS_INFOTIP)
	{
		if (hItem && TV_GetInfoTip(pTree, lpttt, hItem, szBuf, ARRAYSIZE(szBuf)))
		{
			pTree->m_bPlaceTooltip = FALSE;
			pTree->m_hToolTipItem = hItem;
			return;
		}
	}

	// Else it isn't an infotip
	CCResetInfoTipWidth(GetHostWnd(pTree->m_hWnd), pTree->hwndToolTips);

	// If the item is not truncated, then no need for a tooltip
	if (!TV_IsItemTruncated(pTree, hItem, &rc))
	{
		tvItem.hItem = NULL;
		return;
	}

	// Display an in-place tooltip for the item
	pTree->m_bPlaceTooltip = TRUE;
	pTree->m_hToolTipItem = hItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_TEXT | TVIF_STATE;
	tvItem.pszText = szBuf;
	tvItem.stateMask = TVIS_DROPHILITED | TVIS_SELECTED;
	COMPILETIME_ASSERT(MAXLABELTEXT <= ARRAYSIZE(szBuf));
	tvItem.cchTextMax = MAXLABELTEXT;
	TV_OnGetItem(pTree, &tvItem);

	Str_Set(&pTree->pszTip, tvItem.pszText);
	lpttt->lpszText = pTree->pszTip;
	//L DebugMsg(DM_TRACE, TEXT("TV_HandleNeedText for %d returns %s"), tvItem.hItem, lpttt->szText);
}

#ifdef UNICODE
//
//  Visual Studio 5.0 Books Online (part of VB 5.0) subclasses
//  us and responds NFR_ANSI, so we end up getting TTN_NEEDTEXTA
//  instead of TTN_NEEDTEXTW.  We can't risk forcing the tooltip
//  to UNICODE because some other apps may have done this on purpose
//  (because they intend to intercept TTN_NEEDTEXTA and do custom tooltips).
//  So support the ANSI tooltip notification so VB stays happy.
//  Note: This doesn't have to be efficient, as it's an error case anyway.
//
void TV_HandleNeedTextA(PTREE pTree, LPTOOLTIPTEXTA lptttA)
{
	TOOLTIPTEXT ttt;
	ttt.szText[0] = TEXT('\0');
	ttt.hdr       = lptttA->hdr;
	ttt.lpszText  = ttt.szText;
	ttt.hinst     = lptttA->hinst;
	ttt.uFlags    = lptttA->uFlags;
	ttt.lParam    = lptttA->lParam;

	TV_HandleNeedText(pTree, &ttt);
	if (pTree->pszTipA)
	{
		LocalFree(pTree->pszTipA);
	}
	pTree->pszTipA = ProduceAFromW(pTree->m_hWnd->ci.uiCodePage, ttt.lpszText);
	lptttA->lpszText = pTree->pszTipA;
	lptttA->uFlags  = ttt.uFlags;
}
#endif

// ----------------------------------------------------------------------------
//
//  TV_Timer
//
//  Checks to see if it is our name editing timer.  If so it  calls of to
//  do name editing
//
// ----------------------------------------------------------------------------
LRESULT TV_Timer(PTREE pTree, UINT uTimerId)
{
	switch (uTimerId)
	{
	case IDT_NAMEEDIT:
		// Kill the timer as we wont need any more messages from it.
		KillTimer(pTree->m_hWnd, IDT_NAMEEDIT);

		if (pTree->m_bNameEditPending)
		{
			// And start name editing mode.
			if (!TV_EditLabel(pTree, pTree->m_hCaretItem, NULL))
			{
				TV_DismissEdit(pTree, FALSE);
			}

			// remove the flag...
			pTree->m_bNameEditPending = FALSE;
		}
		break;

	case IDT_SCROLLWAIT:
		KillTimer(pTree->m_hWnd, IDT_SCROLLWAIT);
		if (pTree->m_bScrollWait)
		{
			if (pTree->m_hCaretItem != NULL)
			{
				TV_ScrollVertIntoView(pTree, pTree->m_hCaretItem);
			}
			pTree->m_bScrollWait = FALSE;
		}
		break;


	}
	return 0;
}

// ----------------------------------------------------------------------------
//
//  TV_Command
//
//  Process the WM_COMMAND.  See if it is an input from our edit windows.
//  if so we may want to dismiss it, and or set it is being dirty...
//
// ----------------------------------------------------------------------------
void TV_Command(PTREE pTree, int id, HWND hwndCtl, UINT codeNotify)
{
	if ((pTree != NULL) && (hwndCtl == pTree->hwndEdit))
	{
		switch (codeNotify)
		{
		case EN_UPDATE:
			// We will use the ID of the window as a Dirty flag...
			SetWindowID(pTree->hwndEdit, 1);
			TV_SetEditSize(pTree);
			break;

		case EN_KILLFOCUS:
			// We lost focus, so dismiss edit and save changes
			// (Note that the owner might reject the change and restart
			// edit mode, which traps the user.  Owners need to give the
			// user a way to get out.)

#if 0       // BUGBUG raymondc v6.0
			//
			//  Fix horrible undocumented hanging problem:  LVN_ENDLABELEDIT
			//  is sent in response to EN_KILLFOCUS, which is send in response
			//  to WM_KILLFOCUS, and it is undocumented that you cannot display
			//  UI during WM_KILLFOCUS when a journal record hook is active,
			//  because the presence of a hook forces serialization of activation,
			//  and so when you put up UI, you generate activation changes, which
			//  get stuck because you haven't finished responding to the previous
			//  WM_KILLFOCUS message yet.
			//
			//  See NT bug 414634.
			//
			if (InSendMessage())
			{
				ReplyMessage(0);
			}
#endif

			if (!TV_DismissEdit(pTree, FALSE))
			{
				return;
			}
			break;

		case HN_BEGINDIALOG: // penwin is bringing up a dialog
			_ASSERT(GetSystemMetrics(SM_PENWINDOWS)); // only on a pen system
			pTree->m_bNoDismissEdit = TRUE;
			break;

		case HN_ENDDIALOG: // penwin has destroyed dialog
			_ASSERT(GetSystemMetrics(SM_PENWINDOWS)); // only on a pen system
			pTree->m_bNoDismissEdit = FALSE;
			break;
		}

		// Forward edit control notifications up to parent
		//
		if (IsWindow(hwndCtl))
		{
			_ASSERT(FALSE); //TODO:
			// FORWARD_WM_COMMAND(pTree->m_hWnd->ci.hwndParent, id, hwndCtl, codeNotify, SendMessage);
		}
	}
}

void TV_CreateToolTips(PTREE pTree);

void TV_InitCheckBoxes(PTREE pTree)
{
	HIMAGELIST himl;
	XTVITEM ti;
	BOOL fNoColorKey = FALSE;    // Backwards: If Cleartype is turned on, then we don't use colorkey.

	//L
	//	if (g_bRunOnNT5)
	//	{
	//#ifdef CLEARTYPE    // Don't use SPI_CLEARTYPE because it's defined because of APIThk, but not in NT.
	//		SystemParametersInfo(SPI_GETCLEARTYPE, 0, &fNoColorKey, 0);
	//#endif
	//	}

	himl = CreateCheckBoxImagelist(pTree->m_hImageList, TRUE, !fNoColorKey, IS_WINDOW_RTL_MIRRORED(GetHostWnd(pTree->m_hWnd)));
	if (pTree->m_hImageList)
	{
		COLORREF cr = ImageList_GetBkColor(pTree->m_hImageList);
		ImageList_SetBkColor(himl, fNoColorKey ? (CLR_NONE) : (cr));
	}

	TV_SetImageList(pTree, himl, TVSIL_STATE);

	ti.mask = TVIF_STATE;
	ti.state = INDEXTOSTATEIMAGEMASK(1);
	ti.stateMask = TVIS_STATEIMAGEMASK;
	TV_SetItemRecurse(pTree, pTree->m_hRootItem, &ti);
}

void TV_OnStyleChanged(PTREE pTree, WPARAM gwl, LPSTYLESTRUCT pinfo)
{
	// Style changed: redraw everything...
	//
	// try to do this smartly, avoiding unnecessary redraws
	if (gwl == GWL_STYLE)
	{
		DWORD changeFlags;
		DWORD styleNew;

		TV_DismissEdit(pTree, FALSE);   // BUGBUG:  FALSE == accept changes.  Is this right?

		// You cannot combine TVS_HASLINES and TVS_FULLROWSELECT
		// because it doesn't work
		styleNew = pinfo->styleNew;
		if (styleNew & TVS_HASLINES)
		{
			if (styleNew & TVS_FULLROWSELECT)
			{
				//L DebugMsg(DM_ERROR, TEXT("Cannot combine TVS_HASLINES and TVS_FULLROWSELECT"));
			}
			styleNew &= ~TVS_FULLROWSELECT;
		}

		changeFlags = pinfo->styleOld ^ styleNew; // those that changed
		pTree->m_hWnd->ci.style = styleNew;               // change our version

#ifdef WINDOWS_ME
		pTree->m_hWnd->ci.style &= ~TVS_RTLREADING;
		pTree->m_hWnd->ci.style |= (pinfo->styleNew & TVS_RTLREADING);
#endif

		if (changeFlags & (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT))
		{
			TV_CreateIndentBmps(pTree);
		}

		if (changeFlags & TVS_CHECKBOXES)
		{
			if (pTree->m_hWnd->ci.style & TVS_CHECKBOXES)
			{
				TV_InitCheckBoxes(pTree);
			}
		}

		if (changeFlags & TVS_NOTOOLTIPS)
		{
			if (pTree->m_hWnd->ci.style & TVS_NOTOOLTIPS)
			{
				DestroyWindow(pTree->hwndToolTips);
				pTree->hwndToolTips = NULL;
			}
			else
			{
				TV_CreateToolTips(pTree);
			}
		}

		if (changeFlags & TVS_TRACKSELECT)
		{
			if (!(pTree->m_hWnd->ci.style & TVS_TRACKSELECT))
			{
				if (pTree->m_hHotItem != NULL)
				{
					TV_InvalidateItem(pTree, pTree->m_hHotItem, RDW_INVALIDATE | RDW_ERASE);
					pTree->m_hHotItem = NULL;
				}
			}
		}
		// Checkboxes and stuff may have changed width - go recompute
		TV_ScrollBarsAfterSetWidth(pTree, NULL);
	}
#ifdef WINDOWS_ME
	else if (gwl == GWL_EXSTYLE)
	{
		DWORD changeFlags;
		changeFlags = (pinfo->styleNew & WS_EX_RTLREADING) ? TVS_RTLREADING : 0;

		if (changeFlags ^ (pTree->m_hWnd->ci.style & TVS_RTLREADING))
		{
			pTree->m_hWnd->ci.style ^= TVS_RTLREADING;
			TV_DismissEdit(pTree, FALSE);   // Cancels edits

			DestroyWindow(pTree->hwndToolTips);
			pTree->hwndToolTips = NULL;
			TV_CreateToolTips(pTree);
		}
	}
#endif

}

void TV_OnMouseMove(PTREE pTree, DWORD dwPos, WPARAM wParam)
{
	if (pTree->m_hWnd->ci.style & TVS_TRACKSELECT)
	{
		POINT pt;
		HTREEITEM hHot;
		UINT wHitCode;

		pt.x = GET_X_LPARAM(dwPos);
		pt.y = GET_Y_LPARAM(dwPos);

		hHot = TV_CheckHit(pTree, pt.x, pt.y, &wHitCode);

		// 如果不是整行选中，并且没有命中Item文字区域，则无Hot Item
		if (!(pTree->m_hWnd->ci.style & TVS_FULLROWSELECT)
			&& !(wHitCode & TVHT_ONITEM))
		{
			hHot = NULL;
		}

		if (hHot != pTree->m_hHotItem)
		{
			HTREEITEM hOld = pTree->m_hHotItem;

			if (IsUpdateLayeredWindow(GetHostWnd(pTree->m_hWnd)))
			{
				pTree->m_hHotItem = hHot;
				InvalidateRect(pTree->m_hWnd, FALSE);
			}
			else
			{
				TV_InvalidateItem(pTree, pTree->m_hHotItem, RDW_INVALIDATE);
				TV_InvalidateItem(pTree, hHot, RDW_INVALIDATE);
				pTree->m_hHotItem = hHot;
			}
			// update now so that we won't have an invalid area
			// under the tooltips
			UpdateWindow(GetHostWnd(pTree->m_hWnd));

			// 发出Hot item改变事件
			RoutedEventArgs arg;
			arg.hSender = pTree->m_hWnd;
			arg.hOriginalSender = pTree->m_hWnd;
			arg.RoutedEvent = TVN_HOTCHANGED;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.wParam = (WPARAM)hOld;
			arg.lParam = (LPARAM)pTree->m_hHotItem;
			RaiseEvent(pTree->m_hWnd, &arg);
		}
	}

	if (pTree->hwndToolTips != NULL)
	{
		if (!TV_UpdateToolTip(pTree))
		{
			RelayToToolTips(pTree->hwndToolTips, GetHostWnd(pTree->m_hWnd), WM_MOUSEMOVE, wParam, dwPos);
		}
	}
}

void TV_OnWinIniChange(PTREE pTree, WPARAM wParam)
{
	if (!wParam ||
		(wParam == SPI_SETNONCLIENTMETRICS) ||
		(wParam == SPI_SETICONTITLELOGFONT))
	{

		if (pTree->m_bCreatedFont)
		{
			TV_OnSetFont(pTree, NULL, TRUE);
		}

		if (!pTree->m_bIndentSet)
		{
			// this will validate against the minimum
			TV_SetIndent(pTree, 0);
		}
	}
}

void TV_OnSetBkColor(PTREE pTree, COLORREF clr)
{
	if (pTree->clrBk != (COLORREF)(-1))
	{
		DeleteObject(pTree->hbrBk);
	}

	pTree->clrBk = clr;
	if (clr != (COLORREF)(-1))
	{
		pTree->hbrBk = CreateSolidBrush(clr);
	}
	TV_CreateIndentBmps(pTree); // This also invalidates
}

BOOL TV_TranslateAccelerator(PTREE pTree, LPMSG lpmsg)
{
	if (!lpmsg)
	{
		return FALSE;
	}

	Window *pHostWnd = Window::FromHandlePermanent(GetHostWnd(pTree->m_hWnd));
	if (pHostWnd->GetWLFocus() != pTree->m_hWnd)
	{
		return FALSE;
	}

	switch (lpmsg->message)
	{

	case WM_KEYUP:
	case WM_KEYDOWN:

		if (GetKeyState(VK_CONTROL) < 0)
		{
			switch (lpmsg->wParam)
			{
			case VK_LEFT:
			case VK_RIGHT:
			case VK_PRIOR:
			case VK_HOME:
			case VK_NEXT:
			case VK_END:
			case VK_UP:
			case VK_DOWN:
				TranslateMessage(lpmsg);
				DispatchMessage(lpmsg);
				return TRUE;
			}
		}
		else
		{

			switch (lpmsg->wParam)
			{

			case VK_RETURN:
			case VK_PRIOR:
			case VK_HOME:
			case VK_NEXT:
			case VK_END:
			case VK_SUBTRACT:
			case VK_ADD:
			case VK_MULTIPLY:
			case VK_LEFT:
			case VK_BACK:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
			case VK_SPACE:
				TranslateMessage(lpmsg);
				DispatchMessage(lpmsg);
				return TRUE;
			}
		}
		break;
	}

	return FALSE;
}

// 不能使用hHot来命中，因为只有打开hot track时，hHot才有效
HWLWND TV_HitTestBindItem(PTREE pTree, LPPOINT lpPoint)
{
	HWLWND hRet = NULL;

	// 垂直平滑滚动
	HTREEITEM hTI = pTree->hTop;
	CIUIRect rcItem;
	CIUIRect rcClient;
	GetClientRect(pTree->m_hWnd, rcClient);

	while (hTI != NULL)
	{
		TV_GetItemRect(pTree, hTI, rcItem, TRUE);

		// 超出控件，退出绘制
		if (rcItem.top >= rcClient.Height())
		{
			break;
		}

		if (NULL != hTI->hBindWindowless)
		{
			HWLWND hHittest = HitTestChild(hTI->hBindWindowless, lpPoint);
			if (NULL != hHittest)
			{
				hRet = hHittest;
				break;
			}
		}

		hTI = TV_GetNextVisItem(hTI);
	}

	return hRet;
}

// TreeView收到鼠标消息后，如果Item绑定了控件，根据具体情况，转发给这些控件
BOOL TV_IsMouseMessage(PTREE pTree, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
	{
		return FALSE;
	}

#ifdef _DEBUG
	// 调试用代码
	if (uMsg == WM_LBUTTONDOWN)
	{
		int n = 0;
	}
#endif

	CIUIPoint pt(lParam);

	//
	// 命中绑定到Item的控件。
	//
	UINT wHitCode;
	HTREEITEM hHot = TV_CheckHit(pTree, pt.x, pt.y, &wHitCode);
	if (NULL == hHot)
	{
		return FALSE;
	}

	if (NULL == hHot->hBindWindowless)
	{
		return FALSE;
	}

	HWLWND hDest = HitTestChild(hHot->hBindWindowless, &pt);
	if (NULL == hDest)
	{
		return FALSE;
	}

	static int i = 0;
	IUITRACE(_T("%d hDest is %s, caption is %s\n"), i++, GetName(hDest), GetWindowText(hDest));

	// 命中测试
	LRESULT lrHittest = HTCLIENT;
	_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

	CIUIPoint ptScreen = lParam;
	ClientToScreen(pTree->m_hWnd, &ptScreen);
	lrHittest = IUI::HitTest(hDest, &ptScreen);

	if (pThreadState->m_pWLMouseIn != hDest)
	{
		pThreadState->m_pWLMouseIn = hDest;
	}

	if (hDest != NULL)
	{
		// 如果命中了非客户区，要转成非客户区鼠标消息
		UINT uNewMsg = ConvertToNonClientMouseMessage(lrHittest, uMsg);

		// 转成相对目标控件的坐标
		// 对于Windowless RichEdit控件，由于没有窗口句柄，
		// RichEdit控件接收响应的坐标，都应该是相对于Host窗口的坐标。
		// 所以，RichEdit内部，都需要把传入的坐标，转成相对于Host，
		// 这点在写程序时要格外注意。
		CIUIPoint ptDest(lParam);
		ClientToScreen(pTree->m_hWnd, &ptDest);
		ScreenToClient(hDest, &ptDest);

		RoutedEventArgs arg;
		arg.hOriginalSender = hDest;
		arg.RoutedEvent = uNewMsg;
		arg.eRoutingStrategy = RS_TUNNEL;
		arg.wParam = wParam;
		arg.lParam = MAKELPARAM(ptDest.x, ptDest.y);

		RaiseEvent(hDest, &arg);
	}

	// TODO: If there has a capture windowless control, pass the message to it directly.

	return (hDest != NULL);
}

LRESULT WINAPI IUI::TV_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	PTREE pTree = (PTREE)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	// 滚动条悬浮模式
	if (NULL != pTree)
	{
		// 全局或Tree本身任意一个设置为使用悬浮滚动条，都生效。
		if (Metrics::IsFloatScrollBar() || pTree->m_bOverflowScrollBar)
		{
			HWLWND hVScrollBar = GetVerticalScrollBar(pTree->m_hWnd);
			HWLWND hHScrollBar = GetHorizontalScrollBar(pTree->m_hWnd);
			_IUI_THREAD_STATE *pThreadState = IUIGetThreadState();

			if (WM_MOUSEMOVE == pe->RoutedEvent)
			{
				LONG lStyle = GetStyle(pTree->m_hWnd);

				if ((lStyle & WS_VSCROLL) == WS_VSCROLL)
				{
					if (!IsWindowVisible(hVScrollBar))
					{
						ShowWindow(hVScrollBar, SW_SHOW);
					}
				}

				if ((lStyle & WS_HSCROLL) == WS_HSCROLL)
				{
					if (!IsWindowVisible(hHScrollBar))
					{
						ShowWindow(hHScrollBar, SW_SHOW);
					}
				}
			}
			else if (WM_MOUSELEAVE == pe->RoutedEvent)
			{
				// 如果鼠标离开控件，且新命中的控件不是Tree的孩子，则隐藏滚动条。
				BOOL bIs = FALSE;
				if (NULL != pThreadState->m_pWLMouseIn)
				{
					bIs = IsDescendant(pTree->m_hWnd, pThreadState->m_pWLMouseIn);
				}

				if (!bIs || NULL == pThreadState->m_pWLMouseIn)
				{
					LONG lStyle = GetStyle(pTree->m_hWnd);

					if ((lStyle & WS_VSCROLL) == WS_VSCROLL)
					{
						if (IsWindowVisible(hVScrollBar))
						{
							ShowWindow(hVScrollBar, SW_HIDE);
						}
					}

					if ((lStyle & WS_HSCROLL) == WS_HSCROLL)
					{
						if (IsWindowVisible(hHScrollBar))
						{
							ShowWindow(hHScrollBar, SW_HIDE);
						}
					}
				}
			}
		}
	}

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pTree == NULL)
			{
				if (uMsg == WM_NCCREATE)
				{
					TV_OnCreate(hWnd, (LPCREATESTRUCT)lParam);
				}

				return 0;
			}

			switch (uMsg)
			{
			case WM_DESTROY:
				TV_DestroyTree(pTree);
				return 0;

			case WM_WININICHANGE:
				TV_OnWinIniChange(pTree, wParam);
				break;

			case WM_STYLECHANGED:
				TV_OnStyleChanged(pTree, wParam, (LPSTYLESTRUCT)lParam);
				return 1;

			case WM_KILLFOCUS:
				// Reset wheel scroll amount
				IUIGetGloabelVar()->gcWheelDelta = 0;

				pTree->m_bFocus = FALSE;
				if (pTree->m_hCaretItem)
				{
					TV_InvalidateItem(pTree, pTree->m_hCaretItem, RDW_INVALIDATE);
					UpdateWindow(GetHostWnd(pTree->m_hWnd));
				}
				CCSendNotify(&pTree->m_hWnd->ci, NM_KILLFOCUS, NULL);
				IncrementSearchString(&pTree->is, 0, NULL);
				break;

			case WM_SETFOCUS:
				_ASSERT(IUIGetGloabelVar()->gcWheelDelta == 0);

				pTree->m_bFocus = TRUE;
				if (pTree->m_hCaretItem)
				{
					TV_InvalidateItem(pTree, pTree->m_hCaretItem, RDW_INVALIDATE);
					//MyNotifyWinEvent(EVENT_OBJECT_FOCUS, hwnd, OBJID_CLIENT, (LONG_PTR)pTree->m_hCaretItem);
				}
				else
				{
					TV_SelectItem(pTree, TVGN_CARET, pTree->hTop, TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_INTERNAL);
				}

				CCSendNotify(&pTree->m_hWnd->ci, NM_SETFOCUS, NULL);
				break;

			case WM_WINDOWPOSCHANGED:
			case WM_SIZE:
				TV_SizeWnd(pTree, 0, 0);
				break;

			case WM_ENABLE:
				// HACK: we don't get WM_STYLECHANGE on EnableWindow()
				if (wParam)
				{
					pTree->m_hWnd->ci.style &= ~WS_DISABLED;    // enabled
				}
				else
				{
					pTree->m_hWnd->ci.style |= WS_DISABLED;    // disabled
				}
				TV_CreateIndentBmps(pTree); // This invalidates the whole window!
				break;

			case WM_SYSCOLORCHANGE:
				//L InitGlobalColors();
				TV_CreateIndentBmps(pTree); // This invalidates the whole window!
				break;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			if (pTree != NULL
				&& (uMsg >= WM_MOUSEFIRST)
				&& (uMsg <= WM_MOUSELAST)
				&& (pTree->m_hWnd->ci.style & TVS_TRACKSELECT)
				&& !pTree->m_bTrackSet)
			{
				TRACKMOUSEEVENT tme;

				pTree->m_bTrackSet = TRUE;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = GetHostWnd(pTree->m_hWnd);
				tme.dwFlags = TME_LEAVE;

				TrackMouseEvent(&tme);
			}

			BOOL bChildHandled = TV_IsMouseMessage(pTree, uMsg, wParam, lParam);

			switch (uMsg)
			{
			case WM_MOUSELEAVE:
			{
				pTree->m_bTrackSet = FALSE;

				// 在UpdateLayeredWindow中，TV_InvalidateItem会引起立即刷新，所以，要先把
				// hHot设置为NULL，然后再刷新原hHot区域。
				// 而在非UpdateLayeredWindow中，TV_InvalidateItem只是设置无效区域，
				// 并不会立即刷新
				HTREEITEM hOldHot = pTree->m_hHotItem;

				// 发出TVN_HOTCHANGING事件
				{
					RoutedEventArgs arg;
					arg.hSender = pTree->m_hWnd;
					arg.hOriginalSender = pTree->m_hWnd;
					arg.RoutedEvent = TVN_HOTCHANGING;
					arg.eRoutingStrategy = RS_BUBBLE;
					arg.wParam = (WPARAM)hOldHot;
					arg.lParam = NULL;
					LRESULT lr = RaiseEvent(pTree->m_hWnd, &arg);
					if (0 != arg.lResult)
					{
						break;
					}
				}

				RECT rc;
				if (pTree->m_hHotItem != NULL
					&& pTree->m_bRedraw
					&& TV_GetItemRect(pTree, pTree->m_hHotItem, &rc, FALSE))
				{
					pTree->m_hHotItem = NULL;

					InvalidateRect(pTree->m_hWnd, &rc);
				}
				pTree->m_hHotItem = NULL;
				TV_PopBubble(pTree);

				// 发出TVN_HOTCHANGED事件
				RoutedEventArgs arg;
				arg.hSender = pTree->m_hWnd;
				arg.hOriginalSender = pTree->m_hWnd;
				arg.RoutedEvent = TVN_HOTCHANGED;
				arg.eRoutingStrategy = RS_BUBBLE;
				arg.wParam = (WPARAM)hOldHot;
				arg.lParam = NULL;
				RaiseEvent(pTree->m_hWnd, &arg);
			}

			break;

			case WM_CHAR:
				if (pTree->m_nPuntChar)
				{
					pTree->m_nPuntChar--;
					return TRUE;
				}
				else
				{
					return HANDLE_WM_CHAR(pTree, wParam, lParam, TV_OnChar);
				}

			case WM_KEYDOWN:
				if (TV_KeyDown(pTree, wParam, lParam))
				{
					IncrementSearchString(&pTree->is, 0, NULL);
				}
				goto DoDefault;


			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
				if (!bChildHandled)
				{
					TV_ButtonDown(pTree, uMsg, (UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
				}
				break;

			case WM_RBUTTONDOWN:
				if (!bChildHandled)
				{
					TV_SendRButtonDown(pTree, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
				break;

			case WM_MOUSEWHEEL:
			{
				LONG_PTR lStyle = GetStyle(hWnd);
				BOOL bScroll = (!IsIncludeFlag(lStyle, TVS_NOSCROLL));

				if (bScroll)
				{
					int nOldPos = GetScrollPos(hWnd, SB_VERT);
					short zDelta = HIWORD(wParam);
					if (zDelta > 0)
					{
						TV_VertScroll(pTree, SB_THUMBTRACK,
							max(0, nOldPos - pTree->m_nMouseWheelStep));
					}
					else
					{
						TV_VertScroll(pTree, SB_THUMBTRACK,
							nOldPos + pTree->m_nMouseWheelStep);
					}
				}
			}
			break;

			case WM_MOUSEMOVE:
				TV_OnMouseMove(pTree, (DWORD) lParam, wParam);
				break;

			case WM_MBUTTONDOWN:
				SetFocus(GetHostWnd(pTree->m_hWnd), pTree->m_hWnd);
				goto DoDefault;

			case WM_SYSKEYDOWN:
				TV_KeyDown(pTree, wParam, lParam);

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			if (pTree != NULL && uMsg == IUIGetGloabelVar()->g_uDragImages)
			{
				return TV_GenerateDragImage(pTree, (SHDRAGIMAGE *)lParam);
			}

			switch (uMsg)
			{
			case WLM_BIND_STYLE:
				TreeView_BindStyle(hWnd, (CTreeViewProp *)lParam);
				return 0;

			case TVMP_CALCSCROLLBARS:
				TV_CalcScrollBars(pTree);
				break;

			case WM_SETCURSOR:
			{
				NMMOUSE nm;
				HTREEITEM hItem;
				nm.dwHitInfo = lParam;
				hItem = TV_ItemAtCursor(pTree, NULL);
				if (hItem)
				{
					nm.dwItemSpec = (ULONG_PTR)hItem;
					nm.dwItemData = (ULONG_PTR)(hItem->lParam);
				}
				else
				{
					nm.dwItemSpec = 0;
					nm.dwItemData = 0;
				}

#ifdef UNIX
				if (pTree->hwndEdit != (HWND)wParam)
#endif
					if (CCSendNotify(&pTree->m_hWnd->ci, NM_SETCURSOR, &nm.hdr))
					{
						return 0;
					}
			}
			// TODO: 鼠标移动到Hot Item上后，默认变成手型。
			// 但这里的代码会导致光标形状来回变动，暂时屏蔽。
			//if (pTree->hWnd->ci.style & TVS_TRACKSELECT)
			//{
			//	if (pTree->m_hHotItem != NULL)
			//	{
			//		if (pTree->m_hHotItemCursor == NULL)
			//		{
			//			pTree->m_hHotItemCursor = LoadHandCursor(0);
			//		}
			//		SetCursor(pTree->m_hHotItemCursor);
			//		pe->bHandled = TRUE;
			//		return TRUE;
			//	}
			//}
			goto DoDefault;
			break;

			case TVM_GETITEMSTATE:
			{
				XTVITEM tvi;

				tvi.mask = TVIF_STATE;
				tvi.stateMask = (UINT) lParam;
				tvi.hItem = (HTREEITEM)wParam;
				if (!TV_OnGetItem(pTree, &tvi))
				{
					return 0;
				}

				return tvi.state;
			}

			case TVM_SETBKCOLOR:
			{
				LRESULT lres = (LRESULT)pTree->clrBk;
				TV_OnSetBkColor(pTree, (COLORREF)lParam);
				return lres;
			}

			case TVM_SETTEXTCOLOR:
			{
				LRESULT lres = (LRESULT)pTree->clrText;
				pTree->clrText = (COLORREF)lParam;
				TV_CreateIndentBmps(pTree); // This also invalidates
				return lres;
			}

			case TVM_GETBKCOLOR:
				return (LRESULT)pTree->clrBk;

			case TVM_GETTEXTCOLOR:
				return (LRESULT)pTree->clrText;

			case TVM_GETSCROLLTIME:
				return (LRESULT)pTree->uMaxScrollTime;

			case TVM_SETSCROLLTIME:
			{
				UINT u = pTree->uMaxScrollTime;
				pTree->uMaxScrollTime = (UINT)wParam;
				return (LRESULT)u;
			}


#ifdef UNICODE
			case TVM_INSERTITEMA:
				if (!lParam)
				{
					return 0;
				}

				return (LRESULT)TV_InsertItemA(pTree, (LPTV_INSERTSTRUCTA)lParam);

			case TVM_GETITEMA:
				if (!lParam)
				{
					return 0;
				}

				return (LRESULT)TV_OnGetItemA(pTree, (LPTVITEMEXA)lParam);

			case TVM_SETITEMA:
				if (!lParam)
				{
					return 0;
				}

				return (LRESULT)TV_SetItemA(pTree, (LPTVITEMEXA)lParam);

#endif
			case TVM_INSERTITEM:
				return (LRESULT)TV_InsertItem(pTree, (LPTV_INSERTSTRUCT)lParam);

			case TVM_MOVEITEM:
				return (LRESULT)TV_MoveItem(pTree, (LPTV_INSERTSTRUCT)lParam);

			case TVM_DELETEITEM:
				// Assume if items are being deleted that name editing is invalid.
				TV_DismissEdit(pTree, TRUE);
				return TV_DeleteItem(pTree, (TREEITEM *)lParam, TVDI_NORMAL);

			case TVM_GETNEXTITEM:
				return (LRESULT)TV_GetNextItem(pTree, (TREEITEM *)lParam, wParam);

			case TVM_GETITEMRECT:
				// lParam points to hItem to get rect from on input
				if (!lParam)
				{
					return 0;
				}
				if (!ValidateTreeItem(*(HTREEITEM *)lParam, 0))
				{
					return 0;    // Invalid parameter
				}
				return (LRESULT)TV_GetItemRect(pTree, *(HTREEITEM *)lParam, (LPRECT)lParam, (BOOL)wParam);

			case TVM_GETITEM:
				return (LRESULT)TV_OnGetItem(pTree, (LPXTVITEM)lParam);

			case TVM_SETITEM:
				return (LRESULT)TV_SetItem(pTree, (LPCXTVITEM)lParam);

			case TVM_ENSUREVISIBLE:
				if (!ValidateTreeItem((HTREEITEM)lParam, 0))
				{
					return 0;
				}
				return TV_EnsureVisible(pTree, (TREEITEM *)lParam);

			case TVM_SETIMAGELIST:
				return (LRESULT)(ULONG_PTR)TV_SetImageList(pTree, (HIMAGELIST)lParam, (int)wParam);

			case TVM_EXPAND:
				if (!ValidateTreeItem((HTREEITEM)lParam, 0))
				{
					return FALSE;    // invalid parameter
				}
				return TV_Expand(pTree, wParam, (TREEITEM *)lParam, FALSE);

			case TVM_HITTEST:
				return (LRESULT)TV_OnHitTest(pTree, (LPTV_HITTESTINFO)lParam);

			case TVM_GETCOUNT:
				return MAKELRESULTFROMUINT(pTree->cItems);

			case TVM_GETIMAGELIST:
				switch (wParam)
				{
				case TVSIL_NORMAL:
					return MAKELRESULTFROMUINT(pTree->m_hImageList);
				case TVSIL_STATE:
					return MAKELRESULTFROMUINT(pTree->m_hImlState);
				default:
					return 0;
				}

#ifdef UNICODE
			case TVM_GETISEARCHSTRINGA:
			{
				Window *pHostWnd = Window::FromHandlePermanent(GetHostWnd(hWnd));

				if (pHostWnd->GetWLFocus() == pTree->m_hWnd)
				{
					return (LRESULT)GetIncrementSearchStringA(&pTree->is, pTree->m_hWnd->ci.uiCodePage, (LPSTR)lParam);
				}
				else
				{
					return 0;
				}
			}
#endif

			case TVM_GETISEARCHSTRING:
			{
				Window *pHostWnd = Window::FromHandlePermanent(GetHostWnd(hWnd));

				if (pHostWnd->GetWLFocus() == pTree->m_hWnd)
				{
					return (LRESULT)GetIncrementSearchString(&pTree->is, (LPTSTR)lParam);
				}
				else
				{
					return 0;
				}
			}

#ifdef UNICODE
			case TVM_EDITLABELA:
			{
				LPWSTR lpEditString = NULL;
				HWND   hRet;

				if (wParam)
				{
					lpEditString = ProduceWFromA(pTree->m_hWnd->ci.uiCodePage, (LPSTR)wParam);
				}

				hRet = TV_EditLabel(pTree, (HTREEITEM)lParam, lpEditString);

				if (lpEditString)
				{
					FreeProducedString(lpEditString);
				}

				return MAKELRESULTFROMUINT(hRet);
			}
#endif

			case TVM_EDITLABEL:
				return MAKELRESULTFROMUINT(TV_EditLabel(pTree, (HTREEITEM)lParam,
							(LPTSTR)wParam));


			case TVM_GETVISIBLECOUNT:
				return TV_GetVisCount(pTree, (BOOL) wParam);

			case TVM_SETINDENT:
				TV_SetIndent(pTree, wParam);
				pTree->m_bIndentSet = TRUE;
				break;

			case TVM_GETINDENT:
				return MAKELRESULTFROMUINT(pTree->cxIndent);

			case TVM_CREATEDRAGIMAGE:
				return MAKELRESULTFROMUINT(TV_CreateDragImage(pTree, (TREEITEM *)lParam));

			case TVM_GETEDITCONTROL:
				return (LRESULT)(ULONG_PTR)pTree->hwndEdit;

			case TVM_SORTCHILDREN:
				return TV_SortChildren(pTree, (TREEITEM *)lParam, (BOOL)wParam);

			case TVM_SORTCHILDRENCB:
				return TV_SortChildrenCB(pTree, (TV_SORTCB *)lParam, (BOOL)wParam);

			case TVM_SELECTITEM:
				return TV_SelectItem(pTree, wParam, (TREEITEM *)lParam, TVSIF_NOTIFY | TVSIF_UPDATENOW, TVC_UNKNOWN);

			case TVM_ENDEDITLABELNOW:
				return TV_DismissEdit(pTree, (BOOL)wParam);

			case TVM_GETTOOLTIPS:
				return (LRESULT)(ULONG_PTR)pTree->hwndToolTips;

			case TVM_SETTOOLTIPS:
			{
				HWND hwndOld = pTree->hwndToolTips;

				pTree->hwndToolTips = (HWND)wParam;
				return (LRESULT)(ULONG_PTR)hwndOld;
			}

			case TVM_GETITEMHEIGHT:
				return pTree->cyItem;

			case TVM_SETITEMHEIGHT:
			{
				int iOld = pTree->cyItem;
				pTree->m_bCyItemSet = (wParam != (WPARAM)(-1));
				pTree->cyItem = (SHORT)wParam; // must be even
				TV_SetItemHeight(pTree);
				return iOld;
			}
			case TVM_SETBORDER:
			{
				int cyOld = pTree->cyBorder;
				int cxOld = pTree->cxBorder;

				if (wParam & TVSBF_YBORDER)
				{
					pTree->cyBorder = HIWORD(lParam);
				}
				if (wParam & TVSBF_XBORDER)
				{
					pTree->cxBorder = LOWORD(lParam);
				}

				TV_CalcScrollBars(pTree);
				return MAKELONG(cxOld, cyOld);
			}
			case TVM_GETBORDER:
				return MAKELONG(pTree->cxBorder, pTree->cyBorder);
			case TVM_SETINSERTMARK:
				return TV_SetInsertMark(pTree, (TREEITEM *)lParam, (BOOL) wParam);

			case TVM_SETINSERTMARKCOLOR:
			{
				LRESULT lres = (LRESULT)pTree->clrim;
				pTree->clrim = (COLORREF) lParam;
				TV_InvalidateInsertMarkRect(pTree, FALSE); // Repaint in new color
				return lres;
			}
			case TVM_GETINSERTMARKCOLOR:
				return pTree->clrim;

			case TVM_TRANSLATEACCELERATOR:
				return TV_TranslateAccelerator(pTree, (LPMSG)lParam);

			case TVM_SETLINECOLOR:
			{
				LRESULT lres = (LRESULT)pTree->clrLine;
				pTree->clrLine = (COLORREF)lParam;
				TV_CreateIndentBmps(pTree); // This also invalidates
				return lres;
			}

			case TVM_GETLINECOLOR:
				return (LRESULT)pTree->clrLine;

#if defined(FE_IME) || !defined(WINNT)
			case WM_IME_COMPOSITION:
				// Now only Korean version is interested in incremental search with composition string.
				//L if (g_fDBCSInputEnabled)
				//{
				//	if (((ULONG_PTR)GetKeyboardLayout(0L) & 0xF000FFFFL) == 0xE0000412L)
				//	{
				//		if (TV_OnImeComposition(pTree, wParam, lParam))
				//		{
				//			lParam &= ~GCS_RESULTSTR;
				//			goto DoDefault;
				//		}
				//		else
				//		{
				//			break;
				//		}
				//	}
				//}
				goto DoDefault;
#endif

			case WM_SETREDRAW:
				TV_OnSetRedraw(pTree, (BOOL)wParam);
				break;

			case TVM_DRAWITEM:
			{
				DRAWTREEITEMSTRUCT *pDis = (DRAWTREEITEMSTRUCT *)lParam;
				return 0;
			}

			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
				TV_Paint(pTree, (HDC)wParam);
				return 1; // 禁止基类绘制

			case WM_ERASEBKGND:
				//{
				//	RECT rc;

				//	TV_GetBackgroundBrush(pTree, (HDC) wParam);
				//	GetClipBox((HDC) wParam, &rc);
				//	FillRect((HDC)wParam, &rc, pTree->hbrBk);
				//}
				return TRUE;

			case WM_GETDLGCODE:
				return (LRESULT)(DLGC_WANTARROWS | DLGC_WANTCHARS);

			case WM_HSCROLL:
				TV_HorzScroll(pTree, GET_WM_HSCROLL_CODE(wParam, lParam), GET_WM_HSCROLL_POS(wParam, lParam));
				break;

			case WM_VSCROLL:
				TV_VertScroll(pTree, GET_WM_VSCROLL_CODE(wParam, lParam), GET_WM_VSCROLL_POS(wParam, lParam));
				return 1;

			case WM_GETFONT:
				return MAKELRESULTFROMUINT(pTree->hFont);

			case WM_SETFONT:
				TV_OnSetFont(pTree, (HFONT) wParam, (BOOL) lParam);
				break;

			case WM_TIMER:
				TV_Timer(pTree, (UINT) wParam);
				break;

			case WM_COMMAND:
				TV_Command(pTree, (int)GET_WM_COMMAND_ID(wParam, lParam), GET_WM_COMMAND_HWND(wParam, lParam),
					(UINT)GET_WM_COMMAND_CMD(wParam, lParam));
				break;

			case WM_NOTIFY:
			{
				LPNMHDR lpnm = (LPNMHDR)lParam;

				if ((lpnm->code <= PGN_FIRST) && (PGN_LAST <= lpnm->code))
				{
					LRESULT TV_OnPagerControlNotify(PTREE pTree, LPNMHDR pnm);

					return TV_OnPagerControlNotify(pTree, lpnm);
				}
				if (lpnm->hwndFrom == pTree->hwndToolTips)
				{
					switch (lpnm->code)
					{
					case TTN_NEEDTEXT:
						TV_HandleNeedText(pTree, (LPTOOLTIPTEXT)lpnm);
						break;

#ifdef UNICODE
					case TTN_NEEDTEXTA:
						TV_HandleNeedTextA(pTree, (LPTOOLTIPTEXTA)lpnm);
						break;
#endif

					case TTN_SHOW:
						return TV_HandleTTNShow(pTree, lpnm);

					case NM_CUSTOMDRAW:
						return TV_HandleTTCustomDraw(pTree, (LPNMTTCUSTOMDRAW)lpnm);
					}
				}
				break;
			}

			case WM_NOTIFYFORMAT:
				return CIHandleNotifyFormat(&pTree->m_hWnd->ci, lParam);

			case WM_GETOBJECT:
				if (lParam == OBJID_QUERYCLASSNAMEIDX)
				{
					return MSAA_CLASSNAMEIDX_TREEVIEW;
				}
				goto DoDefault;

			// IUI Add
			case TVM_GET_SMOOTH_VSCROLL:
				return pTree->m_bSmoothVScroll;

			case TVM_SET_FLOAT_SCROLLBAR:
				pTree->m_bOverflowScrollBar = lParam;
				return 0;

			case TVM_IS_FLOAT_SCROLLBAR:
				return pTree->m_bOverflowScrollBar;

			case TVM_SETITEMHEIGHTEX:
				if (pTree->m_bSmoothVScroll)
				{
					HTREEITEM hItem = (HTREEITEM)lParam;
					if (hItem != NULL)
					{
						int iOld = hItem->iIntegral;
						hItem->iIntegral = wParam;
						hItem->bIntegralSet = TRUE;

						// 更新总显示高度
						pTree->cShowing += (hItem->iIntegral - iOld);
						// 更新hItem之后的显示索引
						TV_UpdateShownIndexes(pTree, hItem);
						// 重新计算滚动条
						TV_CalcScrollBars(pTree);

						return iOld;
					}
				}
				return 0;

			case TVM_GET_HIGHLIGHT_ITEM:
				return (LRESULT)pTree->m_hHotItem;

			case WLM_SET_TEXT_ALIGN_MODE:
			{
				BOOL bVert = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				if (bVert)
				{
					TEXT_ALIGN_VER eHorAlignMode = (TEXT_ALIGN_VER)lParam;
					pTree->m_eTextVerAlignMode[nIndex] = eHorAlignMode;
				}
				else
				{
					TEXT_ALIGN_HOR eHorAlignMode = (TEXT_ALIGN_HOR)lParam;
					pTree->m_eTextHorAlignMode[nIndex] = eHorAlignMode;
				}
			}
			return 0;

			case WLM_GET_TEXT_ALIGN_MODE:
			{
				BOOL bVert = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				if (bVert)
				{
					return pTree->m_eTextVerAlignMode[nIndex];
				}
				else
				{
					return pTree->m_eTextHorAlignMode[nIndex];
				}
			}
			return 0;

			case WLM_SET_TEXT_MARGIN:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				LPCRECT lprcTextMargin = (LPCRECT)lParam;
				pTree->m_rcTextMargin[nIndex] = *lprcTextMargin;
			}
			return 0;

			case WLM_GET_TEXT_MARGIN:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				LPRECT lprcTextMargin = (LPRECT)lParam;
				if (lprcTextMargin == NULL)
				{
					return -1;
				}

				*lprcTextMargin = pTree->m_rcTextMargin[nIndex];
			}
			return 0;

			case TVM_ENABLE_ITEM_LINE:
				pTree->m_bEnableItemLine = wParam;
				return 0;

			case TVM_IS_ENABLE_ITEM_LINE:
				return pTree->m_bEnableItemLine;

			case TVM_SET_ITEM_LINE_COLOR:
				pTree->m_crItemLine = lParam;
				return 0;

			case TVM_GET_ITEM_LINE_COLOR:
				return pTree->m_crItemLine;

			case TVM_SET_INDENT_IMAGE:
			{
				// 是否+号
				BOOL bAdd = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = (LPCTSTR)lParam;

				if (eControlState == CONTROL_STATE_COMBINE)
				{
					IUISetControlImage(GetProject(hWnd), &pTree->m_himgCombineCollapseExpanded, lpszImage);
				}
				else
				{
					int nIndex = GetButtonStateIndex(eControlState);
					_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

					if (bAdd)
					{
						IUISetControlImage(GetProject(hWnd), &pTree->m_himgCollapse[nIndex], lpszImage);
					}
					else
					{
						IUISetControlImage(GetProject(hWnd), &pTree->m_himgExpanded[nIndex], lpszImage);
					}
				}

				pTree->m_eCollapseExpandedImageResizeMode = IRM_CENTER;
			}
			return 0;

			case TVM_GET_INDENT_IMAGE:
			{
				// 是否+号
				BOOL bAdd = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_COMBINE)
				{
					return (LRESULT)(LPCTSTR)pTree->m_himgCombineCollapseExpanded->GetSafeImageName();
				}
				else
				{
					int nIndex = GetButtonStateIndex(eControlState);
					_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

					if (bAdd)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgCollapse[nIndex]->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgExpanded[nIndex]->GetSafeImageName();
					}
				}
			}
			return 0;

			case WLM_ENABLE_SPEC_ITEM_EFFECT:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				hItem->pItemSpecBackground->bItemBkEnabled = lParam;
			}
			return 0;

			case WLM_IS_SPEC_ITEM_EFFECT_ENABLED:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return FALSE;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					return FALSE;
				}

				return hItem->pItemSpecBackground->bItemBkEnabled;
			}
			return FALSE;

			case WLM_SET_SPEC_ITEM_BK_TYPE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				hItem->pItemSpecBackground->eItemBackgroundType = (BACKGROUND_TYPE)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_TYPE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return BKT_UNKNOWN;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					return BKT_UNKNOWN;
				}

				return hItem->pItemSpecBackground->eItemBackgroundType;
			}
			return BKT_UNKNOWN;

			case WLM_SET_SPEC_ITEM_BK_COLOR:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				ITEM_COLOR *pic = (ITEM_COLOR *)lParam;

				if (hItem == NULL || pic == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pic->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					hItem->pItemSpecBackground->crItemBk[nIndex] = pic->color;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_COLOR:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL || hItem->pItemSpecBackground == NULL)
				{
					return 0;
				}
				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return hItem->pItemSpecBackground->crItemBk[nIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_SPEC_ITEM_BK_IMAGE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				ITEM_IMAGE *pii = (ITEM_IMAGE *)lParam;

				if (hItem == NULL || pii == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pii->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					IUISetControlImage(GetProject(hWnd),
						&hItem->pItemSpecBackground->himgItemBk[nIndex], pii->lpszImage);
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return NULL;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					return NULL;
				}

				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return (LRESULT)(LPCTSTR)hItem->pItemSpecBackground->himgItemBk[nIndex]->GetSafeImageName();
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				hItem->pItemSpecBackground->eItemBkImageResizeMode = (IMAGE_RESIZE_MODE)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE_RESIZE_MODE:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return IRM_UNKNOWN;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					return IRM_UNKNOWN;
				}

				return hItem->pItemSpecBackground->eItemBkImageResizeMode;
			}
			return IRM_UNKNOWN;

			case WLM_SET_SPEC_ITEM_BK_IMAGE_9GRID_ARG:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				hItem->pItemSpecBackground->rcItemBkImage9GridArg = (LPCRECT)lParam;
			}
			return 0;

			case WLM_GET_SPEC_ITEM_BK_IMAGE_9GRID_ARG:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL || lParam == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					return -2;
				}

				*(LPRECT)lParam = hItem->pItemSpecBackground->rcItemBkImage9GridArg;
			}
			return 0;

			case WLM_SET_SPEC_ITEM_TEXT_COLOR:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				ITEM_COLOR *pic = (ITEM_COLOR *)lParam;

				if (hItem == NULL || pic == NULL)
				{
					return -1;
				}

				if (hItem->pItemSpecBackground == NULL)
				{
					hItem->pItemSpecBackground = new ITEM_BACKGROUND;
				}

				int nIndex = GetButtonStateIndex(pic->eControlState);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					hItem->pItemSpecBackground->crText[nIndex] = pic->color;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_SPEC_ITEM_TEXT_COLOR:
			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				if (hItem == NULL || hItem->pItemSpecBackground == NULL)
				{
					return 0;
				}
				int nIndex = GetButtonStateIndex((CONTROL_STATE)lParam);
				if (nIndex >= 0 && nIndex < COMBINEIMAGESIZE4)
				{
					return hItem->pItemSpecBackground->crText[nIndex];
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_ITEM_BK_TYPE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					pTree->m_eSelectedItemBkType = (BACKGROUND_TYPE)lParam;
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					pTree->m_eFocusItemBkType = (BACKGROUND_TYPE)lParam;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					pTree->m_eHighlightItemBkType = (BACKGROUND_TYPE)lParam;
				}
				return 0;

			case WLM_GET_ITEM_BK_TYPE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					return pTree->m_eSelectedItemBkType;
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					return pTree->m_eFocusItemBkType;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					return pTree->m_eHighlightItemBkType;
				}
				return -1;

			case WLM_SET_ITEM_BK_COLOR:
			{
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					pTree->m_crSelectedItemBK = lParam;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					pTree->m_crHighlightItemBK = lParam;
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					pTree->m_crFocusItemBK = lParam;
				}
			}
			return 0;

			case WLM_GET_ITEM_BK_COLOR:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					return pTree->m_crSelectedItemBK;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					return pTree->m_crHighlightItemBK;
				}
				else if (ITEM_FOCUS == LOWORD(wParam))
				{
					return pTree->m_crFocusItemBK;
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = (LPCTSTR)lParam;
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgFocusItemBk, lpszImage);
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					pTree->m_bSelectedItemBkCombine = false;
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgSelectedItemBk[0], lpszImage);
					}
					else if (eControlState == CONTROL_STATE_DISABLED)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgSelectedItemBk[1], lpszImage);
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgHighlightItemBk[1], lpszImage);
					}
					else if (eControlState == CONTROL_STATE_FOCUS)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgHighlightItemBk[0], lpszImage);
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						IUISetControlImage(GetProject(hWnd),
							&pTree->m_himgHighlightItemCombineBk, lpszImage);
					}
				}
			}
			return 0;

			case WLM_GET_ITEM_BK_IMAGE:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgFocusItemBk->GetSafeImageName();
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgSelectedItemBk[0]->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_DISABLED)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgSelectedItemBk[1]->GetSafeImageName();
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (eControlState == CONTROL_STATE_NORMAL)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgHighlightItemBk[1]->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_FOCUS)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgHighlightItemBk[0]->GetSafeImageName();
					}
					else if (eControlState == CONTROL_STATE_COMBINE)
					{
						return (LRESULT)(LPCTSTR)pTree->m_himgHighlightItemCombineBk->GetSafeImageName();
					}
				}
			}
			return 0;

			case WLM_SET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						// TODO: 默认9宫格
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						// TODO: 默认9宫格
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						// TODO: 默认9宫格
					}
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_RESIZE_MODE:
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							// 默认9宫格
							return IRM_9GRID;
						}
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							// 默认9宫格
							return IRM_9GRID;
						}
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							// 默认9宫格
							return IRM_9GRID;
						}
					}
				}
				return 0;

			case WLM_SET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						pTree->m_rcFocusItemBkImage9GridArg = (LPCRECT)lParam;
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						pTree->m_rcSelectedItemBkImage9GridArg = (LPCRECT)lParam;
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						pTree->m_rcHighlightItemBkImage9GridArg = (LPCRECT)lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_BK_IMAGE_9GRID_ARG:
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							*(LPRECT)lParam = pTree->m_rcFocusItemBkImage9GridArg;
						}
					}
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							*(LPRECT)lParam = pTree->m_rcSelectedItemBkImage9GridArg;
						}
					}
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					if (CONTROL_STATE_NORMAL == HIWORD(wParam))
					{
						if (lParam != NULL)
						{
							*(LPRECT)lParam = pTree->m_rcHighlightItemBkImage9GridArg;
						}
					}
				}
				return 0;

			case WLM_SET_ITEM_TEXT_COLOR:
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					pTree->m_crFocusItemText = lParam;
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					pTree->m_crSelectedItemText = lParam;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					pTree->m_crHighlightItemText = lParam;
				}
				else if (ITEM_NORMAL == LOWORD(wParam))
				{
					if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						pTree->m_crDisabledItemText = lParam;
					}
				}
				return 0;

			case WLM_GET_ITEM_TEXT_COLOR:
				if (ITEM_FOCUS == LOWORD(wParam))
				{
					return pTree->m_crFocusItemText;
				}
				else if (ITEM_SELECTED == LOWORD(wParam))
				{
					return pTree->m_crSelectedItemText;
				}
				else if (ITEM_HIGHLIGHT == LOWORD(wParam))
				{
					return pTree->m_crHighlightItemText;
				}
				else if (ITEM_NORMAL == LOWORD(wParam))
				{
					if (CONTROL_STATE_DISABLED == HIWORD(wParam))
					{
						return 	pTree->m_crDisabledItemText;
					}
				}

				return 0;

			case WLM_HITTESTBINDITEM:
				return (LRESULT)TV_HitTestBindItem(pTree, &CIUIPoint(lParam));

			case TVM_SET_HOVER_TIME:
				pTree->m_dwHoverTime = wParam;
				pTree->m_bHoverRepeat = lParam;
				return 0;

			case TVM_GET_HOVER_TIME:
				if (wParam != 0)
				{
					*(DWORD *)wParam = pTree->m_dwHoverTime;
				}
				if (lParam != 0)
				{
					*(BOOL *)lParam = pTree->m_bHoverRepeat;
				}
				return 0;

			case TVM_SET_MOUSE_WHEEL_STEP:
				pTree->m_nMouseWheelStep = lParam;
				return 0;

			case TVM_GET_MOUSE_WHEEL_STEP:
				return pTree->m_nMouseWheelStep;

#ifdef KEYBOARDCUES
			case WM_UPDATEUISTATE:
			{
				DWORD dwUIStateMask = MAKEWPARAM(0xFFFF, UISF_HIDEFOCUS);

				if (CCOnUIState(&(pTree->m_hWnd->ci), WM_UPDATEUISTATE, wParam & dwUIStateMask, lParam))
					if (pTree->m_hCaretItem)
					{
						TV_InvalidateItem(pTree, pTree->m_hCaretItem, TRUE);
					}

				goto DoDefault;
			}
#endif
			default:
				// Special handling of magellan mouse message
				if (uMsg == IUIGetGloabelVar()->g_msgMSWheel)
				{
					BOOL  fScroll;
					BOOL  fDataZoom;
					DWORD dwStyle;
					int   cScrollLines;
					int   cPage;
					int   pos;
					int   cDetants;
					int iWheelDelta;

					//L
					//if (g_bRunOnNT || g_bRunOnMemphis)
					//{
					//	iWheelDelta = (int)(short)HIWORD(wParam);
					//	fScroll = !(wParam & (MK_SHIFT | MK_CONTROL));
					//	fDataZoom = (BOOL)(wParam & MK_SHIFT);
					//}
					//else
					{
						iWheelDelta = (int)wParam;
						fDataZoom = (GetKeyState(VK_SHIFT) < 0);
						fScroll = !fDataZoom && GetKeyState(VK_CONTROL) >= 0;
					}

					// Update count of scroll amount
					IUIGetGloabelVar()->gcWheelDelta -= iWheelDelta;
					cDetants = IUIGetGloabelVar()->gcWheelDelta / WHEEL_DELTA;
					if (cDetants != 0)
					{
						IUIGetGloabelVar()->gcWheelDelta %= WHEEL_DELTA;
					}

					if (fScroll)
					{
						if (IUIGetGloabelVar()->g_ucScrollLines > 0 &&
							cDetants != 0 &&
							(WS_VSCROLL | WS_HSCROLL) & (dwStyle = GetStyle(pTree->m_hWnd)))
						{

							if (dwStyle & WS_VSCROLL)
							{
								cPage = max(1, (pTree->cFullVisible - 1));
								cScrollLines =
									cDetants *
									min(IUIGetGloabelVar()->g_ucScrollLines, (UINT) cPage);

								pos = max(0, pTree->hTop->iShownIndex + cScrollLines);
								TV_VertScroll(pTree, SB_THUMBPOSITION, pos);
							}
							else
							{
								cPage = max(TREE_HORZLINE,
										(pTree->cxWnd - TREE_HORZLINE)) /
									TREE_HORZLINE;

								cScrollLines =
									cDetants *
									(int) min((ULONG) cPage, IUIGetGloabelVar()->g_ucScrollLines) *
									TREE_HORZLINE;

								pos = max(0, pTree->xPos + cScrollLines);
								TV_HorzScroll(pTree, SB_THUMBPOSITION, pos);
							}
						}
						return 1;
					}
					else if (fDataZoom)
					{
						UINT wHitCode;
						POINT pt;

						pt.x = GET_X_LPARAM(lParam);
						pt.y = GET_Y_LPARAM(lParam);
						ScreenToClient(pTree->m_hWnd, &pt);

						// If we are rolling forward and hit an item then navigate into that
						// item or expand tree (simulate lbuttondown which will do it).  We
						// also need to handle rolling backwards over the ITEMBUTTON so
						// that we can collapse the tree in that case.  Otherwise
						// just fall through so it isn't handled.  In that case if we
						// are being hosted in explorer it will do a backwards
						// history navigation.
						if (TV_CheckHit(pTree, pt.x, pt.y, &wHitCode) &&
							(wHitCode & (TVHT_ONITEM | TVHT_ONITEMBUTTON)))
						{
							UINT uFlags = TVBD_FROMWHEEL;
							uFlags |= (iWheelDelta > 0) ? TVBD_WHEELFORWARD : TVBD_WHEELBACK;

							if ((uFlags & TVBD_WHEELFORWARD) || (wHitCode == TVHT_ONITEMBUTTON))
							{
								TV_ButtonDown(pTree, WM_LBUTTONDOWN, 0, pt.x, pt.y, uFlags);
								return 1;
							}
						}
						// else fall through
					}
				}
				else
				{
					if (pTree != NULL)
					{
						LRESULT lres;
						if (CCWndProc(&pTree->m_hWnd->ci, uMsg, wParam, lParam, &lres))
						{
							return lres;
						}
					}
					else
					{
						_ASSERT(FALSE);
					}
				}

DoDefault:
				return DefEventHandler(hWnd, pe);
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

// NOTE: there is very similar code in the listview
//
// Totally disgusting hack in order to catch VK_RETURN
// before edit control gets it.
//
LRESULT CALLBACK TV_EditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PTREE pTree = (PTREE)GetWindowInt(GetParent(hwnd), 0);
	_ASSERT(pTree);

	if (!pTree)
	{
		return 0L;    // wierd cases can get here...
	}

	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			TV_DismissEdit(pTree, FALSE);
			return 0L;

		case VK_ESCAPE:
			TV_DismissEdit(pTree, TRUE);
			return 0L;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case VK_RETURN:
			// Eat the character, so edit control wont beep!
			return 0L;
		}
	}

	return CallWindowProc(pTree->pfnEditWndProc, hwnd, msg, wParam, lParam);
}


void TV_SetEditSize(PTREE pTree)
{
	RECT rcLabel;
	UINT seips;

	if (pTree->m_hEditingItem == NULL)
	{
		return;
	}

	TV_GetItemRect(pTree, pTree->m_hEditingItem, &rcLabel, TRUE);

	// get exact the text bounds (acount for borders used when drawing)

	InflateRect(&rcLabel, -IUIGetGloabelVar()->g_cxLabelMargin, -IUIGetGloabelVar()->g_cyBorder);

	seips = 0;
#ifdef DEBUG
	// If we are in one of the no-scroll modes then it's possible for the
	// resulting rectangle not to be visible.  Similarly, if the item itself
	// isn't visible, then the resulting rectangle is definitely not visible.
	// Tell SetEditInPlaceSize not to freak out in those cases.
	if ((pTree->m_hWnd->ci.style & (TVS_NOSCROLL | TVS_NOHSCROLL)) ||
		!ITEM_VISIBLE(pTree->m_hEditingItem))
	{
		seips |= SEIPS_NOSCROLL;
	}
#endif

	SetEditInPlaceSize(pTree->hwndEdit, &rcLabel, (HFONT)SendMessage(pTree->hwndEdit, WM_GETFONT, 0, 0), seips);
}


void TV_CancelEditTimer(PTREE pTree)
{
	if (pTree->m_bNameEditPending)
	{
		KillTimer(pTree->m_hWnd, IDT_NAMEEDIT);
		pTree->m_bNameEditPending = FALSE;
	}
}

// BUGBUG: very similar code in lvicon.c


HWND TV_EditLabel(PTREE pTree, HTREEITEM hItem, LPTSTR pszInitial)
{
	TCHAR szLabel[MAXLABELTEXT];
	TV_DISPINFO nm;

	if (!(pTree->m_hWnd->ci.style & TVS_EDITLABELS))
	{
		return NULL;
	}

	if (!ValidateTreeItem(hItem, 0))
	{
		return NULL;
	}

	TV_DismissEdit(pTree, FALSE);


	// Now get the text associated with that item
	nm.item.pszText = szLabel;
	nm.item.cchTextMax = ARRAYSIZE(szLabel);
	nm.item.stateMask = TVIS_BOLD;
	// this cast is ok as long as TVIF_INTEGRAL or anything past it isn't asked for
	TV_GetItem(pTree, hItem, TVIF_TEXT | TVIF_STATE, (LPXTVITEM)&nm.item);

	// Must subtract one from ARRAYSIZE(szLabel) because Edit_LimitText
	// doesn't include the terminating NULL
	pTree->hwndEdit = CreateEditInPlaceWindow(GetHostWnd(pTree->m_hWnd),
			pszInitial ? pszInitial : nm.item.pszText, ARRAYSIZE(szLabel) - 1,
			WS_BORDER | WS_CLIPSIBLINGS | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL,
			(nm.item.state & TVIS_BOLD) ? pTree->hFontBold : pTree->hFont);

	if (pTree->hwndEdit)
	{
		if (pszInitial)     // if initialized, it's dirty.
		{
			SetWindowID(pTree->hwndEdit, 1);
		}
		//
		// Now notify the parent of this window and see if they want it.
		// We do it after we cretae the window, but before we show it
		// such that our parent can query for it and do things like limit
		// the number of characters that are input
		nm.item.hItem = hItem;
		nm.item.state = hItem->state;
		nm.item.lParam = hItem->lParam;
		nm.item.mask = (TVIF_HANDLE | TVIF_STATE | TVIF_PARAM | TVIF_TEXT);

		if ((BOOL)CCSendNotify(&pTree->m_hWnd->ci, TVN_BEGINLABELEDIT, &nm.hdr))
		{
			DestroyWindow(pTree->hwndEdit);
			pTree->hwndEdit = NULL;
			return NULL;
		}

		TV_PopBubble(pTree);

		TV_ScrollIntoView(pTree, hItem);

		pTree->pfnEditWndProc = SubclassWindow(pTree->hwndEdit, TV_EditWndProc);

		pTree->m_hEditingItem = hItem;

		TV_SetEditSize(pTree);

		// Show the window and set focus to it.  Do this after setting the
		// size so we don't get flicker.
		SetFocus(pTree->hwndEdit);
		ShowWindow(pTree->hwndEdit, SW_SHOW);
		TV_InvalidateItem(pTree, hItem, RDW_INVALIDATE | RDW_ERASE);

		RescrollEditWindow(pTree->hwndEdit);
	}

	return pTree->hwndEdit;
}


// BUGBUG: very similar code in lvicon.c

BOOL TV_DismissEdit(PTREE pTree, BOOL fCancel)
{
	HWND hwndEdit;
	BOOL fOkToContinue = TRUE;
	HTREEITEM htiEdit;

	if (pTree->m_bNoDismissEdit)
	{
		return FALSE;
	}

	hwndEdit = pTree->hwndEdit;

	if (!hwndEdit)
	{
		// Also make sure there are no pending edits...
		TV_CancelEditTimer(pTree);
		return TRUE;
	}

	// Assume that if we are not visible that the window is in the
	// process of being destroyed and we should not process the
	// editing of the window...
	if (!IsWindowVisible(pTree->m_hWnd))
	{
		fCancel = TRUE;
	}

	//
	// We are using the Window ID of the control as a BOOL to
	// state if it is dirty or not.
	switch (GetWindowID(hwndEdit))
	{
	case 0:
		// The edit control is not dirty so act like cancel.
		fCancel = TRUE;
	//  FALL THROUGH
	case 1:
		// The edit control is dirty so continue.
		SetWindowID(hwndEdit, 2);    // Don't recurse
		break;
	case 2:
		// We are in the process of processing an update now, bail out
		return TRUE;
	}

	// TV_DeleteItemRecurse will set htiEdit to NULL if the program
	// deleted the items out from underneath us (while we are waiting
	// for the edit timer).
	htiEdit = pTree->m_hEditingItem;

	if (htiEdit != NULL)
	{
		TV_DISPINFO nm;
		TCHAR szLabel[MAXLABELTEXT];

		DBG_ValidateTreeItem(htiEdit, 0);

		// Initialize notification message.
		nm.item.hItem = htiEdit;
		nm.item.lParam = htiEdit->lParam;
		nm.item.mask = 0;

		if (fCancel)
		{
			nm.item.pszText = NULL;
		}
		else
		{
			Edit_GetText(hwndEdit, szLabel, ARRAYSIZE(szLabel));
			nm.item.pszText = szLabel;
			nm.item.cchTextMax = ARRAYSIZE(szLabel);
			nm.item.mask |= TVIF_TEXT;
		}

		// Make sure the text redraws properly
		TV_InvalidateItem(pTree, htiEdit, RDW_INVALIDATE | RDW_ERASE);
		pTree->m_bNoDismissEdit = TRUE; // this is so that we don't recurse due to killfocus
		ShowWindow(hwndEdit, SW_HIDE);
		pTree->m_bNoDismissEdit = FALSE;

		//
		// Notify the parent that we the label editing has completed.
		// We will use the NMLVDISPINFOEX structure to return the new
		// label in.  The parent still has the old text available by
		// calling the GetItemText function.
		//

		fOkToContinue = (BOOL)CCSendNotify(&pTree->m_hWnd->ci, TVN_ENDLABELEDIT, &nm.hdr);
		if (fOkToContinue && !fCancel)
		{
			// BUGBUG raymondc: The caller might have deleted the item in
			// response to the edit.  We should revalidate here (or make
			// delete item invalidate our edit item).  Treat a deletion
			// as if it were a rejected edit.

			//
			// If the item has the text set as CALLBACK, we will let the
			// ower know that they are supposed to set the item text in
			// their own data structures.  Else we will simply update the
			// text in the actual view.
			//
			// Note: The callee may have set the handle to null to tell
			// us that the handle to item is no longer valid.
			if (nm.item.hItem != NULL)
			{
				if (htiEdit->lpstr != LPSTR_TEXTCALLBACK)
				{
					// Set the item text (everything's set up in nm.item)
					//
					nm.item.mask = TVIF_TEXT;
					TV_SetItem(pTree, (LPXTVITEM)&nm.item);
				}
				else
				{
					CCSendNotify(&pTree->m_hWnd->ci, TVN_SETDISPINFO, &nm.hdr);
				}
			}
		}
	}

	// If we did not reenter edit mode before now reset the edit state
	// variables to NULL
	if (hwndEdit == pTree->hwndEdit)
	{
		pTree->m_hEditingItem = NULL;
		pTree->hwndEdit = NULL; // so we don't get reentered on the kill focus
	}

	// done with the edit control
	DestroyWindow(hwndEdit);

	return fOkToContinue;
}

LRESULT TV_OnCalcSize(PTREE pTree, LPNMHDR pnm)
{
	LPNMPGCALCSIZE pcalcsize = (LPNMPGCALCSIZE)pnm;

	switch (pcalcsize->dwFlag)
	{
	case PGF_CALCHEIGHT:
		pcalcsize->iHeight = pTree->cShowing * pTree->cyItem;
		//L TraceMsg(TF_WARNING, "tv.PGF_CALCHEIGHT: cShow=%d cShow*cyItem=%d AWR()=%d",
		//	pTree->cShowing, pTree->cShowing * pTree->cyItem, pcalcsize->iHeight);
		break;

	case PGF_CALCWIDTH:
		break;
	}
	return 0L;
}

LRESULT TV_OnPagerControlNotify(PTREE pTree, LPNMHDR pnm)
{
	switch (pnm->code)
	{
	case PGN_SCROLL:
		return TV_OnScroll(pTree, pnm);
		break;
	case PGN_CALCSIZE:
		return TV_OnCalcSize(pTree, pnm);
		break;
	}
	return 0L;
}

LRESULT TV_OnScroll(PTREE pTree, LPNMHDR pnm)
{

	LPNMPGSCROLL pscroll = (LPNMPGSCROLL)pnm;
	RECT rc = pscroll->rcParent;
	RECT rcTemp;
	int iDir = pscroll->iDir;
	int dyScroll = pscroll->iScroll;
	TREEITEM *hItem;
	UINT uCode;
	int parentsize;
	TREEITEM   *hPrevItem;
	TREEITEM   *hNextItem;
	int y;

#ifndef UNIX
	POINT pt = {pscroll->iXpos, pscroll->iYpos};
	POINT ptTemp = pt;
	TREEITEM   *hCurrentItem = TV_CheckHit(pTree, pt.x + 1, pt.y + 1, &uCode);
#else
	POINT pt;
	POINT ptTemp;
	TREEITEM   *hCurrentItem;

	pt.x = pscroll->iXpos;
	pt.y = pscroll->iYpos;

	ptTemp = pt;
	hCurrentItem = TV_CheckHit(pTree, pt.x + 1, pt.y + 1, &uCode);
#endif

	switch (iDir)
	{
	case PGF_SCROLLUP:
		//Check if any Item is partially visible at the left/top. if so then set the bottom
		// of that Item to be our current offset and then scroll. This avoids skipping over
		// certain Items when partial Items are displayed at the left or top
		y = pt.y;
		TV_GetItemRect(pTree, hCurrentItem, &rcTemp, TRUE);

		if (rcTemp.top  <  y - 1)
		{
			hCurrentItem = TV_GetNextItem(pTree, hCurrentItem, TVGN_NEXTVISIBLE);
		}

		// Now do the calculation
		parentsize = RECTHEIGHT(rc);

		//if  the control key is down and we have more than parentsize size of child window
		// then scroll by that amount
		if ((pscroll->fwKeys & PGK_CONTROL) && ((pt.y - parentsize) > 0))
		{
			dyScroll = parentsize;
		}
		else if ((pt.y - pTree->cyItem) > 0)
		{
			// we dont have control key down so scroll by one buttonsize
			dyScroll = pTree->cyItem;
		}
		else
		{
			pscroll->iScroll = pt.y;
			return 0L;
		}
		ptTemp.y -= dyScroll;
		hItem = TV_CheckHit(pTree, ptTemp.x, ptTemp.y, &uCode);

		if (hItem)
		{
			// if  the hit test gives us the same Item as our CurrentItem then set the Item
			// to one Item to the top/left  of the  CurrentItem

			hPrevItem = TV_GetNextItem(pTree, hCurrentItem, TVGN_PREVIOUSVISIBLE);
			if ((hItem == hCurrentItem) && (hPrevItem != NULL))
			{
				hItem = hPrevItem;
			}

			//When scrolling left if we end up in the middle of some Item then we align it to the
			//right of that Item this is to avoid scrolling more than the pager window width but if the
			// Item happens to be the left Item of  our current Item then we end up in not scrolling
			//if thats the case then move one more Item to the left.


			if (hItem == hPrevItem)
			{
				hItem = TV_GetNextItem(pTree, hItem, TVGN_PREVIOUSVISIBLE);
				if (!hItem)
				{
					dyScroll = pt.y;
					break;
				}
			}

			TV_GetItemRect(pTree, hItem, &rcTemp, TRUE);
			dyScroll = pt.y - rcTemp.bottom;
		}
		break;
	case PGF_SCROLLDOWN:
	{
		RECT rcChild;
		int childsize;

		GetWindowRect(pTree->m_hWnd, &rcChild);
		childsize = RECTHEIGHT(rcChild);
		parentsize = RECTHEIGHT(rc);

		//if  the control key is down and we have more than parentsize size of child window
		// then scroll by that amount
		if ((pscroll->fwKeys & PGK_CONTROL) && ((childsize - pt.y - parentsize) > parentsize))
		{
			dyScroll = parentsize;
		}
		else if ((childsize - pt.y - parentsize) > (pTree->cyItem * TV_GetIntegral(pTree, hCurrentItem)))
		{
			// we dont have control key down so scroll by one buttonsize
			dyScroll = pTree->cyItem * TV_GetIntegral(pTree, hCurrentItem);
		}
		else
		{
			pscroll->iScroll = childsize - pt.y - parentsize;
			return 0L;
		}
		ptTemp.y += dyScroll;

		hItem = TV_CheckHit(pTree, ptTemp.x, ptTemp.y, &uCode);

		if (hItem)
		{
			if ((hItem == hCurrentItem) &&
				((hNextItem = TV_GetNextItem(pTree, hItem, TVGN_NEXTVISIBLE)) != NULL))
			{
				hItem = hNextItem;
			}
			TV_GetItemRect(pTree, hItem, &rcTemp, TRUE);
			dyScroll = rcTemp.top  - pt.y ;
		}

		break;
	}
	}
	//Set the scroll value
	pscroll->iScroll = dyScroll;
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////////
// 返回TRUE，停止遍历，否则继续。
BOOL CALLBACK WalkTreeCallBack(HWND hTree, HTREEITEM hCurItem, void *pCallbackParam)
{
	XTVITEM item;
	item.hItem = hCurItem;
	item.mask = TVIF_TEXT;
	CIUIString str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		item.pszText = str.GetBufferSetLength(nLen);
		item.cchTextMax = nLen;
		::SendMessage(hTree, TVM_GETITEM, 0, (LPARAM)&item);
		nRes = lstrlen(item.pszText);
	}
	while (nRes >= nLen - 1);
	str.ReleaseBuffer();

	if (str == _T("IDR_COLOR_RED"))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IUI::WalkTree(HWLWND hTree, HTREEITEM hItemFrom, WalkTreeCallBack fn, void *pCallbackParam)
{
	if (hTree == NULL)
	{
		return FALSE;
	}

	HTREEITEM hCurItem = NULL;
	if (hItemFrom == NULL)
	{
		hCurItem = (HTREEITEM)SendMessage(hTree, TVM_GETNEXTITEM, TVGN_ROOT, 0);
	}
	else
	{
		hCurItem = hItemFrom;
	}

	while (hCurItem != NULL)
	{
#ifdef _DEBUG
		XTVITEM item;
		item.hItem = hCurItem;
		item.mask = TVIF_TEXT;
		CIUIString str;
		int nLen = 128;
		int nRes;
		do
		{
			nLen *= 2;
			item.pszText = str.GetBufferSetLength(nLen);
			item.cchTextMax = nLen;
			SendMessage(hTree, TVM_GETITEM, 0, (LPARAM)&item);
			nRes = lstrlen(item.pszText);
		}
		while (nRes >= nLen - 1);
		str.ReleaseBuffer();
#endif // _DEBUG

		BOOL bRet = fn(hTree, hCurItem, pCallbackParam);
		if (bRet)
		{
			// 不再遍历，提前结束
			return TRUE;
		}

		HTREEITEM hChild = (HTREEITEM)SendMessage(hTree, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hCurItem);
		if (hChild != NULL)
		{
			bRet = WalkTree(hTree, hChild, fn, pCallbackParam);
			if (bRet)
			{
				return TRUE;
			}
		}

		hCurItem = (HTREEITEM)SendMessage(hTree, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hCurItem);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// TreeView Memory

void TV_ScrollItems(PTREE pTree, int nItems, int iTopShownIndex, BOOL fDown);


// 递归删除Item
// hItem: 被删除的Item
// flags: 控制如何删除或删除什么，取值如下：
//        TVDI_NORMAL        删除本节点及所有子节点
//        TVDI_NONOTIFY      不发送TVN_DELETEITEM Notify消息
//        TVDI_CHILDRENONLY  只删除子节点，不删除自己
void TV_DeleteItemRecurse(PTREE pTree, TREEITEM *hItem, UINT flags)
{
	DBG_ValidateTreeItem(hItem, 0);

	//
	// We do this from DeleteItemRecurse(), kind of like how USER sends
	// Destroy notifications from its FreeWindow() code, so that we get
	// deletes for parent and children both.
	//
	MyNotifyWinEvent(EVENT_OBJECT_DESTROY, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT,
		(LONG_PTR)hItem);

	//
	//  While the item is still valid, clean up if it's the insertion point.
	//  The item needs to be valid because we're going to call other
	//  functions that validate their parameters...
	//
	if (hItem == pTree->m_hInsertItem)
	{
		TV_SetInsertMark(pTree, NULL, FALSE);
		_ASSERT(pTree->m_hInsertItem == NULL);
	}

	// 删除所有孩子(及孩子的孩子)
	TREEITEM *hKid;
	TREEITEM *hNext;
	for (hKid = hItem->hKids; hKid != NULL; hKid = hNext)
	{
		// 删除hKid前，记录hKid的兄弟，否则删除hKid后，就找不到它的兄弟了。
		hNext = hKid->hNext;

		// 递归删除每个孩子
		TV_DeleteItemRecurse(pTree, hKid, flags & ~TVDI_CHILDRENONLY);
	}

	if ((flags & TVDI_CHILDRENONLY) || !hItem->hParent)
	{
		return;
	}

	if (!(flags & TVDI_NONOTIFY))    // BUGBUG: this is not set by anyone
	{
		NM_TREEVIEW nm;
		// Let the app clean up after itself
		nm.itemOld.hItem = hItem;
		nm.itemOld.lParam = hItem->lParam;
		nm.itemNew.mask = 0;
		nm.itemOld.mask = (TVIF_HANDLE | TVIF_PARAM);
		CCSendNotify(&pTree->m_hWnd->ci, TVN_DELETEITEM, &nm.hdr);
	}

	//
	// 通知本Item的观察者，让观察者知道Item被删除了.
	//
	int i = DPA_GetPtrCount(pTree->m_hDpaWatch);
	while (--i >= 0)
	{
		PTVWATCHEDITEM pwi = (PTVWATCHEDITEM)DPA_FastGetPtr(pTree->m_hDpaWatch, i);
		_ASSERT(pwi);
		if (pwi->hti == hItem)
		{
			pwi->hti = hItem->hNext;
			pwi->fStale = TRUE;
		}
	}

	TREEITEM *hParent = hItem->hParent;
	_ASSERT(hParent);

	// 修改Tree Item链表指向
	if (hParent->hKids == hItem)
	{
		hParent->hKids = hItem->hNext;
		hKid = NULL;
	}
	else
	{
		// not the first child, find our previous item (linear search!)
		hKid = TV_GetNextItem(pTree, hItem, TVGN_PREVIOUS);
		_ASSERT(hKid);
		hKid->hNext = hItem->hNext;
	}
	
	// 总Items数减 1。
	pTree->cItems--;

	// 如果被删除的Item是展开的, 更新展开的Item数量、TreeView的最大宽度，并重新计算滚动信息
	TV_ScrollBarsAfterRemove(pTree, hItem);

	// reset tooltip after unlink from the parent child chain
	if (pTree->m_hToolTipItem == hItem)
	{
		TV_SetToolTipTarget(pTree, NULL);
	}

	Str_Set(&hItem->lpstr, NULL);

	TV_MarkAsDead(hItem);

	// 以下代码需仔细。 hItem虽然从TreeView Item链表中移除，但hItem仍然有效
	// 它的部分成员也是有效的

	// TREE结构体中的一些变量，可能引用了被删除的Item，需要把它们置NULL
	if (hItem == pTree->m_hEditingItem)
	{
		pTree->m_hEditingItem = NULL;
	}

	if (hItem == pTree->m_hDropTargetItem)
	{
		pTree->m_hDropTargetItem = NULL;
	}

	if (hItem == pTree->m_hOldDropItem)
	{
		pTree->m_hOldDropItem = NULL;
	}

	if (hItem == pTree->m_hHotItem)
	{
		pTree->m_hHotItem = NULL;
	}

	if (hItem == pTree->m_hSearchItem)
	{
		pTree->m_hSearchItem = NULL;
	}

	// if the caret escaped the collapsed area and landed on us, push it away
	if (pTree->m_hCaretItem == hItem)
	{
		HTREEITEM hTemp;
		if (hItem->hNext != NULL)
		{
			hTemp = hItem->hNext;
		}
		else
		{
			hTemp = VISIBLE_PARENT(hItem);
			if (hTemp == NULL)
			{
				hTemp = hKid;    // set above when we unlinked from the previous item
			}
		}
		// Reset the caret to NULL as to not try to reference our
		// invalidated item.
		pTree->m_hCaretItem = NULL;
		TV_SelectItem(pTree, TVGN_CARET, hTemp, (flags & TVDI_NOSELCHANGE) ? 0 : TVSIF_NOTIFY, 0);
		_ASSERT(pTree->m_hCaretItem != hItem);
	}

	// BUGBUG: might want to really do this
	_ASSERT(pTree->hItemPainting != hItem);

	ControlFree(pTree->hheap, hItem);
}

// 移除指定Item及其所有孩子，如果指定的Item是隐藏的根节点，则只移除它的孩子，隐藏的根节点本身不被移除。
BOOL TV_DeleteItem(PTREE pTree, TREEITEM *hItem, UINT flags)
{
	if (TVI_ROOT == hItem || NULL == hItem)
	{
		hItem = pTree->m_hRootItem;
	}

	if (!ValidateTreeItem(hItem, 0))
	{
		return FALSE;
	}

	// 为了加快速度，首先折叠Item(滚动条需要重新计算)
	// 并且在删除Item后，重新设置top item
	if (hItem != pTree->m_hRootItem)
	{
		TV_Expand(pTree, TVE_COLLAPSE, hItem, FALSE);
	}
	else
	{
		// TV_Expand punts on the root item, so manually iterate through it's kids
		TREEITEM *hKid = hItem->hKids;
		while (hKid)
		{
			TV_Expand(pTree, TVE_COLLAPSE, hKid, FALSE);
			if (!ValidateTreeItem(hKid, 0))
			{
				break;    // callback during collapse could delete
			}
			hKid = hKid->hNext;
		}
	}

	// must be done AFTER setting the
	// selection
	// 指定Item下方的区域需要在删除Item后刷新。在设置了新的选中Item后刷新
	if (hItem->hParent == pTree->m_hRootItem || hItem == pTree->m_hRootItem || ITEM_VISIBLE(hItem->hParent))
	{
		if (pTree->m_bRedraw)
		{
			InvalidateRect(pTree->m_hWnd, NULL);
		}
	}
	else
	{
		TV_ScrollBelow(pTree, hItem->hParent, FALSE, FALSE);
	}

	// We can pass in the root to clear all items
	if (hItem == pTree->m_hRootItem)
	{
		flags |= TVDI_CHILDRENONLY;
	}

	// 递归删除Item
	TV_DeleteItemRecurse(pTree, hItem, flags);

	_ASSERT(pTree->m_hRootItem); // didn't go too far, did we?

						   // maybe everything's gone...
						   // check out our cleanup job
	if (NULL == pTree->m_hRootItem->hKids)
	{
		// the tree itself
		_ASSERT(pTree->cItems == 0);
		pTree->cItems = 0; // just removed it all, didn't we?

						   // BUGBUG: this fails because we don't touch hTop if redraw is off
						   // in TV_DeleteItemRecurse()
						   // AssertMsg(pTree->hTop == NULL, TEXT("hTop not NULL, but empty tree"));
		pTree->hTop = NULL;

		//L AssertMsg(pTree->m_hCaretItem == NULL, TEXT("m_hCaretItem not NULL, but empty tree"));
		pTree->m_hCaretItem = NULL;

		pTree->m_bNameEditPending = FALSE;
		pTree->cxMax = 0;
		pTree->xPos = 0;

		// the invisible root
		_ASSERT(pTree->m_hRootItem->hNext == NULL);
		pTree->m_hRootItem->hNext = NULL;
		_ASSERT(pTree->m_hRootItem->hParent == NULL);
		pTree->m_hRootItem->hParent = NULL;
		_ASSERT(pTree->m_hRootItem->hKids == NULL);
		pTree->m_hRootItem->hKids = NULL;
		_ASSERT(pTree->m_hRootItem->state & TVIS_EXPANDED);
		pTree->m_hRootItem->state |= (TVIS_EXPANDED | TVIS_EXPANDEDONCE);
		_ASSERT(pTree->m_hRootItem->iLevel == (BYTE)(-1));
		pTree->m_hRootItem->iLevel = BYTE(-1);
		_ASSERT(pTree->m_hRootItem->iShownIndex == WORD(-1));
		pTree->m_hRootItem->iShownIndex = WORD(-1);
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  Creates the hidden root node for the tree -- all items will trace up to
//  this root, and the first child of the root is the first item in the tree.
//
//  sets m_hRootItem
//
// ----------------------------------------------------------------------------
const TCHAR c_szNULL[] = TEXT("");
BOOL PASCAL TV_CreateRoot(PTREE pTree)
{
	TREEITEM *hRoot = (TREEITEM *)ControlAlloc(pTree->hheap, sizeof(TREEITEM));
	if (!hRoot)
	{
		return FALSE;
	}

	// hRoot->hNext        = NULL;
	// hRoot->hKids        = NULL;
	// hRoot->hParent      = NULL;
	hRoot->iLevel = BYTE(-1);
	hRoot->state = (TVIS_EXPANDED | TVIS_EXPANDEDONCE);
	hRoot->iShownIndex = WORD(-1);
	hRoot->wSignature = TV_SIG;

	pTree->m_hRootItem = hRoot;

	// OLEACC asks for the text of the root item (d'oh!)
	Str_Set(&hRoot->lpstr, c_szNULL);
	return TRUE;
}

#ifdef DEBUG

void DumpItem(TREEITEM *hItem)
{
	LPTSTR p;

	if (hItem->lpstr == LPSTR_TEXTCALLBACK)
	{
		p = TEXT("(callback)");
	}
	else if (hItem->lpstr == NULL)
	{
		p = TEXT("(null)");
	}
	else
	{
		p = hItem->lpstr;
	}

	//L TraceMsg(TF_TREEVIEW, "%s", p);
	//TraceMsg(TF_TREEVIEW, "\tstate:%4.4x show index:%3d level:%2d kids:%ld lparam:%4.4x",
	//	hItem->state, hItem->iShownIndex,
	//	hItem->iLevel, hItem->fKids, hItem->lParam);

}

#else
#define DumpItem(hItem)
#endif


// ----------------------------------------------------------------------------
//
//  Adds the item described by the given arguments to the tree.
//
//  sets hTop, cItems
//
// ----------------------------------------------------------------------------

#ifdef UNICODE
TREEITEM *TV_InsertItemA(PTREE pTree, LPTV_INSERTSTRUCTA lpis)
{
	LPSTR pszA = NULL;
	TREEITEM *ptvi;

	//HACK Alert!  This code assumes that TV_INSERTSTRUCTA is exactly the same
	// as TV_INSERTSTRUCTW except for the text pointer in the XTVITEM
	COMPILETIME_ASSERT(sizeof(TV_INSERTSTRUCTA) == sizeof(TV_INSERTSTRUCTW));

	if (!IsFlagPtr(lpis) && (lpis->DUMMYUNION_MEMBER(item).mask & TVIF_TEXT) && !IsFlagPtr(lpis->DUMMYUNION_MEMBER(item).pszText))
	{

		pszA = lpis->DUMMYUNION_MEMBER(item).pszText;
		lpis->DUMMYUNION_MEMBER(item).pszText = (LPSTR)ProduceWFromA(pTree->m_hWnd->ci.uiCodePage, lpis->DUMMYUNION_MEMBER(item).pszText);

		if (lpis->DUMMYUNION_MEMBER(item).pszText == NULL)
		{
			lpis->DUMMYUNION_MEMBER(item).pszText = pszA;
			return NULL;
		}
	}

	ptvi = TV_InsertItem(pTree, (LPTV_INSERTSTRUCTW)lpis);

	if (pszA)
	{
		FreeProducedString(lpis->DUMMYUNION_MEMBER(item).pszText);
		lpis->DUMMYUNION_MEMBER(item).pszText = pszA;
	}

	return ptvi;
}
#endif

TREEITEM *TV_InsertItem(PTREE pTree, LPTV_INSERTSTRUCT lpis)
{
	TREEITEM *hNewItem, *hItem;
	TREEITEM *hParent;
	TREEITEM *hInsertAfter;
	UINT mask;

	if (!lpis)
	{
		return NULL;    //BUGBUG: Validate LPTV_INSERTSTRUCT
	}

	// initialize _after_ the check for NULL!
	hParent = lpis->hParent;
	hInsertAfter = lpis->hInsertAfter;
	mask = lpis->DUMMYUNION_MEMBER(item).mask;

	// don't allow undefined bits
	//L AssertMsg((lpis->DUMMYUNION_MEMBER(item).mask & ~TVIF_ALL) == 0, TEXT("Invalid TVIF mask specified"));
	if (mask & ~TVIF_ALL)
	{
		// if they used bogus bits,
		// restrict to win95 bits only
		// I'd like to fail completely, but for win95 compat, we can't
		//
		// this fixes  QuaterDesk's CleanSweep which has bogus garbage on the stack for a mask
		mask = (TVIF_WIN95 & mask);
	}

	TV_DismissEdit(pTree, FALSE);

	//
	//  Zillions of apps pass garbage for hInsertAfter, so don't fail if
	//  it's invalid.  Fortunately, we never dereference hInsertAfter, so
	//  garbage is okay.

	if (!ValidateTreeItem(hParent, VTI_NULLOK))     // NULL means TVI_ROOT
	{
		return NULL;
	}

	DBG_ValidateTreeItem(hInsertAfter, 0);

	hNewItem = (TREEITEM *)ControlAlloc(pTree->hheap, sizeof(TREEITEM));
	if (hNewItem == NULL)
	{
		IUITRACE(_T("TreeView: Out of memory"));
		return NULL;
	}

	hNewItem->wSignature = TV_SIG;

	if (mask & TVIF_TEXT)
	{
		//
		// We will setup the text string next, before we link our self in
		// as to handle the case where we run out of memory and need to
		// destroy ourself without having to unlink.
		//
		if (!lpis->DUMMYUNION_MEMBER(item).pszText)
		{
			hNewItem->lpstr = LPSTR_TEXTCALLBACK;
		}
		else
		{
			if (!Str_Set(&hNewItem->lpstr, lpis->DUMMYUNION_MEMBER(item).pszText))
			{
				// Memory allocation failure...
				//L TraceMsg(TF_ERROR, "TreeView: Out of memory");
				TV_MarkAsDead(hNewItem);
				ControlFree(pTree->hheap, hNewItem);
				return NULL;
			}
		}
	}
	else
	{
		Str_Set(&hNewItem->lpstr, c_szNULL);
	}

	//L AssertMsg(hNewItem->lpstr != NULL, TEXT("Item added with NULL text"));

	if ((hParent == NULL) || (hParent == TVI_ROOT))
	{
		hParent = pTree->m_hRootItem;
		if (pTree->hTop == NULL)
		{
			pTree->hTop = hNewItem;
		}
	}
	else if (!pTree->m_hRootItem->hKids)
	{
		TV_MarkAsDead(hNewItem);
		ControlFree(pTree->hheap, hNewItem);
		return NULL;
	}

	// We will do the sort later, so we can handle TEXTCALLBACK things
	if ((hInsertAfter == TVI_FIRST || hInsertAfter == TVI_SORT) || hParent->hKids == NULL)
	{
		hNewItem->hNext = hParent->hKids;
		hParent->hKids = hNewItem;
	}
	else
	{
		// BUGBUG: we should cache the last insert after pointer to try to
		// catch the case of consecutive adds to the end of a node

		if (hInsertAfter == TVI_LAST)
			for (hItem = hParent->hKids; hItem->hNext; hItem = hItem->hNext)
				;
		else
		{
			for (hItem = hParent->hKids; hItem->hNext; hItem = hItem->hNext)
				if (hItem == hInsertAfter)
				{
					break;
				}
		}

		hNewItem->hNext = hItem->hNext;
		hItem->hNext = hNewItem;
	}

	hNewItem->hParent = hParent;
	hNewItem->iLevel = hParent->iLevel + 1;

	if (pTree->m_bSmoothVScroll)
	{
		hNewItem->iIntegral = 0;
	}
	else
	{
		if ((mask & TVIF_INTEGRAL) &&
			LOWORD(lpis->DUMMYUNION_MEMBER(itemex).iIntegral) > 0)
		{
			hNewItem->iIntegral = LOWORD(lpis->DUMMYUNION_MEMBER(itemex).iIntegral);
		}
		else
		{
#ifdef CHEEDEBUG
			// just to get some variety
			hNewItem->iIntegral = ((((int)hNewItem) / 100) % 2) + 1;
#else
			hNewItem->iIntegral = 1;
#endif
		}
	}

	if (pTree->hTop == hNewItem)
	{
		hNewItem->iShownIndex = 0;    // calc me please!
	}
	else
	{
		hNewItem->iShownIndex = WORD(-1);    // calc me please!
	}

	if (mask & TVIF_IMAGE)
	{
		hNewItem->iImage = (WORD)lpis->DUMMYUNION_MEMBER(item).iImage;
	}

	if (mask & TVIF_SELECTEDIMAGE)
	{
		hNewItem->iSelectedImage = (WORD)lpis->DUMMYUNION_MEMBER(item).iSelectedImage;
	}

	if (mask & TVIF_PARAM)
	{
		hNewItem->lParam = lpis->DUMMYUNION_MEMBER(item).lParam;
	}

	if (mask & TVIF_STATE)
	{
		hNewItem->state = lpis->DUMMYUNION_MEMBER(item).state & lpis->DUMMYUNION_MEMBER(item).stateMask;
	}

	// if we're in check box mode, inforce that it has a check box
	if (pTree->m_hWnd->ci.style & TVS_CHECKBOXES)
	{
		if ((hNewItem->state & TVIS_STATEIMAGEMASK) == 0)
		{
			hNewItem->state |= INDEXTOSTATEIMAGEMASK(1);
		}
	}




	if ((hNewItem->state & TVIS_BOLD) && !pTree->hFontBold) //$BOLD
	{
		TV_CreateBoldFont(pTree);    //$BOLD
	}

	// TraceMsg(TF_TRACE, "Tree: Inserting i = %d state = %d", TV_StateIndex(&lpis->item), lpis->item.state);

	if (mask & TVIF_CHILDREN)
	{
		switch (lpis->DUMMYUNION_MEMBER(item).cChildren)
		{
		case I_CHILDRENCALLBACK:
			hNewItem->fKids = KIDS_CALLBACK;
			break;

		case 0:
			hNewItem->fKids = KIDS_FORCE_NO;
			break;

		default:
			hNewItem->fKids = KIDS_FORCE_YES;
			break;
		}
	}

	// accept state bits on create?
	// mask & TVIF_STATE

	pTree->cItems++;

	// I don't want to do any callbacks until the item is completed
	// so sorting waits until the end
	// special case an only child for speed
	// (hKids && hKids->hNext means more than one child)
	if ((hInsertAfter == TVI_SORT)
		&& hParent->hKids != NULL
		&& hParent->hKids->hNext != NULL)
	{
		XTVITEM sThisItem, sNextItem;
		TCHAR szThis[64], szNext[64];    // BUGBUG: these are too small

		sThisItem.pszText = szThis;
		sThisItem.cchTextMax = ARRAYSIZE(szThis);
		TV_GetItem(pTree, hNewItem, TVIF_TEXT, &sThisItem);

		// We know that the first kid of hParent is hNewItem
		for (hItem = hNewItem->hNext; hItem; hItem = hItem->hNext)
		{

			sNextItem.pszText = szNext;
			sNextItem.cchTextMax = ARRAYSIZE(szNext);

			TV_GetItem(pTree, hItem, TVIF_TEXT, &sNextItem);

			if (lstrcmpi(sThisItem.pszText, sNextItem.pszText) < 0)
			{
				break;
			}

			hInsertAfter = hItem;
		}

		// Check if this is still the first item
		if (hInsertAfter != TVI_SORT)
		{
			// Move this item from the beginning to where it
			// should be
			hParent->hKids = hNewItem->hNext;
			hNewItem->hNext = hInsertAfter->hNext;
			hInsertAfter->hNext = hNewItem;
		}
	}


	if ((hNewItem->hNext == pTree->hTop) && !pTree->m_bVert)
	{

		// there's no scrollbars and we got added before the top
		// item.  we're now the top.
		hNewItem->iShownIndex = 0;
		pTree->hTop = hNewItem;
	}

	if (pTree->m_bRedraw)
	{
		BOOL fVert = pTree->m_bVert;
		RECT rc;
		RECT rc2;

		if (TV_ScrollBarsAfterAdd(pTree, hNewItem))
		{
			// scroll everything down one
			if (ITEM_VISIBLE(hNewItem))
			{
				int iTop = hNewItem->iShownIndex - pTree->hTop->iShownIndex;

				// if there wasn't a scrollbar and we're the 0th item,
				// TV_ScrollBarsAfterAdd already scrolled us
				if (iTop > 0 || !fVert)
				{
					TV_ScrollItems(pTree, TV_GetIntegral(pTree, hNewItem),
						iTop + TV_GetIntegral(pTree, hNewItem) - 1, TRUE);
				}
			}
		}

		// connect the lines, add the buttons, etc. on the item above
		// TV_GetPrevVisItem only works after TV_Scroll* stuff is done

		if (TV_GetItemRect(pTree, hNewItem, &rc, FALSE))
		{

			// find the previous sibling or the parent if no prev sib.
			if (hParent->hKids == hNewItem)
			{
				hItem = hParent;
			}
			else
			{
				hItem = hParent->hKids;
				while (hItem->hNext != hNewItem)
				{
					_ASSERT(hItem->hNext);
					hItem = hItem->hNext;
				}
			}

			// invalidate from there to the new one
			if (TV_GetItemRect(pTree, hItem, &rc2, FALSE))
			{
				rc.top = rc2.top;
			}
			RedrawWindow(GetHostWnd(pTree->m_hWnd), &rc, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
	}

	// DumpItem(hNewItem);

	MyNotifyWinEvent(EVENT_OBJECT_CREATE, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT, (LONG_PTR)hNewItem);

	if (pTree->m_hToolTipItem)
	{
		TV_PopBubble(pTree);
	}

	return hNewItem;
}

// 判断hItem是不是hParent的后代
BOOL TV_IsDescendant(HTREEITEM hParent, HTREEITEM hItem)
{
	if (NULL == hParent || hParent == TVI_ROOT)
	{
		return TRUE;
	}

	BOOL bOk = FALSE;
	hItem = hItem->hParent;
	do
	{
		if (hItem == hParent)
		{
			bOk = TRUE;
			break;
		}
	} while (hItem = hItem->hParent);

	return bOk;
}

BOOL TV_MoveItem(PTREE pTree, LPTV_INSERTSTRUCT lpis)
{
	if (NULL == lpis)
	{
		return FALSE;    //BUGBUG: Validate LPTV_INSERTSTRUCT
	}

	// initialize _after_ the check for NULL!
	HTREEITEM hParent = lpis->hParent;
	HTREEITEM hInsertAfter = lpis->hInsertAfter;
	HTREEITEM hItem = lpis->item.hItem;

	// 记录移动前，记录hItem上一个哥哥，如果没有，记录他的父亲，移动结束后，要修改它们hNext或hKids
	HTREEITEM hOldParentItem = hItem->hParent;
	HTREEITEM hOldPreviousItem = TV_GetNextItem(pTree, hItem, TVGN_PREVIOUS);
	HTREEITEM hOldNextItem = hItem->hNext;

	TV_DismissEdit(pTree, FALSE);

	//
	// 先判断能不能移动
	//
	if (!ValidateTreeItem(hParent, VTI_NULLOK))     // NULL means TVI_ROOT
	{
		return FALSE;
	}
	// 如果是自己插入自己之后，直接返回
	if (hInsertAfter == hItem)
	{
		return FALSE;
	}

	DBG_ValidateTreeItem(hInsertAfter, 0);

	if ((hParent == NULL) || (hParent == TVI_ROOT))
	{
		hParent = pTree->m_hRootItem;
		_ASSERT(pTree->hTop != NULL);
	}
	else if (NULL == pTree->m_hRootItem->hKids)
	{
		return FALSE;
	}

	if (TVI_FIRST != hInsertAfter && TVI_LAST != hInsertAfter && TVI_SORT != hInsertAfter)
	{
		if (hInsertAfter->hParent != hParent)
		{
			// 不是直接孩子
			return FALSE;
		}

		// 如果目标是源的孩子，不能移动
		BOOL bDescendant = TV_IsDescendant(hItem, hInsertAfter);
		if (bDescendant)
		{
			return FALSE;
		}
	}

	//
	// 到这里，表示可以移动，开始移动
	//

	// We will do the sort later, so we can handle TEXTCALLBACK things
	if ((hInsertAfter == TVI_FIRST || hInsertAfter == TVI_SORT) || hParent->hKids == NULL)
	{
		// 插入后父节点第一个孩子
		// 原来的孩子变成新节点弟弟。
		hItem->hNext = hParent->hKids;
		hParent->hKids = hItem;
	}
	else
	{
		if (hInsertAfter == TVI_LAST)
		{
			for (hInsertAfter = hParent->hKids; NULL != hInsertAfter->hNext; hInsertAfter = hInsertAfter->hNext)
			{
				;
			}
		}

		hItem->hNext = hInsertAfter->hNext;
		hInsertAfter->hNext = hItem;
	}

	hItem->hParent = hParent;

	if (NULL != hOldPreviousItem)
	{
		hOldPreviousItem->hNext = hOldNextItem;
	}
	else
	{
		if (NULL != hOldParentItem)
		{
			// 自己原来是父亲第一个孩子
			hOldParentItem->hKids = hOldNextItem;
		}
		else
		{
			_ASSERT(FALSE);
		}
	}

	// 排序
	if ((hInsertAfter == TVI_SORT)
		&& hParent->hKids != NULL
		&& hParent->hKids->hNext != NULL)
	{
		XTVITEM sThisItem, sNextItem;
		TCHAR szThis[64], szNext[64];    // BUGBUG: these are too small

		sThisItem.pszText = szThis;
		sThisItem.cchTextMax = ARRAYSIZE(szThis);
		TV_GetItem(pTree, hItem, TVIF_TEXT, &sThisItem);

		// We know that the first kid of hParent is hNewItem
		HTREEITEM hNext = NULL;
		for (hNext = hItem->hNext; NULL != hNext; hNext = hNext->hNext)
		{
			sNextItem.pszText = szNext;
			sNextItem.cchTextMax = ARRAYSIZE(szNext);

			TV_GetItem(pTree, hNext, TVIF_TEXT, &sNextItem);

			if (lstrcmpi(sThisItem.pszText, sNextItem.pszText) < 0)
			{
				break;
			}

			hInsertAfter = hNext;
		}

		// Check if this is still the first item
		if (hInsertAfter != TVI_SORT)
		{
			// Move this item from the beginning to where it
			// should be
			hParent->hKids = hItem->hNext;
			hItem->hNext = hInsertAfter->hNext;
			hInsertAfter->hNext = hItem;
		}
	}

	if (pTree->m_hToolTipItem)
	{
		TV_PopBubble(pTree);
	}

	pTree->hTop = NULL;
	pTree->m_hEditingItem = NULL;
	pTree->m_hDropTargetItem = NULL;
	pTree->m_hOldDropItem = NULL;
	pTree->m_hHotItem = NULL;
	pTree->m_hSearchItem = NULL;
	pTree->m_hCaretItem = NULL;

	//
	// 重新设置每个Item的缩进。
	//
	int nOldLevel = hItem->iLevel;
	// 每个Item增加的Level
	int nOffsetLevel = hParent->iLevel + 1 - nOldLevel;
	TV_OffsetItemLevelRecurse(hItem, nOffsetLevel);

	//
	// 重新计算展开的Item数量、TreeView的最大宽度、滚动信息
	//
	TV_ScrollBarsAfterMove(pTree);

	//
	// 更新pTree->hTop
	//
	int wOldPos = GetScrollPos(pTree->m_hWnd, SB_VERT);
	TV_SmoothSetTopItem(pTree, wOldPos, 0);

	if (pTree->m_bRedraw)
	{
		InvalidateRect(pTree->m_hWnd, NULL);
	}

	return TRUE;
}

void TV_DeleteHotFonts(PTREE pTree)
{
	if (pTree->hFontHot)
	{
		DeleteObject(pTree->hFontHot);
	}

	if (pTree->hFontBoldHot)
	{
		DeleteObject(pTree->hFontBoldHot);
	}

	pTree->hFontHot = pTree->hFontBoldHot = NULL;
}

// ----------------------------------------------------------------------------
//
//  Frees all allocated memory and objects associated with the tree.
//
// ----------------------------------------------------------------------------

void TV_DestroyTree(PTREE pTree)
{
	_ASSERT(pTree->m_hRootItem);

	pTree->m_bRedraw = FALSE;

	TV_OnSetBkColor(pTree, (COLORREF)(-1));

	if (pTree->m_hHotItemCursor != NULL)
	{
		DestroyCursor(pTree->m_hHotItemCursor);
	}

	if (IsWindow(pTree->hwndToolTips))
	{
		DestroyWindow(pTree->hwndToolTips);
	}

	pTree->hwndToolTips = NULL;

	if (IsWindow(pTree->hwndEdit))
	{
		DestroyWindow(pTree->hwndEdit);
	}
	pTree->hwndEdit = NULL;


	// BUGUBG: send TVN_DELETEALLITEMS and TVDI_NONOTIFY if they respond
	TV_DeleteItem(pTree, pTree->m_hRootItem, TVDI_CHILDRENONLY | TVDI_NOSELCHANGE);

	if (pTree->m_hRootItem)
	{
		Str_Set(&pTree->m_hRootItem->lpstr, NULL);

		// No point in marking dead since the entire control is going away
		ControlFree(pTree->hheap, pTree->m_hRootItem);
	}

	if (pTree->hdcBits != NULL)
	{
		if (pTree->hBmp != NULL)
		{
			SelectObject(pTree->hdcBits, pTree->hStartBmp);
			DeleteObject(pTree->hBmp);
		}

		DeleteDC(pTree->hdcBits);
	}

	if (pTree->m_bCreatedFont && pTree->hFont)
	{
		DeleteObject(pTree->hFont);
	}

	if (pTree->hFontBold)                    //$BOLD
	{
		DeleteObject(pTree->hFontBold);    //$BOLD
	}
	Str_Set(&pTree->pszTip, NULL);
#ifdef UNICODE
	if (pTree->pszTipA)
	{
		LocalFree(pTree->pszTipA);
	}
#endif
	TV_DeleteHotFonts(pTree);

	if (pTree->m_hDpaWatch != NULL)
	{
		IUI_DPA_Destroy(pTree->m_hDpaWatch);
	}

	IncrementSearchFree(&pTree->is);

	// Don't try to use this var when window is destroyed...
	SetMember(pTree->m_hWnd, NULL);

	//pTree->Release();
	delete pTree;
}

void TV_CreateToolTips(PTREE pTree);

HTREEITEM Tree_InsertItem(HWLWND hTree, LPCTSTR lpszItem, HTREEITEM hParent)
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = NULL;
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = (LPTSTR)lpszItem;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.lParam = 0;
	return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvis);
}
// ----------------------------------------------------------------------------
//
//  Allocates space for the tree and initializes the tree's data
//
// ----------------------------------------------------------------------------

LRESULT TV_OnCreate(HWLWND hWnd, LPCREATESTRUCT lpCreate)
{
	PTREE pTree = new TREE;

	if (pTree == NULL)
	{
		return -1;    // fail the create window
	}

#ifdef WIN32
	pTree->hheap = GetProcessHeap();
#endif

	if (!TV_CreateRoot(pTree))
	{
		NearFree((HLOCAL)pTree);
		return -1;      // fail the create window
	}

	pTree->m_hDpaWatch = IUI_DPA_Create(8);
	if (pTree->m_hDpaWatch == NULL)
	{
		// No point in marking dead since the entire control is going away
		ControlFree(pTree->hheap, pTree->m_hRootItem);
		NearFree((HLOCAL)pTree);
		return -1;      // fail the create window
	}

	SetMember(hWnd, pTree);
	pTree->m_hWnd = hWnd;
	pTree->m_hWnd->ci.hwndParent = GetParent(hWnd);

#ifdef WINDOWS_ME
	if (lpCreate->dwExStyle & WS_EX_RTLREADING)
	{
		pTree->m_hWnd->ci.style |= TVS_RTLREADING;
	}
#endif

#ifdef DEBUG
	if (GetAsyncKeyState(VK_SHIFT) < 0 &&
		GetAsyncKeyState(VK_CONTROL) < 0)
	{
		pTree->m_hWnd->ci.style |= TVS_SHOWSELALWAYS; // | TVS_CHECKBOXES;
		SetWindowLong(GetHostWnd(pTree->m_hWnd), GWL_STYLE, pTree->m_hWnd->ci.style);
	}
#endif
	pTree->m_bRedraw = TRUE;
	pTree->clrBk = (COLORREF)(-1);
	pTree->clrText = (COLORREF)(-1);
	pTree->clrim = CLR_DEFAULT;
	pTree->clrLine = CLR_DEFAULT;

	// pTree->m_bHorz        = FALSE;
	// pTree->fVert        = FALSE;
	// pTree->m_bFocus       = FALSE;
	// pTree->m_bNameEditPending = FALSE;
	// pTree->cxMax        = 0;
	// pTree->cxWnd        = 0;
	// pTree->cyWnd        = 0;
	// pTree->hTop         = NULL;
	// pTree->m_hCaretItem       = NULL;
	// pTree->m_hDropTargetItem  = NULL;
	// pTree->m_hOldDropItem     = NULL;
	// pTree->cItems       = 0;
	// pTree->cShowing     = 0;
	pTree->cFullVisible = 1;
	// pTree->hdcBits      = NULL;
	// pTree->hBmp         = NULL;
	// pTree->hbrBk        = NULL;
	// pTree->xPos         = 0;
	// pTree->cxIndent     = 0; // init this for real in TV_OnSetFont()
	// pTree->dwCDDepth    = 0;
	pTree->uMaxScrollTime = SSI_DEFAULT;

	TV_OnSetFont(pTree, NULL, TRUE);

	// You cannot combine TVS_HASLINES and TVS_FULLROWSELECT
	// because it doesn't work
	if (pTree->m_hWnd->ci.style & TVS_HASLINES)
	{
		if (pTree->m_hWnd->ci.style & TVS_FULLROWSELECT)
		{
			//L DebugMsg(DM_ERROR, TEXT("Cannot combine TVS_HASLINES and TVS_FULLROWSELECT"));
		}
		pTree->m_hWnd->ci.style &= ~TVS_FULLROWSELECT;
	}

	if (!(pTree->m_hWnd->ci.style & TVS_NOTOOLTIPS))
	{
		TV_CreateToolTips(pTree);
	}

	SetNotify(hWnd, FALSE);
	SetScrollRange(hWnd, SB_HORZ, 0, 0, TRUE);
	SetScrollRange(hWnd, SB_VERT, 0, 0, TRUE);
	SetNotify(hWnd, TRUE);

	pTree->m_bSmoothVScroll = IsIncludeFlag(pTree->m_hWnd->ci.dwExStyle, TVS_EX_SMOOTHVSCROLL);
	pTree->wSmoothVScroll = 20;
	if (pTree->m_bSmoothVScroll)
	{
		pTree->m_nMouseWheelStep = 20;
	}
	else
	{
		pTree->m_nMouseWheelStep = 1;
	}

	return 0;   // success
}


void TV_CreateToolTips(PTREE pTree)
{
#if defined(MAINWIN)
	/* IUENIX : On CDE, when tooltip comes up, WM activates tooltip window causing some
	flashing due to activation/deactivation of various other windows.  Hence asking WM
	not to manage such tooltip windows */
	DWORD exStyle = WS_EX_MW_UNMANAGED_WINDOW;
#else
	DWORD exStyle = 0;
#endif

#ifdef WINDOWS_ME
	if (pTree->m_hWnd->ci.style & TVS_RTLREADING)
	{
		exStyle |= WS_EX_RTLREADING;
	}
#endif // WINDOWS_ME

	pTree->hwndToolTips = CreateWindowEx(exStyle, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		GetHostWnd(pTree->m_hWnd), NULL, NULL,
		NULL);
	if (pTree->hwndToolTips)
	{
		TOOLINFO ti;

		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT;
		ti.hwnd = GetHostWnd(pTree->m_hWnd);
		ti.uId = (UINT_PTR)GetHostWnd(pTree->m_hWnd);
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.lParam = 0;
		SendMessage(pTree->hwndToolTips, TTM_ADDTOOL, 0,
			(LPARAM)(LPTOOLINFO)&ti);
		SendMessage(pTree->hwndToolTips, WM_SETFONT, (WPARAM)pTree->hFont, (LPARAM)TRUE);
#if 0
		SendMessage(pTree->hwndToolTips, TTM_SETTIPBKCOLOR, (WPARAM)GetSysColor(COLOR_WINDOW), 0);
		SendMessage(pTree->hwndToolTips, TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_WINDOWTEXT), 0);
#endif
		SendMessage(pTree->hwndToolTips, TTM_SETDELAYTIME, TTDT_INITIAL, (LPARAM)500);
	}
	else
	{
		pTree->m_hWnd->ci.style |= (TVS_NOTOOLTIPS);
	}
}


//////////////////////////////////////////////////////////////////////////
// TreeView Paint

extern void  TruncateString(char *sz, int cch);

void TV_GetBackgroundBrush(PTREE pTree, HDC hdc)
{
	if (pTree->clrBk == (COLORREF)(-1))
	{
		if (pTree->m_hWnd->ci.style & WS_DISABLED)
		{
			pTree->hbrBk = CreateSolidBrush(RGB(192, 192, 192));
		}
		else
		{
			pTree->hbrBk = CreateSolidBrush(RGB(255, 255, 255));
		}
	}
}

// ----------------------------------------------------------------------------
//
//  绘制从给定点(x,y)出发的指定长度c的水平或垂直虚线
//
// ----------------------------------------------------------------------------
void TV_DrawDottedLine(HDC hdc, int x, int y, int c, BOOL fVert)
{
	while (c > 0)
	{
		PatBlt(hdc, x, y, 1, 1, PATCOPY);
		if (fVert)
		{
			y += 2;
		}
		else
		{
			x += 2;
		}
		c -= 2;
	}
}


// ----------------------------------------------------------------------------
//
//  Draws a plus or minus sign centered around the given (x,y) location and
//  extending out from that location the given distance (c).
//
// ----------------------------------------------------------------------------

void TV_DrawPlusMinus(HDC hdc, int x, int y, int c, HBRUSH hbrSign, HBRUSH hbrBox, HBRUSH hbrBk, BOOL fPlus)
{
	int n;
	int p = (c * 7) / 10;

	n = p * 2 + 1;

	SelectObject(hdc, hbrSign);

	if (p >= 5)
	{
		PatBlt(hdc, x - p, y - 1, n, 3, PATCOPY);
		if (fPlus)
		{
			PatBlt(hdc, x - 1, y - p, 3, n, PATCOPY);
		}

		SelectObject(hdc, hbrBk);
		p--;
		n -= 2;
	}

	PatBlt(hdc, x - p, y, n, 1, PATCOPY);
	if (fPlus)
	{
		PatBlt(hdc, x, y - p, 1, n, PATCOPY);
	}

	n = c * 2 + 1;

	SelectObject(hdc, hbrBox);

	PatBlt(hdc, x - c, y - c, n, 1, PATCOPY);
	PatBlt(hdc, x - c, y - c, 1, n, PATCOPY);
	PatBlt(hdc, x - c, y + c, n, 1, PATCOPY);
	PatBlt(hdc, x + c, y - c, 1, n, PATCOPY);
}


// ----------------------------------------------------------------------------
//
//  创建缩进区域的位图，例如+-号，连接虚线等
//  TreeView控件的这些位图，是通过GDI函数绘制出来，然后保存到TREE::hdcBits中
//  如果满足条件：  fHasLines &&  fHasButtons，则有 7 种样式的缩进图案
//  如果满足条件：  fHasLines && !fHasButtons，则有 3 种样式的缩进图案
//  如果满足条件：  !fHasLines &&  fHasButtons，则有 2 种样式的缩进图案
//
//  本函数会设置TREE结构中的以下变量：hStartBmp, hBmp, hdcBits
//
//  If "has lines" then there are three basic bitmaps.
//
//      |       |       |
//      |       +---    +---
//      |       |
//
//  (The plan vertical line does not get buttons.)
//
//  Otherwise, there are no lines, so the basic bitmaps are blank.
//
//  If "has buttons", then the basic bitmaps are augmented with buttons.
//
//       [+]      [-]
//
//  And if you have "lines at root", you get
//
//      __
//
//
//  And if you have "lines at root" with "has buttons", then you also get
//
//      --[+]   --[-]
//
//  So, there are twelve image types.  Here they are, with the code names
//  written underneath.
//
//      |       |       |       |       |       |       |
//      |       +---    +---   [+]--   [+]--   [-]--   [-]--
//      |       |               |               |
//
//     "|"     "|-"    "L"     "|-+"   "L+"    "|--"   "L-"
//
//      ---    [+]--   [-]--   [+]     [-]
//
//     ".-"    ".-+"   ".--"   "+"     "-"
//
//      And the master table of which styles get which images.
//
//
//  LINES   BTNS    ROOT    |   |-  L   |-+ L+  |-- L-  .-  .-+ .-- +   -
//
//           x                                                      0   1
//    x                     0   1   2                   3
//    x                     0   1   2                   3
//    x      x              0   1   2   3   4   5   6
//    x              x      0   1   2                   3
//    x      x       x      0   1   2   3   4   5   6   7   8   9
//
// ----------------------------------------------------------------------------
void TV_CreateIndentBmps(PTREE pTree)
{
	if (pTree->m_bRedraw)
	{
		InvalidateRect(pTree->m_hWnd, NULL);
	}

	// 缩进区图案种类
	int cnt = 0;
	if (pTree->m_hWnd->ci.style & TVS_HASLINES)
	{
		if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
		{
			cnt = 7;    //   | |-  L   |-+ L+  |-- L-
		}
		else
		{
			cnt = 3;    //   | |-  L
		}

		if (pTree->m_hWnd->ci.style & TVS_LINESATROOT)
		{
			if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
			{
				cnt += 3;    // -  -+ --
			}
			else
			{
				cnt += 1;    // -
			}
		}
	}
	else if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
	{
		cnt = 2;
	}
	else
	{
		return;
	}

	if (pTree->hdcBits == NULL)
	{
		pTree->hdcBits = CreateCompatibleDC(NULL);
	}

	HDC hdc = pTree->hdcBits;

	// Get a new background brush, just like an Edit does.

	TV_GetBackgroundBrush(pTree, hdc);

	HBITMAP hBmpOld = pTree->hBmp;
	pTree->hBmp = CreateColorBitmap(cnt * pTree->cxIndent, pTree->cyItem);
	if (hBmpOld)
	{
		SelectObject(hdc, pTree->hBmp);
		DeleteObject(hBmpOld);
	}
	else
	{
		pTree->hStartBmp = (HBITMAP)SelectObject(hdc, pTree->hBmp);
	}

	HBRUSH hbrLine = NULL;
	if (pTree->clrLine != CLR_DEFAULT)
	{
		hbrLine = CreateSolidBrush(pTree->clrLine);
	}
	else
	{
		hbrLine = IUIGetGloabelVar()->g_hbrGrayText;
	}

	HBRUSH hbrText = NULL;
	if (pTree->clrText != (COLORREF)(-1))
	{
		hbrText = CreateSolidBrush(pTree->clrText);
	}
	else
	{
		hbrText = IUIGetGloabelVar()->g_hbrWindowText;
	}

	HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, hbrLine);

	// 存放缩进区域各种图案的位图的尺寸
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = cnt * pTree->cxIndent;
	rc.bottom = pTree->cyItem;

	// 如果hbrBk为NULL，则填充成白色
	FillRect(hdc, &rc, pTree->hbrBk);
	int x = 0;

	int xMid = 0;
	if (pTree->m_hImageList)
	{
		xMid = (pTree->cxImage - TREE_INDENT) / 2;
	}
	else
	{
		xMid = pTree->cxIndent / 2;
	}

	int yMid = ((pTree->cyItem / 2) + 1) & ~1;

	int c = (min(xMid, yMid)) / 2;

	// 绘制缩进区域的图案
	if (pTree->m_hWnd->ci.style & TVS_HASLINES)
	{
		TV_DrawDottedLine(hdc, x + xMid, 0, pTree->cyItem, TRUE);
		x += pTree->cxIndent;

		TV_DrawDottedLine(hdc, x + xMid, 0, pTree->cyItem, TRUE);
		TV_DrawDottedLine(hdc, x + xMid, yMid, pTree->cxIndent - xMid, FALSE);
		x += pTree->cxIndent;

		TV_DrawDottedLine(hdc, x + xMid, 0, yMid, TRUE);
		TV_DrawDottedLine(hdc, x + xMid, yMid, pTree->cxIndent - xMid, FALSE);
		x += pTree->cxIndent;
	}

	if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
	{
		BOOL fPlus = TRUE;

		x += xMid;

	doDrawPlusMinus:
		TV_DrawPlusMinus(hdc, x, yMid, c, hbrText, hbrLine, pTree->hbrBk, fPlus);

		if (pTree->m_hWnd->ci.style & TVS_HASLINES)
		{
			TV_DrawDottedLine(hdc, x, 0, yMid - c, TRUE);
			TV_DrawDottedLine(hdc, x + c, yMid, pTree->cxIndent - xMid - c, FALSE);
			TV_DrawDottedLine(hdc, x, yMid + c, yMid - c, TRUE);

			x += pTree->cxIndent;

			TV_DrawPlusMinus(hdc, x, yMid, c, hbrText, hbrLine, pTree->hbrBk, fPlus);

			TV_DrawDottedLine(hdc, x, 0, yMid - c, TRUE);
			TV_DrawDottedLine(hdc, x + c, yMid, pTree->cxIndent - xMid - c, FALSE);
		}

		x += pTree->cxIndent;

		if (fPlus)
		{
			fPlus = FALSE;
			goto doDrawPlusMinus;
		}
		x -= xMid;
	}

	if (pTree->m_hWnd->ci.style & TVS_LINESATROOT)
	{

		// -
		TV_DrawDottedLine(hdc, x + xMid, yMid, pTree->cxIndent - xMid, FALSE);
		x += pTree->cxIndent;

		if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
		{
			x += xMid;
			TV_DrawPlusMinus(hdc, x, yMid, c, hbrText, hbrLine, pTree->hbrBk, TRUE);
			TV_DrawDottedLine(hdc, x + c, yMid, pTree->cxIndent - xMid - c, FALSE);
			x += pTree->cxIndent;

			TV_DrawPlusMinus(hdc, x, yMid, c, hbrText, hbrLine, pTree->hbrBk, FALSE);
			TV_DrawDottedLine(hdc, x + c, yMid, pTree->cxIndent - xMid - c, FALSE);
			//  uncomment if there's more to be added
			//x += pTree->cxIndent - xMid;

		}
	}

	if (hbrOld)
	{
		SelectObject(pTree->hdcBits, hbrOld);
	}

	if (pTree->clrLine != CLR_DEFAULT)
	{
		DeleteObject(hbrLine);
	}
	if (pTree->clrText != (COLORREF)(-1))
	{
		DeleteObject(hbrText);
	}
}


// ----------------------------------------------------------------------------
//
//  fills in a XTVITEM structure based by coying data from the item or
//  by calling the callback to get it.
//
//  in:
//	hItem	item to get XTVITEM struct for
//	mask	which bits of the XTVITEM struct you want (TVIF_ flags)
//  out:
//	lpItem	XTVITEM filled in
//
// ----------------------------------------------------------------------------

void TV_GetItem(PTREE pTree, HTREEITEM hItem, UINT mask, LPXTVITEM lpItem)
{
	TV_DISPINFO nm;

	if (!hItem || !lpItem)
	{
		return;
	}

	DBG_ValidateTreeItem(hItem, FALSE);

	nm.item.mask = 0;

	// We need to check the mask to see if lpItem->pszText is valid
	// And even then, it might not be, so be paranoid
	if ((mask & TVIF_TEXT) && lpItem->pszText && lpItem->cchTextMax)
	{
		if (hItem->lpstr == LPSTR_TEXTCALLBACK)
		{
			nm.item.mask |= TVIF_TEXT;
			// caller had to fill in pszText and cchTextMax with valid data
			nm.item.pszText = lpItem->pszText;
			nm.item.cchTextMax = lpItem->cchTextMax;
			nm.item.pszText[0] = 0;

		}
		else
		{
			_ASSERT(hItem->lpstr);
			// we could do this but this is dangerous (when responding
			// to TVM_GETITEM we would be giving the app a pointer to our data)
			// lpItem->pszText = hItem->lpstr;
			lstrcpyn(lpItem->pszText, hItem->lpstr, lpItem->cchTextMax);
#ifndef UNICODE
			// only call truncate string if the source string is larger than the dest buffer
			// this is to deal with corel draw who passes in a bogus cchTextMax value
			//
			// We used to always call TruncateString when cchTextMax is MAX_PATH, but
			// McAfee Virus program (QFE1381) passes MAX_PATH with a smaller than MAX_PATH buffer
			// so we must always check the string length first.  They luck out
			// and lstrlen(hItem->lpstr) is smaller than max path so we don't truncate.
			//
			if (lstrlen(hItem->lpstr) >= lpItem->cchTextMax)
			{
				// takes care of broken dbcs sequence, note lstrcpyn puts nul at
				// cchTextMax-1 if exceeded
				TruncateString(lpItem->pszText, lpItem->cchTextMax);
			}
#endif
		}

	}

	if (mask & TVIF_IMAGE)
	{
		if (hItem->iImage == (WORD)I_IMAGECALLBACK)
		{
			nm.item.mask |= TVIF_IMAGE;
		}
		else
		{
			lpItem->iImage = hItem->iImage;
		}
	}

	if (mask & TVIF_SELECTEDIMAGE)
	{
		if (hItem->iSelectedImage == (WORD)I_IMAGECALLBACK)
		{
			nm.item.mask |= TVIF_SELECTEDIMAGE;
		}
		else
		{
			lpItem->iSelectedImage = hItem->iSelectedImage;
		}
	}

	if (mask & TVIF_INTEGRAL)
	{
		lpItem->iIntegral = hItem->iIntegral;
	}

	if (mask & TVIF_BINDWL)
	{
		lpItem->hBindWindowless = hItem->hBindWindowless;
	}

	if (mask & TVIF_CHILDREN)
	{
		switch (hItem->fKids)
		{
		case KIDS_COMPUTE:
			lpItem->cChildren = hItem->hKids ? 1 : 0;// the actual count doesn't matter
			break;

		case KIDS_FORCE_YES:
			lpItem->cChildren = 1;// the actual count doesn't matter
			break;

		case KIDS_FORCE_NO:
			lpItem->cChildren = 0;
			break;

		case KIDS_CALLBACK:
			nm.item.mask |= TVIF_CHILDREN;
			break;
		}
	}

	// copy out constant parameters (and prepare for callback)
	// IE4 and IE5.0 did this unconditionally
	lpItem->state = nm.item.state = hItem->state;

	//
	//  NOTICE!  We do not set TVIF_STATE nm.item.mask and we do not
	//  check for TVIF_STATE in the "any items need to be filled in
	//  by callback?" test a few lines below.  This is necessary for
	//  backwards compat.  IE5 and earlier did not call the app back
	//  if the only thing you asked for was TVIF_STATE.  You can't
	//  change this behavior unless you guard it with a version check, or
	//  apps will break.  (They'll get callbacks when they didn't used to.)
	//  Besides, nobody knows that they can customize the state, so it's
	//  not like we're missing out on anything.
	//

#ifdef DEBUG_TEST_BOLD
	if ((((int)hItem) / 100) % 2)
	{
		lpItem->state |= TVIS_BOLD;
	}
	if (!pTree->hFontBold)
	{
		TV_CreateBoldFont(pTree);
	}
#endif

	lpItem->lParam = nm.item.lParam = hItem->lParam;

	// any items need to be filled in by callback?
	if (nm.item.mask & (TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN))
	{
		nm.item.hItem = hItem;

		CCSendNotify(&pTree->m_hWnd->ci, TVN_GETDISPINFO, &nm.hdr);

		// copy out things that may have been filled in on the callback
		if (nm.item.mask & TVIF_CHILDREN)
		{
			lpItem->cChildren = nm.item.cChildren;
		}
		if (nm.item.mask & TVIF_IMAGE)
		{
			lpItem->iImage = nm.item.iImage;
		}
		if (nm.item.mask & TVIF_SELECTEDIMAGE)
		{
			lpItem->iSelectedImage = nm.item.iSelectedImage;
		}
		// callback may have redirected pszText to point into its own buffer
		if (nm.item.mask & TVIF_TEXT)
			if (mask & TVIF_TEXT) // did the *original* mask specify TVIF_TEXT?
			{
				lpItem->pszText = CCReturnDispInfoText(nm.item.pszText, lpItem->pszText, lpItem->cchTextMax);
			}
			else
			{
				lpItem->pszText = nm.item.pszText;    // do what we used to do
			}
		if (nm.item.mask & TVIF_STATE)
		{
			lpItem->state = (nm.item.state & nm.item.stateMask) | (lpItem->state & ~nm.item.stateMask);
			if ((lpItem->state & TVIS_BOLD) && !pTree->hFontBold)
			{
				TV_CreateBoldFont(pTree);
			}
		}


		if (nm.item.mask & TVIF_DI_SETITEM)
		{

			if (nm.item.mask & TVIF_TEXT)
				if (nm.item.pszText)
				{
					_ASSERT(hItem->lpstr == LPSTR_TEXTCALLBACK);
					Str_Set(&hItem->lpstr, nm.item.pszText);
				}
			if (nm.item.mask & TVIF_STATE)
			{
				// if the bold bit changed, then the width changed
				if ((hItem->state ^ lpItem->state) & TVIS_BOLD)
				{
					hItem->iWidth = 0;
				}
				hItem->state = (WORD)lpItem->state;
			}
			if (nm.item.mask & TVIF_IMAGE)
			{
				hItem->iImage = (WORD)lpItem->iImage;
			}
			if (nm.item.mask & TVIF_SELECTEDIMAGE)
			{
				hItem->iSelectedImage = (WORD)lpItem->iSelectedImage;
			}
			if (nm.item.mask & TVIF_CHILDREN)
			{
				switch (nm.item.cChildren)
				{
				case I_CHILDRENCALLBACK:
					hItem->fKids = KIDS_CALLBACK;
					break;

				case 0:
					hItem->fKids = KIDS_FORCE_NO;
					break;

				default:
					hItem->fKids = KIDS_FORCE_YES;
					break;
				}

			}
		}
	}
}


// ----------------------------------------------------------------------------
//
//  返回Item是否应该绘制蓝色背景。
//  当Item作为拖放目标选中时，返回TRUE；
//  当Tree有效，并且有焦点，且Item被选中，返回TRUE
//
// ----------------------------------------------------------------------------
BOOL TV_ShouldItemDrawBlue(PTREE pTree, XTVITEM *ti, UINT flags)
{
	return ((ti->state & TVIS_DROPHILITED)
		|| (pTree->m_hDropTargetItem == NULL && !(flags & TVDI_GRAYCTL) && (ti->state & TVIS_SELECTED) && pTree->m_bFocus));
}

#define TV_ShouldItemDrawDisabled(pTree, pti, flags) (flags & TVDI_GRAYCTL)

//
//  Caution:  Depending on the user's color scheme, a Gray item may
//  end up looking Blue if Gray would otherwise be invisible.  So make
//  sure that there are other cues that the user can use to tell whether
//  the item is "Really Blue" or "Gray masquerading as Blue".
//
//  For example, you might get both is if the treeview is
//  participating in drag/drop while it is not the active window,
//  because the selected item gets "Gray masquerading as Blue" and
//  the drop target gets "Really Blue".  But we special-case that
//  and turn off the selection while we are worrying about drag/drop,
//  so there is no confusion after all.
//
BOOL TV_ShouldItemDrawGray(PTREE pTree, XTVITEM *pti, UINT flags)
{
	return ((flags & TVDI_GRAYCTL)
		|| (pTree->m_hDropTargetItem == NULL && ((pti->state & TVIS_SELECTED) && (!pTree->m_bFocus && (pTree->m_hWnd->ci.style & TVS_SHOWSELALWAYS)))));
}

//
//  Draw a descender line for the item.  It is the caller's job to
//  draw the appropriate glyph at level 0.
//
void TV_DrawDescender(PTREE pTree, HDC hdc, int x, int y, HTREEITEM hItem)
{
	int i;

	if (pTree->m_bSmoothVScroll)
	{
		int nTimes = TV_GetIntegral(pTree, hItem) / pTree->cyItem;
		int i = 1;
		for (; i < nTimes; i++)
		{
			TransparentBlt(hdc, x, y + i * pTree->cyItem, pTree->cxIndent, pTree->cyItem,
				pTree->hdcBits, 0, 0, pTree->cxIndent, pTree->cyItem, RGB(255, 255, 255));
		}

		int nLast = TV_GetIntegral(pTree, hItem) % pTree->cyItem;
		TransparentBlt(hdc, x, y + i * pTree->cyItem, pTree->cxIndent, nLast,
			pTree->hdcBits, 0, 0, pTree->cxIndent, nLast, RGB(255, 255, 255));
	}
	else
	{
		for (i = 1; i < TV_GetIntegral(pTree, hItem); i++)
		{
			TransparentBlt(hdc, x, y + i * pTree->cyItem, pTree->cxIndent, pTree->cyItem,
				pTree->hdcBits, 0, 0, pTree->cxIndent, pTree->cyItem, RGB(255, 255, 255));
		}
	}
}

//
//  Draw (or erase) descenders for siblings and children.
//
void TV_DrawKinDescender(PTREE pTree, HDC hdc, int x, int y, HTREEITEM hItem, UINT state)
{
	if (hItem->hNext != NULL)   // Connect to next sibling
	{
		TV_DrawDescender(pTree, hdc, x, y, hItem);
	}

	// If any bonus images, then need to connect the image to the kids.
	if (pTree->m_hImlState != NULL || pTree->m_hImageList != NULL)
	{
		if (state & (TVIS_EXPANDED | TVIS_EXPANDPARTIAL)) // Connect to expanded kids
		{
			TV_DrawDescender(pTree, hdc, x + pTree->cxIndent, y, hItem);
		}
	}
}

// TODO: 如果hItem指定了自己的背景，要画它自己的。
void TV_DrawItemBk(PTREE pTree, HTREEITEM hItem, HDC hdc, int eItemType, LPCRECT lpRect)
{
	BOOL bEnabled = IsWindowEnabled(pTree->m_hWnd);

	// Item背景类型是否是颜色
	BOOL bDrawColor = TRUE;
	COLORREF crBkItem = IUIGetGloabelVar()->g_clrHighlight;
	BOOL bCombineItemBkImage = TRUE;
	HIUIIMAGE himgBkItem = NULL;
	IMAGE_RESIZE_MODE eImageResizeMode = IRM_9GRID;
	CIUIRect rcItemBkImage9GridArg;
	int nCombineBkItemImagePartCount = 2;
	int nDrawCombineBkItemImagePart = 0;

	if (ITEM_FOCUS == eItemType)
	{
		if (BKT_COLOR == pTree->m_eFocusItemBkType)
		{
			crBkItem = pTree->m_crFocusItemBK;
		}
		else if (BKT_IMAGE == pTree->m_eFocusItemBkType)
		{
			bDrawColor = FALSE;
			bCombineItemBkImage = FALSE;

			himgBkItem = IsWindowEnabled(pTree->m_hWnd) ? pTree->m_himgFocusItemBk : NULL;
			rcItemBkImage9GridArg = pTree->m_rcFocusItemBkImage9GridArg;
		}
	}
	else if (ITEM_SELECTED == eItemType)
	{
		if (BKT_COLOR == pTree->m_eSelectedItemBkType)
		{
			crBkItem = pTree->m_crSelectedItemBK;
		}
		else if (BKT_IMAGE == pTree->m_eSelectedItemBkType)
		{
			bDrawColor = FALSE;

			if (pTree->m_bSelectedItemBkCombine)
			{
				himgBkItem = pTree->m_himgSelectedItemCombineBk;
				nCombineBkItemImagePartCount = 2;
				nDrawCombineBkItemImagePart = bEnabled ? 0 : 1;
			}
			else
			{
				bCombineItemBkImage = FALSE;

				himgBkItem = IsWindowEnabled(pTree->m_hWnd) ? pTree->m_himgSelectedItemBk[0] : pTree->m_himgSelectedItemBk[1];
				rcItemBkImage9GridArg = pTree->m_rcSelectedItemBkImage9GridArg;
			}
		}
	}
	else if (ITEM_HIGHLIGHT == eItemType)
	{
		if (BKT_COLOR == pTree->m_eHighlightItemBkType)
		{
			crBkItem = pTree->m_crHighlightItemBK;
		}
		else if (BKT_IMAGE == pTree->m_eHighlightItemBkType)
		{
			bDrawColor = FALSE;

			if (pTree->m_bHighlightItemBkCombine)
			{
				himgBkItem = pTree->m_himgHighlightItemCombineBk;
				nCombineBkItemImagePartCount = 2;
				nDrawCombineBkItemImagePart = bEnabled ? 0 : 1;
			}
			else
			{
				bCombineItemBkImage = FALSE;

				himgBkItem = IsWindowEnabled(pTree->m_hWnd) ? pTree->m_himgHighlightItemBk[0] : pTree->m_himgHighlightItemBk[1];
				rcItemBkImage9GridArg = pTree->m_rcHighlightItemBkImage9GridArg;
			}
		}
	}

	if (bDrawColor)
	{
		FillSolidRect(hdc, lpRect, crBkItem,
			IsUpdateLayeredWindow(GetHostWnd(pTree->m_hWnd)) ? TRUE : FALSE);
	}
	else
	{
		if (bCombineItemBkImage)
		{
			IUIPartDrawImage(hdc, lpRect, himgBkItem, eImageResizeMode, rcItemBkImage9GridArg,
				nCombineBkItemImagePartCount, nDrawCombineBkItemImagePart);
		}
		else
		{
			IUIDrawImage(hdc, lpRect, himgBkItem, eImageResizeMode, rcItemBkImage9GridArg);
		}
	}
}


void TV_DrawItem(PTREE pTree, HTREEITEM hItem, HDC hdc, int x, int y, UINT flags)
{
	BOOL bFullRowSelected = IsIncludeFlag(pTree->m_hWnd->ci.style, TVS_FULLROWSELECT);

	UINT cxIndent = pTree->cxIndent;
	HTREEITEM hItemSave = hItem;

	BOOL fItemFocused = ((pTree->m_bFocus) && (hItem == pTree->m_hCaretItem));

	//
	// 计算Item全尺寸坐标
	//
	RECT rc;
	rc.top = y;
	if (pTree->m_bSmoothVScroll)
	{
		rc.bottom = rc.top + TV_GetIntegral(pTree, hItem);
	}
	else
	{
		rc.bottom = rc.top + (pTree->cyItem * TV_GetIntegral(pTree, hItem));
	}
	rc.left = 0;
	rc.right = pTree->cxWnd;

	// 确保回调函数不要无效这个Item
	pTree->hItemPainting = hItem;

	// 得到Item信息
	XTVITEM ti;
	TCHAR szTemp[MAX_PATH] = { 0 };
	ti.pszText = szTemp;
	ti.cchTextMax = ARRAYSIZE(szTemp);
	ti.stateMask = TVIS_OVERLAYMASK | TVIS_CUT | TVIS_BOLD; //$BOLD
	TV_GetItem(pTree, hItem,
		TVIF_IMAGE | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM | TVIF_BINDWL,
		&ti);

	pTree->hItemPainting = NULL;

	// 为+-号腾出空间
	if (!(flags & TVDI_NOTREE))
	{
		if ((pTree->m_hWnd->ci.style & (TVS_HASLINES | TVS_HASBUTTONS))
			&& (pTree->m_hWnd->ci.style & TVS_LINESATROOT))
		{
			x += cxIndent;
		}
	}

	// 处理边距等
	int iLevel = 0;
	if (flags & TVDI_NOTREE)
	{
		iLevel = 0;
	}
	else
	{
		iLevel = hItem->iLevel;
	}

	x += (pTree->cxBorder + (iLevel * cxIndent));
	y += pTree->cyBorder;

	int iState = 0;
	DWORD dwRet = 0;
	NMTVCUSTOMDRAW nmcd;

	//
	// 准备Item状态
	//
	int iBack = 0;
	int iText = 0;
	BOOL fItemSelected = IsIncludeFlag(ti.state, TVIS_SELECTED);
	BOOL fShowSelAlways = IsIncludeFlag(pTree->m_hWnd->ci.style, TVS_SHOWSELALWAYS);
	BOOL fHot = (hItem == pTree->m_hHotItem);

	{
		// 选中状态
		if (TV_ShouldItemDrawBlue(pTree, &ti, flags))
		{
			// selected
			iBack = COLOR_HIGHLIGHT;
			iText = COLOR_WINDOWTEXT;
		}
		// 强制灰
		else if (TV_ShouldItemDrawDisabled(pTree, &pti, flags))
		{
			iBack = COLOR_3DFACE;
			iText = COLOR_GRAYTEXT;
		}
		// Disabled状态
		else if (TV_ShouldItemDrawGray(pTree, &ti, flags))
		{
			// On some color schemes, the BTNFACE color equals the WINDOW color,
			// and our gray comes out invisible.  In such case, change from gray
			// to blue so you can see it at all.
			if (GetSysColor(COLOR_WINDOW) != GetSysColor(COLOR_BTNFACE))
			{
				iBack = COLOR_BTNFACE;
				iText = COLOR_BTNTEXT;
			}
			else
			{
				iBack = COLOR_HIGHLIGHT;
				iText = COLOR_HIGHLIGHTTEXT;
			}
		}
		// Normal状态
		else
		{
			// not selected
			iBack = COLOR_WINDOW;
			iText = COLOR_WINDOWTEXT;

			// Hover状态
			if (hItem == pTree->m_hHotItem)
			{
				iText = COLOR_HOTLIGHT;
			}
		}
	}

	//
	// 绘制Item背景，如果是Full Row Selected的Tree，则在此处绘制Item背景。
	// 如果不是，则在绘制文本前绘制Item背景。
	//
	if (bFullRowSelected)
	{
		if (pTree->m_bFocus || fShowSelAlways)
		{
			// 如果Item即是焦点Item,也是选中Item，则焦点Item优先
			if (fItemFocused)
			{
				TV_DrawItemBk(pTree, hItem, hdc, ITEM_FOCUS, &rc);
			}
			else if (fItemSelected)
			{
				TV_DrawItemBk(pTree, hItem, hdc, ITEM_SELECTED, &rc);
			}
		}

		// Tree控件未得到焦点时，可也在设置Highlight item.
		if (fHot)
		{
			TV_DrawItemBk(pTree, hItem, hdc, ITEM_HIGHLIGHT, &rc);
		}
	}

	// 如果Item绑定了控件，则只绘制控件
	// Item绑定的控件不走HWLWND常规绘制流程。 因为Tree只需要绘制可视范围内的Item
	if (hItem->hBindWindowless != NULL)
	{
		// 绑定的控件，占用图标和文本区域，因为这两个区域点击后，都可以选中Item。
		// TODO: 研究一下，绑定的控件从图标区的left开始，宽度为控件自己的宽度是否更加合理，
		// 比如把一个宽100的按钮绑定到Item上，那么按钮应该呈现出来就是100，而不是图标和文本的总宽度。
		rc.left = x;
		rc.top = y + pTree->cyBorder;
		if (IsIncludeFlag(pTree->m_hWnd->ci.style, TVS_FULLROWSELECT))
		{
			rc.right = pTree->cxWnd;
		}
		else
		{
			rc.right = min((x + hItem->iWidth), (pTree->cxMax - pTree->cxBorder));
		}
		rc.bottom -= pTree->cyBorder;

		if (pTree->m_hImageList != NULL)
		{
			// 即使没有绘制Image，文本区域也需要右移
			rc.right += pTree->cxImage;
		}

		CIUIRect rcOld;
		GetWindowRectToParent(hItem->hBindWindowless, rcOld);

		if (rcOld != rc)
		{
			MoveWindow(hItem->hBindWindowless, &rc, FALSE);
		}

		// 备份视口、裁剪区域
		POINT ptOrg;
		GetViewportOrgEx(hdc, &ptOrg);

		HRGN hOldRgn = CreateRectRgn(0, 0, 0, 0);
		GetClipRgn(hdc, hOldRgn);

		// 绘制绑定的控件
		DrawControlAndChildren(hItem->hBindWindowless, hdc, &rc);

		// 恢复视口：
		// 注意：IUI默认绘制机制是先绘制父控件，等父控件整个绘制完后，再绘制子控件。
		// 虽然子控件绘制时，会修改HDC的视口，但这时父控件已彻底绘制完了，所以子控件
		// 修改HDC视口，也不会影响父控件的绘制。
		// 但由于ListView的特殊性，ListView是在绘制自己的时候，同时把Item绑定的控件
		// 进行了绘制，而在绘制完Item绑定的控件后，HDC的视口已不是ListView的视口了，
		// 而这时，ListView还未绘制完，这将导致ListView之后的绘制位置全部错误。
		SetViewportOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
		SelectClipRgn(hdc, hOldRgn);
		DeleteObject(hOldRgn);
		hOldRgn = NULL;
	}
	else
	{
		//
		// 文本颜色
		//
		DWORD clrTextTemp = 0;
		if (iText == COLOR_WINDOWTEXT && (pTree->clrText != COLORREF(-1)))
		{
			nmcd.clrText = clrTextTemp = pTree->clrText;
		}
		else
		{
			nmcd.clrText = clrTextTemp = GetSysColor(iText);
		}

		// if forcing black and transparent, do so.  dc's BkMode should
		// already be set to TRANSPARENT by caller
		UINT etoFlags = ETO_OPAQUE | ETO_CLIPPED;
		if (flags & TVDI_TRANSTEXT)
		{
			nmcd.clrText = clrTextTemp = 0x000000;
			etoFlags = 0;			// don't opaque nothin'
		}

		COLORREF rgbOldText = SetTextColor(hdc, nmcd.clrText);

		if (pTree->m_hWnd->ci.style & TVS_RTLREADING)
		{
			etoFlags |= ETO_RTLREADING;
		}

		//
		// 字体
		//
		HFONT hFont = NULL;                        //$BOLD
		if (ti.state & TVIS_BOLD)
		{
			hFont = pTree->hFontBold;
			if (hItem == pTree->m_hHotItem)
			{
				hFont = CCGetHotFont(pTree->hFontBold, &pTree->hFontBoldHot);
			}
		}
		else
		{
			hFont = pTree->hFont;
			if (hItem == pTree->m_hHotItem)
			{
				hFont = CCGetHotFont(pTree->hFont, &pTree->hFontHot);
			}
		}
		hFont = (HFONT)SelectObject(hdc, hFont);

		//
		// 向父亲发送NM_CUSTOMDRAW消息，给父亲一个自绘TreeView Item的机会
		//
		BOOL fSelectedIcon = FALSE;
		{
			nmcd.nmcd.hdc = hdc;
			nmcd.nmcd.dwItemSpec = (DWORD_PTR)hItem;
			nmcd.nmcd.uItemState = 0;
			nmcd.nmcd.rc = rc;
			if (flags & TVDI_NOTREE)
			{
				nmcd.iLevel = 0;
			}
			else
			{
				nmcd.iLevel = hItem->iLevel;
			}

			if (ti.state & TVIS_SELECTED)
			{
				fSelectedIcon = TRUE;

				if (pTree->m_bFocus || (pTree->m_hWnd->ci.style & TVS_SHOWSELALWAYS))
				{
					nmcd.nmcd.uItemState |= CDIS_SELECTED;
				}
			}

			if (fItemFocused)
			{
				nmcd.nmcd.uItemState |= CDIS_FOCUS;
			}
			if (hItem == pTree->m_hHotItem)
			{
				nmcd.nmcd.uItemState |= CDIS_HOT;
			}

			nmcd.nmcd.lItemlParam = ti.lParam;

			dwRet = CICustomDrawNotify(&pTree->m_hWnd->ci, CDDS_ITEMPREPAINT, &nmcd.nmcd);
			if (dwRet & CDRF_SKIPDEFAULT)
			{
				return;
			}

			fItemFocused = (nmcd.nmcd.uItemState & CDIS_FOCUS);
			if (nmcd.nmcd.uItemState & CDIS_SELECTED)
			{
				ti.state |= TVIS_SELECTED;
			}
			else
			{
				ti.state &= ~TVIS_SELECTED;
			}
		}

		// 如果NM_CUSTOMDRAW消息处理函数修改了文本颜色，重新设置文本颜色
		if (nmcd.clrText != clrTextTemp)
		{
			SetTextColor(hdc, nmcd.clrText);
		}

		COLORREF clrBk = CLR_DEFAULT;
		if (pTree->m_hWnd->ci.style & TVS_FULLROWSELECT && !(flags & TVDI_TRANSTEXT))
		{
			//L FillRectClr(hdc, &nmcd.nmcd.rc, GetBkColor(hdc));
			etoFlags |= ETO_OPAQUE;
			clrBk = CLR_NONE;
		}

		//
		// 绘制Image
		//
		if ((!(flags & TVDI_NOTREE) && !(dwRet & TVCDRF_NOIMAGES)) || (flags & TVDI_FORCEIMAGE))
		{
			int dx, dy;     // to clip the images within the borders.
			COLORREF clrImage = CLR_HILIGHT;
			COLORREF clrBkImage = clrBk;

			if (flags & TVDI_NOBK)
			{
				clrBkImage = CLR_NONE;
			}


			if (pTree->m_hImlState != NULL)
			{
				iState = TV_StateIndex(&ti);
				// this sucks.  in the treeview, 0 for the state image index
				// means draw nothing... the 0th item is unused.
				// the listview is 0 based and uses the 0th item.
				if (iState)
				{
					dx = min(pTree->m_cxState, pTree->cxMax - pTree->cxBorder - x);
					dy = min(pTree->m_cyState, pTree->cyItem - (2 * pTree->cyBorder));
					ImageList_DrawEx(pTree->m_hImlState, iState, hdc, x,
						y + max(pTree->cyItem - pTree->m_cyState, 0), dx, dy, clrBk, CLR_DEFAULT, ILD_NORMAL);
					x += pTree->m_cxState;
				}
			}

			if (pTree->m_hImageList != NULL)
			{
				UINT fStyle = 0;
				int i = (fSelectedIcon) ? ti.iSelectedImage : ti.iImage;

				if (ti.state & TVIS_CUT)
				{
					fStyle |= ILD_BLEND50;
					clrImage = ImageList_GetBkColor(pTree->m_hImageList);
				}

				dx = min(pTree->cxImage - TREE_INDENT, pTree->cxMax - pTree->cxBorder - x);
				dy = min(pTree->cyImage, pTree->cyItem - (2 * pTree->cyBorder));
				ImageList_DrawEx(pTree->m_hImageList, i, hdc,
					x, y + (max(pTree->cyItem - pTree->cyImage, 0) / 2), dx, dy,
					clrBkImage, clrImage,
					fStyle | (ti.state & TVIS_OVERLAYMASK));

			}
		}

		if (pTree->m_hImageList != NULL)
		{
			// even if not drawing image, draw text in right place
			x += pTree->cxImage;
		}

		//
		// 绘制文本及焦点Item的焦点框
		//
		LPTSTR lpstr = ti.pszText;
		int cch = lstrlen(lpstr);

		if (!hItem->iWidth || (hItem->lpstr == LPSTR_TEXTCALLBACK))
		{
			TV_ComputeItemWidth(pTree, hItem, hdc); //$BOLD
		}

		// Item坐标，不包含缩进、边框。 这个坐标，也是焦点Item焦点框的坐标。
		rc.left = x;
		rc.top = y + pTree->cyBorder;
		rc.right = min((x + hItem->iWidth), (pTree->cxMax - pTree->cxBorder));
		rc.bottom -= pTree->cyBorder;

		// Draw the text, unless it's the one we are editing
		if (pTree->m_hEditingItem != hItem
			|| !IsWindow(pTree->hwndEdit)
			|| !IsWindowVisible(pTree->hwndEdit))
		{
			if (!bFullRowSelected)
			{
				if (pTree->m_bFocus || fShowSelAlways)
				{
					if (fItemFocused)
					{
						TV_DrawItemBk(pTree, hItem, hdc, ITEM_FOCUS, &rc);
					}
					else if (fItemSelected)
					{
						TV_DrawItemBk(pTree, hItem, hdc, ITEM_SELECTED, &rc);
					}
				}

				if (fHot)
				{
					TV_DrawItemBk(pTree, hItem, hdc, ITEM_HIGHLIGHT, &rc);
				}
			}

			// 发现即使调用SetBkMode(hdc, TRANSPARENT)，ExtTextOut也不能使文字透明，故改成DrawText
			CIUIRect rcTextMargin;
			rcTextMargin.left = IUIGetGloabelVar()->g_cxLabelMargin;
			rcTextMargin.top = ((pTree->cyItem - pTree->cyText) / 2) + IUIGetGloabelVar()->g_cyBorder;
			_ASSERT(x == rc.left && y == rc.top);
			DrawFormatText(hdc, lpstr, cch, &rc, rcTextMargin, TAH_LEFT, TAV_TOP,
				DT_SINGLELINE, NULL, nmcd.clrText,
				IsUpdateLayeredWindow(GetHostWnd(pTree->m_hWnd)));
		}

		SetTextColor(hdc, rgbOldText);

		// Restore the original font.       //$BOLD
		SelectObject(hdc, hFont);           //$BOLD
	}

	//
	// 绘制缩进区域的图案：+-号、连接线等
	//
	if (!(flags & TVDI_NOTREE))
	{
		int dx, dy;

		if (pTree->m_hImageList != NULL)
		{
			x -= pTree->cxImage;
		}

		if (iState)
		{
			x -= pTree->m_cxState;
		}

		if (pTree->m_hWnd->ci.style & TVS_HASLINES)
		{
			// TREE::hdcBits保存了绘制TreeView缩进区域所有+-号、连接线的图案
			// 从左向右等间距（间距为TREE::cxIndent）排列。 i表示水平方向，第几个图案
			// 关于hdcBits的内容，请参考TreeIndent.png
			int i = 0;

			x -= cxIndent;
			if (iLevel-- || (pTree->m_hWnd->ci.style & TVS_LINESATROOT))
			{
				// HACK: 特殊处理第一个Root
				// We will draw a "last" sibling button upside down
				if (iLevel == -1 && hItem == hItem->hParent->hKids)
				{
					if (hItem->hNext != NULL)
					{
						i = 2;              // "L"
						if (ti.cChildren && (pTree->m_hWnd->ci.style & TVS_HASBUTTONS))
						{
							i += 2;         // "L+"
							if ((ti.state & (TVIS_EXPANDED | TVIS_EXPANDPARTIAL)) == TVIS_EXPANDED)
							{
								i += 2;    // "L-"
							}
						}

						dx = min((int)cxIndent, pTree->cxMax - pTree->cxBorder - x);
						dy = pTree->cyItem - (2 * pTree->cyBorder);

						// 垂直翻转绘制
						HDC hdcTemp = CreateCompatibleDC(hdc);
						HBITMAP hbmpTemp = CreateCompatibleBitmap(hdc, cxIndent, pTree->cyItem);
						HBITMAP hbmpOldTemp = (HBITMAP)::SelectObject(hdcTemp, hbmpTemp);

						StretchBlt(hdcTemp, 0, pTree->cyItem, cxIndent, -pTree->cyItem,
							pTree->hdcBits, i * cxIndent, 0, dx, dy, SRCCOPY);

						TransparentBlt(hdc, x, y, cxIndent, pTree->cyItem,
							hdcTemp, 0, 0, cxIndent, pTree->cyItem, RGB(255, 255, 255));

						::SelectObject(hdcTemp, hbmpOldTemp);
						hbmpOldTemp = NULL;
						::DeleteObject(hbmpTemp);
						hbmpTemp = NULL;
						::DeleteDC(hdcTemp);
						hdcTemp = NULL;

						i = -1;
					}
					else
					{
						// 第一个Root，并且没有兄弟节点
						if (pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
						{
							if (ti.cChildren != 0)
							{
								// has buttons, has lines, lines at root
								i = ((ti.state & (TVIS_EXPANDED | TVIS_EXPANDPARTIAL)) == TVIS_EXPANDED) ?
									9 : 8;  // "  [-]--" : "  [+]--"
							}
							else
							{
								// 没有孩子
								i = 7;      // "  ---"
							}
						}
						else
						{
							//				//   |
							i = 3;          //  [+]--
											//				//   |
						}
					}
				}
				else
				{
					i = (hItem->hNext) ? 1 : 2; // "|-" (rep) : "L"
					if (ti.cChildren && (pTree->m_hWnd->ci.style & TVS_HASBUTTONS))
					{
						i += 2;                 // "|-+" (rep) : "L+"
						if ((ti.state & (TVIS_EXPANDED | TVIS_EXPANDPARTIAL)) == TVIS_EXPANDED)
						{
							i += 2;    // "|--" (rep) : "L-"
						}
					}
				}

				// 如果Item占多个基础Item高度，绘制多出来部分的那些连接线（即先
				// 绘制Item下半部分，再绘制Item上半部分）
				if (pTree->m_bSmoothVScroll || TV_GetIntegral(pTree, hItem) > 1)
				{
					TV_DrawKinDescender(pTree, hdc, x, y, hItem, ti.state);
				}

				// 绘制连接线和+-号
				if (i != -1)
				{
					dx = min((int)cxIndent, pTree->cxMax - pTree->cxBorder - x);
					dy = pTree->cyItem - (2 * pTree->cyBorder);
					if ((dx > 0) && (dy > 0))
					{
						//
						// 绘制缩进区域连接线和+-号
						//
						TransparentBlt(hdc, x, y, dx, dy,
							pTree->hdcBits, i * cxIndent, 0, dx, dy, RGB(255, 255, 255));
					}
				}

				// 如果Item有多级父Item，绘制每级父Item垂直连接线
				while ((--iLevel >= 0)
					|| ((pTree->m_hWnd->ci.style & TVS_LINESATROOT) && iLevel >= -1))
				{
					hItem = hItem->hParent;
					x -= cxIndent;
					if (hItem->hNext != NULL)
					{
						dx = min((int)cxIndent, (pTree->cxMax - pTree->cxBorder - x));
						dy = min(pTree->cyItem, pTree->cyWnd - pTree->cyBorder - y);
						if ((dx > 0) && (dy > 0))
						{
							TransparentBlt(hdc, x, y, dx, dy,
								pTree->hdcBits, 0, 0, dx, dy, RGB(255, 255, 255));
						}
						TV_DrawDescender(pTree, hdc, x, y, hItemSave);
					}
				}
			}
		}
		else
		{
			// 无连接线
			if ((pTree->m_hWnd->ci.style & TVS_HASBUTTONS)
				&& (iLevel || pTree->m_hWnd->ci.style & TVS_LINESATROOT)
				&& ti.cChildren)
			{
				int i = (ti.state & TVIS_EXPANDED) ? cxIndent : 0;

				x -= cxIndent;
				dx = min((int)cxIndent, pTree->cxMax - pTree->cxBorder - x);
				dy = min(pTree->cyItem, pTree->cyWnd - pTree->cyBorder - y);
				if ((dx > 0) && (dy > 0))
				{
					TransparentBlt(hdc, x, y, dx, dy,
						pTree->hdcBits, i, 0, dx, dy, RGB(255, 255, 255));
				}
			}
		}
	}

	if (hItem->hBindWindowless == NULL)
	{
		if (dwRet & CDRF_NOTIFYPOSTPAINT)
		{
			nmcd.nmcd.dwItemSpec = (DWORD_PTR)hItemSave;
			CICustomDrawNotify(&pTree->m_hWnd->ci, CDDS_ITEMPOSTPAINT, &nmcd.nmcd);
		}
	}
}

#define INSERTMARKSIZE      6

BOOL TV_GetInsertMarkRect(PTREE pTree, LPRECT prc)
{
	_ASSERT(pTree);

	if (pTree->m_hInsertItem && TV_GetItemRect(pTree, pTree->m_hInsertItem, prc, TRUE))
	{
		if (pTree->m_bInsertAfter)
		{
			prc->top = prc->bottom;
		}
		else
		{
			prc->bottom = prc->top;
		}

		prc->top -= INSERTMARKSIZE / 2;
		prc->bottom += INSERTMARKSIZE / 2 + 1;
		prc->right = pTree->cxWnd - INSERTMARKSIZE;      // should always go all the way to right with pad.
		prc->left -= pTree->cxImage;

		return TRUE;
	}
	return FALSE;
}

__inline COLORREF TV_GetInsertMarkColor(PTREE pTree)
{
	if (pTree->clrim == CLR_DEFAULT)
	{
		return IUIGetGloabelVar()->g_clrWindowText;
	}
	else
	{
		return pTree->clrim;
	}
}

int TV_DrawBackground(PTREE pTree, HDC hdc, BOOL fErase, LPRECT lprc)
{
	int nStateIndex = 0;
	if (!IsWindowEnabled(pTree->m_hWnd))
	{
		nStateIndex = 1;
	}

	CIUIRect rcClient;
	GetClientRect(pTree->m_hWnd, rcClient);

	DrawBkColor(pTree->m_hWnd, hdc, lprc);

	if (IsCombineBkImage(pTree->m_hWnd))
	{
		if (pTree->m_hWnd->ci.m_eBkImageResizeMode == IRM_9GRID)
		{
			IUIPartNineGridBlt(hdc, rcClient,
				pTree->m_hWnd->ci.m_himgCombineBk, pTree->m_hWnd->ci.m_rcBkImage9GridResizeArg,
				COMBINEIMAGESIZE2, nStateIndex);
		}
	}
	else
	{
		HIUIIMAGE hImgBk = NULL;
		hImgBk = pTree->m_himgBk[nStateIndex];

		IUIDrawImage(hdc, rcClient,
			hImgBk, pTree->m_hWnd->ci.m_eBkImageResizeMode, pTree->m_hWnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;
}

void TV_DrawTree(PTREE pTree, HDC hdc, BOOL fErase, LPRECT lprc)
{
	if (!pTree->m_bRedraw)
	{
		return;
	}

	int x = -pTree->xPos;	// 水平滚动位置

	BeforePaint(pTree->m_hWnd, hdc);

	//
	// 绘制背景
	//
	TV_DrawBackground(pTree, hdc, fErase, lprc);

	//
	// 绘制边框
	//
	DrawBorder(pTree->m_hWnd, hdc);

	if (pTree->m_hWnd->ci.style & TVS_CHECKBOXES)
	{
		if (!pTree->m_hImlState)
		{
			TV_InitCheckBoxes(pTree);
		}
	}

	//TV_GetBackgroundBrush(pTree, hdc);

	RECT rc = *lprc; // Tree大小，不包含非客户区（滚动条）

					 //
					 // 绘制Item
					 //
	if (pTree->m_bSmoothVScroll)
	{
		// 垂直平滑滚动
		// draw item(text)
		// Item绑定的windowless控件的绘制，与普通窗口内的windowless控件绘制机制不同，
		// 对于普通窗口，只需要遍历所有windowless控件，把窗口内且可见的控件显示出来就行。
		// 对于item绑定的windowless控件，如果Item数据量庞大，采用普通窗口的形式，性能太差。
		// 所以，只需要从top item开始，绘制窗口内的item绑定的windowless即可。
		// 由于我们也仅布局tree内item绑定的windowless，所以，tree外Item绑定的windowless的坐标
		// 也都是不正确的，故不能使用标准方式，对windowless子控件进行hittest（Item由可见
		// 滚动到不可见前，Item绑定的windowless控件的坐标，是留在可见区域内的）。

		int nScrollPos = GetScrollPos(pTree->m_hWnd, SB_VERT);
		HTREEITEM hTI = pTree->hTop;
		if (hTI != NULL)
		{
			// IUITRACE(_T("========top item = %x, text = %s, ShowIndex = %d, ScrollPos = %d\r\n"),
			// 	hTI, hTI->lpstr, hTI->iShownIndex, nScrollPos);
		}
		int nDrawHeight = 0;// 已绘制完的item的总高度
		int i = 0;
		CIUIRect rcItem;
		CIUIRect rcClient;
		GetClientRect(pTree->m_hWnd, rcClient);
		rcItem.right = rcClient.right;

		SetBkMode(hdc, TRANSPARENT);

		while (hTI != NULL)
		{
			int nItemHeight = TV_GetIntegral(pTree, hTI);
			_ASSERT(nItemHeight != 0);

			// 第一次计算时，算上垂直偏移
			if (i == 0)
			{
				nDrawHeight -= (nScrollPos - hTI->iShownIndex);
			}

			rcItem.top = nDrawHeight;
			rcItem.bottom = rcItem.top + nItemHeight;

			// 超出控件，退出绘制
			if (nDrawHeight > rcClient.Height())
			{
				break;
			}

			nDrawHeight += nItemHeight;

			DRAWTREEITEMSTRUCT dis;
			dis.hDC = hdc;
			dis.hItem = hTI;
			dis.hwndItem = pTree->m_hWnd;
			dis.rcItem = rcItem;
			LRESULT lr = SendMessage(pTree->m_hWnd, TVM_DRAWITEM, 0, (LPARAM)&dis);

			if (lr == 0)
			{
				TV_DrawItem(pTree, hTI, hdc, x, rcItem.top, 0);
			}

			hTI = TV_GetNextVisItem(hTI);
			i++;
		}
	}
	else
	{
		NMCUSTOMDRAW nmcd;
		nmcd.hdc = hdc;
		/// not implemented yet
		if (IsFocused(pTree->m_hWnd))
		{
			nmcd.uItemState = CDIS_FOCUS;
		}
		else
		{
			nmcd.uItemState = 0;
		}
		nmcd.lItemlParam = 0;
		nmcd.rc = rc;
		pTree->m_hWnd->ci.dwCustom = CICustomDrawNotify(&pTree->m_hWnd->ci, CDDS_PREPAINT, &nmcd);
		if (!(pTree->m_hWnd->ci.dwCustom & CDRF_SKIPDEFAULT))
		{
			// 垂直滚动时，每次滚动一个Item.
			int iStart = 0;
			int iCnt = 0;	// 总Item数量减Top Item索引

#ifdef MAINWIN
			if (lprc->top <= 0)   /* fix microsoft BUG */
			{
				iStart = 0;
			}
			else
			{
				iStart = lprc->top / pTree->cyItem;
			}
#else
			iStart = lprc->top / pTree->cyItem;
#endif

			if (pTree->cItems != 0 && pTree->hTop != NULL)
			{
				_ASSERT(ITEM_VISIBLE(pTree->hTop));

				iCnt = pTree->cShowing - pTree->hTop->iShownIndex;
			}
			else
			{
				iCnt = 0;                   // Nothing to draw
			}

			if (iStart < iCnt)
			{
				HTREEITEM   hItem;
				HFONT       hOldFont;
				RECT        rcT;
				int y = 0;

				for (hItem = pTree->hTop; hItem;)
				{
					if (iStart > TV_GetIntegral(pTree, hItem))
					{
						iStart -= TV_GetIntegral(pTree, hItem);
						y += TV_GetIntegral(pTree, hItem) * pTree->cyItem;
						hItem = TV_GetNextVisItem(hItem);
					}
					else
					{
						break;
					}
				}

				hOldFont = pTree->hFont ? (HFONT)SelectObject(hdc, pTree->hFont) : NULL;

				// TVDI_* for all items
				UINT uFlags = (pTree->m_hWnd->ci.style & WS_DISABLED) ? TVDI_GRAYCTL : 0;
				if (fErase)
				{
					uFlags |= TVDI_ERASE;
				}

				//
				// 循环绘制所有可见Item
				//
				// loop from the first visible item until either all visible items are
				// drawn or there are no more items to draw
				for (; hItem && y < lprc->bottom; hItem = TV_GetNextVisItem(hItem))
				{
					TV_DrawItem(pTree, hItem, hdc, x, y, uFlags);
					y += pTree->cyItem * TV_GetIntegral(pTree, hItem);
				}

				//
				// handle drawing the InsertMark next to this item.
				//
				if (TV_GetInsertMarkRect(pTree, &rcT))
				{
					DrawInsertMark(hdc, &rcT, FALSE, TV_GetInsertMarkColor(pTree));
				}


				if (hOldFont)
				{
					SelectObject(hdc, hOldFont);
				}

				rc.top = y;
			}

			//if (fErase)
			//	// Opaque out everything we have not drawn explicitly
			//{
			//	FillRect(hdc, &rc, pTree->hbrBk);
			//}

			// notify parent afterwards if they want us to
			if (pTree->m_hWnd->ci.dwCustom & CDRF_NOTIFYPOSTPAINT)
			{
				CICustomDrawNotify(&pTree->m_hWnd->ci, CDDS_POSTPAINT, &nmcd);
			}
		}
	}

	AfterPaint(pTree->m_hWnd, hdc);
}


// ----------------------------------------------------------------------------
//
//  Set up for paint, call DrawTree, and clean up after paint.
//
// ----------------------------------------------------------------------------

void TV_Paint(PTREE pTree, HDC hdc)
{
	PAINTSTRUCT ps;

	if (hdc)
	{
		// hdc != 0 indicates a subclassed paint -- use the hdc passed in
		SetRect(&ps.rcPaint, 0, 0, pTree->cxWnd, pTree->cyWnd);
		TV_DrawTree(pTree, hdc, TRUE, &ps.rcPaint);
	}
	else
	{
		BeginPaint(GetHostWnd(pTree->m_hWnd), &ps);
		TV_DrawTree(pTree, ps.hdc, ps.fErase, &ps.rcPaint);
		EndPaint(GetHostWnd(pTree->m_hWnd), &ps);
	}
}

// ----------------------------------------------------------------------------
// Create an imagelist to be used for dragging.
//
// 1) create mask and image bitmap matching the select bounds size
// 2) draw the text to both bitmaps (in black for now)
// 3) create an imagelist with these bitmaps
// 4) make a dithered copy of the image onto the new imagelist
// ----------------------------------------------------------------------------
const DWORD dwExStyleRTLMirrorWnd = WS_EX_LAYOUTRTL;
HIMAGELIST TV_CreateDragImage(PTREE pTree, HTREEITEM hItem)
{
	HDC hdcMem = NULL;
	HBITMAP hbmImage = NULL;
	HBITMAP hbmMask = NULL;
	HBITMAP hbmOld;
	HIMAGELIST himl = NULL;
	BOOL bMirroredWnd = FALSE;//L (pTree->m_hWnd->ci.dwExStyle & RTL_MIRRORED_WINDOW);
	int dx, dy;
	int iSrc;

	XTVITEM ti;

	if (!pTree->m_hImageList)
	{
		return NULL;
	}

	if (hItem == NULL)
	{
		hItem = pTree->m_hDragItem;
	}

	if (hItem == NULL)
	{
		return NULL;
	}


	// BUGBUG??? we know it's already been drawn, so is iWidth valid???
	dx = hItem->iWidth + pTree->cxImage;
	dy = pTree->cyItem;

	if (!(hdcMem = CreateCompatibleDC(NULL)))
	{
		goto CDI_Exit;
	}
	if (!(hbmImage = CreateColorBitmap(dx, dy)))
	{
		goto CDI_Exit;
	}
	if (!(hbmMask = CreateMonoBitmap(dx, dy)))
	{
		goto CDI_Exit;
	}

	//
	// Mirror the memory DC so that the transition from
	// mirrored(memDC)->non-mirrored(imagelist DCs)->mirrored(screenDC)
	// is consistent. [samera]
	//
	if (bMirroredWnd)
	{
		//L SET_DC_RTL_MIRRORED(hdcMem);
	}

	// prepare for drawing the item
	if (pTree->hFont)
	{
		SelectObject(hdcMem, pTree->hFont);
	}
	SetBkMode(hdcMem, TRANSPARENT);

	/*
	** draw the text to both bitmaps
	*/
	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmImage);
	// fill image with black for transparency
	PatBlt(hdcMem, 0, 0, dx, dy, BLACKNESS);
	TV_DrawItem(pTree, hItem, hdcMem, 0, 0, TVDI_NOIMAGE | TVDI_NOTREE | TVDI_TRANSTEXT);

	//
	// If the header is RTL mirrored, then
	// mirror the Memory DC, so that when copying back
	// we don't get any image-flipping. [samera]
	//
	if (bMirroredWnd)
	{
		//L MirrorBitmapInDC(hdcMem, hbmImage);
	}

	SelectObject(hdcMem, hbmMask);
	// fill mask with white for transparency
	PatBlt(hdcMem, 0, 0, dx, dy, WHITENESS);
	TV_DrawItem(pTree, hItem, hdcMem, 0, 0, TVDI_NOIMAGE | TVDI_NOTREE | TVDI_TRANSTEXT);

	//
	// If the header is RTL mirrored, then
	// mirror the Memory DC, so that when copying back
	// we don't get any image-flipping. [samera]
	//
	if (bMirroredWnd)
	{
		//L MirrorBitmapInDC(hdcMem, hbmMask);
	}

	// unselect objects that we used
	SelectObject(hdcMem, hbmOld);
	SelectObject(hdcMem, IUIGetGloabelVar()->g_hfontSystem);

	/*
	** make an image list that for now only has the text
	*/
	//
	// BUGBUG: To fix a pri-1 M7 bug, we create a shared image list.
	//
	if (!(himl = ImageList_Create(dx, dy, ILC_MASK, 1, 0)))
	{
		goto CDI_Exit;
	}
	ImageList_SetBkColor(himl, CLR_NONE);
	ImageList_Add(himl, hbmImage, hbmMask);

	/*
	** make a dithered copy of the image part onto our bitmaps
	** (need both bitmap and mask to be dithered)
	*/
	TV_GetItem(pTree, hItem, TVIF_IMAGE, &ti);
	iSrc = ti.iImage;

	//L ImageList_CopyDitherImage(himl, 0, 0, (pTree->cyItem - pTree->cyImage) / 2,
	//	pTree->m_hImageList, iSrc, 0L | (hItem->state & TVIS_OVERLAYMASK));

CDI_Exit:
	if (hdcMem)
	{
		DeleteObject(hdcMem);
	}
	if (hbmImage)
	{
		DeleteObject(hbmImage);
	}
	if (hbmMask)
	{
		DeleteObject(hbmMask);
	}

	return himl;
}

#define COLORKEY RGB(0xF4, 0x0, 0x0)

LRESULT TV_GenerateDragImage(PTREE pTree, SHDRAGIMAGE *pshdi)
{
	LRESULT lRet = 0;
	HBITMAP hbmpOld = NULL;
	HTREEITEM hItem = pTree->m_hDragItem;
	RECT rc;
	HDC  hdcDragImage;

	if (hItem == NULL)
	{
		return FALSE;
	}

	hdcDragImage = CreateCompatibleDC(NULL);

	if (!hdcDragImage)
	{
		return 0;
	}

	// After this rc contains the bounds of all the items in Client Coordinates.
	//
	// Mirror the the DC, if the listview is mirrored.
	//
	//L if (pTree->m_hWnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)
	//{
	//	SET_DC_RTL_MIRRORED(hdcDragImage);
	//}

	TV_GetItemRect(pTree, hItem, &rc, TRUE);

	// Subtract off the image...
	rc.left -= pTree->cxImage;

	pshdi->sizeDragImage.cx = RECTWIDTH(rc);
	pshdi->sizeDragImage.cy = RECTHEIGHT(rc);
	pshdi->hbmpDragImage = CreateBitmap(pshdi->sizeDragImage.cx, pshdi->sizeDragImage.cy,
		GetDeviceCaps(hdcDragImage, PLANES), GetDeviceCaps(hdcDragImage, BITSPIXEL),
		NULL);

	if (pshdi->hbmpDragImage)
	{
		COLORREF clrBkSave;
		RECT  rcImage = { 0, 0, pshdi->sizeDragImage.cx, pshdi->sizeDragImage.cy };

		hbmpOld = (HBITMAP)SelectObject(hdcDragImage, pshdi->hbmpDragImage);

		pshdi->crColorKey = COLORKEY;
		FillRectClr(hdcDragImage, &rcImage, pshdi->crColorKey);

		// Calculate the offset... The cursor should be in the bitmap rect.

		if (pTree->m_hWnd->ci.dwExStyle & RTL_MIRRORED_WINDOW)
		{
			pshdi->ptOffset.x = rc.right - pTree->ptCapture.x;
		}
		else
		{
			pshdi->ptOffset.x = pTree->ptCapture.x - rc.left;
		}

		pshdi->ptOffset.y = pTree->ptCapture.y - rc.top;

		clrBkSave = pTree->clrBk;

		pTree->clrBk = COLORKEY;

		TV_DrawItem(pTree, hItem, hdcDragImage, 0, 0,
			TVDI_NOTREE | TVDI_TRANSTEXT | TVDI_FORCEIMAGE | TVDI_NOBK);

		pTree->clrBk = clrBkSave;

		SelectObject(hdcDragImage, hbmpOld);
		DeleteDC(hdcDragImage);

		// We're passing back the created HBMP.
		return 1;
	}


	return lRet;
}

//////////////////////////////////////////////////////////////////////////
// TreeView Scroll

BOOL TV_EnsureVisible(PTREE pTree, TREEITEM *hItem);

// ----------------------------------------------------------------------------
//
//  更新指定Item下方(按List顺序)所有可见Item的iShownIndex
//
//  返回指定Item及下方所有可见Item的总滚动高度
//
// ----------------------------------------------------------------------------
int TV_UpdateShownIndexes(PTREE pTree, HTREEITEM hWalk)
{
	WORD iShownIndex;

	if (hWalk == pTree->m_hRootItem)
	{
		hWalk = pTree->m_hRootItem->hKids;
		if (hWalk)
		{
			hWalk->iShownIndex = 0;
		}
		else
		{
			return 0;
		}
	}

	iShownIndex = hWalk->iShownIndex + TV_GetIntegral(pTree, hWalk);

	if (iShownIndex <= 0)
	{
		// BUGBUG: We should #define the special TVITEM_HIDDEN value and check
		// for it explicitly
		// This can happen if TV_SortCB passes in a hidden item
		return (-1);
	}

	// 依次更新指定Item之下所有可见Item（不一定与指定Item同级）的iShownIndex
	while ((hWalk = TV_GetNextVisItem(hWalk)) != NULL)
	{
		hWalk->iShownIndex = iShownIndex;

		iShownIndex += (WORD)TV_GetIntegral(pTree, hWalk);
	}

	IUITRACE(_T("tv: updated show indexes (now %d items)\r\n"), (int)iShownIndex);
	return (int)iShownIndex;
}

//
// TODO: 本函数无人调用，需删除？
// hItem: 需要被统计后裔的展开节点
//
// 返回值: 如果TREE::fSmoothVScroll为TRUE，返回指定Item下面总的展开的后裔的高度
// TREEITEM::nBranchHeight缓存了这个值
// 如果TREE::fSmoothVScroll为FALSE，返回指定Item下面总的展开的后裔的数量
// (这个数量是考虑iIntegral的，一个Item占iIntegral个名额)。
//
UINT TV_CountVisibleDescendants(PTREE pTree, HTREEITEM hItem)
{
	UINT cnt = 0;

	for (cnt = 0, hItem = hItem->hKids; hItem != NULL; hItem = hItem->hNext)
	{
		cnt += TV_GetIntegral(pTree, hItem);
		if (hItem->hKids && (hItem->state & TVIS_EXPANDED))
		{
			cnt += TV_CountVisibleDescendants(pTree, hItem);
		}
	}

	return cnt;
}

//  scrolls nItems in the direction of fDown starting from iTopShownIndex
void TV_ScrollItems(PTREE pTree, int nItems, int iTopShownIndex, BOOL fDown)
{
	RECT rc;
	rc.left = 0;
	rc.top = (iTopShownIndex + 1) * pTree->cyItem;
	rc.right = pTree->cxWnd;
	rc.bottom = pTree->cyWnd;

	TV_UpdateToolTip(pTree);
}

//
//  If fRedrawParent is FALSE, then the return value is garbage.
//  If fRedrawParent is TRUE, then returns the number of children scrolled.
//
//  任何时候都不要更新iShownIndex
//
UINT TV_ScrollBelow(PTREE pTree, HTREEITEM hItem, BOOL fRedrawParent, BOOL fDown)
{
	// 如果Item不可见，直接返回
	if (!ITEM_VISIBLE(hItem))
	{
		return 0;
	}

	// 默认返回值
	UINT cnt = TV_GetIntegral(pTree, hItem);

	return (cnt);
}

// The FakeCustomDraw functions are used when you want the customdraw client
// to set up a HDC so you can do stuff like GetTextExtent.
//
//  Usage:
//
//      TVFAKEDRAW tvfd;
//      TreeView_BeginFakeCustomDraw(pTree, &tvfd);
//      for each item you care about {
//          TreeView_BeginFakeItemDraw(&tvfd, hitem);
//          <party on the HDC in tvfd.nmcd.nmcd.hdc>
//          TreeView_EndFakeItemDraw(&tvfd);
//      }
//      TreeView_EndFakeCustomDraw(&tvfd);
//

void TreeView_BeginFakeCustomDraw(PTREE pTree, PTVFAKEDRAW ptvfd)
{
	ptvfd->nmcd.nmcd.hdc = GetDC(GetHostWnd(pTree->m_hWnd));
	ptvfd->nmcd.nmcd.uItemState = 0;
	ptvfd->nmcd.nmcd.dwItemSpec = 0;
	ptvfd->nmcd.nmcd.lItemlParam = 0;
	ptvfd->hfontPrev = (HFONT)GetCurrentObject(ptvfd->nmcd.nmcd.hdc, OBJ_FONT);

	//
	//  Since we aren't actually painting anything, we pass an empty
	//  paint rectangle.  Gosh, I hope no app faults when it sees an
	//  empty paint rectangle.
	//
	SetRectEmpty(&ptvfd->nmcd.nmcd.rc);

	ptvfd->pTree = pTree;
	ptvfd->dwCustomPrev = pTree->m_hWnd->ci.dwCustom;

	pTree->m_hWnd->ci.dwCustom = CIFakeCustomDrawNotify(&pTree->m_hWnd->ci, CDDS_PREPAINT, &ptvfd->nmcd.nmcd);
}

DWORD TreeView_BeginFakeItemDraw(PTVFAKEDRAW ptvfd, HTREEITEM hitem)
{
	PTREE pTree = ptvfd->pTree;

	// Note that if the client says CDRF_SKIPDEFAULT (i.e., is owner-draw)
	// we measure the item anyway, because that's what IE4 did.

	ptvfd->nmcd.nmcd.dwItemSpec = (DWORD_PTR)hitem;
	ptvfd->nmcd.nmcd.lItemlParam = hitem->lParam;

	if (hitem->state & TVIS_BOLD)
	{
		SelectFont(ptvfd->nmcd.nmcd.hdc, pTree->hFontBold);
	}
	else
	{
		SelectFont(ptvfd->nmcd.nmcd.hdc, pTree->hFont);
	}

	if (!(pTree->m_hWnd->ci.dwCustom & CDRF_SKIPDEFAULT))
	{
		// Font should not depend on colors or flags since those change
		// dynamically but we cache the width info forever.  So we don't
		// need to set up uItemState.
		ptvfd->nmcd.clrText = pTree->clrText;
		ptvfd->nmcd.clrTextBk = pTree->clrBk;
		ptvfd->nmcd.iLevel = hitem->iLevel;
		ptvfd->dwCustomItem = CIFakeCustomDrawNotify(&pTree->m_hWnd->ci, CDDS_ITEMPREPAINT, &ptvfd->nmcd.nmcd);
	}
	else
	{
		ptvfd->dwCustomItem = CDRF_DODEFAULT;
	}

	return ptvfd->dwCustomItem;
}

void TreeView_EndFakeItemDraw(PTVFAKEDRAW ptvfd)
{
	PTREE pTree = ptvfd->pTree;

	if (!(ptvfd->dwCustomItem & CDRF_SKIPDEFAULT) &&
		(ptvfd->dwCustomItem & CDRF_NOTIFYPOSTPAINT))
	{
		CIFakeCustomDrawNotify(&pTree->m_hWnd->ci, CDDS_ITEMPOSTPAINT, &ptvfd->nmcd.nmcd);
	}
}

void TreeView_EndFakeCustomDraw(PTVFAKEDRAW ptvfd)
{
	PTREE pTree = ptvfd->pTree;

	// notify parent afterwards if they want us to
	if (!(pTree->m_hWnd->ci.dwCustom & CDRF_SKIPDEFAULT) &&
		pTree->m_hWnd->ci.dwCustom & CDRF_NOTIFYPOSTPAINT)
	{
		CIFakeCustomDrawNotify(&pTree->m_hWnd->ci, CDDS_POSTPAINT, &ptvfd->nmcd.nmcd);
	}

	// Restore previous state
	pTree->m_hWnd->ci.dwCustom = ptvfd->dwCustomPrev;
	SelectObject(ptvfd->nmcd.nmcd.hdc, ptvfd->hfontPrev);
	ReleaseDC(GetHostWnd(pTree->m_hWnd), ptvfd->nmcd.nmcd.hdc);
}


// ----------------------------------------------------------------------------
//
//  Returns the width of the widest shown item in the tree
//
// ----------------------------------------------------------------------------

UINT TV_RecomputeMaxWidth(PTREE pTree)
{
	if (!(pTree->m_hWnd->ci.style & TVS_NOSCROLL))
	{
		HTREEITEM hItem;
		WORD wMax = 0;

		// REVIEW: this might not be the most efficient traversal of the tree

		for (hItem = pTree->m_hRootItem->hKids; hItem; hItem = TV_GetNextVisItem(hItem))
		{
			if (wMax < FULL_WIDTH(pTree, hItem))
			{
				wMax = FULL_WIDTH(pTree, hItem);
			}
		}

		return ((UINT)wMax);
	}
	else
	{
		return pTree->cxWnd;
	}
}

// ----------------------------------------------------------------------------
//
//  返回指定Item的文本的水平宽度（包含边距）
//
// ----------------------------------------------------------------------------
WORD TV_GetItemTextWidth(HDC hdc, PTREE pTree, HTREEITEM hItem)
{
	XTVITEM sItem;
	TCHAR szTemp[MAX_PATH];
	SIZE size = { 0, 0 };

	sItem.pszText = szTemp;
	sItem.cchTextMax = ARRAYSIZE(szTemp);

	TV_GetItem(pTree, hItem, TVIF_TEXT, &sItem);

	GetTextExtentPoint(hdc, sItem.pszText, lstrlen(sItem.pszText), &size);
	return (WORD)(size.cx + (IUIGetGloabelVar()->g_cxLabelMargin * 2));
}

// ----------------------------------------------------------------------------
//
//  设置Item文本及整个Item宽度（包含缩进、图标和文本)。
//  设置Tree控件的滚动宽度
//
//  如果指定HDC，我们假定这个HDC已设置了正确的属性（尤其是字体）。
//  如果未指定HDC，我们将创建一个，测量完字符尺寸后销毁它。
//  如果你准备测量多个Item，我们建议调用者自己准备一个HDC，并且重复使用它，
//  因为创建、初始化并销毁HDC比较慢。
//
// ----------------------------------------------------------------------------
void TV_ComputeItemWidth(PTREE pTree, HTREEITEM hItem, HDC hdc)
{
	TVFAKEDRAW tvfd;                    // in case client uses customdraw
	int iOldWidth = hItem->iWidth;

	if (hdc == NULL)
	{
		TreeView_BeginFakeCustomDraw(pTree, &tvfd);
		TreeView_BeginFakeItemDraw(&tvfd, hItem);
	}
	else
	{
		tvfd.nmcd.nmcd.hdc = hdc;
	}

	hItem->iWidth = TV_GetItemTextWidth(tvfd.nmcd.nmcd.hdc, pTree, hItem);

	if (!(pTree->m_hWnd->ci.style & TVS_NOSCROLL) && iOldWidth != hItem->iWidth)
	{
		if (pTree->cxMax < FULL_WIDTH(pTree, hItem))
		{
			// TODO
			PostMessage(GetHostWnd(pTree->m_hWnd), TVMP_CALCSCROLLBARS, 0, 0);
			pTree->cxMax = FULL_WIDTH(pTree, hItem);
		}
	}

	if (hdc == NULL)
	{
		TreeView_EndFakeItemDraw(&tvfd);
		TreeView_EndFakeCustomDraw(&tvfd);
	}
}

// ----------------------------------------------------------------------------
//
//  如果hItem是被展开的（可见的，但不一定在视图范围内），返回TRUE，否则返回FALSE
//
// ----------------------------------------------------------------------------

BOOL TV_IsShowing(HTREEITEM hItem)
{
	for (hItem = hItem->hParent; hItem != NULL; hItem = hItem->hParent)
	{
		// 父路径中，只要有一级是折叠的，就返回FALSE
		if (!(hItem->state & TVIS_EXPANDED))
		{
			return FALSE;
		}
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  如果新增的Item是可见的，更新可见（展开）的Item总数、最大Item宽度并重新计算
//  滚动条
//
//  设置cxMax和cShowing变量
//
// ----------------------------------------------------------------------------
BOOL TV_ScrollBarsAfterAdd(PTREE pTree, HTREEITEM hItem)
{
	HTREEITEM hPrev = NULL;

	if (!TV_IsShowing(hItem))
	{
		// Item被折叠（不可见），设置可见索引为NOTVISIBLE并返回
		hItem->iShownIndex = (WORD)(-1);
		return FALSE;
	}

	// 得到hItem上方最靠近hItem的可见Item
	hPrev = TV_GetPrevVisItem(hItem);

	// 增加新加入Item下面每一个item的显示索引（按list顺序）
	hItem->iShownIndex = (hPrev != NULL) ? hPrev->iShownIndex + TV_GetIntegral(pTree, hPrev) : 0;

	TV_UpdateShownIndexes(pTree, hItem);

	pTree->cShowing += TV_GetIntegral(pTree, hItem);

	TV_ComputeItemWidth(pTree, hItem, NULL);

	TV_CalcScrollBars(pTree);

	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  如果被删除的Item是展开的, 更新展开的Item数量、TreeView的最大宽度，并重
//  新计算滚动信息
//
//  sets cxMax, cShowing
//
// ----------------------------------------------------------------------------
BOOL TV_ScrollBarsAfterRemove(PTREE pTree, HTREEITEM hItem)
{
	if (!ITEM_VISIBLE(hItem))
	{
		return FALSE;
	}

	//
	// 删除Item后，减小它之后每一个展开的Item的索引（位置）
	//
	hItem->iShownIndex = WORD(-1);

	HTREEITEM hWalk = TV_GetNextVisItem(hItem);
	if (hWalk != NULL)
	{
		hWalk->iShownIndex -= TV_GetIntegral(pTree, hItem);
		TV_UpdateShownIndexes(pTree, hWalk);

		// If we delete the top item, the tree scrolls to the end, so ...
		if (pTree->hTop == hItem)
		{
			TV_SetTopItem(pTree, hWalk->iShownIndex);
			_ASSERT(pTree->hTop != hItem);
		}
	}

	pTree->cShowing -= TV_GetIntegral(pTree, hItem);

	if (pTree->m_bRedraw)
	{
		// 重新计算最宽的Item
		if (hItem->iWidth == 0)
		{
			TV_ComputeItemWidth(pTree, hItem, NULL);
		}

		if (!(pTree->m_hWnd->ci.style & TVS_NOSCROLL))
		{
			if (pTree->cxMax == FULL_WIDTH(pTree, hItem))
			{
				pTree->cxMax = (WORD)TV_RecomputeMaxWidth(pTree);
			}
		}

		TV_CalcScrollBars(pTree);
	}

	return TRUE;
}

// 递归增加hItem及其所有子孙的缩进
BOOL TV_OffsetItemLevelRecurse(HTREEITEM hItem, int nAddLevel)
{
	if (NULL == hItem)
	{
		return FALSE;
	}
	
	// 设置自己
	hItem->iLevel += nAddLevel;

	// 设置孩子
	HTREEITEM hKid = hItem->hKids;
	for (; NULL != hKid; hKid = hKid->hNext)
	{
		TV_OffsetItemLevelRecurse(hKid, nAddLevel);
	}

	return TRUE;
}

// ----------------------------------------------------------------------------
//
//  重新计算展开的Item数量、TreeView的最大宽度、滚动信息
//
//  sets cxMax, cShowing
//
// ----------------------------------------------------------------------------
BOOL TV_ScrollBarsAfterMove(PTREE pTree)
{
	HTREEITEM hPrev = NULL;

	HTREEITEM hItem = pTree->m_hRootItem->hKids;
	if (!TV_IsShowing(hItem))
	{
		// Item被折叠（不可见），设置可见索引为NOTVISIBLE并返回
		hItem->iShownIndex = (WORD)(-1);
		return FALSE;
	}

	// 得到hItem上方最靠近hItem的可见Item
	hPrev = TV_GetPrevVisItem(hItem);

	// 增加新加入Item下面每一个item的显示索引（按list顺序）
	hItem->iShownIndex = (hPrev != NULL) ? hPrev->iShownIndex + TV_GetIntegral(pTree, hPrev) : 0;

	pTree->cShowing = TV_UpdateShownIndexes(pTree, hItem);

	TV_ComputeItemWidth(pTree, hItem, NULL);

	TV_CalcScrollBars(pTree);

	return TRUE;
}

// ----------------------------------------------------------------------------
//
//  如果被展开或折叠的那些Items是可见的，更新展开数量、最大Item宽度，并且重新计算滚动信息
//
// ----------------------------------------------------------------------------
#define SBAEC_COLLAPSE  0
#define SBAEC_EXPAND    1

BOOL TV_ScrollBarsAfterExpandCollapse(PTREE pTree, HTREEITEM hParent, UINT flags)
{
	WORD cxMax = 0;
	HTREEITEM hWalk;
	TVFAKEDRAW tvfd;

	if (!ITEM_VISIBLE(hParent))
	{
		return FALSE;
	}

	//
	// We're going to be measuring a lot of items, so let's set up
	// our DC ahead of time.
	//
	TreeView_BeginFakeCustomDraw(pTree, &tvfd);

	for (hWalk = hParent->hKids;
		hWalk != NULL && (hWalk->iLevel > hParent->iLevel);
		hWalk = TV_GetNextVisItem(hWalk))
	{
		if (flags == SBAEC_COLLAPSE)
		{
			hWalk->iShownIndex = WORD(-1);
		}

		// 如果Item的宽度还未计算，先计算宽度
		if (hWalk->iWidth == 0)
		{
			TreeView_BeginFakeItemDraw(&tvfd, hWalk);
			TV_ComputeItemWidth(pTree, hWalk, tvfd.nmcd.nmcd.hdc);
			TreeView_EndFakeItemDraw(&tvfd);
		}
		if (cxMax < FULL_WIDTH(pTree, hWalk))
		{
			cxMax = FULL_WIDTH(pTree, hWalk);
		}
	}

	TreeView_EndFakeCustomDraw(&tvfd);

	// 更新指定Item下方(按List顺序)所有可见Item的iShownIndex
	// 返回指定Item及下方所有可见Item的总滚动高度
	pTree->cShowing = TV_UpdateShownIndexes(pTree, hParent);

	// 因为最宽的Item可能被展开或折叠，所以更新pTree->cxMax
	if (!(pTree->m_hWnd->ci.style & TVS_NOSCROLL))
	{
		if (flags == SBAEC_COLLAPSE)
		{
			// If one of our newly-hidden items was responsible for
			// the width being what it is, recompute the max width
			// since we hid those items.
			if (cxMax == pTree->cxMax)
			{
				pTree->cxMax = (WORD)TV_RecomputeMaxWidth(pTree);
			}
		}
		else
		{
			// If one of our newly-shown items was responsible is wider
			// then the previous max, then we have set a new max.
			if (cxMax > pTree->cxMax)
			{
				pTree->cxMax = cxMax;
			}
		}
	}

	TV_CalcScrollBars(pTree);
	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  If the expanded items are showing, update the shown (expanded) count,
//  the max item width -- then recompute the scroll bars.
//
//  sets cxMax, cShowing
//
// ----------------------------------------------------------------------------

BOOL TV_ScrollBarsAfterExpand(PTREE pTree, HTREEITEM hParent)
{
	return TV_ScrollBarsAfterExpandCollapse(pTree, hParent, SBAEC_EXPAND);
}


// ----------------------------------------------------------------------------
//
//  If the collapsed items were showing, update the shown (expanded) count,
//  the max item width -- then recompute the scroll bars.
//
//  sets cxMax, cShowing
//
// ----------------------------------------------------------------------------

BOOL TV_ScrollBarsAfterCollapse(PTREE pTree, HTREEITEM hParent)
{
	return TV_ScrollBarsAfterExpandCollapse(pTree, hParent, SBAEC_COLLAPSE);
}

// ----------------------------------------------------------------------------
//
//  If the added item changed height, then scroll thing around,
//  update the shown (expanded) count, recompute the scroll bars.
//
//  sets cShowing
//
// ----------------------------------------------------------------------------

void TV_ScrollBarsAfterResize(PTREE pTree, HTREEITEM hItem, int iIntegralPrev, UINT uRDWFlags)
{
	int iMaxIntegral = max(TV_GetIntegral(pTree, hItem), iIntegralPrev);

	_ASSERT(TV_GetIntegral(pTree, hItem) != iIntegralPrev);

	if (pTree->m_bRedraw)
	{
		int iTop = hItem->iShownIndex - pTree->hTop->iShownIndex +
			iMaxIntegral - 1;
		if (iTop >= 0)
		{
			int iGrowth = TV_GetIntegral(pTree, hItem) - iIntegralPrev;
			TV_ScrollItems(pTree, abs(iGrowth), iTop, iGrowth > 0);
		}
	}

	// update every shown index after resized item
	pTree->cShowing = TV_UpdateShownIndexes(pTree, hItem);
	TV_CalcScrollBars(pTree);

	// Invalidate based on the worst-case height so we handle
	// both the grow and shrink cases.
	if (pTree->m_bRedraw)
	{
		RECT rc;
		if (TV_GetItemRect(pTree, hItem, &rc, FALSE))
		{
			rc.bottom = rc.top + pTree->cyItem * iMaxIntegral;
			RedrawWindow(GetHostWnd(pTree->m_hWnd), &rc, NULL, uRDWFlags);
		}
	}
}



// ----------------------------------------------------------------------------
//
//  Returns the item just below the given item in the tree.
//
// ----------------------------------------------------------------------------

TREEITEM *TV_GetNext(TREEITEM *hItem)
{
	DBG_ValidateTreeItem(hItem, FALSE);

	if (hItem->hKids)
	{
		return hItem->hKids;
	}

checkNext:
	if (hItem->hNext)
	{
		return hItem->hNext;
	}

	hItem = hItem->hParent;
	if (hItem)
	{
		goto checkNext;
	}

	return NULL;
}


// ----------------------------------------------------------------------------
//
//  Go through all the items in the tree, recomputing each item's text extent
//  and full width (indent, image, and text).
//
// ----------------------------------------------------------------------------

void TV_RecomputeItemWidths(PTREE pTree)
{
	HTREEITEM hItem;
	TVFAKEDRAW tvfd;

	TreeView_BeginFakeCustomDraw(pTree, &tvfd);

	hItem = pTree->m_hRootItem->hKids;
	while (hItem)
	{
		TreeView_BeginFakeItemDraw(&tvfd, hItem);
		TV_ComputeItemWidth(pTree, hItem, tvfd.nmcd.nmcd.hdc);
		TreeView_EndFakeItemDraw(&tvfd);
		hItem = TV_GetNext(hItem);
	}
	TreeView_EndFakeCustomDraw(&tvfd);
}


// ----------------------------------------------------------------------------
//
//  If a single item's width changed, alter the max width if needed.
//  If all widths changed, recompute widths and max width.
//  Then recompute the scroll bars.
//
//  sets cxMax
//
// ----------------------------------------------------------------------------

BOOL TV_ScrollBarsAfterSetWidth(PTREE pTree, HTREEITEM hItem)
{
	if (hItem)
	{
		UINT iOldWidth = FULL_WIDTH(pTree, hItem);
		TV_ComputeItemWidth(pTree, hItem, NULL);

		if (!(pTree->m_hWnd->ci.style & TVS_NOSCROLL))
		{
			if (pTree->cxMax == iOldWidth)
			{
				pTree->cxMax = (WORD)TV_RecomputeMaxWidth(pTree);
			}
			else
			{
				return (FALSE);
			}
		}
	}
	else
	{
		TV_RecomputeItemWidths(pTree);
		pTree->cxMax = (WORD)TV_RecomputeMaxWidth(pTree);
	}

	TV_CalcScrollBars(pTree);
	return (TRUE);
}

// ----------------------------------------------------------------------------
//
//  垂直滚动TreeView, 使得指定Item在客户区完全显示
//
// ----------------------------------------------------------------------------
BOOL TV_ScrollVertIntoView(PTREE pTree, HTREEITEM hItem)
{
	// This function has crashed in stress before, so we need to assert the incoming parameters.
	_ASSERT(hItem != NULL);
	_ASSERT(pTree != NULL && pTree->hTop != NULL);

	// Do nothing if the parameters are invalid
	if (hItem == NULL || pTree == NULL || pTree->hTop == NULL)
	{
		return FALSE;
	}

	// Do nothing if this item is not visible
	if (!ITEM_VISIBLE(hItem))
	{
		return FALSE;
	}

	int nVScrollPos = GetScrollPos(pTree->m_hWnd, SB_VERT);

	// 如果指定的Item在TreeView上方部分显示或位于上方TreeView之外
	if (pTree->m_bSmoothVScroll)
	{
		if (hItem->iShownIndex < nVScrollPos)
		{
			BOOL bRet = (TV_SetTopItem(pTree, hItem->iShownIndex));

			InvalidateRect(pTree->m_hWnd, NULL);

			return bRet;
		}
	}
	else
	{
		if (hItem->iShownIndex < pTree->hTop->iShownIndex)
		{
			return (TV_SetTopItem(pTree, hItem->iShownIndex));
		}
	}

	// 如果指定的Item在TreeView下方部分显示或位于下方TreeView之外
	if (pTree->m_bSmoothVScroll)
	{

		// 指定Item的底部，大于（垂直滚动位置 + TreeView客户区高度）
		if (hItem->iShownIndex + TV_GetIntegral(pTree, hItem) > nVScrollPos + pTree->cyWnd)
		{
			BOOL bRet = (TV_SetTopItem(pTree,
				hItem->iShownIndex + TV_GetIntegral(pTree, hItem) - pTree->cFullVisible));

			InvalidateRect(pTree->m_hWnd, NULL);

			return bRet;
		}
	}
	else
	{
		if (hItem->iShownIndex >= (pTree->hTop->iShownIndex + pTree->cFullVisible))
		{
			BOOL bRet = (TV_SetTopItem(pTree, hItem->iShownIndex + 1 - pTree->cFullVisible));

			InvalidateRect(pTree->m_hWnd, NULL);

			return bRet;
		}
	}

	return FALSE;
}


// ----------------------------------------------------------------------------
//
//  Scroll window vertically and horizontally as needed to make given item
//  fully visible vertically and horizontally
//
// ----------------------------------------------------------------------------

BOOL TV_ScrollIntoView(PTREE pTree, HTREEITEM hItem)
{
	UINT iWidth, iOffset;
	BOOL fChange;

	fChange = TV_ScrollVertIntoView(pTree, hItem);

	// ensure that item's text is fully visible horizontally
	iWidth = pTree->cxImage + pTree->m_cxState + hItem->iWidth;
	if (iWidth > (UINT)pTree->cxWnd)
	{
		iWidth = pTree->cxWnd;    //hItem->iWidth;
	}

	iOffset = ITEM_OFFSET(pTree, hItem);

	if ((int)(iOffset) < pTree->xPos)
	{
		fChange |= TV_SetLeft(pTree, iOffset);
	}
	else if ((iOffset + iWidth) > (UINT)(pTree->xPos + pTree->cxWnd))
	{
		fChange |= TV_SetLeft(pTree, iOffset + iWidth - pTree->cxWnd);
	}

	return fChange;
}


// ----------------------------------------------------------------------------
//
//  Sets position of horizontal scroll bar and scrolls window to match that
//  position
//
//  sets xPos
//
// ----------------------------------------------------------------------------

BOOL TV_SetLeft(PTREE pTree, int x)
{
	if (!pTree->m_bHorz || pTree->m_hWnd->ci.style & (TVS_NOSCROLL | TVS_NOHSCROLL))
	{
		return (FALSE);
	}

	if (x > (int)(pTree->cxMax - pTree->cxWnd))
	{
		x = (pTree->cxMax - pTree->cxWnd);
	}
	if (x < 0)
	{
		x = 0;
	}

	if (x == pTree->xPos)
	{
		return (FALSE);
	}

	pTree->xPos = (SHORT)x;

	SetScrollPos(pTree->m_hWnd, SB_HORZ, x, TRUE);
	TV_UpdateToolTip(pTree);

	return (TRUE);
}


// ----------------------------------------------------------------------------
//
//  根据指定的显示索引（滚动位置），返回Tree Item
//
// ----------------------------------------------------------------------------
HTREEITEM TV_GetShownIndexItem(PTREE pTree, HTREEITEM hItem, UINT wShownIndex)
{
	if (hItem == NULL)
	{
		return NULL;
	}

	_ASSERT((int)wShownIndex >= 0);

	HTREEITEM hWalk = NULL;
	for (hWalk = hItem;
		hWalk != NULL && (hWalk->iShownIndex <= wShownIndex);
		hWalk = hWalk->hNext)
	{
		hItem = hWalk;

		// 给定的wShownIndex仍然位于hWalk内
		if (hWalk->iShownIndex + (UINT)TV_GetIntegral(pTree, hWalk) > wShownIndex)
		{
			return hWalk;
		}
	}

	// 递归遍历孩子
	return TV_GetShownIndexItem(pTree, hItem->hKids, wShownIndex);
}


// ----------------------------------------------------------------------------
//
//  把TreeView垂直滚动到wNewTop位置，并设置TREE::hTop
//
// ----------------------------------------------------------------------------
BOOL TV_SmoothSetTopItem(PTREE pTree, UINT wNewTop, UINT uSmooth)
{
	HTREEITEM hItem = pTree->m_hRootItem->hKids;

	if (hItem == NULL)
	{
		return FALSE;
	}

	// 修正不合理参数wNewTop
	if ((pTree->m_hWnd->ci.style & TVS_NOSCROLL)
		|| (wNewTop == (UINT)(-1))
		|| (pTree->cShowing <= pTree->cFullVisible))
	{
		// we've wrapped around (treat as a negative index) -- use min pos
		// or there aren't enough items to scroll
		wNewTop = 0;
	}
	else if (wNewTop > (UINT)(pTree->cShowing - pTree->cFullVisible))
	{
		// we've gone too far down -- use max pos
		wNewTop = (pTree->cShowing - pTree->cFullVisible);
	}

	// if there's no room for anything to show. peg at the end
	if (wNewTop > 0 && wNewTop >= pTree->cShowing)
	{
		wNewTop = pTree->cShowing - 1;
	}

	hItem = TV_GetShownIndexItem(pTree, hItem, wNewTop);

	// BUGBUG (scotth): refreshing in regedit sometimes hits this case
	_ASSERT(hItem != NULL);

	if (NULL == hItem || pTree->hTop == hItem)
	{
		if (!pTree->m_bSmoothVScroll)
		{
			return FALSE;
		}
	}

	if (!pTree->m_bSmoothVScroll)
	{
		// 执行到这里, wNewTop变成新的垂直Scroll Pos，因为非平滑滚动模式时
		// 不能滚动到Item中间位置，所以滚动位置必须是hItem->iShownIndex。
		wNewTop = hItem->iShownIndex;
	}

	pTree->hTop = hItem;

	SetScrollPos(pTree->m_hWnd, SB_VERT, wNewTop, FALSE);
	TV_UpdateToolTip(pTree);

	return TRUE;
}

// ----------------------------------------------------------------------------
//
//  计算水平和垂直滚动条ranges, pages, and positions；
//  当需要的时候，显示或隐藏滚动条
//
// ----------------------------------------------------------------------------
BOOL TV_CalcScrollBars(PTREE pTree)
{
	if (pTree->m_hWnd->ci.style & TVS_NOSCROLL)
	{
		return FALSE;
	}

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);

	// 水平或垂直滚动条的显示状态是否有变化
	// 即TreeView的客户区是否有变化
	BOOL fChange = FALSE;

	// 禁止SetScrollInfo内部发送WM_STYLECHANGED事件，因为TreeView会处理这个事件，做一些无关的事情
	SetNotify(pTree->m_hWnd, FALSE);

	if (!(pTree->m_hWnd->ci.style & TVS_NOHSCROLL))
	{
		if ((SHORT)pTree->cxMax > (SHORT)pTree->cxWnd)
		{
			if (!pTree->m_bHorz)
			{
				fChange = TRUE;
				pTree->m_bHorz = TRUE;
			}

			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			si.nMax = pTree->cxMax - 1;
			si.nPage = pTree->cxWnd;

			TV_SetLeft(pTree, (UINT)SetScrollInfo(pTree->m_hWnd, SB_HORZ, &si, TRUE));
		}
		else if (pTree->m_bHorz)
		{
			TV_SetLeft(pTree, 0);
			SetScrollRange(pTree->m_hWnd, SB_HORZ, 0, 0, TRUE);

			pTree->m_bHorz = FALSE;
			fChange = TRUE;
		}
	}

	if (pTree->cShowing > pTree->cFullVisible)
	{
		if (!pTree->m_bVert)
		{
			pTree->m_bVert = TRUE;
			fChange = TRUE;
		}

		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nMin = 0;
		si.nMax = pTree->cShowing - 1;
		si.nPage = pTree->cFullVisible;

		TV_SetTopItem(pTree, (UINT)SetScrollInfo(pTree->m_hWnd, SB_VERT, &si, FALSE));
	}
	else if (pTree->m_bVert)
	{
		TV_SetTopItem(pTree, 0);
		SetScrollRange(pTree->m_hWnd, SB_VERT, 0, 0, TRUE);

		pTree->m_bVert = FALSE;
		fChange = TRUE;
	}

	SetNotify(pTree->m_hWnd, TRUE);

	if (fChange)
	{
		TV_SizeWnd(pTree, 0, 0);
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//
//  Handles horizontal scrolling.
//
// ----------------------------------------------------------------------------

BOOL TV_HorzScroll(PTREE pTree, UINT wCode, UINT wNewPos)
{
	BOOL fChanged;

	TV_DismissEdit(pTree, FALSE);

	switch (wCode)
	{
	case SB_BOTTOM:
		wNewPos = pTree->cxMax - pTree->cxWnd;
		break;

	case SB_ENDSCROLL:
		wNewPos = pTree->xPos;
		break;

	case SB_LINEDOWN:
		wNewPos = pTree->xPos + TREE_HORZLINE;
		break;

	case SB_LINEUP:
		wNewPos = pTree->xPos - TREE_HORZLINE;
		break;

	case SB_PAGEDOWN:
		wNewPos = pTree->xPos + (pTree->cxWnd - TREE_HORZLINE);
		break;

	case SB_PAGEUP:
		wNewPos = pTree->xPos - (pTree->cxWnd - TREE_HORZLINE);
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		break;

	case SB_TOP:
		wNewPos = 0;
		break;
	}

	if (fChanged = TV_SetLeft(pTree, wNewPos))
	{
		UpdateWindow(GetHostWnd(pTree->m_hWnd));
	}

	return (fChanged);
}

// ----------------------------------------------------------------------------
//
//  Handles vertical scrolling.
//
// ----------------------------------------------------------------------------

BOOL TV_VertScroll(PTREE pTree, UINT wCode, UINT wPos)
{
	UINT wNewPos = 0;
	UINT wOldPos;
	UINT uSmooth = 0;

	if (!pTree->hTop)
	{
		return FALSE;
	}

	if (pTree->m_bSmoothVScroll)
	{
		wOldPos = GetScrollPos(pTree->m_hWnd, SB_VERT);
	}
	else
	{
		wOldPos = pTree->hTop->iShownIndex;
	}
	TV_DismissEdit(pTree, FALSE);

	switch (wCode)
	{
	case SB_BOTTOM:
		wNewPos = pTree->cShowing - pTree->cFullVisible;
		break;

	case SB_ENDSCROLL:
		wNewPos = wOldPos;
		break;

	case SB_LINEDOWN:
		if (pTree->m_bSmoothVScroll)
		{
			// 每行滚动wSmoothVScroll像素
			wNewPos = wOldPos + pTree->wSmoothVScroll;
		}
		else
		{
			// 每行滚动一个Item
			wNewPos = wOldPos + TV_GetIntegral(pTree, pTree->hTop);
		}
		break;

	case SB_LINEUP:
		if (pTree->m_bSmoothVScroll)
		{
			wNewPos = wOldPos - pTree->wSmoothVScroll;
		}
		else
		{
			wNewPos = wOldPos - 1;
		}
		if (wNewPos > wOldPos)
		{
			wNewPos = 0;
		}
		break;

	case SB_PAGEDOWN:
		wNewPos = wOldPos + (pTree->cFullVisible - 1);
		break;

	case SB_PAGEUP:
		wNewPos = wOldPos - (pTree->cFullVisible - 1);
		if (wNewPos > wOldPos)
		{
			wNewPos = 0;
		}
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		uSmooth = SSW_EX_IMMEDIATE;
		wNewPos = wPos;
		break;

	case SB_TOP:
		wNewPos = 0;
		break;
	}

	BOOL fChanged = FALSE;
	if (fChanged = TV_SmoothSetTopItem(pTree, wNewPos, uSmooth))
	{
		InvalidateRect(pTree->m_hWnd, NULL);
	}

	return fChanged;
}


#ifdef DEBUG
static int nCompares;
#endif

typedef struct
{
	LPTSTR lpstr;
	BOOL bCallBack;
	HTREEITEM hItem;
} TVCOMPARE, *LPTVCOMPARE;

// Pointer comparision function for Sort and Search functions.
// lParam is lParam passed to sort/search functions.  Returns
// -1 if p1 < p2, 0 if p1 == p2, and 1 if p1 > p2.
//
int CALLBACK TV_DefCompare(LPTVCOMPARE sCmp1, LPTVCOMPARE sCmp2, LPARAM lParam)
{
#ifdef DEBUG
	++nCompares;
#endif

	return lstrcmpi(sCmp1->lpstr, sCmp2->lpstr);
}


int CALLBACK TV_CompareItems(LPTVCOMPARE sCmp1, LPTVCOMPARE sCmp2, LPARAM lParam)
{
	TV_SORTCB *pSortCB = (TV_SORTCB *)lParam;
#ifdef DEBUG
	++nCompares;
#endif

	return (pSortCB->lpfnCompare(sCmp1->hItem->lParam, sCmp2->hItem->lParam,
		pSortCB->lParam));
}


UINT TV_CountKids(HTREEITEM hItem)
{
	int cnt;

	for (cnt = 0, hItem = hItem->hKids; hItem; hItem = hItem->hNext)
	{
		cnt++;
	}

	return cnt;
}


// BUGBUG: bRecurse not implemented

BOOL PASCAL TV_SortCB(PTREE pTree, TV_SORTCB *pSortCB, BOOL bRecurse,
	PFNDPACOMPARE lpfnDPACompare)
{
	HDPA dpaSort;
	HDSA dsaCmp;
	HTREEITEM hItem, hNext, hFirstMoved;
	LPTVCOMPARE psCompare, *ppsCompare;
	int i, cKids;
	HTREEITEM hParent = pSortCB->hParent;

#ifdef DEBUG
	DWORD dwTime = GetTickCount();
	nCompares = 0;
#endif

	if (!hParent || hParent == TVI_ROOT)
	{
		hParent = pTree->m_hRootItem;
	}

	if (!ValidateTreeItem(hParent, FALSE))
	{
		return FALSE;    // Invalid parameter
	}

	// Code below assumes at least one kid
	cKids = TV_CountKids(hParent);
	if (!cKids)
	{
		return FALSE;
	}

	// Create a DSA for all the extra info we'll need
	dsaCmp = IUI_DSA_Create(sizeof(TVCOMPARE), cKids);
	if (!dsaCmp)
	{
		goto Error1;
	}

	// Create a DPA containing all the tree items
	dpaSort = IUI_DPA_Create(cKids);
	if (!dpaSort)
	{
		goto Error2;
	}

	for (hItem = hParent->hKids; hItem; hItem = hItem->hNext)
	{
		TVCOMPARE sCompare;
		int nItem;

		// If I can't sort all of them, I don't want to sort any of them

		// We want to cache the text callback for default processing
		if (!lpfnDPACompare && hItem->lpstr == LPSTR_TEXTCALLBACK)
		{
			XTVITEM sItem;
			TCHAR szTemp[MAX_PATH];

			sItem.pszText = szTemp;
			sItem.cchTextMax = ARRAYSIZE(szTemp);
			TV_GetItem(pTree, hItem, TVIF_TEXT, &sItem);

			sCompare.lpstr = NULL;
			sCompare.bCallBack = TRUE;
			Str_Set(&sCompare.lpstr, sItem.pszText);
			if (!sCompare.lpstr)
			{
				goto Error3;
			}
		}
		else
		{
			sCompare.lpstr = hItem->lpstr;
			sCompare.bCallBack = FALSE;
		}

		// Create the pointer for this guy and add it to the DPA list
		sCompare.hItem = hItem;
		nItem = DSA_AppendItem(dsaCmp, &sCompare);
		if (nItem < 0)
		{
			if (sCompare.bCallBack)
			{
				Str_Set(&sCompare.lpstr, NULL);
			}
			goto Error3;
		}

		if (DPA_AppendPtr(dpaSort, IUI_DSA_GetItemPtr(dsaCmp, nItem)) < 0)
		{
			goto Error3;
		}
	}

	// Sort the DPA, then stick them back under the parent in the new order
	IUI_DPA_Sort(dpaSort, lpfnDPACompare ? (PFNDPACOMPARE)lpfnDPACompare :
		(PFNDPACOMPARE)TV_DefCompare, (LPARAM)pSortCB);


	// Look for the first moved item, so we can invalidate a smaller area
	ppsCompare = (LPTVCOMPARE *)DPA_GetPtrPtr(dpaSort);
	if (hParent->hKids != (*ppsCompare)->hItem)
	{
		hParent->hKids = (*ppsCompare)->hItem;
		hFirstMoved = hParent->hKids;
	}
	else
	{
		hFirstMoved = NULL;
	}

	// We do n-1 iterations here
	for (i = DPA_GetPtrCount(dpaSort) - 1; i > 0; --i, ++ppsCompare)
	{
		hNext = (*(ppsCompare + 1))->hItem;
		if ((*ppsCompare)->hItem->hNext != hNext && !hFirstMoved)
		{
			hFirstMoved = hNext;
		}
		(*ppsCompare)->hItem->hNext = hNext;
	}
	(*ppsCompare)->hItem->hNext = NULL;

	TV_UpdateShownIndexes(pTree, hParent);
	if ((pSortCB->hParent == TVI_ROOT) || !hParent)
	{
		if (pTree->cShowing < pTree->cFullVisible)
		{
			pTree->hTop = pTree->m_hRootItem->hKids;
		}
	}

	if (hFirstMoved && (hParent->state & TVIS_EXPANDED))
	{
		RECT rcUpdate;

		TV_GetItemRect(pTree, hFirstMoved, &rcUpdate, FALSE);
		if (hParent->hNext)
		{
			RECT rcTemp;

			TV_GetItemRect(pTree, hParent->hNext, &rcTemp, FALSE);
			rcUpdate.bottom = rcTemp.bottom;
		}
		else
		{
			RECT rcClient;
			GetClientRect(GetHostWnd(pTree->m_hWnd), &rcClient);
			// Set to maximal positive number, so the whole rest of
			// the treeview gets invalidated
			rcUpdate.bottom = rcClient.bottom;
		}
		if (pTree->m_bRedraw)
		{
			InvalidateRect(GetHostWnd(pTree->m_hWnd), &rcUpdate, TRUE);
		}
	}

Error3:
	IUI_DPA_Destroy(dpaSort);
Error2:
	for (i = DSA_GetItemCount(dsaCmp) - 1; i >= 0; --i)
	{
		psCompare = (LPTVCOMPARE)IUI_DSA_GetItemPtr(dsaCmp, i);
		if (psCompare->bCallBack)
		{
			Str_Set(&(psCompare->lpstr), NULL);
		}
	}
	IUI_DSA_Destroy(dsaCmp);
Error1:

#ifdef DEBUG
	//L TraceMsg(TF_TREEVIEW, "tv.sort: %ld ms; %d cmps", GetTickCount() - dwTime, nCompares);
#endif

	{
		int wNewPos;
		// restore the scroll position
		if (GetWindowStyle(GetHostWnd(pTree->m_hWnd)) & WS_VSCROLL)
		{
			SCROLLINFO si;

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			wNewPos = 0;
			if (GetScrollInfo(pTree->m_hWnd, SB_VERT, &si))
			{
				wNewPos = si.nPos;
			}

		}
		else
		{
			wNewPos = 0;
		}

		if (TV_SetTopItem(pTree, wNewPos))
		{
			UpdateWindow(GetHostWnd(pTree->m_hWnd));
		}
	}

	// if the caret is the child of the thing that was sorted, make sure it's
	// visible (but if we're sorting something completely unrelated, don't bother
	if (pTree->m_hCaretItem)
	{
		hItem = pTree->m_hCaretItem;
		do
		{
			// do this first.  if hParent is m_hCaretItem, we don't want to ensure visible...
			// only if it's an eventual child
			hItem = hItem->hParent;
			if (hParent == hItem)
			{
				TV_EnsureVisible(pTree, pTree->m_hCaretItem);
			}
		} while (hItem && hItem != pTree->m_hRootItem);
	}

	// The items in the view may have moved around; let apps know
	// Do this last because this call might yield
	MyNotifyWinEvent(EVENT_OBJECT_REORDER, GetHostWnd(pTree->m_hWnd), OBJID_CLIENT, 0);

	return TRUE;
}


BOOL TV_SortChildrenCB(PTREE pTree, LPTV_SORTCB pSortCB, BOOL bRecurse)
{
	return (TV_SortCB(pTree, pSortCB, bRecurse, (PFNDPACOMPARE)TV_CompareItems));
}


BOOL TV_SortChildren(PTREE pTree, HTREEITEM hParent, BOOL bRecurse)
{
	TV_SORTCB sSortCB;

	sSortCB.hParent = hParent;
	return (TV_SortCB(pTree, &sSortCB, bRecurse, NULL));
}

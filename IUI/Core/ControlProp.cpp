#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

namespace IUI
{


#define SCREEN_POSITION_HORIZONTAL_MIN                  -3840 ///< 设置控件相关坐标属性时，指定屏幕坐标的最小值
#define SCREEN_POSITION_HORIZONTAL_MAX                  3840 ///< 设置控件相关坐标属性时，指定屏幕坐标的最大值


	CControlBaseProp::CControlBaseProp()
	{
		m_pParent = NULL;
		m_hBindCtrl = NULL;
		m_pvControls = new std::vector<CControlBaseProp *>;

		m_lStructSize.vtEx = VT_EX_STRUCT_SIZE;
		m_lStructSize.SetVarName(_T("m_lStructSize"));
		m_lStructSize.bShowInPropertiesWnd = m_lStructSize.bSaveToSkinFile = false;
		m_lStructSize = (LONG)sizeof(CControlBaseProp);

		m_eControlType = CT_ARROW;
		m_eControlType.vtEx = VT_EX_CONTROL_TYPE;
		m_eControlType.bReadOnly = true;
		m_eControlType.bGroup = true;
		m_eControlType.SetGroupName(_T("General"));
		m_eControlType.SetVarName(_T("m_eControlType"));
		m_eControlType.SetPropWndName(_T("Type"));
		m_eControlType.SetDescription(_T("Display the type of the control."));

		m_strName = _T("");
		m_strName.vtEx = VT_EX_CONTROL_ID_MACRO;
		m_strName.SetVarName(_T("m_strName"));
		m_strName.SetIUIElementName(_T("name"));
		m_strName.SetPropWndName(_T("Name"));
		m_strName.SetDescription(_T("Input the control's name."));
	}

	CControlBaseProp::~CControlBaseProp()
	{
		if (m_pvControls != NULL)
		{
			std::vector<CControlBaseProp *>::iterator it = m_pvControls->begin();
			for (; it != m_pvControls->end(); ++it)
			{
				CControlBaseProp *pProp = *it;
				delete pProp;
				pProp = NULL;
			}

			m_pvControls->clear();
		}

		delete m_pvControls;
		m_pvControls = NULL;
	}

	int CControlBaseProp::Copy(const CControlBaseProp *pSrc, CControlBaseProp *pDest)
	{
		if (pSrc == pDest)
		{
			return 0;
		}

		// 得到本类对象大小，在类成员变量有增加时，就会断言。
		int nSize = sizeof(CControlBaseProp);
		_ASSERT(5992 == nSize);

		// 先备份目标的m_pvControls
		std::vector<CControlBaseProp *> *pvOldControls = pDest->m_pvControls;

		*pDest = *pSrc;

		// 指针深拷贝
		pDest->m_pvControls = pvOldControls;
		*pDest->m_pvControls = *pSrc->m_pvControls;

		return 0;
	}

	BOOL CControlBaseProp::IsControl() const
	{
		return FALSE;
	}

	LONG CControlBaseProp::GetPropertyCount() const
	{
		LONG lVarCount = 0;
		CONTROL_TYPE eControlType = m_eControlType;

		LONG lTotalSize = m_lStructSize;
		LONG lVirtualSize = (LONG)(LPARAM)(&m_lStructSize) - (LONG)(LPARAM)this;
		LONG lVariantSize = sizeof(UIVARIANT);
		LONG lVariantSize2 = (LONG)(LPARAM)(&m_eControlType) - (LONG)(LPARAM)(&m_lStructSize);

		if (lVariantSize2 != lVariantSize)
		{
			_ASSERT(FALSE);
		}

		if (eControlType == CT_MENU)
		{
			//CMenuProp *pMenuProp = (CMenuProp *)this;
			//lVarCount = GetUIVariantCount(&(pMenuProp->m_pParentMenuItem));
		}
		else
		{
			lVarCount = (lTotalSize - lVirtualSize) / lVariantSize;

			if ((lTotalSize - lVirtualSize) % lVariantSize != 0)
			{
				_ASSERT(FALSE);
			}
		}

		return lVarCount;
	}

	UIVARIANT *CControlBaseProp::GetProperty(int nIndex)
	{
		int nCount = GetPropertyCount();

		if (nIndex < 0 || nIndex >= nCount)
		{
			return NULL;
		}

		LONG lVariantSize = sizeof(UIVARIANT);
		UIVARIANT *pVariant = (UIVARIANT *)((BYTE *)(&m_lStructSize) + lVariantSize * nIndex);
		return pVariant;
	}

	const UIVARIANT *CControlBaseProp::GetProperty(int nIndex) const
	{
		int nCount = GetPropertyCount();

		if (nIndex < 0 || nIndex >= nCount)
		{
			return NULL;
		}

		LONG lVariantSize = sizeof(UIVARIANT);
		UIVARIANT *pVariant = (UIVARIANT *)((BYTE *)(&m_lStructSize) + lVariantSize * nIndex);
		return pVariant;
	}

	int CControlBaseProp::SetVariant(int nIndex, const UIVARIANT *pVariant)
	{
		int nCount = GetPropertyCount();

		if (nIndex < 0 || nIndex >= nCount)
		{
			return -1;
		}

		LONG lVariantSize = sizeof(UIVARIANT);
		UIVARIANT *pFindVariant = (UIVARIANT *)((BYTE *)(&m_lStructSize) + lVariantSize * nIndex);

		*pFindVariant = *pVariant;

		return 0;
	}

	int CControlBaseProp::GetIndex(const UIVARIANT *pVar) const
	{
		if (pVar == NULL)
		{
			return -1;
		}

		LONG lVariantSize = sizeof(UIVARIANT);
		INT_PTR nOffset = ((BYTE *)pVar - (BYTE *)(&m_lStructSize));
#ifdef _WIN64
		_ASSERT((UINT_PTR)nOffset >> 32 == 0);
#endif
		int nIndex = int(nOffset / lVariantSize);
		_ASSERT((nOffset % lVariantSize) == 0);

		return nIndex;
	}

	LONG CControlBaseProp::GetUIVariantCount(void *pLastUiVariant) const
	{
		LONG lVariantSize = sizeof(UIVARIANT);
		LONG lVariantSize2 = (LONG)(LPARAM)(&m_eControlType) - (LONG)(LPARAM)(&m_lStructSize);

		if (lVariantSize2 != lVariantSize)
		{
			_ASSERT(FALSE);
			MessageBox(NULL, _T("sizeof(UIVARIANT) is undefined!"), NULL, MB_OK);
		}

		// 对于派生类结构体中加了非UIVARIANT类型的变量，需要特殊处理。
		LONG lVarCount = 0;

		lVarCount = ((LONG)(LPARAM)(pLastUiVariant) - (LONG)LPARAM(&m_lStructSize)) / lVariantSize;

		if (((LONG)(LPARAM)(pLastUiVariant) - (LONG)LPARAM(&m_lStructSize)) % lVariantSize != 0)
		{
			MessageBox(NULL, _T("lVarCount is invalid!"), NULL, MB_OK);
		}

		return lVarCount;
	}

	CIUIString CControlBaseProp::GetIDPrefix(CONTROL_TYPE m_eControlType)
	{
		CIUIString strName;

		switch (m_eControlType)
		{
		case CT_PUSHBUTTON:
			strName = _T("IDC_BTN_");
			break;

		case CT_CHECK:
			strName = _T("IDC_CHK_");
			break;

		case CT_RADIO:
			strName = _T("IDC_RAD_");
			break;

		case CT_STATIC:
			strName = _T("IDC_STA_");
			break;

		case CT_RICHEDIT:
			strName = _T("IDC_RE_");
			break;

		case CT_COMBOBOX:
			strName = _T("IDC_CMB_");
			break;

		case CT_SPIN:
			strName = _T("IDC_SPN_");
			break;

		case CT_PROGRESS:
			strName = _T("IDC_PRG_");
			break;

		case CT_SLIDER:
			strName = _T("IDC_SLD_");
			break;

		case CT_HOTKEY:
			strName = _T("IDC_HOT_");
			break;

		case CT_IPADDRESS:
			strName = _T("IDC_IPA_");
			break;

		case CT_LISTVIEW:
			strName = _T("IDC_LSTCTRL_");
			break;

		case CT_TREEVIEW:
			strName = _T("IDC_TREECTRL_");
			break;

		case CT_TREELIST:
			strName = _T("IDC_TREELST_");
			break;

		case CT_HTMLVIEW:
			strName = _T("IDC_HTMLCTRL_");
			break;

		case CT_IM:
			strName = _T("IDC_IM_");
			break;

		case CT_SPLITTER:
			strName = _T("IDC_SPLITTER_");
			break;

		case CT_DATETIMEPICKER:
			strName = _T("IDC_DATETIME_");
			break;

		case CT_MENUBAR:
			strName = _T("IDC_MENUBAR_");
			break;

		case CT_PNL_CANVAS:
			strName = _T("IDC_CANVAS_");
			break;

		case CT_PNL_TABCONTROL:
			strName = _T("IDC_PNL_TAB_");
			break;

		case CT_PNL_DOCK:
			strName = _T("IDC_PNL_DOCK_");
			break;

		case CT_UNIFORM_GRID:
			strName = _T("IDC_PNL_UGRID_");
			break;

		case CT_MENU:
			strName = _T("IDR_MENU_");
			break;

		case CT_HEADERCTRL:
			strName = _T("IDR_HEADER_");
			break;

		case CT_SCROLLBAR:
			strName = _T("IDR_SB_");
			break;

		case CT_RESOURCE_FONT:
			strName = _T("IDF_");
			break;

		case CT_RESOURCE_COLOR:
			strName = _T("IDR_COLOR_");
			break;

		case CT_RESOURCE_STRING:
			strName = _T("IDS_STRING");
			break;

		case CT_ARROW:
			strName = _T("IDC_");
			break;

		default:
			strName = _T("IDC_");
			break;
		}

		return (LPCTSTR)strName;
	}

	bool CControlBaseProp::IsFontResource() const
	{
		_ASSERT(FALSE);
		return false;
	}

	bool CControlBaseProp::IsColorResource() const
	{
		_ASSERT(FALSE);
		return false;
	}

	CIUIString CControlBaseProp::GetClassName(BOOL bInCode/* = FALSE*/) const
	{
		CONTROL_TYPE eControlType = m_eControlType;

		return GetControlTypeString(eControlType, bInCode);
	}

	LPCTSTR CControlBaseProp::GetVariablePrefix() const
	{
		CONTROL_TYPE eControlType = m_eControlType;

		switch (eControlType)
		{
		case  CT_PUSHBUTTON:
			return _T("Btn");

		case CT_CHECK:
			return _T("Chk");

		case CT_RADIO:
			return _T("Rad");

		// Static
		case CT_STATIC:
			return _T("Sta");

		case CT_COMBOBOX:
			return _T("Cmb");

		case CT_SPIN:
			return _T("Spn");

		case CT_PROGRESS:
			return _T("Prg");

		case CT_SLIDER:
			return _T("Sld");

		case CT_HOTKEY:
			return _T("HotKey");

		case CT_IPADDRESS:
			return _T("IPA");

		case CT_LISTVIEW:
			return _T("Lst");

		case CT_TREEVIEW:
			return _T("Tree");

		case CT_SCROLLBAR:
			return _T("Sb");

		case CT_RICHEDIT:
			return _T("Re");

		case CT_PNL_TABCONTROL:
			return _T("Tab");

		case CT_PNL_CANVAS:
			return _T("Canvas");

		default:
			// Unknown type.
			return _T("");
		}

		return _T("");
	}

	CIUIString  CControlBaseProp::GetVariableName() const
	{
		CIUIString strCtrlName = (LPCTSTR)m_strName;

		CIUIString strPrefix = CControlBaseProp::GetIDPrefix(m_eControlType);

		if (strCtrlName.Left(strPrefix.GetLength()) == strPrefix)
		{
			strCtrlName.Delete(0, strPrefix.GetLength());
		}

		strCtrlName.MakeLower();
		int nLength = strCtrlName.GetLength();

		for (int i = 0; i < nLength; ++i)
		{
			BOOL bMakeUpper = FALSE;

			if (i > 0)
			{
				if (strCtrlName[i - 1] == '_' || strCtrlName[i - 1] == '-')
				{
					bMakeUpper = TRUE;
				}
			}

			if (i == 0)
			{
				bMakeUpper = TRUE;
			}

			if (bMakeUpper)
			{
				CIUIString strUpper;
				strUpper = strCtrlName[i];
				strUpper.MakeUpper();
				strCtrlName.SetAt(i, strUpper[0]);
			}
		}

		strCtrlName.Remove('-');
		strCtrlName.Remove('_');

		return (LPCTSTR)strCtrlName;
	}

	void CControlBaseProp::SetFileName(LPCTSTR lpszFileName)
	{
		m_strFileName = lpszFileName;
	}

	CIUIString CControlBaseProp::GetFileName() const
	{
		return m_strFileName;
	}

	int CControlBaseProp::AddChild(CControlBaseProp *pChildProp)
	{
		if (pChildProp == NULL)
		{
			return -1;
		}

		pChildProp->m_pParent = this;
		m_pvControls->push_back(pChildProp);

		return 0;
	}

	int CControlBaseProp::InsertChild(int nIndex, CControlBaseProp *pChildProp)
	{
		if (pChildProp == NULL)
		{
			return -1;
		}

		int nCount = m_pvControls->size();

		if (nIndex < -1 || nIndex > nCount)
		{
			return -1;
		}

		pChildProp->m_pParent = this;
		if (nIndex == -1)
		{
			m_pvControls->push_back(pChildProp);

			int nCount = (int)m_pvControls->size();
			return nCount - 1;
		}

		m_pvControls->insert(m_pvControls->begin() + nIndex, pChildProp);

		return nIndex;
	}

	int CControlBaseProp::DeleteChild(CControlBaseProp *pProp)
	{
		std::vector<CControlBaseProp *>::iterator it = find(m_pvControls->begin(), m_pvControls->end(), pProp);
		if (it == m_pvControls->end())
		{
			return -1;
		}

		m_pvControls->erase(it);

		return 0;
	}

	int CControlBaseProp::RemoveChild(int nIndex)
	{
		int nCount = m_pvControls->size();

		if (nIndex < -1 || nIndex > nCount)
		{
			return -1;
		}

		CControlBaseProp *pChild = (*m_pvControls)[nIndex];
		m_pvControls->erase(m_pvControls->begin() + nIndex);

		return 0;
	}

	int CControlBaseProp::DeleteAllChildren()
	{
		int nCount = (int)m_pvControls->size();

		for (int i = nCount - 1; i >= 0; --i)
		{
			CControlBaseProp *pChild = (*m_pvControls)[i];
			delete pChild;
			pChild = NULL;
		}

		m_pvControls->clear();

		return 0;
	}

	CControlBaseProp *CControlBaseProp::GetChild(int nIndex)
	{
		int nCount = m_pvControls->size();

		if (nIndex < 0 || nIndex >= nCount)
		{
			return NULL;
		}

		return (*m_pvControls)[nIndex];
	}

	const CControlBaseProp *CControlBaseProp::GetChild(int nIndex) const
	{
		int nCount = m_pvControls->size();

		if (nIndex < 0 || nIndex > nCount)
		{
			return NULL;
		}

		return (*m_pvControls)[nIndex];
	}

	CControlBaseProp *CControlBaseProp::GetParent()
	{
		return m_pParent;
	}

	const CControlBaseProp *CControlBaseProp::GetParent() const
	{
		return m_pParent;
	}

	int CControlBaseProp::SetBindCtrl(HWLWND hBindCtrl)
	{
		m_hBindCtrl = hBindCtrl;
		return 0;
	}

	HWLWND CControlBaseProp::GetBindCtrl() const
	{
		return m_hBindCtrl;
	}

	int CControlBaseProp::GetChildCount() const
	{
		int nCount = (int)m_pvControls->size();

		return nCount;
	}

	CControlBaseProp *CControlBaseProp::FindChild(LPCTSTR lpszName)
	{
		return (CControlBaseProp *)((const CControlBaseProp *)this)->FindChild(lpszName);
	}

	const CControlBaseProp *CControlBaseProp::FindChild(LPCTSTR lpszName) const
	{
		// 先判断自己的名字
		if (CIUIString((LPCTSTR)m_strName) == lpszName)
		{
			return this;
		}

		const CControlBaseProp *pRet = NULL;

		int nChildCount = GetChildCount();
		for (int i = 0; i < nChildCount; ++i)
		{
			const CControlBaseProp *pChildProp = GetChild(i);
			if (pChildProp != NULL)
			{
				// 递归遍历所有子孙控件
				pRet = pChildProp->FindChild(lpszName);
				if (pRet != NULL)
				{
					break;
				}
			}
			else
			{
				_ASSERT(FALSE);
			}
		}

		return pRet;
	}

	int CControlBaseProp::GetChildList(
		std::vector<CControlBaseProp *> *pvControls,
		BOOL bIncludeSelf)
	{

		if (pvControls == NULL)
		{
			return -1;
		}

		pvControls->clear();

		if (bIncludeSelf)
		{
			pvControls->push_back(this);
		}

		// 递归遍历所有子孙控件
		int nChildCount = GetChildCount();

		for (int i = 0; i < nChildCount; ++i)
		{
			CControlBaseProp *pChildProp = GetChild(i);
			if (pChildProp != NULL)
			{
				// 不受bIncludeSelf影响，子控件一定要加入列表。
				pvControls->push_back(pChildProp);

				pChildProp->GetChildList(pvControls, FALSE);
			}
			else
			{
				_ASSERT(FALSE);
			}
		}

		return 0;
	}

	int CControlBaseProp::GetChildList(
		std::vector<const CControlBaseProp *> *pvControls,
		BOOL bIncludeSelf) const
	{

		if (pvControls == NULL)
		{
			return -1;
		}

		if (bIncludeSelf)
		{
			pvControls->push_back(this);
		}

		// 递归遍历所有子孙控件
		int nChildCount = GetChildCount();

		for (int i = 0; i < nChildCount; ++i)
		{
			const CControlBaseProp *pChildProp = GetChild(i);
			_ASSERT(pChildProp != NULL);
			if (pChildProp)
			{
				// 不受bIncludeSelf影响，子控件一定要加入列表。
				pvControls->push_back(pChildProp);

				pChildProp->GetChildList(pvControls, FALSE);
			}
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// protected

	int CControlBaseProp::InitBoolVariant(UIVARIANT *pVariant, bool bInitValue, LPCTSTR lpszIUIElementName, LPCTSTR lpszPropWndName, bool bGroup/* = false*/, LPCTSTR lpszGroupName/* = NULL*/)
	{
		if (pVariant == NULL)
		{
			return -1;
		}

		if (lpszIUIElementName == NULL)
		{
			return -2;
		}

		if (lpszPropWndName == NULL)
		{
			return -3;
		}

		if (bGroup && lpszGroupName == NULL)
		{
			return -4;
		}

		*pVariant = bInitValue;
		pVariant->SetDefaultValue((_variant_t)*pVariant);
		pVariant->SetIUIElementName(lpszIUIElementName);
		pVariant->SetPropWndName(lpszPropWndName);

		if (bGroup)
		{
			pVariant->bGroup = true;
			pVariant->SetGroupName(lpszGroupName);
		}

		return 0;
	}

	int CControlBaseProp::InitResFontIDVariant(UIVARIANT *pVariant, LPCTSTR lpszIUIElementName, LPCTSTR lpszPropWndName, bool bGroup/* = false*/, LPCTSTR lpszGroupName/* = NULL*/)
	{
		if (pVariant == NULL)
		{
			return -1;
		}

		if (lpszIUIElementName == NULL)
		{
			return -2;
		}

		if (lpszPropWndName == NULL)
		{
			return -3;
		}

		if (bGroup && lpszGroupName == NULL)
		{
			return -4;
		}

		*pVariant = _T("");
		pVariant->vtEx = VT_EX_RESOURCE_FONT;
		pVariant->SetIUIElementName(lpszIUIElementName);
		pVariant->SetPropWndName(lpszPropWndName);

		if (bGroup)
		{
			pVariant->bGroup = true;
			pVariant->SetGroupName(lpszGroupName);
		}

		return 0;
	}

	int CControlBaseProp::InitResColorIDVariant(UIVARIANT *pVariant, LPCTSTR lpszIUIElementName, LPCTSTR lpszPropWndName, bool bGroup/* = false*/, LPCTSTR lpszGroupName/* = NULL*/)
	{
		if (pVariant == NULL)
		{
			return -1;
		}

		if (lpszIUIElementName == NULL)
		{
			return -2;
		}

		if (lpszPropWndName == NULL)
		{
			return -3;
		}

		if (bGroup && lpszGroupName == NULL)
		{
			return -4;
		}

		*pVariant = _T("");
		pVariant->vtEx = VT_EX_RESOURCE_COLOR;
		pVariant->SetIUIElementName(lpszIUIElementName);
		pVariant->SetPropWndName(lpszPropWndName);

		if (bGroup)
		{
			pVariant->bGroup = true;
			pVariant->SetGroupName(lpszGroupName);
		}

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	CControlProp::CControlProp()
	{
		RECT rect = { 0, 0, 0, 0 };

		m_lWidth = (LONG)(-1);
		m_lWidth.SetVarName(_T("m_lWidth"));
		m_lWidth.bShowInPropertiesWnd = m_lWidth.bSaveToSkinFile = false;
		m_lWidth.SetIUIElementName(_T("width"));
		m_lWidth.SetPropWndName(_T("Width"));
		m_lWidth.SetDescription(_T("Input the control's width."));

		m_lHeight = (LONG)0;
		m_lHeight.SetVarName(_T("m_lHeight"));
		m_lHeight.bShowInPropertiesWnd = m_lHeight.bSaveToSkinFile = false;
		m_lHeight.SetIUIElementName(_T("height"));
		m_lHeight.SetPropWndName(_T("Height"));
		m_lHeight.SetDescription(_T("Input the control's height."));

		m_rcControl.vtEx = VT_EX_RECT_WH;
		m_rcControl = rect;
		m_rcControl.SetDefaultValue((_variant_t)m_rcControl);
		m_rcControl.SetVarName(_T("m_rcControl"));
		m_rcControl.SetIUIElementName(_T("Rect"));
		m_rcControl.bShowInPropertiesWnd = false;

		m_bVisible = true;
		m_bVisible.SetDefaultValue((_variant_t)m_bVisible);
		m_bVisible.SetVarName(_T("m_bVisible"));
		m_bVisible.SetIUIElementName(_T("Visibility"));
		m_bVisible.SetPropWndName(_T("Visible"));
		m_bVisible.SetDescription(_T("Specifies that the control is initially visible."));

		m_bEnabled = true;
		m_bEnabled.SetDefaultValue((_variant_t)m_bEnabled);
		m_bEnabled.SetVarName(_T("m_bEnabled"));
		m_bEnabled.SetIUIElementName(_T("enabled"));
		m_bEnabled.SetPropWndName(_T("Enabled"));
		m_bEnabled.SetDescription(_T("Specifies that the control will be initially enabled."));

		m_bTabStop = false;
		m_bTabStop.SetDefaultValue((_variant_t)m_bTabStop);
		m_bTabStop.SetIUIElementName(_T("IsTabStop"));
		m_bTabStop.SetPropWndName(_T("Tabstop"));
		m_bTabStop.SetDescription(_T("Specifies that the user can move to this control with the TAB key."));

		m_bClipChildren = true;
		m_bClipChildren.SetDefaultValue((_variant_t)m_bClipChildren);
		m_bClipChildren.SetIUIElementName(_T("IsClipChildren"));
		m_bClipChildren.SetPropWndName(_T("Clip Children"));

		m_lTitlebarHeight = (LONG)0;
		m_lTitlebarHeight.SetDefaultValue((_variant_t)m_lTitlebarHeight);
		m_lTitlebarHeight.SetVarName(_T("m_lTitlebarHeight"));
		m_lTitlebarHeight.SetIUIElementName(_T("TitleBarHeight"));
		m_lTitlebarHeight.SetPropWndName(_T("Title Bar Height"));
		m_lTitlebarHeight.SetDescription(_T("Title Bar Height, -1 means all client as title bar."));
		m_lTitlebarHeight.nMinValue = -1;
		m_lTitlebarHeight.nMaxValue = 10000;

		m_bMouseTransparent = false;
		m_bMouseTransparent.SetDefaultValue((_variant_t)m_bMouseTransparent);
		m_bMouseTransparent.SetIUIElementName(_T("MouseTransparent"));
		m_bMouseTransparent.SetPropWndName(_T("Mouse Transparent"));
		m_bMouseTransparent.SetDescription(_T("鼠标是否可穿透这个控件. 可穿透鼠标的控件不响应鼠标消息。"));

		m_strBindXml = _T("");
		m_strBindXml.SetDefaultValue((_variant_t)m_strBindXml);
		m_strBindXml.SetVarName(_T("m_strBindXml"));
		m_strBindXml.SetIUIElementName(_T("Xml"));
		m_strBindXml.SetPropWndName(_T("Bind Xml"));
		m_strBindXml.SetDescription(_T("Bind Xml."));

		// Layout
		m_eHorLayoutMode = CLH_FREE;
		m_eHorLayoutMode.vtEx = VT_EX_LAYOUT_HOR;
		m_eHorLayoutMode.SetDefaultValue((_variant_t)m_eHorLayoutMode);
		m_eHorLayoutMode.SetVarName(_T("m_eHorLayoutMode"));
		m_eHorLayoutMode.bReadOnly = true;
		m_eHorLayoutMode.bGroup = true;
		m_eHorLayoutMode.SetGroupName(_T("Layout Mode"));
		m_eHorLayoutMode.SetIUIElementName(_T("HAlign"));
		m_eHorLayoutMode.SetPropWndName(_T("Horizontal Layout"));
		m_eHorLayoutMode.SetDescription(_T("Specifies the horizontal layout mode for the control."));
		m_eHorLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLH_LEFT).GetEnumString());
		m_eHorLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLH_RIGHT).GetEnumString());
		m_eHorLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLH_CENTER).GetEnumString());
		m_eHorLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLH_FILL).GetEnumString());
		m_eHorLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLH_FREE).GetEnumString());

		m_eVerLayoutMode = CLV_FREE;
		m_eVerLayoutMode.vtEx = VT_EX_LAYOUT_VER;
		m_eVerLayoutMode.SetDefaultValue((_variant_t)m_eVerLayoutMode);
		m_eVerLayoutMode.SetVarName(_T("m_eVerLayoutMode"));
		m_eVerLayoutMode.bReadOnly = true;
		m_eVerLayoutMode.SetIUIElementName(_T("VAlign"));
		m_eVerLayoutMode.SetPropWndName(_T("Vertical Layout"));
		m_eVerLayoutMode.SetDescription(_T("Specifies the vertical layout mode for the control."));
		m_eVerLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLV_TOP).GetEnumString());
		m_eVerLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLV_BOTTOM).GetEnumString());
		m_eVerLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLV_CENTER).GetEnumString());
		m_eVerLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLV_FILL).GetEnumString());
		m_eVerLayoutMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CLV_FREE).GetEnumString());

		m_eDockProperty = DOCK_LEFT;
		m_eDockProperty.vtEx = VT_EX_DOCK;
		m_eDockProperty.SetDefaultValue((_variant_t)m_eDockProperty);
		m_eDockProperty.SetVarName(_T("m_eDockProperty"));
		m_eDockProperty.bReadOnly = true;
		m_eDockProperty.SetIUIElementName(_T("Dock"));
		m_eDockProperty.SetPropWndName(_T("Dock"));
		m_eDockProperty.SetDescription(_T("在DockPanel中的停靠方向."));
		m_eDockProperty.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DOCK_LEFT).GetEnumString());
		m_eDockProperty.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DOCK_TOP).GetEnumString());
		m_eDockProperty.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DOCK_RIGHT).GetEnumString());
		m_eDockProperty.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DOCK_BOTTOM).GetEnumString());

		m_rcLayoutMargin.vtEx = VT_EX_RECT;
		m_rcLayoutMargin = rect;
		m_rcLayoutMargin.SetDefaultValue((_variant_t)m_rcLayoutMargin);
		m_rcLayoutMargin.SetPropWndName(_T("Margin"));
		m_rcLayoutMargin.SetIUIElementName(_T("Margin"));
		m_rcLayoutMargin.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcLayoutMargin.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		m_rcInset.vtEx = VT_EX_RECT;
		m_rcInset = rect;
		m_rcInset.SetDefaultValue((_variant_t)m_rcInset);
		m_rcInset.SetPropWndName(_T("Inset"));
		m_rcInset.SetIUIElementName(_T("inset"));
		m_rcInset.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcInset.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Background group
		m_bDrawBkColor = false;
		m_bDrawBkColor.bGroup = true;
		m_bDrawBkColor.SetGroupName(_T("Background Color"));
		m_bDrawBkColor.SetDefaultValue((_variant_t)m_bDrawBkColor);
		m_bDrawBkColor.SetIUIElementName(_T("IsDrawBkColor"));
		m_bDrawBkColor.SetPropWndName(_T("Is Draw Background Color"));

		m_strBkColorResIDN = _T("");
		m_strBkColorResIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDN.SetIUIElementName(_T("BkColorResN"));
		m_strBkColorResIDN.SetPropWndName(_T("Normal Bk Color"));

		m_strBkColorResIDH = _T("");
		m_strBkColorResIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDH.SetIUIElementName(_T("BkColorResH"));
		m_strBkColorResIDH.SetPropWndName(_T("Highlight Bk Color"));

		m_strBkColorResIDP = _T("");
		m_strBkColorResIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDP.SetIUIElementName(_T("BkColorResP"));
		m_strBkColorResIDP.SetPropWndName(_T("Pressed Bk Color"));

		m_strBkColorResIDD = _T("");
		m_strBkColorResIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDD.SetIUIElementName(_T("BkColorResD"));
		m_strBkColorResIDD.SetPropWndName(_T("Disabled Bk Color"));

		m_strBkColorResIDCN = _T("");
		m_strBkColorResIDCN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDCN.SetIUIElementName(_T("BkColorResCN"));
		m_strBkColorResIDCN.SetPropWndName(_T("Checked Normal Bk Color"));

		m_strBkColorResIDCH = _T("");
		m_strBkColorResIDCH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDCH.SetIUIElementName(_T("BkColorResCH"));
		m_strBkColorResIDCH.SetPropWndName(_T("Checked Hover Bk Color"));

		m_strBkColorResIDCP = _T("");
		m_strBkColorResIDCP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDCP.SetIUIElementName(_T("BkColorResCP"));
		m_strBkColorResIDCP.SetPropWndName(_T("Checked Pressed Bk Color"));

		m_strBkColorResIDCD = _T("");
		m_strBkColorResIDCD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBkColorResIDCD.SetIUIElementName(_T("BkColorResCD"));
		m_strBkColorResIDCD.SetPropWndName(_T("Checked Disabled Bk Color"));

		m_bCombineBackgroundImages.m_pBindCtrlProp = this;
		m_bCombineBackgroundImages = true;
		m_bCombineBackgroundImages.bGroup = true;
		m_bCombineBackgroundImages.SetGroupName(_T("Background Image"));
		m_bCombineBackgroundImages.SetIUIElementName(_T("IsCombineImages"));
		m_bCombineBackgroundImages.SetPropWndName(_T("Is Combine Bk Images"));

		m_strBKCombine = _T("");
		m_strBKCombine.vtEx = VT_EX_BITMAPPATH;
		m_strBKCombine.SetIUIElementName(_T("CombineBkImage"));
		m_strBKCombine.SetIUIAttributeName(_T("Value"));
		m_strBKCombine.SetPropWndName(_T("Combine Bk Image"));

		m_strBKImageN = _T("");
		m_strBKImageN.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageN.SetDefaultValue((_variant_t)m_strBKImageN);
		m_strBKImageN.SetVarName(_T("m_strBKImageN"));
		m_strBKImageN.SetIUIElementName(_T("BackgroundNormal"));
		m_strBKImageN.SetIUIAttributeName(_T("image"));
		m_strBKImageN.SetPropWndName(_T("Normal Bk Image"));

		m_strBKImageH = _T("");
		m_strBKImageH.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageH.SetDefaultValue((_variant_t)m_strBKImageH);
		m_strBKImageH.SetVarName(_T("m_strBKImageH"));
		m_strBKImageH.SetIUIElementName(_T("BackgroundHighlight"));
		m_strBKImageH.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageH.SetPropWndName(_T("Highlight Bk Image"));

		m_strBKImageP = _T("");
		m_strBKImageP.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageP.SetDefaultValue((_variant_t)m_strBKImageP);
		m_strBKImageP.SetVarName(_T("m_strBKImageP"));
		m_strBKImageP.SetIUIElementName(_T("BackgroundSelected"));
		m_strBKImageP.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageP.SetPropWndName(_T("Selected Bk Image"));

		m_strBKImageD = _T("");
		m_strBKImageD.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageD.SetDefaultValue((_variant_t)m_strBKImageD);
		m_strBKImageD.SetVarName(_T("m_strBKImageD"));
		m_strBKImageD.SetIUIElementName(_T("BackgroundDisabled"));
		m_strBKImageD.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageD.SetPropWndName(_T("Disabled Bk Image"));

		m_strBKImageCN = _T("");
		m_strBKImageCN.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageCN.SetDefaultValue((_variant_t)m_strBKImageCN);
		m_strBKImageCN.SetVarName(_T("m_strBKImageCN"));
		m_strBKImageCN.SetIUIElementName(_T("BackgroundCheckedNormal"));
		m_strBKImageCN.SetIUIAttributeName(_T("image"));
		m_strBKImageCN.SetPropWndName(_T("Checked Normal Bk Image"));

		m_strBKImageCH = _T("");
		m_strBKImageCH.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageCH.SetDefaultValue((_variant_t)m_strBKImageCH);
		m_strBKImageCH.SetVarName(_T("m_strBKImageCH"));
		m_strBKImageCH.SetIUIElementName(_T("BackgroundCheckedHighlight"));
		m_strBKImageCH.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageCH.SetPropWndName(_T("Checked Highlight Bk Image"));

		m_strBKImageCP = _T("");
		m_strBKImageCP.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageCP.SetDefaultValue((_variant_t)m_strBKImageCP);
		m_strBKImageCP.SetVarName(_T("m_strBKImageCP"));
		m_strBKImageCP.SetIUIElementName(_T("BackgroundCheckedSelected"));
		m_strBKImageCP.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageCP.SetPropWndName(_T("Checked Selected Bk Image"));

		m_strBKImageCD = _T("");
		m_strBKImageCD.vtEx = VT_EX_BITMAPPATH;
		m_strBKImageCD.SetDefaultValue((_variant_t)m_strBKImageCD);
		m_strBKImageCD.SetVarName(_T("m_strBKImageCD"));
		m_strBKImageCD.SetIUIElementName(_T("BackgroundCheckedDisabled"));
		m_strBKImageCD.SetIUIAttributeName(_T("RelativePath"));
		m_strBKImageCD.SetPropWndName(_T("Checked Disabled Bk Image"));

		m_lBkImgTransparent = (LONG)255;
		m_lBkImgTransparent.SetDefaultValue((_variant_t)m_lBkImgTransparent);
		m_lBkImgTransparent.SetIUIElementName(_T("BkImgTransparent"));
		m_lBkImgTransparent.SetPropWndName(_T("Bk Image Transparent"));
		m_lBkImgTransparent.nMinValue = 0;
		m_lBkImgTransparent.nMaxValue = 255;

		m_eBkImageResizeMode = IRM_9GRID;
		m_eBkImageResizeMode.SetDefaultValue((_variant_t)m_eBkImageResizeMode);
		m_eBkImageResizeMode.vtEx = VT_EX_IMAGE_RESIZE_MODE;
		m_eBkImageResizeMode.SetIUIElementName(_T("BkImageResizeMode"));
		m_eBkImageResizeMode.SetPropWndName(_T("Background Image Resize Mode"));
		m_eBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_9GRID).GetEnumString());
		m_eBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_STRETCH).GetEnumString());
		m_eBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_TILE).GetEnumString());
		m_eBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_CENTER).GetEnumString());
		m_eBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_KEEPRATE).GetEnumString());

		m_rcBkImage9GridResizeArg.vtEx = VT_EX_RECT;
		m_rcBkImage9GridResizeArg = rect;
		m_rcBkImage9GridResizeArg.SetDefaultValue((_variant_t)m_rcBkImage9GridResizeArg);
		m_rcBkImage9GridResizeArg.SetPropWndName(_T("9 Grid Parameter"));
		m_rcBkImage9GridResizeArg.SetIUIElementName(_T("NineGridParam"));
		m_rcBkImage9GridResizeArg.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcBkImage9GridResizeArg.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Border group.
		m_bDrawBorderColor = false;
		m_bDrawBorderColor.SetDefaultValue((_variant_t)m_bDrawBorderColor);
		m_bDrawBorderColor.bGroup = true;
		m_bDrawBorderColor.SetGroupName(_T("Border"));
		m_bDrawBorderColor.SetIUIElementName(_T("IsDrawBorderColor"));
		m_bDrawBorderColor.SetPropWndName(_T("Is Draw Border Color"));

		// Border group.
		m_strBorderColorResIDN = _T("");
		m_strBorderColorResIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDN.SetIUIElementName(_T("BorderColorResN"));
		m_strBorderColorResIDN.SetPropWndName(_T("Normal Border Color"));

		m_strBorderColorResIDH = _T("");
		m_strBorderColorResIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDH.SetIUIElementName(_T("BorderColorResH"));
		m_strBorderColorResIDH.SetPropWndName(_T("Hover Border Color"));

		m_strBorderColorResIDP = _T("");
		m_strBorderColorResIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDP.SetIUIElementName(_T("BorderColorResP"));
		m_strBorderColorResIDP.SetPropWndName(_T("Pressed Border Color"));

		m_strBorderColorResIDD = _T("");
		m_strBorderColorResIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDD.SetIUIElementName(_T("BorderColorResD"));
		m_strBorderColorResIDD.SetPropWndName(_T("Disabled Border Color"));

		m_strBorderColorResIDCN = _T("");
		m_strBorderColorResIDCN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDCN.SetIUIElementName(_T("BorderColorResCN"));
		m_strBorderColorResIDCN.SetPropWndName(_T("Checked Normal Border Color"));

		m_strBorderColorResIDCH = _T("");
		m_strBorderColorResIDCH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDCH.SetIUIElementName(_T("BorderColorResCH"));
		m_strBorderColorResIDCH.SetPropWndName(_T("Checked Hover Border Color"));

		m_strBorderColorResIDCP = _T("");
		m_strBorderColorResIDCP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDCP.SetIUIElementName(_T("BorderColorResCP"));
		m_strBorderColorResIDCP.SetPropWndName(_T("Checked Pressed Border Color"));

		m_strBorderColorResIDCD = _T("");
		m_strBorderColorResIDCD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strBorderColorResIDCD.SetIUIElementName(_T("BorderColorResCD"));
		m_strBorderColorResIDCD.SetPropWndName(_T("Checked Disabled Border Color"));

		POINT pt = {0, 0};
		m_ptBorderRound = pt;
		m_ptBorderRound.vtEx = VT_EX_POINT;
		m_ptBorderRound.SetDefaultValue((_variant_t)m_ptBorderRound);
		m_ptBorderRound.SetIUIElementName(TEXT("borderround"));
		m_ptBorderRound.SetPropWndName(TEXT("Border Round"));

		// Caption group
		m_strCaption = _T("");
		m_strCaption.SetDefaultValue((_variant_t)m_strCaption);
		m_strCaption.bGroup = true;
		m_strCaption.SetGroupName(_T("Caption"));
		m_strCaption.SetVarName(_T("m_strCaption"));
		m_strCaption.SetIUIElementName(_T("Content"));	// same to wpf
		m_strCaption.SetPropWndName(_T("Caption"));
		m_strCaption.SetDescription(_T("Specifies the caption of the control."));

		m_rcPadding4Text.vtEx = VT_EX_RECT;
		m_rcPadding4Text = rect;
		m_rcPadding4Text.SetDefaultValue((_variant_t)m_rcPadding4Text);
		m_rcPadding4Text.SetVarName(_T("m_rcPadding4Text"));
		m_rcPadding4Text.SetPropWndName(_T("Text Padding"));
		m_rcPadding4Text.SetIUIElementName(_T("TextPadding"));
		m_rcPadding4Text.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcPadding4Text.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Text Color Group
		m_strTextColorResIDN = _T("");
		m_strTextColorResIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDN.SetIUIElementName(_T("TextColorResN"));
		m_strTextColorResIDN.SetPropWndName(_T("Normal Text Color"));

		m_strTextColorResIDH = _T("");
		m_strTextColorResIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDH.SetIUIElementName(_T("TextColorResH"));
		m_strTextColorResIDH.SetPropWndName(_T("Hover Text Color"));

		m_strTextColorResIDP = _T("");
		m_strTextColorResIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDP.SetIUIElementName(_T("TextColorResP"));
		m_strTextColorResIDP.SetPropWndName(_T("Pressed Text Color"));

		m_strTextColorResIDD = _T("");
		m_strTextColorResIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDD.SetIUIElementName(_T("TextColorResD"));
		m_strTextColorResIDD.SetPropWndName(_T("Disabled Text Color"));

		m_strTextColorResIDCN = _T("");
		m_strTextColorResIDCN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDCN.SetIUIElementName(_T("TextColorResCN"));
		m_strTextColorResIDCN.SetPropWndName(_T("Checked Normal Text Color"));

		m_strTextColorResIDCH = _T("");
		m_strTextColorResIDCH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDCH.SetIUIElementName(_T("TextColorResCH"));
		m_strTextColorResIDCH.SetPropWndName(_T("Checked Hover Text Color"));

		m_strTextColorResIDCP = _T("");
		m_strTextColorResIDCP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDCP.SetIUIElementName(_T("TextColorResCP"));
		m_strTextColorResIDCP.SetPropWndName(_T("Checked Pressed Text Color"));

		m_strTextColorResIDCD = _T("");
		m_strTextColorResIDCD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDCD.SetIUIElementName(_T("TextColorResCD"));
		m_strTextColorResIDCD.SetPropWndName(_T("Checked Disabled Text Color"));

		// Font(Size) group.
		m_strResFontIDN = _T("");
		m_strResFontIDN.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDN.bGroup = true;
		m_strResFontIDN.SetGroupName(_T("Font(Size)"));
		m_strResFontIDN.SetIUIElementName(_T("FontResN"));
		m_strResFontIDN.SetPropWndName(_T("Normal Font"));

		m_strResFontIDH = _T("");
		m_strResFontIDH.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDH.SetIUIElementName(_T("FontResH"));
		m_strResFontIDH.SetPropWndName(_T("Highlight Font"));

		m_strResFontIDP = _T("");
		m_strResFontIDP.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDP.SetIUIElementName(_T("FontResS"));
		m_strResFontIDP.SetPropWndName(_T("Selected Font"));

		m_strResFontIDD = _T("");
		m_strResFontIDD.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDD.SetIUIElementName(_T("FontResD"));
		m_strResFontIDD.SetPropWndName(_T("Disabled Font"));

		// ToolTip group.
		m_bUseToolTip = false;
		m_bUseToolTip.SetDefaultValue((_variant_t)m_bUseToolTip);
		m_bUseToolTip.bGroup = true;
		m_bUseToolTip.SetVarName(_T("m_bUseToolTip"));
		m_bUseToolTip.SetGroupName(_T("ToolTip"));
		m_bUseToolTip.SetIUIElementName(_T("ToolTip"));
		m_bUseToolTip.SetIUIAttributeName(_T("Enable"));
		m_bUseToolTip.SetPropWndName(_T("Enable Tooltip"));
		m_bUseToolTip.SetDescription(_T("Enable Tooltip."));

		m_strToolTip = _T("");
		m_strToolTip.SetDefaultValue((_variant_t)m_strToolTip);
		m_strToolTip.SetIUIElementName(_T("tooltip"));
		m_strToolTip.SetIUIAttributeName(_T("ToolTip"));
		m_strToolTip.SetPropWndName(_T("ToolTip"));
		m_strToolTip.SetDescription(_T(""));

		m_strToolTipStyle = _T("");
		m_strToolTipStyle.SetDefaultValue((_variant_t)m_strToolTipStyle);
		m_strToolTipStyle.bReadOnly = true;
		m_strToolTipStyle.vtEx = VT_EX_STYLE_TOOLTIPS;
		m_strToolTipStyle.SetIUIElementName(_T("ToolTipsStyleID"));
		m_strToolTipStyle.SetPropWndName(_T("ToolTips Style"));
	}

	CControlProp::~CControlProp()
	{

	}

	BOOL CControlProp::IsControl() const
	{
		return TRUE;
	}

	int CControlProp::DisableBackgroundProperties(BOOL bKeepND, BOOL bDisabelChecked)
	{
		if (!bKeepND)
		{
			m_bCombineBackgroundImages.bSaveToSkinFile = m_bCombineBackgroundImages.bShowInPropertiesWnd = false;
			m_strBKCombine.bSaveToSkinFile = m_strBKCombine.bShowInPropertiesWnd = false;
			m_strBKImageN.bSaveToSkinFile = m_strBKImageN.bShowInPropertiesWnd = false;
			m_strBKImageD.bSaveToSkinFile = m_strBKImageD.bShowInPropertiesWnd = false;

			m_eBkImageResizeMode.bSaveToSkinFile = m_eBkImageResizeMode.bShowInPropertiesWnd = false;
			m_lBkImgTransparent.bSaveToSkinFile = m_lBkImgTransparent.bShowInPropertiesWnd = false;
		}

		m_strBKImageH.bSaveToSkinFile = m_strBKImageH.bShowInPropertiesWnd = false;
		m_strBKImageP.bSaveToSkinFile = m_strBKImageP.bShowInPropertiesWnd = false;

		if (bDisabelChecked)
		{
			m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = false;
			m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = false;
			m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = false;
			m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = false;
		}

		return 0;
	}

	int CControlProp::DisableCheckedBackgroundColorProperties()
	{
		m_strBkColorResIDCN.bSaveToSkinFile = m_strBkColorResIDCN.bShowInPropertiesWnd = false;
		m_strBkColorResIDCH.bSaveToSkinFile = m_strBkColorResIDCH.bShowInPropertiesWnd = false;
		m_strBkColorResIDCP.bSaveToSkinFile = m_strBkColorResIDCP.bShowInPropertiesWnd = false;
		m_strBkColorResIDCD.bSaveToSkinFile = m_strBkColorResIDCD.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableCheckedBackgroundImageProperties()
	{
		m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = false;
		m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = false;
		m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = false;
		m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableFontProperties(BOOL bKeepND)
	{
		if (!bKeepND)
		{
			m_strResFontIDN.bSaveToSkinFile = m_strResFontIDN.bShowInPropertiesWnd = false;
			m_strResFontIDD.bSaveToSkinFile = m_strResFontIDD.bShowInPropertiesWnd = false;
		}

		m_strResFontIDH.bSaveToSkinFile = m_strResFontIDH.bShowInPropertiesWnd = false;
		m_strResFontIDP.bSaveToSkinFile = m_strResFontIDP.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableTextColorProperties(BOOL bKeepND)
	{
		if (!bKeepND)
		{
			m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = false;
			m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = false;
		}

		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = false;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableBkColorProperties(BOOL bKeepND, BOOL bDisabelChecked)
	{
		if (!bKeepND)
		{
			m_bDrawBkColor.bSaveToSkinFile = m_bDrawBkColor.bShowInPropertiesWnd = false;
			m_strBkColorResIDN.bSaveToSkinFile = m_strBkColorResIDN.bShowInPropertiesWnd = false;
			m_strBkColorResIDD.bSaveToSkinFile = m_strBkColorResIDD.bShowInPropertiesWnd = false;
		}

		m_strBkColorResIDH.bSaveToSkinFile = m_strBkColorResIDH.bShowInPropertiesWnd = false;
		m_strBkColorResIDP.bSaveToSkinFile = m_strBkColorResIDP.bShowInPropertiesWnd = false;

		if (bDisabelChecked)
		{
			m_strBkColorResIDCN.bSaveToSkinFile = m_strBkColorResIDCN.bShowInPropertiesWnd = false;
			m_strBkColorResIDCH.bSaveToSkinFile = m_strBkColorResIDCH.bShowInPropertiesWnd = false;
			m_strBkColorResIDCP.bSaveToSkinFile = m_strBkColorResIDCP.bShowInPropertiesWnd = false;
			m_strBkColorResIDCD.bSaveToSkinFile = m_strBkColorResIDCD.bShowInPropertiesWnd = false;
		}

		return 0;
	}

	int CControlProp::DisableBorderColorGroup(BOOL bKeepND, BOOL bDisabelChecked)
	{
		if (!bKeepND)
		{
			m_bDrawBorderColor.bSaveToSkinFile = m_bDrawBorderColor.bShowInPropertiesWnd = false;
			m_strBorderColorResIDN.bSaveToSkinFile = m_strBorderColorResIDN.bShowInPropertiesWnd = false;
			m_strBorderColorResIDD.bSaveToSkinFile = m_strBorderColorResIDD.bShowInPropertiesWnd = false;
			m_ptBorderRound.bSaveToSkinFile = m_ptBorderRound.bShowInPropertiesWnd = false;
		}

		m_strBorderColorResIDH.bSaveToSkinFile = m_strBorderColorResIDH.bShowInPropertiesWnd = false;
		m_strBorderColorResIDP.bSaveToSkinFile = m_strBorderColorResIDP.bShowInPropertiesWnd = false;

		if (bDisabelChecked)
		{
			DisableCheckedBorderColorProperties();
		}

		return 0;
	}

	int CControlProp::DisableCheckedBorderColorProperties()
	{
		m_strBorderColorResIDCN.bSaveToSkinFile = m_strBorderColorResIDCN.bShowInPropertiesWnd = false;
		m_strBorderColorResIDCH.bSaveToSkinFile = m_strBorderColorResIDCH.bShowInPropertiesWnd = false;
		m_strBorderColorResIDCP.bSaveToSkinFile = m_strBorderColorResIDCP.bShowInPropertiesWnd = false;
		m_strBorderColorResIDCD.bSaveToSkinFile = m_strBorderColorResIDCD.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableCaptionGroup()
	{
		m_strCaption.bSaveToSkinFile = m_strCaption.bShowInPropertiesWnd = false;
		m_rcPadding4Text.bSaveToSkinFile = m_rcPadding4Text.bShowInPropertiesWnd = false;
		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = false;
		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = false;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = false;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = false;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = false;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = false;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = false;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::DisableTooltipsGroup()
	{
		m_bUseToolTip.bSaveToSkinFile = m_bUseToolTip.bShowInPropertiesWnd = false;
		m_strToolTip.bSaveToSkinFile = m_strToolTip.bShowInPropertiesWnd = false;
		m_strToolTipStyle.bSaveToSkinFile = m_strToolTipStyle.bShowInPropertiesWnd = false;

		return 0;
	}

	int CControlProp::GetControlFont1(const CControlProp *pCtrlProp, CIUIString *pstrFontResID)
	{
		if (pCtrlProp == NULL || pstrFontResID == NULL)
		{
			return -1;
		}

		*pstrFontResID = (LPCTSTR)pCtrlProp->m_strResFontIDN;

		return 0;
	}

	int CControlProp::GetControlFont2(const CControlProp *pCtrlProp, CIUIString *pstrFontResID)
	{
		if (pCtrlProp == NULL || pstrFontResID == NULL)
		{
			return -1;
		}

		int nIndex = 0;
		*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;

		CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strResFontIDD;
		if (strIDD.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDD;
		}

		return 0;
	}

	int CControlProp::GetControlFont4(const CControlProp *pCtrlProp, CIUIString *pstrFontResID)
	{
		if (pCtrlProp == NULL || pstrFontResID == NULL)
		{
			return -1;
		}

		int nIndex = 0;
		*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;

		CIUIString strIDH = (LPCTSTR)pCtrlProp->m_strResFontIDH;
		if (strIDH.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDH;
		}

		CIUIString strIDS = (LPCTSTR)pCtrlProp->m_strResFontIDP;
		if (strIDS.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDS;
		}

		CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strResFontIDD;
		if (strIDD.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDD;
		}

		return 0;
	}

	int CControlProp::GetControlFont8(const CToggleButtonProp *pCtrlProp,
		CIUIString *pstrFontResID)
	{
		if (pCtrlProp == NULL || pstrFontResID == NULL)
		{
			return -1;
		}

		int nIndex = 0;
		*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;

		CIUIString strIDH = (LPCTSTR)pCtrlProp->m_strResFontIDH;
		if (strIDH.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDH;
		}

		CIUIString strIDS = (LPCTSTR)pCtrlProp->m_strResFontIDP;
		if (strIDS.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDS;
		}

		CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strResFontIDD;
		if (strIDD.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDD;
		}

		// Checked state
		CIUIString strIDCN = (LPCTSTR)pCtrlProp->m_strResFontIDCN;
		if (strIDCN.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDCN;
		}

		CIUIString strIDCH = (LPCTSTR)pCtrlProp->m_strResFontIDCH;
		if (strIDCH.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDCH;
		}

		CIUIString strIDCS = (LPCTSTR)pCtrlProp->m_strResFontIDCS;
		if (strIDCS.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDCS;
		}

		CIUIString strIDCD = (LPCTSTR)pCtrlProp->m_strResFontIDCD;
		if (strIDCD.IsEmpty())
		{
			*(pstrFontResID + nIndex++) = (LPCTSTR)pCtrlProp->m_strResFontIDN;
		}
		else
		{
			*(pstrFontResID + nIndex++) = strIDCD;
		}

		return 0;
	}

	int _InitOtherStateColor(COLORREF *pColor, bool bSameNColor, COLORREF crNormal, LPCTSTR lpszColorID)
	{
		if (pColor == NULL)
		{
			return -1;
		}

		if (bSameNColor)
		{
			*pColor = crNormal;
		}
		else
		{
			if (lpszColorID == NULL)
			{
				return -2;
			}

			// 			CColorProp *pColorProp = (CColorProp *)CUIMgr::GetColorResourceItem(
			// 					lpszColorID);
			//
			// 			*pColor = (pColorProp == NULL) ? crNormal : pColorProp->m_crColor;
		}

		return 0;
	}

	int CControlProp::GetBackground2(const CControlProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineBackgroundImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strBKCombine;

			//CreateEmptyIUIImage((LPCTSTR)pCtrlProp->m_strBKCombine);
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageD;
		}

		return 0;
	}

	int CControlProp::GetBackground4(const CControlProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineBackgroundImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strBKCombine;
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageD;
		}

		return 0;
	}

	int CControlProp::GetToggleButtonBackground8(const CToggleButtonProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineBackgroundImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strBKCombine;
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageD;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageCN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageCH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageCP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strBKImageCD;
		}

		return 0;
	}

	int CControlProp::GetToggleButtonForeground8(const CToggleButtonProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		bool bSpecify = pCtrlProp->m_bSpecifyForegroundImages;
		if (!bSpecify)
		{
			return -1;
		}

		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineForegroundImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strFGCombine;
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageD;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageCN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageCH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageCP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageCD;
		}

		return 0;
	}

	int CControlProp::GetButtonForeground4(const CButtonBaseProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineForegroundImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strFGCombine;
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strFGImageD;
		}

		return 0;
	}

	//int CControlProp::GetThumbImages4(const HORSBPROPERTIES *pCtrlProp,
	//	BOOL *pbCombineImage, CIUIString *pstrImageName)
	//{
	//	if (pbCombineImage == NULL)
	//	{
	//		return -1;
	//	}

	//	bool bCombineImage = pCtrlProp->m_bCombineThumbImages;
	//	*pbCombineImage = bCombineImage;

	//	if (bCombineImage)
	//	{
	//		*pstrImageName = (LPCTSTR)pCtrlProp->m_strThumbCombine;
	//	}
	//	else
	//	{
	//		int nIndex = 1;

	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbN;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbH;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbS;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbD;
	//	}

	//	return 0;
	//}

	//int CControlProp::GetThumbImages4(const VERSBPROPERTIES *pCtrlProp,
	//	BOOL *pbCombineImage, CIUIString *pstrImageName)
	//{
	//	if (pbCombineImage == NULL)
	//	{
	//		return -1;
	//	}

	//	bool bCombineImage = pCtrlProp->m_bCombineThumbImages;
	//	*pbCombineImage = bCombineImage;

	//	if (bCombineImage)
	//	{
	//		*pstrImageName = (LPCTSTR)pCtrlProp->m_strThumbCombine;
	//	}
	//	else
	//	{
	//		int nIndex = 1;

	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbN;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbH;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbS;
	//		*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbD;
	//	}

	//	return 0;
	//}

	int CControlProp::GetThumbImages4(const CSliderProp *pCtrlProp,
		BOOL *pbCombineImage, CIUIString *pstrImageName)
	{
		if (pbCombineImage == NULL)
		{
			return -1;
		}

		bool bCombineImage = pCtrlProp->m_bCombineThumbImages;
		*pbCombineImage = bCombineImage;

		if (bCombineImage)
		{
			*pstrImageName = (LPCTSTR)pCtrlProp->m_strThumbCombine;
		}
		else
		{
			int nIndex = 1;

			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbN;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbH;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbP;
			*(pstrImageName + (nIndex++)) = (LPCTSTR)pCtrlProp->m_strThumbD;
		}

		return 0;
	}

	CIUIStringW CControlProp::GetLanguageKey(
		const CIUIProject *pProject,
		const class CControlProp *pCtrlProp,
		BOOL *pbFound)
	{
		CIUIString strRet;

		const CControlProp *pParent = pCtrlProp;
		while (NULL != pParent)
		{
			if (strRet.IsEmpty())
			{
				strRet = (LPCTSTR)pParent->m_strName;
			}
			else
			{
				CIUIString strInsert = (LPCTSTR)pParent->m_strName;
				strInsert += _T("|");
				strRet.Insert(0, strInsert);
			}

			pParent = (CControlProp *)pParent->GetParent();
		}

		return pProject->FindControlContent(strRet, pbFound);
	}

	//////////////////////////////////////////////////////////////////////////

	CLabelProp::CLabelProp()
	{
		m_lStructSize = (LONG)sizeof(CLabelProp);
		m_eControlType = CT_STATIC;

		m_strBkColorResIDH.bSaveToSkinFile = m_strBkColorResIDH.bShowInPropertiesWnd = false;
		m_strBkColorResIDP.bSaveToSkinFile = m_strBkColorResIDP.bShowInPropertiesWnd = false;

		m_strBKImageH.bShowInPropertiesWnd = m_strBKImageH.bSaveToSkinFile = false;
		m_strBKImageP.bShowInPropertiesWnd = m_strBKImageP.bSaveToSkinFile = false;

		m_strBkColorResIDCN.bSaveToSkinFile = m_strBkColorResIDCN.bShowInPropertiesWnd = false;
		m_strBkColorResIDCH.bSaveToSkinFile = m_strBkColorResIDCH.bShowInPropertiesWnd = false;
		m_strBkColorResIDCP.bSaveToSkinFile = m_strBkColorResIDCP.bShowInPropertiesWnd = false;
		m_strBkColorResIDCD.bSaveToSkinFile = m_strBkColorResIDCD.bShowInPropertiesWnd = false;

		m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = false;
		m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = false;
		m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = false;
		m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = false;

		DisableBorderColorGroup(TRUE, TRUE);

		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = true;
		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = false;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = false;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = true;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = false;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = false;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = false;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = false;

		// font
		DisableFontProperties(TRUE);

		// color
		DisableTextColorProperties(TRUE);

		m_strButtonClickArg = _T("");
		m_strButtonClickArg.SetIUIElementName(_T("ButtonClickArg"));
		m_strButtonClickArg.SetPropWndName(_T("Button Click Arg"));
		m_strButtonClickArg.bGroup = true;
		m_strButtonClickArg.SetGroupName(_T("General"));

		// alignment
		m_eTextHorAlignMode = TAH_LEFT;
		m_eTextHorAlignMode.SetDefaultValue((_variant_t)m_eTextHorAlignMode);
		m_eTextHorAlignMode.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignMode.bReadOnly = true;
		m_eTextHorAlignMode.bGroup = true;
		m_eTextHorAlignMode.SetGroupName(_T("Caption"));
		m_eTextHorAlignMode.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignMode.SetIUIAttributeName(_T("Horizontal"));
		m_eTextHorAlignMode.SetPropWndName(_T("Caption Horizontal Alignment"));
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());

		m_eTextVerAlignMode = TAV_CENTER;
		m_eTextVerAlignMode.SetDefaultValue((_variant_t)m_eTextVerAlignMode);
		m_eTextVerAlignMode.vtEx = VT_EX_TEXT_ALIGN_VER;
		m_eTextVerAlignMode.bReadOnly = true;
		m_eTextVerAlignMode.SetIUIElementName(_T("TextAlignment"));
		m_eTextVerAlignMode.SetIUIAttributeName(_T("Vertical"));
		m_eTextVerAlignMode.SetPropWndName(_T("Caption Vertical Alignment"));
		m_eTextVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_TOP).GetEnumString());
		m_eTextVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_BOTTOM).GetEnumString());
		m_eTextVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_CENTER).GetEnumString());

		m_bR2LReadOrder = false;
		m_bR2LReadOrder.SetDefaultValue((_variant_t)m_bR2LReadOrder);
		m_bR2LReadOrder.SetVarName(_T("m_bR2LReadOrder"));
		m_bR2LReadOrder.SetIUIElementName(_T("IsR2LReadOrder"));
		m_bR2LReadOrder.SetPropWndName(_T("Right to Left Reading Order"));
		m_bR2LReadOrder.SetDescription(_T("For languages that support reading order alignment, specifies right-to-left reading order."));

		// Caption group.
		m_bMultiline = false;
		m_bMultiline.SetDefaultValue((_variant_t)m_bMultiline);
		m_bMultiline.SetVarName(_T("m_bMultiline"));
		m_bMultiline.SetIUIElementName(_T("IsMultiline"));
		m_bMultiline.SetPropWndName(_T("Multiline"));
		m_bMultiline.SetDescription(_T("Wraps control text to multiple lines if text is too long for control width."));

		m_bPathEllipsis = false;
		m_bPathEllipsis.SetDefaultValue((_variant_t)m_bPathEllipsis);
		m_bPathEllipsis.SetIUIElementName(_T("IsPathEllipsis"));
		m_bPathEllipsis.SetPropWndName(_T("Path Ellipsis"));
		m_bPathEllipsis.SetDescription(_T(""));

		m_bEndEllipsis = false;
		m_bEndEllipsis.SetDefaultValue((_variant_t)m_bEndEllipsis);
		m_bEndEllipsis.SetIUIElementName(_T("IsEndEllipsis"));
		m_bEndEllipsis.SetPropWndName(_T("End Ellipsis"));
		m_bEndEllipsis.SetDescription(_T(""));

		m_bShadowText = false;
		m_bShadowText.SetDefaultValue((_variant_t)m_bShadowText);
		m_bShadowText.bGroup = true;
		m_bShadowText.SetVarName(_T("m_bShadowText"));
		m_bShadowText.SetGroupName(_T("Shadow Text"));
		m_bShadowText.SetIUIElementName(_T("ShadowText"));
		m_bShadowText.SetIUIAttributeName(_T("Enable"));
		m_bShadowText.SetPropWndName(_T("Enable Shadow Text"));
		m_bShadowText.SetDescription(_T("Enable Shadow Text."));

		m_strTextShadowColorResID = _T("");
		m_strTextShadowColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextShadowColorResID.SetIUIElementName(m_bShadowText.GetIUIElementName());
		m_strTextShadowColorResID.SetIUIAttributeName(_T("ShadowColorRes"));
		m_strTextShadowColorResID.SetPropWndName(_T("Text Shadow Color"));

		m_ptTextShadowOffset = CIUIPoint(1, 1);
		m_ptTextShadowOffset.SetDefaultValue((_variant_t)m_ptTextShadowOffset);
		m_ptTextShadowOffset.vtEx = VT_EX_POINT;
		m_ptTextShadowOffset.SetIUIElementName(m_bShadowText.GetIUIElementName());
		m_ptTextShadowOffset.SetIUIAttributeName(_T("ShadowOffset"));
		m_ptTextShadowOffset.SetPropWndName(_T("Text Shadow offset"));
		m_ptTextShadowOffset.nMinValue = -1;
		m_ptTextShadowOffset.nMaxValue = 8;
	}

	CLabelProp::~CLabelProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CButtonBaseProp::CButtonBaseProp()
	{
		m_lStructSize = (LONG)sizeof(CButtonBaseProp);

		m_strFGImageH.bSaveToSkinFile = m_strFGImageH.bShowInPropertiesWnd = true;
		m_strFGImageP.bSaveToSkinFile = m_strFGImageP.bShowInPropertiesWnd = true;

		// Background group.
		m_strMaskBmp = _T("");
		m_strMaskBmp.vtEx = VT_EX_BITMAPPATH;
		m_strMaskBmp.bGroup = true;
		m_strMaskBmp.SetGroupName(_T("Background"));
		m_strMaskBmp.SetIUIElementName(_T("MaskBitmap"));
		m_strMaskBmp.SetIUIAttributeName(_T("RelativePath"));
		m_strMaskBmp.SetPropWndName(_T("Mask Bitmap"));
		m_strMaskBmp.SetDescription(_T(""));

		// Foreground group.
		m_bSpecifyForegroundImages = false;
		m_bSpecifyForegroundImages.SetDefaultValue((_variant_t)m_bSpecifyForegroundImages);
		m_bSpecifyForegroundImages.bGroup = true;
		m_bSpecifyForegroundImages.SetGroupName(_T("Foreground"));
		m_bSpecifyForegroundImages.SetIUIElementName(_T("IsSpecifyForegroundImages"));
		m_bSpecifyForegroundImages.SetPropWndName(_T("Is Specify Foreground Images"));

		m_bCombineForegroundImages = false;
		m_bCombineForegroundImages.SetDefaultValue((_variant_t)m_bCombineForegroundImages);
		m_bCombineForegroundImages.SetIUIElementName(_T("IsCombineForegroundImages"));
		m_bCombineForegroundImages.SetPropWndName(_T("Is Combine Foreground Images"));

		m_strFGCombine = _T("");
		m_strFGCombine.vtEx = VT_EX_BITMAPPATH;
		m_strFGCombine.SetIUIElementName(_T("ForeGroundCombine"));
		m_strFGCombine.SetPropWndName(_T("Combine Foreground Image"));

		m_strFGImageN = _T("");
		m_strFGImageN.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageN.SetIUIElementName(_T("ForegroundNormal"));
		m_strFGImageN.SetPropWndName(_T("Normal Foreground Image"));

		m_strFGImageH = _T("");
		m_strFGImageH.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageH.SetIUIElementName(_T("ForegroundHighlight"));
		m_strFGImageH.SetPropWndName(_T("Highlight Foreground Image"));

		m_strFGImageP = _T("");
		m_strFGImageP.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageP.SetIUIElementName(_T("ForegroundSelected"));
		m_strFGImageP.SetPropWndName(_T("Selected Foreground Image"));

		m_strFGImageD = _T("");
		m_strFGImageD.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageD.SetIUIElementName(_T("ForegroundDisabled"));
		m_strFGImageD.SetPropWndName(_T("Disabled Foreground Image"));

		m_strFGImageCN = _T("");
		m_strFGImageCN.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageCN.SetIUIElementName(_T("ForegroundCheckedNormal"));
		m_strFGImageCN.SetIUIAttributeName(_T("RelativePath"));
		m_strFGImageCN.SetPropWndName(_T("Checked Normal Image"));

		m_strFGImageCH = _T("");
		m_strFGImageCH.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageCH.SetIUIElementName(_T("ForegroundCheckedHighlight"));
		m_strFGImageCH.SetIUIAttributeName(_T("RelativePath"));
		m_strFGImageCH.SetPropWndName(_T("Checked Highlight Image"));

		m_strFGImageCP = _T("");
		m_strFGImageCP.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageCP.SetIUIElementName(_T("ForegroundCheckedSelected"));
		m_strFGImageCP.SetIUIAttributeName(_T("RelativePath"));
		m_strFGImageCP.SetPropWndName(_T("Checked Selected Image"));

		m_strFGImageCD = _T("");
		m_strFGImageCD.vtEx = VT_EX_BITMAPPATH;
		m_strFGImageCD.SetIUIElementName(_T("ForegroundCheckedDisabled"));
		m_strFGImageCD.SetIUIAttributeName(_T("RelativePath"));
		m_strFGImageCD.SetPropWndName(_T("Checked Disabled Image"));

		m_eForegroundHorAlignMode = FAH_LEFT;
		m_eForegroundHorAlignMode.SetDefaultValue((_variant_t)m_eForegroundHorAlignMode);
		m_eForegroundHorAlignMode.vtEx = VT_EX_FOREGROUND_ALIGN_HOR;
		m_eForegroundHorAlignMode.bReadOnly = true;
		m_eForegroundHorAlignMode.SetIUIElementName(_T("ForegroundAlignMode"));
		m_eForegroundHorAlignMode.SetIUIAttributeName(_T("Horizontal"));
		m_eForegroundHorAlignMode.SetPropWndName(_T("Foregournd Horizontal Alignment"));
		m_eForegroundHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_LEFT).GetEnumString());
		m_eForegroundHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_RIGHT).GetEnumString());
		m_eForegroundHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_CENTER).GetEnumString());
		m_eForegroundHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_STRETCH).GetEnumString());

		m_eForegroundVerAlignMode = FAV_CENTER;
		m_eForegroundVerAlignMode.SetDefaultValue((_variant_t)m_eForegroundVerAlignMode);
		m_eForegroundVerAlignMode.vtEx = VT_EX_FOREGROUND_ALIGN_VER;
		m_eForegroundVerAlignMode.bReadOnly = true;
		m_eForegroundVerAlignMode.SetIUIElementName(_T("ForegroundAlignMode"));
		m_eForegroundVerAlignMode.SetIUIAttributeName(_T("Vertical"));
		m_eForegroundVerAlignMode.SetPropWndName(_T("Foregournd Vertical Alignment"));
		m_eForegroundVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_TOP).GetEnumString());
		m_eForegroundVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_BOTTOM).GetEnumString());
		m_eForegroundVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_CENTER).GetEnumString());
		m_eForegroundVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_STRETCH).GetEnumString());

		m_rcPadding4Foreground = CIUIRect(0, 0, 0, 0);
		m_rcPadding4Foreground.vtEx = VT_EX_RECT;
		m_rcPadding4Foreground.SetDefaultValue((_variant_t)m_rcPadding4Foreground);
		m_rcPadding4Foreground.SetPropWndName(_T("Foreground Margin"));
		m_rcPadding4Foreground.SetIUIElementName(_T("ForegroundMargin"));
		m_rcPadding4Foreground.nMinValue = -3840;
		m_rcPadding4Foreground.nMaxValue = 3840;

		// ToolTip group.
		m_lAutoPopDelayTime = (LONG)5000;    // GetDoubleClickTime() * 10
		m_lAutoPopDelayTime.SetDefaultValue((_variant_t)m_lAutoPopDelayTime);
		m_lAutoPopDelayTime.bGroup = true;
		m_lAutoPopDelayTime.SetGroupName(_T("ToolTip"));
		m_lAutoPopDelayTime.SetIUIElementName(_T("ToolTipAutoPopDelayTime"));
		m_lAutoPopDelayTime.SetPropWndName(_T("ToolTip Auto Pop Delay Time"));
		m_lAutoPopDelayTime.SetDescription(_T("See MSDN: CToolTipCtrl::GetDelayTime and TTM_SETDELAYTIME"));
		m_lAutoPopDelayTime.nMaxValue = 0;    //
		m_lAutoPopDelayTime.nMaxValue = MAXINT;

		m_lInitialDelayTime = (LONG)500;    // GetDoubleClickTime()
		m_lInitialDelayTime.SetDefaultValue((_variant_t)m_lInitialDelayTime);
		m_lInitialDelayTime.SetIUIElementName(_T("ToolTipInitialDelayTime"));
		m_lInitialDelayTime.SetPropWndName(_T("ToolTip Initial Delay Time"));
		m_lInitialDelayTime.SetDescription(_T("See MSDN: CToolTipCtrl::GetDelayTime and TTM_SETDELAYTIME"));
		m_lInitialDelayTime.nMaxValue = 0;    //
		m_lInitialDelayTime.nMaxValue = MAXINT;

		m_lReshowDelayTime = (LONG)100;    // GetDoubleClickTime() / 5
		m_lReshowDelayTime.SetDefaultValue((_variant_t)m_lReshowDelayTime);
		m_lReshowDelayTime.SetIUIElementName(_T("ToolTipReshowDelayTime"));
		m_lReshowDelayTime.SetPropWndName(_T("ToolTip Reshow Delay Time"));
		m_lReshowDelayTime.SetDescription(_T("See MSDN: CToolTipCtrl::GetDelayTime and TTM_SETDELAYTIME"));
		m_lReshowDelayTime.nMaxValue = 0;    //
		m_lReshowDelayTime.nMaxValue = MAXINT;

		// Misc group.
		m_strModalDlgID = _T("");
		m_strModalDlgID.SetDefaultValue((_variant_t)m_strModalDlgID);
		m_strModalDlgID.bGroup = true;
		m_strModalDlgID.SetGroupName(_T("Misc"));
		m_strModalDlgID.vtEx = VT_EX_WINDOW_ID;
		m_strModalDlgID.SetIUIElementName(_T("ModalDlgID"));
		m_strModalDlgID.SetPropWndName(_T("Popup modal dialog ID"));

		m_strModelessWndID = _T("");
		m_strModelessWndID.SetDefaultValue((_variant_t)m_strModelessWndID);
		m_strModelessWndID.vtEx = VT_EX_WINDOW_ID;
		m_strModelessWndID.SetIUIElementName(_T("ModelessWndID"));
		m_strModelessWndID.SetPropWndName(_T("Popup modeless window ID"));
	}

	CButtonBaseProp::~CButtonBaseProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CButtonProp::CButtonProp()
	{
		m_lStructSize = (LONG)sizeof(CButtonProp);
		m_eControlType = CT_PUSHBUTTON;

		m_strBkColorResIDN.bSaveToSkinFile = m_strBkColorResIDN.bShowInPropertiesWnd = true;
		m_strBkColorResIDH.bSaveToSkinFile = m_strBkColorResIDH.bShowInPropertiesWnd = true;
		m_strBkColorResIDP.bSaveToSkinFile = m_strBkColorResIDP.bShowInPropertiesWnd = true;
		m_strBkColorResIDD.bSaveToSkinFile = m_strBkColorResIDD.bShowInPropertiesWnd = true;

		m_bDrawBorderColor.bSaveToSkinFile = m_bDrawBorderColor.bShowInPropertiesWnd = true;

		DisableCheckedBorderColorProperties();

		m_strBKImageN.bSaveToSkinFile = m_strBKImageN.bShowInPropertiesWnd = true;
		m_strBKImageH.bSaveToSkinFile = m_strBKImageH.bShowInPropertiesWnd = true;
		m_strBKImageP.bSaveToSkinFile = m_strBKImageP.bShowInPropertiesWnd = true;
		m_strBKImageD.bSaveToSkinFile = m_strBKImageD.bShowInPropertiesWnd = true;

		m_bDrawBkColor = true;
		m_bDrawBkColor.SetDefaultValue((_variant_t)m_bDrawBkColor);
		m_bDrawBorderColor = true;
		m_bDrawBorderColor.SetDefaultValue((_variant_t)m_bDrawBorderColor);

		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = true;
		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = true;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = true;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = true;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = false;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = false;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = false;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = false;

		m_strResFontIDH.bSaveToSkinFile = m_strResFontIDH.bShowInPropertiesWnd = true;
		m_strResFontIDP.bSaveToSkinFile = m_strResFontIDP.bShowInPropertiesWnd = true;
		m_strResFontIDD.bSaveToSkinFile = m_strResFontIDD.bShowInPropertiesWnd = true;

		m_strFGImageCN.bSaveToSkinFile = m_strFGImageCN.bShowInPropertiesWnd = false;
		m_strFGImageCH.bSaveToSkinFile = m_strFGImageCH.bShowInPropertiesWnd = false;
		m_strFGImageCP.bSaveToSkinFile = m_strFGImageCP.bShowInPropertiesWnd = false;
		m_strFGImageCD.bSaveToSkinFile = m_strFGImageCD.bShowInPropertiesWnd = false;

		m_eBtnType = BT_NORMAL;
		m_eBtnType.SetDefaultValue((_variant_t)m_eBtnType);
		m_eBtnType.SetVarName(_T("m_eBtnType"));
		m_eBtnType.vtEx = VT_EX_BUTTON_TYPE;
		m_eBtnType.bReadOnly = true;
		m_eBtnType.bGroup = true;
		m_eBtnType.SetGroupName(_T("General"));
		m_eBtnType.SetIUIElementName(_T("ButtonType"));
		m_eBtnType.SetPropWndName(_T("Button Type"));
		m_eBtnType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BT_NORMAL).GetEnumString());
		m_eBtnType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BT_MINIMIZEBOX).GetEnumString());
		m_eBtnType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BT_CLOSEBOX).GetEnumString());

		m_bDefPushButton = false;
		m_bDefPushButton.SetDefaultValue((_variant_t)m_bDefPushButton);
		m_bDefPushButton.SetIUIElementName(_T("IsDefPushButton"));
		m_bDefPushButton.SetPropWndName(_T("Default Button"));
	}

	CButtonProp::~CButtonProp()
	{

	}

	CToggleButtonProp::CToggleButtonProp()
	{
		m_lStructSize = (LONG)sizeof(CToggleButtonProp);

		m_bDrawBkColor = false;
		m_bDrawBkColor.SetDefaultValue((_variant_t)m_bDrawBkColor);
		m_bDrawBorderColor = false;
		m_bDrawBorderColor.SetDefaultValue((_variant_t)m_bDrawBorderColor);

		m_strBkColorResIDCN.bSaveToSkinFile = m_strBkColorResIDCN.bShowInPropertiesWnd = true;
		m_strBkColorResIDCH.bSaveToSkinFile = m_strBkColorResIDCH.bShowInPropertiesWnd = true;
		m_strBkColorResIDCP.bSaveToSkinFile = m_strBkColorResIDCP.bShowInPropertiesWnd = true;
		m_strBkColorResIDCD.bSaveToSkinFile = m_strBkColorResIDCD.bShowInPropertiesWnd = true;
		m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = true;
		m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = true;
		m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = true;
		m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = true;

		m_bDrawBorderColor.bSaveToSkinFile = m_bDrawBorderColor.bShowInPropertiesWnd = true;
		m_strBorderColorResIDN.bSaveToSkinFile = m_strBorderColorResIDN.bShowInPropertiesWnd = true;
		m_strBorderColorResIDH.bSaveToSkinFile = m_strBorderColorResIDH.bShowInPropertiesWnd = true;
		m_strBorderColorResIDP.bSaveToSkinFile = m_strBorderColorResIDP.bShowInPropertiesWnd = true;
		m_strBorderColorResIDD.bSaveToSkinFile = m_strBorderColorResIDD.bShowInPropertiesWnd = true;
		m_strBorderColorResIDCN.bSaveToSkinFile = m_strBorderColorResIDCN.bShowInPropertiesWnd = true;
		m_strBorderColorResIDCH.bSaveToSkinFile = m_strBorderColorResIDCH.bShowInPropertiesWnd = true;
		m_strBorderColorResIDCP.bSaveToSkinFile = m_strBorderColorResIDCP.bShowInPropertiesWnd = true;
		m_strBorderColorResIDCD.bSaveToSkinFile = m_strBorderColorResIDCD.bShowInPropertiesWnd = true;

		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = true;
		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = true;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = true;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = true;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = true;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = true;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = true;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = true;

		m_strFGImageCN.bSaveToSkinFile = m_strFGImageCN.bShowInPropertiesWnd = true;
		m_strFGImageCH.bSaveToSkinFile = m_strFGImageCH.bShowInPropertiesWnd = true;
		m_strFGImageCP.bSaveToSkinFile = m_strFGImageCP.bShowInPropertiesWnd = true;
		m_strFGImageCD.bSaveToSkinFile = m_strFGImageCD.bShowInPropertiesWnd = true;

		m_eBtnType.bSaveToSkinFile = m_eBtnType.bShowInPropertiesWnd = false;

		m_strResFontIDCN.bGroup = true;
		m_strResFontIDCN.SetGroupName(m_strResFontIDN.GetGroupName());
		CControlBaseProp::InitResFontIDVariant(&m_strResFontIDCN, _T("FontResCN"), _T("Checked Normal Font"));
		CControlBaseProp::InitResFontIDVariant(&m_strResFontIDCH, _T("FontResCH"), _T("Checked Highlight Font"));
		CControlBaseProp::InitResFontIDVariant(&m_strResFontIDCS, _T("FontResCS"), _T("Checked Selected Font"));
		CControlBaseProp::InitResFontIDVariant(&m_strResFontIDCD, _T("FontResCD"), _T("Checked Disabled Font"));

		CControlBaseProp::InitBoolVariant(&m_bCNSameNColor, true, _T("IsCNSameNColor"), _T("Checked Normal Color Same as Normal"), true, m_strTextColorResIDN.GetGroupName());
		CControlBaseProp::InitResColorIDVariant(&m_strResColorIDCN, _T("ColorResCN"), _T("Checked Normal"));

		CControlBaseProp::InitBoolVariant(&m_bCHSameNColor, true, _T("IsCHSameNColor"), _T("Checked Highlight Color Same as Normal"));
		CControlBaseProp::InitResColorIDVariant(&m_strResColorIDCH, _T("ColorResCH"), _T("Checked Highlight"));

		CControlBaseProp::InitBoolVariant(&m_bCSSameNColor, true, _T("IsCSSameNColor"), _T("Checked Selected Color Same as Normal"));
		CControlBaseProp::InitResColorIDVariant(&m_strResColorIDCS, _T("ColorResCS"), _T("Checked Selected"));

		CControlBaseProp::InitBoolVariant(&m_bCDSameNColor, true, _T("IsCDSameNColor"), _T("Checked Disabled Color Same as Normal"));
		CControlBaseProp::InitResColorIDVariant(&m_strResColorIDCD, _T("ColorResCD"), _T("Checked Disabled"));
	}

	CToggleButtonProp::~CToggleButtonProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CCheckBoxProp::CCheckBoxProp()
	{
		m_lStructSize = (LONG)sizeof(CCheckBoxProp);
		m_eControlType = CT_CHECK;

		m_eChkType = CT_NORMAL;
		m_eChkType.SetDefaultValue((_variant_t)m_eChkType);
		m_eChkType.vtEx = VT_EX_CHECKBOX_TYPE;
		m_eChkType.bReadOnly = true;
		m_eChkType.bGroup = true;
		m_eChkType.SetGroupName(_T("General"));
		m_eChkType.SetIUIElementName(_T("CheckBoxType"));
		m_eChkType.SetPropWndName(_T("CheckBox Type"));
		m_eChkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CT_NORMAL).GetEnumString());
		m_eChkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CT_MAXIMIZEBOX).GetEnumString());
	}

	CCheckBoxProp::~CCheckBoxProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CRadioButtonProp::CRadioButtonProp()
	{
		m_lStructSize = (LONG)sizeof(CRadioButtonProp);
		m_eControlType = CT_RADIO;

		m_bGroup = false;
		m_bGroup.SetDefaultValue((_variant_t)m_bGroup);
		m_bGroup.bGroup = true;
		m_bGroup.SetGroupName(_T("General"));
		m_bGroup.SetIUIElementName(_T("IsGroup"));
		m_bGroup.SetPropWndName(_T("Group"));
		m_bGroup.SetDescription(_T(""));

		// The task window ID while the radio button as a tab of CTaskWndMgr.
		m_strBindTaskWndID = _T("");
		m_strBindTaskWndID.SetDefaultValue((_variant_t)m_strBindTaskWndID);
		m_strBindTaskWndID.vtEx = VT_EX_WINDOW_ID_NOT_INCLUDE_SELF;
		m_strBindTaskWndID.SetIUIElementName(_T("BindTaskWndID"));
		m_strBindTaskWndID.SetPropWndName(_T("Bind Task Window ID"));

		// The parent ID of the task window. A window may be include more then one CTaskWndMgr
		m_strBindTaskMgrID = _T("");
		m_strBindTaskMgrID.SetDefaultValue((_variant_t)m_strBindTaskMgrID);
		m_strBindTaskMgrID.vtEx = VT_EX_TASKMGR_ID;
		m_strBindTaskMgrID.SetIUIElementName(_T("BindTaskMgrID"));
		m_strBindTaskMgrID.SetPropWndName(_T("Bind TaskMgr ID"));
	}

	CRadioButtonProp::~CRadioButtonProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CRichTextBoxProp::CRichTextBoxProp()
	{
		m_lStructSize = (LONG)sizeof(CRichTextBoxProp);
		m_eControlType = CT_RICHEDIT;

		m_strBkColorResIDCN.bSaveToSkinFile = m_strBkColorResIDCN.bShowInPropertiesWnd = false;
		m_strBkColorResIDCH.bSaveToSkinFile = m_strBkColorResIDCH.bShowInPropertiesWnd = false;
		m_strBkColorResIDCP.bSaveToSkinFile = m_strBkColorResIDCP.bShowInPropertiesWnd = false;
		m_strBkColorResIDCD.bSaveToSkinFile = m_strBkColorResIDCD.bShowInPropertiesWnd = false;

		m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = false;
		m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = false;
		m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = false;
		m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = false;

		DisableCaptionGroup();
		m_rcPadding4Text.bSaveToSkinFile = m_rcPadding4Text.bShowInPropertiesWnd = true;
		m_rcPadding4Text.bGroup = true;
		m_rcPadding4Text.SetGroupName(_T("Text"));
		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = true;

		DisableTooltipsGroup();
		DisableCheckedBorderColorProperties();

		m_strResFontIDH.bSaveToSkinFile = m_strResFontIDH.bShowInPropertiesWnd = false;
		m_strResFontIDP.bSaveToSkinFile = m_strResFontIDP.bShowInPropertiesWnd = false;
		m_strResFontIDD.bSaveToSkinFile = m_strResFontIDD.bShowInPropertiesWnd = false;

		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = false;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = false;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = false;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = false;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = false;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = false;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = false;

		m_strResFontIDN.SetPropWndName(_T("Default Font"));

		m_bTabStop = true;

		m_bHScroll.bGroup = true;
		m_bHScroll.SetGroupName(_T("General"));
		m_bHScroll = false;
		m_bHScroll.SetDefaultValue((_variant_t)m_bHScroll);
		m_bHScroll.SetPropWndName(_T("IsHScroll"));
		m_bHScroll.SetIUIElementName(m_bHScroll.GetPropWndName());

		m_bVScroll = false;
		m_bVScroll.SetDefaultValue((_variant_t)m_bVScroll);
		m_bVScroll.SetPropWndName(_T("IsVScroll"));
		m_bVScroll.SetIUIElementName(m_bVScroll.GetPropWndName());

		m_bAutoHScroll = true;
		m_bAutoHScroll.SetDefaultValue((_variant_t)m_bAutoHScroll);
		m_bAutoHScroll.SetPropWndName(_T("IsAutoHScroll"));
		m_bAutoHScroll.SetIUIElementName(m_bAutoHScroll.GetPropWndName());

		m_bAutoVScroll = false;
		m_bAutoVScroll.SetDefaultValue((_variant_t)m_bAutoVScroll);
		m_bAutoVScroll.SetPropWndName(_T("IsAutoVScroll"));
		m_bAutoVScroll.SetIUIElementName(m_bAutoVScroll.GetPropWndName());

		m_bDisableNoScroll = false;
		m_bDisableNoScroll.SetDefaultValue((_variant_t)m_bDisableNoScroll);
		m_bDisableNoScroll.SetPropWndName(_T("IsDisableNoScroll"));
		m_bDisableNoScroll.SetIUIElementName(m_bDisableNoScroll.GetPropWndName());

		m_bLowercase = false;
		m_bLowercase.SetDefaultValue((_variant_t)m_bLowercase);
		m_bLowercase.SetPropWndName(_T("IsLowerCase"));
		m_bLowercase.SetIUIElementName(m_bLowercase.GetPropWndName());

		m_bUppercase = false;
		m_bUppercase.SetDefaultValue((_variant_t)m_bUppercase);
		m_bUppercase.SetPropWndName(_T("IsUpperCase"));
		m_bUppercase.SetIUIElementName(m_bUppercase.GetPropWndName());

		m_bNumber = false;
		m_bNumber.SetDefaultValue((_variant_t)m_bNumber);
		m_bNumber.SetPropWndName(_T("IsNumber"));
		m_bNumber.SetIUIElementName(m_bNumber.GetPropWndName());

		m_bPassword = false;
		m_bPassword.SetDefaultValue((_variant_t)m_bPassword);
		m_bPassword.SetPropWndName(_T("IsPassword"));
		m_bPassword.SetIUIElementName(m_bPassword.GetPropWndName());

		m_bReadOnly = false;
		m_bReadOnly.SetDefaultValue((_variant_t)m_bReadOnly);
		m_bReadOnly.SetPropWndName(_T("IsReadOnly"));
		m_bReadOnly.SetIUIElementName(m_bReadOnly.GetPropWndName());

		m_bMultiline = false;
		m_bMultiline.SetDefaultValue((_variant_t)m_bMultiline);
		m_bMultiline.SetPropWndName(_T("IsMultiline"));
		m_bMultiline.SetIUIElementName(m_bMultiline.GetPropWndName());

		m_bWantReturn = false;
		m_bWantReturn.SetDefaultValue((_variant_t)m_bWantReturn);
		m_bWantReturn.SetPropWndName(_T("IsWantReturn"));
		m_bWantReturn.SetIUIElementName(m_bWantReturn.GetPropWndName());

		m_bR2LReadOrder = false;
		m_bR2LReadOrder.SetDefaultValue((_variant_t)m_bR2LReadOrder);
		m_bR2LReadOrder.SetVarName(_T("m_bR2LReadOrder"));
		m_bR2LReadOrder.SetIUIElementName(_T("IsR2LReadOrder"));
		m_bR2LReadOrder.SetPropWndName(_T("Right to Left Reading Order"));
		m_bR2LReadOrder.SetDescription(_T("For languages that support reading order alignment, specifies right-to-left reading order."));

		// Text Alignment
		m_eTextHorAlignMode = TAH_LEFT;
		m_eTextHorAlignMode.SetDefaultValue((_variant_t)m_eTextHorAlignMode);
		m_eTextHorAlignMode.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignMode.bReadOnly = true;
		m_eTextHorAlignMode.bGroup = true;
		m_eTextHorAlignMode.SetGroupName(_T("Text"));
		m_eTextHorAlignMode.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignMode.SetIUIAttributeName(_T("Horizontal"));
		m_eTextHorAlignMode.SetPropWndName(_T("Horizontal Alignment"));
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());
	}

	CRichTextBoxProp::~CRichTextBoxProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CIMRichTextBoxProp::CIMRichTextBoxProp()
	{
		m_lStructSize = (LONG)sizeof(CIMRichTextBoxProp);
		m_eControlType = CT_IM;

		m_bHScroll.bSaveToSkinFile = m_bHScroll.bShowInPropertiesWnd = false;
		m_bAutoHScroll.bSaveToSkinFile = m_bAutoHScroll.bShowInPropertiesWnd = false;
		m_bLowercase.bSaveToSkinFile = m_bLowercase.bShowInPropertiesWnd = false;
		m_bUppercase.bSaveToSkinFile = m_bUppercase.bShowInPropertiesWnd = false;
		m_bNumber.bSaveToSkinFile = m_bNumber.bShowInPropertiesWnd = false;
		m_bPassword.bSaveToSkinFile = m_bPassword.bShowInPropertiesWnd = false;
		m_bReadOnly.bSaveToSkinFile = m_bReadOnly.bShowInPropertiesWnd = false;
		m_bMultiline.bSaveToSkinFile = m_bMultiline.bShowInPropertiesWnd = false;
		m_bWantReturn.bSaveToSkinFile = m_bWantReturn.bShowInPropertiesWnd = false;

		m_bVScroll.bGroup = true;
		m_bVScroll.SetGroupName(_T("General"));
	}

	CIMRichTextBoxProp::~CIMRichTextBoxProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CComboBoxProp::CComboBoxProp()
	{
		m_lStructSize = (LONG)sizeof(CComboBoxProp);
		m_eControlType = CT_COMBOBOX;

		m_bTabStop = true;
		m_bTabStop.SetDefaultValue((_variant_t)m_bTabStop);

		DisableTextColorProperties(TRUE);

		m_bDrawBorderColor.bSaveToSkinFile = m_bDrawBorderColor.bShowInPropertiesWnd = true;
		m_strBorderColorResIDN.bSaveToSkinFile = m_strBorderColorResIDN.bShowInPropertiesWnd = true;
		m_strBorderColorResIDH.bSaveToSkinFile = m_strBorderColorResIDH.bShowInPropertiesWnd = true;
		m_strBorderColorResIDP.bSaveToSkinFile = m_strBorderColorResIDP.bShowInPropertiesWnd = true;
		m_strBorderColorResIDD.bSaveToSkinFile = m_strBorderColorResIDD.bShowInPropertiesWnd = true;
		DisableCheckedBorderColorProperties();

		m_strBKImageN.bSaveToSkinFile = m_strBKImageN.bShowInPropertiesWnd = true;
		m_strBKImageH.bSaveToSkinFile = m_strBKImageH.bShowInPropertiesWnd = true;
		m_strBKImageP.bSaveToSkinFile = m_strBKImageP.bShowInPropertiesWnd = true;
		m_strBKImageD.bSaveToSkinFile = m_strBKImageD.bShowInPropertiesWnd = true;

		m_bDrawBkColor = true;
		m_bDrawBkColor.SetDefaultValue((_variant_t)m_bDrawBkColor);
		m_bDrawBorderColor = true;
		m_bDrawBorderColor.SetDefaultValue((_variant_t)m_bDrawBorderColor);

		m_strCaption.bSaveToSkinFile = m_strCaption.bShowInPropertiesWnd = false;
		m_strTextColorResIDN.bGroup = true;
		m_strTextColorResIDN.SetGroupName(_T("DropDownList Style Static Control"));
		m_strTextColorResIDN.bSaveToSkinFile = m_strTextColorResIDN.bShowInPropertiesWnd = true;
		m_strTextColorResIDH.bSaveToSkinFile = m_strTextColorResIDH.bShowInPropertiesWnd = true;
		m_strTextColorResIDP.bSaveToSkinFile = m_strTextColorResIDP.bShowInPropertiesWnd = true;
		m_strTextColorResIDD.bSaveToSkinFile = m_strTextColorResIDD.bShowInPropertiesWnd = true;
		m_strTextColorResIDCN.bSaveToSkinFile = m_strTextColorResIDCN.bShowInPropertiesWnd = false;
		m_strTextColorResIDCH.bSaveToSkinFile = m_strTextColorResIDCH.bShowInPropertiesWnd = false;
		m_strTextColorResIDCP.bSaveToSkinFile = m_strTextColorResIDCP.bShowInPropertiesWnd = false;
		m_strTextColorResIDCD.bSaveToSkinFile = m_strTextColorResIDCD.bShowInPropertiesWnd = false;
		m_eTextHorAlignMode.bGroup = true;
		m_eTextHorAlignMode.SetGroupName(m_strTextColorResIDN.GetGroupName());

		m_strResFontIDN.bGroup = false;
		m_strResFontIDH.bSaveToSkinFile = m_strResFontIDH.bShowInPropertiesWnd = true;
		m_strResFontIDP.bSaveToSkinFile = m_strResFontIDP.bShowInPropertiesWnd = true;
		m_strResFontIDD.bSaveToSkinFile = m_strResFontIDD.bShowInPropertiesWnd = true;

		m_eComboStyle = CS_DROPDOWN;
		m_eComboStyle.SetDefaultValue((_variant_t)m_eComboStyle);
		m_eComboStyle.vtEx = VT_EX_COMBOBOX_STYLE;
		m_eComboStyle.bGroup = true;
		m_eComboStyle.SetGroupName(_T("General"));
		m_eComboStyle.SetIUIElementName(_T("ComboStyle"));
		m_eComboStyle.SetPropWndName(_T("Style"));
		m_eComboStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CS_SIMPLE).GetEnumString());
		m_eComboStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CS_DROPDOWN).GetEnumString());
		m_eComboStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(CS_DROPDOWNLIST).GetEnumString());

		// 当ComboBox指定了垂直滚动条风格后，下拉列表才能出现滚动条
		m_bVScroll = true;
		m_bVScroll.SetDefaultValue((_variant_t)m_bVScroll);
		m_bVScroll.SetIUIElementName(_T("IsVScroll"));
		m_bVScroll.SetPropWndName(_T("Vertical Scroll"));

		m_bAutoHScroll = true;
		m_bAutoHScroll.SetDefaultValue((_variant_t)m_bAutoHScroll);
		m_bAutoHScroll.SetIUIElementName(_T("IsAutoHScroll"));
		m_bAutoHScroll.SetPropWndName(_T("Auto HScroll"));

		m_bSort = false;
		m_bSort.SetDefaultValue((_variant_t)m_bSort);
		m_bSort.SetVarName(_T("m_bSort"));
		m_bSort.SetIUIElementName(_T("IsSort"));
		m_bSort.SetPropWndName(_T("Sort"));

		m_bDisableNoScroll = false;
		m_bDisableNoScroll.SetDefaultValue((_variant_t)m_bDisableNoScroll);
		m_bDisableNoScroll.SetVarName(_T("m_bDisableNoScroll"));
		m_bDisableNoScroll.SetIUIElementName(_T("IsDisableNoScroll"));
		m_bDisableNoScroll.SetPropWndName(_T("Disable No Scroll"));

		m_bLowercase = false;
		m_bLowercase.SetDefaultValue((_variant_t)m_bLowercase);
		m_bLowercase.SetVarName(_T("m_bLowercase"));
		m_bLowercase.SetIUIElementName(_T("IsLowerCase"));
		m_bLowercase.SetPropWndName(m_bLowercase.GetIUIElementName());

		m_bUppercase = false;
		m_bUppercase.SetDefaultValue((_variant_t)m_bUppercase);
		m_bUppercase.SetVarName(_T("m_bUppercase"));
		m_bUppercase.SetIUIElementName(_T("IsUpperCase"));
		m_bUppercase.SetPropWndName(m_bUppercase.GetIUIElementName());

		// Edit Box group
		m_rcEditMargin = CIUIRect(0, 0, 0, 0);
		m_rcEditMargin.vtEx = VT_EX_RECT;
		m_rcEditMargin.bGroup = true;
		m_rcEditMargin.SetGroupName(_T("Edit Box"));
		m_rcEditMargin.SetDefaultValue((_variant_t)m_rcEditMargin);
		m_rcEditMargin.SetPropWndName(_T("Edit Box Margin"));
		m_rcEditMargin.SetIUIElementName(_T("EditBoxMargin"));
		m_rcEditMargin.nMinValue = -3840;
		m_rcEditMargin.nMaxValue = 3840;

		// Button group.
		m_lBtnWidth = (LONG)16;
		m_lBtnWidth.bGroup = true;
		m_lBtnWidth.SetGroupName(_T("Drop Button"));
		m_lBtnWidth.SetDefaultValue((_variant_t)m_lBtnWidth);
		m_lBtnWidth.SetIUIElementName(_T("ButtonWidth"));
		m_lBtnWidth.SetPropWndName(_T("Button Width"));
		m_lBtnWidth.nMinValue = 0;
		m_lBtnWidth.nMaxValue = 1920;

		m_rcButtonMargin = CIUIRect(0, 0, 0, 0);
		m_rcButtonMargin.vtEx = VT_EX_RECT;
		m_rcButtonMargin.SetDefaultValue((_variant_t)m_rcButtonMargin);
		m_rcButtonMargin.SetPropWndName(_T("Drop Button Margin"));
		m_rcButtonMargin.SetIUIElementName(_T("DropButtonMargin"));
		m_rcButtonMargin.nMinValue = -3840;
		m_rcButtonMargin.nMaxValue = 3840;

		m_bDrawDropButtonBkColor = true;
		m_bDrawDropButtonBkColor.SetDefaultValue((_variant_t)m_bDrawDropButtonBkColor);
		m_bDrawDropButtonBkColor.SetIUIElementName(_T("IsDrawDropButtonBkColor"));
		m_bDrawDropButtonBkColor.SetPropWndName(_T("Draw Drop Button Bk Color"));

		m_strDropButtonBkResColorIDN = _T("");
		m_strDropButtonBkResColorIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBkResColorIDN.SetIUIElementName(_T("DropButtonBkColorResN"));
		m_strDropButtonBkResColorIDN.SetPropWndName(_T("Normal Drop Button Bk Color"));

		m_strDropButtonBkResColorIDH = _T("");
		m_strDropButtonBkResColorIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBkResColorIDH.SetIUIElementName(_T("DropButtonBkColorResH"));
		m_strDropButtonBkResColorIDH.SetPropWndName(_T("Hover Drop Button Bk Color"));

		m_strDropButtonBkResColorIDP = _T("");
		m_strDropButtonBkResColorIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBkResColorIDP.SetIUIElementName(_T("DropButtonBkColorResP"));
		m_strDropButtonBkResColorIDP.SetPropWndName(_T("Pressed Drop Button Bk Color"));

		m_strDropButtonBkResColorIDD = _T("");
		m_strDropButtonBkResColorIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBkResColorIDD.SetIUIElementName(_T("DropButtonBkColorResD"));
		m_strDropButtonBkResColorIDD.SetPropWndName(_T("Disabled Drop Button Bk Color"));

		m_strDropButtonBkResColorIDS = _T("");
		m_strDropButtonBkResColorIDS.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBkResColorIDS.SetIUIElementName(_T("DropButtonBkColorResS"));
		m_strDropButtonBkResColorIDS.SetPropWndName(_T("Selected Drop Button Bk Color"));

		m_strDropButtonCombineBkImage = _T("");
		m_strDropButtonCombineBkImage.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonCombineBkImage.SetIUIElementName(_T("ButtonCombineBkImage"));
		m_strDropButtonCombineBkImage.SetPropWndName(_T("Combine Button Bk Image"));

		m_strDropButtonBkImageN = _T("");
		m_strDropButtonBkImageN.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonBkImageN.SetIUIElementName(_T("ButtonBkImageN"));
		m_strDropButtonBkImageN.SetPropWndName(_T("Normal Button Bk Image"));

		m_strDropButtonBkImageH = _T("");
		m_strDropButtonBkImageH.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonBkImageH.SetIUIElementName(_T("ButtonBkImageH"));
		m_strDropButtonBkImageH.SetPropWndName(_T("Hover Button Bk Image"));

		m_strDropButtonBkImageP = _T("");
		m_strDropButtonBkImageP.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonBkImageP.SetIUIElementName(_T("ButtonBkImageP"));
		m_strDropButtonBkImageP.SetPropWndName(_T("Pressed Button Bk Image"));

		m_strDropButtonBkImageD = _T("");
		m_strDropButtonBkImageD.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonBkImageD.SetIUIElementName(_T("ButtonBkImageD"));
		m_strDropButtonBkImageD.SetPropWndName(_T("Disabled Button Bk Image"));

		m_strDropButtonBkImageS = _T("");
		m_strDropButtonBkImageS.vtEx = VT_EX_BITMAPPATH;
		m_strDropButtonBkImageS.SetIUIElementName(_T("ButtonBkImageS"));
		m_strDropButtonBkImageS.SetPropWndName(_T("Selected Button Bk Image"));

		m_bDrawDropButtonBorderColor = true;
		m_bDrawDropButtonBorderColor.SetDefaultValue((_variant_t)m_bDrawDropButtonBorderColor);
		m_bDrawDropButtonBorderColor.SetIUIElementName(_T("IsDrawDropButtonBorderColor"));
		m_bDrawDropButtonBorderColor.SetPropWndName(_T("Draw Drop Button Border Color"));

		m_strDropButtonBorderResColorIDN = _T("");
		m_strDropButtonBorderResColorIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBorderResColorIDN.SetIUIElementName(_T("DropButtonBorderColorResN"));
		m_strDropButtonBorderResColorIDN.SetPropWndName(_T("Normal Drop Button Border Color"));

		m_strDropButtonBorderResColorIDH = _T("");
		m_strDropButtonBorderResColorIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBorderResColorIDH.SetIUIElementName(_T("DropButtonBorderColorResH"));
		m_strDropButtonBorderResColorIDH.SetPropWndName(_T("Hover Drop Button Border Color"));

		m_strDropButtonBorderResColorIDP = _T("");
		m_strDropButtonBorderResColorIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBorderResColorIDP.SetIUIElementName(_T("DropButtonBorderColorResP"));
		m_strDropButtonBorderResColorIDP.SetPropWndName(_T("Pressed Drop Button Border Color"));

		m_strDropButtonBorderResColorIDD = _T("");
		m_strDropButtonBorderResColorIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBorderResColorIDD.SetIUIElementName(_T("DropButtonBorderColorResD"));
		m_strDropButtonBorderResColorIDD.SetPropWndName(_T("Disabled Drop Button Border Color"));

		m_strDropButtonBorderResColorIDS = _T("");
		m_strDropButtonBorderResColorIDS.vtEx = VT_EX_RESOURCE_COLOR;
		m_strDropButtonBorderResColorIDS.SetIUIElementName(_T("DropButtonBorderColorResS"));
		m_strDropButtonBorderResColorIDS.SetPropWndName(_T("Selected Drop Button Border Color"));

		// Button Arrow image
		m_bSpecifyButtonArrowImage = false;
		m_bSpecifyButtonArrowImage.SetDefaultValue((_variant_t)m_bSpecifyButtonArrowImage);
		m_bSpecifyButtonArrowImage.SetIUIElementName(_T("IsSpecifyButtonArrowImage"));
		m_bSpecifyButtonArrowImage.SetPropWndName(_T("Is Specify Button Arrow Image"));

		m_bCombineButtonArrowImage = false;
		m_bCombineButtonArrowImage.SetDefaultValue((_variant_t)m_bCombineButtonArrowImage);
		m_bCombineButtonArrowImage.SetIUIElementName(_T("IsCombineButtonArrowImage"));
		m_bCombineButtonArrowImage.SetPropWndName(_T("Is Combine Button Arrow Image"));

		m_strCombineButtonArrowImage = _T("");
		m_strCombineButtonArrowImage.vtEx = VT_EX_BITMAPPATH;
		m_strCombineButtonArrowImage.SetIUIElementName(_T("ButtonArrowCombine"));
		m_strCombineButtonArrowImage.SetPropWndName(_T("Combine Button Arrow Image"));

		m_strButtonArrowN = _T("");
		m_strButtonArrowN.vtEx = VT_EX_BITMAPPATH;
		m_strButtonArrowN.SetIUIElementName(_T("ButtonArrowNormal"));
		m_strButtonArrowN.SetPropWndName(_T("Normal Button Arrow Image"));

		m_strButtonArrowH = _T("");
		m_strButtonArrowH.vtEx = VT_EX_BITMAPPATH;
		m_strButtonArrowH.SetIUIElementName(_T("ButtonArrowHighlight"));
		m_strButtonArrowH.SetPropWndName(_T("Highlight Button Arrow Image"));

		m_strButtonArrowS = _T("");
		m_strButtonArrowS.vtEx = VT_EX_BITMAPPATH;
		m_strButtonArrowS.SetIUIElementName(_T("ButtonArrowSelected"));
		m_strButtonArrowS.SetPropWndName(_T("Selected Button Arrow Image"));

		m_strButtonArrowD = _T("");
		m_strButtonArrowD.vtEx = VT_EX_BITMAPPATH;
		m_strButtonArrowD.SetIUIElementName(_T("ButtonArrowDisabled"));
		m_strButtonArrowD.SetPropWndName(_T("Disabled Button Arrow Image"));

		m_eButtonArrowHorAlignMode = FAH_LEFT;
		m_eButtonArrowHorAlignMode.SetDefaultValue((_variant_t)m_eButtonArrowHorAlignMode);
		m_eButtonArrowHorAlignMode.vtEx = VT_EX_FOREGROUND_ALIGN_HOR;
		m_eButtonArrowHorAlignMode.bReadOnly = true;
		m_eButtonArrowHorAlignMode.SetIUIElementName(_T("ButtonArrowAlignMode"));
		m_eButtonArrowHorAlignMode.SetIUIAttributeName(_T("Horizontal"));
		m_eButtonArrowHorAlignMode.SetPropWndName(_T("Button Arrow Horizontal Alignment"));
		m_eButtonArrowHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_LEFT).GetEnumString());
		m_eButtonArrowHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_RIGHT).GetEnumString());
		m_eButtonArrowHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_CENTER).GetEnumString());
		m_eButtonArrowHorAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAH_STRETCH).GetEnumString());

		m_eButtonArrowVerAlignMode = FAV_CENTER;
		m_eButtonArrowVerAlignMode.SetDefaultValue((_variant_t)m_eButtonArrowVerAlignMode);
		m_eButtonArrowVerAlignMode.vtEx = VT_EX_FOREGROUND_ALIGN_VER;
		m_eButtonArrowVerAlignMode.bReadOnly = true;
		m_eButtonArrowVerAlignMode.SetIUIElementName(_T("ButtonArrowAlignMode"));
		m_eButtonArrowVerAlignMode.SetIUIAttributeName(_T("Vertical"));
		m_eButtonArrowVerAlignMode.SetPropWndName(_T("Button Arrow Vertical Alignment"));
		m_eButtonArrowVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_TOP).GetEnumString());
		m_eButtonArrowVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_BOTTOM).GetEnumString());
		m_eButtonArrowVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_CENTER).GetEnumString());
		m_eButtonArrowVerAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(FAV_STRETCH).GetEnumString());

		m_rcPadding4ButtonArrow = CIUIRect(0, 0, 0, 0);
		m_rcPadding4ButtonArrow.vtEx = VT_EX_RECT;
		m_rcPadding4ButtonArrow.SetDefaultValue((_variant_t)m_rcPadding4ButtonArrow);
		m_rcPadding4ButtonArrow.SetPropWndName(_T("Button Arrow Margin"));
		m_rcPadding4ButtonArrow.SetIUIElementName(_T("ButtonArrowMargin"));
		m_rcPadding4ButtonArrow.nMinValue = -3840;
		m_rcPadding4ButtonArrow.nMaxValue = 3840;

		// Drop List Group.
		m_lDropListItemHeight = (LONG)24;
		m_lDropListItemHeight.bGroup = true;
		m_lDropListItemHeight.SetGroupName(_T("Drop List"));
		m_lDropListItemHeight.SetDefaultValue((_variant_t)m_lDropListItemHeight);
		m_lDropListItemHeight.SetIUIElementName(_T("DropListItemHeight"));
		m_lDropListItemHeight.SetPropWndName(_T("DropList Item Height"));
		m_lDropListItemHeight.nMinValue = 1;
		m_lDropListItemHeight.nMaxValue = 255;

		m_lDropListHeight = (LONG)80;
		m_lDropListHeight.SetDefaultValue((_variant_t)m_lDropListHeight);
		m_lDropListHeight.SetIUIElementName(_T("DropListHeight"));
		m_lDropListHeight.SetPropWndName(_T("DropList Height"));
		m_lDropListHeight.nMinValue = 0;
		m_lDropListHeight.nMaxValue = 1920;

		m_strHighlightItemBkColorResID = _T("");
		m_strHighlightItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strHighlightItemBkColorResID.SetIUIElementName(_T("HighlightItemBk"));
		m_strHighlightItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strHighlightItemBkColorResID.SetPropWndName(_T("Highlight Item Backgound Color ID"));
	}

	CComboBoxProp::~CComboBoxProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CSpinButtonProp::CSpinButtonProp()
	{
		m_lStructSize = (LONG)sizeof(CSpinButtonProp);
		m_eControlType = CT_SPIN;

		DisableFontProperties(FALSE);
		DisableTextColorProperties(FALSE);

		m_eAlignMode = SPNA_UNATTACHED;
		m_eAlignMode.SetDefaultValue((_variant_t)m_eAlignMode);
		m_eAlignMode.vtEx = VT_EX_SPN_ALIGN;
		m_eAlignMode.bGroup = true;
		m_eAlignMode.SetGroupName(_T("General"));
		m_eAlignMode.SetIUIElementName(_T("Alignment"));
		m_eAlignMode.SetPropWndName(_T("Alignment"));
		m_eAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SPNA_UNATTACHED).GetEnumString());
		m_eAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SPNA_LEFT).GetEnumString());
		m_eAlignMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SPNA_RIGHT).GetEnumString());

		m_bVertical = true;
		m_bVertical.SetDefaultValue((_variant_t)m_bVertical);
		m_bVertical.SetIUIElementName(_T("IsVertical"));
		m_bVertical.SetPropWndName(_T("Vertical"));

		m_bAutoBuddy = false;
		m_bAutoBuddy.SetDefaultValue((_variant_t)m_bAutoBuddy);
		m_bAutoBuddy.SetIUIElementName(_T("IsAutoBuddy"));
		m_bAutoBuddy.SetPropWndName(_T("Auto Buddy"));

		m_bSetBuddyInt = false;
		m_bSetBuddyInt.SetDefaultValue((_variant_t)m_bSetBuddyInt);
		m_bSetBuddyInt.SetIUIElementName(_T("IsSetBuddyInt"));
		m_bSetBuddyInt.SetPropWndName(_T("Set Buddy Int"));

		m_bNoThousands = false;
		m_bNoThousands.SetDefaultValue((_variant_t)m_bNoThousands);
		m_bNoThousands.SetIUIElementName(_T("IsNoThousands"));
		m_bNoThousands.SetPropWndName(_T("No Thousands"));

		m_bWrap = false;
		m_bWrap.SetDefaultValue((_variant_t)m_bWrap);
		m_bWrap.SetIUIElementName(_T("IsWrap"));
		m_bWrap.SetPropWndName(_T("Wrap"));

		m_bArrowKeys = true;
		m_bArrowKeys.SetDefaultValue((_variant_t)m_bArrowKeys);
		m_bArrowKeys.SetIUIElementName(_T("IsArrowKeys"));
		m_bArrowKeys.SetPropWndName(_T("Arrow Keys"));

		m_bHotTrack = false;
		m_bHotTrack.SetDefaultValue((_variant_t)m_bHotTrack);
		m_bHotTrack.SetIUIElementName(_T("IsHotTrack"));
		m_bHotTrack.SetPropWndName(_T("Hot Track"));
	}

	CSpinButtonProp::~CSpinButtonProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CSliderProp::CSliderProp()
	{
		m_lStructSize = (LONG)sizeof(CSliderProp);
		m_eControlType = CT_SLIDER;

		// Background and mark
		m_strBkColorResIDH.SetPropWndName(_T("Normal Mark Color"));
		m_strBkColorResIDH.SetIUIElementName(_T("BkColorResMarkN"));
		m_strBkColorResIDP.SetPropWndName(_T("Disabled Mark Color"));
		m_strBkColorResIDP.SetIUIElementName(_T("BkColorResMarkD"));

		m_strBKImageN.SetPropWndName(_T("Normal Background"));
		m_strBKImageH.SetPropWndName(_T("Normal Mark"));
		m_strBKImageH.SetIUIElementName(_T("MarkNormal"));
		m_strBKImageP.SetPropWndName(_T("Disabled Mark"));
		m_strBKImageP.SetIUIElementName(_T("MarkDisabled"));
		m_strBKImageD.SetPropWndName(_T("Disabled Background"));

		DisableCheckedBackgroundColorProperties();
		DisableCheckedBackgroundImageProperties();
		DisableBorderColorGroup(FALSE, TRUE);
		DisableFontProperties(FALSE);
		DisableTextColorProperties(FALSE);
		DisableCaptionGroup();

		m_eExtendedStyle = SLDES_NORMAL;
		m_eExtendedStyle.SetDefaultValue((_variant_t)m_eExtendedStyle);
		m_eExtendedStyle.bGroup = true;
		m_eExtendedStyle.SetGroupName(_T("General"));
		m_eExtendedStyle.SetIUIElementName(_T("ExtendedStyle"));
		m_eExtendedStyle.SetPropWndName(_T("Extended Style"));
		m_eExtendedStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SLDES_NORMAL).GetEnumString());
		m_eExtendedStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SLDES_PROGRESS).GetEnumString());
		m_eExtendedStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(SLDES_SPLITPROGRESS).GetEnumString());

		m_bVertical = false;
		m_bVertical.SetDefaultValue((_variant_t)m_bVertical);
		m_bVertical.SetIUIElementName(_T("IsVertical"));
		m_bVertical.SetPropWndName(_T("Vertical"));

		// Mark background
		m_bUseMark = true;
		m_bUseMark.SetDefaultValue((_variant_t)m_bUseMark);
		m_bUseMark.SetIUIElementName(_T("IsUseMark"));
		m_bUseMark.SetPropWndName(_T("Use Mark"));

		m_bMarkToThumb = false;
		m_bMarkToThumb.SetDefaultValue((_variant_t)m_bMarkToThumb);
		m_bMarkToThumb.SetIUIElementName(_T("IsMarkToThumb"));
		m_bMarkToThumb.SetPropWndName(_T("Mark To Thumb"));

		m_bThumbPositionMode = false;
		m_bThumbPositionMode.SetDefaultValue((_variant_t)m_bThumbPositionMode);
		m_bThumbPositionMode.SetIUIElementName(_T("ThumbPositionMode"));
		m_bThumbPositionMode.SetPropWndName(_T("Thumb Position Mode"));
		m_bThumbPositionMode.SetDescription(_T("当设置为True时，滑块直接滑动到鼠标左键点击处；当设置为False时，采用常规模式，滑块步进到鼠标点击处。"));

		// Thumb Color
		m_bDrawThumbColor = false;
		m_bDrawThumbColor.SetDefaultValue((_variant_t)m_bDrawThumbColor);
		m_bDrawThumbColor.bGroup = true;
		m_bDrawThumbColor.SetGroupName(_T("Thumb Color"));
		m_bDrawThumbColor.SetIUIElementName(_T("IsDrawThumbColor"));
		m_bDrawThumbColor.SetPropWndName(_T("Is Draw Thumb Color"));

		m_strThumbBkColorRes = _T("");
		m_strThumbBkColorRes.SetDefaultValue((_variant_t)m_strThumbBkColorRes);
		m_strThumbBkColorRes.vtEx = VT_EX_RESOURCE_COLOR;
		m_strThumbBkColorRes.SetIUIElementName(_T("ThumbBkColorRes"));
		m_strThumbBkColorRes.SetPropWndName(_T("Thumb Bk Color"));

		m_strThumbBorderColorRes = _T("");
		m_strThumbBorderColorRes.SetDefaultValue((_variant_t)m_strThumbBorderColorRes);
		m_strThumbBorderColorRes.vtEx = VT_EX_RESOURCE_COLOR;
		m_strThumbBorderColorRes.SetIUIElementName(_T("ThumbBorderColorRes"));
		m_strThumbBorderColorRes.SetPropWndName(_T("Thumb Border Color"));

		// Thumb Images
		m_bCombineThumbImages = true;
		m_bCombineThumbImages.SetDefaultValue((_variant_t)m_bCombineThumbImages);
		m_bCombineThumbImages.bGroup = true;
		m_bCombineThumbImages.SetGroupName(_T("Thumb Images"));
		m_bCombineThumbImages.SetIUIElementName(_T("IsCombineThumbImages"));
		m_bCombineThumbImages.SetPropWndName(_T("CombineThumbImage"));

		m_strThumbCombine = _T("");
		m_strThumbCombine.vtEx = VT_EX_BITMAPPATH;
		m_strThumbCombine.SetIUIElementName(_T("ThumbCombine"));
		m_strThumbCombine.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbCombine.SetPropWndName(_T("Combine Thumb"));

		m_strThumbN = _T("");
		m_strThumbN.vtEx = VT_EX_BITMAPPATH;
		m_strThumbN.SetIUIElementName(_T("ThumbN"));
		m_strThumbN.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbN.SetPropWndName(_T("Normal"));

		m_strThumbH = _T("");
		m_strThumbH.vtEx = VT_EX_BITMAPPATH;
		m_strThumbH.SetIUIElementName(_T("ThumbH"));
		m_strThumbH.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbH.SetPropWndName(_T("Highlight"));

		m_strThumbP = _T("");
		m_strThumbP.vtEx = VT_EX_BITMAPPATH;
		m_strThumbP.SetIUIElementName(_T("ThumbS"));
		m_strThumbP.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbP.SetPropWndName(_T("Selected"));

		m_strThumbD = _T("");
		m_strThumbD.vtEx = VT_EX_BITMAPPATH;
		m_strThumbD.SetIUIElementName(_T("ThumbD"));
		m_strThumbD.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbD.SetPropWndName(_T("Disabled"));

		m_lThumbWidth = (LONG)11;
		m_lThumbWidth.SetDefaultValue(_variant_t(m_lThumbWidth));
		m_lThumbWidth.SetIUIElementName(_T("ThumbWidth"));
		m_lThumbWidth.SetPropWndName(_T("Thumb Width"));
		m_lThumbWidth.SetDescription(_T("设置水平Slider滑块的宽度或垂直Slider滑块的高度，即滑块平行轨道方向的长度。"));

		m_lThumbHeight = (LONG)22;
		m_lThumbHeight.SetDefaultValue(_variant_t(m_lThumbHeight));
		m_lThumbHeight.SetIUIElementName(_T("ThumbHeight"));
		m_lThumbHeight.SetPropWndName(_T("Thumb Height"));
		m_lThumbHeight.SetDescription(_T("设置水平Slider滑块的高度或垂直Slider滑块的宽度，即滑块垂直轨道方向的长度。"));

		RECT rect = {0, 0, 0, 0};
		m_rcScrollAreaMargin.vtEx = VT_EX_RECT;
		m_rcScrollAreaMargin = rect;
		m_rcScrollAreaMargin.SetDefaultValue((_variant_t)m_rcScrollAreaMargin);
		m_rcScrollAreaMargin.bGroup = true;
		m_rcScrollAreaMargin.SetGroupName(_T("Scroll Area Margin"));
		m_rcScrollAreaMargin.SetPropWndName(_T("Scroll Area Margin"));
		m_rcScrollAreaMargin.SetIUIElementName(_T("ScrollAreaMargin"));
		m_rcScrollAreaMargin.SetDescription(_T("水平Slider滑块中线扫过的区域的外边距。"));
		m_rcScrollAreaMargin.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcScrollAreaMargin.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Show tip window
		m_bEnableTipWindow = false;
		m_bEnableTipWindow.SetDefaultValue((_variant_t)m_bEnableTipWindow);
		m_bEnableTipWindow.bGroup = true;
		m_bEnableTipWindow.SetGroupName(_T("Show Tip Window"));
		m_sizeTipWindowOffset.SetIUIElementName(_T("EnableTipWindow"));
		m_bEnableTipWindow.SetIUIElementName(m_sizeTipWindowOffset.GetIUIElementName());
		m_bEnableTipWindow.SetPropWndName(_T("Enable"));
		m_bEnableTipWindow.SetIUIAttributeName(m_bEnableTipWindow.GetPropWndName());

		m_sizeTipWindowOffset = CIUIPoint(0, 0);
		m_sizeTipWindowOffset.vtEx = VT_EX_POINT;
		m_sizeTipWindowOffset.SetIUIAttributeName(_T("Offset"));
		m_sizeTipWindowOffset.SetPropWndName(_T("Tip Window Offset"));
		m_sizeTipWindowOffset.nMinValue = -3840;
		m_sizeTipWindowOffset.nMaxValue = 3840;
	}

	CSliderProp::~CSliderProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CProgressBarProp::CProgressBarProp()
	{
		m_lStructSize = (LONG)sizeof(CProgressBarProp);
		m_eControlType = CT_PROGRESS;

		DisableBackgroundProperties(TRUE, TRUE);
		DisableFontProperties(TRUE);
		DisableTextColorProperties(TRUE);

		m_bVertical = false;
		m_bVertical.SetDefaultValue((_variant_t)m_bVertical);
		m_bVertical.bGroup = true;
		m_bVertical.SetGroupName(_T("General"));
		m_bVertical.SetIUIElementName(_T("IsVertical"));
		m_bVertical.SetPropWndName(_T("Vertical"));

		m_eThumbMode = PTM_STRETCH;
		m_eThumbMode.SetDefaultValue((_variant_t)m_eThumbMode);
		m_eThumbMode.bGroup = true;
		m_eThumbMode.SetGroupName(_T("Progress Images"));
		m_eThumbMode.SetIUIElementName(_T("ProgressMode"));
		m_eThumbMode.SetPropWndName(_T("Progress Mode"));
		m_eThumbMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(PTM_STRETCH).GetEnumString());
		m_eThumbMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(PTM_TILE).GetEnumString());
		m_eThumbMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(PTM_CLIP).GetEnumString());

		m_bCombineThumbImages = false;
		m_bCombineThumbImages.SetDefaultValue((_variant_t)m_bCombineThumbImages);
		m_bCombineThumbImages.SetIUIElementName(_T("IsCombineProgressImages"));
		m_bCombineThumbImages.SetPropWndName(_T("CombineProgressImage"));

		m_strThumbCombine = _T("");
		m_strThumbCombine.vtEx = VT_EX_BITMAPPATH;
		m_strThumbCombine.SetIUIElementName(_T("ProgressCombine"));
		m_strThumbCombine.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbCombine.SetPropWndName(_T("Combine Progress"));

		m_strThumbN = _T("");
		m_strThumbN.vtEx = VT_EX_BITMAPPATH;
		m_strThumbN.SetIUIElementName(_T("ThumbN"));
		m_strThumbN.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbN.SetPropWndName(_T("Normal"));

		m_strThumbD = _T("");
		m_strThumbD.vtEx = VT_EX_BITMAPPATH;
		m_strThumbD.SetIUIElementName(_T("ThumbD"));
		m_strThumbD.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbD.SetPropWndName(_T("Disabled"));

		m_eThumbPosition = PTP_DEFAULT;
		m_eThumbPosition.SetDefaultValue((_variant_t)m_eThumbPosition);
		m_eThumbPosition.SetIUIElementName(_T("ThumbPositionMode"));
		m_eThumbPosition.SetPropWndName(_T("Thumb Position Mode"));
		m_eThumbPosition.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(PTP_DEFAULT).GetEnumString());
		m_eThumbPosition.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(PTP_SPECIFIED).GetEnumString());

		m_rcThumbOriginalSpecified.vtEx = VT_EX_RECT_WH;
		m_rcThumbOriginalSpecified = CIUIRect(0, 0, 0, 0);
		m_rcThumbOriginalSpecified.SetDefaultValue((_variant_t)m_rcThumbOriginalSpecified);
		m_rcThumbOriginalSpecified.SetPropWndName(_T("Original Specified Thumb Position"));
		m_rcThumbOriginalSpecified.SetIUIElementName(_T("ThumbOriginalPosition"));
		m_rcThumbOriginalSpecified.nMinValue = 0;
		m_rcThumbOriginalSpecified.nMaxValue = 3840;

		m_bDrawThumbColor = false;
		m_bDrawThumbColor.SetDefaultValue((_variant_t)m_bDrawThumbColor);
		m_bDrawThumbColor.SetIUIElementName(_T("IsDrawThumbColor"));
		m_bDrawThumbColor.SetPropWndName(_T("Is Draw Thumb Color"));

		m_strThumbBkColorRes = _T("");
		m_strThumbBkColorRes.SetDefaultValue((_variant_t)m_strThumbBkColorRes);
		m_strThumbBkColorRes.vtEx = VT_EX_RESOURCE_COLOR;
		m_strThumbBkColorRes.SetIUIElementName(_T("ThumbBkColorRes"));
		m_strThumbBkColorRes.SetPropWndName(_T("Thumb Bk Color"));

		m_strThumbBorderColorRes = _T("");
		m_strThumbBorderColorRes.SetDefaultValue((_variant_t)m_strThumbBorderColorRes);
		m_strThumbBorderColorRes.vtEx = VT_EX_RESOURCE_COLOR;
		m_strThumbBorderColorRes.SetIUIElementName(_T("ThumbBorderColorRes"));
		m_strThumbBorderColorRes.SetPropWndName(_T("Thumb Border Color"));

		m_bShowText = true;
		m_bShowText.SetDefaultValue((_variant_t)m_bShowText);
		m_bShowText.bGroup = true;
		m_bShowText.SetGroupName(_T("Show Percent Text"));
		m_bShowText.SetIUIElementName(_T("IsShowText"));
		m_bShowText.SetPropWndName(_T("Show"));

		m_lHorAlignN = (LONG)0;
		m_lHorAlignN.SetDefaultValue((_variant_t)m_lHorAlignN);
		m_lHorAlignN.SetIUIElementName(_T("TextAlignment"));
		m_lHorAlignN.SetIUIAttributeName(_T("HorOffsetN"));
		m_lHorAlignN.SetPropWndName(_T("Horizontal Offset"));
		m_lHorAlignN.nMinValue = -1920;
		m_lHorAlignN.nMaxValue = 1920;

		m_lVerAlignN = (LONG)0;
		m_lVerAlignN.SetDefaultValue((_variant_t)m_lVerAlignN);
		m_lVerAlignN.SetIUIElementName(_T("TextAlignment"));
		m_lVerAlignN.SetIUIAttributeName(_T("VerOffsetN"));
		m_lVerAlignN.SetPropWndName(_T("Vertical Offset"));
		m_lVerAlignN.nMinValue = -1920;
		m_lVerAlignN.nMaxValue = 1920;
	}

	CProgressBarProp::~CProgressBarProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CColumnHeaderProp::CColumnHeaderProp()
	{
		m_lStructSize = (LONG)sizeof(CColumnHeaderProp);
		m_eControlType = CT_HEADERCTRL;

		m_rcControl.bSaveToSkinFile = m_rcControl.bShowInPropertiesWnd = false;
		m_bVisible.bSaveToSkinFile = m_bVisible.bShowInPropertiesWnd = false;
		m_bEnabled.bSaveToSkinFile = m_bEnabled.bShowInPropertiesWnd = false;
		m_bTabStop.bSaveToSkinFile = m_bTabStop.bShowInPropertiesWnd = false;
		m_eHorLayoutMode.bSaveToSkinFile = m_eHorLayoutMode.bShowInPropertiesWnd = false;
		m_eVerLayoutMode.bSaveToSkinFile = m_eVerLayoutMode.bShowInPropertiesWnd = false;
		m_rcLayoutMargin.bSaveToSkinFile = m_rcLayoutMargin.bShowInPropertiesWnd = false;
		m_rcInset.bSaveToSkinFile = m_rcInset.bShowInPropertiesWnd = false;
		m_lTitlebarHeight.bSaveToSkinFile = m_lTitlebarHeight.bShowInPropertiesWnd = false;
		m_bMouseTransparent.bSaveToSkinFile = m_bMouseTransparent.bShowInPropertiesWnd = false;
		m_strBindXml.bSaveToSkinFile = m_strBindXml.bShowInPropertiesWnd = false;
		DisableBkColorProperties(TRUE, TRUE);

		DisableBackgroundProperties(TRUE, TRUE);
		DisableBorderColorGroup(TRUE, TRUE);
		DisableCaptionGroup();
		DisableTooltipsGroup();

		m_bHeaderTrack = true;
		m_bHeaderTrack.SetDefaultValue((_variant_t)m_bHeaderTrack);
		m_bHeaderTrack.bGroup = true;
		m_bHeaderTrack.SetGroupName(_T("General"));
		m_bHeaderTrack.SetIUIElementName(_T("IsHeaderTrack"));
		m_bHeaderTrack.SetPropWndName(_T("Header Track"));

		m_bHeaderDragDrop = false;
		m_bHeaderDragDrop.SetDefaultValue((_variant_t)m_bHeaderDragDrop);
		m_bHeaderDragDrop.SetIUIElementName(_T("IsHeaderDragDrop"));
		m_bHeaderDragDrop.SetPropWndName(_T("Header Drag & Drop"));

		m_bDrawBkColor.SetGroupName(_T("Background"));

		// Background group
		m_strBKImageN.bSaveToSkinFile = m_strBKImageN.bShowInPropertiesWnd = true;
		m_eBkImageResizeMode.bSaveToSkinFile = m_eBkImageResizeMode.bShowInPropertiesWnd = true;
		m_rcBkImage9GridResizeArg.bSaveToSkinFile = m_rcBkImage9GridResizeArg.bShowInPropertiesWnd = true;

		// Combine Header Button Images
		m_bCombineButtonImage = false;
		m_bCombineButtonImage.SetDefaultValue((_variant_t)m_bCombineButtonImage);
		m_bCombineButtonImage.bGroup = true;
		m_bCombineButtonImage.SetGroupName(_T("Header Button Images"));
		m_bCombineButtonImage.SetIUIElementName(_T("IsCombineHeaderButtonImages"));
		m_bCombineButtonImage.SetPropWndName(_T("Combine Header Button Images"));

		// Combine Header Button Path
		m_strButtonBkCombine = _T("");
		m_strButtonBkCombine.vtEx = VT_EX_BITMAPPATH;
		m_strButtonBkCombine.SetIUIElementName(_T("HeaderButtonCombine"));
		m_strButtonBkCombine.SetIUIAttributeName(_T("RelativePath"));
		m_strButtonBkCombine.SetPropWndName(_T("Combine Header Button Image Path"));

		// Normal Header Button
		m_strButtonBKImageN = _T("");
		m_strButtonBKImageN.vtEx = VT_EX_BITMAPPATH;
		m_strButtonBKImageN.SetIUIElementName(_T("HeaderButtonBkN"));
		m_strButtonBKImageN.SetIUIAttributeName(_T("RelativePath"));
		m_strButtonBKImageN.SetPropWndName(_T("Normal"));

		// Highlight header button
		m_strButtonBKImageH = _T("");
		m_strButtonBKImageH.vtEx = VT_EX_BITMAPPATH;
		m_strButtonBKImageH.SetIUIElementName(_T("HeaderButtonBkH"));
		m_strButtonBKImageH.SetIUIAttributeName(_T("RelativePath"));
		m_strButtonBKImageH.SetPropWndName(_T("Highlight"));

		// Selected header button
		m_strButtonBKImageP = _T("");
		m_strButtonBKImageP.vtEx = VT_EX_BITMAPPATH;
		m_strButtonBKImageP.SetIUIElementName(_T("HeaderButtonBkS"));
		m_strButtonBKImageP.SetIUIAttributeName(_T("RelativePath"));
		m_strButtonBKImageP.SetPropWndName(_T("Selected"));

		// Disabled header button
		m_strButtonBKImageD = _T("");
		m_strButtonBKImageD.vtEx = VT_EX_BITMAPPATH;
		m_strButtonBKImageD.SetIUIElementName(_T("HeaderButtonBkD"));
		m_strButtonBKImageD.SetIUIAttributeName(_T("RelativePath"));
		m_strButtonBKImageD.SetPropWndName(_T("Disabled"));

		m_eButtonBkImageResizeMode = IRM_9GRID;
		m_eButtonBkImageResizeMode.SetDefaultValue((_variant_t)m_eButtonBkImageResizeMode);
		m_eButtonBkImageResizeMode.vtEx = VT_EX_IMAGE_RESIZE_MODE;
		m_eButtonBkImageResizeMode.SetIUIElementName(_T("ButtonBkImageResizeMode"));
		m_eButtonBkImageResizeMode.SetPropWndName(_T("Button Background Image Resize Mode"));
		m_eButtonBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_9GRID).GetEnumString());
		m_eButtonBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_STRETCH).GetEnumString());
		m_eButtonBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_TILE).GetEnumString());
		m_eButtonBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_CENTER).GetEnumString());
		m_eButtonBkImageResizeMode.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(IRM_KEEPRATE).GetEnumString());

		RECT rect = {0, 0, 0, 0};
		m_rcButtonBkImage9GridResizeArg.vtEx = VT_EX_RECT;
		m_rcButtonBkImage9GridResizeArg = rect;
		m_rcButtonBkImage9GridResizeArg.SetDefaultValue((_variant_t)m_rcButtonBkImage9GridResizeArg);
		m_rcButtonBkImage9GridResizeArg.SetPropWndName(_T("9 Grid Parameter"));
		m_rcButtonBkImage9GridResizeArg.SetIUIElementName(_T("ButtonBkImageNineGridArg"));
		m_rcButtonBkImage9GridResizeArg.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcButtonBkImage9GridResizeArg.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Header font
		m_strResFontIDN = _T("");
		m_strResFontIDN.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDN.bGroup = true;
		m_strResFontIDN.SetGroupName(_T("Header Font(Size)"));
		m_strResFontIDN.SetIUIElementName(_T("HeaderFontResN"));
		m_strResFontIDN.SetPropWndName(_T("Normal"));

		m_strResFontIDH = _T("");
		m_strResFontIDH.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDH.SetIUIElementName(_T("HeaderFontResH"));
		m_strResFontIDH.SetPropWndName(_T("Highlight"));

		m_strResFontIDP = _T("");
		m_strResFontIDP.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDP.SetIUIElementName(_T("HeaderFontResS"));
		m_strResFontIDP.SetPropWndName(_T("Selected"));

		m_strResFontIDD = _T("");
		m_strResFontIDD.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDD.SetIUIElementName(_T("HeaderFontResD"));
		m_strResFontIDD.SetPropWndName(_T("Disabled"));

		// Header Text Color
		m_strTextColorResIDN = _T("");
		m_strTextColorResIDN.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDN.bGroup = true;
		m_strTextColorResIDN.SetGroupName(_T("Header Text Color"));
		m_strTextColorResIDN.SetIUIElementName(_T("HeaderColorResN"));
		m_strTextColorResIDN.SetPropWndName(_T("Normal"));

		m_strTextColorResIDH = _T("");
		m_strTextColorResIDH.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDH.SetIUIElementName(_T("HeaderColorResH"));
		m_strTextColorResIDH.SetPropWndName(_T("Highlight"));

		m_strTextColorResIDP = _T("");
		m_strTextColorResIDP.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDP.SetIUIElementName(_T("HeaderColorResP"));
		m_strTextColorResIDP.SetPropWndName(_T("Pressed"));

		m_strTextColorResIDD = _T("");
		m_strTextColorResIDD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strTextColorResIDD.SetIUIElementName(_T("HeaderColorResD"));
		m_strTextColorResIDD.SetPropWndName(_T("Disabled"));
	}

	CColumnHeaderProp::~CColumnHeaderProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CListViewProp::CListViewProp()
	{
		m_lStructSize = (LONG)sizeof(CListViewProp);
		m_eControlType = CT_LISTVIEW;

		DisableBackgroundProperties(TRUE, TRUE);
		DisableCheckedBackgroundColorProperties();
		DisableCheckedBorderColorProperties();
		DisableCaptionGroup();

		// Item text Font
		m_strResFontIDN.SetGroupName(_T("Item Font(Size)"));
		m_strResFontIDN.SetIUIElementName(_T("ItemFontResN"));
		m_strResFontIDH.SetIUIElementName(_T("ItemFontResH"));
		m_strResFontIDP.SetIUIElementName(_T("ItemFontResS"));
		m_strResFontIDD.SetIUIElementName(_T("ItemFontResND"));
		m_strResFontIDD.SetPropWndName(_T("Normal Disabled"));

		// Item text Color
		m_strTextColorResIDN.SetGroupName(_T("Item Text Color"));
		m_strTextColorResIDN.SetIUIElementName(_T("ItemTextColorResN"));
		m_strTextColorResIDH.SetIUIElementName(_T("ItemTextColorResH"));
		m_strTextColorResIDP.SetIUIElementName(_T("ItemTextColorResP"));
		m_strTextColorResIDD.SetIUIElementName(_T("ItemTextColorResND"));
		m_strTextColorResIDD.SetPropWndName(_T("Normal Disabled"));

		m_eListStyle = LSTS_REPORT;
		m_eListStyle.SetDefaultValue((_variant_t)m_eListStyle);
		m_eListStyle.vtEx = VT_EX_LST_STYLE;
		m_eListStyle.bGroup = true;
		m_eListStyle.SetGroupName(_T("General"));
		m_eListStyle.SetIUIElementName(_T("ListStyle"));
		m_eListStyle.SetPropWndName(_T("List Style"));
		m_eListStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTS_ICON).GetEnumString());
		m_eListStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTS_SMALLICON).GetEnumString());
		m_eListStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTS_LIST).GetEnumString());
		m_eListStyle.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTS_REPORT).GetEnumString());

		m_eListSort = LSTST_UNSET;
		m_eListSort.SetDefaultValue((_variant_t)m_eListSort);
		m_eListSort.vtEx = VT_EX_LST_SORT;
		m_eListSort.SetIUIElementName(_T("ListSort"));
		m_eListSort.SetPropWndName(_T("List Sort"));
		m_eListSort.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTST_UNSET).GetEnumString());
		m_eListSort.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTST_ASCENDING).GetEnumString());
		m_eListSort.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTST_DESCENDING).GetEnumString());

		m_bEditLabels = false;
		m_bEditLabels.SetDefaultValue((_variant_t)m_bEditLabels);
		m_bEditLabels.SetIUIElementName(_T("IsEditLabels"));
		m_bEditLabels.SetPropWndName(_T("Edit Labels"));

		m_bShowSelAlways = false;
		m_bShowSelAlways.SetDefaultValue((_variant_t)m_bShowSelAlways);
		m_bShowSelAlways.SetIUIElementName(_T("IsShowSelAlways"));
		m_bShowSelAlways.SetPropWndName(_T("Always Show Selected Item"));

		m_bSingleSel = false;
		m_bSingleSel.SetDefaultValue((_variant_t)m_bSingleSel);
		m_bSingleSel.SetIUIElementName(_T("IsSingleSel"));
		m_bSingleSel.SetPropWndName(_T("Single Select"));

		m_bOwnerData = false;
		m_bOwnerData.SetDefaultValue((_variant_t)m_bOwnerData);
		m_bOwnerData.SetIUIElementName(_T("IsOwnerData"));
		m_bOwnerData.SetPropWndName(_T("Owner Data"));

		m_bHoldCtrlKey = false;
		m_bHoldCtrlKey.SetDefaultValue((_variant_t)m_bHoldCtrlKey);
		m_bHoldCtrlKey.SetIUIElementName(_T("IsHoldCtrlKey"));
		m_bHoldCtrlKey.SetPropWndName(_T("Hold Ctrl Key"));

		m_bCheckBoxes = false;
		m_bCheckBoxes.SetDefaultValue((_variant_t)m_bCheckBoxes);
		m_bCheckBoxes.SetIUIElementName(_T("IsCheckBoxes"));
		m_bCheckBoxes.SetPropWndName(_T("Has Check Boxes"));

		m_bInfoTooltip = false;
		m_bInfoTooltip.SetDefaultValue((_variant_t)m_bInfoTooltip);
		m_bInfoTooltip.SetIUIElementName(_T("IsInfoTip"));
		m_bInfoTooltip.SetPropWndName(_T("Info ToolTip"));

		m_bTwoClickActivate = false;
		m_bTwoClickActivate.SetDefaultValue((_variant_t)m_bTwoClickActivate);
		m_bTwoClickActivate.SetIUIElementName(_T("IsTwoClickActivate"));
		m_bTwoClickActivate.SetPropWndName(_T("Two Click Activate"));

		m_lItemHeight = (LONG)18;
		m_lItemHeight.SetDefaultValue((_variant_t)m_lItemHeight);
		m_lItemHeight.SetIUIElementName(_T("ItemHeight"));
		m_lItemHeight.SetPropWndName(_T("Item Height"));
		m_lItemHeight.nMinValue = 1;

		// Only fro report style
		m_bFullRowSelect = false;
		m_bFullRowSelect.SetDefaultValue((_variant_t)m_bFullRowSelect);
		m_bFullRowSelect.bGroup = true;
		m_bFullRowSelect.SetGroupName(_T("Report extended Properties"));
		m_bFullRowSelect.SetIUIElementName(_T("IsFullRowSelect"));
		m_bFullRowSelect.SetPropWndName(_T("Full Row Select"));

		m_bSubItemImages = false;
		m_bSubItemImages.SetDefaultValue((_variant_t)m_bSubItemImages);
		m_bSubItemImages.SetIUIElementName(_T("IsSubItemImages"));
		m_bSubItemImages.SetPropWndName(_T("Sub Item Images"));

		// Icon and small icon view
		m_eListAlign = LSTA_TOP;
		m_eListAlign.SetDefaultValue((_variant_t)m_eListAlign);
		m_eListAlign.vtEx = VT_EX_LST_ALIGN;
		m_eListAlign.bGroup = true;
		m_eListAlign.SetGroupName(_T("(Small) Icon Extended Properties"));
		m_eListAlign.SetIUIElementName(_T("ListAlign"));
		m_eListAlign.SetPropWndName(_T("List Alignment"));
		m_eListAlign.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTA_TOP).GetEnumString());
		m_eListAlign.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(LSTA_LEFT).GetEnumString());

		m_bAutoArrange = false;
		m_bAutoArrange.SetDefaultValue((_variant_t)m_bAutoArrange);
		m_bAutoArrange.SetIUIElementName(_T("IsAutoArrange"));
		m_bAutoArrange.SetPropWndName(_T("Auto Arrange"));

		m_bNoScroll = false;
		m_bNoScroll.SetDefaultValue((_variant_t)m_bNoScroll);
		m_bNoScroll.SetIUIElementName(_T("IsNoScroll"));
		m_bNoScroll.SetPropWndName(_T("No Scroll"));

		m_bNoLabelWrap = false;
		m_bNoLabelWrap.SetDefaultValue((_variant_t)m_bNoLabelWrap);
		m_bNoLabelWrap.SetIUIElementName(_T("IsNoLabelWrap"));
		m_bNoLabelWrap.SetPropWndName(_T("No Label Wrap"));

		// Grid line
		m_bShowGridLine = false;
		m_bShowGridLine.SetDefaultValue((_variant_t)m_bShowGridLine);
		m_bShowGridLine.bGroup = true;
		m_bShowGridLine.SetGroupName(_T("Grid Line"));
		m_bShowGridLine.SetIUIElementName(_T("GridLine"));
		m_bShowGridLine.SetPropWndName(_T("Show"));
		m_bShowGridLine.SetIUIAttributeName(m_bShowGridLine.GetPropWndName());

		m_bDraw0Row = false;
		m_bDraw0Row.SetDefaultValue((_variant_t)m_bDraw0Row);
		m_bDraw0Row.SetIUIElementName(m_bShowGridLine.GetIUIElementName());
		m_bDraw0Row.SetIUIAttributeName(_T("Draw0Row"));
		m_bDraw0Row.SetPropWndName(_T("Draw First Row"));

		m_lXOffset = (LONG)0;
		m_lXOffset.SetDefaultValue((_variant_t)m_lXOffset);
		m_lXOffset.SetIUIElementName(m_bShowGridLine.GetIUIElementName());
		m_lXOffset.SetIUIAttributeName(_T("XOffset"));
		m_lXOffset.SetPropWndName(_T("Horizontal Offset"));
		m_lXOffset.nMinValue = -3840;
		m_lXOffset.nMaxValue = 3840;

		m_strGridLineColorRes = _T("");
		m_strGridLineColorRes.SetDefaultValue((_variant_t)m_strGridLineColorRes);
		m_strGridLineColorRes.vtEx = VT_EX_RESOURCE_COLOR;
		m_strGridLineColorRes.SetIUIElementName(m_bShowGridLine.GetIUIElementName());
		m_strGridLineColorRes.SetPropWndName(_T("ColorRes"));
		m_strGridLineColorRes.SetIUIAttributeName(m_strGridLineColorRes.GetPropWndName());

		RECT rect = {0};

		// Focus(Caret) Item
		m_eFocusItemBkType = BKT_COLOR;
		m_eFocusItemBkType.SetDefaultValue((_variant_t)m_eFocusItemBkType);
		m_eFocusItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eFocusItemBkType.bGroup = true;
		m_eFocusItemBkType.SetGroupName(_T("Focus(Caret) Item Background"));
		m_eFocusItemBkType.SetIUIElementName(_T("FocusItemBk"));
		m_eFocusItemBkType.SetPropWndName(_T("Type"));
		m_eFocusItemBkType.SetIUIAttributeName(m_eFocusItemBkType.GetPropWndName());
		m_eFocusItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eFocusItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strFocusItemBkColorResID = _T("");
		m_strFocusItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strFocusItemBkColorResID.SetIUIElementName(m_eFocusItemBkType.GetIUIElementName());
		m_strFocusItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strFocusItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strFocusItemBkImage = _T("");
		m_strFocusItemBkImage.vtEx = VT_EX_BITMAPPATH;
		m_strFocusItemBkImage.SetIUIElementName(m_eFocusItemBkType.GetIUIElementName());
		m_strFocusItemBkImage.SetPropWndName(_T("Image"));
		m_strFocusItemBkImage.SetIUIAttributeName(m_strFocusItemBkImage.GetPropWndName());

		m_rcFocusItemBkImage9GridArg = rect;
		m_rcFocusItemBkImage9GridArg.vtEx = VT_EX_RECT;
		m_rcFocusItemBkImage9GridArg.SetDefaultValue(_variant_t(m_rcFocusItemBkImage9GridArg));
		m_rcFocusItemBkImage9GridArg.SetIUIElementName(_T("FocusItemBkImageNineGridArg"));
		m_rcFocusItemBkImage9GridArg.SetPropWndName(_T("9 Grid Arg"));
		m_rcFocusItemBkImage9GridArg.nMinValue = 0;
		m_rcFocusItemBkImage9GridArg.nMaxValue = 3940;

		// Highlight Item
		m_bEnableHighlightItemEffect = false;
		m_bEnableHighlightItemEffect.SetDefaultValue((_variant_t)m_bEnableHighlightItemEffect);
		m_bEnableHighlightItemEffect.bGroup = true;
		m_bEnableHighlightItemEffect.SetGroupName(_T("Highlight Item Background"));
		m_bEnableHighlightItemEffect.SetIUIElementName(_T("HighlightItemBk"));
		m_bEnableHighlightItemEffect.SetPropWndName(_T("Effect"));
		m_bEnableHighlightItemEffect.SetIUIAttributeName(m_bEnableHighlightItemEffect.GetPropWndName());

		m_eHighlightItemBkType = BKT_COLOR;
		m_eHighlightItemBkType.SetDefaultValue((_variant_t)m_eHighlightItemBkType);
		m_eHighlightItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eHighlightItemBkType.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_eHighlightItemBkType.SetPropWndName(_T("Type"));
		m_eHighlightItemBkType.SetIUIAttributeName(m_eHighlightItemBkType.GetPropWndName());
		m_eHighlightItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eHighlightItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strHighlightItemBkColorResID = _T("");
		m_strHighlightItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strHighlightItemBkColorResID.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_strHighlightItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strHighlightItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strHighlightItemBkImage = _T("");
		m_strHighlightItemBkImage.vtEx = VT_EX_BITMAPPATH;
		m_strHighlightItemBkImage.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_strHighlightItemBkImage.SetPropWndName(_T("Image"));
		m_strHighlightItemBkImage.SetIUIAttributeName(m_strHighlightItemBkImage.GetPropWndName());

		m_rcHighlightItemBkImage9GridArg = rect;
		m_rcHighlightItemBkImage9GridArg.vtEx = VT_EX_RECT;
		m_rcHighlightItemBkImage9GridArg.SetDefaultValue(_variant_t(m_rcHighlightItemBkImage9GridArg));
		m_rcHighlightItemBkImage9GridArg.SetIUIElementName(_T("HighlightItemBkImage9GridArg"));
		m_rcHighlightItemBkImage9GridArg.SetPropWndName(_T("9 Grid Arg"));
		m_rcHighlightItemBkImage9GridArg.nMinValue = 0;
		m_rcHighlightItemBkImage9GridArg.nMaxValue = 3940;

		// Selected Item
		m_bSelectedFitClient = false;
		m_bSelectedFitClient.SetDefaultValue((_variant_t)m_bSelectedFitClient);
		m_bSelectedFitClient.bGroup = true;
		m_bSelectedFitClient.SetGroupName(_T("Selected Item Background"));
		m_bSelectedFitClient.SetIUIElementName(_T("SelectedItemBk"));
		m_bSelectedFitClient.SetIUIAttributeName(_T("IsSelectedFitClient"));
		m_bSelectedFitClient.SetPropWndName(_T("Selected Fit Client"));

		m_eSelectedItemBkType = BKT_COLOR;
		m_eSelectedItemBkType.SetDefaultValue((_variant_t)m_eSelectedItemBkType);
		m_eSelectedItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eSelectedItemBkType.SetIUIElementName(m_bSelectedFitClient.GetIUIElementName());
		m_eSelectedItemBkType.SetPropWndName(_T("Type"));
		m_eSelectedItemBkType.SetIUIAttributeName(m_eSelectedItemBkType.GetPropWndName());
		m_eSelectedItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eSelectedItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strSelectedItemBkColorResID = _T("");
		m_strSelectedItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strSelectedItemBkColorResID.SetIUIElementName(m_bSelectedFitClient.GetIUIElementName());
		m_strSelectedItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strSelectedItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strSelectedItemBkImage = _T("");
		m_strSelectedItemBkImage.vtEx = VT_EX_BITMAPPATH;
		m_strSelectedItemBkImage.SetIUIElementName(m_bSelectedFitClient.GetIUIElementName());
		m_strSelectedItemBkImage.SetPropWndName(_T("Image"));
		m_strSelectedItemBkImage.SetIUIAttributeName(m_strSelectedItemBkImage.GetPropWndName());

		m_rcSelectedItemBkImage9GridArg = rect;
		m_rcSelectedItemBkImage9GridArg.vtEx = VT_EX_RECT;
		m_rcSelectedItemBkImage9GridArg.SetDefaultValue(_variant_t(m_rcSelectedItemBkImage9GridArg));
		m_rcSelectedItemBkImage9GridArg.SetIUIElementName(_T("SelectedItemBkImageNineGridArg"));
		m_rcSelectedItemBkImage9GridArg.SetPropWndName(_T("9 Grid Arg"));
		m_rcSelectedItemBkImage9GridArg.nMinValue = 0;
		m_rcSelectedItemBkImage9GridArg.nMaxValue = 3940;

		// Selected Disable Item
		m_eSelectedDisabledItemBkType = BKT_COLOR;
		m_eSelectedDisabledItemBkType.SetDefaultValue((_variant_t)m_eSelectedDisabledItemBkType);
		m_eSelectedDisabledItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eSelectedDisabledItemBkType.bGroup = true;
		m_eSelectedDisabledItemBkType.SetGroupName(_T("Selected Disabled Item Background"));
		m_eSelectedDisabledItemBkType.SetIUIElementName(_T("SelectedDisabledItemBk"));
		m_eSelectedDisabledItemBkType.SetPropWndName(_T("Type"));
		m_eSelectedDisabledItemBkType.SetIUIAttributeName(m_eSelectedDisabledItemBkType.GetPropWndName());
		m_eSelectedDisabledItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eSelectedDisabledItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strSelectedDisabledItemBkColorResID = _T("");
		m_strSelectedDisabledItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strSelectedDisabledItemBkColorResID.SetIUIElementName(m_eSelectedDisabledItemBkType.GetIUIElementName());
		m_strSelectedDisabledItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strSelectedDisabledItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strSelectedDisabledItemBkImage = _T("");
		m_strSelectedDisabledItemBkImage.vtEx = VT_EX_BITMAPPATH;
		m_strSelectedDisabledItemBkImage.SetIUIElementName(m_eSelectedDisabledItemBkType.GetIUIElementName());
		m_strSelectedDisabledItemBkImage.SetPropWndName(_T("Image"));
		m_strSelectedDisabledItemBkImage.SetIUIAttributeName(m_strSelectedDisabledItemBkImage.GetPropWndName());

		m_rcSelectedDisabledItemBkImage9GridArg = rect;
		m_rcSelectedDisabledItemBkImage9GridArg.vtEx = VT_EX_RECT;
		m_rcSelectedDisabledItemBkImage9GridArg.SetDefaultValue(_variant_t(m_rcSelectedDisabledItemBkImage9GridArg));
		m_rcSelectedDisabledItemBkImage9GridArg.SetIUIElementName(_T("SelectedDisabledItemBkImageNineGridArg"));
		m_rcSelectedDisabledItemBkImage9GridArg.SetPropWndName(_T("9 Grid Arg"));
		m_rcSelectedDisabledItemBkImage9GridArg.nMinValue = 0;
		m_rcSelectedDisabledItemBkImage9GridArg.nMaxValue = 3940;

		m_strResFontIDSD = _T("");
		m_strResFontIDSD.vtEx = VT_EX_RESOURCE_FONT;
		m_strResFontIDSD.SetIUIElementName(_T("ItemFontResSD"));
		m_strResFontIDSD.SetPropWndName(_T("Selected Disabled"));

		m_strResColorIDSD = _T("");
		m_strResColorIDSD.vtEx = VT_EX_RESOURCE_COLOR;
		m_strResColorIDSD.SetIUIElementName(_T("ItemTextColorResSD"));
		m_strResColorIDSD.SetPropWndName(_T("Selected Disabled"));

		// For thumbnail view (Icon Mode)item position
		m_lIconModeTotalItemWidth = (LONG)120;
		m_lIconModeTotalItemWidth.SetDefaultValue((_variant_t)m_lIconModeTotalItemWidth);
		m_lIconModeTotalItemWidth.bGroup = true;
		m_lIconModeTotalItemWidth.SetGroupName(_T("Icon Mode Item Size"));
		m_lIconModeTotalItemWidth.SetIUIElementName(_T("TotalIconItemWidth"));
		m_lIconModeTotalItemWidth.SetPropWndName(_T("Icon Item Total Width"));
		m_lIconModeTotalItemWidth.nMinValue = 1;
		m_lIconModeTotalItemWidth.nMaxValue = 3840;
		m_lIconModeTotalItemWidth.SetDescription(_T("for icon view.\r\n\
如果把Icon view划分成N行M列的表格，Item置于单元格内，则Icon Item Total Width和Icon Item Total Height表示单元格的宽和高。\
通过LVM_SETICONSPACING消息来设置这两个值，消息的LPARAM参数为MAKELONG(cx, cy)\
cx和cy的值相对于图标位图的左上角。 因此，为了设置不重叠的图标之间的间距，\
cx或cy值必须包括图标的大小，以及图标之间所需的间距。 不包括图标宽度的值将导致重叠。\
当定义图标间距时，cx和cy必须设置为4或更大。 较小的值不会产生期望的布局。\
若要将cx和cy重置为默认间距，请将LPARAM值设置为-1."));

		m_lIconModeTotalItemHeight = (LONG)140;
		m_lIconModeTotalItemHeight.SetDefaultValue((_variant_t)m_lIconModeTotalItemHeight);
		m_lIconModeTotalItemHeight.SetIUIElementName(_T("TotalIconItemHeight"));
		m_lIconModeTotalItemHeight.SetPropWndName(_T("Icon Item Total Height"));
		m_lIconModeTotalItemHeight.nMinValue = 1;
		m_lIconModeTotalItemHeight.nMaxValue = 3840;
		m_lIconModeTotalItemHeight.SetDescription(_T("for icon view.\r\n\
如果把Icon view划分成N行M列的表格，Item置于单元格内，则Icon Item Total Width和Icon Item Total Height表示单元格的宽和高。\
通过LVM_SETICONSPACING消息来设置这两个值，消息的LPARAM参数为MAKELONG(cx, cy)\
cx和cy的值相对于图标位图的左上角。 因此，为了设置不重叠的图标之间的间距，\
cx或cy值必须包括图标的大小，以及图标之间所需的间距。 不包括图标宽度的值将导致重叠。\
当定义图标间距时，cx和cy必须设置为4或更大。 较小的值不会产生期望的布局。\
若要将cx和cy重置为默认间距，请将LPARAM值设置为-1."));

		// Header
		m_bNoHeader = false;
		m_bNoHeader.SetDefaultValue((_variant_t)m_bNoHeader);
		m_bNoHeader.bGroup = true;
		m_bNoHeader.SetGroupName(_T("Header"));
		m_bNoHeader.SetIUIElementName(_T("IsNoHeader"));
		m_bNoHeader.SetPropWndName(_T("No Header"));

		m_bNoSortHeader = false;
		m_bNoSortHeader.SetDefaultValue((_variant_t)m_bNoSortHeader);
		m_bNoSortHeader.SetIUIElementName(_T("IsNoSortHeader"));
		m_bNoSortHeader.SetPropWndName(_T("Sort Header"));

		m_nHeaderHeight = (LONG)24;
		m_nHeaderHeight.SetDefaultValue((_variant_t)m_nHeaderHeight);
		m_nHeaderHeight.SetIUIElementName(_T("HeaderHeight"));
		m_nHeaderHeight.SetPropWndName(_T("Header Height"));
		m_nHeaderHeight.nMinValue = 0;
		m_nHeaderHeight.nMaxValue = 1200;

		m_lItemHeaderSpace = (LONG)2;
		m_lItemHeaderSpace.SetDefaultValue((_variant_t)m_lItemHeaderSpace);
		m_lItemHeaderSpace.SetIUIElementName(_T("ItemHeaderSpace"));
		m_lItemHeaderSpace.SetPropWndName(_T("Item Header Space"));
		m_lItemHeaderSpace.SetDescription(_T("Report风格的ListView控件，Header与第一个Item的垂直间距"));
		m_lItemHeaderSpace.nMinValue = 0;
		m_lItemHeaderSpace.nMaxValue = 1200;

		m_bHeaderTrack = true;
		m_bHeaderTrack.SetDefaultValue((_variant_t)m_bHeaderTrack);
		m_bHeaderTrack.SetIUIElementName(_T("IsHeaderTrack"));
		m_bHeaderTrack.SetPropWndName(_T("Header Track"));

		m_bHeaderDragDrop = false;
		m_bHeaderDragDrop.SetDefaultValue((_variant_t)m_bHeaderDragDrop);
		m_bHeaderDragDrop.SetIUIElementName(_T("IsHeaderDragDrop"));
		m_bHeaderDragDrop.SetPropWndName(_T("Header Drag Drop"));

		m_strHeaderStyleID = _T("");
		m_strHeaderStyleID.bReadOnly = true;
		m_strHeaderStyleID.vtEx = VT_EX_STYLE_HEADERCTRL;
		m_strHeaderStyleID.SetIUIElementName(_T("HeaderStyleID"));
		m_strHeaderStyleID.SetPropWndName(_T("Header Style"));

		m_lScrollBarWidth = (LONG)Metrics::g_nScrollBarWidth;
		m_lScrollBarWidth.bGroup = true;
		m_lScrollBarWidth.SetGroupName(_T("Scroll Bar"));
		m_lScrollBarWidth.SetDefaultValue((_variant_t)m_lScrollBarWidth);
		m_lScrollBarWidth.SetIUIElementName(_T("ScrollBarWidth"));
		m_lScrollBarWidth.SetPropWndName(_T("Scroll Bar Width"));

		m_strHorScrollbarID = _T("");
		m_strHorScrollbarID.bReadOnly = true;
		m_strHorScrollbarID.vtEx = VT_EX_STYLE_SCROLLBAR;
		m_strHorScrollbarID.SetIUIElementName(_T("HorizontalScrollbarStyleID"));
		m_strHorScrollbarID.SetPropWndName(_T("Horizontal Scrollbar"));

		m_strVerScrollbarID = _T("");
		m_strVerScrollbarID.bReadOnly = true;
		m_strVerScrollbarID.vtEx = VT_EX_STYLE_SCROLLBAR;
		m_strVerScrollbarID.SetIUIElementName(_T("VerticalScrollbarStyleID"));
		m_strVerScrollbarID.SetPropWndName(_T("Vertical Scrollbar"));
	}

	CListViewProp::~CListViewProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CTreeViewProp::CTreeViewProp()
	{
		RECT rect = { 0, 0, 0, 0 };

		m_lStructSize = (LONG)sizeof(CTreeViewProp);
		m_eControlType = CT_TREEVIEW;

		DisableBorderColorGroup(TRUE, TRUE);

		// Background
		DisableBackgroundProperties(TRUE, TRUE);
		DisableBkColorProperties(TRUE, TRUE);
		DisableCaptionGroup();

		// color
		m_strTextColorResIDN.SetGroupName(_T("Item Text Color"));

		m_bCheckBoxes = false;
		m_bCheckBoxes.SetDefaultValue((_variant_t)m_bCheckBoxes);
		m_bCheckBoxes.bGroup = true;
		m_bCheckBoxes.SetGroupName(_T("General"));
		m_bCheckBoxes.SetVarName(_T("bCheckBoxes"));
		m_bCheckBoxes.SetIUIElementName(_T("IsCheckBoxes"));
		m_bCheckBoxes.SetPropWndName(_T("Has Check Boxes"));

		m_bHasButtons = false;
		m_bHasButtons.SetDefaultValue((_variant_t)m_bHasButtons);
		m_bHasButtons.SetVarName(_T("bHasButtons"));
		m_bHasButtons.SetIUIElementName(_T("IsHasButtons"));
		m_bHasButtons.SetPropWndName(_T("Has Buttons"));

		m_bHasLines = false;
		m_bHasLines.SetDefaultValue((_variant_t)m_bHasLines);
		m_bHasLines.SetIUIElementName(_T("IsHasLines"));
		m_bHasLines.SetPropWndName(_T("Has Lines"));

		m_bLinesAtRoot = false;
		m_bLinesAtRoot.SetDefaultValue((_variant_t)m_bLinesAtRoot);
		m_bLinesAtRoot.SetIUIElementName(_T("IsLineAtRoot"));
		m_bLinesAtRoot.SetPropWndName(_T("Line at Root"));

		m_bFullRowSel = false;
		m_bFullRowSel.SetDefaultValue((_variant_t)m_bFullRowSel);
		m_bFullRowSel.SetIUIElementName(_T("IsFullRowSel"));
		m_bFullRowSel.SetPropWndName(_T("Full Row Select"));
		m_bFullRowSel.SetDescription(_T("Enables full-row selection in the tree view. The entire row of the selected item is highlighted, and clicking anywhere on an item's row causes it to be selected. This style cannot be used in conjunction with the TVS_HASLINES style."));

		m_bShowSelAlways = false;
		m_bShowSelAlways.SetDefaultValue((_variant_t)m_bShowSelAlways);
		m_bShowSelAlways.SetIUIElementName(_T("IsShowSelAlways"));
		m_bShowSelAlways.SetPropWndName(_T("Always Show Select"));

		m_bDisableDragDrop = false;
		m_bDisableDragDrop.SetDefaultValue((_variant_t)m_bDisableDragDrop);
		m_bDisableDragDrop.SetIUIElementName(_T("IsDisDragDrop"));
		m_bDisableDragDrop.SetPropWndName(_T("Disable Drap&Drop"));

		m_bEditLabels = false;
		m_bEditLabels.SetDefaultValue((_variant_t)m_bEditLabels);
		m_bEditLabels.SetIUIElementName(_T("IsEditLabels"));
		m_bEditLabels.SetPropWndName(_T("Edit Labels"));

		m_bScroll = true;
		m_bScroll.SetDefaultValue((_variant_t)m_bScroll);
		m_bScroll.SetIUIElementName(_T("IsScroll"));
		m_bScroll.SetPropWndName(_T("Scroll"));

		m_bSmoothVScroll = true;
		m_bSmoothVScroll.SetDefaultValue((_variant_t)m_bSmoothVScroll);
		m_bSmoothVScroll.SetIUIElementName(_T("IsSmoothVScroll"));
		m_bSmoothVScroll.SetPropWndName(_T("Smooth Vertical Scroll"));

		m_bNonEvenHeight = false;
		m_bNonEvenHeight.SetDefaultValue((_variant_t)m_bNonEvenHeight);
		m_bNonEvenHeight.SetIUIElementName(_T("IsNonEvenHeight"));
		m_bNonEvenHeight.SetPropWndName(_T("Non Even Height"));

		m_bSingleExpand = false;
		m_bSingleExpand.SetDefaultValue((_variant_t)m_bSingleExpand);
		m_bSingleExpand.SetIUIElementName(_T("IsSingleExpand"));
		m_bSingleExpand.SetPropWndName(_T("Single Expand"));

		m_bInfoTip = true;
		m_bInfoTip.SetDefaultValue((_variant_t)m_bInfoTip);
		m_bInfoTip.SetIUIElementName(_T("IsInfoTip"));
		m_bInfoTip.SetPropWndName(_T("Info ToolTip"));

		m_bToolTips = false;
		m_bToolTips.SetDefaultValue((_variant_t)m_bToolTips);
		m_bToolTips.SetIUIElementName(_T("IsTooltips"));
		m_bToolTips.SetPropWndName(_T("ToolTip"));

		m_bDefaultItemHeight = true;
		m_bDefaultItemHeight.SetDefaultValue((_variant_t)m_bDefaultItemHeight);
		m_bDefaultItemHeight.SetIUIElementName(_T("DefaultItemHeight"));
		m_bDefaultItemHeight.SetPropWndName(_T("Default Item Height"));

		m_nItemHeight = (LONG)24;
		m_nItemHeight.SetDefaultValue((_variant_t)m_nItemHeight);
		m_nItemHeight.SetIUIElementName(_T("ItemHeight"));
		m_nItemHeight.SetPropWndName(_T("Item Height"));
		m_nItemHeight.nMinValue = 1;
		m_nItemHeight.nMaxValue = 1920;

		m_bEnableItemLineEffect = false;
		m_bEnableItemLineEffect.bGroup = true;
		m_bEnableItemLineEffect.SetGroupName(_T("Enable Item Line"));
		m_bEnableItemLineEffect.SetIUIElementName(_T("ItemLine"));
		m_bEnableItemLineEffect.SetPropWndName(_T("Enable"));
		m_bEnableItemLineEffect.SetIUIAttributeName(m_bEnableItemLineEffect.GetPropWndName());

		m_strItemLineColorResID = _T("");
		m_strItemLineColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strItemLineColorResID.SetIUIElementName(m_bEnableItemLineEffect.GetIUIElementName());
		m_strItemLineColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strItemLineColorResID.SetPropWndName(_T("Color ID"));

		// Collapse and Expand image
		m_strCombineCollapseExpandedImage = _T("");
		m_strCombineCollapseExpandedImage.SetDefaultValue(_variant_t(m_strCombineCollapseExpandedImage));
		m_strCombineCollapseExpandedImage.vtEx = VT_EX_BITMAPPATH;
		m_strCombineCollapseExpandedImage.bGroup = true;
		m_strCombineCollapseExpandedImage.strGroupName = _T("Collapse and Expanded image");
		m_strCombineCollapseExpandedImage.SetIUIElementName(_T("CombineCollapseExpandedImage"));
		m_strCombineCollapseExpandedImage.SetPropWndName(_T("Combine Collapse Expanded Image"));

		m_strCollapseImageN = _T("");
		m_strCollapseImageN.vtEx = VT_EX_BITMAPPATH;
		m_strCollapseImageN.SetIUIElementName(_T("CollapseImageN"));
		m_strCollapseImageN.SetPropWndName(_T("Normal Collapse Image"));

		m_strCollapseImageH = _T("");
		m_strCollapseImageH.vtEx = VT_EX_BITMAPPATH;
		m_strCollapseImageH.SetIUIElementName(_T("CollapseImageH"));
		m_strCollapseImageH.SetPropWndName(_T("Highlight Collapse Image"));

		m_strCollapseImageS = _T("");
		m_strCollapseImageS.vtEx = VT_EX_BITMAPPATH;
		m_strCollapseImageS.SetIUIElementName(_T("CollapseImageS"));
		m_strCollapseImageS.SetPropWndName(_T("Selected Collapse Image"));

		m_strCollapseImageD = _T("");
		m_strCollapseImageD.vtEx = VT_EX_BITMAPPATH;
		m_strCollapseImageD.SetIUIElementName(_T("CollapseImageD"));
		m_strCollapseImageD.SetPropWndName(_T("Disabled Collapse Image"));

		m_strExpandedImageN = _T("");
		m_strExpandedImageN.vtEx = VT_EX_BITMAPPATH;
		m_strExpandedImageN.SetIUIElementName(_T("ExpandedImageN"));
		m_strExpandedImageN.SetPropWndName(_T("Normal Expanded Image"));

		m_strExpandedImageH = _T("");
		m_strExpandedImageH.vtEx = VT_EX_BITMAPPATH;
		m_strExpandedImageH.SetIUIElementName(_T("ExpandedImageH"));
		m_strExpandedImageH.SetPropWndName(_T("Highlight Expanded Image"));

		m_strExpandedImageS = _T("");
		m_strExpandedImageS.vtEx = VT_EX_BITMAPPATH;
		m_strExpandedImageS.SetIUIElementName(_T("ExpandedImageS"));
		m_strExpandedImageS.SetPropWndName(_T("Selected Expanded Image"));

		m_strExpandedImageD = _T("");
		m_strExpandedImageD.vtEx = VT_EX_BITMAPPATH;
		m_strExpandedImageD.SetIUIElementName(_T("ExpandedImageD"));
		m_strExpandedImageD.SetPropWndName(_T("Disabled Expanded Image"));

		// Text horizontal alignment
		m_eTextHorAlignModeN = TAH_LEFT;
		m_eTextHorAlignModeN.SetDefaultValue((_variant_t)m_eTextHorAlignModeN);
		m_eTextHorAlignModeN.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignModeN.bReadOnly = true;
		m_eTextHorAlignModeN.bGroup = true;
		m_eTextHorAlignModeN.SetGroupName(_T("Text Horizontal Align"));
		m_eTextHorAlignModeN.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignModeN.SetIUIAttributeName(_T("HorizontalN"));
		m_eTextHorAlignModeN.SetPropWndName(_T("Normal"));
		m_eTextHorAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());

		m_eTextHorAlignModeH = TAH_LEFT;
		m_eTextHorAlignModeH.SetDefaultValue((_variant_t)m_eTextHorAlignModeH);
		m_eTextHorAlignModeH.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignModeH.bReadOnly = true;
		m_eTextHorAlignModeH.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignModeH.SetIUIAttributeName(_T("HorizontalH"));
		m_eTextHorAlignModeH.SetPropWndName(_T("Highlight"));
		m_eTextHorAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());

		m_eTextHorAlignModeS = TAH_LEFT;
		m_eTextHorAlignModeS.SetDefaultValue((_variant_t)m_eTextHorAlignModeS);
		m_eTextHorAlignModeS.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignModeS.bReadOnly = true;
		m_eTextHorAlignModeS.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignModeS.SetIUIAttributeName(_T("HorizontalS"));
		m_eTextHorAlignModeS.SetPropWndName(_T("Selected"));
		m_eTextHorAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());

		m_eTextHorAlignModeD = TAH_LEFT;
		m_eTextHorAlignModeD.SetDefaultValue((_variant_t)m_eTextHorAlignModeD);
		m_eTextHorAlignModeD.vtEx = VT_EX_TEXT_ALIGN_HOR;
		m_eTextHorAlignModeD.bReadOnly = true;
		m_eTextHorAlignModeD.SetIUIElementName(_T("TextAlignment"));
		m_eTextHorAlignModeD.SetIUIAttributeName(_T("HorizontalD"));
		m_eTextHorAlignModeD.SetPropWndName(_T("Disabled"));
		m_eTextHorAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_LEFT).GetEnumString());
		m_eTextHorAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_RIGHT).GetEnumString());
		m_eTextHorAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAH_CENTER).GetEnumString());

		// Text vertical alignment
		m_eTextVerAlignModeN = TAV_CENTER;
		m_eTextVerAlignModeN.SetDefaultValue(_variant_t(m_eTextVerAlignModeN));
		m_eTextVerAlignModeN.vtEx = VT_EX_TEXT_ALIGN_VER;
		m_eTextVerAlignModeN.bReadOnly = true;
		m_eTextVerAlignModeN.bGroup = true;
		m_eTextVerAlignModeN.strGroupName = _T("Text Verizontal Align");
		m_eTextVerAlignModeN.SetIUIElementName(_T("TextAlignment"));
		m_eTextVerAlignModeN.SetIUIAttributeName(_T("VerticalN"));
		m_eTextVerAlignModeN.SetPropWndName(_T("Normal"));
		m_eTextVerAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_TOP).GetEnumString());
		m_eTextVerAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_CENTER).GetEnumString());
		m_eTextVerAlignModeN.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_BOTTOM).GetEnumString());

		m_eTextVerAlignModeH = TAV_CENTER;
		m_eTextVerAlignModeH.SetDefaultValue(_variant_t(m_eTextVerAlignModeH));
		m_eTextVerAlignModeH.vtEx = VT_EX_TEXT_ALIGN_VER;
		m_eTextVerAlignModeH.bReadOnly = true;
		m_eTextVerAlignModeH.SetIUIElementName(_T("TextAlignment"));
		m_eTextVerAlignModeH.SetIUIAttributeName(_T("VerticalH"));
		m_eTextVerAlignModeH.SetPropWndName(_T("Highlight"));
		m_eTextVerAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_TOP).GetEnumString());
		m_eTextVerAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_CENTER).GetEnumString());
		m_eTextVerAlignModeH.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_BOTTOM).GetEnumString());

		m_eTextVerAlignModeS = TAV_CENTER;
		m_eTextVerAlignModeS.SetDefaultValue(_variant_t(m_eTextVerAlignModeS));
		m_eTextVerAlignModeS.vtEx = VT_EX_TEXT_ALIGN_VER;
		m_eTextVerAlignModeS.bReadOnly = true;
		m_eTextVerAlignModeS.SetIUIElementName(_T("TextAlignment"));
		m_eTextVerAlignModeS.SetIUIAttributeName(_T("VerticalS"));
		m_eTextVerAlignModeS.SetPropWndName(_T("Selected"));
		m_eTextVerAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_TOP).GetEnumString());
		m_eTextVerAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_CENTER).GetEnumString());
		m_eTextVerAlignModeS.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_BOTTOM).GetEnumString());

		m_eTextVerAlignModeD = TAV_CENTER;
		m_eTextVerAlignModeD.SetDefaultValue(_variant_t(m_eTextVerAlignModeD));
		m_eTextVerAlignModeD.vtEx = VT_EX_TEXT_ALIGN_VER;
		m_eTextVerAlignModeD.bReadOnly = true;
		m_eTextVerAlignModeD.SetIUIElementName(_T("TextAlignment"));
		m_eTextVerAlignModeD.SetIUIAttributeName(_T("VerticalD"));
		m_eTextVerAlignModeD.SetPropWndName(_T("Disabled"));
		m_eTextVerAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_TOP).GetEnumString());
		m_eTextVerAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_CENTER).GetEnumString());
		m_eTextVerAlignModeD.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(TAV_BOTTOM).GetEnumString());

		// Padding for Text
		m_rcTextMarginN.vtEx = VT_EX_RECT;
		m_rcTextMarginN = CIUIRect(0, 0, 0, 0);
		m_rcTextMarginN.SetDefaultValue((_variant_t)m_rcTextMarginN);
		m_rcTextMarginN.bGroup = true;
		m_rcTextMarginN.SetGroupName(_T("Item Text Margin"));
		m_rcTextMarginN.SetPropWndName(_T("Normal Item Text Margin"));
		m_rcTextMarginN.SetIUIElementName(_T("ItemTextMarginN"));
		m_rcTextMarginN.nMinValue = -3840;
		m_rcTextMarginN.nMaxValue = 3840;

		m_rcTextMarginH.vtEx = VT_EX_RECT;
		m_rcTextMarginH = CIUIRect(0, 0, 0, 0);
		m_rcTextMarginH.SetDefaultValue((_variant_t)m_rcTextMarginH);
		m_rcTextMarginH.SetPropWndName(_T("Highlight Item Text Margin"));
		m_rcTextMarginH.SetIUIElementName(_T("ItemTextMarginH"));
		m_rcTextMarginH.nMinValue = -3840;
		m_rcTextMarginH.nMaxValue = 3840;

		m_rcTextMarginP.vtEx = VT_EX_RECT;
		m_rcTextMarginP = CIUIRect(0, 0, 0, 0);
		m_rcTextMarginP.SetDefaultValue((_variant_t)m_rcTextMarginP);
		m_rcTextMarginP.SetPropWndName(_T("Selected Item Text Margin"));
		m_rcTextMarginP.SetIUIElementName(_T("ItemTextMarginS"));
		m_rcTextMarginP.nMinValue = -3840;
		m_rcTextMarginP.nMaxValue = 3840;

		m_rcTextMarginD.vtEx = VT_EX_RECT;
		m_rcTextMarginD = CIUIRect(0, 0, 0, 0);
		m_rcTextMarginD.SetDefaultValue((_variant_t)m_rcTextMarginD);
		m_rcTextMarginD.SetPropWndName(_T("Disabled Item Text Margin"));
		m_rcTextMarginD.SetIUIElementName(_T("ItemTextMarginD"));
		m_rcTextMarginD.nMinValue = -3840;
		m_rcTextMarginD.nMaxValue = 3840;

		m_eFocusItemBkType = BKT_COLOR;
		m_eFocusItemBkType.SetDefaultValue((_variant_t)m_eFocusItemBkType);
		m_eFocusItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eFocusItemBkType.bGroup = true;
		m_eFocusItemBkType.SetGroupName(_T("Focus(Caret) Item Background"));
		m_eFocusItemBkType.SetIUIElementName(_T("FocusItemBackground"));
		m_eFocusItemBkType.SetPropWndName(_T("Type"));
		m_eFocusItemBkType.SetIUIAttributeName(m_eFocusItemBkType.GetPropWndName());
		m_eFocusItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eFocusItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strFocusItemBkColorResID = _T("");
		m_strFocusItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strFocusItemBkColorResID.SetIUIElementName(m_eFocusItemBkType.GetIUIElementName());
		m_strFocusItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strFocusItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strFocusItemBkImage = _T("");
		m_strFocusItemBkImage.vtEx = VT_EX_RESOURCE_COLOR;
		m_strFocusItemBkImage.SetIUIElementName(m_eFocusItemBkType.GetIUIElementName());
		m_strFocusItemBkImage.SetIUIAttributeName(_T("Image"));
		m_strFocusItemBkImage.SetPropWndName(_T("Image"));

		m_rcFocusItemBkImage9GridResize = rect;
		m_rcFocusItemBkImage9GridResize.vtEx = VT_EX_RECT;
		m_rcFocusItemBkImage9GridResize.SetIUIElementName(_T("FocusItemBkImageNineGridArg"));
		m_rcFocusItemBkImage9GridResize.SetPropWndName(_T("9 Grid Arg"));
		m_rcFocusItemBkImage9GridResize.nMinValue = 0;
		m_rcFocusItemBkImage9GridResize.nMaxValue = 1920;

		// Selected Item
		m_eSelectedItemBkType = BKT_COLOR;
		m_eSelectedItemBkType.SetDefaultValue((_variant_t)m_eSelectedItemBkType);
		m_eSelectedItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eSelectedItemBkType.bGroup = true;
		m_eSelectedItemBkType.SetGroupName(_T("Selected Item Background"));
		m_eSelectedItemBkType.SetIUIElementName(_T("SelectedItemBackground"));
		m_eSelectedItemBkType.SetPropWndName(_T("Type"));
		m_eSelectedItemBkType.SetIUIAttributeName(m_eSelectedItemBkType.GetPropWndName());
		m_eSelectedItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eSelectedItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strSelectedItemBkColorResID = _T("");
		m_strSelectedItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strSelectedItemBkColorResID.SetIUIElementName(m_eSelectedItemBkType.GetIUIElementName());
		m_strSelectedItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strSelectedItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strSelectedItemBkImageN = _T("");
		m_strSelectedItemBkImageN.vtEx = VT_EX_BITMAPPATH;
		m_strSelectedItemBkImageN.SetIUIElementName(m_eSelectedItemBkType.GetIUIElementName());
		m_strSelectedItemBkImageN.SetPropWndName(_T("NormalImage"));
		m_strSelectedItemBkImageN.SetIUIAttributeName(m_strSelectedItemBkImageN.GetPropWndName());

		m_strSelectedItemBkImageD = _T("");
		m_strSelectedItemBkImageD.vtEx = VT_EX_BITMAPPATH;
		m_strSelectedItemBkImageD.SetIUIElementName(m_eSelectedItemBkType.GetIUIElementName());
		m_strSelectedItemBkImageD.SetPropWndName(_T("DisabledImage"));
		m_strSelectedItemBkImageD.SetIUIAttributeName(m_strSelectedItemBkImageD.GetPropWndName());

		m_rcSelectedItemBkImage9GridResize = rect;
		m_rcSelectedItemBkImage9GridResize.vtEx = VT_EX_RECT;
		m_rcSelectedItemBkImage9GridResize.SetIUIElementName(_T("SelectedItemBkImageNineGridArg"));
		m_rcSelectedItemBkImage9GridResize.SetPropWndName(_T("9 Grid Arg"));
		m_rcSelectedItemBkImage9GridResize.nMinValue = 0;
		m_rcSelectedItemBkImage9GridResize.nMaxValue = 1920;

		// Highlight Item
		m_bEnableHighlightItemEffect = false;
		m_bEnableHighlightItemEffect.bGroup = true;
		m_bEnableHighlightItemEffect.SetGroupName(_T("Highlight Item Background"));
		m_bEnableHighlightItemEffect.SetIUIElementName(_T("HighlightItemBackground"));
		m_bEnableHighlightItemEffect.SetPropWndName(_T("Enable"));
		m_bEnableHighlightItemEffect.SetIUIAttributeName(m_bEnableHighlightItemEffect.GetPropWndName());

		m_eHighlightItemBkType = BKT_COLOR;
		m_eHighlightItemBkType.vtEx = VT_EX_BACKGROUND_TYPE;
		m_eHighlightItemBkType.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_eHighlightItemBkType.SetPropWndName(_T("Type"));
		m_eHighlightItemBkType.SetIUIAttributeName(m_eHighlightItemBkType.GetPropWndName());
		m_eHighlightItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_COLOR).GetEnumString());
		m_eHighlightItemBkType.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(BKT_IMAGE).GetEnumString());

		m_strHighlightItemBkColorResID = _T("");
		m_strHighlightItemBkColorResID.vtEx = VT_EX_RESOURCE_COLOR;
		m_strHighlightItemBkColorResID.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_strHighlightItemBkColorResID.SetIUIAttributeName(_T("ColorResID"));
		m_strHighlightItemBkColorResID.SetPropWndName(_T("Color ID"));

		m_strHighlightItemBkImageN = _T("");
		m_strHighlightItemBkImageN.vtEx = VT_EX_BITMAPPATH;
		m_strHighlightItemBkImageN.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_strHighlightItemBkImageN.SetPropWndName(_T("NormalImage"));
		m_strHighlightItemBkImageN.SetIUIAttributeName(m_strHighlightItemBkImageN.GetPropWndName());

		m_strHighlightItemBkImageD = _T("");
		m_strHighlightItemBkImageD.vtEx = VT_EX_BITMAPPATH;
		m_strHighlightItemBkImageD.SetIUIElementName(m_bEnableHighlightItemEffect.GetIUIElementName());
		m_strHighlightItemBkImageD.SetPropWndName(_T("NoFocusImage"));
		m_strHighlightItemBkImageD.SetIUIAttributeName(m_strHighlightItemBkImageD.GetPropWndName());

		m_rcHighlightItemBkImage9GridResize = rect;
		m_rcHighlightItemBkImage9GridResize.vtEx = VT_EX_RECT;
		m_rcHighlightItemBkImage9GridResize.SetIUIElementName(_T("HighlightItemBkImageNineGridArg"));
		m_rcHighlightItemBkImage9GridResize.SetPropWndName(_T("Resize Point"));
		m_rcHighlightItemBkImage9GridResize.nMinValue = 0;
		m_rcHighlightItemBkImage9GridResize.nMaxValue = 1920;

		m_lScrollBarWidth = (LONG)Metrics::g_nScrollBarWidth;
		m_lScrollBarWidth.bGroup = true;
		m_lScrollBarWidth.SetGroupName(_T("Scroll Bar"));
		m_lScrollBarWidth.SetDefaultValue((_variant_t)m_lScrollBarWidth);
		m_lScrollBarWidth.SetIUIElementName(_T("ScrollBarWidth"));
		m_lScrollBarWidth.SetPropWndName(_T("Scroll Bar Width"));

		m_strHorScrollbarID = _T("");
		m_strHorScrollbarID.bReadOnly = true;
		m_strHorScrollbarID.vtEx = VT_EX_STYLE_SCROLLBAR;
		m_strHorScrollbarID.SetIUIElementName(_T("HorizontalScrollbarStyleID"));
		m_strHorScrollbarID.SetPropWndName(_T("Horizontal Scrollbar"));

		m_strVerScrollbarID = _T("");
		m_strVerScrollbarID.bReadOnly = true;
		m_strVerScrollbarID.vtEx = VT_EX_STYLE_SCROLLBAR;
		m_strVerScrollbarID.SetIUIElementName(_T("VerticalScrollbarStyleID"));
		m_strVerScrollbarID.SetPropWndName(_T("Vertical Scrollbar"));
	}

	CTreeViewProp::~CTreeViewProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CHtmlViewProp::CHtmlViewProp()
	{
		m_lStructSize = (LONG)sizeof(CHtmlViewProp);
		m_eControlType = CT_HTMLVIEW;
		DisableBackgroundProperties(FALSE, TRUE);
		DisableFontProperties(FALSE);
		DisableTextColorProperties(FALSE);

		m_bHideBorder = false;
		m_bHideBorder.SetDefaultValue((_variant_t)m_bHideBorder);
		m_bHideBorder.bGroup = true;
		m_bHideBorder.SetGroupName(_T("General"));
		m_bHideBorder.SetIUIElementName(_T("IsHideBorder"));
		m_bHideBorder.SetPropWndName(_T("Hide Border"));

		m_bHideScrollBar = false;
		m_bHideScrollBar.SetDefaultValue((_variant_t)m_bHideScrollBar);
		m_bHideScrollBar.SetIUIElementName(_T("IsHideScrollBar"));
		m_bHideScrollBar.SetPropWndName(_T("Hide ScrollBar"));

		m_bHideContextMenu = false;
		m_bHideContextMenu.SetDefaultValue((_variant_t)m_bHideContextMenu);
		m_bHideContextMenu.SetIUIElementName(_T("IsHideContextMenu"));
		m_bHideContextMenu.SetPropWndName(_T("Hide Context Menu"));

		m_strStartPage = _T("");
		m_strStartPage.SetIUIElementName(_T("StartPage"));
		m_strStartPage.SetPropWndName(_T("Start Page"));
	}

	CHtmlViewProp::~CHtmlViewProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CSplitterBarProp::CSplitterBarProp()
	{
		m_lStructSize = (LONG)sizeof(CSplitterBarProp);
		m_eControlType = CT_SPLITTER;
	}

	CSplitterBarProp::~CSplitterBarProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CDateTimePickerProp::CDateTimePickerProp()
	{
		m_lStructSize = (LONG)sizeof(CDateTimePickerProp);
		m_eControlType = CT_DATETIMEPICKER;
		DisableTextColorProperties(FALSE);

		m_eFormat = DTF_SHORT_DATE;
		m_eFormat.SetDefaultValue((_variant_t)m_eFormat);
		m_eFormat.vtEx = VT_EX_DATETIME_FORMAT;
		m_eFormat.bGroup = true;
		m_eFormat.SetGroupName(_T("General"));
		m_eFormat.SetIUIElementName(_T("Format"));
		m_eFormat.SetPropWndName(_T("Format"));
		m_eFormat.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DTF_SHORT_DATE).GetEnumString());
		m_eFormat.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DTF_LONG_DATE).GetEnumString());
		m_eFormat.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(DTF_TIME).GetEnumString());

		m_bUseSpinControl = false;
		m_bUseSpinControl.SetDefaultValue((_variant_t)m_bUseSpinControl);
		m_bUseSpinControl.SetIUIElementName(_T("IsUseSpinControl"));
		m_bUseSpinControl.SetPropWndName(_T("Use Spin Control"));

		m_strSpinButtonStyleID = _T("");
		m_strSpinButtonStyleID.SetDefaultValue((_variant_t)m_strSpinButtonStyleID);
		m_strSpinButtonStyleID.bReadOnly = true;
		m_strSpinButtonStyleID.vtEx = VT_EX_STYLE_SPIN;
		m_strSpinButtonStyleID.SetIUIElementName(_T("SpinStyleID"));
		m_strSpinButtonStyleID.SetPropWndName(_T("Spin Button Style"));

		m_bShowNone = false;
		m_bShowNone.SetDefaultValue((_variant_t)m_bShowNone);
		m_bShowNone.SetIUIElementName(_T("IsShowNone"));
		m_bShowNone.SetPropWndName(_T("Show None"));

		m_bAllowEdit = false;
		m_bAllowEdit.SetDefaultValue((_variant_t)m_bAllowEdit);
		m_bAllowEdit.SetIUIElementName(_T("IsAllowEdit"));
		m_bAllowEdit.SetPropWndName(_T("Allow Edit"));

		m_lButtonWidth = (LONG)16;
		m_lButtonWidth.SetDefaultValue((_variant_t)m_lButtonWidth);
		m_lButtonWidth.SetIUIElementName(_T("ButtonWidth"));
		m_lButtonWidth.SetPropWndName(_T("Button Width"));
		m_lButtonWidth.nMinValue = 0;
		m_lButtonWidth.nMaxValue = 1920;

		m_ptTextOffset = CIUIPoint(0, 0);
		m_ptTextOffset.vtEx = VT_EX_POINT;
		m_ptTextOffset.SetIUIElementName(_T("TextOffset"));
		m_ptTextOffset.SetIUIAttributeName(_T("TextOffset"));
		m_ptTextOffset.SetPropWndName(_T("Text Offset"));
		m_ptTextOffset.nMinValue = 0;
		m_ptTextOffset.nMaxValue = 1920;
	}

	CDateTimePickerProp::~CDateTimePickerProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CScrollBarProp::CScrollBarProp()
	{
		RECT rect = { 0, 0, 0, 0 };
		m_lStructSize = (LONG)sizeof(CScrollBarProp);
		m_eControlType = CT_SCROLLBAR;

		m_rcControl.bSaveToSkinFile = m_rcControl.bShowInPropertiesWnd = false;
		m_bVisible.bSaveToSkinFile = m_bVisible.bShowInPropertiesWnd = false;
		m_bEnabled.bSaveToSkinFile = m_bEnabled.bShowInPropertiesWnd = false;
		m_bTabStop.bSaveToSkinFile = m_bTabStop.bShowInPropertiesWnd = false;
		m_eHorLayoutMode.bSaveToSkinFile = m_eHorLayoutMode.bShowInPropertiesWnd = false;
		m_eVerLayoutMode.bSaveToSkinFile = m_eVerLayoutMode.bShowInPropertiesWnd = false;
		m_strBkColorResIDCN.bShowInPropertiesWnd = m_strBkColorResIDCN.bSaveToSkinFile = false;
		m_strBkColorResIDCH.bShowInPropertiesWnd = m_strBkColorResIDCH.bSaveToSkinFile = false;
		m_strBkColorResIDCP.bShowInPropertiesWnd = m_strBkColorResIDCP.bSaveToSkinFile = false;
		m_strBkColorResIDCD.bShowInPropertiesWnd = m_strBkColorResIDCD.bSaveToSkinFile = false;
		m_strBKImageCN.bSaveToSkinFile = m_strBKImageCN.bShowInPropertiesWnd = false;
		m_strBKImageCH.bSaveToSkinFile = m_strBKImageCH.bShowInPropertiesWnd = false;
		m_strBKImageCP.bSaveToSkinFile = m_strBKImageCP.bShowInPropertiesWnd = false;
		m_strBKImageCD.bSaveToSkinFile = m_strBKImageCD.bShowInPropertiesWnd = false;
		DisableCheckedBorderColorProperties();

		DisableFontProperties(FALSE);
		DisableTextColorProperties(FALSE);
		DisableCaptionGroup();
		DisableTooltipsGroup();

		m_bCombineBackgroundImages.bGroup = true;
		m_bCombineBackgroundImages.SetGroupName(_T("Background"));
		m_bCombineBackgroundImages.SetIUIElementName(_T("IsCombineBackgroundImages"));

		// Button1 Image group
		m_bCombineButton1Image = true;
		m_bCombineButton1Image.SetDefaultValue((_variant_t)m_bCombineButton1Image);
		m_bCombineButton1Image.bGroup = true;
		m_bCombineButton1Image.SetGroupName(_T("Button1 Image"));
		m_bCombineButton1Image.SetIUIElementName(_T("IsCombineButton1Image"));
		m_bCombineButton1Image.SetPropWndName(_T("Combine Button1 Image"));

		m_strButton1Combine = _T("");
		m_strButton1Combine.vtEx = VT_EX_BITMAPPATH;
		m_strButton1Combine.SetIUIElementName(_T("Button1Combine"));
		m_strButton1Combine.SetIUIAttributeName(_T("RelativePath"));
		m_strButton1Combine.SetPropWndName(_T("Combine Button1"));

		m_strButton1ImageN = _T("");
		m_strButton1ImageN.vtEx = VT_EX_BITMAPPATH;
		m_strButton1ImageN.bDisabledWhileBindIs = true;
		m_strButton1ImageN.SetBindCompareValue(false);
		m_strButton1ImageN.SetIUIElementName(_T("Button1Normal"));
		m_strButton1ImageN.SetIUIAttributeName(_T("RelativePath"));
		m_strButton1ImageN.SetPropWndName(_T("Normal Button1"));

		m_strButton1ImageH = _T("");
		m_strButton1ImageH.vtEx = VT_EX_BITMAPPATH;
		m_strButton1ImageH.bDisabledWhileBindIs = true;
		m_strButton1ImageH.SetBindCompareValue(false);
		m_strButton1ImageH.SetIUIElementName(_T("Button1Hover"));
		m_strButton1ImageH.SetIUIAttributeName(_T("RelativePath"));
		m_strButton1ImageH.SetPropWndName(_T("Hover Button1"));

		m_strButton1ImageP = _T("");
		m_strButton1ImageP.vtEx = VT_EX_BITMAPPATH;
		m_strButton1ImageP.bDisabledWhileBindIs = true;
		m_strButton1ImageP.SetBindCompareValue(false);
		m_strButton1ImageP.SetIUIElementName(_T("Button1Pressed"));
		m_strButton1ImageP.SetIUIAttributeName(_T("RelativePath"));
		m_strButton1ImageP.SetPropWndName(_T("Pressed Button1"));

		m_strButton1ImageD = _T("");
		m_strButton1ImageD.vtEx = VT_EX_BITMAPPATH;
		m_strButton1ImageD.bDisabledWhileBindIs = true;
		m_strButton1ImageD.SetBindCompareValue(false);
		m_strButton1ImageD.SetIUIElementName(_T("Button1Disabled"));
		m_strButton1ImageD.SetIUIAttributeName(_T("RelativePath"));
		m_strButton1ImageD.SetPropWndName(_T("Disabled Button1"));

		m_rcButton1Image9GridResize.vtEx = VT_EX_RECT;
		m_rcButton1Image9GridResize = rect;
		m_rcButton1Image9GridResize.SetDefaultValue((_variant_t)m_rcButton1Image9GridResize);
		m_rcButton1Image9GridResize.SetPropWndName(_T("Button1 9 Grid Arg"));
		m_rcButton1Image9GridResize.SetIUIElementName(_T("Button1Resize"));
		m_rcButton1Image9GridResize.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcButton1Image9GridResize.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Button2 Button Image group
		m_bCombineButton2Image = true;
		m_bCombineButton2Image.SetDefaultValue((_variant_t)m_bCombineButton2Image);
		m_bCombineButton2Image.bGroup = true;
		m_bCombineButton2Image.SetGroupName(_T("Button2 Button Image"));
		m_bCombineButton2Image.SetIUIElementName(_T("IsCombineButton2Image"));
		m_bCombineButton2Image.SetPropWndName(_T("Combine Button2 Button Image"));

		m_strButton2Combine = _T("");
		m_strButton2Combine.vtEx = VT_EX_BITMAPPATH;
		m_strButton2Combine.SetIUIElementName(_T("Button2Combine"));
		m_strButton2Combine.SetIUIAttributeName(_T("RelativePath"));
		m_strButton2Combine.SetPropWndName(_T("Combine Button2 Button"));

		m_strButton2ImageN = _T("");
		m_strButton2ImageN.vtEx = VT_EX_BITMAPPATH;
		m_strButton2ImageN.bDisabledWhileBindIs = true;
		m_strButton2ImageN.SetBindCompareValue(false);
		m_strButton2ImageN.SetIUIElementName(_T("Button2Normal"));
		m_strButton2ImageN.SetIUIAttributeName(_T("RelativePath"));
		m_strButton2ImageN.SetPropWndName(_T("Normal Button2 Button"));

		m_strButton2ImageH = _T("");
		m_strButton2ImageH.vtEx = VT_EX_BITMAPPATH;
		m_strButton2ImageH.bDisabledWhileBindIs = true;
		m_strButton2ImageH.SetBindCompareValue(false);
		m_strButton2ImageH.SetIUIElementName(_T("Button2Hover"));
		m_strButton2ImageH.SetIUIAttributeName(_T("RelativePath"));
		m_strButton2ImageH.SetPropWndName(_T("Hover Button2 Button"));

		m_strButton2ImageP = _T("");
		m_strButton2ImageP.vtEx = VT_EX_BITMAPPATH;
		m_strButton2ImageP.bDisabledWhileBindIs = true;
		m_strButton2ImageP.SetBindCompareValue(false);
		m_strButton2ImageP.SetIUIElementName(_T("Button2Pressed"));
		m_strButton2ImageP.SetIUIAttributeName(_T("RelativePath"));
		m_strButton2ImageP.SetPropWndName(_T("Pressed Button2 Button"));

		m_strButton2ImageD = _T("");
		m_strButton2ImageD.vtEx = VT_EX_BITMAPPATH;
		m_strButton2ImageD.bDisabledWhileBindIs = true;
		m_strButton2ImageD.SetBindCompareValue(false);
		m_strButton2ImageD.SetIUIElementName(_T("Button2Disabled"));
		m_strButton2ImageD.SetIUIAttributeName(_T("RelativePath"));
		m_strButton2ImageD.SetPropWndName(_T("Disabled Button2 Button"));

		m_rcButton2Image9GridResize.vtEx = VT_EX_RECT;
		m_rcButton2Image9GridResize = rect;
		m_rcButton2Image9GridResize.SetDefaultValue((_variant_t)m_rcButton2Image9GridResize);
		m_rcButton2Image9GridResize.SetPropWndName(_T("Button2 Button 9 Grid Arg"));
		m_rcButton2Image9GridResize.SetIUIElementName(_T("Button2Resize"));
		m_rcButton2Image9GridResize.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcButton2Image9GridResize.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		// Thumb
		m_bCombineThumbImage = false;
		m_bCombineThumbImage.SetDefaultValue((_variant_t)m_bCombineThumbImage);
		m_bCombineThumbImage.bGroup = true;
		m_bCombineThumbImage.SetGroupName(_T("Thumb"));
		m_bCombineThumbImage.SetIUIElementName(_T("IsCombineThumbImage"));
		m_bCombineThumbImage.SetPropWndName(_T("Combine Thumb Image"));

		m_strThumbCombine = _T("");
		m_strThumbCombine.vtEx = VT_EX_BITMAPPATH;
		m_strThumbCombine.SetIUIElementName(_T("ThumbCombine"));
		m_strThumbCombine.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbCombine.SetPropWndName(_T("Combine Thumb"));

		m_strThumbN = _T("");
		m_strThumbN.vtEx = VT_EX_BITMAPPATH;
		m_strThumbN.bDisabledWhileBindIs = true;
		//    strThumbN.SetBindVariant(&bUseCustomScrollBar);
		m_strThumbN.SetBindCompareValue(false);
		m_strThumbN.SetIUIElementName(_T("ThumbNormal"));
		m_strThumbN.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbN.SetPropWndName(_T("Normal Thumb"));

		m_strThumbH = _T("");
		m_strThumbH.vtEx = VT_EX_BITMAPPATH;
		m_strThumbH.bDisabledWhileBindIs = true;
		//    strThumbH.SetBindVariant(&bUseCustomScrollBar);
		m_strThumbH.SetBindCompareValue(false);
		m_strThumbH.SetIUIElementName(_T("ThumbHighlight"));
		m_strThumbH.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbH.SetPropWndName(_T("Highlight Thumb"));

		m_strThumbP = _T("");
		m_strThumbP.vtEx = VT_EX_BITMAPPATH;
		m_strThumbP.bDisabledWhileBindIs = true;
		//    strThumbP.SetBindVariant(&bUseCustomScrollBar);
		m_strThumbP.SetBindCompareValue(false);
		m_strThumbP.SetIUIElementName(_T("ThumbSelected"));
		m_strThumbP.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbP.SetPropWndName(_T("Selected Thumb"));

		m_strThumbD = _T("");
		m_strThumbD.vtEx = VT_EX_BITMAPPATH;
		m_strThumbD.bDisabledWhileBindIs = true;
		//    strThumbD.SetBindVariant(&bUseCustomScrollBar);
		m_strThumbD.SetBindCompareValue(false);
		m_strThumbD.SetIUIElementName(_T("ThumbDisabled"));
		m_strThumbD.SetIUIAttributeName(_T("RelativePath"));
		m_strThumbD.SetPropWndName(_T("Disabled Thumb"));

		m_rcThumbResize.vtEx = VT_EX_RECT;
		m_rcThumbResize = rect;
		m_rcThumbResize.SetDefaultValue((_variant_t)m_rcThumbResize);
		m_rcThumbResize.SetPropWndName(_T("Thumb 9 Grid Arg"));
		m_rcThumbResize.SetIUIElementName(_T("ThumbResize"));
		m_rcThumbResize.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcThumbResize.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		m_strArrow1 = _T("");
		m_strArrow1.vtEx = VT_EX_BITMAPPATH;
		m_strArrow1.bGroup = TRUE;
		m_strArrow1.SetGroupName(_T("Arrow"));
		m_strArrow1.SetIUIElementName(_T("LeftArrow"));
		m_strArrow1.SetIUIAttributeName(_T("RelativePath"));
		m_strArrow1.SetPropWndName(_T("Left Arrow"));

		m_strArrow2 = _T("");
		m_strArrow2.vtEx = VT_EX_BITMAPPATH;
		m_strArrow2.SetIUIElementName(_T("RightArrow"));
		m_strArrow2.SetIUIAttributeName(_T("RelativePath"));
		m_strArrow2.SetPropWndName(_T("Right Arrow"));

		m_nButton1Width = (LONG)16;
		m_nButton1Width.SetDefaultValue((_variant_t)m_nButton1Width);
		m_nButton1Width.bGroup = true;
		m_nButton1Width.SetGroupName(_T("Scroll Bar Button"));
		m_nButton1Width.bDisabledWhileBindIs = true;
		m_nButton1Width.SetBindCompareValue(false);
		m_nButton1Width.SetIUIElementName(_T("LeftUpButtonSize"));
		m_nButton1Width.SetPropWndName(_T("Left or Up Button Size"));
		m_nButton1Width.SetDescription(_T("Width of the left button if the scroll bar is horizontal, or Height of the up button if the scroll bar is vertical"));
		m_nButton1Width.nMinValue = 0;
		m_nButton1Width.nMaxValue = 1920;

		m_nButton2Width = (LONG)16;
		m_nButton2Width.SetDefaultValue((_variant_t)m_nButton2Width);
		m_nButton2Width.bDisabledWhileBindIs = true;
		m_nButton2Width.SetBindCompareValue(false);
		m_nButton2Width.SetIUIElementName(_T("RightDownButtonSize"));
		m_nButton2Width.SetPropWndName(_T("Right or Down Button Size"));
		m_nButton2Width.SetDescription(_T("Width of the right button if the scroll bar is horizontal, or Height of the down button if the scroll bar is vertical"));
		m_nButton2Width.nMinValue = 0;
		m_nButton2Width.nMaxValue = 1920;
	}

	CScrollBarProp::~CScrollBarProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	CPanelBaseProp::CPanelBaseProp()
	{
		m_lStructSize = (LONG)sizeof(CPanelBaseProp);
		m_eControlType = CT_UNIFORM_GRID;

		m_bClipChildren.bSaveToSkinFile = m_bClipChildren.bShowInPropertiesWnd = false;
		m_strBKImageH.bSaveToSkinFile = m_strBKImageH.bShowInPropertiesWnd = false;
		m_strBKImageP.bSaveToSkinFile = m_strBKImageP.bShowInPropertiesWnd = false;
		DisableFontProperties(FALSE);
		DisableTextColorProperties(FALSE);

		m_eBkImageResizeMode.bSaveToSkinFile = m_eBkImageResizeMode.bShowInPropertiesWnd = true;

		// Only use the m_strBKN to store the image.
		m_strBKImageN.bSaveToSkinFile = m_strBKImageN.bShowInPropertiesWnd = true;
		m_strBKImageN.bGroup = true;
		m_strBKImageN.SetGroupName(_T("Image"));
	}

	CPanelBaseProp::~CPanelBaseProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CTabControlProp::CTabControlProp()
	{
		m_lStructSize = (LONG)sizeof(CTabControlProp);
		m_eControlType = CT_PNL_TABCONTROL;

		m_bMouseTransparent = true;
		m_bMouseTransparent.SetDefaultValue((_variant_t)m_bMouseTransparent);
	}

	CTabControlProp::~CTabControlProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CDockPanelProp::CDockPanelProp()
	{
		m_lStructSize = (LONG)sizeof(CDockPanelProp);
		m_eControlType = CT_PNL_DOCK;

		m_bLastChildFill = true;
		m_bLastChildFill.varC4251.strVarName = _T("m_bLastChildFill");
		m_bLastChildFill.SetDefaultValue((_variant_t)m_bLastChildFill);
		m_bLastChildFill.bGroup = true;
		m_bLastChildFill.SetGroupName(_T("General"));
		m_bLastChildFill.SetIUIElementName(_T("LastChildFill"));
		m_bLastChildFill.SetPropWndName(_T("Last Child Fill"));
		m_bLastChildFill.SetDescription(_T("最后一个加入的子控件在布局时的处理方式：\r\n\
如果控件同时设置了Layout Mode属性和DockProperty属性，并且这两种对齐方式有冲突，那么IUI的处理方式如下：\r\n\
1.最后一个控件受LastChildFill属性的影响，当LastChildFill为true时，按Layout Mode的设置对齐; 当为false时，按DockProperty的设置对齐;\r\n\
2.其它前面的控件受DockProperty的影响，与Layout Mode设置无关。"));
	}

	CDockPanelProp::~CDockPanelProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CUniformGridProp::CUniformGridProp()
	{
		m_lStructSize = (LONG)sizeof(CUniformGridProp);
		m_eControlType = CT_UNIFORM_GRID;
	}

	CUniformGridProp::~CUniformGridProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CCanvasProp::CCanvasProp()
	{
		m_lStructSize = (LONG)sizeof(CCanvasProp);
		m_eControlType = CT_PNL_CANVAS;

		m_bMouseTransparent = true;
		m_bMouseTransparent.SetDefaultValue((_variant_t)m_bMouseTransparent);
	}

	CCanvasProp::~CCanvasProp()
	{
	}

	CStackPanelProp::CStackPanelProp()
	{
		m_lStructSize = (LONG)sizeof(CStackPanelProp);
		m_eControlType = CT_PNL_STACK;

		m_bMouseTransparent = true;
		m_bMouseTransparent.SetDefaultValue((_variant_t)m_bMouseTransparent);
	}

	CStackPanelProp::~CStackPanelProp()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CStringTableProp::CStringTableProp()
	{

	}

	CStringTableProp::~CStringTableProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CStringProp::CStringProp()
	{
		m_lStructSize = (LONG)sizeof(CStringProp);
		m_eControlType = CT_RESOURCE_STRING;

		m_strStringValue = _T("");
		m_strStringValue.SetDefaultValue((_variant_t)m_strStringValue);
		m_strStringValue.SetIUIElementName(_T("String"));
		//m_strStringValue.SetPropWndName(_T("String"));

		if (NULL != m_pvControls)
		{
			delete m_pvControls;
			m_pvControls = NULL;
		}
	}

	CStringProp::~CStringProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CWindowProp::CWindowProp()
	{
		m_lStructSize = (LONG)sizeof(CWindowProp);
		m_eControlType = CT_UIWND;

		m_rcPadding4Text.bShowInPropertiesWnd = m_rcPadding4Text.bSaveToSkinFile = false;
		m_bMouseTransparent.bShowInPropertiesWnd = m_bMouseTransparent.bSaveToSkinFile = false;

		m_strBkColorResIDN.SetIUIElementName(_T("ActivatedBkColor"));
		m_strBkColorResIDN.SetPropWndName(_T("Activated Bk Color"));
		m_strBkColorResIDH.SetIUIElementName(_T("DeactivatedBkColor"));
		m_strBkColorResIDH.SetPropWndName(_T("Deactivated Bk Color"));
		m_strBkColorResIDP.bShowInPropertiesWnd = m_strBkColorResIDP.bSaveToSkinFile = false;
		m_strBkColorResIDCN.bShowInPropertiesWnd = m_strBkColorResIDCN.bSaveToSkinFile = false;
		m_strBkColorResIDCH.bShowInPropertiesWnd = m_strBkColorResIDCH.bSaveToSkinFile = false;
		m_strBkColorResIDCP.bShowInPropertiesWnd = m_strBkColorResIDCP.bSaveToSkinFile = false;
		m_strBkColorResIDCD.bShowInPropertiesWnd = m_strBkColorResIDCD.bSaveToSkinFile = false;

		m_bCombineBackgroundImages.bShowInPropertiesWnd = m_bCombineBackgroundImages.bSaveToSkinFile = false;
		m_strBKCombine.bShowInPropertiesWnd = m_strBKCombine.bSaveToSkinFile = false;
		m_strBKImageN.SetIUIElementName(_T("ActivatedBkImage"));
		m_strBKImageN.SetPropWndName(_T("Activated Bk Image"));
		m_strBKImageH.SetIUIElementName(_T("DeactivatedBkImage"));
		m_strBKImageH.SetPropWndName(_T("Deactivated Bk Image"));
		m_strBKImageP.bShowInPropertiesWnd = m_strBKImageP.bSaveToSkinFile = false;
		m_strBKImageCN.bShowInPropertiesWnd = m_strBKImageCN.bSaveToSkinFile = false;
		m_strBKImageCN.bShowInPropertiesWnd = m_strBKImageCN.bSaveToSkinFile = false;
		m_strBKImageCH.bShowInPropertiesWnd = m_strBKImageCH.bSaveToSkinFile = false;
		m_strBKImageCP.bShowInPropertiesWnd = m_strBKImageCP.bSaveToSkinFile = false;
		m_strBKImageCD.bShowInPropertiesWnd = m_strBKImageCD.bSaveToSkinFile = false;

		m_strBorderColorResIDN.SetIUIElementName(_T("ActivatedBorderColor"));
		m_strBorderColorResIDN.SetPropWndName(_T("Activated Border Color"));
		m_strBorderColorResIDH.SetIUIElementName(_T("DeactivatedBorderColor"));
		m_strBorderColorResIDH.SetPropWndName(_T("Deactivated Border Color"));
		m_strBorderColorResIDP.bShowInPropertiesWnd = m_strBorderColorResIDP.bSaveToSkinFile = false;
		DisableCheckedBorderColorProperties();

		m_bWindow = true;
		m_bWindow.bShowInPropertiesWnd = m_bWindow.bSaveToSkinFile = false;

		DisableCaptionGroup();
		DisableFontProperties(NULL);
		DisableTooltipsGroup();

		m_lTitlebarHeight = (LONG)(-1);
		m_lTitlebarHeight.SetDefaultValue(_variant_t(m_lTitlebarHeight));

		m_bNoActivate = false;
		m_bNoActivate.SetDefaultValue((_variant_t)m_bNoActivate);
		m_bNoActivate.bGroup = TRUE;
		m_bNoActivate.SetGroupName(_T("General"));
		m_bNoActivate.SetIUIElementName(_T("IsNoActivate"));
		m_bNoActivate.SetPropWndName(_T("No Activate"));
		m_bNoActivate.SetDescription(_T("Specifies that the window does not activate when the user clicks it."));

		m_bClipSiblings = true;
		m_bClipSiblings.SetDefaultValue((_variant_t)m_bClipSiblings);
		m_bClipSiblings.SetIUIElementName(_T("IsClipSiblings"));
		m_bClipSiblings.SetPropWndName(_T("Clip Siblings"));

		m_bAppWindow = true;
		m_bAppWindow.SetDefaultValue((_variant_t)m_bAppWindow);
		m_bAppWindow.SetIUIElementName(_T("IsAppWindow"));
		m_bAppWindow.SetPropWndName(_T("Application Window"));
		m_bAppWindow.SetDescription(_T("Specifies that the window, when visible, will have an entry in the taskbar."));

		m_bLayeredWindow = false;
		m_bLayeredWindow.SetDefaultValue((_variant_t)m_bLayeredWindow);
		m_bLayeredWindow.SetIUIElementName(_T("IsLayeredWindow"));
		m_bLayeredWindow.SetPropWndName(_T("Layered Window"));
		m_bLayeredWindow.SetDescription(_T("Specifies that the window is a layered window."));

		m_bAlpha = false;
		m_bAlpha.SetDefaultValue((_variant_t)m_bAlpha);
		m_bAlpha.SetIUIElementName(_T("IsAlpha"));
		m_bAlpha.SetPropWndName(_T("Alpha"));
		m_bAlpha.SetDescription(_T("Use Alpha to determine the opacity of the layered window."));

		m_btAlphaValue = (LONG)255;
		m_btAlphaValue.SetDefaultValue((_variant_t)m_btAlphaValue);
		m_btAlphaValue.SetIUIElementName(_T("Alpha"));
		m_btAlphaValue.SetPropWndName(_T("Alpha Value"));
		m_btAlphaValue.SetDescription(_T("Alpha value used to describe the opacity of the layered window.When Alpha is 0, the window is completely transparent. When Alpha is 255, the window is opaque."));
		m_btAlphaValue.nMinValue = 0;
		m_btAlphaValue.nMaxValue = 255;

		m_bUpdateLayeredWindow = false;
		m_bUpdateLayeredWindow.SetDefaultValue((_variant_t)m_bUpdateLayeredWindow);
		m_bUpdateLayeredWindow.SetIUIElementName(_T("IsUpdateLayeredWindow"));
		m_bUpdateLayeredWindow.SetPropWndName(_T("UpdateLayeredWindow"));
		m_bUpdateLayeredWindow.SetDescription(_T("Call 'UpdateLayeredWindow' to draw layered window."));

		m_eAlphaFormat = AF_AC_SRC_ALPHA;
		m_eAlphaFormat.SetDefaultValue((_variant_t)m_eAlphaFormat);
		m_eAlphaFormat.vtEx = VT_EX_ALPHA_FORMAT;
		m_eAlphaFormat.SetIUIElementName(_T("AlphaFormat"));
		m_eAlphaFormat.SetPropWndName(_T("Alpha Format"));
		m_eAlphaFormat.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(AF_NONE).GetEnumString());
		m_eAlphaFormat.varC4251.lstStringOptions.push_back((LPCTSTR)UIVARIANT(AF_AC_SRC_ALPHA).GetEnumString());

		m_bTopmost = false;
		m_bTopmost.SetDefaultValue((_variant_t)m_bTopmost);
		m_bTopmost.SetIUIElementName(_T("IsTopmost"));
		m_bTopmost.SetPropWndName(_T("Topmost"));
		m_bTopmost.SetDescription(_T("Specifies that the window will take z-order precedence over all non-topmost windows."));

		m_bAcceptFiles = false;
		m_bAcceptFiles.SetDefaultValue((_variant_t)m_bAcceptFiles);
		m_bAcceptFiles.SetIUIElementName(_T("IsAcceptFiles"));
		m_bAcceptFiles.SetPropWndName(_T("Accept Files"));

		m_bToolWindow = false;
		m_bToolWindow.SetDefaultValue((_variant_t)m_bToolWindow);
		m_bToolWindow.SetIUIElementName(_T("IsToolWindow"));
		m_bToolWindow.SetPropWndName(_T("Tool Window"));

		m_eWindowType = WT_NORMAL;
		m_eWindowType.SetDefaultValue((_variant_t)m_eWindowType);
		m_eWindowType.vtEx = VT_EX_WINDOW_TYPE;
		m_eWindowType.bReadOnly = true;
		m_eWindowType.SetIUIElementName(_T("WindowType"));
		m_eWindowType.SetPropWndName(_T("Window Type"));
		m_eWindowType.AddStringOption(UIVARIANT(WT_NORMAL).GetEnumString());
		m_eWindowType.AddStringOption(UIVARIANT(WT_TOOLTIPS).GetEnumString());

		m_bResizingBorder = false;
		m_bResizingBorder.bGroup = true;
		m_bResizingBorder.SetGroupName(_T("Border"));
		m_bResizingBorder.SetDefaultValue((_variant_t)m_bResizingBorder);
		m_bResizingBorder.SetIUIElementName(_T("IsResizingBorder"));
		m_bResizingBorder.SetPropWndName(_T("Resizing Border"));

		m_bTrueResizingBorder = false;
		m_bTrueResizingBorder.SetDefaultValue((_variant_t)m_bTrueResizingBorder);
		m_bTrueResizingBorder.SetIUIElementName(_T("TrueResizingBorder"));
		m_bTrueResizingBorder.SetPropWndName(_T("True Resizing Border"));
		m_bTrueResizingBorder.SetDescription(_T("是否使用真正的窗口边框（即为窗口增加WS_THICKFRAME风格）"));

		m_strShadowBorder = _T("");
		m_strShadowBorder.vtEx = VT_EX_BITMAPPATH;
		m_strShadowBorder.SetDefaultValue((_variant_t)m_strShadowBorder);
		m_strShadowBorder.SetVarName(_T("m_strShadowBorder"));
		m_strShadowBorder.SetIUIElementName(_T("ShadowWindow"));
		m_strShadowBorder.SetIUIAttributeName(_T("ShadowBorderImage"));
		m_strShadowBorder.SetPropWndName(_T("Shadow Border Image"));

		RECT rect = { 0, 0, 0, 0 };
		m_rcShadowThinkness.vtEx = VT_EX_RECT;
		m_rcShadowThinkness = rect;
		m_rcShadowThinkness.SetDefaultValue((_variant_t)m_rcShadowThinkness);
		m_rcShadowThinkness.SetIUIElementName(_T("ShadowWindow"));
		m_rcShadowThinkness.SetIUIAttributeName(_T("ShadowThinkness"));
		m_rcShadowThinkness.SetPropWndName(_T("Shadow Thinkness"));
		m_rcShadowThinkness.SetDescription(_T("阴影的厚度"));
		m_rcShadowThinkness.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcShadowThinkness.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;

		m_bTransparentShadow = true;
		m_bTransparentShadow.SetDefaultValue((_variant_t)m_bTransparentShadow);
		m_bTransparentShadow.SetIUIElementName(_T("ShadowWindow"));
		m_bTransparentShadow.SetIUIAttributeName(_T("TransparentShadow"));
		m_bTransparentShadow.SetPropWndName(_T("Transparent Shadow"));
		m_bTransparentShadow.SetDescription(_T("鼠标是否直接穿透阴影区域（相当于为阴影窗口加上WS_EX_TRANSPARENT扩展风格）"));

		m_bResizingShadow = true;
		m_bResizingShadow.SetDefaultValue((_variant_t)m_bResizingShadow);
		m_bResizingShadow.SetIUIElementName(_T("ShadowWindow"));
		m_bResizingShadow.SetIUIAttributeName(_T("ResizingShadow"));
		m_bResizingShadow.SetPropWndName(_T("Resizing Shadow"));
		m_bResizingShadow.SetDescription(_T("是否可以通过鼠标拖动边框阴影来改变窗口大小。 仅在(m_bResizingBorder && !m_bTrueResizingBorder && !m_bTransparentShadow)时有效"));
	}

	CWindowProp::~CWindowProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	CToolTipCtrlProp::CToolTipCtrlProp()
	{
		m_lStructSize = (LONG)sizeof(CToolTipCtrlProp);
		m_eControlType = CT_STYLE_TOOLTIPS;

		RECT rect = { 0, 0, 0, 0 };
		m_rcTextMargin.vtEx = VT_EX_RECT;
		m_rcTextMargin = rect;
		m_rcTextMargin.SetDefaultValue((_variant_t)m_rcTextMargin);
		m_rcTextMargin.SetPropWndName(_T("ToolTips Text Margin"));
		m_rcTextMargin.SetIUIElementName(_T("ToolTipsTextMargin"));
		m_rcTextMargin.nMinValue = SCREEN_POSITION_HORIZONTAL_MIN;
		m_rcTextMargin.nMaxValue = SCREEN_POSITION_HORIZONTAL_MAX;
	}

	CToolTipCtrlProp::~CToolTipCtrlProp()
	{

	}

	//////////////////////////////////////////////////////////////////////////

	CFontProp::CFontProp()
	{
		m_lStructSize = (LONG)sizeof(CFontProp);
		m_eControlType = CT_RESOURCE_FONT;

		m_logFont.vtEx = VT_EX_FONT;
		m_logFont.bNeedToConvert = false;
		m_logFont.SetIUIElementName(_T("Font"));
		m_logFont.SetPropWndName(_T("Font(Size)"));
	}

	CFontProp::~CFontProp()
	{
	}

	bool CFontProp::IsFontResource() const
	{
		if (this == NULL)
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	CColorProp::CColorProp()
	{
		m_lStructSize = (LONG)sizeof(CColorProp);
		m_eControlType = CT_RESOURCE_COLOR;

		m_crColor = (LONG)RGB(0, 0, 0);
		m_crColor.SetDefaultValue((_variant_t)m_crColor);
		m_crColor.vtEx = VT_EX_COLOR;
		m_crColor.SetIUIElementName(_T("Color"));
		m_crColor.SetPropWndName(_T("Color"));
	}

	CColorProp::~CColorProp()
	{
	}

} // namespace IUI

#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


CProjectFilter::CProjectFilter(
	CProjectFilter *pParentFilter,
	FILTER_TYPE eFilterType,
	CIUIProject *pProject)
	: m_pParentFilter(pParentFilter)
	, m_eFilterType(eFilterType)
	, m_pIUIProject(pProject)
{
	m_strFilter = _T("");
}

CProjectFilter::~CProjectFilter()
{
	std::vector<CProjectFilter *>::iterator it = m_vFilterItem.begin();
	for (; it != m_vFilterItem.end(); ++it)
	{
		CProjectFilter *pFilter = *it;

		delete pFilter;
	}
	m_vFilterItem.clear();

	std::vector<CControlProp *>::iterator itItem = m_vProjectItem.begin();
	for (; itItem != m_vProjectItem.end(); ++itItem)
	{
		CControlProp *pProp = *itItem;

		if (pProp->GetClassName() == IUI_WINDOW)
		{
			CUIParse::FreeUIWNDPROPERTIES((CWindowProp *)pProp);
		}
		else
		{
			delete pProp;
		}
	}
	m_vProjectItem.clear();
}

FILTER_TYPE CProjectFilter::GetFilterType() const
{
	return m_eFilterType;
}

CIUIString CProjectFilter::GetFilterName() const
{
	return m_strFilter;
}

int CProjectFilter::SetFilterName(LPCTSTR lpszFilter)
{
	if (lpszFilter == NULL)
	{
		return -1;
	}

	m_strFilter = lpszFilter;

	return 0;
}

CProjectFilter *CProjectFilter::AddFilter(LPCTSTR lpszFilter)
{
	if (lpszFilter == NULL)
	{
		return NULL;
	}

	CProjectFilter *pNewFilter = new CProjectFilter(this, FILTER_TYPE_FILTER, m_pIUIProject);
	pNewFilter->m_strFilter = lpszFilter;
	m_vFilterItem.push_back(pNewFilter);

	return pNewFilter;
}

int CProjectFilter::DeleteFilter(LPCTSTR lpszFilter, BOOL bDeleteProjectItem)
{
	CProjectFilter *pFilterRet = NULL;

	std::vector<CProjectFilter *>::iterator it = m_vFilterItem.begin();
	for (; it != m_vFilterItem.end(); ++it)
	{
		CProjectFilter *pFilter = *it;

		if (pFilter->m_strFilter == lpszFilter)
		{
			pFilterRet = pFilter;
			break;
		}
	}

	if (pFilterRet == NULL)
	{
		return -1;
	}

	// 如果不删除Filter中的Item，把它们加到本Filter中。
	if (!bDeleteProjectItem)
	{
		m_vFilterItem.insert(m_vFilterItem.end(),
			pFilterRet->m_vFilterItem.begin(), pFilterRet->m_vFilterItem.end());

		pFilterRet->m_vFilterItem.clear();

		m_vProjectItem.insert(m_vProjectItem.end(),
			pFilterRet->m_vProjectItem.begin(), pFilterRet->m_vProjectItem.end());
		pFilterRet->m_vProjectItem.clear();
	}

	m_vFilterItem.erase(it);
	delete pFilterRet;
	pFilterRet = NULL;

	return 0;
}

int CProjectFilter::RenameFilter(LPCTSTR lpszOldFilter, LPCTSTR lpszNewFilter)
{
	CProjectFilter *pFilter = FindFilter(lpszOldFilter);

	if (pFilter == NULL)
	{
		return -1;
	}

	pFilter->m_strFilter = lpszNewFilter;

	return 0;
}

int CProjectFilter::GetFilterCount() const
{
	int nCount = (int)m_vFilterItem.size();

	return nCount;
}

CProjectFilter *CProjectFilter::FindFilter(LPCTSTR lpszFilter)
{
	CProjectFilter *pFilterRet = NULL;

	int nCount = (int)m_vFilterItem.size();

	for (int i = 0; i < nCount; ++i)
	{
		CProjectFilter *pFilter = m_vFilterItem[i];

		if (pFilter->m_strFilter == lpszFilter)
		{
			pFilterRet = pFilter;
			break;
		}
	}

	return pFilterRet;
}

CProjectFilter *CProjectFilter::GetFilter(int nIndex)
{
	int nCount = (int)m_vFilterItem.size();

	if (nIndex < 0 || nIndex >= nCount)
	{
		return NULL;
	}

	return m_vFilterItem[nIndex];
}

const CProjectFilter *CProjectFilter::GetFilter(int nIndex) const
{
	int nCount = (int)m_vFilterItem.size();

	if (nIndex < 0 || nIndex >= nCount)
	{
		return NULL;
	}

	return m_vFilterItem[nIndex];
}

int CProjectFilter::AddWindow(LPCTSTR lpszItemPath)
{
	if (lpszItemPath == NULL)
	{
		return -1;
	}

	// 转成绝对路径
	BOOL bIsAbsolutePath = FALSE;
	int nLen = (int)_tcslen(lpszItemPath);
	int i = 0;

	for (i = 0; i < nLen; ++i)
	{
		if (lpszItemPath[i] == ':')
		{
			bIsAbsolutePath = TRUE;
			break;
		}
	}

	CIUIString strPath = lpszItemPath;
	if (!bIsAbsolutePath)
	{
		strPath = m_pIUIProject->GetProjectPathWithoutTitle() + lpszItemPath;
	}

	CWindowProp *pUIWndProp = new CWindowProp;
	pUIWndProp->SetFileName(strPath);
	m_vProjectItem.push_back(pUIWndProp);

	return GetProjectItemCount() - 1;
}

int CProjectFilter::AddProjectItem(CControlProp *pItemProp)
{
	if (pItemProp == NULL)
	{
		return -1;
	}

	m_vProjectItem.push_back(pItemProp);

	return GetProjectItemCount() - 1;
}

int CProjectFilter::DeleteProjectItem(LPCTSTR lpszItemPath)
{
	if (lpszItemPath == NULL)
	{
		return -1;
	}

	BOOL bDeleted = FALSE;

	int nCount = (int)m_vProjectItem.size();

	for (int i = 0; i < nCount; ++i)
	{
		const CControlProp *pItem = m_vProjectItem[i];

		if (CIUIString(pItem->GetFileName()).CompareNoCase(lpszItemPath) == 0)
		{
			m_vProjectItem.erase(m_vProjectItem.begin() + i);
			break;
		}
	}

	return bDeleted ? 0 : -2;
}

CControlProp *CProjectFilter::GetProjectItem(int nIndex)
{
	int nCount = (int)m_vProjectItem.size();

	if (nIndex < 0 || nIndex >= nCount)
	{
		return NULL;
	}

	return m_vProjectItem[nIndex];
}

const CControlProp *CProjectFilter::GetProjectItem(int nIndex) const
{
	int nCount = (int)m_vProjectItem.size();

	if (nIndex < 0 || nIndex >= nCount)
	{
		return NULL;
	}

	return m_vProjectItem[nIndex];
}

int CProjectFilter::GetProjectItemCount() const
{
	int nCount = (int)m_vProjectItem.size();

	return nCount;
}

CControlProp *CProjectFilter::FindProjectItem(LPCTSTR lpszItemPath)
{
	if (lpszItemPath == NULL)
	{
		return NULL;
	}

	CControlProp *pRet = NULL;

	// 如果lpszItemPath不包含.xml后缀，则加上。
	CIUIString strItem = lpszItemPath;
	if (strItem.Right(4).CompareNoCase(_T(".xml")) != 0)
	{
		strItem += _T(".xml");
	}

	std::vector<CControlProp *>::iterator itItem = m_vProjectItem.begin();
	for (; itItem != m_vProjectItem.end(); ++itItem)
	{
		CControlProp *pItem = *itItem;

		if (pItem != NULL && pItem->GetFileName().CompareNoCase(strItem) == 0)
		{
			pRet = pItem;
			break;
		}
	}

	// 本级没找到，从子Filter中继续查找
	if (pRet == NULL)
	{
		std::vector<CProjectFilter *>::iterator itFilter = m_vFilterItem.begin();
		for (; itFilter != m_vFilterItem.end(); ++itFilter)
		{
			CProjectFilter *pFilter = *itFilter;
			_ASSERT(pFilter != NULL);

			pRet = pFilter->FindProjectItem(lpszItemPath);
			if (pRet != NULL)
			{
				break;
			}
		}
	}


	return pRet;
}

int CProjectFilter::MoveProjectItem(LPCTSTR lpszItemPath, LPCTSTR lpszFromFilter, LPCTSTR lpszToFilter)
{
	if (lpszItemPath == NULL || lpszFromFilter == NULL || lpszToFilter == NULL)
	{
		return -1;
	}

	CProjectFilter *pFromFilter = FindFilter(lpszFromFilter);

	if (pFromFilter == NULL)
	{
		return -2;
	}

	CControlProp *pFoundItem = pFromFilter->FindProjectItem(lpszItemPath);

	if (pFoundItem == NULL)
	{
		return -3;
	}

	CProjectFilter *pToFilter = FindFilter(lpszToFilter);

	if (pToFilter == NULL)
	{
		return -4;
	}

	pFromFilter->DeleteProjectItem(lpszItemPath);
	pToFilter->AddProjectItem(pFoundItem);

	return 0;
}

int CProjectFilter::GetProjectItemList(std::vector<CControlProp *> *pvProjectItems)
{
	if (pvProjectItems == NULL)
	{
		return -1;
	}

	// 递归遍历所有子孙
	int nFilterCount = GetFilterCount();

	for (int i = 0; i < nFilterCount; ++i)
	{
		CProjectFilter *pChildFilter = GetFilter(i);
		_ASSERT(pChildFilter != NULL);

		pChildFilter->GetProjectItemList(pvProjectItems);
	}

	int nFileCount = m_vProjectItem.size();

	for (int j = 0; j < nFileCount; ++j)
	{
		pvProjectItems->push_back(m_vProjectItem[j]);
	}

	return 0;
}

int CProjectFilter::GetProjectItemList(std::vector<const CControlProp *> *pvProjectItems) const
{
	if (pvProjectItems == NULL)
	{
		return -1;
	}

	// 递归遍历所有子孙
	int nFilterCount = GetFilterCount();

	for (int i = 0; i < nFilterCount; ++i)
	{
		const CProjectFilter *pChildFilter = GetFilter(i);
		_ASSERT(pChildFilter != NULL);

		pChildFilter->GetProjectItemList(pvProjectItems);
	}

	int nFileCount = m_vProjectItem.size();

	for (int j = 0; j < nFileCount; ++j)
	{
		pvProjectItems->push_back(m_vProjectItem[j]);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CIUIProject::CIUIProject(void)
	: m_bLoadSkinFromResource(FALSE)
	, m_strUIResourceType(_T("IUISKIN"))
	, m_strXMLResourceID(_T("XMLResource.h"))
	, m_RootProjectFilter(NULL, FILTER_TYPE_ROOT, this)
	, m_pDefFontProp(NULL)
	, m_pDefColorProp(NULL)
{
}


CIUIProject::~CIUIProject(void)
{
	Release();
}

/**
 @brief 解析<ProjectItem> 标签
 @details
 <pre>
 </pre>
 @author (Liusw)
 @param[in]
 @param[out]
 @return
  - 0  成功
  - -1 失败
*/
int LoadItem(CIUIProject *pProject, LPCTSTR lpszProjectPath, XML::IXMLDOMElementPtr pItem,
	CProjectFilter *pParentFilter, CIUIString *pXMLName)
{
	if (pProject == NULL || lpszProjectPath == NULL || pItem == NULL || pParentFilter == NULL)
	{
		return -1;
	}

#ifdef _DEBUG
	BSTR bstr;
	pItem->get_nodeName(&bstr);
	CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	_ASSERT(strValue == _T("ProjectItem"));
#endif

	_variant_t varItem = pItem->getAttribute(_T("Path"));
	if (pXMLName != NULL)
	{
		*pXMLName = (LPCTSTR)_bstr_t(varItem);
	}

	// 把XML都加载成Window，即使它的根是布局，这样是为了方便统一处理
	CWindowProp *pProp = CUIParse::AllocUIWNDPROPERTIES();
	CUIParse uiParse;
	uiParse.SetProject(pProject);
	uiParse.SetWindowProp(pProp);
	uiParse.GetWindowProp()->SetFileName((LPCTSTR)_bstr_t(varItem));
	int nRet = uiParse.SetUIPath(CIUIString(lpszProjectPath) + (LPCTSTR)_bstr_t(varItem));
	if (nRet != 0)
	{
		CUIParse::FreeUIWNDPROPERTIES(pProp);
		pProp = NULL;
		return nRet;
	}

	return pParentFilter->AddProjectItem(pProp);
}

/**
 @brief 递归加载Filter标签
 @details Project标签也是个Filter标签，所以加载Project也是用LoadFilter
 <pre>
 </pre>
 @author (Liusw)
 @param[in]
 @param[out]
 @return
  - 0  成功
  - -1 失败
*/
int LoadFilter(CIUIProject *pOwner, LPCTSTR lpszProjectPath,
	XML::IXMLDOMElementPtr pProject, CProjectFilter *pProjectFilter)
{
	if (pOwner == NULL
		|| lpszProjectPath == NULL
		|| pProject == NULL
		|| pProjectFilter == NULL)
	{
		return -1;
	}

#ifdef _DEBUG
	BSTR bstr;
	pProject->get_nodeName(&bstr);
	CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	_ASSERT(strValue == _T("Windows") || strValue == _T("Filter"));
#endif

	// Child filter or project item
	XML::IXMLDOMNode *pFilterOrItem = NULL;
	HRESULT hr = pProject->get_firstChild(&pFilterOrItem);

	while (!FAILED(hr) && pFilterOrItem != NULL)
	{
		BSTR bstr;
		pFilterOrItem->get_nodeName(&bstr);
		CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

		if (bstr != NULL)
		{
			SysFreeString(bstr);
		}

		if (strValue == _T("Filter"))
		{
			XML::IXMLDOMElementPtr pItem = pFilterOrItem;
			_variant_t varFilter = pItem->getAttribute(_T("Name"));
			CProjectFilter *pNewFilter = pProjectFilter->AddFilter(_bstr_t(varFilter));
			LoadFilter(pOwner, lpszProjectPath, pFilterOrItem, pNewFilter);
		}
		else if (strValue == _T("ProjectItem"))
		{
			CIUIString strXml;
			int nRet = LoadItem(pOwner, lpszProjectPath, pFilterOrItem, pProjectFilter, &strXml);
			if (nRet < 0)
			{
				CIUIString str;
				str.Format(_T("加载皮肤‘%s’失败，错误码：%d"), strXml, nRet);
				IUITRACE(str);
			}
		}
		else
		{
			_ASSERT(FALSE);
		}

		hr = pFilterOrItem->get_nextSibling(&pFilterOrItem);
	}

	return 0;
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
				CONTROL_TYPE eControlType = GetControlTypeEnum((LPCTSTR)_bstr_t(bstr)); \
				if (bstr != NULL) \
					SysFreeString(bstr); \
				StructPropName *pStyleProp = new StructPropName(); \
				pStyleProp->m_eControlType = eControlType; \
				LoadResourceStyleProperties(pChildNode, pStyleProp); \
				vpStyle.push_back(pStyleProp); \
				hr = pChildNode->get_nextSibling(&pChildNode); \
			} \
		} \
	}

int CIUIProject::Load(LPCTSTR lpszProjectFile)
{
	if (lpszProjectFile == NULL)
	{
		return -1;
	}

	if (!PathFileExists(lpszProjectFile))
	{
		return -2;
	}

	//
	m_strProjectFile = lpszProjectFile;

	XML::IXMLDOMDocumentPtr xmlDoc(__uuidof(DOMDocument));
	xmlDoc->async = FALSE;
	HRESULT hr = xmlDoc->load(lpszProjectFile);

	XML::IXMLDOMElementPtr pRootElement = xmlDoc->GetdocumentElement();

	if (pRootElement == NULL)
	{
		return -3;
	}

	// Load
	BSTR bstr;
	pRootElement->get_nodeName(&bstr);
	CIUIString strValue = (LPCTSTR)_bstr_t(bstr);

	if (bstr != NULL)
	{
		SysFreeString(bstr);
	}

	if (strValue == _T("Project"))
	{
		XML::IXMLDOMElementPtr pWindows = pRootElement->selectSingleNode("Windows");
		LoadFilter(this, GetProjectPathWithoutTitle(), pWindows, &m_RootProjectFilter);

		XML::IXMLDOMElementPtr pResources = pRootElement->selectSingleNode("Resources");
		if (pResources != NULL)
		{
			XML::IXMLDOMElementPtr pFonts = pResources->selectSingleNode("Fonts");
			CFontParse fp;
			fp.SetProject(this);
			fp.Load(pFonts, &m_pDefFontProp, &m_vpResFont);

			XML::IXMLDOMElementPtr pColors = pResources->selectSingleNode("Colors");
			CColorParse cp;
			cp.SetProject(this);
			cp.Load(pColors, &m_pDefColorProp, &m_vpResColor);
		}

		// Load Styles, the old version of iui file may be not include 'Styles' node.
		XML::IXMLDOMElementPtr pStyles = pRootElement->selectSingleNode("Styles");
		if (pStyles == NULL)
		{
			IUITRACE(_T("[IUI]===Not found 'Styles' node.\n"));
		}
		else
		{
			LOAD_STYLE("PushButtons", m_vpStylePushButton, CButtonProp)
			LOAD_STYLE("CheckBoxes", m_vpStyleCheckBox, CCheckBoxProp)
			LOAD_STYLE("RadioButtons", m_vpStyleRadioButton, CRadioButtonProp)
			LOAD_STYLE("ComboBoxes", m_vpStyleComboBox, CComboBoxProp)
			LOAD_STYLE("Statics", m_vpStyleStatic, CLabelProp)
			LOAD_STYLE("Edits", m_vpStyleRichEdit, CRichTextBoxProp)
			LOAD_STYLE("Sliders", m_vpStyleSlider, CSliderProp)
			LOAD_STYLE("Progresses", m_vpStyleProgress, CProgressBarProp)
			LOAD_STYLE("Spins", m_vpStyleSpin, CSpinButtonProp)
			//LOAD_STYLE("HotKeys", m_vpStyleHotKey, HOTPROPERTIES)
			//LOAD_STYLE("IPAdresses", m_vpStyleIPAddress, IPAPROPERTIES)
			LOAD_STYLE("ListControls", m_vpStyleListCtrl, CListViewProp)
			LOAD_STYLE("TreeControls", m_vpStyleTreeCtrl, CTreeViewProp)
			//LOAD_STYLE("TreeLists", m_vpStyleTreeList, TREELISTPROPERTIES)
			LOAD_STYLE("HtmlControls", m_vpStyleHtmlCtrl, CHtmlViewProp)
			LOAD_STYLE("DateTimes", m_vpStyleDateTime, CDateTimePickerProp)
			//LOAD_STYLE("Menus", m_vpStyleMenu, MENUPROPERTIES)
			//LOAD_STYLE("Menubars", m_vpStyleMenuBar, MENUBARPROPERTIES)
			LOAD_STYLE("Headers", m_vpStyleHeader, CColumnHeaderProp)
			LOAD_STYLE("HorScrollbars", m_vpStyleScrollBar, CScrollBarProp)
			LOAD_STYLE("ToolTips", m_vpStyleToolTips, CToolTipCtrlProp)
			//LOAD_STYLE("Splitterbars", m_vpStyleSplitter, SPLITTERPROPERTIES)
			//LOAD_STYLE("StackPanel", m_vpStylePanelStack, STACKPNLPROPERTIES)
			//LOAD_STYLE("WindowlessLine", m_vpStyleWLLine, LINEWLPROPERTIES)
			//LOAD_STYLE("WindowlessPicture", m_vpStyleWLPic, PICWLPROPERTIES)
		}

	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}

#define RELEASE_STYLE(vVar, type) \
	if (!(vVar).empty()) \
	{\
		std::vector<type *>::iterator it = (vVar).begin();\
		for (; it != (vVar).end(); ++it)\
		{\
			type *pProp = *it;\
			delete pProp;\
		}\
		(vVar).clear();\
	}

int CIUIProject::Release()
{
	delete m_pDefFontProp;
	m_pDefFontProp = NULL;

	{
		std::vector<CFontProp *>::iterator itFont = m_vpResFont.begin();
		for (; itFont != m_vpResFont.end(); ++itFont)
		{
			CFontProp *pFontProp = *itFont;

			delete pFontProp;
		}
		m_vpResFont.clear();
	}

	delete m_pDefColorProp;
	m_pDefColorProp = NULL;
	{
		std::vector<CColorProp *>::iterator itColor = m_vpResColor.begin();
		for (; itColor != m_vpResColor.end(); ++itColor)
		{
			CColorProp *pColorProp = *itColor;

			delete pColorProp;
		}
		m_vpResColor.clear();
	}

	RELEASE_STYLE(m_vpStylePushButton, CButtonProp)
	RELEASE_STYLE(m_vpStyleCheckBox, CCheckBoxProp)
	RELEASE_STYLE(m_vpStyleRadioButton, CRadioButtonProp)
	RELEASE_STYLE(m_vpStyleComboBox, CComboBoxProp)
	RELEASE_STYLE(m_vpStyleStatic, CLabelProp)
	RELEASE_STYLE(m_vpStyleRichEdit, CRichTextBoxProp)
	RELEASE_STYLE(m_vpStyleSlider, CSliderProp)
	RELEASE_STYLE(m_vpStyleProgress, CProgressBarProp)
	RELEASE_STYLE(m_vpStyleSpin, CSpinButtonProp)
	//RELEASE_STYLE(m_vpStyleHotKey, HOTPROPERTIES)
	//RELEASE_STYLE(m_vpStyleIPAddress, IPAPROPERTIES)
	RELEASE_STYLE(m_vpStyleListCtrl, CListViewProp)
	RELEASE_STYLE(m_vpStyleTreeCtrl, CTreeViewProp)
	//RELEASE_STYLE(m_vpStyleTreeList, TREELISTPROPERTIES)
	RELEASE_STYLE(m_vpStyleHtmlCtrl, CHtmlViewProp)
	RELEASE_STYLE(m_vpStyleDateTime, CDateTimePickerProp)
	//RELEASE_STYLE(m_vpStyleMenu, MENUPROPERTIES)
	//RELEASE_STYLE(m_vpStyleMenuBar, MENUBARPROPERTIES)
	RELEASE_STYLE(m_vpStyleHeader, CColumnHeaderProp)
	RELEASE_STYLE(m_vpStyleScrollBar, CScrollBarProp)
	RELEASE_STYLE(m_vpStyleToolTips, CToolTipCtrlProp)
	//RELEASE_STYLE(m_vpStyleSplitter, SPLITTERPROPERTIES)
	//RELEASE_STYLE(m_vpStylePanelStack, STACKPNLPROPERTIES)
	//RELEASE_STYLE(m_vpStyleWLLine, LINEWLPROPERTIES)
	//RELEASE_STYLE(m_vpStyleWLPic, PICWLPROPERTIES)

	return 0;
}

int CIUIProject::SetProjectFile(LPCTSTR lpszProjectFile)
{
	m_strProjectFile = lpszProjectFile;

	return 0;
}

CIUIString CIUIProject::GetProjectPath() const
{
	return m_strProjectFile;
}

CIUIString CIUIProject::GetProjectPathWithoutTitle() const
{
	CIUIString str = m_strProjectFile;

	LPTSTR lpszPath = str.GetBufferSetLength(MAX_PATH);
	PathRemoveFileSpec(lpszPath);
	PathAddBackslash(lpszPath);
	str.ReleaseBuffer();

	return str;
}

int CIUIProject::AddProjectItem(LPCTSTR lpszFilter, LPCTSTR lpszItemPath)
{
	return 0;
}

int CIUIProject::AddProjectItem(LPCTSTR lpszFilter, CControlProp *pItemProp)
{
	if (lpszFilter == NULL || pItemProp == NULL)
	{
		return -1;
	}

	CProjectFilter *pFiter = GetFilter(lpszFilter);
	if (pFiter == NULL)
	{
		return -2;
	}

	return pFiter->AddProjectItem(pItemProp);
}

int CIUIProject::DeleteProjectItem(LPCTSTR lpszFilter, LPCTSTR lpszItemPath)
{
	CProjectFilter *pFilter = GetFilter(lpszFilter);
	if (pFilter == NULL)
	{
		return -1;
	}

	return pFilter->DeleteProjectItem(lpszItemPath);
}

int CIUIProject::DeleteFilter(LPCTSTR lpszFilter, BOOL bDeleteProjectItem)
{
	return 0;
}

int CIUIProject::GetProjectItemCount() const
{
	return 0;
}

int CIUIProject::MoveProjectItem(
	LPCTSTR lpszItemPath,
	LPCTSTR lpszFromFilter,
	LPCTSTR lpszToFilter)
{
	return 0;
}

// 递归遍历filter
CProjectFilter *IUI::FindFilter(CProjectFilter *pParent, LPCTSTR lpszFilter)
{
	if (pParent == NULL)
	{
		return NULL;
	}

	if (lpszFilter == NULL)
	{
		if (pParent->GetFilterType() == FILTER_TYPE_ROOT)
		{
			return pParent;
		}
		else
		{
			return NULL;
		}
	}

	CProjectFilter *pFilterRet = NULL;

	// 先找自己
	int nFilterCount = pParent->GetFilterCount();

	for (int i = 0; i < nFilterCount; ++i)
	{
		CProjectFilter *pFilter = pParent->GetFilter(i);
		_ASSERT(pFilter->GetFilterType() == FILTER_TYPE_FILTER);

		if (pFilter->GetFilterName() == lpszFilter)
		{
			pFilterRet = pFilter;
			break;
		}

		// 如果没找到，从孩子中继续找
		if (pFilterRet == NULL)
		{
			CProjectFilter *pFind = FindFilter(pFilter, lpszFilter);

			if (pFind != NULL)
			{
				pFilterRet = pFind;
				break;
			}
		}
	}

	return pFilterRet;
}

CProjectFilter *CIUIProject::GetFilter(LPCTSTR lpszFilter)
{
	if (lpszFilter == NULL)
	{
		return &m_RootProjectFilter;
	}

	return FindFilter(&m_RootProjectFilter, lpszFilter);
}

CProjectFilter *CIUIProject::GetRootFilter()
{
	return &m_RootProjectFilter;
}

CControlProp *CIUIProject::FindProjectItem(LPCTSTR lpszWindowFileName)
{
	return m_RootProjectFilter.FindProjectItem(lpszWindowFileName);
}

BOOL CIUIProject::SetLoadSkinFromResource()
{
	BOOL bOld = m_bLoadSkinFromResource;
	m_bLoadSkinFromResource = true;
	return bOld;
}

BOOL CIUIProject::IsLoadSkinFromResource()
{
	return m_bLoadSkinFromResource;
}

CIUIString CIUIProject::GetUIResourceType()
{
	return m_strUIResourceType;
}

CIUIString CIUIProject::GetXMLResourceID()
{
	return m_strXMLResourceID;
}

CImageMgr *CIUIProject::GetImageMgr()
{
	return &m_ImageMgr;
}

CFontMgr *CIUIProject::GetFontMgr()
{
	return &m_FontMgr;
}

CFontProp *CIUIProject::GetDefaultFontProp()
{
	return m_pDefFontProp;
}

std::vector<CFontProp *> *CIUIProject::GetResFonts()
{
	return &m_vpResFont;
}

CFontProp *CIUIProject::GetFontProp(LPCTSTR lpszResID)
{
	CFontProp *pRet = NULL;

	std::vector<CFontProp *>::iterator it = m_vpResFont.begin();
	for (; it != m_vpResFont.end(); ++it)
	{
		CIUIString strName = (LPCTSTR)(*it)->m_strName;
		if (strName == lpszResID)
		{
			pRet = *it;
			break;
		}
	}

	return pRet;
}

CColorProp *CIUIProject::GetDefaultColorProp()
{
	return m_pDefColorProp;
}

std::vector<CColorProp *> *CIUIProject::GetResColors()
{
	return &m_vpResColor;
}

CColorProp *CIUIProject::GetColorProp(LPCTSTR lpszResID)
{
	CColorProp *pRet = NULL;

	std::vector<CColorProp *>::iterator it = m_vpResColor.begin();
	for (; it != m_vpResColor.end(); ++it)
	{
		CIUIString strName = (LPCTSTR)(*it)->m_strName;
		if (strName == lpszResID)
		{
			pRet = *it;
			break;
		}
	}

	return pRet;
}

std::list<CStringProp> *CIUIProject::GetResStrings()
{
	return &m_lstResStrings;
}

COLORREF CIUIProject::GetColor(LPCTSTR lpszColorResID)
{
	COLORREF cr = 0;
	CColorProp *pColorProp = GetColorProp(lpszColorResID);
	if (pColorProp == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			cr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		cr = pColorProp->m_crColor;
	}

	return cr;
}

int CIUIProject::GetControlBkColor(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBkColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlBkColor2(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBkColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strBkColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 1) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlBkColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBkColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDH = (LPCTSTR)pCtrlProp->m_strBkColorResIDH;
	if (strIDH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropH = GetColorProp(strIDH);
		*(pcr + 1) = pColorPropH->m_crColor;
	}

	CIUIString strIDP = (LPCTSTR)pCtrlProp->m_strBkColorResIDP;
	if (strIDP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropP = GetColorProp(strIDP);
		*(pcr + 2) = pColorPropP->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strBkColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 3) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlCheckedBkColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropCN = GetColorProp((LPCTSTR)pCtrlProp->m_strBkColorResIDCN);

	if (pColorPropCN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropCN->m_crColor;
	}

	CIUIString strIDCH = (LPCTSTR)pCtrlProp->m_strBkColorResIDCH;
	if (strIDCH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropH = GetColorProp(strIDCH);
		*(pcr + 1) = pColorPropH->m_crColor;
	}

	CIUIString strIDCP = (LPCTSTR)pCtrlProp->m_strBkColorResIDCP;
	if (strIDCP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCP = GetColorProp(strIDCP);
		*(pcr + 2) = pColorPropCP->m_crColor;
	}

	CIUIString strIDCD = (LPCTSTR)pCtrlProp->m_strBkColorResIDCD;
	if (strIDCD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCD = GetColorProp(strIDCD);
		*(pcr + 3) = pColorPropCD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlBorderColor(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBorderColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlBorderColor2(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBorderColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strBorderColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 1) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlBorderColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strBorderColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDH = (LPCTSTR)pCtrlProp->m_strBorderColorResIDH;
	if (strIDH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropH = GetColorProp(strIDH);
		*(pcr + 1) = pColorPropH->m_crColor;
	}

	CIUIString strIDP = (LPCTSTR)pCtrlProp->m_strBorderColorResIDP;
	if (strIDP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropP = GetColorProp(strIDP);
		*(pcr + 2) = pColorPropP->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strBorderColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 3) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlCheckedBorderColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropCN = GetColorProp((LPCTSTR)pCtrlProp->m_strBorderColorResIDCN);

	if (pColorPropCN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropCN->m_crColor;
	}

	CIUIString strIDCH = (LPCTSTR)pCtrlProp->m_strBorderColorResIDCH;
	if (strIDCH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCH = GetColorProp(strIDCH);
		*(pcr + 1) = pColorPropCH->m_crColor;
	}

	CIUIString strIDCP = (LPCTSTR)pCtrlProp->m_strBorderColorResIDCP;
	if (strIDCP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCP = GetColorProp(strIDCP);
		*(pcr + 2) = pColorPropCP->m_crColor;
	}

	CIUIString strIDCD = (LPCTSTR)pCtrlProp->m_strBorderColorResIDCD;
	if (strIDCD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCD = GetColorProp(strIDCD);
		*(pcr + 3) = pColorPropCD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlTextColor(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strTextColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlTextColor2(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strTextColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strTextColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 1) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlTextColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropN = GetColorProp((LPCTSTR)pCtrlProp->m_strTextColorResIDN);

	if (pColorPropN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropN->m_crColor;
	}

	CIUIString strIDH = (LPCTSTR)pCtrlProp->m_strTextColorResIDH;
	if (strIDH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropH = GetColorProp(strIDH);
		*(pcr + 1) = pColorPropH->m_crColor;
	}

	CIUIString strIDP = (LPCTSTR)pCtrlProp->m_strTextColorResIDP;
	if (strIDP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropP = GetColorProp(strIDP);
		*(pcr + 2) = pColorPropP->m_crColor;
	}

	CIUIString strIDD = (LPCTSTR)pCtrlProp->m_strTextColorResIDD;
	if (strIDD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropD = GetColorProp(strIDD);
		*(pcr + 3) = pColorPropD->m_crColor;
	}

	return 0;
}

int CIUIProject::GetControlCheckedTextColor4(const CControlProp *pCtrlProp, COLORREF *pcr)
{
	CColorProp *pColorPropCN = GetColorProp((LPCTSTR)pCtrlProp->m_strTextColorResIDCN);

	if (pColorPropCN == NULL)
	{
		// Default color
		CColorProp *pResDefaultColor = GetDefaultColorProp();
		if (pResDefaultColor != NULL)
		{
			*pcr = pResDefaultColor->m_crColor;
		}
	}
	else
	{
		*pcr = pColorPropCN->m_crColor;
	}

	CIUIString strIDCH = (LPCTSTR)pCtrlProp->m_strTextColorResIDCH;
	if (strIDCH.IsEmpty())
	{
		*(pcr + 1) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCH = GetColorProp(strIDCH);
		*(pcr + 1) = pColorPropCH->m_crColor;
	}

	CIUIString strIDCP = (LPCTSTR)pCtrlProp->m_strTextColorResIDCP;
	if (strIDCP.IsEmpty())
	{
		*(pcr + 2) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCP = GetColorProp(strIDCP);
		*(pcr + 2) = pColorPropCP->m_crColor;
	}

	CIUIString strIDCD = (LPCTSTR)pCtrlProp->m_strTextColorResIDCD;
	if (strIDCD.IsEmpty())
	{
		*(pcr + 3) = *pcr;
	}
	else
	{
		CColorProp *pColorPropCD = GetColorProp(strIDCD);
		*(pcr + 3) = pColorPropCD->m_crColor;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Style

#define BEGIN_GET_STYLE_COUNT(eStyleType, eStyle, vpStyle) \
	if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		return int(nCount); \
	}

#define GET_STYLE_COUNT(eStyleType, eStyle, vpStyle) \
	else if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		return int(nCount); \
	}

int CIUIProject::GetStyleCount(STYLE_TYPE eStyleType)
{
	BEGIN_GET_STYLE_COUNT(eStyleType, STYLET_PUSHBUTTON, m_vpStylePushButton)
	GET_STYLE_COUNT(eStyleType, STYLET_CHECK, m_vpStyleCheckBox)
	GET_STYLE_COUNT(eStyleType, STYLET_RADIO, m_vpStyleRadioButton)
	GET_STYLE_COUNT(eStyleType, STYLET_COMBOBOX, m_vpStyleComboBox)
	GET_STYLE_COUNT(eStyleType, STYLET_LABEL, m_vpStyleStatic)
	GET_STYLE_COUNT(eStyleType, STYLET_RICHEDIT, m_vpStyleRichEdit)
	GET_STYLE_COUNT(eStyleType, STYLET_SLIDER, m_vpStyleSlider)
	GET_STYLE_COUNT(eStyleType, STYLET_PROGRESS, m_vpStyleProgress)
	GET_STYLE_COUNT(eStyleType, STYLET_SPIN, m_vpStyleSpin)
	//GET_STYLE_COUNT(eStyleType, STYLET_HOTKEY, m_vpStyleHotKey)
	//GET_STYLE_COUNT(eStyleType, STYLET_IPADDRESS, m_vpStyleIPAddress)
	GET_STYLE_COUNT(eStyleType, STYLET_LISTCTRL, m_vpStyleListCtrl)
	GET_STYLE_COUNT(eStyleType, STYLET_TREECTRL, m_vpStyleTreeCtrl)
	//GET_STYLE_COUNT(eStyleType, STYLET_TREELIST, m_vpStyleTreeList)
	GET_STYLE_COUNT(eStyleType, STYLET_HTMLCTRL, m_vpStyleHtmlCtrl)
	GET_STYLE_COUNT(eStyleType, STYLET_DATETIMECTRL, m_vpStyleDateTime)
	//GET_STYLE_COUNT(eStyleType, STYLET_MENU, m_vpStyleMenu)
	//GET_STYLE_COUNT(eStyleType, STYLET_MENUBAR, m_vpStyleMenuBar)
	GET_STYLE_COUNT(eStyleType, STYLET_HEADERCTRL, m_vpStyleHeader)
	GET_STYLE_COUNT(eStyleType, STYLET_SCROLLBAR, m_vpStyleScrollBar)
	GET_STYLE_COUNT(eStyleType, STYLET_TOOLTIPS, m_vpStyleToolTips)
	//GET_STYLE_COUNT(eStyleType, STYLET_SPLITTER, m_vpStyleSplitter)
	//GET_STYLE_COUNT(eStyleType, STYLET_PANEL_STACK, m_vpStylePanelStack)
	//GET_STYLE_COUNT(eStyleType, STYLET_WL_LINE, m_vpStyleWLLine)
	//GET_STYLE_COUNT(eStyleType, STYLET_WL_PICTURE, m_vpStyleWLPic)

	_ASSERT(FALSE);
	return -1;
}

#define BEGIN_ADD_STYLE_ITEM(eStyleType, pStyle, eStyle, ctrlType, vpStyle, StylePropName) \
	if (eStyleType == eStyle) \
	{ \
		if ((CONTROL_TYPE)pStyle->m_eControlType != ctrlType) \
		{ \
			_ASSERT(FALSE); \
			return -1; \
		} \
		vpStyle.push_back((StylePropName *)pStyle); \
		size_t nCount = vpStyle.size(); \
		return int(nCount - 1); \
	}

#define ADD_STYLE_ITEM(eStyleType, pStyle, eStyle, ctrlType, vpStyle, StylePropName) \
	else if (eStyleType == eStyle) \
	{ \
		if ((CONTROL_TYPE)pStyle->m_eControlType != ctrlType) \
		{ \
			_ASSERT(FALSE); \
			return -1; \
		} \
		vpStyle.push_back((StylePropName *)pStyle); \
		size_t nCount = vpStyle.size(); \
		return int(nCount - 1); \
	}

int CIUIProject::AddStyleItem(STYLE_TYPE eStyleType, CControlProp *pStyle)
{
	BEGIN_ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_PUSHBUTTON, CT_STYLE_PUSHBUTTON, m_vpStylePushButton, CButtonProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_CHECK, CT_STYLE_CHECK, m_vpStyleCheckBox, CCheckBoxProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_RADIO, CT_STYLE_RADIO, m_vpStyleRadioButton, CRadioButtonProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_COMBOBOX, CT_STYLE_COMBOBOX, m_vpStyleComboBox, CComboBoxProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_LABEL, CT_STYLE_STATIC, m_vpStyleStatic, CLabelProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_RICHEDIT, CT_STYLE_RICHEDIT, m_vpStyleRichEdit, CRichTextBoxProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_SLIDER, CT_STYLE_SLIDER, m_vpStyleSlider, CSliderProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_PROGRESS, CT_STYLE_PROGRESS, m_vpStyleProgress, CProgressBarProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_SPIN, CT_STYLE_SPIN, m_vpStyleSpin, CSpinButtonProp)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_HOTKEY, CT_STYLE_HOTKEY, m_vpStyleHotKey, HOTPROPERTIES)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_IPADDRESS, CT_STYLE_IPADDRESS, m_vpStyleIPAddress, IPAPROPERTIES)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_LISTCTRL, CT_STYLE_LISTCTRL, m_vpStyleListCtrl, CListViewProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_TREECTRL, CT_STYLE_TREECTRL, m_vpStyleTreeCtrl, CTreeViewProp)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_TREELIST, CT_STYLE_TREELIST, m_vpStyleTreeList, TREELISTPROPERTIES)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_HTMLCTRL, CT_STYLE_HTMLCTRL, m_vpStyleHtmlCtrl, CHtmlViewProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_DATETIMECTRL, CT_STYLE_DATETIMECTRL, m_vpStyleDateTime, CDateTimePickerProp)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_MENU, CT_STYLE_MENU, m_vpStyleMenu, MENUPROPERTIES)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_MENUBAR, CT_STYLE_MENUBAR, m_vpStyleMenuBar, MENUBARPROPERTIES)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_HEADERCTRL, CT_STYLE_HEADERCTRL, m_vpStyleHeader, CColumnHeaderProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_SCROLLBAR, CT_STYLE_SCROLLBAR, m_vpStyleScrollBar, CScrollBarProp)
	ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_TOOLTIPS, CT_STYLE_TOOLTIPS, m_vpStyleToolTips, CToolTipCtrlProp)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_SPLITTER, CT_STYLE_SPLITTER, m_vpStyleSplitter, SPLITTERPROPERTIES)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_PANEL_STACK, CT_STYLE_PANEL_STACK, m_vpStylePanelStack, STACKPNLPROPERTIES)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_WL_LINE, CT_STYLE_WL_LINE, m_vpStyleWLLine, LINEWLPROPERTIES)
	//ADD_STYLE_ITEM(eStyleType, pStyle, STYLET_WL_PICTURE, CT_STYLE_WL_PICTURE, m_vpStyleWLPic, PICWLPROPERTIES)

	_ASSERT(FALSE);
	return -1;
}

#define BEGIN_DELETE_STYLE_ITEM(eStyleType, nIndex, eStyle, vpStyle, StylePropName) \
	if (eStyleType == eStyle) \
	{ \
		int nCount = (int)vpStyle.size(); \
		if (nIndex < 0 || nIndex >= nCount) \
			return -1; \
		StylePropName *pStyleProp = vpStyle[nIndex]; \
		delete pStyleProp; \
		vpStyle.erase(vpStyle.begin() + nIndex); \
		return 0; \
	}
#define DELETE_STYLE_ITEM(eStyleType, nIndex, eStyle, vpStyle, StylePropName) \
	else if (eStyleType == eStyle) \
	{ \
		int nCount = (int)vpStyle.size(); \
		if (nIndex < 0 || nIndex >= nCount) \
			return -1; \
		StylePropName *pStyleProp = vpStyle[nIndex]; \
		delete pStyleProp; \
		vpStyle.erase(vpStyle.begin() + nIndex); \
		return 0; \
	}

int CIUIProject::DeleteStyleItem(STYLE_TYPE eStyleType, int nIndex)
{
	BEGIN_DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_PUSHBUTTON, m_vpStylePushButton, CButtonProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_CHECK, m_vpStyleCheckBox, CCheckBoxProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_RADIO, m_vpStyleRadioButton, CRadioButtonProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_COMBOBOX, m_vpStyleComboBox, CComboBoxProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_LABEL, m_vpStyleStatic, CLabelProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_RICHEDIT, m_vpStyleRichEdit, CRichTextBoxProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_SLIDER, m_vpStyleSlider, CSliderProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_PROGRESS, m_vpStyleProgress, CProgressBarProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_SPIN, m_vpStyleSpin, CSpinButtonProp)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_HOTKEY, m_vpStyleHotKey, HOTPROPERTIES)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_IPADDRESS, m_vpStyleIPAddress, IPAPROPERTIES)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_LISTCTRL, m_vpStyleListCtrl, CListViewProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_TREECTRL, m_vpStyleTreeCtrl, CTreeViewProp)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_TREELIST, m_vpStyleTreeList, TREELISTPROPERTIES)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_HTMLCTRL, m_vpStyleHtmlCtrl, CHtmlViewProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_DATETIMECTRL, m_vpStyleDateTime, CDateTimePickerProp)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_MENU, m_vpStyleMenu, MENUPROPERTIES)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_MENUBAR, m_vpStyleMenuBar, MENUBARPROPERTIES)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_HEADERCTRL, m_vpStyleHeader, CColumnHeaderProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_SCROLLBAR, m_vpStyleScrollBar, CScrollBarProp)
	DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_TOOLTIPS, m_vpStyleToolTips, CToolTipCtrlProp)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_SPLITTER, m_vpStyleSplitter, SPLITTERPROPERTIES)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_PANEL_STACK, m_vpStylePanelStack, STACKPNLPROPERTIES)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_WL_LINE, m_vpStyleWLLine, LINEWLPROPERTIES)
	//DELETE_STYLE_ITEM(eStyleType, nIndex, STYLET_WL_PICTURE, m_vpStyleWLPic, PICWLPROPERTIES)

	_ASSERT(FALSE);
	return -1;
}

#define BEGIN_DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, eStyle, vpStyle, StylePropName) \
	if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		for (size_t i=0; i<nCount; ++i) \
		{ \
			StylePropName *p = vpStyle[i]; \
			if (p == pStyleProp) \
			{ \
				delete p; \
				vpStyle.erase(vpStyle.begin() + i); \
				break; \
			} \
		} \
	}

#define DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, eStyle, vpStyle, StylePropName) \
	else if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		for (size_t i=0; i<nCount; ++i) \
		{ \
			StylePropName *p = vpStyle[i]; \
			if (p == pStyleProp) \
			{ \
				delete p; \
				vpStyle.erase(vpStyle.begin() + i); \
				break; \
			} \
		} \
	}

#define END_DELETE_STYLE_ITEM_PROP() \
	else \
	{ \
		_ASSERT(FALSE); \
	}

int CIUIProject::DeleteStyleItem(STYLE_TYPE eStyleType, const CControlProp *pStyleProp)
{
	BEGIN_DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_PUSHBUTTON, m_vpStylePushButton, CButtonProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_CHECK, m_vpStyleCheckBox, CCheckBoxProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_RADIO, m_vpStyleRadioButton, CRadioButtonProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_COMBOBOX, m_vpStyleComboBox, CComboBoxProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_LABEL, m_vpStyleStatic, CLabelProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_RICHEDIT, m_vpStyleRichEdit, CRichTextBoxProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_SLIDER, m_vpStyleSlider, CSliderProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_PROGRESS, m_vpStyleProgress, CProgressBarProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_SPIN, m_vpStyleSpin, CSpinButtonProp)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_HOTKEY, m_vpStyleHotKey, HOTPROPERTIES)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_IPADDRESS, m_vpStyleIPAddress, IPAPROPERTIES)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_LISTCTRL, m_vpStyleListCtrl, CListViewProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_TREECTRL, m_vpStyleTreeCtrl, CTreeViewProp)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_TREELIST, m_vpStyleTreeList, TREELISTPROPERTIES)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_HTMLCTRL, m_vpStyleHtmlCtrl, CHtmlViewProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_DATETIMECTRL, m_vpStyleDateTime, CDateTimePickerProp)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_MENU, m_vpStyleMenu, MENUPROPERTIES)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_MENUBAR, m_vpStyleMenuBar, MENUBARPROPERTIES)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_HEADERCTRL, m_vpStyleHeader, CColumnHeaderProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_SCROLLBAR, m_vpStyleScrollBar, CScrollBarProp)
	DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_TOOLTIPS, m_vpStyleToolTips, CToolTipCtrlProp)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_SPLITTER, m_vpStyleSplitter, SPLITTERPROPERTIES)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_PANEL_STACK, m_vpStylePanelStack, STACKPNLPROPERTIES)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_WL_LINE, m_vpStyleWLLine, LINEWLPROPERTIES)
	//DELETE_STYLE_ITEM_PROP(eStyleType, pStyleProp, STYLET_WL_PICTURE, m_vpStyleWLPic, PICWLPROPERTIES)
	END_DELETE_STYLE_ITEM_PROP()

	return 0;
}

#define BEGIN_GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, eStyle, vpStyle) \
	if (eStyleType == eStyle) \
	{ \
		int nCount = (int)vpStyle.size(); \
		if (nIndex < 0 || nIndex >= nCount) \
			return NULL; \
		return vpStyle[nIndex]; \
	}

#define GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, eStyle, vpStyle) \
	else if (eStyleType == eStyle) \
	{ \
		int nCount = (int)vpStyle.size(); \
		if (nIndex < 0 || nIndex >= nCount) \
			return NULL; \
		return vpStyle[nIndex]; \
	}

CControlProp *CIUIProject::GetStyleItem(STYLE_TYPE eStyleType, int nIndex)
{
	BEGIN_GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_PUSHBUTTON, m_vpStylePushButton)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_CHECK, m_vpStyleCheckBox)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_RADIO, m_vpStyleRadioButton)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_COMBOBOX, m_vpStyleComboBox)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_LABEL, m_vpStyleStatic)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_RICHEDIT, m_vpStyleRichEdit)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_SLIDER, m_vpStyleSlider)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_PROGRESS, m_vpStyleProgress)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_SPIN, m_vpStyleSpin)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_HOTKEY, m_vpStyleHotKey)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_IPADDRESS, m_vpStyleIPAddress)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_LISTCTRL, m_vpStyleListCtrl)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_TREECTRL, m_vpStyleTreeCtrl)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_TREELIST, m_vpStyleTreeList)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_HTMLCTRL, m_vpStyleHtmlCtrl)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_DATETIMECTRL, m_vpStyleDateTime)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_MENU, m_vpStyleMenu)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_MENUBAR, m_vpStyleMenuBar)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_HEADERCTRL, m_vpStyleHeader)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_SCROLLBAR, m_vpStyleScrollBar)
	GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_TOOLTIPS, m_vpStyleToolTips)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_SPLITTER, m_vpStyleSplitter)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_PANEL_STACK, m_vpStylePanelStack)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_WL_LINE, m_vpStyleWLLine)
	//GET_STYLE_ITEM_BY_INDEX(eStyleType, nIndex, STYLET_WL_PICTURE, m_vpStyleWLPic)

	_ASSERT(FALSE);
	return NULL;
}

#define BEGIN_GET_STYLE_ITEM(eStyleType, lpszStyleID, eStyle, vpStyle) \
	if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		for (size_t i=0; i<nCount; ++i) \
		{ \
			CControlProp *pStyleProp = vpStyle[i]; \
			if (pStyleProp->m_strName == lpszStyleID) \
			{ \
				return pStyleProp; \
			} \
		} \
	}

#define GET_STYLE_ITEM(eStyleType, lpszStyleID, eStyle, vpStyle) \
	else if (eStyleType == eStyle) \
	{ \
		size_t nCount = vpStyle.size(); \
		for (size_t i=0; i<nCount; ++i) \
		{ \
			CControlProp *pStyleProp = vpStyle[i]; \
			if (pStyleProp->m_strName == lpszStyleID) \
			{ \
				return pStyleProp; \
			} \
		} \
	}
#define END_GET_SYTLE_ITME() \
	else \
	{\
		_ASSERT(FALSE); \
	}

CControlProp *CIUIProject::GetStyleItem(STYLE_TYPE eStyleType, LPCTSTR lpszStyleID)
{
	BEGIN_GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_PUSHBUTTON, m_vpStylePushButton)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_CHECK, m_vpStyleCheckBox)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_RADIO, m_vpStyleRadioButton)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_COMBOBOX, m_vpStyleComboBox)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_LABEL, m_vpStyleStatic)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_RICHEDIT, m_vpStyleRichEdit)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_SLIDER, m_vpStyleSlider)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_PROGRESS, m_vpStyleProgress)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_SPIN, m_vpStyleSpin)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_HOTKEY, m_vpStyleHotKey)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_IPADDRESS, m_vpStyleIPAddress)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_LISTCTRL, m_vpStyleListCtrl)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_TREECTRL, m_vpStyleTreeCtrl)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_TREELIST, m_vpStyleTreeList)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_HTMLCTRL, m_vpStyleHtmlCtrl)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_DATETIMECTRL, m_vpStyleDateTime)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_MENU, m_vpStyleMenu)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_MENUBAR, m_vpStyleMenuBar)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_HEADERCTRL, m_vpStyleHeader)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_SCROLLBAR, m_vpStyleScrollBar)
	GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_TOOLTIPS, m_vpStyleToolTips)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_SPLITTER, m_vpStyleSplitter)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_PANEL_STACK, m_vpStylePanelStack)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_WL_LINE, m_vpStyleWLLine)
	//GET_STYLE_ITEM(eStyleType, lpszStyleID, STYLET_WL_PICTURE, m_vpStyleWLPic)
	END_GET_SYTLE_ITME()

	return NULL;
}

std::vector<CButtonProp *> *CIUIProject::GetStylePushButton()
{
	return &m_vpStylePushButton;
}

std::vector<CCheckBoxProp *> *CIUIProject::GetStyleCheckBox()
{
	return &m_vpStyleCheckBox;
}

std::vector<CRadioButtonProp *> *CIUIProject::GetStyleRadioButton()
{
	return &m_vpStyleRadioButton;
}

std::vector<CComboBoxProp *> *CIUIProject::GetStyleComboBox()
{
	return &m_vpStyleComboBox;
}

std::vector<CLabelProp *> *CIUIProject::GetStyleStatic()
{
	return &m_vpStyleStatic;
}

std::vector<CRichTextBoxProp *> *CIUIProject::GetStyleEdit()
{
	return &m_vpStyleRichEdit;
}

std::vector<CSliderProp *> *CIUIProject::GetStyleSlider()
{
	return &m_vpStyleSlider;
}

std::vector<CProgressBarProp *> *CIUIProject::GetStyleProgress()
{
	return &m_vpStyleProgress;
}

std::vector<CSpinButtonProp *> *CIUIProject::GetStyleSpin()
{
	return &m_vpStyleSpin;
}

//std::vector<HOTPROPERTIES *> *CIUIProject::GetStyleHotKey()
//{
//	return &m_vpStyleHotKey;
//}
//
//std::vector<IPAPROPERTIES *> *CIUIProject::GetStyleIPAddress()
//{
//	return &m_vpStyleIPAddress;
//}

std::vector<CListViewProp *> *CIUIProject::GetStyleListCtrl()
{
	return &m_vpStyleListCtrl;
}

std::vector<CTreeViewProp *> *CIUIProject::GetStyleTreeCtrl()
{
	return &m_vpStyleTreeCtrl;
}

//std::vector<TREELISTPROPERTIES *> *CIUIProject::GetStyleTreeList()
//{
//	return &m_vpStyleTreeList;
//}

std::vector<CHtmlViewProp *> *CIUIProject::GetStyleHtmlCtrl()
{
	return &m_vpStyleHtmlCtrl;
}

std::vector<CDateTimePickerProp *> *CIUIProject::GetStyleDateTime()
{
	return &m_vpStyleDateTime;
}

//std::vector<MENUPROPERTIES *> *CIUIProject::GetStyleMenu()
//{
//	return &m_vpStyleMenu;
//}
//
//std::vector<MENUBARPROPERTIES *> *CIUIProject::GetStyleMenuBar()
//{
//	return &m_vpStyleMenuBar;
//}

std::vector<CColumnHeaderProp *> *CIUIProject::GetStyleHeader()
{
	return &m_vpStyleHeader;
}

std::vector<CScrollBarProp *> *CIUIProject::GetStyleScrollBar()
{
	return &m_vpStyleScrollBar;
}

std::vector<CToolTipCtrlProp *> *CIUIProject::GetStyleToolTips()
{
	return &m_vpStyleToolTips;
}

//std::vector<SPLITTERPROPERTIES *> *CIUIProject::GetStyleSplitter()
//{
//	return &m_vpStyleSplitter;
//}
//
//std::vector<STACKPNLPROPERTIES *> *CIUIProject::GetStylePanelStack()
//{
//	return &m_vpStylePanelStack;
//}
//
//std::vector<LINEWLPROPERTIES *> *CIUIProject::GetStyleWLLine()
//{
//	return &m_vpStyleWLLine;
//}
//
//std::vector<PICWLPROPERTIES *> *CIUIProject::GetStyleWLPic()
//{
//	return &m_vpStyleWLPic;
//}

int CIUIProject::SwitchLanguage(LPCTSTR lpszLanguageFile)
{
	CParseLanguageFile plf(this);
	plf.SetLanguageFile(lpszLanguageFile, &m_mapContents);

	return 0;
}

CIUIStringW CIUIProject::FindControlContent(LPCTSTR lpszKey, BOOL *pbFound) const
{
	CIUIStringW strRet;
	std::map<std::wstring, std::wstring>::const_iterator it = m_mapContents.find(lpszKey);
	if (it == m_mapContents.end())
	{
		if (NULL != pbFound)
		{
			*pbFound = FALSE;
		}
	}
	else
	{
		if (NULL != pbFound)
		{
			*pbFound = TRUE;
		}

		strRet = it->second.c_str();
	}

	return strRet;
}

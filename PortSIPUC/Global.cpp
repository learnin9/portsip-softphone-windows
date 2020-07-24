#include "StdAfx.h"
#include <strsafe.h>
#include "global.h"
#include "OleVideoCanvasHandler.h"
#include "OleFileCanvasHandler.h"
#include "OleAudioCanvasHandler.h"
#include "OleImageCanvasHandler.h"
#include "PopupVideoWnd.h"
#include "Main/InfoWnd.h"
#include "System/SystemEx.h"
#include "TrayMsgWnd.h"
#include "Main/MsgHistoryWnd.h"
#include "Login/LoginLoadingDlg.h"
using namespace PortGo;
#include <algorithm>

HWND PortGo::g_hMainFrame = NULL;
CMsgHistoryWnd *PortGo::g_pMsgHistoryWnd = NULL;
CLoginLoadingDlg* g_pWmdLoginLoading = NULL;
//HWLWND PortGo::g_hPlayingButton = NULL;

bool PortGo::g_bIsShaking = false;
//////////////////////////////////////////////////////////////////////////
void PortGo::FormatItemContact(CIUIString &strName, CIUIString &strHeadImage, CIUIString &strNumber, CContactDB *pContact)
{
	if (pContact==NULL)
	{
		return;
	}
	if (pContact->m_strHeadImage.empty() != true)
	{
		strHeadImage = PortUtility::Utf82Unicode_ND(pContact->m_strHeadImage).c_str();
	}
	if (pContact->m_strSIPNum.empty() != true)
	{
		strNumber = PortUtility::Utf82Unicode_ND(pContact->m_strSIPNum).c_str();
	}
	std::string strContactName;
	if (pContact->m_strFirstName.empty() || pContact->m_strLastName.empty())
	{
		strContactName = pContact->m_strSIPNum;
		strName = PortUtility::Utf82Unicode_ND(strContactName).c_str();
	}
	else
	{
		strContactName = pContact->m_strFirstName + "  " + pContact->m_strLastName;
		strName = PortUtility::Utf82Unicode_ND(strContactName).c_str();
	}

}
int PortGo::FindContact(LPCTSTR lpszFilter, __out std::vector<CContactDB *> *pvContacts)
{
	if (pvContacts == NULL)
	{
		return -1;
	}

	//TODO 

#if 0
	// TODO: ²âÊÔ´úÂë
	pvContacts->push_back(g_mapContacts.begin()->second);
#endif
	return (int)pvContacts->size();
}

HTREEITEM PortGo::FindContactFromTree(HWLWND hTree, HTREEITEM hParentItem, int nUID)
{
	TreeView *pTree = (TreeView *)CWLWnd::FromHandle(hTree);

	HTREEITEM hItemRet = NULL;
	if (pTree == NULL || nUID <= 0)
	{
		return hItemRet;
	}

	//
	//HTREEITEM hStartItem = hParentItem;
	//if (hStartItem == NULL)
	//{
	//	hStartItem = pTree->GetRootItem();
	//}

	//
	//CContactTreeItemData *pItemData = (CContactTreeItemData *)pTree->GetItemData(hStartItem);
	//if (pItemData != NULL)
	//{
	//	if (pItemData->m_eContactTreeItemType == CTIT_CONTACT
	//		|| pItemData->m_eContactTreeItemType == CTIT_RECENT_CONTACT)
	//	{
	//		const CContact *pContact = (CContact *)pItemData->m_lParam;

	//		if (pContact->GetUID() == nUID)
	//		{
	//			hItemRet = hStartItem;
	//		}
	//	}
	//}

	//if (hItemRet != NULL)
	//{
	//	return hItemRet;
	//}

	//
	//HTREEITEM hChildItem = pTree->GetChildItem(hStartItem);
	//while (hChildItem != NULL)
	//{
	//	hItemRet = FindContactFromTree(pTree, hChildItem, nUID);

	//	if (hItemRet != NULL)
	//	{
	//		break;
	//	}

	//	hChildItem = pTree->GetNextItem(hChildItem, TVGN_NEXT);
	//}

	//if (hItemRet != NULL)
	//{
	//	return hItemRet;
	//}

	//
	//if (hParentItem == NULL)
	//{
	//	do
	//	{
	//		hStartItem = pTree->GetNextItem(hStartItem, TVGN_NEXT);

	//		if (hStartItem != NULL)
	//		{
	//			hItemRet = FindContactFromTree(pTree, hStartItem, nUID);

	//			if (hItemRet != NULL)
	//			{
	//				break;
	//			}
	//		}
	//	}
	//	while (hStartItem != NULL);
	//}

	return hItemRet;
}

int PortGo::SwitchToContact(TAG_RECENT *pContactInfo, CHAT_TAB enumChatTab)
{
	::SendMessage(g_hMainFrame, WM_SWITCH_TO_CONTACT,(WPARAM)enumChatTab, (LPARAM)pContactInfo);
	return 0;
}

int PortGo::SwitchToContactNoBlock(TAG_RECENT *pContactInfo, CHAT_TAB enumChatTab)
{
	::PostMessage(g_hMainFrame, WM_SWITCH_TO_CONTACT, (WPARAM)enumChatTab, (LPARAM)pContactInfo);
	return 0;
}

int PortGo::InsertInputFileOle(HWLWND hReInput, LPCTSTR lpszFile, LPCTSTR lpszFilePath, int cp, HWLWND &hOleWnd,wstring wstrSender)
{
	if (NULL == hReInput)
	{
		return -1;
	}

	
	OleControl *pOleFile = new OleControl(2, hReInput);
	if (NULL == pOleFile)
	{
		return -2;
	}
	pOleFile->SetUserData(OLE_ALONE_MSG);
	HWLWND hBindOleFilePanel= CreateXMLWindowChild(new COleFileCanvasHandler, NULL, _T("IDW_OLE_FILE.xml"),
		hReInput, GetHostWnd(hReInput), 0);
	if (NULL == hBindOleFilePanel)
	{
		return -3;
	}

	hOleWnd = hBindOleFilePanel;
	SendMessage(hBindOleFilePanel, WLM_SETHIDEOLEBORDER, 0, TRUE);

	HWLWND hStaFileName = FindControl(hBindOleFilePanel, _T("IDC_STA_FILE_NAME"));
	Label *pStaFileName = (Label *)CWLWnd::FromHandle(hStaFileName);

	pStaFileName->SetWindowText(lpszFile);
	pOleFile->SetOleBindWindowless(hBindOleFilePanel);

	CIUIRect rcOle;
	GetWindowRectToParent(hBindOleFilePanel, rcOle);

	RichTextBox *pReInput = (RichTextBox *)CWLWnd::FromHandle(hReInput);
	pReInput->InsertOle(pOleFile, REO_CP_SELECTION, rcOle.Width(), rcOle.Height());

	return 0;
}

int PortGo::InsertInputImageOle(HWLWND hReInput, LPCTSTR lpszImage, int cp, HWLWND &hOleWnd)
{
#define MAX_IMAGE_SIZE		300

	if (NULL == hReInput || NULL == lpszImage)
	{
		return -1;
	}

	OleControl *pOleImage = new OleControl(3, hReInput);
	if (NULL == pOleImage)
	{
		return -2;
	}
	HWLWND hStaImage = IUI::CreateWindowEx(NULL, 0, IUI_LABEL, NULL, WS_VISIBLE,
			0, 0, 0, 0,
			GetHostWnd(hReInput), hReInput,
			_T("IDC_STA_IMAGE"), 0, NULL);

	if (NULL == hStaImage)
	{
		_ASSERT(FALSE);
		return -1;
	}
	hOleWnd = hStaImage;
	IUI::SetProp(hStaImage, GETPROP_OLETYPE, (HANDLE)OT_IMAGE);
	IUI::SetTag(hStaImage, lpszImage);
	CombineBkImage(hStaImage, FALSE);
	SetBkImage(hStaImage, FALSE, CONTROL_STATE_NORMAL, lpszImage);
	SetBkImageResizeMode(hStaImage, IRM_KEEPRATE);

	pOleImage->SetOleBindWindowless(hStaImage);
	HIUIIMAGE hImage = CreateIUIImage(GetProject(hReInput), lpszImage);

	int nMaxSideLength = max(hImage->GetWidth(), hImage->GetHeight());
	int nNewWidth = hImage->GetWidth();
	int nNewHeight = hImage->GetHeight();
	if (nMaxSideLength > MAX_IMAGE_SIZE)
	{
		nNewWidth = MulDiv(hImage->GetWidth(), MAX_IMAGE_SIZE, nMaxSideLength);
		nNewHeight = MulDiv(hImage->GetHeight(), MAX_IMAGE_SIZE, nMaxSideLength);
	}

	RichTextBox *pReInput = (RichTextBox *)CWLWnd::FromHandle(hReInput);
	pReInput->InsertOle(pOleImage, REO_CP_SELECTION, nNewWidth, nNewHeight);

	ReleaseIUIImage(hImage);

	return 0;
}

int PortGo::InsertInputVideoOle(HWLWND hReInput, LPCTSTR lpszVideo, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hOleControlPannel, wstring strContactName)
{
	if (NULL == hReInput)
	{
		return -1;
	}

	OleControl *pOleFile = new OleControl(4, hReInput);
	if (NULL == pOleFile)
	{
		return -2;
	}

	pOleFile->SetUserData(OLE_ALONE_MSG);
	COleVideoCanvasHandler *pOleVideoCanvasHandler = new COleVideoCanvasHandler;
	pOleVideoCanvasHandler->SetVideoParam(lpszFilePath, lpszTotalPlayTime,lpszVideo);
	HWLWND hBindOleVideoPanel = CreateXMLWindowChild(pOleVideoCanvasHandler, NULL, _T("IDW_OLE_VIDEO.xml"),
		hReInput, GetHostWnd(hReInput), 0);
	if (NULL == hBindOleVideoPanel)
	{
		return -3;
	}
	hOleControlPannel = hBindOleVideoPanel;
	SendMessage(hBindOleVideoPanel, WLM_SETHIDEOLEBORDER, 0, TRUE);

	pOleFile->SetOleBindWindowless(hBindOleVideoPanel);

	CIUIRect rcOle;
	GetWindowRectToParent(hBindOleVideoPanel, rcOle);
	IUI::SetUserData(hBindOleVideoPanel, MAKELPARAM(rcOle.Width(), rcOle.Height()));

	RichTextBox *pReInput = (RichTextBox *)CWLWnd::FromHandle(hReInput);
	pReInput->InsertOle(pOleFile, REO_CP_SELECTION, 200, 160);
	return 0;
}
int PortGo::InsertHistoryVideoOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszVideo,LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hWndOle,void **pVideoCanvasHandler,  wstring strContactName, int nMsgID, INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace,LPCTSTR lpszSenderID)
{
	HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, lpszFace);

	IMRichTextBox *pIMHistory = (IMRichTextBox *)CWLWnd::FromHandle(hIMHistory);
	_ASSERT(NULL != pIMHistory);

	CIMMsg msg;
	msg.SetMsgSenderTitle(0);
	msg.SetMsgSender(strContactName.c_str());
	msg.SetFace(hFace);
	msg.SetSelf(bSelf);
	msg.SetMsgID(nMsgID);
	msg.SetUserData(false);
	msg.SetMsgSenderID(lpszSenderID);
	if (bSelf)
	{
		msg.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
	}
	{
		OleControl *pOleFile = new OleControl(4, hIMHistory);
		if (NULL == pOleFile)
		{
			return -2;
		}

		pOleFile->SetUserData(OLE_ALONE_MSG);

		COleVideoCanvasHandler *pOleVideoCanvasHandler = new COleVideoCanvasHandler;
		pOleVideoCanvasHandler->SetVideoParam(lpszFilePath, lpszTotalPlayTime, lpszVideo);

		HWLWND hBindOleVideoPanel = CreateXMLWindowChild(pOleVideoCanvasHandler,
			NULL, _T("IDW_OLE_VIDEO.xml"), hIMHistory, GetHostWnd(hIMHistory), 0);
		hWndOle= hBindOleVideoPanel;
		//*pVideoCanvasHandler =(void*)pOleVideoCanvasHandler;
		if (NULL == hBindOleVideoPanel)
		{
			return -3;
		}

		SendMessage(hBindOleVideoPanel, WLM_SETHIDEOLEBORDER, 0, TRUE);
		pOleFile->SetOleBindWindowless(hBindOleVideoPanel);

		CIUIRect rcOle;
		GetWindowRectToParent(hBindOleVideoPanel, rcOle);
		IUI::SetUserData(hBindOleVideoPanel, MAKELPARAM(rcOle.Width(), rcOle.Height()));

		CRichMsgItem item;
		item.m_bOleControl = TRUE;
		item.m_hOleBindWindowless = hBindOleVideoPanel;
		item.m_size = rcOle.Size();

		msg.AddMsgItem(&item);
	}

	pIMHistory->InsertMsg(enumHistoryMode,&msg);
	ImageManager::ReleaseIUIImage(hFace);

	return 0;
}

int PortGo::InsertHistoryMessage(HWLWND hIMHistory, int cp,LPCTSTR lpszFace)
{
	HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, lpszFace);

	IMRichTextBox *pIMHistory = (IMRichTextBox *)CWLWnd::FromHandle(hIMHistory);
	_ASSERT(NULL != pIMHistory);

	CIMMsg msg;
	msg.SetMsgSenderTitle(0);
	msg.SetMsgSender(_T("test name"));
	msg.SetFace(hFace);
	msg.SetUserData(false);
	
	{
		CRichMsgItem item;
		item.m_strMsgContent = _T("test msg");
		msg.AddMsgItem(&item);
	}
	{
		HWLWND hStaImage = IUI::CreateWindowEx(NULL, 0, IUI_LABEL, NULL, WS_VISIBLE,
			0, 0, 0, 0,
			GetHostWnd(hIMHistory), hIMHistory,
			_T("IDC_STA_IMAGE"), 0, NULL);

		if (NULL == hStaImage)
		{
			_ASSERT(FALSE);
			return -1;
		}

		CIUIString strImage = GetRunFolder() + _T("skins\\PortGo\\Title_Center.png");

		IUI::SetProp(hStaImage, GETPROP_OLETYPE, (HANDLE)OT_IMAGE);
		IUI::SetTag(hStaImage, strImage);
		CombineBkImage(hStaImage, FALSE);
		SetBkImage(hStaImage, FALSE, CONTROL_STATE_NORMAL, strImage);
		SetBkImageResizeMode(hStaImage, IRM_KEEPRATE);

		CRichMsgItem item;
		item.m_bOleControl = TRUE;
		item.m_hOleBindWindowless = hStaImage;
		item.m_size = CIUISize(50, 50);
		msg.AddMsgItem(&item);
	}

	{
		CRichMsgItem item;
		item.m_strMsgContent = _T("test add msg");
		msg.AddMsgItem(&item);
	}

	pIMHistory->InsertMsg(INSERT_FORWARD,&msg);
	ImageManager::ReleaseIUIImage(hFace);

	return 0;
}
int PortGo::InsertHistoryImageOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszImagePath, LPCTSTR lpszImageName, HWLWND &hOleHwnd, wstring strContactName, int nMsgID, INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace,LPCTSTR lpszSenderID)
{
	if (NULL == hIMHistory)
	{
		return -1;
	}

	CIUIString strImagePath = lpszImagePath;
	CIUIString strImageName = lpszImageName;
	CIUIString strTemplatePath = strImagePath + strImageName;

	HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, lpszFace);

	CIMMsg msg;
	msg.SetMsgSenderTitle(1);
	msg.SetSelf(bSelf);
	msg.SetMsgSender(strContactName.c_str());
	msg.SetFace(hFace);
	msg.SetMsgID(nMsgID);
	msg.SetUserData(false);
	msg.SetMsgSenderID(lpszSenderID);
	if (bSelf)
	{
		msg.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
	}
	OleControl *pOleImage = new OleControl(3, hIMHistory);
	if (NULL == pOleImage)
	{
		return -2;
	}
	pOleImage->SetUserData(OLE_ALONE_MSG);

	HWLWND hStaImage = IUI::CreateWindowEx(new COleImageCanvasHandler, 0, IUI_LABEL, NULL, WS_VISIBLE,
		0, 0, 0, 0,
		GetHostWnd(hIMHistory), hIMHistory,
		_T("IDC_STA_IMAGE"), 0, NULL);
	if (NULL == hStaImage)
	{
		_ASSERT(FALSE);
		return -1;
	}
	hOleHwnd = hStaImage;
	IUI::SetProp(hStaImage, GETPROP_OLETYPE, (HANDLE)OT_IMAGE);
	IUI::SetTag(hStaImage, strTemplatePath);
	CombineBkImage(hStaImage, FALSE);
	SetBkImage(hStaImage, FALSE, CONTROL_STATE_NORMAL, strTemplatePath);
	SetBkImageResizeMode(hStaImage, IRM_KEEPRATE);
	HWLWND hPrgDownload = IUI::CreateWindowEx(NULL, 0, IUI_PROGRESSBAR, NULL, WS_VISIBLE,
		15, 146, 170, 4,
		GetHostWnd(hIMHistory), hStaImage, _T("IDC_PRG_IMAGE_OLE"), 0, NULL);
	if (NULL == hPrgDownload)
	{
		_ASSERT(FALSE);
		return -4;
	}
	CControlProp *pPrgStyle = g_pProject->GetStyleItem(STYLET_PROGRESS, _T("IDC_PRG_IMAGE_OLE"));
	_ASSERT(NULL != pPrgStyle);
	ProgressBar_BindStyle(hPrgDownload, (CProgressBarProp *)pPrgStyle);
	pOleImage->SetOleBindWindowless(hStaImage);

	SendMessage(hStaImage, WLM_SETHIDEOLEBORDER, 0, TRUE);

	CRichMsgItem item;
	item.m_bOleControl = TRUE;
	item.m_hOleBindWindowless = hStaImage;
	item.m_size = CIUISize(200, 150);
	msg.AddMsgItem(&item);
	//
	//
	IMRichTextBox *pIMHistory = (IMRichTextBox *)CWLWnd::FromHandle(hIMHistory);
	_ASSERT(NULL != pIMHistory);
	if (!msg.GetMsgItems()->empty())
	{
		pIMHistory->InsertMsg(enumHistoryMode,&msg);
	}
	ImageManager::ReleaseIUIImage(hFace);

	return 0;
}
int PortGo::PopupVideoWnd(HBITMAP pBitmap,HWND hParent, CIUIString strFilePath, CIUIString strTotalPlayTime, \
	CIUIString strFileName)
{
	static CPopupVideoWnd* pWmdVideo = new CPopupVideoWnd;
	pWmdVideo->SetPlayParam(pBitmap,strFilePath, strTotalPlayTime, strFileName);
	if (NULL == pWmdVideo->GetSafeHwnd())
	{
		pWmdVideo->SetProject(g_pProject);
		pWmdVideo->CreatePopupWindow(_T("IDW_POPUP_VIDEO.xml"), hParent);
	}
	
	pWmdVideo->CenterWindow();
	ShowWindow(pWmdVideo->GetSafeHwnd(), SW_SHOW);

	return 0;
}
void PortGo::PopupLoginLoadWnd(HWND hParent)
{
	g_pWmdLoginLoading = new CLoginLoadingDlg;
	if (NULL == g_pWmdLoginLoading->GetSafeHwnd())
	{
		g_pWmdLoginLoading->SetProject(g_pProject);
		g_pWmdLoginLoading->SetXml(_T("IDW_LOGIN.xml"));;
	}

	g_pWmdLoginLoading->DoModal();
}
ITEM_POS     PortGo::CompareItemPosByFirstItem(TreeView *pTreeView, HTREEITEM itemX, HTREEITEM itemY)
{
	if (pTreeView==NULL||pTreeView->GetSafeHwnd()==NULL)
	{
		LOG4_INFO("TREE PT ERROR");
		return ITEM_UNKNOW;
	}
	if (itemY == itemX)
	{
		return ITEM_SAME;
	}
	HTREEITEM hPreItem = itemX;
	do
	{
		hPreItem = pTreeView->GetNextItem(hPreItem, TVGN_PREVIOUS);
		if (itemY == hPreItem)
		{
			return ITEM_FRONT;
		}
	} while (hPreItem != NULL);

	hPreItem = itemX;
	do
	{
		hPreItem = pTreeView->GetNextItem(hPreItem, TVGN_NEXT);
		if (itemY == hPreItem)
		{
			return ITEM_BEHIND;
		}
	} while (hPreItem != NULL);

	return ITEM_UNKNOW;
}
int  PortGo::ClosePopupLoginLoadWnd()
{
	if (g_pWmdLoginLoading!=NULL)
	{
		if (g_pWmdLoginLoading->GetSafeHwnd()!=NULL)
		{
			if ((g_pWmdLoginLoading->GetSafeHwnd()))
			{
				g_pWmdLoginLoading->EndDialog(IDOK);
			}
		}
	}

	return 0;
}

HWND PortGo::PopupNoPreviewVideoWnd(HWND hParent)
{
	static CPopupVideoWnd* pWmdVideo = new CPopupVideoWnd;
	if (NULL == pWmdVideo->GetSafeHwnd())
	{
		pWmdVideo->SetProject(g_pProject);
		pWmdVideo->CreatePopupWindow(_T("IDW_POPUP_VIDEO.xml"), hParent);
	}
	pWmdVideo->CenterWindow();
	ShowWindow(pWmdVideo->GetSafeHwnd(), SW_HIDE);
	
	HWND hWnd = pWmdVideo->GetPlayHwnd();
	return hWnd;
}

int PortGo::PopupInfoWnd(LPCTSTR lpszTitle, LPCTSTR lpszInfo,
	HWND hParent, WND_INFO_LEVEL enumWndInfoLevel,int nAutoCloseTime/* = 10000*/)
{
	int nRet = -1;

	CInfoWnd *pInfoWnd = new CInfoWnd(enumWndInfoLevel,false);
	if (NULL != pInfoWnd)
	{
		pInfoWnd->SetAutoCloseTime(nAutoCloseTime);
		pInfoWnd->SetProject(g_pProject);
		pInfoWnd->CreatePopupWindow(_T("IDW_INFO.xml"), hParent);

		pInfoWnd->SetTitle(lpszTitle);
		pInfoWnd->SetInfo(lpszInfo);

		RECT rcClient;
		pInfoWnd->GetWindowRect(&rcClient);

		int nWidth = rcClient.right - rcClient.left;
		int nHeight = rcClient.bottom - rcClient.top;
		int nStartX = CSystemEx::GetInstance()->GetMetricsX() - nWidth;
		int nStartY = CSystemEx::GetInstance()->GetMetricsY() - (nHeight*( 1));
		
		MoveWindow(pInfoWnd->GetSafeHwnd(), nStartX, nStartY, nWidth, nHeight, true);
		ShowWindow(pInfoWnd->GetSafeHwnd(), SW_SHOW);
		pInfoWnd->Invalidate();

		nRet = 0;
	}

	return nRet;
}
void PortGo::ProcessShake()
{
	CBaseThread		ShakeThread;
	ShakeThread.BeginThread(_ShakeThread, NULL);
}
unsigned int PortGo::_ShakeThread(void* pParam)
{
	ShakeWnd(3,CSystemEx::GetInstance()->GetMainDlg());
	return 1;
}
void PortGo::ShakeWnd(int nShakeWndNum, CMainWnd* pShakeWnd)
{
	RECT rcClient;
	int x, y, nWidth, nHeight;
	pShakeWnd->GetWindowRect(&rcClient);
	x = rcClient.left;
	y = rcClient.top;
    nWidth = rcClient.right - x;
	nHeight = rcClient.bottom - y;
	if(g_bIsShaking)
		return;
	g_bIsShaking = true;
	HWND hShakeMainWnd = pShakeWnd->GetSafeHwnd();
	for (int i = 0; i < nShakeWndNum; i++)
	{
		::MoveWindow(hShakeMainWnd,x - 10, y, nWidth, nHeight, TRUE);
		Sleep(50);
		::MoveWindow(hShakeMainWnd,x - 10, y - 10, nWidth, nHeight, TRUE);
		Sleep(50);
		::MoveWindow(hShakeMainWnd,x, y - 10, nWidth, nHeight, TRUE);
		Sleep(50);
		::MoveWindow(hShakeMainWnd,x, y, nWidth, nHeight, TRUE);
		Sleep(50);
	}
	g_bIsShaking = false;
}

int PortGo::PopupTrayMsgWnd(HWND hParent)
{
	static CTrayMsgWnd wndTrayMsg;
	if (NULL == wndTrayMsg.GetSafeHwnd())
	{
		wndTrayMsg.SetProject(g_pProject);
		wndTrayMsg.CreatePopupWindow(_T("IDW_TRAY_MSG.xml"), hParent);
	}

	ShowWindow(wndTrayMsg.GetSafeHwnd(), SW_SHOW);
	wndTrayMsg.Invalidate();

	return 0;
}
int PortGo::PopupMsgHistoryWnd(CContactDB		 *pContact, HWND hParent)
{
	int nRet = -1;

	CMsgHistoryWnd *pMsgHistoryWnd = new CMsgHistoryWnd;
	if (NULL != pMsgHistoryWnd)
	{
		pMsgHistoryWnd->SetProject(g_pProject);
		pMsgHistoryWnd->CreatePopupWindow(_T("IDW_MSG_HISTORY.xml"), hParent);

		g_pMsgHistoryWnd = pMsgHistoryWnd;
		RECT rcMainDlg;
		CSystemEx::GetInstance()->GetMainDlg()->GetClientRect(&rcMainDlg);
		::MoveWindow(pMsgHistoryWnd->GetSafeHwnd(), 0, 0, 360, rcMainDlg.bottom-rcMainDlg.top, FALSE);
		LayoutMsgHistoryWnd();
		ShowWindow(pMsgHistoryWnd->GetSafeHwnd(), SW_SHOW);
		pMsgHistoryWnd->Invalidate();
		nRet = 0;
	}

	return nRet;
}

int PortGo::LayoutMsgHistoryWnd()
{
	if (NULL != g_pMainFrame
		&& NULL != g_pMsgHistoryWnd
		&& NULL != g_pMsgHistoryWnd->GetSafeHwnd()
		&& IsWindowVisible(g_pMsgHistoryWnd->GetSafeHwnd()))
	{
		CIUIRect rcMain;
		::GetWindowRect(g_pMainFrame->GetSafeHwnd(), rcMain);

		CIUIRect rcHistoryWnd;
		::GetWindowRect(g_pMsgHistoryWnd->GetSafeHwnd(), rcHistoryWnd);

		rcHistoryWnd.OffsetRect(rcMain.Width() - (rcHistoryWnd.left - rcMain.left),
			rcMain.top - rcHistoryWnd.top);

		::MoveWindow(g_pMsgHistoryWnd->GetSafeHwnd(),
			rcHistoryWnd.left, rcHistoryWnd.top, rcHistoryWnd.Width(), rcHistoryWnd.Height(), TRUE);
	}

	return 0;
}

VOID CALLBACK FlickerTrayIconTimer(HWND hWnd, UINT, UINT_PTR, DWORD)
{
	static BOOL bShow = TRUE;

	NOTIFYICONDATA tnd;
	tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
	tnd.hWnd = g_pMainFrame->GetSafeHwnd();
	CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
#ifdef BZXX
	tnd.uID = IDI_BZXX_PG;
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif

#ifdef KCXX
	tnd.uID = IDI_KCXX_PG;
	StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
#endif
	
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYNOTIFY;


	if (bShow)
	{
		bShow = FALSE;

		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_HIDE_TRAY_ICON));
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
	else
	{
		bShow = TRUE;

#ifdef BZXX
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
#endif

#ifdef KCXX
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
#endif

		
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
	}
}

int PortGo::FlickerTrayIcon(HICON hIcon,bool bEnable)
{

	::KillTimer(g_pMainFrame->GetSafeHwnd(), IDT_TRAY_FLICKER);

	if (bEnable==false)
	{
		
		NOTIFYICONDATA tnd;
		tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
		tnd.hWnd = g_pMainFrame->GetSafeHwnd();

		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYNOTIFY;
		CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
#ifdef BZXX
		tnd.uID = IDI_BZXX_PG;
		StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
#endif

#ifdef KCXX
		tnd.uID = IDI_KCXX_PG;
		StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
#endif
		
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
		return 0;
	}
	if (NULL != hIcon)
	{
		::SetTimer(g_pMainFrame->GetSafeHwnd(), IDT_TRAY_FLICKER, 500, FlickerTrayIconTimer);
	}
	else
	{
		NOTIFYICONDATA tnd;
		tnd.cbSize = NOTIFYICONDATA_V1_SIZE;
		tnd.hWnd = g_pMainFrame->GetSafeHwnd();
	
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYNOTIFY;	
		CIUIString strAppName = CSystemEx::GetInstance()->GetDynamicTextAppName();
#ifdef BZXX
		tnd.uID = IDI_BZXX_PG;
		StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_BZXX_PG));
#endif

#ifdef KCXX
		tnd.uID = IDI_KCXX_PG;
		StringCchCopy(tnd.szTip, _countof(tnd.szTip), strAppName);
		tnd.hIcon = LoadIcon(IUIGetInstanceHandle(), MAKEINTRESOURCE(IDI_KCXX_PG));
#endif
		Shell_NotifyIcon(NIM_MODIFY, &tnd);
		::SetTimer(g_pMainFrame->GetSafeHwnd(), IDT_TRAY_FLICKER, 500, FlickerTrayIconTimer);
	}

	return 0;
}

int PortGo::InsertHistoryFileOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszFile, LPCTSTR lpszFilePath, HWLWND &hOleWnd, wstring strContactName, int nMsgID,INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace,LPCTSTR lpszSenderID)
{
	if (NULL == hIMHistory)
	{
		return -1;
	}

	HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, lpszFace);

	CIMMsg msg;
	msg.SetMsgSenderTitle(1);
	msg.SetSelf(bSelf);
	msg.SetMsgSender(strContactName.c_str());
	msg.SetFace(hFace);
	msg.SetMsgID(nMsgID);
	msg.SetUserData(false);
	msg.SetMsgSenderID(lpszSenderID);
	if (bSelf)
	{
		msg.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
	}
	COleFileCanvasHandler*pFileCanvasHandler = new COleFileCanvasHandler();
	pFileCanvasHandler->SetFilePath(lpszFilePath);
	HWLWND hBindOleFilePanel = CreateXMLWindowChild(pFileCanvasHandler, NULL, _T("IDW_OLE_FILE.xml"),
		hIMHistory, GetHostWnd(hIMHistory), 0);
	if (NULL == hBindOleFilePanel)
	{
		return -3;
	}
	hOleWnd = hBindOleFilePanel;
	IUI::SetProp(hBindOleFilePanel, GETPROP_UNIQUE, _T("abcdefg"));

	SendMessage(hBindOleFilePanel, WLM_SETHIDEOLEBORDER, 0, TRUE);

	HWLWND hStaFileName = FindControl(hBindOleFilePanel, _T("IDC_STA_FILE_NAME"));
	Label *pStaFileName = (Label *)CWLWnd::FromHandle(hStaFileName);

	pStaFileName->SetWindowText(lpszFile);

	CIUIRect rcOle;
	GetWindowRectToParent(hBindOleFilePanel, rcOle);

	CRichMsgItem item;
	item.m_bOleControl = TRUE;
	item.m_hOleBindWindowless = hBindOleFilePanel;
	item.m_size = rcOle.Size();
	msg.AddMsgItem(&item);

	//
	//
	IMRichTextBox *pIMHistory = (IMRichTextBox *)CWLWnd::FromHandle(hIMHistory);
	_ASSERT(NULL != pIMHistory);
	if (!msg.GetMsgItems()->empty())
	{
		pIMHistory->InsertMsg(enumHistoryMode,&msg);
	}
	ImageManager::ReleaseIUIImage(hFace);

	return 0;
}

int PortGo::InsertInputAudioOle(HWLWND hReInput, LPCTSTR lpszAudio, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hOleControlPannel, wstring strContactName)
{
	if (NULL == hReInput)
	{
		return -1;
	}

	OleControl *pOleFile = new OleControl(5, hReInput);
	if (NULL == pOleFile)
	{
		return -2;
	}

	pOleFile->SetUserData(OLE_ALONE_MSG);

	COleAudioCanvasHandler *pAudioCanvasHandler = new COleAudioCanvasHandler;
	pAudioCanvasHandler->SetAudioParam(lpszFilePath, lpszTotalPlayTime, lpszAudio);
	HWLWND hBindOleAudioPanel = CreateXMLWindowChild(pAudioCanvasHandler, NULL, _T("IDW_OLE_AUDIO.xml"),
		hReInput, GetHostWnd(hReInput), 0);
	if (NULL == hBindOleAudioPanel)
	{
		return -3;
	}
	hOleControlPannel = hBindOleAudioPanel;
	SendMessage(hBindOleAudioPanel, WLM_SETHIDEOLEBORDER, 0, TRUE);

	pOleFile->SetOleBindWindowless(hBindOleAudioPanel);
	CIUIRect rcOle;
	GetWindowRectToParent(hBindOleAudioPanel, rcOle);
	IUI::SetUserData(hBindOleAudioPanel, MAKELPARAM(rcOle.Width(), rcOle.Height()));

	RichTextBox *pReInput = (RichTextBox *)CWLWnd::FromHandle(hReInput);
	pReInput->InsertOle(pOleFile, REO_CP_SELECTION, rcOle.Width(), rcOle.Height());

	return 0;
}


int PortGo::InsertHistoryAudioOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszAudio, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hOleControlPannel, wstring strContactName, int nMsgID, INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace, LPCTSTR lpszSenderID)
{
	if (NULL == hIMHistory)
	{
		return -1;
	}

	HIUIIMAGE hFace = ImageManager::CreateIUIImage(g_pProject, lpszFace);

	CIMMsg msg;
	msg.SetMsgSenderTitle(0);
	msg.SetMsgSender(strContactName.c_str());
	msg.SetFace(hFace);
	msg.SetSelf(bSelf);
	msg.SetMsgID(nMsgID);
	msg.SetUserData(false);
	msg.SetMsgSenderID(lpszSenderID);
	if (bSelf)
	{
		msg.SetUserData(CSystemEx::GetInstance()->GetGlobalUser()->GetUserFaceImageSet());
	}
	{
		COleAudioCanvasHandler *pAudioCanvasHandler = new COleAudioCanvasHandler;
		pAudioCanvasHandler->SetAudioParam(lpszFilePath, lpszTotalPlayTime,lpszAudio);

		HWLWND hBindOleAudioPanel = CreateXMLWindowChild(pAudioCanvasHandler, NULL, _T("IDW_OLE_AUDIO.xml"),
			hIMHistory, GetHostWnd(hIMHistory), 0);
		hOleControlPannel = hBindOleAudioPanel;
		if (NULL == hBindOleAudioPanel)
		{
			return -3;
		}
		hOleControlPannel = hBindOleAudioPanel;
		SendMessage(hBindOleAudioPanel, WLM_SETHIDEOLEBORDER, 0, TRUE);

		CIUIRect rcOle;
		GetWindowRectToParent(hBindOleAudioPanel, rcOle);
		IUI::SetUserData(hBindOleAudioPanel, MAKELPARAM(rcOle.Width(), rcOle.Height()));

		CRichMsgItem item;
		item.m_bOleControl = TRUE;
		item.m_hOleBindWindowless = hBindOleAudioPanel;
		item.m_size = rcOle.Size();

		msg.AddMsgItem(&item);
	}

	IMRichTextBox *pIMHistory = (IMRichTextBox *)CWLWnd::FromHandle(hIMHistory);
	_ASSERT(NULL != pIMHistory);
	if (!msg.GetMsgItems()->empty())
	{
		pIMHistory->InsertMsg(enumHistoryMode,&msg);
	}
	ImageManager::ReleaseIUIImage(hFace);

	return 0;
}


int _DrawFaceToPart(HDC hDC, LPCRECT lprcTarget, LPCTSTR lpszFace, GROUP_FACE_PART ePart,LPCTSTR lpszName = NULL)
{
	CIUIRect rc = lprcTarget;

	HIUIIMAGE himgFace = CreateIUIImage(g_pProject, lpszFace);

	CIUIRect rcPart = rc;
	rcPart.OffsetRect(-rcPart.TopLeft());
	CIUIRect rcSrc;
	rcSrc.right = himgFace->GetWidth();
	rcSrc.bottom = himgFace->GetHeight();
	CIUIRect rcName;
	switch (ePart)
	{
	case GFP_FULL:
		break;

	case GFP_LEFT:
		rcPart.right = rcPart.left + rc.Width() / 2;

		rcSrc.left = himgFace->GetWidth() / 4;
		rcSrc.right = rcSrc.left + himgFace->GetWidth() / 2;


		rcName.right = rcPart.right;
		
		break;
	case GFP_TOP_LEFT:
		rcPart.right = rcPart.left + rc.Width() / 2;
		rcPart.bottom = rcPart.top + rc.Height() / 2;
		break;
	case GFP_TOP_RIGHT:
		rcPart.left = rcPart.right - rc.Width() / 2 + 1;
		rcPart.bottom = rcPart.top + rc.Height() / 2;
		break;
	case GFP_RIGHT:
		rcPart.left = rcPart.right - rc.Width() / 2 + 1;

		rcSrc.left = himgFace->GetWidth() / 4;
		rcSrc.right = rcSrc.left + himgFace->GetWidth() / 2;
		break;
	case GFP_LEFT_BOTTOM:
		rcPart.top = rcPart.bottom - rc.Height() / 2 + 1;
		rcPart.right = rcPart.left + rc.Width() / 2;
		break;
	case GFP_RIGHT_BOTTOM:
		rcPart.left = rcPart.right - rc.Width() / 2 + 1;
		rcPart.top = rcPart.bottom - rc.Height() / 2 + 1;
		break;
	case GFP_BOTTOM:
		rcPart.top = rcPart.bottom - rc.Height() / 2 + 1;

		rcSrc.top = himgFace->GetHeight() / 4;
		rcSrc.bottom = rcSrc.top + himgFace->GetHeight() / 2;
		break;
	default:
		_ASSERT(FALSE);
		break;
	}

	IUIStretchBlt(hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(),
		himgFace, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height());


	
	if (lpszName!=NULL&& GFP_FULL!= ePart)
	{
		HFONT hDefFont = FontManager::CreateIUIFont(g_pProject, _T("IDF_TAHOMA_9"));
		_ASSERT(hDefFont != NULL);
		CIUIString strLeft = lpszName;
		DrawFormatText(hDC, strLeft, 1, rcPart,
			CIUIRect(), TAH_CENTER, TAV_CENTER, DT_SINGLELINE, hDefFont, RGB(30, 30, 30), FALSE);
	}
	

	ReleaseIUIImage(himgFace);
	himgFace = NULL;

	return 0;
}

int PortGo::DrawFace(
	HDC hDC,
	LPCRECT lprcTarget,
	Gdiplus::TextureBrush *ptBrush)
{
	Gdiplus::Graphics g(hDC);
	g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	CIUIRect rc = lprcTarget;

	Gdiplus::GraphicsPath gp;
	gp.AddEllipse(0, 0, rc.Width() - 1, rc.Height() - 1);

	if (NULL != ptBrush)
	{
		ptBrush->TranslateTransform(0, 0);
		g.FillPath(ptBrush, &gp);
	}

	return 0;
}

int PortGo::DrawFace(
	HDC hDC,
	LPCRECT lprcTarget,
	LPCTSTR lpszFace)
{
	CIUIRect rc = lprcTarget;
	HDC hdcMem = CreateCompatibleDC(hDC);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

	BitBlt(hdcMem, 0, 0, rc.Width(), rc.Height(), hDC, rc.left, rc.top, SRCCOPY);

	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace, GFP_FULL);
	SelectObject(hdcMem, hbmpOld);

	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath gp;
		gp.AddEllipse(rc.left, rc.top, rc.Width() - 1, rc.Height() - 1);
		Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromHBITMAP(hbmpMem, NULL);
		Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
		if (NULL != ptBrush)
		{
			ptBrush->TranslateTransform(Gdiplus::REAL(rc.left), Gdiplus::REAL(rc.top));
			g.FillPath(ptBrush, &gp);
		}
		if (NULL != ptBrush)
		{
			delete ptBrush;
			ptBrush = NULL;
		}
		if (NULL != pImage)
		{
			delete pImage;
			pImage = NULL;
		}
	}

	DeleteObject(hbmpMem);
	hbmpMem = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	return 0;
}

int PortGo::DrawGroupFace2(
	HDC hDC,
	LPCRECT lprcTarget,
	LPCTSTR lpszFace1,
	LPCTSTR lpszFace2,
	LPCTSTR lpszName1,
	LPCTSTR lpszName2)
{
	CIUIRect rc = lprcTarget;

	HDC hdcMem = CreateCompatibleDC(hDC);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

	BitBlt(hdcMem, 0, 0, rc.Width(), rc.Height(), hDC, rc.left, rc.top, SRCCOPY);

	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace1, GFP_LEFT, lpszName1);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace2, GFP_RIGHT, lpszName2);
	SelectObject(hdcMem, hbmpOld);
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath gp;
		gp.AddEllipse(rc.left, rc.top, rc.Width() - 1, rc.Height() - 1);
		Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromHBITMAP(hbmpMem, NULL);
		Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
		if (NULL != ptBrush)
		{
			ptBrush->TranslateTransform(Gdiplus::REAL(rc.left), Gdiplus::REAL(rc.top));
			g.FillPath(ptBrush, &gp);
		}
	}

	SelectObject(hdcMem, hbmpOld);
	DeleteObject(hbmpMem);
	hbmpMem = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	return 0;
}

int PortGo::DrawGroupFace3(
	HDC hDC,
	LPCRECT lprcTarget,
	LPCTSTR lpszFace1,
	LPCTSTR lpszFace2,
	LPCTSTR lpszFace3,
	LPCTSTR lpszName1,
	LPCTSTR lpszName2,
	LPCTSTR lpszName3)
{
	CIUIRect rc = lprcTarget;

	HDC hdcMem = CreateCompatibleDC(hDC);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

	BitBlt(hdcMem, 0, 0, rc.Width(), rc.Height(), hDC, rc.left, rc.top,SRCCOPY);

	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace1, GFP_TOP_LEFT, lpszName1);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace2, GFP_TOP_RIGHT, lpszName2);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace3, GFP_BOTTOM, lpszName3);
	SelectObject(hdcMem, hbmpOld);
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath gp;
		gp.AddEllipse(rc.left, rc.top,rc.Width() - 1, rc.Height() - 1);
		Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromHBITMAP(hbmpMem, NULL);
		Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
		if (NULL != ptBrush)
		{
			ptBrush->TranslateTransform(Gdiplus::REAL(rc.left), Gdiplus::REAL(rc.top));
			g.FillPath(ptBrush, &gp);
		}
	}

	SelectObject(hdcMem, hbmpOld);
	DeleteObject(hbmpMem);
	hbmpMem = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	return 0;
}

int PortGo::DrawGroupFace4(
	HDC hDC,
	LPCRECT lprcTarget,
	LPCTSTR lpszFace1,
	LPCTSTR lpszFace2,
	LPCTSTR lpszFace3,
	LPCTSTR lpszFace4,
	LPCTSTR lpszName1,
	LPCTSTR lpszName2,
	LPCTSTR lpszName3,
	LPCTSTR lpszName4)
{
	CIUIRect rc = lprcTarget;

	HDC hdcMem = CreateCompatibleDC(hDC);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

	BitBlt(hdcMem, 0, 0, rc.Width(), rc.Height(), hDC, rc.left, rc.top, SRCCOPY);

	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace1, GFP_TOP_LEFT, lpszName1);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace2, GFP_TOP_RIGHT, lpszName2);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace3, GFP_LEFT_BOTTOM, lpszName3);
	_DrawFaceToPart(hdcMem, lprcTarget, lpszFace4, GFP_RIGHT_BOTTOM, lpszName4);
	SelectObject(hdcMem, hbmpOld);
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath gp;
		gp.AddEllipse(rc.left, rc.top, rc.Width() - 1, rc.Height() - 1);
		Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromHBITMAP(hbmpMem, NULL);
		Gdiplus::TextureBrush *ptBrush = new Gdiplus::TextureBrush(pImage);
		if (NULL != ptBrush)
		{
			ptBrush->TranslateTransform(Gdiplus::REAL(rc.left), Gdiplus::REAL(rc.top));
			g.FillPath(ptBrush, &gp);
		}
		delete ptBrush;
		ptBrush = NULL;
	}

	SelectObject(hdcMem, hbmpOld);
	DeleteObject(hbmpMem);
	hbmpMem = NULL;
	DeleteDC(hdcMem);
	hdcMem = NULL;

	return 0;
}
int PortGo::DrawGroupFace(HDC hDC, LPCRECT lprcTarget, TAG_RECENT *pContactInfo)
{
	if (pContactInfo->m_enumRecentContact != ENUM_MULTI_CONTACT)
	{
		return -1;
	}
	CTalkGroup *pGroup = pContactInfo->m_unionContactInfo.m_pChartGroup;
	if (NULL != hDC && NULL != lprcTarget && NULL != pGroup)
	{
		if (pGroup->GetMemberCount()<=1)
		{
			_ASSERT(FALSE);
		}
		CContactDB *pMember = pGroup->GetMember(0)->GetContactDB();
		CIUIString strCreatorFace = PortUtility::Utf82Unicode_ND(pMember->m_strHeadImage).c_str();
		
		if (pGroup->GetMemberCount() == 2)
		{
			CContactDB *pMember1 = pGroup->GetMember(1)->GetContactDB();
			_ASSERT(NULL != pMember1);
			CIUIString strFace1 =PortUtility::Utf82Unicode_ND(pMember1->m_strHeadImage).c_str() ;

			CIUIString strSrcHeadImage1, strSrcUserName1, strSrcSipNumber1;
			FormatItemContact(strSrcUserName1, strSrcHeadImage1, strSrcSipNumber1, pMember);

			CIUIString strSrcHeadImage2, strSrcUserName2, strSrcSipNumber2;
			FormatItemContact(strSrcUserName2, strSrcHeadImage2, strSrcSipNumber2, pMember1);

			DrawGroupFace2(hDC, lprcTarget, strCreatorFace, strFace1, strSrcUserName1, strSrcUserName2);
		}
		else if (pGroup->GetMemberCount() == 3)
		{
			CContactDB *pMember1 = pGroup->GetMember(1)->GetContactDB();
			_ASSERT(NULL != pMember1);
			CIUIString strFace1 = PortUtility::Utf82Unicode_ND(pMember1->m_strHeadImage).c_str();

			CContactDB *pMember2 = pGroup->GetMember(2)->GetContactDB();
			_ASSERT(NULL != pMember2);
			CIUIString strFace2 = PortUtility::Utf82Unicode_ND(pMember2->m_strHeadImage).c_str();

			CIUIString strSrcHeadImage1, strSrcUserName1, strSrcSipNumber1;
			FormatItemContact(strSrcUserName1, strSrcHeadImage1, strSrcSipNumber1, pMember);

			CIUIString strSrcHeadImage2, strSrcUserName2, strSrcSipNumber2;
			FormatItemContact(strSrcUserName2, strSrcHeadImage2, strSrcSipNumber2, pMember1);


			CIUIString strSrcHeadImage3, strSrcUserName3, strSrcSipNumber3;
			FormatItemContact(strSrcUserName3, strSrcHeadImage3, strSrcSipNumber3, pMember2);
			DrawGroupFace3(hDC, lprcTarget, strCreatorFace, strFace1, strFace2, strSrcUserName1, strSrcUserName2,strSrcUserName3);
		}
		else if (pGroup->GetMemberCount() >= 4)
		{
			CContactDB *pMember1 = pGroup->GetMember(1)->GetContactDB();
			_ASSERT(NULL != pMember1);
			CIUIString strFace1 = PortUtility::Utf82Unicode_ND(pMember1->m_strHeadImage).c_str();

			CContactDB *pMember2 = pGroup->GetMember(2)->GetContactDB();
			_ASSERT(NULL != pMember2);
			CIUIString strFace2 = PortUtility::Utf82Unicode_ND(pMember2->m_strHeadImage).c_str();

			CContactDB *pMember3 = pGroup->GetMember(3)->GetContactDB();
			_ASSERT(NULL != pMember3);
			CIUIString strFace3 = PortUtility::Utf82Unicode_ND(pMember3->m_strHeadImage).c_str();

			CIUIString strSrcHeadImage1, strSrcUserName1, strSrcSipNumber1;
			FormatItemContact(strSrcUserName1, strSrcHeadImage1, strSrcSipNumber1, pMember);

			CIUIString strSrcHeadImage2, strSrcUserName2, strSrcSipNumber2;
			FormatItemContact(strSrcUserName2, strSrcHeadImage2, strSrcSipNumber2, pMember1);

			CIUIString strSrcHeadImage3, strSrcUserName3, strSrcSipNumber3;
			FormatItemContact(strSrcUserName3, strSrcHeadImage3, strSrcSipNumber3, pMember2);

			CIUIString strSrcHeadImage4, strSrcUserName4, strSrcSipNumber4;
			FormatItemContact(strSrcUserName4, strSrcHeadImage4, strSrcSipNumber4, pMember3);
			DrawGroupFace4(hDC, lprcTarget, strCreatorFace, strFace1, strFace2, strFace3, strSrcUserName1, strSrcUserName2, strSrcUserName3, strSrcUserName4);
		}
		else
		{
			_ASSERT(FALSE);
		}
	}
	else
	{
		_ASSERT(FALSE);
	}

	return 0;
}


CIUIString  PortGo::FormatEmojiText(CIUIString strFilePureName)
{
	if (strFilePureName.IsEmpty())
	{
		return L"";
	}
	
	CIUIString strFormatFileName = "[:" + strFilePureName + "]";
	return strFormatFileName;
	
}
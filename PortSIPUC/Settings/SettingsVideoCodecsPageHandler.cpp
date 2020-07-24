#include "stdafx.h"
#include "SettingsVideoCodecsPageHandler.h"
#include "System/SystemEx.h"

CSettingsVideoCodecsPageHandler::CSettingsVideoCodecsPageHandler()
	: m_pTreeVideoCodec(NULL)
{
	m_bIsOpen = false;
}

CSettingsVideoCodecsPageHandler::~CSettingsVideoCodecsPageHandler()
{
}

int CSettingsVideoCodecsPageHandler::InitControls()
{
	m_pTreeVideoCodec = (TreeView *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_TREECTRL_VIDEO_CODEC")));
	_ASSERT(m_pTreeVideoCodec != NULL);

	return 0;
}

int CSettingsVideoCodecsPageHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	LoadOptions();
	m_bIsOpen = true;
	return 0;
}

LRESULT CSettingsVideoCodecsPageHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INITDIALOG == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
		}
	}

	if (m_pTreeVideoCodec->GetSafeHwnd() == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_NOTIFY == pe->RoutedEvent)
			{
				NMHDR *pnmhdr = (NMHDR *)pe->lParam;
				if (TVN_BEGINDRAG == pnmhdr->code)
				{
					NMTREEVIEW *pnmtree = (NMTREEVIEW *)pe->lParam;

					DWORD			dwEffect = 0;
					HRESULT			hr = S_OK;
					IDataObject 	*pdo = NULL;
					IDropSource 	*pds = NULL;

					do
					{
						pdo = CTreeItemDataObject::Create(pe->hOriginalSender,
							m_uCodecTreeDropItemClipFormat, pnmtree->itemNew.hItem);
						if (NULL == pdo)
						{
							hr = E_OUTOFMEMORY;
							break;
						}

						pds = new CCodecTreeDropSource();
						if (NULL == pds)
						{
							hr = E_OUTOFMEMORY;
							break;
						}

						// Set allowable effects
						dwEffect = DROPEFFECT_MOVE;

						// start drag-drop operation
						hr = ::DoDragDrop(pdo, pds, dwEffect, &dwEffect);

						if (SUCCEEDED(hr))
						{
							hr = NOERROR;
						}
					} while (false);

					if (NULL != pdo)
					{
						pdo->Release();
					}
					if (NULL != pds)
					{
						pds->Release();
					}
				}
			}
			else if (DDM_OLEDROPTARGET == pe->RoutedEvent)
			{
				if (DDT_DRAGENTER == pe->wParam || DDT_DRAGOVER == pe->wParam)
				{
					DropInfo *pDropInfo = (DropInfo *)pe->lParam;
					if (NULL != pDropInfo)
					{
						FORMATETC formatEtc;
						LPFORMATETC lpFormatEtc = NULL;
						lpFormatEtc = _IUIFillFormatEtc(lpFormatEtc,
							m_uCodecTreeDropItemClipFormat, &formatEtc);

						if (NULL != pDropInfo->lpDataObject)
						{
							HRESULT hr = pDropInfo->lpDataObject->QueryGetData(lpFormatEtc);
							if (hr != S_OK)
							{
								pDropInfo->dropEffect = DROPEFFECT_NONE;
								pe->bHandled = TRUE;
								return E_FAIL;
							}
							else
							{
								IUITRACE(_T("sssssssssssss Move\r\n"));
								pDropInfo->dropEffect = DROPEFFECT_MOVE;
							}
						}
						else
						{
							// 							pDropInfo->dropEffect = DROPEFFECT_NONE;
						}
					}
					else
					{
						_ASSERT(FALSE);
					}
				}
				else if (DDT_DROP == pe->wParam)
				{
					DropInfo *pDropInfo = (DropInfo *)pe->lParam;
					if (NULL != pDropInfo)
					{
						FORMATETC formatEtc;
						LPFORMATETC lpFormatEtc = NULL;
						lpFormatEtc = _IUIFillFormatEtc(lpFormatEtc,
							m_uCodecTreeDropItemClipFormat, &formatEtc);

						if (NULL != pDropInfo->lpDataObject)
						{
							HRESULT hr = pDropInfo->lpDataObject->QueryGetData(lpFormatEtc);
							if (hr == S_OK)
							{
								STGMEDIUM ste;
								hr = pDropInfo->lpDataObject->GetData(lpFormatEtc, &ste);
								if (S_OK == hr)
								{
									HTREEITEM hDragItem = (HTREEITEM)ste.hGlobal;
									CIUIPoint pt = pDropInfo->point;
									m_pTreeVideoCodec->ScreenToClient(&pt);
									HTREEITEM hTargetItem = m_pTreeVideoCodec->HitTest(pt);
									if (hDragItem != hTargetItem && hDragItem != NULL && hTargetItem != NULL)
									{
										// ÒÆ¶¯Item
										HTREEITEM hParent = m_pTreeVideoCodec->GetParentItem(hTargetItem);
										HTREEITEM hPreItem = NULL;
										ITEM_POS enumItemPos = CompareItemPosByFirstItem(m_pTreeVideoCodec,hDragItem, hTargetItem);
										if (enumItemPos == ITEM_FRONT)
										{
											hPreItem = m_pTreeVideoCodec->GetNextItem(hTargetItem, TVGN_PREVIOUS);
										}
										else if (enumItemPos == ITEM_BEHIND)
										{
											hPreItem = hTargetItem;
										}
										else
										{
											return 0;
										}

										int nCodecDes = m_pTreeVideoCodec->GetItemData(hTargetItem);
										int nCodecSrc = m_pTreeVideoCodec->GetItemData(hDragItem);
							
										BOOL bDelete = m_pTreeVideoCodec->DeleteItem(hDragItem);

										if (bDelete)
										{
											CControlProp *pItemProp = g_pProject->FindProjectItem(_T("IDW_CODEC_ITEM.xml"));
											_ASSERT(pItemProp != NULL);
											CControlProp *pBindItemProp = (CControlProp *)pItemProp->GetChild(0);
											_ASSERT(pBindItemProp != NULL);

											HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindItemProp,
												m_pTreeVideoCodec->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

											HWLWND hStaItem = FindControl(GetBindWLWnd(), _T("IDC_STA_CODEC_ITEM"));
											_ASSERT(hStaItem != NULL);
											vector<CAVCodec>::iterator iterFind;
											CAVCodec oSrcAVCodec=CSystemEx::GetInstance()->GetCodecByID(nCodecSrc, iterFind);

											
											Button *pItemBtn = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_CODE_ITEM")));
											_ASSERT(pItemBtn != NULL);
											pItemBtn->SetCheck(oSrcAVCodec.m_bAVCodecOpen==true? BST_CHECKED:BST_UNCHECKED);
											IUI::SetWindowText(hStaItem, PortUtility::Utf82Unicode_ND(oSrcAVCodec.m_strCodecName).c_str());
											pItemBtn->SetUserData(oSrcAVCodec.m_nAVCodecID);
											HTREEITEM hItem = m_pTreeVideoCodec->InsertItem(_T(""), hParent, hPreItem == NULL ? TVI_FIRST : hPreItem);
											m_pTreeVideoCodec->SetItemData(hItem, (DWORD_PTR)oSrcAVCodec.m_nAVCodecID);
											m_pTreeVideoCodec->SetItemBindWindowless(hItem, hBind);
										}
										//m_pTreeVideoCodec->MoveItemTo(hDragItem, hParent, hPreItem == NULL ? TVI_FIRST : hPreItem);
									}
									 RefreshItemIndex();
								}
								else
								{
									_ASSERT(FALSE);
								}
							}
						}
						else
						{
						}
					}
					else
					{
						_ASSERT(FALSE);
					}
				}
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
	}

	return 0;
}

LRESULT CSettingsVideoCodecsPageHandler::OnButtonClickItemBtn(RoutedEventArgs *pe)
{
	Button *pBtnItem = (Button*)CWLWnd::FromHandle(pe->hOriginalSender);
	if (pBtnItem != NULL && pBtnItem->IsWindow())
	{
		int nBtnStatus = pBtnItem->GetCheck();
		int nCodecID = pBtnItem->GetUserData();

		if (nCodecID >= 0)
		{
			bool bOpen = false;
			nBtnStatus == BST_CHECKED ? bOpen = true : bOpen = false;
			CSystemEx::GetInstance()->RefreshCodecInfoSwitch(bOpen, nCodecID);
		}
	}
	return 0;
}
LRESULT CSettingsVideoCodecsPageHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (pe->hOriginalSender!=NULL)
	{
		OnButtonClickItemBtn(pe);
	}
	return 0;
}

void CSettingsVideoCodecsPageHandler::LoadOptions()
{
	CIUIString strContens = _T("DragVideoCodecTreeItem");
	m_uCodecTreeDropItemClipFormat = ::RegisterClipboardFormat(strContens);	

	CHWLWNDDropTarget *pCodecTreeDropTarget = new CHWLWNDDropTarget(m_pTreeVideoCodec->GetSafeHwnd());
	IUI::RegisterDragDrop(m_pTreeVideoCodec->GetSafeHwnd(), pCodecTreeDropTarget);

	CControlProp *pItemProp = g_pProject->FindProjectItem(_T("IDW_CODEC_ITEM.xml"));
	_ASSERT(pItemProp != NULL);
	CControlProp *pBindItemProp = (CControlProp *)pItemProp->GetChild(0);
	_ASSERT(pBindItemProp != NULL);
	auto vecVideoCodec = CSystemEx::GetInstance()->GetVideoCodec();
	auto iterVecVideoCodec = vecVideoCodec.begin();
	for (iterVecVideoCodec; iterVecVideoCodec != vecVideoCodec.end(); iterVecVideoCodec++)
	{
		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindItemProp,
			m_pTreeVideoCodec->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

		HWLWND hStaItem = FindControl(GetBindWLWnd(), _T("IDC_STA_CODEC_ITEM"));
		_ASSERT(hStaItem != NULL);
		IUI::SetWindowText(hStaItem, PortUtility::Utf82Unicode_ND(iterVecVideoCodec->m_strCodecName).c_str());


		Button *pItemBtn = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_CODE_ITEM")));
		_ASSERT(pItemBtn != NULL);
		pItemBtn->SetCheck(iterVecVideoCodec->m_bAVCodecOpen == true ? BST_CHECKED : BST_UNCHECKED);
		pItemBtn->SetUserData(iterVecVideoCodec->m_nAVCodecID);
		HTREEITEM hItem = m_pTreeVideoCodec->InsertItem(_T(""));
		m_pTreeVideoCodec->SetItemData(hItem, (DWORD_PTR)iterVecVideoCodec->m_nAVCodecID);
		m_pTreeVideoCodec->SetItemBindWindowless(hItem, hBind);
	}
}
void CSettingsVideoCodecsPageHandler::RefreshItemIndex()
{
	HTREEITEM hRoot = m_pTreeVideoCodec->GetRootItem();
	int nItemIndex = 0;
	while (hRoot != NULL)
	{

		int nCodecID = m_pTreeVideoCodec->GetItemData(hRoot);
		CSystemEx::GetInstance()->RefreshCodecInfoItemIndex(nItemIndex, nCodecID);

		hRoot = m_pTreeVideoCodec->GetNextSiblingItem(hRoot);
		if (hRoot == NULL)
		{
			break;
		}
		nItemIndex++;
	}
}
void CSettingsVideoCodecsPageHandler::Save()
{
	if (m_bIsOpen == false)
	{
		return;
	}
	CSystemEx::GetInstance()->UpdateVideoCodec2DB();
	CSystemEx::GetInstance()->InitVideoCodecs();
}
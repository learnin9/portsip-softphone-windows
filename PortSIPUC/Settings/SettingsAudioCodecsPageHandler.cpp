#include "stdafx.h"
#include "SettingsAudioCodecsPageHandler.h"
#include "System/SystemEx.h"

CSettingsAudioCodecsPageHandler::CSettingsAudioCodecsPageHandler()
	: m_pTreeAudioCodec(NULL)
{
	m_bIsOpen = false;
}

CSettingsAudioCodecsPageHandler::~CSettingsAudioCodecsPageHandler()
{
}

int CSettingsAudioCodecsPageHandler::InitControls()
{
	m_pTreeAudioCodec = (TreeView *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_TREECTRL_AUDIO_CODEC")));
	_ASSERT(m_pTreeAudioCodec != NULL);

	return 0;
}

int CSettingsAudioCodecsPageHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	LoadOptions();

	m_bIsOpen = true;
	return 0;
}

LRESULT CSettingsAudioCodecsPageHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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
	

	if (m_pTreeAudioCodec->GetSafeHwnd() == pe->hOriginalSender)
	{
		
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (m_pTreeAudioCodec->IsWindowVisible())
				m_pTreeAudioCodec->ShowScrollBar(SB_VERT, FALSE);

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
									m_pTreeAudioCodec->ScreenToClient(&pt);
									HTREEITEM hTargetItem = m_pTreeAudioCodec->HitTest(pt);
									if (hDragItem != hTargetItem && hDragItem != NULL && hTargetItem != NULL)
									{
										HTREEITEM hParent = m_pTreeAudioCodec->GetParentItem(hTargetItem);
										HTREEITEM hPreItem = NULL;
										ITEM_POS enumItemPos = CompareItemPosByFirstItem(m_pTreeAudioCodec, hDragItem, hTargetItem);
										if (enumItemPos == ITEM_FRONT)
										{
											hPreItem = m_pTreeAudioCodec->GetNextItem(hTargetItem, TVGN_PREVIOUS);
										}
										else if (enumItemPos == ITEM_BEHIND)
										{
											hPreItem = hTargetItem;
										}
										else
										{
											return 0;
										}

										int nCodecDes = m_pTreeAudioCodec->GetItemData(hTargetItem);
										int nCodecSrc = m_pTreeAudioCodec->GetItemData(hDragItem);

										BOOL bDelete = m_pTreeAudioCodec->DeleteItem(hDragItem);

										if (bDelete)
										{
											CControlProp *pItemProp = g_pProject->FindProjectItem(_T("IDW_CODEC_ITEM.xml"));
											_ASSERT(pItemProp != NULL);
											CControlProp *pBindItemProp = (CControlProp *)pItemProp->GetChild(0);
											_ASSERT(pBindItemProp != NULL);

											HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindItemProp,
												m_pTreeAudioCodec->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

											HWLWND hStaItem = FindControl(GetBindWLWnd(), _T("IDC_STA_CODEC_ITEM"));
											_ASSERT(hStaItem != NULL);
											vector<CAVCodec>::iterator iterFind;
											CAVCodec oSrcAVCodec = CSystemEx::GetInstance()->GetCodecByID(nCodecSrc, iterFind);

											IUI::SetWindowText(hStaItem, PortUtility::Utf82Unicode_ND(oSrcAVCodec.m_strCodecName).c_str());

											Button *pItemBtn = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_CODE_ITEM")));
											_ASSERT(pItemBtn != NULL);
											pItemBtn->SetCheck(oSrcAVCodec.m_bAVCodecOpen == true ? BST_CHECKED : BST_UNCHECKED);
											pItemBtn->SetUserData(oSrcAVCodec.m_nAVCodecID);
											HTREEITEM hItem = m_pTreeAudioCodec->InsertItem(_T(""), hParent, hPreItem == NULL ? TVI_FIRST : hPreItem);
											m_pTreeAudioCodec->SetItemData(hItem, (DWORD_PTR)oSrcAVCodec.m_nAVCodecID);
											m_pTreeAudioCodec->SetItemBindWindowless(hItem, hBind);
										}
										//m_pTreeVideoCodec->MoveItemTo(hDragItem, hParent, hPreItem == NULL ? TVI_FIRST : hPreItem);
										RefreshItemIndex();
									}
									int n = 0;
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



LRESULT CSettingsAudioCodecsPageHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (pe->hOriginalSender)
	{
		OnButtonClickItemBtn(pe);
	}
	return 0;
}

void CSettingsAudioCodecsPageHandler::RefreshItemIndex()
{
	HTREEITEM hRoot = m_pTreeAudioCodec->GetRootItem();
	int nItemIndex = 0;
	while (hRoot != NULL)
	{
		
		int nCodecID = m_pTreeAudioCodec->GetItemData(hRoot);
		CSystemEx::GetInstance()->RefreshCodecInfoItemIndex(nItemIndex, nCodecID);

		hRoot = m_pTreeAudioCodec->GetNextSiblingItem(hRoot);
		if (hRoot == NULL)
		{
			break;
		}
		nItemIndex++;
	}
}

LRESULT CSettingsAudioCodecsPageHandler::OnButtonClickItemBtn(RoutedEventArgs *pe)
{
	Button *pBtnItem =(Button*)CWLWnd::FromHandle(pe->hOriginalSender);
	if (pBtnItem!=NULL&&pBtnItem->IsWindow())
	{
		int nBtnStatus = pBtnItem->GetCheck();
		int nCodecID = pBtnItem->GetUserData();
		
		if (nCodecID>=0)
		{
			bool bOpen = false;
			nBtnStatus == BST_CHECKED ? bOpen= true: bOpen = false;
			if (bOpen==false)
			{
				int nOpenNum = CSystemEx::GetInstance()->GetAudioCodecOpen();
				if (nOpenNum<=1)
				{
					CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"Info", L"Please turn on at least one audio codes", NULL);
					pBtnItem->SetCheck(BST_CHECKED);
					return 0;
				}
			}
			CSystemEx::GetInstance()->RefreshCodecInfoSwitch(bOpen,nCodecID);
		}
	}
	return 0;
}


void CSettingsAudioCodecsPageHandler::LoadOptions()
{
	CIUIString strContens = _T("DragAudioCodecTreeItem");
	m_uCodecTreeDropItemClipFormat = ::RegisterClipboardFormat(strContens);	

	CHWLWNDDropTarget *pCodecTreeDropTarget = new CHWLWNDDropTarget(m_pTreeAudioCodec->GetSafeHwnd());
	IUI::RegisterDragDrop(m_pTreeAudioCodec->GetSafeHwnd(), pCodecTreeDropTarget);

	CControlProp *pItemProp = g_pProject->FindProjectItem(_T("IDW_CODEC_ITEM.xml"));
	_ASSERT(pItemProp != NULL);
	CControlProp *pBindItemProp = (CControlProp *)pItemProp->GetChild(0);
	_ASSERT(pBindItemProp != NULL);
	auto vecAudiodeoCodec = CSystemEx::GetInstance()->GetAudioCodec();
	auto iterVecAudioCodec = vecAudiodeoCodec.begin();
	for (iterVecAudioCodec; iterVecAudioCodec != vecAudiodeoCodec.end(); iterVecAudioCodec++)
	{
		HWLWND hBind = CreatePreDefineChildControl(NULL, NULL, pBindItemProp,
			m_pTreeAudioCodec->GetSafeHwnd(), GetHostWnd(GetBindWLWnd()), 0);

		HWLWND hStaItem = FindControl(GetBindWLWnd(), _T("IDC_STA_CODEC_ITEM"));
		_ASSERT(hStaItem != NULL);
		IUI::SetWindowText(hStaItem, PortUtility::Utf82Unicode_ND(iterVecAudioCodec->m_strCodecName).c_str());
		Button *pItemBtn = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_CHK_CODE_ITEM")));
		pItemBtn->SetUserData(iterVecAudioCodec->m_nAVCodecID);
		_ASSERT(pItemBtn != NULL);
		pItemBtn->SetCheck(iterVecAudioCodec->m_bAVCodecOpen == true ? BST_CHECKED : BST_UNCHECKED);
		HTREEITEM hItem = m_pTreeAudioCodec->InsertItem(_T(""));
		m_pTreeAudioCodec->SetItemData(hItem, (DWORD_PTR)iterVecAudioCodec->m_nAVCodecID);
		m_pTreeAudioCodec->SetItemBindWindowless(hItem, hBind);
	}
}

bool  CSettingsAudioCodecsPageHandler::Save()
{
	if (m_bIsOpen == false)
	{
		return true;
	}
	CSystemEx::GetInstance()->UpdateAudioCodec2DB();
	CSystemEx::GetInstance()->InitAudioCodecs();
	return true;
}
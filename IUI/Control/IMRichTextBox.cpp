#include "StdAfx.h"

#if(_MSC_VER <= 1200) // VC6.0
#include <ATLBASE.H>
#endif

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

#define GetProp_IMDATA _T("GetProp_IMDATA")

IMRichTextBox::IMRichTextBox()
{
}

IMRichTextBox::~IMRichTextBox(void)
{
}

int IMRichTextBox::ReleaseObject()
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	// 释放从OleControl派生的ole资源
	CComPtr<IRichEditOle> pRichEditOle = GetIRichEditOle();

	int i = 0;
	LONG nOleCount = pRichEditOle->GetObjectCount();
	for (i = nOleCount - 1; i >= 0 ; --i)
	{
		REOBJECT ro;
		ro.cbStruct = sizeof(REOBJECT);
		HRESULT hr = pRichEditOle->GetObject(i, &ro, REO_GETOBJ_POLEOBJ);
		if (FAILED(hr))
		{
			continue;
		}

		// 表示这个ole是从OleControl派生的。
		OleControl *pIUIOle = NULL;
		ro.poleobj->QueryInterface(IID_IUIOleControl, (void **)&pIUIOle);
		if (NULL != pIUIOle)
		{
			pIUIOle->Release();

			SetSel(ro.cp, ro.cp + 1);
			ReplaceSel(NULL);

			delete pIUIOle;
		}
	}

	// 清空消息
	pBox->m_lstMsg.clear();

	return 0;
}

int IMRichTextBox::BindStyle(const CIMRichTextBoxProp *pRichEditProp)
{
	if (pRichEditProp == NULL)
	{
		return -2;
	}
	CONTROL_TYPE ect = (CONTROL_TYPE)pRichEditProp->m_eControlType;
	if (ect != CT_IM)
	{
		return -3;
	}

	RichTextBox_BindStyle(m_hWnd, pRichEditProp);

	return 0;
}

// 为了性能考虑，调用InsertMsg前，不需要解析消息，因为并不是所有从服务器拉到的消息
// 都会插入到RichEdit。 所以在InsertMsg内部，要解析消息格式。
int IMRichTextBox::InsertMsg(int nIndex, const CIMMsg *pMsg)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_INSERT_MSG, nIndex, (LPARAM)pMsg);
}

int IMRichTextBox::DeleteMsg(int nIndex)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_DELETE_MSG, nIndex, 0);
}

int IMRichTextBox::ShowResendButton(UINT uMsgID, BOOL bShow)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_SHOW_RESEND_BUTTON, uMsgID, bShow);
}

BOOL IMRichTextBox::IsMsgShowResendButton(UINT uMsgID) const
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_IS_RESEND_BUTTON_VISIBLE, uMsgID, 0);
}

int IMRichTextBox::InsertShowMoreMessage(UINT uMsgID, LPCTSTR lpszText)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_INSERT_SHOW_MORE_MSG, uMsgID, LPARAM(lpszText));
}

int IMRichTextBox::InsertLabel(
	UINT nIndex,
	UINT uMsgID,
	LPCTSTR lpszText,
	SIZE sizeLabel,
	const CLabelProp *pBindStyle)
{
	_ASSERT(IsWindow());
	CLabelMsg lm;
	lm.m_nIndex = nIndex;
	lm.m_uMsgID = uMsgID;
	lm.m_lpszText = lpszText;
	lm.m_size = sizeLabel;
	lm.m_pBindStyle = pBindStyle;
	return SendMessage(IMM_INSERT_LABEL, 0, LPARAM(&lm));
}

int IMRichTextBox::SetSenderIndent(int nLeftIndent)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nSenderLeftIndent = nLeftIndent;

	return 0;
}

int IMRichTextBox::GetSenderIndent() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nSenderLeftIndent;
}

int IMRichTextBox::SetMsgLeftIndent(int nLeftIndent)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nMsgLeftIndent = nLeftIndent;

	return 0;
}

int IMRichTextBox::GetMsgLeftIndent() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nMsgLeftIndent;
}

int IMRichTextBox::SetMsgRightIndent(int nRightIndent)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nMsgRightIndent = nRightIndent;

	return 0;
}

int IMRichTextBox::GetMsgRightIndent() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nMsgRightIndent;
}


int IMRichTextBox::SetMaxRelayoutWidth(int nWidth)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nMaxRelayoutWidth = nWidth;

	return 0;
}

int IMRichTextBox::GetMaxRelayoutWidth() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nMaxRelayoutWidth;
}

int IMRichTextBox::SetMsgSenderTitleAlias(int nLevel, LPCTSTR lpszAlias)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_mapMsgSenderTitleAlias[nLevel] = lpszAlias;

	return 0;
}

int IMRichTextBox::ShowMsgSenderTitle(BOOL bShow)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_bShowMsgSenderTitle = bShow;

	return 0;
}

BOOL IMRichTextBox::IsShowMsgSenderTitle() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_bShowMsgSenderTitle;
}

int IMRichTextBox::ShowMsgSenderAlias(BOOL bShow)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_bShowMsgSenderAlias = bShow;

	return 0;
}

BOOL IMRichTextBox::IsShowMsgSenderAlias() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_bShowMsgSenderAlias;
}

int IMRichTextBox::SetTitleSpaceBefore(int nY)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nTitleSpaceBefore = nY;

	return 0;
}

int IMRichTextBox::GetTitleSpaceBefor() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nTitleSpaceBefore;
}

int IMRichTextBox::SetTitleSpaceAfter(int nY)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nTitleSpaceAfter = nY;

	return 0;
}

int IMRichTextBox::GetTitleSpaceAfter() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nTitleSpaceAfter;
}

int IMRichTextBox::SetTitleIndents(int nLeft)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nTitleLeftIndents = nLeft;

	return 0;
}

int IMRichTextBox::GetTitleIndents() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nTitleLeftIndents;
}

int IMRichTextBox::SetTitleColor(COLORREF cr)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_crTitle = cr;

	return 0;
}

COLORREF IMRichTextBox::GetTitleColor() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_crTitle;
}

int IMRichTextBox::SetFaceSize(SIZE sizeFace)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_sizeFace = sizeFace;

	return 0;
}

SIZE IMRichTextBox::GetFaceSize() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_sizeFace;
}

int IMRichTextBox::ShowLeftFace(BOOL bShow)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_bShowLeftFace = bShow;

	return 0;
}

BOOL IMRichTextBox::IsShowLeftFace() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_bShowLeftFace;
}

int IMRichTextBox::ShowRightFace(BOOL bShow)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_bShowRightFace = bShow;

	return 0;
}

BOOL IMRichTextBox::IsShowRightFace() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_bShowRightFace;
}

int IMRichTextBox::GetFirstVisibleMsg() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	int nRet = -1;

	int nFirstVisibleLine = GetFirstVisibleLine();
#ifdef _DEBUG
	int nFirstVisibleCp = LineIndex(nFirstVisibleLine);
#endif // _DEBUG

	// 遍历所有消息
	std::list<CIMMsg>::const_iterator it = pBox->m_lstMsg.begin();
	for (int nIndex = 0; it != pBox->m_lstMsg.end(); ++it, ++nIndex)
	{
		const CIMMsg *pMsg = &(*it);

		if (pMsg->GetMsgStart() + pMsg->GetMsgEnd() >= nFirstVisibleLine)
		{
			nRet = nIndex;
			break;
		}
	}

	return nRet;
}

int IMRichTextBox::FindMsg(int nMsgID, __out CIMMsg *pMsgRet)
{
	// 遍历所有消息
	int nIndexRet = -1;

	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		CIMMsg *pMsg = &(*it);

		if (pMsg->GetMsgID() == nMsgID)
		{
			nIndexRet = std::distance(pBox->m_lstMsg.begin(), it);

			if (NULL != pMsgRet)
			{
				*pMsgRet = *pMsg;
			}

			break;
		}
	}

	return nIndexRet;
}

int IMRichTextBox::GetMsg(int nMsgIndex, __out CIMMsg *pMsg)
{
	if (NULL == pMsg)
	{
		return -1;
	}

	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	std::advance(it, nMsgIndex);

	if (it == pBox->m_lstMsg.end())
	{
		return -2;
	}

	*pMsg = *it;

	return 0;
}

#ifdef _DEBUG
// 测试代码。由外部调用，对RichEdit的某些功能进行测试。
int IMRichTextBox::Test()
{

	return 0;
}
#endif // _DEBUG

// 当消息合并时，计算消息在组内的索引。
// 只有普通消息才有组内索引
// （上中下，用DT_TOP，DT_VCENTER和DT_BOTTOM表示）
int IMRichTextBox::GetMsgPosInGroup(int nMsgIndex)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	int nMsgCount = (int)pBox->m_lstMsg.size();
	if (nMsgCount <= 0 || nMsgIndex >= nMsgCount)
	{
		return -2;
	}

	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	std::advance(it, nMsgIndex);
	const CIMMsg *pMsg = &(*it);

	if (MSGTYPE_MSG != pMsg->GetMsgType())
	{
		return -1;
	}

	if (nMsgIndex == 0)
	{
		return DT_TOP;
	}

#ifdef _DEBUG
	int nc = pMsg->m_lstpStrRichMsg.size();
	for (int j = 0; j < nc ; j++)
	{
		std::list<CRichMsgItem>::const_iterator itItem = pMsg->m_lstpStrRichMsg.begin();
		std::advance(itItem, j);
		const CRichMsgItem *pitem = &(*itItem);
		if (pitem->m_strMsgContent.Find(_T("不够再说")) >= 0)
		{
			int n = 0;
			break;
		}
	}
#endif // _DEBUG

	// 记录本消息消息的发送者
	CIUIString strSenderID = pMsg->GetMsgSenderID();

	//
	// 查看上一个消息
	//
	UINT uMsgPos = DT_TOP;
	it = pBox->m_lstMsg.begin();
	std::advance(it, nMsgIndex - 1);

	const CIMMsg *pMsgPrevious = &(*it);

	if (pMsg->GetMsgType() != pMsgPrevious->GetMsgType())
	{
		return DT_TOP;
	}

	if (strSenderID != pMsgPrevious->GetMsgSenderID())
	{
		uMsgPos = DT_TOP;
	}
	else
	{
		uMsgPos = DT_VCENTER;
	}

	//
	// 如果本消息消息的坐标为DT_VCENTER，查看下一个消息。
	// 如果没有下一条消息，或下一条不是同一个联系人，则修正为DT_BOTTOM
	//
	if (uMsgPos == DT_VCENTER)
	{
		if (nMsgIndex + 1 < nMsgCount)
		{
			it = pBox->m_lstMsg.begin();
			std::advance(it, nMsgIndex + 1);

			const CIMMsg *pMsgNext = &(*it);

			if (strSenderID != pMsgNext->GetMsgSenderID())
			{
				uMsgPos = DT_BOTTOM;
			}
		}
		else
		{
			uMsgPos = DT_BOTTOM;
		}
	}

	return uMsgPos;
}

int IMRichTextBox::EnsureMsgVisible(UINT uMsgID)
{
	// 滚动到指定消息
	CIMMsg msg;
	int nMsgIndex = FindMsg(uMsgID, &msg);
	if (nMsgIndex < 0)
	{
		return -1;
	}

	int nFirstVisibleLine = GetFirstVisibleLine();
	int nLineIndex = LineFromChar(msg.GetMsgStart());
	LineScroll(nLineIndex - nFirstVisibleLine, 0);

	return 0;
}

int IMRichTextBox::SetCombineMsg(BOOL bCombine)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	BOOL bOld = pBox->m_bCombineMsg;
	pBox->m_bCombineMsg = bCombine;
	return bOld;
}

BOOL IMRichTextBox::IsCombineMsg() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_bCombineMsg;
}

// 当使用合并消息模式时，设置多久之内的消息需要合并
int IMRichTextBox::SetCombineMsgTimeSpan(int nSecond)
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	pBox->m_nCombineMsgTimeSpan = nSecond;

	return 0;
}

int IMRichTextBox::GetCombineMsgTimeSpan() const
{
	IMBox *pBox = (IMBox *)GetProp(m_hWnd, GetProp_IMDATA);

	return pBox->m_nCombineMsgTimeSpan;
}

int IMRichTextBox::SetSenderTopQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(FALSE, DT_TOP), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_TOP), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSenderTopQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(FALSE, DT_TOP), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_TOP), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetSenderMidQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(FALSE, DT_VCENTER), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_VCENTER), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSenderMidQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(FALSE, DT_VCENTER), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_VCENTER), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetSenderBottomQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(FALSE, DT_BOTTOM), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_BOTTOM), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSenderBottomQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(FALSE, DT_BOTTOM), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(FALSE, DT_BOTTOM), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetSelfTopQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(TRUE, DT_TOP), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_TOP), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSelfTopQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(TRUE, DT_TOP), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_TOP), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetSelfMidQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(TRUE, DT_VCENTER), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_VCENTER), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSelfMidQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(TRUE, DT_VCENTER), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_VCENTER), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetSelfBottomQiPao(LPCTSTR lpszQiPaoFile, LPCRECT lprc9GridArg)
{
	_ASSERT(IsWindow());
	SendMessage(IMM_SET_QIPAO, MAKEWPARAM(TRUE, DT_BOTTOM), (LPARAM)lpszQiPaoFile);
	SendMessage(IMM_SET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_BOTTOM), (LPARAM)lprc9GridArg);

	return 0;
}

int IMRichTextBox::GetSelfBottomQiPao(__out CIUIString *pstrQiPaoFile, __out LPRECT lprc9GridArg)
{
	_ASSERT(IsWindow());

	if (pstrQiPaoFile != NULL)
	{
		*pstrQiPaoFile = (LPCTSTR)SendMessage(IMM_GET_QIPAO, MAKEWPARAM(TRUE, DT_BOTTOM), 0);
	}

	if (lprc9GridArg != NULL)
	{
		SendMessage(IMM_GET_QIPAO_9GRIDARG, MAKEWPARAM(TRUE, DT_BOTTOM), (LPARAM)lprc9GridArg);
	}

	return 0;
}

int IMRichTextBox::SetResendButtonMarign(int nHor, int nVer)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_SET_RESENDBUTTON_MARGIN, nHor, nVer);
}

int IMRichTextBox::GetResendButtonMargin(LPPOINT lpptMargin)
{
	_ASSERT(IsWindow());
	if (NULL == lpptMargin)
	{
		return -1;
	}

	return SendMessage(IMM_GET_RESENDBUTTON_MARGIN,
			WPARAM(&lpptMargin->x), LPARAM(&lpptMargin->y));
}

int IMRichTextBox::SetResendButtonSize(int nWidth, int nHeight)
{
	_ASSERT(IsWindow());
	return SendMessage(IMM_SET_RESENDBUTTON_SIZE, nWidth, nHeight);
}

int IMRichTextBox::GetResendButtonSize(LPSIZE lpsizeResendButton)
{
	_ASSERT(IsWindow());
	if (NULL == lpsizeResendButton)
	{
		return -1;
	}

	return SendMessage(IMM_GET_RESENDBUTTON_SIZE,
			WPARAM(&lpsizeResendButton->cx), LPARAM(&lpsizeResendButton->cy));
}

int IMRichTextBox::SetResendButtonStyle(const CButtonProp *pButtonProp)
{
	_ASSERT(IsWindow());
	if (NULL == pButtonProp)
	{
		return -1;
	}

	return SendMessage(IMM_SET_RESENDBUTTON_STYLE, 0, LPARAM(pButtonProp));
}

const CButtonProp *IMRichTextBox::GetResendButtonStyle() const
{
	_ASSERT(IsWindow());
	return (const CButtonProp *)SendMessage(IMM_GET_RESENDBUTTON_STYLE, 0, 0);
}

int IMRichTextBox::SetShowMoreMsgButtonStyle(const CButtonProp *pButtonProp)
{
	_ASSERT(IsWindow());
	if (NULL == pButtonProp)
	{
		return -1;
	}

	return SendMessage(IMM_SET_SHOWMOREMSG_BUTTON_STYLE, 0, LPARAM(pButtonProp));
}

const CButtonProp *IMRichTextBox::GetShowMoreMsgButtonStyle() const
{
	_ASSERT(IsWindow());
	return (const CButtonProp *)SendMessage(IMM_GET_SHOWMOREMSG_BUTTON_STYLE, 0, 0);
}

int IUI::IMRichTextBox_BindStyle(HWLWND hWnd, const CIMRichTextBoxProp *pCtrlProp)
{
	HWLWND_BindStyle(hWnd, pCtrlProp);

	IMRichTextBox *pRichTextBox = (IMRichTextBox *)CWLWnd::FromHandle(hWnd);

	// 背景
	CIUIString strImageName[1 + COMBINEIMAGESIZE4];
	BOOL bCombineImage = TRUE;
	CControlProp::GetBackground4(pCtrlProp, &bCombineImage, strImageName);
	if (bCombineImage)
	{
		CombineBkImage(hWnd, TRUE);
		SetCombineBkImage(hWnd, strImageName[0]);
	}
	else
	{
		CombineBkImage(hWnd, FALSE);
		pRichTextBox->SetBkImage(CONTROL_STATE_ALL,
			strImageName[1], strImageName[2], strImageName[3], strImageName[4], FALSE);
	}
	SetBkImageResizeMode(hWnd, pCtrlProp->m_eBkImageResizeMode);
	SetBkImage9GridResizeParam(hWnd, &(RECT)pCtrlProp->m_rcBkImage9GridResizeArg);

	CIUIRect rc;
	pRichTextBox->GetClientRect(rc);
	rc.DeflateRect(&(RECT)pCtrlProp->m_rcPadding4Text);
	pRichTextBox->SetRect(rc);

	// 默认字符格式
	CIUIString strFont;
	CControlProp::GetControlFont1(pCtrlProp, &strFont);
	CFontProp *pFontProp = GetProject(hWnd)->GetFontProp(strFont);
	if (pFontProp == NULL)
	{
		pFontProp = GetProject(hWnd)->GetDefaultFontProp();
	}
	if (pFontProp != NULL)
	{
		LOGFONT lf = (LOGFONT)pFontProp->m_logFont;

		CHARFORMAT2 cf;
		GenerateCharFormat(&cf, lf);
		pRichTextBox->SetDefaultCharFormat(cf);
	}

	COLORREF cr;
	GetProject(hWnd)->GetControlTextColor(pCtrlProp, &cr);
	CHARFORMAT2 cf;
	GenerateCharFormat(&cf, cr);
	pRichTextBox->SetDefaultCharFormat(cf);

	pRichTextBox = NULL;

	// IM控件不作为拖放目标(RevokeDragDrop不影响作为拖放源)。
	IUI::RevokeDragDrop(hWnd);

	return 0;
}

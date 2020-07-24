// 本类是制作IM软件聊天记录显示窗口的控件，支持气泡外观样式。
// 通过子类化RichTextBox实现。

// 除了调用IMRichTextBox::InsertMsg时，IM控件是可写的，其它情况全部只读，
// 包含但不限于：键入、删除字符，拖入、拖出、内部拖放文本，剪切、粘贴文本，Redo|Undo文本等。
#include "stdafx.h"
#if(_MSC_VER <= 1200) // VC6.0
#include <ObjIdl.h>
#endif
#include <RichOle.h>
#include <atlbase.h>
#pragma comment (lib, "Imm32.lib")
#include <time.h>

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG

#define GetProp_IMDATA _T("GetProp_IMDATA")
INT	DCT;				// Double Click Time in milliseconds
INT	cxDoubleClk;		// Double click distances
INT cyDoubleClk;		// Double click distances

int DoSetParaFormat(IMBox *pBox);

CTimeStampParam::CTimeStampParam()
	: m_nIndex(-1)
	, m_uMsgID(0)
	, m_time(0)
{

}

CTimeStampParam::~CTimeStampParam()
{

}

CLabelMsg::CLabelMsg()
	: m_nIndex(-1)
	, m_uMsgID(0)
	, m_lpszText(NULL)
	, m_pBindStyle(NULL)
{

}

CLabelMsg::~CLabelMsg()
{

}

CIMMsg::CIMMsg()
{
	m_nVer = 1000;
	m_nMsgID = 0;
	m_eMsgType = MSGTYPE_MSG;
	m_tMsg = GetCurrentTime();
	m_bSelf = FALSE;
	m_eMsgAlignHor = CLH_LEFT;
	m_bShowResendButton = FALSE;
	m_hResendButton = NULL;
	m_himgFace = NULL;
	m_nMsgSenderTitle = 1;
	m_lParam = 0;
	m_hImCtrl = NULL;
	m_nMsgStartCp = 0;
	m_nMsgContentStartCp = 0;
	m_nMsgEndCp = 0;
	m_nMsgParagraphCount = 1;
}

CIMMsg::CIMMsg(const CIMMsg &rSrc)
{
	Copy(rSrc);
}

CIMMsg::~CIMMsg()
{
}

CIMMsg &CIMMsg::operator=(const CIMMsg &rSrc)
{
	if (&rSrc == this)
	{
		return *this;
	}

	Copy(rSrc);
	return *this;
}

int CIMMsg::GetVer() const
{
	return m_nVer;
}

int CIMMsg::SetMsgID(LPARAM nID)
{
	m_nMsgID = nID;

	return 0;
}

LPARAM CIMMsg::GetMsgID() const
{
	return m_nMsgID;
}

int CIMMsg::SetMsgType(IMMSGTYPE eType)
{
	m_eMsgType = eType;

	return 0;
}

IMMSGTYPE CIMMsg::GetMsgType() const
{
	return m_eMsgType;
}

int CIMMsg::SetMsgSenderID(LPCTSTR lpszOwner)
{
	m_strSenderID = lpszOwner;
	return 0;
}

LPCTSTR CIMMsg::GetMsgSenderID() const
{
	return m_strSenderID;
}

int CIMMsg::SetSelf(BOOL bSelf)
{
	m_bSelf = bSelf;
	if (m_bSelf)
	{
		m_eMsgAlignHor = CLH_RIGHT;
	}
	else
	{
		m_eMsgAlignHor = CLH_LEFT;
	}

	return 0;
}

BOOL CIMMsg::IsSelf() const
{
	return m_bSelf;
}

int CIMMsg::SetHorzLayoutMode(CONTROL_LAYOUT_HOR eAlignHor)
{
	m_eMsgAlignHor = eAlignHor;
	return 0;
}

CONTROL_LAYOUT_HOR CIMMsg::GetHorzLayoutMode() const
{
	return m_eMsgAlignHor;
}

int CIMMsg::ShowResendButton(BOOL bShow)
{
	m_bShowResendButton = bShow;

	return 0;
}

BOOL CIMMsg::IsShowResendButton() const
{
	return m_bShowResendButton;
}

int CIMMsg::SetResendButton(HWLWND hResendButton)
{
	m_hResendButton = hResendButton;

	return 0;
}

HWLWND CIMMsg::GetResendButton()
{
	return m_hResendButton;
}

int CIMMsg::SetFace(HIUIIMAGE hImgFace)
{
	m_himgFace = hImgFace;

	return 0;
}

HIUIIMAGE CIMMsg::GetFace() const
{
	return m_himgFace;
}

int CIMMsg::SetMsgSenderTitle(int nTitle)
{
	m_nMsgSenderTitle = nTitle;

	return 0;
}

int CIMMsg::GetMsgSenderTitle() const
{
	return m_nMsgSenderTitle;
}

int CIMMsg::SetMsgSender(LPCTSTR lpszSender)
{
	m_strMsgSender = lpszSender;

	return 0;
}

CIUIString CIMMsg::GetMsgSender() const
{
	return m_strMsgSender;
}

int CIMMsg::SetMsgStart(int nStart)
{
	m_nMsgStartCp = nStart;

	return 0;
}

int CIMMsg::GetMsgStart() const
{
	return m_nMsgStartCp;
}

int CIMMsg::SetMsgContentStart(int nStart)
{
	m_nMsgContentStartCp = nStart;

	return 0;
}

int CIMMsg::GetMsgContentStart() const
{
	return m_nMsgContentStartCp;
}

int CIMMsg::SetMsgEnd(int nEnd)
{
	m_nMsgEndCp = nEnd;

	return 0;
}

int CIMMsg::GetMsgEnd() const
{
	return m_nMsgEndCp;
}

int CIMMsg::SetMsgParagraphCount(int nCount)
{
	m_nMsgParagraphCount = nCount;

	return 0;
}

int CIMMsg::GetMsgParagraphCount() const
{
	return m_nMsgParagraphCount;
}

LPARAM CIMMsg::SetUserData(LPARAM lParam)
{
	LPARAM lOldParam = m_lParam;
	m_lParam = lParam;
	return lOldParam;
}

LPARAM CIMMsg::GetUserData() const
{
	return m_lParam;
}

int CIMMsg::AddMsgItem(const CRichMsgItem *pMsgItem)
{
	if (pMsgItem == NULL)
	{
		return -1;
	}

	CRichMsgItem rmi;
	rmi = *pMsgItem;

	// TODO:
	// ms - help://MS.MSDNQTR.v90.chs/shellcc/platform/commctls/richedit/richeditcontrols/aboutricheditcontrols.htm#_win32_Rich_Edit_Version_1.0
	// 上面的MSDN文档描述了Rich Edit 1.0使用CR/LF字符组合作为段落标记，Rich Edit 2.0仅使用\r
	// Rich Edit 3.0只使用回车字符，但可以模仿Rich Edit 1.0
	// 普通消息，把换行符换成\r
	if (!rmi.m_bOleControl)
	{
		// 把消息中的\r\n替换成\r。 然后把\n也替换为\r。 因为RichEdit只认\r
		rmi.m_strMsgContent.Replace(_T("\r\n"), _T("\r"));
		rmi.m_strMsgContent.Replace(_T("\n"), _T("\r"));

		// 记录消息中有多少个\r（段落）
		int nMsgLength = rmi.m_strMsgContent.GetLength();
		for (int i = 0; i < nMsgLength; ++i)
		{
			if (rmi.m_strMsgContent[i] == '\r')
			{
				int nOldCount = GetMsgParagraphCount();
				SetMsgParagraphCount(nOldCount + 1);
			}
		}
	}

	m_lstpStrRichMsg.push_back(rmi);

	return 0;
}

std::list<CRichMsgItem> *CIMMsg::GetMsgItems()
{
	return &m_lstpStrRichMsg;
}

const std::list<CRichMsgItem> *CIMMsg::GetMsgItems() const
{
	return &m_lstpStrRichMsg;
}

void CIMMsg::Copy(const CIMMsg &rSrc)
{
	// 当增加类成员后，自动断言，下面需要增加赋值语句
	int nSize = sizeof(CIMMsg);
#ifdef _UNICODE
	_ASSERT(nSize == 112);
#else
	_ASSERT(nSize == 92);
#endif
	m_nVer = rSrc.m_nVer;
	m_nMsgID = rSrc.m_nMsgID;
	m_eMsgType = rSrc.m_eMsgType;
	m_strSenderID = rSrc.m_strSenderID;
	m_tMsg = rSrc.m_tMsg;
	m_bSelf = rSrc.m_bSelf;
	m_eMsgAlignHor = rSrc.m_eMsgAlignHor;
	m_bShowResendButton = rSrc.m_bShowResendButton;
	m_hResendButton = rSrc.m_hResendButton;
	m_himgFace = rSrc.m_himgFace;
	if (NULL != m_himgFace)
	{
		m_himgFace->AddRef();
	}
	m_nMsgSenderTitle = rSrc.m_nMsgSenderTitle;
	m_strMsgSender = rSrc.m_strMsgSender;
	m_strRichMsg = m_strRichMsg;
	m_lParam = rSrc.m_lParam;

	m_hImCtrl = rSrc.m_hImCtrl;
	m_nMsgStartCp = rSrc.m_nMsgStartCp;
	m_nMsgContentStartCp = rSrc.m_nMsgContentStartCp;
	m_nMsgEndCp = rSrc.m_nMsgEndCp;
	m_nMsgParagraphCount = rSrc.m_nMsgParagraphCount;
	m_lstpStrRichMsg = rSrc.m_lstpStrRichMsg;
}

BOOL IsMsgShowSender(IMBox *pBox, const CIMMsg *pMsg, BOOL *pbShowAlias)
{
	BOOL bShowSender = TRUE; // 是否显示消息发送者名字
	if (MSGTYPE_MSG != pMsg->GetMsgType())
	{
		bShowSender = FALSE;
	}
	else if (pBox->m_bQiPaoMode)
	{
		if (CLH_RIGHT == pMsg->GetHorzLayoutMode())
		{
			bShowSender = FALSE;
		}

		if (!pBox->m_bShowMsgSenderTitle)
		{
			bShowSender = FALSE;
		}

		// 分组时，只有组内第一个显示
		IMRichTextBox *pIMCtrl = (IMRichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);
		int nMsgIndex = pIMCtrl->FindMsg(pMsg->GetMsgID(), NULL);
		int nPos = pIMCtrl->GetMsgPosInGroup(nMsgIndex);
		if (DT_TOP != nPos)
		{
			bShowSender = FALSE;
		}

		if (bShowSender && NULL != pbShowAlias)
		{
			*pbShowAlias = pBox->m_bShowMsgSenderAlias;
		}
	}

	return bShowSender;
}

int GetMsgInsertCp(IMBox *pBox, const CIMMsg *pMsg, BOOL *pbAddTail)
{
	std::list<CIMMsg>::const_iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		if (it->GetMsgID() == pMsg->GetMsgID())
		{
			break;
		}
	}
	if (it == pBox->m_lstMsg.end())
	{
		_ASSERT(FALSE);
		return -2;
	}

	// 源代码版本的RichEdit，不支持使用-1作为RichEdit的结尾cp。
	// MSDN中，说如果起始为-1, 则取消选中。
	// CTxtEdit::OnSetSel中代码显示，如果cpMin和cpMost都是-1，最终会被修正为(0, -1)，表示全选整个文档
	IMRichTextBox *pIM = (IMRichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);
	int nEnd = pIM->GetTextLengthEx(GTL_DEFAULT | GTL_NUMCHARS);

	// 指向下一个消息
	it++;

	BOOL bAddTail = TRUE;
	int nInsertCp = nEnd;
	if (it == pBox->m_lstMsg.end())
	{
		nInsertCp = nEnd;
		bAddTail = TRUE;
	}
	else
	{
		nInsertCp = it->GetMsgStart();
		bAddTail = FALSE;
	}

	if (NULL != pbAddTail)
	{
		*pbAddTail = bAddTail;
	}

	return nInsertCp;
}

// 把消息格式化后真正插入RichEdit，插入后再设置缩进
// 如果消息是插入中中间而不是尾部，还需要更新后面所有消息的起始cp
int DoInsertMsg(IMBox *pBox, const CIMMsg *pMsg)
{
	if (pMsg == NULL)
	{
		_ASSERT(FALSE);
		return -1;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);
	CComPtr<ITextDocument> pTextDocument = pRichTextBox->GetITextDocument();
	_ASSERT(NULL != (ITextDocument *)pTextDocument);

	//
	// 查找消息的插入位置
	//
	BOOL bAddTail = TRUE;
	const int nInsertCp = GetMsgInsertCp(pBox, pMsg, &bAddTail);
	if (nInsertCp < -1)
	{
		return -2;
	}

	//
	// 选中插入位置准备插入消息
	// TODO: SetSel会影响已选中的文本，换Range试试。
	//
	pRichTextBox->SetSel(nInsertCp, nInsertCp);

	//
	// 如果插入尾部，先插入一个空行
	//
	int nCurPos = nInsertCp;
	if (bAddTail)
	{
		pRichTextBox->ReplaceSel(_T("\r"));
		nCurPos = pRichTextBox->GetTextLengthEx(GTL_DEFAULT | GTL_NUMCHARS);
	}

	//
	// 记录插入的开始位置
	//
	pRichTextBox->SetSel(nCurPos, nCurPos);
	((CIMMsg *)pMsg)->SetMsgStart(nCurPos);

	//
	// 插入消息，消息插入前先格式化
	//

	// 是否显示消息发送者名字
	BOOL bShowAlias = TRUE;
	BOOL bShowSender = IsMsgShowSender(pBox, pMsg, &bShowAlias);

	int nInsertLength = 0;	// 消息插入RichEdit后占用的长度，如果消息插入中间，后续消息的起始位置要顺延。
	int nCurCp = 0;
	if (bShowSender)
	{
		CIUIString strName;

		if (bShowAlias)
		{
			strName = _T("【");
			strName += pBox->m_mapMsgSenderTitleAlias[pMsg->GetMsgSenderTitle()];
			strName += _T("】 ");
		}
		strName += pMsg->GetMsgSender();
		strName += _T("\r");

		pRichTextBox->ReplaceSel(strName);
		LONG lSelStart = 0;
		LONG lSelEnd = 0;
		pRichTextBox->GetSel(lSelStart, lSelEnd);
		int nNameLength = pRichTextBox->LineLength(pMsg->GetMsgStart());

		// 设置Title颜色
		pRichTextBox->SetSel(pMsg->GetMsgStart(), pMsg->GetMsgStart() + nNameLength);
		CHARFORMAT cf = { 0 };
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask |= CFM_COLOR;
		cf.dwEffects &= ~CFE_AUTOCOLOR;
		cf.crTextColor = pBox->m_crTitle;
		pRichTextBox->SetSelectionCharFormat(cf);

		// 设置完Title颜色后，必须重新定位插入位置
		pRichTextBox->SetSel(lSelEnd, lSelEnd);

		// 设置名字的段落格式。只有别人才显示发送者名字，自己是不显示的。
		// 名字永远显示在左侧，所以，只需要在插入消息时，设置名字的段落格式即可，之后都不需要设置。
		CComPtr<ITextRange> pTextRangeSender = NULL;
		pTextDocument->Range(pMsg->GetMsgStart(), pMsg->GetMsgStart() + nNameLength, &pTextRangeSender);

#ifdef _DEBUG
		BSTR bstr;
		pTextRangeSender->GetText(&bstr);
#endif // _DEBUG

		CComPtr<ITextPara> pTextParaSender = NULL;
		pTextRangeSender->GetPara(&pTextParaSender);
		// 设置行间距
		pTextParaSender->SetSpaceBefore(GetXFPPTSFromPixel(pBox->m_nTitleSpaceBefore));
		pTextParaSender->SetSpaceAfter(GetXFPPTSFromPixel(pBox->m_nTitleSpaceAfter));
		pTextParaSender->SetIndents((float)pBox->m_nTitleLeftIndents,
			GetXFPPTSFromPixel(pBox->m_nSenderLeftIndent), 0);
		pTextRangeSender->SetPara(pTextParaSender);
	}

	//
	// 设置消息正文的cp
	//
	if (bAddTail)
	{
		nCurCp = pRichTextBox->GetTextLengthEx(GTL_DEFAULT | GTL_NUMCHARS);
		((CIMMsg *)pMsg)->SetMsgContentStart(nCurCp - pMsg->GetMsgStart());
	}
	else
	{
		LONG lSelStart = 0;
		LONG lSelEnd = 0;
		pRichTextBox->GetSel(lSelStart, lSelEnd);
		((CIMMsg *)pMsg)->SetMsgContentStart(lSelStart - pMsg->GetMsgStart());
	}

	//
	// 2. 插入消息正文
	//
	std::list<CRichMsgItem>::const_iterator itItem = pMsg->GetMsgItems()->begin();
	for (; itItem != pMsg->GetMsgItems()->end(); ++itItem)
	{
		const CRichMsgItem *pRichMsgItem = &(*itItem);

		//
		// 1 Ole控件
		//
		if (pRichMsgItem->m_bOleControl)
		{
			OleControl *pCustomOleCtrl = new OleControl(1, pBox->m_pREBox->hwnd);

			// 把IUI控件绑定到OLE控件上
			pCustomOleCtrl->SetOleBindWindowless(pRichMsgItem->m_hOleBindWindowless);

			// 把OLE插入Rich Text Box.
			int nDPIedWidth = GetDPIWidth(pRichMsgItem->m_size.cx);
			int nDPIedHeight = GetDPIHeight(pRichMsgItem->m_size.cy);
			pRichTextBox->InsertOle(pCustomOleCtrl, REO_CP_SELECTION, nDPIedWidth, nDPIedHeight);

			// 通知RichEdit父窗口，插入了一个新的自定义OLE控件
			RoutedEventArgs arg;
			arg.hSender = pRichTextBox->GetSafeHwnd();
			arg.hOriginalSender = pRichTextBox->GetSafeHwnd();
			arg.RoutedEvent = REN_ADDED_CUSTOM_OLE;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.lParam = (LPARAM)pCustomOleCtrl;
			LRESULT lr = IUI::RaiseEvent(arg.hOriginalSender, &arg);
		}
		else
		{
			//
			// 普通文本
			//

			long nStartCp = -1;
			long nEndCp = -1;
			pRichTextBox->GetSel(nStartCp, nEndCp);

			_ASSERT(nStartCp == nEndCp);
			pRichTextBox->ReplaceSel(pRichMsgItem->m_strMsgContent);

			long nStartCp2 = -1;
			long nEndCp2 = -1;
			pRichTextBox->GetSel(nStartCp2, nEndCp2);
			_ASSERT(nStartCp2 == nEndCp2);

			// 设置文本颜色
			pRichTextBox->SetSel(nStartCp, nEndCp2);
			CHARFORMAT cf = { 0 };
			cf.cbSize = sizeof(CHARFORMAT);
			cf.dwMask |= CFM_COLOR;
			cf.dwEffects &= ~CFE_AUTOCOLOR;
			cf.crTextColor = pRichMsgItem->m_crText;
			pRichTextBox->SetSelectionCharFormat(cf);

			// 设置完颜色后，必须把插入位置定位到刚插入的地方，否则后面插入的就把文本覆盖了。
			pRichTextBox->SetSel(nEndCp2, nEndCp2);
		}
	}

	// 记录插入的结束位置
	if (bAddTail)
	{
		nCurCp = pRichTextBox->GetTextLengthEx(GTL_DEFAULT | GTL_NUMCHARS);
	}
	else
	{
		LONG lSelStart = 0;
		LONG lSelEnd = 0;
		pRichTextBox->GetSel(lSelStart, lSelEnd);
		nCurCp = lSelEnd;
	}
	((CIMMsg *)pMsg)->SetMsgEnd(nCurCp - pMsg->GetMsgStart());

	//
	// 3. 设置消息体的段落格式。 别人发的消息，永远显示在左侧，只需要在插入消息时设置一次即可。
	//
	CComPtr<ITextRange> pTextRangeMsg = NULL;
	pTextDocument->Range(pMsg->GetMsgStart() + pMsg->GetMsgContentStart(),
		pMsg->GetMsgStart() + pMsg->GetMsgEnd(), &pTextRangeMsg);

#ifdef _DEBUG
	BSTR bstr;
	pTextRangeMsg->GetText(&bstr);
#endif // _DEBUG

	CComPtr<ITextPara> pTextParaMsg = NULL;
	pTextRangeMsg->GetPara(&pTextParaMsg);
	pTextParaMsg->SetIndents(0, GetXFPPTSFromPixel(pBox->m_nMsgLeftIndent),
		GetXFPPTSFromPixel(pBox->m_nMsgRightIndent));
	pTextRangeMsg->SetPara(pTextParaMsg);

	// 为了方便计算行高，每条消息结束，都插入一个空行。
	if (bAddTail)
	{
		pRichTextBox->ReplaceSel(_T("\r"));
	}
	else
	{
		pRichTextBox->ReplaceSel(_T("\r\r"));
	}

	//
	// 平移之后的消息
	//
	if (!bAddTail)
	{
		// 得到当前的位置
		LONG lSelStart = 0;
		LONG lSelEnd = 0;
		pRichTextBox->GetSel(lSelStart, lSelEnd);

		// 消息插入后增加的长度
		int nAddLength = lSelStart - nInsertCp;

		std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
		for (; it != pBox->m_lstMsg.end(); ++it)
		{
			if (it->GetMsgID() == pMsg->GetMsgID())
			{
				++it;
				for (; it != pBox->m_lstMsg.end(); ++it)
				{
					int nOldStartCp = it->GetMsgStart();
					it->SetMsgStart(nOldStartCp + nAddLength);
				}

				break;
			}
		}
	}

	DoSetParaFormat(pBox);

	return 0;
}

// 把消息从RichEdit中真正删除，删除后，还需要更新后面所有消息的起始cp
int DoDeleteMsg(IMBox *pBox, int nIndex)
{
	std::list<CIMMsg>::iterator itMsgItem = pBox->m_lstMsg.begin();
	std::advance(itMsgItem, nIndex);

	if (itMsgItem == pBox->m_lstMsg.end())
	{
		return -1;
	}

	CIMMsg *pMsg = &(*itMsgItem);
	if (pMsg == NULL)
	{
		_ASSERT(FALSE);
		return -2;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);
	CComPtr<ITextDocument> pTextDocument = pRichTextBox->GetITextDocument();
	_ASSERT(NULL != (ITextDocument *)pTextDocument);

	//
	// 选中整个消息后替换为空
	// TODO: SetSel会影响已选中的文本，换Range试试。
	//
	int nStartCp = pMsg->GetMsgStart();
	if (0 == nIndex)
	{
		nStartCp = 0;
	}
	else
	{
		// 把消息前面的换行符也删除掉。
		std::list<CIMMsg>::iterator itMsgItemPrev = pBox->m_lstMsg.begin();
		std::advance(itMsgItemPrev, nIndex - 1);
		CIMMsg *pMsgPrev = &(*itMsgItemPrev);
		nStartCp = pMsgPrev->GetMsgStart() + pMsgPrev->GetMsgEnd();
	}

	int nEndCp = pMsg->GetMsgEnd();
	pRichTextBox->SetSel(nStartCp, pMsg->GetMsgStart() + nEndCp);
	pRichTextBox->ReplaceSel(_T(""));

	// 删除掉的cp长度
	int nDeleteLength = pMsg->GetMsgStart() + nEndCp - nStartCp;

	//
	// 平移之后的消息
	//
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		if (it->GetMsgID() == pMsg->GetMsgID())
		{
			++it;
			for (; it != pBox->m_lstMsg.end(); ++it)
			{
				int nOldStartCp = it->GetMsgStart();
				it->SetMsgStart(nOldStartCp - nDeleteLength);
			}

			break;
		}
	}

	return 0;
}

// 消息显示在RichEdit的左侧还是右侧
BOOL IsMsgShowRight(IMBox *pBox, const CIMMsg *pMsg)
{
	// 只有在气泡模式下，自己发送的消息，在RichEdit宽度小于某个值时，消息在显示到RichEdit右侧
	BOOL bShowLeft = TRUE;
	if (pBox->m_bQiPaoMode && CLH_RIGHT == pMsg->GetHorzLayoutMode())
	{
		CIUIRect rcClient;
		GetClientRect(pBox->m_pREBox->hwnd, rcClient);

		if (rcClient.Width() < pBox->m_nMaxRelayoutWidth)
		{
			bShowLeft = FALSE;
		}
	}

	return !bShowLeft;
}

BOOL IsMsgNewLine(
	IMBox *pBox,
	const CIMMsg *pMsg,
	__out int *pnStartLine,
	__out int *pnEndLine)
{
	if (pMsg == NULL)
	{
		return FALSE;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	// 在没有左缩进的情况下，计算是否需要换行显示
	int nStartLine = pRichTextBox->LineFromChar(pMsg->GetMsgStart() + pMsg->GetMsgContentStart());
	int nEndLine = pRichTextBox->LineFromChar(pMsg->GetMsgStart() + pMsg->GetMsgEnd());
	_ASSERT(nEndLine - nStartLine + 1 >= pMsg->GetMsgParagraphCount());

	if (pnStartLine != NULL)
	{
		*pnStartLine = nStartLine;
	}

	if (pnEndLine != NULL)
	{
		*pnEndLine = nEndLine;
	}

	BOOL bNewLine = (nEndLine - nStartLine + 1 > pMsg->GetMsgParagraphCount());

	return bNewLine;
}

int DoLayoutRightMsg(IMBox *pBox, const CIMMsg *pMsg)
{
	_ASSERT(pBox->m_bQiPaoMode);

	if (IsIconic(GetHostWnd(pBox->m_pREBox->hwnd)))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pBox->m_pREBox->hwnd, rcClient);

	// 消息是否有换行，内部计算时，会把自己发送的消息（需要显示到右侧）的段落格式的左缩进，设置为0.
	int nStartLine = 0;
	int nEndLine = 0;

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	BOOL bNewLine = IsMsgNewLine(pBox, pMsg, &nStartLine, &nEndLine);

	// 有折行的消息，不需要设置左缩进
	if (bNewLine)
	{
		return 1;
	}

	// 没有折行的消息，需要设置左缩进，让消息向右平移
	CIUIRect rcQiPao;

	int nMinLeftLine = MAXINT; // 所有行，left的最小值
	int nMaxRightLine = 0; // 所有行，right的最大值

	for (int i = nStartLine; i <= nEndLine; ++i)
	{
		CIUIPoint ptLine;
		int nLineWidth = 0;
		int nLineHeight = 0;

		if (i == nEndLine)
		{
			pRichTextBox->GetLineRect(i, &ptLine, &nLineWidth, &nLineHeight);
		}
		else
		{
			pRichTextBox->GetLineRect(i, &ptLine, &nLineWidth, NULL);
		}

		if (i == nStartLine)
		{
			rcQiPao.top = ptLine.y;
		}

		if (i == nEndLine)
		{
			rcQiPao.bottom = ptLine.y + nLineHeight;
		}

		nMinLeftLine = min(nMinLeftLine, ptLine.x);
		nMaxRightLine = max(nMaxRightLine, ptLine.x + nLineWidth);
	}

	rcQiPao.left = nMinLeftLine;
	rcQiPao.right = nMaxRightLine;

	// 当气泡需要显示到右侧、设置左缩进时，不能正好设置richedit宽度减
	// 气泡宽度，否则会导致原本可以单行显示的段落换了行。
	// 猜测是还要容纳行尾的回车换行的空间，
	// 这个填充空间的宽度是空格的宽度。（ RichEdit会自动把负偏移设置为0）。
	// nQiPaoLeftOffsetAdjust就是这个填充值
	const int nQiPaoLeftOffsetAdjust = 20; // 这个值，暂时用一个字符宽度比较合适，只会大不会小。
	//nQiPaoLeftOffsetAdjust = GetCharWidthW(hdc, 0, 1, "");

	CIUIRect rcInset;
	IUI::GetLayoutInset(pBox->m_pREBox->hwnd, rcInset);

	rcClient.DeflateRect(rcInset);

	int nXOffset = rcClient.Width() - pBox->m_nMsgRightIndent - rcQiPao.Width() - nQiPaoLeftOffsetAdjust;
	if (nXOffset < 0)
	{
		_ASSERT(FALSE);
		nXOffset = 0;
		return 0;
	}

	rcQiPao.OffsetRect(-rcQiPao.left, 0);
	rcQiPao.OffsetRect(nXOffset, 0);

	CComPtr<ITextDocument> pTextDocument = pRichTextBox->GetITextDocument();

	CComPtr<ITextRange> pTextRange = NULL;
	pTextDocument->Range(pMsg->GetMsgStart() + pMsg->GetMsgContentStart(),
		pMsg->GetMsgStart() + pMsg->GetMsgEnd(), &pTextRange);

#ifdef _DEBUG
	BSTR bstr;
	pTextRange->GetText(&bstr);
#endif // _DEBUG

	CComPtr<ITextPara> pTextPara = NULL;
	pTextRange->GetPara(&pTextPara);
	pTextPara->SetIndents(0, GetXFPPTSFromPixel(nXOffset),
		GetXFPPTSFromPixel(pBox->m_nMsgRightIndent));
	pTextRange->SetPara(pTextPara);

	return 0;
}

// “显示更多消息”、“时间戳”等消息需要居中显示，且单行。
int DoLayoutCenterMsg(IMBox *pBox, const CIMMsg *pMsg)
{
	_ASSERT(pBox->m_bQiPaoMode);

	if (IsIconic(GetHostWnd(pBox->m_pREBox->hwnd)))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pBox->m_pREBox->hwnd, rcClient);

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	CIUIPoint ptLine;
	int nLineWidth = 0;
	int nLineHeight = 0;
	int nLineIndex = pRichTextBox->LineFromChar(pMsg->GetMsgStart() + pMsg->GetMsgContentStart());
	pRichTextBox->GetLineRect(nLineIndex, &ptLine, &nLineWidth, &nLineHeight);

	int nXOffset = (rcClient.Width() - nLineWidth) / 2;
	if (nXOffset < 0)
	{
		_ASSERT(FALSE);
		nXOffset = 0;
		return 0;
	}

	CComPtr<ITextDocument> pTextDocument = pRichTextBox->GetITextDocument();

	CComPtr<ITextRange> pTextRange = NULL;
	pTextDocument->Range(pMsg->GetMsgStart() + pMsg->GetMsgContentStart(),
		pMsg->GetMsgStart() + pMsg->GetMsgEnd(), &pTextRange);

#ifdef _DEBUG
	BSTR bstr;
	pTextRange->GetText(&bstr);
#endif // _DEBUG

	CComPtr<ITextPara> pTextPara = NULL;
	pTextRange->GetPara(&pTextPara);
	pTextPara->SetIndents(0, GetXFPPTSFromPixel(nXOffset),
		GetXFPPTSFromPixel(pBox->m_nMsgRightIndent));
	pTextRange->SetPara(pTextPara);

	return 0;
}

// 在控件缩放或其它需要更新的时候，设置所有内容的段落格式。
// 由于别人发送的消息，已经在插入的时候，设置过段落格式，且在运行中，不会发生变化
// 所以本接口，只需要重新布局自己发送的消息即可。
int DoSetParaFormat(IMBox *pBox)
{
	if (pBox->m_lstMsg.empty())
	{
		return 1;
	}

	SetULWNoRedraw(GetHostWnd(pBox->m_pREBox->hwnd), TRUE);

	CIUIRect rcClient;
	GetClientRect(pBox->m_pREBox->hwnd, rcClient);

	// 在需要设置段落格式时（例如RichEdit的宽度有变化），要对RichEdit里所有消息设置，这是因为
	// 即使隐藏的消息，在RichEdit宽度变化后，也可能发生折行，这就会导致滚动范围的变化。
	// 例如，一个自己发的消息，现在是不可见状态，显示在窗口右侧，不换行，
	// 左缩进设置为200，文本长度为20个字符，20个字符占用的宽度为400（这时Richedit的宽度为600）。
	// 当RichEdit宽度调整到比200大，但无法容纳20个字符时，比如宽度调整到500，这20个字符就要换行。
	// 这将导致滚动范围变大。但实际上，对于500宽度的RichEdit，这20个字符是不需要换行的。
	// 所以，对于不可见的消息，我们只需要把它的左缩进设置为0即可。不需要计算气泡和头像的坐标，
	// 因为，在滚动到可见范围内时，头像和气泡的坐标，会实时获取的。

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);
	CComPtr<ITextDocument> pTextDocument = pRichTextBox->GetITextDocument();

	// 第一遍循环，把所有自己发送的消息的左缩进，设置为0(如果有固定缩进的话，设置为固定缩进)
	// 这样，在第二遍循环时，取得的可见范围消息才精确。
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		const CIMMsg *pMsg = &(*it);

		// 对于对方发送的消息，因为永远显示在左侧，所以在插入消息的时候，就已经为它设置了段落格式，
		// 在缩放RichEdit的时候，不需要重置。 所以，只需要重置自己发送的消息即可
		if (pBox->m_bQiPaoMode && pMsg->GetHorzLayoutMode() == CLH_RIGHT)
		{
			//
			// 把自己这条消息之前的换行符，移到RichEdit右侧外面
			//
			CIUIRect rcWnd;
			pRichTextBox->GetWindowRectToParent(rcWnd);

			// 使用Com指针，像Range这种借口内部，不可能永久保存返回的ITextRange
			// 所以需要使用Com指针，用完释放，否则会引起内存泄露。
			CComPtr<ITextRange> pTextRangeEnter = NULL;
			pTextDocument->Range(pMsg->GetMsgStart(), pMsg->GetMsgStart() + pMsg->GetMsgContentStart(),
				&pTextRangeEnter);

#ifdef _DEBUG
			BSTR bstr;
			pTextRangeEnter->GetText(&bstr);
#endif // _DEBUG

			CComPtr<ITextPara> pTextParaEnter = NULL;
			pTextRangeEnter->GetPara(&pTextParaEnter);
			pTextParaEnter->SetIndents(0, GetXFPPTSFromPixel(rcWnd.Width() + 100), 0);
			pTextRangeEnter->SetPara(pTextParaEnter);

			//
			// 把消息左缩进重置
			//
			CComPtr<ITextRange> pTextRangeMsg = NULL;
			pTextDocument->Range(pMsg->GetMsgStart() + pMsg->GetMsgContentStart(),
				pMsg->GetMsgStart() + pMsg->GetMsgEnd(), &pTextRangeMsg);

#ifdef _DEBUG
			pTextRangeMsg->GetText(&bstr);
#endif // _DEBUG

			CComPtr<ITextPara> pTextParaMsg = NULL;
			pTextRangeMsg->GetPara(&pTextParaMsg);
			pTextParaMsg->SetIndents(0, GetXFPPTSFromPixel(pBox->m_nMsgLeftIndent),
				GetXFPPTSFromPixel(pBox->m_nMsgRightIndent));
			pTextRangeMsg->SetPara(pTextParaMsg);
		}
	}

	// 第二遍循环，布局需要显示到右侧的所有消息
	for (it = pBox->m_lstMsg.begin(); it != pBox->m_lstMsg.end(); ++it)
	{
		const CIMMsg *pMsg = &(*it);

		if (IsMsgShowRight(pBox, pMsg))
		{
			// 布局消息
			DoLayoutRightMsg(pBox, pMsg);
		}
		else if (CLH_CENTER == pMsg->GetHorzLayoutMode())
		{
			// 布局需要居中显示的消息
			DoLayoutCenterMsg(pBox, pMsg);
		}
	}

	SetULWNoRedraw(GetHostWnd(pBox->m_pREBox->hwnd), FALSE);

	return 0;
}

int DoCalFaceRect(IMBox *pBox, const CIMMsg *pMsg, __out LPRECT lprcFace)
{
	if (pMsg == NULL || lprcFace == NULL)
	{
		return -1;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	CIUIRect rcClient;
	GetClientRect(pBox->m_pREBox->hwnd, rcClient);

	CIUIRect rcInset;
	IUI::GetLayoutInset(pBox->m_pREBox->hwnd, rcInset);

	// 计算头像坐标
	CIUIRect rcFace(0, 0, 0, 0);

	// 对于有句柄的RichEdit，GetCharPos返回的是当前滚动位置相对RichEdit的坐标，
	// 所以，返回的坐标，不需要进行滚动位置修正。
	CIUIPoint ptFace = pRichTextBox->GetCharPos(pMsg->GetMsgStart());

	if (IsMsgShowRight(pBox, pMsg))
	{
		rcFace.right = rcClient.right - rcInset.right - (pBox->m_nMsgRightIndent - pBox->m_sizeFace.cx) / 2;
		rcFace.left = rcFace.right - pBox->m_sizeFace.cx;
		rcFace.top = rcClient.top + ptFace.y;
		rcFace.bottom = rcFace.top + pBox->m_sizeFace.cy;
	}
	else
	{
		rcFace.left = rcClient.left + rcInset.left + (pBox->m_nSenderLeftIndent - pBox->m_sizeFace.cx) / 2;
		rcFace.right = rcFace.left + pBox->m_sizeFace.cx;
		rcFace.top = rcClient.top + ptFace.y;
		rcFace.bottom = rcFace.top + pBox->m_sizeFace.cy;
	}

	*lprcFace = rcFace;

	return 0;
}

// 计算换行(显示的行数是否大于CIMMsg::m_nMsgParagraphCount)消息的气泡坐标
int DoCalNewLineQiPaoRect(IMBox *pBox, const CIMMsg *pMsg, __out LPRECT lprcQiPao)
{
	if (lprcQiPao == NULL)
	{
		return -1;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	CIUIRect rcClient;
	GetClientRect(pBox->m_pREBox->hwnd, rcClient);

	CIUIRect rcInset;
	IUI::GetLayoutInset(pBox->m_pREBox->hwnd, rcInset);
	rcClient.DeflateRect(rcInset);

	// 得到消息第一个字符的上坐标
	CIUIPoint ptFirstChar = pRichTextBox->GetCharPos(pMsg->GetMsgStart() + pMsg->GetMsgContentStart());

	lprcQiPao->left = rcClient.left + pBox->m_nMsgLeftIndent;
	lprcQiPao->right = rcClient.right - pBox->m_nMsgRightIndent;
	lprcQiPao->top = ptFirstChar.y;

	// 本条消息最后一行最后一个字符的右下角坐标
	int nLine = pRichTextBox->LineFromChar(pMsg->GetMsgStart() + pMsg->GetMsgEnd());
	CIUIPoint ptLastMsgLine;
	int nLineHeight = 0;
	pRichTextBox->GetLineRect(nLine, &ptLastMsgLine, NULL, &nLineHeight);

	lprcQiPao->bottom = ptLastMsgLine.y + nLineHeight;

	return 0;
}

// 计算不换行(显示的行数是否大于CIMMsg::m_nMsgParagraphCount)消息显示时的气泡坐标
int DoCalSingleLineQiPaoRect(IMBox *pBox, const CIMMsg *pMsg, int nStartLine, int nEndLine,
	__out LPRECT lprcQiPao)
{
	if (lprcQiPao == NULL)
	{
		return -1;
	}

	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pBox->m_pREBox->hwnd);

	CIUIRect rcQiPao;

	int nMinLeftLine = MAXINT; // 所有行，left的最小值
	int nMaxRightLine = 0; // 所有行，right的最大值

	for (int i = nStartLine; i <= nEndLine; ++i)
	{
		CIUIPoint ptLine;
		int nLineWidth = 0;
		int nLineHeight = 0;

		if (i == nEndLine)
		{
			pRichTextBox->GetLineRect(i, &ptLine, &nLineWidth, &nLineHeight);
		}
		else
		{
			pRichTextBox->GetLineRect(i, &ptLine, &nLineWidth, NULL);
		}

		if (i == nStartLine)
		{
			rcQiPao.top = ptLine.y;
		}

		if (i == nEndLine)
		{
			rcQiPao.bottom = ptLine.y + nLineHeight;
		}

		nMinLeftLine = min(nMinLeftLine, ptLine.x);
		nMaxRightLine = max(nMaxRightLine, ptLine.x + nLineWidth);
	}

	rcQiPao.left = nMinLeftLine;
	rcQiPao.right = nMaxRightLine;

	*lprcQiPao = rcQiPao;

	return 0;
}

// 为了性能和解决显示方面的问题
// 在垂直滚动时，只计算气泡坐标，不设置段落格式，而是使用消息内缓存的折行标志
int DoCalQiPaoRect(IMBox *pBox, const CIMMsg *pMsg, __out LPRECT lprcQiPao)
{
	if (lprcQiPao == NULL)
	{
		return -1;
	}

	// 消息是否有换行，内部计算时，会把自己发送的消息（需要显示到右侧）的段落格式的左缩进，设置为0.
	int nStartLine = 0;
	int nEndLine = 0;
	BOOL bNewLine = IsMsgNewLine(pBox, pMsg, &nStartLine, &nEndLine);

	// 下面的代码，会再次把段落的左缩进，设置为最终值。
	if (bNewLine)
	{
		DoCalNewLineQiPaoRect(pBox, pMsg, lprcQiPao);
	}
	else
	{
		DoCalSingleLineQiPaoRect(pBox, pMsg, nStartLine, nEndLine, lprcQiPao);
	}

	return 0;
}

// 基于气泡坐标计算“重新发送”按钮的坐标
int DoCalResendButtonRect(
	IMBox *pBox,
	const CIMMsg *pMsg,
	__in LPCRECT lprcQiPao,
	__out LPRECT lprcResendButton)
{
	if (lprcResendButton == NULL)
	{
		return -1;
	}

	if (pBox->m_bQiPaoMode)
	{
		if (NULL == lprcQiPao)
		{
			return -2;
		}

		if (IsMsgShowRight(pBox, pMsg))
		{
			lprcResendButton->bottom = lprcQiPao->bottom - pBox->m_ptResendButtonMargin.y;
			lprcResendButton->top = lprcResendButton->bottom - pBox->m_sizeResendButton.cy;
			lprcResendButton->right = lprcQiPao->left - pBox->m_ptResendButtonMargin.x;
			lprcResendButton->left = lprcResendButton->right - pBox->m_sizeResendButton.cx;
		}
		else
		{
			lprcResendButton->bottom = lprcQiPao->bottom - pBox->m_ptResendButtonMargin.y;
			lprcResendButton->top = lprcResendButton->bottom - pBox->m_sizeResendButton.cy;
			lprcResendButton->left = lprcQiPao->right + pBox->m_ptResendButtonMargin.x;
			lprcResendButton->right = lprcResendButton->left + pBox->m_sizeResendButton.cx;
		}
	}
	else
	{
		// TODO: 不使用气泡模式时，也要计算个坐标。
		_ASSERT(FALSE);
	}

	return 0;
}

int OnIMInsertMsg(IMBox *pBox, int nIndex, const CIMMsg *pMsg)
{
	if (NULL == pBox || NULL == pMsg)
	{
		return -1;
	}

	if (nIndex < -1)
	{
		return -2;
	}

	CIMMsg msg;
	msg = *pMsg;

	int nInsertIndex = -1;
	// 保存起来。因为插入是硬拷贝，所以返回插入后的指针进行操作。
	if (-1 == nIndex)
	{
		pBox->m_lstMsg.push_back(msg);
		pMsg = &(pBox->m_lstMsg.back());
		nInsertIndex = (int)pBox->m_lstMsg.size() - 1;
	}
	else
	{
		if (nIndex > (int)pBox->m_lstMsg.size())
		{
			nIndex = (int)pBox->m_lstMsg.size();
		}

		std::list<CIMMsg>::iterator itInsertPos = pBox->m_lstMsg.begin();
		std::advance(itInsertPos, nIndex);
		std::list<CIMMsg>::iterator itInsert =  pBox->m_lstMsg.insert(itInsertPos, msg);
		pMsg = &(*itInsert);
		nInsertIndex = std::distance(pBox->m_lstMsg.begin(), itInsert);
	}

	// 记录滚动位置。 当前是否滚动到末尾
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLBARINFO);
	si.fMask = SIF_ALL;
	BOOL bRet = GetScrollInfo(pBox->m_pREBox->hwnd, SB_VERT, &si);
	BOOL bScrollToBottom = FALSE;
	if (bRet)
	{
		if (si.nPos >= si.nMax - (int)si.nPage)
		{
			bScrollToBottom = TRUE;
		}
	}

	DoInsertMsg(pBox, pMsg);

	// 如果插入的时候，内容滚动到最下面，则插入后仍然在最下面。
	// 如果插入的时候，内容在中间，插入后不滚动，否则影响用户正在浏览的内容。
	if (bScrollToBottom)
	{
		SendMessage(pBox->m_pREBox->hwnd, WM_VSCROLL, MAKEWPARAM(SB_BOTTOM, 0), 0);
	}

	// 返回索引
	return nInsertIndex;
}

int OnIMDeleteMsg(IMBox *pBox, int nIndex)
{
	if (NULL == pBox)
	{
		return -1;
	}

	if (nIndex < 0 || nIndex >= (int)pBox->m_lstMsg.size())
	{
		return -2;
	}

	// 从RichEdit中删除
	DoDeleteMsg(pBox, nIndex);

	// 从数据结构中删除
	std::list<CIMMsg>::iterator itMsgItem = pBox->m_lstMsg.begin();
	std::advance(itMsgItem, nIndex);
	pBox->m_lstMsg.erase(itMsgItem);

	return 0;
}

class COleResendButtonHandler : public CWLWndHandler
{
public:
	COleResendButtonHandler()
		: m_pBindMsg(NULL)
	{}
	virtual ~COleResendButtonHandler() {}

protected:
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
	{
		if (pe->hOriginalSender == hWndThis)
		{
			if (RS_BUBBLE == pe->eRoutingStrategy)
			{
				if (UE_BUTTON_CLICK == pe->RoutedEvent)
				{
					RoutedEventArgs args;
					args.RoutedEvent = UE_IM_RESEND_MSG;
					args.eRoutingStrategy = RS_BUBBLE;
					args.hSender = args.hOriginalSender = hWndThis;
					args.wParam = (m_pBindMsg == NULL) ? NULL : m_pBindMsg->GetMsgID();
					args.lParam = (LPARAM)m_pBindMsg;
					RaiseEvent(hWndThis, &args);
					if (args.bHandled)
					{
						pe->bHandled = TRUE;
						return 0;
					}
				}
				else if (WM_NCDESTROY == pe->RoutedEvent)
				{
					delete this;
					return 0;
				}
			}
		}

		return 0;
	}

public:
	CIMMsg *m_pBindMsg;
};

// IMM_SHOW_RESEND_BUTTON handler
int OnShowResendButton(IMBox *pBox, UINT uMsgID, BOOL bShow)
{
	if (pBox == NULL)
	{
		return -1;
	}

	int nRet = -2;

	// 查找到指定消息
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		CIMMsg *pMsg = &(*it);
		if (pMsg->GetMsgID() == uMsgID)
		{
			nRet = 0;

			pMsg->ShowResendButton(bShow);

			if (pMsg->IsShowResendButton())
			{
				if (NULL == pMsg->GetResendButton())
				{
					// 创建Resend按钮控件
					COleResendButtonHandler *pHandler = new COleResendButtonHandler;
					HWLWND hBtnResend = IUI::CreateWindowEx(pHandler,
							0, IUI_BUTTON, NULL, WS_VISIBLE,
							0, 0, 0, 0,
							GetHostWnd(pBox->m_pREBox->hwnd), pBox->m_pREBox->hwnd,
							_T("IDC_BTN_RESEND"), 0, NULL);
					if (NULL == hBtnResend)
					{
						_ASSERT(FALSE);
						return -3;
					}

					pHandler->m_pBindMsg = pMsg;
					PushButton_BindStyle(hBtnResend, pBox->m_pResendButtoStyle);
					SendMessage(hBtnResend, WLM_SETHIDEOLEBORDER, 0, TRUE);

					pMsg->SetResendButton(hBtnResend);
				}
			}
			else
			{
				if (NULL != pMsg->GetResendButton())
				{
					DestroyWindow(pMsg->GetResendButton());
					pMsg->SetResendButton(NULL);
				}
			}
		}
	}

	IUI::Invalidate(pBox->m_pREBox->hwnd);

	return nRet;
}

BOOL OnIsMsgShowResendButton(IMBox *pBox, UINT uMsgID)
{
	if (pBox == NULL)
	{
		return -1;
	}

	BOOL bShowRet = FALSE;

	// 查找到指定消息
	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	for (; it != pBox->m_lstMsg.end(); ++it)
	{
		CIMMsg *pMsg = &(*it);
		if (pMsg->GetMsgID() == uMsgID)
		{
			// 只有自己发送的消息，才可能需要“重新发送”按钮
			if (CLH_RIGHT == pMsg->GetHorzLayoutMode())
			{
				bShowRet = pMsg->IsShowResendButton();
			}
		}
	}

	return bShowRet;
}

// "显示更多消息"按钮响应类
// 不对应界面编辑器中的窗口
class COleLoadMoreMsgHandler : public CWLWndHandler
{
public:
	COleLoadMoreMsgHandler()
	{}
	virtual ~COleLoadMoreMsgHandler()
	{}

protected:
	virtual LRESULT EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
	{
		if (pe->hOriginalSender == hWndThis)
		{
			if (UE_BUTTON_CLICK == pe->RoutedEvent)
			{
				RoutedEventArgs args;
				args.RoutedEvent = UE_IM_SHOW_MORE_MSG;
				args.eRoutingStrategy = RS_BUBBLE;
				args.hSender = args.hOriginalSender = hWndThis;
				RaiseEvent(hWndThis, &args);
			}
			else if (WM_NCDESTROY == pe->RoutedEvent)
			{
				delete this;
				return 0;
			}
		}
		return CWLWndHandler::EventHandler(hWndThis, pe);
	}
};

int OnInsertShowMoreMsg(IMBox *pBox, UINT uMsgID, LPCTSTR lpszText)
{
	if (!pBox->m_lstMsg.empty())
	{
		if (MSGTYPE_SHOW_MORE_MSG_BUTTON == pBox->m_lstMsg.begin()->GetMsgType())
		{
			// 已经有了，不需要再插入
			return 1;
		}
	}

	CIMMsg msg;
	msg.SetMsgType(MSGTYPE_SHOW_MORE_MSG_BUTTON);
	msg.SetHorzLayoutMode(CLH_CENTER);
	msg.SetMsgID(uMsgID);

	{
		// 创建OLE绑定的控件
		COleLoadMoreMsgHandler *pOleLoadMoreMsgHandler = new COleLoadMoreMsgHandler;
		HWLWND hBtnLoadMoreMsg = IUI::CreateWindowEx(pOleLoadMoreMsgHandler, 0,
				IUI_BUTTON, lpszText, WS_VISIBLE,
				0, 0, 0, 0,
				GetHostWnd(pBox->m_pREBox->hwnd), pBox->m_pREBox->hwnd,
				_T("IDC_BTN_LOAD_MORE_MSG"), 0, NULL);
		if (NULL == hBtnLoadMoreMsg)
		{
			return -3;
		}

		PushButton_BindStyle(hBtnLoadMoreMsg, pBox->m_pShowMoreMsgStyle);

		CRichMsgItem item;
		item.m_bOleControl = TRUE;
		item.m_hOleBindWindowless = hBtnLoadMoreMsg;
		item.m_size = CIUISize(200, 30);

		msg.AddMsgItem(&item);
	}

	if (!msg.GetMsgItems()->empty())
	{
		IUI::SendMessage(pBox->m_pREBox->hwnd, IMM_INSERT_MSG, 0, (LPARAM)&msg);
	}

	return 0;
}

int OnInsertLabel(IMBox *pBox, const CLabelMsg *pLabelMsg)
{
	if (NULL == pBox || NULL == pLabelMsg)
	{
		return -1;
	}

	CIMMsg msg;
	msg.SetMsgType(MSGTYPE_LABEL);

	if (NULL != pLabelMsg->m_pBindStyle)
	{
		msg.SetHorzLayoutMode(pLabelMsg->m_pBindStyle->m_eHorLayoutMode);
	}
	msg.SetMsgID(pLabelMsg->m_uMsgID);

	{
		// 创建OLE绑定的控件
		HWLWND hStaLabel = IUI::CreateWindowEx(NULL, 0,
				IUI_LABEL, pLabelMsg->m_lpszText, WS_VISIBLE,
				0, 0, 0, 0,
				GetHostWnd(pBox->m_pREBox->hwnd), pBox->m_pREBox->hwnd,
				_T("IDC_STA_LABEL"), 0, NULL);
		if (NULL == hStaLabel)
		{
			return -3;
		}

		BindStyle(hStaLabel, pLabelMsg->m_pBindStyle);
		SendMessage(hStaLabel, WLM_SETHIDEOLEBORDER, 0, TRUE);

		CRichMsgItem item;
		item.m_bOleControl = TRUE;
		item.m_hOleBindWindowless = hStaLabel;
		item.m_size = pLabelMsg->m_size;

		msg.AddMsgItem(&item);
	}

	if (!msg.GetMsgItems()->empty())
	{
		return IUI::SendMessage(pBox->m_pREBox->hwnd, IMM_INSERT_MSG, pLabelMsg->m_nIndex, (LPARAM)&msg);
	}

	return -1;
}

// 群组头像的每部分
enum GROUP_FACE_PART
{
	GFP_FULL = 0,		// 绘制完整头像
	GFP_LEFT = 1,		// 群组里只有两个人时，表示左半部分
	GFP_TOP_LEFT,		// 群组里有三个或四个人时，表示左上部分
	GFP_TOP_RIGHT,		// 群组里有三个或四个人时，表示右上部分
	GFP_RIGHT,			// 群组里只有两个人时，表示右半部分
	GFP_LEFT_BOTTOM,	// 群组里有四个人时，表示左下部分
	GFP_RIGHT_BOTTOM,	// 群组里有四个人时，表示右下部分
	GFP_BOTTOM			// 群组里有三个人时，表示下半部分
};
int _DrawFaceToPart(HDC hDC, LPCRECT lprcTarget, HIUIIMAGE himgFace, GROUP_FACE_PART ePart)
{
	CIUIRect rc = lprcTarget;

	CIUIRect rcPart = rc;
	rcPart.OffsetRect(-rcPart.TopLeft());
	CIUIRect rcSrc;
	rcSrc.right = himgFace->GetWidth();
	rcSrc.bottom = himgFace->GetHeight();

	switch (ePart)
	{
	case GFP_FULL:
		break;
	case GFP_LEFT:
		rcPart.right = rcPart.left + rc.Width() / 2;

		rcSrc.left = himgFace->GetWidth() / 4;
		rcSrc.right = rcSrc.left + himgFace->GetWidth() / 2;
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

	return 0;
}

int _DrawFace(
	HDC hDC,
	LPCRECT lprcTarget,
	HIUIIMAGE himgFace)
{
	CIUIRect rc = lprcTarget;

	// 把头像画到一个临时DC中
	HDC hdcMem = CreateCompatibleDC(hDC);
	HBITMAP hbmpMem = CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpMem);

	// 拷贝底色
	BitBlt(hdcMem, 0, 0, rc.Width(), rc.Height(), hDC, 0, 0, SRCCOPY);

	_DrawFaceToPart(hdcMem, lprcTarget, himgFace, GFP_FULL);
	SelectObject(hdcMem, hbmpOld);

	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		Gdiplus::GraphicsPath gp;
		// 这个操作执行速度很快，放到for循环中执行1000次，与放到for循环外面执行一次，总时间差不多。
		gp.AddEllipse(rc.left, rc.top, rc.Width() - 1, rc.Height() - 1);
		Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromHBITMAP(hbmpMem, NULL);
		Gdiplus::TextureBrush *ptBrush = ::new Gdiplus::TextureBrush(pImage);
		if (NULL != ptBrush)
		{
			// 注意，这里必须是FillPath，才可以对目标区域进行边缘抗锯齿。如果是FillRegion，
			// 即使调用了g.SetSmoothingMode(SmoothingModeAntiAlias)，边缘也是马赛克。
			// 这或许是因为Region只是内部区域，而Path，才是边界。
			ptBrush->TranslateTransform(Gdiplus::REAL(rc.left), Gdiplus::REAL(rc.top));
			g.FillPath(ptBrush, &gp);
		}
		if (NULL != ptBrush)
		{
			::delete ptBrush;
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

// 画头像, lprcFace是相对于RichEdit控件的，如果头像画到父窗口上，要进行坐标转换
int OnDrawFace(HWLWND hImCtrl, IMBox *pBox, HDC hMemDCParent, int nMsgIndex, LPCRECT lprcFace)
{
	if (lprcFace == NULL)
	{
		return -1;
	}

	int nMsgCount = (int)pBox->m_lstMsg.size();
	if (nMsgCount <= 0 || nMsgIndex >= nMsgCount)
	{
		return -2;
	}

	IMRichTextBox *pIMCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hImCtrl);

	CIUIRect rcNew = *lprcFace;

	// 计算本消息在消息组中的位置（上中下，用DT_TOP，DT_VCENTER和DT_BOTTOM表示）
	UINT uMsgPos = pIMCtrl->GetMsgPosInGroup(nMsgIndex);

	if (uMsgPos == DT_TOP)
	{
		std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
		std::advance(it, nMsgIndex);
		const CIMMsg *pMsg = &(*it);
		rcNew.OffsetRect(0, 5);

		HIUIIMAGE hImage = pMsg->GetFace();
		if (hImage->GetSafeHBITMAP() != NULL)
		{
			// 给父窗口一个自绘头像的机会
			DRAWITEMSTRUCT dis;
			dis.CtlType = ODT_IMFACE;
			dis.itemID = nMsgIndex;
			dis.itemAction = pIMCtrl->IsCombineMsg();
			dis.itemState = uMsgPos;
			dis.hwndItem = (HWND)hImCtrl;
			dis.hDC = hMemDCParent;
			dis.rcItem = rcNew;
			dis.itemData = (ULONG_PTR)pMsg;

			RoutedEventArgs arg;
			arg.hSender = hImCtrl;
			arg.hOriginalSender = hImCtrl;
			arg.RoutedEvent = WM_DRAWITEM;
			arg.eRoutingStrategy = RS_BUBBLE;
			arg.lParam = (LPARAM)&dis;
			LRESULT lr = RaiseEvent(hImCtrl, &arg);
			if (0 == lr)
			{
				_DrawFace(hMemDCParent, rcNew, hImage);
			}
		}
	}

	return 0;
}

// 画气泡, lprcQiPao是相对于RichEdit控件的，如果气泡画到父窗口上，要进行坐标转换
int OnDrawQiPao(HWLWND hImCtrl, IMBox *pBox, HDC hMemDCParent, int nMsgIndex, LPCRECT lprcQiPao)
{
	if (lprcQiPao == NULL)
	{
		return -1;
	}

	int nMsgCount = (int)pBox->m_lstMsg.size();
	if (nMsgCount <= 0 || nMsgIndex >= nMsgCount)
	{
		return -2;
	}

	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	std::advance(it, nMsgIndex);
	const CIMMsg *pMsg = &(*it);

	if (pMsg->GetMsgType() == MSGTYPE_SHOW_MORE_MSG_BUTTON
		|| pMsg->GetMsgType() == MSGTYPE_LABEL)
	{
		return 1;
	}

	IMRichTextBox *pIMCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hImCtrl);

	CIUIRect rcNew = *lprcQiPao;

	// 计算本消息在消息组中的位置（上中下，用DT_TOP，DT_VCENTER和DT_BOTTOM表示）
	UINT uMsgPos = DT_TOP;
	if (pIMCtrl->IsCombineMsg())
	{
		uMsgPos = pIMCtrl->GetMsgPosInGroup(nMsgIndex);
	}

	// 给父窗口一个自绘气泡的机会
	DRAWITEMSTRUCT dis;
	dis.CtlType = ODT_IMQIPAO;
	dis.itemID = nMsgIndex;
	dis.itemAction = pIMCtrl->IsCombineMsg();
	dis.itemState = uMsgPos;
	dis.hwndItem = (HWND)hImCtrl;
	dis.hDC = hMemDCParent;
	dis.rcItem = rcNew;
	dis.itemData = (ULONG_PTR)pMsg;

	RoutedEventArgs arg;
	arg.hSender = hImCtrl;
	arg.hOriginalSender = hImCtrl;
	arg.RoutedEvent = WM_DRAWITEM;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)&dis;
	RaiseEvent(hImCtrl, &arg);
	if (arg.bHandled)
	{
		return 0;
	}

	if (pIMCtrl->IsCombineMsg())
	{
		rcNew.InflateRect(5, 10, 5, 10);

		CIUIProject *pProject = GetProject(hImCtrl);

		HIUIIMAGE hImg = NULL;
		CIUIRect rc9GridArg;
		if (CLH_RIGHT == pMsg->GetHorzLayoutMode())
		{
			if (uMsgPos == DT_TOP)
			{
				hImg = pBox->m_himgSelfQiPao[0];
				rc9GridArg = pBox->m_rcSelfQiPao9GridArg[0];
			}
			else if (uMsgPos == DT_VCENTER)
			{
				hImg = pBox->m_himgSelfQiPao[1];
				rc9GridArg = pBox->m_rcSelfQiPao9GridArg[1];
			}
			else if (uMsgPos == DT_BOTTOM)
			{
				hImg = pBox->m_himgSelfQiPao[2];
				rc9GridArg = pBox->m_rcSelfQiPao9GridArg[2];
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		else
		{
			if (uMsgPos == DT_TOP)
			{
				hImg = pBox->m_himgSenderQiPao[0];
				rc9GridArg = pBox->m_rcSenderQiPao9GridArg[0];
			}
			else if (uMsgPos == DT_VCENTER)
			{
				hImg = pBox->m_himgSenderQiPao[1];
				rc9GridArg = pBox->m_rcSenderQiPao9GridArg[1];
			}
			else if (uMsgPos == DT_BOTTOM)
			{
				hImg = pBox->m_himgSenderQiPao[2];
				rc9GridArg = pBox->m_rcSenderQiPao9GridArg[2];
			}
			else
			{
				_ASSERT(FALSE);
			}
		}
		IUINineGridBlt(hMemDCParent, rcNew, hImg, rc9GridArg);
	}

	return 0;
}

// 画气泡, lprcResendButton是相对于RichEdit控件的，如果按钮画到父窗口上，要进行坐标转换
int OnDrawResendButton(
	HWLWND hImCtrl,
	IMBox *pBox,
	HDC hMemDCParent,
	int nMsgIndex,
	LPCRECT lprcResendButton)
{
	if (lprcResendButton == NULL)
	{
		return -1;
	}

	int nMsgCount = (int)pBox->m_lstMsg.size();
	if (nMsgCount <= 0 || nMsgIndex >= nMsgCount)
	{
		return -2;
	}

	IMRichTextBox *pIMCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hImCtrl);

	CIUIRect rcNew = *lprcResendButton;

	std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
	std::advance(it, nMsgIndex);
	const CIMMsg *pMsg = &(*it);

	// 计算本消息在消息组中的位置（上中下，用DT_TOP，DT_VCENTER和DT_BOTTOM表示）
	UINT uMsgPos = DT_TOP;
	if (pIMCtrl->IsCombineMsg())
	{
		uMsgPos = pIMCtrl->GetMsgPosInGroup(nMsgIndex);
	}

	// 给父窗口一个自绘“重新发送消息”按钮的机会
	DRAWITEMSTRUCT dis;
	dis.CtlType = ODT_IMRESENDBUTTON;
	dis.itemID = nMsgIndex;
	dis.itemAction = pIMCtrl->IsCombineMsg();
	dis.itemState = uMsgPos;
	dis.hwndItem = (HWND)hImCtrl;
	dis.hDC = hMemDCParent;
	dis.rcItem = rcNew;
	dis.itemData = (ULONG_PTR)pMsg;

	RoutedEventArgs arg;
	arg.hSender = hImCtrl;
	arg.hOriginalSender = hImCtrl;
	arg.RoutedEvent = WM_DRAWITEM;
	arg.eRoutingStrategy = RS_BUBBLE;
	arg.lParam = (LPARAM)&dis;
	RaiseEvent(hImCtrl, &arg);
	if (arg.bHandled)
	{
		return 0;
	}

	if (pIMCtrl->IsCombineMsg())
	{
		CIUIProject *pProject = GetProject(hImCtrl);

		IUI::ShowWindow(((CIMMsg *)pMsg)->GetResendButton(), SW_SHOW);

		// 这里千万不要传TRUE，因为OnDrawResendButton是由Host的WM_PAINT调用的
		// 如果传TRUE，会引起绘制死循环。
		IUI::MoveWindow(((CIMMsg *)pMsg)->GetResendButton(), rcNew, FALSE);
	}

	return 0;
}

// 绘制非Rich Text，包含头像、气泡和“重新发送”按钮等，只绘制可视频范围内的消息记录
// 在绘制前计算头像和气泡坐标，以提高性能和准确度。
// 如果在WM_VSCROLL时计算，会发现滚动过程中，气泡比内容滚动慢半拍（即使先调用基类的OnVScroll）。
int OnDrawNonRichText(HWLWND hImCtrl, IMBox *pBox, HDC hMemDCParent)
{
	IMRichTextBox *pImCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hImCtrl);

	// 如果第一可见行小于0
	int nFirstVisibleMsg = pImCtrl->GetFirstVisibleMsg();
	if (nFirstVisibleMsg < 0)
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(hImCtrl, rcClient);

	//
	// 绘制
	//
	int nMsgCount = (int)pBox->m_lstMsg.size();
	int i = nFirstVisibleMsg;
	for (; i < nMsgCount; ++i)
	{
		std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
		std::advance(it, i);
		const CIMMsg *pMsg = &(*it);

		// 下端超出显示范围后，停止绘制
		CIUIPoint pt = pImCtrl->PosFromChar(pMsg->GetMsgStart());
		if (pt.y >= rcClient.Height())
		{
			break;
		}

		// 得到头像坐标
		CIUIRect rcFace;
		DoCalFaceRect(pBox, pMsg, rcFace);

		// 绘制头像
		OnDrawFace(hImCtrl, pBox, hMemDCParent, i, rcFace);

		// 绘制气泡
		CIUIRect rcQiPao;
		if (pBox->m_bQiPaoMode)
		{
			DoCalQiPaoRect(pBox, pMsg, rcQiPao);

			OnDrawQiPao(hImCtrl, pBox, hMemDCParent, i, rcQiPao);
		}

		// 绘制“重新发送”按钮
		if (pMsg->IsShowResendButton())
		{
			CIUIRect rcResendButton;
			DoCalResendButtonRect(pBox, pMsg, rcQiPao, rcResendButton);
			OnDrawResendButton(hImCtrl, pBox, hMemDCParent, i, rcResendButton);
		}
	}

	int nLastVisible = i;

	// 把不可见范围内的重试按钮隐藏
	for (int i = 0; i < nFirstVisibleMsg; ++i)
	{
		std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
		std::advance(it, i);
		const CIMMsg *pMsg = &(*it);
		HWLWND hButton = ((CIMMsg *)pMsg)->GetResendButton();
		if (NULL != hButton)
		{
			ShowWindow(hButton, SW_HIDE);
		}
	}
	for (int i = nLastVisible; i < nMsgCount; ++i)
	{
		std::list<CIMMsg>::iterator it = pBox->m_lstMsg.begin();
		std::advance(it, i);
		const CIMMsg *pMsg = &(*it);
		HWLWND hButton = ((CIMMsg *)pMsg)->GetResendButton();
		if (NULL != hButton)
		{
			ShowWindow(hButton, SW_HIDE);
		}
	}

	return 0;
}

int IMRichTextBox_OnPaint(REBox *pREBox, IMBox *pIMBox, HDC hDC, const RECT &rcClip)
{
	pREBox->m_hDC = hDC;

	ITextServices *pTextServices = pREBox->m_pTextHost->GetTextServices();

	LRESULT lr = S_OK;
	if (NULL != pTextServices)
	{
		CIUIRect rc;
		GetClientRect(pREBox->hwnd, rc);

		if (!rc.IsRectEmpty())
		{
			int nWidth = rc.Width();
			int nHeight = rc.Height();

			int nStateIndex = 0;

			HDC hDrawDC = NULL;
			HBITMAP hTempBmp = NULL;
			HBITMAP hOldBmp = NULL;
			void *pBits = NULL;
			BOOL bUpdateLayeredWindow = IsUpdateLayeredWindow(GetHostWnd(pREBox->hwnd));
			//bUpdateLayeredWindow = FALSE;
			if (bUpdateLayeredWindow)
			{
				HDC hTempDC = CreateCompatibleDC(hDC);
				if (hTempDC == NULL)
				{
					_ASSERT(FALSE);
					return -1;
				}

				hTempBmp = Create32BitsHBITMAP(nWidth, nHeight, 32, BI_RGB, &pBits);
				//HBITMAP hTempBmp = CreateCompatibleBitmap(hDC, nWidth, nHeight);
				if (hTempBmp == NULL)
				{
					_ASSERT(FALSE);
					return -2;
				}

				hOldBmp = (HBITMAP)::SelectObject(hTempDC, hTempBmp);

				// 拷贝RichEdit覆盖区域
				BitBlt(hTempDC, 0, 0, nWidth, nHeight, hDC, 0, 0, SRCCOPY);

				// 把hTempBmp各像素的Alpha值设置为0xFF
				// 图像像素是从左到右、从下向上的。
				for (int y = 0; y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						byte *pByte = (byte *)(pBits) + (y * nWidth * 4 + x * 4);
						//pByte[0] = 0x00; // B
						//pByte[1] = 0x00; // G
						//pByte[2] = 0xFF; // R
						pByte[3] = 0xFF; // alpha
					}
				}

				hDrawDC = hTempDC;
			}
			else
			{
				hDrawDC = hDC;
			}

			BeforePaint(pREBox->hwnd, hDrawDC);
			RichTextBox_DrawBkColor(pREBox, hDrawDC, rcClip);
			AfterPaintBkColor(pREBox->hwnd, hDrawDC);
			RichTextBox_DrawBkImage(pREBox, hDrawDC, rcClip);
			AfterPaintBkImage(pREBox->hwnd, hDrawDC);
			OnDrawNonRichText(pREBox->hwnd, pIMBox, hDrawDC);
			lr = RichTextBox_DrawText(pREBox, hDrawDC, rcClip);

			if (bUpdateLayeredWindow)
			{
				// 把hTempBmp中alpha为0的像素的Alpha值设置为0xFF
				for (int y = 0; SUCCEEDED(lr) && y < nHeight; y++)
				{
					for (int x = 0; x < nWidth; x++)
					{
						byte *pByte = (byte *)(pBits) + (y * nWidth * 4 + x * 4);
						if (pByte[3] == 0x00)
						{
							pByte[3] = 0xFF; // Alpha
						}
					}
				}

				// 把修复好后的位图，贴到目标DC上。
				if (SUCCEEDED(lr))
				{
					BitBlt(hDC, 0, 0, nWidth, nHeight, hDrawDC, 0, 0, SRCCOPY);
				}

				// 释放
				::SelectObject(hDrawDC, hOldBmp);
				hOldBmp = NULL;
				::DeleteObject(hTempBmp);
				hTempBmp = NULL;
				DeleteDC(hDrawDC);
				hDrawDC = NULL;
			}
		}
	}

	AfterPaint(pREBox->hwnd, hDC);

	return SUCCEEDED(lr) ? 0 : -1;
}

HRESULT RangeToDataObject(
	HWLWND hRe,
	LONG lStreamFormat,
	IDataObject **ppdo)
{
	if (ppdo)
	{
		CProtectedDataObject *pdo = CProtectedDataObject::Create(hRe, lStreamFormat);
		*ppdo = pdo;
		return NOERROR;
	}

	return E_INVALIDARG;
}

HRESULT StartDrag(HWLWND hIM)
{
	IMBox *pIMBox = (IMBox *)GetProp(hIM, GetProp_IMDATA);
	if (NULL == pIMBox)
	{
		_ASSERT(FALSE);
		return E_FAIL;
	}

	IMRichTextBox *pIMRe = (IMRichTextBox *)CWLWnd::FromHandle(pIMBox->m_pREBox->hwnd);
	if (NULL == pIMRe)
	{
		_ASSERT(FALSE);
		return E_FAIL;
	}

	DWORD			dwEffect = 0;
	HRESULT			hr = S_OK;
	IDataObject 	*pdo = NULL;
	IDropSource 	*pds = NULL;

	do
	{
		if (pIMBox->m_pRichEditOleCallback)
		{
			CHARRANGE chrg;

			// give the callback a chance to give us its own IDataObject
			pIMRe->GetSel(chrg.cpMin, *(&chrg.cpMin + 1));
			hr = pIMBox->m_pRichEditOleCallback->GetClipboardData(&chrg, RECO_COPY, &pdo);
		}
		else
		{
			// we need to build our own data object.
			hr = S_FALSE;
		}

		// If we didn't get an IDataObject from the callback, build our own
		if (hr != NOERROR || pdo == NULL)
		{
			// Don't include trailing EOP
			//prg->AdjustEndEOP(NONEWCHARS);		//  in some selection cases
			// 这里只记录选中的范围，不准备数据，等释放鼠标时，拖放目标通过IDataObject.GetData来请求数据
			hr = RangeToDataObject(hIM, SF_TEXT | SF_RTF, &pdo);
			if (hr != NOERROR)
			{
				break;
			}
		}

		pds = new CProtectedDropSource();
		if (pds == NULL)
		{
			pds->Release();
			hr = E_OUTOFMEMORY;
			break;
		}

		// Set allowable effects
		dwEffect = DROPEFFECT_COPY;
		LONG lStyle = GetStyle(hIM);
		if (!(lStyle & ES_READONLY))
		{
			dwEffect |= DROPEFFECT_MOVE;
		}

		// Let the client decide what it wants.
		if (pIMBox->m_pRichEditOleCallback)
		{
			hr = pIMBox->m_pRichEditOleCallback->GetDragDropEffect(TRUE, 0, &dwEffect);
		}

		if (!FAILED(hr) || hr == E_NOTIMPL)
		{
			// start drag-drop operation
			hr = ::DoDragDrop(pdo, pds, dwEffect, &dwEffect);
		}

		if (SUCCEEDED(hr))
		{
			hr = NOERROR;
		}
	}
	while (false);

	if (NULL != pdo)
	{
		pdo->Release();
	}
	if (NULL != pds)
	{
		pds->Release();
	}

	//if ((_ped->GetEventMask() & ENM_DRAGDROPDONE))
	//{
	//	NMHDR	hdr;

	//	ZeroMemory(&hdr, sizeof(NMHDR));
	//	_ped->TxNotify(EN_DRAGDROPDONE, &hdr);
	//}

	return hr;
}

// 因为RichEdit的Drop动作在DoDragDrop返回后，
// 如果Drop目标把Drop effect设置成DROPEFFECT_MOVE后，RichEdit会把选中的内容删除（即使ReadOnly）
// 这对于制作IM控件，是不允许的。所以拦截IM控件的鼠标消息，自己发出Drag动作。
BOOL WantDrag(HWLWND hIM, RoutedEventArgs *pe)
{
	if (pe->RoutedEvent == WM_LBUTTONDOWN)
	{
		IMRichTextBox *pIMRe = (IMRichTextBox *)CWLWnd::FromHandle(hIM);
		if (NULL == pIMRe)
		{
			_ASSERT(FALSE);
			return FALSE;
		}

		// TODO: 本来CharFromPos传的是相对于RichEdit的坐标，但IUI实现的有问题
		// 暂时先用相对于Host坐标
		CIUIPoint pt(pe->lParam);
		IUI::ClientToScreen(hIM, &pt);
		ScreenToClient(GetHostWnd(hIM), &pt);
		int nCp = pIMRe->CharFromPos(pt);

		LONG lCpMin = -1;
		LONG lCpMost = -1;
		pIMRe->GetSel(lCpMin, lCpMost);

		if (lCpMost - lCpMin > 0 && nCp >= lCpMin && nCp <= lCpMost)
		{
			BOOL bRet = CheckForDragBegin(hIM, LOWORD(pe->lParam), HIWORD(pe->lParam));
			if (bRet)
			{
				StartDrag(hIM);
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 释放Ole对象
void RelaseObject(IMBox *pIMBox)
{
	RichTextBox *pRichTextBox = (RichTextBox *)CWLWnd::FromHandle(pIMBox->m_pREBox->hwnd);
	CComPtr<IRichEditOle> pRichEditOle = pRichTextBox->GetIRichEditOle();
	_ASSERT(NULL != (IRichEditOle *)pRichEditOle);

	LONG lOleCount = pRichEditOle->GetObjectCount();
	for (LONG i = 0; i < lOleCount; ++i)
	{
		REOBJECT reObj;
		reObj.cbStruct = sizeof(REOBJECT);
		pRichEditOle->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);

		// OLE的位置
		int cp = reObj.cp;

		reObj.poleobj->Release();

		// 如果本OLE是文件OLE、视频OLE等，作为一条单独消息，否则合并到文本消息
		OleControl *pOleControl = NULL;
		reObj.poleobj->QueryInterface(IID_IUIOleControl, (void **)&pOleControl);
		if (NULL != pOleControl)
		{
			delete pOleControl;
			pOleControl = NULL;
		}
		else
		{
			// 输入框中的OLE，应该都是我们插入的。
			_ASSERT(FALSE);
		}
	}
}

// 通过Subclass替换的RichEdit的窗口过程，所以，IMRichTextBox_EventHandler
// 处理不了WM_NCCREATE和WM_CREATE消息。
LRESULT WINAPI IUI::IMRichTextBox_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	fnWLEventHandler fnOldHandler = (fnWLEventHandler)GetProp(hWnd, _T("GetProp_Old_Handler"));

	IMBox *pIMBox = (IMBox *)GetProp(hWnd, GetProp_IMDATA);
	if (pIMBox == NULL)
	{
		pIMBox = new IMBox;
		pIMBox->m_pREBox = (REBox *)GetMember(hWnd);
		SetProp(hWnd, GetProp_IMDATA, pIMBox);
	}

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (pe->RoutedEvent == WM_DESTROY)
			{
				// 释放OLE
				RelaseObject(pIMBox);

				LRESULT lr = CallEventHandler(fnOldHandler, hWnd, pe);

				// RichEdit已经把REBox销毁了，故置NULL。
				pIMBox->m_pREBox = NULL;

				return lr;
			}
			else if (pe->RoutedEvent == WM_NCDESTROY)
			{
				pIMBox->Release();
				delete pIMBox;
				pIMBox = NULL;
				SetProp(hWnd, GetProp_IMDATA, NULL);
				return 0;
			}
			else if (pe->RoutedEvent == WM_WINDOWPOSCHANGED)
			{
				// 设置段落格式
				IMRichTextBox *pImCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hWnd);

				// TODO: 先在内存DC中画一遍，否则使用EM_POSFROMCHAR得到cp坐标有可能是错误的
				// 之后需研究更好的解决方案，目前先用绘制到内存DC一次解决。
				CIUIRect rcClient;
				IUI::GetClientRect(hWnd, rcClient);

				if (rcClient != pIMBox->m_pREBox->m_rcOldClient)
				{
					pIMBox->m_pREBox->m_rcOldClient = rcClient;

					REBox *pREBox = (REBox *)GetMember(hWnd);

					HDC hMemDC = CreateCompatibleDC(HDC(wParam));

					// 把Font拿到, for BoundsChecker
					HFONT hFontOld = (HFONT)::GetCurrentObject(hMemDC, OBJ_FONT);

					HBITMAP hMemBmp = CreateCompatibleBitmap(HDC(wParam),
							rcClient.Width(), rcClient.Height());
					HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);
					IMRichTextBox_OnPaint(pREBox, pIMBox, HDC(hMemDC), *(LPCRECT)lParam);
					SelectObject(hMemDC, hOldBmp);
					DeleteObject(hMemBmp);
					hMemBmp = NULL;

					// 把Font选回
					if (hFontOld != NULL)
					{
						::SelectObject(hMemDC, hFontOld);
					}
					DeleteDC(hMemDC);
					hMemDC = NULL;
				}

				DoSetParaFormat(pIMBox);
			}

			// 由于把RichEdit设置成Readonly后，会使Host禁止接收输入法IME
			// 所以不采用设置Readonly的方式，而是改用禁止响应键盘消息的某些消息
			// 但允许方向键等。另外还要禁止拖放
			BOOL bKeyboardMsg = (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST);
			if (bKeyboardMsg)
			{
				// 允许方向键、翻页键
				if (wParam >= VK_PRIOR && wParam <= VK_DOWN)
				{
					// 允许方向键和Home, end, page up, page down键。
					return CallEventHandler(fnOldHandler, hWnd, pe);
				}

				// 允许Ctrl + A全选，Ctrl+C, Ctrl+X复制
				if ('A' == wParam || 'C' == wParam || 'X' == wParam)
				{
					if ('X' == wParam)
					{
						wParam = 'C';
						pe->wParam = wParam;
					}

					if (GetKeyState(VK_CONTROL) < 0)
					{
						return CallEventHandler(fnOldHandler, hWnd, pe);
					}
				}
			}

			BOOL bIMEMsg = ((uMsg >= WM_IME_STARTCOMPOSITION && uMsg <= WM_IME_KEYLAST)
					|| (uMsg >= WM_IME_SETCONTEXT && uMsg <= WM_IME_CHAR)
					|| WM_IME_REQUEST == uMsg
					|| WM_IME_KEYDOWN == uMsg
					|| WM_IME_KEYUP == uMsg);
			if (bKeyboardMsg || bIMEMsg)
			{
				pe->bHandled = TRUE;
				return 1;
			}
		}

		if (RS_DIRECT == pe->eRoutingStrategy)
		{
			switch (uMsg)
			{
			case UE_SUBCLASSED:
			{
				DCT = GetDoubleClickTime();
				cxDoubleClk = GetSystemMetrics(SM_CXDOUBLECLK);
				cyDoubleClk = GetSystemMetrics(SM_CYDOUBLECLK);

				IMRichTextBox *pImCtrl = (IMRichTextBox *)CWLWnd::FromHandle(hWnd);
				_ASSERT(NULL != pImCtrl);
				pImCtrl->SetDefaultProcHandleIfHitOleCtrl(FALSE);
			}
			break;

			case REM_ISIMCTRL:
				return TRUE;

			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
			{
				REBox *pREBox = (REBox *)GetMember(hWnd);
				IMRichTextBox_OnPaint(pREBox, pIMBox, HDC(wParam), *(LPCRECT)lParam);
			}
			return 0;

			case IMM_INSERT_MSG:
				return OnIMInsertMsg(pIMBox, wParam, (const CIMMsg *)lParam);

			case IMM_DELETE_MSG:
				return OnIMDeleteMsg(pIMBox, wParam);

			case IMM_SHOW_RESEND_BUTTON:
				return OnShowResendButton(pIMBox, UINT(wParam), lParam);

			case IMM_IS_RESEND_BUTTON_VISIBLE:
				return OnIsMsgShowResendButton(pIMBox, wParam);

			case IMM_INSERT_SHOW_MORE_MSG:
				return OnInsertShowMoreMsg(pIMBox, wParam, LPCTSTR(lParam));

			case IMM_INSERT_LABEL:
				return OnInsertLabel(pIMBox, (CLabelMsg *)lParam);

			case IMM_SET_QIPAO:
			{
				BOOL bSelf = LOWORD(wParam);
				UINT uDirection = HIWORD(wParam);

				HIUIIMAGE *phImg = NULL;
				if (bSelf)
				{
					if (DT_TOP == uDirection)
					{
						phImg = &pIMBox->m_himgSelfQiPao[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						phImg = &pIMBox->m_himgSelfQiPao[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						phImg = &pIMBox->m_himgSelfQiPao[2];
					}
				}
				else
				{
					if (DT_TOP == uDirection)
					{
						phImg = &pIMBox->m_himgSenderQiPao[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						phImg = &pIMBox->m_himgSenderQiPao[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						phImg = &pIMBox->m_himgSenderQiPao[2];
					}
				}
				IUISetControlImage(GetProject(pe->hOriginalSender),	phImg, LPCTSTR(lParam));
			}
			return 0;

			case IMM_SET_QIPAO_9GRIDARG:
			{
				BOOL bSelf = LOWORD(wParam);
				UINT uDirection = HIWORD(wParam);

				LPRECT lpRect = (LPRECT)lParam;
				if (lpRect == 0)
				{
					return -1;
				}

				if (bSelf)
				{
					if (DT_TOP == uDirection)
					{
						pIMBox->m_rcSelfQiPao9GridArg[0] = lpRect;
					}
					else if (DT_VCENTER == uDirection)
					{
						pIMBox->m_rcSelfQiPao9GridArg[1] = lpRect;
					}
					else if (DT_BOTTOM == uDirection)
					{
						pIMBox->m_rcSelfQiPao9GridArg[2] = lpRect;
					}
				}
				else
				{
					if (DT_TOP == uDirection)
					{
						pIMBox->m_rcSenderQiPao9GridArg[0] = lpRect;
					}
					else if (DT_VCENTER == uDirection)
					{
						pIMBox->m_rcSenderQiPao9GridArg[1] = lpRect;
					}
					else if (DT_BOTTOM == uDirection)
					{
						pIMBox->m_rcSenderQiPao9GridArg[2] = lpRect;
					}
				}
			}
			return 0;

			case IMM_GET_QIPAO:
			{
				BOOL bSelf = LOWORD(wParam);
				UINT uDirection = HIWORD(wParam);

				HIUIIMAGE hImg = NULL;
				if (bSelf)
				{
					if (DT_TOP == uDirection)
					{
						hImg = pIMBox->m_himgSelfQiPao[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						hImg = pIMBox->m_himgSelfQiPao[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						hImg = pIMBox->m_himgSelfQiPao[2];
					}
				}
				else
				{
					if (DT_TOP == uDirection)
					{
						hImg = pIMBox->m_himgSenderQiPao[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						hImg = pIMBox->m_himgSenderQiPao[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						hImg = pIMBox->m_himgSenderQiPao[2];
					}
				}
				return (LRESULT)(LPCTSTR)hImg->GetSafeImageName();
			}
			return 0;

			case IMM_GET_QIPAO_9GRIDARG:
			{
				BOOL bSelf = LOWORD(wParam);
				UINT uDirection = HIWORD(wParam);

				LPRECT lpRect = (LPRECT)lParam;
				if (lpRect == 0)
				{
					return -1;
				}

				if (bSelf)
				{
					if (DT_TOP == uDirection)
					{
						*lpRect = pIMBox->m_rcSelfQiPao9GridArg[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						*lpRect = pIMBox->m_rcSelfQiPao9GridArg[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						*lpRect = pIMBox->m_rcSelfQiPao9GridArg[2];
					}
				}
				else
				{
					if (DT_TOP == uDirection)
					{
						*lpRect = pIMBox->m_rcSenderQiPao9GridArg[0];
					}
					else if (DT_VCENTER == uDirection)
					{
						*lpRect = pIMBox->m_rcSenderQiPao9GridArg[1];
					}
					else if (DT_BOTTOM == uDirection)
					{
						*lpRect = pIMBox->m_rcSenderQiPao9GridArg[2];
					}
				}
			}
			return 0;

			case IMM_SET_RESENDBUTTON_MARGIN:
				pIMBox->m_ptResendButtonMargin.x = wParam;
				pIMBox->m_ptResendButtonMargin.y = lParam;
				return 0;

			case IMM_GET_RESENDBUTTON_MARGIN:
				if (0 != wParam)
				{
					*((LONG *)wParam) = pIMBox->m_ptResendButtonMargin.x;
				}
				if (0 != lParam)
				{
					*((LONG *)lParam) = pIMBox->m_ptResendButtonMargin.y;
				}
				return 0;

			case IMM_SET_RESENDBUTTON_SIZE:
				pIMBox->m_sizeResendButton.cx = wParam;
				pIMBox->m_sizeResendButton.cy = lParam;
				return 0;

			case IMM_GET_RESENDBUTTON_SIZE:
				if (0 != wParam)
				{
					*((LONG *)wParam) = pIMBox->m_sizeResendButton.cx;
				}
				if (0 != lParam)
				{
					*((LONG *)lParam) = pIMBox->m_sizeResendButton.cy;
				}
				return 0;

			case IMM_SET_RESENDBUTTON_STYLE:
				pIMBox->m_pResendButtoStyle = ((CButtonProp *)lParam);
				return 0;

			case IMM_GET_RESENDBUTTON_STYLE:
				return (LRESULT)pIMBox->m_pResendButtoStyle;

			case IMM_SET_SHOWMOREMSG_BUTTON_STYLE:
				pIMBox->m_pShowMoreMsgStyle = ((CButtonProp *)lParam);
				return 0;

			case IMM_GET_SHOWMOREMSG_BUTTON_STYLE:
				return (LRESULT)pIMBox->m_pShowMoreMsgStyle;

			// 本地保存一下
			case EM_SETOLECALLBACK:
				pIMBox->m_pRichEditOleCallback = (IRichEditOleCallback *)lParam;
				break;

			default:
				break;
			}
		}
		else if (RS_TUNNEL == pe->eRoutingStrategy)
		{
			BOOL bWantDrag = WantDrag(hWnd, pe);
			if (bWantDrag)
			{
				pe->bHandled = TRUE;
				return 0;
			}
		}
	}

	return CallEventHandler(fnOldHandler, hWnd, pe);
}

#include "stdafx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


int Button_DrawBkColor(_BTN *pBtn, HDC hDC, LPCRECT lpRect)
{
	_ASSERT(IsWindow(pBtn->hwnd));

	if (!IsDrawBkColor(pBtn->hwnd))
	{
		return 1;
	}

	// Prepare state index
	int nCheck = pBtn->m_nCheck;
	CONTROL_STATE eButtonState = GetControlState(pBtn->hwnd);
	if (eButtonState < CONTROL_STATE_NORMAL || eButtonState > CONTROL_STATE_DISABLED)
	{
		_ASSERT(FALSE);
		return -1;
	}
	int nStateIndex = 0;
	if (IsWindowEnabled(pBtn->hwnd))
	{
		nStateIndex = GetButtonStateIndex(eButtonState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);

	if (nCheck == BST_CHECKED)
	{
		FillSolidRect(hDC, rcClient, pBtn->m_crBkC[nStateIndex],
			IsUpdateLayeredWindow(GetHostWnd(pBtn->hwnd)));
	}
	else
	{
		FillSolidRect(hDC, rcClient, pBtn->m_crBk[nStateIndex],
			IsUpdateLayeredWindow(GetHostWnd(pBtn->hwnd)));
	}

	return 0;
}

int Button_DrawBkImage(_BTN *pBtn, HDC hDC, LPCRECT lprcDest)
{
	_ASSERT(IsWindow(pBtn->hwnd));

	BOOL bCheckBox = IsIncludeFlag(GetStyle(pBtn->hwnd), BS_AUTOCHECKBOX);
	BOOL bRadioButton = IsIncludeFlag(GetStyle(pBtn->hwnd), BS_AUTORADIOBUTTON);
	BOOL bToggleButton = bCheckBox | bRadioButton;

	// Prepare state index
	int nCheck = pBtn->m_nCheck;
	int nStateIndex = 0;
	if (IsWindowEnabled(pBtn->hwnd))
	{
		CONTROL_STATE eButtonState = GetControlState(pBtn->hwnd);
		nStateIndex = GetButtonStateIndex(eButtonState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);

	if (IsCombineBkImage(pBtn->hwnd))
	{
		int nImagePart = nStateIndex;

		if (nCheck == BST_CHECKED)
		{
			nImagePart += COMBINEIMAGESIZE4;
		}

		IUIPartDrawImage(hDC, rcClient,
			pBtn->hwnd->ci.m_himgCombineBk, pBtn->hwnd->ci.m_eBkImageResizeMode,
			pBtn->hwnd->ci.m_rcBkImage9GridResizeArg,
			bToggleButton ? COMBINEIMAGESIZE8 : COMBINEIMAGESIZE4, nImagePart);
	}
	else
	{
		HIUIIMAGE hImgBk = NULL;
		if (nCheck == BST_CHECKED)
		{
			hImgBk = pBtn->m_himgBkC[nStateIndex];
		}
		else
		{
			hImgBk = pBtn->m_himgBk[nStateIndex];
		}

		IUIDrawImage(hDC, rcClient,
			hImgBk, pBtn->hwnd->ci.m_eBkImageResizeMode, pBtn->hwnd->ci.m_rcBkImage9GridResizeArg);
	}

	return 0;
}

int Button_DrawFgImage(_BTN *pBtn, HDC hDC)
{
	_ASSERT(IsWindow(pBtn->hwnd));

	BOOL bCheckBox = IsIncludeFlag(GetStyle(pBtn->hwnd), BS_AUTOCHECKBOX);
	BOOL bRadioButton = IsIncludeFlag(GetStyle(pBtn->hwnd), BS_AUTORADIOBUTTON);
	BOOL bToggleButton = bCheckBox | bRadioButton;

	// Prepare state index
	CONTROL_STATE eButtonState = GetControlState(pBtn->hwnd);
	int nStateIndex = GetButtonStateIndex(eButtonState);

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);

	CIUIRect rcFgImageMargin = pBtn->m_rcForegroundImageMargin;

	FOREGROUND_ALIGN_HOR eFgAlignHor = pBtn->m_eForegroundImageAlignHor;
	FOREGROUND_ALIGN_VER eFgAlignVer = pBtn->m_eForegroundImageAlignVer;

	if (pBtn->m_bCombineFgImage)
	{
		if (pBtn->m_himgCombineFg->GetSafeHBITMAP() == NULL)
		{
			pBtn->m_himgCombineFg->SafeLoadSavedImage();
		}

		if (pBtn->m_himgCombineFg->GetSafeHBITMAP() != NULL)
		{
			int nPartWidth = 0;
			nPartWidth = pBtn->m_himgCombineFg->GetWidth() / (bToggleButton ? COMBINEIMAGESIZE8 : COMBINEIMAGESIZE4);

			CIUIRect rcFgPart;

			if (pBtn->m_nCheck == BST_CHECKED)
			{
				rcFgPart.left = (nStateIndex + COMBINEIMAGESIZE4) * nPartWidth;
			}
			else
			{
				rcFgPart.left = nStateIndex * nPartWidth;
			}

			rcFgPart.right = rcFgPart.left + nPartWidth;
			rcFgPart.top = 0;
			rcFgPart.bottom = pBtn->m_himgCombineFg->GetHeight();

			IUIPartDrawForeground(hDC, rcClient, rcFgImageMargin,
				pBtn->m_himgCombineFg, rcFgPart, eFgAlignHor, eFgAlignVer, -1, 255);
		}
	}
	else
	{
		HIUIIMAGE hImgFg = (pBtn->m_nCheck == BST_CHECKED)
			? pBtn->m_himgFgC[nStateIndex] : pBtn->m_himgFg[nStateIndex];

		IUIDrawForeground(hDC,
			rcClient,
			pBtn->m_rcForegroundImageMargin,
			hImgFg,
			pBtn->m_eForegroundImageAlignHor,
			pBtn->m_eForegroundImageAlignVer,
			RGB(255, 0, 255), 255);
	}

	return 0;
}

int Button_DrawText(_BTN *pBtn, HDC hDC)
{
	CIUIString strText = GetWindowText(pBtn->hwnd);
	if (strText.IsEmpty())
	{
		return 1;
	}

	// Prepare state index
	int nStateIndex = 0;
	if (IsWindowEnabled(pBtn->hwnd))
	{
		CONTROL_STATE eControlState = GetControlState(pBtn->hwnd);
		nStateIndex = GetButtonStateIndex(eControlState);
	}
	else
	{
		nStateIndex = 3;
	}

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);

	BOOL bChecked = (pBtn->m_nCheck == BST_CHECKED);
	HFONT hFont = bChecked ? pBtn->m_hIUIFontC[nStateIndex]->GetSafeHFont()
		: pBtn->m_hIUIFont[nStateIndex]->GetSafeHFont();

	// 文本水平方向对齐
	if (pBtn->m_eTextHorAlignMode[nStateIndex] == TAH_LEFT)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_CENTER;
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_RIGHT;
	}
	else if (pBtn->m_eTextHorAlignMode[nStateIndex] == TAH_CENTER)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_RIGHT;
		pBtn->hwnd->ci.m_uTextStyle |= DT_CENTER;
	}
	else if (pBtn->m_eTextHorAlignMode[nStateIndex] == TAH_RIGHT)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_CENTER;
		pBtn->hwnd->ci.m_uTextStyle |= DT_RIGHT;
	}

	// 文本垂直方向对齐
	if (pBtn->m_eTextVerAlignMode[nStateIndex] == TAV_TOP)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_VCENTER;
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_BOTTOM;
	}
	else if (pBtn->m_eTextVerAlignMode[nStateIndex] == TAV_CENTER)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_BOTTOM;
		pBtn->hwnd->ci.m_uTextStyle |= DT_VCENTER;
	}
	else if (pBtn->m_eTextVerAlignMode[nStateIndex] == TAV_BOTTOM)
	{
		pBtn->hwnd->ci.m_uTextStyle &= ~DT_VCENTER;
		pBtn->hwnd->ci.m_uTextStyle |= DT_BOTTOM;
	}

	DrawFormatText(hDC, strText, strText.GetLength(),
		rcClient, pBtn->m_rcTextMargin[nStateIndex],
		pBtn->m_eTextHorAlignMode[nStateIndex], pBtn->m_eTextVerAlignMode[nStateIndex],
		pBtn->hwnd->ci.m_uTextStyle, hFont, 
		bChecked ? pBtn->m_crTextC[nStateIndex] : pBtn->m_crText[nStateIndex],
		IsUpdateLayeredWindow(GetHostWnd(pBtn->hwnd)));

	return 0;
}

int Button_DrawBorder(_BTN *pBtn, HDC hDC)
{
	_ASSERT(IsWindow(pBtn->hwnd));

	if (!IsDrawBorderColor(pBtn->hwnd))
	{
		return 1;
	}

	// Prepare state index
	int nCheck = pBtn->m_nCheck;
	CONTROL_STATE eButtonState = GetControlState(pBtn->hwnd);
	if (!IsWindowEnabled(pBtn->hwnd))
	{
		eButtonState = CONTROL_STATE_DISABLED;
	}

	if (eButtonState < CONTROL_STATE_NORMAL || eButtonState > CONTROL_STATE_DISABLED)
	{
		_ASSERT(FALSE);
		return -1;
	}

	int nStateIndex = GetButtonStateIndex(eButtonState);

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);

	COLORREF crBorder = 0;
	if (nCheck == BST_CHECKED)
	{
		crBorder = pBtn->m_crBorderC[nStateIndex];
	}
	else
	{
		crBorder = pBtn->m_crBorder[nStateIndex];
	}

	HBRUSH hbrBorder = CreateSolidBrush(crBorder);
	FrameRect(hDC, rcClient, hbrBorder);
	DeleteObject(hbrBorder);

	return 0;
}

int Button_OnPaint(_BTN *pBtn, HDC hDC, LPCRECT lprcPaint)
{
	if (!IsRedraw(pBtn->hwnd))
	{
		return 1;
	}

	CIUIRect rcClient;
	GetClientRect(pBtn->hwnd, rcClient);
	CIUIRect rcPaint;
	if (!::IntersectRect(&rcPaint, lprcPaint, rcClient))
	{
		return 2;
	}

	HDC hDCTarget = NULL;
	HDC hMemDC = NULL;
	HBITMAP hOldBmp = NULL;

	// 由于GDI不支持浮点数坐标，所以当使用RectF坐标时，采用GDI+绘制
	// RectF坐标常用于制作平滑移动的动画。 这时，先把控件内容画到一个32位alpha位图上
	// 最后再把位图绘制到指定坐标。
	HBITMAP hBmp = GetCachedAlphaBitmap(pBtn->hwnd);
	if (IsUseRectF(pBtn->hwnd))
	{
		hMemDC = ::CreateCompatibleDC(hDC);
		hDCTarget = hMemDC;

		void *pBits = NULL;
		if (hBmp != NULL)
		{
			DeleteObject(hBmp);
			hBmp = NULL;
		}

		hBmp = Create32BitsHBITMAP(rcClient.Width(), rcClient.Height(),
				32, BI_RGB, &pBits);
		SetCachedAlphaBitmap(pBtn->hwnd, hBmp);
		hOldBmp = (HBITMAP)::SelectObject(hDCTarget, hBmp);
	}
	else
	{
		hDCTarget = hDC;
	}

	//
	// 1. 先绘制自己
	//
	// TODO: BeforPaint和AfterPaint应该由框架调用
	BeforePaint(pBtn->hwnd, hDCTarget);
	Button_DrawBkColor(pBtn, hDCTarget, lprcPaint);
	Button_DrawBkImage(pBtn, hDCTarget, NULL);
	Button_DrawFgImage(pBtn, hDCTarget);
	//OnDrawCustom(hDCTarget);
	if (!IsUseRectF(pBtn->hwnd))
	{
		// 由于使用RectF绘制时，内容是画到Alpha HBITMAP上，但文字画到alpha HBITMAP上后
		// 对于雅黑这种本身支持alpha的字体，显示会有问题。
		// 所以文字做成与alpha HBITMAP并列关系。
		Button_DrawText(pBtn, hDCTarget);
	}
	Button_DrawBorder(pBtn, hDCTarget);
	AfterPaint(pBtn->hwnd, hDCTarget);

	if (IsUseRectF(pBtn->hwnd))
	{
		if (hMemDC != NULL && hOldBmp != NULL)
		{
			// 生成m_hCacheAlphaBmp
			::SelectObject(hMemDC, hOldBmp);
			DeleteDC(hMemDC);

			// 因为GDI+绘制时用的坐标与GDI时不同，所以重设视口
			POINT pt;
			SetViewportOrgEx(hDC, 0, 0, &pt);

			Gdiplus::Graphics g(hDC);

			// 把HBITMAP转成Bitmap，Bitmap::FromHBITMAP有Bug
			Gdiplus::Bitmap *pBmp = CreateBitmapFromHBITMAP(hBmp);
			g.DrawImage(pBmp, *GetRectF(pBtn->hwnd));
			::delete pBmp;
			pBmp = NULL;

			// 最后绘制文本。
			Button_DrawText(pBtn, hDC);

			// 重设视口
			SetViewportOrgEx(hDC, pt.x, pt.y, NULL);
		}
	}
	else
	{
		if (IsCachedMode(pBtn->hwnd) && hMemDC != NULL && hOldBmp != NULL)
		{
			BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(),
				hDCTarget, 0, 0, SRCCOPY);
			::SelectObject(hMemDC, hOldBmp);
			DeleteDC(hMemDC);
		}
	}

	return 0;
}

// 收集本组所有Radio Button
int GetThisGroupRadioButton(HWLWND hRadChecked, std::vector<HWLWND> *pvRadios)
{
	if (hRadChecked == NULL || pvRadios == NULL)
	{
		return -1;
	}

	pvRadios->clear();

	pvRadios->push_back(hRadChecked);

	//
	// 1. 向前查询
	//

	// 先确认自己是不是Group风格
	BOOL bGroup = IsIncludeFlag(GetStyle(hRadChecked), WS_GROUP);
	if (!bGroup)
	{
		// 如果自己不是Group风格，向前查询，直到找到第一个Group风格的Radio Button为止。
		// 如果自己之前没有Radio Button，则自己当成同组第一个Radio Button
		// 如果自己之前有Radio Button，但是没有任何一个有Group风格，则第一个当成同组第一个Radio.

		HWLWND hPrevWnd = GetWindow(hRadChecked, GW_HWNDPREV);
		while (hPrevWnd != NULL)
		{
			if (GetClassName(hPrevWnd) == IUI_BUTTON
				&& IsIncludeFlag(GetStyle(hPrevWnd), BS_AUTORADIOBUTTON))
			{
				pvRadios->insert(pvRadios->begin(), hPrevWnd);

				if (IsIncludeFlag(GetStyle(hPrevWnd), WS_GROUP))
				{
					break;
				}
			}

			hPrevWnd = GetWindow(hPrevWnd, GW_HWNDPREV);
		}
	}

	//
	// 2. 向后查找
	//
	HWLWND hNextWnd = GetWindow(hRadChecked, GW_HWNDNEXT);
	while (hNextWnd != NULL)
	{
		if (GetClassName(hNextWnd) == IUI_BUTTON
			&& IsIncludeFlag(GetStyle(hNextWnd), BS_AUTORADIOBUTTON))
		{
			if (IsIncludeFlag(GetStyle(hNextWnd), WS_GROUP))
			{
				break;
			}

			pvRadios->push_back(hNextWnd);
		}

		hNextWnd = GetWindow(hNextWnd, GW_HWNDNEXT);
	}

	return 0;
}

// 仅选中hRadChecked控件，并且把同组RadioButton反选
int OnlyCheck(HWLWND hRadChecked)
{
	std::vector<HWLWND> vRadios;
	GetThisGroupRadioButton(hRadChecked, &vRadios);

	std::vector<HWLWND>::iterator it = vRadios.begin();
	for (; it != vRadios.end(); ++it)
	{
		HWLWND hRad = *it;
		if (hRad == hRadChecked)
		{
			SendMessage(hRad, BM_SETCHECK, BST_CHECKED, 0);
		}
		else
		{
			SendMessage(hRad, BM_SETCHECK, BST_UNCHECKED, 0);
		}
	}

	return 0;
}

LRESULT WINAPI IUI::Button_WndProc(HWLWND hWnd, RoutedEventArgs *pe)
{
	_BTN *pBtn = (_BTN *)GetMember(hWnd);

	UINT uMsg = pe->RoutedEvent;
	WPARAM wParam = pe->wParam;
	LPARAM lParam = pe->lParam;

	if (pe->hOriginalSender == hWnd)
	{
		if (pe->eRoutingStrategy == RS_BUBBLE)
		{
			if (pBtn == NULL)
			{
				if (uMsg == WM_NCCREATE)
				{
					CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;

					pBtn = new _BTN;
					pBtn->hwnd = hWnd;
					pBtn->hwnd->ci.hwndParent = GetParent(hWnd);

					SetMember(hWnd, pBtn);
				}

				return 0;
			}

			switch (uMsg)
			{
			case WM_DESTROY:
				pBtn->Release();
				delete pBtn;
				SetMember(hWnd, NULL);
				return 0;

			case UE_BUTTON_CLICK:
				if (IsIncludeFlag(GetStyle(hWnd), BS_AUTOCHECKBOX))
				{
					if (pBtn->m_nCheck == BST_UNCHECKED)
					{
						SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0);
					}
					else if (pBtn->m_nCheck == BST_CHECKED)
					{
						SendMessage(hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
					}

					return 0;
				}
				else if (IsIncludeFlag(GetStyle(hWnd), BS_AUTORADIOBUTTON))
				{
					// 如果未Check，则Check，同时，把同组其它Radio Uncheck.
					// 对于Win32 RadioButton, 如果同组有1，2，3 三个Radio.
					// 通过代码
					// pRad2->SetCheck(BST_CHECKED);
					// pRad3->SetCheck(BST_CHECKED);
					// 可以让Radio 2和3同时选中。 只有通过鼠标操作时，才可保证单选。
					OnlyCheck(hWnd);

					return 0;
				}
				return 0;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_TUNNEL)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
			{
				// Win32的按钮，当在按钮内按下鼠标左键，并在按钮外释放鼠标，也会收到
				// WM_LBUTTONUP，这是由于win32内部，在处理WM_LBUTTONDOWN时，调用了
				// SetCapture()，同时还设置了焦点

				SetCapture(hWnd);
				SetFocus(GetHostWnd(hWnd), hWnd);
				pBtn->m_bRecordLButtonDown = TRUE;

				SetControlState(hWnd, CONTROL_STATE_PRESSED);
				InvalidateRect(hWnd, NULL);
			}
			break;

			case WM_LBUTTONUP:
			{
				ReleaseCapture(hWnd);

				InvalidateRect(hWnd, NULL);

				BOOL bOld = pBtn->m_bRecordLButtonDown;
				pBtn->m_bRecordLButtonDown = FALSE;

				// 转成按钮点击事件
				if (bOld)
				{
					// 相对于自己
					CIUIPoint pt(pe->lParam);

					CIUIRect rcWnd;
					GetClientRect(hWnd, rcWnd);

					if (rcWnd.PtInRect(pt))
					{
						RoutedEventArgs arg;
						arg.hSender = hWnd;
						arg.hOriginalSender = hWnd;
						arg.RoutedEvent = UE_BUTTON_CLICK;
						arg.eRoutingStrategy = RS_BUBBLE;
						// IUI中的按钮点击时，可以附带一个自定义字符串参数
						arg.wParam = (LPARAM)(LPCTSTR)pBtn->m_strButtonClickArg;
						RaiseEvent(hWnd, &arg);

						// 如果按钮是Close Button，执行完RaiseEvent后，
						// this就被释放了。
						pe->bHandled = TRUE;
						return 1;
					}
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				if (pBtn->m_bRecordLButtonDown)
				{
					CIUIRect rcWnd;
					GetWindowRectToHWND(hWnd, rcWnd);

					// 相对于宿主窗口的鼠标坐标
					CIUIPoint point((DWORD)pe->lParam);
					if (!rcWnd.PtInRect(point))
					{
						if (GetControlState(hWnd) != CONTROL_STATE_HOVER)
						{
							SetControlState(hWnd, CONTROL_STATE_HOVER);
							InvalidateRect(hWnd, NULL);
						}
					}
					else
					{
						if (GetControlState(hWnd) != CONTROL_STATE_PRESSED)
						{
							SetControlState(hWnd, CONTROL_STATE_PRESSED);
							InvalidateRect(hWnd, NULL);
						}
					}
				}
				else
				{
					if (GetControlState(hWnd) != CONTROL_STATE_HOVER)
					{
						SetControlState(hWnd, CONTROL_STATE_HOVER);

						// when set RGN for highlight stats, show the tool tip when
						// mouse over the button(the tool tip and highlight
						// stats at the same time work).
						// don't call InvalidateRect(hWnd, NULL) after this "if" section
						InvalidateRect(hWnd, NULL);
					}
				}
			}
			break;

			case WM_MOUSELEAVE:
			case WM_NCMOUSELEAVE:
			{
				SetControlState(hWnd, CONTROL_STATE_NORMAL);
				InvalidateRect(hWnd, NULL);
			}
			return 0;

			case WM_KILLFOCUS:
			{
				pBtn->m_bRecordLButtonDown = FALSE;

				CIUIRect rcWnd;
				GetWindowRectToHWND(hWnd, rcWnd);

				// 相对于宿主窗口的鼠标坐标
				CIUIPoint point;
				GetCursorPos(&point);
				::ScreenToClient(GetHostWnd(hWnd), &point);

				if (!rcWnd.PtInRect(point))
				{
					SetControlState(hWnd, CONTROL_STATE_NORMAL);
				}
				else
				{
					SetControlState(hWnd, CONTROL_STATE_HOVER);
				}

				InvalidateRect(hWnd, NULL);
			}
			return 0;

			default:
				break;
			}
		}
		else if (pe->eRoutingStrategy == RS_DIRECT)
		{
			switch (uMsg)
			{
			case WM_PRINTCLIENT:
			case WM_PRINT:
			case WM_PAINT:
				Button_OnPaint(pBtn, (HDC)wParam, LPCRECT(lParam));
				return 0;

			case WLM_BIND_STYLE:
			{
				DWORD dwStyle = GetStyle(hWnd);
				if (IsIncludeFlag(dwStyle, BS_AUTOCHECKBOX)
					|| IsIncludeFlag(dwStyle, BS_AUTORADIOBUTTON))
				{
					ToggleButton_BindStyle(hWnd, (CToggleButtonProp *)lParam);
				}
				else
				{
					PushButton_BindStyle(hWnd, (CButtonProp *)lParam);
				}
			}
			return 0;

			case BM_SET_BUTTON_TYPE:
				pBtn->m_nButtonType = wParam;
				return 0;

			case BM_GET_BUTTON_TYPE:
				return pBtn->m_nButtonType;

			case WLM_SETTOOLTIPS:
				_ASSERT(FALSE);
				break;

			//TODO: case WLM_GETTOOLTIPS:
			//	return (LRESULT)pBtn->m_wndToolTip.GetSafeHwnd();

			//case BM_SETTOOLTIPS:
			//{
			//	if (pBtn->m_wndToolTip.GetSafeHwnd() == NULL)
			//	{
			//		pBtn->m_wndToolTip.Create(GetHostWnd(hWnd), GetProject(hWnd), 0);
			//	}

			//	CIUIRect rcButton;
			//	GetWindowRectToHWND(pBtn->hwnd, rcButton);

			//	const CToolTipCtrlProp *pToolTipCtrlProp = (const CToolTipCtrlProp *)wParam;
			//	if (pToolTipCtrlProp != NULL)
			//	{
			//		pBtn->m_wndToolTip.AddTool(GetHostWnd(hWnd), pToolTipCtrlProp, (LPCTSTR)lParam, rcButton, 1);
			//	}
			//	else
			//	{
			//		pBtn->m_wndToolTip.AddTool(GetHostWnd(hWnd), (LPCTSTR)lParam, rcButton, 1);
			//	}
			//}
			//return 0;

			case WLM_SET_BK_COLOR:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eButtonState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eButtonState >= CONTROL_STATE_NORMAL && eButtonState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eButtonState);

					if (bChecked)
					{
						pBtn->m_crBkC[nStateIndex] = cr;
						return 0;
					}
					else
					{
						pBtn->m_crBk[nStateIndex] = cr;
						return 0;
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_BK_COLOR:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eButtonState = (CONTROL_STATE)HIWORD(wParam);

				if (eButtonState >= CONTROL_STATE_NORMAL && eButtonState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eButtonState);

					if (bChecked)
					{
						return pBtn->m_crBkC[nStateIndex];
					}
					else
					{
						return pBtn->m_crBk[nStateIndex];
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_COMBINE_FG_IMAGE:
			{
				BOOL bOld = pBtn->m_bCombineFgImage;
				pBtn->m_bCombineFgImage = lParam;
				return bOld;
			}

			case WLM_IS_COMBINE_FG_IMAGE:
				return pBtn->m_bCombineFgImage;

			case WLM_SET_BK_IMAGE:
			case WLM_SET_FG_IMAGE:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				LPCTSTR lpszImage = LPCTSTR(lParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bChecked)
					{
						return IUISetControlImage(GetProject(hWnd),
								(uMsg == WLM_SET_BK_IMAGE) ? &pBtn->m_himgBkC[nStateIndex] : &pBtn->m_himgFgC[nStateIndex],
								lpszImage);
					}
					else
					{
						return IUISetControlImage(GetProject(hWnd),
								(uMsg == WLM_SET_BK_IMAGE) ? &pBtn->m_himgBk[nStateIndex] : &pBtn->m_himgFg[nStateIndex],
								lpszImage);
					}
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					return IUISetControlImage(GetProject(hWnd),
							(uMsg == WLM_SET_BK_IMAGE) ? &pBtn->hwnd->ci.m_himgCombineBk : &pBtn->m_himgCombineFg,
							lpszImage);
				}
			}
			return 0;

			case WLM_GET_BK_IMAGE:
			case WLM_GET_FG_IMAGE:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL
					&& eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bChecked)
					{
						if (uMsg == WLM_GET_BK_IMAGE)
						{
							return (LRESULT)(LPCTSTR)pBtn->m_himgBkC[nStateIndex]->GetSafeImageName();
						}
						else
						{
							return (LRESULT)(LPCTSTR)pBtn->m_himgFgC[nStateIndex]->GetSafeImageName();
						}
					}
					else
					{
						if (uMsg == WLM_GET_BK_IMAGE)
						{
							return (LRESULT)(LPCTSTR)pBtn->m_himgBk[nStateIndex]->GetSafeImageName();
						}
						else
						{
							return (LRESULT)(LPCTSTR)pBtn->m_himgFg[nStateIndex]->GetSafeImageName();
						}
					}
				}
				else if (eControlState == CONTROL_STATE_COMBINE)
				{
					if (uMsg == WLM_GET_BK_IMAGE)
					{
						return (LRESULT)(LPCTSTR)pBtn->hwnd->ci.m_himgCombineBk->GetSafeImageName();
					}
					else
					{
						return (LRESULT)(LPCTSTR)pBtn->m_himgCombineFg->GetSafeImageName();
					}
				}
			}
			return 0;

			case WLM_SET_BORDER_COLOR:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eButtonState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eButtonState >= CONTROL_STATE_NORMAL && eButtonState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eButtonState);

					if (bChecked)
					{
						pBtn->m_crBorderC[nStateIndex] = cr;
						return 0;
					}
					else
					{
						pBtn->m_crBorder[nStateIndex] = cr;
						return 0;
					}
				}
			}
			return 0;

			case WLM_GET_BORDER_COLOR:
			{
				BOOL bChecked = (LOWORD(wParam) == BST_CHECKED);
				CONTROL_STATE eButtonState = (CONTROL_STATE)HIWORD(wParam);

				if (eButtonState >= CONTROL_STATE_NORMAL && eButtonState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eButtonState);

					if (bChecked)
					{
						return pBtn->m_crBorderC[nStateIndex];
					}
					else
					{
						return pBtn->m_crBorder[nStateIndex];
					}
				}
			}
			return 0;

			case WLM_SET_TEXT_COLOR:
			{
				BOOL bChecked = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				COLORREF cr = lParam;

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bChecked)
					{
						pBtn->m_crTextC[nStateIndex] = cr;
					}
					else
					{
						pBtn->m_crText[nStateIndex] = cr;
					}
					return 0;
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_GET_TEXT_COLOR:
			{
				BOOL bChecked = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState >= CONTROL_STATE_NORMAL && eControlState <= CONTROL_STATE_DISABLED)
				{
					int nStateIndex = GetButtonStateIndex(eControlState);

					if (bChecked)
					{
						return pBtn->m_crTextC[nStateIndex];
					}
					else
					{
						return pBtn->m_crText[nStateIndex];
					}
				}
				else
				{
					_ASSERT(FALSE);
				}
			}
			return 0;

			case WLM_SET_TEXT_FONT:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				return IUISetControlFont4(GetProject(hWnd), eControlState, pBtn->m_hIUIFont,
						(LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam, (LPCTSTR)lParam);
			}

			case WLM_GET_TEXT_FONT:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);

				if (eControlState == CONTROL_STATE_NORMAL)
				{
					return pBtn->m_hIUIFont[0] == NULL ? NULL : (LRESULT)(LPCTSTR)pBtn->m_hIUIFont[0]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_HOVER)
				{
					return pBtn->m_hIUIFont[1] == NULL ? NULL : (LRESULT)(LPCTSTR)pBtn->m_hIUIFont[1]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_PRESSED)
				{
					return pBtn->m_hIUIFont[2] == NULL ? NULL : (LRESULT)(LPCTSTR)pBtn->m_hIUIFont[2]->m_strFontResID;
				}
				else if (eControlState == CONTROL_STATE_DISABLED)
				{
					return pBtn->m_hIUIFont[3] == NULL ? NULL : (LRESULT)(LPCTSTR)pBtn->m_hIUIFont[3]->m_strFontResID;
				}
			}
			return 0;

			case WLM_SET_TEXT_ALIGN_MODE:
			{
				BOOL bVert = LOWORD(wParam);
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				if (bVert)
				{
					TEXT_ALIGN_VER eHorAlignMode = (TEXT_ALIGN_VER)lParam;
					pBtn->m_eTextVerAlignMode[nIndex] = eHorAlignMode;
				}
				else
				{
					TEXT_ALIGN_HOR eHorAlignMode = (TEXT_ALIGN_HOR)lParam;
					pBtn->m_eTextHorAlignMode[nIndex] = eHorAlignMode;
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
					return pBtn->m_eTextVerAlignMode[nIndex];
				}
				else
				{
					return pBtn->m_eTextHorAlignMode[nIndex];
				}
			}
			return 0;

			case WLM_SET_TEXT_MARGIN:
			{
				CONTROL_STATE eControlState = (CONTROL_STATE)HIWORD(wParam);
				int nIndex = GetButtonStateIndex(eControlState);
				_ASSERT(nIndex >= 0 && nIndex < COMBINEIMAGESIZE4);

				LPCRECT lprcTextMargin = (LPCRECT)lParam;
				pBtn->m_rcTextMargin[nIndex] = *lprcTextMargin;
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

				*lprcTextMargin = pBtn->m_rcTextMargin[nIndex];
			}
			return 0;

			case BM_SET_BUTTON_CLICK_ARG:
				pBtn->m_strButtonClickArg = (LPCTSTR)wParam;
				return 0;

			case BM_GET_BUTTON_CLICK_ARG:
				return (LRESULT)(LPCTSTR)pBtn->m_strButtonClickArg;

			case BM_SETCHECK:
			{
				int nOld = pBtn->m_nCheck;
				pBtn->m_nCheck = wParam;

				InvalidateRect(hWnd, NULL);

				return nOld;
			}

			default:
				break;
			}
		}
	}

	return DefEventHandler(hWnd, pe);
}

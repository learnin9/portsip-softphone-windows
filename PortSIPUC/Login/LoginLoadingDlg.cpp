#include "StdAfx.h"
#include "LoginLoadingDlg.h"



CLoginLoadingDlg::CLoginLoadingDlg()
{
	m_pCanvasLogging = NULL;
}


CLoginLoadingDlg::~CLoginLoadingDlg(void)
{
}

int CLoginLoadingDlg::InitControls()
{
	m_pCanvasLogging = (Canvas *)CWLWnd::FromHandle(FindControl(_T("IDC_CANVAS_LOGGING")));
	_ASSERT(m_pCanvasLogging != NULL);
	m_pBtnMin = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_MIN")));
	_ASSERT(m_pBtnMin != NULL);
	m_pChkMax = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_CHK_MAX")));
	_ASSERT(m_pChkMax != NULL);
	m_pBtnClose = (Button *)CWLWnd::FromHandle(FindControl(_T("IDC_BTN_CLOSE")));
	_ASSERT(m_pBtnClose != NULL);
	m_pCanvasLogging->ShowWindow(SW_SHOW);
	m_pBtnMin->ShowWindow(SW_HIDE);
	m_pBtnClose->ShowWindow(SW_HIDE);
	m_pChkMax->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT CLoginLoadingDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	InitControls();

	CenterWindow();
	Invalidate();
	return 0;
}

LRESULT CLoginLoadingDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	 if (uMsg == WM_COMMAND)
		{
			if (HIBYTE(wParam) == CBN_SELCHANGE)
			{
			}
		}
	 if (uMsg ==WM_CREATE_INFO_WND)
	 {
		 InfoWndParam *pInfoWndParam = NULL;
		 pInfoWndParam = (InfoWndParam*)wParam;
		 if (pInfoWndParam==NULL)
		 {
			 PopupInfoWnd(L"Error", L"something wrong when login ", GetSafeHwnd(), INFO_WND, 5000);
		 }
		 else
		 {
			 PopupInfoWnd(pInfoWndParam->strTitle, pInfoWndParam->strMsg, GetSafeHwnd(), INFO_WND, pInfoWndParam->nAutoTimer);
			 delete pInfoWndParam;
			 pInfoWndParam = NULL;
		 }
		
	 }
	 
	return Window::WindowProc(uMsg, wParam, lParam);
}
void CLoginLoadingDlg::ShowLoginWaiting()
{
	m_pCanvasLogging->ShowWindow(SW_SHOW);
}
void CLoginLoadingDlg::EndLoginWaiting()
{
	if(m_pCanvasLogging)
	m_pCanvasLogging->ShowWindow(SW_HIDE);
	
}
void CLoginLoadingDlg::ProcessLoginReslut(bool bLoginSuccess, std::string strErrorInfo)
{
	if (bLoginSuccess)
	{
		EndLoginWaiting();
		EndDialog(IDOK);
	}
	else
	{
		EndLoginWaiting();
	}
	
}
LRESULT CLoginLoadingDlg::OnControlEvent(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = wParam;
	RoutedEventArgs *pe = (RoutedEventArgs *)lParam;

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == WM_NCCREATE)
		{
		}
		else if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
		}
		else if (pe->RoutedEvent == UE_AFTERPAINTBKIMAGE)
		{
		}
	}

	if (pe->eRoutingStrategy == RS_BUBBLE)
	{
		if (pe->RoutedEvent == UE_BUTTON_CLICK)
		{
		}
	}


	return Window::OnControlEvent(wParam, lParam);
}

LRESULT CLoginLoadingDlg::OnBtnClose(RoutedEventArgs *pe)
{
	EndDialog(IDCANCEL);
	return 0;
}
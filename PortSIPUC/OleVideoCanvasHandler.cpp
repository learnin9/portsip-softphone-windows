#include "stdafx.h"
#include "OleVideoCanvasHandler.h"
#include "System/SystemEx.h"
#include <cstdio>
#include<Windows.h>

#include<Gdiplus.h>
COleVideoCanvasHandler::COleVideoCanvasHandler()
	: m_pStaVideo(NULL)
	, m_pBtnPlay(NULL)
	, m_pPrgUpload(NULL)
	, m_pStaUploadFailed(NULL)
{
	this;
	m_strFilePath = L"";
	m_strTotalPlayTime = L"";
}

COleVideoCanvasHandler::~COleVideoCanvasHandler()
{
}
void COleVideoCanvasHandler::SetVideoParam(LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, LPCTSTR lpszFileName)
{
	m_strFilePath = lpszFilePath;
	CIUIString strTempTotalTime = lpszTotalPlayTime;
	m_strTotalPlayTime = L"00:00/" + strTempTotalTime;
	m_strFileName = lpszFileName;
}

unsigned int  COleVideoCanvasHandler::ResetVideoThumbnailImageThread(void* pParam)
{
	COleVideoCanvasHandler *pOleVideoCanvasHandler = (COleVideoCanvasHandler *)pParam;

	CIUIString strTemplateFilePath = pOleVideoCanvasHandler-> m_strFilePath + pOleVideoCanvasHandler->m_strFileName;
	HBITMAP hBmp;
	
	int nTryTimes = 3,nCurrentTimes=0;
	while (nCurrentTimes< nTryTimes)
	{
		PortUtility::GetShellThumbnailImage(strTemplateFilePath, &hBmp);
		Sleep(500);
		nCurrentTimes++;
		if(hBmp!=NULL)
			break;
	}
	if (hBmp == NULL)
	{
		hBmp = IUIImage::LoadHBITMAP(GetRunFolder() + _T("Skins\\PortGo\\VideoThumbnial.bmp"));
	}
	AddCustomImage(g_pProject, pOleVideoCanvasHandler->m_strFileName, hBmp);
	pOleVideoCanvasHandler->m_pStaVideo->SetBkImage(CONTROL_STATE_NORMAL, pOleVideoCanvasHandler->m_strFileName, NULL, FALSE);
	return 0;
}
void COleVideoCanvasHandler::ResetVideoThumbnailImage( )
{
	CBaseThread				oResetVideoThumbnailImageThread;
	oResetVideoThumbnailImageThread.BeginThread(ResetVideoThumbnailImageThread, this);
}
int COleVideoCanvasHandler::InitControls()
{
	m_pStaVideo = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_VIDEO")));
	_ASSERT(m_pStaVideo != NULL);
	m_pPrgUpload = (ProgressBar *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_PRG_UPLOAD")));
	_ASSERT(m_pPrgUpload != NULL);
	m_pStaUploadFailed = (Label *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_STA_UPLOAD_FAILED")));
	_ASSERT(m_pStaUploadFailed != NULL);
	m_pBtnPlay = (Button *)CWLWnd::FromHandle(FindControl(GetBindWLWnd(), _T("IDC_BTN_PLAY")));
	_ASSERT(m_pBtnPlay != NULL);
	return 0;
}

int COleVideoCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();
	HBITMAP hBmp;

	if (m_strFilePath.IsEmpty()|| m_strFileName.IsEmpty())
	{
		LOG4_INFO(L"path or file is error");
		hBmp = IUIImage::LoadHBITMAP(GetRunFolder() + _T("Skins\\PortGo\\VideoThumbnial.bmp"));
	}
	else
	{
		CIUIString strSrcFileName = m_strFilePath + m_strFileName;
		PortUtility::GetShellThumbnailImage(strSrcFileName, &hBmp);
	}
	
	if (hBmp == NULL)
	{
		hBmp = IUIImage::LoadHBITMAP(GetRunFolder() + _T("Skins\\PortGo\\VideoThumbnial.bmp"));
	}
	AddCustomImage(g_pProject, m_strFileName, hBmp);
	m_pStaVideo->SetBkImage(CONTROL_STATE_NORMAL, m_strFileName, NULL, FALSE);

	//delete hBmp;
	m_pBtnPlay->ShowWindow(SW_SHOW);
	m_pPrgUpload->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT COleVideoCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
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

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (UE_BUTTON_CLICK == pe->RoutedEvent)
		{
			return OnButtonClicked(pe);
		}
	}

	return 0;
}

LRESULT COleVideoCanvasHandler::OnBtnPlay(RoutedEventArgs *pe)
{
	HBITMAP hBmp;
	CIUIString strSrcFileName = m_strFilePath + m_strFileName;

	PortUtility::GetShellThumbnailImage(strSrcFileName, &hBmp);

	if (hBmp == NULL)
	{
		hBmp = IUIImage::LoadHBITMAP(GetRunFolder() + _T("Skins\\PortGo\\VideoThumbnial.bmp"));
	}
	PopupVideoWnd(hBmp,GetHostWnd(GetBindWLWnd()),m_strFilePath,m_strTotalPlayTime,m_strFileName);
	//delete hBmp;
	return 0;
}



LRESULT COleVideoCanvasHandler::OnButtonClicked(RoutedEventArgs *pe)
{
	if (m_pBtnPlay->GetSafeHwnd() == pe->hOriginalSender)
	{
		OnBtnPlay(pe);
	}
	
	return 0;
}

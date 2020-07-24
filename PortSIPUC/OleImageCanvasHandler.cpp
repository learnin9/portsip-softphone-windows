#include "stdafx.h"
#include "OleImageCanvasHandler.h"


COleImageCanvasHandler::COleImageCanvasHandler()
	: m_pStaImage(NULL)
	, m_pPrgDownload(NULL)
{
}

COleImageCanvasHandler::~COleImageCanvasHandler()
{
}

int COleImageCanvasHandler::InitControls()
{
	HWLWND hStaImage = GetBindWLWnd();
	m_pStaImage = (Label *)CWLWnd::FromHandle(hStaImage);

	return 0;
}

int COleImageCanvasHandler::OnInitPanel(RoutedEventArgs *pe)
{
	InitControls();

	return 0;
}

LRESULT COleImageCanvasHandler::EventHandler(HWLWND hWndThis, RoutedEventArgs *pe)
{
	_ASSERT(this->GetBindWLWnd() == hWndThis);

	if (hWndThis == pe->hOriginalSender)
	{
		if (RS_BUBBLE == pe->eRoutingStrategy)
		{
			if (WM_INIT == pe->RoutedEvent)
			{
				return OnInitPanel(pe);
			}
		}
	}

	if (RS_BUBBLE == pe->eRoutingStrategy)
	{
		if (WM_INIT == pe->RoutedEvent)
		{
			if (m_pStaImage->GetSafeHwnd() != pe->hOriginalSender)
			{
				if (GetName(pe->hOriginalSender) == _T("IDC_PRG_IMAGE_OLE"))
				{
					m_pPrgDownload = (ProgressBar *)CWLWnd::FromHandle(pe->hOriginalSender);
					_ASSERT(m_pPrgDownload != NULL);
				}
			}
		}
	}

	return 0;
}

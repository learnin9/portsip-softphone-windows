#include "stdafx.h"
#include "BitmapHelper.h"
#include "string.h"
#include "math.h"
BitmapHelper::BitmapHelper(HBITMAP hBitmap)
{
	m_hBitmap = hBitmap;
	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), (LPBYTE)&bmp);
	m_nSrcWidth = bmp.bmWidth;
	m_nSrcHeight = bmp.bmHeight;
}

BitmapHelper::~BitmapHelper()
{
	if (m_hBitmap != NULL)         
	{
		::DeleteObject(m_hBitmap);
	}
}

void BitmapHelper::ShowOnDevice(HDC hdc, int x, int y)
{
	if (this->m_hBitmap == NULL)
	{
		return;
	}
	if (this->m_hBitmap != NULL)
	{
		BITMAP bitmap;
		::GetObject(m_hBitmap, sizeof(BITMAP), &bitmap);

		HDC hdcMem = ::CreateCompatibleDC(hdc);
		::SelectObject(hdcMem,m_hBitmap);
		::BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
		::DeleteDC(hdcMem);
	}
}

void BitmapHelper::ShowOnWindow(HWND hwnd, int x, int y)
{
	if (this->m_hBitmap == NULL)
	{
		return;
	}
	RECT rcClient;
	::GetClientRect(hwnd,&rcClient);
	BYTE *phClientRectBitmapBits = NULL;
	HBITMAP hClientRectBitmap = NULL;
	HDC hdc = ::GetDC(hwnd);
	hClientRectBitmap = CreateBitmapSection(hdc, rcClient.right, rcClient.bottom, &phClientRectBitmapBits);
	if (this->m_hBitmap != NULL&&hClientRectBitmap!=NULL)
	{
		BITMAP bitmap;
		::GetObject(this->m_hBitmap, sizeof(BITMAP), &bitmap);

		HDC hdcBackGroud = ::CreateCompatibleDC(hdc);
		HDC hdcMem = ::CreateCompatibleDC(hdc);
		::SelectObject(hdcBackGroud, hClientRectBitmap);
		::SelectObject(hdcMem, this->m_hBitmap);

		::SetStretchBltMode(hdc,HALFTONE);
		//::BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
		::StretchBlt(hdc,0, 0, rcClient.right, rcClient.bottom, hdcBackGroud,0,0, rcClient.right, rcClient.bottom, SRCCOPY);
		//int nStartPosX = 0, nStartPosY = 0, nHeight = 0, nWidth = 0;
		//CalPaintData(nStartPosX,nStartPosY,nHeight,nWidth,rcClient, bitmap.bmWidth, bitmap.bmHeight);
		//::StretchBlt(hdc, nStartPosX, nStartPosY, nWidth , nHeight, hdcMem,0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		//TransParentPicture(hdc,m_hBitmap, hClientRectBitmap, rcClient, 0);
		//there is something error 
		::DeleteDC(hdcMem);
		::DeleteDC(hdcBackGroud);
		
	}
	::ReleaseDC(hwnd, hdc);

	if (m_hBitmap!=NULL)
	{
		::DeleteObject(m_hBitmap);
	}
	if (hClientRectBitmap != NULL)         
	{
		::DeleteObject(hClientRectBitmap);
	}
	if (phClientRectBitmapBits != NULL)
	{
		::DeleteObject(phClientRectBitmapBits);
	}
	::UpdateWindow(hwnd);
	
}

void BitmapHelper::ShowOnWindow(HWND hwnd)
{
	this->ShowOnWindow(hwnd, 0, 0);
}

HBITMAP BitmapHelper::CreateBitmapSection(HDC hdc, int cx, int cy, BYTE** ppbit)
{
	BITMAPINFO bmi;
	memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = cy;

	return CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)ppbit, NULL, 0);
}

void BitmapHelper::CalPaintData(int &nStartPosX, int &nStartPosY, int &nHeight, int &nWidth,RECT rcClient, int nPicWidth, int nPicHeight)
{
	int nRcWidth = rcClient.right;
	int nRcHeight = rcClient.bottom;
	bool bBigPic = false;
	if (nPicWidth> nRcWidth)
	{
		bBigPic = true;
	}
	if (nPicHeight>nRcHeight)
	{
		bBigPic = true;
	}
	float fScale = 0;
	nWidth = 0; nHeight = 0;
	if (bBigPic)
	{
		fScale = min((float)nRcWidth / (float)nPicWidth, (float)nRcHeight / (float)nPicHeight);
		nWidth = nPicWidth *fScale;
		nHeight = nPicHeight * fScale;
	}
	else
	{
		fScale = min((float)nPicWidth / (float)nRcWidth, (float)nPicHeight / (float)nRcHeight);
		nWidth = nRcWidth * fScale;
		nHeight = nRcWidth * fScale;
	}
	nStartPosX = (nRcWidth - nWidth) / 2;
	nStartPosY = (nHeight - nHeight) / 2;

}
/*void BitmapHelper::TransParentPicture(HDC hdc, HBITMAP hOneBitmap, HBITMAP hTwoBitmap, CRect ClientRect, int Alpha)
{
	BITMAP OneBitmap;
	BITMAP TwoBitmap;
	GetObject(hOneBitmap, sizeof(BITMAP), &OneBitmap);
	GetObject(hTwoBitmap, sizeof(BITMAP), &TwoBitmap);

	int nOneImageWidth = OneBitmap.bmWidth;
	int nOneImageHeight = OneBitmap.bmHeight;
	int nTwoImageWidth = TwoBitmap.bmWidth;
	int nTwoImageHeight = TwoBitmap.bmHeight;

	if (nOneImageWidth < nTwoImageWidth)
	{
		nOneImageWidth = nTwoImageWidth;
	}
	else
	{
		nTwoImageWidth = nOneImageWidth;
	}

	if (nOneImageHeight < nTwoImageHeight)
	{
		nOneImageHeight = nTwoImageHeight;
	}
	else
	{
		nTwoImageHeight = nOneImageHeight;
	}

	int nImageWidth = nOneImageWidth;
	int nImageHeight = nOneImageHeight;
	HDC hdcMem1 = NULL, hdcMem2 = NULL;
	HBITMAP hOldBitmap1 = NULL, hOldBitmap2 = NULL;

	BYTE *pOneBits = NULL;
	hdcMem1 = ::CreateCompatibleDC(hdc);
	hdcMem2 = ::CreateCompatibleDC(hdc);

	HBITMAP hOneBmp = NULL;
	hOneBmp = CreateBitmapSection(hdc, nImageWidth, nImageHeight, &pOneBits);

	hOldBitmap1 = (HBITMAP)SelectObject(hdcMem1, hOneBmp);
	hOldBitmap2 = (HBITMAP)SelectObject(hdcMem2, hOneBitmap);
	//BitBlt(MemDC1.m_hDC, 0, 0, m_nTwoImage_w, m_nTwoImage_h, MemDC2.m_hDC, 0, 0, SRCCOPY);
	StretchBlt(hdcMem1, 0, 0, nImageWidth, nImageHeight,

		hdcMem2, 0, 0, OneBitmap.bmWidth, OneBitmap.bmHeight, SRCCOPY);

	HDC hdcMem3 = NULL, hdcMem4 = NULL;
	HBITMAP hOldBitmap3 = NULL, hOldBitmap4 = NULL;

	BYTE *pTwoBits = NULL;
	hdcMem3 = ::CreateCompatibleDC(hdc);
	hdcMem4 = ::CreateCompatibleDC(hdc);
	HBITMAP hTwoBmp = NULL;
	hTwoBmp = CreateBitmapSection(hdc, nImageWidth, nImageHeight, &pTwoBits);
	hOldBitmap3 = (HBITMAP)SelectObject(hdcMem3, hTwoBmp);
	hOldBitmap4 = (HBITMAP)SelectObject(hdcMem4, hTwoBitmap);
	//BitBlt(MemDC3.m_hDC, 0, 0, m_nTwoImage_w, m_nTwoImage_h, MemDC4.m_hDC, 0, 0, SRCCOPY);
	StretchBlt(hdcMem3, 0, 0, nImageWidth, nImageHeight,
		hdcMem4, 0, 0, TwoBitmap.bmWidth, TwoBitmap.bmHeight, SRCCOPY);

	HDC hdcMem5 = NULL;
	HBITMAP hOldBitmap5 = NULL;
	BYTE* pNewBits = NULL;
	hdcMem5 = ::CreateCompatibleDC(hdc);

	HBITMAP hNewBitmap = NULL;
	hNewBitmap = CreateBitmapSection(hdcMem5, nImageWidth, nImageHeight, &pNewBits);
	hOldBitmap5 = (HBITMAP)SelectObject(hdcMem5, hNewBitmap);
	int linebytes = 0;
	int col = 0;
	int line = (((nImageWidth * 24) + 31) >> 5) << 2;
	linebytes = line;
	col = nImageHeight;
	for (int j = 0; j < col; j++)
	{
		for (int i = 0; i < linebytes; i++)
		{
			*(pNewBits + j * line + i) = (*(pOneBits + j * line + i)*Alpha +
				*(pTwoBits + j * line + i)*(255 - Alpha)) / 255;
		}
	}
	::BitBlt(hdc, 0, 0, ClientRect.Width(), ClientRect.Height(), hdcMem5, 0, 0, SRCCOPY);
	SelectObject(hdcMem5, hOldBitmap5);
	::DeleteDC(hdcMem5);
	SelectObject(hdcMem4, hOldBitmap4);
	::DeleteDC(hdcMem4);
	SelectObject(hdcMem3, hOldBitmap3);
	::DeleteDC(hdcMem3);
	SelectObject(hdcMem2, hOldBitmap2);
	::DeleteDC(hdcMem2);
	SelectObject(hdcMem1, hOldBitmap1);
	::DeleteDC(hdcMem1);
}
*/
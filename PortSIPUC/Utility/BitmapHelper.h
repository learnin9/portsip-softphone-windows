#pragma once
class CDC;
/********************************************************************************
	BitmapHelper *pBmp = new BitmapHelper(L"D:\\100-300.bmp");
	pBmp->ShowOnWindow(this->m_hWnd);
	delete pBmp;  //
********************************************************************************/
class BitmapHelper
{
public:
	BitmapHelper(HBITMAP hBitmap);
	~BitmapHelper();

protected:
	HBITMAP m_hBitmap;       
	int m_nSrcWidth;         
	int m_nSrcHeight;        
protected:
	void ShowOnWindow(HWND hwnd, int x, int y);   
	HBITMAP CreateBitmapSection(HDC hdc, int cx, int cy, BYTE** ppbit);
	void CalPaintData(int &nStartPosX, int &nStartPosY, int &nHeight, int &nWidth,RECT rcClient,int nPicWidth,int nPicHeight);
public:
	void ShowOnWindow(HWND hwnd);
	void ShowOnDevice(HDC dc, int x, int y);

};

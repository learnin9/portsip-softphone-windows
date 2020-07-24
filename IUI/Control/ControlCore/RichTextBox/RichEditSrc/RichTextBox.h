#pragma once

#include <RichOle.h>

class RichTextBox : public CWnd
{
public:
	RichTextBox();
	~RichTextBox();

	virtual BOOL Create(DWORD dwStyle,
		const RECT &rect, CWnd *pParentWnd, LPCTSTR lpszName);

	//<<Begin Copy from CRichEditCtrl
public:
	// Attributes
	BOOL CanUndo() const;
	BOOL CanRedo() const;
	UNDONAMEID GetUndoName() const;
	UNDONAMEID GetRedoName() const;
	int GetLineCount() const;
	BOOL GetModify() const;
	void SetModify(__in BOOL bModified = TRUE);
	BOOL SetTextMode(__in UINT fMode);
	UINT GetTextMode() const;
	void GetRect(__out LPRECT lpRect) const;
	CPoint GetCharPos(__in long lChar) const;
	UINT GetOptions() const;
	void SetOptions(__in WORD wOp, __in DWORD dwFlags);
	BOOL SetAutoURLDetect(__in BOOL bEnable = TRUE);
	UINT GetWordWrapMode() const;
	UINT SetWordWrapMode(__in UINT uFlags) const;
	BOOL GetPunctuation(__in UINT fType, __out PUNCTUATION *lpPunc) const;
	BOOL SetPunctuation(__in UINT fType, __in PUNCTUATION *lpPunc);

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	// NOTE: Copied line does not contain null character!
	int GetLine(__in int nIndex, LPTSTR lpszBuffer) const;
	// NOTE: Copied line does not contain null character!
	int GetLine(__in int nIndex, LPTSTR lpszBuffer, __in int nMaxLength) const;

	BOOL CanPaste(__in UINT nFormat = 0) const;
	void GetSel(__out long &nStartChar, __out long &nEndChar) const;
	void GetSel(__out CHARRANGE &cr) const;
	void LimitText(__in long nChars = 0);
	long LineFromChar(__in long nIndex) const;
	CPoint PosFromChar(__in UINT nChar) const;
	int CharFromPos(__in CPoint pt) const;

	void SetSel(__in long nStartChar, __in long nEndChar);
	void SetSel(__in CHARRANGE &cr);
	DWORD GetDefaultCharFormat(__out CHARFORMAT &cf) const;
	DWORD GetDefaultCharFormat(__out CHARFORMAT2 &cf) const;
	DWORD GetSelectionCharFormat(__out CHARFORMAT &cf) const;
	DWORD GetSelectionCharFormat(__out CHARFORMAT2 &cf) const;
	long GetEventMask() const;
	long GetLimitText() const;
	DWORD GetParaFormat(__out PARAFORMAT &pf) const;
	DWORD GetParaFormat(__out PARAFORMAT2 &pf) const;
	// #if(_MSC_VER > 1310) // VC2003
	// 		// richedit EM_GETSELTEXT is ANSI
	// 		_AFX_INSECURE_DEPRECATE("GetSelText(char *) is unsafe. Instead, use GetSelText(void) returning CString")
	// #endif
	// 		long GetSelText(__out LPSTR lpBuf) const;
	int GetTextRange(__in int nFirst, __in int nLast, __out CString &refString) const;
	CString GetSelText() const;
	WORD GetSelectionType() const;
	COLORREF SetBackgroundColor(__in BOOL bSysColor, __in COLORREF cr);
	BOOL SetDefaultCharFormat(__in CHARFORMAT &cf);
	BOOL SetDefaultCharFormat(__in CHARFORMAT2 &cf);
	BOOL SetSelectionCharFormat(__in CHARFORMAT &cf);
	BOOL SetSelectionCharFormat(__in CHARFORMAT2 &cf);
	BOOL SetWordCharFormat(__in CHARFORMAT &cf);
	BOOL SetWordCharFormat(__in CHARFORMAT2 &cf);
	DWORD SetEventMask(__in DWORD dwEventMask);
	BOOL SetParaFormat(__in PARAFORMAT &pf);
	BOOL SetParaFormat(__in PARAFORMAT2 &pf);
	BOOL SetTargetDevice(__in HDC hDC, __in long lLineWidth);
	//BOOL SetTargetDevice(__in CDC &dc, __in long lLineWidth);
	long GetTextLength() const;
	long GetTextLengthEx(__in DWORD dwFlags, __in UINT uCodePage = -1) const;
	BOOL SetReadOnly(__in BOOL bReadOnly = TRUE);
	int GetFirstVisibleLine() const;

	// Operations
	void EmptyUndoBuffer();
	void StopGroupTyping();
	UINT SetUndoLimit(__in UINT nLimit);

	// Gets the character index of the first character of a specified line in a multiline edit control
	int LineIndex(__in int nLine = -1) const;
	int LineLength(__in int nLine = -1) const;
	void LineScroll(__in int nLines, __in int nChars = 0);
	void ReplaceSel(LPCTSTR lpszNewText, __in BOOL bCanUndo = FALSE);
	void SetRect(__in LPCRECT lpRect);

	BOOL DisplayBand(__in LPRECT pDisplayRect);
	long FindText(__in DWORD dwFlags, __out FINDTEXTEX *pFindText) const;
	DWORD FindWordBreak(__in UINT nCode, __in DWORD nStart) const;
	long FormatRange(__in FORMATRANGE *pfr, __in BOOL bDisplay = TRUE);
	void HideSelection(__in BOOL bHide, __in BOOL bPerm);
	void PasteSpecial(__in UINT nClipFormat, __in DWORD dvAspect = 0, __in HMETAFILE hMF = 0);
	void RequestResize();
	long StreamIn(__in int nFormat, EDITSTREAM &es);
	long StreamOut(__in int nFormat, EDITSTREAM &es);

	// Clipboard operations
	BOOL Undo();
	BOOL Redo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();

	// OLE support
	IRichEditOle *GetIRichEditOle() const;
	BOOL SetOLECallback(IRichEditOleCallback *pCallback);
	//<< End Copy from CRichEditCtrl

public:
	int InsertOle(IOleObject *pOleObject, LONG cp, int nWidth, int nHeight);
	int GetFirstVisibleOleOjbect(__out REOBJECT *pObj);
	IOleObject *HitTest(LPPOINT lpPt);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

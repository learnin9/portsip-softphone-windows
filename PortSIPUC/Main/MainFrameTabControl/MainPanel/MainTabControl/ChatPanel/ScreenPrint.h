#if !defined(AFX_DOGCLASS_H__4542D586_228D_48CB_A0D8_228B3E15DD79__INCLUDED_)
#define AFX_DOGCLASS_H__4542D586_228D_48CB_A0D8_228B3E15DD79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class __declspec(dllimport) CScreenPrint
{
private :
	CString m_sData;
public:
	int ShowDlg(long long nSipID, WCHAR * tempPathBuff, char* szFileName, int nBufferLength);
	void Init();
	CScreenPrint();
	virtual ~CScreenPrint();

	void  GetFileName(CString &strFileName);

};

#endif // !defined(AFX_DOGCLASS_H__4542D586_228D_48CB_A0D8_228B3E15DD79__INCLUDED_)

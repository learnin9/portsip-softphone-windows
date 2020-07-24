#include "StdAfx.h"

#ifdef _DEBUG
#define new	IUI_DEBUG_NEW
#endif // _DEBUG


//#undef __IFileDialog_FWD_DEFINED__
//#undef __IFileDialog_INTERFACE_DEFINED__
//#undef CINTERFACE

namespace IUI
{
	FileDialog::FileDialog(LPCTSTR lpszFilter)
	{
		memset(&m_ofn, 0, sizeof(OPENFILENAME)); // initialize structure to 0/NULL
		ZeroMemory(m_szFile, sizeof(TCHAR) * MAX_PATH);
		ZeroMemory(m_szFileTitle, sizeof(TCHAR) * 64);
		ZeroMemory(m_szTitle, sizeof(TCHAR) * MAX_PATH);
		ZeroMemory(m_szDefExt, sizeof(TCHAR) * MAX_PATH);

		// m_ofn.lpstrFilter的格式为:
		// "Text File(*.txt)\0*.txt\0Image File(*.jpg)\0*.jpg;*.png\0\0"
		// 由多个Filer对组成，Filter对之间使用\0分隔，最后一个Filter使用两个\0结束
		// 每个Filte对内部的两个元素也是使用\0分隔。
		// 这种表达方式不方便使用CString初始化，所以MFC用|代替\0。
		// IUI也使用MFC这种方式：
		// "Text File(*.txt)|*.txt|Image File(*.jpg;*.png)|0*.jpg;*.png||"
		int nLen = _tcslen(lpszFilter) + 1;
		m_pszFilters = new TCHAR[nLen];
		StringCchCopy(m_pszFilters, nLen, lpszFilter);
		LPTSTR pch = m_pszFilters; // modify the buffer in place
		// MFC delimits with '|' not '\0'
		while ((pch = _tcschr(pch, '|')) != NULL)
		{
			*pch++ = '\0';
		}

		m_ofn.lStructSize = sizeof(OPENFILENAME);
		m_ofn.lpstrFile = m_szFile;
		m_ofn.nMaxFile = MAX_PATH;
		m_ofn.lpstrDefExt = m_szDefExt;
		m_ofn.lpstrFileTitle = m_szFileTitle;
		m_ofn.nMaxFileTitle = MAX_PATH;
		m_ofn.hwndOwner = NULL;
		m_ofn.lpstrFilter = m_pszFilters;
		m_ofn.lpstrInitialDir = (LPTSTR)NULL;
		m_ofn.Flags |= OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK | OFN_EXPLORER;
		m_ofn.lpstrTitle = m_szTitle;
	}

	FileDialog::~FileDialog()
	{
		delete[] m_pszFilters;
		m_pszFilters = NULL;
	}

	INT FileDialog::DoModal()
	{
		HWND m_hWndTop = NULL;
		HWND hWnd = GetSafeOwner_(NULL, &m_hWndTop);

		//  This is a special case for the file open/save dialog,
		//  which sometimes pumps while it is coming up but before it has
		//  disabled the main window.
		HWND hWndFocus = ::GetFocus();
		BOOL bEnableParent = FALSE;
		m_ofn.hwndOwner = hWnd;

		if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
		{
			bEnableParent = TRUE;
			::EnableWindow(m_ofn.hwndOwner, FALSE);
		}

		// Display the Filename common dialog box. The
		// filename specified by the user is passed
		// to the CreateEnhMetaFile function and used to
		// store the metafile on disk.
		INT_PTR nResult = GetOpenFileName(&m_ofn);

		// Second part of special case for file open/save dialog.
		if (bEnableParent)
		{
			::EnableWindow(m_ofn.hwndOwner, TRUE);
		}

		if (::IsWindow(hWndFocus))
		{
			::SetFocus(hWndFocus);
		}

		// CDialog::PostModal内容展开
		//Detach();
		if (::IsWindow(m_hWndTop))
		{
			::EnableWindow(m_hWndTop, TRUE);
		}
		m_hWndTop = NULL;

		return nResult ? nResult : IDCANCEL;
	}

	CIUIString FileDialog::GetPathName() const
	{
		return m_ofn.lpstrFile;
	}

	CIUIString FileDialog::GetFileName() const
	{
		return m_ofn.lpstrFileTitle;
	}

	CIUIString FileDialog::GetFileExt() const
	{
		return "";
	}

	CIUIString FileDialog::GetFileTitle() const
	{
		CIUIString strResult = GetFileName();
		LPTSTR pszBuffer = strResult.GetBuffer(0);
		::PathRemoveExtension(pszBuffer);
		strResult.ReleaseBuffer();
		return strResult;
	}

	CIUIString FileDialog::GetFolderPath() const
	{
		return "";
	}


	//////////////////////////////////////////////////////////////////////////

	// To avoid C4238.
#define USE_INTERFACE_PART_STD(localClass) \
	X##localClass tmp##localClass; \
	m_x##localClass.m_vtbl = *(DWORD_PTR*)&tmp##localClass;

	const IID IID_IFileOpenDialog = {0xd57c7288, 0xd4ad, 0x4768, {0xbe, 0x02, 0x9d, 0x96, 0x95, 0x32, 0xd9, 0x60}};
	const IID IID_IFileSaveDialog = {0x84bccd23, 0x5fde, 0x4cdb, {0xae, 0xa4, 0xaf, 0x64, 0xb8, 0x3d, 0x78, 0xab}};
	const IID IID_IFileDialogCustomize = {0xe6fdd21a, 0x163f, 0x4975, {0x9c, 0x8c, 0xa6, 0x9f, 0x1b, 0xa3, 0x70, 0x34}};

	CFileDialogUI::CFileDialogUI(
		BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt/* = NULL*/,
		LPCTSTR lpszFileName/* = NULL*/,
		DWORD dwFlags/* = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/,
		LPCTSTR lpszFilter/* = NULL*/,
		HWND hParentWnd/* = NULL*/,
		DWORD dwSize/* = 0*/,
		BOOL bVistaStyle/* = TRUE*/)
		: m_hParent(hParentWnd)
	{
		m_hWnd = NULL;

		OSVERSIONINFO vi;
		ZeroMemory(&vi, sizeof(OSVERSIONINFO));
		vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		// Fix for warnings when building against WinBlue build 9444.0.130614-1739
		// warning C4996: 'GetVersionExW': was declared deprecated
		// externalapis\windows\winblue\sdk\inc\sysinfoapi.h(442)
		// Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers.
#pragma warning( disable : 4996 )
		::GetVersionEx(&vi);
#pragma warning( default : 4996 )

		// if running under Vista
		if (vi.dwMajorVersion >= 6)
		{
			m_bVistaStyle = bVistaStyle;
		}
		else
		{
			m_bVistaStyle = FALSE;
		}

		m_bPickFoldersMode = FALSE;

		// determine size of OPENFILENAME struct if dwSize is zero
		if (dwSize == 0)
		{
			dwSize = sizeof(OPENFILENAME);
		}

		// size of OPENFILENAME must be at least version 5
		_ASSERT(dwSize >= sizeof(OPENFILENAME));
		// allocate memory for OPENFILENAME struct based on size passed in
		m_pOFN = static_cast<LPOPENFILENAME>(malloc(dwSize));
		_ASSERT(m_pOFN != NULL);

		if (m_pOFN == NULL)
		{
			_ASSERT(FALSE);
		}

		memset(&m_ofn, 0, dwSize); // initialize structure to 0/NULL
		m_szFileName[0] = '\0';
		m_szFileTitle[0] = '\0';
		m_pofnTemp = NULL;

		m_bOpenFileDialog = bOpenFileDialog;
		//m_nIDHelp = bOpenFileDialog ? AFX_IDD_FILEOPEN : AFX_IDD_FILESAVE;

		m_ofn.lStructSize = dwSize;
		m_ofn.lpstrFile = m_szFileName;
		m_ofn.nMaxFile = _countof(m_szFileName);
		m_ofn.lpstrDefExt = lpszDefExt;
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _countof(m_szFileTitle);
		m_ofn.Flags |= dwFlags/* | OFN_ENABLEHOOK*/ | OFN_EXPLORER;

		if (dwFlags & OFN_ENABLETEMPLATE)
		{
			m_ofn.Flags &= ~OFN_ENABLESIZING;
		}

		m_ofn.hInstance = IUIGetResourceHandle();
		//m_ofn.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

		// setup initial file name
		if (lpszFileName != NULL)
		{
			IUIStringCopy(m_szFileName, _countof(m_szFileName), lpszFileName, _TRUNCATE);
		}

		// Translate filter into commdlg format (lots of \0)
		if (lpszFilter != NULL)
		{
			m_strFilter = lpszFilter;
			LPTSTR pch = (LPTSTR)m_strFilter.GetBuffer(0); // modify the buffer in place

			// MFC delimits with '|' not '\0'
			while ((pch = _tcschr(pch, '|')) != NULL)
			{
				*pch++ = '\0';
			}

			m_ofn.lpstrFilter = m_strFilter;
			// do not call ReleaseBuffer() since the string contains '\0' characters
		}

		//#define NTDDI_VERSION NTDDI_VISTA
		//        int n = NTDDI_VERSION;
		//#if (NTDDI_VERSION >= NTDDI_VISTA)
		//        if (m_bVistaStyle == TRUE)
		//        {
		//            if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		//            { // multi-threaded is not supported
		//                IFileDialog* pIFileDialog;
		//                IFileDialogCustomize* pIFileDialogCustomize;
		//
		//                HRESULT hr;
		//                //XFileDialogEvents;
		//                //USE_INTERFACE_PART_STD(FileDialogEvents);
		//                //USE_INTERFACE_PART_STD(FileDialogControlEvents);
		//
		//
		//                if (m_bOpenFileDialog)
		//                {
		//                    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
		//                        IID_IFileOpenDialog, (void **)&pIFileDialog);
		//                }
		//                else
		//                {
		//                    hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER,
		//                        IID_IFileSaveDialog, (void **)&pIFileDialog);
		//                }
		//                if (FAILED(hr))
		//                {
		//                    m_bVistaStyle = FALSE;
		//                    return;
		//                }
		//
		//                hr = pIFileDialog->QueryInterface(IID_IFileDialogCustomize, (void **)&pIFileDialogCustomize);
		//                if (!SUCCEEDED(hr))
		//                {
		//
		//                }
		//
		//                //hr = pIFileDialog->Advise(reinterpret_cast<IFileDialogEvents*>(&m_xFileDialogEvents), &m_dwCookie);
		//                //if (!SUCCEEDED(hr))
		//                //{
		//
		//                //}
		//
		//                m_pIFileDialog = static_cast<void*>(pIFileDialog);
		//                m_pIFileDialogCustomize = static_cast<void*>(pIFileDialogCustomize);
		//            }
		//            else
		//            {
		//                m_bVistaStyle = FALSE;
		//            }
		//        }
		//#endif // #if (NTDDI_VERSION >= NTDDI_VISTA)

		m_hWndTop = NULL;
		m_dwCookie = 0;
		m_pIFileDialog = NULL;
		m_pIFileDialogCustomize = NULL;
	}

	CFileDialogUI::~CFileDialogUI()
	{

	}

	INT CFileDialogUI::DoModal()
	{
		HWND hWnd = GetSafeOwner_(m_hParent, &m_hWndTop);

		if (hWnd == NULL)
		{
			hWnd = m_hParent;
		}

		HookWindowCreate((Window *)this);

		//ASSERT_VALID(this);
		//_ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
		//_ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

		// zero out the file buffer for consistent parsing later
		//_ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
		DWORD nOffset = lstrlen(m_ofn.lpstrFile) + 1;
		_ASSERT(nOffset <= m_ofn.nMaxFile);
		memset(m_ofn.lpstrFile + nOffset, 0, (m_ofn.nMaxFile - nOffset)*sizeof(TCHAR));

		//  This is a special case for the file open/save dialog,
		//  which sometimes pumps while it is coming up but before it has
		//  disabled the main window.
		HWND hWndFocus = ::GetFocus();
		BOOL bEnableParent = FALSE;
		m_ofn.hwndOwner = hWnd;

		if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
		{
			bEnableParent = TRUE;
			::EnableWindow(m_ofn.hwndOwner, FALSE);
		}

		INT_PTR nResult = 0;

		if (m_bOpenFileDialog)
		{
			nResult = ::GetOpenFileName(&m_ofn);
		}
		else
		{
			nResult = ::GetSaveFileName(&m_ofn);
		}

		UnhookWindowCreate();

		// Second part of special case for file open/save dialog.
		if (bEnableParent)
		{
			::EnableWindow(m_ofn.hwndOwner, TRUE);
		}

		if (::IsWindow(hWndFocus))
		{
			::SetFocus(hWndFocus);
		}

		return nResult ? nResult : IDCANCEL;
	}

	void CFileDialogUI::ApplyOFNToShellDialog()
	{
		//#if (NTDDI_VERSION >= NTDDI_VISTA)
		//        _ASSERT(m_bVistaStyle == TRUE);
		//        if (m_bVistaStyle == TRUE)
		//        {
		//            HRESULT hr;
		//            // m_ofn.lpstrTitle
		//            if(m_ofn.lpstrTitle != NULL)
		//            {
		//#ifdef UNICODE
		//                hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetTitle(m_ofn.lpstrTitle);
		//                //ENSURE(SUCCEEDED(hr));
		//#else
		//                CStringW strTitle(m_ofn.lpstrTitle);
		//                hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetTitle(strTitle.GetString());
		//                //ENSURE(SUCCEEDED(hr));
		//#endif
		//            }
		//            // m_ofn.lpstrDefExt
		//            if(m_ofn.lpstrDefExt != NULL)
		//            {
		//                CStringW strDefExt(m_ofn.lpstrDefExt);
		//                hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetDefaultExtension(strDefExt.GetString());
		//                //ENSURE(SUCCEEDED(hr));
		//            }
		//            // m_ofn.lpstrFilter
		//            if(m_ofn.lpstrFilter != NULL)
		//            {
		//                UINT nFilterCount = 0;
		//                LPCTSTR lpstrFilter = m_ofn.lpstrFilter;
		//                while(lpstrFilter[0])
		//                {
		//                    lpstrFilter += _tcslen(lpstrFilter)+1;
		//                    lpstrFilter += _tcslen(lpstrFilter)+1;
		//                    nFilterCount ++;
		//                }
		//                if (nFilterCount > 0)
		//                {
		//                    COMDLG_FILTERSPEC* pFilter = NULL;
		//                    pFilter = new COMDLG_FILTERSPEC[nFilterCount];
		//                    _ASSERT(pFilter != NULL);
		//                    if (pFilter == NULL)
		//                    {
		//                        //AfxThrowMemoryException();
		//                        return;
		//                    }
		//                    lpstrFilter = m_ofn.lpstrFilter;
		//                    size_t nFilterIndex = 0;
		//                    size_t filterSize;
		//                    LPWSTR lpwstrFilter;
		//                    while (nFilterIndex < nFilterCount)
		//                    {
		//                        CStringW strTemp;
		//
		//                        filterSize = _tcslen(lpstrFilter)+1;
		//                        lpwstrFilter = static_cast<LPWSTR>(new WCHAR[filterSize]);
		//                        _ASSERT(lpwstrFilter != NULL);
		//                        if (lpwstrFilter == NULL)
		//                        {
		//                            return;
		//                            //AfxThrowMemoryException();
		//                        }
		//                        strTemp = lpstrFilter;
		//                        memcpy_s(lpwstrFilter, (strTemp.GetLength()+1)*sizeof(WCHAR),
		//                            strTemp.GetString(), (strTemp.GetLength()+1)*sizeof(WCHAR));
		//                        pFilter[nFilterIndex].pszName = lpwstrFilter;
		//                        lpstrFilter += filterSize;
		//
		//                        filterSize = _tcslen(lpstrFilter)+1;
		//                        lpwstrFilter = static_cast<LPWSTR>(new WCHAR[filterSize]);
		//                        _ASSERT(lpwstrFilter != NULL);
		//                        if (lpwstrFilter == NULL)
		//                        {
		//                            return;
		//                            //AfxThrowMemoryException();
		//                        }
		//                        strTemp = lpstrFilter;
		//                        memcpy_s(lpwstrFilter, (strTemp.GetLength()+1)*sizeof(WCHAR),
		//                            strTemp.GetString(), (strTemp.GetLength()+1)*sizeof(WCHAR));
		//                        pFilter[nFilterIndex].pszSpec = lpwstrFilter;
		//                        lpstrFilter += filterSize;
		//
		//                        nFilterIndex ++;
		//                    }
		//                    hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetFileTypes(nFilterCount, pFilter);
		//                    //ENSURE(SUCCEEDED(hr));
		//                    for (nFilterIndex = 0; nFilterIndex < nFilterCount; nFilterIndex++)
		//                    {
		//                        delete[] pFilter[nFilterIndex].pszName;
		//                        delete[] pFilter[nFilterIndex].pszSpec;
		//                    }
		//                    delete[] pFilter;
		//
		//                    hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetFileTypeIndex(m_ofn.nFilterIndex > 1 ? m_ofn.nFilterIndex : 1);
		//                    //ENSURE(SUCCEEDED(hr));
		//                }
		//            }
		//            // m_ofn.lpstrFile and m_ofn.lpstrInitialDir
		//            if((m_ofn.lpstrFile != NULL) || (m_ofn.lpstrInitialDir != NULL))
		//            {
		//                CStringW strInitialDir;
		//                if(m_ofn.lpstrFile != NULL)
		//                {
		//                    CStringW strFile(m_ofn.lpstrFile);
		//                    strInitialDir = strFile;
		//                    ::PathRemoveFileSpecW(strInitialDir.GetBuffer());
		//                    strInitialDir.ReleaseBuffer();
		//                    int offset = strInitialDir.GetLength();
		//                    if (strFile[offset] == L'\\')
		//                    {
		//                        offset++;
		//                    }
		//                    hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetFileName(strFile.GetString() + offset);
		//                    //ENSURE(SUCCEEDED(hr));
		//                }
		//                if((m_ofn.lpstrInitialDir != NULL) && strInitialDir.IsEmpty())
		//                {
		//                    strInitialDir = m_ofn.lpstrInitialDir;
		//                }
		//                if(!strInitialDir.IsEmpty())
		//                {
		//                    IShellItem *psiInitialDir = NULL;
		//
		//                    _ASSERT(FALSE);
		//                    //hr = afxGlobalData.ShellCreateItemFromParsingName(
		//                    //    strInitialDir.GetString(),
		//                    //    NULL,
		//                    //    IID_PPV_ARGS(&psiInitialDir));
		//
		//                    if (SUCCEEDED(hr))
		//                    {
		//                        hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetFolder(psiInitialDir);
		//                        //ENSURE(SUCCEEDED(hr));
		//                        psiInitialDir->Release();
		//                    }
		//                }
		//            }
		//            // m_ofn.Flags
		//            DWORD dwFlags = 0;
		//            hr = (static_cast<IFileDialog*>(m_pIFileDialog))->GetOptions(&dwFlags);
		//            //ENSURE(SUCCEEDED(hr));
		//
		//#ifndef VISTA_FILE_DIALOG_FLAG_MAPPING
		//#define VISTA_FILE_DIALOG_FLAG_MAPPING(OLD,NEW) \
		//    ((m_ofn.Flags & (OLD)) ? (dwFlags |= (NEW)) : (dwFlags &= ~(NEW)))
		//#ifndef VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING
		//#define VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(FLAG) \
		//    VISTA_FILE_DIALOG_FLAG_MAPPING(OFN_##FLAG, FOS_##FLAG)
		//
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(ALLOWMULTISELECT);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(CREATEPROMPT);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(DONTADDTORECENT);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(FILEMUSTEXIST);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(FORCESHOWHIDDEN);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(NOCHANGEDIR);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(NODEREFERENCELINKS);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(NOREADONLYRETURN);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(NOTESTFILECREATE);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(NOVALIDATE);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(OVERWRITEPROMPT);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(PATHMUSTEXIST);
		//            VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING(SHAREAWARE);
		//
		//            (m_ofn.FlagsEx & OFN_EX_NOPLACESBAR) ? (dwFlags |=  FOS_HIDEPINNEDPLACES) : (dwFlags &= ~FOS_HIDEPINNEDPLACES);
		//            m_bPickFoldersMode ? (dwFlags |=  FOS_PICKFOLDERS) : (dwFlags &= ~FOS_PICKFOLDERS);
		//
		//#undef VISTA_FILE_DIALOG_FLAG_DIRECT_MAPPING
		//#undef VISTA_FILE_DIALOG_FLAG_MAPPING
		//#endif
		//#endif
		//            hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetOptions(dwFlags);
		//            //ENSURE(SUCCEEDED(hr));
		//        }
		//#endif // NTDDI_VERSION >= NTDDI_VISTA
	}

	CIUIString CFileDialogUI::GetPathName() const
	{
		//if (m_bVistaStyle == TRUE)
		//{
		//    if (m_hWnd != NULL)
		//    {
		//        CIUIString strResult;
		//        IShellItem *psiResult;
		//        HRESULT hr = (static_cast<IFileDialog*>(m_pIFileDialog))->GetCurrentSelection(&psiResult);
		//        if (SUCCEEDED(hr))
		//        {
		//            SFGAOF sfgaoAttribs;
		//            if ((psiResult->GetAttributes(SFGAO_STREAM, &sfgaoAttribs) == S_FALSE)
		//                && (psiResult->GetAttributes(SFGAO_FOLDER, &sfgaoAttribs) == S_OK))
		//            {
		//                ;
		//            }
		//            else
		//            {
		//                LPWSTR wcPathName = NULL;
		//                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &wcPathName);
		//                if (SUCCEEDED(hr))
		//                {
		//                    strResult = wcPathName;
		//                    strResult.ReleaseBuffer();
		//                    CoTaskMemFree(wcPathName);
		//                }
		//            }
		//            psiResult->Release();
		//        }
		//        return strResult;
		//    }
		//    else
		//    {
		//        return m_ofn.lpstrFile;
		//    }
		//}
		//else
		if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
		{
			_ASSERT(::IsWindow(m_hWnd));
			CIUIString strResult;

			if (::SendMessage(::GetParent(m_hWnd), CDM_GETSPEC, (WPARAM)MAX_PATH,
					(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
			{
				strResult.Empty();
			}
			else
			{
				strResult.ReleaseBuffer();
			}

			if (!strResult.IsEmpty())
			{
				if (::SendMessage(::GetParent(m_hWnd), CDM_GETFILEPATH, (WPARAM)MAX_PATH,
						(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
				{
					strResult.Empty();
				}
				else
				{
					strResult.ReleaseBuffer();
					return strResult;
				}
			}
		}

		return m_ofn.lpstrFile;
	}

	CIUIString CFileDialogUI::GetFileName() const
	{
		//if (m_bVistaStyle == TRUE)
		//{
		//    if (m_hWnd != NULL)
		//    {
		//        LPWSTR wcFileName;
		//        HRESULT hr = (static_cast<IFileDialog*>(m_pIFileDialog))->GetFileName(&wcFileName);
		//        CIUIString strResult(wcFileName);

		//        if (SUCCEEDED(hr))
		//        {
		//            CoTaskMemFree(wcFileName);
		//        }
		//        strResult.ReleaseBuffer();
		//        return strResult;
		//    }
		//    else
		//    {
		//        return m_ofn.lpstrFileTitle;
		//    }
		//}
		//else
		if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
		{
			_ASSERT(::IsWindow(m_hWnd));
			CIUIString strResult;

			if (::SendMessage(::GetParent(m_hWnd), CDM_GETSPEC, (WPARAM)MAX_PATH,
					(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
			{
				strResult.Empty();
			}
			else
			{
				strResult.ReleaseBuffer();
				return strResult;
			}
		}

		return m_ofn.lpstrFileTitle;
	}

	CIUIString CFileDialogUI::GetFileExt() const
	{
		if (m_bVistaStyle == TRUE)
		{
			CIUIString strResult;

			if (m_hWnd != NULL)
			{
				strResult = GetFileName();
			}
			else
			{
				strResult = GetPathName();
			}

			strResult.ReleaseBuffer();
			LPTSTR pszExtension = ::PathFindExtension(strResult);

			if (pszExtension != NULL && *pszExtension == _T('.'))
			{
				return pszExtension + 1;
			}

			strResult.Empty();
			return strResult;
		}
		else if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
		{
			_ASSERT(::IsWindow(m_hWnd));
			CIUIString strResult;
			LPTSTR pszResult = strResult.GetBuffer(MAX_PATH);
			LRESULT nResult = ::SendMessage(::GetParent(m_hWnd), CDM_GETSPEC, MAX_PATH,
					reinterpret_cast<LPARAM>(pszResult));
			strResult.ReleaseBuffer();

			if (nResult >= 0)
			{
				LPTSTR pszExtension = ::PathFindExtension(strResult);

				if (pszExtension != NULL && *pszExtension == _T('.'))
				{
					return pszExtension + 1;
				}
			}

			strResult.Empty();
			return strResult;
		}

		if (m_pofnTemp != NULL)
			if (m_pofnTemp->nFileExtension == 0)
			{
				return _T("");
			}
			else
			{
				return m_pofnTemp->lpstrFile + m_pofnTemp->nFileExtension;
			}

		if (m_ofn.nFileExtension == 0)
		{
			return _T("");
		}
		else
		{
			return m_ofn.lpstrFile + m_ofn.nFileExtension;
		}
	}

	CIUIString CFileDialogUI::GetFileTitle() const
	{
		CIUIString strResult = GetFileName();
		LPTSTR pszBuffer = strResult.GetBuffer(0);
		::PathRemoveExtension(pszBuffer);
		strResult.ReleaseBuffer();
		return strResult;
	}

	IUIPOS CFileDialogUI::GetStartPosition() const
	{
		return (IUIPOS)m_ofn.lpstrFile;
	}

	CIUIString CFileDialogUI::GetNextPathName(IUIPOS &pos) const
	{
		BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
		TCHAR chDelimiter;

		if (bExplorer)
		{
			chDelimiter = '\0';
		}
		else
		{
			chDelimiter = ' ';
		}

		LPTSTR lpsz = (LPTSTR)pos;

		if (lpsz == m_ofn.lpstrFile)   // first time
		{
			if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}

			// find char pos after first Delimiter
			while (*lpsz != chDelimiter && *lpsz != '\0')
			{
				lpsz = _tcsinc(lpsz);
			}

			lpsz = _tcsinc(lpsz);

			// if single selection then return only selection
			if (*lpsz == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}
		}

		CIUIString strBasePath = m_ofn.lpstrFile;

		if (!bExplorer)
		{
			LPTSTR lpszPath = m_ofn.lpstrFile;

			while (*lpszPath != chDelimiter)
			{
				lpszPath = _tcsinc(lpszPath);
			}

			strBasePath = strBasePath.Left(int(lpszPath - m_ofn.lpstrFile));
		}

		LPTSTR lpszFileName = lpsz;
		CIUIString strFileName = lpsz;

		// find char pos at next Delimiter
		while (*lpsz != chDelimiter && *lpsz != '\0')
		{
			lpsz = _tcsinc(lpsz);
		}

		if (!bExplorer && *lpsz == '\0')
		{
			pos = NULL;
		}
		else
		{
			if (!bExplorer)
			{
				strFileName = strFileName.Left(int(lpsz - lpszFileName));
			}

			lpsz = _tcsinc(lpsz);

			if (*lpsz == '\0')   // if double terminated then done
			{
				pos = NULL;
			}
			else
			{
				pos = (IUIPOS)lpsz;
			}
		}

		TCHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_FNAME], strExt[_MAX_EXT];
		TSPLITPATH(strFileName, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, strName, _MAX_FNAME,
			strExt, _MAX_EXT);
		TCHAR strPath[_MAX_PATH];

		if (*strDrive || *strDir)
		{
			StringCchCopy(strPath, _countof(strPath), strFileName);
		}
		else
		{
			if ((strBasePath.GetLength() != 3) || (strBasePath[1] != ':') || (strBasePath[2] != '\\'))
			{
				strBasePath += _T("\\");
			}

			TSPLITPATH(strBasePath, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
			TMAKEPATH(strPath, _MAX_PATH, strDrive, strDir, strName, strExt);
		}

		return strPath;
	}

	void CFileDialogUI::SetTemplate(LPCTSTR lpWin3ID, LPCTSTR lpWin4ID)
	{
		//if (m_bVistaStyle == TRUE)
		//{
		//    AfxThrowNotSupportedException();
		//}
		if (m_ofn.Flags & OFN_EXPLORER)
		{
			m_ofn.lpTemplateName = lpWin4ID;
		}
		else
		{
			m_ofn.lpTemplateName = lpWin3ID;
		}

		m_ofn.Flags |= OFN_ENABLETEMPLATE;
	}

	CIUIString CFileDialogUI::GetFolderPath() const
	{
		CIUIString strResult;
		//if (m_bVistaStyle == TRUE)
		//{
		//    IShellItem *psiResult;
		//    HRESULT hr = (static_cast<IFileDialog*>(m_pIFileDialog))->GetFolder(&psiResult);
		//    if (SUCCEEDED(hr))
		//    {
		//        LPWSTR wcFolderPath = NULL;
		//        hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &wcFolderPath);
		//        if (SUCCEEDED(hr))
		//        {
		//            strResult = wcFolderPath;
		//            CoTaskMemFree(wcFolderPath);
		//        }
		//        psiResult->Release();
		//    }
		//}
		//else
		{
			_ASSERT(::IsWindow(m_hWnd));
			_ASSERT(m_ofn.Flags & OFN_EXPLORER);

			if (::SendMessage(::GetParent(m_hWnd), CDM_GETFOLDERPATH, (WPARAM)MAX_PATH,
					(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
			{
				strResult.Empty();
			}
			else
			{
				strResult.ReleaseBuffer();
			}
		}
		return strResult;
	}

#ifdef UNICODE
	void CFileDialogUI::SetControlText(int nID, const wchar_t  *lpsz)
	{
		_ASSERT(::IsWindow(m_hWnd));

		//if (m_bVistaStyle == TRUE)
		//{
		//    HRESULT hr = (static_cast<IFileDialogCustomize*>(m_pIFileDialogCustomize))->SetControlLabel(nID, lpsz);
		//    ENSURE(SUCCEEDED(hr));
		//}
		//else
		{
			_ASSERT(m_ofn.Flags & OFN_EXPLORER);
			SendMessage(GetParent(m_hWnd), CDM_SETCONTROLTEXT, (WPARAM)nID, (LPARAM)lpsz);
		}
	}
#endif

	void CFileDialogUI::SetControlText(int nID, LPCSTR lpsz)
	{
		_ASSERT(::IsWindow(m_hWnd));

		//if (m_bVistaStyle == TRUE)
		//{
		//    CStringW dest(lpsz);
		//    HRESULT hr = (static_cast<IFileDialogCustomize*>(m_pIFileDialogCustomize))->SetControlLabel(nID, dest.GetString());
		//    ENSURE(SUCCEEDED(hr));
		//}
		//else
		{
			_ASSERT(m_ofn.Flags & OFN_EXPLORER);

#ifdef UNICODE
			USES_CONVERSION;
			LPCWSTR lpszDest = A2W(lpsz);
			::SendMessage(::GetParent(m_hWnd), CDM_SETCONTROLTEXT, (WPARAM)nID, (LPARAM)lpszDest);
#else
			::SendMessage(::GetParent(m_hWnd), CDM_SETCONTROLTEXT, (WPARAM)nID, (LPARAM)lpsz);
#endif
		}
	}

	void CFileDialogUI::HideControl(int nID)
	{
		_ASSERT(::IsWindow(m_hWnd));
		//if (m_bVistaStyle == TRUE)
		//{
		//    HRESULT hr = (static_cast<IFileDialogCustomize*>(m_pIFileDialogCustomize))->SetControlState(nID, CDCS_INACTIVE);
		//    ENSURE(SUCCEEDED(hr));
		//}
		//else
		{
			_ASSERT(m_ofn.Flags & OFN_EXPLORER);
			::SendMessage(::GetParent(m_hWnd), CDM_HIDECONTROL, (WPARAM)nID, 0);
		}
	}

	void CFileDialogUI::SetDefExt(LPCSTR lpsz)
	{
		_ASSERT(::IsWindow(m_hWnd));
		//if (m_bVistaStyle == TRUE)
		//{
		//    CStringW strExt(lpsz);
		//    HRESULT hr = (static_cast<IFileDialog*>(m_pIFileDialog))->SetDefaultExtension(strExt.GetString());
		//    ENSURE(SUCCEEDED(hr));
		//}
		//else
		{
			_ASSERT(m_ofn.Flags & OFN_EXPLORER);
			::SendMessage(::GetParent(m_hWnd), CDM_SETDEFEXT, 0, (LPARAM)lpsz);
		}
	}


}    // namespace IUI

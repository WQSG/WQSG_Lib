/*  WQSG_Lib
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#ifndef __WQSG_DIRDLG_H__
#define __WQSG_DIRDLG_H__


#include <shtypes.h>
#include <shlobj.h>

class CWQSG_DirDlg
{
	HWND m_hWnd;
	WCHAR m_title[ MAX_PATH ];
	WCHAR m_path[ MAX_PATH ];
public:
	CWQSG_DirDlg( HWND a_hWnd , const WCHAR* a_pTitle = NULL , const WCHAR* a_pPath = NULL )
		: m_hWnd( a_hWnd )
	{
		if( a_pTitle )
			WQSG_strcpy( a_pTitle , m_title );
		else
			WQSG_strcpy( L"请选择一个文件夹" , m_title );

		if( a_pPath )
			WQSG_strcpy( a_pPath , m_path );
		else
			m_path[0] = L'\0';
	}

	virtual ~CWQSG_DirDlg()
	{}

	BOOL GetPath( WCHAR* path )const
	{
		BROWSEINFO bi = {};

		bi.hwndOwner = m_hWnd;
		bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE /*| BIF_BROWSEFORCOMPUTER*/;

		LPITEMIDLIST pPidl = NULL;
		if( m_path[0] )
		{
			IShellFolder* pDesktop = NULL;
			SHGetDesktopFolder( &pDesktop );
			if( pDesktop )
			{
				ULONG pchEaten = 0 , pdwAttributes = 0;
				
				pDesktop->ParseDisplayName( m_hWnd , NULL , (LPWSTR)m_path , &pchEaten , &pPidl , &pdwAttributes );
				pDesktop->Release();
				bi.pidlRoot = pPidl;
			}
		}

		bi.lpszTitle = m_title;
		LPITEMIDLIST ret = SHBrowseForFolder( &bi );

		LPMALLOC pMalloc = NULL;
		SHGetMalloc( &pMalloc );

		if( pPidl )
			pMalloc->Free( pPidl );

		SHGetPathFromIDList( ret , path );
		if( ret )
			pMalloc->Free( ret );
		pMalloc->Release();

		return ( *path );
	}
};

#if defined(__AFXDLGS_H__)
#include <shlwapi.h>
#define DEF_USE_WQSGFileDialog

class CWQSGFileDialog : public CFileDialog
{
	CString m_strFolderPath;
public:
	explicit CWQSGFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0,
		BOOL bVistaStyle = TRUE)
		: CFileDialog( bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags,lpszFilter,pParentWnd,dwSize,bVistaStyle)
	{

	}

	virtual ~CWQSGFileDialog(){}

	CString GetFolderPath() const
	{
		return m_strFolderPath;
	}
protected:
	virtual BOOL OnFileNameOK()
	{
		POSITION pos = GetStartPosition( );
		m_strFolderPath = GetNextPathName( pos );

		::PathRemoveFileSpecW(m_strFolderPath.GetBuffer(MAX_PATH));
		m_strFolderPath.ReleaseBuffer();

		return CFileDialog::OnFileNameOK();
	}
};

#elif defined(__ATLSTR_H__)
#include <commdlg.h>
#define DEF_USE_WQSGFileDialog
/////////////////////////////////////////////////////////////////////////////
// Basic types
// abstract iteration position
/*
//already in afx.h
struct __POSITION {};
typedef __POSITION* POSITION;
*/

class CWQSGFileDialog
{
	CString m_strFolderPath;
public:
	// Attributes
	__declspec(property(get=GetOFN)) OPENFILENAME m_ofn;
	const OPENFILENAME& GetOFN() const
	{
		return *m_pOFN;
	}
	OPENFILENAME& GetOFN()
	{
		return *m_pOFN;
	}
	LPOPENFILENAME m_pOFN;

	// Constructors
	explicit CWQSGFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		HWND pParentWnd = NULL,
		DWORD dwSize = 0,
		BOOL bVistaStyle = TRUE)
		: m_bOpenFileDialog(bOpenFileDialog)
		, m_pOFN(NULL)
	{
		// determine size of OPENFILENAME struct if dwSize is zero
		if (dwSize == 0)
		{
			OSVERSIONINFO vi;
			ZeroMemory(&vi, sizeof(OSVERSIONINFO));
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			::GetVersionEx(&vi);

			// if running under Vista
			if (vi.dwMajorVersion >= 6)
			{
				m_bVistaStyle = bVistaStyle;
			}
			else
			{
				m_bVistaStyle = FALSE;
			}

			dwSize = sizeof(OPENFILENAME);
		}

		m_bVistaStyle = FALSE;

		// size of OPENFILENAME must be at least version 5
		ASSERT(dwSize >= sizeof(OPENFILENAME));
		// allocate memory for OPENFILENAME struct based on size passed in
		m_pOFN = static_cast<LPOPENFILENAME>(malloc(dwSize));
		ASSERT(m_pOFN != NULL);

		ZeroMemory(m_pOFN, sizeof(OPENFILENAME));
		m_szFileName[0] = '\0';
		m_szFileTitle[0] = '\0';

		m_ofn.lStructSize = dwSize;
		m_ofn.hwndOwner = pParentWnd;
		m_ofn.lpstrFile = m_szFileName;
		m_ofn.nMaxFile = _countof(m_szFileName);
		m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
		m_ofn.nMaxFileTitle = _countof(m_szFileTitle);
		m_ofn.lpstrInitialDir = NULL;
		m_ofn.Flags |= dwFlags | /*OFN_ENABLEHOOK |*/ OFN_EXPLORER;
		if(dwFlags & OFN_ENABLETEMPLATE)
			m_ofn.Flags &= ~OFN_ENABLESIZING;

		//m_ofn.lpfnHook = _WQSGCommDlgProc;

		m_ofn.lpstrDefExt = lpszDefExt;
		// setup initial file name
		if (lpszFileName != NULL)
			Checked::tcsncpy_s(m_szFileName, _countof(m_szFileName), lpszFileName, _TRUNCATE);

		// Translate filter into commdlg format (lots of \0)
		if (lpszFilter != NULL)
		{
			m_strFilter = lpszFilter;
			LPTSTR pch = m_strFilter.GetBuffer(0); // modify the buffer in place
			// MFC delimits with '|' not '\0'
			while ((pch = _tcschr(pch, '|')) != NULL)
				*pch++ = '\0';
			m_ofn.lpstrFilter = m_strFilter;
			// do not call ReleaseBuffer() since the string contains '\0' characters
		}
	}

	virtual ~CWQSGFileDialog()
	{
		free(m_pOFN);
	}

	INT_PTR DoModal()
	{
		ASSERT( (m_ofn.Flags & OFN_ENABLEHOOK) == 0 );
		ASSERT(m_ofn.lpfnHook == NULL); // can still be a user hook

		INT_PTR rt = IDCANCEL;
		if( m_bOpenFileDialog )
			rt = GetOpenFileName(m_pOFN)?IDOK:IDCANCEL;
		else
			rt = GetSaveFileName(m_pOFN)?IDOK:IDCANCEL;

		if( rt == IDOK )
		{
			POSITION pos = GetStartPosition( );
			m_strFolderPath = GetNextPathName( pos );

			::PathRemoveFileSpecW(m_strFolderPath.GetBuffer(MAX_PATH));
			m_strFolderPath.ReleaseBuffer();
		}

		return rt;
	}

	// Helpers for parsing file name after successful return
	// or during Overridable callbacks if OFN_EXPLORER is set
	CString GetPathName() const  // return full path and filename
	{
		return CString( m_ofn.lpstrFile );
	}

	CString GetFileName() const
	{
		return CString( m_ofn.lpstrFileTitle );
	}

	CString GetFolderPath() const // return full path
	{
		return m_strFolderPath;
	}

	// Enumerating multiple file selections
	POSITION GetStartPosition() const
	{
		return (POSITION)m_ofn.lpstrFile;
	}
	CString GetNextPathName(POSITION& pos) const
	{
		BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
		TCHAR chDelimiter;
		if (bExplorer)
			chDelimiter = '\0';
		else
			chDelimiter = ' ';

		LPTSTR lpsz = (LPTSTR)pos;
		if (lpsz == m_ofn.lpstrFile) // first time
		{
			if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}

			// find char pos after first Delimiter
			while(*lpsz != chDelimiter && *lpsz != '\0')
				lpsz = _tcsinc(lpsz);
			lpsz = _tcsinc(lpsz);

			// if single selection then return only selection
			if (*lpsz == 0)
			{
				pos = NULL;
				return m_ofn.lpstrFile;
			}
		}

		CString strBasePath = m_ofn.lpstrFile;
		if (!bExplorer)
		{
			LPTSTR lpszPath = m_ofn.lpstrFile;
			while(*lpszPath != chDelimiter)
				lpszPath = _tcsinc(lpszPath);
			strBasePath = strBasePath.Left(int(lpszPath - m_ofn.lpstrFile));
		}

		LPTSTR lpszFileName = lpsz;
		CString strFileName = lpsz;

		// find char pos at next Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);

		if (!bExplorer && *lpsz == '\0')
			pos = NULL;
		else
		{
			if (!bExplorer)
				strFileName = strFileName.Left(int(lpsz - lpszFileName));

			lpsz = _tcsinc(lpsz);
			if (*lpsz == '\0') // if double terminated then done
				pos = NULL;
			else
				pos = (POSITION)lpsz;
		}

		TCHAR strDrive[_MAX_DRIVE], strDir[_MAX_DIR], strName[_MAX_FNAME], strExt[_MAX_EXT];
		Checked::tsplitpath_s(strFileName, strDrive, _MAX_DRIVE, strDir, _MAX_DIR, strName, _MAX_FNAME, strExt, _MAX_EXT);
		TCHAR strPath[_MAX_PATH];
		if (*strDrive || *strDir)
		{
			Checked::tcscpy_s(strPath, _countof(strPath), strFileName);
		}
		else
		{
			Checked::tsplitpath_s(strBasePath+_T("\\"), strDrive, _MAX_DRIVE, strDir, _MAX_DIR, NULL, 0, NULL, 0);
			Checked::tmakepath_s(strPath, _MAX_PATH, strDrive, strDir, strName, strExt);
		}

		return strPath;
	}
protected:

	BOOL m_bVistaStyle;

	BOOL m_bOpenFileDialog;       // TRUE for file open, FALSE for file save
	CString m_strFilter;          // filter string
	// separate fields with '|', terminate with '||\0'
	TCHAR m_szFileTitle[64];       // contains file title after return
	TCHAR m_szFileName[_MAX_PATH]; // contains full path name after return
};

#endif

#ifdef DEF_USE_WQSGFileDialog

class CWQSGFileDialog_Save
{
protected:
	CString m_strFolderPath;
	CString m_strFilter;
	CString m_strDefExt;
	CString m_strFileName;

	CString m_strWindowTitle;
	CWQSGFileDialog* m_pDlg;
public:
	explicit CWQSGFileDialog_Save(LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL
		)
		: m_pDlg( NULL )
		, m_strFilter(lpszFilter)
		, m_strDefExt(lpszDefExt)
		, m_strFileName(lpszFileName)
	{
	}

	virtual ~CWQSGFileDialog_Save()
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	virtual INT_PTR DoModal()
	{
		if( m_pDlg )
		{
			delete m_pDlg;
			m_pDlg = NULL;
		}

		m_pDlg = new CWQSGFileDialog( FALSE , m_strDefExt , m_strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , m_strFilter );

		if( !m_strWindowTitle.IsEmpty() )
			m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

		if( !m_strFolderPath.IsEmpty() )
			m_pDlg->m_ofn.lpstrInitialDir = m_strFolderPath;

		const INT_PTR rt = m_pDlg->DoModal();

		if( rt == IDOK )
		{
			m_strFileName = m_pDlg->GetFileName();
			m_strFolderPath = m_pDlg->GetFolderPath();
		}

		return rt;
	}

	CString GetPathName()const
	{
		return m_pDlg?m_pDlg->GetPathName():CString();
	}

	void SetWindowTitle( LPCTSTR lpszTitle )
	{
		m_strWindowTitle = lpszTitle;
	}
};

class CWQSGFileDialog_Open
{
protected:
	CString m_strFolderPath;
	CString m_strFilter;
	CString m_strFileName;

	CString m_strWindowTitle;
	CWQSGFileDialog* m_pDlg;
public:
	explicit CWQSGFileDialog_Open(LPCTSTR lpszFilter = NULL,
		LPCTSTR lpszFileName = NULL
		)
		: m_pDlg( NULL )
		, m_strFilter(lpszFilter)
		, m_strFileName(lpszFileName)
	{
	}

	virtual ~CWQSGFileDialog_Open()
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}

	INT_PTR DoModal()
	{
		if( m_pDlg )
		{
			delete m_pDlg;
			m_pDlg = NULL;
		}

		m_pDlg = new CWQSGFileDialog( TRUE , NULL , m_strFileName , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , m_strFilter );

		if( !m_strWindowTitle.IsEmpty() )
			m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

		if( !m_strFolderPath.IsEmpty() )
			m_pDlg->m_ofn.lpstrInitialDir = m_strFolderPath;

		const INT_PTR rt = m_pDlg->DoModal();

		if( rt == IDOK )
		{
			m_strFileName = m_pDlg->GetFileName();
			m_strFolderPath = m_pDlg->GetFolderPath();
		}

		return rt;
	}

	CString GetPathName()const
	{
		return m_pDlg?m_pDlg->GetPathName():CString();
	}

	CString GetFolderPath()const
	{
		return m_strFolderPath;
	}

	void SetWindowTitle( LPCTSTR lpszTitle )
	{
		m_strWindowTitle = lpszTitle;
	}
};

class CWQSGFileDialog_OpenS
{
protected:
	CString m_strFolderPath;
	CString m_strFilter;

	CString m_strWindowTitle;
	CWQSGFileDialog* m_pDlg;
	CString m_strNameBuffer;
public:
	explicit CWQSGFileDialog_OpenS(LPCTSTR lpszFilter = NULL)
		: m_pDlg( NULL )
		, m_strFilter(lpszFilter)
	{
	}

	virtual ~CWQSGFileDialog_OpenS()
	{
		delete m_pDlg;
		m_pDlg = NULL;

		m_strNameBuffer.ReleaseBuffer();
	}

	INT_PTR DoModal()
	{
		if( m_pDlg )
		{
			delete m_pDlg;
			m_pDlg = NULL;
		}

		m_pDlg = new CWQSGFileDialog( TRUE , NULL , _T("") , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT , m_strFilter );
		m_strNameBuffer = L"";
		m_pDlg->m_ofn.lpstrFile = m_strNameBuffer.GetBuffer(65536);
		m_pDlg->m_ofn.lpstrFile[0] = 0;
		m_pDlg->m_ofn.nMaxFile = 65535;

		if( !m_strWindowTitle.IsEmpty() )
			m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

		if( !m_strFolderPath.IsEmpty() )
			m_pDlg->m_ofn.lpstrInitialDir = m_strFolderPath;

		const INT_PTR rt = m_pDlg->DoModal();

		if( rt == IDOK )
			m_strFolderPath = m_pDlg->GetFolderPath();

		return rt;
	}

	POSITION GetStartPosition( ) const
	{
		if( m_pDlg )
			return m_pDlg->GetStartPosition();

		return NULL;
	}

	BOOL GetNextPathName( CString& a_strPathName , POSITION& pos) const
	{
		if( m_pDlg && pos )
		{
			a_strPathName = m_pDlg->GetNextPathName( pos );
			return TRUE;
		}
		return FALSE;
	}

	void SetWindowTitle( LPCTSTR lpszTitle )
	{
		m_strWindowTitle = lpszTitle;
	}
};
#endif//DEF_USE_WQSGFileDialog

#endif//__WQSG_DIRDLG_H__

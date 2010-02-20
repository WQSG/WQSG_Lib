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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
//#include <atlstr.h>

#define __WQSG_DIRBOX_SYSTEM_ 1
#if !__WQSG_DIRBOX_SYSTEM_
#include<afxcmn.h>
#include<afxwin.h>
#include"WQSG_resource.h"
class CWQSG_DirDlg : public CDialog
{
	DECLARE_DYNAMIC(CWQSG_DirDlg)
    CString m_path ,
        m_title;

    ::CListCtrl* m_List;
    ::CComboBox* m_Drives;
    void 列出目录( );
    CString m_EditPath;
public:
	CWQSG_DirDlg(const WCHAR* title = NULL,
        const WCHAR* path = NULL,
        CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWQSG_DirDlg();

// 对话框数据
	enum { IDD = IDD_WQSG_DIRDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnLvnItemActivateWqsgDirdlgList(NMHDR *pNMHDR, LRESULT *pResult);
    void 上一级();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnNMClickWqsgDirdlgList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnSelchangeWqsgDirlistCombo1();
    afx_msg void OnBnClickedOk();
    CString GetPath(void);
};
#else
#include <WQSG.h>
#include <shtypes.h>
#include <shlobj.h>
class CWQSG_DirDlg
{
	HWND m_hWnd;
	WCHAR m_title[ MAX_PATH ];
	WCHAR m_path[ MAX_PATH ];
public:
	CWQSG_DirDlg(
		HWND hWnd ,
		const WCHAR* title = NULL ,
		const WCHAR* path = NULL
		)
		: m_hWnd( hWnd )
		//, m_title( title )
		//, m_path( path )
	{
		if( title )
			WQSG_strcpy( title , m_title );
		else
			WQSG_strcpy( L"请选择一个文件夹" , m_title );

		if( path )
			WQSG_strcpy( path , m_path );
	}

	virtual ~CWQSG_DirDlg(){}
	BOOL GetPath( WCHAR* path )
	{
		LPITEMIDLIST idl = NULL;
		BROWSEINFO bi = {};

		bi.hwndOwner = m_hWnd;
		bi.pidlRoot = idl;
		bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;

		LPMALLOC pMalloc = NULL;
		SHGetMalloc( &pMalloc );
		SHGetFolderLocation( m_hWnd, 1, 0, 0, &idl );

		bi.lpszTitle = m_title;
		LPITEMIDLIST ret = SHBrowseForFolder( &bi );
		ret->mkid;
		if( idl )
			pMalloc->Free(idl);

		SHGetPathFromIDList( ret , path );
		if( ret )
			pMalloc->Free( ret );
		pMalloc->Release();

		return ( *path );
	}
};
#endif//__WQSG_DIRBOX_SYSTEM_
#ifdef __AFXDLGS_H__

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

	virtual BOOL OnFileNameOK()
	{
		POSITION pos = GetStartPosition( );
		m_strFolderPath = GetNextPathName( pos );

		::PathRemoveFileSpecW(m_strFolderPath.GetBuffer());
		m_strFolderPath.ReleaseBuffer();

		return CFileDialog::OnFileNameOK();
	}
};

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
		m_strNameBuffer.GetBuffer(65536);
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
		m_pDlg->m_ofn.lpstrFile = m_strNameBuffer.GetBuffer();
		m_pDlg->m_ofn.nMaxFile = 65535;

		if( !m_strWindowTitle.IsEmpty() )
			m_pDlg->m_ofn.lpstrTitle = m_strWindowTitle;

		const INT_PTR rt = m_pDlg->DoModal();

		if( rt == IDOK )
		{
			m_strFolderPath = m_pDlg->GetFolderPath();
			m_strFolderPath += L'\\';
		}

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

#endif//__AFXDLGS_H__

#endif//__WQSG_DIRDLG_H__

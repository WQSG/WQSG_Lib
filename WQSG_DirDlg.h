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
	CWQSG_DirDlg(WCHAR* title = NULL,
        WCHAR* path = NULL,
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
class CWQSG_DirDlg
{
	HWND		m_hWnd;
	WCHAR		m_title[ MAX_PATH ];
	WCHAR		m_path[ MAX_PATH ];
public:
	CWQSG_DirDlg(
		HWND hWnd ,
		WCHAR* title = NULL ,
		WCHAR* path = NULL
		);
	virtual ~CWQSG_DirDlg(){}
	BOOL GetPath( WCHAR* path );
};
#endif
#endif

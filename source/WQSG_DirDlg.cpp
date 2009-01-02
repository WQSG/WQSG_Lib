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
//#include <stdafx.h>
#include "WQSG_DirDlg.h"
#include <shlobj.h>
#include "WQSG_String.h"
#if !__WQSG_DIRBOX_SYSTEM_
IMPLEMENT_DYNAMIC(CWQSG_DirDlg, CDialog)
CWQSG_DirDlg::CWQSG_DirDlg(WCHAR* title /*= NULL*/,
        WCHAR* path /*= NULL*/,
        CWnd* pParent /*=NULL*/)
	: CDialog(CWQSG_DirDlg::IDD, pParent)
    , m_List(NULL)
    , m_Drives(NULL)
    , m_EditPath(_T(""))
{
    if(title)   m_title = title;
    else        m_title = L"ѡ��һ���ļ���";

    if(path)   m_path = path;
}
CWQSG_DirDlg::~CWQSG_DirDlg()
{
}
void CWQSG_DirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_WQSG_DIRLIST_EDIT_PATH, m_EditPath );
    DDV_MaxChars(pDX, m_EditPath , MAX_PATH - 2 );
}
BEGIN_MESSAGE_MAP(CWQSG_DirDlg, CDialog)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_WQSG_DIRDLG_LIST, &CWQSG_DirDlg::OnLvnItemActivateWqsgDirdlgList)
    ON_BN_CLICKED(IDC_UP, &CWQSG_DirDlg::OnBnClickedButton1)
    ON_NOTIFY(NM_CLICK, IDC_WQSG_DIRDLG_LIST, &CWQSG_DirDlg::OnNMClickWqsgDirdlgList)
    ON_CBN_SELCHANGE(IDC_WQSG_DIRLIST_COMBO1, &CWQSG_DirDlg::OnCbnSelchangeWqsgDirlistCombo1)
    ON_BN_CLICKED(ID_OK, &CWQSG_DirDlg::OnBnClickedOk)
END_MESSAGE_MAP()
void CWQSG_DirDlg::OnCancel(){CDialog::OnCancel();}
void CWQSG_DirDlg::OnOK(){}
BOOL CWQSG_DirDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    SetWindowTextW ( m_title );

    m_List = (::CListCtrl*)GetDlgItem(IDC_WQSG_DIRDLG_LIST);
    if( NULL == m_List ){
        WQSG_DEBUG_MESSAGEBOX(L"��ȡDirListָ��ʧ��");
        CDialog::OnCancel();
        return FALSE;
    }


    m_Drives = (CComboBox*)GetDlgItem(IDC_WQSG_DIRLIST_COMBO1);
    if( NULL == m_Drives ){
        WQSG_DEBUG_MESSAGEBOX(L"��ȡ �������� ָ��ʧ��");
        CDialog::OnCancel();
        return FALSE;
    }

    CRect rect;
    m_List->GetClientRect( rect );

    m_List->InsertColumn( 0 ,L"Ŀ¼" , LVCFMT_LEFT , rect.right - rect.left );

    m_List->SetExtendedStyle(m_List->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

    if( !::WQSG_IsDir( m_path.GetBuffer() ) ){
        WCHAR path[ MAX_PATH ];
        GetCurrentDirectoryW( MAX_PATH , path );
        m_path = path;
    }

    DWORD dv = ::GetLogicalDrives();
	DWORD dmask = 1;
	int cul = -1;
	for(int i = 0;i < 26;i++,(dmask <<= 1)){
		if( dmask & dv){
			cul++;
			CString str;
			str.Format(L"%C:",i + 'A');
			m_Drives->AddString ( str );
			if( ( str != L"A:" ) &
				( str != L"B:" ) )
			{
                CString dch(m_path.Left(2));
				if( dch.MakeUpper() == str )
                    m_Drives->SetCurSel( cul );
			}
		}
	}

    �г�Ŀ¼( );

    return TRUE;
}
void CWQSG_DirDlg::�г�Ŀ¼(  )
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    m_List->DeleteAllItems();

    hFind = FindFirstFileW( m_path + L"\\*.", &FindFileData );
    if (hFind != INVALID_HANDLE_VALUE)
    {
        m_List->SetRedraw(FALSE);
        do{
            if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
                //Ŀ¼
                CString �ļ���( FindFileData.cFileName );
                if( ( �ļ��� != L"." ) && ( �ļ��� != L".." ) ){
                    m_List->InsertItem( 0 , FindFileData.cFileName );
                }
            }
        }while(::FindNextFileW( hFind ,& FindFileData ));
        ::FindClose(hFind);
        m_List->SetRedraw(TRUE);

        m_EditPath = m_path;
        UpdateData( FALSE );
    }
    else{
        DWORD err = ::GetLastError();
        switch( err ){
                case ERROR_FILE_NOT_FOUND:
                    return ;
                case ERROR_PATH_NOT_FOUND:
                    MessageBox( m_path + L"\r\nĿ¼������");
                    break;
                case ERROR_NOT_READY:
                    MessageBox( m_path + L"\r\n�豸δ����");
                    return;
                default:
                    MessageBox(L"δ֪����");
        }
    }
}
void CWQSG_DirDlg::OnLvnItemActivateWqsgDirdlgList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString str;
    int i = (int)m_List->GetFirstSelectedItemPosition();

    if( i > 0 ){
        str = m_List->GetItemText( i - 1 , 0 );
        if( str == L".."){
            ��һ��( );
        }
        else{
            m_path = m_path + L'\\' + str;
            �г�Ŀ¼( );
        }
    }

    *pResult = 0;
}
void CWQSG_DirDlg::��һ��()
{
    CString str = m_path.Left ( m_path.ReverseFind( L'\\' ) );
    if(str.GetLength() >= 2){
        m_path = str;
        �г�Ŀ¼( );
    }
}
void CWQSG_DirDlg::OnBnClickedButton1()
{
    ��һ��();
}
void CWQSG_DirDlg::OnNMClickWqsgDirdlgList(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString str;
    int i = (int)m_List->GetFirstSelectedItemPosition();

    if( i > 0 ){
        str = m_List->GetItemText( i - 1 , 0 );
        if( str == L".."){
        }
        else{
            m_EditPath = m_path + L'\\' + str;
        }
    }
    UpdateData(FALSE);

    *pResult = 0;
}

void CWQSG_DirDlg::OnCbnSelchangeWqsgDirlistCombo1()
{
    int sel = m_Drives->GetCurSel();
    if(sel >= 0){
        CString str;
        m_Drives->GetLBText( sel , str );
        m_path = str;
        �г�Ŀ¼();
    }
}

void CWQSG_DirDlg::OnBnClickedOk()
{
    UpdateData();

    CString str;
    int i = (int)m_List->GetFirstSelectedItemPosition();
    if(i > 0){
        str = m_List->GetItemText( i - 1 , 0 );
        if( str == L".."){
        }
        else{
            m_EditPath = m_path + L'\\' + str;
        }
    }
    else{
        m_EditPath = m_path;
    }
    UpdateData(FALSE);

    CDialog::OnOK();
}

CString CWQSG_DirDlg::GetPath(void)
{
    return CString( m_EditPath );
}

#else
CWQSG_DirDlg::CWQSG_DirDlg(
						   HWND hWnd
						   , WCHAR* title //= L"��ѡ��һ���ļ���",
						   , WCHAR* path //=NULL
						   )
						   : m_hWnd( hWnd )
						   //, m_title( title )
						   //, m_path( path )
{
	if( title )
		WQSG_strcpy( title , m_title );
	else
		WQSG_strcpy( L"��ѡ��һ���ļ���" , m_title );

	if( path )
		WQSG_strcpy( path , m_path );
}
//-------------------------------------------------------------------------
BOOL CWQSG_DirDlg::GetPath( WCHAR* path )
{
	LPITEMIDLIST idl = NULL;
	BROWSEINFO bi;	memset( &bi , 0 , sizeof(bi) );

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
#endif

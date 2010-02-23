// CAbout.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"

#include <stdio.h>

#include <WQSG_Lib.h>


int main()
{
	const HRSRC hRes = FindResource( NULL , MAKEINTRESOURCE(IDD_ABOUTBOX) , RT_DIALOG );
	if( hRes == NULL )
		return -1;

	const DWORD dwSize = SizeofResource( NULL , hRes );

	const HGLOBAL hResource = LoadResource( NULL , hRes );
	if( hResource == NULL )
		return -1;

	const LPBYTE pBuf = (LPBYTE)LockResource( hResource );
	if( pBuf == NULL )
	{
		FreeResource( hResource );
		return -1;
	}

	CWQSG_memFile fp;

	WQSG_Bin2c_8Bit( fp , pBuf , dwSize , "gs_Dlg" );

	UnlockResource( hResource );
	FreeResource( hResource );

	{
		SWQSG_AboutDlgID ids;

		ids.m_nAppName = IDC_APP_NAME;
		ids.m_nIcon = IDC_LOGO;
		ids.m_nEdit = IDC_EDIT1;
		ids.m_nText0 = IDC_STATIC1;
		ids.m_nLink = IDC_SYSLINK1;

		WQSG_Bin2c_8Bit( fp , &ids , sizeof(ids) , "gs_Ids" );
	}

	CWQSG_File out;
	if( out.OpenFile( L"CAbout_data.h" , 4 , 3 ) )
	{
		if( (u32)fp.GetFileSize() == out.Write( fp.GetBuf() , (u32)fp.GetFileSize() ) )
		{
			return 0;
		}
	}

	return -1;
}

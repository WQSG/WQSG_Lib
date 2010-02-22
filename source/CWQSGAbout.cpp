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
#pragma once

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <objbase.h>
#include "../CWQSGAbout.h"

static HWND g_hWndParent = NULL;
static HICON g_hIcon = NULL;
static const WCHAR* g_pTitle = NULL;
static const WCHAR* g_pAppName = NULL;
static const WCHAR* g_pUrl = NULL;
static const WCHAR* g_pProgrammed = NULL;

static unsigned char g_ids[] = {
	0xe8, 0x03, 0x00, 0x00, 0xeb, 0x03, 0x00, 0x00, 0xe9, 0x03, 0x00, 0x00, 0xea, 0x03, 0x00, 0x00, 
};

static unsigned char g_dlg[] = {
	0x01, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0xc8, 0x80, 
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x00, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x51, 
	0x8e, 0x4e, 0x20, 0x00, 0x43, 0x00, 0x41, 0x00, 0x62, 0x00, 0x6f, 0x00, 0x75, 0x00, 0x74, 0x00, 
	0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4d, 0x00, 0x53, 0x00, 0x20, 0x00, 0x53, 0x00, 
	0x68, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6c, 0x00, 0x67, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x50, 
	0x39, 0x00, 0x07, 0x00, 0x14, 0x00, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0xff, 0xff, 0x82, 0x00, 
	0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x50, 0x07, 0x00, 0x21, 0x00, 0xcd, 0x00, 0x29, 0x00, 0xe9, 0x03, 0x00, 0x00, 
	0x53, 0x00, 0x79, 0x00, 0x73, 0x00, 0x4c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x6b, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x08, 0x81, 0x50, 
	0x07, 0x00, 0x5a, 0x00, 0xcd, 0x00, 0x2e, 0x00, 0xea, 0x03, 0x00, 0x00, 0xff, 0xff, 0x81, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 
	0x51, 0x00, 0x07, 0x00, 0x65, 0x00, 0x13, 0x00, 0xeb, 0x03, 0x00, 0x00, 0xff, 0xff, 0x82, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x50, 
	0x07, 0x00, 0x4f, 0x00, 0xcd, 0x00, 0x08, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x82, 0x00, 
	0x50, 0x00, 0x72, 0x00, 0x6f, 0x00, 0x67, 0x00, 0x72, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x6d, 0x00, 
	0x65, 0x00, 0x64, 0x00, 0x20, 0x00, 0x62, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static const SWQSG_AboutDlgID& s_ids = *(const SWQSG_AboutDlgID*)g_ids;

static inline int Max( int a , int b )
{
	return a>b?a:b;
}

class CWndSize
{
	HWND m_hWnd;
	SIZE m_size;
public:
	CWndSize( HWND a_hWnd )
		: m_hWnd(a_hWnd)
	{
		RECT rect = {};
		GetWindowRect( m_hWnd , &rect );

		m_size.cx = rect.right - rect.left;
		m_size.cy = rect.bottom - rect.top;
	}

	HWND GetHWND()const
	{
		return m_hWnd;
	}

	const SIZE& GetSize()const
	{
		return m_size;
	}

	void Set( int a_PosX , int a_PosY , int a_SizeW , int a_SizeH )
	{
		MoveWindow( m_hWnd , a_PosX , a_PosY , a_SizeW , a_SizeH , TRUE );
		m_size.cx = a_SizeW;
		m_size.cy = a_SizeH;
	}

	void SetC_X( int a_HostW , int a_PosY , int a_SizeMaxW , int a_SizeH )
	{
		int x = a_HostW - 8*2;
		if( a_SizeMaxW > x )
			a_SizeMaxW = x;

		x = (x - a_SizeMaxW) / 2 + 8;

		MoveWindow( m_hWnd , x , a_PosY , a_SizeMaxW , a_SizeH , TRUE );
		m_size.cx = a_SizeMaxW;
		m_size.cy = a_SizeH;
	}
};

static inline INT_PTR CALLBACK AboutFunc(HWND a_hDlg, UINT a_message, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_message)
	{
	case WM_INITDIALOG:
		SendMessage( a_hDlg , WM_SETICON , TRUE , (LPARAM)g_hIcon );
		SendMessage( a_hDlg , WM_SETICON , FALSE , (LPARAM)g_hIcon );

		SetWindowText( a_hDlg , g_pTitle );

		SendMessage( GetDlgItem( a_hDlg , s_ids.m_nIcon ) , STM_SETICON, (WPARAM)g_hIcon , 0L);
		SetDlgItemText( a_hDlg , s_ids.m_nAppName , g_pAppName );

		SetDlgItemText( a_hDlg , s_ids.m_nLink , g_pUrl );

		SetDlgItemText( a_hDlg , s_ids.m_nEdit , g_pProgrammed );

		{
			RECT rcTmp = {};

			GetWindowRect( a_hDlg , &rcTmp );
			RECT rcMain1 = rcTmp;

			GetClientRect( a_hDlg , &rcTmp );

			SIZE szClientSize = { rcTmp.right , rcTmp.bottom };

			const SIZE szMainB = {
				(rcMain1.right - rcMain1.left) - rcTmp.right,
				(rcMain1.bottom - rcMain1.top) - rcTmp.bottom
			};
			//-----------------------------------------
			CWndSize wIcon( GetDlgItem( a_hDlg , s_ids.m_nIcon ) );
			CWndSize wAppName( GetDlgItem( a_hDlg , s_ids.m_nAppName ) );
			CWndSize wLink( GetDlgItem( a_hDlg , s_ids.m_nLink ) );
			CWndSize wProgrammed( GetDlgItem( a_hDlg , s_ids.m_nEdit ) );

			HWND hP2 = GetWindow( a_hDlg , GW_CHILD );
			while ( hP2 && GetDlgCtrlID(hP2) != -1 )
				hP2 = GetWindow( hP2 , GW_HWNDNEXT );

			CWndSize wP2( hP2 );
			//--------------------------------------------------
			int nMaxHeight_Link = 0;
			int nMaxWidth_Link = 0;

			if( wLink.GetHWND() )
			{
				nMaxHeight_Link = (int)::SendMessage( wLink.GetHWND() , LM_GETIDEALHEIGHT , 0 , 0 );

				nMaxWidth_Link = 0;
				int nHeight1;
				int nHeight2;

				SIZE size = {};
				do
				{
					nMaxWidth_Link++;
					nHeight1 = (int)::SendMessage( wLink.GetHWND() , LM_GETIDEALSIZE , (WPARAM)nMaxWidth_Link     , (LPARAM)&size );
					nHeight2 = (int)::SendMessage( wLink.GetHWND() , LM_GETIDEALSIZE , (WPARAM)nMaxWidth_Link + 2 , (LPARAM)&size );
				}
				while ( nMaxHeight_Link != nHeight1 || nMaxHeight_Link != nHeight2 );
			}

			if( nMaxWidth_Link < (szClientSize.cx - 8*2) )
			{
				//nMaxWidth_Link = (szClientSize.cx - 8*2);
			}
			else
				szClientSize.cx = nMaxWidth_Link + 8*2;

			szClientSize.cy =
				8 + wIcon.GetSize().cy +
				2 + wAppName.GetSize().cy +
				16 + nMaxHeight_Link +
				16 + wP2.GetSize().cy +
				2 + wProgrammed.GetSize().cy +
				8;

			MoveWindow( a_hDlg , rcMain1.left , rcMain1.top , szClientSize.cx + szMainB.cx , szClientSize.cy + szMainB.cy , TRUE );
			//
			int posY = 8;

			wIcon.SetC_X( szClientSize.cx , posY , wIcon.GetSize().cx , wIcon.GetSize().cy );
			posY += (2 + wIcon.GetSize().cy);

			SetWindowLong( wAppName.GetHWND() , GWL_STYLE ,  GetWindowLong( wAppName.GetHWND() , GWL_STYLE ) | SS_CENTER );
			wAppName.SetC_X( szClientSize.cx , posY , szClientSize.cx , wAppName.GetSize().cy );
			posY += (16 + wAppName.GetSize().cy);

			wLink.SetC_X( szClientSize.cx , posY , nMaxWidth_Link , nMaxHeight_Link );
			posY += (16 + wLink.GetSize().cy);

			SetWindowLong( wP2.GetHWND() , GWL_STYLE ,  GetWindowLong( wP2.GetHWND() , GWL_STYLE ) | SS_CENTER );
			wP2.SetC_X( szClientSize.cx , posY , szClientSize.cx , wP2.GetSize().cy );
			posY += (2 + wP2.GetSize().cy);

			wProgrammed.SetC_X( szClientSize.cx , posY , szClientSize.cx , wProgrammed.GetSize().cy );
			posY += (2 + wProgrammed.GetSize().cy);
		}

		return (INT_PTR)TRUE;
	case WM_NOTIFY:
		{
			const LPNMHDR pNMHdr = (LPNMHDR)a_lParam;
			switch (pNMHdr->code)
			{
				//case NM_RETURN:
			case NM_CLICK:
				{
					const PNMLINK pNMLink = (PNMLINK)a_lParam;

					if( *pNMLink->item.szUrl )
						ShellExecute( NULL , L"open" , pNMLink->item.szUrl , NULL , NULL , SW_SHOW );
				}
				break;
			};
		}
	case WM_COMMAND:
		if (LOWORD(a_wParam) == IDOK || LOWORD(a_wParam) == IDCANCEL)
		{
			EndDialog( a_hDlg , LOWORD(a_wParam) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR WQSG_About( HICON a_hIcon , HWND a_hWndParent ,
				   const WCHAR* a_pTitle , const WCHAR* a_pAppName ,
				   const WCHAR* a_pUrl , const WCHAR* a_pProgrammed )
{
	g_hIcon = a_hIcon;
	g_hWndParent = a_hWndParent;
	g_pTitle = a_pTitle;
	g_pAppName = a_pAppName;
	g_pUrl = a_pUrl;
	g_pProgrammed = a_pProgrammed;

	if( sizeof(SWQSG_AboutDlgID) != sizeof(g_ids) )
	{
		__asm int 3;
		return IDCANCEL;
	}

	return DialogBoxIndirect( NULL , (LPDLGTEMPLATE)g_dlg , g_hWndParent , AboutFunc );
}

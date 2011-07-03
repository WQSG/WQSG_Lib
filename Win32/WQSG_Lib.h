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
#ifndef __WQSG_LIB_H__
#define	__WQSG_LIB_H__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
//#include <afxwin.h> //基础库不能有MFC相关的
#include <crtdbg.h>
#include <tchar.h>
#include <crtdbg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(WIN32) && !defined(ASSERT)
#define ASSERT( x ) do{if( !(x) ) _asm int 3}while(0)
#endif

#include "../Interface/wqsg.h"
#include "../Common/wqsg_xfile.h"
#include "../Common/wqsg_string.h"
#include "../Common/cwd_bin.h"
#include "../Common/cwqsg_str.h"

#include "WQSG_File.h"
#include "WQSG_File_M.h"
#include "./Dlg/WQSG_DirDlg.h"
#include "./Dlg/About/CWQSGAbout.h"

#ifdef DEBUG
#define WQSG_DEBUG_MESSAGEBOX(str) MessageBox(str);
#else
#define WQSG_DEBUG_MESSAGEBOX(str)
#endif
//------------------------------------------------------------------
inline BOOL WQSG_LoadResA( const WORD resID , char const*const resType , CWD_BIN& wd_bin )
{
	if( NULL == resType )
		return FALSE;

	if( NULL != wd_bin.BIN )
		return FALSE;

	HINSTANCE h = NULL ; //::AfxGetInstanceHandle();
	const HRSRC hrRes = ::FindResourceA( h , MAKEINTRESOURCEA( resID ) , resType );
	if( NULL == hrRes )
		return FALSE;

	const DWORD size = ::SizeofResource( h , hrRes );
	wd_bin.LEN = size;
	if( ( size != (DWORD)wd_bin.LEN ) || ( wd_bin.LEN < 0 ) )
		return FALSE;

	const HGLOBAL hgdRes = ::LoadResource( h , hrRes );
	if( NULL == hgdRes )
		return FALSE;

	LPVOID lp_void = ::LockResource( hgdRes );
	if( NULL == lp_void )
		return FALSE;

	if( NULL == wd_bin.SetBufSize( wd_bin.LEN ) )
	{
		UnlockResource( lp_void );
		return FALSE;
	}

	memcpy( wd_bin.BIN , lp_void , wd_bin.LEN );

	UnlockResource( lp_void );

	return TRUE;
}
inline BOOL WQSG_LoadResW( const WORD resID , WCHAR const*const resType , CWD_BIN& wd_bin )
{
	if( NULL == resType )
		return FALSE;

	if( NULL != wd_bin.BIN )
		return FALSE;

	HINSTANCE h = NULL ; //::AfxGetInstanceHandle();
	const HRSRC hrRes = ::FindResourceW( h , MAKEINTRESOURCEW( resID ) , resType );
	if( NULL == hrRes )
		return FALSE;

	const DWORD size = ::SizeofResource( h , hrRes );
	wd_bin.LEN = size;
	if( ( size != (DWORD)wd_bin.LEN ) || ( wd_bin.LEN < 0 ) )
		return FALSE;

	const HGLOBAL hgdRes = ::LoadResource( h , hrRes );
	if( NULL == hgdRes )
		return FALSE;

	LPVOID lp_void = ::LockResource( hgdRes );
	if( NULL == lp_void )
		return FALSE;

	if( NULL == wd_bin.SetBufSize( wd_bin.LEN ) )
	{
		UnlockResource( lp_void );
		return FALSE;
	}

	memcpy( wd_bin.BIN , lp_void , wd_bin.LEN );

	UnlockResource( lp_void );

	return TRUE;
}
#ifdef UNICODE
#define WQSG_LoadRes WQSG_LoadResW
#else
#define WQSG_LoadRes WQSG_LoadResA
#endif
//------------------------------------------------------------------
class CWQSG_MSG_W
{
	HWND m_hWnd;
	LPCWSTR m_pTitle;
public:
	CWQSG_MSG_W( HWND a_hWnd , LPCWSTR a_pTitle )
		: m_hWnd(a_hWnd) , m_pTitle (a_pTitle)
	{
	}

	~CWQSG_MSG_W(){}

	INT show(LPCWSTR TXT1,UINT a_uType = 0)
	{
		return ::MessageBoxW(m_hWnd,TXT1,m_pTitle,a_uType);
	}

	INT show(LPCWSTR TXT1,LPCWSTR TXT2,UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + 3;
		WCHAR *const SHOWTXT = new WCHAR[len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		len = ::MessageBoxW(m_hWnd,SHOWTXT,m_pTitle,a_uType);
		delete[]SHOWTXT;
		return (INT)len;
	}

	INT show(LPCWSTR TXT1,LPCWSTR TXT2,LPCWSTR TXT3,UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + ::WQSG_strlen((WCHAR*)TXT3) + 5;
		WCHAR *const SHOWTXT = new WCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT3,s1);
		len = ::MessageBoxW(m_hWnd,SHOWTXT,m_pTitle,a_uType);
		delete[]SHOWTXT;
		return (INT)len;
	}

	INT show(LPCWSTR TXT1,LPCWSTR TXT2,LPCWSTR TXT3,LPCWSTR TXT4,UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + ::WQSG_strlen((WCHAR*)TXT3) + ::WQSG_strlen((WCHAR*)TXT4) + 7;
		WCHAR *const SHOWTXT = new WCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT3,s1);
		s1 += ::WQSG_strcpy(L"\r\n",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT4,s1);
		len = ::MessageBoxW(m_hWnd,SHOWTXT,m_pTitle,a_uType);
		delete[]SHOWTXT;
		return (INT)len;
	}
	void SetTitle(LPCWSTR a_pTitle){m_pTitle = a_pTitle;}
	void SetHwmd(HWND a_hWnd){m_hWnd = a_hWnd;}
};
class CWQSG_MSG_A
{
	HWND m_hWnd;
	LPCSTR m_pTitle;
public:
	CWQSG_MSG_A( HWND a_hWnd , LPCSTR a_pTitle )
		: m_hWnd(a_hWnd) , m_pTitle (a_pTitle)
	{
	}
	~CWQSG_MSG_A(){}

	INT show( LPCSTR TXT1 , UINT a_uType = 0 )
	{
		return ::MessageBoxA( m_hWnd , TXT1 , m_pTitle , a_uType );
	}
	INT show(LPCSTR TXT1 , LPCSTR TXT2 , UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen( TXT1 ) + ::WQSG_strlen( TXT2 ) + 3;
		CHAR*const SHOWTXT = new CHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy(TXT1,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy(TXT2,s1);
		len = ::MessageBoxA( m_hWnd , SHOWTXT , m_pTitle , a_uType );
		delete[]SHOWTXT;
		return (INT)len;
	}
	INT show( LPCSTR TXT1 , LPCSTR TXT2 , LPCSTR TXT3 , UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen(TXT1) + ::WQSG_strlen( TXT2 ) + ::WQSG_strlen( TXT3 ) + 5;
		CHAR *const SHOWTXT = new CHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy( TXT1,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy( TXT2,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy( TXT3,s1);
		len = ::MessageBoxA( m_hWnd , SHOWTXT , m_pTitle , a_uType );
		delete[]SHOWTXT;
		return (INT)len;
	}
	INT show( LPCSTR TXT1 , LPCSTR TXT2 , LPCSTR TXT3 , LPCSTR TXT4 , UINT a_uType = 0)
	{
		UINT len = ::WQSG_strlen(TXT1) + ::WQSG_strlen(TXT2) + ::WQSG_strlen(TXT3) + ::WQSG_strlen(TXT4) + 7;
		CHAR *const SHOWTXT = new CHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy(TXT1,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy(TXT2,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy(TXT3,s1);
		s1 += ::WQSG_strcpy("\r\n",s1);
		s1 += ::WQSG_strcpy(TXT4,s1);
		len = ::MessageBoxA( m_hWnd , SHOWTXT , m_pTitle , a_uType );
		delete[]SHOWTXT;
		return (INT)len;
	}
	void SetTitle(LPCSTR a_pTitle){m_pTitle = a_pTitle;}
	void SetHwmd(HWND a_hWnd){m_hWnd = a_hWnd;}
};
//------------------------------------------------------------------
class CWQSG_CriticalSection
{
	CRITICAL_SECTION	m_cs_mutex;
public:
	inline	CWQSG_CriticalSection()
	{
		::InitializeCriticalSectionAndSpinCount( &m_cs_mutex , 100 );
	}
	inline	virtual ~CWQSG_CriticalSection()
	{
		::DeleteCriticalSection( &m_cs_mutex );
	}
	//-------------------------------------------------
	inline	void Lock()
	{
		::EnterCriticalSection( &m_cs_mutex );
	}
	inline	void UnLock()
	{
		::LeaveCriticalSection( &m_cs_mutex );
	}
	inline	BOOL Try_Lock()
	{
		return TryEnterCriticalSection( &m_cs_mutex );
	}
};
//------------------------------------------------------------------
class CWQSG_Mutex
{
	HANDLE m_Mutex;
public:
	CWQSG_Mutex() : m_Mutex(NULL){}
	virtual~CWQSG_Mutex(){	Destroy();	}
	//-----------------------------------------
	inline	BOOL Create()
	{
		if( m_Mutex != NULL )
			return TRUE;

		m_Mutex = ::CreateMutex( NULL , TRUE , NULL );
		return ( m_Mutex != NULL );
	}
	inline	void Destroy()
	{
		HANDLE handle = m_Mutex;
		m_Mutex = NULL;
		if( handle != NULL )
			::CloseHandle( handle );
	}
	inline	void UnLock()
	{
		::ReleaseMutex( m_Mutex );
	}
	inline	void Lock()
	{
		::WaitForSingleObject( m_Mutex , INFINITE );
	}
};
//------------------------------------------------------------------
//#include "ISO/WQSG_UMD.h"
//#include "ISO/WQSG_PsxISO.h"

#endif //__WQSG_LIB_H__

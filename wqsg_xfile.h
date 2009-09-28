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
#ifndef __WQSG_xFile_H__
#define __WQSG_xFile_H__
#include "WQSG_DEF.h"
#include<windows.h>
#include<crtdbg.h>
#include<TCHAR.h>

#include "WQSG_CxFile.h"
#include "WQSG_String.h"
/**************************************************************************
**************************************************************************/
enum WQSG_enCP
{
	en_CP_NULL			= 0	,
	en_CP_ANSI			= 1	,
	en_CP_UNICODE		= 2	,
	en_CP_UNICODE_BIG	= 3	,
	en_CP_UTF8			= 4	,
};
class _m_CRC32
{
	u32	m_CRC32_seed;
public:
#if 1
	_m_CRC32()	: m_CRC32_seed((u32)0xFFFFFFFF){}
#else
	_m_CRC32()	: m_CRC32_seed((u32)0xFFFFFFFF)
	{
		U32 * pcrc = m_crc32_table;
		for (INT i = 0; i < 256; i++,pcrc++)
		{
			(*pcrc) = i;
			for (INT j = 0; j < 8; j++)
			{
				if ((*pcrc) & 1)
					(*pcrc) = ((*pcrc) >> 1) ^ (U32)0xEDB88320;
				else
					(*pcrc) >>= 1;
			}
		}
	}
#endif
	virtual	~_m_CRC32(){}
	inline	void NEW_CRC32(){	m_CRC32_seed = (u32)0xFFFFFFFF;	}
	inline	u32 GetCRC32( u8 const*const pbuf , u32 buflen );
};
//------------------------------------------
class CWQSG_xFile
{
protected:
	CWQSG_xFile(){}
	virtual	~CWQSG_xFile(){}
public:
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len )			= 0;
	__i__	virtual	u32			Write		( void const*const lpBuffre , const u32 len )	= 0;
	__i__	virtual	void		Close		( void )						= 0;
	__i__	virtual	BOOL		WriteStrW	( WCHAR const*const str );		//
	__i__	virtual	s64			GetFileSize	( void )const					= 0;
	__i__	virtual	BOOL		SetFileLength( const s64 Length )			= 0;
	__i__	virtual	s64			Tell		( void )const					= 0;
	__i__	virtual	BOOL		Seek		( const s64 offset )			= 0;
	__i__	virtual	u32			GetCRC32	( void )						= 0;
	__i__	virtual	BOOL		IsOpen		( void )const					= 0;
	__i__	virtual	BOOL		IsCanRead	( void )const					= 0;
	__i__	virtual	BOOL		IsCanWrite	( void )const					= 0;
};
//------------------------------------------
class CWQSG_bufFile:public CWQSG_xFile
{
	u8*		m_buffer;
	size_t	m_pos;
	size_t	m_size;
	BOOL	m_bCanWrite;
protected:
public:
	CWQSG_bufFile();
	CWQSG_bufFile( void*const buffer , const size_t size , const BOOL bCanWrite );
	virtual	~CWQSG_bufFile();
	__i__	virtual u32			Read		( void*const lpBuffre , const u32 len );
	__i__	virtual u32			Write		( void const*const lpBuffre , const u32 len );
	__i__	virtual void		Close		( void );
//	__i__	virtual BOOL		WriteStrW	( WCHAR const*const str );		//
	__i__	virtual s64			GetFileSize	( void )const;
	__i__	virtual BOOL		SetFileLength( const s64 Length );
	__i__	virtual s64			Tell		( void )const;
	__i__	virtual BOOL		Seek		( const s64 offset );
	__i__	virtual u32			GetCRC32	( void );
	__i__	virtual BOOL		IsOpen		( void )const;
	__i__	virtual	BOOL		IsCanRead	( void )const;
	__i__	virtual	BOOL		IsCanWrite	( void )const;
	//---------------------------------------------------
	__i__	BOOL		OpenFile( void*const buffer , const size_t size , const BOOL bCanWrite );
};
//------------------------------------------
class CWQSG_File:public CWQSG_xFile
{
	HANDLE		m_hFile;
	DWORD		m_dwDesiredAccess;
public:
	inline CWQSG_File( void )
		: m_hFile(NULL)
		, m_dwDesiredAccess(0)
	{
		_ASSERT( sizeof( LARGE_INTEGER ) == sizeof(s64) );
	}

	virtual	~CWQSG_File( void )
	{
		Close();
	}
	//----------------------------------------------------
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len );
	__i__	virtual	u32			Write		( void const*const lpBuffre , const u32 len );
	__i__	virtual	void		Close		( void );
	__i__	virtual	s64			GetFileSize	( void )const;
	__i__	virtual	BOOL		SetFileLength( const s64 Length );
	__i__	virtual	s64			Tell		( void )const;
	__i__	virtual	BOOL		Seek		( const s64 offset );
	__i__	virtual	u32			GetCRC32	( void );
	__i__	virtual	BOOL		IsOpen		( void )const;
	__i__	virtual	BOOL		IsCanRead	( void )const;
	__i__	virtual	BOOL		IsCanWrite	( void )const;
	//---------------------------------------------------
	__i__		BOOL		OpenFile( WCHAR const*const lpFileName , const DWORD MODE , const DWORD ShareMode = FILE_SHARE_READ );
	__i__		BOOL		OpenFile( char const*const lpFileName , const DWORD MODE , const DWORD ShareMode = FILE_SHARE_READ );
	__i__		HANDLE		GetFileHANDLE( void )const;
};
//------------------------------------------
class CWQSG_memFile:public CWQSG_xFile
{
	void*	m_mem;
	size_t	m_pointer;
	size_t	m_FileSize;
	size_t	m_memSize;
	size_t	m_inc;
public:
	CWQSG_memFile()
		: m_mem(NULL)
		, m_pointer(0)
		, m_FileSize(0)
		, m_memSize(0)
		, m_inc(2048)
	{
		_ASSERT( sizeof(s64) > sizeof(size_t) );
	}
	virtual	~CWQSG_memFile()
	{
		Close();
	}
	//--------------------------
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len );
	__i__	virtual	u32			Write		( void const*const lpBuffre , const u32 len );
	__i__	virtual	void		Close		( void );
	__i__	virtual	s64			GetFileSize	( void )const;
	__i__	virtual	BOOL		SetFileLength( const s64 Length );
	__i__	virtual	s64			Tell		( void )const;
	__i__	virtual	BOOL		Seek		( const s64 offset );
	__i__	virtual	u32			GetCRC32	( void );
	__i__	virtual	BOOL		IsOpen		( void )const;
	__i__	virtual	BOOL		IsCanRead	( void )const;
	__i__	virtual	BOOL		IsCanWrite	( void )const;
	//----------------------------------------------------------------------
	__i__		void*	GetBuf	( void )const	{	return m_mem;	}
	__i__		BOOL	SetInc	( int inc );
	__i__		void*	�ж�	( void )	{	void*const rt = m_mem;m_memSize = m_FileSize = m_pointer = 0;m_mem = NULL;	return rt;	}
};
//------------------------------------------
//WCHAR*		WQSG_�����ı��ļ����ڴ�( ::CWQSG_File* lpFile );
__i__
//CWD_BIN*	WQSG_ReadBIN	( HANDLE FILE_ID , UINT len );
WCHAR*		WQSG_GetTXT_Line	( WCHAR** WTXT_ALL );
WQSG_enCP	WQSG_A_U_X	( HANDLE hfile );
///--------------------------------------------------------------------------------
class C�ڴ��ı�_W
{
	WCHAR*		m_errTXT;
	WCHAR*		m_�ı�;
protected:
	WQSG_enCP	m_CP;
	WCHAR*		m_TXT;
public:
	C�ڴ��ı�_W(void): m_TXT( NULL )	, m_�ı�( NULL ) , m_errTXT( NULL )	, m_CP( en_CP_NULL ){	}
	virtual	~C�ڴ��ı�_W(void)	{		�ͷ����();	}
	__i__	WCHAR* GeterrTXT(void){	return m_errTXT;	}
	__i__	void	�ͷ����(void);
	__i__	BOOL	Load( WCHAR const*const �ļ�·�� , DWORD �����ļ������ , const UINT codePage = CP_ACP );
	__i__	WCHAR*	ȡһ���ı�(void);
	__i__	WCHAR*	ȡȫ�ı�(void){	m_errTXT = NULL;return m_�ı�;	}
	__i__	BOOL	��ͷ��ʼ( void )
	{
		m_errTXT = NULL;
		if( m_�ı� )
		{
			m_TXT = m_�ı�;
			return TRUE;
		}
		return FALSE;
	}
	__i__	WQSG_enCP GetCP(void){	m_errTXT = NULL;return m_CP;	}
	__i__	BOOL	����( WCHAR const*const p�ı� );
	__i__	WCHAR*	�ж�( void );
};
///--------------------------------------------------------------------------------
inline	BOOL WQSG_ȡ��·��( WCHAR const*const longPath , WCHAR* shortPath );
inline	BOOL WQSG_ȡ��·���ļ���( WCHAR const*const longPath , WCHAR* shortPath );
//---------------------------------------------------------------------------------
///		ȡ����exe��·��
///--------------------------------------------------------------------------------
inline	BOOL WQSG_GetExePath( WCHAR*const outBuf , DWORD maxWordNum )
{
	WCHAR exePathName[MAX_PATH*2];
	DWORD revCount;

	if( NULL == outBuf )
		return FALSE;

	revCount = ::GetModuleFileNameW( NULL , exePathName , MAX_PATH*2 );
	if( revCount <= 3 )
		return FALSE;

	{
		WCHAR* tmp = exePathName + revCount;
		while( (exePathName != tmp) && (*tmp != L'\\' ) )
		{
			--revCount;
			--tmp;
		}
		*tmp = L'\0';
	}
	return( ( revCount >= 2 ) && (revCount <= maxWordNum) && ( revCount == WQSG_strcpy_Ex( exePathName , outBuf , maxWordNum ) ) );
}
//---------------------------------------------------------------------------------
///		�ƶ��ļ�,����ļ��Ѿ�����,�Զ�����
///--------------------------------------------------------------------------------
inline	BOOL WQSG_MoveFileEx( WCHAR const*const srcFileName , WCHAR const*const newFileName )
{
	WCHAR tmpNew[ MAX_PATH ];
	WCHAR* newname = (WCHAR*)newFileName;

	WCHAR path[ MAX_PATH ] , extName[ MAX_PATH ];
	int i = 0;
	///-------------------------------------------
	if( ( NULL == srcFileName ) || ( srcFileName[1] != L':' ) ||
		( NULL == newFileName ) || ( newFileName[1] != L':' )
		)
		return FALSE;
_gt_start:
	if( MoveFileW( srcFileName , newname ) )
	{
		return TRUE;
	}
	else if(  ERROR_FILE_EXISTS == ::GetLastError() )
	{
		if( newname != tmpNew )
		{
			WCHAR B[MAX_PATH],C[MAX_PATH];
			newname = tmpNew;
			if( WQSG_strlen ( newFileName ) > ( MAX_PATH - 10 ) )
				return FALSE;
#if ___SW_���ξ���___
#pragma warning(disable: 4996)
#endif
			_wsplitpath( newFileName , path , B , C , extName );
			_wmakepath( path , tmpNew , B , C , L"" );
#if ___SW_���ξ���___
#pragma warning(default: 4996)
#endif
		}
		if( 3 == wsprintfW( tmpNew , L"%s (%u)%s" , path , ++i , extName ) )
			goto _gt_start;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------
///		����Ƿ���Ŀ¼
///		path		Ҫ�����·��
///--------------------------------------------------------------------------------
inline	BOOL WQSG_IsDir( WCHAR const*const path )
{
	DWORD attr;
	if( NULL == path )
		return FALSE;

	attr = GetFileAttributesW( path );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( attr & FILE_ATTRIBUTE_DIRECTORY ) );
}
//---------------------------------------------------------------------------------
///		����Ƿ����ļ�
///		pathName	Ҫ�����ļ�·����
///--------------------------------------------------------------------------------
inline	BOOL WQSG_IsFile( WCHAR const*const pathName )
{
	DWORD attr;
	if( NULL == pathName )
		return FALSE;

	attr = GetFileAttributesW( pathName );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( !( attr & FILE_ATTRIBUTE_DIRECTORY ) ) );
}
//---------------------------------------------------------------------------------
///		����Ŀ¼,�ɹ����Ѿ����ڷ��� TRUE
///		path		Ҫ������·��
///--------------------------------------------------------------------------------
inline	BOOL WQSG_CreateDir( WCHAR const*const path )
{
     if( NULL == path )
		 return FALSE;

	if( WQSG_IsDir( path ) )
		return TRUE;
	return ::CreateDirectoryW( path , NULL );
}
#endif

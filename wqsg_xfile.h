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
	__i__		void*	切断	( void )	{	void*const rt = m_mem;m_memSize = m_FileSize = m_pointer = 0;m_mem = NULL;	return rt;	}
};
//------------------------------------------
//WCHAR*		WQSG_加载文本文件到内存( ::CWQSG_File* lpFile );
__i__
//CWD_BIN*	WQSG_ReadBIN	( HANDLE FILE_ID , UINT len );
WCHAR*		WQSG_GetTXT_Line	( WCHAR** WTXT_ALL );
WQSG_enCP	WQSG_A_U_X	( HANDLE hfile );
///--------------------------------------------------------------------------------
class CMemTextW
{
	WCHAR* m_errTXT;
	WCHAR* m_Text;
protected:
	WQSG_enCP m_CP;
	WCHAR* m_TXT;
public:
	inline CMemTextW(void): m_TXT( NULL ) , m_Text( NULL ) , m_errTXT( NULL ) , m_CP( en_CP_NULL ){ }
	virtual	inline ~CMemTextW(void){	CMemTextW::Clear();	}
	inline 	const WCHAR* GetErrTXT(void){	return m_errTXT;	}
	inline 	void	Clear(void);
	inline 	BOOL	Load( const WCHAR*const 文件路径 , DWORD 允许文件最长长度 , const UINT codePage = CP_ACP );
	inline 	WCHAR*	GetLine(void);
	inline 	WCHAR*	GetText(void){	m_errTXT = NULL;return m_Text;	}
	inline  	BOOL	ReStart( void )
	{
		m_errTXT = NULL;
		if( m_Text )
		{
			m_TXT = m_Text;
			return TRUE;
		}
		return FALSE;
	}
	inline 	WQSG_enCP GetCP(void){	m_errTXT = NULL;return m_CP;	}
	inline 	BOOL	关联( WCHAR const*const pText );
	inline 	WCHAR*	切断( void );
};
///--------------------------------------------------------------------------------
inline BOOL WQSG_取短路径( WCHAR const*const longPath , WCHAR* shortPath );
inline BOOL WQSG_取短路径文件名( WCHAR const*const longPath , WCHAR* shortPath );
//---------------------------------------------------------------------------------
///		取自身exe的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetExePath( WCHAR*const outBuf , DWORD maxWordNum );
//---------------------------------------------------------------------------------
///		移动文件,如果文件已经存在,自动改名
///--------------------------------------------------------------------------------
inline BOOL WQSG_MoveFileEx( WCHAR const*const srcFileName , WCHAR const*const newFileName );
//---------------------------------------------------------------------------------
///		检测是否是目录
///		path		要检测是路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsDir( WCHAR const*const path );
//---------------------------------------------------------------------------------
///		检测是否是文件
///		pathName	要检测的文件路径名
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsFile( WCHAR const*const pathName );
//---------------------------------------------------------------------------------
///		创建目录,成功或已经存在返回 TRUE
///		path		要创建的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_CreateDir( WCHAR const*const path );
#include "source/WQSG_xFile_inline.h"

#endif

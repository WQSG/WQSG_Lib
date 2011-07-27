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
#ifndef __WQSG_CxFILE_H__
#define __WQSG_CxFILE_H__
#include <stdio.h>
#include <string.h>
//#include <afxwin.h> //基础库不能有MFC相关的
#include "../Interface/wqsg_def.h"
#include <windows.h>
#define	__i__
//------------------------------------------------------------------------------------------------------------------------
class CWQSG_CxFILE
{
protected:
	CWQSG_CxFILE(){}
	virtual	~CWQSG_CxFILE(){}
public:
	__i__	virtual	int			fgetc		( void )	=	0;
	__i__	virtual	int			fputc		( const int _Ch )				= 0;
#if 0
	__i__	virtual	size_t		fread		( void *buffer , size_t size , size_t count )		= 0;
	__i__	virtual	size_t		fwrite		( const void *buffer , size_t size , size_t count )	= 0;
#endif
	__i__	virtual	int			fclose		( void )						= 0;
//	__i__	virtual	BOOL		WriteStrW	( WCHAR const*const str );		//
//	__i__	virtual	BOOL		GetFileSize	( s64& size )const				= 0;
//	__i__	virtual	BOOL		SetFileLength( const s64 Length )			= 0;
	__i__	virtual	long		ftell		( void )const					= 0;
	__i__	virtual	int			fseek		( const long _Offset , const int _Origin )		= 0;
//	__i__	virtual	u32			GetCRC32	( void )						= 0;
	__i__	virtual	bool		IsOpen		( void )const					= 0;
};
//------------------------------------------------------------------------------------------------------------------------
class CWQSG_CFILE : public CWQSG_CxFILE
{
	FILE*	m_fp;
protected:
public:
	CWQSG_CFILE():m_fp(NULL){}
	virtual	~CWQSG_CFILE(){	fclose();	}
	///失败返回 EOF
	__i__	virtual	int			fgetc		( void );
	///失败返回 EOF
	__i__	virtual	int			fputc		( const int _Ch );
	///
	__i__	virtual	size_t		fread		( void *buffer , size_t size , size_t count );
	///
	__i__	virtual	size_t		fwrite		( const void *buffer , size_t size , size_t count );
	///成功返回 0 , 失败返回 EOF
	__i__	virtual	int			fclose		( void );
	///成功返回 读写指针 , 失败返回 -1L
	__i__	virtual	long		ftell		( void )const;
	///成功返回 0 , 失败返回 EOF
	__i__	virtual	int			fseek		( const long _Offset , const int _Origin );
	///成功返回 true , 失败返回 false
	__i__	virtual	bool		IsOpen		( void )const;
	//------------------------------------------------------------------------------------
	__i__
	///成功返回 true , 失败返回 false
	__i__	virtual	bool		fopen		( const char *filename , const char *mode );
	//------------------------------------------------------------------------------------
	__i__
	///成功返回 读写指针 , 失败返回 -1L
	__i__	virtual	__int64		_ftelli64		( void )const;
	///成功返回 0 , 失败返回 EOF
	__i__	virtual	int			_fseeki64		( const __int64 _Offset , const int _Origin );
};
//------------------------------------------------------------------------------------------------------------------------
class CWQSG_CmemFILE : public CWQSG_CxFILE
{
	unsigned char*	m_ptr;
	long			m_fileSize;
	long			m_offset;
	//-------------------------
	size_t			m_memSize;
	//-------------------------
	bool			m_bCanWrite;
	bool			m_bCanAppend;
	__i__	bool	zzz_SetMemSize( const long size );
#if 0
	__i__	size_t		fread		( void *buffer , const size_t size );
	__i__	size_t		fwrite		( const void *buffer , const size_t size );
#endif
protected:
public:
	CWQSG_CmemFILE():m_ptr(NULL),m_fileSize(0),m_offset(0),m_memSize(0),m_bCanWrite(false),m_bCanAppend(false){}
	virtual	~CWQSG_CmemFILE(){	fclose();	}
	//--------------------------------------------------------------------------------------------
	__i__	virtual	int			fgetc		( void );
	__i__	virtual	int			fputc		( const int _Ch );
#if 0
	__i__	virtual	size_t		fread		( void *buffer , size_t size , size_t count );
	__i__	virtual	size_t		fwrite		( const void *buffer , size_t size , size_t count );
#endif
	__i__	virtual	int			fclose		( void );
	__i__	virtual	long		ftell		( void )const;
	__i__	virtual	int			fseek		( const long _Offset , const int _Origin );
	__i__	virtual	bool		IsOpen		( void )const;
	//-------------------------------------------------------------------------------------------
	__i__	virtual	bool		fopen		( const void* buffer , const long size , const bool bCanWrite );
	__i__	virtual	bool		fopen		( void );
	//-------------------------------------------------------------------------------------------
	__i__	const void*const	GetBuf		( void ){	return m_ptr;	}
	__i__			long		GetFileSize	( void ){	return m_fileSize;	}
	//------------------------------------------------------------------------------------
};
inline int __cdecl fgetc( CWQSG_CxFILE& _File ){	return _File.fgetc();}
inline int __cdecl getc( CWQSG_CxFILE& _File ){	return _File.fgetc();}
inline int __cdecl fputc( const int _Ch , CWQSG_CxFILE& _File ){	return	_File.fputc( _Ch );}
inline int __cdecl putc( const int _Ch , CWQSG_CxFILE& _File ){	return	_File.fputc( _Ch );}
inline int __cdecl fclose( CWQSG_CxFILE& _File ){	return _File.fclose();}
inline long __cdecl ftell( CWQSG_CxFILE& _File ){	return _File.ftell();}
inline int __cdecl fseek( CWQSG_CxFILE& _File , const long _Offset , const int _Origin ){	return _File.fseek( _Offset , _Origin );}

#endif

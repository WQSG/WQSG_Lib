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

	_m_CRC32()	: m_CRC32_seed((u32)0xFFFFFFFF)
	{
#if 0
		u32 * pcrc = m_crc32_table;
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
#endif
	}

	virtual	~_m_CRC32()
	{

	}

	inline	void NEW_CRC32()
	{
		m_CRC32_seed = (u32)0xFFFFFFFF;
	}

	inline	u32 _m_CRC32::GetCRC32( const void*const pbuf , const u32 buflen )
	{
		static const u32 s_crc32_table[256] = {
			0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
			0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
			0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
			0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
			0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
			0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
			0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
			0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 
			0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 
			0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 
			0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 
			0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 
			0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 
			0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 
			0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 
			0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 
			0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 
			0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 
			0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 
			0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 
			0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 
			0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 
			0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 
			0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 
			0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 
			0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 
			0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 
			0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 
			0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 
			0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 
			0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 
			0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 
			0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 
			0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 
			0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 
			0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 
			0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 
			0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 
			0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 
			0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 
			0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 
			0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 
			0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
		};

		const u8* ptr = (const u8*)pbuf;
		u32 _buflen = (u32)buflen;

		while ( _buflen-- )
			m_CRC32_seed = (m_CRC32_seed >> 8) ^(s_crc32_table[(m_CRC32_seed & 0xff) ^(*ptr++)]);

		return m_CRC32_seed^(u32)0xFFFFFFFF;
	}
};
//------------------------------------------
class CWQSG_xFile
{
protected:
	CWQSG_xFile(){}
	virtual	~CWQSG_xFile(){}
public:
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len )			= 0;
	__i__	virtual	u32			Write		( const void*const lpBuffre , const u32 len )	= 0;
	__i__	virtual	void		Close		( void )						= 0;
	__i__	virtual	BOOL		WriteStrW	( const WCHAR*const str );		//
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
class CWQSG_bufFile : public CWQSG_xFile
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
	__i__	virtual u32			Write		( const void*const lpBuffre , const u32 len );
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
class CWQSG_File : public CWQSG_xFile
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
class CWQSG_memFile : public CWQSG_xFile
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
WCHAR*		WQSG_GetTXT_Line	( WCHAR** a_pWTXT_ALL );
WQSG_enCP	WQSG_A_U_X	( HANDLE a_hfile );
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
	inline 	BOOL	Load( const WCHAR*const a_pFile , DWORD a_dwMaxLen , const UINT a_uCodePage = CP_ACP );
	inline 	const WCHAR*	GetLine(void);
	inline 	const WCHAR*	GetText(void){	m_errTXT = NULL;return m_Text;	}
	inline  BOOL	ReStart( void )
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
	inline 	BOOL	关联( const WCHAR*const pText );
	inline 	WCHAR*	切断( void );
};
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetShortPath( const WCHAR*const longPath , WCHAR* shortPath );
inline BOOL WQSG_GetShortPathName( const WCHAR*const longPath , WCHAR* shortPath );
//---------------------------------------------------------------------------------
///		取自身exe的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetExePath( WCHAR*const outBuf , DWORD maxWordNum );
//---------------------------------------------------------------------------------
///		移动文件,如果文件已经存在,自动改名
///--------------------------------------------------------------------------------
inline BOOL WQSG_MoveFileEx( const WCHAR*const srcFileName , const WCHAR*const newFileName );
//---------------------------------------------------------------------------------
///		检测是否是目录
///		path		要检测是路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsDir( const WCHAR*const path );
//---------------------------------------------------------------------------------
///		检测是否是文件
///		pathName	要检测的文件路径名
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsFile( const WCHAR*const pathName );
//---------------------------------------------------------------------------------
///		创建目录,成功或已经存在返回 TRUE
///		path		要创建的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_CreateDir( const WCHAR*const path );

__i__

#include "source/WQSG_xFile_inline.h"

#endif

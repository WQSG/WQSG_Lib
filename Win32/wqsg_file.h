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
#ifndef __WQSG_FILE_H__
#define __WQSG_FILE_H__

#include <Windows.h>

#include "../Interface/wqsg_ifile.h"
#include "../Common/wqsg_xfile.h"

EWQSG_CodePage	WQSG_A_U_X	( HANDLE a_hfile );

class CWQSG_File : public CWQSG_IFile
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
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len ) override;
	__i__	virtual	u32			Write		( void const*const lpBuffre , const u32 len ) override;
	__i__	virtual	void		Close		( void ) override;
	__i__	virtual	s64			GetFileSize	( void )const override;
	__i__	virtual	BOOL		SetFileLength( const s64 Length ) override;
	__i__	virtual	s64			Tell		( void )const override;
	__i__	virtual	BOOL		Seek		( const s64 offset ) override;
	__i__	virtual	u32			GetCRC32	( void ) override;
	__i__	virtual	BOOL		IsOpen		( void )const override;
	__i__	virtual	BOOL		IsCanRead	( void )const override;
	__i__	virtual	BOOL		IsCanWrite	( void )const override;
	//---------------------------------------------------
	__i__		BOOL		OpenFile( WCHAR const*const lpFileName , const DWORD MODE , const DWORD ShareMode = FILE_SHARE_READ );
	__i__		BOOL		OpenFile( char const*const lpFileName , const DWORD MODE , const DWORD ShareMode = FILE_SHARE_READ );
	__i__		HANDLE		GetFileHANDLE( void )const;
	__i__		BOOL		Flush( void );
};
///--------------------------------------------------------------------------------
class CMemTextW
{
	WCHAR* m_errTXT;
	WCHAR* m_Text;
protected:
	EWQSG_CodePage m_CP;
	WCHAR* m_TXT;
public:
	inline CMemTextW(void): m_TXT( NULL ) , m_Text( NULL ) , m_errTXT( NULL ) , m_CP( E_CP_NULL ){ }
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
	inline 	EWQSG_CodePage GetCP(void){	m_errTXT = NULL;return m_CP;	}
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
#include "wqsg_file_inline.h"
#endif //__WQSG_FILE_H__

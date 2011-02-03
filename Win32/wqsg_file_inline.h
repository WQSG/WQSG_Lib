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
#ifndef __WQSG_FILE_INLINE__
#define __WQSG_FILE_INLINE__

//------------------------------------------------------------------------------
inline EWQSG_CodePage WQSG_A_U_X( HANDLE a_hfile )
{
	if(NULL == a_hfile)
		return E_CP_NULL;

	::SetFilePointer( a_hfile , 0 , NULL , FILE_BEGIN );

	DWORD sizeH;

	DWORD sizeL = ::GetFileSize( a_hfile , &sizeH );

	const s64 size = ( ((s64)sizeH)<<32) | sizeL;

	if( size < 0 )
		return E_CP_NULL;

	if( size >= 2 )
	{
		u32 con_i = 0;
		sizeL = (size >= 3)?3:2;

		if( (!::ReadFile( a_hfile , &con_i , sizeL , &sizeH , NULL ))
			|| (sizeH != sizeL) )
		{
			return E_CP_NULL;
		}

		switch( con_i )
		{
		case 0xBFBBEF:
			return E_CP_UTF8;
		default:

			switch( con_i & 0xFFFF )
			{
			case 0xFEFF:
				return E_CP_UNICODE;

			case 0xFFFE:
				return E_CP_UNICODE_BIG;

			default:
				break;
			}
		}
	}

	return E_CP_ANSI;
}
//------------------------------------------------------------------------------
inline u32		CWQSG_File::Read		( void*const lpBuffre , const u32 len )
{
	if( NULL == m_hFile )
		return FALSE;

	u32 III;
	return( ::ReadFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline u32		CWQSG_File::Write		( void const*const lpBuffre , const u32 len )
{
	if(NULL == m_hFile)
		return FALSE;

	u32 III;
	return( ::WriteFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;
}
///--------------------------------------------------------------------------------
inline void	CWQSG_File::Close		( void )
{
	if( m_hFile )
	{
		::CloseHandle(m_hFile);

		m_hFile = NULL;
	}

	m_dwDesiredAccess = 0;
}
///--------------------------------------------------------------------------------
inline s64		CWQSG_File::GetFileSize	( void )const
{
	s64 size = -1;

	LARGE_INTEGER li_size;

	if( (NULL != m_hFile) && GetFileSizeEx( m_hFile , &li_size ) )
		size = li_size.QuadPart;

	return size;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::SetFileLength	( const s64 Length )
{
	return ( Seek( Length ) && ::SetEndOfFile( m_hFile ) );
}
///--------------------------------------------------------------------------------
inline s64		CWQSG_File::Tell		( void )const
{
	s64 rtOffset = -1;

	if( NULL != m_hFile )
	{
		_ASSERT( sizeof( LARGE_INTEGER ) == sizeof(rtOffset) );

		LARGE_INTEGER offset_in;	offset_in.QuadPart = 0;
		LARGE_INTEGER offset_out;

		if( ::SetFilePointerEx( m_hFile , offset_in , &offset_out , FILE_CURRENT ) )
			rtOffset = offset_out.QuadPart;
	}

	return rtOffset;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::Seek		( const s64 offset )
{

	if( ( NULL == m_hFile )||( offset < 0 ) )
		return FALSE;

	_ASSERT( sizeof( LARGE_INTEGER ) >= sizeof(offset) );

	LARGE_INTEGER offset_in;
	offset_in.QuadPart = offset;
	LARGE_INTEGER offset_out;

	return ::SetFilePointerEx( m_hFile , offset_in , &offset_out , FILE_BEGIN );
}
///--------------------------------------------------------------------------------
inline u32		CWQSG_File::GetCRC32	( void )
{
	u32 rtCRC = 0;
	if(NULL == m_hFile)
		return rtCRC;
	////////////////
	const s64 nOldPos = Tell( );
	s64 size = GetFileSize( );

	if( (nOldPos < 0) || (size < 0) )
		return rtCRC;
	////////////////
	Seek( 0 );
	/////////////////
	const u32 buflen = ( size > 33554432)?33554432:(u32)size;
	////////////////////////
	u8*const pbuf = new u8[ buflen ];
	CCrc32	_CRC32;

	_CRC32.NEW_CRC32();

	while( size )
	{
		const DWORD rLen = (size > buflen)?buflen:(DWORD)size;
		DWORD III;

		if( (!::ReadFile( m_hFile , pbuf , rLen , &III , NULL )) || (III != rLen) )
		{
			rtCRC = 0;
			break;
		}

		size -= III;

		rtCRC = _CRC32.GetCrc32( pbuf , buflen );
	}

	delete[]pbuf;

	Seek( nOldPos );

	return rtCRC;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::IsOpen		( void )const
{
	return (NULL != m_hFile);
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::IsCanRead	( void )const
{
	return m_dwDesiredAccess & GENERIC_READ;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::IsCanWrite	( void )const
{
	return m_dwDesiredAccess & GENERIC_WRITE;
}
//============================================================================================
inline BOOL	CWQSG_File::OpenFile	( WCHAR const*const _lpFileName , const DWORD MODE , const DWORD ShareMode )
{
	Close();

	WCHAR const* lpFileName = (WCHAR const*)_lpFileName;
	WCHAR shortPathName[ MAX_PATH ];

	DWORD dwCreationDisposition,dwShareMode;

	switch(MODE)
	{
	case 1://只读,不存在失败
		m_dwDesiredAccess = GENERIC_READ;
		dwCreationDisposition = OPEN_EXISTING;
		break;

	case 2://只写,不存在失败
		m_dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = OPEN_EXISTING;
		break;

	case 3://读写,不存在失败
		m_dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		dwCreationDisposition = OPEN_EXISTING;
		break;

	case 4://重写,存在清空,不存在新建
		m_dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = CREATE_ALWAYS;
		break;

/*	case 5://改写,存在直接打开,不存在新建
		dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = OPEN_ALWAYS;
		break;

	case 6://改读写,存在直接 打开,不存在新建
		dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		dwCreationDisposition = OPEN_ALWAYS;
		break;*/

	case 9:
		m_dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		dwCreationDisposition = OPEN_ALWAYS;
		break;

	default:
			return FALSE;
	}

	switch(ShareMode)
	{

	case 1://无限制
		dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case 2://禁止读
		dwShareMode = FILE_SHARE_WRITE;
		break;

	case 3://禁止写
		dwShareMode = FILE_SHARE_READ;
		break;

	default:
		dwShareMode = 0;
	}

__gtReOpen:

	m_hFile = ::CreateFile ( lpFileName , m_dwDesiredAccess , dwShareMode , NULL , dwCreationDisposition , NULL , NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )//如果打开失败
	{
		m_hFile = NULL;

		if( ( lpFileName != shortPathName ) && ( ::GetLastError() == ERROR_FILE_NOT_FOUND ) )
		{
			if( OPEN_EXISTING == dwCreationDisposition )
			{
				if( !WQSG_GetShortPathName( lpFileName , shortPathName ) )
					return FALSE;
			}
			else
			{
				if( !WQSG_GetShortPath( lpFileName , shortPathName ) )
					return FALSE;

				WCHAR* tmp = (WCHAR*)lpFileName;
				while( *tmp )++tmp;
				while( ( tmp > lpFileName ) && ( *tmp != L'\\' ) )--tmp;

				if( *tmp != L'\\' )
					return FALSE;

				WQSG_strcpy( ++tmp , shortPathName + WQSG_strlen ( shortPathName ) );
			}

			lpFileName = (WCHAR const*)shortPathName;

			goto __gtReOpen;
		}

		return FALSE;
	}

	::SetFilePointer( m_hFile , 0 , NULL , ( MODE == 9 )?FILE_END:FILE_BEGIN );

	return TRUE;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_File::OpenFile	( char const*const lpFileName , const DWORD MODE , const DWORD ShareMode )
{
	WCHAR*const tmp = WQSG_char_W( lpFileName );

	const BOOL rev = OpenFile( tmp , MODE , ShareMode ) ;

	delete[]tmp;

	return rev;
}
///--------------------------------------------------------------------------------
inline HANDLE	CWQSG_File::GetFileHANDLE( void )const
{
	return m_hFile;
}

inline BOOL	CWQSG_File::Flush( void )
{
	return FlushFileBuffers( m_hFile );
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline void CMemTextW::Clear()
{
	m_errTXT = NULL;
	m_CP = E_CP_NULL;
	delete[]m_Text;
	m_errTXT = m_Text = m_TXT = NULL;
}
///--------------------------------------------------------------------------------
inline BOOL	CMemTextW::Load( WCHAR const*const a_pFile , DWORD a_dwMaxLen  , const UINT a_uCodePage )
{
	m_errTXT = NULL;

	Clear();

	::CWQSG_File TXT_File;

	if( ! TXT_File.OpenFile( a_pFile , 1 , 3 ) )
	{
		m_errTXT = L"打开失败";
		return FALSE;
	}

	s64 size = TXT_File.GetFileSize( );

	if(size > (s64)a_dwMaxLen)
	{
		/*
		WCHAR tmp1[256];
		if(size < 1024){
			::swprintf(tmp1,100,L"为了安全,文本不能超过 %u Byte",允许文件最长长度);
		}else if(size < 1048576){
			允许文件最长长度 /= 1024;
			::swprintf(tmp1,100,L"为了安全,文本不能超过 %u KB",允许文件最长长度);
		}else{
			允许文件最长长度 /= 1048576;
			::swprintf(tmp1,100,L"为了安全,文本不能超过 %u MB",允许文件最长长度);
		}*/
		m_errTXT = L"文件过大长";
		return FALSE;
	}

	u8* tmp;

	switch( m_CP = ::WQSG_A_U_X(TXT_File.GetFileHANDLE()) )
	{
	case E_CP_ANSI:
		TXT_File.Seek(0);
		tmp = new u8 [(DWORD)size + 1];tmp[(DWORD)size] = 0;
		TXT_File.Read( tmp ,(u32)size );
		m_Text = ::WQSG_char_W( (char*)tmp , a_uCodePage );
		delete[]tmp;

		break;

	case E_CP_UNICODE:
		TXT_File.Seek(2);
		tmp = new u8 [(UINT)size];
		size -= 2;
		TXT_File.Read(tmp,(u32)size );
		m_Text = (WCHAR*)tmp;
		m_Text[(UINT)size>>1] = 0;

		break;

/*	case case en_CP_UNICODE_BIG:
		TXT_File.Seek(2);
		tmp = new u8 [(UINT)size];
		size -= 2;
		TXT_File.Read(tmp,(U32)size );
		m_文本 = (WCHAR*)tmp;
		m_文本[(UINT)size>>1] = 0;

		break;*/

	case E_CP_UTF8:
		TXT_File.Seek(3);
		tmp = new u8 [(DWORD)size-2];tmp[(DWORD)size-3] = 0;
		TXT_File.Read( tmp ,(u32)size-3 );
		m_Text = WQSG_UTF8_W( (char*)tmp );
		delete[]tmp;

		break;

	default:
		m_errTXT = L"不支持的文本格式";
		return FALSE;
	}

	m_TXT = m_Text;

	return TRUE;
}
///--------------------------------------------------------------------------------
inline const WCHAR* CMemTextW::GetLine()
{
	return ::WQSG_GetTXT_Line(&m_TXT);
}

inline BOOL CMemTextW::关联( const WCHAR*const pText )
{
	m_errTXT = NULL;

	if( NULL == pText )
	{
		m_errTXT = L"输入文本为空";
		return FALSE;
	}

	Clear();

	m_CP = E_CP_UNICODE;
	m_TXT = m_Text = (WCHAR*)pText;

	return TRUE;
}

inline WCHAR* CMemTextW::切断( void )
{
	m_errTXT = (m_Text==NULL)?L"无文本":NULL;

	WCHAR* pTXT = m_Text;

	m_Text = NULL;

	Clear();

	return pTXT;
}
//------------------------------------------------------------------------------
inline BOOL WQSG_GetShortPath( const WCHAR*const longPath , WCHAR* shortPath )
{
	WCHAR tmp[ MAX_PATH ];
	*shortPath = L'\0';

	WCHAR* sp = (WCHAR*)longPath;
	INT pos;
	WCHAR str[ MAX_PATH ];
	INT pos2 = 0;

	if( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 )
	{
		do
		{
			if( ( pos2 + pos + 1 ) >= MAX_PATH )
				return FALSE;

			if( !(sp[++pos]) )
			{
				INT pos0 = WQSG_strcpy( str , tmp );
				WQSG_strcpy( sp , tmp + pos0 );
				DWORD attr = GetFileAttributes( tmp );

				if( attr == INVALID_FILE_ATTRIBUTES )
					return FALSE;

				if( !( attr & FILE_ATTRIBUTE_DIRECTORY ) )
				{
					sp += pos;
					break;
				}
			}

			pos2 += WQSG_strcpy_Ex( sp , str + pos2 , pos );
			sp += pos;

			if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
				return FALSE;

			pos2 = WQSG_strcpy( tmp , str );
		}
		while( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 );
	}

	if( *sp )
	{
		pos = WQSG_strlen( sp );

		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		INT pos0 = WQSG_strcpy( str , tmp );

		WQSG_strcpy( sp , tmp + pos0 );

		DWORD attr = GetFileAttributes( tmp );

		if( attr == INVALID_FILE_ATTRIBUTES )
			return FALSE;

		if( attr & FILE_ATTRIBUTE_DIRECTORY )
		{
			pos2 += WQSG_strcpy( sp , str + pos2 );
			WQSG_strcpy( L"\\" , str + pos2 );
		}
	}

	DWORD attr = GetFileAttributes( str );

	if( attr == INVALID_FILE_ATTRIBUTES )
		return FALSE;

	WQSG_strcpy( str , shortPath );

	return TRUE;
}
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetShortPathName( const WCHAR*const longPath , WCHAR* shortPath )
{
	WCHAR tmp[ MAX_PATH ];
	*shortPath = L'\0';

	WCHAR* sp = (WCHAR*)longPath;
	INT pos;
	WCHAR str[ MAX_PATH ];
	INT pos2 = 0;

	while( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 )
	{
		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		pos2 += WQSG_strcpy_Ex( sp , str + pos2 , pos++ );

		if( sp[pos] )
			pos2 += WQSG_strcpy( L"\\" , str + pos2 );

		sp += pos;

		if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
			return FALSE;

		pos2 = WQSG_strcpy( tmp , str );
	}

	if( *sp )
	{
		pos = WQSG_strlen( sp );

		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		WQSG_strcpy( sp , str + pos2);

		if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
			return FALSE;

		WQSG_strcpy( tmp , str );
	}

	DWORD attr = GetFileAttributes( str );

	if( attr == INVALID_FILE_ATTRIBUTES )
		return FALSE;

	WQSG_strcpy( str , shortPath );

	return TRUE;
}
//---------------------------------------------------------------------------------
///		取自身exe的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetExePath( WCHAR*const outBuf , DWORD maxWordNum )
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
///		移动文件,如果文件已经存在,自动改名
///--------------------------------------------------------------------------------
inline BOOL WQSG_MoveFileEx( const WCHAR*const srcFileName , const WCHAR*const newFileName )
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
#if ___SW_屏蔽警告___
#pragma warning(disable: 4996)
#endif
			_wsplitpath( newFileName , path , B , C , extName );
			_wmakepath( path , tmpNew , B , C , L"" );
#if ___SW_屏蔽警告___
#pragma warning(default: 4996)
#endif
		}
		if( 3 == wsprintfW( tmpNew , L"%s (%u)%s" , path , ++i , extName ) )
			goto _gt_start;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------
///		检测是否是目录
///		path		要检测是路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsDir( const WCHAR*const path )
{
	DWORD attr;
	if( NULL == path )
		return FALSE;

	attr = GetFileAttributesW( path );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( attr & FILE_ATTRIBUTE_DIRECTORY ) );
}
//---------------------------------------------------------------------------------
///		检测是否是文件
///		pathName	要检测的文件路径名
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsFile( const WCHAR*const pathName )
{
	DWORD attr;
	if( NULL == pathName )
		return FALSE;

	attr = GetFileAttributesW( pathName );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( !( attr & FILE_ATTRIBUTE_DIRECTORY ) ) );
}
//---------------------------------------------------------------------------------
///		创建目录,成功或已经存在返回 TRUE
///		path		要创建的路径
///--------------------------------------------------------------------------------
inline BOOL WQSG_CreateDir( const WCHAR*const path )
{
	if( NULL == path )
		return FALSE;

	if( WQSG_IsDir( path ) )
		return TRUE;
	return ::CreateDirectoryW( path , NULL );
}

#endif //__WQSG_FILE_INLINE__


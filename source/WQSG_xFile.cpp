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
#include "../WQSG_xFile.h"

__i__

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
BOOL	CWQSG_xFile::WriteStrW	( WCHAR const*const str )
{
	if( NULL == str )
		return FALSE;

	u32 con_i = ::WQSG_strlen(str);

	if( con_i & 0x80000000 )
		return FALSE;

	con_i <<= 1;

	return ( Write( str , con_i ) == con_i );
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
CWQSG_bufFile::CWQSG_bufFile()
: m_buffer(NULL)
, m_pos(0)
, m_size(0)
, m_bCanWrite(FALSE){}
///--------------------------------------------------------------------------------
CWQSG_bufFile::CWQSG_bufFile( void*const buffer , const size_t size , const BOOL bCanWrite )
: m_buffer( (u8*)buffer )
, m_pos(0)
, m_size(size)
, m_bCanWrite(bCanWrite){}
///--------------------------------------------------------------------------------
CWQSG_bufFile::~CWQSG_bufFile()
{
	Close();
}
///--------------------------------------------------------------------------------
u32		CWQSG_bufFile::Read		( void*const lpBuffre , const u32 len )
{
	_ASSERT( sizeof( size_t ) == sizeof( u32 ) );
	u32 rt = 0;
	if( IsOpen() )
	{
		rt = (u32)WWW_WQSG_ReadFile( m_buffer , m_size , m_pos , lpBuffre , len );
		m_pos += rt;
	}

	return rt;
}
///--------------------------------------------------------------------------------
u32		CWQSG_bufFile::Write		( void const*const lpBuffre , const u32 len )
{
	_ASSERT( sizeof( size_t ) == sizeof( u32 ) );

	u32 rt = 0;

	if( IsOpen() && m_bCanWrite )
	{
		rt = (u32)WWW_WQSG_WriteFile( m_buffer , m_size , m_pos , lpBuffre , len );
		m_pos += rt;
	}

	return rt;
}
///--------------------------------------------------------------------------------
void	CWQSG_bufFile::Close		( void )
{
	m_buffer = NULL;
	m_bCanWrite = FALSE;
}
///--------------------------------------------------------------------------------
s64		CWQSG_bufFile::GetFileSize	( void )const
{
	return ( IsOpen()?m_size:-1 );
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::SetFileLength( const s64 Length )
{
	BOOL rt = FALSE;

	if( IsOpen() && (Length >= 0) && (Length <= (s64)m_size) )
	{
		m_size = (size_t)Length;
		rt = TRUE;
	}

	return rt;
}
///--------------------------------------------------------------------------------
s64		CWQSG_bufFile::Tell		( void )const
{
	return( (IsOpen())?(m_pos):(-1) );
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::Seek		( const s64 offset )
{
	BOOL rt = FALSE;

	if( IsOpen() && (offset >= 0) && (offset <= ((size_t)-1)) )
	{
		m_pos = ((size_t)offset);
		rt = TRUE;
	}
	return rt;
}
///--------------------------------------------------------------------------------
u32		CWQSG_bufFile::GetCRC32	( void )
{
	u32 CRC;

	if( IsOpen() )
	{
		_m_CRC32	_CRC32;
		_CRC32.NEW_CRC32();

		size_t fSize = m_size;
		size_t offset = 0;

		while( fSize )
		{
			if( fSize > (u32)-1 )
			{

				CRC = _CRC32.GetCRC32( m_buffer + offset , (u32)-1 );
				offset += (u32)-1;
				fSize -= (u32)-1;
			}
			else
			{
				CRC = _CRC32.GetCRC32( m_buffer + offset , (u32)fSize );
				offset += fSize;
				fSize = 0;
			}
		}
	}

	return CRC;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::IsOpen		( void )const
{
	return( m_buffer != NULL );
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::IsCanRead	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::IsCanWrite	( void )const
{
	return m_bCanWrite;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_bufFile::OpenFile( void*const buffer , const size_t size , const BOOL bCanWrite )
{
	Close();
	if( m_buffer = (u8*)buffer )
	{
		m_pos = 0;
		m_size = size;
		m_bCanWrite = bCanWrite;
	}

	return IsOpen();
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
u32		CWQSG_File::Read		( void*const lpBuffre , const u32 len )
{
	if( NULL == m_hFile )
		return FALSE;

	u32 III;
	return( ::ReadFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;
}
///--------------------------------------------------------------------------------
u32		CWQSG_File::Write		( void const*const lpBuffre , const u32 len )
{
	if(NULL == m_hFile)
		return FALSE;

	u32 III;
	return( ::WriteFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;
}
///--------------------------------------------------------------------------------
void	CWQSG_File::Close		( void )
{
	if( m_hFile )
	{
		::CloseHandle(m_hFile);

		m_hFile = NULL;
	}

	m_dwDesiredAccess = 0;
}
///--------------------------------------------------------------------------------
s64		CWQSG_File::GetFileSize	( void )const
{
	s64 size = -1;

	LARGE_INTEGER li_size;

	if( (NULL != m_hFile) && GetFileSizeEx( m_hFile , &li_size ) )
		size = li_size.QuadPart;

	return size;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::SetFileLength	( const s64 Length )
{
	return ( Seek( Length ) && ::SetEndOfFile( m_hFile ) );
}
///--------------------------------------------------------------------------------
s64		CWQSG_File::Tell		( void )const
{
	s64 rtOffset = -1;

	if( NULL != m_hFile )
	{
#if 0
		union
		{
			struct{	u32 LowPart;u32 HighPart;};
			s64 QuadPart;
		}x6432;

		x6432.QuadPart = 0;
		x6432.LowPart = ::SetFilePointer( m_hFile , (LONG)0 ,(PLONG)&x6432.HighPart , FILE_CURRENT );
		if( x6432.QuadPart >= 0 )
			rtOffset = x6432.QuadPart;
#else
		_ASSERT( sizeof( LARGE_INTEGER ) == sizeof(rtOffset) );

		LARGE_INTEGER offset_in;	offset_in.QuadPart = 0;
		LARGE_INTEGER offset_out;

		if( ::SetFilePointerEx( m_hFile , offset_in , &offset_out , FILE_CURRENT ) )
			rtOffset = offset_out.QuadPart;
#endif
	}

	return rtOffset;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::Seek		( const s64 offset )
{

	if( ( NULL == m_hFile )||( offset < 0 ) )
		return FALSE;
#if 0
	union
	{
		struct{	u32 LowPart;u32 HighPart;};
		s64 QuadPart;
	}offset_6432;

	offset_6432.QuadPart = offset;

	s32 xH , xL;

	if( offset_6432.HighPart > (u32)0x7FFFFFFF )//高位 太大/////////////////////////////////////////////////
	{
		if( offset_6432.LowPart > (s32)0x7FFFFFFF )// 高位太大,低位也太大
		{
			xL = (s32)0x7FFFFFFF;
			xH = (s32)0x7FFFFFFF;
			offset_6432.HighPart -= (s32)0x7FFFFFFF;
			offset_6432.LowPart -= (s32)0x7FFFFFFF;

			if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_BEGIN ) )
				return FALSE;

			//------------------------------------------------------------------------------

			if(offset_6432.HighPart > (u32)0x7FFFFFFF)//再次确认 高位太大///////////////////////////
			{
				if( offset_6432.LowPart > (u32)0x7FFFFFFF )//再次 高位太大,低位也太大
				{
					xL = (s32)0x7FFFFFFF;
					xH = (s32)0x7FFFFFFF;
					offset_6432.HighPart -= (s32)0x7FFFFFFF;
					offset_6432.LowPart -= (s32)0x7FFFFFFF;

					if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_CURRENT ) )
						return FALSE;
				}
				else//再次 高位太大,低位OK
				{
					xL = offset_6432.LowPart;
					xH = (s32)0x7FFFFFFF;
					offset_6432.HighPart -= (s32)0x7FFFFFFF;
					offset_6432.LowPart = (s32)0;

					if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_CURRENT ) )
						return FALSE;
				}
			}
			else//再次 确认 高位OK////////////////////////////////////////////////
			{
				if( offset_6432.LowPart > (u32)0x7FFFFFFF )//再次 确认高位OK,低位太大
				{
					xL = offset_6432.HighPart;
					xH = (s32)0x7FFFFFFF;
					offset_6432.HighPart = (s32)0;
					offset_6432.LowPart -= (s32)0x7FFFFFFF;

					if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_CURRENT ) )
						return FALSE;
				}
			}
		}
		else//高位 太大 ,低位OK////////////////////////////////////////////////////////
		{
			xH = (s32)0x7FFFFFFF;
			xL = offset_6432.LowPart ;
			offset_6432.LowPart = (s32)0;
			offset_6432.HighPart -= (s32)0x7FFFFFFF;

			if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_BEGIN ) )
				return FALSE;

			if(offset_6432.HighPart > (u32)0x7FFFFFFF)//再次验证 高位,低位OK///////
			{
				xH = (s32)0x7FFFFFFF;
				xL = offset_6432.LowPart ;
				offset_6432.LowPart = (s32)0;
				offset_6432.HighPart -= (s32)0x7FFFFFFF;

				if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_CURRENT ) )
					return FALSE;
			}
		}
	}
	else//高位OK..........验证低位.................................................................
	{
		if( offset_6432.LowPart > (u32)0x7FFFFFFF )//低位 太大
		{
			xL = (s32)0x7FFFFFFF;
			xH = offset_6432.HighPart ;
			offset_6432.LowPart -= (s32)0x7FFFFFFF;
			offset_6432.HighPart = (s32)0;

			if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_BEGIN ) )
				return FALSE;

			if( offset_6432.LowPart > (u32)0x7FFFFFFF )//再次 验证低位
			{

				xL = (s32)0x7FFFFFFF;
				xH = offset_6432.HighPart ;
				offset_6432.LowPart -= (s32)0x7FFFFFFF;
				offset_6432.HighPart = (s32)0;

				if( INVALID_SET_FILE_POINTER == ::SetFilePointer( m_hFile , xL , &xH , FILE_CURRENT ) )
					return FALSE;
			}
		}
		else
		{
			xL = offset_6432.LowPart;
			xH = offset_6432.HighPart ;

			return( INVALID_SET_FILE_POINTER != ::SetFilePointer( m_hFile , xL , &xH , FILE_BEGIN ) );
		}
	}

	xL = offset_6432.LowPart;
	xH = offset_6432.HighPart ;

	return( INVALID_SET_FILE_POINTER != ::SetFilePointer( m_hFile , xL ,&xH , FILE_CURRENT ) );
#else

	_ASSERT( sizeof( LARGE_INTEGER ) >= sizeof(offset) );

	LARGE_INTEGER offset_in;
	offset_in.QuadPart = offset;
	LARGE_INTEGER offset_out;

	return ::SetFilePointerEx( m_hFile , offset_in , &offset_out , FILE_BEGIN );
#endif
}
///--------------------------------------------------------------------------------
u32		CWQSG_File::GetCRC32	( void )
{
	u32 rtCRC = 0;
	if(NULL == m_hFile)
		return rtCRC;
	////////////////
	const s64 当前位置 = Tell( );
	s64 size = GetFileSize( );

	if( (当前位置 < 0) || (size < 0) )
		return rtCRC;
	////////////////
	Seek( 0 );
	/////////////////
	const u32 buflen = ( size > 33554432)?33554432:(u32)size;
	////////////////////////
	u8*const pbuf = new u8[ buflen ];
	_m_CRC32	_CRC32;

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

		rtCRC = _CRC32.GetCRC32( pbuf , buflen );
	}

	delete[]pbuf;

	Seek( 当前位置 );

	return rtCRC;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::IsOpen		( void )const
{
	return (NULL != m_hFile);
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::IsCanRead	( void )const
{
	return m_dwDesiredAccess & GENERIC_READ;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::IsCanWrite	( void )const
{
	return m_dwDesiredAccess & GENERIC_WRITE;
}
//============================================================================================
BOOL	CWQSG_File::OpenFile	( WCHAR const*const _lpFileName , const DWORD MODE , const DWORD ShareMode )
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

__gt重试:

	m_hFile = ::CreateFile ( lpFileName , m_dwDesiredAccess , dwShareMode , NULL , dwCreationDisposition , NULL , NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )//如果打开失败
	{
		m_hFile = NULL;

		if( ( lpFileName != shortPathName ) && ( ::GetLastError() == ERROR_FILE_NOT_FOUND ) )
		{
			if( OPEN_EXISTING == dwCreationDisposition )
			{
				if( !WQSG_取短路径文件名( lpFileName , shortPathName ) )
					return FALSE;
			}
			else
			{
				if( !WQSG_取短路径( lpFileName , shortPathName ) )
					return FALSE;

				WCHAR* tmp = (WCHAR*)lpFileName;
				while( *tmp )++tmp;
				while( ( tmp > lpFileName ) && ( *tmp != L'\\' ) )--tmp;

				if( *tmp != L'\\' )
					return FALSE;

				WQSG_strcpy( ++tmp , shortPathName + WQSG_strlen ( shortPathName ) );
			}

			lpFileName = (WCHAR const*)shortPathName;

			goto __gt重试;
		}

		return FALSE;
	}

	::SetFilePointer( m_hFile , 0 , NULL , ( MODE == 9 )?FILE_END:FILE_BEGIN );

	return TRUE;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_File::OpenFile	( char const*const lpFileName , const DWORD MODE , const DWORD ShareMode )
{
	WCHAR*const tmp = WQSG_char_W( lpFileName );

	const BOOL rev = OpenFile( tmp , MODE , ShareMode ) ;

	delete[]tmp;

	return rev;
}
///--------------------------------------------------------------------------------
HANDLE	CWQSG_File::GetFileHANDLE( void )const
{
	return m_hFile;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if WIN32
#include <malloc.h>
/*

template <typename TYPE_0 , typename TYPE_1>
static inline bool xxx_合法( const TYPE_0& val )
{
	const TYPE_1 t1 = -1;

	if( t1 < 0)//有符号
	{

	}
	else if( val >= 0 )
	{
		if( sizeof(val) > sizeof(t1) )
		{
			if( val < (TYPE_0)t1 )
				return true;
		}
		else
			return true;
	}

	return false;
}
*/
u32		CWQSG_memFile::Read			( void*const lpBuffre , const u32 len )
{
#if 0
#else
	if( (NULL != lpBuffre) && (len > 0) && (m_pointer < m_FileSize) && (m_pointer>=0) )

	{

		size_t iLen = m_FileSize - m_pointer;



		if( (size_t)len < iLen )

			iLen = len;



		if( ( m_pointer + len ) > m_pointer )

		{

			memcpy( lpBuffre , (u8*)m_mem + m_pointer , iLen );

			m_pointer += len;

			return len;

		}

	}

	return 0;
#endif
}

///--------------------------------------------------------------------------------
u32		CWQSG_memFile::Write		( void const*const lpBuffre , const u32 len )
{
	if( ( NULL == lpBuffre) || (len <= 0) || (m_pointer<0) )
		return 0;//错误或无需写出

	void* tmp;
	size_t v需要len;

	{
		const size_t 目标长度 = m_pointer + len;
		if( 目标长度 < m_pointer )
			return 0;

		v需要len = (目标长度 <= m_FileSize)?0:(目标长度 - m_FileSize);

		if( v需要len > 0 )

		{

			const size_t xxsize = (目标长度%m_inc);
			const size_t 目标mem长度 = (目标长度 - xxsize) + ((xxsize>0)?m_inc:0);

			if( 目标mem长度 < 目标长度 )
				return 0;//上溢

			tmp = ::realloc( m_mem , 目标mem长度 );

			if( tmp != NULL )
			{
				m_mem = tmp;
				m_memSize = 目标mem长度;
			}
		}
		else
			tmp = m_mem;
	}

	if( NULL == tmp )
		return 0;

	memcpy( (u8*)tmp + m_pointer , lpBuffre , (size_t)len );
	m_pointer += (size_t)len;

	m_FileSize += v需要len;

	return len;
}
///--------------------------------------------------------------------------------
void	CWQSG_memFile::Close		( void )
{
	::free( m_mem );
	m_mem = NULL;
	m_memSize = m_pointer = m_FileSize = 0;
}
///--------------------------------------------------------------------------------
s64		CWQSG_memFile::GetFileSize	( void )const
{
	return (s64)m_FileSize;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::SetFileLength	( const s64 Length )
{
	if( Length > 0 )
	{
		void* tmp;

		if( ( Length < (s64)(size_t)-1 ) &&
			( (tmp = ::realloc( m_mem , (size_t)Length )) != NULL )
			)
		{
			m_mem = tmp;
			m_memSize = m_pointer = m_FileSize = (size_t)Length;
			return TRUE;
		}
	}
	else if( Length == 0 )
	{
		Close();
		return TRUE;
	}

	return FALSE;
}
///--------------------------------------------------------------------------------
s64		CWQSG_memFile::Tell			( void )const
{
	return ( m_mem != NULL )?m_pointer:0;
}

///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::Seek			( const s64 offset )
{
	if( (u64)offset > (size_t)-1 )
		return FALSE;

	m_pointer = (size_t)offset;

	return TRUE;
}
///--------------------------------------------------------------------------------
u32		CWQSG_memFile::GetCRC32		( void )
{
#if 1

	CWQSG_bufFile tmp( m_mem , m_FileSize , FALSE );

	return tmp.GetCRC32( );

#else

	if( m_mem == NULL ) return 0;

	_m_CRC32	_CRC32;

	u32 CRC;

	size_t fSize = m_FileSize;
	size_t offset = 0;

	while( fSize )
	{
		if( fSize > (u32)-1 )
		{
			CRC = _CRC32.GetCRC32( (u8*)m_mem + offset , (u32)-1 );
			offset += (u32)-1;
			fSize -= (u32)-1;
		}
		else
		{
			CRC = _CRC32.GetCRC32( (u8*)m_mem + offset , (u32)fSize );
			offset += fSize;
			fSize = 0;
		}
	}

	return CRC;
#endif
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::IsOpen		( void )const
{
	return TRUE;
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::IsCanRead	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::IsCanWrite	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
BOOL	CWQSG_memFile::SetInc		( int inc )
{
	if( inc >= 0 )
	{
		m_inc = (size_t)inc;
		return TRUE;
	}

	return FALSE;
}

#endif
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0

WCHAR*	WQSG_加载文本文件到内存	( ::CWQSG_File* lpFile )
{
	if(NULL == lpFile->GetFileHANDLE())
	{
		return NULL;
	}

	ULONGLONG size_LL;

	lpFile->GetFileSize(&size_LL);

	if(size_LL > 0x2000000)
	{
		::MessageBox(NULL,_T("文本过大"),_T("出错了"),0);
		return NULL;
	}

	DWORD size = (DWORD)size_LL;

	WCHAR *out_tmp;

	DWORD III;

	switch(::WQSG_A_U_X(lpFile->GetFileHANDLE()))
	{

	case 2://UNICODE
		lpFile->SetFilePointer(2);
		out_tmp = (WCHAR*)new CHAR[size];
		lpFile->Read((CHAR*)out_tmp,size - 2,&III);
		out_tmp[(size >> 1)] = 0;
		break;

	case 3://BIG UNICODE
		::MessageBox(NULL,_T("暂时不支持Big UNICODE文本格式"),_T("出错了"),0);
		return NULL;
		break;

	case 4://UTF-8
		::MessageBox(NULL,_T("暂时不支持UTF-8文本格式"),_T("出错了"),0);
		return NULL;
		break;

	default://ansi
		lpFile->SetFilePointer(0);
		CHAR * tmp = new CHAR[size + 1];
		lpFile->Read(tmp,size,&III);
		tmp[size] = 0;
		out_tmp = ::WQSG_char_W((UCHAR*)tmp);
		delete tmp;
	}

	return out_tmp;
}
//--------------------------------------------------------------------------------
CWD_BIN*	WQSG_ReadBIN	( HANDLE FILE_ID , UINT len )
{
	if(!FILE_ID)
		return NULL;

	CWD_BIN *OUT_0 = new CWD_BIN;
	OUT_0->BIN = new UCHAR[len];
	OUT_0->LEN = len;

	DWORD con_i;
	if(::ReadFile(FILE_ID,OUT_0->BIN,len,&con_i,NULL))
		return OUT_0;

	delete OUT_0;

	LONG X;

	if( con_i > 0x7FFFFFFF )
	{
		X = 0x7FFFFFFF;
		con_i -= X;
		::SetFilePointer(FILE_ID,-X,NULL,FILE_CURRENT);
	}

	X = con_i;

	::SetFilePointer(FILE_ID,-X,NULL,FILE_CURRENT);

	return NULL;
}
//--------------------------------------------------------------------------------
#endif

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
#include "WQSG_xFile.h"

__i__

//--------------------------------------------------------------------------------

const u32 WQSG_crc32_table[256] = {

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

u32 _m_CRC32::GetCRC32( u8 const*const pbuf , const u32 buflen )

{

	u8 const* ptr = pbuf;

	u32 _buflen = (u32)buflen;

	while ( _buflen-- )

		m_CRC32_seed = (m_CRC32_seed >> 8) ^(WQSG_crc32_table[(m_CRC32_seed & 0xff) ^(*ptr++)]);



	return m_CRC32_seed^(u32)0xFFFFFFFF;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BOOL	CWQSG_xFile::WriteStrW	( WCHAR const*const str )

{

	if( NULL == str )		return FALSE;



	u32 con_i = ::WQSG_strlen(str);



	if( con_i & 0x80000000 )		return FALSE;



	con_i <<= 1;

	return ( Write( str , con_i ) == con_i );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CWQSG_bufFile::CWQSG_bufFile() : m_buffer(NULL) , m_pos(0) , m_size(0) , m_bCanWrite(FALSE){}

///--------------------------------------------------------------------------------

CWQSG_bufFile::CWQSG_bufFile( void*const buffer , const size_t size , const BOOL bCanWrite )

: m_buffer( (u8*)buffer ) , m_pos(0) , m_size(size) , m_bCanWrite(bCanWrite){}

///--------------------------------------------------------------------------------

CWQSG_bufFile::~CWQSG_bufFile(){	Close();	}

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

	if( NULL == m_hFile )	return FALSE;



	u32 III;

	return( ::ReadFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;

}

///--------------------------------------------------------------------------------

u32		CWQSG_File::Write		( void const*const lpBuffre , const u32 len )

{

	if(NULL == m_hFile)		return FALSE;



	u32 III;

	return( ::WriteFile( m_hFile , lpBuffre , len , &III , NULL ) )?III:0;

}

///--------------------------------------------------------------------------------

void	CWQSG_File::Close		( void )

{

	::CloseHandle(m_hFile);

	m_hFile = NULL;

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

	if( ( NULL == m_hFile )||( offset < 0 ) )		return FALSE;

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

	if(NULL == m_hFile)		return rtCRC;

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

//============================================================================================

BOOL	CWQSG_File::OpenFile	( WCHAR const*const _lpFileName , const DWORD MODE , const DWORD ShareMode )

{

	Close();



	WCHAR const* lpFileName = (WCHAR const*)_lpFileName;

	WCHAR shortPathName[ MAX_PATH ];





	DWORD dwDesiredAccess,dwCreationDisposition,dwShareMode;

	switch(MODE)

	{

	case 1://只读,不存在失败

		dwDesiredAccess = GENERIC_READ;

		dwCreationDisposition = OPEN_EXISTING;

		break;

	case 2://只写,不存在失败

		dwDesiredAccess = GENERIC_WRITE;

		dwCreationDisposition = OPEN_EXISTING;

		break;

	case 3://读写,不存在失败

		dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;

		dwCreationDisposition = OPEN_EXISTING;

		break;

	case 4://重写,存在清空,不存在新建

		dwDesiredAccess = GENERIC_WRITE;

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

		dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;

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

		dwShareMode = NULL;

	}



__gt重试:

	m_hFile = ::CreateFile ( lpFileName , dwDesiredAccess , dwShareMode , NULL , dwCreationDisposition , NULL , NULL );

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

	if( ( NULL == lpBuffre) || (len <= 0) || (m_pointer<0) )		return 0;//错误或无需写出



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

	_CRC32.NEW_CRC32();



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

BOOL	CWQSG_memFile::IsOpen		( void )const{	return TRUE;	}

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

WCHAR*	WQSG_GetTXT_Line	( WCHAR** WTXT_ALL )

{

	if( (WTXT_ALL == NULL) || (NULL == (*WTXT_ALL)) )

		return NULL;



	WCHAR* tmp = *WTXT_ALL;

	while((L'\0' != *tmp) && (0x0D != *tmp) && (0x0A != *tmp))

		++tmp;

	//定位0x0D或0x0A的位置



	INT I = (INT)(tmp - *WTXT_ALL);//获取字数



	BOOL 是否0D = (0x0D == *tmp);



	if( I || *tmp )

	{

		WCHAR* const out = new WCHAR[I + 1];

		WCHAR* s1 = out;

		tmp = *WTXT_ALL;

		//复制

		while(I--)

			*(s1++) = *(tmp++);



		*s1 = L'\0';

		if(*tmp)

		{

			++tmp;

			if(0x0A == *tmp && 是否0D)

				++tmp;

		}

		*WTXT_ALL = tmp;

		return out;

	}

	return NULL;

}

//--------------------------------------------------------------------------------

WQSG_enCP	WQSG_A_U_X		( HANDLE hfile )

{

	if(NULL == hfile)

		return en_CP_NULL;



	::SetFilePointer( hfile , 0 , NULL , FILE_BEGIN );



	DWORD sizeH;

	DWORD sizeL = ::GetFileSize( hfile , &sizeH );

	const s64 size = ( ((s64)sizeH)<<32) | sizeL;

	if( size < 0 )

		return en_CP_NULL;



	if( size >= 2 )

	{

		u32 con_i = 0;

		sizeL = (size >= 3)?3:2;

		if( (!::ReadFile( hfile , &con_i , sizeL , &sizeH , NULL ))

			|| (sizeH != sizeL) )

		{

			return en_CP_NULL;

		}



		switch( con_i )

		{

		case 0xBFBBEF:

			return en_CP_UTF8;

		default:

			switch( con_i & 0xFFFF )

			{

			case 0xFEFF:

				return en_CP_UNICODE;

			case 0xFFFE:

				return en_CP_UNICODE_BIG;

			default:

				break;

			}

		}

	}

	return en_CP_ANSI;

}

//--------------------------------------------------------------------------------

void	C内存文本_W::释放清除()

{

	m_errTXT = NULL;

	m_CP = en_CP_NULL;

	delete[]m_文本;

	m_errTXT = m_文本 = m_TXT = NULL;

}

///--------------------------------------------------------------------------------

BOOL	C内存文本_W::Load( WCHAR const*const 文件路径 , DWORD 允许文件最长长度  , const UINT codePage )

{

	m_errTXT = NULL;

	释放清除();



	::CWQSG_File TXT_File;

	if( ! TXT_File.OpenFile(文件路径,1,3) )

	{

		m_errTXT = L"打开失败";

		return FALSE;

	}

	s64 size = TXT_File.GetFileSize( );

	if(size > (s64)允许文件最长长度)

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

	case en_CP_ANSI:

		TXT_File.Seek(0);

		tmp = new u8 [(DWORD)size + 1];tmp[(DWORD)size] = 0;

		TXT_File.Read( tmp ,(u32)size );

		m_文本 = ::WQSG_char_W( (char*)tmp , codePage );

		delete[]tmp;

		break;

	case en_CP_UNICODE:

		TXT_File.Seek(2);

		tmp = new u8 [(UINT)size];

		size -= 2;

		TXT_File.Read(tmp,(u32)size );

		m_文本 = (WCHAR*)tmp;

		m_文本[(UINT)size>>1] = 0;

		break;

/*	case case en_CP_UNICODE_BIG:

		TXT_File.Seek(2);

		tmp = new u8 [(UINT)size];

		size -= 2;

		TXT_File.Read(tmp,(U32)size );

		m_文本 = (WCHAR*)tmp;

		m_文本[(UINT)size>>1] = 0;

		break;*/

	case en_CP_UTF8:

		TXT_File.Seek(0);

		tmp = new u8 [(DWORD)size + 1];tmp[(DWORD)size] = 0;

		TXT_File.Read( tmp ,(u32)size );

		m_文本 = WQSG_UTF8_W( (char*)tmp );

		delete[]tmp;

		break;

	default:

		m_errTXT = L"不支持的文本格式";

		return FALSE;

	}

	m_TXT = m_文本;

	return TRUE;

}

///--------------------------------------------------------------------------------

WCHAR*	C内存文本_W::取一行文本(){	return ::WQSG_GetTXT_Line(&m_TXT);	}

BOOL	C内存文本_W::关联( WCHAR const*const p文本 )

{

	m_errTXT = NULL;

	if( NULL == p文本 )

	{

		m_errTXT = L"输入文本为空";

		return FALSE;

	}



	释放清除();



	m_CP = en_CP_UNICODE;

	m_TXT = m_文本 = (WCHAR*)p文本;

	return TRUE;

}

WCHAR*	C内存文本_W::切断( void )

{

	m_errTXT = (m_文本==NULL)?L"无文本":NULL;



	WCHAR* pTXT = m_文本;

	m_文本 = NULL;

	释放清除();

	return pTXT;

}

//------------------------------------------------------------------------------

BOOL WQSG_取短路径( WCHAR const*const longPath , WCHAR* shortPath )

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

BOOL WQSG_取短路径文件名( WCHAR const*const longPath , WCHAR* shortPath )

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

//-----------------------------------------------------------------------------

/*

BOOL WQSG_MoveFileEx( WCHAR const*const srcFileName , WCHAR const*const newFileName){

    WCHAR tmp[ MAX_PATH ];

    WCHAR path[ MAX_PATH ] , extName[ MAX_PATH ];

    WCHAR* newname = (WCHAR*)newFileName;

    int i = 0;

_gt_start:

    if(!::MoveFile( srcFileName , newname )){

        DWORD err = ::GetLastError();

        if( ERROR_FILE_EXISTS == err ){

            if(newname != tmp){

                WCHAR B[MAX_PATH],C[MAX_PATH];

                newname = tmp;

                if( WQSG_strlen ( newFileName ) > ( MAX_PATH - 10 ) ){

                    return FALSE;

                }

#if ___SW_屏蔽警告___

#pragma warning(disable: 4996)

#endif

                _wsplitpath( newFileName , path , B , C , extName );

                _wmakepath( path , tmp , B , C , L"" );

            }

            if(3 == swprintf( tmp , L"%s (%u)%s" , path , ++i , extName ))

                goto _gt_start;

#if ___SW_屏蔽警告___

#pragma warning(default: 4996)

#endif

        }

        return FALSE;

    }

    return TRUE;

}

///--------------------------------------------------------------------------------

BOOL WQSG_IsDir( WCHAR const*const path )

{

    if(path[1] != L':')return FALSE;



    DWORD attr = GetFileAttributes( path );

    return ( (INVALID_FILE_ATTRIBUTES != attr) && ( attr & FILE_ATTRIBUTE_DIRECTORY ) );

}

///--------------------------------------------------------------------------------

BOOL WQSG_IsFile( WCHAR const*const path )

{

	if(path[1] != L':')return FALSE;



    DWORD attr = GetFileAttributes( path );

    return ( (INVALID_FILE_ATTRIBUTES != attr) && ( !( attr & FILE_ATTRIBUTE_DIRECTORY ) ) );

}*/

//------------------------------------------------------------------------------


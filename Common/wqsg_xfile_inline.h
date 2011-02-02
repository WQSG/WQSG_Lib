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

#ifndef __WQSG_XFILE_INLINE__
#define __WQSG_XFILE_INLINE__

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline u32		CWQSG_memFile::Read			( void*const lpBuffre , const u32 len )
{
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
}
///--------------------------------------------------------------------------------
inline u32		CWQSG_memFile::Write		( void const*const lpBuffre , const u32 len )
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
inline void	CWQSG_memFile::Close		( void )
{
	::free( m_mem );
	m_mem = NULL;
	m_memSize = m_pointer = m_FileSize = 0;
}
///--------------------------------------------------------------------------------
inline s64		CWQSG_memFile::GetFileSize	( void )const
{
	return (s64)m_FileSize;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_memFile::SetFileLength	( const s64 Length )
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
inline s64		CWQSG_memFile::Tell			( void )const
{
	return ( m_mem != NULL )?m_pointer:0;
}

///--------------------------------------------------------------------------------
inline BOOL	CWQSG_memFile::Seek			( const s64 offset )
{
	if( (u64)offset > (size_t)-1 )
		return FALSE;

	m_pointer = (size_t)offset;

	return TRUE;
}
///--------------------------------------------------------------------------------
inline u32		CWQSG_memFile::GetCRC32		( void )
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
inline BOOL	CWQSG_memFile::IsOpen		( void )const
{
	return TRUE;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_memFile::IsCanRead	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_memFile::IsCanWrite	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_memFile::SetInc		( int inc )
{
	if( inc >= 0 )
	{
		m_inc = (size_t)inc;
		return TRUE;
	}

	return FALSE;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline CWQSG_bufFile::CWQSG_bufFile()
: m_buffer(NULL)
, m_pos(0)
, m_size(0)
, m_bCanWrite(FALSE)
{
}
///--------------------------------------------------------------------------------
inline CWQSG_bufFile::CWQSG_bufFile( void*const buffer , const size_t size , const BOOL bCanWrite )
: m_buffer( (u8*)buffer )
, m_pos(0)
, m_size(size)
, m_bCanWrite(bCanWrite)
{
}
///--------------------------------------------------------------------------------
inline CWQSG_bufFile::~CWQSG_bufFile()
{
	Close();
}
///--------------------------------------------------------------------------------
inline u32		CWQSG_bufFile::Read		( void*const lpBuffre , const u32 len )
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
inline u32		CWQSG_bufFile::Write		( void const*const lpBuffre , const u32 len )
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
inline void	CWQSG_bufFile::Close		( void )
{
	m_buffer = NULL;
	m_bCanWrite = FALSE;
}
///--------------------------------------------------------------------------------
inline s64		CWQSG_bufFile::GetFileSize	( void )const
{
	return ( IsOpen()?m_size:-1 );
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::SetFileLength( const s64 Length )
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
inline s64		CWQSG_bufFile::Tell		( void )const
{
	return( (IsOpen())?(m_pos):(-1) );
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::Seek		( const s64 offset )
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
inline u32		CWQSG_bufFile::GetCRC32	( void )
{
	u32 CRC;

	if( IsOpen() )
	{
		CCrc32	_CRC32;
		_CRC32.NEW_CRC32();

		size_t fSize = m_size;
		size_t offset = 0;

		while( fSize )
		{
			if( fSize > (u32)-1 )
			{

				CRC = _CRC32.GetCrc32( m_buffer + offset , (u32)-1 );
				offset += (u32)-1;
				fSize -= (u32)-1;
			}
			else
			{
				CRC = _CRC32.GetCrc32( m_buffer + offset , (u32)fSize );
				offset += fSize;
				fSize = 0;
			}
		}
	}

	return CRC;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::IsOpen		( void )const
{
	return( m_buffer != NULL );
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::IsCanRead	( void )const
{
	return IsOpen();
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::IsCanWrite	( void )const
{
	return m_bCanWrite;
}
///--------------------------------------------------------------------------------
inline BOOL	CWQSG_bufFile::OpenFile( void*const buffer , const size_t size , const BOOL bCanWrite )
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
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline WCHAR* WQSG_GetTXT_Line( WCHAR** a_pWTXT_ALL )
{
	if( (a_pWTXT_ALL == NULL) || (NULL == (*a_pWTXT_ALL)) )
		return NULL;

	WCHAR* tmp = *a_pWTXT_ALL;

	while((L'\0' != *tmp) && (0x0D != *tmp) && (0x0A != *tmp))
		++tmp;

	//定位0x0D或0x0A的位置

	INT I = (INT)(tmp - *a_pWTXT_ALL);//获取字数

	BOOL bIs0D = (0x0D == *tmp);

	if( I || *tmp )
	{
		WCHAR* const out = new WCHAR[I + 1];
		WCHAR* s1 = out;
		tmp = *a_pWTXT_ALL;

		//复制
		while(I--)
			*(s1++) = *(tmp++);

		*s1 = L'\0';

		if(*tmp)
		{
			++tmp;

			if(0x0A == *tmp && bIs0D)
				++tmp;
		}
		*a_pWTXT_ALL = tmp;
		return out;
	}

	return NULL;
}
//--------------------------------------------------------------------------------

#endif //__WQSG_XFILE_INLINE__

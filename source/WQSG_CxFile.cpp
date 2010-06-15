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
#include "../WQSG_CxFile.h"
//--------------------------------------------------------------------------------------------
int CWQSG_CFILE::fgetc		( void )
{
	int rt = EOF;
	if( IsOpen() )
		rt = ::fgetc( m_fp );
	return rt;
}
int CWQSG_CFILE::fputc		( const int _Ch )
{
	int rt = EOF;
	if( IsOpen() )
		rt = ::fputc( _Ch , m_fp );
	return rt;
}
size_t CWQSG_CFILE::fread		( void *buffer , size_t size , size_t count )
{
	size_t rt = 0;
	if( IsOpen() )
		rt = ::fread( buffer , size , count , m_fp );
	return rt;
}
size_t CWQSG_CFILE::fwrite		( const void *buffer , size_t size , size_t count )
{
	size_t rt = 0;
	if( IsOpen() )
		rt = ::fwrite( buffer , size , count , m_fp );
	return rt;
}
int CWQSG_CFILE::fclose		( void )
{
	int rt = EOF;
	if( IsOpen() )
	{
		rt = ::fclose( m_fp );
		if( rt != EOF )
			m_fp = NULL;
	}
	return EOF;
}
long CWQSG_CFILE::ftell		( void )const
{
	long rt = -1L;
	if( IsOpen() )
		rt = ::ftell( m_fp );
	return rt;
}
int CWQSG_CFILE::fseek		( const long _Offset , const int _Origin )
{
	int rt = EOF;
	if( IsOpen() )
		rt = ::fseek( m_fp , _Offset , _Origin );
	return rt;
}
bool CWQSG_CFILE::IsOpen		( void )const
{
	return bool( m_fp != NULL );
}
///----------------------------------------------------------
bool CWQSG_CFILE::fopen		( const char *filename , const char *mode )
{
	fclose();
	m_fp = ::fopen( filename , mode );
	return IsOpen();
}
///------------------------------------------------------------------------------------
__int64 CWQSG_CFILE::_ftelli64		( void )const
{
	__int64 rt = -1;
	if( IsOpen() )
		rt = ::_ftelli64( m_fp );
	return rt;
}
int CWQSG_CFILE::_fseeki64		( const __int64 _Offset , const int _Origin )
{
	int rt = EOF;
	if( IsOpen() )
		rt = ::_fseeki64( m_fp , _Offset , _Origin );
	return rt;
}
//--------------------------------------------------------------------------------------------
#include <malloc.h>
#define DEF_mem_inc	2
bool CWQSG_CmemFILE::zzz_SetMemSize( const long _size )
{
	if( _size > 0 )
	{
		const long 余数 = _size%DEF_mem_inc;
		size_t 需要len = _size - 余数;
		if( 余数 != 0 )
		{
			if( ( 需要len + DEF_mem_inc ) < DEF_mem_inc )
				return false;

			需要len += DEF_mem_inc;
		}

		if( 需要len == m_memSize )
			return true;

		void* tmp = (unsigned char*)::realloc( m_ptr , 需要len );
		if( tmp != NULL )
		{
			m_ptr = (unsigned char*)tmp;
			m_memSize = 需要len;
			return true;
		}
	}
	return false;
}
//
int CWQSG_CmemFILE::fgetc		( void )
{
	int rt = EOF;
	if( IsOpen() && (m_fileSize >= 0) && (m_offset<m_fileSize) )
		rt = m_ptr[m_offset++];
	return rt;
}
int CWQSG_CmemFILE::fputc		( const int _Ch )
{
	int rt = EOF;
	if( IsOpen() && m_bCanWrite && (m_offset>=0) )
	{
		if( m_offset < m_fileSize )
		{
			m_ptr[m_offset] = _Ch;
			rt = m_ptr[m_offset++];
		}
		else if( m_bCanAppend && ((m_offset+1) > 0) )
		{
			if( (size_t)m_offset >= m_memSize )
				zzz_SetMemSize( m_offset + 1 );

			if( (size_t)m_offset < m_memSize )
			{
				m_ptr[m_offset] = _Ch;
				rt = m_ptr[m_offset];
				m_fileSize = ++m_offset;
			}
		}
	}
	return rt;
}
//
#if 0
size_t CWQSG_CmemFILE::fread		( void *buffer , size_t size , size_t count )
{
	for( size_t last_i = 0 , i = 0 ; i >= last_i ; last_i = i++ )
	{
	}
	return 0;
}
size_t CWQSG_CmemFILE::fwrite		( const void *buffer , size_t size , size_t count )
{
	for( size_t last_i = 0 , i = 0 ; i >= last_i ; last_i = i++ )
	{
	}
	return 0;
}
#endif
//
int CWQSG_CmemFILE::fclose		( void )
{
	int rt = 0;
	if( IsOpen() )
	{
		rt = EOF;
		if( m_memSize > 0 )
		{
			::free( m_ptr );
			m_memSize = 0;
		}

		m_ptr = NULL;
		m_bCanAppend = m_bCanWrite = false;

		rt = 0;
	}

	return rt;
}
long CWQSG_CmemFILE::ftell		( void )const
{
	long rt = EOF;
	if( IsOpen() && (m_offset >= 0) )
		rt = m_offset;
	return rt;
}
int CWQSG_CmemFILE::fseek		( const long _Offset , const int _Origin )
{
	int rt = EOF;
	if( IsOpen() )
	{
		switch( _Origin )
		{
		case SEEK_CUR:
			if( ( _Offset + m_offset ) >= 0 )
			{
				m_offset += _Offset;
				rt = 0;
			}
			break;
		case SEEK_SET:
			if( _Offset >= 0 )
			{
				m_offset = _Offset;
				rt = 0;
			}
			break;
		case SEEK_END:
			if( (m_fileSize + _Offset) >= 0 )
			{
				m_offset = m_fileSize + _Offset;
				rt = 0;
			}
			break;
		default:
			break;
		}
	}
	return rt;
}
bool CWQSG_CmemFILE::IsOpen		( void )const
{
	return bool(m_ptr != NULL);
}
///-------------------------------------------------------
bool CWQSG_CmemFILE::fopen		( const void* buffer , const long size , const bool bCanWrite )
{
	fclose();
	if( (buffer != NULL) && (size >= 0) )
	{
		m_ptr = (unsigned char*)buffer;

		m_bCanWrite = bCanWrite;
		m_fileSize = size;

		return true;
	}
	return false;
}

bool CWQSG_CmemFILE::fopen		( void )
{
	if( ( 0 == fclose() ) && ( sizeof(size_t) >= sizeof(long) ) && zzz_SetMemSize( DEF_mem_inc ) )
	{
		m_bCanAppend = m_bCanWrite = true;
		return true;
	}
	return false;
}



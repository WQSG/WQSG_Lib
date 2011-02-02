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
#ifndef WQSG_File_M_H__
#define WQSG_File_M_H__
#pragma once
#include "../Interface/wqsg_def.h"
#include<windows.h>


#include "WQSG_File.h"
#include "../Common/WQSG_String.h"

/******************************************************************

******************************************************************/
class WQSG_File_M
{
	DWORD W_设定的分页;  //内存分页
	HANDLE W_hFile_M;//映射句柄
protected:
	::CWQSG_File m_File;
	s64 m_nFileSize ,
		W_文件偏移 ; //文件偏移
	DWORD W_分页的小偏移 ,//分页的小偏移
		W_分页的小偏移界限 ;//分页的小偏移 < 界限 的数据才能用
	PBYTE W_pbFile ,
		W_映射界限 ;//W_pbFile < 界限 的数据才能用
public:
	void Set文件长度(u64 长度){m_nFileSize = 长度;}
	s64 tell(void) const
	{
		return ( W_文件偏移 + W_分页的小偏移 );
	}
	PBYTE GetpbFile(void) const
	{
		return W_pbFile;
	}
	s64 Get大偏移() const
	{
		return W_文件偏移;
	}
	DWORD Get小偏移(void) const
	{
		return W_分页的小偏移;
	}
	DWORD Get小偏移界限(void) const
	{
		return W_分页的小偏移界限;
	}
	PBYTE Get映射界限(void) const
	{
		return W_映射界限;
	}
	s64 GetFileSize(void) const
	{
		return m_nFileSize;
	}
	void 初始化(void)
	{
		if(W_pbFile)
		{
			::UnmapViewOfFile(W_pbFile);
			W_pbFile = NULL;
		}
		if(W_hFile_M)
		{
			::CloseHandle(W_hFile_M);
			W_hFile_M = NULL;
		}
		m_File.Close();
		W_映射界限 = W_pbFile = NULL;
		m_nFileSize = W_文件偏移 = 0; //文件长度
		W_分页的小偏移界限 = W_分页的小偏移 = 0;
	}
	BOOL 到下一个映射段()
	{
		return SeekTo( W_文件偏移 + W_设定的分页 );
	}
//////////------------------------------------------------------------------------------------------------
	WQSG_File_M()
		:W_hFile_M(NULL)
		,W_pbFile(NULL)
	{
		SYSTEM_INFO sinf;
		::GetSystemInfo(&sinf);
		W_设定的分页 = sinf.dwAllocationGranularity * 512; //设置缓冲区域页 为32MB
		初始化();
	}
	virtual	~WQSG_File_M()
	{
		if(W_pbFile)
			::UnmapViewOfFile(W_pbFile);
		if(W_hFile_M)
			::CloseHandle(W_hFile_M);
	}
////////////////////////////////////////////////////////////////////
	BOOL OpenFile( const WCHAR* lpFileName,UINT ShareMode = 0)
	{
		初始化();

		if(!m_File.OpenFile(lpFileName,1,ShareMode))
		{
			//如果打开失败
			return FALSE;
		}

		m_nFileSize = m_File.GetFileSize();
		if(0 == m_nFileSize){
			初始化();
			return FALSE;
		}
		
		W_hFile_M = ::CreateFileMapping( m_File.GetFileHANDLE() , NULL ,PAGE_READONLY ,
			(DWORD)(m_nFileSize>>32) ,(DWORD)(m_nFileSize&((DWORD)-1)) , NULL);
		if(NULL == W_hFile_M)
		{
			初始化();
			return FALSE;
		}
	///////////////////////////////////////////////////////////////////////////

		W_分页的小偏移界限 = (m_nFileSize < (s64)W_设定的分页)?(DWORD)m_nFileSize:W_设定的分页;

		W_pbFile = (PBYTE)::MapViewOfFile(W_hFile_M,FILE_MAP_READ,0,0,W_分页的小偏移界限);
		
		if(NULL == W_pbFile)
		{
			初始化();
			return FALSE;
		}
		W_映射界限 = W_pbFile + W_分页的小偏移界限;
		return TRUE;
	}
///////////////////////////////////////////////////////////////////////////////////////
	BOOL SeekTo( s64 point )
	{
		if( point >= m_nFileSize )
			return FALSE;

		u64 原地址 = tell();
		::UnmapViewOfFile(W_pbFile);
		W_映射界限 = NULL;

		W_分页的小偏移 = (DWORD)(point & (W_设定的分页 - 1));

		W_文件偏移 = point - W_分页的小偏移;

		s64 剩余字节 = m_nFileSize - W_文件偏移;

		W_分页的小偏移界限 = (剩余字节 < (W_设定的分页))?(DWORD)剩余字节:(W_设定的分页);

		W_pbFile = (PBYTE)::MapViewOfFile( W_hFile_M , FILE_MAP_READ , (DWORD)(W_文件偏移>>32) ,
			(DWORD)( W_文件偏移&((DWORD)-1)) , W_分页的小偏移界限 );

		if( NULL == W_pbFile )
		{
			SeekTo(原地址);
			return FALSE;
		}
		W_映射界限 = W_pbFile + W_分页的小偏移界限;
		return TRUE;
	}
	BOOL 计算CRC32(u32& CRC)
	{
		CRC = 0;
		if(NULL == W_pbFile)
			return FALSE;

		s64 地址 = tell();
		SeekTo(0);
		s64 size = m_nFileSize;

		CCrc32 _crc32;
		_crc32.NEW_CRC32();
		while(size)
		{
			size -= (u32)(W_映射界限 - W_pbFile);
			CRC = _crc32.GetCrc32(W_pbFile,(u32)(W_映射界限 - W_pbFile));
			到下一个映射段();
		}
		return SeekTo(地址);
	}
///////////////////////////////////////////////////////////////////////////////////////
};
class WQSG_FM_FILE : public WQSG_File_M
{
public:
	WQSG_FM_FILE(){}
	virtual	~WQSG_FM_FILE(){}
	UINT GetUCHAR( UINT 长度,const u8* 缓冲 )
	{
		u8* s1 = (u8*)缓冲;
		UINT 实际长度 = 长度;
		while( 长度 )
		{
			DWORD	剩余长度 = W_分页的小偏移界限 - W_分页的小偏移;
			if( !剩余长度 )
			{
				if(!到下一个映射段())
					break;

				剩余长度 = W_分页的小偏移界限 - W_分页的小偏移;
			}
			if( 剩余长度 > 长度 )
				剩余长度 = 长度;
			memcpy( s1 , W_pbFile + W_分页的小偏移 , 剩余长度 );
			s1 += 剩余长度 ;
			W_分页的小偏移 += 剩余长度;
			长度 -= 剩余长度;
		}
		实际长度 -= 长度 ;
		return 实际长度;
	}
	BOOL BACK( UINT SEEK )
	{
		if(W_分页的小偏移 >= SEEK)
		{
			//足够后退
			W_分页的小偏移 -= SEEK;
			return TRUE;
		}
		else
		{
			return SeekTo((W_分页的小偏移界限 + W_文件偏移 - SEEK));
		}
	}
	BOOL SeekADD( UINT SEEK )
	{
		W_分页的小偏移 += SEEK;
		if(W_分页的小偏移 < W_分页的小偏移界限)
		{
			//足够前进
			return TRUE;
		}
		else
		{
			return 到下一个映射段();
		}
	}
};
class WQSG_File_mem
{
	DWORD		m_内存颗粒;
	CWQSG_File	m_fp;
	HANDLE		m_hFile_M;
protected:
	PBYTE		m_pbFile;
	s64			m_FileSize;
public:
	WQSG_File_mem()
		:m_hFile_M( NULL ) , m_pbFile( NULL ) , m_FileSize( 0 )
	{
		SYSTEM_INFO sinf;
		::GetSystemInfo(&sinf);
		m_内存颗粒 = sinf.dwAllocationGranularity;
	}
	virtual	~WQSG_File_mem()
	{
		if(m_pbFile)
			::UnmapViewOfFile(m_pbFile);

		::CloseHandle(m_hFile_M);
	}
	BOOL OpenFile( const WCHAR* lpFileName , UINT ShareMode = 0 )
	{
		Close();
		if(!m_fp.OpenFile( lpFileName , 1 ,ShareMode ) )
			return FALSE;//如果打开失败

		m_FileSize = m_fp.GetFileSize( );
		if( 0 == m_FileSize )
		{
			m_fp.Close();
			return FALSE;
		}

		m_hFile_M = ::CreateFileMapping( m_fp.GetFileHANDLE() , NULL ,PAGE_READONLY ,
			(DWORD)( m_FileSize>>32 ) ,(DWORD)( m_FileSize & 0xFFFFFFFF ) , NULL );

		if( NULL == m_hFile_M )
		{
			m_FileSize = 0;
			m_fp.Close();
			return FALSE;
		}
	///////////////////////////////////////////////////////////////////////////
		return TRUE;
	}
	u8 const* const GetPtr( s64 stratPos , u32 len )
	{
		if( ( m_pbFile ) || ( ( stratPos + len ) > m_FileSize ) || stratPos < 0 )
			return NULL;

		DWORD stratOffset = (DWORD)(stratPos%m_内存颗粒);
		stratPos -= stratOffset;

		DWORD 实际len = len + stratOffset;

		m_pbFile = (PBYTE)::MapViewOfFile( m_hFile_M , FILE_MAP_READ , (DWORD)(stratPos>>32) , (DWORD)(stratPos&(0xFFFFFFFF)) , 实际len );
		if( NULL == m_pbFile )
			return NULL;

		return (m_pbFile + stratOffset);
	}
	s64 GetFileSize( ) const
	{
		return m_fp.GetFileSize( );
	}
	void Close( void )
	{
		Free( );

		::CloseHandle( m_hFile_M );
		m_hFile_M = NULL;

		m_fp.Close( );
		m_FileSize = 0;
	}
	void Free( void )
	{
		if( m_pbFile )
		{
			::UnmapViewOfFile(m_pbFile);
			m_pbFile = NULL;
		}
	}
	BOOL GetCrc32( u32& _CRC )
	{
		_CRC = 0;
		if( NULL == m_hFile_M )
			return FALSE;

		s64 size = m_FileSize;
		CCrc32 _crc32;
		_crc32.NEW_CRC32();

		u64 stratPos = 0;
		const u32 prelen = 0x2000000 - (m_内存颗粒%m_内存颗粒);
		while( size )
		{
			u32 len = ( size > prelen )?prelen:(u32)size;

			PBYTE pbFile = (PBYTE)::MapViewOfFile( m_hFile_M , FILE_MAP_READ , (DWORD)(stratPos>>32) , (DWORD)(stratPos&(0xFFFFFFFF)) , len );
			if( NULL == pbFile )
				return FALSE;

			size -= len;
			stratPos += len;

			_CRC = _crc32.GetCrc32( pbFile , len );

			::UnmapViewOfFile( pbFile );
		}
		return TRUE;
	}
	//
};

#endif
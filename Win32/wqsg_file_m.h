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
	DWORD W_�趨�ķ�ҳ;  //�ڴ��ҳ
	HANDLE W_hFile_M;//ӳ����
protected:
	::CWQSG_File m_File;
	s64 m_nFileSize ,
		W_�ļ�ƫ�� ; //�ļ�ƫ��
	DWORD W_��ҳ��Сƫ�� ,//��ҳ��Сƫ��
		W_��ҳ��Сƫ�ƽ��� ;//��ҳ��Сƫ�� < ���� �����ݲ�����
	PBYTE W_pbFile ,
		W_ӳ����� ;//W_pbFile < ���� �����ݲ�����
public:
	void Set�ļ�����(u64 ����){m_nFileSize = ����;}
	s64 tell(void) const
	{
		return ( W_�ļ�ƫ�� + W_��ҳ��Сƫ�� );
	}
	PBYTE GetpbFile(void) const
	{
		return W_pbFile;
	}
	s64 Get��ƫ��() const
	{
		return W_�ļ�ƫ��;
	}
	DWORD GetСƫ��(void) const
	{
		return W_��ҳ��Сƫ��;
	}
	DWORD GetСƫ�ƽ���(void) const
	{
		return W_��ҳ��Сƫ�ƽ���;
	}
	PBYTE Getӳ�����(void) const
	{
		return W_ӳ�����;
	}
	s64 GetFileSize(void) const
	{
		return m_nFileSize;
	}
	void ��ʼ��(void)
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
		W_ӳ����� = W_pbFile = NULL;
		m_nFileSize = W_�ļ�ƫ�� = 0; //�ļ�����
		W_��ҳ��Сƫ�ƽ��� = W_��ҳ��Сƫ�� = 0;
	}
	BOOL ����һ��ӳ���()
	{
		return SeekTo( W_�ļ�ƫ�� + W_�趨�ķ�ҳ );
	}
//////////------------------------------------------------------------------------------------------------
	WQSG_File_M()
		:W_hFile_M(NULL)
		,W_pbFile(NULL)
	{
		SYSTEM_INFO sinf;
		::GetSystemInfo(&sinf);
		W_�趨�ķ�ҳ = sinf.dwAllocationGranularity * 512; //���û�������ҳ Ϊ32MB
		��ʼ��();
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
		��ʼ��();

		if(!m_File.OpenFile(lpFileName,1,ShareMode))
		{
			//�����ʧ��
			return FALSE;
		}

		m_nFileSize = m_File.GetFileSize();
		if(0 == m_nFileSize){
			��ʼ��();
			return FALSE;
		}
		
		W_hFile_M = ::CreateFileMapping( m_File.GetFileHANDLE() , NULL ,PAGE_READONLY ,
			(DWORD)(m_nFileSize>>32) ,(DWORD)(m_nFileSize&((DWORD)-1)) , NULL);
		if(NULL == W_hFile_M)
		{
			��ʼ��();
			return FALSE;
		}
	///////////////////////////////////////////////////////////////////////////

		W_��ҳ��Сƫ�ƽ��� = (m_nFileSize < (s64)W_�趨�ķ�ҳ)?(DWORD)m_nFileSize:W_�趨�ķ�ҳ;

		W_pbFile = (PBYTE)::MapViewOfFile(W_hFile_M,FILE_MAP_READ,0,0,W_��ҳ��Сƫ�ƽ���);
		
		if(NULL == W_pbFile)
		{
			��ʼ��();
			return FALSE;
		}
		W_ӳ����� = W_pbFile + W_��ҳ��Сƫ�ƽ���;
		return TRUE;
	}
///////////////////////////////////////////////////////////////////////////////////////
	BOOL SeekTo( s64 point )
	{
		if( point >= m_nFileSize )
			return FALSE;

		u64 ԭ��ַ = tell();
		::UnmapViewOfFile(W_pbFile);
		W_ӳ����� = NULL;

		W_��ҳ��Сƫ�� = (DWORD)(point & (W_�趨�ķ�ҳ - 1));

		W_�ļ�ƫ�� = point - W_��ҳ��Сƫ��;

		s64 ʣ���ֽ� = m_nFileSize - W_�ļ�ƫ��;

		W_��ҳ��Сƫ�ƽ��� = (ʣ���ֽ� < (W_�趨�ķ�ҳ))?(DWORD)ʣ���ֽ�:(W_�趨�ķ�ҳ);

		W_pbFile = (PBYTE)::MapViewOfFile( W_hFile_M , FILE_MAP_READ , (DWORD)(W_�ļ�ƫ��>>32) ,
			(DWORD)( W_�ļ�ƫ��&((DWORD)-1)) , W_��ҳ��Сƫ�ƽ��� );

		if( NULL == W_pbFile )
		{
			SeekTo(ԭ��ַ);
			return FALSE;
		}
		W_ӳ����� = W_pbFile + W_��ҳ��Сƫ�ƽ���;
		return TRUE;
	}
	BOOL ����CRC32(u32& CRC)
	{
		CRC = 0;
		if(NULL == W_pbFile)
			return FALSE;

		s64 ��ַ = tell();
		SeekTo(0);
		s64 size = m_nFileSize;

		CCrc32 _crc32;
		_crc32.NEW_CRC32();
		while(size)
		{
			size -= (u32)(W_ӳ����� - W_pbFile);
			CRC = _crc32.GetCrc32(W_pbFile,(u32)(W_ӳ����� - W_pbFile));
			����һ��ӳ���();
		}
		return SeekTo(��ַ);
	}
///////////////////////////////////////////////////////////////////////////////////////
};
class WQSG_FM_FILE : public WQSG_File_M
{
public:
	WQSG_FM_FILE(){}
	virtual	~WQSG_FM_FILE(){}
	UINT GetUCHAR( UINT ����,const u8* ���� )
	{
		u8* s1 = (u8*)����;
		UINT ʵ�ʳ��� = ����;
		while( ���� )
		{
			DWORD	ʣ�೤�� = W_��ҳ��Сƫ�ƽ��� - W_��ҳ��Сƫ��;
			if( !ʣ�೤�� )
			{
				if(!����һ��ӳ���())
					break;

				ʣ�೤�� = W_��ҳ��Сƫ�ƽ��� - W_��ҳ��Сƫ��;
			}
			if( ʣ�೤�� > ���� )
				ʣ�೤�� = ����;
			memcpy( s1 , W_pbFile + W_��ҳ��Сƫ�� , ʣ�೤�� );
			s1 += ʣ�೤�� ;
			W_��ҳ��Сƫ�� += ʣ�೤��;
			���� -= ʣ�೤��;
		}
		ʵ�ʳ��� -= ���� ;
		return ʵ�ʳ���;
	}
	BOOL BACK( UINT SEEK )
	{
		if(W_��ҳ��Сƫ�� >= SEEK)
		{
			//�㹻����
			W_��ҳ��Сƫ�� -= SEEK;
			return TRUE;
		}
		else
		{
			return SeekTo((W_��ҳ��Сƫ�ƽ��� + W_�ļ�ƫ�� - SEEK));
		}
	}
	BOOL SeekADD( UINT SEEK )
	{
		W_��ҳ��Сƫ�� += SEEK;
		if(W_��ҳ��Сƫ�� < W_��ҳ��Сƫ�ƽ���)
		{
			//�㹻ǰ��
			return TRUE;
		}
		else
		{
			return ����һ��ӳ���();
		}
	}
};
class WQSG_File_mem
{
	DWORD		m_�ڴ����;
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
		m_�ڴ���� = sinf.dwAllocationGranularity;
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
			return FALSE;//�����ʧ��

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

		DWORD stratOffset = (DWORD)(stratPos%m_�ڴ����);
		stratPos -= stratOffset;

		DWORD ʵ��len = len + stratOffset;

		m_pbFile = (PBYTE)::MapViewOfFile( m_hFile_M , FILE_MAP_READ , (DWORD)(stratPos>>32) , (DWORD)(stratPos&(0xFFFFFFFF)) , ʵ��len );
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
		const u32 prelen = 0x2000000 - (m_�ڴ����%m_�ڴ����);
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
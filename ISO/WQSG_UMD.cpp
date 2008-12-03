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
//#include "StdAfx.h"
#include "WQSG_UMD.h"
//--------------------------------------------------------------------------------------------------
static const unsigned char fileLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x0D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const unsigned char dirLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#define DEF_FN_make_DirLen( __def_nameLen ) (0x21 + ((((__def_nameLen)&1)==1)?(__def_nameLen):((__def_nameLen)+1)) + sizeof(fileLastData))
//--------------------------------------------------------------------------------------------------
CWQSG_UMD::CWQSG_UMD(void){	}
CWQSG_UMD::~CWQSG_UMD(void){	CloseISO();}
s32 CWQSG_UMD::zzz_FindFile( const _tISO_DirEnt& tDirEnt_in , s32 offset , char const*const strFileName , _tISO_DirEnt& tDirEnt  , const bool bFindFree )
{
	if( (0 != ::WQSG_ISO_ReadDirEnt( &m_isoFile , &tDirEnt_in , 0 , &tDirEnt , NULL , false )) ||
		(tDirEnt.len != 0x30) || (tDirEnt.attr !=2 ) || (tDirEnt.nameLen != 1) ||
		(tDirEnt.lba_le != tDirEnt_in.lba_le) || (tDirEnt_in.size_le != tDirEnt_in.size_le) ||
		(0x30 != ::WQSG_ISO_ReadDirEnt( &m_isoFile , &tDirEnt_in , 0x30 , &tDirEnt ,NULL , false )) ||
		(tDirEnt.len != 0x30) || (tDirEnt.attr != 2) || (tDirEnt.nameLen != 1)
		)
	{
		DEF_ERRMSG( m_isoFile , L"��������" );
		return -1;
	}

	CStringA fileName;
	if( strFileName )
	{
		fileName = strFileName;
		if( fileName.GetLength() <= 0 )
		{
			DEF_ERRMSG( m_isoFile , L"���ܲ��ҿ��ļ���" );
			return -1;
		}
		fileName.MakeLower();
	}

	char nameBuffer[256];
	for( ; offset < tDirEnt_in.size_le ; )
	{
		{
			const s32 newOffset( ::WQSG_ISO_ReadDirEnt( &m_isoFile , &tDirEnt_in , offset , &tDirEnt , nameBuffer , bFindFree ) );
			if( newOffset < 0 )
				break;

			offset = newOffset;
		}

		if( tDirEnt.len == 0 )
		{
			if( strFileName )
				break;

			return offset;
		}
		else if( fileName )
		{
			CStringA name( nameBuffer );
			if( fileName == name.MakeLower() )
				return offset;
		}
		offset += tDirEnt.len;
	}
	DEF_ERRMSG( m_isoFile , L"û�ҵ��ļ�" );
	return -1;
}
s32 CWQSG_UMD::FindFile( const _tISO_DirEnt& tDirEnt_in , char const*const strFileName , _tISO_DirEnt& tDirEnt )
{
	return zzz_FindFile( tDirEnt_in , 0x60 , strFileName , tDirEnt , false );
}
BOOL CWQSG_UMD::OpenISO( WCHAR const*const isoPathName  , const BOOL bCanWrite )
{
	if( isoPathName && ::WQSG_ISO_Open( &m_isoFile , isoPathName , bCanWrite  ) )
	{
		if( m_isoFile.m_tHead.FileStructureVersion == 2 )
			return TRUE;
		DEF_ERRMSG( m_isoFile , L"�ļ��ṹ�汾��Ϊ 2" );
	}

	CloseISO();
	return FALSE;
}
void CWQSG_UMD::CloseISO()
{
	::WQSG_ISO_Close( &m_isoFile );
}
//--------------------------------------------------------------------------------------------------
static void memcpyR( void* dst , const void* src , const size_t size )
{
	u8* dp = (u8*)dst;
	const u8* sp = (const u8*)src;

	for( size_t i = 0 ; i < size ; ++i )
		dp[i] = sp[size-i-1];
}
//--------------------------------------------------------------------------------------------------
#define DEF_FN_toLBA( __def_size , __def_LBA ) ((__def_size/__def_LBA) + (((__def_size%__def_LBA)>0)?1:0))
BOOL CWQSG_UMD::WriteFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
						  , const void* buffer , const s32 buflen , const s32 insertOffset , const BOOL isNew )
{
	return WriteFile( tDirEnt_in , fileName , CWQSG_bufFile( (void*)buffer , buflen , FALSE ) , buflen , insertOffset , isNew , FALSE );
}
//--------------------------------------------------------------------------------------------------
BOOL CWQSG_UMD::WriteFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
						  , CWQSG_xFile& buffp , const s32 buflen , const s32 insertOffset , const BOOL isNew , const BOOL isDir )
{
	if( (!m_isoFile.m_bCanWrite) ||
		( !buffp.IsOpen() ) || ( buflen < 0 ) ||
		(fileName == NULL) || ( strlen(fileName) > 120 ) ||
		( insertOffset < 0 ) || ( (insertOffset+buflen) < 0 )
		)
	{
		DEF_ERRMSG( m_isoFile , L"��������" );
		return FALSE;
	}
	//--------------------------------------------------------------------------------------
	const int Ԥ������ = DEF_FN_make_DirLen( strlen(fileName) );
	if( Ԥ������ > 250 )
	{
		DEF_ERRMSG( m_isoFile , L"Ԥ�����ȳ���250" );
		return FALSE;
	}
	//-----------------------------------------------------------------
	_tISO_DirEnt dirEnt;

	bool �� = false;
	s32 dirOffset ;
//	dirOffset = FindFile( tDirEnt_in , fileName , dirEnt );


	s32 ����fileSize = insertOffset + buflen;
	if( ( dirOffset = FindFile( tDirEnt_in , fileName , dirEnt ) ) >= 0 )
	{
		if( dirEnt.attr & 2 )
		{
			if( isDir )
				return TRUE;

			DEF_ERRMSG( m_isoFile , L"����дĿ¼" );
			return FALSE;
		}
		else if( isDir )
		{
			DEF_ERRMSG( m_isoFile , L"����Ŀ¼ʧ��,�Ѵ���ͬ���ļ�" );
			return FALSE;
		}

		if( dirEnt.len != (u8)Ԥ������ )
		{
			DEF_ERRMSG( m_isoFile , L"Ŀ¼��� �� Ԥ�����Ȳ���" );
			return FALSE;
		}

		if( !isNew )
			����fileSize = ((����fileSize>dirEnt.size_le)?(����fileSize):dirEnt.size_le);

		const s32 ��Ҫ��LBA = DEF_FN_toLBA( ����fileSize , m_isoFile.m_tHead.LB_Size_LE );
		const s32 ӵ��LBA = DEF_FN_toLBA( dirEnt.size_le , m_isoFile.m_tHead.LB_Size_LE );

		if( ӵ��LBA < ��Ҫ��LBA )
		{
			if( !m_isoFile.m_pLBA_List->�ͷ�( dirEnt.lba_le ) )
			{
				DEF_ERRMSG( m_isoFile , L"�ͷ�LBAʧ��" );
				return FALSE;
			}
			const s32 LBA_Pos = m_isoFile.m_pLBA_List->����( ��Ҫ��LBA );
			if( LBA_Pos < 0 )
			{
				DEF_ERRMSG( m_isoFile , L"����LBAʧ��" );
				return FALSE;
			}

			dirEnt.lba_le = LBA_Pos;
			memcpyR( &dirEnt.lba_be , &dirEnt.lba_le , sizeof(dirEnt.lba_le) );
		}
	}
	else if( isNew )
	{
		if( insertOffset != 0 )
		{
			DEF_ERRMSG( m_isoFile , L"����ļ�ֻ�ܴ�ƫ�� 0 ��ʼ" );
			return FALSE;
		}

		const s32 ��Ҫ��LBA = ((DEF_FN_toLBA( buflen , m_isoFile.m_tHead.LB_Size_LE ))==0)?1:(DEF_FN_toLBA( buflen , m_isoFile.m_tHead.LB_Size_LE ));

		�� = true;

		dirOffset = 0x60;
		s32 ���޳���;
__gtReTest:
		if( ( dirOffset = zzz_FindFile( tDirEnt_in , dirOffset , NULL , dirEnt , true ) ) < 0 )
			return FALSE;

		���޳��� = dirOffset - ( dirOffset % m_isoFile.m_tHead.LB_Size_LE ) + m_isoFile.m_tHead.LB_Size_LE;

		if(	( ���޳��� <= dirOffset ) || ( ���޳��� > tDirEnt_in.size_le ) )
		{
			DEF_ERRMSG( m_isoFile , L"Ŀ¼�ռ䲻��" );
			return FALSE;
		}
		if( (���޳��� - dirOffset) < Ԥ������ )
		{
			dirOffset = (���޳��� += m_isoFile.m_tHead.LB_Size_LE);
			goto __gtReTest;
		}

		memset( &dirEnt , 0 , sizeof(dirEnt) );

		dirEnt.len = (u8)Ԥ������;
	//	dirEnt.len_ex;
		{
			const s32 LBA_Pos = m_isoFile.m_pLBA_List->����( ��Ҫ��LBA );
			if( LBA_Pos < 0 )
			{
				DEF_ERRMSG( m_isoFile , L"����LBAʧ��" );
				return FALSE;
			}

			dirEnt.lba_le = LBA_Pos;
			memcpyR( &dirEnt.lba_be , &dirEnt.lba_le , sizeof(dirEnt.lba_le) );
		}

		{
			unsigned char data[7] = { 0x6C, 0x03, 0x19, 0x11, 0x34, 0x0B, 0x00 };
			memcpy( dirEnt.time , data , sizeof(data) );
		}
	//	dirEnt.attr;	dirEnt.sp1;	dirEnt.sp2;

		dirEnt.sp3_le = 1;
		memcpyR( &dirEnt.sp3_be , &dirEnt.sp3_le , sizeof(dirEnt.sp3_be) );

		dirEnt.nameLen = strlen(fileName);
		//----------------------------------------------------------
	}
	else
	{
		DEF_ERRMSG( m_isoFile , L"û�ҵ��ļ�" );
		return FALSE;
	}
	//--------------------------------------------------------------------------------
	dirEnt.size_le = isDir?(m_isoFile.m_tHead.LB_Size_LE):(����fileSize);

	memcpyR( &dirEnt.size_be , &dirEnt.size_le , sizeof(dirEnt.size_le) );
	//------------------------------------------------------------------------------------------------------------
	if( isDir )
	{
		dirEnt.attr = 2;

		m_isoFile.m_ISOfp.Seek( dirEnt.lba_le * m_isoFile.m_tHead.LB_Size_LE + insertOffset );

		u8 data[] = {
			0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
			0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
			0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
			0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
			0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		_tISO_DirEnt* pEnt;

		pEnt = (_tISO_DirEnt*)(data);
		memcpy( pEnt , &dirEnt , sizeof(_tISO_DirEnt) - 1 );
		pEnt->len = 0x30;

		pEnt = (_tISO_DirEnt*)(data+0x30);
		memcpy( pEnt , &tDirEnt_in , sizeof(_tISO_DirEnt) - 1 );
		pEnt->len = 0x30;

		if( sizeof(data) != m_isoFile.m_ISOfp.Write( data , sizeof(data) ) )
			return FALSE;

	}
	else if( buflen > 0 )
	{
		m_isoFile.m_ISOfp.Seek( dirEnt.lba_le * m_isoFile.m_tHead.LB_Size_LE + insertOffset );
		CWD_BIN bin;bin.LEN = ((buflen<(1024*1024*32))?buflen:(1024*1024*32));
		if( NULL == bin.SetBufSize( bin.LEN ) )
			return FALSE;

		for( u32 len = buflen ; len > 0 ; )
		{
			const u32 ��Ҫ��ȡ = (len > bin.LEN)?bin.LEN:len;
			len -= ��Ҫ��ȡ;

			if( ��Ҫ��ȡ != buffp.Read( bin.BIN , ��Ҫ��ȡ ) )
				return FALSE;

			if( ��Ҫ��ȡ != m_isoFile.m_ISOfp.Write( bin.BIN , ��Ҫ��ȡ ) )
				return FALSE;
		}
	}
/*
	if( int ���� = len%m_isoFile.m_tHead.LB_Size_LE )
	{
		���� = m_isoFile.m_tHead.LB_Size_LE - ����;
		u8* tmp = new u8[����];
		if( NULL != tmp )
		{
			memset( tmp , 0xff , ���� );
			m_isoFile.m_ISOfp.Write( tmp , ���� );
			delete[]tmp;
		}
	}*/
	//------------------------------------------------------------------------------------------------------------
	m_isoFile.m_ISOfp.Seek( tDirEnt_in.lba_le * m_isoFile.m_tHead.LB_Size_LE + dirOffset );
	if( sizeof(dirEnt) != m_isoFile.m_ISOfp.Write( &dirEnt , sizeof(dirEnt) ) )
	{
		DEF_ERRMSG( m_isoFile , L"дĿ¼��ʧ��" );
		return FALSE;
	}

	if( �� )
	{
//		fileName.MakeUpper();
		const u32 д���� = (dirEnt.nameLen&1)?dirEnt.nameLen:(dirEnt.nameLen+1);
		if( ( д���� != m_isoFile.m_ISOfp.Write( fileName , д���� ) ) ||
			( sizeof((isDir?dirLastData:fileLastData)) != m_isoFile.m_ISOfp.Write( (isDir?dirLastData:fileLastData) , sizeof((isDir?dirLastData:fileLastData)) ) ) )
		{
			DEF_ERRMSG( m_isoFile , L"дĿ¼��ʧ��" );
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
BOOL CWQSG_UMD::ReadFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
						 , CWQSG_xFile& buffp , const s32 buflen , const s32 startOffset )
{
	if( ( !buffp.IsOpen() ) || ( buflen < 0 ) ||
		( fileName == NULL ) || ( strlen(fileName) > 120 ) ||
		( startOffset < 0 ) || ( (startOffset+buflen) < 0 )
		)
	{
		DEF_ERRMSG( m_isoFile , L"��������" );
		return FALSE;
	}

	//--------------------------------------------------------------------------------------
	const int Ԥ������ = DEF_FN_make_DirLen( strlen(fileName) );
	if( Ԥ������ > 250 )
	{
		DEF_ERRMSG( m_isoFile , L"dirԤ�����ȳ���250" );
		return FALSE;
	}

	//-----------------------------------------------------------------
	_tISO_DirEnt dirEnt;
	s32 dirOffset;

	if( ( dirOffset = FindFile( tDirEnt_in , fileName , dirEnt ) ) < 0 )
		return FALSE;

	if( dirEnt.len != (u8)Ԥ������ )
	{
		DEF_ERRMSG( m_isoFile , L"dirʵ�ʳ��� != Ԥ������" );
		return FALSE;
	}

	if( ( dirEnt.size_le - startOffset ) < buflen )
	{
		DEF_ERRMSG( m_isoFile , L"�ļ����Ȳ���" );
		return FALSE;
	}

	m_isoFile.m_ISOfp.Seek( dirEnt.lba_le * m_isoFile.m_tHead.LB_Size_LE + startOffset );


	CWD_BIN bin;bin.LEN = ((buflen<(1024*1024*32))?buflen:(1024*1024*32));
	if( NULL == bin.SetBufSize( bin.LEN ) )
	{
		DEF_ERRMSG( m_isoFile , L"�����ڴ�ʧ��" );
		return FALSE;
	}

	for( u32 len = buflen ; len > 0 ; )
	{
		const u32 ��Ҫ��ȡ = (len > bin.LEN)?bin.LEN:len;
		len -= ��Ҫ��ȡ;

		if( ��Ҫ��ȡ != m_isoFile.m_ISOfp.Read( bin.BIN , ��Ҫ��ȡ ) )
		{
			DEF_ERRMSG( m_isoFile , L"��ISO�ļ�ʧ��" );
			return FALSE;
		}

		if( ��Ҫ��ȡ != buffp.Write( bin.BIN , ��Ҫ��ȡ ) )
		{
			DEF_ERRMSG( m_isoFile , L"������ݴ���" );
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
BOOL CWQSG_UMD::CreateDir( const _tISO_DirEnt& tDirEnt_in , char const*const dirName )
{
	const u8 data[] = {
		0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
		0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
		0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
		0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	return WriteFile( tDirEnt_in , dirName , CWQSG_bufFile( (void*)data , sizeof(data) , FALSE ) , sizeof(data) , 0 , TRUE , TRUE );
}

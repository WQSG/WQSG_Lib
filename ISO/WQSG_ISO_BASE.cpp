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

#include "WQSG_ISO_BASE.h"
static inline void memcpyR( void* a_dst , const void* a_src , const size_t a_size )
{
	u8* dp = (u8*)a_dst;
	const u8* sp = (const u8*)a_src;

	for( size_t i = 0 ; i < a_size ; ++i )
		dp[i] = sp[a_size-i-1];
}
//----------------------------------------------------------------------------------------------------
static const u8 g_fileLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x0D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const u8 g_dirLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#define DEF_FN_make_DirLen( __def_nameLen ) (0x21 + ((((__def_nameLen)&1)==1)?(__def_nameLen):((__def_nameLen)+1)) + sizeof(g_fileLastData))
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
BOOL CWQSG_ISO_Base::InitLbaList( u32 a_uLbaCount )
{
	if( m_pLBA_List )
		delete m_pLBA_List;

	m_pLBA_List = new CWQSG_PartitionList( a_uLbaCount );
	if( NULL == m_pLBA_List )
	{
		DEF_ISO_ERRMSG( L"创建LBA分配表失败" );
		return FALSE;
	}

	if( 0 != m_pLBA_List->Alloc( m_tHead.rootDirEnt.lba_le ) )
	{
		DEF_ISO_ERRMSG( L"设置LBA保留区失败" );
		return FALSE;
	}

	if( (m_tHead.rootDirEnt.size_le < 2048) ||
		((m_tHead.rootDirEnt.size_le % 2048)!=0) ||
		(!m_pLBA_List->AllocPos( m_tHead.rootDirEnt.lba_le , m_tHead.rootDirEnt.size_le/2048 )) )
	{
		DEF_ISO_ERRMSG( L"分配 根目录LBA失败" );
		return FALSE;
	}

	if( !XXX_遍历目录申请( m_tHead.rootDirEnt ) )
		return FALSE;

	return TRUE;
}

CWQSG_ISO_Base::CWQSG_ISO_Base()
: m_pLBA_List(NULL)
{

}
//----------------------------------------------------------------------------------------------------
CWQSG_ISO_Base::~CWQSG_ISO_Base()
{

}
//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::Open( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite )
{
	if( !CWQSG_ISO_Raw::OpenFile( a_strISOPathName , a_bCanWrite ) )
		goto __gtOpenErr;

	if( !ReadUserData( &m_tHead , 16 ) )
	{
		DEF_ISO_ERRMSG( L"读取ISO信息失败" );
		goto __gtOpenErr;
	}

	if( 0 != memcmp( m_tHead.CD001 , "CD001" , 5 ) )
	{
		DEF_ISO_ERRMSG( L"不是 CD001" );
		goto __gtOpenErr;
	}

	if( m_tHead.Volume_Descriptor_Type != 1 || m_tHead.Volume_Descriptor_Version != 1 )
	{
		CString msg;
		msg.Format( L"不支持的 卷描述 %d.%d" , m_tHead.Volume_Descriptor_Type ,
			m_tHead.Volume_Descriptor_Version );
		DEF_ISO_ERRMSG( msg.GetString() );
		goto __gtOpenErr;
	}

	if( !xx_cmpeq( &m_tHead.LB_Size_LE ,
		&m_tHead.LB_Size_BE , sizeof(m_tHead.LB_Size_LE) ) )
	{
		DEF_ISO_ERRMSG( L"逻辑块大小数不同" );
		goto __gtOpenErr;
	}

	if( m_tHead.LB_Size_LE != 2048 )
	{
		CString msg;
		msg.Format( L"逻辑块Size不为 2048 , (%d)" , m_tHead.LB_Size_LE );
		DEF_ISO_ERRMSG( msg.GetString() );
		goto __gtOpenErr;
	}

	if( m_tHead.rootDirEnt.len != sizeof(m_tHead.rootDitBin) )
	{
		DEF_ISO_ERRMSG( L"根目录 size 错误" );
		goto __gtOpenErr;
	}

	if( !xx_cmpeq( &m_tHead.VolumeLBA_Total_LE ,
		&m_tHead.VolumeLBA_Total_BE , sizeof(m_tHead.VolumeLBA_Total_LE) ) )
	{
		DEF_ISO_ERRMSG( L"LBA大小总数不同" );
		goto __gtOpenErr;
	}

	if( !xx_cmpeq( &m_tHead.PathTableSize_LE ,
		&m_tHead.PathTableSize_BE , sizeof(m_tHead.PathTableSize_LE) ) )
	{
		DEF_ISO_ERRMSG( L"LBAPathTableSize大小不同" );
		goto __gtOpenErr;
	}

	if( !xx_cmpeq( &m_tHead.v6_LE , &m_tHead.v6_BE , sizeof(m_tHead.v6_LE) ) )
	{
		DEF_ISO_ERRMSG( L"v6大小不同" );
		goto __gtOpenErr;
	}

	if( !m_tHead.rootDirEnt.cheak() )
	{
		DEF_ISO_ERRMSG( L"根目录校验失败" );
		goto __gtOpenErr;
	}

	u8 szBuffer[2048];

	if( !ReadUserData( szBuffer , 16+1 ) )
	{
		DEF_ISO_ERRMSG( L"读取ISO信息失败" );
		goto __gtOpenErr;
	}

	if( *szBuffer != 0xFF )
	{
		CString str;
		switch( *szBuffer )
		{
		case 1:
			str = "ISO9660"; break;
		case 2:
			str = "JOLIET"; break;
		default:
			str = "未知"; break;
		}

		DEF_ISO_ERRMSG( L"不支持的子ISO系统(" + str + L")" );
		goto __gtOpenErr;
	}

	if( !InitLbaList( m_tHead.VolumeLBA_Total_LE ) )
		goto __gtOpenErr;

	return TRUE;
__gtOpenErr:
	Close();
	return FALSE;
}

//----------------------------------------------------------------------------------------------------
void CWQSG_ISO_Base::Close()
{
	delete m_pLBA_List;
	m_pLBA_List = NULL;
	CWQSG_ISO_Raw::CloseFile();
}
//----------------------------------------------------------------------------------------------------
s32 CWQSG_ISO_Base::ReadDirEnt( SISO_DirEnt& a_tDirEnt , char*const a_strFileName ,
								const SISO_DirEnt& a_ParentDirEnt , s32 a_nOffset , BOOL a_bNext )
{
	if( !a_ParentDirEnt.cheak() || a_ParentDirEnt.lba_le < m_tHead.rootDirEnt.lba_le ||
		a_nOffset < 0 || (a_nOffset%2) != 0 )
	{
		DEF_ISO_ERRMSG( L"参数错误" );
		return -1;
	}

	const s32 nLbaCount = a_ParentDirEnt.size_le/2048;
	s32 nLbaIndex;
	s32 nLbaOffset;
	u8 szBuffer[2048];
__gtReRead:
	nLbaIndex = a_nOffset / 2048;
	nLbaOffset = a_nOffset % 2048;

	if( nLbaIndex >= nLbaCount )
	{
		CString str;
		str.Format( L"参数错误 nLbaIndex(%d) >= nLbaCount(%d)" , nLbaIndex , nLbaCount );
		DEF_ISO_ERRMSG( str.GetString() );
		return -1;
	}

	if( nLbaOffset > (2048-DEF_FN_make_DirLen(0)) )
	{
		if( ++nLbaIndex >= nLbaCount )
		{
			if( a_bNext )
			{
				CString str;
				str.Format( L"参数错误 a_ParentDirEnt.size_le = %08X , nLbaOffset = %d , (2048-DEF_FN_make_DirLen(0)) = %d" ,
					a_ParentDirEnt.size_le , nLbaOffset , (2048-DEF_FN_make_DirLen(0)) );
				DEF_ISO_ERRMSG( str.GetString() );
				return -1;
			}
			else
			{
				memset( &a_tDirEnt , 0 , sizeof(a_tDirEnt) );
				return a_nOffset;
			}
		}

		a_nOffset = nLbaIndex * 2048;
		nLbaOffset = 0;
	}
__gtReRead2:
	if( !ReadUserData( szBuffer , a_ParentDirEnt.lba_le + nLbaIndex ) )
	{
		DEF_ISO_ERRMSG( L"读数据出错" );
		return -1;
	}

	const SISO_DirEnt*const pDirEnt = (SISO_DirEnt*)(szBuffer + nLbaOffset);

	if( 0 == pDirEnt->len )
	{
		if( ++nLbaIndex >= nLbaCount )
		{
			a_tDirEnt = *pDirEnt;
			return a_nOffset;
		}

		a_nOffset = nLbaIndex * 2048;
		nLbaOffset = 0;
		goto __gtReRead2;
	}

	if( pDirEnt->cheak() )
	{
		if( nLbaOffset > (n32)(2048 - DEF_FN_make_DirLen(pDirEnt->nameLen)) )
		{
			DEF_ISO_ERRMSG( L"错误的目录项,文件名长度错误" );
			return -1;
		}

		if( a_bNext )
		{
			a_nOffset += pDirEnt->len;

			if( ( a_nOffset % 2 ) != 0 )
				++a_nOffset;

			a_bNext = FALSE;
			goto __gtReRead;
		}

		if( a_strFileName )
		{
			memcpy( a_strFileName , (szBuffer + nLbaOffset + sizeof(SISO_DirEnt)) ,
				pDirEnt->nameLen );

			*(a_strFileName + pDirEnt->nameLen) = '\0';
		}

		a_tDirEnt = *pDirEnt;
		return a_nOffset;
	}

	DEF_ISO_ERRMSG( L"错误的目录项,目录项校验错误" );
	return -1;
}
//----------------------------------------------------------------------------------------------------
inline BOOL CWQSG_ISO_Base::IsDirEnt( const SISO_DirEnt& a_ParentDirEnt )
{
	if( (a_ParentDirEnt.attr & 2) != 2 || !a_ParentDirEnt.cheak() ||
		a_ParentDirEnt.lba_le < m_tHead.rootDirEnt.lba_le )
	{
		DEF_ISO_ERRMSG( L"参数错误,错误的DirEnt" );
		return FALSE;
	}

	SISO_DirEnt sDirEnt;
	s32 nOffset;

	nOffset = 0;
	if( nOffset != ReadDirEnt( sDirEnt , NULL , a_ParentDirEnt , nOffset , FALSE ) ||
		( sDirEnt.len != 0x30 ) || ( sDirEnt.nameLen != 1 ) ||
		(sDirEnt.lba_le != a_ParentDirEnt.lba_le) )
	{
		DEF_ISO_ERRMSG( L"不是DirEnt(1)" );
		return FALSE;
	}

	nOffset = 0x30;
	if( nOffset != ReadDirEnt( sDirEnt , NULL , a_ParentDirEnt , nOffset , FALSE ) ||
		( sDirEnt.len != 0x30 ) || ( sDirEnt.nameLen != 1 ) )
	{
		DEF_ISO_ERRMSG( L"不是DirEnt(2)" );
		return FALSE;
	}

	return TRUE;
}
//----------------------------------------------------------------------------------------------------
inline BOOL CWQSG_ISO_Base::XXX_遍历目录申请( const SISO_DirEnt& a_ParentDirEnt )
{
	if( !IsDirEnt( a_ParentDirEnt ) )
		return FALSE;

	SISO_DirEnt sDirEnt;
	s32 nOffset = 0x30;

	do
	{
#if _DEBUG
		char buf[256] = {};
		{
			const s32 nOffsetNew = ReadDirEnt( sDirEnt , buf , a_ParentDirEnt , nOffset , TRUE );
#else
		{
			const s32 nOffsetNew = ReadDirEnt( sDirEnt , NULL , a_ParentDirEnt , nOffset , TRUE );
#endif
			if( nOffsetNew < 0 )
				return FALSE;

			nOffset = nOffsetNew;
		}

		if( 0 == sDirEnt.len )
			return TRUE;

		{
			const n32 nLbaCount = ((sDirEnt.size_le%2048)==0)?(sDirEnt.size_le/2048):(sDirEnt.size_le/2048) + 1;

			if( nLbaCount > 0 )
				if( !m_pLBA_List->AllocPos( sDirEnt.lba_le , ((nLbaCount == 0)?1:nLbaCount) ) )
				{
					DEF_ISO_ERRMSG( L"申请LBA失败" );
					return FALSE;
				}
		}

		if( sDirEnt.attr & 2 )
		{
			if( !XXX_遍历目录申请( sDirEnt ) )
				return FALSE;
		}
	}
	while(TRUE);

	return FALSE;
}
//----------------------------------------------------------------------------------------------------
inline s32 CWQSG_ISO_Base::zzz_FindFile(  SISO_DirEnt& a_DirEnt , const SISO_DirEnt& a_ParentDirEnt ,
										s32 a_offset , const char*const a_strFileName )
{
	if( !IsDirEnt( a_ParentDirEnt ) )
		return -1;

	CStringA fileName;
	if( a_strFileName )
	{
		fileName = a_strFileName;
		if( fileName.GetLength() <= 0 )
		{
			DEF_ISO_ERRMSG( L"不能查找空文件名" );
			return -1;
		}
		fileName.MakeLower();
	}

	char nameBuffer[256];
	SISO_DirEnt dirEnt;

	do 
	{
		{
			const s32 newOffset( ReadDirEnt( dirEnt , nameBuffer , a_ParentDirEnt , a_offset , TRUE ) );
			if( newOffset < 0 )
				return -1;

			a_offset = newOffset;
		}

		if( dirEnt.len == 0 )
		{
			if( a_strFileName )
				break;

			a_DirEnt = dirEnt;
			return a_offset;
		}
		else if( a_strFileName )
		{
			CStringA name( nameBuffer );
// 			if( 2352 == m_nSectorSize )
// 			{
// 				const int pos = name.Find( ';' );
// 				if( pos != -1 )
// 				{
// 					nameBuffer[pos] = '\0';
// 					name.Delete( pos , name.GetLength() );
// 				}
// 			}
			if( fileName == name.MakeLower() )
			{
				a_DirEnt = dirEnt;
				return a_offset;
			}
		}
	}
	while(TRUE);

	DEF_ISO_ERRMSG( L"没找到文件" );
	return -1;
}
//----------------------------------------------------------------------------------------------------
s32 CWQSG_ISO_Base::FindFile( SISO_DirEnt& a_DirEnt , const SISO_DirEnt& a_ParentDirEnt , const char*const a_strFileName )
{
	return zzz_FindFile( a_DirEnt , a_ParentDirEnt , 0x30 , a_strFileName );
}
//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::ReadFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
							  CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset )
{
	if( !IsDirEnt( a_tDirEnt_Path ) ||
		a_buflen < 0 || a_fileName == NULL || strlen(a_fileName) > 120 ||
		a_startOffset < 0 || (a_startOffset+a_buflen) < 0
		)
	{
		DEF_ISO_ERRMSG( L"参数错误" );
		return FALSE;
	}

	//--------------------------------------------------------------------------------------
	const s32 预定长度 = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( 预定长度 > 250 )
	{
		DEF_ISO_ERRMSG( L"dir预定长度超过250" );
		return FALSE;
	}

	//-----------------------------------------------------------------
	SISO_DirEnt sDirEnt_File;
	const s32 nDirOffset = FindFile( sDirEnt_File , a_tDirEnt_Path , a_fileName );
	if( nDirOffset < 0 )
		return FALSE;

	if( sDirEnt_File.len != (u8)预定长度 )
	{
// 		if( 2352 != m_nSectorSize )
// 		{
// 			DEF_ISO_ERRMSG( L"dir实际长度 != 预定长度" );
// 			return FALSE;
// 		}
// 		else if( dirEnt.len != (u8)(预定长度 + 2) )
		{
			DEF_ISO_ERRMSG( L"dir实际长度 != 预定长度" );
			return FALSE;
		}
	}

	return ReadFile( sDirEnt_File , a_buffp , a_buflen , a_startOffset );
}

BOOL CWQSG_ISO_Base::ReadFile( const SISO_DirEnt& a_tDirEnt_File ,
			  CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset )
{
	if( !a_tDirEnt_File.cheak() ||
		a_buflen < 0 || a_startOffset < 0 || (a_startOffset+a_buflen) < 0
		)
	{
		DEF_ISO_ERRMSG( L"参数错误" );
		return FALSE;
	}
	//-----------------------------------------------------------------
	if( ( a_tDirEnt_File.size_le - a_startOffset ) < a_buflen )
	{
		DEF_ISO_ERRMSG( L"文件长度不足" );
		return FALSE;
	}

	u8 buffer[2352];
#if 1
	n32 nLbaIndex = a_startOffset / 2048 + a_tDirEnt_File.lba_le;
	n32 nLbaOffset = a_startOffset % 2048;

	s32 nBuflen = a_buflen;
	while( nBuflen > 0 )
	{
		if( !ReadUserData( buffer , nLbaIndex ) )
		{
			DEF_ISO_ERRMSG( L"读取数据错误" );
			return FALSE;
		}

		s32 nLen = 2048 - nLbaOffset;
		if( nBuflen < nLen )
			nLen = nBuflen;

		if( nLen != a_buffp.Write( buffer + nLbaOffset , nLen ) )
		{
			DEF_ISO_ERRMSG( L"输出数据错误" );
			return FALSE;
		}

		nLbaOffset = 0;
		++nLbaIndex;
		nBuflen -= nLen;
	}
#endif
	return TRUE;
}
//----------------------------------------------------------------------------------------------------
#define DEF_FN_toLBA( __def_size , __def_LBA ) ((__def_size/__def_LBA) + (((__def_size%__def_LBA)>0)?1:0))
const u8 g_DirData0[] = {
	0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
BOOL CWQSG_ISO_Base::WriteFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
							   CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_insertOffset ,
							   const BOOL a_bNew , const BOOL a_bDir , const SIsoTime* a_pTime )
{
	if( !IsDirEnt( a_tDirEnt_Path ) ||
		!a_fileName || strlen(a_fileName) > 120 ||
		a_buflen < 0 || a_insertOffset < 0 || (a_insertOffset+a_buflen) < 0
		)
	{
		DEF_ISO_ERRMSG( L"参数错误" );
		return FALSE;
	}
	//--------------------------------------------------------------------------------------
	const s32 n预定长度 = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( n预定长度 > 250 )
	{
		DEF_ISO_ERRMSG( L"预定长度超过250" );
		return FALSE;
	}
	//-----------------------------------------------------------------
	SISO_DirEnt dirEnt_File;

	BOOL b新 = FALSE;
	s32 nDirOffset;

	s32 最终fileSize = a_insertOffset + a_buflen;
	if( ( nDirOffset = FindFile( dirEnt_File , a_tDirEnt_Path , a_fileName ) ) >= 0 )
	{
		if( dirEnt_File.attr & 2 )
		{
			if( a_bDir )
				return TRUE;

			DEF_ISO_ERRMSG( L"不能写目录" );
			return FALSE;
		}
		else if( a_bDir )
		{
			DEF_ISO_ERRMSG( L"创建目录失败,已存在同名文件" );
			return FALSE;
		}

		if( dirEnt_File.len != (u8)n预定长度 )
		{
			DEF_ISO_ERRMSG( L"目录项长度 与 预定长度不符" );
			return FALSE;
		}

		if( !a_bNew )
			最终fileSize = ((最终fileSize>dirEnt_File.size_le)?(最终fileSize):dirEnt_File.size_le);

		const s32 需要的LBA = DEF_FN_toLBA( 最终fileSize , 2048 );
		const s32 拥有LBA = DEF_FN_toLBA( dirEnt_File.size_le , 2048 );

		if( 拥有LBA != 需要的LBA )
		{
			const s32 nOldLba = dirEnt_File.lba_le;

			if( 拥有LBA > 0 )
			{
				if( !m_pLBA_List->Free( nOldLba ) )
				{
					DEF_ISO_ERRMSG( L"释放LBA失败" );
					return FALSE;
				}

				if( 需要的LBA > 0 )
				{
					if( !m_pLBA_List->AllocPos( nOldLba , 需要的LBA ) )
					{
						dirEnt_File.lba_le = -1;
					}
				}
			}

			if( dirEnt_File.lba_le < 0 && 需要的LBA > 拥有LBA )
				dirEnt_File.lba_le = m_pLBA_List->Alloc( 需要的LBA );

			if( dirEnt_File.lba_le < 0 )
			{
				if( !AddLbaCount( 需要的LBA ) )
				{
					m_pLBA_List->AllocPos( nOldLba , 拥有LBA );
					DEF_ISO_ERRMSG( L"ISO空间不足,尝试扩容ISO失败" );
					return FALSE;
				}

				if( !m_pLBA_List->Free( nOldLba ) )
				{
					DEF_ISO_ERRMSG( L"释放LBA失败,这不可能" );
					return FALSE;
				}

				dirEnt_File.lba_le = m_pLBA_List->Alloc( 需要的LBA );

				if( dirEnt_File.lba_le < 0 )
				{
					m_pLBA_List->AllocPos( nOldLba , 拥有LBA );
					DEF_ISO_ERRMSG( L"申请LBA失败,这不可能,怎么会这样" );
					return FALSE;
				}
			}

			memcpyR( &dirEnt_File.lba_be , &dirEnt_File.lba_le , sizeof(dirEnt_File.lba_le) );

			//LBA move
			if( nOldLba != dirEnt_File.lba_le )
			{
				if( a_insertOffset != 0 )
				{
					DEF_ISO_ERRMSG( L"当前LAB块不足以容得下写入的数据，你可以先写偏移0，扩大文件后再写偏移" );
					return FALSE;
				}
			}
		}
	}
	else if( a_bNew )
	{
		if( a_insertOffset != 0 )
		{
			DEF_ISO_ERRMSG( L"添加文件只能从偏移 0 开始" );
			return FALSE;
		}

		const s32 需要的LBA = ((DEF_FN_toLBA( a_buflen , 2048 ))==0)?1:(DEF_FN_toLBA( a_buflen , 2048 ));

		b新 = TRUE;

		nDirOffset = 0x30;
		s32 界限长度;

		nDirOffset = zzz_FindFile( dirEnt_File , a_tDirEnt_Path , nDirOffset , NULL );
		if( nDirOffset < 0 )
			return FALSE;

		界限长度 = nDirOffset - ( nDirOffset % 2048 ) + 2048;
__gtReTest:
		if(	界限长度 <= nDirOffset || 界限长度 > a_tDirEnt_Path.size_le )
		{
			DEF_ISO_ERRMSG( L"目录空间不足" );
			return FALSE;
		}

		if( (界限长度 - nDirOffset) < n预定长度 )
		{
			nDirOffset = 界限长度;
			界限长度 += 2048;
			goto __gtReTest;
		}

		memset( &dirEnt_File , 0 , sizeof(dirEnt_File) );

		dirEnt_File.len = (u8)n预定长度;
		//	dirEnt.len_ex;
		{
			s32 LBA_Pos = m_pLBA_List->Alloc( 需要的LBA );
			if( LBA_Pos < 0 )
			{
				if( !AddLbaCount( 需要的LBA ) )
				{
					DEF_ISO_ERRMSG( L"ISO空间不足,尝试扩容ISO失败" );
					return FALSE;
				}

				LBA_Pos = m_pLBA_List->Alloc( 需要的LBA );

				if( LBA_Pos < 0 )
				{
					DEF_ISO_ERRMSG( L"申请LBA失败,这不可能,怎么会这样" );
					return FALSE;
				}
			}

			dirEnt_File.lba_le = LBA_Pos;
			memcpyR( &dirEnt_File.lba_be , &dirEnt_File.lba_le , sizeof(dirEnt_File.lba_le) );
		}

		//	dirEnt.attr;	dirEnt.sp1;	dirEnt.sp2;

		dirEnt_File.sp3_le = 1;
		memcpyR( &dirEnt_File.sp3_be , &dirEnt_File.sp3_le , sizeof(dirEnt_File.sp3_be) );

		dirEnt_File.nameLen = (u8)strlen(a_fileName);
		//----------------------------------------------------------
	}
	else
	{
		DEF_ISO_ERRMSG( L"没找到文件" );
		return FALSE;
	}

	//--------------------------------------------------------------------------------
	dirEnt_File.size_le = a_bDir?(2048):(最终fileSize);

	memcpyR( &dirEnt_File.size_be , &dirEnt_File.size_le , sizeof(dirEnt_File.size_le) );
	//------------------------------------------------------------------------------------------------------------

	CWQSG_memFile memfp;
	CWQSG_xFile* buffp_tmp = &a_buffp;
	s32 bufLen_tmp = a_buflen;
	s32 offset_tmp = a_insertOffset;

	if( a_bDir )
	{
		dirEnt_File.attr = 2;

		u8 data[sizeof(g_DirData0)];
		memcpy( data , g_DirData0 , sizeof(g_DirData0) );

		SISO_DirEnt* pEnt;

		pEnt = (SISO_DirEnt*)(data);
		memcpy( pEnt , &dirEnt_File , sizeof(SISO_DirEnt) - 1 );
		pEnt->len = 0x30;

		pEnt = (SISO_DirEnt*)(data+0x30);
		memcpy( pEnt , &a_tDirEnt_Path , sizeof(SISO_DirEnt) - 1 );
		pEnt->len = 0x30;

		if( sizeof(data) != memfp.Write( data , sizeof(data) ) )
			return FALSE;

		memfp.Seek(0);

		buffp_tmp = &memfp;
		bufLen_tmp = sizeof(data);
		offset_tmp = 0;
	}

	if( a_pTime )
	{
		dirEnt_File.time = *a_pTime;
	}
	else
	{
		SYSTEMTIME time;
		GetLocalTime( &time );

		dirEnt_File.time.uYear = (u8)(time.wYear - 1900);
		dirEnt_File.time.uMonth = (u8)time.wMonth ;
		dirEnt_File.time.uDay = (u8)time.wDay;
		dirEnt_File.time.uHours = (u8)time.wHour;
		dirEnt_File.time.uMinutes = (u8)time.wMinute;
		dirEnt_File.time.uSeconds = (u8)time.wSecond;
		dirEnt_File.time.uUnknown = 0;
	}

	while( bufLen_tmp > 0 )
	{
		const s32 nLbaIndex = offset_tmp / 2048 + dirEnt_File.lba_le;
		const s32 nLbaOffset = offset_tmp % 2048;

		u8 szLba[2048];
		if( nLbaOffset != 0 || bufLen_tmp < 2048 )
		{
			if( !ReadUserData( szLba , nLbaIndex ) )
				return FALSE;
		}

		u32 需要读取 = 2048 - nLbaOffset;

		if( 需要读取 > bufLen_tmp )
			需要读取 = bufLen_tmp;

		u8 buffer[2048];
		if( 需要读取 != buffp_tmp->Read( buffer , 需要读取 ) )
			return FALSE;

		memcpy( (szLba + nLbaOffset) , buffer , 需要读取 );

		if( !WriteUserData( szLba , nLbaIndex ) )
			return FALSE;

		bufLen_tmp -= 需要读取;
		offset_tmp += 需要读取;
	}

	/*
	if( int 余数 = len%m_isoFile.m_nLbaSize )
	{
	余数 = m_isoFile.m_nLbaSize - 余数;
	u8* tmp = new u8[余数];
	if( NULL != tmp )
	{
	memset( tmp , 0xff , 余数 );
	m_isoFile.m_ISOfp.Write( tmp , 余数 );
	delete[]tmp;
	}
	}*/
	//------------------------------------------------------------------------------------------------------------
	{
		const s32 nLbaIndex = nDirOffset / 2048 + a_tDirEnt_Path.lba_le;
		const s32 nLbaOffset = nDirOffset % 2048;

		u8 szLba[2048];
		if( !ReadUserData( szLba , nLbaIndex ) )
			return FALSE;

		memcpy( (szLba + nLbaOffset) , &dirEnt_File , sizeof(dirEnt_File) );

		if( b新 )
		{
			//		fileName.MakeUpper();
			const u32 nNameLen = (dirEnt_File.nameLen&1)?dirEnt_File.nameLen:(dirEnt_File.nameLen+1);

			n32 nOff = nLbaOffset + sizeof(dirEnt_File);
			memcpy( (szLba + nOff) , a_fileName , nNameLen );
			nOff += nNameLen;
			memcpy( (szLba + nOff) , (a_bDir?g_dirLastData:g_fileLastData) ,
				sizeof((a_bDir?g_dirLastData:g_fileLastData)) );
		}

		if( !WriteUserData( szLba , nLbaIndex ) )
			return FALSE;
	}
#if 0
	
#endif
	return TRUE;
}
//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::WriteFile( const SISO_DirEnt& a_tDirEnt_Path , char const*const a_fileName ,
								const void* a_buffer , const s32 a_buflen , const s32 a_insertOffset ,
								const BOOL a_bNew , const SIsoTime* a_pTime = NULL )
{
	return WriteFile( a_tDirEnt_Path , a_fileName , CWQSG_bufFile( (void*)a_buffer , a_buflen , FALSE ) , a_buflen , a_insertOffset , a_bNew , FALSE , a_pTime );
}
//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::CreateDir( const SISO_DirEnt& a_tDirEnt_Path , char const*const a_dirName )
{
	static u8 data[sizeof(g_DirData0)];

	return WriteFile( a_tDirEnt_Path , a_dirName , CWQSG_bufFile( (void*)data , sizeof(data) , FALSE ) , sizeof(data) , 0 , TRUE , TRUE , NULL );
}
//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::AddLbaCount( n32 a_nLbaCount )
{
	if( a_nLbaCount == 0 )
		return TRUE;

	n32 nMaxLbaCount = GetMaxLbaCount();

	const n32 nMaxLbaCountX = nMaxLbaCount + a_nLbaCount;

	if( a_nLbaCount < 0 && a_nLbaCount < 0 )
		return FALSE;

	u8 buf[2048] = {};

	for ( ; nMaxLbaCount < nMaxLbaCountX ; ++nMaxLbaCount )
	{
		if( !WriteUserData( buf , nMaxLbaCount ) )
			return FALSE;
	}

	SISO_Head2048 tHead = m_tHead;
	tHead.VolumeLBA_Total_LE = nMaxLbaCountX;
	memcpyR( &tHead.VolumeLBA_Total_BE , &nMaxLbaCountX , 4 );

	if( !WriteUserData( &tHead , 16 ) )
		return FALSE;

	m_tHead = tHead;

	return InitLbaList( nMaxLbaCount );
}
//----------------------------------------------------------------------------------------------------
const WCHAR* CWQSG_ISO_Interface::GetErrStr()const
{
	return m_strErrorStr.GetString();
}

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
inline	static bool xx_cmpeq( void const*const bufferLE , void const*const bufferBE , const size_t len )
{
	u8 const* buf1 = (u8 const*const)bufferLE;
	u8 const* buf2 = (u8 const*const)bufferBE;

	if( ( buf1 + len ) < buf1 )
		return false;
	if( ( buf2 + (len-1) ) < buf2 )
		return false;

	buf2 += (len-1);
	for( size_t i = 0 , last_i = 0 ; (i >= last_i) && (i < len) ; last_i = i++ )
	{
		if( *buf1 !=  *buf2 )
			return false;

		++buf1,--buf2;
	}
	return true;
}
bool _tISO_DirEnt::cheak()
{
#if 0
	return ( xx_cmpeq( &lba_le , &lba_be , sizeof(lba_le) ) &&
		xx_cmpeq( &size_le , &size_be , sizeof(size_le) ) &&
		xx_cmpeq( &sp3_le , &sp3_be , sizeof(sp3_le) ) &&
		( (len&1) == 0 ) &&	( len > 0 ) && ( len_ex == 0 ) &&
		(nameLen > 0) && (nameLen<=127) && ( size_le >= 0)
		);
#else
	return ( xx_cmpeq( &lba_le , &lba_be , sizeof(lba_le) ) &&
		xx_cmpeq( &sp3_le , &sp3_be , sizeof(sp3_le) ) &&
		( (len&1) == 0 ) && ( len > 0 ) && ( len_ex == 0 ) &&
		(nameLen > 0) && (nameLen<=127) && ( size_le >= 0)
		);
#endif
}
//-------------------------------------------------------------------------------------------------
#if _DEBUG
#define W_ASSERT( _def ) do{if( !(_def) ) __asm{int 3};}while(0)
#else
#define W_ASSERT( _def )
#endif

CWQSG_ISO_Base::CWQSG_ISO_Base()
: m_pLBA_List(NULL)
{
#if _DEBUG
	int ddd2048 = sizeof( _tISO_Head2048 );
	int ddd2352 = sizeof( _tISO_Head2352 );
#endif
	W_ASSERT( 2048 == sizeof( _tISO_Head2048 ) );
	W_ASSERT( 2352 == sizeof( _tISO_Head2352 ) );
}

CWQSG_ISO_Base::~CWQSG_ISO_Base()
{
	Close();
}

BOOL CWQSG_ISO_Base::Open( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite )
{
	if( (!a_strISOPathName) )
	{
		DEF_ERRMSG( L"传入参数错误" );
		return FALSE;
	}

	Close();

	m_bCanWrite = a_bCanWrite;

	if( !m_ISOfp.OpenFile( a_strISOPathName , (a_bCanWrite?3:1) , 3 ) )
	{
		DEF_ERRMSG( L"打开ISO文件失败" );
		goto __gtErr;
	}

	{
		_tISO_Head2352 testHead;
		m_ISOfp.Seek( 0x0 );
		if( sizeof(testHead) != m_ISOfp.Read( &testHead , sizeof(testHead) ) )
		{
			DEF_ERRMSG( L"读取ISO信息失败" );
			goto __gtErr;
		}

		const u8 szSync[] = {0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};
		if( memcmp( testHead.m_Sync , szSync , 12 ) != 0 )
			m_nSectorSize = 2048;
		else
			m_nSectorSize = 2352;
	}

	m_ISOfp.Seek( 0x10 * m_nSectorSize );
	if( m_nSectorSize != m_ISOfp.Read( &m_tHead0 , m_nSectorSize ) )
	{
		DEF_ERRMSG( L"读取ISO信息失败" );
		goto __gtErr;
	}

	if( m_nSectorSize == 2352 )
	{
		switch( m_tHead0.m_uMode )
		{
		case 1://Mode1
			m_pHead0 = &(m_tHead0.m_Head1);
			break;
		case 2://Mode2 Form1
			m_pHead0 = &(m_tHead0.m_Head2);
			break;
		default:
			DEF_ERRMSG( L"未知的 2352 Mode" );
			goto __gtErr;
		}
	}
	else
	{
		m_pHead0 = &(m_tHead0.m_Head2048);
	}

	m_nHeadOffset = (s32)((u8*)m_pHead0 - (u8*)&m_tHead0);

	if( 0 != memcmp( m_pHead0->CD001 , "CD001" , 5 ) )
	{
		DEF_ERRMSG( L"不是 CD001" );
		goto __gtErr;
	}

	if( ( m_pHead0->Volume_Descriptor_Type != 1 ) ||
		( m_pHead0->Volume_Descriptor_Version != 1 ) )
	{
		CString msg;
		msg.Format( L"不支持的 卷描述 %d.%d" , m_pHead0->Volume_Descriptor_Type ,
			m_pHead0->Volume_Descriptor_Version );
		DEF_ERRMSG( msg.GetBuffer() );
		goto __gtErr;
	}

	if( !xx_cmpeq( &m_pHead0->LB_Size_LE ,
		&m_pHead0->LB_Size_BE , sizeof(m_pHead0->LB_Size_LE) ) )
	{
		DEF_ERRMSG( L"逻辑块大小数不同" );
		goto __gtErr;
	}
	if( m_pHead0->LB_Size_LE != 2048 )
	{
		CString msg;
		msg.Format( L"逻辑块Size不为 2048 , (%d)" , m_pHead0->LB_Size_LE );
		DEF_ERRMSG( msg.GetBuffer() );
		goto __gtErr;
	}

	m_nLbaSize = m_pHead0->LB_Size_LE;

	if( m_pHead0->rootDirEnt.len != sizeof(m_pHead0->rootDitBin) )
	{
		DEF_ERRMSG( L"根目录 size 错误" );
		goto __gtErr;
	}

	if( !xx_cmpeq( &m_pHead0->VolumeLBA_Total_LE ,
		&m_pHead0->VolumeLBA_Total_BE , sizeof(m_pHead0->VolumeLBA_Total_LE) ) )
	{
		DEF_ERRMSG( L"LBA大小总数不同" );
		goto __gtErr;
	}

	if( !xx_cmpeq( &m_pHead0->PathTableSize_LE ,
		&m_pHead0->PathTableSize_BE , sizeof(m_pHead0->PathTableSize_LE) ) )
	{
		DEF_ERRMSG( L"LBAPathTableSize大小不同" );
		goto __gtErr;
	}
	if( !xx_cmpeq( &m_pHead0->v6_LE , &m_pHead0->v6_BE ,
		sizeof(m_pHead0->v6_LE) ) )
	{
		DEF_ERRMSG( L"v6大小不同" );
		goto __gtErr;
	}
	if( !m_pHead0->rootDirEnt.cheak() )
	{
		DEF_ERRMSG( L"根目录校验失败" );
		goto __gtErr;
	}
	m_pLBA_List = new CWQSG_PartitionList( m_pHead0->VolumeLBA_Total_LE );
	if( NULL == m_pLBA_List )
	{
		DEF_ERRMSG( L"创建LBA分配表失败" );
		goto __gtErr;
	}
	if( 0 != m_pLBA_List->申请( m_pHead0->rootDirEnt.lba_le ) )
	{
		DEF_ERRMSG( L"设置LBA保留区失败" );
		goto __gtErr;
	}
	{
		if( (m_pHead0->rootDirEnt.size_le < m_nLbaSize) ||
			((m_pHead0->rootDirEnt.size_le%m_nLbaSize)!=0) ||
			(!m_pLBA_List->申请( m_pHead0->rootDirEnt.lba_le , m_pHead0->rootDirEnt.size_le/m_nLbaSize )) )
		{
			DEF_ERRMSG( L"分配 根目录LBA失败" );
			goto __gtErr;
		}
	}
	if( !XXX_遍历目录申请( m_pHead0->rootDirEnt ) )
		goto __gtErr;

	return TRUE;
__gtErr:
	Close( );
	return FALSE;
}

void CWQSG_ISO_Base::Close()
{
	m_ISOfp.Close();
	if( m_pLBA_List )
	{
		delete m_pLBA_List;
		m_pLBA_List = NULL;
	}
}

inline BOOL CWQSG_ISO_Base::XXX_遍历目录申请( const _tISO_DirEnt& a_tDirEnt_in )
{
	if( (a_tDirEnt_in.lba_le < m_pHead0->rootDirEnt.lba_le ) ||
		( (a_tDirEnt_in.size_le%m_nLbaSize) != 0 )
		)
	{
		DEF_ERRMSG( L"参数错误" );
		return FALSE;
	}

	_tISO_DirEnt dirEnt;
	if( 0 != ReadDirEnt( a_tDirEnt_in  , 0 , dirEnt , NULL , false ) ||
		( dirEnt.len != 0x30 ) || ( dirEnt.nameLen != 1 ) ||
		(dirEnt.lba_le != a_tDirEnt_in.lba_le) )
	{
		DEF_ERRMSG( L"不是文件夹1" );
		return FALSE;
	}

	if( 0x30 != ReadDirEnt( a_tDirEnt_in , 0x30 , dirEnt , NULL , false ) ||
		( dirEnt.len != 0x30 ) || ( dirEnt.nameLen != 1 ) )
	{
		DEF_ERRMSG( L"不是文件夹2" );
		return FALSE;
	}

	for( s32 offset = 0x60 ; offset < a_tDirEnt_in.size_le ; )
	{
		{
			const s32 newOffset( ReadDirEnt( a_tDirEnt_in , offset , dirEnt , NULL , false ) );
			if( newOffset < 0 )
				return FALSE;

			offset = newOffset;
		}
		if( dirEnt.len == 0 )
			break;

		offset += dirEnt.len;

		const s32 LBAcount = (dirEnt.size_le/m_nLbaSize) + (( (dirEnt.size_le%m_nLbaSize)>0 )?1:0);
		if( !m_pLBA_List->申请( dirEnt.lba_le , (LBAcount==0)?1:LBAcount ) )
		{
			DEF_ERRMSG( L"申请LBA失败" );
			return FALSE;
		}

		if( dirEnt.attr & 2 )
			if( !XXX_遍历目录申请( dirEnt ) )
				return FALSE;
	}
	return TRUE;
}

s32 CWQSG_ISO_Base::ReadDirEnt( const _tISO_DirEnt& a_tDirEnt_in , const s32 a_dirOffset ,
						 _tISO_DirEnt& a_tDirEnt , char*const a_strFileName , const BOOL a_bFindFree )
{
	if( (!m_ISOfp.IsOpen()) ||
		(a_tDirEnt_in.lba_le < m_pHead0->rootDirEnt.lba_le ) ||
		(a_dirOffset < 0) )
	{
		DEF_ERRMSG( L"参数错误" );
		return -1;
	}
	s32 nDirOffset( a_dirOffset );
	BOOL bReRead = TRUE;

__gtRead:
	SectorSeek( a_tDirEnt_in.lba_le , nDirOffset );

	if( sizeof(_tISO_DirEnt) == m_ISOfp.Read( &a_tDirEnt , sizeof(_tISO_DirEnt) ) )
	{
		if( 0 == a_tDirEnt.len )
		{
			const s32 nFixOffset = nDirOffset - ( nDirOffset % m_nLbaSize ) + m_nLbaSize;
			if( (!a_bFindFree) && bReRead && (nFixOffset > nDirOffset) &&
				(nFixOffset < a_tDirEnt_in.size_le) )
			{
				bReRead = FALSE;
				nDirOffset = nFixOffset;
				goto __gtRead;
			}
			return nDirOffset;
		}

		if( a_tDirEnt.cheak() )
		{
			if( a_strFileName &&
				(a_tDirEnt.nameLen == m_ISOfp.Read( a_strFileName , a_tDirEnt.nameLen ) ) )
				a_strFileName[a_tDirEnt.nameLen] = '\0';

			return nDirOffset;
		}
		DEF_ERRMSG( L"错误的目录项" );
		return -1;
	}
	DEF_ERRMSG( L"读取目录项失败" );
	return -1;
}
//----------------------------------------------------------------------------------------------------
static const unsigned char fileLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x0D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const unsigned char dirLastData[14] = { 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#define DEF_FN_make_DirLen( __def_nameLen ) (0x21 + ((((__def_nameLen)&1)==1)?(__def_nameLen):((__def_nameLen)+1)) + sizeof(fileLastData))
BOOL CWQSG_ISO_Base::ReadFile( const _tISO_DirEnt& a_tDirEnt_in , const char*const a_fileName ,
						 CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset )
{
	if( ( !a_buffp.IsOpen() ) || ( a_buflen < 0 ) ||
		( a_fileName == NULL ) || ( strlen(a_fileName) > 120 ) ||
		( a_startOffset < 0 ) || ( (a_startOffset+a_buflen) < 0 )
		)
	{
		DEF_ERRMSG( L"参数错误" );
		return FALSE;
	}

	//--------------------------------------------------------------------------------------
	const s32 预定长度 = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( 预定长度 > 250 )
	{
		DEF_ERRMSG( L"dir预定长度超过250" );
		return FALSE;
	}

	//-----------------------------------------------------------------
	_tISO_DirEnt dirEnt;
	s32 dirOffset;

	if( ( dirOffset = FindFile( a_tDirEnt_in , a_fileName , dirEnt ) ) < 0 )
		return FALSE;

	if( dirEnt.len != (u8)预定长度 )
 	{
// 		if( 2352 != m_nSectorSize )
// 		{
// 			DEF_ERRMSG( L"dir实际长度 != 预定长度" );
// 			return FALSE;
// 		}
// 		else if( dirEnt.len != (u8)(预定长度 + 2) )
		{
			DEF_ERRMSG( L"dir实际长度 != 预定长度" );
			return FALSE;
		}
	}

	if( ( dirEnt.size_le - a_startOffset ) < a_buflen )
	{
		DEF_ERRMSG( L"文件长度不足" );
		return FALSE;
	}

	u8 buffer[2352];

	s32 nStartOffset = a_startOffset;
	s32 nBuflen = a_buflen;
	while( nBuflen > 0 )
	{
		s32 nLen = SectorSeek( dirEnt.lba_le , nStartOffset );
		if( nLen > nBuflen )
			nLen = nBuflen;

		if( nLen != m_ISOfp.Read( buffer , nLen ) )
		{
			DEF_ERRMSG( L"读取数据错误" );
			return FALSE;
		}

		if( nLen != a_buffp.Write( buffer , nLen ) )
		{
			DEF_ERRMSG( L"输出数据错误" );
			return FALSE;
		}

		nStartOffset += nLen;
		nBuflen -= nLen;
	}

	return TRUE;
}

inline s32 CWQSG_ISO_Base::zzz_FindFile( const _tISO_DirEnt& tDirEnt_in , s32 offset , char const*const strFileName , _tISO_DirEnt& tDirEnt  , const bool bFindFree )
{
	if( (0 != ReadDirEnt( tDirEnt_in , 0 , tDirEnt , NULL , false )) ||
		(tDirEnt.len != 0x30) || (tDirEnt.attr !=2 ) || (tDirEnt.nameLen != 1) ||
		(tDirEnt.lba_le != tDirEnt_in.lba_le) || (tDirEnt_in.size_le != tDirEnt_in.size_le) ||
		(0x30 != ReadDirEnt( tDirEnt_in , 0x30 , tDirEnt ,NULL , false )) ||
		(tDirEnt.len != 0x30) || (tDirEnt.attr != 2) || (tDirEnt.nameLen != 1)
		)
	{
		DEF_ERRMSG( L"参数错误" );
		return -1;
	}

	CStringA fileName;
	if( strFileName )
	{
		fileName = strFileName;
		if( fileName.GetLength() <= 0 )
		{
			DEF_ERRMSG( L"不能查找空文件名" );
			return -1;
		}
		fileName.MakeLower();
	}

	char nameBuffer[256];
	for( ; offset < tDirEnt_in.size_le ; )
	{
		{
			const s32 newOffset( ReadDirEnt( tDirEnt_in , offset , tDirEnt , nameBuffer , bFindFree ) );
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
		else
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
				return offset;
		}
		offset += tDirEnt.len;
	}
	DEF_ERRMSG( L"没找到文件" );
	return -1;
}

s32 CWQSG_ISO_Base::FindFile( const _tISO_DirEnt& a_tDirEnt_in , char const*const a_strFileName , _tISO_DirEnt& a_tDirEnt )
{
	return zzz_FindFile( a_tDirEnt_in , 0x60 , a_strFileName , a_tDirEnt , false );
}

//--------------------------------------------------------------------------------------------------
static inline void memcpyR( void* a_dst , const void* a_src , const size_t a_size )
{
	u8* dp = (u8*)a_dst;
	const u8* sp = (const u8*)a_src;

	for( size_t i = 0 ; i < a_size ; ++i )
		dp[i] = sp[a_size-i-1];
}
//--------------------------------------------------------------------------------------------------
#define DEF_FN_toLBA( __def_size , __def_LBA ) ((__def_size/__def_LBA) + (((__def_size%__def_LBA)>0)?1:0))
BOOL CWQSG_ISO_Base::WriteFile( const _tISO_DirEnt& a_tDirEnt_in , char const*const a_fileName ,
						  const void* a_buffer , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_isNew )
{
	return WriteFile( a_tDirEnt_in , a_fileName , CWQSG_bufFile( (void*)a_buffer , a_buflen , FALSE ) , a_buflen , a_insertOffset , a_isNew , FALSE );
}
//--------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::WriteFile( const _tISO_DirEnt& a_tDirEnt_in , const char*const a_fileName ,
						  CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_insertOffset ,
						  const BOOL a_isNew , const BOOL a_isDir )
{
	if( (!m_bCanWrite) ||
		( !a_buffp.IsOpen() ) || ( a_buflen < 0 ) ||
		(a_fileName == NULL) || ( strlen(a_fileName) > 120 ) ||
		( a_insertOffset < 0 ) || ( (a_insertOffset+a_buflen) < 0 )
		)
	{
		DEF_ERRMSG( L"参数错误" );
		return FALSE;
	}
	//--------------------------------------------------------------------------------------
	const s32 n预定长度 = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( n预定长度 > 250 )
	{
		DEF_ERRMSG( L"预定长度超过250" );
		return FALSE;
	}
	//-----------------------------------------------------------------
	_tISO_DirEnt dirEnt;

	BOOL b新 = FALSE;
	s32 nDirOffset ;
	//	nDirOffset = FindFile( tDirEnt_in , fileName , dirEnt );


	s32 最终fileSize = a_insertOffset + a_buflen;
	if( ( nDirOffset = FindFile( a_tDirEnt_in , a_fileName , dirEnt ) ) >= 0 )
	{
		if( dirEnt.attr & 2 )
		{
			if( a_isDir )
				return TRUE;

			DEF_ERRMSG( L"不能写目录" );
			return FALSE;
		}
		else if( a_isDir )
		{
			DEF_ERRMSG( L"创建目录失败,已存在同名文件" );
			return FALSE;
		}

		if( dirEnt.len != (u8)n预定长度 )
		{
			DEF_ERRMSG( L"目录项长度 与 预定长度不符" );
			return FALSE;
		}

		if( !a_isNew )
			最终fileSize = ((最终fileSize>dirEnt.size_le)?(最终fileSize):dirEnt.size_le);

		const s32 需要的LBA = DEF_FN_toLBA( 最终fileSize , m_nLbaSize );
		const s32 拥有LBA = DEF_FN_toLBA( dirEnt.size_le , m_nLbaSize );

		if( 拥有LBA != 需要的LBA )
		{
			if( !m_pLBA_List->释放( dirEnt.lba_le ) )
			{
				DEF_ERRMSG( L"释放LBA失败" );
				return FALSE;
			}

			if( 拥有LBA < 需要的LBA )
				dirEnt.lba_le = m_pLBA_List->申请( 需要的LBA );
			else
				dirEnt.lba_le = m_pLBA_List->申请( dirEnt.lba_le , 需要的LBA );

			if( dirEnt.lba_le < 0 )
			{
				DEF_ERRMSG( L"申请LBA失败" );
				return FALSE;
			}

			memcpyR( &dirEnt.lba_be , &dirEnt.lba_le , sizeof(dirEnt.lba_le) );
		}
	}

	else if( a_isNew )
	{
		if( a_insertOffset != 0 )
		{
			DEF_ERRMSG( L"添加文件只能从偏移 0 开始" );
			return FALSE;
		}

		const s32 需要的LBA = ((DEF_FN_toLBA( a_buflen , m_nLbaSize ))==0)?1:(DEF_FN_toLBA( a_buflen , m_nLbaSize ));

		b新 = TRUE;

		nDirOffset = 0x60;
		s32 界限长度;
__gtReTest:
		if( ( nDirOffset = zzz_FindFile( a_tDirEnt_in , nDirOffset , NULL , dirEnt , true ) ) < 0 )
			return FALSE;

		界限长度 = nDirOffset - ( nDirOffset % m_nLbaSize ) + m_nLbaSize;

		if(	( 界限长度 <= nDirOffset ) || ( 界限长度 > a_tDirEnt_in.size_le ) )
		{
			DEF_ERRMSG( L"目录空间不足" );
			return FALSE;
		}
		if( (界限长度 - nDirOffset) < n预定长度 )
		{
			nDirOffset = (界限长度 += m_nLbaSize);
			goto __gtReTest;
		}

		memset( &dirEnt , 0 , sizeof(dirEnt) );

		dirEnt.len = (u8)n预定长度;
		//	dirEnt.len_ex;
		{
			const s32 LBA_Pos = m_pLBA_List->申请( 需要的LBA );
			if( LBA_Pos < 0 )
			{
				DEF_ERRMSG( L"申请LBA失败" );
				return FALSE;
			}

			dirEnt.lba_le = LBA_Pos;
			memcpyR( &dirEnt.lba_be , &dirEnt.lba_le , sizeof(dirEnt.lba_le) );
		}

		//	dirEnt.attr;	dirEnt.sp1;	dirEnt.sp2;

		dirEnt.sp3_le = 1;
		memcpyR( &dirEnt.sp3_be , &dirEnt.sp3_le , sizeof(dirEnt.sp3_be) );

		dirEnt.nameLen = strlen(a_fileName);
		//----------------------------------------------------------
	}
	else
	{
		DEF_ERRMSG( L"没找到文件" );
		return FALSE;
	}

	//--------------------------------------------------------------------------------
	dirEnt.size_le = a_isDir?(m_nLbaSize):(最终fileSize);

	memcpyR( &dirEnt.size_be , &dirEnt.size_le , sizeof(dirEnt.size_le) );
	//------------------------------------------------------------------------------------------------------------

	CWQSG_memFile memfp;
	CWQSG_xFile* buffp_tmp = &a_buffp;
	s32 bufLen_tmp = a_buflen;
	s32 offset_tmp = a_insertOffset;

	if( a_isDir )
	{
		dirEnt.attr = 2;

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
		memcpy( pEnt , &a_tDirEnt_in , sizeof(_tISO_DirEnt) - 1 );
		pEnt->len = 0x30;

		if( sizeof(data) != memfp.Write( data , sizeof(data) ) )
			return FALSE;

		memfp.Seek(0);

		buffp_tmp = &memfp;
		bufLen_tmp = sizeof(data);
		offset_tmp = 0;
	}

	{
		SYSTEMTIME time;
		GetLocalTime( &time );

		dirEnt.time.uYear = (u8)(time.wYear - 1900);
		dirEnt.time.uMonth = (u8)time.wMonth ;
		dirEnt.time.uDay = (u8)time.wDay;
		dirEnt.time.uHours = (u8)time.wHour;
		dirEnt.time.uMinutes = (u8)time.wMinute;
		dirEnt.time.uSeconds = (u8)time.wSecond;
		dirEnt.time.uUnknown = 0;
	}

	while( bufLen_tmp > 0 )
	{
		u32 需要读取 = SectorSeek( dirEnt.lba_le , offset_tmp );

		u8 buffer[2352];

		if( 需要读取 > bufLen_tmp )
			需要读取 = bufLen_tmp;

		if( 需要读取 != buffp_tmp->Read( buffer , 需要读取 ) )
			return FALSE;

		if( 需要读取 != m_ISOfp.Write( buffer , 需要读取 ) )
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
		SectorSeek( a_tDirEnt_in.lba_le , nDirOffset );

		if( sizeof(dirEnt) != m_ISOfp.Write( &dirEnt , sizeof(dirEnt) ) )
		{
			DEF_ERRMSG( L"写目录表失败" );
			return FALSE;
		}
	}

	if( b新 )
	{
		//		fileName.MakeUpper();
		const u32 写长度 = (dirEnt.nameLen&1)?dirEnt.nameLen:(dirEnt.nameLen+1);
		if( ( 写长度 != m_ISOfp.Write( a_fileName , 写长度 ) ) ||
			( sizeof((a_isDir?dirLastData:fileLastData)) !=
			m_ISOfp.Write( (a_isDir?dirLastData:fileLastData) , sizeof((a_isDir?dirLastData:fileLastData)) ) ) )
		{
			DEF_ERRMSG( L"写目录表失败" );
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
BOOL CWQSG_ISO_Base::CreateDir( const _tISO_DirEnt& a_tDirEnt_in , const char*const a_dirName )
{
	const u8 data[] = {
		0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
		0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x30, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
		0x08, 0x00, 0x6C, 0x01, 0x0B, 0x0C, 0x00, 0x00, 0x24, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
		0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x55, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	return WriteFile( a_tDirEnt_in , a_dirName , CWQSG_bufFile( (void*)data , sizeof(data) , FALSE ) , sizeof(data) , 0 , TRUE , TRUE );
}

inline s32 CWQSG_ISO_Base::SectorSeek( s32 a_nLbaID , s32 a_nLbaOffset )
{
	const s32 nLbaIndex = a_nLbaOffset / m_nLbaSize;
	const s32 nLbaOffset = a_nLbaOffset % m_nLbaSize;

	m_ISOfp.Seek( ( a_nLbaID + nLbaIndex ) * m_nSectorSize + nLbaOffset + m_nHeadOffset );

	return m_nLbaSize - nLbaOffset;
}

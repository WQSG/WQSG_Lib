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

#include "wqsg_iso_base.h"
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
		DEF_ISO_SET_ERRMSG( GetLangString(2) );
		return FALSE;
	}

	if( 0 != m_pLBA_List->Alloc( L2H(m_tHead.rootDirEnt.lba_LE) ) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(3) );
		return FALSE;
	}

	if( (L2H(m_tHead.rootDirEnt.size_LE) < 2048) ||
		((L2H(m_tHead.rootDirEnt.size_LE) % 2048)!=0) ||
		(!m_pLBA_List->AllocPos( L2H(m_tHead.rootDirEnt.lba_LE) , L2H(m_tHead.rootDirEnt.size_LE)/2048 )) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(4) );
		return FALSE;
	}

	if( !XXX_����Ŀ¼����( m_tHead.rootDirEnt ) )
		return FALSE;

	return TRUE;
}

CWQSG_ISO_Base::CWQSG_ISO_Base()
: m_pLBA_List(NULL)
, a_pImportTime(NULL)
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
		DEF_ISO_SET_ERRMSG( GetLangString(50) );
		goto __gtOpenErr;
	}

	if( 0 != memcmp( m_tHead.CD001 , "CD001" , 5 ) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(5) );
		goto __gtOpenErr;
	}

	if( m_tHead.Volume_Descriptor_Type != 1 || m_tHead.Volume_Descriptor_Version != 1 )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(6) ,
			m_tHead.Volume_Descriptor_Type ,
			m_tHead.Volume_Descriptor_Version );

		goto __gtOpenErr;
	}

	if( L2H(m_tHead.LB_Size_le) != B2H(m_tHead.LB_Size_be) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(7) );
		goto __gtOpenErr;
	}

	if( L2H(m_tHead.LB_Size_le) != 2048 )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(8) , L2H(m_tHead.LB_Size_le) );
		goto __gtOpenErr;
	}

	if( m_tHead.rootDirEnt.len != sizeof(m_tHead.rootDitBin) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(9) );
		goto __gtOpenErr;
	}

	if( L2H(m_tHead.VolumeLBA_Total_le) != B2H(m_tHead.VolumeLBA_Total_be) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(10) );
		goto __gtOpenErr;
	}

	if( L2H(m_tHead.PathTableSize_le) != B2H(m_tHead.PathTableSize_be) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(11) );
		goto __gtOpenErr;
	}

	if( L2H(m_tHead.v6_le) != B2H(m_tHead.v6_be) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(12) );
		goto __gtOpenErr;
	}

	if( !m_tHead.rootDirEnt.cheak() )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(13) );
		goto __gtOpenErr;
	}

	u8 szBuffer[2048];

	if( !ReadUserData( szBuffer , 16+1 ) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(50) );
		goto __gtOpenErr;
	}

	if( *szBuffer != 0xFF )
	{
		CString str;
		switch( *szBuffer )
		{
		case 1:
			DEF_ISO_SET_ERRMSG( GetLangString(14) );
			break;
		case 2:
			DEF_ISO_SET_ERRMSG( GetLangString(15) );
			break;
		default:
			DEF_ISO_SET_ERRMSG( GetLangString(16) );
			break;
		}
		goto __gtOpenErr;
	}

	if( !InitLbaList( L2H(m_tHead.VolumeLBA_Total_le) ) )
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
	if( !a_ParentDirEnt.cheak() || L2H(a_ParentDirEnt.lba_LE) < L2H(m_tHead.rootDirEnt.lba_LE) ||
		a_nOffset < 0 || (a_nOffset%2) != 0 )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(17) );
		return -1;
	}

	const s32 nLbaCount = L2H(a_ParentDirEnt.size_LE)/2048;
	s32 nLbaIndex;
	s32 nLbaOffset;
	u8 szBuffer[2048];
__gtReRead:
	nLbaIndex = a_nOffset / 2048;
	nLbaOffset = a_nOffset % 2048;

	if( nLbaIndex >= nLbaCount )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(18) , nLbaIndex , nLbaCount );
		return -1;
	}

	if( nLbaOffset > (2048-DEF_FN_make_DirLen(0)) )
	{
		if( ++nLbaIndex >= nLbaCount )
		{
			if( a_bNext )
			{
				DEF_ISO_SET_ERRMSG( GetLangString(19) ,
					L2H(a_ParentDirEnt.size_LE) , nLbaOffset , (2048-DEF_FN_make_DirLen(0)) );
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
	if( !ReadUserData( szBuffer , L2H(a_ParentDirEnt.lba_LE) + nLbaIndex ) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(20) );
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
			DEF_ISO_SET_ERRMSG( GetLangString(21) );
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

	DEF_ISO_SET_ERRMSG( GetLangString(22) );
	return -1;
}
//----------------------------------------------------------------------------------------------------
inline BOOL CWQSG_ISO_Base::IsDirEnt( const SISO_DirEnt& a_ParentDirEnt )
{
	if( (a_ParentDirEnt.attr & 2) != 2 || !a_ParentDirEnt.cheak() ||
		L2H(a_ParentDirEnt.lba_LE) < L2H(m_tHead.rootDirEnt.lba_LE) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(23) );
		return FALSE;
	}

	SISO_DirEnt sDirEnt;
	s32 nOffset;

	nOffset = 0;
	if( nOffset != ReadDirEnt( sDirEnt , NULL , a_ParentDirEnt , nOffset , FALSE ) ||
		( sDirEnt.len != 0x30 ) || ( sDirEnt.nameLen != 1 ) ||
		(L2H(sDirEnt.lba_LE) != L2H(a_ParentDirEnt.lba_LE)) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(24) );
		return FALSE;
	}

	nOffset = 0x30;
	if( nOffset != ReadDirEnt( sDirEnt , NULL , a_ParentDirEnt , nOffset , FALSE ) ||
		( sDirEnt.len != 0x30 ) || ( sDirEnt.nameLen != 1 ) )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(25) );
		return FALSE;
	}

	return TRUE;
}
//----------------------------------------------------------------------------------------------------
inline BOOL CWQSG_ISO_Base::XXX_����Ŀ¼����( const SISO_DirEnt& a_ParentDirEnt )
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
			const n32 nLbaCount = ((L2H(sDirEnt.size_LE)%2048)==0)?(L2H(sDirEnt.size_LE)/2048):(L2H(sDirEnt.size_LE)/2048) + 1;

			if( nLbaCount > 0 )
				if( !m_pLBA_List->AllocPos( L2H(sDirEnt.lba_LE) , ((nLbaCount == 0)?1:nLbaCount) ) )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(26) );
					return FALSE;
				}
		}

		if( sDirEnt.attr & 2 )
		{
			if( !XXX_����Ŀ¼����( sDirEnt ) )
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
			DEF_ISO_SET_ERRMSG( GetLangString(27) );
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

	DEF_ISO_SET_ERRMSG( GetLangString(28) );
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
		DEF_ISO_SET_ERRMSG( GetLangString(17) );
		return FALSE;
	}

	//--------------------------------------------------------------------------------------
	const s32 Ԥ������ = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( Ԥ������ > 250 )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(29) );
		return FALSE;
	}

	//-----------------------------------------------------------------
	SISO_DirEnt sDirEnt_File;
	const s32 nDirOffset = FindFile( sDirEnt_File , a_tDirEnt_Path , a_fileName );
	if( nDirOffset < 0 )
		return FALSE;

	if( sDirEnt_File.len != (u8)Ԥ������ )
	{
// 		if( 2352 != m_nSectorSize )
// 		{
// 			DEF_ISO_ERRMSG( L"dirʵ�ʳ��� != Ԥ������" );
// 			return FALSE;
// 		}
// 		else if( dirEnt.len != (u8)(Ԥ������ + 2) )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(30) );
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
		DEF_ISO_SET_ERRMSG( GetLangString(17) );
		return FALSE;
	}
	//-----------------------------------------------------------------
	if( ( L2H(a_tDirEnt_File.size_LE) - a_startOffset ) < a_buflen )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(31) );
		return FALSE;
	}

	u8 buffer[2352];
#if 1
	n32 nLbaIndex = a_startOffset / 2048 + L2H(a_tDirEnt_File.lba_LE);
	n32 nLbaOffset = a_startOffset % 2048;

	s32 nBuflen = a_buflen;
	while( nBuflen > 0 )
	{
		if( !ReadUserData( buffer , nLbaIndex ) )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(32) );
			return FALSE;
		}

		s32 nLen = 2048 - nLbaOffset;
		if( nBuflen < nLen )
			nLen = nBuflen;

		if( nLen != a_buffp.Write( buffer + nLbaOffset , nLen ) )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(33) );
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
static const u8 g_DirData0[] = {
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
		DEF_ISO_SET_ERRMSG( GetLangString(17) );
		return FALSE;
	}
	//--------------------------------------------------------------------------------------
	const s32 nԤ������ = DEF_FN_make_DirLen( strlen(a_fileName) );
	if( nԤ������ > 250 )
	{
		DEF_ISO_SET_ERRMSG( GetLangString(34) );
		return FALSE;
	}
	//-----------------------------------------------------------------
	SISO_DirEnt dirEnt_File;

	BOOL bNew = FALSE;
	s32 nDirOffset;

	s32 ����fileSize = a_insertOffset + a_buflen;
	if( ( nDirOffset = FindFile( dirEnt_File , a_tDirEnt_Path , a_fileName ) ) >= 0 )
	{
		if( dirEnt_File.attr & 2 )
		{
			if( a_bDir )
				return TRUE;

			DEF_ISO_SET_ERRMSG( GetLangString(35) );
			return FALSE;
		}
		else if( a_bDir )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(36) );
			return FALSE;
		}

		if( dirEnt_File.len != (u8)nԤ������ )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(37) );
			return FALSE;
		}

		if( !a_bNew )
			����fileSize = ((����fileSize>L2H(dirEnt_File.size_LE))?(����fileSize):L2H(dirEnt_File.size_LE));

		const s32 ��Ҫ��LBA = DEF_FN_toLBA( ����fileSize , 2048 );
		const s32 ӵ��LBA = DEF_FN_toLBA( L2H(dirEnt_File.size_LE) , 2048 );

		if( ӵ��LBA != ��Ҫ��LBA )
		{
			const s32 nOldLba = L2H(dirEnt_File.lba_LE);

			if( ӵ��LBA > 0 )
			{
				if( !m_pLBA_List->Free( nOldLba ) )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(38) );
					return FALSE;
				}

				if( ��Ҫ��LBA > 0 )
				{
					if( !m_pLBA_List->AllocPos( nOldLba , ��Ҫ��LBA ) )
					{
						dirEnt_File.lba_LE = H2L(-1);
					}
				}
			}

			if( L2H(dirEnt_File.lba_LE) < 0 && ��Ҫ��LBA > ӵ��LBA )
				dirEnt_File.lba_LE = H2L(m_pLBA_List->Alloc( ��Ҫ��LBA ));

			if( L2H(dirEnt_File.lba_LE) < 0 )
			{
				if( !AddLbaCount( ��Ҫ��LBA ) )
				{
					m_pLBA_List->AllocPos( nOldLba , ӵ��LBA );
					DEF_ISO_SET_ERRMSG( GetLangString(39) );
					return FALSE;
				}

				if( !m_pLBA_List->Free( nOldLba ) )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(40) );
					return FALSE;
				}

				dirEnt_File.lba_LE = H2L(m_pLBA_List->Alloc( ��Ҫ��LBA ));

				if( L2H(dirEnt_File.lba_LE) < 0 )
				{
					m_pLBA_List->AllocPos( nOldLba , ӵ��LBA );
					DEF_ISO_SET_ERRMSG( GetLangString(41) );
					return FALSE;
				}
			}

			dirEnt_File.lba_BE = L2B(dirEnt_File.lba_LE);

			//LBA move
			if( nOldLba != L2H(dirEnt_File.lba_LE) )
			{
				if( a_insertOffset != 0 )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(42) );
					return FALSE;
				}
			}
		}
	}
	else if( a_bNew )
	{
		if( a_insertOffset != 0 )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(43) );
			return FALSE;
		}

		const s32 ��Ҫ��LBA = ((DEF_FN_toLBA( a_buflen , 2048 ))==0)?1:(DEF_FN_toLBA( a_buflen , 2048 ));

		bNew = TRUE;

		nDirOffset = 0x30;
		s32 ���޳���;

		nDirOffset = zzz_FindFile( dirEnt_File , a_tDirEnt_Path , nDirOffset , NULL );
		if( nDirOffset < 0 )
			return FALSE;

		���޳��� = nDirOffset - ( nDirOffset % 2048 ) + 2048;
__gtReTest:
		if(	���޳��� <= nDirOffset || ���޳��� > L2H(a_tDirEnt_Path.size_LE) )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(44) );
			return FALSE;
		}

		if( (���޳��� - nDirOffset) < nԤ������ )
		{
			nDirOffset = ���޳���;
			���޳��� += 2048;
			goto __gtReTest;
		}

		memset( &dirEnt_File , 0 , sizeof(dirEnt_File) );

		dirEnt_File.len = (u8)nԤ������;
		//	dirEnt.len_ex;
		{
			s32 LBA_Pos = m_pLBA_List->Alloc( ��Ҫ��LBA );
			if( LBA_Pos < 0 )
			{
				if( !AddLbaCount( ��Ҫ��LBA ) )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(39) );
					return FALSE;
				}

				LBA_Pos = m_pLBA_List->Alloc( ��Ҫ��LBA );

				if( LBA_Pos < 0 )
				{
					DEF_ISO_SET_ERRMSG( GetLangString(41) );
					return FALSE;
				}
			}

			dirEnt_File.lba_LE = H2L(LBA_Pos);
			dirEnt_File.lba_BE = H2B(LBA_Pos);
		}

		//	dirEnt.attr;	dirEnt.sp1;	dirEnt.sp2;

		dirEnt_File.sp3_LE = H2L(1);
		dirEnt_File.sp3_BE = H2B(dirEnt_File.sp3_LE);

		dirEnt_File.nameLen = (u8)strlen(a_fileName);
		//----------------------------------------------------------
	}
	else
	{
		DEF_ISO_SET_ERRMSG( GetLangString(28) );
		return FALSE;
	}

	//--------------------------------------------------------------------------------
	dirEnt_File.size_LE = H2L(a_bDir?(2048):(����fileSize));
	dirEnt_File.size_BE = L2B(dirEnt_File.size_LE);
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
		{
			DEF_ISO_SET_ERRMSG( GetLangString(45) );
			return FALSE;
		}

		memfp.Seek(0);

		buffp_tmp = &memfp;
		bufLen_tmp = sizeof(data);
		offset_tmp = 0;
	}

	if( a_pTime )
	{
		dirEnt_File.time = *a_pTime;
	}
	else if ( a_pImportTime )
	{
		dirEnt_File.time=*a_pImportTime;
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
		const s32 nLbaIndex = offset_tmp / 2048 + L2H(dirEnt_File.lba_LE);
		const s32 nLbaOffset = offset_tmp % 2048;

		u8 szLba[2048];
		if( nLbaOffset != 0 || bufLen_tmp < 2048 )
		{
			if( !ReadUserData( szLba , nLbaIndex ) )
				return FALSE;
		}

		u32 ��Ҫ��ȡ = 2048 - nLbaOffset;

		if( ��Ҫ��ȡ > bufLen_tmp )
			��Ҫ��ȡ = bufLen_tmp;

		u8 buffer[2048];
		if( ��Ҫ��ȡ != buffp_tmp->Read( buffer , ��Ҫ��ȡ ) )
		{
			DEF_ISO_SET_ERRMSG( GetLangString(46) );
			return FALSE;
		}

		memcpy( (szLba + nLbaOffset) , buffer , ��Ҫ��ȡ );

		if( !WriteUserData( szLba , nLbaIndex ) )
			return FALSE;

		bufLen_tmp -= ��Ҫ��ȡ;
		offset_tmp += ��Ҫ��ȡ;
	}

	/*
	if( int ���� = len%m_isoFile.m_nLbaSize )
	{
	���� = m_isoFile.m_nLbaSize - ����;
	u8* tmp = new u8[����];
	if( NULL != tmp )
	{
	memset( tmp , 0xff , ���� );
	m_isoFile.m_ISOfp.Write( tmp , ���� );
	delete[]tmp;
	}
	}*/
	//------------------------------------------------------------------------------------------------------------
	{
		const s32 nLbaIndex = nDirOffset / 2048 + L2H(a_tDirEnt_Path.lba_LE);
		const s32 nLbaOffset = nDirOffset % 2048;

		u8 szLba[2048];
		if( !ReadUserData( szLba , nLbaIndex ) )
			return FALSE;

		memcpy( (szLba + nLbaOffset) , &dirEnt_File , sizeof(dirEnt_File) );

		if( bNew )
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
	tHead.VolumeLBA_Total_le = H2L(nMaxLbaCountX);
	tHead.VolumeLBA_Total_be = H2B(nMaxLbaCountX);

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

void CWQSG_ISO_Interface::CleanErrStr()
{
	m_strErrorStr = L"";
}


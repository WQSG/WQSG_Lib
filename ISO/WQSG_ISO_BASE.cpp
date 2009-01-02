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
bool _tISO_DirEnt::У��()
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
static BOOL XXX_����Ŀ¼����( _tISO_File*const ptISOFile ,  _tISO_DirEnt const*const tDirEnt_in )
{
	if( ( !tDirEnt_in )|| (tDirEnt_in->lba_le < ptISOFile->m_tHead.rootDirEnt.lba_le ) ||
		( (tDirEnt_in->size_le%ptISOFile->m_tHead.LB_Size_LE) != 0 )
		)
	{
		DEF_ERRMSG( *ptISOFile , L"��������" );
		return FALSE;
	}

	_tISO_DirEnt dirEnt;
	if( 0 != WQSG_ISO_ReadDirEnt( ptISOFile , tDirEnt_in  , 0 , &dirEnt , NULL , false ) ||
		( dirEnt.len != 0x30 ) || ( dirEnt.nameLen != 1 ) || (dirEnt.lba_le != tDirEnt_in->lba_le) )
	{
		DEF_ERRMSG( *ptISOFile , L"�����ļ���1" );
		return FALSE;
	}

	if( 0x30 != WQSG_ISO_ReadDirEnt( ptISOFile , tDirEnt_in , 0x30 , &dirEnt , NULL , false ) ||
		( dirEnt.len != 0x30 ) || ( dirEnt.nameLen != 1 ) )
	{
		DEF_ERRMSG( *ptISOFile , L"�����ļ���2" );
		return FALSE;
	}

	for( s32 offset = 0x60 ; offset < tDirEnt_in->size_le ; )
	{
		{
			const s32 newOffset( WQSG_ISO_ReadDirEnt( ptISOFile , tDirEnt_in , offset , &dirEnt , NULL , false ) );
			if( newOffset < 0 )
				return FALSE;

			offset = newOffset;
		}
		if( dirEnt.len == 0 )
			break;

		offset += dirEnt.len;

		const s32 LBAcount = (dirEnt.size_le/ptISOFile->m_tHead.LB_Size_LE) + (( (dirEnt.size_le%ptISOFile->m_tHead.LB_Size_LE)>0 )?1:0);
		if( !ptISOFile->m_pLBA_List->����( dirEnt.lba_le , (LBAcount==0)?1:LBAcount ) )
		{
			DEF_ERRMSG( *ptISOFile , L"����LBAʧ��" );
			return FALSE;
		}

		if( dirEnt.attr & 2 )
			if( !XXX_����Ŀ¼����( ptISOFile , &dirEnt ) )
				return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------
BOOL	WQSG_ISO_Open	( _tISO_File*const ptISOFile , WCHAR const*const strISOPathName , const BOOL bCanWrite )
{
	if( (!ptISOFile) || (!strISOPathName) )
	{
		DEF_ERRMSG( *ptISOFile , L"�����������" );
		return FALSE;
	}
#if _DEBUG
	int ddd = sizeof( _tISO_Head );
#endif
	if( 0x800 != sizeof( _tISO_Head ) )
	{
		DEF_ERRMSG( *ptISOFile , L"�ṹ���Ȳ���" );
		goto __gtErr;
	}

	WQSG_ISO_Close( ptISOFile );
	ptISOFile->m_bCanWrite = bCanWrite;

	if( !ptISOFile->m_ISOfp.OpenFile( strISOPathName , (bCanWrite?3:1) , 3 ) )
	{
		DEF_ERRMSG( *ptISOFile , L"��ISO�ļ�ʧ��" );
		goto __gtErr;
	}

	ptISOFile->m_ISOfp.Seek( 0x10 * 2048 );
	if( sizeof(ptISOFile->m_tHead) != ptISOFile->m_ISOfp.Read( &ptISOFile->m_tHead , sizeof(ptISOFile->m_tHead) ) )
	{
		DEF_ERRMSG( *ptISOFile , L"��ȡISO��Ϣʧ��" );
		goto __gtErr;
	}

	if( 0 != memcmp( ptISOFile->m_tHead.CD001 , "CD001" , 5 ) )
	{
		DEF_ERRMSG( *ptISOFile , L"���� CD001" );
		goto __gtErr;
	}

	if( ( ptISOFile->m_tHead.Volume_Descriptor_Type != 1 ) || ( ptISOFile->m_tHead.Volume_Descriptor_Version != 1 ) )
	{
		CString msg;
		msg.Format( L"��֧�ֵ� ������ %d.%d" , ptISOFile->m_tHead.Volume_Descriptor_Type , ptISOFile->m_tHead.Volume_Descriptor_Version );
		DEF_ERRMSG( *ptISOFile , msg.GetBuffer() );
		goto __gtErr;
	}

	if( !xx_cmpeq( &ptISOFile->m_tHead.LB_Size_LE , &ptISOFile->m_tHead.LB_Size_BE , sizeof(ptISOFile->m_tHead.LB_Size_LE) ) )
	{
		DEF_ERRMSG( *ptISOFile , L"�߼����С����ͬ" );
		goto __gtErr;
	}
	if( ptISOFile->m_tHead.LB_Size_LE != 0x800 )
	{
		CString msg;
		msg.Format( L"�߼���Size��Ϊ 2048 , (%d)" , ptISOFile->m_tHead.LB_Size_LE );
		DEF_ERRMSG( *ptISOFile , msg.GetBuffer() );
		goto __gtErr;
	}

	if( ptISOFile->m_tHead.rootDirEnt.len != sizeof(ptISOFile->m_tHead.rootDitBin) )
	{
		DEF_ERRMSG( *ptISOFile , L"��Ŀ¼ size ����" );
		goto __gtErr;
	}

	if( !xx_cmpeq( &ptISOFile->m_tHead.VolumeLBA_Total_LE , &ptISOFile->m_tHead.VolumeLBA_Total_BE , sizeof(ptISOFile->m_tHead.VolumeLBA_Total_LE) ) )
	{
		DEF_ERRMSG( *ptISOFile , L"LBA��С������ͬ" );
		goto __gtErr;
	}

	if( !xx_cmpeq( &ptISOFile->m_tHead.PathTableSize_LE , &ptISOFile->m_tHead.PathTableSize_BE , sizeof(ptISOFile->m_tHead.PathTableSize_LE) ) )
	{
		DEF_ERRMSG( *ptISOFile , L"LBAPathTableSize��С��ͬ" );
		goto __gtErr;
	}
	if( !xx_cmpeq( &ptISOFile->m_tHead.v6_LE , &ptISOFile->m_tHead.v6_BE , sizeof(ptISOFile->m_tHead.v6_LE) ) )
	{
		DEF_ERRMSG( *ptISOFile , L"v6��С��ͬ" );
		goto __gtErr;
	}
	if( !ptISOFile->m_tHead.rootDirEnt.У��() )
	{
		DEF_ERRMSG( *ptISOFile , L"��Ŀ¼У��ʧ��" );
		goto __gtErr;
	}
	ptISOFile->m_pLBA_List = new CWQSG_PartitionList( ptISOFile->m_tHead.VolumeLBA_Total_LE );
	if( NULL == ptISOFile->m_pLBA_List )
	{
		DEF_ERRMSG( *ptISOFile , L"����LBA�����ʧ��" );
		goto __gtErr;
	}
	if( 0 != ptISOFile->m_pLBA_List->����( ptISOFile->m_tHead.rootDirEnt.lba_le ) )
	{
		DEF_ERRMSG( *ptISOFile , L"����LBA������ʧ��" );
		goto __gtErr;
	}
	{
		if( (ptISOFile->m_tHead.rootDirEnt.size_le < ptISOFile->m_tHead.LB_Size_LE) ||
			((ptISOFile->m_tHead.rootDirEnt.size_le%ptISOFile->m_tHead.LB_Size_LE)!=0) ||
			(!ptISOFile->m_pLBA_List->����( ptISOFile->m_tHead.rootDirEnt.lba_le , ptISOFile->m_tHead.rootDirEnt.size_le/ptISOFile->m_tHead.LB_Size_LE )) )
		{
			DEF_ERRMSG( *ptISOFile , L"���� ��Ŀ¼LBAʧ��" );
			goto __gtErr;
		}
	}
	if( !XXX_����Ŀ¼����( ptISOFile , &ptISOFile->m_tHead.rootDirEnt ) )
		goto __gtErr;

	return TRUE;
__gtErr:
	WQSG_ISO_Close( ptISOFile );
	return FALSE;
}

void	WQSG_ISO_Close	( _tISO_File*const ptISOFile )
{
	if( ptISOFile )
	{
		ptISOFile->m_ISOfp.Close();
		if( ptISOFile->m_pLBA_List )
		{
			delete ptISOFile->m_pLBA_List;
			ptISOFile->m_pLBA_List = NULL;
		}
	}
}

s32		WQSG_ISO_ReadDirEnt	( _tISO_File*const ptISOFile , _tISO_DirEnt const*const tDirEnt_in , const s32 _dirOffset
							 , _tISO_DirEnt*const ptDirEnt , char*const strFileName , const bool bFindFree )
{
	if( (!ptISOFile) || (!ptISOFile->m_ISOfp.IsOpen()) ||
		(!tDirEnt_in) ||
		(tDirEnt_in->lba_le < ptISOFile->m_tHead.rootDirEnt.lba_le ) ||
		(_dirOffset < 0) || (!ptDirEnt) )
	{
		DEF_ERRMSG( *ptISOFile , L"��������" );
		return -1;
	}
	s32 dirOffset( _dirOffset );
	bool bReRead = true;

__gtRead:
	if( ( ptISOFile->m_ISOfp.Seek( (tDirEnt_in->lba_le * ptISOFile->m_tHead.LB_Size_LE) + dirOffset ) &&
		 sizeof(_tISO_DirEnt) == ptISOFile->m_ISOfp.Read( ptDirEnt , sizeof(_tISO_DirEnt) ) ) )
	{
		if( 0 == ptDirEnt->len )
		{
			s32 ����ֵ = dirOffset - ( dirOffset % ptISOFile->m_tHead.LB_Size_LE ) + ptISOFile->m_tHead.LB_Size_LE;
			if( (!bFindFree) && bReRead && (����ֵ > dirOffset) && (����ֵ < tDirEnt_in->size_le) )
			{
				bReRead = false;
				dirOffset = ����ֵ;
				goto __gtRead;
			}
			return dirOffset;
		}

		if( ptDirEnt->У��() )
		{
			if( strFileName && (ptDirEnt->nameLen == ptISOFile->m_ISOfp.Read( strFileName , ptDirEnt->nameLen ) ) )
				strFileName[ptDirEnt->nameLen] = '\0';

			return dirOffset;
		}
		DEF_ERRMSG( *ptISOFile , L"�����Ŀ¼��" );
		return -1;
	}
	DEF_ERRMSG( *ptISOFile , L"��ȡĿ¼��ʧ��" );
	return -1;
}

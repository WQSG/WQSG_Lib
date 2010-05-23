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
#ifndef _WQSG_ISO_BASE_
#define _WQSG_ISO_BASE_

#include "WQSG_ISO_Raw.h"

class CWQSG_ISO_Base : public CWQSG_ISO_Raw
{
 	CWQSG_PartitionList* m_pLBA_List; //扇区分配表
	BOOL InitLbaList( u32 a_uLbaCount );
protected:
	SISO_Head2048 m_tHead;
protected:
	CWQSG_ISO_Base();
	virtual ~CWQSG_ISO_Base();
	//-----------------------------------------------------------------------------
	BOOL Open( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite );
	void Close();

	virtual BOOL IsOpen()const
	{
		return CWQSG_ISO_Raw::IsOpen();
	}
	//-----------------------------------------------------------------------------
public:
	s32 ReadDirEnt( SISO_DirEnt& a_tDirEnt , char*const a_strFileName ,
		const SISO_DirEnt& a_ParentDirEnt , s32 a_nOffset , BOOL a_bNext );

	BOOL ReadFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset );

	BOOL ReadFile( const SISO_DirEnt& a_tDirEnt_File ,
		CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset );

	BOOL WriteFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_bNew , const BOOL a_bDir , const SIsoTime* a_pTime );

	BOOL WriteFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		const void* a_buffer , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_bNew , const SIsoTime* a_pTime );

	BOOL CreateDir( const SISO_DirEnt& a_tDirEnt_Path , char const*const a_dirName );
	//-----------------------------------------------------------------------------
private:
	inline BOOL IsDirEnt( const SISO_DirEnt& a_ParentDirEnt );
	inline BOOL XXX_遍历目录申请( const SISO_DirEnt& a_ParentDirEnt );
	//-----------------------------------------------------------------------------
	inline s32 zzz_FindFile( SISO_DirEnt& a_DirEnt , const SISO_DirEnt& a_ParentDirEnt ,
		s32 a_offset , const char*const a_strFileName );
public:
	BOOL GetRootDirEnt( SISO_DirEnt& a_DirEnt )
	{
		if( IsOpen() )
		{
			memcpy( &a_DirEnt , &m_tHead.rootDirEnt , sizeof(SISO_DirEnt) );
			return TRUE;
		}
		return FALSE;
	}

	//成功返回 dirOffset , 失败 -1
	s32 FindFile( SISO_DirEnt& a_DirEnt , const SISO_DirEnt& a_ParentDirEnt , const char*const a_strFileName );

	inline n32 GetMaxLbaCount()const
	{
		return m_pLBA_List?m_pLBA_List->GetMaxLbaCount():0;
	}

	BOOL AddLbaCount( n32 a_nLbaCount );

	inline n32 GetPerLbaSize()const
	{
		return CWQSG_ISO_Raw::GetPerLbaSize();
	}

	inline void GetFreeInfo( u32* a_puMaxFreeBlock , u32* a_puFreeLbaCount , u32* a_puFreeBlockCount )const
	{
		m_pLBA_List->GetFreeInfo( a_puMaxFreeBlock , a_puFreeLbaCount , a_puFreeBlockCount );
	}

	inline BOOL GetBlockInfo( s32 a_nSt , u32* a_puLen , bool* a_pbUse )const
	{
		return m_pLBA_List?m_pLBA_List->GetBlockInfo( a_nSt , a_puLen , a_pbUse )!=false:FALSE;
	}

	inline BOOL GetHead( SISO_Head2048& a_Head )const
	{
		if( IsOpen() )
		{
			memcpy( &a_Head , &m_tHead , sizeof(m_tHead) );
			return TRUE;
		}
		return FALSE;
	}
};

enum EWqsgIsoType
{
	E_WIT_UNKNOWN,
	E_WIT_PsxISO,
	E_WIT_UMD,

	E_WIT_MAX,
};

class CWQSG_ISO_Interface : public CWQSG_ISO_Base
{
	const EWqsgIsoType m_eIsoType;
protected:
	CWQSG_ISO_Interface( EWqsgIsoType a_eIsoType )
		: m_eIsoType(a_eIsoType)
	{}
	//------------------------------------------
public:
	virtual ~CWQSG_ISO_Interface(void){}
	//------------------------------------------
public:
	virtual	BOOL OpenISO( const WCHAR*const a_isoPathName , const BOOL a_bCanWrite ) = 0;
	virtual	void CloseISO() = 0;

	inline EWqsgIsoType GetIsoType()const
	{
		return m_eIsoType;
	}

	const WCHAR* GetErrStr()const;

	virtual BOOL IsOpen()const
	{
		return CWQSG_ISO_Base::IsOpen();
	}
	//------------------------------------------
};

#endif

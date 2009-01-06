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
#pragma pack(1)
#pragma pack()

class CWQSG_ISO_Base : public CWQSG_ISO_Raw
{
 	CWQSG_PartitionList* m_pLBA_List; //扇区分配表
protected:
	SISO_Head2048 m_tHead;
protected:
	CWQSG_ISO_Base();
	virtual ~CWQSG_ISO_Base();
	//-----------------------------------------------------------------------------
	BOOL Open( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite );
	void Close();

	virtual BOOL IsOpen()
	{
		return CWQSG_ISO_Raw::IsOpen();
	}
	//-----------------------------------------------------------------------------
public:
	s32 ReadDirEnt( SISO_DirEnt& a_tDirEnt , char*const a_strFileName ,
		const SISO_DirEnt& a_ParentDirEnt , s32 a_nOffset , BOOL a_bNext );

	BOOL ReadFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_startOffset );

	BOOL WriteFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		CWQSG_xFile& a_buffp , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_bNew , const BOOL a_bDir );

	BOOL WriteFile( const SISO_DirEnt& a_tDirEnt_Path , const char*const a_fileName ,
		const void* a_buffer , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_bNew );

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
};

#endif

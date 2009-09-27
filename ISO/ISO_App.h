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
#pragma once

//#include <atlstr.h>
#include <WQSG.h>
#include <vector>
#include "../ISO/WQSG_ISO_BASE.h"

struct SIsoFileData
{
	char name[MAX_PATH];
	s32 size;
	s32 lba;
	BOOL isDir;
};

struct SIsoFileFind
{
	s32 m_nOffset;
	SISO_DirEnt m_DirEnt;
};

class CISO_App
{
	typedef std::vector<SIsoFileFind*> TSIsoFileFindMgrVector;
	TSIsoFileFindMgrVector m_Objs;

	CWQSG_ISO_Interface* m_pIso;
	CString m_strLastErr;

	inline BOOL GetPathDirEnt( SISO_DirEnt& a_tDirEnt , const CStringA a_path );
 	inline BOOL zzz_CreateDir( CStringW a_strPathName , CStringA a_strName , CStringA a_strPath );
 	inline BOOL zzz_导入文件夹( CStringW a_strPathName , CStringA a_path );
 	inline BOOL zzz_WriteFile( CStringW strPathName , CWQSG_xFile& a_InFp , CStringA a_strName ,
		CStringA a_strPath , const s32 a_offset , const BOOL a_isNew  );

	inline BOOL zzz_GetFileData( SISO_DirEnt& a_tDirEnt , CStringA a_pathA , CStringA a_nameA );
public:
	CISO_App(void);
	~CISO_App(void);

	BOOL OpenISO( CStringW a_ISO_PathName , const BOOL a_bCanWrite  , EWqsgIsoType a_eType );

	inline void CloseISO()
	{
		if( m_pIso )
		{
			m_pIso->CloseISO( );
			delete m_pIso;
			m_pIso = NULL;
		}
	}

	inline BOOL IsOpen()const
	{
		return m_pIso?m_pIso->IsOpen():FALSE;
	}

	inline CString GetErrStr()const
	{
		return m_strLastErr;
 	}

 	BOOL 导入文件( CStringW a_strPathName , CStringA a_path , const s32 a_offset );
 	BOOL 写文件偏移( CStringA a_pathA , CStringA a_nameA , s32 a_oldOffset , CStringW a_inFileName );
 	BOOL 替换文件( CStringA a_pathA , CStringA a_nameA , CStringW a_inFileName );
	BOOL 导出文件( CStringW a_strPathName , CStringA a_pathA , CStringA a_nameA );
	BOOL 导出文件夹( CStringW a_strPath , CStringA a_pathA );

	BOOL 导入文件包( CWQSG_xFile& a_InFp );

	BOOL GetFileData( SIsoFileData& a_data , CStringA a_pathA , CStringA a_nameA );

	SIsoFileFind* FindIsoFile( CStringA a_pathA );
	BOOL FindNextIsoFile( SIsoFileFind* a_handle , SIsoFileData& a_data );
	void CloseFindIsoFile( SIsoFileFind* a_handle );

	inline n32 GetMaxLbaCount()const
	{
		return m_pIso?m_pIso->GetMaxLbaCount():0;
	}

	inline BOOL AddLbaCount( n32 a_nLbaCount )
	{
		return m_pIso?m_pIso->AddLbaCount( a_nLbaCount ):FALSE;
	}

	inline n32 GetPerLbaSize()const
	{
		return m_pIso?m_pIso->GetPerLbaSize():0;
	}

	inline void GetFreeInfo( u32* a_puMaxFreeBlock , u32* a_puFreeLbaCount , u32* a_puFreeBlockCount )const
	{
		m_pIso->GetFreeInfo( a_puMaxFreeBlock , a_puFreeLbaCount , a_puFreeBlockCount );
	}

	inline BOOL GetBlockInfo( s32 a_nSt , u32* a_puLen , bool* a_pbUse )const
	{
		return m_pIso?m_pIso->GetBlockInfo( a_nSt , a_puLen , a_pbUse )!=false:FALSE;
	}
};

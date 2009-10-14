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
	SIsoTime time;
};

struct SIsoFileFind
{
	s32 m_nOffset;
	SISO_DirEnt m_DirEnt;
};

#define DEF_WQSG_IsoPatch_Head_Magic "WQSG-IsoFilePack"
enum E_WQSG_IsoPatch_Mask
{
	E_WIPM_CRC32 = WQSG_BIT(0),

	E_WIPM_MAX,
};
#pragma pack(push,1)
struct SWQSG_IsoPatch_Head
{
	u8 m_Magic[16];//DEF_WQSG_IsoPatch_Head_Magic "WQSG-IsoFilePack"
	u8 m_Ver[4];
	n32 m_nFileCount;
	n64 m_nSize;
	u32 m_uMask;
	SISO_Head2048 m_Head;
	u32 m_uCrc32;
};

struct SWQSG_IsoPatch_Block
{
	u16 m_uSize;
	u32 m_uCrc32;
	char m_PathName[256];
	char m_FileName[256];
	u32 m_uFileSize;
	u32 m_uOldFileCrc32;
	SIsoTime m_time;
};

#pragma pack(pop)


class CISO_App
{
	typedef std::vector<SIsoFileFind*> TSIsoFileFindMgrVector;
	TSIsoFileFindMgrVector m_Objs;

	CWQSG_ISO_Interface* m_pIso;
	CString m_strLastErr;

	inline BOOL GetPathDirEnt( SISO_DirEnt& a_tDirEnt , const CStringA a_path );
 	inline BOOL zzz_CreateDir( CStringW a_strPathName , CStringA a_strName , CStringA a_strPath );
 	inline BOOL zzz_导入文件夹( CStringW a_strPathName , CStringA a_path );
 	inline BOOL zzz_WriteFile( CStringW a_strPathName , CWQSG_xFile& a_InFp , CStringA a_strName ,
		CStringA a_strPath , const s32 a_offset , const BOOL a_isNew , const SIsoTime* a_pTime );

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

	inline BOOL IsCanWrite()const
	{
		return IsOpen()?m_pIso->IsCanWrite():FALSE;
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

	n32 导入文件包( CWQSG_xFile& a_InFp , BOOL a_bCheckCrc32 );
	BOOL 生成文件包( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , BOOL a_bCheckCrc32 );
protected:
	BOOL zzz_生成文件包_Path( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 , SWQSG_IsoPatch_Head& a_Head );
	BOOL zzz_生成文件包_File( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 ,
		const SISO_DirEnt& a_dirEnt_self , const SISO_DirEnt* a_pDirEnt_old , s32 a_len , s32 a_offset , _m_CRC32& a_crc32_v , const SIsoFileData& a_data_self );
public:
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

	inline BOOL GetHead( SISO_Head2048& a_Head )const
	{
		return m_pIso?m_pIso->GetHead(a_Head):FALSE;
	}

	inline EWqsgIsoType GetIsoType()const
	{
		return m_pIso?m_pIso->GetIsoType():E_WIT_UNKNOWN;
	}
};

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
#ifndef __ISO_APP_H__
#define __ISO_APP_H__
//#include <atlstr.h>
//#include <WQSG.h>
#include <vector>
#include "../../Common/iso/wqsg_iso_base.h"

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
WQSG_WIN_PACK_BEGIN()

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

WQSG_WIN_PACK_END()

class CISO_App
{
	typedef std::vector<SIsoFileFind*> TSIsoFileFindMgrVector;
	TSIsoFileFindMgrVector m_Objs;

	CWQSG_ISO_Interface* m_pIso;
	CStringW m_strLastErrW;

	CWQSG_StringMgr m_StringMgr;
	const WCHAR*const* m_szUserString;
	u32 m_uUserString;

	inline BOOL GetPathDirEnt( SISO_DirEnt& a_tDirEnt , const CStringA a_strIsoPathA );
 	inline BOOL zzz_CreateDir( BOOL& a_bIsoBreak , CStringA a_strIsoName , CStringA a_strIsoPath );
 	inline BOOL zzz_WriteFile( BOOL& a_bIsoBreak , CStringW a_strInPathName , CWQSG_xFile& a_InFp , CStringA a_strIsoName ,
		CStringA a_strIsoPath , const s32 a_nOffset , const BOOL a_bNew , const SIsoTime* a_pTime );

	inline BOOL zzz_GetFileData( SISO_DirEnt& a_tDirEnt , CStringA a_strPathA , CStringA a_strNameA );
	inline void SetErr( const WCHAR* a_pFmt , ... )
	{
		va_list v;
		va_start( v , a_pFmt );
		m_strLastErrW.FormatV( a_pFmt , v );
		va_end(v);

		if( m_pIso )
		{
			m_strLastErrW += L"\r\n=============================\r\n";
			m_strLastErrW += m_pIso->GetErrStr();
			m_pIso->CleanErrStr();
		}
	}

public:
	CISO_App(void);
	~CISO_App(void);

	BOOL OpenISO( CStringW a_strPathName , const BOOL a_bCanWrite  , EWqsgIsoType a_eType );

	inline void CloseISO();

	inline BOOL IsOpen()const;
	inline BOOL IsCanWrite()const;
	inline const CStringW& GetErrStr()const;
	inline void CleanErrStr()
	{
		m_strLastErrW = L"";
	}
public:
	BOOL ImportFile( BOOL& a_bIsoBreak , CStringA a_strPathA , CStringA a_strNameA , CStringW a_strInPathName );
	BOOL ImportDir( BOOL& a_bIsoBreak , CStringA a_strIsoPathA , CStringW a_strInPathName );
	BOOL CreateDir( BOOL& a_bIsoBreak , CStringA a_strIsoName , CStringA a_strIsoPath )
	{
		return zzz_CreateDir(a_bIsoBreak, a_strIsoName, a_strIsoPath);
	}

 	BOOL EasyImport(  BOOL& a_bIsoBreak , CStringW a_strInPathName , CStringA a_strIsoPathA );
 	BOOL WriteFile( BOOL& a_bIsoBreak , CStringA a_strIsoPathA , CStringA a_strIsoNameA , s32 a_nOffset , CStringW a_strInPathName );

	BOOL ExportFile( CStringW a_strOutPathName , CStringA a_strIsoPathA , CStringA a_strIsoNameA );
	BOOL ExportDir( CStringW a_strOutPath , CStringA a_strIsoPathA );

	BOOL ImportFilePackage( BOOL& a_bIsoBreak , CWQSG_xFile& a_InFp , BOOL a_bCheckCrc32 );
	BOOL MakeFilePackage( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , BOOL a_bCheckCrc32 );
protected:
	BOOL zzz_MakeFilePackage_Path( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 , SWQSG_IsoPatch_Head& a_Head );
	BOOL zzz_MakeFilePackage_File( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 ,
		const SISO_DirEnt& a_dirEnt_self , const SISO_DirEnt* a_pDirEnt_old , s32 a_len , s32 a_offset , CCrc32& a_crc32_v , const SIsoFileData& a_data_self );
public:
	BOOL GetFileData( SIsoFileData& a_data , CStringA a_strPathA , CStringA a_strNameA );

	SIsoFileFind* FindIsoFile( CStringA a_strIsoPathA );
	BOOL FindNextIsoFile( SIsoFileFind* a_handle , SIsoFileData& a_data );
	void CloseFindIsoFile( SIsoFileFind* a_handle );

	inline n32 GetMaxLbaCount()const;
	inline BOOL AddLbaCount( n32 a_nLbaCount );
	inline n32 GetPerLbaSize()const;
	inline void GetFreeInfo( u32* a_puMaxFreeBlock , u32* a_puFreeLbaCount , u32* a_puFreeBlockCount )const;
	inline BOOL GetBlockInfo( s32 a_nSt , u32* a_puLen , bool* a_pbUse )const;
	inline BOOL GetHead( SISO_Head2048& a_Head )const;
	inline EWqsgIsoType GetIsoType()const;

	inline void SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString );
	inline const WCHAR* GetLangString( size_t a_uIndex )const;
	static u32 GetDefaultLangStringCount();

	inline void Base_SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString );

	static u32 Base_GetDefaultLangStringCount()
	{
		return CWQSG_ISO_Interface::GetDefaultLangStringCount();
	}

	BOOL Flush( void )
	{
		return ( m_pIso )?m_pIso->Flush():FALSE;
	}
};

inline void CISO_App::CloseISO()
{
	if( m_pIso )
	{
		m_pIso->CloseISO( );
		delete m_pIso;
		m_pIso = NULL;
	}
}

inline BOOL CISO_App::IsOpen()const
{
	return m_pIso?m_pIso->IsOpen():FALSE;
}

inline BOOL CISO_App::IsCanWrite()const
{
	return IsOpen()?m_pIso->IsCanWrite():FALSE;
}

inline const CStringW& CISO_App::GetErrStr()const
{
	return m_strLastErrW;
}


inline n32 CISO_App::GetMaxLbaCount()const
{
	return m_pIso?m_pIso->GetMaxLbaCount():0;
}

inline BOOL CISO_App::AddLbaCount( n32 a_nLbaCount )
{
	return m_pIso?m_pIso->AddLbaCount( a_nLbaCount ):FALSE;
}

inline n32 CISO_App::GetPerLbaSize()const
{
	return m_pIso?m_pIso->GetPerLbaSize():0;
}

inline void CISO_App::GetFreeInfo( u32* a_puMaxFreeBlock , u32* a_puFreeLbaCount , u32* a_puFreeBlockCount )const
{
	m_pIso->GetFreeInfo( a_puMaxFreeBlock , a_puFreeLbaCount , a_puFreeBlockCount );
}

inline BOOL CISO_App::GetBlockInfo( s32 a_nSt , u32* a_puLen , bool* a_pbUse )const
{
	return m_pIso?m_pIso->GetBlockInfo( a_nSt , a_puLen , a_pbUse )!=false:FALSE;
}

inline BOOL CISO_App::GetHead( SISO_Head2048& a_Head )const
{
	return m_pIso?m_pIso->GetHead(a_Head):FALSE;
}

inline EWqsgIsoType CISO_App::GetIsoType()const
{
	return m_pIso?m_pIso->GetIsoType():E_WIT_UNKNOWN;
}

inline void CISO_App::SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString )
{
	m_StringMgr.SetString( a_szUserString , a_uUserString );
}

inline const WCHAR* CISO_App::GetLangString( size_t a_uIndex )const
{
	return m_StringMgr.GetString( a_uIndex );
}

inline void CISO_App::Base_SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString )
{
	m_szUserString = a_szUserString;
	m_uUserString = a_uUserString;
	if( m_pIso )
		m_pIso->SetLangString( a_szUserString , a_uUserString );
}

#endif //__ISO_APP_H__

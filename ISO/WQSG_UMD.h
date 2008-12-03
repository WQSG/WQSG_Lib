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
#include "WQSG_ISO_BASE.h"
class CWQSG_UMD
{
public:
	//------------------------------------------
private:
protected:
	_tISO_File		m_isoFile;
	//----------------------------------------------
	__i__	s32		zzz_FindFile( const _tISO_DirEnt& tDirEnt_in , s32 offset , char const*const strFileName , _tISO_DirEnt& tDirEnt  , const bool bFindFree );
public:
	CStringW	GetErrStr(){	return m_isoFile.m_errStr;	}
	//------------------------------------------
	CWQSG_UMD(void);
	virtual ~CWQSG_UMD(void);
	//------------------------------------------
	virtual	BOOL	OpenISO( WCHAR const*const isoPathName , const BOOL bCanWrite );
	virtual	void	CloseISO();
	//³É¹¦·µ»Ø dirOffset , Ê§°Ü -1
	__i__	s32		FindFile( const _tISO_DirEnt& tDirEnt_in , char const*const strFileName , _tISO_DirEnt& tDirEnt );
	//------------------------------------------
	__i__	BOOL	WriteFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
		, const void* buffer , const s32 buflen , const s32 insertOffset , const BOOL isNew );
	__i__	BOOL	WriteFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
		, CWQSG_xFile& buffp , const s32 buflen , const s32 insertOffset , const BOOL isNew , const BOOL isDir );
	__i__	BOOL	ReadFile( const _tISO_DirEnt& tDirEnt_in , char const*const fileName
		, CWQSG_xFile& buffp , const s32 buflen , const s32 startOffset );
	//------------------------------------------
	__i__	BOOL	IsOpen(){	return m_isoFile.m_ISOfp.IsOpen();	}
	//------------------------------------------
	__i__	BOOL	CreateDir( const _tISO_DirEnt& tDirEnt_in , char const*const dirName );
};
class CWQSG_UMD_kernel :public CWQSG_UMD
{
public:
	CWQSG_UMD_kernel(){}
	~CWQSG_UMD_kernel(){}
	s32		ReadDirEnt	( const _tISO_DirEnt& tDirEnt_in , const s32 dirOffset , _tISO_DirEnt& tDirEnt , char*const strFileName )
	{
		return ::WQSG_ISO_ReadDirEnt( &m_isoFile , &tDirEnt_in , dirOffset , &tDirEnt , strFileName , false );
	}
	BOOL	GetRootDirEnt( _tISO_DirEnt& tDirEnt )
	{
		if( IsOpen() )
		{
			memcpy( &tDirEnt , &m_isoFile.m_tHead.rootDirEnt , sizeof(tDirEnt) );
			return TRUE;
		}
		return FALSE;
	}
};
/**/
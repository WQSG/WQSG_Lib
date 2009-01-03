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

class CWQSG_UMD : public CWQSG_ISO_Base
{
public:
	//------------------------------------------
private:
protected:
	//----------------------------------------------
public:
	inline	CStringW	GetErrStr()
	{
		return m_strErrorStr;
	}
	//------------------------------------------
	CWQSG_UMD(void);
	virtual ~CWQSG_UMD(void);
	//------------------------------------------
	virtual	BOOL	OpenISO( const WCHAR*const a_isoPathName , const BOOL a_bCanWrite );
	virtual	void	CloseISO();
	//------------------------------------------
	BOOL GetRootDirEnt( _tISO_DirEnt& a_tDirEnt )
	{
		if( IsOpen() )
		{
			memcpy( &a_tDirEnt , &m_pHead0->rootDirEnt , sizeof(a_tDirEnt) );
			return TRUE;
		}
		return FALSE;
	}

	s32 ReadDirEnt( const _tISO_DirEnt& tDirEnt_in , const s32 dirOffset , _tISO_DirEnt& tDirEnt , char*const strFileName )
	{
		return CWQSG_ISO_Base::ReadDirEnt( tDirEnt_in , dirOffset , tDirEnt , strFileName , false );
	}
};

class CWQSG_PsxISO : public CWQSG_ISO_Base
{
public:
	//------------------------------------------
private:
protected:
	//----------------------------------------------
public:
	inline	CStringW	GetErrStr()
	{
		return m_strErrorStr;
	}
	//------------------------------------------
	CWQSG_PsxISO(void){}
	virtual ~CWQSG_PsxISO(void){}
	//------------------------------------------
	virtual	BOOL	OpenISO( const WCHAR*const a_isoPathName , const BOOL a_bCanWrite );
	virtual	void	CloseISO();
	//------------------------------------------
};

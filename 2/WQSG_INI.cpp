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
#include "StdAfx.h"
#include "WQSG_INI.h"
/*
CWQSG_INI::CWQSG_INI( CString PathName , CString MainNodeName )
: m_PathName( m_PathName )
, m_MainNodeName( MainNodeName )
{
}

CWQSG_INI::~CWQSG_INI(void)
{
}

BOOL CWQSG_INI::Set( CString SubNodeName , CString ���� , CString val )
{
	return WritePrivateProfileStringW( (SubNodeName.GetLength() <= 0)?NULL:SubNodeName ,
		(����.GetLength() <= 0)?NULL:���� , val , m_PathName );
}

BOOL CWQSG_INI::Get( CString SubNodeName , CString ���� , CString& val , CString  Ĭ�� )
{
	val = Ĭ��;
	WCHAR rev[1024];
	DWORD len = GetPrivateProfileStringW( SubNodeName , ���� , Ĭ�� , rev , 1024 , m_PathName );

	if( 0 == len )
		return FALSE;

	val = rev;
	return TRUE;
}*/
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
#include "WQSG_UMD.h"
//--------------------------------------------------------------------------------------------------
CWQSG_UMD::CWQSG_UMD(void)
: CWQSG_ISO_Interface(E_WIT_UMD)
{
}

CWQSG_UMD::~CWQSG_UMD(void)
{
	CloseISO();
}

BOOL CWQSG_UMD::OpenISO( const WCHAR*const a_isoPathName  , const BOOL a_bCanWrite )
{
	if( CWQSG_ISO_Interface::Open( a_isoPathName , a_bCanWrite  ) )
	{
		if( m_tHead.FileStructureVersion == 2 || m_tHead.FileStructureVersion == 1 )
			return TRUE;
		DEF_ISO_ERRMSG( L"文件结构版本不为 2 or 1" );
	}

	CloseISO();
	return FALSE;
}

void CWQSG_UMD::CloseISO()
{
	CWQSG_ISO_Interface::Close();
}

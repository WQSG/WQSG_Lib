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
#include "../../../Interface/wqsg_def.h"
/*<A HREF=\"http://www.help.com\">Website</A>,*/

INT_PTR WQSG_About( HICON a_hIcon , HWND a_hWndParent ,
				   const WCHAR* a_pTitle , const WCHAR* a_pAppName ,
				   const WCHAR* a_pUrl , const WCHAR* a_pProgrammed );

WQSG_WIN_PACK_BEGIN()

struct SWQSG_AboutDlgID
{
	int m_nIcon;
	int m_nAppName;
	int m_nLink;
	int m_nText0;
	int m_nEdit;
}WQSG_LINUX_PACK;

WQSG_WIN_PACK_END()

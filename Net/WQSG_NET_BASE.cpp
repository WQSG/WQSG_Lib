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
#include "WQSG_NET_BASE.h"
//-------------------------------------------------
static	WSADATA		wsadata;
static	WORD		wVersionRequested;
static	BOOL		bInit = FALSE;
BOOL	WQSG_NET_Init( const BYTE mainVersion , const BYTE subVersion )
{
	if( bInit )
		return FALSE;

	wVersionRequested = MAKEWORD( subVersion , mainVersion );
	if( 0 != WSAStartup( wVersionRequested , &wsadata) )
		return FALSE;

	if( LOBYTE( wsadata.wVersion ) != subVersion ||
		HIBYTE( wsadata.wVersion ) != mainVersion )
	{
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}
void	WQSG_NET_UnInit( void )
{
	if( bInit )
		WSACleanup();
}
BOOL	WQSG_NET_Is_Init( void ){	return bInit;}

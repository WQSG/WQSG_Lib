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
#ifndef	__WQSG_NET_BASE_H__
#define	__WQSG_NET_BASE_H__
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
BOOL	WQSG_NET_Init( const unsigned char mainVersion = 1 , const unsigned char subVersion = 1 );
void	WQSG_NET_UnInit( void );
BOOL	WQSG_NET_Is_Init( void );
//-------------------------------------------------
#endif

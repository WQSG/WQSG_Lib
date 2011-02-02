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
#include "../Interface/WQSG_def.h"
__i__
class CWD_BIN
{
public:
	u8*		BIN;
	u32		LEN;
	inline	CWD_BIN(void):BIN(NULL),LEN(0){}
	virtual	inline	~CWD_BIN(void){delete[]BIN;}
	inline	u8*	SetBufSize( u32 size )
	{
		delete[]BIN;
		return ( BIN = new u8[size] );
	}
};
//------------------------------------------------------------------

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
#ifndef __WQSG_LIB_H__
#define	__WQSG_LIB_H__

#if WIN32
#include "WQSG.h"
#include "ISO/WQSG_UMD.h"
//---------------------------------------------------------
#if defined(_DEBUG)
#if defined(_DLL)///------------------------------------
#pragma comment(lib, "WQSG_lib_Debug_MD.lib")
#else
#pragma comment(lib, "WQSG_lib_Debug_MT.lib")
#endif
#else///------------------------------------
#if defined(_DLL)
#pragma comment(lib, "WQSG_lib_Release_MD.lib")
#else
#pragma comment(lib, "WQSG_lib_Release_MT.lib")
#endif
#endif///------------------------------------
//---------------------------------------------------------
#endif

#endif
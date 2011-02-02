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
#ifndef __WQSG_LIB_INCLUDE_H__
#define __WQSG_LIB_INCLUDE_H__

#include "WQSG_Lib.h"

#if defined(_DEBUG) && defined(_DLL) && defined(_X86_)
#pragma comment(lib, "WQSG_lib_MDD_x86.lib")
#endif

#if defined(_DEBUG) && !defined(_DLL) && defined(_X86_)
#pragma comment(lib, "WQSG_lib_MTD_x86.lib")
#endif

#if !defined(_DEBUG) && defined(_DLL) && defined(_X86_)
#pragma comment(lib, "WQSG_lib_MDR_x86.lib")
#endif

#if !defined(_DEBUG) && !defined(_DLL) && defined(_X86_)
#pragma comment(lib, "WQSG_lib_MTR_x86.lib")
#endif
//-----------------------------------------------------------------
#if defined(_DEBUG) && defined(_DLL) && defined(_AMD64_)
#pragma comment(lib, "WQSG_lib_MDD_x64.lib")
#endif

#if defined(_DEBUG) && !defined(_DLL) && defined(_AMD64_)
#pragma comment(lib, "WQSG_lib_MTD_x64.lib")
#endif

#if !defined(_DEBUG) && defined(_DLL) && defined(_AMD64_)
#pragma comment(lib, "WQSG_lib_MDR_x64.lib")
#endif

#if !defined(_DEBUG) && !defined(_DLL) && defined(_AMD64_)
#pragma comment(lib, "WQSG_lib_MTR_x64.lib")
#endif

#endif //__WQSG_LIB_INCLUDE_H__

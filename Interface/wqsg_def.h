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
#ifndef __WQSG_DEF_H__
#define __WQSG_DEF_H__

#define ___SW_ÆÁ±Î¾¯¸æ___ 1

#define WQSG_BIT(x) (1<<(x))

#if WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define		__i__ 

typedef signed	char		s8;
typedef signed	short		s16;
typedef signed	long		s32;
typedef signed	__int64		s64;

typedef s8	n8;
typedef s16	n16;
typedef s32	n32;
typedef s64	n64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned __int64	u64;


typedef unsigned int	uint;
#endif

#if defined(_M_IX86)
#define WQSG_BIG_ENDIAN 0
#elif defined(_M_AMD64)
#error _M_AMD64
#define WQSG_BIG_ENDIAN 0
#else
#error WQSG_BIG_ENDIAN 1
#define WQSG_BIG_ENDIAN 1
#endif

#endif
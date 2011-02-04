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

#if WIN32
typedef signed	char		s8;
typedef signed	short		s16;
typedef signed	long		s32;
typedef signed	__int64		s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned __int64	u64;

typedef unsigned int	uint;
#elif defined(CELL_DATA_DIR)// end WIN32
typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

typedef int BOOL;
// #define TRUE 1
// #define FALSE 0

typedef wchar_t WCHAR;
typedef char TCHAR;

#endif //end

typedef s8	n8;
typedef s16	n16;
typedef s32	n32;
typedef s64	n64;

#endif

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
#ifndef __WQSG_IFILE_H__
#define __WQSG_IFILE_H__
#include "wqsg_def.h"

//------------------------------------------
class CWQSG_IFile
{
protected:
	CWQSG_IFile(){}
	virtual	~CWQSG_IFile(){}
public:
	__i__	virtual	u32			Read		( void*const lpBuffre , const u32 len ) = 0;
	__i__	virtual	u32			Write		( const void*const lpBuffre , const u32 len ) = 0;
	__i__	virtual	void		Close		( void )						= 0;
	__i__	virtual	bool		WriteStrW	( const WCHAR*const str );		//
	__i__	virtual	s64			GetFileSize	( void )const					= 0;
	__i__	virtual	bool		SetFileLength( const s64 Length )			= 0;
	__i__	virtual	s64			Tell		( void )const					= 0;
	__i__	virtual	bool		Seek		( const s64 offset )			= 0;
	__i__	virtual	u32			GetCRC32	( void )						= 0;
	__i__	virtual	bool		IsOpen		( void )const					= 0;
	__i__	virtual	bool		IsCanRead	( void )const					= 0;
	__i__	virtual	bool		IsCanWrite	( void )const					= 0;
};
//------------------------------------------
typedef CWQSG_IFile CWQSG_xFile;
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#include "../Common/wqsg_string.h"
inline bool	CWQSG_IFile::WriteStrW	( WCHAR const*const str )
{
	if( NULL == str )
		return false;

	u32 con_i = ::WQSG_strlen(str);

	if( con_i & 0x80000000 )
		return false;

	con_i <<= 1;

	return ( Write( str , con_i ) == con_i );
}

#endif //__WQSG_IFILE_H__

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
#ifndef _WQSG_ISO_BASE_
#define _WQSG_ISO_BASE_
#include <WQSG.h>
#include <atlstr.h>
#define DEF_ERRMSG( __def_tISO_File , __def_msg ) {\
	CString str;str = __FILE__;CString str2;str2.Format( L"file: %s\r\nline: %d\r\n%s" , str.GetBuffer() , __LINE__ , (__def_msg) );\
	(__def_tISO_File).SetErrMsg( str2.GetBuffer() ); }
#pragma pack(1)
struct _tISO_DirEnt
{
	u8	len;
	s8	len_ex;
	s32 lba_le;
	s32 lba_be;
	s32	size_le;
	s32	size_be;
	u8	time[7];
	u8	attr;
	u8	sp1;
	u8	sp2;
	u16	sp3_le;
	u16	sp3_be;
	u8 nameLen;
	bool 校验();
};
struct _tISO_Head
{
	union
	{
		struct
		{
			u8	Volume_Descriptor_Type;			//1
			u8	CD001[5];	//"CD001"
			u8	Volume_Descriptor_Version;		//1
			u8	Unused_1;	//0
		};
		u8	magic[8];
	};
	u8				SystemID[0x20];
	u8				VolumeID[0x20];
	u8				Unused_2[8];			//0
	s32				VolumeLBA_Total_LE;
	s32				VolumeLBA_Total_BE;
	u8				Unused_3[0x20];	//0
	s32				v6_LE;			//
	s32				v6_BE;			//
	s16				LB_Size_LE;
	s16				LB_Size_BE;
	s32				PathTableSize_LE;
	s32				PathTableSize_BE;
	struct
	{
		s32				pathTable1_LE;	//??
		s32				pathTable2_LE;	//??
		s32				pathTable3_BE;	//??
		s32				pathTable4_BE;	//??
	}pathTable;
	union
	{
		_tISO_DirEnt	rootDirEnt;
		u8				rootDitBin[0x22];
	};
	struct
	{
		u8	Volume[0x80];
		u8	Publisher[0x80];
		u8	dataPublisher[0x80];
		u8	App[0x80];
	}Identifier1;
	struct
	{
		u8	CopyrightID[0x25];
		u8	AbstractID[0x25];
		u8	BibliographicID[0x25];
	}Identifier2;
	struct
	{
		u8	Time1[0x11];
		u8	Time2[0x11];
		u8	Time3[0x11];
		u8	Time4[0x11];
	}time;
	u8				FileStructureVersion;	//1 , UMD 2
	u8				Unused_7;				//0
	u8				AppUse[0x200];
	u8				Unused_8[0x28D];		//0
};
/*struct _tISO_DirEntInfo
{
	s32 lba;
	s32	size;
	u8	time[7];
	u8	attr;
};
struct _tISO_HeadInfo
{
	u8		SystemID[0x20];
	u8		VolumeID[0x20];
	s32		VolumeLBA_Total;
	s16		LB_Size;
	s32		PathTableSize;
	u8		FileStructureVersion;	//1 , UMD 2
};*/
#pragma pack()
struct _tISO_File
{
	BOOL					m_bCanWrite;
	_tISO_Head				m_tHead;		//完整的头部
	CWQSG_File				m_ISOfp;		//文件流
	CWQSG_PartitionList*	m_pLBA_List;	//扇区分配表

	CStringW				m_errStr;
	_tISO_File() : m_pLBA_List( NULL ){}
	void SetErrMsg( WCHAR const*const msg )
	{
		m_errStr = msg;
	//	CString str1;str1 = __FILE__;
	//	m_errStr.Format( L"file: %s\r\nline: %d\r\n%s" , str1.GetBuffer() , __LINE__ , msg );
	}
	void SetErrMsg( char const*const msg )
	{
		m_errStr = msg;
	//	CString str1;str1 = __FILE__;
	//	CString str2;str2 = msg;
	//	m_errStr.Format( L"file: %s\r\nline: %d\r\n%s" , str1.GetBuffer() , __LINE__ , str2.GetBuffer() );
	}
};

BOOL	WQSG_ISO_Open		( _tISO_File*const ptISOFile , WCHAR const*const strISOPathName , const BOOL bCanWrite );
void	WQSG_ISO_Close		( _tISO_File*const ptISOFile );
s32		WQSG_ISO_FindFile	( _tISO_File*const ptISOFile , const s32 dirLBA , char const*const fileName , _tISO_DirEnt& dirEntx , s32& offset );
s32		WQSG_ISO_ReadDirEnt	( _tISO_File*const ptISOFile , _tISO_DirEnt const*const tDirEnt_in , const s32 _dirOffset
						 , _tISO_DirEnt*const ptDirEnt , char*const strFileName , const bool bFindFree );
#endif

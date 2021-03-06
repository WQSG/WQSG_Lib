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
#ifndef __WQSG_ISO_RAW__
#define __WQSG_ISO_RAW__
#include "../../Interface/wqsg_def.h"
#include "../cwqsg_str.h"
#include "../wqsg_xfile.h"
#include "../../Win32/wqsg_file.h"

#define DEF_ISO_SET_ERRMSG( __def_fmt , ... ) do{\
	CWQSG_str str;str.Format( L"file: %hs\r\nline: %d\r\n" , __FILE__ , __LINE__ );\
	str.AppendFormat( __def_fmt , __VA_ARGS__  );\
	SetErrMsg( str.GetString() ); }while(0)

WQSG_WIN_PACK_BEGIN()

#ifndef WQSG_LINUX_PACK
#error don't define WQSG_LINUX_PACK
#endif

struct SIsoTime
{
	u8	uYear;
	u8	uMonth;
	u8	uDay;
	u8	uHours;
	u8	uMinutes;
	u8	uSeconds;
	u8	uUnknown;	//24
}WQSG_LINUX_PACK;

struct SISO_DirEnt
{
	u8	len;
	s8	len_ex;
	s32 lba_LE;
	s32 lba_BE;
	s32	size_LE;
	s32	size_BE;
	SIsoTime time;
	u8	attr;
	u8	sp1;
	u8	sp2;
	u16	sp3_LE;
	u16	sp3_BE;
	u8 nameLen;
	bool cheak() const;
}WQSG_LINUX_PACK;

struct SISO_Head2048
{
	union
	{
		struct
		{
			u8	Volume_Descriptor_Type;			//1
			u8	CD001[5];	//"CD001"
			u8	Volume_Descriptor_Version;		//1
			u8	Unused_1;	//0
		}WQSG_LINUX_PACK;
		u8	magic[8];
	}WQSG_LINUX_PACK;
	u8				SystemID[0x20];
	u8				VolumeID[0x20];
	u8				Unused_2[8];			//0
	s32				VolumeLBA_Total_le;
	s32				VolumeLBA_Total_be;
	u8				Unused_3[0x20];	//0
	s32				v6_le;			//
	s32				v6_be;			//
	s16				LB_Size_le;
	s16				LB_Size_be;
	s32				PathTableSize_le;
	s32				PathTableSize_be;
	struct
	{
		s32				pathTable1_le;	//??
		s32				pathTable2_le;	//??
		s32				pathTable3_be;	//??
		s32				pathTable4_be;	//??
	}WQSG_LINUX_PACK pathTable;
	union
	{
		SISO_DirEnt	rootDirEnt;
		u8				rootDitBin[0x22];
	}WQSG_LINUX_PACK;
	struct
	{
		u8	Volume[0x80];
		u8	Publisher[0x80];
		u8	dataPublisher[0x80];
		u8	App[0x80];
	}WQSG_LINUX_PACK Identifier1;
	struct
	{
		u8	CopyrightID[0x25];
		u8	AbstractID[0x25];
		u8	BibliographicID[0x25];
	}WQSG_LINUX_PACK Identifier2;
	struct
	{
		u8	Time1[0x11];
		u8	Time2[0x11];
		u8	Time3[0x11];
		u8	Time4[0x11];
	}WQSG_LINUX_PACK time;
	u8				FileStructureVersion;	//1 , UMD 2
	u8				Unused_7;				//0
	u8				AppUse[0x200];
	u8				Unused_8[0x28D];		//0
}WQSG_LINUX_PACK;

union UISO_Head2352
{
	struct
	{
		u8 m_Sync[12];
		u8 m_XX[3];
		u8 m_uMode;
		union
		{
			struct// _tISO_HeadMode1
			{
				SISO_Head2048 m_Head1;
				u8 m_x1[288];
			}WQSG_LINUX_PACK;
			struct// _tISO_HeadMode2
			{
				u8 m_szMode2[8];
				SISO_Head2048 m_Head2;
				u8 m_x2[280];
			}WQSG_LINUX_PACK;
		}WQSG_LINUX_PACK;
	}WQSG_LINUX_PACK;

	SISO_Head2048 m_Head2048;
}WQSG_LINUX_PACK;

WQSG_WIN_PACK_END()

enum EIsoType
{
	E_IT_Base,

	E_IT_Mode1,

	E_IT_Mode2Form1,
	E_IT_Mode2Form2,
};

class CWQSG_ISO_Raw
{
	BOOL m_bCanWrite;
	CWQSG_File m_ISOfp; //�ļ���

	EIsoType m_eType;
	s32 m_nSectorSize;
	s32 m_nUserDataOffset;

	inline BOOL ReadSectors(void* a_pSector, s32 a_nLBA);
	inline BOOL WriteSectors(void* a_pSector, s32 a_nLBA);

	CWQSG_StringMgr m_StringMgr;
protected:
	CWQSG_ISO_Raw();

	virtual ~CWQSG_ISO_Raw()
	{
		CloseFile();
	}

	BOOL OpenFile( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite );
	void CloseFile( );

	BOOL ReadUserData(void* a_pUserData, s32 a_nLBA);
	BOOL WriteUserData(void* a_pUserData, s32 a_nLBA);

	CWQSG_strW m_strErrorStr;

	inline void SetErrMsg( WCHAR const*const a_msg );
	//inline void SetErrMsg( char const*const a_msg );

	virtual BOOL IsOpen()const
	{
		return m_ISOfp.IsOpen();
	}

	n32 GetPerLbaSize()const;
public:
	inline BOOL IsCanWrite()const;

	inline void SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString );
	inline const WCHAR* GetLangString( size_t a_uIndex )const;

	static u32 GetDefaultLangStringCount();

	inline BOOL Flush( void )
	{
		return m_ISOfp.Flush();
	}

	bool CanCheckLba()const;
	bool CheckLba( s32 a_nLBA );
};

inline void CWQSG_ISO_Raw::SetErrMsg( WCHAR const*const a_msg )
{
	m_strErrorStr = a_msg;
}

// inline void CWQSG_ISO_Raw::SetErrMsg( char const*const a_msg )
// {
// 	m_strErrorStr = a_msg;
// }

inline n32 CWQSG_ISO_Raw::GetPerLbaSize()const
{
	return m_nSectorSize;
}

inline BOOL CWQSG_ISO_Raw::IsCanWrite()const
{
	return m_ISOfp.IsCanWrite();
}

inline void CWQSG_ISO_Raw::SetLangString( const WCHAR*const* a_szUserString , const u32 a_uUserString )
{
	m_StringMgr.SetString( a_szUserString , a_uUserString );
}

inline const WCHAR* CWQSG_ISO_Raw::GetLangString( size_t a_uIndex )const
{
	return m_StringMgr.GetString( a_uIndex );
}

#endif //__WQSG_ISO_RAW__

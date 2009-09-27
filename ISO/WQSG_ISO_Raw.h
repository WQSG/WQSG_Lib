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
#ifndef _WQSG_ISO_RAW_
#define _WQSG_ISO_RAW_
#include <WQSG.h>
#include <atlstr.h>
#define DEF_ISO_ERRMSG( __def_msg ) {\
	CString str;str = __FILE__;CString str2;str2.Format( L"file: %s\r\nline: %d\r\n%s" , str.GetBuffer() , __LINE__ , (__def_msg) );\
	SetErrMsg( str2.GetBuffer() ); }

inline static bool xx_cmpeq( void const*const bufferLE , void const*const bufferBE , const size_t len )
{
	u8 const* buf1 = (u8 const*const)bufferLE;
	u8 const* buf2 = (u8 const*const)bufferBE;

	if( ( buf1 + len ) < buf1 )
		return false;
	if( ( buf2 + (len-1) ) < buf2 )
		return false;

	buf2 += (len-1);
	for( size_t i = 0 , last_i = 0 ; (i >= last_i) && (i < len) ; last_i = i++ )
	{
		if( *buf1 !=  *buf2 )
			return false;

		++buf1,--buf2;
	}
	return true;
}

#pragma pack(1)

struct SIsoTime
{
	u8	uYear;
	u8	uMonth;
	u8	uDay;
	u8	uHours;
	u8	uMinutes;
	u8	uSeconds;
	u8	uUnknown;	//24
};

struct SISO_DirEnt
{
	u8	len;
	s8	len_ex;
	s32 lba_le;
	s32 lba_be;
	s32	size_le;
	s32	size_be;
	SIsoTime time;
	u8	attr;
	u8	sp1;
	u8	sp2;
	u16	sp3_le;
	u16	sp3_be;
	u8 nameLen;
	bool cheak() const;
};

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
		SISO_DirEnt	rootDirEnt;
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
			};
			struct// _tISO_HeadMode2
			{
				u8 m_szMode2[8];
				SISO_Head2048 m_Head2;
				u8 m_x2[280];
			};
		};
	};

	SISO_Head2048 m_Head2048;
};

#pragma pack()

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
	CWQSG_File m_ISOfp; //文件流

	EIsoType m_eType;
	s32 m_nSectorSize;
	s32 m_nUserDataOffset;

	BOOL ReadSectors(void* a_pSector, s32 a_nLBA);
	BOOL WriteSectors(void* a_pSector, s32 a_nLBA);
protected:
	CWQSG_ISO_Raw(){}
	virtual ~CWQSG_ISO_Raw()
	{
		CloseFile();
	}

	BOOL OpenFile( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite );
	void CloseFile( );

	BOOL ReadUserData(void* a_pUserData, s32 a_nLBA);
	BOOL WriteUserData(void* a_pUserData, s32 a_nLBA);

	CStringW m_strErrorStr;

	inline void SetErrMsg( WCHAR const*const a_msg )
	{
		m_strErrorStr = a_msg;
	}

	inline void SetErrMsg( char const*const a_msg )
	{
		m_strErrorStr = a_msg;
	}

	virtual BOOL IsOpen()const
	{
		return m_ISOfp.IsOpen();
	}

	n32 GetPerLbaSize()const
	{
		return m_nSectorSize;
	}
};

#if 0
class CWQSG_ISO_BaseX
{
	inline BOOL XXX_遍历目录申请( const SISO_DirEnt& a_tDirEnt_in );
public:
protected:
	CWQSG_ISO_BaseX();
	virtual ~CWQSG_ISO_BaseX();

	BOOL Open( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite );
	void Close();

	s32 ReadDirEnt( const SISO_DirEnt& a_tDirEnt_in , const s32 a_dirOffset ,
		SISO_DirEnt& a_tDirEnt , char*const a_strFileName , const BOOL a_bFindFree );

	inline s32 SectorSeek( s32 a_nLbaID , s32 a_nLbaOffset );
protected:
	BOOL m_bCanWrite;
	s32 m_nSectorSize;
	s32 m_nLbaSize;
	s32 m_nHeadOffset;

	CWQSG_File m_ISOfp; //文件流
	CWQSG_PartitionList* m_pLBA_List; //扇区分配表

	UISO_Head2352 m_tHead0; //完整的头部
	SISO_Head2048* m_pHead0;

	CStringW m_strErrorStr;

	void SetErrMsg( WCHAR const*const a_msg )
	{
		m_strErrorStr = a_msg;
	}

	void SetErrMsg( char const*const a_msg )
	{
		m_strErrorStr = a_msg;
	}
private:
	inline	s32		zzz_FindFile( const SISO_DirEnt& a_tDirEnt_in , s32 a_offset ,
		const char*const a_strFileName , SISO_DirEnt& a_tDirEnt  , const bool a_bFindFree );
public:
	//成功返回 dirOffset , 失败 -1
	__i__	s32		FindFile( const SISO_DirEnt& a_tDirEnt_in , const char*const a_strFileName , SISO_DirEnt& a_tDirEnt );

	__i__	BOOL	ReadFile( const SISO_DirEnt& a_tDirEnt_in , const char*const a_fileName ,
		CWQSG_xFile& buffp , const s32 a_buflen , const s32 a_startOffset );
	//------------------------------------------
	__i__	BOOL	WriteFile( const SISO_DirEnt& a_tDirEnt_in , const char*const a_fileName ,
		const void* buffer , const s32 a_buflen , const s32 insertOffset , const BOOL isNew );

	__i__	BOOL	WriteFile( const SISO_DirEnt& a_tDirEnt_in , const char*const a_fileName ,
		CWQSG_xFile& buffp , const s32 a_buflen , const s32 a_insertOffset , const BOOL a_isNew , const BOOL a_isDir );
	//------------------------------------------
	__i__	BOOL	CreateDir( const SISO_DirEnt& a_tDirEnt_in , char const*const a_dirName );

	virtual	BOOL	IsOpen()const
	{
		return m_ISOfp.IsOpen();
	}

	BOOL GetRootDirEnt( SISO_DirEnt& a_tDirEnt )
	{
		if( IsOpen() )
		{
			memcpy( &a_tDirEnt , &m_pHead0->rootDirEnt , sizeof(a_tDirEnt) );
			return TRUE;
		}
		return FALSE;
	}

	s32 ReadDirEnt( const SISO_DirEnt& tDirEnt_in , const s32 dirOffset , SISO_DirEnt& tDirEnt , char*const strFileName )
	{
		return ReadDirEnt( tDirEnt_in , dirOffset , tDirEnt , strFileName , false );
	}
};


#endif
#endif

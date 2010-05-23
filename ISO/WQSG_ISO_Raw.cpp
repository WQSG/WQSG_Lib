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
#include "WQSG_ISO_Lang.h"

static inline void GetLastErrorText( CString& a_str , DWORD a_dwErrId )
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		a_dwErrId ,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	a_str = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
}

#include "WQSG_ISO_Raw.h"
bool SISO_DirEnt::cheak()const
{
	if( xx_cmpeq( &lba_le , &lba_be , sizeof(lba_le) ) && lba_le >= 0 &&
		xx_cmpeq( &sp3_le , &sp3_be , sizeof(sp3_le) ) && sp3_le >= 0 &&
		/*xx_cmpeq( &size_le , &size_be , sizeof(size_le) ) &&*/ size_le >= 0 &&
		(len&1) == 0 && len > 0 && len_ex == 0 &&
		nameLen > 0 && nameLen<=127 && size_le >= 0 )
	{
		if( attr & 2 )
		{
			if( (size_le % 2048) != 0 )
				return false;
		}
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------
class CIsoEcc
{
	u32 EDC_LUT[256];
	u8 ECC_F_LUT[256];
	u8 ECC_B_LUT[256];
public:
	CIsoEcc()
	{
		u32 j,nEdc;
		for( u32 i = 0 ; i<256 ; i++ )
		{
			j = (i<<1) ^ ( i&0x80 ?0x011D:0 );
			ECC_F_LUT[i] = (u8)j;
			ECC_B_LUT[i^j] = (u8)i;
			nEdc = i;
			for(j = 0; j<8; j++)
				nEdc = (nEdc>>1) ^ ( nEdc&1 ? 0xD8018001:0 );
			EDC_LUT[i] = nEdc;
		}
	}
	//EDC Size=0x0808=UserDataSize(2048)+SubHeader(8);
	void EDC_Compute(u32& nEDC, u8* pUserData, u32 nSize)
	{
		nEDC = 0;
		while(nSize--)
			nEDC = (nEDC>>8) ^ EDC_LUT[ (nEDC ^ (*pUserData++)) & 0xFF ];
	}

	//ECC P code
	//ECC_Compute(pSector + 0x0C, 86, 24,  2, 86, pSector + 0x081C);
	//ECC Q code
	//ECC_Compute(pSector + 0x0C, 52, 43, 86, 88, pSector + 0x08C8);
	void ECC_Compute(u8* Src, u32 nMajor_count, u32 nMinor_count,
		u32 nMajor_mult, u32 nMinor_inc, u8* Dest)
	{
		UINT nSize = nMajor_count * nMinor_count;
		UINT nMajor, nMinor;
		for(nMajor = 0; nMajor < nMajor_count; nMajor++)
		{
			UINT index = (nMajor >> 1) * nMajor_mult + (nMajor & 1);
			BYTE ECC_A = 0,	ECC_B = 0;
			for(nMinor = 0; nMinor < nMinor_count; nMinor++)
			{
				BYTE temp = Src[index];
				index += nMinor_inc;
				if(index >= nSize) index -= nSize;
				ECC_A ^= temp;
				ECC_B ^= temp;
				ECC_A = ECC_F_LUT[ECC_A];
			}
			ECC_A = ECC_B_LUT[ ECC_F_LUT[ECC_A] ^ ECC_B ];
			Dest[nMajor] = ECC_A;
			Dest[nMajor + nMajor_count] = ECC_A ^ ECC_B;
		}
	}
};
static CIsoEcc g_IsoEcc;
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
static const u8 g_szIsoSync[] = {0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};
//--------------------------------------------------------------------------------------------------
CWQSG_ISO_Raw::CWQSG_ISO_Raw()
: m_StringMgr( g_WQSG_ISO_String , (sizeof(g_WQSG_ISO_String)/sizeof(*g_WQSG_ISO_String)) )
{
}

BOOL CWQSG_ISO_Raw::OpenFile( const WCHAR*const a_strISOPathName , const BOOL a_bCanWrite )
{
	CloseFile();

	if( !m_ISOfp.OpenFile( a_strISOPathName , (a_bCanWrite?3:1) , 3 ) )
	{
		if( a_bCanWrite )
		{
			if( !m_ISOfp.OpenFile( a_strISOPathName , 1 , 3 ) )
			{
				CString str;
				GetLastErrorText( str , GetLastError() );

				DEF_ISO_SET_ERRMSG( str.GetString() );
				return FALSE;
			}
		}
		else
		{
			CString str;
			GetLastErrorText( str , GetLastError() );

			DEF_ISO_SET_ERRMSG( str.GetString() );
			return FALSE;
		}
	}

	UISO_Head2352 tHead;
	m_nSectorSize = sizeof(tHead);

	if( !ReadSectors( &tHead , 0 ) )
	{
		DEF_ISO_SET_ERRMSG( m_StringMgr.GetString(0) );
		goto __gtOpenFileErr;
	}

	if( memcmp( tHead.m_Sync , g_szIsoSync , sizeof(g_szIsoSync) ) != 0 )
		m_nSectorSize = 2048;

	if( !ReadSectors( &tHead , 16 ) )
	{
		DEF_ISO_SET_ERRMSG( m_StringMgr.GetString(0) );
		goto __gtOpenFileErr;
	}

	if( m_nSectorSize != 2048 )
	{
		switch( tHead.m_uMode )
		{
		case 1://Mode1
			m_eType = E_IT_Mode1;
			m_nUserDataOffset = 0x10;
			break;
		case 2://Mode2 Form1
			m_eType = E_IT_Mode2Form1;
			m_nUserDataOffset = 0x18;
			break;
		default:
			DEF_ISO_SET_ERRMSG( m_StringMgr.GetString(1) );
			goto __gtOpenFileErr;
		}
	}
	else
	{
		m_eType = E_IT_Base;
		m_nUserDataOffset = 0;
	}

	return TRUE;
__gtOpenFileErr:
	CloseFile();
	return FALSE;
}

void CWQSG_ISO_Raw::CloseFile( )
{
	m_ISOfp.Close();
}

inline BOOL CWQSG_ISO_Raw::ReadSectors(void* a_pSector, s32 a_nLBA)
{
	if( NULL == a_pSector || a_nLBA < 0 )
	{
		return FALSE;
	}

	if( m_ISOfp.Seek( a_nLBA * (s64)m_nSectorSize ) &&
		m_nSectorSize == m_ISOfp.Read( a_pSector , m_nSectorSize ) )
		return TRUE;

	return FALSE;
}

#pragma warning( push )
#pragma warning( disable : 4996 )
inline static n32 xxx_MakeHeader( n32 a_Mode , s32 a_nLba )
{
	const u32 tmp = a_nLba + 150;

	char szBuffer[10];
	sprintf( szBuffer , "%02X%02d%02d%02d" ,
		a_Mode , tmp%75 , (tmp/75)%60 , tmp/75/60 );

	n32 val;
	sscanf( szBuffer , "%X" , &val );

	return val;
}
#pragma warning( pop )

inline BOOL CWQSG_ISO_Raw::WriteSectors(void* a_pSector, s32 a_nLBA)
{
	if( NULL == a_pSector || a_nLBA < 0 )
	{
		return FALSE;
	}

	const s32 nOffset = a_nLBA * m_nSectorSize;

	switch( m_eType )
	{
	case E_IT_Base:
		break;
	case E_IT_Mode1:
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );
		{
			//HEADER
			u32 temp = xxx_MakeHeader( 1 , a_nLBA );
			memcpy( (u8*)a_pSector+0x0C , &temp , 4 );
			//EDC
			g_IsoEcc.EDC_Compute( temp , (u8*)a_pSector , 0x0810);
			//*(UINT*)(pSector+0x0810) = nOff;
			memcpy( (u8*)a_pSector+0x0810 , &temp , 4 );
			//ECC P code
			g_IsoEcc.ECC_Compute( (u8*)a_pSector + 0x0C, 86, 24,  2, 86, (u8*)a_pSector + 0x081C);
			//ECC Q code
			g_IsoEcc.ECC_Compute( (u8*)a_pSector + 0x0C, 52, 43, 86, 88, (u8*)a_pSector + 0x08C8);
		}
		break;
	case E_IT_Mode2Form1:
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );
		{
			u32 temp;
			//EDC
			g_IsoEcc.EDC_Compute( temp , (u8*)a_pSector+0x10 , 0x0808 );
			//*(UINT*)(pSector+0x0818) = nOff;
			memcpy( (u8*)a_pSector+0x0818 , &temp , 4 );
			//Header=Zero
			memset( (u8*)a_pSector+0x0C , 0 , 4 );
			//ECC P code
			g_IsoEcc.ECC_Compute( (u8*)a_pSector + 0x0C, 86, 24,  2, 86, (u8*)a_pSector + 0x081C);
			//ECC Q code
			g_IsoEcc.ECC_Compute( (u8*)a_pSector + 0x0C, 52, 43, 86, 88, (u8*)a_pSector + 0x08C8);
			//HEADER
			temp = xxx_MakeHeader( 2 , a_nLBA );
			memcpy( (u8*)a_pSector+0x0C , &temp , 4 );
		}
		break;
	default:
		return FALSE;
	}

	if( m_ISOfp.Seek( a_nLBA * (s64)m_nSectorSize ) &&
		m_nSectorSize == m_ISOfp.Write( a_pSector , m_nSectorSize ) )
		return TRUE;

	return FALSE;
}

BOOL CWQSG_ISO_Raw::ReadUserData(void* a_pUserData , s32 a_nLBA)
{
	if( NULL == a_pUserData )
		return FALSE;

	u8 szSectors[2352];
	if( ReadSectors(szSectors , a_nLBA ) )
	{
		memcpy( a_pUserData , szSectors + m_nUserDataOffset , 2048 );
		return TRUE;
	}

	return FALSE;
}

BOOL CWQSG_ISO_Raw::WriteUserData(void* a_pUserData, s32 a_nLBA)
{
	if( !m_ISOfp.IsOpen() )
		return FALSE;

	u8 szSectors[2352];
	memcpy( szSectors + m_nUserDataOffset , a_pUserData , 2048 );

	return WriteSectors( szSectors , a_nLBA );
}
//|||||||||||||||||||||||||||||||||||||||||||||
u32 CWQSG_ISO_Raw::GetDefaultLangStringCount()
{
	return (sizeof(g_WQSG_ISO_String)/sizeof(*g_WQSG_ISO_String));
}

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
#include "wqsg_iso_lang.h"
#include "..\..\Interface\wqsg.h"

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

#include "wqsg_iso_raw.h"
bool SISO_DirEnt::cheak()const
{
	if( L2H(lba_LE) == B2H(lba_BE) && L2H(lba_LE) >= 0 &&
		L2H(sp3_LE) == B2H(sp3_BE) && L2H(sp3_LE) >= 0 &&
		/*xx_cmpeq( &size_le , &size_be , sizeof(size_le) ) &&*/ L2H(size_LE) >= 0 &&
		(len&1) == 0 && len > 0 && len_ex == 0 &&
		nameLen > 0 && nameLen<=127 && L2H(size_LE) >= 0 )
	{
		if( attr & 2 )
		{
			if( (L2H(size_LE) % 2048) != 0 )
				return false;
		}
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------
static const u8 g_szIsoSync[] = {0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00};
//--------------------------------------------------------------------------------------------------
#if 0
class CIsoEcc
{
	u32 EDC_LUT[256];
	u8 ECC_F_LUT[256];
	u8 ECC_B_LUT[256];

#pragma warning( push )
#pragma warning( disable : 4996 )
	inline n32 xxx_MakeHeader( n32 a_Mode , s32 a_nLba )
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

	void Mode1( u8* a_pSector, s32 a_nLBA )
	{
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );

		//HEADER
		u32 temp = xxx_MakeHeader( 1 , a_nLBA );
		*(u32*)(a_pSector+0x0C) = temp;
		//EDC
		EDC_Compute( temp , a_pSector , 0x0810);
		//*(UINT*)(pSector+0x0810) = nOff;
		*(u32*)(a_pSector+0x0810) = temp;
		//ECC P code
		ECC_Compute( a_pSector + 0x0C, 86, 24,  2, 86, a_pSector + 0x081C);
		//ECC Q code
		ECC_Compute( a_pSector + 0x0C, 52, 43, 86, 88, a_pSector + 0x08C8);
	}

	void Mode2Form1( u8* a_pSector, s32 a_nLBA )
	{
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );

		u32 temp;
		//EDC
		EDC_Compute( temp , (u8*)a_pSector+0x10 , 0x0808 );
		//*(UINT*)(pSector+0x0818) = nOff;
		*(u32*)(a_pSector+0x0818) = temp;
		//Header=Zero
		*(u32*)(a_pSector+0x0C) = 0;
		//ECC P code
		ECC_Compute( a_pSector + 0x0C, 86, 24,  2, 86, a_pSector + 0x081C);
		//ECC Q code
		ECC_Compute( a_pSector + 0x0C, 52, 43, 86, 88, a_pSector + 0x08C8);
		//HEADER
		temp = xxx_MakeHeader( 2 , a_nLBA );
		*(u32*)(a_pSector+0x0C) = temp;
	}

};
#else
class CIsoEcc
{
	BYTE ecc_f_lut[256];
	BYTE ecc_b_lut[256];
	DWORD edc_lut[256];
public:
	CIsoEcc()
	{
		DWORD i, j, edc;
		for(i = 0; i < 256; i++)
		{
			j = (i << 1) ^ (i & 0x80 ? 0x11D : 0);
			ecc_f_lut[i] = j;
			ecc_b_lut[i ^ j] = i;
			edc = i;
			for(j = 0; j < 8; j++) edc = (edc >> 1) ^ (edc & 1 ? 0xD8018001 : 0);
			edc_lut[i] = edc;
		}
	}

	/***************************************************************************/
	// Compute EDC for a block
	void edc_computeblock( const BYTE *src, WORD size, u8 *dest )
	{
		DWORD edc=0x00000000;
		while(size--) edc = (edc >> 8) ^ edc_lut[(edc ^ (*src++)) & 0xFF];
		dest[0] = (edc >>  0) & 0xFF;
		dest[1] = (edc >>  8) & 0xFF;
		dest[2] = (edc >> 16) & 0xFF;
		dest[3] = (edc >> 24) & 0xFF;
	}

	void ecc_computeblock( DWORD major_count, DWORD major_mult , BYTE *dest )
	{
		DWORD major;
		for(major = 0; major < major_count; major++)
		{
			dest[major              ] = 0;
			dest[major + major_count] = 0;
		}
	}
	/***************************************************************************/
	// Compute ECC for a block (can do either P or Q)
	void ecc_computeblock( BYTE *src, DWORD major_count, DWORD minor_count, DWORD major_mult, DWORD minor_inc, BYTE *dest)
	{
		DWORD size = major_count * minor_count;
		DWORD major, minor;
		for(major = 0; major < major_count; major++)
		{
			DWORD index = (major >> 1) * major_mult + (major & 1);
			BYTE ecc_a = 0;
			BYTE ecc_b = 0;
			for(minor = 0; minor < minor_count; minor++)
			{
				BYTE temp = src[index];
				index += minor_inc;
				if(index >= size) index -= size;
				ecc_a ^= temp;
				ecc_b ^= temp;
				ecc_a = ecc_f_lut[ecc_a];
			}
			ecc_a = ecc_b_lut[ecc_f_lut[ecc_a] ^ ecc_b];
			dest[major              ] = ecc_a;
			dest[major + major_count] = ecc_a ^ ecc_b;
		}
	}

	// Generate ECC P and Q codes for a block
	void ecc_generate( BYTE *sector)
	{
		//           DWORD major_count,                    DWORD major_mult ,                 BYTE *dest
		//BYTE *src, DWORD major_count, DWORD minor_count, DWORD major_mult, DWORD minor_inc, BYTE *dest

		/* Compute ECC P code */
		ecc_computeblock( 86, 2, sector + 0x81C);
		/* Compute ECC Q code */
		ecc_computeblock( 52, 86, sector + 0x8C8);
	}

	// Generate ECC P and Q codes for a block
	void ecc_generate( BYTE *sector, int zeroaddress)
	{
		BYTE address[4], i;
		/* Save the address and zero it out */
		if(zeroaddress) for(i = 0; i < 4; i++)
		{
			address[i] = sector[12 + i];
			sector[12 + i] = 0;
		}
		/* Compute ECC P code */
		ecc_computeblock(sector + 0xC, 86, 24,  2, 86, sector + 0x81C);
		/* Compute ECC Q code */
		ecc_computeblock(sector + 0xC, 52, 43, 86, 88, sector + 0x8C8);
		/* Restore the address */
		if(zeroaddress) for(i = 0; i < 4; i++) sector[12 + i] = address[i];
	}

	void Mode1( u8* a_pSector, s32 a_nLBA )
	{
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );

		edc_computeblock(a_pSector + 0x00, 0x810, a_pSector + 0x810);
		/* Write out zero bytes */
		for(int i = 0; i < 8; i++)
			a_pSector[0x814 + i] = 0;
		ecc_generate(a_pSector, 0);
	}

	void Mode2Form1( u8* a_pSector, s32 a_nLBA )
	{
		memcpy( a_pSector , g_szIsoSync , sizeof(g_szIsoSync) );

		//disk1
// 		最初坏的类型 0x00200000 , lba = 12
// 		最初坏的类型 0x01640001 , lba = 129463
// 		最初坏的类型 0x41640001 , lba = 133469
		//disk 2
// 		最初坏的类型 0x00200000 , lba = 12
// 		最初坏的类型 0x01640001 , lba = 129463
// 		最初坏的类型 0x41640001 , lba = 133469
// 		最初坏的类型 0x01640101 , lba = 156629
// 		最初坏的类型 0x01E40101 , lba = 168301
// 		最初坏的类型 0x41E40001 , lba = 281847
		//disk 3
// 		最初坏的类型 0x00200000 , lba = 12
// 		最初坏的类型 0x01640001 , lba = 129463
// 		最初坏的类型 0x41640001 , lba = 133469
// 		最初坏的类型 0x01E40001 , lba = 277292
		switch ( *(u32*)(a_pSector + 0x10) )
		{
// 		case 0x00200000://12
// 			return;
// 			*(u32*)(a_pSector + 0x818) = 0;
// 			ecc_generate( a_pSector );
// 			break;
// 		case 0x01640001://129463
// 			break;
// 		case 0x01E40001://277292
// 			break;
// 		case 0x41640001://133469
// 			break;
		default:
			edc_computeblock(a_pSector + 0x10, 0x808, a_pSector + 0x818);
			ecc_generate(a_pSector, 1);
		}
	}
};
#endif
static CIsoEcc g_IsoEcc;
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
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
		DEF_ISO_SET_ERRMSG( m_StringMgr.GetString(50) );
		goto __gtOpenFileErr;
	}

	if( memcmp( tHead.m_Sync , g_szIsoSync , sizeof(g_szIsoSync) ) != 0 )
		m_nSectorSize = 2048;

	if( !ReadSectors( &tHead , 16 ) )
	{
		DEF_ISO_SET_ERRMSG( m_StringMgr.GetString(50) );
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
		g_IsoEcc.Mode1( (u8*)a_pSector, a_nLBA );
		break;
	case E_IT_Mode2Form1:
		g_IsoEcc.Mode2Form1( (u8*)a_pSector, a_nLBA );
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


bool CWQSG_ISO_Raw::CanCheckLba()const
{
	switch( m_eType )
	{
	case E_IT_Base:
		break;
	case E_IT_Mode1:
	case E_IT_Mode2Form1:
		return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}
#include <map>
std::map<u32,u32> g_has;
std::map<u32,u32> g_hasBad;
bool CWQSG_ISO_Raw::CheckLba( s32 a_nLBA )
{
	if( !CanCheckLba() )
		return FALSE;

	u8 szSectors1[2352];
	u8 szSectors2[2352];

	if( !ReadSectors( szSectors1 , a_nLBA ) )
		return FALSE;

	memcpy( szSectors2 , szSectors1 , sizeof(szSectors2) );

	switch( m_eType )
	{
	case E_IT_Mode1:
		g_IsoEcc.Mode1( (u8*)szSectors2, a_nLBA );
		break;
	case E_IT_Mode2Form1:
		g_IsoEcc.Mode2Form1( (u8*)szSectors2, a_nLBA );
		break;
	default:
		return FALSE;
	}

	size_t i;
	for( i = 0 ; i < sizeof(szSectors2) ; ++i )
	{
		if( szSectors1[i] != szSectors2[i] )
		{
			break;
		}
	}

	u32 id = *(u32*)(szSectors2+0x10);
	if( i == sizeof(szSectors2) )
	{
		if( g_has.find( id ) == g_has.end() )
		{
			g_has[id] = 0;
			if( g_hasBad.find( id ) != g_hasBad.end() )
				printf( "可以平反类型 0x%08X , lba = %d\n" , id , a_nLBA );
		}
		return true;
	}
	else
	{
		if( g_hasBad.find( id ) == g_hasBad.end() )
		{
			g_hasBad[id] = 0;

			printf( "最初坏的类型 0x%08X , lba = %d\n" , id , a_nLBA );
			if( g_has.find( id ) != g_has.end() )
				printf( "已存在类型 0x%08X , lba = %d\n" , id , a_nLBA );
		}

		return false;
	}
}

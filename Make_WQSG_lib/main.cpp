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
#ifdef __TEST_DEBUG__
#include<WQSG.h>
#include "WQSG_CxFile.h"

#include "ISO_App.h"

void XX()
{
}
#include<malloc.h>
#include <ISO/WQSG_UMD.h>
#include <ISO/WQSG_PsxIso.h>
int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	CISO_App iso;
	//BOOL D = iso.OpenISO( L"c:\\SLPS01210.img" , FALSE );
	BOOL D = iso.OpenISO( L"c:\\D2.img" , FALSE , E_WIT_PsxISO );

	SIsoFileData data;
#if 0
	iso.GetFileData( data , "/MIDI_A" , "" );
#endif
	{
		//MIDI_A
		SIsoFileFind* find = iso.FindIsoFile( "/" );

		while( iso.FindNextIsoFile( find , data ) )
		{
			printf( "%s , %d \n" , data.name , data.size );
		}
		iso.CloseFindIsoFile( find );
	}

	iso.导出文件( L"c:\\aaaa.txt" , "/" , "123.txt" );

	iso.CloseISO();

// 	CWQSG_PsxISO m_Iso;
// 	BOOL D = m_Iso.OpenISO( L"c:\\SLPS01210.img" , FALSE );

// 	SISO_DirEnt a_tDirEnt;
// 	m_Iso.GetRootDirEnt( a_tDirEnt );
// 	CWQSG_memFile mfp;
// 	m_Iso.ReadFile( a_tDirEnt , "SLPS_025.28;1" , mfp , 10 , 0 );
#else
	CWQSG_UMD m_Iso;
	BOOL D = m_Iso.OpenISO( L"S:\\Luxor.iso" , FALSE );
	SISO_DirEnt a_tDirEnt;
	m_Iso.GetRootDirEnt( a_tDirEnt );
	CWQSG_memFile mfp;
	m_Iso.ReadFile( a_tDirEnt , "UMD_DATA.BIN" , mfp , 10 , 0 );
#endif

	CWQSG_CmemFILE fp;

	char ddd[100];

	fp.fopen( ddd , sizeof(ddd) , true );
	return 0;
}


#endif
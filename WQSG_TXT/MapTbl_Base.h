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

#ifndef __MapTbl_Base_H__
#define __MapTbl_Base_H__

#define WQSG_TXT_LOGFILE L"log.log"

class CWQSG_MapTbl_Base
{
public:
	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL )
	{
		if( NULL == a_szText )
			return;

		CWQSG_File	fp;
		if( fp.OpenFile( WQSG_TXT_LOGFILE , 9 ) )
		{
			if( fp.GetFileSize() == 0 )
				fp.Write( "\xFF\xFE" , 2 );

			SYSTEMTIME time;
			GetLocalTime( &time );
			WCHAR szTime[128];
			::swprintf( szTime , L"\r\n%04d-%02d-%02d %02d:%02d:%02d" , time.wYear , time.wMonth , time.wDay , time.wHour , time.wMinute , time.wMinute );

			fp.WriteStrW( szTime );

			if( a_szTitle )
				fp.WriteStrW( a_szTitle );

			fp.WriteStrW( L"\r\n" );

			fp.WriteStrW( a_szText );
		}
	}
};
//----------------------------------------------------------------------------------------------------
#undef WQSG_TXT_LOGFILE

#endif

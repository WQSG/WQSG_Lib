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
#ifdef __TEST_NET_DEBUG__
#include "WQSG_net.h"

bool ff = true;
//#include <times.h>
void XX()
{
	CWQSG_Http dd( 0 );
	CWQSG_memFile fp;

	s64 sizel;
	dd.GetUrlFileSize( "http://www.nxtv.com.cn/cygd/music/dnd/10.mp3" , sizel , NULL );
	BOOL gg = dd.Download( "http://www.pm222.com/pokemondpdex/index.php?p=2" , fp , false );

	clock_t last_tick = clock();
	while( dd.GetState() == dd.en_STATE_RUN )
	{
		u32 ii = dd.GetSpeed();

		if( ii > (1024*1024) )
			printf( "%d MB/s\n" , ii / (1024*1024) );
		else if( ii > 1024 )
			printf( "%d KB/s\n" , ii / 1024 );
		else
			printf( "%d B/s\n" , ii );

		int ff = ( clock() - last_tick );
		Sleep( 1000 );
		printf( "%5d\n" , ff );
		last_tick += ff;
	}
	printf( "OK?" );
	while(1)
		Sleep(1000);
}
int _tmain(int argc, _TCHAR* argv[])
{
	u8 a = 255;
	u8 b = 1;
	u8 c = b - a;

	WSADATA wsadata;
	unsigned short winsock_version=0x0202;
	if( WSAStartup( winsock_version , &wsadata ) )
		return 1;

	XX();

	WSACleanup( );
	return 0;
}


#endif
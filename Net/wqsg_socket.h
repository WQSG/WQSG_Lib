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
#ifndef __WQSG_SCOKET_H__
#define __WQSG_SCOKET_H__
#pragma once
#include "wqsg_net_base.h"
#include <time.h>

#define LISTENQNUM      10
#if 0
class CWQSG_SockAddr
{
	sockaddr_in m_addr;
public:
	CWQSG_SockAddr();
	CWQSG_SockAddr( sockaddr_in addr );
	CWQSG_SockAddr( char* lpszIp , u_short port );
	//---------------------------------------------------------
	char*	GetIPStr() const;
	u_long	GetIP() const;
	u_short	GetPort()const;
	sockaddr_in	GetAddr() const;
	//--------------------------------------------
	bool	operator==( const CWQSG_SockAddr cmper ) const;
	bool	operator!=(const CWQSG_SockAddr cmper) const;
	void	operator=( const CWQSG_SockAddr cmper );
	void	operator=( const sockaddr_in addr );
};
#endif
//-------------------------------------------------------------------------
class CWQSG_socket_base
{
protected:
	///--------------------------------------------------------------------
	SOCKET  m_socket;
	///--------------------------------------------------------------------
	CWQSG_socket_base(void) : m_socket( INVALID_SOCKET ){	}
	virtual ~CWQSG_socket_base(void){	Close();	}
	bool	zzz_CreateSocket();
public:
	int		Close()
	{
		int rt = 0;
		if( m_socket != INVALID_SOCKET )
		{
			rt = ::closesocket( m_socket );
			if( rt == 0 )
				m_socket = INVALID_SOCKET;
		}
		return rt;
	}
	int		Send( char const*const pBuffer , const int iSendSize , const DWORD dwTimeout );
	int		Recv( char*const pBuffer , const int iBufferByteSize , const DWORD dwTimeout );
#ifdef DEBUG
	//-----------------------------------------------------------------
	operator SOCKET( )
	{
		return m_socket;
	}
#endif
};
class CWQSG_socket_client : public CWQSG_socket_base
{
public:
	//------------¿Í»§--------------------------------------------------------------------------------
	int Connect( const struct sockaddr& name );
//	int Connect( CWQSG_SockAddr& addr ){	return Connect( addr.GetIPStr() , addr.GetPort() );	}
	//If no error occurs, connect returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Connect( char const*const lpszIp , const u_short port )
	{
		struct sockaddr_in addr;
		memset( &addr , 0 , sizeof(addr) );

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ::inet_addr(lpszIp);
		addr.sin_port = ::htons(port);

		return Connect( *(sockaddr*)&addr );
	}
	//-----------------------------------
	bool	Attach( const SOCKET socket );
	SOCKET	Detach()
	{
		SOCKET rt = m_socket;

		m_socket = INVALID_SOCKET;

		return rt;
	}
#ifdef DEBUG
	operator SOCKET( )
	{
		return m_socket;
	}
#endif
};

//-------------------------------------------------------------------------
#if 0
class CWQSG_socket_server : public CWQSG_socket_base
{
public:
	CWQSG_socket_server(void){}
	virtual ~CWQSG_socket_server(void){}
	//------------------------------------------------------------------------------------------------
	int Bind( const sockaddr* name , const int namelen );
	//If no error occurs, bind returns zero. Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.
	int Bind( const u_short port, char const*const lpszIp = NULL);
	//If no error occurs, listen returns zero. Otherwise, a value of SOCKET_ERROR is returned
	int Listen( const int backlog = LISTENQNUM );
	SOCKET Accept( );
};
#endif

#undef LISTENQNUM
#endif
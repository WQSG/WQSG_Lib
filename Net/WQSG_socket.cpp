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
#include "WQSG_socket.h"
#include <crtdbg.h>
///---------------------------------------
#define	__DEF_CFG_USE_WSA_LIB			1
///---------------------------------------
#if !__DEF_CFG_USE_WSA_LIB
static inline bool xxx__setBlocking( SOCKET socket , const bool bBlocking )
{
	unsigned long rb = (bBlocking)?0:1;
	if( 0 == ::ioctlsocket( socket , FIONBIO , &rb ) )
	{
#if 0
		int bufsize = 8192;
		if( (0 != setsockopt( socket , SOL_SOCKET , SO_RCVBUF , (char*)&bufsize, sizeof(bufsize) ) ) ||
			(0 != setsockopt( socket , SOL_SOCKET , SO_SNDBUF , (char*)&bufsize, sizeof(bufsize) ) )
			)
			return false;
#endif
		return true;
	}
	return false;
}
#define DEF_waitW( _def_socket , _def_p_timeval , _def_rt ) {\
	fd_set wset;	FD_ZERO( &wset );	FD_SET( _def_socket , &wset ); \
	_def_rt = ( ::select( 0 , NULL , &wset , NULL , _def_p_timeval ) <= 0 )?false:true;}

#define DEF_waitR( _def_socket , _def_p_timeval , _def_rt ) {\
	fd_set rset;	FD_ZERO( &rset );	FD_SET( _def_socket , &rset ); \
	_def_rt = ( ::select( 0 , &rset , NULL , NULL , _def_p_timeval ) <= 0 )?false:true;}
#endif
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0
CWQSG_SockAddr::CWQSG_SockAddr()
{
}

CWQSG_SockAddr::CWQSG_SockAddr( sockaddr_in addr )
: m_addr(addr)
{
}
CWQSG_SockAddr::CWQSG_SockAddr( char* lpszIp , u_short port )
{
	memset( &m_addr , 0 , sizeof(m_addr) );

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = ::inet_addr( lpszIp );
	m_addr.sin_port = ::htons( port );
}

char* CWQSG_SockAddr::GetIPStr() const
{
	return ::inet_ntoa(m_addr.sin_addr);
}

unsigned long CWQSG_SockAddr::GetIP() const
{
	return ntohl( m_addr.sin_addr.s_addr );
}

unsigned short  CWQSG_SockAddr::GetPort()const
{
	return ::ntohs(m_addr.sin_port);
}

sockaddr_in  CWQSG_SockAddr::GetAddr() const
{
	return m_addr;
}

bool CWQSG_SockAddr::operator==( const CWQSG_SockAddr cmper ) const
{
	return ( GetIP()==cmper.GetIP() ) && ( GetPort()==cmper.GetPort() );
}

bool CWQSG_SockAddr::operator!=(const CWQSG_SockAddr cmper) const
{
	return ( GetIP()!=cmper.GetIP() ) || ( GetPort()!=cmper.GetPort() );
}

void CWQSG_SockAddr::operator=( const CWQSG_SockAddr cmper )
{
	m_addr = cmper.GetAddr();
}

void CWQSG_SockAddr::operator=( const sockaddr_in addr )
{
	m_addr = addr;
}
#endif
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if __DEF_CFG_USE_WSA_LIB
bool CWQSG_socket_base::zzz_CreateSocket()
{
	bool rt = false;
	if( Close() == 0 )
	{
		m_socket = ::socket( AF_INET , SOCK_STREAM , 0 );
		rt = ( m_socket != INVALID_SOCKET );
	}
	return rt;
}
int CWQSG_socket_base::Send( char const*const pBuffer , const int iSendSize , DWORD dwTimeout/* = 3 * 1000*/ )
{
	int	iSendBytes = 0;
	if( NULL == pBuffer )
		return iSendBytes;
#if 1
	const HANDLE hSendEvent = ::CreateEvent( NULL , FALSE , FALSE , NULL );
	if( NULL == hSendEvent )
		return iSendBytes;

	while( iSendBytes < iSendSize )
	{
		if( SOCKET_ERROR == ::WSAEventSelect( m_socket , hSendEvent , FD_WRITE | FD_CLOSE ) )
		{
			_ASSERTE( FALSE );
			break;
		}
		// 发送数据
		int iSend;
		if( (iSend = ::send( m_socket , pBuffer + iSendBytes , iSendSize - iSendBytes , 0 ) ) < 0 )
		{
			int iErrorCode = -iSend;
			if ( iErrorCode != WSAEWOULDBLOCK )	//太多的未完成重叠操作
				break;
			break;
		}
		iSendBytes += iSend;
		// 等等FD_WRITE | FD_CLOSE事件的发生
		const DWORD dwWaitResult = ::WSAWaitForMultipleEvents( 1 , &hSendEvent , TRUE , dwTimeout , TRUE );
		if( dwWaitResult != WSA_WAIT_EVENT_0 )
		{
			_ASSERTE( FALSE );
			break;
		}

		//////////////////////////////////////////////////////////////
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if( SOCKET_ERROR == ::WSAEnumNetworkEvents( m_socket , hSendEvent , &NetEvent ) )
		{
			_ASSERTE( FALSE );
			break;
		}
		// 清除事件 //////////////////////////////////////////////////////////////
		::WSAEventSelect( m_socket , hSendEvent , 0 );
		//判断发生了什么事件 FD_WRITE 或 FD_CLOSE
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( (( NetEvent.lNetworkEvents == FD_WRITE) && (NetEvent.iErrorCode[FD_WRITE_BIT] != 0) ) )
			)	// 发生错误
		{
			_ASSERTE( FALSE );
			break;
		}
	}
	::WSAEventSelect( m_socket , hSendEvent , 0 );
	::CloseHandle( hSendEvent );
#else
	while( iSendBytes < iSendSize )
	{
		int x = ::send( hSocket , pBuffer , iSendSize , 0 );
		if( x < 0 )
			break;
		iSendBytes += x;
	}
#endif
	return iSendBytes;
}
int CWQSG_socket_base::Recv( char*const pBuffer , const int iBufferByteSize , DWORD dwTimeout/* = 3 * 1000*/ )
{
	int	iRecvBytes = 0;

	if( NULL == pBuffer )
		return iRecvBytes;

	const HANDLE hReadEvent = ::CreateEvent( NULL , FALSE , FALSE , NULL );
	if( NULL == hReadEvent )
		return iRecvBytes;

	while( iRecvBytes < iBufferByteSize )
	{
		if( SOCKET_ERROR == ::WSAEventSelect( m_socket , hReadEvent , FD_READ | FD_CLOSE ) )
			break;

		// 等等FD_READ | FD_CLOSE事件的发生
		const DWORD dwWaitResult = ::WSAWaitForMultipleEvents( 1 , &hReadEvent , TRUE , dwTimeout , TRUE );
		if( dwWaitResult != WSA_WAIT_EVENT_0 )
			break;

		//////////////////////////////////////////////////////////////
		///	注意：即使 dwWaitResult == WSA_WAIT_EVENT0 ，也应该 
		///			进一步检查网络是否发生错误
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if( SOCKET_ERROR == ::WSAEnumNetworkEvents( m_socket , hReadEvent , &NetEvent ) )
			break;
		// 清除事件 //////////////////////////////////////////////////////////////
		::WSAEventSelect( m_socket , hReadEvent , 0 );
		//判断发生了什么事件 FD_READ 或 FD_CLOSE
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( (( NetEvent.lNetworkEvents	== FD_READ) && (NetEvent.iErrorCode[FD_READ_BIT] !=0) ) )
			)	// 发生错误
			break;
		// 接收数据
		int iRecvSize;
		if( (iRecvSize = ::recv( m_socket , pBuffer + iRecvBytes , iBufferByteSize - iRecvBytes , 0 ) ) < 0 )
		{
			int iErrorCode = -iRecvBytes;
			if ( iErrorCode != WSAEWOULDBLOCK )	//太多的未完成重叠操作
				break;
			break;
		}
		iRecvBytes += iRecvSize;
		if( iRecvBytes > 0 )
			break;
	}
	::WSAEventSelect( m_socket , hReadEvent , 0 );
	::CloseHandle( hReadEvent );

	return iRecvBytes;
}
#else
bool CWQSG_socket_base::zzz_CreateSocket()
{
	bool rt = false;
	if( Close() == 0 )
	{
		SOCKET tmp = ::socket( AF_INET , SOCK_STREAM , 0 );

		if ( tmp != INVALID_SOCKET )
		{
			if( xxx__setBlocking( tmp , true ) )
			{
				m_socket = tmp;
				rt = true;
			}
			else
				::closesocket( tmp );
		}
	}
	return rt;
}
int CWQSG_socket_base::Send( char const*const pBuffer , const int iSendSize , DWORD dwTimeout/* = 3 * 1000*/ )
{
	bool br;
	int send_count = SOCKET_ERROR;

	timeval val = {(dwTimeout/1000),(dwTimeout%1000*1000)};
	DEF_waitW( m_socket , (dwTimeout!=0)?&val:NULL , br );
	if( br )
	{
		send_count = ::send( m_socket , pBuffer , iSendSize , MSG_DONTROUTE );
		DEF_waitW( m_socket , (dwTimeout!=0)?&val:NULL , br );
	}
	return send_count;
}
int CWQSG_socket_base::Recv( char*const pBuffer , const int iBufferByteSize , DWORD dwTimeout/* = 3 * 1000*/ )
{
	bool br;
	int recv_count = SOCKET_ERROR;

	timeval val = {(dwTimeout/1000),(dwTimeout%1000*1000)};
	DEF_waitR( m_socket , (dwTimeout!=0)?&val:NULL , br );
	if( br )
		recv_count = ::recv( m_socket , pBuffer , iBufferByteSize , 0 );

	return recv_count;
}
#endif
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if __DEF_CFG_USE_WSA_LIB
bool CWQSG_socket_client::Attach( const SOCKET socket )
{
	bool rt = false;
	if( (socket != INVALID_SOCKET) && (Close() == 0) )
	{
		m_socket = socket;
		rt = true;
	}
	return rt;
}
//------------客户------------------------------------------------------------------------------------
int CWQSG_socket_client::Connect( const struct sockaddr& name )
{
	int rt = SOCKET_ERROR;

	if( zzz_CreateSocket() )
		rt= ::connect( m_socket , &name , sizeof(name) );

	return rt;
}
#else
bool CWQSG_socket_client::Attach( const SOCKET socket )
{
	bool rt = false;
	if( (socket != INVALID_SOCKET) && (Close() == 0) )
	{
		xxx__setBlocking( socket , false );

		m_socket = socket;
		rt = true;
	}
	return rt;
}
int CWQSG_socket_client::Connect( const struct sockaddr& name )
{
	return -1;
}
#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0
int CWQSG_socket_server::Bind( const sockaddr* name , const int namelen )
{
//		int flag = 1;
//		setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR , (char*)&flag, sizeof(flag)); 

	return ::bind( m_socket , name , namelen );
}
int CWQSG_socket_server::Bind( const u_short port , char const*const lpszIp /*= NULL*/ )
{
	struct sockaddr_in addr;
	memset( &addr , 0 , sizeof(addr) );

	addr.sin_family = AF_INET;

	if (lpszIp == NULL)
		addr.sin_addr.s_addr = ::htonl( INADDR_ANY );
	else
		addr.sin_addr.s_addr = ::inet_addr( lpszIp );

	addr.sin_port = ::htons(port);

	return Bind( (sockaddr*)&addr , sizeof(addr) );
}
int CWQSG_socket_server::Listen( const int backlog /*= LISTENQNUM*/ )
{
	return ::listen( m_socket , backlog );
}
SOCKET CWQSG_socket_server::Accept( )
{
	bool br;
	DEF_waitR( m_socket , NULL , br );
	if( br )
		return ::accept( m_socket , NULL , NULL );
	return SOCKET_ERROR;
}
#endif

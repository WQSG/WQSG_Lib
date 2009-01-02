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
#ifndef __WQSG_HTTP_H__
#define __WQSG_HTTP_H__
#pragma once
#include <WQSG.h>
#include "WQSG_socket.h"
//#include <Winsock2.h>
class CWQSG_Http
{
	clock_t m_last_tick;
	unsigned int m_totalByte;

	CRITICAL_SECTION m_cs_mutex;
	inline	void	zzz_lock(){	::EnterCriticalSection( &m_cs_mutex );	}
	inline	void	zzz_unlock(){	::LeaveCriticalSection( &m_cs_mutex );	}
	//-------------------------------------------------------------------
	int	zzz_recv( CWQSG_socket_client& cSocket , char*const pBuffer , const int iBufferByteSize )
	{
		int rt = cSocket.Recv( pBuffer , iBufferByteSize , m_dwTimeout );
		if( rt > 0 )
		{
			zzz_lock();
			m_totalByte += rt;
			zzz_unlock();
		}
		return rt;
	}
	void zzz_DownloadThread( char const*const url , CWQSG_xFile& fp , const s64 test_sizefile );
	static	DWORD WINAPI zzz_DownloadThread( LPVOID lpParam );
	//------------------------------------------------------------------------------------------------
public:
	enum	en_STATE
	{
		en_STATE_FREE	=	0,
		en_STATE_RUN		,
		en_STATE_PAUSE		,
		en_STATE_OK			,
	};
private:
	en_STATE		m_state;
	bool			m_bStopDownload;
	bool			m_bPauseDownload;
	DWORD			m_dwTimeout;
	int				m_最大重试;
	s64				m_fileOffset;
	s64				m_fileSize;
	//-------------------------------------------------------------
public:
	CWQSG_Http( int 最大重试 , s64 _fileOffset = 0 , s64 _fileSize = -1 )
		: m_state( en_STATE_FREE ) , m_bStopDownload(true) , m_bPauseDownload(false)
		, m_fileOffset(_fileOffset) , m_fileSize(_fileSize)
		, m_dwTimeout( 1000*5 ) , m_最大重试( 最大重试 )
		, m_last_tick( 0 ) , m_totalByte( 0 )
	{
		::InitializeCriticalSectionAndSpinCount( &m_cs_mutex , 1 );
		if( m_fileOffset < 0 )
			m_fileOffset = 0;
	}
	virtual	~CWQSG_Http()
	{
		StopDownload();
		::DeleteCriticalSection( &m_cs_mutex );
	}
	//-------------------------------------------------------------
	__i__	BOOL	Download( char const*const url , CWQSG_xFile& fp , const bool 异步 , const s64 test_sizefile = -1 );
	__i__	BOOL	Download( WCHAR const*const url , CWQSG_xFile& fp , const bool 异步 , const s64 test_sizefile = -1 );
	__i__	BOOL	GetUrlFileSize( char const*const url , s64& _fileSize , char const*const type );
	//------------------------------------------------------------
	__i__	void				StopDownload();
	__i__	bool				PauseDownload();
	__i__	en_STATE			GetState()const{	return m_state;	}
	__i__	double				Get百分比()const;
	__i__	s64					GetSize()const{	return m_fileSize;	}
//	__i__	bool				IsDownload_OK()	{	return (m_state == en_STATE_OK);	}
	__i__	bool				IsRun()	{	return ((m_state != en_STATE_FREE) && (m_state != en_STATE_OK));	}
//	__i__	bool				IsPause(){	return m_bPauseDownload;	}
	__i__	unsigned int		GetSpeed();
	__i__	void				SetTimeOut( const DWORD ms ){	m_dwTimeout = ms;	}
};
#endif
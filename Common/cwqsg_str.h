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
#pragma once
#ifndef __CWQSG_STR_H__
#define __CWQSG_STR_H__
#include "../Interface/wqsg_def.h"
#include "wqsg_string.h"
#include <stdlib.h>
//---------------------------------------------------------------------
__i__
class CWQSG_StrHelperA
{
public:
	static inline int FormatVLen( const char* pszFormat , va_list args )
	{
#if WIN32
		return _vscprintf( pszFormat , args );
#else
		return vfprintf( stderr , pszFormat , args );
#endif
	}
	static inline int FormatV( char* pszBuffer , size_t a_BufLen , const char* pszFormat , va_list args )
	{
		(void)a_BufLen;
WQSG_WIN_DISABLE_WARNING_BEGIN(4996)
		return vsprintf( pszBuffer , pszFormat , args );
WQSG_WIN_DISABLE_WARNING_END()
	}
};

class CWQSG_StrHelperW
{
public:
	static inline int FormatVLen( const WCHAR* pszFormat , va_list args )
	{
#if WIN32
		return _vscwprintf( pszFormat , args );
#else
		return vfwprintf( stderr , pszFormat , args );
#endif
	}
	static inline int FormatV( WCHAR* pszBuffer , size_t a_BufLen , const WCHAR* pszFormat , va_list args )
	{
WQSG_WIN_DISABLE_WARNING_BEGIN(4996)
		return vswprintf( pszBuffer , a_BufLen , pszFormat , args );
WQSG_WIN_DISABLE_WARNING_END()
	}
};
#if UNICODE
typedef CWQSG_StrHelperW CWQSG_StrHelperT;
#else
typedef CWQSG_StrHelperA CWQSG_StrHelperT;
#endif
__i__
template < typename TYPE_1 , int TSTRINGWORDLEN , class TCWQSG_StrHelper >
class CWQSG_strT
{
	__i__		TYPE_1*			m_pszStrBuffer;
	__i__		int				m_iStrLen;
	__i__		int				m_BufferLen;
	inline		TYPE_1*			GetNull()
	{
		static TYPE_1 s_null = 0;
		return &s_null;
	}
	//---------------------------------
	inline		TYPE_1*			GetWordBuf(int wordLen)
	{
		if(wordLen < 0)
		{
			SetNull();
			return NULL;
		}

		int page = wordLen / TSTRINGWORDLEN;
		if(wordLen % TSTRINGWORDLEN)
		{
			++page;
		}

		unsigned int maxlen = (unsigned int)page * TSTRINGWORDLEN;

		if( maxlen != (unsigned int)m_BufferLen )
		{
			if( m_pszStrBuffer == GetNull() )
				m_pszStrBuffer = NULL;
			m_pszStrBuffer = (TYPE_1*)::realloc( m_pszStrBuffer , ( maxlen + 1 ) * sizeof( TYPE_1 ) );
			if(NULL == m_pszStrBuffer){
				SetNull();
				return NULL;
			}
		}
		m_pszStrBuffer[ maxlen ] = 0;
//		m_iStrLen = wordLen;
		m_BufferLen = maxlen;

		return m_pszStrBuffer;
	}
	inline		void			SetNull ( )
	{
		if( m_pszStrBuffer != GetNull() )
		{
			::free( m_pszStrBuffer );
			m_pszStrBuffer = GetNull();
		}
		m_BufferLen = 0;
		m_iStrLen = 0;
	}
	inline		void			SetStr ( const TYPE_1* a_pStr , int len = -1 )
	{
		SetNull ( );
		const TYPE_1* str = a_pStr;

		m_iStrLen = WQSG_strlen( str );

		if( m_iStrLen > 0 )
		{
			if( m_iStrLen > m_BufferLen )
			{
				if(NULL == GetWordBuf( m_iStrLen ) )
				{
					return;
				}
			}

			if((unsigned int)len > (unsigned int)m_iStrLen)
			{
				len = m_iStrLen;
			}
			else
			{
				m_iStrLen = len;
			}

			TYPE_1* dp = m_pszStrBuffer;
			while( ( *dp = *( str++ ) ) && len-- )
			{
				++dp;
			}
			*dp = 0;
		}
	}
	inline		void			AddStr( TYPE_1 tCh )
	{
		if( tCh == 0 )
			return;

		int t_len2 = 1 + m_iStrLen;

		if( t_len2 > 0 )
		{
			if( t_len2 > m_BufferLen )
			{
				if(NULL == GetWordBuf( t_len2 ) )
				{
					return;
				}
			}
			m_pszStrBuffer[m_iStrLen] = tCh;
			m_pszStrBuffer[m_iStrLen+1] = 0;
			m_iStrLen = t_len2;
		}
	}
	inline		void			AddStr( const TYPE_1* str )
	{
		int t_len = WQSG_strlen(str);
		int t_len2 = t_len + m_iStrLen;

		if( (t_len > 0) && (t_len2 > 0) )
		{
			if( t_len2 > m_BufferLen )
			{
				if(NULL == GetWordBuf( t_len2) )
				{
					return;
				}
			}
			WQSG_strcpy( str , m_pszStrBuffer + m_iStrLen );
			m_iStrLen = t_len2;
		}
	}
	///-------------------------------------------------------------------
	inline		int				FindStr( const TYPE_1* str , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_iStrLen )
		{
			iStart = m_iStrLen;
		}
		return (int)( iStart + WQSG_strstr ( m_pszStrBuffer + iStart , str ) );
	}
	inline		int				FindChr( TYPE_1 ch , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_iStrLen )
		{
			iStart = m_iStrLen;
		}
		return (int)( iStart + WQSG_strchr ( m_pszStrBuffer + iStart , ch ) );
	}
	inline		int				ReFindChr( TYPE_1 ch , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_iStrLen )
		{
			iStart = m_iStrLen;
		}
		TYPE_1* str = m_pszStrBuffer + iStart;
		while( str >=  m_pszStrBuffer )
		{
			if( *str == ch )
			{
				return (int)( str - m_pszStrBuffer );
			}
			--str;
		}
		return -1;
	}
	inline		int				ReFindStr( const TYPE_1* str , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_iStrLen )
		{
			iStart = m_iStrLen;
		}
		TYPE_1* strP = m_pszStrBuffer + iStart;
		iStart = WQSG_strlen( str );
		strP -= iStart;
		while( strP >=  m_pszStrBuffer )
		{
			if( -1 != ( iStart = WQSG_strstr( strP , str ) ) )
				return iStart;
			--strP;
		}
		return -1;
	}
	///-------------------------------------------------------------------
	inline		int				Cmp(const TYPE_1* str)const
	{
		return WQSG_strcmp( str , m_pszStrBuffer );
	}
	///-------------------------------------------------------------------
public:
	inline						CWQSG_strT()
		: m_pszStrBuffer(GetNull())
		, m_iStrLen(0)
		, m_BufferLen(0)
	{
	}
	inline						CWQSG_strT( TYPE_1 tCh )
		: m_pszStrBuffer(GetNull())
		, m_iStrLen(0)
		, m_BufferLen(0)
	{
		TYPE_1 szStr[2] = { tCh , 0 };
		SetStr( szStr );
	}
	inline						CWQSG_strT( const TYPE_1* str )
		: m_pszStrBuffer(GetNull())
		, m_iStrLen(0)
		, m_BufferLen(0)
	{
		SetStr( str );
	}
	inline						CWQSG_strT( const TYPE_1* str , int nCount )
		: m_pszStrBuffer(GetNull())
		, m_iStrLen(0)
		, m_BufferLen(0)
	{
		SetStr( str , nCount );
	}
	inline						CWQSG_strT( const CWQSG_strT& str )
		: m_pszStrBuffer(GetNull())
		, m_iStrLen(0)
		, m_BufferLen(0)
	{
		SetStr( str );
	}
	///-------------------------------------------------------------------
	virtual			~CWQSG_strT()
	{
		 SetNull();
	}
	///-------------------------------------------------------------------
	inline		operator		const TYPE_1* ()const
	{
		return m_pszStrBuffer;
	}
	///-------------------------------------------------------------------
	inline	__i__	u32				strlen()const
	{
		return m_iStrLen;
	}
	///-------------------------------------------------------------------
	inline	__i__	CWQSG_strT& operator = ( TYPE_1 tCh )
	{
		TYPE_1	szStr[2] = { tCh , 0 };
		SetStr( szStr );
		return *this;
	}
	inline	__i__	CWQSG_strT& operator = ( const TYPE_1* str )
	{
		SetStr( str );
		return *this;
	}
	inline	__i__	CWQSG_strT& operator = ( const CWQSG_strT& str )
	{
		SetStr( str );
		return *this;
	}
	///-------------------------------------------------------------------
	inline	__i__	CWQSG_strT& operator += ( TYPE_1 str )
	{
		AddStr(str);
		return *this;
	}
	inline	__i__	CWQSG_strT& operator += ( const TYPE_1* str )
	{
		AddStr(str);
		return *this;
	}
	inline	__i__	CWQSG_strT& operator += ( const CWQSG_strT& str )
	{
		AddStr(str);
		return *this;
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator == ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return str1.strlen() == str2.strlen() && ( str1.Cmp(str2) == 0 );
	}
	inline	friend	bool operator == ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) == 0);
	}
	inline	friend	bool operator == ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return ( str1.Cmp(str2) == 0 );
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator != ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return str1.strlen() != str2.strlen() || ( str1.Cmp(str2) != 0 );
	}
	inline	friend	bool operator != ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) != 0);
	}
	inline	friend	bool operator != ( const CWQSG_strT& str1  , const TYPE_1* str2 )
	{
		return (str1.Cmp(str2) != 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator > ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.Cmp(str2) < 0);
	}
	inline	friend	bool operator > ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) > 0);
	}
	inline	friend	bool operator > ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.Cmp(str2) < 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator < ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.Cmp(str2) > 0);
	}
	inline	friend	bool operator < ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) < 0);
	}
	inline	friend	bool operator < ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.Cmp(str2) > 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator >= ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.Cmp(str2) <= 0);
	}
	inline	friend	bool operator >= ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) >= 0);
	}
	inline	friend	bool operator >= ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.Cmp(str2) <= 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator <= ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.Cmp(str2) >= 0);
	}
	inline	friend	bool operator <= ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.Cmp(str1) <= 0);
	}
	inline	friend	bool operator <= ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.Cmp(str2) >= 0);
	}
	//-------------------------------------------------------------------
	inline	friend	CWQSG_strT operator + ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		CWQSG_strT str_out(str1);
		return (str_out += str2);
	}
	inline	friend	CWQSG_strT operator + ( TYPE_1 tCh , const CWQSG_strT& str2 )
	{
		CWQSG_strT str_out(tCh);
		return (str_out += str2);
	}
	inline	friend	CWQSG_strT operator + ( const CWQSG_strT& str1 , TYPE_1 tCh )
	{
		CWQSG_strT str_out(str1);
		return (str_out += tCh);
	}
	inline	friend	CWQSG_strT operator + ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		CWQSG_strT str_out(str1);
		return (str_out += str2);
	}
	inline	friend	CWQSG_strT operator + ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		CWQSG_strT str_out(str1);
		return (str_out += str2);
	}
	inline	__i__	int			Find( TYPE_1 ch , int iStart = 0 )const
	{
		return					FindChr( ch , iStart );
	}
	inline	__i__	int			Find( const TYPE_1* str , int iStart = 0 )const
	{
		return					FindStr( str , iStart );
	}
	inline	__i__	int			Find( const CWQSG_strT& str , int iStart = 0 )const
	{
		return FindStr( str , iStart );
	}
	inline	__i__	int			ReFind( TYPE_1 ch , int iStart = -1 )const
	{
		return ReFindChr( ch , iStart );
	}
	inline	__i__	int			ReFind( const TYPE_1* str , int iStart = -1 )const
	{
		return ReFindStr( str , iStart );
	}
	inline	__i__	int			ReFind( const CWQSG_strT& str , int iStart = -1 )const
	{
		return ReFindStr( str , iStart );
	}
	///-------------------------------------------------------------------
	inline	__i__	CWQSG_strT	Right( int nCount ) const
	{
		if( nCount <= 0 )
			return ( CWQSG_strT() );

		int iFirst = m_iStrLen - nCount;
		if(iFirst < 0)
		{
			iFirst = 0;
		}
		return ( CWQSG_strT( m_pszStrBuffer + iFirst , nCount ) );
	}
	inline	__i__	CWQSG_strT	Left( int nCount ) const
	{
		if( nCount <= 0 )
			return ( CWQSG_strT() );

		return ( CWQSG_strT( m_pszStrBuffer , nCount ) );
	}
	inline	__i__	CWQSG_strT	Mid	( int iFirst, int nCount ) const
	{
		if( nCount <= 0 )
			return ( CWQSG_strT() );

		if( iFirst < 0 )
		{
			iFirst = 0;
		}else if( iFirst > m_iStrLen )
		{
			iFirst = m_iStrLen;
		}
		return ( CWQSG_strT( m_pszStrBuffer + iFirst , nCount) );
	}
	///-------------------------------------------------------------------
	inline	__i__	void FormatV( const TYPE_1* pszFormat , va_list args )
	{
		int nLength = TCWQSG_StrHelper::FormatVLen( pszFormat , args );
		TYPE_1* pszBuffer = GetWordBuf( nLength );
		TCWQSG_StrHelper::FormatV( pszBuffer , nLength , pszFormat , args );
		m_iStrLen = nLength;
	}
	inline	__i__	void Format( const TYPE_1* pszFormat , ... )
	{
		va_list argList;
		va_start( argList, pszFormat );
		FormatV( pszFormat, argList );
		va_end( argList );
	}
	inline	__i__	void AppendFormatV( const TYPE_1* pszFormat, va_list args )
	{
		int nCurrentLength = strlen();
		int nAppendLength = TCWQSG_StrHelper::FormatVLen( pszFormat , args );
		TYPE_1* pszBuffer = GetWordBuf( nCurrentLength+nAppendLength );
		TCWQSG_StrHelper::FormatV( pszBuffer + nCurrentLength , nCurrentLength+nAppendLength , pszFormat , args );
		m_iStrLen = nCurrentLength+nAppendLength;
	}
	inline	__i__	void AppendFormat( const TYPE_1* pszFormat , ... )
	{
		va_list argList;
		va_start( argList, pszFormat );
		AppendFormatV( pszFormat, argList );
		va_end( argList );
	}
	///-------------------------------------------------------------------
	inline	__i__	const TYPE_1* GetString()const
	{
		return m_pszStrBuffer;
	}
};
__i__
typedef CWQSG_strT< char    , 128 , CWQSG_StrHelperA >		CWQSG_strA;
typedef CWQSG_strT< WCHAR , 128 , CWQSG_StrHelperW >		CWQSG_strW;
typedef CWQSG_strT< TCHAR   , 128 , CWQSG_StrHelperT >		CWQSG_str;
__i__
#endif //__CWQSG_STR_H__

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
#include "../WQSG_def.h"
#include "../WQSG_string.h"
#include <stdlib.h>
//---------------------------------------------------------------------
__i__
#define		WDEF_STRINGWORDLEN	256
template <typename TYPE_1>
class CWQSG_strT
{
	__i__		TYPE_1			m_null;
	__i__		TYPE_1*			m_str;
	__i__		int				m_len;
	__i__		int				m_maxlen;
	//---------------------------------
	inline		TYPE_1*			GetWordBuf(int wordLen)
	{
		if(wordLen < 0)
		{
			SetNull();
			return NULL;
		}

		int page = wordLen / WDEF_STRINGWORDLEN;
		if(wordLen % WDEF_STRINGWORDLEN)
		{
			++page;
		}

		unsigned int maxlen = (unsigned int)page * WDEF_STRINGWORDLEN;

		if( maxlen != (unsigned int)m_maxlen )
		{
			if( m_str == ( &m_null ) )
				m_str = NULL;
			m_str = (TYPE_1*)::realloc( m_str , ( maxlen + 1 ) * sizeof( TYPE_1 ) );
			if(NULL == m_str){
				SetNull();
				return NULL;
			}
		}
		m_str[ maxlen ] = L'\0';
//		m_len = wordLen;
		m_maxlen = maxlen;

		return m_str;
	}
#undef		WDEF_STRINGWORDLEN
	inline		void			SetNull ( )
	{
		if( m_str != ( &m_null ) )
		{
			::free( m_str );
			m_str = &m_null;
		}
		m_maxlen = 0;
		m_len = 0;
	}
	inline		void			SetStr ( const TYPE_1* str , int len = -1 )
	{
		SetNull ( );
		m_len = WQSG_strlen( str );

		if( m_len > 0 )
		{
			if( m_len > m_maxlen )
			{
				if(NULL == GetWordBuf( m_len ) )
				{
					return;
				}
			}

			if((unsigned int)len > (unsigned int)m_len)
			{
				len = m_len;
			}
			else
			{
				m_len = len;
			}

			TYPE_1* dp = m_str;
			while( ( *dp = *( str++ ) ) && len-- )
			{
				++dp;
			}
			*dp = L'\0';
		}
	}
	inline		void			AddStr( TYPE_1 tCh )
	{
		if( tCh == _T('\0') )
			return;

		int t_len2 = 1 + m_len;

		if( t_len2 > 0 )
		{
			if( t_len2 > m_maxlen )
			{
				if(NULL == GetWordBuf( t_len2 ) )
				{
					return;
				}
			}
			m_str[m_len] = tCh;
			m_str[m_len+1] = _T('\0');
			m_len = t_len2;
		}
	}
	inline		void			AddStr( const TYPE_1* str )
	{
		int t_len = WQSG_strlen(str);
		int t_len2 = t_len + m_len;

		if( (t_len > 0) && (t_len2 > 0) )
		{
			if( t_len2 > m_maxlen )
			{
				if(NULL == GetWordBuf( t_len2) )
				{
					return;
				}
			}
			WQSG_strcpy( str , m_str + m_len );
			m_len = t_len2;
		}
	}
	///-------------------------------------------------------------------
	inline		int				FindStr( TYPE_1* str , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_len )
		{
			iStart = m_len;
		}
		return (int)( iStart + WQSG_strstr ( m_str + iStart , str ) );
	}
	inline		int				FindChr( TYPE_1 ch , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_len )
		{
			iStart = m_len;
		}
		return (int)( iStart + WQSG_strchr ( m_str + iStart , ch ) );
	}
	inline		int				ReFindChr( TYPE_1 ch , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_len )
		{
			iStart = m_len;
		}
		TYPE_1* str = m_str + iStart;
		while( str >=  m_str )
		{
			if( *str == ch )
			{
				return (int)( str - m_str );
			}
			--str;
		}
		return -1;
	}
	inline		int				ReFindStr( TYPE_1* str , int iStart )const
	{
		if( (unsigned int)iStart > (unsigned int)m_len )
		{
			iStart = m_len;
		}
		TYPE_1* strP = m_str + iStart;
		iStart = WQSG_strlen( str );
		strP -= iStart;
		while( strP >=  m_str )
		{
			if( -1 != ( iStart = WQSG_strstr( strP , str ) ) )
				return iStart;
			--strP;
		}
		return -1;
	}
	///-------------------------------------------------------------------
	inline		int				比较(TYPE_1* str)const
	{
		return WQSG_strcmp( str , m_str );
	}
	///-------------------------------------------------------------------
public:
	inline						CWQSG_strT()
		: m_str(&m_null)
		, m_null(_T('\0'))
		, m_len(0)
		, m_maxlen(0)
	{
	}
	inline						CWQSG_strT( TYPE_1 tCh )
		: m_str(&m_null)
		, m_null(_T('\0'))
		, m_len(0)
		, m_maxlen(0)
	{
		TYPE_1 szStr[2] = { tCh , 0 };
		SetStr( szStr );
	}
	inline						CWQSG_strT( const TYPE_1* str )
		: m_str(&m_null)
		, m_null(_T('\0'))
		, m_len(0)
		, m_maxlen(0)
	{
		SetStr( str );
	}
	inline						CWQSG_strT( const TYPE_1* str , int nCount )
		: m_str(&m_null)
		, m_null(_T('\0'))
		, m_len(0)
		, m_maxlen(0)
	{
		SetStr( str , nCount );
	}
	inline						CWQSG_strT( const CWQSG_strT& str )
		: m_str(&m_null)
		, m_null(_T('\0'))
		, m_len(0)
		, m_maxlen(0)
	{
		SetStr( str );
	}
	///-------------------------------------------------------------------
	virtual			~CWQSG_strT();
	///-------------------------------------------------------------------
	inline		operator		TYPE_1* ()const
	{
		return m_str;
	}
	///-------------------------------------------------------------------
	inline	__i__	u32				strlen()const
	{
		return m_len;
	}
	///-------------------------------------------------------------------
	inline	__i__	CWQSG_strT& operator = ( TYPE_1 tCh )
	{
		TYPE_1	szStr[2] = { tCh , _T('\0') };
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
		return ( str1.比较(str2) == 0 );
	}
	inline	friend	bool operator == ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) == 0);
	}
	inline	friend	bool operator == ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return ( str1.比较(str2) == 0 );
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator != ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return ( str1.比较(str2) != 0 );
	}
	inline	friend	bool operator != ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) != 0);
	}
	inline	friend	bool operator != ( const CWQSG_strT& str1  , const TYPE_1* str2 )
	{
		return (str1.比较(str2) != 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator > ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.比较(str2) < 0);
	}
	inline	friend	bool operator > ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) > 0);
	}
	inline	friend	bool operator > ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.比较(str2) < 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator < ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.比较(str2) > 0);
	}
	inline	friend	bool operator < ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) < 0);
	}
	inline	friend	bool operator < ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.比较(str2) > 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator >= ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.比较(str2) <= 0);
	}
	inline	friend	bool operator >= ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) >= 0);
	}
	inline	friend	bool operator >= ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.比较(str2) <= 0);
	}
	///-------------------------------------------------------------------
	inline	friend	bool operator <= ( const CWQSG_strT& str1 , const CWQSG_strT& str2 )
	{
		return (str1.比较(str2) >= 0);
	}
	inline	friend	bool operator <= ( const TYPE_1* str1 , const CWQSG_strT& str2 )
	{
		return (str2.比较(str1) <= 0);
	}
	inline	friend	bool operator <= ( const CWQSG_strT& str1 , const TYPE_1* str2 )
	{
		return (str1.比较(str2) >= 0);
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

		int iFirst = m_len - nCount;
		if(iFirst < 0)
		{
			iFirst = 0;
		}
		return ( CWQSG_strT( m_str + iFirst , nCount ) );
	}
	inline	__i__	CWQSG_strT	Left( int nCount ) const
	{
		if( nCount <= 0 )
			return ( CWQSG_strT() );

		return ( CWQSG_strT( m_str , nCount ) );
	}
	inline	__i__	CWQSG_strT	Mid	( int iFirst, int nCount ) const
	{
		if( nCount <= 0 )
			return ( CWQSG_strT() );

		if( iFirst < 0 )
		{
			iFirst = 0;
		}else if( iFirst > m_len )
		{
			iFirst = m_len;
		}
		return ( CWQSG_strT( m_str + iFirst , nCount) );
	}
/*	static int Format( LPWSTR pszBuffer, __format_string LPCWSTR pszFormat, va_list args )
	{
		#pragma warning (push)
		#pragma warning(disable : 4996)
		return vswprintf( pszBuffer, pszFormat, args );
		#pragma warning (pop)
	}*/
};
__i__
__i__
typedef CWQSG_strT<char>		CWQSG_strA;
typedef CWQSG_strT<wchar_t>		CWQSG_strW;
typedef CWQSG_strT<TCHAR>		CWQSG_str;
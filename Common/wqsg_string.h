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
#ifndef __WQSG_STRING_H__
#define __WQSG_STRING_H__

#include "../Interface/wqsg_def.h"
#ifdef	WIN32
#else
#include <pspkernel.h>
#include <string.h>
typedef	u16	WCHAR;
#endif
__i__
inline int	WQSG_strlen( char const*const TXT )
{
	u8 const*eof = (u8 const*)TXT;
	while(*(eof++))
		;
	return (int)( eof - (u8*)TXT - 1 );
}
//-------------------------------------------------------------------------------
inline int	WQSG_strchr( char const*const TXT , const char FIND )
{
	u8 const* cp = (u8 const*)TXT;

//	if ( !FIND )		return 0;

	while (*cp && (u8)FIND != *cp)
		++cp;

	if( *cp == (u8)FIND )
		return (int)(cp - (u8*)TXT);
	return (int)-1;
}
//-------------------------------------------------------------------------------
inline int	WQSG_strstr( char const*const TXT , char const*const FIND )
{
	u8 const*cp = (u8*)TXT;
	u8 const*s1;
	u8 const*s2;

	if ( !(*FIND) )
		return 0;

	while ( *cp )
	{
		s1 = cp;
		s2 = (u8 const*)FIND;
		while ( *s1	&& *s2	&& ( *s1 == *s2 )	)
			s1++, s2++;

		if (!*s2)
			return (int)( cp - (u8*)TXT );

		cp++;
	}
	return (int)-1;
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcpy	( char const*const _FORM , char*const TO )
{
	u8* to = (u8*)TO;
	u8 const* FORM = (u8 const*)_FORM;

	while( (*(to++) = *(u8*)(FORM++)) )
		;
	return (int)(to - (u8*)TO - 1 );
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcpy_Ex( char const*const _FORM , char *const TO , const int _len )
{
	u8 *s1 = (u8*)TO;
	u8 const* FORM = (u8 const*)_FORM;

	int len = (int)_len;

	while( len-- && *FORM )
		*(s1++) = *(u8*)(FORM++);

	*s1 = L'\0';

	return (int)( s1 - (u8*)TO);
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcmp	( char const*const str1 , char const*const str2 )
{
	return strcmp( str1 , str2);
}
//-------------------------------------------------------------------------------
#ifdef	WIN32
inline char*	WQSG_getstrL	( char const*const _TXT , const int _len )
{
	u8* out_0 = NULL;
	int iLen = WQSG_strlen(_TXT);
	if( iLen >= 0 )
	{
		if( ( _len >=0 ) && ( iLen > _len) )
			iLen = _len;

		if( ( (iLen+1) > 0 ) &&
			( out_0 = new u8[iLen + 1] )
			)
		{
			WQSG_strcpy_Ex( (char*)_TXT , (char*)out_0 , iLen );
		}
	}
	return (char*)out_0;
}
//-------------------------------------------------------------------------------
inline char*	WQSG_DelSP_L	( char const*const _TXT )
{
	u8* OUT_0 = NULL;
	int iLen = WQSG_strlen(_TXT);
	if( (iLen >= 0) &&
		((iLen+1) > 0 ) &&
		( OUT_0 = new u8[iLen + 1] )
		)
	{
		u8 const* TXT = (u8 const*) _TXT;
		u8* s1 = OUT_0;

		if( !OUT_0 ) return NULL;

		while( *TXT )
		{
			if( *TXT > 0x7F )
			{
				if( 0xA1 == *TXT )
				{
					if( 0xA1 == *(++TXT) )
						continue;
					else
					{
						*(s1++) = 0xA1;
						*(s1++) = *(TXT++);
					}
				}
				else
				{
					*(s1++) = *(TXT++);
					*(s1++) = *(TXT++);
				}
				break;
			}
			else
			{
				if( ' ' != *TXT )
				{
					*(s1++) = *(TXT++);
					break;
				}
			}
		}
		*s1 = '\0';
	}
	return (char*)OUT_0;
}
//-------------------------------------------------------------------------------
inline char*	WQSG_DelSP_ALL	( char const*const _TXT )
{
	u8* OUT_0 = NULL;
	int iLen = WQSG_strlen(_TXT);
	if( (iLen >= 0) &&
		((iLen+1) > 0 ) &&
		( OUT_0 = new u8[iLen + 1] )
		)
	{
		u8 const* TXT = (u8 const*)_TXT;
		u8* s1 = OUT_0;

		while( *TXT )
		{		
			if( L' ' != *TXT )
			{
				if( *TXT > 0x7F )
				{
					if( 0xA1 != *TXT )
					{
						*(s1++) = *(TXT++);
						*(s1++) = *(TXT++);
					}
					else
					{
						if( 0xA1 == *(++TXT) )
							++TXT;
						else
						{
							*(s1++) = 0xA1;
							*(s1++) = *(TXT++);
						}
					}
				}
				else
					*(s1++) = *(TXT++);
			}
			else
				++TXT;
		}
		*s1 = '\0';
	}
	return (char*)OUT_0;
}
//-------------------------------------------------------------------------------
inline BOOL	WQSG_IsHexText	( char const*const _TXT )
{
	u8 const* TXT = (u8 const*)_TXT;

	if(!(*TXT))
		return FALSE;

	while(*TXT)
	{
		u8 s1 = *TXT;
		if(((s1 >= '0') && (s1 <= '9'))
			|| ((s1 >= 'a') && (s1 <= 'f'))
			||((s1 >= 'A') && (s1 <= 'F'))
			)
			++TXT;
		else
			return FALSE;
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
inline BOOL	WQSG_IsDecText	( char const*const _TXT )
{
	u8 const* TXT = (u8 const*)_TXT;

	if( !(*TXT) )
		return FALSE;

	while(*TXT)
	{
		u8 s1 = *TXT;
		if( (s1 >= '0') && (s1 <= '9') )
			TXT++;
		else
			return FALSE;
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
inline char*	WQSG_abc_ABC	( char const*const _xiao )
{
	u8* DA_0 = NULL;
	int iLen = ::WQSG_strlen( _xiao );
	if( (iLen >= 0) &&
		((iLen+1) > 0) &&
		( DA_0 = new u8[iLen + 1] )
		)
	{
		u8*da = DA_0;
		u8 const* xiao = (u8 const*)_xiao;

		while((*da) = (*(u8*)(xiao++)))
		{
			if( (*da >= 'a') && (*da <= 'z') )
				(*da) -= 0x20;
			++da;
		}
	}
	return (char*)DA_0;
}
//-------------------------------------------------------------------------------
inline char*	WQSG_ABC_abc	( char const*const _da )
{
	u8* XIAO_0 = NULL;
	int iLen = ::WQSG_strlen( _da );
	if( (iLen >= 0) &&
		((iLen+1) > 0) &&
		( XIAO_0 = new u8[iLen + 1] )
		)
	{
		u8* xiao = XIAO_0;
		u8 const* da = (u8 const*)_da;

		while( (*xiao) = (*(u8*)(da++)) )
		{
			if(*xiao >= 'A' && *xiao <= 'Z')
				(*xiao) += 0x20;
			++xiao;
		}
	}

	return (char*)XIAO_0;
}
//-------------------------------------------------------------------------------
inline WCHAR*	WQSG_char_W	( char const*const TXT , const unsigned int codePage = CP_ACP )
{
	WCHAR* out = NULL;
	int	iLen = ::MultiByteToWideChar( codePage , 0 , TXT , -1 , NULL , 0 );
	if( (iLen >= 0) &&
		((iLen+1) > 0) &&
		( out = new WCHAR[iLen + 1] )
		)
	{
		::MultiByteToWideChar ( codePage , 0 , TXT , -1 , (LPWSTR)out , iLen );
	}
	return out;
}
#define				WQSG_UTF8_W( utf8TXT )  WQSG_char_W( utf8TXT , CP_UTF8 )
//-------------------------------------------------------------------------------
inline char*	WQSG_W_char	( WCHAR const*const TXT , const unsigned int codePage = 936 )
{
	char* out = NULL;
	int	iLen = ::WideCharToMultiByte( codePage , 0 , (LPWSTR)TXT , -1 , NULL , 0 , NULL , NULL );

	if( (iLen >= 0) &&
		((iLen+1) > 0) &&
		( out = new char[iLen + 1] )
		)
	{
		::WideCharToMultiByte ( codePage , 0 , (LPWSTR)TXT , -1 , out , iLen , NULL , NULL );
	}
	return out;
}
#define				WQSG_W_UTF8( W_TXT )  WQSG_W_char( W_TXT , CP_UTF8 )
#endif
inline int	WQSG_strlen	( WCHAR const*const TXT )
{
	WCHAR const* eof = (WCHAR const*)TXT;
	while(*(eof++))
		;
	return ((int)(eof - TXT)) - 1;
}
//-------------------------------------------------------------------------------
inline int	WQSG_strchr	( WCHAR const*const TXT , const WCHAR FIND )
{
	WCHAR const* cp = (WCHAR const*)TXT;

//	if ( !FIND )return 0;

	while (*cp && FIND != *cp)
		++cp;

	if( *cp == FIND )
	{
		return (int)(cp - TXT);
	}
	return (int)-1;
}
//-------------------------------------------------------------------------------
inline int	WQSG_strstr	( WCHAR const*const TXT , WCHAR const*const FIND )
{
	WCHAR const*cp = TXT;

	if ( !(*FIND) )
		return (int)0;

	while (*cp)
	{
		WCHAR const*s1 = cp;
		WCHAR const*s2 = (WCHAR const*)FIND;
		while ( *s1 && *s2 && !(*s1-*s2) )
			++s1, ++s2;

		if (!*s2)
			return (int)(cp-TXT);
		++cp;
	}
	return (int)-1;
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcpy	( WCHAR const*const _FORM , WCHAR*const TO )
{
	WCHAR* to = TO;
	WCHAR const* FORM = (WCHAR const*)_FORM;

	while( (*(to++) = *(FORM++)) )
		;

	return (int)(to - TO - 1);
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcpy_Ex( WCHAR const*const _FORM , WCHAR*const TO , const int _len )
{
	WCHAR* s1 = TO;
	WCHAR const* FORM = (WCHAR const*)_FORM;

	int len = (int)_len;
	while( len-- && *FORM )
		*(s1++) = *(FORM++);

	*s1 = L'\0';

	return (int)(s1-TO);
}
//-------------------------------------------------------------------------------
inline int	WQSG_strcmp	( WCHAR const*const str1 , WCHAR const*const str2 )
{
	int ret = 0 ;
	const WCHAR* src = (const WCHAR*)str1;
	const WCHAR* dst = (const WCHAR*)str2;

	while( ! (ret = (int)(*src - *dst)) && *dst)
		++src, ++dst;

	if ( ret < 0 )
		ret = -1 ;
	else if ( ret > 0 )
		ret = 1 ;

	return( ret );
}
//-------------------------------------------------------------------------------
#ifdef	WIN32
inline WCHAR*	WQSG_getstrL	( WCHAR const*const _TXT , const int _len )
{
	WCHAR* out_0 = NULL;
	int iLen = WQSG_strlen( _TXT );

	if( iLen >= 0 )
	{
		if( (_len >= 0) && (iLen > _len) )
			iLen = _len;

		if( ( (iLen + 1) > 0 ) &&
			( out_0 = new WCHAR[iLen + 1] )
			)
			WQSG_strcpy_Ex( _TXT , out_0 , iLen );
	}
	return out_0;
}
//-------------------------------------------------------------------------------
inline WCHAR*	WQSG_DelSP_L	( WCHAR const*const _TXT )
{
	WCHAR* OUT_0 = NULL;
	int iLen = WQSG_strlen( _TXT );

	if( ( iLen >= 0 ) &&
		( (iLen+1) > 0 ) &&
		( OUT_0 = new WCHAR[iLen+1] )
		)
	{
		WCHAR *s1 = OUT_0;
		WCHAR const* TXT = (WCHAR const*)_TXT;

		while( (L' ' == *TXT) || (L'　' == *TXT) )
			++TXT;

		while( *(s1++) = *(TXT++) )
			;
	}
	return OUT_0;
}
//-------------------------------------------------------------------------------
inline WCHAR*	WQSG_DelSP_ALL	( WCHAR const*const _TXT )
{
	WCHAR* OUT_0 = NULL;
	int iLen = WQSG_strlen( _TXT );

	if( ( iLen >= 0 ) &&
		( (iLen+1) > 0 ) &&
		( OUT_0 = new WCHAR[iLen+1] )
		)
	{
		WCHAR* s1 = OUT_0;
		WCHAR const* TXT = (WCHAR const*)_TXT;
		while(*TXT)
		{
			if( (L'　' != *TXT) && (L' ' != *TXT) )
				*(s1++) = *(TXT++);
			else
				++TXT;
		}
		*s1 = L'\0';
	}
	return OUT_0;
}
//-------------------------------------------------------------------------------
inline WCHAR*	WQSG_abc_ABC	( WCHAR const*const _xiao )
{
	WCHAR* DA_0 = NULL;
	int iLen = WQSG_strlen( _xiao );

	if( ( iLen >= 0 ) &&
		( (iLen+1) > 0 ) &&
		( DA_0 = new WCHAR[iLen+1] )
		)
	{
		WCHAR*da = DA_0;
		WCHAR const* xiao = (WCHAR const*)_xiao;

		while( (*da) = (*(xiao++)) )
		{
			if( (*da >= L'a') &&
				(*da <= L'z')
				)
				(*da) -= 0x20;
			++da;
		}
	}
	return DA_0;
}
//-------------------------------------------------------------------------------
inline WCHAR*	WQSG_ABC_abc	( WCHAR const*const _da )
{
	WCHAR* XIAO_0 = NULL;
	int iLen = WQSG_strlen( _da );

	if( ( iLen >= 0 ) &&
		( (iLen+1) > 0 ) &&
		( XIAO_0 = new WCHAR[iLen+1] )
		)
	{
		WCHAR* xiao = XIAO_0;
		WCHAR const* da = (WCHAR const*)_da;

		while( (*xiao) = (*(da++)) )
		{
			if( ( *xiao >= L'A') &&
				( *xiao <= L'Z')
				)
				(*xiao) += 0x20;
			++xiao;
		}
	}
	return XIAO_0;
}
//-------------------------------------------------------------------------------
inline BOOL	WQSG_IsHexText	( WCHAR const*const _TXT )
{
	WCHAR const* TXT = (WCHAR const*)_TXT;

	if( (!TXT) || (!(*TXT)) )
		return FALSE;

	while( const WCHAR s1 = *TXT )
	{
		if( ((s1 >= '0') && (s1 <= '9')) ||
			((s1 >= 'a') && (s1 <= 'f')) ||
			((s1 >= 'A') && (s1 <= 'F'))
			)
			++TXT;
		else
			return FALSE;
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
inline BOOL	WQSG_IsDecText	( WCHAR const*const _TXT )
{

	WCHAR const* TXT = (WCHAR const*)_TXT;

	if( (!TXT) || (!(*TXT)) )
		return FALSE;

	while( const WCHAR s1 = *TXT )
	{
		if( (s1 >= L'0') && (s1 <= L'9')	)
			++TXT;
		else
			return FALSE;
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
#if WIN32
inline int	WQSG_CheckFileName( WCHAR const*const pFileName )
{
	bool blank = true;
	int i = 0;
	if( (*pFileName == L' ') || (*pFileName == L'　') )
		return i;

	for( ; pFileName[i] ; ++i )
	{
		switch( pFileName[i] )
		{
		case L'\\':
		case L'/':
		case L':':
		case L'*':
		case L'?':
		case L'"':
		case L'<':
		case L'>':
		case L'|':
			return i;
		case L' ':
		case L'　':
			blank = true;
		default:
			blank = false;
			break;
		}
	}
	if( i < 0 )
		i = 0;

	return blank?i:-1;
}
#else
inline int	WQSG_CheckFileName( WCHAR const*const pFileName );
#endif
#endif

#endif

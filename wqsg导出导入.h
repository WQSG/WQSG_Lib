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
#ifndef __WQSG导出导入_H__
#define __WQSG导出导入_H__

#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#include<WQSG_DEF.h>
#include<WQSG.h>
#include<WQSG_File_M.h>
/***************************************************************************
指针格式(共)		DEC
指针个数(共)		HEX
指针表首项地址(共)		HEX
文本基础地址(共)		HEX
文件头长(共)		HEX
表项长度(共)		DEC
指针实际长度(共)		DEC
指针倍率(共)		DEC
指针表结束地址(带长度)	DEC
***************************************************************************/
#define WQSG_TXT_LOGFILE L"log.log"

#define WQSG_TXT_BUF 10485760

namespace WQSG_TEXT_OUT
{

}
//----------------------------------------------------------------------------
class CWQSG_MapTbl_Base
{
	inline static WCHAR* zzzLOAD_TXT_ANSI( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 );
	inline static WCHAR* zzzLOAD_TXT_UTF8( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 );
	inline static WCHAR* zzzLOAD_TXT_UNICODE( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 );
protected:
	inline CWQSG_MapTbl_Base();
	virtual	inline ~CWQSG_MapTbl_Base();

	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL );

	inline static WCHAR* zzzLoadText( WCHAR const*const a_pPathName , WCHAR const*const a_p长度限制错误 , WCHAR const*const a_p内存不足 , WCHAR const*const a_p读取失败 );
public:
};

CWQSG_MapTbl_Base::CWQSG_MapTbl_Base( )
{

}
CWQSG_MapTbl_Base::~CWQSG_MapTbl_Base()
{

}

void CWQSG_MapTbl_Base::zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle )
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

WCHAR* CWQSG_MapTbl_Base::zzzLoadText( WCHAR const*const a_pPathName , WCHAR const*const a_p长度限制错误 , WCHAR const*const a_p内存不足 , WCHAR const*const a_p读取失败 )
{
	WCHAR* pTxt = NULL;
	CWQSG_File fp;

	if( !fp.OpenFile( a_pPathName , 1 ,3 ) )
	{
		zzz_Log( L"控制码表打开失败"  , a_pPathName );
		return pTxt;
	}

	switch( ::WQSG_A_U_X( fp.GetFileHANDLE() ) )
	{
	case en_CP_ANSI:
		pTxt = zzzLOAD_TXT_ANSI( a_pPathName , fp ,
			a_p长度限制错误 , a_p内存不足 , a_p读取失败 );
		break;
	case en_CP_UNICODE:
		pTxt = zzzLOAD_TXT_UNICODE( a_pPathName , fp ,
			a_p长度限制错误 , a_p内存不足 , a_p读取失败 );
		break;
	case en_CP_UTF8:
		pTxt = zzzLOAD_TXT_UTF8( a_pPathName , fp ,
			a_p长度限制错误 , a_p内存不足 , a_p读取失败 );
		break;
	default:
		zzz_Log( L"不支持的编码格式"  , a_pPathName );
	}

	return pTxt;
}

WCHAR* CWQSG_MapTbl_Base::zzzLOAD_TXT_ANSI( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 )//
{
	WCHAR* pText = NULL;

	a_fp.Seek(0);
	const s64 size = a_fp.GetFileSize( );

	if( size > 0x2000000 )
	{//码表太大
		zzz_Log( 长度限制错误 , a_pPathName );
		return pText;
	}

	CWD_BIN bin;
	if( NULL == bin.SetBufSize( (u32)size + 1 ) )
	{
		zzz_Log( 内存不足 , a_pPathName );
		return pText;
	}
	*(bin.BIN + (u32)size) = '\0';

	if( size != a_fp.Read( bin.BIN , (u32)size ) )
	{
		zzz_Log( 读取失败 , a_pPathName );
		return pText;
	}

	pText = ::WQSG_char_W ( (char*)bin.BIN );

	if( NULL == pText )
		zzz_Log( 内存不足 , a_pPathName );

	return pText;
}

WCHAR* CWQSG_MapTbl_Base::zzzLOAD_TXT_UTF8( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 )//
{
	WCHAR* pText = NULL;

	a_fp.Seek(0);
	s64 size = a_fp.GetFileSize( );

	if( size > 0x2000000 )
	{//码表太大
		zzz_Log( 长度限制错误 , a_pPathName );
		return pText;
	}

	if( size < 3 )
	{
		zzz_Log( L"UTF8 长度小于3?" , a_pPathName );
		return pText;
	}

	size -= 3;
	CWD_BIN bin;
	if( NULL == bin.SetBufSize( (u32)size + 1 ) )
	{
		zzz_Log( 内存不足 , a_pPathName );
		return pText;
	}
	*(bin.BIN + (u32)size) = '\0';

	if( size != a_fp.Read( bin.BIN , (u32)size ) )
	{
		zzz_Log( 读取失败 , a_pPathName );
		return pText;
	}
	pText = ::WQSG_UTF8_W ( (char*)bin.BIN );
	if( NULL == pText )
		zzz_Log( 内存不足 , a_pPathName );

	return pText;
}

WCHAR* CWQSG_MapTbl_Base::zzzLOAD_TXT_UNICODE( WCHAR const*const a_pPathName , CWQSG_File& a_fp , WCHAR const*const 长度限制错误 , WCHAR const*const 内存不足 , WCHAR const*const 读取失败 )//_T("控制码表文件过大(我还没见过这么大的码表- -)")
{
	WCHAR* pText = NULL;

	a_fp.Seek(2);
	s64 size = a_fp.GetFileSize( ) - 2;

	if( (size%2) != 0 )
		size--;

	if( size > 0x2000000 )
	{//码表太大
		zzz_Log( 长度限制错误 , a_pPathName );
		return pText;
	}

	WCHAR* tmp = new WCHAR [(u32)size/2 + 1];
	if( NULL == tmp )
	{
		zzz_Log( 内存不足 , a_pPathName );
		return pText;
	}

	tmp[(u32)size/2] = 0;

	if( (u32)size != a_fp.Read( tmp , (u32)size ) )
	{
		delete[]tmp;
		zzz_Log( 读取失败 , a_pPathName );
	}
	else
	{
		pText = (WCHAR*)tmp;
	}

	return pText;
}
//============================================================================
class CWQSG_MapTbl_OUT :public CWQSG_MapTbl_Base
{
	BOOL m_b禁止码表错误;
	inline BOOL Add_TBL( WCHAR const*const a_pPathName , WCHAR* WTXT0 );
	inline BOOL Add_TBL2( WCHAR const*const a_pPathName , WCHAR* WTXT0 , const BOOL 不需要结束符 );
	inline u8* Hex2Bin( const WCHAR* a_pText , INT a_iSYLen )
	{
		u8*const pSY = new UCHAR[a_iSYLen];
		u8* s1 = pSY;

		for( INT val ; a_iSYLen > 0 ; --a_iSYLen )
		{
			::swscanf_s( a_pText++ , L"%02X" , &val );
			*s1 = (u8)val;
			++s1;
			a_pText++;
		}

		return pSY;
	}
protected:
	const u64 m_OVER , m_TURN , m_END;
	BOOL m_bCheckTbl;
	INT m_MAX_字节;

	CWD_tree_del<WCHAR> m_Text;
/////////////////////////////////////////////////////////////
	inline BOOL LOAD_TBL( WCHAR const*const a_pPathName );
	inline BOOL LOAD_TBL2( WCHAR const*const TBL2_path , const BOOL 不需要结束符 = TRUE );
	inline CWQSG_MapTbl_OUT( );
	virtual	inline ~CWQSG_MapTbl_OUT();
	inline void ClearTbl(void);
};

BOOL CWQSG_MapTbl_OUT::Add_TBL( WCHAR const*const a_pPathName , WCHAR* WTXT0 )
{
	while( WCHAR* WTXT_ALL = ::WQSG_GetTXT_Line( &WTXT0 ) )
	{
		///判断空行 WTXT_ALL
		WCHAR* pTempText = ::WQSG_DelSP_ALL( WTXT_ALL );
		if( !(*pTempText) || *(u16*)pTempText == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;
			delete[]pTempText;
			continue;
		}
		delete[]pTempText;

		INT nItmp , nJtmp;
		// WTXT_ALL
		nItmp = ::WQSG_strchr( WTXT_ALL , L'=' );
		if( -1 == nItmp )//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n错误的码表格式(=)" , a_pPathName );

			if( m_b禁止码表错误 )
			{
				delete WTXT_ALL;
				return FALSE;
			}

			delete[]WTXT_ALL;
			continue;
		}
		WCHAR* WTXT_L = ::WQSG_getstrL( WTXT_ALL , nItmp );//取等号左边
		pTempText = WTXT_L ;
		WTXT_L = ::WQSG_DelSP_ALL( pTempText );
		delete[]pTempText;
		nJtmp = ::WQSG_strlen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L) //不是HEX
			|| (nJtmp&1)//单数
			|| !(nJtmp>>1)//为0
			)
		{
			delete[]WTXT_L;

			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}

			delete[]WTXT_ALL;
			continue;
		}
		//取右边delete WTXT_L;delete WTXT_ALL;

		WCHAR*WTXT_R = ::WQSG_getstrL (WTXT_ALL + nItmp + 1,-1);//取等号右边
		nItmp = ::WQSG_strlen(WTXT_R);
		if( nItmp <= 0 )//右边不是一个字,或者没有
		{
			delete[]WTXT_L;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n码表右边必须是一个字" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}
		nItmp = nJtmp>>1;
		////////////////////////////////////////////////////////////////////////////////// 加入树

		u8*const pSY = Hex2Bin( WTXT_L , nItmp );
		delete[]WTXT_L;

		pTempText = m_Text.索引( pSY , nItmp );
		//验证重复
		if(pTempText)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n码表重复" , a_pPathName );
				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete[]pSY;
					return FALSE;
				}
			}
			delete pTempText;
		}
		delete[]WTXT_ALL;

		//添加
		m_Text.压入( pSY , nItmp , WTXT_R );
		delete[]pSY;

		if( nItmp > m_MAX_字节 )
			m_MAX_字节 = nItmp;
		/////////////////////////////////////////
	}
	return TRUE;
}

BOOL CWQSG_MapTbl_OUT::Add_TBL2( WCHAR const*const a_pPathName , WCHAR* WTXT0 , const BOOL 不需要结束符 )
{
	BOOL 有结束符 = FALSE;
	while( WCHAR* WTXT_ALL = ::WQSG_GetTXT_Line(&WTXT0) )
	{
		///判断空行 WTXT_ALL
		WCHAR* pTempText = ::WQSG_DelSP_ALL(WTXT_ALL);
		if( !(*pTempText) || *(u16*)pTempText == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;
			delete[]pTempText;
			continue;
		}
		delete[]pTempText;

		INT nItmp , nJtmp;
		// WTXT_ALL

		nItmp = ::WQSG_strchr(WTXT_ALL,L'=');
		if( -1 == nItmp )//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n错误的控制码表格式(=)" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		WCHAR* WTXT_L = ::WQSG_getstrL ( WTXT_ALL , nItmp );//取等号左边
		pTempText = WTXT_L;
		WTXT_L = ::WQSG_DelSP_ALL(pTempText);
		delete[]pTempText;
		nJtmp = ::WQSG_strlen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L) //不是HEX
			|| (nJtmp & 1)//单数
			|| !(nJtmp >> 1)//为0
			)
		{
			delete[]WTXT_L;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表左边必须是(HEX)" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}


		const INT iSY = nJtmp >> 1;
		u8*const pSY = Hex2Bin( WTXT_L , iSY );
		delete[]WTXT_L;

		//取右边delete pSY;delete WTXT_ALL;
		WCHAR* WTXT_R = ::WQSG_getstrL(WTXT_ALL + nItmp + 1,-1);//取等号右边
		nItmp = (int)::wcslen(WTXT_R);
		if( nItmp <= 0 )//无字
		{
			delete[]pSY;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表右边必须至少有一个字" , a_pPathName );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		if( nItmp > 255)
		{
			delete[]pSY;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n控制码表右边不能超过255个字" );

			delete[]WTXT_ALL;
			if( m_b禁止码表错误 )
				return FALSE;

			continue;
		}
		///////////////////判断控制符
		pTempText = ::WQSG_abc_ABC(WTXT_R);
		if((*((u64*)pTempText) == m_OVER))
		{
			有结束符= TRUE;
			delete[]WTXT_R;
			WTXT_R = pTempText;

			pTempText += 4;

			if(*pTempText)
			{
				if(!::WQSG_是十进制文本(pTempText)
					||::wcslen(pTempText) > 3)
				{
					delete[]pSY;delete[]WTXT_R;
					CWQSG_strW errStr( WTXT_ALL );

					zzz_Log( errStr + L"\r\n制码表结束符数字错误\r\n    格式为: 编码=OVERx\r\n  OVER大小写都行\r\n    x 只能为十进制,不能有多于字符,最长位" , a_pPathName );

					delete[]WTXT_ALL;
					if( m_b禁止码表错误 )
						return FALSE;

					continue;
				}
			}
		}
		else if(*((u64*)pTempText) == m_TURN)
		{
			delete[]WTXT_R;
			WTXT_R = pTempText;

			pTempText += 4;

			if(!::WQSG_是十进制文本(pTempText)
				||::wcslen(pTempText) > 2)
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n控制码表跳跃符数字错误\r\n    格式为: 编码=TURNx\r\n  TURN大小写都行\r\n    x 只能为十进制,不能有多于字符,最长2位" , a_pPathName );

				delete[]WTXT_ALL;
				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}
		}
		else if( *((u64*)pTempText) == m_END )
		{
			delete[]WTXT_R;

			if( ::wcslen(pTempText) != 4 )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n\"END:\"后面不能有多余的字符" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			WCHAR* pTxt = m_Text.索引( pSY , iSY );
			if( NULL == pTxt )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n与当前\"END:\"对应编码的项不能为空" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			if( 1 != ::WQSG_strlen( pTxt ) )
			{
				delete[]pSY;delete[]WTXT_R;
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n与当前\"END:\"对应编码的项必须为一个字符" , a_pPathName );

				delete[]WTXT_ALL;

				if( m_b禁止码表错误 )
					return FALSE;

				continue;
			}

			WTXT_R = new WCHAR[6];

			WQSG_strcpy( pTempText , WTXT_R );
			WTXT_R[4] = *pTxt;
			WTXT_R[5] = 0;

			delete[]pTempText;
			delete[]pTxt;

			m_Text.压入( pSY , iSY , NULL );
		}
		else
		{
			delete[]pTempText;
			pTempText = WTXT_R;

			WTXT_R = new WCHAR[nItmp + 3];
			WCHAR *Wtmp2 = WTXT_R;
			*(Wtmp2++) = L'{';
			Wtmp2 += ::WQSG_strcpy(pTempText,Wtmp2);
			*(Wtmp2++) = L'}';
			*Wtmp2 = 0;
			delete[]pTempText;
		}
		////////////////////////////////////////////////////////////////////////////////// 加入树

		pTempText = m_Text.索引( pSY , iSY );
		////////////////////
		if(pTempText)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );

				zzz_Log( errStr + L"\r\n控制码表重复" , a_pPathName );

				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete[]pSY;
					return FALSE;
				}
			}
			delete pTempText;
		}
		delete[]WTXT_ALL;
		/////////////////////////////////////////

		m_Text.压入( pSY , iSY , WTXT_R );
		delete[]pSY;

		if( iSY > m_MAX_字节 )
			m_MAX_字节 = iSY;
	}
	if(不需要结束符 || 有结束符)
		return TRUE;

	zzz_Log( L"\r\n控制符至少需要一个结束符" , a_pPathName );
	return FALSE;		
}

BOOL CWQSG_MapTbl_OUT::LOAD_TBL( WCHAR const*const a_pPathName )
{
	BOOL bRt = FALSE;

	WCHAR* pTxt = zzzLoadText( a_pPathName , L"码表文件不得大于32MB(这么大,这是码表?)" ,
		L"码表内存不足" , L"读取码表失败" );

	if( pTxt )
	{
		bRt = Add_TBL( a_pPathName , pTxt );
		delete[]pTxt;
	}
	return bRt;
}

BOOL CWQSG_MapTbl_OUT::LOAD_TBL2( WCHAR const*const a_pPathName , const BOOL 不需要结束符 )
{
	BOOL bRt = FALSE;

	WCHAR* pTxt = zzzLoadText( a_pPathName , L"控制码表文件不得大于32MB(这么大,这是码表?)" ,
		L"控制码表内存不足" , L"读取控制码表失败" );

	if( pTxt )
	{
		bRt = Add_TBL2( a_pPathName , pTxt , 不需要结束符 );
		delete[]pTxt;
	}
	return bRt;
}

CWQSG_MapTbl_OUT::CWQSG_MapTbl_OUT( )
: CWQSG_MapTbl_Base( )
, m_b禁止码表错误(TRUE)
, m_OVER( *((u64*)L"OVER") )
, m_TURN( *((u64*)L"TURN") )
, m_END( *((u64*)L"END:") )
{
}

CWQSG_MapTbl_OUT::~CWQSG_MapTbl_OUT()
{
}

void CWQSG_MapTbl_OUT::ClearTbl(void)
{
	m_MAX_字节 = 0;
	m_Text.清空();
}
//============================================================================
class CWQSG_MapTbl_IN :public CWQSG_MapTbl_Base
{
	BOOL m_b禁止码表错误;
	inline BOOL Add_TBL( WCHAR const*const a_pPathName , WCHAR* WTXT0 );
	inline BOOL Add_TBL2( WCHAR const*const a_pPathName ,WCHAR* WTXT0 );
protected:
	CWD_tree_del< CWD_BIN >	m_MB2;
	CWD_BIN* m_MB[65536];
	BOOL m_bCheckTbl;
	u64 m_OVER , m_TURN;

	inline BOOL	LOAD_TBL( WCHAR const*const a_pPathName );
	inline BOOL	LOAD_TBL2( WCHAR const*const TBL2_path );
	inline void ClearTbl(void);
	inline CWQSG_MapTbl_IN( );
	virtual inline ~CWQSG_MapTbl_IN();
};

BOOL CWQSG_MapTbl_IN::Add_TBL( WCHAR const*const a_pPathName , WCHAR* WTXT0 )
{
	while(*WTXT0)
	{
		WCHAR*WTXT_ALL = ::WQSG_GetTXT_Line(&WTXT0);
		///判断空行 WTXT_ALL
		WCHAR *TMP = ::WQSG_DelSP_ALL(WTXT_ALL);

		if(!(*TMP) || *(u16*)TMP == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;
			delete[]TMP;
			continue;
		}
		delete[]TMP;

		WCHAR* WTXT_L;
		WCHAR* WTXT_R;
		UINT I , J;
		// WTXT_ALL
		I = ::WQSG_strchr(WTXT_ALL,L'=');
		if(-1 == I)//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );

			zzz_Log( errStr + L"\r\n错误的码表格式" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}
		WTXT_L = ::WQSG_getstrL (WTXT_ALL,I);//取等号左边
		TMP = WTXT_L;
		WTXT_L = ::WQSG_DelSP_ALL (WTXT_L);
		delete[]TMP;
		// WTXT_ALL WTXT_L
		J = (UINT)::wcslen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L)//不是HEX
			|| (J & 1)//单数
			|| !(J >> 1)//小于2
			)
		{
			delete[]WTXT_L;
			CWQSG_strW errStr( WTXT_ALL );
			zzz_Log( errStr + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}

		WTXT_R = ::WQSG_getstrL(WTXT_ALL + I + 1,-1);//取等号右边
		I = (UINT)::wcslen(WTXT_R);
		if(1 != I || !I)//右边不是一个字,或者没有
		{
			delete[]WTXT_L;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );
			zzz_Log( errStr + L"\r\n码表右边必须是一个字符" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}			
		////////////////////////////////////////////////////////////////////////////////// 加入树

		I = J >> 1;
		CWD_BIN *BIN = new CWD_BIN;
		BIN->LEN = I;
		BIN->BIN = new UCHAR[I];
		UCHAR *s1 = (BIN->BIN);
		TMP = WTXT_L;

		for(UINT i = 0,K;i < I;i++)
		{
			::swscanf_s(TMP++,L"%02X",&K);
			*(s1++) = (UCHAR)K;
			TMP++;
		}
		delete[]WTXT_L;
		////delete WTXT_R;delete BIN;
		CWD_BIN *tmp_BIN;
		tmp_BIN = m_MB[*WTXT_R];
		if(tmp_BIN)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );
				zzz_Log( errStr + L"\r\n码表重复" , a_pPathName );
				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete BIN;
					return FALSE;
				}
			}
			delete tmp_BIN;
		}
		delete[]WTXT_ALL;
		m_MB[*WTXT_R] = BIN;
		/////////////////////////////////////////
		delete[]WTXT_R;
	}
	return TRUE;
}
BOOL CWQSG_MapTbl_IN::Add_TBL2( WCHAR const*const a_pPathName ,WCHAR* WTXT0 )
{
	while(*WTXT0)
	{
		WCHAR*WTXT_ALL = ::WQSG_GetTXT_Line(&WTXT0);
		///判断空行 WTXT_ALL
		WCHAR *TMP = ::WQSG_DelSP_ALL(WTXT_ALL);
		if( !(*TMP) || *(u16*)TMP == *(u16*)L"//" )
		{
			delete[]WTXT_ALL;delete[]TMP;
			continue;
		}
		delete[]TMP;

		WCHAR *WTXT_L,
			*WTXT_R;
		UINT I,J;
		// WTXT_ALL
		I = ::WQSG_strchr(WTXT_ALL,L'=');
		if(-1 == I)//错误的码表格式,找不到"="
		{
			CWQSG_strW errStr( WTXT_ALL );
			zzz_Log( errStr + L"\r\n错误的码表格式" );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}
		WTXT_L = ::WQSG_getstrL (WTXT_ALL,I);//取等号左边
		TMP = WTXT_L;
		WTXT_L = ::WQSG_DelSP_ALL (WTXT_L);
		delete TMP;
		// WTXT_ALL WTXT_L
		J = (UINT)::wcslen(WTXT_L);//验证长度
		if(!::WQSG_是十六进制文本(WTXT_L)//不是HEX
			|| (J & 1)//单数
			|| !(J >> 1)//小于2
			)
		{
			delete[]WTXT_L;
			CWQSG_strW errStr( WTXT_ALL );
			zzz_Log( errStr + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}

		WTXT_R = ::WQSG_getstrL (WTXT_ALL + I + 1,-1);//取等号右边
		I = (UINT)::wcslen(WTXT_R);
		if(!I)//没有
		{
			delete[]WTXT_L;delete[]WTXT_R;
			CWQSG_strW errStr( WTXT_ALL );
			zzz_Log( errStr + L"\r\n码表右边必须至少有一个字符" , a_pPathName );
			if( m_b禁止码表错误 )
			{
				delete[]WTXT_ALL;
				return FALSE;
			}
			delete[]WTXT_ALL;
			continue;
		}

		if(I >= 4){
			TMP = WQSG_abc_ABC( WTXT_R );
			if( ( *((u64*)L"OVER") == *((u64*)TMP) )
				|| ( *((u64*)L"TURN") == *((u64*)TMP) ) )
			{
				delete[]TMP;
				delete[]WTXT_L;delete[]WTXT_R;
				delete[]WTXT_ALL;
				continue;
			}
			delete[]TMP;
		}
		////////////////////////////////////////////////////////////////////////////////// 加入树

		J >>= 1;
		CWD_BIN *BIN = new CWD_BIN;
		BIN->LEN = J;
		BIN->BIN = new UCHAR[J];
		UCHAR *s1 = (BIN->BIN);
		TMP = WTXT_L;

		for(UINT i = 0,K;i < J;i++)
		{
			::swscanf_s(TMP++,L"%02X",&K);
			*(s1++) = (UCHAR)K;
			TMP++;
		}
		delete[]WTXT_L;
		////delete WTXT_R;delete BIN;
		CWD_BIN *TMP_BIN = m_MB2.索引((UCHAR*)WTXT_R,I<<1);
		if(TMP_BIN)
		{
			if(m_bCheckTbl)
			{
				CWQSG_strW errStr( WTXT_ALL );
				zzz_Log( errStr + L"\r\n码表重复" , a_pPathName );
				if( m_b禁止码表错误 )
				{
					delete[]WTXT_ALL;delete[]WTXT_R;delete BIN;
					return FALSE;
				}
			}
			delete TMP_BIN;
		}
		delete[]WTXT_ALL;
		m_MB2.压入((UCHAR*)WTXT_R,(I<<1),BIN);
		/////////////////////////////////////////
		delete[]WTXT_R;
	}
	return TRUE;
}
BOOL CWQSG_MapTbl_IN::LOAD_TBL( WCHAR const*const a_pPathName )
{
	BOOL bRt = FALSE;

	WCHAR* pTxt = zzzLoadText( a_pPathName , L"码表文件不得大于32MB(这么大,这是码表?)" ,
		L"码表内存不足" , L"读取码表失败" );

	if( pTxt )
	{
		bRt = Add_TBL( a_pPathName , pTxt );
		delete[]pTxt;
	}
	return bRt;
}

BOOL CWQSG_MapTbl_IN::LOAD_TBL2( WCHAR const*const a_pPathName )
{
	BOOL bRt = FALSE;

	WCHAR* pTxt = zzzLoadText( a_pPathName , L"控制码表文件不得大于32MB(这么大,这是码表?)" ,
		L"控制码表内存不足" , L"读取控制码表失败" );

	if( pTxt )
	{
		bRt = Add_TBL2( a_pPathName , pTxt );
		delete[]pTxt;
	}
	return bRt;
}

void CWQSG_MapTbl_IN::ClearTbl(void)
{
	for(int i = 0;i < 65536;++i)
		delete m_MB[i];

	::memset(m_MB,0,sizeof(m_MB));
	m_MB2.清空();
}

CWQSG_MapTbl_IN::CWQSG_MapTbl_IN( )
	: CWQSG_MapTbl_Base( )
	, m_b禁止码表错误(TRUE)
	, m_OVER( *((ULONGLONG*)L"结束符") )
	, m_TURN( *((ULONGLONG*)L"跳跃符") )
{
	::memset( m_MB , 0 , sizeof(m_MB) );
}

CWQSG_MapTbl_IN::~CWQSG_MapTbl_IN()
{
	for( INT k = 0 ; k <= 65535 ; ++k)
		delete m_MB[k];
}
//----------------------------------------------------------------------------
class CWQSG_TXT_O:private CWQSG_MapTbl_OUT
{
#ifndef _____________________________________________
	inline BOOL 正式导出( CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path );
#else
	BOOL 正式导出( CWQSG_File& a_fp , WQSG_File_mem& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path );
#endif
private:
	UINT MAX_OUT;
	UINT MIN_OUT;
	WCHAR* BUF_OUT;
	WCHAR addr_len[20];
public:
	inline CWQSG_TXT_O( );
	virtual	inline ~CWQSG_TXT_O();
	inline void ClearTbl(void);
	inline BOOL 载入码表( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl = FALSE );
#ifndef _____________________________________________
	inline BOOL 导出文本( const WCHAR* rom_path , const WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#else
	BOOL 导出文本( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#endif
};

#define	DEF_写出文本( __DEF_ot_fp , __DEF_ot_addr , __DEF_ot_len , __DEF_ot_txt ) \
	if( __DEF_ot_len > MIN_OUT && __DEF_ot_len < MAX_OUT )\
	{\
	::WQSG_strcpy ( L"\r\n\r\n" , __DEF_ot_txt );\
	::swprintf( addr_len , L"%08X,%u," , (u32)( __DEF_ot_addr & 0xFFFFFFFF ) , __DEF_ot_len);\
	if( !__DEF_ot_fp.WriteStrW( addr_len ) ){zzz_Log( L"写文本失败"  , TXT_path );}\
	if( !__DEF_ot_fp.WriteStrW( BUF_OUT ) ){zzz_Log( L"写文本失败"  , TXT_path );}\
}

#ifndef _____________________________________________
BOOL CWQSG_TXT_O::正式导出( CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path )
{
	s64		ot_addr = 开始位置;
	UINT	ot_len = 0;
	WCHAR*	ot_txt = BUF_OUT;

	*ot_txt = L'\0';

	u8 tmp_C[1024];
	//--------------------------------------
	while( 剩余长度 )
	{
		UINT BufferLen = mFile_ROM.GetUCHAR( ( 剩余长度 > m_MAX_字节 )?m_MAX_字节:(UINT)剩余长度 , tmp_C );

		if( 0 == BufferLen )
			break;

		UINT 索引长度;
		WCHAR* tmp2 = m_Text.匹配( tmp_C , BufferLen , 索引长度 );

		if( tmp2 )
		{
			剩余长度 -= 索引长度;
			开始位置 += 索引长度;
			ot_len += 索引长度;
			mFile_ROM.BACK( BufferLen - 索引长度 );

			if( *((u64*)tmp2) == m_OVER )
			{//是结束符
				ot_len -= 索引长度;

				if( tmp2[4] )
				{
					::swscanf_s( tmp2 + 4 , L"%u" , &BufferLen );
					if( BufferLen > 剩余长度 )
						剩余长度 = 0;
					else
						剩余长度 -= BufferLen;

					开始位置 += BufferLen;
					mFile_ROM.SeekADD( BufferLen );
				}
				/*
				s1 += ::WQSG_strcpy(L"{结束符",s1);
				for(UINT i = 0;i < 索引长度;i++){
				::swprintf_s((s1++),WQSG_TXT_BUF,L"%02X",tmp_C[i]);
				s1++;
				}
				*(s1++) = L'}';*/
				/////////////////////////////
				DEF_写出文本( a_fp , ot_addr , ot_len , ot_txt )
					ot_txt = BUF_OUT;
				*BUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else if(*((u64*)tmp2) == m_TURN)
			{//是跳跃符
				ot_txt += ::WQSG_strcpy( L"{跳跃符" , ot_txt );
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , tmp_C[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'：';
				///////////////
				::swscanf( tmp2 + 4 , L"%u" , &BufferLen );

				索引长度 = mFile_ROM.GetUCHAR( BufferLen , tmp_C );

				if( 索引长度 > 剩余长度 )
					剩余长度 = 0;
				else
					剩余长度 -= 索引长度;

				开始位置 += 索引长度;
				ot_len += 索引长度;
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , tmp_C[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'}';
			}
			else if(*((u64*)tmp2) == m_END)
			{
				ot_txt += ::WQSG_strcpy( tmp2 + 4 , ot_txt );

				/////////////////////////////
				DEF_写出文本( a_fp , ot_addr , ot_len , ot_txt )
					ot_txt = BUF_OUT;
				*BUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else
			{//通常
				ot_txt += ::WQSG_strcpy( tmp2 , ot_txt );
			}
		}
		else
		{
			mFile_ROM.BACK( --BufferLen );
			--剩余长度;
			++开始位置;
			DEF_写出文本( a_fp , ot_addr , ot_len , ot_txt )
				ot_txt = BUF_OUT;
			*BUF_OUT = 0;
			ot_len = 0;
			ot_addr = 开始位置;
		}
	}
	DEF_写出文本( a_fp , ot_addr , ot_len , ot_txt )

		return TRUE;
}
#else
BOOL CWQSG_TXT_O::正式导出( CWQSG_File& a_fp , WQSG_File_mem& mFile_ROM , s64 开始位置 , s64 剩余长度 , WCHAR const*const TXT_path )
{
	s64		ot_addr = 开始位置;
	UINT	ot_len = 0;
	WCHAR*	ot_txt = BUF_OUT;

	*ot_txt = L'\0';
	//--------------------------------------
	while( 剩余长度 )
	{
		const u32 BufferLen = (剩余长度 > MAX_字节 )?(MAX_字节):(u32)剩余长度;

		u8 const*const Buffer = mFile_ROM.GetPtr( 开始位置 , BufferLen );
		if( !Buffer )
		{
			MessageBoxW( L"未知错误1???" , TXT_path );
			return FALSE;
		}
		//-------------------------------------
		UINT 索引长度 = BufferLen;
		UINT	xlen = 0;
		WCHAR const* tmp2 = NULL;
		{
			节点_TXT_W**	tmpTree = m_ROOT;

			u8 const* SY_tmp = Buffer;
			while( 索引长度-- )
			{
				if( 节点_TXT_W*const tmpNode = tmpTree[*(SY_tmp++)] )
				{//存在下一层
					if( tmpNode->m_TXT_W )
					{
						tmp2 = tmpNode->m_TXT_W;
						xlen = 索引长度;
					}
					tmpTree = tmpNode->m_next;
				}
				else
				{//不存在下一层
					if( tmp2 )
					{
						索引长度 = --xlen;
					}
					break;
				}
			}
			索引长度 = BufferLen - (++索引长度);
		}
		//------------------------------------------------
		if( tmp2 )
		{
			剩余长度 -= 索引长度;
			开始位置 += 索引长度;

			ot_len += 索引长度;
			if( *((u64*)tmp2) == OVER )
			{//是结束符
				ot_len -= 索引长度;

				if( tmp2[4] )
				{
					::swscanf_s( tmp2 + 4 , L"%u" , &索引长度 );
					if( 索引长度 > 剩余长度 )
						剩余长度 = 0;
					else
						剩余长度 -= 索引长度;

					开始位置 += 索引长度;
				}
				/*
				s1 += ::WQSG_strcpy(L"{结束符",s1);
				for(UINT i = 0;i < 索引长度;i++){
				::swprintf_s((s1++),WQSG_TXT_BUF,L"%02X",tmp_C[i]);
				s1++;
				}
				*(s1++) = L'}';*/
				/////////////////////////////
				DEF_写出文本( ot_addr , ot_len , ot_txt )
					ot_txt = BUF_OUT;
				*BUF_OUT = 0;
				ot_len = 0;
				ot_addr = 开始位置;
			}
			else if(*((U64*)tmp2) == TURN)
			{//是跳跃符
				ot_txt += ::WQSG_strcpy( L"{跳跃符" , ot_txt );
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , Buffer[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'：';
				///////////////
				::swscanf( tmp2 + 4 , L"%u" , &索引长度 );
				if( 索引长度 > 剩余长度 )
				{
					索引长度 = (UINT)剩余长度;
					剩余长度 = 0;
				}
				else
					剩余长度 -= 索引长度;

				u8 const*const buf = mFile_ROM.GetPtr( 开始位置 , 索引长度 );
				if( NULL == buf )
				{
					MessageBoxW( L"未知错误2???" , TXT_path );
					mFile_ROM.Free();
					return FALSE;
				}

				开始位置 += 索引长度;

				ot_len += 索引长度;
				for( UINT i = 0 ; i < 索引长度 ; ++i )
				{
					::swprintf( ot_txt , L"%02X" , buf[i] );
					ot_txt += 2;
				}
				*(ot_txt++) = L'}';
			}
			else
			{//通常
				ot_txt += ::WQSG_strcpy( tmp2 , ot_txt );
			}
		}
		else
		{
			--剩余长度;
			++开始位置;
			DEF_写出文本( ot_addr , ot_len , ot_txt )
				ot_txt = BUF_OUT;
			*BUF_OUT = 0;
			ot_len = 0;
			ot_addr = 开始位置;
		}
		mFile_ROM.Free();
	}
	DEF_写出文本( ot_addr , ot_len , ot_txt )

		return TRUE;
}
#endif
#undef DEF_写出文本

CWQSG_TXT_O::CWQSG_TXT_O( )
:CWQSG_MapTbl_OUT( )
{
	BUF_OUT = new WCHAR[ WQSG_TXT_BUF + 1 ];
}

CWQSG_TXT_O::~CWQSG_TXT_O()
{
	delete[]BUF_OUT;
}

void CWQSG_TXT_O::ClearTbl(void)
{
	CWQSG_MapTbl_OUT::ClearTbl();
}

BOOL CWQSG_TXT_O::载入码表( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl )
{
	m_bCheckTbl = a_bCheckTbl;
	ClearTbl();
	if( FALSE == LOAD_TBL( a_pPathName ) )
	{
		ClearTbl();
		return FALSE;
	}
	if( TBL2_path )
	{
		if( *TBL2_path )
			if( FALSE == LOAD_TBL2( TBL2_path ) )
			{
				ClearTbl();
				return FALSE;
			}
	}
	return TRUE;
}
#ifndef _____________________________________________
BOOL CWQSG_TXT_O::导出文本( const WCHAR* rom_path , const WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN ,UINT MAX )
{
	if( KS < 0 )
	{
		zzz_Log( L"开始地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	if( JS < 0 )
	{
		zzz_Log( L"结束地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	*BUF_OUT = 0;
	//////////////// 初始化变量
	MIN_OUT = MIN;
	MAX_OUT = MAX;
	///////////////////////////
	WQSG_FM_FILE mFile_ROM;
	if( !mFile_ROM.OpenFile( rom_path , 3 ) )
	{
		zzz_Log( L"打开ROM文件失败"  , rom_path );
		return FALSE;
	}
	////////// 验证地址
	if( KS > JS )
	{
		zzz_Log( L"结束地址 不能小于 开始地址"  , rom_path );
		return FALSE;
	}
	s64 romsize = mFile_ROM.Get文件长度();
	if( KS >= romsize )
	{
		zzz_Log( L"开始地址不存在,超出文件"  , rom_path );
		return FALSE;
	}
	if( JS >= romsize )
		JS = romsize - 1;
	/////////////////////
	CWQSG_File fp;
	if( !fp.OpenFile( TXT_path , 4 , 3 ) )
	{
		zzz_Log( L"创建文本文件失败"  , TXT_path );
		return FALSE;
	}
	if( !mFile_ROM.SeekTo(KS) )
	{
		zzz_Log( L"文件指针设置失败"  , rom_path );
		return FALSE;
	}
	if( 2 != fp.Write("\377\376" , 2 ) )
	{
		zzz_Log( L"写文本头失败"  , TXT_path );
		return FALSE;
	}
	if( JS > 0xFFFFFFFF )
	{
		u32 JJJ = (u32)(JS>>32);
		::swprintf_s( BUF_OUT , 4096 , L".WQSG:%08X\r\n\r\n\r\n" , JJJ );
		if( fp.Write( BUF_OUT , 40 ) )
		{
			zzz_Log( L"写文本信息头失败"  , TXT_path );
			return FALSE;
		}
	}
	return( 正式导出( fp , mFile_ROM , KS , (JS - KS + 1) , TXT_path ) );
}
#else
BOOL CWQSG_TXT_O::导出文本( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 )
{
	if( KS < 0 )
	{
		MessageBox( L"开始地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	if( JS < 0 )
	{
		MessageBox( L"结束地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
		return FALSE;
	}
	*BUF_OUT = 0;
	//////////////// 初始化变量
	MIN_OUT = MIN;
	MAX_OUT = MAX;
	///////////////////////////
	WQSG_File_mem mFile_ROM;
	if( !mFile_ROM.OpenFile( rom_path , 3 ) )
	{
		MessageBox( L"打开ROM文件失败"  , rom_path );
		return FALSE;
	}
	////////// 验证地址
	if( KS > JS )
	{
		MessageBox( L"结束地址 不能小于 开始地址"  , rom_path );
		return FALSE;
	}
	s64 romsize;
	mFile_ROM.GetFileSize( romsize );
	if( KS >= romsize )
	{
		MessageBox( L"开始地址不存在,超出文件"  , rom_path );
		return FALSE;
	}
	if( JS >= romsize )
		JS = romsize - 1;
	/////////////////////
	CWQSG_File fp;
	if( !fp.OpenFile( TXT_path , 4 , 3 ) )
	{
		MessageBox( L"创建文本文件失败"  , TXT_path );
		return FALSE;
	}
	//		if( !mFile_ROM.SeekTo(KS) )
	//		{
	//			MessageBox( L"文件指针设置失败"  , rom_path );
	//			return FALSE;
	//		}
	if( 2 != fp.Write("\377\376" , 2 ) )
	{
		MessageBox( L"写文本头失败"  , TXT_path );
		return FALSE;
	}
	if( JS > 0xFFFFFFFF )
	{
		u32 JJJ = (u32)(JS>>32);
		::swprintf_s( BUF_OUT , 4096 , L".WQSG:%08X\r\n\r\n\r\n" , JJJ );
		if( fp.Write( BUF_OUT , 40 ) )
		{
			MessageBox( L"写文本信息头失败"  , TXT_path );
			return FALSE;
		}
	}
	return( 正式导出( fp , mFile_ROM , KS , (JS - KS + 1) , TXT_path ) );
}
#endif
//----------------------------------------------------------------------------
class WQSG_TXT_I:private CWQSG_MapTbl_IN
{
	CWD_BIN		BIN_OUT;
	BOOL		m_是否提示长度不足;
	INT			m_填充;
	u8			m_单字节;
	u16			m_双字节;

	BOOL 实际导入( CWQSG_File& ROM_File , C内存文本_W& WQSG , WCHAR const*const 提示文本 )
	{
		s64 地址,地址基址 = 0;
		UINT 长度;
		BOOL 找标记= TRUE;
		while( WCHAR*const 一行 = WQSG.取一行文本() )
		{
			//判断空行
			WCHAR * tmp = ::WQSG_DelSP_L(一行),
				* tmp2;
			if(L'\0' == *tmp)
			{
				delete[]一行;delete[]tmp;
				continue;
			}
			UINT i,k;
			if(找标记)
			{
				//找标记
				找标记= FALSE;
				i = ::WQSG_strstr(tmp,L".WQSG:");
				if(i == 0)
				{
					tmp2 = ::WQSG_getstrL(tmp + 6,-1);
					delete[]tmp;
					k = (UINT)::wcslen(tmp2);
					if(k != 8)
					{
						delete[]tmp2;
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr + L"\r\n文本格式错误,信息头后缀\".WQSG:XXXXXXXX\",\nXXXXXXXX必须是十六进制文本(HEX)" , 提示文本 );
						delete[]一行;
						return FALSE;
					}
					if(!::WQSG_是十六进制文本(tmp2)
						|| 1 != ::swscanf_s(tmp2,L"%X",&k)
						)
					{
							delete[]tmp2;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n文本格式错误,信息头后缀\".WQSG:XXXXXXXX\",\nXXXXXXXX不是合法的十六进制文本(HEX)" , 提示文本 );
							delete[]一行;
							return FALSE;
					}
					delete[]一行;delete[]tmp2;
					地址基址|= k;
					地址基址<<= 32;
					continue;
				}
			}
			//注释
			i = ::WQSG_strstr(tmp,L"//");
			if(0 == i)
			{
				delete[]一行;delete[]tmp;
				continue;
			}
			delete[]tmp;
			//寻找第一个逗号
			i = ::WQSG_strchr(一行,L',');
			if(-1 == i)
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,找不到第一个逗号" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			tmp = ::WQSG_getstrL(一行,i);
			tmp2 = ::WQSG_DelSP_ALL(tmp);
			delete[]tmp;
			//////////////////////// tmp2
			k = (UINT)::wcslen(tmp2);
			if(0 == k || 8 < k)
			{
				delete[]tmp2;
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,地址错误(1)" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			if(!::WQSG_是十六进制文本(tmp2)
				|| 0 == ::swscanf_s(tmp2,L"%X",&k)
				)
			{
				delete[]tmp2;
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,地址错误(1),\r\n不是合法的十六进制数字" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			地址= k;
			delete[]tmp2;
			WCHAR * 一行_tmp = 一行 + i + 1;
			/////////	//寻找第二个逗号
			i = ::WQSG_strchr(一行_tmp,L',');
			if(-1 == i)
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,找不到第二个逗号" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			tmp = ::WQSG_getstrL(一行_tmp,i);
			tmp2 = ::WQSG_DelSP_ALL(tmp);
			delete[]tmp;
			///////////////////// tmp2
			k = (UINT)::wcslen(tmp2);
			if(0 == k || 5 < k)
			{
				delete[]tmp2;
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,长度错误(2)\r\n 必须0位> 十进制数<= 5位" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			if(!::WQSG_是十进制文本(tmp2)
				|| 0 == ::swscanf_s(tmp2,L"%u",&k)
				)
			{
				delete[]tmp2;
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n文本格式错误,长度错误(3), 不是合法的十进制数字" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			长度= k;
			delete[]tmp2;
			一行_tmp += (i + 1);
			/////////////////////////////////////
			BIN_OUT.LEN = 0;
			u8* s1 = BIN_OUT.BIN,* s2;
			while(*一行_tmp)
			{
				if(L'{' == *一行_tmp)
				{
					i = ::WQSG_strchr(一行_tmp,L'}');
					if(-1 == i)
					{
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr + L"\r\n文本格式错误,找不到控制符的右半边" , 提示文本 );
						delete[]一行;
						return FALSE;
					}
					tmp = ::WQSG_getstrL(++一行_tmp,--i);
					一行_tmp += i + 1;
					///////////////////////// 结束符tmp
					if(0 == ::WQSG_strstr(tmp,L"结束符"))
					{
						tmp2 = ::WQSG_DelSP_ALL(tmp + 3);
						/////////////////////////  tmp tmp2
						k = (UINT)::wcslen(tmp2);
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp2))//有非HEX字符
							)
						{
								delete[]tmp2;
								CWQSG_strW errStr( 一行 );
								zzz_Log( errStr + L"\r\n结束符后缀错误" , 提示文本 );
								delete[]tmp;delete[]一行;
								return FALSE;
						}
						delete[]tmp;
						//
						k >>= 1;
						BIN_OUT.LEN += k;
						tmp = tmp2;
						for(;k;k--)
						{
							::swscanf_s(tmp++,L"%2x",&i);
							tmp++,*(s1++) = (u8)i;
						}
						delete[]tmp2;
						//
					}//////////////////// 跳跃符
					else if(0 == ::WQSG_strstr(tmp,L"跳跃符"))
					{
						tmp2 = ::WQSG_DelSP_ALL(tmp + 3);
						i = ::WQSG_strchr(tmp2,L'：');
						// tmp  tmp2
						if(-1 == i)
						{
							delete[]tmp2;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n跳跃符错误,找不到：" , 提示文本 );
							delete[]tmp;delete[]一行;
							return FALSE;
						}
						WCHAR* tmp4 = ::WQSG_getstrL(tmp2,i);
						k = (UINT)::wcslen(tmp4);
						WCHAR* tmp3 = ::WQSG_getstrL(tmp2 + i + 1,-1);
						delete[]tmp2;
						tmp2 = tmp3;
						// tmp  tmp2 tmp4
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp4))//有非HEX字符
							)
						{
								delete[]tmp2;
								delete[]tmp4;
								CWQSG_strW errStr( 一行 );
								zzz_Log( errStr + L"\r\n跳跃符前缀格式错误" , 提示文本 );
								delete[]tmp;delete[]一行;
								return FALSE;
						}
						//
						k >>= 1;
						BIN_OUT.LEN += k;
						tmp3 = tmp4;
						for(;k;k--)
						{
							::swscanf_s(tmp3++,L"%2x",&i);
							tmp3++,*(s1++) = (u8)i;
						}
						delete[]tmp4;
						////////////// 后缀tmp tmp2
						k = (UINT)::wcslen(tmp2);
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp2))//有非HEX字符
							)
						{
								delete[]tmp2;
								CWQSG_strW errStr( 一行 );
								zzz_Log( errStr + L"\r\n跳跃符后缀格式错误" , 提示文本 );
								delete[]tmp;delete[]一行;
								return FALSE;
						}
						delete[]tmp;
						//
						k >>= 1;
						BIN_OUT.LEN += k;
						tmp3 = tmp2;
						for(;k;k--)
						{
							::swscanf_s(tmp3++,L"%2x",&i);
							tmp3++,*(s1++) = (u8)i;
						}
						delete[]tmp2;
					}/////////// 自定义控制符
					else
					{
						::CWD_BIN * BIN_tmp = m_MB2.索引((u8*)tmp,(int)(::wcslen(tmp)<<1));
						if(BIN_tmp)
						{
							for(i = BIN_tmp->LEN,s2 = BIN_tmp->BIN;i;i--)
								*(s1++) = *(s2++);

							BIN_OUT.LEN += BIN_tmp->LEN;
						}///////////////////// 未定义控制符
						else
						{
							k = ::WQSG_strlen(tmp) + 3;
							WCHAR * 文字= new WCHAR[k];
							*文字= L'“';
							::WQSG_strcpy(tmp,文字+ 1);
							文字[k-2] = L'”';
							文字[k-1] = 0;
							delete[]tmp;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr  +  L"\r\n未定义编码的控制符" + L"\r\n\r\n" + 文字 , 提示文本 );
							delete[]文字;
							delete[]一行;
							return FALSE;
						}
						delete[]tmp;
					}
				}
				else
				{
					::CWD_BIN* BIN_tmp = m_MB[*一行_tmp];
					if(BIN_tmp)
					{
						for( i = BIN_tmp->LEN , s2 = BIN_tmp->BIN ; i ; --i )
							*(s1++) = *(s2++);

						BIN_OUT.LEN += BIN_tmp->LEN;
						++一行_tmp;
					}///////////////////// 未定义字符
					else
					{
						WCHAR 文字[] = {L'“',*一行_tmp,L'”',L'\0'};
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr  +  L"\r\n未定义编码的字符" + L"\r\n\r\n" + 文字 , 提示文本 );
						delete[]一行;
						return FALSE;
					}
				}
			}
			////长度验证
			if( BIN_OUT.LEN > 长度 )
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n\r\n长度超出原规定范围,忽略此行导入" , 提示文本 );
			//	switch( MessageBoxW( errStr + L"\r\n\r\n长度超出原规定范围,是否写到文件?\n\n (是) 写  (否) 跳过 (取消) 中断导入" , 提示文本 , MB_YESNOCANCEL ) )
				switch( IDNO )
				{
				case IDYES:
					break;
				case IDNO:
					delete[]一行;
					continue;
					break;
				case IDCANCEL:
					delete[]一行;
					return FALSE;
					break;
				default:
					zzz_Log( errStr + L"\r\n\r\n超长???" , 提示文本 );
					delete[]一行;
					return FALSE;
				}
			}
			else if(BIN_OUT.LEN < 长度)
			{
				if(m_是否提示长度不足)
				{
					CWQSG_strW errStr( 一行 );
					zzz_Log( errStr + L"\r\n\r\n长度不足" , 提示文本 );
					delete[]一行;
					return FALSE;
				}
				//填充
				switch(m_填充)
				{
				case 0://不填充
					break;
				case 1://单字节
					while( BIN_OUT.LEN < 长度 )
					{
						*(s1++) = m_单字节;
						++BIN_OUT.LEN;
					}
					break;
				case 2://双字节
					while( BIN_OUT.LEN < 长度 )
					{
						*(u16*)s1 = m_双字节;
						s1 += 2;
						BIN_OUT.LEN += 2;
					}
					break;
				case 3://单双字节
					k = 长度- BIN_OUT.LEN;
					while(k)
					{
						if( k >= 2 )
						{
							*(u16*)s1 = m_双字节;
							s1 += 2;
							BIN_OUT.LEN += 2;
							--k;
						}
						else
						{
							*(s1++) = m_单字节;
							++BIN_OUT.LEN;
						}
						--k;
					}
					break;
				default://未知
					{
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr + L"\r\n\r\n???" , 提示文本 );
					}
					delete[]一行;
					return FALSE;
				}
				if(BIN_OUT.LEN > 长度)
				{
					CWQSG_strW errStr( 一行 );
					zzz_Log( errStr + L"\r\n\r\n填充后长度超出原规定范围" , 提示文本 );
					delete[]一行;
					return FALSE;
				}
			}
			delete[]一行;
			//写出本行//
			地址|= 地址基址;
			if( !ROM_File.Seek(地址) )
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n设置文件指针错误" , 提示文本 );
				return FALSE;
			}
			if( BIN_OUT.LEN != ROM_File.Write(BIN_OUT.BIN,BIN_OUT.LEN) )
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n写出数据失败" , 提示文本 );
				return FALSE;
			}
		}
		////////////////////
		BIN_OUT.LEN = 0;
		return TRUE;
	}
public:
	WQSG_TXT_I( )
		:CWQSG_MapTbl_IN( )
	{
		BIN_OUT.BIN = new UCHAR[WQSG_TXT_BUF + 1];
		BIN_OUT.LEN = 0;
	}
	virtual	~WQSG_TXT_I()	{	}
	BOOL 载入码表( WCHAR const*const a_pPathName , WCHAR const*const TBL2_path , const BOOL a_bCheckTbl = FALSE )
	{
		m_bCheckTbl = a_bCheckTbl;
		ClearTbl();
		if(LOAD_TBL(a_pPathName))
		{
			if(TBL2_path && *TBL2_path)
			{
				if( LOAD_TBL2(TBL2_path) )
				{
					return TRUE;
				}
			}
			else
			{
				return TRUE;
			}
		}
		ClearTbl();
		return FALSE;
	}
	BOOL 导入文本( WCHAR const*const rom_path , WCHAR const*const TXT_path , u8 const*const _SP1 , u16 const*const _SP2 , const BOOL 长度不足提示 = FALSE )
	{
		m_是否提示长度不足= 长度不足提示;
		m_填充= 0;

		if( _SP1 )
		{
			m_填充+= 1;
			m_单字节 = *_SP1;
		}
		if( _SP2 )
		{
			m_填充+= 2;
			m_双字节 = *_SP2;
		}

		::CWQSG_File ROM_File;
		if( !ROM_File.OpenFile( rom_path , 2 , 3 ) )
		{
			zzz_Log( L"打开ROM文件" , rom_path );
			return FALSE;
		}

		::C内存文本_W  WQSG;
		if( !WQSG.Load( TXT_path , 67108864 ) )
		{
			zzz_Log(  WQSG.GeterrTXT() , TXT_path );
			return FALSE;
		}

		return 实际导入( ROM_File , WQSG , TXT_path );
	}
	void ClearTbl(void)
	{
		CWQSG_MapTbl_IN::ClearTbl();
	}
};
//----------------------------------------------------------------------------
class WQSG_pTXT_O:private CWQSG_MapTbl_OUT
{
	BOOL 读入指针表(WCHAR const*const ROM_path , WQSG_FM_FILE& mFile_ROM ,
		s64 首指针地址 ,u8 指针实长 ,u8 指针间隔 ,u8 指针倍率 ,u32 文本基础地址	)
	{
		s64 tmp64;
		CWQSG_TypeLinkList<s64> 链表数组;

		for(u32 i = 0;i < m_指针个数;i++)
		{
			if( !mFile_ROM.SeekTo( 首指针地址 ))
			{
				zzz_Log( L"未知错误1"  , ROM_path );
				return FALSE;
			}
			tmp64 = 0;
			if((UINT)指针实长 != mFile_ROM.GetUCHAR((UINT)指针实长,(const u8*)&tmp64))
			{
				zzz_Log( L"未知错误2"  , ROM_path );
				return FALSE;
			}
			tmp64 *= 指针倍率;
			tmp64 += 文本基础地址;

			if( 链表数组.AddItem(tmp64) < 0 )
			{
				zzz_Log( L"未知错误3"  , ROM_path );
				return FALSE;
			}

			首指针地址 += 指针间隔;
		}
		m_指向文本的地址 = 链表数组.MakeArray();
		if( NULL == m_指向文本的地址 )
		{
			zzz_Log( L"未知错误4"  , ROM_path );
			return FALSE;
		}
		return TRUE;
	}
	BOOL 地址_正式导出( WCHAR const*const ROM_path , CWQSG_File& a_fp , WQSG_FM_FILE& mFile_ROM )
	{
		u8 缓冲[1024];
		DWORD 长度;
		if( 2 != a_fp.Write("\377\376",2) )
		{
			zzz_Log( L"写出文本头失败" , ROM_path );
			return FALSE;
		}
		if( !a_fp.WriteStrW( BUF_OUT ) )
		{
				zzz_Log( L"写出信息头失败" , ROM_path );
			return FALSE;
		}
		///////////////////////////////////////////////////
		for( u32 指针号 = 0 ; 指针号 < m_指针个数 ; ++指针号 )
		{
			if( !mFile_ROM.SeekTo( m_指向文本的地址[指针号] ) )
			{
				::swprintf_s( BUF_OUT , WQSG_TXT_BUF,L"错误的指针地址\r\n%05u号,指向%08X位置,",指针号,m_指向文本的地址[指针号]);
				zzz_Log( BUF_OUT , ROM_path );
				return FALSE;
			}
			WCHAR* s1 = BUF_OUT;
			*BUF_OUT = 0;
			长度= 0;
			::swprintf_s(s1,13,L"%05u‖文本＝",指针号);
			s1 += 9;
			BOOL 不存在重复 = TRUE;
			s64 本文本地址 = m_指向文本的地址[指针号];
			for( UINT i = 0 ; i < 指针号; ++i )
			{
				if( 本文本地址 == m_指向文本的地址[i] )
				{
					WCHAR 重复TXT[20];
					不存在重复 = FALSE;
					::swprintf_s(重复TXT,19,L"{重复符：%05u}",i);
					s1 += ::WQSG_strcpy (重复TXT,s1);
					break;
				}
			}
			if(不存在重复)
			{
				while(TRUE)
				{
					////////////////////////
					UINT 实际III = mFile_ROM.GetUCHAR( (UINT)m_MAX_字节 , 缓冲 );
					if( 0 == 实际III )
						break;
					//////////////////////匹配
					WCHAR* tmp = NULL;//改成匹配

					UINT 索引长度;
					tmp = m_Text.匹配( 缓冲 , 实际III , 索引长度 );
					/////////////////
					if(tmp)
					{
						长度+= 索引长度;
						mFile_ROM.BACK(实际III - 索引长度);
						if(*((u64*)tmp) == m_OVER)
						{
							s1 += ::WQSG_strcpy(L"{结束符",s1);
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),03,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'}';
							break;
						}
						else if(*((u64*)tmp) == m_TURN)
						{
							s1 += ::WQSG_strcpy(L"{跳跃符",s1);
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),3,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'：';
							///////////////
							tmp = ::WQSG_getstrL(tmp + 4,-1);
							::swscanf_s(tmp,L"%d",&实际III);
							delete[]tmp;

							索引长度= mFile_ROM.GetUCHAR(实际III,缓冲);
							长度+= 索引长度;
							for(UINT i = 0;i < 索引长度;i++)
							{
								::swprintf_s((s1++),3,L"%02X",缓冲[i]);
								s1++;
							}
							*(s1++) = L'}';
						}
						else
						{
							s1 += ::WQSG_strcpy(tmp,s1);
						}
					}
					else
					{// 没找到
						mFile_ROM.BACK(--实际III);
						//未找到的异常处理
						*(s1++) = L'{';
						::swprintf_s(s1++,3,L"%02X",*缓冲);
						*(++s1) = L'}';
						s1++;
						长度++;
					}
				}
			}
			//写出文本
			::WQSG_strcpy (L"\r\n\r\n",s1);
			if( !a_fp.WriteStrW( BUF_OUT ) )
			{
				zzz_Log( L"写出文本失败" , ROM_path );
				return FALSE;
			}
		}
		return TRUE;
	}
	void 清空指针表()
	{
		delete[]m_指向文本的地址;
		m_指向文本的地址 = NULL;
		m_指针个数 = 0;
	}
private:
	BOOL		已载入码表;
	WCHAR*		BUF_OUT;
	///////////////////////////////////
	s64*		m_指向文本的地址;
	u32			m_指针个数;
	///////////////////////
public:
	WQSG_pTXT_O( )
		:CWQSG_MapTbl_OUT( )
		,m_指针个数(0)
		,已载入码表(FALSE)
		,m_指向文本的地址(NULL)
	{
		BUF_OUT = new WCHAR[WQSG_TXT_BUF + 1];
	}
	virtual	~WQSG_pTXT_O()
	{
		delete BUF_OUT;
	}
	void 清空码表(void)
	{
		m_MAX_字节 = 0;
		m_Text.清空();

		已载入码表 = FALSE;
	}
	BOOL 载入码表( WCHAR* a_pPathName , WCHAR* TBL2_path , int a_iCheckTbl = 0 , BOOL 需要结束符 = FALSE )
	{
		m_bCheckTbl = (a_iCheckTbl != 0);
		清空码表();
		if(LOAD_TBL(a_pPathName))
		{
			if(需要结束符)
			{
				if(*TBL2_path)
				{
					if(LOAD_TBL2(TBL2_path,FALSE))
					{
						已载入码表= TRUE;
						return TRUE;
					}
				}
				zzz_Log( L"此模式必须指定一个含有结束符的控制码表" );
			}
			else
			{
				已载入码表= TRUE;
				return TRUE;
			}
		}
		清空码表();
		return FALSE;
	}
	BOOL 地址_指针导出(WCHAR* rom_path,
		WCHAR * TXT_path,
		INT 验证,
		s64 首指针地址,
		s64 最后指针地址,
		u8 指针间隔,
		u8 指针实长,
		u32 文本基础地址= (u32)0,
		u8 指针倍率= (u8)1
		)
	{/////////////////////////////////////////////////////////////////
		if(!已载入码表)
		{
			zzz_Log( L"请先载入码表" , rom_path );
			return FALSE;
		}
		if( 首指针地址 < 0 )
		{
			zzz_Log( L"首指针地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
			return FALSE;
		}
		if( 最后指针地址 < 0 )
		{
			zzz_Log( L"最后指针地址 不能大于 0x7FFFFFFFFFFFFFFF"  , rom_path );
			return FALSE;
		}

		*BUF_OUT = 0;
		m_bCheckTbl= (验证!= 0);

		if(首指针地址 >= 最后指针地址)
		{
			zzz_Log( L"指针表结束地址 必须大于 指针表首项地址" , rom_path );
			return FALSE;
		}
		if(0 == 指针实长)
		{
			zzz_Log( L"指针实长不能为0" , rom_path );
			return FALSE;
		}
		if(指针实长> 8)
		{
			zzz_Log( L"指针实长不能超过8字节" , rom_path );
			return FALSE;
		}

		if(指针间隔< 指针实长)
		{
			zzz_Log( L"表项长度不能小于指针实长" , rom_path );
			return FALSE;
		}
		if(0 == 指针倍率)
		{
			zzz_Log( L"指针倍率不能为0" , rom_path );
			return FALSE;
		}
		u64 指针个数= (最后指针地址- 首指针地址) / 指针间隔 + 1;
		if(指针个数 > 99999)
		{
			zzz_Log( L"安全起见,指针个数不能超过 99999" , rom_path );
			return FALSE;
		}
////////////////////////////////////////////////////////////////
		::WQSG_FM_FILE mFile_ROM;
		if(!mFile_ROM.OpenFile(rom_path,3))
		{
			zzz_Log( L"打开ROM文件失败" , rom_path );
			return FALSE;
		}
		if(最后指针地址 >= mFile_ROM.Get文件长度())
		{
			zzz_Log( L"指针表结束地址不存在,超出文件大小" , rom_path );
			return FALSE;
		}

		清空指针表();
		m_指针个数= (u32)指针个数;
		///////////////////////////////////////////////////////////////
		::CWQSG_File fp;

		if( !fp.OpenFile( TXT_path , 4 , 3 ) )
		{
			zzz_Log( L"创建文本文件失败" , rom_path );
			return FALSE;
		}
		if( !读入指针表(rom_path , mFile_ROM , 首指针地址 , 指针实长 , 指针间隔 , 指针倍率 , 文本基础地址) )
			return FALSE;

		::swprintf_s(BUF_OUT,WQSG_TXT_BUF,L"‖‖‖‖‖01‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%02u‖‖‖‖‖%02u‖‖‖‖‖%02u‖‖‖‖‖\r\n\r\n\r\n",
			(u32)首指针地址 , m_指针个数 , 文本基础地址 ,指针间隔 , 指针实长 , 指针倍率 );

//		CHAR AAA[65535];	::sprintf(AAA,"‖‖‖‖‖01‖‖‖‖‖%08X‖‖‖‖‖%08X‖‖‖‖‖%08X",(U32)首指针地址,文本基础地址,文件头长度);
		if( 地址_正式导出( rom_path , fp , mFile_ROM ) )
		{
			清空指针表();
			zzz_Log( L"导出完毕" , rom_path );
			return TRUE;
		}
		清空指针表();
		return FALSE;
	}
	BOOL 地址_长度_指针导出(WCHAR* rom_path,
		WCHAR * TXT_path,
		INT 验证,
		s64 首指针地址,
		s64 最后指针地址,
		u8 指针间隔,
		u8 指针实长,
		u8 地址长度,
		u32 文本基础地址= (u32)0,
		u8 指针倍率= (u8)1,
		u8 长度块倍率= (u8)1,
		u8 长度块长度字节= (u8)2
		)
	{
		return FALSE;
	}
};
class WQSG_pTXT_I:private CWQSG_MapTbl_IN
{
	int m_文本模式;
	s64 m_首指针地址,m_界限地址;
	u32 m_指针总数,
		m_文本基础地址;
	u8 m_指针间隔,
		m_指针实长,
		m_指针倍率;

	CWD_BIN m_BIN_OUT;

	BOOL m_已载入码表;

	BOOL 实际导入( CWQSG_File& ROM_File ,C内存文本_W& WQSG,WCHAR const*const 提示文本)
	{
		s64 指针位置 , 位置 , I_64;
		WCHAR* tmp,
			* tmp2,
			* tmp3,
			* tmp4,
			* tmp5;
		UINT I,k;
		///////////////////////////
		while(WCHAR * const 一行 = WQSG.取一行文本())
		{
			tmp = ::WQSG_DelSP_L(一行);
			if(*tmp == L'\0')
			{
				delete[]一行;delete[]tmp;
				continue;
			}
			delete[]tmp;
			/////////////////////////
			I = ::WQSG_strstr(一行,L"‖文本＝");
			if(I == -1)
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n\r\n找不到 ‖文本＝" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			tmp = ::WQSG_getstrL(一行,I);
			tmp2 = ::WQSG_DelSP_ALL(tmp);
			delete[]tmp;

			tmp3 = 一行 + I + 4;
			///////////////////// tmp2 一行
			I = ::WQSG_strlen(tmp2);
			if((I > 5)
				|| (0 == I)
				|| (!::WQSG_是十进制文本(tmp2))
				)
			{
				delete[]tmp2;
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n\r\n错误的 指针编号" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			::swscanf_s(tmp2,L"%u",&I);
			delete[]tmp2;
			//////////////////////////// 一行
			if(I >= m_指针总数)
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n\r\n导出时不存在这个 指针编号" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			/////////////
			位置 = ROM_File.Tell();
			I_64 = (位置 - m_文本基础地址);// / m_指针倍率 * m_指针倍率;
			if((I_64 % m_指针倍率) != 0)
			{
				I_64 /= m_指针倍率;
				I_64++;
				位置 = I_64 * m_指针倍率;
			}
			else
			{
				I_64 /= m_指针倍率;
			}

			指针位置 = (I * m_指针间隔) + m_首指针地址;
			ROM_File.Seek(指针位置);
			ROM_File.Write(&I_64,m_指针实长);
			ROM_File.Seek(位置);

			/////////////////////////////////////
			m_BIN_OUT.LEN = 0;
			u8* s1 = m_BIN_OUT.BIN
				,* s2;
			while(*tmp3 != L'\0')
			{
				if(L'{' == *tmp3)
				{
					I = ::WQSG_strchr(tmp3,L'}');
					if(-1 == I)
					{
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr + L"\r\n\r\n文本格式错误,找不到控制符的右半边" , 提示文本 );
				//		m_MSG.show(提示文本,L"文本格式错误,找不到控制符的右半边",一行);	
						delete[]一行;
						return FALSE;
					}
					tmp = ::WQSG_getstrL(++tmp3,--I);
					tmp3 += I + 1;
					///////////////////////// 结束符tmp
					if(0 == ::WQSG_strstr(tmp,L"结束符"))
					{
						tmp2 = ::WQSG_DelSP_ALL(tmp + 3);
						/////////////////////////  tmp tmp2
						k = (UINT)::wcslen(tmp2);
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp2))//有非HEX字符
							)
						{
							delete[]tmp2;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n结束符后缀错误" , 提示文本 );
							delete[]tmp;
							delete[]一行;
							return FALSE;
						}
						delete[]tmp;
						//
						k >>= 1;
						m_BIN_OUT.LEN += k;
						tmp = tmp2;
						for(;k;k--)
						{
							::swscanf_s(tmp++,L"%2x",&I);
							tmp++,*(s1++) = (u8)I;
						}
						delete[]tmp2;
						//
					}//////////////////// 跳跃符
					else if(0 == ::WQSG_strstr(tmp,L"跳跃符"))
					{
						tmp2 = ::WQSG_DelSP_ALL(tmp + 3);
						I = ::WQSG_strchr(tmp2,L'：');
						// tmp  tmp2
						if(-1 == I)
						{
							delete[]tmp2;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n跳跃符错误,找不到：" , 提示文本 );
							delete[]tmp;
							delete[]一行;
							return FALSE;
						}
						tmp4 = ::WQSG_getstrL(tmp2,I);
						k = (UINT)::wcslen(tmp4);
						tmp5 = ::WQSG_getstrL(tmp2 + I + 1,-1);
						delete[]tmp2;
						tmp2 = tmp5;
						// tmp  tmp2 tmp4
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp4))//有非HEX字符
							)
						{
							delete[]tmp2;
							delete[]tmp4;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n跳跃符前缀格式错误" , 提示文本 );
							delete[]tmp;
							delete[]一行;
							return FALSE;
						}
						//
						k >>= 1;
						m_BIN_OUT.LEN += k;
						tmp5 = tmp4;
						for(;k;k--)
						{
							::swscanf_s(tmp5++,L"%2x",&I);
							tmp5++,*(s1++) = (u8)I;
						}
						delete[]tmp4;
						////////////// 后缀tmp tmp2
						k = (UINT)::wcslen(tmp2);
						if(!(k >> 1)//为或小于
							|| (k & 1)//单数
							|| (FALSE == ::WQSG_是十六进制文本(tmp2))//有非HEX字符
							)
						{
							delete[]tmp2;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n跳跃符后缀格式错误" , 提示文本 );
							delete[]tmp;
							delete[]一行;
							return FALSE;
						}
						delete[]tmp;
						//
						k >>= 1;
						m_BIN_OUT.LEN += k;
						tmp5 = tmp2;
						for(;k;k--)
						{
							::swscanf_s(tmp5++,L"%2x",&I);
							tmp5++,*(s1++) = (u8)I;
						}
						delete[]tmp2;
					}/////////// 自定义控制符
					else if(0 == ::WQSG_strstr(tmp,L"重复符："))
					{

						tmp2 = ::WQSG_getstrL(tmp + 4,-1);
						delete[]tmp;

						k = (UINT)::wcslen(tmp2);
						if((k < 0)//为或小于
							|| (k > 5)
							|| (FALSE == ::WQSG_是十进制文本(tmp2))//有非HEX字符
							)
						{
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n重复符：后缀格式错误" , 提示文本 );
							delete[]一行;
							delete[]tmp2;
							return FALSE;
						}
						I = 0;
						::swscanf_s(tmp2,L"%u",&I);
						delete[]tmp2;
						ROM_File.Seek((I * m_指针间隔) + m_首指针地址);
						I_64 = 0;
						ROM_File.Read(&I_64,m_指针实长);

						ROM_File.Seek(指针位置);
						ROM_File.Write(&I_64,m_指针实长);

						ROM_File.Seek(位置);
					}
					else
					{
						::CWD_BIN * BIN_tmp = m_MB2.索引((u8*)tmp,(int)(::wcslen(tmp)<<1));
						if(BIN_tmp)
						{
							for(I = BIN_tmp->LEN,s2 = BIN_tmp->BIN;I;I--)
								*(s1++) = *(s2++);

							m_BIN_OUT.LEN += BIN_tmp->LEN;
						}///////////////////// 未定义控制符
						else
						{
							k = ::WQSG_strlen(tmp) + 3;
							WCHAR * 文字= new WCHAR[k];
							*文字 = L'“';
							::WQSG_strcpy(tmp,文字 + 1);
							文字[k-2] = L'”';
							文字[k-1] = 0;
							delete[]tmp;
							CWQSG_strW errStr( 一行 );
							zzz_Log( errStr + L"\r\n\r\n未定义编码的控制符\r\n\r\n" + 文字 , 提示文本 );
							delete[]文字;
							delete[]一行;
							return FALSE;
						}
						delete[]tmp;
					}
				}
				else{
					::CWD_BIN * BIN_tmp = m_MB[*tmp3];
					if(BIN_tmp)
					{
						for(I = BIN_tmp->LEN,s2 = BIN_tmp->BIN;I;I--)
							*(s1++) = *(s2++);

						m_BIN_OUT.LEN += BIN_tmp->LEN;
						tmp3++;
					}///////////////////// 未定义字符
					else
					{
						WCHAR 文字[] = {L'“',*tmp3,L'”',L'\0'};
						CWQSG_strW errStr( 一行 );
						zzz_Log( errStr + L"\r\n\r\n未定义编码的字符\r\n\r\n" + 文字 , 提示文本 );
						delete[]一行;
						return FALSE;
					}
				}
			}
			//////////////////////////
			if((位置 + m_BIN_OUT.LEN) > (m_界限地址 + 1))
			{
				CWQSG_strW errStr( 一行 );
				zzz_Log( errStr + L"\r\n\r\n从此句开始超出界限地址" , 提示文本 );
				delete[]一行;
				return FALSE;
			}
			delete[]一行;
			ROM_File.Write(m_BIN_OUT.BIN,(u32)m_BIN_OUT.LEN);
		}
		////////////////////////////////
		m_BIN_OUT.LEN = 0;

		return TRUE;
	}
	BOOL 检测信息( WCHAR* tmp , WCHAR const*const 提示文本 )
	{
		UINT x1 = ::WQSG_strstr( tmp , L"‖‖‖‖‖" );
		if( -1 == x1 )
		{
			zzz_Log( L"找不到‖‖‖‖‖(1),不是WQSG指针文本" , 提示文本 );
			return FALSE;
		}
		if( 0 != x1 )
		{
			zzz_Log( L"‖‖‖‖‖格式错误" , 提示文本 );
			return FALSE;
		}
		tmp += (x1 + 5);
		///
		unsigned int x2;
		WCHAR* tmp2;
		///
#define	DEF_数据段VAL( __def_字段size , __def_进制检测 , __def_转换格式 , __def_VALptr , __def_提示1 , __def_提示2 ) \
	x1 = ::WQSG_strstr(tmp,L"‖‖‖‖‖");\
	if(-1 == x1)\
	{\
		zzz_Log( __def_提示1 , 提示文本 );\
		return FALSE;\
	}\
	tmp2 = ::WQSG_getstrL( tmp , x1 );\
	if( (::WQSG_strlen(tmp2) != __def_字段size) || (!::__def_进制检测(tmp2)))\
	{\
		delete tmp2;\
		zzz_Log( __def_提示2 , 提示文本 );\
		return FALSE;\
	}\
	::swscanf_s( tmp2 , __def_转换格式 , __def_VALptr );\
	delete tmp2;\
	tmp += (x1 + 5);
		///////////////////////////////////////////// 模式
		DEF_数据段VAL( 2 , WQSG_是十进制文本 , L"%u" , &x2 , L"找不到‖‖‖‖‖(2),不是WQSG指针文本" , L"文本模式,信息头已被损坏" )
		m_文本模式 = x2;
		/////////////////////////////////////////////// 首指针地址
		DEF_数据段VAL( 8 , WQSG_是十六进制文本 , L"%x" , &x2 , L"找不到‖‖‖‖‖(3),不是WQSG指针文本" , L"首指针地址,信息头已被损坏" )
		m_首指针地址 = x2;
		/////////////////////////////////////////////// 指针总数
		DEF_数据段VAL( 8 , WQSG_是十六进制文本 , L"%x" , &x2 , L"找不到‖‖‖‖‖(4),不是WQSG指针文本" , L"指针总数,信息头已被损坏" )
		m_指针总数 = x2;
		/////////////////////////////////////////////////////// 文本基础地址
		DEF_数据段VAL( 8 , WQSG_是十六进制文本 , L"%x" , &x2 , L"找不到‖‖‖‖‖(5),不是WQSG指针文本" , L"文本基础地址,信息头已被损坏" );
		m_文本基础地址 = x2;
		///////////////////////////////////////////////////// 文件头长度
/*		x1 = ::WQSG_strstr(tmp,L"‖‖‖‖‖");
		if(-1 == x1){
			m_MSG.show(提示文本,L"找不到‖‖‖‖‖(6),不是WQSG指针文本");
			return FALSE;
		}
		tmp2 = ::WQSG_getstrL(tmp,x1);
		if((::WQSG_strlen(tmp2) != 8)
			|| (!::WQSG_是十六进制文本(tmp2))){
				delete tmp2;
				m_MSG.show(提示文本,L"文件头长度,信息头已被损坏");
				return FALSE;
		}
		::swscanf_s(tmp2,L"%x",&x2);
		m_文件头长度 = x2;
		delete tmp2;
		tmp += (x1 + 5);
		*/
		///////////////////////////////////// m_指针间隔,
		DEF_数据段VAL( 2 , WQSG_是十进制文本 , L"%u" , &x2 , L"找不到‖‖‖‖‖(7),不是WQSG指针文本" , L"指针间隔,信息头已被损坏" )
		m_指针间隔 = (u8)x2;
		//////////////////////////////////// m_指针实长,
		DEF_数据段VAL( 2 , WQSG_是十进制文本 , L"%u" , &x2 , L"找不到‖‖‖‖‖(8),不是WQSG指针文本" , L"指针实长,信息头已被损坏" )
		m_指针实长 = (u8)x2;
		////////////////////////////////////////// m_指针倍率;
		DEF_数据段VAL( 2 , WQSG_是十进制文本 , L"%u" , &x2 , L"找不到‖‖‖‖‖(9),不是WQSG指针文本" , L"指针倍率,信息头已被损坏" )
		m_指针倍率 = (u8)x2;
		//////////////////////////////////
		switch(m_文本模式)
		{
		case 1:
			return TRUE;
			break;
		case 2:
			return FALSE;
			break;
		default:
			return FALSE;
		}
#undef	DEF_数据段VAL
	}
public:
	WQSG_pTXT_I( )
		:CWQSG_MapTbl_IN( )
		,m_已载入码表(FALSE)
	{
		m_BIN_OUT.BIN = new UCHAR[WQSG_TXT_BUF + 1];
		m_BIN_OUT.LEN = 0;
	}
	virtual	~WQSG_pTXT_I(){}
	void 清空码表(void)
	{
		m_已载入码表 = FALSE;
		for(int i = 0;i < 65536;i++)
			delete m_MB[i];

		::memset(m_MB,0,sizeof(m_MB));
		m_MB2.清空();
	}
	BOOL 载入码表(WCHAR * a_pPathName,WCHAR * TBL2_path,int 验证 = 0)
	{
		m_bCheckTbl = (验证 != 0);
		清空码表();
		if(LOAD_TBL(a_pPathName))
		{
			if(*TBL2_path != L'\0')
			{
				if(LOAD_TBL2(TBL2_path))
				{
					m_已载入码表 = TRUE;
					return TRUE;
				}
			}
			else
			{
				m_已载入码表 = TRUE;
				return TRUE;
			}
		}
		清空码表();
		return FALSE;
	}
	BOOL 导入文本(WCHAR* rom_path,WCHAR* TXT_path,u64 文本区首地址,u64 文本区界限地址)
	{
		if(!m_已载入码表)
		{
			zzz_Log( L"未载入码表" , rom_path );
			return FALSE;
		}
		/////////////
		if(文本区界限地址 < 文本区首地址)
		{
			zzz_Log( L"界限地址 小于 开始地址" , rom_path );
			return FALSE;
		}
		m_界限地址 = 文本区界限地址;
		::CWQSG_File ROM_File;
		if(! ROM_File.OpenFile(rom_path,3,3))
		{
			zzz_Log( L"打开ROM文件失败" , rom_path );
			return FALSE;
		}
		/////////////////////
		::C内存文本_W  WQSG;
		if(!WQSG.Load(TXT_path,(DWORD)33554432))
		{
			zzz_Log( WQSG.GeterrTXT() , TXT_path );
			return FALSE;
		}
		////////////////////
		WCHAR* tmp = WQSG.取一行文本();
		if(tmp == NULL)
		{
			zzz_Log( L"空文本,不是WQSG指针文本" , TXT_path );
			return FALSE;
		}
		if(!检测信息(tmp,TXT_path))
		{
			delete[]tmp;
			return FALSE;
		}
		delete[]tmp;
		//////////////////////////
		ROM_File.Seek(文本区首地址);
		return 实际导入(ROM_File,WQSG,TXT_path);
	}
};
#undef WQSG_TXT_BUF
//----------------------------------------------------------------------------

#endif
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
#ifndef __WQSG_TXT_Import_H__
#define __WQSG_TXT_Import_H__

#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#include<WQSG_DEF.h>
#include<WQSG.h>
#include<WQSG_File_M.h>

#include "MapTbl_Base.h"
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
#define WQSG_TXT_BUF 10485760

namespace WQSG_TEXT_OUT
{

}
//============================================================================
class CWQSG_MapTbl_IN :public CWQSG_MapTbl_Base
{
	BOOL m_b禁止码表错误;
	inline BOOL Add_TBL( CMemTextW& a_tp , const WCHAR*const a_pPathName );
	inline BOOL Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName );
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

BOOL CWQSG_MapTbl_IN::Add_TBL( CMemTextW& a_tp , WCHAR const*const a_pPathName )
{
	while( WCHAR* WTXT_ALL = a_tp.GetLine() )
	{
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
BOOL CWQSG_MapTbl_IN::Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName )
{
	while( WCHAR*WTXT_ALL = a_tp.GetLine() )
	{
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
		const CWD_BIN *TMP_BIN = m_MB2.索引((UCHAR*)WTXT_R,I<<1);
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
	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		return FALSE;
	}

	return Add_TBL( tp , a_pPathName );
}

BOOL CWQSG_MapTbl_IN::LOAD_TBL2( WCHAR const*const a_pPathName )
{
	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		return FALSE;
	}

	return Add_TBL2( tp , a_pPathName );
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
class WQSG_TXT_I:private CWQSG_MapTbl_IN
{
	CWD_BIN		BIN_OUT;
	BOOL		m_是否提示长度不足;
	INT			m_填充;
	u8			m_单字节;
	u16			m_双字节;

	BOOL 实际导入( CWQSG_File& ROM_File , CMemTextW& WQSG , WCHAR const*const 提示文本 )
	{
		s64 地址,地址基址 = 0;
		UINT 长度;
		BOOL 找标记= TRUE;
		while( WCHAR*const 一行 = WQSG.GetLine() )
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
						const ::CWD_BIN * BIN_tmp = m_MB2.索引((u8*)tmp,(int)(::wcslen(tmp)<<1));
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

		::CMemTextW  WQSG;
		if( !WQSG.Load( TXT_path , 67108864 ) )
		{
			zzz_Log(  WQSG.GetErrTXT() , TXT_path );
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

	BOOL 实际导入( CWQSG_File& ROM_File ,CMemTextW& WQSG,WCHAR const*const 提示文本)
	{
		s64 指针位置 , 位置 , I_64;
		WCHAR* tmp,
			* tmp2,
			* tmp3,
			* tmp4,
			* tmp5;
		UINT I,k;
		///////////////////////////
		while(WCHAR * const 一行 = WQSG.GetLine())
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
						const ::CWD_BIN * BIN_tmp = m_MB2.索引((u8*)tmp,(int)(::wcslen(tmp)<<1));
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
		::CMemTextW  WQSG;
		if(!WQSG.Load(TXT_path,(DWORD)33554432))
		{
			zzz_Log( WQSG.GetErrTXT() , TXT_path );
			return FALSE;
		}
		////////////////////
		WCHAR* tmp = WQSG.GetLine();
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
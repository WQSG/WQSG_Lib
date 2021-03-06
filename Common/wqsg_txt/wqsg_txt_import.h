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

#include "../../Interface/wqsg_def.h"
// #include <WQSG.h>
// #include <WQSG_File_M.h>
#include <algorithm>
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
	BOOL m_bTblErrStop;//禁止码表错误;
	inline BOOL Add_TBL( const CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR , const WCHAR*const a_pPathName );
	inline BOOL Add_TBL2( const CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR , const WCHAR*const a_pPathName );

	inline BOOL Add_TblX( CMemTextW& a_tp , const WCHAR*const a_pPathName , BOOL a_b1 );
protected:
	BOOL m_bCheckTbl;
public:
	inline BOOL	LOAD_TBL( const WCHAR*const a_pPathName , BOOL a_bCheckTbl );
	inline BOOL	LOAD_TBL2( const WCHAR*const a_pPathName , BOOL a_bCheckTbl );
	inline void ClearTbl(void);
	inline CWQSG_MapTbl_IN();
	virtual inline ~CWQSG_MapTbl_IN();

	CByteTree< CWD_BIN >	m_MB2;
	std::vector<CWD_BIN*> m_BinDatas;
	CWD_BIN* m_MB[65536];
	const u64 m_OVER , m_TURN;
};

inline BOOL CWQSG_MapTbl_IN::Add_TblX( CMemTextW& a_tp , const WCHAR*const a_pPathName , BOOL a_b1 )
{
	CStringW strLine;
	CStringW strL;
	CStringW strR;

	while( const WCHAR* _line = a_tp.GetLine() )
	{
		strLine = _line;
		delete[]_line;
		strLine.TrimLeft();
		///判断空行 WTXT_ALL
		if( strLine.GetLength() == 0 || strLine.Find( L"//" ) == 0 )
			continue;

		const int iPos = strLine.Find( L'=' );
		if( -1 == iPos )//错误的码表格式,找不到"="
		{
			zzz_Log( strLine + L"\r\n错误的码表格式" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		strL = strLine.Left( iPos );
		strL.TrimRight();

		if( !::WQSG_IsHexText( strL.GetString() )//不是HEX
			|| (strL.GetLength() & 1)//单数
			|| !(strL.GetLength() >> 1)//小于2
			)
		{
			zzz_Log( strLine + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		strR = strLine.Mid( iPos + 1 );

		if( a_b1 )
		{
			if( !Add_TBL( strLine , strL , strR , a_pPathName ) )
				return FALSE;
		}
		else
		{
			if( !Add_TBL2( strLine , strL , strR , a_pPathName ) )
				return FALSE;
		}
	}

	return TRUE;
}

inline BOOL CWQSG_MapTbl_IN::Add_TBL( const CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR , const WCHAR*const a_pPathName )
{
	if( 1 != a_strR.GetLength() )//右边不是一个字,或者没有
	{
		zzz_Log( a_strLine + L"\r\n码表右边必须是一个字符" , a_pPathName );
		if( m_bTblErrStop )
			return FALSE;

		return TRUE;
	}
	////////////////////////////////////////////////////////////////////////////////// 加入树
	CWD_BIN* pBin = m_MB[a_strR[0]];
	if( pBin )
	{
		if(m_bCheckTbl)
		{
			zzz_Log( a_strLine + L"\r\n码表重复" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;
		}
		delete pBin;
	}

	pBin = new CWD_BIN;
	pBin->LEN = a_strL.GetLength() >> 1;
	pBin->BIN = new UCHAR[pBin->LEN];
	UCHAR *s1 = pBin->BIN;

	const WCHAR* pTmp = a_strL.GetString();
	for(UINT i = 0 , K ; i < pBin->LEN ; i++ )
	{
		::swscanf_s( pTmp++ , L"%02X" , &K );
		*(s1++) = (UCHAR)K;
		pTmp++;
	}

	m_MB[a_strR[0]] = pBin;

	return TRUE;
}
inline BOOL CWQSG_MapTbl_IN::Add_TBL2( const CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR , const WCHAR*const a_pPathName )
{
	if( 0 == a_strR.GetLength() )//右边不是一个字,或者没有
	{
		zzz_Log( a_strLine + L"\r\n码表右边必须至少有一个字符" , a_pPathName );
		if( m_bTblErrStop )
			return FALSE;

		return TRUE;
	}

	if( a_strR.GetLength() >= 4 )
	{
		CStringW strTmp = a_strR;
		strTmp.MakeUpper();

		if( ( *((u64*)L"OVER") == *((u64*)strTmp.GetString()) )
			|| ( *((u64*)L"TURN") == *((u64*)strTmp.GetString()) ) )
		{
			return TRUE;
		}
	}
	////////////////////////////////////////////////////////////////////////////////// 加入树
	CWD_BIN* pBin = m_MB2.get( (UCHAR*)a_strR.GetString() , a_strR.GetLength()<<1 );
	if( pBin )
	{
		if(m_bCheckTbl)
		{
			zzz_Log( a_strLine + L"\r\n码表重复" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;
		}

		m_BinDatas.erase( find( m_BinDatas.begin() , m_BinDatas.end() , pBin ) );
		delete pBin;
	}

	pBin = new CWD_BIN;
	pBin->LEN = a_strL.GetLength() >> 1;
	pBin->BIN = new UCHAR[pBin->LEN];
	UCHAR *s1 = pBin->BIN;

	const WCHAR* pTmp = a_strL.GetString();
	for(UINT i = 0 , K ; i < pBin->LEN ; i++ )
	{
		::swscanf_s( pTmp++ , L"%02X" , &K );
		*(s1++) = (UCHAR)K;
		pTmp++;
	}

	m_BinDatas.push_back(pBin);
	m_MB2.add( (UCHAR*)a_strR.GetString() , (a_strR.GetLength()<<1) , pBin );

	return TRUE;
}

inline BOOL CWQSG_MapTbl_IN::LOAD_TBL( const WCHAR*const a_pPathName , BOOL a_bCheckTbl )
{
	m_bCheckTbl = a_bCheckTbl;
	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		return FALSE;
	}

	return Add_TblX( tp , a_pPathName , TRUE );
}

inline BOOL CWQSG_MapTbl_IN::LOAD_TBL2( const WCHAR*const a_pPathName , BOOL a_bCheckTbl )
{
	m_bCheckTbl = a_bCheckTbl;
	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		return FALSE;
	}

	return Add_TblX( tp , a_pPathName , FALSE );
}

inline void CWQSG_MapTbl_IN::ClearTbl(void)
{
	for(int i = 0;i < 65536;++i)
		delete m_MB[i];

	::memset(m_MB,0,sizeof(m_MB));
	m_MB2.clear();

	std::vector<CWD_BIN*>::iterator it = m_BinDatas.begin();
	for( ; it != m_BinDatas.end() ; ++it )
	{
		delete (*it);
	}
	m_BinDatas.clear();
}

inline CWQSG_MapTbl_IN::CWQSG_MapTbl_IN( )
	: CWQSG_MapTbl_Base( )
	, m_bTblErrStop(TRUE)
	, m_OVER( *((ULONGLONG*)L"结束符") )
	, m_TURN( *((ULONGLONG*)L"跳跃符") )
{
	::memset( m_MB , 0 , sizeof(m_MB) );
}

inline CWQSG_MapTbl_IN::~CWQSG_MapTbl_IN()
{
	for( INT k = 0 ; k <= 65535 ; ++k)
		delete m_MB[k];
}
//----------------------------------------------------------------------------
class CWQSG_TxtImport
{
	CWQSG_memFile m_binOut;
	BOOL m_是否提示长度不足;
	INT m_FillMode;
	u8 m_FillByte;
	u16 m_FillWord;

	CWQSG_MapTbl_IN m_Tbl;

	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL )
	{
		CWQSG_MapTbl_IN::zzz_Log( a_szText , a_szTitle );
	}

	BOOL Import( CWQSG_File& a_ROM_File , CMemTextW& WQSG , WCHAR const*const 提示文本 )
	{
		s64 nAddr;
		UINT uLen;

		s64 nPosHigh = 0;
		BOOL bFindHead = TRUE;

		CStringW strLine;
		CStringW strTmp;

		INT k = 0;

		while( const WCHAR*const _line = WQSG.GetLine() )
		{
			strLine = _line;
			delete[]_line;
			//判断空行
			strLine.TrimLeft();
			if( strLine.GetLength() == 0 )
				continue;

			if(bFindHead)
			{
				bFindHead= FALSE;

				int iPos = strLine.Find( L".WQSG:" );
				if( 0 == iPos )
				{
					CStringW str = strLine.Mid( iPos + 6 );
					str.TrimLeft();
					str.TrimRight();

					if( str.GetLength() != 8 )
					{
						zzz_Log( strLine + L"\r\n文本格式错误,信息头后缀\".WQSG:XXXXXXXX\",\nXXXXXXXX必须是十六进制文本(HEX)" , 提示文本 );
						return FALSE;
					}

					k = 0;
					if( !::WQSG_IsHexText(str.GetString())
						|| 1 != ::swscanf_s( str.GetString() , L"%X" , &k )
						)
					{
						zzz_Log( strLine + L"\r\n文本格式错误,信息头后缀\".WQSG:XXXXXXXX\",\nXXXXXXXX不是合法的十六进制文本(HEX)" , 提示文本 );
						return FALSE;
					}

					nPosHigh = k;
					nPosHigh <<= 32;
					continue;
				}
			}
			//注释
			const int iPos0 = strLine.Find( L"//" );
			if( 0 == iPos0 )
				continue;

			//寻找第一个逗号
			const int iPos1 = strLine.Find( L',' );
			if( -1 == iPos1 )
			{
				zzz_Log( strLine + L"\r\n文本格式错误,找不到第一个逗号" , 提示文本 );
				return FALSE;
			}

			const int iPos2 = strLine.Find( L',' , iPos1 + 1 );
			if( -1 == iPos2 )
			{
				zzz_Log( strLine + L"\r\n文本格式错误,找不到第二个逗号" , 提示文本 );
				return FALSE;
			}

			////////////////////////
			strTmp = strLine.Left( iPos1 );
			//strTmp.TrimLeft();
			strTmp.TrimRight();

			if( 0 == strTmp.GetLength() || strTmp.GetLength() > 8 )
			{
				zzz_Log( strLine + L"\r\n文本格式错误,地址错误(1)" , 提示文本 );
				return FALSE;
			}
			k = 0;
			if(!::WQSG_IsHexText(strTmp.GetString())
				|| 0 == ::swscanf_s( strTmp.GetString() , L"%X" , &k )
				)
			{
				zzz_Log( strLine + L"\r\n文本格式错误,地址错误(1),\r\n不是合法的十六进制数字" , 提示文本 );
				return FALSE;
			}
			nAddr = k;
			////////////////////////
			strTmp = strLine.Mid( iPos1 + 1 , iPos2 - iPos1 - 1 );
			strTmp.TrimLeft();
			strTmp.TrimRight();

			if( 0 == strTmp.GetLength() || strTmp.GetLength() > 5 )
			{
				zzz_Log( strLine + L"\r\n文本格式错误,长度错误(2)\r\n 必须0位> 十进制数<= 5位" , 提示文本 );
				return FALSE;
			}

			k = 0;
			if(!::WQSG_IsDecText(strTmp.GetString())
				|| 0 == ::swscanf_s( strTmp.GetString() , L"%u" , &k )
				)
			{
				zzz_Log( strLine + L"\r\n文本格式错误,长度错误(3), 不是合法的十进制数字" , 提示文本 );
				return FALSE;
			}

			uLen = k;
			/////////////////////////////////////
			const WCHAR* pText = strLine.GetString() + iPos2 + 1;

			m_binOut.Seek(0);

			while( *pText )
			{
				if( L'{' == *pText )
				{
					++pText;
					int iPos3 = ::WQSG_strchr( pText , L'}' );
					if( -1 == iPos3 )
					{
						zzz_Log( strLine + L"\r\n文本格式错误,找不到控制符的右半边" , 提示文本 );
						return FALSE;
					}
					strTmp.SetString( pText , iPos3 );
					pText += (iPos3 + 1);
					///////////////////////// 结束符tmp
					if( strTmp.Find( L"结束符" ) == 0 )
					{
						CStringW strHex( strTmp.Mid(3) );
						/////////////////////////  tmp tmp2
						if( !(strHex.GetLength() >> 1)//为或小于
							|| (strHex.GetLength() & 1)//单数
							|| (FALSE == ::WQSG_IsHexText(strHex.GetString()))//有非HEX字符
							)
						{
							zzz_Log( strLine + L"\r\n结束符后缀错误" , 提示文本 );
							return FALSE;
						}
						//
						while( strHex.GetLength() )
						{
							k = 0;
							::swscanf_s( strHex.GetString() , L"%2x" , &k );
							strHex.Delete( 0 , 2 );
							m_binOut.Write( &k , 1 );
						}
					}//////////////////// 跳跃符
					else if( strTmp.Find( L"跳跃符" ) == 0 )
					{
						CStringW strHex2( strTmp.Mid(3) );
						iPos3 = strHex2.Find( L'：' );

						if( -1 == iPos3 )
						{
							zzz_Log( strLine + L"\r\n跳跃符错误,找不到：" , 提示文本 );
							return FALSE;
						}

						CStringW strHex1( strHex2.Left(iPos3) );
						strHex2.Delete( 0 , iPos3 + 1 );

						strHex1.TrimLeft();
						strHex1.TrimRight();
						strHex2.TrimLeft();
						strHex2.TrimRight();

						if(!(strHex1.GetLength() >> 1)//为或小于
							|| (strHex1.GetLength() & 1)//单数
							|| (FALSE == ::WQSG_IsHexText(strHex1.GetString()))//有非HEX字符
							)
						{
							zzz_Log( strLine + L"\r\n跳跃符前缀格式错误" , 提示文本 );
							return FALSE;
						}

						if(!(strHex2.GetLength() >> 1)//为或小于
							|| (strHex2.GetLength() & 1)//单数
							|| (FALSE == ::WQSG_IsHexText(strHex2.GetString()))//有非HEX字符
							)
						{
							zzz_Log( strLine + L"\r\n跳跃符后缀格式错误" , 提示文本 );
							return FALSE;
						}
						//
						while( strHex1.GetLength() )
						{
							k = 0;
							::swscanf_s( strHex1.GetString() , L"%2x" , &k );
							strHex1.Delete( 0 , 2 );
							m_binOut.Write( &k , 1 );
						}

						while( strHex2.GetLength() )
						{
							k = 0;
							::swscanf_s( strHex2.GetString() , L"%2x" , &k );
							strHex2.Delete( 0 , 2 );
							m_binOut.Write( &k , 1 );
						}
					}/////////// 自定义控制符
					else
					{
						const ::CWD_BIN* pBinTmp = m_Tbl.m_MB2.get( (u8*)strTmp.GetString() , (int)strTmp.GetLength()<<1 );
						if(pBinTmp)
						{
							m_binOut.Write( pBinTmp->BIN , pBinTmp->LEN );
						}///////////////////// 未定义控制符
						else
						{
							zzz_Log( strLine  +  L"\r\n未定义编码的控制符\r\n\r\n"L"“" + strTmp + L"”" , 提示文本 );
							return FALSE;
						}
					}
				}
				else
				{
					const ::CWD_BIN* pBinTmp = m_Tbl.m_MB[*pText];
					if(pBinTmp)
					{
						m_binOut.Write( pBinTmp->BIN , pBinTmp->LEN );

						++pText;
					}///////////////////// 未定义字符
					else
					{
						WCHAR szMsg[] = {L'“' , *pText , L'”' , L'\0' };
						zzz_Log( strLine  +  L"\r\n未定义编码的字符" + L"\r\n\r\n" + szMsg , 提示文本 );
						return FALSE;
					}
				}
			}
			////长度验证
			if( m_binOut.Tell() > uLen )
			{
				CStringW str;
				str.Format( L"%ls\r\n\r\n长度超出原规定范围,忽略此行导入.(超出%d字节)" , strLine.GetString() , m_binOut.Tell() - uLen );
				zzz_Log( str , 提示文本 );
			//	switch( MessageBoxW( errStr + L"\r\n\r\n长度超出原规定范围,是否写到文件?\n\n (是) 写  (否) 跳过 (取消) 中断导入" , 提示文本 , MB_YESNOCANCEL ) )
				switch( IDNO )
				{
				case IDYES:
					break;
				case IDNO:
					continue;
					break;
				case IDCANCEL:
					return FALSE;
					break;
				default:
					zzz_Log( strLine + L"\r\n\r\n超长???" , 提示文本 );
					return FALSE;
				}
			}
			else if( m_binOut.Tell() < uLen )
			{
				if(m_是否提示长度不足)
				{
					zzz_Log( strLine + L"\r\n\r\n长度不足" , 提示文本 );
					return FALSE;
				}
				//填充
				switch(m_FillMode)
				{
				case 0://不填充
					break;
				case 1://单字节
					while( m_binOut.Tell() < uLen )
					{
						m_binOut.Write( &m_FillByte , 1 );
					}
					break;
				case 2://双字节
					while( m_binOut.Tell() < uLen )
					{
						m_binOut.Write( &m_FillWord , 2 );
					}
					break;
				case 3://单双字节
					k = uLen - (u32)m_binOut.Tell();
					while(k)
					{
						if( k >= 2 )
						{
							m_binOut.Write( &m_FillWord , 2 );
							k -= 2;
						}
						else
						{
							m_binOut.Write( &m_FillByte , 1 );
							--k;
						}
					}
					break;
				default://未知
					zzz_Log( strLine + L"\r\n\r\n???" , 提示文本 );
					return FALSE;
				}
				if( m_binOut.Tell() > uLen )
				{
					zzz_Log( strLine + L"\r\n\r\n填充后长度超出原规定范围" , 提示文本 );
					return FALSE;
				}
			}
			//写出本行//
			nAddr |= nPosHigh;

			if( !a_ROM_File.Seek( nAddr ) )
			{
				zzz_Log( strLine + L"\r\n设置文件指针错误" , 提示文本 );
				return FALSE;
			}
			if( (u32)m_binOut.Tell() != a_ROM_File.Write( m_binOut.GetBuf() , (u32)m_binOut.Tell() ) )
			{
				zzz_Log( strLine + L"\r\n写出数据失败" , 提示文本 );
				return FALSE;
			}
		}

		return TRUE;
	}
public:
	CWQSG_TxtImport()
	{
		m_binOut.SetInc( WQSG_TXT_BUF );
	}
	virtual	~CWQSG_TxtImport(){}
	BOOL LoadTbl( const WCHAR*const a_pPathName , const WCHAR*const TBL2_path , const BOOL a_bCheckTbl = FALSE )
	{
		ClearTbl();
		if(m_Tbl.LOAD_TBL(a_pPathName,a_bCheckTbl))
		{
			if(TBL2_path && *TBL2_path)
			{
				if( m_Tbl.LOAD_TBL2(TBL2_path,a_bCheckTbl) )
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
	BOOL ImportText( const WCHAR*const a_pRomPath , const WCHAR*const a_pTxtPath , const u8*const a_pFillByte , const u16*const a_pFillWord , const BOOL 长度不足提示 = FALSE )
	{
		m_是否提示长度不足= 长度不足提示;
		m_FillMode = 0;
		if( a_pFillByte )
		{
			m_FillMode += 1;
			m_FillByte = *a_pFillByte;
		}
		if( a_pFillWord )
		{
			m_FillMode += 2;
			m_FillWord = *a_pFillWord;
		}

		::CWQSG_File ROM_File;
		if( !ROM_File.OpenFile( a_pRomPath , 2 , 3 ) )
		{
			zzz_Log( L"打开ROM文件" , a_pRomPath );
			return FALSE;
		}

		::CMemTextW  WQSG;
		if( !WQSG.Load( a_pTxtPath , 67108864 ) )
		{
			zzz_Log( WQSG.GetErrTXT() , a_pTxtPath );
			return FALSE;
		}

		return Import( ROM_File , WQSG , a_pTxtPath );
	}
	void ClearTbl(void)
	{
		m_Tbl.ClearTbl();
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

	BOOL Import( CWQSG_File& ROM_File ,CMemTextW& WQSG,WCHAR const*const 提示文本)
	{
		s64 指针位置 , 位置 , I_64;
		WCHAR* tmp,
			* tmp2;
		const WCHAR * tmp3;
		WCHAR * tmp4,
			* tmp5;
		UINT I,k;
		///////////////////////////
		while( const WCHAR * const 一行 = WQSG.GetLine())
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
				|| (!::WQSG_IsDecText(tmp2))
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
							|| (FALSE == ::WQSG_IsHexText(tmp2))//有非HEX字符
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
							|| (FALSE == ::WQSG_IsHexText(tmp4))//有非HEX字符
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
							|| (FALSE == ::WQSG_IsHexText(tmp2))//有非HEX字符
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
							|| (FALSE == ::WQSG_IsDecText(tmp2))//有非HEX字符
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
						const ::CWD_BIN * BIN_tmp = m_MB2.get((u8*)tmp,(int)(::wcslen(tmp)<<1));
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
	BOOL 检测信息( const WCHAR* tmp , WCHAR const*const 提示文本 )
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
		DEF_数据段VAL( 2 , WQSG_IsDecText , L"%u" , &x2 , L"找不到‖‖‖‖‖(2),不是WQSG指针文本" , L"文本模式,信息头已被损坏" )
			m_文本模式 = x2;
		/////////////////////////////////////////////// 首指针地址
		DEF_数据段VAL( 8 , WQSG_IsHexText , L"%x" , &x2 , L"找不到‖‖‖‖‖(3),不是WQSG指针文本" , L"首指针地址,信息头已被损坏" )
			m_首指针地址 = x2;
		/////////////////////////////////////////////// 指针总数
		DEF_数据段VAL( 8 , WQSG_IsHexText , L"%x" , &x2 , L"找不到‖‖‖‖‖(4),不是WQSG指针文本" , L"指针总数,信息头已被损坏" )
			m_指针总数 = x2;
		/////////////////////////////////////////////////////// 文本基础地址
		DEF_数据段VAL( 8 , WQSG_IsHexText , L"%x" , &x2 , L"找不到‖‖‖‖‖(5),不是WQSG指针文本" , L"文本基础地址,信息头已被损坏" );
		m_文本基础地址 = x2;
		///////////////////////////////////////////////////// 文件头长度
		/*		x1 = ::WQSG_strstr(tmp,L"‖‖‖‖‖");
		if(-1 == x1){
		m_MSG.show(提示文本,L"找不到‖‖‖‖‖(6),不是WQSG指针文本");
		return FALSE;
		}
		tmp2 = ::WQSG_getstrL(tmp,x1);
		if((::WQSG_strlen(tmp2) != 8)
		|| (!::WQSG_IsHexText(tmp2))){
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
		DEF_数据段VAL( 2 , WQSG_IsDecText , L"%u" , &x2 , L"找不到‖‖‖‖‖(7),不是WQSG指针文本" , L"指针间隔,信息头已被损坏" )
			m_指针间隔 = (u8)x2;
		//////////////////////////////////// m_指针实长,
		DEF_数据段VAL( 2 , WQSG_IsDecText , L"%u" , &x2 , L"找不到‖‖‖‖‖(8),不是WQSG指针文本" , L"指针实长,信息头已被损坏" )
			m_指针实长 = (u8)x2;
		////////////////////////////////////////// m_指针倍率;
		DEF_数据段VAL( 2 , WQSG_IsDecText , L"%u" , &x2 , L"找不到‖‖‖‖‖(9),不是WQSG指针文本" , L"指针倍率,信息头已被损坏" )
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
		: CWQSG_MapTbl_IN( )
		, m_已载入码表(FALSE)
	{
		m_BIN_OUT.BIN = new UCHAR[WQSG_TXT_BUF + 1];
		m_BIN_OUT.LEN = 0;
	}
	virtual	~WQSG_pTXT_I(){}
	void CearTbl(void)
	{
		m_已载入码表 = FALSE;
		for(int i = 0;i < 65536;i++)
			delete m_MB[i];

		::memset(m_MB,0,sizeof(m_MB));
		m_MB2.clear();
	}
	BOOL LoadTbl(WCHAR * a_pPathName,WCHAR * TBL2_path,int 验证 = 0)
	{
		m_bCheckTbl = (验证 != 0);
		CearTbl();
		if(LOAD_TBL(a_pPathName,m_bCheckTbl))
		{
			if(*TBL2_path != L'\0')
			{
				if(LOAD_TBL2(TBL2_path,m_bCheckTbl))
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
		CearTbl();
		return FALSE;
	}
	BOOL ImportText(WCHAR* rom_path,WCHAR* TXT_path,u64 文本区首地址,u64 文本区界限地址)
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
		const WCHAR* tmp = WQSG.GetLine();
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
		return Import(ROM_File,WQSG,TXT_path);
	}
};

#undef WQSG_TXT_BUF
//----------------------------------------------------------------------------

#endif
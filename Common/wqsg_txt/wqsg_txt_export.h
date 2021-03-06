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
#ifndef __WQSG_TXT_Export_H__
#define __WQSG_TXT_Export_H__

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

class IExportFile
{
public:
	IExportFile(){}
	virtual ~IExportFile(){}

	virtual const u8* GetBin( n64 a_nPos , u16 a_uMaxLen , u16& a_uReturnLen ) = 0;

	virtual n64 GetFileSize() = 0;
};

class CExportFile_xFile : public IExportFile
{
	CWQSG_File m_File;
	std::vector<u8> m_Buffer;
	n64 m_nBufReadStart;
	u32 m_uBufReadLen;
	n64 m_nFileSize;
public:
	CExportFile_xFile()
	{

	}

	virtual ~CExportFile_xFile()
	{

	}

	BOOL OpenFile( WCHAR const*const a_pFile )
	{
		m_Buffer.resize( WQSG_TXT_BUF );

		if( !m_File.OpenFile( a_pFile , 1 , 3 ) )
			return FALSE;

		m_nFileSize = m_File.GetFileSize();

		if( m_nFileSize < 0 )
		{
			m_File.Close();
			return FALSE;
		}

		m_nBufReadStart = _I64_MAX;
		m_uBufReadLen = 0;//m_File.Read( &m_Buffer[0] , WQSG_TXT_BUF );

		return TRUE;
	}

	virtual const u8* GetBin( n64 a_nPos , u16 a_uMaxLen , u16& a_uReturnLen ) override
	{
		if( a_nPos < 0 )
			return NULL;

		if( a_nPos < m_nBufReadStart )
		{
			m_File.Seek( m_nBufReadStart = a_nPos );
			m_uBufReadLen = m_File.Read( &m_Buffer[0] , WQSG_TXT_BUF );
			a_uReturnLen = m_uBufReadLen > a_uMaxLen?a_uMaxLen:(u16)m_uBufReadLen;
			return a_uReturnLen?&m_Buffer[0]:NULL;
		}

		const n64 nEnd = m_nBufReadStart + m_uBufReadLen;
		const u32 uHasLen = u32(nEnd - a_nPos);

		if( uHasLen < a_uMaxLen && m_nFileSize != nEnd )
		{
			m_File.Seek( m_nBufReadStart = a_nPos );
			m_uBufReadLen = m_File.Read( &m_Buffer[0] , WQSG_TXT_BUF );
			a_uReturnLen = m_uBufReadLen > a_uMaxLen?a_uMaxLen:(u16)m_uBufReadLen;
			return a_uReturnLen?&m_Buffer[0]:NULL;
		}

		a_uReturnLen = uHasLen > a_uMaxLen?a_uMaxLen:(u16)uHasLen;
		return a_uReturnLen?(&m_Buffer[0] + size_t(a_nPos - m_nBufReadStart)):0;
	}

	virtual n64 GetFileSize() override
	{
		return m_nFileSize;
	}
};
namespace WQSG_TEXT_OUT
{

}
//============================================================================
class CWQSG_MapTbl_OUT : public CWQSG_MapTbl_Base
{
	BOOL m_bTblErrStop;
	inline BOOL Add_TBL( CMemTextW& a_tp , const WCHAR*const a_pPathName );
	inline BOOL Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName , const BOOL 不需要结束符 );
	inline const u8* Hex2Bin( const WCHAR* a_pText , INT a_iSYLen )
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
	inline BOOL GetLine( CMemTextW& a_tp , const WCHAR*const a_pPathName , CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR );
protected:
	BOOL m_bCheckTbl;

	CByteTree<const WCHAR> m_Text;
	std::vector<const WCHAR*> m_TextDatas;
/////////////////////////////////////////////////////////////
public:
	inline BOOL LOAD_TBL( WCHAR const*const a_pPathName , BOOL a_bCheckTbl );
	inline BOOL LOAD_TBL2( WCHAR const*const TBL2_path , BOOL a_bCheckTbl , const BOOL 不需要结束符 = TRUE );
	inline CWQSG_MapTbl_OUT( );
	virtual	inline ~CWQSG_MapTbl_OUT();
	inline void ClearTbl(void);

	const u64 m_OVER , m_TURN , m_END;

	inline size_t GetMaxByte()const
	{
		return m_Text.GetDepth();
	}

	inline const CByteTree<const WCHAR>& GetText()const
	{
		return m_Text;
	}
};

inline BOOL CWQSG_MapTbl_OUT::GetLine( CMemTextW& a_tp , const WCHAR*const a_pPathName , CStringW& a_strLine , CStringW& a_strL , CStringW& a_strR )
{
	while( const WCHAR* pLine = a_tp.GetLine() )
	{
		a_strLine = pLine;
		delete[]pLine;

		a_strLine.TrimLeft();

		if( a_strLine.GetLength() == 0 || a_strLine.Find( L"//" ) == 0 )
			continue;

		const int iPos = a_strLine.Find( L"=" );
		if( iPos == -1 )
		{
			zzz_Log( a_strLine + L"\r\n错误的码表格式(=)" , a_pPathName );

			if( m_bTblErrStop )
				return FALSE;
			continue;
		}

		a_strL = a_strLine.Left( iPos );
		a_strL.TrimRight();

		if(!::WQSG_IsHexText(a_strL.GetString()) //不是HEX
			|| (a_strL.GetLength()&1)//单数
			|| !(a_strL.GetLength()>>1)//为0
			)
		{
			zzz_Log( a_strLine + L"\r\n码表左边必须是(HEX)" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		a_strR = a_strLine.Mid( iPos + 1 );

		return TRUE;
	}

	return FALSE;
}

inline BOOL CWQSG_MapTbl_OUT::Add_TBL( CMemTextW& a_tp , const WCHAR*const a_pPathName )
{
	CStringW strLine , strL , strR;
	while( GetLine( a_tp , a_pPathName , strLine , strL , strR ) )
	{
		if( strR.GetLength() <= 0 )//右边不是一个字,或者没有
		{
			zzz_Log( strLine + L"\r\n码表右边必须是一个字" , a_pPathName );
			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		////////////////////////////////////////////////////////////////////////////////// 加入树
		const u8*const pSY = Hex2Bin( strL.GetString() , strL.GetLength()>>1 );

		const WCHAR* pTempText = m_Text.get( pSY , strL.GetLength()>>1 );
		//验证重复
		if( pTempText )
		{
			if( m_bCheckTbl )
			{
				zzz_Log( strLine + L"\r\n码表重复" , a_pPathName );
				if( m_bTblErrStop )
				{
					delete[]pSY;
					return FALSE;
				}
			}
			m_TextDatas.erase( find( m_TextDatas.begin() , m_TextDatas.end() , pTempText ) );
			delete[]pTempText;
		}

		pTempText = new WCHAR[strR.GetLength()+1];

		WQSG_strcpy( strR.GetString() , (WCHAR*)pTempText );

		//添加
		m_TextDatas.push_back(pTempText);
		m_Text.add( pSY , (strL.GetLength()>>1) , pTempText );
		delete[]pSY;
		/////////////////////////////////////////
	}
	return TRUE;
}

inline BOOL CWQSG_MapTbl_OUT::Add_TBL2( CMemTextW& a_tp , const WCHAR*const a_pPathName , const BOOL 不需要结束符 )
{
	BOOL bHasKzOver = FALSE;
	CStringW strLine , strL , strR;
	while( GetLine( a_tp , a_pPathName , strLine , strL , strR ) )
	{
		if( strR.GetLength() <= 0 )//无字
		{
			zzz_Log( strLine + L"\r\n控制码表右边必须至少有一个字" , a_pPathName );

			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		if( strR.GetLength() > 255 )
		{
			zzz_Log( strLine + L"\r\n控制码表右边不能超过255个字" );

			if( m_bTblErrStop )
				return FALSE;

			continue;
		}

		WCHAR* pR = NULL;
		if( strR.GetLength() >= 4 )
		{
			CStringW strTmp = strR;
			strTmp.MakeUpper();

			const u64 uKz = *(u64*)strTmp.GetString();

			if( uKz == m_OVER )
			{
				bHasKzOver = TRUE;

				strR.Delete( 0 , 4 );

				if( strR.GetLength() > 0 )
				{
					if( !::WQSG_IsDecText( strR.GetString() ) || strR.GetLength() > 3 )
					{
						zzz_Log( strLine + L"\r\n制码表结束符数字错误\r\n    格式为: 编码=OVERx\r\n  OVER大小写都行\r\n    x 只能为十进制,不能有多于字符,最长位" , a_pPathName );

						if( m_bTblErrStop )
							return FALSE;

						continue;
					}
				}

				pR = new WCHAR[strTmp.GetLength() + 1];
				WQSG_strcpy( strTmp.GetString() , pR );
			}
			else if( uKz == m_TURN )
			{
				strR.Delete( 0 , 4 );

				if( !::WQSG_IsDecText( strR.GetString() ) || strR.GetLength() > 2 )
				{
					zzz_Log( strLine + L"\r\n控制码表跳跃符数字错误\r\n    格式为: 编码=TURNx\r\n  TURN大小写都行\r\n    x 只能为十进制,不能有多于字符,最长2位" , a_pPathName );

					if( m_bTblErrStop )
						return FALSE;

					continue;
				}

				pR = new WCHAR[strTmp.GetLength() + 1];
				WQSG_strcpy( strTmp.GetString() , pR );
			}
			else if( uKz == m_END )
			{
				if( strTmp.GetLength() != 4 )
				{
					zzz_Log( strLine + L"\r\n\"END:\"后面不能有多余的字符" , a_pPathName );

					if( m_bTblErrStop )
						return FALSE;

					continue;
				}

				const u8*const pSY = Hex2Bin( strL.GetString() , strL.GetLength()>>1 );

				const WCHAR* pTxt = m_Text.get( pSY , strL.GetLength()>>1 );
				if( NULL == pTxt )
				{
					zzz_Log( strLine + L"\r\n与当前\"END:\"对应编码的项不能为空"  , a_pPathName );

					delete[]pSY;

					if( m_bTblErrStop )
						return FALSE;

					continue;
				}

				if( 1 != ::WQSG_strlen( pTxt ) )
				{
					zzz_Log( strLine + L"\r\n与当前\"END:\"对应编码的项必须为一个字符" , a_pPathName );

					delete[]pSY;

					if( m_bTblErrStop )
						return FALSE;

					continue;
				}

				pR = new WCHAR[6];

				WQSG_strcpy( strTmp.GetString() , pR );
				pR[4] = *pTxt;
				pR[5] = 0;

				delete[]pTxt;

				m_Text.add( pSY , (strL.GetLength()>>1) , NULL );

				delete[]pSY;
			}
		}

		if( !pR )
		{
			strR += L'}';
			pR = new WCHAR[strR.GetLength() + 2];

			pR[0] = L'{';
			::WQSG_strcpy( strR.GetString() , pR + 1 );
		}
		/////////////////////////////////////////////////
		const u8*const pSY = Hex2Bin( strL.GetString() , strL.GetLength()>>1 );
		const WCHAR* pTempText = m_Text.get( pSY , strL.GetLength()>>1 );
		////////////////////
		if( pTempText )
		{
			if(m_bCheckTbl)
			{
				zzz_Log( strLine + L"\r\n控制码表重复" , a_pPathName );

				if( m_bTblErrStop )
				{
					delete[]pR;
					delete[]pSY;
					return FALSE;
				}
			}

			m_TextDatas.erase( find( m_TextDatas.begin() , m_TextDatas.end() , pTempText ) );
			delete[]pTempText;
		}
		/////////////////////////////////////////
		m_TextDatas.push_back(pR);
		m_Text.add( pSY , (strL.GetLength()>>1) , pR );
		delete[]pSY;
	}

	if( 不需要结束符 || bHasKzOver )
		return TRUE;

	zzz_Log( L"\r\n控制符至少需要一个结束符" , a_pPathName );
	return FALSE;		
}

inline BOOL CWQSG_MapTbl_OUT::LOAD_TBL( WCHAR const*const a_pPathName , BOOL a_bCheckTbl )
{
	m_bCheckTbl = a_bCheckTbl;

	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		//zzz_Log();
		return FALSE;
	}

	return Add_TBL( tp , a_pPathName );
}

inline BOOL CWQSG_MapTbl_OUT::LOAD_TBL2( WCHAR const*const a_pPathName , BOOL a_bCheckTbl , const BOOL 不需要结束符 )
{
	m_bCheckTbl = a_bCheckTbl;

	CMemTextW tp;
	if( !tp.Load( a_pPathName , 1024*1024*32 ) )
	{
		//zzz_Log();
		return FALSE;
	}

	return Add_TBL2( tp , a_pPathName , 不需要结束符 );
}

inline CWQSG_MapTbl_OUT::CWQSG_MapTbl_OUT( )
: CWQSG_MapTbl_Base( )
, m_bTblErrStop(TRUE)
, m_OVER( *((u64*)L"OVER") )
, m_TURN( *((u64*)L"TURN") )
, m_END( *((u64*)L"END:") )
{
}

inline CWQSG_MapTbl_OUT::~CWQSG_MapTbl_OUT()
{
	ClearTbl();
}

inline void CWQSG_MapTbl_OUT::ClearTbl(void)
{
	m_Text.clear();

	std::vector<const WCHAR*>::iterator it = m_TextDatas.begin();
	for( ; it != m_TextDatas.end() ; ++it )
	{
		delete[] (*it);
	}
	m_TextDatas.clear();
}
//----------------------------------------------------------------------------
class CWQSG_TxtExport
{
	inline void zzz_WriteText( CWQSG_File& a_fp , s64 a_nAddr , UINT a_uLen , const WCHAR*const a_pTXT_path )
	{
		if( a_uLen > m_MIN_OUT && a_uLen < m_MAX_OUT )
		{
			m_strOut += L"\r\n\r\n";

			WCHAR addr_len[20];
			::swprintf( addr_len , L"%08X,%u," , (u32)( a_nAddr & 0xFFFFFFFF ) , a_uLen );

			if( !a_fp.WriteStrW( addr_len ) )
			{
				zzz_Log( L"写文本失败"  , a_pTXT_path );
			}

			if( !a_fp.Write( m_strOut.GetString() , m_strOut.GetLength()<<1 ) )
			{
				zzz_Log( L"写文本失败"  , a_pTXT_path );
			}
		}
	}

	inline BOOL Export( CWQSG_File& a_fp , IExportFile& a_Rom , s64 a_nBeingPos , const n64 a_nEndPos , WCHAR const*const TXT_path );
private:
	UINT m_MAX_OUT;
	UINT m_MIN_OUT;
	CWQSG_MapTbl_OUT m_Tbl;

	CStringW m_strOut;
	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL )
	{
		CWQSG_MapTbl_OUT::zzz_Log( a_szText , a_szTitle );
	}
public:
	inline CWQSG_TxtExport( );
	virtual	inline ~CWQSG_TxtExport();
	inline void ClearTbl(void);
	inline BOOL LoadTbl( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl = FALSE );
#ifndef _____________________________________________
	inline BOOL ExportText( const WCHAR* rom_path , const WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#else
	BOOL ExportText( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 );
#endif
};

template<typename T2>
inline u16 TxtExportMin16( const n64& a_1 , const T2& a_2 )
{
	return a_1 < a_2?(u16)a_1:(u16)a_2;
}

BOOL CWQSG_TxtExport::Export( CWQSG_File& a_fp , IExportFile& a_Rom , s64 a_nBeingPos , const n64 a_nEndPos , const WCHAR*const TXT_path )
{
	m_strOut = L"";

	s64		ot_addr = a_nBeingPos;
	UINT	ot_len = 0;

	if( m_strOut.GetBuffer( WQSG_TXT_BUF + 1 ) == NULL )
		return FALSE;

	const CByteTree<const WCHAR>& text = m_Tbl.GetText();

	const u8* pBin;
	u16 uBinLen;

	size_t returnLen;
	const WCHAR* pReturnText;
	//--------------------------------------
	while( pBin = a_Rom.GetBin( a_nBeingPos , TxtExportMin16( (a_nEndPos - a_nBeingPos) , m_Tbl.GetMaxByte() ) , uBinLen ) )
	{
		pReturnText = text.find( returnLen , pBin , uBinLen );

		if( pReturnText )
		{
			a_nBeingPos += returnLen;

			if( WQSG_strlen( pReturnText ) >= 4 )
			{
				const u64 uKz = *(u64*)pReturnText;
				if( uKz == m_Tbl.m_OVER )
				{//是结束符
					if( pReturnText[4] )
					{
						UINT uiTmp = 0;
						::swscanf_s( pReturnText + 4 , L"%u" , &uiTmp );
						a_nBeingPos += uiTmp;
					}
					/////////////////////////////
					zzz_WriteText( a_fp , ot_addr , ot_len , TXT_path );
					m_strOut = L"";
					ot_len = 0;
					ot_addr = a_nBeingPos;
				}
				else if( uKz == m_Tbl.m_TURN)
				{//是跳跃符
					ot_len += returnLen;

					m_strOut += L"{跳跃符";
					for( UINT i = 0 ; i < returnLen ; ++i )
					{
						m_strOut.AppendFormat( L"%02X" , pBin[i] );
					}
					m_strOut += L'：';
					///////////////
					UINT uiTmp = 0;
					::swscanf( pReturnText + 4 , L"%u" , &uiTmp );

					pBin = a_Rom.GetBin( a_nBeingPos , TxtExportMin16( (a_nEndPos - a_nBeingPos) , uiTmp ) , uBinLen );
					if( pBin )
					{
						a_nBeingPos += uBinLen;
						ot_len += uBinLen;

						for( UINT i = 0 ; i < uBinLen ; ++i )
						{
							m_strOut.AppendFormat( L"%02X" , pBin[i] );
						}
					}

					m_strOut += L'}';
				}
				else if( uKz == m_Tbl.m_END)
				{
					ot_len += returnLen;

					m_strOut += ( pReturnText + 4 );

					/////////////////////////////
					zzz_WriteText( a_fp , ot_addr , ot_len , TXT_path );
					m_strOut = L"";
					ot_len = 0;
					ot_addr = a_nBeingPos;
				}
				else
				{//通常
					ot_len += returnLen;
					m_strOut += pReturnText;
				}
			}
			else
			{//通常
				ot_len += returnLen;
				m_strOut += pReturnText;
			}
		}
		else
		{
			++a_nBeingPos;
			zzz_WriteText( a_fp , ot_addr , ot_len , TXT_path );
			m_strOut = L"";
			ot_len = 0;
			ot_addr = a_nBeingPos;
		}
	}
	zzz_WriteText( a_fp , ot_addr , ot_len , TXT_path );

	return TRUE;
}

CWQSG_TxtExport::CWQSG_TxtExport( )
{
}

CWQSG_TxtExport::~CWQSG_TxtExport()
{
}

void CWQSG_TxtExport::ClearTbl(void)
{
	m_Tbl.ClearTbl();
}

BOOL CWQSG_TxtExport::LoadTbl( const WCHAR* a_pPathName , const WCHAR* TBL2_path , BOOL a_bCheckTbl )
{
	ClearTbl();
	if( !m_Tbl.LOAD_TBL( a_pPathName , a_bCheckTbl ) )
	{
		ClearTbl();
		return FALSE;
	}
	if( TBL2_path )
	{
		if( *TBL2_path )
			if( !m_Tbl.LOAD_TBL2( TBL2_path , a_bCheckTbl ) )
			{
				ClearTbl();
				return FALSE;
			}
	}
	return TRUE;
}
#ifndef _____________________________________________
BOOL CWQSG_TxtExport::ExportText( const WCHAR* a_rom_path , const WCHAR* a_TXT_path , s64 a_KS , s64 a_JS , UINT a_MIN , UINT a_MAX )
{
	if( a_KS < 0 )
	{
		zzz_Log( L"开始地址 不能大于 0x7FFFFFFFFFFFFFFF"  , a_rom_path );
		return FALSE;
	}
	if( a_JS < 0 )
	{
		zzz_Log( L"结束地址 不能大于 0x7FFFFFFFFFFFFFFF"  , a_rom_path );
		return FALSE;
	}
	////////// 验证地址
	if( a_KS > a_JS )
	{
		zzz_Log( L"结束地址 不能小于 开始地址"  , a_rom_path );
		return FALSE;
	}
	//////////////// 初始化变量
	m_MIN_OUT = a_MIN;
	m_MAX_OUT = a_MAX;
	///////////////////////////
	CExportFile_xFile romFile;
	if( !romFile.OpenFile( a_rom_path ) )
	{
		zzz_Log( L"打开ROM文件失败"  , a_rom_path );
		return FALSE;
	}
	////////// 验证地址
	const s64 romsize = romFile.GetFileSize();
	if( a_KS >= romsize )
	{
		zzz_Log( L"开始地址不存在,超出文件"  , a_rom_path );
		return FALSE;
	}
	if( a_JS >= romsize )
		a_JS = romsize - 1;
	/////////////////////
	CWQSG_File fp;
	if( !fp.OpenFile( a_TXT_path , 4 , 3 ) )
	{
		zzz_Log( L"创建文本文件失败"  , a_TXT_path );
		return FALSE;
	}
	if( 2 != fp.Write("\xFF\xFE" , 2 ) )
	{
		zzz_Log( L"写文本头失败"  , a_TXT_path );
		return FALSE;
	}
	if( a_JS > 0xFFFFFFFF )
	{
		u32 JJJ = (u32)(a_JS>>32);
		m_strOut.Format( L".WQSG:%08X\r\n\r\n\r\n" , JJJ );

		if( (m_strOut.GetLength()<<1) != fp.Write( m_strOut.GetString() , m_strOut.GetLength()<<1 ) )
		{
			zzz_Log( L"写文本信息头失败"  , a_TXT_path );
			return FALSE;
		}
	}
	return( Export( fp , romFile , a_KS , a_JS+1 , a_TXT_path ) );
}
#else
BOOL CWQSG_TxtExport::ExportText( WCHAR* rom_path , WCHAR* TXT_path , s64 KS , s64 JS , UINT MIN = 0 ,UINT MAX = 99999 )
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
	return( 正式导出( fp , mFile_ROM , KS , JS + 1 , TXT_path ) );
}
#endif
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
		if( 2 != a_fp.Write("\xFF\xFE",2) )
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
					UINT 实际III = mFile_ROM.GetUCHAR( (UINT)GetMaxByte() , 缓冲 );
					if( 0 == 实际III )
						break;
					//////////////////////匹配
					const WCHAR* tmp = NULL;//改成匹配

					size_t 索引长度;
					tmp = m_Text.find( 索引长度 , 缓冲 , 实际III );
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
		m_Text.clear();

		已载入码表 = FALSE;
	}
	BOOL 载入码表( WCHAR* a_pPathName , WCHAR* TBL2_path , int a_iCheckTbl = 0 , BOOL 需要结束符 = FALSE )
	{
		m_bCheckTbl = (a_iCheckTbl != 0);
		清空码表();
		if(LOAD_TBL(a_pPathName,m_bCheckTbl))
		{
			if(需要结束符)
			{
				if(*TBL2_path)
				{
					if(LOAD_TBL2(TBL2_path,m_bCheckTbl,FALSE))
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
		if(最后指针地址 >= mFile_ROM.GetFileSize())
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
#undef WQSG_TXT_BUF
//----------------------------------------------------------------------------

#endif
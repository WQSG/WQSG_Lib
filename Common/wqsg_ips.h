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
#ifndef WQSG_IPS_H__
#define WQSG_IPS_H__
//3.0							   占用字节
/*****************************************************************************************
0x00	*IPS文件头					*	13	*	WQSG-PATCH3.0								(ASCII) 
0x0D	*							*	3	*	(保留,常为0)								(HEX)
0x10	*总文件长度					*	8	*												(HEX)
0x18	*原文件的CRC32				*	4	*	原文件的CRC32								(HEX)
0x1C	*补丁数据在本文件开始位置	*	4	*	相对于 0x00的偏移							(HEX)
0x20	*说明文本在本文件中的偏移	*	8	*	相对于 0x00的偏移,最后以0结尾				(HEX)
0x28	*标志						*	1	*	0x0 未定义,									(HEX)
												0x1 紧接地址为4字节的相对偏移,
												0x2 紧接地址为 8字节的绝对地址
												0xFF 文件结束
0x?1	*补丁数据地址				*	4(8)*	相对于上一个地址的偏移量(绝对地址)			(HEX)
0x?2	*补丁数据长度				*	2	*	紧接 补丁数据 的长度						(HEX)
0x?2+2	*补丁数据					*	?	*	数据最大长度不能超过上面规定的				(原HEX)
		*.........					*	...	*	重复上面三项,直到补丁数据结束
0x?3	*说明的文本					*	?	*  utf16le 编码格式,以文本结束标志结束,可有可无 (UNICODE)
//****************************************************************************************/
#include "../Interface/wqsg_ifile.h"

class CWD_WipsInfo
{
public:
	char m_szMagic[17];
	WCHAR m_szDesc[0x10000];
	s64 m_nTargetSize;
	u32 m_uCrc32;
	CWD_WipsInfo()
	{
		Clear();
	}
	virtual	~CWD_WipsInfo(){}
	void Clear()
	{
		m_nTargetSize = 0;
		m_uCrc32 = 0;
		*m_szMagic = 0;
		*m_szDesc = 0;
	}

	BOOL LoadWipsDesc( const TCHAR*const a_pWips , n64 a_nBaseOffset = 0 )
	{
		Clear();

		if( a_nBaseOffset < 0 )
			return FALSE;

		::CWQSG_File IPS_File;
		if( !IPS_File.OpenFile( a_pWips , 1 , 3 ) )
		{
			return FALSE;
		}

		IPS_File.Seek( a_nBaseOffset );

		if( !IPS_File.Read( m_szMagic , 16 ) )
		{
			return FALSE;
		}

		m_szMagic[16] = 0;
		int X , Y;
		if(2 != ::sscanf_s( m_szMagic , "WQSG-PATCH%d.%d" , &X , &Y ) )
		{
			return FALSE;
		}

		switch(X)
		{
		case 1:
			Clear();
			break;
		case 2:
			Clear();
			break;
		case 3:
			switch(Y)
			{
			case 0:
				if( !IPS_File.Read( &m_nTargetSize , 8 ) )
				{
					Clear();
					break;
				}
				m_nTargetSize = L2H(m_nTargetSize);

				if( !IPS_File.Read( &m_uCrc32 , 4 ) )
				{
					Clear();
					break;
				}
				m_uCrc32 = L2H(m_uCrc32);
				{
					DWORD dataOffset;
					//取数据指针
					if( !IPS_File.Read( &dataOffset , 4 ) )
					{
						Clear();
						break;
					}
					dataOffset = L2H(dataOffset);

					n64 nFileSize = IPS_File.GetFileSize();
					if( dataOffset < 40
						|| nFileSize <= dataOffset )
					{
						Clear();
						break;
					}

					s64 nDescOffset;
					if( !IPS_File.Read( &nDescOffset , 8 ) )
					{
						Clear();
						break;
					}
					nDescOffset = L2H(nDescOffset);
					if(nDescOffset)
					{
						IPS_File.Seek( a_nBaseOffset + nDescOffset );
						IPS_File.Read( m_szDesc , 65536 );
					}
				}
				return TRUE;
				break;
			default:
				Clear();
				break;
			}
			break;
		default:
			Clear();
			break;
		}

		return FALSE;
	}
};

class CWQSG_Ips_Maker
{
	WCHAR m_szMsgBuffer[1024];

	BOOL StartMaker30( WQSG_File_mem& a_OldFile , WQSG_File_mem& a_NewFile , ::CWQSG_File& a_IPS_File ,
		const s64 a_pos_begin , const s64 a_pos_end )
	{
		if( a_pos_begin < 0 )
		{
			swprintf( m_szMsgBuffer , L"开始地址 不能超过 0x7FFFFFFFFFFFFFFF" );
			return FALSE;
		}
		if( a_pos_end < 0 )
		{
			swprintf( m_szMsgBuffer , L"结束地址 不能超过 0x7FFFFFFFFFFFFFFF" );
			return FALSE;
		}

		const s64 界限pos原 = a_OldFile.GetFileSize( ) - 1;

		const s64 界限pos新 = a_NewFile.GetFileSize( ) - 1;

		if( a_pos_begin > a_pos_end )
		{
			swprintf( m_szMsgBuffer , L"结束地址 不能大于 开始地址" );
			return FALSE;
		}
		if( ((u64)a_pos_begin>(u64)界限pos原) || ((u64)a_pos_begin>(u64)界限pos新) )
		{
			swprintf( m_szMsgBuffer , L"不存在 公共开始地址" );
			return FALSE;
		}

		CWD_BIN binbuf;
		if( NULL == binbuf.SetBufSize( 0xFFFF ) )
			return FALSE;

		s64 commbase = 0;
		s64 commBeginPos = a_pos_begin;
		s64 commEndPos = (界限pos新 < 界限pos原 )?(界限pos新):(界限pos原);
		commEndPos = ( commEndPos < a_pos_end )?commEndPos:a_pos_end;

		//第一次,两个共同存在区域
		{
			s64 界限地址 = commEndPos + 1;
			//证明只需要搞定 新文件
			s64 剩余len;
			while( (剩余len = (界限地址 - commBeginPos)) > 0 )
			{
				const u32 curlen = ( 剩余len > 0x2000000 )?0x2000000:(u32)剩余len;
				剩余len -= curlen;

				{
					u8 const* ptr原 = a_OldFile.GetPtr( commBeginPos , curlen );
					if( NULL == ptr原 )
						return FALSE ;

					u8 const* ptr新 = a_NewFile.GetPtr( commBeginPos , curlen );
					if( NULL == ptr新 )
						return FALSE ;
					//--------------------------------------------------------------
					binbuf.LEN = 0;
					u8 const*const ptr界限 = ptr原 + curlen;
					u8* buf = binbuf.BIN;

					while( ptr原 < ptr界限 )
					{
						if( 0x647F890 == commBeginPos )
						{
							int x = 0;
						}
						if( (*ptr原) != (*ptr新) )
						{
							*(buf++) = *ptr新;
							if( ++(binbuf.LEN) == 0xFFFF )
							{
								break;
							}
							++ptr原 , ++ptr新;
						}
						else if( binbuf.LEN )
						{
							break;
						}
						else
						{
							++ptr原 , ++ptr新;

							if( 剩余len && (( ptr界限 - ptr原 ) < 0xFFFF ) )
							{
								break;
							}
							++commBeginPos;
						}
					}
					a_OldFile.Free();a_NewFile.Free();
				}
				if( binbuf.LEN )
				{
					s64 def_offset = commBeginPos - commbase;
					if( def_offset <= 0xFFFFFFFF )
					{
						if( 1 != a_IPS_File.Write( "\1" , 1 ) )
						{
							swprintf( m_szMsgBuffer , L"写出补丁失败(1),磁盘空间不足?" );
							return FALSE;
						}
						if( 4 != a_IPS_File.Write( &H2L((u32)def_offset) , 4 ) )
						{
							swprintf( m_szMsgBuffer , L"写出补丁失败(-0),磁盘空间不足?" );
							return FALSE;
						}
					}
					else
					{
						if( 1 != a_IPS_File.Write( "\2" , 1 ) )
						{
							swprintf( m_szMsgBuffer , L"写出补丁失败(1),磁盘空间不足?" );
							return FALSE;
						}
						if( 8 != a_IPS_File.Write( &H2L((u64)commBeginPos) , 8 ) )
						{
							swprintf( m_szMsgBuffer , L"写出补丁失败(-0),磁盘空间不足?" );
							return FALSE;
						}
					}
					if( 2 != a_IPS_File.Write( &H2L((u16)binbuf.LEN) , 2 ) )
					{
						swprintf( m_szMsgBuffer , L"写出补丁失败(-1),磁盘空间不足?" );
						return FALSE;
					}
					if( binbuf.LEN != a_IPS_File.Write( binbuf.BIN , binbuf.LEN ) )
					{
						swprintf( m_szMsgBuffer , L"写出补丁失败(-2),磁盘空间不足?" );
						return FALSE;
					}
					commbase = commBeginPos;
					commBeginPos += (binbuf.LEN);
				}
				else
				{
					++commBeginPos;
				}
			}
		}
		//第二次,新文件多余
		if( 界限pos新 > 界限pos原 )
		{
			commBeginPos = 界限pos原 + 1;
			s64 剩余len = 界限pos新 - commBeginPos + 1;
			while( 剩余len )
			{
				const u32 curlen = ( 剩余len > 0xFFFF )?0xFFFF:(u32)剩余len;

				u8 const* ptr新 = a_NewFile.GetPtr( commBeginPos , curlen );
				if( NULL == ptr新 )
					return FALSE ;

				剩余len -= curlen;

				s64 def_offset = commBeginPos - commbase;
				if( def_offset <= 0xFFFFFFFF )
				{
					if( 1 != a_IPS_File.Write( "\1" , 1 ) )
					{
						a_NewFile.Free();
						swprintf( m_szMsgBuffer , L"写出补丁失败(1),磁盘空间不足?" );
						return FALSE;
					}
					if( 4 != a_IPS_File.Write( &H2L((u32)def_offset) , 4 ) )
					{
						a_NewFile.Free();
						swprintf( m_szMsgBuffer , L"写出补丁失败(-0),磁盘空间不足?" );
						return FALSE;
					}
				}
				else
				{
					if( 1 != a_IPS_File.Write( "\2" , 1 ) )
					{
						a_NewFile.Free();
						swprintf( m_szMsgBuffer , L"写出补丁失败(1),磁盘空间不足?" );
						return FALSE;
					}
					if( 8 != a_IPS_File.Write( &H2L((u64)commBeginPos) , 8 ) )
					{
						a_NewFile.Free();
						swprintf( m_szMsgBuffer , L"写出补丁失败(-0),磁盘空间不足?" );
						return FALSE;
					}
				}
				if( 2 != a_IPS_File.Write( &H2L((u16)curlen) , 2 ) )
				{
					a_NewFile.Free();
					swprintf( m_szMsgBuffer , L"写出补丁失败(-1),磁盘空间不足?" );
					return FALSE;
				}
				if( curlen != a_IPS_File.Write( ptr新 , curlen ) )
				{
					a_NewFile.Free();
					swprintf( m_szMsgBuffer , L"写出补丁失败(-2),磁盘空间不足?" );
					return FALSE;
				}
				a_NewFile.Free();
				commbase = commBeginPos;
				commBeginPos += curlen;
			}
		}
		a_IPS_File.Write("\xff",1);
		return TRUE;
#undef DEF_FN_写出IPS
	}

public:
	CWQSG_Ips_Maker()
	{
		m_szMsgBuffer[0] = 0;
	}

	virtual	~CWQSG_Ips_Maker()
	{}

	BOOL MakeWips30( const TCHAR*const a_pOldFile , const TCHAR*const a_pNewFile , const TCHAR*const a_pWipsFile , const s64 a_nBeginOffset ,//其中一个文件失效
		const s64 a_nEndOffset ,//大于新文件,按照到结尾
		const s64 a_nBaseOffset ,
		INT a_bUseCRC32 = 0 ,//非零验证
		const WCHAR* a_pDesc = NULL ,
		DWORD a_dwDescLen = 0 ,
		CWD_BIN* EXE = NULL )
	{
		if( a_nBeginOffset < 0 )
		{
			swprintf( m_szMsgBuffer , L"开始地址 不能超过 0x7FFFFFFFFFFFFFFF" );
			return FALSE;
		}
		if( a_nEndOffset < 0 )
		{
			swprintf( m_szMsgBuffer , L"结束地址 不能超过 0x7FFFFFFFFFFFFFFF" );
			return FALSE;
		}
		if( a_nEndOffset < a_nBeginOffset )
		{
			swprintf( m_szMsgBuffer , L"结束地址 不能比 开始地址小" );
			return FALSE;
		}

		if( a_dwDescLen > 65535 )
		{
			swprintf( m_szMsgBuffer , L"说明文字 字数不能超过65535个" );
			return FALSE;
		}
		//打开原文件
		WQSG_File_mem oldFile , newFile ;

		if( !oldFile.OpenFile( a_pOldFile , 3 ) )
		{
			swprintf( m_szMsgBuffer , L"原文件打开失败" );
			return FALSE;
		}
		//打开新文件
		if( !newFile.OpenFile( a_pNewFile , 3 ) )
		{
			swprintf( m_szMsgBuffer , L"新文件打开失败" );
			return FALSE;
		}
		//判定开始地址 合法性
		s64 size1 = oldFile.GetFileSize( );
		const s64 size2 = newFile.GetFileSize( );

		if( a_nBeginOffset >= size1 )
		{
			swprintf( m_szMsgBuffer , L"开始地址 不能比 原文件大\r\n没意义" );
			return FALSE;
		}
		if( a_nBeginOffset >= size2 )
		{
			swprintf( m_szMsgBuffer , L"开始地址 不能比 新文件大\r\n没意义" );
			return FALSE;
		}

		const s64 pos_begin = a_nBeginOffset;
		const s64 pos_end =  ( a_nEndOffset < size2 )?a_nEndOffset:size2 - 1;
		///////////////
		::CWQSG_File IPS_File;
		if( ! IPS_File.OpenFile( a_pWipsFile ,(a_nBaseOffset==0)?4:2,3) )
		{
			swprintf( m_szMsgBuffer , L"创建补丁文件失败" );
			return FALSE;
		}
		IPS_File.Seek(a_nBaseOffset);
		//验证CRC
		DWORD CRC32 = 0;
		if( a_bUseCRC32 != 0 )
		{
			if( !oldFile.GetCrc32( CRC32 ) )
			{
				swprintf( m_szMsgBuffer , L"计算CRC32失败" );
				return FALSE;
			}//
		}
		////////////////////////////////自解头
		u64 uExeOffset = 0;
		if(EXE != NULL)
		{
			IPS_File.Write( EXE->BIN , EXE->LEN );
			uExeOffset = EXE->LEN;
		}
		//写文标识
		if( 16 != IPS_File.Write("WQSG-PATCH3.0\0\0\0",16) )
		{
			swprintf( m_szMsgBuffer , L"WIPS信息失败(1)" );
			return FALSE;
		}
		//写文件长度
		if( 8 != IPS_File.Write( &H2L((u64)size2) , 8 ) )
		{
			swprintf( m_szMsgBuffer , L"WIPS信息失败(2)" );
			return FALSE;
		}
		////写出CRC验证
		if( 4 != IPS_File.Write( &H2L((u32)CRC32) ,4 ) )
		{
			swprintf( m_szMsgBuffer , L"WIPS信息失败(3)" );
			return FALSE;
		}
		//写出数据偏移
		size1 = 0x28;
		if( 4 != IPS_File.Write( &H2L((u32)size1) ,4 ) )
		{
			swprintf( m_szMsgBuffer , L"WIPS信息失败(4)" );
			return FALSE;
		}
		//写出说明文本偏移
		if( 8 != IPS_File.Write( &H2H((u64)0) , 8 ) )
		{
			swprintf( m_szMsgBuffer , L"WIPS信息失败(5)" );
			return FALSE;
		}
		///////////////////
		if( StartMaker30( oldFile  , newFile , IPS_File , pos_begin , pos_end ) )
		{
			if( a_dwDescLen )
			{
				const s64 pos = IPS_File.Tell( ) - uExeOffset;
				IPS_File.Write( a_pDesc , (a_dwDescLen<<1) );
				IPS_File.Write( &H2H((u16)0), 2 );
				IPS_File.Seek( uExeOffset + a_nBaseOffset + 0x20 );
				IPS_File.Write( &H2L((u64)pos) , 8 );
			}
			swprintf( m_szMsgBuffer , L"补丁制作完毕" );
			return TRUE;
		}
		return FALSE;
	}

	const WCHAR* GetMsg()const
	{
		return m_szMsgBuffer;
	}
};

class CWQSG_Ips_In
{
	WCHAR m_szMsgBuffer[1024];
	u8 m_buf[0xFFFF];

	BOOL 打补丁30( CWQSG_File& a_IPS_File , CWQSG_File& a_ROM_File )
	{
		DWORD dwLen;
		s64 nPos = 0;

		while(TRUE)
		{
			if( !a_IPS_File.Read( m_buf , 1 ) )
			{
				swprintf( m_szMsgBuffer , L"打补丁失败\n补丁文件已损坏(5)" );
				return FALSE;
			}
			switch(*m_buf)
			{
			case 0x01://相对偏移
				if( !a_IPS_File.Read( m_buf , 4 ) )
				{
					swprintf( m_szMsgBuffer , L"打补丁失败\n补丁文件已损坏(6.1)" );
					return FALSE;
				}
				nPos += L2H(*(u32*)m_buf);
				break;
			case 0x02://绝对偏移
				if( !a_IPS_File.Read( &nPos , 8 ) )
				{
					swprintf( m_szMsgBuffer , L"打补丁失败\n补丁文件已损坏(6.2)" );
					return FALSE;
				}
				nPos = L2H(nPos);
				break;
			case 0xFF:
				return TRUE;
				break;
			default:
				swprintf( m_szMsgBuffer , L"非法的WIPS补丁文件(2)" );
				return FALSE;
			}
			dwLen = 0;

			if( !a_IPS_File.Read( &dwLen , 2 ) )
			{
				swprintf( m_szMsgBuffer , L"打补丁失败\n补丁文件已损坏(7)" );
				return FALSE;
			}
			dwLen = L2H(dwLen);

			if( !a_IPS_File.Read( m_buf , dwLen ) )
			{
				swprintf( m_szMsgBuffer , L"打补丁失败\n补丁文件已损坏(8)" );
				return FALSE;
			}

			if( !a_ROM_File.Seek( nPos ) )
			{
				swprintf( m_szMsgBuffer , L"打补丁失败\n设置ROM文件位置失败" );
				return FALSE;
			}
			if( dwLen != a_ROM_File.Write( m_buf , dwLen ) )
			{
				swprintf( m_szMsgBuffer , L"打补丁失败\n写数据失败失败" );
				return FALSE;
			}
		}
	}
public:
	CWQSG_Ips_In()
	{
		m_szMsgBuffer[0] = 0;
	}

	const WCHAR* GetMsg()const
	{
		return m_szMsgBuffer;
	}

	BOOL 打补丁( const TCHAR*const a_pWipsFile , const TCHAR*const a_pTFile , n64 a_nBaseOffset = 0 )
	{
		if( a_nBaseOffset < 0 )
		{
			swprintf( m_szMsgBuffer , L"基础地址不能小于0" );
			return FALSE;
		}

		::CWQSG_File IPS_File,ROM_File;
		if( !IPS_File.OpenFile( a_pWipsFile , 1 , 3 ) )
		{
			swprintf( m_szMsgBuffer , L"补丁文件打开失败" );
			return FALSE;
		}

		if( !ROM_File.OpenFile( a_pTFile , 3 , 3 ) )
		{
			swprintf( m_szMsgBuffer , L"目标文件打开失败" );
			return FALSE;
		}

		IPS_File.Seek( a_nBaseOffset );

		if( !IPS_File.Read( m_buf,16) )
		{
			swprintf( m_szMsgBuffer , L"非法的WIPS补丁文件" );
			return FALSE;
		}

		m_buf[16] = 0;
		int X,Y;
		if(2 != ::sscanf_s((CHAR*)m_buf,"WQSG-PATCH%d.%d",&X,&Y))
		{
			swprintf( m_szMsgBuffer , L"非法的WIPS补丁文件" );
			return FALSE;
		}

		switch(X)
		{
		case 1:
			swprintf( m_szMsgBuffer , L"暂不支持老版本的WIPS补丁文件" );
			return FALSE;
			break;
		case 2:
			swprintf( m_szMsgBuffer , L"暂不支持老版本的WIPS补丁文件" );
			return FALSE;
			break;
		case 3:
			switch(Y)
			{
			case 0:
				{
					s64 nFinalFileSize;
					if( !IPS_File.Read( &nFinalFileSize , 8 ) )
					{
						swprintf( m_szMsgBuffer , L"补丁文件已损坏(1)" );
						return FALSE;
					}
					nFinalFileSize = L2H(nFinalFileSize);

					//CRC
					{
						u32 CRC32;
						if( !IPS_File.Read(&CRC32,4) )
						{
							swprintf( m_szMsgBuffer , L"补丁文件已损坏(2)" );
							return FALSE;
						}
						CRC32 = L2H(CRC32);
					}
					//取数据指针
					{
						s64 nDataOffset = 0;
						if( !IPS_File.Read( &nDataOffset , 4 ) )
						{
							swprintf( m_szMsgBuffer , L"补丁文件已损坏(3)" );
							return FALSE;
						}
						nDataOffset = L2H((u32)nDataOffset);
						{
							const s64 size = IPS_File.GetFileSize() - a_nBaseOffset;
							if( nDataOffset < 40 || size <= nDataOffset )
							{
								swprintf( m_szMsgBuffer , L"补丁文件已损坏(4)" );
								return FALSE;
							}
						}
						if( !ROM_File.SetFileLength(nFinalFileSize) )
						{
							swprintf( m_szMsgBuffer , L"改变文件大小失败,可能是磁盘容量不足" );
							return FALSE;
						}
						if( ! IPS_File.Seek( a_nBaseOffset + nDataOffset ) )
						{
							swprintf( m_szMsgBuffer , L"设置文件指针错误(1)" );
							return FALSE;
						}
					}
				}
				
				if( 打补丁30( IPS_File , ROM_File ) )
				{
					swprintf( m_szMsgBuffer , L"补丁成功" );
					return TRUE;
				}
				return FALSE;
				break;
			default:
				swprintf( m_szMsgBuffer , L"未知版本的WIPS补丁文件" );
				return FALSE;
			}
			break;
		default:
			break;
		}

		swprintf( m_szMsgBuffer , L"未知版本的WIPS补丁文件" );
		return FALSE;
	}
};

#endif

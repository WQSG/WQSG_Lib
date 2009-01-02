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
0x?3	*说明的文本					*	?	*  UNICODE 编码格式,以文本结束标志结束,可有可无 (UNICODE)
//****************************************************************************************/
#include"WQSG_xFile.h"
#include"WQSG_File_M.h"

#include<windows.h>
#include<stdio.h>
#include<tchar.h>

class CWD_WIPS信息
{
public:
	TCHAR 补丁标识[17];
	WCHAR 说明文本[0x10000];
	s64 目标大小;
	DWORD 验证CRC32;
	CWD_WIPS信息()
	{
		清空();
	}
	virtual	~CWD_WIPS信息(){}
	void 清空()
	{
		目标大小 = 0;
		验证CRC32 = 0;
		*补丁标识 = 0;
		*说明文本 = 0;
	}
};
class CWQSG_IPS_OUT
{
	HWND			m_hwnd;
	TCHAR*			m_标题;
	::CWQSG_MSG_W	m_MSG;
	BOOL 正式制作30( WQSG_File_mem& 原文件映射 , WQSG_File_mem& 新文件映射 , ::CWQSG_File& IPS_File ,
		const s64 pos_begin , const s64 pos_end )
	{
		if( pos_begin < 0 )
		{
			m_MSG.show(_T("开始地址 不能超过 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( pos_end < 0 )
		{
			m_MSG.show(_T("结束地址 不能超过 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}

		const s64 界限pos原 = 原文件映射.GetFileSize( ) - 1;

		const s64 界限pos新 = 新文件映射.GetFileSize( ) - 1;

		if( pos_begin > pos_end )
		{
			m_MSG.show(_T("结束地址不能大于开始地址"));
			return FALSE;
		}
		if( ((u64)pos_begin>(u64)界限pos原) || ((u64)pos_begin>(u64)界限pos新) )
		{
			m_MSG.show(_T("不存在公共开始地址"));
			return FALSE;
		}

		CWD_BIN binbuf;
		if( NULL == binbuf.SetBufSize( 0xFFFF ) )
			return FALSE;

		s64 commbase = 0;
		s64 commBeginPos = pos_begin;
		s64 commEndPos = (界限pos新 < 界限pos原 )?(界限pos新):(界限pos原);
		commEndPos = ( commEndPos < pos_end )?commEndPos:pos_end;

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
					u8 const* ptr原 = 原文件映射.GetPtr( commBeginPos , curlen );
					if( NULL == ptr原 )
						return FALSE ;

					u8 const* ptr新 = 新文件映射.GetPtr( commBeginPos , curlen );
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
					原文件映射.Free();新文件映射.Free();
				}
				if( binbuf.LEN )
				{
					s64 def_offset = commBeginPos - commbase;
					if( def_offset <= 0xFFFFFFFF )
					{
						if( 1 != IPS_File.Write( "\1" , 1 ) )
						{
							m_MSG.show(_T("写出补丁失败(1),磁盘空间不足?"));
							return FALSE;
						}
						if( 4 != IPS_File.Write( &def_offset , 4 ) )
						{
							m_MSG.show(_T("写出补丁失败(-0),磁盘空间不足?"));
							return FALSE;
						}
					}
					else
					{
						if( 1 != IPS_File.Write( "\2" , 1 ) )
						{
							m_MSG.show(_T("写出补丁失败(1),磁盘空间不足?"));
							return FALSE;
						}
						if( 8 != IPS_File.Write( &commBeginPos , 8 ) )
						{
							m_MSG.show(_T("写出补丁失败(-0),磁盘空间不足?"));
							return FALSE;
						}
					}
					if( 2 != IPS_File.Write( &binbuf.LEN , 2 ) )
					{
						m_MSG.show(_T("写出补丁失败(-1),磁盘空间不足?"));
						return FALSE;
					}
					if( binbuf.LEN != IPS_File.Write( binbuf.BIN , binbuf.LEN ) )
					{
						m_MSG.show(_T("写出补丁失败(-2),磁盘空间不足?"))
							;return FALSE;
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

				u8 const* ptr新 = 新文件映射.GetPtr( commBeginPos , curlen );
				if( NULL == ptr新 )
					return FALSE ;

				剩余len -= curlen;

				s64 def_offset = commBeginPos - commbase;
				if( def_offset <= 0xFFFFFFFF )
				{
					if( 1 != IPS_File.Write( "\1" , 1 ) )
					{
						新文件映射.Free();
						m_MSG.show(_T("写出补丁失败(1),磁盘空间不足?"));
						return FALSE;
					}
					if( 4 != IPS_File.Write( &def_offset , 4 ) )
					{
						新文件映射.Free();
						m_MSG.show(_T("写出补丁失败(-0),磁盘空间不足?"));
						return FALSE;
					}
				}
				else
				{
					if( 1 != IPS_File.Write( "\2" , 1 ) )
					{
						新文件映射.Free();
						m_MSG.show(_T("写出补丁失败(1),磁盘空间不足?"));
						return FALSE;
					}
					if( 8 != IPS_File.Write( &commBeginPos , 8 ) )
					{
						新文件映射.Free();
						m_MSG.show(_T("写出补丁失败(-0),磁盘空间不足?"));
						return FALSE;
					}
				}
				if( 2 != IPS_File.Write( &curlen , 2 ) )
				{
					新文件映射.Free();
					m_MSG.show(_T("写出补丁失败(-1),磁盘空间不足?"));
					return FALSE;
				}
				if( curlen != IPS_File.Write( ptr新 , curlen ) )
				{
					新文件映射.Free();
					m_MSG.show(_T("写出补丁失败(-2),磁盘空间不足?"));
					return FALSE;
				}
				新文件映射.Free();
				commbase = commBeginPos;
				commBeginPos += curlen;
			}
		}
		IPS_File.Write("\377",1);
		return TRUE;
#undef DEF_FN_写出IPS
	}
public:
	CWQSG_IPS_OUT(HWND hwnd,TCHAR * 标题)
		:m_hwnd(hwnd)
		,m_标题(标题)
		,m_MSG(hwnd,标题)
	{}
	virtual	~CWQSG_IPS_OUT(){};
	BOOL 制作补丁30( TCHAR*const 原文件 , TCHAR*const 新文件 , TCHAR*const 补丁文件 , const s64 开始地址 ,//其中一个文件失效
		const s64 结束地址 ,//大于新文件,按照到结尾
		const s64 基础偏移 ,
		INT 是否使用CRC32 = 0 ,//非零验证
		WCHAR* 说明文本 = NULL ,
		DWORD 文本字数 = 0 ,
		CWD_BIN* EXE = NULL )
	{
		if( 开始地址 < 0 )
		{
			m_MSG.show(_T("开始地址 不能超过 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( 结束地址 < 0 )
		{
			m_MSG.show(_T("结束地址 不能超过 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( 结束地址 < 开始地址 )
		{
			m_MSG.show(_T("结束地址 不能比 开始地址小"));
			return FALSE;
		}

		if( 文本字数 > 65535 )
		{
			m_MSG.show(_T("说明文字 字数不能超过65535个"));
			return FALSE;
		}
		//打开原文件
		WQSG_File_mem 原文件映射 , 新文件映射 ;

		if( !原文件映射.OpenFile( 原文件 , 3 ) )
		{
			m_MSG.show(_T("原文件打开失败"));
			return FALSE;
		}
		//打开新文件
		if( !新文件映射.OpenFile( 新文件 , 3 ) )
		{
			m_MSG.show(_T("新文件打开失败"));
			return FALSE;
		}
		//判定开始地址 合法性
		s64 size1 = 原文件映射.GetFileSize( );
		const s64 size2 = 新文件映射.GetFileSize( );

		if( 开始地址 >= size1 )
		{
			m_MSG.show(_T("开始地址 不能比 原文件大\n没意义"));
			return FALSE;
		}
		if( 开始地址 >= size2 )
		{
			m_MSG.show(_T("开始地址 不能比 新文件大\n没意义"));
			return FALSE;
		}

		const s64 pos_begin = 开始地址;
		const s64 pos_end =  ( 结束地址 < size2 )?结束地址:size2 - 1;
		///////////////
		::CWQSG_File IPS_File;
		if( ! IPS_File.OpenFile( 补丁文件 ,(基础偏移==0)?4:2,3) )
		{
			m_MSG.show(_T("创建补丁文件失败"));
			return FALSE;
		}
		IPS_File.Seek(基础偏移);
		//验证CRC
		DWORD CRC32 = 0;
		if( 是否使用CRC32 != 0 )
		{
			if( !原文件映射.计算CRC32( CRC32 ) )
			{
				m_MSG.show(_T("计算CRC32失败"));
				return FALSE;
			}//
		}
		////////////////////////////////自解头
		u64 EXE偏移 = 0;
		if(EXE != NULL)
		{
			IPS_File.Write( EXE->BIN , EXE->LEN );
			EXE偏移 = EXE->LEN;
		}
		//写文标识
		if( 16 != IPS_File.Write("WQSG-PATCH3.0\0\0\0",16) )
		{
			m_MSG.show(_T("WIPS信息失败(1)"));
			return FALSE;
		}
		//写文件长度
		if( 8 != IPS_File.Write( &size2 , 8 ) )
		{
			m_MSG.show(_T("WIPS信息失败(2)"));
			return FALSE;
		}
		////写出CRC验证
		if( 4 != IPS_File.Write( &CRC32 ,4 ) )
		{
			m_MSG.show(_T("WIPS信息失败(3)"));
			return FALSE;
		}
		//写出数据偏移
		size1 = 0x28;
		if( 4 != IPS_File.Write( &size1 ,4 ) )
		{
			m_MSG.show(_T("WIPS信息失败(4)"));
			return FALSE;
		}
		//写出说明文本偏移
		if( 8 != IPS_File.Write( "\0\0\0\0\0\0\0\0" , 8 ) )
		{
			m_MSG.show(_T("WIPS信息失败(5)"));
			return FALSE;
		}
		///////////////////
		if( 正式制作30( 原文件映射  , 新文件映射 , IPS_File , pos_begin , pos_end ) )
		{
			if( 文本字数 )
			{
				const s64 pos = IPS_File.Tell( ) - EXE偏移;
				IPS_File.Write( 说明文本 , (文本字数<<1) );
				IPS_File.Write( "\0\0", 2 );
				IPS_File.Seek( EXE偏移 + 基础偏移 + 0x20 );
				IPS_File.Write( &pos , 8 );
			}
			m_MSG.show(_T("补丁制作完毕"));
			return TRUE;
		}
		return FALSE;
	}
};
class CWQSG_IPS_IN
{
	HWND			m_hwnd;
	TCHAR*			m_标题;
	::CWQSG_MSG_W	m_MSG;
	u8				m_buf[0xFFFF];
	BOOL 打补丁30( CWQSG_File& IPS_File , CWQSG_File& ROM_File)
	{
		DWORD 长度;
		s64 指针位置 = 0;
		while(TRUE)
		{
			if( !IPS_File.Read( m_buf , 1 ) )
			{
				m_MSG.show(_T("打补丁失败\n补丁文件已损坏(5)"));
				return FALSE;
			}
			switch(*m_buf)
			{
			case 0x01://相对偏移
				if( !IPS_File.Read( m_buf,4 ) )
				{
					m_MSG.show(_T("打补丁失败\n补丁文件已损坏(6.1)"));
					return FALSE;
				}
				指针位置 += *((u32*)m_buf );
				break;
			case 0x02://绝对偏移
				if( !IPS_File.Read( &指针位置 , 8 ) )
				{
					m_MSG.show(_T("打补丁失败\n补丁文件已损坏(6.2)"));
					return FALSE;
				}
				break;
			case 0xFF:
				return TRUE;
				break;
			default:
				m_MSG.show(_T("非法的WIPS补丁文件(2)"));
				return FALSE;
			}
			长度 = 0;

			if( !IPS_File.Read(&长度,2) )
			{
				m_MSG.show(_T("打补丁失败\n补丁文件已损坏(7)"));
				return FALSE;
			}

			if( !IPS_File.Read( m_buf , 长度 ) )
			{
				m_MSG.show(_T("打补丁失败\n补丁文件已损坏(8)"));
				return FALSE;
			}
			if( !ROM_File.Seek( 指针位置 ) )
			{
				m_MSG.show(_T("打补丁失败\n设置ROM文件位置失败"));
				return FALSE;
			}
			if( 长度 != ROM_File.Write( m_buf,长度 ) )
			{
				m_MSG.show(_T("打补丁失败\n写数据失败失败"));
				return FALSE;
			}
		}
	}
public:
	CWQSG_IPS_IN( HWND hwnd , TCHAR* 标题 )
		:m_hwnd(hwnd)
		,m_标题(标题)
		,m_MSG( hwnd , 标题 ){}
	virtual	~CWQSG_IPS_IN(){}
	BOOL 打补丁(TCHAR *const 补丁文件,TCHAR *const 目标文件)
	{
		::CWQSG_File IPS_File,ROM_File;
		if( !IPS_File.OpenFile(补丁文件,1,3))
		{
			m_MSG.show(_T("补丁文件打开失败"));
			return FALSE;
		}
		if( !ROM_File.OpenFile(目标文件,3,3))
		{
			m_MSG.show(_T("目标文件打开失败"));
			return FALSE;
		}

		if( !IPS_File.Read( m_buf,16) )
		{
			m_MSG.show(_T("非法的WIPS补丁文件"));
			return FALSE;
		}
		m_buf[16] = 0;
		int X,Y;
		if(2 != ::sscanf_s((CHAR*)m_buf,"WQSG-PATCH%d.%d",&X,&Y))
		{
			m_MSG.show(_T("非法的WIPS补丁文件"));
			return FALSE;
		}
		s64 文件大小;
		switch(X)
		{
		case 1:
			m_MSG.show(_T("暂不支持老版本的WIPS补丁文件"));
			return FALSE;
			break;
		case 2:
			m_MSG.show(_T("暂不支持老版本的WIPS补丁文件"));
			return FALSE;
			break;
		case 3:
			switch(Y)
			{
			case 0:
				if( !IPS_File.Read(&文件大小,8) )
				{
					m_MSG.show(_T("补丁文件已损坏(1)"));
					return FALSE;
				}
				//CRC
				{
					u32 CRC32;
					if( !IPS_File.Read(&CRC32,4) )
					{
						m_MSG.show(_T("补丁文件已损坏(2)"));
						return FALSE;
					}
					if(CRC32 != 0)
					{
						if(CRC32 != ROM_File.GetCRC32())
						{
							if(IDOK != m_MSG.show(_T("目标文件没有通过CRC32验证,要继续吗?"),MB_OKCANCEL))
							{
								return FALSE;
							}
						}
					}
				}
				//取数据指针
				{
					s64 数据指针 = 0;
					if( !IPS_File.Read(&数据指针,4) )
					{
						m_MSG.show(_T("补丁文件已损坏(3)"));
						return FALSE;
					}
					{
						const s64 size = IPS_File.GetFileSize();
						if( 数据指针 < 40 || size <= 数据指针 )
						{
							m_MSG.show(_T("补丁文件已损坏(4)"));
							return FALSE;
						}
					}
					if( !ROM_File.SetFileLength(文件大小))
					{
						m_MSG.show(_T("改变文件大小失败,可能是磁盘容量不足"));
						return FALSE;
					}
					if( ! IPS_File.Seek(数据指针) )
					{
						m_MSG.show(_T("设置文件指针错误(1)"));
						return FALSE;
					}
				}
				if(打补丁30( IPS_File , ROM_File) )
				{
					m_MSG.show(_T("补丁成功"));
					return TRUE;
				}
				return FALSE;
				break;
			default:
				m_MSG.show(_T("未知版本的WIPS补丁文件"));
				return FALSE;
			}
			break;
		default:
			m_MSG.show(_T("未知版本的WIPS补丁文件"));
			return FALSE;
		}
	}
	BOOL 取文件补丁信息(TCHAR *const 补丁文件,CWD_WIPS信息* WIPS信息结构)
	{
		WIPS信息结构->清空();
		::CWQSG_File IPS_File;
		if(! IPS_File.OpenFile(补丁文件,1,3))
		{
			return FALSE;
		}

		if( !IPS_File.Read( m_buf,16) )
		{
			return FALSE;
		}
		m_buf[16] = 0;
		int X,Y;
		if(2 != ::sscanf_s((CHAR*)m_buf,"WQSG-PATCH%d.%d",&X,&Y))
		{
			return FALSE;
		}
#ifdef UNICODE
		WCHAR *const tmp = ::WQSG_char_W( (char*)m_buf);
		::WQSG_strcpy(tmp,WIPS信息结构->补丁标识);
		delete[]tmp;
#else
		::WQSG_strcpy(W_buf,WIPS信息结构->补丁标识);
#endif
		s64 文件大小;
		DWORD CRC32;
		s64 size;
		switch(X)
		{
		case 1:
			WIPS信息结构->清空();
			return FALSE;
			break;
		case 2:
			WIPS信息结构->清空();
			return FALSE;
			break;
		case 3:
			switch(Y)
			{
			case 0:
				if( !IPS_File.Read(&WIPS信息结构->目标大小,8) )
				{
					WIPS信息结构->清空();
					return FALSE;
				}

				if( !IPS_File.Read(&WIPS信息结构->验证CRC32,4) )
				{
					WIPS信息结构->清空();
					return FALSE;
				}
				//取数据指针
				if( !IPS_File.Read(&CRC32,4) )
				{
					WIPS信息结构->清空();
					return FALSE;
				}
				size = IPS_File.GetFileSize();
				if(CRC32 < 40
					|| size <= CRC32)
				{
					WIPS信息结构->清空();
					return FALSE;
				}
				if( !IPS_File.Read(&文件大小,8) )
				{
					WIPS信息结构->清空();
					return FALSE;
				}
				if(文件大小)
				{
					IPS_File.Seek(文件大小);
					IPS_File.Read(WIPS信息结构->说明文本,65536);
				}
				return TRUE;
				break;
			default:
				WIPS信息结构->清空();
				return FALSE;
			}
			break;
		default:
			WIPS信息结构->清空();
			return FALSE;
		}
	}
};
#endif

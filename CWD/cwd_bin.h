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
#include "WQSG_def.h"
#include <windows.h>
__i__
__i__
__i__
__i__
class CWD_BIN
{
public:
	u8*		BIN;
	u32		LEN;
	inline	CWD_BIN(void):BIN(NULL),LEN(0){}
	virtual	inline	~CWD_BIN(void){delete[]BIN;}
	inline	u8*	SetBufSize( u32 size )
	{
		delete[]BIN;
		return ( BIN = new u8[size] );
	}
};
//------------------------------------------------------------------------------
class WD_TXTW
{
	WCHAR*	m_TXT;
public:
	WD_TXTW():m_TXT(NULL){}
	virtual	~WD_TXTW(){delete[]m_TXT;}
	WCHAR* operator	=( WCHAR* TXT ){		delete[]m_TXT;		m_TXT = TXT;		return m_TXT;	}
	void	脱离(){		m_TXT = NULL;	}
	operator WCHAR*	(){		return m_TXT;	}
};
//------------------------------------------------------------------
#ifdef __WD_str_LB_W__
class WD_str_LB_W{
#define WQSG_BUFlen 512
	WCHAR*		TXT;
	WD_str_LB_W*	next;
	void z_Copy(WCHAR*s){
		s += WQSG_strcpy(TXT,s);
		if(next)
			next->z_Copy(s);
	}
public:
	WD_str_LB_W(){
		TXT = new WCHAR[WQSG_BUFlen + 1];
		next = NULL;
	}
	~WD_str_LB_W(){
		delete TXT;
		if(next)
			delete next;
	}
	INT ReadTXTLine(HANDLE FILE_ID)//从文件读入文本,缓冲区满 返回 ,遇到 换行 或 文件结束 返回 WD_str_LB*,缓冲区 为0,返回NULL;
	{
		WCHAR C[1],*TXT_BUF = TXT;
		DWORD con_i;INT i;
		for(i = 0 ; i < WQSG_BUFlen ; i+=2 )
		{
			if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)//正常读
			{
				if(2 != con_i)
					break;
				if(0x0D == *C)//判断是否换行,是就返回
				{
					i+=4;
					if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)////////////////////////////////////////正常读
					{
						if(0x0A != *C)//非换行后缀
						{
							::SetFilePointer(FILE_ID,-2,NULL,FILE_CURRENT);
							i-=2;
						}
					}
					else//读入失败
					{
						i-=2;
					}
					*TXT_BUF = 0;
					if(i>0)//缓冲区有内容
						return i;
					return 0;//无内容,返回假
				}
				*(TXT_BUF++) = *C;
			}////////////////////////////////////////////////////////////////////////////////正常读 结束
			else/////////////////////////////////////////读失败,可能是文件结束
			{
				*TXT_BUF = 0;
				if(i>0)//缓冲区有内容
					return i;
				return 0;//无内容,返回假
			}///////////////////////////////////////////读失败 结束
		}
		////////////////////////////////////////////缓冲区已满
		*TXT_BUF = 0;
		next = new WD_str_LB_W;
		int x =	next->ReadTXTLine(FILE_ID);
		if(!x)
		{
			delete next;
			next=NULL;
		}
		return (i+x);
	}

	UINT Getlen()
	{
		UINT i = WQSG_strlen(TXT);
		if(next)
			i += next->Getlen();
		return i;
	}
	WCHAR*GetTXT()
	{
		WCHAR*out_0=new WCHAR[Getlen() + 1];
		z_Copy(out_0);
		return out_0;
	}
#undef WQSG_BUFlen// 512
};
#endif
#if 0
//------------------------------------------------------------------
class 节点_TXT_W
{
public:
	WCHAR*			m_TXT_W;
	节点_TXT_W*		m_next[256];
public:
	节点_TXT_W()
	{
		m_TXT_W = NULL;
		::memset(m_next,0,sizeof(m_next));
	}
	virtual	~节点_TXT_W()
	{
		delete[]m_TXT_W;
		for( INT I = 0 ; I <= 255 ; ++I )
			delete m_next[I];
	}
};
#endif
//------------------------------------------------------------------
template <typename TYPE_1>
class CWD_tree_del
{
	class __Node
	{
	public:
		TYPE_1*	m_Type;
		__Node*	m_next[256];
		///////////////////////////////////////////////////////////////////
		inline __Node(): m_Type ( NULL ){	::memset( m_next , 0 , sizeof(m_next) );}
		virtual	inline ~__Node(){	delete m_Type;	for( INT I = 0 ; I <= 255 ; ++I )delete m_next[I];	}
	};
	__Node*	m_ROOB[256];
public:
	inline	CWD_tree_del(){	::memset( m_ROOB , 0 , sizeof(m_ROOB) );	}
	virtual	inline	~CWD_tree_del()	{		for( INT k = 0 ; k <= 255 ; ++k )delete m_ROOB[k];	}
	///////////////////////////////////////////////////////////////////
	inline	void		清空()
	{
		for(INT k = 0;k <= 255;++k)
			delete m_ROOB[k];
		::memset(m_ROOB,0,sizeof(m_ROOB));
	}
	inline	BOOL		压入( UCHAR* 索引 , INT 索引长度 , TYPE_1* 添加的Type )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return FALSE;

		__Node** 节点树 = m_ROOB;
		__Node* tmp;
		INT x;

		while(索引长度--)
		{
			x = *(索引++);
			tmp = 节点树[x];
			if(tmp)
			{//存在,继续搜索下一个
				节点树 = tmp->m_next;
			}
			else
			{//不存在,创建新的 节点
				tmp = new __Node;				//::memset(tmp,0,sizeof(节点_TXT_W));
				节点树[x] = tmp;
				节点树 = tmp->m_next;
			}
		}
		tmp->m_Type = 添加的Type;

		return TRUE;
	}
	inline	TYPE_1*		匹配( u8* 索引 , UINT 索引长度 , UINT& 实际匹配长度 )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return NULL;

		实际匹配长度 = 索引长度;
		__Node** 节点树 = m_ROOB;
		__Node* tmp;

		u32 xlen = 0;
		TYPE_1*	xtmp = NULL;
		while(索引长度--)
		{
			tmp = 节点树[*(索引++)];
			if(tmp)
			{//存在下一层
				if( tmp ->m_Type )
				{
					xtmp = tmp ->m_Type;
					xlen = 索引长度;
				}
				节点树 = tmp->m_next;
			}
			else if( xtmp )//不存在下一层
				索引长度 = --xlen;
			else
				break;
		}
		实际匹配长度 -= (索引长度 + 1);
		return xtmp;
	}
	inline	TYPE_1*		索引( u8* 索引 , INT 索引长度 )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return NULL;

		__Node** 节点树 = m_ROOB;
		__Node* tmp;

		while(索引长度--)
		{
			tmp = 节点树[*(索引++)];
			if(tmp)//存在下一层
				节点树 = tmp->m_next;
			else//不存在下一层
				return NULL;
		}
		return tmp->m_Type;
	}
};
//------------------------------------------------------------------
template <typename TYPE_1>
class CWD_tree
{
	class __Node
	{
	public:
		TYPE_1	m_Type;
		__Node*	m_next[256];
		///////////////////////////////////////////////////////////////////
		inline __Node(): m_Type ( 0 ){::memset( m_next , 0 , sizeof(m_next) );}
		virtual	inline ~__Node()	{	for( INT I = 0 ; I <= 255 ; ++I )delete m_next[I];		}
	};
	__Node*	m_ROOB[256];
public:
	inline	CWD_tree(){	::memset( m_ROOB , 0 , sizeof(m_ROOB) );	}
	virtual	inline	~CWD_tree()	{		for( INT k = 0 ; k <= 255 ; ++k )delete m_ROOB[k];	}
	///////////////////////////////////////////////////////////////////
	inline	void		清空()
	{
		for(INT k = 0;k <= 255;++k)
			delete m_ROOB[k];
		::memset(m_ROOB,0,sizeof(m_ROOB));
	}
	inline	BOOL		压入( UCHAR* 索引 , INT 索引长度 , TYPE_1 添加的Type )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return FALSE;

		__Node** 节点树 = m_ROOB;
		__Node* tmp;
		INT x;

		while(索引长度--)
		{
			x = *(索引++);
			tmp = 节点树[x];
			if(tmp)
			{//存在,继续搜索下一个
				节点树 = tmp->m_next;
			}
			else
			{//不存在,创建新的 节点
				tmp = new __Node;				//::memset(tmp,0,sizeof(节点_TXT_W));
				节点树[x] = tmp;
				节点树 = tmp->m_next;
			}
		}
		tmp->m_Type = 添加的Type;

		return TRUE;
	}
	inline	TYPE_1		匹配( u8* 索引 , INT 索引长度 , INT& 实际匹配长度 )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return NULL;

		实际匹配长度 = 索引长度;
		__Node** 节点树 = m_ROOB,
		__Node* tmp;

		u32 xlen = 0;
		TYPE_1	xtmp = 0;
		while(索引长度--)
		{
			tmp = 节点树[*(索引++)];
			if(tmp)
			{//存在下一层
				if( tmp ->m_Type )
				{
					xtmp = tmp ->m_Type;
					xlen = 索引长度;
				}
				节点树 = tmp->m_next;
			}
			else if( xtmp )//不存在下一层
				索引长度 = --xlen;
			else
				break;
		}
		实际匹配长度 -= (索引长度 + 1);
		return xtmp;
	}
	inline	TYPE_1		索引( u8* 索引 , INT 索引长度 )
	{
		if( ( 索引长度 <= 0 ) || ( 索引 == NULL ) )
			return NULL;

		__Node** 节点树 = m_ROOB;
		__Node* tmp;

		while(索引长度--)
		{
			tmp = 节点树[*(索引++)];
			if(tmp)//存在下一层
				节点树 = tmp->m_next;
			else//不存在下一层
				return 0;
		}
		return tmp->m_Type;
	}
};
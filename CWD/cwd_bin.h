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
	void	����(){		m_TXT = NULL;	}
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
	INT ReadTXTLine(HANDLE FILE_ID)//���ļ������ı�,�������� ���� ,���� ���� �� �ļ����� ���� WD_str_LB*,������ Ϊ0,����NULL;
	{
		WCHAR C[1],*TXT_BUF = TXT;
		DWORD con_i;INT i;
		for(i = 0 ; i < WQSG_BUFlen ; i+=2 )
		{
			if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)//������
			{
				if(2 != con_i)
					break;
				if(0x0D == *C)//�ж��Ƿ���,�Ǿͷ���
				{
					i+=4;
					if(::ReadFile(FILE_ID,C,2,&con_i,NULL) && 2 == con_i)////////////////////////////////////////������
					{
						if(0x0A != *C)//�ǻ��к�׺
						{
							::SetFilePointer(FILE_ID,-2,NULL,FILE_CURRENT);
							i-=2;
						}
					}
					else//����ʧ��
					{
						i-=2;
					}
					*TXT_BUF = 0;
					if(i>0)//������������
						return i;
					return 0;//������,���ؼ�
				}
				*(TXT_BUF++) = *C;
			}////////////////////////////////////////////////////////////////////////////////������ ����
			else/////////////////////////////////////////��ʧ��,�������ļ�����
			{
				*TXT_BUF = 0;
				if(i>0)//������������
					return i;
				return 0;//������,���ؼ�
			}///////////////////////////////////////////��ʧ�� ����
		}
		////////////////////////////////////////////����������
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
class �ڵ�_TXT_W
{
public:
	WCHAR*			m_TXT_W;
	�ڵ�_TXT_W*		m_next[256];
public:
	�ڵ�_TXT_W()
	{
		m_TXT_W = NULL;
		::memset(m_next,0,sizeof(m_next));
	}
	virtual	~�ڵ�_TXT_W()
	{
		delete[]m_TXT_W;
		for( INT I = 0 ; I <= 255 ; ++I )
			delete m_next[I];
	}
};
#endif
//------------------------------------------------------------------

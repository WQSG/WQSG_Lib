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
//3.0							   ռ���ֽ�
/*****************************************************************************************
0x00	*IPS�ļ�ͷ					*	13	*	WQSG-PATCH3.0								(ASCII) 
0x0D	*							*	3	*	(����,��Ϊ0)								(HEX)
0x10	*���ļ�����					*	8	*												(HEX)
0x18	*ԭ�ļ���CRC32				*	4	*	ԭ�ļ���CRC32								(HEX)
0x1C	*���������ڱ��ļ���ʼλ��	*	4	*	����� 0x00��ƫ��							(HEX)
0x20	*˵���ı��ڱ��ļ��е�ƫ��	*	8	*	����� 0x00��ƫ��,�����0��β				(HEX)
0x28	*��־						*	1	*	0x0 δ����,									(HEX)
												0x1 ���ӵ�ַΪ4�ֽڵ����ƫ��,
												0x2 ���ӵ�ַΪ 8�ֽڵľ��Ե�ַ
												0xFF �ļ�����
0x?1	*�������ݵ�ַ				*	4(8)*	�������һ����ַ��ƫ����(���Ե�ַ)			(HEX)
0x?2	*�������ݳ���				*	2	*	���� �������� �ĳ���						(HEX)
0x?2+2	*��������					*	?	*	������󳤶Ȳ��ܳ�������涨��				(ԭHEX)
		*.........					*	...	*	�ظ���������,ֱ���������ݽ���
0x?3	*˵�����ı�					*	?	*  UNICODE �����ʽ,���ı�������־����,���п��� (UNICODE)
//****************************************************************************************/
#include"WQSG_xFile.h"
#include"WQSG_File_M.h"

#include<windows.h>
#include<stdio.h>
#include<tchar.h>

class CWD_WIPS��Ϣ
{
public:
	TCHAR ������ʶ[17];
	WCHAR ˵���ı�[0x10000];
	s64 Ŀ���С;
	DWORD ��֤CRC32;
	CWD_WIPS��Ϣ()
	{
		���();
	}
	virtual	~CWD_WIPS��Ϣ(){}
	void ���()
	{
		Ŀ���С = 0;
		��֤CRC32 = 0;
		*������ʶ = 0;
		*˵���ı� = 0;
	}
};
class CWQSG_IPS_OUT
{
	HWND			m_hwnd;
	TCHAR*			m_����;
	::CWQSG_MSG_W	m_MSG;
	BOOL ��ʽ����30( WQSG_File_mem& ԭ�ļ�ӳ�� , WQSG_File_mem& ���ļ�ӳ�� , ::CWQSG_File& IPS_File ,
		const s64 pos_begin , const s64 pos_end )
	{
		if( pos_begin < 0 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( pos_end < 0 )
		{
			m_MSG.show(_T("������ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}

		const s64 ����posԭ = ԭ�ļ�ӳ��.GetFileSize( ) - 1;

		const s64 ����pos�� = ���ļ�ӳ��.GetFileSize( ) - 1;

		if( pos_begin > pos_end )
		{
			m_MSG.show(_T("������ַ���ܴ��ڿ�ʼ��ַ"));
			return FALSE;
		}
		if( ((u64)pos_begin>(u64)����posԭ) || ((u64)pos_begin>(u64)����pos��) )
		{
			m_MSG.show(_T("�����ڹ�����ʼ��ַ"));
			return FALSE;
		}

		CWD_BIN binbuf;
		if( NULL == binbuf.SetBufSize( 0xFFFF ) )
			return FALSE;

		s64 commbase = 0;
		s64 commBeginPos = pos_begin;
		s64 commEndPos = (����pos�� < ����posԭ )?(����pos��):(����posԭ);
		commEndPos = ( commEndPos < pos_end )?commEndPos:pos_end;

		//��һ��,������ͬ��������
		{
			s64 ���޵�ַ = commEndPos + 1;
			//֤��ֻ��Ҫ�㶨 ���ļ�
			s64 ʣ��len;
			while( (ʣ��len = (���޵�ַ - commBeginPos)) > 0 )
			{
				const u32 curlen = ( ʣ��len > 0x2000000 )?0x2000000:(u32)ʣ��len;
				ʣ��len -= curlen;

				{
					u8 const* ptrԭ = ԭ�ļ�ӳ��.GetPtr( commBeginPos , curlen );
					if( NULL == ptrԭ )
						return FALSE ;

					u8 const* ptr�� = ���ļ�ӳ��.GetPtr( commBeginPos , curlen );
					if( NULL == ptr�� )
						return FALSE ;
					//--------------------------------------------------------------
					binbuf.LEN = 0;
					u8 const*const ptr���� = ptrԭ + curlen;
					u8* buf = binbuf.BIN;

					while( ptrԭ < ptr���� )
					{
						if( 0x647F890 == commBeginPos )
						{
							int x = 0;
						}
						if( (*ptrԭ) != (*ptr��) )
						{
							*(buf++) = *ptr��;
							if( ++(binbuf.LEN) == 0xFFFF )
							{
								break;
							}
							++ptrԭ , ++ptr��;
						}
						else if( binbuf.LEN )
						{
							break;
						}
						else
						{
							++ptrԭ , ++ptr��;

							if( ʣ��len && (( ptr���� - ptrԭ ) < 0xFFFF ) )
							{
								break;
							}
							++commBeginPos;
						}
					}
					ԭ�ļ�ӳ��.Free();���ļ�ӳ��.Free();
				}
				if( binbuf.LEN )
				{
					s64 def_offset = commBeginPos - commbase;
					if( def_offset <= 0xFFFFFFFF )
					{
						if( 1 != IPS_File.Write( "\1" , 1 ) )
						{
							m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
							return FALSE;
						}
						if( 4 != IPS_File.Write( &def_offset , 4 ) )
						{
							m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
							return FALSE;
						}
					}
					else
					{
						if( 1 != IPS_File.Write( "\2" , 1 ) )
						{
							m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
							return FALSE;
						}
						if( 8 != IPS_File.Write( &commBeginPos , 8 ) )
						{
							m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
							return FALSE;
						}
					}
					if( 2 != IPS_File.Write( &binbuf.LEN , 2 ) )
					{
						m_MSG.show(_T("д������ʧ��(-1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( binbuf.LEN != IPS_File.Write( binbuf.BIN , binbuf.LEN ) )
					{
						m_MSG.show(_T("д������ʧ��(-2),���̿ռ䲻��?"))
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
		//�ڶ���,���ļ�����
		if( ����pos�� > ����posԭ )
		{
			commBeginPos = ����posԭ + 1;
			s64 ʣ��len = ����pos�� - commBeginPos + 1;
			while( ʣ��len )
			{
				const u32 curlen = ( ʣ��len > 0xFFFF )?0xFFFF:(u32)ʣ��len;

				u8 const* ptr�� = ���ļ�ӳ��.GetPtr( commBeginPos , curlen );
				if( NULL == ptr�� )
					return FALSE ;

				ʣ��len -= curlen;

				s64 def_offset = commBeginPos - commbase;
				if( def_offset <= 0xFFFFFFFF )
				{
					if( 1 != IPS_File.Write( "\1" , 1 ) )
					{
						���ļ�ӳ��.Free();
						m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( 4 != IPS_File.Write( &def_offset , 4 ) )
					{
						���ļ�ӳ��.Free();
						m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
						return FALSE;
					}
				}
				else
				{
					if( 1 != IPS_File.Write( "\2" , 1 ) )
					{
						���ļ�ӳ��.Free();
						m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( 8 != IPS_File.Write( &commBeginPos , 8 ) )
					{
						���ļ�ӳ��.Free();
						m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
						return FALSE;
					}
				}
				if( 2 != IPS_File.Write( &curlen , 2 ) )
				{
					���ļ�ӳ��.Free();
					m_MSG.show(_T("д������ʧ��(-1),���̿ռ䲻��?"));
					return FALSE;
				}
				if( curlen != IPS_File.Write( ptr�� , curlen ) )
				{
					���ļ�ӳ��.Free();
					m_MSG.show(_T("д������ʧ��(-2),���̿ռ䲻��?"));
					return FALSE;
				}
				���ļ�ӳ��.Free();
				commbase = commBeginPos;
				commBeginPos += curlen;
			}
		}
		IPS_File.Write("\377",1);
		return TRUE;
#undef DEF_FN_д��IPS
	}
public:
	CWQSG_IPS_OUT(HWND hwnd,TCHAR * ����)
		:m_hwnd(hwnd)
		,m_����(����)
		,m_MSG(hwnd,����)
	{}
	virtual	~CWQSG_IPS_OUT(){};
	BOOL ��������30( TCHAR*const ԭ�ļ� , TCHAR*const ���ļ� , TCHAR*const �����ļ� , const s64 ��ʼ��ַ ,//����һ���ļ�ʧЧ
		const s64 ������ַ ,//�������ļ�,���յ���β
		const s64 ����ƫ�� ,
		INT �Ƿ�ʹ��CRC32 = 0 ,//������֤
		WCHAR* ˵���ı� = NULL ,
		DWORD �ı����� = 0 ,
		CWD_BIN* EXE = NULL )
	{
		if( ��ʼ��ַ < 0 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( ������ַ < 0 )
		{
			m_MSG.show(_T("������ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( ������ַ < ��ʼ��ַ )
		{
			m_MSG.show(_T("������ַ ���ܱ� ��ʼ��ַС"));
			return FALSE;
		}

		if( �ı����� > 65535 )
		{
			m_MSG.show(_T("˵������ �������ܳ���65535��"));
			return FALSE;
		}
		//��ԭ�ļ�
		WQSG_File_mem ԭ�ļ�ӳ�� , ���ļ�ӳ�� ;

		if( !ԭ�ļ�ӳ��.OpenFile( ԭ�ļ� , 3 ) )
		{
			m_MSG.show(_T("ԭ�ļ���ʧ��"));
			return FALSE;
		}
		//�����ļ�
		if( !���ļ�ӳ��.OpenFile( ���ļ� , 3 ) )
		{
			m_MSG.show(_T("���ļ���ʧ��"));
			return FALSE;
		}
		//�ж���ʼ��ַ �Ϸ���
		s64 size1 = ԭ�ļ�ӳ��.GetFileSize( );
		const s64 size2 = ���ļ�ӳ��.GetFileSize( );

		if( ��ʼ��ַ >= size1 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܱ� ԭ�ļ���\nû����"));
			return FALSE;
		}
		if( ��ʼ��ַ >= size2 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܱ� ���ļ���\nû����"));
			return FALSE;
		}

		const s64 pos_begin = ��ʼ��ַ;
		const s64 pos_end =  ( ������ַ < size2 )?������ַ:size2 - 1;
		///////////////
		::CWQSG_File IPS_File;
		if( ! IPS_File.OpenFile( �����ļ� ,(����ƫ��==0)?4:2,3) )
		{
			m_MSG.show(_T("���������ļ�ʧ��"));
			return FALSE;
		}
		IPS_File.Seek(����ƫ��);
		//��֤CRC
		DWORD CRC32 = 0;
		if( �Ƿ�ʹ��CRC32 != 0 )
		{
			if( !ԭ�ļ�ӳ��.����CRC32( CRC32 ) )
			{
				m_MSG.show(_T("����CRC32ʧ��"));
				return FALSE;
			}//
		}
		////////////////////////////////�Խ�ͷ
		u64 EXEƫ�� = 0;
		if(EXE != NULL)
		{
			IPS_File.Write( EXE->BIN , EXE->LEN );
			EXEƫ�� = EXE->LEN;
		}
		//д�ı�ʶ
		if( 16 != IPS_File.Write("WQSG-PATCH3.0\0\0\0",16) )
		{
			m_MSG.show(_T("WIPS��Ϣʧ��(1)"));
			return FALSE;
		}
		//д�ļ�����
		if( 8 != IPS_File.Write( &size2 , 8 ) )
		{
			m_MSG.show(_T("WIPS��Ϣʧ��(2)"));
			return FALSE;
		}
		////д��CRC��֤
		if( 4 != IPS_File.Write( &CRC32 ,4 ) )
		{
			m_MSG.show(_T("WIPS��Ϣʧ��(3)"));
			return FALSE;
		}
		//д������ƫ��
		size1 = 0x28;
		if( 4 != IPS_File.Write( &size1 ,4 ) )
		{
			m_MSG.show(_T("WIPS��Ϣʧ��(4)"));
			return FALSE;
		}
		//д��˵���ı�ƫ��
		if( 8 != IPS_File.Write( "\0\0\0\0\0\0\0\0" , 8 ) )
		{
			m_MSG.show(_T("WIPS��Ϣʧ��(5)"));
			return FALSE;
		}
		///////////////////
		if( ��ʽ����30( ԭ�ļ�ӳ��  , ���ļ�ӳ�� , IPS_File , pos_begin , pos_end ) )
		{
			if( �ı����� )
			{
				const s64 pos = IPS_File.Tell( ) - EXEƫ��;
				IPS_File.Write( ˵���ı� , (�ı�����<<1) );
				IPS_File.Write( "\0\0", 2 );
				IPS_File.Seek( EXEƫ�� + ����ƫ�� + 0x20 );
				IPS_File.Write( &pos , 8 );
			}
			m_MSG.show(_T("�����������"));
			return TRUE;
		}
		return FALSE;
	}
};
class CWQSG_IPS_IN
{
	HWND			m_hwnd;
	TCHAR*			m_����;
	::CWQSG_MSG_W	m_MSG;
	u8				m_buf[0xFFFF];
	BOOL �򲹶�30( CWQSG_File& IPS_File , CWQSG_File& ROM_File)
	{
		DWORD ����;
		s64 ָ��λ�� = 0;
		while(TRUE)
		{
			if( !IPS_File.Read( m_buf , 1 ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(5)"));
				return FALSE;
			}
			switch(*m_buf)
			{
			case 0x01://���ƫ��
				if( !IPS_File.Read( m_buf,4 ) )
				{
					m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(6.1)"));
					return FALSE;
				}
				ָ��λ�� += *((u32*)m_buf );
				break;
			case 0x02://����ƫ��
				if( !IPS_File.Read( &ָ��λ�� , 8 ) )
				{
					m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(6.2)"));
					return FALSE;
				}
				break;
			case 0xFF:
				return TRUE;
				break;
			default:
				m_MSG.show(_T("�Ƿ���WIPS�����ļ�(2)"));
				return FALSE;
			}
			���� = 0;

			if( !IPS_File.Read(&����,2) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(7)"));
				return FALSE;
			}

			if( !IPS_File.Read( m_buf , ���� ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(8)"));
				return FALSE;
			}
			if( !ROM_File.Seek( ָ��λ�� ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n����ROM�ļ�λ��ʧ��"));
				return FALSE;
			}
			if( ���� != ROM_File.Write( m_buf,���� ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\nд����ʧ��ʧ��"));
				return FALSE;
			}
		}
	}
public:
	CWQSG_IPS_IN( HWND hwnd , TCHAR* ���� )
		:m_hwnd(hwnd)
		,m_����(����)
		,m_MSG( hwnd , ���� ){}
	virtual	~CWQSG_IPS_IN(){}
	BOOL �򲹶�(TCHAR *const �����ļ�,TCHAR *const Ŀ���ļ�)
	{
		::CWQSG_File IPS_File,ROM_File;
		if( !IPS_File.OpenFile(�����ļ�,1,3))
		{
			m_MSG.show(_T("�����ļ���ʧ��"));
			return FALSE;
		}
		if( !ROM_File.OpenFile(Ŀ���ļ�,3,3))
		{
			m_MSG.show(_T("Ŀ���ļ���ʧ��"));
			return FALSE;
		}

		if( !IPS_File.Read( m_buf,16) )
		{
			m_MSG.show(_T("�Ƿ���WIPS�����ļ�"));
			return FALSE;
		}
		m_buf[16] = 0;
		int X,Y;
		if(2 != ::sscanf_s((CHAR*)m_buf,"WQSG-PATCH%d.%d",&X,&Y))
		{
			m_MSG.show(_T("�Ƿ���WIPS�����ļ�"));
			return FALSE;
		}
		s64 �ļ���С;
		switch(X)
		{
		case 1:
			m_MSG.show(_T("�ݲ�֧���ϰ汾��WIPS�����ļ�"));
			return FALSE;
			break;
		case 2:
			m_MSG.show(_T("�ݲ�֧���ϰ汾��WIPS�����ļ�"));
			return FALSE;
			break;
		case 3:
			switch(Y)
			{
			case 0:
				if( !IPS_File.Read(&�ļ���С,8) )
				{
					m_MSG.show(_T("�����ļ�����(1)"));
					return FALSE;
				}
				//CRC
				{
					u32 CRC32;
					if( !IPS_File.Read(&CRC32,4) )
					{
						m_MSG.show(_T("�����ļ�����(2)"));
						return FALSE;
					}
					if(CRC32 != 0)
					{
						if(CRC32 != ROM_File.GetCRC32())
						{
							if(IDOK != m_MSG.show(_T("Ŀ���ļ�û��ͨ��CRC32��֤,Ҫ������?"),MB_OKCANCEL))
							{
								return FALSE;
							}
						}
					}
				}
				//ȡ����ָ��
				{
					s64 ����ָ�� = 0;
					if( !IPS_File.Read(&����ָ��,4) )
					{
						m_MSG.show(_T("�����ļ�����(3)"));
						return FALSE;
					}
					{
						const s64 size = IPS_File.GetFileSize();
						if( ����ָ�� < 40 || size <= ����ָ�� )
						{
							m_MSG.show(_T("�����ļ�����(4)"));
							return FALSE;
						}
					}
					if( !ROM_File.SetFileLength(�ļ���С))
					{
						m_MSG.show(_T("�ı��ļ���Сʧ��,�����Ǵ�����������"));
						return FALSE;
					}
					if( ! IPS_File.Seek(����ָ��) )
					{
						m_MSG.show(_T("�����ļ�ָ�����(1)"));
						return FALSE;
					}
				}
				if(�򲹶�30( IPS_File , ROM_File) )
				{
					m_MSG.show(_T("�����ɹ�"));
					return TRUE;
				}
				return FALSE;
				break;
			default:
				m_MSG.show(_T("δ֪�汾��WIPS�����ļ�"));
				return FALSE;
			}
			break;
		default:
			m_MSG.show(_T("δ֪�汾��WIPS�����ļ�"));
			return FALSE;
		}
	}
	BOOL ȡ�ļ�������Ϣ(TCHAR *const �����ļ�,CWD_WIPS��Ϣ* WIPS��Ϣ�ṹ)
	{
		WIPS��Ϣ�ṹ->���();
		::CWQSG_File IPS_File;
		if(! IPS_File.OpenFile(�����ļ�,1,3))
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
		::WQSG_strcpy(tmp,WIPS��Ϣ�ṹ->������ʶ);
		delete[]tmp;
#else
		::WQSG_strcpy(W_buf,WIPS��Ϣ�ṹ->������ʶ);
#endif
		s64 �ļ���С;
		DWORD CRC32;
		s64 size;
		switch(X)
		{
		case 1:
			WIPS��Ϣ�ṹ->���();
			return FALSE;
			break;
		case 2:
			WIPS��Ϣ�ṹ->���();
			return FALSE;
			break;
		case 3:
			switch(Y)
			{
			case 0:
				if( !IPS_File.Read(&WIPS��Ϣ�ṹ->Ŀ���С,8) )
				{
					WIPS��Ϣ�ṹ->���();
					return FALSE;
				}

				if( !IPS_File.Read(&WIPS��Ϣ�ṹ->��֤CRC32,4) )
				{
					WIPS��Ϣ�ṹ->���();
					return FALSE;
				}
				//ȡ����ָ��
				if( !IPS_File.Read(&CRC32,4) )
				{
					WIPS��Ϣ�ṹ->���();
					return FALSE;
				}
				size = IPS_File.GetFileSize();
				if(CRC32 < 40
					|| size <= CRC32)
				{
					WIPS��Ϣ�ṹ->���();
					return FALSE;
				}
				if( !IPS_File.Read(&�ļ���С,8) )
				{
					WIPS��Ϣ�ṹ->���();
					return FALSE;
				}
				if(�ļ���С)
				{
					IPS_File.Seek(�ļ���С);
					IPS_File.Read(WIPS��Ϣ�ṹ->˵���ı�,65536);
				}
				return TRUE;
				break;
			default:
				WIPS��Ϣ�ṹ->���();
				return FALSE;
			}
			break;
		default:
			WIPS��Ϣ�ṹ->���();
			return FALSE;
		}
	}
};
#endif

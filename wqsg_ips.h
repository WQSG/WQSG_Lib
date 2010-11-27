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
0x?3	*˵�����ı�					*	?	*  utf16le �����ʽ,���ı�������־����,���п��� (UNICODE)
//****************************************************************************************/
#include "WQSG_xFile.h"
#include "WQSG_File_M.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

class CWD_WipsInfo
{
public:
	TCHAR ������ʶ[17];
	WCHAR m_szDesc[0x10000];
	s64 Ŀ���С;
	u32 m_uCrc32;
	CWD_WipsInfo()
	{
		Clear();
	}
	virtual	~CWD_WipsInfo(){}
	void Clear()
	{
		Ŀ���С = 0;
		m_uCrc32 = 0;
		*������ʶ = 0;
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

		u8 buf[0xFFFF];

		if( !IPS_File.Read( buf , 16 ) )
		{
			return FALSE;
		}

		buf[16] = 0;
		int X , Y;
		if(2 != ::sscanf_s( (CHAR*)buf , "WQSG-PATCH%d.%d" , &X , &Y ) )
		{
			return FALSE;
		}
#ifdef UNICODE
		WCHAR *const tmp = ::WQSG_char_W( (char*)buf );
		::WQSG_strcpy( tmp , ������ʶ );
		delete[]tmp;
#else
		::WQSG_strcpy( buf , ������ʶ );
#endif
		s64 �ļ���С;
		DWORD CRC32;
		s64 size;

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
				if( !IPS_File.Read( &Ŀ���С , 8 ) )
				{
					Clear();
					break;
				}

				if( !IPS_File.Read( &m_uCrc32 , 4 ) )
				{
					Clear();
					break;
				}

				//ȡ����ָ��
				if( !IPS_File.Read(&CRC32,4) )
				{
					Clear();
					break;
				}

				size = IPS_File.GetFileSize();
				if( CRC32 < 40
					|| size <= CRC32 )
				{
					Clear();
					break;
				}

				if( !IPS_File.Read(&�ļ���С,8) )
				{
					Clear();
					break;
				}
				if(�ļ���С)
				{
					IPS_File.Seek( a_nBaseOffset + �ļ���С );
					IPS_File.Read( m_szDesc , 65536 );
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

class CWQSG_IPS_OUT
{
	HWND			m_hWnd;
	const TCHAR*	m_pTitle;
	::CWQSG_MSG_W	m_MSG;
	BOOL ��ʽ����30( WQSG_File_mem& a_OldFile , WQSG_File_mem& a_NewFile , ::CWQSG_File& a_IPS_File ,
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

		const s64 ����posԭ = a_OldFile.GetFileSize( ) - 1;

		const s64 ����pos�� = a_NewFile.GetFileSize( ) - 1;

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
					u8 const* ptrԭ = a_OldFile.GetPtr( commBeginPos , curlen );
					if( NULL == ptrԭ )
						return FALSE ;

					u8 const* ptr�� = a_NewFile.GetPtr( commBeginPos , curlen );
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
					a_OldFile.Free();a_NewFile.Free();
				}
				if( binbuf.LEN )
				{
					s64 def_offset = commBeginPos - commbase;
					if( def_offset <= 0xFFFFFFFF )
					{
						if( 1 != a_IPS_File.Write( "\1" , 1 ) )
						{
							m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
							return FALSE;
						}
						if( 4 != a_IPS_File.Write( &def_offset , 4 ) )
						{
							m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
							return FALSE;
						}
					}
					else
					{
						if( 1 != a_IPS_File.Write( "\2" , 1 ) )
						{
							m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
							return FALSE;
						}
						if( 8 != a_IPS_File.Write( &commBeginPos , 8 ) )
						{
							m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
							return FALSE;
						}
					}
					if( 2 != a_IPS_File.Write( &binbuf.LEN , 2 ) )
					{
						m_MSG.show(_T("д������ʧ��(-1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( binbuf.LEN != a_IPS_File.Write( binbuf.BIN , binbuf.LEN ) )
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

				u8 const* ptr�� = a_NewFile.GetPtr( commBeginPos , curlen );
				if( NULL == ptr�� )
					return FALSE ;

				ʣ��len -= curlen;

				s64 def_offset = commBeginPos - commbase;
				if( def_offset <= 0xFFFFFFFF )
				{
					if( 1 != a_IPS_File.Write( "\1" , 1 ) )
					{
						a_NewFile.Free();
						m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( 4 != a_IPS_File.Write( &def_offset , 4 ) )
					{
						a_NewFile.Free();
						m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
						return FALSE;
					}
				}
				else
				{
					if( 1 != a_IPS_File.Write( "\2" , 1 ) )
					{
						a_NewFile.Free();
						m_MSG.show(_T("д������ʧ��(1),���̿ռ䲻��?"));
						return FALSE;
					}
					if( 8 != a_IPS_File.Write( &commBeginPos , 8 ) )
					{
						a_NewFile.Free();
						m_MSG.show(_T("д������ʧ��(-0),���̿ռ䲻��?"));
						return FALSE;
					}
				}
				if( 2 != a_IPS_File.Write( &curlen , 2 ) )
				{
					a_NewFile.Free();
					m_MSG.show(_T("д������ʧ��(-1),���̿ռ䲻��?"));
					return FALSE;
				}
				if( curlen != a_IPS_File.Write( ptr�� , curlen ) )
				{
					a_NewFile.Free();
					m_MSG.show(_T("д������ʧ��(-2),���̿ռ䲻��?"));
					return FALSE;
				}
				a_NewFile.Free();
				commbase = commBeginPos;
				commBeginPos += curlen;
			}
		}
		a_IPS_File.Write("\377",1);
		return TRUE;
#undef DEF_FN_д��IPS
	}
public:
	CWQSG_IPS_OUT( HWND a_hWnd , const TCHAR* a_pTtle )
		:m_hWnd(a_hWnd)
		,m_pTitle(a_pTtle)
		,m_MSG(a_hWnd,a_pTtle)
	{}

	virtual	~CWQSG_IPS_OUT()
	{}

	BOOL MakeWips30( const TCHAR*const a_pOldFile , const TCHAR*const a_pNewFile , const TCHAR*const a_pWipsFile , const s64 a_nBeginOffset ,//����һ���ļ�ʧЧ
		const s64 a_nEndOffset ,//�������ļ�,���յ���β
		const s64 a_nBaseOffset ,
		INT a_bUseCRC32 = 0 ,//������֤
		const WCHAR* a_pDesc = NULL ,
		DWORD a_dwDescLen = 0 ,
		CWD_BIN* EXE = NULL )
	{
		if( a_nBeginOffset < 0 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( a_nEndOffset < 0 )
		{
			m_MSG.show(_T("������ַ ���ܳ��� 0x7FFFFFFFFFFFFFFF"));
			return FALSE;
		}
		if( a_nEndOffset < a_nBeginOffset )
		{
			m_MSG.show(_T("������ַ ���ܱ� ��ʼ��ַС"));
			return FALSE;
		}

		if( a_dwDescLen > 65535 )
		{
			m_MSG.show(_T("˵������ �������ܳ���65535��"));
			return FALSE;
		}
		//��ԭ�ļ�
		WQSG_File_mem oldFile , newFile ;

		if( !oldFile.OpenFile( a_pOldFile , 3 ) )
		{
			m_MSG.show(_T("ԭ�ļ���ʧ��"));
			return FALSE;
		}
		//�����ļ�
		if( !newFile.OpenFile( a_pNewFile , 3 ) )
		{
			m_MSG.show(_T("���ļ���ʧ��"));
			return FALSE;
		}
		//�ж���ʼ��ַ �Ϸ���
		s64 size1 = oldFile.GetFileSize( );
		const s64 size2 = newFile.GetFileSize( );

		if( a_nBeginOffset >= size1 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܱ� ԭ�ļ���\nû����"));
			return FALSE;
		}
		if( a_nBeginOffset >= size2 )
		{
			m_MSG.show(_T("��ʼ��ַ ���ܱ� ���ļ���\nû����"));
			return FALSE;
		}

		const s64 pos_begin = a_nBeginOffset;
		const s64 pos_end =  ( a_nEndOffset < size2 )?a_nEndOffset:size2 - 1;
		///////////////
		::CWQSG_File IPS_File;
		if( ! IPS_File.OpenFile( a_pWipsFile ,(a_nBaseOffset==0)?4:2,3) )
		{
			m_MSG.show(_T("���������ļ�ʧ��"));
			return FALSE;
		}
		IPS_File.Seek(a_nBaseOffset);
		//��֤CRC
		DWORD CRC32 = 0;
		if( a_bUseCRC32 != 0 )
		{
			if( !oldFile.GetCrc32( CRC32 ) )
			{
				m_MSG.show(_T("����CRC32ʧ��"));
				return FALSE;
			}//
		}
		////////////////////////////////�Խ�ͷ
		u64 uExeOffset = 0;
		if(EXE != NULL)
		{
			IPS_File.Write( EXE->BIN , EXE->LEN );
			uExeOffset = EXE->LEN;
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
		if( ��ʽ����30( oldFile  , newFile , IPS_File , pos_begin , pos_end ) )
		{
			if( a_dwDescLen )
			{
				const s64 pos = IPS_File.Tell( ) - uExeOffset;
				IPS_File.Write( a_pDesc , (a_dwDescLen<<1) );
				IPS_File.Write( "\0\0", 2 );
				IPS_File.Seek( uExeOffset + a_nBaseOffset + 0x20 );
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
	HWND			m_hWnd;
	const TCHAR*	m_pTitle;
	::CWQSG_MSG_W	m_MSG;
	u8				m_buf[0xFFFF];
	BOOL �򲹶�30( CWQSG_File& a_IPS_File , CWQSG_File& a_ROM_File )
	{
		DWORD dwLen;
		s64 nPos = 0;
		while(TRUE)
		{
			if( !a_IPS_File.Read( m_buf , 1 ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(5)"));
				return FALSE;
			}
			switch(*m_buf)
			{
			case 0x01://���ƫ��
				if( !a_IPS_File.Read( m_buf , 4 ) )
				{
					m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(6.1)"));
					return FALSE;
				}
				nPos += *(u32*)m_buf;
				break;
			case 0x02://����ƫ��
				if( !a_IPS_File.Read( &nPos , 8 ) )
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
			dwLen = 0;

			if( !a_IPS_File.Read( &dwLen , 2 ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(7)"));
				return FALSE;
			}

			if( !a_IPS_File.Read( m_buf , dwLen ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n�����ļ�����(8)"));
				return FALSE;
			}
			if( !a_ROM_File.Seek( nPos ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\n����ROM�ļ�λ��ʧ��"));
				return FALSE;
			}
			if( dwLen != a_ROM_File.Write( m_buf , dwLen ) )
			{
				m_MSG.show(_T("�򲹶�ʧ��\nд����ʧ��ʧ��"));
				return FALSE;
			}
		}
	}
public:
	CWQSG_IPS_IN( HWND a_hWnd , const TCHAR* a_pTitle )
		:m_hWnd(a_hWnd)
		,m_pTitle(a_pTitle)
		,m_MSG( a_hWnd , a_pTitle ){}
	virtual	~CWQSG_IPS_IN(){}
	BOOL �򲹶�( const TCHAR*const a_pWipsFile , const TCHAR*const a_pTFile , n64 a_nBaseOffset = 0 )
	{
		if( a_nBaseOffset < 0 )
			return FALSE;

		::CWQSG_File IPS_File,ROM_File;
		if( !IPS_File.OpenFile( a_pWipsFile , 1 , 3 ) )
		{
			m_MSG.show(_T("�����ļ���ʧ��"));
			return FALSE;
		}

		if( !ROM_File.OpenFile( a_pTFile , 3 , 3 ) )
		{
			m_MSG.show(_T("Ŀ���ļ���ʧ��"));
			return FALSE;
		}

		IPS_File.Seek( a_nBaseOffset );

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
						const s64 size = IPS_File.GetFileSize() - a_nBaseOffset;
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
					if( ! IPS_File.Seek(a_nBaseOffset + ����ָ��) )
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
};
#endif

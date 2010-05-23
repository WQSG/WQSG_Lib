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

__i__

///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
inline WCHAR* WQSG_GetTXT_Line( WCHAR** WTXT_ALL )
{
	if( (WTXT_ALL == NULL) || (NULL == (*WTXT_ALL)) )
		return NULL;

	WCHAR* tmp = *WTXT_ALL;

	while((L'\0' != *tmp) && (0x0D != *tmp) && (0x0A != *tmp))
		++tmp;

	//��λ0x0D��0x0A��λ��

	INT I = (INT)(tmp - *WTXT_ALL);//��ȡ����

	BOOL �Ƿ�0D = (0x0D == *tmp);

	if( I || *tmp )
	{
		WCHAR* const out = new WCHAR[I + 1];
		WCHAR* s1 = out;
		tmp = *WTXT_ALL;

		//����
		while(I--)
			*(s1++) = *(tmp++);

		*s1 = L'\0';

		if(*tmp)
		{
			++tmp;

			if(0x0A == *tmp && �Ƿ�0D)
				++tmp;
		}
		*WTXT_ALL = tmp;
		return out;
	}

	return NULL;
}
//--------------------------------------------------------------------------------
inline WQSG_enCP WQSG_A_U_X( HANDLE hfile )
{
	if(NULL == hfile)
		return en_CP_NULL;

	::SetFilePointer( hfile , 0 , NULL , FILE_BEGIN );

	DWORD sizeH;

	DWORD sizeL = ::GetFileSize( hfile , &sizeH );

	const s64 size = ( ((s64)sizeH)<<32) | sizeL;

	if( size < 0 )
		return en_CP_NULL;

	if( size >= 2 )
	{
		u32 con_i = 0;
		sizeL = (size >= 3)?3:2;

		if( (!::ReadFile( hfile , &con_i , sizeL , &sizeH , NULL ))
			|| (sizeH != sizeL) )
		{
			return en_CP_NULL;
		}

		switch( con_i )
		{
		case 0xBFBBEF:
			return en_CP_UTF8;
		default:

			switch( con_i & 0xFFFF )
			{
			case 0xFEFF:
				return en_CP_UNICODE;

			case 0xFFFE:
				return en_CP_UNICODE_BIG;

			default:
				break;
			}
		}
	}

	return en_CP_ANSI;
}
//--------------------------------------------------------------------------------
inline void CMemTextW::Clear()
{
	m_errTXT = NULL;
	m_CP = en_CP_NULL;
	delete[]m_Text;
	m_errTXT = m_Text = m_TXT = NULL;
}
///--------------------------------------------------------------------------------
inline BOOL	CMemTextW::Load( WCHAR const*const �ļ�·�� , DWORD �����ļ������  , const UINT codePage )
{
	m_errTXT = NULL;

	Clear();

	::CWQSG_File TXT_File;

	if( ! TXT_File.OpenFile(�ļ�·��,1,3) )
	{
		m_errTXT = L"��ʧ��";
		return FALSE;
	}

	s64 size = TXT_File.GetFileSize( );

	if(size > (s64)�����ļ������)
	{
		/*
		WCHAR tmp1[256];
		if(size < 1024){
			::swprintf(tmp1,100,L"Ϊ�˰�ȫ,�ı����ܳ��� %u Byte",�����ļ������);
		}else if(size < 1048576){
			�����ļ������ /= 1024;
			::swprintf(tmp1,100,L"Ϊ�˰�ȫ,�ı����ܳ��� %u KB",�����ļ������);
		}else{
			�����ļ������ /= 1048576;
			::swprintf(tmp1,100,L"Ϊ�˰�ȫ,�ı����ܳ��� %u MB",�����ļ������);
		}*/
		m_errTXT = L"�ļ�����";
		return FALSE;
	}

	u8* tmp;

	switch( m_CP = ::WQSG_A_U_X(TXT_File.GetFileHANDLE()) )
	{
	case en_CP_ANSI:
		TXT_File.Seek(0);
		tmp = new u8 [(DWORD)size + 1];tmp[(DWORD)size] = 0;
		TXT_File.Read( tmp ,(u32)size );
		m_Text = ::WQSG_char_W( (char*)tmp , codePage );
		delete[]tmp;

		break;

	case en_CP_UNICODE:
		TXT_File.Seek(2);
		tmp = new u8 [(UINT)size];
		size -= 2;
		TXT_File.Read(tmp,(u32)size );
		m_Text = (WCHAR*)tmp;
		m_Text[(UINT)size>>1] = 0;

		break;

/*	case case en_CP_UNICODE_BIG:
		TXT_File.Seek(2);
		tmp = new u8 [(UINT)size];
		size -= 2;
		TXT_File.Read(tmp,(U32)size );
		m_�ı� = (WCHAR*)tmp;
		m_�ı�[(UINT)size>>1] = 0;

		break;*/

	case en_CP_UTF8:
		TXT_File.Seek(3);
		tmp = new u8 [(DWORD)size-2];tmp[(DWORD)size-3] = 0;
		TXT_File.Read( tmp ,(u32)size-3 );
		m_Text = WQSG_UTF8_W( (char*)tmp );
		delete[]tmp;

		break;

	default:
		m_errTXT = L"��֧�ֵ��ı���ʽ";
		return FALSE;
	}

	m_TXT = m_Text;

	return TRUE;
}
///--------------------------------------------------------------------------------
inline WCHAR* CMemTextW::GetLine()
{
	return ::WQSG_GetTXT_Line(&m_TXT);
}

inline BOOL CMemTextW::����( const WCHAR*const pText )
{
	m_errTXT = NULL;

	if( NULL == pText )
	{
		m_errTXT = L"�����ı�Ϊ��";
		return FALSE;
	}

	Clear();

	m_CP = en_CP_UNICODE;
	m_TXT = m_Text = (WCHAR*)pText;

	return TRUE;
}

inline WCHAR* CMemTextW::�ж�( void )
{
	m_errTXT = (m_Text==NULL)?L"���ı�":NULL;

	WCHAR* pTXT = m_Text;

	m_Text = NULL;

	Clear();

	return pTXT;
}
//------------------------------------------------------------------------------
inline BOOL WQSG_ȡ��·��( const WCHAR*const longPath , WCHAR* shortPath )
{
	WCHAR tmp[ MAX_PATH ];
	*shortPath = L'\0';

	WCHAR* sp = (WCHAR*)longPath;
	INT pos;
	WCHAR str[ MAX_PATH ];
	INT pos2 = 0;

	if( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 )
	{
		do
		{
			if( ( pos2 + pos + 1 ) >= MAX_PATH )
				return FALSE;

			if( !(sp[++pos]) )
			{
				INT pos0 = WQSG_strcpy( str , tmp );
				WQSG_strcpy( sp , tmp + pos0 );
				DWORD attr = GetFileAttributes( tmp );

				if( attr == INVALID_FILE_ATTRIBUTES )
					return FALSE;

				if( !( attr & FILE_ATTRIBUTE_DIRECTORY ) )
				{
					sp += pos;
					break;
				}
			}

			pos2 += WQSG_strcpy_Ex( sp , str + pos2 , pos );
			sp += pos;

			if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
				return FALSE;

			pos2 = WQSG_strcpy( tmp , str );
		}
		while( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 );
	}

	if( *sp )
	{
		pos = WQSG_strlen( sp );

		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		INT pos0 = WQSG_strcpy( str , tmp );

		WQSG_strcpy( sp , tmp + pos0 );

		DWORD attr = GetFileAttributes( tmp );

		if( attr == INVALID_FILE_ATTRIBUTES )
			return FALSE;

		if( attr & FILE_ATTRIBUTE_DIRECTORY )
		{
			pos2 += WQSG_strcpy( sp , str + pos2 );
			WQSG_strcpy( L"\\" , str + pos2 );
		}
	}

	DWORD attr = GetFileAttributes( str );

	if( attr == INVALID_FILE_ATTRIBUTES )
		return FALSE;

	WQSG_strcpy( str , shortPath );

	return TRUE;
}
///--------------------------------------------------------------------------------
inline BOOL WQSG_ȡ��·���ļ���( const WCHAR*const longPath , WCHAR* shortPath )
{
	WCHAR tmp[ MAX_PATH ];
	*shortPath = L'\0';

	WCHAR* sp = (WCHAR*)longPath;
	INT pos;
	WCHAR str[ MAX_PATH ];
	INT pos2 = 0;

	while( ( pos = WQSG_strchr( sp , L'\\') ) >= 0 )
	{
		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		pos2 += WQSG_strcpy_Ex( sp , str + pos2 , pos++ );

		if( sp[pos] )
			pos2 += WQSG_strcpy( L"\\" , str + pos2 );

		sp += pos;

		if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
			return FALSE;

		pos2 = WQSG_strcpy( tmp , str );
	}

	if( *sp )
	{
		pos = WQSG_strlen( sp );

		if( ( pos2 + pos + 1 ) >= MAX_PATH )
			return FALSE;

		WQSG_strcpy( sp , str + pos2);

		if( 0 == GetShortPathNameW( str , tmp , MAX_PATH ) )
			return FALSE;

		WQSG_strcpy( tmp , str );
	}

	DWORD attr = GetFileAttributes( str );

	if( attr == INVALID_FILE_ATTRIBUTES )
		return FALSE;

	WQSG_strcpy( str , shortPath );

	return TRUE;
}
//---------------------------------------------------------------------------------
///		ȡ����exe��·��
///--------------------------------------------------------------------------------
inline BOOL WQSG_GetExePath( WCHAR*const outBuf , DWORD maxWordNum )
{
	WCHAR exePathName[MAX_PATH*2];
	DWORD revCount;

	if( NULL == outBuf )
		return FALSE;

	revCount = ::GetModuleFileNameW( NULL , exePathName , MAX_PATH*2 );
	if( revCount <= 3 )
		return FALSE;

	{
		WCHAR* tmp = exePathName + revCount;
		while( (exePathName != tmp) && (*tmp != L'\\' ) )
		{
			--revCount;
			--tmp;
		}
		*tmp = L'\0';
	}
	return( ( revCount >= 2 ) && (revCount <= maxWordNum) && ( revCount == WQSG_strcpy_Ex( exePathName , outBuf , maxWordNum ) ) );
}
//---------------------------------------------------------------------------------
///		�ƶ��ļ�,����ļ��Ѿ�����,�Զ�����
///--------------------------------------------------------------------------------
inline BOOL WQSG_MoveFileEx( const WCHAR*const srcFileName , const WCHAR*const newFileName )
{
	WCHAR tmpNew[ MAX_PATH ];
	WCHAR* newname = (WCHAR*)newFileName;

	WCHAR path[ MAX_PATH ] , extName[ MAX_PATH ];
	int i = 0;
	///-------------------------------------------
	if( ( NULL == srcFileName ) || ( srcFileName[1] != L':' ) ||
		( NULL == newFileName ) || ( newFileName[1] != L':' )
		)
		return FALSE;
_gt_start:
	if( MoveFileW( srcFileName , newname ) )
	{
		return TRUE;
	}
	else if(  ERROR_FILE_EXISTS == ::GetLastError() )
	{
		if( newname != tmpNew )
		{
			WCHAR B[MAX_PATH],C[MAX_PATH];
			newname = tmpNew;
			if( WQSG_strlen ( newFileName ) > ( MAX_PATH - 10 ) )
				return FALSE;
#if ___SW_���ξ���___
#pragma warning(disable: 4996)
#endif
			_wsplitpath( newFileName , path , B , C , extName );
			_wmakepath( path , tmpNew , B , C , L"" );
#if ___SW_���ξ���___
#pragma warning(default: 4996)
#endif
		}
		if( 3 == wsprintfW( tmpNew , L"%s (%u)%s" , path , ++i , extName ) )
			goto _gt_start;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------
///		����Ƿ���Ŀ¼
///		path		Ҫ�����·��
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsDir( const WCHAR*const path )
{
	DWORD attr;
	if( NULL == path )
		return FALSE;

	attr = GetFileAttributesW( path );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( attr & FILE_ATTRIBUTE_DIRECTORY ) );
}
//---------------------------------------------------------------------------------
///		����Ƿ����ļ�
///		pathName	Ҫ�����ļ�·����
///--------------------------------------------------------------------------------
inline BOOL WQSG_IsFile( const WCHAR*const pathName )
{
	DWORD attr;
	if( NULL == pathName )
		return FALSE;

	attr = GetFileAttributesW( pathName );
	return ( (INVALID_FILE_ATTRIBUTES != attr) && ( !( attr & FILE_ATTRIBUTE_DIRECTORY ) ) );
}
//---------------------------------------------------------------------------------
///		����Ŀ¼,�ɹ����Ѿ����ڷ��� TRUE
///		path		Ҫ������·��
///--------------------------------------------------------------------------------
inline BOOL WQSG_CreateDir( const WCHAR*const path )
{
	if( NULL == path )
		return FALSE;

	if( WQSG_IsDir( path ) )
		return TRUE;
	return ::CreateDirectoryW( path , NULL );
}

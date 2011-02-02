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
#include "ISO_App.h"
#include "../../Common/iso/WQSG_PsxISO.h"
#include "../../Common/iso/WQSG_UMD.h"
#include "ISO_App_Lang.h"
#include <algorithm>
#include <map>
#include <hash_map>
#include <string>
class CWQSG_TempMapFile : public CWQSG_xFile
{
	CWQSG_xFile* m_pFile;
	s64 m_nStartOffset;
	s64 m_nFileSzie;
	s64 m_nFileOffset;
public:
	CWQSG_TempMapFile()
		: m_pFile(NULL)
		, m_nStartOffset(0)
		, m_nFileSzie(0)
		, m_nFileOffset(0)
	{

	}

	virtual	u32			Read		( void*const lpBuffre , const u32 len )
	{
		if( IsOpen() )
		{
			if( m_nFileOffset >= 0 && m_nFileOffset < m_nFileSzie && m_pFile->Seek( m_nStartOffset + m_nFileOffset ) )
			{
				const s64 xLen = m_nFileSzie - m_nFileOffset;
				u32 uLen = (xLen < len)?u32(xLen):len;

				const u32 uReadLen = m_pFile->Read( lpBuffre , uLen );

				m_nFileOffset += uReadLen;
				return uReadLen;
			}
		}

		return 0;
	}

	virtual	u32			Write		( void const*const lpBuffre , const u32 len ){return FALSE;}
	virtual	void		Close		( void )
	{
		m_pFile = NULL;
		m_nStartOffset = m_nFileSzie = m_nFileOffset = 0;
	}

	virtual	s64			GetFileSize	( void )const
	{
		return m_nFileSzie;
	}

	virtual	BOOL		SetFileLength( const s64 Length )
	{
		return FALSE;
	}

	virtual	s64			Tell		( void )const
	{
		return m_nFileOffset;
	}

	virtual	BOOL		Seek		( const s64 offset )
	{
		m_nFileOffset = offset;
		return TRUE;
	}

	virtual	u32			GetCRC32	( void )
	{
		return 0;
	}

	virtual	BOOL		IsOpen		( void )const
	{
		return (m_pFile && m_pFile->IsOpen());
	}

	virtual	BOOL		IsCanRead	( void )const
	{
		return TRUE;
	}
	virtual	BOOL		IsCanWrite	( void )const
	{
		return FALSE;
	}
	//=================================================
	BOOL Init( CWQSG_xFile* a_pFile , s64 a_nStartOffset , s64 a_nFileSize )
	{
		Close();

		if( NULL == a_pFile || !a_pFile->IsOpen() )
			return FALSE;

		const s64 size = a_pFile->GetFileSize();

		if( size < 0 || a_nStartOffset < 0 || a_nStartOffset >= size )
			return FALSE;

		const s64 xSize = size - a_nStartOffset;
		if( a_nFileSize < 0 || a_nFileSize > xSize )
			return FALSE;

		a_pFile->Seek( a_nStartOffset );

		m_pFile = a_pFile;
		m_nStartOffset = a_nStartOffset;
		m_nFileSzie = a_nFileSize;
		m_nFileOffset = 0;

		return TRUE;
	}

};
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
class CSIsoFileFindMgr
{
	typedef std::map<SIsoFileFind*,SIsoFileFind*> TSIsoFileFindMgrMap;
	TSIsoFileFindMgrMap m_map;
public:
	CSIsoFileFindMgr(){}

	~CSIsoFileFindMgr()
	{
		for( TSIsoFileFindMgrMap::iterator it = m_map.begin() ; 
			it != m_map.end() ; ++it )
		{
			delete it->second;
		}

		m_map.clear();
	}

	SIsoFileFind* Allocate()
	{
		SIsoFileFind* pRt = new SIsoFileFind;
		if( pRt )
			m_map[pRt] = pRt;

		return pRt;
	}

	void Free( SIsoFileFind* a_Ptr )
	{
		TSIsoFileFindMgrMap::iterator it = m_map.find(a_Ptr);
		if( it != m_map.end() )
		{
			m_map.erase( it );
			delete a_Ptr;
		}
	}
};

static CSIsoFileFindMgr g_IsoFileFindMgr;
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
CISO_App::CISO_App(void)
: m_pIso(NULL)
, m_StringMgr( g_ISO_App_String , sizeof(g_ISO_App_String)/sizeof(*g_ISO_App_String) )
, m_szUserString(NULL)
, m_uUserString(0)
{
}

CISO_App::~CISO_App(void)
{
	CloseISO();
}

BOOL CISO_App::OpenISO( CStringW a_strPathName , const BOOL a_bCanWrite , EWqsgIsoType a_eType )
{
	CloseISO();

	switch( a_eType )
	{
	case E_WIT_PsxISO:
		m_pIso = new CWQSG_PsxISO;
		break;
	case E_WIT_UMD:
		m_pIso = new CWQSG_UMD;
		break;
	default:
		SetErr( L"???" );
		return FALSE;
	}

	if( NULL == m_pIso )
	{
		SetErr( L"???" );
		return FALSE;
	}

	m_pIso->SetLangString( m_szUserString , m_uUserString );

	if( !m_pIso->OpenISO( a_strPathName , a_bCanWrite ) )
	{
		SetErr( GetLangString(35) , a_strPathName.GetString() );
		return FALSE;
	}
	return TRUE;
}

inline BOOL CISO_App::GetPathDirEnt( SISO_DirEnt& a_tDirEnt , const CStringA a_strIsoPathA )
{
	if( NULL == m_pIso )
		return FALSE;

	if( !m_pIso->GetRootDirEnt( a_tDirEnt ) )
	{
		SetErr( GetLangString(11) );
		return FALSE;
	}

	CStringA strIsoPathA( a_strIsoPathA );
	if( strIsoPathA.Right( 1 ) != L'/' )
		strIsoPathA += L'/';

	while( strIsoPathA[0] == '/' )
		strIsoPathA.Delete( 0 , 1 );

	int pos;
	SISO_DirEnt tmp;
	while( (pos = strIsoPathA.Find( '/' )) > 0 )
	{
		const CStringA strNameA( strIsoPathA.Left( pos ) );

		strIsoPathA.Delete( 0 , pos );
		while( strIsoPathA[0] == '/' )
			strIsoPathA.Delete( 0 , 1 );

		if( m_pIso->FindFile( tmp , a_tDirEnt , strNameA.GetString() ) < 0 )
		{
			SetErr( GetLangString(24) , a_strIsoPathA.GetString() );
			return FALSE;
		}

		if( 2 != (tmp.attr & 2) )
		{
			SetErr( GetLangString(24) , a_strIsoPathA.GetString() );
			return FALSE;
		}

		a_tDirEnt = tmp;
	}
	return TRUE;
}

inline static BOOL TestName( CStringA& strName )
{
	for( int i = 0; (i>=0) && (strName[i]) ; ++i )
	{
		u8 ch = strName[i];
		if( ( (ch < 'a') || (ch > 'z') ) &&
			( (ch < 'A') || (ch > 'Z') ) &&
			( (ch < '0') || (ch > '9') ) &&
			( ch != '.' ) && ( ch != '_' ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

inline BOOL CISO_App::zzz_CreateDir( BOOL& a_bIsoBreak , CStringA a_strIsoName , CStringA a_strIsoPath )
{
	a_bIsoBreak = FALSE;
	if( NULL == m_pIso )
		return FALSE;

	if( !TestName(a_strIsoName) )
	{
		SetErr( GetLangString(34) , a_strIsoName.GetString() );
		return FALSE;
	}

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_strIsoPath ) )
		return FALSE;

	if( !m_pIso->CreateDir( sDirEnt_Path , a_strIsoName ) )
	{
		SetErr( GetLangString(33) , a_strIsoName.GetString() );
		return FALSE;
	}

	return TRUE;
}

inline BOOL CISO_App::ImportDir( BOOL& a_bIsoBreak , CStringA a_strIsoPathA , CStringW a_strInPathName )
{
	a_bIsoBreak = FALSE;

	if( a_strInPathName.Right(1) != L'\\' )
		a_strInPathName += L'\\';

	BOOL rt = TRUE;
	WIN32_FIND_DATAW data;

	const HANDLE handle = ::FindFirstFileW( a_strInPathName + L"*.*" , &data );
	if( INVALID_HANDLE_VALUE != handle )
	{
		do{
			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CStringW nameW( data.cFileName );
				if( nameW != L"." && nameW != L".." )
				{
					if( !EasyImport( a_bIsoBreak , a_strInPathName + data.cFileName , a_strIsoPathA ) )
					{
						rt = FALSE;
						break;
					}
				}
			}
			else
			{
				if( !EasyImport( a_bIsoBreak , a_strInPathName + data.cFileName , a_strIsoPathA ) )
				{
					rt = FALSE;
					break;
				}
			}

		}while( FindNextFileW( handle , &data ) );

		FindClose( handle );
	}

	return rt;
}

BOOL CISO_App::EasyImport( BOOL& a_bIsoBreak , CStringW a_strInPathName , CStringA a_strIsoPathA )
{
	a_bIsoBreak = FALSE;
	CStringA strNameA;
	{
		const int pos = a_strInPathName.ReverseFind( '\\' );
		if( pos <= 0 )
		{
			SetErr( GetLangString(28) , a_strInPathName.GetString() );
			return FALSE;
		}

		CString strNameW( a_strInPathName.Mid( pos + 1 ) );
		strNameA = strNameW;
	}

	if( ::WQSG_IsFile( a_strInPathName.GetString() ) )
	{
		if( !ImportFile( a_bIsoBreak , a_strIsoPathA , strNameA , a_strInPathName ) )
		{
			return FALSE;
		}
	}
	else if( ::WQSG_IsDir( a_strInPathName.GetString() ) )
	{
		if( !zzz_CreateDir( a_bIsoBreak , strNameA , a_strIsoPathA ) )
			return FALSE;

		if( !ImportDir( a_bIsoBreak , (a_strIsoPathA + '/' + strNameA) , a_strInPathName ) )
			return FALSE;
	}
	else
	{
		SetErr( GetLangString(27) , a_strInPathName.GetString() );
		return FALSE;
	}
	return TRUE;
}

inline BOOL CISO_App::zzz_WriteFile( BOOL& a_bIsoBreak , CStringW a_strInPathName , CWQSG_xFile& a_InFp , CStringA a_strIsoName ,
							 CStringA a_strIsoPath , const s32 a_nOffset , const BOOL a_bNew , const SIsoTime* a_pTime )
{
	a_bIsoBreak = FALSE;

	if( NULL == m_pIso )
		return FALSE;

	if( !TestName(a_strIsoName) )
	{
		SetErr( GetLangString(32) , a_strIsoName.GetString() );
		return FALSE;
	}

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_strIsoPath ) )
	{
		return FALSE;
	}

	const s64 srcFileSize = a_InFp.GetFileSize();
	if( ( srcFileSize < 0 ) || ( srcFileSize > (((u32)-1))>>1) )
	{
		SetErr( GetLangString(29) , a_strInPathName.GetString() );
		return FALSE;
	}

	if( !m_pIso->WriteFile( sDirEnt_Path , a_strIsoName , a_InFp , (s32)srcFileSize , a_nOffset , a_bNew , FALSE , a_pTime ) )
	{
		SetErr( GetLangString(30) , a_strInPathName.GetString() );
		return FALSE;
	}

	return TRUE;
}

BOOL CISO_App::ExportFile( CStringW a_strOutPathName , CStringA a_strIsoPathA , CStringA a_strIsoNameA )
{
	if( NULL == m_pIso )
		return FALSE;

	::DeleteFile( a_strOutPathName );

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_strIsoPathA ) )
		return FALSE;

	SISO_DirEnt sDirEnt_File;
	if( m_pIso->FindFile( sDirEnt_File , sDirEnt_Path , a_strIsoNameA.GetString() ) < 0 )
		return FALSE;

	CWQSG_File fp;
	if( !fp.OpenFile( a_strOutPathName.GetString() , 4 , 3 ) )
	{
		SetErr( GetLangString(23) , a_strOutPathName.GetString() );
		return FALSE;
	}

	if( !m_pIso->ReadFile( sDirEnt_Path , a_strIsoNameA , fp , L2H(sDirEnt_File.size_LE) , 0 ) )
	{
		fp.Close();
		::DeleteFile( a_strOutPathName );
		SetErr( GetLangString(31) , a_strOutPathName.GetString() );
		return FALSE;
	}

	return TRUE;
}

BOOL CISO_App::ExportDir( CStringW a_strOutPath , CStringA a_strIsoPathA )
{
	if( NULL == m_pIso )
		return FALSE;

	SIsoFileFind* pFind = FindIsoFile( a_strIsoPathA );
	if( !pFind )
	{
		SetErr( GetLangString(24) , a_strIsoPathA.GetString() );
		return FALSE;
	}

	if( a_strOutPath.Right(1) != L'\\' )
		a_strOutPath += L'\\';

	if( a_strIsoPathA.Right(1) != L'/' )
		a_strIsoPathA += L'/';

	if ( !WQSG_CreateDir( a_strOutPath.GetString() ) )
	{
		SetErr( GetLangString(26) , a_strOutPath.GetString() );
		return FALSE;
	}

	BOOL bRt = TRUE;
	CStringW strPathName;
	SIsoFileData data;

	while( FindNextIsoFile( pFind , data ) )
	{
		strPathName.Format( L"%ls%hs" , a_strOutPath.GetString() , data.name );

		if( data.isDir )
		{
			if( !ExportDir( a_strOutPath , a_strIsoPathA + data.name ) )
			{
				bRt = FALSE;
				goto __gtSaveDirExit;
			}
		}
		else
		{
			if( !ExportFile( a_strOutPath , a_strIsoPathA , data.name ) )
			{
				bRt = FALSE;
				goto __gtSaveDirExit;
			}
		}
	}

__gtSaveDirExit:
	if( pFind )
	{
		CloseFindIsoFile( pFind );
		pFind = NULL;
	}

	return bRt;
}

BOOL CISO_App::WriteFile( BOOL& a_bIsoBreak , CStringA a_strIsoPathA , CStringA a_strIsoNameA , s32 a_nOffset , CStringW a_strInPathName )
{
	a_bIsoBreak = FALSE;

	SISO_DirEnt sDirEnt_File;
	if( !zzz_GetFileData( sDirEnt_File , a_strIsoPathA , a_strIsoNameA ) )
		return FALSE;

	if( sDirEnt_File.attr & 2 )
	{
		SetErr( GetLangString(25) , (a_strIsoPathA + a_strIsoNameA).GetString() );
		return FALSE;
	}

	CWQSG_File fp;
	if( !fp.OpenFile( a_strInPathName.GetString() , 1 , 3 ) )
	{
		SetErr( GetLangString(23) , a_strInPathName.GetString() );
		return FALSE;
	}

	return zzz_WriteFile( a_bIsoBreak , a_strInPathName , fp , a_strIsoNameA , a_strIsoPathA , a_nOffset , FALSE , NULL );
}

BOOL CISO_App::ImportFile( BOOL& a_bIsoBreak , CStringA a_strPathA , CStringA a_strNameA , CStringW a_strInPathName )
{
	a_bIsoBreak = FALSE;

	CWQSG_File fp;
	if( !fp.OpenFile( a_strInPathName.GetString() , 1 , 3 ) )
	{
		SetErr( GetLangString(23) , a_strInPathName.GetString() );
		return FALSE;
	}

	return zzz_WriteFile( a_bIsoBreak , a_strInPathName , fp , a_strNameA , a_strPathA , 0 , TRUE , NULL );
}

inline BOOL CISO_App::zzz_GetFileData( SISO_DirEnt& a_tDirEnt , CStringA a_strPathA , CStringA a_strNameA )
{
	if( NULL == m_pIso )
		return FALSE;

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_strPathA ) )
		return FALSE;

	if( a_strNameA[0] != 0 )
	{
		if( m_pIso->FindFile( a_tDirEnt , sDirEnt_Path , a_strNameA.GetString() ) < 0 )
		{
			SetErr( GetLangString(22) , (a_strPathA + a_strNameA).GetString() );
			return FALSE;
		}
	}
	else
		a_tDirEnt = sDirEnt_Path;

	return TRUE;
}

BOOL CISO_App::GetFileData( SIsoFileData& a_data , CStringA a_strPathA , CStringA a_strNameA  )
{
	SISO_DirEnt tmp;
	if( zzz_GetFileData( tmp , a_strPathA , a_strNameA ) )
	{
		a_data.isDir = ( (tmp.attr & 2) == 2 );
		a_data.size = L2H(tmp.size_LE);
		a_data.lba = L2H(tmp.lba_LE);
		a_data.time = tmp.time;

		WQSG_strcpy( a_strNameA.GetString() , a_data.name );

		return TRUE;
	}

	return FALSE;
}

SIsoFileFind* CISO_App::FindIsoFile( CStringA a_strIsoPathA )
{
	SISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , a_strIsoPathA ) )
		return NULL;

	SIsoFileFind* pRt = g_IsoFileFindMgr.Allocate();
	if( pRt )
	{
		m_Objs.push_back(pRt);

		pRt->m_nOffset = 0x30;

		pRt->m_DirEnt = tP_DirEnt;
	}

	return pRt;
}

BOOL CISO_App::FindNextIsoFile( SIsoFileFind* a_handle , SIsoFileData& a_data )
{
	if( NULL == m_pIso )
		return FALSE;

	SISO_DirEnt sDirEnt;

	const s32 nOffset = m_pIso->ReadDirEnt( sDirEnt , a_data.name , a_handle->m_DirEnt , a_handle->m_nOffset , TRUE );
	if( nOffset < 0 )
		return FALSE;

	if( sDirEnt.len == 0 )
	{
		SetErr( L"OK" );
		return FALSE;
	}

	a_handle->m_nOffset = nOffset;

	a_data.size = L2H(sDirEnt.size_LE);
	a_data.lba = L2H(sDirEnt.lba_LE);
	a_data.isDir = (sDirEnt.attr & 2) == 2;
	a_data.time = sDirEnt.time;

	return TRUE;
}

void CISO_App::CloseFindIsoFile( SIsoFileFind* a_handle )
{
	TSIsoFileFindMgrVector::iterator it = std::find( m_Objs.begin() , m_Objs.end() , a_handle );
	if( m_Objs.end() != it )
	{
		m_Objs.erase(it);
		g_IsoFileFindMgr.Free( a_handle );
	}
}

BOOL CISO_App::ImportFilePackage( BOOL& a_bIsoBreak , CWQSG_xFile& a_InFp , BOOL a_bCheckCrc32 )
{
	a_bIsoBreak = FALSE;
	SetErr( L"" );

	SWQSG_IsoPatch_Head head;
	if( sizeof(head) != a_InFp.Read( &head , sizeof(head) ) )
	{
		SetErr( GetLangString(21) );
		return FALSE;
	}

	if( 0 != memcmp( head.m_Magic , DEF_WQSG_IsoPatch_Head_Magic , sizeof(head.m_Magic) ) )
	{
		SetErr( GetLangString(20) , DEF_WQSG_IsoPatch_Head_Magic );
		return FALSE;
	}

	if( memcmp( head.m_Ver , "1.0" , 4 ) != 0 )
	{
		SetErr( GetLangString(19) );
		return FALSE;
	}

	{
		const u32 uCrc32_old = head.m_uCrc32;
		head.m_uCrc32 = 0;

		CCrc32 crc32;
		const u32 uCrc32_new = crc32.GetCrc32( (u8*)&head , sizeof(head) );
		if( uCrc32_new != uCrc32_old )
		{
			SetErr( GetLangString(18) );
			return FALSE;
		}
	}

	if( head.m_nSize < sizeof(head) )
	{
		SetErr( GetLangString(17) );
		return FALSE;
	}

	const n64 nFileSize = a_InFp.GetFileSize();
	if( nFileSize < head.m_nSize )
	{
		SetErr( GetLangString(16) );
		return FALSE;
	}

	{
		SISO_Head2048 head_self;
		if( !GetHead( head_self ) )
		{
			SetErr( GetLangString(1) );
			return FALSE;
		}

		if( memcmp( head_self.SystemID , head.m_Head.SystemID , sizeof(head_self.SystemID) ) != 0 ||
			memcmp( head_self.AppUse , head.m_Head.AppUse , sizeof(head_self.AppUse) ) != 0 )
		{
			SetErr( GetLangString(15) );
			return FALSE;
		}
	}

	a_bCheckCrc32 = a_bCheckCrc32 && (head.m_uMask&E_WIPM_CRC32);

	n32 nFileCount = 0;
	s64 offset64 = sizeof(head);
	while ( offset64 < head.m_nSize )
	{
		SWQSG_IsoPatch_Block blockInfo;

		a_InFp.Seek( offset64 );

		if( sizeof(blockInfo) != a_InFp.Read( &blockInfo , sizeof(blockInfo) ) )
		{
			SetErr( GetLangString(9) );
			return FALSE;
		}

		if( blockInfo.m_uSize != sizeof(blockInfo) )
		{
			SetErr( GetLangString(14) );
			return FALSE;
		}

		const u32 uCrc32Src = blockInfo.m_uCrc32;
		blockInfo.m_uCrc32 = 0;

		CCrc32 crc32;
		const u32 uCrc32New = crc32.GetCrc32( (u8*)&blockInfo , blockInfo.m_uSize );

		if( uCrc32New != uCrc32Src )
		{
			SetErr( GetLangString(13) );
			return FALSE;
		}

		offset64 += (blockInfo.m_uFileSize + sizeof(blockInfo));
		nFileCount++;
	}

	if( nFileCount != head.m_nFileCount )
	{
		SetErr( GetLangString(12) );
		return FALSE;
	}

	if( a_bCheckCrc32 )
	{
		offset64 = sizeof(head);

		while ( offset64 < head.m_nSize )
		{
			SWQSG_IsoPatch_Block blockInfo;

			a_InFp.Seek( offset64 );

			if( sizeof(blockInfo) != a_InFp.Read( &blockInfo , sizeof(blockInfo) ) )
			{
				SetErr( GetLangString(9) );
				return FALSE;
			}

			SISO_DirEnt tDirEnt_Dir;
			if( !GetPathDirEnt( tDirEnt_Dir , blockInfo.m_PathName ) )
			{
				return FALSE;
			}

			SISO_DirEnt dirEnt_File;

			if( m_pIso->FindFile( dirEnt_File , tDirEnt_Dir , blockInfo.m_FileName ) < 0 )
			{
				if( blockInfo.m_uOldFileCrc32 != 0 )
				{
					SetErr( L"blockInfo.m_uOldFileCrc32 != 0" );
					return FALSE;
				}
			}
			else
			{
				CCrc32 crc32_v;

				s32 len = L2H(dirEnt_File.size_LE);
				s32 offset32 = 0;

				CWQSG_memFile mfp;

				u32 uCrc32 = 0;

				while( len > 0 )
				{
					const s32 rLen = (len > 1024*512)?1024*512:len;
					len -= rLen;

					if( !m_pIso->ReadFile( dirEnt_File , mfp , rLen , offset32 ) )
					{
						return FALSE;
					}

					uCrc32 = crc32_v.GetCrc32( (u8*)mfp.GetBuf() , rLen );

					mfp.Seek(0);

					offset32 += rLen;
				}

				if( uCrc32 != blockInfo.m_uOldFileCrc32 )
				{
					SetErr( GetLangString(10) , blockInfo.m_PathName , blockInfo.m_FileName , uCrc32 , blockInfo.m_uOldFileCrc32 );
					return FALSE;
				}
			}

			offset64 += (blockInfo.m_uFileSize + sizeof(blockInfo));
		}
	}
	offset64 = sizeof(head);
	while ( offset64 < head.m_nSize )
	{
		SWQSG_IsoPatch_Block blockInfo;

		a_InFp.Seek( offset64 );

		if( sizeof(blockInfo) != a_InFp.Read( &blockInfo , sizeof(blockInfo) ) )
		{
			SetErr( GetLangString(9) );
			return FALSE;
		}

		CWQSG_TempMapFile fp;
		if( !fp.Init( &a_InFp , offset64 + sizeof(blockInfo) , blockInfo.m_uFileSize ) )
		{
			SetErr( GetLangString(8) );
			return FALSE;
		}

		CStringW str;
		str.Format( L"%hs/%hs" , blockInfo.m_PathName , blockInfo.m_FileName );

		if( !zzz_WriteFile( a_bIsoBreak , str , fp , blockInfo.m_FileName , blockInfo.m_PathName , 0 , TRUE , &blockInfo.m_time ) )
		{
			m_strLastErrW.Insert( 0 , GetLangString(7) );
			return FALSE;
		}

		offset64 += (blockInfo.m_uFileSize + sizeof(blockInfo));
	}

	return TRUE;
}

BOOL CISO_App::MakeFilePackage( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , BOOL a_bCheckCrc32 )
{
	if( !a_Iso.IsOpen() )
	{
		SetErr( GetLangString(6) );
		return FALSE;
	}

	if( !IsOpen() )
	{
		SetErr( GetLangString(5) );
		return FALSE;
	}

	if( GetIsoType() != a_Iso.GetIsoType() )
	{
		SetErr( GetLangString(4) );
		return FALSE;
	}

	{
		SISO_Head2048 head1;
		SISO_Head2048 head2;

		if( !a_Iso.GetHead( head1 ) ||
			!GetHead( head2 ) ||
			0 != memcmp( head1.AppUse , head2.AppUse , sizeof(head1.AppUse) ) )
		{
			SetErr( GetLangString(3) );
			return FALSE;
		}
	}

	const n64 nStartOffset = a_OutFp.Tell();
	SWQSG_IsoPatch_Head head = {};
	if( sizeof(head) != a_OutFp.Write( &head , sizeof(head) ) )
	{
		SetErr( GetLangString(2) );
		return FALSE;
	}

	memcpy( head.m_Magic , DEF_WQSG_IsoPatch_Head_Magic , sizeof(head.m_Magic) );
	WQSG_strcpy( "1.0" , (char*)head.m_Ver );
	if( a_bCheckCrc32 )
		head.m_uMask |= E_WIPM_CRC32;

	if( !GetHead( head.m_Head ) )
	{
		SetErr( GetLangString(1) );
		return FALSE;
	}

	if( !zzz_MakeFilePackage_Path( a_Iso , a_OutFp , "" , a_bCheckCrc32 , head ) )
	{
		//m_strLastErr = L"写补丁文件失败";
		return FALSE;
	}

	const n64 nEndOffset = a_OutFp.Tell();

	head.m_nSize = (nEndOffset - nStartOffset);

	CCrc32 crc32;
	head.m_uCrc32 = crc32.GetCrc32( (u8*)&head , sizeof(head) );

	a_OutFp.Seek( nStartOffset );
	if( sizeof(head) != a_OutFp.Write( &head , sizeof(head) ) )
	{
		SetErr( GetLangString(36) );
		return FALSE;
	}

	a_OutFp.Seek( nEndOffset );

	return TRUE;
}

BOOL CISO_App::zzz_MakeFilePackage_Path( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 , SWQSG_IsoPatch_Head& a_Head )
{
	typedef stdext::hash_map<std::string,SIsoFileData> TMap1;
	TMap1 fileList_Self;
	TMap1 fileList_Old;

	SIsoFileFind* pFind_Old = NULL;
	SIsoFileFind* pFind_Self = NULL;

	BOOL rt = FALSE;

	SISO_DirEnt sDirEnt_Path_self;
	SISO_DirEnt sDirEnt_Path_old;

	if( !GetPathDirEnt( sDirEnt_Path_self , a_strPath ) )
	{
		goto __gtErrExit;
	}

	if( !a_Iso.GetPathDirEnt( sDirEnt_Path_old , a_strPath ) )
	{
		goto __gtErrExit;
	}
	{
		SIsoFileData data;

		pFind_Self = FindIsoFile( a_strPath );
		if( !pFind_Self )
		{
			goto __gtErrExit;
		}

		pFind_Old = a_Iso.FindIsoFile( a_strPath );
		if( !pFind_Old )
		{
			goto __gtErrExit;
		}

		while( FindNextIsoFile( pFind_Self , data ) )
		{
			const CStringA strPathName( a_strPath + "/" + data.name );

			if( data.isDir )
			{
				if( !zzz_MakeFilePackage_Path( a_Iso , a_OutFp , strPathName , a_bCheckCrc32 , a_Head ) )
				{
					goto __gtErrExit;
				}
			}
			else
			{
				CStringA strName(data.name);
				strName.MakeLower();

				fileList_Self[std::string(strName.GetString())] = data;
			}
		}

		while( a_Iso.FindNextIsoFile( pFind_Old , data ) )
		{
			const CStringA strPathName( a_strPath + "/" + data.name );

			if( data.isDir )
			{
			}
			else
			{
				CStringA strName(data.name);
				strName.MakeLower();

				fileList_Old[std::string(strName.GetString())] = data;
			}
		}
	}

	CloseFindIsoFile( pFind_Self );
	pFind_Self = NULL;
	CloseFindIsoFile( pFind_Old );
	pFind_Old = NULL;

	for( TMap1::iterator it_self = fileList_Self.begin() ;
		it_self != fileList_Self.end() ; )
	{
		TMap1::iterator it_old = fileList_Old.find( it_self->first );
		if( it_old == fileList_Old.end() )
		{
			//新加的文件
			const SIsoFileData& data_self = it_self->second;

			SISO_DirEnt dirEnt_self;

			CCrc32 crc32_v;

			if( m_pIso->FindFile( dirEnt_self , sDirEnt_Path_self , data_self.name ) < 0 )
				goto __gtErrExit;


			if( !zzz_MakeFilePackage_File( a_Iso , a_OutFp , a_strPath , a_bCheckCrc32 ,
				dirEnt_self , NULL , 0 , 0 , crc32_v , data_self ) )
				goto __gtErrExit;

			a_Head.m_nFileCount++;

			it_self = fileList_Self.erase( it_self );
		}
		else
		{
			const SIsoFileData& data_self = it_self->second;
			const SIsoFileData& data_old = it_old->second;

			if( data_self.size != data_old.size )
			{
				SISO_DirEnt dirEnt_self;
				SISO_DirEnt dirEnt_old;

				CCrc32 crc32_v;

				if( m_pIso->FindFile( dirEnt_self , sDirEnt_Path_self , data_self.name ) < 0 )
					goto __gtErrExit;

				if( a_Iso.m_pIso->FindFile( dirEnt_old , sDirEnt_Path_old , data_old.name ) < 0 )
					goto __gtErrExit;

				if( !zzz_MakeFilePackage_File( a_Iso , a_OutFp , a_strPath , a_bCheckCrc32 ,
					dirEnt_self , &dirEnt_old , data_old.size , 0 , crc32_v , data_self ) )
					goto __gtErrExit;

				a_Head.m_nFileCount++;
			}
			else
			{
				//否则对比文件
				CWQSG_memFile fp_self;
				CWQSG_memFile fp_old;

				SISO_DirEnt dirEnt_self;
				SISO_DirEnt dirEnt_old;

				s32 len = data_old.size;
				s32 offset = 0;

				CCrc32 crc32_v;
				u32 uCrc32_old = 0;

				if( m_pIso->FindFile( dirEnt_self , sDirEnt_Path_self , data_self.name ) < 0 )
					goto __gtErrExit;

				if( a_Iso.m_pIso->FindFile( dirEnt_old , sDirEnt_Path_old , data_old.name ) < 0 )
					goto __gtErrExit;

				while( len > 0 )
				{
					const s32 rLen = (len > 1024*512)?1024*512:len;
					len -= rLen;

					if( !m_pIso->ReadFile( dirEnt_self , fp_self , rLen , offset ) )
						goto __gtErrExit;

					if( !a_Iso.m_pIso->ReadFile( dirEnt_old , fp_old , rLen , offset ) )
						goto __gtErrExit;

					const BOOL bEQ = ( 0 == memcmp( fp_self.GetBuf() , fp_old.GetBuf() , rLen ) );

					if( a_bCheckCrc32 )
						uCrc32_old = crc32_v.GetCrc32( (u8*)fp_old.GetBuf() , rLen );

					fp_self.Seek(0);
					fp_old.Seek(0);

					offset += rLen;

					if( !bEQ )
					{
						if( !zzz_MakeFilePackage_File( a_Iso , a_OutFp , a_strPath , a_bCheckCrc32 ,
							dirEnt_self , &dirEnt_old , len , offset , crc32_v , data_self ) )
							goto __gtErrExit;

						a_Head.m_nFileCount++;
						break;
					}
				}
			}
			it_self = fileList_Self.erase( it_self );
			it_old = fileList_Old.erase( it_old );
		}
	}

	rt = TRUE;
__gtErrExit:
	if( pFind_Self )
		CloseFindIsoFile( pFind_Self );
	if( pFind_Old )
		CloseFindIsoFile( pFind_Old );
	return rt;
}

BOOL CISO_App::zzz_MakeFilePackage_File( CISO_App& a_Iso , CWQSG_xFile& a_OutFp , CStringA a_strPath , BOOL a_bCheckCrc32 ,
							  const SISO_DirEnt& a_dirEnt_self , const SISO_DirEnt* a_pDirEnt_old , s32 a_len , s32 a_offset , CCrc32& a_crc32_v , const SIsoFileData& a_data_self )
{
	SWQSG_IsoPatch_Block blockHead = {};

	blockHead.m_uSize = u16(sizeof(blockHead));
	WQSG_strcpy( a_strPath.GetString() , blockHead.m_PathName );
	WQSG_strcpy( a_data_self.name , blockHead.m_FileName );
	blockHead.m_uFileSize = a_data_self.size;
	blockHead.m_time = a_data_self.time;

	if( a_bCheckCrc32 && a_pDirEnt_old )
	{
		CWQSG_memFile mfp;

		while( a_len > 0 )
		{
			const s32 rLen = (a_len > 1024*512)?1024*512:a_len;
			a_len -= rLen;

			if( !a_Iso.m_pIso->ReadFile( *a_pDirEnt_old , mfp , rLen , a_offset ) )
				return FALSE;

			blockHead.m_uOldFileCrc32 = a_crc32_v.GetCrc32( (u8*)mfp.GetBuf() , rLen );

			mfp.Seek(0);
			a_offset += rLen;
		}

		blockHead.m_uOldFileCrc32 = a_crc32_v.GetCrc32( NULL , 0 );
	}

	a_crc32_v.NEW_CRC32();
	blockHead.m_uCrc32 = a_crc32_v.GetCrc32( (u8*)&blockHead , sizeof(blockHead) );

	//const n64 nStartOffset = a_OutFp.Tell();

	if( sizeof(blockHead) != a_OutFp.Write( &blockHead , sizeof(blockHead) ) )
		return FALSE;

	if( !m_pIso->ReadFile( a_dirEnt_self , a_OutFp , a_data_self.size , 0 ) )
		return FALSE;

	//const n64 nEndOffset = a_OutFp.Tell();
	return TRUE;
}

u32 CISO_App::GetDefaultLangStringCount()
{
	return sizeof(g_ISO_App_String)/sizeof(*g_ISO_App_String);
}

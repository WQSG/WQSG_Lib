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
#include "../ISO/WQSG_PsxISO.h"
#include "../ISO/WQSG_UMD.h"
#include <algorithm>
#include <map>
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
			if( m_nFileOffset >= 0 && m_nFileOffset < m_nFileSzie )
			{
				const s64 xLen = m_nFileSzie - m_nFileOffset;
				u32 uLen = (xLen < len)?u32(xLen):len;

				return m_pFile->Read( lpBuffre , uLen );
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
	//=================================================
	BOOL Init( CWQSG_xFile* a_pFile , s64 a_nStartOffset , s64 a_nFileSize )
	{
		Close();

		if( NULL == a_pFile || !a_pFile->IsOpen() )
			return FALSE;

		const s64 size = a_pFile->GetFileSize();

		if( size < 0 || a_nStartOffset < size )
			return FALSE;

		const s64 xSize = size - a_nStartOffset;
		if( a_nFileSize < 0 || a_nFileSize > xSize )
			return FALSE;

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

static CSIsoFileFindMgr m_IsoFileFindMgr;
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
CISO_App::CISO_App(void)
: m_pIso(NULL)
{
}

CISO_App::~CISO_App(void)
{
	CloseISO();
}

BOOL CISO_App::OpenISO( CStringW a_ISO_PathName , const BOOL a_bCanWrite , EWqsgIsoType a_eType )
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
		return FALSE;
	}

	if( NULL == m_pIso )
		return FALSE;

	if( !m_pIso->OpenISO( a_ISO_PathName , a_bCanWrite ) )
	{
		m_strLastErr = a_ISO_PathName + L"\r\n" + m_pIso->GetErrStr();
		return FALSE;
	}
	return TRUE;
}

inline BOOL CISO_App::GetPathDirEnt( SISO_DirEnt& a_tDirEnt , const CStringA a_path )
{
	if( NULL == m_pIso )
		return FALSE;

	CStringA path( a_path );

	if( !m_pIso->GetRootDirEnt( a_tDirEnt ) )
	{
		m_strLastErr = L"读取根目录失败";
		return FALSE;
	}

	if( path.Right( 1 ) != L'/' )
		path += L'/';

	SISO_DirEnt tmp;

	while( path[0] == '/' )
		path.Delete( 0 , 1 );

	int pos;
	while( (pos = path.Find( '/' )) > 0 )
	{
		CStringA name( path.Left( pos ) );

		path.Delete( 0 , pos );
		while( path[0] == '/' )
			path.Delete( 0 , 1 );

		if( m_pIso->FindFile( tmp , a_tDirEnt , name.GetString() ) < 0 )
		{
			CStringW str;str = a_path;
			m_strLastErr =  str + L"\r\n路经错误";
			return FALSE;
		}

		if( 2 != (tmp.attr & 2) )
		{
			CStringW str;str = a_path;
			m_strLastErr =  str + L"\r\n路经错误";
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
			//			CStringW strNameW;
			//			MessageBox( L"文件名只能用 字母 数字 点 下划线" , strNameW = strName );
			return FALSE;
		}
	}

	return TRUE;
}

inline BOOL CISO_App::zzz_CreateDir( CStringW a_strPathName , CStringA a_strName , CStringA a_strPath )
{
	if( NULL == m_pIso )
		return FALSE;

	if( !TestName(a_strName) )
	{
		CStringW strNameW;strNameW = a_strName;
		m_strLastErr = strNameW + L"\r\n目录名只能用 字母 数字 点 下划线" ;
		return FALSE;
	}

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_strPath ) )
	{
		CStringW strNameW;strNameW = a_strPath;
		m_strLastErr = strNameW + L"\r\nISO路径错误?";
		return FALSE;
	}

	if( !m_pIso->CreateDir( sDirEnt_Path , a_strName ) )
	{
		CStringW strNameW;strNameW = a_strName;
		m_strLastErr = strNameW + L"\r\n创建目录失败" ;
		return FALSE;
	}

	return TRUE;
}

inline BOOL CISO_App::zzz_导入文件夹( CStringW a_strPathName , CStringA path )
{
	CStringW strPathName( a_strPathName );
	{
		while( strPathName.Right(1) == L'\\' )
			strPathName.Delete( strPathName.GetLength() -1 );

		const int pos = strPathName.ReverseFind( L'\\' );
		if( pos <= 0 )
		{
			m_strLastErr = a_strPathName + L"\r\n目录路径错误" ;
			return FALSE;
		}

		CString nameX = strPathName.Mid( pos + 1 );

		CStringA nameA;nameA = nameX;

		if( !zzz_CreateDir( strPathName , nameA , path ) )
			return FALSE;

		path += ('/' + nameA);

		strPathName += L'\\';
	}

	BOOL rt = TRUE;
	WIN32_FIND_DATAW data;

	const HANDLE handle = ::FindFirstFileW( strPathName + L"*.*" , &data );
	if( INVALID_HANDLE_VALUE != handle )
	{
		do{
			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CStringW nameW( data.cFileName );
				if( nameW != L"." && nameW != L".." )
				{
					if( !导入文件( strPathName + data.cFileName , path , 0 ) )
					{
						rt = FALSE;
						break;
					}
				}
			}
			else
			{
				if( !导入文件( strPathName + data.cFileName , path , 0 ) )
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

BOOL CISO_App::导入文件( CStringW a_strPathName , CStringA a_path , const s32 a_offset )
{
	CStringA strNameA;
	{
		const int pos = a_strPathName.ReverseFind( '\\' );
		if( pos <= 0 )
			return FALSE;

		CString strNameW( a_strPathName.Mid( pos + 1 ) );
		strNameA = strNameW;
	}

	if( ::WQSG_IsFile( a_strPathName.GetString() ) )
	{
		CWQSG_File fp;
		if( !fp.OpenFile( a_strPathName.GetString() , 1 , 3 ) )
		{
			m_strLastErr = a_strPathName + L"\r\n打开文件失败";
			return FALSE;
		}

		if( !zzz_WriteFile( a_strPathName , fp , strNameA , a_path , a_offset , TRUE ) )
			return FALSE;
	}
	else if( ::WQSG_IsDir( a_strPathName.GetString() ) )
	{
		if( !zzz_导入文件夹( a_strPathName , a_path ) )
			return FALSE;
	}
	else
	{
		m_strLastErr = a_strPathName + L"\r\n文件不存在" ;
		return FALSE;
	}
	return TRUE;
}

inline BOOL CISO_App::zzz_WriteFile( CStringW strPathName , CWQSG_xFile& a_InFp , CStringA strName ,
							 CStringA strPath , const s32 offset , const BOOL isNew  )
{
	if( NULL == m_pIso )
		return FALSE;

	if( !TestName(strName) )
	{
		CStringW strNameW ; strNameW = strName;
		m_strLastErr = strNameW + L"\r\n文件名只能用 字母 数字 点 下划线" ;
		return FALSE;
	}

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , strPath ) )
	{
		CStringW strNameW ; strNameW = strPath;
		m_strLastErr = strNameW + L"\r\nISO路径错误?" ;
		return FALSE;
	}

// 	CWQSG_File fp;
// 	if( !fp.OpenFile( strPathName.GetString() , 1 , 3 ) )
// 	{
// 		m_strLastErr = strPathName + L"\r\n打开文件失败";
// 		return FALSE;
// 	}

	const s64 srcFileSize = a_InFp.GetFileSize();
	if( ( srcFileSize < 0 ) || ( srcFileSize > (((u32)-1))>>1) )
	{
		m_strLastErr = strPathName + L"\r\n文件大小错误" ;
		return FALSE;
	}

	if( !m_pIso->WriteFile( sDirEnt_Path , strName , a_InFp , (s32)srcFileSize , offset , isNew , FALSE ) )
	{
		m_strLastErr = strPathName + L"\r\n写文件到ISO出错" ;
		return FALSE;
	}

	return TRUE;
}

BOOL CISO_App::导出文件( CStringW a_strPathName , CStringA a_pathA , CStringA a_nameA )
{
	if( NULL == m_pIso )
		return FALSE;

	::DeleteFile( a_strPathName );

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_pathA ) )
		return FALSE;

	SISO_DirEnt sDirEnt_File;
	if( m_pIso->FindFile( sDirEnt_File , sDirEnt_Path , a_nameA.GetString() ) < 0 )
		return FALSE;

	CWQSG_File fp;
	if( !fp.OpenFile( a_strPathName.GetString() , 4 , 3 ) )
	{
		m_strLastErr = a_strPathName + L"\r\n打开文件失败" ;
		return FALSE;
	}

	if( !m_pIso->ReadFile( sDirEnt_Path , a_nameA , fp , sDirEnt_File.size_le , 0 ) )
	{
		fp.Close();
		::DeleteFile( a_strPathName );
		m_strLastErr = a_strPathName + L"\r\n导出文件失败" ;
		return FALSE;
	}

	return TRUE;
}

BOOL CISO_App::导出文件夹( CStringW a_strPath , CStringA a_pathA )
{
	if( NULL == m_pIso )
		return FALSE;

	SIsoFileFind* pFind = FindIsoFile( a_pathA );
	if( !pFind )
	{
		m_strLastErr = a_pathA;
		m_strLastErr + L"\r\n找不到文件夹" ;
		return FALSE;
	}

	if( a_strPath.Right(1) != L'\\' )
		a_strPath += L'\\';

	if( a_pathA.Right(1) != L'/' )
		a_pathA += L'/';

	if ( !WQSG_CreateDir( a_strPath.GetString() ) )
	{
		m_strLastErr = a_strPath + L"\r\n创建文件夹失败" ;
		return FALSE;
	}

	BOOL bRt = TRUE;
	CStringW strPathName;
	SIsoFileData data;

	while( FindNextIsoFile( pFind , data ) )
	{
		strPathName = data.name;
		strPathName.Insert( 0 , a_strPath );

		if( data.isDir )
		{
			if( !导出文件夹( a_strPath , a_pathA + data.name ) )
			{
				bRt = FALSE;
				goto __gtSaveDirExit;
			}
		}
		else
		{
			if( !导出文件( a_strPath , a_pathA , data.name ) )
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

BOOL CISO_App::写文件偏移( CStringA a_pathA , CStringA a_nameA , s32 a_oldOffset , CStringW a_inFileName )
{
	SISO_DirEnt sDirEnt_File;
	if( !zzz_GetFileData( sDirEnt_File , a_pathA , a_nameA ) )
		return FALSE;

	if( sDirEnt_File.attr & 2 )
	{
		CStringW str;str = a_pathA + a_nameA;
		m_strLastErr = str + L"\r\n不能写文件夹";
		return FALSE;
	}

	CWQSG_File fp;
	if( !fp.OpenFile( a_inFileName.GetString() , 1 , 3 ) )
	{
		m_strLastErr = a_inFileName + L"\r\n打开文件失败";
		return FALSE;
	}

	return zzz_WriteFile( a_inFileName , fp , a_nameA , a_pathA , a_oldOffset , FALSE );
}

BOOL CISO_App::替换文件( CStringA a_pathA , CStringA a_nameA , CStringW a_inFileName )
{
	CWQSG_File fp;
	if( !fp.OpenFile( a_inFileName.GetString() , 1 , 3 ) )
	{
		m_strLastErr = a_inFileName + L"\r\n打开文件失败";
		return FALSE;
	}

	return zzz_WriteFile( a_inFileName , fp , a_nameA , a_pathA , 0 , TRUE );
}

inline BOOL CISO_App::zzz_GetFileData( SISO_DirEnt& a_tDirEnt , CStringA a_pathA , CStringA a_nameA )
{
	if( NULL == m_pIso )
		return FALSE;

	SISO_DirEnt sDirEnt_Path;
	if( !GetPathDirEnt( sDirEnt_Path , a_pathA ) )
		return FALSE;

	if( a_nameA[0] != 0 )
	{
		if( m_pIso->FindFile( a_tDirEnt , sDirEnt_Path , a_nameA.GetString() ) < 0 )
		{
			CStringW str;str = a_pathA + a_nameA;
			m_strLastErr = str + L"\r\nISO没有此文件";
			return FALSE;
		}
	}
	else
		a_tDirEnt = sDirEnt_Path;

	return TRUE;
}

BOOL CISO_App::GetFileData( SIsoFileData& a_data , CStringA a_pathA , CStringA a_nameA  )
{
	SISO_DirEnt tmp;
	if( zzz_GetFileData( tmp , a_pathA , a_nameA ) )
	{
		a_data.isDir = ( (tmp.attr & 2) == 2 );
		a_data.size = tmp.size_le;
		a_data.lba = tmp.lba_le;

		WQSG_strcpy( a_nameA.GetString() , a_data.name );

		return TRUE;
	}

	return FALSE;
}

SIsoFileFind* CISO_App::FindIsoFile( CStringA a_pathA )
{
	SISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , a_pathA ) )
		return NULL;

	SIsoFileFind* pRt = m_IsoFileFindMgr.Allocate();
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
		m_strLastErr = "OK";
		return FALSE;
	}

	a_handle->m_nOffset = nOffset;

	a_data.size = sDirEnt.size_le;
	a_data.lba = sDirEnt.lba_le;
	a_data.isDir = (sDirEnt.attr & 2) == 2;

	return TRUE;
}

void CISO_App::CloseFindIsoFile( SIsoFileFind* a_handle )
{
	TSIsoFileFindMgrVector::iterator it = std::find( m_Objs.begin() , m_Objs.end() , a_handle );
	if( m_Objs.end() != it )
	{
		m_Objs.erase(it);
		m_IsoFileFindMgr.Free( a_handle );
	}
}

BOOL CISO_App::导入文件包( CWQSG_xFile& a_InFp )
{
	SWQSG_IsoPatch_Head head;
	if( sizeof(head) != a_InFp.Read( &head , sizeof(head) ) )
	{
		return FALSE;
	}

	if( 0 != memcmp( head.m_Magic , DEF_WQSG_IsoPatch_Head_Magic , sizeof(head.m_Magic) ) )
	{
		return FALSE;
	}

	if( memcmp( head.m_Ver , "1.0" , 4 ) != 0 )
		return FALSE;

	if( head.m_nSize < sizeof(head) )
		return FALSE;

	const n64 nFileSize = a_InFp.GetFileSize();
	if( nFileSize < head.m_nSize )
		return FALSE;

	n32 nFileCount = 0;
	s64 offset = sizeof(head);
	while ( offset < head.m_nSize )
	{
		SWQSG_IsoPatch_Block blockInfo;

		a_InFp.Seek( offset );

		if( sizeof(blockInfo) != a_InFp.Read( &blockInfo , sizeof(blockInfo) ) )
			return FALSE;

		if( blockInfo.m_uSize != sizeof(blockInfo) )
			return FALSE;

		const u32 uCrc32Src = blockInfo.m_uCrc32;
		blockInfo.m_uCrc32 = 0;

		_m_CRC32 crc32;
		const u32 uCrc32New = crc32.GetCRC32( (u8*)&blockInfo , blockInfo.m_uSize );

		if( uCrc32New != uCrc32Src )
			return FALSE;

		offset += blockInfo.m_uFileSize;
		nFileCount++;
	}

	if( nFileCount != head.m_nFileCount )
	{
		return FALSE;
	}

	offset = sizeof(head);
	while ( offset < head.m_nSize )
	{
		SWQSG_IsoPatch_Block blockInfo;

		a_InFp.Seek( offset );

		if( sizeof(blockInfo) != a_InFp.Read( &blockInfo , sizeof(blockInfo) ) )
			return FALSE;

		CWQSG_TempMapFile fp;
		if( !fp.Init( &a_InFp , offset + sizeof(blockInfo) , blockInfo.m_uFileSize ) )
			return FALSE;

		if( !zzz_WriteFile( L"" , fp , blockInfo.m_FileName , blockInfo.m_PathName , 0 , true ) )
			return TRUE;

		offset += blockInfo.m_uFileSize;
	}

	return FALSE;
}

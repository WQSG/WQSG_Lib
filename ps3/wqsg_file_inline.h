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
inline u32 CWQSG_File::Read( void*const lpBuffre , const u32 len )
{
	uint64_t read;
	m_iErrorId = cellFsRead( m_hFile , lpBuffre , len , &read );

	return read;
}

inline u32 CWQSG_File::Write		( void const*const lpBuffre , const u32 len )
{
	uint64_t write;
	m_iErrorId = cellFsWrite( m_hFile , lpBuffre , len , &write );

	return write;
}

inline void CWQSG_File::Close( void )
{
	m_iErrorId = CELL_FS_SUCCEEDED;
	if( m_hFile )
	{
		m_iErrorId = cellFsClose( m_hFile );
		m_hFile = 0;
	}
	m_dwDesiredAccess = 0;
}

inline s64 CWQSG_File::GetFileSize	( void )const
{
	CellFsStat stat;
	m_iErrorId = cellFsFstat( m_hFile , &stat );

	return (m_iErrorId == CELL_FS_SUCCEEDED)?stat.st_size:-1;
}

inline bool CWQSG_File::SetFileLength( const s64 Length )
{
	m_iErrorId = cellFsFtruncate( m_hFile , Length );
	return m_iErrorId == CELL_FS_SUCCEEDED;
}

inline s64 CWQSG_File::Tell		( void )const
{
	u64 pos;
	m_iErrorId = cellFsLseek( m_hFile , 0 , CELL_FS_SEEK_CUR , &pos );
	return (m_iErrorId == CELL_FS_SUCCEEDED)?pos:-1;
}

inline bool CWQSG_File::Seek		( const s64 offset )
{
	u64 pos;
	m_iErrorId = cellFsLseek( m_hFile , offset , CELL_FS_SEEK_SET , &pos );
	return m_iErrorId == CELL_FS_SUCCEEDED;
}

inline u32 CWQSG_File::GetCRC32	( void )
{
	return 0;
}

inline bool CWQSG_File::IsOpen		( void )const
{
	return m_hFile != 0;
}

inline bool CWQSG_File::IsCanRead	( void )const
{
	return m_dwDesiredAccess & GENERIC_READ;
}

inline bool CWQSG_File::IsCanWrite	( void )const
{
	return m_dwDesiredAccess & GENERIC_WRITE;
}

inline bool CWQSG_File::OpenFile( char const*const lpFileName , const int a_MODE )
{
	Close();

	int flags = 0;

	switch(a_MODE)
	{
	case 1://只读,不存在失败
		m_dwDesiredAccess = GENERIC_READ;
		flags = CELL_FS_O_RDONLY;
		break;

	case 2://只写,不存在失败
		m_dwDesiredAccess = GENERIC_WRITE;
		flags = CELL_FS_O_WRONLY;
		break;

	case 3://读写,不存在失败
		m_dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		flags = CELL_FS_O_RDWR;
		break;

	case 4://重写,存在清空,不存在新建
		m_dwDesiredAccess = GENERIC_WRITE;
		flags = CELL_FS_O_WRONLY | CELL_FS_O_CREAT | CELL_FS_O_TRUNC ;
		break;

/*	case 5://改写,存在直接打开,不存在新建
		dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = OPEN_ALWAYS;
		break;

	case 6://改读写,存在直接 打开,不存在新建
		dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		dwCreationDisposition = OPEN_ALWAYS;
		break;*/

	case 9:
		m_dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
		flags = CELL_FS_O_RDWR | CELL_FS_O_CREAT | CELL_FS_O_APPEND;
		break;

	default:
		m_iErrorId = CELL_FS_EINVAL;
		return false;
	}

	m_iErrorId = cellFsOpen( lpFileName , flags , &m_hFile , NULL , 0 );

	return m_iErrorId == CELL_FS_SUCCEEDED;
}

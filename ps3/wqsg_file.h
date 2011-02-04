#ifndef __WQSG_FILE_H__
#define __WQSG_FILE_H__

#include <cell/cell_fs.h>
#include "../Interface/wqsg_ifile.h"
//------------------------------------------
enum
{
	GENERIC_READ = 1,
	GENERIC_WRITE = 2,
};

class CWQSG_File : public CWQSG_IFile
{
	mutable CellFsErrno m_iErrorId;
	__i__	int			m_hFile;
	__i__	u32			m_dwDesiredAccess;
public:
	inline CWQSG_File( void )
		: m_iErrorId(0)
		, m_hFile(0)
		, m_dwDesiredAccess(0)
	{
	}

	virtual	~CWQSG_File( void )
	{
		Close();
	}
	//----------------------------------------------------
	inline virtual	u32			Read		( void*const lpBuffre , const u32 len )override;
	inline virtual	u32			Write		( void const*const lpBuffre , const u32 len )override;
	inline virtual	void		Close		( void )override;
	inline virtual	s64			GetFileSize	( void )const override;
	inline virtual	bool		SetFileLength( const s64 Length )override;
	inline virtual	s64			Tell		( void )const override;
	inline virtual	bool		Seek		( const s64 offset )override;
	inline virtual	u32			GetCRC32	( void )override;
	inline virtual	bool		IsOpen		( void )const override;
	inline virtual	bool		IsCanRead	( void )const override;
	inline virtual	bool		IsCanWrite	( void )const override;
	//---------------------------------------------------
	inline __i__	bool		OpenFile( char const*const lpFileName , const int a_MODE );
	inline __i__	int 		GetFileHANDLE( void )const;
	inline __i__	CellFsErrno GetError()const
	{
		return m_iErrorId;
	}
};
//------------------------------------------
__i__
#include "wqsg_file_inline.h"

//=========================================
#endif //__WQSG_FILE_H__

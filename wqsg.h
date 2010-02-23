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
#ifndef __WQSG_H__
#define __WQSG_H__
#include <crtdbg.h>

#include "WQSG_def.h"
#include "WQSG_xFile.h"
#include "WQSG_String.h"

#include "CWQSG_str/CWQSG_str.h"
#include "CWD/CWD.h"

#include "WQSG_File_M.h"

#include "Dlg/CWQSGAbout.h"
#include "Dlg/WQSG_DirDlg.h"
//------------------------------------------------------------------
template <typename TYPE_1>
class CWQSG_TypeLinkList
{
	CRITICAL_SECTION	m_cs_mutex;
	inline	void	zzz_Lock()
	{
		::EnterCriticalSection( &m_cs_mutex );
	}
	inline	void	zzz_UnLock()
	{
		::LeaveCriticalSection( &m_cs_mutex );
	}
	//-------------------------------
	struct	tgNode
	{
		TYPE_1	Type;
		tgNode*	next;
		tgNode( const TYPE_1& type ) : next( NULL )	{	Type = type;	}
	};
	tgNode*		m_head;
	int			m_Count;
    tgNode*		m_PosPtr;
    int			m_Pos;
public:
	CWQSG_TypeLinkList():m_head(NULL),m_Count(0),m_PosPtr(NULL),m_Pos(-1)
	{
		::InitializeCriticalSectionAndSpinCount( &m_cs_mutex , 1 );
	}
	virtual	~CWQSG_TypeLinkList()
	{
		DelAll();
		::DeleteCriticalSection( &m_cs_mutex );
	}
	//-------------------------------------------------------------------------------------
	inline	void		DelAll()
    {
		zzz_Lock();
        m_PosPtr = m_head;
        while(m_PosPtr)
        {
            m_head = m_PosPtr->next;
            delete m_PosPtr;
            m_PosPtr = m_head;
        }
        m_Count = 0;
        m_Pos = -1;
		zzz_UnLock();
    }
	inline	int			AddItem( const TYPE_1& type )
    {
#if 0
        for( int target = m_Count - 1 ;m_Pos < target;++m_Pos)
        {
            m_PosPtr = m_PosPtr->next;
        }

        if( m_PosPtr )
        {
            m_PosPtr->next = new tgNode(type);
            if( NULL == m_PosPtr->next )return -1;
            m_PosPtr = m_PosPtr->next;
        }
        else
        {
            m_head = m_PosPtr = new tgNode(type);
            if( NULL == m_PosPtr )return -1;
        }
        return ++m_Count,++m_Pos;
#else
		return InsetItem( -1 , type );
#endif
    }
	inline	int			InsetItem( const int _index , const TYPE_1& type )
    {
		zzz_Lock();
		const int index = ( (unsigned int)_index > (unsigned int)m_Count )?m_Count:_index;
		if(
			( m_Pos >= 0 )//存在
			&& ( 0 != index )
			)
		{
			if( index < m_Pos )//重头来
			{
				m_Pos = 0;
				m_PosPtr = m_head;
			}
			for( int target = index - 1 ;m_Pos < target;++m_Pos)
			{
				m_PosPtr = m_PosPtr->next;
			}
		}
		else
		{
			m_Pos = -1;
			m_PosPtr = NULL;
		}

		//----------------------------------------------
        if( m_PosPtr )
        {//不是文件头
			tgNode* tmp = new tgNode(type);
            if( NULL == tmp )
				goto __gtErr;

			tmp->next = m_PosPtr->next;
			m_PosPtr->next = tmp;
			m_PosPtr = tmp;
        }
        else
        {//是文件头
			m_PosPtr = new tgNode(type);
			if( NULL == m_PosPtr )
				goto __gtErr;

			m_PosPtr->next = m_head;
			m_head = m_PosPtr;
        }
		zzz_UnLock();
        return ++m_Count,++m_Pos;
__gtErr:
		zzz_UnLock();
		return -1;
    }
	inline	bool		GetItem( const int index , TYPE_1& type )
    {
		zzz_Lock();
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
			zzz_UnLock();
			return false;
		}

        if( index < m_Pos )
        {
            m_Pos = 0;
            m_PosPtr = m_head;
        }

        for( ;m_Pos < index;++m_Pos )
        {
            m_PosPtr = m_PosPtr->next;
        }
        type = m_PosPtr->Type;

		zzz_UnLock();
        return true;
    }
	inline	bool		SetItem( const int index , const TYPE_1& type )
    {
		zzz_Lock();
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
			zzz_UnLock();
			return false;
		}

        if( index < m_Pos )
        {
            m_Pos = 0;
            m_PosPtr = m_head;
        }

        for( ;m_Pos < index;++m_Pos )
        {
            m_PosPtr = m_PosPtr->next;
        }
        m_PosPtr->Type = type;

		zzz_UnLock();
        return true;
    }
	inline	bool		DelItem( const int index )
    {
		zzz_Lock();
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
			zzz_UnLock();
			return false;
		}

        tgNode* tmp;
        if( index )//不是 0
        {
            if( index <= m_Pos )
            {
                m_Pos = 0;
                m_PosPtr = m_head;
            }

            for( int target = index - 1 ;m_Pos < target;++m_Pos )
            {
                m_PosPtr = m_PosPtr->next;
            }
            tmp = m_PosPtr->next;
            m_PosPtr->next = tmp->next;
            delete tmp;
            //
            --m_Count;
        }
        else//是 0
        {
            tmp = m_head;
            m_head = tmp->next;
            delete tmp;
            //
            if( --m_Count )
            {
                m_PosPtr = m_head;
                m_Pos = 0;
            }
            else
            {
                m_Pos = -1;
                m_PosPtr = NULL;
            }
        }

		zzz_UnLock();
        return true;
    }
	inline	int			GetCount( void ){   return m_Count; }
	inline	int			GetPos( void ){ return m_Pos;   }
	inline	TYPE_1*		MakeArray( void )
	{
		zzz_Lock();
		TYPE_1* line = new TYPE_1[ m_Count ];
		if( NULL == line )
		{
			zzz_UnLock();
			return NULL;
		}

		tgNode* tmpNode = m_head;

		for( int i = 0 ; i < m_Count ; ++i )
		{
			line[i] = tmpNode->Type;
			tmpNode = tmpNode->next;
		}
		zzz_Lock();
		return line;
	}
};
//------------------------------------------------------------------
#ifdef DEBUG
#define WQSG_DEBUG_MESSAGEBOX(str) MessageBox(str);
#else
#define WQSG_DEBUG_MESSAGEBOX(str)
#endif
//------------------------------------------------------------------
inline BOOL WQSG_LoadResA( const WORD resID , char const*const resType , CWD_BIN& wd_bin )
{
	if( NULL == resType )
		return FALSE;

	if( NULL != wd_bin.BIN )
		return FALSE;

	HINSTANCE h = NULL ; //::AfxGetInstanceHandle();
	const HRSRC hrRes = ::FindResourceA( h , MAKEINTRESOURCEA( resID ) , resType );
	if( NULL == hrRes )
		return FALSE;

	const DWORD size = ::SizeofResource( h , hrRes );
	wd_bin.LEN = size;
	if( ( size != (DWORD)wd_bin.LEN ) || ( wd_bin.LEN < 0 ) )
		return FALSE;

	const HGLOBAL hgdRes = ::LoadResource( h , hrRes );
	if( NULL == hgdRes )
		return FALSE;

	LPVOID lp_void = ::LockResource( hgdRes );
	if( NULL == lp_void )
		return FALSE;

	if( NULL == wd_bin.SetBufSize( wd_bin.LEN ) )
	{
		UnlockResource( lp_void );
		return FALSE;
	}

	memcpy( wd_bin.BIN , lp_void , wd_bin.LEN );

	UnlockResource( lp_void );

	return TRUE;
}
inline BOOL WQSG_LoadResW( const WORD resID , WCHAR const*const resType , CWD_BIN& wd_bin )
{
	if( NULL == resType )
		return FALSE;

	if( NULL != wd_bin.BIN )
		return FALSE;

	HINSTANCE h = NULL ; //::AfxGetInstanceHandle();
	const HRSRC hrRes = ::FindResourceW( h , MAKEINTRESOURCEW( resID ) , resType );
	if( NULL == hrRes )
		return FALSE;

	const DWORD size = ::SizeofResource( h , hrRes );
	wd_bin.LEN = size;
	if( ( size != (DWORD)wd_bin.LEN ) || ( wd_bin.LEN < 0 ) )
		return FALSE;

	const HGLOBAL hgdRes = ::LoadResource( h , hrRes );
	if( NULL == hgdRes )
		return FALSE;

	LPVOID lp_void = ::LockResource( hgdRes );
	if( NULL == lp_void )
		return FALSE;

	if( NULL == wd_bin.SetBufSize( wd_bin.LEN ) )
	{
		UnlockResource( lp_void );
		return FALSE;
	}

	memcpy( wd_bin.BIN , lp_void , wd_bin.LEN );

	UnlockResource( lp_void );

	return TRUE;
}
#ifdef UNICODE
#define WQSG_LoadRes WQSG_LoadResW
#else
#define WQSG_LoadRes WQSG_LoadResA
#endif
//------------------------------------------------------------------
class CWQSG_MSG_W
{
	HWND W_hwnd;
	LPCWSTR W_标题;
public:
	CWQSG_MSG_W(HWND hwnd,LPCWSTR 标题)
	{
		W_hwnd = hwnd;
		W_标题 = 标题;
	}
	~CWQSG_MSG_W(){}
	INT show(LPCWSTR TXT1,UINT 样式 = 0)
	{
		return ::MessageBoxW(W_hwnd,TXT1,W_标题,样式);
	}
	INT show(LPCWSTR TXT1,LPCWSTR TXT2,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + 3;
		WCHAR *const SHOWTXT = new WCHAR[len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		len = ::MessageBoxW(W_hwnd,SHOWTXT,W_标题,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	INT show(LPCWSTR TXT1,LPCWSTR TXT2,LPCWSTR TXT3,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + ::WQSG_strlen((WCHAR*)TXT3) + 5;
		WCHAR *const SHOWTXT = new WCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT3,s1);
		len = ::MessageBoxW(W_hwnd,SHOWTXT,W_标题,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	INT show(LPCWSTR TXT1,LPCWSTR TXT2,LPCWSTR TXT3,LPCWSTR TXT4,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((WCHAR*)TXT1) + ::WQSG_strlen((WCHAR*)TXT2) + ::WQSG_strlen((WCHAR*)TXT3) + ::WQSG_strlen((WCHAR*)TXT4) + 7;
		WCHAR *const SHOWTXT = new WCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((WCHAR*)TXT1,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT2,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT3,s1);
		s1 += ::WQSG_strcpy(L"\15\12",s1);
		s1 += ::WQSG_strcpy((WCHAR*)TXT4,s1);
		len = ::MessageBoxW(W_hwnd,SHOWTXT,W_标题,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	void Set_W_标题(LPCWSTR 标题){W_标题 = 标题;}
	void Set_W_hwnd(HWND hwnd){W_hwnd = hwnd;}
};
class CWQSG_MSG_A
{
	HWND W_hwnd;
	UCHAR*W_TTT;
public:
	CWQSG_MSG_A(HWND hwnd,UCHAR*TTT)
	{
		W_hwnd = hwnd;
		W_TTT = TTT;
	}
	~CWQSG_MSG_A(){}
	INT show(UCHAR*TXT1,UINT 样式 = 0)
	{
		return ::MessageBoxA(W_hwnd,(char*)TXT1,(char*)W_TTT,样式);
	}
	INT show(UCHAR*TXT1,UCHAR*TXT2,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((char*)TXT1) + ::WQSG_strlen((char*)TXT2) + 3;
		UCHAR *const SHOWTXT = new UCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((char*)TXT1,(char*)s1);
		s1 += ::WQSG_strcpy("\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT2,(char*)s1);
		len = ::MessageBoxA(W_hwnd,(char*)SHOWTXT,(char*)W_TTT,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	INT show(UCHAR*TXT1,UCHAR*TXT2,UCHAR*TXT3,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((char*)TXT1) + ::WQSG_strlen((char*)TXT2) + ::WQSG_strlen((char*)TXT3) + 5;
		UCHAR *const SHOWTXT = new UCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((char*)TXT1,(char*)s1);
		s1 += ::WQSG_strcpy("\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT2,(char*)s1);
		s1 += ::WQSG_strcpy("\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT3,(char*)s1);
		len = ::MessageBoxA(W_hwnd,(char*)SHOWTXT,(char*)W_TTT,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	INT show(UCHAR*TXT1,UCHAR*TXT2,UCHAR*TXT3,UCHAR*TXT4,UINT 样式 = 0)
	{
		UINT len = ::WQSG_strlen((char*)TXT1) + ::WQSG_strlen((char*)TXT2) + ::WQSG_strlen((char*)TXT3) + ::WQSG_strlen((char*)TXT4) + 7;
		UCHAR *const SHOWTXT = new UCHAR [len],
			*s1 = SHOWTXT;
		s1 += ::WQSG_strcpy((char*)TXT1,(char*)s1);
		s1 += ::WQSG_strcpy("\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT2,(char*)s1);
		s1 += ::WQSG_strcpy((char*)"\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT3,(char*)s1);
		s1 += ::WQSG_strcpy("\15\12",(char*)s1);
		s1 += ::WQSG_strcpy((char*)TXT4,(char*)s1);
		len = ::MessageBoxA(W_hwnd,(char*)SHOWTXT,(char*)W_TTT,样式);
		delete SHOWTXT;
		return (INT)len;
	}
	void Set_W_TTT(UCHAR*w_ttt){W_TTT = w_ttt;}
	void Set_W_hwnd(HWND w_hwnd){W_hwnd = w_hwnd;}
};
//------------------------------------------------------------------
#include <vector>
template<class _Ttype0>
class CWQSG_vector
{
	typedef std::vector<_Ttype0>	_t_vector;
	_t_vector		m_list;
public:
	void		push( _Ttype0 val )
	{
		int count = (int)m_list.size();
		if( ++count > 0 )
			m_list.push_back( val );
	}
	void		pop( )					{	m_list.pop_back();			}
	_Ttype0&	operator[]( int pos )	{	return m_list[pos];			}
	int			size( )					{	return (int)m_list.size();	}
	bool		empty()					{	return m_list.empty();		}
	//------------------------------------------------------------------
	bool		delItem( int iItem )
	{
		bool rt = false;
		if( (iItem >= 0) && ((_t_vector::size_type)iItem < m_list.size() ) )
		{
			m_list.erase( m_list.begin() + iItem );
			rt = true;
		}
		return rt;
	}
};
//------------------------------------------------------------------
#include "objbase.h"
class CWQSG_ML
{
	void zzz_UnInit(void)
	{
		::CoUninitialize();
	}
protected:
	bool	m_Init;
	CWQSG_ML() : m_Init(false){}
	virtual ~CWQSG_ML(){	zzz_UnInit();	}
	bool zzz_Init(void)
	{
		if( !m_Init )
			m_Init = SUCCEEDED( ::CoInitialize(NULL) );

		return m_Init;
	}
};
class CWQSG_CRITICAL_SECTION
{
	CRITICAL_SECTION	m_cs_mutex;
public:
	inline	CWQSG_CRITICAL_SECTION()
	{
		::InitializeCriticalSectionAndSpinCount( &m_cs_mutex , 1 );
	}
	inline	virtual ~CWQSG_CRITICAL_SECTION()
	{
		::DeleteCriticalSection( &m_cs_mutex );
	}
	//-------------------------------------------------
	inline	void Lock()
	{
		::EnterCriticalSection( &m_cs_mutex );
	}
	inline	void UnLock()
	{
		::LeaveCriticalSection( &m_cs_mutex );
	}
	inline	BOOL Try_Lock()
	{
		return TryEnterCriticalSection( &m_cs_mutex );
	}
};
class CWQSG_Mutex
{
	HANDLE m_Mutex;
public:
	CWQSG_Mutex() : m_Mutex(NULL){}
	virtual~CWQSG_Mutex(){	Destroy();	}
	//-----------------------------------------
	inline	BOOL Create()
	{
		if( m_Mutex != NULL )
			return TRUE;

		m_Mutex = ::CreateMutex( NULL , TRUE , NULL );
		return ( m_Mutex != NULL );
	}
	inline	void Destroy()
	{
		HANDLE handle = m_Mutex;
		m_Mutex = NULL;
		if( handle != NULL )
			::CloseHandle( handle );
	}
	inline	void UnLock()
	{
		::ReleaseMutex( m_Mutex );
	}
	inline	void Lock()
	{
		::WaitForSingleObject( m_Mutex , INFINITE );
	}
};
//------------------------------------------------------------------
class CWQSG_PartitionList
{
	class CLinkList
	{
	public:
		u32			m_start;
		u32			m_len;
		bool		m_use;
		CLinkList*	next;
		CLinkList(): m_start(0),m_len(0),m_use(false),next(NULL){}
	};
	CLinkList	m_head;
	const u32		m_maxLBA;
	//------------------------------------------------
public:
	CWQSG_PartitionList( const u32 maxLBA ): m_maxLBA( maxLBA ){	FreeAll();	}
	~CWQSG_PartitionList(){	FreeAll();	}
	//------------------------------------------------
	inline void FreeAll()
	{
		CLinkList* tmp1 = m_head.next;
		CLinkList* tmp2 = NULL;
		while( tmp1 )
		{
			tmp2 = tmp1->next;
			delete tmp1;
			tmp1 = tmp2;
		}
		m_head.m_start = 0;
		m_head.m_len = m_maxLBA;
		m_head.m_use = false;
		m_head.next = NULL;
	}
	inline bool AllocPos( const s32 st , const u32 len )
	{
		if( ( st < 0 ) || ( len <= 0 ) )
			return false;

		CLinkList* tmp = &m_head;

		do
		{
			if( (u32)st < ( tmp->m_start + tmp->m_len ) )
			{
				if( ( tmp->m_use ) || ( ( st + len ) > ( tmp->m_start + tmp->m_len ) ) )
					return false;
				break;
			}
		}while( tmp = tmp->next );
		if( NULL == tmp )
			return false;

		if( (u32)st > tmp->m_start )
		{//先分掉前面的
			CLinkList* newtmp = new CLinkList;
			if( NULL == newtmp )
				return false;

			newtmp->m_start = st;
			newtmp->m_len = tmp->m_len - ( st - tmp->m_start );
			newtmp->m_use = false;
			newtmp->next = tmp->next;

			tmp->m_len = ( st - tmp->m_start );
			tmp->m_use = false;
			tmp->next = newtmp;
			tmp = newtmp;
		}

		if( tmp->m_len > len )
		{//分掉后面的
			CLinkList* newtmp = new CLinkList;
			if( NULL == newtmp )
				return false;

			newtmp->m_start = st + len;
			newtmp->m_len = tmp->m_len - len;
			newtmp->m_use = false;
			newtmp->next = tmp->next;

			tmp->next = newtmp;
			tmp->m_len = len;
		}
		tmp->m_use = true;

		return true;
	}
	inline s32 Alloc( const u32 len , const u32 align = 1 , const s32 startPos = 0 )
	{
		if( len <= 0 || align == 0 || startPos < 0 )
			return -1;

		CLinkList* tmp = &m_head;

		CLinkList* pPrev = NULL;

		do
		{
			if( (tmp->m_start >= startPos) && ( ! tmp->m_use ) && ( tmp->m_len >= len ) )
			{
				u32 x = tmp->m_start % align;

				if( x == 0 )
					break;

				if( (tmp->m_len - len) < x )
					continue;


				//分掉前面的
				CLinkList* newtmp = new CLinkList;
// 				if( NULL == newtmp )
// 					return -1;

				newtmp->m_start = tmp->m_start;
				newtmp->m_len = align - x;
				newtmp->m_use = false;
				newtmp->next = tmp;

				pPrev->next = newtmp;

				tmp->m_start += newtmp->m_len;
				tmp->m_len -= newtmp->m_len;

				break;
			}

			pPrev = tmp;
		}while( tmp = pPrev->next );

		if( NULL == tmp )
			return -1;

		if( tmp->m_len > len )
		{//分掉后面的
			CLinkList* newtmp = new CLinkList;
// 			if( NULL == newtmp )
// 				return -1;

			newtmp->m_start = tmp->m_start + len;
			newtmp->m_len = tmp->m_len - len;
			newtmp->m_use = false;
			newtmp->next = tmp->next;

			tmp->next = newtmp;
			tmp->m_len = len;
		}
		tmp->m_use = true;

		return tmp->m_start;
	}
	inline bool Free( const s32 st )
	{
		if( st < 0 )
			return false;

		CLinkList* tmp0 = NULL;
		CLinkList* tmp1 = &m_head;

		do
		{
			if( st == tmp1->m_start )
			{
				if( tmp1->m_use )
					break;
				return true;
			}
			tmp0 = tmp1;

		}while( tmp1 = tmp1->next );

		if( NULL == tmp1 )
			return false;

		tmp1->m_use = false;
		//检测前面合并
		if( tmp0 )
		{
			if( !tmp0->m_use )
			{//存在空闲
				tmp0->m_len += tmp1->m_len;
				tmp0->next = tmp1->next;
				delete tmp1;
				tmp1 = tmp0;
			}
		}
		//检测后续合并
		if( tmp0 = tmp1->next )
		{
			if( !tmp0->m_use )
			{
				tmp1->m_len += tmp0->m_len;
				tmp1->next = tmp0->next;
				delete tmp0;
			}
		}
		return true;
	}

	inline u32 GetMaxLbaCount()const
	{
		return m_maxLBA;
	}

	inline void GetFreeInfo( u32* a_puMaxFreeBlock , u32* a_puFreeLbaCount , u32* a_puFreeBlockCount )const
	{
		if( !a_puMaxFreeBlock || !a_puFreeLbaCount )
			return;

		u32 uMax = 0;
		u32 uFreeLbaCount = 0;
		u32 uFreeBlockCount = 0;

		const CLinkList* pNode = &m_head;
		while( pNode )
		{
			if( pNode->m_use )
			{

			}
			else
			{
				if( pNode->m_len > uMax )
				{
					uMax = pNode->m_len;
				}

				uFreeLbaCount += pNode->m_len;
				uFreeBlockCount++;
			}
			pNode = pNode->next;
		}

		if( a_puMaxFreeBlock )
			*a_puMaxFreeBlock = uMax;

		if( a_puFreeLbaCount )
			*a_puFreeLbaCount = uFreeLbaCount;

		if( a_puFreeBlockCount )
			*a_puFreeBlockCount = uFreeBlockCount;
	}

	inline bool GetBlockInfo( s32 a_nSt , u32* a_puLen , bool* a_pbUse )const
	{
		if( a_nSt < 0 )
			return false;

		if( !a_puLen || !a_pbUse )
			return true;

		const u32 uSt = a_nSt;

		u32 uMax = 0;
		u32 uFreeLbaCount = 0;

		const CLinkList* pNode = &m_head;
		while( pNode )
		{
			if( pNode->m_start > uSt )
				return false;
			else if( pNode->m_start == uSt )
			{
				if( a_puLen )
					*a_puLen = pNode->m_len;

				if( a_pbUse )
					*a_pbUse = pNode->m_use;

				return true;
			}

			pNode = pNode->next;
		}
		return false;
	}
#ifdef DEBUG
	void 写出调试信息( CStringW& log )
	{
		log = L"";
		CLinkList* tmp = &m_head;
		while( tmp )
		{
			CString str;
			str.Format( L"\"%08d\"\11\11\"%d\"\11\11\"%d\"\r\n" , tmp->m_start , tmp->m_len , tmp->m_use );
			log += str;
			tmp = tmp->next;
		}
	}
#endif
};

template<typename TType , size_t TAlign>
inline BOOL WQSG_Bin2c_template( CWQSG_xFile& a_Out ,
						 const void* a_pBin , size_t a_size ,
						 const char* a_szTag ,
						 const char* a_szFmt , const char* a_szTypeName )
{
	if( NULL == a_pBin || a_size == 0 || (a_size % sizeof(TType)) != 0 )
		return FALSE;

	a_size /= sizeof(TType);
	const TType* pType = (const TType*)a_pBin;


	a_Out.Write( "static const " , (u32)strlen("static const ") );
	a_Out.Write( a_szTypeName , (u32)strlen(a_szTypeName) );
	a_Out.Write( " " , 1 );
	a_Out.Write( a_szTag , (u32)strlen(a_szTag) );
	a_Out.Write( "[] = {\r\n" , (u32)strlen("[] = {\r\n") );

	char szBuf[32];

	size_t count = 0;
	while( a_size-- )
	{
		if( count++ > TAlign )
		{
			count = 0;

			a_Out.Write( "\r\n" , 2 );
		}

		sprintf_s( szBuf , sizeof(szBuf) , a_szFmt , *pType );
		pType++;

		const size_t len = strlen(szBuf);
		a_Out.Write( szBuf , (u32)len );

		if( a_size > 0 )
			a_Out.Write( " , " , 3 );
	}

	a_Out.Write( "\r\n};\r\n" , (u32)strlen("\r\n};\r\n") );

	return TRUE;
}

inline BOOL WQSG_Bin2c_8Bit( CWQSG_xFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u8 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%02X" , "u8" );
}

inline BOOL WQSG_Bin2c_16Bit( CWQSG_xFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u16 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%04X" , "u16" );
}

inline BOOL WQSG_Bin2c_32Bit( CWQSG_xFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u32 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%08X" , "u32" );
}



#endif
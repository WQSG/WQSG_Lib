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

#include "WQSG_def.h"
#include "wqsg_ifile.h"

template <typename TYPE_1>
class CWQSG_TypeLinkList
{
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
	}
	virtual	~CWQSG_TypeLinkList()
	{
		DelAll();
	}
	//-------------------------------------------------------------------------------------
	inline	void		DelAll()
    {
        m_PosPtr = m_head;
        while(m_PosPtr)
        {
            m_head = m_PosPtr->next;
            delete m_PosPtr;
            m_PosPtr = m_head;
        }
        m_Count = 0;
        m_Pos = -1;
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

        return ++m_Count,++m_Pos;
__gtErr:
		return -1;
    }
	inline	bool		GetItem( const int index , TYPE_1& type )
    {
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
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

        return true;
    }
	inline	bool		SetItem( const int index , const TYPE_1& type )
    {
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
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

        return true;
    }
	inline	bool		DelItem( const int index )
    {
        if( (unsigned int)index >= (unsigned int)m_Count )
		{
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

        return true;
    }
	inline	int			GetCount( void ){   return m_Count; }
	inline	int			GetPos( void ){ return m_Pos;   }
	inline	TYPE_1*		MakeArray( void )
	{
		TYPE_1* line = new TYPE_1[ m_Count ];
		if( NULL == line )
		{
			return NULL;
		}

		tgNode* tmpNode = m_head;

		for( int i = 0 ; i < m_Count ; ++i )
		{
			line[i] = tmpNode->Type;
			tmpNode = tmpNode->next;
		}
		return line;
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
			if( (tmp->m_start >= (u32)startPos) && ( ! tmp->m_use ) && ( tmp->m_len >= len ) )
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
#if defined(DEBUG) && (defined(__AFXDLGS_H__) || defined(__ATLSTR_H__))
	void DebugInfo( CStringW& log )
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
//------------------------------------------------------------------
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

inline BOOL WQSG_Bin2c_8Bit( CWQSG_IFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u8 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%02X" , "u8" );
}

inline BOOL WQSG_Bin2c_16Bit( CWQSG_IFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u16 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%04X" , "u16" );
}

inline BOOL WQSG_Bin2c_32Bit( CWQSG_IFile& a_Out , const void* a_pBin , size_t a_size , const char* a_szTag )
{
	return WQSG_Bin2c_template<u32 , 16>( a_Out , a_pBin , a_size , a_szTag , "0x%08X" , "u32" );
}
//------------------------------------------------------------------
class CWQSG_StringMgr
{
	const WCHAR*const* m_szDefaultString;
	const size_t m_uDefaultString;

	const WCHAR*const* m_szUserString;
	size_t m_uUserString;
public:
	inline CWQSG_StringMgr( const WCHAR*const* a_szDefaultString , const size_t a_uDefaultString )
		: m_szDefaultString(a_szDefaultString)
		, m_uDefaultString(a_uDefaultString)
		, m_szUserString(NULL)
		, m_uUserString(0)
	{
	}

	inline const WCHAR* GetString( size_t a_uIndex )const
	{
		if( m_szUserString && a_uIndex < m_uUserString && m_szUserString[a_uIndex] )
			return m_szUserString[a_uIndex];
		else if( m_szDefaultString && a_uIndex < m_uDefaultString && m_szDefaultString[a_uIndex] )
			return m_szDefaultString[a_uIndex];

		return L"<NoString>";
	}

	inline size_t GetStringCount()const
	{
		return m_uDefaultString;
	}

	inline void SetString( const WCHAR*const* a_szUserString , const size_t a_uUserString )
	{
		m_szUserString = a_szUserString;
		m_uUserString = a_uUserString;
	}
};
//===========================================================
template<typename TType>
class CValReverse
{
	TType m_Val;
public:
	inline CValReverse( const TType& a_Val )
	{
		u8* pDst = (u8*)&m_Val;
		const u8* pSrc = (const u8*)&a_Val;

		for ( size_t n = 0 ; n < sizeof(TType) ; ++n )
			pDst[sizeof(TType)-n-1] = pSrc[n];
	}

	inline const TType& GetVal()const
	{
		return m_Val;
	}
};

template<typename TType>
class CVal
{
	TType m_Val;
public:
	inline CVal( const TType& a_Val )
		: m_Val(a_Val)
	{
	}

	inline const TType& GetVal()const
	{
		return m_Val;
	}
};
template<typename TType>
inline CValReverse<TType> _ValReverse_Helper( const TType& a_Val )
{
	return CValReverse<TType>( a_Val );
}

template<typename TType>
inline CVal<TType> _H2H_Helper( const TType& a_Val )
{
	return CVal<TType>( a_Val );
}

#define H2H( _v ) _H2H_Helper(_v).GetVal()
#if WQSG_BIG_ENDIAN
#define H2L( _v ) _ValReverse_Helper(_v).GetVal()
#define H2B( _v ) H2H(_v)
#define L2H( _v ) _ValReverse_Helper(_v).GetVal()
#define B2H( _v ) H2H(_v)
#else //WQSG_BIG_ENDIAN
#define H2L( _v ) H2H(_v)
#define H2B( _v ) _ValReverse_Helper(_v).GetVal()
#define L2H( _v ) H2H(_v)
#define B2H( _v ) _ValReverse_Helper(_v).GetVal()
#endif //WQSG_BIG_ENDIAN
#define L2B( _v ) _ValReverse_Helper(_v).GetVal()
#define B2H( _v ) _ValReverse_Helper(_v).GetVal()

#endif

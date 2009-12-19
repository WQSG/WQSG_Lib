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

#ifndef __MapTbl_Base_H__
#define __MapTbl_Base_H__

#define WQSG_TXT_LOGFILE L"log.log"

class CWQSG_MapTbl_Base
{
public:
	inline static void zzz_Log( const WCHAR*const a_szText , const WCHAR*const a_szTitle = NULL )
	{
		if( NULL == a_szText )
			return;

		CWQSG_File	fp;
		if( fp.OpenFile( WQSG_TXT_LOGFILE , 9 ) )
		{
			if( fp.GetFileSize() == 0 )
				fp.Write( "\xFF\xFE" , 2 );

			SYSTEMTIME time;
			GetLocalTime( &time );
			WCHAR szTime[128];
			::swprintf( szTime , L"\r\n%04d-%02d-%02d %02d:%02d:%02d" , time.wYear , time.wMonth , time.wDay , time.wHour , time.wMinute , time.wMinute );

			fp.WriteStrW( szTime );

			if( a_szTitle )
				fp.WriteStrW( a_szTitle );

			fp.WriteStrW( L"\r\n" );

			fp.WriteStrW( a_szText );
		}
	}
};
//----------------------------------------------------------------------------------------------------
#undef WQSG_TXT_LOGFILE

template<typename T1>
class CByteTree
{
	struct SBtNode
	{
		T1* m_pData;
		size_t m_NextLayerIndex;
	};

	struct SBtLayer
	{
		SBtNode node[256];
	};

	std::vector<SBtLayer> m_Layers;

	size_t m_depth;
public:
	inline CByteTree()
		: m_depth(0)
	{
		SBtLayer layer;
		memset( &layer , 0 , sizeof(layer) );
		m_Layers.push_back(layer);
	}

	inline void add( const u8* a_pKey , size_t a_KeyLen , T1* a_pData );
	inline T1* get( const u8* a_pKey , size_t a_KeyLen )const;
	inline T1* find( size_t& a_return , const u8* a_pKey , size_t a_KeyLen )const;

	inline void clear();

	inline size_t GetDepth()const
	{
		return m_depth;
	}
};
//----------------------------------------------------------------------------------------------------
template<typename T1>
inline void CByteTree<T1>::add( const u8* a_pKey , size_t a_KeyLen , T1* a_pData )
{
	if ( !a_KeyLen )
		return;

	a_KeyLen--;

	size_t layerIndex = 0;
	size_t keyOffset;
	for( keyOffset = 0 ; keyOffset < a_KeyLen ; ++keyOffset )
	{
		const u8 key = a_pKey[keyOffset];

		SBtLayer& layer = m_Layers[layerIndex];

		size_t nextIndex = layer.node[key].m_NextLayerIndex;
		if( nextIndex )
		{
			layerIndex = nextIndex;
		}
		else
		{
			layerIndex = m_Layers.size();
			layer.node[key].m_NextLayerIndex = layerIndex;

			SBtLayer layer;
			memset( &layer , 0 , sizeof(layer) );

			m_Layers.push_back( layer );
		}
	}

	const u8 key = a_pKey[keyOffset];
	m_Layers[layerIndex].node[key].m_pData = a_pData;

	keyOffset++;
	if( keyOffset > m_depth )
		m_depth = keyOffset;
}
//----------------------------------------------------------------------------------------------------
template<typename T1>
inline T1* CByteTree<T1>::get( const u8* a_pKey , size_t a_KeyLen )const
{
	size_t retuenLen;
	T1* pData = find( retuenLen , a_pKey , a_KeyLen );
	return (pData && retuenLen == a_KeyLen)?pData:NULL;
}
//----------------------------------------------------------------------------------------------------
template<typename T1>
inline T1* CByteTree<T1>::find( size_t& a_return , const u8* a_pKey , size_t a_KeyLen )const
{
	T1* pData = NULL;
	if( a_KeyLen )
	{
		size_t layerIndex = 0;
		size_t keyOffset;
		for( keyOffset = 0 ; keyOffset < a_KeyLen ; ++keyOffset )
		{
			const u8 key = a_pKey[keyOffset];

			const SBtLayer& layer = m_Layers[layerIndex];

			const SBtNode& node = layer.node[key];
			size_t nextIndex = node.m_NextLayerIndex;

			if( node.m_pData )
			{
				pData = node.m_pData;
				a_return = keyOffset + 1;
			}

			layerIndex = node.m_NextLayerIndex;
			if( !layerIndex )
				break;
		}
	}

	return pData;
}
//----------------------------------------------------------------------------------------------------
template<typename T1>
inline void CByteTree<T1>::clear()
{
	m_Layers.clear();
	m_depth = 0;

	SBtLayer layer;
	memset( &layer , 0 , sizeof(layer) );
	m_Layers.push_back(layer);
}

#endif

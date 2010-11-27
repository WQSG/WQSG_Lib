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

#include "WQSG_XML.h"

class CWQSG_INI_XML
{
	CWQSG_XML	m_xml;

	__Class_XML::IXMLDOMNodeListPtr		m_pConfigNodeList;

	__Class_XML::IXMLDOMNodePtr			m_pConfigNode;

	//--------------------------------------------------

	__Class_XML::IXMLDOMNodePtr			zzz_FindIDNode( CString ConfigName )
	{
		if( ConfigName.GetLength( ) > 0 )
		{
			try
			{
				__Class_XML::IXMLDOMNodePtr node;

				long rev = 0;

				while( ( rev = XML_寻找指定类型节点( m_xml.Get节点列表( L"" ) , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
				{
					++rev;

					if( (TCHAR*)node->nodeName == CString( L"Config" ) )
					{
						__Class_XML::IXMLDOMElementPtr pElement( node );

						_variant_t 属性值_t( pElement->getAttribute( L"ConfigName" ) );

						if( 属性值_t == ConfigName )
							return node;
					}
				}
			}
			catch(...){}
		}

		return NULL;
	}

	__Class_XML::IXMLDOMNodePtr			zzz_FindAppNode( CString appName )
	{
		if( appName.GetLength( ) > 0 )
		{
			try
			{
				__Class_XML::IXMLDOMNodePtr node;

				long rev = 0;
				while( ( rev = XML_寻找指定类型节点( m_pConfigNodeList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
				{
					++rev;
					if( (TCHAR*)node->nodeName == appName )
						return node;
				}
			}
			catch(...){}
		}

		return NULL;
	}

public:

	CWQSG_INI_XML( void ){}

	virtual	~CWQSG_INI_XML(void){}

	BOOL	Load( CString XmlFilePathName , CString rootNodeName , BOOL 文件必须存在 )
	{
		m_pConfigNodeList = NULL;
		m_pConfigNode = NULL;
		m_xml.Close();

		if( ::WQSG_IsFile( XmlFilePathName.GetBuffer() ) )
		{
			if( (!m_xml.Load( XmlFilePathName )) || ( m_xml.Get根节点名() != rootNodeName ) )
				return FALSE;
		}
		else
		{
			if( 文件必须存在 || (!m_xml.Create( rootNodeName )) )
				return FALSE;
		}

		return TRUE;
	}

	BOOL	Save( CString XmlFilePathName )	{	return m_xml.Save( XmlFilePathName );	}
	//------------------------------------------------------------------------------------
	LONG	GetConfigCount()
	{
		__Class_XML::IXMLDOMNodeListPtr pList( m_xml.Get节点列表( L"" ) );

		long rev = 0;
		LONG count = 0;

		try
		{
			__Class_XML::IXMLDOMNodePtr node;

			while( ( rev = XML_寻找指定类型节点( pList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
			{
				++rev;
				if( (TCHAR*)node->nodeName == CString( L"Config" ) )
					++count;
			}
		}
		catch(...){}

		return count;
	}

	BOOL	GetConfigName( const LONG index , CString& name )
	{
		if( index >= 0 )
		{
			try
			{
				__Class_XML::IXMLDOMNodePtr node;

				long rev = 0;
				LONG ii = 0;

				while( ( rev = XML_寻找指定类型节点( m_xml.Get节点列表( L"" ) , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
				{
					++rev;

					if( ( (TCHAR*)node->nodeName == CString( L"Config" ) ) && ( ii++ == index ) )
					{
						__Class_XML::IXMLDOMElementPtr pElement( node );

						_variant_t 属性值_t( pElement->getAttribute( L"ConfigName" ) );

						name = 属性值_t;

						return TRUE;
					}
				}
			}
			catch(...){}
		}
		return FALSE;
	}

	LONG	FindConfig( CString name )
	{
		if( name.GetLength() > 0 )
		{
			try
			{
				__Class_XML::IXMLDOMNodePtr node;

				long rev = 0;
				LONG ii = 0;

				__Class_XML::IXMLDOMNodeListPtr pList( m_xml.Get节点列表( L"" ) );

				while( ( rev = XML_寻找指定类型节点( pList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
				{
					++rev;

					if( ( (TCHAR*)node->nodeName == CString( L"Config" ) ) )
					{
						++ii;

						__Class_XML::IXMLDOMElementPtr pElement( node );

						_variant_t 属性值_t( pElement->getAttribute( L"ConfigName" ) );

						if( name == 属性值_t )
							return ii;

					}
				}
			}
			catch(...){}
		}

		return -1L;
	}

	BOOL	DelConfig( CString name )
	{
		if( name.GetLength() > 0 )
		{
			try
			{
				__Class_XML::IXMLDOMNodePtr node;

				long rev = 0;
				LONG ii = 0;

				__Class_XML::IXMLDOMNodeListPtr pList( m_xml.Get节点列表( L"" ) );

				while( ( rev = XML_寻找指定类型节点( pList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
				{
					++rev;

					if( ( (TCHAR*)node->nodeName == CString( L"Config" ) ) )
					{

						++ii;

						__Class_XML::IXMLDOMElementPtr pElement( node );

						_variant_t 属性值_t( pElement->getAttribute( L"ConfigName" ) );


						if( name == 属性值_t )
						{
							if( m_pConfigNode == node )
							{
								m_pConfigNodeList = NULL;
								m_pConfigNode = NULL;
							}

							if( m_xml.Get节点( L"" )->removeChild( node ) )
								return TRUE;

							return FALSE;
						}
					}
				}
			}
			catch(...){}
		}
		return FALSE;
	}
	//------------------------------------------------------------------------------------
	BOOL	SelConfigName( CString IDName , BOOL 不存在新建 = TRUE )
	{
		__Class_XML::IXMLDOMNodePtr pNode( zzz_FindIDNode( IDName ) );

		try
		{
			if( !pNode )
			{
				if( !不存在新建 )
					return FALSE;

				__Class_XML::IXMLDOMElementPtr pElemen ( m_xml.m_DocPtr->createElement ( L"Config" ) );
				_bstr_t keyName_t2( L"ConfigName" );
				_variant_t val_t( IDName );

				if FAILED( pElemen->setAttribute( keyName_t2 , val_t ) )
					return FALSE;

				pNode = m_xml.Get节点( L"" )->appendChild( pElemen );

				if( !pNode )
					return FALSE;
			}

			m_pConfigNode = pNode;

			m_pConfigNodeList = pNode->childNodes;

			return TRUE;
		}
		catch(...){}

		return FALSE;
	}

	LONG	GetItemCount()
	{
		__Class_XML::IXMLDOMNodePtr node;

		long rev = 0;
		LONG count = 0;

		while( ( rev = XML_寻找指定类型节点( m_pConfigNodeList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
		{
			++rev;
			++count;
		}

		return count;
	}

	BOOL	GetAppName( LONG index , CString& AppName )
	{
		__Class_XML::IXMLDOMNodePtr node;

		long rev = 0;
		LONG count = 0;

		try
		{
			while( ( rev = XML_寻找指定类型节点( m_pConfigNodeList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
			{
				++rev;

				if( count++ == index )
				{
					AppName = (LPWSTR)node->nodeName;
					return TRUE;
				}
			}
		}
		catch(...){}

		return FALSE;
	}

	BOOL	修改AppName( CString oldAppNmae , CString newAppNmae )
	{
		__Class_XML::IXMLDOMNodePtr node;

		long rev = 0;
		LONG count = 0;

		try
		{
			while( ( rev = XML_寻找指定类型节点( m_pConfigNodeList , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 )
			{
				++rev;

				if( (WCHAR*)node->nodeName == oldAppNmae )
				{
					_bstr_t 节点名_t( newAppNmae );

					__Class_XML::IXMLDOMElementPtr pElemenNew ( m_xml.m_DocPtr->createElement ( 节点名_t ) );

					return( XML_复制属性( pElemenNew , node ) &&

						m_pConfigNode->replaceChild( pElemenNew , node ) );
				}
			}
		}
		catch(...){}

		return FALSE;
	}
	///------------------
	BOOL	SetApp( CString appName , CString keyName , CString val )
	{
		__Class_XML::IXMLDOMElementPtr pElement( zzz_FindAppNode( appName ) );

		try
		{
			_bstr_t keyName_t2( keyName );

			_variant_t val_t( val );

			if( !pElement )
			{
				_bstr_t 节点名_t( appName );

				__Class_XML::IXMLDOMElementPtr pElemenNew ( m_xml.m_DocPtr->createElement ( 节点名_t ) );

				pElement = m_pConfigNode->appendChild( pElemenNew );
			}

			return SUCCEEDED( pElement->setAttribute( keyName_t2 , val_t ) );
		}
		catch(...){}

		return FALSE;
	}

	BOOL	GetApp( CString appName , CString keyName , CString& val , CString  默认 )
	{

		try
		{
			__Class_XML::IXMLDOMElementPtr pElement( zzz_FindAppNode( appName ) );

			_bstr_t keyName_t2( keyName );

			if( pElement )
			{
				_variant_t val_t ( pElement->getAttribute( keyName_t2 ) );

				val = val_t;

				return TRUE;
			}
		}
		catch(...){}

		val = 默认;

		return FALSE;
	}

	BOOL	DelApp( CString appName )
	{
		__Class_XML::IXMLDOMElementPtr pElement( zzz_FindAppNode( appName ) );

		try
		{
			__Class_XML::IXMLDOMNode* _result = 0;

			return SUCCEEDED( m_pConfigNode->raw_removeChild( pElement , &_result ) );
		}
		catch(...){}

		return FALSE;
	}

};


class CWQSG_INI_Node
{
	__Class_XML::IXMLDOMNodePtr	m_node;

	BOOL			zzz_GetAttribute( __Class_XML::IXMLDOMElementPtr pElement , CString strAttrName , CString& strOutVal )
	{
		try
		{
//			__Class_XML::IXMLDOMElementPtr pElement( _node );

			strOutVal = ( pElement->getAttribute( strAttrName.GetBuffer() ) );

			return TRUE;
		}
		catch(...){}

		return FALSE;
	}

public:

	CWQSG_INI_Node():m_node(NULL){	}

	CWQSG_INI_Node( const CWQSG_INI_Node& node ) : m_node(node.m_node){	}

	CWQSG_INI_Node& operator= ( const CWQSG_INI_Node& node )
	{
		m_node = node.m_node;

		return *this;
    }
	///------------------------------------------------------------------------
	CWQSG_INI_Node( const __Class_XML::IXMLDOMNodePtr& node )
	{
		CString str;

		if( ( (TCHAR*)node->nodeName == CString( _T("Node") ) ) &&
			( zzz_GetAttribute( node , _T("NodeName") , str ) ) )
			m_node = node;
	}
	//----------------------------------------------------------------------------
	BOOL			GetAttribute( CString strAttrName , CString& strOutVal )
	{
		return zzz_GetAttribute( m_node , strAttrName , strOutVal );
	}

	long			GetAttrCount()
	{
		long count = 0;

		try
		{
			__Class_XML::IXMLDOMNodePtr node;

			__Class_XML::IXMLDOMNodeListPtr list( m_node->childNodes );

			for( long rev = 0 ; ( rev = XML_寻找指定类型节点( list , node , DOMNodeType::NODE_ATTRIBUTE , rev ) ) >= 0 ; ++rev )
			{
				++count;
			}
		}
		catch(...){}

		return (count>=1)?(count- 1):0;
	}

	BOOL			GetAttrName( const long index , CString& strOutName )
	{
		try
		{
			long count = 0;

			__Class_XML::IXMLDOMNodePtr node;

			__Class_XML::IXMLDOMNodeListPtr list( m_node->childNodes );

			for( long rev = 0 ; ( rev = XML_寻找指定类型节点( list , node , DOMNodeType::NODE_ATTRIBUTE , rev ) ) >= 0 ; ++rev )
			{
				if( index == count )
				{
					strOutName = (TCHAR*)node->nodeName;

					return TRUE;
				}
			}
		}
		catch(...){}

		return FALSE;
	}
	///---------------------------
	BOOL			SetAttribute( CString strAttrName , CString strInVal )
	{
		try
		{
			__Class_XML::IXMLDOMElementPtr pElement( m_node );

			if( SUCCEEDED( pElement->setAttribute( strAttrName.GetBuffer() , _variant_t( strInVal.GetBuffer() ) ) ) )
				return TRUE;
		}
		catch(...){}

		return FALSE;
	}
};

class CWQSG_INI_XML2
{
	CWQSG_XML			m_xml;
	//--------------------------------------------------
	__Class_XML::IXMLDOMNodePtr	zzz_FindNode( __Class_XML::IXMLDOMNodePtr baseNode , CString strNodeName )
	{
		try
		{
			__Class_XML::IXMLDOMNodePtr node;

			__Class_XML::IXMLDOMNodeListPtr list( baseNode->childNodes );

			for( long rev = 0 ; ( rev = XML_寻找指定类型节点( list , node , DOMNodeType::NODE_ELEMENT , rev ) ) >= 0 ; ++rev )
			{
				if( (TCHAR*)node->nodeName == CString( _T("Node") ) )
				{
					__Class_XML::IXMLDOMElementPtr pElement( node );

					_variant_t 属性值_t( pElement->getAttribute( _T("NodeName") ) );

					if( 属性值_t == strNodeName )
						return node;
				}
			}
		}
		catch(...){}

		return NULL;
	}

	void		zzz_Path( CString& path )
	{
		if( (path.Right(1) != _T('\\')) && (path.Right(1) != _T('/')) )
			path += _T('\\');

		path.TrimLeft( _T('\\') );
		path.TrimLeft( _T('/') );

		path.Replace( _T('\\') , _T('\0') );
		path.Replace( _T('/') , _T('\0') );
	}

	BOOL		zzz_GetNode( CString path , __Class_XML::IXMLDOMNodePtr& p父Node , __Class_XML::IXMLDOMNodePtr& pNode )
	{
		zzz_Path( path );

		__Class_XML::IXMLDOMNodePtr node( m_xml.Get节点( L"" ) );

		WCHAR const* pPath = path.GetBuffer();

		while( *pPath )
		{
			if( node == NULL )
				return FALSE;

			CString name( pPath );

			pPath += (name.GetLength() + 1);

			name.TrimLeft();
			name.TrimRight();
			//------------------------------------------
			node = zzz_FindNode( p父Node = node , name );

		}

		if( node == NULL )
			return FALSE;

		pNode = node;

		return TRUE;
	}

public:

	CWQSG_INI_XML2( void ){}

	virtual	~CWQSG_INI_XML2(void){}

	BOOL	Load( CString XmlFilePathName , CString rootNodeName , BOOL 文件必须存在 )
	{
		m_xml.Close();

		if( ::WQSG_IsFile( XmlFilePathName.GetBuffer() ) )
		{
			if( (!m_xml.Load( XmlFilePathName )) || ( m_xml.Get根节点名() != rootNodeName ) )
				return FALSE;
		}
		else
		{
			if( 文件必须存在 || (!m_xml.Create( rootNodeName )) )
				return FALSE;
		}

		return TRUE;
	}

	BOOL	Save( CString XmlFilePathName )	{	return m_xml.Save( XmlFilePathName );	}
	//------------------------------------------------------------------------------------
	BOOL		GetNode( CString path , CWQSG_INI_Node*const pNode )
	{
		__Class_XML::IXMLDOMNodePtr node0;

		__Class_XML::IXMLDOMNodePtr node;

		if( !zzz_GetNode( path , node0 , node ) )
			return FALSE;

		if( pNode != NULL )
			*pNode = CWQSG_INI_Node( node );

		return TRUE;
	}

	BOOL		AddNode( CString path )
	{
		zzz_Path( path );

		__Class_XML::IXMLDOMNodePtr p父Node;
		__Class_XML::IXMLDOMNodePtr node( m_xml.Get节点( L"" ) );

		WCHAR const* pPath = path.GetBuffer();

		while( *pPath )
		{
			if( node == NULL )
				return FALSE;

			CString name( pPath );

			pPath += (name.GetLength() + 1);

			name.TrimLeft();
			name.TrimRight();
			//-------------------------------------
			node = zzz_FindNode( p父Node = node , name );

			if( node == NULL )
			{
				try
				{
					__Class_XML::IXMLDOMDocumentPtr docPtr( m_xml.GetDoc() );
					__Class_XML::IXMLDOMElementPtr pElement( docPtr->createElement( _T("Node") ) );


					if( FAILED( pElement->setAttribute( _T("NodeName") , _variant_t( name.GetBuffer() ) ) ) )
						return FALSE;

					node = p父Node->appendChild( pElement );
				}
				catch(...){return FALSE;}
			}
		}

		return TRUE;
	}

	BOOL		DelNode( CString path )
	{
		__Class_XML::IXMLDOMNodePtr 父node;

		__Class_XML::IXMLDOMNodePtr node;

		if( !zzz_GetNode( path , 父node , node ) )
			return FALSE;

		父node->removeChild( node );

		return TRUE;
	}
};

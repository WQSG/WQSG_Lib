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
#include "StdAfx.h"
#include "WQSG_XML.h"

#ifdef	_DEBUG

#define	__OUT_DEGTEXT(x)	printf("%s\n",x)

#define XML_CATCH_DEF  catch(_com_error &e){\
	OUT_ERR(e);}\
	catch(...){\
	_tprintf(_T("Unknown error!"));}

#else

#define	__OUT_DEGTEXT(x)

#define XML_CATCH_DEF catch(...){_tprintf(_T("Unknown error!"));}

#endif

#define __引用检测__ 0

#pragma warning(push)
#pragma warning(disable: 4482)

#define WQSG_XML_标记头 (L"version=\"1.0\" encoding=\"utf-8\"")

static inline void TESTHR( HRESULT _hr )
{
	if FAILED(_hr)
		throw(_hr);
}

//---------------------------------------------------------------
inline long XML_寻找指定类型节点( __Class_XML::IXMLDOMNodeListPtr pNodeList , __Class_XML::IXMLDOMNodePtr& pNode0 , DOMNodeType NodeType , long StartIndex )
{
	pNode0 = NULL;

	if( StartIndex >= 0 )
	{
		try
		{
			__Class_XML::IXMLDOMNodePtr pNode;

			for( ; ( pNode = pNodeList->Getitem( StartIndex ) ); ++StartIndex )
			{
				if( NodeType == pNode->nodeType )
				{
					pNode0 = pNode;
					return StartIndex;
				}
			}
		}
		XML_CATCH_DEF
	}
	return -1L;
}
//---------------------------------------------------------------
inline long XML_寻找指定类型节点( __Class_XML::IXMLDOMNodePtr pNode , __Class_XML::IXMLDOMNodePtr& pNode0 , DOMNodeType NodeType , long StartIndex )
{
	try
	{
		return ( XML_寻找指定类型节点( pNode->GetchildNodes() , pNode0 , NodeType , StartIndex ) );
	}
	XML_CATCH_DEF
		pNode0 = NULL;
	return -1L;
}
//---------------------------------------------------------------
static CStringW 取路径段( WCHAR** path )
{
	WCHAR * sp = *path;
	CStringW 段名;
	while( (*sp) )
	{
		if( *sp == L'\\') *sp = L'/';
		++sp;
	}
	sp = *path;
	while( (*sp) && (*sp == L'/') ){	++sp;	}
	while( (*sp) && (*sp != L'/') ){	段名 += *(sp++);	}
	while( (*sp) && (*sp == L'/') ){	++sp;	}

	*path = sp;

	return 段名;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_寻找节点( __Class_XML::IXMLDOMNodePtr pNode , CStringW 节点名 )

{
	__Class_XML::IXMLDOMNodePtr rev_pNode;

	try
	{
		_bstr_t _节点名_t( 节点名 );

		__Class_XML::IXMLDOMNodeListPtr pNodeList( pNode->GetchildNodes() );
#if __引用检测__
		if(!pNodeList)return rev_pNode;
#endif
		long rev = 0L;
		long cnt = pNodeList->Getlength();

		while( rev < cnt )
		{
			rev = XML_寻找指定类型节点( pNodeList , rev_pNode , DOMNodeType::NODE_ELEMENT , rev );

			if( ( rev < 0) ||
				( rev_pNode->nodeName == _节点名_t ) )
				break;

			++rev;
		}

		if( rev == cnt )
			rev_pNode = NULL;
	}
	XML_CATCH_DEF

		return rev_pNode;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_寻找属性( __Class_XML::IXMLDOMNodePtr pNode , CStringW 属性名 )
{
	__Class_XML::IXMLDOMNodePtr rev_pNode;

	try
	{
		long rev = 0L;

		_bstr_t _属性名_t(属性名);

		__Class_XML::IXMLDOMNamedNodeMapPtr pNamedNodeMap (pNode->Getattributes());
#if __引用检测__
		if(!pNamedNodeMap)return rev_pNode;
#endif
		long cnt = pNamedNodeMap->Getlength();

		while(rev < cnt)
		{
			rev_pNode = pNamedNodeMap->Getitem( rev );

#if __引用检测__
			if(!rev_pNode)break;
#endif
			if( rev_pNode->nodeName == _属性名_t ){	break;	}

			++rev;
		}

		if(rev == cnt)
			rev_pNode = NULL;
	}
	XML_CATCH_DEF

		return rev_pNode;
}
//---------------------------------------------------------------
bool XML_取节点值( __Class_XML::IXMLDOMNodePtr pNode , CString& 节点值 )
{
	节点值 = L"";

	try
	{
		__Class_XML::IXMLDOMNodeListPtr pNodeList( pNode->GetchildNodes() );

#if __引用检测__
		if(!pNodeList)return false;
#endif

		__Class_XML::IXMLDOMNodePtr rev_pNode;

		if( XML_寻找指定类型节点( pNodeList , rev_pNode , DOMNodeType::NODE_TEXT , 0) >= 0 )
		{
			节点值 = (WCHAR*)rev_pNode->text;
			return true;
		}
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool XML_取属性值( __Class_XML::IXMLDOMNodePtr pNode , CStringW 属性名 , CStringW& 属性值 )
{
	属性值 = L"";

	try
	{
		__Class_XML::IXMLDOMElementPtr pElement( pNode );

#if __引用检测__
		if(!pElement)return false;
#endif

		_bstr_t 属性值_t2( 属性名 );

		_variant_t 属性值_t( pElement->getAttribute( 属性值_t2 ) );

		属性值 = 属性值_t;
		return true;
	}
	XML_CATCH_DEF
		return false;
}
//---------------------------------------------------------------
static __Class_XML::IXMLDOMNodePtr 按路径名取节点( __Class_XML::IXMLDOMNodePtr pNode , CStringW 路径 )
{
	__Class_XML::IXMLDOMNodePtr rev_pNode ;
	try
	{
		WCHAR* path = 路径.GetBuffer();
		CStringW 名称;

		名称 = 取路径段( &path ) ;

		rev_pNode = pNode;

		while( 名称.GetLength() > 0 )
		{
			rev_pNode = XML_寻找节点( rev_pNode ,名称);

			if( !pNode) break;

			名称 = 取路径段( &path ) ;
		}
	}
	XML_CATCH_DEF
		return rev_pNode;
}
//---------------------------------------------------------------
bool XML_复制子节点( __Class_XML::IXMLDOMElementPtr p目标Element , __Class_XML::IXMLDOMNodePtr p源Node )
{
	__Class_XML::IXMLDOMNodePtr pNode;

	long cnt;

	try
	{
		__Class_XML::IXMLDOMNodeListPtr p源NodeList( p源Node->GetchildNodes() );

#if __引用检测__
		if(!p源NodeList)return false;
#endif

		cnt = p源NodeList->Getlength();

		long index;

		for(index = 0; pNode = p源NodeList->nextNode() ; ++index)
			//		for(index = 0; pNode = p源NodeList->Getitem( index ) ; ++index)
		{
			__OUT_DEGTEXT((char*)pNode->nodeName);

			//			cout << (char*)pNode->nodeTypeString << (int)pNode->nodeTypedValue  << endl;
			if( ! (bool)p目标Element->appendChild( pNode ) ) return false;
		}

		if( cnt != index ) return false;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
bool XML_复制属性( __Class_XML::IXMLDOMElementPtr p目标Element , __Class_XML::IXMLDOMNodePtr p源Node )
{
	__Class_XML::IXMLDOMAttributePtr pNode;

	long cnt;

	try
	{
		__Class_XML::IXMLDOMNamedNodeMapPtr p源NamedNodeMap( p源Node->Getattributes() );

#if __引用检测__
		if(!p源NamedNodeMap)return false;
#endif

		cnt = p源NamedNodeMap->Getlength();

		long index;

		for(index = 0;pNode = p源NamedNodeMap->nextNode() ; ++index)
		{
			//			if( !p目标Element->setAttributeNode( pNode ) )	return false;

			_variant_t val(pNode->text);

			TESTHR(p目标Element->setAttribute( pNode->nodeName , val ));
		}

		if( cnt != index) return false;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
CWQSG_XML::CWQSG_XML	(void)
:m_根节点名(_T(""))
{
}
//---------------------------------------------------------------
CWQSG_XML::~CWQSG_XML	(void)
{
	Close(false);
}
//---------------------------------------------------------------
void CWQSG_XML::Close	(bool reStart)
{
	try
	{
		m_BaseNode = m_RootNode;

		//	m_RootNode = NULL;

		if(m_DocPtr)		m_DocPtr.Release();
		if( m_RootNode )		m_RootNode.Release();

		m_根节点名 = _T("");

		m_BaseNode = NULL;

		if( reStart )
		{
			TESTHR( m_DocPtr.CreateInstance(__uuidof(__Class_XML::DOMDocument30 ) ) );

			m_DocPtr->put_async(VARIANT_FALSE);
			m_DocPtr->put_validateOnParse(VARIANT_FALSE);
			m_DocPtr->put_resolveExternals(VARIANT_FALSE);
			m_DocPtr->put_preserveWhiteSpace(VARIANT_TRUE);
		}
	}
	XML_CATCH_DEF
}
//---------------------------------------------------------------
bool CWQSG_XML::Create	( LPCWSTR 节点名 )
{
	if( !zzz_Init() )
	{
		__OUT_DEGTEXT("没有初始化");

		return false;
	}

	Close( );

	try
	{
		_bstr_t var节点(L"xml");
		_bstr_t varData(WQSG_XML_标记头);

		__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var节点,varData) );

		if( !ppp )
		{
			Close( );
			return false;
		}

		m_DocPtr->appendChild( ppp );

		var节点 = 节点名;

		m_RootNode = m_DocPtr->createElement( var节点 );

		m_DocPtr->appendChild( m_RootNode );

		m_BaseNode = m_RootNode;

		m_根节点名 = 节点名;

		return true;
	}
	XML_CATCH_DEF

		Close( );
	return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Load	( LPCWSTR lpPathName )
{
	if( !zzz_Init() )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	Close( );

	try
	{
		// Load a document.
		_variant_t varOut;

		_variant_t varXml(lpPathName);
		varOut = m_DocPtr->load(varXml);

		if ( !((bool)varOut) )
		{
			__Class_XML::IXMLDOMParseErrorPtr errPtr = m_DocPtr->GetparseError();
			__OUT_DEGTEXT((char*)errPtr->reason);
			return false;
		}

		__Class_XML::IXMLDOMNodeListPtr pNodeList( m_DocPtr->GetchildNodes() );
		if(!pNodeList)
		{
			Close( );
			__OUT_DEGTEXT("获取节点表失败");
			return false;
		}

		long 节点编号 = XML_寻找指定类型节点( pNodeList , m_RootNode ,DOMNodeType::NODE_PROCESSING_INSTRUCTION , 0L );

		if( 节点编号 < 0L )
		{//不存在标识头
			__OUT_DEGTEXT("不存在表示头");

			节点编号 = 0L;

			_bstr_t var节点(L"xml");
			_bstr_t varData(WQSG_XML_标记头);

			__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var节点,varData) );

			if( !ppp ){	Close( );	return false;	}

			m_DocPtr->appendChild( ppp );
		}

		节点编号 = XML_寻找指定类型节点( pNodeList , m_RootNode , DOMNodeType::NODE_ELEMENT , 节点编号 );

		if( 节点编号 < 0L )
		{//不存在根节点
			Close( );
			__OUT_DEGTEXT("没有根节点");
			return false;
		}

		m_根节点名 = (LPWSTR)m_RootNode->nodeName;

		m_BaseNode = m_RootNode;

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::LoadMem( _bstr_t memXml )
{
	if( !zzz_Init() )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	Close( );

	try
	{
		// Load a document.

		_variant_t varOut;
		varOut = m_DocPtr->loadXML( memXml );

		if ( !((bool)varOut) )
		{
			__Class_XML::IXMLDOMParseErrorPtr errPtr = m_DocPtr->GetparseError();
			__OUT_DEGTEXT((char*)errPtr->reason);
			return false;
		}

		__Class_XML::IXMLDOMNodeListPtr pNodeList( m_DocPtr->GetchildNodes() );
		if(!pNodeList)
		{
			Close( );
			__OUT_DEGTEXT("获取节点表失败");
			return false;
		}

		long 节点编号 = XML_寻找指定类型节点( pNodeList , m_RootNode ,DOMNodeType::NODE_PROCESSING_INSTRUCTION , 0L );

		if( 节点编号 < 0L )
		{//不存在标识头
			__OUT_DEGTEXT("不存在表示头");
			节点编号 = 0L;

			_bstr_t var节点(L"xml");
			_bstr_t varData(WQSG_XML_标记头);

			__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var节点,varData) );

			if( !ppp ){	Close( );	return false;	}

			m_DocPtr->appendChild( ppp );
		}

		节点编号 = XML_寻找指定类型节点( pNodeList , m_RootNode , DOMNodeType::NODE_ELEMENT , 节点编号 );

		if( 节点编号 < 0L )
		{//不存在根节点
			Close( );
			__OUT_DEGTEXT("没有根节点");
			return false;
		}

		m_根节点名 = (LPWSTR)m_RootNode->nodeName;

		m_BaseNode = m_RootNode;

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Save	( LPCWSTR lpPathName )
{
#if __引用检测__

	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}

#endif

	try
	{
		_variant_t varXml(lpPathName);

		TESTHR( m_DocPtr->save( varXml ) );
		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Is空( void )
{
	return( m_根节点名.GetLength() == 0 );
}
//---------------------------------------------------------------
bool CWQSG_XML::添加节点( CStringW 路径 , CStringW 节点名 )
{
#if __引用检测__

	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}

#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNode ( 按路径名取节点( m_BaseNode , 路径 ) );

#if __引用检测__
		if( !pNode )return false;
#endif

		if( (bool)XML_寻找节点(pNode ,节点名) ){return false;}

		_bstr_t 节点名_t(节点名);

		__Class_XML::IXMLDOMElementPtr pElemen ( m_DocPtr->createElement ( 节点名_t ) );

#if __引用检测__
		if( !pElemen ) return false;
#endif

		return ( (bool)pNode->appendChild ( pElemen ) );
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
bool CWQSG_XML::删除节点( CStringW 路径 , CStringW 节点名 )
{

#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode, 路径 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif
		__Class_XML::IXMLDOMNodePtr pNode ( XML_寻找节点( pNodeRoot ,节点名) );

#if __引用检测__
		if( !pNode ){	return false;	}
#endif

		_bstr_t 节点名_t(节点名);

		return((bool)pNodeRoot->removeChild( pNode ) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::修改节点名( CStringW 路径 , CStringW 节点名 , CStringW 新节点名 )
{

#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNodePtr pNode ( XML_寻找节点( pNodeRoot ,节点名 ) );

#if __引用检测__
		if( !pNode )return false;
#endif
		_bstr_t 新节点名_t(新节点名);

		__Class_XML::IXMLDOMElementPtr pElement( m_DocPtr->createElement( 新节点名_t ) );
#if __引用检测__
		if( !pElement )return false;
#endif

		if( ( !XML_复制属性( pElement , pNode ) ) ||
			( !XML_复制子节点( pElement , pNode ) ) )
			return false;

		return ((bool)pNodeRoot->replaceChild( pElement , pNode) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::修改节点值( CStringW 路径节点名 , CStringW 新节点值 )
{
#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNodePtr pNode;

		_bstr_t 新节点值_t(新节点值);

		if( XML_寻找指定类型节点( pNodeRoot ,pNode,DOMNodeType::NODE_TEXT,0) < 0)
		{//没找到
			pNode = m_DocPtr->createTextNode (新节点值_t);

#if __引用检测__
			if(!pNode)return false;
#endif
			return ((bool)pNodeRoot->appendChild ( pNode ) );
		}
		else
		{
			pNode->Puttext( 新节点值_t );
		}

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::修改节点值( CStringW 路径 , CStringW 节点名 , CStringW 新节点值 )
{
	return 修改节点值( 路径 + L'/' + 节点名 , 新节点值 );
}
//---------------------------------------------------------------
bool CWQSG_XML::添加属性( CStringW 路径节点名 , CStringW 属性名 , CStringW 属性值 )
{

#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );
#if __引用检测__
		if( !pNodeRoot )return false;
#endif
		_variant_t 属性值_t(属性值);

		__Class_XML::IXMLDOMNodePtr pNode ( XML_寻找属性( pNodeRoot ,属性名 ) );
		if(pNode)
		{
			//存在
			return false;

			//			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNode) );

			//		_bstr_t 属性值_t2(属性值);

			//		pNode->Puttext( 属性值_t2 );
		}
		else
		{
			//不存在
			_bstr_t 属性名_t(属性名);

			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNodeRoot) );

			TESTHR ( pElement->setAttribute( 属性名_t , 属性值_t ) );
		}
		return true;
	}
	XML_CATCH_DEF
		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::删除属性( CStringW 路径节点名 , CStringW 属性名 )
{

#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNamedNodeMapPtr pNamedNodeMap ( pNodeRoot->Getattributes() );

#if __引用检测__
		if( !pNamedNodeMap )return false;
#endif

		_bstr_t 属性名_t(属性名);

		return ((bool)pNamedNodeMap->removeNamedItem( 属性名_t ) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::修改属性名( CStringW 路径节点名 , CStringW 属性名 , CStringW 新属性名 )
{
#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif
	CStringW 属性值;

	return( 取属性值(路径节点名,属性名,属性值) &&
		删除属性(路径节点名,属性名) &&
		添加属性(路径节点名 ,新属性名 ,属性值 ) );
}
//---------------------------------------------------------------
bool CWQSG_XML::修改属性值( CStringW 路径节点名 , CStringW 属性名 , CStringW 新属性值 )
{
#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif

		_variant_t 属性值_t( 新属性值 );

		__Class_XML::IXMLDOMNodePtr pNode ( XML_寻找属性( pNodeRoot ,属性名 ) );

		if(pNode)
		{
			//存在
			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNode) );

			_bstr_t 属性值_t2(新属性值);
			pNode->Puttext( 属性值_t2 );
		}
		else
		{
			//不存在
			return false;
			//			_bstr_t 属性名_t(属性名);

			//			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNodeRoot) );

			//			TESTHR ( pElement->setAttribute( 属性名_t , 属性值_t ) );
		}
		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::取属性值( CStringW 路径节点名 , CStringW 属性名 , CString& 属性值 )
{
	属性值 = L"";

#if __引用检测__

	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif
		__Class_XML::IXMLDOMElementPtr pElement( pNodeRoot );

		_bstr_t 属性值_t2( 属性名 );

		_variant_t 属性值_t( pElement->getAttribute( 属性值_t2 ) );

		属性值 = 属性值_t;

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Get节点值( CStringW 路径节点名 , CString& 节点值 )
{
	节点值 = L"";

#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return false;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径节点名 ) );

#if __引用检测__
		if( !pNodeRoot )return false;
#endif

		return XML_取节点值( pNodeRoot ,节点值 );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodeListPtr CWQSG_XML::Get节点列表(CStringW 路径)
{
#if __引用检测__
	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return NULL;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return NULL;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( 按路径名取节点( m_BaseNode , 路径 ) );

#if __引用检测__
		if( !pNodeRoot )return NULL;
#endif
		return pNodeRoot->childNodes;
	}
	XML_CATCH_DEF

		return NULL;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr CWQSG_XML::Get节点(CStringW 路径)
{
#if __引用检测__

	if( !m_Init )
	{
		__OUT_DEGTEXT("没有初始化");
		return NULL;
	}

	if(Is空())
	{
		__OUT_DEGTEXT("空的XML");
		return NULL;
	}
#endif

	return 按路径名取节点( m_BaseNode , 路径 );
}
//---------------------------------------------------------------
bool CWQSG_XML::SetBaseNode( CString 路径节点名 )
{
	try
	{
		__Class_XML::IXMLDOMNodePtr newNode( 按路径名取节点( m_RootNode , 路径节点名 ) );

		if( !newNode )return false;

		m_BaseNode = newNode;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

#pragma warning(pop)

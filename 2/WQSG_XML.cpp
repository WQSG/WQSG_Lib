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

#define __���ü��__ 0

#pragma warning(push)
#pragma warning(disable: 4482)

#define WQSG_XML_���ͷ (L"version=\"1.0\" encoding=\"utf-8\"")

static inline void TESTHR( HRESULT _hr )
{
	if FAILED(_hr)
		throw(_hr);
}

//---------------------------------------------------------------
inline long XML_Ѱ��ָ�����ͽڵ�( __Class_XML::IXMLDOMNodeListPtr pNodeList , __Class_XML::IXMLDOMNodePtr& pNode0 , DOMNodeType NodeType , long StartIndex )
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
inline long XML_Ѱ��ָ�����ͽڵ�( __Class_XML::IXMLDOMNodePtr pNode , __Class_XML::IXMLDOMNodePtr& pNode0 , DOMNodeType NodeType , long StartIndex )
{
	try
	{
		return ( XML_Ѱ��ָ�����ͽڵ�( pNode->GetchildNodes() , pNode0 , NodeType , StartIndex ) );
	}
	XML_CATCH_DEF
		pNode0 = NULL;
	return -1L;
}
//---------------------------------------------------------------
static CStringW ȡ·����( WCHAR** path )
{
	WCHAR * sp = *path;
	CStringW ����;
	while( (*sp) )
	{
		if( *sp == L'\\') *sp = L'/';
		++sp;
	}
	sp = *path;
	while( (*sp) && (*sp == L'/') ){	++sp;	}
	while( (*sp) && (*sp != L'/') ){	���� += *(sp++);	}
	while( (*sp) && (*sp == L'/') ){	++sp;	}

	*path = sp;

	return ����;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_Ѱ�ҽڵ�( __Class_XML::IXMLDOMNodePtr pNode , CStringW �ڵ��� )

{
	__Class_XML::IXMLDOMNodePtr rev_pNode;

	try
	{
		_bstr_t _�ڵ���_t( �ڵ��� );

		__Class_XML::IXMLDOMNodeListPtr pNodeList( pNode->GetchildNodes() );
#if __���ü��__
		if(!pNodeList)return rev_pNode;
#endif
		long rev = 0L;
		long cnt = pNodeList->Getlength();

		while( rev < cnt )
		{
			rev = XML_Ѱ��ָ�����ͽڵ�( pNodeList , rev_pNode , DOMNodeType::NODE_ELEMENT , rev );

			if( ( rev < 0) ||
				( rev_pNode->nodeName == _�ڵ���_t ) )
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
__Class_XML::IXMLDOMNodePtr XML_Ѱ������( __Class_XML::IXMLDOMNodePtr pNode , CStringW ������ )
{
	__Class_XML::IXMLDOMNodePtr rev_pNode;

	try
	{
		long rev = 0L;

		_bstr_t _������_t(������);

		__Class_XML::IXMLDOMNamedNodeMapPtr pNamedNodeMap (pNode->Getattributes());
#if __���ü��__
		if(!pNamedNodeMap)return rev_pNode;
#endif
		long cnt = pNamedNodeMap->Getlength();

		while(rev < cnt)
		{
			rev_pNode = pNamedNodeMap->Getitem( rev );

#if __���ü��__
			if(!rev_pNode)break;
#endif
			if( rev_pNode->nodeName == _������_t ){	break;	}

			++rev;
		}

		if(rev == cnt)
			rev_pNode = NULL;
	}
	XML_CATCH_DEF

		return rev_pNode;
}
//---------------------------------------------------------------
bool XML_ȡ�ڵ�ֵ( __Class_XML::IXMLDOMNodePtr pNode , CString& �ڵ�ֵ )
{
	�ڵ�ֵ = L"";

	try
	{
		__Class_XML::IXMLDOMNodeListPtr pNodeList( pNode->GetchildNodes() );

#if __���ü��__
		if(!pNodeList)return false;
#endif

		__Class_XML::IXMLDOMNodePtr rev_pNode;

		if( XML_Ѱ��ָ�����ͽڵ�( pNodeList , rev_pNode , DOMNodeType::NODE_TEXT , 0) >= 0 )
		{
			�ڵ�ֵ = (WCHAR*)rev_pNode->text;
			return true;
		}
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool XML_ȡ����ֵ( __Class_XML::IXMLDOMNodePtr pNode , CStringW ������ , CStringW& ����ֵ )
{
	����ֵ = L"";

	try
	{
		__Class_XML::IXMLDOMElementPtr pElement( pNode );

#if __���ü��__
		if(!pElement)return false;
#endif

		_bstr_t ����ֵ_t2( ������ );

		_variant_t ����ֵ_t( pElement->getAttribute( ����ֵ_t2 ) );

		����ֵ = ����ֵ_t;
		return true;
	}
	XML_CATCH_DEF
		return false;
}
//---------------------------------------------------------------
static __Class_XML::IXMLDOMNodePtr ��·����ȡ�ڵ�( __Class_XML::IXMLDOMNodePtr pNode , CStringW ·�� )
{
	__Class_XML::IXMLDOMNodePtr rev_pNode ;
	try
	{
		WCHAR* path = ·��.GetBuffer();
		CStringW ����;

		���� = ȡ·����( &path ) ;

		rev_pNode = pNode;

		while( ����.GetLength() > 0 )
		{
			rev_pNode = XML_Ѱ�ҽڵ�( rev_pNode ,����);

			if( !pNode) break;

			���� = ȡ·����( &path ) ;
		}
	}
	XML_CATCH_DEF
		return rev_pNode;
}
//---------------------------------------------------------------
bool XML_�����ӽڵ�( __Class_XML::IXMLDOMElementPtr pĿ��Element , __Class_XML::IXMLDOMNodePtr pԴNode )
{
	__Class_XML::IXMLDOMNodePtr pNode;

	long cnt;

	try
	{
		__Class_XML::IXMLDOMNodeListPtr pԴNodeList( pԴNode->GetchildNodes() );

#if __���ü��__
		if(!pԴNodeList)return false;
#endif

		cnt = pԴNodeList->Getlength();

		long index;

		for(index = 0; pNode = pԴNodeList->nextNode() ; ++index)
			//		for(index = 0; pNode = pԴNodeList->Getitem( index ) ; ++index)
		{
			__OUT_DEGTEXT((char*)pNode->nodeName);

			//			cout << (char*)pNode->nodeTypeString << (int)pNode->nodeTypedValue  << endl;
			if( ! (bool)pĿ��Element->appendChild( pNode ) ) return false;
		}

		if( cnt != index ) return false;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
bool XML_��������( __Class_XML::IXMLDOMElementPtr pĿ��Element , __Class_XML::IXMLDOMNodePtr pԴNode )
{
	__Class_XML::IXMLDOMAttributePtr pNode;

	long cnt;

	try
	{
		__Class_XML::IXMLDOMNamedNodeMapPtr pԴNamedNodeMap( pԴNode->Getattributes() );

#if __���ü��__
		if(!pԴNamedNodeMap)return false;
#endif

		cnt = pԴNamedNodeMap->Getlength();

		long index;

		for(index = 0;pNode = pԴNamedNodeMap->nextNode() ; ++index)
		{
			//			if( !pĿ��Element->setAttributeNode( pNode ) )	return false;

			_variant_t val(pNode->text);

			TESTHR(pĿ��Element->setAttribute( pNode->nodeName , val ));
		}

		if( cnt != index) return false;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
CWQSG_XML::CWQSG_XML	(void)
:m_���ڵ���(_T(""))
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

		m_���ڵ��� = _T("");

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
bool CWQSG_XML::Create	( LPCWSTR �ڵ��� )
{
	if( !zzz_Init() )
	{
		__OUT_DEGTEXT("û�г�ʼ��");

		return false;
	}

	Close( );

	try
	{
		_bstr_t var�ڵ�(L"xml");
		_bstr_t varData(WQSG_XML_���ͷ);

		__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var�ڵ�,varData) );

		if( !ppp )
		{
			Close( );
			return false;
		}

		m_DocPtr->appendChild( ppp );

		var�ڵ� = �ڵ���;

		m_RootNode = m_DocPtr->createElement( var�ڵ� );

		m_DocPtr->appendChild( m_RootNode );

		m_BaseNode = m_RootNode;

		m_���ڵ��� = �ڵ���;

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
		__OUT_DEGTEXT("û�г�ʼ��");
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
			__OUT_DEGTEXT("��ȡ�ڵ��ʧ��");
			return false;
		}

		long �ڵ��� = XML_Ѱ��ָ�����ͽڵ�( pNodeList , m_RootNode ,DOMNodeType::NODE_PROCESSING_INSTRUCTION , 0L );

		if( �ڵ��� < 0L )
		{//�����ڱ�ʶͷ
			__OUT_DEGTEXT("�����ڱ�ʾͷ");

			�ڵ��� = 0L;

			_bstr_t var�ڵ�(L"xml");
			_bstr_t varData(WQSG_XML_���ͷ);

			__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var�ڵ�,varData) );

			if( !ppp ){	Close( );	return false;	}

			m_DocPtr->appendChild( ppp );
		}

		�ڵ��� = XML_Ѱ��ָ�����ͽڵ�( pNodeList , m_RootNode , DOMNodeType::NODE_ELEMENT , �ڵ��� );

		if( �ڵ��� < 0L )
		{//�����ڸ��ڵ�
			Close( );
			__OUT_DEGTEXT("û�и��ڵ�");
			return false;
		}

		m_���ڵ��� = (LPWSTR)m_RootNode->nodeName;

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
		__OUT_DEGTEXT("û�г�ʼ��");
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
			__OUT_DEGTEXT("��ȡ�ڵ��ʧ��");
			return false;
		}

		long �ڵ��� = XML_Ѱ��ָ�����ͽڵ�( pNodeList , m_RootNode ,DOMNodeType::NODE_PROCESSING_INSTRUCTION , 0L );

		if( �ڵ��� < 0L )
		{//�����ڱ�ʶͷ
			__OUT_DEGTEXT("�����ڱ�ʾͷ");
			�ڵ��� = 0L;

			_bstr_t var�ڵ�(L"xml");
			_bstr_t varData(WQSG_XML_���ͷ);

			__Class_XML::IXMLDOMProcessingInstructionPtr ppp( m_RootNode = m_DocPtr->createProcessingInstruction(var�ڵ�,varData) );

			if( !ppp ){	Close( );	return false;	}

			m_DocPtr->appendChild( ppp );
		}

		�ڵ��� = XML_Ѱ��ָ�����ͽڵ�( pNodeList , m_RootNode , DOMNodeType::NODE_ELEMENT , �ڵ��� );

		if( �ڵ��� < 0L )
		{//�����ڸ��ڵ�
			Close( );
			__OUT_DEGTEXT("û�и��ڵ�");
			return false;
		}

		m_���ڵ��� = (LPWSTR)m_RootNode->nodeName;

		m_BaseNode = m_RootNode;

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Save	( LPCWSTR lpPathName )
{
#if __���ü��__

	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
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
bool CWQSG_XML::Is��( void )
{
	return( m_���ڵ���.GetLength() == 0 );
}
//---------------------------------------------------------------
bool CWQSG_XML::��ӽڵ�( CStringW ·�� , CStringW �ڵ��� )
{
#if __���ü��__

	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}

#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNode ( ��·����ȡ�ڵ�( m_BaseNode , ·�� ) );

#if __���ü��__
		if( !pNode )return false;
#endif

		if( (bool)XML_Ѱ�ҽڵ�(pNode ,�ڵ���) ){return false;}

		_bstr_t �ڵ���_t(�ڵ���);

		__Class_XML::IXMLDOMElementPtr pElemen ( m_DocPtr->createElement ( �ڵ���_t ) );

#if __���ü��__
		if( !pElemen ) return false;
#endif

		return ( (bool)pNode->appendChild ( pElemen ) );
	}
	XML_CATCH_DEF

		return false;
}

//---------------------------------------------------------------
bool CWQSG_XML::ɾ���ڵ�( CStringW ·�� , CStringW �ڵ��� )
{

#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode, ·�� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif
		__Class_XML::IXMLDOMNodePtr pNode ( XML_Ѱ�ҽڵ�( pNodeRoot ,�ڵ���) );

#if __���ü��__
		if( !pNode ){	return false;	}
#endif

		_bstr_t �ڵ���_t(�ڵ���);

		return((bool)pNodeRoot->removeChild( pNode ) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::�޸Ľڵ���( CStringW ·�� , CStringW �ڵ��� , CStringW �½ڵ��� )
{

#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·�� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNodePtr pNode ( XML_Ѱ�ҽڵ�( pNodeRoot ,�ڵ��� ) );

#if __���ü��__
		if( !pNode )return false;
#endif
		_bstr_t �½ڵ���_t(�½ڵ���);

		__Class_XML::IXMLDOMElementPtr pElement( m_DocPtr->createElement( �½ڵ���_t ) );
#if __���ü��__
		if( !pElement )return false;
#endif

		if( ( !XML_��������( pElement , pNode ) ) ||
			( !XML_�����ӽڵ�( pElement , pNode ) ) )
			return false;

		return ((bool)pNodeRoot->replaceChild( pElement , pNode) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::�޸Ľڵ�ֵ( CStringW ·���ڵ��� , CStringW �½ڵ�ֵ )
{
#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNodePtr pNode;

		_bstr_t �½ڵ�ֵ_t(�½ڵ�ֵ);

		if( XML_Ѱ��ָ�����ͽڵ�( pNodeRoot ,pNode,DOMNodeType::NODE_TEXT,0) < 0)
		{//û�ҵ�
			pNode = m_DocPtr->createTextNode (�½ڵ�ֵ_t);

#if __���ü��__
			if(!pNode)return false;
#endif
			return ((bool)pNodeRoot->appendChild ( pNode ) );
		}
		else
		{
			pNode->Puttext( �½ڵ�ֵ_t );
		}

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::�޸Ľڵ�ֵ( CStringW ·�� , CStringW �ڵ��� , CStringW �½ڵ�ֵ )
{
	return �޸Ľڵ�ֵ( ·�� + L'/' + �ڵ��� , �½ڵ�ֵ );
}
//---------------------------------------------------------------
bool CWQSG_XML::�������( CStringW ·���ڵ��� , CStringW ������ , CStringW ����ֵ )
{

#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );
#if __���ü��__
		if( !pNodeRoot )return false;
#endif
		_variant_t ����ֵ_t(����ֵ);

		__Class_XML::IXMLDOMNodePtr pNode ( XML_Ѱ������( pNodeRoot ,������ ) );
		if(pNode)
		{
			//����
			return false;

			//			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNode) );

			//		_bstr_t ����ֵ_t2(����ֵ);

			//		pNode->Puttext( ����ֵ_t2 );
		}
		else
		{
			//������
			_bstr_t ������_t(������);

			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNodeRoot) );

			TESTHR ( pElement->setAttribute( ������_t , ����ֵ_t ) );
		}
		return true;
	}
	XML_CATCH_DEF
		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::ɾ������( CStringW ·���ڵ��� , CStringW ������ )
{

#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif

		__Class_XML::IXMLDOMNamedNodeMapPtr pNamedNodeMap ( pNodeRoot->Getattributes() );

#if __���ü��__
		if( !pNamedNodeMap )return false;
#endif

		_bstr_t ������_t(������);

		return ((bool)pNamedNodeMap->removeNamedItem( ������_t ) );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::�޸�������( CStringW ·���ڵ��� , CStringW ������ , CStringW �������� )
{
#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif
	CStringW ����ֵ;

	return( ȡ����ֵ(·���ڵ���,������,����ֵ) &&
		ɾ������(·���ڵ���,������) &&
		�������(·���ڵ��� ,�������� ,����ֵ ) );
}
//---------------------------------------------------------------
bool CWQSG_XML::�޸�����ֵ( CStringW ·���ڵ��� , CStringW ������ , CStringW ������ֵ )
{
#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif

		_variant_t ����ֵ_t( ������ֵ );

		__Class_XML::IXMLDOMNodePtr pNode ( XML_Ѱ������( pNodeRoot ,������ ) );

		if(pNode)
		{
			//����
			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNode) );

			_bstr_t ����ֵ_t2(������ֵ);
			pNode->Puttext( ����ֵ_t2 );
		}
		else
		{
			//������
			return false;
			//			_bstr_t ������_t(������);

			//			__Class_XML::IXMLDOMElementPtr pElement( /*static_cast<__Class_XML::IXMLDOMElementPtr>*/ (pNodeRoot) );

			//			TESTHR ( pElement->setAttribute( ������_t , ����ֵ_t ) );
		}
		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::ȡ����ֵ( CStringW ·���ڵ��� , CStringW ������ , CString& ����ֵ )
{
	����ֵ = L"";

#if __���ü��__

	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif
		__Class_XML::IXMLDOMElementPtr pElement( pNodeRoot );

		_bstr_t ����ֵ_t2( ������ );

		_variant_t ����ֵ_t( pElement->getAttribute( ����ֵ_t2 ) );

		����ֵ = ����ֵ_t;

		return true;
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
bool CWQSG_XML::Get�ڵ�ֵ( CStringW ·���ڵ��� , CString& �ڵ�ֵ )
{
	�ڵ�ֵ = L"";

#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return false;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return false;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·���ڵ��� ) );

#if __���ü��__
		if( !pNodeRoot )return false;
#endif

		return XML_ȡ�ڵ�ֵ( pNodeRoot ,�ڵ�ֵ );
	}
	XML_CATCH_DEF

		return false;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodeListPtr CWQSG_XML::Get�ڵ��б�(CStringW ·��)
{
#if __���ü��__
	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return NULL;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return NULL;
	}
#endif

	try
	{
		__Class_XML::IXMLDOMNodePtr pNodeRoot ( ��·����ȡ�ڵ�( m_BaseNode , ·�� ) );

#if __���ü��__
		if( !pNodeRoot )return NULL;
#endif
		return pNodeRoot->childNodes;
	}
	XML_CATCH_DEF

		return NULL;
}
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr CWQSG_XML::Get�ڵ�(CStringW ·��)
{
#if __���ü��__

	if( !m_Init )
	{
		__OUT_DEGTEXT("û�г�ʼ��");
		return NULL;
	}

	if(Is��())
	{
		__OUT_DEGTEXT("�յ�XML");
		return NULL;
	}
#endif

	return ��·����ȡ�ڵ�( m_BaseNode , ·�� );
}
//---------------------------------------------------------------
bool CWQSG_XML::SetBaseNode( CString ·���ڵ��� )
{
	try
	{
		__Class_XML::IXMLDOMNodePtr newNode( ��·����ȡ�ڵ�( m_RootNode , ·���ڵ��� ) );

		if( !newNode )return false;

		m_BaseNode = newNode;

		return true;
	}
	XML_CATCH_DEF

		return false;
}

#pragma warning(pop)

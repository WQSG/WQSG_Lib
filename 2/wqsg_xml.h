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

#ifndef __WQSG_XML_H__
#define __WQSG_XML_H__

#include <WQSG.h>
#import "msxml3.dll"

#define __Class_XML	MSXML2

#ifndef CStringW
#include <atlstr.h>
#endif

class CWQSG_XML :public CWQSG_ML
{
	CString m_根节点名;

	__Class_XML::IXMLDOMNodePtr m_RootNode;
	__Class_XML::IXMLDOMNodePtr m_BaseNode;
public:
	__Class_XML::IXMLDOMDocumentPtr m_DocPtr;

	CWQSG_XML(void);
	virtual	~CWQSG_XML(void);

	void Close(bool reStart = true);

	bool Create(LPCWSTR 节点名);

	bool Load(LPCWSTR lpPathName);

	bool Save(LPCWSTR lpPathName);

	bool Is空();

	bool 添加节点(CString 路径,CString 节点名);

	bool 删除节点(CString 路径,CString 节点名);

	bool 修改节点名(CString 路径,CString 节点名, CString 新节点名);

	bool 修改节点值(CString 路径节点名 , CString 新节点值);

	bool 修改节点值(CString 路径 , CString 节点名 , CString 新节点值);

	bool 添加属性(CString 路径节点名,CString 属性名,CString 属性值);

	bool 删除属性(CString 路径节点名 ,CString 属性名);

	bool 修改属性名(CString 路径节点名 ,CString 属性名, CString 新属性名);

	bool 修改属性值(CString 路径节点名 ,CString 属性名, CString 新属性值);

	bool 取属性值(CString 路径节点名 ,CString 属性名, CString & 属性值);

	CString Get根节点名(){return CString(m_根节点名);}

	bool Get节点值(CString 路径节点名 , CString& 节点值);

	__Class_XML::IXMLDOMNodeListPtr Get节点列表(CString 路径);

	__Class_XML::IXMLDOMNodePtr Get节点(CString 路径);

	__Class_XML::IXMLDOMDocumentPtr GetDoc(){	return m_DocPtr;	}

	bool LoadMem( _bstr_t memXml );

	bool SetBaseNode( CString 路径节点名 );
};

long XML_寻找指定类型节点( __Class_XML::IXMLDOMNodeListPtr pNodeList , __Class_XML::IXMLDOMNodePtr& 根节点名 , DOMNodeType NodeType , long StartIndex);
//---------------------------------------------------------------
long XML_寻找指定类型节点( __Class_XML::IXMLDOMNodePtr pNode , __Class_XML::IXMLDOMNodePtr& 根节点名 , DOMNodeType NodeType , long StartIndex);
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_寻找节点( __Class_XML::IXMLDOMNodePtr pNode , CStringW 节点名 );
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_寻找属性( __Class_XML::IXMLDOMNodePtr pNode , CStringW 属性名 );
//---------------------------------------------------------------
bool XML_取节点值( __Class_XML::IXMLDOMNodePtr pNode , CStringW& 节点值 );
//---------------------------------------------------------------
bool XML_取属性值( __Class_XML::IXMLDOMNodePtr pNode , CStringW 属性名 , CStringW& 属性值 );
//---------------------------------------------------------------
bool XML_复制子节点( __Class_XML::IXMLDOMElementPtr p目标Element , __Class_XML::IXMLDOMNodePtr p源Node );
//---------------------------------------------------------------
bool XML_复制属性( __Class_XML::IXMLDOMElementPtr p目标Element , __Class_XML::IXMLDOMNodePtr p源Node );

#include<windows.h>

#define OUT_ERR(e) do{\
	_tprintf(_T("Error:\n"));\
	CString ff;ff.Format(_T("Code = %08lx\nCode meaning = %s\nSource = %s\nError Description = %s\n"),\
	e.Error(), e.ErrorMessage(),(TCHAR*) e.Source(), (TCHAR*) e.Description());\
	_tprintf(_T("Code = %08lx\nCode meaning = %s\nSource = %s\nError Description = %s\n"),\
	e.Error(), e.ErrorMessage(),(TCHAR*) e.Source(), (TCHAR*) e.Description());\
	CString str;str.Format( _T("Code = %08lx\nCode meaning = %s\nSource = %s\nError Description = %s\n"),\
	e.Error(), e.ErrorMessage(),(TCHAR*) e.Source(), (TCHAR*) e.Description() );\
	::MessageBox( NULL, str ,NULL , MB_OK);\
}while(0)

#endif

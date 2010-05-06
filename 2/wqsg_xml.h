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
	CString m_���ڵ���;

	__Class_XML::IXMLDOMNodePtr m_RootNode;
	__Class_XML::IXMLDOMNodePtr m_BaseNode;
public:
	__Class_XML::IXMLDOMDocumentPtr m_DocPtr;

	CWQSG_XML(void);
	virtual	~CWQSG_XML(void);

	void Close(bool reStart = true);

	bool Create(LPCWSTR �ڵ���);

	bool Load(LPCWSTR lpPathName);

	bool Save(LPCWSTR lpPathName);

	bool Is��();

	bool ��ӽڵ�(CString ·��,CString �ڵ���);

	bool ɾ���ڵ�(CString ·��,CString �ڵ���);

	bool �޸Ľڵ���(CString ·��,CString �ڵ���, CString �½ڵ���);

	bool �޸Ľڵ�ֵ(CString ·���ڵ��� , CString �½ڵ�ֵ);

	bool �޸Ľڵ�ֵ(CString ·�� , CString �ڵ��� , CString �½ڵ�ֵ);

	bool �������(CString ·���ڵ���,CString ������,CString ����ֵ);

	bool ɾ������(CString ·���ڵ��� ,CString ������);

	bool �޸�������(CString ·���ڵ��� ,CString ������, CString ��������);

	bool �޸�����ֵ(CString ·���ڵ��� ,CString ������, CString ������ֵ);

	bool ȡ����ֵ(CString ·���ڵ��� ,CString ������, CString & ����ֵ);

	CString Get���ڵ���(){return CString(m_���ڵ���);}

	bool Get�ڵ�ֵ(CString ·���ڵ��� , CString& �ڵ�ֵ);

	__Class_XML::IXMLDOMNodeListPtr Get�ڵ��б�(CString ·��);

	__Class_XML::IXMLDOMNodePtr Get�ڵ�(CString ·��);

	__Class_XML::IXMLDOMDocumentPtr GetDoc(){	return m_DocPtr;	}

	bool LoadMem( _bstr_t memXml );

	bool SetBaseNode( CString ·���ڵ��� );
};

long XML_Ѱ��ָ�����ͽڵ�( __Class_XML::IXMLDOMNodeListPtr pNodeList , __Class_XML::IXMLDOMNodePtr& ���ڵ��� , DOMNodeType NodeType , long StartIndex);
//---------------------------------------------------------------
long XML_Ѱ��ָ�����ͽڵ�( __Class_XML::IXMLDOMNodePtr pNode , __Class_XML::IXMLDOMNodePtr& ���ڵ��� , DOMNodeType NodeType , long StartIndex);
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_Ѱ�ҽڵ�( __Class_XML::IXMLDOMNodePtr pNode , CStringW �ڵ��� );
//---------------------------------------------------------------
__Class_XML::IXMLDOMNodePtr XML_Ѱ������( __Class_XML::IXMLDOMNodePtr pNode , CStringW ������ );
//---------------------------------------------------------------
bool XML_ȡ�ڵ�ֵ( __Class_XML::IXMLDOMNodePtr pNode , CStringW& �ڵ�ֵ );
//---------------------------------------------------------------
bool XML_ȡ����ֵ( __Class_XML::IXMLDOMNodePtr pNode , CStringW ������ , CStringW& ����ֵ );
//---------------------------------------------------------------
bool XML_�����ӽڵ�( __Class_XML::IXMLDOMElementPtr pĿ��Element , __Class_XML::IXMLDOMNodePtr pԴNode );
//---------------------------------------------------------------
bool XML_��������( __Class_XML::IXMLDOMElementPtr pĿ��Element , __Class_XML::IXMLDOMNodePtr pԴNode );

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

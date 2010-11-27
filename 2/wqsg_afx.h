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

#ifndef __WQSG_afx_H__
#define __WQSG_afx_H__

inline void EditCheckHaxStr( CStringW& a_test , CEdit* a_pEdit_tmp , CWnd* a_pCwnd , BOOL a_bDelFirstZero = TRUE )
{
	CString cstr;

	INT i = 0;
	DWORD x = a_pEdit_tmp->GetSel() & 0xFFFF;

	while(a_bDelFirstZero &&
		(_T('0') == a_test.Left(1))
		&& (1 < a_test.GetLength())
		)
	{
		a_test.Delete(0);
		x--;
	}

	while(i < a_test.GetLength())
	{
		cstr = a_test.Mid(i,1);

		if(    (cstr == _T('0'))
			|| (cstr == _T('1'))
			|| (cstr == _T('2'))
			|| (cstr == _T('3'))
			|| (cstr == _T('4'))
			|| (cstr == _T('5'))
			|| (cstr == _T('6'))
			|| (cstr == _T('7'))
			|| (cstr == _T('8'))
			|| (cstr == _T('9'))
			|| (cstr == _T('A'))
			|| (cstr == _T('B'))
			|| (cstr == _T('C'))
			|| (cstr == _T('D'))
			|| (cstr == _T('E'))
			|| (cstr == _T('F'))
			)
		{
			i++;
		}
		else
		{
			a_test.Replace(cstr,_T(""));
			x--;
		}
	}

	a_pCwnd->UpdateData(FALSE);

	a_pEdit_tmp->SetSel(x,x,FALSE);
}
//--------------------------------------------------------------------------------
inline void EditCheckDecStr(CStringW& a_test , CEdit* a_pEdit_tmp , CWnd* a_pCwnd )
{
	INT i = 0;

	DWORD x = a_pEdit_tmp->GetSel() & 0xFFFF;
	while(
		(L'0' == a_test.Left(1))
		&& (1 < a_test.GetLength())
		)
	{
		a_test.Delete(0);
		x--;
	}

	WCHAR* str = a_test.GetBuffer();
	while( str[i] )
	{
		if( ( str[i] >= L'0' ) && ( str[i] <= L'9' ) )
		{
			i++;
		}
		else
		{
			a_test.Replace( str[i] , L'' );
			x--;
		}
	}

	a_pCwnd->UpdateData(FALSE);

	a_pEdit_tmp->SetSel( x  ,x , FALSE );
}
//--------------------------------------------------------------------------------
inline CString 取路径(CString 全地址)
{
	CString 路径;
	int i = 全地址.ReverseFind(L'\\');
	路径 = 全地址.Left(i);
	return 路径;
}

#endif

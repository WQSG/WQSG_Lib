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
//#include <windows.h>

#include <Commdlg.h>
#ifdef OPENFILENAME_SIZE_VERSION_400
#include "WQSG_xFile.h"
class WQSG_FileDlgW
{
	BOOL m_是否打开;
	WCHAR m_标题buf[MAX_PATH];
	WCHAR * m_文件名buf;
	WCHAR * m_过滤器buf;
	WCHAR * pos;
	OPENFILENAMEW m_ofn;

public:
	WQSG_FileDlgW(BOOL 是否打开,
	LPCWSTR lp默认扩展,
	LPCWSTR lp文件名,
	LPCWSTR lp过滤器,
	UINT 字数 = MAX_PATH,
	DWORD 标志 = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
	//DWORD dwSize = 0
	)
	{
		m_文件名buf = m_过滤器buf = NULL;
		if(字数 < MAX_PATH)
		{
			字数 = MAX_PATH;
		}
		DWORD dwSize;
	//	if (dwSize == 0)
	//	{
			OSVERSIONINFOW vi;
			ZeroMemory(&vi, sizeof(OSVERSIONINFOW));
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
			::GetVersionExW(&vi);
			// if running under NT and version is >= 5
			if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion >= 5)
				dwSize = sizeof(OPENFILENAMEW);
			else
				dwSize = OPENFILENAME_SIZE_VERSION_400W;
	//	}
		// size of OPENFILENAME must be at least version 4
		//ASSERT(dwSize >= OPENFILENAME_SIZE_VERSION_400);
#ifdef _DEBUG
			if(dwSize < OPENFILENAME_SIZE_VERSION_400)
			{
				::MessageBox(NULL,_T("版本无法继续"),NULL,MB_OK);
				return;
			}
#endif

		memset(&m_ofn, 0, dwSize); // initialize structure to 0/NULL
		m_文件名buf = new WCHAR[字数];
		m_文件名buf[0] = '\0';
		pos = m_文件名buf;
		m_标题buf[0] = '\0';
//		m_pofnTemp = NULL;

		m_是否打开 = 是否打开;
//		m_nIDHelp = bOpenFileDialog ? AFX_IDD_FILEOPEN : AFX_IDD_FILESAVE;

		m_ofn.lStructSize = dwSize;
		m_ofn.lpstrFile = m_文件名buf;
		m_ofn.nMaxFile = 字数;
		m_ofn.lpstrDefExt = lp默认扩展;
		m_ofn.lpstrFileTitle = (LPWSTR)m_标题buf;
		m_ofn.nMaxFileTitle = sizeof(m_标题buf)/ sizeof(*m_标题buf);
		m_ofn.Flags |= 标志 | OFN_ENABLEHOOK | OFN_EXPLORER;
		if(标志 & OFN_ENABLETEMPLATE)
			m_ofn.Flags &= ~OFN_ENABLESIZING;
//		m_ofn.hInstance = AfxGetResourceHandle();
//		m_ofn.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

		// setup initial file name
		if (lp文件名 != NULL)
			wcscpy(m_文件名buf,(WCHAR*)lp文件名);

		// Translate filter into commdlg format (lots of \0)
		if (lp过滤器 != NULL)
		{
			m_过滤器buf = new WCHAR[wcslen((WCHAR*)lp过滤器) + 1];
			wcscpy(m_过滤器buf,(WCHAR*)lp过滤器);
			for (INT i=0; m_过滤器buf[i]!='\0'; i++)
				if (m_过滤器buf[i] == '|')
					m_过滤器buf[i] = '\0';
			// do not call ReleaseBuffer() since the string contains '\0' characters
		}else
		{
			m_过滤器buf = new WCHAR[20];
			wcscpy(m_过滤器buf,L"所有文件|*.*|");
			for (INT i=0; m_过滤器buf[i]!='\0'; i++)
				if (m_过滤器buf[i] == '|')
					m_过滤器buf[i] = '\0';
		}
		m_ofn.lpstrFilter = m_过滤器buf;
	}
	virtual	~WQSG_FileDlgW()
	{
		delete[]m_过滤器buf;
		delete[]m_文件名buf;
	}
	virtual INT_PTR DoModal()
	{
		m_ofn.hwndOwner = GetFocus();
		pos = m_ofn.lpstrFile;
		BOOL 返回值;
		if(m_是否打开)
		{
			返回值 = GetOpenFileNameW(&m_ofn);
		}
		else
		{
			返回值 = GetSaveFileNameW(&m_ofn);
		}
		return ((返回值)?IDOK:IDCANCEL);
	}
	BOOL GetPathNameNext(WCHAR*lpSTR)
	{
		*lpSTR = '\0';
		if(pos != NULL)
		{
			WCHAR * tmp;
			if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
			{
				pos = NULL;
				tmp = m_ofn.lpstrFile;
				while((*(lpSTR++) = *(tmp++)) != '\0');
				return TRUE;
			}
			
			if(pos == m_ofn.lpstrFile)
			{//第一次
				while(*(pos++) != '\0');
				if(*pos == '\0')
				{//只选择了一个
					pos = NULL;
					tmp = m_ofn.lpstrFile;
					while((*(lpSTR++) = *(tmp++)) != '\0');
					return TRUE;
				}
			}
			//取路径
			tmp = m_ofn.lpstrFile;
			while((*(lpSTR++) = *(tmp++)) != '\0');
			lpSTR--;
			*(lpSTR++) = L'\\';
			while((*(lpSTR++) = *(pos++)) != '\0');
			if(*pos == '\0')
				pos = NULL;
			return TRUE;
		}
		return FALSE;
	}
	BOOL GetPathName(WCHAR*lpSTR)
	{
		*lpSTR = '\0';
		WCHAR * tmp = m_ofn.lpstrFile;
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
		{
			while((*(lpSTR++) = *(tmp++)) != '\0');
			return TRUE;
		}

		while((*(lpSTR++) = *(tmp++)) != '\0');
		if(*pos == '\0')
		{//只选择了一个
			return TRUE;
		}
		lpSTR--;
		*(lpSTR++) = L'\\';
		while((*(lpSTR++) = *(tmp++)) != '\0');
		return TRUE;
	}
};
#endif
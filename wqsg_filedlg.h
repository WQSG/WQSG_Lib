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
	BOOL m_�Ƿ��;
	WCHAR m_����buf[MAX_PATH];
	WCHAR * m_�ļ���buf;
	WCHAR * m_������buf;
	WCHAR * pos;
	OPENFILENAMEW m_ofn;

public:
	WQSG_FileDlgW(BOOL �Ƿ��,
	LPCWSTR lpĬ����չ,
	LPCWSTR lp�ļ���,
	LPCWSTR lp������,
	UINT ���� = MAX_PATH,
	DWORD ��־ = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
	//DWORD dwSize = 0
	)
	{
		m_�ļ���buf = m_������buf = NULL;
		if(���� < MAX_PATH)
		{
			���� = MAX_PATH;
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
				::MessageBox(NULL,_T("�汾�޷�����"),NULL,MB_OK);
				return;
			}
#endif

		memset(&m_ofn, 0, dwSize); // initialize structure to 0/NULL
		m_�ļ���buf = new WCHAR[����];
		m_�ļ���buf[0] = '\0';
		pos = m_�ļ���buf;
		m_����buf[0] = '\0';
//		m_pofnTemp = NULL;

		m_�Ƿ�� = �Ƿ��;
//		m_nIDHelp = bOpenFileDialog ? AFX_IDD_FILEOPEN : AFX_IDD_FILESAVE;

		m_ofn.lStructSize = dwSize;
		m_ofn.lpstrFile = m_�ļ���buf;
		m_ofn.nMaxFile = ����;
		m_ofn.lpstrDefExt = lpĬ����չ;
		m_ofn.lpstrFileTitle = (LPWSTR)m_����buf;
		m_ofn.nMaxFileTitle = sizeof(m_����buf)/ sizeof(*m_����buf);
		m_ofn.Flags |= ��־ | OFN_ENABLEHOOK | OFN_EXPLORER;
		if(��־ & OFN_ENABLETEMPLATE)
			m_ofn.Flags &= ~OFN_ENABLESIZING;
//		m_ofn.hInstance = AfxGetResourceHandle();
//		m_ofn.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

		// setup initial file name
		if (lp�ļ��� != NULL)
			wcscpy(m_�ļ���buf,(WCHAR*)lp�ļ���);

		// Translate filter into commdlg format (lots of \0)
		if (lp������ != NULL)
		{
			m_������buf = new WCHAR[wcslen((WCHAR*)lp������) + 1];
			wcscpy(m_������buf,(WCHAR*)lp������);
			for (INT i=0; m_������buf[i]!='\0'; i++)
				if (m_������buf[i] == '|')
					m_������buf[i] = '\0';
			// do not call ReleaseBuffer() since the string contains '\0' characters
		}else
		{
			m_������buf = new WCHAR[20];
			wcscpy(m_������buf,L"�����ļ�|*.*|");
			for (INT i=0; m_������buf[i]!='\0'; i++)
				if (m_������buf[i] == '|')
					m_������buf[i] = '\0';
		}
		m_ofn.lpstrFilter = m_������buf;
	}
	virtual	~WQSG_FileDlgW()
	{
		delete[]m_������buf;
		delete[]m_�ļ���buf;
	}
	virtual INT_PTR DoModal()
	{
		m_ofn.hwndOwner = GetFocus();
		pos = m_ofn.lpstrFile;
		BOOL ����ֵ;
		if(m_�Ƿ��)
		{
			����ֵ = GetOpenFileNameW(&m_ofn);
		}
		else
		{
			����ֵ = GetSaveFileNameW(&m_ofn);
		}
		return ((����ֵ)?IDOK:IDCANCEL);
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
			{//��һ��
				while(*(pos++) != '\0');
				if(*pos == '\0')
				{//ֻѡ����һ��
					pos = NULL;
					tmp = m_ofn.lpstrFile;
					while((*(lpSTR++) = *(tmp++)) != '\0');
					return TRUE;
				}
			}
			//ȡ·��
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
		{//ֻѡ����һ��
			return TRUE;
		}
		lpSTR--;
		*(lpSTR++) = L'\\';
		while((*(lpSTR++) = *(tmp++)) != '\0');
		return TRUE;
	}
};
#endif
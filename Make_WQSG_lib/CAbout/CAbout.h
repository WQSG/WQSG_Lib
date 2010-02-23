#pragma once

#include <shellapi.h>
#include <commctrl.h>
#include <objbase.h>

#pragma pack(push,1)
struct SWQSG_AboutDlgID
{
	int m_nIcon;
	int m_nAppName;
	int m_nLink;
	int m_nEdit;
};
#pragma pack(pop)


class CWQSG_About
{
protected:
	HWND m_hWndParent;
	HICON m_hIcon;
	const WCHAR* m_pTitle;
	const WCHAR* m_pAppName;
	const WCHAR* m_pUrl;
	const WCHAR* m_pProgrammed;
public:
	CWQSG_About()
		: m_hWndParent(NULL)
		, m_hIcon(NULL)
		, m_pTitle(NULL)
		, m_pAppName(NULL)
		, m_pUrl(NULL)
		, m_pProgrammed(NULL)
	{

	}

	inline static INT_PTR CALLBACK AboutFunc(HWND, UINT, WPARAM, LPARAM);

	inline INT_PTR WQSG_About( HICON a_hIcon , HWND a_hWndParent , const WCHAR* a_pTitle , const WCHAR* a_pAppName ,
		const WCHAR* a_pUrl , const WCHAR* a_pProgrammed );
};

inline CWQSG_About& GetWQSGAbout();

inline INT_PTR CWQSG_About::WQSG_About( HICON a_hIcon , HWND a_hWndParent , const WCHAR* a_pTitle , const WCHAR* a_pAppName ,
								const WCHAR* a_pUrl , const WCHAR* a_pProgrammed )
{
	GetWQSGAbout().m_hIcon = a_hIcon;
	GetWQSGAbout().m_hWndParent = a_hWndParent;
	GetWQSGAbout().m_pTitle = a_pTitle;
	GetWQSGAbout().m_pAppName = a_pAppName;
	GetWQSGAbout().m_pUrl = a_pUrl;
	GetWQSGAbout().m_pProgrammed = a_pProgrammed;

	return DialogBoxIndirect( NULL , LPDLGTEMPLATE , GetWQSGAbout().m_hWndParent , CWQSG_About::AboutFunc );
}

inline INT_PTR CALLBACK CWQSG_About::AboutFunc(HWND a_hDlg, UINT a_message, WPARAM a_wParam, LPARAM a_lParam)
{
	switch (a_message)
	{
	case WM_INITDIALOG:
		SendMessage( a_hDlg , WM_SETICON , TRUE , (LPARAM)GetWQSGAbout().m_hIcon );
		SendMessage( a_hDlg , WM_SETICON , FALSE , (LPARAM)GetWQSGAbout().m_hIcon );

		SetWindowText( a_hDlg , GetWQSGAbout().m_pTitle );

		SendMessage( GetDlgItem( a_hDlg , IDC_LOGO ) , STM_SETICON, (WPARAM)GetWQSGAbout().m_hIcon , 0L);
		SetDlgItemText( a_hDlg , IDC_APP_NAME , GetWQSGAbout().m_pAppName );

		SetDlgItemText( a_hDlg , IDC_SYSLINK1 , GetWQSGAbout().m_pUrl );

		SetDlgItemText( a_hDlg , IDC_EDIT1 , GetWQSGAbout().m_pProgrammed );

		return (INT_PTR)TRUE;
	case WM_NOTIFY:
		{
			const LPNMHDR pNMHdr = (LPNMHDR)a_lParam;
			switch (pNMHdr->code)
			{
				//case NM_RETURN:
			case NM_CLICK:
				{
					const PNMLINK pNMLink = (PNMLINK)a_lParam;

					if( *pNMLink->item.szUrl )
						ShellExecute( NULL , L"open" , pNMLink->item.szUrl , NULL , NULL , SW_SHOW );
				}
				break;
			};
		}
	case WM_COMMAND:
		if (LOWORD(a_wParam) == IDOK || LOWORD(a_wParam) == IDCANCEL)
		{
			EndDialog( a_hDlg , LOWORD(a_wParam) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

inline CWQSG_About& GetWQSGAbout()
{
	static CWQSG_About about;
	return about;
}

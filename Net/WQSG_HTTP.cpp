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
#include "wqsg_http.h"
#include <atltime.h>
#define DEFAULT_HTTP_PORT   80
#define PM_ON 1
//-----------------------------------------------------------------------------------------------------------
static inline bool zzz_WriteFile( CWQSG_xFile& fp , void const*const buf , const s64 start , const u32 len )
{
	fp.Seek( start );
	return( len == fp.Write( buf , len ) );
}
static	CStringA	cnUrl2UTF8( CStringA inputString )
	{
		WCHAR*	pTmp = WQSG_char_W( inputString.GetBuffer() );
		CStringW __str( pTmp );
		delete[]pTmp;pTmp = __str.GetBuffer();

		inputString = "";

		while( *pTmp )
		{
			if( *pTmp > 0xFF )
			{
				CStringW str( *pTmp );
				char* utf = WQSG_W_UTF8( str.GetBuffer() );
				CStringA str2( utf );
				delete[]utf;utf = str2.GetBuffer();
				while(*utf)
				{
					inputString.AppendFormat( "%%%02X" , *(u8*)utf );
					++utf;
				}
			}
			else
			{
				if( *pTmp == ' ' )
				{
					inputString += "%20";
				}
				else
					inputString += *pTmp;
			}
			++pTmp;
		}
		return CStringA( inputString );
	}
enum			en_Transfer_Encoding
{
	en_Transfer_Encoding_NULL,
	en_Transfer_Encoding_chunked,
};
struct tgHTTP_HeadInfo
{
	enum en_HTTP_RETUEN
	{
		enHTTP_OK         = 0,
		enHTTP_ERROR      = 1,
		enHTTP_REDIRECT   = 2,
		enHTTP_FAIL       = 3,
	};
	DWORD		dwStatusCode;
	s64			zi64ContentLength;
	s64			i64FileSize;
	CStringA    strAHostName;
	CStringA    strAObject;
	u16         nPort;
	bool		bIsHtml;
	CStringA	strType;
	en_Transfer_Encoding	m_Transfer_Encoding;
	//-------------------------------------------------------
	void �ָ���ʼ��()
	{
		zi64ContentLength = i64FileSize = 0;
		strAHostName = "";
		strAObject = "";
		nPort = DEFAULT_HTTP_PORT;
		m_Transfer_Encoding = en_Transfer_Encoding_NULL;
		bIsHtml = false;
		strType = "";
	}
	//-------------------------------------------------------
	CStringA	CreateHred( const s64 i64Offset )
	{
		CStringA head;
		CStringA tmpStr;
//		if( dw������ַ < i64Offset )
//			return head;

		head.Format( "\
GET %s HTTP/1.1\r\n\
Host:%s\r\n\
Accept:*/*\r\n\
User-Agent: HttpDownload/2.0 \r\n\
Connection: close\r\n"
	,strAObject.GetBuffer() , strAHostName.GetBuffer()
	);

//		Pragma: no-cache\r\n\
//Cache-Control: no-cache\r\n\

		if( i64Offset >= 0 )
		{
			tmpStr.Format( "Range: bytes=%llu-\r\n" , i64Offset );
			head += tmpStr;
		}

		head += "\r\n";

		return head;
	}
	//-------------------------------------------------------
	en_HTTP_RETUEN	GetHeadInfo( CStringA strHeader )
	{
		zi64ContentLength = i64FileSize = 0;
		m_Transfer_Encoding = en_Transfer_Encoding_NULL;
		CTime TimeLastModified( CTime::GetCurrentTime() );

		strHeader.MakeLower();
		//��ֳ�HTTPӦ���ͷ��Ϣ�ĵ�һ��
		int nPos = strHeader.Find("\r\n");
		if (nPos == -1)
			return en_HTTP_RETUEN::enHTTP_FAIL;
		CStringA strFirstLine = strHeader.Left(nPos);

		// ��÷�����: Status Code
		strFirstLine.TrimLeft();
		strFirstLine.TrimRight();
		nPos = strFirstLine.Find(' ');
		if( nPos == -1 )
			return en_HTTP_RETUEN::enHTTP_FAIL;

		strFirstLine = strFirstLine.Mid( nPos+1 );
		nPos = strFirstLine.Find(' ');
		if( nPos == -1 )
			return en_HTTP_RETUEN::enHTTP_FAIL;

		strFirstLine = strFirstLine.Left(nPos);
		dwStatusCode = (DWORD)atoi( strFirstLine.GetBuffer() );

		// ���������󣬿�������
		if( dwStatusCode >=500 )
			return en_HTTP_RETUEN::enHTTP_ERROR;
		// �ͻ��˴�����������
		else if( dwStatusCode >=400 && dwStatusCode <500 )
			return en_HTTP_RETUEN::enHTTP_FAIL;
		// ��鷵���� //���ȼ��һ�·�������Ӧ���Ƿ�Ϊ�ض���
		else if( dwStatusCode >= 300 && dwStatusCode < 400 ) 
		{
			nPos = strHeader.Find("location:");
			if (nPos == -1)
				return en_HTTP_RETUEN::enHTTP_FAIL;

			CStringA strRedirectFileName = strHeader.Mid(nPos + (int)strlen("location:"));
			nPos = strRedirectFileName.Find("\r\n");
			if (nPos == -1)
				return en_HTTP_RETUEN::enHTTP_FAIL;

			strRedirectFileName = strRedirectFileName.Left(nPos);
			strRedirectFileName.TrimLeft();
			strRedirectFileName.TrimRight();

			// �ж��Ƿ��ض��������ķ�����
			nPos = strRedirectFileName.Find("http://");
			if( nPos != -1 )
			{
				// ����Ҫ���ص�URL�Ƿ���Ч
				if ( ParseURL( strRedirectFileName.GetBuffer() ))
					return en_HTTP_RETUEN::enHTTP_REDIRECT;

				return en_HTTP_RETUEN::enHTTP_FAIL;
			}

			// �ض��򵽱��������������ط�
			strRedirectFileName.Replace("\\","/");

			// ������ڸ�Ŀ¼
			if( strRedirectFileName[0] == '/' )
			{
				strAObject = strRedirectFileName;
				return en_HTTP_RETUEN::enHTTP_REDIRECT;
			}

			// ����Ե�ǰĿ¼
			CStringA strDownloadUrl( "http://" + strAHostName + strAObject );

			int nParentDirCount = 0;
			nPos = strRedirectFileName.Find("../");
			while (nPos != -1)
			{
				strRedirectFileName = strRedirectFileName.Mid(nPos+3);
				nParentDirCount++;
				nPos = strRedirectFileName.Find("../");
			}
			for (int i=0; i<=nParentDirCount; i++)
			{
				nPos = strDownloadUrl.ReverseFind('/');
				if (nPos != -1)
					strDownloadUrl = strDownloadUrl.Left(nPos);
			}
			strDownloadUrl += ( "/" + strRedirectFileName );

			if ( ParseURL( strDownloadUrl.GetBuffer() ))
				return en_HTTP_RETUEN::enHTTP_REDIRECT;

			return en_HTTP_RETUEN::enHTTP_FAIL;
		}
		else if( dwStatusCode >= 200 && dwStatusCode < 300 )
		{
			//-----------------------------------------------isHtml
			{
				nPos = strHeader.Find("content-type:");
				if (nPos == -1)
					en_HTTP_RETUEN::enHTTP_FAIL;

				strType = strHeader.Mid(nPos + (int)strlen("content-type:") );	
				nPos = strType.Find("\r\n");
				if (nPos == -1)
					return en_HTTP_RETUEN::enHTTP_FAIL;

				strType = strType.Left(nPos);	
				strType.TrimLeft();
				strType.TrimRight();

				bIsHtml = ( strType == "text/html" );
			}

			//--------------------------------------------------
			nPos = strHeader.Find("transfer-encoding:");
			///���������
			if (nPos == -1)
			{
				// ��ȡContentLength
				nPos = strHeader.Find("content-length:");
				if (nPos == -1)
				{
					//���� html �� �޳�������
					if( !bIsHtml )
						return en_HTTP_RETUEN::enHTTP_FAIL;
					zi64ContentLength = -1;
				}
				else
				{
					CStringA strDownFileLen = strHeader.Mid(nPos + (int)strlen("content-length:"));	
					nPos = strDownFileLen.Find("\r\n");
					if (nPos == -1)
						return en_HTTP_RETUEN::enHTTP_FAIL;

					strDownFileLen = strDownFileLen.Left(nPos);	
					strDownFileLen.TrimLeft();
					strDownFileLen.TrimRight();

					// Content-Length:
					zi64ContentLength = _atoi64( strDownFileLen.GetBuffer() );
				}
				//----------------------------------------------------------------------------
				nPos = strHeader.Find("content-range:");
				if (nPos == -1)
					i64FileSize = zi64ContentLength;
				else
				{
					CStringA strDownFileLen = strHeader.Mid(nPos + (int)strlen("content-range:"));
					nPos = strDownFileLen.Find("\r\n");
					if (nPos == -1)
						return en_HTTP_RETUEN::enHTTP_FAIL;

					strDownFileLen = strDownFileLen.Left(nPos);

					nPos = strDownFileLen.Find( L'/' );
					if (nPos == -1)
						return en_HTTP_RETUEN::enHTTP_FAIL;

					strDownFileLen = strDownFileLen.Mid( nPos + 1 );

					strDownFileLen.TrimLeft();
					strDownFileLen.TrimRight();

					i64FileSize = _atoi64( strDownFileLen.GetBuffer() );
				}
			}
			///�б���
			else
			{
				CStringA strDownFileLen = strHeader.Mid( nPos + (int)strlen("transfer-encoding:") );	
				nPos = strDownFileLen.Find("\r\n");
				if (nPos == -1)
					return en_HTTP_RETUEN::enHTTP_FAIL;

				strDownFileLen = strDownFileLen.Left(nPos);	
				strDownFileLen.TrimLeft();
				strDownFileLen.TrimRight();

				if( "chunked" == strDownFileLen )
				{
					m_Transfer_Encoding = en_Transfer_Encoding_chunked;
				}
				else
					return en_HTTP_RETUEN::enHTTP_FAIL;

				i64FileSize = zi64ContentLength = -1;
			}
			//---------------------------------------------------- ��ȡLast-Modified:
			nPos = strHeader.Find("last-modified:");
			if (nPos != -1)
			{
				CStringA strTime = strHeader.Mid(nPos + (int)strlen("last-modified:"));
				nPos = strTime.Find("\r\n");
				if (nPos != -1)
				{
					strTime = strTime.Left(nPos);
					strTime.TrimLeft();
					strTime.TrimRight();
		//			TimeLastModified = GetTime(strTime);
				}
			}


			return en_HTTP_RETUEN::enHTTP_OK;
		}
		return en_HTTP_RETUEN::enHTTP_FAIL;
	}
	//-------------------------------------------------------
	BOOL		ParseURL( CStringA strURL )
	{
		strURL.TrimLeft();
		strURL.TrimRight();
		strURL = cnUrl2UTF8( strURL );
		// �������
		strAHostName = strAObject = _T("");nPort	  = 0;

		int nPos = strURL.Find("://");
		if( nPos == -1 )
			return FALSE;

		// ��һ����֤�Ƿ�Ϊhttp://
		CStringA strTemp = strURL.Left( nPos+lstrlenA("://") );
		strTemp.MakeLower();
		if( strTemp.Compare("http://") != 0 )
			return FALSE;

		strURL = strURL.Mid( strTemp.GetLength() );
		nPos = strURL.Find('/');
		if ( nPos == -1 )
			return FALSE;

		strAObject = strURL.Mid(nPos);
		strTemp   = strURL.Left(nPos);

		///////////////////////////////////////////////////////////////
		/// ע�⣺��û�п���URL�����û����Ϳ�������κ������#������
		/// ���磺http://abc@def:www.yahoo.com:81/index.html#link1
		/// 
		//////////////////////////////////////////////////////////////

		// �����Ƿ��ж˿ں�
		nPos = strTemp.Find(":");
		if( nPos == -1 )
		{
			strAHostName = strTemp;
			nPort	  = DEFAULT_HTTP_PORT;
		}
		else
		{
			strAHostName = strTemp.Left( nPos );
			strTemp	  = strTemp.Mid( nPos+1 );
			nPort	  = (u16)atoi(strTemp.GetBuffer());
		}
		return TRUE;
	}
};
//-------------------------------------------------------
static int �ֽ��һ�����ݰ�ȡ��ͷ��( char* buf , CStringA& head )
{
	char* sp1 = buf;
	char* sp2 = sp1;
	head = "";
	while( sp2 = strstr( sp1 , "\r\n" ) )
	{
		if(((int)(sp2 - sp1)) < 2)
		{
			*sp1 = 0;
			head = buf;
			return (int)( (sp2 - buf) + 2);
		}
		sp1 = sp2 + 2;
	}
	return -1;
}
#undef DEFAULT_HTTP_PORT//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
struct _tgWHTTP_����
{
	CWQSG_Http*		http;
	CStringA		url;
	CWQSG_xFile*	fp;
	s64 test_sizefile;
	_tgWHTTP_����( CWQSG_Http*	_http , CStringA _url , CWQSG_xFile* _fp , s64 _test_sizefile )
		: http(_http) , url(_url) , fp(_fp) , test_sizefile(_test_sizefile)
	{
	}
};
void CWQSG_Http::zzz_DownloadThread( char const*const url , CWQSG_xFile& fp , const s64 test_sizefile )
{
	m_fileOffset = 0;
	m_fileSize = (test_sizefile >= 0)?test_sizefile:-1;
	///����  /////////
	class CDTP
	{
	public:
		struct DTP
		{
			CWQSG_socket_client		cSocket;
			char					szBuffer[1024*8];
			const int				iszBufferByteSize;
			int						i�Ѿ����Դ���;
			CStringA				strHtml;

			tgHTTP_HeadInfo			fileInfo;
			//----------------------------------------------
			DTP(): iszBufferByteSize( sizeof(szBuffer) )
				, i�Ѿ����Դ���(0)
			{
				fileInfo.�ָ���ʼ��();
			}
		};
	private:
		DTP*		m_DTP;
	public:
		CDTP():m_DTP(NULL)	{ }
		~CDTP()				{ delete m_DTP; }
		DTP* GetDTP()
		{
			if( m_DTP == NULL )
				m_DTP = new DTP;

			return m_DTP;
		}
	};
	CDTP cDtp;
	CDTP::DTP* pDtp = cDtp.GetDTP();
	int			iRev;
	if( pDtp == NULL )
		goto __gt�����˳�;

	{
		const s64 fileSize = fp.GetFileSize( );
		if( fileSize < 0 )
			goto __gt�����˳�;

		m_fileOffset = fileSize;
	}
    //�ֽ��ַ
	if( !pDtp->fileInfo.ParseURL( url ) )
		goto __gt�����˳�;
    //���� ���� ����
	GetSpeed();
__tg����:
	if( 0 != pDtp->cSocket.Close() )
		goto __gt�����˳�;
	{
		CStringA head;
		while( true )
		{
			if( m_bStopDownload )
				goto __gt�û��˳�;

			if( !m_bPauseDownload )
			{
				m_state = en_STATE_RUN;
				break;
			}
			m_state = en_STATE_PAUSE;
			Sleep(10);
		}
		//��������ͷ
		head = pDtp->fileInfo.CreateHred( m_fileOffset );
		iRev = head.GetLength();
		if( iRev <= 0 )
			goto __gt�����˳�;

		{
			in_addr	addr;
			{
				//��ȡIP tgInfo.strAHostName
				hostent*const tghost = gethostbyname( pDtp->fileInfo.strAHostName.GetBuffer() );
				if( tghost == NULL )
					goto __gt�����˳�;

				memcpy( &addr , tghost->h_addr_list[0] , sizeof(in_addr) );
			}
#ifdef DEBUG
			char* pp = inet_ntoa( addr );
#endif
			if( 0 != pDtp->cSocket.Connect( inet_ntoa( addr ) , pDtp->fileInfo.nPort ) )
				goto __gt�����˳�;
		}
		iRev = pDtp->cSocket.Send( head.GetBuffer() , head.GetLength() , m_dwTimeout );
		if( iRev <= 0 )
			goto __gt��������;
	}

    ///����ͷ���Ͳ�������
	memset( pDtp->szBuffer , 0 , pDtp->iszBufferByteSize );
	char* pBuffer = pDtp->szBuffer;
	{
		int iOffset;
		CStringA tmpStr;

		int bufSize = pDtp->iszBufferByteSize;
		do
		{
			iRev = zzz_recv( pDtp->cSocket , pBuffer , bufSize );
			if( iRev <= 0 )
				goto __gt�����˳�;

			pBuffer += iRev;
			bufSize -= iRev;

			iOffset = �ֽ��һ�����ݰ�ȡ��ͷ��( pDtp->szBuffer , tmpStr );
		}while( iOffset < 0 );

		switch( pDtp->fileInfo.GetHeadInfo( tmpStr ) )
		{
		case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_OK://�ɹ�
			if( m_fileOffset < 0 )
				goto __gt�����˳�;
			switch( pDtp->fileInfo.dwStatusCode )
			{
			case 200:
				if( m_fileOffset != 0 )
					if( !fp.SetFileLength( 0 ) )
						goto __gt�����˳�;
				m_fileOffset = 0;
				break;
			case 206:
				break;
			default:
				goto __gt�����˳�;
			}
			break;
		case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_REDIRECT://�ض���
			goto __tg����;//һ������
			break;
		case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_ERROR://����
			goto __gt��������;
		case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_FAIL://ʧ��
			if( ( pDtp->fileInfo.dwStatusCode == 416 ) && ( pDtp->fileInfo.i64FileSize == m_fileOffset ) )
				goto __gt���;
			goto __gtʧ���˳�;
		default:
			goto __gt�����˳�;
		}
		pBuffer = pDtp->szBuffer + iOffset;
		iRev -= iOffset;
	}
	switch( pDtp->fileInfo.m_Transfer_Encoding )
	{
	case en_Transfer_Encoding::en_Transfer_Encoding_chunked:
		pDtp->fileInfo.i64FileSize = 0;
		break;
	case en_Transfer_Encoding::en_Transfer_Encoding_NULL:
		break;
	default:
		goto __gt�����˳�;
	}
__gt�����:
	switch( pDtp->fileInfo.m_Transfer_Encoding )
	{
	case en_Transfer_Encoding::en_Transfer_Encoding_chunked:
		{
			CStringA	strTmp( pBuffer );
			int pos = strTmp.Find( "\r\n" );
			if( pos <= 0 )
				goto __gt�����˳�;
			//---------------------------
			pBuffer += ( pos + 2 );
			iRev -= ( pos + 2 );
			//-----------------------------
			strTmp = strTmp.Left( pos );
			strTmp = strTmp.TrimLeft();
			strTmp = strTmp.TrimRight();
			pos = strTmp.GetLength();
			if( ( pos <= 0 ) || (pos > 8) ||
				(!::WQSG_��ʮ�������ı�( strTmp.GetBuffer() )) )
				goto __gt�����˳�;

			unsigned int iChunkedLen = 0;
			::sscanf( strTmp.GetBuffer() , "%x" , &iChunkedLen );
			if( 0 == iChunkedLen )
				goto __gt���;
			pDtp->fileInfo.i64FileSize += iChunkedLen;
		}
		break;
	case en_Transfer_Encoding::en_Transfer_Encoding_NULL:
		if( ( test_sizefile >= 0 ) && ( pDtp->fileInfo.i64FileSize != test_sizefile ) )
			goto __gt�����˳�;
		m_fileSize = pDtp->fileInfo.i64FileSize;
		break;
	default:
		goto __gt�����˳�;
	}
	if( pDtp->fileInfo.i64FileSize < 0 )
	{
		if( !pDtp->fileInfo.bIsHtml )
			goto __gt�����˳�;

		pDtp->fileInfo.i64FileSize = pDtp->iszBufferByteSize;
	}

	{
__gtHtml:
		s64 ʣ��len = pDtp->fileInfo.i64FileSize - m_fileOffset;
		while( true )
		{
			const int wlen = ( (s64)iRev > ʣ��len )?(int)(ʣ��len):(iRev);
			if( wlen < 0 )
				goto __gt�����˳�;
			else if( wlen > 0 )
			{
				if( !zzz_WriteFile( fp , pBuffer , m_fileOffset ,wlen ) )
					goto __gt�����˳�;

				m_fileOffset += wlen;
				ʣ��len -= wlen;
				iRev -= wlen;//����,�󷽼��

				if( pDtp->fileInfo.bIsHtml )
				{
					pDtp->strHtml.Append( pBuffer , wlen );
					int pos = pDtp->strHtml.ReverseFind( '<' );
					if( pos > 0 )
						pDtp->strHtml = pDtp->strHtml.Mid( pos );

					pBuffer += wlen;
					for( char* tmp = pDtp->strHtml.GetBuffer() ; *tmp ; ++tmp )
					{
						if( (*tmp >= 'A') && (*tmp <= 'Z') )
							(*tmp) += 0x20;
					}

					if( pDtp->strHtml.Find( "</html>" ) >= 0 )
					{
						pDtp->fileInfo.i64FileSize = m_fileOffset;
						goto __gt���;
					}
				}
			}
			if( m_fileOffset >= pDtp->fileInfo.i64FileSize )
				break;
			///-----------
			while( true )
			{
				if( m_bStopDownload )
					goto __gt�û��˳�;

				if( !m_bPauseDownload )
				{
					m_state = en_STATE_RUN;
					break;
				}
				m_state = en_STATE_PAUSE;
				Sleep(10);
			}
			///-----------
			iRev = zzz_recv( pDtp->cSocket , pDtp->szBuffer , pDtp->iszBufferByteSize );
			if( iRev <= 0 )
			{
				if( pDtp->fileInfo.bIsHtml )
				{
					for( char* tmp = pDtp->strHtml.GetBuffer() ; *tmp ; ++tmp )
					{
						if( (*tmp >= 'A') && (*tmp <= 'Z') )
							(*tmp) += 0x20;
					}
					if( pDtp->strHtml.Find( "</html>" ) >= 0 )
					{
						pDtp->fileInfo.i64FileSize = m_fileOffset;
						goto __gt���;
					}
				}
				goto __gt��������;
			}
			pBuffer = pDtp->szBuffer;
		}
		switch( pDtp->fileInfo.m_Transfer_Encoding )
		{
		case en_Transfer_Encoding::en_Transfer_Encoding_chunked:
			if( iRev > 0 )
			{
				if( iRev < 2 )
					goto __gt�����˳�;
				iRev -= 2;
				pBuffer += 2;
				goto __gt�����;
			}
		case en_Transfer_Encoding::en_Transfer_Encoding_NULL:
			if( pDtp->fileInfo.bIsHtml )
			{
				pDtp->fileInfo.i64FileSize += pDtp->iszBufferByteSize;
				goto __gtHtml;
			}
			if( iRev > 0 )
				goto __gt�����˳�;
			break;
		default:
			goto __gt�����˳�;
		}
	}
__gt���:
	//���
	m_fileSize = pDtp->fileInfo.i64FileSize;
	m_state = en_STATE_OK;
    return ;
__gt��������:
	if( ( m_������� == 0 ) || ( ( m_������� > 0 ) && (pDtp->i�Ѿ����Դ���++ < m_�������) ) )
		goto __tg����;
	goto __gt�����˳�;
    ///----------------
__gt�û��˳�:
	m_state = en_STATE_FREE;
    return ;
    ///----------------
__gtʧ���˳�:
	m_state = en_STATE_FREE;
    return ;
    ///----------------
__gt�����˳�:
	m_state = en_STATE_FREE;
    return ;
}
DWORD WINAPI CWQSG_Http::zzz_DownloadThread( LPVOID lpParam )
{
	_tgWHTTP_����* ���� = (_tgWHTTP_����*)lpParam;

	����->http->zzz_DownloadThread( ����->url.GetBuffer() , *����->fp , ����->test_sizefile );

	delete ����;
	return 0;
}
BOOL CWQSG_Http::Download( char const*const url , CWQSG_xFile& fp , const bool �첽 , const s64 test_sizefile )
{
	zzz_lock();
	if( ( url == NULL ) || (!fp.IsOpen()) || ( m_state != en_STATE_FREE ) )
	{
		zzz_unlock();
		return FALSE;
	}

	m_state = en_STATE_RUN;

	zzz_unlock();

	m_bStopDownload = m_bPauseDownload = false;
	BOOL rt = FALSE;
	if( �첽 )
	{
		_tgWHTTP_����*const ���� = new _tgWHTTP_����( this , url , &fp , test_sizefile );
		if( ���� != NULL )
		{
			const HANDLE handle = ::CreateThread( NULL , 0 , CWQSG_Http::zzz_DownloadThread , ���� , 0 , NULL );
			if( handle != NULL )
			{
				::CloseHandle( handle );
				rt = TRUE;
			}
			else
			{
				m_state = en_STATE_FREE;
				delete ����;
			}
		}
	}
	else
	{
		zzz_DownloadThread( url , fp , test_sizefile );
		rt = (m_state == en_STATE_OK);
	}
	return rt;
}
BOOL CWQSG_Http::Download( WCHAR const*const url , CWQSG_xFile& fp , const bool �첽 , const s64 test_sizefile )
{
	BOOL rt = FALSE;
	if( NULL != url )
	{
		CStringA urlA = url;
		rt = Download( urlA.GetBuffer() , fp , �첽 , test_sizefile );
	}
	return rt;
}
BOOL CWQSG_Http::GetUrlFileSize( char const*const url , s64& _fileSize , char const*const type )
{
	zzz_lock();
	_fileSize = -1;
	if( NULL != url )
	{
		///����  /////////
		CWQSG_socket_client		cSocket;
		tgHTTP_HeadInfo fileInfo;
		int			i�Ѿ����Դ��� = 0;

		int			iRev;
		CWD_BIN		bufferBin;
		bufferBin.LEN = 1024*8;
		char*const	szBuffer = (char*const)bufferBin.SetBufSize( bufferBin.LEN );
		const int	szBufferByteSize = (int)bufferBin.LEN;
		if( NULL == szBuffer )
			goto __gt�����˳�;

		fileInfo.�ָ���ʼ��();
		//�ֽ��ַ
		if( !fileInfo.ParseURL( url ) )
			goto __gt�����˳�;
		//���� ���� ����
		GetSpeed();
__tg����:
		if( cSocket.Close() != 0 )
			goto __gt�����˳�;
		{
			CStringA	head;
			//��������ͷ
			head = fileInfo.CreateHred( 0 );
			iRev = head.GetLength();
			if( iRev <= 0 )
				goto __gt�����˳�;

			{
				in_addr	addr;
				{
					//��ȡIP tgInfo.strAHostName
					hostent* tghost;
					if( ( tghost = gethostbyname( fileInfo.strAHostName.GetBuffer() ) ) == NULL )
						goto __gt�����˳�;

					memcpy( &addr , tghost->h_addr_list[0] , sizeof(in_addr) );
				}
#ifdef DEBUG
				char* pp = inet_ntoa( addr );
#endif
				if( 0 != cSocket.Connect( inet_ntoa( addr ) , fileInfo.nPort ) )
					goto __gt�����˳�;
			}
			iRev = cSocket.Send( head.GetBuffer() , head.GetLength() , m_dwTimeout );
			if( iRev <= 0 )
				goto __gt��������;
		}

		///����ͷ���Ͳ�������
		char* pBuffer = szBuffer;
		{
			int iOffset;
			CStringA tmpStr;

			memset( szBuffer , 0 , szBufferByteSize );

			iRev = zzz_recv( cSocket , szBuffer , szBufferByteSize );
			if( iRev <= 0 )
				goto __gt�����˳�;

			iOffset = �ֽ��һ�����ݰ�ȡ��ͷ��( szBuffer , tmpStr );
			if( iOffset < 0 )
				goto __gt�����˳�;

			switch( fileInfo.GetHeadInfo( tmpStr ) )
			{
			case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_OK://�ɹ�
				break;
			case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_REDIRECT://�ض���
				goto __tg����;//һ������
				break;
			case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_ERROR://����
				goto __gt��������;
			case tgHTTP_HeadInfo::en_HTTP_RETUEN::enHTTP_FAIL://ʧ��
				goto __gtʧ���˳�;
			default:
				goto __gt�����˳�;
			}
			pBuffer = szBuffer + iOffset;
			iRev -= iOffset;

			if( NULL != type )
			{
				CStringA strType( type );
				strType.MakeLower();
				strType.TrimLeft();
				strType.TrimRight();

				if( fileInfo.strType != strType )
					goto __gt�����˳�;
			}
		}
		switch( fileInfo.m_Transfer_Encoding )
		{
		case en_Transfer_Encoding::en_Transfer_Encoding_chunked:
			fileInfo.i64FileSize = -1;
			break;
		case en_Transfer_Encoding::en_Transfer_Encoding_NULL:
			break;
		default:
			goto __gt�����˳�;
		}
		_fileSize = fileInfo.i64FileSize;
		//���
		zzz_unlock();
		return TRUE;
__gt��������:
	if( ( m_������� == 0 ) || ( ( m_������� > 0 ) && (i�Ѿ����Դ���++ < m_�������) ) )
		goto __tg����;
	goto __gt�����˳�;
	///----------------
__gtʧ���˳�:
	zzz_unlock();
	return FALSE;
	///----------------
	}
__gt�����˳�:
	zzz_unlock();
	return FALSE;
}

//----------------------------------------------------------------------------
void CWQSG_Http::StopDownload()
{
	m_bStopDownload = true;
	while(
		( m_state != en_STATE_OK ) &&
		( m_state != en_STATE_FREE )
		)
		Sleep( 10 );
}
//----------------------------------------------------------------------------
bool CWQSG_Http::PauseDownload()
{
	return ( m_bPauseDownload = !m_bPauseDownload );
}
//----------------------------------------------------------------------------
double CWQSG_Http::Get�ٷֱ�()const
{
	double min = (double)m_fileOffset;
	double max = (double)m_fileSize;
	if( m_fileSize >= 0 )
	{
		double xyz = ( min / max );
		xyz = (xyz*100);
		return xyz;
	}
	return 0;
}
//----------------------------------------------------------------------------
unsigned int CWQSG_Http::GetSpeed()
{
	const clock_t cur_tick = clock();

	const clock_t xxx = ( ( cur_tick < m_last_tick )?(cur_tick - m_last_tick):( cur_tick - m_last_tick ) );
	zzz_lock();
	const unsigned int rt = (xxx==0)?0:((m_totalByte / xxx )*1000);
	m_totalByte = 0;
	zzz_unlock();

	m_last_tick = cur_tick;
	return rt;
}
//----------------------------------------------------------------------------
#if 0
int CWQSG_Http::zzz_SendData( SOCKET hSocket , char const*const pBuffer , const int iSendSize )
{
	int	iSendBytes = 0;
	if( NULL == pBuffer )
		return iSendBytes;
#if 0
	const HANDLE hSendEvent = ::CreateEvent( NULL , FALSE , FALSE , NULL );
	if( NULL == hSendEvent )
		return iSendBytes;

	while( iSendBytes < iSendSize )
	{
		if( SOCKET_ERROR == ::WSAEventSelect( hSocket , hSendEvent , FD_WRITE | FD_CLOSE ) )
			break;
		// ��������
		int iSend;
		if( (iSend = ::send( hSocket , pBuffer + iSendBytes , iSendSize - iSendBytes , 0 ) ) < 0 )
		{
			int iErrorCode = -iSend;
			if ( iErrorCode != WSAEWOULDBLOCK )	//̫���δ����ص�����
				break;
			break;
		}
		iSendBytes += iSend;
		// �ȵ�FD_READ | FD_CLOSE�¼��ķ���
		const DWORD dwWaitResult = ::WSAWaitForMultipleEvents( 1 , &hSendEvent , TRUE , m_dwTimeout , TRUE );
		if( dwWaitResult != WSA_WAIT_EVENT_0 )
			break;

		//////////////////////////////////////////////////////////////
		///	ע�⣺��ʹ dwWaitResult == WSA_WAIT_EVENT0 ��ҲӦ�� 
		///			��һ����������Ƿ�������
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if( SOCKET_ERROR == ::WSAEnumNetworkEvents( hSocket , hSendEvent , &NetEvent ) )
			break;
		// ����¼� //////////////////////////////////////////////////////////////
		::WSAEventSelect( hSocket , hSendEvent , 0 );
		//�жϷ�����ʲô�¼� FD_WRITE �� FD_CLOSE
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( (( NetEvent.lNetworkEvents	== FD_WRITE) && (NetEvent.iErrorCode[FD_WRITE_BIT] !=0) ) )
			)	// ��������
			break;
	}
	::WSAEventSelect( hSocket , hSendEvent , 0 );
	::CloseHandle( hSendEvent );
#else
	while( iSendBytes < iSendSize )
	{
		int x = ::send( hSocket , pBuffer , iSendSize , 0 );
		if( x < 0 )
			break;
		iSendBytes += x;
	}
#endif
	return iSendBytes;
}
//----------------------------------------------------------------------------
int CWQSG_Http::zzz_RecvData( SOCKET hSocket , char*const pBuffer , const int iBufferByteSize )
{
	int	iRecvBytes = 0;
	if( NULL == pBuffer )
		return iRecvBytes;

	const HANDLE hReadEvent = ::CreateEvent( NULL , FALSE , FALSE , NULL );
	if( NULL == hReadEvent )
		return iRecvBytes;

	while( iRecvBytes < iBufferByteSize )
	{
		if( SOCKET_ERROR == ::WSAEventSelect( hSocket , hReadEvent , FD_READ | FD_CLOSE ) )
			break;

		// �ȵ�FD_READ | FD_CLOSE�¼��ķ���
		const DWORD dwWaitResult = ::WSAWaitForMultipleEvents( 1 , &hReadEvent , TRUE , m_dwTimeout , TRUE );
		if( dwWaitResult != WSA_WAIT_EVENT_0 )
			break;

		//////////////////////////////////////////////////////////////
		///	ע�⣺��ʹ dwWaitResult == WSA_WAIT_EVENT0 ��ҲӦ�� 
		///			��һ����������Ƿ�������
		///////////////////////////////////////////////////////////////
		WSANETWORKEVENTS NetEvent;
		if( SOCKET_ERROR == ::WSAEnumNetworkEvents( hSocket , hReadEvent , &NetEvent ) )
			break;
		// ����¼� //////////////////////////////////////////////////////////////
		::WSAEventSelect( hSocket , hReadEvent , 0 );
		//�жϷ�����ʲô�¼� FD_READ �� FD_CLOSE
		if( ( NetEvent.lNetworkEvents == FD_CLOSE ) ||
			( (( NetEvent.lNetworkEvents	== FD_READ) && (NetEvent.iErrorCode[FD_READ_BIT] !=0) ) )
			)	// ��������
			break;
		// ��������
		int iRecvSize;
		if( (iRecvSize = ::recv( hSocket , pBuffer + iRecvBytes , iBufferByteSize - iRecvBytes , 0 ) ) < 0 )
		{
			int iErrorCode = -iRecvBytes;
			if ( iErrorCode != WSAEWOULDBLOCK )	//̫���δ����ص�����
				break;
			break;
		}
		iRecvBytes += iRecvSize;
		if( iRecvBytes > 0 )
			break;
	}
	::WSAEventSelect( hSocket , hReadEvent , 0 );
	::CloseHandle( hReadEvent );

	return iRecvBytes;
}
#endif

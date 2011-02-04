
#ifndef __WQSG_FONTSYSTEM_H__
#define __WQSG_FONTSYSTEM_H__
#include "wqsg_def.h"
#include "wqsg_font.h"
#include <new>

class CFontSystem : public CSingletonT<CFontSystem>
{
	friend class CSingletonT<CFontSystem>;
	struct CNode
	{
		char m_V[sizeof(CFont)];
		CNode* m_pP;
		CNode* m_pN;
	};
	struct SFonts_t
	{
		int sysFontMax;
		CellFont SystemFont[ SYSTEM_FONT_MAX ];

		int userFontMax;
		CellFont UserFont[ USER_FONT_MAX ];

		uint32_t openState;
	};

	SFonts_t* m_pFonts;
	const CellFontLibrary* m_pFreeType;
	CellFontRenderer m_Renderer;
	CNode* m_pFontList;
protected:
	static void* fonts_malloc( void*obj, uint32_t size )
	{
		obj = NULL;
		return malloc( size );
	}
	static void  fonts_free( void*obj, void*p )
	{
		obj = NULL;
		free( p );
	}
	static void* fonts_realloc( void*obj, void* p, uint32_t size )
	{
		obj = NULL;
		return realloc( p, size );
	}
	static void* fonts_calloc( void*obj, uint32_t numb, uint32_t blockSize )
	{
		obj = NULL;
		return calloc( numb, blockSize );
	}

	static void* loadFile( const uint8_t* fname, size_t *size, int offset, int addSize )
	{
		FILE* fp;
		size_t file_size;
		void* p;

		fp = fopen( (const char*)fname, "rb" );
		if (! fp )
		{
			printf("cannot open %s\n", fname );
			if ( size )
				*size = 0;
			return NULL;
		}

		fseek( fp, 0, SEEK_END );
		file_size = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		if ( size ) *size = file_size;

		p = malloc( file_size + offset + addSize );
		if ( p )
			fread( (unsigned char*)p+offset, file_size, 1, fp );

		fclose( fp );

		return p;
	}

	inline int Fonts_Init();
	//J libfont ライブラリ終了
	inline int Fonts_End();
	//J フォントインターフェースライブラリ初期化 ( FreeType2を使用します。)
	inline int Fonts_InitLibraryFreeType();
	//J フォントインターフェースライブラリ 終了
	inline int Fonts_EndLibrary();
	//J フォントのオープン
	inline int Fonts_OpenFonts();
	inline int Fonts_CloseFonts();
	//J libfontライブラリのロード
	inline int Fonts_LoadModules()
	{
		int ret;

		//J libfont モジュールのロード
		ret = cellSysmoduleLoadModule( CELL_SYSMODULE_FONT );
		if ( ret == CELL_OK ) {
			//J libfreetype モジュールのロード
			ret = cellSysmoduleLoadModule( CELL_SYSMODULE_FREETYPE );
			if ( ret == CELL_OK ) {
				//J libfontFT モジュールのロード
				ret = cellSysmoduleLoadModule( CELL_SYSMODULE_FONTFT );
				if ( ret == CELL_OK ) {
					return ret;
				}
				else printf("Fonts: 'CELL_SYSMODULE_FONTFT' NG! %08x\n",ret);
				//J 以下、ロードエラー時のアンロード処理
				cellSysmoduleUnloadModule( CELL_SYSMODULE_FREETYPE );
			}
			else printf("Fonts: 'CELL_SYSMODULE_FREETYPE' NG! %08x\n",ret);

			cellSysmoduleUnloadModule( CELL_SYSMODULE_FONT );
		}
		else printf("Fonts: 'CELL_SYSMODULE_FONT' NG! %08x\n",ret);

		return ret;
	}
	//J レンダラー初期化。
	inline int CreateRenderer( uint32_t initSize );
	//J レンダラを破棄
	inline int DestroyRenderer();
	//J 指定したフォント取得。
	inline int Fonts_AttachFont( int fontEnum , CellFont*ins );
	//J 使用し終わったフォントの返却
	inline int Fonts_DetachFont( CellFont*ins );
	inline int Fonts_BindRenderer( CellFont*ins );
	inline int Fonts_UnbindRenderer( CellFont*ins );
private:
	CFontSystem()
		: m_pFonts(0)
		, m_pFontList(0)
	{
	}

	~CFontSystem()
	{
		Uninit();
	}

	CNode* Quick_DeleteFont( CNode* a_pNode )
	{
		CNode* pNext = a_pNode->m_pN;

		if( a_pNode->m_pP )
			a_pNode->m_pP->m_pN = pNext;
		if( pNext )
			pNext->m_pP = a_pNode->m_pP;

		CFont* pFont = (CFont*)a_pNode->m_V;
		pFont->End();

		Fonts_UnbindRenderer( &pFont->m_Font );
		Fonts_DetachFont( &pFont->m_Font );
		pFont->~CFont();
		delete a_pNode;

		return pNext;
	}
public:
	static CFontSystem& I()
	{
		static CFontSystem I;
		return I;
	}

	int Init()
	{
		Fonts_LoadModules();

		int ret = Fonts_Init();
		if( ret == CELL_OK )
		{
			ret = Fonts_InitLibraryFreeType();
			if( ret == CELL_OK )
			{
				ret = Fonts_OpenFonts();
				if( ret == CELL_OK )
				{
					ret = CreateRenderer( 0 );
					if ( ret == CELL_OK )
					{
						//J フォント初期化 正常終了!
						printf("App:Fonts Initialize All OK!\n");

						return CELL_OK;
					}
					//J 以下エラー処置
					//J フォントクローズ
					Fonts_CloseFonts();
				}
				else
				{
					Fonts_PrintError( "    Fonts_OpenFonts=", ret );
				}
				Fonts_EndLibrary();
			}
			else
			{
				Fonts_PrintError( "    Fonts_InitLibraryFreeType=", ret );
			}
			Fonts_End();
		}
		else
		{
			Fonts_PrintError( "    Fonts_Init=", ret );
		}

		return ret;
	}

	void Uninit()
	{
		if( m_pFonts )
		{
			for( ; m_pFontList ; )
				m_pFontList = Quick_DeleteFont( m_pFontList );

			Fonts_CloseFonts();
			DestroyRenderer();
			Fonts_EndLibrary();
			Fonts_End();

// 			cellSysmoduleUnloadModule( CELL_SYSMODULE_FONTFT );
// 			cellSysmoduleUnloadModule( CELL_SYSMODULE_FREETYPE );
// 			cellSysmoduleUnloadModule( CELL_SYSMODULE_FONT );
		}
	}
	//===============================================
	int CreateFont( CFont** a_Obj , int x )
	{
		CNode* pNode = new CNode;
		pNode->m_pN = pNode->m_pP = NULL;

		CFont* pFont = new((void*)pNode->m_V) CFont;

		//J ビットマップベースのフォント用にフォントを割り当て
		int ret = Fonts_AttachFont( FONT_SYSTEM_GOTHIC_LATIN , &pFont->m_Font );
		if( ret == CELL_OK )
		{
			ret = Fonts_BindRenderer( &pFont->m_Font );
			if( ret == CELL_OK )
			{
				//J ビットマップフォント初期化
				ret = pFont->Init( x , x , 1.0f , 0.1f , 256 );
				if( ret == CELL_OK )
				{
					//pNode->m_V.SetFontScale( 48 );
					if( a_Obj )
						*a_Obj = pFont;

					if( m_pFontList )
					{
						m_pFontList->m_pP = pNode;
						pNode->m_pN = m_pFontList;
					}
					m_pFontList = pNode;
					return ret;
				}
				else
				{
					Fonts_UnbindRenderer( &pFont->m_Font );
				}
				//pFont->End();
			}
			else
			{
				Fonts_DetachFont( &pFont->m_Font );
			}
		}

		pFont->~CFont();
		delete pNode;
		return ret;
	}
	void DeleteFont( CFont* a_Obj )
	{
		if( !a_Obj )
			return;

		CNode* pN = m_pFontList;
		while( pN )
		{
			if( (char*)a_Obj == pN->m_V )
			{
				Quick_DeleteFont( pN );
				break;
			}

			pN = pN->m_pN;
		}
	}
};
inline int CFontSystem::Fonts_Init()
{
	//J libfontに必要なリソース
	static CellFontEntry UserFontEntrys[USER_FONT_MAX];
	static uint32_t FontFileCache[FONT_FILE_CACHE_SIZE/sizeof(uint32_t)];

	CellFontConfig config;
	int ret;

	//J libfont 初期化パラメータ設定。
	//----------------------------------------------------------------------
	CellFontConfig_initialize( &config );

	//J ファイルキャッシュバッファを設定
	config.FileCache.buffer = FontFileCache;
	config.FileCache.size   = FONT_FILE_CACHE_SIZE;

	//J ユーザーフォント用、エントリバッファ設定。
	config.userFontEntrys   = UserFontEntrys;
	config.userFontEntryMax = sizeof(UserFontEntrys)/sizeof(CellFontEntry);

	//J フラグは現在 0 固定
	config.flags = 0;

	//J libfontライブラリ 初期化
	//----------------------------------------------------------------------
	ret = cellFontInit( &config );
	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "   Fonts:cellFontInit=", ret );
		return ret;
	}

	static SFonts_t Fonts;
	m_pFonts = &Fonts;
	return CELL_OK;
}
//J libfont ライブラリ終了
inline int CFontSystem::Fonts_End()
{
	int ret = cellFontEnd();

	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "    Fonts:cellFontEnd=", ret );
	}
	return ret;
}
//J フォントインターフェースライブラリ初期化 ( FreeType2を使用します。)
inline int CFontSystem::Fonts_InitLibraryFreeType()
{
	CellFontLibraryConfigFT config;
	//J フォントインターフェースライブラリ初期化パラメータの初期化
	CellFontLibraryConfigFT_initialize( &config );
	//J メモリーインターフェースの設定( 設定必須 )
	config.MemoryIF.Object  = NULL;
	config.MemoryIF.Malloc  = fonts_malloc;
	config.MemoryIF.Free    = fonts_free;
	config.MemoryIF.Realloc = fonts_realloc;
	config.MemoryIF.Calloc  = fonts_calloc;

	//J フォントインターフェースライブラリ初期化
	int ret = cellFontInitLibraryFreeType( &config , &m_pFreeType );
	if ( ret != CELL_OK )
		Fonts_PrintError( "   Fonts:cellFontInitLibrary_FreeType=", ret );

	return ret;
}
//J フォントインターフェースライブラリ 終了
inline int CFontSystem::Fonts_EndLibrary()
{
	if( m_pFreeType )
	{
		int ret = cellFontEndLibrary( m_pFreeType );
		if ( ret != CELL_OK )
			Fonts_PrintError( "    Fonts:cellFontEndLibrary=", ret );

		m_pFreeType = NULL;
		return ret;
	}
	return CELL_OK;
}
//J フォントのオープン
inline int CFontSystem::Fonts_OpenFonts()
{
	int ret = CELL_OK;

	//J システム搭載フォントセットのオープン例
	{
		static struct {
			uint32_t isMemory;
			int      fontsetType;
		} openSystemFont[ SYSTEM_FONT_MAX ] = {
			{ 1, CELL_FONT_TYPE_GOTHIC_SCHINESE_CJK_JP_SET },
			{ 0, CELL_FONT_TYPE_DEFAULT_GOTHIC_LATIN_SET },
			{ 0, CELL_FONT_TYPE_DEFAULT_GOTHIC_JP_SET    },
			{ 1, CELL_FONT_TYPE_DEFAULT_SANS_SERIF       },
			{ 1, CELL_FONT_TYPE_DEFAULT_SERIF            },
		};
		CellFontType type;

		m_pFonts->sysFontMax = 4;
		//J オンメモリでオープンする
		for ( int n=0; n < m_pFonts->sysFontMax; n++ )
		{
			if ( !openSystemFont[n].isMemory )
				continue;

			type.type = openSystemFont[n].fontsetType;
			type.map  = CELL_FONT_MAP_UNICODE;
			//J システム搭載フォントセットオープン
			ret = cellFontOpenFontsetOnMemory( m_pFreeType , &type , &m_pFonts->SystemFont[n] );
			if ( ret != CELL_OK )
			{
				Fonts_PrintError( "   Fonts:cellFontOpenFontset=", ret );
				Fonts_CloseFonts(); //J オープンした分、クローズして戻る。
				return ret;
			}
			m_pFonts->openState |= (1<<n);
			//J 初期スケール設定
#if 1
			//J ポイントで指定
			cellFontSetResolutionDpi( &m_pFonts->SystemFont[n], 72, 72 );
			cellFontSetScalePoint( &m_pFonts->SystemFont[n], 26.f, 26.f );
#else
			//J ピクセルで指定
			cellFontSetScalePixel( &m_pFonts->SystemFont[n], 26.f, 26.f );
#endif
		}
		//J ファイルアクセスオープンする
		for ( int n=0; n < m_pFonts->sysFontMax; n++ )
		{
			if ( openSystemFont[n].isMemory )
				continue;

			type.type = openSystemFont[n].fontsetType;
			type.map  = CELL_FONT_MAP_UNICODE;
			//J システム搭載フォントセットオープン
			ret = cellFontOpenFontset( m_pFreeType , &type, &m_pFonts->SystemFont[n] );
			if ( ret != CELL_OK )
			{
				Fonts_PrintError( "   Fonts:cellFontOpenFontset=", ret );
				Fonts_CloseFonts(); //J オープンした分、クローズして戻る。
				return ret;
			}
			m_pFonts->openState |= (1<<n);
			//J 初期スケール設定
#if 1
			//J ポイントで指定
			cellFontSetResolutionDpi( &m_pFonts->SystemFont[n] , 72 , 72 );
			cellFontSetScalePoint( &m_pFonts->SystemFont[n] , 26.f , 26.f );
#else
			//J ピクセルで指定
			cellFontSetScalePixel( &m_pFonts->SystemFont[n], 26.f, 26.f );
#endif
		}
	}

	//J アプリケーションで用意したフォントのオープン例
	{
		static struct {
			uint32_t isMemory;
			const char* filePath;
		} userFont[ USER_FONT_MAX ] = {
			{ 0, SYS_APP_HOME"/app.ttf" }, //J app.ttf は、用意されていません。
		};
		uint32_t fontUniqueId = 0;

		m_pFonts->userFontMax = 0;            //J app.ttf は、用意されていません。

		for ( int n=0; n < m_pFonts->userFontMax; n++ )
		{
			uint8_t* path = (uint8_t*)userFont[n].filePath;

			if (! userFont[n].isMemory )
			{
				//J ファイルアクセスでオープン
				ret = cellFontOpenFontFile( m_pFreeType , path, 0, fontUniqueId, &m_pFonts->UserFont[n] );
				if ( ret != CELL_OK )
				{
					Fonts_PrintError( "    Fonts:cellFontOpenFile=", ret );
					Fonts_CloseFonts();
					printf("    Fonts:   [%s]\n", userFont[n].filePath);
					return ret;
				}
			}
			else
			{
				//J メモリーアクセスでオープン
				size_t size;
				void *p = loadFile( path, &size, 0, 0 );

				ret = cellFontOpenFontMemory( m_pFreeType , p, size, 0, fontUniqueId, &m_pFonts->UserFont[n] );
				if ( ret != CELL_OK )
				{
					if (p) free( p );
					Fonts_PrintError( "    Fonts:cellFontMemory=", ret );
					Fonts_CloseFonts();
					printf("    Fonts:   [%s]\n", userFont[n].filePath);
					return ret;
				}
			}
			m_pFonts->openState |= (1<<(FONT_USER_FONT0+n));
			fontUniqueId++;

			if ( ret == CELL_OK )
			{
				//J 初期スケール設定
#if 1
				//J ポイントで指定
				cellFontSetResolutionDpi( &m_pFonts->UserFont[n], 72, 72 );
				cellFontSetScalePoint( &m_pFonts->UserFont[n], 26.f, 26.f );
#else
				//J ピクセルで指定
				cellFontSetScalePixel( &m_pFonts->UserFont[n], 26.f, 26.f );
#endif
			}
		}
	}

	return ret;
}
inline int CFontSystem::Fonts_CloseFonts()
{
	int n;
	int ret, err = CELL_FONT_OK;

	if ( !m_pFonts )
		return err;

	//J システム搭載フォントセットのクローズ
	for ( n=0; n < m_pFonts->sysFontMax; n++ )
	{
		uint32_t checkBit = (1<<n);

		if ( m_pFonts->openState & checkBit )
		{
			ret = cellFontCloseFont( &m_pFonts->SystemFont[n] );
			if ( ret != CELL_OK )
			{
				Fonts_PrintError( "    Fonts.SystemFont:cellFontCloseFont=", ret );
				err = ret;
				continue;
			}
			m_pFonts->openState &= (~checkBit);
		}
	}
	//J ユーザーフォントのクローズ
	for ( n=0; n < m_pFonts->userFontMax; n++ )
	{
		uint32_t checkBit = (1<<(FONT_USER_FONT0+n));

		if ( m_pFonts->openState & checkBit )
		{
			ret = cellFontCloseFont( &m_pFonts->UserFont[n] );
			if ( ret != CELL_OK )
			{
				Fonts_PrintError( "    Fonts.UserFont:cellFontCloseFont=", ret );
				err = ret;
				continue;
			}
			m_pFonts->openState &= (~checkBit);
		}
	}

	return err;
}
inline int CFontSystem::CreateRenderer( uint32_t initSize )
{
	CellFontRendererConfig config;

	CellFontRendererConfig_initialize( &config );
	CellFontRendererConfig_setAllocateBuffer( &config , initSize , 0 );

	//J レンダラの作成
	int ret = cellFontCreateRenderer( m_pFreeType , &config , &m_Renderer );
	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "   Fonts:cellFontCreateRenderer=" , ret );
		return ret;
	}

	return ret;
}
//J レンダラを破棄
inline int CFontSystem::DestroyRenderer()
{
	int ret = cellFontDestroyRenderer( &m_Renderer );

	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "    Fonts:cellFontDestroyRenderer=", ret );
	}
	return ret;
}

inline int CFontSystem::Fonts_AttachFont( int fontEnum , CellFont*ins )
{
	CellFont* openedFont = (CellFont*)0;
	if ( m_pFonts )
	{
		if ( fontEnum < FONT_USER_FONT0 )
		{
			uint32_t n = fontEnum;

			if ( n < (uint32_t)m_pFonts->sysFontMax )
			{
				if ( m_pFonts->openState & (1<<fontEnum) )
				{
					openedFont = &m_pFonts->SystemFont[ n ];
				}
			}
		}
		else
		{
			uint32_t n = fontEnum - FONT_USER_FONT0;

			if ( n < (uint32_t)m_pFonts->userFontMax )
			{
				if ( m_pFonts->openState & (1<<fontEnum) )
				{
					openedFont = &m_pFonts->UserFont[ n ];
				}
			}
		}
	}
	//J すでにオープン済みのフォントのインスタンスを生成。
	int ret = cellFontOpenFontInstance( openedFont , ins );
	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "    Fonts:AttachFont:cellFontOpenFontInstance=", ret );
	}
	return ret;
}

//J 使用し終わったフォントの返却
inline int CFontSystem::Fonts_DetachFont( CellFont*ins )
{
	int ret = cellFontCloseFont( ins );
	if ( ret != CELL_OK )
		Fonts_PrintError( "    Fonts:cellFontCloseFont=", ret );

	return ret;
}

inline int CFontSystem::Fonts_BindRenderer( CellFont*ins )
{
	//J レンダラー接続
	int ret = cellFontBindRenderer( ins , &m_Renderer );
	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "    Fonts:AttachFont:cellFontBindRenderer=", ret );
	}
	return ret;
}
inline int CFontSystem::Fonts_UnbindRenderer( CellFont*ins ) 
{
	//J レンダラー接続解除。
	int ret = cellFontUnbindRenderer( ins );
	if ( ret != CELL_OK )
	{
		Fonts_PrintError( "    Fonts:UnbindRenderer:cellFontUnbindRenderer=", ret );
	}
	return ret;
}

#endif //__WQSG_FONTSYSTEM_H__

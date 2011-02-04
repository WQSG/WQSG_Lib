#ifndef __WQSG_FONT_H__
#define __WQSG_FONT_H__
inline char* GetStrBuf()
{
	static char buff[1024] = "NoError";
	return buff;
}

#define FontLog( fmt , ... ) sprintf( GetStrBuf() , fmt , __VA_ARGS__ )

#if 0
//J UTF-8 文字列から、UCS4形式で文字列取り出し
inline uint32_t getUcs4( const uint8_t*a_utf8, uint32_t*a_ucs4, uint32_t a_alterCode )
{
	uint64_t code = 0L;
	uint32_t len = 0;

	code = (uint64_t)*a_utf8;

	if ( code )
	{
		a_utf8++;
		len++;
		if ( code >= 0x80 )
		{
			while (1)
			{
				//J UTF-8 先頭コードチェック。
				if ( code & 0x40 )
				{ 
					uint64_t mask = 0x20L;
					uint64_t encode;
					uint64_t n;

					for ( n=2;;n++ )
					{
						if ( (code & mask) == 0 )
						{
							len = n;
							mask--;
							if ( mask == 0 )
							{ // 0xFE or 0xFF 
								//J 先頭コードエラー
								*a_ucs4 = 0x00000000;
								return 0;
							}
							break;
						}
						mask = (mask >> 1);
					}
					code &= mask;

					for ( n=1; n<len; n++ )
					{
						encode = (uint64_t)*a_utf8;
						if ( (encode & 0xc0) != 0x80 )
						{
							//J 文字コードが途中で切れている！
							if ( a_ucs4 ) *a_ucs4 = a_alterCode;
							return n;
						}
						code = ( ( code << 6 ) | (encode & 0x3f) );
						a_utf8++;
					}
					break;
				}
				else
				{ //J 先頭コードエラー
					//J UTF-8の文字列であるならば文字コードの途中と判断。
					//J 次の文字までスキップ。
					for( ;; a_utf8++ )
					{
						code = (uint64_t)*a_utf8;
						if ( code < 0x80 ) break;
						if ( code & 0x40 ) break;
						len++;
					}
					if ( code < 0x80 ) break;
				}
			}
		}
	}
	if ( a_ucs4 )
		*a_ucs4 = (uint32_t)code;

	return len;
}

#else
inline uint32_t getUcs4( const uint8_t*a_utf8, uint32_t*a_ucs4 , uint32_t a_alterCode )
{
	uint32_t utf32 = *a_utf8;
	int len = 0;

	if( utf32 )
	{
		if( (utf32 & 0x80) == 0 )
		{
			len = 1;
		}
		else if( (utf32 & 0xE0) == 0xC0 )
		{
			len = 2;
			utf32 &= ~0xC0;
		}
		else if( (utf32 & 0xF0) == 0xE0 )
		{
			len = 3;
			utf32 &= ~0xE0;
		}
		else if( (utf32 & 0xF8) == 0xF0 )
		{
			len = 4;
			utf32 &= ~0xF0;
		}
		else if( (utf32 & 0xFC) == 0xF8 )
		{
			len = 5;
			utf32 &= ~0xF8;
		}
		else if( (utf32 & 0xFE) == 0xFC )
		{
			len = 6;
			utf32 &= ~0xFC;
		}
		else
		{
			len = 1;
			utf32 = a_alterCode;
		}

		for( int i = 1 ; i < len ; ++i )
		{
			const uint8_t bit = a_utf8[i];
			const uint8_t flag = a_utf8[i] & 0xC0;
			if( flag != 0x80 )
			{
				len = 1;
				utf32 = a_alterCode;
				break;
			}

			utf32 <<= 6;
			utf32 |= (bit & 0x3F);
		}
	}

	if( a_ucs4 )
		*a_ucs4 = utf32;

	return len;
}
#endif
inline void Fonts_PrintError( const char*mess, int d )
{
	const char* s;
	switch( d ) {
	case CELL_FONT_OK                              : s="CELL_FONT_OK";                               break;
	case CELL_FONT_ERROR_FATAL                     : s="CELL_FONT_ERROR_FATAL";                      break;
	case CELL_FONT_ERROR_INVALID_PARAMETER         : s="CELL_FONT_ERROR_INVALID_PARAMETER";          break;
	case CELL_FONT_ERROR_UNINITIALIZED             : s="CELL_FONT_ERROR_UNINITIALIZED";              break;
	case CELL_FONT_ERROR_INITIALIZE_FAILED         : s="CELL_FONT_ERROR_INITIALIZE_FAILED";          break;
	case CELL_FONT_ERROR_INVALID_CACHE_BUFFER      : s="CELL_FONT_ERROR_INVALID_CACHE_BUFFER";       break;
	case CELL_FONT_ERROR_ALREADY_INITIALIZED       : s="CELL_FONT_ERROR_ALREADY_INITIALIZED";        break;
	case CELL_FONT_ERROR_ALLOCATION_FAILED         : s="CELL_FONT_ERROR_ALLOCATION_FAILED";          break;
	case CELL_FONT_ERROR_NO_SUPPORT_FONTSET        : s="CELL_FONT_ERROR_NO_SUPPORT_FONTSET";         break;
	case CELL_FONT_ERROR_OPEN_FAILED               : s="CELL_FONT_ERROR_OPEN_FAILED";                break;
	case CELL_FONT_ERROR_READ_FAILED               : s="CELL_FONT_ERROR_READ_FAILED";                break;
	case CELL_FONT_ERROR_FONT_OPEN_FAILED          : s="CELL_FONT_ERROR_FONT_OPEN_FAILED";           break;
	case CELL_FONT_ERROR_FONT_NOT_FOUND            : s="CELL_FONT_ERROR_FONT_NOT_FOUND";             break;
	case CELL_FONT_ERROR_FONT_OPEN_MAX             : s="CELL_FONT_ERROR_FONT_OPEN_MAX";              break;
	case CELL_FONT_ERROR_FONT_CLOSE_FAILED         : s="CELL_FONT_ERROR_FONT_CLOSE_FAILED";          break;
	case CELL_FONT_ERROR_NO_SUPPORT_FUNCTION       : s="CELL_FONT_ERROR_NO_SUPPORT_FUNCTION";        break;
	case CELL_FONT_ERROR_NO_SUPPORT_CODE           : s="CELL_FONT_ERROR_NO_SUPPORT_CODE";            break;
	case CELL_FONT_ERROR_NO_SUPPORT_GLYPH          : s="CELL_FONT_ERROR_NO_SUPPORT_GLYPH";           break;
	case CELL_FONT_ERROR_RENDERER_ALREADY_BIND     : s="CELL_FONT_ERROR_RENDERER_ALREADY_BIND";      break;
	case CELL_FONT_ERROR_RENDERER_UNBIND           : s="CELL_FONT_ERROR_RENDERER_UNBIND";            break;
	case CELL_FONT_ERROR_RENDERER_INVALID          : s="CELL_FONT_ERROR_RENDERER_INVALID";           break;
	case CELL_FONT_ERROR_RENDERER_ALLOCATION_FAILED: s="CELL_FONT_ERROR_RENDERER_ALLOCATION_FAILED"; break;
	case CELL_FONT_ERROR_ENOUGH_RENDERING_BUFFER   : s="CELL_FONT_ERROR_ENOUGH_RENDERING_BUFFER";    break;
	default:s="unknown!";
	}
	if (!mess) mess="";
	FontLog("%s%s\n",mess,s);
}

#define FONT_FILE_CACHE_SIZE (1*1024*1024) //1MB

#define SYSTEM_FONT_MAX (16)
#define USER_FONT_MAX   (32-SYSTEM_FONT_MAX)

enum FontEnum
{
	FONT_SYSTEM_FONT0 = 0,
	FONT_SYSTEM_GOTHIC_LATIN = FONT_SYSTEM_FONT0,
	FONT_SYSTEM_GOTHIC_JP,
	FONT_SYSTEM_SANS_SERIF,
	FONT_SYSTEM_SERIF,
	FONT_USER_FONT0    = SYSTEM_FONT_MAX
};

struct SFontBitmapCharGlyph_t
{
	uint32_t code;
	CellFontGlyphMetrics Metrics;
	uint16_t w, h;
	int16_t x0, y0;
	uint8_t *Image;
};

class CFont
{
	friend class CFontSystem;

	struct SFontBitmapCharGlyphCache
	{
		int type;
		uint32_t count;
		SFontBitmapCharGlyph_t* BitmapGlyph;
		CellFont* font;
		float wf, hf;
		float weight, slant;
		uint32_t cacheN;
	} ;

	struct STransData
	{
		CellFontRenderSurface* m_pSurf;
		float m_fX;
		float m_fY;
	};
	//J 文字イメージオブジェクト生成
	inline int FontBitmapCharGlyph_Generate( SFontBitmapCharGlyph_t *glyph , int count , const SFontBitmapCharGlyphCache& a_Val );
	//J 文字イメージキャッシュのから、文字イメージ取得
	inline const SFontBitmapCharGlyph_t* Cache_GetGlyph( uint32_t code );
	//J 文字イメージオブジェクトの取得（半角文字専用)
	inline const SFontBitmapCharGlyph_t* Ascii_GetGlyph( uint32_t code );
	//J 文字イメージオブジェクトの破棄
	inline void Glyph_Delete( SFontBitmapCharGlyph_t* a_pBitmapGlyph , int a_count );
	//J 文字イメージキャッシュの作成
	inline int Cache_Init( int count, float wf, float hf, float weight, float slant );
	//J 文字イメージオブジェクトの半角文字作り置き
	inline int Ascii_Init( float wf, float hf, float weight, float slant );
	//J 文字イメージを破棄（半角文字）
	inline void Ascii_End();
	//J 文字イメージオブジェクトからテクスチャへ転送
	//J イメージのクオリティは、サブピクセル分のシフトがあるため元イメージより劣化します。
	inline int CharGlyph_Trans_blendCast_ARGB8( const STransData& a_Data , const SFontBitmapCharGlyph_t *glyph );
	CellFont m_Font;
	CellFontHorizontalLayout   m_HorizontalLayout;
	CellFontVerticalLayout     m_VerticalLayout;
	SFontBitmapCharGlyphCache m_Ascii;
	SFontBitmapCharGlyphCache m_Cache;
	uint32_t m_uColor;

	inline CFont()
	{
		m_uColor = 0xFFFFFFFF;
		memset( &m_Font , 0 , sizeof(m_Font) );
		memset( &m_HorizontalLayout , 0 , sizeof(m_HorizontalLayout) );
		memset( &m_VerticalLayout , 0 , sizeof(m_VerticalLayout) );
		memset( &m_Ascii , 0 , sizeof(m_Ascii) );
		memset( &m_Cache , 0 , sizeof(m_Cache) );
	}

	~CFont()
	{
	}
	//J 文字イメージフォントの初期化
	inline int Init( float a_wf, float a_hf, float a_weight, float a_slant, int a_cacheMax );
	//J 文字イメージフォントの終了
	inline void End();
public:
	//J 文字イメージフォントの横書きレイアウト取得
	inline const CellFontHorizontalLayout& GetHorizontalLayout()const;
	inline uint8_t GetR()const;
	inline uint8_t GetG()const;
	inline uint8_t GetB()const;
	//J 文字イメージフォントの文字イメージ取得
	inline const SFontBitmapCharGlyph_t* GetGlyph( uint32_t code );
	//J 文字イメージフォントによる文字列レンダリング
	inline float RenderPropText( CellFontRenderSurface* a_pSurf , float a_fX , float a_fY , const uint8_t* utf8 , float a_fBetween = 0.0f );
private:
	//J フォントのスケール設定(ピクセル指定。縦横比１：１)
	inline int SetFontScale( float scale )
	{
		int ret;

		ret = cellFontSetScalePixel( &m_Font , scale , scale );
		if ( ret != CELL_OK )
			Fonts_PrintError( "    Fonts:cellFontSetScalePixel=", ret );

		return ret;
	}
public:
	//J フォントの太さの調整値設定
	inline int SetFontEffectWeight( float effWeight )
	{
		int ret;

		ret = cellFontSetEffectWeight( &m_Font , effWeight );
		if ( ret != CELL_OK ) {
			Fonts_PrintError( "    Fonts:cellFontSetEffectWeight=", ret );
		}
		return ret;
	}
	//J フォントに与えるを傾きを設定
	inline int SetFontEffectSlant( float effSlant )
	{
		int ret;

		ret = cellFontSetEffectSlant( &m_Font , effSlant );
		if ( ret != CELL_OK ) {
			Fonts_PrintError( "    Fonts:cellFontSetEffectSlant=", ret );
		}
		return ret;
	}

} ;
#include "wqsg_font_inline.h"

#endif //__WQSG_FONT_H__

#ifndef __WQSG_FONT_INLINE_H__
#define __WQSG_FONT_INLINE_H__

inline int CFont::FontBitmapCharGlyph_Generate( SFontBitmapCharGlyph_t *glyph , int count , const SFontBitmapCharGlyphCache& a_Val )
{
	CellFontRenderSurface    Surface;
	CellFontGlyphMetrics     metrics;
	CellFontImageTransInfo   TransInfo;
	int x,y;
	int w,h;
	int ret;

	if ( CELL_OK != (ret = cellFontSetupRenderScalePixel( &m_Font, a_Val.wf, a_Val.hf ) ) )
	{
		Fonts_PrintError( "cellFontSetupRenderScalePixel " , ret );
		return ret;
	}
	if ( CELL_OK != (ret = cellFontSetupRenderEffectWeight( &m_Font, a_Val.weight ) ) )
	{
		Fonts_PrintError( "cellFontSetupRenderEffectWeight " , ret );
		return ret;
	}
	if ( CELL_OK != (ret = cellFontSetupRenderEffectSlant( &m_Font, a_Val.slant ) ) )
	{
		Fonts_PrintError( "cellFontSetupRenderEffectSlant " , ret );
		return ret;
	}

	x = ((int)a_Val.wf) * 2;
	y = ((int)a_Val.hf) * 2;
	w = x*2;
	h = y*2;

	cellFontRenderSurfaceInit( &Surface , (void*)0 , w , 1 , w , h );
	cellFontRenderSurfaceSetScissor( &Surface , 0 , 0 , w , h );

	for ( ; count; count-- )
	{
		ret = cellFontRenderCharGlyphImage( &m_Font , glyph->code, &Surface, (float)x, (float)y, &metrics, &TransInfo );
		if ( ret == CELL_OK )
		{
			glyph->w = TransInfo.imageWidth;
			glyph->h = TransInfo.imageHeight;

			const size_t size = glyph->w * glyph->h;

			glyph->Image = size?(uint8_t *)malloc( size ):(uint8_t*)0;

			if ( glyph->Image )
			{
				for ( int iy = 0; iy < glyph->h; iy++ )
				{
					memcpy( glyph->Image + iy*glyph->w , TransInfo.Image + iy*TransInfo.imageWidthByte , glyph->w );
// 							for ( int ix = 0; ix < glyph->w; ix++ )
// 								glyph->Image[iy*glyph->w+ix] = TransInfo.Image[ iy*TransInfo.imageWidthByte+ix ];
				}
			}

			glyph->x0 = (int)TransInfo.Surface % TransInfo.surfWidthByte - x;
			glyph->y0 = (int)TransInfo.Surface / TransInfo.surfWidthByte - y;

			glyph->Metrics = metrics;
		}
		else
		{
			Fonts_PrintError( "cellFontRenderCharGlyphImage " , ret );
			glyph->Image = (uint8_t*)0;
		}
		glyph++;
	}
	return CELL_OK;
}
//J 文字イメージキャッシュのから、文字イメージ取得
inline const SFontBitmapCharGlyph_t* CFont::Cache_GetGlyph( uint32_t code )
{
	SFontBitmapCharGlyph_t* codeGlyph;
	uint32_t n;
	int ret;

	if ( code == 0x0000 )
		return (SFontBitmapCharGlyph_t*)0;

	for ( n = 0; n < m_Cache.count; n++ )
	{
		codeGlyph = &m_Cache.BitmapGlyph[ n ];

		if ( codeGlyph->code == code )
			return codeGlyph;
	}

	for ( n = 0; n < m_Cache.count; n++ )
	{
		if ( ! m_Cache.BitmapGlyph[ n ].Image )
		{
			codeGlyph = &m_Cache.BitmapGlyph[ n ];

			codeGlyph->code = code;
			ret = FontBitmapCharGlyph_Generate( codeGlyph , 1 , m_Cache );

			if ( ret != CELL_OK )
			{
				codeGlyph->code = 0;
				return (SFontBitmapCharGlyph_t*)0;
			}
			return codeGlyph;
		}
	}
	m_Cache.cacheN++;
	if ( m_Cache.cacheN >= m_Cache.count )
		m_Cache.cacheN = 0;

	codeGlyph = &m_Cache.BitmapGlyph[ m_Cache.cacheN ];

	//J キャッシュをつぶして使う
	Glyph_Delete( codeGlyph , 1 );

	codeGlyph->code = code;
	ret = FontBitmapCharGlyph_Generate( codeGlyph , 1 , m_Cache );

	if ( ret != CELL_OK )
	{
		codeGlyph->code = 0;
		return (SFontBitmapCharGlyph_t*)0;
	}
	return codeGlyph;
}

//J 文字イメージオブジェクトの取得（半角文字専用)
inline const SFontBitmapCharGlyph_t* CFont::Ascii_GetGlyph( uint32_t code )
{
	int i;

	if ( code < 0x80 )
	{
		i = ( code < 0x20 || code > 0x7E )? 0 : code - 0x20;
		return &m_Ascii.BitmapGlyph[ i ];
	}
	return (SFontBitmapCharGlyph_t*)0;
}

//J 文字イメージオブジェクトの破棄
inline void CFont::Glyph_Delete( SFontBitmapCharGlyph_t* a_pBitmapGlyph , int a_count )
{
	for ( int n = 0; n < a_count; n++ )
	{
		if ( a_pBitmapGlyph[ n ].Image )
		{
			free(a_pBitmapGlyph[ n ].Image);
			a_pBitmapGlyph[ n ].Image = NULL;
		}
	}
}
//J 文字イメージキャッシュの作成
inline int CFont::Cache_Init( int count, float wf, float hf, float weight, float slant )
{
	int n, ret = !CELL_OK;

	m_Cache.font  = &m_Font;
	m_Cache.count = count;
	m_Cache.wf = wf;
	m_Cache.hf = hf;
	m_Cache.weight = weight;
	m_Cache.slant  = slant;

	m_Cache.BitmapGlyph = (SFontBitmapCharGlyph_t*)malloc( count * sizeof( SFontBitmapCharGlyph_t ) );

	if ( m_Cache.BitmapGlyph )
	{
		for ( n = 0; n < count; n++ )
		{
			m_Cache.BitmapGlyph[ n ].code  = 0x00000000;
			m_Cache.BitmapGlyph[ n ].Image = (uint8_t*)0;
		}
		return CELL_OK;
	}
	return ret;
}
//J 文字イメージオブジェクトの半角文字作り置き
inline int CFont::Ascii_Init( float wf, float hf, float weight, float slant )
{
	uint32_t n;
	int ret = !CELL_OK;

	//m_Ascii.font  = &m_Font;
	m_Ascii.count = 0x7E - 0x20 + 1;
	m_Ascii.wf = wf;
	m_Ascii.hf = hf;
	m_Ascii.weight = weight;
	m_Ascii.slant  = slant;

	m_Ascii.BitmapGlyph = (SFontBitmapCharGlyph_t*)malloc( m_Ascii.count * sizeof( SFontBitmapCharGlyph_t ) );

	if ( m_Ascii.BitmapGlyph )
	{
		for ( n = 0; n < m_Ascii.count; n++ )
			m_Ascii.BitmapGlyph[ n ].code = 0x20 + n;

		FontBitmapCharGlyph_Generate( m_Ascii.BitmapGlyph , m_Ascii.count , m_Ascii );
		return CELL_OK;
	}
	return ret;
}
//J 文字イメージを破棄（半角文字）
inline void CFont::Ascii_End()
{
	if( m_Ascii.BitmapGlyph )
	{
		Glyph_Delete( m_Ascii.BitmapGlyph , m_Ascii.count );
		free(m_Ascii.BitmapGlyph);
		m_Ascii.BitmapGlyph = NULL;
		m_Ascii.count = 0;
	}
}
//J 文字イメージオブジェクトからテクスチャへ転送
//J イメージのクオリティは、サブピクセル分のシフトがあるため元イメージより劣化します。
inline int CFont::CharGlyph_Trans_blendCast_ARGB8( const STransData& a_Data , const SFontBitmapCharGlyph_t *glyph )
{
	int xi, yi;
	int x, y, n;
	int sx, sy;
	float f0,f1,f2,f3;

	{
		float xm0, ym0, xm1, ym1;
		xi = (int)a_Data.m_fX;
		if ( a_Data.m_fX > 0 )
		{
			xm1 = a_Data.m_fX - (float)xi;
			xm0 = 1.0f - xm1;
		}
		else
		{
			xm0 = -( a_Data.m_fX - (float)xi );
			xm1 = 1.0f - xm0;
			if ( xm0 != 0.0f ) xi--;
		}
		yi = (int)a_Data.m_fY;
		if ( a_Data.m_fY > 0 )
		{
			ym1 = a_Data.m_fY - (float)yi;
			ym0 = 1.0f - ym1;
		}
		else
		{
			ym0 = -( a_Data.m_fY - (float)yi );
			ym1 = 1.0f - ym0;
			if ( ym0 != 0.0f ) yi--;
		}
		xi += (int)glyph->x0;
		yi += (int)glyph->y0;

		f0 = xm0 * ym0;
		f1 = xm1 * ym0;
		f2 = xm0 * ym1;
		f3 = xm1 * ym1;
	}
	n = 0;
	for ( y = 0; y < glyph->h; y++ )
	{
		for ( x = 0; x < glyph->w; x++ )
		{
			unsigned char *p;
			int level, a;
			level = glyph->Image[ n ];
			n++;

			sx = xi + x;
			sy = yi + y;
			p = (uint8_t*)a_Data.m_pSurf->buffer + (sy*a_Data.m_pSurf->widthByte + sx*4);

			if ( sy >= (int)a_Data.m_pSurf->Scissor.y0 && sy < (int)a_Data.m_pSurf->Scissor.y1 )
			{
				if ( sx >= (int)a_Data.m_pSurf->Scissor.x0 && sx < (int)a_Data.m_pSurf->Scissor.x1 )
				{
					a = int(level * f0);
					if ( x || y )
					{
						a += p[0];
					}
					if ( a )
					{
						uint32_t rgba = a;
						rgba = (rgba<<8)|( p[1] * (256-a) + GetR() * a)/ 256;
						rgba = (rgba<<8)|( p[2] * (256-a) + GetG() * a)/ 256;
						rgba = (rgba<<8)|( p[3] * (256-a) + GetB() * a)/ 256;
						*(uint32_t*)&p[0] = rgba;
					}
				}
				sx++;
				if ( sx >= (int)a_Data.m_pSurf->Scissor.x0 && sx <  (int)a_Data.m_pSurf->Scissor.x1 )
				{
					a = int(level * f1);
					if ( y )
						a += p[4];
					if ( a )
					{
						if ( x == glyph->w-1 )
						{
							uint32_t rgba = a;
							rgba = (rgba<<8)|( p[5] * (256-a) + GetR() * a)/ 256;
							rgba = (rgba<<8)|( p[6] * (256-a) + GetG() * a)/ 256;
							rgba = (rgba<<8)|( p[7] * (256-a) + GetB() * a)/ 256;
							*(uint32_t*)&p[4] = rgba;
						}
						else p[4] = a;
					}
				}
				sx--;
			}
			sy++;
			if ( sy >= (int)a_Data.m_pSurf->Scissor.y0 && sy < (int)a_Data.m_pSurf->Scissor.y1 )
			{
				p += a_Data.m_pSurf->widthByte;
				if ( sx >= (int)a_Data.m_pSurf->Scissor.x0 && sx < (int)a_Data.m_pSurf->Scissor.x1 )
				{
					a = int(level * f2);
					if ( x )
						a += p[0];
					if ( a )
					{
						if ( y == glyph->h-1 )
						{
							uint32_t rgba = a;
							rgba = (rgba<<8)|( p[1] * (256-a) + GetR() * a)/ 256;
							rgba = (rgba<<8)|( p[2] * (256-a) + GetG() * a)/ 256;
							rgba = (rgba<<8)|( p[3] * (256-a) + GetB() * a)/ 256;
							*(uint32_t*)&p[0] = rgba;
						}
						else p[0] = a;
					}
				}
				sx++;
				if ( sx >= (int)a_Data.m_pSurf->Scissor.x0 && sx < (int)a_Data.m_pSurf->Scissor.x1 )
				{
					a = int(level * f3);
					p[4] = a;
				}
			}
		}
	}
	return CELL_OK;
}
//J 文字イメージフォントの初期化
inline int CFont::Init( float a_wf, float a_hf, float a_weight, float a_slant, int a_cacheMax )
{
	int ret;

	if( CELL_OK != (ret = cellFontSetScalePixel( &m_Font , a_wf , a_hf ) ) )
	{
		FontLog( "cellFontSetScalePixel = %08X" , ret );
		return ret;
	}
	if( CELL_OK != (ret = cellFontSetEffectWeight( &m_Font , a_weight ) ) )
	{
		FontLog( "cellFontSetEffectWeight = %08X" , ret );
		return ret;
	}
	if( CELL_OK != (ret = cellFontSetEffectSlant( &m_Font , a_slant ) ) )
	{
		FontLog( "cellFontSetEffectSlant = %08X" , ret );
		return ret;
	}
	if( CELL_OK != (ret = cellFontGetHorizontalLayout( &m_Font , &m_HorizontalLayout ) ) )
	{
		FontLog( "cellFontGetHorizontalLayout = %08X" , ret );
		return ret;
	}
	if( CELL_OK != (ret = cellFontGetVerticalLayout( &m_Font , &m_VerticalLayout ) ) )
	{
		FontLog( "cellFontGetVerticalLayout = %08X" , ret );
		return ret;
	}

	ret = Ascii_Init( a_wf , a_hf , a_weight , a_slant );
	if ( ret == CELL_OK )
	{
		ret = Cache_Init( a_cacheMax, a_wf, a_hf, a_weight, a_slant );
		if ( ret == CELL_OK )
			return ret;
		FontLog( "Cache_Init = %08X" , ret );
		Ascii_End();
	}
	else
	{
		FontLog( "Ascii_Init = %08X" , ret );
	}
	return ret;
}
//J 文字イメージフォントの終了
inline void CFont::End()
{
	Ascii_End( );
	//J 文字イメージキャッシュ破棄
	if( m_Cache.BitmapGlyph )
	{
		Glyph_Delete( m_Cache.BitmapGlyph , m_Cache.count );
		free(m_Cache.BitmapGlyph);
		m_Cache.BitmapGlyph = NULL;
		m_Cache.count = 0;
	}
}
//J 文字イメージフォントの横書きレイアウト取得
inline const CellFontHorizontalLayout& CFont::GetHorizontalLayout()const
{
	return m_HorizontalLayout;
}
inline uint8_t CFont::GetR()const
{
	return (uint8_t)((m_uColor>>16) & 0xFF);
}
inline uint8_t CFont::GetG()const
{
	return (uint8_t)((m_uColor>>8) & 0xFF);
}
inline uint8_t CFont::GetB()const
{
	return (uint8_t)(m_uColor & 0xFF);
}
//J 文字イメージフォントの文字イメージ取得
inline const SFontBitmapCharGlyph_t* CFont::GetGlyph( uint32_t code )
{
	const SFontBitmapCharGlyph_t* codeGlyph;
	codeGlyph = Ascii_GetGlyph( code );
	if ( ! codeGlyph )
	{
		codeGlyph = Cache_GetGlyph( code );
		if ( ! codeGlyph )
			codeGlyph = Ascii_GetGlyph( 0 );
	}
	return codeGlyph;
}
//J 文字イメージフォントによる文字列レンダリング
inline float CFont::RenderPropText( CellFontRenderSurface* a_pSurf , float a_fX , float a_fY , const uint8_t* utf8 , float a_fBetween /*= 0.0f*/ )
{
	const SFontBitmapCharGlyph_t* codeGlyph;
	uint32_t code;

	if ( (!utf8) || *utf8 == 0x00 )
		return a_fX;

	STransData data;
	data.m_pSurf = a_pSurf;
	data.m_fY = a_fY;
	data.m_fX = a_fX;

	//J 最初の文字取り出し
	utf8 += getUcs4( utf8 , &code , 0x3000 );

	codeGlyph = GetGlyph( code );

	//J 最初の文字の左合わせ
	data.m_fX += -(codeGlyph->Metrics.Horizontal.bearingX);

	for (;;)
	{
		//J レンダリング
		if ( codeGlyph->Image )
		{
			CharGlyph_Trans_blendCast_ARGB8( data , codeGlyph );
		}

		data.m_fX += codeGlyph->Metrics.Horizontal.advance + a_fBetween;

		//J 次の文字を取得
		utf8 += getUcs4( utf8, &code, 0x3000 );

		if ( code == 0x00000000 )
			break;
		codeGlyph = GetGlyph( code );
	}
	return data.m_fX;
}
#endif //__WQSG_FONT_INLINE_H__

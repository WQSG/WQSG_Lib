#ifndef __WQSG_DISPLAY_H__
#define __WQSG_DISPLAY_H__
#include "wqsg_def.h"

#define PSGL 1
#include <PSGL/psgl.h>
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;
// default values for display
#ifdef CELL_DISPLAY_MODE
#define FWDISPLAYINFO_DEFAULT_MODE			CELL_DISPLAY_MODE
#else
#define FWDISPLAYINFO_DEFAULT_MODE			DisplayMode_720p
#endif


#define FWDISPLAYINFO_DEFAULT_PERSISTENT_MEMORY_SIZE	(160 << 20)
#define FWDISPLAYINFO_DEFAULT_PSGL_RAW_SPUS		1
#define FWDISPLAYINFO_DEFAULT_HOST_MEMORY_SIZE		0

class CDisplayInfo
{
public:
	inline CDisplayInfo();
	~CDisplayInfo(){}

	//-----------------------------------------------------------------------------
	// Description: Display mode enumeration
	//-----------------------------------------------------------------------------
	enum EDisplayMode
	{
		DisplayMode_VGA,	// VGA monitor
		DisplayMode_480i,	// 720x480 lines interlaced
		DisplayMode_480p,	// 720x480 lines progressive
		DisplayMode_576i,	// 720x576 lines interlaced
		DisplayMode_576p,	// 720x576 lines progressive
		DisplayMode_720p,	// 1280x720 lines progressive
		DisplayMode_1080i,	// 1920x1080 lines interlaced
		DisplayMode_1080p,	// 1920x1080 lines progressive

		DisplayMode_WXGA_720p, // 1280x720 lines progressive on VESA 1280x768
		DisplayMode_SXGA_720p, // 1280x720 lines progressive on VESA 1280x1024
		DisplayMode_WUXGA_1080p, // 1920x1080 lines progressive on VESA 1920x1200
	};

	// Display mode	
	EDisplayMode		m_DisplayMode;
	// Display width (pixels)
	int			m_Width;
	// Display height (pixels)
	int			m_Height;
	// Color depth (bits)
	int			m_ColorBits;
	// Alpha depth (bits)
	int			m_AlphaBits;
	// Z depth (bits)
	int			m_DepthBits;
	// Stencil depth (bits)
	int			m_StencilBits;
	// Antialiasing enabled flag
	bool			m_AntiAlias;
	// Vsync enabled flag
	bool			m_VSync;
	// Aspect Ratio (16:9 wide-screen)
	bool		m_WideScreen;
#ifdef PSGL
	// PSGL initialization options
	PSGLinitOptions		m_PSGLInitOptions;
#endif//PSGL
	// PSGL context
	PSGLcontext* m_GlContext;
};

CDisplayInfo::CDisplayInfo()
: m_DisplayMode(DisplayMode_720p)
, m_ColorBits(24)
, m_AlphaBits(8)
, m_DepthBits(24)
, m_StencilBits(8)
, m_AntiAlias(false)
, m_VSync(true)
{
	// Set width and height given a particular display mode
	switch(m_DisplayMode)
	{
	case DisplayMode_VGA:
		m_Width = 1024;
		m_Height = 768;
		break;
	case DisplayMode_480i:
	case DisplayMode_480p:
		m_Width = 720;
		m_Height = 480;
		break;
	case DisplayMode_576i:
	case DisplayMode_576p:
		m_Width = 720;
		m_Height = 576;
		break;
	case DisplayMode_720p:
	case DisplayMode_WXGA_720p:
	case DisplayMode_SXGA_720p:
		m_Width = 1280;
		m_Height = 720;
		break;
	case DisplayMode_1080i:
	case DisplayMode_1080p:
	case DisplayMode_WUXGA_1080p:
		m_Width = 1920;
		m_Height = 1080;
		break;
	}

#ifdef PSGL
	// set default PSGL initialization options
	// these can be overridden by the application in its constructor if required
	m_PSGLInitOptions.enable = PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_PERSISTENT_MEMORY_SIZE | PSGL_INIT_HOST_MEMORY_SIZE;
	m_PSGLInitOptions.maxSPUs = 1;
	m_PSGLInitOptions.initializeSPUs = GL_FALSE;
	m_PSGLInitOptions.persistentMemorySize = 160<<20;
	m_PSGLInitOptions.hostMemorySize = 0;
#endif//PSGL
}

class CDisplay : public CSingletonT<CDisplay>
{
	friend class CSingletonT<CDisplay>;
	CDisplayInfo m_DispInfo;

	PSGLdevice* m_pDevice;

	CDisplay()
		: m_DispInfo()
		, m_pDevice(NULL)
	{
	}
	~CDisplay(){}
public:
	inline int32_t Init();

	inline void UnInit();

	inline void setRenderingContext();

	inline void flip();

	inline const CDisplayInfo& GetDispInfo()const
	{
		return m_DispInfo;
	}
protected:
	inline void createDeviceAndSurfaces();
};

inline int32_t CDisplay::Init()
{
	int32_t ret;

	// setup video
	// read the current video status
	// INITIAL DISPLAY MODE HAS TO BE SET BY RUNNING SETMONITOR.SELF
	CellVideoOutState videoState;
	ret = cellVideoOutGetState( CELL_VIDEO_OUT_PRIMARY , 0 , &videoState );
	if ( ret != CELL_OK )
	{
		printf("cellVideoOutGetState failed\n");
		return ret;
	}

	// keep the current resolution
	// get width and height in pixels from resolutionId
	CellVideoOutResolution resolution;
	ret = cellVideoOutGetResolution( videoState.displayMode.resolutionId , &resolution );
	if ( ret != CELL_OK )
	{
		printf("cellVideoOutGetResolution failed\n");
		return ret;
	}

	m_DispInfo.m_WideScreen = (videoState.displayMode.aspect == CELL_VIDEO_OUT_ASPECT_16_9);

	// init graphics
	PSGLinitOptions	psglInitOptions;
	psglInitOptions = m_DispInfo.m_PSGLInitOptions;	
	psglInit( &psglInitOptions );
	psglFXInit();

	// 	m_DispInfo.mColorBits = dispInfo.mColorBits;
	// 	m_DispInfo.mAlphaBits = dispInfo.mAlphaBits;
	// 	m_DispInfo.mDepthBits = dispInfo.mDepthBits;
	// 	m_DispInfo.mStencilBits = dispInfo.mStencilBits;
	// 	m_DispInfo.mAntiAlias = dispInfo.mAntiAlias;
	// 	m_DispInfo.mVSync = dispInfo.mVSync;
	// 	m_DispInfo.mPSGLInitOptions = dispInfo.mPSGLInitOptions;

	// create it and surfaces
	createDeviceAndSurfaces();

	printf("Display resolution: %dx%d " , m_DispInfo.m_Width , m_DispInfo.m_Height );
	printf( m_DispInfo.m_WideScreen ? "(16:9)\n" : "(4:3)\n" );

	// set display information to FWApplication
	// 	FWGLApplication *app;
	// 	app = (FWGLApplication*)FWApplication::getApplication();
	// 	app->FWGLApplication::onSize(mDispInfo);

	return CELL_OK;
}

inline void CDisplay::UnInit()
{
	psglDestroyContext( m_DispInfo.m_GlContext );
	psglDestroyDevice( m_pDevice );
	psglFXExit();
	psglExit();
}

inline void CDisplay::setRenderingContext()
{
	psglMakeCurrent( m_DispInfo.m_GlContext , m_pDevice );
}

inline void CDisplay::flip()
{
	psglSwap();
}

inline void CDisplay::createDeviceAndSurfaces()
{
	GLenum colorFormat = (m_DispInfo.m_ColorBits == 24) ?GL_ARGB_SCE : GL_RGBA16F_ARB;
	//UINT8 when mColorBits are 24, FP16 in other case
	GLenum depthFormat = (m_DispInfo.m_DepthBits == 24) ?GL_DEPTH_COMPONENT24 : GL_NONE;
	GLenum multisamplingMode = (m_DispInfo.m_AntiAlias == true) ?GL_MULTISAMPLING_4X_SQUARE_CENTERED_SCE : GL_MULTISAMPLING_NONE_SCE;

	// create PSGL device and context
	m_pDevice = psglCreateDeviceAuto(colorFormat, depthFormat, multisamplingMode);
	m_DispInfo.m_GlContext = psglCreateContext();
	psglMakeCurrent( m_DispInfo.m_GlContext, m_pDevice );

	// reset the context and setup buffers
	psglResetCurrentContext();

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);

	// enable vsync
	if( m_DispInfo.m_VSync )
		glEnable(GL_VSYNC_SCE);
	else
		glDisable(GL_VSYNC_SCE);

	// set default server and client texture units
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);

	psglGetDeviceDimensions( m_pDevice , (GLuint*)&m_DispInfo.m_Width , (GLuint*)&m_DispInfo.m_Height);
}


#include <cell/dbgfont.h>
#define DEF_DDBFONT 1
class CDebugFont : public CSingletonT<CDebugFont>
{
	friend class CSingletonT<CDebugFont>;

	CellDbgFontConsoleId m_DbgFontID;

	CDebugFont()
		: m_DbgFontID(0)
	{
	}
	~CDebugFont(){}
public:
	void dbgFontInit(void)
	{
#if DEF_DDBFONT
		// initialize debug font library, then open 2 consoles
		CellDbgFontConfig cfg;
		cfg.bufSize      = 512;
		cfg.screenWidth  = CDisplay::I().GetDispInfo().m_Width;
		cfg.screenHeight = CDisplay::I().GetDispInfo().m_Height;
		cellDbgFontInit(&cfg);

		CellDbgFontConsoleConfig ccfg0;
		ccfg0.posLeft     = 0.1f;
		ccfg0.posTop      = 0.8f;
		ccfg0.cnsWidth    = 16;
		ccfg0.cnsHeight   = 4;
		ccfg0.scale       = 0.25f;
		ccfg0.color       = 0xff0080ff;  // ABGR -> orange
		m_DbgFontID = cellDbgFontConsoleOpen(&ccfg0);
#endif
	}

	void dbgFontExit(void)
	{
#if DEF_DDBFONT
		cellDbgFontExit();
#endif
	}

	void dbgFontDraw(void)
	{
#if DEF_DDBFONT
		cellDbgFontDraw();
#endif
	}

	void dbgFontPrintf(float x,float y, float scale,char* fmt,...)
	{
#if DEF_DDBFONT
		//build the output string
		char tempstr[512];

		va_list arglist;
		va_start(arglist, fmt);
		vsprintf( tempstr , fmt , arglist );
		va_end(arglist);

		cellDbgFontPuts((x/CDisplay::I().GetDispInfo().m_Width),(y/CDisplay::I().GetDispInfo().m_Height),scale, 0xffffffff, tempstr);
#endif
	}
};

#endif

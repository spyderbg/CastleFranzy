// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "CXBOX360D3D9Driver.h"

#ifdef _IRR_XBOX_PLATFORM_

#include "irros.h"
#include "S3DVertex.h"
#include "CXBOX360D3D9Texture.h"
#include "CImage.h"
#include "CXBOX360D3D9MaterialRenderer.h"
#include "CXBOX360D3D9ShaderMaterialRenderer.h"
#include "CXBOX360D3D9NormalMapRenderer.h"
#include "CXBOX360D3D9ParallaxMapRenderer.h"
#include "CXBOX360D3D9HLSLMaterialRenderer.h"
#include "CXBOX360D3D9BasicShaders.h"
#include "CTextureManager.h"
#include <float.h>

#include <xtl.h>
#include <xgraphics.h>

namespace irr
{
namespace video
{

CXBOX360D3D9ShaderBasicConstantSetCallBack *g_pBasicShaderConstantCallBack;

D3DMATRIX UnitMatrixD3D9;
D3DMATRIX SphereMapMatrixD3D9;

//--------------------------------------------------------------------------------------
// Tiling scenarios
//
// Note that the list must be NULL terminated, and that it is not necessary to
// initialize all 15 possible tiling rectangles.
//
// Also note that the tiling rectangles must be texture tile aligned (32x32), unless 
// a rectangle coordinate matches the dimensions of the front buffer (i.e. the rectangle
// touches an edge of the screen).
//--------------------------------------------------------------------------------------
// 640x480 tiling scenarios
static TILING_SCENARIO TILING_SCENARIOS_480[] =
{
	{
		L"640x480 4xMSAA Vertical Split",
			640,
			480,
			D3DMULTISAMPLE_4_SAMPLES,
			D3DTILING_ONE_PASS_ZPASS,
			D3DSEQM_PRECLIP,
			2,
		{
			{   0, 0, 320, 480 },
			{ 320, 0, 640, 480 }
		}
	},
	{ 0 }
};

static TILING_SCENARIO TILING_SCENARIOS_720[] =
{
    {
        L"1280x720 No MSAA",
        1280,
        720,
        D3DMULTISAMPLE_NONE,
        0,
        D3DSEQM_PRECLIP,
        1,
        {
            { 0,   0, 1280, 720 },
        }
    },
    {
        L"1280x720 2xMSAA Horizontal Split",
        1280,
        720,
        D3DMULTISAMPLE_2_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        2,
        {
            { 0,   0, 1280, 384 },
            { 0, 384, 1280, 720 }
        }
    },
    {
        L"1280x720 2xMSAA Vertical Split",
        1280,
        720,
        D3DMULTISAMPLE_2_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        2,
        {
            {   0, 0,  640, 720 },
            { 640, 0, 1280, 720 }
        }
    },
    {
        L"1280x720 2xMSAA Horizontal Split, One Pass Z Pass",
        1280,
        720,
        D3DMULTISAMPLE_2_SAMPLES,
        D3DTILING_ONE_PASS_ZPASS | D3DTILING_FIRST_TILE_INHERITS_DEPTH_BUFFER,
        D3DSEQM_CULLED,
        2,
        {
            { 0,   0, 1280, 384 },
            { 0, 384, 1280, 720 }
        }
    },
    {
        L"1280x720 2xMSAA Vertical Split, One Pass Z Pass",
        1280,
        720,
        D3DMULTISAMPLE_2_SAMPLES,
        D3DTILING_ONE_PASS_ZPASS,
        D3DSEQM_CULLED,
        2,
        {
            {   0, 0,  640, 720 },
            { 640, 0, 1280, 720 }
        }
    },
    {
        L"1280x720 4xMSAA Horizontal Split",
        1280,
        720,
        D3DMULTISAMPLE_4_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        3,
        {
            { 0,   0, 1280, 256 },
            { 0, 256, 1280, 512 },
            { 0, 512, 1280, 720 },
        }
    },
    {
        L"1280x720 4xMSAA Vertical Split",
        1280,
        720,
        D3DMULTISAMPLE_4_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        4,
        {
            {   0,   0,  320, 720 },
            { 320,   0,  640, 720 },
            { 640,   0,  960, 720 },
            { 960,   0, 1280, 720 },
        }
	}
};

static TILING_SCENARIO TILING_SCENARIOS_1080[] =
{
    {
        L"1920x1080 No MSAA Horizontal Split",
        1920,
        1080,
        D3DMULTISAMPLE_NONE,
        0,
        D3DSEQM_PRECLIP,
        2,
        {
            {   0,   0, 1920,  544 },
            {   0, 544, 1920, 1080 },
        }
    },
    {
        L"1920x1080 No MSAA Vertical Split",
        1920,
        1080,
        D3DMULTISAMPLE_NONE,
        0,
        D3DSEQM_PRECLIP,
        2,
        {
            {   0,   0,  960, 1080 },
            { 960,   0, 1920, 1080 },
        }
    },
    {
        L"1920x1080 2x MSAA Vertical Split",
        1920,
        1080,
        D3DMULTISAMPLE_2_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        4,
        {
            {    0,   0,  480, 1080 },
            {  480,   0,  960, 1080 },
            {  960,   0, 1440, 1080 },
            { 1440,   0, 1920, 1080 },
        }
    },
    {
        L"1920x1080 4x MSAA Horizontal Split",
        1920,
        1080,
        D3DMULTISAMPLE_4_SAMPLES,
        0,
        D3DSEQM_PRECLIP,
        7,
        {
            {    0,    0, 1920,  160 },
            {    0,  160, 1920,  320 },
            {    0,  320, 1920,  480 },
            {    0,  480, 1920,  640 },
            {    0,  640, 1920,  800 },
            {    0,  800, 1920,  960 },
            {    0,  960, 1920, 1080 },
        }
    },
    { NULL }
};

//--------------------------------------------------------------------------------------
// Name: LargestTileRectSize()
// Desc: Returns the dimensions that fit the largest rectangle(s) out of the tiling
//       rectangles.
//--------------------------------------------------------------------------------------
static VOID LargestTileRectSize( const TILING_SCENARIO& Scenario, D3DPOINT* pMaxSize )
{
    pMaxSize->x = 0;
    pMaxSize->y = 0;
    for( DWORD i = 0; i < Scenario.dwTileCount; i++ )
    {
        DWORD dwWidth = Scenario.TilingRects[i].x2 - Scenario.TilingRects[i].x1;
        DWORD dwHeight = Scenario.TilingRects[i].y2 - Scenario.TilingRects[i].y1;
        if( dwWidth > ( DWORD )pMaxSize->x )
            pMaxSize->x = dwWidth;
        if( dwHeight > ( DWORD )pMaxSize->y )
            pMaxSize->y = dwHeight;
    }
}

//! constructor
CXBOX360D3D9Driver::CXBOX360D3D9Driver(const core::dimension2d<s32>& screenSize, HWND window,
				bool fullscreen, bool stencilbuffer,
				io::IFileSystem* io, bool pureSoftware)
: CNullDriver(io, screenSize), CurrentRenderMode(ERM_NONE),
	ResetRenderStates(true), Transformation3DChanged(false), StencilBuffer(stencilbuffer),
	D3DLibrary(0), pID3D(0), pID3DDevice(0), PrevRenderTarget(0),
	WindowId(0), SceneSourceRect(0),
	LastVertexType((video::E_VERTEX_TYPE)-1), MaxTextureUnits(0), MaxUserClipPlanes(0),
	MaxLightDistance(sqrtf(FLT_MAX)), LastSetLight(-1), DeviceLost(false),
	Fullscreen(fullscreen), DriverWasReset(true), 
	m_pCurrentScenario(0),m_nCurrentRenderFrontBuffer(0)
{
	CXBOX360D3D9ShaderBasicConstantSetCallBack* pCB = irrnew CXBOX360D3D9ShaderBasicConstantSetCallBack();
	pCB->grab();
	g_pBasicShaderConstantCallBack = pCB;

	#ifdef _DEBUG
	setDebugName("CXBOX360D3D9Driver");
	#endif

	m_pFrontBuffer[0] = NULL;
	m_pFrontBuffer[1] = NULL;
	m_pTilingRenderTarget[0] = NULL;
	m_pTilingRenderTarget[1] = NULL;
	m_pTilingDepthStencil[0] = NULL;
	m_pTilingDepthStencil[1] = NULL;
	printVersion();

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		CurrentTexture[i] = 0;
		LastTextureMipMapsAvailable[i] = false;
	}

	// create sphere map matrix

	SphereMapMatrixD3D9._11 = 0.5f; SphereMapMatrixD3D9._12 = 0.0f;
	SphereMapMatrixD3D9._13 = 0.0f; SphereMapMatrixD3D9._14 = 0.0f;
	SphereMapMatrixD3D9._21 = 0.0f; SphereMapMatrixD3D9._22 =-0.5f;
	SphereMapMatrixD3D9._23 = 0.0f; SphereMapMatrixD3D9._24 = 0.0f;
	SphereMapMatrixD3D9._31 = 0.0f; SphereMapMatrixD3D9._32 = 0.0f;
	SphereMapMatrixD3D9._33 = 1.0f; SphereMapMatrixD3D9._34 = 0.0f;
	SphereMapMatrixD3D9._41 = 0.5f; SphereMapMatrixD3D9._42 = 0.5f;
	SphereMapMatrixD3D9._43 = 0.0f; SphereMapMatrixD3D9._44 = 1.0f;

	core::matrix4 mat;
	UnitMatrixD3D9 = *(D3DMATRIX*)((void*)mat.pointer());

	ClearColor = 0;
	ClearDepth = 1.0;
	ClearStencil = 0;
	//m_pMaterialCallback = irrnew CXBOX360D3D9ShaderBasicConstantSetCallBack();
	// init direct 3d is done in the factory function
}


//! destructor
CXBOX360D3D9Driver::~CXBOX360D3D9Driver()
{
	if(pID3DDevice)
	{
		pID3DDevice->SetRenderTarget(0, NULL);
		pID3DDevice->SetRenderTarget(1, NULL);
		pID3DDevice->SetRenderTarget(2, NULL);
		pID3DDevice->SetRenderTarget(3, NULL);
		pID3DDevice->SetDepthStencilSurface(NULL);
		pID3DDevice->SetVertexShader(NULL);
		pID3DDevice->SetPixelShader(NULL);
		
	}
	//if(m_pMaterialCallback)
	//{
	//	m_pMaterialCallback->drop();
	//	m_pMaterialCallback = NULL;
	//}
	deleteMaterialRenders();

	if(g_pBasicShaderConstantCallBack)
	{
		g_pBasicShaderConstantCallBack->drop();
		g_pBasicShaderConstantCallBack = NULL;
	}
	// drop d3d9

	for(int i = 1; i >=0; --i)
	{
		if(m_pTilingDepthStencil[i])
		{
			m_pTilingDepthStencil[i]->Release();
			m_pTilingDepthStencil[i] = NULL;

		}
		if(m_pTilingRenderTarget[i])
		{
			m_pTilingRenderTarget[i]->Release();
			m_pTilingRenderTarget[i] = NULL;
		}
		if(m_pFrontBuffer[i])
		{
			m_pFrontBuffer[i]->Release();
			m_pFrontBuffer[i] = NULL;
		}
	}
	if (pID3DDevice)
		pID3DDevice->Release();

	if (pID3D)
		pID3D->Release();


}

static void addAndDropMaterialRendererLocal(IMaterialRenderer* m)
{
	m->drop();
}

void CXBOX360D3D9Driver::createMaterialRenderers()
{
	// create D3D9 material renderers

	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_SOLID(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_SOLID_2_LAYER_DECAL(pID3DDevice, this));

	// add the same renderer for all lightmap types

	CXBOX360D3D9MaterialRenderer_LIGHTMAP* lmr = irrnew CXBOX360D3D9MaterialRenderer_LIGHTMAP(pID3DDevice, this);
	//addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining fixed function pipeline material renderers

	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_DETAIL_MAP(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_SPHERE_MAP(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_REFLECTION_2_LAYER(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_ADD_COLOR(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(pID3DDevice, this));

	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;

	renderer = irrnew CXBOX360D3D9NormalMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();

	renderer = irrnew CXBOX360D3D9NormalMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();

	renderer = irrnew CXBOX360D3D9NormalMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	//// add parallax map renderers

	renderer = irrnew CXBOX360D3D9ParallaxMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_SOLID].Renderer);
	renderer->drop();

	renderer = irrnew CXBOX360D3D9ParallaxMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	renderer->drop();

	renderer = irrnew CXBOX360D3D9ParallaxMapRenderer(pID3DDevice, this, tmp,
		MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	renderer->drop();

	//// add basic 1 texture blending
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_ONETEXTURE_BLEND(pID3DDevice, this));


	//TRANSPARENT_TEXTURE_VERTEX_ALPHA
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(pID3DDevice, this));
	addAndDropMaterialRendererLocal(irrnew CXBOX360D3D9MaterialRenderer_2D_ALPHA(pID3DDevice, this));
	

}


//! initialises the Direct3D API
bool CXBOX360D3D9Driver::initDriver(const core::dimension2d<s32>& screenSize,
		HWND hwnd, u32 bits, bool fullScreen, bool pureSoftware,
		bool highPrecisionFPU, bool vsync, bool antiAlias, E_FULLSCREEN_ANTIALIASING_SETTING aa_setting)
{

	fullScreen = true;
	pureSoftware = false;
	//because XBOX 360 TCR, we will always use the anitaliasing at least 2x
	antiAlias = true;
	if(aa_setting == irr::video::EAA_NONE)
		aa_setting = irr::video::EAA_2X;
	//vsync = false;
	bits = 32;

    //XVIDEO_MODE VideoMode;
    //XGetVideoMode( &VideoMode );

	if(screenSize != core::dimension2d<s32>(640, 480)
		&&screenSize != core::dimension2d<s32>(1280, 720)
		&&screenSize != core::dimension2d<s32>(1920, 1080)
		)
	{
		os::Printer::log("Due to 2x or 4x MSAA only implement with 640x480, 1280x720, 1920x1080, the Screen Size should only in these three type!", ELL_ERROR);
		return false;		
	}

	//AA is M$'s TCR
	if(screenSize.Height == 480)
	{
		//always use the 4x MSAA
		aa_setting = irr::video::EAA_4X;
		m_pCurrentScenario = &TILING_SCENARIOS_480[0];
	}
	else if(screenSize.Height == 720)
	{
		if(aa_setting == irr::video::EAA_2X)
			m_pCurrentScenario = &TILING_SCENARIOS_720[4];//use the 2xMSAA
		else if(aa_setting == irr::video::EAA_4X)
			m_pCurrentScenario = &TILING_SCENARIOS_720[6];//use the 4xMSAA
		else
			m_pCurrentScenario = &TILING_SCENARIOS_720[0];
	}
	else if(screenSize.Height == 1080)
	{
		if(aa_setting == irr::video::EAA_2X)
			m_pCurrentScenario = &TILING_SCENARIOS_1080[2];//use the 2xMSAA
		else if(aa_setting == irr::video::EAA_2X)
			m_pCurrentScenario = &TILING_SCENARIOS_1080[3];//use the 4xMSAA
		else
			m_pCurrentScenario = &TILING_SCENARIOS_1080[0];
	}

	HRESULT hr;
	Fullscreen = fullScreen;
	CurrentDepthBufferSize = screenSize;

	if (!pID3D)
	{
		pID3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (!pID3D)
		{
			os::Printer::log("Error initializing D3D.", ELL_ERROR);
			return false;
		}
	}

	UINT adapter = D3DADAPTER_DEFAULT;
	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

	{//set up present

		ZeroMemory(&present, sizeof(present));

		//D3DPRESENT_INTERVAL_DEFAULT
		if (vsync)
		{
			present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
	
		present.BackBufferCount		= 1;
		present.EnableAutoDepthStencil	= FALSE;
		present.BackBufferWidth = screenSize.Width;
		present.BackBufferHeight = screenSize.Height;
		// request 32bit mode if user specified 32 bit, added by Thomas Stuefe
		//if (bits == 32)
			present.BackBufferFormat = D3DFMT_A8R8G8B8;
		//else
		//	present.BackBufferFormat = D3DFMT_R5G6B5;
		present.SwapEffect	= D3DSWAPEFFECT_DISCARD;
		present.Windowed	= FALSE;
		present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		present.MultiSampleType = D3DMULTISAMPLE_NONE;
		present.AutoDepthStencilFormat = D3DFMT_D24S8;
		present.DisableAutoBackBuffer = true;
		present.DisableAutoFrontBuffer = true;
		present.FrontBufferFormat = D3DFMT_LE_X8R8G8B8;
	}

	// create device
	{
		hr = pID3D->CreateDevice(adapter, devtype, hwnd,
				D3DCREATE_BUFFER_2_FRAMES, &present, &pID3DDevice);

		if (FAILED(hr))
			os::Printer::log("Was not able to create Direct3D9 device.", ELL_ERROR);

	}

	if (!pID3DDevice)
	{
		os::Printer::log("Was not able to create DIRECT3D9 device.", ELL_ERROR);
		return false;
	}
	//create render target in EDRAW
	{
		StencilBuffer = true;
		// Set up tiling front buffer texture.
		// These are the size of your desired rendering surface (in this case, the whole
		// screen).
		// We also create a scene resolve texture here, which will be used to hold the
		// results of predicated tiling.  If we were tiling directly to the front buffer,
		// we would require 2 front buffers used on even-odd frames, to avoid visual
		// tearing.
		if(m_pFrontBuffer[0] == NULL)
		{
			hr = pID3DDevice->CreateTexture( present.BackBufferWidth, present.BackBufferHeight,
									 1, 0, 
									 present.FrontBufferFormat,
									 D3DPOOL_DEFAULT, &m_pFrontBuffer[0], NULL );
			if (FAILED(hr))
				os::Printer::log("[1]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		if(m_pFrontBuffer[1] == NULL)
		{
			hr = pID3DDevice->CreateTexture( present.BackBufferWidth, present.BackBufferHeight,
									 1, 0, 
									 present.FrontBufferFormat,
									 D3DPOOL_DEFAULT, &m_pFrontBuffer[1], NULL );
			if (FAILED(hr))
				os::Printer::log("[2]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		D3DPOINT LargestTileSize;
		LargestTileRectSize( *m_pCurrentScenario, &LargestTileSize );
		// Create color and depth/stencil rendertargets.
		// These rendertargets are where Predicated Tiling will render each tile.  Therefore,
		// these rendertargets should be set up with all rendering quality settings you desire,
		// such as multisample antialiasing.
		// Note how we use the dimension of the largest tile rectangle to define how big the
		// rendertargets are.
		DWORD dwTileWidth = 0;
		DWORD dwTileHeight = 0;
		switch( m_pCurrentScenario->MSAAType )
		{
			case D3DMULTISAMPLE_NONE:
				dwTileWidth = XGNextMultiple( LargestTileSize.x, GPU_EDRAM_TILE_WIDTH_1X );
				dwTileHeight = XGNextMultiple( LargestTileSize.y, GPU_EDRAM_TILE_HEIGHT_1X );
				break;
			case D3DMULTISAMPLE_2_SAMPLES:
				dwTileWidth = XGNextMultiple( LargestTileSize.x, GPU_EDRAM_TILE_WIDTH_2X );
				dwTileHeight = XGNextMultiple( LargestTileSize.y, GPU_EDRAM_TILE_HEIGHT_2X );
				break;
			case D3DMULTISAMPLE_4_SAMPLES:
				dwTileWidth = XGNextMultiple( LargestTileSize.x, GPU_EDRAM_TILE_WIDTH_4X );
				dwTileHeight = XGNextMultiple( LargestTileSize.y, GPU_EDRAM_TILE_HEIGHT_4X );
				break;
		}
		 if( m_pCurrentScenario->dwTileCount > 1 )
		{
			// Expand tile surface dimensions to texture tile size, if it isn't already
			dwTileWidth = XGNextMultiple( dwTileWidth, GPU_TEXTURE_TILE_DIMENSION );
			dwTileHeight = XGNextMultiple( dwTileHeight, GPU_TEXTURE_TILE_DIMENSION );
		}

		// Use custom EDRAM allocation to create the rendertargets.
		// The color rendertarget is placed at address 0 in EDRAM.
		D3DSURFACE_PARAMETERS SurfaceParams;
		memset( &SurfaceParams, 0, sizeof( D3DSURFACE_PARAMETERS ) );
		SurfaceParams.Base = 0;
		if(m_pTilingRenderTarget[0] == NULL)
		{
			hr = pID3DDevice->CreateRenderTarget( dwTileWidth,
												   dwTileHeight,
												   ( D3DFORMAT )MAKESRGBFMT( D3DFMT_A8R8G8B8 ),
												   m_pCurrentScenario->MSAAType,
												   0, FALSE,
												   &m_pTilingRenderTarget[0],
												   &SurfaceParams );
			if (FAILED(hr))
				os::Printer::log("[1]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		if(m_pTilingRenderTarget[1] == NULL)
		{
			hr = pID3DDevice->CreateRenderTarget( dwTileWidth,
												   dwTileHeight,
												   ( D3DFORMAT )MAKESRGBFMT( D3DFMT_A8R8G8B8 ),
												   m_pCurrentScenario->MSAAType,
												   0, FALSE,
												   &m_pTilingRenderTarget[1],
												   &SurfaceParams );
			if (FAILED(hr))
				os::Printer::log("[2]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		// Record the size of the created rendertarget, and then set up allocation
		// for the next rendertarget right after the end of the first rendertarget.
		// Put the hierarchical Z buffer at the start of hierarchical Z memory.
		


		if(m_pTilingDepthStencil[0] == NULL)
		{
			SurfaceParams.Base = m_pTilingRenderTarget[0]->Size / GPU_EDRAM_TILE_SIZE;
			SurfaceParams.HierarchicalZBase = 0;
			hr = pID3DDevice->CreateDepthStencilSurface( dwTileWidth,
														  dwTileHeight,
														  D3DFMT_D24S8,
														  m_pCurrentScenario->MSAAType,
														  0, FALSE,
														  &m_pTilingDepthStencil[0],
														  &SurfaceParams );
			if (FAILED(hr))
				os::Printer::log("[1]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		if(m_pTilingDepthStencil[1] == NULL)
		{
			SurfaceParams.Base = m_pTilingRenderTarget[1]->Size / GPU_EDRAM_TILE_SIZE;
			SurfaceParams.HierarchicalZBase = 0;
			hr = pID3DDevice->CreateDepthStencilSurface( dwTileWidth,
														  dwTileHeight,
														  D3DFMT_D24S8,
														  m_pCurrentScenario->MSAAType,
														  0, FALSE,
														  &m_pTilingDepthStencil[1],
														  &SurfaceParams );
			if (FAILED(hr))
				os::Printer::log("[2]Failed to create render target in EDRAW.", ELL_ERROR);
		}
		// Set up the screen extents query mode that tiling will use.
		pID3DDevice->SetScreenExtentQueryMode( m_pCurrentScenario->ScreenExtentQueryMode );
	}
	// get caps
	pID3DDevice->GetDeviceCaps(&Caps);

	// disable stencilbuffer if necessary
	if (StencilBuffer &&
		(!(Caps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
	{
		os::Printer::log("Device not able to use stencil buffer, disabling stencil buffer.", ELL_WARNING);
		StencilBuffer = false;
	}

	// set default vertex shader
	setVertexShader(EVT_STANDARD);

	// enable antialiasing
	if (antiAlias)
		pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// set exposed data
	ExposedData.D3D9.D3D9 = pID3D;
	ExposedData.D3D9.D3DDev9 = pID3DDevice;
	ExposedData.D3D9.HWnd = hwnd;

	ResetRenderStates = true;

	// create materials
	createMaterialRenderers();

	MaxTextureUnits = core::min_((u32)Caps.MaxSimultaneousTextures, MATERIAL_MAX_TEXTURES);
	MaxUserClipPlanes = (u32)Caps.MaxUserClipPlanes;

	// set the renderstates
	setRenderStates3DMode();

	// set maximal anisotropy
	pID3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, min(16ul, Caps.MaxAnisotropy));
	pID3DDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, min(16ul, Caps.MaxAnisotropy));
	pID3DDevice->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, min(16ul, Caps.MaxAnisotropy));
	pID3DDevice->SetSamplerState(3, D3DSAMP_MAXANISOTROPY, min(16ul, Caps.MaxAnisotropy));

	// so far so good.
	return true;
}

bool
CXBOX360D3D9Driver::beginScene()
{
	CNullDriver::beginScene();
	//WindowId = windowId;
	//SceneSourceRect = sourceRect;

	if (!pID3DDevice)
		return false;
	HRESULT hr;
	// Set the back and stencil buffers and clear both
	hr = pID3DDevice->SetRenderTarget( 0, m_pTilingRenderTarget[m_nCurrentRenderFrontBuffer] );
	hr = pID3DDevice->SetDepthStencilSurface( m_pTilingDepthStencil[m_nCurrentRenderFrontBuffer] );


	BOOL bNeedTiling = ( m_pCurrentScenario->dwTileCount > 1 );
	if( bNeedTiling )
	{
	   // Begin tiling.  We specify the tile count, the tiling rectangles, and the
	   // color and Z/stencil values to clear the tiling rendertarget with for each tile.
		SColorf cf(ClearColor);
		m_ClearColor = *(D3DVECTOR4*)&cf;
	   pID3DDevice->BeginTiling(
		   m_pCurrentScenario->dwTilingFlags,
		   m_pCurrentScenario->dwTileCount,
		   m_pCurrentScenario->TilingRects,
		   &m_ClearColor, ClearDepth, ClearStencil );
	}
	else
	{
		//TODO:[xbox]: ASSERT(0) about Clear
		_IRR_DEBUG_BREAK_IF(0);
	   // We're not tiling in this scenario.  Just clear the rendertargets.
		//hr = pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
		//if (FAILED(hr))
		//	os::Printer::log("DIRECT3D9 clear failed.", ELL_WARNING);

	}
	// Begin Z pass.  The Z pass is a subset of the full scene rendering.
	pID3DDevice->BeginZPass( 0 );

	return true;
}


bool
CXBOX360D3D9Driver::swapBuffers(int)
{
	HRESULT hr = S_OK;



	return false;
}

void
CXBOX360D3D9Driver::clearBuffers(int clearMask)
{
	DWORD flags = 0;

	if (clearMask & EFB_COLOR)
		flags |= D3DCLEAR_TARGET;

	if (clearMask & EFB_DEPTH)
		flags |= D3DCLEAR_ZBUFFER;

	if (clearMask & EFB_STENCIL)
		flags |= D3DCLEAR_STENCIL;


	HRESULT hr = pID3DDevice->Clear( 0, NULL, flags, ClearColor.toARGB8(), ClearDepth, ClearStencil);
	if (FAILED(hr))
		os::Printer::log("DIRECT3D9 clear failed.", ELL_WARNING);
}

void
CXBOX360D3D9Driver::setClearColor(const SColor& color)
{
	ClearColor = color;
}

SColor
CXBOX360D3D9Driver::getClearColor() const
{
	return ClearColor;
}

void
CXBOX360D3D9Driver::setClearDepth(float depth)
{
	ClearDepth = depth;
}

float
CXBOX360D3D9Driver::getClearDepth() const
{
	return ClearDepth;
}

void
CXBOX360D3D9Driver::setClearStencil(int stencil)
{
	ClearStencil = stencil;
}

int
CXBOX360D3D9Driver::getClearStencil() const
{
	return ClearStencil;
}

//
////! applications must call this method before performing any rendering. returns false if failed.
//bool CXBOX360D3D9Driver::beginScene(bool backBuffer, bool zBuffer, SColor color,
//		void* windowId, core::rect<s32>* sourceRect)
//{
//	CNullDriver::beginScene(backBuffer, zBuffer, color, windowId, sourceRect);
//	WindowId = windowId;
//	SceneSourceRect = sourceRect;
//
//	if (!pID3DDevice)
//		return false;
//	HRESULT hr;
//	DWORD flags = 0;
//    // Set the back and stencil buffers and clear both
//    hr = pID3DDevice->SetRenderTarget( 0, m_pTilingRenderTarget[m_nCurrentRenderFrontBuffer] );
//    hr = pID3DDevice->SetDepthStencilSurface( m_pTilingDepthStencil[m_nCurrentRenderFrontBuffer] );
//
//
//	if (backBuffer)
//		flags |= D3DCLEAR_TARGET;
//
//	if (zBuffer)
//		flags |= D3DCLEAR_ZBUFFER;
//
//	if (StencilBuffer)
//		flags |= D3DCLEAR_STENCIL;
//
//    BOOL bNeedTiling = ( m_pCurrentScenario->dwTileCount > 1 );
//    if( bNeedTiling )
//    {
//        // Begin tiling.  We specify the tile count, the tiling rectangles, and the
//        // color and Z/stencil values to clear the tiling rendertarget with for each tile.
//		SColorf cf(color);
//
//		m_ClearColor = *(D3DVECTOR4*)&cf;
//        pID3DDevice->BeginTiling(
//            m_pCurrentScenario->dwTilingFlags,
//            m_pCurrentScenario->dwTileCount,
//            m_pCurrentScenario->TilingRects,
//            &m_ClearColor, 1.0f, 0L );
//    }
//    else
//    {
//        // We're not tiling in this scenario.  Just clear the rendertargets.
//		hr = pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
//		if (FAILED(hr))
//			os::Printer::log("DIRECT3D9 clear failed.", ELL_WARNING);
//
//    }
//    // Begin Z pass.  The Z pass is a subset of the full scene rendering.
//    pID3DDevice->BeginZPass( 0 );
//
//	//hr = pID3DDevice->BeginScene();
//	//if (FAILED(hr))
//	//{
//	//	os::Printer::log("DIRECT3D9 begin scene failed.", ELL_WARNING);
//	//	return false;
//	//}
//
//	return true;
//}


//! applications must call this method after performing any rendering. returns false if failed.
bool CXBOX360D3D9Driver::endScene()
{
	HRESULT hr;
	CNullDriver::endScene();
	DriverWasReset=false;
	BOOL bNeedTiling = ( m_pCurrentScenario->dwTileCount > 1 );
	// End Z pass.
    pID3DDevice->EndZPass();

    if( bNeedTiling )
    {
        // End tiling.  This will cause the accumulated command buffer to be replayed
        // for each tile, predicated upon the boundaries of each tile.  At the end of
        // each tile's rendering, it will be resolved into a section of the front buffer
        // texture.
        pID3DDevice->EndTiling( D3DRESOLVE_RENDERTARGET0 |
                                 D3DRESOLVE_ALLFRAGMENTS |
                                 D3DRESOLVE_CLEARRENDERTARGET |
                                 D3DRESOLVE_CLEARDEPTHSTENCIL,
                                 NULL, 
								 m_pFrontBuffer[m_nCurrentRenderFrontBuffer],//m_pSceneResolveTexture,
                                 &m_ClearColor, 1.0f, 0L, NULL );
    }
    else
    {
        // We're not tiling in this scenario.  Just resolve rendertarget 0 to the
        // scene resolve texture.
        pID3DDevice->Resolve( D3DRESOLVE_RENDERTARGET0 |
                               D3DRESOLVE_CLEARRENDERTARGET,
                               NULL,
                               m_pFrontBuffer[m_nCurrentRenderFrontBuffer], //m_pSceneResolveTexture,
                               NULL, 0, 0,
                               &m_ClearColor, 1.0f, 0, NULL );
    }

    // Wait for the vertical blank before we resolve to the front buffer to avoid tearing.
    if(SUCCEEDED(hr = pID3DDevice->SynchronizeToPresentationInterval()))
	{
		//// Resolve back buffer to front buffer
		//if(SUCCEEDED(hr = pID3DDevice->Resolve( D3DRESOLVE_RENDERTARGET0,
		//					   NULL, m_pFrontBuffer, NULL, 0, NULL, NULL, 0.0f, 0, NULL )))
		{

			// Present the scene
			hr = pID3DDevice->Swap( m_pFrontBuffer[m_nCurrentRenderFrontBuffer], NULL );
			m_nCurrentRenderFrontBuffer = m_nCurrentRenderFrontBuffer == 0? 1:0;
		}
	}
	if (SUCCEEDED(hr))
	{
		return true;
		////TODO:[xbox]:endscene()
		//HRESULT hr = pID3DDevice->EndScene();
		//if (FAILED(hr))
		//{
		//	os::Printer::log("DIRECT3D9 end scene failed.", ELL_WARNING);
		//	return false;
		//}
		//else
		//{
		//	return true;
		//}

	}

	if (hr == D3DERR_DEVICELOST)
	{
		DeviceLost = true;
		os::Printer::log("Present failed", "DIRECT3D9 device lost.", ELL_WARNING);
	}
	else if (hr == D3DERR_INVALIDCALL)
	{
		os::Printer::log("Present failed", "Invalid Call", ELL_WARNING);
	}
	else
		os::Printer::log("DIRECT3D9 present failed.", ELL_WARNING);


	return true;
}


//! queries the features of the driver, returns true if feature is available
bool CXBOX360D3D9Driver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	if (!FeatureEnabled[feature])
		return false;

	switch (feature)
	{
	case EVDF_MULTITEXTURE:
	case EVDF_BILINEAR_FILTER:
		return true;
	case EVDF_RENDER_TO_TARGET:
		return Caps.NumSimultaneousRTs > 0;
	case EVDF_HARDWARE_TL:
		return (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
	case EVDF_MIP_MAP:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
	case EVDF_MIP_MAP_AUTO_UPDATE:
		// always return false because a lot of drivers claim they do
		// this but actually don't do this at all.
		return false; //(Caps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer && Caps.StencilCaps;
	case EVDF_VERTEX_SHADER_1_1:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_VERTEX_SHADER_2_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case EVDF_VERTEX_SHADER_3_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(3,0);
	case EVDF_PIXEL_SHADER_1_1:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_PIXEL_SHADER_1_2:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,2);
	case EVDF_PIXEL_SHADER_1_3:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,3);
	case EVDF_PIXEL_SHADER_1_4:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,4);
	case EVDF_PIXEL_SHADER_2_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case EVDF_PIXEL_SHADER_3_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(3,0);
	case EVDF_HLSL:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_TEXTURE_NSQUARE:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) == 0;
	case EVDF_TEXTURE_NPOT:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
	default:
		return false;
	};
}


//! sets transformation
void CXBOX360D3D9Driver::setTransform(E_TRANSFORMATION_STATE state,
		const core::matrix4& mat)
{
	Transformation3DChanged = true;
	Matrices[state] = mat;
	SetWVP();
	SetTextureMatrix(0, Matrices[ETS_TEXTURE_0]);
	SetTextureMatrix(1, Matrices[ETS_TEXTURE_1]);
	SetTextureMatrix(2, Matrices[ETS_TEXTURE_2]);
	SetTextureMatrix(3, Matrices[ETS_TEXTURE_3]);	
}


//! sets the current Texture
bool CXBOX360D3D9Driver::setTexture(s32 stage, const video::ITexture* texture)
{
	if (CurrentTexture[stage] == texture)
		return true;

	if (CurrentTexture[stage] == texture)
	{
		// will still must check if the texture's state is dirty in case only
		// parameters have changed
		//if (texture && texture->isDirty())
		if (texture)//TODO:[D3D9]:set filter depending on stage not on texture filter
		{
			// should not happen due to a condition farther below
			_IRR_DEBUG_BREAK_IF(texture->getDriverType() != getDriverType());
			static_cast<const CXBOX360D3D9Texture*>(texture)->updateParameters(pID3DDevice, stage);
		}
		return true;
	}

	if (texture && texture->getDriverType() != EDT_DIRECT3D9)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	CurrentTexture[stage] = texture;

	if (!texture)
	{
		pID3DDevice->SetTexture(stage, 0);
	}
	else
	{
		pID3DDevice->SetTexture(stage, ((const CXBOX360D3D9Texture*)texture)->getDX9Texture());
		_IRR_DEBUG_BREAK_IF(texture->getDriverType() != getDriverType());
		static_cast<const CXBOX360D3D9Texture*>(texture)->updateParameters(pID3DDevice, stage);
	}
	return true;
}


//! sets a material
void CXBOX360D3D9Driver::setMaterial(const SMaterial& material)
{
	Material = material;

	for (u32 i=0; i<MaxTextureUnits; ++i)
	{
		setTexture(i, Material.getTexture(i));
		setTransform((E_TRANSFORMATION_STATE) ( ETS_TEXTURE_0 + i ),
				material.getTextureMatrix(i));
	}
}


//! returns a device dependent texture from a software surface (IImage)
video::ITexture* CXBOX360D3D9Driver::createDeviceDependentTexture(IImage* surface,
		const char* name)
{
	return irrnew CXBOX360D3D9Texture(surface, this, irr::video::ETCF_OPTIMIZED_FOR_QUALITY, name);
}


//! Enables or disables a texture creation flag.
void CXBOX360D3D9Driver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag,
		bool enabled)
{
	if (flag == video::ETCF_CREATE_MIP_MAPS && !queryFeature(EVDF_MIP_MAP))
		enabled = false;

	CNullDriver::setTextureCreationFlag(flag, enabled);
}


//! sets a render target
bool CXBOX360D3D9Driver::setRenderTarget(video::ITexture* texture,
		bool clearBackBuffer, bool clearZBuffer, SColor color)
{
	// check for right driver type

	if (texture && texture->getDriverType() != EDT_DIRECT3D9)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return false;
	}

	// check for valid render target

	CXBOX360D3D9Texture* tex = (CXBOX360D3D9Texture*)texture;

	if (texture && !tex->isRenderTarget())
	{
		os::Printer::log("Fatal Error: Tried to set a non render target texture as render target.", ELL_ERROR);
		return false;
	}

	if (texture && (tex->getSize().Width > CurrentDepthBufferSize.Width ||
		tex->getSize().Height > CurrentDepthBufferSize.Height))
	{
		os::Printer::log("Error: Tried to set a render target texture which is bigger than the depth buffer.", ELL_ERROR);
		return false;
	}

	// check if we should set the previous RT back

	bool ret = true;

	if (tex == 0)
	{
		if (PrevRenderTarget)
		{
			if (FAILED(pID3DDevice->SetRenderTarget(0, PrevRenderTarget)))
			{
				os::Printer::log("Error: Could not set back to previous render target.", ELL_ERROR);
				ret = false;
			}

			CurrentRendertargetSize = core::dimension2d<s32>(0,0);
			PrevRenderTarget->Release();
			PrevRenderTarget = 0;
		}
	}
	else
	{
		// we want to set a new target. so do this.

		// store previous target

		if (!PrevRenderTarget)
			if (FAILED(pID3DDevice->GetRenderTarget(0, &PrevRenderTarget)))
			{
				os::Printer::log("Could not get previous render target.", ELL_ERROR);
				return false;
			}

		// set new render target

		if (FAILED(pID3DDevice->SetRenderTarget(0, tex->getRenderTargetSurface())))
		{
			os::Printer::log("Error: Could not set render target.", ELL_ERROR);
			return false;
		}

		CurrentRendertargetSize = tex->getSize();
	}

	if (clearBackBuffer || clearZBuffer)
	{
		DWORD flags = 0;

		if (clearBackBuffer)
			flags |= D3DCLEAR_TARGET;

		if (clearZBuffer)
			flags |= D3DCLEAR_ZBUFFER;

		pID3DDevice->Clear(0, NULL, flags, color.toARGB8(), ClearDepth, ClearStencil);
	}

	return ret;
}


//! sets a viewport
void CXBOX360D3D9Driver::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	D3DVIEWPORT9 viewPort;
	viewPort.X = vp.UpperLeftCorner.X;
	viewPort.Y = vp.UpperLeftCorner.Y;
	viewPort.Width = vp.getWidth();
	viewPort.Height = vp.getHeight();
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;

	HRESULT hr = D3DERR_INVALIDCALL;
	if (vp.getHeight()>0 && vp.getWidth()>0)
		hr = pID3DDevice->SetViewport(&viewPort);

	if (FAILED(hr))
		os::Printer::log("Failed setting the viewport.", ELL_WARNING);

	ViewPort = vp;
}


//! gets the area of the current viewport
const core::rect<s32>& CXBOX360D3D9Driver::getViewPort() const
{
	return ViewPort;
}


bool CXBOX360D3D9Driver::updateVertexHardwareBuffer(SHWBufferLink_d3d9 *HWBuffer)
{
	if (!HWBuffer)
		return false;

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const void* vertices=mb->getVertices();
	const u32 vertexCount=mb->getVertexCount();
	const E_VERTEX_TYPE vType=mb->getVertexType();
	const u32 vertexSize = getVertexPitchFromType(vType);

	void* pLockedBuffer = 0;

	if (!HWBuffer->vertexBuffer || vertexSize * vertexCount > HWBuffer->vertexBufferSize)
	{
		DWORD flags = 0;

		u32 vertexSize;
		DWORD FVF;

		// Get the vertex sizes and cvf
		switch (vType)
		{
			case EVT_STANDARD:
				vertexSize = sizeof(S3DVertex);
				FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
				break;
			case EVT_2TCOORDS:
				vertexSize = sizeof(S3DVertex2TCoords);
				FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2;
				break;
			case EVT_TANGENTS:
				vertexSize = sizeof(S3DVertexTangents);
				FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3;
				break;
			default:
				return false;
		}


#ifndef _IRR_XBOX_PLATFORM_
		flags = D3DUSAGE_WRITEONLY; // SIO2: Default to D3DUSAGE_WRITEONLY
		if(HWBuffer->Mapped_Vertex != scene::EHM_STATIC)
			flags |= D3DUSAGE_DYNAMIC;
#else

		//TODO:XBOX 360 Review:need updateVertexHardwareBuffer
		flags = D3DUSAGE_CPU_CACHED_MEMORY;
#endif
		pID3DDevice->CreateVertexBuffer(vertexCount * vertexSize, flags, FVF, D3DPOOL_DEFAULT, &HWBuffer->vertexBuffer, NULL);

		if(!HWBuffer->vertexBuffer)
			return false;

#ifndef _IRR_XBOX_PLATFORM_
		flags = 0; // SIO2: Reset flags before Lock
		if(HWBuffer->Mapped_Vertex != scene::EHM_STATIC)
			flags = D3DLOCK_DISCARD;
#endif
		HWBuffer->vertexBuffer->Lock(0, vertexCount * vertexSize, (void**)&pLockedBuffer, flags);
		memcpy(pLockedBuffer, vertices, vertexCount * vertexSize);
		HWBuffer->vertexBuffer->Unlock();

		HWBuffer->vertexBufferSize = vertexCount * vertexSize;
	}
	else
	{
#ifndef _IRR_XBOX_PLATFORM_
		HWBuffer->vertexBuffer->Lock(0, vertexCount * vertexSize, (void**)&pLockedBuffer, D3DLOCK_DISCARD);
#else
		//TODO:XBOX 360 Review:need updateVertexHardwareBuffer
		HWBuffer->vertexBuffer->Lock(0, vertexCount * vertexSize, (void**)&pLockedBuffer, D3DUSAGE_CPU_CACHED_MEMORY);
#endif
		memcpy(pLockedBuffer, vertices, vertexCount * vertexSize);
		HWBuffer->vertexBuffer->Unlock();
	}

	return true;
}


bool CXBOX360D3D9Driver::updateIndexHardwareBuffer(SHWBufferLink_d3d9 *HWBuffer)
{
	if (!HWBuffer)
		return false;

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const u16* indices=mb->getIndices();
	const u32 indexCount=mb->getIndexCount();
	u32 indexSize = 2;
	D3DFORMAT indexType=D3DFMT_UNKNOWN;
	switch (mb->getIndexType())
	{
		case (EIT_16BIT):
		{
			indexType=D3DFMT_INDEX16;
			indexSize = 2;
			break;
		}
		case (EIT_32BIT):
		{
			indexType=D3DFMT_INDEX32;
			indexSize = 4;
			break;
		}
	}

	if (!HWBuffer->indexBuffer || indexSize * indexCount > HWBuffer->indexBufferSize)
	{
		DWORD flags = 0;

#ifndef _IRR_XBOX_PLATFORM_
		flags = D3DUSAGE_WRITEONLY; // SIO2: Default to D3DUSAGE_WRITEONLY
		if(HWBuffer->Mapped_Index != scene::EHM_STATIC)
			flags |= D3DUSAGE_DYNAMIC; // SIO2: Add DYNAMIC flag for dynamic buffer data
#else

		//TODO:XBOX 360 Review:need updateIndexHardwareBuffer
		flags = D3DUSAGE_CPU_CACHED_MEMORY;
#endif

		if(FAILED(pID3DDevice->CreateIndexBuffer( indexCount * indexSize, flags, indexType, D3DPOOL_DEFAULT, &HWBuffer->indexBuffer, NULL)))
			return false;

		void* pIndices = 0;
#ifndef _IRR_XBOX_PLATFORM_
		flags = 0; // SIO2: Reset flags before Lock
		if(HWBuffer->Mapped_Index != scene::EHM_STATIC)
			flags = D3DLOCK_DISCARD;
#endif
		if(FAILED(HWBuffer->indexBuffer->Lock( 0, 0, (void**)&pIndices, flags)))
			return false;

		memcpy(pIndices, indices, indexCount * indexSize);
		HWBuffer->indexBuffer->Unlock();

		HWBuffer->indexBufferSize = indexCount * indexSize;
	}
	else
	{
		void* pIndices = 0;
#ifndef _IRR_XBOX_PLATFORM_
		if( SUCCEEDED(HWBuffer->indexBuffer->Lock( 0, 0, (void**)&pIndices, D3DLOCK_DISCARD)))
#else
		if( SUCCEEDED(HWBuffer->indexBuffer->Lock( 0, 0, (void**)&pIndices, D3DUSAGE_CPU_CACHED_MEMORY)))
#endif
		{
			memcpy(pIndices, indices, indexCount * indexSize);
			HWBuffer->indexBuffer->Unlock();
		}
	}

	return true;
}


//! updates hardware buffer if needed
bool CXBOX360D3D9Driver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (HWBuffer->Mapped_Vertex!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Vertex != HWBuffer->MeshBuffer->getChangedID_Vertex()
			|| !((SHWBufferLink_d3d9*)HWBuffer)->vertexBuffer)
		{

			HWBuffer->ChangedID_Vertex = HWBuffer->MeshBuffer->getChangedID_Vertex();

			if (!updateVertexHardwareBuffer((SHWBufferLink_d3d9*)HWBuffer))
				return false;
		}
	}

	if (HWBuffer->Mapped_Index!=scene::EHM_NEVER)
	{
		if (HWBuffer->ChangedID_Index != HWBuffer->MeshBuffer->getChangedID_Index()
			|| !((SHWBufferLink_d3d9*)HWBuffer)->indexBuffer)
		{

			HWBuffer->ChangedID_Index = HWBuffer->MeshBuffer->getChangedID_Index();

			if (!updateIndexHardwareBuffer((SHWBufferLink_d3d9*)HWBuffer))
				return false;
		}
	}

	return true;
}


//! Create hardware buffer from meshbuffer
CXBOX360D3D9Driver::SHWBufferLink *CXBOX360D3D9Driver::createHardwareBuffer(const scene::IMeshBuffer* mb)
{
	if (!mb || (mb->getHardwareMappingHint_Index()==scene::EHM_NEVER && mb->getHardwareMappingHint_Vertex()==scene::EHM_NEVER))
		return 0;

	SHWBufferLink_d3d9 *HWBuffer=irrnew SHWBufferLink_d3d9(mb);

	//add to map
	HWBufferMap.insert(HWBuffer->MeshBuffer, HWBuffer);

	HWBuffer->ChangedID_Vertex=HWBuffer->MeshBuffer->getChangedID_Vertex();
	HWBuffer->ChangedID_Index=HWBuffer->MeshBuffer->getChangedID_Index();
	HWBuffer->Mapped_Vertex=mb->getHardwareMappingHint_Vertex();
	HWBuffer->Mapped_Index=mb->getHardwareMappingHint_Index();
	HWBuffer->LastUsed=0;
	HWBuffer->vertexBuffer=0;
	HWBuffer->indexBuffer=0;
	HWBuffer->vertexBufferSize=0;
	HWBuffer->indexBufferSize=0;

	if (!updateHardwareBuffer(HWBuffer))
	{
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	return HWBuffer;
}


void CXBOX360D3D9Driver::deleteHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer) return;

	SHWBufferLink_d3d9 *HWBuffer=(SHWBufferLink_d3d9*)_HWBuffer;
	if (HWBuffer->indexBuffer)
	{
		HWBuffer->indexBuffer->Release();
		HWBuffer->indexBuffer = 0;
	}

	if (HWBuffer->vertexBuffer)
	{
		HWBuffer->vertexBuffer->Release();
		HWBuffer->vertexBuffer = 0;
	}

	CNullDriver::deleteHardwareBuffer(_HWBuffer);
}


//! Draw hardware buffer
void CXBOX360D3D9Driver::drawHardwareBuffer(SHWBufferLink *_HWBuffer)
{
	if (!_HWBuffer)
		return;

	SHWBufferLink_d3d9 *HWBuffer=(SHWBufferLink_d3d9*)_HWBuffer;

	updateHardwareBuffer(HWBuffer); //check if update is needed

	HWBuffer->LastUsed=0;//reset count

	const scene::IMeshBuffer* mb = HWBuffer->MeshBuffer;
	const E_VERTEX_TYPE vType = mb->getVertexType();
	const u32 stride = getVertexPitchFromType(vType);
	if (HWBuffer->vertexBuffer) pID3DDevice->SetStreamSource(0, HWBuffer->vertexBuffer, 0, stride);
	if (HWBuffer->indexBuffer) pID3DDevice->SetIndices(HWBuffer->indexBuffer);

	//TODO:[xbox]: double check it
	drawVertexPrimitiveList(0, 0, 0, mb->getVertexCount(), mb->getIndexCount()/3, mb->getVertexType(), scene::EPT_TRIANGLES, mb->getIndexType());

	if (HWBuffer->vertexBuffer)
		pID3DDevice->SetStreamSource(0, 0, 0, 0);
	if (HWBuffer->indexBuffer)
		pID3DDevice->SetIndices(0);
}


//! draws a vertex primitive list
void CXBOX360D3D9Driver::drawVertexPrimitiveList(const void* vertices,
									 const void* indexList,
									 u32 minIndex,
									 u32 maxIndex,
									 u32 primitiveCount,
									 E_VERTEX_TYPE vType,
									 scene::E_PRIMITIVE_TYPE pType,
									 E_INDEX_TYPE iType,
									 IDriverBinding** binding)
{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, indexList, minIndex , maxIndex, primitiveCount, vType, pType,iType, binding);
	int vertexCount = maxIndex-minIndex;
	if (!vertexCount || !primitiveCount)
		return;

	setVertexShader(vType);

	const u32 stride = getVertexPitchFromType(vType);


	D3DFORMAT indexType=D3DFMT_UNKNOWN;



	switch (iType)
	{
		case (EIT_16BIT):
		{
			indexType=D3DFMT_INDEX16;
			break;
		}
		case (EIT_32BIT):
		{
			indexType=D3DFMT_INDEX32;
			break;
		}
	}

	if (setRenderStates3DMode())
	{
		switch (pType)
		{
			case scene::EPT_POINT_SPRITES:
			case scene::EPT_POINTS:
			{
#ifndef _IRR_XBOX_PLATFORM_
				if (pType==scene::EPT_POINT_SPRITES)
					pID3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)(&Material.Thickness));
				f32 tmp=1.0f;
				pID3DDevice->SetRenderState(D3DRS_POINTSCALE_C, *(DWORD*)(&tmp));
				tmp=0.0f;
				pID3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *(DWORD*)(&tmp));
				pID3DDevice->SetRenderState(D3DRS_POINTSCALE_A, *(DWORD*)(&tmp));
				pID3DDevice->SetRenderState(D3DRS_POINTSCALE_B, *(DWORD*)(&tmp));

				if (!vertices)
				{
					pID3DDevice->DrawIndexedPrimitive(D3DPT_POINTLIST, 0, 0, vertexCount, 0, primitiveCount);
				}
				else
				{
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_POINTLIST, 0, vertexCount,
					primitiveCount, indexList, indexType, vertices, stride);
				}

				pID3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
				if (pType==scene::EPT_POINT_SPRITES)
					pID3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
#else
				//TODO:XBOX 360 Review:need, drawVertexPrimitiveList, when type is EPT_POINT_SPRITES or EPT_POINTS
#endif
			}
				break;
			case scene::EPT_LINE_STRIP:
				if(!vertices)
					pID3DDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, minIndex, vertexCount, 0, primitiveCount);
				else
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, minIndex, vertexCount,
						primitiveCount, indexList, indexType, vertices, stride);
				break;
			case scene::EPT_LINE_LOOP:
				if(!vertices)
				{
					// TODO: Implement proper hardware support for this primitive type.
					// (No looping occurs currently because this would require a way to
					// draw the hardware buffer with a custom set of indices. We may even
					// need to create a new mini index buffer specifically for this
					// primitive type.)
					pID3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, minIndex, vertexCount, 0, primitiveCount);
				}
				else
				{
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, minIndex, vertexCount,
					primitiveCount, indexList, indexType, vertices, stride);

					u16 tmpIndices[] = {0, primitiveCount};

					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, minIndex, vertexCount,
						1, tmpIndices, indexType, vertices, stride);
				}
				break;
			case scene::EPT_LINES:
				if(!vertices)
					pID3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, minIndex, vertexCount, 0, primitiveCount);
				else
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, vertexCount,
						primitiveCount, indexList, indexType, vertices, stride);
				break;
			case scene::EPT_TRIANGLE_STRIP:
				if(!vertices)
					pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, minIndex, vertexCount, 0, primitiveCount);
				else
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, minIndex, vertexCount, primitiveCount,
							indexList, indexType, vertices, stride);
				break;
			case scene::EPT_TRIANGLE_FAN:
				if(!vertices)
					pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, minIndex, vertexCount, 0, primitiveCount);
				else
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLEFAN, minIndex, vertexCount, primitiveCount,
							indexList, indexType, vertices, stride);
					break;
			case scene::EPT_TRIANGLES:
				if(!vertices)
				{
					pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, minIndex, vertexCount, 0, primitiveCount);
				}
				else
				{
					pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, minIndex, vertexCount,
						primitiveCount, indexList, indexType, vertices, stride);
				}
				break;
		}
	}
}


void CXBOX360D3D9Driver::draw2DImage(const video::ITexture* texture,
		const core::rect<s32>& destRect,
		const core::rect<s32>& sourceRect,
		const core::rect<s32>* clipRect,
		const video::SColor* const colors,
		bool useAlphaChannelOfTexture)
{
	if(!texture)
		return;

	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (f32)sourceRect.UpperLeftCorner.X / (f32)ss.Width;
	tcoords.UpperLeftCorner.Y = (f32)sourceRect.UpperLeftCorner.Y / (f32)ss.Height;
	tcoords.LowerRightCorner.X = (f32)sourceRect.LowerRightCorner.X / (f32)ss.Width;
	tcoords.LowerRightCorner.Y = (f32)sourceRect.LowerRightCorner.Y / (f32)ss.Height;
	//TODO:XBOX 360 Review:need draw2DImage
	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	core::rect<f32> npos;
	f32 xFact = 2.0f / ( renderTargetSize.Width );
	f32 yFact = 2.0f / ( renderTargetSize.Height );
	npos.UpperLeftCorner.X = ( destRect.UpperLeftCorner.X * xFact ) - 1.0f;
	npos.UpperLeftCorner.Y = 1.0f - ( destRect.UpperLeftCorner.Y * yFact );
	npos.LowerRightCorner.X = ( destRect.LowerRightCorner.X * xFact ) - 1.0f;
	npos.LowerRightCorner.Y = 1.0f - ( destRect.LowerRightCorner.Y * yFact );

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;

	S3DVertex vtx[4]; // clock wise
	vtx[0] = S3DVertex(npos.UpperLeftCorner.X, npos.UpperLeftCorner.Y, 0.0f,
			0.0f, 0.0f, 0.0f, useColor[0],
			tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[1] = S3DVertex(npos.LowerRightCorner.X, npos.UpperLeftCorner.Y, 0.0f,
			0.0f, 0.0f, 0.0f, useColor[3],
			tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[2] = S3DVertex(npos.LowerRightCorner.X, npos.LowerRightCorner.Y, 0.0f,
			0.0f, 0.0f, 0.0f, useColor[2],
			tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vtx[3] = S3DVertex(npos.UpperLeftCorner.X, npos.LowerRightCorner.Y, 0.0f,
			0.0f, 0.0f, 0.0f, useColor[1],
			tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	s16 indices[6] = {0,1,2,0,2,3};

	setRenderStates2DMode(useColor[0].getAlpha()<255 || useColor[1].getAlpha()<255 ||
			useColor[2].getAlpha()<255 || useColor[3].getAlpha()<255,
			true, useAlphaChannelOfTexture);

	setTexture(0, const_cast<video::ITexture*>(texture));

	setVertexShader(EVT_STANDARD);

	if (clipRect)
	{
		pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		RECT scissor;
		scissor.left = clipRect->UpperLeftCorner.X;
		scissor.top = clipRect->UpperLeftCorner.Y;
		scissor.right = clipRect->LowerRightCorner.X;
		scissor.bottom = clipRect->LowerRightCorner.Y;
		pID3DDevice->SetScissorRect(&scissor);
	}

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	setShaderState(true, true);
	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
				D3DFMT_INDEX16,&vtx[0], sizeof(S3DVertex));

	if (clipRect)
		pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}


//! draws a 2d image, using a color and the alpha channel of the texture if
//! desired. The image is drawn at pos and clipped against clipRect (if != 0).
void CXBOX360D3D9Driver::draw2DImage(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::rect<s32>& sourceRect,
				const core::rect<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	if (!setTexture(0, const_cast<video::ITexture*>(texture)))
		return;

	core::rect<s32> poss(pos, core::abs(sourceRect.getSize()));
	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	core::rect<f32> tcoords((sourceRect.UpperLeftCorner.X + 0.5f) * invW,
							(sourceRect.UpperLeftCorner.Y + 0.5f) * invH,
							(sourceRect.LowerRightCorner.X + 0.5) * invW,
							(sourceRect.LowerRightCorner.Y + 0.5f) * invH);

	if (clipRect && !clip(poss, tcoords, *clipRect))
	{
		return;
	}
#ifdef _IRR_ENABLE_DRAW2DIMAGE_MANUAL_VIEWPORT_CLIPPING_
	if (!clip(poss, tcoords, core::rect<s32>(core::position2d<s32>(0,0),
											 getCurrentRenderTargetSize())))
	{
		return;
	}
#endif

	// ok, we've clipped everything.
	// now draw it.
	core::dimension2d<s32> renderTargetSize = getCurrentRenderTargetSize();
	s32 xPlus = -renderTargetSize.Width / 2;
	f32 xFact = 2.0f / renderTargetSize.Width;

	s32 yPlus = renderTargetSize.Height / 2;
	f32 yFact = 2.0f / renderTargetSize.Height;

	setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

	S3DVertex vtx[4];
	vtx[0] = S3DVertex((f32)(poss.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-poss.UpperLeftCorner.Y ) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, color,
			tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[1] = S3DVertex((f32)(poss.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus- poss.UpperLeftCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, color,
			tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[2] = S3DVertex((f32)(poss.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus-poss.LowerRightCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, color,
			tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vtx[3] = S3DVertex((f32)(poss.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-poss.LowerRightCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, color,
			tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	s16 indices[6] = {0,1,2,0,2,3};

	setVertexShader(EVT_STANDARD);

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	setShaderState(true, true);
	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
		D3DFMT_INDEX16,&vtx[0],	sizeof(S3DVertex));
}


//!Draws a 2d rectangle with a gradient.
void CXBOX360D3D9Driver::draw2DRectangle(const core::rect<s32>& position,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const core::rect<s32>* clip)
{
	core::rect<s32> pos(position);

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();

	s32 xPlus = -renderTargetSize.Width / 2;
	f32 xFact = 2.0f / renderTargetSize.Width;

	s32 yPlus = renderTargetSize.Height / 2;
	f32 yFact = 2.0f / renderTargetSize.Height;

	S3DVertex vtx[4];
	vtx[0] = S3DVertex((f32)(pos.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-pos.UpperLeftCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, colorLeftUp, 0.0f, 0.0f);
	vtx[1] = S3DVertex((f32)(pos.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus- pos.UpperLeftCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, colorRightUp, 0.0f, 1.0f);
	vtx[2] = S3DVertex((f32)(pos.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus-pos.LowerRightCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, colorRightDown, 1.0f, 0.0f);
	vtx[3] = S3DVertex((f32)(pos.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-pos.LowerRightCorner.Y) * yFact, 0.0f,
			0.0f, 0.0f, 0.0f, colorLeftDown, 1.0f, 1.0f);

	s16 indices[6] = {0,1,2,0,2,3};

	setRenderStates2DMode(
		colorLeftUp.getAlpha() < 255 ||
		colorRightUp.getAlpha() < 255 ||
		colorLeftDown.getAlpha() < 255 ||
		colorRightDown.getAlpha() < 255, false, false);

	setTexture(0,0);

	setVertexShader(EVT_STANDARD);

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	setShaderState(true, false);
	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
		D3DFMT_INDEX16, &vtx[0], sizeof(S3DVertex));

}



//! Draws a 2d line.
void CXBOX360D3D9Driver::draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end,
				SColor color)
{
	// thanks to Vash TheStampede who sent in his implementation

	const core::dimension2d<s32>& renderTargetSize = getCurrentRenderTargetSize();
	const s32 xPlus = -renderTargetSize.Width / 2;
	const f32 xFact = 2.0f / renderTargetSize.Width;

	const s32 yPlus = renderTargetSize.Height / 2;
	const f32 yFact = 2.0f / renderTargetSize.Height;

	S3DVertex vtx[2];
	vtx[0] = S3DVertex((f32)(start.X + xPlus) * xFact,
						(f32)(yPlus - start.Y) * yFact,
						0.0f, // z
						0.0f, 0.0f, 0.0f, // normal
						color,
						0.0f, 0.0f); // texture

	vtx[1] = S3DVertex((f32)(end.X+xPlus) * xFact,
						(f32)(yPlus- end.Y) * yFact,
						0.0f,
						0.0f, 0.0f, 0.0f,
						color,
						0.0f, 0.0f);

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	setTexture(0,0);

	setVertexShader(EVT_STANDARD);

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1,
					&vtx[0], sizeof(S3DVertex) );
}



//! sets right vertex shader
void CXBOX360D3D9Driver::setVertexShader(E_VERTEX_TYPE newType)
{
	if (newType != LastVertexType)
	{
		LastVertexType = newType;
		HRESULT hr = 0;

		switch(newType)
		{
		case EVT_STANDARD:
			hr = pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			break;
		case EVT_2TCOORDS:
			hr = pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);
			break;
		case EVT_TANGENTS:
			hr = pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 |
				D3DFVF_TEXCOORDSIZE2(0) | // real texture coord
				D3DFVF_TEXCOORDSIZE3(1) | // misuse texture coord 2 for tangent
				D3DFVF_TEXCOORDSIZE3(2) // misuse texture coord 3 for binormal
				);
			break;
		}

		if (FAILED(hr))
		{
			os::Printer::log("Could not set vertex Shader.", ELL_ERROR);
			return;
		}
	}
}



//! sets the needed renderstates
bool CXBOX360D3D9Driver::setRenderStates3DMode()
{
	if (!pID3DDevice)
		return false;

	if (CurrentRenderMode != ERM_3D)
	{
#ifndef _IRR_XBOX_PLATFORM_
		// switch back the matrices
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&Matrices[ETS_VIEW]));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&Matrices[ETS_WORLD]));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&Matrices[ETS_PROJECTION]));
#endif
		pID3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		ResetRenderStates = true;
	}

	if (ResetRenderStates || LastMaterial != Material)
	{
		// unset old material

		if (CurrentRenderMode == ERM_3D &&
			LastMaterial.getMaterialType() != Material.getMaterialType() &&
			LastMaterial.getMaterialType() >= 0 && LastMaterial.getMaterialType() < (s32)MaterialRenderers.size())
			MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();

		// set new material.

		if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.getMaterialType()].Renderer->onSetMaterial(
				Material, LastMaterial, ResetRenderStates, this);
	}

	bool shaderOK = true;
	if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
		shaderOK = MaterialRenderers[Material.getMaterialType()].Renderer->onRender(this, LastVertexType);

	LastMaterial = Material;

	ResetRenderStates = false;

	//TODO:yzh:review here
	SetWVP();
	CurrentRenderMode = ERM_3D;

	return shaderOK;
}


//! Can be called by an IMaterialRenderer to make its work easier.
void CXBOX360D3D9Driver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
	bool resetAllRenderstates)
{
	if (resetAllRenderstates ||
		lastmaterial.getAmbientColor() != material.getAmbientColor() ||
		lastmaterial.getDiffuseColor() != material.getDiffuseColor() ||
		lastmaterial.getSpecularColor() != material.getSpecularColor() ||
		lastmaterial.getEmissiveColor() != material.getEmissiveColor() ||
		lastmaterial.getShininess() != material.getShininess())
	{
		D3DMATERIAL9 mat;
		mat.Diffuse = colorToD3D(material.getDiffuseColor());
		mat.Ambient = colorToD3D(material.getAmbientColor());
		mat.Specular = colorToD3D(material.getSpecularColor());
		mat.Emissive = colorToD3D(material.getEmissiveColor());
		mat.Power = material.getShininess();
		//TODO: XBOX 360 Material:
#ifndef _IRR_XBOX_PLATFORM_
		pID3DDevice->SetMaterial(&mat);
#endif
	}

	// fillmode
	if (resetAllRenderstates || lastmaterial.getFlag(irr::video::EMF_WIREFRAME) != material.getFlag(irr::video::EMF_WIREFRAME) || lastmaterial.getFlag(irr::video::EMF_POINTCLOUD) != material.getFlag(irr::video::EMF_POINTCLOUD))
	{
		if (material.getFlag(irr::video::EMF_WIREFRAME))
			pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		else
		if (material.getFlag(irr::video::EMF_POINTCLOUD))
			pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
		else
			pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// shademode

	//if (resetAllRenderstates || lastmaterial.GouraudShading != material.GouraudShading)
	{
		// The following render states are not supported by Xbox 360
		//if (material.GouraudShading)
		//	pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		//else
		//	pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}

	// lighting

	//if (resetAllRenderstates || lastmaterial.Lighting != material.Lighting)
	{
		// The following render states are not supported by Xbox 360
		//TODO:[xbox]:UNIMPLEMENTED("[XBOX360]The following render states are not supported by Xbox 360");
		//if (material.Lighting)
		//	pID3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		//else
		//	pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	// zbuffer

	if (resetAllRenderstates || lastmaterial.getFlag(irr::video::EMF_ZBUFFER) != material.getFlag(irr::video::EMF_ZBUFFER))
	{
		//TODO:[xbox]: new mat material.getFlag(irr::video::EMF_ZBUFFER)
		switch (material.getFlag(irr::video::EMF_ZBUFFER))
		{
			case 0:
				pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
				break;
			case 1:
				pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				break;
			case 2:
				pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
				break;
		}
	}

	// zwrite
//	if (resetAllRenderstates || (lastmaterial.ZWriteEnable != material.ZWriteEnable))
	{
		if ( material.getFlag(irr::video::EMF_ZWRITE_ENABLE) && (/*AllowZWriteOnTransparent || */!material.isTransparent()))
			pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		else
			pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
	}

	// back face culling

	if (resetAllRenderstates || (lastmaterial.getFlag(irr::video::EMF_FRONT_FACE_CULLING) != material.getFlag(irr::video::EMF_FRONT_FACE_CULLING)) || (lastmaterial.getFlag(irr::video::EMF_BACK_FACE_CULLING) != material.getFlag(irr::video::EMF_BACK_FACE_CULLING)))
	{
		if (material.getFlag(irr::video::EMF_FRONT_FACE_CULLING) && material.getFlag(irr::video::EMF_BACK_FACE_CULLING))
			pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW|D3DCULL_CCW);
		else
		if (material.getFlag(irr::video::EMF_FRONT_FACE_CULLING))
			pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		else
		if (material.getFlag(irr::video::EMF_BACK_FACE_CULLING))
			pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		else
			pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	// fog
	//if (resetAllRenderstates || lastmaterial.FogEnable != material.FogEnable)
	{
		// The following render states are not supported by Xbox 360
		//TODO:[xbox]:UNIMPLEMENTED("[XBOX360]The following render states are not supported by Xbox 360");
		//pID3DDevice->SetRenderState(D3DRS_FOGENABLE, material.FogEnable);
	}

	// specular highlights
	//if (resetAllRenderstates || !core::equals(lastmaterial.Shininess,material.Shininess))
	{
		// The following render states are not supported by Xbox 360
		//TODO:[xbox]:UNIMPLEMENTED("[XBOX360]The following render states are not supported by Xbox 360");
		//bool enable = (material.Shininess!=0.0f);
		//pID3DDevice->SetRenderState(D3DRS_SPECULARENABLE, enable);
		//pID3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	}

	// normalization
	//if (resetAllRenderstates || lastmaterial.NormalizeNormals != material.NormalizeNormals)
	{
		// The following render states are not supported by Xbox 360
		//TODO:[xbox]:UNIMPLEMENTED("[XBOX360]The following render states are not supported by Xbox 360");
		//pID3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, material.NormalizeNormals);
	}

	// thickness
	if (resetAllRenderstates || lastmaterial.getThickness() != material.getThickness())
	{
		irr::f32 tn = material.getThickness();
		pID3DDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&tn));
	}

	//if (resetAllRenderstates || lastmaterial.TextureLayer[st].TextureWrap != material.TextureLayer[st].TextureWrap)
	{
		setWrapMode(material);
	}

}


//! sets the needed renderstates
void CXBOX360D3D9Driver::setRenderStatesStencilShadowMode(bool zfail)
{
	if ((CurrentRenderMode != ERM_SHADOW_VOLUME_ZFAIL &&
		CurrentRenderMode != ERM_SHADOW_VOLUME_ZPASS) ||
		Transformation3DChanged)
	{
#ifndef _IRR_XBOX_PLATFORM_
		// switch back the matrices
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&Matrices[ETS_VIEW]));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&Matrices[ETS_WORLD]));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&Matrices[ETS_PROJECTION]));
#endif
		Transformation3DChanged = false;

		setTexture(0,0);
		setTexture(1,0);
		setTexture(2,0);
		setTexture(3,0);
#ifndef _IRR_XBOX_PLATFORM_
		pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#endif
		pID3DDevice->SetFVF(D3DFVF_XYZ);
		LastVertexType = (video::E_VERTEX_TYPE)(-1);

		pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
#ifndef _IRR_XBOX_PLATFORM_
		pID3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
#endif
		// unset last 3d material
		if (CurrentRenderMode == ERM_3D &&
			Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
			MaterialRenderers[Material.getMaterialType()].Renderer->onUnsetMaterial();

		//pID3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		//pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	if (CurrentRenderMode != ERM_SHADOW_VOLUME_ZPASS && !zfail)
	{
		// USE THE ZPASS METHOD

		pID3DDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );

		pID3DDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
		pID3DDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
		pID3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

		pID3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		pID3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		pID3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	else
	if (CurrentRenderMode != ERM_SHADOW_VOLUME_ZFAIL && zfail)
	{
		// USE THE ZFAIL METHOD

		pID3DDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

		pID3DDevice->SetRenderState( D3DRS_STENCILREF, 0x0 );
		pID3DDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
		pID3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

		pID3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pID3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		pID3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}

	CurrentRenderMode = zfail ? ERM_SHADOW_VOLUME_ZFAIL : ERM_SHADOW_VOLUME_ZPASS;
}


//! sets the needed renderstates
void CXBOX360D3D9Driver::setRenderStatesStencilFillMode(bool alpha)
{
	if (CurrentRenderMode != ERM_STENCIL_FILL || Transformation3DChanged)
	{
		core::matrix4 mat;
#ifndef _IRR_XBOX_PLATFORM_
		pID3DDevice->SetTransform(D3DTS_VIEW, &UnitMatrixD3D9);
		pID3DDevice->SetTransform(D3DTS_WORLD, &UnitMatrixD3D9);
		pID3DDevice->SetTransform(D3DTS_PROJECTION, &UnitMatrixD3D9);
#endif
		pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
#ifndef _IRR_XBOX_PLATFORM_
		pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pID3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

		pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#endif
		pID3DDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
		pID3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
		//pID3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);
		pID3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff );
		pID3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff );

		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		Transformation3DChanged = false;

		if (alpha)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	CurrentRenderMode = ERM_STENCIL_FILL;
}


//! sets the needed renderstates
void CXBOX360D3D9Driver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	if (!pID3DDevice)
		return;

	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		// unset last 3d material
		if (CurrentRenderMode != ERM_2D)
		{
			if (static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
				MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
			//setBasicRenderStates(SMaterial(), SMaterial(), true);
			//TODO:XBOX 360 Review:need setRenderStates2DMode
			getMaterialRenderer(EMT_SOLID)->onSetMaterial(SMaterial(), SMaterial(), true, this);
			getMaterialRenderer(EMT_SOLID)->onRender(this, EVT_STANDARD);
			
			// everything that is wrongly set by SMaterial default
			pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
			pID3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			pID3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		}

		SetWVP((core::matrix4 *)&UnitMatrixD3D9, (core::matrix4 *)&UnitMatrixD3D9, (core::matrix4 *)&UnitMatrixD3D9); 
		SetTextureMatrix(0, *(core::matrix4 *)&UnitMatrixD3D9);
		SetTextureMatrix(1, *(core::matrix4 *)&UnitMatrixD3D9);
		SetTextureMatrix(2, *(core::matrix4 *)&UnitMatrixD3D9);
		SetTextureMatrix(3, *(core::matrix4 *)&UnitMatrixD3D9);
		Transformation3DChanged = false;
	}

	if (texture)
	{
		//TODO:[XBOX 360 SAMP FILTER]:yzh to review
		//pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		//pID3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		//pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pID3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

		//pID3DDevice->SetSamplerState(st, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		//pID3DDevice->SetSamplerState(st, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		//pID3DDevice->SetSamplerState(st, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		if (alphaChannel)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			if (alpha)
			{
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			}
			else
			{
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			}
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
			if (alpha)
			{
#ifndef _IRR_XBOX_PLATFORM_
				pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
#endif
				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}
			else
			{
#ifndef _IRR_XBOX_PLATFORM_
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#endif
				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
		}
	}
	else
	{
		if (alpha)
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
#ifndef _IRR_XBOX_PLATFORM_
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#endif
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}
 
	CurrentRenderMode = ERM_2D;
}


//! deletes all dynamic lights there are
void CXBOX360D3D9Driver::deleteAllDynamicLights()
{
#ifndef _IRR_XBOX_PLATFORM_
	for (s32 i=0; i<LastSetLight+1; ++i)
		pID3DDevice->LightEnable(i, false);
#endif
	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}


//! adds a dynamic light
void CXBOX360D3D9Driver::addDynamicLight(const SLight& dl)
{
	if ((u32)LastSetLight == Caps.MaxActiveLights-1)
		return;

	CNullDriver::addDynamicLight(dl);

	D3DLIGHT9 light;

	switch (dl.Type)
	{
	case ELT_POINT:
		light.Type = D3DLIGHT_POINT;
	break;
	case ELT_SPOT:
		light.Type = D3DLIGHT_SPOT;
	break;
	case ELT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
	break;
	}

	light.Position = *(D3DVECTOR*)((void*)(&dl.Position));
	light.Direction = *(D3DVECTOR*)((void*)(&dl.Direction));

	light.Range = core::min_(dl.Radius, MaxLightDistance);
	light.Falloff = dl.Falloff;

	light.Diffuse = *(D3DCOLORVALUE*)((void*)(&dl.DiffuseColor));
	light.Specular = *(D3DCOLORVALUE*)((void*)(&dl.SpecularColor));
	light.Ambient = *(D3DCOLORVALUE*)((void*)(&dl.AmbientColor));

	light.Attenuation0 = dl.Attenuation.X;
	light.Attenuation1 = dl.Attenuation.Y;
	light.Attenuation2 = dl.Attenuation.Z;

	light.Theta = dl.InnerCone * 2.0f * core::DEGTORAD;
	light.Phi = dl.OuterCone * 2.0f * core::DEGTORAD;

	++LastSetLight;
#ifndef _IRR_XBOX_PLATFORM_
	pID3DDevice->SetLight(LastSetLight, &light);
	pID3DDevice->LightEnable(LastSetLight, true);
#endif
}


//! returns the maximal amount of dynamic lights the device can handle
u32 CXBOX360D3D9Driver::getMaximalDynamicLightAmount() const
{
	return Caps.MaxActiveLights;
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CXBOX360D3D9Driver::setAmbientLight(const SColorf& color)
{
	if (!pID3DDevice)
		return;

	AmbientLight = color;
	D3DCOLOR col = color.toSColor().toARGB8();
#ifndef _IRR_XBOX_PLATFORM_
	pID3DDevice->SetRenderState(D3DRS_AMBIENT, col);
#endif
}


//! \return Returns the name of the video driver. Example: In case of the DIRECT3D9
//! driver, it would return "Direct3D9.0".
const wchar_t* CXBOX360D3D9Driver::getName() const
{
	return L"Direct3D 9.0";
}


//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CXBOX360D3D9Driver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	if (!StencilBuffer || !count)
		return;

	setRenderStatesStencilShadowMode(zfail);

	if (!zfail)
	{
		// ZPASS Method

		// Draw front-side of shadow volume in stencil/z only
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
		pID3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));

		// Now reverse cull order so front sides of shadow volume are written.
		pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
		pID3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECRSAT);
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));
	}
	else
	{
		// ZFAIL Method

		// Draw front-side of shadow volume in stencil/z only
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
		pID3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT );
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));

		// Now reverse cull order so front sides of shadow volume are written.
		pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_DECRSAT );
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));
	}

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif
}


//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void CXBOX360D3D9Driver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
			video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	S3DVertex vtx[4];
	vtx[0] = S3DVertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, leftUpEdge, 0.0f, 0.0f);
	vtx[1] = S3DVertex(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, rightUpEdge, 0.0f, 1.0f);
	vtx[2] = S3DVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, leftDownEdge, 1.0f, 0.0f);
	vtx[3] = S3DVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, rightDownEdge, 1.0f, 1.0f);

	s16 indices[6] = {0,1,2,1,3,2};

	setRenderStatesStencilFillMode(
		leftUpEdge.getAlpha() < 255 ||
		rightUpEdge.getAlpha() < 255 ||
		leftDownEdge.getAlpha() < 255 ||
		rightDownEdge.getAlpha() < 255);

	setTexture(0,0);

	setVertexShader(EVT_STANDARD);

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif
	setShaderState(true, false);
	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
		D3DFMT_INDEX16, &vtx[0], sizeof(S3DVertex));

	if (clearStencilBuffer)
		pID3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL,0, 1.0, 0);
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 CXBOX360D3D9Driver::getMaximalPrimitiveCount() const
{
	return Caps.MaxPrimitiveCount;
}


//! Sets the fog mode.
void CXBOX360D3D9Driver::setFog(SColor color, bool linearFog, f32 start,
	f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(color, linearFog, start, end, density, pixelFog, rangeFog);

	if (!pID3DDevice)
		return;

	UNIMPLEMENTED(__FUNCTION__);
}


//! Draws a 3d line.
void CXBOX360D3D9Driver::draw3DLine(const core::vector3df& start,
	const core::vector3df& end, SColor color)
{
	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();
	video::S3DVertex v[2];
	v[0].Color = color;
	v[1].Color = color;
	v[0].Pos = start;
	v[1].Pos = end;

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif

	pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(S3DVertex));
}


//! resets the device
bool CXBOX360D3D9Driver::reset()
{
	u32 i;
	os::Printer::log("Resetting D3D9 device.", ELL_INFORMATION);

	//TODO:[xbox]: need review(about texture release)
	//the texture manager release seem not suit for "new materail interface" trunk
	if(TextureManager)
	{
		irr::u32 tex_num = TextureManager->getTextureCount();
		for (i = 0; i < tex_num; ++i)
		{
			ITexture *p= TextureManager->getTextureByIndex(i);
			if(p->isRenderTarget())
			{
				IDirect3DTexture9* tex = ((CXBOX360D3D9Texture*)(p))->getDX9Texture();
				if (tex)
					tex->Release();				
			}

		}
	}

	DriverWasReset=true;

	HRESULT hr = pID3DDevice->Reset(&present);

	//TODO:[xbox]: need review(about texture release)
	if(TextureManager)
	{
		irr::u32 tex_num = TextureManager->getTextureCount();
		for (i = 0; i < tex_num; ++i)
		{
			ITexture *p= TextureManager->getTextureByIndex(i);
			if(p->isRenderTarget())
			{
				((CXBOX360D3D9Texture*)(p))->createRenderTarget();			
			}

		}
	}

	//for (i=0; i<Textures.size(); ++i)
	//{
	//	if (Textures[i].Surface->isRenderTarget())
	//		((CXBOX360D3D9Texture*)(Textures[i].Surface))->createRenderTarget();
	//}

	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
		{
			DeviceLost = true;
			os::Printer::log("Resetting failed due to device lost.", ELL_WARNING);
		}
#ifdef D3DERR_DEVICEREMOVED
		else if (hr == D3DERR_DEVICEREMOVED)
		{
			os::Printer::log("Resetting failed due to device removed.", ELL_WARNING);
		}
#endif
		else if (hr == D3DERR_DRIVERINTERNALERROR)
		{
			os::Printer::log("Resetting failed due to internal error.", ELL_WARNING);
		}
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			os::Printer::log("Resetting failed due to out of memory.", ELL_WARNING);
		}
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			os::Printer::log("Resetting failed due to not reset.", ELL_WARNING);
		}
		else if (hr == D3DERR_INVALIDCALL)
		{
			os::Printer::log("Resetting failed due to invalid call", "You need to release some more surfaces.", ELL_WARNING);
		}
		else
		{
			os::Printer::log("Resetting failed due to unknown reason.", core::stringc(hr).c_str(), ELL_WARNING);
		}
		return false;
	}

	DeviceLost = false;
	ResetRenderStates = true;
	LastVertexType = (E_VERTEX_TYPE)-1;

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		CurrentTexture[i] = 0;

	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
	setAmbientLight(AmbientLight);

	return true;
}


void CXBOX360D3D9Driver::OnResize(const core::dimension2d<s32>& size)
{
	if (!pID3DDevice)
		return;

	CNullDriver::OnResize(size);
	present.BackBufferWidth = size.Width;
	present.BackBufferHeight = size.Height;

	reset();
}


//! Returns type of video driver
E_DRIVER_TYPE CXBOX360D3D9Driver::getDriverType() const
{
	return EDT_DIRECT3D9;
}


//! Returns the transformation set by setTransform
const core::matrix4& CXBOX360D3D9Driver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}


//! Sets a vertex shader constant.
void CXBOX360D3D9Driver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	if (data)
		pID3DDevice->SetVertexShaderConstantF(startRegister, data, constantAmount);
}


//! Sets a pixel shader constant.
void CXBOX360D3D9Driver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	if (data)
		pID3DDevice->SetPixelShaderConstantF(startRegister, data, constantAmount);
}

bool CXBOX360D3D9Driver::hasVertexShaderVariable(const irr::c8* name)
{
	if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
	{
		CXBOX360D3D9HLSLMaterialRenderer* r = (CXBOX360D3D9HLSLMaterialRenderer*)MaterialRenderers[Material.getMaterialType()].Renderer;
		return r->hasVariable(true, name);
	}
	return false;
}

bool CXBOX360D3D9Driver::hasPixelShaderVariable(const irr::c8* name)
{
	if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
	{
		CXBOX360D3D9HLSLMaterialRenderer* r = (CXBOX360D3D9HLSLMaterialRenderer*)MaterialRenderers[Material.getMaterialType()].Renderer;
		return r->hasVariable(false, name);
	}
	return false;
}

//! Sets a constant for the vertex shader based on a name.
bool CXBOX360D3D9Driver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
	{
		CXBOX360D3D9HLSLMaterialRenderer* r = (CXBOX360D3D9HLSLMaterialRenderer*)MaterialRenderers[Material.getMaterialType()].Renderer;
		return r->setVariable(true, name, floats, count);
	}

	return false;
}


//! Sets a constant for the pixel shader based on a name.
bool CXBOX360D3D9Driver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	if (Material.getMaterialType() >= 0 && Material.getMaterialType() < (s32)MaterialRenderers.size())
	{
		CXBOX360D3D9HLSLMaterialRenderer* r = (CXBOX360D3D9HLSLMaterialRenderer*)MaterialRenderers[Material.getMaterialType()].Renderer;
		return r->setVariable(false, name, floats, count);
	}

	return false;
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* CXBOX360D3D9Driver::getGPUProgrammingServices()
{
	return this;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 CXBOX360D3D9Driver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;
	CXBOX360D3D9ShaderMaterialRenderer* r = irrnew CXBOX360D3D9ShaderMaterialRenderer(
		pID3DDevice, this, nr, vertexShaderProgram, pixelShaderProgram,
		callback, getMaterialRenderer(baseMaterial), userData);

	r->drop();
	return nr;
}


//! Adds a new material renderer to the VideoDriver, based on a high level shading
//! language.
s32 CXBOX360D3D9Driver::addHighLevelShaderMaterial(
		const c8* vertexShaderProgram,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderProgram,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		IShaderConstantSetCallBack* callback,
		E_MATERIAL_TYPE baseMaterial, s32 userData)
{
	s32 nr = -1;

	CXBOX360D3D9HLSLMaterialRenderer* hlsl = irrnew CXBOX360D3D9HLSLMaterialRenderer(
		pID3DDevice, this, nr,
		vertexShaderProgram,
		vertexShaderEntryPointName,
		vsCompileTarget,
		pixelShaderProgram,
		pixelShaderEntryPointName,
		psCompileTarget,
		callback,
		getMaterialRenderer(baseMaterial),
		userData);

	hlsl->drop();
	return nr;
}


//! Returns a pointer to the IVideoDriver interface. (Implementation for
//! IMaterialRendererServices)
IVideoDriver* CXBOX360D3D9Driver::getVideoDriver()
{
	return this;
}


//! Creates a render target texture.
ITexture* CXBOX360D3D9Driver::addRenderTargetTexture(const core::dimension2d<s32>& size,
		const c8* name)
{
	if (!name)
		name="rt";
	ITexture* tex = irrnew CXBOX360D3D9Texture(this, size, name);
	addTexture(tex);
	tex->drop();
	return tex;
}


//! Clears the ZBuffer.
void CXBOX360D3D9Driver::clearZBuffer()
{
	HRESULT hr = pID3DDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, 0);

	if (FAILED(hr))
		os::Printer::log("CXBOX360D3D9Driver clearZBuffer() failed.", ELL_WARNING);
}


//! Returns an image created from the last rendered frame.
IImage* CXBOX360D3D9Driver::createScreenShot()
{
#ifndef _IRR_XBOX_PLATFORM_
	HRESULT hr;

	// query the screen dimensions of the current adapter
	D3DDISPLAYMODE displayMode;
	pID3DDevice->GetDisplayMode(0, &displayMode);

	// create the image surface to store the front buffer image [always A8R8G8B8]
	LPDIRECT3DSURFACE9 lpSurface;
	if (FAILED(hr = pID3DDevice->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &lpSurface, 0)))
		return 0;

	// read the front buffer into the image surface
	if (FAILED(hr = pID3DDevice->GetFrontBufferData(0, lpSurface)))
	{
		lpSurface->Release();
		return 0;
	}

	RECT clientRect;
	{
		POINT clientPoint;
		clientPoint.x = 0;
		clientPoint.y = 0;

		ClientToScreen((HWND)getExposedVideoData().D3D9.HWnd, &clientPoint);

		clientRect.left   = clientPoint.x;
		clientRect.top    = clientPoint.y;
		clientRect.right  = clientRect.left + ScreenSize.Width;
		clientRect.bottom = clientRect.top  + ScreenSize.Height;
	}

	// lock our area of the surface
	D3DLOCKED_RECT lockedRect;
	if (FAILED(lpSurface->LockRect(&lockedRect, &clientRect, D3DLOCK_READONLY)))
	{
		lpSurface->Release();
		return 0;
	}

	// this could throw, but we aren't going to worry about that case very much
	IImage* newImage = irrnew CImage(ECF_A8R8G8B8, ScreenSize);

	// d3d pads the image, so we need to copy the correct number of bytes
	u32* dP = (u32*)newImage->lock();
	u8 * sP = (u8 *)lockedRect.pBits;

	for (s32 y = 0; y < ScreenSize.Height; ++y)
	{
		memcpy(dP, sP, ScreenSize.Width * 4);

		sP += lockedRect.Pitch;
		dP += ScreenSize.Width;
	}

	newImage->unlock();

	// we can unlock and release the surface
	lpSurface->UnlockRect();

	// release the image surface
	lpSurface->Release();

	// return status of save operation to caller
	return newImage;
#endif
	return NULL;
}


// returns the current size of the screen or rendertarget
const core::dimension2d<s32>& CXBOX360D3D9Driver::getCurrentRenderTargetSize() const
{
	if ( CurrentRendertargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRendertargetSize;
}


// Set/unset a clipping plane.
bool CXBOX360D3D9Driver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return false;

	pID3DDevice->SetClipPlane(index, (const float*)&plane);
	enableClipPlane(index, enable);
	return true;
}


// Enable/disable a clipping plane.
void CXBOX360D3D9Driver::enableClipPlane(u32 index, bool enable)
{
	if (index >= MaxUserClipPlanes)
		return;
	DWORD renderstate;
	pID3DDevice->GetRenderState(D3DRS_CLIPPLANEENABLE, &renderstate);
	if (enable)
		renderstate |= (1 << index);
	else
		renderstate &= ~(1 << index);
	pID3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, renderstate);
}


//set scissor test
//! \param scissorRect: the scissor rect, upperleft cornor is 0,0
void CXBOX360D3D9Driver::setScissor(const core::rect<s32>& scissorRect)
{
	pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	RECT scissor;
	scissor.left = scissorRect.UpperLeftCorner.X;
	scissor.top = scissorRect.UpperLeftCorner.Y;
	scissor.right = scissorRect.LowerRightCorner.X;
	scissor.bottom = scissorRect.LowerRightCorner.Y;
	pID3DDevice->SetScissorRect(&scissor);

}

//close scissor test
void CXBOX360D3D9Driver::resetScissor()
{
	pID3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}

bool CXBOX360D3D9Driver::endScene2D()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	return true;
}

bool CXBOX360D3D9Driver::beginScene2D(bool resetStates)
{
	//Reset2DRenderStates = resetStates;
	//setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	//setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	//setTransform ( video::ETS_WORLD, core::IdentityMatrix );
	setTransform ( video::ETS_TEXTURE_0, core::IdentityMatrix );
	//setTransform ( video::ETS_TEXTURE_1, core::IdentityMatrix );
	//setTransform ( video::ETS_TEXTURE_2, core::IdentityMatrix );
	//setTransform ( video::ETS_TEXTURE_3, core::IdentityMatrix );
	setRenderStates2DMode(false,false,false);
	return true;
}

void CXBOX360D3D9Driver::setWrapMode(const video::SMaterial& material)
{
	//TODO:[xbox]: new mat
	// texture address mode
//	for (u32 st=0; st<MaxTextureUnits; ++st)
//	{
////		if (resetAllRenderstates || lastmaterial.TextureLayer[st].TextureWrap != material.TextureLayer[st].TextureWrap)
//		{
//			u32 mode = D3DTADDRESS_WRAP;
//			switch (material.TextureLayer[st].TextureWrap)
//			{
//				case ETC_REPEAT:
//					mode=D3DTADDRESS_WRAP;
//					break;
//				case ETC_CLAMP:
//				case ETC_CLAMP_TO_EDGE:
//					mode=D3DTADDRESS_CLAMP;
//					break;
//				case ETC_MIRROR:
//					mode=D3DTADDRESS_MIRROR;
//					break;
//				case ETC_CLAMP_TO_BORDER:
//					mode=D3DTADDRESS_BORDER;
//					break;
//			}
//
//			pID3DDevice->SetSamplerState(st, D3DSAMP_ADDRESSU, mode );
//			pID3DDevice->SetSamplerState(st, D3DSAMP_ADDRESSV, mode );
//		}
//
//		// Bilinear, trilinear, and anisotropic filter
//		//if (resetAllRenderstates ||
//		//	lastmaterial.TextureLayer[st].BilinearFilter != material.TextureLayer[st].BilinearFilter ||
//		//	lastmaterial.TextureLayer[st].TrilinearFilter != material.TextureLayer[st].TrilinearFilter ||
//		//	lastmaterial.TextureLayer[st].AnisotropicFilter != material.TextureLayer[st].AnisotropicFilter)
//		{
//			if (material.TextureLayer[st].BilinearFilter || material.TextureLayer[st].TrilinearFilter || material.TextureLayer[st].AnisotropicFilter)
//			{
//				D3DTEXTUREFILTERTYPE tftMag = ((Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) &&
//						material.TextureLayer[st].AnisotropicFilter) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
//				D3DTEXTUREFILTERTYPE tftMin = ((Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) &&
//						material.TextureLayer[st].AnisotropicFilter) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
//				D3DTEXTUREFILTERTYPE tftMip = material.TextureLayer[st].TrilinearFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT;
//
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MAGFILTER, tftMag);
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MINFILTER, tftMin);
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MIPFILTER, tftMip);
//			}
//			else
//			{
//				//TODO:[XBOX 360 SAMP FILTER]:yzh to review
//				//pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//				//pID3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//				//pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//				pID3DDevice->SetSamplerState(st, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//			}
//		}
//	}
}

void CXBOX360D3D9Driver::SetTextureMatrix(u32 texture_stage, const core::matrix4 &m)
{
	if(texture_stage == 0 )
	{
		if( hasVertexShaderVariable("tex0mat") )
		{
			this->setVertexShaderConstant("tex0mat", m.pointer(), 16);
		}
	}
	//TODO:XBOX 360 Shader: implement Texture 1,2,3's matrix in shader
}

void CXBOX360D3D9Driver::SetWVP(core::matrix4 *m, core::matrix4 *v, core::matrix4 *p)
{
	if( !hasVertexShaderVariable("tex0mat") )
	{
		return;
	}

	if(m != NULL && v!= NULL && p != NULL)
	{
		core::matrix4 mat = (*p)*(*v)*(*m);
		this->setVertexShaderConstant("mat", mat.pointer(), 16);
	}
	else
	{
		core::matrix4 mat = Matrices[ETS_PROJECTION]*Matrices[ETS_VIEW]*Matrices[ETS_WORLD];
		this->setVertexShaderConstant("mat", mat.pointer(), 16);
	}
}

void CXBOX360D3D9Driver::setShaderState(bool bDiffuse, bool bTexture)
{
	f32 t[1] = {1.0f};
	f32 f[1] = {0.0f};
	f32 *diffuse = bDiffuse?t:f;
	f32 *texture = bTexture?t:f;
	setPixelShaderConstant("ps_bDiffuse", diffuse, 1);
	setPixelShaderConstant("ps_bTexture", texture, 1);		
}

void CXBOX360D3D9ShaderBasicConstantSetCallBack::onSetConstants(IMaterialRendererServices* services, s32 userData)
{
	CXBOX360D3D9Driver* vd = (CXBOX360D3D9Driver*)services->getVideoDriver();
	vd->grab();
	IDirect3DDevice9* pD3D9Device=vd->getExposedVideoData().D3D9.D3DDev9;
	//f32 f[4] = {1.0f,1.0f,1.0f,1.0f};
	f32 t[1] = {1.0f};
	f32 f[1] = {0.0f};
	f32 RGBA_OP[] = {0.f, 1.f, 2.f, 3.f};
	//0: nothing, 1: Texture color replace 2: diffuse color 3: module



	f32 *diffuse = t;
	f32 *texture = t;
	f32 *alpha_op = &RGBA_OP[1];
	f32 *rgb_op = &RGBA_OP[3];
	switch(userData)
	{
	case EMT_2D_ALPHA:
	case EMT_TRANSPARENT_VERTEX_ALPHA:
		{
			diffuse = t;
			texture = t;
			alpha_op = &RGBA_OP[2];
			rgb_op = &RGBA_OP[3];		
		}
		break;
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		{
			diffuse = t;
			texture = t;	
			alpha_op = &RGBA_OP[3];//TODO:yzh: original is 1
			rgb_op = &RGBA_OP[3];
		}
		break;
	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		{
			diffuse = f;
			texture = t;
			alpha_op = &RGBA_OP[1];
			rgb_op = &RGBA_OP[3];
		}
		break;
	case EMT_SOLID:
		{
			diffuse = t;
			texture = t;
		}
		break;

	case EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA:
		{
			diffuse = t;
			texture = t;
			alpha_op = &RGBA_OP[3];
			rgb_op = &RGBA_OP[3];
		}
		break;
	default:
		{
			diffuse = t;
			texture = t;
			//pD3D9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			//pD3D9Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			//pD3D9Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			//pD3D9Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			//pD3D9Device->SetRenderState(D3DRS_ALPHAREF, 0);
			//pD3D9Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		}
		break;
	}

	//if(vd->getCurrentRenderMode() == CXBOX360D3D9Driver::ERM_2D)
	//{
	//	if(vd->getCurrentTexture(0) == NULL)
	//	{
	//		diffuse = t;
	//		texture = f;
	//	}
	//}
	services->setPixelShaderConstant("RGB_OP", rgb_op, 1);
	services->setPixelShaderConstant("ALPHA_OP", alpha_op, 1);
	services->setPixelShaderConstant("ps_bDiffuse", diffuse, 1);
	services->setPixelShaderConstant("ps_bTexture", texture, 1);		
	vd->drop();
}

video::ITexture* CXBOX360D3D9Driver::createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data)
{
	return irrnew CXBOX360D3D9Texture(this, irr::core::dimension2di(w, h), name, false);
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
video::ITexture* CXBOX360D3D9Driver::createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
																		const c8* hashName, bool refData)
{
	CXBOX360D3D9Texture* t = irrnew CXBOX360D3D9Texture(file, this, irr::video::ETCF_OPTIMIZED_FOR_QUALITY, hashName);
	if(t->isLoadOk())
	{
		return t;
	}
	else
	{
		t->drop();
		return NULL;
	}
	return NULL;
}
#endif

} // end namespace video
} // end namespace irr

#endif // #ifdef _IRR_XBOX_PLATFORM_



namespace irr
{
namespace video
{


//! creates a video driver
IVideoDriver* createXBOX360DirectX9Driver(const core::dimension2d<s32>& screenSize,
		HWND window, u32 bits, bool fullscreen, bool stencilbuffer,
		io::IFileSystem* io, bool pureSoftware, bool highPrecisionFPU,
		bool vsync, bool antiAlias, irr::video::E_FULLSCREEN_ANTIALIASING_SETTING aa_setting)
{
	CXBOX360D3D9Driver* dx9 = irrnew CXBOX360D3D9Driver(screenSize, window, fullscreen, stencilbuffer, io, pureSoftware);
	if (!dx9->initDriver(screenSize, window, bits, fullscreen, pureSoftware, highPrecisionFPU, vsync, antiAlias, aa_setting))
	{
		dx9->drop();
		dx9 = 0;
	}

	return dx9;
}


} // end namespace video
} // end namespace irr


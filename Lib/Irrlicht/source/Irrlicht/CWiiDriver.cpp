#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CWiiDriver.h"

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_WII_DEVICE_

#include "Wii/CWiiTexture.h"
#include "irros.h"
#include "CImage.h"
#include "CAttributes.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "IImageLoader.h"
#include "IImageWriter.h"
#include "IMaterialRenderer.h"
#include "Wii/CWiiMaterialRenderer.h"
#include "CIrrDeviceWii.h"
#include <revolution.h>
#include <revolution/mem.h>

extern "C" void FIOFindFirst()
{
};

namespace irr
{
namespace video
{

namespace
{
const static GXCompare ZBufferFuncMap[] =
{
	GX_NEVER,		// ECFN_NEVER = 0,
	GX_LESS,		// ECFN_LESS,
	GX_EQUAL,		// ECFN_EQUAL,
	GX_LEQUAL,		// ECFN_LEQUAL,
	GX_GREATER,		// ECFN_GREATER,
	GX_NEQUAL,		// ECFN_NOTEQUAL,
	GX_GEQUAL,		// ECFN_GEQUAL,
	GX_ALWAYS		// ECFN_ALWAYS
};

const int VertexComponentTypeMap[] =
{
	GX_S8,	//CT_BYTE = 0,
	GX_U8,	//CT_UNSIGNED_BYTE = 1,
	GX_S16,	//CT_SHORT = 2,
	GX_U16,	//CT_UNSIGNED_SHORT = 3,
	GX_F32,	//CT_INT = 4,
	GX_F32,	//CT_UNSIGNED_INT = 5,
	GX_F32,	//CT_FLOAT = 6,
};
}

CWiiDriver::SBinding::SBinding(CWiiDriver* driver)
	: Driver(driver)
	, DisplayListBuffer(NULL)
	, DisplayListUsedSize(0)
{
}

CWiiDriver::SBinding::~SBinding()
{
	setDisplayListUsedSize(0);
}

E_DRIVER_TYPE
CWiiDriver::SBinding::getDriverType() const
{
	return EDT_WII;
}

void
CWiiDriver::SBinding::onDelete() 
{
	GXDrawDone();
}

void
CWiiDriver::SBinding::setDisplayListUsedSize(u32 usedSize)
{
	DisplayListUsedSize = usedSize;
	if (DisplayListBuffer)
	{
		if (DisplayListUsedSize == 0)
		{
			delete[] DisplayListBuffer;
			DisplayListBuffer = NULL;
		}
		else
		{
			flushDisplayListRange();
			u8* newBuffer = (DisplayListUsedSize
							 ? irrnew u8 [DisplayListUsedSize]
							 : NULL);
			memcpy(newBuffer, DisplayListBuffer, DisplayListUsedSize);
			delete[] DisplayListBuffer;
			DisplayListBuffer = newBuffer;
			flushDisplayListRange();
		}
	}
	else if (DisplayListUsedSize > 0)
	{
		DisplayListBuffer = irrnew u8 [DisplayListUsedSize];
	}
}

E_COMPILE_DATA_TYPE
CWiiDriver::SCompileData::getType() const
{
	return ECDT_DRIVER_DISPLAY_LIST;
}

//! constructor
CWiiDriver::CWiiDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize, CIrrDeviceWii *pDevice)
	: CNullDriver(io, screenSize)
	, Device(pDevice)
	//, CurrentMaterial(0)
	, RenderTargetTexture(0)
	, CurrentRenderTargetSize(0,0)
	, CurrentRenderMode(ERM_NONE)
	, ResetRenderStates(true)
	, ClearColor(0, 0, 0, 0)
	, ClearDepth(GX_MAX_Z24)
	, CompileData(NULL)
{
	GXSetCopyClear(*reinterpret_cast<GXColor*>(ClearColor.getDataPtr()),
				   ClearDepth);

	#ifdef _DEBUG
	setDebugName("CWiiDriver");
	#endif

	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		CurrentTexture[i] = NULL;
	}

	int length = GXGetTexBufferSize(screenSize.Width,
									screenSize.Height,
									GX_TF_RGBA8,
									GX_FALSE,
									0);
	ClearFrameBufferData
		= irrnewh (MEMHINT_ZERO | MEMHINT_ALIGN32 | MEMHINT_FAST) u8 [length];

	createMaterialRenderers();
}

//! destructor
CWiiDriver::~CWiiDriver()
{

	// delete textures

	deleteAllTextures();

	// delete surface loader

	u32 i;
	for (i=0; i<SurfaceLoader.size(); ++i)
		SurfaceLoader[i]->drop();
	SurfaceLoader.clear();

	// delete surface writer

	for (i=0; i<SurfaceWriter.size(); ++i)
		SurfaceWriter[i]->drop();
	SurfaceWriter.clear();

	deleteMaterialRenders();

	if(ClearFrameBufferData)
		delete ClearFrameBufferData;
}

//! applications must call this method before performing any rendering. returns false if failed.
bool
CWiiDriver::beginScene()
{	
	CNullDriver::beginScene();
	PrimitivesDrawn = 0;

	if(0)//if (m_renderModeObj->field_rendering)
	{
		GXSetViewportJitter(
		  0.0F, 0.0F, (float)ScreenSize.Width, (float)ScreenSize.Height, 
		  0.0F, 1.0F, VIGetNextField());
	}
	else
	{
		GXSetViewport(
		  0.0F, 0.0F, (float)ScreenSize.Width, (float)ScreenSize.Height, 
		  0.0F, 1.0F);
	}

    // Invalidate vertex cache in GP
    GXInvalidateVtxCache();
    // Invalidate texture cache in GP
    GXInvalidateTexAll();

	return true;
}

bool
CWiiDriver::swapBuffers(int clearMask)
{
	bool clear = clearMask != 0;

	GXBool colorUpdate, alphaUpdate;
	GXGetColorUpdate(&colorUpdate);
	GXGetAlphaUpdate(&alphaUpdate);
	if (clear)
	{
		GXSetCopyClear(*reinterpret_cast<GXColor*>(ClearColor.getDataPtr()),
					   ClearDepth);
		bool clearColor = (clearMask & EFB_COLOR) != 0;
		GXSetColorUpdate(clearColor);
		GXSetAlphaUpdate(clearColor);

		GXSetZMode(GX_FALSE, GX_ALWAYS, (clearMask & EFB_DEPTH) != 0);
	}

	Device->swapBuffer(clear);

	IMaterialRenderer* mr = getMaterialRenderer(LastMaterial.getMaterialType());
	GXSetZMode(GX_TRUE,
			   GX_LEQUAL,
			   (LastMaterial.getFlag(EMF_ZWRITE_ENABLE)
				&& (getOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT)
					|| !(mr && mr->isTransparent()))));
	GXSetColorUpdate(colorUpdate);
	GXSetAlphaUpdate(alphaUpdate);
	return true;
}

void
CWiiDriver::clearBuffers(int clearMask)
{
	GXSetCopyClear(*reinterpret_cast<GXColor*>(ClearColor.getDataPtr()), ClearDepth);

	GXBool colorUpdate, alphaUpdate;
	GXGetColorUpdate(&colorUpdate);
	GXGetAlphaUpdate(&alphaUpdate);

	//NOTE-FH: Change for SetDispCopy
	//GXSetDispCopySrc(0, 0, getScreenSize().Width, getScreenSize().Height);
	//GXSetDispCopyDst(getScreenSize().Width, getScreenSize().Height);
	GXSetTexCopySrc(0, 0, getScreenSize().Width, getScreenSize().Height);
	GXSetTexCopyDst(getScreenSize().Width, getScreenSize().Height, GX_TF_RGBA8, GX_FALSE);

	bool clearColor = (clearMask & EFB_COLOR) != 0;
	GXSetColorUpdate(clearColor);
	GXSetAlphaUpdate(clearColor);
	GXSetZMode(GX_FALSE, GX_ALWAYS, (clearMask & EFB_DEPTH) != 0);
	GXCopyTex(ClearFrameBufferData, GX_TRUE);

	GXSetZMode(GX_TRUE,
			   GX_LEQUAL,
			   (LastMaterial.getFlag(EMF_ZWRITE_ENABLE)
				&& (getOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT)
					|| !LastMaterial.isTransparent())));
	GXSetColorUpdate(colorUpdate);
	GXSetAlphaUpdate(alphaUpdate);
}

void
CWiiDriver::setClearColor(const SColor& color)
{
	ClearColor = color;
}

SColor
CWiiDriver::getClearColor() const
{
	return ClearColor;
}

void
CWiiDriver::setClearDepth(float depth)
{
	ClearDepth = static_cast<u32>(depth * GX_MAX_Z24 + 0.5f);
}

float
CWiiDriver::getClearDepth() const
{
	return ClearDepth;
}

void
CWiiDriver::setClearStencil(int)
{
	_IRR_DEBUG_BREAK_IF("not supported by Wii");
}

int
CWiiDriver::getClearStencil() const
{
	return 0;
}

bool
CWiiDriver::beginScene2D()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	ProjectionMatrixBackup = getTransform(ETS_PROJECTION);
	ViewMatrixBackup = getTransform(ETS_VIEW);
	WorldMatrixBackup = getTransform(ETS_WORLD);
	
	core::matrix4 projMtx(irr::core::matrix4::EM4CONST_NOTHING);
	MTXOrtho((f32 (*)[4])projMtx.pointer()
			, 0
			, getCurrentRenderTargetSize().Height
			, 0
			, getCurrentRenderTargetSize().Width
			, 0.0F
			, 1.0F);
	GXSetProjection((f32 (*)[4])projMtx.pointer(), GX_ORTHOGRAPHIC);
	Matrices[ETS_PROJECTION] = projMtx.getTransposed();
	
    // Set the model view matrix as an identity matrix
    GXLoadPosMtxImm((f32 (*)[4])core::IdentityMatrix.pointer(), GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    Matrices[ETS_VIEW] = core::IdentityMatrix;
	Matrices[ETS_WORLD] = core::IdentityMatrix;
	Matrices[ETS_TEXTURE_0] = core::IdentityMatrix;
	Matrices[ETS_TEXTURE_1] = core::IdentityMatrix;

	// init default material flags
	if (static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
		MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
	SMaterial material;
	material.setMaterialType(EMT_SOLID);
	material.setFlag(EMF_ZBUFFER, false);
	material.setFlag(EMF_ZWRITE_ENABLE, false);
	material.setFlag(EMF_LIGHTING, false);
	material.setFlag(EMF_FRONT_FACE_CULLING, false);
	material.setFlag(EMF_BACK_FACE_CULLING, false);
	setBasicRenderStates(material, material, true);
	Next2DMaterial = material;

	GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_FALSE);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevSwapModeTable(GX_TEV_SWAP0,GX_CH_RED,GX_CH_GREEN,GX_CH_BLUE,GX_CH_ALPHA);

	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);  
	
    
	CurrentRenderMode = ERM_2D;
	return true;
}

bool
CWiiDriver::endScene2D()
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	CurrentRenderMode = ERM_3D;
	
	setTransform(ETS_PROJECTION, ProjectionMatrixBackup);
	setTransform(ETS_VIEW, ViewMatrixBackup);
	setTransform(ETS_WORLD, WorldMatrixBackup);
	
	return true;
}


//! Returns the transformation set by setTransform
const core::matrix4&
CWiiDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}

//! sets transformation
void
CWiiDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	//f32 gxmat[16];
	Matrices[state] = mat;
	
	switch(state)
	{
	case ETS_VIEW:
	case ETS_WORLD:
	{
 		core::matrix4 gxMat = Matrices[ETS_VIEW] * Matrices[ETS_WORLD];
		gxMat[2] = -gxMat[2];
		gxMat[6] = -gxMat[6];
		gxMat[10] = -gxMat[10];
		gxMat[14] = -gxMat[14];

 		GXLoadPosMtxImm((f32 (*)[4])  gxMat.getTransposed().pointer(), GX_PNMTX0);

		core::matrix4 gxMat2(core::matrix4::EM4CONST_NOTHING);
		gxMat.getInverse(gxMat2);
		GXLoadNrmMtxImm((f32 (*)[4])  gxMat2.pointer(), GX_PNMTX0);
		//Matrices[ETS_VIEW].getInverse(gxMat2);
		GXLoadTexMtxImm((const f32 (*)[4])gxMat2.pointer(), (GXTexGenSrc)(GX_TEXMTX2), GX_MTX3x4);
		break;
	}
	case ETS_PROJECTION:
	{
		core::matrix4 gxMat = mat.getTransposed();
		// transposed, so 11 and 14 are now switched

		// is it ortho or perspective?
		if (gxMat[14] == 0.0f)
		{
			// Substitute n/(n-f) for -f(f-n)
			gxMat[11] = gxMat[11] - 1.0f;

			// Substitute 1/(f-n) for -1/(f-n)
			gxMat[10] = -gxMat[10];

			GXSetProjection((f32 (*)[4]) gxMat.pointer(), GX_ORTHOGRAPHIC);
		}
		else
		{
			// Substitute f/(f-n) for -n(f-n)
			gxMat[10] = 1.0f - gxMat[10];

			// Substitute 1 for -1, but it is actually not copied by GXSetProjection
			//gxMat[14] = -gxMat[14];

			GXSetProjection((f32 (*)[4]) gxMat.pointer(), GX_PERSPECTIVE);
		}
		break;
	}
	case ETS_TEXTURE_0:
	case ETS_TEXTURE_1:
	case ETS_TEXTURE_2:
	case ETS_TEXTURE_3:
	{
		// in 2D mode, for consistency with GL, texture matrix state is unaffected
		const u32 i = state - ETS_TEXTURE_0;
		//const bool isRTT = Material.getTexture(i) && Material.getTexture(i)->isRenderTarget();

		/*if (mat.isIdentity())
		{
			GXSetTexCoordGen((GXTexCoordID)(GX_TEXCOORD0 + i), GX_TG_MTX3x4, (GXTexGenSrc)(GX_TG_TEX0 + i), GX_IDENTITY);
		}
		else*/
		{
			GXLoadTexMtxImm((const f32 (*)[4])mat.getTransposed().pointer(), (GXTexGenSrc)(GX_TEXMTX0), GX_MTX3x4);
			//GXSetTexCoordGen((GXTexCoordID)(GX_TEXCOORD0 + i), GX_TG_MTX3x4, (GXTexGenSrc)(GX_TG_TEX0 + i), GX_TEXMTX0);
		}
		break;
	}
	default:
		break;
	}
}

//! sets a material
void
CWiiDriver::setMaterial(const SMaterial& material)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	//CurrentMaterial = &material;
	Material = material;

	for (s32 i = MATERIAL_MAX_TEXTURES - 1; i >= 0; --i)
	{
		setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + i),
					 Material.getTextureMatrix(i));
	}
}

//! Set the current 2D Material to be used on the next 2D calls
void
CWiiDriver::set2DMaterial(SMaterial& material)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	// Make sure it's a real 2D material before comparing it to the current
	if( material.getFlag((E_MATERIAL_FLAG)(EMF_ZBUFFER | EMF_LIGHTING | EMF_BACK_FACE_CULLING)) || 
	   (material.getMaterialType() != EMT_2D_ALPHA && material.getMaterialType() != EMT_SOLID) )
	{
		material.setMaterialType( Next2DMaterial.getFlag(EMF_2D_USE_TEXTURE_ALPHA) || 
								  Next2DMaterial.getFlag(EMF_2D_USE_VERTEX_ALPHA) 
										? EMT_2D_ALPHA 
										: EMT_SOLID);
		material.setFlag((E_MATERIAL_FLAG)(EMF_ZBUFFER | EMF_LIGHTING | EMF_BACK_FACE_CULLING), false);
		
	}

	Next2DMaterial = material;
}

//! Change the texture of the current 2D Material
void
CWiiDriver::set2DTexture(const ITexture* tex, bool useTextureAlpha)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);

	if( tex != Next2DMaterial.getTexture(0) ||
		Next2DMaterial.getFlag(EMF_2D_USE_TEXTURE_ALPHA) != useTextureAlpha )
	{
		Next2DMaterial.setMaterialType( useTextureAlpha || Next2DMaterial.getFlag(EMF_2D_USE_VERTEX_ALPHA)  ? EMT_2D_ALPHA : EMT_SOLID);
		Next2DMaterial.setFlag(EMF_2D_USE_TEXTURE_ALPHA, useTextureAlpha);
		Next2DMaterial.setTexture(0,const_cast<ITexture*>(tex));
	}
}

//! Change the EMF_2D_USE_VERTEX_ALPHA flag for the current 2D material
void
CWiiDriver::set2DUseVertexAlpha(bool value)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	if(Next2DMaterial.getFlag(EMF_2D_USE_VERTEX_ALPHA) != value)
	{
		Next2DMaterial.setMaterialType( value || Next2DMaterial.getFlag(EMF_2D_USE_TEXTURE_ALPHA) ? EMT_2D_ALPHA : EMT_SOLID);
		Next2DMaterial.setFlag(EMF_2D_USE_VERTEX_ALPHA,value);
	}
}

video::ITexture*
CWiiDriver::createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data)
{
	return irrnew CWiiTexture(name, w, h, static_cast<GXTexFmt>(fmt), data);
}

//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture*
CWiiDriver::createDeviceDependentTexture(IImage* surface, const char* name)
{
	return irrnew CWiiTexture(surface, name, this);
}

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
//! returns a device dependant texture from a "native" format in a file
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture*
CWiiDriver::createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
													   const c8* hashName,
													   bool refData)
{
	CWiiTexture* texture = irrnew CWiiTexture(file, hashName, this, refData);
	if (texture && !texture->isValid())
	{
		delete texture;
		texture = 0;
	}
	return texture;
}
#endif

ITexture*
CWiiDriver::addRenderTargetTexture(const core::dimension2d<s32>& size,
								   const c8* name,
								   ECOLOR_FORMAT format)
{
	ITexture* rtt = irrnew CWiiTexture(size.Width, size.Height, name, this);
	addTexture(rtt);
	rtt->drop();
	return rtt;
}

//! sets a render target
bool
CWiiDriver::setRenderTarget(video::ITexture* texture,
							int clearMask)
{
	// check if we should set the previous RT back

	//setTexture(0, 0);
	ResetRenderStates = true;
	if (RenderTargetTexture != 0)
	{
		RenderTargetTexture->unbindRTT();
	}

	if (texture)
	{
		// we want to set a new target. so do this.
		GXSetViewport(0, 0, texture->getSize().Width, texture->getSize().Height, 0.0f, 1.0f);
		RenderTargetTexture = static_cast<CWiiTexture*>(texture);
		RenderTargetTexture->bindRTT();
		RenderTargetTexture->setRenderTarget(true);
		CurrentRenderTargetSize = texture->getSize();
	}
	else
	{
		GXSetViewport(0, 0, ScreenSize.Width, ScreenSize.Height, 0.0f, 1.0f);
		RenderTargetTexture = 0;
		CurrentRenderTargetSize = core::dimension2d<s32>(0,0);
	}

	if (clearMask)
	{
		clearBuffers(clearMask);
	}

	return true;
}

//! sets a viewport
void
CWiiDriver::setViewPort(const core::rect<s32>& area)
{
	GXSetViewport(area.UpperLeftCorner.X, 
		area.UpperLeftCorner.Y, 
		area.LowerRightCorner.X - area.UpperLeftCorner.X,   
		area.LowerRightCorner.Y - area.UpperLeftCorner.Y,
		0.0,
		1.0);
}

// returns the current size of the screen or rendertarget
const core::dimension2d<s32>&
CWiiDriver::getCurrentRenderTargetSize() const
{
	if ( CurrentRenderTargetSize.Width == 0 )
		return ScreenSize;
	else
		return CurrentRenderTargetSize;
}

//! gets the area of the current viewport
const core::rect<s32>&
CWiiDriver::getViewPort() const
{
	return ViewPort;
}

void
CWiiDriver::setRenderStates3DMode()
{
	ResetRenderStates = true;
	if (CurrentRenderMode != ERM_3D)
	{
		ResetRenderStates = true;
		
		core::matrix4 gxMat = Matrices[ETS_PROJECTION].getTransposed();
		//gxMat[3] *= -1.0f;
		
		if (gxMat[14] == 0.0f)
		{
			GXSetProjection((f32 (*)[4]) gxMat.pointer(), GX_ORTHOGRAPHIC);
		}
		else
		{
			GXSetProjection((f32 (*)[4]) gxMat.pointer(), GX_PERSPECTIVE);
		}
 		GXLoadPosMtxImm((f32 (*)[4])  gxMat.getTransposed().pointer(), GX_PNMTX0);

		
 		gxMat = Matrices[ETS_VIEW] * Matrices[ETS_WORLD];
 		GXLoadPosMtxImm((f32 (*)[4])  gxMat.getTransposed().pointer(), GX_PNMTX0);
	}

	if (ResetRenderStates || LastMaterial != Material)
	{
		// unset old material
		if (LastMaterial.getMaterialType() != Material.getMaterialType() &&
			LastMaterial.getMaterialType() >= 0 &&
			static_cast<u32>(LastMaterial.getMaterialType()) < MaterialRenderers.size())
		{
			MaterialRenderers[LastMaterial.getMaterialType()].Renderer->onUnsetMaterial();
		}

		// set new material.
		if (Material.getMaterialType() >= 0 && 
			Material.getMaterialType() < (s32)MaterialRenderers.size())
		{
			MaterialRenderers[Material.getMaterialType()].Renderer->onSetMaterial(Material, 
																				  LastMaterial, 
																				  ResetRenderStates, 
																				  NULL);
		}
		
		LastMaterial = Material;
		ResetRenderStates = false;
	}

	if (Material.getMaterialType() >= 0 && 
		Material.getMaterialType() < (s32)MaterialRenderers.size())
	{
		MaterialRenderers[Material.getMaterialType()].Renderer->onRender(NULL, 
																		 video::EVT_STANDARD);
	}

	CurrentRenderMode = ERM_3D;
}

//! Draws a mesh buffer
void
CWiiDriver::drawMeshBuffer(const scene::IMeshBuffer* mb)
{
//	CNullDriver::drawMeshBuffer(mb);
	if (!mb)
		return;

	if (getBinding(mb) == NULL
		|| static_cast<SBinding*>(getBinding(mb))->DisplayListBuffer == NULL)
	{
		SCompileData data;
		beginCompile(&data);
		drawVertexPrimitiveList(mb->getVertices(),
								mb->getIndices(),
								0,
								mb->getVertexCount(),
								mb->getIndexCount() / 3,
								mb->getVertexType(),
								mb->getPrimitiveType(),
								mb->getIndexType(),
								&getBinding(mb));
		endCompile();
	}
	_IRR_DEBUG_BREAK_IF(getBinding(mb) == NULL);
	drawVertexPrimitiveDisplayList(mb->getVertices(),
								   mb->getIndices(),
								   0,
								   mb->getVertexCount(),
								   mb->getIndexCount() / 3,
								   mb->getVertexType(),
								   mb->getPrimitiveType(),
								   mb->getIndexType(),
								   *static_cast<SBinding*>(getBinding(mb)));
}

//! draws a vertex primitive list
void
CWiiDriver::drawVertexPrimitiveDisplayList(const void* vertices,
										   const void* indexList,
										   u32 startIndex,
										   u32 endIndex,
										   u32 primitiveCount,
										   E_VERTEX_TYPE vType,
										   scene::E_PRIMITIVE_TYPE pType,
										   E_INDEX_TYPE iType,
										   SBinding &dl)
{
	if (!primitiveCount || endIndex == startIndex)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	PrimitivesDrawn += primitiveCount;
	
	setRenderStates3DMode();

	switch (vType)
	{
		case EVT_COMPONENT_ARRAYS:
			if (vertices)
			{
				const S3DVertexComponentArrays* components
					= reinterpret_cast<const S3DVertexComponentArrays*>(vertices);

				if (components->PositionType != S3DVertexComponentArrays::ECT_FLOAT)
				{
					const core::vector3df& scale = *components->getPositionScale();
					const core::vector3df& offset = *components->getPositionOffset();
					core::matrix4 transform;
					transform.setScale(scale);
					transform.setTranslation(offset); 
					//{	scale.X, 0,0,0,
					//	0, scale.Y, 0,0,
					//	0, 0, scale.Z,0,
					//	offset.X, offset.Y, offset.Z, 1 };
					
					core::matrix4 gxMat = Matrices[ETS_VIEW] * Matrices[ETS_WORLD] * transform;
					gxMat[2] = -gxMat[2];
					gxMat[6] = -gxMat[6];
					gxMat[10] = -gxMat[10];
					gxMat[14] = -gxMat[14];
 					GXLoadPosMtxImm((f32 (*)[4])gxMat.getTransposed().pointer(), GX_PNMTX0);
				}

				if (components->TexCoord[0].Coord)
				{
					if (components->TexCoord[0].Type != S3DVertexComponentArrays::ECT_FLOAT)
					{
						const core::vector3df& scale = components->TexCoord[0].Scale;
						const core::vector3df& offset = components->TexCoord[0].Offset;
						core::matrix4 transform;
						transform.setScale(scale);
						transform.setTranslation(offset); 
						//{	scale.X,	0,0, 0,
						//	0, scale.Y, 0,0,
						//	0,0,1,0,
						//	offset.X, offset.Y, 0, 1 };

						core::matrix4 gxMat = Matrices[ETS_TEXTURE_0] * transform;
						GXLoadTexMtxImm((const f32 (*)[4])gxMat.getTransposed().pointer(), (GXTexGenSrc)(GX_TEXMTX0), GX_MTX3x4);
					}
				}
			}
			break;
	}
	GXCallDisplayList(dl.DisplayListBuffer, dl.DisplayListUsedSize);
}

//! draws a vertex primitive list
void
CWiiDriver::drawVertexPrimitiveList(const void* vertices,
									const void* indexList,
									u32 startIndex,
									u32 endIndex,
									u32 primitiveCount,
									E_VERTEX_TYPE vType,
									scene::E_PRIMITIVE_TYPE pType,
									E_INDEX_TYPE iType,
									IDriverBinding** binding)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	
	const u16* indices = reinterpret_cast<const u16*>(indexList);

	if (!primitiveCount || startIndex == endIndex)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	setRenderStates3DMode();

	SBinding* b = NULL;
	if (CompileData == NULL)
	{
		CNullDriver::drawVertexPrimitiveList(vertices,
											 indexList,
											 startIndex,
											 endIndex,
											 primitiveCount,
											 vType,
											 pType,
											 iType);
	}
	else
	{
		b = ensureBinding(binding);
		_IRR_DEBUG_BREAK_IF(b == NULL);

		b->flushDisplayListRange();
		if (b->DisplayListBuffer == NULL)
		{
			b->setDisplayListUsedSize(64 * 1024);
		}
		GXBeginDisplayList(b->DisplayListBuffer, b->DisplayListUsedSize);
		GXResetWriteGatherPipe();
	}
	
	GXClearVtxDesc();

	GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	// Get maximum index

	UNIMPLEMENTED("move DCFlushRange before the drawVertexPrimitiveList with the appropriate range")
	//DCFlushRange((void *)vertices, sizeof(S3DVertex) * endIndex);

	switch (vType)
	{
		case EVT_STANDARD:
		{
			if (CompileData == NULL)
			{
				DCFlushRange((void*)vertices, sizeof(S3DVertex) * 64 * 1024);
			}
			GXSetArray(GX_VA_POS, &((reinterpret_cast<const S3DVertex*>(vertices))[0].Pos), sizeof(S3DVertex));
			GXSetArray(GX_VA_NRM, &(reinterpret_cast<const S3DVertex*>(vertices))[0].Normal, sizeof(S3DVertex));
			GXSetArray(GX_VA_TEX0, &(reinterpret_cast<const S3DVertex*>(vertices))[0].TCoords, sizeof(S3DVertex));
			GXSetArray(GX_VA_CLR0, &(reinterpret_cast<const S3DVertex*>(vertices))[0].Color, sizeof(S3DVertex));
		}
		break;

		case EVT_2TCOORDS:
		{
			if (CompileData == NULL)
			{
				DCFlushRange((void *)vertices, sizeof(S3DVertex2TCoords) * endIndex);
			}
			GXSetArray(GX_VA_POS, &(reinterpret_cast<const S3DVertex2TCoords*>(vertices))[0].Pos, sizeof(S3DVertex2TCoords));
			GXSetArray(GX_VA_NRM, &(reinterpret_cast<const S3DVertex2TCoords*>(vertices))[0].Normal, sizeof(S3DVertex2TCoords));
			GXSetArray(GX_VA_TEX0, &(reinterpret_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords, sizeof(S3DVertex2TCoords));
			GXSetArray(GX_VA_TEX1, &(reinterpret_cast<const S3DVertex2TCoords*>(vertices))[0].TCoords2, sizeof(S3DVertex2TCoords));
			GXSetArray(GX_VA_CLR0, &(reinterpret_cast<const S3DVertex2TCoords*>(vertices))[0].Color, sizeof(S3DVertex2TCoords));
		}
		break;

		case EVT_TANGENTS:
		{
			if (CompileData)
			{
				DCFlushRange((void *)vertices, sizeof(S3DVertexTangents) * endIndex);
			}
			UNIMPLEMENTED("EVT_TANGENTS - See GX_VA_NBT alignment");
			GXSetArray(GX_VA_POS, &(reinterpret_cast<const S3DVertexTangents*>(vertices))[0].Pos, sizeof(S3DVertexTangents));
			GXSetArray(GX_VA_NBT, &(reinterpret_cast<const S3DVertexTangents*>(vertices))[0].Normal, sizeof(S3DVertexTangents));
			GXSetArray(GX_VA_TEX0, &(reinterpret_cast<const S3DVertexTangents*>(vertices))[0].TCoords, sizeof(S3DVertexTangents));
			GXSetArray(GX_VA_CLR0, &(reinterpret_cast<const S3DVertexTangents*>(vertices))[0].Color, sizeof(S3DVertexTangents));
		}
		break;

		case EVT_COMPONENT_ARRAYS:
		{
			GXClearVtxDesc();

			if (vertices)
			{
				const S3DVertexComponentArrays* components
					= reinterpret_cast<const S3DVertexComponentArrays*>(vertices);

				if (CompileData == NULL)
				{
					// Flush+Sync all buffers
					DCFlushRangeNoSync(core::stepPointer(components->Position,
														 components->PositionStride * startIndex),
									   components->PositionStride * endIndex);
					if (components->Normal)
					{
						DCFlushRangeNoSync(core::stepPointer(components->Normal,
															 components->NormalStride * startIndex),
										   components->NormalStride * endIndex);
					}
					if (components->TexCoord[0].Coord)
					{
						DCFlushRangeNoSync(core::stepPointer(components->TexCoord[0].Coord,
															 components->TexCoord[0].Stride * startIndex),
										   components->TexCoord[0].Stride * endIndex);
					}
					if (components->Color0)
					{
						DCFlushRangeNoSync(core::stepPointer(components->Color0,
															 components->Color0Stride * startIndex),
										   components->Color0Stride * endIndex);
					}
					PPCSync();

					// Adjust matrix for quantized positions
					core::matrix4 gxWorldViewMat(Matrices[ETS_VIEW] * Matrices[ETS_WORLD]);
					if (components->PositionType != S3DVertexComponentArrays::ECT_FLOAT)
					{
						const core::vector3df& scale = *components->getPositionScale();
						const core::vector3df& offset = *components->getPositionOffset();
						core::matrix4 transform;
						transform.setScale(scale);
						transform.setTranslation(offset); 
						//{	scale.X, 0,0,0,
						//	0, scale.Y, 0,0,
						//	0, 0, scale.Z,0,
						//	offset.X, offset.Y, offset.Z, 1 };
						
						gxWorldViewMat *= transform;
					}
					gxWorldViewMat[2] = -gxWorldViewMat[2];
					gxWorldViewMat[6] = -gxWorldViewMat[6];
					gxWorldViewMat[10] = -gxWorldViewMat[10];
					gxWorldViewMat[14] = -gxWorldViewMat[14];
					GXLoadPosMtxImm((f32 (*)[4])gxWorldViewMat.getTransposed().pointer(), GX_PNMTX0);
				}

				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, (GXCompType)VertexComponentTypeMap[components->PositionType], 0);
				GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
				GXSetArray(GX_VA_POS, components->Position, components->PositionStride);

				if (components->Normal)
				{
					GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
					GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, (GXCompType)VertexComponentTypeMap[components->NormalType], 0);
					GXSetArray(GX_VA_NRM, components->Normal, components->NormalStride);
				}

				if (components->TexCoord[0].Coord)
				{
					if (CompileData == NULL)
					{
						core::matrix4 gxTexMat = Matrices[ETS_TEXTURE_0];
						if (components->TexCoord[0].Type != S3DVertexComponentArrays::ECT_FLOAT)
						{
							const core::vector3df& scale = components->TexCoord[0].Scale;
							const core::vector3df& offset = components->TexCoord[0].Offset;
							core::matrix4 transform;
							transform.setScale(scale);
							transform.setTranslation(offset); 
							//{	scale.X,	0,0, 0,
							//	0, scale.Y, 0,0,
							//	0,0,1,0,
							//	offset.X, offset.Y, 0, 1 };
							transform[10] = 1.0f;

							gxTexMat *= transform;
						}
						GXLoadTexMtxImm((const f32 (*)[4])gxTexMat.getTransposed().pointer(), (GXTexGenSrc)(GX_TEXMTX0), GX_MTX3x4);
					}
					
					GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
					GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, (GXCompType)VertexComponentTypeMap[components->TexCoord[0].Type], 0);
					GXSetArray(GX_VA_TEX0, components->TexCoord[0].Coord, components->TexCoord[0].Stride);
				}

				if (components->Color0)
				{
					GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
					GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
					GXSetArray(GX_VA_CLR0, components->Color0, components->Color0Stride);
				}
			}
		}
		break;
	}

	int nverts = 0;
	switch (pType)
	{
		case scene::EPT_POINTS:
		case scene::EPT_POINT_SPRITES:
		{
			UNIMPLEMENTED("EPT_POINT_SPRITES");
			nverts = primitiveCount;
			GXBegin(GX_POINTS, GX_VTXFMT0, nverts);
		}
			break;
		case scene::EPT_LINE_STRIP:
			nverts = primitiveCount+1;
			GXBegin(GX_LINESTRIP, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_LINE_LOOP:
			UNIMPLEMENTED("Natively unsupported - to be emulated");
			return;
		case scene::EPT_LINES:
			nverts = primitiveCount*2;
			GXBegin(GX_LINES, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_TRIANGLE_STRIP:
			nverts = primitiveCount+2;
			GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_TRIANGLE_FAN:
			nverts = primitiveCount+2;
			GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_TRIANGLES:
			nverts = primitiveCount*3;
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_QUAD_STRIP:
			UNIMPLEMENTED("Natively unsupported - to be emulated");
			return;
		case scene::EPT_QUADS:
			nverts = primitiveCount*4;
			GXBegin(GX_QUADS, GX_VTXFMT0, nverts);
			break;
		case scene::EPT_POLYGON:
			UNIMPLEMENTED("Natively unsupported - to be emulated");
			return;
	}

	if(vType == EVT_COMPONENT_ARRAYS)
	{
		const S3DVertexComponentArrays* components = (const S3DVertexComponentArrays *)vertices;
		int componentCount = 0;
		if (components)
		{
			++componentCount;
		}
		if (components->Normal)
		{
			++componentCount;
		}
		if (components->TexCoord[0].Coord)
		{
			++componentCount;
		}
		if (components->Color0)
		{
			++componentCount;
		}

		switch (componentCount)
		{
			case 1:
			{
				for (int i = 0; i < nverts; ++i, ++indices)
				{
					GXPosition1x16(*indices);
				}
			}
			break;

			case 2:
			{
				for (int i = 0; i < nverts; ++i, ++indices)
				{
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
				}
			}
			break;

			case 3:
			{
				for (int i = 0; i < nverts; ++i, ++indices)
				{
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
				}
			}
			break;

			case 4:
			{
				for (int i = 0; i < nverts; ++i, ++indices)
				{
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
					GXPosition1x16(*indices);
				}
			}
			break;
		}
	}
	else
	{
		for (int i = 0; i < nverts; ++i, ++indices)
		{
			GXPosition1x16(*indices);
			GXNormal1x16(*indices);
			GXColor1x16(*indices);
			GXTexCoord1x16(*indices);
		}
	}

	GXEnd();

	if (CompileData != NULL)
	{
		u32 size = GXEndDisplayList();
		_IRR_DEBUG_BREAK_IF(size > b->DisplayListUsedSize);
		DCInvalidateRange(b->DisplayListBuffer, size);
		b->setDisplayListUsedSize(size);
	}
}

//! Draws a 3d line.
void
CWiiDriver::draw3DLine(const core::vector3df& start,
					   const core::vector3df& end, SColor color)
{
	setRenderStates3DMode();
#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls;
#endif
}

//! Draws a 3d triangle.
void
CWiiDriver::draw3DTriangle(const core::triangle3df& triangle, SColor color)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	
	draw3DLine(triangle.pointA, triangle.pointB, color);
	draw3DLine(triangle.pointB, triangle.pointC, color);
	draw3DLine(triangle.pointC, triangle.pointA, color);
}

//! Draws a 3d axis aligned box.
void
CWiiDriver::draw3DBox(const core::aabbox3d<f32>& box, SColor color)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode == ERM_2D);
	
	core::vector3df edges[8];
	box.getEdges(edges);

	// TODO: optimize into one big drawIndexPrimitive call.

	draw3DLine(edges[5], edges[1], color);
	draw3DLine(edges[1], edges[3], color);
	draw3DLine(edges[3], edges[7], color);
	draw3DLine(edges[7], edges[5], color);
	draw3DLine(edges[0], edges[2], color);
	draw3DLine(edges[2], edges[6], color);
	draw3DLine(edges[6], edges[4], color);
	draw3DLine(edges[4], edges[0], color);
	draw3DLine(edges[1], edges[0], color);
	draw3DLine(edges[3], edges[2], color);
	draw3DLine(edges[7], edges[6], color);
	draw3DLine(edges[5], edges[4], color);
}

//! Draws a part of the texture into the rectangle.
void
CWiiDriver::draw2DImage(const video::ITexture* texture,
						const core::rect<s32>& destRect,
						const core::rect<s32>& sourceRect,
						const core::rect<s32>* clipRect,
						const video::SColor* colors,
						bool useAlphaChannelOfTexture)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	if (!texture)
		return;

	//if (!sourceRect.isValid())
	//	return;

	core::rect<s32> poss(destRect);
	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::rect<f32> tcoords((f32)sourceRect.UpperLeftCorner.X / ss.Width,
							(f32)sourceRect.UpperLeftCorner.Y / ss.Height,
							(f32)sourceRect.LowerRightCorner.X / ss.Width,
							(f32)sourceRect.LowerRightCorner.Y / ss.Height);
	if (clipRect && !clip(poss, tcoords, *clipRect))
	{
		return;
	}

#ifdef _IRR_ENABLE_DRAW2DIMAGE_MANUAL_VIEWPORT_CLIPPING_
	if (!clip(poss, tcoords, core::rect<s32>(core::position2d<s32>(0, 0),
											 getCurrentRenderTargetSize())))
	{
		return;
	}
#endif
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetNumTevStages(1);
    GXSetNumTexGens(1);
    GXSetNumChans(0);

	SColor color(0xff, 0xff, 0xff, 0xff);
	if(colors)
		color = colors[0];

	GXColor trans_fx = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
	GXSetTevColor(GX_TEVREG0, trans_fx);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);	
	
	//GXInvalidateTexAll();
	static_cast<const CWiiTexture*>(texture)->bind(0);

	GXClearVtxDesc();
     
    // set up vertex descriptors
    GXSetVtxDesc( GX_VA_POS,  GX_DIRECT );
    GXSetVtxDesc( GX_VA_TEX0,  GX_DIRECT );
    
    GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_POS,  GX_POS_XY, GX_S16, 0 );
    GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,  GX_F32, 0 );

	const f32 x1 = poss.UpperLeftCorner.X;
	const f32 y1 = poss.UpperLeftCorner.Y;
	const f32 x2 = poss.LowerRightCorner.X;
	const f32 y2 = poss.LowerRightCorner.Y;
	const f32 min_sx = tcoords.UpperLeftCorner.X;
	const f32 max_sx = tcoords.LowerRightCorner.X;
	const f32 min_sy = tcoords.UpperLeftCorner.Y;
	const f32 max_sy = tcoords.LowerRightCorner.Y;

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	GXBegin( GX_QUADS, GX_VTXFMT0, 4 ) ;
	{
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
		GXPosition2s16( x1, y2 ) ; 	  
		GXTexCoord2f32( min_sx, max_sy );

		GXPosition2s16( x2, y2 ) ;  
		GXTexCoord2f32( max_sx, max_sy );

		GXPosition2s16( x2, y1 ) ; 
		GXTexCoord2f32( max_sx, min_sy );

		GXPosition2s16( x1, y1 ) ;	  
		GXTexCoord2f32( min_sx, min_sy );
#else
		GXPosition2s16( x1, y1 ) ;	  
		GXTexCoord2f32( min_sx, min_sy );

		GXPosition2s16( x2, y1 ) ; 
		GXTexCoord2f32( max_sx, min_sy );

		GXPosition2s16( x2, y2 ) ;  
		GXTexCoord2f32( max_sx, max_sy );

		GXPosition2s16( x1, y2 ) ; 	  
		GXTexCoord2f32( min_sx, max_sy );
#endif
	}
	GXEnd();
}

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void
CWiiDriver::draw2DImage(const video::ITexture* texture,
						const core::position2d<s32>& pos,
						const core::rect<s32>& sourceRect,
						const core::rect<s32>* clipRect,
						SColor color,
						bool useAlphaChannelOfTexture)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	core::rect<s32> poss(pos, core::abs(sourceRect.getSize()));
	const core::dimension2d<s32>& ss = texture->getOriginalSize();
	core::rect<f32> tcoords((f32)sourceRect.UpperLeftCorner.X / ss.Width,
							(f32)sourceRect.UpperLeftCorner.Y / ss.Height,
							(f32)sourceRect.LowerRightCorner.X / ss.Width,
							(f32)sourceRect.LowerRightCorner.Y / ss.Height);

	if (clipRect && !clip(poss, tcoords, *clipRect))
	{
		return;
	}

#ifdef _IRR_ENABLE_DRAW2DIMAGE_MANUAL_VIEWPORT_CLIPPING_
	if (!clip(poss, tcoords, core::rect<s32>(core::position2d<s32>(0, 0),
											 getCurrentRenderTargetSize())))
	{
		return;
	}
#endif
	
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetNumTevStages(1);
    GXSetNumTexGens(1);
    GXSetNumChans(0);

	GXColor trans_fx = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
	GXSetTevColor(GX_TEVREG0, trans_fx);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_DISABLE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);	
	
	//GXInvalidateTexAll();
	static_cast<const CWiiTexture*>(texture)->bind(0);

	GXClearVtxDesc();
     
    // set up vertex descriptors
    GXSetVtxDesc( GX_VA_POS,  GX_DIRECT );
    GXSetVtxDesc( GX_VA_TEX0,  GX_DIRECT );
    
	GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_POS,  GX_POS_XY, GX_S16, 0 );
    GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,  GX_F32, 0 );

	const f32 x1 = poss.UpperLeftCorner.X;
	const f32 y1 = poss.UpperLeftCorner.Y;
	const f32 x2 = poss.LowerRightCorner.X;
	const f32 y2 = poss.LowerRightCorner.Y;
	const f32 min_sx = tcoords.UpperLeftCorner.X;
	const f32 max_sx = tcoords.LowerRightCorner.X;
	const f32 min_sy = tcoords.UpperLeftCorner.Y;
	const f32 max_sy = tcoords.LowerRightCorner.Y;

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	GXBegin( GX_QUADS, GX_VTXFMT0, 4 ) ;
	{
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
		GXPosition2s16( x1, y2 ) ; 	  
		GXTexCoord2f32( min_sx, max_sy );

		GXPosition2s16( x2, y2 ) ;  
		GXTexCoord2f32( max_sx, max_sy );

		GXPosition2s16( x2, y1 ) ; 
		GXTexCoord2f32( max_sx, min_sy );

		GXPosition2s16( x1, y1 ) ;	  
		GXTexCoord2f32( min_sx, min_sy );
#else
		GXPosition2s16( x1, y1 ) ;	  
		GXTexCoord2f32( min_sx, min_sy );

		GXPosition2s16( x2, y1 ) ; 
		GXTexCoord2f32( max_sx, min_sy );

		GXPosition2s16( x2, y2 ) ;  
		GXTexCoord2f32( max_sx, max_sy );

		GXPosition2s16( x1, y2 ) ; 	  
		GXTexCoord2f32( min_sx, max_sy );
#endif
	}
	GXEnd() ;
}

//!Draws an 2d rectangle with a gradient.
void
CWiiDriver::draw2DRectangle(const core::rect<s32>& position,
							SColor colorLeftUp,
							SColor colorRightUp,
							SColor colorLeftDown,
							SColor colorRightDown,
							const core::rect<s32>* clip)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;


	// setup TEV for vertex color only
	GXSetNumTevStages(1);
	GXSetNumChans(1);
	GXSetNumTexGens(0);			

	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);

	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    // set up vertex descriptors
	GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS,  GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0,  GX_DIRECT);
    
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XY, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA,  GX_RGBA8, 0);

	const f32 x1 = pos.UpperLeftCorner.X;
	const f32 y1 = pos.UpperLeftCorner.Y;
	const f32 x2 = pos.LowerRightCorner.X;
	const f32 y2 = pos.LowerRightCorner.Y;

	u8 test = colorLeftUp.getRed();

#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif

	GXBegin( GX_QUADS, GX_VTXFMT0, 4 ) ;
	{
#ifdef _IRR_USE_RIGHT_HAND_CONVENTION_
		GXPosition2s16(x1, y2) ; 	  
		GXColor4u8(colorLeftDown.R, colorLeftDown.G, colorLeftDown.B, colorLeftDown.A);

		GXPosition2s16(x2, y2) ;  
		GXColor4u8(colorRightDown.R, colorRightDown.G, colorRightDown.B, colorRightDown.A);

		GXPosition2s16(x2, y1) ; 
		GXColor4u8(colorRightUp.R, colorRightUp.G, colorRightUp.B, colorRightUp.A);

		GXPosition2s16(x1, y1) ;	  
		GXColor4u8(colorLeftUp.R, colorLeftUp.G, colorLeftUp.B, colorLeftUp.A);
#else
		GXPosition2s16(x1, y1) ;	  
		GXColor4u8(colorLeftUp.R, colorLeftUp.G, colorLeftUp.B, colorLeftUp.A);

		GXPosition2s16(x2, y1) ; 
		GXColor4u8(colorRightUp.R, colorRightUp.G, colorRightUp.B, colorRightUp.A);

		GXPosition2s16(x2, y2) ;  
		GXColor4u8(colorRightDown.R, colorRightDown.G, colorRightDown.B, colorRightDown.A);

		GXPosition2s16(x1, y2) ; 	
		GXColor4u8(colorLeftDown.R, colorLeftDown.G, colorLeftDown.B, colorLeftDown.A);
#endif
	}
	GXEnd() ;
}

//! Draws a 2d line.
void
CWiiDriver::draw2DLine(const core::position2d<s32>& start,
					   const core::position2d<s32>& end,
					   SColor color)
{
	_IRR_DEBUG_BREAK_IF(CurrentRenderMode != ERM_2D);
	
	int a = 1;
#ifdef _IRR_WITH_FRAME_STATISTICS_
	++DrawCalls2D;
#endif
}

//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void
CWiiDriver::setAmbientLight(const SColorf& color)
{
	AmbiantLightColor = color;
}

//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
//! driver, it would return "Direct3D8".
const wchar_t*
CWiiDriver::getName() const
{
	return L"Irrlicht WiiDevice";
}

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void
CWiiDriver::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
}

//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void
CWiiDriver::drawStencilShadow(bool clearStencilBuffer,
							  video::SColor leftUpEdge,
							  video::SColor rightUpEdge,
							  video::SColor leftDownEdge,
							  video::SColor rightDownEdge)
{
}

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32
CWiiDriver::getMaximalPrimitiveCount() const
{
	return 0xFFFFFFFF;
}

//! Returns type of video driver
E_DRIVER_TYPE
CWiiDriver::getDriverType() const
{
	return EDT_WII;
}

void
CWiiDriver::createMaterialRenderers()
{
	// create Wii material renderers

	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID_2_LAYER(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID_2_LAYER_DECAL(this));

	// add the same renderer for all lightmap types
	CWiiMaterialRenderer_LIGHTMAP* lmr = irrnew CWiiMaterialRenderer_LIGHTMAP(this);
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
	lmr->drop();

	// add remaining material renderer
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_DETAIL_MAP(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SPHERE_MAP(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_REFLECTION_2_LAYER(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));

	//// add normal map renderers (solid is temporary)
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	//s32 tmp = 0;
	//video::IMaterialRenderer* renderer = 0;
	//renderer = irrnew CWiiNormalMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	//renderer->drop();
	//renderer = irrnew CWiiNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	//renderer->drop();
	//renderer = irrnew CWiiNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	//renderer->drop();

	//// add parallax map renderers (solid is temporary)
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_SOLID(this));
	//renderer = irrnew CWiiParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
	//renderer->drop();
	//renderer = irrnew CWiiParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
	//renderer->drop();
	//renderer = irrnew CWiiParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
	//renderer->drop();

	// add basic 1 texture blending
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_ONETEXTURE_BLEND(this));

	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(this));
	
	// add the 2D material renderers
	addAndDropMaterialRenderer(irrnew CWiiMaterialRenderer_2D_ALPHA(this));
}

void
CWiiDriver::setBasicRenderStates(const SMaterial& material, 
								 const SMaterial& lastmaterial,
								 bool resetAllRenderStates)
{
	if (material.getFlag(EMF_ZWRITE_ENABLE) && (getOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT)
												|| !material.isTransparent()))
	{
		GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	}
	else
	{
		GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	}
	/*
	// zbuffer
	if (resetAllRenderStates || 
		lastmaterial.getFlag(EMF_ZBUFFER) != material.getFlag(EMF_ZBUFFER) || 
		lastmaterial.getFlag(EMF_ZWRITE_ENABLE) != material.getFlag(EMF_ZWRITE_ENABLE) ||
		lastmaterial.getZBufferFunc() != material.getZBufferFunc())
	{
		GXSetZMode(material.getFlag(EMF_ZBUFFER), 
				   ZBufferFuncMap[material.getZBufferFunc()], 
				   material.getFlag(EMF_ZWRITE_ENABLE));
	}
	*/
	/// culling
	if (Material.getFlag(EMF_BACK_FACE_CULLING) && Material.getFlag(EMF_FRONT_FACE_CULLING))
		GXSetCullMode(GX_CULL_ALL);
	else if(Material.getFlag(EMF_BACK_FACE_CULLING))
		GXSetCullMode(GX_CULL_FRONT);
	else if(Material.getFlag(EMF_FRONT_FACE_CULLING))
		GXSetCullMode(GX_CULL_BACK);
	else
		GXSetCullMode(GX_CULL_NONE);
}

bool
CWiiDriver::setTexture(u32 stage, const ITexture* texture)
{
	if (stage >= MATERIAL_MAX_TEXTURES)
	{
		return false;
	}

// 	if (CurrentTexture[stage] == texture)
// 	{
// 		if (texture && texture->isDirty())
// 		{
// 			_IRR_DEBUG_BREAK_IF(texture->getDriverType() != EDT_WII);
// 			static_cast<const CWiiTexture*>(texture)->updateParameters(); 
// 		}
// 		return true;
// 	}

	CurrentTexture[stage] = texture;

	if (!texture)
	{
		// no texture
		GXSetNumTexGens(0); 
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		return true;
	}
	else
	{
		if (texture->getDriverType() != EDT_WII)
		{
			// no texture
			GXSetNumTexGens(0); 
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

			os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
			return false;
		}

#ifdef _IRR_WITH_FRAME_STATISTICS_
		++TextureBindings;
#endif
		static_cast<const CWiiTexture*>(texture)->bind(stage);
	}
	return true;
}

//! deletes all dynamic lights there are
void
CWiiDriver::deleteAllDynamicLights()
{
	/*for (s32 i=0; i<LastSetLight+1; ++i)
		glDisable(GL_LIGHT0 + i);*/

	LastSetLight = -1;

	CNullDriver::deleteAllDynamicLights();
}

//! returns the maximal amount of dynamic lights the device can handle
u32
CWiiDriver::getMaximalDynamicLightAmount() const
{
	return 8;
}

//! adds a dynamic light
void
CWiiDriver::addDynamicLight(const SLight& light)
{
	if (LastSetLight == MaxLight - 1)
		return;

		// set diffuse color
	GXColor gx = {0, 0, 0, 255};
	light.DiffuseColor.toSColor().toRGBA8((irr::u8 *)&gx);

	if(*(int*)&gx == 0)
	{
		return;
	}

	//setTransform(ETS_WORLD, core::matrix4());
		
	core::matrix4 modelView = getTransform(ETS_VIEW) * light.AbsoluteTransformation;
	modelView[2] = -modelView[2];
	modelView[6] = -modelView[6];
	modelView[10] = -modelView[10];
	modelView[14] = -modelView[14];
	
	core::vector3df lightPos = modelView.getTranslation(), lightDir = core::vector3df(.0f,.0f,-1.0f);
	modelView.rotateVect(lightDir);
	lightDir.normalize();

	++LastSetLight;
	CNullDriver::addDynamicLight(light);

	s32 lidx = 1 << LastSetLight; // 1, 2, 4, 8, 16, 32, 64, 128 ...
	f32 data[4];
	GXLightObj lt_obj;

	GXInitLightColor(&lt_obj, gx);
	
	switch (light.Type)
	{
	case video::ELT_SPOT:
		//viewMatrix.transformVect(lightPos);
		//viewMatrix.transformVect(lightDir);
		GXInitLightPos(&lt_obj, lightPos.X, lightPos.Y, lightPos.Z);
		GXInitLightDir(&lt_obj, lightDir.X, lightDir.Y, lightDir.Z);
		GXInitLightSpot(&lt_obj, light.OuterCone, GX_SP_COS);
		GXInitLightAttnK(&lt_obj, light.Attenuation.X, light.Attenuation.Y, light.Attenuation.Z);	
	break;
	case video::ELT_POINT:
		//viewMatrix.transformVect(lightPos);
		GXInitLightPos(&lt_obj, lightPos.X, lightPos.Y, lightPos.Z);
	break;
	case video::ELT_DIRECTIONAL:
		// set direction
		//viewMatrix.transformVect(lightDir);
		GXInitLightPos(&lt_obj, 
		-lightDir.X * 10000.0f, 
		-lightDir.Y * 10000.0f,
		-lightDir.Z * 10000.0f);
		//GXInitLightDir(&lt_obj, -light.Direction.X, light.Direction.Y, -light.Direction.Z);
	break;
	}


	// set specular color


	// set ambient color


	// 1.0f / (constant + linear * d + quadratic*(d*d);

	// set attenuation
	
	GXLoadLightObjImm(&lt_obj, (GXLightID)lidx);

}

//! creates a video driver
IVideoDriver*
createWiiDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize, CIrrDeviceWii *pDevice)
{
	return irrnew CWiiDriver(io, screenSize, pDevice);
}

void
CWiiDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
}

void
CWiiDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
}

bool
CWiiDriver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	return true;
}

bool
CWiiDriver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	return true;
}

IVideoDriver*
CWiiDriver::getVideoDriver() 
{ 
	return (Device) ? Device->getVideoDriver() : 0; 
}

//! Set alpha/color mask to write in color buffer.
void
CWiiDriver::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	GXSetColorUpdate(red || green || blue);
	GXSetAlphaUpdate(alpha);
}

const SColorf&
CWiiDriver::getAmbientLight() const
{
	return AmbiantLightColor;
}

//!
void
CWiiDriver::beginCompile(ICompileData* data)
{
	_IRR_DEBUG_BREAK_IF(data == NULL);
	CompileData = static_cast<SCompileData*>(data);
}

//!
void
CWiiDriver::endCompile()
{
	CompileData = NULL;
}

//!
E_DRIVER_ALLOCATION_RESULT
CWiiDriver::getProcessBuffer(
	u32 vertexStart,
	u32 vertexEnd,
	u32 attributes,
	E_PROCESS_BUFFER_TYPE type,
	S3DVertexComponentArrays& components,
	IDriverBinding** binding,
	bool allocate
)
{
	E_DRIVER_ALLOCATION_RESULT result = EDAR_SUCCESS;
	switch (type)
	{
		case EPBT_DYNAMIC :
		{
			if (binding && *binding)
			{
				SBinding* b = static_cast<SBinding*>(*binding);
				if (b->ProcessBuffer != NULL)
				{
					result = EDAR_NOT_FOUND;
					break;
				}
			}
			u32 stride = 0;
			void* buf = allocateProcessBuffer(
				vertexStart,
				vertexEnd,
				attributes,
				components,
				SBinding::getProcessBufferHeapAllocator(),
				stride
			);
			if (buf == NULL)
			{
				result = EDAR_FAILED;
			}
		}
		break;

		case EPBT_STATIC :
		{
			SBinding* b = ensureBinding(binding);
			_IRR_DEBUG_BREAK_IF(b == NULL);
			result = b->getProcessBuffer(vertexStart,
										 vertexEnd,
										 attributes,
										 components,
										 allocate);
		}
		break;
	}
	return result;
}

//!
/** \param vertexStart Index of first vertex in buffer starting at
	bindingOrDynamicBuffer. To be set only when releasing a process buffer of
	type EPBT_DYNAMIC.
	\param stride Vertex stride in buffer bindingOrDynamicBuffer. To be set
	only when releasing a process buffer of type EPBT_DYNAMIC. */
void
CWiiDriver::releaseProcessBuffer(E_PROCESS_BUFFER_TYPE type,
								 void* bindingOrDynamicBuffer,
								 u32 vertexStart,
								 u32 stride)
{
	_IRR_DEBUG_BREAK_IF(bindingOrDynamicBuffer == NULL);
	switch (type)
	{
		case EPBT_DYNAMIC :
		{
			irr::releaseProcessBuffer(
				reinterpret_cast<u8*>(bindingOrDynamicBuffer)
				+ vertexStart * stride
			);
		}
		break;

		case EPBT_STATIC :
		{
			SBinding* b = static_cast<SBinding*>(bindingOrDynamicBuffer);
			b->clearProcessBuffer();
		}
		break;
	}
}

CWiiDriver::SBinding*
CWiiDriver::ensureBinding(IDriverBinding** binding)
{
	_IRR_DEBUG_BREAK_IF(binding == NULL);
	if (*binding == NULL)
	{
		*binding = irrnew SBinding(this);
	}
	return static_cast<SBinding*>(*binding);
}

video::IDriverBinding* CWiiDriver::createBinding()
{
	return irrnew SBinding(this);
}

} // end namespace
} // end namespace

#endif _IRR_COMPILE_WITH_WII_

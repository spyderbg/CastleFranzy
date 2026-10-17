//-*-c++-*-
// Copyright (C) 2002-2009 Gameloft
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_WII_H_INCLUDED__
#define __C_VIDEO_WII_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_USE_WII_DEVICE_

#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IImagePresenter.h"
//#include "IGPUProgrammingServices.h"
#include "IMaterialRendererServices.h"
#include "irrArray.h"
#include "irrString.h"
#include "IAttributes.h"
#include "IMeshBuffer.h"
#include "CFPSCounter.h"
#include "CNullDriver.h"
//#include "CWiiDriverEx.h"
#include "S3DVertex.h"
#include "SLight.h"
#include "SExposedVideoData.h"
#include "CWiiTexture.h"
#include "SProcessBufferBindingBase.h"

//#include <revolution/gx.h>

namespace irr
{

class CIrrDeviceWii;

namespace io
{

class IWriteFile;
class IReadFile;

} // end namespace io

namespace video
{

class IImageLoader;
class IImageWriter;

class CWiiDriver : public CNullDriver
{
public:

	//! constructor
	CWiiDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize, CIrrDeviceWii*);

	//! destructor
	virtual ~CWiiDriver();

	//!
	struct SBinding
		: SProcessBufferBindingBase<SBinding, IDriverBinding>
	{
	public:

		CWiiDriver* Driver;
		u8* DisplayListBuffer;
		u32 DisplayListUsedSize;

		//!
		explicit SBinding(CWiiDriver* driver);

		//!
		virtual ~SBinding();

		//!
		CWiiDriver* getDriver() const
		{
			return Driver;
		}

		//!
		virtual E_DRIVER_TYPE getDriverType() const;

		//!
		virtual void onDelete();

		//!
		void setDisplayListUsedSize(u32 usedSize);

		//!
		void flushDisplayListRange()
		{
			if (DisplayListBuffer)
			{
				DCFlushRange(DisplayListBuffer, DisplayListUsedSize);
			}
		}
	};

	//!
	struct SCompileData
		: ICompileData
	{
		// no field required so far

		virtual E_COMPILE_DATA_TYPE getType() const;
	};

	virtual bool beginScene();

	/** \note On Wii, swapBuffers(int) should be called before beginScene(). */
	virtual bool swapBuffers(int clearMask = EFB_COLOR | EFB_DEPTH);

	virtual void clearBuffers(int);
	virtual void setClearColor(const SColor& color);
	virtual SColor getClearColor() const;
	virtual void setClearDepth(float);
	virtual float getClearDepth() const;
	virtual void setClearStencil(int);
	virtual int getClearStencil() const;

	virtual bool beginScene2D();
	virtual bool endScene2D();

	const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;
		
	//! sets transformation
	virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

	//! sets a material
	virtual void setMaterial(const SMaterial& material);

	//! Set a 2D Material if it's different from the next material to be used
	void set2DMaterial(SMaterial& material);
	
	//! Set the texture to be used with 2D render
	void set2DTexture(const ITexture* tex, bool useTextureAlpha = false);
	void set2DUseVertexAlpha(bool value);

	//! sets a render target
	virtual bool setRenderTarget(video::ITexture* texture,
								 int clearMask);

	//! Get the size of the current render target
	/** This method will return the screen size if the driver
		doesn't support render to texture, or if the current render
		target is the screen.
		\return Size of render target or screen/window */
	virtual const core::dimension2d<s32>& getCurrentRenderTargetSize() const;

	//! sets a viewport
	virtual void setViewPort(const core::rect<s32>& area);

	//! gets the area of the current viewport
	virtual const core::rect<s32>& getViewPort() const;

	//! Draws a mesh buffer
	virtual void drawMeshBuffer(const scene::IMeshBuffer* mb);

	//!
	void drawVertexPrimitiveDisplayList(const void* vertices,
										const void* indexList,
										u32 startIndex,
										u32 endIndex,
										u32 primitiveCount,
										E_VERTEX_TYPE vType,
										scene::E_PRIMITIVE_TYPE pType,
										E_INDEX_TYPE iType,
										SBinding& dl);

	//! draws a vertex primitive list
	virtual void drawVertexPrimitiveList(const void* vertices,
										 const void* indexList,
										 u32 minIndex,
										 u32 maxIndex,
										 u32 primitiveCount,
										 E_VERTEX_TYPE vType,
										 scene::E_PRIMITIVE_TYPE pType,
										 E_INDEX_TYPE iType,
										 IDriverBinding** binding = NULL);
	
	//! Draw a 2D surface with the current 2D Material
	void draw2DRectangle(const core::rect<s32>& destRects,
					     const core::rect<s32>& sourceRects,
					     core::array<SColor>& colors,
					     const core::rect<s32>* clipRect = 0);

	//! Draws a 3d line.
	virtual void draw3DLine(const core::vector3df& start,
							const core::vector3df& end, 
							SColor color = SColor(255,255,255,255));

	//! Draws a 3d triangle.
	virtual void draw3DTriangle(const core::triangle3df& triangle,
								SColor color = SColor(255,255,255,255));

	//! Draws a 3d axis aligned box.
	virtual void draw3DBox(const core::aabbox3d<f32>& box,
						   SColor color = SColor(255,255,255,255));

	//! draws an 2d image
	virtual void draw2DImage(const video::ITexture* texture, 
						     const core::rect<s32>& destRect,
							 const core::rect<s32>& sourceRect, 
							 const core::rect<s32>* clipRect,
							 const video::SColor* colors, 
							 bool useAlphaChannelOfTexture);

	//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
	virtual void draw2DImage(const video::ITexture* texture, 
							 const core::position2d<s32>& destPos,
							 const core::rect<s32>& sourceRect,
							 const core::rect<s32>* clipRect, 
							 SColor color,
							 bool useAlphaChannelOfTexture);

	//!Draws an 2d rectangle with a gradient.
	virtual void draw2DRectangle(const core::rect<s32>& pos,
								 SColor colorLeftUp, 
								 SColor colorRightUp, 
								 SColor colorLeftDown, 
								 SColor colorRightDown,
								 const core::rect<s32>* clip = 0);

	//! Draws a 2d line. 
	virtual void draw2DLine(const core::position2d<s32>& start,
							const core::position2d<s32>& end,
							SColor color=SColor(255,255,255,255));
	
	//! returns the maximal amount of dynamic lights the device can handle
	virtual u32 getMaximalDynamicLightAmount() const;

	//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
	//! driver, it would return "Direct3D8.1".
	virtual const wchar_t* getName() const;

	//! Sets the dynamic ambient light color. The default color is
	//! (0,0,0,0) which means it is dark.
	//! \param color: New color of the ambient light.
	virtual void setAmbientLight(const SColorf& color);

	//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
	//! this: Frist, draw all geometry. Then use this method, to draw the shadow
	//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
	virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail=true);

	//! Fills the stencil shadow with color. After the shadow volume has been drawn
	//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
	//! to draw the color of the shadow. 
	virtual void drawStencilShadow(bool clearStencilBuffer=false, 
								   video::SColor leftUpEdge = video::SColor(0,0,0,0), 
								   video::SColor rightUpEdge = video::SColor(0,0,0,0),
								   video::SColor leftDownEdge = video::SColor(0,0,0,0),
								   video::SColor rightDownEdge = video::SColor(0,0,0,0));
	
	//! Returns the maximum amount of primitives (mostly vertices) which
	//! the device is able to render with one drawIndexedTriangleList
	//! call.
	virtual u32 getMaximalPrimitiveCount() const;
	
	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	//! Adds a dynamic light.
	/** \param light Data specifying the dynamic light. */
	virtual void addDynamicLight(const SLight& light);

	//! deletes all dynamic lights there are
	virtual void deleteAllDynamicLights();

	// Render Services
	virtual void setBasicRenderStates(const SMaterial& material, 
									  const SMaterial& lastmaterial, 
									  bool resetAllRenderstates);
									  
	virtual void setVertexShaderConstant(const f32* data, 
										 s32 startRegister, 
										 s32 constantAmount=1);
										 
	virtual void setPixelShaderConstant(const f32* data, 
										s32 startRegister, 
										s32 constantAmount=1);
										
	virtual bool setVertexShaderConstant(const c8* name, 
										 const f32* floats, 
										 int count);
										 
	virtual bool setPixelShaderConstant(const c8* name, 
										const f32* floats, 
										int count);
										
	virtual IVideoDriver* getVideoDriver();

	virtual void setColorMask(bool red, bool green, bool blue, bool alpha);

	bool setTexture(u32 stage, const ITexture* texture);

	virtual const SColorf& getAmbientLight() const;

	//!
	virtual void beginCompile(ICompileData* data);

	//!
	virtual void endCompile();

	//!
	virtual E_DRIVER_ALLOCATION_RESULT getProcessBuffer(
		u32 vertexStart,
		u32 vertexEnd,
		u32 attributes,
		E_PROCESS_BUFFER_TYPE type,
		S3DVertexComponentArrays& components,
		IDriverBinding** binding = NULL,
		bool allocate = false
	);

	//!
	/** \param vertexStart Index of first vertex in buffer starting at
		bindingOrDynamicBuffer. To be set only when releasing a process buffer of
		type EPBT_DYNAMIC.
		\param stride Vertex stride in buffer bindingOrDynamicBuffer. To be set
		only when releasing a process buffer of type EPBT_DYNAMIC. */
	virtual void releaseProcessBuffer(E_PROCESS_BUFFER_TYPE type,
									  void* bindingOrDynamicBuffer,
									  u32 vertexStart = 0,
									  u32 stride = 0);

	virtual video::IDriverBinding* createBinding();

protected:

	//!
	SBinding* ensureBinding(IDriverBinding** binding);

	// enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
	enum E_RENDER_MODE
	{
		ERM_NONE = 0,	// no render state has been set yet.
		ERM_2D,			// 2d drawing rendermode
		ERM_3D,			// 3d rendering mode
	};

	E_RENDER_MODE CurrentRenderMode;

	bool ResetRenderStates;
	SMaterial Material, LastMaterial;
	
	SMaterial Next2DMaterial;

	//! returns a device dependent texture from a software surface (IImage)
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_		
	//! returns a device dependant texture from a "native" format in a file
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
																		const c8* hashName = 0,
																		bool refData = false);
#endif

	virtual video::ITexture* createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data);

	//! Adds a new render target texture to the texture cache.
	/** \param size Size of the texture, in pixels. Width and
		height should be a power of two (e.g. 64, 128, 256, 512, ...)
		and it should not be bigger than the backbuffer, because it
		shares the zbuffer with the screen buffer.
		\param name An optional name for the RTT.
		\return Pointer to the created texture or 0 if the texture
		could not be created. This pointer should not be dropped. See
		IReferenceCounted::drop() for more information. */
	virtual ITexture* addRenderTargetTexture(const core::dimension2d<s32>& size, const c8* name = 0, ECOLOR_FORMAT format = ECF_A8R8G8B8);
		
	void createMaterialRenderers();

	void setRenderStates3DMode();

	CIrrDeviceWii* Device;
	//const SMaterial* CurrentMaterial;
	CWiiTexture* RenderTargetTexture;
	const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
	u8* ClearFrameBufferData;
	core::dimension2d<s32> CurrentRenderTargetSize;
	core::matrix4 Matrices[ETS_COUNT];
	
	core::matrix4 ProjectionMatrixBackup;
	core::matrix4 WorldMatrixBackup;
	core::matrix4 ViewMatrixBackup;

	u8 LastSetLight;
	static const u8 MaxLight = 8;

	SColorf AmbiantLightColor;

	SColor ClearColor;
	u32 ClearDepth;

	SCompileData* CompileData;
};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_WII_

#endif

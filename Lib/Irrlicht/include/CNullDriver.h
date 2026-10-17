//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_NULL_H_INCLUDED__
#define __C_VIDEO_NULL_H_INCLUDED__

#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IImagePresenter.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"
#include "irrMap.h"
#include "IAttributes.h"
#include "IMeshBuffer.h"
#include "CFPSCounter.h"
#include "S3DVertex.h"
#include "SVertexIndex.h"
#include "SLight.h"
#include "SExposedVideoData.h"

namespace irr
{
namespace io
{

class IWriteFile;
class IReadFile;

} // end namespace io

namespace video
{
class CTextureManager;
class IImageLoader;
class IImageWriter;

class CNullDriver : public IVideoDriver, public IGPUProgrammingServices
{
public:
	friend class CTextureManager;
	//! constructor
	CNullDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize);

	//! destructor
	virtual ~CNullDriver();

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool swapBuffers(int);

	virtual void clearBuffers(int);
	virtual void setClearColor(const SColor& color);
	virtual SColor getClearColor() const;
	virtual void setClearDepth(float);
	virtual float getClearDepth() const;
	virtual void setClearStencil(int);
	virtual int getClearStencil() const;

	virtual bool beginScene2D();
	virtual bool endScene2D();

	//! Disable a feature of the driver.
	virtual void disableFeature(E_VIDEO_DRIVER_FEATURE feature, bool flag=true);

	//! queries the features of the driver, returns true if feature is available
	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	//! sets transformation
	virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

	//! sets a material
	virtual void setMaterial(const SMaterial& material);
	
		//! if value is true, the getTexture will try to use getAbsolutePath to prevent dupication happen
	virtual void setTextureFullPathMode(bool value);

	//! Set a 2D Material if it's different from the next material to be used
	virtual void set2DMaterial(SMaterial& material);
	
	//! Set the texture to be used with 2D render
	virtual void set2DTexture(const ITexture* tex, bool useTextureAlpha = false);

	//! Set if the vertex alpha will be used for the 2D render
	virtual void set2DUseVertexAlpha(bool value);
		
	//! loads a Texture
	virtual ITexture* getTexture(const c8* filename);

	//! loads a Texture
	virtual ITexture* getTexture(const core::stringc& filename)
	{ return getTexture(filename.c_str()); }

	//! loads a Texture
	virtual ITexture* getTexture(io::IReadFile* file, bool refData = false);

	//! Returns a texture by index
	virtual ITexture* getTextureByIndex(u32 index);

	//! Returns amount of textures currently loaded
	virtual u32 getTextureCount() const;

	//! Renames a texture
	virtual void renameTexture(ITexture* texture, const c8* newName);

	//! creates a Texture
	virtual ITexture* addTexture(const core::dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format = ECF_A8R8G8B8);

	//! sets a render target
	virtual bool setRenderTarget(video::ITexture* texture,
								 int clearMask);

	//! sets a viewport
	virtual void setViewPort(const core::rect<s32>& area);

	//! gets the area of the current viewport
	virtual const core::rect<s32>& getViewPort() const;

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

	//! Draws a 3d line.
	virtual void draw3DLine(const core::vector3df& start,
							const core::vector3df& end, SColor color = SColor(255,255,255,255));

	//! Draws a 3d triangle.
	virtual void draw3DTriangle(const core::triangle3df& triangle,
								SColor color = SColor(255,255,255,255));

	//! Draws a 3d axis aligned box.
	virtual void draw3DBox(const core::aabbox3d<f32>& box,
						   SColor color = SColor(255,255,255,255));

	//! draws an 2d image
	virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos);

	//! draws a set of 2d images, using a color and the alpha
	/** channel of the texture if desired. The images are drawn
		beginning at pos and concatenated in one line. All drawings
		are clipped against clipRect (if != 0).
		The subtextures are defined by the array of sourceRects
		and are chosen by the indices given.
		\param texture: Texture to be drawn.
		\param pos: Upper left 2d destination position where the image will be drawn.
		\param sourceRects: Source rectangles of the image.
		\param indices: List of indices which choose the actual rectangle used each time.
		\param kerningWidth: offset on position
		\param clipRect: Pointer to rectangle on the screen where the image is clipped to.
		This pointer can be 0. Then the image is not clipped.
		\param color: Color with which the image is colored.
		Note that the alpha component is used: If alpha is other than 255, the image will be transparent.
		\param useAlphaChannelOfTexture: If true, the alpha channel of the texture is
		used to draw the image. */
	virtual void draw2DImage(const video::ITexture* texture,
							 const core::position2d<s32>& pos,
							 const core::array<core::rect<s32> >& sourceRects,
							 const core::array<s32>& indices,
							 s32 kerningWidth = 0,
							 const core::rect<s32>* clipRect = 0,
							 SColor color = SColor(255,255,255,255),
							 bool useAlphaChannelOfTexture = false);

	//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
	virtual void draw2DImage(const video::ITexture* texture,
							 const core::position2d<s32>& destPos,
							 const core::rect<s32>& sourceRect,
							 const core::rect<s32>* clipRect = 0,
							 SColor color = SColor(255,255,255,255),
							 bool useAlphaChannelOfTexture = false);

	//! Draws a part of the texture into the rectangle.
	virtual void draw2DImage(const video::ITexture* texture,
							 const core::rect<s32>& destRect,
							 const core::rect<s32>& sourceRect,
							 const core::rect<s32>* clipRect = 0,
							 const video::SColor* const colors = 0,
							 bool useAlphaChannelOfTexture = false);

	//! draw an 2d rectangle
	virtual void draw2DRectangle(SColor color,
								 const core::rect<s32>& pos,
								 const core::rect<s32>* clip = 0);

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
							SColor color = SColor(255,255,255,255));

	//! Draws a non filled concyclic reqular 2d polyon.
	virtual void draw2DPolygon(core::position2d<s32> center,
							   f32 radius,
							   SColor Color,
							   s32 vertexCount);

	//! Draw a 2D surface with the current 2D Material, this will replace all the draw2DImage and draw2DRectangle
	virtual void draw2DRectangle(const core::rect<s32>& destRect,
							     const core::rect<s32>& sourceRect,
							     const SColor colors[4],
							     const core::rect<s32>* clipRect = 0);

	virtual void setFog(SColor color=SColor(0,255,255,255), bool linearFog=true,
						f32 start=50.0f, f32 end=100.0f,
						f32 density=0.01f, bool pixelFog=false, bool rangeFog=false);

	//! get color format of the current color buffer
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! get screen size
	virtual const core::dimension2d<s32>& getScreenSize() const;

	//! get render target size
	virtual const core::dimension2d<s32>& getCurrentRenderTargetSize() const;

	// get current frames per second value
	virtual s32 getFPS() const;

	//! returns amount of primitives (mostly triangles) were drawn in the last frame.
	//! very useful method for statistics.
	virtual u32 getPrimitiveCountDrawn( u32 param = 0 ) const;

	//! returns the number of non-2D draw calls in the last frame.
	//! very useful method for statistics.
	virtual u32 getDrawCallCount() const;

	//! returns the number of 2D draw calls in the last frame.
	//! very useful method for statistics.
	virtual u32 getDrawCall2DCount() const;

	//! returns the number of texture bindings in the last frame.
	//! very useful method for statistics.
	virtual u32 getTextureBindingCount() const;

	//! deletes all dynamic lights there are
	virtual void deleteAllDynamicLights();

	//! adds a dynamic light
	virtual void addDynamicLight(const SLight& light);

	//! returns the maximal amount of dynamic lights the device can handle
	virtual u32 getMaximalDynamicLightAmount() const;

	//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
	//! driver, it would return "Direct3D8.1".
	virtual const wchar_t* getName() const;

	//! Sets the dynamic ambient light color. The default color is
	//! (0,0,0,0) which means it is dark.
	//! \param color: New color of the ambient light.
	virtual void setAmbientLight(const SColorf& color);

	//! Adds an external image loader to the engine.
	virtual void addExternalImageLoader(IImageLoader* loader);

	//! Adds an external image writer to the engine.
	virtual void addExternalImageWriter(IImageWriter* writer);

	//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
	//! this: Frist, draw all geometry. Then use this method, to draw the shadow
	//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
	virtual void drawStencilShadowVolume(const core::vector3df* triangles,
										 s32 count,
										 bool zfail=true);

	//! Fills the stencil shadow with color. After the shadow volume has been drawn
	//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
	//! to draw the color of the shadow.
	virtual void drawStencilShadow(bool clearStencilBuffer=false,
								   video::SColor leftUpEdge = video::SColor(0,0,0,0),
								   video::SColor rightUpEdge = video::SColor(0,0,0,0),
								   video::SColor leftDownEdge = video::SColor(0,0,0,0),
								   video::SColor rightDownEdge = video::SColor(0,0,0,0));

	//! Returns current amount of dynamic lights set
	//! \return Current amount of dynamic lights set
	virtual u32 getDynamicLightCount() const;

	//! Returns light data which was previously set with IVideDriver::addDynamicLight().
	//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
	//! than IVideoDriver()::getDynamicLightCount.
	//! \return Light data.
	virtual const SLight& getDynamicLight(u32 idx) const;

	//! Removes a texture from the texture cache and deletes it, freeing lot of
	//! memory.
	virtual void removeTexture(ITexture* texture);

	//! Removes all texture from the texture cache and deletes them, freeing lot of
	//! memory.
	virtual void removeAllTextures();

	//! Creates a render target texture.
	virtual ITexture* addRenderTargetTexture(const core::dimension2d<s32>& size,
											 const c8* name,
											 ECOLOR_FORMAT format = ECF_A8R8G8B8);

	//! Creates an 1bit alpha channel of the texture based of an color key.
	virtual void makeColorKeyTexture(video::ITexture* texture, video::SColor color) const;

	//! Creates an 1bit alpha channel of the texture based of an color key position.
	virtual void makeColorKeyTexture(video::ITexture* texture, core::position2d<s32> colorKeyPixelPos) const;

	//! Creates a normal map from a height map texture.
	//! \param amplitude: Constant value by which the height information is multiplied.
	virtual void makeNormalMapTexture(video::ITexture* texture, f32 amplitude=1.0f) const;

	//! Returns the maximum amount of primitives (mostly vertices) which
	//! the device is able to render with one drawIndexedTriangleList
	//! call.
	virtual u32 getMaximalPrimitiveCount() const;

	//! Creates a software image from a file.
	virtual IImage* createImageFromFile(const char* filename);

	//! Creates a software image from a file.
	virtual IImage* createImageFromFile(io::IReadFile* file);

	//! Creates a software image from a byte array.
	/** \param useForeignMemory: If true, the image will use the data pointer
		directly and own it from now on, which means it will also try to delete [] the
		data when the image will be destructed. If false, the memory will by copied. */
	virtual IImage* createImageFromData(ECOLOR_FORMAT format,
										const core::dimension2d<s32>& size,
										void *data,
										bool ownForeignMemory = true,
										bool deleteForeignMemory = true);

	//! Creates an empty software image.
	virtual IImage* createImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size);

	//! Creates a software image from another image.
	virtual IImage* createImage(ECOLOR_FORMAT format, IImage *imageToCopy);

	//! Creates a software image from part of another image.
	virtual IImage* createImage(IImage* imageToCopy,
								const core::position2d<s32>& pos,
								const core::dimension2d<s32>& size);

	//! Draws a mesh buffer
	virtual void drawMeshBuffer(const scene::IMeshBuffer* mb);
	
	//! Sets max texture size for debugging purpose only
	virtual void setMaxTextureSize(const core::dimension2d<s32>& maxTextureSize) { MaxTextureSize = maxTextureSize; }

	//! Gets max texture size for debugging purpose only
	virtual const core::dimension2d<s32>& getMaxTextureSize() { return MaxTextureSize; }
		
	virtual void setOrthoOrientation(E_ORTHO_ORIENTATION newOri);
	virtual void Orientation3DProjectionTransform(irr::f32 *matrix,
												  E_ORIENTATION orientation);
	virtual void Orientation3DViewTransform(irr::f32 *matrix,
											E_ORIENTATION orientation);

	virtual void Orientation3D_ScreenPos2Internal(irr::s32 &x, irr::s32 &y);
	virtual void Orientation3D_Internal2ScreenPos(irr::s32 &x, irr::s32 &y);

	virtual E_ORTHO_ORIENTATION getOrthoOrientation() { return EOO_0; }
	virtual void setOrientation3D(E_ORIENTATION newOri) {}
	virtual E_ORIENTATION getOrientation3D() { return EOO_0; }

protected:

	struct SHWBufferLink
	{
		SHWBufferLink(const scene::IMeshBuffer *_MeshBuffer)
			: MeshBuffer(_MeshBuffer)
			, ChangedID_Vertex(0)
			, ChangedID_Index(0)
			, LastUsed(0)
			, Mapped_Vertex(scene::EHM_NEVER)
			, Mapped_Index(scene::EHM_NEVER)
		{
			if (MeshBuffer)
				MeshBuffer->grab();
		}

		virtual ~SHWBufferLink()
		{
			if (MeshBuffer)
				MeshBuffer->drop();
		}

		scene::E_BUFFER_TYPE Contains;
		const scene::IMeshBuffer *MeshBuffer;
		u32 ChangedID_Vertex;
		u32 ChangedID_Index;
		u32 LastUsed;
		scene::E_HARDWARE_MAPPING Mapped_Vertex;
		scene::E_HARDWARE_MAPPING Mapped_Index;
	};

	//! Gets hardware buffer link from a meshbuffer (may create or update buffer)
	virtual SHWBufferLink *getBufferLink(const scene::IMeshBuffer* mb);

	//! updates hardware buffer if needed  (only some drivers can)
	virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer) {return false;}

	//! Create hardware buffer from mesh (only some drivers can)
	virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb) {return 0;}

	//! Draw hardware buffer (only some drivers can)
	virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer) {}

	//! Update all hardware buffers, remove unused ones
	virtual void updateAllHardwareBuffers();

	//! Delete hardware buffer
	virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer);

	//! Remove hardware buffer
	virtual void removeHardwareBuffer(const scene::IMeshBuffer* mb);

	//! Remove all hardware buffers
	virtual void removeAllHardwareBuffers();

	//! is vbo recommended on this mesh?
	virtual bool isHardwareBufferRecommend(const scene::IMeshBuffer* mb);

public:

	//! Only used by the engine internally.
	/** Used to notify the driver that the window was resized. */
	virtual void OnResize(const core::dimension2d<s32>& size);

	//! Adds a new material renderer to the video device.
	virtual s32 addMaterialRenderer(IMaterialRenderer* renderer,
									const char* name = 0);

	//! Returns driver and operating system specific data about the IVideoDriver.
	virtual const SExposedVideoData& getExposedVideoData();

	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	//! Returns the transformation set by setTransform
	virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;

	//! Returns pointer to the IGPUProgrammingServices interface.
	virtual IGPUProgrammingServices* getGPUProgrammingServices();

	//! Adds a new material renderer to the VideoDriver, using pixel and/or
	//! vertex shaders to render geometry.
	virtual s32 addShaderMaterial(const c8* vertexShaderProgram = 0,
								  const c8* pixelShaderProgram = 0,
								  IShaderConstantSetCallBack* callback = 0,
								  E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
								  s32 userData=0);

	//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
	//! programs from files.
	virtual s32 addShaderMaterialFromFiles(io::IReadFile* vertexShaderProgram = 0,
										   io::IReadFile* pixelShaderProgram = 0,
										   IShaderConstantSetCallBack* callback = 0,
										   E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
										   s32 userData=0);

	//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
	//! programs from files.
	virtual s32 addShaderMaterialFromFiles(const c8* vertexShaderProgramFileName = 0,
										   const c8* pixelShaderProgramFileName = 0,
										   IShaderConstantSetCallBack* callback = 0,
										   E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
										   s32 userData=0);

	//! Returns pointer to material renderer or null
	virtual IMaterialRenderer* getMaterialRenderer(u32 idx) const;

	//! Returns amount of currently available material renderers.
	virtual u32 getMaterialRendererCount() const;

	//! Returns name of the material renderer
	virtual const char* getMaterialRendererName(u32 idx) const;

	//! Adds a new material renderer to the VideoDriver, based on a high level shading
	//! language. Currently only HLSL in D3D9 is supported.
	virtual s32 addHighLevelShaderMaterial(const c8* vertexShaderProgram,
										   const c8* vertexShaderEntryPointName = 0,
										   E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
										   const c8* pixelShaderProgram = 0,
										   const c8* pixelShaderEntryPointName = 0,
										   E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
										   IShaderConstantSetCallBack* callback = 0,
										   E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
										   u32 vertexAttributeMask = 0,
										   s32 userData=0);

	//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
	//! but tries to load the programs from files.
	virtual s32 addHighLevelShaderMaterialFromFiles(const c8* vertexShaderProgram,
													const c8* vertexShaderEntryPointName = "main",
													E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
													const c8* pixelShaderProgram = 0,
													const c8* pixelShaderEntryPointName = "main",
													E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
													IShaderConstantSetCallBack* callback = 0,
													E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
													u32 vertexAttributeMask = 0,
													s32 userData=0);

	//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
	//! but tries to load the programs from files.
	virtual s32 addHighLevelShaderMaterialFromFiles(io::IReadFile* vertexShaderProgram,
													const c8* vertexShaderEntryPointName = "main",
													E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
													io::IReadFile* pixelShaderProgram = 0,
													const c8* pixelShaderEntryPointName = "main",
													E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
													IShaderConstantSetCallBack* callback = 0,
													E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
													u32 vertexAttributeMask = 0,
													s32 userData=0);

	//! Returns a pointer to the mesh manipulator.
	virtual scene::IMeshManipulator* getMeshManipulator();

	//! Returns an image created from the last rendered frame.
	virtual IImage* createScreenShot();

	//! Writes the provided image to disk file
	virtual bool writeImageToFile(IImage* image, const char* filename, u32 param = 0);

	//! Sets the name of a material renderer.
	virtual void setMaterialRendererName(s32 idx, const char* name);

	//! Creates material attributes list from a material, usable for serialization and more.
	virtual io::IAttributes* createAttributesFromMaterial(const video::SMaterial& material);

	//! Fills an SMaterial structure from attributes.
	virtual void fillMaterialStructureFromAttributes(video::SMaterial& outMaterial, io::IAttributes* attributes);

	//! looks if the image is already loaded
	virtual video::ITexture* findTexture(const c8* filename);

	//! Set/unset a clipping plane.
	//! There are at least 6 clipping planes available for the user to set at will.
	//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
	//! \param plane: The plane itself.
	//! \param enable: If true, enable the clipping plane else disable it.
	virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable=false);

	//! Enable/disable a clipping plane.
	//! There are at least 6 clipping planes available for the user to set at will.
	//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
	//! \param enable: If true, enable the clipping plane else disable it.
	virtual void enableClipPlane(u32 index, bool enable);

	//! Returns the graphics card vendor name.
	virtual core::stringc getVendorInfo() {return "Not available on this driver.";}

	//! deprecated method
	virtual ITexture* createRenderTargetTexture(const core::dimension2d<s32>& size,
												const c8* name=0);

	virtual bool checkDriverReset() {return false;}

	virtual void setScissor(const core::rect<s32>& scissorRect);
	virtual const SColorf& getAmbientLight() const { return AmbientLight; }

	virtual void resetScissor();

	virtual void setColorMask(bool red, bool green, bool blue, bool alpha);

	virtual void setDepthRange(float near, float far);

	virtual video::ITexture* createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data);

	//! Checks is a driver option is enabled (see E_VIDEO_DRIVER_OPTION).
	virtual bool getOption(u32 option) const;

	//! Sets a driver option (see E_VIDEO_DRIVER_OPTION).
	virtual void setOption(u32 option, bool value);
		//! Provides access to the virtual file system.
	/** \return Pointer to the file system. */
	virtual io::IFileSystem* getFileSystem();

	virtual IDriverBinding* createBinding();

protected:

	//! deletes all textures
	void deleteAllTextures();

	//! opens the file and loads it into the surface
	video::ITexture* loadTextureFromFile(io::IReadFile* file, const c8* hashName = 0, bool refData = false);

	//! adds a surface, not loaded or created by the Irrlicht Engine
	void addTexture(video::ITexture* surface);

	//! Creates a texture from a loaded IImage.
	virtual ITexture* addTexture(const c8* name, IImage* image);

	//! returns a device dependent texture from a software surface (IImage)
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! returns a device dependant texture from a "native" format in a file
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
																		const c8* hashName = 0, bool refData = false);
#endif

	//! checks triangle count and print warning if wrong
	bool checkPrimitiveCount(u32 prmcnt) const;

	// adds a material renderer and drops it afterwards. To be used for internal creation
	s32 addAndDropMaterialRenderer(IMaterialRenderer* m);

	//! deletes all material renderers
	void deleteMaterialRenders();

	// prints renderer version
	void printVersion();

	//!
	virtual void beginCompile(ICompileData*);

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
	virtual void releaseProcessBuffer(E_PROCESS_BUFFER_TYPE type,
									  void* bindingOrDynamicBuffer,
									  u32 vertexStart = 0,
									  u32 stride = 0);

	//! Helper draw2DImage clipping
	static bool clip(core::rect<s32>& rect,
					 core::rect<f32>& sourceRect,
					 const core::rect<s32>& clipRect,
					 SColor* colors = NULL);

	//! normal map lookup 32 bit version
	inline f32 nml32(int x, int y, int pitch, int height, s32 *p) const
	{
		if (x < 0) x = pitch-1; if (x >= pitch) x = 0;
		if (y < 0) y = height-1; if (y >= height) y = 0;
		return (f32)(((p[(y * pitch) + x])>>16) & 0xff);
	}

	//! normal map lookup 16 bit version
	inline f32 nml16(int x, int y, int pitch, int height, s16 *p) const
	{
		if (x < 0) x = pitch-1; if (x >= pitch) x = 0;
		if (y < 0) y = height-1; if (y >= height) y = 0;

		return (f32) getA1R5G5BAverage ( p[(y * pitch) + x] );
	}

	

	struct SMaterialRenderer
	{
		core::stringc Name;
		IMaterialRenderer* Renderer;
	};

	

	CTextureManager* TextureManager;
	core::array<video::IImageLoader*> SurfaceLoader;
	core::array<video::IImageWriter*> SurfaceWriter;
	core::array<SLight> Lights;
	core::array<SMaterialRenderer> MaterialRenderers;

	//core::array<SHWBufferLink*> HWBufferLinks;
	core::map< const scene::IMeshBuffer* , SHWBufferLink* > HWBufferMap;

	io::IFileSystem* FileSystem;

	//! mesh manipulator
	scene::IMeshManipulator* MeshManipulator;

	core::rect<s32> ViewPort;
	core::dimension2d<s32> ScreenSize;
	core::matrix4 TransformationMatrix;

#ifdef _IRR_WITH_FRAME_STATISTICS_
	CFPSCounter FPSCounter;

	u32 PrimitivesDrawn;
	//! Non-2D draw calls
	u32 DrawCalls;
	u32 DrawCalls2D;
	u32 TextureBindings;
#endif

	u32 OptionFlags;

	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
	SColor FogColor;
	bool LinearFog;
	bool PixelFog;
	bool RangeFog;

	SExposedVideoData ExposedData;

	bool FeatureEnabled[video::EVDF_COUNT];

	core::dimension2d<s32> MaxTextureSize;

	SColorf AmbientLight;
};

} // end namespace video
} // end namespace irr

#endif

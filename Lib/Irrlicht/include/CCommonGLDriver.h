//-*-c++-*-
#ifndef __C_VIDEO_COMMON_GL_H_INCLUDED__
#define __C_VIDEO_COMMON_GL_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
#include "CCommonGLHeaders.h"

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "CCommonGLExtensionHandler.h"
#include "SIrrCreationParameters.h"
#include "irrTexGen.h"
#include "CBatchBuffer.h"
#include "IBatchBinding.h"
#include "SProcessBufferBindingBase.h"

#ifdef _DEBUG
	#define IRR_ENABLE_TEST_GLERROR_PARANOID		0	// Make an incredible amount of testGLError
	#define IRR_ENABLE_TEST_GLERROR_DEBUG			1	// Make the minimal testGLError
	#define IRR_ENABLE_TEST_GLERROR_BREAK_ON_ERROR	0	// Break on error instead of logging
#else // in Release turn off all these checks
	#define IRR_ENABLE_TEST_GLERROR_PARANOID		0
	#define IRR_ENABLE_TEST_GLERROR_DEBUG			0
	#define IRR_ENABLE_TEST_GLERROR_BREAK_ON_ERROR	0
#endif

namespace irr
{

namespace scene
{

class IBatchList;

}

namespace video
{

class CBatchListCompileData;
class CCommonGLTexture;

class CCommonGLDriver : public CNullDriver, public IMaterialRendererServices, public CCommonGLExtensionHandler
{
public:

#ifdef _IRR_WINDOWS_API_
	//! win32 constructor
	CCommonGLDriver(const core::dimension2d<s32>& screenSize, 
					HWND window,
					bool stencilBuffer, 
					io::IFileSystem* io, 
					bool antiAlias);
	
	virtual bool initWindowsDriver(const core::dimension2d<s32>& screenSize, 
								   HWND window, u32 bits, bool vsync, 
								   bool stencilBuffer) = 0;
#endif

#if defined(_IRR_USE_LINUX_DEVICE_) || defined(_IRR_USE_SDL_DEVICE_)
	CCommonGLDriver(const SIrrlichtCreationParameters& params, 
		            io::IFileSystem* io);
#endif
	
#ifdef _IRR_USE_IPHONEOS_DEVICE_
	CCommonGLDriver(const SIrrlichtCreationParameters& params,
				    io::IFileSystem* io, 
					CIrrDeviceIPhoneOS *device);
#endif

#ifdef _IRR_USE_OSX_DEVICE_
	CCommonGLDriver(const SIrrlichtCreationParameters& params,
				    io::IFileSystem* io, 
				    CIrrDeviceMacOSX *device);
#endif
	
#ifdef _IRR_COMPILE_WITH_PS3_
	//! PS3 constructor and init code
	CCommonGLDriver(const SIrrlichtCreationParameters& params,
					io::IFileSystem* io, 
					CIrrDevicePS3 *device);
#endif

	//! destructor
	virtual ~CCommonGLDriver();

	//! inits the windows specific parts of the open gl driver
	bool initDriver(const core::dimension2d<s32>& screenSize, 
				#ifdef _IRR_WINDOWS_API_
	                HWND window, u32 bits, 
				#endif
					bool vsync, 
					bool stencilBuffer);

	//!
	struct SBinding : SProcessBufferBindingBase<SBinding, IBatchBinding>
	{
	protected:

		CCommonGLDriver* Driver;

	public:

		SBinding(CCommonGLDriver* driver)
			: Driver(driver)
		{
		}

		IVideoDriver* getDriver() const
		{
			return Driver;
		}

		E_DRIVER_TYPE getDriverType() const;
	};

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool swapBuffers(int clearMask = EFB_COLOR | EFB_DEPTH);

	virtual void clearBuffers(int clearMask);
	virtual void setClearColor(const SColor& color);
	virtual SColor getClearColor() const;
	virtual void setClearDepth(float depth);
	virtual float getClearDepth() const;
#ifdef _IRR_HAS_STENCIL_BUFFER_
	virtual void setClearStencil(int stencil);
	virtual int getClearStencil() const;
#endif

	virtual bool beginScene2D();
	virtual bool endScene2D();

	//! sets transformation
	virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);


	struct SHWBufferLink_opengl : public SHWBufferLink
	{
		SHWBufferLink_opengl(const scene::IMeshBuffer *_MeshBuffer): SHWBufferLink(_MeshBuffer), vbo_verticesID(0),vbo_indicesID(0){}

		GLuint vbo_verticesID; //tmp
		GLuint vbo_indicesID; //tmp

		GLuint vbo_verticesSize; //tmp
		GLuint vbo_indicesSize; //tmp

	};

	bool updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);
	bool updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);

	//! updates hardware buffer if needed
	virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer);

	//! Create hardware buffer from mesh
	virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb);

	//! Delete hardware buffer (only some drivers can)
	virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer);

	//! Draw hardware buffer
	virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer);

	//! Draw a mesh buffer
	virtual void drawMeshBuffer(const scene::IMeshBuffer* mb);

	//! draws a vertex primitive list
	virtual void drawVertexPrimitiveList(const void* vertices,
										 const void* indexList,
										 u32 startIndex,
										 u32 endIndex,
										 u32 primitiveCount,
										 E_VERTEX_TYPE vType,
										 scene::E_PRIMITIVE_TYPE pType,
										 E_INDEX_TYPE iType,
										 IDriverBinding** binding = NULL);

	//! queries the features of the driver, returns true if feature is available
	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
	{
		return CCommonGLExtensionHandler::queryFeature(feature);
	}

	//! Sets a material. All 3d drawing functions draw geometry now
	//! using this material.
	//! \param material: Material to be used from now on.
	virtual void setMaterial(const SMaterial& material);

	//! Set a 2D Material if it's different from the next material to be used
	void set2DMaterial(SMaterial& material);
	
	//! Set the texture to be used with 2D render
	void set2DTexture(const ITexture* tex, bool useTextureAlpha = false);
	void set2DUseVertexAlpha(bool value);
	
	//! Draw a 2D surface with the current 2D Material, this will replace all the draw2DImage and draw2DRectangle
	void draw2DRectangle(const core::rect<s32>& destRect,
					     const core::rect<s32>& sourceRect,
						 const SColor colors[4],
					     const core::rect<s32>* clipRect = 0);

	//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
	virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
							 const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
							 SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);

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
							 const core::rect<s32>* clipRect=0,
							 SColor color=SColor(255,255,255,255),
							 bool useAlphaChannelOfTexture=false);

	//! Draws a part of the texture into the rectangle.
	virtual void draw2DImage(const video::ITexture* texture, 
							 const core::rect<s32>& destRect,
							 const core::rect<s32>& sourceRect, 
							 const core::rect<s32>* clipRect = 0,
							 const video::SColor* const colors=0, 
							 bool useAlphaChannelOfTexture=false);

	//! draw an 2d rectangle
	virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
								 const core::rect<s32>* clip = 0);

	//!Draws an 2d rectangle with a gradient.
	virtual void draw2DRectangle(const core::rect<s32>& pos,
								 SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
								 const core::rect<s32>* clip = 0);

	//! Draws a 2d line.
	virtual void draw2DLine(const core::position2d<s32>& start,
							const core::position2d<s32>& end,
							SColor color=SColor(255,255,255,255));

	//! Draws a 3d line.
	virtual void draw3DLine(const core::vector3df& start,
							const core::vector3df& end,
							SColor color = SColor(255,255,255,255));

	//! \return Returns the name of the video driver. Example: In case of the Direct3D8
	//! driver, it would return "Direct3D8.1".
	virtual const wchar_t* getName() const;

	//! deletes all dynamic lights there are
	virtual void deleteAllDynamicLights();

	//! adds a dynamic light
	virtual void addDynamicLight(const SLight& light);

	//! returns the maximal amount of dynamic lights the device can handle
	virtual u32 getMaximalDynamicLightAmount() const;

	//! Sets the dynamic ambient light color. The default color is
	//! (0,0,0,0) which means it is dark.
	//! \param color: New color of the ambient light.
	virtual void setAmbientLight(const SColorf& color);

	//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
	//! this: First, draw all geometry. Then use this method, to draw the shadow
	//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
	virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail) = 0;

	//! Fills the stencil shadow with color. After the shadow volume has been drawn
	//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
	//! to draw the color of the shadow.
	virtual void drawStencilShadow(bool clearStencilBuffer=false,
								   video::SColor leftUpEdge = video::SColor(0,0,0,0),
								   video::SColor rightUpEdge = video::SColor(0,0,0,0),
								   video::SColor leftDownEdge = video::SColor(0,0,0,0),
								   video::SColor rightDownEdge = video::SColor(0,0,0,0)) = 0;

	//! Keep game coordinates independent of the physical window resolution.
	//! An empty size restores one logical unit per window pixel.
	void setLogicalScreenSize(const core::dimension2d<s32>& size);
	virtual const core::dimension2d<s32>& getScreenSize() const;
	const core::dimension2d<s32>& getPhysicalScreenSize() const { return ScreenSize; }

	//! sets a viewport
	virtual void setViewPort(const core::rect<s32>& area);

	//! Sets the fog mode.
	virtual void setFog(SColor color, bool linearFog, f32 start,
						f32 end, f32 density, bool pixelFog, bool rangeFog);

	//! Only used by the internal engine. Used to notify the driver that
	//! the window was resized.
	virtual void OnResize(const core::dimension2d<s32>& size);

	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	//! get color format of the current color buffer
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! Returns the transformation set by setTransform
	virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;

	//! Can be called by an IMaterialRenderer to make its work easier.
	virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial,
									  bool resetAllRenderstates);

	//! Sets a vertex shader constant.
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a pixel shader constant.
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount=1);

	//! Sets a constant for the vertex shader based on a name.
	virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);

	//! Sets a constant for the pixel shader based on a name.
	virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);

	//! Adds a new material renderer to the VideoDriver, using
	//! extGLGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status)
	//! pixel and/or vertex shaders to render geometry.
	virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
								  IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData);

	//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
	virtual s32 addHighLevelShaderMaterial(const c8* vertexShaderProgram, const c8* vertexShaderEntryPointName,
										   E_VERTEX_SHADER_TYPE vsCompileTarget, const c8* pixelShaderProgram, const c8* pixelShaderEntryPointName,
										   E_PIXEL_SHADER_TYPE psCompileTarget, IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial,
										   u32 vertexAttributeMask, s32 userData);

	//! Returns pointer to the IGPUProgrammingServices interface.
	virtual IGPUProgrammingServices* getGPUProgrammingServices();

	//! sets the current Texture
	//! Returns whether setting was a success or not.
	bool setTexture(u32 stage, const video::ITexture* texture);

	//! disables all textures beginning with the optional fromStage parameter. Otherwise all texture stages are disabled.
	//! Returns whether disabling was successful or not.
	bool disableTextures(u32 fromStage=0);

	//! Returns a pointer to the IVideoDriver interface. (Implementation for
	//! IMaterialRendererServices)
	virtual IVideoDriver* getVideoDriver();

	//! Returns the maximum amount of primitives (mostly vertices) which
	//! the device is able to render with one drawIndexedTriangleList
	//! call.
	virtual u32 getMaximalPrimitiveCount() const;

	virtual ITexture* addRenderTargetTexture(const core::dimension2d<s32>& size,
											 const c8* name,
											 ECOLOR_FORMAT format = ECF_A8R8G8B8);

	ITexture* addRenderTargetTexture(const core::dimension2d<s32>& size,
									 const c8* name, bool colorTexture=true, bool depthTexture=false);

	virtual bool setRenderTarget(video::ITexture* texture,
								 int clearMask = EFB_COLOR | EFB_DEPTH);

	//! Returns an image created from the last rendered frame.
	virtual IImage* createScreenShot();

	//! checks if an OpenGL error has happend and prints it
	//! for performance reasons only available in debug mode
	static bool testGLError();

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
	virtual core::stringc getVendorInfo() {return vendorName;};

	virtual void setOrthoOrientation(E_ORTHO_ORIENTATION newOri);
	virtual E_ORTHO_ORIENTATION getOrthoOrientation();
	virtual void setOrientation3D(E_ORIENTATION newOri);
	virtual E_ORIENTATION getOrientation3D();        
	virtual void setScissor(const core::rect<s32>& scissorRect);

	virtual void resetScissor();

	//! Set alpha/color mask to write in color buffer.
	virtual void setColorMask(bool red, bool green, bool blue, bool alpha);

	virtual void setDepthRange(float near, float far);

	//!
	virtual void flush();

	//!
	virtual void beginCompile(ICompileData*);

	//!
	virtual void endCompile();

	//!
	virtual void setOption(u32 flag, bool value);

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

	virtual IDriverBinding* createBinding();

	u32 getMaxBatchSegmentSize() const
	{
		return MaxDynamicBatchSegmentSize;
	}

	void setMaxBatchSegmentSize(u32 size);

	//!
	void setDynamicBatchCapatity(u32 vertexCapacity,
							     u32 indexCapacity);

	void setTexEnvMode(GLenum mode);
	void setCombineRGB(GLenum mode);
	void setCombineAlpha(GLenum mode);
	void setSourceRGB(int i, GLenum src);
	void setSourceAlpha(int i, GLenum src);
	void setOperandRGB(int i, GLenum oper);
	void setOperandAlpha(int i, GLenum oper);
	void setRGBScale(GLfloat scale);
	void setAlphaScale(GLfloat scale);

	void setActiveTexture(u32 i);
	void setClientActiveTexture(u32 i);

	void bindArrayBuffer(GLuint name);
	void bindElementArrayBuffer(GLuint name);

	void setColorMaterialEnable(GLboolean enable);

	void setTexGen(int texunit, E_TEX_GEN_TYPE texgen);

	//! gets the current Texture
	const video::ITexture* getCurrentTexture(u32 stage);

#if !defined(_IRR_COMPILE_WITH_PS3_)
	GLuint getTextureBinding() const;
#endif

	enum E_MATERIAL_COLOR_PARAM
	{
		EMCP_AMBIENT,
		EMCP_DIFFUSE,
		EMCP_SPECULAR,
		EMCP_EMISSION
	};

	void setMaterialColor(E_MATERIAL_COLOR_PARAM param, const SColor& color);
	void setMaterialShininess(GLfloat value);
#ifdef GL_EXT_separate_specular_color
	void setLightModelColorControl(GLenum ctrl);
#endif

#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) && defined(GL_EXT_texture_lod_bias)
	//! \internal
	void setTexEnvLodBias(GLfloat lodBias);
#endif

	void setDepthMask(bool value);

protected:

	//! Do the actual material change from the setMaterial and set2DMaterial
	void setCurrentMaterial(const SMaterial& material);

	const SMaterial& getCurrentMaterial() const
	{
		_IRR_DEBUG_BREAK_IF(DynamicBatch == 0);
		return DynamicBatch->getMaterial();
	}

	u32 getCurrentRequiredAttributes() const
	{
		_IRR_DEBUG_BREAK_IF(DynamicBatch == 0);
		return DynamicBatch->getVertexAttributeMask();
	}

	//! set opengl material colors
	//void setMaterialColor(const SMaterial& material);

	//! Apply the changes to the local matrices that have a dirty flag
	void applyMatricesChanges(bool resetAllRenderStates);


	//! draws a vertex primitive list
	IRR_DEBUG_VIRTUAL void drawVertexPrimitiveList(
		const void* vertices,
		const void* indexList,
		u32 startIndex,
		u32 endIndex,
		u32 primitiveCount,
		E_VERTEX_TYPE vType,
		scene::E_PRIMITIVE_TYPE pType,
		E_INDEX_TYPE iType,
		bool fVBOFlag
	);

	void uploadClipPlane(u32 index);

	//! inits the parts of the open gl driver used on all platforms
	bool genericDriverInit(const core::dimension2d<s32>& screenSize, bool stencilBuffer);

	//! returns a device dependent texture from a software surface (IImage)
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name) = 0;

	//! returns a device dependent RenderTarget texture from a software surface (IImage)
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentRTTexture(const core::dimension2d<s32>& size, const char* name, bool colorTexture, bool depthTexture) = 0;

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! returns a device dependant texture from a "native" format in a file
	//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
	virtual video::ITexture* createDeviceDependentNativeTextureFromFile(io::IReadFile* file,
																		const c8* hashName = 0,
																		bool refData = false) { return 0; } ;
#endif	

	//! creates a transposed matrix in supplied GLfloat array to pass to OpenGL
	inline void createGLMatrix(GLfloat gl_matrix[16], const core::matrix4& m);
	inline void createGLTextureMatrix(GLfloat gl_matrix[16], const core::matrix4& m);

	//! sets the needed renderstates
	void setRenderStates3DMode();
	
	//! sets the needed renderstates
	void setRenderStates2DMode();

	// returns the current size of the screen or rendertarget
	virtual const core::dimension2d<s32>& getCurrentRenderTargetSize() const;

	virtual void createMaterialRenderers() = 0;

	//!
	u32 setupArrays(u32 requiredAttribs,
					const S3DVertexComponentArrays* components,
					bool fVBOFlag = false,
					bool* quitInTextureMode = NULL);

	//!
	void unsetupArrays(u32 requiredAttribs,
					   const S3DVertexComponentArrays* components,
					   bool wasInTextureMatrixMode = false);

	//!
	void setupArrayEnables(u32 requiredAttribs);

	//!
	void softTexGen(u32 texGenEnable,
					SScopedProcessArray<core::vector2df> texGenBuffer[MATERIAL_MAX_TEXTURES],
					S3DVertexComponentArrays* components,
					u32 startIndex,
					u32 endIndex);

	//!
	void drawQuads(const core::rect<s32>& npos,
				   const core::rect<f32>& tcoords,
				   const SColor* colors);

	//!
	bool convertVertexType(void* vertices,
						   u32 startIndex,
						   u32 endIndex,
						   E_VERTEX_TYPE type,
						   SScopedProcessArray<SColor>& colorBuffer);

	//!
	static void oglSetMaterialColor(GLenum param, const SColor& color);

	//!
	void allocateDynamicBatch(u32 vertexCapacity, u32 indexCapacity
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
							  , bool useProcessBuffer = true
#endif
	);

	//!
	void releaseDynamicBatch(u32* oldVertexCapacity = NULL,
							 u32* oldIndexCapacity = NULL);

	//!
	SBinding* ensureBinding(IDriverBinding** binding);

	core::stringw Name;
	core::matrix4 Matrices[ETS_COUNT];
	u16 MatricesDirtyFlag;

	//! Backup of the matrices to keep a backup of the 3D state when switching to 2D
	core::matrix4 ProjectionMatrixBackup;
	core::matrix4 WorldMatrixBackup;
	core::matrix4 ViewMatrixBackup;

	//! enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
	enum E_RENDER_MODE
	{
		ERM_NONE = 0,	// no render state has been set yet.
		ERM_2D,		// 2d drawing rendermode
		ERM_3D		// 3d rendering mode
	};

	E_RENDER_MODE CurrentRenderMode;	

	E_ORTHO_ORIENTATION CurrentOrientation;
	E_ORTHO_ORIENTATION CurrentOrientation3D;

	SColor ClearColor;
	float ClearDepth;
#ifdef _IRR_HAS_STENCIL_BUFFER_
	int ClearStencil;
#endif

	//! bool to make all renderstates reset if set to true.
	bool ResetRenderStates;
	//bool Transformation3DChanged;
	bool ProjectionChanged;
	bool AntiAlias;
	//u32 IsTextureMatrixIdentity;
	bool WasDynamicBatchEnabledBefore2D;
	bool IsFlushingDynamicBatch;
	bool LastRenderWasDynamicBatch;

	scene::CBatchBuffer* DynamicBatch;
	u32 MaxDynamicBatchSegmentSize;
	u32 ActualMaxDynamicBatchSegmentSize;
	SMaterial LastMaterial;
	u32 LastRequiredVertexAttributes;

	CBatchListCompileData* CompileData;
	u32 CurrentBatchId;
	u32 CurrentSegmentId;
	u32 SavedMaxDynamicBatchSegmentSize;
	u32 SavedDynamicBatchVertexCapacity;
	u32 SavedDynamicBatchIndexCapacity;
	bool SavedBatchingOption;
#ifndef _IRR_ENABLE_PROCESS_BUFFER_HEAP_EXCESS_
	bool IsDynamicBatchUsingProcessBuffer;
#endif
	u32 CurrentDynamicBatchVertexCapacity;
	u32 CurrentDynamicBatchIndexCapacity;

	CCommonGLTexture* RenderTargetTexture;
	const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];

	s32 LastSetLight;

	core::array<core::plane3df> UserClipPlane;
	core::array<bool> UserClipPlaneEnabled;

	core::dimension2d<s32> CurrentRendertargetSize;
	core::dimension2d<s32> LogicalScreenSize;

	core::stringc vendorName;

	core::matrix4 TextureFlipMatrix;

	static const GLenum MaterialColorParamMap[];

#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	//! Implements a state shadow layer for caching some render states.
	struct SShadowState
	{
		// Add more as needed

		struct STexEnv
		{
			GLenum Mode;
			GLenum CombineRGB;
			GLenum CombineAlpha;

	#if !defined(_IRR_COMPILE_WITH_PS3_)
			struct SSource
			{
				GLenum RGB;
				GLenum Alpha;
				GLenum OperandRGB;
				GLenum OperandAlpha;

			} Source[3];
	#endif

			GLfloat RGBScale;
			GLfloat AlphaScale;
	#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) && defined(GL_EXT_texture_lod_bias)
			GLfloat LodBias;
	#endif

			STexEnv();

		} TexEnv[MATERIAL_MAX_TEXTURES];

		struct SMaterial
		{
			SColor Color[4];
			GLfloat Shininess;
#ifdef GL_EXT_separate_specular_color
			GLenum ColorControl;
#endif

			SMaterial();

		} Material;

		GLuint ArrayBuffer;
		GLuint ElementArrayBuffer;
		GLboolean ColorMaterialEnabled;

		bool DepthMask;

		SShadowState();

	} ShadowState;
#endif

	struct QuadVertex
	{
		// 3d instead of 2d for more code reuse; anyways, 2D is usually not that
		// heavy, and we leave xy.Z untouched (0, that is).
		core::vector3df Pos;

		core::vector2df TexCoord;
		SColor Color;

		QuadVertex()
		{
		}

		QuadVertex(s32 x, s32 y, f32 s, f32 t, const SColor& c)
			: Pos((f32)x, (f32)y, 0.0f)
			, TexCoord(s, t)
			, Color(c)
		{
		}

		void set(s32 x, s32 y, f32 s, f32 t, const SColor& c)
		{
			Pos.set((f32)x, (f32)y, 0.0f);
			TexCoord.set(s,t);
			Color = c;
		}
	};
	
	QuadVertex QuadBuffer[4];
	SS3DVertexComponentArrays QuadComponents;


	//! Bit mask of which tex units have soft tex gen.
	int TexGenEnabled;
	//! Soft tex gen type per tex unit.
	E_TEX_GEN_TYPE TexGenType[MATERIAL_MAX_TEXTURES];

	//! Color buffer format
	ECOLOR_FORMAT ColorFormat;

	//! Used for conversion of legacy vertex types and for "tempotary"
	//! components
	S3DVertexComponentArrays VertexComponents;

#ifdef _IRR_WINDOWS_API_
	HDC HDc; // Private GDI Device Context
	HWND Window;
	HGLRC HRc; // Permanent Rendering Context

#elif defined(_IRR_USE_LINUX_DEVICE_)
	GLXDrawable Drawable;
#elif defined(_IRR_USE_OSX_DEVICE_)
	CIrrDeviceMacOSX *MacDevice;
#elif defined(_IRR_USE_IPHONEOS_DEVICE_)
	CIrrDeviceIPhoneOS *IphoneDevice;
#endif

public:

	static bool testGlErrorParanoid()
	{
#if IRR_ENABLE_TEST_GLERROR_PARANOID
		bool result = testGLError();
	#if IRR_ENABLE_TEST_GLERROR_BREAK_ON_ERROR
		_IRR_DEBUG_BREAK_IF(result);
	#else
		if(result)
			os::Printer::log( "OpenGL Error Test failed ! (IRR_TEST_GL_ERROR_PARANOID;)", ELL_ERROR);
	#endif
		return result;
#else
		return false;
#endif
	}

	bool testGlErrorDebug()
	{
#if IRR_ENABLE_TEST_GLERROR_DEBUG || IRR_ENABLE_TEST_GLERROR_PARANOID
	bool result = testGLError();
	#if IRR_ENABLE_TEST_GLERROR_BREAK_ON_ERROR
		_IRR_DEBUG_BREAK_IF(result) 
	#else
		if(result)
			os::Printer::log( "OpenGL Error Test failed ! (IRR_TEST_GL_ERROR_DEBUG)", ELL_ERROR);
	#endif
	return result;
#else
		return false;
#endif
	}
};

//! Changes GL_TEXTURE_ENV_MODE's value, if different than before
inline void CCommonGLDriver::setTexEnvMode(GLenum mode)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (mode != texenv.Mode)
#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.Mode = mode;
#endif
	}
	testGlErrorParanoid();
}

//! Changes GL_COMBINE_RGB's value, if different than before
inline void CCommonGLDriver::setCombineRGB(GLenum mode)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)

	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (mode != texenv.CombineRGB)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, mode);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.CombineRGB = mode;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setCombineRGB), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_COMBINE_ALPHA's value, if different than before
inline void CCommonGLDriver::setCombineAlpha(GLenum mode)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (mode != texenv.CombineAlpha)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, mode);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.CombineAlpha = mode;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setCombineAlpha), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_SOURCEi_RGB's value, if different than before
inline void CCommonGLDriver::setSourceRGB(int i, GLenum src)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	_IRR_DEBUG_BREAK_IF(i < 0 || i > 2);
	SShadowState::STexEnv::SSource& source
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0].Source[i];
	if (src != source.RGB)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB + i, src);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		source.RGB = src;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setSourceRGB), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_SOURCEi_ALPHA's value, if different than before
inline void CCommonGLDriver::setSourceAlpha(int i, GLenum src)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	_IRR_DEBUG_BREAK_IF(i < 0 || i > 2);
	SShadowState::STexEnv::SSource& source
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0].Source[i];
	if (src != source.Alpha)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA + i, src);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		source.Alpha = src;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setSourceAlpha), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_OPERANDi_RGB's value, if different than before
inline void CCommonGLDriver::setOperandRGB(int i, GLenum oper)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	_IRR_DEBUG_BREAK_IF(i < 0 || i > 2);
	SShadowState::STexEnv::SSource& source
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0].Source[i];
	if (oper != source.OperandRGB)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB + i, oper);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		source.OperandRGB = oper;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setOperandRGB), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_OPERANDi_ALPHA's value, if different than before
inline void CCommonGLDriver::setOperandAlpha(int i, GLenum oper)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	_IRR_DEBUG_BREAK_IF(i < 0 || i > 2);
	SShadowState::STexEnv::SSource& source
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0].Source[i];
	if (oper != source.OperandAlpha)
	#endif
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA + i, oper);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		source.OperandAlpha = oper;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setOperandAlpha), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_RGB_SCALE's value, if different than before
inline void CCommonGLDriver::setRGBScale(GLfloat scale)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (scale != texenv.RGBScale)
	#endif
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, scale);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.RGBScale = scale;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setRGBScale), not defined in PSGL", ELL_ERROR);
#endif
}

//! Changes GL_ALPHA_SCALE's value, if different than before
inline void CCommonGLDriver::setAlphaScale(GLfloat scale)
{
#if !defined(_IRR_COMPILE_WITH_PS3_)
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (scale != texenv.AlphaScale)
	#endif
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, scale);
	#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.AlphaScale = scale;
	#endif
	}
	testGlErrorParanoid();
#elif defined(_DEBUG)
	os::Printer::log( "Error (setAlphaScale), not implemented in PSGL", ELL_ERROR);
#endif
}

inline void CCommonGLDriver::bindArrayBuffer(GLuint name)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (ShadowState.ArrayBuffer != name)
	{
#endif
		bindBuffer(GL_ARRAY_BUFFER, name);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.ArrayBuffer = name;
	}
#endif
	testGlErrorParanoid();
}

inline void CCommonGLDriver::bindElementArrayBuffer(GLuint name)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (ShadowState.ElementArrayBuffer != name)
	{
#endif
		bindBuffer(GL_ELEMENT_ARRAY_BUFFER, name);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.ElementArrayBuffer = name;
	}
#endif
	testGlErrorParanoid();
}

inline void CCommonGLDriver::setActiveTexture(u32 i)
{
	// if setActiveTexture as been called with a texture define use i directly, 
	// else, add GL_TEXTURE0 to get the right texture
	activeTexture((i >= GL_TEXTURE0 ? 0 : GL_TEXTURE0) + i);
	testGlErrorParanoid();
}

inline void CCommonGLDriver::setClientActiveTexture(u32 i)
{
	// if setClientActiveTexture as been called with a texture define use i directly, 
	// else, add GL_TEXTURE0 to get the right texture
#ifdef _DEBUG
	if((i >= GL_TEXTURE0 && i<GL_TEXTURE0+MaxTextureUnits) || (i>=0 && i<MaxTextureUnits))
#endif
		clientActiveTexture( (i >= GL_TEXTURE0 ? 0 : GL_TEXTURE0) + i);
#ifdef _DEBUG
	else
	{
		os::Printer::log( "Error (setClientActiveTexture), trying to activate a texture over the driver's limit", ELL_ERROR);
	}
#endif
	testGlErrorParanoid();
}

inline void CCommonGLDriver::oglSetMaterialColor(GLenum param, const SColor& color)
{
	const static GLfloat f = 1.0f / 255.0f;
	GLfloat fcolor[] = { color.R * f,
						 color.G * f,
						 color.B * f,
						 color.A * f };
	glMaterialfv(GL_FRONT_AND_BACK, param, fcolor);
	testGlErrorParanoid();
}

inline void CCommonGLDriver::setColorMaterialEnable(GLboolean enable)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (enable != ShadowState.ColorMaterialEnabled)
	{
#endif
		if (enable)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
			oglSetMaterialColor(GL_AMBIENT,
								ShadowState.Material.Color[EMCP_AMBIENT]);
			oglSetMaterialColor(GL_DIFFUSE,
								ShadowState.Material.Color[EMCP_DIFFUSE]);
#endif
		}
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.ColorMaterialEnabled = enable;
	}
#endif
	testGlErrorParanoid();
}

//! Tells the driver to regenerate texture coordinates of given type.
inline void CCommonGLDriver::setTexGen(int texunit, E_TEX_GEN_TYPE texgen)
{
	if (texunit < 1 || MultiTextureExtension)
	{
		TexGenType[texunit] = texgen;
		if (texgen)
		{
			TexGenEnabled |= (1 << texunit);
		}
		else
		{
			TexGenEnabled &= ~(1 << texunit);
		}
	}
}

//! Gets the current Texture
inline const video::ITexture* CCommonGLDriver::getCurrentTexture(u32 stage)
{
	if (stage >= MaxTextureUnits)
		return NULL;

	return CurrentTexture[stage];
}

#if !defined(_IRR_COMPILE_WITH_PS3_)
inline GLuint CCommonGLDriver::getTextureBinding() const
{
	GLint tex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex);
	return tex;
}
#endif

inline void CCommonGLDriver::setMaterialColor(E_MATERIAL_COLOR_PARAM param,
											const SColor& color)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (ShadowState.Material.Color[param] != color)
#endif
	{
		oglSetMaterialColor(MaterialColorParamMap[param], color);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.Material.Color[param] = color;
#endif
	}
	testGlErrorParanoid();
}

inline void CCommonGLDriver::setMaterialShininess(GLfloat value)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (ShadowState.Material.Shininess != value)
#endif
	{
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, value);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.Material.Shininess = value;
#endif
	}
	testGlErrorParanoid();
}

#ifdef GL_EXT_separate_specular_color
inline void CCommonGLDriver::setLightModelColorControl(GLenum ctrl)
{
	if (FeatureAvailable[IRR_EXT_separate_specular_color]
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		&& ShadowState.Material.ColorControl != ctrl
#endif
		)
	{	
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, ctrl);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.Material.ColorControl = ctrl;
#endif
	}	
	testGlErrorParanoid();
}
#endif

#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) && defined(GL_EXT_texture_lod_bias)
//! Changes TEXTURE_LOD_BIAS_EXT's value, if different than before
inline void CCommonGLDriver::setTexEnvLodBias(GLfloat lodBias)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	SShadowState::STexEnv& texenv
		= ShadowState.TexEnv[ExtensionShadowState.ActiveTexture - GL_TEXTURE0];
	if (lodBias != texenv.LodBias)
#endif
	{
		glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, lodBias);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		texenv.LodBias = lodBias;
#endif
	}
	testGlErrorParanoid();
}
#endif

inline void CCommonGLDriver::setDepthMask(bool value)
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	if (ShadowState.DepthMask != value)
#endif
	{
		glDepthMask(value);
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ShadowState.DepthMask = value;
#endif
	}
}

namespace{

inline GLint getOGLInteger(GLenum what)
{
	GLint val;
	glGetIntegerv(what, &val);
	return val;
}

#if !defined(_IRR_COMPILE_WITH_PS3_)
inline GLvoid* getOGLPointer(GLenum what)
{
	GLvoid* ptr;
	glGetPointerv(what, &ptr);
	return ptr;
}
#endif

} // end namespace anonymous
} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
#endif // __C_VIDEO_COMMON_GL_H_INCLUDED__

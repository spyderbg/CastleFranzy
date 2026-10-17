//-*-c++-*-
#ifndef __CCOLLADAINSTANCEDSCENENODE_H__
#define __CCOLLADAINSTANCEDSCENENODE_H__

#include "IVideoDriver.h"
#include "CSceneManager.h"
#include "ISceneNode.h"

namespace irr
{
namespace collada
{

class CInstancingProxy;

class CInstancedSceneNode 
	: public scene::ISceneNode
{
public:

	CInstancedSceneNode(CInstancingProxy* proxy);
	virtual ~CInstancedSceneNode();

	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual void OnAnimate(u32 timeMs);
	virtual void OnRegisterSceneNode();

	virtual void render(void* renderData = 0);
	
	//! Returns type of the scene node
	virtual scene::ESCENE_NODE_TYPE getType() const;

private:

	CInstancingProxy* InstanceProxy;
};

//------------------------------------------------------------------------------

class CInstancingProxy 
	: public scene::CSceneManager
{
public:

	friend class CVideoDriverProxy;

public:

	CInstancingProxy(video::IVideoDriver* driver, scene::ISceneNode* instancedNode);
	//CInstancingProxy(scene::CSceneManager* manager);
	virtual ~CInstancingProxy();

	void setCurrentTransformation(core::matrix4& trans)
	{
		CurInstAbsoluteTransformation = trans;
	}

	void setCameraEye(core::vector3df eye)
	{
		CamWorldPos = eye;
	}

	bool isRegistered() const
	{
		return !SolidNodeList.empty() || !TransparentNodeList.empty(); //|| !SkyBoxList.empty() || !ShadowNodeList.empty();
	}

	virtual void OnAnimate(u32 timeMs);
	virtual void OnRegisterSceneNode();

	int getSolidNodeCount() const
	{
		return SolidNodeList.size();
	}

	int getTransparentNodeCount() const
	{
		return TransparentNodeList.size();
	}

	void increaseSolidRenderCount()
	{
		++SolidListRenderCount;
	}
	void increaseTransparentRenderCount() 
	{
		++TransparentListRenderCount;
	}

	//NOTE-FH: Could add the render of the other lists if necessary
	void renderSolidList();
	void renderTransparentList();

	//! ISceneManager methods
	virtual video::IVideoDriver* getVideoDriver();

	//! Returns ambient color of the scene
	virtual const video::SColorf& getAmbientLight() const;

	//! Sets ambient color of the scene
	virtual void setAmbientLight(const video::SColorf &ambientColor);

	virtual u32	registerNodeForRendering(scene::ISceneNode* node,
										 const video::SMaterial* material = 0,
										 void* renderData = 0,
										 scene::E_SCENE_NODE_RENDER_PASS pass = scene::ESNRP_AUTOMATIC);

private:

	class CVideoDriverProxy : public video::IVideoDriver
	{
	private:

		CInstancingProxy* Proxy;

	public:

		CVideoDriverProxy(CInstancingProxy* proxy)
			: Proxy(proxy)
		{
		}
		
		//! IVideoDriver methods
		virtual void					setTransform(video::E_TRANSFORMATION_STATE state, const core::matrix4& mat);
		virtual const core::matrix4&	getTransform(video::E_TRANSFORMATION_STATE state) const;

		virtual video::ITexture* createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data);

		//! All currently unused IVideoDriver inherited abstract methods
		//***********************************************************
		virtual const wchar_t* getName() const;
		virtual bool beginScene();
		virtual bool endScene();
		virtual bool swapBuffers(int);
		virtual void clearBuffers(int);
		virtual void setClearColor(const video::SColor& color);
		virtual video::SColor getClearColor() const;
		virtual void setClearDepth(float);
		virtual float getClearDepth() const;
		virtual void setClearStencil(int);
		virtual int getClearStencil() const;
		virtual bool beginScene2D();
		virtual bool endScene2D();
		virtual bool queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const;
		virtual void disableFeature(video::E_VIDEO_DRIVER_FEATURE feature, bool flag=true);
		virtual bool checkDriverReset();
		virtual void setMaterial(const video::SMaterial& material);
		virtual void set2DMaterial(video::SMaterial& material);
		virtual void set2DTexture(const video::ITexture* tex, bool useTextureAlpha = false);
		virtual void set2DUseVertexAlpha(bool value);
		virtual video::ITexture* getTexture(const c8* filename);
		virtual video::ITexture* getTexture(const core::stringc& filename);
		virtual video::ITexture* getTexture(io::IReadFile* file, bool refData = false);
		virtual video::ITexture* getTextureByIndex(u32 index);
		virtual u32 getTextureCount() const;
		virtual void renameTexture(video::ITexture* texture, const c8* newName);
		virtual video::ITexture* addTexture(const core::dimension2d<s32>& size,
											const c8* name, video::ECOLOR_FORMAT format = video::ECF_A8R8G8B8);
		virtual video::ITexture* addTexture(const c8* name, video::IImage* image);
		virtual video::ITexture* addRenderTargetTexture(const core::dimension2d<s32>& size,
														const c8* name = 0,
														video::ECOLOR_FORMAT format = video::ECF_A8R8G8B8);
		virtual video::ITexture* createRenderTargetTexture(const core::dimension2d<s32>& size,
														   const c8* name=0);
		virtual void removeTexture(video::ITexture* texture);
		virtual void removeAllTextures();
		virtual void removeHardwareBuffer(const scene::IMeshBuffer* mb);
		virtual void removeAllHardwareBuffers();
		virtual void makeColorKeyTexture(video::ITexture* texture, video::SColor color) const;
		virtual void makeColorKeyTexture(video::ITexture* texture,
										 core::position2d<s32> colorKeyPixelPos) const;
		virtual void makeNormalMapTexture(video::ITexture* texture, f32 amplitude=1.0f) const;
		virtual bool setRenderTarget(video::ITexture* texture, int);
		virtual void setViewPort(const core::rect<s32>& area);
		virtual const core::rect<s32>& getViewPort() const;
		virtual void drawVertexPrimitiveList(const void* vertices,
											 const void* indexList,
											 u32 startIndex,
											 u32 minIndex,
											 u32 primCount,
											 video::E_VERTEX_TYPE vType,
											 scene::E_PRIMITIVE_TYPE pType,
											 video::E_INDEX_TYPE iType,
											 video::IDriverBinding** binding = NULL);
		virtual void draw3DLine(const core::vector3df& start,
								const core::vector3df& end, video::SColor color = video::SColor(255,255,255,255));
		virtual void draw3DTriangle(const core::triangle3df& triangle,
									video::SColor color = video::SColor(255,255,255,255));
		virtual void draw3DBox(const core::aabbox3d<f32>& box,
							   video::SColor color = video::SColor(255,255,255,255));
		virtual void draw2DImage(const video::ITexture* texture,
								 const core::position2d<s32>& destPos);
		virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
								 const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
								 video::SColor color=video::SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);
		virtual void draw2DImage(const video::ITexture* texture,
								 const core::position2d<s32>& pos,
								 const core::array<core::rect<s32> >& sourceRects,
								 const core::array<s32>& indices,
								 s32 kerningWidth=0,
								 const core::rect<s32>* clipRect=0,
								 video::SColor color=video::SColor(255,255,255,255),
								 bool useAlphaChannelOfTexture=false);
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
								 const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
								 const video::SColor * const colors=0, bool useAlphaChannelOfTexture=false);
		virtual void draw2DRectangle(video::SColor color, const core::rect<s32>& pos,
									 const core::rect<s32>* clip = 0);
		virtual void draw2DRectangle(const core::rect<s32>& pos,
									 video::SColor colorLeftUp, video::SColor colorRightUp,
									 video::SColor colorLeftDown, video::SColor colorRightDown,
									 const core::rect<s32>* clip = 0);
		virtual void draw2DLine(const core::position2d<s32>& start,
								const core::position2d<s32>& end,
								video::SColor color=video::SColor(255,255,255,255));
		virtual void draw2DPolygon(core::position2d<s32> center,
								   f32 radius,
								   video::SColor color=video::SColor(100,255,255,255),
								   s32 vertexCount=10);
		virtual void draw2DRectangle(const core::rect<s32>& destRect,
								     const core::rect<s32>& sourceRect,
									 const video::SColor colors[4],
								     const core::rect<s32>* clipRect = 0);
		virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail=true);
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
									   video::SColor leftUpEdge = video::SColor(255,0,0,0),
									   video::SColor rightUpEdge = video::SColor(255,0,0,0),
									   video::SColor leftDownEdge = video::SColor(255,0,0,0),
									   video::SColor rightDownEdge = video::SColor(255,0,0,0));
		virtual void drawMeshBuffer(const scene::IMeshBuffer* mb);
		virtual void setFog(video::SColor color=video::SColor(0,255,255,255),
							bool linearFog=true, f32 start=50.0f, f32 end=100.0f,
							f32 density=0.01f,
							bool pixelFog=false, bool rangeFog=false);
		virtual video::ECOLOR_FORMAT getColorFormat() const;
		virtual const core::dimension2d<s32>& getScreenSize() const;
		virtual const core::dimension2d<s32>& getCurrentRenderTargetSize() const;
		virtual s32 getFPS() const;
		virtual u32 getPrimitiveCountDrawn( u32 mode = 0 ) const;
		virtual u32 getDrawCallCount() const;
		virtual u32 getDrawCall2DCount() const;
		virtual u32 getTextureBindingCount() const;
		virtual void deleteAllDynamicLights();
		virtual void addDynamicLight(const video::SLight& light);
		virtual u32 getMaximalDynamicLightAmount() const;
		virtual u32 getDynamicLightCount() const;
		virtual const video::SLight& getDynamicLight(u32 idx) const;
		virtual void addExternalImageLoader(video::IImageLoader* loader);
		virtual void addExternalImageWriter(video::IImageWriter* writer);
		virtual u32 getMaximalPrimitiveCount() const;
		virtual video::IImage* createImageFromFile(const c8* filename);
		virtual video::IImage* createImageFromFile(io::IReadFile* file);
		virtual bool writeImageToFile(video::IImage* image, const c8* filename, u32 param = 0);
		virtual video::IImage* createImageFromData(video::ECOLOR_FORMAT format,
												   const core::dimension2d<s32>& size, void *data,
												   bool ownForeignMemory=false,
												   bool deleteMemory = true);
		virtual video::IImage* createImage(video::ECOLOR_FORMAT format, const core::dimension2d<s32>& size);
		virtual video::IImage* createImage(video::ECOLOR_FORMAT format, video::IImage *imageToCopy);
		virtual video::IImage* createImage(video::IImage* imageToCopy,
										   const core::position2d<s32>& pos, const core::dimension2d<s32>& size);
		virtual void OnResize(const core::dimension2d<s32>& size);
		virtual s32 addMaterialRenderer(video::IMaterialRenderer* renderer, const c8* name = 0);
		virtual video::IMaterialRenderer* getMaterialRenderer(u32 idx) const;
		virtual u32 getMaterialRendererCount() const;
		virtual const c8* getMaterialRendererName(u32 idx) const;
		virtual void setMaterialRendererName(s32 idx, const c8* name);
		virtual io::IAttributes* createAttributesFromMaterial(const video::SMaterial& material);
		virtual void fillMaterialStructureFromAttributes(video::SMaterial& outMaterial, io::IAttributes* attributes);
		virtual const video::SExposedVideoData& getExposedVideoData();
		virtual video::E_DRIVER_TYPE getDriverType() const;
		virtual video::IGPUProgrammingServices* getGPUProgrammingServices();
		virtual scene::IMeshManipulator* getMeshManipulator();
		virtual video::IImage* createScreenShot();
		virtual video::ITexture* findTexture(const c8* filename);
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable=false);
		virtual void enableClipPlane(u32 index, bool enable);
		virtual core::stringc getVendorInfo();
		virtual void setAmbientLight(const video::SColorf& color);
		virtual void setMaxTextureSize(const core::dimension2d<s32>& maxTextureSize);
		virtual const core::dimension2d<s32>& getMaxTextureSize();
		virtual void setOrthoOrientation(video::E_ORTHO_ORIENTATION newOri);
		virtual video::E_ORTHO_ORIENTATION getOrthoOrientation();
		virtual void setOrientation3D(video::E_ORIENTATION newOri);
		virtual video::E_ORIENTATION getOrientation3D();
		virtual void Orientation3DProjectionTransform(irr::f32 *matrix, video::E_ORIENTATION orientation);
		virtual void Orientation3DViewTransform(irr::f32 *matrix, video::E_ORIENTATION orientation);
		virtual void Orientation3D_ScreenPos2Internal(irr::s32 &x, irr::s32 &y);
		virtual void Orientation3D_Internal2ScreenPos(irr::s32 &x, irr::s32 &y);
		virtual void setScissor(const core::rect<s32>& scissorRect);
		virtual void resetScissor();
		virtual void setColorMask(bool red, bool green, bool blue, bool alpha);
		virtual void setDepthRange(float near, float far) {};
		virtual const video::SColorf& getAmbientLight() const;
		virtual bool getOption(u32 option) const;
		virtual void setOption(u32 option, bool value);
		virtual void beginCompile(ICompileData*);
		virtual void endCompile();
		virtual video::E_DRIVER_ALLOCATION_RESULT getProcessBuffer(
			u32 vertexStart,
			u32 vertexEnd,
			u32 attributes,
			video::E_PROCESS_BUFFER_TYPE type,
			video::S3DVertexComponentArrays& components,
			video::IDriverBinding** binding = NULL,
			bool allocate = false
		);
		virtual void releaseProcessBuffer(video::E_PROCESS_BUFFER_TYPE type,
										  void* bindingOrDynamicBuffer,
										  u32 vertexStart = 0,
										  u32 stride = 0);
		virtual video::IDriverBinding* createBinding();

		virtual void setTextureFullPathMode(bool value);
	};

private:

	scene::ISceneNode*		InstancedSubScene;
	core::matrix4			CurInstAbsoluteTransformation;
	CVideoDriverProxy		DriverProxy;

	int						TransparentListRenderCount;
	int						SolidListRenderCount;
	u32						CurrentTime;
};

};
};

#endif //__CCOLLADAINSTANCEDSCENENODE_H__

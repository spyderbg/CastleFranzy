#include "stdafx.h"
#include "CColladaInstancedSceneNode.h"
#include "ICameraSceneNode.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace collada
{

CInstancedSceneNode::CInstancedSceneNode(CInstancingProxy* proxy)
	: InstanceProxy(proxy)
{
	InstanceProxy->grab();
	//setAutomaticCulling(scene::EAC_OFF);
}

CInstancedSceneNode::~CInstancedSceneNode()
{
	if (InstanceProxy)
	{
		InstanceProxy->drop();
	}
}

scene::ESCENE_NODE_TYPE
CInstancedSceneNode::getType() const 
{ 
	return scene::ESNT_COLLADA_ROOT;
}

const core::aabbox3d<f32>&
CInstancedSceneNode::getBoundingBox() const
{
	//TODO-FH:
	return InstanceProxy->getChildren().getFirstElem()->getBoundingBox();
}

void
CInstancedSceneNode::OnAnimate(u32 timeMs)
{
	InstanceProxy->OnAnimate(timeMs);

	//Animate children if any
	ISceneNode::OnAnimate(timeMs);
}

void
CInstancedSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		//NOTE-FH: Potential render sorting problem in the instanceProxy Lists

		//set the camera eye for sorting
		scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
		if (!camera)
			return;

		core::vector3df eyePos = camera->getAbsolutePosition();
		//get the inverse of the instanced node world
		core::matrix4 worldInv;
		AbsoluteTransformation.getInverse(worldInv);
		worldInv.transformVect(eyePos);
		InstanceProxy->setCameraEye(eyePos);

		//If the proxy hasnt registered his nodes through another instance it will do it
		//NOTE-FH: Might use collectAllNodes optim
		InstanceProxy->OnRegisterSceneNode();

		//Register according to mesh types to render counted
		if (InstanceProxy->getSolidNodeCount())
		{
			if (SceneManager->registerNodeForRendering(this, 0, 0, scene::ESNRP_SOLID) == 1)
			{
				InstanceProxy->increaseSolidRenderCount();
			}
		}

		if (InstanceProxy->getTransparentNodeCount())
		{
			if (SceneManager->registerNodeForRendering(this, 0, 0, scene::ESNRP_TRANSPARENT) == 1)
			{
				InstanceProxy->increaseTransparentRenderCount();
			}
		}

		//Register children if any
		ISceneNode::OnRegisterSceneNode();
	}
}

void
CInstancedSceneNode::render(void*)
{
	//set the absoluteTransformation for the instance
	InstanceProxy->setCurrentTransformation(AbsoluteTransformation);

	//TODO-FH: Override some materials if required

	//Render the subscene
	if (SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT)
		InstanceProxy->renderTransparentList();
	else
		InstanceProxy->renderSolidList();
}

//------------------------------------------------------------------------------

CInstancingProxy::CInstancingProxy(video::IVideoDriver* driver,
								   scene::ISceneNode* instancedNode)
	: CSceneManager(driver, 0, 0)
	, InstancedSubScene(instancedNode)
	, DriverProxy(this)
	, TransparentListRenderCount(0)
	, SolidListRenderCount(0)
	, CurrentTime(0)
{
	InstancedSubScene->grab();
	addChild(InstancedSubScene);
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	collectAllNodes();
	HierarchyChanged = false;
#endif
}

CInstancingProxy::~CInstancingProxy()
{
	if (InstancedSubScene)
		InstancedSubScene->drop();
}

void
CInstancingProxy::OnAnimate(u32 timeMs)
{
	if (CurrentTime != timeMs)
	{
		CSceneManager::OnAnimate(timeMs);
		CurrentTime = timeMs;
	}
}

void
CInstancingProxy::OnRegisterSceneNode()
{
	if (!isRegistered())
	{
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		if(HierarchyChanged)
		{
			collectAllNodes();
		}
#endif
		CSceneManager::OnRegisterSceneNode();
	}
}

void
CInstancingProxy::renderSolidList()
{
	CurrentRendertime = scene::ESNRP_SOLID;
	SolidNodeList.sort(); // sort by textures

	for (u32 i = 0; i < SolidNodeList.size(); ++i)
		SolidNodeList[i].Node->render(SolidNodeList[i].RenderData);

	Parameters.setAttribute ( "drawn", (s32) SolidNodeList.size() );

	if (--SolidListRenderCount == 0)
		SolidNodeList.set_used(0);

	CurrentRendertime = scene::ESNRP_COUNT;
}

void
CInstancingProxy::renderTransparentList()
{
	CurrentRendertime = scene::ESNRP_TRANSPARENT;
	TransparentNodeList.sort(); // sort by distance from camera

	for (u32 i = 0; i < TransparentNodeList.size(); ++i)
		TransparentNodeList[i].Node->render(TransparentNodeList[i].RenderData);

	if (--TransparentListRenderCount == 0)
		TransparentNodeList.set_used(0);

	CurrentRendertime = scene::ESNRP_COUNT;
}

video::IVideoDriver*
CInstancingProxy::getVideoDriver()
{
	return &DriverProxy;
}

const video::SColorf&
CInstancingProxy::getAmbientLight() const
{
	return AmbientLight;
}

void
CInstancingProxy::setAmbientLight(const video::SColorf &ambientColor)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

u32
CInstancingProxy::registerNodeForRendering(scene::ISceneNode* node,
										   const video::SMaterial* material,
										   void* renderData,
										   scene::E_SCENE_NODE_RENDER_PASS pass)
{
	u32 taken = 0;

	switch(pass)
	{
		// take camera if it doesn't exists
		case scene::ESNRP_CAMERA:
		{
		}
		break;

		case scene::ESNRP_LIGHT:
			break;

		case scene::ESNRP_SKY_BOX:
			//SkyBoxList.push_back(node);
			//taken = 1;
			break;

		case scene::ESNRP_SOLID:
			if (!isCulled(node))
			{
				SolidNodeList.push_back(SDefaultNodeEntry(node,
														  material,
														  renderData));
				taken = 1;
			}
			break;

		case scene::ESNRP_TRANSPARENT:
			if (!isCulled(node))
			{
				TransparentNodeList.push_back(STransparentNodeEntry(node,
																	CamWorldPos,
																	material,
																	renderData));
				taken = 1;
			}
			break;

		case scene::ESNRP_AUTOMATIC:
			if (!isCulled(node))
			{
				taken = 1; 
				if (material)
				{
					video::IMaterialRenderer* rnd =
						Driver->getMaterialRenderer(material->getMaterialType());
					if (rnd && rnd->isTransparent())
					{
						TransparentNodeList.push_back(STransparentNodeEntry(node,
																			CamWorldPos,
																			material,
																			renderData));
						break;
					}
				}
				SolidNodeList.push_back(SDefaultNodeEntry(node,
														  material,
														  renderData));
			}
			break;

		case scene::ESNRP_SHADOW:
			//if (!isCulled(node))
			//{
			//	ShadowNodeList.push_back(node);
			//	taken = 1;
			//}
			break;
		case scene::ESNRP_COUNT: // ignore this one
			break;
	}
#ifdef DEBUG_STATISTICS
	++Stats.CullTests;
	if (0 == taken)
	{
		++Stats.Culled;
	}
#endif
	return taken;
}

void
CInstancingProxy::CVideoDriverProxy::setTransform(video::E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	core::matrix4 stateMat = mat;
	if (state == video::ETS_WORLD)
	{
		stateMat = Proxy->CurInstAbsoluteTransformation * mat;
	}
	Proxy->Driver->setTransform(state, stateMat);
}

const core::matrix4&
CInstancingProxy::CVideoDriverProxy::getTransform(video::E_TRANSFORMATION_STATE state) const
{
	return Proxy->Driver->getTransform(state);
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::createDeviceDependentNativeTexture(const char* name, int w, int h, int fmt, void* data)
{
	return Proxy->Driver->createDeviceDependentNativeTexture(name, w, h, fmt, data);
}

const wchar_t*
CInstancingProxy::CVideoDriverProxy::getName() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

bool
CInstancingProxy::CVideoDriverProxy::beginScene()
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

bool
CInstancingProxy::CVideoDriverProxy::endScene()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

bool
CInstancingProxy::CVideoDriverProxy::swapBuffers(int)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::clearBuffers(int)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::setClearColor(const video::SColor& color)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::SColor
CInstancingProxy::CVideoDriverProxy::getClearColor() const
{
	return video::SColor(0, 0, 0, 0);
}

void
CInstancingProxy::CVideoDriverProxy::setClearDepth(float)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

float
CInstancingProxy::CVideoDriverProxy::getClearDepth() const
{
	return 1.0f;
}

void
CInstancingProxy::CVideoDriverProxy::setClearStencil(int)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

int
CInstancingProxy::CVideoDriverProxy::getClearStencil() const
{
	return 0;
}

bool
CInstancingProxy::CVideoDriverProxy::beginScene2D()
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

bool
CInstancingProxy::CVideoDriverProxy::endScene2D()
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

bool
CInstancingProxy::CVideoDriverProxy::queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::disableFeature(video::E_VIDEO_DRIVER_FEATURE feature, bool flag)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

bool
CInstancingProxy::CVideoDriverProxy::checkDriverReset()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::setMaterial(const video::SMaterial& material)
{ 
	Proxy->Driver->setMaterial(material);
}

void
CInstancingProxy::CVideoDriverProxy::set2DMaterial(video::SMaterial& material)
{
	Proxy->Driver->set2DMaterial(material);
}

void
CInstancingProxy::CVideoDriverProxy::set2DTexture(const video::ITexture* tex, bool useTextureAlpha)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::set2DUseVertexAlpha(bool value)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::getTexture(const c8* filename)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::getTexture(const core::stringc& filename)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::getTexture(io::IReadFile* file, bool refData)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::getTextureByIndex(u32 index)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

u32
CInstancingProxy::CVideoDriverProxy::getTextureCount() const
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::renameTexture(video::ITexture* texture, const c8* newName)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::addTexture(const core::dimension2d<s32>& size,
												const c8* name,
												video::ECOLOR_FORMAT format)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::addTexture(const c8* name, video::IImage* image)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::addRenderTargetTexture(const core::dimension2d<s32>& size,
															const c8* name,
															video::ECOLOR_FORMAT format)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::createRenderTargetTexture(const core::dimension2d<s32>& size,
															   const c8* name)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::removeTexture(video::ITexture* texture)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::removeAllTextures()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::removeHardwareBuffer(const scene::IMeshBuffer* mb)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::removeAllHardwareBuffers()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::makeColorKeyTexture(video::ITexture* texture, video::SColor color) const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::makeColorKeyTexture(video::ITexture* texture,
														 core::position2d<s32> colorKeyPixelPos) const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::makeNormalMapTexture(video::ITexture* texture, f32 amplitude) const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

bool
CInstancingProxy::CVideoDriverProxy::setRenderTarget(video::ITexture* texture,
													 int)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::setViewPort(const core::rect<s32>& area)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

const core::rect<s32>&
CInstancingProxy::CVideoDriverProxy::getViewPort() const
{ 
	return Proxy->Driver->getViewPort();
}

void
CInstancingProxy::CVideoDriverProxy::drawVertexPrimitiveList(const void* vertices,
															 const void* indexList,
															 u32 startIndex,
															 u32 endIndex,
															 u32 primCount,
															 video::E_VERTEX_TYPE vType,
															 scene::E_PRIMITIVE_TYPE pType,
															 video::E_INDEX_TYPE iType,
															 video::IDriverBinding** binding)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw3DLine(const core::vector3df& start,
												const core::vector3df& end, video::SColor color)
{ 
	Proxy->Driver->draw3DLine(start, end, color);
}

void
CInstancingProxy::CVideoDriverProxy::draw3DTriangle(const core::triangle3df& triangle,
													video::SColor color)
{
	Proxy->Driver->draw3DTriangle(triangle, color);
}

void
CInstancingProxy::CVideoDriverProxy::draw3DBox(const core::aabbox3d<f32>& box,
											   video::SColor color)
{ 
	Proxy->Driver->draw3DBox(box, color);
}

void
CInstancingProxy::CVideoDriverProxy::draw2DImage(const video::ITexture* texture,
												 const core::position2d<s32>& destPos)
{ 
	Proxy->Driver->draw2DImage(texture, destPos);
}

void
CInstancingProxy::CVideoDriverProxy::draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
												 const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
												 video::SColor color, bool useAlphaChannelOfTexture)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DImage(const video::ITexture* texture,
												 const core::position2d<s32>& pos,
												 const core::array<core::rect<s32> >& sourceRects,
												 const core::array<s32>& indices,
												 s32 kerningWidth,
												 const core::rect<s32>* clipRect,
												 video::SColor color,
												 bool useAlphaChannelOfTexture)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
												 const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
												 const video::SColor * const colors, bool useAlphaChannelOfTexture)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DRectangle(video::SColor color, const core::rect<s32>& pos,
													 const core::rect<s32>* clip)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DRectangle(const core::rect<s32>& pos,
													 video::SColor colorLeftUp, video::SColor colorRightUp,
													 video::SColor colorLeftDown, video::SColor colorRightDown,
													 const core::rect<s32>* clip)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DLine(const core::position2d<s32>& start,
												const core::position2d<s32>& end,
												video::SColor color)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DPolygon(core::position2d<s32> center,
												   f32 radius,
												   video::SColor color,
												   s32 vertexCount)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::draw2DRectangle(const core::rect<s32>& destRect,
													 const core::rect<s32>& sourceRect,
													 const video::SColor colors[4],
													 const core::rect<s32>* clipRect)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}


void
CInstancingProxy::CVideoDriverProxy::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::drawStencilShadow(bool clearStencilBuffer,
													   video::SColor leftUpEdge,
													   video::SColor rightUpEdge,
													   video::SColor leftDownEdge,
													   video::SColor rightDownEdge)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::drawMeshBuffer( const scene::IMeshBuffer* mb)
{
	Proxy->Driver->drawMeshBuffer(mb);
}

void
CInstancingProxy::CVideoDriverProxy::setFog(video::SColor color,
											bool linearFog, f32 start, f32 end,
											f32 density,
											bool pixelFog, bool rangeFog)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::ECOLOR_FORMAT
CInstancingProxy::CVideoDriverProxy::getColorFormat() const
{ 
	return Proxy->Driver->getColorFormat();
}

const core::dimension2d<s32>&
CInstancingProxy::CVideoDriverProxy::getScreenSize() const
{ 
	return Proxy->Driver->getScreenSize();
}

const core::dimension2d<s32>&
CInstancingProxy::CVideoDriverProxy::getCurrentRenderTargetSize() const
{ 
	return Proxy->Driver->getCurrentRenderTargetSize();
}

s32
CInstancingProxy::CVideoDriverProxy::getFPS() const
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

u32
CInstancingProxy::CVideoDriverProxy::getPrimitiveCountDrawn( u32 mode ) const
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

u32
CInstancingProxy::CVideoDriverProxy::getDrawCallCount() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

u32
CInstancingProxy::CVideoDriverProxy::getDrawCall2DCount() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

u32
CInstancingProxy::CVideoDriverProxy::getTextureBindingCount() const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::deleteAllDynamicLights()
{ 
	Proxy->Driver->deleteAllDynamicLights();
}

void
CInstancingProxy::CVideoDriverProxy::addDynamicLight(const video::SLight& light)
{
	Proxy->Driver->addDynamicLight(light);
}

u32
CInstancingProxy::CVideoDriverProxy::getMaximalDynamicLightAmount() const
{
	return Proxy->Driver->getMaximalDynamicLightAmount();
}

u32
CInstancingProxy::CVideoDriverProxy::getDynamicLightCount() const
{
	return Proxy->Driver->getDynamicLightCount();
}

const video::SLight&
 CInstancingProxy::CVideoDriverProxy::getDynamicLight(u32 idx) const
{ 
	return Proxy->Driver->getDynamicLight(idx);
}

void
 CInstancingProxy::CVideoDriverProxy::addExternalImageLoader(video::IImageLoader* loader)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::addExternalImageWriter(video::IImageWriter* writer)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

u32
CInstancingProxy::CVideoDriverProxy::getMaximalPrimitiveCount() const
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImageFromFile(const c8* filename)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImageFromFile(io::IReadFile* file)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

bool
CInstancingProxy::CVideoDriverProxy::writeImageToFile(video::IImage* image, const c8* filename, u32 param)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImageFromData(video::ECOLOR_FORMAT format,
														 const core::dimension2d<s32>& size, void *data,
														 bool ownForeignMemory,
														 bool deleteMemory)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImage(video::ECOLOR_FORMAT format, const core::dimension2d<s32>& size)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImage(video::ECOLOR_FORMAT format, video::IImage *imageToCopy)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createImage(video::IImage* imageToCopy,
												 const core::position2d<s32>& pos, const core::dimension2d<s32>& size)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::OnResize(const core::dimension2d<s32>& size)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

s32
CInstancingProxy::CVideoDriverProxy::addMaterialRenderer(video::IMaterialRenderer* renderer, const c8* name)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IMaterialRenderer*
CInstancingProxy::CVideoDriverProxy::getMaterialRenderer(u32 idx) const
{
	return Proxy->Driver->getMaterialRenderer(idx);
}

u32
CInstancingProxy::CVideoDriverProxy::getMaterialRendererCount() const
{
	return Proxy->Driver->getMaterialRendererCount();
}

const c8*
CInstancingProxy::CVideoDriverProxy::getMaterialRendererName(u32 idx) const
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::setMaterialRendererName(s32 idx, const c8* name)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

io::IAttributes*
CInstancingProxy::CVideoDriverProxy::createAttributesFromMaterial(const video::SMaterial& material)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

void
CInstancingProxy::CVideoDriverProxy::fillMaterialStructureFromAttributes(video::SMaterial& outMaterial, io::IAttributes* attributes)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

const video::SExposedVideoData&
CInstancingProxy::CVideoDriverProxy::getExposedVideoData()
{
	return Proxy->Driver->getExposedVideoData();
}

video::E_DRIVER_TYPE
CInstancingProxy::CVideoDriverProxy::getDriverType() const
{
	return Proxy->Driver->getDriverType();
}

video::IGPUProgrammingServices*
CInstancingProxy::CVideoDriverProxy::getGPUProgrammingServices()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

scene::IMeshManipulator*
CInstancingProxy::CVideoDriverProxy::getMeshManipulator()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::IImage*
CInstancingProxy::CVideoDriverProxy::createScreenShot()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

video::ITexture*
CInstancingProxy::CVideoDriverProxy::findTexture(const c8* filename)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return 0;
}

bool
CInstancingProxy::CVideoDriverProxy::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{ 
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::enableClipPlane(u32 index, bool enable)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

core::stringc
CInstancingProxy::CVideoDriverProxy::getVendorInfo()
{
	return Proxy->Driver->getVendorInfo();
}

void
CInstancingProxy::CVideoDriverProxy::setAmbientLight(const video::SColorf& color)
{
	Proxy->Driver->setAmbientLight(color);
}

void
CInstancingProxy::CVideoDriverProxy::setMaxTextureSize(const core::dimension2d<s32>& maxTextureSize)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

const core::dimension2d<s32>&
CInstancingProxy::CVideoDriverProxy::getMaxTextureSize()
{ 
	return Proxy->Driver->getMaxTextureSize();
}

void
CInstancingProxy::CVideoDriverProxy::setOrthoOrientation(video::E_ORTHO_ORIENTATION newOri)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::E_ORTHO_ORIENTATION
CInstancingProxy::CVideoDriverProxy::getOrthoOrientation()
{ 
	return Proxy->Driver->getOrthoOrientation();
}

void
CInstancingProxy::CVideoDriverProxy::setOrientation3D(video::E_ORIENTATION newOri)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

video::E_ORIENTATION
CInstancingProxy::CVideoDriverProxy::getOrientation3D()
{
	return Proxy->Driver->getOrientation3D();
}

void
CInstancingProxy::CVideoDriverProxy::Orientation3DProjectionTransform(irr::f32 *matrix, video::E_ORIENTATION orientation)
{
	Proxy->Driver->Orientation3DProjectionTransform(matrix, orientation);
}

void
CInstancingProxy::CVideoDriverProxy::Orientation3DViewTransform(irr::f32 *matrix, video::E_ORIENTATION orientation)
{
	Proxy->Driver->Orientation3DViewTransform(matrix, orientation);
}

void
CInstancingProxy::CVideoDriverProxy::Orientation3D_ScreenPos2Internal(irr::s32 &x, irr::s32 &y)
{
	Proxy->Driver->Orientation3D_ScreenPos2Internal(x, y);
}

void
CInstancingProxy::CVideoDriverProxy::Orientation3D_Internal2ScreenPos(irr::s32 &x, irr::s32 &y)
{
	Proxy->Driver->Orientation3D_Internal2ScreenPos(x, y);
}

void
CInstancingProxy::CVideoDriverProxy::setScissor(const core::rect<s32>& scissorRect)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::resetScissor()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

void
CInstancingProxy::CVideoDriverProxy::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
}

const video::SColorf&
CInstancingProxy::CVideoDriverProxy::getAmbientLight() const
{ 
	return Proxy->Driver->getAmbientLight();
}

bool
CInstancingProxy::CVideoDriverProxy::getOption(u32 option) const
{
	return false;
}

void
CInstancingProxy::CVideoDriverProxy::setOption(u32 option, bool value)
{
	// nothing
}

void
CInstancingProxy::CVideoDriverProxy::beginCompile(ICompileData*)
{
	// nothing
}

void
CInstancingProxy::CVideoDriverProxy::endCompile()
{
	// nothing
}

video::E_DRIVER_ALLOCATION_RESULT
CInstancingProxy::CVideoDriverProxy::getProcessBuffer(
	u32 vertexStart,
	u32 vertexEnd,
	u32 attributes,
	video::E_PROCESS_BUFFER_TYPE type,
	video::S3DVertexComponentArrays& components,
	video::IDriverBinding** binding,
	bool allocate
)
{
	return Proxy->Driver->getProcessBuffer(vertexStart,
										   vertexEnd,
										   attributes,
										   type,
										   components,
										   binding,
										   allocate);
	return video::EDAR_FAILED;
}

void
CInstancingProxy::CVideoDriverProxy::releaseProcessBuffer(
	video::E_PROCESS_BUFFER_TYPE type,
	void* bindingOrDynamicBuffer,
	u32 vertexStart,
	u32 stride
)
{
	Proxy->Driver->releaseProcessBuffer(type,
										bindingOrDynamicBuffer,
										vertexStart,
										stride);
}

video::IDriverBinding* CInstancingProxy::CVideoDriverProxy::createBinding()
{
	_IRR_DEBUG_BREAK_IF("Not implemented");
	return NULL;
}

void CInstancingProxy::CVideoDriverProxy::setTextureFullPathMode(bool value)
{
	// nothing
}

} // namespace collada
} // namespace irr

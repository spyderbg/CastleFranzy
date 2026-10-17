//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SCENE_MANAGER_H_INCLUDED__
#define __C_SCENE_MANAGER_H_INCLUDED__

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ICursorControl.h"
#include "irrString.h"
#include "irrArray.h"
#include "IMeshLoader.h"
#include "CAttributes.h"

namespace irr
{
namespace io
{
class IXMLWriter;
class IFileSystem;
}
namespace scene
{
class IMeshCache;

	
/*!
  The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
*/
class CSceneManager : public ISceneManager, public ISceneNode
{
public:
		
	//! constructor
	CSceneManager(video::IVideoDriver* driver,
				  io::IFileSystem* fs,
				  gui::ICursorControl* cursorControl,
				  IMeshCache* cache = 0,
				  gui::IGUIEnvironment* guiEnvironment = 0);

	//! destructor
	virtual ~CSceneManager();

	//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
	virtual IAnimatedMesh* getMesh(const c8* filename);

	//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
	virtual IAnimatedMesh* getMesh(io::IReadFile* file);

	virtual ISceneNode *loadColladaScene( const c8* filename );
	virtual ISceneNode *loadColladaScene( io::IReadFile* file );

	//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
	virtual IMeshCache* getMeshCache();

	//! returns the video driver
	virtual video::IVideoDriver* getVideoDriver();

	//! Returns the first scene node with the specified uid.
	virtual ISceneNode* getSceneNodeFromUID(const c8* uid, ISceneNode* start=0);

	//! Returns the first scene node with the specified name.
	virtual ISceneNode* getSceneNodeFromName(const c8* name, ISceneNode* start=0);


	virtual gui::IGUIEnvironment* getGUIEnvironment();

	//! adds Volume Lighting Scene Node.
	//! the returned pointer must not be dropped.
	virtual IVolumeLightSceneNode* addVolumeLightSceneNode(
		ISceneNode* parent=0,
		s32 id=-1,
		const u32 subdivU = 32,
		const u32 subdivV = 32,
		const video::SColor foot = video::SColor(51, 0, 230, 180),
		const video::SColor tail = video::SColor(0, 0, 0, 0),
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	//! adds a cube scene node to the scene. It is a simple cube of (1,1,1) size.
	//! the returned pointer must not be dropped.
	virtual IMeshSceneNode* addCubeSceneNode(
		f32 size=10.0f,
		ISceneNode* parent=0,
		s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	//! Adds a sphere scene node to the scene.
	virtual IMeshSceneNode* addSphereSceneNode(
		f32 radius=5.0f,
		s32 polyCount=16,
		ISceneNode* parent=0,
		s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	//! adds a scene node for rendering an animated mesh model
	virtual IAnimatedMeshSceneNode* addAnimatedMeshSceneNode(
		IAnimatedMesh* mesh,
		ISceneNode* parent=0,
		s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
		bool alsoAddIfMeshPointerZero=false
	);

	//! adds a scene node for rendering a static mesh
	//! the returned pointer must not be dropped.
	virtual IMeshSceneNode* addMeshSceneNode(
		IMesh* mesh,
		ISceneNode* parent=0,
		s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
		bool alsoAddIfMeshPointerZero=false
	);

	//! Adds a scene node for rendering a animated water surface mesh.
	virtual ISceneNode* addWaterSurfaceSceneNode(
		IMesh* mesh,
		f32 waveHeight,
		f32 waveSpeed,
		f32 wLength,
		ISceneNode* parent=0,
		s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	//! renders the node.
	virtual void render(void* renderData = 0);

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! registers a node for rendering it at a specific time.
	virtual u32 registerNodeForRendering(ISceneNode* node,
										 const video::SMaterial* material,// = NULL,
										 void* renderData,// = NULL,
										 E_SCENE_NODE_RENDER_PASS = ESNRP_AUTOMATIC,
										 const core::vector3df* absPos = NULL,
										 int layer = u32(-1) >> 1);

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	void collectAllNodes();
	void collectAllNodes(ISceneNode* node);
	virtual void notifyHierarchyChanged();
#endif

	virtual void OnRegisterSceneNode();

	//! draws all scene nodes
	virtual void drawAll(ISceneNode* root = NULL);

	//! update all scene nodes
	void update(f32 deltaTimeMillis = DELTA_TIME_USE_SYSTEM_TIME, bool bInternalTimeOnly = false);

	//! Adds a scene node for rendering using a octtree to the scene graph. This a good method for rendering
	//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
	//! faster then a bsp tree.
	virtual ISceneNode* addOctTreeSceneNode(
		IAnimatedMesh* mesh,
		ISceneNode* parent=0,
		s32 id=-1,
		s32 minimalPolysPerNode=512,
		bool alsoAddIfMeshPointerZero=false
	);

	//! Adss a scene node for rendering using a octtree. This a good method for rendering
	//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
	//! faster then a bsp tree.
	virtual ISceneNode* addOctTreeSceneNode(
		IMesh* mesh,
		ISceneNode* parent=0,
		s32 id=-1,
		s32 minimalPolysPerNode=128,
		bool alsoAddIfMeshPointerZero=false
	);

	//! Adds a camera scene node to the tree and sets it as active camera.
	//! \param position: Position of the space relative to its parent where the camera will be placed.
	//! \param lookat: Position where the camera will look at. Also known as target.
	//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
	//! the camera will move too.
	//! \return Returns pointer to interface to camera
	virtual ICameraSceneNode* addCameraSceneNode(
		ISceneNode* parent = 0,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& lookat = core::vector3df(0,0,0), s32 id=-1
	);

	//! Adds a camera scene node to the tree and sets it as active camera.
	//! \param position: Position of the space relative to its parent where the camera will be placed.
	//! \param lookat: ISceneNode where the camera will look at. Also known as target.
	//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
	//! the camera will move too.
	//! \return Returns pointer to interface to camera
	virtual ICameraSceneNode* addCameraSceneNodeTargetTraker(
		ISceneNode* parent = 0,
		const core::vector3df& position = core::vector3df(0,0,0),
		const ISceneNode * lookat = 0,
		s32 id=-1
	);

	//! Adds a camera scene node which is able to be controlle with the mouse similar
	//! like in the 3D Software Maya by Alias Wavefront.
	//! The returned pointer must not be dropped.
	virtual ICameraSceneNode* addCameraSceneNodeMaya(
		ISceneNode* parent = 0,
		f32 rotateSpeed = -1500.0f,
		f32 zoomSpeed = 200.0f,
		f32 translationSpeed = 100.0f,
		s32 id=-1
	);

	//! Adds a camera scene node which is able to be controled with the mouse and keys
	//! like in most first person shooters (FPS):
	virtual ICameraSceneNode* addCameraSceneNodeFPS(
		ISceneNode* parent = 0,
		f32 rotateSpeed = 1500.0f,
		f32 moveSpeed = 200.0f,
		s32 id=-1,
		SKeyMap* keyMapArray=0,
		s32 keyMapSize=0,
		bool noVerticalMovement=false,
		f32 jumpSpeed = 0.f
	);

	//! Adds a dynamic light scene node. The light will cast dynamic light on all
	//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
	//! turned on. (This is the default setting in most scene nodes).
	virtual ILightSceneNode* addLightSceneNode(
		ISceneNode* parent = 0,
		const core::vector3df& position = core::vector3df(0,0,0),
		video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),
		f32 range=100.0f,
		s32 id=-1
	);

	//! Adds a billboard scene node to the scene. A billboard is like a 3d sprite: A 2d element,
	//! which always looks to the camera. It is usually used for things like explosions, fire,
	//! lensflares and things like that.
	virtual IBillboardSceneNode* addBillboardSceneNode(
		ISceneNode* parent = 0,
		const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
		const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1,
		video::SColor shade_top = video::SColor(0xFF,0xFF,0xFF,0xFF), 
		video::SColor shade_down = video::SColor(0xFF,0xFF,0xFF,0xFF)
	);

	//! Adds a skybox scene node. A skybox is a big cube with 6 textures on it and
	//! is drawn around the camera position.
	virtual ISceneNode* addSkyBoxSceneNode(
		video::ITexture* top,
		video::ITexture* bottom,
		video::ITexture* left,
		video::ITexture* right,
		video::ITexture* front,
		video::ITexture* back,
		ISceneNode* parent = 0,
		s32 id=-1
	);

	//! Adds a skydome scene node. A skydome is a large (half-) sphere with a
	//! panoramic texture on it and is drawn around the camera position.
	virtual ISceneNode* addSkyDomeSceneNode(
		video::ITexture* texture,
		u32 horiRes,
		u32 vertRes,
		f64 texturePercentage,
		f64 spherePercentage,
		ISceneNode* parent=0,
		s32 id=-1
	);

	//! Adds a text scene node, which is able to display
	//! 2d text at a position in three dimensional space
	virtual ITextSceneNode* addTextSceneNode(
		gui::IGUIFont* font,
		const wchar_t* text,
		video::SColor color=video::SColor(100,0xFF,0xFF,0xFF),
		ISceneNode* parent = 0,
		const core::vector3df& position = core::vector3df(0,0,0),
		s32 id=-1
	);

	//! Adds a text scene node, which uses billboards
	virtual ITextSceneNode* addBillboardTextSceneNode(
		gui::IGUIFont* font,
		const wchar_t* text,
		ISceneNode* parent = 0,
		const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
		const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1,
		video::SColor shade_top = video::SColor(0xFF,0xFF,0xFF,0xFF), 
		video::SColor shade_down = video::SColor(0xFF,0xFF,0xFF,0xFF)
	);

	//! Adds a scene node, which can render a quake3 shader
	virtual ISceneNode* addQuake3SceneNode(
		IMeshBuffer* meshBuffer,
		const quake3::SShader * shader,
		ISceneNode* parent=0, s32 id=-1
	);


	//! Adds a Hill Plane mesh to the mesh pool. The mesh is
	//! generated on the fly and looks like a plane with some hills
	//! on it. You can specify how many hills should be on the plane
	//! and how high they should be. Also you must specify a name
	//! for the mesh because the mesh is added to the mesh pool and
	//! can be retrieved back using ISceneManager::getMesh with the
	//! name as parameter.
	virtual IAnimatedMesh* addHillPlaneMesh(
		const c8* name,
		const core::dimension2d<f32>& tileSize,
		const core::dimension2d<u32>& tileCount,
		video::SMaterial* material = 0,
		f32 hillHeight = 0.0f,
		const core::dimension2d<f32>& countHills = core::dimension2d<f32>(1.0f, 1.0f),
		const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.0f, 1.0f)
	);

	//! Adds a terrain mesh to the mesh pool.
	virtual IAnimatedMesh* addTerrainMesh(
		const c8* meshname,
		video::IImage* texture,
		video::IImage* heightmap,
		const core::dimension2d<f32>& stretchSize,
		f32 maxHeight,
		const core::dimension2d<s32>& defaultVertexBlockSize
	);

	//! Add a arrow mesh to the mesh pool
	virtual IAnimatedMesh* addArrowMesh(
		const c8* name,
		video::SColor vtxColor0,
		video::SColor vtxColor1,
		u32 tesselationCylinder,
		u32 tesselationCone,
		f32 height,
		f32 cylinderHeight,
		f32 width0,
		f32 width1
	);

	//! Adds a static sphere mesh to the mesh pool.
	IAnimatedMesh* addSphereMesh(
		const c8* name,
		f32 radius,
		u32 polyCountX,
		u32 polyCountY
	);

	//! Adds a particle system scene node.
	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(
		bool withDefaultEmitter=true, ISceneNode* parent=0, s32 id=-1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	//! Adds a terrain scene node to the scene graph.
	virtual ITerrainSceneNode* addTerrainSceneNode(
		const c8* heightMapFileName,
		ISceneNode* parent=0, s32 id=-1,
		const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
		const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
		const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
		video::SColor vertexColor = video::SColor(255,255,255,255),
		s32 maxLOD=4, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17,s32 smoothFactor=0,
		bool addAlsoIfHeightmapEmpty = false
	);

	//! Adds a terrain scene node to the scene graph.
	virtual ITerrainSceneNode* addTerrainSceneNode(
		io::IReadFile* heightMap,
		ISceneNode* parent=0, s32 id=-1,
		const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
		const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
		const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
		video::SColor vertexColor = video::SColor(255,255,255,255),
		s32 maxLOD=4, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17,s32 smoothFactor=0,
		bool addAlsoIfHeightmapEmpty=false
	);

	//! Adds a dummy transformation scene node to the scene graph.
	virtual IDummyTransformationSceneNode* addDummyTransformationSceneNode(
		ISceneNode* parent=0,
		s32 id=-1
	);

	//! Adds an empty scene node.
	virtual ISceneNode* addEmptySceneNode(ISceneNode* parent, s32 id=-1);

	//! Returns the root scene node. This is the scene node wich is parent
	//! of all scene nodes. The root scene node is a special scene node which
	//! only exists to manage all scene nodes. It is not rendered and cannot
	//! be removed from the scene.
	//! \return Returns a pointer to the root scene node.
	virtual ISceneNode* getRootSceneNode();

	//! Returns the current active camera.
	//! \return The active camera is returned. Note that this can be NULL, if there
	//! was no camera created yet.
	virtual ICameraSceneNode* getActiveCamera();

	//! Sets the active camera. The previous active camera will be deactivated.
	//! \param camera: The new camera which should be active.
	virtual void setActiveCamera(ICameraSceneNode* camera);

	//! creates a rotation animator, which rotates the attached scene node around itself.
	//! \param rotationPerSecond: Specifies the speed of the animation
	//! \return Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
	//! and the animator will animate it.
	virtual ISceneNodeAnimator* createRotationAnimator(const core::vector3df& rotationPerSecond);

	//! creates a fly circle animator
	/** Lets the attached scene node fly around a center.
		\param center Center relative to node origin
		\param speed rotation speed
		\return Animator. Attach it to a scene node with ISceneNode::addAnimator()
		and the animator will animate it. */
	virtual ISceneNodeAnimator* createFlyCircleAnimator(
		const core::vector3df& center=core::vector3df(0.f, 0.f, 0.f),
		f32 radius=100.f, f32 speed=0.001f,
		const core::vector3df& direction=core::vector3df(0.f, 1.f, 0.f)
	);

	//! Creates a fly straight animator, which lets the attached scene node
	//! fly or move along a line between two points.
	virtual ISceneNodeAnimator* createFlyStraightAnimator(
		const core::vector3df& startPoint,
		const core::vector3df& endPoint, u32 timeForWay, bool loop=false
	);

	//! Creates a texture animator, which switches the textures of the target scene
	//! node based on a list of textures.
	virtual ISceneNodeAnimator* createTextureAnimator(
		const core::array<video::ITexture*>& textures,
		s32 timePerFrame, bool loop
	);

	//! Creates a scene node animator, which deletes the scene node after
	//! some time automaticly.
	virtual ISceneNodeAnimator* createDeleteAnimator(u32 timeMS);


	//! Creates a special scene node animator for doing automatic collision detection
	//! and response.
	virtual ISceneNodeAnimatorCollisionResponse* createCollisionResponseAnimator(
		ITriangleSelector* world, ISceneNode* sceneNode,
		const core::vector3df& ellipsoidRadius = core::vector3df(30,60,30),
		const core::vector3df& gravityPerSecond = core::vector3df(0,-1.0f,0),
		const core::vector3df& ellipsoidTranslation = core::vector3df(0,0,0),
		f32 slidingValue = 0.0005f
	);

	//! Creates a follow spline animator.
	virtual ISceneNodeAnimator* createFollowSplineAnimator(
		s32 startTime,
		const core::array< core::vector3df >& points,
		f32 speed = 1.0f,
		f32 tightness = 0.5f
	);

	//! Creates a simple ITriangleSelector, based on a mesh.
	virtual ITriangleSelector* createTriangleSelector(
		IMesh* mesh,
		ISceneNode* node
	);

	//! Creates a simple ITriangleSelector, based on a mesh.
	virtual ITriangleSelector* createOctTreeTriangleSelector(
		IMesh* mesh,
		ISceneNode* node, s32 minimalPolysPerNode
	);

	//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
	virtual ITriangleSelector* createTriangleSelectorFromBoundingBox(
		ISceneNode* node
	);

	//! Creates a meta triangle selector.
	virtual IMetaTriangleSelector* createMetaTriangleSelector();

	//! Creates a triangle selector which can select triangles from a terrain scene node
	//! \param: Pointer to the created terrain scene node
	//! \param: Level of detail, 0 for highest detail.
	virtual ITriangleSelector* createTerrainTriangleSelector(
		ITerrainSceneNode* node,
		s32 LOD=0
	);

	//! Adds an external mesh loader.
	virtual void addExternalMeshLoader(IMeshLoader* externalLoader);

	//! Returns a pointer to the scene collision manager.
	virtual ISceneCollisionManager* getSceneCollisionManager();

	//! Returns a pointer to the mesh manipulator.
	virtual IMeshManipulator* getMeshManipulator();

	//! Sets the color of stencil buffers shadows drawn by the scene manager.
	virtual void setShadowColor(video::SColor color);

	//! Returns the current color of shadows.
	virtual video::SColor getShadowColor() const;

	//! Adds a scene node to the deletion queue.
	virtual void addToDeletionQueue(ISceneNode* node);

	//! Returns the first scene node with the specified id.
	virtual ISceneNode* getSceneNodeFromId(s32 id, ISceneNode* start=0);

	//! Returns the first scene node with the specified type.
	virtual ISceneNode* getSceneNodeFromType(scene::ESCENE_NODE_TYPE type,
											 ISceneNode* start=0);

	//! returns scene nodes by type.
	virtual void getSceneNodesFromType(ESCENE_NODE_TYPE type,
									   core::array<scene::ISceneNode*>& outNodes,
									   ISceneNode* start=0);

	//! Posts an input event to the environment. Usually you do not have to
	//! use this method, it is used by the internal engine.
	virtual bool postEventFromUser(const SEvent& event);

	//! Clears the whole scene. All scene nodes are removed.
	virtual void clear();

	//! Removes all children of this scene node
	virtual void removeAll();

	//! Returns interface to the parameters set in this scene.
	virtual io::IAttributes* getParameters();

	//! Returns current render pass.
	virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass() const;

	//! Set current render pass. If you want call ISceneNode->render() directly to draw something,you can give this 
	// scene node a empty CSceneManager(by createNewSceneManager), and set its renderpass as you wish.
	// note: do not call drawall
	virtual E_SCENE_NODE_RENDER_PASS setSceneNodeRenderPass(E_SCENE_NODE_RENDER_PASS);

	//! Creates a new scene manager.
	virtual ISceneManager* createNewSceneManager(bool cloneContent);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return ESNT_SCENE_MANAGER; }

	//! Returns the default scene node factory which can create all built in scene nodes
	virtual ISceneNodeFactory* getDefaultSceneNodeFactory();

	//! Adds a scene node factory to the scene manager.
	/** Use this to extend the scene manager with new scene node types which it should be
		able to create automaticly, for example when loading data from xml files. */
	virtual void registerSceneNodeFactory(ISceneNodeFactory* factoryToAdd);

	//! Returns amount of registered scene node factories.
	virtual u32 getRegisteredSceneNodeFactoryCount() const;

	//! Returns a scene node factory by index
	virtual ISceneNodeFactory* getSceneNodeFactory(u32 index);

	//! Returns a typename from a scene node type or null if not found
	virtual const c8* getSceneNodeTypeName(ESCENE_NODE_TYPE type);

	//! Adds a scene node to the scene by name
	virtual ISceneNode* addSceneNode(const char* sceneNodeTypeName, ISceneNode* parent=0);

	//! Returns the default scene node animator factory which can create all built-in scene node animators
	virtual ISceneNodeAnimatorFactory* getDefaultSceneNodeAnimatorFactory();

	//! Adds a scene node animator factory to the scene manager.
	virtual void registerSceneNodeAnimatorFactory(ISceneNodeAnimatorFactory* factoryToAdd);

	//! Returns amount of registered scene node animator factories.
	virtual u32 getRegisteredSceneNodeAnimatorFactoryCount() const;

	//! Returns a scene node animator factory by index
	virtual ISceneNodeAnimatorFactory* getSceneNodeAnimatorFactory(u32 index);

	//! Saves the current scene into a file.
	//! \param filename: Name of the file .
	virtual bool saveScene(const c8* filename, ISceneUserDataSerializer* userDataSerializer=0);

	//! Saves the current scene into a file.
	virtual bool saveScene(io::IWriteFile* file, ISceneUserDataSerializer* userDataSerializer=0);

	//! Loads a scene. Note that the current scene is not cleared before.
	//! \param filename: Name of the file .
	virtual bool loadScene(const c8* filename, ISceneUserDataSerializer* userDataSerializer=0);

	//! Loads a scene. Note that the current scene is not cleared before.
	virtual bool loadScene(io::IReadFile* file, ISceneUserDataSerializer* userDataSerializer=0);

	//! Writes attributes of the scene node.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns a mesh writer implementation if available
	virtual IMeshWriter* createMeshWriter(EMESH_WRITER_TYPE type);

	//! Sets ambient color of the scene
	virtual void setAmbientLight(const video::SColorf &ambientColor);

	//! Returns ambient color of the scene
	virtual const video::SColorf& getAmbientLight() const;

	//! enable cull in scenemanager
	virtual void enableCulling();

	//! disable cull in scenemanager
	virtual void disableCulling();

	//! Indicates if culling is enabled
	virtual bool isCullingEnabled() const;

	//! Get the current scene time
	s32 getSceneTime() const { return (s32)SceneTimeMillis; } 

	//! Returns the node currently being rendered.
	/** This is set within drawAll() and can be used by callbacks to get
		information about the current node. */
	virtual ISceneNode* getCurrentRenderedNode(void** renderData = NULL,
											   int* renderLayer = NULL) const;

	//! Returns the node currently being rendered.
	/** This is set within drawAll() and can be used by callbacks to get
		information about the next node to render. */
	virtual ISceneNode* getNextRenderedNode(void** renderData = NULL,
											int* renderLayer = NULL) const;

	//! Returns whether the scene manager is registering the transparent
	//! pass as a solid pass (false by default).
	virtual bool isTransparentPassRegisteredAsSolid() const;

	//! Sets whether (true) or not (false) the transparent pass is register
	//! as solid for rendering.
	virtual void setRegisterTransparentPassAsSolid(bool value);

protected:

	virtual void onDelete();

	//! Returns a typename from a scene node animator type or null if not found
	virtual const c8* getAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type) const;

	//! returns if node is culled
	bool isCulled(const core::aabbox3d<f32> &tbox, scene::E_CULLING_TYPE cullingType);
	bool isCulled(const ISceneNode* node);

	//! clears the deletion list
	void clearDeletionList();

	//! writes a scene node
	void writeSceneNode(io::IXMLWriter* writer, ISceneNode* node, ISceneUserDataSerializer* userDataSerializer);

	//! reads a scene node
	void readSceneNode(io::IXMLReader* reader, ISceneNode* parent, ISceneUserDataSerializer* userDataSerializer);

	//! read materials
	void readMaterials(io::IXMLReader* reader, ISceneNode* node);

	//! reads animators of a node
	void readAnimators(io::IXMLReader* reader, ISceneNode* node);

	//! reads user data of a node
	void readUserData(io::IXMLReader* reader, ISceneNode* node, ISceneUserDataSerializer* userDataSerializer);

	//!
	template < typename Entry >
	void renderList(E_SCENE_NODE_RENDER_PASS pass, core::array<Entry>& list);

	struct SDefaultNodeEntry
	{
		SDefaultNodeEntry()
			: Node(NULL)
			, RenderData(NULL)
			, Material(NULL)
			, Layer(0)
		{
		}

		explicit SDefaultNodeEntry(ISceneNode* n,
								   const video::SMaterial* material = 0,
								   void* renderData = 0,
								   int layer = u32(-1) >> 1)
			: Node(n)
			, RenderData(renderData)
			, Material(material)
		{
			if(layer != u32(-1) >> 1)
			{
				Layer = layer;
			}
			else
			{
				Layer = Node->getRenderingLayer();
			}
		}

		int getLayer() const
		{
			return Layer;
		}

		bool operator < (const SDefaultNodeEntry& other) const
		{
			const video::SMaterial* const otherMat = other.Material;
			if (Layer > other.Layer)
			{
				return true;
			}
			else if (Layer == other.Layer)
			{
				if (Material && otherMat)
				{
					if (*Material == *otherMat)
					{
						return Node->getRenderVertexCount(RenderData) < other.Node->getRenderVertexCount(other.RenderData);
					}
					else
					{
						return *Material < *otherMat;
					}
				}
				else if (Material != otherMat)
				{
					return Material < otherMat;
				}
				return Node < other.Node;
			}
			return false;
		}

		ISceneNode* Node;
		void* RenderData;
		const video::SMaterial* Material;

	protected:

		int Layer;
	};


	struct STransparentNodeEntry : SDefaultNodeEntry
	{
		STransparentNodeEntry()
			: Distance(0.0f)
		{
		}

		STransparentNodeEntry(ISceneNode* n,
							  const core::vector3df& camera,
							  const video::SMaterial* material = NULL,
							  void* renderData = NULL,
							  const core::vector3df* absPos = NULL,
							  int layer = u32(-1) >> 1)
			: SDefaultNodeEntry(n, material, renderData, layer)
		{
			Distance = ((absPos
						 ? absPos->getDistanceFromSQ(camera)
						 : f32(Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(camera)))
						+ Node->getCameraOffset());
		}

		bool operator < (const STransparentNodeEntry& other) const
		{
			const video::SMaterial* const otherMat = other.Material;
			if (Layer > other.Layer)
			{
				return true;
			}
			else if (Layer == other.Layer)
			{
				if (Distance > other.Distance)
				{
					return true;
				}
				if (Distance == other.Distance)
				{
					if (Material && otherMat)
					{
						if (*Material == *otherMat)
						{
							return Node->getRenderVertexCount(RenderData) < other.Node->getRenderVertexCount(other.RenderData);
						}
						else
						{
							return *Material < *otherMat;
						}
					}
					else if (Material != otherMat)
					{
						return Material < otherMat;
					}
					return Node < other.Node;
				}
			}
			return false;
		}

	protected:

		f32 Distance;
		
	};

	//! sort on distance (sphere) to camera
	struct SDistanceNodeEntry
	{
		explicit SDistanceNodeEntry(ISceneNode* n = 0,
									f64 d = 0.0,
									void* renderData = 0)
			: Node(n)
			, Distance(d)
			, RenderData(renderData)
		{
		}

		SDistanceNodeEntry(ISceneNode* n, const core::vector3df& cameraPos, void* renderData = 0)
			: Node(n)
			, RenderData(renderData)
		{
			Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(cameraPos);
			Distance -= Node->getBoundingBox().getExtent().getLengthSQ() * 0.5;
		}

		int getLayer() const
		{
			return 0;
		}

		bool operator < (const SDistanceNodeEntry& other) const
		{
			return Distance < other.Distance;
		}

		ISceneNode* Node;
		void* RenderData;

	private:

		f64 Distance;
	};

	struct SUnsortedNodeEntry
	{
		explicit SUnsortedNodeEntry(ISceneNode* node = 0,
									void* renderData = 0)
			: Node(node)
			, RenderData(renderData)
		{
		}

		int getLayer() const
		{
			return 0;
		}

		ISceneNode* Node;
		void* RenderData;
	};

	//! video driver
	video::IVideoDriver* Driver;

	//! file system
	io::IFileSystem* FileSystem;

	//! GUI Enviroment ( Debug Purpose )
	gui::IGUIEnvironment* GUIEnvironment;

	//! cursor control
	gui::ICursorControl* CursorControl;

	//! collision manager
	ISceneCollisionManager* CollisionManager;

	//! render pass lists
	core::array<SUnsortedNodeEntry> CameraList;
	core::array<SDistanceNodeEntry> LightList;
	core::array<SUnsortedNodeEntry> ShadowNodeList;
	core::array<SUnsortedNodeEntry> SkyBoxList;
	core::array<SDefaultNodeEntry> SolidNodeList;
	core::array<STransparentNodeEntry> TransparentNodeList;

	struct SRenderedNodeInfo
	{
		//! Current node being rendered.
		ISceneNode* CurrentRenderedNode;

		//! Render data of current rendered node.
		void* CurrentRenderedNodeRenderData;

		//! Current rendering layer.
		int CurrentRenderLayer;

		//! Next node to render after current.
		ISceneNode* NextRenderedNode;

		//! Render data of next rendered node.
		void* NextRenderedNodeRenderData;

		//! Current rendering layer.
		int NextRenderLayer;

		SRenderedNodeInfo()
			: CurrentRenderedNode(NULL)
			, CurrentRenderedNodeRenderData(NULL)
			, CurrentRenderLayer(0)
			, NextRenderedNode(NULL)
			, NextRenderedNodeRenderData(NULL)
			, NextRenderLayer(0)
		{
		}

		void setNext(ISceneNode* node,
					 void* renderData = NULL,
					 int layer = 0)
		{
			CurrentRenderedNode = NextRenderedNode;
			CurrentRenderedNodeRenderData = NextRenderedNodeRenderData;
			CurrentRenderLayer = NextRenderLayer;
			NextRenderedNode = node;
			NextRenderedNodeRenderData = renderData;
			NextRenderLayer = layer;
		}

		template < typename Entry >
		void setNext(const Entry& entry)
		{
			setNext(entry.Node, entry.RenderData, entry.getLayer());
		}
	};

	SRenderedNodeInfo RenderedNodeInfo;

	core::array<IMeshLoader*> MeshLoaderList;
	core::array<ISceneNode*> DeletionList;
	core::array<ISceneNodeFactory*> SceneNodeFactoryList;
	core::array<ISceneNodeAnimatorFactory*> SceneNodeAnimatorFactoryList;

	//! current active camera
	ICameraSceneNode* ActiveCamera;
	core::vector3df CamWorldPos; // Position of camera for transparent nodes.

	video::SColor ShadowColor;
	video::SColorf AmbientLight;

	//! String parameters
	io::CAttributes Parameters;

	//! Mesh cache
	IMeshCache* MeshCache;

	E_SCENE_NODE_RENDER_PASS CurrentRendertime;

	//! constants for reading and writing XML.
	//! Not made static due to portability problems.
	const core::stringw IRR_XML_FORMAT_SCENE;
	const core::stringw IRR_XML_FORMAT_NODE;
	const core::stringw IRR_XML_FORMAT_NODE_ATTR_TYPE;
	bool  CullFlag;
	//! Scene virtual time
	float SceneTimeMillis;

	// Traversal
	core::array<ISceneNode*> AllNodeList;
	core::array<ISceneNode*> VisibleNodeList;
	core::array<ISceneNode*> RenderableNodeList;
	core::array<ISceneNode*> AnimatorNodeList;
	bool HierarchyChanged;

	//! Transparent pass registration behaviour
	bool IsTransparentPassRegisteredAsSolid;
};

template < typename Entry >
inline
void
CSceneManager::renderList(E_SCENE_NODE_RENDER_PASS pass,
						  core::array<Entry>& list)
{
	CurrentRendertime = pass;

	u32 cnt = list.size();

	// Add a dummy at end
	list.push_back(Entry());

	RenderedNodeInfo.setNext(list[0]);
	for (u32 i = 0; i < cnt; )
	{
		++i;
		RenderedNodeInfo.setNext(list[i]);
		RenderedNodeInfo.CurrentRenderedNode->render(
			RenderedNodeInfo.CurrentRenderedNodeRenderData
		);
	}
	// clear current
	RenderedNodeInfo.setNext(list.getLast());

	list.set_used(0);
}

} // end namespace video
} // end namespace scene

#endif



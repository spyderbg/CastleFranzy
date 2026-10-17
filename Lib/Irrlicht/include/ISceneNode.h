//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_H_INCLUDED__
#define __I_SCENE_NODE_H_INCLUDED__

#include "IAttributeExchangingObject.h"
#include "ESceneNodeTypes.h"
#include "ESceneNodeFlags.h"
#include "ECullingTypes.h"
#include "EDebugSceneTypes.h"
#include "ISceneNodeAnimator.h"
#include "ITriangleSelector.h"
#include "SMaterial.h"
#include "irrString.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "irrList.h"
#include "IAttributes.h"
#include "irros.h"

namespace irr
{
namespace scene
{
	class ISceneManager;

	//! draw options for ISceneNode
	enum E_OPTION_FLAG
	{
		EOF_DISABLE_ONANIMATE = 0x00000001,
		
	};

	//! Names for culling type
	const c8* const OptionFlagNames[] =
	{
		"disableOnAnimate",
		0
	};

	//! Scene node interface.
	/** A scene node is a node in the hierarchical scene graph. Every scene
	node may have children, which are also scene nodes. Children move
	relative to their parent's position. If the parent of a node is not
	visible, its children won't be visible either. In this way, it is for
	example easily possible to attach a light to a moving car, or to place
	a walking character on a moving platform on a moving ship.
	*/
	class ISceneNode : virtual public io::IAttributeExchangingObject
	{
	public:

#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
//#    define DEBUG_ISCENE_NODE

#    ifdef DEBUG_ISCENE_NODE
		static int NodeCount;
		static int AnimatedCount;
		static int AbsoluteUpdate;
		static int RelativeUpdate;
		static int RelativeRotationUpdate;
		static int RelativeTranslationUpdate;;

		static void ResetXFormDebugCounters();
#    endif
#endif

		//! Constructor
		IRRLICHT_API ISceneNode(s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::quaternion& rotation = core::quaternion(0,0,0,1.0f),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! Constructor
		IRRLICHT_API ISceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id=-1,
				const core::vector3df& position = core::vector3df(0,0,0),
				const core::quaternion& rotation = core::quaternion(0,0,0,1.0f),
				const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));


		//! Destructor
		IRRLICHT_API virtual ~ISceneNode();

		//!
		IRRLICHT_API void SetOption(E_OPTION_FLAG opt, bool value);
		//!
		IRRLICHT_API bool GetOption(E_OPTION_FLAG opt) const;
		IRRLICHT_API void SetNodeFlags(s32 flag);
		//!
		IRRLICHT_API void LogTree(int tabs = 0);

		//! This method is called just before the rendering process of the whole scene.
		/** Nodes may register themselves in the render pipeline during this call,
		precalculate the geometry which should be renderered, and prevent their
		children from being able to register themselves if they are clipped by simply
		not calling their OnRegisterSceneNode method.
		If you are implementing your own scene node, you should overwrite this method
		with an implementation code looking like this:
		\code
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
		\endcode
		*/
		IRRLICHT_API virtual void OnRegisterSceneNode();

		//! OnAnimate() is called just before rendering the whole scene.
		/** Nodes may calculate or store animations here, and may do other useful things,
		depending on what they are. Also, OnAnimate() should be called for all
		child scene nodes here. This method will be called once per frame, independent
		of whether the scene node is visible or not.
		\param timeMs Current time in milliseconds. */
		IRRLICHT_API virtual void OnAnimate(u32 timeMs);

		//! OnUpdateTime()
		/** This method can be used to force the internal times for each animator to be updated 
		without updating the animations.
		\param timeMs Current time in milliseconds. */
		IRRLICHT_API virtual void OnUpdateTime(u32 timeMs);

		//! Renders the node.
		IRRLICHT_API virtual void render(void* renderData = 0) = 0;

		//! Get info ralated to the render data
		IRRLICHT_API virtual int getRenderVertexCount(void* renderData) { return -1; }

		//! Returns the name of the node.
		/** \return Name as character string. */
		IRRLICHT_API virtual const c8* getName() const;

		//! Sets the name of the node.
		/** \param name New name of the scene node. */
		IRRLICHT_API virtual void setName(const c8* name);

		//! Sets the name of the node.
		/** \param name New name of the scene node. */
		IRRLICHT_API virtual void setName(const core::stringc& name);

		//! Get the axis aligned, not transformed bounding box of this node.
		/** This means that if this node is an animated 3d character,
		moving in a room, the bounding box will always be around the
		origin. To get the box in real world coordinates, just
		transform it with the matrix you receive with
		getAbsoluteTransformation() or simply use
		getTransformedBoundingBox(), which does the same.
		\return The non-transformed bounding box. */
		IRRLICHT_API virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

		//! Get the axis aligned, transformed and animated absolute bounding box of this node.
		/** \return The transformed bounding box. */
#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
		IRRLICHT_API virtual const core::aabbox3d<f32>& getTransformedBoundingBox() const;
#else
		IRRLICHT_API virtual const core::aabbox3d<f32> getTransformedBoundingBox() const;
#endif

		//! Get the absolute transformation of the node. Is recalculated every OnAnimate()-call.
		//! \return The absolute transformation matrix.
		IRRLICHT_API virtual const core::matrix4& getAbsoluteTransformation() const;

		//! Set the absolute transformation of the node.
		IRRLICHT_API virtual void setAbsoluteTransformation(core::matrix4& mat);

		//! Returns the relative transformation of the scene node.
		/** The relative transformation is stored internally as 3
		vectors: translation, rotation and scale. To get the relative
		transformation matrix, it is calculated from these values.
		\return The relative transformation matrix. */
		IRRLICHT_API virtual const core::matrix4& getRelativeTransformation() const;

		IRRLICHT_API virtual void setRelativeTransformation(const core::matrix4 &mat);

		//! Returns true if the node is visible.
		/** This is only an option set by the user, but has nothing to
		do with geometry culling
		\return The visibility of the node, true means visible. */
		IRRLICHT_API virtual bool isVisible() const;

		//! Returns a reference to te visibility variable
		/** This is only an option set by the user, but has nothing to
		do with geometry culling
		\return The visibility of the node, true means visible. */
		IRRLICHT_API virtual int &getVisibility();
		
		//! Sets if the node should be visible or not.
		/** All children of this node won't be visible either, when set
		to false.
		\param isVisible If the node shall be visible. */
		IRRLICHT_API virtual void setVisible(bool isVisible);

#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		//! Gets node flag value for animation.
		IRRLICHT_API virtual bool isAnimated(ESCENE_NODE_ANIMATION_FLAG what) const;

		//! Sets node flags for animation.
		/** \internal Meant for internal use when binding animations.
			\todo We might want to allow groups of flag to be set. */
		IRRLICHT_API virtual void setIsAnimated(ESCENE_NODE_ANIMATION_FLAG what, bool value);
#endif

		//! Get the id of the scene node.
		/** This id can be used to identify the node.
		\return The id. */
		IRRLICHT_API virtual s32 getID() const;

		//! Sets the id of the scene node.
		/** This id can be used to identify the node.
		\param id The new id. */
		IRRLICHT_API virtual void setID(s32 id);
		//! Returns the first scene node with the specified uid.
		IRRLICHT_API ISceneNode* getSceneNodeFromUID(const char* uid);
		//! Returns the first scene node with the specified name.
		IRRLICHT_API const ISceneNode* getSceneNodeFromUID(const char* uid) const;

		//! Returns the first scene node with the specified type.
		IRRLICHT_API ISceneNode* getSceneNodeFromType(scene::ESCENE_NODE_TYPE type);

		//! Get scene nodes by type.
		/** \param type: Type of scene node to find (ESNT_ANY will return all child nodes).
		\param outNodes: array to be filled with results. */
		IRRLICHT_API void getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes);

		//! Get the uid of the scene node.
		/** This uid can be used to identify the node characteristics.
		\return The uid. */
		IRRLICHT_API virtual const char * getUID() const;

		/** This id can be used to identify the node characteristics.
		\return The scope id. */
		IRRLICHT_API virtual const char * getScopeID() const;

		//! Adds a child to this scene node.
		/** If the scene node already has a parent it is first removed
		from the other parent.
		\param child A pointer to the new child. */
		IRRLICHT_API virtual void addChild(ISceneNode* child);

		//! Removes a child from this scene node.
		/** \param child A pointer to the new child.
		\return True if the child was removed, and false if not,
		e.g. because it couldn't be found in the children list. */
		IRRLICHT_API virtual bool removeChild(ISceneNode* child);

		//! Removes all children of this scene node
		IRRLICHT_API virtual void removeAll();
		//! Removes this scene node from the scene, deleting it.
		IRRLICHT_API virtual void remove();

		//! Adds an animator which should animate this node.
		/** \param animator A pointer to the new animator. */
		IRRLICHT_API virtual void addAnimator(ISceneNodeAnimator* animator);

		//! Get a list of all scene node animators.
		/** \return The list of animators attached to this node. */
		IRRLICHT_API const core::list<ISceneNodeAnimator*>& getAnimators() const;

		//! Removes an animator from this scene node.
		/** \param animator A pointer to the animator to be deleted. */
		IRRLICHT_API virtual void removeAnimator(ISceneNodeAnimator* animator);

		//! Removes all animators from this scene node.
		IRRLICHT_API virtual void removeAnimators();

		//! Adds an animator which should animate this node.
		/** \param animator A pointer to the new animator. */
		IRRLICHT_API virtual void addBindedAnimator(ISceneNodeAnimator* animator);

		//! Get a list of all scene node animators.
		/** \return The list of animators attached to this node. */
		IRRLICHT_API const core::list<ISceneNodeAnimator*>& getBindedAnimators() const;

		//! Removes an animator from this scene node.
		/** \param animator A pointer to the animator to be deleted. */
		IRRLICHT_API virtual void removeBindedAnimator(ISceneNodeAnimator* animator);

		//! Removes all animators from this scene node.
		IRRLICHT_API virtual void removeBindedAnimators();

		//! Returns the material based on the zero based index i.
		/** To get the amount of materials used by this scene node, use
		getMaterialCount(). This function is needed for inserting the
		node into the scene hierarchy at an optimal position for
		minimizing renderstate changes, but can also be used to
		directly modify the material of a scene node.
		\param num Zero based index. The maximal value is getMaterialCount() - 1.
		\return The material at that index. */
		IRRLICHT_API virtual video::SMaterial& getMaterial(u32 num);
		//! Get amount of materials used by this scene node.
		/** \return Current amount of materials of this scene node. */
		IRRLICHT_API virtual u32 getMaterialCount() const;

		//! Sets all material flags at once to a new value.
		/** Useful, for example, if you want the whole mesh to be
		affected by light.
		\param flag Which flag of all materials to be set.
		\param newvalue New value of that flag. */
		IRRLICHT_API void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);

		//! Sets the texture of the specified layer in all materials of this scene node to the new texture.
		/** \param textureLayer Layer of texture to be set. Must be a
		value smaller than MATERIAL_MAX_TEXTURES.
		\param texture New texture to be used. */
		IRRLICHT_API void setMaterialTexture(u32 textureLayer, video::ITexture* texture);

		//! Sets the material type of all materials in this scene node to a new material type.
		/** \param newType New type of material to be set. */
		IRRLICHT_API void setMaterialType(video::E_MATERIAL_TYPE newType);

		//! Notify the scene node that one of his material type has been changed.
		IRRLICHT_API virtual void notifyMaterialChanged() {};

		//! Gets the relative scale of the scene node.
		/** \return The scale of the scene node. */
		IRRLICHT_API virtual const core::vector3df& getScale() const;

		//! Sets the relative scale of the scene node.
		/** \param scale New scale of the node */
		IRRLICHT_API virtual void setScale(const core::vector3df& scale);

		//! Gets the rotation of the node.
		/** Note that this is the relative rotation of the node.
		\return Current relative rotation of the scene node. */
		IRRLICHT_API virtual const core::quaternion& getRotation() const;


		//! Sets the rotation of the node.
		/** This only modifies the relative rotation of the node.
		\param rotation New rotation of the node in degrees. */
		IRRLICHT_API virtual void setRotation(const core::quaternion& rotation);

		//! Gets the position of the node.
		/** Note that the position is relative to the parent.
		\return The current position of the node relative to the parent. */
		IRRLICHT_API virtual const core::vector3df& getPosition() const;


		//! Sets the position of the node.
		/** Note that the position is relative to the parent.
		\param newpos New relative postition of the scene node. */
		IRRLICHT_API virtual void setPosition(const core::vector3df& newpos);
		
		void setPosition(f32 x, f32 y, f32 z);

		//! Gets the abolute position of the node.
		/** \return The current absolute position of the scene node. */
		IRRLICHT_API virtual core::vector3df getAbsolutePosition() const;

		//! Enables or disables automatic culling based on the bounding box.
		/** Automatic culling is enabled by default. Note that not
		all SceneNodes support culling and that some nodes always cull
		their geometry because it is their only reason for existence,
		for example the OctreeSceneNode.
		\param state The culling state to be used. */
		IRRLICHT_API void setAutomaticCulling( E_CULLING_TYPE state);

		//! Gets the automatic culling state.
		/** \return The automatic culling state. */
		E_CULLING_TYPE getAutomaticCulling() const;

		//! Sets if debug data like bounding boxes should be drawn.
		/** A bitwise OR of the types is supported.
		Please note that not all scene nodes support this feature.
		\param state The debug data visibility state to be used. */
		IRRLICHT_API virtual void setDebugDataVisible(s32 state);
		//! Returns if debug data like bounding boxes are drawn.
		/** \return A bitwise OR of the debug data values currently visible. */
		IRRLICHT_API s32 isDebugDataVisible() const;
		//! Sets if this scene node is a debug object.
		/** Debug objects have some special properties, for example they can be easily
		excluded from collision detection or from serialization, etc. */
		IRRLICHT_API void setIsDebugObject(bool debugObject);

		//! Returns if this scene node is a debug object.
		/** Debug objects have some special properties, for example they can be easily
		excluded from collision detection or from serialization, etc.
		\return If this node is a debug object, true is returned. */
		IRRLICHT_API bool isDebugObject() const;

		//! Returns a const reference to the list of all children.
		/** \return The list of all children of this node. */
		IRRLICHT_API const core::list<ISceneNode*>& getChildren() const;

		//! Returns the first scene node with the specified name.
		IRRLICHT_API ISceneNode* getSceneNodeFromName(const char* name);

		//! Returns the first scene node with the specified name.
		IRRLICHT_API const ISceneNode* getSceneNodeFromName(const char* name) const;

		//! Returns the first scene node with the specified name.
		IRRLICHT_API ISceneNode* getSceneNodeFromScopeID(const char* scopeID);

		//! Returns the first scene node with the specified name.
		IRRLICHT_API const ISceneNode* getSceneNodeFromScopeID(const char* scopeID) const;

		IRRLICHT_API virtual const void * getUserProperty() const { return 0; }

		IRRLICHT_API virtual const void * getUserPropertyStr() const { return 0; }
		//! Returns the triangle selector attached to this scene node.
		/** The Selector can be used by the engine for doing collision
		detection. You can create a TriangleSelector with
		ISceneManager::createTriangleSelector() or
		ISceneManager::createOctTreeTriangleSelector and set it with
		ISceneNode::setTriangleSelector(). If a scene node got no triangle
		selector, but collision tests should be done with it, a triangle
		selector is created using the bounding box of the scene node.
		\return A pointer to the TriangleSelector or 0, if there
		is none. */
		IRRLICHT_API virtual ITriangleSelector* getTriangleSelector() const;

		//! Sets the triangle selector of the scene node.
		/** The Selector can be used by the engine for doing collision
		detection. You can create a TriangleSelector with
		ISceneManager::createTriangleSelector() or
		ISceneManager::createOctTreeTriangleSelector(). Some nodes may
		create their own selector by default, so it would be good to
		check if there is already a selector in this node by calling
		ISceneNode::getTriangleSelector().
		\param selector New triangle selector for this scene node. */
		IRRLICHT_API virtual void setTriangleSelector(ITriangleSelector* selector);

		//! Updates the absolute position based on the relative and the parents position
		/** 
		\param updateChildren Propagate the call to all his children
		*/
		IRRLICHT_API virtual void updateAbsolutePosition(bool updateChildren = false);
		//! Returns the parent of this scene node
		/** \return A pointer to the parent. */
		IRRLICHT_API scene::ISceneNode* getParent() const;
 
		//! Returns type of the scene node
		/** \return The type of this node. */
		IRRLICHT_API virtual ESCENE_NODE_TYPE getType() const;


		//! Writes attributes of the scene node.
		/** Implement this to expose the attributes of your scene node
		for scripting languages, editors, debuggers or xml
		serialization purposes.
		\param out The attribute container to write into.
		\param options Additional options which might influence the
		serialization. */
		IRRLICHT_API virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;


		//! Reads attributes of the scene node.
		/** Implement this to set the attributes of your scene node for
		scripting languages, editors, debuggers or xml deserialization
		purposes.
		\param in The attribute container to read from.
		\param options Additional options which might influence the
		deserialization. */
		IRRLICHT_API virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
		//! Creates a clone of this scene node and its children.
		/** \param newParent An optional new parent.
		\param newManager An optional new scene manager.
		\return The newly created clone of this node. */
		IRRLICHT_API virtual ISceneNode* clone();

		//! Sets the GameData
		IRRLICHT_API virtual void setGameData(void* data);
		//! Returns the GameData
		IRRLICHT_API virtual void* getGameData() const;

		//! Set the offset from camera useful for transparent materials
		//! when you want one node to be always drawn before the others
		IRRLICHT_API virtual void setCameraOffset(f32 cameraOffset);

		//! Get the offset from camera useful for transparent materials
		//! when you want one node to be always drawn before the others
		IRRLICHT_API virtual f32 getCameraOffset() const;

		//! Set a rendering layer useful for transparent materials
		//! when you want one node to be always drawn before the others
		IRRLICHT_API virtual void setRenderingLayer(int renderingLayer);

		//! Get the rendering layer useful for transparent materials
		//! when you want one node to be always drawn before the others
		IRRLICHT_API virtual int getRenderingLayer() const;

		IRRLICHT_API virtual void onMaterialChanged();

		//! Reset the local transformation to it initial state
		IRRLICHT_API virtual void resetTransform(bool resetChildren);

	protected:

		virtual void onDelete();

		//! Changes the parent of the scene node.
		/** \param newParent The new parent to be used. */
		IRRLICHT_API void setParent(ISceneNode* newParent);

		void setSceneManager(scene::ISceneManager *pSceneManager)
		{
			SceneManager = pSceneManager;
			core::list<ISceneNode*>::Iterator childIt = Children.begin();
			for(; childIt != Children.end(); childIt++)
			{
				(*childIt)->setSceneManager(pSceneManager);
			}

			onChangedSceneManager();
		}

		//! Sets if the node should be visible or not.
		/** Notify all children 
		\param isVisible If the node shall be visible. */
		virtual void notifyVisibilityChanged(bool visibility);

		virtual void onChangedSceneManager()
		{
			// default implementation do nothing
		}

		//! A clone function for the ISceneNode members.
		/** This method can be used by clone() implementations of
		derived classes
		\param toCopyFrom The node from which the values are copied
		\param newManager The new scene manager. */
		IRRLICHT_API void cloneMembers(ISceneNode* toCopyFrom);

		//! Name of the scene node.
		core::stringc Name;

		//! Absolute transformation of the node.
		core::matrix4 AbsoluteTransformation;

		//! Relative transformation of the node.
		mutable core::matrix4 RelativeTransformation;

		//! Relative translation of the scene node.
		/** \todo RelativeTranslation could be directly packed into
		 * RelativeTransformation, but this requires more API changes. */
		core::vector3df RelativeTranslation;

		//! Relative rotation of the scene node.
		core::quaternion RelativeRotation;

		//! Relative scale of the scene node.
		core::vector3df RelativeScale;

#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
		mutable core::aabbox3d<f32> TransformedBBox;
#endif

		//! Pointer to the parent
		ISceneNode* Parent;

		//! List of all children of this node
		core::list<ISceneNode*> Children;

		//! List of all animator nodes
		core::list<ISceneNodeAnimator*> Animators;

		//! List of all binded animator
		// Binded animator drive this node but also may drive other nodes
		// This node MUST not own this animator
		core::list<ISceneNodeAnimator*> BindedAnimators;

		//! ID of the node.
		s32 ID;

		//! Pointer to the scene manager
		ISceneManager* SceneManager;

		//! Pointer to the triangle selector
		ITriangleSelector* TriangleSelector;

		//! Automatic culling state
		E_CULLING_TYPE AutomaticCullingState;

		//!Draw Options
		int OptionFlags;

		//! General optimization and other various flags
		mutable int NodeFlags;

		//! Is the node visible?
		int IsVisible;

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		int IsLocalyVisible;
		int IsGlobalyVisible;
#endif

		//! Flag if debug data should be drawn, such as Bounding Boxes.
		s32 DebugDataVisible;

		//! Can be set by game to extend the content of a ISceneNode
		void * GameData;

		//! Offset from camera to change the way the transparent nodes are sorted
		f32 CameraOffset;

		//! Layers to change the way the transparent nodes are sorted
		int RenderingLayer;
	};

} // end namespace scene
} // end namespace irr

#endif


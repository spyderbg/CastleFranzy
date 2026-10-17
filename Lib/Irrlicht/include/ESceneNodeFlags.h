//-*-c++-*-
#ifndef __E_SCENE_NODE_FLAGS_H_INCLUDED__
#define __E_SCENE_NODE_FLAGS_H_INCLUDED__

namespace irr
{
namespace scene
{

	//! An enumeration for all flag masks of scene nodes.
	/**
	   Enumaration values are either single-flag masks or flag groups.
	   \todo Change ISceneNode::IsVisible to be a flag also (more involved modifications).
	*/
	enum ESCENE_NODE_FLAG
	{
		// //! Indicates if a node is visible (not related to automatic culling).
		// ESNF_IS_VISIBLE = 1,

		//! Indicates the node has scale animation.
		ESNF_SCALE_ANIMATED = /*ESNF_VISIBLE <<*/ 1,

		//! Indicates the node has rotation animation.
		ESNF_ROTATION_ANIMATED = ESNF_SCALE_ANIMATED << 1,

		//! Indicates the node has translation animation.
		ESNF_TRANSLATION_ANIMATED = ESNF_ROTATION_ANIMATED << 1,

		//! Bit mask for transformation animated.
		ESNF_TRANSFORMATION_ANIMATED = (unsigned int)(ESNF_SCALE_ANIMATED | ESNF_ROTATION_ANIMATED | ESNF_TRANSLATION_ANIMATED),

		//! Indicates the node's scale has been modified in some way other than animation.
		ESNF_SCALE_CHANGED = ESNF_TRANSLATION_ANIMATED << 1,

		//! Indicates the node's rotation has been modified in some way other than animation.
		ESNF_ROTATION_CHANGED = ESNF_SCALE_CHANGED << 1,

		//! Indicates the node's translation has been modified in some way other than animation.
		ESNF_TRANSLATION_CHANGED = ESNF_ROTATION_CHANGED << 1,

		//! Bit mask for transformation change.
		ESNF_TRANSFORMATION_CHANGED = (unsigned int)(ESNF_SCALE_CHANGED | ESNF_ROTATION_CHANGED | ESNF_TRANSLATION_CHANGED),

		//! Bit mask for scale changes.
		ESNF_IS_SCALE_DIRTY = (unsigned int)(ESNF_SCALE_ANIMATED | ESNF_SCALE_CHANGED),

		//! Bit mask for rotation changes.
		ESNF_IS_ROTATION_DIRTY = (unsigned int)(ESNF_ROTATION_ANIMATED | ESNF_ROTATION_CHANGED),

		//! Bit mask for translation changes.
		ESNF_IS_TRANSLATION_DIRTY = (unsigned int)(ESNF_TRANSLATION_ANIMATED | ESNF_TRANSLATION_CHANGED),

		//! Bit mask for transformation dirtiness.
		ESNF_IS_TRANSFORMATION_DIRTY = (unsigned int)(ESNF_IS_SCALE_DIRTY | ESNF_IS_ROTATION_DIRTY | ESNF_IS_TRANSLATION_DIRTY),

		//! Flag set when transformation was updated (useful for traversals).
		ESNF_WAS_TRANSFORMATION_UPDATED = ESNF_TRANSLATION_CHANGED << 1,

		//! Indicates a change of the node's parent.
		ESNF_PARENT_CHANGED = ESNF_WAS_TRANSFORMATION_UPDATED << 1,

		//! Indicates the node a used for debugging only.
		ESNF_IS_DEBUG_OBJECT = ESNF_PARENT_CHANGED << 1,

		//! Indicates the transformed bounding box is not up to date
		ESNF_TRANSFORMED_BBOX_DIRTY = ESNF_IS_DEBUG_OBJECT << 1

	};

	//! An enumeration for animations of scene node transformation.
	/**
	   This enumaration is actually a subset of ESCENE_NODE_FLAG, and is meant
	   to make the API cleaner for animation flag setting in
	   ISceneNode::isAnimated and ISceneNode::setIsAnimated.
	*/
	enum ESCENE_NODE_ANIMATION_FLAG
	{
		//! Bit for scale animation.
		ESNAF_SCALE = ESNF_SCALE_ANIMATED,

		//! Bit for rotation animation.
		ESNAF_ROTATION = ESNF_ROTATION_ANIMATED,

		//! Bit for translation animation.
		ESNAF_TRANSLATION = ESNF_TRANSLATION_ANIMATED
	};

} // end namespace scene
} // end namespace irr


#endif

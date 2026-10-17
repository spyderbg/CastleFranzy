//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_ANIMATOR_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "vector3d.h"
#include "ESceneNodeAnimatorTypes.h"
#include "IAttributeExchangingObject.h"
#include "IEventReceiver.h"
#include "ITimelineController.h"

namespace irr
{
namespace io
{

class IAttributes;

} // end namespace io

namespace scene
{

class ISceneNode;
class ISceneManager;

//! Animates a scene node. Can animate position, rotation, material, and so on.
/** A scene node animator is able to animate a scene node in a very simple way. It may
	change its position, rotation, scale and/or material. There are lots of animators
	to choose from. You can create scene node animators with the ISceneManager interface.
*/
class ISceneNodeAnimator : public io::IAttributeExchangingObject, public IEventReceiver
{
public:

	ISceneNodeAnimator()
		: TimelineCtrl(0)
	{
	}

	//! Destructor
	virtual ~ISceneNodeAnimator();

	//! Animates a scene node.
	/** \param node Node to animate.
		\param timeMs Current time in milli seconds. */
	virtual void animateNode(ISceneNode* node, u32 timeMs) = 0;

	//! Update this animator time.
	/** \param timeMs Current time in milli seconds. */
	virtual void updateTime(u32 timeMs);

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling
		this. */
	virtual ISceneNodeAnimator* createClone() = 0;

	//! Returns true if this animator receives events.
	//! When attached to the an active camera, this animator will be able to respond to events
	//! such as mouse and keyboard events.
	virtual bool isEventReceiverEnabled() const;

	//! Event receiver, override this function for camera controlling animators
	virtual bool OnEvent(const SEvent& event);

	//! Returns type of the scene node animator
	virtual ESCENE_NODE_ANIMATOR_TYPE getType() const;

	//! Allow Animator to link with the node
	/** Default implementation : Do nothing */
	virtual void onBind(ISceneNode* node);

	//! Allow Animator to unlink with the node
	/** Default implementation : Do nothing */
	virtual void onUnbind(ISceneNode* node);

	//! Allow Animator to set a current clip by index
	virtual void setAnimationClip(int idx);

	//! Allow Animator to set a current clip by string id
	virtual void setAnimationClip(const char *animId);

	//! Get the animation index of a given string id
	virtual int getAnimationClipIndex(const char *animId);

	virtual void setTimelineCtrl(ITimelineController *timeCtrl);

	virtual const ITimelineController* getTimelineCtrl() const;
	virtual ITimelineController* getTimelineCtrl();
	virtual int getLength() const;

	//! Write the animation values on the target using the target as a temporary buffer.
	/* This function is usefull for blending.
	 */
	virtual void computeAnimationValues(u32 timeMs);

	//! Apply the animation values on the target using the target as an object and by calling
	//! the appropriate function.
	virtual void applyAnimationValues(u32 timeMs);

protected:

	ITimelineController* TimelineCtrl;
};

} // end namespace scene
} // end namespace irr

#endif


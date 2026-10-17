//-*-c++-*-
#ifndef __ICOLLADASCENENODEANIMATOR_H__
#define __ICOLLADASCENENODEANIMATOR_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "ISceneNodeAnimator.h"
#include "CColladaAnimationTrack.h"
#include "CColladaAnimationTrackWeight.h"
#include "CColladaRootSceneNode.h"
#include "CColladaTimelineController.h"
#include "CColladaEventsManager.h"

namespace irr
{
namespace collada
{

class ISceneNodeAnimator
	: public scene::ISceneNodeAnimator
{
public:

	ISceneNodeAnimator();
	virtual ~ISceneNodeAnimator();

	virtual void updateTime(u32 timeMs);

	virtual const SChannel& getAnimationTrack(int track) = 0;

	virtual const CAnimationTrackEx* getAnimationTrackEx(int track) = 0;

	virtual bool hasCompatibleTarget(const SChannel& model);
	virtual bool setCompatibleTarget(const SChannel& model, void* target);

	virtual void setTarget(const char* targetID, void* target);

	virtual void setTarget(int channel, void* target) = 0;

	virtual const char *getBindURI(int i) = 0;

	virtual int getTargetCount() = 0;

	virtual int getTargetSize(int i) = 0;

	virtual int getTargetsSize() = 0;

	//virtual void getTargetDefaultValue(const char *pTargetID, void *pValue) = 0;

	//! Allow Animator to link with the node
	virtual void onBind(scene::ISceneNode* node);

	//! Allow Animator to unlink with the node
	virtual void onUnbind(scene::ISceneNode* node);

	//! Allow Animator to unlink with the node -- the true unbind equivalent of onBind
	virtual void onUnbindAll()
	{
		forceUnbindAll();
	}

	//! animates a scene node
	virtual void animateNode(scene::ISceneNode* node, u32 timeMs) = 0;

	//! Writes attributes of the scene node animator.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node animator.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node animator
	//virtual ESCENE_NODE_ANIMATOR_TYPE getType() const { return ESNAT_TEXTURE; }
	
	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual scene::ISceneNodeAnimator* createClone();

	virtual int getLength() const;

	void forceBind();
	void forceUnbindAll();

	void forceUnbind(const char* uid);

	//! Allow an override of the default interpolation mode for all the track contained in the animator
	/** This is usefull when an animation has been sampled. 
	By passing it to IT_STEP, the lookup will be faster and the interpolation won't be computed.
	Keep in mind that this is only usefull as long as you keep your animation delta time close to 
	the one of your framerate. Otherwise, you will see artifact in your animation.
	*/
	enum E_INTERPOLATION_MODE
	{
		EIT_DEFAULT = 0,
		EIT_STEP = 1,
		EIT_LINEAR = 2,
	};

	void setInterpolationMode(E_INTERPOLATION_MODE interpolationType)
	{
		InterpolationType = interpolationType;
	}

	E_INTERPOLATION_MODE getInterpolationMode() const
	{
		return InterpolationType;
	}

	//! Set the events handler
	void setEventsCallback(FxEventTrackCallback callback, void* userData = 0) 
	{ 
		EventsCallback = callback;
		EventsUserData = userData;
		if(EventsManager)
		{
			EventsManager->setCallback(EventsCallback, EventsUserData); 
		}
	}

	//! Set the events track
	void setEventsTrack(const SEventsTrack* track)
	{
		if (EventsManager)
		{
			EventsManager->drop();
		}
		if (track)
		{
			EventsManager = irrnew CEventsManager(*track);
			EventsManager->setCallback(EventsCallback, EventsUserData);
		}
		else
		{
			EventsManager = 0;
		}
	}

	CEventsManager*	getEventsManager()						
	{ 
		return EventsManager; 
	}

	void setEventsManager(CEventsManager* eventsManager)	
	{
		if(EventsManager)
			EventsManager->drop();
		
		EventsManager = eventsManager;

		if(EventsManager)
			EventsManager->grab();
	};

protected:

	E_INTERPOLATION_MODE InterpolationType;
	scene::ISceneNode* BindTo;
	s32 Length;
	CEventsManager* EventsManager;
	FxEventTrackCallback EventsCallback;
	void* EventsUserData;
};

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __ICOLLADASCENENODEANIMATOR_H__

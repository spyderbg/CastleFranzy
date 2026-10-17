//-*-c++-*-
#ifndef __CCOLLADASCENENODEANIMATOR_H__
#define __CCOLLADASCENENODEANIMATOR_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSceneNodeAnimator.h"
#include "SDatabaseCollada.h"
#include "CColladaEventsManager.h"

namespace irr
{
namespace collada
{

class CSceneNodeAnimator
	: public collada::ISceneNodeAnimator
	, public IObject
{

protected:

	friend class CSceneNodeAnimatorBlender;

	virtual const CAnimationTrackEx* getAnimationTrackEx(int track);

	virtual const SChannel& getAnimationTrack(int track);

	virtual void setTarget(int channel, void* target);

	virtual const char* getBindURI(int i);

	virtual int getTargetCount();

	virtual int getTargetSize(int i);

	virtual int getTargetsSize();

public:

	float BlendOutWeight;

	CSceneNodeAnimator(const CColladaDatabase& database, SLibraryAnimationClips& animationClips);

	virtual ~CSceneNodeAnimator();

	virtual void updateTime(u32 timeMs);

	//! animates a scene node
	virtual void animateNode(scene::ISceneNode* node, u32 timeMs);
	
	virtual void applyAnimationValues(u32 timeMs);

	virtual void computeAnimationValues(u32 timeMs);

	//! Writes attributes of the scene node animator.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node animator.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
	
	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling
	this. */
	virtual scene::ISceneNodeAnimator* createClone();

	//! Add a new channel animator to the current set.
	void addAnimationTrack(CAnimationTrack* pAnimator);

	void removeAnimationTrack(CAnimationTrack* pAnimator);

	void removeAnimationTracks();

	//! Retrieve the animation start time in Milliseconds
	s32 getStart() const
	{
		return Start;
	}

	//! Retrieve the animation end time in Milliseconds
	s32 getEnd() const
	{
		return End;
	}

private:

	bool IsSampled;
	s32 Start;
	s32 End;
	SLibraryAnimationClips& AnimationClips;

	struct SBinding
	{
		SBinding()
			: Hint(0)
			, Target(0)
			, Track(0)
		{
		}

		CAnimationTrack* Track;
		void*			 Target;
		int				 Hint;
	};

	core::array<SBinding> ChannelAnimations;

	CEventsManager* EventsManager;
};

} // namespace collada
} // namespace irr


#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
#endif

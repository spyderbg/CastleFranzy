//-*-c++-*-
#ifndef __CCOLLADASCENENODEANIMATORSET_H__
#define __CCOLLADASCENENODEANIMATORSET_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSceneNodeAnimator.h"
#include "CColladaAnimationTrack.h"
#include "irrArray.h"

namespace irr
{
namespace collada
{

class CAnimationSet
	: public virtual IReferenceCounted
{
public:
	CAnimationSet()
		: MismatchBehavior(EMB_TRIM_ANIMATION)
	{
	}

	friend class CSceneNodeAnimatorSet;

	struct SBinding
	{
		SBinding()
			: Type(EBT_NO_VALUE)
			, DefaultValue(0)
			, Animation(0)
		{}
		enum E_BINDING_TYPE
		{
			EBT_NO_VALUE,
			EBT_DEFAULT_VALUE,
			EBT_ANIMATION_VALUE
		};

		E_BINDING_TYPE Type;
		void* DefaultValue;
		const SAnimation* Animation;
	};

	enum E_MISMATCH_BEHAVIOR
	{
		EMB_TRIM_ANIMATION = 0,
		EMB_KEEP_ANIMATION = 1,
	};

protected:
	E_MISMATCH_BEHAVIOR						MismatchBehavior;
	core::array<const SChannel*>			Channels;
	core::array<const CAnimationTrackEx*>	AnimatorEx;
	core::array<CColladaDatabase>			AnimationSet;
	core::array<SBinding>					Binding;
	u32										AnimatedTrackCount;

	virtual int addAnimation(const SAnimation* animation);
	virtual int	remAnimation(const SAnimation* animation);

	virtual SChannel& getChannel(u32 index);
	virtual const SChannel& getChannel(u32 index) const;


public:
	

	virtual void setMismatchBehavior(E_MISMATCH_BEHAVIOR behavior) 
	{
		MismatchBehavior = behavior;
	};

	virtual const SChannel& getAnimationTrack(int track) const;

	virtual const CAnimationTrackEx* getAnimationTrackEx(int track);

	virtual int addAnimationLibrary(const CColladaDatabase& database);

	virtual void remAnimationLibrary(CColladaDatabase& database);
	virtual void remAnimationLibrary(int index);

	int getAnimationCount() const;

	int getAnimatedTrackCount() const;

	int getAnimationLength(int i) const;

	int getAnimationStart(int i) const;

	int getAnimationEnd(int i) const;

	const CColladaDatabase& getDatabase(int i);

	const SBinding& getBinding(int i) const;

	virtual void compile();
};

class CSceneNodeAnimatorSet
	: public ISceneNodeAnimator
{
protected:

	CAnimationSet*							AnimationSet;
	core::array<void*>						Targets;
	core::array<int>						Hints;
	int										AnimationStart;
	int										CurrentAnim;

	int getAnimationLength(int i);
	int getAnimationStart(int i);
	int getAnimationEnd(int i);

public:

	CSceneNodeAnimatorSet(CAnimationSet* animationSet);

	virtual ~CSceneNodeAnimatorSet();

	void setCurrentAnimation(int i);
	int getCurrentAnimation() const;

	const char* getAnimationVisualSceneID(int i) const;

	int getAnimationCount() const;

	virtual const SChannel& getAnimationTrack(int track);

	virtual const CAnimationTrackEx* getAnimationTrackEx(int track);

	virtual const char* getBindURI(int i);

	virtual int getTargetCount();

	virtual int getTargetSize(int i);

	virtual int getTargetsSize();

	virtual void setTarget(int channel, void* target);

	//! animates a scene node
	virtual void animateNode(scene::ISceneNode* node, u32 timeMs);

	virtual void computeAnimationValues(u32 timeMs);

	virtual void applyAnimationValues(u32 timeMs);
};

}
}

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif

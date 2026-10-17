#ifndef __SANIMATIONCLIP_H__
#define __SANIMATIONCLIP_H__

namespace irr
{
namespace scene
{

//! CAnimationClip 
/** Store information loaded from a Collada File <animation_clip>
	This structure store the range of a given animation named Id
	and a set of animation references.
*/
class CAnimationClip
{
public:
	//! Retrieve the clip Id
	inline const core::stringc &GetId() const { return m_Id; }

	//! Retrieve the clip start time in Ms
	inline s32 GetStartTime() const { return m_StartMs; }

	//! Retrieve the clip end time in Ms
	inline s32 GetEndTime() const { return m_EndMs; }

	//! Retrieve the clip Length in Ms
	inline s32 GetLength() const { return m_EndMs - m_StartMs; }


protected:
	//! Clip Id
	core::stringc m_Id;

	//! Clip start time
	s32			  m_StartMs;

	//! Clip end time
	s32			  m_EndMs;

	//! Animations tracks
	core::vector<IAnimationTrack *> m_AnimationTracks;
};

};
};

#endif
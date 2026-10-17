#ifndef __CANIMATIONTRACKLIBRARY_H__
#define __CANIMATIONTRACKLIBRARY_H__

namespace irr
{
namespace scene
{

class CAnimationTrackLibrary
{
public:
	s32 GetSize() { return m_AnimationTracks.size(); }
	IAnimationTrack * GetAnimation(const char *Id) {}
	IAnimationTrack * operator[](int index) { return m_AnimationTracks[index]; }

protected:
	core::array<IAnimationTrack *> m_AnimationTracks;
};

};
};

#endif
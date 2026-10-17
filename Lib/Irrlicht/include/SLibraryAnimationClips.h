#ifndef __COLLADA_SLIBRARYANIMATIONCLIPS_H__
#define __COLLADA_SLIBRARYANIMATIONCLIPS_H__

#include "irrArray.h"
#include <irrString.h>

struct SAnimationClip
{
	const char *	id;
	int				start;	// Start in ms
	int				end;	// End in ms
	// Should store animation reference but we assume that every animation are referenced
};

struct SLibraryAnimationClips
{
	int				iClips;
	SAnimationClip *pClips;
};

struct CAnimationClip
{
	irr::core::stringc	id;
	int				start;	// Start in ms
	int				end;	// End in ms
};

// Temporary used instead of SLibraryAnimationClips
class CLibraryAnimationClips 
	: virtual public irr::IReferenceCounted
{
public:
	irr::core::array<CAnimationClip> clips;
};

#endif // __COLLADA_SLIBRARYANIMATIONCLIPS_H__

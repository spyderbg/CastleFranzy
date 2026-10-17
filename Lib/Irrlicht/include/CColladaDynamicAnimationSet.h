//-*-c++-*-
#ifndef __CCOLLADADYNAMICANIMATIONSET_H__
#define __CCOLLADADYNAMICANIMATIONSET_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaSceneNodeAnimatorSet.h"

namespace irr
{
namespace collada
{

class CDynamicAnimationSet
	: public CAnimationSet
{
protected:

	bool					NeedsCompilation;
	core::array<SChannel>	ChannelsCopy;			//Use copy for channel instead of reference to collada database data that could be unloaded

	int			remAnimation(const SAnimation* animation);
	int			addAnimation(const SAnimation* animation);
	int			getDatabaseIndex(CColladaDatabase& database);
	SChannel&	getChannel(u32 index);
	const SChannel&	getChannel(u32 index) const;

public:

	CDynamicAnimationSet() 
		: NeedsCompilation(true)
	{}

	bool needsCompilation() const
	{
		return NeedsCompilation;
	}

	void clearTracks();
	void clearSet();

	int		addAnimationLibrary(const CColladaDatabase& database);
	void	remAnimationLibrary(CColladaDatabase& database);
	void	remAnimationLibrary(u32 index);

	void	compile();

	//! The add/rem bindings function doesn't require a recompile of the animation set.
	//	They however, don't modify the actual number of channel the set can animate.
	//	Adding animation through this function when the set is already empty will thus fail

	void addAnimationLibraryBindings(const CColladaDatabase& database);

	void remAnimationLibraryBindings(CColladaDatabase& pDatabase);
	void remAnimationLibraryBindings(u32 index);

	void overwriteAnimationLibraryBindings(const CColladaDatabase& replace, CColladaDatabase& find);
	void overwriteAnimationLibraryBindings(const CColladaDatabase& database, u32 index);
};

} // end namespace collada
} // end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif //__CCOLLADADYNAMICANIMATIONSET_H__

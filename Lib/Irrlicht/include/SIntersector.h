//-*-c++-*-
#ifndef __S_INTERSECTOR_H_INCLUDED__
#define __S_INTERSECTOR_H_INCLUDED__

#include "SViewFrustum.h"

namespace irr
{
namespace scene
{

//!
struct SUniverseIntersector
{
	SUniverseIntersector()
	{
	}

	bool operator () (const core::aabbox3df& /*bbox*/) const
	{
		return true;
	}
};

//!
struct SBoxIntersector
{
	const core::aabbox3df& FrustumBBox;

	SBoxIntersector(const SViewFrustum& frustum)
		: FrustumBBox(frustum.getBoundingBox())
	{
	}

	bool operator () (const core::aabbox3df& bbox) const
	{
		return FrustumBBox.intersectsWithBox(bbox);
	}
};

//!
struct SFrustumBoxIntersector
{
	const SViewFrustum& Frustum;

	SFrustumBoxIntersector(const SViewFrustum& frustum)
		: Frustum(frustum)
	{
	}

	bool operator () (const core::aabbox3df& bbox) const
	{
		return Frustum.intersectsWithoutBoxTest(bbox);
	}
};

//!
struct SFrustumBoxIntersector3
{
	const SViewFrustum& Frustum;

	SFrustumBoxIntersector3(const SViewFrustum& frustum)
		: Frustum(frustum)
	{
	}

	bool operator () (const core::aabbox3df& bbox) const
	{
		return Frustum.intersectsWithoutBoxTest3(bbox);
	}
};

} // end namespace scene
} // end namespace irr

#endif

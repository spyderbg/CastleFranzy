//-*-c++-*-
#ifndef __IRR_CORE_GEOMETRY_UTIL_H_INCLUDED__
#define __IRR_CORE_GEOMETRY_UTIL_H_INCLUDED__

#include "aabbox3d.h"
#include "rect.h"
#include "coreutil.h"

namespace irr
{
namespace core
{

//!
template <typename T>
inline
void
computeBoundingBox(const vector3d<T>* pos,
				   u32 stride,
				   u32 count,
				   aabbox3d<T>& aabb)
{
	_IRR_DEBUG_BREAK_IF(pos == NULL || stride < sizeof(vector3d<T>));
	if (count > 0)
	{
		aabb.MinEdge = aabb.MaxEdge = *pos;
		pos = stepPointer(pos, stride);
		for (const vector3d<T>* end = stepPointer(pos, (count - 1) * stride);
			 pos != end;
			 pos = stepPointer(pos, stride))
		{
			if (pos->X < aabb.MinEdge.X)
			{
				aabb.MinEdge.X = pos->X;
			}
			if (pos->X > aabb.MaxEdge.X)
			{
				aabb.MaxEdge.X = pos->X;
			}
			if (pos->Y < aabb.MinEdge.Y)
			{
				aabb.MinEdge.Y = pos->Y;
			}
			if (pos->Y > aabb.MaxEdge.Y)
			{
				aabb.MaxEdge.Y = pos->Y;
			}
			if (pos->Z < aabb.MinEdge.Z)
			{
				aabb.MinEdge.Z = pos->Z;
			}
			if (pos->Z > aabb.MaxEdge.Z)
			{
				aabb.MaxEdge.Z = pos->Z;
			}
		}
	}
	else
	{
		aabb.MinEdge = aabb.MaxEdge = core::vector3d<T>();
	}
}

//!
template <typename T>
inline
aabbox3d<T>
computeBoundingBox(const vector3d<T>* pos,
				   u32 stride,
				   u32 count)
{
	aabbox3d<T> aabb;
	computeBoundingBox(pos, stride, count, aabb);
	return aabb;
}

//!
template <typename T>
inline
void
computeBoundingRect(const position2d<T>* pos,
					u32 stride,
					u32 count,
					rect<T>& aabb)
{
	_IRR_DEBUG_BREAK_IF(pos == NULL || stride < sizeof(vector2d<T>));
	if (count > 0)
	{
		aabb.UpperLeftCorner = aabb.LowerRightCorner = *pos;
		pos = stepPointer(pos, stride);
		for (const position2d<T>* end = stepPointer(pos, (count - 1) * stride);
			 pos != end;
			 pos = stepPointer(pos, stride))
		{
			if (pos->X < aabb.UpperLeftCorner.X)
			{
				aabb.UpperLeftCorner.X = pos->X;
			}
			if (pos->X > aabb.LowerRightCorner.X)
			{
				aabb.LowerRightCorner.X = pos->X;
			}
			if (pos->Y < aabb.UpperLeftCorner.Y)
			{
				aabb.UpperLeftCorner.Y = pos->Y;
			}
			if (pos->Y > aabb.LowerRightCorner.Y)
			{
				aabb.LowerRightCorner.Y = pos->Y;
			}
		}
	}
	else
	{
		aabb.UpperLeftCorner = aabb.LowerRightCorner = core::position2d<T>();
	}
}

//!
template <typename T>
inline
rect<T>
computeBoundingRect(const position2d<T>* pos,
					u32 stride,
					u32 count)
{
	rect<T> aabb;
	computeBoundingRect(pos, stride, count, aabb);
	return aabb;
}

} // end namespace core
} // end namespace irr

#endif

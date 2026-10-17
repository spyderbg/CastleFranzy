#ifndef __IRR_PARTICLE_H__
#define __IRR_PARTICLE_H__

#include "vector3d.h"
#include "vector2d.h"
#include "SColor.h"

namespace irr
{
namespace ps
{
	//! Struct for holding one particle data
	struct SParticle
	{
		SParticle()
			: color(video::SColor(255, 255, 255, 255))
			, animOffset(0.0f)
			, animLength(0.0f)
			, uTransform(1.0, 0.0, 0.0)
			, vTransform(0.0, 1.0, 0.0)
		{
		}

		//! Position of the particle
		core::vector3df position;

		//! Direction and speed of the particle
		core::vector3df velocity;

		//! Current color of the particle, UVoffset and anim offset and length of material animation
		video::SColor	color;

		//! Texture UV Transform
		core::vector3df uTransform;
		core::vector3df vTransform;

		f32				animOffset;
		f32				animLength;

		//! Current age of the particle
		f32				age;
		f32				life;

		//! Size of the particle
		f32				size;
		f32				targetSize;

		//! Spin angle of the particle in radians
		f32				spin;

		core::vector2df transformUV(const core::vector2df &uv)
		{
			core::vector2df newUV;
			newUV.X = uTransform.X * uv.X + uTransform.Y * uv.Y + uTransform.Z;
			newUV.Y = vTransform.X * uv.X + vTransform.Y * uv.Y + vTransform.Z;
			return newUV;
		}
	};

} // end namespace ps
} // end namespace irr

#endif //__IRR_PARTICLE_H__